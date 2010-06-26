/******************************************************************************
 * Flash File System (lfs)
 * Idea, design and coding by Kim T. Petersen, ktp@ti.com
 *
 * ffs public API functions
 *
 * 
 *
 ******************************************************************************/

#include "ffs/ffs.h"

#if (LINEAR_FILE_SYSTEM)
#ifndef TARGET
#include "ffs.cfg"
#endif


#if (TARGET == 1)
#include "etm/etm.h"
#include "etm/etm_api.h"
#include "ffs/board/task.h"
#include "ffs/board/tmlfs.h"
#endif

#include "ffs/board/core.h"
#include "ffs/board/ffstrace.h"

#include <string.h>

#define TMLFS_VERSION 1

/******************************************************************************
 *Globals, Marcos and prototypes
 ******************************************************************************/

static int32 bufsize, tmpsize;
static uint8 stringsize;

#define tmlfs_put8(x)  *outp++ = x;
#define tmlfs_put16(x) *outp++ = (x & 0xff); *outp++ = (x>>8);


#define tmlfs_get8() inp[0]; inp += 1; 
#define tmlfs_get16() (inp[0]) | (inp[1] << 8); inp += 2; 
#define tmlfs_get32()  inp[0] | (inp[1] << 8) | (inp[2] << 16)\
                       | (inp[3] << 24); inp += 4; 

#define tmlfs_getdata()    bufsize = inp[0]; inp += 1; \
                           memcpy(buffer, inp, bufsize); inp += bufsize;


/******************************************************************************
 * LFS protocol  
 ******************************************************************************/

#define TMLFS_BUFFER_SIZE 240   // FIXME change to packet size
#define TMLFS_STRING_SIZE 127


//#if (TARGET == 1)
int etm_lfs(T_ETM_PKT *pkt, unsigned char *inp, int insize);

/******************************************************************************
 * TM LFS registration to ETM database
 *****************************************************************************/

/* Callback function registered in ETM database */
int etm_lfs_pkt_receive(uint8 *data, int size)
{
    int status;
    T_ETM_PKT *pkt = NULL;  

    ttw(ttr(TTrTmlfs, "etm_lfs_pkt_receive(*, %d)" NL, size)); 

    /* Create TestMode return Packet */
    if ((pkt = (T_ETM_PKT *) target_malloc(sizeof(T_ETM_PKT))) == NULL) {
        ttw(ttr(TTrTmlfs, "etm_lfs_pkt_receive(): Limit of memory bank reached" NL)); 
        return ETM_NOMEM;
    } 
    
    status = etm_lfs(pkt, data, size);
    return status;
}

/* Init of FFS in the ETM database */
int etm_lfs_init(void)
{
    int status;
    
    status = etm_register("LFS", ETM_LFS, 0, 0, etm_lfs_pkt_receive);

    return status;
} 
//#endif // (TARGET == 1)


/******************************************************************************
 * Helper function
 ******************************************************************************/

// If size is less than zero it is because of a error and we dont have to put any
// data if size is returned in status.
int tmlfs_putdata(unsigned char **buf, unsigned char *src, int size) 
{
 	unsigned char *p = *buf;

	if (size > 0) {
		*p++ = size;
		memcpy(p, src, size); 
		*buf += 1 + size;
	}
	return size;
}

int tmlfs_putstring(unsigned char **buf, char *src, int size) 
{
	unsigned char *p = *buf;

	if (size > 0) {
		*p++ = size;
		memcpy(p, src, size); 
		*buf += 1 + size;
	}
	return size;
}

int tmlfs_getstring(unsigned char ** buf, char *string)
{
	unsigned char *p = *buf;
	
	stringsize = *p++;

	if (stringsize > TMLFS_STRING_SIZE) 
		return EFFS_TOOBIG;

	memcpy(string, p, stringsize);
	*buf += 1 + stringsize;

	return stringsize;
}

/******************************************************************************
 * tm_lfs
 ******************************************************************************/

// Parse input message and execute function. Then fill output buffer with
// return values from the called function and transmit the message.  Return
// number of bytes inserted into output buffer. If return value is negative,
// it represents an error code.
int etm_lfs(T_ETM_PKT *pkt, unsigned char *inp, int insize)
{
	tmlfs_fid_t fid;
	unsigned char buffer[TMLFS_BUFFER_SIZE];
	char string[TMLFS_STRING_SIZE];

    unsigned char *outp_start;
    unsigned char *inp_start  = inp;
	unsigned char *outp;
    char *data_addr = NULL;

	int error = 0, i, fdi, size, param, flags, total_read = 0;
    int offset;
	uint8 type;

	bufsize = stringsize = tmpsize = 0;

	tw(tr(TR_BEGIN, TrTmlfs, "TmLFS\n"));
    
	outp_start = outp = pkt->data;
	
	fid = *inp++;
	ttw(ttr(TTrTmlfs, "etm_lfs 0x%x" NL, fid));
	switch(fid)
	{
		/**********************************************************
		 * Generic Protocol Functions
		 **********************************************************/

	case TMLFS_VERSION:
		tmlfs_put16(TMLFS_VERSION);
		break;

		/**********************************************************
		 * LFS Functions
         **********************************************************/

	case TMLFS_OPEN:
		error = tmlfs_getstring(&inp, string); // Get pathname 'string'
		flags = tmlfs_get8(); // Creat, read or write flag
        size  = tmlfs_get16() // Get size of file
		if (error >= 0)
			error = lfs_open(string, flags, &size);
		tmlfs_put8(error); // fdi
		tw(tr(TR_FUNC, TrTmlfs, "TMLFS_OPEN('%s', %d)\n", string, flags));
		ttw(ttr(TTrTmlfs, "tm_open" NL));
		break;
	case TMLFS_CLOSE:
		fdi = tmlfs_get8(); 
		error = lfs_close(fdi);
		tw(tr(TR_FUNC, TrTmlfs, "TMLFS_CLOSE(%d)\n", fdi));
		ttw(ttr(TTrTmlfs, "tm_close" NL));
		break;
	case TMLFS_WRITE:
		fdi = tmlfs_get8(); 
		tmlfs_getdata();   // copy inputdata to buffer[]
		error = lfs_write(fdi, buffer, bufsize);
		tmlfs_put8(error); // put written size
		tw(tr(TR_FUNC, TrTmlfs, "TMLFS_WRITE(%d, %d)\n", fdi, bufsize));
		ttw(ttr(TTrTmlfs, "tm_write" NL));
		break;
	case TMLFS_READ:
		fdi   = tmlfs_get8(); 
		size  = tmlfs_get8();   // max size is 240 bytes
        offset = tmlfs_get16(); // Get pointer offset
		error = lfs_read(fdi, &data_addr); 
        if ((error != EFFS_OK) && (data_addr != NULL))
            break;
        error = tmlfs_putdata(&outp, (data_addr + offset), size);
		tw(tr(TR_FUNC, TrTmlfs, "TMLFS_READ(%d, %d)\n", fdi, size));
		ttw(ttr(TTrTmlfs, "tm_read" NL));
		break;
	case TMLFS_STAT:
		error = tmlfs_getstring(&inp, string);
		if (error >= 0) 
			error = ffs_stat(string, (struct stat_s *) buffer); // Use ffs stat func.
		if (error >= 0)
			tmffs_putdata(&outp, buffer, sizeof(struct stat_s));
		
		tw(tr(TR_FUNC, TrTmffs, "TMLFS_STAT()\n"));
		ttw(ttr(TTrTmffs, "tm_st" NL));
		break;
	case TMLFS_REMOVE:
		error = tmlfs_getstring(&inp, string);
		if (error >= 0)
			error = lfs_remove(string);
		tw(tr(TR_FUNC, TrTmlfs, "TMLFS_REMOVE()\n"));
		ttw(ttr(TTrTmlfs, "tm_rm" NL));
		break;
	default:
		error = ELFS_NOSYS;
		tmlfs_put8(ELFS_NOSYS);
		tw(tr(TR_FUNC, TrTmlfs, "ERROR: Unknown tmlfs protocol code\n"));
	 	ttw(ttr(TTrTmlfs, "tm?" NL));
		break;
	}
	
	// check if we read beyond buffer end
	if (inp > inp_start + insize) {
		tw(tr(TR_FUNC, TrTmlfs, "ERROR: Read beyond end of input buffer\n"));
		ttw(ttr(TTrTmlfs, "tm_fatal" NL));
		ttw(ttr(TTrTmlfs, "insize: %d, diff: %d" NL, insize, 
				inp - (inp_start + insize)));
		// NOTEME: We really should reset output buffer and put a return
		// code that tells us what went wrong!
		error = ETM_PACKET;  // FIXME find another error 
	}

	ttw(ttr(TTrTmlfs, "error %d" NL, error));
	if (error > 0)
		error = 0;

    pkt->mid    = ETM_LFS;
	pkt->size   = outp - outp_start;
    pkt->status = -error;

    etm_pkt_send(pkt);
    etm_free(pkt);

    tw(tr(TR_END, TrTmlfs, ""));

    return ETM_OK;
}

#endif // LINEAR_FILE_SYSTEM
