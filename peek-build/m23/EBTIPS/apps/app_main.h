/*******************************************************************************\
*                                                                       		*
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
*   FILE NAME:		app_main.h
*
*   DESCRIPTION:	Constant definitions and function prototypes for the
*					app_main.c module 
*
*   AUTHOR:			V. Abram
*
\*******************************************************************************/

#ifndef APP_MAIN_H
#define APP_MAIN_H


#define APP_MSG_RCV_ID_MASK		(0x000000ff)	/* Mask for message receiver */

/* Common BT User Actions */
#define APP_UA_BT_RADIO_OFF		(0)
#define APP_UA_BT_RADIO_ON		(1)

/* User Actions offsets definitions */
#define APP_UA_OFFSET			(0)

/* Definitions of translation macros and functions */
#define ASCII_TO_INT(_val)		(_val - '0')
#define TWO_DIGITS_TO_VALUE(_prim, _offset)	((ASCII_TO_INT(_prim[_offset])) * 10 + \
											 ASCII_TO_INT(_prim[_offset + 1]))

unsigned char APP_HexDigitToInt(const char _val);
#define HEX_DIGIT_TO_INT(_val)	APP_HexDigitToInt(_val)

/* Macro for sending events to UI - meanwhile, trace message to PCO Viewer */
extern int vsi_o_state_ttrace ( const char * const format, ... );
#define UI_DISPLAY(s) vsi_o_state_ttrace s

#endif /* APP_MAIN_H */

