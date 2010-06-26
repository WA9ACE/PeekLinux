/**
 * @file	usb_api.c
 *
 * API for USB SWE.
 *
 * @author	Anton van Breemen
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	26/09/2003	Anton van Breemen (ICT)		
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */
#include "chipset.cfg"
#include "usb/usb_i.h"
#if (CHIPSET!=15)
#include "abb/abb.h"
#endif
#include "usb/usb_api.h"
#include "usb/usb_inth.h"
#include "inth/sys_inth.h"
#include "bspI2c.h"
#include "bspUtil_BitUtil.h"
#include "string.h"
#include "bspTwl3029_I2c.h"
#include "bspTwl3029_UsbOtg.h"

/****************************************************************************************
 |	GLOBAL VARIABLES																	|
 V**************************************************************************************V*/

/** External ref "global variables" structure. */
extern T_USB_ENV_CTRL_BLK	*usb_env_ctrl_blk_p;
extern volatile UINT8 readyCount;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	BOOL callback_not_called;
#endif
BOOL usb_lld_delay=0;

/**
 *---------------------------------------------------------------------------------* 
 * @name usb_start_timer
 *
 * This function is used to start a timer for detecting escape sequences
 *
 * @param :	reference to the current device
 *
 * @return				RVM_OK
 *						RV_INTERNAL_ERR	: Incompatible message
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RV_RET usb_start_timer()
{
	T_USB_START_TIMER_MSG *msg_p;

	USB_SEND_TRACE("USB: usbfax_start_timer", RV_TRACE_LEVEL_DEBUG_LOW);	

	if(create_usb_msg(sizeof(T_USB_START_TIMER_MSG), USB_START_TIMER_MSG, \
		(T_RVF_MSG **)&msg_p) == USB_OK)
	{
		/*good the memory needed for the message is reserved*/
		/*set callback function to NULL*/
		msg_p->hdr.callback_func = NULL;
	}
	else
	{
		/*ERROR the memory needed for the buffer is not reserved*/
		USB_SEND_TRACE("USB API: Error to get memory for USB_START_TIMER_MSG ",RV_TRACE_LEVEL_ERROR);
		return RV_MEMORY_ERR;
	}
	USB_SEND_TRACE("USB API: usb_start_timer bridge function", RV_TRACE_LEVEL_DEBUG_LOW);

	/* Send the message using mailbox. */
	return rvf_send_msg(usb_env_ctrl_blk_p->addr_id,
						(void*)msg_p);
	
}

/**
 *---------------------------------------------------------------------------------* 
 * @name usb_stop_timer
 *
 * This function is used to start a timer for detecting escape sequences
 *
 * @param :	reference to the current device
 *
 * @return				RVM_OK
 *						RV_INTERNAL_ERR	: Incompatible message
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RV_RET usb_stop_timer()
{

	T_USB_STOP_TIMER_MSG *msg_p;

	USB_SEND_TRACE("USB: usbfax_stop_timer", RV_TRACE_LEVEL_DEBUG_LOW);	

	if(create_usb_msg(sizeof(T_USB_STOP_TIMER_MSG), USB_STOP_TIMER_MSG, \
		(T_RVF_MSG **)&msg_p) == USB_OK)
	{
		/*good the memory needed for the message is reserved*/
		/*set callback function to NULL*/
		msg_p->hdr.callback_func = NULL;
	}
	else
	{
		/*ERROR the memory needed for the buffer is not reserved*/
		USB_SEND_TRACE("USB API: Error to get memory for USB_STOP_TIMER_MSG ",RV_TRACE_LEVEL_ERROR);
		return RV_MEMORY_ERR;
	}
	USB_SEND_TRACE("USB API: usb_stop_timer bridge function", RV_TRACE_LEVEL_DEBUG_LOW);

	/* Send the message using mailbox. */
	return rvf_send_msg(usb_env_ctrl_blk_p->addr_id,
						(void*)msg_p);

}

/****************************************************************************************
 |	BRIDGE	FUNCTIONS																	|
 V**************************************************************************************V*/

 /**
 * @name usb_fm_subscribe
 *
 * this function is called by a FM to subscribe to an interface.
 *
 * Every interface is controlled by a function manager. In the "usb_interface_cfg.h"
 * is defined which FM controls which interface. This function is used
 * to actually subsribe the defined FM to the specified interface.
 * Theoratically the usb driver can see which FM is subscribing by reading the
 * "hdr". In the case that 1 FM supports more than 1 interface the 
 * FM must indicate during subscription, to which interface it will subscribe
 *
 * @param	UINT8:			interface_id	holds the interface number as described 
 *											in "usb_interface_cfg.h"
 * @return	T_RV_RET:		RV_OK, RV_NOT_READY, RV_INVALID_PARAMETER
 */
/*@{*/
T_RV_RET usb_fm_subscribe(UINT8 interface_id, T_RV_RETURN return_path)
{
	T_USB_FM_SUBSCRIBE_MSG * fm_subscribe_msg_p;

	if(create_usb_msg(sizeof(T_USB_FM_SUBSCRIBE_MSG), USB_FM_SUBSCRIBE_MSG, \
		(T_RVF_MSG **)&fm_subscribe_msg_p) == USB_OK)
	{
		/*good the memory needed for the message is reserved*/
		/*set callback function to NULL*/
		fm_subscribe_msg_p->hdr.callback_func = NULL;
		fm_subscribe_msg_p->interface_id = interface_id;
		fm_subscribe_msg_p->return_path = return_path;
		//fm_subscribe_msg_p->eplist_p = eplist_p;
	}
	else
	{
		/*ERROR the memory needed for the buffer is not reserved*/
		USB_SEND_TRACE("USB API: Error to get memory for USB_FM_SUBSCRIBE_MSG ",RV_TRACE_LEVEL_ERROR);
		return RV_MEMORY_ERR;
	}
	USB_SEND_TRACE("USB API: usb_send fm subscribe bridge function", RV_TRACE_LEVEL_DEBUG_LOW);

	/* Send the message using mailbox. */
	return rvf_send_msg(usb_env_ctrl_blk_p->addr_id,
						(void*)fm_subscribe_msg_p);
}
/*@}*/

/**
 * @name usb_fm_release
 *
 * this function must be called by a FM to release it subsrciption to an interface.
 *
 * Every interface is controlled by a function manager. In the "usb_interface_cfg.h"
 * is defined which FM controls which interface. This function is used
 * to release the subsription with the defined FM to the specified interface.
 *
 * @param	UINT8:			interface_id	holds the interface number as described 
 *											in "usb_interface_cfg.h"
 * @return	T_RV_RET:		RV_OK, RV_NOT_READY, RV_INVALID_PARAMETER
 */
/*@{*/								
T_RV_RET usb_fm_unsubscribe(UINT8 interface_id)
{
	T_USB_FM_UNSUBSCRIBE_MSG* fm_release_msg_p;

	if(create_usb_msg(sizeof(T_USB_FM_UNSUBSCRIBE_MSG), USB_FM_UNSUBSCRIBE_MSG, (T_RVF_MSG **)&fm_release_msg_p)\
		== USB_OK)
	{
		/*good the memory needed for the message is reserved*/
		/*set callback function to NULL*/
		fm_release_msg_p->hdr.callback_func = NULL;
		fm_release_msg_p->interface_id = interface_id;
	}
	else
	{
		/*ERROR the memory needed for the buffer is not reserved*/
		USB_SEND_TRACE("USB API: Error to get memory for T_USB_FM_RELEASE_MSG",RV_TRACE_LEVEL_ERROR);
		return RV_MEMORY_ERR;
	}
	USB_SEND_TRACE("USB API: usb_send fm release bridge function", RV_TRACE_LEVEL_DEBUG_LOW);

	/* Send the message using mailbox. */
	return rvf_send_msg(usb_env_ctrl_blk_p->addr_id,
						(void*)fm_release_msg_p);
}
/*@}*/



T_RV_RET usb_fm_ready(UINT8 interface_id, T_RV_RETURN return_path)
{
	T_USB_IF_READY* fm_ready_msg_p;

	if(create_usb_msg(sizeof(T_USB_IF_READY), USB_IF_READY, \
		(T_RVF_MSG **)&fm_ready_msg_p) == USB_OK)
	{
		/*good the memory needed for the message is reserved*/
		/*set callback function to NULL*/
		fm_ready_msg_p->hdr.callback_func = NULL;
		fm_ready_msg_p->interface_id = interface_id;
		fm_ready_msg_p->return_path = return_path;
		//fm_subscribe_msg_p->eplist_p = eplist_p;
	}
	else
	{
		/*ERROR the memory needed for the buffer is not reserved*/
		USB_SEND_TRACE("USB API: Error to get memory for USB_IF_READY_MSG ",RV_TRACE_LEVEL_ERROR);
		return RV_MEMORY_ERR;
	}
	USB_SEND_TRACE("USB API: usb_send fm ready bridge function", RV_TRACE_LEVEL_DEBUG_LOW);

	/* Send the message using mailbox. */
	return rvf_send_msg(usb_env_ctrl_blk_p->addr_id,
						(void*)fm_ready_msg_p);
}
/*@}*/




/**
 * @name usb_get_status
 *
 * this function must be called by a FM to get status info.
 *
 * The FM calls this function to retrieve status information about the endpoints
 * and the USB Hardware. The diver will respond to this function call by sending a 
 * T_RV_HDR type variable to the callback function that belongs with the requester FM 
 *
 * @param	T_USB_STATUS*:	status_p	pointer to status information storage space
 *									 	created by FM, filled by usb driver
 * @param	UINT8:			interface_id	holds the interface number as described 
 *											in "usb_interface_cfg.h"
 * @return	T_RV_RET:		RV_OK, RV_NOT_READY, RV_INVALID_PARAMETER
 */
/*@{*/
T_RV_RET usb_get_status(UINT8 interface_id, T_USB_STATUS* status_p)
{
	T_USB_GET_STATUS_MSG* get_status_msg_p;

	if(create_usb_msg(sizeof(T_USB_GET_STATUS_MSG), USB_GET_STATUS_MSG, \
		(T_RVF_MSG **)&get_status_msg_p) == USB_OK)
	{
		/*good the memory needed for the message is reserved*/
		/*set callback function to NULL*/
		get_status_msg_p->hdr.callback_func = NULL;
		get_status_msg_p->status_p = status_p;
		get_status_msg_p->interface_id = interface_id;
	}
	else
	{
		/*ERROR the memory needed for the buffer is not reserved*/
		USB_SEND_TRACE("USB API: Error to get memory for USB_GET_STATUS_MS",RV_TRACE_LEVEL_ERROR);
		return RV_MEMORY_ERR;
	}
	USB_SEND_TRACE("USB API: usb_send get_status_msg_p message", RV_TRACE_LEVEL_DEBUG_LOW);

	/* Send the message using mailbox. */
	return rvf_send_msg(usb_env_ctrl_blk_p->addr_id,
						(void*)get_status_msg_p);
}
/*@}*/

/**
 * @name usb_set_rx_buffer
 *
 * This function provides the usb driver with a buffer into which the recieved data can be
 * be placed.
 *
 * The driver places all the data packets as large as the available endpoints into this buffer
 * as one large data packet
 * 
 * THIS FUNCTION MUST ALWAYS BE CALLED AFTER THE INTERACE HAS BEEN NOTIFIED OF RECIEVED DATA
 * 
 * @param	UINT8:		interface_id	holds the interface number of the specified 
 *										endpoint. 
 * @param	UINT8:		endpoint		endpoint number 
 * @param   UINT8*:		buffer_p		pointer to the provided data buffer
 * @param   UINT16:		size			size of the provided buffer
 *
 * @return	T_RV_RET:	RV_OK, RV_NOT_READY, RV_INVALID_PARAMETER
 */
/*@{*/
T_RV_RET usb_set_rx_buffer(UINT8 interface_id, UINT8 endpoint, UINT8* buffer_p, UINT16 size)
{
	T_RV_RET	ret = RV_INVALID_PARAMETER;
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;

	T_USB_ACTIVE_CNFG *cnfg = &usb_env_ctrl_blk_p->cnfg;
	int i=interface_id;
	UINT8	phys_ep_idx;
	UINT8	pep_nr;

	USB_SEND_TRACE("USB: usb_read", RV_TRACE_LEVEL_ERROR);	
	USB_TRACE_WARNING_PARAM("if_nr : ",		interface_id);
	USB_TRACE_WARNING_PARAM("log_ep_nr : ", endpoint);
	USB_TRACE_WARNING_PARAM("size : ",		size);

	/* sanity check */
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	if (interface_id > USB_TOTAL_DIFF_IF) 
#else
	if (interface_id >= cnfg->nof_if) 
#endif
	{
		return RV_INTERNAL_ERR;
	}

	usb_lock();
	if((USB_CNFGNR != USB_CONF_DEFAULT) &&
		(USB_CNFGNR != USB_CONF_ADRESSED)) 
	{
		if(endpoint == 0)
		{
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
			for(i=0;i<USB_TOTAL_DIFF_IF;i++)
			{
				if(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_number == interface_id){
					break;
				}
			}		
#endif
			endpoint_vs_interface_entry_p = 
					&(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i]);
		}
		else
		{
			//find physical ep
			endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl + 
				(LEP_2_PEP(endpoint, interface_id, USB_RX));
		}

		//store data
		endpoint_vs_interface_entry_p->buf_p = buffer_p;
		endpoint_vs_interface_entry_p->buf_size = size;
		endpoint_vs_interface_entry_p->temp_buf_p = 
										endpoint_vs_interface_entry_p->buf_p;

		ret = RV_OK;
	}
	else 
	{
		USB_SEND_TRACE("USB: cannot set endpoint buffer for invalid configuration",
		RV_TRACE_LEVEL_ERROR);	
		ret = USB_INTERNAL_ERR;
	}
	if(ret == RV_OK) 
	{
		if(endpoint == 0)
		{
			configure_ep0_interface(interface_id);
		}
		else
		{
			if(buffer_p != NULL)
			{
				if(interface_id == USB_IF_CDC_SERIAL)
				{			
					phys_ep_idx = LEP_2_PEP(endpoint, interface_id, USB_RX);
					pep_nr = phys_ep_idx + 1;					
					/* Store info required to handle Missing ZLP Scenario */				
					usb_env_ctrl_blk_p->trackMissingZLP.pep_idx = phys_ep_idx;
					usb_env_ctrl_blk_p->trackMissingZLP.size =  size;			
				}		

				if(rmv_stall_cont_rx_trsfr(endpoint, interface_id) != USB_OK) 
				{
					ret = USB_INTERNAL_ERR;
					usb_env_ctrl_blk_p->trackMissingZLP.pep_idx = 255;
					usb_env_ctrl_blk_p->trackMissingZLP.size =  0;
				
					USB_SEND_TRACE("USB: rmv_stall_cont_rx_trsfr failed",
					RV_TRACE_LEVEL_ERROR);	
				}
			}
			else
			{
				phys_ep_idx = LEP_2_PEP(endpoint, interface_id, USB_RX);
				pep_nr = phys_ep_idx + 1;

				USB_SEND_TRACE("USB: POP stall endpoint", RV_TRACE_LEVEL_ERROR);

				USB_EP_NUM_SEL_RX_EP(pep_nr);
				//no valid buffer is provided yet
				stall_req_and_ep(pep_nr, USB_RX);
				//set endpoint stalled flag
				usb_env_ctrl_blk_p->bm_rx_ep_buf_stall |= (1 << pep_nr);
			}
		}
	}

	usb_unlock();
	return ret;
}
/*@}*/

/**
 * @name usb_reclaim_rx_buffer
 *
 * This function gives the FM back the control over the buffer.
 * The USB expects to get a new buffer. 
 * 
 * @param	UINT8:		interface_id	holds the interface number of the specified 
 *										endpoint.
 * @param	UINT8:		endpoint		endpoint number 
 *
 * @return	T_RV_RET:	RV_OK, RV_NOT_READY, RV_INVALID_PARAMETER
 */
/*@{*/
T_RV_RET usb_reclaim_rx_buffer(UINT8 interface_id, UINT8 endpoint)
{	
	T_RV_RET	ret = RV_INVALID_PARAMETER;
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;

	USB_SEND_TRACE("USB: usb_reclaim_rx_buffer", RV_TRACE_LEVEL_ERROR);	
	USB_TRACE_WARNING_PARAM("if_nr : ",		interface_id);
	USB_TRACE_WARNING_PARAM("log_ep_nr : ", endpoint);
	USB_TRACE_WARNING_PARAM("USB_CNFGNR : ", USB_CNFGNR);

	// for control endpoint
	 if( !(endpoint))
	 {
		USB_SEND_TRACE("USB: Reclaim Called for EP0",RV_TRACE_LEVEL_ERROR);	
		return USB_INTERNAL_ERR;
     }

	usb_lock();
	if((USB_CNFGNR != USB_CONF_DEFAULT) 
		&& (USB_CNFGNR != USB_CONF_ADRESSED)
		&& (usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl) ) 
	{

		T_USB_RX_BUFFER_FULL_MSG *rsp_msg_p;
		T_RV_RETURN_PATH rp;
		ret = RV_OK;

		USB_SEND_TRACE( "USB: Reclaim buffer condtion satisfied", RV_TRACE_LEVEL_DEBUG_HIGH);

		endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl + 
			(LEP_2_PEP(endpoint, interface_id, USB_RX));
		rp = endpoint_vs_interface_entry_p->interface_data_p->swe_return_path;
		if (RVF_RED == rvf_get_msg_buf(usb_env_ctrl_blk_p->prim_mb_id, 
				sizeof (T_USB_RX_BUFFER_FULL_MSG),
				USB_RX_BUFFER_FULL_MSG,
				((T_RVF_MSG**) &rsp_msg_p))) {
			USB_SEND_TRACE("USB: out of memory", RV_TRACE_LEVEL_ERROR);
			ret = RVM_MEMORY_ERR;
		}
		rsp_msg_p->endpoint = endpoint;
		rsp_msg_p->size = endpoint_vs_interface_entry_p->buf_size;
		rsp_msg_p->end_of_packet = FALSE;
		endpoint_vs_interface_entry_p->buf_p = NULL;
		endpoint_vs_interface_entry_p->temp_buf_p = NULL;
		/* follow return path of this interface */
		if (rp.callback_func) {
			rp.callback_func(rsp_msg_p);
			rvf_free_buf(rsp_msg_p);
		} else {
			if (RVF_OK != rvf_send_msg (rp.addr_id, rsp_msg_p)) {
				USB_SEND_TRACE("USB: could not buffer full msg", RV_TRACE_LEVEL_ERROR);
				ret = RVM_MEMORY_ERR;
			}
		}
	}
	else {
		USB_SEND_TRACE("USB: cannot release endpoint buffer for invalid configuration",
		RV_TRACE_LEVEL_ERROR);	
		ret = USB_INTERNAL_ERR;
	}
	usb_unlock();
	return ret;
}
/*@}*/

/**
 * @name usb_set_tx_buffer
 *
 * This function provides the usb driver with a buffer containing data to be send
 *
 * The driver splits the buffer in data packets as large as the available endpoints 
 * fifo
 * 
 * @param	UINT8:		interface_id	holds the interface number of the specified 
 *										endpoint.
 * @param	UINT8:		endpoint		endpoint number 
 * @param   UINT8*:		buffer_p		pointer to the provided data buffer
 * @param   UINT16:		size			size of the provided buffer
 *
 * @return	T_RV_RET:	RV_OK, RV_NOT_READY, RV_INVALID_PARAMETER
 */
/*@{*/

T_RV_RET usb_set_tx_buffer(UINT8 interface_id, UINT8 endpoint, UINT8* buffer_p, UINT16 size,
		BOOL shorter_transfer)
{
	T_RV_RET	ret = RV_INVALID_PARAMETER;
	T_USB_CROSS_REF *endpoint0_vs_interface_entry_p = NULL;
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p = NULL;	

	T_USB_ACTIVE_CNFG *cnfg = &usb_env_ctrl_blk_p->cnfg;
	int i=interface_id;

	usb_lock();
	USB_SEND_TRACE("USB: usb_write", RV_TRACE_LEVEL_ERROR);		
	USB_TRACE_WARNING_PARAM("if_nr : ",		interface_id);
	USB_TRACE_WARNING_PARAM("log_ep_nr : ", endpoint);
	USB_TRACE_WARNING_PARAM("size : ",		size);

	/* sanity check */
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	if (interface_id > USB_TOTAL_DIFF_IF) 
#else
	if (interface_id >= cnfg->nof_if) 
#endif
	{
		usb_unlock();
		return RV_INTERNAL_ERR;
	}

	if((USB_CNFGNR != USB_CONF_DEFAULT) &&
		(USB_CNFGNR != USB_CONF_ADRESSED)) 
	{
		if(endpoint == 0)
		{
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
			for(i=0;i<USB_TOTAL_DIFF_IF;i++)
			{
				if(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_number == interface_id){
					break;
				}
			}		
#endif
			endpoint0_vs_interface_entry_p = 
					&(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i]);
		}
		else
		{
			//find physical ep
			endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl + 
									(LEP_2_PEP(endpoint, interface_id, USB_TX));
		}

/*
Suthar: Check whether the buffer is empty. Before scheduling the new buffer for transfer.
            If the buffer is not empty return error.
*/
              if(endpoint0_vs_interface_entry_p == NULL)
              {
	              if((endpoint_vs_interface_entry_p->buf_p == NULL) && (endpoint_vs_interface_entry_p->temp_buf_p == NULL))
	              {
				//store data
				endpoint_vs_interface_entry_p->buf_p = buffer_p;
				endpoint_vs_interface_entry_p->buf_size = size;
				endpoint_vs_interface_entry_p->shorter_transfer = shorter_transfer;
				endpoint_vs_interface_entry_p->temp_buf_p = 
														endpoint_vs_interface_entry_p->buf_p;
				

				ret = RV_OK;
	              }
			else
			{
			    ret = RV_INTERNAL_ERR;
			}
              }
	       else
	       {
			//store data
			endpoint0_vs_interface_entry_p->buf_p = buffer_p;
			endpoint0_vs_interface_entry_p->buf_size = size;
			endpoint0_vs_interface_entry_p->shorter_transfer = shorter_transfer;
			endpoint0_vs_interface_entry_p->temp_buf_p = 
													endpoint0_vs_interface_entry_p->buf_p;
				

			ret = RV_OK;	       
	       }
	}
	else 
	{
		USB_SEND_TRACE("USB: cannot set endpoint buffer for invalid configuration",
		RV_TRACE_LEVEL_ERROR);	
		ret = USB_INTERNAL_ERR;
	}

	if(ret == RV_OK) 
	{
		if(endpoint == 0)
		{
			ret = prepare_first_ep0_trsfr(interface_id);
		}
		else
		{
			ret = prepare_first_tx_trsfr(endpoint, interface_id);
		}
	}
	USB_SEND_TRACE("USB: leaving usb_write", RV_TRACE_LEVEL_DEBUG_LOW);
	usb_unlock();
	return ret;
}

/*@}*/

/****************************************************************************************
 |	API FUNCTIONS																		|
 V**************************************************************************************V*/

/**
 * @name usb_get_hw_version
 *
 * this function must be called to get the USB hardware version
 *
 * This function is called this to retrieve hardware version information of the USB 
 * hardware.
 *
 * @return	UINT8:		b0-3 minor version number (4bits)
 *						b4-7 major version number (4bits)	
 */
/*@{*/
UINT8 usb_get_hw_version(void)
{
	UINT16 rev = W2FC_REV;
	return (UINT8)rev;
}
/*@}*/

/**
 * @name usb_get_sw_version
 *
 * this function must be called to get the USB software driver version
 *
 * This function is called this to retrieve software version information of the USB 
 * driver.
 *
 * @return	UINT32:		b0-15  build number			(8bits)
 *						b16-23 minor version number (8bits)
 *						b24-32 major version number (8bits)	
 */
/*@{*/
UINT32 usb_get_sw_version(void)
{
	return BUILD_VERSION_NUMBER(USB_MAJOR_SW_VERSION_NR,
								USB_MINOR_SW_VERSION_NR,
								USB_BUILD_NR);
}
/*@}*/

/**
 * @name usb_register_notifcation
 *
 * This function must be called by a Application to subscribe for USB event notification.
 *
 *
 * @param	T_USB_EVENT_CALLBACK: callback	the callback function pointer. This function 
 *										    will be called for notifying USB events. 
 * @param	void:				  ctx		This is the context information provided with callback.
 *											This will be provided transparently back to the caller.
 */
/*@{*/
T_USB_RETURN usb_register_notifcation(T_USB_EVENT_CALLBACK callback, void *ctx)
{
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	if(usb_env_ctrl_blk_p->ntfcCallback != NULL) {
		return USB_INTERNAL_ERR;
	}
	usb_env_ctrl_blk_p->ntfcCallback = callback;
	usb_env_ctrl_blk_p->ntfcCtx = ctx;

	if(readyCount) {
		/* CHECK USB State if Cable Attached */
		if(usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem & W2FC_DEVSTAT_ATT){
			(*(usb_env_ctrl_blk_p->ntfcCallback))(USB_CABLE_CONNECT_EVENT,usb_env_ctrl_blk_p->ntfcCtx);
		}
	}
#endif
	return USB_OK;
}
/*@}*/

/**
 * @name usb_enum_control
 *
 * This function must be called by a Application to subscribe for USB event notification.
 * Implemented to support dynamic configuration
 *
 * @param	T_USB_ENUM_CONTROL:   enumCtrl	This variable controls the progress of enumeration. 
 *
 * @param	T_USB_ENUM_INFO*: 	  enumInfo	This provides the information of which interface to 
 *                                          enumerate. NULL means enumerate default.
 */
/*@{*/

T_USB_RETURN usb_enum_control(T_USB_ENUM_CONTROL enumCtrl, T_USB_ENUM_INFO * enumInfo)
{
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	UINT8 info = USB_MS| USB_AT|USB_TRACE;
	UINT16 cfg_size_cntr;
	UINT8 cfg_ctr=0;
	UINT8 i=0;

	if((enumInfo == ( T_USB_ENUM_INFO *)NULL )&& (enumCtrl == USB_ENUM))
		return RV_INVALID_PARAMETER;
	
	usb_env_ctrl_blk_p->enumInfo.info = enumInfo->info;
	
	if( !((enumCtrl ==  USB_NO_ENUM) ||(enumCtrl == USB_ENUM)))
	{
		return USB_INTERNAL_ERR;
	}	

	/* Clear Pull Always as it would be used in Dynamic Configuration */
	if(usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem & W2FC_DEVSTAT_ATT)
	{
		bspTwl3029_UsbOtg_PullControl(BSP_TWL3029_USB_CLEAR_PULL);	
	}

	if(enumCtrl == USB_NO_ENUM) {
		return USB_OK;
	}

	while(usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem & W2FC_DEVSTAT_ATT);
	
	/* Unsubscribe any subscribed DMA channels */
	for(i=0;i<USB_TOTAL_DIFF_IF;i++)
	{
		usb_env_ctrl_blk_p->usb_if_tbl[i].interface_enabled = IF_DISABLED;
		unsubscribe_dma_usb_interface(i);
	}

	/* ToDo Implement Switch Configuration Functionality */
	/* For Now Just Enable Pull-UP on DP That is Simulate Connect */
	/* Wait while USB Initialize Properly */
	info = enumInfo->info;	
	create_configuration_descriptor(info);

	conf_tot_descr_size(&cfg_size_cntr, cfg_ctr);
	usb_conf_descr_table[cfg_ctr].wTotalLength = cfg_size_cntr;

	callback_not_called = TRUE;

	/*
		Following is not required since this is being called
		either from set_configuration/force_to_default configuration
	*/
	/* conf_if_xrefs(1); */
	if(!(usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem & W2FC_DEVSTAT_ATT)){
		usb_con_int();
	}
	bspTwl3029_UsbOtg_PullControl(BSP_TWL3029_USB_SET_PULL);
#endif	
	return USB_OK;
}
/*@}*/

/**
 * @name usb_con_int
 *
 * This function is called by the ABB external interrupt handler.
 *
 * This function is called to initialize the USB transceiver (TRITON - TWL3029).
 * In functional terms, it is responsible for bringing up the USB Connectivity.
 *
 * @return	void
 *
 */
/*@{*/
void usb_con_int()
{
	/*reset  registers*/
#ifndef _WINDOWS
	reset_registers();
	W2FC_SYSCON1 &=(~W2FC_SYSCON1_SOFF_DIS);
	W2FC_SYSCON1 |= W2FC_SYSCON1_PULLUP_EN;
	
	USB_CONF_CORE |= USB_VBUS_MODE | USB_VBUS_CTRL_CON;
	
#endif
}
void usb_discon_int()
{
	//set USB_VBUS_CTRL & USB_VBUS_MODE
	USB_CONF_CORE &= USB_VBUS_CTRL_DISCON;
	W2FC_SYSCON1 = W2FC_16BIT_RESET;
}

#if (CHIPSET==15)
void usb_otg_attach_hndlr(INT8 srcid)
{
	usb_con_int();	
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	if(callback_not_called == TRUE)
	{
		callback_not_called = FALSE;
		if(usb_env_ctrl_blk_p->ntfcCallback) {
			(*(usb_env_ctrl_blk_p->ntfcCallback))(USB_ENUM_FAILURE_EVENT,usb_env_ctrl_blk_p->ntfcCtx);
		}			
	}
	if(readyCount)
	{
		if(usb_env_ctrl_blk_p->ntfcCallback) {
			(*(usb_env_ctrl_blk_p->ntfcCallback))(USB_CABLE_CONNECT_EVENT,usb_env_ctrl_blk_p->ntfcCtx);
			usb_lld_delay=0;// fix for issue OMAPS00156766
		}
	}
#else
	if(readyCount)
	{
		bspTwl3029_UsbOtg_PullControl(BSP_TWL3029_USB_SET_PULL);
	}
#endif
}

void usb_otg_detach_hndlr(INT8 srcid)
{
/*reset  registers*/
#ifdef USB_DYNAMIC_CONFIG_SUPPORT

	if(usb_env_ctrl_blk_p->ntfcCallback) {
		(*(usb_env_ctrl_blk_p->ntfcCallback))(USB_CABLE_DISCONNECT_EVENT,usb_env_ctrl_blk_p->ntfcCtx);
	}
#endif
	bspTwl3029_UsbOtg_PullControl(BSP_TWL3029_USB_CLEAR_PULL);
}
#endif

/****************************************************************************************
 |	INTERNAL FUNCTIONS																	|
 V**************************************************************************************V*/

/**
 * @name create_usb_msg
 *
 * this function is called to create a message variable for USB
 *
 * This function is called to create a message variable of a T_RVF_MSG type
 * that can be used an casted to USB specific messages
 * 
 * @param	UINT32			message_size	size of the message type in bytes
 * @param	T_RVF_MSG_ID	msg_id			message specific id
 * @param	UT_RVF_MSG **	pp_msg			pointer to allocated memory that
 *											will hold the message data.
 * @return	T_RV_RET:	RV_OK, RV_NOT_READY, RV_INVALID_PARAMETER
 */
/*@{*/
T_RV_RET create_usb_msg(UINT32 message_size, T_RVF_MSG_ID msg_id, T_RVF_MSG ** pp_msg)
{
	/* Memory bank status (red, yellow, green). */
	T_RVF_MB_STATUS mb_status;

	/* Get a message buffer. */
	mb_status = rvf_get_msg_buf(usb_env_ctrl_blk_p->prim_mb_id, message_size, msg_id, pp_msg);
	if (mb_status == RVF_RED)
	{
		/*
		 * The flag returned by rvf_get_buf is red, there is not enough
		 * memory to allocate the buffer.
		 */
		USB_SEND_TRACE("USB API: Error to get memory ",RV_TRACE_LEVEL_ERROR);
				
		return USB_MEMORY_ERR;
	}
	else if (mb_status == RVF_YELLOW)
	{
		/*
		 * The flag is yellow, there will soon be not enough memory anymore.
		 */
		USB_SEND_TRACE("USB API: Getting short on memory ", RV_TRACE_LEVEL_WARNING);
	}
	return USB_OK;
}
/*@}*/
