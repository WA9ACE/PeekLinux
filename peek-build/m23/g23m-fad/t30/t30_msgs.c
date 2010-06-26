/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  t30_msgs
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
|  Purpose :  This Modul defines the functions for processing
|             of incomming signals for the component T30
|             of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#ifndef T30_MSGS_C
#define T30_MSGS_C
#endif

#define ENTITY_T30

/*==== INCLUDES ===================================================*/

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "pcm.h"
#include "vsi.h"
#include "macdef.h"
#include "pconst.cdg"
#include "mconst.cdg"
#include "message.h"
#include "ccdapi.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"
#include "cnf_t30.h"
#include "mon_t30.h"
#include "pei.h"
#include "tok.h"
#include "dti.h"      /* functionality of the dti library */
#include "t30.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_MSGS            |
| STATE   : code                       ROUTINE : sig_mux_msg_msg_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal MSG_IND received from process MUX.
            The received fax data are passed to T30.
*/

GLOBAL void sig_mux_msg_msg_ind (T_FAD_DATA_IND *fad_data_ind)
{
  TRACE_FUNCTION ("sig_mux_msg_msg_ind()");

  if (t30_data->bitorder & FBO_REV_DATA) /* reverse the bit order of each byte */
  {
    USHORT i;

    for (i = 0; i < fad_data_ind->sdu.l_buf >> 3; i++)
    {
      fad_data_ind->sdu.buf[i] = BIT_MIRROR[fad_data_ind->sdu.buf[i]];
    }
  }
  sig_msg_ker_mdat_ind (fad_data_ind);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_MSGS            |
| STATE   : code                       ROUTINE : sig_ker_msg_mdat_req|
+--------------------------------------------------------------------+

  PURPOSE : Process converted DTI data received from process KER.
            The received fax data are passed to FAD.
*/

GLOBAL void sig_ker_msg_mdat_req (T_FAD_DATA_REQ *fad_data_req)
{
  TRACE_FUNCTION ("sig_ker_msg_mdat_req()");

  if (t30_data->bitorder & FBO_REV_DATA) /* reverse the bit order of each byte */
  {
    USHORT i;

    for (i = 0; i < fad_data_req->sdu.l_buf >> 3; i++)
    {
      fad_data_req->sdu.buf[i] = BIT_MIRROR[fad_data_req->sdu.buf[i]];
    }
  }
  sig_msg_mux_msg_req (fad_data_req);
}

