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
  | Purpose:    RELIANCE API function interface
  +----------------------------------------------------------------------------*/

#include "relfs.h"
#include "relfs_pei.h"
#include "relfs_env.h"
#include "reliance_rtos.h"
#include "nucleus.h"
#include "rfs/rfs_api.h"
#include "rfs/fscore_types.h"
#define MSG_ALLOC(mail) if ((rvf_get_buf(ril_env_ctrl_blk_p->mbid, sizeof(T_Rel_req), \
                                         (T_RVF_BUFFER*) &mail)) == RVF_RED) { \
                                  return EFFS_MEMORY; \
                        }

#define MSG_SEND(mail)  if (rvf_send_msg(ril_env_ctrl_blk_p->addr_id, mail)) { \
                             return EFFS_MSGSEND; \
                         }


#define RELFS_RFS_STREAM_RET_PATH  (&(ril_env_ctrl_blk_p->rfs_stream_return_path))
#define RELFS_RFS_FILE_RET_PATH  (&(ril_env_ctrl_blk_p->rfs_file_return_path))




extern T_HANDLE                 ril_comm_handle;
extern T_HANDLE                 ril_task_handle;

static int32	rel_get_free_pair_id (void);
static unsigned short ffs_map_ril_flag(T_FFS_OPEN_FLAGS flags );
/* Structure containing date and time */

/*decalre date & time related functions*/
BOOL sec_into_datetime(unsigned long date_time_in_sec, T_RTC_DATE_TIME *temp_time);
time_t	datetime_into_sec(T_RTC_DATE_TIME *datetime);



extern time_t Rtc_get_datetime_wrapper();

/* EXTENED FFS functions */
static T_FFS_REQ_ID ffs_open_uc_nb(const T_WCHAR *pathname, T_FFS_OPEN_FLAGS option,T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_read_nb(T_FFS_FD fd,void* pBuf, T_FFS_SIZE size,T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_mkdir_uc_nb(const T_WCHAR *pathname,T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_truncate_uc_nb(const T_WCHAR *path, T_FFS_OFFSET length, T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_stat_rfs_uc_nb(const T_WCHAR* path, T_FSCORE_STAT * stat,T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_fstat_rfs_nb ( T_FFS_FD fd, T_FSCORE_STAT * stat,T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_format_uc_nb(const T_WCHAR *name, uint16 magic, T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_remove_uc_nb(const T_WCHAR *pathname, T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_rmdir_uc_nb(const T_WCHAR *pathname, T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_opendir_rfs_uc_nb(const T_WCHAR *name, T_FSCORE_DIR *dir, T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_readdir_rfs_uc_nb(T_FSCORE_DIR *dir, T_WCHAR *buf, T_FFS_SIZE size,T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_closedir_rfs_nb(T_FSCORE_DIR * dir_p,T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_rename_uc_nb(const T_WCHAR *oldname, const T_WCHAR *newname,T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_setpartition_rfs_nb(T_FSCORE_PARTITION_TABLE * partition_table,T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_get_attribute_rfs_uc_nb(const T_WCHAR* path,T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_set_attribute_rfs_uc_nb(const T_WCHAR* path,T_FFS_ATTRIB_TYPE attrib, T_RV_RETURN *cp);
static T_FFS_REQ_ID ffs_fchmod_rfs_nb(T_FFS_FD fd, T_RV_RETURN * cp);
static T_FFS_REQ_ID ffs_chmod_rfs_uc_nb(const T_WCHAR* path,T_RV_RETURN *cp);




/* FFS-RFS interface functions */
static T_WCHAR *  		skip_mount_point_name(T_WCHAR * pathname);
static T_FFS_OPEN_FLAGS convert_flags_rfs_to_ffs(T_FSCORE_FLAGS flags);

static T_FSCORE_RET 	relfs_rfs_close (T_FSCORE_FD fd);
static T_FSCORE_RET 	relfs_rfs_fchmod (T_FSCORE_FD fd, T_FSCORE_MODE mode);
static T_FSCORE_RET 	relfs_rfs_fcntl (T_FSCORE_FD fd, INT8 cmd, void *arg_p);
static T_FSCORE_SIZE 	relfs_rfs_write (T_FSCORE_FD fd,const void *buf, T_FSCORE_SIZE size);
static T_FSCORE_RET 	relfs_rfs_trunc (T_FSCORE_FD fd, T_FSCORE_SIZE size);
static T_FSCORE_RET 	relfs_rfs_fstat (T_FSCORE_FD fd, T_FSCORE_STAT * stat);
static T_FSCORE_RET 	relfs_rfs_fsync (T_FSCORE_FD fd);
static T_FSCORE_OFFSET 	relfs_rfs_lseek (T_FSCORE_FD fd, T_FSCORE_OFFSET offset, T_FSCORE_WHENCE whence);
static T_FSCORE_RET 	relfs_rfs_set_partition_table (T_FSCORE_PARTITION_TABLE * partition_table,T_RV_RETURN return_path);
static T_FSCORE_SIZE  	relfs_rfs_read (T_FSCORE_FD fd, void *buf, T_FSCORE_SIZE size);
static T_FSCORE_RET 	relfs_rfs_readdir_uc_nb (T_FSCORE_DIR * dirp,T_WCHAR *buf, T_FSCORE_SIZE size, T_RV_RETURN_PATH return_path);
static T_FSCORE_RET    	relfs_rfs_closedir_nb(T_FSCORE_DIR   *dirp, T_RV_RETURN   return_path);
static T_FSCORE_RET 	relfs_rfs_format_uc_nb(const T_WCHAR*	pathname,const T_WCHAR  *name,UINT16  magic,T_RV_RETURN_PATH return_path);
static T_FSCORE_RET 	relfs_rfs_mkdir_uc_nb(const T_WCHAR   *pathname, T_FSCORE_MODE   mode,T_RV_RETURN_PATH  return_path);
static T_FSCORE_RET 	relfs_rfs_opendir_uc_nb (const T_WCHAR *pathname, T_FSCORE_DIR * dirp,T_RV_RETURN_PATH return_path);
static T_FSCORE_RET 	relfs_rfs_open_uc_nb(const T_WCHAR*	pathname,T_FSCORE_FLAGS  flags,T_FSCORE_MODE  mode, T_RV_RETURN_PATH return_path);
static T_FSCORE_RET 	relfs_rfs_preformat_uc_nb(const T_WCHAR  *pathname,UINT16  magic,T_RV_RETURN_PATH return_path);
static T_FSCORE_RET 	relfs_rfs_remove_uc_nb( const T_WCHAR  *pathname, T_RV_RETURN_PATH return_path);
static T_FSCORE_RET 	relfs_rfs_rename_uc_nb(		const T_WCHAR *oldname,const T_WCHAR *newname,T_RV_RETURN_PATH return_path);
static T_FSCORE_RET 	relfs_rfs_rmdir_uc_nb(const T_WCHAR *pathname, T_RV_RETURN_PATH return_path);
static T_FSCORE_RET 	relfs_rfs_stat_uc_nb( const T_WCHAR  *pathname, T_FSCORE_STAT  *stat,T_RV_RETURN_PATH return_path);
static T_FSCORE_RET 	relfs_rfs_chmod_uc_nb (const T_WCHAR *pathname, T_FSCORE_MODE mode, T_RV_RETURN return_path);
static T_FSCORE_RET 	relfs_rfs_getattribute_uc_nb(const T_WCHAR *pathname,T_RV_RETURN_PATH return_path);
static T_FSCORE_RET 	relfs_rfs_setattribute_uc_nb(const T_WCHAR *pathname,T_FFS_ATTRIB_TYPE attrib,T_RV_RETURN_PATH return_path);
static T_FSCORE_RET 	relfs_rfs_getdatetime_uc_nb(const T_WCHAR *pathname,T_RTC_DATE_TIME *date_time_p,T_RTC_DATE_TIME * crdate_time_p,T_RV_RETURN_PATH return_path);
static T_FSCORE_RET 	relfs_rfs_mount_unmount_nb(int op,	T_RV_RETURN_PATH          return_path);


/* ******************************************************
NAME : ffs_open()

FUNCTIONALITY: Opens the file

INPUTS: pathname : Complete path to the File to be opened
optin	 : The Permission list that the file may be
opened with.

RETRUN : <0 in case of failures
File Descriptor in case of success.

 ********************************************************* */
T_FFS_FD ffs_open(const char *pathname, T_FFS_OPEN_FLAGS option)
{
    T_FFS_FD fd;
    unsigned short uMode;
    unsigned short flags;
    int status = REL_OK;

    /* Check wether the entering task is already registered to the reliance file system or not*/
    if(relFs_Check_File_User())
    {
    	/* If not register */
	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return ffs_map_ril_error(status);
    }

	if(option == FFS_O_EMPTY)
		{
		    return EFFS_INVALID;
		}

    /* Reliance is not handling FFS_O_RDONLY | FFS_O_APPEND.So test it here */
    if((option & FFS_O_RDONLY ) && (option & FFS_O_APPEND))
    {
	if(!(option & FFS_O_WRONLY))
	{
	    return EFFS_INVALID;
	}
    }
    if((option & FFS_O_APPEND) && (option & FFS_O_TRUNC))
	option ^=  FFS_O_APPEND;

    if(option & FFS_O_TRUNC)
	option |=  FFS_O_CREATE;

    /*Find file mode. This is valid for Reliance FS only */
    if (option & FFS_O_CREATE)
    {
	if(option & (FFS_O_WRONLY | FFS_O_RDWR))
	    uMode = REL_IWRITE | REL_IREAD;
	else
	    uMode = REL_IREAD;
    }
    else
	uMode = 0x0000;
    /* Map the ffs2fs file flag to Reliance file flags */
    flags = ffs_map_ril_flag(option);

    /* Call reliancne Open function */
    fd = relFs_Open(pathname, flags, uMode);
    if (fd >= REL_OK)
	return fd;
    else
	return ffs_map_ril_error(fd);
}


/* *********************************************************

NAME : ffs_open_nb()

FUNCTIONALITY: Opens a File

INPUTS: pathname : Complete path to the File to be opened
option	 : The Permission list that the file may be
opened with.
cp		 : The return path for non blocking fucntionality.

RETRUN : <0 in case of failures
File Descriptor in case of success.

 ********************************************************* */


T_FFS_REQ_ID ffs_open_nb(const char *pathname, T_FFS_OPEN_FLAGS option,T_RV_RETURN *cp)
{
   	T_Rel_req *Req; MSG_ALLOC(Req);
	memset(Req, 0, sizeof(T_Rel_req));
	Req->msg = REL_OPEN_NB_REQ;
	Req->option = option;
	Req->pathname = pathname;
	Req->uMode = 0;
	Req->req_id = rel_get_free_pair_id ();
	Req->cp = cp;
	MSG_SEND(Req);
	return Req->req_id;
}




/* *********************************************************

NAME : ffs_open_uc_nb()

FUNCTIONALITY: Opens a File

INPUTS: pathname : Complete path to the File to be opened
option	 : The Permission list that the file may be
opened with.
cp		 : The return path for non blocking fucntionality.

RETRUN : <0 in case of failures
File Descriptor in case of success.

 ********************************************************* */


static T_FFS_REQ_ID ffs_open_uc_nb(const T_WCHAR *pathname, T_FFS_OPEN_FLAGS option,T_RV_RETURN *cp)
{
   	T_Rel_req *Req; MSG_ALLOC(Req);
	memset(Req, 0, sizeof(T_Rel_req));
	Req->msg = REL_OPEN_NB_REQ;
	Req->option = option;
	Req->pathname = (const char*)pathname;
	Req->uMode = 0;
	Req->unicode = 1;
	Req->req_id = rel_get_free_pair_id ();
	Req->cp = cp;
	MSG_SEND(Req);
	return Req->req_id;
}


/* *********************************************************

NAME : ffs_close()

FUNCTIONALITY: closes a File

INPUTS: fd : The file descriptor given by ril_ffs_open_ffs2fs

RETRUN : <0 in case of failures
0  in case of success.

 ********************************************************* */
T_FFS_RET ffs_close(T_FFS_FD fd)
{
    int retval;
    int status = REL_OK;

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return ffs_map_ril_error(status);
    }
    retval = relFs_Close(fd);
    if (retval == REL_OK )
	return retval;
    else
	return ffs_map_ril_error(retval);
}



/* *********************************************************

NAME : ffs_close_nb()

FUNCTIONALITY: closes a File

INPUTS: fd : The file descriptor given by ril_ffs_open_ffs2fs
cp : The return path for non blocking fucntionality.

RETRUN : <0 in case of failures
>0 , the request id, in case of success.

 ********************************************************* */
T_FFS_REQ_ID ffs_close_nb(T_FFS_FD fd,T_RV_RETURN *cp)
{

	T_Rel_req *Req; MSG_ALLOC(Req);
	memset(Req, 0, sizeof(T_Rel_req));
	Req->msg = REL_CLOSE_NB_REQ;
	Req->fd  = fd;
	Req->cp = cp;
	Req->req_id = rel_get_free_pair_id ();
	MSG_SEND(Req);
	return Req->req_id;

}


/* *********************************************************

NAME : ffs_read()

FUNCTIONALITY: Read the data from file

INPUTS: fd   : The file descriptor given by ril_ffs_open_ffs2fs
pBuf  : The buffer to copy the data from file.
size : the number of bytes to read.

RETRUN : <0 in case of failures
>0, amount of data read, in case of success.

 ********************************************************* */
T_FFS_SIZE ffs_read(T_FFS_FD fd,void *pBuf,T_FFS_SIZE size)
{
    int retval;
    int status = REL_OK;

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return ffs_map_ril_error(status);
    }
    retval = relFs_Read(fd,pBuf,size);
    if(retval < 0)
	return ffs_map_ril_error(retval);
    else
	return retval;
}


/* *********************************************************

NAME : ffs_read_nb()

FUNCTIONALITY: Read the given data to file

INPUTS: fd   : The file descriptor given by ril_ffs_open_ffs2fs
pBuf  : The buffer containing the data to write to the file.
size : the number of bytes to write.
cp	 : The return path for non blocking fucntionality.

RETRUN : <0 in case of failures
>0, Request id, in case of success.

 ********************************************************* */

static T_FFS_REQ_ID ffs_read_nb(T_FFS_FD fd,void* pBuf, T_FFS_SIZE size,T_RV_RETURN *cp)
{

	T_Rel_req *Req; MSG_ALLOC(Req);
	memset(Req, 0, sizeof(T_Rel_req));
	Req->msg = REL_READ_NB_REQ;
	Req->pathname = NULL;
	Req->fd  = fd;
	Req->pBuf = pBuf;
	Req->cp = cp;
	Req->size = size;
	Req->req_id = rel_get_free_pair_id ();
	MSG_SEND(Req);
	return Req->req_id;
}



/* *********************************************************

NAME : ffs_write()

FUNCTIONALITY: Write the given data to file

INPUTS: fd   : The file descriptor given by ril_ffs_open_ffs2fs
pBuf  : The buffer containing the data to write to the file.
size : the number of bytes to write.

RETRUN : <0 in case of failures
>0, amount of data writtten, in case of success.

 ********************************************************* */
T_FFS_SIZE ffs_write(T_FFS_FD fd,void* pBuf, T_FFS_SIZE size)
{
    int retval;
    int status =REL_OK;

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return ffs_map_ril_error(status);
    }
    retval = relFs_Write(fd,pBuf,size);
    if(retval < 0)
	return ffs_map_ril_error(retval);
    else
	return retval;
}

/* *********************************************************

NAME : ffs_write_nb()

FUNCTIONALITY: Write the given data to file

INPUTS: fd   : The file descriptor given by ril_ffs_open_ffs2fs
pBuf  : The buffer containing the data to write to the file.
size : the number of bytes to write.
cp	 : The return path for non blocking fucntionality.

RETRUN : <0 in case of failures
>0, Request id, in case of success.

 ********************************************************* */

T_FFS_REQ_ID ffs_write_nb(T_FFS_FD fd,void* pBuf, T_FFS_SIZE size,T_RV_RETURN *cp)
{

	T_Rel_req *Req; MSG_ALLOC(Req);
	memset(Req, 0, sizeof(T_Rel_req));
	Req->msg = REL_WRITE_NB_REQ;
	Req->fd  = fd;
	Req->pathname = NULL;
	Req->pBuf = pBuf;
	Req->cp = cp;
	Req->size = size;
	Req->req_id = rel_get_free_pair_id ();
	MSG_SEND(Req);
	return Req->req_id;
}

/* *********************************************************

NAME : ffs_seek()

FUNCTIONALITY: seek the position of the file

INPUTS: fd      : The file descriptor given by ril_ffs_open_ffs2fs
offset  : The new offset to be set to the file.
origin  : FFS_SEEK_SET or FFS_SEEK_CUR or FFS_SEEK_END

RETRUN : <0 in case of failures
>0,the new position of the file,in case of success.

 ********************************************************* */
T_FFS_SIZE ffs_seek(T_FFS_FD fd,T_FFS_SIZE offset,T_FFS_SIZE origin)
{
    int retval;
    int status = REL_OK;
    REL_STAT pstat;
    signed long orgIndex, reqPos;

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return ffs_map_ril_error(status);
    }

    relFs_Stat(fd, &pstat);
    orgIndex = relFs_Seek(fd, 0, FFS_SEEK_CUR) ;
    reqPos = relFs_Seek(fd, 0, origin) ;

    // We have already completed requested operation
    if (offset == 0)
        {
		return (int)reqPos;
		}

    // Check if we are going beyond EOF
    relFs_Seek(fd, orgIndex, FFS_SEEK_SET) ;
    if ((reqPos+ offset) > pstat.ulSize)
    	{
	return EFFS_INVALID;
    	}

    retval = relFs_Seek(fd,offset,origin);
    if(retval < 0)
	return ffs_map_ril_error(retval);
    else
	return retval;
}

/* *********************************************************

NAME : ffs_seek_nb()

FUNCTIONALITY: seek the position of the file

CALLED BY: Any application.

INPUTS: fd      : The file descriptor given by ril_ffs_open_ffs2fs
offset  : The new offset to be set to the file.
origin  : FFS_SEEK_SET or FFS_SEEK_CUR or FFS_SEEK_END
cp		: The return path for non blocking fucntionality.

RETRUN : <0 in case of failures
>0,request id,in case of success.

 ********************************************************* */
T_FFS_REQ_ID ffs_seek_nb(T_FFS_FD fd,T_FFS_SIZE offset,T_FFS_SIZE origin,T_RV_RETURN *cp)
{
	T_Rel_req *Req; MSG_ALLOC(Req);
	memset(Req, 0, sizeof(T_Rel_req));
	Req->msg = REL_SEEK_NB_REQ;
	Req->fd  = fd;
	Req->pathname = NULL;
	Req->offset = offset;
	Req->origin = origin;
	Req->req_id = rel_get_free_pair_id ();
	Req->cp = cp;
	MSG_SEND(Req);
	return Req->req_id;
}

/* ******************************************************
NAME : ffs_mkdir()

FUNCTIONALITY: Create a directory

INPUTS: pathname	: The pathname of the directory.

RETRUN : <0 in case of failures
0 in case of success.

 ********************************************************* */
T_FFS_RET ffs_mkdir(const char *pathname)
{
    int retval;
    int status = REL_OK;

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return ffs_map_ril_error(status);
    }
    retval = relFs_Make_Dir(pathname);
    if(retval < 0)
	return ffs_map_ril_error(retval);
    else
	return retval;
}

/* ******************************************************
NAME : ffs_mkdir_nb()

FUNCTIONALITY: Create a directory

INPUTS: pathname	: The pathname of the directory.
cp			:  The return path either callback or communication id.

RETRUN : <0 in case of failures
>0,request id, in case of success.

 ********************************************************* */

T_FFS_REQ_ID ffs_mkdir_nb(const char *pathname,T_RV_RETURN *cp)
{

    T_Rel_req *Req; MSG_ALLOC(Req);
    memset(Req, 0, sizeof(T_Rel_req));
    Req->msg = REL_MKDIR_NB_REQ;
    Req->pathname = pathname;
    Req->cp = cp;
    Req->req_id = rel_get_free_pair_id ();
    MSG_SEND(Req);
    return Req->req_id;
}



/* ******************************************************
NAME : ffs_mkdir_uc_nb()

FUNCTIONALITY: Create a directory

INPUTS: pathname	: The pathname of the directory.
cp			:  The return path either callback or communication id.

RETRUN : <0 in case of failures
>0,request id, in case of success.

 ********************************************************* */

static T_FFS_REQ_ID ffs_mkdir_uc_nb(const T_WCHAR *pathname,T_RV_RETURN *cp)
{

    T_Rel_req *Req; MSG_ALLOC(Req);
    memset(Req, 0, sizeof(T_Rel_req));
    Req->msg = REL_MKDIR_NB_REQ;
    Req->pathname = (const char*)pathname;
	Req->unicode = 1;
    Req->cp = cp;
    Req->req_id = rel_get_free_pair_id ();
    MSG_SEND(Req);
    return Req->req_id;
}


/* Blocking truncate call */
/* *********************************************************
NAME : ffs_truncate()

FUNCTIONALITY: Truncates the file to new position.

INPUTS: path		: The pathname of the file.
length  	: new length of the file.

RETRUN : <0 in case of failures
>0,new file size,in case of success.

 ********************************************************* */
T_FFS_RET ffs_truncate(const char *path, T_FFS_OFFSET length)
{
    T_FFS_FD fd;
    int status = REL_OK;
    signed long current_offset;
    REL_STAT pstat;

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
		return ffs_map_ril_error(status);
    }
    if ((fd = relFs_Open(path,REL_O_RDWR,0))< 0)  /* Find the fd */
    {
		return EFFS_INVALID;
    }

    relFs_Stat(fd, &pstat);
    if (length > pstat.ulSize)
	return EFFS_OK;

    /* Call to reliance truncate function */
    status = relFs_Truncate(fd, length);
    if (relFs_Close(fd)< 0) /* Close the file */
    {
		return EFFS_INVALID;
    }
    if(status < 0)
		return ffs_map_ril_error(status);
    else
		return (T_FFS_RET)status;
}

/* Non_Blocking call to truncate */
/* *********************************************************

NAME : ffs_truncate_nb()

FUNCTIONALITY: Truncates the file to new position.

INPUTS: path	: The pathname of the file.
length  : new length of the file.
cp		: The return path for non blocking fucntionality.

RETRUN : <0 in case of failures
>0,request id,in case of success.

 ********************************************************* */

T_FFS_REQ_ID ffs_truncate_nb(const char *path, T_FFS_OFFSET length, T_RV_RETURN *cp)
{
    T_FFS_FD fd,retval;

    {
	PALLOC(req,Rel_req);
	memset(req, 0, sizeof(T_Rel_req));
	req->msg = REL_TRUNC_NB_REQ;
	req->pathname = path;
	req->length = length;
	req->cp = cp;
	req->req_id = rel_get_free_pair_id ();
	PSEND(ril_env_ctrl_blk_p->addr_id,req);
	return req->req_id;
    }

}



/* Non_Blocking call to truncate */
/* *********************************************************

NAME : ffs_truncate_uc_nb()

FUNCTIONALITY: Truncates the file to new position.

INPUTS: path	: The pathname of the file.
length  : new length of the file.
cp		: The return path for non blocking fucntionality.

RETRUN : <0 in case of failures
>0,request id,in case of success.

 ********************************************************* */

static T_FFS_REQ_ID ffs_truncate_uc_nb(const T_WCHAR *path, T_FFS_OFFSET length, T_RV_RETURN *cp)
{
    T_FFS_FD fd,retval;

    {
	PALLOC(req,Rel_req);
	memset(req, 0, sizeof(T_Rel_req));
	req->msg = REL_TRUNC_NB_REQ;
	req->pathname = (const char*)path;
	req->length = length;
	req->unicode = 1;
	req->cp = cp;
	req->req_id = rel_get_free_pair_id ();
	PSEND(ril_env_ctrl_blk_p->addr_id,req);
	return req->req_id;
    }

}


/* Blocking call to ftruncate */
/* *********************************************************

NAME : ffs_ftruncate()

FUNCTIONALITY: Truncates the file to new position.

INPUTS: fd	: the file descriptor given by ril_ffs_open_ffs2fs.
length  	: new length of the file.

RETRUN : <0 in case of failures
>0,new size,in case of success.

 ********************************************************* */
T_FFS_RET ffs_ftruncate(T_FFS_FD fd, T_FFS_OFFSET length)
{
    int status = REL_OK,retval;
    signed long current_offset;
    REL_STAT pstat;

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return (char)ffs_map_ril_error(status);
    }

    /* Do not truncate if the current file pointer is greater then the size
       of the file to be truncate*/
    current_offset = relFs_Seek(fd, 0, FFS_SEEK_CUR);
    if (current_offset > length )
	return EFFS_INVALID;

    relFs_Stat(fd, &pstat);
    if (length > pstat.ulSize)
	return EFFS_OK;


    /* Call to reliance truncate function */
    retval =  relFs_Truncate(fd, length);
    relFs_Seek(fd, current_offset, FFS_SEEK_SET);
    if(retval < 0)
    {
	return (char)ffs_map_ril_error(retval);
    }
    else
	return (char)retval;
}

/* Non Blocking call to ftruncate */
/* *********************************************************

NAME :ffs_ftruncate_nb()

FUNCTIONALITY: Truncates the file to new position.

INPUTS: fd	: the file descriptor given by ril_ffs_open_ffs2fs.
length  	: new length of the file.
cp	: The return path for non blocking fucntionality.

RETRUN : <0 in case of failures
>0,request id,in case of success.

 ********************************************************* */
T_FFS_REQ_ID ffs_ftruncate_nb(T_FFS_FD fd, T_FFS_OFFSET length, T_RV_RETURN *cp)
{
    PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_TRUNC_NB_REQ;
	req->pathname = NULL;
    req->fd = fd;
    req->length = length;
    req->cp = cp;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}


/* *********************************************************

NAME : ffs_fdatasync

FUNCTIONALITY: Flush the write buffer to the file
referenced by the specified file descriptor, fd.

INPUTS: fd	: the file descriptor given by ril_ffs_open_ffs2fs.

RETRUN : <0 in case of failures
0 in case of success.

 ********************************************************* */

T_FFS_RET ffs_fdatasync(T_FFS_FD fd)
{
    int status = REL_OK;
	int retval;

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return (char)ffs_map_ril_error(status);
    }
    retval =  relFs_Flush(fd);
    if(retval < 0)
	return (char)ffs_map_ril_error(retval);
    else
	return (char)retval;
}

/* Non Blocking call to flush. Flush the write buffer to the file
   referenced by the specified file descriptor, fd. */
/* *********************************************************

NAME : ffs_fdatasync_nb

FUNCTIONALITY: Flush the write buffer to the file
referenced by the specified file descriptor, fd.

INPUTS: fd	: the file descriptor given by ril_ffs_open_ffs2fs.
cp	: The return path for non blocking fucntionality.

RETRUN : <0 in case of failures
>0,request id,in case of success.

 ********************************************************* */
T_FFS_REQ_ID ffs_fdatasync_nb(T_FFS_FD fd, T_RV_RETURN *cp)
{
    PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_FLUSH_NB_REQ;
	req->pathname = NULL;
    req->fd = fd;
    req->cp = cp;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}

/* Blocking call to statistics */
/* ******************************************************
NAME : ffs_stat()

FUNCTIONALITY: Get the statistics of the file/directory/partition.

INPUTS: path	: The pathname of the file.
stat  	: structure to get the statistic of the file.

RETRUN : <0 in case of failures
0 in case of success.

 ********************************************************* */
T_FFS_RET ffs_stat(const char *path, T_FFS_STAT *stat)
{
    int retval,status = REL_OK;
    T_FFS_FD fd;
    REL_STAT        sRelStat;

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return (char)ffs_map_ril_error(status);
    }
    fd = relFs_Open(path, REL_O_RDONLY, 0x00);
    if(fd < 0)
    {
	return (char)ffs_map_ril_error(fd);
    }

    retval = relFs_Stat(fd, &sRelStat);

    if (sRelStat.uAttributes & REL_ATTR_DIRECTORY) /* Just stating file or dir */
	stat->type = OT_DIR;
    else
	stat->type = OT_FILE;
    stat->size =sRelStat.ulSize;
    stat->flags = FFS_O_RDWR;	/* Default to each call*/

    relFs_Close(fd);

    if(retval < 0)
	return (char)ffs_map_ril_error(retval);
    else
	return (char)retval;
}



/* Non Blocking call to statistics */
/* ******************************************************
NAME : ffs_stat_nb()

FUNCTIONALITY: Get the statistics of the file/directory/partition.

INPUTS: path	: The pathname of the file.
stat  	: structure to get the statistic of the file.

RETRUN : <0 in case of failures
>0 request id .

 ********************************************************* */
T_FFS_REQ_ID ffs_stat_nb(const char *path, T_FFS_STAT *stat,T_RV_RETURN *cp)
{
 	PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_STAT_NB_REQ;
   	req->pathname = path;
    req->cp = cp;
	req->ptr_data = (void*)(stat);
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}




/* Non Blocking call to statistics */
/* ******************************************************
NAME : ffs_stat_rfs_uc_nb()

FUNCTIONALITY: Get the statistics of the file/directory/partition.

INPUTS: path	: The pathname of the file.
stat  	: structure to get the statistic of the file.

RETRUN : <0 in case of failures
>0 request id .

 ********************************************************* */
static T_FFS_REQ_ID ffs_stat_rfs_uc_nb(const T_WCHAR* path, T_FSCORE_STAT * stat,T_RV_RETURN *cp)
{
 	PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_STAT_RFS_NB_REQ;
   	req->pathname = (const char*)path;
    req->cp = cp;
	req->unicode = 1;
	req->ptr_data = (void*)(stat);
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}



static T_FFS_REQ_ID ffs_get_attribute_rfs_uc_nb(const T_WCHAR* path,T_RV_RETURN *cp)
{
 	PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_GET_ATTRIB_RFS_REQ;
   	req->pathname = (const char*)path;
    req->cp = cp;
	req->unicode = 1;
	req->ptr_data = NULL;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}


static T_FFS_REQ_ID ffs_set_attribute_rfs_uc_nb(const T_WCHAR* path,T_FFS_ATTRIB_TYPE attrib, T_RV_RETURN *cp)
{
 	PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_SET_ATTRIB_RFS_REQ;
   	req->pathname = (const char*)path;
    req->cp = cp;
	req->unicode = 1;
    req->uMode = attrib;
	req->ptr_data = NULL;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}






/* ******************************************************
NAME : ffs_lstat()

FUNCTIONALITY: Get the statistics of the file/directory/partition.

CALLED BY: Any application.

INPUTS: pathname	: The pathname of the file.
		stat  		: structure to get the statistic of the file.


OUTPUTS: None

RETRUN : <0 in case of failures
          0 in case of success.

********************************************************* */

T_FFS_RET ffs_lstat (const char *pathname, T_FFS_STAT *stat )
{
    return ffs_stat(pathname, stat);
}

/* ******************************************************
NAME : ffs_xlstat()

FUNCTIONALITY: Get the statistics of the file/directory/partition.

INPUTS: name	: The pathname of the file.
stat  	: structure to get the statistic of the file.

RETRUN : <0 in case of failures
0 in case of success.

 ********************************************************* */
T_FFS_RET ffs_xlstat(const char *name, T_FFS_XSTAT *stat)
{
    int32 retval,status = REL_OK;
    T_FFS_FD fd;
    REL_STAT        sRelStat;

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return (char)ffs_map_ril_error(status);
    }

    fd = relFs_Open(name, REL_O_RDONLY,0x00);
    if(fd < 0)
    {
	return (char)ffs_map_ril_error(fd);
    }

    retval = relFs_Stat(fd, &sRelStat);

    if (sRelStat.uAttributes & REL_ATTR_DIRECTORY) /* Just stating file or dir */
	stat->type = OT_DIR;
    else
	stat->type = OT_FILE;
    stat->size =sRelStat.ulSize;
    stat->flags = FFS_O_RDWR;	/* Default to each call*/

    /* Closing as No-Body is going to close this FD */
    relFs_Close(fd);

    if(retval < 0)
		return (char)ffs_map_ril_error(retval);
    else
		return (char)retval;
}

/* ******************************************************
NAME : ffs_fchmod_rfs_nb()

FUNCTIONALITY: change mode of the file.

INPUTS: fd  : the file descriptor given by ril_ffs_open_ffs2fs.


RETRUN : 0 (always success) because chmod functionality is not supported in nucleus

 ********************************************************* */
static T_FFS_REQ_ID ffs_fchmod_rfs_nb ( T_FFS_FD fd,T_RV_RETURN *cp)
{

	PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_FCHMOD_RFS_NB_REQ;
    req->pathname = NULL;
    req->fd = fd;
    req->cp = cp;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;	
}

/* ******************************************************
NAME : ffs_chmod_rfs_uc_nb()

FUNCTIONALITY: change the access mode of the file.

INPUTS: path    : The pathname of the file.

RETRUN : 0 (always success) because chmod functionality is not supported in nucleus

 ********************************************************* */
static T_FFS_REQ_ID ffs_chmod_rfs_uc_nb(const T_WCHAR* path,T_RV_RETURN *cp)
{
    
	PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_CHMOD_RFS_NB_REQ;
    req->pathname = (const char*)path;
    req->cp = cp;
    req->unicode = 1;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}




/* ******************************************************
NAME : ffs_fstat()

FUNCTIONALITY: Get the statistics of the file.

INPUTS: fd	: the file descriptor given by ril_ffs_open_ffs2fs.
stat: structure to get the statistic of the file.

RETRUN : <0 in case of failures
0 in case of success.

 ********************************************************* */
T_FFS_RET ffs_fstat ( T_FFS_FD fd, T_FFS_STAT *stat)
{
    int retval;
	ERR_MSG status = REL_OK;
    REL_STAT pstat;

    if(relFs_Check_File_User())
    {
    	status = (ERR_MSG)relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return (char)ffs_map_ril_error(status);
    }

    retval =  relFs_Stat(fd,&pstat);

    if (pstat.uAttributes & REL_ATTR_DIRECTORY) /* Just stating file or dir */
	stat->type = OT_DIR;
    else
	stat->type = OT_FILE;
    stat->size = pstat.ulSize;
    stat->flags = FFS_O_RDWR;	/* Default to each call*/

    if(retval < 0)
	return (char)ffs_map_ril_error(retval);
    else
	return (char)retval;

}


/* ******************************************************
NAME : ffs_fstat_nb()

FUNCTIONALITY: Get the statistics of the file.

INPUTS: fd	: the file descriptor given by ril_ffs_open_ffs2fs.
stat: structure to get the statistic of the file.

RETRUN : <0 in case of failures
>0 in case of success.

 ********************************************************* */
T_FFS_REQ_ID ffs_fstat_nb ( T_FFS_FD fd, T_FFS_STAT *stat,T_RV_RETURN *cp)
{
 	PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_FSTAT_NB_REQ;
	req->pathname = NULL;
   	req->fd = fd;
    req->cp = cp;
	req->ptr_data = (void*)(stat);
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}



/* ******************************************************
NAME : ffs_fstat_rfs_nb()

FUNCTIONALITY: Get the statistics of the file.

INPUTS: fd	: the file descriptor given by ril_ffs_open_ffs2fs.
stat: structure to get the statistic of the file.

RETRUN : <0 in case of failures
>0 in case of success.

 ********************************************************* */
static T_FFS_REQ_ID ffs_fstat_rfs_nb ( T_FFS_FD fd, T_FSCORE_STAT * stat,T_RV_RETURN *cp)
{
 	PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_FSTAT_RFS_NB_REQ;
	req->pathname = NULL;
   	req->fd = fd;
    req->cp = cp;
	req->ptr_data = (void*)(stat);
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}





/* Blocking call to link creation */
/* ******************************************************
NAME : ffs_symlink()

FUNCTIONALITY: Creates a synbolic link to file/directory. Works only on JFFS

INPUTS:
actualpath : The pathname of file/directory.
pathname   : The pathname of the synbolic link to be created.

RETRUN : <0 in case of failures
0 in case of success.

 ********************************************************* */
T_FFS_RET ffs_symlink(const char *actualpath, const char *pathname)
{
    int32 retval,status = REL_OK;

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
	return ffs_map_ril_error(status);
    retval = relFs_Link(actualpath,pathname);
    if(retval < 0)
	return ffs_map_ril_error(retval);
    else
	return retval;
}

/*-------------------------------------------------------------------
FUNCTION :ffs_symlink_nb

FUNCTIONALITY :
Create a new named link to a file.

INPUTS:
actualpath : The pathname of file/directory.
pathname   : The pathname of the synbolic link to be created.
cp		   : return path either callback function or GSP communcation id.

RETRUN : <0 in case of failures
>0, request id, in case of success.
-------------------------------------------------------------------*/
T_FFS_REQ_ID ffs_symlink_nb(const char *actualpath, const char *pathname,
	T_RV_RETURN *cp)
{
    PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_LINK_NB_REQ;
    req->actualpath = actualpath;
    req->pathname = pathname;
    req->cp = cp;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}
T_FFS_RET ffs_preformat(uint16 magic)
{

	if(magic == 0xDEAD)
		return EFFS_OK;
	else
		return EFFS_INVALID;
}

T_FFS_REQ_ID ffs_preformat_nb(uint16 magic, T_RV_RETURN *cp)
{

	return EFFS_OK;
}


/*------------------------------------------------------------------
FUNCTION NAME:  ffs_format

FUNCTIONALITY :

INPUTS:

RETURN:
-------------------------------------------------------------------*/
T_FFS_RET ffs_format(const char *name, uint16 magic)
{
    int status = REL_OK;
    int	retval,val;
    unsigned long BlockSize = 512;

    if (magic != 0x2BAD) {
	return (T_FFS_RET )EFFS_INVALID;
    }

    if (name == NULL) {
	name = "/";
    }

    if (*name != '/') {
	return (T_FFS_RET )EFFS_BADNAME;
    }

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
	return (char)ffs_map_ril_error(status);
    retval = relFs_Format(0, BlockSize);/*drive number is hard coded */
    if(retval < 0)
    {
	val = ffs_map_ril_error(status);
	if( val == EFFS_NOTFOUND)
	    return EFFS_BADNAME;
    }
	return (char)retval;


}

/*------------------------------------------------------------------
FUNCTION : ffs_format_nb

FUNCTIONALITY :

INPUTS:

RETURN:
-------------------------------------------------------------------*/
T_FFS_REQ_ID ffs_format_nb(const char *name, uint16 magic, T_RV_RETURN *cp)
{

	PALLOC(req,Rel_req);
	memset(req, 0, sizeof(T_Rel_req));
	req->msg = REL_FORMAT_NB_REQ;
	req->pathname = name;
	req->cp = cp;
	req->magic = magic;
	req->req_id = rel_get_free_pair_id ();
	PSEND(ril_env_ctrl_blk_p->addr_id,req);
	return req->req_id;

}


/*------------------------------------------------------------------
FUNCTION : ffs_format_uc_nb

FUNCTIONALITY :

INPUTS:

RETURN:
-------------------------------------------------------------------*/
static T_FFS_REQ_ID ffs_format_uc_nb(const T_WCHAR *name, uint16 magic, T_RV_RETURN *cp)
{

	PALLOC(req,Rel_req);
	memset(req, 0, sizeof(T_Rel_req));
	req->msg = REL_FORMAT_NB_REQ;
	req->pathname = (const char*)name;
	req->cp = cp;
	req->magic = magic;
	req->unicode = 1;
	req->req_id = rel_get_free_pair_id ();
	PSEND(ril_env_ctrl_blk_p->addr_id,req);
	return req->req_id;

}





/*------------------------------------------------------------------
FUNCTION : ffs_preformat_uc_nb

FUNCTIONALITY :

INPUTS:

RETURN:
-------------------------------------------------------------------*/
static T_FFS_REQ_ID ffs_preformat_uc_nb(const T_WCHAR *pathname, uint16 magic, T_RV_RETURN *cp)
{

	PALLOC(req,Rel_req);
	memset(req, 0, sizeof(T_Rel_req));
	req->msg = REL_PREFORMAT_NB_REQ;
	req->pathname = (const char*)pathname;
	req->cp = cp;
	req->magic = magic;
	req->unicode = 1;
	req->req_id = rel_get_free_pair_id ();
	PSEND(ril_env_ctrl_blk_p->addr_id,req);
	return req->req_id;

}



/* ******************************************************
NAME : ffs_remove()

FUNCTIONALITY: Remove the file or directory

INPUTS:
pathname	: The pathname of the file.

RETRUN : <0 in case of failures
0 in case of success.

 ********************************************************* */

T_FFS_RET ffs_remove(const char *pathname)
{
    int status = REL_OK;
    int retval;
    int iStatus;
    unsigned short  uAttr;

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
	return (char)ffs_map_ril_error(status);


    /*  Determine if this is a file or directory.
    */
    iStatus = relFs_Get_Attributes(pathname, &uAttr);
    if(iStatus != REL_OK)
    {
	return (char)ffs_map_ril_error(iStatus);
    }

    if(uAttr & REL_ATTR_DIRECTORY)
    {
	retval = relFs_Remove_Dir(pathname);
    }
    else
    {
	retval = relFs_Delete(pathname);
    }
    if(retval == -1001)
	retval = -1023;
    if(retval < 0)
	return (char)ffs_map_ril_error(retval);
    else
	return (char)retval;
}

/* ******************************************************
NAME : ffs_remove_nb()

FUNCTIONALITY: Remove the file or directory

INPUTS:
pathname	: The pathname of the file.
cp			: Return path

RETRUN : <0 in case of failures
0 in case of success.

 ********************************************************* */
T_FFS_REQ_ID ffs_remove_nb(const char *pathname, T_RV_RETURN *cp)
{
    PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_REMOVE_NB_REQ;
    req->pathname = pathname;
    req->cp = cp;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}


/* ******************************************************
NAME : ffs_remove_uc_nb()

FUNCTIONALITY: Remove the file or directory

INPUTS:
pathname	: The pathname of the file.
cp			: Return path

RETRUN : <0 in case of failures
0 in case of success.

 ********************************************************* */
static T_FFS_REQ_ID ffs_remove_uc_nb(const T_WCHAR *pathname, T_RV_RETURN *cp)
{
    PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_REMOVE_NB_REQ;
    req->pathname = (const char*)pathname;
    req->cp = cp;
	req->unicode =1;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}





/* ******************************************************
NAME : ffs_rmdir_uc_nb()

FUNCTIONALITY: Remove the file or directory

INPUTS:
pathname	: The pathname of the file.
cp			: Return path

RETRUN : <0 in case of failures
0 in case of success.

 ********************************************************* */
static T_FFS_REQ_ID ffs_rmdir_uc_nb(const T_WCHAR *pathname, T_RV_RETURN *cp)
{
    PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_RMDIR_NB_REQ;
    req->pathname = (const char*)pathname;
    req->cp = cp;
	req->unicode =1;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}



/* ******************************************************

NAME  :  ffs_opendir()

FUNCTIONALITY: Open a directory

INPUTS: name		: The pathname of the directory.
dir			: The dir structure to be filled by this function.

RETRUN : <0 in case of failures
>Number of entries in the directory in case of success.

 ********************************************************* */
T_FFS_SIZE ffs_opendir(const char *name, T_FFS_DIR *dir)
{
    int status = REL_OK;
    int retval;
    void *ddir;
    REL_DIR *RelDir;


    if ((name==NULL) || (dir== NULL))
    {
       return EFFS_INVALID;
    }

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
	return ffs_map_ril_error(status);

    if(rvf_get_buf(EXT_MEM_POOL, sizeof(REL_DIR), (T_RVF_BUFFER*) &RelDir) == RVF_RED)
    {
	return EFFS_MEMORY;
    }

    retval = relFs_Open_Dir(name, RelDir);
    if(retval != REL_OK)
    {
       dir->this = 0;
       dir->index = 0;
	rvf_free_buf((T_RVF_BUFFER*)RelDir);
	return ffs_map_ril_error(retval);
    }

    dir->this = (int32)(0xFFFF&(int32)RelDir);
    dir->index = (int32)(0xFFFF&((int32)RelDir>>16));

   /* Read the number of entries in directory
   */
    retval = relFs_Entries_Dir(RelDir);
    if(retval < 0)
    {
           dir->this = 0;
           dir->index = 0;
	    rvf_free_buf((T_RVF_BUFFER*)RelDir);
	    return ffs_map_ril_error(retval);
    }

    return  retval;
}



/* ******************************************************

NAME  :  ffs_opendir_nb()

FUNCTIONALITY: Open a directory

INPUTS: name		: The pathname of the directory.
dir			: The dir structure to be filled by this function.

RETRUN : <0 in case of failures
>Request id.

 ********************************************************* */
T_FFS_REQ_ID ffs_opendir_nb(const char *name, T_FFS_DIR *dir, T_RV_RETURN *cp)
{
   PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_OPENDIR_NB_REQ;
    req->pathname = name;
	req->ptr_data = (void*)dir;
    req->cp = cp;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}



/* ******************************************************

NAME  :  ffs_opendir_rfs_uc_nb()

FUNCTIONALITY: Open a directory

INPUTS: name		: The pathname of the directory.
dir			: The dir structure to be filled by this function.

RETRUN : <0 in case of failures
>Request id.

 ********************************************************* */
static T_FFS_REQ_ID ffs_opendir_rfs_uc_nb(const T_WCHAR *name, T_FSCORE_DIR *dir, T_RV_RETURN *cp)
{
   PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_OPENDIR_RFS_NB_REQ;
    req->pathname = (const char*)name;
	req->ptr_data = (void*)dir;
    req->cp = cp;
	req->unicode = 1;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}




/* ******************************************************
NAME : ffs_readdir()

FUNCTIONALITY: Open a directory

INPUTS: dir			: The dir structure to be filled by ffs_opendir function.
buf			: The buffer to be filled by the contents of the folder.
nbytes		: The number of bytes to be written to the buf
 ******************************************************************  */

T_FFS_REQ_ID ffs_readdir(T_FFS_DIR *dir, char *buf, T_FFS_SIZE size)
{
    int status = REL_OK;
    int retval;
    REL_DIR *pReldir;
    unsigned int tempPtr;
    unsigned short lsw, msw;

    if ((buf==NULL) || (dir== NULL)|| (size<=0))
    {
       return EFFS_INVALID;
    }

    lsw   = (unsigned short)(dir->this);
    msw = (unsigned short)(dir->index);

    tempPtr = ((unsigned int)lsw & 0x0000FFFF) | (((unsigned int)msw) << 16);

    pReldir = (REL_DIR*)(tempPtr);
    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }

    if(status != REL_OK)
	return ffs_map_ril_error(status);

    retval =  relFs_Read_Dir(pReldir);

    if(retval == -1014)
    {
       dir->this = 0;
       dir->index = 0;
	/* Close the Directory Handle */
	relFs_Close_Dir(pReldir);
	rvf_free_buf((T_RVF_BUFFER*)pReldir);
	/* Full Directory read */
	return 0;
    }
    if(retval != REL_OK)
    {
	return ffs_map_ril_error(retval);
    }
    return snprintf(buf,size,"%s",pReldir->szName);
}


/* ******************************************************
NAME : ffs_readdir_nb()

FUNCTIONALITY: Open a directory

INPUTS: dir			: The dir structure to be filled by ffs_opendir function.
buf			: The buffer to be filled by the contents of the folder.
nbytes		: The number of bytes to be written to the buf
 ******************************************************************  */

T_FFS_REQ_ID ffs_readdir_nb(T_FFS_DIR *dir, char *buf, T_FFS_SIZE size,T_RV_RETURN *cp)
{
   PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_READDIR_NB_REQ;
    req->ptr_data = (void*)(dir);
	req->pBuf = buf;
	req->size = size;
	req->pathname = NULL;	
	req->fd = 0;
    req->cp = cp;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}



/* ******************************************************
NAME : ffs_readdir_rfs_uc_nb()

FUNCTIONALITY: Open a directory

INPUTS: dir			: The dir structure to be filled by ffs_opendir function.
buf			: The buffer to be filled by the contents of the folder.
nbytes		: The number of bytes to be written to the buf
 ******************************************************************  */

static T_FFS_REQ_ID ffs_readdir_rfs_uc_nb(T_FSCORE_DIR *dir, T_WCHAR *buf, T_FFS_SIZE size,T_RV_RETURN *cp)
{
   PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_READDIR_RFS_NB_REQ;
    req->ptr_data = (void*)(dir);
	req->pBuf = (char*)buf;
	req->size = size;
	req->pathname = NULL;
	req->fd = 0;
    req->cp = cp;
	req->unicode = 1;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}




/* ******************************************************
NAME : ffs_closedir()

FUNCTIONALITY: Closes a opened directory.

INPUTS: dir_p		: The dir structure


OUTPUTS: None

RETRUN : <0 in case of failures
0 in case of success.

 ********************************************************* */
T_FFS_RET ffs_closedir(T_FFS_DIR * dir_p)
{
    int retval;
	int status = REL_OK;
    REL_DIR *pdir;
    unsigned int tempPtr;
    unsigned short lsw, msw;

    if (dir_p== NULL)
    {
       return EFFS_INVALID;
    }

    lsw   = (unsigned short)(dir_p->this);
    msw = (unsigned short)(dir_p->index);

    tempPtr = ((unsigned int)lsw & 0x0000FFFF) | (((unsigned int)msw) << 16);

    pdir = (REL_DIR*)(tempPtr);

    if (pdir== NULL)
    {
       return EFFS_INVALID;
    }

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
	return ffs_map_ril_error(status);

    retval = relFs_Close_Dir(pdir);
    if(retval != REL_OK)
    {
	return ffs_map_ril_error(retval);
    }

    dir_p->this = 0;
    dir_p->index = 0;

    rvf_free_buf((T_RVF_BUFFER*)pdir);
    return retval;
}


/* ******************************************************
NAME : ffs_closedir_nb()

FUNCTIONALITY: Closes a opened directory.

INPUTS: dir_p		: The dir structure


OUTPUTS: None

RETRUN : <0 in case of failures
>0 in case of success.

 ********************************************************* */
T_FFS_REQ_ID ffs_closedir_nb(T_FFS_DIR * dir_p,T_RV_RETURN *cp)
{
   PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_CLOSEDIR_NB_REQ;
    req->ptr_data = (void*)(dir_p);
	req->fd = 0;
    req->cp = cp;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}



/* ******************************************************
NAME : ffs_closedir_rfs_nb()

FUNCTIONALITY: Closes a opened directory.

INPUTS: dir_p		: The dir structure


OUTPUTS: None

RETRUN : <0 in case of failures
>0 in case of success.

 ********************************************************* */
static T_FFS_REQ_ID ffs_closedir_rfs_nb(T_FSCORE_DIR * dir_p,T_RV_RETURN *cp)
{
   PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_CLOSEDIR_RFS_NB_REQ;
    req->ptr_data = (void*)(dir_p);
	req->pathname = NULL;
	req->fd = 0;
    req->cp = cp;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}




/* ******************************************************
NAME : ffs_readlink()

FUNCTIONALITY: reads the contents of file pointed by symbolic link. Works only on JFFS

INPUTS:
name   : The pathname of the synbolic link.
pBuf   : the output buffer where data should be copied.
size   : Number of bytes.


OUTPUTS: None

RETRUN : <0 in case of failures
>0, bytes read, in case of success.

 ********************************************************* */
T_FFS_SIZE ffs_readlink(const char *name, char *pBuf, T_FFS_SIZE size)
{

    int retval;
    int status = REL_OK;
    T_FFS_FD fd;

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return ffs_map_ril_error(status);
    }
    /* Call reliancne Open function to get file discriptor fd*/
    fd = relFs_Open(name, REL_O_RDONLY,REL_IWRITE | REL_IREAD);
    if (fd < 0 )
	return ffs_map_ril_error(fd);

    /* Call reliance read function */
    retval = relFs_Read(fd,pBuf,size);

    if ((status = relFs_Close(fd)) < 0)
	return ffs_map_ril_error(status);

    if(retval < 0)
	return ffs_map_ril_error(retval);
    else
	return retval;
}

/* ******************************************************
NAME : ffs_file_read()

FUNCTIONALITY: reads the contents of given file.

INPUTS:
pathname   : The pathname of the file.
buf		   : the output buffer where data should be copied.
nbytes	   : Number of bytes.


OUTPUTS: None

RETRUN : <0 in case of failures
>0, bytes read, in case of success.

 ********************************************************* */
T_FFS_SIZE ffs_file_read(const char *name, void *buf, T_FFS_SIZE size)
{
    int retval;
    int status = REL_OK;
    T_FFS_FD fd;

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return ffs_map_ril_error(status);
    }
    /* Call reliancne Open function to get file discriptor fd*/
    fd = relFs_Open(name, REL_O_RDONLY, REL_IWRITE | REL_IREAD);
    if (fd < 0 )
	return ffs_map_ril_error(fd);

    /* Call reliance Read function */
    retval = relFs_Read(fd,buf,size);

    if ((status = relFs_Close(fd)) < 0)
	return ffs_map_ril_error(status);

    if(retval < 0)
	return ffs_map_ril_error(retval);
    else
	return retval;
}
/* ******************************************************
NAME : ffs_fread()

FUNCTIONALITY: reads the contents of given file.

INPUTS:
pathname   : The pathname of the file.
buf		   : the output buffer where data should be copied.
nbytes	   : Number of bytes.


OUTPUTS: None

RETRUN : <0 in case of failures
>0, bytes read, in case of success.

 ********************************************************* */
T_FFS_SIZE ffs_fread (const char *pathname, void *buf, int32 nbytes )
{
    return ffs_file_read(pathname,buf, nbytes);
}

/* ******************************************************
NAME : ffs_file_write()

FUNCTIONALITY: writes the data to given file.

INPUTS:
name   	: The pathname of the file.
buf	   	: the Input buffer from where data should be copied.
size   	: Number of bytes to write.
option	: The Permission list

RETRUN : <0 in case of failures
>0, bytes written, in case of success.

 ********************************************************* */
T_FFS_RET ffs_file_write(const char *name, void *buf, T_FFS_SIZE size,
	T_FFS_OPEN_FLAGS option)
{
    int retval;
    int status=REL_OK;
    T_FFS_FD fd;
    unsigned short flags;
    unsigned short uMode;

    if(relFs_Check_File_User())
    {
	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return ffs_map_ril_error(status);
    }

    /* Reliance is not handling FFS_O_RDONLY | FFS_O_APPEND.So test it here */
    if((option & FFS_O_RDONLY ) && (option & FFS_O_APPEND))
    {
	if(!(option & FFS_O_WRONLY))
	{
	    return EFFS_INVALID;
	}
    }
    if((option & FFS_O_APPEND) && (option & FFS_O_TRUNC))
	option ^=  FFS_O_APPEND;

    /* Map the ffs2fs file flag to Reliance file flags */
    flags = ffs_map_ril_flag(option);

    /*Find file mode. This is valid for Reliance FS only */
    if (flags & REL_O_CREAT)
    {
	if(option & (REL_O_WRONLY | REL_O_RDWR))
	    uMode = REL_IWRITE | REL_IREAD;
	else
	    uMode = REL_IREAD;
    }
    else
	uMode = 0x0000;



    fd = relFs_Open(name, flags, uMode);
    if (fd < REL_OK)
	return (char)ffs_map_ril_error(fd);

    /*call reliance file write function */
    retval = relFs_Write(fd,buf,size);

    if ((status = relFs_Close(fd)) < 0)
	return (char)ffs_map_ril_error(status);

    if(retval < 0)
	return (char)ffs_map_ril_error(retval);
    else
    	{
    		retval=0;
	      return (char)retval;
      }
}
/* ******************************************************
NAME : ffs_fwrite()

FUNCTIONALITY: writes the data to given file.

CALLED BY: Any application.

INPUTS:
		pathname   : The pathname of the file.
		buf		   : the Input buffer from where data should be copied.
		nbytes	   : Number of bytes to write.


OUTPUTS: None

RETRUN : <0 in case of failures
         >0, bytes written, in case of success.

********************************************************* */
T_FFS_RET ffs_fwrite (const char *pathname, void *buf, T_FFS_SIZE size)
{
   return ffs_file_write(pathname,buf,size,FFS_O_CREATE|FFS_O_WRONLY|FFS_O_TRUNC);
}

/* ******************************************************
NAME : ffs_file_write_nb()

FUNCTIONALITY: writes the data to given file.

INPUTS:
pathname   : The pathname of the file.
buf		   : the Input buffer from where data should be copied.
size	   : Number of bytes to write.
cp		   : return path either callback function or GSP communcation id.

RETRUN : <0 in case of failures
>0, request id, in case of success.

 ********************************************************* */
T_FFS_REQ_ID ffs_file_write_nb(const char *pathname, void *buf, T_FFS_SIZE size,
	T_FFS_OPEN_FLAGS option,T_RV_RETURN *cp)
{
    T_Rel_req *Req; MSG_ALLOC(Req);
	memset(Req, 0, sizeof(T_Rel_req));
	Req->msg = REL_WRITE_NB_REQ;
	Req->pathname = pathname;
	Req->pBuf = buf;
	Req->cp = cp;
	Req->option = option;
	Req->size = size;
	Req->req_id = rel_get_free_pair_id ();
	MSG_SEND(Req);
	return Req->req_id;


}
/* ******************************************************
NAME : ffs_fwrite_nb()

FUNCTIONALITY: writes the data to given file.

CALLED BY: Any application.

INPUTS:
		pathname   : The pathname of the file.
		buf		   : the Input buffer from where data should be copied.
		nbytes	   : Number of bytes to write.
		return_path : return path either callback function or GSP communcation id.

OUTPUTS: None

RETRUN : <0 in case of failures
         >0, request id, in case of success.

********************************************************* */
T_FFS_REQ_ID ffs_fwrite_nb( const char * pathname, void * buf, int32 nbytes,T_RV_RETURN *return_path)
{
	return ffs_file_write_nb(pathname,buf,nbytes,FFS_O_WRONLY|FFS_O_TRUNC,return_path);
}
/* ******************************************************
NAME : ffs_update()

FUNCTIONALITY: Appends the data to given file.

INPUTS:
pathname   : The pathname of the file.
buf		   : the Input buffer from where data should be copied.
size	   : Number of bytes to write.

RETRUN : <0 in case of failures
>0, bytes written, in case of success.

 ********************************************************* */
T_FFS_RET ffs_fupdate(const char *pathname, void *buf, T_FFS_SIZE size)
{
    return ffs_file_write(pathname, buf,size,FFS_O_APPEND|FFS_O_WRONLY);
}


/* ******************************************************
NAME : ffs_fupdate_nb()

FUNCTIONALITY: Appends the data to given file.

INPUTS:
pathname   : The pathname of the file.
buf		   : the Input buffer from where data should be copied.
size	   : Number of bytes to write.
cp		   : return path either callback function or GSP communcation id.

RETRUN : <0 in case of failures
>0,request id, in case of success.

 ********************************************************* */
T_FFS_REQ_ID ffs_fupdate_nb(const char *pathname, void *buf, T_FFS_SIZE size,
	T_RV_RETURN *cp)
{
    return ffs_file_write_nb(pathname, buf,size,FFS_O_APPEND|FFS_O_WRONLY,cp);
}

/* ******************************************************
NAME : ffs_fcreate()

FUNCTIONALITY: Create& Write the data to given file.

INPUTS:
pathname   : The pathname of the file.
buf		   : the Input buffer from where data should be copied.
size	   : Number of bytes to write.

RETRUN : <0 in case of failures
>0, bytes written, in case of success.

 ********************************************************* */
T_FFS_RET ffs_fcreate(const char *pathname, void *buf, T_FFS_SIZE size)
{
	return ffs_file_write(pathname, buf,size,FFS_O_CREATE|FFS_O_TRUNC|FFS_O_WRONLY);
}

/* ******************************************************
NAME : ffs_fcreate_nb()

FUNCTIONALITY: Create& Write the data to given file.

INPUTS:
pathname   : The pathname of the file.
buf		   : the Input buffer from where data should be copied.
size	   : Number of bytes to write.
cp		   : return path either callback function or GSP communcation id.

RETRUN : <0 in case of failures
>0, request id, in case of success.

 ********************************************************* */
T_FFS_REQ_ID ffs_fcreate_nb(const char *pathname, void *buf, T_FFS_SIZE size,
	T_RV_RETURN *cp)
{
	 return ffs_file_write_nb(pathname, buf,size,FFS_O_CREATE|FFS_O_TRUNC|FFS_O_WRONLY,cp);
}

/* ******************************************************
NAME : ffs_rename()

FUNCTIONALITY: Renames the file or directory in name to newname. Fails if name is invalid,
newname already exists or path not found.New name may contain a path
part and must contain a name part.  If no path is supplied, the current
working directory is used.
INPUTS:
oldname   : The original pathname of the file.
newname	  : the newname to the file

RETRUN : <0 in case of failures
>0,

 ********************************************************* */

T_FFS_RET ffs_rename(const char * oldname, const char * newname)
{
    int status = REL_OK;
    int retval;
    int iStatus;
    unsigned short  uAttr;
    T_FFS_FD fd; 
    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return (char)ffs_map_ril_error(status);
    }

     /*  Determine if this is a file or directory.
    */
    iStatus = relFs_Get_Attributes((const char *)oldname, &uAttr);
    if(iStatus != REL_OK)
    {
	return (char)ffs_map_ril_error(iStatus);
    }

    if(uAttr & REL_ATTR_DIRECTORY)
    {
	retval = relFs_Rename((const char *)oldname,(const char *)newname);
    }
    else
    {
       /* checking whether the particular file is opened or not */	
    fd = relFs_Open((const char *)oldname, REL_O_WRONLY | REL_O_NOSHAREANY, REL_IWRITE);
    if (fd < REL_SUCCESS)
	return EFFS_LOCKED;
    else
	relFs_Close(fd);
    retval = relFs_Rename((const char *)oldname,(const char *)newname);    
    }	
    if(retval < 0)
	return ffs_map_ril_error(retval);
    else
	return retval;
}

/* ******************************************************
NAME : ffs_rename_nb()

FUNCTIONALITY: Renames the file or directory in name to newname. Fails if name is invalid,
newname already exists or path not found.New name may contain a path
part and must contain a name part.  If no path is supplied, the current
working directory is used.
INPUTS:
oldname   : The original pathname of the file.
newname	  : the newname to the file
cp		  : Return path either callback function or GSP communcation id

RETRUN : <0 in case of failures
>0,

 ********************************************************* */
T_FFS_REQ_ID ffs_rename_nb(const char *oldname, const char *newname,
	T_RV_RETURN *cp)
{
	T_Rel_req *Req; MSG_ALLOC(Req);
	memset(Req, 0, sizeof(T_Rel_req));
	Req->msg = REL_RENAME_NB_REQ;
	Req->actualpath = oldname;
	Req->pathname= newname;
	Req->cp = cp;
	Req->req_id = rel_get_free_pair_id ();
	MSG_SEND(Req);
	return Req->req_id;
}



/* ******************************************************
NAME : ffs_rename_uc_nb()

FUNCTIONALITY: Renames the file or directory in name to newname. Fails if name is invalid,
newname already exists or path not found.New name may contain a path
part and must contain a name part.  If no path is supplied, the current
working directory is used.
INPUTS:
oldname   : The original pathname of the file.
newname	  : the newname to the file
cp		  : Return path either callback function or GSP communcation id

RETRUN : <0 in case of failures
>0,

 ********************************************************* */
static T_FFS_REQ_ID ffs_rename_uc_nb(const T_WCHAR *oldname, const T_WCHAR *newname,T_RV_RETURN *cp)
{
	T_Rel_req *Req; MSG_ALLOC(Req);
	memset(Req, 0, sizeof(T_Rel_req));
	Req->msg = REL_RENAME_NB_REQ;
	Req->actualpath = (const char*)oldname;
	Req->pathname= (const char*)newname;
	Req->unicode =1;
	Req->cp = cp;
	Req->req_id = rel_get_free_pair_id ();
	MSG_SEND(Req);
	return Req->req_id;
}




/* ******************************************************
NAME : ffs_fcontrol()

FUNCTIONALITY: Not supported for Reliance FS
CALLED BY: Any application.

INPUTS:
		pathname   : The pathname of the file.
		action	   : The control operation to be carried out.
		param	   : input parameter depending on <action> input variable.


OUTPUTS: None

RETRUN : <0 in case of failures
         >0 in case of success.

********************************************************* */
T_FFS_RET ffs_fcontrol ( const char *pathname, INT8 action, int param )
{

 			return EFFS_OK; /* Not supported for Reliance FS*/
}



/* ******************************************************
NAME : ffs_fcontrol_nb()

FUNCTIONALITY: Not supported for Reliance FS

CALLED BY: Any application.

INPUTS:
		pathname   : The pathname of the file.
		action	   : The control operation to be carried out.
		param	   : input parameter depending on <action> input variable.
		return_path : return path either callback function or GSP communcation id.

OUTPUTS: None

RETRUN : <0 in case of failures
         >0, request id, in case of success.

********************************************************* */
T_FFS_REQ_ID ffs_fcontrol_nb ( const char * pathname, INT8 action, int param, T_RV_RETURN *cp )
{
 	return EFFS_OK; /* Not supported for Reliance FS*/
}

T_FFS_RET ffs_is_modifiable ( const char * pathname )
{
    return EFFS_OK;   /* All the files are allowed to modify */
}

/* ******************************************************
NAME : ffs_query()

FUNCTIONALITY:

INPUTS:

RETRUN : <0 in case of failures
>0,

 ********************************************************* */
T_FFS_RET ffs_query(INT8 query, void *p)
{
    T_FFS_FD fd=-1; /*For NOR */
    int retval;
    int status = REL_OK;
    unsigned int lostblocks;
    REL_STATFS pstatfs;
    struct versioninfo version;
    memset (&version,0,sizeof(version));

    if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
	return (char)ffs_map_ril_error(status);
    }

    if(fd < 0)
    {
	fd = relFs_Open("A:", REL_O_RDONLY, REL_IREAD);
    }
    if(fd < REL_OK)
    {
		return (char)ffs_map_ril_error(fd);
    }

    /*Retrieves information about the file system.*/
    retval = relFs_StatFs( fd, &pstatfs);
    relFs_Close(fd);
    if(retval != REL_OK)
    {
	return (char)ffs_map_ril_error(retval);
    }

    if(retval != REL_OK)
    {
		return (char)ffs_map_ril_error(retval);
    }

    switch (query)
    {
	case Q_BYTES_FREE: *(uint32*)p =( pstatfs.ulFreeBlocks * pstatfs.ulBlockSize);break;
	case Q_BYTES_USED:*(uint32*)p = (pstatfs.ulUsedBlocks * pstatfs.ulBlockSize);break;
	case Q_BYTES_LOST:
			  lostblocks = (pstatfs.ulNumBlocks - (pstatfs.ulUsedBlocks + pstatfs.ulFreeBlocks));
			  *(uint32*)p = lostblocks;break;
	case Q_BYTES_MAX:*(uint32*)p =( pstatfs.ulNumBlocks * pstatfs.ulBlockSize);break;
	case Q_OBJECTS_TOTAL: *(uint32*)p = 0x00;break;
	case Q_BLOCKS_FREE: *(uint32*)p = pstatfs.ulFreeBlocks;break;
	case Q_BYTES_FREE_RAW:*(uint32*)p =( pstatfs.ulFreeBlocks * pstatfs.ulBlockSize);break;
			      /*case Q_DEV_BASE:         *(uint32*)p = (uint32) dev.base; break;*/

			      // RELIANCE versions
	case Q_FFS_API_VERSION:  *(uint32*)p = version.ulVersion; break;
	case Q_FFS_DRV_VERSION:  *(uint32*)p = version.ulVersion; break;
	case Q_FFS_REVISION:     *(uint32*)p = REL_REVISION; break;
	case Q_FFS_FORMAT_WRITE: *(uint32*)p = REL_FORMAT_VERSION; break;
	case Q_FFS_FORMAT_READ:  *(uint32*)p = 0x0000; break;
	case Q_FFS_LASTERROR:    *(int32*)p  = 0x0000; break;
	case Q_FFS_TM_VERSION:   *(int32*)p  = version.ulDebug; break;

				 // File system queries
	case Q_FILENAME_MAX:     *(uint16*)p = 0xff; break;
	case Q_PATH_DEPTH_MAX:   *(uint16*)p = 0x06; break;

	case Q_OBJECTS_FREE:     *(uint16*)p = 0x00;break;
	case Q_INODES_USED:      *(uint16*)p = 0x00; break;
	case Q_INODES_LOST:      *(uint16*)p = 0x00; break;
	case Q_OBJECTS_MAX:      *(uint16*)p = 0x00; break;

	case Q_INODES_MAX:       *(uint16*)p = 0x00; break;
	case Q_CHUNK_SIZE_MAX:   *(uint16*)p = 0x00; break;

				 // File descriptor queris
	case Q_FD_BUF_SIZE:      *(uint32*)p = 0x00; break;
	case Q_FD_MAX:           *(uint16*)p = 0x00; break;

				 // device queries
	case Q_DEV_MANUFACTURER: *(uint16*)p = 0x89; break;
	case Q_DEV_DEVICE:       *(uint16*)p = 0x880A; break;
	case Q_DEV_BLOCKS:       *(uint16*)p = (uint16)pstatfs.ulNumBlocks; break;
	case Q_DEV_ATOMSIZE:     *(uint16*)p = 0x00; break;
	case Q_DEV_DRIVER:       *(uint16*)p = 0x00; break;

				 // Miscellaneous/Internal
	case Q_BLOCKS_FREE_MIN:  *(uint16*)p = 0x00; break;

				 // Debug queries
	case Q_FS_FLAGS:         *(uint16*)p = 0x00; break;
	case Q_FS_INODES:        *(uint16*)p = 0x00; break;
	case Q_FS_ROOT:          *(uint16*)p = 0x00; break;

	case Q_STATS_DRECLAIMS:        *(uint32*)p = 0x00; break;
	case Q_STATS_IRECLAIMS:        *(uint32*)p = 0x00; break;
	case Q_STATS_DATA_RECLAIMED:   *(uint32*)p = 0x00; break;
	case Q_STATS_INODES_RECLAIMED: *(uint32*)p = 0x00; break;
	case Q_STATS_DATA_ALLOCATED:   *(uint32*)p = 0x00; break;
	case Q_REQUEST_ID_LAST:        *(uint32*)p = 0x00; break;
	default:
				       /*Invalid case*/
				       return REL_ERR;

    }
    return REL_OK;
}

/* ******************************************************
NAME : ffs_getdatetime()

FUNCTIONALITY: This functionlity will get creation time,last access time and last modification time of a file/DIR

INPUTS:const char *pathname,T_RTC_DATE_TIME *A_date_time_p, T_RTC_DATE_TIME * crdate_time_p,T_RV_RETURN_PATH return_path
Pathname      : The pathname of the file/DIR.
C_date_time_p : A pointer to creation time.
A_date_time_p : A pointer to last access time.
M_date_time_p : A pointer to last modification time.

RETRUN :  0 on success; < 0 on failures


 ********************************************************* */
T_FFS_RET ffs_getdatetime(const char *pathname,T_RTC_DATE_TIME * C_date_time_p, 
T_RTC_DATE_TIME * A_date_time_p,T_RTC_DATE_TIME * M_date_time_p)
{
    int retval,status = REL_OK;
    T_FFS_FD fd;
	REL_STAT        sRelStat;
    unsigned short  uAttr;

	if(relFs_Check_File_User())
    {
    	status = relFs_Become_File_User();
    }
    if(status != REL_OK)
    {
		return (int32)ffs_map_ril_error(status);
    }
	 
	    fd = relFs_Open(pathname, REL_O_RDONLY, 0x00);
    	if(fd < 0)
    	{
			return (int32)ffs_map_ril_error(fd);
    	}
    	retval = relFs_Stat(fd, &sRelStat);
		
		if(retval < 0)
			return (int32)ffs_map_ril_error(retval);
		
		/*Creation time */
		sec_into_datetime(sRelStat.ulCreation,C_date_time_p);
			
		/*Last Access time */
		sec_into_datetime(sRelStat.ulAccess,A_date_time_p);
		
		/* Last modification time */
		sec_into_datetime(sRelStat.ulModify,M_date_time_p);
  
      /*close the file */
		relFs_Close(fd);	     
 
	 return retval;
	
}

/*-------------------------------------------------------------------
    datetime_into_sec()

    Description
        This routine Converts given date&time in to Seconds elaspsed from 
        1-1-1970    
    Parameters
        datetime  A pointer to the struct T_RTC_DATE_TIME

    Return Value
        time_t     An unsigned long integer(seconds)
-------------------------------------------------------------------*/

time_t	datetime_into_sec(T_RTC_DATE_TIME *datetime)
{
    
    UINT16  current_year,year=1970;        /* current_year                      - [00,99] */
    UINT32 total_days=0;
	UINT16 months_array[12]={31,28,31,30,31,30,31,31,30,31,30,31}, month_count;
	BOOL leap_year;
	time_t time_in_sec;

    /*no of days from 1-1-1970 to 31-12-1999 */

	total_days=10956;
	
	year=2000;
	
	current_year=datetime->year+2000;/*RTC-timer starts from 2000*/
	
	/*no of days from 1-1-2000 to 31/12/(current_year-1) */
	while (year<current_year)
	{
			total_days+=((year%4==0)?((year%100==0)?((year%400 == 0)?366:365):366):365);
			year++;
	}
	
	/*check current_year is leap year or not */
		leap_year=((year%4==0)?((year%100==0)?((year%400 == 0)?TRUE:FALSE):TRUE):FALSE);
	
	/* no of days b/w 1st jan and 1st of current month*/
	for(month_count=0;month_count<(datetime->month-1);month_count++)
	{
		if(month_count == 1 && leap_year == TRUE)	
			total_days+=(months_array[month_count]+1);
		else
			total_days+=months_array[month_count];
		 
	}
	
	/*number of days in the current month */
	total_days+=datetime->day;
	
	/*convert days into sec */
	time_in_sec=(total_days*24*60*60); 

	/*convert time in Hr:min:sec to sec */
	if(datetime->mode_12_hour == TRUE)
	{
		if(datetime->PM_flag == TRUE)
		{
			time_in_sec+= (12*60*60)+(datetime->hour)*60*60+(datetime->minute)*60+(datetime->second);
		}
		else
			time_in_sec+= (datetime->hour)*60*60+(datetime->minute)*60+(datetime->second);
	}
	else
		time_in_sec+= (datetime->hour)*60*60+(datetime->minute)*60+(datetime->second);
	
    /*return time_in_sec*/

	return time_in_sec;
}



/* ******************************************************
NAME : sec_into_datetime()

FUNCTIONALITY: This functionlity will convert sec into date_time format

INPUTS:
date_time_in_sec : date and time in seconds relative to 1/1/1970
temp_time        : A pointer to structure T_RTC_DATE_TIME

RETRUN : 

BOOL
 


 ********************************************************* */
BOOL sec_into_datetime(unsigned long date_time_in_sec, T_RTC_DATE_TIME *temp_time)
{
	
	UINT8 total_days=0,temp_day_week;
	int year;
	UINT8 month_count;
	BOOL leap_year;
	int8 months_array[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	int8 day_array[7]={1,2,3,4,5,6,7},temp;

	if(date_time_in_sec==0)
		return FALSE;
	
	temp_time->second = date_time_in_sec%60;/*second*/
	temp_time->minute = (date_time_in_sec/60)%60;/*minutes */
	temp_time->hour   = (date_time_in_sec/3600)%24;/*hour*/
	if(temp_time->mode_12_hour ==TRUE)
	{
		if(temp_time->hour>12)
		{
		  temp_time->PM_flag =TRUE;
		  temp_time->hour-=12;
		}
		else
		  temp_time->PM_flag=FALSE;
	}

	total_days=(date_time_in_sec/3600/24);/*total days from 1/1/1970 */

	/*number of days from 1/1/2000 */
	total_days-=10956;
	temp_day_week=total_days;/* to find day of the week */
	year=2000;
	while (total_days>365)
	{
       if(((year%4==0)?((year%100==0)?((year%400 == 0)?TRUE:FALSE):TRUE):FALSE))
			total_days-=366;
	   else
	   	total_days-=365;
	   year++;
	
	}
	/*year*/
 	temp_time->year= (UINT8)(year-2000);
	/* check current year is leap year or not */
	leap_year=((year%4==0)?((year%100==0)?((year%400 == 0)?TRUE:FALSE):TRUE):FALSE);
	for(month_count=0;month_count<12;month_count++)
	{
		if(month_count==1 && leap_year==TRUE)
		{
		  if(total_days<=months_array[month_count])
		  {
		  	break;
		  }
		  else
		  	total_days-=months_array[month_count];
		}
		else
		{
		  if(total_days<=months_array[month_count])
		  {
		  	break;
		  }
		  else
		  	total_days-=months_array[month_count];
		}	
	}

	temp_time->month=month_count+1;/*month*/
	temp_time->day=total_days;/*day*/

	/*calculate week day*/
	temp=temp_day_week%7;
	if(temp==6)
		temp_time->wday=0;/*sunday*/
	else
		temp_time->wday=temp+1;
	
	return TRUE;
}

/* ******************************************************
NAME : Rtc_get_datetime_wrapper()

FUNCTIONALITY: This functionlity will call RTC_GetDateTime

INPUTS: temp_datetime a pointer to the structure Rtc_date_time_structure


RETRUN : 0 on success.
         <0 on faliures.

int
 


 ********************************************************* */

time_t Rtc_get_datetime_wrapper()
{
 T_RTC_DATE_TIME temp_datetime;
 
   if(RTC_GetDateTime(&temp_datetime)!=RV_OK)
   	{
   	 return 0;
   	} 
   else 
   	{
   	  return datetime_into_sec(&temp_datetime);
   	}
   	
}



/* ******************************************************
NAME : ffs_setpartition_rfs_nb()

FUNCTIONALITY: Open a directory

INPUTS: dir			: The dir structure to be filled by ffs_opendir function.
buf			: The buffer to be filled by the contents of the folder.
nbytes		: The number of bytes to be written to the buf
 ******************************************************************  */

static T_FFS_REQ_ID ffs_setpartition_rfs_nb(T_FSCORE_PARTITION_TABLE * partition_table,T_RV_RETURN *cp)
{
   PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_SETPART_RFS_NB_REQ;
    req->ptr_data = (void*)(partition_table);
	req->fd = 0;
    req->cp = cp;
    req->req_id = rel_get_free_pair_id ();
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
    return req->req_id;
}



/*------------------------------------------------------------------
FUNCTION : rel_get_free_pair_id

FUNCTIONALITY :

INPUTS:

RETURN:
-------------------------------------------------------------------*/
    static int32
rel_get_free_pair_id (void)
{
    int32 pair_id;

    //  rel_lock_mutex_pair_id ();
    vsi_s_get(VSI_CALLER,ril_env_ctrl_blk_p->mutex_pair_id);

    if(ril_env_ctrl_blk_p->pair_id< REL_REQID_LBOUND)
	ril_env_ctrl_blk_p->pair_id = REL_REQID_LBOUND;

    pair_id = ril_env_ctrl_blk_p->pair_id++;

    if(ril_env_ctrl_blk_p->pair_id > REL_REQID_UBOUND)
	ril_env_ctrl_blk_p->pair_id = REL_REQID_LBOUND;

    //  rel_unlock_mutex_pair_id ();
    vsi_s_release(VSI_CALLER,ril_env_ctrl_blk_p->mutex_pair_id);

    return pair_id;
}

/*------------------------------------------------------------------
FUNCTION : rel_get_free_pair_id

FUNCTIONALITY :Maps flags from ffs2fs to reliance FS

INPUTS:
flags	:  Flags passed by warapper layer.

RETURN:
rflags	:	Mapped flags
-------------------------------------------------------------------*/

static unsigned short ffs_map_ril_flag(T_FFS_OPEN_FLAGS flags )
{
    unsigned short rflags=0;

    if ( flags & FFS_O_CREATE )
	rflags = REL_O_CREAT;
    if ( flags & FFS_O_APPEND )
	rflags |= REL_O_APPEND;
    if (flags & FFS_O_EXCL )
	rflags |= REL_O_EXCL;
    if (flags & FFS_O_TRUNC )
	rflags |= REL_O_TRUNC;
    if (flags & FFS_O_RDONLY )
	rflags |= REL_O_RDONLY;
    if (flags & FFS_O_WRONLY)
	rflags |= REL_O_WRONLY;
    if ((flags & FFS_O_RDONLY) && (flags & FFS_O_WRONLY))
    {
	rflags |= REL_O_RDWR;
	rflags ^= REL_O_WRONLY;
    }

    if ((flags & FFS_O_CREATE) && (flags & FFS_O_TRUNC))
    {
	rflags |= REL_O_RDWR;
    }
    return rflags;
}

int32 ffs_map_ril_error(T_FFS_FD fd)
{
    switch (fd )
    {
	case REL_FAILURE:return EFFS_INVALID;
	case REL_ERR_ACCES:return EFFS_ACCESS;
	case REL_ERR_BADUSER:return EFFS_INVALID;
	case REL_ERR_BADDRIVE:return EFFS_NODEVICE;
	case REL_ERR_BADFILE:return EFFS_NOTAFILE;
	case REL_ERR_BADPARM:return EFFS_INVALID;
	case REL_ERR_BLOCKSIZE:return EFFS_CORRUPTED;
	case REL_ERR_EXIST:return EFFS_EXISTS;
	case REL_ERR_FORMAT:return EFFS_BADFORMAT;
	case REL_ERR_INVNAME:return EFFS_BADNAME;
	case REL_ERR_INVPARCMB:return EFFS_INVALID;
	case REL_ERR_INTERNAL :return EFFS_DRIVER;
	case REL_ERR_IN_USE:return EFFS_ACCESS;
	case REL_ERR_IO_ERROR :return EFFS_DRIVER;
	case REL_ERR_LAST_ENTRY:return EFFS_INVALID;
	case REL_ERR_LONGPATH:return EFFS_PATHTOODEEP;
	case REL_ERR_MAXFILE_SIZE:return EFFS_FILETOOBIG;
	case REL_ERR_NO_MEMORY:return EFFS_MEMORY;
	case REL_ERR_NOEMPTY:return EFFS_DIRNOTEMPTY;
	case REL_ERR_NOFILE:return EFFS_NOTFOUND;
	case REL_ERR_NOSPC:return EFFS_NOSPACE;
	case REL_ERR_NOT_OPENED:return EFFS_BADFD;
	case REL_ERR_PEMFILE:return EFFS_NUMFD;
	case REL_ERR_SHARE:return EFFS_DIRNOTEMPTY;
	case REL_ERR_ISDIR:return EFFS_NOTAFILE;
	default:
			   return EFFS_INVALID;
    }

}



int relfs_map_attr_rfs(int attr)
{

 switch (attr)
 	{
 	  case REL_ATTR_NORMAL		: return FFS_ATTRIB_READWRITE;
	  case REL_ATTR_READ_ONLY 	: return FFS_ATTRIB_READONLY;
	  case REL_ATTR_HIDDEN		: return FFS_ATTRIB_HIDDEN;
	  case REL_ATTR_SYSTEM		: return FFS_ATTRIB_SYSTEM;
	  case REL_ATTR_VOL_LABEL	: return FFS_ATTRIB_PART;
	  case REL_ATTR_DIRECTORY	: return FFS_ATTRIB_SUBDIR;
	  case REL_ATTR_ARCHIVE		: return FFS_ATTRIB_ARCHIVE;
	  default:
	  	       return FFS_ATTRIB_INVALID;
 	}
}
	


void *target_malloc(unsigned int size)
{
    char *buf;

    if ((rvf_get_buf(ril_env_ctrl_blk_p->mbid, size, (T_RVF_BUFFER*) &buf)) == RVF_RED)
        return 0;
    else
        return buf;
}

void target_free(void *buf)
{
   if(buf != NULL)
   {
      rvf_free_buf((T_RVF_BUFFER*) &buf);
   }
}

/* Stub Function Just for Time Being */

T_FFS_RET ffs_exit(void)
{
	int status;
	status = FileSystemUnInitialize();
	return ffs_map_ril_error(status);
}

void ffs_main_init(void)
{
	/* Just to Keep Backward Comptability */
	return;
}

void ffsdrv_copy_code_to_ram(uint16 *dst, uint16 *src, int size)
{
    // The ARM7TDMI compiler sets bit 0 for thumb mode function pointers, so
    // we need to clear this in order to copy *all* bytes. Otherwise we
    // exclude first byte and the resulting copy becomes garbage
    src = (uint16 *) (~1 & (int) src);
    size /= 2;

    while (size--)
        *dst++ = *src++;
    return;
}
void ffsdrv_device_id_read(uint32 base_add,uint32 *manfc,uint16 *device)
{
    volatile unsigned char temp;
	*manfc = 0x01;
	temp = 0x2C;
	device[0] = temp<<8;
    temp = 0x7E;
	device[0] |= temp;
	return;
}

T_FFS_REQ_ID lfs_read(T_FFS_FD fd,char **src)
{
	/* To resolve Linker Issue: Undefine LFS to get rid of this */
	return -1;
}

T_FFS_FD lfs_open(const char *pathname, T_FFS_OPEN_FLAGS option, unsigned int *size_of_file)
{
	/* To resolve Linker Issue: Undefine LFS to get rid of this */
	return -1;
}
T_FFS_RET lfs_close(T_FFS_FD fd)
{
	/* To resolve Linker Issue: Undefine LFS to get rid of this */
	return -1;
}

// Work around till ER for security API's is implemented.

#undef OS_OK
#undef MSG_ALLOC
#undef MSG_SEND
#include "ffs/board/task.h"
#include "ffstrace.h"
#include "drv.h"

OS_MUTEX ffs_write_mutex;
struct dev_s dev;
extern  struct ffsdrv_s ffsdrv;

extern uint32 int_disable(void);
extern void int_enable(uint32 cpsr);
extern uint32 get_cpsr(void);
	
#define addr2offset(address) ( (int) (address) - (int) dev.base )
#define offset2block(offset) (((uint32) offset) >> dev.binfo[0].size_ld)
#define FLASH_READ(addr)        (*(volatile UINT16 *) (addr))
#define FLASH_WRITE(addr, data) (*(volatile UINT16 *) (addr)) = data

// 128x64kw
static const struct block_info_s flash_128x64kw[] =
{
  	{ 0x000000, 17 },
    { 0x020000, 17 },
    { 0x040000, 17 },
    { 0x060000, 17 },
    { 0x080000, 17 },
    { 0x0A0000, 17 },
    { 0x0C0000, 17 },
    { 0x0E0000, 17 },
    { 0x100000, 17 },
    { 0x120000, 17 },
    { 0x140000, 17 },
    { 0x160000, 17 },
    { 0x180000, 17 },
    { 0x1A0000, 17 },
    { 0x1C0000, 17 },
    { 0x1E0000, 17 },
    { 0x200000, 17 },
    { 0x220000, 17 },
    { 0x240000, 17 },
    { 0x260000, 17 },
    { 0x280000, 17 },
    { 0x2A0000, 17 },
    { 0x2C0000, 17 },
    { 0x2E0000, 17 },
    { 0x300000, 17 },
    { 0x320000, 17 },
    { 0x340000, 17 },
    { 0x360000, 17 },
    { 0x380000, 17 },
    { 0x3A0000, 17 },
    { 0x3C0000, 17 },
    { 0x3E0000, 17 },
    { 0x400000, 17 },
    { 0x420000, 17 },
    { 0x440000, 17 },
    { 0x460000, 17 },
    { 0x480000, 17 },
    { 0x4A0000, 17 },
    { 0x4C0000, 17 },
    { 0x4E0000, 17 },
    { 0x500000, 17 },
    { 0x520000, 17 },
    { 0x540000, 17 },
    { 0x560000, 17 },
    { 0x580000, 17 },
    { 0x5A0000, 17 },
    { 0x5C0000, 17 },
    { 0x5E0000, 17 },
    { 0x600000, 17 },
    { 0x620000, 17 },
    { 0x640000, 17 },
    { 0x660000, 17 },
    { 0x680000, 17 },
    { 0x6A0000, 17 },
    { 0x6C0000, 17 },
    { 0x6E0000, 17 },
    { 0x700000, 17 },
    { 0x720000, 17 },
    { 0x740000, 17 },
    { 0x760000, 17 },
    { 0x780000, 17 },
    { 0x7A0000, 17 },
    { 0x7C0000, 17 },
    { 0x7E0000, 17 },
    { 0x800000, 17 },
    { 0x820000, 17 },
    { 0x840000, 17 },
    { 0x860000, 17 },
    { 0x880000, 17 },
    { 0x8A0000, 17 },
    { 0x8C0000, 17 },
    { 0x8E0000, 17 },
    { 0x900000, 17 },
    { 0x920000, 17 },
    { 0x940000, 17 },
    { 0x960000, 17 },
    { 0x980000, 17 },
    { 0x9A0000, 17 },
    { 0x9C0000, 17 },
    { 0x9E0000, 17 },
    { 0xA00000, 17 },
    { 0xA20000, 17 },
    { 0xA40000, 17 },
    { 0xA60000, 17 },
    { 0xA80000, 17 },
    { 0xAA0000, 17 },
    { 0xAC0000, 17 },
    { 0xAE0000, 17 },
    { 0xB00000, 17 },
    { 0xB20000, 17 },
    { 0xB40000, 17 },
    { 0xB60000, 17 },
    { 0xB80000, 17 },
    { 0xBA0000, 17 },
    { 0xBC0000, 17 },
    { 0xBE0000, 17 },
    { 0xC00000, 17 },
    { 0xC20000, 17 },
    { 0xC40000, 17 },
    { 0xC60000, 17 },
    { 0xC80000, 17 },
    { 0xCA0000, 17 },
    { 0xCC0000, 17 },
    { 0xCE0000, 17 },
    { 0xE00000, 17 },
    { 0xE20000, 17 },
    { 0xE40000, 17 },
    { 0xE60000, 17 },
    { 0xE80000, 17 },
    { 0xEA0000, 17 },
    { 0xEC0000, 17 },
    { 0xEE0000, 17 },
    { 0xF00000, 17 },
    { 0xF20000, 17 },
    { 0xF40000, 17 },
    { 0xF60000, 17 },
    { 0xF80000, 17 },
    { 0xFA0000, 17 },
    { 0xFC0000, 17 },
    { 0xFE0000, 17 }

};

struct flash_info_s mbit_spansion_flash_info = {
   &flash_128x64kw[0], 
   (char *) 0x06800000, 
   MANUFACT_AMD, 
   0x3500,
   FFS_DRIVER_AMD_MIRROR_BIT,  
   7  
};
 
void ffsdrv_write_byte(void *dst, uint8 value)
{
    uint16 halfword;

    tw(tr(TR_FUNC, TrDrvWrite, "ffsdrv_write_byte(0x%05x, 0x%x)\n",
       (int) (addr2offset(dst)), value));
    ttw(str(TTrDrvWrite, "wbyte" NL));

    if ((int) dst & 1)
        halfword =                (value << 8) | *((uint8 *) dst - 1);
    else
        halfword = (*((uint8 *) dst + 1) << 8) | (value);

    ffsdrv.write_halfword((uint16 *) ((int) dst & ~1), halfword);
}

void ffsdrv_amd_mb_buffer_write_new(volatile UINT16 *dst_addr, const UINT8 *src_addr, UINT16 size)
{
    volatile uint16 *flash = (volatile uint16*)dev.base;
    volatile UINT16 *dst_ptr;
    const UINT8 *src_ptr;
    volatile UINT16 *last_loaded_addr;
    UINT16 write_data;
    UINT32 word_count;
    UINT32 word_size = size >> 1;
    uint32 cpsr;
    static int badFlashCount=0;

	dst_ptr=dst_addr;
	src_ptr=src_addr;

OS_LOCK_MUTEX(&ffs_write_mutex);
   dev.state=DEV_WRITE;

   cpsr=int_disable();
       // two unlock cycles
	  flash[0x555]=0xAA;
	   flash[0x2AA]=0x55;
	   // Unlock Bypass
	  flash[0x555]=0x20;
    //int_enable(cpsr);

    while (word_size >0)
    {

	// maximum 16 words can be written using write buffer
       word_count = 16;

        if (word_count > word_size)
        {
            word_count = word_size;
        }
        word_size -= word_count;




redowrite_buf:
      //  cpsr=int_disable();

   	flash[0]=0x0f0;

	/* Write to Buffer Command */
        FLASH_WRITE(dst_ptr, 0x25);

	dev.addr=dst_ptr;
	dev.data=src_ptr[0] | (src_ptr[1] << 8);

        /* Write number of locations to program */
        FLASH_WRITE(dst_ptr, word_count - 1);


        /* Load data into buffer */
        while (word_count > 0)
        {
            /* Store last loaded address and data value (for polling) */
            last_loaded_addr = dst_ptr;
            write_data = src_ptr[0] | (src_ptr[1] << 8);

            /* Write Data */
		*dst_ptr=write_data;
            dst_ptr++;
            src_ptr +=2;
            word_count--;
        }

        /* Program Buffer to Flash Command */
        FLASH_WRITE(last_loaded_addr, 0x29);
        


        /* Poll for completion, then check result */
	   while (1)
	   {
	   		unsigned short temp= (*last_loaded_addr);
			badFlashCount++;
			if(!((temp ^ write_data) & 0x80))
				break;
			else if ((temp & 0x20))
			{
				 if((((*last_loaded_addr) ^ write_data) & 0x80))
				 	{
				 	   	goto redowrite_buf;
				 	}
				 break;
			}
			else
				continue;
	   }

    }

FLASH_WRITE(dst_ptr, 0x90);
FLASH_WRITE(dst_ptr, 0x00);
    int_enable(cpsr);
   /* change the state to default read mode */
   dev.state=DEV_READ;

   OS_UNLOCK_MUTEX(&ffs_write_mutex);

}

int ffsdrv_amd_mb_init(void)
{
	dev.state = DEV_READ;
	dev.manufact = mbit_spansion_flash_info.manufact;
	dev.device = mbit_spansion_flash_info.device;
	dev.binfo = (struct block_info_s *)(mbit_spansion_flash_info.binfo);


	dev.base = (char *)0x06800000;
	dev.base_norbm = (char *)0x06900000;
	dev.numblocks_norbm = 0x0C;
	dev.write_buffersize = 0x0040;

	dev.numblocks = mbit_spansion_flash_info.numblocks;
	dev.driver = mbit_spansion_flash_info.driver;

	dev.blocksize_ld = dev.binfo[0].size_ld;
	dev.blocksize = (1 << dev.blocksize_ld);

	dev.atomlog2 = FFS_ATOM_LOG2;
	dev.atomsize = 1 << dev.atomlog2;
	dev.atomnotmask = dev.atomsize - 1;

	 return EFFS_OK;
}


void ffsdrv_amd_mb_erase(uint8 block)
{
    volatile uint16 *flash = (volatile uint16*)dev.base;
    uint32 cpsr;
	uint32 unlockOffset;


/* Acquire the mutex */
  OS_LOCK_MUTEX(&ffs_write_mutex);

    dev.addr = (uint16 *) block2addr(block);
    unlockOffset = ((uint32)(dev.addr)-(uint32)(dev.base) + 0x80) >> 1;

    /* change the device status */
    dev.state=DEV_ERASE;

   /* disable the interrupts */
   cpsr=int_disable();

   /* sector erase command -- 6 cycles */
   flash[0x555] =0xAA;
   flash[0x2AA]=0x55;
   flash[0x555]=0x80;
   flash[0x555]= 0xAA;
   flash[0x2AA]=0x55;
   *((volatile uint16*)(dev.addr))=0x30;   /* write the command to (short) sector address */

   /* Enable the interrupts */
 //CQ72069 start - new while loop added

 // delay re-enabling until after sector erased or we have an int to service

 /* Wait for the erase to complete  */
 while ((*dev.addr & 0x80) == 0)   /* DQ7 will have 1 on completion of erase */  /* Data polling */
 // add option to suspend if a frame interrupt occurs, same as old AMD single bank driver

 {  	 // Poll interrupts, taking interrupt mask into account and check if IRQ was actually enabled
        if ((INT_REQUESTED) && !(cpsr & ~0x80))
        {
            // 1. suspend erase
            // 2. enable interrupts
            // .. now the interrupt code executes
            // 3. disable interrupts
            // 4. resume erase

            // Suspend erase
            *((volatile uint16*)(dev.addr)) = 0xB0;

            // wait for erase suspend to finish
            while ((*((volatile uint16*)(dev.addr)) & 0x80) == 0);

            dev.state = DEV_ERASE_SUSPEND;
   		int_enable(cpsr);

            // Other interrupts and tasks run now...
            cpsr=int_disable();

             // Before resuming erase we must? check if the erase is really
            // suspended or if it did finish
            *((volatile uint16*)(dev.addr)) = 0x30;
            dev.state = DEV_ERASE;

        }
 }
 int_enable(cpsr);
//CQ72069 end

 ttw(ttr(TTrDrvErase, "}" NL));

 /* change the device state  to default READ mode */
    dev.state=DEV_READ;
  OS_UNLOCK_MUTEX(&ffs_write_mutex);
}


void ffsdrv_amd_mb_write_halfword(volatile uint16 *addr, uint16 value)
{
    volatile uint16 *flash = (volatile uint16*)dev.base;
    uint32 cpsr;
	uint8 * offset;
	uint32 unlockOffset;
	static int badFlashCount=0;
	unsigned short temp1;


   /* device state is write state */
   OS_LOCK_MUTEX(&ffs_write_mutex);

   offset = (uint8*)addr - (uint32)(dev.base);
   unlockOffset = (dev.binfo[offset2block(offset)].offset+ 0x80)>>1;

   dev.state=DEV_WRITE;
	temp1 = (*addr);

redowrite:
    /* Dissable the interrupts */
   cpsr=int_disable();

   flash[0]=0x0f0;

    /* two unlock cycles */
   flash[0x555]=0xAA;
   flash[0x2AA]=0x55;

   /* issue the program command now */
   flash[0x555]=0xA0;

   dev.addr=addr;    /* record the last write */
   dev.data=value;

   *addr = value;   /* this will be base+offset */

   

   /* Wait for the write to complete */
   while (1)
   {
   		unsigned short temp= (*addr);
		badFlashCount++;
		if(!((temp ^ dev.data) & 0x80))
			break;
		else if ((temp & 0x20))
		{
			 if((((*addr) ^ dev.data) & 0x80))
			 	{
			 	  goto redowrite;
			 	}
			 break;
		}
		else
			continue;
   }/* Data polling */
 /* Enable the interrupts */

    int_enable(cpsr);
   /* change the state to default read mode */
   dev.state=DEV_READ;

   OS_UNLOCK_MUTEX(&ffs_write_mutex);

}

void ffsdrv_amd_mb_buffer_write(void *dst, const void *src, uint16 size)
{
    uint8 *mydst = dst;
    const uint8 *mysrc = src;
    uint8 unalignedbytes = 0;
    uint16 sizeEven;

// use halfword write for size less than 32 bytes
    if (size <= 32)
    {
        if ((unsigned int) mydst & 1) {
            ffsdrv_write_byte(mydst++, *mysrc++);
            size--;
        }
	while (size >= 2)
	{
            ffsdrv_amd_mb_write_halfword((uint16 *) mydst, mysrc[0] | (mysrc[1] << 8));
	            size -= 2;
	            mysrc += 2;
	            mydst += 2;
	}
	if (size == 1)
	{
		ffsdrv_write_byte(mydst++, *mysrc++);
		size--;
	 }
    }
// If size is more than 32 bytes then go for write buffer feature
    else if (size > 32)
    {
	// if the destination addredd is odd, write first byte in that.
        if ((unsigned int) mydst & 1) {
            ffsdrv_write_byte(mydst++, *mysrc++);
            size--;
        }
	// write unaligned bytes in the write buffer page, since write buffer feature does not allow
	// write across write buffer boundaries and write can not start in the midded of the write buffer page.
	unalignedbytes = (0x20 - (unsigned int) mydst & 0x0000001F);
	if(unalignedbytes > 0)
	{
	       while (unalignedbytes > 0)
		{
				ffsdrv_amd_mb_write_halfword((uint16 *) mydst, mysrc[0] | (mysrc[1] << 8));
				size -= 2;
				mysrc += 2;
				mydst += 2;
				unalignedbytes -= 2;
			}
	}

	// Write data using buffer write function
        if (size >=2)
        {
	     // Truncate to an even number of bytes
            sizeEven = size & (uint16) ~1;
	// write the data using write buffer feature
	ffsdrv_amd_mb_buffer_write_new((uint16 *) mydst, mysrc, sizeEven);
		size -=sizeEven;
		mysrc += sizeEven;
		mydst += sizeEven;

        }
//	Write Last byte

	if(size==1 )
	{
		ffsdrv_write_byte(mydst++, *mysrc++);
            	size--;
	}
    }
}

void ffsdrv_amd_mb_write_suspend(void)
{
    /* get the mutex */
    OS_LOCK_MUTEX(&ffs_write_mutex);
    dev.state=DEV_WRITE_SUSPEND;
}


void ffsdrv_amd_mb_write_resume(void)
{
  dev.state = DEV_READ;
 /* Unlock the mutex */
 OS_UNLOCK_MUTEX(&ffs_write_mutex);
}


void ffsdrv_amd_mb_erase_suspend(void)
{
    /* get the mutex */
    OS_LOCK_MUTEX(&ffs_write_mutex);
	dev.state=DEV_ERASE_SUSPEND;
}

void ffsdrv_amd_mb_erase_resume(void)
{
  dev.state= DEV_READ;
 /* Unlock the mutex */
 OS_UNLOCK_MUTEX(&ffs_write_mutex);
}



void ffsdrv_amd_mb_erase_sector(void *dst)
{
    volatile uint16 *flash;
    uint32 cpsr;
    UINT32 DYBstatus;

    DYBstatus=1;


    OS_LOCK_MUTEX(&ffs_write_mutex);

	 /* change the device status */
    dev.state=DEV_ERASE;

   /* disable the interrupts */
   cpsr=int_disable();

    dev.addr = (uint16 *) dst;
    flash= dev.addr;


 // DYB bit is dynamically set.
 //As per data sheet 11.14.1. Upon power-up or a hadrware reset, the contents of all DYBs are
 //set (programmed to "0") means locked.
 //But practical observation is that DYBs are either "0" or "1" not predictable.
 // Hence this code has been added.
     //DYB command set entry: 3 cycles
       flash[0x555]=0xAA;
       flash[0x2AA]=0x55;
       flash[0x555]=0xE0;
	DYBstatus=*((volatile uint16*)(dev.addr));
	if (DYBstatus==0)
	{
		flash[0]=0xA0;
            *((volatile uint16*)(dev.addr))=0x01;
	}
	flash[0]=0x90;
   	flash[0]=0x00;

   /* sector erase command -- 6 cycles */
   flash[0x555] =0xAA;
   flash[0x2AA]=0x55;
   flash[0x555]=0x80;
   flash[0x555]= 0xAA;
   flash[0x2AA]=0x55;
   *((volatile uint16*)(dev.addr))=0x30;   /* write the command to sector address */

  

      while ((*dev.addr & 0x80) == 0)   /* DQ7 will have 1 on completion of erase */  /* Data polling */
        // add option to suspend if a frame interrupt occurs, same as old AMD single bank driver

    {  	 // Poll interrupts, taking interrupt mask into account and check if IRQ was actually enabled
        if ((INT_REQUESTED) && !(cpsr & ~0x80))
        {
            // 1. suspend erase
            // 2. enable interrupts
            // .. now the interrupt code executes
            // 3. disable interrupts
            // 4. resume erase

            // Suspend erase
            *((volatile uint16*)(dev.addr)) = 0xB0;

            // wait for erase suspend to finish
            while ((*((volatile uint16*)(dev.addr)) & 0x80) == 0);

            dev.state = DEV_ERASE_SUSPEND;
            int_enable(cpsr);

            // Other interrupts and tasks run now...
            cpsr=int_disable();

            // Before resuming erase we must? check if the erase is really
            // suspended or if it did finish
            *((volatile uint16*)(dev.addr)) = 0x30;
            dev.state = DEV_ERASE;

        }
    }
    int_enable(cpsr);
    //CQ72069 end 

 /* change the device state  to default READ mode */
    dev.state=DEV_READ;
   OS_UNLOCK_MUTEX(&ffs_write_mutex);

}

void ffsdrv_null_write_buffer(volatile uint16 *addr, volatile uint16 *src,
                              uint16 words2write)
{
    ttw(ttr(TTrDrvWrite, "ffsdrv_null_write_buffer(0x%x, 0x%x)"
            NL, addr, words2write));
}

struct ffsdrv_s ffsdrv = {
	ffsdrv_amd_mb_init,
	ffsdrv_amd_mb_erase,
    ffsdrv_null_write_buffer,
	ffsdrv_amd_mb_write_halfword,
	ffsdrv_amd_mb_buffer_write,
	ffsdrv_amd_mb_write_suspend,             
       ffsdrv_amd_mb_write_resume,
       ffsdrv_amd_mb_erase_suspend,            
       ffsdrv_amd_mb_erase_resume,            
	ffsdrv_amd_mb_erase_sector

};

uint32 int_disable(void)
{
    __asm("        .state16");
    __asm("        mov       A1, #0xC0");
    __asm("        ldr       A2, tct_disable");
    __asm("        bx        A2      ");

    // XXX Fails to compile XXX __asm("        .align");  // word align
    __asm("tct_disable    .field     _TCT_Control_Interrupts+0,32");
    __asm("               .global    _TCT_Control_Interrupts");
}

void int_enable(uint32 cpsr)
{
    __asm("        .state16");
    __asm("        ldr       A2, tct_enable");
    __asm("        bx        A2      ");

    __asm("tct_enable     .field     _TCT_Control_Interrupts+0,32");
    __asm("               .global    _TCT_Control_Interrupts");
}

uint32 get_cpsr(void)
{
   	__asm("        .state16");
    	__asm("        add       A2, PC, #0x4"); //
    	__asm("        bx       A2");
     	__asm("	  .state32");
    	__asm("get_cpsr32:	");
//       __asm("MRS		R0,CPSR");// get current CPSR");
  	__asm(" 	  mrs      A1, cpsr");
}





/* ------------------------------------------------------------------------- */
/* 	Rivera File system Wrapper functions									 */            
/* ------------------------------------------------------------------------- */


static T_WCHAR *  skip_mount_point_name(T_WCHAR * pathname)
{

 while (pathname[0] == '/')
	{
	  pathname++;
	}
  pathname = (T_WCHAR *) wstrchr (pathname, '/');
  return pathname;
}




static T_FSCORE_RET relfs_rfs_close (T_FSCORE_FD fd)
{
  return ffs_close_nb(fd, RELFS_RFS_STREAM_RET_PATH);
}


static T_FSCORE_RET relfs_rfs_fchmod (T_FSCORE_FD fd, T_FSCORE_MODE mode)
{

    return ffs_fchmod_rfs_nb (fd,RELFS_RFS_STREAM_RET_PATH);

   
}


static  T_FSCORE_RET relfs_rfs_fcntl (T_FSCORE_FD fd, INT8 cmd, void *arg_p)
{
  return FSCORE_EOK;
}



static  T_FSCORE_SIZE relfs_rfs_write (T_FSCORE_FD fd,const void *buf, T_FSCORE_SIZE size)
{
  return ffs_write_nb(fd,(void*)buf,size, RELFS_RFS_STREAM_RET_PATH);
   
}

static T_FSCORE_RET relfs_rfs_trunc (T_FSCORE_FD fd, T_FSCORE_SIZE size)
{
  return ffs_ftruncate_nb(fd, size, RELFS_RFS_STREAM_RET_PATH);
}

static T_FSCORE_RET relfs_rfs_fstat (T_FSCORE_FD fd, T_FSCORE_STAT * stat)
{
  return ffs_fstat_rfs_nb(fd, stat, RELFS_RFS_STREAM_RET_PATH);     
}


static T_FSCORE_RET relfs_rfs_fsync (T_FSCORE_FD fd)
{
  
  return ffs_fdatasync_nb(fd,RELFS_RFS_STREAM_RET_PATH);
}


static T_FSCORE_OFFSET relfs_rfs_lseek (T_FSCORE_FD fd, T_FSCORE_OFFSET offset, T_FSCORE_WHENCE whence)
{
  return ffs_seek_nb(fd,offset, whence, RELFS_RFS_STREAM_RET_PATH); 
}


static T_FSCORE_RET relfs_rfs_set_partition_table (T_FSCORE_PARTITION_TABLE * partition_table,
							T_RV_RETURN return_path)
{
  return ffs_setpartition_rfs_nb(partition_table,RELFS_RFS_STREAM_RET_PATH);
}

static T_FSCORE_SIZE  relfs_rfs_read (T_FSCORE_FD fd, void *buf, T_FSCORE_SIZE size)
{
 
  return ffs_read_nb(fd, buf,size,RELFS_RFS_STREAM_RET_PATH);
}

static T_FSCORE_RET relfs_rfs_readdir_uc_nb (T_FSCORE_DIR * dirp,
				   T_WCHAR *buf, T_FSCORE_SIZE size, T_RV_RETURN_PATH return_path)
{
   return ffs_readdir_rfs_uc_nb(dirp, buf, size, RELFS_RFS_STREAM_RET_PATH);
}



static T_FSCORE_RET    relfs_rfs_closedir_nb(T_FSCORE_DIR   *dirp, T_RV_RETURN   return_path)
{
   return ffs_closedir_rfs_nb(dirp, RELFS_RFS_STREAM_RET_PATH);
}



static T_FSCORE_RET relfs_rfs_format_uc_nb(
							const T_WCHAR*			pathname,
                    		const T_WCHAR        *name,
                          	UINT16                magic,  
                          	T_RV_RETURN_PATH          return_path)
{

  return ffs_format_uc_nb(NULL, magic, RELFS_RFS_FILE_RET_PATH);
}







static T_FSCORE_RET relfs_rfs_mkdir_uc_nb(const T_WCHAR   *pathname, 
                                T_FSCORE_MODE   mode,  
                                T_RV_RETURN_PATH  return_path)
{
 return ffs_mkdir_uc_nb((const T_WCHAR*)skip_mount_point_name((T_WCHAR*)pathname), RELFS_RFS_FILE_RET_PATH);
}



static T_FSCORE_RET relfs_rfs_opendir_uc_nb (const T_WCHAR *pathname,
										 T_FSCORE_DIR * dirp,
										 T_RV_RETURN_PATH return_path)
{
  return ffs_opendir_rfs_uc_nb((const T_WCHAR*)skip_mount_point_name((T_WCHAR*)pathname), dirp, RELFS_RFS_FILE_RET_PATH);   
}



static T_FFS_OPEN_FLAGS convert_flags_rfs_to_ffs(T_FSCORE_FLAGS flags)
{
  const T_FSCORE_FLAGS    valid_flags = FSCORE_O_RDONLY | FSCORE_O_WRONLY | FSCORE_O_RDWR | FSCORE_O_CREATE | FSCORE_O_APPEND | FSCORE_O_TRUNC | FSCORE_O_EXCL;
  T_FFS_OPEN_FLAGS ffs_flags=FFS_O_EMPTY; 

 if(flags&(~valid_flags))   return ffs_flags;
 	  

 if((flags&FSCORE_O_RDONLY) ==  FSCORE_O_RDONLY)
 	{
 	       ffs_flags |= FFS_O_RDONLY;
 	}

 if((flags&FSCORE_O_WRONLY) == FSCORE_O_WRONLY)
 	{
 	    ffs_flags |= FFS_O_RDWR;    /* set to both read and write */
 	}

 if((flags&FSCORE_O_RDWR) == FSCORE_O_RDWR)
 	{
 	    ffs_flags |= FFS_O_RDWR;
 	}

 if((flags&FSCORE_O_CREATE) == FSCORE_O_CREATE)
 	{
 	    ffs_flags |= FFS_O_CREATE;
 	}

 if((flags&FSCORE_O_APPEND) == FSCORE_O_APPEND)
 	{
 	    ffs_flags |= FFS_O_APPEND;
 	} 


 if((flags&FSCORE_O_TRUNC) == FSCORE_O_TRUNC)
 	{
 	    ffs_flags |= FFS_O_TRUNC;
 	} 

 if((flags&FSCORE_O_EXCL) == FSCORE_O_EXCL)
 	{
 	    ffs_flags |= FFS_O_EXCL;
 	} 
 

  return ffs_flags;
  
}



static T_FSCORE_RET relfs_rfs_open_uc_nb(
						  const T_WCHAR*		pathname,
                          T_FSCORE_FLAGS        flags, 
                          T_FSCORE_MODE         mode,  
                          T_RV_RETURN_PATH           return_path)
{
 T_FFS_OPEN_FLAGS ffs_flags;
 ffs_flags = convert_flags_rfs_to_ffs(flags);
 
 return ffs_open_uc_nb((const T_WCHAR*)skip_mount_point_name((T_WCHAR*)pathname),ffs_flags, RELFS_RFS_FILE_RET_PATH);     /* check the flags */
} 



static T_FSCORE_RET relfs_rfs_preformat_uc_nb(
								  const T_WCHAR  *pathname,
                                  UINT16  magic,  
                                  T_RV_RETURN_PATH return_path)
{
  return  ffs_preformat_uc_nb(pathname, magic,RELFS_RFS_FILE_RET_PATH);
}



static T_FSCORE_RET relfs_rfs_remove_uc_nb( const T_WCHAR  *pathname, T_RV_RETURN_PATH return_path)
{
   return  ffs_remove_uc_nb((const T_WCHAR*)skip_mount_point_name((T_WCHAR*)pathname), RELFS_RFS_FILE_RET_PATH);
}



static T_FSCORE_RET relfs_rfs_rename_uc_nb(		const T_WCHAR *oldname,
 										const T_WCHAR *newname,
										T_RV_RETURN_PATH return_path)
{
  return ffs_rename_uc_nb((const T_WCHAR*)skip_mount_point_name((T_WCHAR*)oldname), (const T_WCHAR*)skip_mount_point_name((T_WCHAR*)newname), RELFS_RFS_FILE_RET_PATH);
}

static T_FSCORE_RET relfs_rfs_rmdir_uc_nb(const T_WCHAR *pathname, T_RV_RETURN_PATH return_path)
{
 return ffs_rmdir_uc_nb((const T_WCHAR*)skip_mount_point_name((T_WCHAR*)pathname), RELFS_RFS_FILE_RET_PATH);
}


static T_FSCORE_RET relfs_rfs_stat_uc_nb( const T_WCHAR  *pathname, T_FSCORE_STAT  *stat,T_RV_RETURN_PATH return_path)
{
   return ffs_stat_rfs_uc_nb((const T_WCHAR*)skip_mount_point_name((T_WCHAR*)pathname), stat, RELFS_RFS_FILE_RET_PATH);
}



static T_FSCORE_RET relfs_rfs_chmod_uc_nb (const T_WCHAR *pathname, T_FSCORE_MODE mode, T_RV_RETURN return_path)
{
 return ffs_chmod_rfs_uc_nb((const T_WCHAR*)skip_mount_point_name((T_WCHAR*)pathname),RELFS_RFS_FILE_RET_PATH);
}


static T_FSCORE_RET relfs_rfs_getattribute_uc_nb(const T_WCHAR *pathname,T_RV_RETURN_PATH return_path)
{
 return ffs_get_attribute_rfs_uc_nb((const T_WCHAR*)skip_mount_point_name((T_WCHAR*)pathname), RELFS_RFS_FILE_RET_PATH);
}


static T_FSCORE_RET relfs_rfs_setattribute_uc_nb(const T_WCHAR *pathname,T_FFS_ATTRIB_TYPE attrib,T_RV_RETURN_PATH return_path)
{
  return ffs_set_attribute_rfs_uc_nb((const T_WCHAR*)skip_mount_point_name((T_WCHAR*)pathname),attrib, RELFS_RFS_FILE_RET_PATH);
}



static T_FSCORE_RET relfs_rfs_getdatetime_uc_nb(const T_WCHAR *pathname,T_RTC_DATE_TIME *date_time_p,T_RTC_DATE_TIME * crdate_time_p,T_RV_RETURN_PATH return_path)
{
  return FSCORE_ENOTSUPPORTED;
}



static T_FSCORE_RET relfs_rfs_mount_nb(T_RV_RETURN_PATH          return_path)
{
   return FSCORE_ENOTSUPPORTED;
}


static T_FSCORE_RET relfs_rfs_unmount_nb(T_RV_RETURN_PATH          return_path)
{
   return FSCORE_ENOTSUPPORTED;
}



/**
 * function:  rfsfat_get_func_table
 */
T_FSCORE_RET relfs_get_func_table (T_FSCORE_FUNC_TABLE * func_table_p)
{

  func_table_p->fscore_close = relfs_rfs_close;
  func_table_p->fscore_fchmod = relfs_rfs_fchmod;
  func_table_p->fscore_fcntl = relfs_rfs_fcntl;
  func_table_p->fscore_write = relfs_rfs_write;
  func_table_p->fscore_trunc = relfs_rfs_trunc;
  func_table_p->fscore_fstat = relfs_rfs_fstat;
  func_table_p->fscore_fsync = relfs_rfs_fsync;
  func_table_p->fscore_lseek = relfs_rfs_lseek;
  func_table_p->fscore_set_partition_table = relfs_rfs_set_partition_table;
  func_table_p->fscore_read = relfs_rfs_read;
  func_table_p->fscore_mount_nb = relfs_rfs_mount_nb;
  func_table_p->fscore_unmount_nb = relfs_rfs_unmount_nb;

/* Unicode functions */  
  func_table_p->fscore_format_uc_nb = relfs_rfs_format_uc_nb;
  func_table_p->fscore_mkdir_uc_nb = relfs_rfs_mkdir_uc_nb;
  func_table_p->fscore_opendir_uc_nb = relfs_rfs_opendir_uc_nb;
  func_table_p->fscore_open_uc_nb = relfs_rfs_open_uc_nb;
  func_table_p->fscore_preformat_uc_nb = relfs_rfs_preformat_uc_nb;
  func_table_p->fscore_readdir_uc_nb = relfs_rfs_readdir_uc_nb;
  func_table_p->fscore_remove_uc_nb = relfs_rfs_remove_uc_nb;
  func_table_p->fscore_rename_uc_nb = relfs_rfs_rename_uc_nb;
  func_table_p->fscore_rmdir_uc_nb = relfs_rfs_rmdir_uc_nb;
  func_table_p->fscore_stat_uc_nb = relfs_rfs_stat_uc_nb;
  func_table_p->fscore_chmod_uc_nb = relfs_rfs_chmod_uc_nb;
  func_table_p->fscore_getattrib_uc_nb = relfs_rfs_getattribute_uc_nb;
  func_table_p->fscore_setattrib_uc_nb = relfs_rfs_setattribute_uc_nb;
  func_table_p->fscore_getdatetime_uc_nb = relfs_rfs_getdatetime_uc_nb;
  func_table_p->fscore_closedir_uc_nb = relfs_rfs_closedir_nb;

/* Ascii functions */
  func_table_p->fscore_format_nb = NULL;
  func_table_p->fscore_mkdir_nb = NULL;
  func_table_p->fscore_opendir_nb = NULL;
  func_table_p->fscore_open_nb = NULL;
  func_table_p->fscore_preformat_nb = NULL;
  func_table_p->fscore_readdir_nb = NULL;
  func_table_p->fscore_remove_nb = NULL;
  func_table_p->fscore_rename = NULL;
  func_table_p->fscore_rmdir_nb = NULL;
  func_table_p->fscore_stat_nb = NULL;	
  func_table_p->fscore_chmod_nb = NULL;
  func_table_p->fscore_getattrib_nb = NULL;
  func_table_p->fscore_setattrib_nb = NULL;
  func_table_p->fscore_getdatetime_nb = NULL;
    
  return FSCORE_EOK;
}



/* Blocking call to Garbage collection */
/* ******************************************************
NAME : ffs_Garbage_Collection()

FUNCTIONALITY: Get the statistics of the file/directory/partition.

INPUTS: path	: The pathname of the file.
stat  	: structure to get the statistic of the file.

RETRUN : <0 in case of failures
>0 request id .

 ********************************************************* */
void ffs_Garbage_Collection(void)
{
  unsigned int used=0,free=0;

  /* check the channel status  */
  vsi_c_status(ril_task_handle,&used,&free);

   if( (free != 0) && (ril_env_ctrl_blk_p->Garbage_Collection_Process == FALSE))
   	{
   	 
 	PALLOC(req,Rel_req);
    memset(req, 0, sizeof(T_Rel_req));
    req->msg = REL_START_GC_REQ;
    PSEND(ril_env_ctrl_blk_p->addr_id,req);
	ril_env_ctrl_blk_p->Garbage_Collection_Process=TRUE;
	
   	}
   
    return; 
}

