/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_suf.c
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
|             SDL-documentation (SU-statemachine)
+----------------------------------------------------------------------------- 
*/ 

/*---- HISTORY --------------------------------------------------------------*/

#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "prim.h"       /* to get the definitions of used SAP and directions */

#include "dti.h"

#include "sndcp.h"        /* to get the global entity definitions */
#include "sndcp_f.h"       /* to get the functions to access the global arrays*/

#include "sndcp_nus.h"   /* to get signals to service nu */
#include "sndcp_cias.h"   /* to get signals to service cia */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
#ifdef _SNDCP_DTI_2_

/*
+------------------------------------------------------------------------------
| Function    : su_get_nsapi
+------------------------------------------------------------------------------
| Description : finde the affected nsapi in sn-PDU.
|
| Parameters  : ll_unitdata_req*
| Returns     : the nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL U8 su_get_nsapi (T_LL_UNITDESC_REQ* ll_unitdesc_req) 
{ 
   T_desc3* desc3 = NULL;
   U8 * sndcp_header = NULL;

  /* Get the descriptor describing the memory area with the sndcp header */

  desc3 = (T_desc3*)ll_unitdesc_req->desc_list3.first;
  sndcp_header = (U8*)desc3->buffer;

  TRACE_FUNCTION( "su_get_nsapi" );

  return (*(sndcp_header + ENCODE_OFFSET_BYTE)) & 0xf;
} /* sua_get_nsapi() */

#else /* _SNDCP_DTI_2_ */

/*
+------------------------------------------------------------------------------
| Function    : su_get_nsapi
+------------------------------------------------------------------------------
| Description : finde the affected nsapi in sn-PDU.
|
| Parameters  : ll_unitdata_req*
| Returns     : the nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE su_get_nsapi (T_LL_UNITDATA_REQ* ll_unitdata_req) 
{ 
  USHORT octet_offset = (ll_unitdata_req->sdu.o_buf) >> 3;

  TRACE_FUNCTION( "su_get_nsapi" );

  return ll_unitdata_req->sdu.buf[octet_offset] & 0xf;

} /* su_get_nsapi() */

#endif /* _SNDCP_DTI_2_ */

/*
+------------------------------------------------------------------------------
| Function    : su_init
+------------------------------------------------------------------------------
| Description : The function su_init() .... 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void su_init (void)
{ 
  TRACE_FUNCTION( "su_init" );

  /*
   * Not initialized:
   * Members of sn_unitdata_q are not set to NULL.
   * Members of npdu_number_q are not set to 0.
   * Members of ll_unitdata_q are not set to NULL.
   */

  sndcp_data->su = & sndcp_data->su_base[0];
  INIT_STATE(SU_0, SU_LLC_NOT_RECEPTIVE);
#ifdef SNDCP_2to1
  sndcp_data->su->sapi = PS_SAPI_3; 
#else
  sndcp_data->su->sapi = LL_SAPI_3; 
#endif /*SNDCP_2to1*/

  sndcp_data->su = & sndcp_data->su_base[1];
  INIT_STATE(SU_1, SU_LLC_NOT_RECEPTIVE);
#ifdef SNDCP_2to1
  sndcp_data->su->sapi = PS_SAPI_5; 
#else
  sndcp_data->su->sapi = LL_SAPI_5; 
#endif /*SNDCP_2to1*/

  sndcp_data->su = & sndcp_data->su_base[2];
  INIT_STATE(SU_2, SU_LLC_NOT_RECEPTIVE);
#ifdef SNDCP_2to1
  sndcp_data->su->sapi = PS_SAPI_9; 
#else
  sndcp_data->su->sapi = LL_SAPI_9; 
#endif /*SNDCP_2to1*/

  sndcp_data->su = & sndcp_data->su_base[3];
  INIT_STATE(SU_3, SU_LLC_NOT_RECEPTIVE);
#ifdef SNDCP_2to1
  sndcp_data->su->sapi = PS_SAPI_11; 
#else
  sndcp_data->su->sapi = LL_SAPI_11; 
#endif /*SNDCP_2to1*/
  {
    UBYTE sapi_index = 0;

    for (sapi_index = 0; sapi_index < SNDCP_NUMBER_OF_SAPIS; sapi_index++) {
      sndcp_data->su = & sndcp_data->su_base[sapi_index];
      sndcp_data->su->n201_u = N201_U_DEFAULT;
      sndcp_data->su->sn_unitdata_q_read = 0;
      sndcp_data->su->sn_unitdata_q_write = 0;
#ifdef _SNDCP_DTI_2_
      sndcp_data->su->ll_unitdesc_q_read = 0;
      sndcp_data->su->ll_unitdesc_q_write = 0;
#else /* _SNDCP_DTI_2_ */
      sndcp_data->su->ll_unitdata_q_read = 0;
      sndcp_data->su->ll_unitdata_q_write = 0;
#endif /* _SNDCP_DTI_2_ */
      sndcp_data->su->cia_state = CIA_IDLE;
    }
  }

  

} /* su_init() */


/*
+------------------------------------------------------------------------------
| Function    : su_next_sn_unitdata_req
+------------------------------------------------------------------------------
| Description : If queue with sn_unitdata_req is
| not empty, send SIG_SU_CIA_DATA_REQ with next prim from sn_unitdata_req queue
|
| Parameters  : sapi
| Pre         : cia_state == CIA_IDLE
|
+------------------------------------------------------------------------------
*/
GLOBAL void su_next_sn_unitdata_req (UBYTE sapi) 
{ 
  TRACE_FUNCTION( "su_next_sn_unitdata_req" );

  if (sndcp_data->su->sn_unitdata_q_write !=
      sndcp_data->su->sn_unitdata_q_read) {
    /*
     * Get sn_unitdata_req from queue.
     */
    T_SN_UNITDATA_REQ* sn_unitdata_req = 
      sndcp_data->su->sn_unitdata_q[sndcp_data->su->sn_unitdata_q_read];
    /*
     * Get npdu_number from queue.
     */
    USHORT npdu_num = 
      sndcp_data->su->npdu_number_q[sndcp_data->su->sn_unitdata_q_read];
    UBYTE nsapi = sn_unitdata_req->nsapi;

    sndcp_data->su->sn_unitdata_q[sndcp_data->su->sn_unitdata_q_read] = NULL;
    sndcp_data->su->sn_unitdata_q_read = 
      (sndcp_data->su->sn_unitdata_q_read + 1 ) % SN_UNITDATA_Q_LEN;

    /*
     * Cia is now busy. Send data.
     */
    sndcp_data->su->cia_state = CIA_BUSY;
    sig_su_cia_cia_comp_req(sn_unitdata_req,
                        npdu_num,
                        nsapi,
                        sapi);
  }  else if (sndcp_data->su->cia_state == CIA_IDLE) {
    UBYTE nsapi = 0;

    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      UBYTE used_sapi = 0;
      BOOL used = FALSE;

      sndcp_is_nsapi_used(nsapi, &used);
      if (used) {
        sndcp_get_nsapi_sapi(nsapi, &used_sapi);
        if (used_sapi == sapi) {
          sig_su_nu_ready_ind(nsapi);
        }
      }
    }
  }


} /* su_next_sn_unitdata_req() */

