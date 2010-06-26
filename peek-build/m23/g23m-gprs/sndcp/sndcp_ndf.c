/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_ndf.c
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
|             SDL-documentation (ND-statemachine)
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
| Function    : nd_init
+------------------------------------------------------------------------------
| Description : The function nd_init() .... 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void nd_init (void)
{ 
  UBYTE nsapi = 0;

  TRACE_FUNCTION( "nd_init" );

  sndcp_data->nd = & sndcp_data->nd_base[0];
  INIT_STATE(ND_0, ND_DEFAULT);
  sndcp_data->nd = & sndcp_data->nd_base[1];
  INIT_STATE(ND_1, ND_DEFAULT);
  sndcp_data->nd = & sndcp_data->nd_base[2];
  INIT_STATE(ND_2, ND_DEFAULT);
  sndcp_data->nd = & sndcp_data->nd_base[3];
  INIT_STATE(ND_3, ND_DEFAULT);
  sndcp_data->nd = & sndcp_data->nd_base[4];
  INIT_STATE(ND_4, ND_DEFAULT);
  sndcp_data->nd = & sndcp_data->nd_base[5];
  INIT_STATE(ND_5, ND_DEFAULT);
  sndcp_data->nd = & sndcp_data->nd_base[6];
  INIT_STATE(ND_6, ND_DEFAULT);
  sndcp_data->nd = & sndcp_data->nd_base[7];
  INIT_STATE(ND_7, ND_DEFAULT);
  sndcp_data->nd = & sndcp_data->nd_base[8];
  INIT_STATE(ND_8, ND_DEFAULT);
  sndcp_data->nd = & sndcp_data->nd_base[9];
  INIT_STATE(ND_9, ND_DEFAULT);
  sndcp_data->nd = & sndcp_data->nd_base[10];
  INIT_STATE(ND_10, ND_DEFAULT);
  sndcp_data->nd = & sndcp_data->nd_base[11];
  INIT_STATE(ND_11, ND_DEFAULT);
  sndcp_data->nd = & sndcp_data->nd_base[12];
  INIT_STATE(ND_12, ND_DEFAULT);
  sndcp_data->nd = & sndcp_data->nd_base[13];
  INIT_STATE(ND_13, ND_DEFAULT);
  sndcp_data->nd = & sndcp_data->nd_base[14];
  INIT_STATE(ND_14, ND_DEFAULT);
  sndcp_data->nd = & sndcp_data->nd_base[15];
  INIT_STATE(ND_15, ND_DEFAULT);
  
  
  
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
    sndcp_data->nd->nsapi = nsapi;
    sndcp_data->nd->npdu_num = 0;
  }
  
  
  
} /* nd_init() */


