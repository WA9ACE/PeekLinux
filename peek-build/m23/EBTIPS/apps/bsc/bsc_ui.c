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
*   FILE NAME:      bsc_ui.c
*
*   DESCRIPTION:    Implementation of user interface part of Bluetooth System
*                   Coordinator sample application.
*
*   AUTHOR:         V. Abram
*
\*******************************************************************************/


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bttypes.h"
#include "osapi.h"
#include "lineparser.h"
#include "btl_bsc.h"


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

typedef U8 BscaUiCommandId;

/* MDG Application User Actions */
#define BSCA_UI_SET_HANDOVER_CONFIG             (1)
#define BSCA_UI_SET_PAUSE_MEDIA_PLAYER_CONFIG   (2)

typedef struct _BscaUiCommand
{
	char 		     cmdName[LINE_PARSER_MAX_STR_LEN];
	BscaUiCommandId  cmdId;
} BscaUiCommand;


/********************************************************************************
 *
 * Function prototypes
 *
 *******************************************************************************/
void BSCA_UI_ProcessUserAction(U8 *msg);


/********************************************************************************
 *
 * External functions
 *
 *******************************************************************************/
extern void BSCA_SetVoiceHandoverConfig(BtlBscVoiceHandoverConfig *handoverConfig);
extern void BSCA_SetPauseMediaPlayerOnIncCallConfig(BtlBscPauseMediaPlayerOnIncCallConfig *pauseMediaPlayerConfig);


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/* BSC Application commands for testing BTL BSC */
static const  BscaUiCommand bscaUiCommands[] =
{
    {"sethandoverconfig",           BSCA_UI_SET_HANDOVER_CONFIG}, 		       
    {"setpausemediaplayerconfig",   BSCA_UI_SET_PAUSE_MEDIA_PLAYER_CONFIG}
};

#define BSCA_NUM_COMMANDS (sizeof(bscaUiCommands) / sizeof(BscaUiCommand))


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BSCA_ProcessUserAction()
 *
 *		Translates received message to command and calls appropriate function.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	msg - received message.
 *
 * Returns:
 *		None.
 */
void BSCA_UI_ProcessUserAction(U8 *msg)
{
	U32 idx;
    BtlBscVoiceHandoverConfig handoverConfig;
    BtlBscPauseMediaPlayerOnIncCallConfig pauseMediaPlayerConfig;
	U8 command[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS status;

	status = LINE_PARSER_GetNextStr(command, LINE_PARSER_MAX_STR_LEN);

	if (LINE_PARSER_STATUS_SUCCESS != status)
	{
		Report(("BSCA_UI: wrong BSC command. Can't parse line"));
		return;
	}

	/* Look for the correct function */
	for(idx = 0; idx<BSCA_NUM_COMMANDS; idx++)
	{
		if (0 == OS_StrCmp((const char *)command,
                           (const char *)bscaUiCommands[idx].cmdName))
		{

            switch(bscaUiCommands[idx].cmdId)
			{
                case BSCA_UI_SET_HANDOVER_CONFIG:
	                /* Get voice handover configuration */
                    LINE_PARSER_GetNextBool(&handoverConfig.autoVoiceCallHandover);

                    /* Set the configuration */
                    BSCA_SetVoiceHandoverConfig(&handoverConfig);
                    break;

                case BSCA_UI_SET_PAUSE_MEDIA_PLAYER_CONFIG:
                    /* Get pause media player config */
                    LINE_PARSER_GetNextBool(&pauseMediaPlayerConfig.pausePlayerOnIncCall);
                    LINE_PARSER_GetNextBool(&pauseMediaPlayerConfig.pausePlayerOnIncCallToAnotherHeadset);

                    /* Set the configuration */
                    BSCA_SetPauseMediaPlayerOnIncCallConfig(&pauseMediaPlayerConfig);
                    break;

                default:
                    Report(("BSCA_UI: Mistake in commands table"));
                    break;
			}
            
			return;
		}
	}

	Report(("BSCA_UI: wrong BSC command"));
}

