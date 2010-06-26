/****************************************************************************/
/*                                                                          */
/*  Name        rvf_pool_size.h                                             */
/*                                                                          */
/*  Function    this file contains definitions for buffer pool sizes        */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*  Version     0.3                                                         */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  4/19/1999   Create                                                      */
/*  10/27/1999  Remove declaration of ntohs, htons, ntohl, htonl            */
/*              in order to avoid conflict with winsock.h                   */
/*  12/23/1999  Add buffer and memory bank related definition               */
/*  07/12/2000  Implement dynamic memory allocation.                        */
/*  01/13/2002  Extracted buffer pool size info from rvf_target.h           */
/*              to prevent useless full recompilation when those datas      */
/*              are changed                                                 */
/*  10/01/2002  Completely reworked in order to be modular                  */
/*                                                                          */
/*  Author      David Lamy-Charrier (dlamy@tif.ti.com)                      */
/*                                                                          */
/* (C) Copyright 1999 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#ifndef _RVF_POOL_SIZE_H
#define _RVF_POOL_SIZE_H

#ifndef _SIMULATION_
  #include "r2d.cfg"
  #include "rv.cfg"
  #include "debug.cfg"
  #include "board.cfg"
  #include "l1sw.cfg"
  #include "swconfig.cfg"
#endif

#include "rv_defined_swe.h"
#if (defined RVM_MDC_SWE)
  #include "mdc.cfg"
#endif

/*
 * Defines the memory pools used by each Riviera SWE
 *
 * In case of new SWE (i.e. called NEW), use the following template:
 *
 * #ifdef RVM_NEW_SWE
 *
 *   #define RVF_NEW_STACK_SIZE (Stack_size)
 *   #define RVF_NEW_MB1_SIZE   (MB1_size)
 *   #define RVF_NEW_MB2_SIZE   (MB2_size)
 *   #define RVF_NEW_MB3_SIZE   (MB3_size)
 *
 *   #define RVF_NEW_POOL_SIZE  (RVF_NEW_STACK_SIZE + \
 *                               RVF_NEW_MB1_SIZE + RVF_NEW_MB2_SIZE + RVF_NEW_MB3_SIZE)
 * #else
 *   #define RVF_NEW_POOL_SIZE  (0)
 * #endif
 *

 *
 * ATP SWE
 */

#ifdef RVM_ATP_SWE
  /*
   * Values used in atp_config.h
   */
  #define RVF_ATP_MB1_SIZE (5000)
  #define RVF_ATP_POOL_SIZE (RVF_ATP_MB1_SIZE)
#else
  #define RVF_ATP_POOL_SIZE (0)
#endif

/*
 * ATP_UART SWE
 */

#ifdef RVM_ATP_UART_SWE
  /*
   * Values used in atp_uart_env.h
   */
  #define RVF_ATP_UART_STACK_SIZE (1024)
  #define RVF_ATP_UART_MB1_SIZE (1024)
  #define RVF_ATP_UART_POOL_SIZE (RVF_ATP_UART_STACK_SIZE + RVF_ATP_UART_MB1_SIZE)
#else
  #define RVF_ATP_UART_POOL_SIZE (0)
#endif

/*
 * AUDIO SWE and background
 */

#ifdef RVM_AUDIO_MAIN_SWE
  /*
   * Values used in audio_env_i.h
   */
  #define RVF_AUDIO_STACK_SIZE (1280)
  #define RVF_AUDIO_MB1_SIZE (512)
  #define RVF_AUDIO_MB2_SIZE (768)

  #if (GSMLITE)
    #define RVF_AUDIO_MB3_SIZE (0)
  #elif (SPEECH_RECO)
    #define RVF_AUDIO_MB3_SIZE (7382)
  #else
    #define RVF_AUDIO_MB3_SIZE (2048)
  #endif

  #define RVF_AUDIO_POOL_SIZE (RVF_AUDIO_STACK_SIZE + \
                                RVF_AUDIO_MB1_SIZE + RVF_AUDIO_MB2_SIZE + RVF_AUDIO_MB3_SIZE)
#else
  #define RVF_AUDIO_POOL_SIZE (0)
#endif

#ifdef RVM_AUDIO_BGD_SWE
  /*
   * Values used in audio_env_i.h
   */
  #define RVF_AUDIO_BGD_STACK_SIZE (512)
  #define RVF_AUDIO_BGD_MB1_SIZE (256)

  #define RVF_AUDIO_BGD_POOL_SIZE (RVF_AUDIO_BGD_STACK_SIZE + \
                                           RVF_AUDIO_BGD_MB1_SIZE)
#else
  #define RVF_AUDIO_BGD_POOL_SIZE (0)
#endif

/*
 * BTCTRL SWE
 */

#ifdef RVM_BTCTRL_SWE
  /*
   * Values used in btctrl_env.h
   */
  #define RVF_BTCTRL_STACK_SIZE (2000)
  #define RVF_BTCTRL_MB1_SIZE (5000)
  #define RVF_BTCTRL_POOL_SIZE (RVF_BTCTRL_STACK_SIZE + RVF_BTCTRL_MB1_SIZE)
#else
  #define RVF_BTCTRL_POOL_SIZE (0)
#endif

/*
 * DAR SWE
 */

#ifdef RVM_DAR_SWE
  /*
   * Values used in dar_env.h
   */
  #define RVF_DAR_STACK_SIZE (1024)
  #define RVF_DAR_MB1_SIZE (512)
  #define RVF_DAR_POOL_SIZE (RVF_DAR_STACK_SIZE + RVF_DAR_MB1_SIZE)
#else
  #define RVF_DAR_POOL_SIZE (0)
#endif

/*
 * DEV1 SWE
 */

#ifdef RVM_DEV1_SWE
  /*
   * Values used in dev1_env.h
   */
  #define RVF_DEV1_STACK_SIZE (2000)
  #define RVF_DEV1_MB1_SIZE (5000)
  #define RVF_DEV1_POOL_SIZE (RVF_DEV1_STACK_SIZE + RVF_DEV1_MB1_SIZE)
#else
  #define RVF_DEV1_POOL_SIZE (0)
#endif

/*
 * DEV2 SWE
 */

#ifdef RVM_DEV2_SWE
  /*
   * Values used in dev2_env.h
   */
  #define RVF_DEV2_STACK_SIZE (2000)
  #define RVF_DEV2_MB1_SIZE (5000)
  #define RVF_DEV2_POOL_SIZE (RVF_DEV2_STACK_SIZE + RVF_DEV2_MB1_SIZE)
#else
  #define RVF_DEV2_POOL_SIZE (0)
#endif

/*
 * DEV3 SWE
 */

#ifdef RVM_DEV3_SWE
  /*
   * Values used in dev3_env.h
   */
  #define RVF_DEV3_STACK_SIZE (2000)
  #define RVF_DEV3_MB1_SIZE (5000)
  #define RVF_DEV3_POOL_SIZE (RVF_DEV3_STACK_SIZE + RVF_DEV3_MB1_SIZE)
#else
  #define RVF_DEV3_POOL_SIZE (0)
#endif

/*
 * DUN_GW SWE
 */

#ifdef RVM_DUN_GW_SWE
  /*
   * Values used in dun_GW_env.h
   */
  #define RVF_DUN_GW_STACK_SIZE (1024)
  #define RVF_DUN_GW_MB1_SIZE (1024)
  #define RVF_DUN_GW_MB2_SIZE (4096)
  #define RVF_DUN_GW_MB3_SIZE (4096)
  #define RVF_DUN_GW_POOL_SIZE (RVF_DUN_GW_STACK_SIZE + \
                                 RVF_DUN_GW_MB1_SIZE + RVF_DUN_GW_MB2_SIZE + RVF_DUN_GW_MB3_SIZE)
#else
  #define RVF_DUN_GW_POOL_SIZE (0)
#endif

/*
 * ETM SWE
 */

#ifdef RVM_ETM_SWE
  /*
   * Values used in etm_env.h
   */
  #define RVF_ETM_STACK_SIZE (1024)
  #define RVF_ETM_MB1_SIZE (2048)
  #define RVF_ETM_POOL_SIZE (RVF_ETM_STACK_SIZE + RVF_ETM_MB1_SIZE)
#else
  #define RVF_ETM_POOL_SIZE (0)
#endif

/*
 * EXPL SWE
 */

#ifdef RVM_EXPL_SWE
  /*
   * Values used in expl_env.h
   */
  #define RVF_EXPL_STACK_SIZE (2000)
  #define RVF_EXPL_MB1_SIZE (5000)
  #define RVF_EXPL_POOL_SIZE (RVF_EXPL_STACK_SIZE + RVF_EXPL_MB1_SIZE)
#else
  #define RVF_EXPL_POOL_SIZE (0)
#endif

/*
 * FAX_GW SWE
 */

#ifdef RVM_FAX_GW_SWE
  /*
   * Values used in fax_GW_env.h
   */
  #define RVF_FAX_GW_STACK_SIZE (1024)
  #define RVF_FAX_GW_MB1_SIZE (1024)
  #define RVF_FAX_GW_MB2_SIZE (4096)
  #define RVF_FAX_GW_MB3_SIZE (4096)
  #define RVF_FAX_GW_POOL_SIZE (RVF_FAX_GW_STACK_SIZE + \
                                 RVF_FAX_GW_MB1_SIZE + RVF_FAX_GW_MB2_SIZE + RVF_FAX_GW_MB3_SIZE)
#else
  #define RVF_FAX_GW_POOL_SIZE (0)
#endif

/*
 * FFS SWE
 */

#ifdef RVM_FFS_SWE
  /*
   * Values used in ffs_env.h
   */
  #define RVF_FFS_STACK_SIZE (512)

  #if (!GSMLITE)
    #define RVF_FFS_MB1_SIZE (8192)
  #else
    #define RVF_FFS_MB1_SIZE (4096)
  #endif

  #define RVF_FFS_POOL_SIZE (RVF_FFS_STACK_SIZE + RVF_FFS_MB1_SIZE)
#else
  #define RVF_FFS_POOL_SIZE (0)
#endif

/*
 * FTP SWE
 */

#ifdef RVM_FTP_SWE
  /*
   * Values used in ftp_env.h
   */
  #define RVF_FTP_STACK_SIZE (4000)
  #define RVF_FTP_MB1_SIZE (5000)
  #define RVF_FTP_MB2_SIZE (5000)
  #define RVF_FTP_MB3_SIZE (5000)
  #define RVF_FTP_POOL_SIZE (RVF_FTP_STACK_SIZE + \
                              RVF_FTP_MB1_SIZE + RVF_FTP_MB2_SIZE + RVF_FTP_MB3_SIZE)
#else
  #define RVF_FTP_POOL_SIZE (0)
#endif

/*
 * HCI SWE
 */

#ifdef RVM_HCI_SWE
  /*
   * Values used in hci_env.h
   */
  #define RVF_HCI_MB1_SIZE (10000)
  #define RVF_HCI_MB2_SIZE (10000)
  #define RVF_HCI_POOL_SIZE (RVF_HCI_MB1_SIZE + RVF_HCI_MB2_SIZE)
#else
  #define RVF_HCI_POOL_SIZE (0)
#endif

/*
 * HS SWE
 */

#ifdef RVM_HS_SWE
  /*
   * Values used in hs_config.h
   */
  #define RVF_HS_STACK_SIZE (1000)
  #define RVF_HS_MB1_SIZE (5000)
  #define RVF_HS_MB2_SIZE (5000)
  #define RVF_HS_MB3_SIZE (5000)
  #define RVF_HS_POOL_SIZE (RVF_HS_STACK_SIZE + \
                             RVF_HS_MB1_SIZE + RVF_HS_MB2_SIZE + RVF_HS_MB3_SIZE)
#else
  #define RVF_HS_POOL_SIZE (0)
#endif

/*
 * HSG SWE
 */

#ifdef RVM_HSG_SWE
  /*
   * Values used in hsg_config.h
   */
  #define RVF_HSG_STACK_SIZE (1000)
  #define RVF_HSG_MB1_SIZE (5000)
  #define RVF_HSG_MB2_SIZE (5000)
  #define RVF_HSG_MB3_SIZE (5000)
  #define RVF_HSG_POOL_SIZE (RVF_HSG_STACK_SIZE + \
                              RVF_HSG_MB1_SIZE + RVF_HSG_MB2_SIZE + RVF_HSG_MB3_SIZE)
#else
  #define RVF_HSG_POOL_SIZE (0)
#endif

/*
 * KCL SWE
 */

#ifdef RVM_KCL_SWE
  /*
   * Values used in kcl_env.h
   */
  #define RVF_KCL_MB1_SIZE (500)
  #define RVF_KCL_POOL_SIZE (RVF_KCL_MB1_SIZE)
#else
  #define RVF_KCL_POOL_SIZE (0)
#endif

/*
 * KGC SWE
 */

#ifdef RVM_KGC_SWE
  /*
   * Values used in kgc_env.h
   */
  #define RVF_KGC_MB1_SIZE (500)
  #define RVF_KGC_POOL_SIZE (RVF_KGC_MB1_SIZE)
#else
  #define RVF_KGC_POOL_SIZE (0)
#endif

/*
 * KIL SWE
 */

#ifdef RVM_KIL_SWE
  /*
   * Values used in kil_env.h
   */
  #define RVF_KIL_MB1_SIZE (500)
  #define RVF_KIL_POOL_SIZE (RVF_KIL_MB1_SIZE)
#else
  #define RVF_KIL_POOL_SIZE (0)
#endif

/*
 * KMM SWE
 */

#ifdef RVM_KMM_SWE
  /*
   * Values used in kmm_env.h
   */
  #define RVF_KMM_MB1_SIZE (500)
  #define RVF_KMM_POOL_SIZE (RVF_KMM_MB1_SIZE)
#else
  #define RVF_KMM_POOL_SIZE (0)
#endif

/*
 * KNM SWE
 */

#ifdef RVM_KNM_SWE
  /*
   * Values used in knm_env.h
   */
  #define RVF_KNM_MB1_SIZE (500)
  #define RVF_KNM_POOL_SIZE (RVF_KNM_MB1_SIZE)
#else
  #define RVF_KNM_POOL_SIZE (0)
#endif

/*
 * KPD SWE
 */

#ifdef RVM_KPD_SWE
  /*
   * Values used in kpd_env.h
   */
  #define RVF_KPD_STACK_SIZE (1000)

  #if ((BOARD == 7) || (BOARD == 8) || (BOARD == 9))
    #define RVF_KPD_MB1_SIZE (716)
  #elif ((BOARD == 34) || (BOARD == 40) || (BOARD == 41) || (BOARD == 42) || (BOARD == 43))
    #define RVF_KPD_MB1_SIZE (788)
  #elif defined _WINDOWS
    #define RVF_KPD_MB1_SIZE (788)
  #endif /* BOARD */

  #define RVF_KPD_POOL_SIZE (RVF_KPD_STACK_SIZE + RVF_KPD_MB1_SIZE)
#else
  #define RVF_KPD_POOL_SIZE (0)
#endif

/*
 * KPG SWE
 */

#ifdef RVM_KPG_SWE
  /*
   * Values used in kpg_env.h
   */
  #define RVF_KPG_MB1_SIZE (500)
  #define RVF_KPG_POOL_SIZE (RVF_KPG_MB1_SIZE)
#else
  #define RVF_KPG_POOL_SIZE (0)
#endif

/*
 * KZP SWE
 */

#ifdef RVM_KZP_SWE
  /*
   * Values used in kzp_env.h
   */
  #define RVF_KZP_MB1_SIZE (500)
  #define RVF_KZP_POOL_SIZE (RVF_KZP_MB1_SIZE)
#else
  #define RVF_KZP_POOL_SIZE (0)
#endif

/*
 * L2CAP SWE
 */

#ifdef RVM_L2CAP_SWE
  /*
   * Values used in l2cap_env.h
   */
  #define RVF_L2CAP_STACK_SIZE (1024)
  #define RVF_L2CAP_MB1_SIZE (3072)
  #define RVF_L2CAP_MB2_SIZE (2048)
  #define RVF_L2CAP_MB3_SIZE (1024)
  #define RVF_L2CAP_POOL_SIZE (RVF_L2CAP_STACK_SIZE + \
                                RVF_L2CAP_MB1_SIZE + RVF_L2CAP_MB2_SIZE + RVF_L2CAP_MB3_SIZE)
#else
  #define RVF_L2CAP_POOL_SIZE (0)
#endif

/*
 * LLS SWE
 */

#ifdef RVM_LLS_SWE
  /*
   * Values used in lls_env.h
   */
  #define RVF_LLS_MB1_SIZE (50)
  #define RVF_LLS_POOL_SIZE (RVF_LLS_MB1_SIZE)
#else
  #define RVF_LLS_POOL_SIZE (0)
#endif

/*
 * MKS SWE
 */

#ifdef RVM_MKS_SWE
  /*
   * Values used in mks_env.h
   */
  #define RVF_MKS_STACK_SIZE (500)
  #define RVF_MKS_MB1_SIZE (500)
  #define RVF_MKS_POOL_SIZE (RVF_MKS_STACK_SIZE + RVF_MKS_MB1_SIZE)
#else
  #define RVF_MKS_POOL_SIZE (0)
#endif

/*
 * MPM SWE
 */

#ifdef RVM_MPM_SWE
  /*
   * Values used in mpm_env.h
   */
  #define RVF_MPM_STACK_SIZE (2000)
  #define RVF_MPM_MB1_SIZE (100000)
  #define RVF_MPM_POOL_SIZE (RVF_MPM_STACK_SIZE + RVF_MPM_MB1_SIZE)
#else
  #define RVF_MPM_POOL_SIZE (0)
#endif

/*
 * OBX SWE
 */

#ifdef RVM_OBX_SWE
  /*
   * Values used in obx_env.h
   */
  #define RVF_OBX_STACK_SIZE (2000)
  #define RVF_OBX_MB1_SIZE (3000)
  #define RVF_OBX_MB2_SIZE (2000)
  #define RVF_OBX_MB3_SIZE (1500)
  #define RVF_OBX_POOL_SIZE (RVF_OBX_STACK_SIZE + \
                              RVF_OBX_MB1_SIZE + RVF_OBX_MB2_SIZE + RVF_OBX_MB3_SIZE)
#else
  #define RVF_OBX_POOL_SIZE (0)
#endif

/*
 * OPP SWE
 */

#ifdef RVM_OPP_SWE
  /*
   * Values used in opp_env.h
   */
  #define RVF_OPP_STACK_SIZE (2000)
  #define RVF_OPP_MB1_SIZE (1500)
  #define RVF_OPP_MB2_SIZE (1500)
  #define RVF_OPP_MB3_SIZE (1500)
  #define RVF_OPP_POOL_SIZE (RVF_OPP_STACK_SIZE + \
                              RVF_OPP_MB1_SIZE + RVF_OPP_MB2_SIZE + RVF_OPP_MB3_SIZE)
#else
  #define RVF_OPP_POOL_SIZE (0)
#endif

/*
 * PWR SWE
 */

#ifdef RVM_PWR_SWE
  /*
   * Values used in pwr_env.h
   */
  #define RVF_PWR_MB1_SIZE (1000)
  #define RVF_PWR_POOL_SIZE (RVF_PWR_MB1_SIZE)

#else
  #define RVF_PWR_POOL_SIZE (0)
#endif

/*
 * R2D SWE
 */

#ifdef RVM_R2D_SWE
  /*
   * Values used in r2d_env.h (Stack) & r2d_inits.c (Memory Banks)
   */
  #define RVF_R2D_STACK_SIZE (1000)

  #if (R2D_LCD_TEST == 1)
     #if defined (_WINDOWS)
        #define RVF_R2D_MB1_SIZE (4000)
     #else
        #define RVF_R2D_MB1_SIZE (3000)
     #endif
  #elif ((BOARD == 7) || (BOARD == 8) || (BOARD == 9))
  /* #if ((R2D_EMBEDDED_LCD == R2D_MIRRORED_LCD) || (R2D_EMBEDDED_LCD == R2D_SIMPLE_LCD)) */
    #define RVF_R2D_MB1_SIZE (2000) /* B or C-Sample with B&W LCD (default setting) */
  /* #elif (R2D_EMBEDDED_LCD == R2D_BOARD_COLOR_LCD) */
    /* #define RVF_R2D_MB1_SIZE (3000) */ /* C-Sample with Color LCD */
  #elif ((BOARD == 40) || (BOARD == 41) || (BOARD == 42) || (BOARD == 43))
  /* #elif ((R2D_EMBEDDED_LCD == R2D_BOARD_DSAMPLE) || (R2D_EMBEDDED_LCD == R2D_BOARD_BW_DSAMPLE)) */
    #define RVF_R2D_MB1_SIZE (4000) /* D-Sample */
  #elif defined (_WINDOWS)
  /* #elif (R2D_EMBEDDED_LCD == R2D_PC_COLOR_LCD) */
    #define RVF_R2D_MB1_SIZE (4000) /* ColorPC */
  /* #elif (R2D_EMBEDDED_LCD == R2D_PC_CSAMPLE) */
    /* #define RVF_R2D_MB1_SIZE (XXXX) */
  /* #elif (R2D_EMBEDDED_LCD == R2D_PC_DSAMPLE) */
    /* #define RVF_R2D_MB1_SIZE (XXXX) */
  /* #elif (R2D_EMBEDDED_LCD == R2D_CUSTOMER_LCD) */
    /* #define RVF_R2D_MB1_SIZE (XXXX) */
  /* #elif (R2D_EMBEDDED_LCD == R2D_HORIZONTAL_LCD) */
    /* #define RVF_R2D_MB1_SIZE (XXXX) */
  #endif

  #define RVF_R2D_POOL_SIZE (RVF_R2D_STACK_SIZE + RVF_R2D_MB1_SIZE)
#else
  #define RVF_R2D_POOL_SIZE (0)
#endif

/*
 * RFCOMM SWE
 */

#ifdef RVM_RFCOMM_SWE
  /*
   * Values used in rfc_env.h
   */
  #define RVF_RFCOMM_STACK_SIZE (1024)
  #define RVF_RFCOMM_MB1_SIZE (5000)
  #define RVF_RFCOMM_MB2_SIZE (5000)
  /* Memory Bank created dynamically */
  /* #define RVF_RFCOMM_MB3_SIZE (To Be defined) */
  #define RVF_RFCOMM_POOL_SIZE (RVF_RFCOMM_STACK_SIZE + \
                                 RVF_RFCOMM_MB1_SIZE + RVF_RFCOMM_MB2_SIZE)
                                 /* RVF_RFCOMM_MB1_SIZE + RVF_RFCOMM_MB2_SIZE + RVF_RFCOMM_MB3_SIZE) */
#else
  #define RVF_RFCOMM_POOL_SIZE (0)
#endif

/*
 * RGUI SWE
 */

#ifdef RVM_RGUI_SWE
  /*
   * Values used in rgui_env.c
   */
  #define RVF_RGUI_STACK_SIZE (8000)
  #define RVF_RGUI_MB1_SIZE (9000)
  #define RVF_RGUI_POOL_SIZE (RVF_RGUI_STACK_SIZE + RVF_RGUI_MB1_SIZE)
#else
  #define RVF_RGUI_POOL_SIZE (0)
#endif

/*
 * RNET SWE
 */

#ifdef RVM_RNET_SWE
  /*
   * Values used in rnet_env.h
   */
  #define RVF_RNET_MB1_SIZE (500)
  #define RVF_RNET_POOL_SIZE (RVF_RNET_MB1_SIZE)
#else
  #define RVF_RNET_POOL_SIZE (0)
#endif

/*
 * RNET_BR SWE
 */

#ifdef RVM_RNET_BR_SWE
  /*
   * Values used in rnet_br_env.h
   */
  #define RVF_RNET_BR_MB1_SIZE (8000)
  #define RVF_RNET_BR_POOL_SIZE (RVF_RNET_BR_MB1_SIZE)
#else
  #define RVF_RNET_BR_POOL_SIZE (0)
#endif

/*
 * RNET_RT SWE
 */

#ifdef RVM_RNET_RT_SWE
  /*
   * Values used in rnet_rt_env.h
   */
  #define RVF_RNET_RT_STACK_SIZE (2048) //21/02/03 : 2048 initially
  #define RVF_RNET_RT_MB1_BUFPOOL_SIZE (65536)
  #define RVF_RNET_RT_MB1_MSGPOOL_SIZE (512)
  #define RVF_RNET_RT_MB1_VARIABLE_SIZE (30000) // initially 20000
  #define RVF_RNET_RT_POOL_SIZE (RVF_RNET_RT_STACK_SIZE + \
                                  RVF_RNET_RT_MB1_BUFPOOL_SIZE + \
                                  RVF_RNET_RT_MB1_MSGPOOL_SIZE + \
                                  RVF_RNET_RT_MB1_VARIABLE_SIZE)
#else
  #define RVF_RNET_RT_POOL_SIZE (0)
#endif

/*
 * RNET_WS SWE
 */

#ifdef RVM_RNET_WS_SWE
  /*
   * Values used in rnet_ws_env.h
   */
  #define RVF_RNET_WS_STACK_SIZE (2000)
  #define RVF_RNET_WS_MB1_SIZE (5000)
  #define RVF_RNET_WS_POOL_SIZE (RVF_RNET_WS_STACK_SIZE + RVF_RNET_WS_MB1_SIZE)
#else
  #define RVF_RNET_WS_POOL_SIZE (0)
#endif

/*
 * DCM SWE
 */

#ifdef RVM_DCM_SWE
  /*
   * Values used in dcm_env.h
   */

  #define RVF_DCM_STACK_SIZE (1024) // old one : 1024
  #define RVF_DCM_MB1_SIZE (2048)
  #define RVF_DCM_POOL_SIZE (RVF_DCM_STACK_SIZE + RVF_DCM_MB1_SIZE)
#else
  #define RVF_DCM_POOL_SIZE (0)
#endif

/*
 * RTC SWE
 */

#ifdef RVM_RTC_SWE
  /*
   * Values used in rtc_env.h
   */
  #define RVF_RTC_STACK_SIZE (700)
  #define RVF_RTC_MB1_SIZE (100)
  #define RVF_RTC_POOL_SIZE (RVF_RTC_STACK_SIZE + RVF_RTC_MB1_SIZE)
#else
  #define RVF_RTC_POOL_SIZE (0)
#endif

/*
 * RTEST SWE
 */

#ifdef RVM_RTEST_SWE
  /*
   * Values used in rtest_env.h
   */
  #define RVF_RTEST_STACK_SIZE (1024)
  #define RVF_RTEST_MB1_SIZE (190000) // + 550000) //190000
  #define RVF_RTEST_POOL_SIZE (RVF_RTEST_STACK_SIZE + RVF_RTEST_MB1_SIZE)
#else
  #define RVF_RTEST_POOL_SIZE (0)
#endif

/*
 * RVM SWE; This SWE is of course always enabled.
 *
 * Values used in rvm_i.h
 */
#define RVF_RVM_STACK_SIZE (1500)

#if (defined BLUETOOTH) || (defined TEST) || (TI_PROFILER == 1) || (TI_NUC_MONITOR == 1)
  #define RVF_RVM_MB1_SIZE (10000)
#else
   #if (GSMLITE)
      #define RVF_RVM_MB1_SIZE (1000)
   #else
      #define RVF_RVM_MB1_SIZE (3000)
   #endif
#endif
#define RVF_RVM_MB2_SIZE (500)

#if (GSMLITE)
   #define RVF_RVM_MB3_SIZE (2700)
   #define RVF_RVM_MB4_SIZE (0)
   #define RVF_RVM_MB5_SIZE (0)
   #define RVF_RVM_COMPENSATOR_POOL_SIZE (500)
#else
   #define RVF_RVM_MB3_SIZE (6000)
   #define RVF_RVM_MB4_SIZE (3000)
   #define RVF_RVM_MB5_SIZE (3000)
   #define RVF_RVM_COMPENSATOR_POOL_SIZE (12500)
#endif

#define RVF_RVM_POOL_SIZE (RVF_RVM_STACK_SIZE + RVF_RVM_MB1_SIZE + \
                            RVF_RVM_MB2_SIZE + RVF_RVM_MB3_SIZE + \
                            RVF_RVM_MB4_SIZE + RVF_RVM_MB5_SIZE +\
                            RVF_RVM_COMPENSATOR_POOL_SIZE )

/*
 * SDP SWE
 */

#ifdef RVM_SDP_SWE
  /*
   * Values used in sdp_env.h
   */
  #define RVF_SDP_STACK_SIZE (1024)
  #define RVF_SDP_MB1_SIZE (4096)
  #define RVF_SDP_MB2_SIZE (1024)
  #define RVF_SDP_MB3_SIZE (1024)
  #define RVF_SDP_POOL_SIZE (RVF_SDP_STACK_SIZE + \
                              RVF_SDP_MB1_SIZE + RVF_SDP_MB2_SIZE + RVF_SDP_MB3_SIZE)
#else
  #define RVF_SDP_POOL_SIZE (0)
#endif

/*
 * SPI SWE
 */

#ifdef RVM_SPI_SWE
  /*
   * Values used in spi_env.h
   */
  #define RVF_SPI_STACK_SIZE (1000)
  #define RVF_SPI_MB1_SIZE (256)
  #define RVF_SPI_POOL_SIZE (RVF_SPI_STACK_SIZE + RVF_SPI_MB1_SIZE)
#else
  #define RVF_SPI_POOL_SIZE (0)
#endif

/*
 * SPP SWE
 */

#ifdef RVM_SPP_SWE
  /*
   * Values used in spp_env.h
   */
  #define RVF_SPP_STACK_SIZE (1024)
  #define RVF_SPP_MB1_SIZE (5000)
  #define RVF_SPP_POOL_SIZE (RVF_SPP_STACK_SIZE + RVF_SPP_MB1_SIZE)
#else
  #define RVF_SPP_POOL_SIZE (0)
#endif

/*
 * SYN SWE
 */

#ifdef RVM_SYN_SWE
  /*
   * Values used in syn_env.h
   */
  #define RVF_SYN_STACK_SIZE (2000)
  #define RVF_SYN_MB1_SIZE (5000)
  #define RVF_SYN_MB2_SIZE (5000)
  #define RVF_SYN_MB3_SIZE (5000)
  #define RVF_SYN_POOL_SIZE (RVF_SYN_STACK_SIZE + \
                              RVF_SYN_MB1_SIZE + RVF_SYN_MB2_SIZE + RVF_SYN_MB3_SIZE)
#else
  #define RVF_SYN_POOL_SIZE (0)
#endif

/*
 * TI_PRF SWE
 */

#ifdef RVM_TI_PRF_SWE
  /*
   * Values used in ti_profiler_env.h
   */
  #define RVF_TI_PRF_STACK_SIZE (1000)

  #if (TI_NUC_MONITOR == 1)
    #define RVF_TI_PRF_MB1_SIZE (1080000)
  #else
    #define RVF_TI_PRF_MB1_SIZE (400)
  #endif

  #define RVF_TI_PRF_POOL_SIZE (RVF_TI_PRF_STACK_SIZE + RVF_TI_PRF_MB1_SIZE)
#else
  #define RVF_TI_PRF_POOL_SIZE (0)
#endif

/*
 * TRACE SWE
 */

#ifdef RVM_RVT_SWE
  /*
   * Values used in rvt_def_i.h
   */
  #define RVF_TRACE_STACK_SIZE (512)

  #ifdef TEST
    #define RVF_TRACE_MB1_SIZE (25000)
  #elif (!GSMLITE)
    #define RVF_TRACE_MB1_SIZE (5000)
  #else
    #define RVF_TRACE_MB1_SIZE (4000)
  #endif

  #define RVF_TRACE_POOL_SIZE (RVF_TRACE_STACK_SIZE + RVF_TRACE_MB1_SIZE)
#else
  #define RVF_TRACE_POOL_SIZE (0)
#endif

/*
 * TUT SWE
 */

#ifdef RVM_TUT_SWE
  /*
   * Values used in tut_env.h
   */
  #define RVF_TUT_STACK_SIZE (1024)
  #define RVF_TUT_MB1_SIZE (512)
  #define RVF_TUT_POOL_SIZE (RVF_TUT_STACK_SIZE + RVF_TUT_MB1_SIZE)
#else
  #define RVF_TUT_POOL_SIZE (0)
#endif

/*
 * UVM SWE
 */

#ifdef RVM_UVM_SWE
  /*
   * Values used in uvm_env.h
   */
  #define RVF_UVM_STACK_SIZE (20000)
  #define RVF_UVM_MB1_SIZE (350000)
  #define RVF_UVM_MB2_SIZE (800000)
  #define RVF_UVM_MB3_SIZE (250000)
  #define RVF_UVM_POOL_SIZE (RVF_UVM_STACK_SIZE + \
                              RVF_UVM_MB1_SIZE + RVF_UVM_MB2_SIZE + RVF_UVM_MB3_SIZE)
#else
  #define RVF_UVM_POOL_SIZE (0)
#endif

/*
 * JTM SWE
 */

#ifdef RVM_JTM_SWE
  /*
   * Values used in jtm_env.h
   */
  #define RVF_JTM_STACK_SIZE (4096)
  #define RVF_JTM_MB1_SIZE (30000)
  #define RVF_JTM_POOL_SIZE (RVF_JTM_STACK_SIZE + RVF_JTM_MB1_SIZE)
#else
  #define RVF_JTM_POOL_SIZE (0)
#endif

/*
 * MDC SWE
 */

#ifdef RVM_MDC_SWE
  /*
   * Values used in mdc_env.h
   */

  #ifdef MIDI_PLAYER
  #define RVF_MDC_MB1_SIZE (24000)
  #elif defined IMEL_PLAYER
  #define RVF_MDC_MB1_SIZE (4000)
  #elif defined DGP_PLAYER
    #define RVF_MDC_MB1_SIZE (4000)
  #endif

  #define RVF_MDC_POOL_SIZE (RVF_MDC_MB1_SIZE)
#else
  #define RVF_MDC_POOL_SIZE (0)
#endif

/*
 * TTY SWE
 */

#ifdef RVM_TTY_SWE
/*
   * Values used in tty_env.h
   */
  #define RVF_TTY_STACK_SIZE (2000)
  #define RVF_TTY_MB1_SIZE (5300)
  #define RVF_TTY_POOL_SIZE (RVF_TTY_STACK_SIZE + \
                              RVF_TTY_MB1_SIZE)
#else
  #define RVF_TTY_POOL_SIZE (0)
#endif

/*
 * Obigo SWEs
 */
/* MSFE SWE */
#ifdef RVM_MSFE_SWE
  #define RVF_MSFE_STACK_SIZE (512)
  #define RVF_MSFE_MB1_SIZE (25000)
  #define RVF_MSFE_POOL_SIZE (RVF_MSFE_STACK_SIZE + RVF_MSFE_MB1_SIZE)
#else
  #define RVF_MSFE_POOL_SIZE (0)
#endif
/* MSME SWE */
#ifdef RVM_MSME_SWE
  #define RVF_MSME_STACK_SIZE (512)
  #define RVF_MSME_MB1_SIZE (20000)
  #define RVF_MSME_POOL_SIZE (RVF_MSME_STACK_SIZE + RVF_MSME_MB1_SIZE)
#else
  #define RVF_MSME_POOL_SIZE (0)
#endif
/* STKE SWE */
#ifdef RVM_STKE_SWE
  #define RVF_STKE_STACK_SIZE (512)
  #define RVF_STKE_MB1_SIZE (20000)
  #define RVF_STKE_POOL_SIZE (RVF_STKE_STACK_SIZE + RVF_STKE_MB1_SIZE)
#else
  #define RVF_STKE_POOL_SIZE (0)
#endif
/* BRSE SWE */
#ifdef RVM_BRSE_SWE
  #define RVF_BRSE_STACK_SIZE (512)
  #define RVF_BRSE_MB1_SIZE (75000)
  #define RVF_BRSE_POOL_SIZE (RVF_BRSE_STACK_SIZE + RVF_BRSE_MB1_SIZE)
#else
  #define RVF_BRSE_POOL_SIZE (0)
#endif
/* BRAE SWE */
#ifdef RVM_BRAE_SWE
  #define RVF_BRAE_STACK_SIZE (512)
  #define RVF_BRAE_MB1_SIZE (10000)
  #define RVF_BRAE_POOL_SIZE (RVF_BRAE_STACK_SIZE + RVF_BRAE_MB1_SIZE)
#else
  #define RVF_BRAE_POOL_SIZE (0)
#endif
/* IT1E SWE */
#ifdef RVM_IT1E_SWE
  #define RVF_IT1E_STACK_SIZE (512)
  #define RVF_IT1E_MB1_SIZE (512)
  #define RVF_IT1E_POOL_SIZE (RVF_IT1E_STACK_SIZE + RVF_IT1E_MB1_SIZE)
#else
  #define RVF_IT1E_POOL_SIZE (0)
#endif
/* IT2E SWE */
#ifdef RVM_IT2E_SWE
  #define RVF_IT2E_STACK_SIZE (512)
  #define RVF_IT2E_MB1_SIZE (512)
  #define RVF_IT2E_POOL_SIZE (RVF_IT2E_STACK_SIZE + RVF_IT2E_MB1_SIZE)
#else
  #define RVF_IT2E_POOL_SIZE (0)
#endif

/*
 * MFW SWE
 */
#ifdef RVM_MFW_SWE
  /*
   * Values used in mfw_env.h
   */
  #define RVF_MFW_STACK_SIZE (2000)
  #define RVF_MFW_MB1_SIZE (125000)
  #define RVF_MFW_POOL_SIZE (RVF_MFW_STACK_SIZE + \
                              RVF_MFW_MB1_SIZE)
#else
  #define RVF_MFW_POOL_SIZE (0)
#endif

/*
 * SMBS SWE
 */

#ifdef RVM_SMBS_SWE
/*
   * Values used in smbs_env.h
   */
  #define RVF_SMBS_STACK_SIZE (5000)
  #define RVF_SMBS_MB1_SIZE (10000)
  #define RVF_SMBS_POOL_SIZE (RVF_SMBS_STACK_SIZE + \
                               RVF_SMBS_MB1_SIZE)
#else
  #define RVF_SMBS_POOL_SIZE (0)
#endif

/* define the memory pools used by the RVF */

/* define RVF_NB_POOLS to 1 or 2 if you want to use 2 buffers in RAM,
for example 1 in internal RAM, 1 in external RAM */
#define RVF_NB_POOLS 1

/* 500 corresponds to a "hard-coded" value for RVM_STACKS Memory Bank defined in rvm_i.h */
#define RVF_POOL_0_SIZE (RVF_ATP_POOL_SIZE + RVF_ATP_UART_POOL_SIZE + RVF_AUDIO_POOL_SIZE + \
                         RVF_BTCTRL_POOL_SIZE + RVF_DAR_POOL_SIZE + RVF_DEV1_POOL_SIZE + \
                         RVF_DEV2_POOL_SIZE + RVF_DEV3_POOL_SIZE + RVF_DUN_GW_POOL_SIZE + \
                         RVF_ETM_POOL_SIZE + RVF_EXPL_POOL_SIZE + RVF_FAX_GW_POOL_SIZE + \
                         RVF_FFS_POOL_SIZE + RVF_FTP_POOL_SIZE + RVF_HCI_POOL_SIZE + \
                         RVF_HS_POOL_SIZE + RVF_HSG_POOL_SIZE + RVF_KCL_POOL_SIZE + \
                         RVF_KGC_POOL_SIZE + RVF_KIL_POOL_SIZE + RVF_KMM_POOL_SIZE + \
                         RVF_KNM_POOL_SIZE + RVF_KPD_POOL_SIZE + RVF_KPG_POOL_SIZE + \
                         RVF_KZP_POOL_SIZE + RVF_L2CAP_POOL_SIZE + RVF_LLS_POOL_SIZE + \
                         RVF_MKS_POOL_SIZE + RVF_TTY_POOL_SIZE + RVF_MPM_POOL_SIZE + \
                         RVF_OBX_POOL_SIZE + RVF_OPP_POOL_SIZE + RVF_PWR_POOL_SIZE + \
                         RVF_R2D_POOL_SIZE + RVF_RFCOMM_POOL_SIZE + RVF_RGUI_POOL_SIZE + \
                         RVF_RNET_POOL_SIZE + RVF_RNET_BR_POOL_SIZE + RVF_MDC_POOL_SIZE + \
                         RVF_RNET_RT_POOL_SIZE + RVF_RNET_WS_POOL_SIZE + RVF_RTC_POOL_SIZE + \
                         RVF_RTEST_POOL_SIZE + RVF_RVM_POOL_SIZE + RVF_SDP_POOL_SIZE + \
                         RVF_SPI_POOL_SIZE + RVF_SPP_POOL_SIZE + RVF_SYN_POOL_SIZE + \
                         RVF_TI_PRF_POOL_SIZE + RVF_TRACE_POOL_SIZE + RVF_TUT_POOL_SIZE + \
                         RVF_UVM_POOL_SIZE + RVF_JTM_POOL_SIZE + RVF_DCM_POOL_SIZE + \
                         RVF_MSFE_POOL_SIZE + RVF_MSME_POOL_SIZE + RVF_STKE_POOL_SIZE + \
                         RVF_BRSE_POOL_SIZE + RVF_IT1E_POOL_SIZE + RVF_IT2E_POOL_SIZE + \
                         RVF_BRAE_POOL_SIZE + RVF_MFW_POOL_SIZE + RVF_SMBS_POOL_SIZE + \
                         RVF_AUDIO_BGD_POOL_SIZE )

#if (RVF_NB_POOLS > 1 )
    #define RVF_POOL_1_SIZE 0
#endif

/* define the memory usage ratio */
/* e.g. the ratio between the sum of the memory banks sizes and the memory available in the system */
/* for example: 90 for 90 % to be sure to always satisfy memory requests*/
/*              120 for 120 % to optimize memory usage */
#define RVF_MEM_USAGE_RATIO 100

#endif /* _RVF_POOL_SIZE_H */

