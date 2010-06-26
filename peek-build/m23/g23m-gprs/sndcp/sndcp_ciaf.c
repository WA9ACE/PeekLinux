/*
+-----------------------------------------------------------------------------
|  Project :  GPRS (8441)
|  Modul   :  tgen_gprs.cpp
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
|             SDL-documentation (CIA-statemachine)
+-----------------------------------------------------------------------------
*/


#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"         /* to get a lot of macros */
#include "macdef.h"
#include "prim.h"        /* to get the definitions of used SAP and directions */

#include "dti.h"
#include "sndcp.h"       /* to get the global entity definitions */
#include "sndcp_f.h"     /* to get the functions to access the global arrays*/

#ifdef _SNDCP_DTI_2_
#include <string.h>    /* to get memcpy() */
#endif


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : cia_compress_init
+------------------------------------------------------------------------------
| Description : This routine initializes the state structure for both the transmit
|               and receive halves of some serial line. It must be called each
|               time the line is brought up.
|
| Parameters  : number of slots (from 0 to MAX_STATES)
|
+------------------------------------------------------------------------------
*/

GLOBAL void cia_compress_init(UBYTE number_of_slots)
{
  UBYTE i;
  struct cstate *tstate = sndcp_data->cia.comp.tstate;
  struct cstate *rstate = sndcp_data->cia.comp.rstate;

  TRACE_FUNCTION( "cia_compress_init");

  memset((char *)&(sndcp_data->cia.comp), 0, sizeof(sndcp_data->cia.comp));
  for (i = number_of_slots; i > 0; --i) {
    tstate[i].cs_id = i;
    tstate[i].cs_next = &tstate[i - 1];
    tstate[i].cs_ip = (T_SNDCP_IP_HEADER*)tstate[i].cs_hdr;
  }
  tstate[0].cs_next = &tstate[number_of_slots];
  tstate[0].cs_id = 0;
  tstate[0].cs_ip = (T_SNDCP_IP_HEADER*)tstate[0].cs_hdr;
  sndcp_data->cia.comp.last_cs = &tstate[0];
  sndcp_data->cia.comp.last_recv = 255;
  sndcp_data->cia.comp.last_xmit = 255;
  sndcp_data->cia.comp.flags = SLF_TOSS;

  for (i = number_of_slots; i > 0; --i) {
    rstate[i].cs_id = i;
    rstate[i].cs_next = &rstate[i - 1];
    rstate[i].cs_ip = (T_SNDCP_IP_HEADER*)rstate[i].cs_hdr;
  }
  rstate[0].cs_next = &rstate[number_of_slots];
  rstate[0].cs_id = 0;
  rstate[0].cs_ip = (T_SNDCP_IP_HEADER*)rstate[0].cs_hdr;

  sndcp_data->cia.comp.slots_to_use = number_of_slots + 1;
}


/*
+------------------------------------------------------------------------------
| Function    : cia_init
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void cia_init (void)
{
  UBYTE nsapi;
  TRACE_FUNCTION( "cia_init" );
  INIT_STATE(CIA, CIA_DEFAULT);

  sndcp_reset_xid_block(&sndcp_data->cia.cur_xid_block);
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi ++){
    sndcp_data->cia.cur_cia_decomp_ind[nsapi] = NULL;
#ifdef SNDCP_TRACE_ALL
    sndcp_data->cia.cia_decomp_ind_number[nsapi] = 0;
#endif

  }


} /* cia_init() */



