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
*   FILE NAME:      btl_bmg_debug.h
*
*   DESCRIPTION:    This file defines common macros that should be used for message logging, 
*					and exception checking, handling and reporting
*
*					In addition, it contains miscellaneous other related definitions.
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/


#include "btl_bmg_debug.h"
#include "btl_bmg_request.h"
#include "btips_retrieve_srv.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BMG);

const char *BTL_BMG_DEBUG_pBool(BOOL value)
{
	if (TRUE == value)
	{
		return "TRUE";
	}
	else
	{
		return "FALSE";
	}
}

const char *BTL_BMG_DEBUG_pRequestType(U32 type)
{
	BtlBmgRequestType	requestType = (BtlBmgRequestType) type;

	switch (requestType)
	{
	 case BTL_BMG_REQUEST_TYPE_UNSPECIFIED: return "UNSPECIFIED";
	 case BTL_BMG_REQUEST_TYPE_SEARCH_BY_COD: return "SEARCH_BY_COD";
	 case BTL_BMG_REQUEST_TYPE_SEARCH_BY_DEVICES: return "SEARCH_BY_DEVICES";
	 case BTL_BMG_REQUEST_TYPE_INQUIRY: return "INQUIRY";
	 case BTL_BMG_REQUEST_TYPE_NAME_REQUEST: return "NAME_REQUEST";
	 case BTL_BMG_REQUEST_TYPE_SEND_HCI_COMMAND: return "SEND_HCI_COMMAND";
	 case BTL_BMG_REQUEST_TYPE_DISCOVER_SERVICES: return "DISCOVER_SERVICES";
	 case BTL_BMG_REQUEST_TYPE_BOND: return "BOND";
	 case BTL_BMG_REQUEST_TYPE_CREATE_LINK: return "CREATE_LINK";
	 case BTL_BMG_REQUEST_TYPE_SET_SECURITY_MODE: return "SET_SECURITY_MODE";
	 case BTL_BMG_REQUEST_TYPE_ENABLE_SEC_MODE_3: return "ENABLE_SEC_MODE_3";
	 case BTL_BMG_REQUEST_TYPE_DISABLE_SEC_MODE_3: return "DISABLE_SEC_MODE_3";
	 case BTL_BMG_REQUEST_TYPE_AUTHENTICATE_LINK: return "AUTHENTICATE_LINK";
	 case BTL_BMG_REQUEST_TYPE_ENABLE_TEST_MODE: return "ENABLE_TEST_MODE";
	 case BTL_BMG_REQUEST_TYPE_RADIO_ON: return "RADIO_ON";
	 case BTL_BMG_REQUEST_TYPE_SET_LINK_ENCRYPTION: return "SET_LINK_ENCRYPTION";
	 case BTL_BMG_REQUEST_TYPE_SET_CONTROL_INCOMING_CON_ACCEPT: return "SET_CONTROL_INCOMING_CON_ACCEPT";
	 case BTL_BMG_REQUEST_TYPE_SERVICE_SEARCH_ATTR: return "SERVICE_SEARCH_ATTR";
	 case BTL_BMG_REQUEST_TYPE_INTERCEPT_HCI_INIT_EVENTS: return "INTERCEPT_HCI_INIT_EVENTS";
	 case BTL_BMG_REQUEST_TYPE_RADIO_OFF: return "RADIO_OFF";
	 case BTL_BMG_REQUEST_TYPE_SET_ACCESSIBILITY: return "SET_ACCESSIBILITY";
	 case BTL_BMG_REQUEST_TYPE_SERVICE_SEARCH: return "SERVICE_SEARCH";
	 case BTL_BMG_REQUEST_TYPE_SERVICE_ATTR: return "SERVICE_ATTR";

	default:	return "Unknown";
		
	};
}

const char *BTL_BMG_DEBUG_pForwardingMode(U32 mode)
{
	switch (mode)
	{
		case BTL_BMG_EVENT_FORWARDING_ALL_EVENTS:

			return "FORWARD_ALL_EVENTS";



		case BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY:

			return "FORWARD_MY_EVENT_ONLY";



		default:

			return "UNKNOWN FORWARDING MODE";
	};
}

const char *BTL_BMG_DEBUG_pSdpRetrieveStatus(U32 status)
{
	switch ((BTIPS_RS_Status)status)
	{
	case BTIPS_RS_STATUS_OK: return "OK";
	case BTIPS_RS_STATUS_QUERY_FAILURE: return "FAILURE";
	case BTIPS_RS_STATUS_QUERY_EMPTY: return "EMPTY";
	case BTIPS_RS_STATUS_BUFFER_OVERFLOW: return "BUFFER_OVERFLOW";
	case BTIPS_RS_STATUS_CONNECT_FAILURE: return "CONNECT_FAILURE";
	case BTIPS_RS_STATUS_INTERNAL: return "INTERNAL";
	case BTIPS_RS_STATUS_INVALID_INPUT: return "INVALID_INPUT";
	case BTIPS_RS_STATUS_QUERY_IN_PROGRESS: return "IN_PROGRESS";
	case BTIPS_RS_STATUS_DEVICE_NOT_REGISTERED: return "DEVICE_NOT_REGISTERED";
	case BTIPS_RS_STATUS_SDP_CONT_STATE: return "SDP_CONT_STATE";
	case BTIPS_RS_STATUS_PENDING: return "PENDING";
	case BTIPS_RS_STATUS_OPERATION_END: return "OPERATION_END";
	default: return "INVALID";
	};
}

