/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_sdf.c
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
|             SDL-documentation (SD-statemachine)
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

#include "sndcp_cias.h"       /* to get the signals to service cia */
#include "sndcp_sdf.h"       /* to get functions in sd */
#include "sndcp_pds.h"    /* to get the sigbnals to service pd.*/




/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

LOCAL UBYTE sd_get_dcomp (T_LL_UNITDATA_IND* ll_unitdata_ind);

/*
+------------------------------------------------------------------------------
| Function    : sd_get_dcomp
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Returns the DCOMP value in the sdu of the given primitive.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
LOCAL UBYTE sd_get_dcomp (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 
  return (ll_unitdata_ind->sdu.buf[ll_unitdata_ind->sdu.o_buf / 8 + 1] & 0xf0) >> 4 ;
 
} /* sd_get_dcomp() */

/*
+------------------------------------------------------------------------------
| Function    : sd_get_npdu_num
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Gets the N-PDU number from the segment header.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*, npdu_num*
|
+------------------------------------------------------------------------------
*/
LOCAL void sd_get_npdu_num (T_LL_UNITDATA_IND* ll_unitdata_ind, USHORT* npdu_num)
{ 
  USHORT msn_off = 1;
  USHORT lsb_off = 2;
  UBYTE msn = 0;
  UBYTE lsb = 0;

  if (sd_f_bit(ll_unitdata_ind)) {
    msn_off = 2;
    lsb_off = 3;
  }

  msn =
    ll_unitdata_ind->sdu.buf[(ll_unitdata_ind->sdu.o_buf / 8) + msn_off];
  lsb = 
    ll_unitdata_ind->sdu.buf[(ll_unitdata_ind->sdu.o_buf / 8) + lsb_off];

  *npdu_num = ((msn & 0xf) << 8) + lsb;
} /* sd_get_npdu_num() */

/*
+------------------------------------------------------------------------------
| Function    : sd_get_nsapi
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Gets the NSAPI from the segment header.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*, nsapi*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_get_nsapi (T_LL_UNITDATA_IND* ll_unitdata_ind, UBYTE* nsapi)
{ 
  /*
   * The nsapi number in the sn-pdu header is the least sig half first octet.
   */
  UBYTE first = ll_unitdata_ind->sdu.buf[(ll_unitdata_ind->sdu.o_buf / 8)];

  *nsapi = first & 0xf;
} /* sd_get_nsapi() */


/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : sd_is_seg_valid
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
GLOBAL void sd_is_seg_valid(T_LL_UNITDATA_IND* ll_unitdata_ind,
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

  TRACE_FUNCTION( "sd_is_seg_valid" );

  *valid = TRUE;
  /*
   * Get the affected nsapi from primitive. 
   */
  sd_get_nsapi(ll_unitdata_ind, &nsapi);
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
  if (sapi != ll_unitdata_ind->sapi) {
    *valid = FALSE;
    return;
  }
  /*
   * If the nsapi normally uses acknowledged LLC mode, leave.
   * Note: if 'ack' is true the NSAPI is in acknowledged
   * mode normally and the 'sd' service is the wrong one 
   * anyway, but it was chosen because of the unack primitive.
   */
  sndcp_get_nsapi_ack(nsapi, &ack);
  if (ack) {
    *valid = FALSE;
    return;
  }
  
} /* sd_is_seg_valid() */



/*
+------------------------------------------------------------------------------
| Function    : sd_delete_cur_sn_unitdata_ind
+------------------------------------------------------------------------------
| Description : The descriptor list in service variable cur_sn_unitdata_ind is 
|               deleted recursively, then the primitive is freed with PFREE 
|               macro.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_delete_cur_sn_unitdata_ind (U8 nsapi)
{
  TRACE_FUNCTION( "sd_delete_cur_sn_unitdata_ind" );

  if (sndcp_data->sd->cur_sn_unitdata_ind[nsapi] != NULL) {
#ifdef _SNDCP_DTI_2_
    MFREE_PRIM(sndcp_data->sd->cur_sn_unitdata_ind[nsapi]);
#else /*_SNDCP_DTI_2_*/
    PFREE_DESC(sndcp_data->sd->cur_sn_unitdata_ind[nsapi]); 
#endif /*_SNDCP_DTI_2_*/
    sndcp_data->sd->cur_sn_unitdata_ind[nsapi] = NULL;
  }
} /* sd_delete_cur_sn_unitdata_ind(void) */


/*
+------------------------------------------------------------------------------
| Function    : sd_f_bit
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Returns TRUE if the f bit in the given sdu is set to 1, else 
|               FALSE.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL sd_f_bit (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 
  UBYTE f_b = ll_unitdata_ind->sdu.buf[ll_unitdata_ind->sdu.o_buf / 8] & 0x40;

  TRACE_FUNCTION( "sd_f_bit" );

  return (f_b > 0);
 
} /* sd_f_bit() */




/*
+------------------------------------------------------------------------------
| Function    : sd_get_pcomp
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Returns the PCOMP value in the sdu of the given primitive.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE sd_get_pcomp (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 
  return ll_unitdata_ind->sdu.buf[ll_unitdata_ind->sdu.o_buf / 8 + 1] & 0xf;
 
} /* sd_get_pcomp() */



/*
+------------------------------------------------------------------------------
| Function    : sd_get_unitdata_if_nec
+------------------------------------------------------------------------------
| Description : This function sends a SIG_SD_PD_GETUNITDATA_REQ to the affected
|               SAPI 
|               if necessary (service variable llc_may_send was set to FALSE) 
|               and sets llc_may_send to TRUE or does nothing (service 
|               variable llc_may_send already was set to TRUE).
|
| Parameters  : FPAR IN sapi UBYTE the number of the affected SAPI
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_get_unitdata_if_nec (UBYTE sapi)
{ 
  UBYTE sapi_index = 0;

  TRACE_FUNCTION( "sd_get_unitdata_if_nec" );

#ifdef SNDCP_TRACE_ALL
  if (sndcp_data->sd->llc_may_send) {
    TRACE_EVENT("llc_may_send TRUE");
  } else {
    TRACE_EVENT("llc_may_send FALSE");
  }
#endif
  
   /*
    * set service instance according to sapi in signal
    */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sd = & sndcp_data->sd_base[sapi_index];
  if (sndcp_data->sd->llc_may_send == FALSE) {
    sig_sd_pd_getunitdata_req(sapi);
    /*
     * Mark this LLC SAPI as pending.
     */
    sndcp_data->sd->llc_may_send = TRUE;
  }
  
} /* sd_get_unitdata_if_nec() */



/*
+------------------------------------------------------------------------------
| Function    : sd_init
+------------------------------------------------------------------------------
| Description : Initialize the entity instance.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_init (void)
{ 
  TRACE_FUNCTION( "sd_init" );

  sndcp_data->sd = & sndcp_data->sd_base[0];
  INIT_STATE(SD_0, SD_DEFAULT);
#ifdef SNDCP_2to1
  sndcp_data->sd->sapi = PS_SAPI_3;
#else
  sndcp_data->sd->sapi = LL_SAPI_3; 
#endif /*SNDCP_2to1*/
  sndcp_data->sd = & sndcp_data->sd_base[1];
  INIT_STATE(SD_1, SD_DEFAULT);
#ifdef SNDCP_2to1
  sndcp_data->sd->sapi = PS_SAPI_5;
#else
  sndcp_data->sd->sapi = LL_SAPI_5; 
#endif /*SNDCP_2to1*/

  sndcp_data->sd = & sndcp_data->sd_base[2];
  INIT_STATE(SD_2, SD_DEFAULT);
#ifdef SNDCP_2to1
  sndcp_data->sd->sapi = PS_SAPI_9;
#else
  sndcp_data->sd->sapi = LL_SAPI_9; 
#endif /*SNDCP_2to1*/

  sndcp_data->sd = & sndcp_data->sd_base[3];
  INIT_STATE(SD_3, SD_DEFAULT);
#ifdef SNDCP_2to1
  sndcp_data->sd->sapi = PS_SAPI_11;
#else
  sndcp_data->sd->sapi = LL_SAPI_11; 
#endif /*SNDCP_2to1*/

  {
    UBYTE sapi_index = 0;

    for (sapi_index = 0; sapi_index < SNDCP_NUMBER_OF_SAPIS; sapi_index++) {
      UBYTE nsapi = 0;

      sndcp_data->sd = & sndcp_data->sd_base[sapi_index];
      for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
        sd_set_nsapi_rec(nsapi, FALSE);
        sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_DISCARD);
        sndcp_data->sd->cur_sn_unitdata_ind[nsapi] = NULL;
      }
      sndcp_data->sd->llc_may_send = FALSE;
    }
  }
  
 
} /* sd_init() */

/*
+------------------------------------------------------------------------------
| Function    : sd_is_nsapi_rec
+------------------------------------------------------------------------------
| Description : Returns TRUE if the given nsapi is receptive, else 
|               FALSE.
|
| Parameters  :  nsapi, BOOL* b
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_is_nsapi_rec (UBYTE nsapi, BOOL* b)
{ 
  *b = sndcp_data->sd->nsapi_rec_ra[nsapi]; 
} /* sd_is_nsapi_rec() */


/*
+------------------------------------------------------------------------------
| Function    : sd_m_bit
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Returns TRUE if the m bit in the given sdu is set to 1, else 
|               FALSE.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL sd_m_bit (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 
  UBYTE m_b;

  TRACE_FUNCTION( "sd_m_bit" );

  m_b = ll_unitdata_ind->sdu.buf[ll_unitdata_ind->sdu.o_buf / 8] & 0x10;
  return (m_b > 0);
 
} /* sd_m_bit() */



/*
+------------------------------------------------------------------------------
| Function    : sd_set_nsapi_rec
+------------------------------------------------------------------------------
| Description : Sets the given nsapi_rec_ra element to the given BOOL.
|
| Parameters  :  nsapi, BOOL b
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_set_nsapi_rec (UBYTE nsapi, BOOL b)
{ 
  sndcp_data->sd->nsapi_rec_ra[nsapi] = b; 
} /* sd_set_nsapi_rec */


/*
+------------------------------------------------------------------------------
| Function    : sd_un_d_f0_m0
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label UN_D_F0_M0 in SDL.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_un_d_f0_m0 (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 
  UBYTE nsapi = 0;
  sd_get_unitdata_if_nec(ll_unitdata_ind->sapi);
  PFREE(ll_unitdata_ind);

  sd_get_nsapi(ll_unitdata_ind, &nsapi);
  sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_RECEIVE_FIRST_SEGMENT);
} /* sd_un_d_f0_m0() */

/*
+------------------------------------------------------------------------------
| Function    : sd_un_d_f0_m1
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label UN_D_F0_M1 in SDL.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_un_d_f0_m1 (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 
  if (ll_unitdata_ind != NULL) {
    sd_get_unitdata_if_nec(ll_unitdata_ind->sapi);
    PFREE(ll_unitdata_ind);
  }
} /* sd_un_d_f0_m1() */

/*
+------------------------------------------------------------------------------
| Function    : sd_un_d_f1_m0
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label UN_D_F1_M0 in SDL.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_un_d_f1_m0 (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 
  UBYTE nsapi = 0;
  
  sd_get_nsapi(ll_unitdata_ind, &nsapi);

  if (ll_unitdata_ind != NULL) {
    sd_get_unitdata_if_nec(ll_unitdata_ind->sapi);
    PFREE(ll_unitdata_ind);
  }

  sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_RECEIVE_FIRST_SEGMENT);

} /* sd_un_d_f1_m0() */

/*
+------------------------------------------------------------------------------
| Function    : sd_un_d_f1_m1
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label UN_D_F1_M1 in SDL.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_un_d_f1_m1 (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 
  if (ll_unitdata_ind != NULL) {
    sd_get_unitdata_if_nec(ll_unitdata_ind->sapi);
    PFREE(ll_unitdata_ind);
  }
} /* sd_un_d_f1_m1() */



/*
+------------------------------------------------------------------------------
| Function    : sd_un_f_f0_m0
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label UN_F_F0_M0 in SDL.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_un_f_f0_m0 (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 
  if (ll_unitdata_ind != NULL) {
    sd_get_unitdata_if_nec(ll_unitdata_ind->sapi);
    PFREE(ll_unitdata_ind);
  }
} /* sd_un_f_f0_m0() */

/*
+------------------------------------------------------------------------------
| Function    : sd_un_f_f0_m1
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label UN_F_F0_M1 in SDL.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_un_f_f0_m1 (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 
  UBYTE nsapi = 0;
  
  sd_get_nsapi(ll_unitdata_ind, &nsapi);

  if (ll_unitdata_ind != NULL) {
    sd_get_unitdata_if_nec(ll_unitdata_ind->sapi);
    PFREE(ll_unitdata_ind);
  }
  
  sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_DISCARD);

} /* sd_un_f_f0_m1() */



/*
+------------------------------------------------------------------------------
| Function    : sd_un_f_f1_m0
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label UN_F_F1_M0 in SDL.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_un_f_f1_m0 (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 
  sndcp_set_unack_transfer_params(ll_unitdata_ind);
  sig_sd_cia_cia_decomp_req(ll_unitdata_ind);
} /* sd_un_f_f1_m0() */

/*
+------------------------------------------------------------------------------
| Function    : sd_un_f_f1_m1
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label UN_F_F1_M1 in SDL.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_un_f_f1_m1 (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 
  UBYTE nsapi = 0;
  sndcp_set_unack_transfer_params(ll_unitdata_ind);
  sig_sd_cia_cia_decomp_req(ll_unitdata_ind);

  sd_get_nsapi(ll_unitdata_ind, &nsapi);
  sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_RECEIVE_SUBSEQUENT_SEGMENT);

} /* sd_un_f_f1_m1() */


/*
+------------------------------------------------------------------------------
| Function    : sd_un_s_f0_m0
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label UN_S_F0_M0 in SDL.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_un_s_f0_m0 (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 

  USHORT npdu_num = 0;
  UBYTE nsapi = 0;
  
  /*
   * Does the received segment have the current npdu number?
   */
  sd_get_npdu_num(ll_unitdata_ind, &npdu_num);
  sd_get_nsapi(ll_unitdata_ind, &nsapi);
  sndcp_data->cur_seg_pos[nsapi] = SEG_POS_LAST;
  sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_RECEIVE_FIRST_SEGMENT);
  if (npdu_num == 
      sndcp_data->cur_pdu_ref[nsapi].ref_npdu_num) {
    sig_sd_cia_cia_decomp_req(ll_unitdata_ind);
  } else {
    sd_delete_cur_sn_unitdata_ind(nsapi);
    if (ll_unitdata_ind != NULL) {
      sd_get_unitdata_if_nec(ll_unitdata_ind->sapi);
      PFREE(ll_unitdata_ind);
    }
  }
  
} /* sd_un_s_f0_m0() */


/*
+------------------------------------------------------------------------------
| Function    : sd_un_s_f0_m1
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label UN_S_F0_M1 in SDL.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_un_s_f0_m1 (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 
  USHORT npdu_num = 0;
  UBYTE nsapi = 0;

  /*
   * Does the received segment have the current npdu number?
   */
  sd_get_npdu_num(ll_unitdata_ind, &npdu_num);
  sd_get_nsapi(ll_unitdata_ind, &nsapi);
  if (npdu_num == 
      sndcp_data->cur_pdu_ref[nsapi].ref_npdu_num) {

    sndcp_data->cur_seg_pos[nsapi] = SEG_POS_NONE;
    sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_RECEIVE_SUBSEQUENT_SEGMENT);
    sig_sd_cia_cia_decomp_req(ll_unitdata_ind);
  } else {
    sd_delete_cur_sn_unitdata_ind(nsapi);
    if (ll_unitdata_ind != NULL) {
      sd_get_unitdata_if_nec(ll_unitdata_ind->sapi);
      PFREE(ll_unitdata_ind);
    }
    sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_RECEIVE_FIRST_SEGMENT);
  }
  
} /* sd_un_s_f0_m1() */

/*
+------------------------------------------------------------------------------
| Function    : sd_un_s_f1_m1
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label UN_S_F1_M1 in SDL.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_un_s_f1_m1 (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 
  USHORT npdu_num = 0;
  UBYTE nsapi = 0;

  /*
   * Get NSAPI
   */
  sd_get_nsapi(ll_unitdata_ind, &nsapi);  
  /*
   * Does the received segment have the current npdu number?
   */
  sd_get_npdu_num(ll_unitdata_ind, &npdu_num);
  if (npdu_num == 
      sndcp_data->cur_pdu_ref[nsapi].ref_npdu_num) {

    /*
     * Are PCOMP and DCOMP value the same as in the first segment?
     */
    UBYTE prim_dcomp = 0;
    UBYTE prim_pcomp = 0;

    prim_dcomp = sd_get_dcomp(ll_unitdata_ind);
    prim_pcomp = sd_get_pcomp(ll_unitdata_ind);
    if (prim_dcomp == sndcp_data->cur_dcomp[nsapi] &&
        prim_pcomp == sndcp_data->cur_pcomp[nsapi]) {

      sndcp_data->big_head[nsapi] = TRUE;
      sd_un_s_f0_m1(ll_unitdata_ind);
    } else {
      sd_delete_cur_sn_unitdata_ind(nsapi);
      if (ll_unitdata_ind != NULL) {
        sd_get_unitdata_if_nec(ll_unitdata_ind->sapi);
        PFREE(ll_unitdata_ind);
      }
      sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_DISCARD);
    }

  } else {
    sd_delete_cur_sn_unitdata_ind(nsapi);
    sd_un_f_f1_m1(ll_unitdata_ind);
  }

} /* sd_un_s_f1_m1() */

/*
+------------------------------------------------------------------------------
| Function    : sd_un_s_f1_m0
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Represents the label UN_S_F1_M1 in SDL.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sd_un_s_f1_m0 (T_LL_UNITDATA_IND* ll_unitdata_ind)
{ 
  USHORT npdu_num = 0;
  UBYTE nsapi = 0;

  /*
   * Get NSAPI
   */
  sd_get_nsapi(ll_unitdata_ind, &nsapi);
  /*
   * Does the received segment have the current npdu number?
   */
  sd_get_npdu_num(ll_unitdata_ind, &npdu_num);
  if (npdu_num == 
      sndcp_data->cur_pdu_ref[nsapi].ref_npdu_num) {

    /*
     * Are PCOMP and DCOMP value the same as in the first segment?
     */
    UBYTE prim_dcomp = 0;
    UBYTE prim_pcomp = 0;

    prim_dcomp = sd_get_dcomp(ll_unitdata_ind);
    prim_pcomp = sd_get_pcomp(ll_unitdata_ind);
    if (prim_dcomp == sndcp_data->cur_dcomp[nsapi] &&
      prim_pcomp == sndcp_data->cur_pcomp[nsapi]) {
      sndcp_data->big_head[nsapi] = TRUE;
      sd_un_s_f0_m0(ll_unitdata_ind);
    } else {
      sd_delete_cur_sn_unitdata_ind(nsapi);
      if (ll_unitdata_ind != NULL) {
        sd_get_unitdata_if_nec(ll_unitdata_ind->sapi);
        PFREE(ll_unitdata_ind);
      }
      sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_RECEIVE_FIRST_SEGMENT);
    }

  } else {

    sndcp_data->cur_seg_pos[nsapi] = SEG_POS_LAST + SEG_POS_FIRST;
    sd_delete_cur_sn_unitdata_ind(nsapi);
    sd_un_f_f1_m0(ll_unitdata_ind);

  }
  
} /* sd_un_s_f1_m0() */
