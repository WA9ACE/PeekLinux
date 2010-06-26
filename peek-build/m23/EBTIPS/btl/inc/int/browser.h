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
*   FILE NAME:      browser.h
*
*   DESCRIPTION:    This file contains the interface for the folder browser.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/


#ifndef __BROWSER_H
#define __BROWSER_H


#include "osapi.h"
#include "obexerr.h"
#include "btl_config.h"
#include "btl_unicode.h"

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/


/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode BROWSER_Open(const char *Folder)
 *
 * Description:   Initialize the folder browser to read the specified folder
 *                listing.
 *
 * Parameters:    Folder - name of folder to read.
 *
 * Returns:       OBRC_SUCCESS - Browser is ready.
 */
ObexRespCode BROWSER_Open(const BtlUtf8 *Folder,U8*	hideObjects[BTL_CONFIG_FTPS_MAX_HIDE_OBJECT],BOOL hideMode);

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode BROWSER_ReadFlex(U8           *Buff,
 *                                              U16          *Len,
 *                                              BOOL         *More )
 *
 * Description:   Read the folder object data.
 *
 * Parameters:    Buff - pointer to location where data is read into.
 *                Len - maximum number of bytes of data to read
 *                More - callee sets to TRUE if it has more data to read.
 *
 * Returns:       OBRC_SUCCESS      - Read was successful.
 *                OBRC_UNAUTHORIZED - Read failed. 
 */
ObexRespCode BROWSER_ReadFlex(U8 *Buff, U16 *Len, BOOL *More);
#endif

/*---------------------------------------------------------------------------
 *
 * Prototype:     U32 BROWSER_GetObjectLen(void)
 *
 * Description:   Get the length of an folder listing object.
 *
 * Parameters:    Obm - pointer to the object manager entry.
 *
 * Returns:       The object length
 */
U32 BROWSER_GetObjectLen(void);

#define BROWSER_GetObjectLen()  UNKNOWN_OBJECT_LENGTH

#endif /* __BROWSER_H */
