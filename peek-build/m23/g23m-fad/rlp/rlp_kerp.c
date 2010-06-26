/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  Rlp_kerp.c
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
|  Purpose :  This Modul defines the functions for processing
|             of incomming primitives for the component
|             Radio Link Protocol of the base station
+----------------------------------------------------------------------------- 
*/ 

#ifndef RLP_KERP_C
#define RLP_KERP_C
#endif

#define ENTITY_RLP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "vsi.h"
#include "macdef.h"
#include "custom.h"
#include "gsm.h"
#include "cus_rlp.h"
#include "prim.h"
#include "tok.h"
#include "rlp.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+------------------------------------------------------------------------------
|  Function    : ker_rlp_attach_req
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_ATTACH_REQ received from L2R.
|
|  Parameters  : rlp_attach_req - 
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void ker_rlp_attach_req
                  (
                    T_RLP_ATTACH_REQ *rlp_attach_req
                  )
{
  TRACE_FUNCTION ("ker_rlp_attach_req()");
  PACCESS        (rlp_attach_req);

  /*******************************/
  /*
   *  Initialize Shared Fax/Data memory
   */
  memset (rlp_data, 0, sizeof (T_RLP_DATA));

  ker_init (); 
  snd_init (&rlp_data->snd);
  rcv_init ();

  rlp_data->uplink_frame_trace   = FALSE;
  rlp_data->downlink_frame_trace = FALSE;

  rbm_reset(&rlp_data->rbm);
  rlp_data_magic_num = RLP_DATA_MAGIC_NUM;       /* memory is initialized */

  /*******************************/

  switch (GET_STATE (KER))
  {
    case RLP_ADM_AND_DETACHED:
      /*
       * processing for state RLP_ADM_AND_DETACHED
       */
      
      rlp_data->ker.Poll_xchg    = IW_IDLE;
      rlp_data->ker.SABM_State   = ISW_IDLE;
      rlp_data->ker.T1           = GEN_TO;
      rlp_data->ker.T_ul         = (USHORT)UL_TO;
      rlp_data->ker.BytesPerPrim = rlp_attach_req->bytes_per_prim;

      ker_set_frame_size (rlp_attach_req->rate);

      ker_get_frame_desc (rlp_data->ker.UI_Frame,
                          &rlp_data->ker.UI_FrameDesc);

      ker_get_frame_desc (rlp_data->ker.XID_C_Frame,
                          &rlp_data->ker.XID_C_FrameDesc);

      ker_get_frame_desc (rlp_data->ker.XID_R_Frame,
                          &rlp_data->ker.XID_R_FrameDesc);

      ker_get_frame_desc (rlp_data->ker.TEST_R_Frame,
                          &rlp_data->ker.TEST_R_FrameDesc);
      
      ker_get_frame_desc (rlp_data->ker.REMAP_Frame,
                          &rlp_data->ker.REMAP_FrameDesc);
      
      rlp_data->ker.TEST_R_State = IS_IDLE;
      rlp_data->ker.UA_State     = IS_IDLE;
      rlp_data->ker.UI_State     = IS_IDLE;
      rlp_data->ker.XID_R_State  = IS_IDLE;
      
      ker_init_xid_data(rlp_attach_req);

      SET_STATE (KER, RLP_ADM_AND_ATTACHED);
      break; 

    default:
      break;
  }
  PFREE (rlp_attach_req);
}

/*
+------------------------------------------------------------------------------
|  Function    : ker_rlp_detach_req
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_DETACH_REQ received from L2R.
|
|  Parameters  : rlp_detach_req -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void ker_rlp_detach_req(T_RLP_DETACH_REQ *rlp_detach_req)
{
  TRACE_FUNCTION ("ker_rlp_detach_req()");
  PACCESS        (rlp_detach_req);

  /*
  * processing for any kernel state
  */
  rlp_data->ker.DM_State = IS_IDLE;
  ker_deinit_link_vars ();
  SET_STATE (KER, RLP_ADM_AND_DETACHED);
  {
  PPASS (rlp_detach_req, rlp_detach_cnf, RLP_DETACH_CNF);
  PSENDX (L2R, rlp_detach_cnf);
  }
  rlp_data_magic_num = 0;  /* memory is no more initialized */
}

/*
+------------------------------------------------------------------------------
|  Function    : ker_rlp_connect_req
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_CONNECT_REQ received from L2R.
|
|  Parameters  : rlp_connect_req - 
|                  
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void ker_rlp_connect_req
                  (
                    T_RLP_CONNECT_REQ *rlp_connect_req
                  )
{
  TRACE_FUNCTION ("ker_rlp_connect_req()");
  PACCESS        (rlp_connect_req);

  switch (GET_STATE (KER))
  {
    case RLP_ADM_AND_ATTACHED:
      /*
       * processing for state RLP_ADM_AND_ATTACHED
       */
      rlp_data->ker.SABM_State = ISW_SEND;
      rlp_data->ker.SABM_Count = 0;
      rlp_data->ker.DM_State   = IS_IDLE;

      SET_STATE (KER, RLP_PENDING_CONNECT_REQ);
      break; 

    default:
      break;
  }
  PFREE (rlp_connect_req);
}

/*
+------------------------------------------------------------------------------
|  Function    : ker_rlp_connect_res
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_CONNECT_RES received from L2R.
|
|  Parameters  : rlp_connect_res -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void ker_rlp_connect_res
                  (
                    T_RLP_CONNECT_RES *rlp_connect_res
                  )
{
  TRACE_FUNCTION ("ker_rlp_connect_res()");
  PACCESS        (rlp_connect_res);

  switch (GET_STATE (KER))
  {
    case RLP_PENDING_CONNECT_IND:
    {
      /*
       * processing for state RLP_PENDING_CONNECT_IND
       */
      rlp_data->ker.ABit = rlp_connect_res->ack_flg;
      
      switch (rlp_data->ker.ABit)
      {
        case RLP_NAK:
          rlp_data->ker.DM_State = IS_SEND;
          rlp_data->ker.DM_FBit  = 1;
          SET_STATE (KER, RLP_ADM_AND_ATTACHED);
          break;

        case RLP_ACK:
          rlp_data->ker.UA_State = IS_SEND;
          rlp_data->ker.UA_FBit  = 1;
          ker_init_link_vars ();
          SET_STATE (KER, RLP_CONNECTION_ESTABLISHED);
          break;
      }
    }
  }
  PFREE (rlp_connect_res);
}

/*
+------------------------------------------------------------------------------
|  Function    : ker_rlp_disc_req
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_DISC_REQ received from L2R.
|
|  Parameters  : rlp_disc_req - 
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void ker_rlp_disc_req
                  (
                    T_RLP_DISC_REQ *rlp_disc_req
                  )
{
  TRACE_FUNCTION ("ker_rlp_disc_req()");
  PACCESS        (rlp_disc_req);

  switch (GET_STATE (KER))
  {
    case RLP_ADM_AND_DETACHED:
    case RLP_ADM_AND_ATTACHED:
      /*
       * processing for state RLP_ADM_AND_DETACHED
       */
      break; 

    default:
      /*
       * processing for any other state
       */
      TIMERSTOP (TT_HANDLE);

      rlp_data->ker.DISC_State = ISW_SEND;
      rlp_data->ker.DISC_Count = 0;

      switch (rlp_data->ker.Poll_xchg)
      {
        case IW_IDLE:
          rlp_data->ker.DISC_PBit = 1;
          break;
    
        default:
          rlp_data->ker.DISC_PBit = 0;
          break;
      }

      ker_deinit_link_vars ();
      rlp_data->ker.DISC_Ind = FALSE;

      SET_STATE (KER, RLP_DISCONNECT_INITIATED);
      TRACE_EVENT("#1");
      break;
  }
  PFREE (rlp_disc_req);
}

/*
+------------------------------------------------------------------------------
|  Function    : ker_rlp_reset_req
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_RESET_REQ received from L2R.
|
|  Parameters  : rlp_reset_req -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void ker_rlp_reset_req
                  (
                    T_RLP_RESET_REQ *rlp_reset_req
                  )
{
  TRACE_FUNCTION ("ker_rlp_reset_req()");
  PACCESS        (rlp_reset_req);

  switch (GET_STATE (KER))
  {
    case RLP_CONNECTION_ESTABLISHED:
      /*
       * processing for state RLP_CONNECTION_ESTABLISHED
       */
      TIMERSTOP (TT_HANDLE);
      TIMERSTOP (TRCVR_HANDLE);
      ker_reset_all_t_rcvs ();
      
      rlp_data->ker.SABM_State = ISW_SEND;
      rlp_data->ker.SABM_Count = 0;
      
      ker_deinit_link_vars ();

      SET_STATE (KER, RLP_PENDING_RESET_REQ);
      break;
  }
  PFREE (rlp_reset_req);
}

/*
+------------------------------------------------------------------------------
|  Function    : ker_rlp_reset_res
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_RESET_RES received from L2R.
|
|  Parameters  : rlp_reset_res - 
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void ker_rlp_reset_res
                  (
                    T_RLP_RESET_RES *rlp_reset_res
                  )
{
  TRACE_FUNCTION ("ker_rlp_reset_res()");
  PACCESS        (rlp_reset_res);

  switch (GET_STATE (KER))
  {
    case RLP_PENDING_RESET_IND:
      /*
       * processing for state RLP_PENDING_RESET_IND
       */
      rlp_data->ker.UA_State = IS_SEND;
      rlp_data->ker.UA_FBit  = 1;
 
      ker_init_link_vars ();
      SET_STATE (KER, RLP_CONNECTION_ESTABLISHED);
      break;
  }
  PFREE (rlp_reset_res);
}

/*
+------------------------------------------------------------------------------
|  Function    : ker_rlp_data_req
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_ATTACH_REQ received from L2R.
|
|  Parameters  : rlp_data_req -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void ker_rlp_data_req
                  (
                    T_RLP_DATA_REQ *rlp_data_req
                  )
{
  TRACE_FUNCTION ("ker_rlp_data_req()");
  PACCESS        (rlp_data_req);

  switch (GET_STATE (KER))
  {
    case RLP_CONNECTION_ESTABLISHED:
      switch (rlp_data->ker.UL_Snd_State)
      {
        case IW_WAIT:
          sbm_store_prim (rlp_data_req);
          rlp_data->ker.UL_Snd_State = IW_IDLE;
          return;
      }
      break; 
  }
  PFREE (rlp_data_req);
}

/*
+------------------------------------------------------------------------------
|  Function    : ker_rlp_getdata_req
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_GETDATA_REQ received from L2R.
|
|  Parameters  : rlp_getdata_req -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void ker_rlp_getdata_req
                  (
                    T_RLP_GETDATA_REQ *rlp_getdata_req
                  )
{
  TRACE_FUNCTION ("ker_rlp_getdata_req()");
  PACCESS        (rlp_getdata_req);

  switch (GET_STATE (KER))
  {
    case RLP_CONNECTION_ESTABLISHED:
    case RLP_PENDING_REMAP_REQ:
    case RLP_REMAP_DATA:
    {
      T_RLP_DATA_IND *rlp_data_ind;
      /*
       * processing for states RLP_CONNECTION_ESTABLISHED
       *                       RLP_PENDING_REMAP_REQ:
       *                       RLP_REMAP_DATA:
       */
      
      if (rbm_get_prim 
            (
              &rlp_data_ind,
              &rlp_data->ker.LRReady,
              &rlp_data->ker.LRFull
            ))
      {
        /*
         * primitive is ready for sending to L2R
         */
        PSENDX (L2R, rlp_data_ind);
        rlp_data->ker.UL_Rcv_State = IW_IDLE;
        TIMERSTOP (TUL_RCV_HANDLE);
      }
      else
      {
        /*
         * no primitive is ready for sending to L2R
         */
        TIMERSTART(TUL_RCV_HANDLE, rlp_data->ker.T_ul);
        rlp_data->ker.UL_Rcv_State = IW_WAIT;
      }
      break;
    }
  }
  PFREE (rlp_getdata_req);
}

/*
+------------------------------------------------------------------------------
|  Function    : ker_rlp_ui_req
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_UI_REQ received from L2R.
|
|  Parameters  : rlp_ui_req -
|                 
|
|  Return      : 
+------------------------------------------------------------------------------
*/


GLOBAL void ker_rlp_ui_req
                  (
                    T_RLP_UI_REQ *rlp_ui_req
                  )
{
  TRACE_FUNCTION ("ker_rlp_ui_req()");
  PACCESS        (rlp_ui_req);

  switch (GET_STATE (KER))
  {
    case RLP_ADM_AND_DETACHED:
      /*
       * processing for state RLP_ADM_AND_DETACHED
       */
      break; 

    default:
      ker_copy_sdu_to_frame 
      (
        &rlp_ui_req->sdu,
        rlp_data->ker.UI_Frame,
        0
      );

      rlp_data->ker.UI_State = IS_SEND;
      rlp_data->ker.UI_PBit  = 0;
      break;
  }
  PFREE (rlp_ui_req);
}

/*
+------------------------------------------------------------------------------
|  Function    : ker_rlp_remap_req
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_REMAP_REQ received from L2R.
|
|  Parameters  : rlp_remap_req -
|                 
|
|  Return      : 
+------------------------------------------------------------------------------
*/


GLOBAL void ker_rlp_remap_req
                  (
                    T_RLP_REMAP_REQ *rlp_remap_req
                  )
{
	T_FRAME_NUM vr;
  USHORT oldFrameSize;

  TRACE_FUNCTION ("ker_rlp_remap_req()");
  PACCESS        (rlp_remap_req);

  switch (GET_STATE (KER))
  {
    case RLP_CONNECTION_ESTABLISHED:
      /*
       * processing for state RLP_CONNECTION_ESTABLISHED
       */
      rlp_data->ker.OldFrameSize = rlp_data->ker.FrameSize;

      ker_set_frame_size(rlp_remap_req->rate);

      if (rlp_data->ker.OldFrameSize NEQ rlp_data->ker.FrameSize)
      {
        ker_get_frame_desc (rlp_data->ker.UI_Frame,
          &rlp_data->ker.UI_FrameDesc);
        
        ker_get_frame_desc (rlp_data->ker.XID_C_Frame,
          &rlp_data->ker.XID_C_FrameDesc);
        
        ker_get_frame_desc (rlp_data->ker.XID_R_Frame,
          &rlp_data->ker.XID_R_FrameDesc);
        
        ker_get_frame_desc (rlp_data->ker.TEST_R_Frame,
          &rlp_data->ker.TEST_R_FrameDesc);
        
        ker_get_frame_desc (rlp_data->ker.REMAP_Frame,
          &rlp_data->ker.REMAP_FrameDesc);
        rlp_data->ker.Poll_xchg = IW_IDLE;
        
        TIMERSTOP (TT_HANDLE);
        TIMERSTOP (TRCVR_HANDLE);
        ker_reset_all_t_rcvs ();
        rbm_reset_all_r_states();
        TIMERSTOP (TXID_HANDLE); 
        
        SET_STATE(KERXID_C, ISW_IDLE);
        
        rlp_data->ker.DTX_SF      = DTX_N;

        vr = rbm_prepare_remap (rlp_data->ker.FrameSize);
          
        ker_fill_remap_frame(vr);
        
        SET_STATE (KER, RLP_PENDING_REMAP_REQ);
      }
      break;

    case RLP_PENDING_REMAP_REQ:
    case RLP_REMAP_DATA:
      /*
       * processing for state RLP_PENDING_REMAP_REQ
       */
      oldFrameSize = rlp_data->ker.FrameSize;

      ker_set_frame_size(rlp_remap_req->rate);

      if (oldFrameSize NEQ rlp_data->ker.FrameSize)
      {
        ker_get_frame_desc (rlp_data->ker.UI_Frame,
          &rlp_data->ker.UI_FrameDesc);
        
        ker_get_frame_desc (rlp_data->ker.XID_C_Frame,
          &rlp_data->ker.XID_C_FrameDesc);
        
        ker_get_frame_desc (rlp_data->ker.XID_R_Frame,
          &rlp_data->ker.XID_R_FrameDesc);
        
        ker_get_frame_desc (rlp_data->ker.TEST_R_Frame,
          &rlp_data->ker.TEST_R_FrameDesc);
        
        ker_get_frame_desc (rlp_data->ker.REMAP_Frame,
          &rlp_data->ker.REMAP_FrameDesc);
        rlp_data->ker.Poll_xchg = IW_IDLE;
        
        TIMERSTOP (TT_HANDLE); 
        
        vr = rbm_prepare_remap (rlp_data->ker.FrameSize);
          
        ker_fill_remap_frame(vr);

        if (GET_STATE (KER) EQ RLP_REMAP_DATA)
        {
          SET_STATE (KER, RLP_PENDING_REMAP_REQ);
        }
      }
      break;
  }
  PFREE (rlp_remap_req);
}

/*
+------------------------------------------------------------------------------
|  Function    : ker_rlp_remap_data_res
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_REMAP_DATA_RES received from L2R.
|
|  Parameters  : rlp_remap_data_res -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void ker_rlp_remap_data_res
                  (
                    T_RLP_REMAP_DATA_RES *rlp_remap_data_res
                  )
{
  TRACE_FUNCTION ("ker_rlp_remap_data_res()");
  PACCESS        (rlp_remap_data_res);

  switch (GET_STATE (KER))
  {
    case RLP_REMAP_DATA:
      /*
       * processing for state RLP_PENDING_REMAP_REQ
       */
      if (!ker_send_remap_data ())
      {
        PALLOC (rlp_remap_cnf, RLP_REMAP_CNF);
        
        sbm_reset_after_remap
          (
          rlp_data->ker.FrameSize, 
          rlp_data->ker.RemapNr
          );
        
        PSENDX (L2R, rlp_remap_cnf);
        SET_STATE (KER, RLP_CONNECTION_ESTABLISHED);
      }
      break; 
  }
  PFREE (rlp_remap_data_res);
}

