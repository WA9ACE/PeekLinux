/**
 * @file	rfs_message_i.h
 *
 * Internal RFS messages. They are are not available out of the SWE.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	02/02/2004	 ()		Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __RFS_MESSAGE_I_H_
#define __RFS_MESSAGE_I_H_

#include "rfs/rfs_message.h"

/** Control message */
/*#define RFS_CNTL_REQ_MSG (RFS_MESSAGE_OFFSET | (RFS_LAST_EXT_MESSAGE+0x0008))

typedef struct {
    T_RV_HDR      hdr;
    T_RFS_FD      fd;
    INT8          cmd;
    void          *arg;
    T_RV_RETURN   return_path;
} T_RFS_CNTL_REQ_MSG;
*/
/** Close request. */
#define RFS_CLOSE_REQ_MSG (RFS_MESSAGE_OFFSET | (RFS_LAST_EXT_MESSAGE+0x0001))

typedef struct {
    T_RV_HDR      hdr;
    T_RFS_FD      fd;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
    T_RV_RETURN   return_path;
    T_RFS_PAIR_VALUE pair_value;
} T_RFS_CLOSE_REQ_MSG;

/** Write request. */
#define RFS_WRITE_REQ_MSG (RFS_MESSAGE_OFFSET | (RFS_LAST_EXT_MESSAGE+0x0002))

typedef struct {
    T_RV_HDR      hdr;
    T_RFS_FD      fd;
    const void    *buf;
    T_RFS_SIZE    size;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
    T_RV_RETURN   return_path;
    T_RFS_PAIR_VALUE pair_value;
} T_RFS_WRITE_REQ_MSG;

/** Read request. */
#define RFS_READ_REQ_MSG (RFS_MESSAGE_OFFSET | (RFS_LAST_EXT_MESSAGE+0x0003))

typedef struct {
    T_RV_HDR      hdr;
    T_RFS_FD      fd;
    void    *buf;
    T_RFS_SIZE    size;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
    T_RV_RETURN   return_path;
    T_RFS_PAIR_VALUE pair_value;
} T_RFS_READ_REQ_MSG;

/** Lseek request. */
#define RFS_LSEEK_REQ_MSG (RFS_MESSAGE_OFFSET | (RFS_LAST_EXT_MESSAGE+0x0004))

#define T_RFS_WHENCE T_FSCORE_WHENCE
typedef struct {
    T_RV_HDR      hdr;
    T_RFS_FD      fd;
    T_RFS_OFFSET  offset;
    T_RFS_WHENCE  whence;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
    T_RV_RETURN   return_path;
    T_RFS_PAIR_VALUE pair_value;
} T_RFS_LSEEK_REQ_MSG;

/** Fchmod request. */
#define RFS_FCHMOD_REQ_MSG (RFS_MESSAGE_OFFSET | (RFS_LAST_EXT_MESSAGE+0x0005))

typedef struct {
    T_RV_HDR      hdr;
    T_RFS_FD      fd;
    T_RFS_MODE    mode;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
    T_RV_RETURN   return_path;
    T_RFS_PAIR_VALUE pair_value;
} T_RFS_FCHMOD_REQ_MSG;

/** Fstat request. */
#define RFS_FSTAT_REQ_MSG (RFS_MESSAGE_OFFSET | (RFS_LAST_EXT_MESSAGE+0x0006))

typedef struct {
    T_RV_HDR      hdr;
    T_RFS_FD      fd;
    T_RFS_STAT    *stat;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
    T_RV_RETURN   return_path;
    T_RFS_PAIR_VALUE pair_value;
} T_RFS_FSTAT_REQ_MSG;


/** Fsync request. */
#define RFS_FSYNC_REQ_MSG (RFS_MESSAGE_OFFSET | (RFS_LAST_EXT_MESSAGE+0x0007))

typedef struct {
    T_RV_HDR      hdr;
    T_RFS_FD      fd;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
    T_RV_RETURN   return_path;
    T_RFS_PAIR_VALUE pair_value;
} T_RFS_FSYNC_REQ_MSG;


#define RFS_CLOSEDIR_REQ_MSG (RFS_MESSAGE_OFFSET | (RFS_LAST_EXT_MESSAGE+0x0008))

typedef struct {
    T_RV_HDR      hdr;
    T_RFS_DIR     *dirp;
    T_RVF_MUTEX   *mutex;
    T_RFS_RET     *result;
	T_RV_RETURN   return_path;
    T_RFS_PAIR_VALUE pair_value;
} T_RFS_CLOSEDIR_REQ_MSG;



/** This message is send to the RFS mailbox at startup */
#define RFS_I_STARTUP_MSG (RFS_MESSAGE_OFFSET | (RFS_LAST_EXT_MESSAGE+0x0009))

typedef struct {
    T_RV_HDR 		    hdr;
} T_RFS_I_STARTUP_MSG;


/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* __RFS_MESSAGE_H_ */
