
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
*   FILE NAME:      ftpc_app.c
*
*   DESCRIPTION:	This file contains the implementation of the FTP Client application primitives (user actions)
*
*   AUTHOR:         Yaniv Rabin
*
\*******************************************************************************/ 

#include "btl_config.h"
#include "osapi.h"
#if BTL_CONFIG_FTPC == BTL_CONFIG_ENABLED


#include "btl_common.h"
#include "debug.h"

#include "../app_main.h"

#include "btl_ftpc.h"
#include "btl_ftpc_xmlprs.h"
#include "btl_config.h"
#include "bthal_config.h"
#include "bthal_fs.h"
#include "lineparser.h"

/* Internal Global variables for FTP Client*/
BtlFtpcContext			*ftpcContext = 0;
static BtSecurityLevel	securityLevel = BSL_NO_SECURITY;

#define XML_PARSER_BUFFER_SIZE 20000
#define FTPC_APP_MAX_FILE_LEN	256
#define	FTPC_APP_MAX_PATH_LEN	256

/****************************************************************************
 *
 * Internal Function prototypes
 *
 ***************************************************************************/

void FTPCA_ProcessUserAction(U8 *msg);
void FTPCA_Init(void);
void FTPCA_Deinit(void);
static void FTPCA_Enable(void);
static void FTPCA_Disable(void);
static void FTPCA_Destroy(void);
static void FTPCA_Create(void);
static void FTPCA_Abort(void);
static void FTPCA_Connect(void);
static void FTPCA_Disconnect(void);
static void FTPCA_PushFile(void);
static void FTPCA_PullFile(void);
static void FTPCA_Delete(void);
static void FTPCA_PullListFolder(void);
static void FTPCA_PullListFolderFS(void);
static void FTPCA_SetFolder(void);
static void FTPCA_SetFolderBackUp(void);
static void FTPCA_SetFolderRoot(void);
static void FTPCA_ParseFolderListing();
static void FTPCA_SetSecurityLevel(void);
static void FTPCA_GetSecurityLevel(void);
#if OBEX_AUTHENTICATION == XA_ENABLED
static void FTPCA_ConfigInitiatingObexAuthentication(void);
static void FTPCA_ObexAuthenticationResponse(void);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
static void FTPCA_CallBack(const BtlFtpcEvent *event);
static const char *FtpcaBtStatus(BtStatus Status);
static const char *GoepOpName(GoepOperation Op);
static const char *pLineParser_Status(I8 Status);
static const char *ClientAbortMsg(ObexRespCode Reason);
static void findFileName(const char *source, char *dest);
static void AppPrintEntryAttibutes(BtlFtpcParsedEntry parsedEntry);
static void AppPrintEntryPermissisons(BtlFtpcParsedEntyPerm entryPermissions);

/****************************************************************************
 *
 * RAM data
 *
 ****************************************************************************/
typedef void (*FuncType)(void);

typedef struct _FtpcCommand
{
	char 		funcName[LINE_PARSER_MAX_STR_LEN];
	FuncType	funcPtr;
} FtpcCommand;

/* FTPC commands array */
static const  FtpcCommand ftpcCommands[] =  { {"init",	    	FTPCA_Init}, 		       
												{"deinit",      	FTPCA_Deinit}, 			
												{"abort",       	FTPCA_Abort}, 		     	
												{"setSec",      	FTPCA_SetSecurityLevel},   
												{"getSec",      	FTPCA_GetSecurityLevel},   
												{"enable",      	FTPCA_Enable}, 		    
												{"disable",     	FTPCA_Disable},		     	 
												{"create",      	FTPCA_Create},			    
												{"destroy",     	FTPCA_Destroy},			    	
												{"ObAuthResp",  	FTPCA_ObexAuthenticationResponse}, 		 
												{"ConfObAuth",		FTPCA_ConfigInitiatingObexAuthentication},
												{"connect",			FTPCA_Connect},			    
												{"disconnect",		FTPCA_Disconnect},			    	
												{"pushFile",		FTPCA_PushFile}, 		 
												{"pullFile",		FTPCA_PullFile},
												{"delete",			FTPCA_Delete},			    
												{"pullListFolder",	FTPCA_PullListFolder},			    	
												{"pullListFolderFS",FTPCA_PullListFolderFS},			    	
												{"setFolder",		FTPCA_SetFolder}, 		 
												{"setFolderBackup",	FTPCA_SetFolderBackUp},
												{"setFolderRoot",	FTPCA_SetFolderRoot},
												{"parseFolderListing",	FTPCA_ParseFolderListing}													
											 };

#define FTPC_COMMANDS_NUM (sizeof(ftpcCommands) / sizeof(FtpcCommand))

#if OBEX_AUTHENTICATION == XA_ENABLED
static char	realm[20];		/* OBEX realm */
static char	password[20];	/* OBEX Authentication password */
static char	userId[20];		/* OBEX Authentication userId */
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

static BD_ADDR	bdAddr;
static char		fullPathFileToPush[FTPC_APP_MAX_PATH_LEN + FTPC_APP_MAX_FILE_LEN];
static char		fileToPull[FTPC_APP_MAX_FILE_LEN];
static char		objToDelete[FTPC_APP_MAX_FILE_LEN];
static char		folderName[FTPC_APP_MAX_PATH_LEN];
static char		fullPathPull[FTPC_APP_MAX_PATH_LEN];
static char		pathToPullListFolder[FTPC_APP_MAX_PATH_LEN];
static char		ftpPullFolderListingFileName[] =	{"folder_listing.txt"};

static	BtlObject	folderListing;



/*******************************************************************************\
*  FUNCTION:	FTPCA_ProcessUserAction
*                                                                         
*  PARAMETERS:	msg - Parameters of actions.
*                                                                         
*  DESCRIPTION:	Processes primitives for FTPC Sample APP.
*                                                                         
*  RETURNS:		None.
\*******************************************************************************/

void FTPCA_ProcessUserAction(U8 *msg)
{
	U8 i;
	char command[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS status;
	
	status = LINE_PARSER_GetNextStr((U8 *)command, LINE_PARSER_MAX_STR_LEN);

//	Report(("LINE_PARSER_GetNextStr() returned %s.",pLineParser_Status(status)));
	
	/* Look for the correct function */
	for(i = 0; i < FTPC_COMMANDS_NUM; i++)
	{
		if (OS_StrCmp(command, ftpcCommands[i].funcName) == 0)
		{
			ftpcCommands[i].funcPtr();
			return;
		}
	}
	Report(("Wrong FTPC command"));
	
}

/*---------------------------------------------------------------------------
 *            FTPCA_Init
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Create and Enable the FTP Client .
 *
 * Return:    TRUE if initialized, FALSE otherwise
 *
 */
void FTPCA_Init(void)
{
	BtStatus status;
	LINE_PARSER_STATUS lineStatus;
		
	OS_MemCopy(realm, "MyRealm", 8);
	OS_MemCopy(password, "MyPassword", 11);
	OS_MemCopy(userId, "MyUserId", 9);

	status =  BTL_FTPC_Create(0, FTPCA_CallBack, &securityLevel, &ftpcContext);
	
	Report(("BTL_FTPC_Create() returned %s.",FtpcaBtStatus(status)));
	
	status = BTL_FTPC_Enable(ftpcContext);
		
	Report(("BTL_FTPC_Enable() returned %s.",FtpcaBtStatus(status)));
	
}

/*---------------------------------------------------------------------------
 *            FTPCA_Deinit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disable and Distroy the FTP Client.
 *
 * Return:    void
 *
 */
void FTPCA_Deinit(void)
{
	BtStatus status;

	status = BTL_FTPC_Disable(ftpcContext);
	
	Report(("BTL_FTPC_Disable() returned %s.",FtpcaBtStatus(status)));

	status =  BTL_FTPC_Destroy(&ftpcContext);
	
	Report(("BTL_FTPC_Destroy() returned %s.",FtpcaBtStatus(status)));

}

/*---------------------------------------------------------------------------
 *            FTPCA_Enable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Enable the FTP Client.
 *
 * Return:    void.
 *
 */
void FTPCA_Enable(void)
{
	BtStatus status;
		
	status = BTL_FTPC_Enable(ftpcContext);
		
	Report(("BTL_FTPC_Enable() returned %s.",FtpcaBtStatus(status)));	
	
}

/*---------------------------------------------------------------------------
 *            FTPCA_Disable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disable the FTP Client.
 *
 * Return:   void
 *
 */
void FTPCA_Disable(void)
{
	BtStatus status;

	status = BTL_FTPC_Disable(ftpcContext);
	
	Report(("BTL_FTPC_Disable() returned %s.",FtpcaBtStatus(status)));
	
}

/*---------------------------------------------------------------------------
 *            FTPCA_Create
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Create the FTP Client.
 *
 * Return:    void
 *
 */
void FTPCA_Create(void)
{
	BtStatus status;
	LINE_PARSER_STATUS lineStatus;

	if (TRUE == LINE_PARSER_AreThereMoreArgs() )
	{
		lineStatus = LINE_PARSER_GetNextU8(&securityLevel, TRUE);
		
		if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
		{
			Report(("FTPC: Can't read security level. Ignoring command"));
			Report(("LINE_PARSER_GetNextU8() returned %s.",pLineParser_Status(lineStatus)));
			return;
		}
	}
	
	status =  BTL_FTPC_Create(0, FTPCA_CallBack, &securityLevel, &ftpcContext);
	
	Report(("BTL_FTPC_Create() returned %s.",FtpcaBtStatus(status)));
	
}


/*---------------------------------------------------------------------------
 *            FTPCA_Destroy
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Destroy the FTP Server.
 *
 * Return:    void
 *
 */
void FTPCA_Destroy(void)
{
	BtStatus status;

	status =  BTL_FTPC_Destroy(&ftpcContext);
	
	Report(("BTL_FTPC_Destroy() returned %s.",FtpcaBtStatus(status)));

}


/*---------------------------------------------------------------------------
 *            FTPCA_Abort
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Aborts the current client operation.
 *
 * Return:    void
 *
 */
void FTPCA_Abort(void)
{
	BtStatus status;
	
	status = BTL_FTPC_Abort(ftpcContext);
	
	Report(("BTL_FTPC_Abort() returned %s.",FtpcaBtStatus(status)));

}


/*---------------------------------------------------------------------------
 *            FTPCA_Connect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Create transport connection.
 *
 * Return:    void
 *
 */
void FTPCA_Connect(void)
{
	BtStatus	status;
	U8 			BdArray[BDADDR_NTOA_SIZE];
	LINE_PARSER_STATUS lineStatus;
	
	lineStatus = LINE_PARSER_GetNextStr(BdArray, BDADDR_NTOA_SIZE);

	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("FTPC wrong BD addr. Can't connect."));
		return;
	}
	
	bdAddr = bdaddr_aton((const char *)(BdArray));
//	bdAddr = bdaddr_aton("00:16:38:5F:92:35");
	
	status = BTL_FTPC_Connect(ftpcContext,&bdAddr);
	
	Report(("BTL_FTPC_Connect() returned %s.",FtpcaBtStatus(status)));
	
}

/*---------------------------------------------------------------------------
 *            FTPCA_Disconnect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnect transport connection.
 *
 * Return:    void
 *
 */
void FTPCA_Disconnect(void)
{
	BtStatus status;
	
	status = BTL_FTPC_Disconnect(ftpcContext);
	
	Report(("BTL_FTPC_Disconnect() returned %s.",FtpcaBtStatus(status)));
	
}

/*---------------------------------------------------------------------------
 *            FTPCA_PushFile
 *---------------------------------------------------------------------------
 *
 * Synopsis:  send a file to the remote server.
 *
 * Return:    void
 *
 */
void FTPCA_PushFile(void)
{
	BtStatus			status;
	BtlObject 			objToPush;
	LINE_PARSER_STATUS	lineStatus;
	U8					fileNamePush[128];
	
	lineStatus = LINE_PARSER_GetNextStr((U8 *)fullPathFileToPush, LINE_PARSER_MAX_STR_LEN);

	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("FTPC: Can't read full path file to push. Ignoring command"));
		Report(("LINE_PARSER_GetNextStr() returned %s.",pLineParser_Status(lineStatus)));
		return;
	}

	findFileName(fullPathFileToPush, (char *)fileNamePush);

	objToPush.objectName 					= fileNamePush;
	objToPush.location.fsLocation.fsPath 		= (const BtlUtf8 *)fullPathFileToPush;
	objToPush.objectMimeType 				= NULL;
	objToPush.objectLocation 				= BTL_OBJECT_LOCATION_FS;
	
	status = BTL_FTPC_Push(ftpcContext, &objToPush);
	
	Report(("BTL_FTPC_PushFile() returned %s.",FtpcaBtStatus(status)));
	
}

/*---------------------------------------------------------------------------
 *            FTPCA_PullFile
 *---------------------------------------------------------------------------
 *
 * Synopsis:  receive a file to the remote server.
 *
 * Return:    void
 *
 */
void FTPCA_PullFile(void)
{
	BtStatus status;
	BtlObject objToPull;
	LINE_PARSER_STATUS lineStatus;
	
	lineStatus = LINE_PARSER_GetNextStr((U8 *)fileToPull, LINE_PARSER_MAX_STR_LEN);

	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("FTPC: Can't read file to pull from Server. Ignoring command"));
		Report(("LINE_PARSER_GetNextStr() returned %s.",pLineParser_Status(lineStatus)));
		return;
	}

	lineStatus = LINE_PARSER_GetNextStr((U8 *)fullPathPull, LINE_PARSER_MAX_STR_LEN);

	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("FTPC: Can't read full path file to set on the client. Ignoring command"));
		Report(("LINE_PARSER_GetNextStr() returned %s.",pLineParser_Status(lineStatus)));
		return;
	}

	objToPull.objectName 					= (const BtlUtf8 *)fileToPull;
	objToPull.location.fsLocation.fsPath 		= (const BtlUtf8 *)fullPathPull;
	objToPull.objectLocation 				= BTL_OBJECT_LOCATION_FS;
	
	status = BTL_FTPC_Pull(ftpcContext, &objToPull);
	
	Report(("BTL_FTPC_PullFile() returned %s.",FtpcaBtStatus(status)));
	
}

/*---------------------------------------------------------------------------
 *            FTPCA_Delete
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Delete an object on the remote server..
 *
 * Return:    void
 *
 */
void FTPCA_Delete(void)
{
	BtStatus status;
	LINE_PARSER_STATUS lineStatus;
	
	lineStatus = LINE_PARSER_GetNextStr((U8 *)objToDelete, LINE_PARSER_MAX_STR_LEN);

	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("FTPC: Can't read object to delete. Ignoring command"));
		Report(("LINE_PARSER_GetNextStr() returned %s.",pLineParser_Status(lineStatus)));
		return;
	}

	Report(("FTPCA_Delete() objToDelete = %s ",objToDelete));

	status = BTL_FTPC_Delete(ftpcContext,(const BtlUtf8 *)objToDelete);
	
	Report(("BTL_FTPC_Delete() returned %s.",FtpcaBtStatus(status)));
	
}

/*---------------------------------------------------------------------------
 *            FTPCA_PullListFolder
 *---------------------------------------------------------------------------
 * 
 * Synopsis:  Get the contents of remote server folders and files to memory.
 *
 * Return:    void
 *
 */
void FTPCA_PullListFolder(void)
{
	BtStatus		status;
	static char		xmlBuffer[XML_PARSER_BUFFER_SIZE];
	
	OS_MemSet((char*)xmlBuffer, 0, (U32)XML_PARSER_BUFFER_SIZE);

	folderListing.objectName 						= NULL;
	folderListing.location.memLocation.memAddr		= xmlBuffer;		
	folderListing.location.memLocation.size 		= (U32)XML_PARSER_BUFFER_SIZE;
	folderListing.objectLocation 					= BTL_OBJECT_LOCATION_MEM;
	
	status = BTL_FTPC_PullListFolder(ftpcContext,&folderListing);
	
	Report(("BTL_FTPC_PullListFolder() returned %s.",FtpcaBtStatus(status)));
}

/*---------------------------------------------------------------------------
 *            FTPCA_PullListFolderFS
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get the contents of remote server folders and files to FS.
 *
 * Return:    void
 *
 */
void FTPCA_PullListFolderFS(void)
{
	BtStatus			status;
	LINE_PARSER_STATUS	lineStatus;
	
	lineStatus = LINE_PARSER_GetNextStr((U8 *)pathToPullListFolder, LINE_PARSER_MAX_STR_LEN);

	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("FTPC: Can't read path to file list Folder. Ignoring command"));
		Report(("LINE_PARSER_GetNextStr() returned %s.",pLineParser_Status(lineStatus)));
		return;
	}

	OS_StrCat(pathToPullListFolder,"/" );
	OS_StrCat(pathToPullListFolder,ftpPullFolderListingFileName);

	folderListing.location.fsLocation.fsPath 		= (const BtlUtf8 *)pathToPullListFolder;
	folderListing.objectName 					= (const BtlUtf8 *)ftpPullFolderListingFileName;
	folderListing.objectLocation 				= BTL_OBJECT_LOCATION_FS;
	
	status = BTL_FTPC_PullListFolder(ftpcContext,&folderListing);
	
	Report(("BTL_FTPC_PullListFolder() returned %s.",FtpcaBtStatus(status)));
	
}

/*---------------------------------------------------------------------------
 *            FTPCA_SetFolder
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set current folder to the new folder if doesn't exist create it.
 *
 * Return:    void
 *
 */
void FTPCA_SetFolder(void)
{
	BtStatus status;
	LINE_PARSER_STATUS lineStatus;
	BOOL allowCreate;
	
	lineStatus = LINE_PARSER_GetNextStr((U8 *)folderName, LINE_PARSER_MAX_STR_LEN);

	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("FTPC: Can't read folder name. Ignoring command"));
		Report(("LINE_PARSER_GetNextStr() returned %s.",pLineParser_Status(lineStatus)));
		return;
	}

	lineStatus = LINE_PARSER_GetNextBool(&allowCreate);

	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("FTPC: Can't read allow create folder state. Ignoring command"));
		Report(("LINE_PARSER_GetNextStr() returned %s.",pLineParser_Status(lineStatus)));
		return;
	}

	Report(("FTPCA_SetFolder() folderName = %s ",folderName));
	
	status = BTL_FTPC_SetFolder(ftpcContext, (const BtlUtf8 *)folderName, allowCreate);
	
	Report(("BTL_FTPC_SetFolder() returned %s.",FtpcaBtStatus(status)));
	
}

/*---------------------------------------------------------------------------
 *            FTPCA_SetFolderBackUp
 *---------------------------------------------------------------------------
 *
 * Synopsis:  operation to change to the parent folder.
 *
 * Return:    void
 *
 */
void FTPCA_SetFolderBackUp(void)
{
	BtStatus status;
	
	status = BTL_FTPC_SetFolderBackUp(ftpcContext);
	
	Report(("BTL_FTPC_SetFolderBackUp() returned %s.",FtpcaBtStatus(status)));
	
}

/*---------------------------------------------------------------------------
 *            FTPCA_SetFolderRoot
 *---------------------------------------------------------------------------
 *
 * Synopsis:  operation to change to the root folder.
 *
 * Return:    void
 *
 */
void FTPCA_SetFolderRoot(void)
{
	BtStatus status;
	
	status = BTL_FTPC_SetFolderRoot(ftpcContext);
	
	Report(("BTL_FTPC_SetFolderRoot() returned %s.",FtpcaBtStatus(status)));
	
}


void FTPCA_ParseFolderListing()
{	
	ObStatus        status;
	BtlFtpcParsedEntry parsedEntry;
	
	if (folderListing.location.memLocation.size == 0)
	{
		Report(("Client: Failed reading a folder entry from memory "));
		return;
	}

	status = BTL_FTPC_GetParsedFolderListingFirst( &folderListing, &parsedEntry);
	Report(("Client: BTL_FTPC_GetParsedFolderListingFirst() returned %s.", FtpcaBtStatus(status)));
	if (BT_STATUS_SUCCESS == status)
		Report(("Client: Folder Content:"));
	while (BT_STATUS_NOT_FOUND != status)
	{
		if (status == BT_STATUS_FAILED)
		{
			Report(("Failed reading a folder entry"));
			return;
		}
		AppPrintEntryAttibutes(parsedEntry);
		status = BTL_FTPC_GetParsedFolderListingNext( &folderListing, &parsedEntry);
	}

}

/*---------------------------------------------------------------------------
 *            FTPCA_SetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets security level for the given FTPC context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
void FTPCA_SetSecurityLevel(void)
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

	status = BTL_FTPC_SetSecurityLevel(ftpcContext, &newSecurityLevel);
	
	Report(("BTL_FTPC_SetSecurityLevel() returned %s.",FtpcaBtStatus(status)));

}

/*---------------------------------------------------------------------------
 *            FTPCA_GetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Gets security level for the given FTPC context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
void FTPCA_GetSecurityLevel(void)
{
	BtStatus status;
	BtSecurityLevel level;
	
	status = BTL_FTPC_GetSecurityLevel(ftpcContext, &level);
	
	Report(("BTL_FTPC_GetSecurityLevel() returned %s.",FtpcaBtStatus(status)));

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
 *            FTPCA_ConfigInitiatingObexAuthentication
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set the OBEX Client Authentication.
 *
 * Return:    void
 *
 */
void FTPCA_ConfigInitiatingObexAuthentication(void)
{
	BtStatus status;
	
	BOOL EnableobexAuth = TRUE;

	status = BTL_FTPC_ConfigInitiatingObexAuthentication(ftpcContext, EnableobexAuth, (const BtlUtf8 *)realm, (const BtlUtf8 *)userId, (const BtlUtf8 *)password);
	
	Report(("BTL_FTPC_ConfigInitiatingObexAuthentication() returned %s.",FtpcaBtStatus(status)));
}


/*---------------------------------------------------------------------------
 *            FTPCA_ObexAuthenticationResponse
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Responde to OBEX auth.
 *
 * Return:    void
 *
 */
void FTPCA_ObexAuthenticationResponse(void)
{
	BtStatus status;

	status = BTL_FTPC_ObexAuthenticationResponse(ftpcContext, (const BtlUtf8 *)userId, (const BtlUtf8 *)password);
	
	Report(("BTL_FTPC_ObexAuthenticationResponse() returned %s.",FtpcaBtStatus(status)));
}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */



/*---------------------------------------------------------------------------
 *            FTPCA_CallBack
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes OBEX Client protocol events.
 *
 * Return:    void
 *
 */
static void FTPCA_CallBack(const BtlFtpcEvent *event)
{
	static GoepConnectReq    req;
	BD_ADDR 	bdAddr;
	char 		addr[BDADDR_NTOA_SIZE];

	switch (event->ftpcEvent->event) 
	{
		case FTPC_EVENT_DISCOVERY_FAILED:
			UI_DISPLAY(("FTPClient  Discovery Failure."));
			break;

		case FTPC_EVENT_NO_SERVICE_FOUND:
			UI_DISPLAY(("FTPClient No OBEX Service Found."));
			break;

		case FTPC_EVENT_TP_DISCONNECTED:
			UI_DISPLAY(("FTPClient: Transport Connection has been disconnected."));
			break;

		case FTPC_EVENT_TP_CONNECTED:
			BTL_FTPC_GetConnectedDevice(ftpcContext, &bdAddr);
			UI_DISPLAY(("FTPClient: Transport Connection is up to %s.", bdaddr_ntoa(&bdAddr, addr)));
			break;

#if OBEX_AUTHENTICATION == XA_ENABLED

    case FTPC_EVENT_AUTH_CHALLENGE:
       UI_DISPLAY(("FTPClient: Received Authenticate Challenge for %s operation.", 
            GoepOpName(event->ftpcEvent->oper)));
#if OBEX_MAX_REALM_LEN > 0
        Report(("FTPClient: Realm: %s, charset %d , Options %d.", 
            event->ftpcEvent->info.challenge.realm+1, event->ftpcEvent->info.challenge.realm[0], event->ftpcEvent->info.challenge.options));
#endif /* OBEX_MAX_REALM_LEN > 0 */

//		FTPCA_ObexAuthenticationResponse();

        break;        

#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

		case FTPC_EVENT_COMPLETE:
			/* The requested operation has completed. */
			UI_DISPLAY(("FTPClient The current operation is complete."));
			break;

		case FTPC_EVENT_ABORTED:
			/* The requested operation was aborted. */
			UI_DISPLAY(("FTPClient The Operation was aborted, \"%s\".", ClientAbortMsg(event->ftpcEvent->reason)));
			break;

		case FTPC_EVENT_OBJECT_PROGRESS:
        		UI_DISPLAY(("FTPClient: object at progress."));
//        		UI_DISPLAY(("FTPClient: %d percent of progress was complete.",
//						(event->ftpcEvent->info.progress.currPos/event->ftpcEvent->info.progress.maxPos)*100));
        		break;


		default:
			UI_DISPLAY(("FTPClient: Unexpected event: %i", event->ftpcEvent->event));
	}
}

/*---------------------------------------------------------------------------
 *            GoepOpName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return a pointer to the name of the current clinet operation.
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
 *            pObStatus
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return a pointer to a description of the OBEX status code.
 *
 * Return:    ASCII String pointer.
 *
 */
static const char *FtpcaBtStatus(BtStatus Status)
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
 *            ClientAbortMsg
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return a pointer to a description of the OBEX Abort reason.
 *
 * Return:    ASCII String pointer.
 *
 */
static const char *ClientAbortMsg(ObexRespCode Reason)
{
    switch (Reason) {
    case OBRC_CLIENT_RW_ERROR:
        return "Client Internal R/W Error";
    case OBRC_LINK_DISCONNECT:
        return "Transport Link Disconnected";
    case OBRC_NOT_FOUND:
        return "Object Not Found";
    case OBRC_FORBIDDEN:
        return "Operation Forbidden";
    case OBRC_UNAUTHORIZED:
        return "Unauthorized Operation";
    case OBRC_NOT_ACCEPTABLE:
        return "Not Acceptable";
    case OBRC_CONFLICT:
        return "Conflict";
    case OBRC_USER_ABORT:
        return "User Aborted";
	case OBRC_PRECONDITION_FAILED:
		return "Precondition Failed";
    }
    return "Other";
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
/*---------------------------------------------------------------------------
 *           findFileName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  function searchs for file name from entier given path, searching for last '/' apearence
 *
 * Return:    string-file name, pointed by 'dest'
 *
 */
void findFileName(const char *source, char *dest)
{
	int i, len;

	len = OS_StrLen((const char *)source);
	len--;
	for ( ;(source[len] != BTHAL_FS_PATH_DELIMITER) && (len >= 0);len--)
		;
	len++;
	for (i=0; source[len];i++, len++)
		dest[i] = source[len];
	dest[i] = '\0';
}

void AppPrintEntryPermissisons(BtlFtpcParsedEntyPerm entryPermissions)
{
	switch (entryPermissions)
	{
		case 0:
			Report(("Client:   permissions: No persmissions"));
			break;
		case 1:
			Report(("Client:   permissions: R"));
			break;
		case 2:
			Report(("Client:   permissions: D"));
			break;
		case 4:
			Report(("Client:   permissions: W"));
			break;
		case 3:
			Report(("Client:   permissions: RD"));
			break;
		case 5:
			Report(("Client:   permissions: RW"));
			break;
		case 6:
			Report(("Client:   permissions: DW"));
			break;
		case 7:
			Report(("Client:   permissions: RDW"));
			break;
		default:
			Report(("Client: Error parsing entry permissions"));
			break;
	}
}

void AppPrintEntryAttibutes(BtlFtpcParsedEntry parsedEntry)
{
	switch (parsedEntry.entryType)
	{
	
		case BTL_FTPC_ENTRY_TYPE_PARENT_FOLDER:
			Report(("Client: PARENT FOLDER exists"));
			break;
			
		case BTL_FTPC_ENTRY_TYPE_FOLDER:
			Report(("Client: FOLDER:"));
			if (parsedEntry.entryAttributes & BTL_FTPC_ENTRY_SUPPORTS_NAME)
				Report(("Client:   name: %s",parsedEntry.entryName));
			if (parsedEntry.entryAttributes & BTL_FTPC_ENTRY_SUPPORTS_SIZE)
				Report(("Client:   size: %d",parsedEntry.entrySize));
			if (parsedEntry.entryAttributes & BTL_FTPC_ENTRY_SUPPORTS_MODIFIED_TIME)
			{
				if (TRUE == parsedEntry.entryModifiedTime.utcTime)
					Report(("Client:   modified time (UTC): %.2d/%.2d/%.4d, %.2d:%.2d:%.2d",
						parsedEntry.entryModifiedTime.month, parsedEntry.entryModifiedTime.day, 
						parsedEntry.entryModifiedTime.year, parsedEntry.entryModifiedTime.hour, 
						parsedEntry.entryModifiedTime.minute, parsedEntry.entryModifiedTime.second));
				else
					Report(("Client:   modified time (Local) : %.2d/%.2d/%.4d, %.2d:%.2d:%.2d",
						parsedEntry.entryModifiedTime.month, parsedEntry.entryModifiedTime.day, 
						parsedEntry.entryModifiedTime.year, parsedEntry.entryModifiedTime.hour, 
						parsedEntry.entryModifiedTime.minute, parsedEntry.entryModifiedTime.second));
			}
			if (parsedEntry.entryAttributes & BTL_FTPC_ENTRY_SUPPORTS_CREATED_TIME)
			{
				if (TRUE == parsedEntry.entryModifiedTime.utcTime)
					Report(("Client:   created time (UTC): %.2d/%.2d/%.4d, %.2d:%.2d:%.2d",
						parsedEntry.entryCreatedTime.month, parsedEntry.entryCreatedTime.day, 
						parsedEntry.entryCreatedTime.year, parsedEntry.entryCreatedTime.hour, 
						parsedEntry.entryCreatedTime.minute, parsedEntry.entryCreatedTime.second));
				else
					Report(("Client:   created time (Local) : %.2d/%.2d/%.4d, %.2d:%.2d:%.2d",
						parsedEntry.entryCreatedTime.month, parsedEntry.entryCreatedTime.day, 
						parsedEntry.entryCreatedTime.year, parsedEntry.entryCreatedTime.hour, 
						parsedEntry.entryCreatedTime.minute, parsedEntry.entryCreatedTime.second));
			}
			if (parsedEntry.entryAttributes & BTL_FTPC_ENTRY_SUPPORTS_ACCESSED_TIME)
			{
				if (TRUE == parsedEntry.entryModifiedTime.utcTime)
					Report(("Client:   accessed time (UTC): %.2d/%.2d/%.4d, %.2d:%.2d:%.2d",
						parsedEntry.entryAccessedTime.month, parsedEntry.entryAccessedTime.day, 
						parsedEntry.entryAccessedTime.year, parsedEntry.entryAccessedTime.hour, 
						parsedEntry.entryAccessedTime.minute, parsedEntry.entryAccessedTime.second));
				else
					Report(("Client:   accessed time (Local) : %.2d/%.2d/%.4d, %.2d:%.2d:%.2d",
						parsedEntry.entryAccessedTime.month, parsedEntry.entryAccessedTime.day, 
						parsedEntry.entryAccessedTime.year, parsedEntry.entryAccessedTime.hour, 
						parsedEntry.entryAccessedTime.minute, parsedEntry.entryAccessedTime.second));
			}
			if (parsedEntry.entryAttributes & BTL_FTPC_ENTRY_SUPPORTS_USER_PERMISSIONS)
				AppPrintEntryPermissisons(parsedEntry.entryPermissions);
			break;
			
		case BTL_FTPC_ENTRY_TYPE_FILE:
			Report(("Client: FILE:"));
			if (parsedEntry.entryAttributes & BTL_FTPC_ENTRY_SUPPORTS_NAME)
				Report(("Client:   name: %s",parsedEntry.entryName));
			if (parsedEntry.entryAttributes & BTL_FTPC_ENTRY_SUPPORTS_SIZE)
				Report(("Client:   size: %d",parsedEntry.entrySize));
			if (parsedEntry.entryAttributes & BTL_FTPC_ENTRY_SUPPORTS_MODIFIED_TIME)
			{
				if (TRUE == parsedEntry.entryModifiedTime.utcTime)
					Report(("Client:   modified time (UTC): %.2d/%.2d/%.4d, %.2d:%.2d:%.2d",
						parsedEntry.entryModifiedTime.month, parsedEntry.entryModifiedTime.day, 
						parsedEntry.entryModifiedTime.year, parsedEntry.entryModifiedTime.hour, 
						parsedEntry.entryModifiedTime.minute, parsedEntry.entryModifiedTime.second));
				else
					Report(("Client:   modified time (Local) : %.2d/%.2d/%.4d, %.2d:%.2d:%.2d",
						parsedEntry.entryModifiedTime.month, parsedEntry.entryModifiedTime.day, 
						parsedEntry.entryModifiedTime.year, parsedEntry.entryModifiedTime.hour, 
						parsedEntry.entryModifiedTime.minute, parsedEntry.entryModifiedTime.second));
			}
			if (parsedEntry.entryAttributes & BTL_FTPC_ENTRY_SUPPORTS_CREATED_TIME)
			{
				if (TRUE == parsedEntry.entryModifiedTime.utcTime)
					Report(("Client:   created time (UTC): %.2d/%.2d/%.4d, %.2d:%.2d:%.2d",
						parsedEntry.entryCreatedTime.month, parsedEntry.entryCreatedTime.day, 
						parsedEntry.entryCreatedTime.year, parsedEntry.entryCreatedTime.hour, 
						parsedEntry.entryCreatedTime.minute, parsedEntry.entryCreatedTime.second));
				else
					Report(("Client:   created time (Local) : %.2d/%.2d/%.4d, %.2d:%.2d:%.2d",
						parsedEntry.entryCreatedTime.month, parsedEntry.entryCreatedTime.day, 
						parsedEntry.entryCreatedTime.year, parsedEntry.entryCreatedTime.hour, 
						parsedEntry.entryCreatedTime.minute, parsedEntry.entryCreatedTime.second));
			}
			if (parsedEntry.entryAttributes & BTL_FTPC_ENTRY_SUPPORTS_ACCESSED_TIME)
			{
				if (TRUE == parsedEntry.entryModifiedTime.utcTime)
					Report(("Client:   accessed time (UTC): %.2d/%.2d/%.4d, %.2d:%.2d:%.2d",
						parsedEntry.entryAccessedTime.month, parsedEntry.entryAccessedTime.day, 
						parsedEntry.entryAccessedTime.year, parsedEntry.entryAccessedTime.hour, 
						parsedEntry.entryAccessedTime.minute, parsedEntry.entryAccessedTime.second));
				else
					Report(("Client:   accessed time (Local) : %.2d/%.2d/%.4d, %.2d:%.2d:%.2d",
						parsedEntry.entryAccessedTime.month, parsedEntry.entryAccessedTime.day, 
						parsedEntry.entryAccessedTime.year, parsedEntry.entryAccessedTime.hour, 
						parsedEntry.entryAccessedTime.minute, parsedEntry.entryAccessedTime.second));
			}
			if (parsedEntry.entryAttributes & BTL_FTPC_ENTRY_SUPPORTS_USER_PERMISSIONS)
				AppPrintEntryPermissisons(parsedEntry.entryPermissions);
			break;
			
		default:
			Report(("Client: Error: Unknown Entry Type"));
	}
}
				

#else /* BTL_CONFIG_FTPC == BTL_CONFIG_ENABLED */

void FTPCA_Init(void)
{
	Report(("FTPCA_Init -BTL_CONFIG_FTPC is disabled "));
}
void FTPCA_Deinit(void)
{
	Report(("FTPCA_Deinit  - BTL_CONFIG_FTPC is disabled"));
}

void FTPCA_ProcessUserAction(U8 *msg)
{
	msg=msg;
	Report(("FTPC_APP is disabled via BTL_CONFIG."));

}

#endif /* BTL_CONFIG_FTPC == BTL_CONFIG_ENABLED */



