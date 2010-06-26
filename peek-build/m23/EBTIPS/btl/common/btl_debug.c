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
*   FILE NAME:      btl_debug.h
*
*   DESCRIPTION:    This file defines various debug utilities
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bttypes.h"
#include "utils.h"
#include "obex.h"
#include "debug.h"
#include "bpp.h"
#include "btl_debug.h"

/********************************************************************************
 *
 * Function implementation
 *
 *******************************************************************************/

#ifdef BTL_DEBUG_CANCEL_RADIO_ON

U32 btlDebugCheckPointIndex = BTL_DEBUG_UNDEFINED_CP_USER_IDX;
U32 btlDebugCheckPointOfInterest;
U32 btlDebugCheckPointInitScriptCounter;

void BTL_DEBUG_CroCheckPoint(BtldebugCancelRadioOnIndx index)
{
	if (BTL_DEBUG_THRESHOLD_IDX == index) 
	{
		if (BTL_DEBUG_CRO_INIT_SCRIPT_THRESHOLD == btlDebugCheckPointInitScriptCounter)
		{
			btlDebugCheckPointIndex = index;
			BTHAL_OS_Sleep(BTL_DEBUG_CRO_CHECK_POINT_SLEEP_TIME);
		}
		else
		{
			return;
		}
	}

	btlDebugCheckPointIndex = index;
	BTHAL_OS_Sleep(BTL_DEBUG_CRO_CHECK_POINT_SLEEP_TIME);
}


void BTL_DEBUG_CroClearInitScriptCounter(void)
{
	btlDebugCheckPointInitScriptCounter = 0;
}

void BTL_DEBUG_CroIncrementInitScriptCounter(void)
{
	btlDebugCheckPointInitScriptCounter++;
}

#endif



char* BTL_DEBUG_pObStatus(I8 obStatus)
{
	ObStatus	obexStatus = (ObStatus)obStatus;
	
	switch (obexStatus) 
	{
		case OB_STATUS_SUCCESS: 		return "Success";
		case OB_STATUS_FAILED: 			return "Failed";
		case OB_STATUS_PENDING:		return "Pending";
		case OB_STATUS_DISCONNECT:		return "Disconnect";
		case OB_STATUS_NO_CONNECT:	return "No Connection";
		case OB_STATUS_INVALID_PARM:	return "Invalid Parameter";
		case OB_STATUS_INVALID_HANDLE:	return "Invalid Handle";
		case OB_STATUS_PACKET_TOO_SMALL:	return "Packet Too Small";
		case OB_STATUS_BUSY:			return "Status Busy";
		case OB_STATUS_NO_RESOURCES:	return "No Resources";
		default:							return "UNKNOWN";
	};
}

char* BTL_DEBUG_pBppEvent(U8 event)
{
	BppEvent eventAsBppEvent = (BppEvent)event;
	
	switch (eventAsBppEvent) 
	{
		case BPPCLIENT_DATA_REQ: 	return "BPPCLIENT_DATA_REQ";
		case BPPCLIENT_DATA_IND: 	return "BPPCLIENT_DATA_IND";
		case BPPCLIENT_RESPONSE:	return "BPPCLIENT_RESPONSE";
		case BPPCLIENT_COMPLETE:	return "BPPCLIENT_COMPLETE";
		case BPPCLIENT_FAILED:		return "BPPCLIENT_FAILED";
		case BPPCLIENT_ABORT:		return "BPPCLIENT_ABORT";
		case BPPCLIENT_AUTH_CHALLENGE_RCVD:		
									return "BPPCLIENT_AUTH_CHALLENGE_RCVD";
		case BPPSERVER_DATA_REQ:	return "BPPSERVER_DATA_REQ";
		case BPPSERVER_DATA_IND:	return "BPPSERVER_DATA_IND";
		case BPPSERVER_REQUEST:	return "BPPSERVER_REQUEST";
		case BPPSERVER_COMPLETE:	return "BPPSERVER_COMPLETE";
		case BPPSERVER_FAILED:		return "BPPSERVER_FAILED";
		case BPPSERVER_ABORT:		return "BPPSERVER_ABORT";
		case BPPSERVER_CONTINUE:	return "BPPSERVER_CONTINUE";
		default:						return "UNKNOWN";
	};
}

char* BTL_DEBUG_pBppOp(U16 bppOp)
{
	BppOp opAsBppOp = (BppOp)bppOp;
	
	switch (opAsBppOp) 
	{
		case BPPOP_NOP: 			return "BPPOP_NOP";
		case BPPOP_CONNECT: 		return "BPPOP_CONNECT";
		case BPPOP_SIMPLEPUSH: 		return "BPPOP_SIMPLEPUSH";
		case BPPOP_CREATEJOB: 		return "BPPOP_CREATEJOB";
		case BPPOP_SENDDOCUMENT: 	return "BPPOP_SENDDOCUMENT";
		case BPPOP_GETJOBATTR: 		return "BPPOP_GETJOBATTR";
		case BPPOP_GETPRTATTR: 		return "BPPOP_GETPRTATTR";
		case BPPOP_CANCELJOB: 		return "BPPOP_CANCELJOB";
		case BPPOP_GETEVENT: 		return "BPPOP_GETEVENT";
		case BPPOP_SOAP: 			return "BPPOP_SOAP";
		case BPPOP_GETREFOBJ: 		return "BPPOP_GETREFOBJ";
		case BPPOP_SENDREFERENCE: 	return "BPPOP_SENDREFERENCE";
		case BPPOP_SIMPLEREFERENCE: return "BPPOP_SIMPLEREFERENCE";
		case BPPOP_CREATEPRECISE: 	return "BPPOP_CREATEPRECISE";
		case BPPOP_GETMARGINS: 		return "BPPOP_GETMARGINS";
		case BPPOP_GETRUI: 			return "BPPOP_GETRUI";
		default:						return "UNKNOWN";
	};
}

char* BTL_DEBUG_pBppChannel(U8 channel)
{
	BppChannel channelAsBppChannel = (BppChannel)channel;
	
	switch (channelAsBppChannel) 
	{
		case BPPCH_SND_JOB: 	return "BPPCH_SND_JOB";
		case BPPCH_SND_STATUS: return "BPPCH_SND_STATUS";
		case BPPCH_PRN_JOB: 	return "BPPCH_PRN_JOB";
		case BPPCH_PRN_STATUS: 	return "BPPCH_PRN_STATUS";
		case BPPCH_SND_OBJECT: 	return "BPPCH_SND_OBJECT";
		case BPPCH_PRN_OBJECT: 	return "BPPCH_PRN_OBJECT";
		case BPPCH_SND_RUI: 	return "BPPCH_SND_RUI";
		case BPPCH_PRN_RUI: 	return "BPPCH_PRN_RUI";
		default:					return "UNKNOWN";
	}
}


char* BTL_DEBUG_pBtStatus(BtStatus status)
{
	switch (status) 
	{
		case BT_STATUS_SUCCESS:				return "SUCCESS";        
		case BT_STATUS_FAILED:					return "FAILED";      
		case BT_STATUS_PENDING:					return "Pending";         
		case BT_STATUS_BUSY:					return "BUSY";           
		case BT_STATUS_NO_RESOURCES:			return "NO_RESOURCES";
		case BT_STATUS_NOT_FOUND:				return "NOT_FOUND";   
		case BT_STATUS_DEVICE_NOT_FOUND:		return "DEVICE_NOT_FOUND";
		case BT_STATUS_CONNECTION_FAILED:		return "CONNECTION_FAILED";
		case BT_STATUS_TIMEOUT:					return "TIMEOUT";        
		case BT_STATUS_NO_CONNECTION:			return "NO_CONNECTION";    
		case BT_STATUS_INVALID_PARM:			return "INVALID_PARM";   
		case BT_STATUS_IN_PROGRESS:			return "IN_PROGRESS";     
		case BT_STATUS_RESTRICTED:				return "RESTRICTED";     
		case BT_STATUS_INVALID_TYPE:			return "INVALID_TYPE";    
		case BT_STATUS_HCI_INIT_ERR:			return "HCI_INIT_ERR";   
		case BT_STATUS_NOT_SUPPORTED:			return "NOT_SUPPORTED";  
		case BT_STATUS_IN_USE:					return "IN_USE";      
		case BT_STATUS_SDP_CONT_STATE: 		return "SDP_CONT_STATE";   
		case BT_STATUS_CANCELLED:				return "CANCELLED";            

#if TI_CHANGES == XA_ENABLED 
		/* An internal error occurred - Stack must be reset */
		case BT_STATUS_INTERNAL_ERROR:			return "INTERNAL_ERROR";		
		case BT_STATUS_IMPROPER_STATE:			return "IMPROPER_STATE";			
		case BT_STATUS_FFS_ERROR:				return "FFS_ERROR";			
		case BT_STATUS_ALREADY_INITIALIZED:		return "ALREADY_INITIALIZED";	
		case BT_STATUS_RADIO_OFF:		return "RADIO_OFF";				
		case BT_STATUS_INIT_FAILED:		return "INIT_FAILED";			
#endif /* TI_CHANGES == XA_ENABLED */

		default:							return "UNKNOWN";
	};
}



        

