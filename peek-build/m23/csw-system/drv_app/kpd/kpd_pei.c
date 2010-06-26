/* 
+----------------------------------------------------------------------------- 
|  Project :  Riv2Gpf
|  Module  :  KPD
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
|  Purpose :  This module implements the process body interface
|             for the entity KPD
|
|             Exported functions:
|
|          pei_create    - Create the Protocol Stack Entity
|          pei_init      - Initialize Protocol Stack Entity
|          pei_exit      - Close resources and terminate
|          pei_run       - Process Messages
|          pei_primitive - Process Primitive
|          pei_signal    - Process Signals             
|          pei_timeout   - Process Timeout
|          pei_config    - Dynamic Configuration
|          pei_monitor   - Monitoring of physical Parameters
|+----------------------------------------------------------------------------- 
*/ 

/*============================ Includes ======================================*/

#ifndef _WINDOWS
#include "nucleus.h"
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "vsi.h"
#include "ccdapi.h"    


#include "pei.h"
#include "tok.h"

#include "kpd/kpd_env.h"
#include "kpd/kpd_i.h"

#include "rvm/rvm_priorities.h"
#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_use_id_list.h"
#include "frm_types.h"

#ifndef _WINDOWS
    #include "rv.cfg"
#endif

#include "rvm/rvm_api.h"
#include "rvt/rvt_gen.h"
//sys_types.h is necessary for function prototypes in buzzer.h
#include "sys_types.h"
#include "buzzer/buzzer.h"
#include "rv/rv_defined_swe.h"
#include "chipset.cfg"

#ifndef _WINDOWS
#  include "power/power.h"
#endif

#include <stdio.h>
#include <string.h>
#include "sys_inth.h"
#include "l1sw.cfg"

/*============================ Macros ========================================*/
#define VSI_CALLER KPD_handle, 
#define pei_create kpd_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*============================ Global Variables ==============================*/
T_HANDLE KPD_handle;
T_HANDLE kpd_hCommKPD = -1;

T_KPD_ENV_CTRL_BLK  gsp_kpd_env_ctrl_blk;
T_KPD_ENV_CTRL_BLK* kpd_env_ctrl_blk = NULL;
static T_KPD_ENV_CTRL_BLK kpd_env_ctrl_blk_data;

static BOOL first_access  = TRUE;

/*============================ Extern Variables ==============================*/
extern int gsp_KPD_intrpt_flag;

/*============================ Extern Functions ==============================*/
extern T_RV_RET kpd_handle_msg(T_RV_HDR*  msg_p);
extern T_RV_RET kpd_handle_timer(UINT8 timer_num);
void power_init();
void  kpd_long_key_sw_handler();

/*============================ Functions =====================================*/



/*
+------------------------------------------------------------------------------
| Function    : pei_config
+------------------------------------------------------------------------------
| Description : This function is called by the frame when a primitive is
|               received indicating dynamic configuration.
|
|               This function is not used in this entity.
|
|Parameters   :  in_string   - configuration string
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_config (char *inString)
{
  RVM_TRACE_DEBUG_HIGH ("KPD: pei_config");
  return PEI_OK;

} /* End of pei_config(..) */


/*
+------------------------------------------------------------------------------
|  Function     :  pei_timeout
+------------------------------------------------------------------------------
|  Description  :  Process timeout.
|
|  Parameters   :  index     - timer index
|
|  Return       :  PEI_OK    - timeout processed
|                  PEI_ERROR - timeout not processed
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_timeout (unsigned short index)
{
  RVM_TRACE_DEBUG_HIGH ("KPD: pei_timeout");
#if (L1_RF_KBD_FIX == 1)
  kpd_long_key_sw_handler();
#endif
  return PEI_OK;

} /* End of pei_timeout(..) */



/*
+------------------------------------------------------------------------------
| Function    : pei_exit
+------------------------------------------------------------------------------
| Description : This function is called by the frame when the entity is
|               terminated. All open resources are freed.
|
| Parameters  : -
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_exit (void)
{
   RVM_TRACE_DEBUG_HIGH ("KPD: pei_exit");
 
   /*
    * Close communication channels
    */
   vsi_c_close (VSI_CALLER kpd_hCommKPD);
   kpd_hCommKPD = VSI_ERROR;
 
   rvf_free_buf(kpd_env_ctrl_blk);
 
   kpd_kill_keypad_driver();
 
   return PEI_OK;
} /* End of pei_exit(..) */

/*
+------------------------------------------------------------------------------
| Function    : pei_primitive
+------------------------------------------------------------------------------
| Description :  This function is called by the frame when a primitive is
|                received and needs to be processed.
|
| Parameters  : prim      - Pointer to the received primitive
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_primitive (void * primptr)
{
   T_RV_HDR* msg_p;

   RVM_TRACE_DEBUG_HIGH("KPD: pei_primitive");

   msg_p = (T_RV_HDR*) P2D(primptr);
   
   kpd_handle_msg(msg_p);

   return PEI_OK;
   
} /* End of pei_primitive(..) */

#ifdef USE_ACTIVE_BODY
/*
+------------------------------------------------------------------------------
| Function    : pei_run
+------------------------------------------------------------------------------
| Description : This function is called by the frame when entering the main
|               loop. This fucntion is only required in the active variant.
|
|               This function is not used.
|
| Parameters  : handle            - Communication handle
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle )
{   
   BOOLEAN error_occured = FALSE;
   T_RV_HDR* msg_p = NULL;
   UINT16 received_event = 0;

   /* loop to process messages */
   while (error_occured == FALSE)
   {
      /* Wait for the necessary events. */
      received_event = rvf_wait ( 0xffff,0);

      if (received_event & RVF_TASK_MBOX_0_EVT_MASK)
      {
         msg_p = (T_RV_HDR*) rvf_read_mbox(0);
         kpd_handle_msg(msg_p);
      }
   }

   return PEI_OK;
} /* End of pei_run(..) */

#endif

extern T_FRM_TASK_TABLE_ENTRY pf_TaskTable  [];
extern USHORT MaxEntities;

void     disable_trace_suspend()
{
int i=0;
for(;i<MaxEntities;i++)
	pf_TaskTable[i].Flags|= TRC_NO_SUSPEND;
}


/*
+------------------------------------------------------------------------------
| Function    : pei_init
+------------------------------------------------------------------------------
| Description : This function is called by the frame. It is used to initialise
|               the entitiy.
|
| Parameters  : handle            - task handle
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_init (T_HANDLE handle)
{
    T_RV_RET ret = RV_OK;


    
    RVM_TRACE_DEBUG_HIGH("KPD: pei_init");
    /*
     * Initialize task handle
     */
    KPD_handle = handle;
    kpd_hCommKPD = -1;

    if(KPD_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("KPD_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( KPD_handle, "KPD" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("KPD: gsp_init_remu_task Not returning RVF_OK");
    }
    /*
     * Open communication channels
     */
    if (kpd_hCommKPD < VSI_OK)
    {
      if ((kpd_hCommKPD = vsi_c_open (VSI_CALLER "KPD" )) < VSI_OK)
        return PEI_ERROR;
    }
    
    /* Create instance gathering all the variable used by EXPL instance. Removing since we have made env block global */

/*    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_KPD_ENV_CTRL_BLK),
                   (T_RVF_BUFFER**)&kpd_env_ctrl_blk) != RVF_GREEN)
    {
       // The environemnt will cancel the EXPL instance creation. 
       return RVM_MEMORY_ERR;
    }
*/    

    kpd_env_ctrl_blk=&kpd_env_ctrl_blk_data;
    /* Store the pointer to the error function */
    kpd_env_ctrl_blk->error_ft = rvm_error;
    /* Store the mem bank id. */
    kpd_env_ctrl_blk->prim_id = EXT_MEM_POOL;
     /* Store the address id */
    kpd_env_ctrl_blk->addr_id = kpd_hCommKPD;
    kpd_env_ctrl_blk->swe_is_initialized = FALSE;
    
    /* Initialise keypad driver */
    ret = kpd_initialize_keypad_driver();
    
    if ( (ret == RV_OK) && (kpd_env_ctrl_blk != 0))
    {
       kpd_env_ctrl_blk->swe_is_initialized = TRUE;
       RVM_TRACE_DEBUG_HIGH("kpd_env_ctrl_blk->swe_is_initialized = TRUE");
    }
 
    disable_trace_suspend();
	
/* moved this from rv_start function. IN REMU rv_start is called from Application Initialize context.   Since this is a blocking call, we cannot afford to block in Application_Initialization. */
    power_init();
    RVM_TRACE_DEBUG_HIGH("KPD: pei_init Complete");
/* enabling KPD interrupt. This was done earlier in init.c but since kpd driver is not initialized at that time key press during boot up time leads to 
   system hang because of continous kpd interrupts */
    F_INTH_ENABLE_ONE_IT(C_INTH_KEYBOARD_IT); 

    return (PEI_OK);
} /* End of pei_init(..) */


/*
+------------------------------------------------------------------------------
| Function    : pei_create
+------------------------------------------------------------------------------
| Description :  This function is called by the frame when the process is
|                created.
|
| Parameters  : out_name          - Pointer to the buffer in which to locate
|                                   the name of this entity
|
| Return      : PEI_OK            - entity created successfuly
|               PEI_ERROR         - entity could not be created
|
+------------------------------------------------------------------------------
*/
GLOBAL SHORT pei_create (T_PEI_INFO **info)
{
static const T_PEI_INFO pei_info =
              {
               "KPD",         /* name */
               {              /* pei-table */
                 pei_init,
                 pei_exit,
                 pei_primitive,        /* pei_primitive */
                 pei_timeout, /* pei_timeout */
                 NULL,	      /* pei_signal */
                 NULL,			/* pei_run -- Active Entity--*/
                 pei_config,        /* pei_config */
                 NULL         /* pei_monitor */
               },
               KPD_STACK_SIZE,      /* stack size */
               10,        /* queue entries */
	       (255-RVM_KPD_TASK_PRIORITY),	/*Priority - 1->low */
               1,        /* number of timers */
               (COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND | PASSIVE_BODY) /* Flags Settings */
              };

  RVM_TRACE_DEBUG_HIGH("KPD: pei_create");

  /*
   * Close Resources if open
   */
  if (first_access)
    first_access = FALSE;
  else
    pei_exit();

  /*
   * Export startup configuration data
   */
  *info = (T_PEI_INFO *)&pei_info;
  
  return PEI_OK;
} /* End of pei_create(..) */

void power_init()
{
#ifndef _WINDOWS
    // Perform switch ON processing.
    Switch_ON();
#endif

#if (_GSM==1)
    BZ_KeyBeep_ON ();          // Audio feedback if ON/OFF pushed
#endif // if (_GSM==1)

}

