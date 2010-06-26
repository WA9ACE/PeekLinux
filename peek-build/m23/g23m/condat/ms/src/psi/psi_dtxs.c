/*
+-----------------------------------------------------------------------------
|  File     : psi_dtxs.c
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
|  Purpose  : This modul is part of the entity PSI and implements all 
|             functions to handles the incoming process internal signals as  
|             described in the SDL-documentation (DTX-statemachine)
|  $Identity:$
+-----------------------------------------------------------------------------
*/

#define ENTITY_PSI

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"     /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "dti.h"
#include "psi.h"       /* to get the global entity definitions */
#include "psi_rxs.h"   /* to get TX signal definitions */
#include "psi_txp.h"   /* to get TX signal definitions */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_dtx_close
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_KER_DTX_CLOSE
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_dtx_close (void)
{
  
  TRACE_FUNCTION ("psi_ker_dtx_close()");

  switch(GET_STATE(PSI_SERVICE_DTX)) 
  {
    case PSI_DTX_READY:
    case PSI_DTX_NOT_READY:
      SET_STATE(PSI_SERVICE_DTX, PSI_DTX_DEAD);
      psi_dtx_rx_close();
      break;
    case PSI_DTX_DEAD:
    default:
      TRACE_ERROR( "SIG_KER_DTX_CLOSE unexpected" );
      break;

  } /* switch */

} /* psi_ker_dtx_close() */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_dtx_open
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_KER_DTX_OPEN
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_dtx_open (void)
{
  
  TRACE_FUNCTION ("psi_ker_dtx_open()");

  switch(GET_STATE(PSI_SERVICE_DTX))
  {
    case PSI_DTX_DEAD:
      SET_STATE(PSI_SERVICE_DTX, PSI_DTX_NOT_READY);
      break;
    case PSI_DTX_NOT_READY:
    case PSI_DTX_READY:
    default:
      TRACE_ERROR( "SIG_KER_DTX_CLOSE unexpected" );
      break;
  } /* switch */
} /* psi_ker_dtx_open() */

/*
+------------------------------------------------------------------------------
| Function    : psi_rx_dtx_data_pkt
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_RX_DTX_DATA
|
| Parameters  : data, len of user data, protocol id
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_rx_dtx_data_pkt(T_desc2* buffer, U16 len, U8 p_id)
{
 
  
  TRACE_FUNCTION ("psi_rx_dtx_data_pkt()");

  switch(GET_STATE(PSI_SERVICE_DTX))
  {
    case PSI_DTX_READY:
    {
      PALLOC_DESC2(dti_data_ind, DTI2_DATA_IND);
 
      dti_data_ind->parameters.st_lines.st_break_len = DTI_BREAK_OFF; /* not used */
      dti_data_ind->parameters.st_lines.st_flow = DTI_FLOW_ON;        /* not used */
      dti_data_ind->parameters.st_lines.st_line_sa = DTI_SA_ON;       /* not used */
      dti_data_ind->parameters.st_lines.st_line_sb = DTI_SB_ON;       /* not used */
      dti_data_ind->parameters.st_lines.st_break_len = 0;             /* not used */

      dti_data_ind->parameters.p_id = p_id;                           /* protocol identifier */
      
      dti_data_ind->desc_list2.first = (ULONG)buffer;
      dti_data_ind->desc_list2.list_len = len;

      dti_send_data(psi_data->hDTI,
                    psi_data->instance,    /* U8 instance */
                    0,                     /* U8 interfac */
                    0,                     /* U8 channel */
                    dti_data_ind);
    }
    break;
    case PSI_DTX_DEAD:
    case PSI_DTX_NOT_READY:
    default:
      psi_mfree_desc2_chain(buffer);/* free descriptor list because data sent not possible */
      TRACE_ERROR( "SIG_RX_DTX_DATA unexpected" );
      break;

  } /* switch */

} /* psi_rx_dtx_data() */

/*
+------------------------------------------------------------------------------
| Function    : psi_rx_dtx_data
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_RX_DTX_DATA
|
| Parameters  : data, len of user data, line states 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_rx_dtx_data(T_desc2* buffer, U16 len,T_DIO_CTRL* control_info)
{
 
  T_DIO_CTRL_LINES * ctrl_lin_ptr = NULL;
  T_DIO_CTRL_MUX * ctrl_mux_ptr = NULL;
  
  TRACE_FUNCTION ("psi_rx_dtx_data()");

  switch(GET_STATE(PSI_SERVICE_DTX))
  {
    case PSI_DTX_READY:
    {
      PALLOC_DESC2(dti_data_ind, DTI2_DATA_IND);
 
      switch(control_info->control_type)
      {
        case DIO4_CTRL_LINES:
          ctrl_lin_ptr = (T_DIO_CTRL_LINES *)control_info;
          dti_data_ind->parameters.st_lines.st_break_len = DTI_BREAK_OFF;
          if((ctrl_lin_ptr->state & DIO_X) EQ PSI_DIO_X_OFF)
            dti_data_ind->parameters.st_lines.st_flow = DTI_FLOW_OFF;
          else
            dti_data_ind->parameters.st_lines.st_flow = DTI_FLOW_ON;
          
          if((ctrl_lin_ptr->state & DIO_SA) EQ PSI_DIO_SA_OFF)
            dti_data_ind->parameters.st_lines.st_line_sa = DTI_SA_OFF;
          else
            dti_data_ind->parameters.st_lines.st_line_sa = DTI_SA_ON;

          if((ctrl_lin_ptr->state & DIO_SB) EQ PSI_DIO_SB_OFF)
            dti_data_ind->parameters.st_lines.st_line_sb = DTI_SB_OFF;
          else
            dti_data_ind->parameters.st_lines.st_line_sb = DTI_SB_ON;

          if((ctrl_lin_ptr->state & DIO_BRK)EQ PSI_DIO_BRK_REC)
          {
            dti_data_ind->parameters.st_lines.st_break_len = (U16)(ctrl_lin_ptr->state & DIO_BRKLEN_MASK);
            /* TRACE_EVENT_P1("psi_rx_dtx_data DIO_BRK DIO_BRK %u",dti_data_ind->parameters.st_lines.st_break_len);*/            
          }
          else
          {
            dti_data_ind->parameters.st_lines.st_break_len = DTI_BREAK_OFF;
            /* TRACE_EVENT_P1("psi_rx_dtx_data DIO_BRK DTI_BREAK_OFF %u",dti_data_ind->parameters.st_lines.st_break_len);*/    
          }
          break;
        case DIO4_CTRL_MUX:
           ctrl_mux_ptr = (T_DIO_CTRL_MUX *)control_info;
          if((ctrl_mux_ptr->state & DIO_SA) EQ PSI_DIO_SA_OFF)
            dti_data_ind->parameters.st_lines.st_line_sa = DTI_SA_OFF;
          else
            dti_data_ind->parameters.st_lines.st_line_sa = DTI_SA_ON;

          if((ctrl_mux_ptr->state & DIO_SB) EQ PSI_DIO_SB_OFF)
            dti_data_ind->parameters.st_lines.st_line_sb = DTI_SB_OFF;
          else
            dti_data_ind->parameters.st_lines.st_line_sb = DTI_SB_ON;
          break;
        default:
          break;
      } /* switch */ 
      
      dti_data_ind->desc_list2.first = (ULONG)buffer;
      dti_data_ind->desc_list2.list_len = len;

      dti_send_data(psi_data->hDTI,
                    psi_data->instance,    /* U8 instance */
                    0,                     /* U8 interfac */
                    0,                     /* U8 channel */
                    dti_data_ind);
      
     /* if (psi_data->dtx.state EQ PSI_DTX_READY)
      {
        psi_dtx_rx_ready();
      }*/
    }
      break;
    case PSI_DTX_DEAD:
    case PSI_DTX_NOT_READY:
    default:
      psi_mfree_desc2_chain(buffer);/* free descriptor list because data sent not possible */
      TRACE_ERROR( "SIG_RX_DTX_DATA unexpected" );
      break;

  } /* switch */

} /* psi_rx_dtx_data() */






