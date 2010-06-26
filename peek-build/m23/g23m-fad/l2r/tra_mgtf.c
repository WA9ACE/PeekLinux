/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  tra_mgtf.c
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
|  Purpose :  This Modul defines the procedures and functions for
|             the component L2R of the base station
+----------------------------------------------------------------------------- 
*/ 

#ifndef TRA_MGTF_C
#define TRA_MGTF_C
#endif

#define ENTITY_L2R

/*==== INCLUDES ===================================================*/
#include <string.h>

#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "custom.h"
#include "macdef.h"
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
|  Function    : send_tra_dti_cnf
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void send_tra_dti_cnf(T_TRA_DTI_REQ *tra_dti_req)
{
  PALLOC (tra_dti_cnf, TRA_DTI_CNF);

  TRACE_FUNCTION ("send_tra_dti_cnf()");

  tra_dti_cnf->dti_conn = tra_dti_req->dti_conn;
  tra_dti_cnf->link_id  = tra_dti_req->link_id;
  PSENDX (CTRL, tra_dti_cnf);
}

/*
+------------------------------------------------------------------------------
|  Function    : send_tra_deactivate_cnf
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : errnum -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void  send_tra_deactivate_cnf(void)
{
  PALLOC (tra_deactivate_cnf, TRA_DEACTIVATE_CNF);

  TRACE_FUNCTION ("send_tra_deactivate_cnf()");

  PSENDX (CTRL, tra_deactivate_cnf);
}
