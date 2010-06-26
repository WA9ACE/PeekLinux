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
*   FILE NAME:      bpp_app.c
*
*   DESCRIPTION:	This file contains the implementation of the BPP application primitives (user actions)
*
*   AUTHOR:         Uzi Doron
*
\*******************************************************************************/
#include "btl_config.h"
#include "osapi.h"
#if BTL_CONFIG_BPPSND == BTL_CONFIG_ENABLED

/* GPF Includes */
#include "typedefs.h"
#include "vsi.h"
#include "../p_btt.h"

/* BTIPS Includes */
#include "me.h"

#include "../app_main.h"
#include "btl_common.h"
#include "bthal_btdrv.h"
#include "bthal_pm.h"
#include "bttypes.h"
#include "goep.h"
#include "bpp.h"
#include "lineparser.h"

/* BTL inclides */
#include "btl_bppsnd.h"


#define	IMAGE_FILE_NAME	"/bpp/image.jpg"

#ifndef OBEX_AUTHENTICATION
#define	OBEX_AUTHENTICATION	XA_ENABLED
#endif


/* Internal functions prototypes */
void BPPA_Init( void );
void BPPA_Deinit( void );
static void App_Bpp_Connect( void);
static void App_Bpp_Create( void );
static void App_Bpp_Destroy( void );
static void App_Bpp_Disconnect( void );
static void App_Bpp_Enable( void );
static void App_Bpp_Disable( void );
static void App_Bpp_SimplePush( void );
static void App_Bpp_Abort( void );
static void BtEventHandler(const BtEvent *Event);
static void HandleBtlBppsndEvent(const BtlBppsndEvent *event);
static void AppSendReferenceObjectResponse(BtlBppsndContext 	*bppsndContext, BppCallbackParms *parms);
static void App_UpdateProgress(BppData *data);
#if OBEX_AUTHENTICATION == XA_ENABLED
static void AppObexAuthenticationResponse( void );
#endif
static void AppCreateJobCompleted( BppCallbackParms *parms );
static void AppCancelJobCompleted( BppCallbackParms *parms );
static void AppGetJobAttrCompleted( BppCallbackParms *parms );
static void AppGetEventReceived( BppCallbackParms *parms );
static void AppSendFileToPrintCompleted( BppCallbackParms *parms , char *name);
static void AppGetPrtAttrCompleted( BppCallbackParms *parms );
static void AppConvertToMemObject(BtlObject *objToPrint);
static const char *pBT_Status(I8 Status);
static const char *GetObexRespCodeStr( ObexRespCode ObexRC);
static const char *pBppOp(BppOp bppOp);
static const char *GetObexAbortReasonStr( ObexAbortReason abortReason);


#define BPP_COMMANDS_NUM 8

typedef void (*FuncType)(void);

typedef struct
{
	char 		funcName[LINE_PARSER_MAX_STR_LEN];
	FuncType    funcPtr;
} _bppAppCommands;

/* PBAP commands array */
static const  _bppAppCommands bppAppCommands[] =    {   {"connect", 	App_Bpp_Connect},
														{"disconnect", 	App_Bpp_Disconnect},
														{"abort",       App_Bpp_Abort}, 		     	
														{"enable",      App_Bpp_Enable}, 		    
														{"disable",     App_Bpp_Disable},		     	 
														{"create",      App_Bpp_Create},			    
														{"destroy",     App_Bpp_Destroy},	
														{"push",        App_Bpp_SimplePush},
													
											        };

/* Internal Global variables for BPP Sender*/
static BtHandler			bppAppGlobHandler;
static BtRemoteDevice		*bppAppRemDev = 0;
static BtSecurityLevel		bppAppSecurityLevel = BSL_NO_SECURITY;
static BtlBppsndContext 	*bppsndContext;
static BppService			bppService;
static char					bppAppUserId[BPP_MAX_USERID_LEN];
static char					bppAppPassword[BPP_MAX_PASSWORD_LEN];
static BOOL  				referencedObjFromMem = FALSE; // when true - convert a referenced file into memory object before sending to printer

/*******************************************************************************\
*  FUNCTION:	BPPA_ProcessUserAction
*                                                                         
*  PARAMETERS:	msg - buffer holding primitive string
*                                                                         
*  DESCRIPTION:	Processes primitives for BPP Sample APP.
*                                                                         
*  RETURNS:		None.
\*******************************************************************************/
void BPPA_ProcessUserAction(U8 *msg)
{
	U8 i;
	char command[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS status;
	
	status = LINE_PARSER_GetNextStr((U8 *)command, LINE_PARSER_MAX_STR_LEN);

	if (status != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("Wrong BPP command. Can't parse line"));
		return;
	}

	/* Look for the correct function */
	for(i = 0; i < BPP_COMMANDS_NUM; i++)
	{
		if (OS_StrCmp(command, bppAppCommands[i].funcName) == 0)
		{
			bppAppCommands[i].funcPtr();
			return;
		}
	}
	Report(("Wrong BPP command"));

}

void BPPA_Init( void )
{
	BtStatus			status;

	OS_StrCpy(bppAppPassword, "");
	OS_StrCpy(bppAppUserId, "");
	
	/* Register the global handler */
	//ME_InitHandler(&bppAppGlobHandler);  //Adib Nep - PC merge.
	bppAppGlobHandler.callback = BtEventHandler;
	//ME_RegisterGlobalHandler(&bppAppGlobHandler);  //Adib Nep - PC merge.
	//ME_SetEventMask(&bppAppGlobHandler, BEM_LINK_CONNECT_IND |  //Adib Nep - PC merge.
	//		BEM_LINK_CONNECT_CNF |	BEM_LINK_DISCONNECT);

	status = BTL_BPPSND_Create(
				(BtlAppHandle *) 0,
				HandleBtlBppsndEvent,
				0, /* &securityLevel, */
				&bppsndContext);
		if ( BT_STATUS_SUCCESS != status )
		{
			Report(("ERROR: BTL_BPPSND_Create() returned %s", pBT_Status(status)));
		}
		else
		{
			Report(("BTL_BPPSND_Create() success"));
			status = BTL_BPPSND_Enable(bppsndContext);
			Report(("BTL_BPPSND_Enable returned %s", pBT_Status(status)));
			
		}


}

void BPPA_Deinit( void )
{
	BtStatus			status;
	
	status = BTL_BPPSND_Disable(bppsndContext);
	if ( BT_STATUS_SUCCESS != status )
	{
		Report(("ERROR: BTL_BPPSND_Disable() returned %s", pBT_Status(status)));
		return;
	}
	Report(("BTL_BPPSND_Disable() completed successfully"));
	
	status = BTL_BPPSND_Destroy(&bppsndContext);
	if ( BT_STATUS_SUCCESS != status )
	{
		Report(("ERROR: BTL_BPPSND_Destroy() returned %s", pBT_Status(status)));
	}
	else
	{
		Report(("BTL_BPPSND_Destroy() completed successfully"));
	}

}


void App_Bpp_Connect()
{

	U8 					BdArray[17];
	BtStatus    		status;
	BD_ADDR 			BdAddr;
	LINE_PARSER_STATUS  ParserStatus;
	ParserStatus = LINE_PARSER_GetNextStr(BdArray, 17);

	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("BPP wrong BD addr. Can't connect."));
		return;
	}
	
	BdAddr = bdaddr_aton((const char *)(BdArray));
	
	status = BTL_BPPSND_Connect( bppsndContext, &BdAddr);
	Report(("BTL_BPPSND_Connect returned %s", pBT_Status(status)));
}


void App_Bpp_Disconnect( void )
{
	BtStatus			status;
	
	status = BTL_BPPSND_Disconnect( bppsndContext);
	Report(("BTL_BPPSND_Disconnect returned %s", pBT_Status(status)));
}

static void App_Bpp_Create( void )
{
	BtStatus			status;
	
	status = BTL_BPPSND_Create(
				(BtlAppHandle *) 0,
				HandleBtlBppsndEvent,
				0, /* &securityLevel, */
				&bppsndContext);

	Report(("BTL_BPPSND_Create returned %s", pBT_Status(status)));

}

static void App_Bpp_Destroy( void )
{
	BtStatus			status;
	
	status = BTL_BPPSND_Destroy(&bppsndContext);
	if ( BT_STATUS_SUCCESS != status )
	{
		Report(("ERROR: BTL_BPPSND_Destroy() returned %s", pBT_Status(status)));
	}
	else
	{
		Report(("BTL_BPPSND_Destroy() completed successfully"));
	}
}

void App_Bpp_Enable( void )
{
	BtStatus			status;
	status = BTL_BPPSND_Enable(bppsndContext);
	Report(("BTL_BPPSND_Enable returned %s", pBT_Status(status)));
	
}

void App_Bpp_Disable( void )
{
	BtStatus			status;
	status = BTL_BPPSND_Disable(bppsndContext);
	Report(("BTL_BPPSND_Disable returned %s", pBT_Status(status)));
}

void App_Bpp_SimplePush( )
{
	BtStatus			status;
	LINE_PARSER_STATUS  ParserStatus;
	static char myObjectName[] = "myObjectName";
	
	static BtlObject objToPrint;
	static char fileName[LINE_PARSER_MAX_STR_LEN];
	static char mimeType[LINE_PARSER_MAX_STR_LEN];

	/* Get file name from primitive */
	ParserStatus = LINE_PARSER_GetNextStr((U8 *)fileName, LINE_PARSER_MAX_STR_LEN);

	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("BPP error reading file name. Can't push"));
		return;
	}

	Report(("BPP APP: file name is %s",fileName));
	
	/* Get mime type from primitive */
	ParserStatus = LINE_PARSER_GetNextStr((U8 *)mimeType, LINE_PARSER_MAX_STR_LEN);

	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("BPP error reading mime type. Can't push"));
		return;
	}

	Report(("BPP APP: mime type is %s",mimeType));
	

	/* Inserting input parameters for printing*/
	
	objToPrint.objectLocation = BTL_OBJECT_LOCATION_FS;

	objToPrint.location.fsLocation.fsPath = (const BtlUtf8 *)fileName;

	objToPrint.objectMimeType = mimeType;
	
	objToPrint.objectName = (const BtlUtf8 *)&myObjectName[0];

	status = BTL_BPPSND_SimplePushPrint(bppsndContext, 	&objToPrint);
	
	Report(("BTL_BPPSND_SimplePushPrint returned %s", pBT_Status(status)));

}

void App_Bpp_Abort(  )
{
	BtStatus			status;
	
	status = BTL_BPPSND_Abort( bppsndContext );
	Report(("BTL_BPPSND_Abort returned %s", pBT_Status(status)));
}


/****************************************************************************
 * This event handler receives events for the ACL link and the completion
 * event for the DS_SelectDevice() request.
 */
static void BtEventHandler(const BtEvent *Event)
{
	switch (Event->eType)
	{
		case BTEVENT_LINK_CONNECT_IND:
			Report(("Incoming ACL connection"));
			bppAppRemDev = Event->p.remDev;
			break;

		case BTEVENT_LINK_CONNECT_CNF:
			if (Event->errCode == BEC_NO_ERROR)
			{
				Report(("Outgoing ACL connection"));
				bppAppRemDev = Event->p.remDev;
			}
			else
			{
			    Report(("ACL connection failed"));
			}
			break;

		case BTEVENT_LINK_DISCONNECT:
			Report(("ACL disconnected"));
			bppAppRemDev = 0;
			break;
			
		default:
			Report(("Unknown BtEvent %d", Event->eType));
			bppAppRemDev = 0;
			break;
			
	}
	
}

/****************************************************************************
 ** callback function called from BTL to report events
 **/
static void HandleBtlBppsndEvent(const BtlBppsndEvent *event)
{
	BppCallbackParms *parms = (BppCallbackParms *) event->bppsndCallbackParms;
	switch (parms->event) 
	{
	    case BPPCLIENT_COMPLETE:
			switch (parms->data->bppOp) 
			{
				case BPPOP_CONNECT:
					if (parms->status == OB_STATUS_SUCCESS)
					{
						UI_DISPLAY(("Sender Connect Complete, channel = %x", parms->channel));
					}
					else
					{
						UI_DISPLAY(("ERROR: event BPPOP_CONNECT failed, status %s", pBT_Status(parms->status)));
					}
					break;
				case BPPOP_DISCONNECT:
					if (parms->status == OB_STATUS_SUCCESS)
					{
						UI_DISPLAY(("Sender Disconnect Complete, channel = %x", parms->channel));
						bppService = BPPSVC_DPS;
					}
					else
					{
						UI_DISPLAY(("ERROR: event BPPOP_DISCONNECT failed, status %s", pBT_Status(parms->status)));
					}
					break;

				case BPPOP_SIMPLEPUSH:
					AppSendFileToPrintCompleted(parms, "SimplePush");
					UI_DISPLAY(("Simple push print operation completed."));
					break;
					
				case BPPOP_SENDDOCUMENT:
					AppSendFileToPrintCompleted(parms, "SendDocument");
					UI_DISPLAY(("Send Document operation completed."));
					break;

				case BPPOP_GETPRTATTR:
					if (parms->status == OB_STATUS_SUCCESS)
						AppGetPrtAttrCompleted(parms);
					
					else
						UI_DISPLAY(("ERROR: BPPOP_GETPRTATTR failed %s", 
							parms->status == OB_STATUS_NO_RESOURCES ? "(not enough memory)" : ""));	
					break;

				case BPPOP_CREATEJOB:
					AppCreateJobCompleted(parms);
					break;

				case BPPOP_CANCELJOB:
					AppCancelJobCompleted(parms);
					break;
				
				case BPPOP_GETJOBATTR:
					AppGetJobAttrCompleted(parms);
					break;

				case BPPOP_GETEVENT :
					AppGetEventReceived(parms);
					break;
					
				case BPPOP_SOAP:
					UI_DISPLAY(("%s job operation completed...", pBppOp(parms->data->bppOp)));
					break;
						
			}	/* switch bppOp */
			break;
			
		case BPPCLIENT_PROGRESS:
			{
				BtlObjProgressInd *progressInd;
				BppData data;

				progressInd = parms->data->progressInfo;
				data.offset = progressInd->currPos;
				data.totLen = progressInd->maxPos;
				App_UpdateProgress(&data);
			}
			break;
			
		case BPPCLIENT_FAILED:
			UI_DISPLAY(("FAIL, reason: %s", GetObexRespCodeStr((U8) parms->status)));
			break;
			
		case BPPCLIENT_ABORT:
			UI_DISPLAY(("ABORT, reason: %s", GetObexAbortReasonStr((U8) parms->status)));
			UI_DISPLAY(("ObexRespCode: %s", GetObexRespCodeStr((U8) parms->status)));
			break;

#if OBEX_AUTHENTICATION == XA_ENABLED
		case BPPCLIENT_AUTH_CHALLENGE_RCVD:
			if ( '\0'  != bppAppPassword[0] )
			 // app is clever enough to think that it has a valid password
			 // at hand for this printer, so no need to open a dialog box
			 // to ask user for userId and password
			{
				AppObexAuthenticationResponse();
			}
			else
			{
				UI_DISPLAY(("Challenge received - please enter authentication info"));
			}
			//AppGetUseridAndPassword();
			break;
#endif

    		case BPPSERVER_REQUEST:
        		if ( BPPOP_GETREFOBJ == parms->data->bppOp )
			{
			            /* Send response */
				     AppSendReferenceObjectResponse(event->bppsndContext , parms);
        		}
            break;

		default:
			UI_DISPLAY(("ERROR: Unhandled event type %d", parms->event));
			break;
	}	/* switch (parms->event) */
} /* HandleBtlBppsndEvent */

void AppSendReferenceObjectResponse(BtlBppsndContext 	*bppsndContext, BppCallbackParms *parms)
{
	BtStatus 	status;
	BtlObject	responseObj = { 0 };
	char 	refFileFullName[] = IMAGE_FILE_NAME;
	char 	refFilMimeType[] = "image/jpeg";
	
	responseObj.objectLocation = BTL_OBJECT_LOCATION_FS;
	responseObj.location.fsLocation.fsPath = (const BtlUtf8 *)refFileFullName;
	responseObj.objectMimeType = refFilMimeType;
	if ( referencedObjFromMem )
	{
		AppConvertToMemObject(&responseObj);
	}
	status = BTL_BPPSND_GetReferenceObjectResponse(bppsndContext, &responseObj);
	Report(("BTL_BPPSND_GetReferenceObjectResponse() returned %s", pBT_Status(status)));
}

static void App_UpdateProgress(BppData *data)
{
	U32     offset, totLen;

	if (data->totLen != 0)
	{
		offset = data->offset;
		totLen = data->totLen;
	}
	else
	{
		offset = totLen = 0;
	}
	UI_DISPLAY(("Progress %2d%%", offset * 100 / totLen));
}


static void  AppObexAuthenticationResponse( void )
{
	BtStatus status;

	status = BTL_BPPSND_ObexAuthenticationResponse(bppsndContext, (const BtlUtf8 *)bppAppUserId, (const BtlUtf8 *)bppAppPassword);
	Report(("BTL_BPPSND_ObexAuthenticationResponse(%s,%s) returned %s", 
			bppAppUserId, bppAppPassword, pBT_Status(status)));
 }

void AppCreateJobCompleted( BppCallbackParms *parms )
{
	Report(("AppCreateJobCompleted Not Implemented Yet !!!"));
}

void AppCancelJobCompleted( BppCallbackParms *parms )
{
	Report(("AppCancelJobCompleted Not Implemented Yet !!!"));
}

void AppGetJobAttrCompleted( BppCallbackParms *parms )
{
	Report(("AppGetJobAttrCompleted Not Implemented Yet !!!"));
}

void AppGetEventReceived( BppCallbackParms *parms )
{
	Report(("AppGetEventReceived Not Implemented Yet !!!"));
}

void AppSendFileToPrintCompleted( BppCallbackParms *parms , char *name)
{
	Report(("AppSendFileToPrintCompleted Not Implemented Yet !!!"));
}

void AppGetPrtAttrCompleted( BppCallbackParms *parms )
{
	Report(("AppGetPrtAttrCompleted Not Implemented Yet !!!"));
}



// funciton for testing printting from memory objects
// convert file object to memory object by reading the file and modifying proper fileds
// uses a static buffer, so only one such object can be supported at a time
// maximum file size is 512 KB
#define MAX_BUF (512 * 1024)
static void AppConvertToMemObject(BtlObject *objToPrint)
{
#if 0
	static char buf[MAX_BUF];
	char *fileName = (char *) objToPrint->location.fsLocation.fsPath;
	FILE *fd;
	int size;

	if ( fd = fopen(fileName, "rb") )
	{
	 	if ( (size = fread(buf, 1, MAX_BUF, fd )) < MAX_BUF )
	 	{
			objToPrint->objectLocation = BTL_OBJECT_LOCATION_MEM;
			objToPrint->location.fsLocation.fsPath = NULL;
			objToPrint->location.memLocation.memAddr = &buf[0];
			objToPrint->location.memLocation.size = size;
			App_Report("AppConvertToMemObject(%s), size=%d", 
				fileName, objToPrint->location.memLocation.size);
	 	}
		fclose ( fd );
	}
#endif
}



/*---------------------------------------------------------------------------
 *            General Purpose Debug functions
 *---------------------------------------------------------------------------
 */
const char *pBT_Status(I8 Status)
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

 const char *GetObexRespCodeStr( ObexRespCode ObexRC)
{
	switch (ObexRC)
	{
		case  OBRC_CONTINUE             : return ("Continue");
		case  OBRC_SUCCESS              : return ("OK, Success");
		case  OBRC_CREATED              : return ("Created");
		case  OBRC_ACCEPTED             : return ("Accepted");
		case  OBRC_NON_AUTHOR_INFO      : return ("Non-Authoritative Information");
		case  OBRC_NO_CONTENT           : return ("No Content");
		case  OBRC_RESET_CONTENT        : return ("Reset Content");
		case  OBRC_PARTIAL_CONTENT      : return ("Partial Content");
		case  OBRC_MULTIPLE_CHOICES     : return ("Multiple Choices");
		case  OBRC_MOVED_PERMANENT      : return ("Moved Permanently");
		case  OBRC_MOVED_TEMPORARY      : return ("Moved Temporarily");
		case  OBRC_SEE_OTHER            : return ("See Other");
		case  OBRC_NOT_MODIFIED         : return ("Not Modified");
		case  OBRC_USE_PROXY            : return ("Use Proxy");
		case  OBRC_BAD_REQUEST          : return ("Bad Request");
		case  OBRC_UNAUTHORIZED         : return ("Unauthorized");
		case  OBRC_PAYMENT_REQUIRED     : return ("Payment Required");
		case  OBRC_FORBIDDEN            : return ("Forbidden");
		case  OBRC_NOT_FOUND            : return ("Not Found");
		case  OBRC_METHOD_NOT_ALLOWED   : return ("Method Not Allowed");
		case  OBRC_NOT_ACCEPTABLE       : return ("Not Acceptable");
		case  OBRC_PROXY_AUTHEN_REQ     : return ("Proxy Authentication Required");
		case  OBRC_REQUEST_TIME_OUT     : return ("Request Timed Out");
		case  OBRC_CONFLICT             : return ("Conflict");
		case  OBRC_GONE                 : return ("Gone");
		case  OBRC_LENGTH_REQUIRED      : return ("Length Required");
		case  OBRC_PRECONDITION_FAILED  : return ("Precondition Failed");
		case  OBRC_REQ_ENTITY_TOO_LARGE : return ("Requested entity is too large");
		case  OBRC_REQ_URL_TOO_LARGE    : return ("Requested URL is too large");
		case  OBRC_UNSUPPORT_MEDIA_TYPE : return ("Unsupported Media Type");
		case  OBRC_INTERNAL_SERVER_ERR  : return ("Internal Server Error");
		case  OBRC_NOT_IMPLEMENTED      : return ("Not Implemented");
		case  OBRC_BAD_GATEWAY          : return ("Bad Gateway");
		case  OBRC_SERVICE_UNAVAILABLE  : return ("Service Unavailable");
		case  OBRC_GATEWAY_TIMEOUT      : return ("Gateway Timeout");
		case  OBRC_HTTP_VER_NO_SUPPORT  : return ("HTTP version not supported");
		case  OBRC_DATABASE_FULL        : return ("Database Full");
		case  OBRC_DATABASE_LOCKED      : return ("Database Locked"); 
		default:
			{
				static char unknownStr[25];
				sprintf(unknownStr, "Unknown (%d)", ObexRC);
				return (unknownStr);
			}
	}
}

 /****************************************************************************
 ** Returns ASCII Operation Types
 **/
const char *pBppOp(BppOp bppOp)
{
    switch (bppOp) {
    case BPPOP_CREATEJOB:
        return "CreateJob SOAP";

    case BPPOP_GETJOBATTR:
        return "GetJobAttributes SOAP";

    case BPPOP_GETPRTATTR:
        return "GetPrinterAttributes SOAP";

    case BPPOP_CANCELJOB:
        return "CancelJob SOAP";

    case BPPOP_GETEVENT:
        return "GetEvent SOAP";

    case BPPOP_GETMARGINS:
        return "GetMargins SOAP";

    case BPPOP_SENDDOCUMENT:
        return "SendDocument";

    case BPPOP_SIMPLEPUSH:
        return "SimplePush";

    default:
        return "UNKNOWN";
    }
}

const char *GetObexAbortReasonStr( ObexAbortReason abortReason)
{
	switch (abortReason)
	{
		case  OBRC_LINK_DISCONNECT			: return ("LINK_DISCONNECT");
		case  OBRC_CLIENT_RW_ERROR			: return ("CLIENT_RW_ERRO");
		case  OBRC_USER_ABORT				: return ("USER_ABORT");
#if OBEX_SESSION_SUPPORT == XA_ENABLED
		case  OBRC_SESSION_USER_ABORT         : return ("SESSION_USER_ABORT");
		case  OBRC_SESSION_NO_SEQ_NUM      	: return ("SESSION_NO_SEQ_NUM");
		case  OBRC_SESSION_INVALID_PARMS	: return ("SESSION_INVALID_PARMS");
		case  OBRC_SESSION_ERROR			: return ("SESSION_ERROR");
#endif
		default:
			{
				static char unknownStr[25];
				sprintf(unknownStr, "Unknown (0x%x)", abortReason);
				return (unknownStr);
			}
	}
}

#else /* BTL_CONFIG_BPPSND == BTL_CONFIG_ENABLED */


void BPPA_Init( void )
{
	Report(("BPPA_Init -BTL_CONFIG_BPPSND is disabled "));
}
void BPPA_Deinit( void )
{
	Report(("BPPA_Deinit  - BTL_CONFIG_BPPSND is disabled"));
}

void BPPA_ProcessUserAction(U8 *msg)
{
	msg=msg;
	Report(("BPPSND_APP is disabled via BTL_CONFIG."));

}



#endif /* BTL_CONFIG_BPPSND == BTL_CONFIG_ENABLED */

