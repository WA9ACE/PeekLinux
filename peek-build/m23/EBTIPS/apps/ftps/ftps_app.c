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
*   FILE NAME:      ftps_app.c
*
*   DESCRIPTION:	This file contains the implementation of the FTP Server application primitives (user actions)
*
*   AUTHOR:         Yaniv Rabin
*
\*******************************************************************************/
#include "btl_config.h"
#include "osapi.h"
#if BTL_CONFIG_FTPS == BTL_CONFIG_ENABLED


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

#include "goep.h"
#include "ftp.h"

/* BTL inclides */
#include "btl_ftps.h"
#include "lineparser.h"

#include "bthal_fs.h"

/* Internal Global variables for FTP Server*/
BtlFtpsContext			*ftpsContext = 0;
static BtSecurityLevel	securityLevel = BSL_NO_SECURITY;


/****************************************************************************
 *
 * Internal Function prototypes
 *
 ***************************************************************************/

void FTPSA_ProcessUserAction(U8 *msg);
void FTPSA_Init(void);
void FTPSA_Deinit(void);
static void FTPSA_Enable(void);
static void FTPSA_Disable(void);
static void FTPSA_Create(void);
static void FTPSA_Destroy(void);
static void FTPSA_Abort(void);
static void FTPSA_SetReadOnlyMode(void);
static void FTPSA_GetRootPath(void);
static void FTPSA_GetCurrentPath(void);
static void FTPSA_ConfigObjectRequest(void);
static void FTPSA_AcceptObjectRequest(void);
static void FTPSA_SetSecurityLevel(void);
static void FTPSA_GetSecurityLevel(void);
#if OBEX_AUTHENTICATION == XA_ENABLED
static void FTPSA_ConfigInitiatingObexAuthentication(void);
static void FTPSA_ObexAuthenticationResponse(void);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

static void FTPSA_CallBack(const BtlFtpsEvent *event);
static const char *FtpsaBtStatus(BtStatus Status);
static const char *GoepOpName(GoepOperation Op);
static const char *pLineParser_Status(I8 Status);

/****************************************************************************
 *
 * RAM data
 *
 ****************************************************************************/
#define FTPS_COMMANDS_NUM 16

typedef void (*FuncType)(void);

typedef struct _ftpsCommand
{
	char 		funcName[LINE_PARSER_MAX_STR_LEN];
	FuncType		funcPtr;
} FtpsCommand;

/* FTPS commands array */
static const  FtpsCommand ftpsCommands[] =  { {"init",	    	FTPSA_Init}, 		       
												{"deinit",      	FTPSA_Deinit}, 			
												{"abort",       	FTPSA_Abort}, 		     	
												{"setSec",      	FTPSA_SetSecurityLevel},   
												{"getSec",      	FTPSA_GetSecurityLevel},   
												{"enable",      	FTPSA_Enable}, 		    
												{"disable",     	FTPSA_Disable},		     	 
												{"create",      	FTPSA_Create},			    
												{"destroy",     	FTPSA_Destroy},			    	
												{"ObAuthResp",  	FTPSA_ObexAuthenticationResponse}, 		 
												{"ConfObAuth",		FTPSA_ConfigInitiatingObexAuthentication},
												{"configObjReq",  	FTPSA_ConfigObjectRequest}, 		 
												{"acceptObjReq",	FTPSA_AcceptObjectRequest},
												{"readOnly",		FTPSA_SetReadOnlyMode},			    
												{"getRootPath",		FTPSA_GetRootPath},
												{"getCurrentPath",	FTPSA_GetCurrentPath}			    
											};
#ifdef FTPS_COMMANDS_NUM
#undef FTPS_COMMANDS_NUM
#define FTPS_COMMANDS_NUM (sizeof(ftpsCommands) / sizeof(FtpsCommand))
#endif

#if OBEX_AUTHENTICATION == XA_ENABLED
static char	realm[20];		/* OBEX realm */
static char	password[20];	/* OBEX Authentication password */
static char	userId[20];		/* OBEX Authentication userId */
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

static char rootPath[LINE_PARSER_MAX_STR_LEN];
static BtlFtpsObjectRequestMask objectRequestMask;
static BOOL	accept;

/*******************************************************************************
 * 
 * FTPS PCO commands 
 *
 *******************************************************************************
 *
 * ftps abort 		    	- abort
 *
 * ftps setsec <seclevel>	- Set secutiry level. 
 *							 The <seclevel> parameter is a 10 base number representing the security level. 
 *							 (relevant only in security mode 2) For example:
 *					 	     3 - means BSL_AUTHENTICATION_IN and BSL_AUTHORIZATION_IN
 *						     25 - means BSL_AUTHENTICATION_IN and BSL_AUTHENTICATION_OUT and BSL_AUTHORIZATION_OUT which is 
 *						     (0x01 + 0x08 + 0x10) = (1 + 8 + 16)
 *
 *							 	0x01 :  BSL_AUTHENTICATION_IN  -  Authentication on incoming required 
 *							 	0x02 :  BSL_AUTHORIZATION_IN   -  Authorization on incoming required 
 *								0x04 : 	BSL_ENCRYPTION_IN      -  Encryption on incoming required 
 *
 *								0x08 :  BSL_AUTHENTICATION_OUT -  Authentication on outgoing required 
 *								0x10 :  BSL_AUTHORIZATION_OUT  -  Authorization on outgoing required 
 *								0x20 :  BSL_ENCRYPTION_OUT     -  Encryption on outgoing required 
 *								0x40 :  BSL_BROADCAST_IN       -  Reception of broadcast packets allowed 
 *
 *							To configure BSL_NO_SECURITY, insert 0.	
 *
 * ftps getsec 				- Get security level
 *
 * ftps enable 				- enable
 *
 * ftps disable 			- disable
 *
 * ftps create <seclevel>	- create. The meaning of <seclevel> is explained in setsec primitive. (not optional in this case) 
 * 
 * ftps destroy 			- destroy
 *
 * ftps obauthresp <password> <user id> -  OBEX authentication response. The parameters <password> <user id> are strings.
 *
 * ftps confobauth <auth enable> <password> <user id> <realm> -  OBEX authenticaiton configuration. <auth enable> is 0 or 1. Other parameters 
 *                                                           are strings. 
 *
 * ftps configObjReq <ObjReq>	- ConfigObjectRequest		
 *
 *								(0x00) : BTL_FTPS_OBJECT_REQUEST_NONE			- No request is delivered to app
 *								(0x01) : BTL_FTPS_OBJECT_REQUEST_PUSH			- Client push an object
 *								(0x02) : BTL_FTPS_OBJECT_REQUEST_PULL			- Client pull an object
 *								(0x04) : BTL_FTPS_OBJECT_REQUEST_DELETE			- Client delete an object
 *								(0x08) : BTL_FTPS_OBJECT_REQUEST_CREATE_FOLDER	- Client create a new folder
 *								(0xFF) : BTL_FTPS_OBJECT_REQUEST_ALL			- All requests are delivered to app
 *
 * ftps acceptObjReq <accept>	- AcceptObjectRequest TRUE - Accept , FALSE - Reject.
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 * 	
 * 
 *
 *******************************************************************************



*******************************************************************************
*  FUNCTION:	FTPSA_ProcessUserAction
*                                                                         
*  PARAMETERS:	msg - Parameters of actions.
*                                                                         
*  DESCRIPTION:	Processes primitives for FTPS Sample APP.
*                                                                         
*  RETURNS:		None.
*******************************************************************************/

void FTPSA_ProcessUserAction(U8 *msg)
{
	U8 i;
	char command[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS status;
	
	status = LINE_PARSER_GetNextStr((U8 *)command, LINE_PARSER_MAX_STR_LEN);

//	Report(("LINE_PARSER_GetNextStr() returned %s.",pLineParser_Status(status)));
	
	/* Look for the correct function */
	for(i = 0; i < FTPS_COMMANDS_NUM; i++)
	{
		if (OS_StrCmp(command, ftpsCommands[i].funcName) == 0)
		{
			ftpsCommands[i].funcPtr();
			return;
		}
	}
	Report(("Wrong FTPS command"));
	
}

/*---------------------------------------------------------------------------
 *            FTPSA_Init
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Create and Enable the FTP Server .
 *
 * Return:    void
 *
 */
void FTPSA_Init(void)
{
	BtStatus status;
	LINE_PARSER_STATUS lineStatus;
		
	OS_StrCpy(realm, "MyRealm");
	OS_StrCpy(password, "MyPassword");
	OS_StrCpy(userId, "MyUserId");

	lineStatus = LINE_PARSER_GetNextStr((U8 *)rootPath, LINE_PARSER_MAX_STR_LEN);
	
	Report(("LINE_PARSER_GetNextStr() returned %s.",pLineParser_Status(lineStatus)));

	if(lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		OS_StrCpy(rootPath, "/");
	}
	
	status =  BTL_FTPS_Create(
						(BtlAppHandle *) 0,
						FTPSA_CallBack,
						&securityLevel,
						&ftpsContext);
	
	Report(("BTL_FTPS_Create() returned %s.",FtpsaBtStatus(status)));

	status = BTL_FTPS_Enable(ftpsContext, 0, (const BtlUtf8 *)rootPath);
	
	Report(("BTL_FTPS_Enable() returned %s.",FtpsaBtStatus(status)));
	
}

/*---------------------------------------------------------------------------
 *            FTPSA_Deinit
 *---------------------------------------------------------------------------
 *
 * Synopsis: Disable and Distroy the FTP Server.
 *
 * Return:    void
 *
 */
void FTPSA_Deinit(void)
{
	BtStatus status;

	status = BTL_FTPS_Disable(ftpsContext);
	
	Report(("BTL_FTPS_Disable() returned %s.",FtpsaBtStatus(status)));

	status =  BTL_FTPS_Destroy(&ftpsContext);
	
	Report(("BTL_FTPS_Destroy() returned %s.",FtpsaBtStatus(status)));	

}

/*---------------------------------------------------------------------------
 *            FTPSA_Enable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Enable the FTP Server.
 *
 * Return:    void
 *
 */
void FTPSA_Enable(void)
{
	BtStatus status;
	LINE_PARSER_STATUS lineStatus;
	
	lineStatus = LINE_PARSER_GetNextStr((U8 *)rootPath, LINE_PARSER_MAX_STR_LEN);

	Report(("LINE_PARSER_GetNextStr() returned %s.",pLineParser_Status(lineStatus)));

	if(lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		OS_StrCpy(rootPath, "/");
	}
		
	status = BTL_FTPS_Enable(ftpsContext, 0, (const BtlUtf8 *)rootPath);
	
	Report(("BTL_FTPS_Enable() returned %s.",FtpsaBtStatus(status)));  	
}

/*---------------------------------------------------------------------------
 *            FTPSA_Disable
 *---------------------------------------------------------------------------
 *
 * Synopsis: Disable the FTP Server.
 *
 * Return:   void
 *
 */
void FTPSA_Disable(void)
{
	BtStatus status;

	status = BTL_FTPS_Disable(ftpsContext);
	
	Report(("BTL_FTPS_Disable() returned %s.",FtpsaBtStatus(status)));
	
}


/*---------------------------------------------------------------------------
 *            FTPSA_Create
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Create the FTP Server.
 *
 * Return:    void
 *
 */
void FTPSA_Create(void)
{
	BtStatus status;
	LINE_PARSER_STATUS lineStatus;

	if (TRUE == LINE_PARSER_AreThereMoreArgs() )
	{
		lineStatus = LINE_PARSER_GetNextU8(&securityLevel, TRUE);
		
		if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
		{
			Report(("FTPS: Can't read security level. Ignoring command"));
			Report(("LINE_PARSER_GetNextU8() returned %s.",pLineParser_Status(lineStatus)));
			return;
		}
	}
	
	status =  BTL_FTPS_Create(
						(BtlAppHandle *) 0,
						FTPSA_CallBack,
						&securityLevel,
						&ftpsContext);
	
	Report(("BTL_FTPS_Create() returned %s.",FtpsaBtStatus(status)));
}


/*---------------------------------------------------------------------------
 *            FTPSA_Destroy
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Destroy the FTP Server.
 *
 * Return:    void
 *
 */
void FTPSA_Destroy(void)
{
	BtStatus status;

	status =  BTL_FTPS_Destroy(&ftpsContext);
	
	Report(("BTL_FTPS_Destroy() returned %s.",FtpsaBtStatus(status)));

}

/*---------------------------------------------------------------------------
 *            FTPSA_Abort
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Requests the current operation be aborted.
 *
 * Return:    void
 *
 */
void FTPSA_Abort(void)
{
	BtStatus status;
	
	status = BTL_FTPS_Abort(ftpsContext, OBRC_CONFLICT);
	
	Report(("BTL_FTPS_Abort() returned %s.",FtpsaBtStatus(status)));
	
}

/*---------------------------------------------------------------------------
 *            FTPSA_SetReadOnlyMode
 *---------------------------------------------------------------------------
 *
 * Synopsis:  SetReadOnlyMode according to readOnly param.
 *
 * Return:    void
 *
 */
void FTPSA_SetReadOnlyMode(void)
{
	BtStatus status;
	BOOL readOnly;
	LINE_PARSER_STATUS lineStatus;
	
	lineStatus = LINE_PARSER_GetNextBool(&readOnly);

	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("FTPS: Can't read read only mode. Ignoring command"));
		Report(("LINE_PARSER_GetNextBool() returned %s.",pLineParser_Status(lineStatus)));
		return;
	}
	
	status = BTL_FTPS_SetReadOnlyMode(ftpsContext,readOnly);
	
	Report(("BTL_FTPS_SetReadOnlyMode() returned %s.",FtpsaBtStatus(status)));
	
}

/*---------------------------------------------------------------------------
 *            FTPSA_GetRootPath
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get the root path of the server.
 *
 * Return:    void
 *
 */
void FTPSA_GetRootPath(void)
{
	BtStatus	status;
	BtlUtf8 		*currentRootPath;
	
	status = BTL_FTPS_GetRootPath(ftpsContext,&currentRootPath);
	
	Report(("BTL_FTPS_GetRootPath() returned %s.",FtpsaBtStatus(status)));

	Report(("Root path is \"%s\".", currentRootPath));
	
}

/*---------------------------------------------------------------------------
 *            FTPSA_GetCurrentPath
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get the current path of the server.
 *
 * Return:    void
 *
 */
void FTPSA_GetCurrentPath(void)
{
	BtStatus	status;
	BtlUtf8		*currentPath;
	
	status = BTL_FTPS_GetCurrentPath(ftpsContext,&currentPath);
	
    Report(("BTL_FTPS_GetCurrentPath() returned %s.",FtpsaBtStatus(status)));

	Report(("Current path is \"%s\".", currentPath));
	
}

/*---------------------------------------------------------------------------
 *            FTPSA_ConfigObjectRequest
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Config the auto request.
 *
 * Return:    void
 *
 */
void FTPSA_ConfigObjectRequest()
{
	BtStatus status;
	LINE_PARSER_STATUS lineStatus;
	
	lineStatus = LINE_PARSER_GetNextU8(&objectRequestMask, TRUE);	

	Report(("FTPS: FTPSA_ConfigObjectRequest objectRequestMask = %x",objectRequestMask ));	

	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("FTPS: Can't read object request mask. Ignoring command"));
		Report(("LINE_PARSER_GetNextU8() returned %s.",pLineParser_Status(lineStatus)));	
		return;
	}
	
	status = BTL_FTPS_ConfigObjectRequest(ftpsContext,objectRequestMask );
	
    Report(("BTL_FTPS_ConfigObjectRequest() returned %s.",FtpsaBtStatus(status)));
	
}

/*---------------------------------------------------------------------------
 *            FTPSA_AcceptObjectRequest
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function must be called after receiving object request from the client,
 *				in order to accept or reject the request.
 *
 * Return:    BOOL
 *
 */
void FTPSA_AcceptObjectRequest(void)
{
	BtStatus	status;
	LINE_PARSER_STATUS lineStatus;
	
	lineStatus = LINE_PARSER_GetNextBool(&accept);

	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("FTPS: Can't read accept / reject . Ignoring command"));
		Report(("LINE_PARSER_GetNextBool() returned %s.",pLineParser_Status(lineStatus)));	
		return;
	}
	
	status = BTL_FTPS_AcceptObjectRequest(ftpsContext,accept);
	
    Report(("BTL_FTPS_AcceptObjectRequest() returned %s.",FtpsaBtStatus(status)));
}

/*---------------------------------------------------------------------------
 *            FTPSA_SetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets security level for the given FTPS context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
void FTPSA_SetSecurityLevel(void)
{
	BtStatus status;
	BtSecurityLevel newSecurityLevel = securityLevel;
	LINE_PARSER_STATUS lineStatus;
	
	lineStatus = LINE_PARSER_GetNextU8(&newSecurityLevel, TRUE);
	
	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("FTPS: Can't read security level. Ignoring command"));
		Report(("LINE_PARSER_GetNextU8() returned %s.",pLineParser_Status(lineStatus)));	
		return;
	}
	
	status = BTL_FTPS_SetSecurityLevel(ftpsContext, &newSecurityLevel);
	
	Report(("BTL_FTPS_SetSecurityLevel() returned %s.",FtpsaBtStatus(status)));

}

/*---------------------------------------------------------------------------
 *            FTPSA_GetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Gets security level for the given FTPS context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
void FTPSA_GetSecurityLevel(void)
{
	BtStatus status;
	BtSecurityLevel level;
	
	status = BTL_FTPS_GetSecurityLevel(ftpsContext, &level);
	
	Report(("BTL_FTPS_GetSecurityLevel() returned %s.",FtpsaBtStatus(status)));

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
}


#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 *            FTPSA_ConfigInitiatingObexAuthentication
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set the OBEX Client Authentication.
 *
 * Return:    void
 *
 */
void FTPSA_ConfigInitiatingObexAuthentication(void)
{
	BtStatus status;
	
	BOOL EnableobexAuth = TRUE;

	status = BTL_FTPS_ConfigInitiatingObexAuthentication(ftpsContext, EnableobexAuth, (const BtlUtf8 *)realm, (const BtlUtf8 *)userId, (const BtlUtf8 *)password);
	
	Report(("BTL_FTPS_ConfigInitiatingObexAuthentication() returned %s.",FtpsaBtStatus(status)));
}


/*---------------------------------------------------------------------------
 *            FTPSA_ObexAuthenticationResponse
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Responde to OBEX auth.
 *
 * Return:    void
 *
 */
void FTPSA_ObexAuthenticationResponse(void)
{
	BtStatus status;

	status = BTL_FTPS_ObexAuthenticationResponse(ftpsContext, (const BtlUtf8 *)userId, (const BtlUtf8 *)password);
	
	Report(("BTL_FTPS_ObexAuthenticationResponse() returned %s.",FtpsaBtStatus(status)));

}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */



/*---------------------------------------------------------------------------
 *            FTPSA_CallBack
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes OBEX Server protocol events.
 *
 * Return:    void
 *
 */
static void FTPSA_CallBack(const BtlFtpsEvent *event)
{
	BD_ADDR bdAddr;
	char addr[BDADDR_NTOA_SIZE];

    /* First process only events that are not "object" related. */
    switch ( event->ftpsEvent->event) {
    case FTPS_EVENT_TP_CONNECTED:
		BTL_FTPS_GetConnectedDevice(ftpsContext, &bdAddr);
        UI_DISPLAY(("FTP Server: Transport Connection is up to %s.", bdaddr_ntoa(&bdAddr, addr)));
        break;

    case FTPS_EVENT_TP_DISCONNECTED:
        UI_DISPLAY(("FTP Server: Transport Connection has been disconnected."));
        break;

#if OBEX_AUTHENTICATION == XA_ENABLED

    case FTPS_EVENT_AUTH_CHALLENGE:
        UI_DISPLAY(("FTP Server: Received Authenticate Challenge for %s operation.", 
            GoepOpName(event->ftpsEvent->oper)));
#if OBEX_MAX_REALM_LEN > 0
        Report(("FTP Server: Realm: %s, charset %d , Options %d.", 
            event->ftpsEvent->info.challenge.realm+1, event->ftpsEvent->info.challenge.realm[0], event->ftpsEvent->info.challenge.options));
#endif /* OBEX_MAX_REALM_LEN > 0 */

//		FTPSA_ObexAuthenticationResponse();
        break;        

#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
    
	case FTPS_EVENT_START:
        UI_DISPLAY(("FTP Server: Receiving a %s operation.", GoepOpName(event->ftpsEvent->oper)));
	    break;

    case FTPS_EVENT_ABORTED:
        UI_DISPLAY(("FTPServer: %s operation aborted.", GoepOpName(event->ftpsEvent->oper)));
		break;

    case FTPS_EVENT_COMPLETE:
        UI_DISPLAY(("FTPServer: %s operation complete.", GoepOpName(event->ftpsEvent->oper)));
        break;

	case FTPS_EVENT_OBJECT_PROGRESS:
		Report(("FTPServer: progress bar %d / %d",
						event->ftpsEvent->info.progress.currPos, event->ftpsEvent->info.progress.maxPos));
        break;

	case FTPS_EVENT_OBJECT_REQUEST:
		Report(("FTP Object request: operation \"%s\".", GoepOpName(event->ftpsEvent->oper)));
		Report(("FTP Object request: name \"%s\".", event->ftpsEvent->info.request.name));
		Report(("FTP Object request: length \"%d\".", event->ftpsEvent->info.request.objectLen));
		UI_DISPLAY(("FTP Please accept or reject!"));
        break;
    }
}

/*---------------------------------------------------------------------------
 *            GoepOpName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return a pointer to the name of the current server operation.
 *
 * Return:    ASCII String pointer.
 *
 */
static const char *GoepOpName(GoepOperation Op)
{
   switch (Op) {
   case GOEP_OPER_PUSH:
       return "Push";
   case GOEP_OPER_PULL:
       return "Pull";
   case GOEP_OPER_SETFOLDER:
       return "Set Folder";
   case GOEP_OPER_CONNECT:
       return "Connect";
   case GOEP_OPER_DISCONNECT:
       return "Disconnect";
   case GOEP_OPER_DELETE:
       return "Delete";
   case GOEP_OPER_ABORT:
       return "Abort";
   }
   return "Unknown";
}

/*---------------------------------------------------------------------------
 *            General Purpose Debug functions
 *---------------------------------------------------------------------------
 */
static const char *FtpsaBtStatus(BtStatus Status)
{
    switch (Status) {
    case BT_STATUS_SUCCESS:
        return ("Success");
    case BT_STATUS_FAILED:
        return ("Failed");
    case BT_STATUS_PENDING:
        return ("Pending");
    case BT_STATUS_BUSY:
        return ("Busy");
    case BT_STATUS_NO_RESOURCES:
        return ("No Resources");
    case BT_STATUS_NOT_FOUND:
        return ("Not Found");
    case BT_STATUS_DEVICE_NOT_FOUND:
        return ("Device Not Found");
    case BT_STATUS_CONNECTION_FAILED:
        return ("Connection Failed");
    case BT_STATUS_TIMEOUT:
        return ("Timeout");
    case BT_STATUS_NO_CONNECTION:
        return ("No Connection");
    case BT_STATUS_INVALID_PARM:
        return ("Invalid parameter");
    case BT_STATUS_IN_PROGRESS:
        return ("In Progress");
    case BT_STATUS_RESTRICTED:
        return ("Restricted");
    case BT_STATUS_INVALID_TYPE:
        return ("Invalid Type");
    case BT_STATUS_HCI_INIT_ERR:
        return ("HCI Initialization Error");
    case BT_STATUS_NOT_SUPPORTED:
        return ("Not Supported");
    case BT_STATUS_IN_USE:
        return ("In Use");
    case BT_STATUS_SDP_CONT_STATE:
        return ("SDP Continue");
    case BT_STATUS_CANCELLED:
        return ("Cancelled");
    }
    return ("UNKNOWN");
}

/*---------------------------------------------------------------------------
 *            General Purpose Line parser status
 *---------------------------------------------------------------------------
 */
const char *pLineParser_Status(I8 Status)
{
    switch (Status) {
    case LINE_PARSER_STATUS_SUCCESS:
        return ("Success");
    case LINE_PARSER_STATUS_FAILED:
        return ("Failed");
    case LINE_PARSER_STATUS_ARGUMENT_TOO_LONG:
        return ("Argument too long");
    case LINE_PARSER_STATUS_NO_MORE_ARGUMENTS:
        return ("No more arguments");
    }
    return ("UNKNOWN");
}


#else /* BTL_CONFIG_FTPS == BTL_CONFIG_ENABLED*/

void FTPSA_Init(void)
{
	Report(("FTPSA_Init -BTL_CONFIG_FTPS is disabled "));
}
void FTPSA_Deinit(void)
{
	Report(("FTPSA_Deinit  - BTL_CONFIG_FTPS is disabled"));
}

void FTPSA_ProcessUserAction(U8 *msg)
{
	msg=msg;
	Report(("FTPC_APP is disabled via BTL_CONFIG."));

}



#endif /* BTL_CONFIG_FTPS == BTL_CONFIG_ENABLED */


