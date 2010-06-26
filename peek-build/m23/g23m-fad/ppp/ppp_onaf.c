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
|             SDL-documentation (ONA-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_ONAF_C
#define PPP_ONAF_C
#endif /* !PPP_ONAF_C */

#define ENTITY_PPP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"     /* to get a lot of macros */
#include "custom.h"     /* to get a lot of macros */
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_ppp.h"    /* to get cnf-definitions */
#include "mon_ppp.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "dti.h"        /* to get the DTILIB definitions */
#include "ppp.h"        /* to get the global entity definitions */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : ona_init
+------------------------------------------------------------------------------
| Description : The function ona_init() initializes the option negotiation
|               automaton (ONA) 
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void ona_init ()
{
  UBYTE i;

  TRACE_FUNCTION( "ona_init" );

  for (i = 0;i < ONA_NUM_INC;i++)
  {
    ppp_data->ona = & ppp_data->ona_base[i];

    /* counter for Configure-Request and Terminate-Request sent 	*/
    ppp_data->ona->restarts = 0;		
    /* counter for Configure-Nak and Configure-Reject sent		*/
    ppp_data->ona->failures = 0;		
    /*
     * counter for Configure-Ack received
     * this counter avoids endless loops where the PPP peer always answers with
     * a Configure-Ack, but it does not send a Configure-Req
     */
    ppp_data->ona->loops = 0;

    INIT_STATE( PPP_SERVICE_ONA , ONA_CLOSED );
  }
} /* ona_init() */


/*
+------------------------------------------------------------------------------
| Function    : ona_get_inst_index
+------------------------------------------------------------------------------
| Description : gets the index (0, 1) for the given ptype (DTI_PID_LCP, 
| DTI_PID_IPCP). This function is used when a calling service serves a key for 
| one of the 2 possible instances of services LCR and NCR.
|
| Parameters  : USHORT ptype, UBYTE* index
|
+------------------------------------------------------------------------------
*/
GLOBAL void ona_get_inst_index(USHORT ptype,
                                 UBYTE* index)
{
  TRACE_FUNCTION("ona_get_inst_index");

  if (ptype EQ DTI_PID_LCP) 
  {
    *index = 0;
  } 
  else if (ptype EQ DTI_PID_IPCP) 
  {
    *index = 1;
  } 
  else 
  {
    TRACE_ERROR( "ONA_GET_INST_INDEX unexpected ptype" );
    *index = 0;
  }
}

