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
*   FILE NAME:      opps_app.c
*
*   DESCRIPTION:    This file contains the implementation of the OPPS sample 
*					application.
*
*   AUTHOR:         Ronen Levy
*
\*******************************************************************************/
#include "btl_config.h"

#if BTL_CONFIG_OPP == BTL_CONFIG_ENABLED

/*******************************************************************************/

#include "btl_opps.h"
#include "bthal_fs.h"
#include "debug.h"

#if (BTHAL_PLATFORM == PLATFORM_LOCOSTO)
#include "../app_main.h"
#endif

/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

#define OPPSA_DEFAULT_VCARD_NAME     "mycard.vcf"
#define OPPSA_NEW_VCARD_NAME         "mycard_t.vcf"
#define OPPSA_NEW_PUSH_NAME          "pushed_file.vcf"
#define OPPSA_SERVER_NAME            "OBEX Object Push"


#if (BTHAL_PLATFORM == PLATFORM_WINDOWS)

#define OPPSA_DEFAULT_ACCEPT_DIR     "C:\\QbInbox"
#define OPPSA_DEFAULT_FULL_NAME      "C:\\QbInbox\\default_object\\mycard.vcf"

#define OPPSA_NEW_FULL_NAME          "C:\\QbInbox\\default_object\\mycard_t.vcf"
#define OPPSA_NEW_ACCEPT_DIR         "C:\\QbInbox"

#else

#define OPPSA_DEFAULT_ACCEPT_DIR     "/QbInbox"
#define OPPSA_DEFAULT_FULL_NAME      "/QbInbox/default_object/mycard.vcf"

#define OPPSA_NEW_FULL_NAME          "/QbInbox/default_object/mycard_t.vcf"
#define OPPSA_NEW_ACCEPT_DIR         "/QbInbox"

#endif 


 /*******************************************************************************
 *
 * Macro definitions
 *
 ******************************************************************************/
#define OPPSA_PULL_BUFF_MAX          (1 * 512)
#define OPPSA_PUSH_BUFF_MAX          (1 * 1024)

#define OPPSA_CHECK_OBJ_LOCATION(objectLocation)                  \
          ((objectLocation == BTL_OBJECT_LOCATION_FS) ||         \
              (objectLocation == BTL_OBJECT_LOCATION_MEM))


/*******************************************************************************
 *
 * External definitions
 *
 ******************************************************************************/

#if (BTHAL_PLATFORM == PLATFORM_WINDOWS)

extern void OPP_Progress(U32 barId, U32 currPos, U32 maxPos);
extern void OPP_Report(const char *format,...);
extern U32 OPP_GetServerUIContext(void);

#undef Report
#define Report(s) OPP_Report s
#define UI_DISPLAY(s) OPP_Report s
#define OPP_PROGRESS(barId, currPos, maxPos) OPP_Progress((barId), (currPos), (maxPos))

#else

#define OPP_PROGRESS(barId, currPos, maxPos) UI_DISPLAY(("OPPSA_Callback: progress bar %d / %d", (currPos), (maxPos)))

#endif


/*******************************************************************************
 *
 * OPP External Function prototypes
 *
 ******************************************************************************/

void OPPSA_Init(void);
void OPPSA_Deinit(void);
void OPPSA_Create(void);
void OPPSA_Destroy(void);
void OPPSA_Enable(void);
void OPPSA_Disable(void);
void OPPSA_AbortServer(void);
void OPPSA_Disconnect(void);
void OPPSA_GetConnectedDevice(void);
BOOL OPPSA_GetSecurityLevel(void);
BOOL OPPSA_SetSecurityLevel(BtSecurityLevel level);
void OPPSA_AcceptObjectRequest(BOOL accept);
void OPPSA_SetObjLocation(BtlObjectLocation objLocation);


/********************************************************************************
 *
 * Internal Types
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * OppsaPullUseObject useObject
 *
 *     	Defines which object to use in accept/reject Pull operation.
 */
typedef U8 OppsaPullUseObject;

#define OPPSA_PULL_USE_DEFAULT_OBJ     				(0x01)
#define OPPSA_PULL_USE_NEW_OBJ           			(0x02)	


/*-------------------------------------------------------------------------------
 * OppsaState type
 *
 *     Defines the OPPS current state.
 */
typedef U8 OppsaState;

#define OPPSA_IDLE				0x00	/* OBEX and Transport disconnected */   
#define OPPSA_CONNECTING		0x01	/* OBEX connecting */
#define OPPSA_TP_CONNECTED		0x02	/* Transport connected */
#define OPPSA_CONNECTED			0x03	/* OBEX connected */
#define OPPSA_DISCONNECTING		0x04	/* OBEX disconnecting */   
#define OPPSA_TP_DISCONNECTING	0x05	/* Transport disconnecting */
#define OPPSA_DISABLING     	0x06	/* OBEX and Transport disabling */


typedef struct _OppsaData 
{
	OppsaState		state;

	BOOL			isConnected;

	BOOL			operationOngoing;

	BtSecurityLevel	securityLevel;

	OppsOperation	serverOperation;  
    
	BtlObjectLocation	objLocation;
    
	/* Memory Objects */
	char                pushBuff[OPPSA_PUSH_BUFF_MAX];
	char                pullBuff[OPPSA_PULL_BUFF_MAX];

} OppsaData;


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

static OppsaData           oppServer;		

static BtlOppsContext      *oppsContext = 0;   

static char userPushFileName[BTHAL_FS_MAX_FILE_NAME_LENGTH + 1] = {'\0'};
static char userPushPath[BTHAL_FS_MAX_PATH_LENGTH + 1] = {'\0'};
static char userPushFsPath[BTHAL_FS_MAX_FILE_NAME_LENGTH + BTHAL_FS_MAX_PATH_LENGTH + 1] = {'\0'};

/*******************************************************************************
 *
 * Internal Function prototypes
 *
 ******************************************************************************/
static void  OPPSA_Callback(const BtlOppsEvent *Event);
static const char *OppsaGoepOpName(GoepOperation Op);

static void OppsaInitMemPushObject(BtlObject *obj, char *buff, U32 size);
static BOOL OppsaInitMemPullObject(BtlObject *obj, char *buff, OppsaPullUseObject useObj);

static void OppsaInitFsPushObject(BtlObject *obj, char *userPushFileName, char *userPushPath);
static void OppsaInitFsPullObject(BtlObject *obj, OppsaPullUseObject useObj);

static void OppsaSetDefaultObject(BtlObject *obj, OppsaPullUseObject useObj);


/*---------------------------------------------------------------------------
 *            OPPSA_Init
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX server for the OPP application 
 *            and the Object Store.
 *
 * Return:    void
 *
 */
void OPPSA_Init(void)
{
	BtStatus status;
	BtlObject serverObject;    
    
	oppServer.securityLevel = BSL_NO_SECURITY;
	oppServer.state = OPPSA_IDLE;
	oppServer.isConnected = FALSE;
	oppServer.operationOngoing = FALSE;
	oppServer.objLocation = BTL_OBJECT_LOCATION_FS;
    
	OppsaSetDefaultObject(&serverObject, OPPSA_PULL_USE_DEFAULT_OBJ);
        
	status = BTL_OPPS_SetDefaultObject(&serverObject);

	Report(("BTL_OPPS_SetDefaultObject() returned %s.",pBT_Status(status)));

#if (BTHAL_PLATFORM == PLATFORM_WINDOWS)
#else
	status = BTL_OPPS_Create((BtlAppHandle *)0, OPPSA_Callback, &oppServer.securityLevel, &oppsContext);	

	Report(("BTL_OPPS_Create() returned %s.",pBT_Status(status)));
#endif

	status = BTL_OPPS_Enable(oppsContext, (const BtlUtf8*)OPPSA_SERVER_NAME, BTL_OPPS_SUPPORTED_FORMATS_ANY);

	Report(("BTL_OPPS_Enable() returned %s.",pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            OPPSA_Deinit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the OBEX Server protocol.
 *
 * Return:    void
 *
 */
void OPPSA_Deinit(void)
{
	BtStatus status;

	status = BTL_OPPS_Disable(oppsContext);

	Report(("BTL_OPPS_Disable() returned %s.",pBT_Status(status)));

	if (status == BT_STATUS_SUCCESS)
	{

#if (BTHAL_PLATFORM == PLATFORM_WINDOWS)
#else
		status = BTL_OPPS_Destroy(&oppsContext);

		Report(("BTL_OPPS_Destroy() returned %s.", pBT_Status(status)));
#endif
	    oppServer.state = OPPSA_IDLE;
	    oppServer.isConnected = FALSE;
	    oppServer.operationOngoing = FALSE;
	}
	else
	{
	    oppServer.state = OPPSA_DISABLING;
	}
}


/*---------------------------------------------------------------------------
 *            OPPSA_Create
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Allocates a unique OPPS context.
 *
 * Return:    void
 *
 */
void OPPSA_Create(void)
{
	BtStatus status;

	status = BTL_OPPS_Create((BtlAppHandle *)0, OPPSA_Callback, &oppServer.securityLevel, &oppsContext);	

	Report(("BTL_OPPS_Create() returned %s.",pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            OPPSA_Destroy
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Releases a OPPS context (previously allocated with BTL_OPPS_Create).
 *
 * Return:    void
 *
 */
void OPPSA_Destroy(void)
{
	BtStatus status;

	status = BTL_OPPS_Destroy(&oppsContext);

	Report(("BTL_OPPS_Destroy() returned %s.", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            OPPSA_Enable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Enable OPPS and register OPP server SDP record, called after 
 *		      BTL_OPPS_Create.
 *
 * Return:    void
 *
 */
void OPPSA_Enable(void)
{
	BtStatus status;

	status = BTL_OPPS_Enable(oppsContext, (const BtlUtf8*)OPPSA_SERVER_NAME, BTL_OPPS_SUPPORTED_FORMATS_ANY);

	Report(("BTL_OPPS_Enable() returned %s.", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            OPPSA_Disable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disable OPPS, called before BTL_OPPS_Destroy.
 *		      If a connection exists, it will be disconnected automatically.
 *
 * Return:    void
 *
 */
void OPPSA_Disable(void)
{
	BtStatus status;

	status = BTL_OPPS_Disable(oppsContext);

	Report(("OPPSA_Disable() returned %s.", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            OPPSA_AbortServer
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Requests the current operation to be aborted.
 *
 * Return:    void
 *
 */
void OPPSA_AbortServer(void)
{
	BtStatus status;

	if (oppServer.operationOngoing == TRUE)
	{
	    status = BTL_OPPS_Abort(oppsContext, OBRC_CONFLICT);
	    Report(("BTL_OPPS_Abort() returned %s.",pBT_Status(status)));
	}
	else
	{
	    Report(("OPPSA_AbortServer: No ongoing OPP operation!"));
	}
}


/*---------------------------------------------------------------------------
 *            OPPSA_Disconnect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnects 
 *
 */
void OPPSA_Disconnect(void)
{
	BtStatus status; 	

	if (oppServer.isConnected == TRUE)
	{
	    status = BTL_OPPS_Disconnect(oppsContext);
		Report(("BTL_OPPS_Disconnect() returned %s.", pBT_Status(status)));
	}
	else
	{
		Report(("OPPSA_Disconnect: Server is not connected!"));
	}
}


/*---------------------------------------------------------------------------
 *            OPPSA_GetConnectedDevice
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get Server connected device
 *
 */
void OPPSA_GetConnectedDevice(void)
{
	BD_ADDR bd_addr;
	U8 bdArray[BDADDR_NTOA_SIZE];
	BtStatus status;

	status = BTL_OPPS_GetConnectedDevice(oppsContext, &bd_addr);
	if (BT_STATUS_SUCCESS == status)
	{
	    Report(("OPPSA_GetConnectedDevice(): bd_Addr %s.", bdaddr_ntoa(&bd_addr, (char *)bdArray)));
	}
	else
	{
	    Report(("OPPSA_GetConnectedDevice(): Err returned %s.", pBT_Status(status)));
	}
}
            

/*---------------------------------------------------------------------------
 *            OPPSA_SetSecurityLevel
 *
 * Synopsis:  Set the OPP Server security level.
 *
 * Return:    void
 *
 */
BOOL OPPSA_SetSecurityLevel(BtSecurityLevel level)
{
	BtStatus status;
	BtSecurityLevel securityLevel;
	BOOL ret_val;

	securityLevel = oppServer.securityLevel;

	if (securityLevel & level)
	{
	    securityLevel &= (~level);
	    ret_val = FALSE;
	}
	else
	{
	    securityLevel |= level;
	    ret_val = TRUE;
	}

	status = BTL_OPPS_SetSecurityLevel(oppsContext, &securityLevel);

	Report(("BTL_OPPS_SetSecurityLevel() returned %s.", pBT_Status(status)));

	if (status == BT_STATUS_SUCCESS)
	{
	    oppServer.securityLevel = securityLevel;
	}

	return ((status == BT_STATUS_SUCCESS) ? (ret_val) : (!ret_val));
}


/*---------------------------------------------------------------------------
 *            OPPSA_GetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Gets security level for the given OPPC context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
BOOL OPPSA_GetSecurityLevel(void)
{
	BtStatus status;
	BtSecurityLevel securityLevel;

	status = BTL_OPPS_GetSecurityLevel(oppsContext, &securityLevel);

	Report(("BTL_OPPS_GetSecurityLevel() returned %s.",pBT_Status(status)));

	switch (securityLevel)
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


/*---------------------------------------------------------------------------
 *            OPPSA_AcceptObjectRequest
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Accept the pending object request.
 *
 * Return:    void
 *
 */
void OPPSA_AcceptObjectRequest(BOOL accept)
{
	BtStatus    status;
	BtlObject	serverObject;
	BOOL        retVal;

	if (TRUE != OPPSA_CHECK_OBJ_LOCATION(oppServer.objLocation))
	{
	    Report(("OPPSA_AcceptObjectRequest: Invalid objectLocation"));
	    return;
	}

	if (accept == FALSE)
	{
		OppsaInitFsPullObject(&serverObject, OPPSA_PULL_USE_DEFAULT_OBJ);
					
		status = BTL_OPPS_AcceptObjectRequest(oppsContext, FALSE, &serverObject);

	    Report(("BTL_OPPS_AcceptObjectRequest() returned %s.", pBT_Status(status)));
	}
	else
	{
		if (oppServer.serverOperation == GOEP_OPER_PUSH)
		{
			if (oppServer.objLocation == BTL_OBJECT_LOCATION_MEM)
			{
				OppsaInitMemPushObject(&serverObject, oppServer.pushBuff, OPPSA_PUSH_BUFF_MAX);
			}
			else if (oppServer.objLocation == BTL_OBJECT_LOCATION_FS)
			{
				OppsaInitFsPushObject(&serverObject, userPushFileName, OPPSA_NEW_ACCEPT_DIR);
			}
		}
		else if (oppServer.serverOperation == GOEP_OPER_PULL)
		{
			if (oppServer.objLocation == BTL_OBJECT_LOCATION_MEM)
			{
				retVal = OppsaInitMemPullObject(&serverObject, &oppServer.pullBuff[0], OPPSA_PULL_USE_NEW_OBJ); 
				if (TRUE != retVal)
				{
					accept = FALSE;
				}
			}
			else if (oppServer.objLocation == BTL_OBJECT_LOCATION_FS)
			{
				OppsaInitFsPullObject(&serverObject, OPPSA_PULL_USE_NEW_OBJ);
			}
		}

		status = BTL_OPPS_AcceptObjectRequest(oppsContext, accept, &serverObject);

		Report(("BTL_OPPS_AcceptObjectRequest() returned %s.", pBT_Status(status)));
	}
}


/*---------------------------------------------------------------------------
 *            OPPSA_SetObjLocation
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set the object Location (Memory or FS)
 *
 * Return:    None
 *
 */
void OPPSA_SetObjLocation(BtlObjectLocation objLocation)
{
	oppServer.objLocation = objLocation;
}


/*---------------------------------------------------------------------------
 *            OPPSA_Callback
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes OBEX server protocol events.
 *
 * Return:    void
 *
 */
void OPPSA_Callback(const BtlOppsEvent *Event)
{
	BD_ADDR bdAddr;
	char addr[BDADDR_NTOA_SIZE];

	/* Save the GOEP operation for future use */
	oppServer.serverOperation = Event->oppsEvent->oper;

	switch (Event->oppsEvent->event) 
	{

		case OPPS_EVENT_START:
		    
			UI_DISPLAY(("Server: Starting %s operation.", OppsaGoepOpName(Event->oppsEvent->oper)));
			oppServer.operationOngoing = TRUE;
			break;


		case OPPS_EVENT_COMPLETE:
		    
			UI_DISPLAY(("Server: %s operation complete.", OppsaGoepOpName(Event->oppsEvent->oper)));
		    
			OPP_PROGRESS(OPP_GetServerUIContext(), 0, 0); 
			oppServer.operationOngoing = FALSE;
			break;


		case OPPS_EVENT_ABORTED:

			UI_DISPLAY(("Server: GOEP %s operation failed or aborted.", OppsaGoepOpName(Event->oppsEvent->oper)));
			OPP_PROGRESS(OPP_GetServerUIContext(), 0, 0);			

			oppServer.isConnected = FALSE;
			oppServer.operationOngoing = FALSE;
			oppServer.state = OPPSA_IDLE;
			break;

		case OPPS_EVENT_TP_CONNECTED:  

			BTL_OPPS_GetConnectedDevice(oppsContext, &bdAddr);
			UI_DISPLAY(("Server: Transport Connection is up to %s.", bdaddr_ntoa(&bdAddr, addr)));
			
			oppServer.state = OPPSA_TP_CONNECTED;
			oppServer.isConnected = TRUE;
			break;

		case OPPS_EVENT_TP_DISCONNECTED:
			
			UI_DISPLAY(("Server: Transport Connection has been disconnected."));
			OPP_PROGRESS(OPP_GetServerUIContext(), 0, 0);			
		    
			oppServer.isConnected = FALSE;
			oppServer.operationOngoing = FALSE;

			if (oppServer.state != OPPSA_DISABLING)
			{
				oppServer.state = OPPSA_IDLE; 
			}
		  
			break;

		case OPPS_EVENT_OBJECT_PROGRESS:
			
			OPP_PROGRESS(OPP_GetServerUIContext(), Event->oppsEvent->info.progress.currPos, 
				Event->oppsEvent->info.progress.maxPos);
			break;

		case OPPS_EVENT_OBJECT_REQUEST:
			
			if (GOEP_OPER_PUSH == Event->oppsEvent->oper)
			{
				userPushFileName[0] = '\0';

				if (0 != Event->oppsEvent->info.request.name[0])
				{
					OS_StrCpyUtf8(userPushFileName, (const char *)Event->oppsEvent->info.request.name); 
				}
			}	
			
			UI_DISPLAY(("Server: Incoming object request: %s operation.", OppsaGoepOpName(Event->oppsEvent->oper)));

			UI_DISPLAY(("Server: Name: %s, Type: %s, Size: %u.", Event->oppsEvent->info.request.name, 
				Event->oppsEvent->info.request.type, Event->oppsEvent->info.request.objectLen));
					
			UI_DISPLAY(("Server: Please accept or reject!"));
			break;

		case OPPS_EVENT_DISABLED:
		    
			UI_DISPLAY(("Server: Receiving OPPS_EVENT_DISABLED."));
		    
			oppServer.state = OPPSA_IDLE;
			break;

		default: 
			
			UI_DISPLAY(("Server: Unexpected event: %i", Event->oppsEvent->event));
			break;

	}
}



/*---------------------------------------------------------------------------
 *            OppsaGoepOpName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return a pointer to the name of the current operation.
 *
 * Return:    ASCII String pointer.
 *
 */
static const char *OppsaGoepOpName(GoepOperation Op)
{
	switch (Op) 
	{
	case GOEP_OPER_PUSH:
	    return "Push";
	case GOEP_OPER_PULL:
	    return "Pull";
	case GOEP_OPER_CONNECT:
	    return "Connect";
	case GOEP_OPER_DISCONNECT:
	    return "Disconnect";
	case GOEP_OPER_ABORT:
	    return "Abort";
	case GOEP_OPER_DELETE:
	    return "Delete";
	case GOEP_OPER_SETFOLDER:
	    return "Set Folder";
	}
	return "Unknown";
}


/*---------------------------------------------------------------------------
 * OppsaInitFsPullObject()
 *
 * Synopsis:  Initialize obj as FS Pull object.
 *
 * Return:    void
 *
 */
static void OppsaInitFsPullObject(BtlObject *obj, OppsaPullUseObject useObj)
{			
	switch (useObj)
	{
		case OPPSA_PULL_USE_DEFAULT_OBJ:
			obj->objectName 		        = (const BtlUtf8 *)OPPSA_DEFAULT_VCARD_NAME;  
			obj->location.fsLocation.fsPath = (const BtlUtf8 *)OPPSA_DEFAULT_FULL_NAME;
			break;

		case OPPSA_PULL_USE_NEW_OBJ:
			obj->objectName 			    = (const BtlUtf8 *)OPPSA_NEW_VCARD_NAME;  
			obj->location.fsLocation.fsPath = (const BtlUtf8 *)OPPSA_NEW_FULL_NAME;
			break;
	}

	obj->objectMimeType = "text/x-vCard";
	obj->objectLocation = BTL_OBJECT_LOCATION_FS;
}

/*---------------------------------------------------------------------------
 * OppsaSetToFsPushObject()
 *
 * Synopsis:  Initialize obj as FS Push object.
 *
 * Return:    void
 *
 */
static void OppsaInitFsPushObject(BtlObject *obj, char *userPushFileName, char *userPushPath)
{			
	U16 len;

    obj->objectName 			        = NULL;  
	obj->objectMimeType 		        = NULL;
	obj->objectLocation 		        = BTL_OBJECT_LOCATION_FS;


    if (userPushFileName[0] == '\0')
    {
        OS_StrCpyUtf8(userPushFileName, (const char *)OPPSA_NEW_PUSH_NAME); 
    }
    
    userPushFsPath[0] = '\0';
	OS_StrCpyUtf8(userPushFsPath, userPushPath);
	len = OS_StrLenUtf8(userPushFsPath);
	userPushFsPath[len] = BTHAL_FS_PATH_DELIMITER;
	userPushFsPath[++len] = '\0';
    OS_StrCatUtf8(userPushFsPath, userPushFileName); 

    userPushFileName[0] = '\0';

	obj->location.fsLocation.fsPath     = (const BtlUtf8 *)userPushFsPath;
}

/*---------------------------------------------------------------------------
 * OppsaSetDefaultObject()
 *
 * Synopsis:  Initialize obj as the default object (Memory or FS object).
  *
 * Return:    void
 *
 */
static void OppsaSetDefaultObject(BtlObject *obj, OppsaPullUseObject useObj)
{			
	if (BTL_OBJECT_LOCATION_FS == oppServer.objLocation)
	{
		OppsaInitFsPullObject(obj, useObj);  
	}
	else if (BTL_OBJECT_LOCATION_MEM == oppServer.objLocation)
	{
		if (TRUE != OppsaInitMemPullObject(obj, &oppServer.pullBuff[0], useObj))
		{
			return;
		}
	}
}

/*---------------------------------------------------------------------------
 * OppsaInitMemPushObject()
 *
 * Synopsis:  Initialize obj as a memory Push object.
 *
 * Return:    void
 *
 */
static void OppsaInitMemPushObject(BtlObject *obj, char *buff, U32 size)
{			
	
	obj->objectName 			        = NULL;  
	obj->objectMimeType 		        = NULL;
	obj->objectLocation                 = BTL_OBJECT_LOCATION_MEM;
    
	obj->location.memLocation.memAddr   = buff;
	obj->location.memLocation.size      = size;	
}

/*---------------------------------------------------------------------------
 * OppsaInitMemPullObject()
 *
 * Synopsis:  Fills buff with default object data, and initialize 
 *            obj as memory Pull object.
 *
 * Return:    void
 *
 */
static BOOL OppsaInitMemPullObject(BtlObject *obj, char *buff, OppsaPullUseObject useObj)
{
	BtFsStatus      btFsStatus;
	BthalFsFileDesc fd;
	BTHAL_U32       numRead;
	BthalFsStat     FileStat;
	const BTHAL_U8 *fsPushPath = NULL;
	BOOL retVal = FALSE;

	switch (useObj)
	{
	    case OPPSA_PULL_USE_DEFAULT_OBJ:
	        fsPushPath      = (const BtlUtf8*)OPPSA_DEFAULT_FULL_NAME;
	        obj->objectName = (const BtlUtf8 *)OPPSA_DEFAULT_VCARD_NAME;  
	        break;

	    case OPPSA_PULL_USE_NEW_OBJ:
	        fsPushPath      = (const BtlUtf8*)OPPSA_NEW_FULL_NAME;
	        obj->objectName = (const BtlUtf8*)OPPSA_NEW_VCARD_NAME;  
	        break;
	}

	btFsStatus = BTHAL_FS_Stat(fsPushPath, &FileStat); 
	if(btFsStatus != BT_STATUS_HAL_FS_SUCCESS)
	{
		Report(("Server: Error - File was not found %s", fsPushPath));
	    return retVal;
	}

	if (FileStat.size > OPPSA_PULL_BUFF_MAX)
	{
	    Report(("Server: Error pushed file > %d bytes", OPPSA_PULL_BUFF_MAX));
	    return retVal;
	}
	   
	btFsStatus = BTHAL_FS_Open(fsPushPath, 
		            BTHAL_FS_O_RDONLY | BTHAL_FS_O_BINARY,      /* Read Only */
	                        (BthalFsFileDesc *)&fd);

	if (btFsStatus == BT_STATUS_HAL_FS_SUCCESS)
	{
	    btFsStatus  = BTHAL_FS_Read(fd, buff, FileStat.size, &numRead);
	    
	    if (btFsStatus == BT_STATUS_HAL_FS_SUCCESS)
	 	{
			Report(("Server: fsPushPath = %s, numRead = %d", fsPushPath, numRead));
	        
	        
	        obj->objectMimeType 		        = "text/x-vCard";
	        obj->objectLocation                 = BTL_OBJECT_LOCATION_MEM;

	        obj->location.memLocation.memAddr   = buff;
	        obj->location.memLocation.size      = FileStat.size;	

	        retVal = TRUE;
	 	}
		
	    BTHAL_FS_Close(fd);
	}
	else
	{
		Report(("Server: Error - could not open %s", fsPushPath));
	    return retVal;
	}

	return retVal;
}


#else /* BTL_CONFIG_OPP == BTL_CONFIG_ENABLED */

void OPPSA_Init(void)
{
	
	Report(("OPPSA_Init -BTL_CONFIG_OPP is disabled"));
}


void OPPSA_Deinit(void)
{

	Report(("OPPSA_Deinit -BTL_CONFIG_OPP is disabled"));
}



#endif /* BTL_CONFIG_OPP == BTL_CONFIG_ENABLED */

