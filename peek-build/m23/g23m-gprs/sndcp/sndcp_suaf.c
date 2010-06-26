/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_suaf.c
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
|             SDL-documentation (SUA-statemachine)
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

#include "sndcp_suaf.h"  /* to get this file  */
#include "sndcp_nus.h"  /* to get signals to service nu  */
#include "sndcp_cias.h"  /* to get signals to service cia  */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

#ifdef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : sua_get_nsapi
+------------------------------------------------------------------------------
| Description : find the affected nsapi in sn-PDU.
|
| Parameters  : ll_desc_req*
| Returns     : the nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL U8 sua_get_nsapi (T_LL_DESC_REQ* ll_desc_req) 
{ 
   T_desc3* desc3 = NULL;
   U8 * sndcp_header = NULL;

  /* Get the descriptor describing the memory area with the sndcp header */

  desc3 = (T_desc3*)ll_desc_req->desc_list3.first;
  sndcp_header = (U8*)desc3->buffer;

  TRACE_FUNCTION( "sua_get_nsapi" );

  return (*(sndcp_header + ENCODE_OFFSET_BYTE)) & 0xf;
} /* sua_get_nsapi() */
#else /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : sua_get_nsapi
+------------------------------------------------------------------------------
| Description : find the affected nsapi in sn-PDU.
|
| Parameters  : ll_data_req*
| Returns     : the nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE sua_get_nsapi (T_LL_DATA_REQ* ll_data_req) 
{ 
  USHORT octet_offset = (ll_data_req->sdu.o_buf) >> 3;

  TRACE_FUNCTION( "sua_get_nsapi" );

  return ll_data_req->sdu.buf[octet_offset] & 0xf;

} /* sua_get_nsapi() */
#endif /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : sua_init
+------------------------------------------------------------------------------
| Description : The function sua_init()
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sua_init (void)
{ 
  TRACE_FUNCTION( "sua_init" );

  /*
   * Not initialized:
   * Members of sn_data_q are not set to NULL.
   * Members of npdu_number_q are not set to 0.
   * Members of ll_desc_q are not set to NULL.
   * Members of ll_data_q are not set to NULL.
   */

  sndcp_data->sua = & sndcp_data->sua_base[0];
  INIT_STATE(SUA_0, SUA_LLC_NOT_RECEPTIVE);
#ifdef SNDCP_2to1
  sndcp_data->sua->sapi = PS_SAPI_3; 
#else
  sndcp_data->sua->sapi = LL_SAPI_3; 
#endif /*SNDCP_2to1*/
  sndcp_data->sua = & sndcp_data->sua_base[1];
  INIT_STATE(SUA_1, SUA_LLC_NOT_RECEPTIVE);
#ifdef SNDCP_2to1
  sndcp_data->sua->sapi = PS_SAPI_5; 
#else
  sndcp_data->sua->sapi = LL_SAPI_5; 
#endif /*SNDCP_2to1*/

  sndcp_data->sua = & sndcp_data->sua_base[2];
  INIT_STATE(SUA_2, SUA_LLC_NOT_RECEPTIVE);
#ifdef SNDCP_2to1
  sndcp_data->sua->sapi = PS_SAPI_9; 
#else
  sndcp_data->sua->sapi = LL_SAPI_9; 
#endif /*SNDCP_2to1*/

  sndcp_data->sua = & sndcp_data->sua_base[3];
  INIT_STATE(SUA_3, SUA_LLC_NOT_RECEPTIVE);
#ifdef SNDCP_2to1
  sndcp_data->sua->sapi = PS_SAPI_11; 
#else
  sndcp_data->sua->sapi = LL_SAPI_11; 
#endif /*SNDCP_2to1*/
  {
    UBYTE sapi_index = 0;

    for (sapi_index = 0; sapi_index < SNDCP_NUMBER_OF_SAPIS; sapi_index++) {
      sndcp_data->sua = & sndcp_data->sua_base[sapi_index];
      sndcp_data->sua->n201_i = N201_I_DEFAULT;
      sndcp_data->sua->sn_data_q_read = 0;
      sndcp_data->sua->sn_data_q_write = 0;
#ifdef _SNDCP_DTI_2_
      sndcp_data->sua->ll_desc_q_read = 0;
      sndcp_data->sua->ll_desc_q_write = 0;
#else /* _SNDCP_DTI_2_ */
      sndcp_data->sua->ll_data_q_read = 0;
      sndcp_data->sua->ll_data_q_write = 0;
#endif /* _SNDCP_DTI_2_ */
      sndcp_data->sua->first_seg_info = NULL;
      sndcp_data->sua->cia_state = CIA_IDLE;

    }
  }

} /* sua_init() */

 


/*
+------------------------------------------------------------------------------
| Function    : sua_mark_segment
+------------------------------------------------------------------------------
| Description : The function sua_mark_segment()
|
| Parameters  : N-PDU number, NSAPI, LL_DESC_REQ in which the reference has to be set.
| Pre         : The "correct" sua instance is activated and 
|               sndcp_data->sua->segment_number is the segment number of the
|               current segment.
|
+------------------------------------------------------------------------------
*/
GLOBAL void sua_mark_segment (UBYTE npdu_number,
                              UBYTE nsapi)
{ 
  T_SEG_INFO* seg_info = NULL;

  TRACE_FUNCTION( "sua_mark_segment" );

  /*
   * Add new seg_info to the list.
   */
  if (sndcp_data->sua->first_seg_info == NULL) {
    MALLOC(seg_info, sizeof(T_SEG_INFO));
    /*
     * The list has been empty.
     */
    sndcp_data->sua->first_seg_info = seg_info;
    /*
     * First segment.
     */
    seg_info->number_of_segments = 1;
    /*
     * Reset the confirm mask.
     */
    seg_info->cnf_segments = 0;
    seg_info->npdu_number = npdu_number;
    seg_info->next = NULL;
    seg_info->nsapi = nsapi;
  } else {
    /*
     * There are already some seg_infos. Find the seg_info for the given N-PDU.
     * number and NSAPI or create a new one.
     */
    /*
     * Are N-PDU number and NSAPI different?
     */
    BOOL different = TRUE;
    BOOL end = FALSE;
    T_SEG_INFO* help = sndcp_data->sua->first_seg_info;

    while(!end && different) {
      if (nsapi == help->nsapi && npdu_number == help->npdu_number) {
        different = FALSE;
      } else {
        if (help->next == NULL) {
          end = TRUE;
        } else {
          help = help->next;
        }
      }
    }
    if (different) {
      /*
       * A seg_info with same NSAPI and N-PDU number has not been found.
       * Create a new one.
       */
      MALLOC(seg_info, sizeof(T_SEG_INFO));
      /*
       * First segment.
       */
      seg_info->number_of_segments = 1;
      /*
       * Reset the confirm mask.
       */
      seg_info->cnf_segments = 0;
      seg_info->npdu_number = npdu_number;
      seg_info->next = NULL;
      seg_info->nsapi = nsapi;
      /*
       * Add the new seg_info to the list.
       */
      help->next = seg_info;
    } else {
      /*
       * A seg_info with same NSAPI and N-PDU number has been found.
       * Use this one.
       */
      help->number_of_segments++;
    }

  }
 
} /* sua_mark_segment() */


/*
+------------------------------------------------------------------------------
| Function    : sua_next_sn_data_req
+------------------------------------------------------------------------------
| Description : If queue with sn_data_req is
| not empty, send SIG_SUA_CIA_DATA_REQ with next prim from sn_data_req queue
|
| Parameters  : sapi
| Pre         : cia_state == CIA_IDLE
|
+------------------------------------------------------------------------------
*/
GLOBAL void sua_next_sn_data_req (UBYTE sapi) 
{ 
  TRACE_FUNCTION( "sua_next_sn_data_req" );

  if (sndcp_data->sua->sn_data_q_write !=
      sndcp_data->sua->sn_data_q_read) {
    /*
     * Get sn_data_req from queue.
     */
    T_SN_DATA_REQ* sn_data_req = 
      sndcp_data->sua->sn_data_q[sndcp_data->sua->sn_data_q_read];
    /*
     * Get npdu_number from queue.
     */
    UBYTE npdu_num = 
      sndcp_data->sua->npdu_number_q[sndcp_data->sua->sn_data_q_read];
    UBYTE nsapi = sn_data_req->nsapi;

    sndcp_data->sua->sn_data_q[sndcp_data->sua->sn_data_q_read] = NULL;
    sndcp_data->sua->sn_data_q_read = 
      (sndcp_data->sua->sn_data_q_read + 1) % SN_DATA_Q_LEN;

    /*
     * Cia is now busy. Send data.
     */
    sndcp_data->sua->cia_state = CIA_BUSY;
    sig_sua_cia_cia_comp_req(sn_data_req,
                         npdu_num,
                         nsapi,
                         sapi);
  } else if (sndcp_data->sua->cia_state == CIA_IDLE) {
    UBYTE nsapi = 0;

    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      UBYTE used_sapi = 0;
      BOOL used = FALSE;
      BOOL ack = FALSE;

      sndcp_is_nsapi_used(nsapi, &used);
      if (used) {
        sndcp_get_nsapi_sapi(nsapi, &used_sapi);
        sndcp_get_nsapi_ack(nsapi, &ack);
        if (used_sapi == sapi && ack) {
          sig_sua_nu_ready_ind(nsapi);
        }
      }
    }
  }

} /* sua_next_sn_data_req() */

