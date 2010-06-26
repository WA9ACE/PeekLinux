/**
 * @file	usbms_message.h
 *
 * Data structures:
 * 1) used to send messages to the USBMS SWE,
 * 2) USBMS can receive.
 *
 * @author	Your name here (your_email_here)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	3/19/2004	Virgile COULANGE		Create.
 *
 * (C) Copyright 2004 by PHILOG, All Rights Reserved
 */

#ifndef __USBMS_MESSAGE_H_
#define __USBMS_MESSAGE_H_


#include "rv/rv_general.h"

#include "usbms/usbms_cfg.h"


#ifdef __cplusplus
extern "C"
{
#endif


/** 
 * The message offset must differ for each SWE in order to have 
 * unique msg_id in the system.
 */
#define USBMS_MESSAGE_OFFSET	 BUILD_MESSAGE_OFFSET(USBMS_USE_ID)



/**
 * @name USBMS_SAMPLE_MESSAGE
 *
 * Short description.
 *
 * Detailled description
 */
/*@{*/
/** Message ID. */
#define USBMS_SAMPLE_MESSAGE (USBMS_MESSAGE_OFFSET | 0x001)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;

	/** Some parameters. */
	/* ... */

} T_USBMS_SAMPLE_MESSAGE;
/*@}*/


#ifdef __cplusplus
}
#endif

#endif /* __USBMS_MESSAGE_H_ */
