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
|  Purpose :  This modul is part of the entity PPP and implements all
|             functions to handles the incoming process internal signals as
|             described in the SDL-documentation (PRX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#define ENTITY_PPP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"     /* to get a lot of macros */
#include "custom.h"     /* to get a lot of macros */
/*lint -efile(766,gsm.h) */
#include "gsm.h"        /* to get a lot of macros */
/*lint -efile(766,cnf_ppp.h) */
#include "cnf_ppp.h"    /* to get cnf-definitions */
/*lint -efile(766,mon_ppp.h) */
#include "mon_ppp.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "dti.h"        /* to get the DTILIB definitions */
#include "ppp.h"        /* to get the global entity definitions */

#include "ppp_arbf.h"   /* to get arb functions */
#include "ppp_ftxs.h"   /* to get signal interface from ftx */

#ifdef FF_STATUS_TE
#include "ste.h"        /* To include function interface for TE status driver */
#include "gdi.h"        /* To include driver type-defines */
#endif /* FF_STATUS_TE */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sig_ftx_prx_start_flow_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_FTX_PRX_START_FLOW_REQ.
|               It is sent in case the ftx service is able
|               to send the next packet.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ftx_prx_start_flow_req ()
{
  TRACE_ISIG( "sig_ftx_prx_start_flow_req" );

  switch( GET_STATE( PPP_SERVICE_PRX ) )
  {
    case PRX_READY:
      {
        /*
         *  Tell DTILIB that PRX can start to receive data primitives again
         */
        dti_start(ppp_data->ppphDTI, PPP_INSTANCE, PROT_LAYER, PROT_CHANNEL);
      }
      break;

    case PRX_NDTI:
      /*
       * store signal for later activation
       */
      SET_STATE( PPP_SERVICE_PRX, PRX_NDTI_START_FLOW );
      break;

    case PRX_NDTI_START_FLOW:
      /*
       * silently ignore duplicate signal..
       */
      break;
      
    default:
      TRACE_ERROR( "SIG_FTX_PRX_START_FLOW_REQ unexpected" );
      break;
  }
} /* sig_ftx_prx_start_flow_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ftx_prx_stop_flow_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_FTX_PRX_STOP_FLOW_REQ.
|               It is sent in case the ftx service is able
|               to send the next packet.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ftx_prx_stop_flow_req ()
{
  TRACE_ISIG( "sig_ftx_prx_stop_flow_req" );

  switch( GET_STATE( PPP_SERVICE_PRX ) )
  {
    case PRX_READY:
      /*
       *  Tell DTILIB that PRX does not want to receive data primitives
       */
      dti_stop(ppp_data->ppphDTI, PPP_INSTANCE, PROT_LAYER, PROT_CHANNEL);
      break;

    case PRX_NDTI_START_FLOW:
      /*
       * Send a stop signal to DTI 
       */
      SET_STATE( PPP_SERVICE_PRX, PRX_NDTI );
      break;

    case PRX_NDTI:
      /*
       * silently ignore duplicate signal..
       */
      break;

    default:
      TRACE_ERROR( "SIG_PTX_PRX_STOP_FLOW_REQ unexpected" );
      break;
  }
} /* sig_ftx_prx_stop_flow_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ftx_prx_dead_mode_req
+------------------------------------------------------------------------------
| Description : Handles the release of buffered data in case of PPP link 
|               termination
|
| Parameters  : None
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ftx_prx_dead_mode_req ()
{
  TRACE_FUNCTION( "sig_ftx_prx_dead_mode_req()" );

#ifdef FF_STATUS_TE
  /*
   * Release buffered data - if any...
   */
  MFREE_DESC2(ppp_data->prx.TE_data_buffer_desc_list_first);
  ppp_data->prx.TE_data_buffer_desc_list_first = NULL;
  ppp_data->prx.TE_data_buffer_p_id            = DTI_PID_UOS;
  ppp_data->prx.TE_data_buffered               = FALSE;
#endif /* FF_STATUS_TE */

  switch( GET_STATE( PPP_SERVICE_PRX ) )
  {
    case PRX_READY:
    case PRX_NDTI_START_FLOW:
    case PRX_NDTI:
      SET_STATE( PPP_SERVICE_PRX, PRX_DEAD );
      break;

    default:
      TRACE_ERROR( "SIG_PTX_PRX_DEAD_MODE_REQ unexpected" );
      break;
  }
} /* sig_ftx_prx_dead_mode_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ftx_prx_ready_mode_req
+------------------------------------------------------------------------------
| Description : sets prx to ready mode 

| Parameters  : None
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ftx_prx_ready_mode_req ()
{
  TRACE_FUNCTION( "sig_ftx_prx_ready_mode_req()" );

  switch( GET_STATE( PPP_SERVICE_PRX ) )
  {
  case PRX_DEAD:
    SET_STATE( PPP_SERVICE_PRX, PRX_NDTI );
    break;

  case PRX_DEAD_DTI:
    SET_STATE( PPP_SERVICE_PRX, PRX_READY );
    break;

  default:
    TRACE_ERROR( "SIG_FTX_PRX_READY_MODE_REQ unexpected" );
    break;
  }
} /* sig_ftx_prx_ready_mode_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_arb_prx_dti_connected_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_PRX_DTI_CONNECTED_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_prx_dti_connected_req ()
{
  TRACE_ISIG( "sig_arb_prx_dti_connected_req" );

  switch( GET_STATE( PPP_SERVICE_PRX ) )
  {
    case PRX_DEAD:
      SET_STATE( PPP_SERVICE_PRX, PRX_DEAD_DTI );
      break;
    case PRX_NDTI:
      SET_STATE( PPP_SERVICE_PRX, PRX_READY );
      break;
    case PRX_NDTI_START_FLOW:
      SET_STATE( PPP_SERVICE_PRX, PRX_READY );
      /*
       * Send a start signal to DTI 
       */
      dti_start(ppp_data->ppphDTI, PPP_INSTANCE, PROT_LAYER, PROT_CHANNEL);
      break;
    default:
      TRACE_ERROR( "SIG_ARB_PRX_DTI_CONNECTED_REQ unexpected" );
      break;
  }
} /* sig_arb_prx_dti_connected_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_dti_prx_data_received_ind
+------------------------------------------------------------------------------
| Description : Handles the signal SIG_DTI_PRX_DATA_RECEIVED_IND
|
| Parameters  : dti_data_ind  - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_dti_prx_data_received_ind(T_DTI2_DATA_IND *dti_data_ind)
{
  TRACE_FUNCTION( "sig_dti_prx_data_received_ind" );
  PACCESS(dti_data_ind);

  switch( GET_STATE( PPP_SERVICE_PRX ) )
  {
    case PRX_READY:
#ifdef FF_STATUS_TE
      if(ppp_data->mode EQ PPP_SERVER)
      {
        /*
         * Check if TE is awake and ready to receive data... 
         * DRV_OK means it's awake
         * DRV_IN_PROGRESS means is was a sleep and processing a wake up
         * - await callback when TE is awake
         */
        if (DRV_OK EQ STE_Write(STE_IDENTIFIER_POWER, 
                                STE_POWER_ON, 
                                STE_POWER_ON))
        {
#ifdef _SIMULATION_
          TRACE_EVENT( "Return DRV_OK from STE_Write()" );
#endif /* _SIMULATION_ */
          sig_any_ftx_packet_req(dti_data_ind->p_id,
                                 (T_desc2*)dti_data_ind->desc_list2.first);
        }
        else
        {
#ifdef _SIMULATION_
          TRACE_EVENT( "Return DRV_IN_PROCESS from STE_Write()" );
#endif /* _SIMULATION_ */
          /* 
           * The TE is asleep and data can NOT be forwarded but must be buffered
           * Buffer p_id and desc_list until TE is awake and ready
           * but free old stored packets before
           */
          MFREE_DESC2(ppp_data->prx.TE_data_buffer_desc_list_first);
          ppp_data->prx.TE_data_buffered       = TRUE;
          ppp_data->prx.TE_data_buffer_p_id    = dti_data_req->p_id;
          ppp_data->
            prx.TE_data_buffer_desc_list_first = (T_desc2*)dti_data_req->
                                                   desc_list2.first;
        }
      }
      else
#endif /* FF_STATUS_TE  */
      {
        sig_any_ftx_packet_req(dti_data_ind->parameters.p_id,
                               (T_desc2*)dti_data_ind->desc_list2.first);
      }
      break;

    default:
        TRACE_ERROR( "DTI signal unexpected" );       
        arb_discard_packet((T_desc2*)dti_data_ind->desc_list2.first);
      break;
  }
  PFREE (dti_data_ind);
} /* sig_dti_prx_data_received_ind() */



#ifdef FF_STATUS_TE
/*
+------------------------------------------------------------------------------
| Function    : new_power_status_TE
+------------------------------------------------------------------------------
| Description : Handles the signal NEW_POWER_STATUS
|
| Parameters  : None
|
+------------------------------------------------------------------------------
*/
GLOBAL void new_power_status_TE ( void )
{
  ULONG    ste_power_status;
  UBYTE    temp_p_id;
  T_desc2* temp_desc;
  USHORT   ret_value;

  TRACE_FUNCTION( "new_power_status_TE" );

  if (ppp_data->prx.TE_data_buffered)
  {
    /*
     * Check if TE power status is ON or OFF... 
     */
    ret_value = STE_Read(STE_IDENTIFIER_POWER, &ste_power_status);

    if(ret_value NEQ DRV_OK)
    {
      TRACE_ERROR( "TE status driver error" );
    }
    /*
     * If this read operation fails (return ERROR) there must be a 
     * driver problem so we do not care about the driver and forward the data.
     * If power status of the TE is ON we also forward the data
     * If driver is OK, there are buffered data and TE is powered OFF
     * We try to turn on the TE. In case the write operation returns OK and 
     * not INPROCESS we also forward the data
     */
    if((ret_value NEQ DRV_OK) ||
       (ste_power_status EQ STE_POWER_ON) || 
       (DRV_OK EQ STE_Write(STE_IDENTIFIER_POWER, 
                            STE_POWER_ON, 
                            STE_POWER_ON)))
    {
        /*
         * reset store variables
         */
        temp_desc = ppp_data->prx.TE_data_buffer_desc_list_first;
        temp_p_id = ppp_data->prx.TE_data_buffer_p_id;
        ppp_data->prx.TE_data_buffer_desc_list_first = NULL;
        ppp_data->prx.TE_data_buffer_p_id            = DTI_PID_UOS;
        ppp_data->prx.TE_data_buffered               = FALSE;
        /*
         * forward stored data
         */
        sig_any_ftx_packet_req(temp_p_id, temp_desc);
    }
  }
} /* new_power_status_TE() */
#endif /* FF_STATUS_TE */

