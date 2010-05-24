/**
 * @file	rfs_message.h
 *
 * Data structures:
 * 1) used to send messages to the RFS SWE,
 * 2) RFS can receive.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/23/2004	 ()		Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __RFS_MESSAGE_H_
#define __RFS_MESSAGE_H_


#include "rv/rv_general.h"

#include "rfs/rfs_cfg.h"


#ifdef __cplusplus
extern "C"
{
#endif


/** 
 * The message offset must differ for each SWE in order to have 
 * unique msg_id in the system.
 */
#define RFS_MESSAGE_OFFSET	 BUILD_MESSAGE_OFFSET(RFS_USE_ID)


/**
 * @name RFS_OPEN_REQ_MSG
 *
 * Detailled description
 * The T_RFS_OPEN_REQ_MSG message can be used to open a file. This message is similar
 * to the rfs_open_nb() function. The RFS responds with a T_RFS_READY_RSP_MSG message,
 * with com-mand index: T_RFS_OPEN_RSP.
 */
/*@{*/

/** Open request. */
#define RFS_OPEN_REQ_MSG (RFS_MESSAGE_OFFSET | 0x001)

typedef struct {
    T_RV_HDR      hdr;
    const T_WCHAR *pathname;
    T_RFS_FLAGS   flags;
    T_RFS_MODE    mode;
    T_RV_RETURN   return_path;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
    T_RFS_PAIR_VALUE pair_value;
} T_RFS_OPEN_REQ_MSG;

/**
 * @name RFS_CHMOD_REQ_MSG
 *
 * Detailled description
 * The T_RFS_CHMODE_REQ_MSG message can be used to change the permission attributes 
 * of a file or directory. This message is similar to the rfs_chmod_nb() function. 
 * The RFS responds with a T_RFS_READY_RSP_MSG message, with command index: 
 * RFS_CHMOD_RSP.
 */
/*@{*/

/** Chmod request. */
#define RFS_CHMOD_REQ_MSG (RFS_MESSAGE_OFFSET | 0x002)

typedef struct {
    T_RV_HDR 		  hdr;
    const T_WCHAR	*pathname;
    T_RFS_MODE		mode;
    T_RV_RETURN		return_path;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
    T_RFS_PAIR_VALUE pair_value;
} T_RFS_CHMOD_REQ_MSG;

/**
 * @name RFS_STAT_REQ_MSG
 *
 * Detailled description
 * The T_RFS_STAT_REQ_MSG message can be used to obtain information about the file, 
 * directory or device. This message is similar to the rfs_stat_nb() function. The 
 * RFS responds with a T_RFS_READY_RSP_MSG message, with command index: RFS_STAT_RSP.
 */
/*@{*/

/** Stat request. */
#define RFS_STAT_REQ_MSG (RFS_MESSAGE_OFFSET | 0x003)

typedef struct {
    T_RV_HDR 		  hdr;
    const T_WCHAR	*pathname;
    T_RFS_STAT 		*stat;
    T_RV_RETURN		return_path;
    T_RFS_PAIR_VALUE pair_value;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
} T_RFS_STAT_REQ_MSG;

/**
 * @name RFS_REMOVE_REQ_MSG
 *
 * Detailled description
 * The T_RFS_REMOVE_REQ_MSG message can be used to remove a file or directory. This 
 * message is similar to the rfs_remove_nb() function. The RFS responds with a 
 * T_RFS_READY_RSP_MSG message, with command index: RFS_REMOVE_RSP.
 */
/*@{*/

/** Remove request. */
#define RFS_REMOVE_REQ_MSG (RFS_MESSAGE_OFFSET | 0x004)

typedef struct {
    T_RV_HDR 		  hdr;
    const T_WCHAR	*pathname;
    T_RV_RETURN		return_path;
    T_RFS_PAIR_VALUE pair_value;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
} T_RFS_REMOVE_REQ_MSG;


/**
 * @name RFS_RENAME_REQ_MSG
 *
 * Detailled description
 * The T_RFS_RENAME_REQ_MSG message can be used to rename a file or directory. 
 * This message is similar to the rfs_rename_nb() function. The RFS responds with a
 * T_RFS_READY_RSP_MSG message, with command index: RFS_RENAME_RSP.
 */
/*@{*/

/** Rename request. */
#define RFS_RENAME_REQ_MSG (RFS_MESSAGE_OFFSET | 0x005)

typedef struct {
    T_RV_HDR      hdr;
    const T_WCHAR	*oldname;
    const T_WCHAR	*newname;
    T_RV_RETURN		return_path;
    T_RFS_PAIR_VALUE pair_value;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
} T_RFS_RENAME_REQ_MSG;

/**
 * @name RFS_MKDIR_REQ_MSG
 *
 * Detailled description
 * The T_RFS_MKDIR_REQ_MSG message can be used to create a directory. This message is 
 * similar to the rfs_mkdir_nb() function. The RFS responds with a T_RFS_READY_RSP_MSG 
 * message, with command index: RFS_MKDIR_RSP.
 */
/*@{*/

/** Mkdir request. */
#define RFS_MKDIR_REQ_MSG (RFS_MESSAGE_OFFSET | 0x006)

typedef struct {
    T_RV_HDR 		  hdr;
    const T_WCHAR		*pathname;
    T_RFS_MODE 		mode;
    T_RFS_PAIR_VALUE pair_value;
    T_RV_RETURN		return_path;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
} T_RFS_MKDIR_REQ_MSG;

/**
 * @name RFS_RMDIR_REQ_MSG
 *
 * Detailled description
 * The T_RFS_RMDIR_REQ_MSG message can be used to remove a directory. This message is
 * similar to the rfs_rmdir_nb() function. The RFS responds with a T_RFS_READY_RSP_MSG
 * message, with command index: RFS_RMDIR_RSP.
 */
/*@{*/

/** Rmdir request. */
#define RFS_RMDIR_REQ_MSG (RFS_MESSAGE_OFFSET | 0x007)

typedef struct {
    T_RV_HDR 		  hdr;
    const T_WCHAR	*pathname;
    T_RV_RETURN		return_path;
    T_RFS_PAIR_VALUE pair_value;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
} T_RFS_RMDIR_REQ_MSG;

/**
 * @name RFS_OPENDIR_REQ_MSG
 *
 * Detailled description
 * The T_RFS_OPENDIR_REQ_MSG message can be used to open a directory. This message is 
 * similar to the rfs_opendir_nb() function. The RFS responds with a 
 * T_RFS_READY_RSP_MSG message, with command index: RFS_OPENDIR_RSP.
 */
/*@{*/

/** Opendir request. */
#define RFS_OPENDIR_REQ_MSG (RFS_MESSAGE_OFFSET | 0x008)

typedef struct {
    T_RV_HDR 		  hdr;
    const T_WCHAR	*pathname;
    T_RFS_DIR     *dirp;
    T_RFS_PAIR_VALUE pair_value;
    T_RV_RETURN		return_path;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
} T_RFS_OPENDIR_REQ_MSG;

/**
 * @name RFS_READDIR_REQ_MSG
 *
 * Detailled description
 * The T_RFS_READDIR_REQ_MSG message can be used to read a directory entry. This 
 * message is similar to the rfs_readdir_nb() function. The RFS responds with a 
 * T_RFS_READY_RSP_MSG mes-sage, with command index: RFS_READDIR_RSP.
 */
/*@{*/

/** Readdir request. */
#define RFS_READDIR_REQ_MSG (RFS_MESSAGE_OFFSET | 0x009)

typedef struct {
    T_RV_HDR 		  hdr;
    T_RFS_DIR 		*dirp;
    T_WCHAR	  	  *buf;
  	T_RFS_SIZE   	size;
    T_RV_RETURN		return_path;
    T_RFS_PAIR_VALUE pair_value;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
} T_RFS_READDIR_REQ_MSG;

/**
 * @name RFS_PREFORMAT_REQ_MSG
 *
 * Detailled description
 * The T_RFS_PREFORMAT_REQ_MSG message can be used to erase all data in RFS. This
 * message is similar to the rfs_preformat_nb() function. The RFS responds with a 
 * T_RFS_READY_RSP_MSG message, with command index: RFS_PREFORMAT_RSP.
 */
/*@{*/

/** Preformat request. */
#define RFS_PREFORMAT_REQ_MSG (RFS_MESSAGE_OFFSET | 0x00A)

typedef struct {
    T_RV_HDR 		  hdr;
    UINT16		    magic;
    T_RV_RETURN		return_path;
    T_RFS_PAIR_VALUE pair_value;
    const T_WCHAR  *pathname;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
} T_RFS_PREFORMAT_REQ_MSG;


/**
 * @name RFS_FORMAT_REQ_MSG
 *
 * Detailled description
 * The T_RFS_FORMAT_REQ_MSG message can be used to format RFS. This message is similar
 * to the rfs_format_nb() function. The RFS responds with a T_RFS_READY_RSP_MSG 
 * message, with command index: RFS_FORMAT_RSP.
 */
/*@{*/

/** Preformat request. */
#define RFS_FORMAT_REQ_MSG (RFS_MESSAGE_OFFSET | 0x00B)

typedef struct {
    T_RV_HDR 		  hdr;
    const T_WCHAR	*name;
    UINT16		    magic;
    T_RV_RETURN		return_path;
    T_RFS_PAIR_VALUE pair_value;
    const T_WCHAR *pathname;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
} T_RFS_FORMAT_REQ_MSG;

#define RFS_GETATTRIB_REQ_MSG (RFS_MESSAGE_OFFSET | 0x00C)

typedef struct {
       T_RV_HDR 		  hdr;
       const T_WCHAR *pathname;
	T_FFS_ATTRIB_TYPE attr;   
	T_RV_RETURN_PATH return_path;
	T_RFS_PAIR_VALUE pair_value;
        T_RVF_MUTEX   *mutex;
       T_RFS_RET     *result;
} T_RFS_GETATTRIB_REQ_MSG;

#define RFS_SETATTRIB_REQ_MSG (RFS_MESSAGE_OFFSET | 0x00D)

typedef struct {
       T_RV_HDR 		  hdr;
       const T_WCHAR *pathname;
	T_FFS_ATTRIB_TYPE attr;   
	T_RV_RETURN_PATH return_path;
	T_RFS_PAIR_VALUE pair_value;
        T_RVF_MUTEX   *mutex;
       T_RFS_RET     *result;
} T_RFS_SETATTRIB_REQ_MSG;


#define RFS_GETDATETIME_REQ_MSG (RFS_MESSAGE_OFFSET | 0x00E)

 typedef struct
  {   
      T_RV_HDR 		  hdr;
	const T_WCHAR *pathname;
	T_RTC_DATE_TIME *date_time_p;
	T_RTC_DATE_TIME *crdate_time_p;	
	T_RV_RETURN_PATH return_path;
	T_RFS_PAIR_VALUE pair_value;
        T_RVF_MUTEX   *mutex;
       T_RFS_RET     *result;
  } T_RFS_GETDATETIME_REQ_MSG;




#define RFS_MOUNT_UNMOUNT_REQ_MSG (RFS_MESSAGE_OFFSET | 0x00F)
#define RFS_UNMOUNT                 (0)
#define RFS_MOUNT					(1)


 typedef struct
  {   
    T_RV_HDR 		  hdr;
	T_RFS_FSTYPE      fs_type;
	int 			  op;
	T_RV_RETURN_PATH return_path;
	T_RFS_PAIR_VALUE pair_value;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
  } T_RFS_MOUNT_UNMOUNT_REQ_MSG;






/**
 * @name RFS_READY_RSP_MSG
 *
 * Detailled description
 * The RFS defines one generic response message T_RFS_READLINK_RSP_MSG. The command
 * index indicates the asynchronous operation, which is finished. The result value 
 * varies for each finished operation.
 */

/*@{*/

/** Generic ready response. */
#define RFS_READY_RSP_MSG (RFS_MESSAGE_OFFSET | 0x010)
#define T_APP_DATA void
typedef struct {
    T_RV_HDR      hdr;
    T_RFS_CMD_ID  command_id;
    T_RFS_RET     result;
    T_RFS_PAIR_VALUE pair_value;
    T_APP_DATA  *app_data_p;
} T_RFS_READY_RSP_MSG;



/**
 * @name FSCORE_READY_RSP_MSG
 *
 * Detailled description
 */

/*@{*/

/** Generic ready response. */
#define FSCORE_READY_RSP_MSG (RFS_MESSAGE_OFFSET | 0x010)





/* These Messages are is described in the rfs_message.h file */


/**
 * @name RFS_SET_LABEL_REQ_MSG
 *
 * Detailled description
 */

/*@{*/

/** Generic ready response. */
#define RFS_VOL_LABEL_MAX 11 //8+3 label entry for FAT 

#define RFS_SET_LABEL_REQ_MSG (RFS_MESSAGE_OFFSET | 0x011)

typedef struct {
    T_RV_HDR      hdr;
    const T_WCHAR	*mpt_name;
    const T_WCHAR	*label;
    T_RV_RETURN		return_path;
    T_RFS_PAIR_VALUE pair_value;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
} T_RFS_SET_LABEL_REQ_MSG;

/**
 * @name RFS_GET_LABEL_REQ_MSG
 *
 * Detailled description
 */

/*@{*/

/** Generic ready response. */
#define RFS_GET_LABEL_REQ_MSG (RFS_MESSAGE_OFFSET | 0x012)

typedef struct {
    T_RV_HDR      hdr;
    const T_WCHAR	*mpt_name;
    T_WCHAR	*label;
    T_RV_RETURN		return_path;
    T_RFS_PAIR_VALUE pair_value;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
} T_RFS_GET_LABEL_REQ_MSG;


/**
 * @name FSCORE_SET_PARTITION_TABLE_RSP_MSG
 *
 * Detailled description
 */

/*@{*/

#define FSCORE_SET_PARTITION_TABLE_RSP_MSG (RFS_MESSAGE_OFFSET | 0x013)


/* Keep track of last used message offset. */
#define RFS_LAST_EXT_MESSAGE FSCORE_SET_PARTITION_TABLE_RSP_MSG



/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* __RFS_MESSAGE_H_ */
