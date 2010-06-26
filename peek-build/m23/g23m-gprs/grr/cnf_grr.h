/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRR
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
|  Purpose :  Dynamic Configuration for GPRS Radio Resource Management (GRR).
+----------------------------------------------------------------------------- 
*/ 

#ifndef CNF_GRR_H
#define CNF_GRR_H


/*==== CONST ================================================================*/
/*
 * CONFIGURATION PARAMETER
 *
 * Description :  The constants define the commands for dynamic
 *                configuration proposals.
 */
#define GRR_STR_IM_INIT     "GRR_IM_INIT"
#define GRR_STR_TBF_CCCH    "GRR_TBF_CCCH"
#define GRR_STR_NO_TBF_CCCH "GRR_NO_TBF_CCCH"
#define GRR_STR_CRP_TRACE   "GRR_CRP_TRACE"
#define GRR_STR_IM_TRACE    "GRR_IM_TRACE"
#define GRR_STR_RES_RANDOM  "GRR_RES_RANDOM"
#define GRR_STR_STD         "STD"
#define GRR_STR_NC2         "NC2_ACTIVE"


#define GRR_NUM_IM_INIT     0
#define GRR_NUM_TBF_CCCH    1
#define GRR_NUM_NO_TBF_CCCH 2
#define GRR_NUM_CRP_TRACE   3
#define GRR_NUM_IM_TRACE    4
#define GRR_NUM_RES_RANDOM  5
#define GRR_NUM_STD         6
#define GRR_NUM_NC2         7

#endif /* !CNF_GRR_H */
