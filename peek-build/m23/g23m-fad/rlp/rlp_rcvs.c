/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  Rlp_rcvs.c
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
|             of incomming signals for the component
|             Radio Link Protocol of the base station
+----------------------------------------------------------------------------- 
*/ 

#ifndef RLP_RCVS_C
#define RLP_RCVS_C
#endif

#define ENTITY_RLP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "vsi.h"
#include "macdef.h"
#include "custom.h"
#include "gsm.h"
#include "cus_rlp.h"
#include "prim.h"
#include "tok.h"
#include "rlp.h"

/*==== COnsT =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

LOCAL void sig_rcv_ker_ui_ind(void)
{
  TRACE_FUNCTION ("sig_rcv_ker_ui_ind()");

  switch (GET_STATE (KER))
  {
    case RLP_ADM_AND_DETACHED:
      /*
       * processing for state RLP_ADM_AND_DETACHED
       */
      break; 

    default:
    {
#ifdef _TARGET_
      USHORT  sduSize = rlp_data->ker.FrameSize<<3;
#else
      USHORT  sduSize = (rlp_data->ker.FrameSize + 3)<<3;
#endif
      /*
       * processing for any other kernel state
       */
      PALLOC_SDU (rlp_ui_ind, RLP_UI_IND, sduSize);

      rlp_ui_ind->sdu.o_buf = 0;
      rlp_ui_ind->sdu.l_buf = sduSize;

      ker_copy_frame_to_sdu
      (
        rbm_get_current_frame (),
        &rlp_ui_ind->sdu
      );
      /*
       * clear the RLP header
       */
      memset (&rlp_ui_ind->sdu.buf[0], 0, HEADER_LEN); /*lint !e419 ( Apparent data overrun for function 'memset')*/
      
#ifdef _SIMULATION_ /* for test purpose clear the CRC trailer */
      memset (&rlp_ui_ind->sdu.buf[(sduSize>>3)-3], 0, 3); 
#endif
      
      PSENDX (L2R, rlp_ui_ind); 
      break;
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_rcv_rawdata_res
+------------------------------------------------------------------------------
|  Description : Process signal SIG_RAWDATA_RES received
|                from process kernel.
|
|
|  Parameters  : pduType -
|                cBit    -
|                pFBit   -
|                nr      -
|                ns      -
|                crc     -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_rcv_rawdata_res 
            (
              T_PDU_TYPE  pduType,
              T_BIT       cBit,
              T_BIT       pFBit,
              T_FRAME_NUM nr,
              T_FRAME_NUM ns,
              BOOL        crc
            )
{
  TRACE_FUNCTION ("sig_ker_rcv_rawdata_res()");

  if (GET_STATE(RCV) NEQ RPDU_WAIT_FOR_A_BLOCK OR crc EQ FALSE)
    return;
  
  switch (pduType)
  {
  case PDU_SABM:
    if (cBit NEQ 0 AND pFBit NEQ 0)
      sig_rcv_ker_sabm_ind ();
   break;

  case PDU_DISC:
    if (cBit NEQ 0)
      sig_rcv_ker_disc_ind (pFBit);
    break;

  case PDU_UA:
    if (cBit EQ 0)
      sig_rcv_ker_ua_ind (pFBit);
    break;

  case PDU_DM:
    if (cBit EQ 0)
      sig_rcv_ker_dm_ind (pFBit);
    break;

  case PDU_TEST:
    sig_rcv_ker_test_ind (cBit, pFBit);
    break;

  case PDU_XID:
    sig_rcv_ker_xid_ind (cBit, pFBit);
    break;

  case PDU_UI:
    sig_rcv_ker_ui_ind ();
    break;

  case PDU_RR_I:
    sig_rcv_ker_rr_i_ind (cBit, pFBit, nr, ns);
    break;

  case PDU_RNR_I:
    sig_rcv_ker_rnr_i_ind (cBit, pFBit, nr, ns);
    break;

  case PDU_SREJ_I:
    sig_rcv_ker_srej_i_ind (cBit, pFBit, nr, ns);
    break;

  case PDU_REJ_I:
    sig_rcv_ker_rej_i_ind (cBit, pFBit, nr, ns);
    break;

  case PDU_RR:
    sig_rcv_ker_rr_ind (cBit, pFBit, nr);
    break;

  case PDU_RNR:
    sig_rcv_ker_rnr_ind (cBit, pFBit, nr);
    break;

  case PDU_SREJ:
    sig_rcv_ker_srej_ind (cBit, pFBit, nr);
    break;

  case PDU_REJ:
    sig_rcv_ker_rej_ind (cBit, pFBit, nr);
    break;

  case PDU_REMAP:
    if (cBit EQ 0 AND pFBit EQ 0)
      sig_rcv_ker_remap_ind ();
    break;

  default: /* ignore block */
    break;
  }
}
