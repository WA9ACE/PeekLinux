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
|             procedures and functions as described in the 
|             SDL-documentation (PRX-statemachine)
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

#ifdef FF_STATUS_TE
#include "gdi.h"        /* To include driver type-defines */
#include "ste.h"        /* To include function interface for TE status driver */
#include "ppp_prxf.h"   /* To include call-back function for TE status driver */
#endif /* FF_STATUS_TE */
/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : prx_init
+------------------------------------------------------------------------------
| Description : The function prx_init() initializes Packet Receive (PRX)
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void prx_init ()
{ 
  TRACE_FUNCTION( "prx_init" );
  
#if defined FF_STATUS_TE
  /*
   * Init of the TE status driver - the handle is unique inside PPP only, SMN
   * PPP handle-management are placed in the end of ppp.h 
   */
  if (DRV_OK NEQ STE_Init( STE_HANDLE, cb_status_TE ))
  {
    /*
     * Error during init of TE status driver - either it is already init or it has failed 
     */
    TRACE_ERROR( "STE driver init. error" );
  }
  else
  {

#ifdef _SIMULATION_
    TRACE_EVENT( "Return DRV_OK from STE_Init()" );
#endif /* _SIMULATION_ */

    /*
     * Setup of used signal-types for driver to trig a callback
     */
    if (DRV_OK NEQ STE_SetSignal( DRV_SIGTYPE_READ ))
    {
      /* 
       * Error during signal-set - either signal illegal, not available or internal fail 
       */
      TRACE_ERROR( "STE driver setup error" );
    }
#ifdef _SIMULATION_
    else
    {
      TRACE_EVENT( "Return DRV_OK from STE_SetSignal()" );
    }
#endif /* _SIMULATION_ */
  }
  /*
   * initialize variables
   */
  ppp_data->prx.TE_data_buffer_desc_list_first = NULL;
  ppp_data->prx.TE_data_buffer_p_id            = DTI_PID_UOS;
  ppp_data->prx.TE_data_buffered               = FALSE;
#endif /* FF_STATUS_TE */

   INIT_STATE( PPP_SERVICE_PRX , PRX_DEAD );
} /* prx_init() */



#ifdef FF_STATUS_TE
/*
+------------------------------------------------------------------------------
| Function    : cb_status_TE
+------------------------------------------------------------------------------
| Description : Handles the call-back from the TE status driver and sends the
|               NEW_POWER_STATUS signal to the PS
|
| Parameters  : None
|
+------------------------------------------------------------------------------
*/
GLOBAL void cb_status_TE (T_DRV_SIGNAL * signal)
{ 

  TRACE_FUNCTION( "cb_status_TE" );

  if((signal->SignalType EQ DRV_SIGTYPE_READ) &&
     (signal->DrvHandle  EQ STE_HANDLE) &&
     (*((USHORT*)signal->UserData)  EQ STE_IDENTIFIER_POWER))
  {
    PSIGNAL(hCommPPP, NEW_POWER_STATUS, NULL);
  }
  else
  {
    TRACE_ERROR( "STE driver: Signal not supported" );
  }
} /* cb_status_TE() */
#endif /* FF_STATUS_TE */
