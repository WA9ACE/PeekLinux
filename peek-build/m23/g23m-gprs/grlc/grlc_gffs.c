/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRLC
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
|  Purpose :  This module implements signal handler functions for service
|             GFF of entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_GFFS_C
#define GRLC_GFFS_C
#endif

#define ENTITY_GRLC

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"
#include "grlc.h"        /* to get the global entity definitions */
#include "grlc_gfff.h"

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
/*
+------------------------------------------------------------------------------
| Function    : sig_tm_gff_ul_activate
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TM_GFF_UL_ACTIVATE
|
| Parameters  : activate_cause - added for 2 Phase Access support
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tm_gff_ul_activate ( UBYTE activate_cause) 
{ 
  TRACE_ISIG( "sig_tm_gff_ul_activate" );
  
  switch( GET_STATE( GFF ) )
  {
    case GFF_TWO_PHASE:
      if (activate_cause EQ GFF_ACTIVE)
      {
        SET_STATE(GFF,GFF_ACTIVE);
      }
      break; 
    case GFF_ACTIVE:
      if ( (activate_cause EQ GFF_ACTIVE )  AND
           (grlc_data->gff.rlc_status EQ RLC_STATUS_DL ))
      {
        grlc_data->gff.rlc_status = RLC_STATUS_BOTH;
      }
      break;
    case GFF_DEACTIVE:
      if (activate_cause EQ GFF_TWO_PHASE)
      {
        SET_STATE(GFF,GFF_TWO_PHASE);
      }
      else
      if(activate_cause EQ GFF_ACTIVE )
      {
        SET_STATE(GFF,GFF_ACTIVE);
      }
      gff_tbf_init();
      grlc_data->gff.rlc_status = RLC_STATUS_UL;
      break;
    default:
      TRACE_ERROR( "SIG_TM_GFF_UL_ACTIVATE unexpected" );
      break;
  }
} /* sig_tm_gff_ul_activate() */
#else

/*
+------------------------------------------------------------------------------
| Function    : sig_tm_gff_ul_activate
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TM_GFF_UL_ACTIVATE
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tm_gff_ul_activate ( void) 
{ 
  TRACE_ISIG( "sig_tm_gff_ul_activate" );
  
  switch( GET_STATE( GFF ) )
  {
    case GFF_ACTIVE:
    case GFF_DEACTIVE:
      if(grlc_data->gff.rlc_status EQ RLC_STATUS_DL )
      {
        grlc_data->gff.rlc_status = RLC_STATUS_BOTH;
      }
      else if(grlc_data->gff.rlc_status EQ RLC_STATUS_NULL )
      {
        SET_STATE(GFF,GFF_ACTIVE);
        gff_tbf_init();
        grlc_data->gff.rlc_status = RLC_STATUS_UL;
      }
      break;
    default:
      TRACE_ERROR( "SIG_TM_GFF_UL_ACTIVATE unexpected" );
      break;
  }
} /* sig_tm_gff_ul_activate() */
#endif



/*
+------------------------------------------------------------------------------
| Function    : sig_tm_gff_ul_deactivate
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TM_GFF_UL_DEACTIVATE
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tm_gff_ul_deactivate ( void) 
{ 
  TRACE_ISIG( "sig_tm_gff_ul_deactivate" );

  grlc_data->ul_tfi         = 0xFF;  
  
  switch( GET_STATE( GFF ) )
  {
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case GFF_TWO_PHASE:
#endif
    case GFF_ACTIVE:
      if(grlc_data->gff.rlc_status EQ RLC_STATUS_BOTH )
        grlc_data->gff.rlc_status = RLC_STATUS_DL;
      else
      {
        grlc_data->gff.rlc_status = RLC_STATUS_NULL;
        SET_STATE(GFF,GFF_DEACTIVE);    
      }
      break;
    default:
      TRACE_ERROR( "SIG_TM_GFF_UL_DEACTIVATE unexpected" );
      break;
  }
} /* sig_tm_gff_ul_deactivate() */



/*
+------------------------------------------------------------------------------
| Function    : sig_tm_gff_dl_activate
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TM_GFF_DL_ACTIVATE
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tm_gff_dl_activate ( void) 
{ 
  TRACE_ISIG( "sig_tm_gff_dl_activate" );
  
  switch( GET_STATE( GFF ) )
  {
    case GFF_ACTIVE:
    case GFF_DEACTIVE:
      if(grlc_data->gff.rlc_status EQ RLC_STATUS_UL )
        grlc_data->gff.rlc_status = RLC_STATUS_BOTH;
      else if(grlc_data->gff.rlc_status EQ RLC_STATUS_NULL )
      {
        SET_STATE(GFF,GFF_ACTIVE);
        gff_tbf_init();
        grlc_data->gff.rlc_status = RLC_STATUS_DL;
      }
      break;
    default:
      TRACE_ERROR( "SIG_TM_GFF_DL_ACTIVATE unexpected" );
      break;
  }
} /* sig_tm_gff_dl_activate() */



/*
+------------------------------------------------------------------------------
| Function    : sig_tm_gff_dl_deactivate
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TM_GFF_DL_DEACTIVATE
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tm_gff_dl_deactivate ( void) 
{ 
  TRACE_ISIG( "sig_tm_gff_dl_deactivate" );

  grlc_data->dl_tfi         = 0xFF;

  switch( GET_STATE( GFF ) )
  {
    case GFF_ACTIVE:
      if(grlc_data->gff.rlc_status EQ RLC_STATUS_BOTH )
        grlc_data->gff.rlc_status = RLC_STATUS_UL;
      else
      {
        grlc_data->gff.rlc_status = RLC_STATUS_NULL;
        SET_STATE(GFF,GFF_DEACTIVE);    
      }
      break;
    default:
      TRACE_ERROR( "SIG_TM_GFF_DL_DEACTIVATE unexpected" );
      break;
  }
} /* sig_tm_gff_dl_deactivate() */


