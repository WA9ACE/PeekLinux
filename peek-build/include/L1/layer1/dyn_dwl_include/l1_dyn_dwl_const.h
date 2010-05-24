/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1_DYN_DWL_CONST.H
 *
 *        Filename l1_dyn_dwl_const.h.h
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#if (L1_DYN_DSP_DWNLD == 1)

#ifndef _L1_DYN_DWL_CONST_H_
#define _L1_DYN_DWL_CONST_H_

/* DSP dynamic download background task id */
#define C_BGD_DSP_DYN_DWNLD   10
#define RED                                1
#define GREEN                              0

#if ((CODE_VERSION == SIMULATION) || (CHIPSET == 12) || (CHIPSET == 15))
  #define MAX_NUM_OF_PATCH_IDS               7
  #define MAX_NUM_OF_SEMAPHORES              9
  #define NUM_OF_DYN_DWNLD_PRIMITIVES        10
#elif (CHIPSET == 10 && BOARD == 35)
  #define MAX_NUM_OF_PATCH_IDS               3
  #define MAX_NUM_OF_SEMAPHORES            4
  #define NUM_OF_DYN_DWNLD_PRIMITIVES 4
#endif // (CODE_VERSION == SIMULATION) || (CHIPSET == 10))

#define NUM_WORDS_COPY_API                 256     // even value mandatory
#define NUM_PRIM_DYN_DWNLD                 8
#define START_API_DWNLD_AREA               0x1808  // 0x1808
#define SIZE_API_DWNLD_AREA                0x7F8   // 0x800
#define START_API_DWNLD_AREA_DURING_E2     0x10BE  // 0x10BE
#define SIZE_API_DWNLD_AREA_DURING_E2      0x410   // 0x410
#define MCU_API_BASE_ADDRESS               0xFFD00000L
#define DSP_API_BASE_ADDRESS               0x800


/* Dynamic Download API base address */
#define C_DYN_DWNLD_API_BASE_ADDRESS       0x17F6
#define HEADER_PATCH_SIZE                  4

#if(CODE_VERSION == SIMULATION)
  #define CRC_SIMU_OK           0xCAFE
  #define SIZE_DWNLD_AREA_SIMU  2048
#endif

#define TRUE  1
#define FALSE 0


/* Define commands MCU/DSP*/
#define C_DWL_DOWNLOAD_CTRL_DSP_ACK   0
#define C_DWL_DOWNLOAD_CTRL_DOWNLOAD  1
#define C_DWL_DOWNLOAD_CTRL_INSTALL   2
#define C_DWL_DOWNLOAD_CTRL_UNINSTALL 3
#define C_DWL_DOWNLOAD_CTRL_ABORT     4
#define C_DWL_DOWNLOAD_CTRL_INIT      5
#define C_DWL_ERR_RESET               0

/* Patch IDs definitions */

#if ((CODE_VERSION == SIMULATION) || (CHIPSET == 12) || (CHIPSET == 15))
  #define   MP3_PATCH 0
  #define   MMS_PATCH 1
  #define   E2_PATCH 2
  #define   TTY_PATCH 3
  #define   SPEECH_ACOUSTIC_PATCH 4
  #define   AAC_PATCH 5
  #define   PCM_EXTRACTION_PATCH 6
#elif (CHIPSET == 10 && BOARD == 35)
  #define   MMS_PATCH 0
  #define   TTY_PATCH 1
  #define   SPEECH_ACOUSTIC_PATCH 2
#endif
#if ((CODE_VERSION == SIMULATION) || (CHIPSET == 12) || (CHIPSET == 15))
  #define   MP3_STATE_MACHINE 0
  #define   VM_STATE_MACHINE 1
  #define   E2_STATE_MACHINE 2
  #define   ANR_STATE_MACHINE 3
  #define   SR_STATE_MACHINE 4
  #define   TTY_STATE_MACHINE 5
  #define   WCM_STATE_MACHINE 6
  #define   AAC_STATE_MACHINE 7
  #define   PCM_EXTRACTION_STATE_MACHINE 8

  #define   AGC_UL_STATE_MACHINE ANR_STATE_MACHINE
  #define   AGC_DL_STATE_MACHINE ANR_STATE_MACHINE
  #define   DRC_STATE_MACHINE ANR_STATE_MACHINE
  #define   AEC_STATE_MACHINE ANR_STATE_MACHINE

#elif (CHIPSET == 10 && BOARD == 35)
  #define   VM_STATE_MACHINE 0
  #define   ANR_STATE_MACHINE 1
  #define   SR_STATE_MACHINE 2
  #define   TTY_STATE_MACHINE 3
#endif

#endif  // _L1_DYN_DWL_CONST_H_
#endif // L1_DYN_DSP_DWNLD
