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
*   FILE NAME:      bipr_ui.c
*
*   DESCRIPTION:    This file contains the implementation of the BIP Initiator sample UI 
*					layer of the Neptune platform.
*
*   AUTHOR:         Yoni Shavit
*
\*******************************************************************************/

#include "btl_config.h"

#if BTL_CONFIG_BIP == BTL_CONFIG_ENABLED

#include "osapi.h"
#include "debug.h"
#include "../app_main.h"
#include "lineparser.h"

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

typedef void (*BipraFuncType)(void);

typedef struct
{
	char 			commandName[LINE_PARSER_MAX_STR_LEN + 1];
	BipraFuncType		funcPtr;
} BipraCommand;

/********************************************************************************
 *
 * Function prototypes
 *
 *******************************************************************************/

void BIPR_UI_ProcessUserAction(U8 *msg);

/********************************************************************************
 *
 * External functions
 *
 *******************************************************************************/
extern void BIPRA_Init(void);
extern void BIPRA_Deinit(void);
extern void BIPRA_Create(void);
extern void BIPRA_Destroy(void);
extern void BIPRA_Enable(void);
extern void BIPRA_Disable(void);
extern void BIPRA_Abort(void);
extern void BIPRA_GetSecurityLevel(void);
extern void BIPRA_SetSecurityLevel(void);

extern void BIPRA_GetPushedImageHandle(void);
extern void BIPRA_SetPushedImageHandle(void);

extern void BIPRA_GetAutoRequestThumbnailSetting(void);
extern void BIPRA_SetAutoRequestThumbnailSetting(void);

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/
/* BIPR commands array */
static const  BipraCommand bipraCommands[] =  {
			{"init",				BIPRA_Init},
			{"deinit",				BIPRA_Deinit},
			{"abort",       			BIPRA_Abort}, 		     	
			{"setSec",      		BIPRA_SetSecurityLevel},   
			{"getSec",      		BIPRA_GetSecurityLevel},   
			{"enable",      		BIPRA_Enable}, 		    
			{"disable",     			BIPRA_Disable},		     	 
			{"create",      		BIPRA_Create},		    
			{"destroy",     		BIPRA_Destroy},
			{"getPutImageHandle",	BIPRA_GetPushedImageHandle},
			{"setPutImageHandle",	BIPRA_SetPushedImageHandle},
			{"getAutoReqThumb",	BIPRA_GetAutoRequestThumbnailSetting},
			{"setAutoReqThumb",	BIPRA_SetAutoRequestThumbnailSetting}			
};

static const U32 bipraNumOfCommands = (sizeof(bipraCommands) / sizeof(BipraCommand));


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*------------------------------------------------------------------------------
 *            BIPRA_ProcessUserAction
 *------------------------------------------------------------------------------
 *
 * Synopsis:  Processes primitives for the BIPR application
 *
 * Return:    void
 *
 */
void BIPRA_UI_ProcessUserAction(U8 *msg)
{
	U8 	idx;
	U8 	command[LINE_PARSER_MAX_STR_LEN];
	
	if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr(command, LINE_PARSER_MAX_STR_LEN))
	{
		Report(("BIPRA: BIPRA_UI: Wrong BIPR command. Can't parse line"));
		return;
	}

	/* Look for the correct function */
	for(idx = 0; idx < bipraNumOfCommands; idx++)
	{
		if (OS_StriCmp((char*)command, bipraCommands[idx].commandName) == 0)
		{
			bipraCommands[idx].funcPtr();
			return;
		}
	}
	Report(("BIPRA: Invalid BIPRA Command(%s)", command));

}

#endif /* BTL_CONFIG_BIP == BTL_CONFIG_ENABLED */



