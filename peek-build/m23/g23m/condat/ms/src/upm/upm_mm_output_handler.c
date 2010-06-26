/*----------------------------------------------------------------------------
|  Project :  3G PS
|  Module  :  UPM
+-----------------------------------------------------------------------------
|             Copyright 2003 Texas Instruments.
|             All rights reserved. 
| 
|             This file is confidential and a trade secret of Texas 
|             Instruments .
|             The receipt of or possession of this file does not convey 
|             any rights to reproduce or disclose its contents or to 
|             manufacture, use, or sell anything it may describe, in 
|             whole, or in part, without the specific written consent of 
|             Texas Instruments. 
+-----------------------------------------------------------------------------
| Purpose:    Output functions for primitives from UPM to the MM entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES ============================================================*/

#include "upm.h"

#include "upm_mm_output_handler.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : send_mmpm_reestablish_req
+------------------------------------------------------------------------------
| Description : Allocate, pack and send MMPM_REESTABLISH_RES primitive.
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
#ifdef TI_UMTS
void send_mmpm_reestablish_req(U8 most_demanding)
{
  U8  establish_cause;
  (void)TRACE_FUNCTION("send_mmpm_reestablish_req");

  switch ((T_PS_tc)most_demanding) {
  case PS_TC_CONV:
    establish_cause = (U8)MMPM_CAUSE_ESTABL_ORIG_CONVERSATIONAL;
    break;
  case PS_TC_STREAM:
    establish_cause = (U8)MMPM_CAUSE_ESTABL_ORIG_STREAMING;
    break;
  case PS_TC_INTER:
    establish_cause = (U8)MMPM_CAUSE_ESTABL_ORIG_INTERACTIVE;
    break;
  case PS_TC_BG:
    establish_cause = (U8)MMPM_CAUSE_ESTABL_ORIG_BACKGROUND;
    break;
  case PS_TC_SUB:
    establish_cause = (U8)MMPM_CAUSE_ESTABL_ORIG_BACKGROUND;
    break;
  }

  {
    PALLOC(prim, MMPM_REESTABLISH_REQ);

    prim->establish_cause = establish_cause;

    (void)PSEND(hCommMM, prim);
  }
}
#endif /* TI_UMTS */

/*
+------------------------------------------------------------------------------
| Function    : send_mmpm_sequence_res
+------------------------------------------------------------------------------
| Description : Allocate, pack and send MMPM_SEQUENCE_RES primitive.
|
| Parameters  : sn_sequence_cnf   - SN_SEQUENCE_CNF primitive
+------------------------------------------------------------------------------
*/
#ifdef TI_GPRS
void send_mmpm_sequence_res(T_SN_SEQUENCE_CNF *sn_sequence_cnf)
{
  U16  index;
  (void)TRACE_FUNCTION("send_mmpm_sequence_res");
  {
    PALLOC(prim, MMPM_SEQUENCE_RES);

  /*lint -e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    prim->c_npdu_list = sn_sequence_cnf->c_receive_no_list;

    memcpy(&prim->npdu_list, sn_sequence_cnf->receive_no_list,
	   (size_t)sn_sequence_cnf->c_receive_no_list * sizeof(T_MMPM_npdu_list));
    for (index = 0; index < (U16)sn_sequence_cnf->c_receive_no_list; index++)
    {
      prim->npdu_list[index].nsapi                    = sn_sequence_cnf->receive_no_list[index].nsapi;
      prim->npdu_list[index].receive_n_pdu_number_val = sn_sequence_cnf->receive_no_list[index].receive_no;
    }
  /*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
   (void)PSEND(hCommMM, prim);
  }
}
#endif /* TI_GPRS */
/*==== END OF FILE ==========================================================*/
