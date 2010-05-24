/**
 * @file	usb_dma.h
 *
 * Declarations DMA USB data specific functions
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

/****************************************************************************************
 |	EXTERNAL FUNCTIONS																	|
 V**************************************************************************************V*/
T_RV_RET init_dma_usb_tbl();

T_RV_RET	subscribe_dma_usb_interface	(UINT8 interface_id);

T_RV_RET 	unsubscribe_dma_usb_interface	(UINT8 interface_id);

T_RV_RET 	usb_prepare_dma_tx_transfer	(UINT8 interface_id,
										 UINT8 endpoint_id);

T_RV_RET 	usb_prepare_dma_rx_transfer	(UINT8 interface_id,
										 UINT8 endpoint_id);

T_RV_RET	usb_reserve_dma_channels	();

T_RV_RET						usb_get_dma_information(BOOL transmit, UINT8 phys_ep_nr_requested, UINT8* dmg_channel, UINT8* usb_dma_channel);
T_USB_DMA_REQUEST_INFORMATION*	usb_get_dma_request_information(UINT8 if_nr, UINT8 log_ep_nr);
T_USB_DMA_REQUEST_INFORMATION*	usb_get_dma_request_information_from_phys_endpoint(UINT8 phys_ep_nr_requested);

#if (CHIPSET==15)
T_RV_RET	unsubscribe_dma_usb_channel(T_DMA_CHANNEL	dma_channel);
#else
T_RV_RET	unsubscribe_dma_usb_channel(T_DMG_CHANNEL	dma_channel);
#endif
UINT8		usb_ask_dma_reservation();
void		usb_update_dma_reservations();
/****************************************************************************************
 |	Callback functions																	|
 V**************************************************************************************V*/
static T_RV_RET usb_non_iso_tx_dma_done_hndlr(void);
static T_RV_RET usb_non_iso_rx_dma_eot_hndlr(void);
static T_RV_RET usb_non_iso_rx_dma_transaction_count_hndlr(void);

