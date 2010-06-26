/**
 * @file	rfs_cfg.h
 *
 * Configuration definitions for the RFS instance.
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

#ifndef __RFS_CFG_H_
#define __RFS_CFG_H_


#include "rv/rv_general.h"		/* General Riviera definitions. */
#include "rfs/rfs_pool_size.h"	/* Stack & Memory Bank sizes definitions */

/**
 * Software version 
 */
#define RFS_MAJOR  0
#define RFS_MINOR  1
#define RFS_BUILD  0

/**
 * Sample value.
 */
#define RFS_SAMPLE_VALUE	            10


/**
 * @name Mem bank
 *
  * Memory bank size and watermark.
 * During development, it is better to put the value here (when the
 * test are activated).
 * At integration, put the value in rvf_pool_size.h
*/
/*@{*/
#define RFS_MB_PRIM_SIZE                (RFS_MB1_SIZE)
#define RFS_MB_PRIM_WATERMARK           (RFS_MB_PRIM_SIZE - 128)
/*@}*/


/** 
 * Size of task stack in bytes.
 *
 * During development, put the hardcoded value here.
 * After integration, the value should be in rvf_pool_size.h
 */
/* 
#ifdef RVF_RFS_STACK_SIZE
#define RFS_STACK_SIZE                  RVF_RFS_STACK_SIZE	
#else
#define RFS_STACK_SIZE                  2048	
#endif
*/

/**
 * @name Mem bank
 *
 * Memory bank size and watermark.
 * During development, it is better to put the value here (when the
 * test are activated).
 * At integration, put the value in rvf_pool_size.h
 */
/*@{*/
/*
#ifdef RVF_RFS_MB1_SIZE
#define RFS_MB_PRIM_SIZE                RVF_RFS_MB1_SIZE
#else
#define RFS_MB_PRIM_SIZE                2048	
#endif

#define RFS_MB_PRIM_WATERMARK           (RFS_MB_PRIM_SIZE - 128)
*/
/*@}*/

/** 
 * Wished priority of the host task for the SWE.
 *
 * During development, put the hardcoded value here.
 * After integration, the value should be in rvm_priorities.h
 */
#ifdef RVM_RFS_TASK_PRIORITY
#define RFS_TASK_PRIORITY               RVM_RFS_TASK_PRIORITY	
#else
#define RFS_TASK_PRIORITY               103	
#endif

/* 
 * RFS API: Configuration Items 
 */
#define RFS_FILENAME_MAX                255
#define RFS_PATHNAME_MAX                255

#define RFS_MAX_NR_OPEN_FILES           10


#include "gbi/gbi_api.h"
#include "rfsfat/rfsfat_api.h"
#include "rfsnand/rfsnand_api.h"
#include "chipset.cfg"
#include "swconfig.cfg"

#if ((RELIANCE_FS == 1) || (RELIANCE_FS == 2))
#include "ffs/reliance/relfs.h"
#endif

#if(CHIPSET != 15) 

#define RFS_NMB_FSCORE	2

#else

#if ((RELIANCE_FS == 1) || (RELIANCE_FS == 2))
#define RFS_NMB_FSCORE	2
#else
#define RFS_NMB_FSCORE	1
#endif

#endif

/* The list of file systems a core supports: core number 0 supports FAT16 and FAT32
 * RFS search the filesystem types (returned by GBI) in this list.
 * The matching core number is used to access the correct function table.
 */
#if(CHIPSET != 15) 
#define RFS_FSCORES		\
		{0, GBI_FAT16_LBA}, 	\
		{0, GBI_FAT32_LBA}, 	\
		{1, GBI_TINAFFS}

#else
#if ((RELIANCE_FS == 1) || (RELIANCE_FS == 2))

#define RFS_RELIANCE_NUM    0
#define RFS_FAT_NUM 		1

#define RFS_FSCORES		\
		{RFS_RELIANCE_NUM, GBI_RELIANCE }, \
                {RFS_FAT_NUM, GBI_FAT16_LBA}, \
                {RFS_FAT_NUM, GBI_UNKNOWN}, \
                {RFS_FAT_NUM, GBI_FAT32_LBA}, \
                {RFS_FAT_NUM, GBI_FAT12 }	

#else
#define RFS_FAT_NUM 		0
#define RFS_FSCORES		\
                {RFS_FAT_NUM, GBI_FAT16_LBA}, \
                {RFS_FAT_NUM, GBI_UNKNOWN}, \
                {RFS_FAT_NUM, GBI_FAT32_LBA}, \
                {RFS_FAT_NUM, GBI_FAT12 }	
#endif
#endif

#if(CHIPSET != 15) 

#define RFS_NMB_OF_FSCORE_ITEMS 3

#else

#if ((RELIANCE_FS == 1) || (RELIANCE_FS == 2))
#define RFS_NMB_OF_FSCORE_ITEMS 5
#else
#define RFS_NMB_OF_FSCORE_ITEMS 4

#endif
#endif    /* #if (CHIPSET == 15)*/


typedef struct{
	UINT8			     rfs_fscore_nmb;
	T_GBI_FS_TYPE	 rfs_fscore_type;
} T_RFS_FSCORE_LIST;

#if(CHIPSET != 15) 

/* core API functions to get the function table from (sequence of core numbers) */
#define	RFS_FSCORE_FUNC_TABLE	\
		rfsfat_get_func_table,		\
		rfsnand_get_func_table


#else

#if ((RELIANCE_FS == 1) || (RELIANCE_FS == 2))
#define	RFS_FSCORE_FUNC_TABLE	\
	    relfs_get_func_table, 	\
		rfsfat_get_func_table
 

#else
#define	RFS_FSCORE_FUNC_TABLE	\
		rfsfat_get_func_table

#endif


#endif

#endif /* __RFS_CFG_H_ */
