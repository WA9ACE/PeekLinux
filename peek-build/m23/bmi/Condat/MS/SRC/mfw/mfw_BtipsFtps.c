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
 $Module:   Bluetooth BMG MFW
 $File:       Mfw_Btips.c
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS FTP Server APPlication functionality.

********************************************************************************
 $History: Mfw_BtipsFtps.c

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

#include "osapi.h"

/* GPF Includes */
#include "typedefs.h"
#include "vsi.h"
//#include "../p_btt.h"

#if BT_STACK == XA_ENABLED
#include <me.h>
#include <bttypes.h>
#include <sec.h>
#endif

#include "btl_common.h"
#include "btl_bmg.h"
#include "debug.h"

#include "goep.h"
#include "ftp.h"

/* BTL inclides */
#include "btl_ftps.h"

#include "bthal_fs.h"

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_icn.h"	/* included for mfw_mnu.h */
#include "mfw_mnu.h"
#include "mfw_tim.h"
#include "mfw_kbd.h"
#include "mfw_sat.h" /* included for MmiDummy.h, included for MmiMenu.h */

#include "Mfw_Btips.h"
#include "Mfw_BtipsBmg.h"

/* Internal Global variables for FTP Server*/
static BtlFtpsContext *ftpsContext = 0;
static BtSecurityLevel securityLevel = BSL_NO_SECURITY;
static BD_ADDR bdAddr;


/****************************************************************************
 *
 * Internal Function prototypes
 *
 ***************************************************************************/
static void btips_ftpsCallBack(const BtlFtpsEvent *event);
#ifdef WIN32
const char *pBT_Status(I8 Status);
#else
static const char *pBT_Status(I8 Status);
#endif
static const char *GoepOpName(GoepOperation Op);


/****************************************************************************
 *
 * RAM data
 *
 ****************************************************************************/

#if OBEX_AUTHENTICATION == XA_ENABLED
static char	realm[20];		/* OBEX realm */
static char	password[20];	/* OBEX Authentication password */
static char	userId[20];		/* OBEX Authentication userId */
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

static char rootPath[80];
static BtlFtpsObjectRequestMask objectRequestMask;
static BOOL	accept;
char BTStr[20];
/*---------------------------------------------------------------------------
 *            mfw_btips_ftpsInit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Create and Enable the FTP Server .
 *
 * Return:    void
 *
 */
 
void mfw_btips_ftpsInit (void)
{
	BtStatus status;
		
	OS_StrCpy(realm, "MyRealm");
	OS_StrCpy(password, "MyPassword");
	OS_StrCpy(userId, "MyUserId");

	OS_StrCpy(rootPath, "/");
	
	status =  BTL_FTPS_Create ((BtlAppHandle *) 0, btips_ftpsCallBack,
						&securityLevel, &ftpsContext);
	
	Report(("BTL_FTPS_Create() returned %s.",pBT_Status(status)));

       OS_StrCpy (rootPath, BT_DEMO_FOLDER_PATH);

	status = BTL_FTPS_Enable(ftpsContext, 0, rootPath);
	
	Report(("BTL_FTPS_Enable() returned %s.",pBT_Status(status)));

	status = BTL_FTPS_ConfigObjectRequest (	ftpsContext,
											BTL_FTPS_OBJECT_REQUEST_PUSH	| 
											BTL_FTPS_OBJECT_REQUEST_PULL	|
											BTL_FTPS_OBJECT_REQUEST_DELETE	);

	Report(("BTL_FTPS_ConfigObjectRequest() returned %s.",pBT_Status(status)));
	
}

/*---------------------------------------------------------------------------
 *            mfw_btips_ftpsAbort
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Requests the current operation be aborted.
 *
 * Return:    void
 *
 */
void mfw_btips_ftpsAbort(void)
{
	BtStatus status;
	
	status = BTL_FTPS_Abort(ftpsContext, OBRC_CONFLICT);
	
	Report(("BTL_FTPS_Abort() returned %s.",pBT_Status(status)));
	
}

/*---------------------------------------------------------------------------
 *            mfw_btips_ftpsAcceptObjectRequest
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function must be called after receiving object request from the client,
 *				in order to accept or reject the request.
 *
 * Return:    BOOL
 *
 */
void mfw_btips_ftpsAcceptObjectRequest (BOOL flag)
{
	BtStatus	status;
	
	status = BTL_FTPS_AcceptObjectRequest (ftpsContext, flag);
	
	Report(("BTL_FTPS_AcceptObjectRequest() returned %s.", pBT_Status(status)));
}

/*---------------------------------------------------------------------------
 *            mfw_btips_ftpsSetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets security level for the given FTPS context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
BOOL mfw_btips_ftpsSetSecurityLevel (void)
{
	BtStatus status;
	BtSecurityLevel newSecurityLevel = securityLevel;
	
	status = BTL_FTPS_SetSecurityLevel (ftpsContext, &newSecurityLevel);
	
	Report(("BTL_FTPS_SetSecurityLevel() returned %s.", pBT_Status(status)));

	return TRUE;
}


#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 *            mfw_btips_ftpsObexAuthenticationResponse
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Responde to OBEX auth.
 *
 * Return:    void
 *
 */
BOOL mfw_btips_ftpsObexAuthenticationResponse (void)
{
	BtStatus status;

	status = BTL_FTPS_ObexAuthenticationResponse (ftpsContext, userId, password);
	
	Report(("BTL_FTPS_ObexAuthenticationResponse() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

BOOL mfw_btips_ftpsGetConnectedDevice (BD_ADDR *bdAddr, U8 *name)
{
	BtStatus status;
	
	status = BTL_FTPS_GetConnectedDevice (ftpsContext, bdAddr);
	
	if (status == BT_STATUS_SUCCESS)
	{
		mfw_btips_bmgGetDeviceName (bdAddr, name);
		return TRUE;
	}
	return FALSE;
}

void mfw_btips_ftpsDisconnect (void)
{
	BTL_FTPS_Disconnect (ftpsContext);
}


/*---------------------------------------------------------------------------
 *            btips_ftpsCallBack
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes FTP Events received from BTL, only events requested in BTL_FTPS_ConfigObjectRequest
 *			will be received
 * Return:    void
 *
 */
static void btips_ftpsCallBack(const BtlFtpsEvent *event)
{
	char addr[BDADDR_NTOA_SIZE];
	T_BTIPS_MMI_IND	btips_mmi_ind;
	BtlBmgDeviceRecord record;
	BtStatus status;

	TRACE_FUNCTION_P1("btips_ftpsCallback %d", event->ftpsEvent->event);

	btips_mmi_ind.mfwBtipsEventType = MFW_BTIPS_FTPS_EVENT;


    /* First process only events that are not "object" related. */
	switch ( event->ftpsEvent->event)
	{
		case FTPS_EVENT_TP_CONNECTED:
			BTL_FTPS_GetConnectedDevice(ftpsContext, &bdAddr);
			TRACE_FUNCTION_P1("FTP Server: Transport Connection is up to %s.", bdaddr_ntoa(&bdAddr, addr));
		break;

		case FTPS_EVENT_TP_DISCONNECTED:
			TRACE_FUNCTION(("FTP Server: Transport Connection has been disconnected."));
		break;

#if OBEX_AUTHENTICATION == XA_ENABLED

		case FTPS_EVENT_AUTH_CHALLENGE:
			TRACE_FUNCTION_P1("FTP Server: Received Authenticate Challenge for %s operation.", 
			GoepOpName(event->ftpsEvent->oper));
#if OBEX_MAX_REALM_LEN > 0
			Report(("FTP Server: Realm: %s, charset %d , Options %d.", 
			event->ftpsEvent->info.challenge.realm+1, event->ftpsEvent->info.challenge.realm[0], event->ftpsEvent->info.challenge.options));
#endif /* OBEX_MAX_REALM_LEN > 0 */

			mfw_btips_ftpsObexAuthenticationResponse ( );
		break;        

#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
    
		case FTPS_EVENT_START:
			TRACE_FUNCTION_P1("FTP Server: Receiving a %s operation.", GoepOpName(event->ftpsEvent->oper));
		break;

		case FTPS_EVENT_ABORTED:
			TRACE_FUNCTION_P1("FTPServer: %s operation aborted.", GoepOpName(event->ftpsEvent->oper));
			btips_mmi_ind.data.ftpsBtEvent.event = MFW_BTIPS_FTPS_ABORTED_EVENT;
			btipsNotifyMFW(&btips_mmi_ind);
		break;

		case FTPS_EVENT_COMPLETE:
			TRACE_FUNCTION_P1("FTPServer: %s operation complete.", GoepOpName(event->ftpsEvent->oper));
			switch(event->ftpsEvent->oper)
				{
				case GOEP_OPER_PUSH:
					btips_mmi_ind.data.ftpsBtEvent.event = MFW_BTIPS_FTPS_PUT_COMP_EVENT;
				break;
				case GOEP_OPER_DELETE:
					btips_mmi_ind.data.ftpsBtEvent.event = MFW_BTIPS_FTPS_DELETE_COMP_EVENT;
				break;
				case GOEP_OPER_CONNECT:
					btips_mmi_ind.data.ftpsBtEvent.event = MFW_BTIPS_FTPS_CONNECT_EVENT;
				break;
				case GOEP_OPER_DISCONNECT:
					btips_mmi_ind.data.ftpsBtEvent.event = MFW_BTIPS_FTPS_DISCONNECT_EVENT;
				break;
				}
			btipsNotifyMFW(&btips_mmi_ind);
		break;

		case FTPS_EVENT_OBJECT_PROGRESS:
			Report(("FTPServer: %d percent of progress was complete.",
							(event->ftpsEvent->info.progress.currPos/event->ftpsEvent->info.progress.maxPos)*100));
		break;
			
		case FTPS_EVENT_OBJECT_REQUEST:
			Report(("FTP Object request  : operation \"%s\".", GoepOpName(event->ftpsEvent->oper)));
			Report(("FTP Object request: name \"%s\".", event->ftpsEvent->info.request.name));
			Report(("FTP Object request: length \"%d\".", event->ftpsEvent->info.request.objectLen));
			TRACE_FUNCTION("FTP Please accept or reject!");
			
#if 0
			status = BTL_BMG_DDB_FindRecord (bdAddr, &record);
			if (status == BT_STATUS_SUCCESS)
			{
#endif
				if (event->ftpsEvent->oper == GOEP_OPER_PUSH)
				{
					btips_mmi_ind.data.ftpsBtEvent.event = MFW_BTIPS_FTPS_PUT_EVENT;
					strcpy (btips_mmi_ind.data.ftpsBtEvent.p.ftpPutRequest.objectName, event->ftpsEvent->info.request.name);
//					strcpy (btips_mmi_ind.data.ftpsBtEvent.p.ftpPutRequest.deviceName, record.name);
//					btips_mmi_ind.data.ftpsBtEvent.p.ftpPutRequest.bdAddr = bdAddr;
					mfw_btips_ftpsGetConnectedDevice (&(btips_mmi_ind.data.ftpsBtEvent.p.ftpPutRequest.bdAddr), btips_mmi_ind.data.ftpsBtEvent.p.ftpPutRequest.deviceName);
				}
				else if (event->ftpsEvent->oper == GOEP_OPER_PULL)
				{
					btips_mmi_ind.data.ftpsBtEvent.event = MFW_BTIPS_FTPS_GET_EVENT;		
					strcpy (btips_mmi_ind.data.ftpsBtEvent.p.ftpGetRequest.objectName, event->ftpsEvent->info.request.name);
//					strcpy (btips_mmi_ind.data.ftpsBtEvent.p.ftpGetRequest.deviceName, record.name);
//					btips_mmi_ind.data.ftpsBtEvent.p.ftpGetRequest.bdAddr = bdAddr;
					mfw_btips_ftpsGetConnectedDevice (&(btips_mmi_ind.data.ftpsBtEvent.p.ftpGetRequest.bdAddr), btips_mmi_ind.data.ftpsBtEvent.p.ftpGetRequest.deviceName);
				}
				else if (event->ftpsEvent->oper == GOEP_OPER_DELETE) 
				{
					btips_mmi_ind.data.ftpsBtEvent.event = MFW_BTIPS_FTPS_DELETE_EVENT;
					strcpy (btips_mmi_ind.data.ftpsBtEvent.p.ftpDeleteRequest.objectName, event->ftpsEvent->info.request.name);
#if 0
					strcpy (btips_mmi_ind.data.ftpsBtEvent.p.ftpDeleteRequest.deviceName, record.name);
					btips_mmi_ind.data.ftpsBtEvent.p.ftpDeleteRequest.bdAddr = bdAddr;
#endif
					mfw_btips_ftpsGetConnectedDevice (&(btips_mmi_ind.data.ftpsBtEvent.p.ftpDeleteRequest.bdAddr), btips_mmi_ind.data.ftpsBtEvent.p.ftpDeleteRequest.deviceName);
				}
				else
				{
					break;
				}

				btipsNotifyMFW(&btips_mmi_ind);
#if 0
			}
#endif
	        break;
	}
}

void mfw_btips_ftpsCallback(T_BTIPS_MMI_IND * mmiInd)
{
	MfwBtFtpsEvent ftpsBtEvent = mmiInd->data.ftpsBtEvent;

	TRACE_FUNCTION_P1("mfw_btips_ftpsCallback Event = %x", ftpsBtEvent.event);
	switch(ftpsBtEvent.event)
	{
		case MFW_BTIPS_FTPS_PUT_EVENT:
			mfw_btips_signal(E_BTIPS_FTPS_PUT_EVENT, mmiInd);
		break;
			
		case MFW_BTIPS_FTPS_GET_EVENT:
			mfw_btips_signal(E_BTIPS_FTPS_GET_EVENT, mmiInd);
		break;
		
		case MFW_BTIPS_FTPS_DELETE_EVENT:			
			mfw_btips_signal(E_BTIPS_FTPS_DELETE_EVENT, mmiInd);
		break;

		case MFW_BTIPS_FTPS_COMPLETE_EVENT:
			mfw_btips_signal(E_BTIPS_FTPS_COMPLETE_EVENT,mmiInd);
		break;

		case MFW_BTIPS_FTPS_ABORTED_EVENT:
			mfw_btips_signal(E_BTIPS_FTPS_ABORTED_EVENT,mmiInd);
		break;

		case MFW_BTIPS_FTPS_CONNECT_EVENT:
			mfw_btips_signal(E_BTIPS_FTPS_CONNECT_EVENT,mmiInd);
		break;

		case MFW_BTIPS_FTPS_DISCONNECT_EVENT:
			mfw_btips_signal(E_BTIPS_FTPS_DISCONNECT_EVENT,mmiInd);
		break;
		
		case MFW_BTIPS_FTPS_PUT_COMP_EVENT:
			mfw_btips_signal(E_BTIPS_FTPS_PUT_COMP_EVENT,mmiInd);
		break;	

		case MFW_BTIPS_FTPS_DELETE_COMP_EVENT:
			mfw_btips_signal(E_BTIPS_FTPS_DELETE_COMP_EVENT,mmiInd);
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
   case GOEP_OPER_CONNECT:
       return "Connect";
   case GOEP_OPER_DISCONNECT:
       return "Disconnect";   
   case GOEP_OPER_SETFOLDER:
       return "Set Folder";
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

#endif

