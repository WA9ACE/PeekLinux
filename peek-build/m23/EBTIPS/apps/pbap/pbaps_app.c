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
*   FILE NAME:      pbaps_app.c
*
*   DESCRIPTION:    This file contains the PBAP server application in the neptune
*					platform.
*
*
*   AUTHOR:         Yoni Shavit
*
\*******************************************************************************/

#include "btl_config.h"
#include "bttypes.h"
#include "osapi.h"
#if BTL_CONFIG_PBAPS == BTL_CONFIG_ENABLED

 


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
#include "btl_pbaps.h"
#include "lineparser.h"

BtlPbapsContext	*pbapsContext = 0;
BtSecurityLevel btlPbapSecurityLevel = BSL_NO_SECURITY;

/****************************************************************************
 *
 * Internal Function prototypes
 *
 ***************************************************************************/

void PBAPA_ProcessUserAction(U8 *msg);
BOOL PBAPA_Init(void);
BOOL PBAPA_Deinit(void);
BOOL APP_PBAPS_Create(void);
BOOL APP_PBAPS_Destroy(void);
BOOL APP_PBAPS_Enable(void);
BOOL APP_PBAPS_Disable(void);
BOOL APP_PBAPS_Abort(void);
BOOL APP_PBAPS_Disconnect(void);
BOOL APP_PBAPS_SetSecurityLevel(void);
BOOL APP_PBAPS_GetSecurityLevel(void);
#if OBEX_AUTHENTICATION == XA_ENABLED
BOOL APP_PBAPS_ConfigInitiatingObexAuthentication(void);
BOOL APP_PBAPS_ObexAuthenticationResponse(void);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

static void APP_PBAPS_CallBack(const BtlPbapsEvent *event);
static const char *PbapOpName(PbapOp Op);

static const char *pObStatus(ObStatus status);
static const char *GoepOpName(GoepOperation Op);


/****************************************************************************
 *
 * RAM data
 *
 ****************************************************************************/
#define PBAP_COMMANDS_NUM 10

typedef BOOL (*FuncType)(void);

typedef struct
{
	char 		funcName[LINE_PARSER_MAX_STR_LEN];
	FuncType    funcPtr;
} _BtlPbapCommands;

/* PBAP commands array */
static const  _BtlPbapCommands btlPbapCommands[] =  {   
														{"abort",       APP_PBAPS_Abort}, 		     	
														{"setsec",      APP_PBAPS_SetSecurityLevel},   
														{"getsec",      APP_PBAPS_GetSecurityLevel},   
														{"enable",      APP_PBAPS_Enable}, 		    
														{"disable",     APP_PBAPS_Disable},		     	 
														{"create",      APP_PBAPS_Create},			    
														{"destroy",     APP_PBAPS_Destroy},
														{"disconnect",  APP_PBAPS_Disconnect},
														{"obauthresp",  APP_PBAPS_ObexAuthenticationResponse}, 		 
														{"confobauth", APP_PBAPS_ConfigInitiatingObexAuthentication}  
											        };
 
#if OBEX_AUTHENTICATION == XA_ENABLED
static char              pbapAppRealm[6];	   /* OBEX realm */
static char              pbapAppPassword[6]; /* OBEX Authentication password */
static char              pbapAppUserId[6];   /* OBEX Authentication userId */
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

/*******************************************************************************
 * 
 * PBAP PCO commands 
 *
 *******************************************************************************
 *
 * pbaps abort 		    - abort
 *
 * pbaps setsec <seclevel> - Set secutiry level. 
 *							 The <seclevel> parameter is a 10 base number representing the security level. 
 *							 (relevant only in security mode 2) For example:
 *					 	     3 - means BSL_AUTHENTICATION_IN and BSL_AUTHORIZATION_IN
 *						     25 - means BSL_AUTHENTICATION_IN and BSL_AUTHENTICATION_OUT and BSL_AUTHORIZATION_OUT which is 
 *						     (0x01 + 0x08 + 0x10) = (1 + 8 + 16)
 *
 *							 	0x01 :  BSL_AUTHENTICATION_IN  -  Authentication on incoming required 
 *							 	0x02 :  BSL_AUTHORIZATION_IN   -  Authorization on incoming required 
 *								0x04:  BSL_ENCRYPTION_IN      -  Encryption on incoming required 
 *
 *								0x08:  BSL_AUTHENTICATION_OUT  -  Authentication on outgoing required 
 *								0x10:  BSL_AUTHORIZATION_OUT   -  Authorization on outgoing required 
 *								0x20:  BSL_ENCRYPTION_OUT      -  Encryption on outgoing required 
 *								0x40:  BSL_BROADCAST_IN        -  Reception of broadcast packets allowed 
 *
 *							To configure BSL_NO_SECURITY, insert 0.	
 *
 * pbaps getsec 			- Get security level
 *
 * pbaps enable 			- enable
 *
 * pbaps disable 			- disable
 *
 * pbaps create <seclevel>	- create. The meaning of <seclevel> is explained in setsec primitive. (not optional in this case) 
 * 
 * pbaps destroy 			- destroy
 *
 * pbaps obauthresp <password> <user id> -  OBEX authentication response. The parameters <password> <user id> are strings.
 *
 * pbaps confobauth <auth enable> <password> <user id> <realm> -  OBEX authenticaiton configuration. <auth enable> is 0 or 1. Other parameters 
                                                            are strings. 
 *
 *
 *******************************************************************************


*******************************************************************************
*  FUNCTION:	PBAPA_ProcessUserAction
*                                                                         
*  PARAMETERS:	msg - Parameters of actions.
*                                                                         
*  DESCRIPTION:	Processes primitives for the PBAP application
*                                                                         
*  RETURNS:		None.
*******************************************************************************/
void PBAPA_ProcessUserAction(U8 *msg)
{
	U8 i;
	char command[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS status;
	
	status = LINE_PARSER_GetNextStr((U8 *)command, LINE_PARSER_MAX_STR_LEN);

	if (status != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("Wrong PBAP command. Can't parse line"));
		return;
	}

	/* Look for the correct function */
	for(i = 0; i < PBAP_COMMANDS_NUM; i++)
	{
		if (OS_StrCmp(command, btlPbapCommands[i].funcName) == 0)
		{
			btlPbapCommands[i].funcPtr();
			return;
		}
	}
	Report(("Wrong PBAP command"));
	
}

/*---------------------------------------------------------------------------
 *            PBAPA_Init
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Init the PBAP server.		
 *
 * Return:    TRUE if initialized, FALSE otherwise
 *
 */
BOOL PBAPA_Init(void)
{
	BtStatus status;
	BtlPbapsRepositoriesMask reposMask = (BTL_PBAPS_LOCAL_PHONEBOOK | BTL_PBAPS_SIM_CARD);
	
	status = BTL_PBAPS_Create(0, APP_PBAPS_CallBack, 0, &pbapsContext);

	Report(("BTL_PBAPS_Create() returned %s.",pBT_Status(status)));

	status = BTL_PBAPS_Enable(pbapsContext, 0, &reposMask);
		
    Report(("BTL_PBAPS_Enable() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
	
}

/*---------------------------------------------------------------------------
 *            PBAPA_Deinit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize and Destroy the PBAP server.
 *
 * Return:    TRUE if initialized, FALSE otherwise
 *
 */
BOOL PBAPA_Deinit(void)
{
	BtStatus status;

	status = BTL_PBAPS_Disable(pbapsContext);
	
    Report(("BTL_PBAPS_Disable() returned %s.",pBT_Status(status)));
	
	status =  BTL_PBAPS_Destroy(&pbapsContext);
	
    Report(("BTL_PBAPS_Destroy() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}


/*---------------------------------------------------------------------------
 *            APP_PBAPS_Abort
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Aborts the current server operation.
 *
 * Return:    void
 *
 */
BOOL APP_PBAPS_Abort(void)
{
	BtStatus status;
	
	status = BTL_PBAPS_Abort(pbapsContext, PBRC_UNAUTHORIZED);
	
    Report(("BTL_PBAPS_Abort() returned %s.",pBT_Status(status)));
	
	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

BOOL APP_PBAPS_Disconnect(void)
{
	BtStatus status;
	
	status = BTL_PBAPS_Disconnect(pbapsContext);
	
    Report(("BTL_PBAPS_Disconnect() returned %s.",pBT_Status(status)));
	
	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

/*---------------------------------------------------------------------------
 *            APP_PBAPS_Enable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Enable the PBAP server.
 *
 * Return:    TRUE if initialized, FALSE otherwise
 *
 */
BOOL APP_PBAPS_Enable(void)
{
	BtStatus status;
	BtlPbapsRepositoriesMask reposMask = (BTL_PBAPS_LOCAL_PHONEBOOK | BTL_PBAPS_SIM_CARD);
		
	status = BTL_PBAPS_Enable(pbapsContext, 0, &reposMask);
		
    Report(("BTL_PBAPS_Enable() returned %s.",pBT_Status(status)));
	
	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

/*---------------------------------------------------------------------------
 *            APP_PBAPS_Disable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disable the PBAP server.
 *
 * Return:    TRUE if Disable, FALSE otherwise
 *
 */
BOOL APP_PBAPS_Disable(void)
{
	BtStatus status;
	
	status = BTL_PBAPS_Disable(pbapsContext);
	
    Report(("BTL_PBAPS_Disable() returned %s.",pBT_Status(status)));
	
	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

/*---------------------------------------------------------------------------
 *            APP_PBAPS_Create
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Create the PBAP server.		
 *
 * Return:    TRUE if initialized, FALSE otherwise
 *
 */
BOOL APP_PBAPS_Create(void)
{
	BtStatus status;
	LINE_PARSER_STATUS parserStatus;

	if (TRUE == LINE_PARSER_AreThereMoreArgs() )
	{
		parserStatus = LINE_PARSER_GetNextU8(&btlPbapSecurityLevel, TRUE);
		
		if (parserStatus != LINE_PARSER_STATUS_SUCCESS)
		{
			Report(("PBAP: Can't read security level. Ignoring command"));
			return FALSE;
		}
	}
	status = BTL_PBAPS_Create(0, APP_PBAPS_CallBack, &btlPbapSecurityLevel, &pbapsContext);

	Report(("BTL_PBAPS_Create() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

/*---------------------------------------------------------------------------
 *            APP_PBAPS_Destroy
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Destroy the PBAP server.		
 *
 * Return:    TRUE if initialized, FALSE otherwise
 *
 */
BOOL APP_PBAPS_Destroy(void)
{
	BtStatus status;
	
	status = BTL_PBAPS_Destroy(&pbapsContext);

	Report(("BTL_PBAPS_Destroy() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}


/*---------------------------------------------------------------------------
 *            APP_PBAPS_SetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets security level for the given PBAPS context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
BOOL APP_PBAPS_SetSecurityLevel()
{
	BtStatus status;
	BOOL retVal = FALSE;
	BtSecurityLevel newSecurityLevel = btlPbapSecurityLevel;
	LINE_PARSER_STATUS parserStatus;
	
	parserStatus = LINE_PARSER_GetNextU8(&newSecurityLevel, TRUE);
	
	if (parserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("PBAP: Can't read security level. Ignoring command"));
		return FALSE;
	}
	

	status = BTL_PBAPS_SetSecurityLevel(pbapsContext, &newSecurityLevel);
	
    Report(("BTL_PBAPS_SetSecurityLevel() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (retVal) : (!retVal));
}

/*---------------------------------------------------------------------------
 *            APP_PBAPS_GetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Gets security level for the given PBAPS context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
BOOL APP_PBAPS_GetSecurityLevel(void)
{
	BtStatus status;
	BtSecurityLevel level;
	
	status = BTL_PBAPS_GetSecurityLevel(pbapsContext, &level);
	
    Report(("BTL_PBAPS_GetSecurityLevel() returned %s.",pBT_Status(status)));

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


#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 *            APP_PBAPS_ConfigInitiatingObexAuthentication
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set the OBEX Client Authentication.
 *
 * Return:    void
 *
 */
BOOL APP_PBAPS_ConfigInitiatingObexAuthentication(void)
{
	BtStatus status;
	
	BOOL EnableobexAuth = TRUE;

	status = BTL_PBAPS_ConfigInitiatingObexAuthentication(pbapsContext, EnableobexAuth, (const BtlUtf8 *)pbapAppRealm, (const BtlUtf8 *)pbapAppUserId, (const BtlUtf8 *)pbapAppPassword);
	
    Report(("BTL_PBAPS_ConfigInitiatingObexAuthentication() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}


/*---------------------------------------------------------------------------
 *            APP_PBAPS_ObexAuthenticationResponse
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Responde to OBEX auth.
 *
 * Return:    void
 *
 */
BOOL APP_PBAPS_ObexAuthenticationResponse(void)
{
	BtStatus status;

	status = BTL_PBAPS_ObexAuthenticationResponse(pbapsContext, (const BtlUtf8 *)pbapAppUserId, (const BtlUtf8 *)pbapAppPassword);
	
    Report(("BTL_PBAPS_ObexAuthenticationResponse() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */


/*---------------------------------------------------------------------------
 *            APP_PBAPS_CallBack
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes OBEX server protocol events.
 *
 * Return:    void
 *
 */
static void APP_PBAPS_CallBack(const BtlPbapsEvent *event)
{
	BD_ADDR 				bdAddr;
	char 					addr[BDADDR_NTOA_SIZE];
	
    switch (event->callbackParms->event) {

#if OBEX_AUTHENTICATION == XA_ENABLED
    case PBAP_EVENT_AUTH_RESULT:
        if (event->callbackParms->u.auth.result) 
		{
            Report(("Server: PBAP Authentication succeeded."));
            Assert(event->callbackParms->u.auth.reason == PBAP_AUTH_NO_FAILURE);
        } else 
        {
            if (event->callbackParms->u.auth.reason == PBAP_AUTH_NO_RCVD_AUTH_RESP) {
                Report(("Server: PBAP Authentication failed: No Auth Resp Received."));
            } else if (event->callbackParms->u.auth.reason == PBAP_AUTH_INVALID_CREDENTIALS) {
                Report(("Server: PBAP Authentication failed: Invalid Credentials."));
            } else {
                Report(("Server: Error - wrong OBEX auth reason"));
            }
            Report(("Server: PBAP Client must retry Connect."));
        }
        break;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

   
    case PBAP_EVENT_TP_CONNECTED:
		Report(("Server: PBAP Transport Connection has come up."));
		Report(("BTL_PBAPS_GetConnectedDevice() returnes %s",pBT_Status(BTL_PBAPS_GetConnectedDevice(pbapsContext, &bdAddr))));
		Report(("Server: Transport Connection is up to %s.", bdaddr_ntoa(&bdAddr, addr)));
#if OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED
        if (event->callbackParms->u.connect.profileVersion) {
            Report(("Server: PBAP Client profile version %x.%x", (U8)(event->callbackParms->u.connect.profileVersion >> 8), 
                       (U8)(event->callbackParms->u.connect.profileVersion)));
        }
#endif /* OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED */   
        break;

    case PBAP_EVENT_TP_DISCONNECTED:
        Report(("Server: PBAP Transport Connection has been disconnected."));
        break;

    case PBAP_EVENT_START:
        if (event->callbackParms->oper == PBAPOP_NONE) {
            /* We don't know the exact operation type yet. We will need to get
             * more information from the headers to determine this.
             */
            Report(("Server: PBAP Receiving an operation"));
        } else {
            Report(("Server: PBAP Receiving a %s .", PbapOpName(event->callbackParms->oper)));
        }
        break;
		
    case PBAP_EVENT_ABORTED:
        Report(("Server: PBAP %s aborted.", PbapOpName(event->callbackParms->oper)));
        break;
		
#if OBEX_AUTHENTICATION == XA_ENABLED	
	case PBAP_EVENT_AUTH_CHALLENGE:
		Report(("Server: Received Authenticate Challenge for %s operation.", 
            PbapOpName(event->callbackParms->oper)));
#if OBEX_MAX_REALM_LEN > 0
        Report(("Server: Realm: %s, charset %d , Options %d.", 
            event->callbackParms->u.challenge.realm+1, event->callbackParms->u.challenge.realm[0], event->callbackParms->u.challenge.options));
#endif /* OBEX_MAX_REALM_LEN > 0 */
		BTL_PBAPS_ObexAuthenticationResponse(pbapsContext, (const BtlUtf8 *)pbapAppUserId, (const BtlUtf8 *)pbapAppPassword);
        break;    
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

    case PBAP_EVENT_COMPLETE:
        Report(("Server: PBAP %s complete.", PbapOpName(event->callbackParms->oper)));
        break;

    default:
        Report(("Server: PBAP Unexpected event: %i", event->callbackParms->event));
        break;
    }
}

/*---------------------------------------------------------------------------
 *            pObStatus
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return a pointer to a description of the OBEX status code.
 *
 * Return:    ASCII String pointer.
 *
 */
static const char *pObStatus(ObStatus status)
{
    switch (status) {
    case OB_STATUS_SUCCESS:
        return "Success";
    case OB_STATUS_FAILED:
        return "Failed";
    case OB_STATUS_PENDING:
        return "Pending";
    case OB_STATUS_DISCONNECT:
        return "Disconnect";
    case OB_STATUS_NO_CONNECT:
        return "No Connection";
    case OB_STATUS_MEDIA_BUSY:
        return "Media Busy";
    case OB_STATUS_INVALID_PARM:
        return "Invalid Parameter";
    case OB_STATUS_INVALID_HANDLE:
        return "Invalid Handle";
    case OB_STATUS_PACKET_TOO_SMALL:
        return "Packet Too Small";
    case OB_STATUS_BUSY:
       return "Status Busy";
    }
    return "UNKNOWN";
}
/*---------------------------------------------------------------------------
 *            PbapOpName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return a pointer to the name of the current server operation.
 *
 * Return:    ASCII String pointer.
 *
 */
static const char *PbapOpName(PbapOp Op)
{
    switch (Op) {
    case PBAPOP_NONE:
        return "None";
    case PBAPOP_CONNECT:
        return "Connect";
    case PBAPOP_DISCONNECT:
        return "Disconnect";
    case PBAPOP_PULL:
        return "Pull";
    case PBAPOP_PULL_PHONEBOOK:
        return "Pull Phonebook";
    case PBAPOP_SET_PHONEBOOK:
        return "Set Phonebook";
    case PBAPOP_PULL_VCARD_LISTING:
        return "Pull vCard Listing";
    case PBAPOP_PULL_VCARD_ENTRY:
        return "Pull vCard Entry";
    }
    return "UNKNOWN";
}


#else /* BTL_CONFIG_PBAPS == BTL_CONFIG_ENABLED */

BOOL PBAPA_Init(void)
{
	Report(("PBAPA_Init -BTL_CONFIG_PBAPS is disabled "));
	return BT_STATUS_SUCCESS;
}
BOOL PBAPA_Deinit(void)
{
	Report(("PBAPA_Deinit  - BTL_CONFIG_PBAPS is disabled"));
	return BT_STATUS_SUCCESS;
}

void PBAPA_ProcessUserAction(U8 *msg)
{
	msg=msg;
	Report(("PBAPS_APP is disabled via BTL_CONFIG."));

}


#endif /* BTL_CONFIG_PBAPS == BTL_CONFIG_ENABLED */
