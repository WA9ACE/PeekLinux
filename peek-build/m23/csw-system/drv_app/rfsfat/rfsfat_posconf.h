/**
 * @file	rfsfat_posconf.h
 *
 * This file contains function declarations of rfsfat_posconf.c
 *
 * @author	Anton van Breemen
 * @version 0.1 
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	03/15/2004	Anton van Breemen		Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */


#ifndef RFSFAT_POSCONF_H
#define RFSFAT_POSCONF_H

#include "rv/rv_general.h"

#define	RFSFAT_SEPARATOR	'/'
#define KBYTES						1024
#define RFSFAT_UNTOUCHED  0

/******************************************************************************
 *     Function prototypes                                                    *
 ******************************************************************************/
extern T_FSCORE_RET rfsfat_ffs_file_open (T_RV_HDR * msg_p);
extern T_FSCORE_RET rfsfat_ffs_file_read (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_file_write (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_file_close (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_file_lseek (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_file_fchmod (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_file_fstat (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_file_fsync (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_file_remove (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_fman_chmod (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_stat (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_fman_rename (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_preformat (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_dman_mkdir (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_dman_rmdir (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_dman_opendir (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_dman_readdir (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_format (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_file_trunc (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_file_trunc_name (T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_file_read_by_name(T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_file_write_by_name(T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_vol_set_label(T_RV_HDR * inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_vol_get_label(T_RV_HDR * inmsg_p);
time_t converttime (UINT16 Time, UINT16 Date);

#if (FFS_UNICODE == 1)
/**************************************************/
// Unicode FFS       
/**************************************************/
#if 0
extern T_FSCORE_RET  rfsfat_ffs_fman_chmod_uc(void *inmsg_p);
#endif

extern T_FSCORE_RET  rfsfat_ffs_format_uc(void *inmsg_p);
extern T_FSCORE_RET  rfsfat_ffs_dman_mkdir_uc(void *inmsg_p);
extern T_FSCORE_RET  rfsfat_ffs_dman_opendir_uc (void * inmsg_p);
extern T_FSCORE_RET  rfsfat_ffs_dman_readdir_uc (void * inmsg_p);
extern T_FSCORE_RET  rfsfat_ffs_file_open_uc(void *inmsg_p);
extern T_FSCORE_RET  rfsfat_ffs_preformat_uc(void *inmsg_p);
extern T_FSCORE_RET  rfsfat_ffs_file_remove_uc(void *inmsg_p);
extern T_FSCORE_RET  rfsfat_ffs_fman_rename_uc(void *inmsg_p);
extern T_FSCORE_RET  rfsfat_ffs_dman_rmdir_uc(void *inmsg_p);
extern T_FSCORE_RET  rfsfat_ffs_stat_uc(void *inmsg_p);
extern T_FSCORE_RET rfsfat_ffs_file_trunc_name_uc (void * inmsg_p);


/**************************************************/
// end of Unicode FFS       
/**************************************************/
#endif


T_FSCORE_RET  rfsfat_ffs_getattribute(void *inmsg_p);
T_FSCORE_RET  rfsfat_ffs_setattribute(void *inmsg_p);
T_FSCORE_RET  rfsfat_ffs_getdatetime(void *inmsg_p);

#if (FFS_UNICODE == 1)

T_FSCORE_RET  rfsfat_ffs_getattribute_uc(void *inmsg_p);
T_FSCORE_RET  rfsfat_ffs_setattribute_uc(void *inmsg_p);
T_FSCORE_RET  rfsfat_ffs_getdatetime_uc(void *inmsg_p);
T_FSCORE_RET  rfsfat_ffs_fman_chmod_uc(void *inmsg_p);

#endif

T_FSCORE_RET  rfsfat_ffs_mount_unmount (void *inmsg_p);


UINT8 *string_toupper (UINT8 * const string);
UINT8 RFSFAT_get_mpt_idx(const UINT8* pucPath, UINT16 *index_p);

#endif
