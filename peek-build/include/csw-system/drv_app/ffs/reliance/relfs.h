/*-----------------------------------------------------------------------------
|  Project :  
|  Module  :  RELIANCE
+------------------------------------------------------------------------------
|             Copyright 2003 Texas Instruments.
|             All rights reserved. 
| 
|             This file is confidential and a trade secret of Texas 
|             Instruments .
|             The receipt of or possession of this file does not convey 
|             any rights to reproduce or disclose its contents or to 
|             manufacture, use, or sell anything it may describe, in 
|             whole, or in part, without the specific written consent of 
|             Texas Instruments. 
+------------------------------------------------------------------------------
| Purpose:    Definitions for the Protocol Stack Entity reliance.
+----------------------------------------------------------------------------*/

#ifndef _RELFS_H_
#define _RELFS_H_
#ifndef TARGET
#include "ffs.cfg"
#endif

#if (TARGET == 0) 
#include <stdlib.h>
#endif

#include <string.h>
#include <limits.h>
#include "ffs/ffs.h"
#include "rfs/fscore_types.h"
#include "rtc/rtc_api.h"


#ifdef WCP_PROF
#include "prf/prf_api.h"
#endif

#define OS_FREE(addr) rvf_free_buf(addr)
#define OS_MAIL_WAIT(event) rvf_wait(event, 0)
#define OS_MAIL_READ() rvf_read_mbox(RVF_TASK_MBOX_0)
#define OS_MAIL_SEND(dest, data) rvf_send_msg(dest, data);
#define OS_DELAY(time) rvf_delay(time)



#define RELFS_GETBUF(s,p) {if (rvf_get_buf (ril_env_ctrl_blk_p->mbid, \
                                s, (T_RVF_BUFFER **) &p) == RVF_RED) \
                            { p =NULL; \
                             /* Dont return */ } }


#define RELFS_MSG_GETBUF(s,p)  { if ((rvf_get_buf(ril_env_ctrl_blk_p->mbid, s, \
                                         (T_RVF_BUFFER*) &p)) == RVF_RED) { \
                                    p=NULL;        } \
								}


                             
#define RELFS_FREE_BUF(buf_p)   {if(buf_p) rvf_free_buf(buf_p);(buf_p)=NULL;}


typedef enum
{
	REL_OK		= 0,
	REL_ERR		= -50,
	REL_MEMORY_ERR 	= -51,
	REL_INVNAME_ERR = -52
}ERR_MSG;	


int32 map_ril_error(int32 fd);
int ril_disk_reclaim(void);
/*int32 ril_ffs_remove_nb(const int8 *pathname, T_FFS_return_path *cp);*/

/* Macros for Request Ids for Non-blocking functions */
#define REL_REQID_LBOUND				(1)
#define REL_REQID_UBOUND				(REL_REQID_LBOUND+50)

struct versioninfo
{
    unsigned int       ulVersion;	   /*  = VERSIONVAL */
    unsigned int       ulBuildNumber;      /*  = RELBUILDNUM */
    unsigned int       iTfsLayoutVersion;  /*  = TFS_LAYOUT_VERSION */
    unsigned int       ulDebug;   	   /*  = TFS_DEBUG */
    char         szVersionStr[100];  	   /*  = VERSIONNUM */
};

  
typedef enum
{
	REL_OPEN_NB_REQ = 1,
	REL_CLOSE_NB_REQ,
	REL_WRITE_NB_REQ,
	REL_READ_NB_REQ,
	REL_SEEK_NB_REQ,
	REL_MKDIR_NB_REQ,
	REL_REMOVE_NB_REQ,
	REL_RMDIR_NB_REQ,
	REL_TRUNC_NB_REQ,
	REL_TRUNC_NAME_NB_REQ,
	REL_FLUSH_NB_REQ,
	REL_LINK_NB_REQ,
	REL_PREFORMAT_NB_REQ,
	REL_FORMAT_NB_REQ,
	REL_CREAT_NB_REQ,
	REL_RENAME_NB_REQ,
	REL_OPENDIR_NB_REQ,
	REL_OPENDIR_RFS_NB_REQ,
	REL_READDIR_NB_REQ,
	REL_READDIR_RFS_NB_REQ,
	REL_CLOSEDIR_NB_REQ,
	REL_CLOSEDIR_RFS_NB_REQ,
	REL_FSTAT_NB_REQ,
	REL_STAT_NB_REQ,
	REL_FSTAT_RFS_NB_REQ,
	REL_FCHMOD_RFS_NB_REQ,
	REL_STAT_RFS_NB_REQ,
	REL_CHMOD_RFS_NB_REQ,
	REL_GET_ATTRIB_RFS_REQ,
	REL_SET_ATTRIB_RFS_REQ,
	REL_GET_DATETIME_RFS_REQ,
	REL_SETPART_RFS_NB_REQ,
    REL_START_GC_REQ
}RIL_MSG_ID;

#define REL_API_VERSION 	0x0001
#define REL_DRV_VERSION 	0x0001
#define REL_REVISION 		0x0001
#define REL_FORMAT_VERSION 	0x0001
#define REL_TM_VERSION 		0x0001

enum {
	Rel_req,
	FFS_STREAM_CNF,
	FFS_FILE_CNF	
};

typedef struct 
{
	T_RV_HDR   		header;
	unsigned int	msg;
	int32 			fd ;
	int32			req_id;
	T_RV_RETURN 	*cp;
	const char *	actualpath;
	const char *	pathname;
	ffs_options_t 	option;
	unsigned short 	uMode;
	int32			offset;
	int32 			origin;
	int32			length;
	int32			size;
	char *			pBuf;
	uint16			magic;
	void*			ptr_data;
	char 			unicode;
	
}T_Rel_req;

/*Datlight FFX related function prototype */
extern int FilesytemGetVersion (struct versioninfo * );

extern T_FSCORE_RET relfs_get_func_table (T_FSCORE_FUNC_TABLE * func_table_p);
extern int32 ffs_map_ril_error(T_FFS_FD fd);
extern int relfs_map_attr_rfs(int attr);

extern  T_FFS_RET ffs_getdatetime(const char *pathname,T_RTC_DATE_TIME * C_date_time_p,T_RTC_DATE_TIME * A_date_time_p,T_RTC_DATE_TIME * M_date_time_p);

#endif  //_RELFS_H_
