/**
 * @file	rvf_pool_size.h
 *
 * This file computes the sum of all included SWE pool sizes
 * (memory bank sizes + stack sizes) in order to initialize the
 * main pool size for Riviera memory allocator.
 *
 * IMPORTANT:
 * The SWE are classified in alphabetical order of the USE ID.
 * Please respect this rule.
 *
 * (C) Copyright 1999 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __RVF_POOL_SIZE_H_
#define __RVF_POOL_SIZE_H_

#ifndef _WINDOWS
  #include "config/r2d.cfg"
  #include "config/debug.cfg"
  #include "config/board.cfg"
  #include "config/l1sw.cfg"
  #include "config/swconfig.cfg"
#endif

#include "config/rv.cfg"

#include "rv/rv_defined_swe.h"
#if defined(RVM_MDC_SWE)
  #include "config/mdc.cfg"
#endif


/*
 * ATP SWE
 */
#ifdef RVM_ATP_SWE
  #include "atp/atp_pool_size.h"
  #define RVF_ATP_POOL_SIZE (ATP_POOL_SIZE)
#else
  #define RVF_ATP_POOL_SIZE (0)
#endif

/*
 * ATP_UART SWE
 */
#ifdef RVM_ATP_UART_SWE
  #include "atp/atp_pool_size.h"
  #define RVF_ATP_UART_POOL_SIZE  (ATP_UART_POOL_SIZE)
#else
  #define RVF_ATP_UART_POOL_SIZE  (0)
#endif

/*
 * AUDIO SWE
 */
#ifdef RVM_AUDIO_MAIN_SWE
  #include "audio/audio_pool_size.h"
  #define RVF_AUDIO_POOL_SIZE  (AUDIO_POOL_SIZE)
  #define RVF_AUDIO_INT_POOL_SIZE  (AUDIO_INT_POOL_SIZE)
#else
  #define RVF_AUDIO_POOL_SIZE  (0)
  #define RVF_AUDIO_INT_POOL_SIZE  (0)
#endif

/*
 * Background AUDIO SWE
 */
#ifdef RVM_AUDIO_BGD_SWE
  #include "audio/audio_pool_size.h"
  #define RVF_AUDIO_BGD_POOL_SIZE  (AUDIO_BGD_POOL_SIZE)
  #define RVF_AUDIO_BGD_INT_POOL_SIZE  (AUDIO_BGD_INT_POOL_SIZE)
#else
  #define RVF_AUDIO_BGD_POOL_SIZE  (0)
  #define RVF_AUDIO_BGD_INT_POOL_SIZE  (0)
#endif

/*
 * AS SWE
 */
#ifdef RVM_AS_SWE
  #include "as/as_pool_size.h"
  #define RVF_AS_POOL_SIZE  (AS_POOL_SIZE)
  #define RVF_AS_INT_POOL_SIZE  (AS_INT_POOL_SIZE)
#else
  #define RVF_AS_POOL_SIZE  (0)
  #define RVF_AS_INT_POOL_SIZE  (0)
#endif


/*
 * BPR SWE
 */

#ifdef RVM_BPR_SWE
  /*
   * Values used in bpr1_env.h
   */
  #define RVF_BPR_STACK_SIZE (2 * 1024)
  #define RVF_BPR_MB1_SIZE   (102 * 1024) //(0x40000)
  #define RVF_BPR_POOL_SIZE  (RVF_BPR_STACK_SIZE + RVF_BPR_MB1_SIZE)
#else
  #define RVF_BPR_POOL_SIZE  (0)
#endif

/*
 * BAE SWE
 */

#ifdef RVM_BAE_SWE
  #include "bae/bae_pool_size.h"
  #define RVF_BAE_POOL_SIZE      (BAE_POOL_SIZE)
  #define RVF_BAE_INT_POOL_SIZE  (BAE_INT_POOL_SIZE)
#else
  #define RVF_BAE_POOL_SIZE  (0)
  #define RVF_BAE_INT_POOL_SIZE  (0)
#endif


/* 
 * DACI SWE
 */
#ifdef RVM_DACI_SWE
  #include "daci/daci_pool_size.h"
  #define RVF_DACI_POOL_SIZE  (DACI_POOL_SIZE)
#else
  #define RVF_DACI_POOL_SIZE  (0)
#endif

/*
 * DAR SWE
 */
#ifdef RVM_DAR_SWE
  #include "dar/dar_pool_size.h"
  #define RVF_DAR_POOL_SIZE  (DAR_POOL_SIZE)
#else
  #define RVF_DAR_POOL_SIZE  (0)
#endif

/*
 * DCM SWE
 */
#ifdef RVM_DCM_SWE
  #include "dcm/dcm_pool_size.h"
  #define RVF_DCM_POOL_SIZE  (DCM_POOL_SIZE)
#else
  #define RVF_DCM_POOL_SIZE  (0)
#endif

/*
 * DCFG SWE
 */
#ifdef RVM_DCFG_SWE
  #include "dcfg/dcfg_pool_size.h"
  #define RVF_DCFG_POOL_SIZE  (DCFG_POOL_SIZE)
#else
  #define RVF_DCFG_POOL_SIZE  (0)
#endif


/*
 * DMG SWE
 */
#ifdef RVM_DMG_SWE
  #include "dmg/dmg_pool_size.h"
  #define RVF_DMG_POOL_SIZE  (DMG_POOL_SIZE)
  #define RVF_DMG_INT_POOL_SIZE  (DMG_INT_POOL_SIZE)
#else
  #define RVF_DMG_POOL_SIZE  (0)
  #define RVF_DMG_INT_POOL_SIZE  (0)
#endif

/*
 * CAMD SWE
 */
#ifdef RVM_CAMD_SWE
  #include "camd/camd_pool_size.h"
  #define RVF_CAMD_POOL_SIZE  (CAMD_POOL_SIZE)
  #define RVF_CAMD_INT_POOL_SIZE  (CAMD_INT_POOL_SIZE)
#else
  #define RVF_CAMD_POOL_SIZE  (0)
  #define RVF_CAMD_INT_POOL_SIZE  (0)
#endif

/*
 * DMA SWE
 */
#ifdef RVM_DMA_SWE
  #include "dma/dma_pool_size.h"
  #define RVF_DMA_POOL_SIZE  (DMA_POOL_SIZE)
  #define RVF_DMA_INT_POOL_SIZE  (DMA_INT_POOL_SIZE)
#else
  #define RVF_DMA_POOL_SIZE  (0)
  #define RVF_DMA_INT_POOL_SIZE  (0)
#endif

/*
 * ETM SWE
 */
#ifdef RVM_ETM_SWE
  #include "etm/etm_pool_size.h"
  #define RVF_ETM_POOL_SIZE  (ETM_POOL_SIZE)
#else
  #define RVF_ETM_POOL_SIZE  (0)
#endif

/*
 * FFS SWE
 */
#ifdef RVM_FFS_SWE
  #include "ffs/ffs_pool_size.h"
  #define RVF_FFS_POOL_SIZE  (FFS_POOL_SIZE)
#else
  #define RVF_FFS_POOL_SIZE  (0)
#endif


/*
 * HASH SWE
 */
#ifdef RVM_HASH_SWE
  #include "hash/hash_pool_size.h"
  #define RVF_HASH_POOL_SIZE  (HASH_POOL_SIZE)
#else
   #define RVF_HASH_POOL_SIZE  (0)
#endif



/*
 * I2C SWE
 */
#ifdef RVM_I2C_SWE
  #include "i2c/i2c_pool_size.h"
  #define RVF_I2C_POOL_SIZE  (I2C_POOL_SIZE)
#else
  #define RVF_I2C_POOL_SIZE  (0)
#endif


/*
 * IMG SWE
 */
#ifdef RVM_IMG_SWE
  #include "img/inc/img_pool_size.h"
  #define RVF_IMG_POOL_SIZE  (IMG_POOL_SIZE)
  #define RVF_IMG_INT_POOL_SIZE  (IMG_INT_POOL_SIZE)
#else
  #define RVF_IMG_POOL_SIZE  (0)
  #define RVF_IMG_INT_POOL_SIZE  (0)
#endif

/*
 * CAMA SWE
 */
#ifdef RVM_CAMA_SWE
  #include "cama/cama_pool_size.h"
  #define RVF_CAMA_POOL_SIZE  (CAMA_POOL_SIZE)
  #define RVF_CAMA_INT_POOL_SIZE  (CAMA_INT_POOL_SIZE)
#else
  #define RVF_CAMA_POOL_SIZE  (0)
  #define RVF_CAMA_INT_POOL_SIZE  (0)
#endif


/*
 * KPD SWE
 */
#ifdef RVM_KPD_SWE
  #include "kpd/kpd_pool_size.h"
  #define RVF_KPD_POOL_SIZE  (KPD_POOL_SIZE)
#else
  #define RVF_KPD_POOL_SIZE  (0)
#endif

/*
 * LLS SWE
 */
#ifdef RVM_LLS_SWE
  #include "lls/lls_pool_size.h"
  #define RVF_LLS_POOL_SIZE (LLS_POOL_SIZE)
#else
  #define RVF_LLS_POOL_SIZE (0)
#endif

/*
 * MDC SWE
 */
#ifdef RVM_MDC_SWE
  #include "mdc/mdc_pool_size.h"
  #define RVF_MDC_POOL_SIZE  (MDC_POOL_SIZE)
#else
  #define RVF_MDC_POOL_SIZE  (0)
#endif

/*
 * MDL SWE
 */
#ifdef RVM_MDL_SWE
  #include "mdl/mdl_pool_size.h"
  #define RVF_MDL_POOL_SIZE  (MDL_POOL_SIZE)
#else
  #define RVF_MDL_POOL_SIZE  (0)
#endif

/* 
 * MFW SWE
 */
#ifdef RVM_MFW_SWE
  #include "mfw/mfw_pool_size.h"
  #define RVF_MFW_POOL_SIZE  (MFW_POOL_SIZE)
#else
  #define RVF_MFW_POOL_SIZE  (0)
#endif

/*
 * MKS SWE
 */
#ifdef RVM_MKS_SWE
  #include "mks/mks_pool_size.h"
  #define RVF_MKS_POOL_SIZE  (MKS_POOL_SIZE)
#else
  #define RVF_MKS_POOL_SIZE  (0)
#endif


/*	
 * MPM SWE
 */
#ifdef RVM_MPM_SWE
  #include "mpm/mpm_pool_size.h"
  #define RVF_MPM_POOL_SIZE  (MPM_POOL_SIZE)
#else
  #define RVF_MPM_POOL_SIZE  (0)
#endif


/* 
 * USB SWE (USB LLD SWE)
 */
#ifdef RVM_USB_SWE
  #include "usb/usb_pool_size.h"
  #define RVF_USB_POOL_SIZE  (USB_POOL_SIZE)
#else
  #define RVF_USB_POOL_SIZE  (0)
#endif

/* 
 * USBTRC SWE (USBTRC Class SWE)
 */
#ifdef RVM_USBTRC_SWE
  #include "usbtrc/usbtrc_pool_size.h"
  #define RVF_USBTRC_POOL_SIZE (USBTRC_POOL_SIZE)
#else
  #define RVF_USBTRC_POOL_SIZE  (0)
#endif

/* 
 * USBMS SWE (USBMS SWE)
 */
#ifdef RVM_USBMS_SWE
  #include "usbms/usbms_pool_size.h"
  #define RVF_USBMS_POOL_SIZE  (USBMS_POOL_SIZE)
  #define RVF_USBMS_INT_POOL_SIZE  (USBMS_INT_POOL_SIZE)
#else
  #define RVF_USBMS_POOL_SIZE  (0)
  #define RVF_USBMS_INT_POOL_SIZE  (0)
#endif

/* 
 * USBFAX SWE (USB FAX/DATA Class SWE)
 */
#ifdef RVM_USBFAX_SWE
  #include "usbfax/usbfax_pool_size.h"
  #define RVF_USBFAX_POOL_SIZE (USBFAX_POOL_SIZE)
#else
  #define RVF_USBFAX_POOL_SIZE  (0)
#endif


/*
 * MC SWE
 */
#ifdef RVM_MC_SWE
  #include "mc/mc_pool_size.h"
  #define RVF_MC_POOL_SIZE  (MC_POOL_SIZE)
#else
   #define RVF_MC_POOL_SIZE  (0)
#endif



/*
 * DATALIGHT SWE
 */
#ifdef RVM_DATALIGHT_SWE
  #include "datalight/datalight_pool_size.h"
  #define RVF_DATALIGHT_POOL_SIZE  (DATALIGHT_POOL_SIZE)
#else
   #define RVF_DATALIGHT_BM_POOL_SIZE  (0)

/*
 * NAN SWE
 */
#ifdef RVM_NAN_SWE
  #include "nan/nan_pool_size.h"
  #define RVF_NAN_POOL_SIZE  (NAN_BM_POOL_SIZE)
#else
   #define RVF_NAN_POOL_SIZE  (0)
#endif

/*
 * NOR_BM SWE
 */
#ifdef RVM_NOR_BM_SWE
  #include "nor_bm/nor_bm_pool_size.h"
  #define RVF_NOR_BM_POOL_SIZE  (NOR_BM_POOL_SIZE)
#else
   #define RVF_NOR_BM_POOL_SIZE  (0)
#endif

#endif



/* 
 * GBI SWE (Generic Block Interface)
 */
#ifdef RVM_GBI_SWE
  #include "gbi/gbi_pool_size.h"
  #define RVF_GBI_POOL_SIZE  (GBI_POOL_SIZE)
#else
  #define RVF_GBI_POOL_SIZE  (0)
#endif


/* 
 * RFSFAT SWE (FAT16 File System Core)
 */
#ifdef RVM_RFSFAT_SWE
  #include "rfsfat/rfsfat_pool_size.h"
  #define RVF_RFSFAT_POOL_SIZE  (RFSFAT_POOL_SIZE)
#else
  #define RVF_RFSFAT_POOL_SIZE  (0)
#endif

/*
 * RFSNAND SWE
 */
#ifdef RVM_RFSNAND_SWE
  #include "rfsnand/rfsnand_pool_size.h"
  #define RVF_RFSNAND_POOL_SIZE  (RFSNAND_POOL_SIZE)
#else
   #define RVF_RFSNAND_POOL_SIZE  (0)
#endif


/*
 * RFS SWE
 */
#ifdef RVM_RFS_SWE
  #include "rfs/rfs_pool_size.h"
  #define RVF_RFS_POOL_SIZE  (RFS_POOL_SIZE)
#else
   #define RVF_RFS_POOL_SIZE  (0)
#endif


/*
 * PWR SWE
 */
#ifdef RVM_PWR_SWE
  #include "pwr/pwr_pool_size.h"
  #define RVF_PWR_POOL_SIZE (PWR_POOL_SIZE)
#else
  #define RVF_PWR_POOL_SIZE (0)
#endif

/*
 * LCC SWE
 */
#ifdef RVM_LCC_SWE
  #include "lcc/lcc_pool_size.h"
  #define RVF_LCC_POOL_SIZE (LCC_POOL_SIZE)
#else
  #define RVF_LCC_POOL_SIZE (0)
#endif

/*
 * R2D SWE
 */
#ifdef RVM_R2D_SWE
  #include "r2d/r2d_pool_size.h"
  #define RVF_R2D_POOL_SIZE    (R2D_POOL_SIZE)
#else
  #define RVF_R2D_POOL_SIZE    (0)
#endif


/*
 * LCD SWE
 */
#ifdef RVM_LCD_SWE
  #include "lcd/lcd_pool_size.h"
  #define RVF_LCD_POOL_SIZE    (LCD_POOL_SIZE)
#else
  #define RVF_LCD_POOL_SIZE    (0)
#endif



/*
 * RGUI SWE
 */
#ifdef RVM_RGUI_SWE
  #include "rgui/rgui_pool_size.h"
  #define RVF_RGUI_POOL_SIZE  (RGUI_POOL_SIZE)
#else
  #define RVF_RGUI_POOL_SIZE  (0)
#endif

/*
 * RNET SWE
 */
#ifdef RVM_RNET_SWE
  #include "rnet/rnet_pool_size.h"
  #define RVF_RNET_POOL_SIZE (RNET_POOL_SIZE)
#else
  #define RVF_RNET_POOL_SIZE (0)
#endif

/*
 * RNET_BR SWE
 */
#ifdef RVM_RNET_BR_SWE
  #include "rnet/rnet_br/rnet_br_pool_size.h"
  #define RVF_RNET_BR_POOL_SIZE (RNET_BR_POOL_SIZE)
#else
  #define RVF_RNET_BR_POOL_SIZE (0)
#endif

/*
 * RNET_RT SWE
 */
#ifdef RVM_RNET_RT_SWE
  #include "rnet/rnet_rt/rnet_rt_pool_size.h"
  #define RVF_RNET_RT_POOL_SIZE  (RNET_RT_POOL_SIZE)
#else
  #define RVF_RNET_RT_POOL_SIZE  (0)
#endif

/*
 * RNET_WS SWE
 */
#ifdef RVM_RNET_WS_SWE
  #include "rnet/rnet_ws/rnet_ws_pool_size.h"
  #define RVF_RNET_WS_POOL_SIZE  (RNET_WS_POOL_SIZE)
#else
  #define RVF_RNET_WS_POOL_SIZE  (0)
#endif

/*
 * RTC SWE
 */
#ifdef RVM_RTC_SWE
  #include "rtc/rtc_pool_size.h"
  #define RVF_RTC_POOL_SIZE  (RTC_POOL_SIZE)
#else
  #define RVF_RTC_POOL_SIZE  (0)
#endif

/*
 * RTEST SWE
 */
#ifdef RVM_RTEST_SWE
  #include "tests/rtest/rtest_pool_size.h"
  #define RVF_RTEST_POOL_SIZE  (RTEST_POOL_SIZE)
  #define RVF_RTEST_INT_POOL_SIZE  (RTEST_INT_POOL_SIZE)
#else
  #define RVF_RTEST_POOL_SIZE  (0)
  #define RVF_RTEST_INT_POOL_SIZE  (0)
#endif

/* 
 * SMBS SWE
 */
#ifdef RVM_SMBS_SWE
  #include "smbs/smbs_pool_size.h"
  #define RVF_SMBS_POOL_SIZE  (SMBS_POOL_SIZE)
#else
  #define RVF_SMBS_POOL_SIZE  (0)
#endif

/*
 * SPI SWE
 */
#ifdef RVM_SPI_SWE
  #include "spi/spi_pool_size.h"
  #define RVF_SPI_POOL_SIZE  (SPI_POOL_SIZE)
#else
  #define RVF_SPI_POOL_SIZE  (0)
#endif

/*
 * TI_PRF SWE
 */
#ifdef RVM_TI_PRF_SWE
  #include "rvt/ti_profiler/ti_prf_pool_size.h"
  #define RVF_TI_PRF_POOL_SIZE  (TI_PRF_POOL_SIZE)
#else
  #define RVF_TI_PRF_POOL_SIZE  (0)
#endif

/*
 * TRACE SWE
 */
#ifdef RVM_RVT_SWE
  #include "rvt/rvt_pool_size.h"
  #define RVF_TRACE_POOL_SIZE  (TRACE_POOL_SIZE)
#else
  #define RVF_TRACE_POOL_SIZE  (0)
#endif

/*
 * TUT SWE
 */
#ifdef RVM_TUT_SWE
  #include "tut/tut_pool_size.h"
  #define RVF_TUT_POOL_SIZE  (TUT_POOL_SIZE)
#else
  #define RVF_TUT_POOL_SIZE  (0)
#endif

/*
 * TTY SWE
 */
#ifdef RVM_TTY_SWE
  #include "tty/tty_pool_size.h"
  #define RVF_TTY_POOL_SIZE  (TTY_POOL_SIZE)
#else
  #define RVF_TTY_POOL_SIZE  (0)
#endif



/*** Bluetooth SWE (to move in a widcomm/widcomm_pool_size.h) ***/

/* 
 * BTA SWE (Bluetooth SWE)
 */
#ifdef RVM_BTA_SWE
  #include "widcomm/bta/bta_pool_size.h"
  #define RVF_BTA_POOL_SIZE  (BTA_POOL_SIZE)
#else
  #define RVF_BTA_POOL_SIZE  (0)
#endif

/* 
 * BTH SWE (Bluetooth SWE)
 */
#ifdef RVM_BTH_SWE
  #include "widcomm/bth/bth_pool_size.h"
  #define RVF_BTH_POOL_SIZE  (BTU_POOL_SIZE)
#else
  #define RVF_BTH_POOL_SIZE  (0)
#endif

/* 
 * BTU SWE (Bluetooth SWE)
 */
#ifdef RVM_BTU_SWE
  #include "widcomm/btu/btu_pool_size.h"
  #define RVF_BTU_POOL_SIZE  (BTU_POOL_SIZE)
#else
  #define RVF_BTU_POOL_SIZE  (0)
#endif

/* 
 * BTUI SWE (Bluetooth SWE)
 */
#ifdef RVM_BTUI_SWE
  #include "widcomm/btui/btui_pool_size.h"
  #define RVF_BTUI_POOL_SIZE  (BTUI_POOL_SIZE)
#else
  #define RVF_BTUI_POOL_SIZE  (0)
#endif

/* 
 * GKI SWE (Bluetooth SWE)
 */
#ifdef RVM_GKI_SWE
  #include "widcomm/gki/gki_pool_size.h"
  #define RVF_GKI_POOL_SIZE  (GKI_POOL_SIZE)
#else
  #define RVF_GKI_POOL_SIZE  (0)
#endif

/* 
 * JPEG SWE (JPEG SWE)
 */
#ifdef RVM_JPEG_SWE
  #include "jpeg_pool_size.h"
  #define RVF_JPEG_POOL_SIZE  (JPEG_POOL_SIZE)
  #define RVF_JPEG_INT_POOL_SIZE  (JPEG_INT_POOL_SIZE)
#else
  #define RVF_JPEG_POOL_SIZE  (0)
  #define RVF_JPEG_INT_POOL_SIZE  (0)
#endif



/*
 * HCI SWE (Bluetooth SWE)
 */
#ifdef RVM_HCI_SWE
  #include "widcomm/hci/hci_pool_size.h"
  #define RVF_HCI_POOL_SIZE  (HCI_POOL_SIZE)
#else
  #define RVF_HCI_POOL_SIZE  (0)
#endif

/* 
 * RPC SWE (Bluetooth SWE)
 */
#ifdef RVM_RPC_SWE
  #include "widcomm/rpc/rpc_pool_size.h"
  #define RVF_RPC_POOL_SIZE  (RPC_POOL_SIZE)
#else
  #define RVF_RPC_POOL_SIZE  (0)
#endif

#define WIDCOMM_POOL_SIZE (0 + \
						   RVF_BTA_POOL_SIZE + \
						   RVF_BTU_POOL_SIZE + \
						   RVF_BTUI_POOL_SIZE + \
						   RVF_BTH_POOL_SIZE + \
						   RVF_GKI_POOL_SIZE + \
						   RVF_RPC_POOL_SIZE )
#define RVF_WIDCOMM_POOL_SIZE  WIDCOMM_POOL_SIZE 


/* RVM SWE; This SWE is of course always enabled. */
#include "rvm/rvm_pool_size.h"



/// Size of the pool of internal memory.
#define RVF_POOL_INTERNAL_MEM_SIZE (RVF_AUDIO_INT_POOL_SIZE     + \
                                    RVF_AUDIO_BGD_INT_POOL_SIZE + \
                                    RVF_AS_INT_POOL_SIZE        + \
                                    RVF_IMG_INT_POOL_SIZE       + \
                                    RVF_DMA_INT_POOL_SIZE       + \
                                    RVF_DMG_INT_POOL_SIZE       + \
                                    RVF_BAE_INT_POOL_SIZE       + \
                                    RVF_CAMA_INT_POOL_SIZE      + \
                                    RVF_CAMD_INT_POOL_SIZE      + \
                                    RVF_USBMS_INT_POOL_SIZE     + \
                                    RVF_RVM_INT_POOL_SIZE       + \
									RVF_JPEG_INT_POOL_SIZE		+ \
                                    RVF_RTEST_INT_POOL_SIZE)

/// The value of internal memory moved to external one, if no internal memory allocation allowed.
#if RVF_ALLOC_INT_RAM == 1
  #define RVF_POOL_EXTERNAL_MOVE_SIZE (0)
#else
  #define RVF_POOL_EXTERNAL_MOVE_SIZE (RVF_POOL_INTERNAL_MEM_SIZE)
#endif


/// Size of the pool of external memory.
#define RVF_POOL_EXTERNAL_MEM_SIZE (RVF_ATP_POOL_SIZE      + \
                                    RVF_ATP_UART_POOL_SIZE + \
                                    RVF_AUDIO_POOL_SIZE    + \
                                    RVF_AUDIO_BGD_POOL_SIZE + \
                                    RVF_AS_POOL_SIZE       + \
                                    RVF_DACI_POOL_SIZE     + \
                                    RVF_DAR_POOL_SIZE      + \
                                    RVF_DCFG_POOL_SIZE     + \
                                    RVF_DCM_POOL_SIZE      + \
                                    RVF_DMG_POOL_SIZE      + \
                                    RVF_DMA_POOL_SIZE      + \
                                    RVF_I2C_POOL_SIZE      + \
                                    RVF_CAMD_POOL_SIZE     + \
                                    RVF_CAMA_POOL_SIZE     + \
                                    RVF_ETM_POOL_SIZE      + \
                                    RVF_FFS_POOL_SIZE      + \
                                    RVF_HASH_POOL_SIZE     + \
                                    RVF_IMG_POOL_SIZE      + \
                                    RVF_KPD_POOL_SIZE      + \
                                    RVF_LLS_POOL_SIZE      + \
                                    RVF_MDC_POOL_SIZE      + \
                                    RVF_MDL_POOL_SIZE      + \
                                    RVF_MFW_POOL_SIZE      + \
                                    RVF_MKS_POOL_SIZE      + \
                                    RVF_MPM_POOL_SIZE      + \
                                    RVF_USB_POOL_SIZE      + \
                                    RVF_USBFAX_POOL_SIZE   + \
                                    RVF_USBTRC_POOL_SIZE   + \
                                    RVF_USBMS_POOL_SIZE    + \
                                    RVF_RFS_POOL_SIZE      + \
                                    RVF_RFSFAT_POOL_SIZE   + \
                                    RVF_RFSNAND_POOL_SIZE  + \
                                    RVF_GBI_POOL_SIZE      + \
                                    RVF_MC_POOL_SIZE       + \
                                    RVF_NAN_POOL_SIZE      + \
				    RVF_NOR_BM_POOL_SIZE   + \
				    RVF_DATALIGHT_POOL_SIZE   + \
                                    RVF_R2D_POOL_SIZE      + \
                                    RVF_RGUI_POOL_SIZE     + \
                                    RVF_RNET_POOL_SIZE     + \
                                    RVF_RNET_BR_POOL_SIZE  + \
                                    RVF_RNET_RT_POOL_SIZE  + \
                                    RVF_RNET_WS_POOL_SIZE  + \
                                    RVF_RTC_POOL_SIZE      + \
                                    RVF_SMBS_POOL_SIZE     + \
                                    RVF_SPI_POOL_SIZE      + \
                                    RVF_TI_PRF_POOL_SIZE   + \
                                    RVF_TRACE_POOL_SIZE    + \
                                    RVF_TTY_POOL_SIZE      + \
                                    RVF_TUT_POOL_SIZE      + \
                                    RVF_WIDCOMM_POOL_SIZE  + \
                                    RVF_BAE_POOL_SIZE      + \
                                    RVF_BPR_POOL_SIZE      + \
                                    RVF_RVM_POOL_SIZE      + \
                                    RVF_RTEST_POOL_SIZE    + \
									RVF_JPEG_POOL_SIZE	   + \
									RVF_PWR_POOL_SIZE	   + \
									RVF_LCC_POOL_SIZE	   + \
                                    RVF_POOL_EXTERNAL_MOVE_SIZE)


/**
 * Define the memory usage ratio
 * e.g. the ratio between the sum of the memory banks sizes and the memory available in the system
 * for example: 90 for 90 % to be sure to always satisfy memory requests
 *              120 for 120 % to optimize memory usage
 */
#define RVF_MEM_USAGE_RATIO     100

#endif // __RVF_POOL_SIZE_H_
