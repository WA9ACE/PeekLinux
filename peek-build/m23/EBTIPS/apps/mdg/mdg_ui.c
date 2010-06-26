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
*   FILE NAME:      mdg_ui.c
*
*   DESCRIPTION:    Implementation of user interface part of Modem Data Gateway
*                   sample application for DUN.
*
*   AUTHOR:         V. Abram
*
\*******************************************************************************/


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "btl_config.h"
#include "osapi.h"

#if BTL_CONFIG_MDG == BTL_CONFIG_ENABLED

#include "bttypes.h"
#include "sec.h"
#include "spp.h"
#include "lineparser.h"


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

typedef U8 MdgaUiCommandId;

/* MDG Application User Actions */
#define MDGA_UI_ENABLE                      (1)
#define MDGA_UI_DISABLE                     (2)
#define MDGA_UI_GET_SECURITY_LEVEL          (3)
#define MDGA_UI_SET_SECURITY_LEVEL          (4)
#define MDGA_UI_GET_COM_SETTINGS            (5)
#define MDGA_UI_SET_COM_SETTINGS            (6)
#define MDGA_UI_GET_CONNECTED_DEVICE        (7)
#define MDGA_UI_TERMINATE_DATA_CALL         (8)

typedef struct _MdgaUiCommand
{
	char 		     cmdName[LINE_PARSER_MAX_STR_LEN];
	MdgaUiCommandId  cmdId;
} MdgaUiCommand;


/********************************************************************************
 *
 * Function prototypes
 *
 *******************************************************************************/
void MDGA_UI_ProcessUserAction(U8 *msg);


/********************************************************************************
 *
 * External functions
 *
 *******************************************************************************/
extern void MDGA_Enable(U8 serverIndex);
extern void MDGA_Disable(U8 serverIndex);
extern void MDGA_GetSecurityLevel(U8 serverIndex);
extern void MDGA_SetSecurityLevel(U8 serverIndex, BtSecurityLevel *securityLevel);
extern void MDGA_GetComSettings(U8 serverIndex);
extern void MDGA_SetComSettings(U8 serverIndex, SppComSettings *comSettings);
extern void MDGA_GetConnectedDevice(U8 serverIndex);
extern void MDGA_TerminateDataCall(void);


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/* MDG Application commands for testing BTL MDG */
static const  MdgaUiCommand mdgaUiCommands[] =
{
    {"enable",          MDGA_UI_ENABLE}, 		       
    {"disable",         MDGA_UI_DISABLE}, 			
    {"getsec",          MDGA_UI_GET_SECURITY_LEVEL}, 			
    {"setsec",          MDGA_UI_SET_SECURITY_LEVEL}, 			
    {"getcomset",       MDGA_UI_GET_COM_SETTINGS}, 			
    {"setcomset",       MDGA_UI_SET_COM_SETTINGS}, 			
    {"getcondev",       MDGA_UI_GET_CONNECTED_DEVICE}, 			
    {"termdatacall",    MDGA_UI_TERMINATE_DATA_CALL}
};

#define MDGA_NUM_COMMANDS (sizeof(mdgaUiCommands) / sizeof(MdgaUiCommand))


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * MDGA_ProcessUserAction()
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
void MDGA_UI_ProcessUserAction(U8 *msg)
{
	U32 idx;
    U8 serverIndex;
	U8 command[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS status;
	
	status = LINE_PARSER_GetNextStr(command, LINE_PARSER_MAX_STR_LEN);

	if (LINE_PARSER_STATUS_SUCCESS != status)
	{
		Report(("MDGA_UI: wrong MDG command. Can't parse line"));
		return;
	}

	/* Look for the correct function */
	for(idx = 0; idx<MDGA_NUM_COMMANDS; idx++)
	{
		if (0 == OS_StrCmp((const char *)command,
                           (const char *)mdgaUiCommands[idx].cmdName))
		{
	        /* Get server's number common for all commands */
            LINE_PARSER_GetNextU8(&serverIndex, FALSE);

            switch(mdgaUiCommands[idx].cmdId)
			{
                case MDGA_UI_ENABLE:
                    MDGA_Enable(serverIndex);
                    break;

                case MDGA_UI_DISABLE:
                    MDGA_Disable(serverIndex);
                    break;

                case MDGA_UI_GET_SECURITY_LEVEL:
                    MDGA_GetSecurityLevel(serverIndex);
                    break;

                case MDGA_UI_SET_SECURITY_LEVEL:
                {
                    BtSecurityLevel securityLevel;

	                LINE_PARSER_GetNextU8(&securityLevel, FALSE);
                    MDGA_SetSecurityLevel(serverIndex, &securityLevel);
                    break;
                }

                case MDGA_UI_GET_COM_SETTINGS:
                    MDGA_GetComSettings(serverIndex);
                    break;

                case MDGA_UI_SET_COM_SETTINGS:
                {
                    SppComSettings comSettings;

	                /* Extract communication settings */
	                LINE_PARSER_GetNextU8(&comSettings.baudRate, FALSE);
	                LINE_PARSER_GetNextU8(&comSettings.dataFormat, FALSE);
	                LINE_PARSER_GetNextU8(&comSettings.flowControl, FALSE);
	                LINE_PARSER_GetNextU8(&comSettings.xonChar, FALSE);
	                LINE_PARSER_GetNextU8(&comSettings.xoffChar, FALSE);
	                LINE_PARSER_GetNextU16(&comSettings.parmMask, FALSE);
                    
                    MDGA_SetComSettings(serverIndex, &comSettings);
                    break;
                }

                case MDGA_UI_GET_CONNECTED_DEVICE:
                    MDGA_GetConnectedDevice(serverIndex);
                    break;

                case MDGA_UI_TERMINATE_DATA_CALL:
                    MDGA_TerminateDataCall();
                    break;

                default:
                    Report(("MDGA_UI: Mistake in commands table"));
                    break;
			}
            
			return;
		}
	}

	Report(("MDGA_UI: wrong MDG command"));
}

#else/* BTL_CONFIG_MDG == BTL_CONFIG_ENABLED */

void MDGA_UI_ProcessUserAction(U8 *msg)
{

	msg=msg;
	Report(("MDG_APP is disabled via BTL_CONFIG."));
}


#endif /* BTL_CONFIG_MDG == BTL_CONFIG_ENABLED */

