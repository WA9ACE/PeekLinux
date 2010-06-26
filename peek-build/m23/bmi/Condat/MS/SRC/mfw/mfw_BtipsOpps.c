/* =========================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================== */
/*
 $Project name: Basic Bluetooth MMI
 $Project code:
 $Module:		Bluetooth BMG MFW
 $File:			mfw_BtipsOpps.c
 $Revision:		1.0
 $Author:		Texas Instruments
 $Date:			26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS BMG APPlication functionality.

********************************************************************************
 $History: mfw_BtipsOpps.c

  26/06/07 Sasken original version

 $End

*******************************************************************************/




/*******************************************************************************

                                Include files

*******************************************************************************/

#define ENTITY_MFW
/* includes */

#include <string.h>

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#ifdef FF_MMI_BTIPS_APP
#include "btl_bmg.h"

/* Macro for sending events to UI - meanwhile, trace message to PCO Viewer */
extern int vsi_o_state_ttrace ( const char * const format, ... );
#define UI_DISPLAY(s) vsi_o_state_ttrace s

#include "osapi.h"
#include "btl_common.h"
#include "debug.h"

#include "btl_opps.h"
#include "btl_config.h"
#include "bthal_config.h"
#include "bthal_fs.h"

#include "Mfw_mfw.h"
#include "Mfw_Btips.h"



/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

#define OPPSA_DEFAULT_VCARD_NAME     "mycard.vcf"
#define OPPSA_ACCEPT_DEFAULT_DIR     "/MfwBtDemo"

#define OPPSA_DEFAULT_FULL_NAME      "/MfwBtDemo/default/mycard.vcf"

#define OPPSA_SERVER_NAME            "OBEX Object Push"

#define OPPSA_DEFAULT_FULL_DIR "/MfwBtDemo"

#define OPPSA_NEW_PUSH_NAME          "pushed_file.vcf"//sundeep

#define OPPSA_PULL_USE_DEFAULT_OBJ     				(0x01)//sundeep
#define OPPSA_PULL_USE_NEW_OBJ           			(0x02)//sundeep	
#define OPPSA_NEW_VCARD_NAME         "mycard_t.vcf"
#define OPPSA_NEW_FULL_NAME          "/QbInbox/default_object/mycard_t.vcf"
typedef U8 OppsaPullUseObject;

 /*******************************************************************************
 *
 * Macro definitions
 *
 ******************************************************************************/
#define OPPSA_BUFF_MAX                    (1 * 512)


#define OPPSA_CHECK_OBJ_LOCATION(objectLocation)                  \
          ((objectLocation == BTL_OBJECT_LOCATION_FS) ||         \
              (objectLocation == BTL_OBJECT_LOCATION_MEM))
//sundeep
static char userPushFileName[BTHAL_FS_MAX_FILE_NAME_LENGTH + 1] = {'\0'};
static char userPushPath[BTHAL_FS_MAX_PATH_LENGTH + 1] = {'\0'};
static char userPushFsPath[BTHAL_FS_MAX_FILE_NAME_LENGTH + BTHAL_FS_MAX_PATH_LENGTH + 1] = {'\0'};


/*******************************************************************************
 *
 * OPP External Function prototypes
 *
 ******************************************************************************/
#if 1
void mfw_btips_oppsInit(void);
void mfw_btips_oppsDeinit(void);
void mfw_btips_opps_AbortServer(void);
void mfw_btips_opps_Disconnect(void);
void mfw_btips_opps_GetConnectedDevice(void);
BOOL mfw_btips_opps_GetSecurityLevel(void);
BOOL mfw_btips_opps_SetSecurityLevel(BtSecurityLevel level);
void mfw_btips_oppsAcceptObjectRequest(BOOL accept);
void OPPSA_SetObjLocation(BtlObjectLocation objLocation);
#endif


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
    OppsaState          state;

    BOOL                isConnected;

    BOOL 	            operationOngoing;

    BtSecurityLevel     securityLevel;

    OppsOperation       serverOperation;  
    
    BtlObjectLocation   objLocation;
    
    /* Memory Objects */
    char                pushBuff[OPPSA_BUFF_MAX];
    char                pullBuff[OPPSA_BUFF_MAX];

} OppsaData;


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

static OppsaData           oppServer;		

static BtlOppsContext      *oppsContext = 0;   


/*******************************************************************************
 *
 * Internal Function prototypes
 *
 ******************************************************************************/
static void btips_oppsCallback(const BtlOppsEvent *Event);
static const char *mfw_btips_oppsGoepOpName(GoepOperation Op);
//static void mfw_btips_oppsSetDefaultObject(BtlObject *obj, const char *ObName, const char* fullPath);//sundeep
static void mfw_btips_oppsSetDefaultObject(BtlObject *obj, OppsaPullUseObject useObj);
static void mfw_btips_oppsSetToMemObject(BtlObject *obj, char *buff);
static void mfw_btips_oppsSetToFsObject(BtlObject *obj, char *userPushFileName, char *userPushPath);
static void mfw_btips_oppsCreateDefaultVCardFile(void);
BOOL mfw_btips_oppsGetConnectedDevice (BD_ADDR *bdAddr, U8 *name);
BOOL mfw_btips_OppsDisconnect ();

/*---------------------------------------------------------------------------
 *            mfw_btips_oppsInit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX server for the OPP application 
 *            and the Object Store.
 *
 * Return:    voidOppsaPullUseObject useObj
 *
 */
void mfw_btips_oppsInit(void)
{
    BtStatus status;
    BtlObject serverObject;    
    
    oppServer.securityLevel = BSL_NO_SECURITY;
    oppServer.state = OPPSA_IDLE;
    oppServer.isConnected = FALSE;
    oppServer.operationOngoing = FALSE;
    oppServer.objLocation = BTL_OBJECT_LOCATION_FS;

	status = BTL_OPPS_Create((BtlAppHandle *)0, btips_oppsCallback, &oppServer.securityLevel, &oppsContext);	
    Report(("BTL_OPPS_Create() returned %s.",pBT_Status(status)));

	mfw_btips_oppsCreateDefaultVCardFile();
	
    //mfw_btips_oppsSetDefaultObject(&serverObject, OPPSA_DEFAULT_VCARD_NAME, OPPSA_DEFAULT_FULL_NAME);//sundeep
	mfw_btips_oppsSetDefaultObject(&serverObject, OPPSA_PULL_USE_DEFAULT_OBJ);
    status = BTL_OPPS_SetDefaultObject(&serverObject);
    Report(("BTL_OPPS_SetDefaultObject() returned %s.",pBT_Status(status)));

    status = BTL_OPPS_Enable(oppsContext, "OPPS Server", BTL_OPPS_SUPPORTED_FORMATS_ANY);
    Report(("BTL_OPPS_Enable() returned %s.",pBT_Status(status)));
}


/*---------------------------------------------------------------------------
 *            mfw_btips_oppsDeinit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the OBEX Server protocol.
 *
 * Return:    voidOPPSA_PULL_USE_DEFAULT_OBJ
 *
 */

void mfw_btips_oppsDeinit(void)
{
	BtStatus status;

	status = BTL_OPPS_Disable(oppsContext);
	
    Report(("BTL_OPPS_Disable() returned %s.",pBT_Status(status)));

    if (status == BT_STATUS_SUCCESS)
    {
    status = BTL_OPPS_Destroy(&oppsContext);
    
    Report(("BTL_OPPS_Destroy() returned %s.", pBT_Status(status)));

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
 *            mfw_btips_opps_AbortServer
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Requests the current operation to be aborted.
 *
 * Return:    void
 *
 */
void mfw_btips_opps_AbortServer(void)
{
    BtStatus status;

    if (oppServer.operationOngoing == TRUE)
    {
        status = BTL_OPPS_Abort(oppsContext, OBRC_CONFLICT);
        Report(("BTL_OPPS_Abort() returned %s.",pBT_Status(status)));
    }
	else
    {
        Report(("mfw_btips_opps_AbortServer: No ongoing OPP operation!"));
    }
}


/*---------------------------------------------------------------------------
 *            mfw_btips_opps_Disconnect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnects 
 *
 */
void mfw_btips_opps_Disconnect(void)
{
	BtStatus status; 	

    if (oppServer.isConnected == TRUE)
	{
        status = BTL_OPPS_Disconnect(oppsContext);
		Report(("BTL_OPPS_Disconnect() returned %s.", pBT_Status(status)));
	}
	else
	{
		Report(("mfw_btips_opps_Disconnect: Server is not connected!"));
	}
}


     

/*---------------------------------------------------------------------------
 *            mfw_btips_opps_SetSecurityLevel
 *
 * Synopsis:  Set the OPP Server security level.
 *
 * Return:    void
 *
 */
BOOL mfw_btips_opps_SetSecurityLevel(BtSecurityLevel level)
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
 *            mfw_btips_opps_GetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Gets security level for the given OPPC context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
BOOL mfw_btips_opps_GetSecurityLevel(void)
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
void mfw_btips_oppsAcceptObjectRequest(BOOL accept)
{
	BtStatus    status;
    BtlObject	serverObject;    

	TRACE_FUNCTION_P1("mfw_btips_oppsAcceptObjectRequest - %d", accept);
    if (TRUE != OPPSA_CHECK_OBJ_LOCATION(oppServer.objLocation))
    {
        Report(("OPPSA_AcceptObjectRequest: Invalid objectLocation"));
        return;
    }

	if (accept == FALSE)
	{
		mfw_btips_oppsSetToFsObject(&serverObject, userPushFileName, OPPSA_ACCEPT_DEFAULT_DIR);
					
		status = BTL_OPPS_AcceptObjectRequest(oppsContext, FALSE, &serverObject);

        Report(("BTL_OPPS_AcceptObjectRequest() returned %s.", pBT_Status(status)));
	}
    else
    {
		if (oppServer.serverOperation == GOEP_OPER_PUSH)
		{
			if (oppServer.objLocation == BTL_OBJECT_LOCATION_MEM)
			{
                mfw_btips_oppsSetToMemObject(&serverObject, oppServer.pushBuff);
			}
            else if (oppServer.objLocation == BTL_OBJECT_LOCATION_FS)
            {
                mfw_btips_oppsSetToFsObject(&serverObject, userPushFileName, OPPSA_ACCEPT_DEFAULT_DIR);
            }
		}
		else if (oppServer.serverOperation == GOEP_OPER_PULL)
		{
    		if (oppServer.objLocation == BTL_OBJECT_LOCATION_MEM)
			{
				mfw_btips_oppsSetToMemObject(&serverObject, oppServer.pullBuff);
			}
            else if (oppServer.objLocation == BTL_OBJECT_LOCATION_FS)
            {
                mfw_btips_oppsSetToFsObject(&serverObject, userPushFileName, OPPSA_ACCEPT_DEFAULT_DIR);
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
 *            btips_oppsCallback
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes OBEX server protocol events.
 *
 * Return:    void
 *
 */
void btips_oppsCallback(const BtlOppsEvent *Event)
{
	/* Local variables */
	static BD_ADDR	bdAddr;
	char 		addr[BDADDR_NTOA_SIZE];
	BtStatus 		status = BT_STATUS_FAILED;
	BtlBmgDeviceRecord 	record;
	T_BTIPS_MMI_IND		btips_mmi_ind;
				
	btips_mmi_ind.mfwBtipsEventType = MFW_BTIPS_OPPS_EVENT;

	TRACE_EVENT_P1("btips_oppsCallback %d", Event->oppsEvent->event);
	/* Save the GOEP operation for future use */
	oppServer.serverOperation = Event->oppsEvent->oper;

	switch (Event->oppsEvent->event) 
	{
           case OPPS_EVENT_START:
		    TRACE_EVENT_P1("Server: Starting %s operation.", mfw_btips_oppsGoepOpName(Event->oppsEvent->oper));
		    oppServer.operationOngoing = TRUE;
            break;

        case OPPS_EVENT_COMPLETE:	    
		    TRACE_EVENT_P1("Server: %s operation complete.", mfw_btips_oppsGoepOpName(Event->oppsEvent->oper));     
            oppServer.operationOngoing = FALSE;
//#if 0
		if (Event->oppsEvent ->oper == GOEP_OPER_PUSH)
		{
			btips_mmi_ind.data.oppsBtEvent.mfwOppsEventType = MFW_BTIPS_OPPS_PUSH_COMP_EVENT;
		}
		if(Event->oppsEvent ->oper == GOEP_OPER_PULL)
		{
			btips_mmi_ind.data.oppsBtEvent.mfwOppsEventType = MFW_BTIPS_OPPS_PULL_COMP_EVENT;
		}
		btipsNotifyMFW(&btips_mmi_ind);
#if 0		
		if (Event->oppsEvent ->oper == GOEP_OPER_DISCONNECT)
		{
			btips_mmi_ind.data.oppsBtEvent.mfwOppsEventType = MFW_BTIPS_OPPS_DISCONNECT_EVENT;
		}
		btipsNotifyMFW(&btips_mmi_ind);
#endif
            break;

        case OPPS_EVENT_ABORTED:
		    TRACE_EVENT_P1("Server: GOEP %s operation failed or aborted.", mfw_btips_oppsGoepOpName(Event->oppsEvent->oper));
            oppServer.isConnected = FALSE;
			oppServer.operationOngoing = FALSE;
            oppServer.state = OPPSA_IDLE;
            break;

        case OPPS_EVENT_TP_CONNECTED:  
			BTL_OPPS_GetConnectedDevice(oppsContext, &bdAddr);
			TRACE_EVENT_P1("Server: Transport Connection is up to %s.", bdaddr_ntoa(&bdAddr, addr));
            oppServer.state = OPPSA_TP_CONNECTED;
            oppServer.isConnected = TRUE;
        	break;

    	case OPPS_EVENT_TP_DISCONNECTED:
            TRACE_EVENT("Server: Transport Connection has been disconnected.");        
        	oppServer.isConnected = FALSE;
			oppServer.operationOngoing = FALSE;
        	if (oppServer.state != OPPSA_DISABLING)
            {
            oppServer.state = OPPSA_IDLE;
            }
            break;
	
       	case OPPS_EVENT_OBJECT_PROGRESS:
            TRACE_EVENT("OPPSA_Callback: progress bar ...");
        	break;

	case OPPS_EVENT_OBJECT_REQUEST:
		TRACE_EVENT_P1("Server: Incoming object request: %s operation.", mfw_btips_oppsGoepOpName(Event->oppsEvent->oper));	
		TRACE_EVENT_P3("Server: Name: %s, Type: %s, Size: %u.", Event->oppsEvent->info.request.name, 
				Event->oppsEvent->info.request.type, Event->oppsEvent->info.request.objectLen);
		TRACE_EVENT("Server: Please accept or reject!");
		status = BTL_BMG_DDB_FindRecord(bdAddr, &record);
		Report(("BTL_BMG_DDB_FindRecord() returned %s.", pBT_Status(status)));
		btips_mmi_ind.data.oppsBtEvent.mfwOppsEventType = Event->oppsEvent ->oper;
		strcpy(btips_mmi_ind.data.oppsBtEvent.p.opps.deviceName, record.name);
		strcpy(btips_mmi_ind.data.oppsBtEvent.p.opps.oppObjectName, Event ->oppsEvent->info.request.name);
#if 1
		btipsNotifyMFW(&btips_mmi_ind);
#else	
		mfw_btips_oppsAcceptObjectRequest(TRUE);
#endif		
        	break;

        case OPPS_EVENT_DISABLED: 
            UI_DISPLAY(("Server: Receiving OPPS_EVENT_DISABLED."));
            if (OPPSA_DISABLING == oppServer.state)
            {
                status = BTL_OPPS_Destroy(&oppsContext);
                Report(("BTL_OPPS_Destroy() returned %s.", pBT_Status(status)));
            }
            oppServer.state = OPPSA_IDLE;
            break;

        default:	
            UI_DISPLAY(("Server: Unexpected event: %i", Event->oppsEvent->event));
        	break;

    }
}

/*---------------------------------------------------------------------------
 *            mfw_btips_oppsCallback
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes OBEX server protocol events.
 *
 * Return:    void
 *
 */
void mfw_btips_oppsCallback(T_BTIPS_MMI_IND *para)
{

	MfwBtOppsEvent oopsBtEvent = para->data.oppsBtEvent;
	TRACE_FUNCTION_P1("mfw_btips_oppsCallback %d", oopsBtEvent.mfwOppsEventType);
	
	switch(oopsBtEvent.mfwOppsEventType)
	{
		case OPPS_OPER_PUSH:
			mfw_btips_signal(E_BTIPS_OPPS_PUSH_EVENT, para);
		break;
		
		case OPPS_OPER_PULL:
			mfw_btips_signal(E_BTIPS_OPPS_PULL_EVENT, para);
		break;
//#if 0		
		case MFW_BTIPS_OPPS_PUSH_COMP_EVENT:
			mfw_btips_signal(E_BTIPS_OPPS_PUSH_COMP_EVENT, para);
		break;

		case MFW_BTIPS_OPPS_PULL_COMP_EVENT:
			mfw_btips_signal(E_BTIPS_OPPS_PULL_COMP_EVENT, para);
//#endif
	}

}

/*---------------------------------------------------------------------------
 *            mfw_btips_oppsGoepOpName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return a pointer to the name of the current operation.
 *
 * Return:    ASCII String pointer.
 *
 */
static const char *mfw_btips_oppsGoepOpName(GoepOperation Op)
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
 * mfw_btips_oppsSetDefaultObject()
 *
 * Synopsis:  Function for setting the OPPS default object.
  *
 * Return:    void
 *
 */
//static void mfw_btips_oppsSetDefaultObject(BtlObject *obj, const char *ObName, const char* fullPath)
static void mfw_btips_oppsSetDefaultObject(BtlObject *obj, OppsaPullUseObject useObj)
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
   // obj->objectName      			    = ObName;
	//obj->location.fsLocation.fsPath     = fullPath;  
	
    obj->objectMimeType 			    = "text/x-vCard";
	obj->objectLocation 			    = oppServer.objLocation;
}

/*---------------------------------------------------------------------------
 * mfw_btips_oppsSetToMemObject()
 *
 * Synopsis:  Function for testing OPP from memory objects. 
 *            Sets memory object fields prior to PULL operation
 *
 * Return:    void
 *
 */
static void mfw_btips_oppsSetToMemObject(BtlObject *obj, char *buff)
{			
	obj->objectLocation                 = BTL_OBJECT_LOCATION_MEM;
	obj->location.fsLocation.fsPath     = NULL;
	obj->location.memLocation.memAddr   = buff;
	obj->location.memLocation.size      = OPPSA_BUFF_MAX;		

    obj->objectMimeType 		        = NULL;
}

/*---------------------------------------------------------------------------
 * mfw_btips_oppsSetToFsObject()
 *
 * Synopsis:  Function for testing OPP from memory objects. 
 *            Sets FS object
 *
 * Return:    void
 *
 */
static void mfw_btips_oppsSetToFsObject(BtlObject *obj, char *userPushFileName, char *userPushPath)
{			
    U16 len;
	/* ToDo: obj->objectName 
    
      [NULL]- pushed objects are saved in C:\QInbox location
      
      ["defaultPushed.vcf"] - pushed objects are saved in C:\QInbox location, 
         and then rename to "defaultPushed.vcf" file.               
     */
    
    //obj->objectName 			        = "defaultPushed.vcf";  
    obj->objectName 			        = NULL;  
    obj->objectMimeType 		        = NULL;
    obj->objectLocation 		        = BTL_OBJECT_LOCATION_FS;
    //obj->location.fsLocation.fsPath     = OPPSA_ACCEPT_DEFAULT_DIR;
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
 *            OppsaUiCreateDefaultVCardFile
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Creates the default VCard file 
 *
 * Return:    void.
 *
 */
static void mfw_btips_oppsCreateDefaultVCardFile(void)
{
	static char  content[] = "BEGIN:VCARD\nVERSION:2.1\nN:Adam;Smith\nFN:Adam Smith\nTEL;WORK;VOICE:erez972-76271234\nTEL;CELL:972-9-54-785345\n \
     X-IRMC-LUID:0000000089112FB8F5ED98469F141223445D85EC0700651F055323C0104ABA16A3A82F42C81E000000D0841B0000B783C70B99B5BE4CAF782AB8DD188B6D0000DF0956010000\n \
     END:VCARD";

	BtFsStatus btFsStatus;
    BthalFsFileDesc fp;
    BTHAL_U32 pNumWritten;
    BTHAL_INT nSize;
    
	/* Creates / Overwrites the current default VCard file */
	
    //BTHAL_FS_Remove(OPPSA_DEFAULT_FULL_NAME);
    btFsStatus = BTHAL_FS_Open((const BTHAL_U8 *)OPPSA_DEFAULT_FULL_NAME, 
    			BTHAL_FS_O_RDWR| BTHAL_FS_O_BINARY, (BthalFsFileDesc *)&fp);
    if (BT_STATUS_HAL_FS_SUCCESS == btFsStatus)
    {
    		BTHAL_FS_Close(fp);
		return;
    }		
    btFsStatus = BTHAL_FS_Open((const BTHAL_U8 *)OPPSA_DEFAULT_FULL_NAME, 
                        BTHAL_FS_O_CREATE | BTHAL_FS_O_RDWR | BTHAL_FS_O_BINARY,  /* Create file */
                            (BthalFsFileDesc *)&fp);
    if (BT_STATUS_HAL_FS_SUCCESS != btFsStatus)
    {
        Report(("OppsaUiCreateDefaultVCardFile(): Error - could not create Default VCard file %s.", OPPSA_DEFAULT_FULL_NAME));
        Report(("Make sure that %s directory exists !.", OPPSA_DEFAULT_FULL_DIR));
        return;
    }
    
    nSize = OS_StrLen(content);

    if (BT_STATUS_HAL_FS_SUCCESS == BTHAL_FS_Write(fp, (void*)content, nSize, &pNumWritten))
	{
		Report(("OppsaUiCreateDefaultVCardFile(): Default VCard file \"%s\" created successfully.", OPPSA_DEFAULT_FULL_NAME));
	}
	else
	{
		Report(("OppsaUiCreateDefaultVCardFile(): Error - Unable to create default VCard file."));
	}

	
    BTHAL_FS_Close(fp);

    Report(("OppsaUiCreateDefaultVCardFile(): nSize = %d, pNumWritten = %d", nSize, pNumWritten));
}

/*******************************************************************************

 $Function:     mfw_btips_oppsGetConnectDevice

 $Description: Returns the TRUE when succeeds
 $Returns:		TRUE or FALSE 

 $Arguments:		

*******************************************************************************/

BOOL mfw_btips_oppsGetConnectedDevice (BD_ADDR *bdAddr, U8 *name)
{
	BtStatus status;
	
	
	status = BTL_OPPS_GetConnectedDevice (oppsContext, bdAddr);
	
	
	
	
	if (status == BT_STATUS_SUCCESS)
	{
		mfw_btips_bmgGetDeviceName (bdAddr, name);
		TRACE_FUNCTION_P1("mfw_btips_oppsGetConnectDevice OPPS name %s  ",name);
		return TRUE;
	}
	return FALSE;
}



#endif

