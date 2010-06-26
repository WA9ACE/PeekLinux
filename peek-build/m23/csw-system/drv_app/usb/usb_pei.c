/* 
+----------------------------------------------------------------------------- 
|  Project :  Riv2Gpf
|  Module  :  USB
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
|             for the entity USB
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "general.h"
#include "nucleus.h"
/* Just to Remove Funny Warnings */
typedef unsigned char       UCHAR;
typedef unsigned int        UINT;
#include "typedefs.h"
#include "vsi.h"
#include "ccdapi.h"

#include "pei.h"
#include "tok.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"

#include "rvm/rvm_ext_priorities.h"
#include "rvm/rvm_use_id_list.h"

#include "chipset.cfg"
#include "usb/usb_cfg.h"
#include "usb/usb_i.h"
#include "usb/usb_inth.h"
#include "usb/usb_env.h"
#include "usb/usb_cfg_dat.h"
#include "usb/usb_inth.h"
#include "usb/usb_dma.h"

#if (CHIPSET==15)
#include "types.h"
#include "otg/bspUsblh_Otg.h"
#endif

#ifndef _WINDOWS
	#include "inth/sys_inth.h"
#if (CHIPSET!=15)
	#include "abb/abb.h"
#endif	
	#include "inth/iq.h"
#endif


/*============================ Macros ========================================*/
#define VSI_CALLER USB_handle, 
#define pei_create usb_pei_create
#if 0
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )
#endif

#define RVM_TRACE_DEBUG_HIGH(string) 

/*============================ Global Variables ==============================*/
T_HANDLE USB_handle;
T_HANDLE usb_hCommUSB = -1;

T_USB_ENV_CTRL_BLK *usb_env_ctrl_blk_p = NULL;

static BOOL first_access = TRUE;
#ifndef USB_DYNAMIC_CONFIG_SUPPORT
static
#endif
void conf_tot_descr_size(UINT16* cfg_size_cntr, UINT8 cfg_nr);

T_RVF_MUTEX usb_tx_mutex;

/*============================ Extern Functions ==============================*/
extern void usb_timer_expiration (UNSIGNED id);


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

/*
LOCAL SHORT pei_monitor (void ** out_monitor)
{
  RVM_TRACE_DEBUG_HIGH ("USB: pei_monitor");

  return PEI_OK;

} // End pei_monitor(..) 
*/

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

/*
LOCAL SHORT pei_config (char *inString)
{
  RVM_TRACE_DEBUG_HIGH ("USB: pei_config");

  return PEI_OK;

}// End of pei_config(..) 

*/

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
  RVM_TRACE_DEBUG_HIGH ("USB: pei_timeout");

	if((USB_CNFGNR != USB_CONF_DEFAULT) &&
		(USB_CNFGNR != USB_CONF_ADRESSED)) 
	{
  notify_rx_buffer_full(usb_env_ctrl_blk_p->trackMissingZLP.pep_idx, usb_env_ctrl_blk_p->trackMissingZLP.size,
  						USB_EOT);
	}

  return PEI_OK;

}/* End of pei_timeout(..) */


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

/*
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
  RVM_TRACE_DEBUG_HIGH ("USB: pei_signal");
  
  return PEI_OK;

}// End of pei_signal(..) 
*/


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
   RVM_TRACE_DEBUG_HIGH ("USB: pei_exit");
 
   /*
    * Close communication channels
    */
   vsi_c_close (VSI_CALLER usb_hCommUSB);
   usb_hCommUSB = VSI_ERROR;

   usb_stop (NULL);
   usb_kill();
   return PEI_OK;
}/* End of pei_exit(..) */

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
   T_RV_HDR* msg_p ; 

   RVM_TRACE_DEBUG_HIGH("USB: pei_primitive");

   msg_p = (T_RV_HDR*) P2D(primptr);
   usb_handle_message(msg_p);

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
| Parameters  : handle            - Communication handle
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/

/*
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle)
{
   BOOLEAN error_occured = FALSE;
   T_RV_HDR* msg_p = NULL;
   UINT16 received_event = 0;
   char buf[64] = "";

   RVM_TRACE_DEBUG_HIGH("USB: pei_run");

   // loop to process messages 
   while (error_occured == FALSE)
   {
      // Wait for the necessary events. 	   
      received_event = rvf_wait ( 0xffff,0);

      if (received_event & RVF_TASK_MBOX_0_EVT_MASK)
      {
         msg_p = (T_RV_HDR*) rvf_read_mbox(0);
         usb_handle_message(msg_p);
      }
  	
   }
   return PEI_OK;
  
}// End of pei_run(..) 
*/


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
    
    RVM_TRACE_DEBUG_HIGH("USB: Initialization");
    /*
     * Initialize task handle
     */
    USB_handle = handle;
    usb_hCommUSB = -1;

     if(USB_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("USB_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( USB_handle, "USB" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }
    /*
     * Open communication channels
     */
    if (usb_hCommUSB < VSI_OK)
    {
      if ((usb_hCommUSB = vsi_c_open (VSI_CALLER "USB" )) < VSI_OK)
        return PEI_ERROR;
    }
    
    RVM_TRACE_DEBUG_HIGH("USB Open communication channels done"); 

    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_USB_ENV_CTRL_BLK), (T_RVF_BUFFER**)&usb_env_ctrl_blk_p) == RVF_RED)
    {
        /* The environemnt will cancel the EXPL instance creation. */
        RVM_TRACE_DEBUG_HIGH ("USB: Error to get memory ");
	
        return RVM_MEMORY_ERR;	
	
    }

    /* Store the address ID. */
    usb_env_ctrl_blk_p->addr_id = handle;

    /* Store the pointer to the error function. */
    usb_env_ctrl_blk_p->error_ft = rvm_error;

    usb_env_ctrl_blk_p->prim_mb_id = EXT_MEM_POOL;
   
    rvf_initialize_static_mutex (&usb_tx_mutex);
#ifdef USB_DYNAMIC_CONFIG_SUPPORT    
	create_configuration_descriptor(USB_MS);
#endif    
    usb_init();
    usb_start();	

    RVM_TRACE_DEBUG_HIGH("USB: pei_init Complete"); 

    return RVM_OK;
} /* End pei_init */

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
               "USB",         /* name */
               {              /* pei-table */
                  pei_init,
	          pei_exit,
                  pei_primitive,   /*-- PASSIVE Entity--*/
                  pei_timeout,            /* pei_timeout */
                  NULL, 	   /* pei_signal */
                  NULL,            /* pei_run */  
                  NULL,            /* pei_config */
                  NULL             /* pei_monitor */
	       },
               USB_STACK_SIZE,            /* stack size */
               10,                        /* queue entries */
               (255-USB_TASK_PRIORITY),     /* priority (1->low, 255->high) */
               1,                         /* number of timers */
               COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND | PASSIVE_BODY
		       /* Flags Settings */
              };

  RVM_TRACE_DEBUG_HIGH("USB: pei_create");

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

/*
+------------------------------------------------------------------------------
| Function    : usb_init
+------------------------------------------------------------------------------
| Description : This function is called to initialise.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usb_init (void)
{
	/* Memory bank status (red, yellow, green). */
	T_RVF_MB_STATUS mb_status;

	T_RV_RET  ret = USB_OK;
	UINT8	  cntr = 0;
	UINT8     ep_ctr = 0;
	UINT8     cfg_ctr = 0;
	UINT8     if_ctr = 0;
	UINT16	  cfg_size_cntr;
	UINT8	  dma_ctr = 0;

	CHAR		dma_rx_timer_name[8];

	unsigned int	length = 0;

       (*(volatile UINT16 *)0xFFFFFD0C)|=2; //make usb clock=52MHz

       USB_SEND_TRACE("USB: usb init called by RVM", RV_TRACE_LEVEL_DEBUG_LOW);
	/*
	 * Here the instance (usb_env_ctrl_blk_p) could be initialised.
	 */
	change_internal_state(USB_INITIALISING);
	rvf_initialize_static_mutex(&usb_env_ctrl_blk_p->mutex);
	rvf_initialize_static_mutex(&usb_env_ctrl_blk_p->dma_mutex);

	usb_env_ctrl_blk_p->usb_return_path.addr_id = usb_env_ctrl_blk_p->addr_id;
	usb_env_ctrl_blk_p->usb_return_path.callback_func = NULL;
	usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl = NULL;
	usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl = NULL;
	usb_env_ctrl_blk_p->endpoint_rx_descr_tbl = NULL;
	usb_env_ctrl_blk_p->endpoint_tx_descr_tbl = NULL;
	usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl = NULL;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	/* Initialization for USB Event Notification Enhancement */
    usb_env_ctrl_blk_p->ntfcCallback = NULL;
	usb_env_ctrl_blk_p->ntfcCtx = NULL;
	usb_env_ctrl_blk_p->enumInfo.info= USB_MS|USB_AT|USB_TRACE;
#endif
	/**
	 * All global variables are initalised
	 */
	/*default configuration is 1*/
	USB_CNFGNR = USB_CONF_DEFAULT;

	/*
	 * During this phase the SWE in not yet initialised
	 * change_internal_state(USB_UNINITIALISED);
	 */

	/*set global DS_chg_mem variable to 0*/
	usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem = W2FC_16BIT_RESET; 

	/*set the hardware state to un attached*/
	usb_env_ctrl_blk_p->host_data.hw_state = USB_HW_UNATTACHED;
	usb_env_ctrl_blk_p->host_data.state_prior_to_sus = USB_HW_UNATTACHED;

	/*disabled remote wake up of the USB HOST by this device*/
	usb_env_ctrl_blk_p->host_data.usb_rem_wake_enabled = FALSE;

	/*set setup variables*/
	usb_env_ctrl_blk_p->host_data.ep0_setup.setup_in = FALSE;
	usb_env_ctrl_blk_p->host_data.ep0_setup.setup_out = FALSE;
	usb_env_ctrl_blk_p->host_data.ep0_setup.current_setup_req = NULL;
	usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req = 0;
	usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc = 0;
	usb_env_ctrl_blk_p->host_data.ep0_setup.rx_or_tx_ep0_int = USB_RX_EP0_INT;	//default to RX int
	usb_env_ctrl_blk_p->host_data.ep0_setup.if_nbr = 0xFF;
	usb_env_ctrl_blk_p->host_data.ep0_data_p =	NULL;
	usb_env_ctrl_blk_p->host_data.fifo_not_full = FALSE;
	usb_env_ctrl_blk_p->bm_rx_ep_buf_stall = USB_ZERO_BMP;
	usb_env_ctrl_blk_p->bm_tx_ep_buf_stall = USB_ZERO_BMP;
	
	/*
	 * TODO check this for every possible cnfg and not for the total of if's
	 * it is possible that more ep's are defined as there are physical ep's as long
	 * as they are not all together in the same configuration check if the configuration 
	 * is valid
	 */

	for(cfg_ctr = 0; cfg_ctr < USB_DEV_NUM_CFG; cfg_ctr++)
	{
		for(if_ctr = 0; if_ctr < configurations_table[cfg_ctr].if_cfg_descr_p->bNumInterfaces; if_ctr++)
		{
			ep_ctr += configurations_table[cfg_ctr].if_cnfg_p[if_ctr].ifdescr_p->bNumEndpoints;
		}
		if(ep_ctr >= USB_MAX_EP)
		{
			ret = USB_INTERNAL_ERR;
			/*there are more endpoints defined to be configured as there are physical endpoints*/
			USB_TRACE_WARNING_PARAM("USB: too many endpoints in configuration ", cfg_ctr);
		}
		/*anyway set counter back to 0*/
		ep_ctr = 0;
	}
	
	/* 
	 * Determine the total size of the configuration total descriptors
	 * and store it in the therewith belonging configuration descriptor
	 */
	for(cfg_ctr = 0; 
		cfg_ctr < USB_DEV_NUM_CFG; 
		cfg_ctr++)
	{
		USB_TRACE_WARNING_PARAM("USB: configurations_table[cfg_ctr].if_cfg_descr_p->bNumInterfaces ",configurations_table[cfg_ctr].if_cfg_descr_p[cfg_ctr].bNumInterfaces )

		conf_tot_descr_size(&cfg_size_cntr, cfg_ctr);
		usb_conf_descr_table[cfg_ctr].wTotalLength = cfg_size_cntr;
		USB_TRACE_WARNING_PARAM("USB: cfg_size_cntr",cfg_size_cntr);
	}
	USB_SEND_TRACE("USB: usb init -> total conf descr size calculated ->OK ", RV_TRACE_LEVEL_DEBUG_LOW);

	/* interrupt handler configurered*/
	if(usb_int_init() != USB_OK)
	{
		ret = USB_INTERNAL_ERR;
		USB_SEND_TRACE("USB: Error to configure interrupt handler",RV_TRACE_LEVEL_ERROR);
		/*init failed, return to uninitialised state*/
		change_internal_state(USB_UNINITIALISED);
		USB_SEND_TRACE("USB: state changed to USB_UNINITIALISED", RV_TRACE_LEVEL_DEBUG_LOW);
	}
#if (CHIPSET==15)
	if(bspUsblh_Otg_Init(NULL)!= TRUE)
	{
		ret = USB_INTERNAL_ERR;
		USB_SEND_TRACE("USB: Error to initialize OTG",RV_TRACE_LEVEL_ERROR);
		/*init failed, return to uninitialised state*/
		change_internal_state(USB_UNINITIALISED);
		USB_SEND_TRACE("USB: state changed to USB_UNINITIALISED", RV_TRACE_LEVEL_DEBUG_LOW);
	}

	if (bspUsblh_Otg_Register_Triton_Interrupt()!=USB_OK)
		{
		ret = USB_INTERNAL_ERR;
		USB_SEND_TRACE("USB: Error to configure interrupt handler",RV_TRACE_LEVEL_ERROR);
		/*init failed, return to uninitialised state*/
		change_internal_state(USB_UNINITIALISED);
		USB_SEND_TRACE("USB: state changed to USB_UNINITIALISED", RV_TRACE_LEVEL_DEBUG_LOW);
	}
#endif
	USB_SEND_TRACE("USB: usb init -> int handlr successfuly initialised", RV_TRACE_LEVEL_DEBUG_LOW);

	/**
	 *init physical endpoints
	 */
	if(init_physical_eps() != USB_OK)
	{
		ret = USB_INTERNAL_ERR;
		USB_SEND_TRACE("USB: Error to create physical endpoint table",RV_TRACE_LEVEL_ERROR);
		/*init failed, return to uninitialised state*/
		change_internal_state(USB_UNINITIALISED);
		USB_SEND_TRACE("USB: state changed to USB_UNINITIALISED", RV_TRACE_LEVEL_DEBUG_LOW);
	}
	init_dma_usb_tbl();
	USB_SEND_TRACE("USB: usb init -> physical endpoints initialised", RV_TRACE_LEVEL_DEBUG_LOW);
	/**
	 *create interface variables
	 */
	/*make sure previous step was succesfully executed*/
	if(usb_env_ctrl_blk_p->state == USB_INITIALISING)
	{	
		/* Create buffer to contain the interface specific data for every interface in 
		this configuration*/
		mb_status = rvf_get_buf(usb_env_ctrl_blk_p->prim_mb_id, 
									(sizeof(T_USB_INTERFACE_DATA) * USB_TOTAL_DIFF_IF),
									((T_RVF_BUFFER**) &usb_env_ctrl_blk_p->usb_if_tbl));
		if (mb_status == RVF_RED)
		{
			/*
			 * The flag returned by rvf_get_buf is red, there is not enough
			 * memory to allocate the buffer.
			 * The environemnt will cancel the USB instance creation.
			 */
			USB_SEND_TRACE("USB: Error to get memory ",RV_TRACE_LEVEL_ERROR);
			/*init failed, so return to uninitialised state*/
			change_internal_state(USB_UNINITIALISED);		
			USB_SEND_TRACE("USB: state changed to USB_UNINITIALISED", RV_TRACE_LEVEL_DEBUG_LOW);
			ret = RVM_MEMORY_ERR;
		}
		else if (mb_status == RVF_YELLOW)
		{
			/*
			 * The flag is yellow, there will soon be not enough memory anymore.
			 */
			USB_SEND_TRACE("USB: Getting short on memory ", RV_TRACE_LEVEL_WARNING);
		}
	}
	USB_SEND_TRACE("USB: usb init -> interface variables created", RV_TRACE_LEVEL_DEBUG_LOW);

	/**
	 *configure interface variables
	 */
	/*make sure previous step was succesfully executed*/
	if(usb_env_ctrl_blk_p->state == USB_INITIALISING)
	{
		if(conf_if_variables() != USB_OK)
		{
			/*hw init failed, so return to uninitialised state*/
			change_internal_state(USB_UNINITIALISED);
			USB_SEND_TRACE("USB: state changed to USB_UNINITIALISED", RV_TRACE_LEVEL_DEBUG_LOW);
			ret = USB_INTERNAL_ERR;
			USB_SEND_TRACE("USB: interface variable configuration failed", RV_TRACE_LEVEL_ERROR);
		}
	}
	conf_if_xrefs(1); //Temporary fixed value testing USB_CONF_DEFAULT);
	USB_SEND_TRACE("USB: usb init -> interface variables configured", RV_TRACE_LEVEL_DEBUG_LOW);

	/* make sure previous step was succesfully executed */
	if(usb_env_ctrl_blk_p->state == USB_INITIALISING)
	{
		/* Create buffer to contain setup request data*/
		mb_status = rvf_get_buf(usb_env_ctrl_blk_p->prim_mb_id, 
									(sizeof(T_USB_DEV_REQ)),
									((T_RVF_BUFFER**) &usb_env_ctrl_blk_p->host_data.ep0_setup.request_p));
		if (mb_status == RVF_RED)
		{
			/*
			 * The flag returned by rvf_get_buf is red, there is not enough
			 * memory to allocate the buffer.
			 * The environemnt will cancel the USB instance creation.
			 */
			USB_SEND_TRACE("USB: Error to get memory ",RV_TRACE_LEVEL_ERROR);
			ret = RVM_MEMORY_ERR;
		}
		else if (mb_status == RVF_YELLOW)
		{
			/*
			 * The flag is yellow, there will soon be not enough memory anymore.
			 */
			USB_SEND_TRACE("USB: Getting short on memory ", RV_TRACE_LEVEL_WARNING);
			USB_SEND_TRACE("USB: setup memory reserved", RV_TRACE_LEVEL_WARNING);
		}
		else
		{
			USB_SEND_TRACE("USB: setup memory reserved", RV_TRACE_LEVEL_WARNING);
		}
	}

	if(usb_env_ctrl_blk_p->state != USB_INITIALISING)
	{
		/*initialisation failed so delete created interrupt handler as well*/
		/* interrupt handler deletion*/
		if(disable_interrupt_hndlr() != USB_OK)
		{
			ret = USB_INTERNAL_ERR;
			USB_SEND_TRACE("USB: Error to delete interrupt handler",RV_TRACE_LEVEL_ERROR);
			/*init failed, so return to uninitialised state*/
			change_internal_state(USB_UNINITIALISED);
			USB_SEND_TRACE("USB: state changed to USB_UNINITIALISED", RV_TRACE_LEVEL_DEBUG_LOW);
		}
	}
	USB_SEND_TRACE("USB: usb init -> hw registers reset", RV_TRACE_LEVEL_DEBUG_LOW);

	/*the usb_bus is not yet connected*/
	usb_env_ctrl_blk_p->host_data.usb_connected = FALSE;	
	/*althought he driver is initialised it is not ready yet*/
	usb_env_ctrl_blk_p->driver_ready = FALSE;	
	
    for(dma_ctr = 0; dma_ctr < DMA_RX_CHANNELS; dma_ctr++)
	{
		strcpy(dma_rx_timer_name, "DmaRx");
		length = strlen(dma_rx_timer_name);

		dma_rx_timer_name[length]		= dma_ctr + '0';
		dma_rx_timer_name[length + 1]	= 0;

	    (void) NU_Create_Timer (
					   &(USB_DMA_CFG.dma_usb_rx_timer[dma_ctr]),
					   dma_rx_timer_name,
					   usb_timer_expiration,
					   dma_ctr, /* Parameter supplied to the routine: used to identify DMA channel */
					   WAKE_UP_TIME_IN_TDMA,
					   0, /* The timer expires once. */
					   NU_DISABLE_TIMER);
	}

	return ret;
}

extern void usb_init_dma_channel_variables();

/*
+------------------------------------------------------------------------------
| Function    : usb_start
+------------------------------------------------------------------------------
| Description : Called to start the USB SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usb_start(void)
{
	T_RVM_RETURN ret = RVM_OK;

	USB_SEND_TRACE("USB: start called", RV_TRACE_LEVEL_DEBUG_LOW);
	if(ret == RVM_OK)
	{
		change_internal_state(USB_IDLE); //swe has started
		USB_SEND_TRACE("USB: state changed to USB_IDLE", RV_TRACE_LEVEL_DEBUG_LOW);
		usb_env_ctrl_blk_p->driver_ready = TRUE;		/*driver is ready*/
	}

	usb_init_dma_channel_variables();
	
#ifndef _WINDOWS
	//enable int
	usb_int_clr();
#if (CHIPSET==15)
    bspUsblh_Otg_Enable_Triton_Interrupt();
#endif
#endif
	return ret;
}


/*
+------------------------------------------------------------------------------
| Function    : usb_stop
+------------------------------------------------------------------------------
| Description : Called to stop the USB SWE.
|
| Parameters  : Message
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usb_stop (T_RV_HDR * msg)
{
	/*
	 * Here we should stop the activities of the SWE
	 * It is still possible to send messages to other SWE, to unregister for example.
	 */
	USB_SEND_TRACE("USB: stop called", RV_TRACE_LEVEL_DEBUG_LOW);

	change_internal_state(USB_STOPPED);
	USB_SEND_TRACE("USB: state changed to USB_STOPPED", RV_TRACE_LEVEL_DEBUG_LOW);
	usb_env_ctrl_blk_p->driver_ready = FALSE;		/*driver is NOT ready*/

	return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : usb_kill
+------------------------------------------------------------------------------
| Description : Called to stop the USB SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usb_kill (void)
{
	UINT8 cntr = 0;
	T_RVM_RETURN ret = RVM_OK;
	/*
	 * Here we cannot send messages anymore. We only free the last
	 * used resources, like the control block buffer.
	 */
	USB_SEND_TRACE("USB: kill called", RV_TRACE_LEVEL_DEBUG_LOW);
	rvf_delete_mutex(&usb_env_ctrl_blk_p->mutex);
	rvf_delete_mutex(&usb_env_ctrl_blk_p->dma_mutex);

	/**
	 *free interface variables
	 */
	if (usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl) {
		rvf_free_buf(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl);
	}
	if (usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl) {
		rvf_free_buf(usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl);
	}
	if (usb_env_ctrl_blk_p->endpoint_rx_descr_tbl) {
		rvf_free_buf(usb_env_ctrl_blk_p->endpoint_rx_descr_tbl);
	}
	if (usb_env_ctrl_blk_p->endpoint_tx_descr_tbl) {
		rvf_free_buf(usb_env_ctrl_blk_p->endpoint_tx_descr_tbl);
	}

	for(cntr = 0; cntr < USB_TOTAL_DIFF_IF; cntr++)
	{
		/* free memory interface buffer*/
		rvf_free_buf(usb_env_ctrl_blk_p->usb_if_tbl);
	}
	rvf_free_buf(usb_env_ctrl_blk_p);
	
	/*kill allocated HISR*/
	/* interrupt handler deletion*/
#if (CHIPSET==15)
    bspUsblh_Otg_Disable_Triton_Interrupt();
#endif
	if(disable_interrupt_hndlr() != USB_OK)
	{
		ret = RVM_INTERNAL_ERR;
		USB_SEND_TRACE("USB: Error to delete interrupt handler",RV_TRACE_LEVEL_ERROR);
	}

	/*we delete the buffer here also, just to make sure, and deleting a NULL 
	buffer can't hurt anyway*/
	rvf_free_buf(usb_env_ctrl_blk_p->host_data.ep0_setup.request_p);
	rvf_free_buf(usb_env_ctrl_blk_p->host_data.ep0_data_p);
	
		change_internal_state(USB_KILLED);
	USB_SEND_TRACE("USB: state changed to USB_KILLED", RV_TRACE_LEVEL_DEBUG_LOW);

	usb_env_ctrl_blk_p->driver_ready = FALSE;		/*driver is NOT ready*/

	return (ret);
}

/*
+------------------------------------------------------------------------------
| Function    : change_internal_state
+------------------------------------------------------------------------------
| Description : change internal state
|
| Parameters  : state
|
| Return      : None
+------------------------------------------------------------------------------
*/
void change_internal_state(T_USB_INTERNAL_STATE state)
{
	usb_env_ctrl_blk_p->state = state;	
}

/*
+------------------------------------------------------------------------------
| Function    : conf_tot_descr_size
+------------------------------------------------------------------------------
| Description : Called to stop the USB SWE.
|
| Parameters  : Configuration size counter and configuration Number
|
| Return      : 
|               
+------------------------------------------------------------------------------
*/
#ifndef USB_DYNAMIC_CONFIG_SUPPORT
static
#endif
void conf_tot_descr_size(UINT16* cfg_size_cntr, UINT8 cfg_nr)
{
	UINT16	size = 0;
	UINT8	cntr;

	//first the size of the configuration descriptor
	size = USB_CNF_DESCR_LNT;
	
	// USB_TRACE_WARNING_PARAM("USB: after confg",size);

	if(configurations_table[cfg_nr].cfg_iad_descr_p != NULL)
	{
		size +=  USB_IAD_DESCR_LNT;
	}

	//size of all the interface descriptors
	size +=  configurations_table[cfg_nr].if_cfg_descr_p->bNumInterfaces *
		USB_IF_LNT;
			
	// USB_TRACE_WARNING_PARAM("USB: configurations_table[cfg_nr].if_cfg_descr_p->bNumInterfaces ",
	//	configurations_table[cfg_nr].if_cfg_descr_p->bNumInterfaces )
	
	// USB_TRACE_WARNING_PARAM("USB: after nr if's",size);

	//size of all endpoint descriptors active for this configuration
	for(cntr = 0; 
		cntr < configurations_table[cfg_nr].if_cfg_descr_p->bNumInterfaces; 
		cntr++)
	{
		if(configurations_table[cfg_nr].if_cnfg_p[cntr].if_cdc_descr_p != NULL)
		{
			size +=  USB_CDC_DESCR_LNT;
		}

		size += configurations_table[cfg_nr].if_cnfg_p[cntr].ifdescr_p->bNumEndpoints * 
			USB_EP_DESCR_LGNT;
	
	//	USB_TRACE_WARNING_PARAM("USB: size  after ep",size );
	}

	//assign to descriptor
	*cfg_size_cntr = size;
}

