/**
 * @file  rfsfat_i.h
 *
 * Internal definitions for RFSFAT.
 *
 * @author  Anton van Breemen
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  01/23/2004  Anton van Breemen   Create.
 *	04/29/2004	E. Oude Middendorp		FAT32 added.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */

#ifndef __RFSFAT_INST_I_H_
#define __RFSFAT_INST_I_H_

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#ifndef _WINDOWS
#include "nucleus.h"
#include "inth/sys_inth.h"
#endif
/* FATFS optimization */
#define ICT_SPEEDUP_RFSFAT1

#define RFSFAT_SEEK_MAX_WINDOW_SIZE      (20480)

#define ICT_SPEEDUP_RFSFAT2
#if 0
#define ICT_SPEEDUP_RFSFAT3
#endif
#define ICT_SPEEDUP_RFSFAT4		/* cluster Size calculations */

#include "rfs/fscore_types.h"

#include "rfsfat_cfg.h"
#include "rfsfat_ffs.h"
#include "rfsfat_dir.h"
#include "rfsfat_mma.h"
#include "rfsfat_fam.h"
#include "rfsfat_clm.h"
#include "rfsfat_ffs_dman.h"
#include "rfsfat_ffs_file.h"
#include "rfsfat_blk.h"
#include "rfsfat_state_i.h"
#include "gbi/gbi_api.h"
#include "gbi/gbi_message.h"

/******************************************************************************
*     Defines and enums                                                       *
******************************************************************************/
 
#define RFSFAT_MMA_JMPCODE_SIZE    (3) /* nmb of bytes */
#define RFSFAT_MMA_BLKMARK_SIZE    (2) /* nmb of bytes */
#define MPT_START               0x0000  //every mountpoint (partition) starts
                      //on adress 0. That it can be the 3th
                      //partition on a media doesn't matter
                      //to us. we only adress mountpoint. 
                      //The GBI must translate this


//while GBI is not yet ready and included define it here
//#define GBI_MAX_PARTITION_NAME  6
//LFN defines
#define MIN_LFN_ENTRIES   0x41
#define MAX_LFN_ENTRIES   0x54
#define FIRST_POS_MARKER  0x40
#define BYTES_IN_LFNE     26 
#define CHARS_IN_LFNE     13 
#define DEF_LFNE_VALUE    0xFF


#define rfsfat_lock_mutex_pair_id()    rvf_lock_mutex (&rfsfat_env_ctrl_blk_p->mutex_pair_id)
#define rfsfat_unlock_mutex_pair_id()  rvf_unlock_mutex (&rfsfat_env_ctrl_blk_p->mutex_pair_id)

#define RFS_FD_DEF_VALUE         0x0000
#define RFS_ADDR_ID_DEF_VALUE    0x00
#define RFS_CB_DEF_VALUE         NULL
#define RFS_OPMODE_DEF_VALUE     O_SIO
#define LFN_DIR_SIZE_META_NAME   0x101 /* Size of meta data fields in long filename == 0xff 
                                          + 1 for EOSTR (+ 1 for alignment)*/

#define EOSTR                   (0)
#define POSITION_FIRST_CHAR_IN_PATH                                           0

/******************************************************************************/
/* Definition of return value offsets of the different modules.               */
/******************************************************************************/
#define FFS_OFFSET_RETURNVALUES_FFS   1






/******************************************************************************/
/* Default return value, used by all modules, indicating operation was        */
/* ended successfully.                                                        */
/******************************************************************************/

#define FFS_NO_ERRORS                                                         0

#define FFS_RV_UNAVAILABLE                     (FFS_OFFSET_RETURNVALUES_FFS+ 0)
#define FFS_RV_FORMATTED                       (FFS_OFFSET_RETURNVALUES_FFS+ 1)
#define FFS_RV_UNFORMATTED                     (FFS_OFFSET_RETURNVALUES_FFS+ 2)
#define FFS_RV_ERR_NOT_INITIALISED             (FFS_OFFSET_RETURNVALUES_FFS+ 3)
#define FFS_RV_ERR_FILE_ALREADY_EXISTS         (FFS_OFFSET_RETURNVALUES_FFS+ 4)
#define FFS_RV_ERR_INVALID_ACCESS_MODE         (FFS_OFFSET_RETURNVALUES_FFS+ 5)
#define FFS_RV_ERR_INVALID_HANDLE              (FFS_OFFSET_RETURNVALUES_FFS+ 6)
#define FFS_RV_ERR_INVALID_NAMEEXT             (FFS_OFFSET_RETURNVALUES_FFS+ 7)
#define FFS_RV_ERR_INVALID_OFFSET              (FFS_OFFSET_RETURNVALUES_FFS+ 8)
#define FFS_RV_ERR_INVALID_PATH                (FFS_OFFSET_RETURNVALUES_FFS+ 9)
#define FFS_RV_IS_DIR                          (FFS_OFFSET_RETURNVALUES_FFS+10)
#define FFS_RV_IS_PART                         (FFS_OFFSET_RETURNVALUES_FFS+11)
#define FFS_RV_IS_NOT_A_DIR                    (FFS_OFFSET_RETURNVALUES_FFS+12)
#define FFS_RV_IS_NOT_A_FILE                   (FFS_OFFSET_RETURNVALUES_FFS+13)
#define FFS_RV_IS_NOT_A_PART                   (FFS_OFFSET_RETURNVALUES_FFS+14)
#define FFS_RV_ERR_FILE_ALLREADY_OPEN          (FFS_OFFSET_RETURNVALUES_FFS+15)
#define FFS_RV_ERR_FILE_READ_ONLY              (FFS_OFFSET_RETURNVALUES_FFS+16)
#define FFS_RV_ERR_TO_MUCH_OPEN_FILES          (FFS_OFFSET_RETURNVALUES_FFS+17)
#define FFS_RV_ERR_INVALID_PARAMS              (FFS_OFFSET_RETURNVALUES_FFS+18)
#define FFS_RV_ERR_WRITE_NOT_ALLOWED           (FFS_OFFSET_RETURNVALUES_FFS+19)
#define FFS_RV_ERR_TOO_MUCH_FILE_DATA          (FFS_OFFSET_RETURNVALUES_FFS+20)
#define FFS_RV_ERR_ENTRY_EMPTY                 (FFS_OFFSET_RETURNVALUES_FFS+21)
#define FFS_RV_ERR_LAST_ENTRY                  (FFS_OFFSET_RETURNVALUES_FFS+22)
#define FFS_RV_FILES_IN_OPEN_FILE_LIST         (FFS_OFFSET_RETURNVALUES_FFS+23)
#define FFS_RV_ROOTDIR_SELECTED                (FFS_OFFSET_RETURNVALUES_FFS+24)
#define FFS_RV_ALREADY_INITIALISED             (FFS_OFFSET_RETURNVALUES_FFS+25)
#define FFS_RV_ERR_DIFFERENT_PART              (FFS_OFFSET_RETURNVALUES_FFS+26)
#define FFS_RV_ERR_DATE_TIME_DETERMINATION     (FFS_OFFSET_RETURNVALUES_FFS+27)
#define FFS_RV_ERR_MOVING_NOT_ALLOWED          (FFS_OFFSET_RETURNVALUES_FFS+28)
#define FFS_RV_ERR_REMOVING_NOT_ALLOWED        (FFS_OFFSET_RETURNVALUES_FFS+29)
#define FFS_RV_ERR_WRITING_IN_READMODE         (FFS_OFFSET_RETURNVALUES_FFS+30)
#define FFS_RV_NO_SUBDIR_OF_DIR                (FFS_OFFSET_RETURNVALUES_FFS+31)
#define FFS_RV_SUBDIR_OF_DIR                   (FFS_OFFSET_RETURNVALUES_FFS+32)
#define FFS_RV_FEATURE_UNAVAILABLE             (FFS_OFFSET_RETURNVALUES_FFS+33)
#define FFS_RV_ERR_QUERY_ALREADY_STARTED       (FFS_OFFSET_RETURNVALUES_FFS+34)
#define FFS_RV_ERR_END_OF_QUERY_RESULT         (FFS_OFFSET_RETURNVALUES_FFS+35)
#define FFS_RV_ERR_QUERY_NOT_STARTED           (FFS_OFFSET_RETURNVALUES_FFS+36)
#define FFS_RV_ERR_INVALID_QUERY               (FFS_OFFSET_RETURNVALUES_FFS+37)
#define FFS_RV_ERR_WRITING_FAT_TABLES          (FFS_OFFSET_RETURNVALUES_FFS+38)

#define LAST_FFS_OFFSET_USED                   (FFS_OFFSET_RETURNVALUES_FFS+38)

/******************************************************************************/
/* Directory module return values.                                            */
/******************************************************************************/
#define FFS_OFFSET_RETURNVALUES_DIR   (LAST_FFS_OFFSET_USED+1)
#define DIR_RV_DIR_IS_EMPTY     (FFS_OFFSET_RETURNVALUES_DIR+0) /* The directory is empty. */
#define DIR_RV_DIR_IS_NOT_EMPTY (FFS_OFFSET_RETURNVALUES_DIR+1) /* The directory is not empty. */
#define DIR_RV_DIR_NOT_FOUND    (FFS_OFFSET_RETURNVALUES_DIR+2) /* Directory label not found. */
#define DIR_RV_DIRTABLE_FULL    (FFS_OFFSET_RETURNVALUES_DIR+3) /* The directory table is full. Writing not possible. */
#define DIR_RV_INVALID_PARAM    (FFS_OFFSET_RETURNVALUES_DIR+4) /* The wrong parameters */
#define DIR_RV_CORRUPT_LFN      (FFS_OFFSET_RETURNVALUES_DIR+5) /* The wrong parameters */
#define LAST_DIR_OFFSET_USED    (FFS_OFFSET_RETURNVALUES_DIR+5)

/*******************************************************************************/
/* Return values used by the FAT manager                                       */
/*******************************************************************************/
#define FFS_OFFSET_RETURNVALUES_FAM   (LAST_DIR_OFFSET_USED+1)
#define FAM_RET_CLUSTER_NR      (FFS_OFFSET_RETURNVALUES_FAM+0)
#define FAM_RET_BAD_CLUSTER     (FFS_OFFSET_RETURNVALUES_FAM+1)
#define FAM_RET_NO_FAT_CLUSTER  (FFS_OFFSET_RETURNVALUES_FAM+2)
#define FAM_RET_LAST_CLUSTER    (FFS_OFFSET_RETURNVALUES_FAM+3)
#define FAM_RET_FAT_READ_ERROR  (FFS_OFFSET_RETURNVALUES_FAM+4)
#define FAM_RET_FAT_WRITE_ERROR (FFS_OFFSET_RETURNVALUES_FAM+5)
#define FAM_RET_FAT_NO_SPACE    (FFS_OFFSET_RETURNVALUES_FAM+6)
#define FAM_RESERVED_CLUSTER    (FFS_OFFSET_RETURNVALUES_FAM+7)
#define FAM_RET_FREE_CLUSTER    (FFS_OFFSET_RETURNVALUES_FAM+8)
#define FAM_RET_ROOT_CLUSTER    (FFS_OFFSET_RETURNVALUES_FAM+9)

#define LAST_FAM_OFFSET_USED    (FFS_OFFSET_RETURNVALUES_FAM+9)

/******************************************************************************/
/* Return values used of the MMA                                              */
/******************************************************************************/
#define FFS_OFFSET_RETURNVALUES_MMA   (LAST_FAM_OFFSET_USED+1)
#define MMA_RET_NO_SPACE              (FFS_OFFSET_RETURNVALUES_MMA + 1)
#define MMA_RET_WRITE_ERR             (FFS_OFFSET_RETURNVALUES_MMA + 2)
#define MMA_RET_READ_ERR              (FFS_OFFSET_RETURNVALUES_MMA + 3)
#define MMA_RET_FX_TABLE_ERR          (FFS_OFFSET_RETURNVALUES_MMA + 4)
#define MMA_RET_BS_BAD_VAL            (FFS_OFFSET_RETURNVALUES_MMA + 5)
#define MMA_RET_INCOR_BOOTRECORD      (FFS_OFFSET_RETURNVALUES_MMA + 6)
#define MMA_RET_VOLUME_NOT_AVAILABLE  (FFS_OFFSET_RETURNVALUES_MMA + 7)
#define MMA_RET_RW_OUTSIDE_PARTITION  (FFS_OFFSET_RETURNVALUES_MMA + 8)
#define LAST_MMA_OFFSET_USED          (FFS_OFFSET_RETURNVALUES_MMA + 8)

/******************************************************************************/
/* Return values used of the CLM                                              */
/******************************************************************************/
#define FFS_OFFSET_RETURNVALUES_CLM   (LAST_MMA_OFFSET_USED+1)
#define CLM_RET_UNDEF_ERROR           (FFS_OFFSET_RETURNVALUES_CLM+0)
#define CLM_RET_LIST_ERROR            (FFS_OFFSET_RETURNVALUES_CLM+1) /* logical error in cluster linked list                                       */
#define CLM_RET_NO_SPACE              (FFS_OFFSET_RETURNVALUES_CLM+2) /* There is no space left in ROOT                                             */

#define LAST_CLM_OFFSET_USED          (FFS_OFFSET_RETURNVALUES_CLM+2)

/******************************************************************************/
/* Return values of general use                                               */
/******************************************************************************/
#define FFS_OFFSET_RETURNVALUES_GEN   (LAST_CLM_OFFSET_USED+1)
#define RFSFAT_INTERNAL_ERR            (FFS_OFFSET_RETURNVALUES_GEN+0)
#define RFSFAT_MEMORY_ERR			  (FFS_OFFSET_RETURNVALUES_GEN+1)	
#define	RFSFAT_INVALID_PARAM   		  (FFS_OFFSET_RETURNVALUES_GEN+2)	

#define LAST_GEN_OFFSET_USED          (FFS_OFFSET_RETURNVALUES_GEN+3)

#define	RFSFAT_OK					      FFS_NO_ERRORS    






/******************************************************************************
 *     Macros                                                                 *
 ******************************************************************************/

/** Macro used for tracing RFSFAT messages. */
//#define RFSFAT_ENABLE_TRACE
#ifndef RFSFAT_ENABLE_TRACE
#define RFSFAT_SEND_TRACE(string, trace_level)
#define RFSFAT_SEND_TRACE_PARAM(string, param, trace_level)
#define RFSFAT_SEND_TRACE_PARAM_DEBUG_LOW(string, param)
#define RFSFAT_SEND_TRACE_PARAM_WARNING(string, param)
#define RFSFAT_SEND_TRACE_DEBUG_LOW(string)
#define RFSFAT_SEND_TRACE_WARNING(string)
#define RFSFAT_SEND_TRACE_PARAM_DEBUG_HIGH(string, param)
#define RFSFAT_SEND_TRACE_DEBUG_HIGH(string)
#define RFSFAT_SEND_TRACE_PARAM_ERROR(string, param) 
#define RFSFAT_SEND_TRACE_ERROR(string) 

#else  //#ifndef RFSFAT_ENABLE_TRACE


#define RFSFAT_SEND_TRACE(string, trace_level) \
	rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, RFSFAT_USE_ID)

#define RFSFAT_SEND_TRACE_PARAM(string, param, trace_level) \
	rvf_send_trace (string, (sizeof(string) - 1), param, trace_level, RFSFAT_USE_ID)

#define RFSFAT_SEND_TRACE_PARAM_DEBUG_LOW(string, param) \
	rvf_send_trace (string, (sizeof(string) - 1), param, RV_TRACE_LEVEL_DEBUG_LOW, RFSFAT_USE_ID)

#define RFSFAT_SEND_TRACE_PARAM_WARNING(string, param) \
	rvf_send_trace (string, (sizeof(string) - 1), param, RV_TRACE_LEVEL_WARNING, RFSFAT_USE_ID)

#define RFSFAT_SEND_TRACE_DEBUG_LOW(string) \
	rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, RFSFAT_USE_ID)

#define RFSFAT_SEND_TRACE_WARNING(string) \
	rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, RV_TRACE_LEVEL_WARNING, RFSFAT_USE_ID)

#define RFSFAT_SEND_TRACE_PARAM_DEBUG_HIGH(string, param) \
	rvf_send_trace (string, (sizeof(string) - 1), param, RV_TRACE_LEVEL_DEBUG_HIGH, RFSFAT_USE_ID)

#define RFSFAT_SEND_TRACE_DEBUG_HIGH(string) \
	rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, RV_TRACE_LEVEL_DEBUG_HIGH, RFSFAT_USE_ID)


#define RFSFAT_SEND_TRACE_PARAM_ERROR(string, param) \
	rvf_send_trace (string, (sizeof(string) - 1), param, RV_TRACE_LEVEL_ERROR, RFSFAT_USE_ID)

#define RFSFAT_SEND_TRACE_ERROR(string) \
	rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, RV_TRACE_LEVEL_ERROR, RFSFAT_USE_ID)

#endif  //#ifndef RFSFAT_ENABLE_TRACE





#ifdef RFSFAT_ENABLE_API_TRACE
#define RFSFAT_API_TRACE(str) RFSFAT_SEND_TRACE((str), RV_TRACE_LEVEL_DEBUG_HIGH)
#else
#define RFSFAT_API_TRACE(str) /* nothing */
#endif

/** assert macros **/
#define RFSFAT_ASSERT_PREFIX "RFSFAT Assertion failed: "
#define RFSFAT_ASSERT_CONTINUE(expr) do { if(!(expr)) {\
		RFSFAT_SEND_TRACE_PARAM_ERROR(RFSFAT_ASSERT_PREFIX #expr " is FALSE in " __FILE__ " line",__LINE__); \
               							 }} while(0)
                
#define RFSFAT_ASSERT_LEAVE(expr, fail_code) do {if(!(expr)) { \
		RFSFAT_SEND_TRACE_PARAM_ERROR(RFSFAT_ASSERT_PREFIX #expr " is FALSE in " __FILE__ " line",__LINE__); \
				 fail_code;} }while(0) 


#define RFSFAT_GETBUF(s,p) {if (rvf_get_buf (rfsfat_env_ctrl_blk_p->prim_mb_id, \
                                s, (T_RVF_BUFFER **) p) == RVF_RED) \
                            {RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: Error to get memory :"__FILE__",line:", __LINE__); \
                             return RFSFAT_MEMORY_ERR; } }


#define RFSFAT_GETBUF_NO_RETURN(s,p) {if (rvf_get_buf (rfsfat_env_ctrl_blk_p->prim_mb_id, \
                                s, (T_RVF_BUFFER **) p) == RVF_RED) \
                            {RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: Error to get memory :"__FILE__",line:", __LINE__); \
                             /* Dont return */ } }

                             
#define RFSFAT_FREE_BUF(buf_p)   {if(buf_p) rvf_free_buf(buf_p);(buf_p)=NULL;}

#define RFSFAT_LFN_FLAG       rfsfat_env_ctrl_blk_p->ffs_globals.lfn_data.lfn_flag
#define RFSFAT_LFN            rfsfat_env_ctrl_blk_p->ffs_globals.lfn_data.lfn
#define RFSFAT_LFN_FROM_LABEL rfsfat_env_ctrl_blk_p->ffs_globals.lfn_data.lfn_from_label

#define ATTR_CHECK_SUBDIR(pntr) ((pntr->ucAttribute & FFS_ATTRIB_SUBDIR_MASK) == \
                                  FFS_ATTRIB_SUBDIR_MASK) 
#define ATTR_CHECK_LFN(pntr) ((pntr->ucAttribute & LFN_MASK) == LFN_MASK)
#define ATTR_CHECK_PART(pntr) ((pntr->ucAttribute & FFS_ATTRIB_PART_MASK) == \
                                FFS_ATTRIB_PART_MASK) 

#define CPY_LBL_TO_LBL(l1,l2)    {(void)rfsfat_vpMemcpy((void*)(l1),(const void*)(l2),\
                                   sizeof(DIR_DIRECTORY_LABEL_STRUCT));}

#define RFSFAT_FIRST_LFN_ENTRY ((DIR_META_DATA.aucName[0] >= MIN_LFN_ENTRIES) && \
                                (DIR_META_DATA.aucName[0] <= MAX_LFN_ENTRIES))

#define ATTR_CHECK_NAME_SMALL(pntr) ((pntr->lcase & FFS_ATTRIB_NAME_SMALL_MASK) == FFS_ATTRIB_NAME_SMALL_MASK)

#define ATTR_CHECK_EXT_SMALL(pntr)  ((pntr->lcase &FFS_ATTRIB_EXT_SMALL_MASK)== FFS_ATTRIB_EXT_SMALL_MASK)


/******************************************************************************
 *     Typedefs                                                               *
 ******************************************************************************/

/**
 * This structure is used for long filenames, containing information on the 
 * filename used /searched for is LFN or 8+3 when the name is long it will be 
 * stored in this structure
 * 
 */
typedef struct
{
  BOOL lfn_flag;                          //indicates long filename
  UINT8 lfn[LFN_DIR_SIZE_META_NAME];  //the long filename + 1 for the EOSTR
  UINT16 *lfn_from_label;                 //long filename read from label
}RFSFAT_LFN_DAT;

typedef struct
{
  MMA_BOOT_SECTOR_INFO_STRUCT * MMA_stBootSector;
  MMA_MEDIA_STRUCT astMediaFunctions;
} MMA_DATA_STRUCT;

/**
 * This structure defines the file descriptor table, containing information 
 * about the open files.
 */
typedef struct
{
  T_FSCORE_FD fd;       /* File descriptor */
  T_RV_RETURN ret_path;     /* Return path */
  UINT8 op_mode;        /* Opened file in synchronous/asynchronous mode */
} T_RFSFAT_FD_TABLE;



typedef struct
{
 UINT32       fatCashSectorBase;
 UINT8        *pFamCash;
 BOOL         fatCashChanged;
 UINT8		  reserv1;    /* Packing for 32 bits */
 UINT8		  reserv2;    /* packing for 32 bits */
 UINT32		  nmbFatEntriesInFamCash;
}T_RFSFAT_FATCACHE;


/**
 * @name RFSFAT_FFS_GLOBALS
 *
 * This type definition describes the data type that contains all variables
 * that previously were ffs globals.
 */
/*@{*/
typedef struct
{
  MMA_DATA_STRUCT                  media_data;
  BOOL                             ffs_bSystemInitDone;   //prevent access before init
  BOOL                             bInitialised;      //indicates if MMA is initialised
  OPEN_FILE_ENTRY_STRUCT           ffs_astOpenFileList[FSCORE_MAX_NR_OPEN_FSCORE_FILES];
  T_RFSFAT_FD_TABLE                fd_table[FSCORE_MAX_NR_OPEN_FSCORE_FILES];
  QUERY_LIST_STRUCT                ffs_stQuery;
 
  UINT8                            fd_rr;          //upper byt, round robin
  UINT8                            aucJmpCode[RFSFAT_MMA_JMPCODE_SIZE];
  UINT8                            ucMediaDescr;
  UINT8                            aucExecMarker[RFSFAT_MMA_BLKMARK_SIZE];
  DIR_DIRECTORY_LABEL_STRUCT       stMetaData;

  T_FSCORE_PARTITION_TABLE         mpt_tbl;  //maps media and partition
  T_RFSFAT_FATCACHE				   fat_cache[GBI_MAX_NR_OF_PARTITIONS];	
  BLK_DATA_STRUCT                  blk_data;   //holds block manager specific data
  RFSFAT_LFN_DAT                   lfn_data;  //long filename data
  UINT32                           last_clusterSeqNmb;
  UINT32                           last_get_next_cluster_next;
  UINT32                           clusterBaseNmb;
} RFSFAT_FFS_GLOBALS;


typedef struct
{
  void (*convert_unicode_to_u8) (const T_WCHAR *mp_uc, UINT8 *mp);
  void (*convert_u8_to_unicode) (const UINT8 *mp, T_WCHAR *mp_uc);
} T_FFS_UNICODE_FUNC_TABLE;



/*@}*/

/**
 * The Control Block buffer of RFSFAT, which gathers all 'Global variables'
 * used by RFSFAT instance.
 *
 * A structure should gathers all the 'global variables' of RFSFAT instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_RFSFAT_ENV_CTRL_BLK buffer is allocated when creating RFSFAT instance and is 
 * then always refered by RFSFAT instance when access to 'global variable' 
 * is necessary.
 */
typedef struct
{
  /** Store the current state of the RFSFAT instance */
  T_RFSFAT_INTERNAL_STATE state;

  /** Pointer to the error function */
    T_RVM_RETURN (*error_ft) (T_RVM_NAME swe_name,
                T_RVM_RETURN error_cause,
                T_RVM_ERROR_TYPE error_type,
                T_RVM_STRING error_msg);
  T_RVF_MB_ID prim_mb_id;
  T_RVF_ADDR_ID addr_id;
  RFSFAT_FFS_GLOBALS ffs_globals; //container of all previously global variables
  T_RV_RETURN gbi_return_path;
  T_RV_RETURN_PATH return_path_2nd_queue; //returnpath to second rfsfat mailbox
  //used to recieve response msg of GBI
  //on a blk read or blk write action.
  T_FSCORE_RET pair_id;     //for msg req-resp pairing: cnt
  T_RVF_MUTEX mutex_pair_id;  //for msg req-resp pairing
  
void *gbi_result_p;
  /* Media information */
  UINT8				nmb_of_media;
  T_GBI_MEDIA_INFO *media_info_p;
  UINT32 unmount_count;


#if (FFS_UNICODE ==1)
/****************************************************************************/
// Unicode
/****************************************************************************/
  T_FFS_UNICODE_FUNC_TABLE  uc_conv;  //unicode support functions
/****************************************************************************/
// end of Unicode
/****************************************************************************/
#endif 
  
  
}
T_RFSFAT_ENV_CTRL_BLK;

/** External ref "global variables" structure. */
extern T_RFSFAT_ENV_CTRL_BLK *rfsfat_env_ctrl_blk_p;



/*********** functions */
T_FSCORE_RET rfsfat_send_response_msg (T_FSCORE_CMD_ID cmdId,
                     T_FSCORE_PAIR_VALUE pair_value,
                     T_RV_RETURN * return_path,
                     UINT8 internal_status,
                     T_FSCORE_RET result);

#endif /* __RFSFAT_INST_I_H_ */
