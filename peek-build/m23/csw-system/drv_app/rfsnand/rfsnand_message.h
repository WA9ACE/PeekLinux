/**
 * @file  rfsnand_message.h
 *
 * Data structures:
 * 1) used to send messages to the RFSNAND SWE,
 * 2) RFSNAND can receive.
 *
 * @author  D. Meijer (dolf.meijer@ict.nl)
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author                         Modification
 *  -------------------------------------------------------------------
 *  2/23/2004   D. Meijer (dolf.meijer@ict.nl) Create.
 *
 * (C) Copyright 2004 by Texas Instruments Incorporated, All Rights Reserved
 */
#ifndef __RFSNAND_MESSAGE_H_
#define __RFSNAND_MESSAGE_H_

#include "rv/rv_general.h"
#include "rfs/fscore_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * The message offset must differ for each SWE in order to have 
 * unique msg_id in the system.
 */
#define RFSNAND_MESSAGE_OFFSET   BUILD_MESSAGE_OFFSET(RFSNAND_USE_ID)

/**
 *  These fields are common for all messages.
 */
#define RFSNAND_MESSAGE_COMMON_FIELDS   T_RV_HDR hdr;\
                                        T_RV_RETURN return_path;\
                                        T_FSCORE_PAIR_VALUE pair_value;

/**
 * @name RFSNAND_OPEN_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_OPEN_REQ_MSG message can be used to open a file. This message is similar
 * to the rfsnand_api_open() function. Response: T_FSCORE_READY_RSP_MSG message,
 * with command index: RFSNAND_OPEN_RSP.
 */
/*@{*/

/** Open request. */
#define RFSNAND_OPEN_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x001)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  const char     *path; // [RFSNAND_MAX_PATH_LENGTH+1]
  T_FSCORE_FLAGS flags;
  T_FSCORE_MODE  mode; // only used when O_CREAT flag is set and file not exists
  UINT32         uid; // user ID
  UINT32         gid; // group ID
} T_RFSNAND_OPEN_REQ_MSG;

/**
 * @name RFSNAND_CHMOD_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_CHMODE_REQ_MSG message can be used to change the permission attributes 
 * of a file or directory. This message is similar to the rfsnand_api_chmod() function. 
 * Response: T_FSCORE_READY_RSP_MSG message, with command index: 
 * RFSNAND_CHMOD_RSP.
 */
/*@{*/

/** Chmod request. */
#define RFSNAND_CHMOD_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x002)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  const char   *path; // [RFSNAND_MAX_PATH_LENGTH+1]
  T_FSCORE_MODE mode;
} T_RFSNAND_CHMOD_REQ_MSG;

/**
 * @name RFSNAND_STAT_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_STAT_REQ_MSG message can be used to obtain information about the file, 
 * directory or device. This message is similar to the rfsnand_api_stat() function. The 
 * RFS responds with a T_FSCORE_READY_RSP_MSG message, with command index: RFSNAND_STAT_RSP.
 */
/*@{*/

/** Stat request. */
#define RFSNAND_STAT_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x003)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  const char    *path; // [RFSNAND_MAX_PATH_LENGTH+1]
  T_FSCORE_STAT *pStat; // allocated by client!
} T_RFSNAND_STAT_REQ_MSG;

/**
 * @name RFSNAND_REMOVE_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_REMOVE_REQ_MSG message can be used to remove a file or directory. This 
 * message is similar to the rfsnand_api_remove() function. Response: 
 * T_FSCORE_READY_RSP_MSG message, with command index: RFSNAND_REMOVE_RSP.
 */
/*@{*/

/** Remove request. */
#define RFSNAND_REMOVE_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x004)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  const char *path; // [RFSNAND_MAX_PATH_LENGTH+1]
} T_RFSNAND_REMOVE_REQ_MSG;


/**
 * @name RFSNAND_RENAME_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_RENAME_REQ_MSG message can be used to rename a file or directory. 
 * This message is similar to the rfsnand_api_rename() function. Response:
 * T_FSCORE_READY_RSP_MSG message, with command index: RFSNAND_RENAME_RSP.
 */
/*@{*/

/** Rename request. */
#define RFSNAND_RENAME_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x005)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  const char *oldname;// [RFSNAND_MAX_PATH_LENGTH+1];
  const char *newname;// [RFSNAND_MAX_PATH_LENGTH+1];
} T_RFSNAND_RENAME_REQ_MSG;

/**
 * @name RFSNAND_MKDIR_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_MKDIR_REQ_MSG message can be used to create a directory. This message is 
 * similar to the rfsnand_api_mkdir() function. Response: T_FSCORE_READY_RSP_MSG 
 * message, with command index: RFSNAND_MKDIR_RSP.
 */
/*@{*/

/** Mkdir request. */
#define RFSNAND_MKDIR_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x006)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  const char   *path; // [RFSNAND_MAX_PATH_LENGTH+1]
  T_FSCORE_MODE mode;
  // UINT32         uid; // user ID
  // UINT32         gid; // group ID
} T_RFSNAND_MKDIR_REQ_MSG;

/**
 * @name RFSNAND_RMDIR_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_RMDIR_REQ_MSG message can be used to remove a directory. This message is
 * similar to the rfsnand_api_rmdir() function. Response: T_FSCORE_READY_RSP_MSG
 * message, with command index: RFSNAND_RMDIR_RSP.
 */
/*@{*/

/** Rmdir request. */
#define RFSNAND_RMDIR_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x007)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  const char  *path; // [RFSNAND_MAX_PATH_LENGTH+1]
} T_RFSNAND_RMDIR_REQ_MSG;

/**
 * @name RFSNAND_CHDIR_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_CHDIR_REQ_MSG message can be used to change the current directory. This
 * message is similar to the rfsnand_api_chdir () function. Response: 
 * T_FSCORE_READY_RSP_MSG message, with command index: RFSNAND_CHDIR_RSP.
 */
/*@{*/

/** Chdir request. */
#define RFSNAND_CHDIR_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x008)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  char    *path; // [RFSNAND_MAX_PATH_LENGTH+1]
} T_RFSNAND_CHDIR_REQ_MSG;

/**
 * @name RFSNAND_OPENDIR_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_OPENDIR_REQ_MSG message can be used to open a directory. This message is 
 * similar to the rfsnand_api_opendir() function. Response: 
 * T_FSCORE_READY_RSP_MSG message, with command index: RFSNAND_OPENDIR_RSP. */
/*@{*/

/** Opendir request. */
#define RFSNAND_OPENDIR_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x009)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  const char    *path; // [RFSNAND_MAX_PATH_LENGTH+1];
  T_FSCORE_DIR  *pDir;
} T_RFSNAND_OPENDIR_REQ_MSG;

/**
 * @name RFSNAND_READDIR_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_READDIR_REQ_MSG message can be used to read a directory entry. This 
 * message is similar to the rfsnand_api_readdir() function. Response: 
 * T_FSCORE_READY_RSP_MSG mes-sage, with command index: RFSNAND_READDIR_RSP.*/
/*@{*/

/** Readdir request. */
#define RFSNAND_READDIR_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x00A)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  T_FSCORE_DIR    *pDir;
  char            *buffer;
  T_FSCORE_SIZE    size;
} T_RFSNAND_READDIR_REQ_MSG;

/**
 * @name RFSNAND_PREFORMAT_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_PREFORMAT_REQ_MSG message can be used to erase all data in RFS. This
 * message is similar to the rfsnand_api_preformat() function. Response: 
 * T_FSCORE_READY_RSP_MSG message, with command index: RFSNAND_PREFORMAT_RSP.*/
/*@{*/

/** Preformat request. */
#define RFSNAND_PREFORMAT_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x00B)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  UINT16        magic; // must be 0xDEAD
  const char   *path; // [RFSNAND_MAX_PATH_LENGTH+1];
} T_RFSNAND_PREFORMAT_REQ_MSG;


/**
 * @name RFSNAND_FORMAT_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_FORMAT_REQ_MSG message can be used to format RFS. This message is similar
 * to the rfsnand_api_format() function. Response: T_FSCORE_READY_RSP_MSG 
 * message, with command index: RFSNAND_FORMAT_RSP. */
/*@{*/

/** Format request. */
#define RFSNAND_FORMAT_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x00C)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  const char    *path; // [RFSNAND_MAX_PATH_LENGTH+1];
  const char    *name; // [RFSNAND_MAX_NAME_LENGTH+1];
  UINT16        magic; // must be 0x2BAD
} T_RFSNAND_FORMAT_REQ_MSG;

/**
 * @name RFSNAND_CLOSE_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_CLOSE_REQ_MSG message can be used to close a file. 
 * Response: T_FSCORE_READY_RSP_MSG with command index: RFSNAND_CLOSE_RSP. */
/*@{*/

/** Close request. */
#define RFSNAND_CLOSE_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x000D)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  T_FSCORE_FD   fd; // file descriptor
} T_RFSNAND_CLOSE_REQ_MSG;

/**
 * @name RFSNAND_WRITE_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_WRITE_REQ_MSG message can be used to format a partition. 
 * Response: T_FSCORE_READY_RSP_MSG with command index: RFSNAND_WRITE_RSP.*/
/*@{*/

/** Write request. */
#define RFSNAND_WRITE_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x000E)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  T_FSCORE_FD   fd; // file descriptor
  const void   *buffer; // allocated by client!
  T_FSCORE_SIZE size;
} T_RFSNAND_WRITE_REQ_MSG;

/**
 * @name RFSNAND_READ_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_READ_REQ_MSG message can be used to read. 
 * Response: T_FSCORE_READY_RSP_MSG with command index: RFSNAND_READ_RSP.*/
/*@{*/

/** Preformat request. */
#define RFSNAND_READ_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x000F)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  T_FSCORE_FD   fd; // file descriptor
  const void    *buffer; // allocated by client!
  T_FSCORE_SIZE size;
} T_RFSNAND_READ_REQ_MSG;

/**
 * @name RFSNAND_LSEEK_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_LSEEK_REQ_MSG message can be used to seek in a file. 
 * Response: T_FSCORE_READY_RSP_MSG with command index: RFSNAND_LSEEK_RSP.*/
/*@{*/

/** Lseek request. */
#define RFSNAND_LSEEK_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x0010)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  T_FSCORE_FD      fd; // file descriptor
  T_FSCORE_OFFSET  offset;
  INT8             whence;
} T_RFSNAND_LSEEK_REQ_MSG;

/**
 * @name RFSNAND_FCHMOD_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_FCHMOD_REQ_MSG message can be used to change the mode of a file. 
 * Response: T_FSCORE_READY_RSP_MSG with command index: RFSNAND_FCHMOD_RSP.*/
/*@{*/

/** Fchmod request. */
#define RFSNAND_FCHMOD_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x0011)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  T_FSCORE_FD      fd; // file descriptor
  T_FSCORE_MODE    mode;
} T_RFSNAND_FCHMOD_REQ_MSG;

/**
 * @name RFSNAND_FSTAT_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_FSTAT_REQ_MSG message can be used to fstat a file. 
 * Response: T_FSCORE_READY_RSP_MSG with command index: RFSNAND_FSTAT_RSP.*/
/*@{*/

/** Preformat request. */
#define RFSNAND_FSTAT_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x0012)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  T_FSCORE_FD      fd; // file descriptor
  T_FSCORE_STAT   *pStat;
} T_RFSNAND_FSTAT_REQ_MSG;

/**
 * @name RFSNAND_FSYNC_REQ_MSG
 *
 * Detailled description
 * The T_RFSNAND_FSYNC_REQ_MSG message can be used to sync a file. 
 * Response: T_FSCORE_READY_RSP_MSG with command index: RFSNAND_FSYNC_RSP.*/
/*@{*/

/** Preformat request. */
#define RFSNAND_FSYNC_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x0013)

typedef struct 
{
  RFSNAND_MESSAGE_COMMON_FIELDS
  T_FSCORE_FD      fd; // file descriptor
} T_RFSNAND_FSYNC_REQ_MSG;


#define RFSNAND_SET_PARTITION_TABLE_REQ_MSG (RFSNAND_MESSAGE_OFFSET | 0x0014)

typedef struct 
{
  T_RV_HDR                  hdr;
  T_FSCORE_PARTITION_TABLE  *partition_table;
  T_RV_RETURN               return_path;
} T_RFSNAND_SET_PARTITION_TABLE_REQ_MSG;
// response is RFSNAND_SET_PARTITION_TABLE_RSP_MSG

#define RFSNAND_SET_PARTITION_TABLE_RSP_MSG (RFSNAND_MESSAGE_OFFSET | 0x0015)

/* Keep track of last used message offset. */
#define RFSNAND_LAST_EXT_MESSAGE 0x0015

/**
 * @name RFSNAND_READY_RSP_MSG
 *
 * Detailled description
 * The RFS defines one generic response message T_RFSNAND_READLINK_RSP_MSG. The command
 * index indicates the asynchronous operation, which is finished. The result value 
 * varies for each finished operation.
 */

/*@{*/

/** Generic ready response. */
#define RFSNAND_READY_RSP_MSG (RFSNAND_MESSAGE_OFFSET | (RFSNAND_LAST_EXT_MESSAGE + 1))

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* __RFSNAND_MESSAGE_H_ */
