/**
 * @file	usb_test_cfg.h
 *
 * Configuration definitions for the USB instance.
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

#ifndef __USB_CFG_H_
#define __USB_CFG_H_


#include "rv/rv_general.h"		/* General Riviera definitions. */
#include "usb/usb_pool_size.h"	/* Stack & Memory Bank sizes definitions */
#include "usb/usb_w2fc.h"


#define USB_BUILD_NR				      0
#define USB_MINOR_SW_VERSION_NR		0
#define USB_MAJOR_SW_VERSION_NR		1

#define MAX_STR_SIZE		  126
#define USB_CONF_ADRESSED	0
#define USB_CONF_DEFAULT	255

/**
 * Default endpoint buffer size. this is used in usb_endpoints_i.h
 */
#define	DEFAULT_SIZE			8

#define USB_EP_DEF_DESCR_LENGTH	7
#define USB_EP_TYPE				  5		  /*endpoint descriptor type according to USB 1.1 spec*/
#define USB_STRING_TYPE			3		  /*string descriptor type according to USB 1.1 spec*/

/*device specific config*/
#define USB_DEVICE_TYPE			1		  /*device descriptor type according to USB 1.1 spec*/
#define USB_DEV_DESCR_LNT		0x12
#define USB_BCD_CODE			0x0110	/*this should be set to a legal value*/
#define	USB_DEV_CLASS			0x00	// for composite device
#define USB_DEV_SUB_CLASS		0x00	// for composite device
#define USB_DEV_PROTOCOL		0x00


/*configuration specific config*/
#define USB_CONFIGURATION_TYPE	2	/*configuration descriptor type according to USB 1.1 spec*/	
#define USB_CNF_DESCR_LNT		0x09
//#define USB_CFG_TOT_DESCR_LNT	10

/*CDC specific config*/
#define USB_CDC_DESCR_LNT		0x0E	/*size of CDC descriptor = 14*/

/*IAD specific config*/
#define USB_IAD_DESCR_LNT		0x08	/*size of IAD descriptor = 8*/

/*interface specific config*/
#define USB_INTERFACE_TYPE		 4	/*interface descriptor type according to USB 1.1 spec*/
#define USB_IF_LNT				  0x09  /*this can be used for all interfaces*/


#define USB_EP_BASE				0x000	  /*reserved for setup fifo*/
#define USB_EP_BASE_SIZE		8				

/*endpoint0 config*/
#define USB_ENDP0_ADDR			(USB_EP_BASE + (USB_EP_BASE_SIZE / DEFAULT_SIZE))

#define USB_ENDP0_SIZE			64
#define USB_ENDP_BASE_ADDR		(USB_ENDP0_ADDR	+ (USB_ENDP0_SIZE / DEFAULT_SIZE)) /*this is the 
																					start address 
																					for the "normal" 
																					endpoints*/
//string index table
//As no string indexes are used the original table is deleted and values
//Are set to no string index
#define	USB_PROD_STR    1
#define	USB_MANUF_STR   2
#define USB_SERIAL_STR  3
//	USB_CFG1_STR,
//	USB_CFG2_STR,
//	USB_TEST1_IF_STR,
//	USB_TEST2_IF_STR,
//	USB_TEST3_IF_STR

/*endpoint types*/
#define USB_CONTROL				0x00
#define USB_ISOCHRONOUS		0x01		/*NOT SUPPORTED YET*/
#define USB_BULK				  0x02
#define USB_INTERRUPT			0x03		/*NOT SUPPORTED YET*/

#define	USB_RX_OUT				0
#define	USB_TX_IN				  1


#define USB_DEFAULT_EP	  0

/**
 * include endpoint specific data
 */


/**
 * @name Mem bank
 *
 * Memory bank size and watermark.
 * During development, it is better to put the value here (when the
 * test are activated).
 * At integration, put the value in rvf_pool_size.h
 */
/*@{*/
#define USB_MB_PRIM_SIZE                (USB_MB1_SIZE)
#define USB_MB_PRIM_WATERMARK           (USB_MB_PRIM_SIZE - 128)
/*@}*/

/** 
 * Wished priority of the host task for the SWE.
 *
 * During development, put the hardcoded value here.
 * After integration, the value should be in rvm_priorities.h
 */
#ifdef RVM_USB_TASK_PRIORITY
#define USB_TASK_PRIORITY               RVM_USB_TASK_PRIORITY	
#else
#define USB_TASK_PRIORITY               71 // old 71
#endif

#endif /* __USB_CFG_H_ */
