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
*   FILE NAME:      oppc_app.c
*
*   DESCRIPTION:    This file contains the implementation of the OPPC sample 
*					application.
*
*   AUTHOR:         Ronen Levy
*
\*******************************************************************************/
#include "btl_config.h"
#if BTL_CONFIG_OPP == BTL_CONFIG_ENABLED

/*******************************************************************************/

#include "btl_oppc.h"
#include "bthal_fs.h"
#include "debug.h"

#if (BTHAL_PLATFORM == PLATFORM_WINDOWS)
#else
#include "../app_main.h"
#endif


/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/



/*******************************************************************************
 *
 * Macro definitions
 *
 ******************************************************************************/
#define OPPCA_PULL_BUFF_MAX                    (1 * 512)
#define OPPCA_PUSH_BUFF_MAX                    (1 * 1024)

#define OPPCA_CHECK_FS_NAME_LEN(fsPath)    \
             (OS_StrLen(fsPath) < (BTHAL_FS_MAX_PATH_LENGTH + BTHAL_FS_MAX_FILE_NAME_LENGTH + 1)) 



#if (BTHAL_PLATFORM == PLATFORM_WINDOWS)

extern void OPP_Progress(U32 barId, U32 currPos, U32 maxPos);
extern void OPP_Report(const char *format,...);
extern U32  OPP_GetClientUIContext(void);

#undef Report
#define Report(s) OPP_Report s
#define UI_DISPLAY(s) OPP_Report s
#define OPP_PROGRESS(barId, currPos, maxPos) OPP_Progress((barId), (currPos), (maxPos))

#else

#define OPP_PROGRESS(barId, currPos, maxPos) UI_DISPLAY(("OPPCA_Callback: progress bar %d / %d", (currPos), (maxPos)))

#endif

/*******************************************************************************
 *
 * OPPC External Function prototypes
 *
 ******************************************************************************/

void OPPCA_Init(void);
void OPPCA_Deinit(void);
void OPPCA_Create(void);
void OPPCA_Destroy(void);
void OPPCA_Enable(void);
void OPPCA_Disable(void);
void OPPCA_AbortClient(void);
void OPPCA_Connect(BD_ADDR *bd_addr);
void OPPCA_Disconnect(void);
BOOL OPPCA_GetSecurityLevel(void);
BOOL OPPCA_SetSecurityLevel(BtSecurityLevel level);
void OPPCA_SetObjLocation(BtlObjectLocation objLocation);

void OPPCA_BasicPush(const char *fsPushPath);
void OPPCA_BasicPull(const char *fsPullPath);
void OPPCA_BasicExchange(const char *fsPushPath, const char *fsPullPath);

void OPPCA_EncapsulatePush(const char *fsPushPath, BD_ADDR *bd_addr);
void OPPCA_EncapsulatePull(const char *fsPullPath, BD_ADDR *bd_addr);
void OPPCA_EncapsulateExchange(const char *fsPushPath, const char *fsPullPath, BD_ADDR *bd_addr);




/*-------------------------------------------------------------------------------
 * OppcaCurrOperation type
 *
 *     Defines the OPPCA current operation.
 */
typedef U8 OppcaCurrOperation;

#define OPPCA_OPER_NONE     		0x00	
#define OPPCA_OPER_CONNECT          0x01	/* Connect */
#define OPPCA_OPER_BASIC_PULL		0x02	/* Pull */
#define OPPCA_OPER_BASIC_PUSH    	0x03	/* Push */
#define OPPCA_OPER_BASIC_EXCHANGE	0x04	/* Push-Pull */
#define OPPCA_OPER_ENC_PULL 		0x05	/* Connect-Pull-Disconnect */
#define OPPCA_OPER_ENC_PUSH 		0x06	/* Connect-Push-Disconnect */
#define OPPCA_OPER_ENC_EXCHANGE     0x07	/* Connect-Push-Pull-Disconnect */


/*-------------------------------------------------------------------------------
 * OppcaState type
 *
 *     Defines the OPPCA current state.
 */
typedef U8 OppcaState;

#define OPPCA_IDLE				0x00	/* OBEX and Transport disconnected */   
#define OPPCA_CONNECTING		0x01	/* OBEX connecting */
#define OPPCA_TP_CONNECTED		0x02	/* Transport connected */
#define OPPCA_CONNECTED			0x03	/* OBEX connected */
#define OPPCA_DISCONNECTING		0x04	/* OBEX disconnecting */   
#define OPPCA_TP_DISCONNECTING	0x05	/* Transport disconnecting */
#define OPPCA_DISABLING     	0x06	/* OBEX and Transport disabling */


typedef struct _OppcaData 
{
    OppcaState          state;

    BOOL                isConnecting;

    BOOL                isConnected;

    BtSecurityLevel     securityLevel;

    BtlObjectLocation   objLocation;

    OppcaCurrOperation  currOper;

    /* Memory Objects */
    char                pushBuff[OPPCA_PUSH_BUFF_MAX];
    char                pullBuff[OPPCA_PULL_BUFF_MAX];

} OppcaData;

 

/*-------------------------------------------------------------------------------
 * Oppca Mime Types
 *
 *     Defines the equivalent MIME type for a given file extention.
 */

/*
 * List of MIME types
 */
#define OPPCA_MIME_TYPE_NAMES {                                                        \
    "text/x-vcard", \
    "text/x-vcalendar", \
    "text/x-vmessage", \
    "text/html", \
    "text/javascript", \
    "text/plain", \
    "image/jpeg", \
    "image/jpeg", \
    "image/gif", \
    "image/tiff", \
    "image/png", \
    0 }

/*
 * List of file extentions
 */
#define OPPCA_FILE_EXTENTION_NAMES {                                                        \
    "vcf", \
    "vcf", \
    "vcf", \
    "html", \
    "js", \
    "txt", \
    "jpg", \
    "jpeg", \
    "gif", \
    "tiff", \
    "png", \
    0 }



/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

static OppcaData        oppClient;
static BtlOppcContext   *oppcContext = 0;   

static char ObPushName[BTHAL_FS_MAX_FILE_NAME_LENGTH + 1] = {'\0'};
static char ObPullName[BTHAL_FS_MAX_FILE_NAME_LENGTH + 1] = {'\0'};


/*******************************************************************************
 *
 * Internal Function prototypes
 *
 ******************************************************************************/
static void  OPPCA_Callback(const BtlOppcEvent *Event);

static const char *OppcaGoepOpName(GoepOperation Op);

static BOOL OppcaBuildPushObject(BtlObject *objToPush, const char *fsPath, char *buff);
static BOOL OppcaBuildPullObject(BtlObject *objToPull, const char *fsPath, char *buff);

static void OppcaInitFsPushObject(BtlObject *obj, const char *fsPushPath);
static void OppcaInitFsPullObject(BtlObject *obj, const char *fsPullPath);

static void OppcaInitMemPullObject(BtlObject *obj, const char *fsPullPath, char *buff);
static BOOL OppcaInitMemPushObject(BtlObject *obj, const char *fsPushPathh, char *buff);

static void OppcaExtractObjName(const char *source, char *dest);

static const char * OppcaConvertFileExtentionToMimeType(const char *source);
static BOOL OppcaIsOperationInProgress(void);

/*---------------------------------------------------------------------------
 *            OPPCA_Init
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX Client for the OPP application 
 *            and the Object Store.
 *
 * Return:    void
 *
 */
void OPPCA_Init(void)
{
	BtStatus status;
    
	oppClient.state = OPPCA_IDLE;
	oppClient.isConnecting = FALSE;
	oppClient.isConnected = FALSE;
	oppClient.securityLevel = BSL_NO_SECURITY;
	oppClient.currOper = OPPCA_OPER_NONE;
	oppClient.objLocation = BTL_OBJECT_LOCATION_FS;  
  
#if (BTHAL_PLATFORM == PLATFORM_WINDOWS)
#else
	status = BTL_OPPC_Create((BtlAppHandle *)0, OPPCA_Callback, &oppClient.securityLevel, &oppcContext);

	Report(("BTL_OPPC_Create() returned %s.",pBT_Status(status)));
#endif

	status = BTL_OPPC_Enable(oppcContext);

	Report(("BTL_OPPC_Enable() returned %s.",pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            OPPCA_Deinit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the OBEX Client protocol.
 *
 * Return:    void
 *
 */
void OPPCA_Deinit(void)
{
	BtStatus status;

	status =  BTL_OPPC_Disable(oppcContext);
	
	Report(("BTL_OPPC_Disable() returned %s.",pBT_Status(status)));

	if (status == BT_STATUS_SUCCESS)
	{

#if (BTHAL_PLATFORM == PLATFORM_WINDOWS)
#else
		status = BTL_OPPC_Destroy(&oppcContext);

		Report(("BTL_OPPC_Destroy() returned %s.", pBT_Status(status)));
#endif
		oppClient.state = OPPCA_IDLE;
		oppClient.isConnecting = FALSE;
		oppClient.isConnected = FALSE;
	}
	else
	{
		oppClient.state = OPPCA_DISABLING;
	}
}


/*---------------------------------------------------------------------------
 *            OPPCA_Create
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Allocates a unique OPPC context.
 *
 * Return:    void
 *
 */
void OPPCA_Create(void)
{
	BtStatus status;

	status = BTL_OPPC_Create((BtlAppHandle *)0, OPPCA_Callback, &oppClient.securityLevel, &oppcContext);	

	Report(("BTL_OPPC_Create() returned %s.",pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            OPPCA_Destroy
 *---------------------------------------------------------------------------
 *
 * Synopsis: Releases a OPPC context (previously allocated with BTL_OPPC_Create). 
 *
 * Return:    void
 *
 */
void OPPCA_Destroy(void)
{
	BtStatus status;

	status = BTL_OPPC_Destroy(&oppcContext);
    
	Report(("BTL_OPPC_Destroy() returned %s.", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            OPPCA_Enable
 *---------------------------------------------------------------------------
 *
 * Synopsis: Enable OPPC, called after BTL_OPPC_Create.
 *		After calling this function, OPP client is ready for usage. 
 *
 * Return:    void
 *
 */
void OPPCA_Enable(void)
{
	BtStatus status;

	status = BTL_OPPC_Enable(oppcContext);
    
	Report(("BTL_OPPC_Enable() returned %s.", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            OPPSA_Disable
 *---------------------------------------------------------------------------
 *
 * Synopsis: Disable OPPC, called before BTL_OPPC_Destroy.
 *		If a connection exists, it will be disconnected automatically. 
 *
 * Return:    void
 *
 */
void OPPCA_Disable(void)
{
	BtStatus status;

	status = BTL_OPPC_Disable(oppcContext);
    
	Report(("OPPCA_Disable() returned %s.", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            OPPCA_AbortClient
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Requests the current client operation to be aborted.
 *
 * Return:    void
 *
 */
void OPPCA_AbortClient(void)
{
	BtStatus status;
    
	if (TRUE == OppcaIsOperationInProgress())
	{
		status = BTL_OPPC_Abort(oppcContext);
		Report(("BTL_OPPC_Abort() returned %s.",pBT_Status(status)));
	}
	else
	{
		Report(("OPPCA_AbortClient: No ongoing OBEX operation!"));
	}
}


/*---------------------------------------------------------------------------
 *            OPPCA_Connect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Connects to the given server.
 *			  If the given BD_ADDR is NULL, a search is performed.
 *
 */
void OPPCA_Connect(BD_ADDR *bd_addr)
{
	BtStatus status; 	
    
	status = BTL_OPPC_Connect(oppcContext, bd_addr); 
	Report(("BTL_OPPC_Connect() returned %s.",pBT_Status(status)));
		
	if (status == OB_STATUS_PENDING) 
	{
		oppClient.isConnecting = TRUE;
		oppClient.state = OPPCA_CONNECTING;
		oppClient.currOper = OPPCA_OPER_CONNECT;
	}

	Report(("OPPCA_Connect: OPush Connect returned \"%s\".", pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            OPPCA_Disconnect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnects from the given server
 *
 */
void OPPCA_Disconnect(void)
{
	BtStatus status; 	

	if ((TRUE == oppClient.isConnecting) || (TRUE == oppClient.isConnected))
	{
		status = BTL_OPPC_Disconnect(oppcContext);
		Report(("BTL_OPPC_Disconnect() returned %s.", pBT_Status(status)));
	}
	else
	{
		Report(("OPPCA_Disconnect: Client is not connected or connecting!"));
	}
}


/*---------------------------------------------------------------------------
 *            OPPCA_SetSecurityLevel
 *
 * Synopsis:  Set the OPP Client security level.
 *
 * Return:    void
 *
 */
BOOL OPPCA_SetSecurityLevel(BtSecurityLevel level)
{
	BtStatus status;
	BtSecurityLevel securityLevel;
	BOOL ret_val;
    
	securityLevel = oppClient.securityLevel;
    
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
	
	status = BTL_OPPC_SetSecurityLevel(oppcContext, &securityLevel);

	Report(("OPPCA_SetSecurityLevel() returned %s.", pBT_Status(status)));

	if (status == BT_STATUS_SUCCESS)
	{
		oppClient.securityLevel = securityLevel;
	}
    
	return ((status == BT_STATUS_SUCCESS) ? (ret_val) : (!ret_val));
}


/*---------------------------------------------------------------------------
 *            OPPCA_GetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Gets security level for the given OPPC context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
BOOL OPPCA_GetSecurityLevel(void)
{
	BtStatus status;
	BtSecurityLevel securityLevel;
	
	status = BTL_OPPC_GetSecurityLevel(oppcContext, &securityLevel);
	
    Report(("BTL_OPPC_GetSecurityLevel() returned %s.",pBT_Status(status)));

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
 *            OPPCA_SetObjLocation
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set the object Location (Memory or FS)
 *
 * Return:    None
 *
 */
void OPPCA_SetObjLocation(BtlObjectLocation objLocation)
{
	oppClient.objLocation = objLocation;
}


/*---------------------------------------------------------------------------
 *            OPPCA_BasicPush
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate an OBEX Object Push procedure.
 *			  fsPushPath must be given as a complete path.
 *			  If the given BD_ADDR is NULL, a search is performed.
 *
 * Return:    void
 *
 */
void OPPCA_BasicPush(const char *fsPushPath)
{
	BtStatus status;
	BtlObject objToPush;
    	
	Report(("OPPCA_BasicPush: fsPushPath = %s", fsPushPath));

	
	if (TRUE != OPPCA_CHECK_FS_NAME_LEN(fsPushPath))
	{
		Report(("OPPCA_BasicPush: Can't push. fsPushPath too long"));
		return;
	}
	
	if (TRUE != OppcaBuildPushObject(&objToPush, fsPushPath, oppClient.pushBuff))
	{
		Report(("OPPCA_BasicPush: Can't push."));
		return;
	}
    
	status = BTL_OPPC_Push(oppcContext, &objToPush);
	Report(("BTL_OPPC_Push() returned %s.",pBT_Status(status)));
    
	if (status == OB_STATUS_PENDING)
	{
		Report(("OPPCA_ObjectPush(): Started ..."));
		oppClient.currOper = OPPCA_OPER_BASIC_PUSH;
	}
}


/*---------------------------------------------------------------------------
 *            OPPCA_BasicPull
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate an OBEX Business Card Pull procedure.
 *			  ObType can be 0 (no type will be send).
 *			  If the given BD_ADDR is NULL, a search is performed.
 *
 * Return:    void
 *
 */
void OPPCA_BasicPull(const char *fsPullPath)
{
	BtStatus status;
	BtlObject objToPull;
	   
	Report(("OPPCA_BasicPull: fsPullPath = %s", fsPullPath));


	if (TRUE != OPPCA_CHECK_FS_NAME_LEN(fsPullPath))
	{
	    Report(("OPPCA_BasicPull: Can't push. fsPullPath too long"));
	    return;
	}

	if (TRUE != OppcaBuildPullObject(&objToPull, fsPullPath, oppClient.pullBuff))
	{
		Report(("OPPCA_BasicPull: Can't pull"));
		return;
	}

	status = BTL_OPPC_Pull(oppcContext, &objToPull);
	Report(("BTL_OPPC_Pull() returned %s.",pBT_Status(status)));

	if (status == OB_STATUS_PENDING)
	{
		Report(("OPPCA_ObjectPull(): Started ..."));
		oppClient.currOper = OPPCA_OPER_BASIC_PULL;
	}
}


/*---------------------------------------------------------------------------
 *            OPPCA_BasicExchange
 *---------------------------------------------------------------------------
 *
 * Synopsis:  	Initiate an OBEX Business Card Exchange procedure.
 *			If the given BD_ADDR is NULL, a search is performed.
 *
 * Return:	void
 *
 */
void OPPCA_BasicExchange(const char *fsPushPath, const char *fsPullPath)
{
	BtStatus status;
	BtlObject objToPush, objToPull;
	
	Report(("OPPCA_BasicExchange: fsPushPath = %s, fsPullPath = %s", fsPushPath, fsPullPath));
    
	/* -------- Push Operation --------- */

	if (TRUE != OPPCA_CHECK_FS_NAME_LEN(fsPushPath))
	{
		Report(("OPPCA_BasicExchange: Can't push. fsPushPath too long"));
		return;
	}

	if (TRUE != OppcaBuildPushObject(&objToPush, fsPushPath, oppClient.pushBuff))
	{
		Report(("OPPCA_BasicExchange: Can't push"));
		return;
	}


	/* -------- Pull Operation --------- */

	if (TRUE != OPPCA_CHECK_FS_NAME_LEN(fsPullPath))
	{
		Report(("OPPCA_BasicExchange: Can't pull. fsPullPath too long"));
		return;
	}

	if (TRUE != OppcaBuildPullObject(&objToPull, fsPullPath, oppClient.pullBuff))
	{
		Report(("OPPCA_BasicExchange: Can't pull"));
		return;
	}
    
	status = BTL_OPPC_Exchange(oppcContext, &objToPush, &objToPull);
	Report(("BTL_OPPC_Exchange() returned %s.", pBT_Status(status)));
		
	if (status == OB_STATUS_PENDING)
	{
		Report(("OPPCA_BasicExchange(): Started ..."));
		oppClient.currOper = OPPCA_OPER_BASIC_EXCHANGE;
	}
}


/*---------------------------------------------------------------------------
 *            OPPCA_EncapsulatePush
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate an OBEX Object Push procedure.
 *            Includes Connect and Disconnect from the opp server.  
 *			  If the given BD_ADDR is NULL, a search is performed.
 *
 * Return:    void
 *
 */
void OPPCA_EncapsulatePush(const char *fsPushPath, BD_ADDR *bd_addr)
{
	BtStatus status;
	BtlObject objToPush;
	
	Report(("OPPCA_EncapsulatePush: fsPushPath = %s", fsPushPath));

	if (TRUE != OPPCA_CHECK_FS_NAME_LEN(fsPushPath))
	{
		Report(("OPPCA_EncapsulatePush: Can't push. fsPushPath too long"));
		return;
	}
	
	if (TRUE != OppcaBuildPushObject(&objToPush, fsPushPath, oppClient.pushBuff))
	{
		Report(("OPPCA_BasicPull: Can't pull"));
		return;
	}

	status = BTL_OPPC_ConnectPushDisconnect(oppcContext, bd_addr, &objToPush);

	Report(("BTL_OPPC_ConnectPushDisconnect() returned %s.", pBT_Status(status)));
	if (status == OB_STATUS_PENDING)
	{
		Report(("OPPCA_EncapsulatePush(): Started ..."));
		oppClient.currOper = OPPCA_OPER_ENC_PUSH;
	}
}


/*---------------------------------------------------------------------------
 *            OPPCA_EncapsulatePull
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate an OBEX Business Card Pull procedure.
 *            Includes Connect and Disconnect from the opp server.  
 *			  If the given BD_ADDR is NULL, a search is performed.
 *
 * Return:    void
 *
 */
void OPPCA_EncapsulatePull(const char *fsPullPath, BD_ADDR *bd_addr)
{
	BtStatus status;
	BtlObject objToPull;
	
	Report(("OPPCA_EncapsulatePull: fsPullPath = %s", fsPullPath));
   
	if (TRUE != OPPCA_CHECK_FS_NAME_LEN(fsPullPath))
	{
		Report(("OPPCA_EncapsulatePull: Can't pull. fsPullPath too long"));
		return;
	}
     
	if (TRUE != OppcaBuildPullObject(&objToPull, fsPullPath, oppClient.pullBuff))
	{
		Report(("OPPCA_EncapsulatePull: Can't pull"));
		return;
	}
    
	status = BTL_OPPC_ConnectPullDisconnect(oppcContext, bd_addr, &objToPull);
	
	Report(("BTL_OPPC_Pull() returned %s.", pBT_Status(status)));
		
	if (status == OB_STATUS_PENDING)
	{
		Report(("OPPCA_EncapsulatePull(): Started ..."));
		oppClient.currOper = OPPCA_OPER_ENC_PULL;
	}
}


/*---------------------------------------------------------------------------
 *            OPPCA_EncapsulateExchange
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate an OBEX Business Card Exchange procedure.
 *            Includes Connect and Disconnect from the opp server.  
 *			  If the given BD_ADDR is NULL, a search is performed.
 *
 * Return:    void
 *
 */
void OPPCA_EncapsulateExchange(const char *fsPushPath, const char *fsPullPath, BD_ADDR *bd_addr)
{
	BtStatus status;
	BtlObject objToPush, objToPull;
	
	Report(("OPPCA_EncapsulateExchange: fsPushPath = %s, fsPullPath = %s", fsPushPath, fsPullPath));


	/* -------- Push Operation --------- */
    
	if (TRUE != OPPCA_CHECK_FS_NAME_LEN(fsPushPath))
	{
		Report(("OPPCA_EncapsulateExchange: Can't push. fsPushath too long"));
		return;
	}

	if (TRUE != OppcaBuildPushObject(&objToPush, fsPushPath, oppClient.pushBuff))
	{
		Report(("OPPCA_EncapsulateExchange: Can't push"));
		return;
	}
   
	/* -------- Pull Operation --------- */

	if (TRUE != OPPCA_CHECK_FS_NAME_LEN(fsPullPath))
	{
		Report(("OPPCA_EncapsulateExchange: Can't pull. fsPullPath too long"));
		return;
	}

	if (TRUE != OppcaBuildPullObject(&objToPull, fsPullPath, oppClient.pullBuff))
	{
		Report(("OPPCA_EncapsulateExchange: Can't pull"));
		return;
	}


	status = BTL_OPPC_ConnectExchangeDisconnect(oppcContext, bd_addr, &objToPush, &objToPull);

	Report(("BTL_OPPC_ConnectExchangeDisconnect() returned %s.", pBT_Status(status)));
	
	if (status == OB_STATUS_PENDING)
	{
		Report(("OPPCA_EncapsulateExchange(): Started ..."));
		oppClient.currOper = OPPCA_OPER_ENC_EXCHANGE;
	}
}


/*---------------------------------------------------------------------------
 *            OPPCA_Callback
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes OBEX Client protocol events.
 *
 * Return:    void
 *
 */
static void OPPCA_Callback(const BtlOppcEvent *Event)
{
    BD_ADDR bdAddr;
	char addr[BDADDR_NTOA_SIZE];

#if (BTHAL_PLATFORM == PLATFORM_WINDOWS)
#else
    BtStatus status;
#endif

	switch (Event->oppcEvent->event)
	{
		case OPPC_EVENT_COMPLETE:
            
			UI_DISPLAY(("OPPCA_Callback: The current %s is complete.", OppcaGoepOpName(Event->oppcEvent->oper))); 

			OPP_PROGRESS(OPP_GetClientUIContext(), 0, 0);            
            
			switch (Event->oppcEvent->oper) 
			{
				case GOEP_OPER_CONNECT:
	                    
					UI_DISPLAY(("OPPCA_Callback: GOEP Connection has been established."));
	                    
					BTL_OPPC_GetConnectedDevice(oppcContext, &bdAddr);
						UI_DISPLAY(("OPPCA_Callback: Transport Connection is up to %s.", bdaddr_ntoa(&bdAddr, addr)));

					oppClient.state = OPPCA_CONNECTED;
					oppClient.isConnecting = FALSE;
					oppClient.isConnected = TRUE;
	                    
					if (OPPCA_OPER_CONNECT == oppClient.currOper)
					{
						oppClient.currOper = OPPCA_OPER_NONE;
					}
					break;
	           
				case GOEP_OPER_DISCONNECT:
	            
					UI_DISPLAY(("OPPCA_Callback: GOEP Connection has been disconnected."));
			    	    
					oppClient.isConnecting = FALSE;
					oppClient.isConnected = FALSE;
					oppClient.state = OPPCA_IDLE;
					oppClient.currOper = OPPCA_OPER_NONE;
					break;

				case GOEP_OPER_PULL:

					switch(oppClient.currOper)
					{
						case OPPCA_OPER_BASIC_PULL:
							oppClient.currOper = OPPCA_OPER_NONE;
							break;

						case OPPCA_OPER_BASIC_EXCHANGE:
							oppClient.currOper = OPPCA_OPER_NONE;
							 break;
					}
					break;


				case GOEP_OPER_PUSH:

					switch(oppClient.currOper)
					{
						case OPPCA_OPER_BASIC_PUSH:
							oppClient.currOper = OPPCA_OPER_NONE;
							break;
					}
					break;
			}
			break;

        case OPPC_EVENT_ABORTED:
			
		UI_DISPLAY(("OPPCA_Callback: %s operation failed.", OppcaGoepOpName(Event->oppcEvent->oper))); 
		OPP_PROGRESS(OPP_GetClientUIContext(), 0, 0);			

		oppClient.currOper = OPPCA_OPER_NONE;
		break;

        case OPPC_EVENT_TP_CONNECTED:
			
		BTL_OPPC_GetConnectedDevice(oppcContext, &bdAddr);
			UI_DISPLAY(("OPPCA_Callback: Transport Connection is up to %s.", bdaddr_ntoa(&bdAddr, addr)));

		oppClient.state = OPPCA_TP_CONNECTED;
		oppClient.isConnected = TRUE;
		break;

	case OPPC_EVENT_TP_DISCONNECTED:
			
		UI_DISPLAY(("OPPCA_Callback: Transport Connection has been disconnected."));
		OPP_PROGRESS(OPP_GetClientUIContext(), 0, 0);			
            
		oppClient.isConnected = FALSE;
		oppClient.isConnecting = FALSE;
		oppClient.currOper = OPPCA_OPER_NONE;
            
		if (oppClient.state != OPPCA_DISABLING)
		{
			oppClient.state = OPPCA_IDLE; 
		}
		break;

        
	case OPPC_EVENT_DISCOVERY_FAILED:
			
		UI_DISPLAY(("OPPCA_Callback: Discovery Failure."));
		oppClient.state = OPPCA_IDLE;
		break;

	case OPPC_EVENT_NO_SERVICE_FOUND:
			
		UI_DISPLAY(("OPPCA_Callback: No OBEX Service Found."));
		oppClient.state = OPPCA_IDLE;
		break;
		
	
	 case OPPC_EVENT_OBJECT_PROGRESS:

		OPP_PROGRESS(OPP_GetClientUIContext(), Event->oppcEvent->info.progress.currPos, 
		Event->oppcEvent->info.progress.maxPos); 
		break;
   

	case OPPC_EVENT_DISABLED:
            
		UI_DISPLAY(("OPPCA_Callback: Receiving OPPC_EVENT_DISABLED."));

#if (BTHAL_PLATFORM == PLATFORM_WINDOWS)
#else
		if (OPPCA_DISABLING == oppClient.state)
		{
			status = BTL_OPPC_Destroy(&oppcContext);

			Report(("BTL_OPPC_Destroy() returned %s.", pBT_Status(status)));  
		}
#endif
		
		oppClient.state = OPPCA_IDLE;
		break;

	default:

		UI_DISPLAY(("OPPCA_Callback: Unexpected event: %i", Event->oppcEvent->event));
		break;
    }
}


/*---------------------------------------------------------------------------
 *            OppcaGoepOpName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return a pointer to the name of the current operation.
 *
 * Return:    ASCII String pointer.
 *
 */
static const char *OppcaGoepOpName(GoepOperation Op)
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
 * OppcaBuildPushObject()
 *
 * Synopsis:  Build push object
 *
 * Return:    TRUE - success FALSE - failed
 *
 */
static BOOL OppcaBuildPushObject(BtlObject *objToPush, const char *fsPath, char *buff)
{
	if (oppClient.objLocation == BTL_OBJECT_LOCATION_MEM)
	{
		if (TRUE != OppcaInitMemPushObject(objToPush, fsPath, buff))
		{
			 return FALSE;
		}
	}
	else if (oppClient.objLocation == BTL_OBJECT_LOCATION_FS)
	{
		OppcaInitFsPushObject(objToPush, fsPath);
	}

	return TRUE;
}


/*---------------------------------------------------------------------------
 * OppcaBuildPullObject()
 *
 * Synopsis:  Build pull object
 *
 * Return:    TRUE - success FALSE - failed
 *
 */
static BOOL OppcaBuildPullObject(BtlObject *objToPull, const char *fsPath, char *buff)
{
	if (oppClient.objLocation == BTL_OBJECT_LOCATION_MEM)
	{
		OppcaInitMemPullObject(objToPull, fsPath, buff);
	}
	else if (oppClient.objLocation == BTL_OBJECT_LOCATION_FS)
	{
		OppcaInitFsPullObject(objToPull, fsPath);	
	}

	return TRUE;
}


/*---------------------------------------------------------------------------
 * OppcaInitMemPushObject()
 *
 * Synopsis:  Fills buff with pushed object data, and initialize 
 *            obj as memory Push object.
 *
 * Return:    void
 *
 */
static BOOL OppcaInitMemPushObject(BtlObject *obj, const char *fsPushPath, char *buff)
{
	BtFsStatus btFsStatus;
	BthalFsFileDesc fd;
	BTHAL_U32 numRead;
	BthalFsStat FileStat;

   	btFsStatus = BTHAL_FS_Stat((const BTHAL_U8 *)fsPushPath, &FileStat); 
	if(btFsStatus != BT_STATUS_HAL_FS_SUCCESS)
	{
		Report(("Client: Error - File was not found %s", fsPushPath));
		return FALSE;
	}
	
	if (FileStat.size > OPPCA_PUSH_BUFF_MAX)
	{
		Report(("Client: Error pushed file > %d bytes", OPPCA_PUSH_BUFF_MAX));
		return FALSE;
	}
       
	btFsStatus = BTHAL_FS_Open((const BTHAL_U8 *)fsPushPath, 
		            BTHAL_FS_O_RDONLY | BTHAL_FS_O_BINARY,      /* Read Only */
                            (BthalFsFileDesc *)&fd);
 
	if (btFsStatus == BT_STATUS_HAL_FS_SUCCESS)
	{
	        btFsStatus  = BTHAL_FS_Read(fd, oppClient.pushBuff, OPPCA_PUSH_BUFF_MAX, &numRead);
	        
	        if (btFsStatus == BT_STATUS_HAL_FS_SUCCESS)
		{
			Report(("Client: fsPushPath = %s, numRead = %d", fsPushPath, numRead));
	        }
			
	        BTHAL_FS_Close(fd);
		
	        /* Extract object name from fsPath */
	        OppcaExtractObjName(fsPushPath, ObPushName);
	        
		obj->objectLocation                 = BTL_OBJECT_LOCATION_MEM;
		obj->objectName                     = (const BtlUtf8 *)ObPushName;
		obj->location.memLocation.memAddr   = buff;
		obj->location.memLocation.size      = numRead;
		obj->objectMimeType 			    = OppcaConvertFileExtentionToMimeType(fsPushPath);
	}
	else
	{
		Report(("Client: Error - could not open %s", fsPushPath));
		return FALSE;
	}

	return TRUE;
}


/*---------------------------------------------------------------------------
 * OppcaInitMemPullObject()
 *
 * Synopsis:  Initialize obj as Memory Pull object.
 *
 * Return:    void
 *
 */
static void OppcaInitMemPullObject(BtlObject *obj, const char *fsPullPath, char *buff)
{			
	OppcaExtractObjName(fsPullPath, ObPullName);
    
	obj->objectLocation				= BTL_OBJECT_LOCATION_MEM;
	
	obj->location.fsLocation.fsPath		= NULL;
	obj->location.memLocation.memAddr	= buff;
	obj->location.memLocation.size		= OPPCA_PULL_BUFF_MAX;		

	obj->objectMimeType 		        = OppcaConvertFileExtentionToMimeType(fsPullPath);
	obj->objectName                     = (const BtlUtf8 *)ObPullName;
}


/*---------------------------------------------------------------------------
 * OppcaInitFsPushObject()
 *
 * Synopsis:  Initialize obj as FS Push object. 
 *
 * Return:    void
 *
 */
static void OppcaInitFsPushObject(BtlObject *obj, const char *fsPushPath)
{			
    OppcaExtractObjName(fsPushPath, ObPushName);

	obj->objectName				= (const BtlUtf8 *)ObPushName;
	obj->location.fsLocation.fsPath	= (const BtlUtf8 *)fsPushPath;
	obj->objectMimeType			= OppcaConvertFileExtentionToMimeType(fsPushPath);
	obj->objectLocation			= BTL_OBJECT_LOCATION_FS;
}


/*---------------------------------------------------------------------------
 * OppcaInitFsPullObject()
 *
 * Synopsis:  Initialize obj as FS Pull object. 
 *
 * Return:    void
 *
 */
static void OppcaInitFsPullObject(BtlObject *obj, const char *fsPullPath)
{			
	obj->objectName      			    = NULL;
	obj->location.fsLocation.fsPath     = (const BtlUtf8 *)fsPullPath;
	obj->objectMimeType 			    = OppcaConvertFileExtentionToMimeType(fsPullPath);
	obj->objectLocation 			    = BTL_OBJECT_LOCATION_FS;
}


/*---------------------------------------------------------------------------
 * OppcaExtractObjName()
 *
 * Extract object name from fsPath.
 *
 */
void OppcaExtractObjName(const char *source, char *dest)
{
	int i, len;

	len = OS_StrLenUtf8(source);
	len--;
	for ( ;(source[len] != BTHAL_FS_PATH_DELIMITER) && (len >= 0); len--)
	{
		/* do-nothing */
	}
		
	len++;
	for (i=0; source[len];i++, len++)
	{
		dest[i] = source[len];
	}
	
	dest[i] = '\0';	
}

/*---------------------------------------------------------------------------
 * OppcaExtractFileExtention()
 *
 * Extract object name from fsPath.
 *
 */
static const char * OppcaConvertFileExtentionToMimeType(const char *source)
{
	static char fileExtention[BTHAL_FS_MAX_FILE_NAME_LENGTH + 1] = {'\0'};
    
	static const char *mimeTypeStr[] = OPPCA_MIME_TYPE_NAMES;
	static const char *fileExtenionStr[] = OPPCA_FILE_EXTENTION_NAMES;

	int i, len;

	len = OS_StrLenUtf8(source);
	len--;
	for ( ;(source[len] != '.') && (len >= 0); len--)
	{
		/* do-nothing */
	}
		
	len++;
	for (i=0; source[len];i++, len++)
	{
		fileExtention[i] = source[len];
	}
	
	fileExtention[i] = '\0';	


	/* Convert File Extention to MimeType */
	for (i = 0; fileExtenionStr[i]; i++)
	{
		if (0 == OS_StriCmp(fileExtenionStr[i], fileExtention))
		{
			break;
		}
	}

	if (0 == fileExtenionStr[i])
	{
		i = 0;    
	}  

	return mimeTypeStr[i];
}

/*---------------------------------------------------------------------------
 * OppcaIsOperationInProgress()
 *
 * Checks if there is already operation in progress.
 * 
 * Return:	TRUE - operation is in progress
 *			FALSE - no operation is running
 *
 */
BOOL OppcaIsOperationInProgress(void)
{
	BOOL status = FALSE;

	switch (oppClient.currOper)
	{
		case OPPCA_OPER_CONNECT:
			Report(("OPPCA_Connect: operation is already in progress ..."));
			status = TRUE;
			break;

		case OPPCA_OPER_BASIC_PULL:
			Report(("OPPCA_BasicPull: operation is already in progress ..."));
			status = TRUE;
			break;

		case OPPCA_OPER_BASIC_PUSH:
			Report(("OPPCA_BasicPush: operation is already in progress ..."));
			status = TRUE;
			break;

		case OPPCA_OPER_BASIC_EXCHANGE:
			Report(("OPPCA_BasicExchange: operation is already in progress ..."));
			status = TRUE;
			break;

		case OPPCA_OPER_ENC_PULL:                            
			Report(("OPPCA_EncapsulatePull: operation is already in progress ..."));
			status = TRUE;
			break;

		case OPPCA_OPER_ENC_PUSH:
			Report(("OPPCA_EncapsulatePush: operation is already in progress ..."));
			status = TRUE;
			break;

		case OPPCA_OPER_ENC_EXCHANGE:
			Report(("OPPCA_EncapsulateExchange: operation is already in progress ..."));
			status = TRUE;
			break;
        
		case OPPCA_OPER_NONE:
		default:
			break;
    }

    return status;
}

#else /* BTL_CONFIG_OPP == BTL_CONFIG_ENABLED */

void OPPCA_Init(void)
{
	
	Report(("OPPCA_Init -BTL_CONFIG_OPP is disabled"));
}


void OPPCA_Deinit(void)
{

	Report(("OPPCA_Deinit -BTL_CONFIG_OPP is disabled"));
}



#endif /* BTL_CONFIG_OPP == BTL_CONFIG_ENABLED*/