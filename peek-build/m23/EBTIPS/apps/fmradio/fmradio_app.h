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
*   FILE NAME:      fmradio_app.c
*
*   DESCRIPTION:	This file contains the implementation of the FM Radio Primitive Handling - USER ACTIONS*
*   
*	AUTHOR:         Singh,Vikas
*
\*******************************************************************************/


#ifndef _FMRADIO_API_H
#define _FMRADIO_API_H


#define VOLUMN_MIN 0
#define VOLUMN_MAX 61808
#define VOLUMN_INIT 30904

#define GAIN_STEP	5000


void FMRADIOA_UI_ProcessUserAction(U8 * msg);
void FMRADIOA_Init();
void FMRADIOA_Deinit();





#endif

