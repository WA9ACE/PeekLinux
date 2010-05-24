/**
 * @file	usb_inth.h
 *
 * USB initialise and handle interrupt functions. FOR TARGET ENVIRONMENT
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
 *  14/01/2004  Pierre-Olivier POUX (PHILOG)
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#ifndef _USB_INTH_H_
#define _USB_INTH_H_

#include "clkm/clkm.h" /* for wait_ARM_cycles() */
#include "usb/usb_i.h"

/****************************************************************************************
 |	MACRO DEFINITIONS																	|
 V**************************************************************************************V*/

#define USB_EP_NUM_SEL_TX_EP(a)		USB_REG16_ASSIGN(W2FC_EP_NUM, (W2FC_EP_NUM_EP_SEL | W2FC_EP_NUM_EP_DIR| a))
#define USB_EP_NUM_SEL_RX_EP(a)		USB_REG16_ASSIGN(W2FC_EP_NUM, (W2FC_EP_NUM_EP_SEL | a))
#define USB_EP_NUM_TX_RELEASE(a)	USB_REG16_ASSIGN(W2FC_EP_NUM, (W2FC_EP_NUM_EP_DIR | (a) ))
#define USB_EP_NUM_RX_RELEASE(a)	USB_REG16_ASSIGN(W2FC_EP_NUM,  a)
#define W2FC_IRQ_SRC_CLR_INT(a)		(W2FC_IRQ_SRC = a)

/****************************************************************************************
 |	DEFINITIONS																			|
 V**************************************************************************************V*/

#define USB_NO_EOT		FALSE
#define	USB_EOT			TRUE

/****************************************************************************************
 |	EXTERNAL FUNCTIONS																	|
 V**************************************************************************************V*/

extern void usb_int_handler(void);
void usb_int_clr(void);
T_RV_RET usb_int_init(void);
T_RV_RET disable_interrupt_hndlr(void);
BOOL fill_ep_descr(T_USB_ENDPOINT_DESCR* temp_ep, T_USB_EP_DESCR ep_type);
T_RV_RET rmv_stall_cont_rx_trsfr(UINT8 log_ep_nr, UINT8 if_nr);
T_RV_RET prepare_first_tx_trsfr(UINT8 log_ep_nr, UINT8 if_nr);

T_RV_RET configure_ep0_interface(UINT8 if_nr);
T_RV_RET prepare_first_ep0_trsfr(UINT8 if_nr);
#undef _USB_INTH_H_
#endif
