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
*   FILE NAME:      avrcptg_ui.c
*
*   DESCRIPTION:    Implementation of user interface part of Avrcp target role.
*
*   AUTHOR:         Alon Cheifetz.
*
\*******************************************************************************/

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include "btl_config.h"
#include "osapi.h"
#if BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED

#include "../app_main.h"
#include "lineparser.h"	

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/* Application User Actions */
#define AVRCPTGA_UI_INIT					(1)			      
#define AVRCPTGA_UI_DEINIT 					(2)
#define AVRCPTGA_UI_CREATE					(3)
#define AVRCPTGA_UI_DESTROY					(4)
#define AVRCPTGA_UI_ENABLE 					(5)
#define AVRCPTGA_UI_DISABLE					(6)
#define AVRCPTGA_UI_SET_SECURITY_LEVEL		(7)
#define AVRCPTGA_UI_GET_SECURITY_LEVEL		(8)
#define AVRCPTGA_UI_CONNECT 				(9)
#define AVRCPTGA_UI_DISCONNECT 				(10)
#define AVRCPTGA_UI_GET_CONNECTED_DEVICE	(11)

typedef U8 AvrcptgaUiCommandId;

typedef struct _AvrcptgUiCommand
{
	char 		     	cmdName[LINE_PARSER_MAX_STR_LEN];
	AvrcptgaUiCommandId  cmdId;
} AvrcptgaUiCommand;

/****************************************************************************
 *
 * Internal Function prototypes
 *
 ***************************************************************************/

void AVRCPTGA_UI_ProcessUserAction(U8 *msg);

/********************************************************************************
 *
 * External functions
 *
 *******************************************************************************/
extern void AVRCPTGA_Init(void);
extern void AVRCPTGA_Deinit(void);
extern void avrcptgaCreate(void);
extern void avrcptgaDestroy(void);
extern void avrcptgaEnable(void);
extern void avrcptgaDisable(void);
extern void avrcptgaSetSecurityLevel(void);
extern void avrcptgaGetSecurityLevel(void);
extern void avrcptgaConnect(void);
extern void avrcptgaConnectResponse(void);
extern void avrcptgaDisconnect(void);
extern void avrcptgaGetConnectedDevice(void);

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/* AVRCPTG commands array */
static const  AvrcptgaUiCommand avrcptgaUiCommands[] =  { {"init",		AVRCPTGA_UI_INIT}, 		       
														{"deinit",		AVRCPTGA_UI_DEINIT}, 			
														{"create",		AVRCPTGA_UI_CREATE},			    
														{"destroy",		AVRCPTGA_UI_DESTROY},
														{"enable",		AVRCPTGA_UI_ENABLE}, 		     	
														{"disable",		AVRCPTGA_UI_DISABLE}, 		     	
														{"setSecurity",	AVRCPTGA_UI_SET_SECURITY_LEVEL},   
														{"getSecurity",	AVRCPTGA_UI_GET_SECURITY_LEVEL},   
														{"connect",		AVRCPTGA_UI_CONNECT}, 		     	
														{"disconnect",	AVRCPTGA_UI_DISCONNECT}, 		     	
														{"getConnDev",	AVRCPTGA_UI_GET_CONNECTED_DEVICE}  	
													 };
													 
#define AVRCPTG_COMMANDS_NUM (12)

/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*******************************************************************************\
*  FUNCTION:	AVRCPTGA_UI_ProcessUserAction
*                                                                         
*  PARAMETERS:	msg - Parameters of actions.
*                                                                         
*  DESCRIPTION:	Processes primitives for the AVRCPTG application
*                                                                         
*  RETURNS:		None.
\*******************************************************************************/
void AVRCPTGA_UI_ProcessUserAction(U8 *msg)
{
	U32 i;
	U8 command[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS status;

	status = LINE_PARSER_GetNextStr(command, LINE_PARSER_MAX_STR_LEN);

	if (status != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("Wrong AVRCPTG command. Can't parse line"));
		return;
	}

	/* Look for the correct function */
	for(i = 0; i < AVRCPTG_COMMANDS_NUM; i++)
	{
		if (OS_StrCmp((const char *)command, avrcptgaUiCommands[i].cmdName) == 0)
		{
            switch(avrcptgaUiCommands[i].cmdId)

			{

                case AVRCPTGA_UI_INIT:
					AVRCPTGA_Init();
                    break;

                case AVRCPTGA_UI_DEINIT:
					AVRCPTGA_Deinit();
                    break;

                case AVRCPTGA_UI_CREATE:
					avrcptgaCreate();
                    break;

                case AVRCPTGA_UI_DESTROY:
					avrcptgaDestroy();
                    break;

                case AVRCPTGA_UI_ENABLE:
					avrcptgaEnable();
                    break;

                case AVRCPTGA_UI_DISABLE:
					avrcptgaDisable();
                    break;

                case AVRCPTGA_UI_SET_SECURITY_LEVEL:
					avrcptgaSetSecurityLevel();
                    break;

                case AVRCPTGA_UI_GET_SECURITY_LEVEL:
					avrcptgaGetSecurityLevel();
                    break;

                case AVRCPTGA_UI_CONNECT:
					avrcptgaConnect();
                    break;

                case AVRCPTGA_UI_DISCONNECT:
					avrcptgaDisconnect();
                    break;

                case AVRCPTGA_UI_GET_CONNECTED_DEVICE:
					avrcptgaGetConnectedDevice();
                    break;

                default:
                    Report(("AVRCPTGA_UI: Mistake in commands table"));
                    break;
			}
			
			return;
		}
	}

	Report(("Wrong AVRCPTG command"));
}


#else /*BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED*/

void AVRCPTGA_UI_ProcessUserAction(U8 *msg)
{
	msg=msg;
	Report(("AvrcpTg_APP is disabled via BTL_CONFIG."));

}



#endif /*BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED*/

