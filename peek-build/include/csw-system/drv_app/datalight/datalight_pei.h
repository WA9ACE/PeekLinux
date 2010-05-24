/*-----------------------------------------------------------------------------
|  Project :  
|  Module  :  DL 
+------------------------------------------------------------------------------
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
+------------------------------------------------------------------------------
| Purpose:    Definitions for the Protocol Stack Entity gbi datalight.
+----------------------------------------------------------------------------*/

#ifndef __DATALIGHT_PEI_H_
#define __DATALIGHT_PEI_H_

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get standard types */
#include "vsi.h"        /* to get a lot of macros */
#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"

/*==== CONSTS ===============================================================*/
#if 0
#define VSI_CALLER       dl_task_handle
#define ENTITY_DATA      dl_data
#endif

#define DATALIGHT_TRACE_DEBUG_HIGH(string) \
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID)


#define DATALIGHT_TRACE_DEBUG_HIGH_PARAM(string,param) \
	rvf_send_trace (string,(sizeof(string)-1),param,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID)



#define DATALIGHT_TRACE_DEBUG_ERROR(string) \
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_ERROR,RVM_USE_ID)



#define DATALIGHT_TRACE_DEBUG_ERROR_PARAM(string,param) \
	rvf_send_trace (string,(sizeof(string)-1),param,RV_TRACE_LEVEL_ERROR,RVM_USE_ID)



/*==== EXPORTS ===============================================================*/

extern short pei_create (T_PEI_INFO **info);
#if 0
#define GBI_DATLIGHT_NAME    	"GBI_DATALIGHT"
#define ACI_NAME    		"ACI_GBI_DATALIGHT"
#define DL_TEST_NAME 		"GBI_DATALIGHT_TEST_NAME" 

#ifdef GBI_DATALIGHT_PEI_C

/* Communication handles */ 
extern T_HANDLE                dl_comm_handle ;
extern T_HANDLE                dl_task_handle ;

#endif /* DATALIGHT_PEI_C */

#endif

#endif /* !DATALIGHT_PEI_H */

