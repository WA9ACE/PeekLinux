/**
 *
 * @file	rv_defined_swe.h
 *
 * Definition of the SWE that will be compiled.
 *
 * To include a SWE in the build, the System/env.mak file is used.
 * Before the build, a rv_swe.h file is generated from env.mak.
 * This file includes rv_swe.h and makes some adjustement if necessary
 * (e.g. mandatory SWE to include if some other is included, include
 * of group of SWE, etc).
 *
 * @author	Cristian Livadiotti
 * @version	0.4
 *
 */

/*
 * Revision History:
 *
 * 10/10/2000	Cristian Livadiotti		Create.
 * 10/18/2001   Christophe Favergeon	Updated for R2D.
 * 10/15/2002   Laurent Sollier			Updated for automatic definition from env.mak file.
 * 11/19/2002	Vincent Oberle			Added RivieraTool support to automatic definition.
 *
 * (C) Copyright 2000 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _RV_DEFINED_SWE_H_
#define _RV_DEFINED_SWE_H_

#ifndef _WINDOWS
   #include "config/l1sw.cfg"
   #include "config/rv.cfg"
   #include "config/swconfig.cfg"
   #include "config/debug.cfg"
#endif

/*
 * rv_swe.h is generated before build from System/env.mak
 * It is used both for RivieraTool and Target.
 */
#include "config/rv_swe.h"


/* ------------------------------------------------------------------
 * Grouped SWEs
 * Some define in rv_swe.h might not represent one SWE, but a group
 * of SWEs, this is decoded here.
 * ------------------------------------------------------------------ */


/*
 * J2ME SW entities
 * If RVM_J2ME_K_SWE is defined, the SWE's for the Java KVM are defined.
 */
#ifdef RVM_JAVA_K_SWE
   #define RVM_KGC_SWE
   #define RVM_KIL_SWE
   #define RVM_KCL_SWE
   #define RVM_KMM_SWE
   #define RVM_KNM_SWE
   #define RVM_UVM_SWE
   #define RVM_KZP_SWE
   #define RVM_KPG_SWE
   #define RVM_JTM_SWE
#endif

/*
 * AUDIO_FRAMEWORK SW entities
 * If it is defined, AUDIO SWE is defined and AUDIO BACKGROUND may be defined
 * depending on available audio features
 */

#ifdef RVM_AUDIO_SWE
  #define RVM_AUDIO_MAIN_SWE
  #if ((MELODY_E2) || (L1_GTT == 1))
    #define RVM_AUDIO_BGD_SWE
  #endif
#endif


/*
 * Obigo SWEs.
 */
#if ((defined RVM_OBIGO_WAP_FULL_SWE) || (defined RVM_OBIGO_MMS_FULL_SWE) || (defined RVM_OBIGO_FULL_SWE))
   #define RVM_MSFE_SWE
   #define RVM_MSME_SWE
   #define RVM_STKE_SWE
   #define RVM_PHSE_SWE
   #define RVM_PRSE_SWE
   #define RVM_IBSE_SWE
   #define RVM_SELE_SWE
   #define RVM_SECE_SWE
   #if ((defined RVM_OBIGO_WAP_FULL_SWE) || (defined RVM_OBIGO_FULL_SWE))
      #define RVM_BRSE_SWE
      #define RVM_BRAE_SWE
   #endif
   #if ((defined RVM_OBIGO_MMS_FULL_SWE) || (defined RVM_OBIGO_FULL_SWE))
      #define RVM_MMSE_SWE
      #define RVM_SLSE_SWE
      #define RVM_MEAE_SWE
      #define RVM_SLAE_SWE
   #endif
#endif



/* ----------------------------------------
 * Specifics for a RivieraTool build.
 * ---------------------------------------- */
#ifdef _WINDOWS

   #ifndef RVM_ATP_UART_SWE
      #define RVM_ATP_UART_SWE
   #endif

   // Bluetooth PS SW entities: Is this Windows specific???
   #ifdef BLUETOOTH
      #define RVM_SYN_SWE
   #endif

   /* If a TCP/IP Winsocket implementation is defined */
   #ifdef RVM_RNET_WS_SWE
      #define RVM_RNET_SWE          /* Define the generic API. */
      #ifdef RVM_RNET_RT_SWE
         #undef RVM_RNET_RT_SWE     /* Undef Bridge implemetation. */
      #endif
      #ifdef RVM_RNET_BR_SWE
         #undef RVM_RNET_BR_SWE     /* Undef Real implemetation. */
      #endif
   #endif
   /* If TCP/IP Bridge implementation is defined */
   #ifdef RVM_RNET_BR_SWE
      #define RVM_RNET_SWE          /* Define the generic API. */
      #ifdef RVM_RNET_RT_SWE
         #undef RVM_RNET_RT_SWE     /* Undef Real implemetation. */
      #endif
   #endif
   /* If TCP/IP Real implementation is defined */
   #ifdef RVM_RNET_RT_SWE
      #define RVM_RNET_SWE          /* Define the generic API. */
      #ifdef RVM_RNET_BR_SWE
         #undef RVM_RNET_BR_SWE     /* Undef Bridge implemetation. */
      #endif
   #endif

   // TEMPORARY: THESE SWEs MUST BE FIXED FOR RIVIERATOOL
   #ifdef RVM_DAR_SWE
      #undef RVM_DAR_SWE
   #endif
   #ifdef RVM_ETM_SWE
      #undef RVM_ETM_SWE
   #endif
   #ifdef RVM_TTY_SWE
      #undef RVM_TTY_SWE
   #endif

/* ----------------------------------------
 * Specifics for target build.
 * ---------------------------------------- */
#else
   #if (TI_PROFILER == 1) || (TI_NUC_MONITOR == 1)
      #define RVM_TI_PRF_SWE
   #endif

   #ifndef RVM_RVT_SWE
      #define RVM_RVT_SWE
   #endif

   #if ((defined RVM_TTY_SWE) && (L1_GTT == 0))
     #error "TTY entity cannot be enabled if L1_GTT is disabled (= 0)"
   #endif

   #if ((!defined RVM_TTY_SWE) && (L1_GTT == 1))
     #error "L1_GTT cannot be enabled (= 1) if TTY entity is disabled"
   #endif

  #if ((defined RVM_BAE_SWE) && (L1_EXT_AUDIO_MGT == 0))
    #error "BAE entity cannot be enabled if L1_EXT_AUDIO_MGT is disabled (= 0)"
  #endif

  #if ((!defined RVM_BAE_SWE) && (L1_EXT_AUDIO_MGT == 1))
    #error "L1_EXT_AUDIO_MGT cannot be enabled (= 1) if BAE entity is disabled"
  #endif
  
   /* If TCP/IP Bridge AND Real implementation are defined, error */
   #if (defined RVM_RNET_RT_SWE) && (defined RVM_RNET_BR_SWE)
     #error "RNET RT and BR cannot be compiled together on target"
   #endif
   /* If TCP/IP Bridge OR Real implementation are defined */
   #if (defined RVM_RNET_RT_SWE) || (defined RVM_RNET_BR_SWE)
      #define RVM_RNET_SWE          /* Define the generic API. */
      #ifdef RVM_RNET_WS_SWE
         #undef RVM_RNET_WS_SWE     /* Undef Winsocket implemetation. */
      #endif
   #endif

#endif

#if (TEST == 1) && (defined RVM_R2D_SWE) && (defined RVM_RGUI_SWE) 
   #define RVM_RTEST_SWE
#endif

#ifndef RVM_LCD_SWE
      #define RVM_LCD_SWE
#endif


#endif /* _RV_DEFINED_SWE_H_ */
