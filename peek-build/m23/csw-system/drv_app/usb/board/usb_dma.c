/**
 * @file	usb_dma.c
 *
 * DMA management by the USB controler.
 *
 * @author	Pierre-Olivier POUX
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	21/04/2004	Pierre-Olivier POUX (Philog)
 *
 * (C) Copyright 2004 by Philog, All Rights Reserved
 */

#include "chipset.cfg"
#include "sys_types.h"

#include "usb/usb_i.h"
#include "usb/usb_inth.h"

#include "dma/sys_dma.h"

#if (CHIPSET==15)
#include "dma/dma_message.h"
#include "dma/dma_api.h"
#else
#include "dmg/dmg_message.h"
#include "dmg/dmg_api.h"
#endif

#include "usb/usb_dma.h"

#include "inth/sys_inth.h"

#if (LOCOSTO_LITE)
#ifndef USB_DMA_SWE_REMOVAL
#define USB_DMA_SWE_REMOVAL
#endif
#endif

#ifdef USB_DMA_SWE_REMOVAL
extern T_DMA_CALL_BACK pf_dma_call_back_address[C_DMA_NUMBER_OF_CHANNEL]; 
#endif //USB_DMA_SWE_REMOVAL

/****************************************************************************************
 |	MACRO DEFINITIONS																	|
 V**************************************************************************************V*/

#if (CHIPSET!=15)
#define USB_DMG_SRC_ADDR_READ		(UINT32) &W2FC_DATA_DMA
#define USB_DMG_DEST_ADDR_WRITE		(UINT32) &W2FC_DATA_DMA
#else
#define USB_DMA_SRC_ADDR_READ		(UINT32) &W2FC_DATA_DMA
#define USB_DMA_DEST_ADDR_WRITE		(UINT32) &W2FC_DATA_DMA
#endif //CHIPSET!=15

/****************************************************************************************
 |	GLOBAL VARIABLES																	|
 V**************************************************************************************V*/

#ifndef USB_DMA_SWE_REMOVAL
#if (CHIPSET!=15)
T_DMG_CHANNEL		usb_dma_rx_channel_tbl[] = {
	DMG_ANY_CHANNEL,
	DMG_ANY_CHANNEL,
	DMG_ANY_CHANNEL
};

T_DMG_PERIPHERAL	usb_dma_rx_peripheral_tbl[] = {
	DMG_PERIPHERAL_USB_RX1,
	DMG_PERIPHERAL_USB_RX2,
	DMG_PERIPHERAL_USB_RX3
};

T_DMG_CHANNEL		usb_dma_tx_channel_tbl[] = {
	DMG_ANY_CHANNEL,
	DMG_ANY_CHANNEL,
	DMG_ANY_CHANNEL
};

T_DMG_PERIPHERAL	usb_dma_tx_peripheral_tbl[] = {
	DMG_PERIPHERAL_USB_TX1,
	DMG_PERIPHERAL_USB_TX2,
	DMG_PERIPHERAL_USB_TX3
};

#else
T_DMA_CHANNEL		usb_dma_rx_channel_tbl[] = {
	DMA_CHAN_ANY,
	DMA_CHAN_ANY,
	DMA_CHAN_ANY
};

T_DMA_SYNC_DEVICE usb_dma_rx_peripheral_tbl[] = {
	DMA_SYNC_DEVICE_USB_RX1,
	DMA_SYNC_DEVICE_USB_RX2,
	DMA_SYNC_DEVICE_USB_RX3
};

T_DMA_CHANNEL		usb_dma_tx_channel_tbl[] = {
	DMA_CHAN_ANY,
	DMA_CHAN_ANY,
	DMA_CHAN_ANY
};

T_DMA_SYNC_DEVICE	usb_dma_tx_peripheral_tbl[] = {
	DMA_SYNC_DEVICE_USB_TX1,
	DMA_SYNC_DEVICE_USB_TX2,
	DMA_SYNC_DEVICE_USB_TX3
};
#endif //CHIPSET!=15
#else

T_DMA_TYPE_CHANNEL_NUMBER usb_dma_rx_channel_tbl[] = {
	C_DMA_CHANNEL_1,
	C_DMA_CHANNEL_3
};

T_DMA_TYPE_CHANNEL_HW_SYNCH usb_dma_rx_peripheral_tbl[] = {
	C_DMA_CHANNEL_USB_RX1,
	C_DMA_CHANNEL_USB_RX2,
	C_DMA_CHANNEL_USB_RX3
};

T_DMA_TYPE_CHANNEL_NUMBER usb_dma_tx_channel_tbl[] = {
	C_DMA_CHANNEL_4,
	C_DMA_CHANNEL_5
};

T_DMA_TYPE_CHANNEL_HW_SYNCH usb_dma_tx_peripheral_tbl[] = {
	C_DMA_CHANNEL_USB_TX1,
	C_DMA_CHANNEL_USB_TX2,
	C_DMA_CHANNEL_USB_TX3
};
#endif //USB_DMA_SWE_REMOVAL

T_RV_RET usb_process_dma_request(T_DMA_TYPE_CHANNEL_NUMBER	dma_channel);

/**
 *---------------------------------------------------------------------------------*
 * @name usb_wait_for_message.
 *
 * This function can wait to a specified message
 *
 * @param	msg_id	Waited message ID, null for any message.
 *
 * @return	The received message.
 *---------------------------------------------------------------------------------*
 */
T_RV_HDR*	usb_wait_for_message(UINT32 msg_id)
{
	T_RV_HDR * msg_p;

	for (;;)
	{
		/* Waiting for an event, blocking USBFAX task. */
		if (rvf_wait(0xFFFF, 0) & RVF_TASK_MBOX_0_EVT_MASK)
		{
			msg_p = (T_RV_HDR*)rvf_read_mbox(0);
			if (msg_p != NULL)
			{
				if (msg_id)
				{
					/* If expected msg received, return it */
					if (msg_p->msg_id == msg_id)
					{
						return msg_p;
					}
					else
					{
						USB_SEND_TRACE("USB DMA: Bad message received : OK", RV_TRACE_LEVEL_DEBUG_LOW);
					}

					rvf_free_buf(msg_p);
				}
				else
				{
					return msg_p;
				}
			}
		}
	}
}
/*@}*/


/**
 *---------------------------------------------------------------------------------*
 * @name usb_subscribe_dma_channel.
 *
 *
 *
 * @param	dma_channel		: DMA channel
 *			dma_peripheral	:
 *
 * @return
 *---------------------------------------------------------------------------------*
 */
#ifndef USB_DMA_SWE_REMOVAL 
#if (CHIPSET !=15)
T_DMG_CHANNEL	usb_subscribe_dma_channel(T_DMG_CHANNEL dma_channel, T_DMG_PERIPHERAL dma_peripheral)
{
	T_DMG_STATUS_RSP_MSG *msg_p;

	T_RV_RETURN_PATH dmg_test_path;

	T_DMG_CHANNEL_DATA dmg_test_channel_data_p;
	T_DMG_REQUEST_DATA dmg_test_request_data_p;

	T_DMG_CHANNEL channel = 255;

	/* Setup parameters for this test */
	dmg_test_channel_data_p.channel        = dma_channel;
	dmg_test_channel_data_p.lock           = DMG_CHANNEL_LOCKED;

	dmg_test_request_data_p.dmg_peripheral = dma_peripheral;
	dmg_test_request_data_p.dmg_queued     = DMG_QUEUE_ENABLE;

	if (dmg_reserve_channel (&dmg_test_channel_data_p,
							&dmg_test_request_data_p,
							usb_env_ctrl_blk_p->usb_return_path))
	{
		USB_SEND_TRACE("DMG TEST immediate return NOT OK ", RV_TRACE_LEVEL_DEBUG_LOW);
	}

	return(channel);
}
 #else
 T_DMA_CHANNEL	usb_subscribe_dma_channel(T_DMA_CHANNEL dma_channel, T_DMA_SYNC_DEVICE dma_peripheral)
{
	T_DMA_SPECIFIC specific;
	T_DMA_CHANNEL channel;
	T_RV_RET ret;

	if( DMA_CHAN_ANY == dma_channel )
	{
		specific = DMA_CHAN_ANY;
		channel = 0;
	}
	else
	{
		specific = DMA_CHAN_SPECIFIC;
		channel = dma_channel;
	}

ret = dma_reserve_channel(specific, channel, DMA_QUEUE_DISABLE, 10,
							usb_env_ctrl_blk_p->usb_return_path);
#if 0
	if( DMA_RESERVE_OK != dma_reserve_channel(specific, channel, DMA_QUEUE_DISABLE, 10,
							usb_env_ctrl_blk_p->usb_return_path))
#endif
	if(RV_OK != ret)
	{
		USB_SEND_TRACE("DMA TEST immediate return NOT OK ", RV_TRACE_LEVEL_DEBUG_LOW);
	}

	return(channel);
}
#endif //(CHIPSET!=15)
#else
T_DMA_TYPE_CHANNEL_NUMBER usb_subscribe_dma_channel(T_DMA_TYPE_CHANNEL_NUMBER dma_channel, 
							T_DMA_TYPE_CHANNEL_HW_SYNCH dma_peripheral)
{
	USB_SEND_TRACE("USB-usb_subscribe_dma_channel", RV_TRACE_LEVEL_DEBUG_LOW); 
	/* reserve the channel for ARM */
	f_dma_channel_allocation_set(dma_channel, C_DMA_CHANNEL_ARM);
	/* enable the channel */
	//f_dma_channel_enable(dma_channel);
	usb_process_dma_request(dma_channel);
	usb_dma_unlock();
	
	return(dma_channel);
}
#endif //USB_DMA_SWE_REMOVAL

/**
 *--------------------------------------------------------------------------------*
 * @name name unsubscribe_dma_usb_channel
 *
 *
 *  @param interface_id
 *
 *  @return	BOOL 		Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */
#ifndef USB_DMA_SWE_REMOVAL 
#if (CHIPSET!=15)
T_RV_RET unsubscribe_dma_usb_channel(T_DMG_CHANNEL	dma_channel)
{
	T_DMG_STATUS_RSP_MSG *msg_p;

	if(dma_channel != 0xFF)
	{
		USB_TRACE_WARNING_PARAM("USB DMA: realease DMA channel : ",	dma_channel);

		if (dmg_release_channel (dma_channel, usb_env_ctrl_blk_p->usb_return_path) != RV_OK)
		{
			USB_SEND_TRACE("DMG TEST immediate return NOT OK ", RV_TRACE_LEVEL_DEBUG_LOW);
		}
	}
}
 #else
 
 T_RV_RET unsubscribe_dma_usb_channel(T_DMA_CHANNEL	dma_channel)
{
	if(dma_channel != 0xFF)
	{
		USB_TRACE_WARNING_PARAM("USB DMA: realease DMA channel : ",	dma_channel);

		if (dma_release_channel (dma_channel) != DMA_OK)
		{
			USB_SEND_TRACE("DMA TEST immediate return NOT OK ", RV_TRACE_LEVEL_DEBUG_LOW);
		}
	}
	return (RV_OK);
}
#endif //(CHIPSET!=15)
#else
T_RV_RET unsubscribe_dma_usb_channel(T_DMA_TYPE_CHANNEL_NUMBER dma_channel)
{
	USB_SEND_TRACE("USB-unsubscribe_dma_usb_channel", RV_TRACE_LEVEL_DEBUG_LOW); 
	/* disable the dma transfer for the channel */
	f_dma_channel_disable(dma_channel); 
	/* reset the channel */
	f_dma_channel_soft_reset(dma_channel);
	return (RV_OK);
}
#endif //USB_DMA_SWE_REMOVAL

/**
 *--------------------------------------------------------------------------------*
 * @name name reserve_usb_channels
 *
 *
 *  @param no param
 *
 *  @return	T_RV_RET	Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */
T_RV_RET	usb_reserve_dma_channels()
{
#ifndef USB_DMA_SWE_REMOVAL
#if (CHIPSET==15)
	T_DMA_CHANNEL		dma_channel		= 0;
	T_DMA_SYNC_DEVICE   dma_peripheral	= 0;
#else
	T_DMG_CHANNEL		dma_channel		= 0;
	T_DMG_PERIPHERAL	dma_peripheral	= 0;
#endif //(CHIPSET==15)
#else
	T_DMA_TYPE_CHANNEL_NUMBER dma_channel = 0;
	T_DMA_TYPE_CHANNEL_HW_SYNCH dma_peripheral = 0;
#endif // USB_DMA_SWE_REMOVAL

	UINT8	i = 0;

	/*
	 * Reserve dma receiption channels
	 */
	// for(i = 0; i < 2; i++)
	for(i = 0; i < DMA_RX_CHANNELS; i++)
	{
		dma_channel		= usb_dma_rx_channel_tbl[i];
		dma_peripheral	= usb_dma_rx_peripheral_tbl[i];

//		USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].dmg_channel =
			usb_subscribe_dma_channel(dma_channel, dma_peripheral);
//		USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].channel_used = TRUE;
	}

	/*
	 * Reserve dma receiption channels
	 */
	// for(i = 0; i < 2; i++)
	for(i = 0; i < DMA_TX_CHANNELS; i++)
	{
		dma_channel		= usb_dma_tx_channel_tbl[i];
		dma_peripheral	= usb_dma_tx_peripheral_tbl[i];

//		USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].dmg_channel =
			usb_subscribe_dma_channel(dma_channel, dma_peripheral);
//		USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].channel_used = TRUE;
	}

	return (RV_OK);
}
/*@}*/


void	usb_init_dma_channel_variables()
{

	UINT8	i = 0;

	for(i = 0; i < DMA_RX_CHANNELS; i++)
	{
		USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].channel_used = FALSE;
	}

	for(i = 0; i < DMA_TX_CHANNELS; i++)
	{
		USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].channel_used = FALSE;
	}

	return;
}
/*@}*/


/**
*--------------------------------------------------------------------------------*
 * @name name subscribe_dma_usb_interface
 *
 *
 *  @param interface_id
 *
 *  @return	BOOL 		Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */
T_RV_RET subscribe_dma_usb_interface(UINT8	interface_id)
{
	BOOL	prev_ep_is_bulk_rx64 = FALSE;
	BOOL	prev_ep_is_bulk_tx64 = FALSE;

	UINT8       ep_ctr = 0;
	UINT8		i = 0;
	UINT8		j = 0;
	

	UINT8		current_config = 1;

	T_RV_RET	ret = RV_OK;
	const T_USB_EP_DESCR* if_logical_ep_array_p;
	const T_USB_INTERFACE_DESCR*	ifdescr_p;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	INT8 lif;
#endif

	for(j=0;j<3;j++)
	{
		if(configurations_table[current_config-1].if_cnfg_p[j].ifdescr_p->bInterfaceNumber
																	== interface_id)
			break;
	}

	/*
		NOTE:
		'interface_id' is replaced with 'j'. This will work even if 'DYNAMIC_CONFI_SUPPORT'
		is disabled. Because with 'DYNAMIC_CONFIG_SUPPORT' disabled, interface number in
		interface descriptor is same as interface number used by FM.
	*/
	if_logical_ep_array_p =
		configurations_table[current_config-1].if_cnfg_p[j].if_logical_ep_array_p;

	ifdescr_p =
		(configurations_table[current_config-1].if_cnfg_p[j]).ifdescr_p;

#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	lif = pif_2_lif(usb_env_ctrl_blk_p->enumInfo.info, interface_id);
	if(lif == -1)
	{
		return RV_NOT_SUPPORTED;
	}
	interface_id = lif;
#endif


	/*
	 * For each endpoint of the current interface
	 */
	for(	ep_ctr = 0;
			ep_ctr < ifdescr_p->bNumEndpoints;
			ep_ctr++)
		{
			T_USB_EP_DESCR ep_type = if_logical_ep_array_p[ep_ctr];
			switch(ep_type)
			{
				case EP_RX64 :
					/* a pair of IN/OUT bulk endpointhas been found */
					if (prev_ep_is_bulk_tx64 == TRUE)
					{
						i = 0;
						/* Find a free DMA channel */
						while((i < DMA_TX_CHANNELS)
							   && (USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].endpoint_id != DMA_INVALID_ENDPOINT))
						{
							i++;
						}

						if(i == DMA_TX_CHANNELS)
						{
							return(RV_NOT_SUPPORTED);
						}

						/* Save interface identifier */
						USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].interface_id	=
							USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].interface_id = interface_id;

						/* previous endpoint is a IN Bulk endpoint */
						USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].endpoint_id	= ep_ctr + 1;

						/* current endpoint is a IN Bulk endpoint */
						USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].endpoint_id	= ep_ctr;

						/* Reset flags */
						prev_ep_is_bulk_tx64 = FALSE;
					}
					else
					{
						prev_ep_is_bulk_rx64 = TRUE;
					}
				break;

				case EP_TX64 :
					/* a pair of IN/OUT bulk endpointhas been found */
					if (prev_ep_is_bulk_rx64 == TRUE)
					{
						i = 0;
						/* Find a free DMA channel */
						while((i < DMA_RX_CHANNELS)
							   && (USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].endpoint_id != DMA_INVALID_ENDPOINT))
						{
							i++;
						}

						if(i == DMA_RX_CHANNELS)
						{
							return(RV_NOT_SUPPORTED);
						}

						/* Save interface identifier */
						USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].interface_id	=
							USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].interface_id = interface_id;

						USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].endpoint_id	= ep_ctr + 1;
						USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].endpoint_id	= ep_ctr;

						prev_ep_is_bulk_rx64 = FALSE;
					}
					else
					{
						prev_ep_is_bulk_tx64 = TRUE;
					}
				break;

				default:
					prev_ep_is_bulk_rx64 = FALSE;
					prev_ep_is_bulk_tx64 = FALSE;
				break;
			}
		}

	return(ret);
}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name name subscribe_dma_usb_interface
 *
 *
 *  @param interface_id
 *
 *  @return	BOOL 		Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */
T_RV_RET init_dma_usb_tbl()
{
	UINT8	i = 0;

	USB_SEND_TRACE("USB DMA Cross Ref table initialization", RV_TRACE_LEVEL_DEBUG_LOW);

	for(i = 0; i < DMA_RX_CHANNELS; i++)
	{
		USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].endpoint_id = DMA_INVALID_ENDPOINT;
		USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].subscription_order = 0;
	}

	for(i = 0; i < DMA_TX_CHANNELS; i++)
	{
		USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].endpoint_id = DMA_INVALID_ENDPOINT;
		USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].subscription_order = 0;
	}

	return (RV_OK);
}
/*@}*/


/**
 *--------------------------------------------------------------------------------*
 * @name name subscribe_dma_usb_interface
 *
 *
 *  @param interface_id
 *
 *  @return	BOOL 		Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */
T_RV_RET unsubscribe_dma_usb_interface(UINT8 interface_id)
{
	UINT8	i = 0;
	T_RV_RET ret = RV_OK;
	for(i = 0; i < DMA_RX_CHANNELS; i++)
	{
		if(USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].interface_id == interface_id)
		{
			if(TRUE == USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].channel_used)
			{
				ret = unsubscribe_dma_usb_channel(USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].dmg_channel);
				if(RV_OK == ret)
				{
					USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].channel_used = FALSE;
					USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].endpoint_id = DMA_INVALID_ENDPOINT;
					USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].channel_used = FALSE;
					USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].endpoint_id = DMA_INVALID_ENDPOINT;					
				}
			}
			USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].endpoint_id = DMA_INVALID_ENDPOINT;
			/* 
				Disable associated timer 
			*/
			(void) NU_Control_Timer (&(USB_DMA_CFG.dma_usb_rx_timer[i]),
				                     NU_DISABLE_TIMER);
		}
	}

	for(i = 0; i < DMA_TX_CHANNELS; i++)
	{
		if(USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].interface_id == interface_id)
		{
			if(TRUE == USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].channel_used)
			{
				ret = unsubscribe_dma_usb_channel(USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].dmg_channel);
				if(RV_OK == ret)
				{
					USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].channel_used = FALSE;
					USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].endpoint_id = DMA_INVALID_ENDPOINT;					
				}
			}
					USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].endpoint_id = DMA_INVALID_ENDPOINT;
		}
	}

	return (RV_OK);
}
/*@}*/

/**
*--------------------------------------------------------------------------------*
 * @name usb_start_write_dma_transfer
 *
 *
 *  @param interface_id
 *
 *  @return	BOOL 		Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */
#ifndef USB_DMA_SWE_REMOVAL
#if (CHIPSET!=15)
T_RV_RET usb_start_write_dma_transfer(T_DMG_CHANNEL dma_channel, T_DMG_PERIPHERAL dma_peripheral, UINT8* buffer_p,
					UINT16 size, T_USB_DMA_REQUEST_INFORMATION* current_dma_request)
{
	T_DMG_CHANNEL_PARAMETERS	dmg_channel_param;
	T_DMG_STATUS_RSP_MSG*		msg_p;

	BOOL	dma_request_result = TRUE;

	static UINT8 mmc_afl = 1;/*0x20;*/

	USB_SEND_TRACE("USB-usb_start_write_dma_transfer ", RV_TRACE_LEVEL_DEBUG_LOW);

	/*Set DMA channel parameters*/
	// dmg_channel_param.data_width               = DMG_DATA_S16;
	dmg_channel_param.data_width               = DMG_DATA_S8;// CHB
	dmg_channel_param.sync                     = dma_peripheral;
	dmg_channel_param.hw_priority              = DMG_HW_PRIORITY_LOW;
	dmg_channel_param.repeat                   = DMG_SINGLE;
	dmg_channel_param.flush                    = DMG_FLUSH_DISABLED;

	// CHB
	dmg_channel_param.nmb_elements             = current_dma_request->W2FC_DMA_Element0;// CHB size; // CHB size / (mmc_afl * sizeof(UINT16));
	dmg_channel_param.nmb_frames               = current_dma_request->W2FC_DMA_Frame0;// CHB 1;

	USB_TRACE_WARNING_PARAM("USB DMA: dmg_channel_param.nmb_elements",	dmg_channel_param.nmb_elements);
	USB_TRACE_WARNING_PARAM("USB DMA: dmg_channel_param.nmb_frames",	dmg_channel_param.nmb_frames);

	dmg_channel_param.dmg_end_notification     = DMG_NOTIFICATION;
	dmg_channel_param.secure                   = DMG_NOT_SECURED;

	dmg_channel_param.source_address		= (UINT32)buffer_p;
	dmg_channel_param.source_address_mode	= DMG_ADDR_MODE_POST_INC;
	dmg_channel_param.source_packet			= DMG_NOT_PACKED;
	dmg_channel_param.source_burst			= DMG_NO_BURST;

	dmg_channel_param.destination_address		= (UINT32) &W2FC_DATA_DMA;
	dmg_channel_param.destination_address_mode  = DMG_ADDR_MODE_CONSTANT;
	dmg_channel_param.destination_packet		= DMG_NOT_PACKED;
	dmg_channel_param.destination_burst			= DMG_NO_BURST;

	dmg_channel_param.double_buf_destination_address = NULL;
	dmg_channel_param.double_buf_source_address      = NULL;

    /*Reserve DMA channel*/
    if( dmg_set_channel_parameters(dma_channel, &dmg_channel_param, usb_env_ctrl_blk_p->usb_return_path)!= RV_OK)
    {
		USB_SEND_TRACE("USB	error calling dmg_set_channel_parameters", RV_TRACE_LEVEL_ERROR);
		dma_request_result  = FALSE;
    }

	USB_SEND_TRACE("USB DMA: dmg_set_channel_parameters : OK", RV_TRACE_LEVEL_DEBUG_LOW);

}
 #else
T_RV_RET usb_start_write_dma_transfer(T_DMA_CHANNEL dma_channel, T_DMA_SYNC_DEVICE dma_peripheral, 
			UINT8* buffer_p, UINT16 size, T_USB_DMA_REQUEST_INFORMATION *current_dma_request)
{
	T_DMA_CHANNEL_PARAMETERS	dma_channel_param;

	BOOL	dma_request_result = TRUE;

	static UINT8 mmc_afl = 1;/*0x20;*/

	USB_SEND_TRACE("USB-usb_start_write_dma_transfer ", RV_TRACE_LEVEL_DEBUG_LOW);

	/*Set DMA channel parameters*/
	// dmg_channel_param.data_width               = DMG_DATA_S16;
	dma_channel_param.data_width               = DMA_DATA_S8;// CHB
	dma_channel_param.sync                     = dma_peripheral;
	dma_channel_param.hw_priority              = DMA_HW_PRIORITY_LOW;
	dma_channel_param.dma_mode                 = DMA_SINGLE;
	dma_channel_param.flush                    = DMA_FLUSH_DISABLED;

	// CHB
	dma_channel_param.nmb_elements             = current_dma_request->W2FC_DMA_Element0;// CHB size; // CHB size / (mmc_afl * sizeof(UINT16));
	dma_channel_param.nmb_frames               = current_dma_request->W2FC_DMA_Frame0;// CHB 1;

	USB_TRACE_WARNING_PARAM("USB DMA: dma_channel_param.nmb_elements",	dma_channel_param.nmb_elements);
	USB_TRACE_WARNING_PARAM("USB DMA: dma_channel_param.nmb_frames",	dma_channel_param.nmb_frames);

	dma_channel_param.dma_end_notification     = DMA_NOTIFICATION;
	dma_channel_param.secure                   = DMA_NOT_SECURED;

	dma_channel_param.source_address		= (UINT32)buffer_p;
	dma_channel_param.source_address_mode	= DMA_ADDR_MODE_POST_INC;
	dma_channel_param.source_packet			= DMA_NOT_PACKED;
	dma_channel_param.source_burst			= DMA_NO_BURST;

	dma_channel_param.destination_address		= (UINT32) &W2FC_DATA_DMA;
	dma_channel_param.destination_address_mode  = DMA_ADDR_MODE_CONSTANT;
	dma_channel_param.destination_packet		= DMA_NOT_PACKED;
	dma_channel_param.destination_burst			= DMA_NO_BURST;

	dma_channel_param.transfer = DMA_MODE_TRANSFER_ENABLE;
	
    /*Reserve DMA channel*/
    if( dma_set_channel_parameters(	dma_channel, &dma_channel_param) != DMA_OK )
    {
		USB_SEND_TRACE("USB	error calling dma_set_channel_parameters", RV_TRACE_LEVEL_ERROR);
		dma_request_result  = FALSE;
    }

	USB_SEND_TRACE("USB DMA: dma_set_channel_parameters : OK", RV_TRACE_LEVEL_DEBUG_LOW);

    return (RV_OK);
}
#endif //(CHIPSET!=15)
#else
T_RV_RET usb_start_write_dma_transfer(T_DMA_TYPE_CHANNEL_NUMBER dma_channel, T_DMA_TYPE_CHANNEL_HW_SYNCH dma_peripheral, 
				UINT8* buffer_p, UINT16 size, T_USB_DMA_REQUEST_INFORMATION* current_dma_request)
{
	T_DMA_TYPE_CHANNEL_PARAMETER dma_channel_param;

	//BOOL dma_request_result = TRUE;

	//static UINT8 mmc_afl = 1;/*0x20;*/

	USB_SEND_TRACE("USB-usb_start_write_dma_transfer ", RV_TRACE_LEVEL_DEBUG_LOW); 

	/*Set DMA channel parameters*/
	dma_channel_param.pf_dma_call_back_address = pf_dma_call_back_address[dma_channel];
	dma_channel_param.d_dma_channel_number = dma_channel;
	
	dma_channel_param.d_dma_channel_secured = C_DMA_CHANNEL_NOT_SECURED;
	dma_channel_param.d_dma_channel_data_type = C_DMA_DATA_S8; 
		
	dma_channel_param.d_dma_src_channel_packed = C_DMA_CHANNEL_NOT_PACKED;
	dma_channel_param.d_dma_src_channel_burst_en = C_DMA_CHANNEL_SINGLE; 
	dma_channel_param.d_dma_dst_channel_packed = C_DMA_CHANNEL_NOT_PACKED;
	dma_channel_param.d_dma_dst_channel_burst_en = C_DMA_CHANNEL_SINGLE;
	
	dma_channel_param.d_dma_channel_hw_synch = dma_peripheral;
	dma_channel_param.d_dma_channel_priority = C_DMA_CHANNEL_PRIORITY_LOW;
	dma_channel_param.d_dma_channel_fifo_flush = C_DMA_CHANNEL_FIFO_FLUSH_OFF;
	dma_channel_param.d_dma_channel_auto_init = C_DMA_CHANNEL_SINGLE;
	
	dma_channel_param.d_dma_src_channel_addr_mode = C_DMA_CHANNEL_ADDR_MODE_POST_INC;
	dma_channel_param.d_dma_dst_channel_addr_mode = C_DMA_CHANNEL_ADDR_MODE_CONSTANT;
	
	dma_channel_param.d_dma_channel_it_time_out = C_DMA_CHANNEL_IT_TIME_OUT_ON;
	dma_channel_param.d_dma_channel_it_drop = C_DMA_CHANNEL_IT_DROP_ON;
	dma_channel_param.d_dma_channel_it_frame = C_DMA_CHANNEL_IT_FRAME_OFF;
	
	//dma_channel_param.d_dma_channel_it_block = C_DMA_NOTIFICATION;
	dma_channel_param.d_dma_channel_it_half_block = C_DMA_CHANNEL_IT_HALF_BLOCK_OFF;

	dma_channel_param.d_dma_channel_src_address = (SYS_UWORD32)buffer_p;
	dma_channel_param.d_dma_channel_dst_address = (SYS_UWORD32)&W2FC_DATA_DMA;

	dma_channel_param.d_dma_channel_src_port = C_DMA_IMIF_PORT; 
	dma_channel_param.d_dma_channel_dst_port = C_DMA_RHEA_PORT;

	dma_channel_param.d_dma_channel_element_number = current_dma_request->W2FC_DMA_Element0;
	dma_channel_param.d_dma_channel_frame_number = current_dma_request->W2FC_DMA_Frame0;

	USB_TRACE_WARNING_PARAM("USB DMA: dma_channel_param.nmb_elements", dma_channel_param.d_dma_channel_element_number);
	USB_TRACE_WARNING_PARAM("USB DMA: dma_channel_param.nmb_frames", dma_channel_param.d_dma_channel_frame_number);

	/* sets the actual parameters */
	f_dma_channel_parameter_set (&dma_channel_param);

	USB_SEND_TRACE("USB DMA: dma_set_channel_parameters : OK", RV_TRACE_LEVEL_DEBUG_LOW);

	/* enable the channel */
	f_dma_channel_enable(dma_channel);

	return (RV_OK);
}
#endif // USB_DMA_SWE_REMOVAL

/**
 *--------------------------------------------------------------------------------*
 * @name usb_start_dma_read_transfer
 *
 *
 *  @param interface_id
 *
 *  @return	BOOL 		Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */
#ifndef USB_DMA_SWE_REMOVAL
#if (CHIPSET!=15)
T_RV_RET usb_start_dma_read_transfer(T_DMG_CHANNEL dma_channel, T_DMG_PERIPHERAL dma_peripheral,
									UINT8* buffer_p, UINT16 size)
{
	T_DMG_CHANNEL_PARAMETERS	dmg_channel_param;
	T_DMG_STATUS_RSP_MSG*		msg_p;

	BOOL	dma_request_result = TRUE;

	static UINT8 mmc_afl = 1;/*0x20;*/

	/*Set DMA channel parameters*/
	// dmg_channel_param.data_width               = DMG_DATA_S16;
	dmg_channel_param.data_width               = DMG_DATA_S8;// CHB
	dmg_channel_param.sync                     = dma_peripheral;
	dmg_channel_param.hw_priority              = DMG_HW_PRIORITY_LOW;
	dmg_channel_param.repeat                   = DMG_SINGLE;
	dmg_channel_param.flush                    = DMG_FLUSH_DISABLED;

	/* Size of USB DMA FIFO */
	dmg_channel_param.nmb_elements             = mmc_afl;// CHB  = 1 ok sinon 64

	/* Number of USB transactions of 64 bytes */
	dmg_channel_param.nmb_frames               = size;// CHB W2FC_DMA_Element0 * W2FC_DMA_Frame0;// CHB  = 2 x 64 si 65

	USB_TRACE_WARNING_PARAM("USB DMA: dmg_channel_param.nmb_elements",	dmg_channel_param.nmb_elements);
	USB_TRACE_WARNING_PARAM("USB DMA: dmg_channel_param.nmb_frames",	dmg_channel_param.nmb_frames);

	dmg_channel_param.dmg_end_notification     = DMG_NOTIFICATION;
	dmg_channel_param.secure                   = DMG_NOT_SECURED;

	dmg_channel_param.source_address				= (UINT32) &W2FC_DATA_DMA;
	dmg_channel_param.source_address_mode			= DMG_ADDR_MODE_CONSTANT;
	dmg_channel_param.source_packet					= DMG_NOT_PACKED;
	dmg_channel_param.source_burst					= DMG_NO_BURST;

	dmg_channel_param.destination_address           = (UINT32)buffer_p;
	dmg_channel_param.destination_address_mode      = DMG_ADDR_MODE_POST_INC;
	dmg_channel_param.destination_packet            = DMG_NOT_PACKED;
	dmg_channel_param.destination_burst             = DMG_NO_BURST;

	dmg_channel_param.double_buf_destination_address = NULL;
	dmg_channel_param.double_buf_source_address      = NULL;

    /*Reserve DMA channel*/
    if( dmg_set_channel_parameters(dma_channel, &dmg_channel_param, usb_env_ctrl_blk_p->usb_return_path)!= RV_OK)
    {
		USB_SEND_TRACE("USB	error calling dmg_set_channel_parameters", RV_TRACE_LEVEL_ERROR);
		dma_request_result  = FALSE;
    }

	USB_SEND_TRACE("USB DMA: dmg_set_channel_parameters : OK", RV_TRACE_LEVEL_DEBUG_LOW);

}
 #else
T_RV_RET usb_start_dma_read_transfer(T_DMA_CHANNEL dma_channel, T_DMA_SYNC_DEVICE dma_peripheral,
									UINT8* buffer_p, UINT16 size)
{
	T_DMA_CHANNEL_PARAMETERS	dma_channel_param;

	BOOL	dma_request_result = TRUE;

	static UINT8 mmc_afl = 1;/*0x20;*/

	/*Set DMA channel parameters*/
	// dmg_channel_param.data_width               = DMG_DATA_S16;
	dma_channel_param.data_width               = DMA_DATA_S8;// CHB
	dma_channel_param.sync                     = dma_peripheral;
	dma_channel_param.hw_priority              = DMA_HW_PRIORITY_LOW;
	dma_channel_param.dma_mode                 = DMA_SINGLE;
	dma_channel_param.flush                    = DMA_FLUSH_DISABLED;

	/* Size of USB DMA FIFO */
	dma_channel_param.nmb_elements             = mmc_afl;// CHB  = 1 ok sinon 64

	/* Number of USB transactions of 64 bytes */
	dma_channel_param.nmb_frames               = size;// CHB W2FC_DMA_Element0 * W2FC_DMA_Frame0;// CHB  = 2 x 64 si 65

	USB_TRACE_WARNING_PARAM("USB DMA: dma_channel_param.nmb_elements",	dma_channel_param.nmb_elements);
	USB_TRACE_WARNING_PARAM("USB DMA: dma_channel_param.nmb_frames",	dma_channel_param.nmb_frames);

	dma_channel_param.dma_end_notification     = DMA_NOTIFICATION;
	dma_channel_param.secure                   = DMA_NOT_SECURED;

	dma_channel_param.source_address				= (UINT32) &W2FC_DATA_DMA;
	dma_channel_param.source_address_mode			= DMA_ADDR_MODE_CONSTANT;
	dma_channel_param.source_packet					= DMA_NOT_PACKED;
	dma_channel_param.source_burst					= DMA_NO_BURST;

	dma_channel_param.destination_address           = (UINT32)buffer_p;
	dma_channel_param.destination_address_mode      = DMA_ADDR_MODE_POST_INC;
	dma_channel_param.destination_packet            = DMA_NOT_PACKED;
	dma_channel_param.destination_burst             = DMA_NO_BURST;

	dma_channel_param.transfer = DMA_MODE_TRANSFER_ENABLE;

    /*Reserve DMA channel*/
    if( dma_set_channel_parameters(dma_channel,&dma_channel_param ) != DMA_OK )
    {
		USB_SEND_TRACE("USB	error calling dma_set_channel_parameters", RV_TRACE_LEVEL_ERROR);
		dma_request_result  = FALSE;
    }

	USB_SEND_TRACE("USB DMA: dma_set_channel_parameters : OK", RV_TRACE_LEVEL_DEBUG_LOW);

	return (RV_OK);
}
#endif //(CHIPSET!=15)
#else
T_RV_RET usb_start_dma_read_transfer(T_DMA_TYPE_CHANNEL_NUMBER dma_channel, T_DMA_TYPE_CHANNEL_HW_SYNCH dma_peripheral,
				     UINT8* buffer_p, UINT16 size)
{
	T_DMA_TYPE_CHANNEL_PARAMETER dma_channel_param;

	//BOOL	dma_request_result = TRUE;

	static UINT8 mmc_afl = 1;/*0x20;*/

	USB_SEND_TRACE("USB DMA: usb_start_dma_read_transfer", RV_TRACE_LEVEL_DEBUG_LOW); 
	
	/*Set DMA channel parameters*/
	dma_channel_param.pf_dma_call_back_address = pf_dma_call_back_address[dma_channel];
	dma_channel_param.d_dma_channel_number = dma_channel;
	
	dma_channel_param.d_dma_channel_secured = C_DMA_CHANNEL_NOT_SECURED;
	dma_channel_param.d_dma_channel_data_type = C_DMA_DATA_S8; 
		
	dma_channel_param.d_dma_src_channel_packed = C_DMA_CHANNEL_NOT_PACKED;
	dma_channel_param.d_dma_src_channel_burst_en = C_DMA_CHANNEL_SINGLE; 
	dma_channel_param.d_dma_dst_channel_packed = C_DMA_CHANNEL_NOT_PACKED;
	dma_channel_param.d_dma_dst_channel_burst_en = C_DMA_CHANNEL_SINGLE;
	
	dma_channel_param.d_dma_channel_hw_synch = dma_peripheral;
	dma_channel_param.d_dma_channel_priority = C_DMA_CHANNEL_PRIORITY_LOW;
	dma_channel_param.d_dma_channel_fifo_flush = C_DMA_CHANNEL_FIFO_FLUSH_OFF;
	dma_channel_param.d_dma_channel_auto_init = C_DMA_CHANNEL_SINGLE;
	
	dma_channel_param.d_dma_src_channel_addr_mode = C_DMA_CHANNEL_ADDR_MODE_CONSTANT;
	dma_channel_param.d_dma_dst_channel_addr_mode = C_DMA_CHANNEL_ADDR_MODE_POST_INC;
	
	dma_channel_param.d_dma_channel_it_time_out = C_DMA_CHANNEL_IT_TIME_OUT_ON;
	dma_channel_param.d_dma_channel_it_drop = C_DMA_CHANNEL_IT_DROP_ON;
	dma_channel_param.d_dma_channel_it_frame = C_DMA_CHANNEL_IT_FRAME_OFF;
	
	//dma_channel_param.d_dma_channel_it_block = C_DMA_NOTIFICATION;
	dma_channel_param.d_dma_channel_it_half_block = C_DMA_CHANNEL_IT_HALF_BLOCK_OFF;

	dma_channel_param.d_dma_channel_src_address = (SYS_UWORD32)&W2FC_DATA_DMA;
	dma_channel_param.d_dma_channel_dst_address = (SYS_UWORD32)buffer_p;

	dma_channel_param.d_dma_channel_src_port = C_DMA_RHEA_PORT; 
	dma_channel_param.d_dma_channel_dst_port = C_DMA_IMIF_PORT;

	dma_channel_param.d_dma_channel_element_number = mmc_afl;
	dma_channel_param.d_dma_channel_frame_number = size;

	USB_TRACE_WARNING_PARAM("USB DMA: dma_channel_param.nmb_elements", dma_channel_param.d_dma_channel_element_number);
	USB_TRACE_WARNING_PARAM("USB DMA: dma_channel_param.nmb_frames", dma_channel_param.d_dma_channel_frame_number);

	/* sets the actual parameters */
	f_dma_channel_parameter_set (&dma_channel_param);
	
	USB_SEND_TRACE("USB DMA: dma_set_channel_parameters : OK", RV_TRACE_LEVEL_DEBUG_LOW);
	
	/* enable the channel */
	f_dma_channel_enable(dma_channel);

	return (RV_OK);

}
#endif // USB_DMA_SWE_REMOVAL

/**
 *---------------------------------------------------------------------------------*
 * @name usb_non_iso_tx_dma_start.
 *
 *
 *
 * @param	msg_id	.
 *
 * @return	none.
 *---------------------------------------------------------------------------------*
 */
#ifndef USB_DMA_SWE_REMOVAL
void usb_non_iso_tx_dma_start( UINT8 if_nr, UINT8 log_ep_nr, UINT8 dma_tx_channel, UINT16 size,
#if (CHIPSET==15)
							T_DMA_CHANNEL	dma_channel, T_DMA_SYNC_DEVICE dma_peripheral, UINT8* buffer_p )
#else
							T_DMG_CHANNEL	dma_channel, T_DMG_PERIPHERAL dma_peripheral, UINT8* buffer_p )
#endif //(CHIPSET==15)
#else
void usb_non_iso_tx_dma_start( UINT8 if_nr, UINT8 log_ep_nr, UINT8 dma_tx_channel, UINT16 size,
			T_DMA_TYPE_CHANNEL_NUMBER dma_channel, T_DMA_TYPE_CHANNEL_HW_SYNCH dma_peripheral, UINT8* buffer_p)
#endif // USB_DMA_SWE_REMOVAL
{
	UINT8	phys_ep_idx	= 0;
	UINT8	phys_ep_nr	= 0;

	UINT16	bitmask_phys_ep_nr	= 0;
	UINT16	bitmask_reset		= 0x0F;

	UINT16	TXDMAN = 0;

	unsigned int	temp;
	unsigned int	writeSize;
	unsigned char	i;

	T_USB_DMA_REQUEST_INFORMATION*	current_dma_request = NULL;

	USB_SEND_TRACE("USB-usb_non_iso_tx_dma_start ", RV_TRACE_LEVEL_DEBUG_LOW);

	current_dma_request = usb_get_dma_request_information(if_nr, log_ep_nr);

	if (current_dma_request == NULL)
	{
	USB_SEND_TRACE("USB-TX current_dma_request NULL ", RV_TRACE_LEVEL_DEBUG_LOW);
	}

	if(current_dma_request == NULL)
	{
		USB_SEND_TRACE("USB-usb_non_iso_tx_dma_start error current_dma_request == NULL", RV_TRACE_LEVEL_ERROR);

		return;
	}

	phys_ep_idx = LEP_2_PEP(log_ep_nr, if_nr, USB_TX);
	phys_ep_nr	= phys_ep_idx + 1;

	// EP number-->TXDMA_CFG.TXDMAn_EP //
	bitmask_reset = bitmask_reset	<< (4 * dma_tx_channel);
	W2FC_TXDMA_CFG = W2FC_TXDMA_CFG & ~bitmask_reset;
	bitmask_phys_ep_nr = phys_ep_nr << (4 * dma_tx_channel);
	W2FC_TXDMA_CFG = W2FC_TXDMA_CFG | bitmask_phys_ep_nr;

	USB_TRACE_WARNING_PARAM("USB-DMA W2FC_TXDMA_CFG",	W2FC_TXDMA_CFG);

	USB_TRACE_WARNING_PARAM("USB-usb_non_iso_tx_dma_start size",	size);

	// FTZ > 1024 Bytes? -> No Check //
	if( (size < 1024) && ((size&(64-1)) != 0)) {// CHB

		// Set XSWLn = 0 //
		current_dma_request->W2FC_TXDMA_XSWL0 = 0;

		// Start single pass DMA transfert of FTZ bytes //
		//- Set TXDMAn Parameter -//
		TXDMAN = size;
		TXDMAN |= W2FC_TXDMAn_TXn_EOT;

		//- Set Frame & Element for DMA Transfer -//
		current_dma_request->W2FC_DMA_Frame0 = size >> 6;
		if (size & (64 - 1))
		{
			current_dma_request->W2FC_DMA_Frame0++;
		}

		current_dma_request->W2FC_DMA_Element0 = 64;

		//- Set W2FC_TXDMA_fEOT=1 -//
		current_dma_request->W2FC_TXDMA_fEOT0 = 1;
	}
	else {

		//- Preparation for TX DMA Transfer -//
		current_dma_request->W2FC_DMA_Frame0 = W2FC_TXDMAn_TXn_TSC + 1;// 1024
		current_dma_request->W2FC_DMA_Element0 = 64;

		// EOTBn = FTZ & (EPsize-1) //
		// temp  = FTZ >> EPsize    //
		// XSWLn = temp >> 10bit    //
		// FBTn  = temp & 0x3FF     //
		current_dma_request->W2FC_TXDMA_EOTB0 = size & (64 - 1);// 0
		temp = size >> 6;// 2
		current_dma_request->W2FC_TXDMA_XSWL0 = temp / current_dma_request->W2FC_DMA_Frame0;// 0
		current_dma_request->W2FC_TXDMA_FBT0  = temp & W2FC_TXDMAn_TXn_TSC;// 2

		//- Reset W2FC_TXDMA_fEOT -//
		current_dma_request->W2FC_TXDMA_fEOT0 = 0;

		// XSWLn=0? //
		if( current_dma_request->W2FC_TXDMA_XSWL0 ) {
			// XSWLn = XSWLn - 1 (used by interrupt handler for next pass) //
			current_dma_request->W2FC_TXDMA_XSWL0 -= 1;

			//- Set TXDMAn Parameter -//
			TXDMAN = current_dma_request->W2FC_DMA_Frame0;
		}
		else {
			if( current_dma_request->W2FC_TXDMA_FBT0 ) {
				//- Set Frame for DMA Transfer -//
				current_dma_request->W2FC_DMA_Frame0 = current_dma_request->W2FC_TXDMA_FBT0;// 2

				//- Set TXDMAn Parameter -//
				TXDMAN = current_dma_request->W2FC_DMA_Frame0;// 2
				//TXDMAN = W2FC_DMA_Frame0 - 1;// 1 CHB

				//- FBTn = 0 -//
				current_dma_request->W2FC_TXDMA_FBT0 = 0;// 0
			}
			else {
				//- Set Frame & Element for DMA Transfer -//
				current_dma_request->W2FC_DMA_Frame0   = 1;
				current_dma_request->W2FC_DMA_Element0 = current_dma_request->W2FC_TXDMA_EOTB0;

				//- Set TXDMAn Parameter -//
				TXDMAN = current_dma_request->W2FC_DMA_Element0 | W2FC_TXDMAn_TXn_EOT;

				//- Set W2FC_TXDMA_fEOT=1 -//
				current_dma_request->W2FC_TXDMA_fEOT0 = 1;
			}
		}
	}

	//- Set TXDMAn Parameter -//
	TXDMAN |= W2FC_TXDMAn_TXn_START;

	// Application specific action to initialize the main system DMA controller //
	// CHB size = ... frame x element
	current_dma_request->CHB_buffer_p = buffer_p;

	usb_start_write_dma_transfer(dma_channel, dma_peripheral, buffer_p, size, current_dma_request);

	/*
		Start DMA tranfer
		Fill DMA_IRQ_EN register with appropriate value
	*/
	switch(dma_tx_channel)
	{
		case 0 :
			W2FC_TXDMA0      = TXDMAN;
			W2FC_DMA_IRQ_EN |= W2FC_DMA_IRQ_EN_TX0_DONE_IE;
			USB_TRACE_WARNING_PARAM("USB DMA: W2FC_TXDMA0", W2FC_TXDMA0);
		break;

		case 1 :
			W2FC_TXDMA1      = TXDMAN;
			W2FC_DMA_IRQ_EN |= W2FC_DMA_IRQ_EN_TX1_DONE_IE;
			USB_TRACE_WARNING_PARAM("USB DMA: W2FC_TXDMA1", W2FC_TXDMA1);
		break;

		case 2 :
			W2FC_TXDMA2      = TXDMAN;
			W2FC_DMA_IRQ_EN |= W2FC_DMA_IRQ_EN_TX2_DONE_IE;
			USB_TRACE_WARNING_PARAM("USB DMA: W2FC_TXDMA2", W2FC_TXDMA2);
		break;
	}

	USB_TRACE_WARNING_PARAM("USB DMA: W2FC_DMA_IRQ_EN", W2FC_DMA_IRQ_EN);
}

T_RV_RET send_deferred_dma_write(UINT32 phys_ep_nr)
{
	T_USB_TX_EP_INTERRUPT_MSG	*msg_p;

	if(create_usb_msg(	sizeof(T_USB_TX_EP_INTERRUPT_MSG), 
						USB_DMA_TX_EP_INTERRUPT_MSG, \
		(T_RVF_MSG **)&msg_p) == USB_OK)
	{
		msg_p->endpoint = phys_ep_nr;
	}
	else
	{
		/*ERROR the memory needed for the buffer is not reserved*/
		USB_SEND_TRACE("USB API: Error to get memory for USB_FM_SUBSCRIBE_MSG ",RV_TRACE_LEVEL_ERROR);
		return RV_MEMORY_ERR;
	}
	USB_SEND_TRACE("USB: send_deferred_write", RV_TRACE_LEVEL_DEBUG_LOW);

	/* Send the message using mailbox. */
	return rvf_send_msg(usb_env_ctrl_blk_p->addr_id,
						(void*)msg_p);
}

/**
 *---------------------------------------------------------------------------------*
 * @name usb_non_iso_tx_dma_done_handler.
 *
 *
 *
 * @param	msg_id	Waited message ID, null for any message.
 *
 * @return	The received message.
 *---------------------------------------------------------------------------------*
 */
void usb_non_iso_tx_dma_done_handler( void )
{
	UINT8	endp_nb;
	UINT8	DMA_nb 	= 0;// 0 = a generaliser
	UINT8	DMA_ch 	= 4;// 4 = a generaliser
	UINT8	DMA_per = 0;// 0 = a generaliser

	unsigned char	i;
	unsigned int	temp;
	unsigned int	writeSize;
	unsigned short	TXDMAN;
	unsigned int	INT;

	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;
	T_RV_RET ret = RV_OK;

	T_USB_DMA_REQUEST_INFORMATION*	current_dma_request = NULL;

	UINT16	bitmask_reset		= 0x0F;
	UINT16	bitmask_result		= 0x0F;

	USB_SEND_TRACE("USB-usb_non_iso_tx_dma_done_handler ", RV_TRACE_LEVEL_DEBUG_LOW);

	TXDMAN = W2FC_TXDMA0;

	// Read the endpoint number n in DMAN_STAT.DMAn_TX_IT_src register //
	endp_nb = (W2FC_DMAN_STAT & W2FC_DMAN_STAT_DMAN_TX_IT_SRC);

	if(usb_get_dma_information(TRUE, endp_nb, &DMA_ch, &DMA_nb) == RV_INTERNAL_ERR)
	{
		USB_SEND_TRACE("USB-usb_non_iso_tx_dma_done_handler ", RV_TRACE_LEVEL_ERROR);

		return;
	}

	DMA_per	= usb_dma_tx_peripheral_tbl[DMA_nb];

	current_dma_request  = usb_get_dma_request_information_from_phys_endpoint(endp_nb);

	// Set IRQ_SRC.TXn_Done=1 to clear the IT //
	W2FC_IRQ_SRC_CLR_INT(W2FC_IRQ_SRC_TXN_DONE);

	// EOTn=1? //
	if( current_dma_request ->W2FC_TXDMA_fEOT0 ) {
		/*
			DMA Transaction is done
			release DMA channel
		*/
		if(TRUE == USB_DMA_CFG.dma_usb_endpoint_tx_tbl[DMA_nb].channel_used)
		{
			ret = unsubscribe_dma_usb_channel(DMA_ch);
			if(RV_OK == ret)
				USB_DMA_CFG.dma_usb_endpoint_tx_tbl[DMA_nb].channel_used = FALSE;
		}

		// Inform the application that the TX DMA transfer is completed. //
		notify_tx_buffer_empty(USB_PEP_INDX(endp_nb));

		bitmask_reset = bitmask_reset << (4 * DMA_nb);
		bitmask_result = W2FC_TXDMA_CFG & ~bitmask_reset;
		W2FC_TXDMA_CFG = bitmask_result;
		USB_TRACE_WARNING_PARAM("USB DMA: W2FC_TXDMA_CFG", W2FC_TXDMA_CFG);
		return;
	}
	else
	{
		endpoint_vs_interface_entry_p =
			usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl + USB_PEP_INDX(endp_nb);
		
		/*	If last transaction is an empty packet, 
			check user has defined it as a short trasnfer */ 
		if((current_dma_request->W2FC_TXDMA_XSWL0		== 0)
			&& (current_dma_request->W2FC_TXDMA_FBT0	== 0)
			&& (current_dma_request->W2FC_TXDMA_EOTB0	== 0)
			&& (endpoint_vs_interface_entry_p->shorter_transfer == FALSE))
		{
			USB_SEND_TRACE("USBDMA : Do not send an empty packet to finish transfer", RV_TRACE_LEVEL_DEBUG_LOW);
			/*
				DMA Transaction is done
				release DMA channel
			*/
			if(TRUE == USB_DMA_CFG.dma_usb_endpoint_tx_tbl[DMA_nb].channel_used)
			{
				ret = unsubscribe_dma_usb_channel(DMA_ch);
				if(RV_OK == ret)
					USB_DMA_CFG.dma_usb_endpoint_tx_tbl[DMA_nb].channel_used = FALSE;
			}
			

			// Inform the application that the TX DMA transfer is completed. //
			notify_tx_buffer_empty(USB_PEP_INDX(endp_nb));

			bitmask_reset = bitmask_reset << (4 * DMA_nb);
			bitmask_result = W2FC_TXDMA_CFG & ~bitmask_reset;
			W2FC_TXDMA_CFG = bitmask_result;
			USB_TRACE_WARNING_PARAM("USB DMA: W2FC_TXDMA_CFG", W2FC_TXDMA_CFG);
		}
		else
		{
			send_deferred_dma_write(endp_nb);
		}
	}
}


/**
 *---------------------------------------------------------------------------------*
 * @name usb_deffered_non_iso_tx_dma_done_handler.
 *
 *
 *
 * @param	msg_id	Waited message ID, null for any message.
 *
 * @return	The received message.
 *---------------------------------------------------------------------------------*
 */
void usb_deffered_non_iso_tx_dma_done_handler(UINT8 endp_nb)
{
	UINT8	DMA_nb 	= 0;// 0 = a generaliser
	UINT8	DMA_ch 	= 4;// 4 = a generaliser
	UINT8	DMA_per = 0;// 0 = a generaliser

	unsigned char	i;
	unsigned int	temp;
	unsigned int	writeSize;
	unsigned short	TXDMAN;
	unsigned int	INT;

	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;

	T_USB_DMA_REQUEST_INFORMATION*	current_dma_request = NULL;

	UINT16	bitmask_reset		= 0x0F;
	UINT16	bitmask_result		= 0x0F;

	USB_SEND_TRACE("USB-usb_non_iso_tx_dma_done_handler ", RV_TRACE_LEVEL_DEBUG_LOW);

	if(usb_get_dma_information(TRUE, endp_nb, &DMA_ch, &DMA_nb) == RV_INTERNAL_ERR)
	{
		USB_SEND_TRACE("USB-usb_non_iso_tx_dma_done_handler ", RV_TRACE_LEVEL_ERROR);

		return;
	}

	DMA_per	= usb_dma_tx_peripheral_tbl[DMA_nb];

	current_dma_request  = usb_get_dma_request_information_from_phys_endpoint(endp_nb);

	// EOTn=1? //
	if( current_dma_request ->W2FC_TXDMA_fEOT0 ) {
		return;
	}

	//- Updata Next Pointer -//
	writeSize = current_dma_request ->W2FC_DMA_Frame0 * current_dma_request ->W2FC_DMA_Element0;

	endpoint_vs_interface_entry_p =
		usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl + USB_PEP_INDX(endp_nb);

	endpoint_vs_interface_entry_p->temp_buf_p += writeSize;

	// XSWLn=0? //
	if( current_dma_request ->W2FC_TXDMA_XSWL0 ) {
		// XSWLn = XSWLn - 1 //
		current_dma_request ->W2FC_TXDMA_XSWL0 -= 1;

		//- Set TXDMAn Parameter -//
		TXDMAN = current_dma_request ->W2FC_DMA_Frame0;// normalement = 1
	}
	else {
		if( current_dma_request->W2FC_TXDMA_FBT0 ) {
			//- Set Frame for DMA Transfer -//
			current_dma_request->W2FC_DMA_Frame0 = current_dma_request->W2FC_TXDMA_FBT0;

			//- Set TXDMAn Parameter -//
			TXDMAN = current_dma_request ->W2FC_DMA_Frame0;

			//- FBTn = 0 -//
			current_dma_request ->W2FC_TXDMA_FBT0 = 0;
		}
		else {
			//- Set Frame & Element for DMA Transfer -//
			current_dma_request->W2FC_DMA_Frame0   = 1;// 1
			current_dma_request->W2FC_DMA_Element0 = current_dma_request->W2FC_TXDMA_EOTB0;// 0

			//- Set TXDMAn Parameter -//
			TXDMAN = current_dma_request->W2FC_DMA_Element0 | W2FC_TXDMAn_TXn_EOT;

			//- Set W2FC_TXDMA_fEOT=1 -//
			current_dma_request->W2FC_TXDMA_fEOT0 = 1;
		}
	}

	//- Set TXDMAn Parameter -//
	TXDMAN |= W2FC_TXDMAn_TXn_START;

	//- Updata Next Pointer -//
	writeSize = current_dma_request->W2FC_DMA_Frame0 * current_dma_request->W2FC_DMA_Element0;

	usb_start_write_dma_transfer(	DMA_ch,
					DMA_per,
					endpoint_vs_interface_entry_p->temp_buf_p,
					writeSize,
					current_dma_request);

	// Start DMA tranfer //
	switch( DMA_nb ) {
	case 0:
		W2FC_TXDMA0 = TXDMAN;
		W2FC_DMA_IRQ_EN |= W2FC_DMA_IRQ_EN_TX0_DONE_IE;

		USB_TRACE_WARNING_PARAM("USB DMA: W2FC_TXDMA0", W2FC_TXDMA0);
		break;
	case 1:
		W2FC_TXDMA1 = TXDMAN;
		W2FC_DMA_IRQ_EN |= W2FC_DMA_IRQ_EN_TX1_DONE_IE;

		USB_TRACE_WARNING_PARAM("USB DMA: W2FC_TXDMA1", W2FC_TXDMA1);
		break;
	case 2:
		W2FC_TXDMA2 = TXDMAN;
		W2FC_DMA_IRQ_EN |= W2FC_DMA_IRQ_EN_TX2_DONE_IE;
		USB_TRACE_WARNING_PARAM("USB DMA: W2FC_TXDMA2", W2FC_TXDMA2);
		break;
	default:
		break;
	}
}


/**
 *--------------------------------------------------------------------------------*
 * @name usb_prepare_dma_tx_transfer
 *
 *
 *  @param interface_id
 *
 *  @return	BOOL 		Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */
T_RV_RET usb_prepare_dma_tx_transfer(UINT8 interface_id, UINT8 endpoint_id)
{
#ifndef USB_DMA_SWE_REMOVAL
#if (CHIPSET==15)
	T_DMA_CHANNEL		dma_channel		= 0;
	T_DMA_SYNC_DEVICE   dma_peripheral	= 0;
#else
	T_DMG_CHANNEL		dma_channel		= 0;
	T_DMG_PERIPHERAL	dma_peripheral	= 0;
#endif //(CHIPSET==15)
#else	
	T_DMA_TYPE_CHANNEL_NUMBER	dma_channel = 0;
	T_DMA_TYPE_CHANNEL_HW_SYNCH	dma_peripheral = 0;
#endif // USB_DMA_SWE_REMOVAL

	UINT8*	buffer_p	= NULL;
	UINT16	size		= 0;

	UINT8			i = 0;
	BOOL			found = FALSE;

	UINT8			usb_dma_channel = 0;

	/* Get index of physical endpoint from logical endpoint */
	UINT8 phys_ep_idx = LEP_2_PEP(endpoint_id, interface_id, USB_TX);

	T_RV_RET	ret = RV_INVALID_PARAMETER;

	USB_SEND_TRACE("USB-usb_prepare_dma_tx_transfer ", RV_TRACE_LEVEL_DEBUG_LOW);

	buffer_p	= usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl[phys_ep_idx].buf_p;
	size		= usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl[phys_ep_idx].buf_size;

	for(i = 0; i < DMA_TX_CHANNELS; i++)
	{
		if( (USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].interface_id == interface_id)
			&& (USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].endpoint_id == endpoint_id) )
		{
			dma_channel		= usb_dma_tx_channel_tbl[i];
			dma_peripheral	= usb_dma_tx_peripheral_tbl[i];
			usb_dma_channel	= i;

			found = TRUE;
		}
	}

	if(found == TRUE)
	{
		usb_dma_lock();
		/* New management */
		USB_DMA_CFG.dma_usb_endpoint_tx_tbl[usb_dma_channel].subscription_order = 1;
//		USB_DMA_CFG.dma_usb_endpoint_tx_tbl[usb_dma_channel].dmg_channel =
		usb_subscribe_dma_channel(dma_channel, dma_peripheral);
//		USB_DMA_CFG.dma_usb_endpoint_tx_tbl[usb_dma_channel].channel_used = TRUE;

		ret = RV_OK;
	}
	else
	{
		USB_SEND_TRACE("USB DMA : DMA configuration not found", RV_TRACE_LEVEL_ERROR);
		ret = RV_INTERNAL_ERR;
	}

	return(ret);
}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name usb_process_tx_dma_request
 *
 *
 *  @param
 *
 *  @return	T_RV_RET	Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */
void	usb_process_tx_dma_request(UINT8 usb_dma_channel, UINT8 dma_channel)
{
	/* Get logical configuration interface and logical endoint */
	UINT8	if_nr		= USB_DMA_CFG.dma_usb_endpoint_tx_tbl[usb_dma_channel].interface_id;
	UINT8	log_ep__nr	= USB_DMA_CFG.dma_usb_endpoint_tx_tbl[usb_dma_channel].endpoint_id;

	UINT8	dma_peripheral	= usb_dma_tx_peripheral_tbl[usb_dma_channel];

	/* Get index of physical endpoint from logical endpoint */
	UINT8 phys_ep_idx 	= LEP_2_PEP(log_ep__nr, if_nr, USB_TX);

	/* Get reference to buffer to send */
	UINT8*	buffer_p	= usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl[phys_ep_idx].buf_p;

	/* Get size of buffer to send */
	UINT16	size		= usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl[phys_ep_idx].buf_size;

	/* Fill Dma structure */
	USB_DMA_CFG.dma_usb_endpoint_tx_tbl[usb_dma_channel].dmg_channel = dma_channel;
	USB_DMA_CFG.dma_usb_endpoint_tx_tbl[usb_dma_channel].channel_used = TRUE;

	F_INTH_DISABLE_ONE_IT(C_INTH_USB_IT);
	/* Start DMA transfer */
	usb_non_iso_tx_dma_start(	if_nr, log_ep__nr, usb_dma_channel, size,
								dma_channel, dma_peripheral, buffer_p );
	F_INTH_ENABLE_ONE_IT(C_INTH_USB_IT);
}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name usb_non_iso_rx_dma_start
 *
 *
 *  @param interface_id
 *
 *  @return	BOOL 		Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */
T_RV_RET usb_non_iso_rx_dma_start(UINT8 if_nr, UINT8 log_ep_nr, UINT8 dma_rx_channel, UINT16 size)
{
	UINT8	phys_ep_idx	= 0;
	UINT8	phys_ep_nr	= 0;

	UINT16	bitmask_phys_ep_nr	= 0;
	UINT16	bitmask_reset		= 0x0F;

	UINT16	RXDMAN = 0;
	UINT16 _size_to_get,_sav_size_to_get;
	char _stri[10];
	UINT8* buffer_p;

	T_USB_DMA_REQUEST_INFORMATION*	current_dma_request = NULL;

	current_dma_request = usb_get_dma_request_information(if_nr, log_ep_nr);

	if(current_dma_request == NULL)
	{
		return RV_INTERNAL_ERR;
	}

	/* Get index of physical endpoint from logical endpoint */
	phys_ep_idx = LEP_2_PEP(log_ep_nr, if_nr, USB_RX);

	/* Get physical endpoint number */
	phys_ep_nr = phys_ep_idx + 1;

	/* Assign non-ISO endpoint number to DMA channel n */
	bitmask_reset = bitmask_reset << (4 * dma_rx_channel);
	W2FC_RXDMA_CFG = W2FC_RXDMA_CFG & ~bitmask_reset;

	bitmask_phys_ep_nr = phys_ep_nr << (4 * dma_rx_channel);
	W2FC_RXDMA_CFG = W2FC_RXDMA_CFG | bitmask_phys_ep_nr;

	USB_TRACE_WARNING_PARAM("USB DMA: W2FC_RXDMA_CFG", W2FC_RXDMA_CFG);

	current_dma_request->W2FC_DMA_Frame0 = size / 64;// 1024 / 64 = 16
	// current_dma_request->W2FC_DMA_Frame0 = 0x200;
	// current_dma_request->W2FC_DMA_Element0 = 64;

	// Set max transactions count into RXDMAn.RXn_TC //
	// Fill DMA_IRQ_EN register with appropriate value //
	RXDMAN = (current_dma_request->W2FC_DMA_Frame0 - 1) | W2FC_RXDMAn_RXn_STOP;
	// RXDMAN = current_dma_request->W2FC_DMA_Frame0 - 1;

	/* Fill DMA Interrupt Enable Register with appropriate value */
	switch(dma_rx_channel)
	{
		case 0 :
			W2FC_RXDMA0 = RXDMAN;
			USB_TRACE_WARNING_PARAM("USB DMA: W2FC_RXDMA0", RXDMAN);

			W2FC_DMA_IRQ_EN &= ~W2FC_DMA_IRQ_EN_RX0_CNT_IE;
			W2FC_DMA_IRQ_EN |= W2FC_DMA_IRQ_EN_RX0_EOT_IE;
		break;

		case 1 :
			W2FC_RXDMA1 = RXDMAN;
			USB_TRACE_WARNING_PARAM("USB DMA: W2FC_RXDMA1", RXDMAN);

			W2FC_DMA_IRQ_EN &= ~W2FC_DMA_IRQ_EN_RX1_CNT_IE;
			W2FC_DMA_IRQ_EN |= W2FC_DMA_IRQ_EN_RX1_EOT_IE;
		break;

		case 2 :
			W2FC_RXDMA2 = RXDMAN;
			USB_TRACE_WARNING_PARAM("USB DMA: W2FC_RXDMA2", RXDMAN);

			W2FC_DMA_IRQ_EN &= ~W2FC_DMA_IRQ_EN_RX2_CNT_IE;
			W2FC_DMA_IRQ_EN |= W2FC_DMA_IRQ_EN_RX2_EOT_IE;
		break;

		default:
		break;
	}

	USB_TRACE_WARNING_PARAM("USB DMA: W2FC_DMA_IRQ_EN", W2FC_DMA_IRQ_EN);

	// Set EP_NUM.EP_Num=endp then set CTRL.Set_FIFO_En to enable DMA transfer //

	USB_EP_NUM_SEL_RX_EP(phys_ep_nr);
	// Write CTRL register: CTRL.Set_FIFO_En=1 //
	W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
	USB_EP_NUM_RX_RELEASE(phys_ep_nr);

//	(void) NU_Control_Timer (&(USB_DMA_CFG.dma_usb_rx_timer[dma_rx_channel]), NU_ENABLE_TIMER);	

	USB_TRACE_WARNING_PARAM("USB DMA: W2FC_CTRL", W2FC_CTRL);

	return (RV_OK);
}

/**
 *--------------------------------------------------------------------------------*
 * @name usb_prepare_dma_rx_transfer
 *
 *
 *  @param interface_id
 *
 *  @return	BOOL 		Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */
T_RV_RET usb_prepare_dma_rx_transfer(UINT8 interface_id, UINT8 endpoint_id)
{
#ifndef USB_DMA_SWE_REMOVAL
#if (CHIPSET==15)
	T_DMA_CHANNEL		dma_channel		= 0;
	T_DMA_SYNC_DEVICE 	dma_peripheral	= 0;
#else
	T_DMG_CHANNEL		dma_channel		= 0;
	T_DMG_PERIPHERAL	dma_peripheral	= 0;
#endif //(CHIPSET==15)
#else	
	T_DMA_TYPE_CHANNEL_NUMBER	dma_channel	= 0;
	T_DMA_TYPE_CHANNEL_HW_SYNCH	dma_peripheral	= 0;
#endif // USB_DMA_SWE_REMOVAL

	UINT8				usb_dma_channel = 0;

	UINT8			i = 0;
	BOOL			found = FALSE;

	T_RV_RET	ret = RV_INVALID_PARAMETER;

	/* Get index of physical endpoint from logical endpoint */
	UINT8 phys_ep_idx = LEP_2_PEP(endpoint_id, interface_id, USB_RX);

	for(i = 0; i < DMA_RX_CHANNELS; i++)
	{
		if( (USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].interface_id == interface_id)
			&& (USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].endpoint_id == endpoint_id) )
		{
			dma_channel		= usb_dma_rx_channel_tbl[i];
			dma_peripheral	= usb_dma_rx_peripheral_tbl[i];
			usb_dma_channel = i;

			found = TRUE;
		}
	}

	if(found == TRUE)
	{
		usb_dma_lock();
		/* New management */
		USB_DMA_CFG.dma_usb_endpoint_rx_tbl[usb_dma_channel].subscription_order	= 1;
//		USB_DMA_CFG.dma_usb_endpoint_rx_tbl[usb_dma_channel].dmg_channel =
		usb_subscribe_dma_channel(dma_channel, dma_peripheral);
//		USB_DMA_CFG.dma_usb_endpoint_rx_tbl[usb_dma_channel].channel_used = TRUE;

		ret = RV_OK;
	}
	else
	{
		USB_SEND_TRACE("USB DMA : DMA configuration not found", RV_TRACE_LEVEL_ERROR);
		ret = RV_INTERNAL_ERR;
	}

	return(ret);
}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name usb_process_rx_dma_request
 *
 *
 *  @param
 *
 *  @return	T_RV_RET	Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */
void	usb_process_rx_dma_request(UINT8 usb_dma_channel, UINT8 dma_channel)
{
	/* Get logical configuration interface and logical endoint */
	UINT8	if_nr			= USB_DMA_CFG.dma_usb_endpoint_rx_tbl[usb_dma_channel].interface_id;
	UINT8	log_ep_nr		= USB_DMA_CFG.dma_usb_endpoint_rx_tbl[usb_dma_channel].endpoint_id;

	UINT8	dma_peripheral	= usb_dma_rx_peripheral_tbl[usb_dma_channel];

	/* Get index of physical endpoint from logical endpoint */
	UINT8 phys_ep_idx 	= LEP_2_PEP(log_ep_nr, if_nr, USB_RX);

	UINT16	size = 0;

	/* Store used DMA channel */
	USB_DMA_CFG.dma_usb_endpoint_rx_tbl[usb_dma_channel].dmg_channel = dma_channel;
	USB_DMA_CFG.dma_usb_endpoint_rx_tbl[usb_dma_channel].channel_used = TRUE;
	
	f_dma_channel_soft_reset(dma_channel);

//Suthar:	(void) NU_Control_Timer (&(USB_DMA_CFG.dma_usb_rx_timer[usb_dma_channel]), NU_ENABLE_TIMER);

	USB_DMA_CFG.dma_usb_rx_memo_size[usb_dma_channel] = 0;
	/* Configure W2FC controler to DMA transfer for a specific endpoint */

	size = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[phys_ep_idx].buf_size;

	/*
	 * Application specific action to initialize
	 * the main system DMA controller
	 */
	usb_start_dma_read_transfer(dma_channel, dma_peripheral,
				usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[phys_ep_idx].temp_buf_p,
				size);


	F_INTH_DISABLE_ONE_IT(C_INTH_USB_IT);
	usb_non_iso_rx_dma_start(if_nr, log_ep_nr, usb_dma_channel,
									usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[phys_ep_idx].buf_size);
	F_INTH_ENABLE_ONE_IT(C_INTH_USB_IT);

}

/**
 *--------------------------------------------------------------------------------*
 * @name usb_process_rx_request
 *
 *
 *  @param
 *
 *  @return	T_RV_RET	Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */
void	usb_process_rx_request(UINT8 usb_dma_channel, UINT8 dma_channel)
{
	UINT8 phys_ep_idx = 255;		//physical ep nr 255 does NOT exist
	UINT8 pep_nr;
	UINT16 remvove_stall;
	T_RV_RET ret = RV_OK;

	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;

	UINT8	if_nr			= USB_DMA_CFG.dma_usb_endpoint_rx_tbl[usb_dma_channel].interface_id;
	UINT8	log_ep_nr		= USB_DMA_CFG.dma_usb_endpoint_rx_tbl[usb_dma_channel].endpoint_id;

	phys_ep_idx = LEP_2_PEP(log_ep_nr, if_nr, USB_RX);
	pep_nr = phys_ep_idx + 1;

	//find physical ep
	endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl +
		(LEP_2_PEP(log_ep_nr, if_nr, USB_RX));

	/* Check if necessary to use DMA transfer */
	if(endpoint_vs_interface_entry_p->buf_p)
	{
		usb_process_rx_dma_request(usb_dma_channel, dma_channel);
	}
	else
	{
		/* Data has been read in the fifo of the endpoint */
//		if(TRUE == USB_DMA_CFG.dma_usb_endpoint_rx_tbl[usb_dma_channel].channel_used)
//		{
			ret = unsubscribe_dma_usb_channel(dma_channel);
//			if(RV_OK == ret)
				USB_DMA_CFG.dma_usb_endpoint_rx_tbl[usb_dma_channel].channel_used = FALSE;
//		}
	}
}


/**
 *--------------------------------------------------------------------------------*
 * @name usb_get_dma_information
 *
 *	From a physical endpoint number this function returns
 *		- the DMA channel
 *		- the USB DMA cannel
 *
 *  @param dma_channel
 *
 *  @return	T_RV_RET
 *
 *---------------------------------------------------------------------------------*
 */
T_RV_RET usb_get_dma_information(BOOL transmit, UINT8 phys_ep_nr_requested, UINT8* dmg_channel, UINT8* usb_dma_channel)
{
	BOOL	index_found		= FALSE;
	UINT8	index			= 0xFF;

	UINT8	phys_ep_nr, phys_ep_idx;

	UINT8	i;

	if(transmit == TRUE)
	{
		/*
		 * Reserve dma receiption channels
		 */
		for(i = 0; i < DMA_TX_CHANNELS; i++)
		{
			/* Get index of physical endpoint from logical endpoint */
			phys_ep_idx = LEP_2_PEP(
				USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].endpoint_id,
				USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].interface_id, USB_TX);

			/* Get physical endpoint number */
			phys_ep_nr	= phys_ep_idx + 1;

			if(phys_ep_nr == phys_ep_nr_requested)
			{
				*dmg_channel		= USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].dmg_channel;
				*usb_dma_channel	= i;

				USB_TRACE_WARNING_PARAM("USB DMA : usb_get_dma_information : dmg_channel",
					*dmg_channel);

				USB_TRACE_WARNING_PARAM("USB DMA : usb_get_dma_information : usb_dma_channel",
					*usb_dma_channel);

				return(RVM_OK);
			}
		}
	}
	else
	{
		for(i = 0; i < DMA_RX_CHANNELS; i++)
		{
			/* Get index of physical endpoint from logical endpoint */
			phys_ep_idx = LEP_2_PEP(
				USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].endpoint_id,
				USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].interface_id,  USB_RX);

			/* Get physical endpoint number */
			phys_ep_nr	= phys_ep_idx + 1;

			if(phys_ep_nr == phys_ep_nr_requested)
			{
				*dmg_channel		= USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].dmg_channel;
				*usb_dma_channel	= i;

				USB_TRACE_WARNING_PARAM("USB DMA : usb_get_dma_information : dmg_channel",
					*dmg_channel);

				USB_TRACE_WARNING_PARAM("USB DMA : usb_get_dma_information : usb_dma_channel",
					*usb_dma_channel);

				return(RVM_OK);
			}
		}
	}

	USB_SEND_TRACE("USB DMA : no endpoint found to notify", RV_TRACE_LEVEL_ERROR);

	return(RV_INTERNAL_ERR);
}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name usb_get_dma_request_information
 *
 *	From a physical endpoint number this function returns
 *		- the DMA channel
 *		- the USB DMA cannel
 *
 *  @param dma_channel
 *
 *  @return	T_RV_RET
 *
 *---------------------------------------------------------------------------------*
 */
T_USB_DMA_REQUEST_INFORMATION*	usb_get_dma_request_information(UINT8 if_nr, UINT8 log_ep_nr)
{
	BOOL	index_found		= FALSE;
	UINT8	index			= 0xFF;

	UINT8	phys_ep_nr, phys_ep_idx;

	UINT8	i;

	/*
	 * Reserve dma receiption channels
	 */
	for(i = 0; i < DMA_TX_CHANNELS; i++)
	{
		if((USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].endpoint_id == log_ep_nr)
			&& (USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].interface_id == if_nr))
		{
			return(&(USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].usb_dma_transfer_info));
		}
	}

	for(i = 0; i < DMA_RX_CHANNELS; i++)
	{
		if((USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].endpoint_id == log_ep_nr)
			&& (USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].interface_id == if_nr))
		{
			return(&(USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].usb_dma_transfer_info));
		}
	}

	USB_SEND_TRACE("USB DMA : no endpoint found to notify", RV_TRACE_LEVEL_ERROR);

	return(NULL);
}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name usb_get_dma_request_information
 *
 *	From a physical endpoint number this function returns
 *		- the DMA channel
 *		- the USB DMA cannel
 *
 *  @param dma_channel
 *
 *  @return	T_RV_RET
 *
 *---------------------------------------------------------------------------------*
 */
T_USB_DMA_REQUEST_INFORMATION*	usb_get_dma_request_information_from_phys_endpoint(UINT8 phys_ep_nr_requested)
{
	BOOL	index_found		= FALSE;
	UINT8	index			= 0xFF;

	UINT8	phys_ep_nr, phys_ep_idx;

	UINT8	i;

	/*
	 * Reserve dma receiption channels
	 */
	for(i = 0; i < DMA_TX_CHANNELS; i++)
	{
		/* Get index of physical endpoint from logical endpoint */
		phys_ep_idx = LEP_2_PEP(
			USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].endpoint_id,
			USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].interface_id, USB_TX);

		/* Get physical endpoint number */
		phys_ep_nr	= phys_ep_idx + 1;

		if(phys_ep_nr == phys_ep_nr_requested)
		{
			return(&(USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].usb_dma_transfer_info));
		}
	}

	for(i = 0; i < DMA_RX_CHANNELS; i++)
	{
		/* Get index of physical endpoint from logical endpoint */
		phys_ep_idx = LEP_2_PEP(
			USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].endpoint_id,
			USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].interface_id, USB_RX);

		/* Get physical endpoint number */
		phys_ep_nr	= phys_ep_idx + 1;

		if(phys_ep_nr == phys_ep_nr_requested)
		{
			return(&(USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].usb_dma_transfer_info));
		}
	}

	USB_SEND_TRACE("USB DMA : no endpoint found to notify", RV_TRACE_LEVEL_ERROR);

	return(NULL);
}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name usb_ask_dma_reservation
 *
 *
 *  @param dma_channel
 *
 *  @return	T_RV_RET
 *
 *---------------------------------------------------------------------------------*
 */
UINT8	usb_ask_dma_reservation()
{
	UINT8	i;
	UINT8	index_ticket = 1;

	for(i = 0; i < DMA_TX_CHANNELS; i++)
	{
		if(USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].subscription_order >= index_ticket)
		{
			index_ticket = USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].subscription_order + 1;
		}
	}

	for(i = 0; i < DMA_RX_CHANNELS; i++)
	{
		if(USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].subscription_order >= index_ticket)
		{
			index_ticket = USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].subscription_order + 1;
		}
	}

	USB_TRACE_WARNING_PARAM("USB DMA: DMA reservation ticket : ", index_ticket);

	return(index_ticket);
}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name usb_update_dma_reservations
 *
 *
 *  @param dma_channel
 *
 *  @return	T_RV_RET
 *
 *---------------------------------------------------------------------------------*
 */
void	usb_update_dma_reservations()
{
	UINT8	i;

	for(i = 0; i < DMA_TX_CHANNELS; i++)
	{
		if(USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].subscription_order > 0)
		{
			USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].subscription_order--;
		}
	}

	for(i = 0; i < DMA_RX_CHANNELS; i++)
	{
		if(USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].subscription_order > 0)
		{
			USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].subscription_order--;
		}
	}
}
/*@}*/


/**
 *--------------------------------------------------------------------------------*
 * @name usb_process_dma_request
 *
 *
 *  @param dma_channel
 *
 *  @return	T_RV_RET
 *
 *---------------------------------------------------------------------------------*
 */
T_RV_RET usb_process_dma_request(T_DMA_TYPE_CHANNEL_NUMBER	dma_channel)
{
	BOOL	found = FALSE;
	UINT8	i;

	USB_SEND_TRACE("USB DMA: Getting a DMA channel ", 	RV_TRACE_LEVEL_DEBUG_LOW);
	USB_TRACE_WARNING_PARAM("USB DMA: DMA channel : ", 	dma_channel);

	for(i = 0; (i < DMA_TX_CHANNELS) && (found == FALSE); i++)
	{
		if(USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].subscription_order == 1)
		{
			USB_SEND_TRACE("USB DMA: Processing a TX DMA request", RV_TRACE_LEVEL_DEBUG_LOW);
			USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].subscription_order = 0;
			usb_process_tx_dma_request(i, dma_channel);
			found = TRUE;
		}
	}

	for(i = 0; (i < DMA_RX_CHANNELS) && (found == FALSE); i++)
	{
		if(USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].subscription_order == 1)
		{
			USB_SEND_TRACE("USB DMA: Processing a RX DMA request", RV_TRACE_LEVEL_DEBUG_LOW);
			USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].subscription_order = 0;
			usb_process_rx_request(i, dma_channel);
			found = TRUE;
		}
	}

	// usb_update_dma_reservations();
	
	return (RV_OK);
}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name usb_process_dma_request_in_standard_mode
 *
 *
 *  @param dma_channel
 *
 *  @return	T_RV_RET
 *
 *---------------------------------------------------------------------------------*
 */
T_RV_RET usb_process_dma_request_in_standard_mode()
{
	UINT8	i;
	T_RV_RET ret;
	BOOL	found = FALSE;

	UINT8 phys_ep_idx, phys_ep_nr;
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;	

	UINT8	dma_channel;

	USB_SEND_TRACE("USB DMA: No free DMA channel", 	RV_TRACE_LEVEL_DEBUG_LOW);

	for(i = 0; (i < DMA_TX_CHANNELS) && (found == FALSE); i++)
	{
		if(USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].subscription_order == 1)
		{
			USB_SEND_TRACE("USB DMA: Processing a TX DMA request in standard mode", RV_TRACE_LEVEL_DEBUG_LOW);
			USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].subscription_order = 0;

			phys_ep_idx = LEP_2_PEP(	USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].endpoint_id,
										USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].interface_id,
										USB_TX);

			phys_ep_nr = phys_ep_idx + 1;

			USB_EP_NUM_SEL_TX_EP(phys_ep_nr);
			//since a buffer with valid data is provided enable endpoint!
			W2FC_CTRL = W2FC_CTRL_CLR_HALT;
			//update the endpoint stall bitmap remove stall condition for this ep
			usb_env_ctrl_blk_p->bm_tx_ep_buf_stall &= ~(1U << phys_ep_nr );

			// TX Handler //
			if(write_non_iso_packet(phys_ep_idx) != USB_OK) {
				// ret = USB_INTERNAL_ERR;
				USB_SEND_TRACE("USB: write_non_iso_packet failed",
							RV_TRACE_LEVEL_ERROR);
				W2FC_CTRL = W2FC_CTRL_SET_HALT;
			}

			USB_EP_NUM_TX_RELEASE(phys_ep_nr );

			found = TRUE;
		}
	}

	for(i = 0; (i < DMA_RX_CHANNELS) && (found == FALSE); i++)
	{
		if(USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].subscription_order == 1)
		{
			USB_SEND_TRACE("USB DMA: Processing a RX DMA request in standard mode", RV_TRACE_LEVEL_DEBUG_LOW);
			USB_SEND_TRACE("USB DMA: Nothing to do", RV_TRACE_LEVEL_DEBUG_LOW);
			USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].subscription_order = 0;

			phys_ep_idx = LEP_2_PEP(	USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].endpoint_id,
										USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].interface_id,
										USB_RX);

			phys_ep_nr = phys_ep_idx + 1;

			endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl + phys_ep_idx;

			USB_DMA_CFG.dma_usb_rx_memo_size[i] = 0;			

			if(USB_DMA_CFG.dma_usb_endpoint_rx_tbl[i].interface_id == USB_IF_CDC_SERIAL)
			{
				/* Store info required to handle Missing ZLP Scenario */
				usb_env_ctrl_blk_p->trackMissingZLP.pep_idx = phys_ep_idx;
				usb_env_ctrl_blk_p->trackMissingZLP.size =  endpoint_vs_interface_entry_p->buf_size;	
						
				/* Start the timer to track missing ZLP */
				usb_start_timer();
			}
			/* 
				a user buffer is provided then enable fifo endpoint for the interrupt mode 
			*/
			USB_EP_NUM_SEL_RX_EP(phys_ep_nr);
			W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;			
			USB_EP_NUM_RX_RELEASE(phys_ep_nr);

			found = TRUE;
		}
	}

	usb_update_dma_reservations();

	return (RV_OK);
}
/*@}*/

T_RV_RET free_all_dma_channels_on_discon()
{
	UINT8	i = 0;

	for (i = 0; i < USB_TOTAL_DIFF_IF; i++) 
	{
		unsubscribe_dma_usb_interface(i);
	}

	/* 
		RX DMA request is not active 
	*/
	W2FC_RXDMA_CFG = 0;
	W2FC_TXDMA_CFG = 0;

	/* 
		wait 5 cycles, the hardware seems to need some time to update the registers
	*/
	for(i = 0; i < 5; i++);

	return (RV_OK);
}

