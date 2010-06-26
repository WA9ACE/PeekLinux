/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  tra_mgts.c
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
|             of incomming signals for the component TRA
+----------------------------------------------------------------------------- 
*/ 

#ifndef TRA_MGTS_C
#define TRA_MGTS_C
#endif

#define ENTITY_L2R

/*==== INCLUDES ===================================================*/
#include <string.h>

#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "macdef.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "dti.h"

#include "cl_ribu.h"
#include "tra_pei.h"
#include "tra.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+------------------------------------------------------------------------------
|  Function    : sig_up_tra_mgt_break_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_UP_MGT_BREAK_IND received from process DN.
|
|  Parameters  : USHORT break_len
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_up_tra_mgt_break_ind(USHORT break_len)
{
  TRACE_FUNCTION ("sig_up_tra_mgt_break_ind()");

  switch (GET_STATE (MGT))
  {
  case MGT_IDLE:
    sig_mgt_tra_up_break_req();
    sig_mgt_tra_up_send_break_req(break_len);
    break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_dn_tra_mgt_break_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_DN_MGT_BREAK_IND received from process DN. 
|
|  Parameters  : sa   - 
|                sb
|                flow            
|
|  Return      : 
+------------------------------------------------------------------------------
*/
GLOBAL void sig_dn_tra_mgt_break_ind(U8 sa, U8 sb, U8 flow, USHORT break_len)
{
  TRACE_FUNCTION ("sig_dn_mgt_break_ind()");

  switch (GET_STATE (MGT))
  {
  case MGT_IDLE:
    {
    PALLOC (dti_data_ind, DTI2_DATA_IND);
    dti_data_ind->parameters.st_lines.st_line_sa = sa;
    dti_data_ind->parameters.st_lines.st_line_sb = sb;
    dti_data_ind->parameters.st_lines.st_flow = flow;
    dti_data_ind->desc_list2.first                  = 0;
    dti_data_ind->desc_list2.list_len               = 0;
    dti_data_ind->parameters.st_lines.st_break_len  = break_len;
    dti_data_ind->parameters.p_id                   = DTI_PID_UOS;
    tra_data->dn.Brk_dti_data_ind = dti_data_ind;
    }
    sig_mgt_tra_dn_break_req();
    sig_mgt_tra_up_break_req();
    sig_any_tra_dn_send_break_req();
    break;
  }
}
