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
|  Purpose :  This modul is part of the entity LLC and implements all 
|             procedures and functions as described in the 
|             SDL-documentation (UITX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_UITXF_C
#define LLC_UITXF_C
#endif

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "llc.h"        /* to get the global entity definitions */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : uitx_init
+------------------------------------------------------------------------------
| Description : This procedure initialises all necessary variables of
|               ui_frames_tx for all SAPIs.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void uitx_init (void)
{
  TRACE_FUNCTION( "uitx_init" );

  /*
   * Initialise every incarnation of UITX with state TLLI_UNASSIGNED_NOT_READY.
   */
  SWITCH_SERVICE (llc, uitx, 0);
  INIT_STATE (UITX_0, UITX_TLLI_UNASSIGNED_NOT_READY);

  SWITCH_SERVICE (llc, uitx, 1);
  INIT_STATE (UITX_1, UITX_TLLI_UNASSIGNED_NOT_READY);
  
  SWITCH_SERVICE (llc, uitx, 2);
  INIT_STATE (UITX_2, UITX_TLLI_UNASSIGNED_NOT_READY);
  
  SWITCH_SERVICE (llc, uitx, 3);
  INIT_STATE (UITX_3, UITX_TLLI_UNASSIGNED_NOT_READY);
  
  SWITCH_SERVICE (llc, uitx, 4);
  INIT_STATE (UITX_4, UITX_TLLI_UNASSIGNED_NOT_READY);
  
  SWITCH_SERVICE (llc, uitx, 5);
  INIT_STATE (UITX_5, UITX_TLLI_UNASSIGNED_NOT_READY);

  return;
} /* uitx_init() */



/*
+------------------------------------------------------------------------------
| Function    : uitx_init_sapi
+------------------------------------------------------------------------------
| Description : This procedure initialises all necessary variables of
|               ui_frames_tx for the given SAPI.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void uitx_init_sapi (void)
{ 
  TRACE_FUNCTION( "uitx_init_sapi" );
  
  /*
   * <R.LLC.TLLI_ASS.A.007>, <R.LLC.TLLI_ASS.A.008>
   */
  llc_data->uitx->vu = 0;

  return;
} /* uitx_init_sapi() */



/*
+------------------------------------------------------------------------------
| Function    : uitx_build_ui_header
+------------------------------------------------------------------------------
| Description : This procedure inserts the address and control field of an UI 
|               frame in the given sdu. sdu already contains the user data. 
|               The variable cipher indicates the setting of the E bit. The 
|               variable relclass contains the information if the frame shall 
|               be transmitted in protected or unprotected mode, which is 
|               needed in this function to set the PM bit properly.
|
| Parameters  : sdu - a valid pointer to a T_sdu variable, must contain enough
|                     spare octets at the beginning for the LLC header fields
|                     (address + control field = 3 octets for UI frames)
|               cr_bit - C/R bit
|               sapi - a valid SAPI for unacknowledged operation
|               nu - N(U)
|               cipher - ciphering indicator (see LL SAP: LL-UNITDATA-REQ)
|               relclass - QoS reliability class (see LL SAP), must not be set
|                          to LL_RELCLASS_SUB
|
+------------------------------------------------------------------------------
*/
GLOBAL void uitx_build_ui_header 
(
#ifdef LL_DESC
  T_desc_list3* desc_list3,  
#else                             
  T_sdu *sdu,
#endif
  T_BIT cr_bit,
  UBYTE sapi,
  T_FRAME_NUM nu,
  UBYTE cipher,
  UBYTE relclass
) 

{ 

#ifdef LL_DESC
  T_desc3 *desc3 = (T_desc3*)(desc_list3->first); 
  UBYTE *buf = (UBYTE*)(desc3->buffer);
#endif
  
  TRACE_FUNCTION( "uitx_build_ui_header" );

#ifdef LL_DESC
      /*
   * set new offset (3 octets earlier) and new length of desc_list3
   */
  desc3->offset -= UI_CTRL_MIN_OCTETS; 
  desc3->len += UI_CTRL_MIN_OCTETS; 
  desc_list3->list_len += UI_CTRL_MIN_OCTETS;
  /*
   * insert address field (PD is always 0)
   */
  buf[desc3->offset] = ((UBYTE)cr_bit << 6) | sapi; 

  /*
   * insert 1st octet of control field
   * (UI code + 3 most significant bits of N(U))
   */
  buf[desc3->offset+1]= 0xC0 | (UBYTE)(nu >> 6); 

  /*
   * insert 2nd octet of control field
   * (6 least significant bits of N(U) + E bit + PM bit)
   */
  buf[desc3->offset+2]= (((UBYTE)nu & 0x3F) << 2); 

  if (cipher == LL_CIPHER_ON)
  {
    buf[desc3->offset+2] |= 0x02;    
  }

  /*
   * all reliability classes except LL_NO_REL request protected data
   */
  if (relclass != LL_NO_REL)
  {
    buf[desc3->offset+2] |= 0x01; 
  }

  return;

#else

  /*
   * set new offset (3 octets earlier) and new length of sdu
   */
  sdu->o_buf -= UI_CTRL_MIN_OCTETS * 8;
  sdu->l_buf += UI_CTRL_MIN_OCTETS * 8;

  /*
   * insert address field (PD is always 0)
   */
  sdu->buf[sdu->o_buf/8] = ((UBYTE)cr_bit << 6) | sapi;

  /*
   * insert 1st octet of control field
   * (UI code + 3 most significant bits of N(U))
   */
  sdu->buf[(sdu->o_buf/8)+1] = 0xC0 | (UBYTE)(nu >> 6);

  /*
   * insert 2nd octet of control field
   * (6 least significant bits of N(U) + E bit + PM bit)
   */
  sdu->buf[(sdu->o_buf/8)+2] = (((UBYTE)nu & 0x3F) << 2);

  if (cipher == LL_CIPHER_ON)
  {
    sdu->buf[(sdu->o_buf/8)+2] |= 0x02;
  }

  /*
   * all reliability classes except LL_NO_REL request protected data
   */
  if (relclass != LL_NO_REL)
  {
    sdu->buf[(sdu->o_buf/8)+2] |= 0x01;
  }

  return;

#endif /* LL_DESC */

} /* uitx_build_ui_header() */
