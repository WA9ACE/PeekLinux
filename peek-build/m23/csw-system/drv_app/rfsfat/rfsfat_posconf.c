/**
 * @file  rfsfat_posconf.c
 *
 *  In this file, the adaptation layer between Posix and the FAT lib is
 *  implemented.
 *
 * @author  Anton van Breemen
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  03/15/2004  Anton van Breemen   Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */


/******************************************************************************
 *     Includes                                                               *
 ******************************************************************************/


#include "rv/rv_general.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include <string.h>
#include <ctype.h>
#include "rfsfat/rfsfat_ffs.h"
#include "gbi/gbi_api.h"
#include "rfs/rfs_api.h"
#include "rfs/fscore_types.h"
#include "rfsfat/rfsfat_posconf.h"

#include "rfsfat/rfsfat_api.h"
#include "rfsfat/rfsfat_i.h"
#include "rfsfat/rfsfat_dir.h"
#include "rfsfat/rfsfat_mma.h"
#include "rfsfat/rfsfat_fam.h"
#include "rfsfat/rfsfat_ffs_disk.h"
#include "rfsfat/rfsfat_ffs_dman.h"
#include "rfsfat/rfsfat_ffs_fman.h"
#include "rfsfat/rfsfat_mem_operations.h"
#include "rfsfat/rfsfat_posconf.h"
#include "rfsfat/rfsfat_ffs_file.h"
#include "rfsfat_ffs.h"
/******************************************************************************
 *      Module specific Defines and enums                                     *
 ******************************************************************************/
#ifndef FFS_FAT_H
#error "ERROR: The file ffs.h must be included before this file."
#endif

#define RFSFAT_FORMAT_MAGIC         0x2BAD
#define RFSFAT_START_OF_FILE        0
#define RFSFAT_ADD_EOF              1
/******************************************************************************
 *      Static-Function prototypes                                            *
 ******************************************************************************/
#ifdef FFS_FEATURE_DIRECTORIES
static UINT8 ucCheckIfSubdirOfDir
(
  DIR_DIRECTORY_LABEL_STRUCT * pstMetDatMainDir,
  DIR_LABEL_LOCATION_STRUCT * pstLabLocSubDir
);
#endif

#if WCP_PROF == 1
  extern void prf_LogPointOfInterest(const char*);
#endif

//UINT8 convertfat_datetime_to_local(UINT16 date, UINT16 time, T_RTC_DATE_TIME* date_time_p);
#define RFSFAT_VALID_PERMS  (FSCORE_IRWXU | FSCORE_IRWXG | FSCORE_IRWXO)

static const char* separateFileAndPath(char *pathOnly, const char *pathname);


static int convert_FATfstype_string(T_GBI_FS_TYPE fs_type, char *fs_name);
static int convert_mediatype_string(T_GBI_MEDIA_TYPE media_type, char *media_name);

#if (FFS_UNICODE == 1)

/**************************************************/
// Unicode
/**************************************************/


#if 0
static UINT8 determinePartitionIndex_uc(UINT8 *pucPath, UINT8 *mp_idx_p);
static UINT8 determine_partition_uc(const T_WCHAR *mpAndpath,UINT8          *mp_idx_p);
#endif



static UINT8 separateMountpointFromPath_uc(
                          const T_WCHAR *mpAndpath,
                                T_WCHAR **path,
                                UINT8          *mp_idx_p);

static T_WCHAR* separateFileFromPath_uc(const  T_WCHAR *pathAndFile_p,
                                           T_WCHAR *pathN_p);
/**************************************************/
// end of Unicode
/**************************************************/

#endif



INT8 convertfat_datetime_to_local(UINT16 date, UINT16 time, T_RTC_DATE_TIME* date_time_p)
{

 if(date_time_p == NULL)
 		return -1;

  /*
       Time integer:
       MSB %HHHH.HMMM.MMMS.SSSS   LSB
       H = hours
       M = Minutes
       S = Seconds in 2-second steps.
     */
 date_time_p->second = time&0x001F;
 date_time_p->minute  = ((time&0x07E0)>>5);
 date_time_p->hour	  = ((time&0xF800)>>11);


  /*
       Date integer:
       MSB %YYYY.YYYM.MMMD.DDDD   LSB
       Y = Year
       M = Month
       D = Day
     */

  date_time_p->day   = date&0x001F;
  date_time_p->month = ((date&0x01E0)>>5);
  date_time_p->year  = ((date&0xFE00)>>9);

  if(date_time_p->year >= 20)
  	{
  	    /* The year stored in FAT table entry , is from 1980, but RTC needs it from 2000 */
  		date_time_p->year -= 20;
  	}

  return 0;

}






/**
 *  POSIX entry for file or directory remove
 */
T_FSCORE_RET  rfsfat_ffs_file_remove(T_RV_HDR *inmsg_p)
{
  UINT8                     status = FFS_NO_ERRORS;
  UINT8                     status2;
  T_RFSFAT_REMOVE_REQ_MSG   *msg_p;
  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  msg_p = (T_RFSFAT_REMOVE_REQ_MSG*) inmsg_p;
  if (msg_p->pathname == NULL) {
      status = FFS_RV_ERR_INVALID_PATH;
    }
  else
    {
#if 0
      pathonly_p = &pathonly[0];

      //separate file name and path (partition name+directories) from pathname
      nameAndExt_p = separateFileAndPath(pathonly_p, (const char*)msg_p->pathname);

      if (((*nameAndExt_p == FFS_ASCII_VALUE_FWSLASH) &&
          (*nameAndExt_p + 1 == FFS_ASCII_EOL)) ||
          (*nameAndExt_p == FFS_ASCII_EOL)) {

        status = E_FFS_ucDmanRmDir((UINT8*)msg_p->pathname);
      } else {
        status = E_FFS_ucFmanUnlink((UINT8*)msg_p->pathname);
        status2 = flush_write_blk();
        if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS)) {

          status = status2;
        }
      }
#endif
 /* Get directory label.                                                     */
  ucResult = ffs_ucGetLabel((RW_PTR_TO_RO(UINT8)) msg_p->pathname, &stLabLoc,&stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ffs_ucGetLabel failed");
    status=ucResult;
  }
  else
  	{

  /* Check if attribute bit 'directory' nor 'partition' is set.                  */
  if (ATTR_CHECK_SUBDIR((&stMetDat)))
  {
    /* element is not file */
    status = E_FFS_ucDmanRmDir((UINT8*)msg_p->pathname);
    status2 = flush_write_blk();
    if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS)) {
         status = status2;
        }

  }else {
        status = E_FFS_ucFmanUnlink((UINT8*)msg_p->pathname);
        status2 = flush_write_blk();
        if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS)) {
          status = status2;
        }
      }
    if(status == FFS_NO_ERRORS)
    	{
		#ifdef ICT_SPEEDUP_RFSFAT2
  		ucFlushFamCash(stLabLoc.mpt_idx);
		#endif //ICT_SPEEDUP_RFSFAT2

		kill_gbi_readcash();
    	}
      }
  }    /* end of if(msg_p->pathname)*/
  return rfsfat_send_response_msg( FSCORE_REMOVE_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status, FSCORE_EOK);
}

/**
 *  POSIX entry for the fstat function (status info on open file) */
T_FSCORE_RET  rfsfat_ffs_fman_chmod(T_RV_HDR *inmsg_p)
{
  UINT8                   status = FFS_NO_ERRORS;
  T_RFSFAT_CHMOD_REQ_MSG  *msg_p;
  T_FSCORE_RET            ret_value = FSCORE_EOK;


  msg_p = (T_RFSFAT_CHMOD_REQ_MSG*) inmsg_p;

  /* Just return success, All chmod parameters should work because FAT file system does not have
     groups, super user*/

  return rfsfat_send_response_msg( FSCORE_CHMOD_RSP,
                                    msg_p->pair_value,
                                    &(msg_p->return_path),
                                    status,
                                    ret_value);
}
/**
 *  This function separates the file name from the path info.
 *  @param pathOnly   Output: pointer to buffer where path without filename
 *                    will be copied to. Path is null string on error.
 *  @param pathname   Input: pointer to null terminated path string
 *                    (incl. file name).
 *
 *  @return pointer to file name. Full original path on error.
 */
static const char* separateFileAndPath(char *pathOnly, const char *pathname)
{
  UINT16  i;

  i = strlen(pathname);
  while(pathname[i] != RFSFAT_SEPARATOR)
  {
    i--;
    if ((i==0) && (pathname[i] != RFSFAT_SEPARATOR))
    {	//No '/' found: empty path
		  pathOnly[0]= 0;
  		return pathname;   //point to first char of file name
  	}
  }

  strncpy(pathOnly, pathname, ++i);
/*
  strncpy(pathOnly, pathname,  (unsigned int)i);  //copy pathname to pathOnly
*/
  pathOnly[i]= 0;
  return &pathname[i];   //point to first char of file name
}


/**
 *  POSIX entry for file open
 *  The mode msg parameter is ignored because FAT does not support this
 */
T_FSCORE_RET  rfsfat_ffs_file_open(T_RV_HDR *inmsg_p)
{
  char                    pathonly[255];
  char                    *pathonly_p;
  const char              *nameAndExt_p;
  UINT8                   status = FFS_NO_ERRORS;
  UINT8                   status2;
  UINT8                   ucAccessMode;
  UINT8                   handle;
  UINT8                   i;
  T_FSCORE_FD             fd = 0;
  T_RFSFAT_OPEN_REQ_MSG   *msg_p;
  BOOL                    flag_create = FALSE;
  const T_FSCORE_FLAGS    valid_flags = FSCORE_O_RDONLY | FSCORE_O_WRONLY | FSCORE_O_RDWR | FSCORE_O_CREATE | FSCORE_O_APPEND | FSCORE_O_TRUNC | FSCORE_O_EXCL;


    msg_p = (T_RFSFAT_OPEN_REQ_MSG*) inmsg_p;

  /* parameter check */
  if (msg_p->pathname == NULL) {
	status = FFS_RV_ERR_INVALID_PATH;
  }

  /* check for other non existing flags */
  if ((status==FFS_NO_ERRORS) && (msg_p->flags & (~valid_flags)))
  	{
  	  status = FFS_RV_ERR_INVALID_PARAMS;
	 }

if(status == FFS_NO_ERRORS)
{

  if(((msg_p->flags&FSCORE_O_WRONLY) == FSCORE_O_WRONLY)||((msg_p->flags&FSCORE_O_RDWR) == FSCORE_O_RDWR))
  	{
  	      status = FFS_NO_ERRORS;
   	}
  else
  if(msg_p->flags == FSCORE_O_RDONLY)
  	{
  	    status = FFS_NO_ERRORS;
  	}
  else
  	   status = FFS_RV_ERR_INVALID_PARAMS;
}

  if (status != FFS_NO_ERRORS) {
    return rfsfat_send_response_msg( FSCORE_OPEN_RSP,
                              msg_p->pair_value,
                              &(msg_p->return_path),
                              status,
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  }

  pathonly_p = &pathonly[0];
  //separate file name and path (partition name+directories) from pathname
  nameAndExt_p = separateFileAndPath(pathonly_p, (const char*)msg_p->pathname);

  //file does not exist, may we create it?
  if ((msg_p->flags & FSCORE_O_CREATE) == FSCORE_O_CREATE)
  {
    flag_create = TRUE;
  }
  status = E_FFS_ucFileCreate((UINT8*)pathonly, (UINT8*)nameAndExt_p, flag_create);
  if ((status != FFS_NO_ERRORS) && (status != FFS_RV_ERR_FILE_ALREADY_EXISTS))
  {
    //Error, failed to create new file.
    return rfsfat_send_response_msg( FSCORE_OPEN_RSP,
                              msg_p->pair_value,
                              &(msg_p->return_path),
                              status,
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  }



  if((status == FFS_RV_ERR_FILE_ALREADY_EXISTS) && \
  ((msg_p->flags&(FSCORE_O_CREATE|FSCORE_O_EXCL)) == (FSCORE_O_CREATE|FSCORE_O_EXCL)))
  	{
  	return rfsfat_send_response_msg( FSCORE_OPEN_RSP,
                              msg_p->pair_value,
                              &(msg_p->return_path),
                              status,
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  	}

#if 0  /* optimization */
  status2 = flush_write_blk();
  if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS))
  {
    status = status2;
  }
#endif
  //At this point, file does either exist or is created

  /* FAT does not support write only, this will be set to readwrite. */
  if ((msg_p->flags &
        (FSCORE_O_RDONLY | FSCORE_O_WRONLY | FSCORE_O_RDWR)) == FSCORE_O_RDONLY)
  {
    ucAccessMode = FFS_OPEN_MODE_READ;
  } else
  {
    ucAccessMode = FFS_OPEN_MODE_READWRITE;
  }

  /* FAT does not support access mode: ignored */
  status = E_FFS_ucFileOpen((UINT8*)msg_p->pathname, ucAccessMode, msg_p->flags, &handle);
  if (status == FFS_NO_ERRORS)
  {
    //fd bookkeeping
    fd = rfsfat_env_ctrl_blk_p->ffs_globals.fd_rr;
    fd = (fd+1) & 0x7F; //Only positive range of INT8 (its later shift left by 8)
    if (fd == 0)  //(skip 0)
      fd = 1;
    rfsfat_env_ctrl_blk_p->ffs_globals.fd_rr = (UINT8)fd;
    fd = (fd << 8) | handle;
    for (i = 0; (i < FSCORE_MAX_NR_OPEN_FSCORE_FILES); i++)
    {
      if (rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[i].fd == RFS_FD_DEF_VALUE)
      {
        rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[i].fd = fd;
        //bugfix set opmode to default value
        rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[i].op_mode = O_SIO;
        break;
      }
    }
	if(i == FSCORE_MAX_NR_OPEN_FSCORE_FILES)
		{
		   status = FFS_RV_ERR_TO_MUCH_OPEN_FILES;
  }

  	}
  return rfsfat_send_response_msg( FSCORE_OPEN_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status,(T_FSCORE_RET)fd);

}

/**
 *  POSIX entry for file read
 */
T_FSCORE_RET  rfsfat_ffs_file_read(T_RV_HDR *inmsg_p)
{
  UINT8                   status;
  T_RFSFAT_READ_REQ_MSG   *msg_p;
  FFS_FILE_LENGTH_TYPE    size;
  T_FSCORE_PAIR_VALUE     pair_value;
  T_FSCORE_RET            ret_value = (T_FSCORE_RET)0;


  msg_p = (T_RFSFAT_READ_REQ_MSG*) inmsg_p;

  pair_value.fd = msg_p->fd;
  pair_value.pair_id = msg_p->pair_id;

	if ((msg_p->size < 0) || (msg_p->buf == NULL))
	{
	  return rfsfat_send_response_msg( FSCORE_READ_RSP,
	                            pair_value,
	                            &(msg_p->return_path),
	                            FFS_RV_ERR_INVALID_PARAMS,
	                            (T_FSCORE_RET)0);
	}

	if (msg_p->size == 0)
	{
	  return rfsfat_send_response_msg( FSCORE_READ_RSP,
	                            pair_value,
	                            &(msg_p->return_path),
	                            FFS_NO_ERRORS,
	                            (T_FSCORE_RET)0);
	}


  size = msg_p->size;
  status = E_FFS_ucFileRead((UINT8)msg_p->fd, ((FFS_FILE_LENGTH_TYPE*)(&size)), (UINT8*)msg_p->buf);
  if(status == FFS_NO_ERRORS)
  {
    ret_value = (T_FSCORE_RET)size;
  }


  #if WCP_PROF == 1
  prf_LogPointOfInterest("rfsfat_read_end");
 #endif

  return rfsfat_send_response_msg( FSCORE_READ_RSP,
                            pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}


/**
 *  POSIX entry for file fchmod */
T_FSCORE_RET  rfsfat_ffs_file_fchmod(T_RV_HDR *inmsg_p)
{
  T_RFSFAT_FCHMOD_REQ_MSG *msg_p;
  T_FSCORE_PAIR_VALUE     pair_value;
  UINT8                   status = FFS_NO_ERRORS;

  msg_p = (T_RFSFAT_FCHMOD_REQ_MSG*) inmsg_p;
  pair_value.fd = msg_p->fd;
  pair_value.pair_id = 0;

  // NO ACTION TO BE TAKEN
  // return EOK because the caller doesn't need to be stalled
  // However, the rfsfat doesn't support file change mode functionality
  return rfsfat_send_response_msg( FSCORE_FCHMOD_RSP,
                            pair_value,
                            &(msg_p->return_path),
                            status,
                            FSCORE_EOK);
}


/**
 *  POSIX entry for file write
 */
T_FSCORE_RET  rfsfat_ffs_file_write(T_RV_HDR *inmsg_p)
{
  UINT8                   status;
  T_RFSFAT_WRITE_REQ_MSG  *msg_p;
  FFS_FILE_LENGTH_TYPE    size;
  T_FSCORE_PAIR_VALUE     pair_value;
  T_FSCORE_RET            ret_value = (T_FSCORE_RET)0;

  msg_p = (T_RFSFAT_WRITE_REQ_MSG*) inmsg_p;

  pair_value.fd = msg_p->fd;
  pair_value.pair_id = msg_p->pair_id;

	if ((msg_p->size < 0) || (msg_p->buf == NULL))
	{
	  return rfsfat_send_response_msg( FSCORE_WRITE_RSP,
	                            pair_value,
	                            &(msg_p->return_path),
	                            FFS_RV_ERR_INVALID_PARAMS,
	                            (T_FSCORE_RET)0);
	}

	if (msg_p->size == 0)
	{
	  return rfsfat_send_response_msg( FSCORE_WRITE_RSP,
	                            pair_value,
	                            &(msg_p->return_path),
	                            FFS_NO_ERRORS,
	                            (T_FSCORE_RET)0);
	}

  size = msg_p->size;
  status = E_FFS_ucFileWrite((UINT8)msg_p->fd,  ((FFS_FILE_LENGTH_TYPE*)(&size)), (UINT8*)msg_p->buf);
  if (status == FFS_NO_ERRORS)
  {
    ret_value = (T_FSCORE_RET)size;
  }

  #if WCP_PROF == 1
   prf_LogPointOfInterest("rfsfat_write_end");
  #endif
  return rfsfat_send_response_msg( FSCORE_WRITE_RSP,
                            pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}


/**
 *  POSIX entry for file close
 */
T_FSCORE_RET  rfsfat_ffs_file_close(T_RV_HDR *inmsg_p)
{
  UINT8                   status;
  UINT8                   status2;
  UINT8                   i;
  T_RFSFAT_CLOSE_REQ_MSG  *msg_p;
  T_FSCORE_PAIR_VALUE     pair_value;
  T_FSCORE_RET            ret_value = FSCORE_EOK;

  msg_p = (T_RFSFAT_CLOSE_REQ_MSG*) inmsg_p;


  pair_value.fd = msg_p->fd;
  pair_value.pair_id = msg_p->pair_id;
  status = E_FFS_ucFileClose((UINT8)msg_p->fd);
  if (status == FFS_NO_ERRORS)
  {
    //fd bookkeeping. Remove from open list.
    for (i = 0; (i < FSCORE_MAX_NR_OPEN_FSCORE_FILES); i++)
    {
      if (rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[i].fd == msg_p->fd)
      {
        rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[i].fd = RFS_FD_DEF_VALUE;
        break;
      }
    }
   if(i == FSCORE_MAX_NR_OPEN_FSCORE_FILES)
   	{
   	           status=FFS_RV_ERR_TO_MUCH_OPEN_FILES;
   	}

  }


  status2 = flush_write_blk();
  if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS))
  {
    status = status2;
  }
  kill_gbi_readcash();
  return rfsfat_send_response_msg( FSCORE_CLOSE_RSP,
                            pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}

/**
 *  POSIX entry for the fstat function (status info on open file) */
T_FSCORE_RET  rfsfat_ffs_file_fstat(T_RV_HDR *inmsg_p)
{
  UINT8                   status = FFS_NO_ERRORS;
  T_RFSFAT_FSTAT_REQ_MSG  *msg_p;
  T_FSCORE_PAIR_VALUE     pair_value;
  T_FSCORE_RET            ret_value = FSCORE_EOK;
  DIR_DIRECTORY_LABEL_STRUCT labloc;
  UINT8                   mpt_idx=0;

  msg_p = (T_RFSFAT_FSTAT_REQ_MSG*) inmsg_p;
  pair_value.fd = msg_p->fd;
  pair_value.pair_id = msg_p->pair_id;
  memset(&labloc,0,sizeof(DIR_DIRECTORY_LABEL_STRUCT));

  if (msg_p->stat == NULL) {
	  status = FFS_RV_ERR_INVALID_PARAMS;
  } else {
    status = E_FFS_ucFileFstat((UINT8)msg_p->fd, &labloc);
    mpt_idx = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[(UINT8)msg_p->fd].\
              stLabLoc.mpt_idx;
  }
  if (status == FFS_NO_ERRORS) {

    msg_p->stat->file_dir.ino = (labloc.uiFirstClusterNumberHigh<<16) | labloc.uiFirstClusterNumber;
    msg_p->stat->file_dir.size = labloc.ulFileSize;

    msg_p->stat->file_dir.mtime = converttime(labloc.uiLastChangedTime,
                                              labloc.uiLastChangedDate);
    msg_p->stat->file_dir.ctime = converttime(labloc.crtTime,
                                              labloc.crtDate);

	msg_p->stat->file_dir.reserved=0;
    if((labloc.ucAttribute & 1) == 1) {

      //file is read only
      msg_p->stat->file_dir.mode = FSCORE_IRUSR;
    } else {
      //file is read write
      msg_p->stat->file_dir.mode = FSCORE_IRWXU;
    }



    kill_gbi_readcash();
  }
  //if status is NOT ok then the value of ret_value is not relevant
  //anymore. Only when status is OK the glue layer may return an error
  //using ret_value
  return rfsfat_send_response_msg( FSCORE_FSTAT_RSP,
                                    pair_value,
                                    &(msg_p->return_path),
                                    status,
                                    ret_value);
}

/**
 *  POSIX entry for the fsync function (sync non written data) */
T_FSCORE_RET  rfsfat_ffs_file_fsync(T_RV_HDR *inmsg_p)
{
  T_RFSFAT_FSYNC_REQ_MSG  *msg_p;
  T_FSCORE_PAIR_VALUE     pair_value;
  UINT8            ret_value = FSCORE_EOK;

  msg_p = (T_RFSFAT_FSYNC_REQ_MSG*) inmsg_p;

  pair_value.fd = msg_p->fd;
  pair_value.pair_id = msg_p->pair_id;

  ret_value =E_FFS_ucFileSync(msg_p->fd);
  
  return rfsfat_send_response_msg( FSCORE_FSYNC_RSP,
                                    pair_value,
                                    &(msg_p->return_path),
                                    ret_value,
                                    ret_value);
}

/**
 *  POSIX entry for moving file pointer lseek */
T_FSCORE_RET  rfsfat_ffs_file_lseek(T_RV_HDR *inmsg_p)
{
  UINT8                   status = FFS_NO_ERRORS;
  T_RFS_OFFSET            fp_offset=0;
  T_RFSFAT_LSEEK_REQ_MSG  *msg_p;
  T_FSCORE_PAIR_VALUE     pair_value;
  T_FSCORE_RET            ret_value = FSCORE_EOK;
  UINT8                   fd_idx;

  msg_p = (T_RFSFAT_LSEEK_REQ_MSG*) inmsg_p;
  fd_idx = (UINT8)msg_p->fd;
  pair_value.fd = msg_p->fd;
  pair_value.pair_id = msg_p->pair_id;
  if(msg_p->whence == RFS_SEEK_SET) {
    //the ofset is done from the absolute beginning of the file

    if(msg_p->offset < 0)
    {
      //a negative offset cannot work when whence is RFS_SEEK_SET
      status = FFS_RV_ERR_INVALID_OFFSET;
      RFSFAT_SEND_TRACE_ERROR("A negative offset is not allowed whith whence RFS_SEEK_SET");
    } else {
      fp_offset = msg_p->offset;
    }
  } else if(msg_p->whence == RFS_SEEK_CUR) {
    //offset from current position

    fp_offset = (msg_p->offset + rfsfat_env_ctrl_blk_p->ffs_globals.\
      ffs_astOpenFileList[fd_idx].tFilePointer);
    if(fp_offset < 0)
    {
      //a negative offset cannot work
      RFSFAT_SEND_TRACE_ERROR("the negative offset whith whence RFS_SEEK_CUR is to large");
      status = FFS_RV_ERR_INVALID_OFFSET;
    }
    //if the offset is to large in positive directionit will be discovered in E_FFS_ucFileLSeek
  } else if(msg_p->whence == RFS_SEEK_END){

    if(msg_p->offset > 0)
    {
      //the offset points after the file end...
      RFSFAT_SEND_TRACE_ERROR("offset is to large, it may not exceed filelength");
      status = FFS_RV_ERR_INVALID_OFFSET;
    } else {
      fp_offset = (msg_p->offset + rfsfat_env_ctrl_blk_p->ffs_globals.\
                  ffs_astOpenFileList[fd_idx].tFileSize);
      if(fp_offset < 0)
      {
        //a negative offset cannot work
        RFSFAT_SEND_TRACE_ERROR("the negative offset whith whence RFS_SEEK_END is to large");
        status = FFS_RV_ERR_INVALID_OFFSET;
      }
    }
  } else status = FFS_RV_ERR_INVALID_PARAMS;

  if(status == FFS_NO_ERRORS) {

    status = E_FFS_ucFileLSeek(fd_idx, fp_offset);
    if (status != FFS_NO_ERRORS) {

      RFSFAT_SEND_TRACE("RFSFAT: setting file pointer failed",
        RV_TRACE_LEVEL_ERROR);
    }
  } else {
    RFSFAT_SEND_TRACE("RFSFAT: file allocation failed",
      RV_TRACE_LEVEL_ERROR);
  }
  /* Write the new offset */
  ret_value = fp_offset;
  return rfsfat_send_response_msg( FSCORE_LSEEK_RSP,
                            pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}


static int convert_FATfstype_string(T_GBI_FS_TYPE fs_type, char *fs_name)
{

  switch(fs_type)
  	{
  	  case GBI_FAT12:
	  	   rfsfat_ucStrcpy((UINT8 *)fs_name,(const UINT8*)RFS_FAT12_FS_NAME);
	  	   break;

	  case GBI_FAT16_A:
	  case GBI_FAT16_B:
	  case GBI_FAT16_LBA:
	   	   rfsfat_ucStrcpy((UINT8 *)fs_name,(const UINT8*)RFS_FAT16_FS_NAME);
	  	break;

	  case GBI_FAT32:
	  case GBI_FAT32_LBA:
	  	   rfsfat_ucStrcpy((UINT8 *)fs_name,(const UINT8*)RFS_FAT32_FS_NAME);
	  	break;

	  default:
	  	   return -1;   /* Unknown file system */

  	}

  return 0;   /* Success */
}



static int convert_mediatype_string(T_GBI_MEDIA_TYPE media_type, char *media_name)
{

  switch(media_type)
  	{
  	  case GBI_MMC:
	  	   rfsfat_ucStrcpy((UINT8 *)media_name,(const UINT8*)"MMC\0");
	  	   break;

     case GBI_SD:
	  	   rfsfat_ucStrcpy((UINT8 *)media_name,(const UINT8*)"SD\0");
	  	   break;

	 case GBI_INTERNAL_NAND:
	  	   rfsfat_ucStrcpy((UINT8 *)media_name,(const UINT8*)"NAND\0");
	  	   break;

	 case GBI_INTERNAL_NOR:
	  	   rfsfat_ucStrcpy((UINT8 *)media_name,(const UINT8*)"NOR\0");
	  	   break;

	  default:
	  	   return -1;   /* Unknown Media */

  	}

  return 0;   /* Success */
}




/**
 *  POSIX entry obtaining statistics on mountpoint, file or filesystem*/
T_FSCORE_RET  rfsfat_ffs_stat(T_RV_HDR *inmsg_p)
{
  BOOL                    found = FALSE;
  UINT8                   mpt_idx = 0;
  UINT8                   mpt_cntr = 0;
  UINT8                   ofctr = 0;
  UINT8                   usedentry=0; //for pclint only
  UINT8                   pth_cntr;
  UINT8                   status = FFS_NO_ERRORS;
  UINT16                   foundcntr = 0;
  UINT8*                  statpath;
  T_RFSFAT_STAT_REQ_MSG   *msg_p;
  T_FSCORE_RET            ret_value = FSCORE_EOK;
  DIR_DIRECTORY_LABEL_STRUCT labloc;
  UINT32                  freesize=0;
  UINT32                  totdatasize=0;

  msg_p = (T_RFSFAT_STAT_REQ_MSG*) inmsg_p;
  statpath = (UINT8*)msg_p->pathname;

  if ( (msg_p == NULL) || (msg_p->pathname == NULL) || (msg_p->stat == NULL)) {
	  status = FFS_RV_ERR_INVALID_PARAMS;
  }
	else {
	  statpath = (UINT8*)msg_p->pathname;

     mpt_idx = RFSFAT_get_mpt_idx(statpath, &foundcntr);

    if(mpt_idx != RFSFAT_INVALID_MPT){

      if( (statpath[foundcntr] == FFS_ASCII_EOL) ||
          (statpath[foundcntr+1] == FFS_ASCII_EOL) ||
          ((statpath[foundcntr+1] == FFS_ASCII_VALUE_FWSLASH) &&
          (statpath[foundcntr+2] == FFS_ASCII_EOL)) ) {

        //partition / mountpoint statistics are requested
        status = E_FFS_ucDiskFree(mpt_idx, &freesize);
        if(status == FFS_NO_ERRORS) {

          RFSFAT_SEND_TRACE("#### diskfree succeeded", RV_TRACE_LEVEL_DEBUG_LOW);

		  E_FFS_ucDiskDataSpace(mpt_idx,&totdatasize);   /* Total data space */
		  
          msg_p->stat->mount_point.blk_size = rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table[mpt_idx].blk_size;
		  msg_p->stat->mount_point.free_space = (freesize / KBYTES);
		  msg_p->stat->mount_point.used_size = ((totdatasize - freesize) / KBYTES);
		  msg_p->stat->mount_point.mpt_size = (totdatasize / KBYTES);
 		  msg_p->stat->mount_point.mpt_id = mpt_idx;
		  msg_p->stat->mount_point.nr_blks = (rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table[mpt_idx].partition_size);
		  msg_p->stat->mount_point.reserved_1 = rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table[mpt_idx].media_type;
		  msg_p->stat->mount_point.reserved_2 = rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table[mpt_idx].filesys_type;
		  convert_FATfstype_string(rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table[mpt_idx].filesys_type,msg_p->stat->mount_point.fs_type);
		  convert_mediatype_string(rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table[mpt_idx].media_type, msg_p->stat->mount_point.media_type);
        } else {
          RFSFAT_SEND_TRACE("#### diskfree failed", RV_TRACE_LEVEL_DEBUG_LOW);
        }
      } else {
        //file or directory statistics are requested

        status = E_FFS_ucDiskStat(statpath, &labloc);
        if (status == FFS_NO_ERRORS) {

          msg_p->stat->file_dir.ino = ((labloc.uiFirstClusterNumberHigh<<16) | (labloc.uiFirstClusterNumber));
          msg_p->stat->file_dir.size = labloc.ulFileSize;

          msg_p->stat->file_dir.mtime = ((labloc.uiLastChangedDate <<16)|(labloc.uiLastChangedTime));
          msg_p->stat->file_dir.ctime = ((labloc.crtDate<<16)|(labloc.crtTime));

          if((labloc.ucAttribute & 1) == 1) {

            //file is read only
            msg_p->stat->file_dir.mode = FSCORE_IRUSR;
          } else {
            //file is read write
            msg_p->stat->file_dir.mode = FSCORE_IWUSR;
          }
           //if dir always set executable flag ON
          if((labloc.ucAttribute & 0x10) == 0x10) {

            //DIR!
            msg_p->stat->file_dir.mode |= FSCORE_IXUSR;
          }
        }
      }
      kill_gbi_readcash();
    } else {
      //wrong pathname
      RFSFAT_SEND_TRACE("#### mptname not found", RV_TRACE_LEVEL_DEBUG_LOW);

      ret_value = FFS_RV_ERR_INVALID_PATH;
    }

}
  return rfsfat_send_response_msg( FSCORE_STAT_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}

/**
 *  Remove the given file
 */
T_FSCORE_RET  rfsfat_dir_file_delete(const char *path_name)
{
  UINT8                     status = FFS_NO_ERRORS;
  UINT8                     status2;
  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  if(path_name == NULL)  return FFS_RV_ERR_INVALID_PATH;

 RFSFAT_SEND_TRACE("rfsfat_dir_file_delete entered, following path", RV_TRACE_LEVEL_DEBUG_LOW);
RFSFAT_SEND_TRACE_DEBUG_LOW((char*)path_name);
 /* Get directory label.                                                     */
  ucResult = ffs_ucGetLabel((RW_PTR_TO_RO(UINT8)) path_name, &stLabLoc,&stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ffs_ucGetLabel failed");
    status=ucResult;
  }
  else
  	{

  /* Check if attribute bit 'directory' nor 'partition' is set.                  */
  if (ATTR_CHECK_SUBDIR((&stMetDat)))
  {
    /* element is not file */
    status = E_FFS_ucDmanRmDir((UINT8*)path_name);
    status2 = flush_write_blk();
    if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS)) {
         status = status2;
        }

  }else {
        status = E_FFS_ucFmanUnlink((UINT8*)path_name);
        status2 = flush_write_blk();
        if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS)) {

          status = status2;
        }
      }

#ifdef ICT_SPEEDUP_RFSFAT2
  status2 = ucFlushFamCash(stLabLoc.mpt_idx);
  if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS))
  {
    status = status2;
  }
#endif

  /* Flush the gbi readcash */
  kill_gbi_readcash();

  	}   /* End of Else.....of if(ucResult)*/

  return status;
}
/**
 *  POSIX entry for renaming a file or directory */
T_FSCORE_RET  rfsfat_ffs_fman_rename(T_RV_HDR *inmsg_p)
{
  char                    pathonly[255];
  char                    *pathonly_p;
  const char              *nameAndExt_p;
  const char              *fullpath_p;
  UINT8                   status = FFS_NO_ERRORS;
  UINT8                   status2;
  T_RFSFAT_RENAME_REQ_MSG *msg_p;
  T_FSCORE_RET            ret_value = FSCORE_EOK;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  msg_p = (T_RFSFAT_RENAME_REQ_MSG*) inmsg_p;
  if (msg_p->oldname == NULL || msg_p->newname == NULL) {
      status = FFS_RV_ERR_INVALID_PATH;
    }
  else
    {
      /* Check if the old name exists or not */
	  status=ffs_ucGetLabel((RW_PTR_TO_RO(UINT8)) msg_p->oldname,&stLabLoc,&stMetDat);

     if(status == FFS_NO_ERRORS )
     	{
      fullpath_p = msg_p->newname;
      pathonly_p = &pathonly[0];
#if 0
      		/* Delete the file or directory of newname, if it is existing */
      		rfsfat_dir_file_delete(msg_p->newname);
#endif

      nameAndExt_p = separateFileAndPath(pathonly_p, (const char*)fullpath_p);

      status = E_FFS_ucFmanMv((UINT8*)msg_p->oldname,
                              (UINT8*)pathonly_p, (UINT8*)nameAndExt_p);

      }
      kill_gbi_readcash();
    }
  return rfsfat_send_response_msg( FSCORE_RENAME_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}

/**
 *  POSIX entry for making a directory */
T_FSCORE_RET  rfsfat_ffs_dman_mkdir(T_RV_HDR *inmsg_p)
{
  char                    pathonly[255];
  char                    *pathonly_p;
  const char              *nameAndExt_p;
  UINT8                   status;
  UINT8                   status2;
  T_RFSFAT_MKDIR_REQ_MSG  *msg_p;
  T_FSCORE_RET            ret_value = FSCORE_EOK;

  msg_p = (T_RFSFAT_MKDIR_REQ_MSG*) inmsg_p;
  if (msg_p->pathname == NULL) {
    status = FFS_RV_ERR_INVALID_PATH;
  } else {
    pathonly_p = &pathonly[0];

    nameAndExt_p = separateFileAndPath(pathonly_p, msg_p->pathname);
    status = E_FFS_ucDmanMkDir((UINT8*)pathonly_p, (UINT8*)nameAndExt_p);
    status2 = flush_write_blk();
    if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS)){

      status = status2;
    }

if(status==FFS_NO_ERRORS)
    {
  		/* Flush the gbi readcash */
  		kill_gbi_readcash();
  	}
  }
  return rfsfat_send_response_msg( FSCORE_MKDIR_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}

/**
 *  POSIX entry for removing a directory */
T_FSCORE_RET  rfsfat_ffs_dman_rmdir(T_RV_HDR *inmsg_p)
{
  UINT8                   status = FFS_NO_ERRORS;
  UINT8                   status2;
  T_RFSFAT_RMDIR_REQ_MSG  *msg_p;
  T_FSCORE_RET            ret_value = FSCORE_EOK;

  msg_p = (T_RFSFAT_RMDIR_REQ_MSG*) inmsg_p;
  if (msg_p->pathname == NULL) {
      status = FFS_RV_ERR_INVALID_PATH;
    }
  else {
    status = E_FFS_ucDmanRmDir((UINT8*)msg_p->pathname);
    status2 = flush_write_blk();
    if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS)){

      status = status2;
    }
#ifdef ICT_SPEEDUP_RFSFAT2
    status2 = flush_write_blk();
    if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS))
    {
      status = status2;
    }
#endif //ICT_SPEEDUP_RFSFAT2

if(status == FFS_NO_ERRORS)
  	{
  		/* Flush the gbi readcash */
  		kill_gbi_readcash();
  	}
  }

  return rfsfat_send_response_msg( FSCORE_RMDIR_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}

/**
 *  POSIX entry for opening a directory */
T_FSCORE_RET  rfsfat_ffs_dman_opendir(T_RV_HDR *inmsg_p)
{
  UINT8                     status = FFS_NO_ERRORS;
  T_RFSFAT_OPENDIR_REQ_MSG  *msg_p;
  T_FSCORE_RET              ret_value = FSCORE_EOK;

  msg_p = (T_RFSFAT_OPENDIR_REQ_MSG*) inmsg_p;

  if (msg_p->pathname == NULL) {
      status = FFS_RV_ERR_INVALID_PATH;
    }
  else if (msg_p->dirp == NULL) {
      status = FFS_RV_ERR_INVALID_PARAMS;
    }
#if 0
  else if(msg_p->pathname[0] == '/' && msg_p->pathname[1] == '\0') {
  	/* opening the root dir '/' */
    msg_p->dirp->opendir_ino = ROOT_DIR_INODE_NUMBER;
    msg_p->dirp->lastread_ino = 0;
	msg_p->dirp->mpt_idx = 0;  /*  No mount point index */

  	}
 #endif
  else {
      //continue with opening new dir stream
      status = E_FFS_open_dir((UINT8*)msg_p->pathname);
      if(status == FFS_NO_ERRORS) {

        //save first entry found for first read action
        msg_p->dirp->opendir_ino = RFSFAT_QUERY.stLabLoc.uiFirstCl;
        msg_p->dirp->lastread_ino = RFSFAT_QUERY.stLabLoc.tLblPos;
		msg_p->dirp->mpt_idx = RFSFAT_QUERY.stLabLoc.mpt_idx;
		msg_p->dirp->resrv_1=  rfsfat_env_ctrl_blk_p->unmount_count; /* store the unmount conter value */

        //the readdir function returns 0 if the last entry is read, so the
        //return value of open dir is of no real importance
        //Because finding out the real nr of obj in this dir requires a big effort
        //the default return value is 0XFFFFFFFF, nr of objects
        ret_value = RFSFAT_MAX_RET_ENTRIES;
      }
		/* Flush the gbi readcash */
    kill_gbi_readcash();

  }
  return rfsfat_send_response_msg( FSCORE_OPENDIR_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}

/**
 *  POSIX entry for removing a directory */
T_FSCORE_RET  rfsfat_ffs_dman_readdir(T_RV_HDR *inmsg_p)
{
  UINT8                     status = FFS_NO_ERRORS;
  T_RFSFAT_READDIR_REQ_MSG  *msg_p;
  T_FSCORE_RET              ret_value = FSCORE_EOK;
  FFS_SMALL_DIRECTORY_LABEL_STRUCT  dirlabel;
  UINT8						mount_index;

  msg_p = (T_RFSFAT_READDIR_REQ_MSG*) inmsg_p;
  //parameter check
  if( (msg_p->dirp == NULL) ||
      (msg_p->buf == NULL) ||
      ( (msg_p->pair_value.fd < 0) &&
        (msg_p->pair_value.pair_id < 0)) ||
      (msg_p->size == 0) || (msg_p->dirp->resrv_1 != rfsfat_env_ctrl_blk_p->unmount_count))
  {
    status = FFS_RV_ERR_INVALID_PARAMS;
  }
#if 0
  else
  	{

  	/* Parameters are correct */
  if (msg_p->dirp->opendir_ino == ROOT_DIR_INODE_NUMBER) {
     /* reading root directory '/'*/
	 /* read the patition names as directories */
	 mount_index =msg_p->dirp->lastread_ino;

	/* search for mounted partition */
	while((mount_index < NR_OF_MOUNTPOINTS) &&( MPT_TABLE[mount_index].is_mounted == FALSE))
			mount_index++;
	msg_p->dirp->lastread_ino = mount_index;
	if(mount_index >= NR_OF_MOUNTPOINTS)
		{
		    /* No mount points */
			ret_value = 0;
			status = FFS_NO_ERRORS;
		}
	else
		{
		   /* copy the partition name to buffer */
		   short unsigned int str_index=0, part_len=0;

		   /* verify msg_p->size */
		   for(part_len=0;(part_len<GBI_MAX_PARTITION_NAME) &&((MPT_TABLE[mount_index].partition_name[part_len])!= '\0');part_len++);

           if(msg_p->size < part_len)
           	{
           	  ret_value = -1;  /* return value should be an error */
			  status = FFS_RV_ERR_INVALID_PARAMS;
           	}
		   else
		   	{

		   	   for(str_index=0;str_index<part_len;str_index++)
		   		{
		          /* copy the character */
				  msg_p->buf[str_index]=MPT_TABLE[mount_index].partition_name[str_index];
				  str_index++;
		   		}

		   		/* Make the last character as '\0' */
		   		msg_p->buf[str_index]='\0';
		   		ret_value = str_index;
				status = FFS_NO_ERRORS;
		   	}
		}

  	}  /* IF ROOT_DIR_INODE_NUMBER */
 #endif
  else {

        /* Fill the RFSFAT_QUERY structure */
		 RFSFAT_QUERY.stLabLoc.uiFirstCl = msg_p->dirp->opendir_ino;
         RFSFAT_QUERY.stLabLoc.tLblPos   = msg_p->dirp->lastread_ino;
         RFSFAT_QUERY.stLabLoc.mpt_idx   = msg_p->dirp->mpt_idx;


        //continue with opening new dir stream
        status = E_FFS_ucDmanGetNext(&dirlabel);
        if(status == FFS_NO_ERRORS) {

          if(LFN_QUERY_FLAG)
          {
            //use LFN_QUERY_NAME instead of dirlabel to copy to client
           /* strncpy(msg_p->buf, (const char*)&LFN_QUERY_NAME, msg_p->size);  */
			/* FIX: Dont put the & character for strncpy */
			strncpy(msg_p->buf, (const char*)LFN_QUERY_NAME, msg_p->size);

            //return the length
            ret_value = (T_FSCORE_RET)strlen((const char*)LFN_QUERY_NAME);
            //free allocated buffer
            RFSFAT_FREE_BUF(LFN_QUERY_NAME);
            //reset lfn query flag
            LFN_QUERY_FLAG = FALSE;
          } else {
            strncpy(msg_p->buf, (const char*)dirlabel.aucNameExt, msg_p->size);
            //return the length
            ret_value = (T_FSCORE_RET)strlen((const char*)dirlabel.aucNameExt);
          }
        } else if (status == FFS_RV_ERR_LAST_ENTRY) {

          //last entry was found return nul length
          ret_value = (T_FSCORE_RET)RFSFAT_UNTOUCHED;
          //status signals an error but this is not an error
          status = FFS_NO_ERRORS;
        } else {
          //an error occurred return nagative value
          ret_value = (T_FSCORE_RET)FSCORE_EINVALID;
        }
	   //when the programmer didnt free the RFSFAT_LFN_FROM_LABEL buffer
      //it is done here to prevent leaking
      RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
      RFSFAT_LFN_FROM_LABEL = NULL;
        msg_p->dirp->opendir_ino = RFSFAT_QUERY.stLabLoc.uiFirstCl;
        msg_p->dirp->lastread_ino = RFSFAT_QUERY.stLabLoc.tLblPos;

		/* Flush the gbi readcash */
      kill_gbi_readcash();

	   }  /* elese part of  IF ROOT_DIR_INODE_NUMBER */

  return rfsfat_send_response_msg( FSCORE_READDIR_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}


/**
 *  POSIX entry for preformat*/
T_FSCORE_RET  rfsfat_ffs_preformat(T_RV_HDR *inmsg_p)
{
  T_RFSFAT_PREFORMAT_REQ_MSG  *msg_p;
  T_FSCORE_RET                ret_value = FSCORE_EOK;
  UINT8                       status = FFS_NO_ERRORS;

  msg_p = (T_RFSFAT_PREFORMAT_REQ_MSG*) inmsg_p;
  if (msg_p->pathname == NULL) {
      status = FFS_RV_ERR_INVALID_PATH;
    }
  if (msg_p->magic != 0xDEAD) {
      status = FFS_RV_ERR_INVALID_PARAMS;
    }

  return rfsfat_send_response_msg( FSCORE_PREFORMAT_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}

/**
 *  POSIX entry for format*/
T_FSCORE_RET  rfsfat_ffs_format(T_RV_HDR *inmsg_p)
{
  UINT8                   mpt_cntr = 0;
  UINT8*                  new_partname;
  UINT8                   status = FFS_NO_ERRORS;
#ifdef ICT_SPEEDUP_RFSFAT2
  UINT8                   status2;
#endif //ICT_SPEEDUP_RFSFAT2
  UINT8                   mpt_idx = 0;
  T_RFSFAT_FORMAT_REQ_MSG *msg_p;
  T_FSCORE_RET            ret_value = FSCORE_EOK;

  msg_p = (T_RFSFAT_FORMAT_REQ_MSG*) inmsg_p;

  if (msg_p->pathname == NULL || msg_p->name == NULL) {
      status = FFS_RV_ERR_INVALID_PATH;
    }
  else if (msg_p->magic != RFSFAT_FORMAT_MAGIC) {
      status = FFS_RV_ERR_INVALID_PARAMS;
    }
  else
	{

     mpt_idx= RFSFAT_get_mpt_idx((const UINT8*)msg_p->pathname, NULL);

    if(mpt_idx != RFSFAT_INVALID_MPT){

      if(msg_p->name != NULL) {

        new_partname = (UINT8*)msg_p->name;
      } else {
        //WHEN NO NAME IS SPECIFIED USE MOUNTPOINT NAME
        new_partname = MPT_TABLE[mpt_idx].partition_name;
      }
      //format the indicated mountpoint
      status = E_FFS_ucDiskFormat(mpt_idx, new_partname,GBI_UNKNOWN);
#ifdef ICT_SPEEDUP_RFSFAT2
      status2 = ucFlushFamCash(mpt_idx);
      if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS))
      {
        status = status2;
      }
#endif //ICT_SPEEDUP_RFSFAT2
      kill_gbi_readcash();
    }
   else
   	{
   	       status = FFS_RV_ERR_INVALID_PATH;
   	}

  }
  return rfsfat_send_response_msg( FSCORE_FORMAT_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}


/**
 *  POSIX entry for file truncation
 */
T_FSCORE_RET  rfsfat_ffs_file_trunc(T_RV_HDR *inmsg_p)
{
  UINT8                     status = FFS_NO_ERRORS;
  T_RFSFAT_TRUNC_REQ_MSG   *msg_p;
  T_FSCORE_PAIR_VALUE     pair_value;

  msg_p = (T_RFSFAT_TRUNC_REQ_MSG*) inmsg_p;

  pair_value.fd = msg_p->fd;
  pair_value.pair_id = msg_p->pair_id;

  if (msg_p->size < 0)
	return rfsfat_send_response_msg( FSCORE_TRUNC_RSP,pair_value,&(msg_p->return_path),
						                            FFS_RV_ERR_INVALID_PARAMS, FSCORE_EOK);

  if (msg_p->size == 0)
	return rfsfat_send_response_msg( FSCORE_TRUNC_RSP, pair_value, &(msg_p->return_path),
						                            FFS_NO_ERRORS, FSCORE_EOK);


  status = E_FFS_ucFileTrunc((UINT8) msg_p->fd,  msg_p->size);

  return rfsfat_send_response_msg( FSCORE_TRUNC_RSP,
					                            pair_value,
					                            &(msg_p->return_path),
					                            status,
					                            FSCORE_EOK);
}
T_FSCORE_RET  rfsfat_ffs_getattribute(void *inmsg_p)
{
 T_RFSFAT_GETATTRIB_REQ_MSG *msg_p;
 UINT8						attrib;
 UINT8                     status = FFS_NO_ERRORS;
 T_FSCORE_PAIR_VALUE     pair_value;



 msg_p = (T_RFSFAT_GETATTRIB_REQ_MSG *) inmsg_p;

 pair_value.fd =0;
 pair_value.pair_id = msg_p->pair_id;


 status = E_FFS_ucFmanGetAttributes((RW_PTR_TO_RO(UINT8)) msg_p->path_name,&attrib);


 return rfsfat_send_response_msg( FSCORE_GETATTRIB_RSP,
					                           pair_value,
					                            &(msg_p->return_path),
					                            status,
					                            attrib);
}
T_FSCORE_RET  rfsfat_ffs_setattribute(void *inmsg_p)
{

 T_RFSFAT_SETATTRIB_REQ_MSG *msg_p;
 UINT8                     status = FFS_NO_ERRORS;
 T_FSCORE_PAIR_VALUE     pair_value;



 msg_p = (T_RFSFAT_SETATTRIB_REQ_MSG *) inmsg_p;

 pair_value.fd =0;
 pair_value.pair_id = msg_p->pair_id;


 status = E_FFS_ucFmanSetAttributes((RW_PTR_TO_RO(UINT8)) msg_p->path_name,msg_p->attrib);

 if (status == FFS_NO_ERRORS) {
   status = flush_write_blk();
 }


 return rfsfat_send_response_msg( FSCORE_SETATTRIB_RSP,
												pair_value,
					                            &(msg_p->return_path),
					                            status,
					                            FSCORE_EOK);

}


T_FSCORE_RET  rfsfat_ffs_getdatetime(void *inmsg_p)
{
 T_RFSFAT_GETDATETIME_REQ_MSG *msg_p;
 UINT8                     status = FFS_NO_ERRORS;
 T_FSCORE_PAIR_VALUE     pair_value;
 UINT16					var_date,var_time, var_cdate,var_ctime;



 msg_p = (T_RFSFAT_GETDATETIME_REQ_MSG *) inmsg_p;

 pair_value.fd =0;
 pair_value.pair_id = msg_p->pair_id;

 if(msg_p->path_name == NULL || ((msg_p->date_time_p == NULL) && (msg_p->crdate_time_p == NULL)) )
 	{
 	      status = FFS_RV_ERR_INVALID_PARAMS;

		return rfsfat_send_response_msg( FSCORE_GETDATETIME_RSP,
					                           pair_value,
					                            &(msg_p->return_path),
					                            status,
				                            FSCORE_EOK);

 	}


 status = E_FFS_ucFmanGetDateTime((RW_PTR_TO_RO(UINT8))msg_p->path_name,&var_date,&var_time, &var_cdate, &var_ctime);

 if(status == FFS_NO_ERRORS)
 	{

	  if(msg_p->date_time_p != NULL)
 	{
       /* convert the date and time */
	   if(convertfat_datetime_to_local(var_date,var_time,msg_p->date_time_p) != 0)
	   	          status = FFS_RV_ERR_INVALID_PARAMS;
	  	}

      if(msg_p->crdate_time_p != NULL)
	  	{
   		  RFSFAT_SEND_TRACE("rfsfat_ffs_getdatetime, retriving created date & time", RV_TRACE_LEVEL_DEBUG_LOW);
       		/* convert the date and time */
	   		if(convertfat_datetime_to_local(var_cdate,var_ctime,msg_p->crdate_time_p) != 0)
	   	          status = FFS_RV_ERR_INVALID_PARAMS;
	  	}

 	}

 return rfsfat_send_response_msg( FSCORE_GETDATETIME_RSP,
					                           pair_value,
					                            &(msg_p->return_path),
					                            status,
					                            FSCORE_EOK);
}


#if (FFS_UNICODE ==1)

/**************************************************/
// Unicode FFS
/**************************************************/

#if 0

/* Determine the partion number from media type */
static UINT8 determine_partition_uc(FS_MEDIA_PARTITION part_num, UINT8 *mp_idx_p)
{

 UINT8   nr_mptcntr = 0;

 for(nr_mptcntr=0;nr_mptcntr < NR_OF_MOUNTPOINTS;nr_mptcntr++)
 	{
          if(part_num == FS_MMC)
          	{
          	  if((MPT_TABLE[nr_mptcntr].media_type == GBI_SD) ||(MPT_TABLE[nr_mptcntr].media_type == GBI_MMC))
          	  	{
          	  	            *mp_idx_p=nr_mptcntr;
							return FFS_NO_ERRORS;

          	  	}
          	}
		  else
		  	if((part_num == FS_NAND) &&(MPT_TABLE[nr_mptcntr].media_type == GBI_INTERNAL_NAND))
		  		{
				  		 *mp_idx_p=nr_mptcntr;
							return FFS_NO_ERRORS;

		  		}

 	}



 return FFS_RV_IS_NOT_A_PART;
}

#endif

/**
 *  This function separates the path from the filename.
 *  pathN_p will have buffer, pathAndFile_p also.
 *  return will be pointer to file name
 */
static T_WCHAR* separateFileFromPath_uc(const  T_WCHAR *pathAndFile_p,
                                           T_WCHAR *pathN_p)

{
  INT16  i;
  UINT16  strLenght;

  strLenght = wstrlen(pathAndFile_p);

  for( i = (INT16)strLenght;i >= 0; i--)
  {
    if (pathAndFile_p[i] == RFSFAT_SEPARATOR)
    { // '/' found
      if(i > 0)
      	{
      		wstrncpy(pathN_p,pathAndFile_p,(UINT16)i);
      	}
      pathN_p[i]= EOSTR;
      if (i == strLenght)
      {
        return NULL;  /* No file name */
      }

      return (((T_WCHAR*)(&pathAndFile_p[i+1])));
    } /* end of if */
  } /* end of for */

  //No path before file name,
  pathN_p[0]= EOSTR;  //empty
  return ((T_WCHAR*)pathAndFile_p); //complete input path is file name

}





#if 0
/****************************************************************************/
/* Determine partition.                                                        */
/****************************************************************************/
static UINT8 determinePartitionIndex_uc(UINT8 *pucPath, UINT8 *mp_idx_p)
{
  UINT16  pathcntr = 0;
  BOOL    found= FALSE;
  UINT8   nr_mptcntr = 0,partname_idx=0;

  if((pucPath==NULL) || (*pucPath==EOSTR))
  	{
  	  found=FALSE;
  	}
  else
 	{
  	 RFSFAT_SEND_TRACE_WARNING ("RFSFAT: determinePartitionIndex_uc");

   RFSFAT_SEND_TRACE_WARNING(pucPath);

  while ((!found) && (nr_mptcntr < NR_OF_MOUNTPOINTS))
	{

	 //find out if mountpoint names match
	  pathcntr = 0;
	  if((pucPath[pathcntr] == '/') && (MPT_TABLE[nr_mptcntr].partition_name[0] != '/'))
	  	{
	  	   pathcntr++;     /* mount point name is starting with '/' */
	  	}
	  for (partname_idx=0; partname_idx < GBI_MAX_PARTITION_NAME; pathcntr++,partname_idx++)
		{

		  if (toupper(pucPath[pathcntr]) != toupper( MPT_TABLE[nr_mptcntr].partition_name[partname_idx]))
			{

			  if (pucPath[pathcntr ] == FFS_ASCII_VALUE_FWSLASH)
				{

				  found = TRUE;
				  //store index in mountpoint table
				  *mp_idx_p = nr_mptcntr;
				  break;
				}
			  else
				{
				  //they do not match
				  nr_mptcntr++;
				  break;
				}
			}
		  else if (pucPath[pathcntr ] == FFS_ASCII_EOL)
			{

			  //this was the last character of the string untill now all matched
			  //so we have a MATCH
			  found = TRUE;
			  //store index in mountpoint table
			  *mp_idx_p = nr_mptcntr;
			  break;
			}
		}  /* End of for statement */
	  if (partname_idx == GBI_MAX_PARTITION_NAME)
		{

		  //until here it did match so we found the mountpointname
		  found = TRUE;
		  //store index in mountpoint table
		  *mp_idx_p = nr_mptcntr;
		}

    	}   /* End of while */
 	} /* End of else */
  if (!(found))
	{
	  //no matching mountpointnames were found
	  RFSFAT_SEND_TRACE ("RFSFAT: no matching mountpoint names were found",
						 RV_TRACE_LEVEL_ERROR);

	  return FFS_RV_ERR_INVALID_PATH;
	}


  return FFS_NO_ERRORS;
}

#endif



//max nr bytes of mountpoint name in unicode (incl NULL)
#define MP_SIZE_UC  (GBI_MAX_PARTITION_NAME)


/**
 *  This function separates the mountpoint name from the path.
 */
static UINT8 separateMountpointFromPath_uc(
                          const T_WCHAR *mpAndpath,
                                T_WCHAR **path,
							    UINT8          *mp_idx_p)
{
 UINT8 	 temp_path_ascii[MP_SIZE_UC+1];
 UINT16	 i;
 UINT8           status=FFS_NO_ERRORS;


 for(i=0;(mpAndpath[0]!=EOSTR)&&(i<MP_SIZE_UC);i++)
 	{
 	  if((i!=0) && ((mpAndpath[i] == RFSFAT_SEPARATOR) || ((mpAndpath[i] == EOSTR))))
 	  	{

		  if((i-2) > MP_SIZE_UC) /* leave the 1st / character and current / character */
		  	{
		  	      /* too long partition name, error */
				  return FFS_RV_IS_NOT_A_PART;
		  	}

 	  	  /* Copy the mount point name */
		  convert_unicode_to_u8_length((const T_WCHAR*)mpAndpath, (char*)temp_path_ascii,i);

          *mp_idx_p= RFSFAT_get_mpt_idx(temp_path_ascii,NULL);

		  if(path!=NULL)
		  	{
		  	     /* path without mount point */
				 *path=((T_WCHAR *) (mpAndpath+i));   /* This includes the '/' char  */
		  	}

		  if( *mp_idx_p  == RFSFAT_INVALID_MPT)
		  	{
				  return FFS_RV_ERR_INVALID_PATH;
		  	}


		  return FFS_NO_ERRORS;

 	  	} /* End of if */
 	}/* End of for */
 return FFS_RV_IS_NOT_A_PART;

}



#if 0


/**
 *  POSIX entry for the fstat function (status info on open file) */
T_FSCORE_RET  rfsfat_ffs_fman_chmod_uc(void *inmsg_p)
{
  T_RFSFAT_CHMOD_UC_REQ_MSG   *msg_p;
  T_FSCORE_RET                ret_value = FSCORE_EOK;
  UINT8                       attr      = FFS_ATTRIB_READWRITE_MASK;

  msg_p = (T_RFSFAT_CHMOD_UC_REQ_MSG*) inmsg_p;

/* @TODO update code for UC */
#if 0
  /* both read and write selected */
  if ((msg_p->mode & (FSCORE_IRUSR|FSCORE_IWUSR)) == (FSCORE_IRUSR|FSCORE_IWUSR)) {

    //make file read write
    attr = FFS_ATTRIB_READWRITE_MASK;
  } else if ((msg_p->mode & FSCORE_IRUSR) == FSCORE_IRUSR) {

    //make file read only
    attr = FFS_ATTRIB_READONLY_MASK;
  }

  /* other (invalid) bits? */
  if (msg_p->mode & ~RFSFAT_VALID_PERMS) {
    status = FFS_RV_ERR_INVALID_PARAMS; //FFS_RV_ERR_INVALID_ACCESS_MODE;
  }

  if (msg_p->pathname == NULL) {
    status = FFS_RV_ERR_INVALID_PATH;
  }

  if (status == FFS_NO_ERRORS) {

    if(E_FFS_ucFmanSetAttributes((UINT8*)msg_p->pathname, attr)) {

      ret_value = msg_p->mode;
    }
    kill_gbi_readcash();
  }
  //if status is NOT ok then the value of ret_value is not relevant
  //anymore. Only when status is OK the glue layer may return an error
  //using ret_value
  return rfsfat_send_response_msg( FSCORE_CHMOD_RSP,
                                    msg_p->pair_value,
                                    &(msg_p->return_path),
                                    status,
                                    ret_value);
#endif
  return FSCORE_EOK;
}

#endif

/**
 *  POSIX entry for format*/
T_FSCORE_RET  rfsfat_ffs_format_uc(void *inmsg_p)
{

  UINT8                       mpName_u8_p[MAX_PART_NAME+1];
  T_WCHAR					  temp_buf[MAX_PART_NAME+1];
  UINT8*                      new_partname;
  UINT8                       status = FFS_NO_ERRORS;
#ifdef ICT_SPEEDUP_RFSFAT2
  UINT8                       status2;
#endif //ICT_SPEEDUP_RFSFAT2
  UINT8                       mpt_idx = 0;
  T_RFSFAT_FORMAT_UC_REQ_MSG  *msg_p;
  T_FSCORE_RET                ret_value = FSCORE_EOK;
  T_WCHAR					  *path_no_mp;



  msg_p    = (T_RFSFAT_FORMAT_UC_REQ_MSG*) inmsg_p;


 if (msg_p->magic != RFSFAT_FORMAT_MAGIC)
       {
         status = FFS_RV_ERR_INVALID_PARAMS;
       }
       else
       {
         status = separateMountpointFromPath_uc(msg_p->pathname,&path_no_mp, &mpt_idx);

		  if(status==FFS_NO_ERRORS)
		  	{
			 if(path_no_mp[0]!=EOSTR)
			 	{
			 	   if(path_no_mp[0]!=RFSFAT_SEPARATOR)
				   	    status = FFS_RV_ERR_INVALID_PATH;
				   else
				   	if(path_no_mp[1]!=EOSTR)
						status = FFS_RV_ERR_INVALID_PATH;
			 	}
		  	}

          if (status != FFS_NO_ERRORS)
          {
              return rfsfat_send_response_msg( FSCORE_FORMAT_RSP,
                                      msg_p->pair_value,
                                      &(msg_p->return_path),
                                      status,
                                      (T_FSCORE_RET)FSCORE_EOK); //dummy
          }

          /* copy the partition name to temp buffer */
		    wstrncpy(temp_buf,msg_p->name,MAX_PART_NAME);
		    temp_buf[MAX_PART_NAME] = EOSTR;


          if ((temp_buf != NULL) && (temp_buf[0] != EOSTR))
          {
            convertUcToU8(temp_buf, (UINT8*)mpName_u8_p);
            mpName_u8_p[MAX_PART_NAME] = EOSTR; //make shure its not too long
            new_partname = mpName_u8_p;
          }
          else
          {
            //WHEN NO NAME IS SPECIFIED USE MOUNTPOINT NAME
            new_partname = MPT_TABLE[mpt_idx].partition_name;
          }

          //format the indicated mountpoint
          status = E_FFS_ucDiskFormat(mpt_idx, new_partname,GBI_UNKNOWN);

          #ifdef ICT_SPEEDUP_RFSFAT2
          status2 = ucFlushFamCash(mpt_idx);
          if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS))
          {
           	status = status2;
          }
		  #endif //ICT_SPEEDUP_RFSFAT2

          status2 = flush_write_blk();
          if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS))
          {
           	status = status2;
          }
          kill_gbi_readcash();

       }

  return rfsfat_send_response_msg( FSCORE_FORMAT_RSP,
                                   msg_p->pair_value,
                                   &(msg_p->return_path),
                                   status,
                                   ret_value);
}

/**
 *  POSIX entry for making a directory */
T_FSCORE_RET  rfsfat_ffs_dman_mkdir_uc(void *inmsg_p)
{
  T_WCHAR          *pathName_p,*path_no_mp;
  T_WCHAR          *dirName_p;
  UINT16                  NmbBytesFullPath;
  UINT8                     status=FFS_NO_ERRORS;
  UINT8                     status2=FFS_NO_ERRORS;
  UINT8                     mp_idx;
  T_RFSFAT_MKDIR_UC_REQ_MSG *msg_p;
  T_FSCORE_RET              ret_value = FSCORE_EOK;

  msg_p = (T_RFSFAT_MKDIR_UC_REQ_MSG*) inmsg_p;

  if (msg_p->mode & ~RFSFAT_VALID_PERMS) {
    status = FFS_RV_ERR_INVALID_PARAMS; //FFS_RV_ERR_INVALID_ACCESS_MODE;
  } else if (msg_p->pathname == NULL) {
    status = FFS_RV_ERR_INVALID_PATH;
  } else {


  status = separateMountpointFromPath_uc(msg_p->pathname, &path_no_mp,&mp_idx);

  if (status != FFS_NO_ERRORS)
  {

    return rfsfat_send_response_msg( FSCORE_MKDIR_RSP,
                              msg_p->pair_value,
                              &(msg_p->return_path),
                              status,
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  }

  NmbBytesFullPath = (wstrlen(path_no_mp)+1) * sizeof(T_WCHAR);
  RFSFAT_GETBUF_NO_RETURN(NmbBytesFullPath, &pathName_p);

  if(pathName_p == NULL)
  	{

  	 /* send the Memory failure message to client  */
	 return rfsfat_send_response_msg( FSCORE_MKDIR_RSP,
                              msg_p->pair_value,
                              &(msg_p->return_path),
                              RFSFAT_MEMORY_ERR,  /* Memory error */
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  	}




  dirName_p=separateFileFromPath_uc(path_no_mp, pathName_p);


 if(dirName_p== NULL)
  	{
  	  /* File name not found */
  	 /* free the earlier memory allocation */
	 RFSFAT_FREE_BUF(pathName_p);

	 /* send the Memory failure message to client  */
	 return rfsfat_send_response_msg( FSCORE_MKDIR_RSP,
                              msg_p->pair_value,
                              &(msg_p->return_path),
                              FFS_RV_ERR_INVALID_PATH,  /* Invalid path */
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  	}



  status = E_FFS_ucDmanMkDir_uc(mp_idx, pathName_p, dirName_p);


#ifdef ICT_SPEEDUP_RFSFAT2
    status2 = ucFlushFamCash(mp_idx);
    if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS)){

      status = status2;
    }
#endif //ICT_SPEEDUP_RFSFAT2
    status2 = flush_write_blk();
    if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS)){

      status = status2;
    }
  kill_gbi_readcash();
  RFSFAT_FREE_BUF(pathName_p);

  }


  return rfsfat_send_response_msg( FSCORE_MKDIR_RSP,
                                   msg_p->pair_value,
                                   &(msg_p->return_path),
                                   status,
                                   ret_value);
}





/**
 *  POSIX entry for file open
 *  The mode msg parameter is ignored because FAT does not support this
 */
T_FSCORE_RET  rfsfat_ffs_file_open_uc(void *inmsg_p)
{
  T_WCHAR          *pathName_p=NULL,*path_no_mp;
  T_WCHAR          *fileName_p=NULL;
  UINT8                   status = FFS_NO_ERRORS;
  UINT8                   ucAccessMode;
  UINT8                   handle;
  UINT8                   i;
  UINT8                   mp_idx;
  T_FSCORE_FD             fd = 0;
  T_RFSFAT_OPEN_UC_REQ_MSG   *msg_p;
  BOOL                    flag_create = FALSE;
  const T_FSCORE_FLAGS    valid_flags = FSCORE_O_RDONLY | FSCORE_O_WRONLY | FSCORE_O_RDWR | FSCORE_O_CREATE | FSCORE_O_APPEND | FSCORE_O_TRUNC | FSCORE_O_EXCL;


  msg_p = (T_RFSFAT_OPEN_UC_REQ_MSG*) inmsg_p;

  /* parameter check */
  if (msg_p->pathname == NULL) {
	status = FFS_RV_ERR_INVALID_PATH;
  }

  /* check for other non existing flags */
  if ((status==FFS_NO_ERRORS) && (msg_p->flags & (~valid_flags)))
  	{
  	  status = FFS_RV_ERR_INVALID_PARAMS;
	 }

if(status == FFS_NO_ERRORS)
{

  if(((msg_p->flags&FSCORE_O_WRONLY) == FSCORE_O_WRONLY)||((msg_p->flags&FSCORE_O_RDWR) == FSCORE_O_RDWR))
  	{
  	      status = FFS_NO_ERRORS;
   	}
  else
  if(msg_p->flags == FSCORE_O_RDONLY)
  	{
  	    status = FFS_NO_ERRORS;
  	}
  else
  	   status = FFS_RV_ERR_INVALID_PARAMS;
}

  if (status != FFS_NO_ERRORS) {
    return rfsfat_send_response_msg( FSCORE_OPEN_RSP,
                              msg_p->pair_value,
                              &(msg_p->return_path),
                              status,
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  }

  //file does not exist, may we create it?
  if ((msg_p->flags & FSCORE_O_CREATE) == FSCORE_O_CREATE)
  {
    flag_create = TRUE;
  }

  /* get the mount point index from part_num */
  /* The path of the file does not contains the partition name */
  status = separateMountpointFromPath_uc(msg_p->pathname,&path_no_mp, &mp_idx);

  if (status != FFS_NO_ERRORS)
  {

    return rfsfat_send_response_msg( FSCORE_OPEN_RSP,
                              msg_p->pair_value,
                              &(msg_p->return_path),
                              status,
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  }


  /* Allocate the memory */
  RFSFAT_GETBUF_NO_RETURN(((wstrlen(path_no_mp)* sizeof(T_WCHAR))+2), &pathName_p);

  if(pathName_p == NULL)
  	{
  	 /* send the Memory failure message to client  */
	 return rfsfat_send_response_msg( FSCORE_OPEN_RSP,
                              msg_p->pair_value,
                              &(msg_p->return_path),
                              RFSFAT_MEMORY_ERR,  /* Memory error */
                              (T_FSCORE_RET)FSCORE_EOK); //dummy

  	}


  fileName_p=separateFileFromPath_uc(path_no_mp, pathName_p);


  if(fileName_p== NULL)
  	{
  	 /* free the earlier memory allocation */
	 RFSFAT_FREE_BUF(pathName_p);

	 /* send the Memory failure message to client  */
	 return rfsfat_send_response_msg( FSCORE_OPEN_RSP,
                              msg_p->pair_value,
                              &(msg_p->return_path),
                              FFS_RV_ERR_INVALID_PATH,  /* Memory error */
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  	}



  status = E_FFS_ucFileCreate_uc(mp_idx, (const T_WCHAR*)pathName_p,
                                      (const T_WCHAR*)fileName_p, flag_create);


  /* Deallocate the memory */
  RFSFAT_FREE_BUF(pathName_p);


  if ((status != FFS_NO_ERRORS) && (status != FFS_RV_ERR_FILE_ALREADY_EXISTS))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: E_FFS_ucFileCreate_uc failed, Uc");
    return rfsfat_send_response_msg( FSCORE_OPEN_RSP,
                              msg_p->pair_value,
                              &(msg_p->return_path),
                              status,
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  }

 if((status == FFS_RV_ERR_FILE_ALREADY_EXISTS) && \
  ((msg_p->flags&(FSCORE_O_CREATE|FSCORE_O_EXCL)) == (FSCORE_O_CREATE|FSCORE_O_EXCL)))
  	{
  	return rfsfat_send_response_msg( FSCORE_OPEN_RSP,
                              msg_p->pair_value,
                              &(msg_p->return_path),
                              status,
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  	}

#if 0
  /* optimizations */
  status2 = flush_write_blk();
  if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS))
  {
    status = status2;
  }
#endif
  //At this point, file does either exist or is created

  /* FAT does not support write only, this will be set to readwrite. */
  if ((msg_p->flags &
        (FSCORE_O_RDONLY | FSCORE_O_WRONLY | FSCORE_O_RDWR)) == FSCORE_O_RDONLY)
  {
    ucAccessMode = FFS_OPEN_MODE_READ;
  } else
  {
    ucAccessMode = FFS_OPEN_MODE_READWRITE;
  }


  status = E_FFS_ucFileOpen_uc( mp_idx, (const T_WCHAR*)(path_no_mp),
                                ucAccessMode, msg_p->flags, &handle);
  if (status == FFS_NO_ERRORS)
  {
    kill_gbi_readcash();

    //fd bookkeeping
    fd = rfsfat_env_ctrl_blk_p->ffs_globals.fd_rr;
    fd = (fd+1) & 0x7F; //Only positive range of INT8 (its later shift left by 8)
    if (fd == 0)  //(skip 0)
      fd = 1;
    rfsfat_env_ctrl_blk_p->ffs_globals.fd_rr = (UINT8)fd;
    fd = (fd << 8) | handle;
    for (i = 0; (i < FSCORE_MAX_NR_OPEN_FSCORE_FILES); i++)
    {
      if (rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[i].fd == RFS_FD_DEF_VALUE)
      {
        rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[i].fd = fd;
        //bugfix set opmode to default value
        rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[i].op_mode = O_SIO;
        break;
      }
    }
  }


  return rfsfat_send_response_msg( FSCORE_OPEN_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status,(T_FSCORE_RET)fd);
}

/**
 *  POSIX entry for preformat*/
T_FSCORE_RET  rfsfat_ffs_preformat_uc(void *inmsg_p)
{
  T_RFSFAT_PREFORMAT_UC_REQ_MSG *msg_p;
  T_FSCORE_RET                  ret_value = FSCORE_EOK;
  UINT8                         status    = FFS_NO_ERRORS;

  msg_p = (T_RFSFAT_PREFORMAT_UC_REQ_MSG*) inmsg_p;

  if (msg_p->pathname == NULL) {
      status = FFS_RV_ERR_INVALID_PATH;
    }
  if (msg_p->magic != 0xDEAD) {
      status = FFS_RV_ERR_INVALID_PARAMS;
    }

  return rfsfat_send_response_msg( FSCORE_PREFORMAT_RSP,
                                   msg_p->pair_value,
                                   &(msg_p->return_path),
                                   status,
                                   ret_value);
}

/**
 *  POSIX entry for file or directory remove
 */
T_FSCORE_RET  rfsfat_ffs_file_remove_uc(void *inmsg_p)
{
  UINT8                   status = FFS_NO_ERRORS;
  UINT8                   status2;
  UINT8                   mp_idx;
  T_RFSFAT_REMOVE_UC_REQ_MSG  *msg_p;
  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;
  T_WCHAR *path_no_mp;



  msg_p = (T_RFSFAT_REMOVE_UC_REQ_MSG*) inmsg_p;
  if (msg_p->pathname == NULL) {
      status = FFS_RV_ERR_INVALID_PATH;
    }
  else
    {

     status = separateMountpointFromPath_uc(msg_p->pathname, &path_no_mp ,&mp_idx);

	 if (status != FFS_NO_ERRORS)
      {
         return rfsfat_send_response_msg( FSCORE_REMOVE_RSP,
                                  msg_p->pair_value,
                                  &(msg_p->return_path),
                                  status,
                                  (T_FSCORE_RET)FSCORE_EOK); //dummy
      }

   stLabLoc.mpt_idx = mp_idx;

  ucResult = ffs_ucGetLabel_uc((const T_WCHAR*)path_no_mp, &stLabLoc,&stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ffs_ucGetLabel_uc failed");
    status=ucResult;
  }
  else
  	{

  		/* Check if attribute bit 'directory' nor 'partition' is set.                  */
  		if (ATTR_CHECK_SUBDIR((&stMetDat)))
  		{
    		/* element is not file */
    		status = E_FFS_ucDmanRmDir_uc(mp_idx,path_no_mp);

  		}else {
        status = E_FFS_ucFmanUnlink_uc(mp_idx,path_no_mp);
      	}





#ifdef ICT_SPEEDUP_RFSFAT2
        status2 = ucFlushFamCash(mp_idx);
        if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS))
        {
          status = status2;
        }
#endif //ICT_SPEEDUP_RFSFAT2
        status2 = flush_write_blk();
        if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS)) {

          status = status2;
        }
        kill_gbi_readcash();
  	 }   /* end of else of (ucResult != FFS_NO_ERRORS) */
  	}   /* end of else of msg_p->pathname == NULL  */

  return rfsfat_send_response_msg( FSCORE_REMOVE_RSP,
                                   msg_p->pair_value,
                                   &(msg_p->return_path),
                                   status, FSCORE_EOK);

}

/**
 *  POSIX entry for renaming a file or directory */
T_FSCORE_RET  rfsfat_ffs_fman_rename_uc(void *inmsg_p)
{
  T_WCHAR        			*newpathName_p=NULL, *newpath_no_mp;
  T_WCHAR        			*newfileName_p=NULL, *oldpath_no_mp;
  T_WCHAR        			*oldpathName_p=NULL;
  T_WCHAR        			*oldfileName_p=NULL;
  UINT8                       status    = FFS_NO_ERRORS;
  UINT8                       status2;
  T_RFSFAT_RENAME_UC_REQ_MSG  *msg_p;
  T_FSCORE_RET                ret_value = FSCORE_EOK;
  UINT8                       oldname_mp_idx;
  UINT8                       newname_mp_idx;

  msg_p = (T_RFSFAT_RENAME_UC_REQ_MSG*) inmsg_p;



status  = separateMountpointFromPath_uc(msg_p->oldname,&oldpath_no_mp,&oldname_mp_idx);
status2 = separateMountpointFromPath_uc(msg_p->newname,&newpath_no_mp,&newname_mp_idx);

  if (status != FFS_NO_ERRORS || status2 != FFS_NO_ERRORS)
  {

    if(status == FFS_NO_ERRORS)
    {
      //separate separateMountpointFromPath_uc from the newname failed
      status = status2;
    }

    return rfsfat_send_response_msg( FSCORE_RENAME_RSP,
                              msg_p->pair_value,
                              &(msg_p->return_path),
                              status,
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  }



  if (msg_p->oldname == NULL ||
        msg_p->newname == NULL ||
        newname_mp_idx != oldname_mp_idx )
  {
      status = FFS_RV_ERR_INVALID_PATH;
    }
  else
    {
    RFSFAT_GETBUF_NO_RETURN((wstrlen(newpath_no_mp)* sizeof(T_WCHAR)),&newpathName_p);
    RFSFAT_GETBUF_NO_RETURN((wstrlen(oldpath_no_mp)* sizeof(T_WCHAR)),&oldpathName_p);

    if((newpathName_p ==NULL) || (oldpathName_p==NULL))
    	{
    	   /* free the earlier memory allocation */
		    RFSFAT_FREE_BUF(newpathName_p);
		    RFSFAT_FREE_BUF(oldpathName_p);

			return rfsfat_send_response_msg( FSCORE_RENAME_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            RFSFAT_MEMORY_ERR,
                            ret_value);


    	}

    newfileName_p = separateFileFromPath_uc((const T_WCHAR*)newpath_no_mp, (T_WCHAR*) newpathName_p);

    oldfileName_p = separateFileFromPath_uc((const T_WCHAR*)oldpath_no_mp, (T_WCHAR*)oldpathName_p);

    if((newfileName_p ==NULL) || (oldfileName_p==NULL))
    	{
    	     RFSFAT_FREE_BUF(oldpathName_p);
		  	 RFSFAT_FREE_BUF(newpathName_p);
			return rfsfat_send_response_msg( FSCORE_RENAME_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            FFS_RV_ERR_INVALID_PATH,
                            ret_value);


    	}



    status = E_FFS_ucFmanMv_uc( oldname_mp_idx,
                                oldfileName_p,
                                oldpath_no_mp,
                                newpathName_p,
                                newfileName_p);

#ifdef ICT_SPEEDUP_RFSFAT2
      status2 = ucFlushFamCash(oldname_mp_idx);  /* oldname_mp_idx == newname_mp_idx*/
      if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS))
      {
        status = status2;
      }
#endif //ICT_SPEEDUP_RFSFAT2
      status2 = flush_write_blk();
      if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS)){

        status = status2;
      }
      kill_gbi_readcash();
	  RFSFAT_FREE_BUF(oldpathName_p);
  	  RFSFAT_FREE_BUF(newpathName_p);

    }

  return rfsfat_send_response_msg( FSCORE_RENAME_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}

/**
 *  POSIX entry for removing a directory */
T_FSCORE_RET  rfsfat_ffs_dman_rmdir_uc(void *inmsg_p)
{
  UINT8                     status    = FFS_NO_ERRORS;
  UINT8                     status2;
  UINT8                  	 mp_idx;
  T_RFSFAT_RMDIR_UC_REQ_MSG *msg_p;
  T_FSCORE_RET              ret_value = FSCORE_EOK;
  T_WCHAR					*path_no_mp;

  msg_p = (T_RFSFAT_RMDIR_UC_REQ_MSG*) inmsg_p;
  if (msg_p->pathname == NULL) {
      status = FFS_RV_ERR_INVALID_PATH;
  } else {
   status = separateMountpointFromPath_uc(msg_p->pathname,&path_no_mp, &mp_idx);

    if (status != FFS_NO_ERRORS)
    {

      return rfsfat_send_response_msg( FSCORE_RMDIR_RSP,
                                msg_p->pair_value,
                                &(msg_p->return_path),
                                status,
                                (T_FSCORE_RET)FSCORE_EOK); //dummy
    }
    status = E_FFS_ucDmanRmDir_uc(mp_idx, path_no_mp);
#ifdef ICT_SPEEDUP_RFSFAT2
    status2 = ucFlushFamCash(mp_idx);
    if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS))
    {
      status = status2;
    }
#endif //ICT_SPEEDUP_RFSFAT2
    status2 = flush_write_blk();
    if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS)){

      status = status2;
    }
    kill_gbi_readcash();
  }

  return rfsfat_send_response_msg( FSCORE_RMDIR_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);


}

/**
 *  POSIX entry obtaining statistics on mountpoint, file or filesystem*/
T_FSCORE_RET  rfsfat_ffs_stat_uc(void *inmsg_p)
{
  UINT8                       mp_idx;
  UINT8                       ofctr     = 0;
  UINT8                       usedentry = 0; //for pclint only
  UINT8                       status    = FFS_NO_ERRORS;
  T_FSCORE_RET                ret_value = FSCORE_EOK;
  T_RFSFAT_STAT_UC_REQ_MSG    *msg_p;
  DIR_DIRECTORY_LABEL_STRUCT  labloc;
  UINT32                      freesize=0;
  UINT32                      totdatasize=0;
  T_WCHAR					 *path_no_mp;


  msg_p    = (T_RFSFAT_STAT_UC_REQ_MSG*) inmsg_p;

  if ( (msg_p == NULL) || (msg_p->pathname == NULL) || (msg_p->stat == NULL)) {
	  status = FFS_RV_ERR_INVALID_PARAMS;
  }
  else
  {
     //separate mountpoint from path and filename
	  status = separateMountpointFromPath_uc(msg_p->pathname,&path_no_mp,&mp_idx);
      if (status != FFS_NO_ERRORS)
      {
        return rfsfat_send_response_msg( FSCORE_STAT_RSP,
                                  msg_p->pair_value,
                                  &(msg_p->return_path),
                                  status,
                                  (T_FSCORE_RET)FSCORE_EOK); //dummy
      }


      if ((path_no_mp == NULL) || (path_no_mp[0] == EOSTR))
      {
        //Mountpoint info required

        //partition / mountpoint statistics are requested
        status = E_FFS_ucDiskFree(mp_idx, &freesize);
        if(status == FFS_NO_ERRORS)
        {
          RFSFAT_SEND_TRACE("#### diskfree succeeded, UC", RV_TRACE_LEVEL_DEBUG_LOW);
		  E_FFS_ucDiskDataSpace(mp_idx,&totdatasize);   /* Total data space */
		  
          msg_p->stat->mount_point.blk_size = rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table[mp_idx].blk_size;
		  msg_p->stat->mount_point.free_space = (freesize / KBYTES);
		  msg_p->stat->mount_point.used_size = ((totdatasize - freesize) / KBYTES);
		  msg_p->stat->mount_point.mpt_size = (totdatasize / KBYTES);
		  msg_p->stat->mount_point.mpt_id = mp_idx;
		  msg_p->stat->mount_point.nr_blks = (rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table[mp_idx].partition_size);
		  msg_p->stat->mount_point.reserved_1 = rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table[mp_idx].media_type;
		  msg_p->stat->mount_point.reserved_2 = rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table[mp_idx].filesys_type;
		  convert_FATfstype_string(rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table[mp_idx].filesys_type,msg_p->stat->mount_point.fs_type);
		  convert_mediatype_string(rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table[mp_idx].media_type,msg_p->stat->mount_point.media_type);
        }
        else
        {
          RFSFAT_SEND_TRACE("#### diskfree failed, UC", RV_TRACE_LEVEL_DEBUG_LOW);
        }
      }
      else
      {
        //Dir/file info required

        status = E_FFS_ucDiskStat_uc(path_no_mp, &labloc, mp_idx);
        if (status == FFS_NO_ERRORS)
        {
          msg_p->stat->file_dir.ino   = (labloc.uiFirstClusterNumberHigh<<16) |
                                         labloc.uiFirstClusterNumber;
          msg_p->stat->file_dir.size  =  labloc.ulFileSize;
		  msg_p->stat->file_dir.mtime = ((labloc.uiLastChangedDate <<16)|(labloc.uiLastChangedTime));
          msg_p->stat->file_dir.ctime = ((labloc.uiLastChangedDate <<16)|(labloc.uiLastChangedTime));


          if((labloc.ucAttribute & 1) == 1) {

            //file is read only
            msg_p->stat->file_dir.mode = FSCORE_IRUSR;
          } else {
            //file is read write
            msg_p->stat->file_dir.mode = FSCORE_IWUSR;
          }
           //if dir always set executable flag ON
          if((labloc.ucAttribute & 0x10) == 0x10) {

            //DIR!
            msg_p->stat->file_dir.mode |= FSCORE_IXUSR;
          }
        }
      }
      kill_gbi_readcash();

  }
  return rfsfat_send_response_msg( FSCORE_STAT_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}



/**
 *  POSIX entry for opening a directory */
T_FSCORE_RET  rfsfat_ffs_dman_opendir_uc(void *inmsg_p)
{
  UINT8                     status = FFS_NO_ERRORS;
  T_RFSFAT_OPENDIR_UC_REQ_MSG  *msg_p;
  T_FSCORE_RET              ret_value = FSCORE_EOK;
  UINT8                       mp_idx;
  T_WCHAR 					*path_no_mp;

  msg_p = (T_RFSFAT_OPENDIR_UC_REQ_MSG*) inmsg_p;

  if (msg_p->pathname == NULL) {
      status = FFS_RV_ERR_INVALID_PATH;
    }
  else if (msg_p->dirp == NULL) {
      status = FFS_RV_ERR_INVALID_PARAMS;
    }
  else
  {
		status = separateMountpointFromPath_uc(msg_p->pathname,&path_no_mp,&mp_idx);

		if (status != FFS_NO_ERRORS)
      	{
          return rfsfat_send_response_msg( FSCORE_OPENDIR_RSP,
                                  msg_p->pair_value,
                                  &(msg_p->return_path),
                                  status,
                                  (T_FSCORE_RET)FSCORE_EOK); //dummy
      	}


      //continue with opening new dir stream
      status = E_FFS_open_dir_uc(mp_idx,path_no_mp);

      if(status == FFS_NO_ERRORS) {

	  	 //save first entry found for first read action
        msg_p->dirp->opendir_ino = RFSFAT_QUERY.stLabLoc.uiFirstCl;
        msg_p->dirp->lastread_ino = RFSFAT_QUERY.stLabLoc.tLblPos;
		msg_p->dirp->mpt_idx = RFSFAT_QUERY.stLabLoc.mpt_idx;
		msg_p->dirp->resrv_1=  rfsfat_env_ctrl_blk_p->unmount_count; /* store the unmount conter value */

       //the readdir function returns 0 if the last entry is read, so the
        //return value of open dir is of no real importance
        //Because finding out the real nr of obj in this dir requires a big effort
        //the default return value is 0XFFFFFFFF, nr of objects
        ret_value = RFSFAT_MAX_RET_ENTRIES;
      }
		/* Flush the gbi readcash */
  		kill_gbi_readcash();


  }

  return rfsfat_send_response_msg( FSCORE_OPENDIR_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}




/**
 *  POSIX entry for removing a directory */
T_FSCORE_RET  rfsfat_ffs_dman_readdir_uc(void *inmsg_p)
{
  UINT8                     status = FFS_NO_ERRORS;
  T_RFSFAT_READDIR_UC_REQ_MSG  *msg_p;
  T_FSCORE_RET              ret_value = FSCORE_EOK;
  FFS_SMALL_DIRECTORY_LABEL_STRUCT  dirlabel;
  UINT8						mount_index;


  msg_p = (T_RFSFAT_READDIR_UC_REQ_MSG*) inmsg_p;

  RFSFAT_SEND_TRACE_DEBUG_HIGH("rfsfat_ffs_dman_readdir_uc....");

  //parameter check
  if( (msg_p->dirp == NULL) ||
      (msg_p->buf == NULL) ||
      ( (msg_p->pair_value.fd < 0) &&
        (msg_p->pair_value.pair_id < 0)) ||
      (msg_p->size == 0)|| (msg_p->dirp->resrv_1 != rfsfat_env_ctrl_blk_p->unmount_count))
  {
    RFSFAT_SEND_TRACE_ERROR("rfsfat_ffs_dman_readdir_uc... Invalid parameters...");

    status = FFS_RV_ERR_INVALID_PARAMS;
  }

#if 0
  else
  	{        /* Parameters are correct */
  if (msg_p->dirp->opendir_ino == ROOT_DIR_INODE_NUMBER) {


	RFSFAT_SEND_TRACE_DEBUG_HIGH("rfsfat_ffs_dman_readdir_uc....root directory");
     /* reading root directory '/'*/
	 /* read the patition names as directories */
	 mount_index =msg_p->dirp->lastread_ino;

	/* search for mounted partition */
	while((mount_index < NR_OF_MOUNTPOINTS) &&( MPT_TABLE[mount_index].is_mounted == FALSE))
			mount_index++;
	msg_p->dirp->lastread_ino = mount_index;
	if(mount_index >= NR_OF_MOUNTPOINTS)
		{
		    /* No mount points */
			ret_value = 0;
			status = FFS_NO_ERRORS;
		}
	else
		{
		   /* copy the partition name to buffer */
		   short unsigned int str_index=0, part_len=0;

		   /* verify msg_p->size */
		   for(part_len=0;(part_len<GBI_MAX_PARTITION_NAME) &&((MPT_TABLE[mount_index].partition_name[part_len])!= '\0');part_len++);

           /* it should have space to store the Unicode partition name */
           if(msg_p->size *2 < part_len)
           	{
           	  ret_value = -1;  /* return value should be an error */
			  status = FFS_RV_ERR_INVALID_PARAMS;
           	}
		   else
		   	{

		   	   for(str_index=0;str_index<part_len;str_index++)
		   		{
		          /* copy the character */
				  msg_p->buf[str_index]=(T_WCHAR)(MPT_TABLE[mount_index].partition_name[str_index]);
				  str_index++;
		   		}

		   		/* Make the last character as '\0' */
		   		msg_p->buf[str_index]='\0';
		   		ret_value = str_index;
				status = FFS_NO_ERRORS;
		   	}
		}

  	}  /* IF ROOT_DIR_INODE_NUMBER */
 #endif
  else {

        /* Fill the RFSFAT_QUERY structure */
		 RFSFAT_QUERY.stLabLoc.uiFirstCl = msg_p->dirp->opendir_ino;
         RFSFAT_QUERY.stLabLoc.tLblPos   = msg_p->dirp->lastread_ino;
         RFSFAT_QUERY.stLabLoc.mpt_idx   = msg_p->dirp->mpt_idx;


        //continue with opening new dir stream
        /* This function allocates the memory form RFSFAT_LFN_LABEL */
        status = E_FFS_ucDmanGetNext(&dirlabel);
        if(status == FFS_NO_ERRORS) {



		ret_value = (T_FSCORE_RET)wstrlen(RFSFAT_LFN_FROM_LABEL);

		if(ret_value > msg_p->size)
				ret_value = msg_p->size;

		/* copy the contents of RFSFAT_LFN_FROM_LABEL */
		 wstrncpy(msg_p->buf, RFSFAT_LFN_FROM_LABEL, ret_value);
		msg_p->buf[ret_value]=EOSTR;


        } else if (status == FFS_RV_ERR_LAST_ENTRY) {

          //last entry was found return nul length
          ret_value = (T_FSCORE_RET)RFSFAT_UNTOUCHED;
          //status signals an error but this is not an error
          status = FFS_NO_ERRORS;
        } else {
          //an error occurred return nagative value
          ret_value = (T_FSCORE_RET)FSCORE_EINVALID;
        }

	  //when the programmer didnt free the RFSFAT_LFN_FROM_LABEL buffer
      //it is done here to prevent leaking
      RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
      RFSFAT_LFN_FROM_LABEL = NULL;
	  RFSFAT_FREE_BUF(LFN_QUERY_NAME);
	  LFN_QUERY_NAME = NULL;

        msg_p->dirp->opendir_ino = RFSFAT_QUERY.stLabLoc.uiFirstCl;
        msg_p->dirp->lastread_ino = RFSFAT_QUERY.stLabLoc.tLblPos;


		/* Flush the gbi readcash */
  		kill_gbi_readcash();

	   }  /* elese part of  IF ROOT_DIR_INODE_NUMBER */

  return rfsfat_send_response_msg( FSCORE_READDIR_RSP,
                            msg_p->pair_value,
                            &(msg_p->return_path),
                            status,
                            ret_value);
}




T_FSCORE_RET  rfsfat_ffs_file_trunc_name_uc(void *inmsg_p)
{
  T_RFSFAT_TRUNC_NAME_UC_REQ_MSG *msg_p;
  UINT8                   status = FFS_NO_ERRORS,status2=FFS_NO_ERRORS;
  T_FSCORE_PAIR_VALUE     pair_value;
  UINT8					  mp_idx;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
   DIR_DIRECTORY_LABEL_STRUCT  stMetDat;
   T_WCHAR 				*path_no_mp;

  msg_p=(T_RFSFAT_TRUNC_NAME_UC_REQ_MSG *) inmsg_p;
  pair_value.fd =0;
  pair_value.pair_id = msg_p->pair_id;


  if(msg_p->pathname == NULL || msg_p->size < 0)
  return rfsfat_send_response_msg( FSCORE_TRUNC_NAME_RSP,pair_value,&(msg_p->return_path),
						                            FFS_RV_ERR_INVALID_PARAMS, FSCORE_EOK);

  if (msg_p->size == 0)
	return rfsfat_send_response_msg( FSCORE_TRUNC_NAME_RSP,pair_value  , &(msg_p->return_path),
						                            FFS_NO_ERRORS, FSCORE_EOK);


  /* determine the mount point index by part_num of the message */
  mp_idx = separateMountpointFromPath_uc(msg_p->pathname, &path_no_mp,&mp_idx);

  if(status != FFS_NO_ERRORS)
  	{
  	 return rfsfat_send_response_msg( FSCORE_TRUNC_NAME_RSP,
					                            pair_value,
					                            &(msg_p->return_path),
					                            status,
					                            0);

  	}

 /* save the partition index */
 stLabLoc.mpt_idx = mp_idx;

/* FAT does not support access mode: ignored */
  status =  ffs_ucGetFileLocation_uc (path_no_mp, &stLabLoc, &stMetDat);

  if(status != FFS_NO_ERRORS)
  	{
  	 return rfsfat_send_response_msg( FSCORE_TRUNC_NAME_RSP,
					                            pair_value,
					                            &(msg_p->return_path),
					                            status,
					                            0);

  	}

  /* File is opened and you have proper file fd */
  status = E_FFS_ucFmanTrunc (stLabLoc,msg_p->size);


  status2 = flush_write_blk();
  if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS)){

      status = status2;
    }


 	  /* Flush GBI read cash */
	   kill_gbi_readcash();


  return rfsfat_send_response_msg( FSCORE_TRUNC_NAME_RSP,
					                            pair_value,
					                            &(msg_p->return_path),
					                            status,
					                            FSCORE_EOK);

}



T_FSCORE_RET  rfsfat_ffs_getattribute_uc(void *inmsg_p)
{
 T_RFSFAT_GETATTRIB_UC_REQ_MSG *msg_p;
 UINT8						attrib=0;
 UINT8                     status = FFS_NO_ERRORS;
 T_FSCORE_PAIR_VALUE     pair_value;
 UINT8					mp_idx;
 T_WCHAR 				*path_no_mp;

 msg_p = (T_RFSFAT_GETATTRIB_UC_REQ_MSG *) inmsg_p;

 pair_value.fd =0;
 pair_value.pair_id = msg_p->pair_id;

 status  = separateMountpointFromPath_uc(msg_p->path_name,&path_no_mp,&mp_idx);

 if(status == FFS_NO_ERRORS)
 	{
	 status = E_FFS_ucFmanGetAttributes_uc(path_no_mp,&attrib,mp_idx);
 	}

 return rfsfat_send_response_msg( FSCORE_GETATTRIB_RSP,
												pair_value,
					                            &(msg_p->return_path),
					                            status,
					                            attrib);
}


T_FSCORE_RET  rfsfat_ffs_setattribute_uc(void *inmsg_p)
{

 T_RFSFAT_SETATTRIB_UC_REQ_MSG *msg_p;
 UINT8                     status = FFS_NO_ERRORS;
 T_FSCORE_PAIR_VALUE     pair_value;
 UINT8					mp_idx;
 T_WCHAR 				*path_no_mp;


 msg_p = (T_RFSFAT_SETATTRIB_UC_REQ_MSG *) inmsg_p;

 pair_value.fd =0;
 pair_value.pair_id = msg_p->pair_id;

 status  = separateMountpointFromPath_uc(msg_p->path_name,&path_no_mp,&mp_idx);

if(status == FFS_NO_ERRORS)
 	{
 	status = E_FFS_ucFmanSetAttributes_uc(path_no_mp,msg_p->attrib,mp_idx);
	if (status == FFS_NO_ERRORS)
		{
			status = flush_write_blk();
		}

	}

 return rfsfat_send_response_msg( FSCORE_SETATTRIB_RSP,
					                            pair_value,
					                            &(msg_p->return_path),
					                            status,
					                          FSCORE_EOK);
}



T_FSCORE_RET  rfsfat_ffs_getdatetime_uc(void *inmsg_p)
{
 T_RFSFAT_GETDATETIME_UC_REQ_MSG *msg_p;
 UINT8                     status = FFS_NO_ERRORS;
 T_FSCORE_PAIR_VALUE     pair_value;
 UINT8					mp_idx;
 UINT16 				var_date,var_time,var_cdate,var_ctime;
  T_WCHAR 				*path_no_mp;

 msg_p = (T_RFSFAT_GETDATETIME_UC_REQ_MSG *) inmsg_p;

 pair_value.fd =0;
 pair_value.pair_id = msg_p->pair_id;


if((msg_p->date_time_p == NULL) && (msg_p->crdate_time_p == NULL))
 	{
 	      status = FFS_RV_ERR_INVALID_PARAMS;

		return rfsfat_send_response_msg( FSCORE_GETDATETIME_RSP,
					                           pair_value,
					                            &(msg_p->return_path),
					                            status,
				                            FSCORE_EOK);

 	}


 status  = separateMountpointFromPath_uc(msg_p->path_name,&path_no_mp,&mp_idx);

 if(status == FFS_NO_ERRORS)
 	{
	 status = E_FFS_ucFmanGetDateTime_uc (path_no_mp, &var_date,&var_time,&var_cdate,&var_ctime,mp_idx);
	 if(status == FFS_NO_ERRORS)
	 	{

		 if(msg_p->date_time_p != NULL)
		 	{
 				RFSFAT_SEND_TRACE("rfsfat_ffs_getdatetime_uc, retriving last updated date & time", RV_TRACE_LEVEL_DEBUG_LOW);
	 	  if(convertfat_datetime_to_local(var_date, var_time,msg_p->date_time_p) != 0)
		  	       status = FFS_RV_ERR_INVALID_PARAMS;
		 	}

		 if(msg_p->crdate_time_p != NULL)
		 	{
		 	 	RFSFAT_SEND_TRACE("rfsfat_ffs_getdatetime_uc, retriving created date & time", RV_TRACE_LEVEL_DEBUG_LOW);
	 	  		if(convertfat_datetime_to_local(var_cdate, var_ctime,msg_p->crdate_time_p) != 0)
		  	       status = FFS_RV_ERR_INVALID_PARAMS;
		 	}


	 	}

 	}

 return rfsfat_send_response_msg( FSCORE_GETDATETIME_RSP,
												pair_value,
					                            &(msg_p->return_path),
					                            status,
					                            FSCORE_EOK);
}








/**
 *  POSIX entry for the fstat function (status info on open file) */
T_FSCORE_RET  rfsfat_ffs_fman_chmod_uc(void *inmsg_p)
{
  UINT8                   status = FFS_NO_ERRORS;
  T_RFSFAT_CHMOD_UC_REQ_MSG  *msg_p;
  T_FSCORE_RET            ret_value = FSCORE_EOK;


  msg_p = (T_RFSFAT_CHMOD_UC_REQ_MSG*) inmsg_p;

  /* Just return success, All chmod parameters should work because FAT file system does not have
     groups, super user*/

  return rfsfat_send_response_msg( FSCORE_CHMOD_RSP,
                                    msg_p->pair_value,
                                    &(msg_p->return_path),
                                    status,
                                    ret_value);
}


/**************************************************/
//
// end of Unicode FFS
//
/**************************************************/
#endif



UINT8 RFSFAT_get_mpt_idx(const UINT8* pucPath, UINT16 *index_p)
{
  UINT8 mpt_idx;
  UINT16 nr_mptcntr=0,pathcntr,path_start,in_path_cntr;

 /* Skip all leading slashs, SPACES  */
  for(path_start=0;((pucPath[path_start] == FFS_ASCII_VALUE_FWSLASH) || (pucPath[path_start] == FFS_ASCII_VALUE_SPACE));path_start++);


 /* Now path_start will have index to first char of mount point name */
 for(mpt_idx = RFSFAT_INVALID_MPT,nr_mptcntr=0;((mpt_idx == RFSFAT_INVALID_MPT) && (nr_mptcntr < NR_OF_MOUNTPOINTS)); nr_mptcntr++)

 {

	if((MPT_TABLE[nr_mptcntr].is_mounted ==TRUE))
	{
	  //find out if mountpoint names match
	  for (pathcntr = 0, in_path_cntr=path_start; pathcntr < GBI_MAX_PARTITION_NAME; pathcntr++, in_path_cntr++)
		{

		  if (toupper(pucPath[in_path_cntr]) != toupper( MPT_TABLE[nr_mptcntr].partition_name[pathcntr]))
			{
			  if (pucPath[in_path_cntr] == FFS_ASCII_VALUE_FWSLASH)
				{

				  //store index in mountpoint table
				  mpt_idx = nr_mptcntr;

				}
			   break;   /* Move out of inner for loop */

			}
		  else if (pucPath[in_path_cntr] == FFS_ASCII_EOL)
			{
			  //store index in mountpoint table
			  mpt_idx = nr_mptcntr;
			  break;   /* Move out of inner for loop */
			}
		}   /* Enf of for loop */
	  if (pathcntr == GBI_MAX_PARTITION_NAME)
		{
		  //store index in mountpoint table
		  mpt_idx = nr_mptcntr;
		}
	  }  /* End of IF */

	}   /* End of For */

 if(index_p != NULL)
 	{
 	     /* store the index of the path */
		 *index_p=in_path_cntr;    /* Index at '/' character or '\0' character */
 	}

   return mpt_idx;
}





T_FSCORE_RET  rfsfat_ffs_mount_unmount (void *inmsg_p)
{
 T_RFSFAT_MOUNT_UNMOUNT_REQ_MSG *msg_p;
 UINT8                     status = FFS_NO_ERRORS;
 T_FSCORE_PAIR_VALUE     pair_value;
 T_FSCORE_CMD_ID   rsp_id;


 msg_p = (T_RFSFAT_MOUNT_UNMOUNT_REQ_MSG *) inmsg_p;

 pair_value.fd =0;
 pair_value.pair_id = msg_p->pair_id;


if(msg_p->op == RFSFAT_MOUNT)  /* Mount operation */
{
 RFSFAT_SEND_TRACE("rfsfat_ffs_mount_unmount, Mount operation \n\r", RV_TRACE_LEVEL_DEBUG_LOW);
 status=MMA_ucMount();
 rsp_id = FSCORE_MOUNT_RSP;

}
else if(msg_p->op == RFSFAT_UNMOUNT )  /* UnMount  operation */
{
 RFSFAT_SEND_TRACE("rfsfat_ffs_mount_unmount, Un Mount operation \n\r", RV_TRACE_LEVEL_DEBUG_LOW);
 status = MMA_ucUnMount();
  rsp_id = FSCORE_UNMOUNT_RSP;
}
else
{
 RFSFAT_SEND_TRACE("rfsfat_ffs_mount_unmount, Invalid operation \n\r", RV_TRACE_LEVEL_ERROR);
   status = FFS_RV_ERR_INVALID_PARAMS;
}

 return rfsfat_send_response_msg( rsp_id,
												pair_value,
					                            &(msg_p->return_path),
					                            status,
					                            FSCORE_EOK);
}








T_FSCORE_RET  rfsfat_ffs_file_trunc_name(T_RV_HDR *inmsg_p)
{
  T_RFSFAT_TRUNC_NAME_REQ_MSG *msg_p;
  UINT8                   status = FFS_NO_ERRORS,status2=FFS_NO_ERRORS;
  T_FSCORE_PAIR_VALUE     pair_value;
  short 				  i;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT  stMetDat;


  msg_p=(T_RFSFAT_TRUNC_NAME_REQ_MSG *) inmsg_p;
  pair_value.fd =0;
  pair_value.pair_id = msg_p->pair_id;


  if(msg_p->path_name == NULL || msg_p->size < 0)
  return rfsfat_send_response_msg( FSCORE_TRUNC_NAME_RSP,pair_value,&(msg_p->return_path),
						                            FFS_RV_ERR_INVALID_PARAMS, FSCORE_EOK);

  if (msg_p->size == 0)
	return rfsfat_send_response_msg( FSCORE_TRUNC_NAME_RSP,pair_value  , &(msg_p->return_path),
						                            FFS_NO_ERRORS, FSCORE_EOK);

/* FAT does not support access mode: ignored */
  status = ffs_ucGetFileLocation ((RW_PTR_TO_RO(UINT8))msg_p->path_name, &stLabLoc, &stMetDat);


  if(status != FFS_NO_ERRORS)
  	{

    RFSFAT_SEND_TRACE ("RFSFAT ffs_ucGetFileLocation failed",
             RV_TRACE_LEVEL_ERROR);

  	 return rfsfat_send_response_msg( FSCORE_TRUNC_NAME_RSP,
					                            pair_value,
					                            &(msg_p->return_path),
					                            status,
					                            0);

  	}

  /* File is opened and you have proper file fd */
  status = E_FFS_ucFmanTrunc (stLabLoc,msg_p->size);


   status2 = flush_write_blk();
    if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS)){

      status = status2;
    }


  	  /* Flush GBI read cash */
	   kill_gbi_readcash();


  return rfsfat_send_response_msg( FSCORE_TRUNC_NAME_RSP,
					                            pair_value,
					                            &(msg_p->return_path),
					                            status,
					                            FSCORE_EOK);

}


T_FSCORE_RET rfsfat_file_read_by_name(T_RV_HDR * inmsg_p)
{
  T_RFSFAT_FILE_READ_REQ_MSG *msg_p;
  UINT8                   status = FFS_NO_ERRORS;
  UINT8                   handle;
  FFS_FILE_LENGTH_TYPE	  size=0;
  T_FSCORE_PAIR_VALUE     pair_value;



  msg_p=(T_RFSFAT_FILE_READ_REQ_MSG*) inmsg_p;

  pair_value.fd =0;
  pair_value.pair_id = msg_p->pair_id;


  if((msg_p->path_name == NULL) || (msg_p->size < 0) || (msg_p->buf == NULL))
  return rfsfat_send_response_msg( FSCORE_FILE_READ_RSP,pair_value,&(msg_p->return_path),
						                            FFS_RV_ERR_INVALID_PARAMS, FSCORE_EOK);

  if (msg_p->size == 0)
	return rfsfat_send_response_msg( FSCORE_FILE_READ_RSP,pair_value, &(msg_p->return_path),
						                            FFS_NO_ERRORS, FSCORE_EOK);

/* FAT does not support access mode: ignored */
  status = E_FFS_ucFileOpen((UINT8*)msg_p->path_name,FFS_OPEN_MODE_READ, FSCORE_O_RDWR, &handle);

  if(status != FFS_NO_ERRORS)
  	{
  	 return rfsfat_send_response_msg( FSCORE_FILE_READ_RSP,
					                            pair_value,
					                            &(msg_p->return_path),
					                            status,
					                            0);

  	}

 /* check the buffer and size to read */
if(msg_p->buf != NULL && msg_p->size > 0)
{
  /* read the file contents to buffer */
  size = msg_p->size;
  status = E_FFS_ucFileRead((UINT8)handle, &size, (UINT8*)msg_p->buf);
  if(status != FFS_NO_ERRORS)
  	{
  		size = 0;
  		return rfsfat_send_response_msg( FSCORE_FILE_READ_RSP,
					                            pair_value,
					                            &(msg_p->return_path),
					                            status,
					                            size);
  	}
}
 /* close the file */
  status = E_FFS_ucFileClose((UINT8)handle);


  if(status == FFS_NO_ERRORS)
  	{
  	  /* Flush GBI read cash */
	   kill_gbi_readcash();
  	}


	return rfsfat_send_response_msg( FSCORE_FILE_READ_RSP,
					                            pair_value,
					                            &(msg_p->return_path),
					                            status,
					                            size);
}
T_FSCORE_RET rfsfat_file_write_by_name(T_RV_HDR * inmsg_p)
{
  char                    pathonly[255];
  char                    *pathonly_p;
  const char              *nameAndExt_p;
  UINT8                   status = FFS_NO_ERRORS;
  UINT8                   status2;
  UINT8                   ucAccessMode;
  UINT8                   handle;
  T_RFSFAT_FILE_WRITE_REQ_MSG	*msg_p;
  BOOL                    flag_create = FALSE;
  FFS_FILE_LENGTH_TYPE	  write_size=0;

  T_FSCORE_PAIR_VALUE     pair_value;



  msg_p = (T_RFSFAT_FILE_WRITE_REQ_MSG*) inmsg_p;

  pair_value.fd =0;
  pair_value.pair_id = msg_p->pair_id;

  /* parameter check */
  if (msg_p->path_name == NULL) {
	status = FFS_RV_ERR_INVALID_PATH;
  }
  if (((msg_p->flags & FSCORE_O_WRONLY) != FSCORE_O_WRONLY ) && ((msg_p->flags & FSCORE_O_RDONLY) != FSCORE_O_RDONLY)) {
	status = FFS_RV_ERR_INVALID_PARAMS;
  }

  if (status != FFS_NO_ERRORS) {
    return rfsfat_send_response_msg( FSCORE_FILE_WRITE_RSP,
                              pair_value,
                              &(msg_p->return_path),
                              status,
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  }

  pathonly_p = &pathonly[0];
  //separate file name and path (partition name+directories) from pathname
  nameAndExt_p = separateFileAndPath(pathonly_p, (const char*)msg_p->path_name);

  //file does not exist, may we create it?
  if ((msg_p->flags & FSCORE_O_CREATE) == FSCORE_O_CREATE)
  {
    flag_create = TRUE;
  }
  status = E_FFS_ucFileCreate((UINT8*)pathonly, (UINT8*)nameAndExt_p, flag_create);
  if ((status != FFS_NO_ERRORS) && (status != FFS_RV_ERR_FILE_ALREADY_EXISTS))
  {
    //Error, failed to create new file.
    return rfsfat_send_response_msg( FSCORE_FILE_WRITE_RSP,
                              pair_value,
                              &(msg_p->return_path),
                              status,
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  }

  if((status == FFS_RV_ERR_FILE_ALREADY_EXISTS) && \
  ((msg_p->flags&(FSCORE_O_CREATE|FSCORE_O_EXCL)) == (FSCORE_O_CREATE|FSCORE_O_EXCL)))
  	{
  	return rfsfat_send_response_msg( FSCORE_FILE_WRITE_RSP,
                              pair_value,
                              &(msg_p->return_path),
                              status,
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  	}

  status2 = flush_write_blk();
  if ((status == FFS_NO_ERRORS) && (status2 != FFS_NO_ERRORS))
  {
    status = status2;
  }
  //At this point, file does either exist or is created

  /* FAT does not support write only, this will be set to readwrite. */
  if ((msg_p->flags &
        (FSCORE_O_RDONLY | FSCORE_O_WRONLY | FSCORE_O_RDWR)) == FSCORE_O_RDONLY)
  {
    ucAccessMode = FFS_OPEN_MODE_READ;
  } else
  {
    ucAccessMode = FFS_OPEN_MODE_READWRITE;
  }

  /* FAT does not support access mode: ignored */
  status = E_FFS_ucFileOpen((UINT8*)msg_p->path_name, ucAccessMode, msg_p->flags, &handle);


if(status == FFS_NO_ERRORS)
{
	if(msg_p->buf != NULL && msg_p->size > 0)
	{
  		write_size = msg_p->size;
  		status = E_FFS_ucFileWrite((UINT8)handle, &write_size, (UINT8*)msg_p->buf);
  		if(status != FFS_NO_ERRORS)
  			{
  				write_size = 0;
  				return rfsfat_send_response_msg( FSCORE_FILE_WRITE_RSP,
                            pair_value,
                            &(msg_p->return_path),
                            status,
                            write_size);
  			}
	}
}

/* close the file */
  status = E_FFS_ucFileClose((UINT8)handle);


  if(status == FFS_NO_ERRORS)
  	{
  	  /* Flush GBI read cash */
	   kill_gbi_readcash();
  	}

return rfsfat_send_response_msg( FSCORE_FILE_WRITE_RSP,
                            pair_value,
                            &(msg_p->return_path),
                            status,
                            write_size);


}

T_FSCORE_RET rfsfat_ffs_vol_set_label(T_RV_HDR * inmsg_p)
{
  UINT8                   a_path[20];
  UINT8                   a_label[20];
  UINT8                   status = FFS_NO_ERRORS;
  UINT8                   mpt_idx;
  UINT16                   foundcntr = 0;
  T_RFSFAT_SET_VOL_LABEL_REQ_MSG	*msg_p;

  T_FSCORE_PAIR_VALUE     pair_value;

  msg_p = (T_RFSFAT_SET_VOL_LABEL_REQ_MSG*) inmsg_p;

  pair_value.fd =0;
  pair_value.pair_id = msg_p->pair_id;

  /* parameter check */
  convert_unicode_to_u8(msg_p->path_name, (char *)a_path);
  convert_unicode_to_u8(msg_p->vol_label, (char *)a_label);

  if( (a_path == NULL)  || (a_label == NULL) )
 {
	status = FFS_RV_ERR_INVALID_PATH;
  }

  mpt_idx = RFSFAT_get_mpt_idx(a_path, &foundcntr);

  if (mpt_idx== RFSFAT_INVALID_MPT)
  {
       RFSFAT_SEND_TRACE_ERROR("RFSFAT: rfsfat_ffs_vol_get_label : Invalid mouint point.");
	status = FFS_RV_ERR_INVALID_PATH;
  }
  else
  {
      status = E_FFS_ucDiskSetVolumeLabel(mpt_idx, (const char *)a_label);

      // Write the label in the boot sector.
      if (status == FFS_NO_ERRORS)
      {
          if(MMA_ucSetBootSectorLabel(mpt_idx, a_label) != FFS_NO_ERRORS )
          {
              RFSFAT_SEND_TRACE_ERROR("RFSFAT:  Failed to set label in boot sector");
          }
       }
  }

  if (status != FFS_NO_ERRORS) {
    return rfsfat_send_response_msg( FSCORE_SET_VOL_LABEL_RSP,
                              pair_value,
                              &(msg_p->return_path),
                              status,
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  }

  return rfsfat_send_response_msg( FSCORE_SET_VOL_LABEL_RSP,
                            pair_value,
                            &(msg_p->return_path),
                            status,
                            0);

}

T_FSCORE_RET rfsfat_ffs_vol_get_label(T_RV_HDR * inmsg_p)
{
  UINT8                   a_path[20];
  UINT8                   a_label[20];
  UINT8                   status = FFS_NO_ERRORS;
  UINT8                   mpt_idx;
  UINT16                   foundcntr = 0;
  T_RFSFAT_GET_VOL_LABEL_REQ_MSG	*msg_p;

  T_FSCORE_PAIR_VALUE     pair_value;

  msg_p = (T_RFSFAT_GET_VOL_LABEL_REQ_MSG*) inmsg_p;

  pair_value.fd =0;
  pair_value.pair_id = msg_p->pair_id;

  /* parameter check */
  convert_unicode_to_u8(msg_p->path_name, (char *)a_path);
  convert_unicode_to_u8(msg_p->vol_label, (char *)a_label);
  if (a_path == NULL) {
	status = FFS_RV_ERR_INVALID_PATH;
  }

  mpt_idx = RFSFAT_get_mpt_idx(a_path, &foundcntr);

  if (mpt_idx== RFSFAT_INVALID_MPT) {
	status = FFS_RV_ERR_INVALID_PATH;
       RFSFAT_SEND_TRACE_ERROR("RFSFAT: rfsfat_ffs_vol_get_label : Invalid mouint point.");
  }

  status = E_FFS_ucDiskGetVolumeLabel(mpt_idx, (char *)a_label);

  if (status != FFS_NO_ERRORS) {
    return rfsfat_send_response_msg( FSCORE_GET_VOL_LABEL_RSP,
                              pair_value,
                              &(msg_p->return_path),
                              status,
                              (T_FSCORE_RET)FSCORE_EOK); //dummy
  }

  convert_u8_to_unicode((char *)a_label, msg_p->vol_label);
  return rfsfat_send_response_msg( FSCORE_GET_VOL_LABEL_RSP,
                            pair_value,
                            &(msg_p->return_path),
                            status,
                            0);

}



/* This function is used to place the time as it is stored in a FAT label
* in a tm struct (c type) and return a time_t timetype by calling timelocal
*
* @param             UINT16 Time
* @param             UINT16 Date
*
* @return            time_t the time in seconds since 1970
*/
time_t converttime( UINT16 Time, UINT16 Date)
{
  struct tm storedtm;

  /*
       Time integer:
       MSB %HHHH.HMMM.MMMS.SSSS   LSB
       H = hours
       M = Minutes
       S = Seconds in 2-second steps.
     */
  storedtm.tm_sec  = (UINT8)(Time&0x001F);
  storedtm.tm_min  = (UINT8)((Time&0x07E0)>>5);
  storedtm.tm_hour = (UINT8)((Time&0xF800)>>11);

  /*
       Date integer:
       MSB %YYYY.YYYM.MMMD.DDDD   LSB
       Y = Year
       M = Month
       D = Day
     */

  storedtm.tm_mday = (UINT8)(Date&0x001F);
  storedtm.tm_mon  = (UINT8)((Date&0x01E0)>>5);
  storedtm.tm_year = (UINT8)(((Date&0xFE00)>>9)+80);   /* year since 1900 */


  return mktime(&storedtm);

}


UINT8* string_toupper(UINT8* const string)
{
  UINT16 loop;

  for(loop=0;loop<strlen((const char*)string);loop++)
    string[loop]=toupper(string[loop]);

  return string;
}

