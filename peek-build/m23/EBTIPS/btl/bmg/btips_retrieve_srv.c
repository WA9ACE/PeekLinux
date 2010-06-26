/*******************************************************************************\
*
*   FILE NAME:      btips_retrieve_srv.c
*
*   DESCRIPTION:    This file implements the function prototypes for general
*				  retrieving of attributes from SDP server.
*
*   AUTHOR:         Yuval Hevrony
*
\*******************************************************************************/

#include "me.h"
#include "btips_retrieve_srv.h"
#include "btips_retrieve_srv_data.h"
#include "medev.h"
#include "stdlib.h"
#include "debug.h"
#include "btl_bmg.h"
#include "btrom.h"
#include "btl_defs.h"
#include "btl_log_modules.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BMG);

/* MACROS */

/* The max request size is calculated using the SDP specs.
 * 1) UUID list data element.
 * 2) result size limit.
 * 3) Attribute IDs list data element
 */
#define MAX_SERVICE_SEARCH_ATTR_REQUEST_SIZE \
	(2+BTIPS_RS_MAX_UUIDS*(UUID_MAX_SIZE + 1)+2+ 3+BTL_CONFIG_BMG_MAX_NUM_OF_ATTRIBUTES_TO_SEARCH*3)

#define MAX_SERVICE_REQUEST_SIZE \
	(2+BTIPS_RS_MAX_UUIDS*(UUID_MAX_SIZE + 1) + 2)


#define MAX_SERVICE_ATTR_REQUEST_SIZE \
	(4+2+2+3+BTL_CONFIG_BMG_MAX_NUM_OF_ATTRIBUTES_TO_SEARCH*3)



/* FUNCTION DECLERATIONS */
static BTIPS_RS_Status BTIPS_RS_ValidateServiceSearchAttrInput(BD_ADDR * bd_addr, BTIPS_RS_SERVICE_SEARCH_ATTR_PARAMS * att_params);
static BTIPS_RS_Status BTIPS_RS_ValidateServiceSearchInput(BD_ADDR * bd_addr, BTIPS_RS_SERVICE_SEARCH_PARAMS * params);
static BTIPS_RS_Status BTIPS_RS_ValidateServiceAttrInput(BD_ADDR * bd_addr, BTIPS_RS_SERVICE_ATTR_PARAMS * params);
static U8* BTIPS_RS_BuildServiceAttributeSearch(BtlBmgUuid *uuids, U8 uuid_count, SdpAttributeId *, U16 attribute_count, U16 *tot_size);
static U8* BTIPS_RS_BuildServiceSearch(BtlBmgUuid *uuids, U8 uuid_count, U16 *tot_size);
static U8* BTIPS_RS_BuildServiceAttribute(U32 recordHandle, SdpAttributeId *, U16 attribute_count, U16 *tot_size);
static void BTIPS_RS_ServiceSearchAttributeEventHandler(const BtEvent* event);
static void BTIPS_RS_ServiceSearchEventHandler(const BtEvent* event);
static void BTIPS_RS_ServiceAttrEventHandler(const BtEvent* event);
static BTIPS_RS_Status BTIPS_RS_EstablishConnection(BD_ADDR *bdAddr);
static void BTIPS_RS_ConnectionEvent(const BtEvent *event);
static BTIPS_RS_Status BTIPS_RS_ExtractAttributes(SdpQueryToken *token);
static BTIPS_RS_Status BTIPS_RS_ExtractAttributesAndParse(SdpQueryToken *token);
static BTIPS_RS_Status BTIPS_RS_ExtractRecordHandles(SdpQueryToken *token);
static BTIPS_RS_Status BTIPS_RS_ExtractRecordsAttributes(SdpQueryToken *token);
static void BTIPS_RS_SortAttributes(SdpAttributeId *attributeIds,  U16 numOfAttributes);
static int BTIPS_RS_CompareAttrId(const void *id1, const void *id2);
static BtStatus BTIPS_RS_EndConnection(void);
static BTIPS_RS_Status BTIPS_RS_ServiceSearchAttrWithLink(BtRemoteDevice *remDev, BTIPS_RS_SERVICE_SEARCH_ATTR_PARAMS *att_params);
static BTIPS_RS_Status BTIPS_RS_ServiceSearchWithLink(BtRemoteDevice* remDev, BTIPS_RS_SERVICE_SEARCH_PARAMS *params);
static BTIPS_RS_Status BTIPS_RS_ServiceAttrWithLink(BtRemoteDevice* remDev, BTIPS_RS_SERVICE_ATTR_PARAMS *params);
static void BTIPS_RS_ResetGlobalCounter(void);
static void BTIPS_RS_PerformSearchAttrPreExitOperations(U8 *buff, BTIPS_RS_Status status, U8 release_ind, U8 end_connection_ind);
void BTIPS_RS_PerformSearchPreExitOperations(U8 * buff, U16 serviceHandlesCount, BTIPS_RS_Status status, U8 release_ind, U8 end_connection_ind);
void BTIPS_RS_PerformAttrPreExitOperations(U8 * buff, BTIPS_RS_Status status, U8 release_ind, U8 end_connection_ind);
BOOL BTIPS_RS_ParseHeader(U8 *buff, U16 len);

/* struct that holds the processed query data */
typedef struct 
{
	SdpQueryType	queryType;

	union
	{
		BTIPS_RS_SERVICE_SEARCH_ATTR_PARAMS *g_serviceSearchAttrParams;	
		BTIPS_RS_SERVICE_SEARCH_PARAMS *g_serviceSearchParams;
		BTIPS_RS_SERVICE_ATTR_PARAMS *g_serviceAttrParams;
	}g_params;			/* query parameters.*/							

	U16 g_serviceTotal;			/* total service records received - used in service search and service search attr requests.*/

	U16 g_servicesArrived;	/* service records arrived - used in service search request.*/

	U16 g_attrIndex;

	SdpQueryToken g_queryToken;	/* token for the SDP query.*/

	BtHandler g_handler;				/* handler for connection establishment.*/
	
	BtRemoteDevice * g_remDev;	/* remote device.*/
	
	U8 g_disconnect_at_exit;
	
	SdpAttributeId g_sortedAttributeIds[BTL_CONFIG_BMG_MAX_NUM_OF_ATTRIBUTES_TO_SEARCH]; /* for SDP query attributes list must be sorted.*/

	BOOL g_parseNeeded;		/* indicates whether to parse service search attributes for attributes values.*/
	
	U8 *g_curPos;					/* possition on the result buffer.*/
	
	U32 g_remainingLen;		/* remaining len till the end of a record.*/
	
	U8 g_headerMissingBytes; /* bytes missing in the header (in case not all response was received in one packet and there is header that was split in the middle) */

	U8 g_headerBuf[6];			/* used when not all response was received in one packet and there is header that was split in the middle to temporary hold the header.*/

	U8 g_headerMissing_offset; /* bytes received from the header (in case not all response was received in one packet and there is header that was split in the middle) */

	BOOL g_connectionEstablished; /* flag to indicate if the connection was established - for the use of cancel function */

	BOOL g_operationCanceled;	/* flag if the operation was canceled */
	
} BTIPS_RS_globals;
 

/* GLOBAL PARAMETERS */

/* Hoiuld the number of queries in progress.
 * Implemented as counter for future use
 * for supporting of multi queries processing.
 * can be 0 or 1 */
static U16 g_numberOfApp=0;

/* the processed query global data */
static BTIPS_RS_globals g_btips_rs;

/* FUNCTIONS */

void BTIPS_RS_Init(void)
{
	/* init the global remote device */
	g_btips_rs.g_remDev = 0;

	g_btips_rs.g_disconnect_at_exit = 0;

	/* copy the data struct pointer */
	g_btips_rs.g_params.g_serviceSearchAttrParams = 0;

	g_btips_rs.g_params.g_serviceSearchParams = 0;

	g_btips_rs.g_params.g_serviceAttrParams = 0;

	/* total of service records handled */
	g_btips_rs.g_serviceTotal = 0;

	/* handled attribute index */
	g_btips_rs.g_attrIndex = 0;

	g_btips_rs.g_remainingLen = 0;

	g_btips_rs.g_headerMissingBytes = 0;

	g_btips_rs.g_headerMissing_offset = 0;
	
	g_btips_rs.g_connectionEstablished = FALSE;

	g_btips_rs.g_operationCanceled = FALSE;

	OS_MemSet(g_btips_rs.g_headerBuf, 0, 6);
}

/*
 * The function performs pre exit operations.
 * 1) End the connection if needed
 * 2) release the global precced request counter if needed
 * 3) call the callback function - indicate operation end or failure. the resuls of the query are sent during BTIPS_RS_ExtractAttributes for every record
 */
void BTIPS_RS_PerformSearchAttrPreExitOperations(U8 * buff, BTIPS_RS_Status status, U8 release_ind, U8 end_connection_ind)
{
	BTL_FUNC_START("BTIPS_RS_PerformSearchAttrPreExitOperations");
	
	BTL_LOG_DEBUG(("BTIPS_RS_PerformSearchAttrPreExitOperations status: %d", status));
		
	if (end_connection_ind == 1)
		BTIPS_RS_EndConnection();
	
	if (release_ind == 1)
		BTIPS_RS_ResetGlobalCounter();
	if(g_btips_rs.g_operationCanceled == TRUE)
	{
		status = BTIPS_RS_STATUS_OPERATION_CANCELLED;
	}
	g_btips_rs.g_params.g_serviceSearchAttrParams->callbackFunc(buff, status);

	BTL_FUNC_END();
}

/*
 * The function performs pre exit operations.
 * 1) End the connection if needed
 * 2) release the global precced request counter if needed
 * 3) call the callback function - in case if success - buff holds the result handles
 */
void BTIPS_RS_PerformSearchPreExitOperations(U8 * buff, U16 serviceHandlesCount, BTIPS_RS_Status status, U8 release_ind, U8 end_connection_ind)
{
	BTL_FUNC_START("BTIPS_RS_PerformSearchPreExitOperations");
	
	BTL_LOG_DEBUG(("BTIPS_RS_PerformSearchPreExitOperations status: %d", status));
		
	if (end_connection_ind == 1)
		BTIPS_RS_EndConnection();
	
	if (release_ind == 1)
		BTIPS_RS_ResetGlobalCounter();
	if(g_btips_rs.g_operationCanceled == TRUE)
	{
		status = BTIPS_RS_STATUS_OPERATION_CANCELLED;
	}
	g_btips_rs.g_params.g_serviceSearchParams->callbackFunc(buff, serviceHandlesCount, status);

	BTL_FUNC_END();
}

/*
 * The function performs pre exit operations.
 * 1) End the connection if needed
 * 2) release the global precced request counter if needed
 * 3) call the callback function - in case if success - buff holds the result attributes
 */
void BTIPS_RS_PerformAttrPreExitOperations(U8 * buff, BTIPS_RS_Status status, U8 release_ind, U8 end_connection_ind)
{
	BTL_FUNC_START("BTIPS_RS_PerformAttrPreExitOperations");
	
	BTL_LOG_DEBUG(("BTIPS_RS_PerformAttrPreExitOperations status: %d", status));
		
	if (end_connection_ind == 1)
		BTIPS_RS_EndConnection();
	
	if (release_ind == 1)
		BTIPS_RS_ResetGlobalCounter();
	if(g_btips_rs.g_operationCanceled == TRUE)
	{
		status = BTIPS_RS_STATUS_OPERATION_CANCELLED;
	}
	g_btips_rs.g_params.g_serviceAttrParams->callbackFunc(buff, status);

	BTL_FUNC_END();
}


/*
 * The function sets the processed requests global counter to 0.
 */
 void BTIPS_RS_ResetGlobalCounter(void)
{
	g_numberOfApp=0;
}

/* 
 * get the number of service records from the result buff
*/
U16  BTIPS_RS_GetNumberOfAttributesFound(U8 *buff)
{
	return BTIPS_RS_GET_ATTRIBUTE_FOUND_NUM(buff);
}

/*
 * Called when control returns to the application
 * Disconnects the link if it was not created by the application
 */
BtStatus BTIPS_RS_EndConnection(void)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTIPS_RS_EndConnection");
	
	/* If g_btips_rs.g_disconnect_at_exit == 1, then connection was 
	 * established by us. */
	if (g_btips_rs.g_disconnect_at_exit != 0)
	{
		g_btips_rs.g_disconnect_at_exit = 0;
		status = ME_DisconnectLink(&g_btips_rs.g_handler, g_btips_rs.g_remDev);
	}

	BTL_FUNC_END();

	return status;
}

/* 
 * User input validation. checked conditions:
 * 1) BD_ADDR != NULL
 * 2) SDP parameters != NULL
 * 3) UUID list has at least 1 element
 * 4) attribute list has at least 1 element
 * 5) user supplied callback function != 0
 * 6) user max record limitation > 0
*/
static BTIPS_RS_Status BTIPS_RS_ValidateServiceSearchAttrInput(BD_ADDR * bd_addr, BTIPS_RS_SERVICE_SEARCH_ATTR_PARAMS * att_params)
{
	if (bd_addr == 0) {
		BTL_LOG_ERROR (("An Empty BD_ADDR"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (att_params == 0) {
		BTL_LOG_ERROR (("An Empty query struct"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (att_params->uuids==0) {
		BTL_LOG_ERROR (("An Empty UUID list"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if(att_params->numOfUuids > BTIPS_RS_MAX_UUIDS)
	{
		BTL_LOG_ERROR (("Max UUIDS per qery reached"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (att_params->attributeIds==0) {
		BTL_LOG_ERROR (("An Empty Attribute ID list"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (att_params->numOfAttributes==0) {
		BTL_LOG_ERROR (("An Empty Attribute ID list"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (att_params->numOfAttributes > BTL_CONFIG_BMG_MAX_NUM_OF_ATTRIBUTES_TO_SEARCH) {
		BTL_LOG_ERROR (("Max attributes per qery reached"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (att_params->callbackFunc==0) {
		BTL_LOG_ERROR (("NULL callback function"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (att_params->maxServiceRecords==0) {
		BTL_LOG_ERROR (("Max record must be more then 0"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	BTL_LOG_DEBUG(("Query Input is Valid"));
	
	return BTIPS_RS_STATUS_OK;

}

/* 
 * User input validation. checked conditions:
 * 1) BD_ADDR != NULL
 * 2) SDP parameters != NULL
 * 3) UUID list has at least 1 element
 * 5) user supplied callback function != 0
*/
static BTIPS_RS_Status BTIPS_RS_ValidateServiceSearchInput(BD_ADDR * bd_addr, BTIPS_RS_SERVICE_SEARCH_PARAMS * params)
{
	if (bd_addr == 0) {
		BTL_LOG_ERROR (("An Empty BD_ADDR"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (params == 0) {
		BTL_LOG_ERROR (("An Empty query struct"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (params->uuids==0) {
		BTL_LOG_ERROR (("An Empty UUID list"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if(params->numOfUuids > BTIPS_RS_MAX_UUIDS)
	{
		BTL_LOG_ERROR (("Max UUIDS per qery reached"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (params->callbackFunc==0) {
		BTL_LOG_ERROR (("NULL callback function"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (params->maxServiceRecords==0) {
		BTL_LOG_ERROR (("Max record must be more then 0"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	BTL_LOG_DEBUG(("Query Input is Valid"));
	
	return BTIPS_RS_STATUS_OK;

}

/* 
 * User input validation. checked conditions:
 * 1) BD_ADDR != NULL
 * 2) SDP parameters != NULL
 * 4) attribute list has at least 1 element
 * 5) user supplied callback function != 0
 * 6) user max record limitation > 0
*/
static BTIPS_RS_Status BTIPS_RS_ValidateServiceAttrInput(BD_ADDR * bd_addr, BTIPS_RS_SERVICE_ATTR_PARAMS * params)
{
	if (bd_addr == 0) {
		BTL_LOG_ERROR (("An Empty BD_ADDR"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (params == 0) {
		BTL_LOG_ERROR (("An Empty query struct"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (params->attributeIds==0) {
		BTL_LOG_ERROR (("An Empty Attribute ID list"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (params->numOfAttributes==0) {
		BTL_LOG_ERROR (("An Empty Attribute ID list"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (params->numOfAttributes > BTL_CONFIG_BMG_MAX_NUM_OF_ATTRIBUTES_TO_SEARCH) {
		BTL_LOG_ERROR (("Max attributes per qery reached"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	if (params->callbackFunc==0) {
		BTL_LOG_ERROR (("NULL callback function"));
		return BTIPS_RS_STATUS_INVALID_INPUT;
	}

	BTL_LOG_DEBUG(("Query Input is Valid"));
	
	return BTIPS_RS_STATUS_OK;

}


/* 
 * This is the compare function used by qsort.
 * compares between 2 attributes IDs
 */
static int BTIPS_RS_CompareAttrId(const void *id1, const void *id2)
{
	if (*((const SdpAttributeId *)id1) > *((const SdpAttributeId *)id2))
		return 1;
	else if (*((const SdpAttributeId *)id1) < *((const SdpAttributeId *)id2))
		return -1;
	return 0;
}

/*
 * The function sorts the attributes list.
 * It copies the attributes IDs list sent by the used
 * to a writable array, and sorts it.
 * It also counts the attributes.
 * Input : unsorted attributes ID array
 * Output: sorted attributes ID array 
 */
void BTIPS_RS_SortAttributes(SdpAttributeId * attributeIds, U16 numOfAttributes)
{
	U16 i;
	
	/* copy attribute id to writable array */
	for (i=0; i<numOfAttributes; ++i)
		g_btips_rs.g_sortedAttributeIds[i] = attributeIds[i];

	/* sort the attribute IDs */
	qsort((void *)g_btips_rs.g_sortedAttributeIds, numOfAttributes, sizeof(SdpAttributeId), BTIPS_RS_CompareAttrId);

}

/*
 * callback function to handle the event of
 * respond after connection to the remote device
 */
static void BTIPS_RS_ConnectionEvent(const BtEvent * event)
{
	BTIPS_RS_Status status;

	BTL_FUNC_START("BTIPS_RS_ConnectionEvent");

	BTL_LOG_DEBUG(("event: %s (errCode: %s)", pME_Event(event->eType), pHC_Status(event->errCode)));
	
	switch(event->eType)
	{
	/* we got OK respond - we have a connection */
	case BTEVENT_LINK_CONNECT_CNF:
		
		if ((BEC_NO_ERROR == event->errCode)&&(!(g_btips_rs.g_operationCanceled)))
		{
			/* since we established the connection, 
			 * we must indicate that we need to disconnect later */
			g_btips_rs.g_disconnect_at_exit = 1;

			if(g_btips_rs.queryType == BSQT_SERVICE_SEARCH_ATTRIB_REQ)
			{
				status = BTIPS_RS_ServiceSearchAttrWithLink(g_btips_rs.g_remDev, g_btips_rs.g_params.g_serviceSearchAttrParams);

				/* failed sending the query */
				if (status != BTIPS_RS_STATUS_PENDING) 
				{
					BTL_LOG_ERROR(("BTIPS_RS_ServiceSearchAttrWithLink Failed (%d)", status));
					BTIPS_RS_PerformSearchAttrPreExitOperations(0, BTIPS_RS_STATUS_INTERNAL, 1, 1);
				}
			}
			else if(g_btips_rs.queryType == BSQT_SERVICE_SEARCH_REQ)
			{
				status = BTIPS_RS_ServiceSearchWithLink(g_btips_rs.g_remDev, g_btips_rs.g_params.g_serviceSearchParams);

				/* failed sending the query */
				if (status != BTIPS_RS_STATUS_PENDING) 
				{
					BTL_LOG_ERROR(("BTIPS_RS_ServiceSearchWithLink Failed (%d)", status));
					BTIPS_RS_PerformSearchPreExitOperations(0, 0, BTIPS_RS_STATUS_INTERNAL, 1, 1);
				}

			}
			else /*(g_btips_rs.queryType == BSQT_ATTRIB_REQ)*/
			{
				status = BTIPS_RS_ServiceAttrWithLink(g_btips_rs.g_remDev, g_btips_rs.g_params.g_serviceAttrParams);

				/* failed sending the query */
				if (status != BTIPS_RS_STATUS_PENDING) 
				{
					BTL_LOG_ERROR(("BTIPS_RS_ServiceAttrWithLink Failed (%d)", status));
					BTIPS_RS_PerformAttrPreExitOperations(0, BTIPS_RS_STATUS_INTERNAL, 1, 1);
				}

			}

		}
		else
		{	
			if(g_btips_rs.g_operationCanceled)
			{
				BTL_LOG_INFO(("Link Establishment Canceled "));
				status= BTIPS_RS_STATUS_OPERATION_CANCELLED;
			}
			else
			{
				BTL_LOG_INFO(("Link Establishment Failed (%s)", pHC_Status(event->errCode)));
				status= BTIPS_RS_STATUS_CONNECT_FAILURE;
			}
			
			if(g_btips_rs.queryType == BSQT_SERVICE_SEARCH_ATTRIB_REQ)
			{
				BTIPS_RS_PerformSearchAttrPreExitOperations(0, status, 1, 0);
			}
			else if(g_btips_rs.queryType == BSQT_SERVICE_SEARCH_REQ)
			{
				BTIPS_RS_PerformSearchPreExitOperations(0, 0, status, 1, 1);
			}
			else /*(g_btips_rs.queryType == BSQT_ATTRIB_REQ)*/
			{
				BTIPS_RS_PerformAttrPreExitOperations(0, status, 1, 1);
			}
		}

		break;

	case BTEVENT_ROLE_CHANGE:

		/* Do Nothing */

		break;

	/* some kind of error */
	default:
			if(g_btips_rs.queryType == BSQT_SERVICE_SEARCH_ATTRIB_REQ)
			{
				BTIPS_RS_PerformSearchAttrPreExitOperations(0, BTIPS_RS_STATUS_CONNECT_FAILURE, 1, 0);
			}
			else if(g_btips_rs.queryType == BSQT_SERVICE_SEARCH_REQ)
			{
				BTIPS_RS_PerformSearchPreExitOperations(0, 0, BTIPS_RS_STATUS_CONNECT_FAILURE, 1, 1);
			}
			else /*(g_btips_rs.queryType == BSQT_ATTRIB_REQ)*/
			{
				BTIPS_RS_PerformAttrPreExitOperations(0, BTIPS_RS_STATUS_CONNECT_FAILURE, 1, 1);
			}

		break;
	}

	BTL_FUNC_END();
}

/*
 * try establishing connection to the remote device
 * using the remote device's BD_ADDR
 */
static BTIPS_RS_Status BTIPS_RS_EstablishConnection(BD_ADDR * bdAddr)
{
	BtStatus		status;
	BTIPS_RS_Status	rsStatus = BTIPS_RS_STATUS_OK;
	BtDeviceContext *device;

	BTL_FUNC_START("BTIPS_RS_EstablishConnection");
	
	/* Verify the device is registered */
	device = DS_FindDevice(bdAddr);
	
	BTL_VERIFY_FATAL_SET_RETVAR(	(device != 0), 
									rsStatus = BTIPS_RS_STATUS_DEVICE_NOT_REGISTERED, 
									("BTIPS_RS_EstablishConnection: The BT device is not registered with the DS."));

	ME_InitHandler(&g_btips_rs.g_handler);
	g_btips_rs.g_handler.callback = BTIPS_RS_ConnectionEvent;
	
	status = ME_CreateLink(&g_btips_rs.g_handler, bdAddr, &device->psi, &g_btips_rs.g_remDev);

	/* Only when PENDING, the callback is called. */
	if (status == BT_STATUS_SUCCESS){
		/* since we establish the connection, 
		 * we must indicate that we need to disconnect later */
		BTL_LOG_DEBUG(("BTIPS_RS_EstablishConnection: Connection was already established"));
		
		g_btips_rs.g_disconnect_at_exit = 1;
		rsStatus = BTIPS_RS_STATUS_OK;
	}
	else  if (status == BT_STATUS_PENDING)
	{
		BTL_LOG_DEBUG(("Waiting for SDP Query link to establish"));
		rsStatus = BTIPS_RS_STATUS_PENDING;
	}
	else
	{
		BTL_ERR_SET_RETVAR(	(rsStatus = BTIPS_RS_STATUS_CONNECT_FAILURE),
								("BTIPS_RS_EstablishConnection: Error connecting to device, status: %s", pBT_Status(status)));
	}

	BTL_FUNC_END();

	return rsStatus;
}
/*
 * cancel the astablishment of the connection
 * if it is not elready established
 */
static BTIPS_RS_Status BTIPS_RS_CancelEstablishmentOfConnection()
{
	BtStatus		status;
	BTIPS_RS_Status rsStatus = BTIPS_RS_STATUS_PENDING;
	
	BTL_FUNC_START("BTIPS_RS_CancelEstablishmentOfConnection");
	
	if(!(g_btips_rs.g_connectionEstablished))
	{
		/* there is no connection yet */
		status = ME_CancelCreateLink(&g_btips_rs.g_handler, g_btips_rs.g_remDev);
		
		if (status == BT_STATUS_SUCCESS)
		{
			BTL_LOG_DEBUG(("BTIPS_RS_CancelEstablishmentOfConnection: Connection was canceled"));

			g_btips_rs.g_disconnect_at_exit = 0;
			rsStatus = BTIPS_RS_STATUS_OK;
		}
		else
		{
			BTL_ERR_SET_RETVAR( (rsStatus = BTIPS_RS_STATUS_QUERY_FAILURE),
								("BTIPS_RS_CancelEstablishmentOfConnection: Error Connection operation is not in progress, status: %s", pBT_Status(status)));
		}
	}

	BTL_FUNC_END();

	return rsStatus;
}

/*
 * Fetch attributes requested by the application
 * 1) Create connection ACL
 * 2) call BTIPS_RS_ServiceSearchAttrWithLink()
 */
BTIPS_RS_Status BTIPS_RS_ServiceSearchAttr(BD_ADDR *bd_addr, BTIPS_RS_SERVICE_SEARCH_ATTR_PARAMS *att_params, BOOL parseNeeded)
{
	BTIPS_RS_Status status = BTIPS_RS_STATUS_PENDING;

	BTL_FUNC_START("BTIPS_RS_ServiceSearchAttr");

	BTL_VERIFY_FATAL_SET_RETVAR(g_numberOfApp == 0, status = BTIPS_RS_STATUS_QUERY_IN_PROGRESS, ("A request is in progress"));
	
	/* validate input parameters */
	status = BTIPS_RS_ValidateServiceSearchAttrInput(bd_addr, att_params);
	BTL_VERIFY_ERR((status == BTIPS_RS_STATUS_OK), status, ("BTIPS_RS_ValidateServiceSearchAttrInput Failed (%d)", status));

	/* Init global parameters */
	BTIPS_RS_Init();			

	/* save SDP Query parameters struct */
	g_btips_rs.queryType = BSQT_SERVICE_SEARCH_ATTRIB_REQ;
	g_btips_rs.g_params.g_serviceSearchAttrParams = att_params;
	g_btips_rs.g_parseNeeded = parseNeeded;
	
	/* create ACL Link */
	status = BTIPS_RS_EstablishConnection(bd_addr);
	
	/* connection was established already */
	if (status == BTIPS_RS_STATUS_OK)
	{
		BTL_LOG_DEBUG(("Connection already exists, calling BTIPS_RS_ServiceSearchAttrWithLink"));
		
		status = BTIPS_RS_ServiceSearchAttrWithLink(g_btips_rs.g_remDev, g_btips_rs.g_params.g_serviceSearchAttrParams);
		BTL_VERIFY_FATAL(status == BTIPS_RS_STATUS_PENDING, status, ("BTIPS_RS_ServiceSearchAttrWithLink Failed (%d)", status));
	}
	else if (status != BTIPS_RS_STATUS_PENDING)
	{
		BTL_FATAL(BTIPS_RS_STATUS_INTERNAL, ("BTIPS_RS_EstablishConnection Failed Unexpectedly (%d)", status));
	}

	g_numberOfApp=1;

	BTL_FUNC_END();

	return status;
}

/*
 * Fetch attributes requested by the application
 */
BTIPS_RS_Status BTIPS_RS_ServiceSearchAttrWithLink(BtRemoteDevice* remDev, BTIPS_RS_SERVICE_SEARCH_ATTR_PARAMS *att_params)
{
	U8 * 			requestBuffer;
	BtStatus 			btStatus;
	BTIPS_RS_Status	status = BTIPS_RS_STATUS_PENDING;
	U16 				size;

	BTL_FUNC_START("BTIPS_RS_ServiceSearchAttrWithLink");
	
	BTL_VERIFY_FATAL((remDev != 0), BTIPS_RS_STATUS_INTERNAL, ("Null remDev"));
	BTL_VERIFY_FATAL((att_params != 0), BTIPS_RS_STATUS_INTERNAL, ("Null att_params"));

	/* save the remote device details */
	g_btips_rs.g_remDev = remDev;

	/* the connection was established */
	g_btips_rs.g_connectionEstablished = TRUE;
	
	/* sort the attribute Ids */
	BTIPS_RS_SortAttributes(att_params->attributeIds, att_params->numOfAttributes);

	/* Build the Service search request */
	requestBuffer = BTIPS_RS_BuildServiceAttributeSearch(	att_params->uuids, 
														att_params->numOfUuids, 
														g_btips_rs.g_sortedAttributeIds, 
														att_params->numOfAttributes, 
														&size);

	/* Initialize the queryToken */
	g_btips_rs.g_queryToken.callback = BTIPS_RS_ServiceSearchAttributeEventHandler;
	g_btips_rs.g_queryToken.parms = requestBuffer;
	g_btips_rs.g_queryToken.plen = size;
	g_btips_rs.g_queryToken.type = BSQT_SERVICE_SEARCH_ATTRIB_REQ;
	g_btips_rs.g_queryToken.rm = g_btips_rs.g_remDev;
	g_btips_rs.g_queryToken.mode = BSPM_BEGINNING;
	
	/* perform the Query */
	btStatus = SDP_Query(&g_btips_rs.g_queryToken, BSQM_FIRST);
	BTL_VERIFY_FATAL(btStatus == BT_STATUS_PENDING, BTIPS_RS_STATUS_INTERNAL, ("SDP_Query Failed (%s)", pBT_Status(btStatus)));

	BTL_FUNC_END();

	return status;
}
/*
 *
 *
 * 
 */
BTIPS_RS_Status BTIPS_RS_CancelServiceSearchAttr()
{
	BTIPS_RS_Status status = BTIPS_RS_STATUS_PENDING;

	BTL_FUNC_START("BTIPS_RS_CancelServiceSearchAttr");

	g_btips_rs.g_operationCanceled = TRUE;	

	
	
	/* cancel ACL Link creation */
	status = BTIPS_RS_CancelEstablishmentOfConnection();
	if(status == BTIPS_RS_STATUS_OK)
	{
		BTIPS_RS_ResetGlobalCounter();
	}
	
	BTL_FUNC_END();

	return status;
}

/*
 * Fetch record handles requested by the application
 */
BTIPS_RS_Status BTIPS_RS_ServiceSearch(BD_ADDR *bd_addr, BTIPS_RS_SERVICE_SEARCH_PARAMS *params)
{
	BTIPS_RS_Status status = BTIPS_RS_STATUS_PENDING;

	BTL_FUNC_START("BTIPS_RS_ServiceSearch");

	BTL_VERIFY_FATAL_SET_RETVAR(g_numberOfApp == 0, status = BTIPS_RS_STATUS_QUERY_IN_PROGRESS, ("A request is in progress"));
	
	/* validate input parameters */
	status = BTIPS_RS_ValidateServiceSearchInput(bd_addr, params);
	BTL_VERIFY_ERR((status == BTIPS_RS_STATUS_OK), status, ("BTIPS_RS_ValidateServiceSearchInput Failed (%d)", status));

	/* Init global parameters */
	BTIPS_RS_Init();			

	/* save SDP Query parameters struct */
	g_btips_rs.queryType = BSQT_SERVICE_SEARCH_REQ;
	g_btips_rs.g_params.g_serviceSearchParams = params;
	
	/* create ACL Link */
	status = BTIPS_RS_EstablishConnection(bd_addr);
	
	/* connection was established already */
	if (status == BTIPS_RS_STATUS_OK)
	{
		BTL_LOG_DEBUG(("Connection already exists, calling BTIPS_RS_ServiceSearchAttrWithLink"));
		
		status = BTIPS_RS_ServiceSearchWithLink(g_btips_rs.g_remDev, g_btips_rs.g_params.g_serviceSearchParams);
		BTL_VERIFY_FATAL(status == BTIPS_RS_STATUS_PENDING, status, ("BTIPS_RS_ServiceSearchWithLink Failed (%d)", status));
	}
	else if (status != BTIPS_RS_STATUS_PENDING)
	{
		BTL_FATAL(BTIPS_RS_STATUS_INTERNAL, ("BTIPS_RS_EstablishConnection Failed Unexpectedly (%d)", status));
	}

	g_numberOfApp=1;

	BTL_FUNC_END();

	return status;
}

/*
 * Fetch attributes requested by the application for the given handle
 */
BTIPS_RS_Status BTIPS_RS_ServiceSearchWithLink(BtRemoteDevice* remDev, BTIPS_RS_SERVICE_SEARCH_PARAMS *params)
{
	U8 * 			requestBuffer;
	BtStatus 			btStatus;
	BTIPS_RS_Status	status = BTIPS_RS_STATUS_PENDING;
	U16 				size;

	BTL_FUNC_START("BTIPS_RS_ServiceSearchWithLink");
	
	/*
 	 * Fetch attributes requested by the application for the given handle
 	 */
	BTL_VERIFY_FATAL((remDev != 0), BTIPS_RS_STATUS_INTERNAL, ("Null remDev"));
	BTL_VERIFY_FATAL((params != 0), BTIPS_RS_STATUS_INTERNAL, ("Null params"));

	/* save the remote device details */
	g_btips_rs.g_remDev = remDev;

	/* the connection was established */
	g_btips_rs.g_connectionEstablished = TRUE;
	
	/* Build the Service search request */
	requestBuffer = BTIPS_RS_BuildServiceSearch(params->uuids, 
														params->numOfUuids, 
														&size);

	/* Initialize the queryToken */
	g_btips_rs.g_queryToken.callback = BTIPS_RS_ServiceSearchEventHandler;
	g_btips_rs.g_queryToken.parms = requestBuffer;
	g_btips_rs.g_queryToken.plen = size;
	g_btips_rs.g_queryToken.type = BSQT_SERVICE_SEARCH_REQ;
	g_btips_rs.g_queryToken.rm = g_btips_rs.g_remDev;
	g_btips_rs.g_queryToken.mode = BSPM_BEGINNING;
	
	
	/* perform the Query */
	btStatus = SDP_Query(&g_btips_rs.g_queryToken, BSQM_FIRST);
	BTL_VERIFY_FATAL(btStatus == BT_STATUS_PENDING, BTIPS_RS_STATUS_INTERNAL, ("SDP_Query Failed (%s)", pBT_Status(btStatus)));

	BTL_FUNC_END();

	return status;
}
/*
 *
 *
 * 
 */
BTIPS_RS_Status BTIPS_RS_CancelServiceSearch()
{
	BTIPS_RS_Status status = BTIPS_RS_STATUS_PENDING;

	BTL_FUNC_START("BTIPS_RS_CancelServiceSearch");

	g_btips_rs.g_operationCanceled = TRUE;	

	
	
	/* cancel ACL Link creation */
	status = BTIPS_RS_CancelEstablishmentOfConnection();
	if(status == BTIPS_RS_STATUS_OK)
	{
		BTIPS_RS_ResetGlobalCounter();
	}

	BTL_FUNC_END();

	return status;
}

/*
 * Build request buffer to be send to the peer device.
 */
static U8* BTIPS_RS_BuildServiceSearch(BtlBmgUuid *uuids, U8 uuid_count, U16 *tot_size)
{
	static U8 requestBuffer[MAX_SERVICE_REQUEST_SIZE];
	
	U8 * tmp_uuid;
	U16 size=0;
	U16 i;
	static const U16 MAX_RECORD_HANDLES_COUNT = 20;

	*tot_size=0;

	/* add the uuids 
	 * First add the data, then the header */
	size+=sizeof(U16);
	
	for (i=0; i<uuid_count; ++i) 
	{
		tmp_uuid = (U8 *)(&uuids[i]);

		if(OS_MemCmp(&BT_BASE_UUID[4], 12, &tmp_uuid[4], 12))
		{
			/* BT based UUID*/
			if((tmp_uuid[0] == 0) &&(tmp_uuid[1] == 0))
			{
				/* 16 bit UUID*/
				requestBuffer[size++] = DETD_UUID + DESD_2BYTES;
				/* uuid is already big endian - copy as it is.*/
				requestBuffer[size++] = tmp_uuid[2];
				requestBuffer[size++] = tmp_uuid[3];
			}
			else
			{
				/* 32 bit UUID*/
				requestBuffer[size++] = DETD_UUID + DESD_4BYTES;
				/* uuid is already big endian - copy as it is.*/
				requestBuffer[size++] = tmp_uuid[0];
				requestBuffer[size++] = tmp_uuid[1];
				requestBuffer[size++] = tmp_uuid[2];
				requestBuffer[size++] = tmp_uuid[3];
			}
		}
		else
		{
				/* 128 bit UUID*/
				/* uuid is already big endian - copy as it is.*/
				requestBuffer[size++] = DETD_UUID + DESD_16BYTES;
				OS_MemCopy(&(requestBuffer[size]), tmp_uuid, UUID_MAX_SIZE);
				size += UUID_MAX_SIZE;
		}
	}

	/* uuid list header */
	requestBuffer[0] = DETD_SEQ + DESD_ADD_8BITS;
	requestBuffer[1] = (U8)(size - sizeof(U16)); /* decrease the header size.*/

	StoreBE16(&requestBuffer[size], MAX_RECORD_HANDLES_COUNT);
	size += sizeof(U16);

	*tot_size = size;

	return requestBuffer;

}

/*
 * Build request buffer to be send to the peer device.
 */
static U8* BTIPS_RS_BuildServiceAttributeSearch(BtlBmgUuid * uuids, U8 uuid_count, SdpAttributeId * attributeIDs, U16 attribute_count, U16 * tot_size)
{
	static U8 requestBuffer[MAX_SERVICE_SEARCH_ATTR_REQUEST_SIZE];

	/* set the size of max byte count returned from the SDP Server
	 * This number was taken as used by ESI code.
	 * In most cases it is bigger then the actual response size of the SDP Server*/
	static const U16 MAX_ATTR_BYTE_COUNT = 672;
	
	U16 attributes_loc;
	U8 * tmp_uuid;
	U16 attr_size;
	U16 size=0;
	U16 i;

	*tot_size=0;

	/* add the uuids 
	 * First add the data, then the header */
	size+=sizeof(U16);
	
	for (i=0; i<uuid_count; ++i) 
	{
		tmp_uuid = (U8 *)(&uuids[i]);

		if(OS_MemCmp(&BT_BASE_UUID[4], 12, &tmp_uuid[4], 12))
		{
			/* BT based UUID*/
			if((tmp_uuid[0] == 0) &&(tmp_uuid[1] == 0))
			{
				/* 16 bit UUID*/
				requestBuffer[size++] = DETD_UUID + DESD_2BYTES;
				/* uuid is already big endian - copy as it is.*/
				requestBuffer[size++] = tmp_uuid[2];
				requestBuffer[size++] = tmp_uuid[3];
			}
			else
			{
				/* 32 bit UUID*/
				requestBuffer[size++] = DETD_UUID + DESD_4BYTES;
				/* uuid is already big endian - copy as it is.*/
				requestBuffer[size++] = tmp_uuid[0];
				requestBuffer[size++] = tmp_uuid[1];
				requestBuffer[size++] = tmp_uuid[2];
				requestBuffer[size++] = tmp_uuid[3];
			}
		}
		else
		{
				/* 128 bit UUID*/
				/* uuid is already big endian - copy as it is.*/
				requestBuffer[size++] = DETD_UUID + DESD_16BYTES;
				OS_MemCopy(&(requestBuffer[size]), tmp_uuid, UUID_MAX_SIZE);
				size += UUID_MAX_SIZE;
		}
	}

	/* uuid list header */
	requestBuffer[0] = DETD_SEQ + DESD_ADD_8BITS;
	requestBuffer[1] = (U8)(size - sizeof(U16)); /* decrease the header size.*/

  /* Max number of bytes for attribute is 672 */
	StoreBE16(&requestBuffer[size], MAX_ATTR_BYTE_COUNT);
	size += sizeof(U16);

	/* set attribute list 
	 * First add the data, then the header 
	 * Save space for header */
	attributes_loc = size;
	size+=1+sizeof(U16);	/* one byte header + 2 bytes size*/

	/* Insert data elements of Attribute IDs for the search pattern */
	for (i=0;i < attribute_count; ++i) {
		requestBuffer[size++] = DETD_UINT + DESD_2BYTES;
		StoreBE16(&requestBuffer[size], attributeIDs[i]);
		size += sizeof(U16);
	}

	/* attribute Id list header */
	attr_size = (U16)(i*3);
	requestBuffer[attributes_loc] = DETD_SEQ + DESD_ADD_16BITS;
	StoreBE16(&requestBuffer[attributes_loc + 1], attr_size);
	
	*tot_size = size;

	return requestBuffer;
}

/*
 * Build request buffer to be send to the peer device.
 */
BTIPS_RS_Status BTIPS_RS_ServiceAttr(BD_ADDR *bd_addr, BTIPS_RS_SERVICE_ATTR_PARAMS *params)
{
	BTIPS_RS_Status status = BTIPS_RS_STATUS_PENDING;

	BTL_FUNC_START("BTIPS_RS_ServiceAttr");

	BTL_VERIFY_FATAL_SET_RETVAR(g_numberOfApp == 0, status = BTIPS_RS_STATUS_QUERY_IN_PROGRESS, ("A request is in progress"));
	
	/* validate input parameters */
	status = BTIPS_RS_ValidateServiceAttrInput(bd_addr, params);
	BTL_VERIFY_ERR((status == BTIPS_RS_STATUS_OK), status, ("BTIPS_RS_ValidateServiceAttrInput Failed (%d)", status));

	/* Init global parameters */
	BTIPS_RS_Init();			

	/* save SDP Query parameters struct */
	g_btips_rs.queryType = BSQT_ATTRIB_REQ;
	g_btips_rs.g_params.g_serviceAttrParams = params;
	
	/* create ACL Link */
	status = BTIPS_RS_EstablishConnection(bd_addr);
	
	/* connection was established already */
	if (status == BTIPS_RS_STATUS_OK)
	{
		BTL_LOG_DEBUG(("Connection already exists, calling BTIPS_RS_ServiceSearchAttrWithLink"));
		
		status = BTIPS_RS_ServiceAttrWithLink(g_btips_rs.g_remDev, g_btips_rs.g_params.g_serviceAttrParams);
		BTL_VERIFY_FATAL(status == BTIPS_RS_STATUS_PENDING, status, ("BTIPS_RS_ServiceAttrWithLink Failed (%d)", status));
	}
	else if (status != BTIPS_RS_STATUS_PENDING)
	{
		BTL_FATAL(BTIPS_RS_STATUS_INTERNAL, ("BTIPS_RS_EstablishConnection Failed Unexpectedly (%d)", status));
	}

	g_numberOfApp=1;

	BTL_FUNC_END();

	return status;
}

/*
 * Fetch attributes requested by the application for the given handle
 */
BTIPS_RS_Status BTIPS_RS_ServiceAttrWithLink(BtRemoteDevice* remDev, BTIPS_RS_SERVICE_ATTR_PARAMS *params)
{
	U8 * 			requestBuffer;
	BtStatus 			btStatus;
	BTIPS_RS_Status	status = BTIPS_RS_STATUS_PENDING;
	U16 				size;

	BTL_FUNC_START("BTIPS_RS_ServiceAttrWithLink");
	
	BTL_VERIFY_FATAL((remDev != 0), BTIPS_RS_STATUS_INTERNAL, ("Null remDev"));
	BTL_VERIFY_FATAL((params != 0), BTIPS_RS_STATUS_INTERNAL, ("Null params"));

	/* save the remote device details */
	g_btips_rs.g_remDev = remDev;

	/* the connection was established */
	g_btips_rs.g_connectionEstablished = TRUE;
	
	BTIPS_RS_SortAttributes(params->attributeIds, params->numOfAttributes);

	/* Build the Service search request */
	requestBuffer = BTIPS_RS_BuildServiceAttribute(params->recordHandle, 
														g_btips_rs.g_sortedAttributeIds,
														params->numOfAttributes,
														&size);

	/* Initialize the queryToken */
	g_btips_rs.g_queryToken.callback = BTIPS_RS_ServiceAttrEventHandler;
	g_btips_rs.g_queryToken.parms = requestBuffer;
	g_btips_rs.g_queryToken.plen = size;
	g_btips_rs.g_queryToken.type = BSQT_ATTRIB_REQ;
	g_btips_rs.g_queryToken.rm = g_btips_rs.g_remDev;
	g_btips_rs.g_queryToken.mode = BSPM_BEGINNING;

	
	/* perform the Query */
	btStatus = SDP_Query(&g_btips_rs.g_queryToken, BSQM_FIRST);
	BTL_VERIFY_FATAL(btStatus == BT_STATUS_PENDING, BTIPS_RS_STATUS_INTERNAL, ("SDP_Query Failed (%s)", pBT_Status(btStatus)));

	BTL_FUNC_END();

	return status;
}
/*
 *
 *
 * 
 */
BTIPS_RS_Status BTIPS_RS_CancelServiceAttr()
{
	BTIPS_RS_Status status = BTIPS_RS_STATUS_PENDING;

	BTL_FUNC_START("BTIPS_RS_CancelServiceSearch");

	g_btips_rs.g_operationCanceled = TRUE;	

	
	
	/* cancel ACL Link creation */
	status = BTIPS_RS_CancelEstablishmentOfConnection();
	if(status == BTIPS_RS_STATUS_OK)
	{
	  BTIPS_RS_ResetGlobalCounter();
	}

	BTL_FUNC_END();

	return status;
}

/*
 * Build request buffer to be send to the peer device.
 */
static U8* BTIPS_RS_BuildServiceAttribute(U32 recordHandle, SdpAttributeId *attributeIDs, U16 attribute_count, U16 *tot_size)
{
	static U8 requestBuffer[MAX_SERVICE_ATTR_REQUEST_SIZE];

	/* set the size of max byte count returned from the SDP Server
	 * This number was taken as used by ESI code.
	 * In most cases it is bigger then the actual response size of the SDP Server*/
	static const U16 MAX_ATTR_BYTE_COUNT = 672;
	
	U16 attributes_loc;
	U16 attr_size;
	U16 size=0;
	U16 i;

	*tot_size=0;

	StoreBE32(&requestBuffer[size], recordHandle);
	size += sizeof(U32);

  /* Max number of bytes for attribute is 672 */
	StoreBE16(&requestBuffer[size], MAX_ATTR_BYTE_COUNT);
	size += sizeof(U16);

	/* set attribute list 
	 * First add the data, then the header 
	 * Save space for header */
	attributes_loc = size;
	size+=1+sizeof(U16);

	/* Insert data elements of Attribute IDs for the search pattern */
	for (i=0;i < attribute_count; ++i) {
		requestBuffer[size++] = DETD_UINT + DESD_2BYTES;
		StoreBE16(&requestBuffer[size], attributeIDs[i]);
		size += sizeof(U16);
	}

	/* attribute Id list header */
	attr_size = (U16)(i*3);
	requestBuffer[attributes_loc] = DETD_SEQ + DESD_ADD_16BITS;
	StoreBE16(&requestBuffer[attributes_loc + 1], attr_size);
	
	*tot_size = size;

	return requestBuffer;
}

 /*
 * callback function to handle the returned records after the SDP_Query
 * In case of an error, the error will be passed to the upper layer.
 * In case of a legal response - BTIPS_RS_ExtractRecordHandles is called to parse the result to records and build the result buffer. 
 * BTIPS_RS_ExtractRecordHandles calles the upper layer callback for every record. In case not all data received 
 * (continuation state) SDP_Query will be called again to retrieve the rest. When all data was received BTIPS_RS_STATUS_OPERATION_END
 * will be sent to indicate the end of the records.
 */
static void BTIPS_RS_ServiceSearchAttributeEventHandler(const BtEvent* event)
{
	BTIPS_RS_Status 	status = BTIPS_RS_STATUS_OPERATION_END; /* if operation cancelled don't continue */
	BtStatus			btStatus;

	BTL_FUNC_START("BTIPS_RS_ServiceSearchAttributeEventHandler");
	
	BTL_LOG_DEBUG(("event is %s\n", pME_Event(event->eType)));
	
	switch(event->eType) {
		
	case SDEVENT_QUERY_RSP:
		
		if(g_btips_rs.g_operationCanceled != TRUE)
		{
			/* extract attributes after query */
			if(TRUE == g_btips_rs.g_parseNeeded)
			{
				status = BTIPS_RS_ExtractAttributesAndParse(event->p.token);
				BTL_LOG_DEBUG(("BTIPS_RS_ExtractAttributesAndParse: status is %d\n", status));
			}
			else
			{
				status = BTIPS_RS_ExtractAttributes(event->p.token);
				BTL_LOG_DEBUG(("BTIPS_RS_ExtractAttributes: status is %d\n", status));
			}
		}
		switch (status)
		{
		case BTIPS_RS_STATUS_SDP_CONT_STATE:

			/* continuation state */

			BTL_LOG_DEBUG(("Continuation state. call SDP_Query again\n"));

			btStatus = SDP_Query(&g_btips_rs.g_queryToken, BSQM_CONTINUE);
			
			if (btStatus != BT_STATUS_PENDING)
			{
				BTL_FATAL_NORET(("SDP_Query Failed (%s)", pBT_Status(btStatus)));
				BTIPS_RS_PerformSearchAttrPreExitOperations(0, BTIPS_RS_STATUS_QUERY_FAILURE, 1, 1);
			}
			
		break;
		
		case BTIPS_RS_STATUS_QUERY_EMPTY:

			BTIPS_RS_PerformSearchAttrPreExitOperations(0, status, 1, 1);
			
		break;
	
		case BTIPS_RS_STATUS_OPERATION_END:

			BTIPS_RS_PerformSearchAttrPreExitOperations(0, status, 1, 1);
			
		break;
	
		default:
			{
				/* An error was returned. status other then SUCCESS or PENDING*/
				
				U8 *buff = 0;
		
				BTL_LOG_ERROR(("BTIPS_RS_ExtractAttributesAndParse or BTIPS_RS_ExtractAttributes Failed (%d)", status));
				
				/* operation was ended (no more service records) or an error occured.*/
				BTIPS_RS_PerformSearchAttrPreExitOperations(buff, status, 1, 1);
			}

		break;

		}

	break;

	case SDEVENT_QUERY_ERR:
    	case SDEVENT_QUERY_FAILED:
		BTL_ERR_NORET(("SDP Query Failed (%s)", pME_Event(event->eType)));
		
		BTIPS_RS_PerformSearchAttrPreExitOperations(0, BTIPS_RS_STATUS_QUERY_FAILURE, 1, 1);
			
	break;
	
	default:
		BTL_FATAL_NORET(("Unexpecter event type received for query (%s)", pME_Event(event->eType)));
		
		BTIPS_RS_PerformSearchAttrPreExitOperations(0, BTIPS_RS_STATUS_INTERNAL, 1, 1);
		
	break;
	
	}
}

/*
 * callback function to handle the returned
 * the record handles after the SDP_Query
 * In case of an error, the error will be passed to the upper layer.
 * In case of a legal response - BTIPS_RS_ExtractRecordHandles is called to fetch the handles and build the result buffer to 
 * be sent to the upper layer when done. In case not all handles received (continuation state) SDP_Query will be called again to 
 * retrieve the rest.
 */
static void BTIPS_RS_ServiceSearchEventHandler(const BtEvent* event)
{
	BTIPS_RS_Status 	status = BTIPS_RS_STATUS_OPERATION_END; /* if operation cancelled don't continue */
	BtStatus			btStatus;
	
	BTL_FUNC_START("BTIPS_RS_ServiceSearchEventHandler");
	
	BTL_LOG_DEBUG(("event is %s\n", pME_Event(event->eType)));
	
	switch(event->eType) {
		
	case SDEVENT_QUERY_RSP:
		if(g_btips_rs.g_operationCanceled != TRUE)
		{
			
			status = BTIPS_RS_ExtractRecordHandles(event->p.token);
			BTL_LOG_DEBUG(("BTIPS_RS_ExtractRecordHandles: status is %d\n", status));
		}
		switch (status)
		{
		case BTIPS_RS_STATUS_SDP_CONT_STATE:
			
			/* continuation state */
			
			BTL_LOG_DEBUG(("Continuation state. call SDP_Query again\n"));
			
			btStatus = SDP_Query(&g_btips_rs.g_queryToken, BSQM_CONTINUE);
			
			if (btStatus != BT_STATUS_PENDING)
			{
				BTL_FATAL_NORET(("SDP_Query Failed (%s)", pBT_Status(btStatus)));
				BTIPS_RS_PerformSearchPreExitOperations(0, 0, BTIPS_RS_STATUS_QUERY_FAILURE, 1, 1);
			}
			
			break;

		case BTIPS_RS_STATUS_QUERY_EMPTY:
			
			BTIPS_RS_PerformSearchPreExitOperations(0, 0, status, 1, 1);
			
			break;
			
		case BTIPS_RS_STATUS_OPERATION_END:
			
			BTIPS_RS_PerformSearchPreExitOperations(g_btips_rs.g_params.g_serviceSearchParams->buff, g_btips_rs.g_servicesArrived, status, 1, 1);
			
			break;
			
		default:
			{
				/* An error was returned. status other then SUCCESS or PENDING*/
				
				BTL_LOG_ERROR(("BTIPS_RS_ExtractRecordHandles (%d)", status));
				
				BTIPS_RS_PerformSearchPreExitOperations(0, 0, status, 1, 1);
			}
			
			break;
			
		}
		
		break;
		
		case SDEVENT_QUERY_ERR:
		case SDEVENT_QUERY_FAILED:
			BTL_ERR_NORET(("SDP Query Failed (%s)", pME_Event(event->eType)));
			
			BTIPS_RS_PerformSearchPreExitOperations(0, 0, BTIPS_RS_STATUS_QUERY_FAILURE, 1, 1);
			
			break;
			
		default:
			BTL_FATAL_NORET(("Unexpecter event type received for query (%s)", pME_Event(event->eType)));
			
			BTIPS_RS_PerformSearchPreExitOperations(0, 0, BTIPS_RS_STATUS_INTERNAL, 1, 1);
			
			break;
			
	}
}

/*
 * callback function to handle the returned attributes after the SDP_Query
 * In case of an error, the error will be passed to the upper layer.
 * In case of a legal response - BTIPS_RS_ExtractRecordsAttributes is called to parse the result and build the result buffer. 
 * In case not all data received (continuation state) SDP_Query will be called again to retrieve the rest. 
 * When all data was received BTIPS_RS_STATUS_OPERATION_END will be sent to upper layer with the result attributes.
 */

static void BTIPS_RS_ServiceAttrEventHandler(const BtEvent* event)
{
	BTIPS_RS_Status 	status = BTIPS_RS_STATUS_OPERATION_END; /* if operation cancelled don't continue */
	BtStatus			btStatus;
	
	BTL_FUNC_START("BTIPS_RS_ServiceAttrEventHandler");
	
	BTL_LOG_DEBUG(("event is %s\n", pME_Event(event->eType)));
	
	switch(event->eType) {
		
	case SDEVENT_QUERY_RSP:
		if(g_btips_rs.g_operationCanceled != TRUE)
		{
			status = BTIPS_RS_ExtractRecordsAttributes(event->p.token);
			BTL_LOG_DEBUG(("BTIPS_RS_ExtractRecordsAttributes: status is %d\n", status));
		}
		switch (status)
		{			
		case BTIPS_RS_STATUS_SDP_CONT_STATE:
			
			/* continuation state */
			
			BTL_LOG_DEBUG(("Continuation state. call SDP_Query again\n"));
			
			btStatus = SDP_Query(&g_btips_rs.g_queryToken, BSQM_CONTINUE);
			
			if (btStatus != BT_STATUS_PENDING)
			{
				BTL_FATAL_NORET(("SDP_Query Failed (%s)", pBT_Status(btStatus)));
				BTIPS_RS_PerformAttrPreExitOperations(0, BTIPS_RS_STATUS_QUERY_FAILURE, 1, 1);
			}
			
			break;

		case BTIPS_RS_STATUS_QUERY_EMPTY:
			BTIPS_RS_PerformAttrPreExitOperations(0, 
																						status, 
																						1, 
																						1);
			
			break;
			
		case BTIPS_RS_STATUS_OPERATION_END:
			BTIPS_RS_PerformAttrPreExitOperations(g_btips_rs.g_params.g_serviceAttrParams->buff, 
																						status, 
																						1, 
																						1);
			
			break;
			
		default:
			{
				/* An error was returned. status other then SUCCESS or PENDING*/
				
				BTL_LOG_ERROR(("BTIPS_RS_ExtractRecordsAttributes (%d)", status));
				
				BTIPS_RS_PerformAttrPreExitOperations(0, status, 1, 1);
			}
			
			break;
			
		}
		
		break;
		
		case SDEVENT_QUERY_ERR:
		case SDEVENT_QUERY_FAILED:
			BTL_ERR_NORET(("SDP Query Failed (%s)", pME_Event(event->eType)));
			
			BTIPS_RS_PerformAttrPreExitOperations(0, BTIPS_RS_STATUS_QUERY_FAILURE, 1, 1);
			
			break;
			
		default:
			BTL_FATAL_NORET(("Unexpecter event type received for query (%s)", pME_Event(event->eType)));
			
			BTIPS_RS_PerformAttrPreExitOperations(0, BTIPS_RS_STATUS_INTERNAL, 1, 1);
			
			break;
			
	}
}


/*
 * The function is called after SDP_Query response event.
 * It fetches the attributes and writes them to the buffer.
 * The attribute list must be sorted. 
 * It performs 2 loops:
 * 1) Loop on services
 * 2) Inner loop for each service, it fetches the attributes
 * If all attributes were found for a service, the service record
 * is considered valid and will be indicated in the result buff
 * In case an attribute was not found for a service, the service will be 
 * ignored.
 */
static BTIPS_RS_Status BTIPS_RS_ExtractAttributesAndParse(SdpQueryToken * token)
{
	BtStatus status = XA_STATUS_FAILED;
	BTIPS_RS_Status	rsStatus;
	
	/* check if the query response is not empty */
	if (token->mode == BSPM_BEGINNING) {
		U16 byte_count = SDP_GetU16(token->results);
		if (byte_count <= 3) {
			BTL_LOG_DEBUG(("Query returned with no data"));
			g_btips_rs.g_serviceTotal = 0;
			return BTIPS_RS_STATUS_QUERY_EMPTY;
		}
	}
	
	do {
		/* reached end of SDP results */
		if (token->mode==BSPM_RESUME && token->remainLen <= 1)
			break;
		
		/* Initialize the result buffer */
		if (BTIPS_RS_InitBuff(g_btips_rs.g_params.g_serviceSearchAttrParams->buff, g_btips_rs.g_params.g_serviceSearchAttrParams->len, g_btips_rs.g_sortedAttributeIds, g_btips_rs.g_params.g_serviceSearchAttrParams->numOfAttributes) == 0) {
			BTL_LOG_DEBUG(("Buffer has no minimal size for data"));
			return BTIPS_RS_STATUS_INTERNAL;
		}
		
		/* loop to retrieve all attributes */
		for (; g_btips_rs.g_attrIndex < g_btips_rs.g_params.g_serviceSearchAttrParams->numOfAttributes; ++(g_btips_rs.g_attrIndex)) {
			token->attribId = g_btips_rs.g_sortedAttributeIds[g_btips_rs.g_attrIndex];
			
			/* in case UUID is not 0, it is searched inside the attribute value */
			token->uuid = 0;
			
			/* parse the retuned results from the query */
			status = SDP_ParseAttributes(token);
			
			/* check for error. If so ignore the service record */
			if (status!=BT_STATUS_SUCCESS && status!=BT_STATUS_SDP_CONT_STATE) {
			/* Attribute was not found.
				* continue to the next attribute */
				continue;
			}
			
			/* save the attribute value in the buff */
			rsStatus = BTIPS_RS_InsertServiceAttributeData(	g_btips_rs.g_sortedAttributeIds[g_btips_rs.g_attrIndex],
				token->valueBuff,
				token->availValueLen,
				g_btips_rs.g_params.g_serviceSearchAttrParams->buff,
				g_btips_rs.g_params.g_serviceSearchAttrParams->len,
				1);
			if (rsStatus == BTIPS_RS_STATUS_BUFFER_OVERFLOW) {
				return BTIPS_RS_STATUS_BUFFER_OVERFLOW;
			}
			else if (rsStatus == BTIPS_RS_STATUS_ATTIRBUTE_ID_NO_FOUND)
			{
				/* Attribute wasn't found, continue to next one */
				continue;
			}
			
			if (status == BT_STATUS_SUCCESS) 
			{
				token->mode = BSPM_RESUME;
				BTIPS_RS_SET_ATTRIBUTE_FOUND_NUM(g_btips_rs.g_params.g_serviceSearchAttrParams->buff, 
					(U16)(BTIPS_RS_GET_ATTRIBUTE_FOUND_NUM(g_btips_rs.g_params.g_serviceSearchAttrParams->buff) + 1));
			}
			else { /* BT_STATUS_SDP_CONT_STATE */
				token->mode = BSPM_CONT_STATE;
				return BTIPS_RS_STATUS_SDP_CONT_STATE;
			}
		}

		if(BTIPS_RS_GET_ATTRIBUTE_FOUND_NUM(g_btips_rs.g_params.g_serviceSearchAttrParams->buff) == 0)
		{
			/* no attribute was found - report an error.*/
			return BTIPS_RS_STATUS_QUERY_FAILURE;
		}
		
		/* reset all fields */
		g_btips_rs.g_attrIndex = 0;
		
		/* Set the SDP parsing mode */
		token->mode = BSPM_RESUME;
		
		/* Call the callback with the found record.*/
		g_btips_rs.g_params.g_serviceSearchAttrParams->callbackFunc(g_btips_rs.g_params.g_serviceSearchAttrParams->buff,
														BTIPS_RS_STATUS_OK);

		++(g_btips_rs.g_serviceTotal);
		
	} while (g_btips_rs.g_serviceTotal < g_btips_rs.g_params.g_serviceSearchAttrParams->maxServiceRecords) ;
	
	/* reset query mode */
	token->mode = BSPM_BEGINNING;
		
	/* Successful parsing */
	return BTIPS_RS_STATUS_OPERATION_END;
}

/*
 * The function is called after SDP_Query response event for service search attributes query.
 * It parses the response to single records and calls the callback with BTIPS_RS_STATUS_OK for each record.
 *
 * Algorithm:
 * there are two main parameters that are tracked and updated in this algorithm:
 *	1. 	g_btips_rs.g_headerMissingBytes - indicates the number of bytes missing from a header in case a record header 
 *			was split between two responses.
 *	2.	g_btips_rs.g_remainingLen - indicates the number of bytes missing from a record data in case a record 
 *			was split between two responses.
 *
 * this function can be called in three scenarios:
 *	1.  new header: in this case we parse a new record received. This may happed either when we parse the
 *			begining of the SDP response or when this is a continuation of a response and the last response ended after a whole
 *			record. (g_btips_rs.g_headerMissingBytes = 0, g_btips_rs.g_remainingLen = 0)
 *	2.	continuation when the privios response ended in the middle of record header
 *			(g_btips_rs.g_headerMissingBytes != 0, g_btips_rs.g_remainingLen = 0)
 *	3. 	continuation when the privios response ended in the middle of record data
 *			(g_btips_rs.g_headerMissingBytes == 0, g_btips_rs.g_remainingLen != 0)
 *
 *	Algorithm flow:
 *	loop untill reached maximum service records:
 *	{
 *	- if g_btips_rs.g_headerMissingBytes != 0		(we are in the middle of a header when the begining of the record were received in previos response and were copied to a special header buffer)
 *			- fatch the rest of the header to header buff 
 *			- copy header buff to result buff 
 *			- update g_btips_rs.g_remainingLen to be record length
 *	- if g_btips_rs.g_remainingLen = 0 					(new record)
 *			- if all header bytes received
 *				- fetch records length
 *				- if all record's data bytes received
 *					- copy records header + data
 *				- else 
 *					- copy header + received data bytes
 *					- update g_btips_rs.g_remainingLen to indicate the number of bytes missing for this recored to be fetched in the next response
 *			- else 
 *				- copy partial header received bytes to special header buff
 *	- if g_btips_rs.g_remainingLen != 0 					(middle of record's data record)
 *			- if all record's remaining data bytes received
 *				- copy rest of the record
 *			- else
 *				- copy received data bytes
 *				- update g_btips_rs.g_remainingLen to indicate the number of bytes missing for this recored to be fetched in the next response
 *
 *	- if g_btips_rs.g_remainingLen = 0 & g_btips_rs.g_headerMissingBytes = 0	(we finised coping a record)
 *		- call calback with the fetched record
 *		- update number of services found
 *	- else					(continuation state)															
 *		- handle continuation state (update token fields)
 *		- return continuation state
 *
 *	- if we are in the end of the received data (either end of data or response ended after a whole record)
 *		- if there is continuation state - 
 *			- handle continuation state (update token fields)
 *			- return continuation state
 *	}
 *	return operation end
 */
 static BTIPS_RS_Status BTIPS_RS_ExtractAttributes(SdpQueryToken * token)
 {
	 BTIPS_RS_Status status = BTIPS_RS_STATUS_QUERY_FAILURE;
	 U8 *tmpPos = token->results; /* possition on the buffer received in the token.*/
	 U16 offset;
	 U16 byteCount;
	 U32 len;
	 U32 copySize;
	 BOOL headerParsed;
	 U16 reminder;
	 U8 i;
	 U8* endData;
	 
	 BTL_LOG_DEBUG(("BTIPS_RS_ExtractAttributes"));
	 
	 byteCount = SDP_GetU16(tmpPos);
	 
	 /* check if the query response is not empty */
	 if (token->mode == BSPM_BEGINNING) {
		 if (byteCount <= 3) {
			 BTL_LOG_DEBUG(("Query returned with no data"));
			 g_btips_rs.g_serviceTotal = 0;
			 return BTIPS_RS_STATUS_QUERY_EMPTY;
		 }
	 }
	 
	 tmpPos += 2;

	 endData = token->results + (byteCount + 2);
	 
	 if (token->mode == BSPM_BEGINNING)
	 {
		 g_btips_rs.g_curPos = g_btips_rs.g_params.g_serviceSearchAttrParams->buff;
		 
		 len = SDP_ParseDataElement((tmpPos), &offset);
		 
		 tmpPos += offset;	 
	 }
	 
	 do 
	 {
		 if(0 != g_btips_rs.g_headerMissingBytes)
		 {
		 	/* this is a continuation response while in the privios response packet a header was split.*/
			 for(i=0; i< g_btips_rs.g_headerMissingBytes; i++)
			 {
				 g_btips_rs.g_headerBuf[g_btips_rs.g_headerMissing_offset + i] = tmpPos[i];
			 }
			 len = (U16)SDP_ParseDataElement(g_btips_rs.g_headerBuf, &offset);
			 
			 /* Copy the header.*/
			 OS_MemCopy(g_btips_rs.g_curPos, 
				 g_btips_rs.g_headerBuf,
				 g_btips_rs.g_headerMissingBytes + g_btips_rs.g_headerMissing_offset);
			 			 
			 tmpPos+= g_btips_rs.g_headerMissingBytes;
			 
			 g_btips_rs.g_curPos += g_btips_rs.g_headerMissingBytes + g_btips_rs.g_headerMissing_offset;
			 
			 g_btips_rs.g_headerMissingBytes = 0;
			 
			 g_btips_rs.g_headerMissing_offset = 0;
			 
			 g_btips_rs.g_remainingLen = len;				
		 }
		 if(0 == g_btips_rs.g_remainingLen)
		 {
		 	/* new header.*/
			 headerParsed = BTIPS_RS_ParseHeader(tmpPos, (U16)(endData - tmpPos));
			 
			 if (TRUE == headerParsed)
			 {
				 len = (U16)SDP_ParseDataElement(tmpPos, &offset);
				 
				 if(tmpPos + offset + len > endData)
				 {
					 g_btips_rs.g_remainingLen = (tmpPos + offset + len) - endData;
					 copySize = len + offset - g_btips_rs.g_remainingLen;
				 }
				 else
				 {
					 copySize = len + offset;
				 }
			 }
			 else
			 {
				 copySize = 0;
				 tmpPos += g_btips_rs.g_headerMissing_offset;
			 }
		 }
		 else
		 {
		 	/* we have a partial record - get the rest.*/
			 if(tmpPos + g_btips_rs.g_remainingLen > endData)
			 {
				 reminder = (U16)((tmpPos + g_btips_rs.g_remainingLen) - endData);
				 
				 copySize = g_btips_rs.g_remainingLen - reminder;
				 
				 g_btips_rs.g_remainingLen = reminder;
				 
			 }
			 else
			 {
				 copySize = g_btips_rs.g_remainingLen;
				 g_btips_rs.g_remainingLen = 0;
			 }
		 }
		 
		 if(copySize > 0)
		 {
			 OS_MemCopy(g_btips_rs.g_curPos, tmpPos, copySize);
			 			 
			 tmpPos += copySize;
			 g_btips_rs.g_curPos += copySize;
		 }
		 
		 if((0 == g_btips_rs.g_remainingLen) && (g_btips_rs.g_headerMissingBytes == 0))
		 {
			 /* we have a full record. Call the callback with the found record.*/
			 g_btips_rs.g_params.g_serviceSearchAttrParams->callbackFunc(g_btips_rs.g_params.g_serviceSearchAttrParams->buff, BTIPS_RS_STATUS_OK);
			 
			 /* we reuse the buffer in next record. set g_btips_rs.g_curPos to its beginning */
			 g_btips_rs.g_curPos = g_btips_rs.g_params.g_serviceSearchAttrParams->buff; 
			 
			 g_btips_rs.g_serviceTotal++;
		 }
		 else
		 {
			 /* continuation state - for the rest of the record*/
			 token->mode = BSPM_CONT_STATE;
			 token->contState = tmpPos;
			 token->contStateLen = (U8)((*token->contState) + 1);
			 if(token->results + token->rLen - tmpPos != token->contStateLen)
			 {
				 status = BTIPS_RS_STATUS_QUERY_FAILURE;
			 }
			 else if (token->contStateLen > 1) 
			 {
				 status = BTIPS_RS_STATUS_SDP_CONT_STATE;
			 } 
			 else 
			 {
				 /* Clear out the missing or invalid contination values */
				 token->contState = 0;
				 token->contStateLen = 0;
				 status = BTIPS_RS_STATUS_QUERY_FAILURE;
			 }
			 
			 return status;
		 }
		 
		 if (tmpPos == endData)
		 {
			 /* End of the buffer.*/
			 if(*tmpPos == 0)
			 {
				 /* reset query mode */
				 token->mode = BSPM_BEGINNING;
				 
				 /* Successful parsing */
				 return BTIPS_RS_STATUS_OPERATION_END;
			 }
			 else
			 {
				 /* continuation state - for the next record*/
				 token->mode = BSPM_CONT_STATE;
				 token->contState = tmpPos;
				 token->contStateLen = (U8)((*token->contState) + 1);
				 if(endData - tmpPos != token->contStateLen)
				 {
					 status = BTIPS_RS_STATUS_QUERY_FAILURE;
				 }
				 else if (token->contStateLen > 1) 
				 {
					 status = BTIPS_RS_STATUS_SDP_CONT_STATE;
				 } 
				 else 
				 {
					 /* Clear out the missing or invalid contination values */
					 token->contState = 0;
					 token->contStateLen = 0;
					 status = BTIPS_RS_STATUS_OPERATION_END;
				 }
				 
				 return status;
			 }
		 }
		}while(g_btips_rs.g_serviceTotal < g_btips_rs.g_params.g_serviceSearchAttrParams->maxServiceRecords);
		
		/* reset query mode */
		token->mode = BSPM_BEGINNING;
		
		/* Successful parsing */
		return BTIPS_RS_STATUS_OPERATION_END;
}

 /*
 * The function is called after SDP_Query response event for service attributes query.
 * It parses the response and builds a buffer with all attributes returened for the given record.
 * When this buffer when done - is returned later in the callback with BTIPS_RS_STATUS_OPERATION_END status
 *
 * Algorithm:
 * there is one main parameter that is tracked and updated in this algorithm:
 * g_btips_rs.g_remainingLen - indicates the number of bytes missing from a record data in case a record 
 *														 was split between two responses.
 *
 * this function can be called in two scenarios:
 *	1.  new header: in this case we parse a new record received. This may happed either when we parse the
 *			begining of the SDP response or when this is a continuation of a response and the last response ended after a whole
 *			record. (g_btips_rs.g_remainingLen = 0)
 *	3. 	continuation when the privios response ended in the middle of record data
 *			(g_btips_rs.g_remainingLen != 0)
 *
 *	Algorithm flow:
 *	- if g_btips_rs.g_remainingLen = 0 					(new record)
 *			- fetch records length
 *			- if all record's data bytes received
 *					- copy records header + data
 *			- else 
 *				- copy header + received data bytes
 *				- update g_btips_rs.g_remainingLen to indicate the number of bytes missing for this recored to be fetched in the next response
 *	- if g_btips_rs.g_remainingLen != 0 					(middle of record's data record)
 *			- if all record's remaining data bytes received
 *				- copy rest of the record
 *			- else
 *				- copy received data bytes
 *				- update g_btips_rs.g_remainingLen to indicate the number of bytes missing for this recored to be fetched in the next response
 *
 *	- if g_btips_rs.g_remainingLen = 0 	(we finised coping a record)
 *		- return operation end
 *	- else					(continuation state)															
 *		- handle continuation state (update token fields)
 *		- return continuation state
 *
 */

static BTIPS_RS_Status BTIPS_RS_ExtractRecordsAttributes(SdpQueryToken * token)
{
	BTIPS_RS_Status status = BTIPS_RS_STATUS_QUERY_FAILURE;
	U8 *tmpPos = token->results; /* possition on the buffer received in the token.*/
	U8* endData;
	U16 offset;
	U16 byteCount;
	U32 len;
	U32 copySize;
	U16 reminder;
	
	BTL_LOG_DEBUG(("BTIPS_RS_ExtractRecordsAttributes"));
	
	byteCount = SDP_GetU16(tmpPos);
	
	/* check if the query response is not empty */
	if (token->mode == BSPM_BEGINNING) 
	{
		if (byteCount <= 3) 
		{
			BTL_LOG_DEBUG(("Query returned with no data"));
			return BTIPS_RS_STATUS_QUERY_EMPTY;
		}
		
		g_btips_rs.g_curPos = g_btips_rs.g_params.g_serviceAttrParams->buff;
		
	}
	 	 
	tmpPos += 2;	

	endData = token->results + (byteCount + 2);
	
	if(0 == g_btips_rs.g_remainingLen)
	{
		/* record header.*/
		len = (U16)SDP_ParseDataElement(tmpPos, &offset);
		
		if(tmpPos + offset + len > endData)
		{
			g_btips_rs.g_remainingLen = (tmpPos + offset + len) -  endData;
			copySize = len + offset - g_btips_rs.g_remainingLen;
		}
		else
		{
			copySize = len + offset;
		}
	}
	else
	{
		/* we have a partial record - get the rest.*/
		if(tmpPos + g_btips_rs.g_remainingLen > endData)
		{
			reminder = (U16)((tmpPos + g_btips_rs.g_remainingLen) -  endData);
			
			copySize = g_btips_rs.g_remainingLen - reminder;
			
			g_btips_rs.g_remainingLen = reminder;
			
		}
		else
		{
			copySize = g_btips_rs.g_remainingLen;
			g_btips_rs.g_remainingLen = 0;
		}
	}
	
	if(copySize > 0)
	{
		OS_MemCopy(g_btips_rs.g_curPos, tmpPos, copySize);
				
		tmpPos += copySize;
		g_btips_rs.g_curPos += copySize;
	}
	
	if(0 == g_btips_rs.g_remainingLen)
	{
		/* reset query mode */
		token->mode = BSPM_BEGINNING;
		
		status = BTIPS_RS_STATUS_OPERATION_END;
	}
	else
	{
		/* continuation state - for the rest of the record*/
		token->mode = BSPM_CONT_STATE;
		token->contState = tmpPos;
		token->contStateLen = (U8)((*token->contState) + 1);
		if(token->results + token->rLen - tmpPos != token->contStateLen)
		{
			status = BTIPS_RS_STATUS_QUERY_FAILURE;
		} 
		else if (token->contStateLen > 1) 
		{
			status = BTIPS_RS_STATUS_SDP_CONT_STATE;
		} 
		else 
		{
			/* Clear out the missing or invalid contination values */
			token->contState = 0;
			token->contStateLen = 0;
			status = BTIPS_RS_STATUS_QUERY_FAILURE;
		}
	}
	
	return status;
}


/*
 * The function is called after SDP_Query response event for service search query.
 * It parses the response and builds a buffer with all record handles returened for the given record.
 * When this buffer when done - is returned later in the callback with BTIPS_RS_STATUS_OPERATION_END status
 * the function copy all received record handles to the result buffer. when done BTIPS_RS_STATUS_OPERATION_END is returned
 * in case not all handles were received BTIPS_RS_STATUS_SDP_CONT_STATE will be returned in order to call SDP_Query again
 */
static BTIPS_RS_Status BTIPS_RS_ExtractRecordHandles(SdpQueryToken * token)
{
	U8 *tmpPos = token->results; /* possition on the buffer received in the token.*/
	U8 i;
	U16 currentServiceRecordCount;
	
	BTL_LOG_DEBUG(("BTIPS_RS_ExtractRecordHandles"));
	
	/* check if the query response is not empty */
	if (token->mode == BSPM_BEGINNING) 
	{
		g_btips_rs.g_curPos = g_btips_rs.g_params.g_serviceSearchParams->buff;
	}
	
	g_btips_rs.g_serviceTotal = SDP_GetU16(tmpPos);
	
	tmpPos += sizeof(U16);
	
	if(g_btips_rs.g_serviceTotal > g_btips_rs.g_params.g_serviceSearchParams->maxServiceRecords)
	{
		g_btips_rs.g_serviceTotal = g_btips_rs.g_params.g_serviceSearchParams->maxServiceRecords;
	}
	
	currentServiceRecordCount = SDP_GetU16(tmpPos);
	
	tmpPos += sizeof(U16);
	
	if((currentServiceRecordCount == 0) || (g_btips_rs.g_serviceTotal == 0))
	{
		BTL_LOG_DEBUG(("Query returned with no data"));
		return BTIPS_RS_STATUS_QUERY_EMPTY;
	}
	
	if(g_btips_rs.g_servicesArrived + currentServiceRecordCount > g_btips_rs.g_params.g_serviceSearchParams->maxServiceRecords)
	{
		currentServiceRecordCount = (U16)(g_btips_rs.g_params.g_serviceSearchParams->maxServiceRecords - g_btips_rs.g_servicesArrived);
	}
	
	g_btips_rs.g_servicesArrived = (U16)(g_btips_rs.g_servicesArrived + currentServiceRecordCount);
	
	for(i=0; i<currentServiceRecordCount; i++)
	{
		((U32*)g_btips_rs.g_curPos)[0] = SDP_GetU32(tmpPos);
		tmpPos += sizeof(U32);
		g_btips_rs.g_curPos += sizeof(U32);
	}
	
	if(g_btips_rs.g_servicesArrived == g_btips_rs.g_serviceTotal)
	{
		token->mode = BSPM_BEGINNING;
		
		return BTIPS_RS_STATUS_OPERATION_END;
	}

	/* continuation state.*/
	token->mode = BSPM_CONT_STATE;
	token->contState = tmpPos;
	token->contStateLen = (U8)((*token->contState) + 1);
	if(token->results + token->rLen - tmpPos != token->contStateLen)
	{
	 	return BTIPS_RS_STATUS_QUERY_FAILURE;
	}
	if (token->contStateLen > 1) 
	{
		return BTIPS_RS_STATUS_SDP_CONT_STATE;
	} 
	else 
	{
		 /* Clear out the missing or invalid contination values */
		 token->contState = 0;
		 token->contStateLen = 0;
		 return BTIPS_RS_STATUS_QUERY_FAILURE;
	}
}

/*
 * Parses the given header (located at the begining of buff and:
 * - if all header bytes exists (depends on len) returns TRUE
 * - else: 1. copy the partial header bytes to g_btips_rs.g_headerBuf buffer
 *				 2. update g_btips_rs.g_headerMissingBytes to indicate how many bytes are missing for the header
 *				 3. update g_btips_rs.g_headerMissing_offset to indicate the number of bytes existes for the header
 *				 4. returns FALSE.
 */
BOOL BTIPS_RS_ParseHeader(U8 *buff, U16 len)
{
	U8 addLen = 0;
	BOOL answer = TRUE;
	U8 *tmp = buff;
	U8 header = *tmp;
	U8 i;
	
	switch(header & DESD_MASK)
	{
    case DESD_1BYTE:
    case DESD_2BYTES:
    case DESD_4BYTES:
    case DESD_8BYTES:
    case DESD_16BYTES:
        return TRUE;
		
    case DESD_ADD_8BITS:
		addLen = 1;
		answer = (len >= 2);
        break;
		
    case DESD_ADD_16BITS:
		addLen = 2;
		answer = (len >= 3);
        break;
		
    case DESD_ADD_32BITS:
		addLen = 4;
		answer = (len >= 5);
        break;
		
    default:
        Assert(0);
        break;
    }
	
	if(FALSE == answer)
	{
		g_btips_rs.g_headerMissingBytes = (U8)((addLen+1) - len);
		
		g_btips_rs.g_headerMissing_offset = 0;
		
		for(i=0; i<len; i++)
		{
			g_btips_rs.g_headerBuf[i] = *tmp;
			tmp++;
			g_btips_rs.g_headerMissing_offset++; 
		}
	}
	return answer;
	
}

/*
 * Used for testing only
 */
void BTIPS_RS_PrintAttributesData(U8 * buff)
{
	BTIPS_RS_PrintAttributesDataInternal(buff);
}

