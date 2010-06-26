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
*   FILE NAME:      bipi_ui.c
*
*   DESCRIPTION:    This file contains the implementation of the BIP Initiator sample UI 
*					layer of the Neptune platform.
*
*   AUTHOR:         Yoni Shavit
*
\*******************************************************************************/

#include "btl_config.h"
#include "osapi.h"
#if BTL_CONFIG_BIP == BTL_CONFIG_ENABLED


#include "debug.h"
#include "../app_main.h"
#include "lineparser.h"

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

typedef void (*BipiaFuncType)(void);

typedef struct
{
	char 			commandName[LINE_PARSER_MAX_STR_LEN + 1];
	BipiaFuncType		funcPtr;
} BipiaCommand;

/********************************************************************************
 *
 * Function prototypes
 *
 *******************************************************************************/

void BIPI_UI_ProcessUserAction(U8 *msg);

/********************************************************************************
 *
 * External functions
 *
 *******************************************************************************/
extern void BIPIA_Create(void);
extern void BIPIA_Destroy(void);
extern void BIPIA_Enable(void);
extern void BIPIA_Disable(void);
extern void BIPIA_Abort(void);
extern void BIPIA_Disconnect(void);
extern void BIPIA_GetSecurityLevel(void);
extern void BIPIA_SetSecurityLevel(void);
extern void BIPIA_Connect(void);

extern void BIPIA_GetCapabilities(void);
extern void BIPIA_GetImagesList(void);
extern void BIPIA_GetImage(void);
extern void BIPIA_GetImageProperties(void);
extern void BIPIA_GetMonitoringImage(void);
extern void BIPIA_GetLinkedThumbnail(void);

extern void BIPIA_PutImage(void);
extern void BIPIA_PutLinkedThumbnail(void);

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/
/* BIPI commands array */
static const  BipiaCommand bipiaCommands[] =  { 
			{"abort",       			BIPIA_Abort}, 		     	
			{"setSec",      		BIPIA_SetSecurityLevel},   
			{"getSec",      		BIPIA_GetSecurityLevel},   
			{"enable",      		BIPIA_Enable}, 		    
			{"disable",     			BIPIA_Disable},		     	 
			{"create",      		BIPIA_Create},			    
			{"destroy",     		BIPIA_Destroy},			    	
			{"connect",			BIPIA_Connect},			    
			{"disconnect",			BIPIA_Disconnect},
			{"getCapabilities",		BIPIA_GetCapabilities},
			{"getList",			BIPIA_GetImagesList}, 		 
			{"getImage",			BIPIA_GetImage},
			{"getProperties",		BIPIA_GetImageProperties},			    
			{"getMonImage",		BIPIA_GetMonitoringImage},			    	
			{"getThumbnail",		BIPIA_GetLinkedThumbnail},			    	
			{"putImage",			BIPIA_PutImage}, 		 
			{"PutThumbnail",		BIPIA_PutLinkedThumbnail},
											 };
static const U32 bipiaNumOfCommands = (sizeof(bipiaCommands) / sizeof(BipiaCommand));


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*------------------------------------------------------------------------------
 *            BIPIA_ProcessUserAction
 *------------------------------------------------------------------------------
 *
 * Synopsis:  Processes primitives for the BIPI application
 *
 * Return:    void
 *
 */
void BIPIA_UI_ProcessUserAction(U8 *msg)
{
	U8 	idx;
	U8 	command[LINE_PARSER_MAX_STR_LEN];
	
	if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(command, LINE_PARSER_MAX_STR_LEN))
	{
		Report(("BIPIA: BIPIA_UI: Wrong BIPI command. Can't parse line"));
		return;
	}

	/* Look for the correct function */
	for(idx = 0; idx < bipiaNumOfCommands; idx++)
	{
		if (OS_StriCmp((char*)command, bipiaCommands[idx].commandName) == 0)
		{
			bipiaCommands[idx].funcPtr();
			return;
		}
	}
	Report(("BIPIA: Invalid BIPIA Command"));

}

#else /* BTL_CONFIG_BIP == BTL_CONFIG_ENABLED */

void BIPIA_UI_ProcessUserAction(U8 *msg)
{
	msg=msg;
	Report(("BipiA_APP is disabled via BTL_CONFIG."));
}

#endif /* BTL_CONFIG_BIP == BTL_CONFIG_ENABLED */

