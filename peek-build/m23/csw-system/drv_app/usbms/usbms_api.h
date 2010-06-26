/**
 * @file	usbms_api.h
 *
 * API Definition for USBMS SWE.
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

#ifndef __USBMS_API_H_
#define __USBMS_API_H_


#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @name USBMS Return type and return values.
 *
 * Currently they are the standard RV return types, but they may
 * be customized in the future.
 */
/*@{*/
typedef T_RV_RET T_USBMS_RETURN;

#define	USBMS_OK					RV_OK				
#define	USBMS_NOT_SUPPORTED			RV_NOT_SUPPORTED		
#define	USBMS_MEMORY_ERR			RV_MEMORY_ERR			
#define	USBMS_INTERNAL_ERR			RV_INTERNAL_ERR
#define	USBMS_ERROR					-12
/*@}*/


/**
 * @name API functions
 *
 * API functions declarations (bridge functions).
 */
/*@{*/

T_USBMS_RETURN usbms_send_sample ();

/*@}*/


#ifdef __cplusplus
}
#endif


#endif /*__USBMS_API_H_*/

