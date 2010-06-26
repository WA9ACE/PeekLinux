/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-FaD (8411)
|  Modul   :  IP_TIM
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
|  Purpose :  This Modul defines the timer handling functions
|             for the component
|             Radio Link Protocol of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#define ENTITY_IP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "macdef.h"
#include "pconst.cdg"
#include "custom.h"
#include "gsm.h"
#include "cnf_ip.h"
#include "mon_ip.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "ccdapi.h"

#include "dti.h"
#include "ip.h"
#include "ip_udp.h"

/*==== EXPORT =====================================================*/

/*==== PRIVAT =====================================================*/

/*==== VARIABLES ==================================================*/

/*==== FUNCTIONS ==================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)        MODULE  : IP_TIM                   |
| STATE   : code                  ROUTINE : tim_reassembly           |
+--------------------------------------------------------------------+

  PURPOSE : Timeout of timer T_REASSEMBLY

*/

GLOBAL void tim_reassembly (/* USHORT layer, USHORT segm_server*/)
{
  /*
   * shall be replaced for multiple instances by
   * 
   * UBYTE sel_dl_layer = layer;
   * p_dl->pos_server = segm_server;
   */
  /* T_HILA *p_ul = &(ip_data->hila); */
  T_LOLA *p_dl = &(ip_data->lola);
  T_KER *p_ker = &(ip_data->ker);

  
  TRACE_FUNCTION ("tim_reassembly()");
  

  
  switch (GET_STATE (KER))
  {
    
  case CONNECTED:
    {          

      build_ip_packet(FALSE, B_ICMP_REASSEMBLE_TIMEOUT);
    
      p_dl->state_reassembly[p_dl->pos_server] = NO_SEGMENTS;  
      
      /* 
       *  If ready indication is received - send data_req at once
       */
      
      if(GET_STATE(HILA) EQ WAIT)
      {
        T_DTI2_DATA_REQ  *dti_data_req;
    
        p_ker->send_icmp = FALSE;
          
        dti_data_req        = p_ker->icmp_dti_data_req;
              
        dti_data_req          = p_ker->icmp_dti_data_req;
        dti_data_req->parameters.p_id                  = DTI_PID_IP;
        dti_data_req->parameters.st_lines.st_flow      = DTI_FLOW_ON;
        dti_data_req->parameters.st_lines.st_line_sa   = DTI_SA_ON;
        dti_data_req->parameters.st_lines.st_line_sb   = DTI_SB_ON;
        dti_data_req->parameters.st_lines.st_break_len = DTI_BREAK_OFF;
          
        PACCESS (dti_data_req); 
        {
          PPASS(dti_data_req, dti_data_ind, DTI2_DATA_IND);
          dti_send_data(ip_hDTI, IP_DTI_DEF_INSTANCE, IP_DTI_LL_INTERFACE,
          IP_DTI_DEF_CHANNEL, dti_data_ind);
        }
        dti_start(ip_hDTI, IP_DTI_DEF_INSTANCE, IP_DTI_LL_INTERFACE,
        IP_DTI_DEF_CHANNEL); 
        SET_STATE(HILA, IDLE);
      }  
      else
     
       /* 
        * Send the ICMP-Message later on after ready indication
        */
      
        p_ker->send_icmp = TRUE;       
    }
    break;
  
  default:
    break;
  }

  /* 
   * Free and stop the timer 
   */
  p_dl->timer_reass_running[p_dl->pos_server] = FALSE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)               MODULE  : IP_TIM            |
| STATE   : code                         ROUTINE : tim_init_timer    |
+--------------------------------------------------------------------+

  PURPOSE : Initialise Time-out FIFO and configuration data.

*/

GLOBAL BOOL tim_init_timer (void)
{
  return TRUE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)        MODULE  : IP_TIM                   |
| STATE   : code                  ROUTINE : tim_set_timeout_flag     |
+--------------------------------------------------------------------+

  PURPOSE : Set timeout flag according to timer handle

*/
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)        MODULE  : IP_TIM                   |
| STATE   : code                  ROUTINE : tim_flush_fifo           |
+--------------------------------------------------------------------+

  PURPOSE : Flush timer fifo for timer handle

*/
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)        MODULE  : IP_TIM                   |
| STATE   : code                  ROUTINE : tim_handle_timeout       |
+--------------------------------------------------------------------+

  PURPOSE : execute timeout function depending on t_flag

*/

