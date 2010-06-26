/*
+-----------------------------------------------------------------------------
|  Project :  Riv2Gpf
|  Module  :  LCD
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
|             for the entity LCD
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

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"

#include "rvm/rvm_priorities.h"
#include "general.h"

#include "rvm/rvm_gen.h"
#include "rvm/rvm_ext_priorities.h"
#include "rvm/rvm_use_id_list.h"

#include "lcd_manager.h"
#include "lcd_env.h"
#include "lcd_pwr.h"
#include "lcd_transport.h"

/* global pointer to the error function */
T_RVM_RETURN (*lcd_error_ft)(T_RVM_NAME swe_name, T_RVM_RETURN error_cause, \
					T_RVM_ERROR_TYPE error_type,T_RVM_STRING error_msg);

/* global addr id */
T_RVF_ADDR_ID lcd_addr_id;

/* global memory bank ID */
T_RVF_MB_ID lcd_mb_id;

extern UINT16 *ImageDataPtrTemp;


/*============================ Macros ========================================*/
#define VSI_CALLER lcd_handle,
#define pei_create lcd_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*============================ Global Variables ==============================*/
T_HANDLE lcd_handle;
T_HANDLE lcd_hCommLCD = -1;

LOCAL BOOL first_access = TRUE;


/*============================ Function Definition============================*/
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
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_monitor (void ** out_monitor)
{
  RVM_TRACE_DEBUG_HIGH ("LCD: pei_monitor");

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
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_config (char *inString)
{
  RVM_TRACE_DEBUG_HIGH ("LCD: pei_config");

  return PEI_OK;

}/* End of pei_config(..) */


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
LOCAL SHORT pei_timeout (int h1,int h2,USHORT tnum)
{
  TRACE_FUNCTION ("pei_timeout");
  rvf_send_event(lcd_addr_id,(UINT16)(EVENT_MASK(((tnum % RVF_NUM_TASK_TIMERS)+4)) ) );
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
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
  RVM_TRACE_DEBUG_HIGH ("LCD: pei_signal");

  return PEI_OK;

}/* End of pei_signal(..) */


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
   RVM_TRACE_DEBUG_HIGH ("LCD: pei_exit");

   vsi_c_close (VSI_CALLER lcd_hCommLCD);
   lcd_hCommLCD = VSI_ERROR;

   lcd_stop();

   lcd_kill();

   return PEI_OK;
}

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

   return PEI_OK;

}/* End of pei_primitive(..) */


/*
+------------------------------------------------------------------------------
| Function    : pei_run
+------------------------------------------------------------------------------
| Description : This function is called by the frame when entering the main
|               loop. This fucntion is only required in the active variant.
|
|               This function is not used.
|
| Parameters  :  taskhandle  - handle of current process
|                comhandle   - queue handle of current process
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle)
{
  RVM_TRACE_DEBUG_HIGH("LCD: pei_run");

  lcd_start();

  return PEI_OK;

}/* End pei_run(..) */


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

    RVM_TRACE_DEBUG_HIGH("lcd: Initialization");

    /*
     * Initialize task handle
     */
    lcd_handle = handle;
    lcd_hCommLCD = -1;

    if(lcd_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("LCD_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( lcd_handle, "LCD" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }

    /*
     * Open communication channels
     */
    if (lcd_hCommLCD < VSI_OK)
    {
      if ((lcd_hCommLCD = vsi_c_open (VSI_CALLER "LCD" )) < VSI_OK)
        return PEI_ERROR;
    }

    RVM_TRACE_DEBUG_HIGH("LCD Open communication channels done");

     /* Store the address id */
    lcd_addr_id = lcd_handle;
    lcd_mb_id	 =EXT_MEM_POOL;
    lcd_error_ft = rvm_error;

	F_LCD_CAM_NANDFLASH_AQUIRE_BUS;
   	 ret = lcd_init();
	F_LCD_CAM_NANDFLASH_RELEASE_BUS;

    if (ret != RV_OK)
    {
       RVM_TRACE_DEBUG_HIGH("RTC_Initialize() != RV_OK");
       return PEI_ERROR;
    }

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
GLOBAL SHORT pei_create (T_PEI_INFO **info)
{

static const T_PEI_INFO pei_info =
              {
               "LCD",          /* name */
               {               /* pei-table */
                  pei_init,
	          pei_exit,
                  NULL,        /* pei_primitive */
                  NULL,        /* pei_timeout */
                  NULL,        /* pei_signal */
                  pei_run,     /*-- ACTIVE Entity--*/
                  NULL,        /* pei_config */
                  NULL         /* pei_monitor */
	       },
               LCD_STACK_SIZE,            /* stack size */
               2,                        /* queue entries */
               (255-RVM_LCD_TASK_PRIORITY),     /* priority (1->low, 255->high) */
               1,                         /* number of timers */
               COPY_BY_REF|TRC_NO_SUSPEND|PRIM_NO_SUSPEND
              };

  RVM_TRACE_DEBUG_HIGH("LCD: pei_create");

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


/******************************************************************************
* Function	  : xxx_start
*
* Description : This function is called by the RV manager to start the xxx
*				SWE, it is the body of the task.
*
* Parameters  : None
*
* Return      : T_RVM_RETURN
*
* History	  : 0.1 (09-September-2005)
*
*
******************************************************************************/
void lcd_start(void)
{
	UINT32 received_event;

	lcd_fb_coordinates p_lcd_coord;
	p_lcd_coord.start_x=0;
	p_lcd_coord.start_y=0;
	p_lcd_coord.end_x=176;
	p_lcd_coord.end_y=220;


	rvf_send_trace("LCD START STARTED",24, NULL_PARAM,
			   RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );

       rvf_create_timer(lcd_addr_id,0,TRUE, pei_timeout );


	while(1)
		{
	received_event = rvf_wait ((EVENT_MASK(RVF_APPL_EVT_0)), 0);

	if (received_event & EVENT_MASK(RVF_APPL_EVT_0))
		{
		  lcd_pri_if_display(ImageDataPtrTemp,&p_lcd_coord);
		}
	if (received_event & EVENT_MASK(RVF_TIMER_0_EVT_MASK) )
			{
 			  if(lcd_g_state == LCD_ACTIVE)
			  {
			  	lcd_sleep_timer1_expiration();
			  	rvf_start_timer(RVF_TIMER_0,RVF_MS_TO_TICKS(5000),FALSE);
			  }
			  else if(lcd_g_state == DISPLAY_OFF)
			  {
			  	lcd_sleep_timer2_expiration();
			  	rvf_stop_timer(RVF_TIMER_0);
			  }
			  else
			  	{
			  	  lcd_g_state=CLOCK_OFF;
				  rvf_stop_timer(RVF_TIMER_0);
			  	}

			}

		}

}


/******************************************************************************
* Function	  : xxx_stop
*
* Description : This function is called by the RV manager to stop the xxx SWE.
*
* Parameters  : None
*
* Return      : T_RVM_RETURN
*
* History	  : 0.1 (09-September-2005)
*
*
******************************************************************************/
T_RVM_RETURN lcd_stop(void)
{
	/*
	** other SWEs have not been killed yet, xxx can send messages to other SWEs
	*/
	return RVM_OK;
}

/******************************************************************************
* Function	  : xxx_kill
*
* Description : This function is called by the RV manager to kill the xxx
*				SWE, after the xxx_stop function has been called.
*
* Parameters  : None
*
* Return      : T_RVM_RETURN
*
* History	  : 0.1 (09-September-2005)
*
*
******************************************************************************/
T_RVM_RETURN lcd_kill (void)
{
    return RV_OK;
}




/******************************************************************************
* Function	  : xxx_init
*
* Description : This function is called by the RV manager to initialize the
*				xxx SWE before creating the task and calling xxx_start.
*
* Parameters  : None
*
* Return      : T_RVM_RETURN
*
* History	  : 0.1 (09-September-2005)
*
*
******************************************************************************/
T_RVM_RETURN lcd_init(void)
{
rvf_send_trace("LCD INIT TASK STARTED",24, NULL_PARAM,
			   RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );
return(lcd_initialization(DISPLAY_MAIN_LCD));
}


