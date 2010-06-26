/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_pdaf.c
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
|             SDL-documentation (PDA-statemachine)
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


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : pda_init
+------------------------------------------------------------------------------
| Description : Initializes the pda service.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void pda_init (void)
{ 
  TRACE_FUNCTION( "pda_init" );
  sndcp_data->pda = & sndcp_data->pda_base[0];
  INIT_STATE(PDA_0, PDA_DEFAULT);
#ifdef SNDCP_2to1
  sndcp_data->pda->sapi = PS_SAPI_3; 
#else
  sndcp_data->pda->sapi = LL_SAPI_3; 
#endif /*SNDCP_2to1*/
  sndcp_data->pda = & sndcp_data->pda_base[1];
  INIT_STATE(PDA_1, PDA_DEFAULT);
#ifdef SNDCP_2to1
  sndcp_data->pda->sapi = PS_SAPI_5; 
#else
  sndcp_data->pda->sapi = LL_SAPI_5; 
#endif /*SNDCP_2to1*/


  sndcp_data->pda = & sndcp_data->pda_base[2];
  INIT_STATE(PDA_2, PDA_DEFAULT);
#ifdef SNDCP_2to1
  sndcp_data->pda->sapi = PS_SAPI_9; 
#else
  sndcp_data->pda->sapi = LL_SAPI_9; 
#endif /*SNDCP_2to1*/


  sndcp_data->pda = & sndcp_data->pda_base[3];
  INIT_STATE(PDA_3, PDA_DEFAULT);
#ifdef SNDCP_2to1
  sndcp_data->pda->sapi = PS_SAPI_11; 
#else
  sndcp_data->pda->sapi = LL_SAPI_11; 
#endif /*SNDCP_2to1*/


} /* pda_init() */

