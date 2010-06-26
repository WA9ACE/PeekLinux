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
*   FILE NAME:		sbaps_app.c
*
*   DESCRIPTION:		This file contains the SAP server application in the neptune
*					platform.
*
*
*   AUTHOR:			Itay Klein
*
\*******************************************************************************/

 
#include "btl_config.h"
#include "osapi.h"
#include "bttypes.h"
#if BTL_CONFIG_SAPS == BTL_CONFIG_ENABLED

/* GPF Includes */
#include "typedefs.h"
#include "vsi.h"
#include "../p_btt.h"

#if BT_STACK == XA_ENABLED
#include <me.h>
#include <bttypes.h>
#include <sec.h>
#endif

#include "btl_common.h"
#include "debug.h"

#include "../app_main.h"


/* BTL includes */
#include "btl_saps.h"
#include "lineparser.h"

BtlSapsContext	*sapsContext = 0;
BtSecurityLevel	btlSapsSecurityLevel = BSL_NO_SECURITY;

/****************************************************************************
 *
 * Internal Function prototypes
 *
 ***************************************************************************/

void SAPA_ProcessUserAction(U8 *msg);
BOOL SAPA_Init(void);
BOOL SAPA_Deinit(void);
BOOL APP_SAPS_Create(void);
BOOL APP_SAPS_Destroy(void);
BOOL APP_SAPS_Enable(void);
BOOL APP_SAPS_Disable(void);
BOOL APP_SAPS_Disconnect(void);
BOOL APP_SAPS_SetSecurityLevel(void);
BOOL APP_SAPS_GetSecurityLevel(void);

static void APP_SAPS_CallBack(BtlSapsEvent *event);


typedef BOOL (*FuncType)(void);

typedef struct _BtlSapsCommand
{
	char			funcName[LINE_PARSER_MAX_STR_LEN];
	FuncType		funcPtr;
	
} BtlSapsCommand;

/****************************************************************************
 *
 * ROM data
 *
 ****************************************************************************/

/* SAPS commands array */
static const  BtlSapsCommand btlSapsCommands[] =	{   
														{"init",		SAPA_Init},
														{"setsec",	APP_SAPS_SetSecurityLevel},
														{"getsec",	APP_SAPS_GetSecurityLevel},
														{"enable",	APP_SAPS_Enable},
														{"disable",	APP_SAPS_Disable},
														{"create",	APP_SAPS_Create},
														{"destroy",	APP_SAPS_Destroy},
														{"disconnect",	APP_SAPS_Disconnect}
												};

void SAPA_ProcessUserAction(U8 *msg)
{
	U8 i;
	char command[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS status;
	
	status = LINE_PARSER_GetNextStr((U8*)command, LINE_PARSER_MAX_STR_LEN);

	if (status != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("SAPS APP: Wrong SAPS command. Can't parse line"));
		return;
	}

	/* Look for the correct function */
	for(i = 0; i < sizeof(btlSapsCommands)/sizeof(BtlSapsCommand); ++i)
	{
		if (OS_StrCmp(command, btlSapsCommands[i].funcName) == 0)
		{
			btlSapsCommands[i].funcPtr();
			return;
		}
	}
	Report(("SAPS APP: Wrong SAPS command"));
	
}

BOOL SAPA_Init(void)
{
	BtStatus status;
	
	status = BTL_SAPS_Create(0, APP_SAPS_CallBack, 0, &sapsContext);

	Report(("SAPS APP: BTL_SAPS_Create() returned %s.",pBT_Status(status)));

	status = BTL_SAPS_Enable(sapsContext, 0);
		
	Report(("SAPS APP: BTL_SAPS_Enable() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

BOOL SAPA_Deinit(void)
{
	BtStatus status;

	status = BTL_SAPS_Disable(sapsContext);
	
	Report(("SAPS APP: BTL_SAPS_Disable() returned %s.",pBT_Status(status)));
	
	status =  BTL_SAPS_Destroy(&sapsContext);
	
	Report(("SAPS APP: BTL_SAPS_Destroy() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

BOOL APP_SAPS_Disconnect(void)
{
	BtStatus status;
	
	status = BTL_SAPS_Disconnect(sapsContext, FALSE);
	
	Report(("SAPS APP: BTL_SAPS_Disconnect() returned %s.",pBT_Status(status)));
	
	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

BOOL APP_SAPS_Enable(void)
{
	BtStatus status;
		
	status = BTL_SAPS_Enable(sapsContext, 0);
		
	Report(("SAPS APP: BTL_PBAPS_Enable() returned %s.",pBT_Status(status)));
	
	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

BOOL APP_SAPS_Disable(void)
{
	BtStatus status;
	
	status = BTL_SAPS_Disable(sapsContext);
	
	Report(("SAPS APP: BTL_PBAPS_Disable() returned %s.",pBT_Status(status)));
	
	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

BOOL APP_SAPS_Create(void)
{
	BtStatus status;
	LINE_PARSER_STATUS parserStatus;

	if (TRUE == LINE_PARSER_AreThereMoreArgs() )
	{
		parserStatus = LINE_PARSER_GetNextU8(&btlSapsSecurityLevel, TRUE);
		
		if (parserStatus != LINE_PARSER_STATUS_SUCCESS)
		{
			Report(("SAPS APP: Can't read security level. Ignoring command"));
			return FALSE;
		}
	}
	status = BTL_SAPS_Create(0, APP_SAPS_CallBack, &btlSapsSecurityLevel, &sapsContext);

	Report(("SAPS APP: BTL_SAPS_Create() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

BOOL APP_SAPS_Destroy(void)
{
	BtStatus status;
	
	status = BTL_SAPS_Destroy(&sapsContext);

	Report(("SAPS APP: BTL_SAPS_Destroy() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

BOOL APP_SAPS_SetSecurityLevel()
{
	BtStatus status;
	BOOL retVal = FALSE;
	BtSecurityLevel newSecurityLevel = btlSapsSecurityLevel;
	LINE_PARSER_STATUS parserStatus;
	
	parserStatus = LINE_PARSER_GetNextU8(&newSecurityLevel, TRUE);
	
	if (parserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("SAPS APP: Can't read security level. Ignoring command"));
		return FALSE;
	}
	
	status = BTL_SAPS_SetSecurityLevel(sapsContext, &newSecurityLevel);
	
	Report(("SAPS APP: BTL_SAPS_SetSecurityLevel() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (retVal) : (!retVal));
}

BOOL APP_SAPS_GetSecurityLevel(void)
{
	BtStatus status;
	BtSecurityLevel level;
	
	status = BTL_SAPS_GetSecurityLevel(sapsContext, &level);
	
	Report(("SAPS APP: BTL_SAPS_GetSecurityLevel() returned %s.",pBT_Status(status)));

	switch (level)
	{
		case 0:
			Report(("Security level: BSL_NO_SECURITY"));
			break;
		case 1:
			Report(("Security level: BSL_AUTHENTICATION_IN"));
			break;
		case 2:
			Report(("Security level: BSL_AUTHORIZATION_IN"));
			break;
		case 4:
			Report(("Security level: BSL_ENCRYPTION_IN"));
			break;
		case 3:
			Report(("Security level: BSL_AUTHENTICATION_IN"));
			Report(("and BSL_AUTHORIZATION_IN"));
			break;
		case 5:
			Report(("Security level: BSL_AUTHENTICATION_IN"));
			Report(("and BSL_ENCRYPTION_IN"));
			break;
		case 6:
			Report(("Security level: BSL_AUTHORIZATION_IN"));
			Report(("and BSL_ENCRYPTION_IN"));
			break;
		case 7:
			Report(("Security level: BSL_AUTHENTICATION_IN"));
			Report(("and BSL_AUTHORIZATION_IN"));
			Report(("and BSL_ENCRYPTION_IN"));
			break;
		default:
			Report(("Error: Wrong security level"));
			break;
			
	}
	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}


static void APP_SAPS_CallBack(BtlSapsEvent *event)
{
	BtStatus		status;
	BD_ADDR	bdAddr;
	char addr[BDADDR_NTOA_SIZE];

	switch (event->event) {
	case SAEVENT_SERVER_OPEN_IND:
		if ((status = BTL_SAPS_RespondOpenInd(sapsContext, TRUE)) != BT_STATUS_PENDING)
		{
			Report(("SAPS APP: Incoming connection could not be accepted, reason: %s", pBT_Status(status)));
		}
		else
		{
			Report(("SAPS APP: Incoming connection"));
		}
		break;

	case SAEVENT_SERVER_OPEN:
		Report(("SAPS APP: Connection open"));
		status = BTL_SAPS_GetConnectedDevice(sapsContext, &bdAddr);
		if (status == BT_STATUS_SUCCESS)
		{
			Report(("SAPS APP: Connected device: %s.", bdaddr_ntoa(&(bdAddr), addr)));
		}
		else
		{
			Report(("SAPS APP: BTL_SAPS_GetConnectedDevice returned: %s.", pBT_Status(status)));
		}
		break;

	case SAPS_EVENT_DISABLED:
		Report(("SAPS APP: BTL SAP Server Deregistered"));
		break;

	case SAEVENT_SERVER_CLOSED:
		Report(("SAPS APP: Connection closed"));
		break;

	default:
		Report(("SAPS event %d", (int)event->event));
		break;
	} 
} 


#else /*BTL_CONFIG_SAPS == BTL_CONFIG_ENABLED*/

BOOL SAPA_Init(void)
{
	Report(("SAPA_Init -BTL_CONFIG_SAPS is disabled "));
	return BT_STATUS_SUCCESS;
}
BOOL SAPA_Deinit(void)
{
	Report(("SAPA_Deinit  - BTL_CONFIG_SAPS is disabled"));
	return BT_STATUS_SUCCESS;
}

void SAPA_ProcessUserAction(U8 *msg)
{
	msg=msg;
	Report(("SAPS_APP is disabled via BTL_CONFIG."));

}



#endif /*BTL_CONFIG_SAPS == BTL_CONFIG_ENABLED*/



