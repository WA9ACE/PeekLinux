/**
 * @file  gbi_i.h
 *
 * Internal definitions for GBI.
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

#ifndef __GBI_I_H_
#define __GBI_I_H_


#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"
#include "rvm/rvm_gen.h"    /* Generic RVM types and functions. */

#include "gbi/gbi_api.h"
#include "gbi/gbi_message.h"
#include "gbi/gbi_message_i.h"
#include "gbi/gbi_state_i.h"
#include "gbi/gbi_pi_cfg.h"


/** Macro used for tracing GBI messages. */

#define GBI_SEND_TRACE(string, trace_level) 

/** Macro used for tracing GBI messages with a parameter. */
#define GBI_SEND_TRACE_PARAM(text, param, level)


//#define GBI_SEND_TRACE(string, trace_level) rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, GBI_USE_ID)


/** Macro used for tracing GBI messages with a parameter. */

//#define GBI_SEND_TRACE_PARAM(text, param, level)  rvf_send_trace(text, sizeof(text)-1, param, level, GBI_USE_ID )

/**  Macro used for generating asserts */
#define GBI_ASSERT_PREFIX "GBI Assertion failed: "
#define GBI_ASSERT(expr) do { if(!(expr)) \
        rvf_send_trace(GBI_ASSERT_PREFIX #expr " is FALSE in " __FILE__ " line", \
                sizeof(GBI_ASSERT_PREFIX #expr " is FALSE in " __FILE__ " line")-1, \
                __LINE__, RV_TRACE_LEVEL_ERROR, GBI_USE_ID); } while(0)


/**  Macro used for generating asserts, based on 2 parameters 
 *   For example: XXX_ASSERT2(buf_p != NULL, return RV_INVALID_PARAMETER); 
 */
#define GBI_ASSERT2(expr, fail_code) do {if(!(expr)) { \
        rvf_send_trace(GBI_ASSERT_PREFIX #expr " is FALSE in " __FILE__ " line", \
                sizeof(GBI_ASSERT_PREFIX #expr " is FALSE in " __FILE__ " line")-1, \
                __LINE__, RV_TRACE_LEVEL_ERROR, GBI_USE_ID); fail_code;}} while(0)


#define NAND_SPEED_UP_READ
#define NAND_SPEED_UP_PROGRAM
/**
 *This structure gather general informations about subscriber 
 */
 #if 0
typedef struct 
{ 
  T_RV_RETURN ret_path;           /* Return path */
  UINT8       subscriber_id;      /* Unique subscriber id */

} T_SUBSCRIBER_INFO;
#endif
/**
 *This structure gather general informations about subscriber 
 */
typedef struct 
{ 
  T_RV_RETURN ret_path;           /* Return path */

} T_SUBSCRIBE_EVENTS;


/* This structure defines the different types of timers.*/
typedef enum{
  GBI_TIMER_ONCE = 0,
  GBI_TIMER_INTERVAL    
} T_GBI_TIMER_REQ_TYPE;

/* This type defines the GBI time */
typedef UINT32 T_GBI_TIME_T;

typedef struct 
{ 
  UINT8   fs_name_type;
  char    *fs_name; 
} T_GBI_FS_NAMES;


/* Defintion of message is not in progress */
#define GBI_NO_MESSAGE_IN_PROGRESS  0xFFFFFFFF

/* Defintion of plugin is not in progress */
#define GBI_NO_PLUGIN_IN_PROGRESS   0xFF

/* Defintion of mb_id, which is used or valid */
#define GBI_NO_MB_ID                0xFFFF

/* Subscribe events definitions */
#define GBI_MAX_NR_OF_EVENTS                  3  // GBI_EVENT_NAN_MEDIA_AVAILABLE event is added

#define GBI_EVENT_MASK_0                      0x0001
#define GBI_EVENT_MASK_1                      0x0002
#define GBI_EVENT_MASK_2                      0x0004

#define GBI_EVENT_0                           0x00
#define GBI_EVENT_1                           0x01
#define GBI_EVENT_2                           0x02


typedef enum{
  GBI_STATE_READY = 0,
  GBI_STATE_PROCESSING,
  GBI_STATE_BUSY
} T_GBI_STATUS;

typedef struct 
{ 
  /* Status of the plugin: READY, PROCESSING, BUSY, etc */ 
  T_GBI_STATUS  plugin_state;

  /* When a plugin processes a message, the message id is stored, 
   * 0xFFFFFFFF means no message is in progress 
   */
  UINT32        msg_id_in_progress;

  /* Plugin has already synchronised it's media and partition information to GBI 
   * When the media is removed and again inserted, this flag should be reset to FALSE
   * FALSE means not synchronised, TRUE means synchronised 
   */
  BOOL          plugin_info_sync;


  /* Plugin store the Media and partition info requests data in it's own memory.
   * Each plugin stores the pointer to this data, until the GBI has gathered all
   * the information (all plugins ready with the request) and creatd a response.
   */
  void *media_data_p[GBI_MAX_NR_OF_MEDIA];
  void *nmb_media_p;
  void *partition_data_p[GBI_MAX_NR_OF_PARTITIONS];
  void *nmb_partition_p;

}T_GBI_PLUGIN_INFO;


/**
 * The Control Block buffer of GBI, which gathers all 'Global variables'
 * used by GBI instance.
 *
 * A structure should gathers all the 'global variables' of GBI instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_GBI_ENV_CTRL_BLK buffer is allocated when creating GBI instance and is 
 * then always refered by GBI instance when access to 'global variable' 
 * is necessary.
 */
typedef struct
{
  /** Store the current state of the GBI instance */
  T_GBI_INTERNAL_STATE state;

  /** Pointer to the error function */
  T_RVM_RETURN (*error_ft)(T_RVM_NAME swe_name, 
               T_RVM_RETURN error_cause,
               T_RVM_ERROR_TYPE error_type,
               T_RVM_STRING error_msg);
  /** Mem bank id. */
  T_RVF_MB_ID prim_mb_id;

  T_RVF_ADDR_ID addr_id;

  /* Plugin information */
  T_GBI_PLUGIN_INFO   plugin_table[GBI_NMB_PLUGINS];  

  UINT8               plugin_nmb_in_progress;
  
  UINT32              multiple_msg_in_progress;
  T_RV_RETURN         multiple_msg_ret_path;
  T_RVF_MB_ID         multiple_msg_mb_id;

  /* Subcriber information for each partition of each media */
  T_SUBSCRIBE_EVENTS     sub_events[GBI_MAX_NR_OF_EVENTS][GBI_MAX_EVENT_SUBSCRIBERS]; 


  /*********************************************************/
  /* Next variables are part of the GBI PC-simulation      */
  /* Should be part of it's own environment                */
  /*********************************************************/
  
  /* Media and partition information storage */  
  T_GBI_MEDIA_INFO       media_table[GBI_MAX_NR_OF_MEDIA];
  T_GBI_PARTITION_INFO   partition_table[GBI_MAX_NR_OF_PARTITIONS];

  /* Number of media and partition storage */  
  UINT8             number_of_media;                                    
  UINT8             number_of_partitions[GBI_MAX_NR_OF_MEDIA]; 


  /* Simulation definitions: partition memory */
  UINT32 *media_0_partition_0_mem;
  UINT32 *media_0_partition_1_mem;
  UINT32 *media_0_partition_2_mem;
  UINT32 *media_0_partition_3_mem;
} T_GBI_ENV_CTRL_BLK;


/** External ref "global variables" structure. */
extern T_GBI_ENV_CTRL_BLK *gbi_env_ctrl_blk_p;

/* Definition in case index can't be found */
#define INVALID_INDEX   0xFF

/* Definition of data to use during erase function */
#define GBI_ERASE_DATA  0x00



/***********************************************************************
 *        Following definition concerns: simulation definitions        *
 **********************************************************************/

/* Media 0: with 3 partitions, definitions  */

#define GBI_PLUGIN_NMB_0                  0x00

#define GBI_NR_OF_MEDIA                   0x01
#define GBI_MEDIA_SPARE_AREA_NONE         0x00
#define GBI_MEDIA_NR_0                    GBI_PLUGIN_0_MEDIA_OFFSET
#define GBI_MEDIA_0_NR_OF_PARTITIONS      0x04
#define GBI_MEDIA_0_MEDIA_TYPE            GBI_MMC
#define GBI_MEDIA_0_MEDIA_ID              0x123
#define GBI_MEDIA_0_BYTES_PER_BLOCK       0x200    /* 512 bytes */
#define GBI_MEDIA_0_SPARE_AREA_SIZE       6        /* 6 bytes */
#define GBI_MEDIA_0_READ_SPEED            0x400    /* 1024 kbits/sec */
#define GBI_MEDIA_0_WRITE_SPEED           0x400    /* 1024 kbits/sec */
#define GBI_MEDIA_0_WRITEABLE             TRUE

/* Media 0, partition 0, definitions */
#define GBI_MEDIA_0_PARTITION_NR_0        0x00
#define GBI_MEDIA_0_PAR_0_FILESYSTEM      GBI_FAT16_LBA
#define GBI_MEDIA_0_PAR_0_NMB_OF_BLOCKS   0x3000
#define GBI_MEDIA_0_PAR_0_FIRST_BLOCK     0x00
#define GBI_MEDIA_0_PAR_0_LAST_BLOCK      0x2FFF

/* Media 0, partition 1, definitions */
#define GBI_MEDIA_0_PARTITION_NR_1        0x01
#define GBI_MEDIA_0_PAR_1_FILESYSTEM      GBI_FAT12
#define GBI_MEDIA_0_PAR_1_NMB_OF_BLOCKS   0x1000 //~2MB
#define GBI_MEDIA_0_PAR_1_FIRST_BLOCK     0x00
#define GBI_MEDIA_0_PAR_1_LAST_BLOCK      0xFFF

/* Media 0, partition 2, definitions */
#define GBI_MEDIA_0_PARTITION_NR_2        0x02
#define GBI_MEDIA_0_PAR_2_FILESYSTEM      GBI_TINAFFS
#define GBI_MEDIA_0_PAR_2_NMB_OF_BLOCKS   0x100
#define GBI_MEDIA_0_PAR_2_FIRST_BLOCK     0x00
#define GBI_MEDIA_0_PAR_2_LAST_BLOCK      0xFF

/* Media 0, partition 3, definitions */
#define GBI_MEDIA_0_PARTITION_NR_3        0x03
#define GBI_MEDIA_0_PAR_3_FILESYSTEM      GBI_FAT32_LBA
#define GBI_MEDIA_0_PAR_3_NMB_OF_BLOCKS   0x200
#define GBI_MEDIA_0_PAR_3_FIRST_BLOCK     0x00
#define GBI_MEDIA_0_PAR_3_LAST_BLOCK      0x1FF

/* FAT16 contains 4 areas: bootsector, first fat table, second fas table 
 * and root directory. Next definitions concern these areas.
 */

/* 1. Boot sector */
#define BOOTSECTOR_ADDR                      0x0000
#define BOOTSECTOR_SIZE                      0x0200 /* 512 bytes */

#define JUMP_BOOTSECTOR_ADDR                 0x00
#define JUMP_BOOTSECTOR_SIZE                 3      /* bytes */
#define JUMP_BOOTSECTOR_VALUE_1              0xEB
#define JUMP_BOOTSECTOR_VALUE_2              0x3E
#define JUMP_BOOTSECTOR_VALUE_3              0x90

#define MANUFACTURER_ADDR                    0x03
#define MANUFACTURER_SIZE                    8      /* bytes */
#define MANUFACTURER_VALUE                   "MANUFACT"
                                                               
#define BYTES_PER_SECTOR_ADDR                0x0B
#define BYTES_PER_SECTOR_SIZE                2      /* bytes */
#define BYTES_PER_SECTOR_VALUE_1             0x00   /* 512 bytes, LSB */
#define BYTES_PER_SECTOR_VALUE_2             0x02   /* 512 bytes, MSB */
#define BYTES_PER_SECTOR_VALUE               0x0200

#define SECTORS_PER_CLUSTER_ADDR             0x0D
#define SECTORS_PER_CLUSTER_SIZE             1      /* bytes */
#define SECTORS_PER_CLUSTER_VALUE            0x02

#define RESERVED_SECTORS_ADDR                0x0E
#define RESERVED_SECTORS_SIZE                2      /* bytes */
#define RESERVED_SECTORS_VALUE_1             0x01   /* LSB */
#define RESERVED_SECTORS_VALUE_2             0x00   /* MSB */

#define NBR_FAT_TABLES_ADDR                  0x10
#define NBR_FAT_TABLES_SIZE                  1      /* bytes */
#define NBR_FAT_TABLES_VALUE                 0x02

#define MAX_NBR_FILES_DIRS_IN_ROOT_ADDR      0x11
#define MAX_NBR_FILES_DIRS_IN_ROOT_SIZE      2      /* bytes */
#define MAX_NBR_FILES_DIRS_IN_ROOT_VALUE_1   0x00   /* 512 max, LSB */
#define MAX_NBR_FILES_DIRS_IN_ROOT_VALUE_2   0x02   /* 512 max, MSB */

#define NBR_SECTORS_IN_VOLUME_ADDR           0x13
#define NBR_SECTORS_IN_VOLUME_SIZE           2      /* bytes */
#define NBR_SECTORS_IN_VOLUME_VALUE_1        0x00   /* 256 sectors, LSB */
#define NBR_SECTORS_IN_VOLUME_VALUE_2        0x01   /* 256 sectors, MSB */

#define MEDIA_DESCRIPTOR_ADDR                0x15
#define MEDIA_DESCRIPTOR_SIZE                1      /* bytes */
#define MEDIA_DESCRIPTOR_VALUE               0xF8

#define SECTORS_PER_FAT_ADDR                 0x16
#define SECTORS_PER_FAT_SIZE                 2      /* bytes */
#define SECTORS_PER_FAT_VALUE_1              0x14   /* 20 sector/fat, LSB */
#define SECTORS_PER_FAT_VALUE_2              0x00   /* 20 sector/fat, MSB */
#define SECTORS_PER_FAT_VALUE                0x0014

#define SECTORS_PER_TRACK_ADDR               0x18
#define SECTORS_PER_TRACK_SIZE               2      /* bytes */
#define SECTORS_PER_TRACK_VALUE_1            0x20   /* 32 sector/track, LSB */
#define SECTORS_PER_TRACK_VALUE_2            0x00   /* 32 sector/track, MSB */

#define NBR_HEADS_ADDR                       0x1A
#define NBR_HEADS_SIZE                       2      /* bytes */
#define NBR_HEADS_VALUE_1                    0x10   /* 16 read/write heads, LSB */
#define NBR_HEADS_VALUE_2                    0x00   /* 16 read/write heads, MSB */

#define REMOVE_HIDDEN_SECTOR_ADDR            0x1C
#define REMOVE_HIDDEN_SECTOR_SIZE            4      /* bytes */
#define REMOVE_HIDDEN_SECTOR_VALUE_1         0x20   /* byte 1, LSB */
#define REMOVE_HIDDEN_SECTOR_VALUE_2         0x00   /* byte 1, MSB */
#define REMOVE_HIDDEN_SECTOR_VALUE_3         0x00   /* byte 2, LSB */
#define REMOVE_HIDDEN_SECTOR_VALUE_4         0x00   /* byte 2, MSB */

#define TOT_SECTORS_32_ADDR                  0x20
#define TOT_SECTORS_32_SIZE                  4
#define TOT_SECTORS_32_VALUE_1               0x00   /* byte 1, LSB */
#define TOT_SECTORS_32_VALUE_2               0x00   /* byte 1, MSB */
#define TOT_SECTORS_32_VALUE_3               0x00   /* byte 2, LSB */
#define TOT_SECTORS_32_VALUE_4               0x00   /* byte 2, MSB */

#define DRIVE_NUMBER_ADDR                    0x24
#define DRIVE_NUMBER_SIZE                    1
#define DRIVE_NUMBER_VALUE                   0x80

#define RESERVED1_ADDR                       0x25
#define RESERVED1_SIZE                       1
#define RESERVED1_VALUE                      0x00

#define BOOTSIGN_ADDR                        0x26
#define BOOTSIGN_SIZE                        1
#define BOOTSIGN_VALUE                       0x29

#define VOLUME_ID_ADDR                       0x27
#define VOLUME_ID_SIZE                       4
#define VOLUME_ID_VALUE_1                    0x12   /* Just an ID */
#define VOLUME_ID_VALUE_2                    0x34
#define VOLUME_ID_VALUE_3                    0x56
#define VOLUME_ID_VALUE_4                    0x78

#define VOLUME_LABEL_ADDR                    0x2B
#define VOLUME_LABEL_SIZE                    11
#define VOLUME_LABEL_VALUE                   "VOLUME NAME"

#define FILE_SYSTEM_TYPE_ADDR                0x36
#define FILE_SYSTEM_TYPE_SIZE                8
#define FILE_SYSTEM_TYPE_VALUE               "FAT16   "

#define BOOT_ROUTINE_ADDR                    0x3E
#define BOOT_ROUTINE_SIZE                    448    /* bytes */

#define SIGNATURE_ADDR                       0x1FE
#define SIGNATURE_SIZE                       2      /* bytes */
#define SIGNATURE_VALUE_1                    0x55   /* LSB */
#define SIGNATURE_VALUE_2                    0xAA   /* MSB*/


/* 2. First FAT-table */
#define FAT_TABLE_1_ADDR    (BOOTSECTOR_ADDR + BOOTSECTOR_SIZE)
#define FAT_TABLE_1_SIZE    (SECTORS_PER_FAT_VALUE * BYTES_PER_SECTOR_VALUE)    
    /* FAT TABLE size is 10240 bytes */

#define FAT1_RESERVED_0_ADDR                  FAT_TABLE_1_ADDR
#define FAT1_RESERVED_0_SIZE                  2      /* bytes */
#define FAT1_RESERVED_0_VALUE_1               0xF8   /* LSB */
#define FAT1_RESERVED_0_VALUE_2               0xFF   /* MSB*/

#define FAT1_RESERVED_1_ADDR                  (FAT_RESERVED_0_ADDR + FAT_RESERVED_0_SIZE)
#define FAT1_RESERVED_1_SIZE                  2      /* bytes */
#define FAT1_RESERVED_1_VALUE_1               0xFF8   /* LSB */
#define FAT1_RESERVED_1_VALUE_2               0xFF   /* MSB*/

/* 3. Second FAT-table */
#define FAT_TABLE_2_ADDR    (FAT_TABLE_1_ADDR + FAT_TABLE_1_SIZE)
#define FAT_TABLE_2_SIZE    (SECTORS_PER_FAT_VALUE * BYTES_PER_SECTOR_VALUE)
    /* FAT TABLE size is 10240 bytes */

#define FAT2_RESERVED_0_ADDR                  FAT_TABLE_2_ADDR
#define FAT2_RESERVED_0_SIZE                  FAT1_RESERVED_0_SIZE
#define FAT2_RESERVED_0_VALUE_1               FAT1_RESERVED_0_VALUE_1
#define FAT2_RESERVED_0_VALUE_2               FAT1_RESERVED_0_VALUE_2

#define FAT2_RESERVED_1_ADDR                  (FAT_RESERVED_1_ADDR + FAT_RESERVED_1_SIZE)
#define FAT2_RESERVED_1_SIZE                  FAT1_RESERVED_1_SIZE
#define FAT2_RESERVED_1_VALUE_1               FAT1_RESERVED_1_VALUE_1
#define FAT2_RESERVED_1_VALUE_2               FAT1_RESERVED_1_VALUE_2

/* 4. Root Directory */
#define ROOT_DIRECTORY_ADDR (FAT_TABLE_2_ADDR + FAT_TABLE_2_SIZE)
#define ROOT_DIRECTORY_SIZE ((NBR_SECTORS_IN_VOLUME_VALUE * BYTES_PER_SECTOR_VALUE) - BOOTSECTOR_SIZE - FAT_TABLE_1_SIZE - FAT_TABLE_2_SIZE)


#endif /* __GBI_I_H_ */
