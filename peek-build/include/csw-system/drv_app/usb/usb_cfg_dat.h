/**
 * @file	usb_cfg_dat.h
 *
 * Declarations USB data specific functions
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

#ifndef __USB_CFG_DAT_H_
#define __USB_CFG_DAT_H_


#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */

/****************************************************************************************
 |	EXTERNAL FUNCTIONS	DECLARATIONS													|
 V**************************************************************************************V*/

/**
 * @name usb specific functions
 *
 * USB specific declarations 
 */
/*@{*/

T_RV_RET find_endpoint(UINT8 rx_tx, UINT8 if_nr, UINT8 ep_type, UINT8 ep_nr, T_USB_ENDPOINT_DATA* out_ep_pointer);

T_RV_RET conf_if_variables(void);

/*@}*/

#endif /*__USB_CFG_DAT_H_*/
