/**
 * @file  time_stamp.h
 *
 * Gathered prototypesused for precice time logging.
 *
 * @author  J.A. Renia ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  1/23/2006 J.A. Renia ()   Create.
 *
 * (C) Copyright 2006 by Texas Instruments Incorporated, All Rights Reserved
 */

/* Timer logger */
/* Functions to use a precise timer and log it */
   
/************************************************************/ 
/*********************** TIMER STUFF *************************/ 
/************************************************************/ 
#include "rvm/rvm_api.h"
#include "rvm/rvm_use_id_list.h"
#include "tests/rtest/rtest_api.h"
#include <string.h>

#include "timer/timer.h"

#define SYS_UWORD16 UINT16
#define MEM_TIMER_ADDR        0xfffff800  /* TIMER control register */
#define MEM_TIMER1            0xFFFE3800
#define MEM_TIMER2            0xFFFE6800
#define TIMELOG_RELOAD_VALUE  0xFFFF
#define TIMELOG_AUTO_RELOAD_OPTION 1
#define TIMELOG_STR_LEN       75
#define TIMELOG_CLOCKSCALE    0

// Allowed values for TIMELOG_CLOCKSCALE
// 0 Timer full run: 174.85ms   One tick = 2.67us
// 1 Timer full run: 345ms      One tick = 5.26us
// 2 Timer full run: 695ms      One tick = 10.6us
// 3 Timer full run:  1.395s    One tick = 21.3us
// 4 Timer full run:  2.800s    One tick = 42.7us
// 5 Timer full run:  5.590s    One tick = 85.3us
// 6 Timer full run: 11.185s    One tick = 171us
// 7 Timer full run: 22.370s    One tick = 341us

#define TIMER_RUN_TIME        (17485)
#define TS_BUFFER_SIZE        (400)

#define TIMELOG_YES           (1)
#define TIMELOG_NO            (0)

#define TIMELOG_DISABLED
//#undef TIMELOG_DISABLED
#ifdef  TIMELOG_DISABLED
#define TIMELOG_RFS_ENABLED       TIMELOG_NO
#define TIMELOG_RFS_FAT_ENABLED   TIMELOG_NO
#define TIMELOG_GBI_ENABLED       TIMELOG_NO
#define TIMELOG_NAN_ENABLED       TIMELOG_NO
#define TIMELOG_NAN_BM_ENABLED    TIMELOG_NO
#define TIMELOG_MC_ENABLED        TIMELOG_NO
#else
#define TIMELOG_RFS_ENABLED       TIMELOG_YES  
#define TIMELOG_RFS_FAT_ENABLED   TIMELOG_YES  
#define TIMELOG_GBI_ENABLED       TIMELOG_YES  
#define TIMELOG_NAN_ENABLED       TIMELOG_YES
#define TIMELOG_NAN_BM_ENABLED    TIMELOG_YES
#define TIMELOG_MC_ENABLED        TIMELOG_YES
#endif


enum{
//RFS
  //non blocking functions not included
  TIMELOG_RFS=0,
  TIMELOG_RFS_RESP=0,
  TIMELOG_rfs_fcntl,
  TIMELOG_rfs_close,
  TIMELOG_rfs_write,
  TIMELOG_rfs_read,
  TIMELOG_rfs_lseek,
  TIMELOG_rfs_fchmod,
  TIMELOG_rfs_fstat,
  TIMELOG_rfs_fsync,
  TIMELOG_rfs_open,
  TIMELOG_rfs_chmod,
  TIMELOG_rfs_stat,
  TIMELOG_rfs_remove,
  TIMELOG_rfs_rename,
  TIMELOG_rfs_mkdir,
  TIMELOG_rfs_rmdir,
  TIMELOG_rfs_opendir,
  TIMELOG_rfs_readdir,
  TIMELOG_rfs_preformat,
  TIMELOG_rfs_format,
//RFSFAT
  TIMELOG_RFS_FAT,
  TIMELOG_RFS_FAT_RESP,
  TIMELOG_rfsfat_set_partition_table,
  TIMELOG_rfsfat_fcntl,
  TIMELOG_rfsfat_close,
  TIMELOG_rfsfat_write,
  TIMELOG_rfsfat_read,
  TIMELOG_rfsfat_lseek,
  TIMELOG_rfsfat_fchmod,
  TIMELOG_rfsfat_fstat,
  TIMELOG_rfsfat_fsync,
  TIMELOG_rfsfat_open_nb,
  TIMELOG_rfsfat_chmod_nb,
  TIMELOG_rfsfat_stat_nb,
  TIMELOG_rfsfat_remove_nb,
  TIMELOG_rfsfat_rename_nb,
  TIMELOG_rfsfat_mkdir_nb,
  TIMELOG_rfsfat_rmdir_nb,
  TIMELOG_rfsfat_opendir_nb,
  TIMELOG_rfsfat_readdir_nb,
  TIMELOG_rfsfat_preformat_nb,
  TIMELOG_rfsfat_format_nb,
//GBI
  TIMELOG_GBI,
  TIMELOG_GBI_RESP,
  TIMELOG_gbi_read,
  TIMELOG_gbi_write_with_spare,
  TIMELOG_gbi_write,
  TIMELOG_gbi_erase,
  TIMELOG_gbi_flush,
  TIMELOG_gbi_get_media_info,
  TIMELOG_gbi_get_partition_info,
  TIMELOG_gbi_subscribe_events,
  TIMELOG_gbi_read_spare_data,
//NAND
  TIMELOG_NAND,
  TIMELOG_NAND_RESP,
  TIMELOG_nan_flash_read,
  TIMELOG_nan_flash_program_page,
  TIMELOG_nan_flash_copy_back,
  TIMELOG_nan_flash_erase_block,
  TIMELOG_nan_flash_read_status,
  TIMELOG_nan_flash_read_id,
  TIMELOG_nan_flash_reset,
  TIMELOG_nan_flash_block_valid,
  TIMELOG_nan_parport_read,
  TIMELOG_nan_parport_read_continuous,
  TIMELOG_nan_dma_mode,
  TIMELOG_nan_flash_read_opt,
  TIMELOG_nan_flash_program_page_opt,
//NAND_BM
  TIMELOG_NAND_BM,
  TIMELOG_NAND_BM_RESP,
  TIMELOG_nand_bm_read,
  TIMELOG_nand_bm_write,
  TIMELOG_nand_bm_erase,
  TIMELOG_nand_bm_flush,
  TIMELOG_nand_bm_dma_mode,
//MC
  TIMELOG_MC,
  TIMELOG_MC_RESP,
  TIMELOG_mc_subscribe,
  TIMELOG_mc_unsubscribe,
  TIMELOG_mc_read,
  TIMELOG_mc_write,
  TIMELOG_mc_erase,
  TIMELOG_mc_set_write_protect,
  TIMELOG_mc_clr_write_protect,
  TIMELOG_mc_sd_get_mech_wp,
  TIMELOG_mc_get_write_protect,
  TIMELOG_mc_get_card_status,
  TIMELOG_mc_get_controller_status,
  TIMELOG_mc_dma_mode,
  TIMELOG_mc_update_acq,
  TIMELOG_mc_reset,
  TIMELOG_mc_get_stack_size,
  TIMELOG_mc_read_card_stack,
  TIMELOG_mc_read_OCR,
  TIMELOG_mc_read_CID,
  TIMELOG_mc_read_CSD,
  TIMELOG_mc_write_CSD,
  TIMELOG_mc_erase_CSD,
  TIMELOG_mc_send_notification,
  TIMELOG_mc_sd_get_card_status,
  TIMELOG_mc_get_card_type,
  TIMELOG_mc_read_scr,
  TIMELOG_END
};

void TS_PrintTimer (UINT32 Time, char* Text);
    
/*---------------------------------------------------------------*/
/*  TS_ResetTimer()                     */
/*--------------------------------------------------------------*/
/*              timer value, reload yes or not, scale       */
/* Return     : none                        */
/* Functionality : Give the timewr state                */
/*--------------------------------------------------------------*/
void  TS_ResetTimer (SYS_UWORD16 countValue, 
                     SYS_UWORD16 autoReload,
                     SYS_UWORD16 clockScale);

/*
 * TS_ReadTimer
 *
 * Returns current timer value
 *
 */
SYS_UWORD16 TS_ReadTimer (void);

/* Function to calculate the elapsed time since start of the timer. 
   Return in hundred's of miliseconds */
UINT32 TS_CalcElapsedTime (UINT16 count);

void TS_StartTimer  (void);
void TS_StopTimer   (void);
void TS_EnableTimer (void);
void TS_RestartTimer(void);

void _TS_StoreCurrentTimeStamp_Turbo (UINT8 TS_entity);
void _TS_StoreCurrentTimeStamp_Turbo_Param (UINT8 TS_entity, UINT32 param1, UINT32 param2,UINT32 param3);
void TS_PrintBuffer (void);

#define TS_StoreCurrentTimeStamp_Turbo(x)       _TS_StoreCurrentTimeStamp_Turbo(x)
#define TS_StoreCurrentTimeStamp_Turbo_Param(x1,x2,x3,x4) _TS_StoreCurrentTimeStamp_Turbo_Param(x1,x2,x3,x4)

#ifdef TIMELOG_RFS_API
  #if (!TIMELOG_RFS_ENABLED)
    #undef  TS_StoreCurrentTimeStamp_Turbo
    #define TS_StoreCurrentTimeStamp_Turbo(x)
    #undef  TS_StoreCurrentTimeStamp_Turbo_Param
    #define TS_StoreCurrentTimeStamp_Turbo_Param(x1,x2,x3,x4)
  #endif
#endif

#ifdef TIMELOG_RFS_FAT_API
  #if (!TIMELOG_RFS_FAT_ENABLED)
    #undef  TS_StoreCurrentTimeStamp_Turbo
    #define TS_StoreCurrentTimeStamp_Turbo(x)
    #undef  TS_StoreCurrentTimeStamp_Turbo_Param
    #define TS_StoreCurrentTimeStamp_Turbo_Param(x1,x2,x3,x4)
  #endif
#endif

#ifdef TIMELOG_GBI_API
  #if (!TIMELOG_GBI_ENABLED)
    #undef  TS_StoreCurrentTimeStamp_Turbo
    #define TS_StoreCurrentTimeStamp_Turbo(x)
    #undef  TS_StoreCurrentTimeStamp_Turbo_Param
    #define TS_StoreCurrentTimeStamp_Turbo_Param(x1,x2,x3,x4)
  #endif
#endif

#ifdef TIMELOG_MC_API
  #if (!TIMELOG_MC_ENABLED)
    #undef  TS_StoreCurrentTimeStamp_Turbo
    #define TS_StoreCurrentTimeStamp_Turbo(x)
    #undef  TS_StoreCurrentTimeStamp_Turbo_Param
    #define TS_StoreCurrentTimeStamp_Turbo_Param(x1,x2,x3,x4)
  #endif
#endif

#ifdef TIMELOG_NAN_API
  #if (!TIMELOG_NAN_ENABLED)
    #undef  TS_StoreCurrentTimeStamp_Turbo
    #define TS_StoreCurrentTimeStamp_Turbo(x)
    #undef  TS_StoreCurrentTimeStamp_Turbo_Param
    #define TS_StoreCurrentTimeStamp_Turbo_Param(x1,x2,x3,x4)
  #endif
#endif

#ifdef TIMELOG_NAN_BM_API
  #if (!TIMELOG_NAN_BM_ENABLED)
    #undef  TS_StoreCurrentTimeStamp_Turbo
    #define TS_StoreCurrentTimeStamp_Turbo(x)
    #undef  TS_StoreCurrentTimeStamp_Turbo_Param
    #define TS_StoreCurrentTimeStamp_Turbo_Param(x1,x2,x3,x4)
  #endif
#endif




/************************************************************/ 
/*********************** END TIMER STUFF *********************/ 
/************************************************************/ 



