/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      dio_bt.h
*
*   DESCRIPTION:    This file contains definitions and function prototypes for
*					dio_bt.c module implementing MODEM bridge.
*
*   AUTHOR:         Anat Firer
*
\*******************************************************************************/

#ifndef __DIO_BT_H__
#define __DIO_BT_H__

#include "dio_il/dio_drv.h"

/* Number of driver buffers */
#define DIO_DRV_RX_BUF_NUM			(2)
#define DIO_DRV_TX_BUF_NUM			(2)

/* Specific device prototypes */

/*
+------------------------------------------------------------------------------
|  Function : dio_init_bt
+------------------------------------------------------------------------------
|  Description : This function actually initialized the driver.  Each driver has
|                its own initialization function.  The function is not called
|                directly by the user of the DIO interface.  The DIO interface
|                layer calls the initialization functions of all DIO drivers when
|                dio_init() is called..
|                The function returns DRV_INITIALIZED if the driver has already
|                been initialized and is ready to be used or is already in use.
|                In case of an initialization failure (i.g. the configuration
|                given with drv_init can not be used) the function returns
|                DRV_INITFAILURE. In this case the drvier can not be used.
|
|  Parameter : none
|
|  Return : DRV_OK          - Initialization successful
|           DRV_INITFAILURE - Initialization failed
|           DRV_INITIALIZED - Interface already initialized and is ready to be
|                             used or is already in use.
|
|  NOTE:  none
+------------------------------------------------------------------------------
*/
U16 dio_init_bt (T_DIO_DRV *drv_init);



/*
+------------------------------------------------------------------------------
|  Function : dio_export_bt
+------------------------------------------------------------------------------
|  Description : This function returns a list of functions which are exported
|                by the driver.  Each driver has its own export function.
|                dio_export_drv() is not called directly byt he user of the
|                DIO interface.  The DIO interface layer calls the export
|                functions of all DIO drivers when dio_init() is called.  This
|                function needs to be provided by each DIO driver in order to
|                enable more than one driver using the DIO interface.
|
|  Parameter : dio func - With this parameter the driver returns a pointer to
|              the list of functions exported by the driver.
|
|  Return : T_DIO_FUNC  - pointer to static list of functions.
|
+------------------------------------------------------------------------------
*/
void dio_export_bt(T_DIO_FUNC **dio_func);

/*
+------------------------------------------------------------------------------
|  Function : SPP2MS_ConnectModem
+------------------------------------------------------------------------------
|  Description : Connects the SPP2M Bridge to the modem by connecting BT_DIO device to the
|		DIO interface sending signal DRV_SIGTYPE_CONNECT.
|
|  Parameter : None.
|
|  Return :	None.
|
|  NOTE:  none
+------------------------------------------------------------------------------
*/
void SPP2MS_ConnectModem(void);

/*
+------------------------------------------------------------------------------
|  Function : SPP2MS_DisconnectModem
+------------------------------------------------------------------------------
|  Description : Disconnects the SPP2M Bridge from the modem by disconnecting BT_DIO device
|		from the the DIO interface sending signal DRV_SIGTYPE_DISCONNECT.
|
|  Parameter : None.
|
|  Return :	None.
|
|  NOTE:  none
+------------------------------------------------------------------------------
*/
void SPP2MS_DisconnectModem(void);

#endif /* __DIO_BT_H__ */
