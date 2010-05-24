/**
 * @file  gbi_cfg.h
 *
 * Configuration definitions for the GBI instance.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  12/29/2003   ()   Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __GBI_CFG_H_
#define __GBI_CFG_H_

#include "rv/rv_general.h"		/* General Riviera definitions. */
#include "gbi/gbi_pool_size.h"	/* Stack & Memory Bank sizes definitions */


/* The following constant defines the maximum number of subscribers for GBI events. */
#define GBI_MAX_EVENT_SUBSCRIBERS   4

/* The default number will be 2. This number is configurable to enable the customer to 
 * minimise ram memory usage in the GBI entity. 
 */

/* The following constant defines the maximum length a file system name may occupy 
 * (includes the string terminator).
 */
#define GBI_MAX_FS_NAME   10

/* The default number will be 10. This number is configurable to enable the customer 
 * to minimise ram memory usage in the GBI entity. 
 */

/* The following constant defines the maximum length a partition name may occupy 
 * (includes the string terminator).
 */
#define GBI_MAX_PARTITION_NAME    10

/* The default number will be 10. This number is configurable to enable the customer 
 * to minimise ram memory usage in the GBI entity. 
 */

/* The partition names used in the UFS system find their base in the media names as 
 * specified here in the GBI entity. The following macro defines the media names.
 */

#define GBI_MEDIA_NAMES     \
  "MMC",    \
  "SD",   \
  "MS",   \
  "CF",   \
  "SM",   \
  "INAND",  \
  "INOR"  
/* The sequence and number of partition names must be according to the list in 
 * T_GBI_MEDIA_TYPE.
 */

/* The following macro defines the file system names. */
#define GBI_FS_NAMES    \
  {GBI_EMPTY,     "--"},    \
  {GBI_FAT12,     "FAT12"}, \
  {GBI_FAT16_A,   "FAT16_A"}, \
  {GBI_EXT_DOS,   "EXT_DOS"}, \
  {GBI_FAT16_B,   "FAT16_B"}, \
  {GBI_NTFS,    "NTSF"},  \
  {GBI_FAT32,     "FAT32"}, \
  {GBI_FAT32_LBA,   "FAT32_LBA"}, \
  {GBI_FAT16_LBA,   "FAT16_LBA"}, \
  {GBI_JFS,     "JFS"}, \
  {GBI_TINAFFS,   "TINAFFS"}, \
  {GBI_TINOFFS,   "TINOFFS"}, \
  {GBI_EXT2,    "EXT2"}, \
  {GBI_UNKNOWN,   "UNKNOWN"}

/* The names (including the terminating zero) may not be longer then GBI_MAX_FS_NAME.
 * The last entry must be GBI_UNKNOWN. No duplicate file system numbers are used.
 */

/* This defintion defines the number of file system names defined within the GBI macro table.
 * This definition includes also GBI_UNKNOWN. Whenever a new file system name is newly
 * introduced, this number should be adapted as well
 */
#define GBI_NMB_OF_FS_NAMES   14

/**
 * The following constant defines the number of medias.
 */
#define GBI_SUBSCRIBER_ID_MASK            0xFF00
#define GBI_MEDIA_MASK                    0x00F0
#define GBI_PARTITION_MASK                0x000F

#define GBI_MAX_NR_OF_MEDIA               0x03
#define GBI_MAX_NR_OF_PARTITIONS          0x05

/**
 * Software version 
 */
#define GBI_MAJOR  1
#define GBI_MINOR  1
#define GBI_BUILD  0


/**
 * @name Mem bank
 *
 * Memory bank size and watermark.
 * During development, it is better to put the value here (when the
 * test are activated).
 * At integration, put the value in rvf_pool_size.h
 */
/*@{*/
#define GBI_MB_PRIM_SIZE                GBI_MB1_SIZE
#define GBI_MB_PRIM_WATERMARK           (GBI_MB_PRIM_SIZE - 128)
/*@}*/

/** 
 * Wished priority of the host task for the SWE.
 *
 * During development, put the hardcoded value here.
 * After integration, the value should be in rvm_priorities.h
 */
#ifdef RVM_GBI_TASK_PRIORITY
#define GBI_TASK_PRIORITY               RVM_GBI_TASK_PRIORITY 
#else
#define GBI_TASK_PRIORITY               80  
#endif

#endif /* __GBI_CFG_H_ */
