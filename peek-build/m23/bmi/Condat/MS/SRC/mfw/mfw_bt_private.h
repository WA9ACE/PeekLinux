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
*   FILE NAME:      mfw_bt_Private.h
*
*   DESCRIPTION:	This file defines the structures needed for BMI BT interface
*                   that are not exposed to BT
*
*   AUTHOR:         Anandhi Ramesh
*
*	Rev 0.1
*
\*******************************************************************************/
#ifndef __MFW_BT_PRIVATE
#define __MFW_BT_PRIVATE
#include "mfw_fm.h"
#include "mfw_bt_api.h"

#define BMI_BT_NOTCONNECTED 0
#define BMI_BT_CONNECTED 1

/*---------------------------------------------------------------------------*/
/*
 * BMI_BT_CMD_SRCTYPE
* Contains the types of commands that can occur. 
 */
/*---------------------------------------------------------------------------*/
typedef enum
{
  BMI_BT_COMMAND_NONE = 0,
  BMI_BT_HANDSET_COMMAND,
  BMI_BT_HEADSET_COMMAND
}BMI_BT_CMD_SRCTYPE;

/*---------------------------------------------------------------------------*/
/*
 * BMI_BT_STRUCTTYPE
 * Global structure that contains the following information
 *    Connection status
 *    State of audio - play/pause/stop etc
 *    Source of the command - could be from headset or handset
 *    callback function for BT
 */
/*---------------------------------------------------------------------------*/
typedef struct
{
    BOOL bConnected;
    T_FM_AUD_STATE tAudioState;
    BMI_BT_CMD_SRCTYPE tCmdSrc;
    void (*BmiEventCallback)(BmiAvEvent);
}BMI_BT_STRUCTTYPE;

#endif // __MFW_BT_PRIVATE


