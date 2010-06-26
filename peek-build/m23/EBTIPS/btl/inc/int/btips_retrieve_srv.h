/*******************************************************************************\
*
*   FILE NAME:      btips_retrieve_srv.h
*
*   DESCRIPTION:    This file specifies the function prototypes for general
*				  retrieving of attributes from SDP server.
*
*   AUTHOR:         Yuval Hevrony
*
\*******************************************************************************/

#ifndef __BTIPS_RETRIEVE_SRV_H
#define __BTIPS_RETRIEVE_SRV_H

#include "sdp.h"
#include "btl_config.h"

/*
   The maximum number of UUIDs that can be given in a SDP Query,
   as defined in Bluetooth SIG spec.
*/
#define BTIPS_RS_MAX_UUIDS      12

#define UUID_MAX_SIZE		16

typedef struct _BtlBmgUuid {
    U8    uuid[UUID_MAX_SIZE];
} BtlBmgUuid;

/*
 * BTIPS_RS_GetNumberOfAttributesFound returns the number of attribute IDs found in the service record.
 */	
U16  BTIPS_RS_GetNumberOfAttributesFound(U8 *buff);

/*
 * BTIPS_RS_Status is used to describe the status of the query results.
*/   
typedef enum BTIPS_RS_Status {
	BTIPS_RS_STATUS_OK = 0,                      /* the operation was succesful and there are more records */
	BTIPS_RS_STATUS_QUERY_FAILURE,       /* operation failure */
	BTIPS_RS_STATUS_QUERY_EMPTY,          /* there was no data in the query */
	BTIPS_RS_STATUS_BUFFER_OVERFLOW,  /* The buffer that was passed from the application is too small */
	BTIPS_RS_STATUS_CONNECT_FAILURE,  /* Error while tring to connect remote device */	
	BTIPS_RS_STATUS_INTERNAL,                  /* An internal error */
	BTIPS_RS_STATUS_INVALID_INPUT,         /* An invalid input */
	BTIPS_RS_STATUS_QUERY_IN_PROGRESS,  /* query is in progress */
	BTIPS_RS_STATUS_DEVICE_NOT_REGISTERED, /* BT device not registered */
	BTIPS_RS_STATUS_SDP_CONT_STATE, /* SDP continuation state */
	BTIPS_RS_STATUS_PENDING,
	BTIPS_RS_STATUS_OPERATION_END,		/* End of opperation, no more records available.*/
	BTIPS_RS_STATUS_OPERATION_CANCELLED,   	/* The opperation was cancelled */
	BTIPS_RS_STATUS_ATTIRBUTE_ID_NO_FOUND	/* Attribute Id wasn't found in the response buffer */
} BTIPS_RS_Status;

/*
 * BTIPS_RS_ATTR_Callback
 *
 * Desc: 
 *       The callback function that should be defined in the application. 
 *       The function will notify the application about the results.
 *
 * Parameters:
 *       a) U8* buffer. This is the results buffer.
 *       b) BTIPS_RS_Statuses :
 *				BTIPS_RS_STATUS_OK
 *				BTIPS_RS_STATUS_QUERY_FAILURE
 *				BTIPS_RS_STATUS_QUERY_EMPTY
 *				BTIPS_RS_STATUS_BUFFER_OVERFLOW
 *				BTIPS_RS_STATUS_CONNECT_FAILURE
 *              BTIPS_RS_STATUS_INTERNAL
 *				BTIPS_RS_STATUS_PENDING
 *
 * Return value:
 *       void.		
 */
typedef void (*BTIPS_RS_ATTR_Callback) (U8* result, BTIPS_RS_Status status);

typedef void (*BTIPS_RS_SERVICE_Callback) (U8* result, U16 handlesCount, BTIPS_RS_Status status);


/* struct to hold all data for the query operation. 
 * All fields are valid after callback 
 */
typedef struct 
{
	/* attribute list to fetch 
	 * The field will contaign all attributes IDs
	 * that we would like to fetch.
	 * It will also include attribute IDs that are
	 * mandatory in the service record
	 */
	SdpAttributeId attributeIds[BTL_CONFIG_BMG_MAX_NUM_OF_ATTRIBUTES_TO_SEARCH];

	U16 numOfAttributes;

	/* 
	 *  uuids array for service search.
	 *  may contaign maximal number of BTIPS_RS_MAX_UUIDS.
	 */
	BtlBmgUuid uuids[BTIPS_RS_MAX_UUIDS];

	U8				numOfUuids;

	/* maximal number of service records to retrieve after validation */
	U16 maxServiceRecords;

	/* application callback function */
	BTIPS_RS_ATTR_Callback callbackFunc;

	/* fetched data for application 
	 * must be writable. 
	 * will be filled by BTIPS_RS_ServiceSearchAttr */
	U8 *buff;

	/* buff size (suplied by the application) */
	U16 len;
} BTIPS_RS_SERVICE_SEARCH_ATTR_PARAMS;

/* struct to hold all data for the query operation. 
 * All fields are valid after callback 
 */
typedef struct 
{
	U32 recordHandle;
	
	/* attribute list to fetch 
	 * The field will contaign all attributes IDs
	 * that we would like to fetch.
	 * It will also include attribute IDs that are
	 * mandatory in the service record
	 */
	SdpAttributeId attributeIds[BTL_CONFIG_BMG_MAX_NUM_OF_ATTRIBUTES_TO_SEARCH];

	U16 numOfAttributes;

	/* application callback function */
	BTIPS_RS_ATTR_Callback callbackFunc;

	/* fetched data for application 
	 * must be writable. 
	 * will be filled by BTIPS_RS_ServiceSearchAttr */
	U8 *buff;

	/* buff size (suplied by the application) */
	U16 len;
} BTIPS_RS_SERVICE_ATTR_PARAMS;

typedef struct 
{
	/* 
	 *  uuids array for service search.
	 *  may contaign maximal number of BTIPS_RS_MAX_UUIDS.
	 */
	BtlBmgUuid uuids[BTIPS_RS_MAX_UUIDS];

	U8				numOfUuids;

	/* maximal number of service records to retrieve after validation */
	U16 maxServiceRecords;

	/* application callback function */
	BTIPS_RS_SERVICE_Callback callbackFunc;

	/* fetched data for application 
	 * must be writable. 
	 * will be filled by BTIPS_RS_ServiceSearchAttr */
	U8 *buff;

	/* buff size (suplied by the application) */
	U16 len;
} BTIPS_RS_SERVICE_SEARCH_PARAMS;



/*
 * BTIPS_RS_ServiceSearchAttr
 *
 * Desc: 
 *       The function fetches attribute values, using the 
 *       input parameters in the struct.
 *       The flow:
 *       1) Establish connection using ObexTpAddr*.
 *       2) Compose an SDP Query using the input parameter
 *       3) Perform the query
 *			 4) For every recors found, an event with the requested attributs will be sent to the callback or if parseNeeded = TRUE - a special buffer will be built.
 *			 5) when the callback returns - the buffer is filled again with the next record.
 *       
 *
 *       All attributes with attribute IDs as indicated in the sruct are
 *       retrieved from all the service records that contain the UUIDs.
 *			 Only service records that contains all the UUIDs are valid.
 *       If there are more service records then max number of services to
 *       be fetched, we discard the last ones.
 *
 * Parameters:
 *       a) BD_ADDR - holds the remote BT device's BD_ADDR.
 *          Indicates the SDP Server BD_ADDR.
 *       b) BTIPS_RS_SERVICE_SEARCH_ATTR_PARAMS - the data struct.
 *          Holds the parameters for the data fetching.
 *          - attributed - attribute IDs array.
 *          - uuids - uuids array.
 *          - maxServiceRecords - max number of service records to
 *                                retrieve data from.
 *          - callbackFunc - callback function to be called when we finished fetching.
 *          - buff - buffer to be filled with results.
 *          - len - the size of the supplied buff
 *				c) BOOL parseNeeded - when TRUE - the result buffer will be parsed to attribute id's and their values in the following order:
 *
  * |attribute count (2 ) |attribute found num (2 ) |attr1 data description (sizeof(BTIPS_RS_ATT_DESC))|...|attrK data description (sizeof(BTIPS_RS_ATT_DESC))|attr1 value|...|attrK value|
 *
 * attr data description:
 * |attr ID (2)|attr size (2)|attr data offset from service starting(2)|
 *
 * Return value:
 *       BT_STATUS_INTERNAL - there was an internal error
 *       BT_STATUS_PENDING.
 *
 * Generated events: 	on successful process for every record an event with BTIPS_RS_STATUS_OK will be sent with the record buffer
 * 										when done, BTIPS_RS_STATUS_OPERATION_END will be sent to indicate completion.
 *										in case of an error see BTIPS_RS_Status values.
 */
BTIPS_RS_Status BTIPS_RS_ServiceSearchAttr(BD_ADDR *bd_addr, BTIPS_RS_SERVICE_SEARCH_ATTR_PARAMS *att_params, BOOL parseNeeded);
/*
 * BTIPS_RS_CancelServiceSearchAttr
 *
 * Desc: cancel previosly reqested BTIPS_RS_ServiceSearchAttr .
 *
 *
 * Parameters:
 *
 * Return value:
 *		 BTIPS_RS_STATUS_OK - the ACL link was not created yet and was succesfuly cancelled
 *		 BTIPS_RS_STATUS_PENDING - there is an ACL link and an cancel event will be generated when cancelation completes
 *
 * Generated events:	if BTIPS_RS_STATUS_PENDING was returned then an event with BTIPS_RS_OPERATION_CANCELLED will be sent 
 *										 
 *										 
 */

BTIPS_RS_Status BTIPS_RS_CancelServiceSearchAttr();

/*
 * BTIPS_RS_ServiceSearch
 *
 * Desc: 
 *       The function fetches attribute values, using the 
 *       input parameters in the struct.
 *       The flow:
 *       1) Establish connection using ObexTpAddr*.
 *       2) Compose an SDP Query using the input parameter
 *       3) Perform the query
 *			 4) calles a callback function with the result handles.
 *
 *			 Only service records that contains all the UUIDs are valid.
 *       If there are more service records then max number of services to
 *       be fetched, we discard the last ones.
 *
 * Parameters:
 *       a) BD_ADDR - holds the remote BT device's BD_ADDR.
 *          Indicates the SDP Server BD_ADDR.
 *       b) BTIPS_RS_SERVICE_SEARCH_ATTR_PARAMS - the data struct.
 *          Holds the parameters for the data fetching.
 *          - uuids - uuids array.
 *					- numOfUuids - uuids array size
 *          - maxServiceRecords - max number of service records to
 *                                retrieve data from.
 *          - callbackFunc - callback function to be called when we finished fetching.
 *          - buff - buffer to be filled with results.
 *          - len - the size of the supplied buff
 *
 * Return value:
 *       BT_STATUS_INTERNAL - there was an internal error
 *       BT_STATUS_PENDING.
 *
 * Generated events: 	BTIPS_RS_STATUS_OPERATION_END will be sent to indicate a successful completion.
 *										in case of an error see BTIPS_RS_Status values.
 */
BTIPS_RS_Status BTIPS_RS_ServiceSearch(BD_ADDR *bd_addr, BTIPS_RS_SERVICE_SEARCH_PARAMS *params);
/*
 * BTIPS_RS_CancelServiceSearch
 *
 * Desc: cancel previosly reqested BTIPS_RS_ServiceSearch .
 *
 *
 * Parameters:
 *
 * Return value:
 *		 BTIPS_RS_STATUS_OK - the ACL link was not created yet and was succesfuly cancelled
 *		 BTIPS_RS_STATUS_PENDING - there is an ACL link and an cancel event will be generated when cancelation completes
 *
 * Generated events:	if BTIPS_RS_STATUS_PENDING was returned then an event with BTIPS_RS_OPERATION_CANCELLED will be sent 
 *										 
 *										 
 */


BTIPS_RS_Status BTIPS_RS_CancelServiceSearch();


/*
 * BTIPS_RS_ServiceAttr
 *
 * Desc: 
 *       The function fetches attribute values, using the 
 *       input parameters in the struct.
 *       The flow:
 *       1) Establish connection using ObexTpAddr*.
 *       2) Compose an SDP Query using the input parameter
 *       3) Perform the query
 *			 4) calles a callback function with the result record attributes.
 *       
 *       All attributes with attribute IDs as indicated in the sruct are
 *       retrieved from the service record that corresponds with the given handle.
 *
 * Parameters:
 *       a) BD_ADDR - holds the remote BT device's BD_ADDR.
 *          Indicates the SDP Server BD_ADDR.
 *       b) BTIPS_RS_SERVICE_SEARCH_ATTR_PARAMS - the data struct.
 *          Holds the parameters for the data fetching.
 *					- recordHandle - handle to retrieve attributes for
 *          - attributIds - attribute IDs array.
 *					- numOfAttributes - attributIds length
 *          - callbackFunc - callback function to be called when we finished fetching.
 *          - buff - buffer to be filled with results.
 *          - len - the size of the supplied buff
 *
 * Return value:
 *       BT_STATUS_INTERNAL - there was an internal error
 *       BT_STATUS_PENDING.
 *
 * Generated events: 	BTIPS_RS_STATUS_OPERATION_END will be sent to indicate a successful completion.
 *										in case of an error see BTIPS_RS_Status values.
 */
BTIPS_RS_Status BTIPS_RS_ServiceAttr(BD_ADDR *bd_addr, BTIPS_RS_SERVICE_ATTR_PARAMS *params);

/*
 * BTIPS_RS_CancelServiceAttr
 *
 * Desc: cancel previosly reqested BTIPS_RS_ServiceAttr .
 *
 *
 * Parameters:
 *
 * Return value:
 *		 BTIPS_RS_STATUS_OK - the ACL link was not created yet and was succesfuly cancelled
 *		 BTIPS_RS_STATUS_PENDING - there is an ACL link and an cancel event will be generated when cancelation completes
 *
 * Generated events:	if BTIPS_RS_STATUS_PENDING was returned then an event with BTIPS_RS_OPERATION_CANCELLED will be sent 
 *										 
 *										 
 */


BTIPS_RS_Status BTIPS_RS_CancelServiceAttr();


/*
 * BTIPS_RS_GetAttributeData
 *
 * Desc:
 *       The function returns a pointer to a specified attribute data.
 *       The function is called by the application to fetch a specific attribute 
 *       from the buffer after it was filled.
 *
 * Parameters:
 *	   a) buff - The result buff that was supplied by the application in 
 *          BTIPS_RS_SERVICE_SEARCH_ATTR_PARAMS   
 *       b) attribute_id  - the attribute ID whose attribute value is returned.
 *       c)  size - the size of the attribute value returned (Output).
 *
 * Return value:
 *       pointer to the attribute data.
 *       0 if nothing was found (attribute id was not found in the buffer or service index to big).
 */
U8 * BTIPS_RS_GetAttributeData(U8 * buff, SdpAttributeId attribute_id, U16 *size);

/*
 * Used for testing only
 */
 void BTIPS_RS_PrintAttributesData(U8 * buff);

#endif /* __BTIPS_RETRIEVE_SRV_H */
