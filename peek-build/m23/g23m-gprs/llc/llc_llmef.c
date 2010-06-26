/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  This modul is part of the entity LLC and implements all 
|             procedures and functions as described in the 
|             SDL-documentation (LLME-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_LLMEF_C
#define LLC_LLMEF_C
#endif

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include <string.h>     /* to get memcpy() */

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "llc.h"        /* to get the global entity definitions */

#include "llc_par.h"    /* to get the default values of the LLC paramters */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : llme_init
+------------------------------------------------------------------------------
| Description : This procedure initialises all necessary variables of llme for 
|               all SAPIs.
|               Also global variables are initialised that relate not to a
|               specific service (e.g. tlli_new/tlli_old).
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void llme_init (void)
{
  int n;

  TRACE_FUNCTION( "llme_init" );

  /*
   * Initialise service LLME with state TLLI_UNASSIGNED.
   */
  INIT_STATE (LLME, LLME_TLLI_UNASSIGNED);

  /*
   * Initialise global variables.
   */
  llc_data->suspended           = FALSE;
  llc_data->grlc_suspended      = FALSE;

#ifdef LL_2to1
  llc_data->tlli_new            = PS_TLLI_INVALID;
  llc_data->tlli_old            = PS_TLLI_INVALID;
#else
  llc_data->tlli_new            = LL_TLLI_INVALID;
  llc_data->tlli_old            = LL_TLLI_INVALID;
#endif

  llc_data->ciphering_algorithm = LLGMM_CIPHER_NO_ALGORITHM;
#ifdef LL_2to1
  memset (&(llc_data->kc), 0, sizeof(T_LLGMM_llgmm_kc));
#else
  memset (&(llc_data->kc), 0, sizeof(T_llgmm_kc));
#endif

  /* init current values */
#ifdef REL99
  llc_data->cur_pfi          = LL_PFI_SIGNALING; 
#endif
  llc_data->cur_qos.delay    = LL_DELAY_SUB;
  llc_data->cur_qos.relclass = LL_NO_REL;
  llc_data->cur_qos.peak     = LL_PEAK_SUB;
  llc_data->cur_qos.preced   = LL_PRECED_SUB;
  llc_data->cur_qos.mean     = LL_MEAN_SUB;
  llc_data->cur_radio_prio   = LL_RADIO_PRIO_1;  

  /*
   * Initialise all the global SAPI data, which is 
   * not related to only one service
   */
  for (n=0; n < MAX_SAPI_INC; n++)
  {
    SWITCH_SERVICE (llc, sapi, n);

    llc_data->sapi->vur       = 0;

    llc_data->sapi->va        = 0;
    llc_data->sapi->vs        = 0;
    llc_data->sapi->vr        = 0;

    llc_data->sapi->oc_ui_tx  = 0L;
    llc_data->sapi->oc_ui_rx  = 0L;
    llc_data->sapi->oc_i_tx   = 0L;
    llc_data->sapi->oc_i_rx   = 0L;

    llc_data->sapi->pbit_outstanding = FALSE;
  }

  return;
} /* llme_init() */



/*
+------------------------------------------------------------------------------
| Function    : llme_init_cipher
+------------------------------------------------------------------------------
| Description : This procedure initialises the global LLC variables llc_kc
|               (ciphering key Kc) and llc_ciphering_algorithm (ciphering
|               algorithm). If ciphering_algorithm is LLGMM_CIPH_NA, the
|               parameters are ignored and the procedure doesn't change 
|               anything. The procedures cipher_ll_pdu (service send_pdu) 
|               and decipher_ll_pdu (service receive_pdu) use the global 
|               variables for ciphering and deciphering.
|
| Parameters  : kc - ciphering key
|               ciphering_algorithm - ciphering algorithm
|
+------------------------------------------------------------------------------
*/
#ifdef LL_2to1
GLOBAL void llme_init_cipher (T_LLGMM_llgmm_kc kc, UBYTE ciphering_algorithm)
#else
GLOBAL void llme_init_cipher (T_llgmm_kc kc, UBYTE ciphering_algorithm)
#endif
{
  TRACE_FUNCTION( "llme_init_cipher" );
  
  if (ciphering_algorithm != LLGMM_CIPHER_NA)
  {
    switch (ciphering_algorithm)
    {
      case LLGMM_CIPHER_NO_ALGORITHM:
        TRACE_0_INFO("No cipher algorithm used!");
        break;

      case LLGMM_CIPHER_GPRS_A5_1:
        TRACE_0_INFO("Cipher algorithm GEA/1 used!");
        break;

      case LLGMM_CIPHER_GPRS_A5_2:
        TRACE_0_INFO("Cipher algorithm GEA/2 used!");
        break;

      default:
        TRACE_ERROR("Unknown Cipher algorithm used!");
        break;
    }

    /*
     * Set IOV default values, if kc is changed to a different value
     */
#ifdef LL_2to1
    if ( memcmp (&(llc_data->kc), &kc, sizeof(T_LLGMM_llgmm_kc)) )
#else
    if ( memcmp (&(llc_data->kc), &kc, sizeof(T_llgmm_kc)) )
#endif
    {
      TRACE_0_INFO( "kc changed" );
#ifdef LL_2to1
      memcpy (&(llc_data->kc), &kc, sizeof(T_LLGMM_llgmm_kc));
#else
      memcpy (&(llc_data->kc), &kc, sizeof(T_llgmm_kc));
#endif
    
      /*
       * Apply the default value of IOV-UI in case no negotiation
       * is started by the SGSN.
       * <R.LLC.LLC_PAR.A.016>
       */
      llc_data->iov_ui                       = LLC_IOV_UI_ALL_SAPIS;

      /*
       * Apply the default value of IOV-I in case no negotiation
       * is started by the SGSN.
       * <R.LLC.LLC_PAR.A.018>
       */
#ifdef LL_2to1
      llc_data->iov_i_base[IMAP(PS_SAPI_3)]  = LLC_IOV_I_SAPI_3;
      llc_data->iov_i_base[IMAP(PS_SAPI_5)]  = LLC_IOV_I_SAPI_5;
      llc_data->iov_i_base[IMAP(PS_SAPI_9)]  = LLC_IOV_I_SAPI_9;
      llc_data->iov_i_base[IMAP(PS_SAPI_11)] = LLC_IOV_I_SAPI_11;
#else
      llc_data->iov_i_base[IMAP(LL_SAPI_3)]  = LLC_IOV_I_SAPI_3;
      llc_data->iov_i_base[IMAP(LL_SAPI_5)]  = LLC_IOV_I_SAPI_5;
      llc_data->iov_i_base[IMAP(LL_SAPI_9)]  = LLC_IOV_I_SAPI_9;
      llc_data->iov_i_base[IMAP(LL_SAPI_11)] = LLC_IOV_I_SAPI_11;
#endif
    }

    /*
     * Set new ciphering algorithm
     */
    llc_data->ciphering_algorithm           = ciphering_algorithm;
  }
  else
  {
    TRACE_0_INFO("Cipher algorithm not changed");
  }
  
  return;
} /* llme_init_cipher() */
