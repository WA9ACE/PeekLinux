/*
+-----------------------------------------------------------------------------
|  File     : psi_kerf.c
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
|             procedures and functions as described in the
|             SDL-documentation (KER-statemachine)
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
#include "psi.h"        /* to get the global entity definitions */
#include "psi_drxs.h"   /* to get signal definitions */
#include "psi_dtxs.h"   /* to get signal definitions */
#include "psi_kerp.h"
#include "P_psi.val"
#include <string.h>

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : psi_ker_assign_ctrl
+------------------------------------------------------------------------------
| Description : This functions checks the line state requests sent by ACI
|
| Parameters  : line_state send by PSI_LINE_STATE_REQ
+------------------------------------------------------------------------------
*/

GLOBAL void psi_ker_assign_ctrl( T_DIO_CTRL_LINES *ptr_dio_ctrl, U16 line_state)
{    
  TRACE_FUNCTION ("psi_ker_assign_ctrl()");
 
  if((psi_data->device_no & DIO_TYPE_DAT_MASK) EQ DIO_DATA_SER)
  {
    ptr_dio_ctrl->control_type = DIO4_CTRL_LINES;
    ptr_dio_ctrl->length = sizeof(T_DIO_CTRL_LINES);
    /*TRACE_EVENT_P1("psi_ker_assign_ctrl old %u",ptr_dio_ctrl->state);*/

    if(((line_state & LINE_STD_RING_ON) EQ PSI_DTI_RING_ON) AND ((ptr_dio_ctrl->state & PSI_DIO_RING_ON) NEQ PSI_DIO_RING_ON))
    {
      ptr_dio_ctrl->state = ptr_dio_ctrl->state | DIO_RING;
      psi_data->tx.psi_dio_flush = TRUE;
      TRACE_FUNCTION("psi_ker_assign_ctrl RING on");
    }
    if(((line_state & LINE_STD_RING_OFF) EQ PSI_DTI_RING_OFF) AND ((ptr_dio_ctrl->state & PSI_DIO_RING_ON) EQ PSI_DIO_RING_ON))
    {
      ptr_dio_ctrl->state = ptr_dio_ctrl->state & ~(DIO_RING);
      psi_data->tx.psi_dio_flush = TRUE;
      TRACE_FUNCTION("psi_ker_assign_ctrl RING off");
    }
    if(((line_state & LINE_STD_DCD_ON) EQ PSI_DTI_DCD_ON) AND ((ptr_dio_ctrl->state & PSI_DIO_SB_OFF) EQ PSI_DIO_SB_OFF))
    {
      ptr_dio_ctrl->state = ptr_dio_ctrl->state & ~(DIO_SB);
      psi_data->tx.psi_dio_flush = TRUE;
      TRACE_FUNCTION("psi_ker_assign_ctrl DCD on, del DIO_SB");
    }   
    if(((line_state & LINE_STD_DCD_OFF) EQ PSI_DTI_DCD_OFF) AND ((ptr_dio_ctrl->state & PSI_DIO_SB_OFF) NEQ PSI_DIO_SB_OFF))
    {
      ptr_dio_ctrl->state = ptr_dio_ctrl->state| DIO_SB;
      psi_data->tx.psi_dio_flush = TRUE;
      TRACE_FUNCTION("psi_ker_assign_ctrl DCD off, set DIO_SB");
    }  
    /*TRACE_EVENT_P1("psi_ker_assign_ctrl new %u",ptr_dio_ctrl->state);*/
  }  
  else
  {
    TRACE_ERROR("psi_ker_assign_ctrl(): Unexpected device type");
  }
}

/*
+------------------------------------------------------------------------------
| Function    : check_flow_control
+------------------------------------------------------------------------------
| Description : This functions checks the flow control values sent by DTI
|
| Parameters  : dio_flow_control set by PSI_SETCONF_REQ
|               sap_flow_control set first by dio_get_config() later by PSI_SETCONF_REQ
|               (set by user)
+------------------------------------------------------------------------------
*/

GLOBAL BOOL check_flow_control(U32* dio_flow_control, U32 sap_flow_control)
{
  T_DIO_CAP_SER * cap_ser_ptr = NULL;
  
  TRACE_FUNCTION ("check_flow_control()");

  cap_ser_ptr = (T_DIO_CAP_SER *)psi_data->ker.capabilities;
 
 /* check character frame with supported capabilities */
    if((cap_ser_ptr->flow_control & sap_flow_control) EQ sap_flow_control)
    {
      *dio_flow_control = sap_flow_control;
      return TRUE;
    }
    else
    {
      return FALSE;
    }
}

/*
+------------------------------------------------------------------------------
| Function    : check_char_frame
+------------------------------------------------------------------------------
| Description : This functions checks the character frame values sent by DTI
|
| Parameters  : dio_char_frame set by PSI_SETCONF_REQ
|               sap_char_frame set first by dio_get_config() later by PSI_SETCONF_REQ
|               (set by user)
+------------------------------------------------------------------------------
*/

GLOBAL BOOL check_char_frame(U32* dio_char_frame, U32 sap_char_frame)
{
   T_DIO_CAP_SER * cap_ser_ptr = NULL;
  
  TRACE_FUNCTION ("check_char_frame()");
  
  cap_ser_ptr = (T_DIO_CAP_SER *)psi_data->ker.capabilities;
 
 /* check character frame with supported capabilities */
    if((cap_ser_ptr->char_frame & sap_char_frame) EQ sap_char_frame)
    {
      *dio_char_frame = sap_char_frame;
      return TRUE;
    }
    else
    {
      return FALSE;
    }
}

/*
+------------------------------------------------------------------------------
| Function    : check_baudrate
+------------------------------------------------------------------------------
| Description : This functions checks the baudrate sent by DTI
|
| Parameters  : sap_baudrate set by PSI_SETCONF_REQ
|               dio_baudrate set first by dio_get_config() later by PSI_SETCONF_REQ
|               (set by user)
+------------------------------------------------------------------------------
*/

GLOBAL BOOL check_baudrate(U32* dio_baudrate,U32 sap_baudrate)
{
  T_DIO_CAP_SER * cap_ser_ptr = NULL;
  
  TRACE_FUNCTION ("check_baudrate()");
  
   cap_ser_ptr = (T_DIO_CAP_SER *)psi_data->ker.capabilities;
  /* check if enable automatic detection of baudrate */
  if((sap_baudrate & DIO4_BAUD_AUTO) EQ PSI_DIO_BAUDR_AUTO )
  {
    *dio_baudrate = PSI_DIO_BAUDR_AUTO;
    return TRUE;

    /* check baudrate which can be automatically detected
    baudrate_help = sap_baudrate-1;   without bit 0
    
    if((cap_ser_ptr->baudrate_auto & baudrate_help) EQ baudrate_help)
    {
      *dio_baudrate = sap_baudrate;
      return TRUE;
    }
    else
    {
      return FALSE;
    } */
  }
  
  /* check baudrate which can not be automatically detected */ 
  if((cap_ser_ptr->baudrate_fixed & sap_baudrate) EQ sap_baudrate)
  {
    *dio_baudrate = sap_baudrate;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : search_highest_baudrate
+------------------------------------------------------------------------------
| Description : search the highest baudrate in DIO capabilities 
|
| Parameters  : DIO capabilities
+------------------------------------------------------------------------------
*/
GLOBAL U32 search_highest_baudrate (T_DIO_CAP_SER * capabilities)
{
  U32 baud_rate = DIO4_BAUD_921600;
  UBYTE i;

  for(i= 0;i<MAX_SER_BAUD_RATE;i++)
  {   
    if((capabilities->baudrate_fixed & baud_rate) EQ baud_rate)
    { 
      return baud_rate;
    }
    else
    {
      baud_rate = baud_rate>>1;
    }
  }
  return 0;
}



/*
+------------------------------------------------------------------------------
| Function    : psi_ker_assign_cause
+------------------------------------------------------------------------------
| Description : This functions maps a result value from a driver call to 
|               a cause value for PSI SAP.
|
| Parameters  : Pointer to the destination cause as defined by the PSI SAP
|               The result value as defined by the driver
|
+------------------------------------------------------------------------------
*/

GLOBAL void psi_ker_assign_cause (U16* cause, U16 result)
{
  TRACE_FUNCTION ("psi_ker_assign_cause()");
  switch(result) 
  {
    case DRV_OK:
      *cause = PSICS_SUCCESS;
      break;
    case DRV_INVALID_PARAMS:
      *cause = PSICS_INVALID_PARAMS;
      break;
    case DRV_INTERNAL_ERROR:
      *cause = PSICS_INTERNAL_DRV_ERROR;
      break;
    default:
      TRACE_EVENT_P1("result %d unexpected.", result);
      break;
  } /* switch */
}

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_set_init_conf
+------------------------------------------------------------------------------
| Description : This functions set the first device configuration. Later the FFS
|               is used for the first driver configuration parameter. In the 
|               first phase we set hardcoded the according parameter
| Parameters  : 

|
+------------------------------------------------------------------------------
*/

GLOBAL void psi_ker_set_init_conf (void)
{  
   T_DIO_CAP_SER * cap_ser_ptr = NULL;
   U32 baud_rat = 0;
 
  TRACE_FUNCTION ("psi_ker_set_init_conf()");
  
  switch(psi_data->ker.capabilities->device_type)
  {
    case DIO_TYPE_SER:
      psi_malloc_dcb_ser();/* allocating dio control block for storing default configuration */
      cap_ser_ptr = (T_DIO_CAP_SER *)psi_data->ker.capabilities;

      if(cap_ser_ptr->baudrate_auto NEQ 0)
      { /* automatical detection of baudrate */
        psi_data->dcb->baudrate = DIO4_BAUD_AUTO;
      }
      else
      {/* no automatical detection of baudrate */
        baud_rat = search_highest_baudrate(cap_ser_ptr);
        if(baud_rat NEQ 0)
        {
          psi_data->dcb->baudrate = baud_rat;
        }
        else
        {
          TRACE_ERROR("psi_ker_set_init_conf: no correct baudrate");
        }         
      }

      psi_data->dcb->char_frame = DIO_CF_8N1;
      psi_data->dcb->flow_control = DIO_FLOW_RTS_CTS;
      if((cap_ser_ptr->device_flags & DIO_FLAG_SLEEP) EQ PSI_DIO_SLEEP_SUPP )
      {
        psi_data->dcb->sleep_mode = DIO_SLEEP_ENABLE;
      }
      else
      {
       psi_data->dcb->sleep_mode = DIO_SLEEP_NOT_VALID;
      }
      
      if((cap_ser_ptr->ser_flags & DIO_FLAG_SER_ESC) EQ PSI_DIO_SER_ESC_ON)
      {
        psi_data->dcb->esc_char = ESC_CHAR_DEFAULT;
        psi_data->dcb->guard_period = DIO_GUARD_PER_DEFAULT;
      }
      else
      {
        psi_data->dcb->esc_char = ESC_CHAR_NOT_VALID;
        psi_data->dcb->guard_period = DIO_ESC_OFF;
      }
      if((psi_data->device_no & DIO_DRV_MASK) EQ DIO_DRV_USB)
      {
        psi_data->dcb->device_type = DIO_DATA_SER;/* actually not correct, DIO_TYPE_SER is the correct value,
                                                     but USBFAX refuses this later in
                                                     usbfax_set_rx_buffer(usbfax_export.c) */
      }
      else
      {/* for all serial driver except USB implementation on E-Sample */
        psi_data->dcb->device_type = DIO_TYPE_SER;
      }
      psi_data->dcb->xoff = XOFF_DEFAULT;
      psi_data->dcb->xon = XON_DEFAULT;
      break;
    case DIO_TYPE_SER_MUX:
      break;
    case DIO_TYPE_PKT:
      psi_malloc_dcb_pkt();/* allocating dio control block for storing default configuration */
        psi_data->dcb_pkt->device_type = DIO_TYPE_PKT;
        psi_data->dcb_pkt->sleep_mode = DIO_SLEEP_DISABLE;
      break;
    default:
      {
      TRACE_ERROR("psi_ker_set_init_conf(): Not supported device type");
        break;
      }
  }       
}
#ifdef _SIMULATION_
/*
+------------------------------------------------------------------------------
| Function    : psi_ker_assign_dcb_sim
+------------------------------------------------------------------------------
| Description : This functions copies the DCB data provided by the SAP into 
|               a T_DIO_DCB data structure as defined by the DIO driver.
|               --- only for simulation ---
| Parameters  : Pointer to the destination structure as defined by the driver
|               Pointer to the source structure as defined by the PSI SAP
|
+------------------------------------------------------------------------------
*/

GLOBAL BOOL psi_ker_assign_dcb_sim (T_DIO_DCB* dcb, U32 dev_type,T_DIO_DCB_UN* sap_dcb)
{   
    T_DIO_DCB_SER * dcb_ser_ptr = NULL;
    T_DIO_DCB_SER * sap_ser_ptr = NULL;
    T_DIO_CAP_SER * cap_ser_ptr = NULL;
  
  TRACE_FUNCTION ("psi_ker_assign_dcb_sim()");

  /* Fill Device Control Block from primitive data */

  switch(dev_type)
  {
    case DIO_DATA_SER:
      dcb_ser_ptr = (T_DIO_DCB_SER *)dcb;
      cap_ser_ptr = (T_DIO_CAP_SER *)psi_data->ker.capabilities;
      sap_ser_ptr = &sap_dcb->DIO_DCB_SER;
      dcb_ser_ptr->device_type = sap_ser_ptr->device_type;
      if((cap_ser_ptr->device_flags & DIO_FLAG_SLEEP) EQ PSI_DIO_SLEEP_SUPP )
      {/* if sleep mode is supported by driver */
        if((sap_ser_ptr->sleep_mode EQ DIO_SLEEP_ENABLE) OR
           (sap_ser_ptr->sleep_mode EQ DIO_SLEEP_DISABLE))
        {
           dcb_ser_ptr->sleep_mode =sap_ser_ptr->sleep_mode;
        }
        else /* wrong sleep mode parameter */
          return FALSE;               
      }
      else
      {/* if sleep mode is not supported by driver */
         dcb_ser_ptr->sleep_mode = DIO_SLEEP_NOT_VALID;
      }
      
      /* if baudrate should be not changed than no baudrate test */
      if(sap_ser_ptr->baudrate NEQ DIO4_BAUD_NOT_VALID)
      {/* check baudrate with automatic and fix baud rate values (capabilities) */
        if(!check_baudrate(&dcb_ser_ptr->baudrate,sap_ser_ptr->baudrate))
        {
          /* wrong baudrate parameter */
          return FALSE; 
        }
      }

      /* if char frame should be not changed than no char frame test */
      if(sap_ser_ptr->char_frame NEQ DIO_CF_NOT_VALID)
      {
        if(( dcb_ser_ptr->baudrate & DIO4_BAUD_AUTO) EQ PSI_DIO_BAUDR_AUTO )
        {/* if automatically baud rate detection then character framing value is invalid */
           dcb_ser_ptr->char_frame = DIO_CF_NOT_VALID;
        }
        else
        {/* check set character frame values with capabilities */
          if(!check_char_frame(&dcb_ser_ptr->char_frame, sap_ser_ptr->char_frame))
          {
            /* wrong character frame parameter */
            return FALSE; 
          }           
        }
      }
      
      /* if flow control should be not changed than no flow control test */
      if(sap_ser_ptr->flow_control NEQ DIO_FLOW_NOT_VALID)
      {     
        /* check set flow control values with capabilities */
        if(!check_flow_control(&dcb_ser_ptr->flow_control, sap_ser_ptr->flow_control))
        {
          /* wrong character frame parameter */
          return FALSE; 
        }     
      }
      
      /* set xon/xoff parameter */
      if(sap_ser_ptr->xoff NEQ XOFF_NOT_VALID)/* not changed */
      {
       dcb_ser_ptr->xoff= sap_ser_ptr->xoff;
      }
      if(sap_ser_ptr->xon NEQ XON_NOT_VALID)/* not changed */
      {
        dcb_ser_ptr->xon = sap_ser_ptr->xon;
      }

      if(cap_ser_ptr->ser_flags NEQ DIO_FLAG_SER_ESC)
      {/* if escape sequence detection is not supported by driver */
        dcb_ser_ptr->esc_char = ESC_CHAR_NOT_VALID;
        dcb_ser_ptr->guard_period = DIO_ESC_OFF;
      }
      else
      {/* set escape sequence character and guard period */
        dcb_ser_ptr->esc_char = sap_ser_ptr->esc_char;
        dcb_ser_ptr->guard_period = sap_ser_ptr->guard_period;
      }                           
      break;
    case DIO_DATA_MUX:
     /* dcb->dio_dcb_ser_mux.device_type = sap_dcb->dio_dcb_ser_mux.device_type;
      dcb->dio_dcb_ser_mux.sleep_mode = sap_dcb->dio_dcb_ser_mux.sleep_mode;
      dcb->dio_dcb_ser_mux.baudrate = sap_dcb->dio_dcb_ser_mux.baudrate;
      dcb->dio_dcb_ser_mux.char_frame = sap_dcb->dio_dcb_ser_mux.char_frame;
      dcb->dio_dcb_ser_mux.flow_control= sap_dcb->dio_dcb_ser_mux.flow_control;
      dcb->dio_dcb_ser_mux.xoff= sap_dcb->dio_dcb_ser_mux.xoff;
      dcb->dio_dcb_ser_mux.xon = sap_dcb->dio_dcb_ser_mux.xon;
      dcb->dio_dcb_ser_mux.esc_char = sap_dcb->dio_dcb_ser_mux.esc_char;
      dcb->dio_dcb_ser_mux.guard_period= sap_dcb->dio_dcb_ser_mux.guard_period;                             
      dcb->dio_dcb_ser_mux.device_mode= sap_dcb->dio_dcb_ser_mux.device_mode;
      dcb->dio_dcb_ser_mux.mux_mode = sap_dcb->dio_dcb_ser.mux_mode;
      dcb->dio_dcb_ser_mux.n1 = sap_dcb->dio_dcb_ser_mux.n1;
      dcb->dio_dcb_ser_mux.n2 = sap_dcb->dio_dcb_ser_mux.n2;
      dcb->dio_dcb_ser_mux.t1 = sap_dcb->dio_dcb_ser_mux.t1;    
      dcb->dio_dcb_ser_mux.t2 = sap_dcb->dio_dcb_ser_mux.t2;
      dcb->dio_dcb_ser_mux.t3 = sap_dcb->dio_dcb_ser_mux.t3;    
      dcb->dio_dcb_ser_mux.k = sap_dcb->dio_dcb_ser_mux.k;  */                                        
      break;
    case DIO_DATA_PKT:
    default:
      break;   
  }
  return TRUE; 
}

#else
/*
+------------------------------------------------------------------------------
| Function    : psi_ker_assign_dcb
+------------------------------------------------------------------------------
| Description : This functions copies the DCB data provided by the SAP into 
|               a T_DIO_DCB data structure as defined by the DIO driver.
|
| Parameters  : Pointer to the destination structure as defined by the driver
|               Pointer to the source structure as defined by the PSI SAP
|
+------------------------------------------------------------------------------
*/

GLOBAL BOOL psi_ker_assign_dcb (T_DIO_DCB* dcb, U32 dev_type, const T_DIO_DCB* sap_dcb)
{   
    T_DIO_DCB_SER * dcb_ser_ptr = NULL;
    T_DIO_DCB_SER * sap_ser_ptr = NULL;
    T_DIO_CAP_SER * cap_ser_ptr = NULL;
  
  TRACE_FUNCTION ("psi_ker_assign_dcb()");

  /* Fill Device Control Block from primitive data */

  switch(dev_type)
  {
    case DIO_DATA_SER:
      dcb_ser_ptr = (T_DIO_DCB_SER *)dcb;
      cap_ser_ptr = (T_DIO_CAP_SER *)psi_data->ker.capabilities;
      sap_ser_ptr = (T_DIO_DCB_SER *)sap_dcb;
      dcb_ser_ptr->device_type = sap_ser_ptr->device_type;
      if((cap_ser_ptr->device_flags & DIO_FLAG_SLEEP) EQ PSI_DIO_SLEEP_SUPP )
      {/* if sleep mode is supported by driver */
        if((sap_ser_ptr->sleep_mode EQ DIO_SLEEP_ENABLE) OR
           (sap_ser_ptr->sleep_mode EQ DIO_SLEEP_DISABLE))
        {
           dcb_ser_ptr->sleep_mode =sap_ser_ptr->sleep_mode;
        }
        else /* wrong sleep mode parameter */
          return FALSE;               
      }
      else
      {/* if sleep mode is not supported by driver */
         dcb_ser_ptr->sleep_mode = DIO_SLEEP_NOT_VALID;
      }
      
      /* if baudrate should be not changed than no baudrate test */
      if(sap_ser_ptr->baudrate NEQ DIO4_BAUD_NOT_VALID)
      {/* check baudrate with automatic and fix baud rate values (capabilities) */
        if(!check_baudrate(&dcb_ser_ptr->baudrate,sap_ser_ptr->baudrate))
        {
          /* wrong baudrate parameter */
          return FALSE; 
        }
      }

      /* if char frame should be not changed than no char frame test */
      if(sap_ser_ptr->char_frame NEQ DIO_CF_NOT_VALID)
      {
        if(( dcb_ser_ptr->baudrate & DIO4_BAUD_AUTO) EQ PSI_DIO_BAUDR_AUTO )
        {/* if automatically baud rate detection then character framing value is invalid */
           dcb_ser_ptr->char_frame = DIO_CF_NOT_VALID;
        }
        else
        {/* check set character frame values with capabilities */
          if(!check_char_frame(&dcb_ser_ptr->char_frame, sap_ser_ptr->char_frame))
          {
            /* wrong character frame parameter */
            return FALSE; 
          }           
        }
      }
      
      /* if flow control should be not changed than no flow control test */
      if(sap_ser_ptr->flow_control NEQ DIO_FLOW_NOT_VALID)
      {     
        /* check set flow control values with capabilities */
        if(!check_flow_control(&dcb_ser_ptr->flow_control, sap_ser_ptr->flow_control))
        {
          /* wrong character frame parameter */
          return FALSE; 
        }     
      }
      
      /* set xon/xoff parameter */
      if(sap_ser_ptr->xoff NEQ XOFF_NOT_VALID)/* not changed */
      {
       dcb_ser_ptr->xoff= sap_ser_ptr->xoff;
      }
      if(sap_ser_ptr->xon NEQ XON_NOT_VALID)/* not changed */
      {
        dcb_ser_ptr->xon = sap_ser_ptr->xon;
      }

      if(cap_ser_ptr->ser_flags NEQ DIO_FLAG_SER_ESC)
      {/* if escape sequence detection is not supported by driver */
        dcb_ser_ptr->esc_char = ESC_CHAR_NOT_VALID;
        dcb_ser_ptr->guard_period = DIO_ESC_OFF;
      }
      else
      {/* set escape sequence character and guard period */
        dcb_ser_ptr->esc_char = sap_ser_ptr->esc_char;
        dcb_ser_ptr->guard_period = sap_ser_ptr->guard_period;
      }                           
      break;
    case DIO_DATA_MUX:
     /* dcb->dio_dcb_ser_mux.device_type = sap_dcb->dio_dcb_ser_mux.device_type;
      dcb->dio_dcb_ser_mux.sleep_mode = sap_dcb->dio_dcb_ser_mux.sleep_mode;
      dcb->dio_dcb_ser_mux.baudrate = sap_dcb->dio_dcb_ser_mux.baudrate;
      dcb->dio_dcb_ser_mux.char_frame = sap_dcb->dio_dcb_ser_mux.char_frame;
      dcb->dio_dcb_ser_mux.flow_control= sap_dcb->dio_dcb_ser_mux.flow_control;
      dcb->dio_dcb_ser_mux.xoff= sap_dcb->dio_dcb_ser_mux.xoff;
      dcb->dio_dcb_ser_mux.xon = sap_dcb->dio_dcb_ser_mux.xon;
      dcb->dio_dcb_ser_mux.esc_char = sap_dcb->dio_dcb_ser_mux.esc_char;
      dcb->dio_dcb_ser_mux.guard_period= sap_dcb->dio_dcb_ser_mux.guard_period;                             
      dcb->dio_dcb_ser_mux.device_mode= sap_dcb->dio_dcb_ser_mux.device_mode;
      dcb->dio_dcb_ser_mux.mux_mode = sap_dcb->dio_dcb_ser.mux_mode;
      dcb->dio_dcb_ser_mux.n1 = sap_dcb->dio_dcb_ser_mux.n1;
      dcb->dio_dcb_ser_mux.n2 = sap_dcb->dio_dcb_ser_mux.n2;
      dcb->dio_dcb_ser_mux.t1 = sap_dcb->dio_dcb_ser_mux.t1;    
      dcb->dio_dcb_ser_mux.t2 = sap_dcb->dio_dcb_ser_mux.t2;
      dcb->dio_dcb_ser_mux.t3 = sap_dcb->dio_dcb_ser_mux.t3;    
      dcb->dio_dcb_ser_mux.k = sap_dcb->dio_dcb_ser_mux.k;  */                                        
      break;
    case DIO_DATA_PKT:
    default:
      break;   
  }
  return TRUE; 
}
#endif /* _SIMULATION_ */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_dti_reason_connection_opened
+------------------------------------------------------------------------------
| Description : reaction to dti callback DTI_REASON_CONNECTION_OPENED
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_dti_reason_connection_opened (void)
{
  TRACE_FUNCTION("psi_ker_dti_reason_connection_opened()");

  
  switch (psi_data->ker.dti_state)
  {
    case PSI_KER_DTI_OPENING:
      psi_data->ker.dti_state = PSI_KER_DTI_READY;
      psi_ker_drx_open();/* start DTI2 - receiving data */
      psi_ker_dtx_open();
      {
        PALLOC(psi_dti_open_cnf, PSI_DTI_OPEN_CNF);
        psi_dti_open_cnf->devId = psi_data->device_no;
        psi_dti_open_cnf->cause = PSICS_SUCCESS;
        psi_dti_open_cnf->link_id = psi_data->link_id;
        PSEND(hCommMMI, psi_dti_open_cnf);
      }
      break;
    case PSI_KER_DTI_DEAD:
    case PSI_KER_DTI_READY:
    default:
      TRACE_ERROR("DTI_REASON_CONNECTION_OPENED unexpected");
  } /* switch */
} /* psi_ker_dti_reason_connection_opened() */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_dti_reason_connection_closed
+------------------------------------------------------------------------------
| Description : reaction to dti callback KER_DTI_REASON_CONNECTION_CLOSED
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_dti_reason_connection_closed (void)
{
  TRACE_FUNCTION("psi_ker_dti_reason_connection_closed()");

  switch (psi_data->ker.dti_state) 
  {
    case PSI_KER_DTI_OPENING:
      psi_data->ker.dti_state = PSI_KER_DTI_DEAD;
      {
        PALLOC(psi_dti_close_ind, PSI_DTI_CLOSE_IND);
        psi_dti_close_ind->devId = psi_data->device_no;
        psi_dti_close_ind->link_id = psi_data->link_id;
        PSEND(hCommMMI, psi_dti_close_ind);
      }
      break;
    case PSI_KER_DTI_READY:
      psi_data->ker.dti_state = PSI_KER_DTI_DEAD;
      psi_ker_drx_close();
      psi_ker_dtx_close();
      {
        PALLOC(psi_dti_close_ind, PSI_DTI_CLOSE_IND);
        psi_dti_close_ind->devId = psi_data->device_no;
        psi_dti_close_ind->link_id = psi_data->link_id;
        PSEND(hCommMMI, psi_dti_close_ind);
      }
      break;
    case PSI_KER_DTI_DEAD:
    default:
      TRACE_ERROR("DTI_REASON_CONNECTION_CLOSED unexpected");
  } /* switch */

} /* psi_ker_dti_reason_connection_closed() */


/*
+------------------------------------------------------------------------------
| Function    : psi_ker_init
+------------------------------------------------------------------------------
| Description : The function psi_ker_init() initializes the PSI
|
| Parameters  : no parameter
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_init  (void)
{
  TRACE_FUNCTION( "psi_ker_init" );

  INIT_STATE(PSI_SERVICE_KER, PSI_KER_DEAD);
  psi_data->ker.dti_state = PSI_KER_DTI_DEAD;
  psi_data->ker.capabilities = NULL;
} /* psi_ker_init() */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_search_basic_data_by_device
+------------------------------------------------------------------------------
| Description : Switches psi_data pointer to the instance of psi with the
|               given device.
|
| Parameters  : device
| Returns     : PSI_DEVICE_FOUND, PSI_DEVICE_NOT_FOUND
|
+------------------------------------------------------------------------------
*/
GLOBAL U16 psi_ker_search_basic_data_by_device (U32 device)
{
  int i = 0;

  TRACE_FUNCTION("psi_ker_search_basic_data_by_device()");

  for( i = 0; i < PSI_INSTANCES; i++ ) 
  {
    psi_data = &(psi_data_base[i]);
    if (psi_data->used &&
       (psi_data->device_no EQ device)) 
    {
      return PSI_DEVICE_FOUND;
    } /* if used and equal */
  } /* for all instances */

  return PSI_DEVICE_NOT_FOUND;

} /* psi_ker_search_basic_data_by_device() */



/*
+------------------------------------------------------------------------------
| Function    : psi_ker_instance_switch
+------------------------------------------------------------------------------
| Description : Switches psi_data pointer to the indicated instance of 
|               psi.
|
| Parameters  : instance
| Returns     : PSI_INST_NOT_FOUND, PSI_INST_OK
|
+------------------------------------------------------------------------------
*/
GLOBAL U16 psi_ker_instance_switch(U8 instance)
{

  TRACE_FUNCTION("psi_ker_instance_switch()");

  psi_data = &(psi_data_base[instance]);
  if (psi_data->used) {
    return PSI_INST_OK;
  }
  return PSI_INST_NOT_FOUND;

} /* psi_ker_instance_switch() */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_new_instance
+------------------------------------------------------------------------------
| Description : Creates a new instance of psi with the given device_no and
|               switches the current pktio_data to this new instance.
|
| Parameters  : device
| Returns     : PSI_NEW_OK, PSI_NEW_FULL, PSI_NEW_USED
|
+------------------------------------------------------------------------------
*/
GLOBAL U16 psi_ker_new_instance (U32 device)
{
  int i = 0;
  BOOL full = TRUE;

  TRACE_FUNCTION( "psi_ker_new_instance" );

  for( i = 0; i < PSI_INSTANCES; i++ )
  {
    psi_data = &(psi_data_base[i]);
    if (psi_data->used EQ FALSE) 
    {
     full = FALSE;
     break;
    }
    if (psi_data->device_no EQ device) 
    {
      return PSI_NEW_USED;
    }
  }
  if (full) 
  {
    psi_data = &(psi_data_base[0]);
    return PSI_NEW_FULL;
  }
  psi_data->used = TRUE;
  psi_data->device_no = device;
  return PSI_NEW_OK;
} /* ker_new_instance() */
