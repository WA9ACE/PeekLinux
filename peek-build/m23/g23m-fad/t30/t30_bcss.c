/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  t30_bcss
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

#ifndef T30_BCSS_C
#define T30_BCSS_C
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
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSF            |
| STATE   : code                       ROUTINE : bcs_encode          |
+--------------------------------------------------------------------+

  PURPOSE : This function encodes an HDLC frame.
*/

LOCAL UBYTE bcs_encode(void)
{
  SHORT ret;
  T_sdu *BCI_stream = (T_sdu*)t30_data->BCI_stream;

  BCI_stream->o_buf = 16;
  BCI_stream->l_buf = BIT_STREAM_LEN_BYT * 8;

  ret = ccd_codeMsg(CCDENT_T30, UPLINK, (T_MSGBUF*)BCI_stream, _decodedMsg, HDLC_ADDR);

  if (ret EQ ccdError)
  {
    TRACE_EVENT ("ERROR: ccd_encode") ;
  }

#ifdef _SIMULATION_ /* show contents of buffer */
  if (t30_data->test_mode & TST_BUF)
  {
    if (ret EQ ccdOK)
    {
      ker_debug("ccd_encode", &BCI_stream->buf[3], (USHORT)(BCI_stream->l_buf >> 3));
    }
  }
#endif

  return ((ret EQ ccdError) ? CCD_ERR : CCD_OK);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSS            |
| STATE   : code                       ROUTINE : sig_ker_bcs_bdat_req|
+--------------------------------------------------------------------+

  PURPOSE : Process signal BDAT_REQ received from process KER.
            The received bcs_data is encoded and collected.
*/

GLOBAL void sig_ker_bcs_bdat_req (UBYTE final)
{
  T_FAD_DATA_REQ *fad_data_req_fin;

  TRACE_FUNCTION ("sig_ker_bcs_bdat_req()");

  if (t30_data->fad_data_req EQ NULL)
  {
    PALLOC_SDU (fad_data_req, FAD_DATA_REQ, BCS_FRM_SIZE_BITS);
    memset (fad_data_req, 0, sizeof (T_FAD_DATA_REQ));
    t30_data->fad_data_req = fad_data_req;
    t30_data->stuff_bits = 8;
  }
  else
    bcs_bcs_encode(CTL_C0, FINAL_NO);

  switch (bcs_encode())
  {
  case CCD_OK:
    if (!final)
      return;

    bcs_bcs_encode (CTL_C8, FINAL_YES);
    fad_data_req_fin = t30_data->fad_data_req;
    t30_data->fad_data_req = NULL;
    fad_data_req_fin->final = TRUE;
    fad_data_req_fin->trans_rate = R_BCS;
    sig_bcs_mux_bcs_req(fad_data_req_fin);
    break;

  case CCD_ERR:
    PFREE (t30_data->fad_data_req);
    t30_data->fad_data_req = NULL;
    snd_error_ind(ERR_CCD_ENC);
    break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSS            |
| STATE   : code                       ROUTINE : sig_mux_bcs_bcs_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal BCS_IND received from process MUX.
            The received bcs_data is collected, destuffed and
            decoded. The decoded data is sent to the kernel.
*/

GLOBAL void sig_mux_bcs_bcs_ind (T_FAD_DATA_IND *fad_data_ind)
{
  TRACE_FUNCTION ("sig_mux_bcs_bcs_ind()");

  switch (bcs_fill_bcs_frm(fad_data_ind))
  {
  case BCS_FRM_FILLING:
    return;

  case BCS_FRM_FILLED:
    bcs_bcs_decode();
    break;

  case BCS_FRM_FULL:
    sig_bcs_ker_err_ind(ERR_BUF_FULL);
    break;
  }

  memset (t30_data->bcs_frm, 0, BCS_FRM_SIZE);
}


