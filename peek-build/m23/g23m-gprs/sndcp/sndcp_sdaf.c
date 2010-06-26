/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_sdaf.c
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
|  Purpose :  This modul is part of the entity SNDCP and implements all 
|             procedures and functions as described in the 
|             SDL-documentation (SDA-statemachine)
+----------------------------------------------------------------------------- 
*/ 



#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "prim.h"       /* to get the definitions of used SAP and directions */

#include "dti.h"
#include "sndcp.h"        /* to get the global entity definitions */
#include "sndcp_f.h"       /* to get the functions to access the global arrays*/

#include "sndcp_sdaf.h"       /* to get functions from sda */
#include "sndcp_pdas.h"   /* to get the signals to service pda.*/
#include "sndcp_mgs.h"   /* to get the signals to service mg.*/
#include "sndcp_cias.h"    /* to get the signals to service cia.*/



/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

LOCAL UBYTE sda_get_dcomp (T_LL_DATA_IND* ll_data_ind);


/*
+------------------------------------------------------------------------------
| Function    : sda_get_dcomp
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Returns the DCOMP value in the sdu of the given primitive.
|
| Parameters  :  ll_data_ind T_LL_DATA_IND*
|
+------------------------------------------------------------------------------
*/
LOCAL UBYTE sda_get_dcomp (T_LL_DATA_IND* ll_data_ind)
{ 
  return (ll_data_ind->sdu.buf[ll_data_ind->sdu.o_buf / 8 + 1] & 0xf0) >> 4 ;
 
} /* sda_get_dcomp() */

/*
+------------------------------------------------------------------------------
| Function    : sda_get_nsapi
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Gets the NSAPI from the segment header.
|
| Parameters  :  ll_data_ind T_LL_DATA_IND*, nsapi*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sda_get_nsapi (T_LL_DATA_IND* ll_data_ind, UBYTE* nsapi)
{ 
  /*
   * The nsapi number in the sn-pdu header is the least sig half first octet.
   */
  UBYTE first = ll_data_ind->sdu.buf[(ll_data_ind->sdu.o_buf / 8)];

  *nsapi = first & 0xf;
} /* sda_get_nsapi() */

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sda_get_data_if_nec
+------------------------------------------------------------------------------
| Description : This function sends a SIG_SD_PD_GETDATA_REQ to the affected
|               SAPI 
|               if necessary (service variable llc_may_send was set to FALSE) 
|               and sets llc_may_send to TRUE or does nothing (service 
|               variable llc_may_send already was set to TRUE).
|
| Parameters  : FPAR IN sapi UBYTE the number of the affected SAPI
|
+------------------------------------------------------------------------------
*/
GLOBAL void sda_get_data_if_nec (UBYTE sapi)
{ 
  UBYTE sapi_index = 0;
  USHORT state = 0;

  TRACE_FUNCTION( "sda_get_data_if_nec" );

#ifdef SNDCP_TRACE_ALL
  if (sndcp_data->sda->llc_may_send) {
    TRACE_EVENT("llc_may_send TRUE");
  } else {
    TRACE_EVENT("llc_may_send FALSE");
  }
#endif

   /*
    * set service instance according to sapi in signal
    */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sda = & sndcp_data->sda_base[sapi_index];
  sndcp_get_sapi_state(sapi, &state);
  if (! sndcp_data->sda->llc_may_send) {
    sig_sda_pda_getdata_req(sapi);
    /*
     * Mark this LLC SAPI as pending.
     */
    sndcp_data->sda->llc_may_send = TRUE;
  }
  
} /* sd_get_data_if_nec() */



/*
+------------------------------------------------------------------------------
| Function    : sda_is_seg_valid
+------------------------------------------------------------------------------
| Description : According to GSM 4.65, 6.9 unacknowledged segments for NSAPIs
|               in acknowledged mode and vice versa and also segments for not 
|               activated NSAPIs shall be discarded without further error 
|               notification. 
|
| Parameters  : nsapi, sapi, BOOL ack, BOOL* valid
|
+------------------------------------------------------------------------------
*/
GLOBAL void sda_is_seg_valid(T_LL_DATA_IND* ll_data_ind,
                         BOOL* valid)
{ 
  UBYTE nsapi = 0;
  UBYTE sapi = 0;
  /*
   * Is the affected NSAPI using acknowledged LLC mode?
   */
  BOOL ack = TRUE;
  /*
   * Is the NSAPI used at all?
   */
  BOOL used = FALSE;

  TRACE_FUNCTION( "sda_is_seg_valid" );

  *valid = TRUE;
  /*
   * Get the affected nsapi from primitive. 
   */
  sda_get_nsapi(ll_data_ind, &nsapi);
  /*
   * If the NSAPI is not used, leave.
   */
  sndcp_is_nsapi_used(nsapi, &used);
  if (!used) {
    *valid = FALSE;
    return;
  }
  /*
   * To which sapi is the given nsapi mapped?
   */
  sndcp_get_nsapi_sapi(nsapi, &sapi);
  /*
   * If the sapi in the primitive is the wrong one, leave.
   */
  if (sapi != ll_data_ind->sapi) {
    *valid = FALSE;
    return;
  }
  /*
   * If the nsapi normally uses unacknowledged LLC mode, leave.
   * Note: if 'ack' is false the NSAPI is in unacknowledged
   * mode normally and the 'sda' service is the wrong one 
   * anyway, but it was chosen because of the ack primitive.
   */
  sndcp_get_nsapi_ack(nsapi, &ack);
  if (!ack) {
    *valid = FALSE;
    return;
  }
  
} /* sd_is_seg_valid() */



/*
+------------------------------------------------------------------------------
| Function    : sda_delete_cur_sn_data_ind
+------------------------------------------------------------------------------
| Description : The descriptor list in service variable cur_sn_data_ind is 
|               deleted recursively, then the primitive is freed with PFREE 
|               macro.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sda_delete_cur_sn_data_ind (U8 nsapi)
{ 
  TRACE_FUNCTION( "sda_delete_cur_sn_data_ind" );

  if (sndcp_data->sda->cur_sn_data_ind[nsapi] != NULL) {
#ifdef _SNDCP_DTI_2_
    MFREE_PRIM(sndcp_data->sda->cur_sn_data_ind[nsapi]);
#else /*_SNDCP_DTI_2_*/
    PFREE_DESC(sndcp_data->sda->cur_sn_data_ind[nsapi]); 
#endif /*_SNDCP_DTI_2_*/

    sndcp_data->sda->cur_sn_data_ind[nsapi] = NULL;
  }
} /* sda_delete_cur_sn_data_ind() */


/*
+------------------------------------------------------------------------------
| Function    : sda_f_bit
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Returns TRUE if the f bit in the given sdu is set to 1, else 
|               FALSE.
|
| Parameters  :  ll_data_ind T_LL_DATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL sda_f_bit (T_LL_DATA_IND* ll_data_ind)
{ 
  UBYTE f_b = ll_data_ind->sdu.buf[ll_data_ind->sdu.o_buf / 8] & 0x40;

  TRACE_FUNCTION( "sda_f_bit" );

  return (f_b > 0);
 
} /* sda_f_bit() */



/*
+------------------------------------------------------------------------------
| Function    : sda_get_pcomp
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Returns the PCOMP value in the sdu of the given primitive.
|
| Parameters  :  ll_data_ind T_LL_DATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE sda_get_pcomp (T_LL_DATA_IND* ll_data_ind)
{ 
  return ll_data_ind->sdu.buf[ll_data_ind->sdu.o_buf / 8 + 1] & 0xf;
 
} /* sda_get_pcomp() */


/*
+------------------------------------------------------------------------------
| Function    : sda_init
+------------------------------------------------------------------------------
| Description : Initialize the entity instance.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sda_init (void)
{ 
  TRACE_FUNCTION( "sda_init" );

  sndcp_data->sda = & sndcp_data->sda_base[0];
  INIT_STATE(SDA_0, SDA_DEFAULT);
#ifdef SNDCP_2to1
  sndcp_data->sda->sapi = PS_SAPI_3; 
#else
  sndcp_data->sda->sapi = LL_SAPI_3; 
#endif /*SNDCP_2to1*/
  sndcp_data->sda = & sndcp_data->sda_base[1];
  INIT_STATE(SDA_1, SDA_DEFAULT);
#ifdef SNDCP_2to1
  sndcp_data->sda->sapi = PS_SAPI_5; 
#else
  sndcp_data->sda->sapi = LL_SAPI_5; 
#endif /*SNDCP_2to1*/


  sndcp_data->sda = & sndcp_data->sda_base[2];
  INIT_STATE(SDA_2, SDA_DEFAULT);
#ifdef SNDCP_2to1
  sndcp_data->sda->sapi = PS_SAPI_9; 
#else
  sndcp_data->sda->sapi = LL_SAPI_9; 
#endif /*SNDCP_2to1*/


  sndcp_data->sda = & sndcp_data->sda_base[3];
  INIT_STATE(SDA_3, SDA_DEFAULT);
#ifdef SNDCP_2to1
  sndcp_data->sda->sapi = PS_SAPI_11; 
#else
  sndcp_data->sda->sapi = LL_SAPI_11; 
#endif /*SNDCP_2to1*/

  {
    UBYTE sapi_index = 0;

    for (sapi_index = 0; sapi_index < SNDCP_NUMBER_OF_SAPIS; sapi_index++) {
      UBYTE nsapi = 0;

      sndcp_data->sda = & sndcp_data->sda_base[sapi_index];
      for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
        sda_set_nsapi_rec(nsapi, FALSE);
        sndcp_data->sda->cur_sn_data_ind[nsapi] = NULL;
      }
      sndcp_data->sda->llc_may_send = FALSE;
      sndcp_data->sda->uncomp_npdu_num = 0;
    }
  }
  
 
} /* sda_init() */

/*
+------------------------------------------------------------------------------
| Function    : sda_is_nsapi_rec
+------------------------------------------------------------------------------
| Description : Returns TRUE if the given nsapi is receptive, else 
|               FALSE.
|
| Parameters  :  nsapi, BOOL* b
|
+------------------------------------------------------------------------------
*/
GLOBAL void sda_is_nsapi_rec (UBYTE nsapi, BOOL* b)
{ 
  *b = sndcp_data->sda->nsapi_rec_ra[nsapi]; 
#ifdef SNDCP_TRACE_ALL
  TRACE_EVENT("is nsapi receptive?");
  if (*b) {
    TRACE_EVENT("TRUE");
  } else {
    TRACE_EVENT("FALSE");
  }
#endif
} /* sda_is_nsapi_rec() */


/*
+------------------------------------------------------------------------------
| Function    : sda_m_bit
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Returns TRUE if the m bit in the given sdu is set to 1, else 
|               FALSE.
|
| Parameters  :  ll_data_ind T_LL_DATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL sda_m_bit (T_LL_DATA_IND* ll_data_ind)
{ 
  UBYTE m_b = 0;

  TRACE_FUNCTION( "sda_m_bit" );

  m_b = ll_data_ind->sdu.buf[ll_data_ind->sdu.o_buf / 8] & 0x10;
  return (m_b > 0);
 
} /* sda_m_bit() */



/*
+------------------------------------------------------------------------------
| Function    : sda_set_nsapi_rec
+------------------------------------------------------------------------------
| Description : Sets the given nsapi_rec_ra element to the given BOOL.
|
| Parameters  :  nsapi, BOOL b
|
+------------------------------------------------------------------------------
*/
GLOBAL void sda_set_nsapi_rec (UBYTE nsapi, BOOL b)
{ 
  sndcp_data->sda->nsapi_rec_ra[nsapi] = b; 
#ifdef SNDCP_TRACE_ALL
  TRACE_EVENT("set nsapi receptive to");
  if (b) {
    TRACE_EVENT("TRUE");
  } else {
    TRACE_EVENT("FALSE");
  }
#endif
} /* sda_set_nsapi_rec */


/*
+------------------------------------------------------------------------------
| Function    : sda_ac_f_f0
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label AC_F_F0 in SDL.
|
| Parameters  :  ll_data_ind T_LL_DATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sda_ac_f_f0 (T_LL_DATA_IND* ll_data_ind)
{ 
  /* 
   * Get the NSAPI number from ll_data_ind.
   */
  UBYTE nsapi = 0;

  sda_get_nsapi(ll_data_ind, &nsapi);
  sig_sda_mg_re_est(ll_data_ind->sapi, nsapi);
  PFREE(ll_data_ind);
} /* sda_ac_f_f0() */


/*
+------------------------------------------------------------------------------
| Function    : sda_ac_f_f1_m0
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label AC_F_F1_M0 in SDL.
|
| Parameters  :  ll_data_ind T_LL_DATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sda_ac_f_f1_m0 (T_LL_DATA_IND* ll_data_ind)
{ 
  sndcp_set_ack_transfer_params(ll_data_ind);
  sig_sda_cia_cia_decomp_req(ll_data_ind);
} /* sda_ac_f_f1_m0() */

/*
+------------------------------------------------------------------------------
| Function    : sda_ac_f_f1_m1
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label AC_F_F1_M1 in SDL.
|
| Parameters  :  ll_data_ind T_LL_DATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sda_ac_f_f1_m1 (T_LL_DATA_IND* ll_data_ind)
{ 
  UBYTE nsapi = 0;

  sda_get_nsapi(ll_data_ind, &nsapi);
  sndcp_set_ack_transfer_params(ll_data_ind);
  sig_sda_cia_cia_decomp_req(ll_data_ind);
  
  sndcp_set_nsapi_rec_state(nsapi, SDA_RECEIVE_SUBSEQUENT_SEGMENT);

} /* sda_ac_f_f1_m0() */


/*
+------------------------------------------------------------------------------
| Function    : sda_ac_s_f0_m0
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label UN_S_F0_M0 in SDL.
|
| Parameters  :  ll_data_ind T_LL_DATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sda_ac_s_f0_m0 (T_LL_DATA_IND* ll_data_ind)
{ 
  UBYTE nsapi = 0;

  sda_get_nsapi(ll_data_ind, &nsapi);
  sndcp_data->cur_seg_pos[nsapi] = SEG_POS_LAST;
  sndcp_set_nsapi_rec_state(nsapi, SDA_RECEIVE_FIRST_SEGMENT);

  sig_sda_cia_cia_decomp_req(ll_data_ind);
} /* sda_ac_s_f0_m0() */


/*
+------------------------------------------------------------------------------
| Function    : sda_ac_s_f0_m1
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label AC_S_F0_M1 in SDL.
|
| Parameters  :  ll_data_ind T_LL_DATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sda_ac_s_f0_m1 (T_LL_DATA_IND* ll_data_ind)
{ 
  UBYTE nsapi = 0;

  sda_get_nsapi(ll_data_ind, &nsapi);
  sndcp_data->cur_seg_pos[nsapi] = SEG_POS_NONE;
  sig_sda_cia_cia_decomp_req(ll_data_ind);
    
} /* sda_ac_s_f0_m1() */

/*
+------------------------------------------------------------------------------
| Function    : sda_ac_s_f1_m1
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label AC_S_F1_M1 in SDL.
|
| Parameters  :  ll_data_ind T_LL_DATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sda_ac_s_f1_m1 (T_LL_DATA_IND* ll_data_ind)
{ 
  /*
   * Are PCOMP and DCOMP value the same as in the first segment?
   */
  UBYTE prim_dcomp = 0;
  UBYTE prim_pcomp = 0;
  UBYTE nsapi = 0;

  sda_get_nsapi(ll_data_ind, &nsapi);
  prim_dcomp = sda_get_dcomp(ll_data_ind);
  prim_pcomp = sda_get_pcomp(ll_data_ind);
  if (prim_dcomp == sndcp_data->cur_dcomp[nsapi] &&
    prim_pcomp == sndcp_data->cur_pcomp[nsapi]) {
    sndcp_data->big_head[nsapi] = TRUE;
    sda_ac_s_f0_m1(ll_data_ind);
  } else {
    sda_delete_cur_sn_data_ind(nsapi);
    sig_sda_mg_re_est(ll_data_ind->sapi, nsapi);
    PFREE(ll_data_ind);
  }
  
} /* sda_ac_s_f1_m1() */

/*
+------------------------------------------------------------------------------
| Function    : sda_ac_s_f1_m0
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label AC_S_F1_M1 in SDL.
|
| Parameters  :  ll_data_ind T_LL_DATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sda_ac_s_f1_m0 (T_LL_DATA_IND* ll_data_ind)
{ 
  /*
   * Are PCOMP and DCOMP value the same as in the first segment?
   */
  UBYTE prim_dcomp = 0;
  UBYTE prim_pcomp = 0;
  UBYTE nsapi = 0;

  sda_get_nsapi(ll_data_ind, &nsapi);
  prim_dcomp = sda_get_dcomp(ll_data_ind);
  prim_pcomp = sda_get_pcomp(ll_data_ind);
  if (prim_dcomp == sndcp_data->cur_dcomp[nsapi] &&
    prim_pcomp == sndcp_data->cur_pcomp[nsapi]) {
    sndcp_data->big_head[nsapi] = TRUE;
    sda_ac_s_f0_m0(ll_data_ind);
  } else {
    sda_delete_cur_sn_data_ind(nsapi);
    sig_sda_mg_re_est(ll_data_ind->sapi, nsapi);
    PFREE(ll_data_ind);
  }
  
} /* sda_ac_s_f1_m0() */
