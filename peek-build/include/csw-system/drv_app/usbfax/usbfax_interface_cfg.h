/**
 * @file	usbfax_interface_cfg.h
 *
 * Configuration definitions for the USBFAX instance.
 * 
 * @author
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	18/02/2004
 */

#ifndef __USBFAX_INTERFACE_CFG_H_
#define __USBFAX_INTERFACE_CFG_H_

#include "usbfax/usbfax_export_i.h"

/**
 * @name T_USB_ENDPOINT_CONTEXT
 *
 *
 *
 *
 */
/*@{*/
typedef struct
{
	U8	interface_id;
	U8	endpoint_id;
	U8	MaxPacketSize;
} T_USBFAX_ENDPOINT_CONTEXT;
/*@}*/

/**
 * @name T_USBFAX_ENDPOINT_CONTEXT_CNFG
 *
 * This structure is used to store line serial emulation
 * 
 *
 */
/*@{*/
typedef struct
{
  	T_USBFAX_ENDPOINT_CONTEXT	ep_control_context;
 	T_USBFAX_ENDPOINT_CONTEXT	ep_int_context;
  	T_USBFAX_ENDPOINT_CONTEXT	ep_tx_context;
	T_USBFAX_ENDPOINT_CONTEXT	ep_rx_context;
} T_USBFAX_ENDPOINT_CONTEXT_CNFG;
/*@}*/

#endif /* __USBFAX_INTERFACE_CFG_H_ */
