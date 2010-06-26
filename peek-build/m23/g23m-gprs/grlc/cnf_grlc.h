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
|  Purpose :  Dynamic Configuration for GPRS Radio Resource Management (GRLC).
+----------------------------------------------------------------------------- 
*/ 

#ifndef CNF_GRLC_H
#define CNF_GRLC_H


/*==== CONST ================================================================*/
/*
 * CONFIGURATION PARAMETER
 *
 * Description :  The constants define the commands for dynamic
 *                configuration proposals.
 */
#define GRLC_STR_TPC_PWR_PAR "GRLC_TPC_PWR_PAR"
#define GRLC_STR_TPC_FIX_PCL "GRLC_TPC_FIX_PCL"
#define GRLC_STR_TPC_TRACE   "GRLC_TPC_TRACE"
#define GRLC_STR_IM_TRACE    "GRLC_IM_TRACE"
#define GRLC_STR_CCD_USAGE   "GRLC_CCD_USAGE"

#define GRLC_NUM_TPC_PWR_PAR 0
#define GRLC_NUM_TPC_FIX_PCL 1
#define GRLC_NUM_TPC_TRACE   2
#define GRLC_NUM_IM_TRACE    3
#define GRLC_NUM_CCD_USAGE   4
#endif /* !CNF_GRLC_H */
