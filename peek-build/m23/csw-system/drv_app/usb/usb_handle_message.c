/**
 * @file	usb_handle_message.c
 *
 * USB handle_msg function, which is called when the SWE
 * receives a new message in its mailbox.
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

#include "usb/usb_i.h"
#include "usb/usb_env.h"
#include "rvf/rvf_api.h"
#include "usb/usb_inth.h"
#include "chipset.cfg"

#if (CHIPSET==15)
#include "dma/dma_message.h"
#else
#include "dmg/dmg_message.h"
#endif
#include "usb/usb_dma.h"
#include "bspI2c.h"
#include "bspUtil_BitUtil.h"
#include "string.h"
#include "bspTwl3029_I2c.h"
#include "bspTwl3029_UsbOtg.h"
/****************************************************************************************
 |	MACRO'S																				|
 V**************************************************************************************V*/

#define FIRST_FM	1
#define USB_INTERFACE_ACTIVE(a)	(usb_env_ctrl_blk_p->usb_if_tbl[a].interface_enabled == IF_ENABLED ? TRUE : FALSE)
#define USB_FM_ADDR(a)	usb_if_nr_of_ep_tbl[a].interface_return_path.addr_id
#define USB_FM_ACTIVE(a, b)	((fm_active_cntr[a].addr_id == b) ? TRUE : FALSE)
#define USB_IF_CLASS_ID(a)	if_descr_table[a].bInterfaceClass
#define USB_IF_SUB_CLASS_ID(a)	if_descr_table[a].bInterfaceSubClass

 /****************************************************************************************
 |	FUNCTION PROTOTYPES																	|
 V**************************************************************************************V*/

static T_RV_RET process_msg(T_RV_HDR *msg_p);
static T_RV_RET subscribe_fm_to_if(T_USB_FM_SUBSCRIBE_MSG* msg_p);
static T_RV_RET release_fm_from_if(T_USB_FM_UNSUBSCRIBE_MSG* msg_p);
static T_RV_RET if_ready(T_USB_IF_READY* msg_p);
static T_RV_RET get_status(T_USB_GET_STATUS_MSG* msg_p);
//static T_RV_RET tx_buffer_full(T_USB_TX_BUFFER_FULL_MSG* msg_p);
static T_RV_RET send_usb_status_ok(void);
static T_RV_RET set_tx_buffer(T_USB_SET_TX_BUFFER_MSG *msg_p);
static T_RV_RET set_rx_buffer(T_USB_SET_RX_BUFFER_MSG *msg_p);
static T_RV_RET reclaim_rx_buffer(T_USB_RECLAIM_RX_BUFFER_MSG *msg_p);
static int gUsbIfCount = 0;
extern BOOL usb_lld_delay;

/****************************************************************************************
 |	EXTERNAL FUNCTIONS																	|
 V**************************************************************************************V*/

/**
 * Called every time the SW entity is in WAITING state 
 * and get a new message in its mailbox.
 *
 * The message in parameter is freed in this function.
 *
 * @param	msg_p	Pointer on the header of the message.
 * @return	RVM_OK or RVM_MEMORY_ERR.
 */
T_RVM_RETURN usb_handle_message (T_RV_HDR *msg_p)
{
	T_RVM_RETURN ret = RVM_OK;

	USB_SEND_TRACE("USB: usb_handle_message called",RV_TRACE_LEVEL_DEBUG_LOW);

	if (msg_p != NULL)
	{
		switch (usb_env_ctrl_blk_p->state)
		{
		case USB_UNINITIALISED:
			/*we cannot process any message while we're USB_UNINITIALISED*/
		  	USB_SEND_TRACE("USB: Got an unexpected event in this (USB_UNINITIALISED) state - Ignored",
								RV_TRACE_LEVEL_WARNING);
			break;
		case USB_INITIALISING:
			/*we cannot process any message while we're USB_INITIALISING*/
		  	USB_SEND_TRACE("USB: Got an unexpected event in this (USB_INITIALISING) state - Ignored",
								RV_TRACE_LEVEL_WARNING);
			break;
		case USB_IDLE:
			change_internal_state(USB_HANDLE_REQUEST);
			if(process_msg(msg_p) != USB_OK)
			{
				USB_SEND_TRACE("USB: Unable to process message", RV_TRACE_LEVEL_ERROR);
				ret = RVM_MEMORY_ERR;
			}
			break;
		case USB_STOPPED:
			/*we cannot process any message while state is USB_STOPPED*/
		  	USB_SEND_TRACE("USB: Got an unexpected event in this (USB_STOPPED) state - Ignored", 
								RV_TRACE_LEVEL_WARNING);
			break;
		case USB_KILLED:
			/*we cannot process any message while state is USB_KILLED*/
		  	USB_SEND_TRACE("USB: Got an unexpected event in this (USB_KILLED) state - Ignored", 
								RV_TRACE_LEVEL_WARNING);
			break;
		case USB_HANDLE_INTERRUPT:
			/*we cannot process any message while state is USB_HANDLE_INTERRUPT*/
		  	USB_SEND_TRACE("USB: Got an unexpected event in this (USB_HANDLE_INTERRUPT) state - Ignored", 
								RV_TRACE_LEVEL_WARNING);
			break;
		case USB_HANDLE_REQUEST:
			/**For the moment we cannot process any message while state is 
			 *USB_HANDLE_REQUEST in the future it could be possible that a
			 *situation exists that a message should be processed while in this state
			 */
		  	USB_SEND_TRACE("USB: Got an unexpected event in this (USB_HANDLE_REQUEST) state - Ignored", 
							RV_TRACE_LEVEL_WARNING);
			break;
		default: 
			/* Unknow message has been received */
			USB_SEND_TRACE("USB: USB in an unknown state",
							RV_TRACE_LEVEL_ERROR);
			break; 
		}		
		/* Free message */
		if (rvf_free_buf(msg_p) != RVF_OK)
		{
			USB_SEND_TRACE("USB: Unable to free message",
							RV_TRACE_LEVEL_ERROR);
			ret = RVM_MEMORY_ERR;
		}
		/*set driver state back to idle*/
		change_internal_state(USB_IDLE);
	}
	return ret;
}


/****************************************************************************************
 |	INTERNAL	FUNCTIONS																|
 V**************************************************************************************V*/
volatile UINT8 readyCount = 0;

UINT8 timer_started;

static T_RV_RET process_msg(T_RV_HDR *msg_p)
{
	T_RV_RET ret = USB_OK;

	/*find out what message is recieved*/
	switch (msg_p->msg_id)
	{
	case USB_FM_SUBSCRIBE_MSG:

		if(subscribe_fm_to_if((T_USB_FM_SUBSCRIBE_MSG*)msg_p) != USB_OK)
		{
			/*the subscription seems to have failed*/
			USB_SEND_TRACE("USB: Unable to subscribe FM to interface", RV_TRACE_LEVEL_ERROR);
			ret = USB_INTERNAL_ERR;
		}
		break;
	case USB_FM_UNSUBSCRIBE_MSG:
		if(release_fm_from_if((T_USB_FM_UNSUBSCRIBE_MSG*)msg_p) != USB_OK)
		{
			/*the release process did not succeed*/
			USB_SEND_TRACE("USB: Unable to release FM from interface", RV_TRACE_LEVEL_ERROR);
			ret = USB_INTERNAL_ERR;
		}
		break;
	case USB_IF_READY:
		
		if(if_ready((T_USB_IF_READY*)msg_p) != USB_OK)
		{
			/*the release process did not succeed*/
			USB_SEND_TRACE("USB: Unable to respond", RV_TRACE_LEVEL_ERROR);
			ret = USB_INTERNAL_ERR;
		}
		if( (USB_TOTAL_DIFF_IF == gUsbIfCount) &&(readyCount == 0))
		{
			readyCount=1;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
			if(usb_env_ctrl_blk_p->ntfcCallback) {
/* fix for OMAPS00156766
    when the systme is fully loaded, SIM card is full with SMSs and phone book entries,  if we plug in the USB cable at 
    power on then no port is accessible at the host side after enumeration.
    The root cause is, SIM task is at high priority then GBI. GBI task is starving and lead to timeout in host communication 
    with mass storage and host keep on sending reset to the USB LLD and no ports are accessible.
    So we have delayed the enumeration till the SIM task finishes its job.
    usb_lld_delay flag is used to synchronize with the usb_otg_attach_hndlr.
    Note: This fix should not take directly to the other line because its dependent on the boot time of the system and has to be 
    tuned for that.
    */
				usb_lld_delay=1;
				rvf_delay(RVF_SECS_TO_TICKS(15));				
				/* CHECK USB State if Cable Attached */
				if(usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem & W2FC_DEVSTAT_ATT){

					if(usb_lld_delay)
					(*(usb_env_ctrl_blk_p->ntfcCallback))(USB_CABLE_CONNECT_EVENT,usb_env_ctrl_blk_p->ntfcCtx);
				}


			}
#else
			bspTwl3029_UsbOtg_PullControl(BSP_TWL3029_USB_SET_PULL);
#endif
    	}

		break;		
		
	case USB_GET_STATUS_MSG:
		if(get_status((T_USB_GET_STATUS_MSG*)msg_p) != USB_OK)
		{
			/*the get status process did not succeed*/
			USB_SEND_TRACE("USB: Unable to get USB status", RV_TRACE_LEVEL_ERROR);
			ret = USB_INTERNAL_ERR;
		}
		break;
	case USB_SET_TX_BUFFER_MSG:
		if(set_tx_buffer((T_USB_SET_TX_BUFFER_MSG*)msg_p) != USB_OK)
		{
			/*the release process did not succeed*/
			USB_SEND_TRACE("USB: Unable to set TX buffer", RV_TRACE_LEVEL_ERROR);
			ret = USB_INTERNAL_ERR;
		}
		break;
	case USB_SET_RX_BUFFER_MSG:
		if(set_rx_buffer((T_USB_SET_RX_BUFFER_MSG*)msg_p) != USB_OK)
		{
			/*the release process did not succeed*/
			USB_SEND_TRACE("USB: Unable to set RX buffer", RV_TRACE_LEVEL_ERROR);
			ret = USB_INTERNAL_ERR;
		}
		break;
	case USB_RECLAIM_RX_BUFFER_MSG:
		if(reclaim_rx_buffer((T_USB_RECLAIM_RX_BUFFER_MSG*)msg_p) != USB_OK)
		{
			/*the release process did not succeed*/
			USB_SEND_TRACE("USB: Unable to reclaim RX buffer", RV_TRACE_LEVEL_ERROR);
			ret = USB_INTERNAL_ERR;
		}
		break;
	case USB_START_TIMER_MSG:

		rvf_start_timer(RVF_TIMER_0,
					RVF_MS_TO_TICKS(10),
					FALSE);
		timer_started = 0x01;
		break;
	case USB_STOP_TIMER_MSG:

		rvf_stop_timer(RVF_TIMER_0);
		timer_started = 0;
		break;		
	case USB_DMA_TX_EP_INTERRUPT_MSG:
		{
			UINT8	phys_ep_nr	= ((T_USB_TX_EP_INTERRUPT_MSG *)msg_p)->endpoint;

			usb_deffered_non_iso_tx_dma_done_handler(phys_ep_nr);
		}
		break;
#if (CHIPSET==15)
	case DMA_STATUS_RSP_MSG:
#else
	case DMG_STATUS_RSP_MSG:
#endif
			{
#if (CHIPSET==15)
			T_DMA_STATUS_RSP_MSG*	dma_msg_p = (T_DMA_STATUS_RSP_MSG *)msg_p;
#else
			T_DMG_STATUS_RSP_MSG*	dmg_msg_p = (T_DMG_STATUS_RSP_MSG *)msg_p;
#endif

#if (CHIPSET==15)
			switch(dma_msg_p->result.status)
			{
				case DMA_RESERVE_OK:
					USB_TRACE_WARNING_PARAM("USB DMA: DMA_RESERVE_OK : channel : ", dma_msg_p->result.channel);

					usb_process_dma_request(dma_msg_p->result.channel);
					usb_dma_unlock();
				break;

				case DMA_PARAM_SET:
					USB_TRACE_WARNING_PARAM("USB DMA: DMA_PARAM_SET : channel : ", dma_msg_p->result.channel);
				break;

				case DMA_COMPLETED:
					USB_TRACE_WARNING_PARAM("USB_handle_message:: DMA_COMPLETED : channel : ", dma_msg_p->result.channel);
				break;

      			case DMA_NO_CHANNEL  :
      				USB_TRACE_WARNING_PARAM("USB_handle_message:: DMA_NO_CHANNEL : channel : ", dma_msg_p->result.channel);
#else
			switch(dmg_msg_p->result.status)
			{
				case DMG_RESERVE_OK:
					USB_TRACE_WARNING_PARAM("USB DMA: DMG_RESERVE_OK : channel : ", dmg_msg_p->result.channel);

					usb_process_dma_request(dmg_msg_p->result.channel);
					usb_dma_unlock();
				break;

				case DMG_PARAM_SET:
					USB_TRACE_WARNING_PARAM("USB DMA: DMG_PARAM_SET : channel : ", dmg_msg_p->result.channel);
				break;

				case DMG_COMPLETED:
					USB_TRACE_WARNING_PARAM("USB_handle_message:: DMG_COMPLETED : channel : ", dmg_msg_p->result.channel);
				break;

      			case DMG_NO_CHANNEL  :
      				USB_TRACE_WARNING_PARAM("USB_handle_message:: DMG_NO_CHANNEL : channel : ", dmg_msg_p->result.channel);
#endif

					usb_process_dma_request_in_standard_mode();
					usb_dma_unlock();
      			break;
#if (CHIPSET==15)
				case DMA_CHANNEL_RELEASED :
      				USB_TRACE_WARNING_PARAM("USB_handle_message:: DMA_CHANNEL_RELEASED : channel : ", dma_msg_p->result.channel);
#else
				case DMG_CHANNEL_RELEASED :
      				USB_TRACE_WARNING_PARAM("USB_handle_message:: DMG_CHANNEL_RELEASED : channel : ", dmg_msg_p->result.channel);
#endif
      			break;

				default:
					USB_SEND_TRACE			("USB DMA: Unknown DMG message", RV_TRACE_LEVEL_ERROR);
#if (CHIPSET==15)
					USB_TRACE_WARNING_PARAM	("USB DMA: Message id : ", dma_msg_p->result.status);
#else
					USB_TRACE_WARNING_PARAM	("USB DMA: Message id : ", dmg_msg_p->result.status);
#endif
				break;
			}
		}
		break;
	default :
		/*unexpected msg*/
		USB_SEND_TRACE("USB: Unable to process UNKNOWN message", RV_TRACE_LEVEL_ERROR);
		ret = USB_INTERNAL_ERR;
		break;
	}
		
	return ret;
}

static T_RV_RET subscribe_fm_to_if(T_USB_FM_SUBSCRIBE_MSG* msg_p)
{
	T_RV_RET	ret = RV_INVALID_PARAMETER;
	T_USB_ACTIVE_CNFG *cnfg = &usb_env_ctrl_blk_p->cnfg;
	UINT32 if_idx;
	T_USB_FM_RESULT_MSG *rsp_msg_p;

	/* get response msg */
	if (RVF_RED == rvf_get_msg_buf(usb_env_ctrl_blk_p->prim_mb_id, 
				sizeof (T_USB_FM_RESULT_MSG),
				USB_FM_RESULT_MSG,
				((T_RVF_MSG**) &rsp_msg_p))) {
		USB_SEND_TRACE("USB: out of memory", RV_TRACE_LEVEL_ERROR);
		return RVM_MEMORY_ERR;
	}
	rsp_msg_p->result = fail_subscribe;

	/* find matching interface & subscribe */
	usb_lock();
	for (if_idx = 0; if_idx < USB_TOTAL_DIFF_IF; if_idx++) {
		T_USB_INTERFACE_DATA *usb_if_tbl_entry = usb_env_ctrl_blk_p->usb_if_tbl + if_idx;
		if (usb_if_tbl_entry->interface_number == msg_p->interface_id) {
#ifndef USB_DYNAMIC_CONFIG_SUPPORT
			usb_if_tbl_entry->interface_enabled = IF_ENABLED;
#endif
			usb_if_tbl_entry->swe_return_path = msg_p->return_path;
			rsp_msg_p->result = succes;
			ret = RV_OK;
			break;
		}
	} /* END if_idx loop */
#ifdef  USB_DMA_USED
#ifndef USB_DYNAMIC_CONFIG_SUPPORT
	subscribe_dma_usb_interface(msg_p->interface_id);
#endif
#endif
	usb_unlock();				

	/* follow return path of this interface */
	if (msg_p->return_path.callback_func) {
		msg_p->return_path.callback_func(rsp_msg_p);
		rvf_free_buf(rsp_msg_p);
	} else {
		if (RVF_OK != rvf_send_msg (msg_p->return_path.addr_id, rsp_msg_p)) {
			USB_SEND_TRACE("USB: could send subscribe response", RV_TRACE_LEVEL_ERROR);
			ret = RVM_MEMORY_ERR;
		}
	}
	/* TODO: HW enable all associated endpoints? */
	return ret;
}

static T_RV_RET release_fm_from_if(T_USB_FM_UNSUBSCRIBE_MSG* msg_p)
{	
	T_RV_RET	ret = RV_INVALID_PARAMETER;
	T_USB_ACTIVE_CNFG *cnfg = &usb_env_ctrl_blk_p->cnfg;
	UINT32 if_idx;
	T_USB_FM_RESULT_MSG *rsp_msg_p;
	T_RV_RETURN_PATH rp = {RVF_INVALID_ADDR_ID, NULL};

	/* get response msg */
	if (RVF_RED == rvf_get_msg_buf(usb_env_ctrl_blk_p->prim_mb_id, 
				sizeof (T_USB_FM_RESULT_MSG),
				USB_FM_RESULT_MSG,
				((T_RVF_MSG**) &rsp_msg_p))) {
		USB_SEND_TRACE("USB: out of memory", RV_TRACE_LEVEL_ERROR);
		return RVM_MEMORY_ERR;
	}
	rsp_msg_p->result = fail_unsubscribe;

	/* find matching interface & subscribe */
	usb_lock();
	for (if_idx = 0; if_idx < USB_TOTAL_DIFF_IF; if_idx++) {
		T_USB_INTERFACE_DATA *usb_if_tbl_entry = usb_env_ctrl_blk_p->usb_if_tbl + if_idx;
		if (usb_if_tbl_entry->interface_number == msg_p->interface_id) {
			if (usb_if_tbl_entry->interface_enabled == IF_ENABLED) {
				usb_if_tbl_entry->interface_enabled = IF_DISABLED;
				rp = usb_if_tbl_entry->swe_return_path;
				rsp_msg_p->result = succes;
				ret = RV_OK;
			} else {
				rsp_msg_p->result = not_subscribed;
				ret = RV_INVALID_PARAMETER;
			}
			break;
		}
	} /* END if_idx loop */
#ifdef  USB_DMA_USED
	unsubscribe_dma_usb_interface(msg_p->interface_id);
#endif
	usb_unlock();

	if (rsp_msg_p->result == not_subscribed) {
		/* clean up unused response msg */
		rvf_free_buf(rsp_msg_p);
	} else if (rp.callback_func) {
		/* follow return path of this interface */
		rp.callback_func(rsp_msg_p);
		rvf_free_buf(rsp_msg_p);
	} else if (rp.addr_id != RVF_INVALID_ADDR_ID && RVF_OK != rvf_send_msg (rp.addr_id, rsp_msg_p)) {
			USB_SEND_TRACE("USB: could send unsubscribe response", RV_TRACE_LEVEL_ERROR);
			ret = RVM_MEMORY_ERR;
	}
	/* TODO: HW disable all associated endpoints? */
	return ret;
}


static T_RV_RET if_ready(T_USB_IF_READY* msg_p)
{
	T_RV_RET	ret = RV_OK;
	T_USB_ACTIVE_CNFG *cnfg = &usb_env_ctrl_blk_p->cnfg;
	UINT32 if_idx;
	T_USB_FM_RESULT_MSG *rsp_msg_p;
	static UINT8 if_ready[USB_TOTAL_DIFF_IF] = {0};

	/* get response msg */
	if (RVF_RED == rvf_get_msg_buf(usb_env_ctrl_blk_p->prim_mb_id, 
				sizeof (T_USB_FM_RESULT_MSG),
				USB_FM_RESULT_MSG,
				((T_RVF_MSG**) &rsp_msg_p))) {
		USB_SEND_TRACE("USB: out of memory", RV_TRACE_LEVEL_ERROR);
		return RVM_MEMORY_ERR;
	}
	rsp_msg_p->result = succes;

	/* find matching interface & subscribe */
	usb_lock();
	for (if_idx = 0; if_idx < USB_TOTAL_DIFF_IF; if_idx++) 
		{
			if(if_idx == msg_p->interface_id && if_ready[if_idx] == 0)
			{
				if_ready[if_idx] = 1;
				gUsbIfCount++;
				ret = RV_OK;
				break;
			}
		} /* END if_idx loop */
	usb_unlock();				

	/* follow return path of this interface */
	if (msg_p->return_path.callback_func) {
		msg_p->return_path.callback_func(rsp_msg_p);
		rvf_free_buf(rsp_msg_p);
	} else {
		if (RVF_OK != rvf_send_msg (msg_p->return_path.addr_id, rsp_msg_p)) {
			USB_SEND_TRACE("USB: could ready subscribe response", RV_TRACE_LEVEL_ERROR);
			ret = RVM_MEMORY_ERR;
		}
	}
	/* TODO: HW enable all associated endpoints? */
	return ret;
}



static T_RV_RET set_tx_buffer(T_USB_SET_TX_BUFFER_MSG *msg_p)
{	
	T_RV_RET	ret = RV_INVALID_PARAMETER;
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;

	usb_lock();
	if((USB_CNFGNR != USB_CONF_DEFAULT) &&
		(USB_CNFGNR != USB_CONF_ADRESSED)) 
	{
		if(msg_p->endpoint == 0)
		{
			endpoint_vs_interface_entry_p = 
					&(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[msg_p->interface_id]);
		}
		else
		{
			//find physical ep
			endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl + 
				(LEP_2_PEP(msg_p->endpoint, msg_p->interface_id, USB_TX));
		}

		//store data
		endpoint_vs_interface_entry_p->buf_p = msg_p->buffer_p;
		endpoint_vs_interface_entry_p->buf_size = msg_p->size;
		endpoint_vs_interface_entry_p->shorter_transfer = msg_p->shorter_transfer;
		endpoint_vs_interface_entry_p->temp_buf_p = endpoint_vs_interface_entry_p->buf_p;
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
		if(msg_p->endpoint == 0)
		{
			prepare_first_ep0_trsfr(msg_p->interface_id);
		}
		else
		{
			if(prepare_first_tx_trsfr(msg_p->endpoint, msg_p->interface_id) != USB_OK) 
			{
				ret = USB_INTERNAL_ERR;
				USB_SEND_TRACE("USB: prepare_first_tx_trsfr failed",
				RV_TRACE_LEVEL_ERROR);	
			}
		}
	}
	usb_unlock();
	return ret;
}

static T_RV_RET set_rx_buffer(T_USB_SET_RX_BUFFER_MSG *msg_p)
{	
	T_RV_RET	ret = RV_INVALID_PARAMETER;
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;
	
	usb_lock();
	if((USB_CNFGNR != USB_CONF_DEFAULT) &&
		(USB_CNFGNR != USB_CONF_ADRESSED)) 
	{
		if(msg_p->endpoint == 0)
		{
			endpoint_vs_interface_entry_p = 
					&(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[msg_p->interface_id]);
		}
		else
		{
			//find physical ep
			endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl + 
				(LEP_2_PEP(msg_p->endpoint, msg_p->interface_id, USB_RX));
		}

		//store data
		endpoint_vs_interface_entry_p->buf_p = msg_p->buffer_p;
		endpoint_vs_interface_entry_p->buf_size = msg_p->size;
		endpoint_vs_interface_entry_p->temp_buf_p = endpoint_vs_interface_entry_p->buf_p;
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
		if(msg_p->endpoint == 0)
		{
			configure_ep0_interface(msg_p->interface_id);
		}
		else
		{
			if(rmv_stall_cont_rx_trsfr(msg_p->endpoint, msg_p->interface_id) != USB_OK) 
			{
				ret = USB_INTERNAL_ERR;
				USB_SEND_TRACE("USB: rmv_stall_cont_rx_trsfr failed",
				RV_TRACE_LEVEL_ERROR);	
			}
		}
	}

	usb_unlock();
	return ret;
}

static T_RV_RET reclaim_rx_buffer(T_USB_RECLAIM_RX_BUFFER_MSG *msg_p)
{	
	T_RV_RET	ret = RV_INVALID_PARAMETER;
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;
	
	usb_lock();
	if((USB_CNFGNR != USB_CONF_DEFAULT) &&
		(USB_CNFGNR != USB_CONF_ADRESSED)) {
		T_USB_RX_BUFFER_FULL_MSG *rsp_msg_p;
		T_RV_RETURN_PATH rp;
			ret = RV_OK;

		endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl + 
			(LEP_2_PEP(msg_p->endpoint, msg_p->interface_id, USB_RX));
		 rp = endpoint_vs_interface_entry_p->interface_data_p->swe_return_path;
		if (RVF_RED == rvf_get_msg_buf(usb_env_ctrl_blk_p->prim_mb_id, 
				sizeof (T_USB_RX_BUFFER_FULL_MSG),
				USB_RX_BUFFER_FULL_MSG,
				((T_RVF_MSG**) &rsp_msg_p))) {
			USB_SEND_TRACE("USB: out of memory", RV_TRACE_LEVEL_ERROR);
			ret = RVM_MEMORY_ERR;
		}
		rsp_msg_p->endpoint = msg_p->endpoint;
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

static T_RV_RET get_status(T_USB_GET_STATUS_MSG* msg_p)
{
	T_RV_RET	ret = USB_OK;
	UINT8		if_cntr = 0;
	UINT8		fm_cntr = 0;
	UINT8		active_if_cntr = 0;
	UINT8		active_fm_cntr = 0;
	T_USB_NR_ACTIVE_FM* fm_active_cntr;
	BOOL		exists = FALSE;

	/* Memory bank status (red, yellow, green). */
	T_RVF_MB_STATUS mb_status;

	/* reserve memory for tx_endpoints buffer*/
	mb_status = rvf_get_buf(usb_env_ctrl_blk_p->prim_mb_id, 
							((sizeof(T_USB_NR_ACTIVE_FM)) * USB_CURRENT_CNFG_NR_IF),
							(T_RVF_BUFFER**)&fm_active_cntr);
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
	}
	if(ret == USB_OK)
	{
		/*configurte created table*/
		for(if_cntr = 0; if_cntr < USB_CURRENT_CNFG_NR_IF; if_cntr++)
		{	
			fm_active_cntr[if_cntr].addr_id = 0;
			fm_active_cntr[if_cntr].nr_of_fm = 0;
		}
		/**
		 * find number of active FM (1 fm can have 3 active if's)
		 * and store active interface in the list provided by the caller 
		 */
		for(if_cntr = 0; if_cntr < USB_CURRENT_CNFG_NR_IF; if_cntr++)
		{
			if(if_cntr == 0)
			{
				/*this could be the first active interface */
				if(USB_INTERFACE_ACTIVE(if_cntr) == TRUE)
				{
					/*it is active*/
					//fm_active_cntr[if_cntr].addr_id = USB_FM_ADDR(if_cntr);
					/*since it is the first nr is 1*/
					//fm_active_cntr[if_cntr].nr_of_fm = FIRST_FM;
					
					/*create active interface list*/
					//msg_p->status_p->fm_id_list_p[active_if_cntr].interface_class_id = 
					//	USB_IF_CLASS_ID(if_cntr);
					//msg_p->status_p->fm_id_list_p[active_if_cntr].interface_subclass_id = 
					//	USB_IF_SUB_CLASS_ID(if_cntr);
					/*increment active interface list index*/
					active_if_cntr++;
				}
			}
			else
			{
				/*is this an active interface ?*/
				if(USB_INTERFACE_ACTIVE(if_cntr) == TRUE)
				{
					/*yes it is*/
					
					/*start new search for fm it has not been found in this stadium 
					 so set exists to false*/
					exists = FALSE;

					/*it is probably not the first see if this fm has been used before*/
					for(fm_cntr = 0; if_cntr < USB_CURRENT_CNFG_NR_IF; if_cntr++)
					{
						//if(USB_FM_ACTIVE(fm_cntr, USB_FM_ADDR(if_cntr)) != TRUE)
						//{
						//	/*it exists, increment the number of fm's active*/
						//	fm_active_cntr[fm_cntr].nr_of_fm++;
						//	exists = TRUE;
						//}
					}
					if(exists == FALSE)
					{
						/*this fm was not found before so it is the first active instance*/
						fm_cntr = 0;
						/* find first free table entry to store new active fm*/
						while((fm_active_cntr[fm_cntr].addr_id != 0) && (fm_cntr < USB_CURRENT_CNFG_NR_IF))
						{
							fm_cntr++;
						}
						if(fm_cntr < USB_CURRENT_CNFG_NR_IF)
						{
							/*a new table entry was found store new fm*/
						//	fm_active_cntr[fm_cntr].addr_id = USB_FM_ADDR(if_cntr);
						//	fm_active_cntr[fm_cntr].nr_of_fm++;
						}
						else
						{	/*strange... the table should be as large as the number of intefaces*/
							/*there can never be more active fm's as there are interfaces*/
							USB_SEND_TRACE("USB: Error to find table entry to store active fm info",RV_TRACE_LEVEL_ERROR);
							ret = USB_INTERNAL_ERR;
							break; /*quit for loop*/
						}
					}
					/*update active interface list*/
				//msg_p->status_p->fm_id_list_p[active_if_cntr].interface_class_id = 
				//	USB_IF_CLASS_ID(if_cntr);
				//msg_p->status_p->fm_id_list_p[active_if_cntr].interface_subclass_id = 
				//	USB_IF_SUB_CLASS_ID(if_cntr);
					/*increment active interface list index*/
					active_if_cntr++;
				}/*is this an active interface*/
			}/*cntr != 0*/
		}/*end of interfaceslist*/ 
		/*count nr of active fm's*/
		active_fm_cntr = 0;
		for(fm_cntr = 0; if_cntr < USB_CURRENT_CNFG_NR_IF; if_cntr++)
		{
			if((fm_active_cntr[fm_cntr].addr_id != 0) && (fm_cntr <USB_CURRENT_CNFG_NR_IF))
			{
				/*if this table entry contains a value include it in the count*/
				active_fm_cntr++;
			}
		}
		/**
		 *assign value to variable provided by the caller
		 */
	//msg_p->status_p->nr_active_fm = active_fm_cntr;
		/**
		 *the active interfacelist is already filled in the previous part
		 */
		/**
		 * check driver_ready and assign value to the caller
		 */
		msg_p->status_p->driver_ready = usb_env_ctrl_blk_p->driver_ready;
		/**
		 * check usb connected and assign value to the caller
		 */
		msg_p->status_p->usb_connected = usb_env_ctrl_blk_p->host_data.usb_connected;
		
		/*free the memory reserved from the riviera manager to temporarily store 
		  the active FM list in*/
		rvf_free_buf(fm_active_cntr);

		if(ret == USB_OK)
		{
			/*send T_USB_STATUS_READY_MSG to caller, only if the status is 
			successfully gathered*/
			ret = send_usb_status_ok();
		}
	}
	return ret;
}

static T_RV_RET send_usb_status_ok(void)
{
	/* Memory bank status (red, yellow, green). */
	T_RVF_MB_STATUS mb_status;
	
	T_USB_STATUS_READY_MSG* status_ready_p;
	/*
	 * Get a message buffer.
	 * The special function for messages rvf_get_msg_buf is used to get the
	 * message buffer.
	 */
	mb_status = rvf_get_msg_buf(usb_env_ctrl_blk_p->prim_mb_id,
								sizeof(T_USB_STATUS_READY_MSG),
								USB_STATUS_READY_MSG,
								(T_RVF_MSG**) &status_ready_p);
	if (mb_status == RVF_RED)
	{
		/*
		 * The flag returned by rvf_get_buf is red, there is not enough
		 * memory to allocate the buffer.
		 */
		USB_SEND_TRACE("USB: Error to get memory for status ready msg",RV_TRACE_LEVEL_ERROR);
				
		return USB_MEMORY_ERR;
	}
	else if (mb_status == RVF_YELLOW)
	{
		/*
		 * The flag is yellow, there will soon be not enough memory anymore.
		 */
		USB_SEND_TRACE("USB: Getting short on memory for status ready msg", RV_TRACE_LEVEL_WARNING);
	}
	
	/* Init message specific parameters. */
	/*none*/

	/* Send the message using mailbox. */
	return rvf_send_msg(usb_env_ctrl_blk_p->addr_id, (void*)status_ready_p);
}


