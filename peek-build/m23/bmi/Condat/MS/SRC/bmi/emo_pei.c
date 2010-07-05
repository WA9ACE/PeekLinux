#include "nucleus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "vsi.h"
#include "ccdapi.h"

#include "pei.h"
#include "tok.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"

#include "rvm/rvm_priorities.h"
#include "rvm/rvm_use_id_list.h"
#include "Lcd_manager.h"
#include "lls_api.h"

/*=========================== MACROS =========================================*/

#define VSI_CALLER EMO_handle, 
#define pei_create emo_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*===========================Global Variables==================================*/

typedef struct
{
    T_RVF_MB_ID     prim_id;
    T_RVF_ADDR_ID   addr_id;
    void*           msg_alarm_event;      
} T_EMO_ENV_CTRL_BLK;

T_EMO_ENV_CTRL_BLK *emo_env_ctrl_blk = NULL;

T_HANDLE EMO_handle;
T_HANDLE emo_hCommEMO = -1;

LOCAL BOOL first_access = TRUE;

static T_RVM_RETURN (*emo_error_ft) (T_RVM_NAME       swe_name,
                                     T_RVM_RETURN     error_cause,
                                     T_RVM_ERROR_TYPE error_type,
                                     T_RVM_STRING     error_msg);

/*===========================Function Definition================================*/
/*
+------------------------------------------------------------------------------
| Function    : pei_monitor
+------------------------------------------------------------------------------
| Description : This function is called by the frame in case sudden entity
|               specific data is requested (e.g. entity Version).
|
| Parameters  : out_monitor       - return the address of the data to be
|                                   monitoredCommunication handle
|
| Return      : PEI_OK            - successful (address in out_monitor is valid)
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_monitor (void ** out_monitor)
{
  RVM_TRACE_DEBUG_HIGH ("EMO: pei_monitor");

  return PEI_OK;
} /* End pei_monitor(..) */


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
| Return      : PEI_OK            - sucessful
|               PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_config (char *inString)
{
  RVM_TRACE_DEBUG_HIGH ("EMO: pei_config");

  return PEI_OK;

}/* End pei_config(..) */


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
  RVM_TRACE_DEBUG_HIGH ("EMO: pei_timeout");

  return PEI_OK;
}/* End pei_timeout(..) */


/*
+------------------------------------------------------------------------------
| Function    : pei_signal
+------------------------------------------------------------------------------
| Description : This function is called by the frame when a signal has been
|               received.
|
| Parameters  : opc               - signal operation code
|               *data             - pointer to primitive
|
| Return      : PEI_OK            - signal processed
|               PEI_ERROR         - signal not processed
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
  RVM_TRACE_DEBUG_HIGH ("EMO: pei_signal");
  
  return(PEI_OK);
}/* End pei_signal(..) */


/*
+------------------------------------------------------------------------------
|  Function     :  pei_primitive
+------------------------------------------------------------------------------
|  Description  :  Process protocol specific primitive.
|
|  Parameters   :  prim      - pointer to the received primitive
|
|  Return       :  PEI_OK    - function succeeded
|                  PEI_ERROR - function failed
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_primitive (void * primptr)
{
  T_RV_HDR* msg_p;

   RVM_TRACE_DEBUG_HIGH("EMO: pei_primitive");

   return PEI_OK;
}/* End pei_primitive(..) */


/*
+------------------------------------------------------------------------------
| Function    : pei_run
+------------------------------------------------------------------------------
| Description : This function is called by the frame when entering the main
|               loop. This function is only required in the active variant.
|
|               This function is not used.
|
|  Parameters   :  taskhandle  - handle of current process
|                  comhandle   - queue handle of current process
|
| Return      : PEI_OK            - sucessful
|               PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
extern BOOL powered_on;
extern void mmiInit( void);
extern void lcd_init_cmdsequence();
extern int lcd_g_state;
extern UINT8 lcd_pwr_interface(UINT8);
extern void UiTask(void);

LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle)
{  
  int i;
  RVM_TRACE_DEBUG_HIGH("EMO: pei_run");

  powered_on=1;
 // mmiInit();
	UiTask();
  
  return RV_OK;  

}/* End pei_run(..) */


/*
+------------------------------------------------------------------------------
| Function    : pei_exit
+------------------------------------------------------------------------------
| Description : This function is called by the frame when the entity is
|               terminated. All open resources are freed.
|
| Parameters  : -
|
| Return      : PEI_OK            - exit successful
|               PEI_ERROR         - exit not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_exit (void)
{
   RVM_TRACE_DEBUG_HIGH ("EMO: pei_exit");
 
   /*
    * Close communication channels
    */
 
   vsi_c_close (VSI_CALLER emo_hCommEMO);
   emo_hCommEMO = VSI_ERROR;

   return PEI_OK;
}/* End pei_exit(..) */


/*
+------------------------------------------------------------------------------
| Function    : pei_init
+------------------------------------------------------------------------------
| Description : This function is called by the frame. It is used to initialise
|               the entitiy.
|
| Parameters  : handle            - task handle
|
| Return      : PEI_OK            - entity initialised
|               PEI_ERROR         - entity not (yet) initialised
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_init (T_HANDLE handle)
{
    T_RV_RET ret = RV_OK;
    
    RVM_TRACE_DEBUG_HIGH ("EMO: pei_init");

    /*
     * Initialize task handle
     */
    EMO_handle = handle;
    emo_hCommEMO = -1;

    if(EMO_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("EMO_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( EMO_handle, "EMO" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }

    /*
     * Open communication channels
     */
    if (emo_hCommEMO < VSI_OK)
    {
      if ((emo_hCommEMO = vsi_c_open (VSI_CALLER "EMO" )) < VSI_OK)
        return PEI_ERROR;
    }
   
    RVM_TRACE_DEBUG_HIGH("EMO Open communication channels done"); 
  
     /* Create instance gathering all the variable used by EMO instance */
     
	if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_EMO_ENV_CTRL_BLK), (T_RVF_BUFFER**)&emo_env_ctrl_blk) != RVF_GREEN)
	{
            /* The environemnt will cancel the EXPL instance creation. */
            RVM_TRACE_DEBUG_HIGH ("EMO: Error to get memory ");

            return RVM_MEMORY_ERR;	
	}

    /* Store the address ID. */
    emo_env_ctrl_blk->addr_id = EMO_handle;

    /* Store the pointer to the error function. */
    emo_error_ft = rvm_error;

    emo_env_ctrl_blk->prim_id = EXT_MEM_POOL;
    
    return (PEI_OK);
} /* End pei_init(..) */


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
#define RVM_EMO_TASK_PRIORITY 215
GLOBAL SHORT pei_create (T_PEI_INFO **info)
{

static const T_PEI_INFO pei_info =
              {
               "EMO",         /* name */
               {              /* pei-table */
                  pei_init,
                  pei_exit,
                  NULL,           /* NO pei_primitive */
                  NULL,           /* NO pei_timeout */
                  NULL,           /* NO pei_signal */   
                  pei_run,        /*-- ACTIVE Entity--*/
                  NULL,           /* NO pei_config */
                  NULL            /* NO pei_monitor */
               },
               0x2000,            /* stack size */
               2,                        /* queue entries */
		(255-RVM_EMO_TASK_PRIORITY),     /* priority (1->low, 255->high) */
               0,                         /* number of timers */
               COPY_BY_REF	/* Flags Settings */
              };

  RVM_TRACE_DEBUG_HIGH("EMO: pei_create");

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
}/* End pei_create(..) */

/*======================== END OF FILE =========================================*/
