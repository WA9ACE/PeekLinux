/*
+-----------------------------------------------------------------------------
|  Project :  GPRS (8441)
|  Modul   :  sndcp_nds.c
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
|             functions to handles the incoming process internal signals as
|             described in the SDL-documentation (ND-statemachine)
+-----------------------------------------------------------------------------
*/


#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */

#include "dti.h"

#include "sndcp.h"        /* to get the global entity definitions */
#include "sndcp_f.h"       /* to get the functions to access the global arrays*/
#include <string.h>     /* to get memcpy() */

#include "sndcp_mgs.h"       /* to get the signals to mg */
#include "sndcp_sdas.h"       /* to get the signals to sda */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/* 
+------------------------------------------------------------------------------ 
| Function : sndcp_reset_nd_nsapi_npdu_num 
+------------------------------------------------------------------------------ 
| Description : reset the npdu_num(N-PDU receive number) to zero in 
|               acknowledgement mode
| 
| Parameters  : nsapi number
| 
+------------------------------------------------------------------------------ 
*/ 

#ifndef CF_FAST_EXEC

GLOBAL void sndcp_reset_nd_nsapi_npdu_num(UBYTE nsapi) 
{ 
  TRACE_FUNCTION(" sndcp_reset_nd_nsapi_npdu_num ");
  sndcp_data->nd = & sndcp_data->nd_base[nsapi];
  sndcp_data->nd->npdu_num = 0;
} 
#endif /* CF_FAST_EXEC */

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sig_mg_nd_get_rec_no
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_GET_REC_NO
|
| Parameters  : (UBYTE), the nsapi number to choose the instance
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

#ifdef SNDCP_UPM_INCLUDED 
GLOBAL U8 sig_mg_nd_get_rec_no (UBYTE nsapi)
#else
GLOBAL void sig_mg_nd_get_rec_no (UBYTE nsapi)
#endif  /*SNDCP_UPM_INCLUDED*/
{
  TRACE_ISIG( "sig_mg_nd_get_rec_no" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nd = & sndcp_data->nd_base[nsapi];

  switch( GET_STATE(ND) )
  {
    case ND_DEFAULT:
    case ND_RECOVER:
    case ND_SUSPEND:
      sig_nd_mg_rec_no(nsapi, sndcp_data->nd->npdu_num);
      break;
    default:
      TRACE_ERROR( "SIG_MG_ND_GET_REC_NO unexpected" );
      break;
  }
#ifdef SNDCP_UPM_INCLUDED 
  return sndcp_data->nd->npdu_num;
#endif  /*SNDCP_UPM_INCLUDED*/
} /* sig_mg_nd_get_rec_no() */

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : sig_mg_nd_recover
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_ND_RECOVER
|
| Parameters  : (UBYTE), the nsapi number to choose the instance
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_mg_nd_recover (UBYTE nsapi)
{
  TRACE_ISIG( "sig_mg_nd_recover" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nd = & sndcp_data->nd_base[nsapi];

  switch( GET_STATE(ND) )
  {
    case ND_DEFAULT:
      SET_STATE (ND, ND_RECOVER);
      break;
    case ND_RECOVER:
    case ND_SUSPEND:
      break;
    default:
      TRACE_ERROR( "SIG_MG_ND_RECOVER unexpected" );
      break;
  }
} /* sig_mg_nd_recover() */

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sig_mg_nd_reset_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_ND_RESET_IND
|
| Parameters  : (UBYTE), the nsapi number to choose the instance
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_mg_nd_reset_ind (UBYTE nsapi)
{
  TRACE_ISIG( "sig_mg_nd_reset_ind" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nd = & sndcp_data->nd_base[nsapi];

  switch( GET_STATE(ND) )
  {
    case ND_DEFAULT:
    case ND_RECOVER:
    case ND_SUSPEND:
      break;
    default:
      TRACE_ERROR( "SIG_MG_ND_RESET_IND unexpected" );
      break;
  }
} /* sig_mg_nd_reset_ind() */

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : sig_mg_nd_resume
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_ND_RESUME
|
| Parameters  : (UBYTE), the nsapi number to choose the instance
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_mg_nd_resume (UBYTE nsapi)
{
  TRACE_ISIG( "sig_mg_nd_resume" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nd = & sndcp_data->nd_base[nsapi];

  switch( GET_STATE(ND) )
  {
    case ND_SUSPEND:
      SET_STATE (ND, ND_RECOVER);
      break;
    default:
      TRACE_ERROR( "SIG_MG_ND_RESUME unexpected" );
      break;
  }
} /* sig_mg_nd_resume() */

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sig_mg_nd_suspend
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_ND_SUSPEND
|
| Parameters  : (UBYTE), the nsapi number to choose the instance
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_mg_nd_suspend (UBYTE nsapi)
{
  TRACE_ISIG( "sig_mg_nd_suspend" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nd = & sndcp_data->nd_base[nsapi];

  switch( GET_STATE(ND) )
  {
    case ND_DEFAULT:
    case ND_RECOVER:
      SET_STATE(ND, ND_SUSPEND);
      break;
    default:
      TRACE_ERROR( "SIG_MG_ND_SUSPEND unexpected" );
      break;
  }
} /* sig_mg_nd_suspend() */

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sig_sd_nd_unitdata_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_SD_ND_UNITDATA_IND
|
| Parameters  : nsapi number,
|               T_SN_UNITDATA_IND, N-PDU to be sent to nsapidown in form
|                of primitive struct
|
+------------------------------------------------------------------------------
*/
/*#if defined(CF_FAST_EXEC) || defined(_SIMULATION_) || \
   !defined(SNDCP_UPM_INCLUDED)   || defined(SNDCP_2to1) */

GLOBAL void sig_sd_nd_unitdata_ind (UBYTE nsapi,
                                    T_SN_UNITDATA_IND* sn_unitdata_ind)
{
#ifdef _SNDCP_DTI_2_
  ULONG linkid = 0;
#endif /*_SNDCP_DTI_2_*/
  TRACE_ISIG( "sig_sd_nd_unitdata_ind" );


#ifdef _SNDCP_MEAN_TRACE_
#ifdef _SNDCP_DTI_2_
  sndcp_mean_trace(nsapi,
                   SNDCP_MEAN_DOWN,
                   SNDCP_MEAN_UNACK,
                   sn_unitdata_ind->desc_list2.list_len);
#else /*_SNDCP_DTI_2_*/
  sndcp_mean_trace(nsapi,
                   SNDCP_MEAN_DOWN,
                   SNDCP_MEAN_UNACK,
                   sn_unitdata_ind->desc_list.list_len);
#endif /*_SNDCP_DTI_2_*/

#endif /* _SNDCP_MEAN_TRACE_ */

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nd = & sndcp_data->nd_base[nsapi];


#ifdef SNDCP_TRACE_IP_DATAGRAM
  if(sndcp_data->trace_ip_header || sndcp_data->trace_ip_datagram){
    TRACE_EVENT("-------- Downlink IP datagram --------");
#ifdef _SNDCP_DTI_2_
    sndcp_trace_ip_datagram(& sn_unitdata_ind->desc_list2);
#else /*_SNDCP_DTI_2_*/
    sndcp_trace_ip_datagram(& sn_unitdata_ind->desc_list);
#endif /*_SNDCP_DTI_2_*/
    TRACE_EVENT("--------------------------------------");
  } else {  
#ifdef _SNDCP_DTI_2_
    sndcp_default_ip_trace(& sn_unitdata_ind->desc_list2, SNDCP_DL_PACKET);
#else /*_SNDCP_DTI_2_*/
    sndcp_default_ip_trace(& sn_unitdata_ind->desc_list, SNDCP_DL_PACKET);
#endif /*_SNDCP_DTI_2_*/
  }
  if(sndcp_data->trace_ip_bin){
#ifdef _SNDCP_DTI_2_
    sndcp_bin_trace_ip(& sn_unitdata_ind->desc_list2, SNDCP_DL_PACKET);
#else /*_SNDCP_DTI_2_*/
    sndcp_bin_trace_ip(& sn_unitdata_ind->desc_list, SNDCP_DL_PACKET);
#endif /*_SNDCP_DTI_2_*/
  }

#else /* SNDCP_TRACE_IP_DATAGRAM */

#ifdef SNDCP_TRACE_BUFFER
#ifdef _SNDCP_DTI_2_
  TRACE_EVENT_P1("downlink at SN SAP: %d octets",
                 sn_unitdata_ind->desc_list2.list_len);
  sndcp_trace_desc_list(& sn_unitdata_ind->desc_list2);
#else /*_SNDCP_DTI_2_*/
  TRACE_EVENT_P1("downlink at SN SAP: %d octets",
                 sn_unitdata_ind->desc_list.list_len);
  sndcp_trace_desc_list(& sn_unitdata_ind->desc_list);
#endif /*_SNDCP_DTI_2_*/
#endif /* SNDCP_TRACE_BUFFER */
#endif /* SNDCP_TRACE_IP_DATAGRAM */


  switch( GET_STATE(ND) )
  {
    case ND_DEFAULT:

#ifdef _SNDCP_DTI_2_

      /*
      * Copy sn_unitdata_ind to dti2_data_ind.
      */
      {
        PALLOC_DESC2(dti_data_ind, DTI2_DATA_IND);

        sndcp_get_nsapi_linkid(nsapi, &linkid);
        dti_data_ind->link_id = linkid;
        dti_data_ind->parameters.p_id = sn_unitdata_ind->p_id;
        dti_data_ind->parameters.st_lines.st_flow = 0;
        dti_data_ind->parameters.st_lines.st_line_sa = 0;
        dti_data_ind->parameters.st_lines.st_line_sb = 0;
        dti_data_ind->parameters.st_lines.st_break_len = 0;
        dti_data_ind->desc_list2 = sn_unitdata_ind->desc_list2;

        /*
         * Free the received test primitive.
         */
        MFREE (sn_unitdata_ind);
        sn_unitdata_ind = NULL;

        dti_send_data(sndcp_data->hDTI, /* DTI_HANDLE hDTI, */
                      0, /* U8 instance, */
                      SNDCP_INTERFACE_UNACK, /* U8 interface, */
                      nsapi,/* U8 channel, */
                      dti_data_ind /* T_DTI2_DATA_IND *dti_data_ind);*/
                      );

      }

#else /*_SNDCP_DTI_2_*/
      /*
       * Copy sn_unitdata_ind to dti_data_ind.
       */
      {
        /*
         * Copy sn_data_req_test to dti_data_ind.
         */
        PALLOC_DESC(dti_data_ind, DTI_DATA_IND);

        dti_data_ind->p_id = sn_unitdata_ind->p_id;
        dti_data_ind->desc_list = sn_unitdata_ind->desc_list;
        dti_data_ind->op_ack = OP_ACK;

        /*
         * Free the received test primitive.
         */
        MFREE (sn_unitdata_ind);
        sn_unitdata_ind = NULL;

        /*
         * Trace p_id.
         */

#ifdef SNDCP_TRACE_ALL
        switch (dti_data_ind->p_id) {
        case DTI_PID_IP:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->p_id,
                         "DTI_PID_IP");
          break;
        case DTI_PID_CTCP:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->p_id,
                         "DTI_PID_CTCP");
          break;
        case DTI_PID_UTCP:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->p_id,
                         "DTI_PID_UTCP");
          break;
        case DTI_PID_FRAME:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->p_id,
                         "DTI_PID_FRAME");
          break;
        default:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->p_id,
                         "unknown");


        }
#endif /* SNDCP_TRACE_ALL */
        dti_send_data(sndcp_data->hDTI, /* DTI_HANDLE hDTI, */
                      0, /* U8 instance, */
                      SNDCP_INTERFACE_UNACK, /* U8 interface, */
                      nsapi,/* U8 channel, */
                      dti_data_ind /* T_DTI2_DATA_IND *dti_data_ind);*/
                      );

      }
#endif /*_SNDCP_DTI_2_*/
      break;
    default:
      if (sn_unitdata_ind != NULL) {
#ifdef _SNDCP_DTI_2_
        MFREE_PRIM (sn_unitdata_ind);
#else /*_SNDCP_DTI_2_*/
        PFREE_DESC (sn_unitdata_ind);
#endif /*_SNDCP_DTI_2_*/
      }
      TRACE_ERROR( "SIG_SD_ND_UNITDATA_IND unexpected" );
      break;
  }
} /* sig_sd_nd_unitdata_ind() */

/*#endif *//* CF_FAST_EXEC || _SIMULATION_ || !SNDCP_UPM_INCLUDED || SNDCP_2to1 */

/*
+------------------------------------------------------------------------------
| Function    : sig_sda_nd_data_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_SDA_ND_DATA_IND
|
| Parameters  : nsapi number,
|               T_SN_DATA_IND, N-PDU to be sent to nsapidown in form
|                of primitive struct,
|               npdu number
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_sda_nd_data_ind (UBYTE nsapi,
                                 T_SN_DATA_IND* sn_data_ind,
                                 UBYTE npdu_number)
{
#ifdef _SNDCP_DTI_2_
  ULONG linkid = 0;
#endif /*_SNDCP_DTI_2_*/
  UBYTE sapi = 0;

  TRACE_ISIG( "sig_sda_nd_data_ind" );

#ifdef _SNDCP_MEAN_TRACE_
#ifdef _SNDCP_DTI_2_
  sndcp_mean_trace(nsapi,
                   SNDCP_MEAN_DOWN,
                   SNDCP_MEAN_ACK,
                   sn_data_ind->desc_list2.list_len);
#else /*_SNDCP_DTI_2_*/
  sndcp_mean_trace(nsapi,
                   SNDCP_MEAN_DOWN,
                   SNDCP_MEAN_ACK,
                   sn_data_ind->desc_list.list_len);
#endif /*_SNDCP_DTI_2_*/
#endif /* _SNDCP_MEAN_TRACE_ */


  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nd = & sndcp_data->nd_base[nsapi];

#ifdef SNDCP_TRACE_IP_DATAGRAM
  if(sndcp_data->trace_ip_header || sndcp_data->trace_ip_datagram){
    TRACE_EVENT("-------- Downlink IP datagram --------");
#ifdef _SNDCP_DTI_2_
    sndcp_trace_ip_datagram(& sn_data_ind->desc_list2);
#else /*_SNDCP_DTI_2_*/
    sndcp_trace_ip_datagram(& sn_data_ind->desc_list);
#endif /*_SNDCP_DTI_2_*/
    TRACE_EVENT("--------------------------------------");
  } else {  
#ifdef _SNDCP_DTI_2_
    sndcp_default_ip_trace(& sn_data_ind->desc_list2, SNDCP_DL_PACKET);
#else /*_SNDCP_DTI_2_*/
    sndcp_default_ip_trace(& sn_data_ind->desc_list, SNDCP_DL_PACKET);
#endif /*_SNDCP_DTI_2_*/
  }
  if(sndcp_data->trace_ip_bin){
#ifdef _SNDCP_DTI_2_
    sndcp_bin_trace_ip(& sn_data_ind->desc_list2, SNDCP_DL_PACKET);
#else /*_SNDCP_DTI_2_*/
    sndcp_bin_trace_ip(& sn_data_ind->desc_list, SNDCP_DL_PACKET);
#endif /*_SNDCP_DTI_2_*/
  }
#else /* SNDCP_TRACE_IP_DATAGRAM */

#ifdef SNDCP_TRACE_BUFFER
#ifdef _SNDCP_DTI_2_
  TRACE_EVENT_P1("downlink at SN SAP: %d octets",
                 sn_data_ind->desc_list2.list_len);
  sndcp_trace_desc_list(& sn_data_ind->desc_list2);
#else /*_SNDCP_DTI_2_*/
  TRACE_EVENT_P1("downlink at SN SAP: %d octets",
                 sn_data_ind->desc_list.list_len);
  sndcp_trace_desc_list(& sn_data_ind->desc_list);
#endif /*_SNDCP_DTI_2_*/
#endif /* SNDCP_TRACE_BUFFER */
#endif /* SNDCP_TRACE_IP_DATAGRAM */


  switch( GET_STATE(ND) )
  {
    case ND_DEFAULT:
      sndcp_data->nd->npdu_num++;

#ifdef _SNDCP_DTI_2_
      /*
       * Copy sn_unitdata_ind to dti_data_ind.
       */
      {
        /*
         * Copy sn_data_req_test to dti_data_ind.
         */
        PALLOC_DESC2(dti_data_ind, DTI2_DATA_IND);

        sndcp_get_nsapi_linkid(nsapi, &linkid);
        dti_data_ind->link_id = linkid;
        dti_data_ind->parameters.p_id = sn_data_ind->p_id;
        dti_data_ind->parameters.st_lines.st_flow = 0;
        dti_data_ind->parameters.st_lines.st_line_sa = 0;
        dti_data_ind->parameters.st_lines.st_line_sb = 0;
        dti_data_ind->parameters.st_lines.st_break_len = 0;
        dti_data_ind->desc_list2 = sn_data_ind->desc_list2;

        /*
         * Free the received test primitive.
         */
        MFREE (sn_data_ind);
        sn_data_ind = NULL;

        /*
         * Trace p_id.
         */
#ifdef SNDCP_TRACE_ALL
        switch (dti_data_ind->parameters.p_id) {
        case DTI_PID_IP:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->parameters.p_id,
                         "DTI_PID_IP");
          break;
        case DTI_PID_CTCP:
          TRACE_EVENT_P2("dti_data_ind->parameters.p_id: %02x (%s)",
                         dti_data_ind->parameters.p_id,
                         "DTI_PID_CTCP");
          break;
        case DTI_PID_UTCP:
          TRACE_EVENT_P2("dti_data_ind->parameters.p_id: %02x (%s)",
                         dti_data_ind->parameters.p_id,
                         "DTI_PID_UTCP");
          break;
        case DTI_PID_UOS:
          TRACE_EVENT_P2("dti_data_ind->parameters.p_id: %02x (%s)",
                         dti_data_ind->parameters.p_id,
                         "DTI_PID_FRAME");
          break;
        default:
          TRACE_EVENT_P2("dti_data_ind->parameters.p_id: %02x (%s)",
                         dti_data_ind->parameters.p_id,
                         "unknown");


        }
#endif /* SNDCP_TRACE_ALL */

        dti_send_data(sndcp_data->hDTI, /* DTI_HANDLE hDTI, */
                      0, /* U8 instance, */
#ifdef SNDCP_UPM_INCLUDED 
                      SNDCP_INTERFACE_UNACK, /* U8 interface, */
#else
                      SNDCP_INTERFACE_ACK, /* U8 interface, */
#endif 
                      nsapi,/* U8 channel, */
                      dti_data_ind /* T_DTI2_DATA_IND *dti_data_ind);*/
                      );

      }

#else /*_SNDCP_DTI_2_*/
      /*
       * Copy sn_unitdata_ind to dti_data_ind.
       */
      {
        /*
         * Copy sn_data_req_test to dti_data_ind.
         */
        PALLOC_DESC(dti_data_ind, DTI_DATA_IND);

        dti_data_ind->p_id = sn_data_ind->p_id;
        dti_data_ind->desc_list = sn_data_ind->desc_list;
        dti_data_ind->op_ack = OP_ACK;

        /*
         * Free the received test primitive.
         */
        MFREE (sn_data_ind);
        sn_data_ind = NULL;

        /*
         * Trace p_id.
         */
#ifdef SNDCP_TRACE_ALL
        switch (dti_data_ind->p_id) {
        case DTI_PID_IP:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->p_id,
                         "DTI_PID_IP");
          break;
        case DTI_PID_CTCP:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->p_id,
                         "DTI_PID_CTCP");
          break;
        case DTI_PID_UTCP:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->p_id,
                         "DTI_PID_UTCP");
          break;
        case DTI_PID_FRAME:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->p_id,
                         "DTI_PID_FRAME");
          break;
        default:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->p_id,
                         "unknown");


        }
#endif /* SNDCP_TRACE_ALL */

        dti_send_data(sndcp_data->hDTI, /* DTI_HANDLE hDTI, */
                      0, /* U8 instance, */
                      SNDCP_INTERFACE_ACK, /* U8 interface, */
                      nsapi,/* U8 channel, */
                      dti_data_ind /* T_DTI2_DATA_IND *dti_data_ind);*/
                      );

      }
#endif /*_SNDCP_DTI_2_*/

      break;
    case ND_RECOVER:
      if (sndcp_data->nd->npdu_num == npdu_number) {
        sndcp_data->nd->npdu_num++;
        SET_STATE(ND, ND_DEFAULT);

#ifdef _SNDCP_DTI_2_
      /*
       * Copy sn_unitdata_ind to dti_data_ind.
       */
      {
        /*
         * Copy sn_data_req_test to dti_data_ind.
         */
        PALLOC_DESC2(dti_data_ind, DTI2_DATA_IND);

        sndcp_get_nsapi_linkid(nsapi, &linkid);
        dti_data_ind->link_id = linkid;
        dti_data_ind->parameters.p_id = sn_data_ind->p_id;
        dti_data_ind->parameters.st_lines.st_flow = 0;
        dti_data_ind->parameters.st_lines.st_line_sa = 0;
        dti_data_ind->parameters.st_lines.st_line_sb = 0;
        dti_data_ind->parameters.st_lines.st_break_len = 0;
        dti_data_ind->desc_list2 = sn_data_ind->desc_list2;

        /*
         * Free the received test primitive.
         */
        MFREE (sn_data_ind);
        sn_data_ind = NULL;

        dti_send_data(sndcp_data->hDTI, /* DTI_HANDLE hDTI, */
                      0, /* U8 instance, */
#ifdef SNDCP_UPM_INCLUDED 
                      SNDCP_INTERFACE_UNACK, /* U8 interface, */
#else
                      SNDCP_INTERFACE_ACK, /* U8 interface, */
#endif 
                      nsapi,/* U8 channel, */
                      dti_data_ind /* T_DTI2_DATA_IND *dti_data_ind);*/
                      );
      }

#else /*_SNDCP_DTI_2_*/

      /*
       * Copy sn_unitdata_ind to dti_data_ind.
       */
      {
        /*
         * Copy sn_data_req_test to dti_data_ind.
         */
        PALLOC_DESC(dti_data_ind, DTI_DATA_IND);

        dti_data_ind->p_id = sn_data_ind->p_id;
        dti_data_ind->desc_list = sn_data_ind->desc_list;
        dti_data_ind->op_ack = OP_ACK;

        /*
         * Free the received test primitive.
         */
        MFREE (sn_data_ind);
        sn_data_ind = NULL;

                /*
         * Trace p_id.
         */
#ifdef SNDCP_TRACE_ALL
        switch (dti_data_ind->p_id) {
        case DTI_PID_IP:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->p_id,
                         "DTI_PID_IP");
          break;
        case DTI_PID_CTCP:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->p_id,
                         "DTI_PID_CTCP");
          break;
        case DTI_PID_UTCP:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->p_id,
                         "DTI_PID_UTCP");
          break;
        case DTI_PID_FRAME:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->p_id,
                         "DTI_PID_FRAME");
          break;
        default:
          TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                         dti_data_ind->p_id,
                         "unknown");


        }
#endif /* SNDCP_TRACE_ALL */

        dti_send_data(sndcp_data->hDTI, /* DTI_HANDLE hDTI, */
                      0, /* U8 instance, */
                      SNDCP_INTERFACE_ACK, /* U8 interface, */
                      nsapi,/* U8 channel, */
                      dti_data_ind /* T_DTI2_DATA_IND *dti_data_ind);*/
                      );
      }

#endif /*_SNDCP_DTI_2_*/
      }
      else
      {
        /*
         * Discard primitive.
         */
        if (sn_data_ind != NULL) {
#ifdef _SNDCP_DTI_2_
          MFREE_PRIM(sn_data_ind);
#else /*_SNDCP_DTI_2_*/
          PFREE_DESC(sn_data_ind);
#endif /*_SNDCP_DTI_2_*/
          sn_data_ind = NULL;
        }
        /*
         * Request next from sda.
         */
        sndcp_get_nsapi_sapi(nsapi, &sapi);
        sig_nd_sda_getdata_req(sapi, nsapi);
      };

      break;
    case ND_SUSPEND:
      /*
       * Discard primitive.
       */
      if (sn_data_ind != NULL) {
#ifdef _SNDCP_DTI_2_
        MFREE_PRIM(sn_data_ind);
#else /*_SNDCP_DTI_2_*/
        PFREE_DESC(sn_data_ind);
#endif /*_SNDCP_DTI_2_*/
        sn_data_ind = NULL;
      }
      /*
       * Request next from sda.
       */
      sndcp_get_nsapi_sapi(nsapi, &sapi);
      sig_nd_sda_getdata_req(sapi, nsapi);
      break;
    default:
      TRACE_ERROR( "SIG_SDA_ND_DATA_IND unexpected" );
#ifdef _SNDCP_DTI_2_
        MFREE_PRIM(sn_data_ind);
#else /*_SNDCP_DTI_2_*/
        PFREE_DESC(sn_data_ind);
#endif /*_SNDCP_DTI_2_*/
	sn_data_ind = NULL;
      break;
  }
} /* sig_sda_nd_data_ind() */

#endif /* CF_FAST_EXEC */

