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
*   FILE NAME:      btl_bmg.c
*
*   DESCRIPTION:    This file implements the API of the BTL Bluetooth manager.
*
*   AUTHOR:         Udi Ron, Gili Friedman
*
\*******************************************************************************/

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bttypes.h" 
#include "btconfig.h"
#include "me.h"
#include "hci.h"
#include "debug.h"  
#include "btl_bmgi.h"
#include "btl_pool.h"
#include "btl_apphandle.h"
#include "btl_config.h"
#include "btl_defs.h"
#include "btl_bmg_evm.h"
#include "btl_bmg_request.h"
#include "btl_utils.h"
#include "btl_bmg_debug.h"
#include "btips_retrieve_srv_data.h"
#include "btalloc.h"
#include "btl_bmg_ddb.h"
#include "btrom.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BMG);

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

/* Max num of search responses of all possible search types */
#define	BTL_BMG_MAX_NUM_OF_FILTERED_RESPONSES										\
			max(	BTL_CONFIG_BMG_MAX_NUM_OF_RESPONSES_FOR_SEARCH_BY_COD,			\
					BTL_CONFIG_BMG_MAX_NUM_OF_DEVICES_FOR_SEARCH_BY_DEVICES)

/*
	Size of SDP Query results buffer
*/
#define BTL_BMG_MAX_SDP_QUERY_RESULT_BUF			1024

#define BTL_BMG_MAX_PIN_LEN							16

/* Size of service record including all its content (attributes + attributes value).*/
/* a record is saved in mempory as a buffer that holds the record, then the attributes and then the attributes value.*/
/* we save it that way in order to allocate our own memory resiurces for records addition and update.*/
/* record representation in memory: */
/*
__________________________________________________________________________________________________________
|                          |                       |				|		|			    |		         |                  |          |		    |
|      SdpRecord     | SdpAttribute 1 | SdpAttribute 2  |    ...    | SdpAttribute N  |attr value 1 |attr value 2 |  ...     |attr value N |
-----------------------------------------------------------------------------------------------------------
*/
#define BTL_BMG_SERVICE_RECORD_ACTUAL_SIZE	(sizeof(SdpRecord) + (BTL_CONFIG_BMG_MAX_ATTRIBUTES_PER_SERVICE_RECORD * sizeof(SdpAttribute) ))	

/* Union whose size will equal the size of the largest request type */
typedef union _BtlBmgRequestSizeUnion
{
	BtlBmgRequest			request;	
	BtlBmgDynamicRequest		dynamicRequest;
	BtlBmgCompoundRequest	compoundRequest;
} BtlBmgRequestSizeUnion;

#define BTL_BMG_MAX_REQUEST_SIZE	sizeof(BtlBmgRequestSizeUnion)

#define ATTR_ID_HEADER_SIZE	1

#define ATTR_ID_SIZE				2

#define BTL_BMG_MAX_NUM_OF_PAGING_ACTIVITIES	(BTL_BMG_MAX_NUM_OF_REQUESTS)


static const BtEventType setSecModeStaticEvents[] = {BTEVENT_SECURITY_CHANGE};

static const U32 numOfSetSecModeStaticEvents = sizeof(setSecModeStaticEvents) / sizeof(BtEventType);


static const BtEventType bondStaticEvents[] = {	BTEVENT_PIN_REQ,
											BTEVENT_PAIRING_COMPLETE,
											BTEVENT_AUTHENTICATED};

static const U32 numOfBondStaticEvents = sizeof(bondStaticEvents) / sizeof(BtEventType);

static const BtEventType inquiryStaticEvents[] = {	BTEVENT_INQUIRY_RESULT, 
											BTEVENT_INQUIRY_COMPLETE,
											BTEVENT_INQUIRY_CANCELED};
static const U32 numOfInquiryStaticEvents = sizeof(inquiryStaticEvents) / sizeof(BtEventType);

static const BtEventType btlBmgHciInitEvents[] = {
						BTEVENT_HCI_INITIALIZED,
						BTEVENT_HCI_INIT_ERROR,
						BTEVENT_HCI_DEINITIALIZED,
						BTEVENT_HCI_FAILED
					};

static const U32 btlBmgNumOfHciInitStaticEvents = sizeof(btlBmgHciInitEvents) / sizeof(BtEventType);

static const BtEventType setAccessibilityStaticEvents[] = {	BTEVENT_ACCESSIBLE_CHANGE};

static const U32 numOfSetAccessibilityStaticEvents = sizeof(setAccessibilityStaticEvents) / sizeof(BtEventType);


static const BtAccessModeInfo _btlDfltAccessibleModeInfoC = 
{
	BTL_CONFIG_BMG_DFLT_INQ_SCAN_INTERVAL_C, 
	BTL_CONFIG_BMG_DFLT_INQ_SCAN_WINDOW_C,
	BTL_CONFIG_BMG_DFLT_PAGE_SCAN_INTERVAL_C, 
	BTL_CONFIG_BMG_DFLT_PAGE_SCAN_WINDOW_C
};

static const BtAccessModeInfo _btlDfltAccessibleModeInfoNC = 
{
	BTL_CONFIG_BMG_DFLT_INQ_SCAN_INTERVAL_NC, 
	BTL_CONFIG_BMG_DFLT_INQ_SCAN_WINDOW_NC,
	BTL_CONFIG_BMG_DFLT_PAGE_SCAN_INTERVAL_NC, 
	BTL_CONFIG_BMG_DFLT_PAGE_SCAN_WINDOW_NC
};

#define _BTL_BMG_INVALID_SECURITY_MODE		(BSM_SEC_ENCRYPT + 1)

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*
	Type definition for the common search logic filtering function
*/
typedef BOOL (*BtlBmgSearchFilterFunc)(const BtEvent *event);

/*
	Initialization states of the BMG
*/
typedef enum _BtlBmgInitStateType
{
	BTL_BMG_INIT_STATE_NOT_INTIALIZED,
	BTL_BMG_INIT_STATE_INITIALIZED,
	BTL_BMG_INIT_STATE_INITIALIZATION_FAILED,
	BTL_BMG_INIT_STATE_DEINITIALIZATION_FAILED
} BtlBmgInitStateType;

/*
	Enumerator that contains values for the HCI commands that are used when enabling test mode
*/
typedef enum _BtlBmgEnableTestModeHciCommands
{
	BTL_BMG_ENABLE_TEST_MODE_HCI_ENABLE_TEST_MODE,
	BTL_BMG_ENABLE_TEST_MODE_HCI_SET_EVENT_FILTER,
	BTL_BMG_ENABLE_TEST_MODE_HCI_WRITE_SCAN_ENABLE,

	BTL_BMG_ENABLE_TEST_MODE_FIRST_HCI_COMMAND = BTL_BMG_ENABLE_TEST_MODE_HCI_ENABLE_TEST_MODE,
	BTL_BMG_ENABLE_TEST_MODE_LAST_HCI_COMMAND = BTL_BMG_ENABLE_TEST_MODE_HCI_WRITE_SCAN_ENABLE
} BtlBmgEnableTestModeHciCommands;

static BtlBmgInitStateType BtlBmgInitState = BTL_BMG_INIT_STATE_NOT_INTIALIZED;

typedef struct _BtlBmgLocalNameParms
{
	U8		localName[BTL_BMG_MAX_LOCAL_NAME_LEN];
	U8		localNameLen;
} BtlBmgLocalNameParms;

typedef struct _BtlBmgDiscoverServicesParams
{
	BTIPS_RS_SERVICE_SEARCH_ATTR_PARAMS 	queryParms;
	U32							currentServiceToDiscover;
	SdpServicesMask		servicesMask;
	SdpServicesMask		discoveredServices;
	BtlBmgRequest				*discoveryRequest;
	BOOL						discoveryCancelled;
} BtlBmgDiscoverServicesParams;

typedef struct _BtlBmgServiceSearchAttrParams
{
	BTIPS_RS_SERVICE_SEARCH_ATTR_PARAMS 	queryParms;
	BtlBmgRequest				*serviceSearchAttrRequest;
	BOOL						serviceSearchAttrCancelled;
} BtlBmgServiceSearchAttrParams;

typedef struct _BtlBmgServiceSearchParams
{
	BTIPS_RS_SERVICE_SEARCH_PARAMS 	queryParms;
	BtlBmgRequest				*serviceSearchRequest;
	BOOL						serviceSearchCancelled;
} BtlBmgServiceSearchParams;

typedef struct _BtlBmgServiceAttrParams
{
	BTIPS_RS_SERVICE_ATTR_PARAMS 	queryParms;
	BtlBmgRequest				*serviceAttrRequest;
	BOOL						serviceAttrCancelled;
} BtlBmgServiceAttrParams;


typedef struct _BtlBmgPagingActivityPointerEntry
{
	ListEntry node;
	BtlBmgRequest *pagingActivityRequestPointer;
} BtlBmgPagingActivityPointerEntry;


/*
	Structure that stores variables that are common to both search types
*/
typedef struct _BtlBmgSearchCommonData
{
	/* search method */
	BtlBmgSearchMethod		method;
	
	BOOL 					performNameRequest;

	/* The services to discover on filtere devices */
	SdpServicesMask 	sdpServicesMask;

	/* Num of inquiry results that passed the filter */
	U8						numOfFilteredResponses;

	/* BD addresses of devices that passed the filter */
	BD_ADDR				filteredDevicesAddresses[BTL_BMG_MAX_NUM_OF_FILTERED_RESPONSES];

	/* Num of name requests that have been initiated by the search, but haven't completed */
	U8						numOfPendingNameRequests;

	/* 
		Flags that indicate if there is a name request in progress the corresponding device
		If pendingNameRequestsFlags[i] == 1 => There is a name request in progress for
		filteredDevicesAddresses[i]
	*/
	U8						pendingNameRequestsFlags[BTL_BMG_MAX_NUM_OF_FILTERED_RESPONSES];

	/* Indicates if an inquiry is in progress */
	BOOL					inquiryInProgress;

	/* Indicates if service discovery completed or not (if not required => completed) */
	BOOL					servicesDiscoveryCompleted;
	
	BOOL					numOfDevicesThatCompletedServiceDiscovery;

	/* Records the error code that was set in the last event received by the search callback */
	BtErrorCode				lastErrorCode;

	/* Flag that indicates if a request to cancel the inquiry process was already initiated */
	BOOL					inquiryCancelRequested;

	/* Flag that indicates if the search was cancelled by the user */
	BOOL					searchCancellationInProgress;	
} BtlBmgSearchCommonData;

typedef struct _BtlBmgSearchByCodData
{
	BtlBmgSearchCommonData	commonData;

	/* max num of requested filtered responses */
	U8						maxResp;
	
	BtClassOfDevice			codFilter;
} BtlBmgSearchByCodData;

typedef struct _BtlBmgSearchByDevicesData
{
	BtlBmgSearchCommonData	commonData;

	/* List of BD Addrresses that are of interest to the caller */
	BD_ADDR				devicesList[BTL_CONFIG_BMG_MAX_NUM_OF_DEVICES_FOR_SEARCH_BY_DEVICES];
	
	/* length of BD addresses list */
	U8						listLength;
} BtlBmgSearchByDevicesData;

typedef struct _BtlBmgBondData
{
	U8 			pin[BTL_BMG_MAX_PIN_LEN];
	U8 			len; 
	BtPairingType	type;
	BOOL		waitingForPin;
	BOOL		completeBondWhenPeerConnects;
	BtErrorCode	bondCompletionCode;
	BOOL		linkConnectCnfEventReceived;
	BOOL		bondCancellationInProgress;
} BtlBmgBondData;

typedef enum
{
	_BTL_BMG_RADIO_OFF_EVENT_START,
	_BTL_BMG_RADIO_OFF_EVENT_PROCESS_COMPLETED,
	_BTL_BMG_RADIO_OFF_EVENT_REQUEST_CB_CALLED
} _BtlBmgRadioOffEventType;

typedef enum
{
	_BTL_BMG_RADIO_OFF_STATE_NONE,
	_BTL_BMG_RADIO_OFF_STATE_DISABLE_SCANS,
	_BTL_BMG_RADIO_OFF_STATE_WAITING_FOR_SCANS_DISABLING,
	_BTL_BMG_RADIO_OFF_STATE_CALLING_CANCEL_PROCESSES_IN_PROGRESS,
	_BTL_BMG_RADIO_OFF_STATE_CANCEL_PROCESSES_IN_PROGRESS,
	_BTL_BMG_RADIO_OFF_STATE_WAITING_FOR_PROCESSES_CANCELLATIONS,
	_BTL_BMG_RADIO_OFF_STATE_DONE
} _BtlBmgRadioOffStateType;

typedef enum
{
	_BTL_BMG_RADIO_ON_EVENT_START,
	_BTL_BMG_RADIO_ON_EVENT_SECURITY_MODE_SET,
	_BTL_BMG_RADIO_ON_EVENT_ACCESSIBILITY_MODES_SET
} _BtlBmgRadioOnEventType;

typedef enum
{
	_BTL_BMG_RADIO_ON_STATE_NONE,
	_BTL_BMG_RADIO_ON_STATE_SET_SECURITY_MODE,
	_BTL_BMG_RADIO_ON_STATE_SETTING_SECURITY_MODE,
	_BTL_BMG_RADIO_ON_STATE_SET_ACCESSIBILITY_MODES,
	_BTL_BMG_RADIO_ON_STATE_SETTING_ACCESSIBILITY_MODES,
	_BTL_BMG_RADIO_ON_STATE_DONE
} _BtlBmgRadioOnStateType;

typedef struct _BtlBmgRadioOnData
{
	_BtlBmgRadioOnStateType	state;
	BOOL					asynchronous;
	BtlBmgCompoundRequest	*radioOnRequest;
} BtlBmgRadioOnData;

typedef struct _BtlBmgRadioOffData
{
	_BtlBmgRadioOffStateType	state;
	BOOL					asynchronous;
	BtlBmgCompoundRequest	*radioOffRequest;
	U32						numOfProcessesWaitingCancellation;
	BtStatus					status;
} BtlBmgRadioOffData;

typedef struct _BtlBmgAccessibilityData
{
	BtAccessibleMode 			accessibleModeNC;
	BtAccessModeInfo			accessModeInfoNC;
	
	BtAccessibleMode 			accessibleModeC;
	BtAccessModeInfo			accessModeInfoC;

	BtlBmgRequest			*request;

	BOOL					pendingNCSettingInChip;
} BtlBmgAccessibilityData;

typedef struct _BtlBmgDataType
{
	/* auxiliary request that contains all top-level requests */
	BtlBmgCompoundRequest			masterPseudoRequest;

	/* List of BMG contexts */
	ListEntry							contextsList;

	/* Stores the current local device name information */
	BtlBmgLocalNameParms				localNameParms;

	/* 
		A placeholder for events that the BTL BMG generates on its own, rather that receiving them from
		ESI stack and passing them on
	*/
	BtEvent							selfGeneratedEvent;

	/* Pointer to the current search request (at most one simultaneosuly) */
	BtlBmgCompoundRequest			*searchRequest;

	/* Union for the current search data (at most one simultaneosly) */
	union
	{
		BtlBmgSearchByCodData		searchByCodData;
		BtlBmgSearchByDevicesData	searchByDevicesData;
	} searchData;

	ListEntry		pagingActivitiesPointersList;

	BtlBmgPagingActivityPointerEntry *pagingActivityInProgress;

	/* Indicates whether there is any SDP query in progress (at most once simultaneously) */
	BOOL							sdpQueryInProgress;

	U32 	sdpQueryResultsBuff[BTL_BMG_MAX_SDP_QUERY_RESULT_BUF/4 ];

	union
	{
		BtlBmgDiscoverServicesParams		discoverServicesParams;

		BtlBmgServiceSearchAttrParams	serviceSearchAttrParams;

		BtlBmgServiceSearchParams	serviceSearchParams;
	
    BtlBmgServiceAttrParams	serviceAttrParams;
  }searchParams;

	/* Security Mode Related Fields */

	/* Current security mode */
	BtSecurityMode					securityMode;

	/* New security mode that is pending to be applied, once mode changing succeeds */
	BtSecurityMode					pendingSecurityMode;

	/* Indicates if a security mode changing process is in progress (only a single process simultaneosuly) */
	BOOL							securityModeChangingInProgress;

	/* Indicates the next HCI command that should be sent as part of enabling test mode */
	BtlBmgEnableTestModeHciCommands	enableTestModeNextHciCommand;

	BtlBmgRadioOnData				radioOnData;
	BtlBmgRadioOffData				radioOffData;
	
	BtlBmgAccessibilityData				accessibilityData;

	BtlBmgRequest					*hciInitInterceptionRequest;	
	BOOL							blockHciInitEvents;

	BtHandler							monitorHandler;
	
	/* Memory pools */
	BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_CONFIG_BMG_MAX_NUM_OF_BMG_CONTEXTS, sizeof(BtlBmgContext));
	BTL_POOL_DECLARE_POOL(requestsPool, requestsMemory, BTL_BMG_MAX_NUM_OF_REQUESTS, BTL_BMG_MAX_REQUEST_SIZE);
	BTL_POOL_DECLARE_POOL(hciParmsPool, hciParmsMemory, BTL_CONFIG_BMG_MAX_NUM_OF_CONCURRENT_HCI_COMMANDS, HCI_CMD_PARM_LEN);
	BTL_POOL_DECLARE_POOL(bondParmsPool, bondParmsMemory, BTL_CONFIG_BMG_MAX_NUM_OF_CONCURRENT_BOND_REQUESTS, sizeof(BtlBmgBondData));
	BTL_POOL_DECLARE_POOL(pagingActivitiesPointersPool, pagingActivitiesPointersMemory, BTL_BMG_MAX_NUM_OF_PAGING_ACTIVITIES, sizeof(BtlBmgPagingActivityPointerEntry));
	BTL_POOL_DECLARE_POOL(sdpServiceRecordsPool, sdpServiceRecordsMemory, BTL_CONFIG_BMG_MAX_NUM_OF_USER_SERVICE_RECORDS, BTL_BMG_SERVICE_RECORD_ACTUAL_SIZE);
	BTL_POOL_DECLARE_POOL(sdpAttrValuesPool, sdpAttrValuesMemory, BTL_CONFIG_BMG_MAX_NUM_OF_ATTRIBUTES, BTL_CONFIG_BMG_MAX_ATTRIBUTES_VALUE_SIZE);
							 	  
} BtlBmgData;

static BtlBmgData	btlBmgData;

/* Pool names (for debugging) */

static const char BtlBmgContextsPoolName[] = "BmgContexts";
static const char BtlBmgRequestsPoolName[] = "BmgRequests";
static const char BtlBmgHciParmsPoolName[] = "HciParms";
static const char BtlBmgBondParmsPoolName[] = "BondParms";
static const char BtlBmgsdpServiceRecordsPoolName[] = "sdpServiceRecords";
static const char BtlBmgsdpAttrValuesPoolName[] = "sdpAttrValues";
static const char BtlBmgPagingActivitiesPointersPoolName[] = "pagingActivitiesPointers";

static BtlBmgContext *CurrTestModeBmgContext;

/********************************************************************************
 *
 * Forward Declarations
 *
 *******************************************************************************/
 
static BtStatus BTL_BMG_CreatePools(void);
static BtStatus BTL_BMG_DestroyPools(void);

static BtStatus BTL_BMG_InitData(void);

static BtStatus BTL_BMG_InternalCreateLink(	BtlBmgContext 				*bmgContext,
											BtlBmgCompoundRequest 		*parentRequest,
											const BD_ADDR 				*bdAddr);

static BtStatus BTL_BMG_InternalDisconnectLink(BtlBmgRequest *request);

static BtStatus BTL_BMG_InternalDiscoverServices(	BtlBmgContext 				*bmgContext,
												BtlBmgCompoundRequest 		*parentRequest,
												const BD_ADDR 				*bdAddr,
										  		const SdpServicesMask 	sdpServicesMask);

static BtStatus 	BTL_BMG_InternalCancelServiceDiscovery(void);

static BtStatus 	BTL_BMG_InternalCancelServiceSearchAttributeRequest(void);

static BtStatus 	BTL_BMG_InternalCancelServiceSearchRequest(void);

static BtStatus 	BTL_BMG_InternalCancelServiceAttributeRequest(void);


static BtStatus BTL_BMG_InternalInquiry(BtlBmgContext			*bmgContext,
									BtlBmgCompoundRequest 	*parentRequest,
									const BtIac 				lap, 
									const U8 					length, 
									const U8 					maxResp);

static BtStatus BTL_BMG_InternalCancelInquiryRequest(	BtlBmgContext 			*bmgContext,
															BtlBmgCompoundRequest	*parentRequest);

static BtStatus BTL_BMG_InternalNameRequest(	BtlBmgContext 			*bmgContext,
												BtlBmgCompoundRequest 	*parentRequest,
												const BD_ADDR			*bdAddr);

static BtStatus BTL_BMG_InternalCancelNameRequest(	BtlBmgContext 			*bmgContext,
														BtlBmgCompoundRequest	*parentRequest,
														const BD_ADDR			*bdAddr,
														BtlBmgDynamicRequest		*executingNameRequest);

static BtStatus BTL_BMG_InternalSetSecurityMode(	BtlBmgContext 			*bmgContext, 
												BtlBmgCompoundRequest	*parentRequest,
												const BtSecurityMode 		securityMode,
												BOOL					modifyUnconditionally);

static BtStatus BTL_BMG_InternalSendHciCommand(	BtlBmgContext 			*bmgContext, 
														BtlBmgCompoundRequest	*parentRequest,
														const U16 				hciCommand,
														const U8 					parmsLen, 
														const U8 					*parms, 
														const U8 					event);

BtStatus BTL_BMG_InternalSetAccessibleMode(	BtlBmgContext 			*bmgContext, 
													BtlBmgCompoundRequest	*parentRequest,
													const BtAccessibleMode 		*modeNC,
													const BtAccessModeInfo 	*infoNC,
													const BtAccessibleMode 		*modeC, 
													const BtAccessModeInfo 	*infoC);

static BtStatus _BTL_BMG_InternalSetClassOfDevice(BtClassOfDevice 	classOfDevice);
#if 0
static BtStatus _BTL_BMG_InternalSetPreferredConnectionRole(BtConnectionRole role);
#endif

static void _BTL_BMG_MakeSureAccessibilityModesWillBeAppliedToChip();
static void _BTL_BMG_MakeSureSecurityModeWillBeAppliedToChip();

static BOOL BTL_BMG_DefaultStaticEventsCb(const BtEvent *event, BtlBmgRequest *request);

static BOOL BTL_BMG_SearchRegularByCodCb(const BtEvent *event, BtlBmgRequest *request);
static BOOL BTL_BMG_SearchRegularByDevicesCb(const BtEvent *event, BtlBmgRequest *request);

static BOOL BTL_BMG_SearchRegularCommonCb(	const BtEvent 					*event, 
												BtlBmgRequest 				*request,
												BtlBmgSearchCommonData		*searchData,
												U8							maxFilteredResp,
												BtlBmgSearchFilterFunc			filterFunc);

static BOOL BTL_BMG_HasRegularSearchCompleted(BtlBmgSearchCommonData *searchData);
static void BTL_BMG_CompleteSearch(	BtlBmgSearchCommonData *searchData, 
											BtlBmgCompoundRequest	*specificRequest,
											BOOL					sendCompletionEvent);

static BtStatus BTL_BMG_SearchRegularHandleInquiryResult(	const BtEvent 				*event,
														BtlBmgCompoundRequest	*specificRequest,
														BtlBmgSearchCommonData	*searchData,
														U8						maxFilteredResp,
														BtlBmgSearchFilterFunc		filterFunc);

static void BTL_BMG_SearchRegularAddPendingNameRequest(	BtlBmgSearchCommonData	*searchData);
static void BTL_BMG_SearchRegularRemovePendingNameRequest(	BtlBmgSearchCommonData	*searchData,
																	const BD_ADDR*			bdAddress);
static BOOL BTL_BMG_InternalInquiryCb(const BtEvent *event, BtlBmgRequest *request);
static BOOL BTL_BMG_InternalCreateLinkCb(const BtEvent *event, BtlBmgRequest *request);
static BOOL BTL_BMG_InternalNameRequestCb(const BtEvent *event, BtlBmgRequest *request);

static BOOL BTL_BMG_InternalSendHciCommandCb(const BtEvent *event, BtlBmgRequest *request);

void BTL_BMG_InternalDiscoverServicesCb(U8* respBuff, BTIPS_RS_Status queryStatus);

static BOOL BTL_BMG_InternalSetAccessibleModeCb(const BtEvent *event, BtlBmgRequest *request);

static BOOL btlBmgHciInitEventCb(const BtEvent *event, BtlBmgRequest *request);

static BOOL _BTL_BMG_RadioOnProcessorCb(const BtEvent *event, BtlBmgRequest *request);

static BtlBmgRequest *BTL_BMG_FindMatchingRequest(BtlBmgCompoundRequest *parentRequest, 
														BtlBmgRequest *requestToMatch);

static BtStatus BTL_BMG_HandleRequestCreation(	
				BtlBmgRequest			**request,
				BtlBmgRequestType		requestType,
				BtlBmgCompoundRequest	*parentRequest,
				BtlBmgContext			*bmgContext,
				BtlBmgEvmCallBack		bmgCallback,
				const BD_ADDR			*bdAddress);

static BtStatus BTL_BMG_HandleDynamicRequestCreation(	
				BtlBmgDynamicRequest		**request,
				BtlBmgRequestType		requestType,
				BtlBmgCompoundRequest	*parentRequest,
				BtlBmgContext			*bmgContext,
				BtlBmgEvmCallBack		bmgCallback,
				const BD_ADDR			*bdAddress,
				BtlBmgRequestHandlerType	handlerType);

static BtStatus BTL_BMG_HandleCompoundRequestCreation(	
				BtlBmgCompoundRequest	**request,
				BtlBmgRequestType		requestType,
				BtlBmgCompoundRequest	*parentRequest,
				BtlBmgContext			*bmgContext,
				BtlBmgEvmCallBack		bmgCallback,
				const BD_ADDR			*bdAddress);

static void BTL_BMG_HandleRequestDestruction(BtlBmgRequest *request);

static void _BTL_BMG_HandleTopLevelProcessCompletion(BtlBmgRequest *request);

static BtStatus BTL_BMG_FreeRequest(void** requestAsVoid);

static BtlBmgRequest *BTL_BMG_GetExecutingInquiryRequest(
							const BtlBmgContext		*bmgContext,
							BtlBmgCompoundRequest  	*parentRequest);

static BtlBmgDynamicRequest *BTL_BMG_GetExecutingNameRequest(
							const BtlBmgContext		*bmgContext,
							BtlBmgCompoundRequest  	*parentRequest,
							const BD_ADDR			*bdAddress);

static BtlBmgDynamicRequest *BTL_BMG_GetExecutingConnectRequest(
							const BtlBmgContext		*bmgContext,
							BtlBmgCompoundRequest  	*parentRequest,
							const BD_ADDR			*bdAddress);


static BtlBmgCompoundRequest *BTL_BMG_GetExecutingBondRequest(
								const BtlBmgContext		*bmgContext,
								BtlBmgCompoundRequest  	*parentRequest,
								const BD_ADDR			*bdAddress);

static BtlBmgDynamicRequest *BTL_BMG_GetExecutingControlIncomConnRequest	(
								const BtlBmgContext		*bmgContext,
								BtlBmgCompoundRequest  	*parentRequest);


static BtlBmgRequest *BTL_BMG_GetExecutingRequestByType(	const BtlBmgContext		*bmgContext,
																	BtlBmgCompoundRequest 	*parentRequest,
																	BtlBmgRequestType		requestType);

static BtlBmgRequest *BTL_BMG_GetExecutingRequestByTypeAndAddress(
							const BtlBmgContext		*bmgContext,
							BtlBmgCompoundRequest  	*parentRequest,
							BtlBmgRequestType		requestType,
							const BD_ADDR			*bdAddress);
							

static BOOL BTL_BMG_VerifyLocalNameParms(const U8 *name, U8 len);

static void BTL_BMG_SetLocalNameParms(	const U8 *				name, 
												U8 						len,
												BtlBmgLocalNameParms		*localNameParms);

static void BTL_BMG_InitSearchCommonData(BtlBmgSearchCommonData		*commonSearchData,
												BtlBmgSearchMethod 			method,
												const BOOL 					performNameRequest,
												const SdpServicesMask	sdpServicesMask);

static BtClassOfDevice BTL_BMG_GetCodFromCodFilter(const BtlBmgCodFilter* codFilter);
static BOOL BTL_BMG_DoesPassCodFilter(const BtEvent *event);
static BOOL BTL_BMG_DoesPassDevicesFilter(const BtEvent *event);

static BtStatus BTL_BMG_CancelInquiryDuringSearch(	BtlBmgCompoundRequest		*specificRequest,
														BtlBmgSearchCommonData		*searchData);

static BtStatus BTL_BMG_InternalCancelConnect(BtlBmgContext 		*bmgContext,
										BtlBmgCompoundRequest	*parentRequest,
										const BD_ADDR			*bdAddr);

static void BTL_BMG_CompleteInquiryCancellationDuringSearch(
			BtlBmgCompoundRequest	*specificRequest,
			BtlBmgSearchCommonData	*searchData,
			BtEventType				reportedEvent);

static void BTL_BMG_FillServiceDiscoveryQueryParms(SdpServicesMask serviceMask);
static SdpServicesMask BTL_BMG_GetNextServiceToDiscover(void);

static BtErrorCode BTL_BMG_MapSdpQueryStatusToEventErrorCode(BTIPS_RS_Status queryStatus);

static BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponse(U8 *respBuff, BOOL *answer);

static BOOL BTL_BMG_ShouldSecurityHandlersBeRegistered(BtSecurityMode securityMode);

static BtStatus BTL_BMG_InternalSetLinkEncryption(BtlBmgContext 			*bmgContext,
										BtlBmgCompoundRequest  	*parentRequest,
										const BD_ADDR			*bdAddr,
										const BtEncryptMode encryptMode);	

static BOOL BTL_BMG_InternalSetLinkEncryptionCB(const BtEvent *event, BtlBmgRequest *request);

static BtStatus BTL_BMG_InternalControlIncomingConnectionRequests(BtlBmgContext *bmgContext, 
													BtlBmgCompoundRequest  *parentRequest,
													BOOL enable);

static BOOL BTL_BMG_InternalControlIncomingConnectionRequestsCB(const BtEvent *event, BtlBmgRequest *request);

static BtStatus BTL_BMG_HandleAuthorizationEvent(const BtEvent *event, BOOL *dispatchEvent);

static BtStatus Btl_Bmg_UpdateServiceRecord(	SdpRecord* sdpRecord,
												const SdpAttribute *attributeList,
												const U32 numOfAttributs);

BtStatus BTL_BMG_InternalServiceSearchAttribute(BtlBmgContext *bmgContext,
																					BtlBmgCompoundRequest *parentRequest,
																					const BD_ADDR	*bdAddr,
																					BtlBmgUuid* uuidList,
																					U8  uuidListLen,
																					U16	maxServiceRecords,
																					SdpAttributeId *attributeIdList,
																					U16 attributeIdListLen,
										  										U32 *requestId);

BtStatus BTL_BMG_InternalServiceSearchRequest(BtlBmgContext *bmgContext,
												BtlBmgCompoundRequest *parentRequest,
												const BD_ADDR	*bdAddr,
												BtlBmgUuid* uuidList,
												U8  uuidListLen,
												U16	maxServiceRecords,
									  		U32 *requestId);

BtStatus BTL_BMG_InternalServiceAttrRequest(BtlBmgContext *bmgContext,
												BtlBmgCompoundRequest *parentRequest,
												const BD_ADDR	*bdAddr,
												U32 recordHandle,
												SdpAttributeId *attributeIdList,
												U16 attributeIdListLen,
									  		U32 *requestId);

static void BTL_BMG_ServiceSearchAttributeRequestCb(U8* respBuff, BTIPS_RS_Status queryStatus);

static void BTL_BMG_ServiceSearchRequestCb(U8* respBuff, U16 handlesCount , BTIPS_RS_Status queryStatus);

static void BTL_BMG_ServiceAttributeRequestCb(U8* respBuff, BTIPS_RS_Status queryStatus);

void BTL_BMG_FillServiceSearchAttrQueryParms(BtlBmgUuid* uuidList,
																				U8  uuidListLen,
																				U16	maxServiceRecords,
																				SdpAttributeId *attributeIdList,
																				U16 attributeIdListLen);

void BTL_BMG_FillServiceSearchQueryParms(BtlBmgUuid* uuidList,
																				U8  uuidListLen,
																				U16	maxServiceRecords);

void BTL_BMG_FillServiceAttrQueryParms	(U32 recordHandle,
																				SdpAttributeId *attributeIdList,
																				U16 attributeIdListLen);


void BTL_BMG_CompleteServiceDiscovery(	BtEventType 			completionEventType,
												BtErrorCode 				completionCode,
												BOOL					sendCompletionEvent);

void BTL_BMG_CompleteServiceSearchAttributes(	BtEventType	completionEventType,
												BtErrorCode completionCode,
												BOOL sendCompletionEvent);

void BTL_BMG_CompleteServiceSearch(	BtEventType	completionEventType,
												U16 handlesCount,
												BtErrorCode completionCode,
												BOOL sendCompletionEvent);

void BTL_BMG_CompleteServiceAttributes(	BtEventType	completionEventType,
												BtErrorCode completionCode,
												BOOL sendCompletionEvent);


static BtStatus BTL_BMG_ExecuteNextPagingActivity(void);

static void BTL_BMG_PagingActivityEnded(BtlBmgRequest *pagingRequestActivity);

static BtStatus BTL_BMG_RemovePendingPagingActivity(BtlBmgRequest *requestToRemove);

static BtStatus _BTL_BMG_SetDefaultSecurityMode(void);
static BtStatus _BTL_BMG_SetDefaultDefaultLinkPolicy(void);
static BtStatus _BTL_BMG_SetDefaultAccessibilityModes(void);
static BtStatus _BTL_BMG_SetDefaultClassOfDevice(void);
#if 0
static BtStatus _BTL_BMG_SetDefaultPreferredConnectionRole(void);
#endif

static BtStatus _BTL_BMG_BtlNotificationsCb(BtlModuleNotificationType notificationType);
static void _BTL_BMG_DdbNotificationsCb(BtlBmgDdbNotificationType notificationType, BtlBmgDeviceRecord *record);
static BtStatus _BTL_BMG_RadioOnProcessor(_BtlBmgRadioOnEventType event);
static BtStatus _BTL_BMG_RadioOffProcessor(_BtlBmgRadioOffEventType event, void *additionalData);

static BtStatus btlBmgInterceptHciEvents(void);
static BtStatus BTL_BMG_FindUuidInRespBuff(U8 *respBuff , U16 uuid, BOOL *answer);

extern BtStatus TI_RunSpecialInitScript(char *InitScriptName, TiSpecialScriptCallBack specialScriptCB);

static void BtlBmgMonitorEventHandler(const BtEvent *Event);
static BtStatus _BTL_BMG_InitDsFromDdb(void);

/********************************************************************************
 *
 * Functions Definitions 
 *
 *******************************************************************************/
BtStatus BTL_BMG_Init()
{
	BtStatus	status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_BMG_Init");

	BTL_LOG_INFO(("Initializing BTL BMG"));
		
	BTL_VERIFY_ERR(	(BTL_BMG_INIT_STATE_NOT_INTIALIZED == BtlBmgInitState), 
						BT_STATUS_FAILED, ("BTL_BMG_Init Already called"));
	
	BtlBmgInitState = BTL_BMG_INIT_STATE_INITIALIZATION_FAILED;

	status = BTL_BMG_CreatePools();
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, (""));

	status = BTL_BMG_InitData();
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, (""));

	/* Create the EVM singleton */
	status = BTL_BMG_EVM_Create(	BTL_BMG_DefaultStaticEventsCb, 
									&btlBmgData.contextsList,							
									/* Use default sec mode to decide if sec handlers should be registered */
									BTL_BMG_ShouldSecurityHandlersBeRegistered(btlBmgData.securityMode));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, (""));

	status = _BTL_BMG_SetDefaultDefaultLinkPolicy();
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, (""));

	status = _BTL_BMG_SetDefaultAccessibilityModes();
	BTL_VERIFY_FATAL((status == BT_STATUS_SUCCESS), status, (""));

	status = _BTL_BMG_SetDefaultSecurityMode();
	BTL_VERIFY_FATAL((status == BT_STATUS_SUCCESS), status, (""));

	status = _BTL_BMG_SetDefaultClassOfDevice();
	BTL_VERIFY_FATAL((status == BT_STATUS_SUCCESS), status, (""));
#if 0
	status = _BTL_BMG_SetDefaultPreferredConnectionRole();
	BTL_VERIFY_FATAL((status == BT_STATUS_SUCCESS), status, (""));
#endif
	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_BMG, _BTL_BMG_BtlNotificationsCb);

	btlBmgInterceptHciEvents();
	
	BTL_BMG_DDB_Open();

	status = BTL_BMG_DDB_RegisterForNotifications(_BTL_BMG_DdbNotificationsCb);
	BTL_VERIFY_FATAL((status == BT_STATUS_SUCCESS), status, (""));

	status = _BTL_BMG_InitDsFromDdb();
	BTL_VERIFY_FATAL((status == BT_STATUS_SUCCESS), status, (""));
	
	ME_InitHandler(&btlBmgData.monitorHandler);
	btlBmgData.monitorHandler.callback = BtlBmgMonitorEventHandler;

	DS_RegisterMonitorHandler2(&btlBmgData.monitorHandler);
	
	BtlBmgInitState = BTL_BMG_INIT_STATE_INITIALIZED;

	BTL_LOG_INFO(("BTL BMG Initialized Successfully"));
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_Deinit()
{	
	BtStatus	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_COMMON("BTL_BMG_Deinit");

	BTL_LOG_INFO(("De-Initializing BTL BMG"));

	BtlBmgInitState = BTL_BMG_INIT_STATE_DEINITIALIZATION_FAILED;

	DS_RegisterMonitorHandler2(NULL);
	
	BTL_BMG_DDB_RegisterForNotifications(NULL);

	BTL_BMG_DDB_Close();

	BTL_VERIFY_ERR_NORET(	(0 != IsListEmpty(&btlBmgData.contextsList)),  
						("There are still linked BMG contexts"));
	BTL_VERIFY_ERR_NORET(	(0 != IsListEmpty(&btlBmgData.masterPseudoRequest.childRequests)), 
						("There are still Requests In Progress"));

	BTL_BMG_EVM_Destroy();

	BTL_BMG_DestroyPools();
				
	BtlBmgInitState = BTL_BMG_INIT_STATE_NOT_INTIALIZED;

	BTL_LOG_INFO(("BTL BMG De-Initialized Successfully"));
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}


BtStatus BTL_BMG_Create(BtlAppHandle 			*appHandle,
							const BtlBmgCallBack 	bmgCallback, 
							BtlBmgContext 		**bmgContext)
{
	BtStatus			status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_Create");
	
	BTL_VERIFY_ERR(	(BTL_BMG_INIT_STATE_INITIALIZED == BtlBmgInitState), 
						BT_STATUS_FAILED, ("BTL BMG Not Initialized"));	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	
	/* Allocate memory for the new context */
	status = BTL_POOL_Allocate(&btlBmgData.contextsPool, (void **)bmgContext);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating BMG context"));

	status = BTL_HandleModuleInstanceCreation(appHandle, BTL_MODULE_TYPE_BMG, &(*bmgContext)->base);
	if (BT_STATUS_SUCCESS != status)
	{
		/* bmgContext must be freed before we exit */
		BTL_POOL_Free(&btlBmgData.contextsPool, (void **)bmgContext);

		BTL_ERR(status, ("Failed Handling BMG Instance Creation"));
	}
	
	(*bmgContext)->bmgCallcack = bmgCallback;
	(*bmgContext)->forwardingMode = BTL_BMG_DFLT_EVENT_FORWARDING_MODE;

	/* Add the new context to the contexts list */
	InsertTailList(&btlBmgData.contextsList, &((*bmgContext)->base.node));

	BTL_LOG_INFO(("App [%s] Successfully Created a BMG Context", (0 != appHandle) ? ((char*)(appHandle->appName)) : ("Default")));
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_Destroy(BtlBmgContext **bmgContext)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	BOOL	isAllocated = FALSE;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_Destroy");

	BTL_VERIFY_ERR(	(BTL_BMG_INIT_STATE_INITIALIZED == BtlBmgInitState), 
						BT_STATUS_FAILED, ("BTL BMG Not Initialized"));	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != *bmgContext), BT_STATUS_INVALID_PARM, ("Null *bmgContext"));

	status = BTL_POOL_IsElelementAllocated(&btlBmgData.contextsPool, *bmgContext, &isAllocated);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));
	BTL_VERIFY_ERR((TRUE == isAllocated), BT_STATUS_INVALID_PARM, ("Invalid context"));

	BTL_LOG_INFO(("App [%s] Destroying a BMG Context", (*bmgContext)->base.appHandle->appName));
	
	/* Clean opened 'control incoming connection' requests since these requests remains as long as the context lives*/
	/* We release only top level requests - the assumption is that low level requests are responsible for disabling themselves.*/
	BTL_BMG_InternalControlIncomingConnectionRequests(*bmgContext, &btlBmgData.masterPseudoRequest, FALSE);
	
	/* Remove the context from the list of all BMG contexts */
	RemoveEntryList(&((*bmgContext)->base.node));

	status = BTL_HandleModuleInstanceDestruction(&(*bmgContext)->base);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	status = BTL_POOL_Free(&btlBmgData.contextsPool, (void **)bmgContext);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed Freeing context"));

	*bmgContext = 0;

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_SetEventForwardingMode(	BtlBmgContext 					*bmgContext, 
													const BtlBmgEventForwardingMode	mode)
{
	BtStatus	status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_SetEventForwardingMode");

	BTL_VERIFY_ERR(	(BTL_BMG_INIT_STATE_INITIALIZED == BtlBmgInitState), 
						BT_STATUS_FAILED, ("BTL BMG Not Initialized"));
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	/* Set the new forwarding mode */
	bmgContext->forwardingMode = mode;
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_SearchByCod(BtlBmgContext 				*bmgContext, 
									const BtlBmgSearchMethod 		method,
									const BtIac 					lap, 
									const U8 						length, 
									const U8 						maxResp, 
									const BtlBmgCodFilter 			*filter,
									const BOOL 					performNameRequest,
									const SdpServicesMask 	sdpServicesMask)
{	
	BtStatus					status = BT_STATUS_SUCCESS;
	BtlBmgSearchByCodData	*searchData = 0;
	BtlBmgCompoundRequest	*request = 0;
	BtlBmgEvmCallBack		callback = 0;
	U8						maxNumOfInquiryResponses = maxResp;
	U32						codFilter ;
	
	
	BTL_FUNC_START_AND_LOCK("BTL_BMG_SearchByCod");
	
	codFilter = BTL_BMG_GetCodFromCodFilter(filter);
	
	BTL_VERIFY_ERR(	(BTL_BMG_INIT_STATE_INITIALIZED == BtlBmgInitState), 
						BT_STATUS_FAILED, ("BTL BMG Not Initialized"));
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	BTL_VERIFY_ERR((BTL_BMG_SEARCH_REGULAR == method), BT_STATUS_INVALID_PARM, ("Only regular search method supported"));

	/* Check maxResp validity (<= BTL_CONFIG_BMG_MAX_NUM_OF_RESPONSES_FOR_SEARCH_BY_COD) */
	BTL_VERIFY_ERR(	(maxResp <= BTL_CONFIG_BMG_MAX_NUM_OF_RESPONSES_FOR_SEARCH_BY_COD),
						BT_STATUS_INVALID_PARM, 
						(	"Max Resp Must be <= %d", 
							BTL_CONFIG_BMG_MAX_NUM_OF_RESPONSES_FOR_SEARCH_BY_COD));

	/* Only a single search (of any type) may progress simultaneously */
	BTL_VERIFY_ERR((0 == btlBmgData.searchRequest), BT_STATUS_IN_PROGRESS, ("Search already in progress"));

	if(0 == maxResp)
	{
		maxNumOfInquiryResponses = BTL_CONFIG_BMG_MAX_NUM_OF_RESPONSES_FOR_SEARCH_BY_COD;
	}
	BTL_LOG_INFO(("Starting Search By COD: maxResp: %d, Filter: %x, Get Names: %s, Services Mask: %x", 
					maxNumOfInquiryResponses, codFilter, BTL_BMG_DEBUG_pBool(performNameRequest), sdpServicesMask));
	
	/* Only a single process that requires SDP query may be in progress simultaneously */
	if (SDP_SERVICE_NONE != sdpServicesMask)
	{
		BTL_VERIFY_ERR(FALSE == btlBmgData.sdpQueryInProgress,
						BT_STATUS_NO_RESOURCES, 
						("A Process that involves an SDP query is already in progress"));

		/* Make sure before we start that we would be able to run the SDP queries when we get to that in the search process */
		btlBmgData.sdpQueryInProgress = TRUE;

		BTL_VERIFY_FATAL_NORET(btlBmgData.searchParams.discoverServicesParams.discoveryRequest == 0, ("btlBmgData.searchParams.discoverServicesParams.discoveryRequest != 0"));
	}

	searchData = &btlBmgData.searchData.searchByCodData;
		
	/* Initialize the common (COD / Devices) search data */
	BTL_BMG_InitSearchCommonData(&searchData->commonData,
									method,
									performNameRequest,
									sdpServicesMask);
	
	/* Initialize the COD-specific search data */
	
	searchData->maxResp = maxNumOfInquiryResponses;
	searchData->codFilter = codFilter;

	if (BTL_BMG_SEARCH_REGULAR == method)
	{
		callback = BTL_BMG_SearchRegularByCodCb;
	}

	status = BTL_BMG_HandleCompoundRequestCreation(	&request, 
														BTL_BMG_REQUEST_TYPE_SEARCH_BY_COD,
														&btlBmgData.masterPseudoRequest,
														bmgContext, 
														callback, 
														0);

	if (BT_STATUS_SUCCESS != status)
	{
		BTL_BMG_CompleteSearch(&searchData->commonData,  0, FALSE);
		BTL_ERR(status, ("BTL_BMG_HandleCompoundRequestCreation Failed"));
	}

	/* Start the inquiry */
	if (0 != filter)
	{
		/* No limit (0) on max num of inquiry responses (limit is on filtering) */
		maxNumOfInquiryResponses = 0;
	}

	status = BTL_BMG_InternalInquiry(bmgContext, request, lap, length, maxNumOfInquiryResponses);

	if (BT_STATUS_PENDING != status)
	{
		BTL_BMG_CompleteSearch(&searchData->commonData,  request, FALSE);
		BTL_ERR(status, ("BTL_BMG_InternalInquiry Failed, status = %s", pBT_Status(status)));
	}

	/* Search started successfully */
	
	btlBmgData.searchRequest = request;
	searchData->commonData.inquiryInProgress = TRUE;

	BTL_LOG_INFO(("Started Inquiry as part of Search By COD"));
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_SearchByDevices(BtlBmgContext 		*bmgContext,
								const BtlBmgSearchMethod 		method,
								const BtIac 					lap, 
								const U8 						length, 
								const U8 						listLength, 
								const BD_ADDR 				*devicesList,
								const BOOL 					performNameRequest,
								const SdpServicesMask 	sdpServicesMask)
{	
	BtStatus						status = BT_STATUS_SUCCESS;
	BtlBmgSearchByDevicesData	*searchData = 0;
	BtlBmgCompoundRequest		*request = 0;
	BtlBmgEvmCallBack			callback = 0;
	
	BTL_FUNC_START_AND_LOCK("BTL_BMG_SearchByDevices");
	
	BTL_VERIFY_ERR(	(BTL_BMG_INIT_STATE_INITIALIZED == BtlBmgInitState), 
						BT_STATUS_FAILED, ("BTL BMG Not Initialized"));
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	BTL_VERIFY_ERR((BTL_BMG_SEARCH_REGULAR == method), BT_STATUS_INVALID_PARM, ("Only regular search method supported"));
	BTL_VERIFY_ERR((listLength > 0), BT_STATUS_INVALID_PARM, ("There must be at least one address in the list"));
	
	BTL_VERIFY_ERR((listLength <= BTL_CONFIG_BMG_MAX_NUM_OF_DEVICES_FOR_SEARCH_BY_DEVICES), 
					BT_STATUS_INVALID_PARM, 
					(	"There must be at most %d addresses in the list", 
						BTL_CONFIG_BMG_MAX_NUM_OF_DEVICES_FOR_SEARCH_BY_DEVICES));
	
	/* Only a single search (of any type) may progress simultaneously */
	BTL_VERIFY_ERR((0 == btlBmgData.searchRequest), BT_STATUS_IN_PROGRESS, ("Search already in progress"));

	BTL_LOG_INFO(("Starting Search By COD: #Devices: %d, Get Names: %s, Services Mask: %x", 
					listLength, BTL_BMG_DEBUG_pBool(performNameRequest), sdpServicesMask));
	
	searchData = &btlBmgData.searchData.searchByDevicesData;
	
	/* Initialize the common (COD / Devices) search data */
	BTL_BMG_InitSearchCommonData(&searchData->commonData,
									method,
									performNameRequest,
									sdpServicesMask);
	
	/* Initialize the Devices-specific search data */
	
	searchData->listLength = listLength;
	OS_MemCopy((U8*)searchData->devicesList, (U8*)devicesList, searchData->listLength * BD_ADDR_SIZE);

	if (BTL_BMG_SEARCH_REGULAR == method)
	{
		callback = BTL_BMG_SearchRegularByDevicesCb;
	}

	status = BTL_BMG_HandleCompoundRequestCreation(
											&request, 
											BTL_BMG_REQUEST_TYPE_SEARCH_BY_DEVICES,
											&btlBmgData.masterPseudoRequest,
											bmgContext, 
											callback, 
											0);
	if (BT_STATUS_SUCCESS != status)
	{
		BTL_BMG_CompleteSearch(&searchData->commonData,  0, FALSE);
		BTL_ERR(status, ("BTL_BMG_HandleCompoundRequestCreation Failed"));
	}

	/* Start the inquiry. No limit on max num of inquiry responses (device list limits responses) */
	status = BTL_BMG_InternalInquiry(bmgContext, request, lap, length, 0);
	
	if (BT_STATUS_PENDING != status)
	{
		BTL_BMG_CompleteSearch(&searchData->commonData,  request, FALSE);
		BTL_ERR(status, ("BTL_BMG_InternalInquiry Failed, status = %d", status));
	}

	/* Search successfully started */
	
	btlBmgData.searchRequest = request;
	searchData->commonData.inquiryInProgress = TRUE;
	
	BTL_LOG_INFO(("Started Inquiry as part of Search By Devices"));
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}


BOOL BTL_BMG_HasRegularSearchCompleted(BtlBmgSearchCommonData *searchData)
{
	BOOL answer = TRUE;

	BTL_FUNC_START("BTL_BMG_HasRegularSearchCompleted");

	/* Check completion of inquiry procedures */
	if (TRUE == searchData->inquiryInProgress)
	{
		BTL_LOG_DEBUG(("search is not completed - inquiryInProgress"));
		answer = FALSE;
	}
	/* Inquiry Completed => name requests must have been initiated => check their completion */
	else if (searchData->numOfPendingNameRequests > 0)
	{
		/* There are still pending requests that need to complete executing or cancellation */
		BTL_LOG_DEBUG(("search is not completed - numOfPendingNameRequests is %d", searchData->numOfPendingNameRequests));
		answer = FALSE;
	}
	/*
		Check completion of services discovery procedures:
		Inquiry completed => filtered devices count is final =>
		Verify that we have discovered services on all of them 
	*/
	else if (FALSE == searchData->servicesDiscoveryCompleted)
	{
		BTL_LOG_DEBUG(("search is not completed - services discovery is not completed"));
		answer = FALSE;
	}
	else
	{
		BTL_LOG_INFO(("SEARCH IS COMPLETE"));
	}
	
	BTL_FUNC_END();

	
	return answer;
}

BtStatus BTL_BMG_SearchRegularHandleInquiryResult(	const BtEvent 				*event,
															BtlBmgCompoundRequest	*specificRequest,
															BtlBmgSearchCommonData	*searchData,
															U8						maxFilteredResp,
															BtlBmgSearchFilterFunc		filterFunc)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtDeviceContext    device;
	BtDeviceContext	*currDevice = 0;
	BtlBmgDeviceRecord record;
	U8	resultIndex = 0;
	
	BTL_FUNC_START("BTL_BMG_SearchRegularHandleInquiryResult");
	
	if (BEC_NO_ERROR != event->errCode)
	{
		BTL_LOG_ERROR(("Inquiry Result With Error (%d)", event->errCode));
		BTL_RET_NO_RETVAR();
	}
	
	BTL_LOG_INFO(("Inquiry Result Returned: %s, COD: 0x%x", BTL_UTILS_LockedBdaddrNtoa(&event->p.inqResult.bdAddr), event->p.inqResult.classOfDevice));
	
	BTL_VERIFY_FATAL(	(0 == maxFilteredResp) || (searchData->numOfFilteredResponses <= maxFilteredResp), 
		BT_STATUS_INTERNAL_ERROR,
		("numOfFilteredResponses (%d) > maxFilteredResp (%d)",
		searchData->numOfFilteredResponses, maxFilteredResp));

	/* Check if we recived an inquiry response for this bd address*/
	for(resultIndex = 0; resultIndex < searchData->numOfFilteredResponses ;resultIndex++)
	{
		if(OS_MemCmp((U8*)&searchData->filteredDevicesAddresses[resultIndex],sizeof(BD_ADDR),
			(U8*)&event->p.inqResult.bdAddr,sizeof(BD_ADDR)))
		{
			BTL_LOG_INFO(("Inquiry Result was filtered since this result was already recived (%s)", 
									 BTL_UTILS_LockedBdaddrNtoa(&searchData->filteredDevicesAddresses[resultIndex])));
			BTL_RET_NO_RETVAR();
		}
	}
	/* Check max filtered responses */
	if ((0 != maxFilteredResp) && (searchData->numOfFilteredResponses == maxFilteredResp))
	{
		/* Reached max filtered limit => Cancel inquiry (if not already cancelled) + discard inquiry result */
		
		BTL_LOG_INFO(("Max Num of Required Filtered Devices Reached (%d), Discarding Result", maxFilteredResp));
		
		BTL_BMG_CancelInquiryDuringSearch(specificRequest, searchData);
		BTL_RET_NO_RETVAR();
	}
	
	/* Check if the device passes the filter */
	if (FALSE == filterFunc(event))
	{
		/* inquiry result failed to pass the filter => discard it */
		BTL_LOG_INFO(	("Inq Result FAILED Devices Filter (%s). discarding it", 
			BTL_UTILS_LockedBdaddrNtoa(&event->p.inqResult.bdAddr)));
		
		BTL_RET_NO_RETVAR();
	}
	
	/* 
	Another device passed the filter => handle it:
	1. Record its address
	2. Count it
	3. Notify applications
	4. Issue a name request if necessary
	5. Initiate service discovery if this is the first filtered response
	*/
	
	/* If device is not in list yet, add it now. */
	currDevice = DS_FindDevice(&event->p.inqResult.bdAddr);
		
	if (currDevice == 0) {
		OS_MemSet((U8 *)&device, 0, (U32)sizeof(BtDeviceContext));
		device.addr = event->p.inqResult.bdAddr;
		
		status = DS_AddDevice(&device, &currDevice);
		
		BTL_VERIFY_FATAL(status == BT_STATUS_SUCCESS, BT_STATUS_INTERNAL_ERROR, ("Failed adding device to internal device DB"));
	}
	
	/* Mark device as in range, always update PSI & CoD */
	currDevice->state |= BDS_IN_RANGE;
	currDevice->classOfDevice = event->p.inqResult.classOfDevice;
	currDevice->psi = event->p.inqResult.psi;
	
	if(BTL_BMG_DDB_FindRecord(&currDevice->addr, &record) == BT_STATUS_SUCCESS)
	{
		record.classOfDevice = currDevice->classOfDevice;
		record.psi.psRepMode = currDevice->psi.psRepMode;
		record.psi.psMode = currDevice->psi.psMode;
		
		BTL_BMG_DDB_AddRecord(&record);
	}
	
	/* 2. count it */
	++searchData->numOfFilteredResponses;
	
	BTL_LOG_INFO(("%d Devices PASSED Filter", searchData->numOfFilteredResponses));
	
	/* 1. Record its address */
	OS_MemCopy(	(U8*)&searchData->filteredDevicesAddresses[searchData->numOfFilteredResponses - 1],
		(U8*)&event->p.inqResult.bdAddr,
		BD_ADDR_SIZE);
	
	/* 3. Notify applications */
	BTL_BMG_EVM_DispatchEvent(&(specificRequest->base), 
		event, 
		BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);
	
	BTL_LOG_INFO((	"Issuing Name Request For %s", BTL_UTILS_LockedBdaddrNtoa(&event->p.inqResult.bdAddr)));
	
	/* 4. Issue a name request if necessary */
	if (TRUE == searchData->performNameRequest)
	{
		status = BTL_BMG_InternalNameRequest(
			specificRequest->base.bmgContext, 
			specificRequest, 
			&searchData->filteredDevicesAddresses[searchData->numOfFilteredResponses - 1]);
		
		if (BT_STATUS_PENDING == status)
		{
			BTL_BMG_SearchRegularAddPendingNameRequest(searchData);
		}
		else
		{
			/* [@ToDo][Udi] - Consider which error code to set in searchData->lastErrorCode */
			BTL_LOG_ERROR(("BTL_BMG_InternalNameRequest Failed status: %s", pBT_Status(status)));
		}
	}
	
	/* 5 Initiate service discovery if there isn't already an active one */
	if (0 == btlBmgData.searchParams.discoverServicesParams.discoveryRequest)
	{
		if (SDP_SERVICE_NONE != searchData->sdpServicesMask)
		{
			BTL_LOG_DEBUG(("Discovering Services On Next Device (%s)",
				BTL_UTILS_LockedBdaddrNtoa(
				&searchData->filteredDevicesAddresses[searchData->numOfDevicesThatCompletedServiceDiscovery])));
			
			status = BTL_BMG_InternalDiscoverServices(
				specificRequest->base.bmgContext,
				specificRequest,
				&searchData->filteredDevicesAddresses[searchData->numOfDevicesThatCompletedServiceDiscovery],
				searchData->sdpServicesMask);
			
			if (BT_STATUS_PENDING != status)
			{
				BTL_LOG_ERROR(("BTL_BMG_InternalDiscoverServices Failed with status: %s, Moving on to next device", pBT_Status(status)));

				searchData->lastErrorCode = BEC_PARTIAL_SUCCESS;
				
				/* Discovery failed for this device, move on to the next (no discovery result event to wait for) */
				++searchData->numOfDevicesThatCompletedServiceDiscovery;
			}
		}
	}
	
	BTL_FUNC_END();
	
	return status;
}

void BTL_BMG_SearchRegularAddPendingNameRequest(	BtlBmgSearchCommonData	*searchData)
{
	BTL_FUNC_START("BTL_BMG_SearchRegularAddPendingNameRequest");

	searchData->pendingNameRequestsFlags[searchData->numOfFilteredResponses - 1] = 1;
	
	/* Wait for another name request */
	++searchData->numOfPendingNameRequests;

	BTL_FUNC_END();
}

void BTL_BMG_SearchRegularRemovePendingNameRequest(	BtlBmgSearchCommonData	*searchData,
																	const BD_ADDR*			bdAddress)
{
	U32	addressIndex = 0;
	U32	removedAddressIndex = searchData->numOfFilteredResponses;

	BTL_FUNC_START("BTL_BMG_SearchRegularRemovePendingNameRequest");

	BTL_LOG_DEBUG(("Removing BD Address: |%s|", BTL_UTILS_LockedBdaddrNtoa(bdAddress)));
	
	/* Find the index of the removed address */
	for (addressIndex = 0; addressIndex < searchData->numOfFilteredResponses; ++addressIndex)
	{
		if (TRUE == AreBdAddrsEqual(bdAddress, &searchData->filteredDevicesAddresses[addressIndex]))
		{
			removedAddressIndex = addressIndex;
			break;
		}
	}

	BTL_VERIFY_FATAL_NORET(	(removedAddressIndex < searchData->numOfFilteredResponses),
								("Address Wasn't Found", BTL_UTILS_LockedBdaddrNtoa(bdAddress)));

	BTL_LOG_DEBUG(("The matching adress index is %d", addressIndex));

	searchData->pendingNameRequestsFlags[removedAddressIndex] = 0;
	--searchData->numOfPendingNameRequests;

	BTL_LOG_DEBUG(("There are NOW #%d Pending Name Requests Left", searchData->numOfPendingNameRequests));

	BTL_FUNC_END();
}


BtStatus _BTL_BMG_InternalCancelSearch(BtlBmgContext *bmgContext)
{
	BtStatus						status = BT_STATUS_SUCCESS;
	BtStatus						tempStatus = BT_STATUS_SUCCESS;
	BtlBmgSearchCommonData		*searchData = 0;
	U32							nameRequestIndex = 0;
	BtlBmgDynamicRequest			*executingNameRequest = 0;

	BTL_FUNC_START("_BTL_BMG_InternalCancelSearch");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	/* If there is no search in progress, operation completed successfully */
	if (0 == btlBmgData.searchRequest)
	{
		BTL_LOG_INFO(("No search in progress"));
		
		BTL_RET(BT_STATUS_SUCCESS);
	}
	
	/* Obtain the correct search data object */
	if (BTL_BMG_REQUEST_TYPE_SEARCH_BY_COD == btlBmgData.searchRequest->base.requestType)
	{
		BTL_LOG_DEBUG(("Cancelling COD Search"));
		
		searchData = &btlBmgData.searchData.searchByCodData.commonData;
	}
	else
	{
		BTL_LOG_DEBUG(("Cancelling Devices Search"));
		
		searchData = &btlBmgData.searchData.searchByDevicesData.commonData;
	}

	BTL_VERIFY_ERR((searchData->searchCancellationInProgress == FALSE), BT_STATUS_IN_PROGRESS, ("Search cancellation in progress"));

	/* Record the fact that the search is being canceled */
	searchData->searchCancellationInProgress = TRUE;
	
	status = BTL_BMG_CancelInquiryDuringSearch(btlBmgData.searchRequest, searchData);

	/* Cancel all pending name requests */
	for (nameRequestIndex = 0; nameRequestIndex < searchData->numOfFilteredResponses; ++nameRequestIndex)
	{
		if (0 != searchData->pendingNameRequestsFlags[nameRequestIndex])
		{
			BTL_LOG_DEBUG(("Cancelling Name Request #%d For |%s|", 
							nameRequestIndex, 
							BTL_UTILS_LockedBdaddrNtoa(&searchData->filteredDevicesAddresses[nameRequestIndex])));
		
			tempStatus = BTL_BMG_InternalCancelNameRequest(	btlBmgData.searchRequest->base.bmgContext,
																btlBmgData.searchRequest,
																&searchData->filteredDevicesAddresses[nameRequestIndex],
																executingNameRequest);

			if (BT_STATUS_SUCCESS == tempStatus)
			{
				BTL_BMG_SearchRegularRemovePendingNameRequest(searchData, &searchData->filteredDevicesAddresses[nameRequestIndex]);
			}
			else if (BT_STATUS_PENDING == tempStatus)
			{
				status  = BT_STATUS_PENDING;
			}
			else
			{/* when BT_STATUS_PENDING == status an event will be sent*/
				BTL_ERR_NORET(("BTL_BMG_InternalCancelNameRequest failed for bdAddr: %s, status: %s",
					BTL_UTILS_LockedBdaddrNtoa(&searchData->filteredDevicesAddresses[nameRequestIndex]),
					pBT_Status(status)));
			}
		}
		else
		{
			BTL_LOG_DEBUG(("Entry #%d in pendingNameRequestsFlags is empty", nameRequestIndex));
		}
	}

	BTL_LOG_DEBUG(("btlBmgData.searchParams.discoverServicesParams.discoveryRequest = %x", btlBmgData.searchParams.discoverServicesParams.discoveryRequest));
	
	/* cancel pending discover services.*/
	if(searchData->sdpServicesMask != SDP_SERVICE_NONE)
	{
		if (btlBmgData.searchParams.discoverServicesParams.discoveryRequest != 0)
		{
			tempStatus = BTL_BMG_InternalCancelServiceDiscovery();
				
			if (BT_STATUS_SUCCESS == tempStatus) 
			{
				searchData->servicesDiscoveryCompleted = TRUE;
			}
			else if (BT_STATUS_PENDING == tempStatus)
			{
				status  = BT_STATUS_PENDING;
			}					
		}
	}

	if (status == BT_STATUS_SUCCESS)
	{
		BTL_LOG_INFO(("Cancel Search Completed Immediatley, Calling BTL_BMG_CompleteSearch"));
		BTL_BMG_CompleteSearch(searchData, btlBmgData.searchRequest, FALSE);		
	}
	
	BTL_FUNC_END();
	
	return status;
}


BtStatus BTL_BMG_CancelSearch(BtlBmgContext *bmgContext)
{
	BtStatus	status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_CancelSearch");

	status = _BTL_BMG_InternalCancelSearch(bmgContext);
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_GetRemoteDeviceName(	BtlBmgContext *bmgContext, 
												const BD_ADDR *bdAddr)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_BMG_GetRemoteDeviceName");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null (bdAddr)"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	status = BTL_BMG_InternalNameRequest(bmgContext, &btlBmgData.masterPseudoRequest, bdAddr);
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, (""));

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus _BTL_BMG_CancelGetRemoteDeviceName(	BtlBmgContext 	*bmgContext, 
														const BD_ADDR 	*bdAddr)
{
	BtStatus					status = BT_STATUS_SUCCESS;
	BtlBmgDynamicRequest		*executingNameRequest = 0;
	
	BTL_FUNC_START("_BTL_BMG_CancelGetRemoteDeviceName");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null (bdAddr)"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	status = BTL_BMG_InternalCancelNameRequest(	bmgContext, 
												&btlBmgData.masterPseudoRequest, 
												bdAddr,
												executingNameRequest);
	BTL_VERIFY_ERR(((BT_STATUS_SUCCESS == status) || (BT_STATUS_PENDING == status)), status, (""));

	if ((BT_STATUS_SUCCESS == status) && (0 != executingNameRequest))
	{
		_BTL_BMG_HandleTopLevelProcessCompletion(&(executingNameRequest->base));
	}
	
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_CancelGetRemoteDeviceName(	BtlBmgContext 	*bmgContext, 
														const BD_ADDR 	*bdAddr)
{
	BtStatus					status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_BMG_CancelGetRemoteDeviceName");
	
	status = 	_BTL_BMG_CancelGetRemoteDeviceName(bmgContext, bdAddr);
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_SetLocalDeviceName(BtlBmgContext 	*bmgContext, 
											const U8 			*name, 
											const U8 			len)
{
	BtStatus 					status = BT_STATUS_SUCCESS;
	BOOL					areParmsValid = FALSE;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_SetLocalDeviceName");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != name), BT_STATUS_INVALID_PARM, ("Null name"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	areParmsValid = BTL_BMG_VerifyLocalNameParms(name, len);
	BTL_VERIFY_ERR((TRUE == areParmsValid), BT_STATUS_INVALID_PARM, ("Invalid Parms"));

	/* Save the new local name parms. Use it when we know the operation succeeded (in the callback) */
	BTL_BMG_SetLocalNameParms(name, len, &btlBmgData.localNameParms);

	status = ME_SetLocalDeviceName(	btlBmgData.localNameParms.localName, 
									(U8)(btlBmgData.localNameParms.localNameLen));
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("ME_SetLocalDeviceName Failed"));
		
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_GetLocalDeviceName(BtlBmgContext 	*bmgContext, 
											U8 				*name, 
											U8 				*len)
{
	BtStatus					status = BT_STATUS_SUCCESS;
	BtlBmgLocalNameParms		*localNameParms = &btlBmgData.localNameParms;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_GetLocalDeviceName");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != name), BT_STATUS_INVALID_PARM, ("Null name"));
	BTL_VERIFY_ERR((0 != len), BT_STATUS_INVALID_PARM, ("Null len"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	/* Copy the name without the (potential) terminating null */
	OS_MemCopy(name,  localNameParms->localName, localNameParms->localNameLen);

	/* Set the terminating null in case it is needed */
	if (localNameParms->localNameLen < BTL_BMG_MAX_LOCAL_NAME_LEN)
	{
		name[localNameParms->localNameLen] = 0;
	}
	
	*len = localNameParms->localNameLen;
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;	
}

BtStatus BTL_BMG_ReadLocalBdAddr(BtlBmgContext *bmgContext, BD_ADDR *Addr)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_BMG_ReadLocalBdAddr");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != Addr), BT_STATUS_INVALID_PARM, ("Null Addr"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	status = ME_ReadLocalBdAddr(Addr);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("ME_ReadLocalBdAddr Failed"));
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
	
}

BtStatus BTL_BMG_SetClassOfDevice(BtlBmgContext 		*bmgContext,
										const BtClassOfDevice 	classOfDevice)
{
	BtStatus 					status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_SetClassOfDevice");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	status = _BTL_BMG_InternalSetClassOfDevice(classOfDevice);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));
		
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_SetAccessibleMode(	BtlBmgContext 		*bmgContext, 
											const BtAccessibleMode 	*modeNC,
											const BtAccessModeInfo *infoNC,
											const BtAccessibleMode 	*modeC, 
											const BtAccessModeInfo *infoC)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_SetAccessibleMode");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	status = BTL_BMG_InternalSetAccessibleMode(bmgContext, &btlBmgData.masterPseudoRequest, modeNC, infoNC, modeC, infoC);
	BTL_VERIFY_ERR((status == BT_STATUS_SUCCESS), status, (""));

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_GetAccessibleMode(	BtlBmgContext 	*bmgContext, 
											BtAccessibleMode 	*modeNC,
									 		BtAccessModeInfo 	*infoNC,
									 		BtAccessibleMode 	*modeC, 
									 		BtAccessModeInfo 	*infoC)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_SetAccessibleMode");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	
	if (0 != modeNC)
	{
		*modeNC = btlBmgData.accessibilityData.accessibleModeNC;
	}

	if (0 != infoNC)
	{
		*infoNC = btlBmgData.accessibilityData.accessModeInfoNC;
	}
	
	if (0 != modeC)
	{
		*modeC = btlBmgData.accessibilityData.accessibleModeC;
	}

	if (0 != infoC)
	{
		*infoC = btlBmgData.accessibilityData.accessModeInfoC;
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BMG_SendHciCommand(	BtlBmgContext 	*bmgContext, 
											const U16 		hciCommand,
											const U8 			parmsLen, 
											const U8 			*parms, 
											const U8 			event)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_BMG_SendHciCommand");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	/* A command may have no parms (=> len must be 0), or have parms of valid length */
	BTL_VERIFY_ERR(	((0 == parms) && (parmsLen == 0)) || 									/* no parms */
						((0 != parms) && (parmsLen > 0) && (parmsLen <=  HCI_CMD_PARM_LEN)), 	/* parms of valid len */
						BT_STATUS_INVALID_PARM, 
						("Invalid parmsLen (%d)", parmsLen));

	status = BTL_BMG_InternalSendHciCommand(	bmgContext, 
												&btlBmgData.masterPseudoRequest, 
												hciCommand,
												parmsLen,
												parms,
												event);
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, (""));

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_SetDefaultLinkPolicy(	BtlBmgContext *bmgContext, 
											const BtLinkPolicy inACL, 
											const BtLinkPolicy outACL)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_SetDefaultLinkPolicy");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	status = ME_SetDefaultLinkPolicy(	inACL, outACL);
	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, status, 
					("BTL_BMG_SetDefaultLinkPolicy Failed, Status = %s", pBT_Status(status))); 

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BMG_GetDefaultLinkPolicy(BtlBmgContext *bmgContext, 
										BtLinkPolicy *inACL, 
										BtLinkPolicy *outACL)
{
	BtStatus 			status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_GetDefaultLinkPolicy");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != inACL), BT_STATUS_INVALID_PARM, ("Null inACL"));
	BTL_VERIFY_ERR((0 != outACL), BT_STATUS_INVALID_PARM, ("Null outACL"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	*inACL = MEC(inAclPolicy);
	*outACL = MEC(outAclPolicy);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BMG_GetRemDevState(	BtlBmgContext 	*bmgContext, 
										const BD_ADDR 	*bdAddr, 
										BtRemDevState 	*state)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtRemoteDevice	*remDev = 0;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_BMG_GetRemDevState");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((0 != state), BT_STATUS_INVALID_PARM, ("Null state"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	/* Check if there is an ACL link to the specified device */
	remDev = ME_FindRemoteDevice((BD_ADDR*)bdAddr);

	if (0 != remDev)
	{
		/* ACL link exists, return the current state */
		*state = ME_GetRemDevState(remDev);
		
		BTL_LOG_INFO(("ACL Link State to %s is %d", BTL_UTILS_LockedBdaddrNtoa(bdAddr), *state));
	}
	else
	{
		BTL_LOG_INFO(("No ACL Link to %s", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));
		
		/* No ACL Link => Disconnected */
		*state = BDS_DISCONNECTED;
	}

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_GetRemDevCod(	BtlBmgContext *bmgContext, 
										const BD_ADDR *bdAddr, 
										BtClassOfDevice *cod)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtDeviceContext	*devContext = 0;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_GetRemDevCod");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((0 != cod), BT_STATUS_INVALID_PARM, ("Null cod"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	devContext = DS_FindDevice(bdAddr);

	if (0 != devContext)
	{
		*cod = devContext->classOfDevice;

		BTL_LOG_INFO(("COD (from Known Devices): is %x for %s", *cod, BTL_UTILS_LockedBdaddrNtoa(bdAddr)));
	}
	else /* Device is not in Known devices as well */
	{
		BTL_LOG_INFO(("Failed setting COD for %s", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));
		*cod = 0;
		BTL_RET(BT_STATUS_DEVICE_NOT_FOUND);
	}

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_SendNextTestModeHciCommand(BtlBmgCompoundRequest *enableTestModeRequest)
{
	BtStatus status = BT_STATUS_SUCCESS;

	static const U8 setEventFilterParms[] = {0x02, 0x00, 0x02};
	static const U8 writeScanEnableParms[] = {0x03};

	BTL_FUNC_START("BTL_BMG_SendNextTestModeHciCommand");

	switch (btlBmgData.enableTestModeNextHciCommand)
	{
		case BTL_BMG_ENABLE_TEST_MODE_HCI_ENABLE_TEST_MODE:

			BTL_LOG_DEBUG(("Sending Enable DUT HCI Command"));
			
			status = 	BTL_BMG_InternalSendHciCommand(
							enableTestModeRequest->base.bmgContext,
							enableTestModeRequest,
							HCC_ENABLE_DUT,
							0,
							0,
							HCE_COMMAND_COMPLETE);

			break;

		case BTL_BMG_ENABLE_TEST_MODE_HCI_SET_EVENT_FILTER:

			BTL_LOG_DEBUG(("Sending Set Event Filter HCI Command"));
			
			status = 	BTL_BMG_InternalSendHciCommand(
							enableTestModeRequest->base.bmgContext,
							enableTestModeRequest,
							HCC_EVENT_FILTER,
							3,
							setEventFilterParms,
							HCE_COMMAND_COMPLETE);
			
			break;

		case BTL_BMG_ENABLE_TEST_MODE_HCI_WRITE_SCAN_ENABLE:

			BTL_LOG_DEBUG(("Sending Write Scan Enable HCI Command"));

			status = 	BTL_BMG_InternalSendHciCommand(
							enableTestModeRequest->base.bmgContext,
							enableTestModeRequest,
							HCC_WRITE_SCAN_ENABLE,
							1,
							writeScanEnableParms,
							HCE_COMMAND_COMPLETE);
			
			break;

		default:

			BTL_FATAL(	BT_STATUS_INTERNAL_ERROR, ("Unexpected HCI Command (%d)", 
						btlBmgData.enableTestModeNextHciCommand));
			
	};

	BTL_VERIFY_FATAL(BT_STATUS_PENDING == status, status, 
						("Failed sending HCI command while enabling test mode - please reset device and restart BT stack"));
	
	BTL_FUNC_END();

	return status;
}

void BTL_BMG_CompleteEnableTestMode(	BtlBmgCompoundRequest	*request, 
												BtErrorCode 				completionCode,
												BOOL					sendCompletionEvent)
{
	BOOL isTopLevel = FALSE;
	
	if (0 != request)
	{
		isTopLevel = BTL_BMG_IsRequestTopLevel(&request->base);
	}

	
	if (TRUE == sendCompletionEvent)
	{		
		btlBmgData.selfGeneratedEvent.eType = BTEVENT_ENABLE_TEST_MODE_RESULT;
		btlBmgData.selfGeneratedEvent.errCode = completionCode;

		BTL_BMG_EVM_DispatchEvent(&request->base, 
									&btlBmgData.selfGeneratedEvent, 
									BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);
	}

	if ((0 != request) && (TRUE == isTopLevel))
	{
		_BTL_BMG_HandleTopLevelProcessCompletion(&request->base);
	}
}



BOOL BTL_BMG_SearchRegularCommonCb(	const BtEvent 					*event, 
												BtlBmgRequest 				*request,
												BtlBmgSearchCommonData		*searchData,
												U8							maxFilteredResp,
												BtlBmgSearchFilterFunc			filterFunc)
{
	BtStatus 					status = BT_STATUS_SUCCESS;
	BtlBmgCompoundRequest	*specificRequest = (BtlBmgCompoundRequest*)request;
	BtEventType				inquiryCancellationEvent;
	BOOL					discoverServices = TRUE;
		
	BTL_FUNC_START("BTL_BMG_SearchRegularCommonCb");

	BTL_LOG_DEBUG(("Search Regular CB: Event: %s, Error Code: %s", pME_Event(event->eType), pBT_Status(event->errCode)));

	/* 
		Record the last error code as the operation's error code
		
		[@ToDo][Udi] - Perform some combined logic in the various error codes and 
		generate a single error code for the whole search
	*/
	if (BEC_NO_ERROR != event->errCode)
	{
		searchData->lastErrorCode = event->errCode;
		BTL_LOG_DEBUG(("search last error code: %d", searchData->lastErrorCode));
	}
	
	switch (event->eType)
	{
		case BTEVENT_INQUIRY_RESULT:
			
			/* Discard inquiry results that are received while the search is being cancelled */
			if (TRUE == searchData->searchCancellationInProgress)
			{
				BTL_LOG_DEBUG(("Discarding an Inquiry Result while search is being cancelled"));
			}
			else
			{
				status = BTL_BMG_SearchRegularHandleInquiryResult(	event,
																	specificRequest,
																	searchData,
																	maxFilteredResp,
																	filterFunc);
			}
			
			break;

		case BTEVENT_INQUIRY_COMPLETE:

			BTL_LOG_DEBUG(("Inquiry Complete"));

			searchData->inquiryInProgress = FALSE;

			/* Filteres Inquiry results should be forwarded to all applications */
			BTL_BMG_EVM_DispatchEvent(&(specificRequest->base), 
										event, 
										BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);
			
			break;

		case BTEVENT_INQUIRY_CANCELED:

			if (FALSE == searchData->searchCancellationInProgress)
			{
				BTL_LOG_DEBUG(("Inquiry Cancelled - Normal Flow"));
				inquiryCancellationEvent = BTEVENT_INQUIRY_COMPLETE;
			}
			else
			{
				BTL_LOG_DEBUG(("Inquiry Cancelled Due To Search Cancellation"));
				inquiryCancellationEvent = BTEVENT_INQUIRY_CANCELED;
			}

			BTL_BMG_CompleteInquiryCancellationDuringSearch(	specificRequest, 
																searchData, 
																inquiryCancellationEvent);
			
			break;

		case BTEVENT_NAME_RESULT:
			
			if (FALSE == searchData->searchCancellationInProgress)
			{
				/* Name results should be forwarded to the requesting application only */
				BTL_BMG_EVM_DispatchEvent(&(specificRequest->base), 
											event, 
											BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY);
			}
			
			BTL_BMG_SearchRegularRemovePendingNameRequest(searchData, &event->p.meToken->p.name.bdAddr);
					
			break;

		case BTEVENT_DISCOVER_SERVICES_RESULT:

			/* Another device completed service discovery */
			++searchData->numOfDevicesThatCompletedServiceDiscovery;

			BTL_LOG_DEBUG(("Service Discovery of Filtered Device #%d Completed", 
							searchData->numOfDevicesThatCompletedServiceDiscovery));

			if (FALSE == searchData->searchCancellationInProgress)
			{				
				BTL_BMG_EVM_DispatchEvent(&(specificRequest->base), 
											event, 
											BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY);

				/* arbitrary status value, must be != BT_STATUS_PENDING */
				discoverServices = TRUE;
				
				while (TRUE == discoverServices)  
				{
					if (searchData->numOfDevicesThatCompletedServiceDiscovery < searchData->numOfFilteredResponses)
					{
						BTL_LOG_DEBUG(("Discovering Services On Next Device (%s)",
										BTL_UTILS_LockedBdaddrNtoa(
											&searchData->filteredDevicesAddresses[
																searchData->numOfDevicesThatCompletedServiceDiscovery])));
						
						status = BTL_BMG_InternalDiscoverServices(
									specificRequest->base.bmgContext,
									specificRequest,
									&searchData->filteredDevicesAddresses[
															searchData->numOfDevicesThatCompletedServiceDiscovery],
									searchData->sdpServicesMask);

						if (BT_STATUS_PENDING != status)
						{
							BTL_LOG_ERROR(("BTL_BMG_InternalDiscoverServices Failed with status: %s, Moving on to next device", pBT_Status(status)));
							
							/* Discovery failed for this device, move on to the next (no discovery result event to wait for) */
							++searchData->numOfDevicesThatCompletedServiceDiscovery;
						}
						else
						{
							discoverServices = FALSE;
						}
					}
					else
					{
						discoverServices = FALSE;
					}
				}
			}
			else
			{
				/* Search was cancelled - ignore discovery result, and mark service discovery as completed */
				searchData->servicesDiscoveryCompleted = TRUE;
			}
			
			break;

		case BTEVENT_DISCOVER_SERVICES_CANCELLED:

			searchData->servicesDiscoveryCompleted = TRUE;

			break;

		default:
			
			BTL_ERR_NORET(("Invalid event type: %d", event->eType));
			
	};


	if ((searchData->servicesDiscoveryCompleted == FALSE) &&
			(searchData->numOfDevicesThatCompletedServiceDiscovery == searchData->numOfFilteredResponses) &&
			(searchData->inquiryInProgress == FALSE))
	{
			BTL_LOG_DEBUG(("Completed Service Discovery on all filtered devices"));
			searchData->servicesDiscoveryCompleted = TRUE;
	}
						
	/* Check & handle the end of the search */
	if (TRUE == BTL_BMG_HasRegularSearchCompleted(searchData))
	{
		BTL_BMG_CompleteSearch(searchData, specificRequest, TRUE);
	}

	BTL_FUNC_END();

	return TRUE;
}

BOOL BTL_BMG_SearchRegularByDevicesCb(const BtEvent *event, BtlBmgRequest *request)
{
	BtlBmgCompoundRequest		*specificRequest = (BtlBmgCompoundRequest*)request;
	BtStatus 						status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_SearchRegularByDevicesCb");

	BTL_VERIFY_FATAL(	(BTL_BMG_REQUEST_TYPE_SEARCH_BY_DEVICES== specificRequest->base.requestType), 
						BT_STATUS_INTERNAL_ERROR, 
						("Invalid Request Type: %d", specificRequest->base.requestType));

	BTL_BMG_SearchRegularCommonCb(	event, 
										request,
										&btlBmgData.searchData.searchByDevicesData.commonData,
										btlBmgData.searchData.searchByDevicesData.listLength,
										BTL_BMG_DoesPassDevicesFilter);

	BTL_FUNC_END();

	return TRUE;
}

BOOL BTL_BMG_InternalEnableTestModeCb(const BtEvent *event, BtlBmgRequest *request)
{
	BtStatus 					status = BT_STATUS_SUCCESS;
	BOOL					eventHandled = TRUE;
	BtlBmgCompoundRequest	*enableTestModeRequest = (BtlBmgCompoundRequest*)request;

	BTL_FUNC_START("BTL_BMG_InternalEnableTestModeCb");

	BTL_LOG_DEBUG(("Event: %s (%s)", pME_Event(event->eType), pHC_Status(event->errCode)));
	
	switch (event->eType)
	{
		case BTEVENT_SET_SECURITY_MODE_RESULT:

			if (BEC_NO_ERROR == event->errCode)
			{
				status = BTL_BMG_SendNextTestModeHciCommand(enableTestModeRequest);

				if (BT_STATUS_SUCCESS != status)
				{
					BTL_BMG_CompleteEnableTestMode(	enableTestModeRequest, 
														BEC_UNSPECIFIED_ERR,
														TRUE);
				}
			}
			else
			{
				BTL_LOG_FATAL(("Security disabling failed, failing test mode setting"));
				BTL_BMG_CompleteEnableTestMode(	enableTestModeRequest, 
													event->errCode,
													TRUE);
			}

			break;

		case BTEVENT_SEND_HCI_COMMAND_RESULT:

			if (BEC_NO_ERROR == event->errCode)
			{
				BTL_LOG_INFO(("HCI Command (%d) Sent Successfully", btlBmgData.enableTestModeNextHciCommand));
				
				++btlBmgData.enableTestModeNextHciCommand;

				if (btlBmgData.enableTestModeNextHciCommand <= BTL_BMG_ENABLE_TEST_MODE_LAST_HCI_COMMAND)
				{
					BTL_LOG_DEBUG(("Sending next HCI command"));
					
					status = BTL_BMG_SendNextTestModeHciCommand(enableTestModeRequest);

					if (BT_STATUS_PENDING != status)
					{
						BTL_BMG_CompleteEnableTestMode(	enableTestModeRequest, 
															BEC_UNSPECIFIED_ERR,
															TRUE);
					}
				}
				else
				{
					BTL_LOG_INFO(("Successfully sent all HCI commands"));

					BTL_BMG_CompleteEnableTestMode(	enableTestModeRequest, 
														BEC_NO_ERROR,
														TRUE);
				}
			}
			else
			{
				BTL_FATAL_NORET(("HCI Command (%d) Failed", btlBmgData.enableTestModeNextHciCommand));
			}
			
			break;
			
		case BTEVENT_SECURITY3_COMPLETE:			
		case BTEVENT_SECURITY_CHANGE:

			eventHandled = FALSE;
			
			break;

		default:

			BTL_LOG_ERROR(("Unexpected event received (%s)", pME_Event(event->eType)));
			
			eventHandled = FALSE;
			
	};

	BTL_FUNC_END();

	return eventHandled;
}			


static BtStatus BtlBmgContinueTestMode(BtlBmgContext *bmgContext)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	BtlBmgCompoundRequest	*request = 0;

	BTL_FUNC_START("BtlBmgContinueTestMode");
	
	status = BTL_BMG_HandleCompoundRequestCreation(	&request, 
														BTL_BMG_REQUEST_TYPE_ENABLE_TEST_MODE,
														&btlBmgData.masterPseudoRequest,
														bmgContext, 
														BTL_BMG_InternalEnableTestModeCb, 
														0);
	
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	BTL_LOG_INFO(("Disabling security"));
	
	/* Disable security before entering test mode */
	status = BTL_BMG_InternalSetSecurityMode(bmgContext, request, BSM_SEC_DISABLED, FALSE);

	if (BT_STATUS_PENDING == status)
	{
		BTL_LOG_DEBUG(("Waiting for security to become disabled"));

		/* Exit and wait for the security event to continue */
		BTL_RET(BT_STATUS_PENDING);
	}
	else if (BT_STATUS_SUCCESS == status)
	{
		BTL_LOG_INFO(("Security Disabled"));

		status = BTL_BMG_SendNextTestModeHciCommand(request);

		if (BT_STATUS_PENDING != status)
		{
			BTL_BMG_CompleteEnableTestMode(request, BEC_UNSPECIFIED_ERR, FALSE);
		}
	}
	else
	{
		BTL_BMG_CompleteEnableTestMode(request, BEC_UNSPECIFIED_ERR, FALSE);
		BTL_ERR(status, (""));
	}

	BTL_FUNC_END();

	return status;
}

static void BtlBmgTestModeCallback(BtStatus status)
{
	if(status == BT_STATUS_SUCCESS)
		BTL_LOG_INFO(("Loading special init script finished successfully"));
	else
		BTL_LOG_INFO(("Loading special init script failed"));
	
	BtlBmgContinueTestMode(CurrTestModeBmgContext);
}

BtStatus BTL_BMG_EnableTestMode(BtlBmgContext *bmgContext)
{
	BtStatus 	status = BT_STATUS_PENDING;
	char InitScripName[] = "TIInitRf";

	BTL_FUNC_START_AND_LOCK("BTL_BMG_EnableTestMode");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	/* [@ToDo][Udi] - Add a global test mode flag of the BTL to on and disabe all host BT operations */

	CurrTestModeBmgContext = bmgContext;
	
	if(BT_STATUS_PENDING != TI_RunSpecialInitScript(InitScripName, BtlBmgTestModeCallback))
	{
		/* Case runing special init script failed */
		status = BtlBmgContinueTestMode(bmgContext);
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_BMG_AuthorizeDeviceReply(BtlBmgContext *bmgContext, 
									  const BD_ADDR *bdAddr, 
									  BtlBmgAuthorizationType authorizationType)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtRemoteDevice	*remDev = 0;
	BtlBmgDeviceRecord record;
	BOOL answer;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_AuthorizeDeviceReply");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	/* Obtain the remDev object for the specified address (it must exist) */
	remDev = ME_FindRemoteDevice((BD_ADDR*)bdAddr);
	BTL_VERIFY_ERR((0 != remDev), BT_STATUS_INVALID_PARM, 
					("ME_FindRemoteDevice Failed for %s", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));

	if ((BTL_BMG_AUTHORIZED_TRUSTED == authorizationType) && 
			(BTL_BMG_DDB_FindRecord(&remDev->bdAddr, &record) != BT_STATUS_SUCCESS))
	{
		BTL_BMG_DDB_IsFull(&answer);

		if(TRUE == answer)
		{
			SEC_Authorize(remDev, FALSE, FALSE);

			BTL_LOG_ERROR(("Authorization failed for %s, Data base is full", BTL_UTILS_LockedBdaddrNtoa(&remDev->bdAddr)));

			BTL_RET(BT_STATUS_NO_RESOURCES);
		}
	}

	switch(authorizationType)
	{
		case BTL_BMG_NOT_AUTHORIZED:
			status = SEC_Authorize(remDev, FALSE, FALSE);
			break;
		case BTL_BMG_AUTHORIZED_NOT_TRUSTED:
			status = SEC_Authorize(remDev, TRUE, FALSE);
			break;
		case BTL_BMG_AUTHORIZED_TRUSTED:
			status = BTL_BMG_DDB_FindRecord(&remDev->bdAddr, &record);

			if(BT_STATUS_SUCCESS != status)
			{
				/* there isn't a record for this device - create one.*/
				BTL_BMG_DDB_InitRecord(&record);
				record.bdAddr = *bdAddr;

				status = BTL_BMG_SetDeviceRecord(bmgContext, &record);

				BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("")); 
			}
			
			status = SEC_Authorize(remDev, TRUE, TRUE);
			break;
		default:
			BTL_ERR(BT_STATUS_INVALID_PARM, ("Invalid authorization type"));

	}
	
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
					("SEC_Authorize Failed for %s, status: %d", BTL_UTILS_LockedBdaddrNtoa(bdAddr), pBT_Status(status))); 
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;

}


BtStatus BTL_BMG_AuthorizeServiceReply(BtlBmgContext *bmgContext, 
										const BD_ADDR *bdAddr, 
										BtlBmgAuthorizationType authorizationType)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtRemoteDevice	*remDev = 0;
	BtlBmgDeviceRecord record;
	BOOL answer;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_AuthorizeServiceReply");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	/* Obtain the remDev object for the specified address (it must exist) */
	remDev = ME_FindRemoteDevice((BD_ADDR*)bdAddr);
	BTL_VERIFY_ERR((0 != remDev), BT_STATUS_INVALID_PARM, 
					("ME_FindRemoteDevice Failed for %s", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));

	if ((BTL_BMG_AUTHORIZED_TRUSTED == authorizationType) && 
			(BTL_BMG_DDB_FindRecord(&remDev->bdAddr, &record) != BT_STATUS_SUCCESS))
	{
		BTL_BMG_DDB_IsFull(&answer);

		if(TRUE == answer)
		{
			SEC_AuthorizeService(remDev, FALSE);

			BTL_LOG_ERROR(("Authorization failed for %s, Data base is full", BTL_UTILS_LockedBdaddrNtoa(&remDev->bdAddr)));

			BTL_RET(BT_STATUS_NO_RESOURCES);
		}
	}

	switch(authorizationType)
	{
		case BTL_BMG_NOT_AUTHORIZED:
			status = SEC_AuthorizeService(remDev, FALSE);
			break;
		case BTL_BMG_AUTHORIZED_TRUSTED:
			status = BTL_BMG_DDB_FindRecord(&remDev->bdAddr, &record);

			if(BT_STATUS_SUCCESS != status)
			{
				/* there isn't a record for this device - create one.*/
				BTL_BMG_DDB_InitRecord(&record);
				record.bdAddr = *bdAddr;
			}

			record.trustedPerService |= (SEC_GetSecurityRecord(remDev))->service;

			status = BTL_BMG_SetDeviceRecord(bmgContext, &record);

			BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("")); 
			
			/* no break - drop to next case.*/
		case BTL_BMG_AUTHORIZED_NOT_TRUSTED:
			status = SEC_AuthorizeService(remDev, TRUE);
			break;
		default:
			BTL_ERR(BT_STATUS_INVALID_PARM, ("Invalid authorization type"));
	}
		
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
					("SEC_Authorize Failed for %s, status: %d", BTL_UTILS_LockedBdaddrNtoa(bdAddr), status)); 
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;

}	

BtStatus BTL_BMG_PinReply(	BtlBmgContext		*bmgContext, 
								const BD_ADDR 		*bdAddr, 
								const U8 				*pin,
								const U8 				len, 
								const BtPairingType 	type)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtRemoteDevice	*remDev = 0;
	BtlBmgCompoundRequest	*bondRequest = 0;
	BtSecurityRecord *secRecord;
	BtlBmgDeviceRecord record;
	BOOL answer;
	

	BTL_FUNC_START_AND_LOCK("BTL_BMG_PinReply");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	/* Obtain the remDev object for the specified address (it must exist) */
	remDev = ME_FindRemoteDevice((BD_ADDR*)bdAddr); 
	BTL_VERIFY_ERR((0 != remDev), BT_STATUS_INVALID_PARM, 
					("ME_FindRemoteDevice Failed for %s", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));

	/* In case we are in the middle of bond - record the fact that we received a pin */
	/* there is an assumption here that bond is not an internal process.*/
	bondRequest = BTL_BMG_GetExecutingBondRequest(bmgContext, &btlBmgData.masterPseudoRequest, bdAddr);

	if (0 != bondRequest)
	{
		((BtlBmgBondData *)(bondRequest->base.userData))->waitingForPin = FALSE;
	}

	secRecord = SEC_GetSecurityRecord(remDev);

	if(secRecord != 0)
	{
		if((secRecord->service == SDP_SERVICE_SAP) && (len < SAP_MIN_PIN_LEN))
		{
			status = SEC_SetPin(remDev, 0, 0, 0);
			BTL_ERR(BT_STATUS_INVALID_PARM, ("Pin code for SAP profile must be at least 16 digits"));
		}
	}

	if ((BPT_NOT_SAVED != type) && 
			(BTL_BMG_DDB_FindRecord(&remDev->bdAddr, &record) != BT_STATUS_SUCCESS))
	{
		BTL_BMG_DDB_IsFull(&answer);

		if(TRUE == answer)
		{
			SEC_SetPin(remDev, 0, 0, 0);

			BTL_LOG_ERROR(("Pin reply failed for %s, Data base is full", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));

			BTL_RET(BT_STATUS_NO_RESOURCES);
		}
	}

	status = SEC_SetPin(remDev, pin, len, type);
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, 
					("SEC_SetPin Failed for %s, status: %d", BTL_UTILS_LockedBdaddrNtoa(bdAddr), status)); 
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BOOL BTL_BMG_DoesBondHavePinData(BtlBmgCompoundRequest	*request)
{
	BOOL answer = TRUE;
	BtlBmgBondData	*data = (BtlBmgBondData*)request->base.userData;

	BTL_FUNC_START("BTL_BMG_DoesBondHavePinData");
	
	BTL_VERIFY_FATAL_SET_RETVAR(0 != data, answer = FALSE, ("No Bond Data attached to request"));
	
	if (data->len > 0)
	{
		answer = TRUE;
	}
	else
	{
		answer = FALSE;
	}

	BTL_FUNC_END();

	return answer;
}

static BOOL BTL_BMG_DefaultStaticEventsCb(const BtEvent *event, BtlBmgRequest *request)
{	
	BOOL dispatchEvent = TRUE;
	BtSecurityRecord *secRecord;
		
	BTL_FUNC_START("BTL_BMG_DefaultStaticEventsCb");

	/* Catch here events that are not registered by specific request but still
	have to be processed before forwarding to application.*/

	switch(event->eType)
	{			
		case BTEVENT_AUTHORIZATION_REQ:
			BTL_BMG_HandleAuthorizationEvent(event, &dispatchEvent);
			break;

		case BTEVENT_PIN_REQ:
			secRecord = SEC_GetSecurityRecord(event->p.security.remDev);
			if (secRecord != 0) 
			{
				((BtEvent *)event)->p.security.service = secRecord->service;
			}
			else
			{
				((BtEvent *)event)->p.security.service = 0;
			}
			break;
			
	    case BTEVENT_LINK_CONNECT_CNF:
	    case BTEVENT_LINK_CONNECT_IND:

			{
				BtlBmgDeviceRecord 	record;

				/* Make sure DS record (has updated COD) */
				BtDeviceContext		*bdc = DS_FindDevice(&event->p.remDev->bdAddr);

				if (bdc != NULL)
				{
					if (event->p.remDev->cod != 0)
					{
						bdc->classOfDevice = event->p.remDev->cod;
					}
				}
				
				/* Update COD in DDB record (if exists) */
				if(BTL_BMG_DDB_FindRecord(&event->p.remDev->bdAddr, &record) == BT_STATUS_SUCCESS)
				{
					if (event->p.remDev->cod != 0)
					{
						record.classOfDevice = event->p.remDev->cod;

						BTL_BMG_DDB_AddRecord(&record);
					}				
				}
			}			
		default:
			break;
	}
	
	if(TRUE == dispatchEvent)
	{
		BTL_BMG_EVM_DispatchEvent(request, event, BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);
	}
	
	BTL_FUNC_END();

	return TRUE;
}

static BtStatus BTL_BMG_HandleAuthorizationEvent(const BtEvent *event, BOOL *dispatchEvent)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtRemoteDevice *remDev;
	BtSecurityRecord *secRecord;
	BtlBmgDeviceRecord devRecord;
	BD_ADDR *addr;

	BTL_FUNC_START("BTL_BMG_HandleAuthorizationEvent");

	*dispatchEvent = TRUE;

	secRecord = SEC_GetSecurityRecord(event->p.security.remDev);
			
	BTL_VERIFY_ERR((secRecord != 0), BT_STATUS_FAILED, ("NULL security record for authorization"));

	((BtEvent *)event)->p.security.service = secRecord->service;

	addr = &((event->p.security.remDev)->bdAddr);

	status = BTL_BMG_DDB_FindRecord(addr, &devRecord);

	if(BT_STATUS_SUCCESS != status)
	{
		BTL_LOG_DEBUG(("No device record for", BTL_UTILS_LockedBdaddrNtoa(addr)));

		BTL_RET(status);
	}
			
	if(secRecord->service & devRecord.trustedPerService)
	{
		/*service is authorized for this device - authorize service and do not dispatch event.*/
		remDev = ME_FindRemoteDevice(addr);
		BTL_VERIFY_ERR((0 != remDev), BT_STATUS_INVALID_PARM, 
						("ME_FindRemoteDevice Failed for %s", BTL_UTILS_LockedBdaddrNtoa(addr)));

		status = SEC_AuthorizeService(remDev, TRUE);

		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
				("SEC_AuthorizeService Failed for %s, status: %d", BTL_UTILS_LockedBdaddrNtoa(addr), status)); 

		*dispatchEvent = FALSE;
	}
		
	BTL_FUNC_END();

	return status;
}

BOOL BTL_BMG_SearchRegularByCodCb(const BtEvent *event, BtlBmgRequest *request)
{
	BtlBmgCompoundRequest	*specificRequest = (BtlBmgCompoundRequest*)request;
	BtStatus 					status = BT_STATUS_SUCCESS;


	BTL_FUNC_START("BTL_BMG_SearchRegularByCodCb");

	BTL_VERIFY_FATAL(	(BTL_BMG_REQUEST_TYPE_SEARCH_BY_COD == specificRequest->base.requestType), 
						BT_STATUS_INTERNAL_ERROR, 
						("Invalid Request Type: %d", specificRequest->base.requestType));

	BTL_BMG_SearchRegularCommonCb(	event, 
										request,
										&btlBmgData.searchData.searchByCodData.commonData,
										btlBmgData.searchData.searchByCodData.maxResp,
										BTL_BMG_DoesPassCodFilter);

	BTL_FUNC_END();

	return TRUE;
}

BOOL BTL_BMG_InternalAuthenticateLinkCb(const BtEvent *event, BtlBmgRequest 	*request)
{
	/* [@ToDo][Udi] - The assumption here is that this is always a part of a Bond => Dispatch to the Bond Cb */
	
	BTL_BMG_EVM_DispatchEvent(request, event, BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY);
	
	return FALSE;
}

BtStatus BTL_BMG_InternalAuthenticateLink(	BtlBmgContext			*bmgContext,
												BtlBmgCompoundRequest 	*parentRequest,
												const BD_ADDR			*bdAddr)
{
	BtStatus 					status = BT_STATUS_SUCCESS;
	BtlBmgDynamicRequest		*request = 0;
	BtRemoteDevice			*remDev = 0;
	BOOL isTopLevel = FALSE;
	
	BTL_FUNC_START("BTL_BMG_InternalAuthenticateLink");

	remDev = ME_FindRemoteDevice((BD_ADDR*)bdAddr);
	BTL_VERIFY_ERR(0 != remDev, BT_STATUS_NOT_FOUND, 
					("No remDev Found (%s)", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));

	status = BTL_BMG_HandleDynamicRequestCreation(	&request, 
													BTL_BMG_REQUEST_TYPE_AUTHENTICATE_LINK,
													parentRequest,
													bmgContext, 
													BTL_BMG_InternalAuthenticateLinkCb, 
													bdAddr,
													BTL_BMG_REQUEST_HANDLER_TYPE_BT_HANDLER);

	
	status = SEC_AuthenticateLink(&request->token.btHandler, remDev);

	if(BT_STATUS_PENDING == status)
	{
		/* Do nothing - wait for completion event */
	}
	else
	{
		isTopLevel = BTL_BMG_IsRequestTopLevel(&request->base);

		if (TRUE == isTopLevel)
		{
			_BTL_BMG_HandleTopLevelProcessCompletion(&request->base);
		}
		
		BTL_ERR(status, ("SEC_AuthenticateLink Failed, status = %s", pBT_Status(status)));
	}

	BTL_FUNC_END();

	return status;
}

BtStatus BTL_BMG_CompleteBond(	BtlBmgCompoundRequest	*request, 
										BtErrorCode 				bondCompletionCode,
										BtEventType				reportedEvent,
										BOOL					sendCompletionEvent,
										BOOL					deRegisterGlobalEvents,
										BOOL					disconnectLink)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtlBmgRequest	*createLinkRequest = 0;
	BtlBmgBondData	*bondData = request->base.userData;

	BTL_FUNC_START("BTL_BMG_CompleteBond");

	if (TRUE == disconnectLink)
	{
		/* Find the original create link request */
		createLinkRequest = BTL_BMG_GetExecutingRequestByTypeAndAddress(
							request->base.bmgContext,
							request, 
							BTL_BMG_REQUEST_TYPE_CREATE_LINK,
							&request->base.bdAddress);
		BTL_VERIFY_FATAL(0 != createLinkRequest, BT_STATUS_INTERNAL_ERROR, ("Failed to find The original Create Link Request"));
		
		status = BTL_BMG_InternalDisconnectLink(createLinkRequest);

		if (BT_STATUS_IN_PROGRESS == status)
		{
			BTL_LOG_INFO(("Peer is in the process of coming up - will retry completing bond later"));

			bondData->completeBondWhenPeerConnects = TRUE;
			bondData->bondCompletionCode = bondCompletionCode;
			
			BTL_RET(BT_STATUS_IN_PROGRESS);
		}
	}

	if (TRUE == sendCompletionEvent)
	{
		btlBmgData.selfGeneratedEvent.eType = reportedEvent;
		btlBmgData.selfGeneratedEvent.errCode = bondCompletionCode;

		if (BEC_NO_ERROR == bondCompletionCode)
		{
			OS_MemCopy((U8*)&btlBmgData.selfGeneratedEvent.p.bdAddr, (const U8*)&request->base.bdAddress, BD_ADDR_SIZE);
		}
		
		BTL_BMG_EVM_DispatchEvent(&request->base, 
									&btlBmgData.selfGeneratedEvent, 
									BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);
	}

	if (TRUE == deRegisterGlobalEvents)
	{
		BTL_BMG_EVM_DeRegisterFromStaticEvents(	bondStaticEvents, 
													numOfBondStaticEvents,
													&request->base);
	}
	
	if (0 != request->base.userData)
	{
		BTL_POOL_Free(&btlBmgData.bondParmsPool, &request->base.userData);
	}
	
	_BTL_BMG_HandleTopLevelProcessCompletion(&request->base);

	BTL_FUNC_END();
	
	return status;
}


BOOL BTL_BMG_InternalBondRequestCb(const BtEvent *event, BtlBmgRequest *request)
{
	BtStatus				status = BT_STATUS_SUCCESS;
	BOOL 					eventHandled = TRUE;
	BtlBmgCompoundRequest	*bondRequest = (BtlBmgCompoundRequest*)request;
	BtlBmgBondData			*bondData = 0;
	BtEventType				reportedEvent;
	BtErrorCode				bondErrorCode;
	

	BTL_FUNC_START("BTL_BMG_InternalBondRequestCb");

	BTL_VERIFY_FATAL_SET_RETVAR	(BTL_BMG_REQUEST_TYPE_BOND == request->requestType, 
									eventHandled = FALSE, 
									("Invalid Request Type: %s", BTL_BMG_DEBUG_pRequestType(request->requestType)));

	bondData = bondRequest->base.userData;
	
	BTL_LOG_DEBUG(("Event: %s", pME_Event(event->eType)));
	
	switch (event->eType)
	{
		case BTEVENT_LINK_CONNECT_CNF:			
			if (FALSE == bondData->completeBondWhenPeerConnects)
			{
				if (BEC_NO_ERROR == event->errCode)
				{

					bondData->linkConnectCnfEventReceived = TRUE;
					
					status = BTL_BMG_InternalAuthenticateLink( 	bondRequest->base.bmgContext,
															bondRequest,
															&bondRequest->base.bdAddress);
					if (BT_STATUS_PENDING != status)
					{
						BTL_LOG_ERROR(("Link Authentication Failed, Failing Bond"));
						status = BTL_BMG_CompleteBond(bondRequest, BEC_UNSPECIFIED_ERR, BTEVENT_BOND_RESULT,  TRUE, TRUE, TRUE);
					}
				}
				else
				{
					BTL_LOG_ERROR(("Connection To %s Failed (%s)", BTL_UTILS_LockedBdaddrNtoa(&bondRequest->base.bdAddress),
									pHC_Status(event->errCode)));
					BTL_BMG_CompleteBond(bondRequest, event->errCode, BTEVENT_BOND_RESULT, TRUE, TRUE, FALSE);
				}
			}
			else
			{
				BTL_LOG_DEBUG(("Peer connected, 2nd attempt to complete bond"));
						
				bondData->completeBondWhenPeerConnects = FALSE;

				status = BTL_BMG_CompleteBond(bondRequest, bondData->bondCompletionCode, BTEVENT_BOND_RESULT, TRUE, TRUE, TRUE);
				BTL_VERIFY_FATAL(BT_STATUS_SUCCESS == status, BT_STATUS_INTERNAL_ERROR,
									("Failed 2nd attempt to disconnect and complete Bonding"));
						
			}

			break;
			
		case BTEVENT_PIN_REQ:
			if(TRUE== bondData->bondCancellationInProgress)
			{
				/* In case cancellation is in progress, Reject the pairing operation internally*/
				status = BTL_BMG_PinReply(	bondRequest->base.bmgContext, 
											&bondRequest->base.bdAddress,
											0, 
											0, 
											bondData->type);
				
				BTL_VERIFY_FATAL(BT_STATUS_PENDING == status, BT_STATUS_INTERNAL_ERROR,
									("Failed Setting PIN Automatically (%s)", pBT_Status(status)));
				
				BTL_LOG_INFO(("Pin request is rejected due to bond cancellation"));

				BTL_RET(status);
			}

			/* Verify that the event is for me (This is a static event) */
			if (FALSE == AreBdAddrsEqual(&event->p.remDev->bdAddr, &bondRequest->base.bdAddress))
			{
				BTL_LOG_DEBUG(("PIN Request for another device, ignoring"));
				BTL_RET_SET_RETVAR(eventHandled = FALSE);
			}
				
			if (TRUE == BTL_BMG_DoesBondHavePinData(bondRequest))
			{
				BTL_LOG_INFO(("Pin Request, and Pin specified for Bond => Setting PIN automatically"));
				
				status = BTL_BMG_PinReply(	bondRequest->base.bmgContext, 
											&bondRequest->base.bdAddress,
											bondData->pin, 
											bondData->len, 
											bondData->type);
				BTL_VERIFY_FATAL(BT_STATUS_PENDING == status, BT_STATUS_INTERNAL_ERROR,
									("Failed Setting PIN Automatically (%s)", pBT_Status(status)));

				BTL_LOG_INFO(("Pin is being set, waiting for pairing completion"));
			}
			else
			{
				bondData->waitingForPin = TRUE;
				BTL_LOG_INFO(("Pin Request, and no Pin specified for Bond => Forwarding to Application to supply Pin"));
				eventHandled = FALSE;
			}
			
			break;

		case BTEVENT_PAIRING_COMPLETE:

			BTL_LOG_DEBUG(("Pairing Completed, Ignoring event => will be dispatched to all applications"));
			eventHandled = FALSE;

			break;
			
		case BTEVENT_AUTHENTICATE_CNF:

			if (BEC_NO_ERROR != event->errCode)
			{
				BTL_LOG_DEBUG(("Authentication Failed (%s) => Bond Failed and should be completed", 
								pHC_Status(event->errCode)));
				if(TRUE == bondData->bondCancellationInProgress)
				{
					reportedEvent = BTEVENT_BOND_CANCELLED;
					bondErrorCode = BEC_NO_ERROR;
				}
				else
				{
					reportedEvent = BTEVENT_BOND_RESULT;
					bondErrorCode = event->errCode;
				}
				BTL_BMG_CompleteBond(bondRequest, bondErrorCode, reportedEvent, TRUE, TRUE, TRUE);
			}
			else
			{
				BTL_LOG_DEBUG(("Authentication Succeeded, wait for  BTEVENT_AUTHENTICATED event to complete bond"));
			}
			
			break;

		case BTEVENT_AUTHENTICATED:

			/* Verify that the event is for me (This is a static event) */
			if (FALSE == AreBdAddrsEqual(&event->p.remDev->bdAddr, &bondRequest->base.bdAddress))
			{
				BTL_LOG_DEBUG(("Authenticate Confirmation for another device, ignoring"));
				BTL_RET_SET_RETVAR(eventHandled = FALSE);
			}

			/* Forward the event before the Bond-Completion event */
			BTL_BMG_EVM_DispatchEvent(&bondRequest->base, event, BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);
			
			BTL_LOG_DEBUG(("Authentication Succeeded => Bond Succeeded and should be completed"));
			if(TRUE == bondData->bondCancellationInProgress)
			{
				reportedEvent = BTEVENT_BOND_CANCELLED;
			}
			else
			{
				reportedEvent = BTEVENT_BOND_RESULT;
			}
			BTL_BMG_CompleteBond(bondRequest, BEC_NO_ERROR, reportedEvent, TRUE, TRUE, TRUE);
			
			break;
			
		default:

			BTL_ERR_SET_RETVAR(eventHandled = FALSE, ("Unexpected Event: %s", pME_Event(event->eType)));
			
	};

	BTL_FUNC_END();
	
	return eventHandled;
}

BtStatus BTL_BMG_InitBondData(	BtlBmgCompoundRequest	*request,
								const U8 					*pin, 
								const U8 					len, 
								const BtPairingType		type)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtlBmgBondData	*data = 0;

	BTL_FUNC_START("BTL_BMG_HandleBondData");

	status = BTL_POOL_Allocate(&btlBmgData.bondParmsPool, (void **)(&data));
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));
	
	if (0 == pin)
	{
		data->len = 0;
	}
	else
	{
		OS_MemCopy(data->pin, pin, len);
		data->len = len;
		data->type = type;
	}

	data->waitingForPin = FALSE;
	data->completeBondWhenPeerConnects = FALSE;
	data->bondCompletionCode = BEC_NO_ERROR;
	data->linkConnectCnfEventReceived = FALSE;
	data->bondCancellationInProgress = FALSE;
	
	/* Attach the bond-specific data to the request */
	request->base.userData = data;
	
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_UnbondDevice(BtlBmgContext *bmgContext, const BD_ADDR *bdAddr)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtDeviceRecord	secRecord;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_UnbondDevice");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	BTL_LOG_DEBUG(("BD ADDR: |%s|", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));
	
	status = SEC_FindDeviceRecord(bdAddr, &secRecord);
	BTL_VERIFY_ERR(	(BT_STATUS_SUCCESS == status) || (BT_STATUS_FAILED == status),
						status, ("SEC_FindDeviceRecord Failed, status = %s", pBT_Status(status)));

	if (BT_STATUS_FAILED == status)
	{
		/* Device is already UnBonded */
		BTL_LOG_DEBUG(("No device record found => Device already unbonded"));
		status = BT_STATUS_SUCCESS;
	}
	else
	{
		BTL_LOG_DEBUG(("Deleting device security record to unbond it"));
		
		status = SEC_DeleteDeviceRecord(bdAddr);
		BTL_VERIFY_ERR(	BT_STATUS_SUCCESS == status, BT_STATUS_INTERNAL_ERROR, 
							("SEC_DeleteDeviceRecord Failed, status = %s", pBT_Status(status)));
	}
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_UnbondAll(BtlBmgContext *bmgContext)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_UnbondAll");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
			
	status = SEC_DeleteAllDeviceRecords();
	
	BTL_VERIFY_ERR(	BT_STATUS_SUCCESS == status, BT_STATUS_INTERNAL_ERROR, 
							("SEC_DeleteAllDeviceRecords Failed, status = %s", pBT_Status(status)));
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}


BtStatus BTL_BMG_Bond(	BtlBmgContext 		*bmgContext,
							const BD_ADDR 		*bdAddr,
							const U8 				*pin, 
							const U8 				len, 
							const BtPairingType	type)
{
	BtStatus 					status = BT_STATUS_SUCCESS;
	BtlBmgCompoundRequest	*request = 0;
	BtlBmgCompoundRequest	*executingBondRequest = 0;
	BtlBmgBondData	*bondData;
	BtDeviceRecord record;
	BtRemoteDevice *remDev;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_Bond");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR(	(0 == pin) || ((0 != pin) && (len > 0) && (len <= BTL_BMG_MAX_PIN_LEN)),
						BT_STATUS_INVALID_PARM, ("Pin len (%d) must be between 0 and %d", BTL_BMG_MAX_PIN_LEN));
	BTL_VERIFY_ERR(btlBmgData.securityMode > BSM_SEC_DISABLED,
					BT_STATUS_IMPROPER_STATE, ("Security Mode is Disabled, Illegal to Initiate bonding"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	/* Check if there is already an executing bond request for this context + BD Address */
	executingBondRequest = BTL_BMG_GetExecutingBondRequest(bmgContext, &btlBmgData.masterPseudoRequest, bdAddr);

	/* Reject an attempt to initiate a conflicting bond */
	if (0 != executingBondRequest)
	{
		BTL_LOG_INFO(("Only a single Bond request may progress simultaneously for the same {context,  BD Address}"));
		BTL_RET(BT_STATUS_IN_PROGRESS);
	}

	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, status, (""));

	/* Bond for bonded device is not allowed.*/
	status = SEC_FindDeviceRecord(bdAddr, &record);

	/* In this case - Success means that device is bonded - return failure.*/
	if (BT_STATUS_SUCCESS == status) 
	{
		/* device is already bonded.*/
		BTL_LOG_DEBUG(("Device is already bonded"));
		BTL_RET(BT_STATUS_FAILED);
	}

	remDev = ME_FindRemoteDevice((BD_ADDR *)bdAddr);

	BTL_VERIFY_ERR((0 == remDev), BT_STATUS_FAILED,("Bond for connected device is not allowed"));
		
	status = BTL_BMG_HandleCompoundRequestCreation(	&request, 
														BTL_BMG_REQUEST_TYPE_BOND,
														&btlBmgData.masterPseudoRequest,
														bmgContext, 
														BTL_BMG_InternalBondRequestCb, 
														bdAddr);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	status = BTL_BMG_InitBondData(request, pin, len, type);
	
	if (BT_STATUS_SUCCESS != status)
	{
		BTL_BMG_CompleteBond(request, BEC_UNSPECIFIED_ERR, BTEVENT_BOND_RESULT, FALSE, FALSE, FALSE);
		BTL_ERR(status, (""));
	}

	status = BTL_BMG_EVM_RegisterForStaticEvents(	bondStaticEvents, 
													numOfBondStaticEvents,
													&request->base);
	if (BT_STATUS_SUCCESS != status)
	{
		BTL_BMG_CompleteBond(request, BEC_UNSPECIFIED_ERR, BTEVENT_BOND_RESULT, FALSE, FALSE, FALSE);
		BTL_FATAL(BT_STATUS_INTERNAL_ERROR, (""));
	}

	status = BTL_BMG_InternalCreateLink(bmgContext, request, bdAddr);
	
	if (BT_STATUS_PENDING == status)
	{
		/* Continue processing when connection is established */
		BTL_LOG_DEBUG(("Waiting for Connection to be established"));
		BTL_RET(BT_STATUS_PENDING);
	}
	else if (BT_STATUS_SUCCESS != status)
	{
		BTL_LOG_ERROR(("Failed Creating Connection, Bond Failed"));

		BTL_BMG_CompleteBond(request, BEC_CONNECTION_FAILED, BTEVENT_BOND_RESULT, FALSE, TRUE, FALSE);
		
		BTL_RET(status);
	}
	else
	{
		bondData = request->base.userData;
		bondData->linkConnectCnfEventReceived = TRUE;
		BTL_LOG_DEBUG(("Link Already Exists Authenticating Link"));
	}

	/* Connection exists, continue with process */

	status = BTL_BMG_InternalAuthenticateLink(	bmgContext,
											request,
											bdAddr);
	if (BT_STATUS_PENDING != status)
	{
		BTL_BMG_CompleteBond(request, BEC_UNSPECIFIED_ERR, BTEVENT_BOND_RESULT, FALSE, TRUE, TRUE);
		BTL_RET(status);
	}

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_GetDeviceBondingState(BtlBmgContext *bmgContext, BD_ADDR *bdAddr, BOOL *answer)
{
	BtStatus 					status = BT_STATUS_SUCCESS;
	BtDeviceRecord record;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_GetDeviceBondingState");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	*answer = ((SEC_FindDeviceRecord(bdAddr, &record) == BT_STATUS_SUCCESS));

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}



BtStatus _BTL_BMG_CancelBond(BtlBmgContext *bmgContext, 
							const BD_ADDR *bdAddr)
{	
	BtStatus 					status = BT_STATUS_SUCCESS;
	BtlBmgCompoundRequest	*executingBondRequest = 0;
	BtlBmgBondData			*bondData;

	BTL_FUNC_START("_BTL_BMG_CancelBond");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));


	/* If there is no bond in progress, operation completed successfully */
	executingBondRequest = BTL_BMG_GetExecutingBondRequest(bmgContext, &btlBmgData.masterPseudoRequest, bdAddr);
	
	if (0 == executingBondRequest)
	{
		BTL_LOG_INFO(("No bond in progress"));
		
		BTL_RET(BT_STATUS_SUCCESS);
	}

	bondData = executingBondRequest->base.userData;
	
	/* Reject an attempt to initiate a conflicting cancel bond request */
	if(TRUE == bondData->bondCancellationInProgress)
	{
		BTL_LOG_INFO(("Cancel bond to %s is already in progress", BTL_UTILS_LockedBdaddrNtoa(&(executingBondRequest->base.bdAddress))));
		
		BTL_RET(BT_STATUS_IN_PROGRESS);
	}

	if(TRUE == bondData->waitingForPin)
	{
		BTL_LOG_INFO(("Waiting for pin. First enter pin and then retry cancelling bond"));
		
		BTL_RET(BT_STATUS_RESTRICTED);
	}

	
	/* Record the fact that the bond is being canceled */
	bondData->bondCancellationInProgress = TRUE;

	if(FALSE == bondData->linkConnectCnfEventReceived)
	{
		status = BTL_BMG_InternalCancelConnect(executingBondRequest->base.bmgContext,
											executingBondRequest,
											&(executingBondRequest->base.bdAddress));

		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), (""));

		/* Don't send event the call will exit with status (No pending case).*/
		status = BTL_BMG_CompleteBond(executingBondRequest, BT_STATUS_SUCCESS,  BTEVENT_BOND_CANCELLED, FALSE, TRUE, FALSE);

		BTL_RET(status);
	}

	status = BT_STATUS_PENDING;

	BTL_FUNC_END();
	
	return status;
	
}


BtStatus BTL_BMG_CancelBond(BtlBmgContext *bmgContext, 
							const BD_ADDR *bdAddr)
{	
	BtStatus 	status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_CancelBond");

	status = _BTL_BMG_CancelBond(bmgContext, bdAddr);

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}
	

BtStatus BTL_BMG_SetLinkEncryption(BtlBmgContext *bmgContext, 
									const BD_ADDR *bdAddr, 
									const BtEncryptMode encryptMode)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_BMG_SetLinkEncryption");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null (bdAddr)"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	BTL_VERIFY_ERR((BECM_ENCRYPT_DISABLE == encryptMode || BECM_ENCRYPT_ENABLE == encryptMode), 
					BT_STATUS_INVALID_PARM, ("Invalid encryptMode"));


	status = BTL_BMG_InternalSetLinkEncryption(bmgContext, &btlBmgData.masterPseudoRequest, bdAddr, encryptMode);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status), status, (""));

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;

}

BtStatus BTL_BMG_InternalSetLinkEncryption(BtlBmgContext 			*bmgContext,
										BtlBmgCompoundRequest  	*parentRequest,
										const BD_ADDR			*bdAddr,
										const BtEncryptMode encryptMode)		
{

	BtStatus 		status = BT_STATUS_SUCCESS;
	BtRemoteDevice *remDev = 0;
	BtlBmgDynamicRequest		*request = 0;
	BOOL isTopLevel = FALSE;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_InternalSetLinkEncryption");

	BTL_VERIFY_FATAL((0 != parentRequest), BT_STATUS_INTERNAL_ERROR, ("Null parent"));

	remDev = ME_FindRemoteDevice((BD_ADDR *)bdAddr);

	BTL_VERIFY_ERR(0 != remDev, BT_STATUS_DEVICE_NOT_FOUND, 
					("No remDev Found (%s)", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));


	status = BTL_BMG_HandleDynamicRequestCreation(	&request, 
													BTL_BMG_REQUEST_TYPE_SET_LINK_ENCRYPTION,
													parentRequest,
													bmgContext, 
													BTL_BMG_InternalSetLinkEncryptionCB, 
													bdAddr,
													BTL_BMG_REQUEST_HANDLER_TYPE_BT_HANDLER);
	
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));
	
	status = SEC_SetLinkEncryption(&request->token.btHandler, remDev, encryptMode);

	if(BT_STATUS_PENDING == status)
	{
		/* Do nothing - wait for completion event */
	}
	else
	{
		isTopLevel = BTL_BMG_IsRequestTopLevel(&request->base);

		if (TRUE == isTopLevel)
		{
			_BTL_BMG_HandleTopLevelProcessCompletion(&request->base);
		}
		
		if(BT_STATUS_SUCCESS != status)
		{
			BTL_ERR(status, ("SEC_SetLinkEncryption Failed, status = %s", pBT_Status(status)));
		}
	}

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
	
}


BOOL BTL_BMG_InternalSetLinkEncryptionCB(const BtEvent *event, BtlBmgRequest *request)
{
	BOOL isTopLevel = BTL_BMG_IsRequestTopLevel(request);
	
	BTL_FUNC_START("BTL_BMG_InternalSetLinkEncryptionCB");
				
	BTL_BMG_EVM_DispatchEvent(request, event,  BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);

	if (TRUE == isTopLevel)
	{
		_BTL_BMG_HandleTopLevelProcessCompletion(request);
	}

	BTL_FUNC_END();

	return TRUE;
}

BtStatus BTL_BMG_GetLinkEncryption(BtlBmgContext *bmgContext, 
									const BD_ADDR *bdAddr, 
									BtEncryptMode *encryptMode)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	BtRemoteDevice *remDev = 0;
	BtEncryptState encryptState = BES_NOT_ENCRYPTED;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_GetLinkEncryption");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null (bdAddr)"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	BTL_VERIFY_ERR((0 != encryptMode), BT_STATUS_INVALID_PARM, ("Invalid encryptMode"));

	remDev = ME_FindRemoteDevice((BD_ADDR *)bdAddr);

	BTL_VERIFY_ERR(0 != remDev, BT_STATUS_DEVICE_NOT_FOUND, 
					("No remDev Found (%s)", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));

	encryptState = ME_GetRemDevEncryptState(remDev);

	*encryptMode = (U8)(encryptState == 0 ? BECM_ENCRYPT_DISABLE : BECM_ENCRYPT_ENABLE);

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;

}



BtStatus BTL_BMG_InternalCancelConnect(	BtlBmgContext 		*bmgContext,
									BtlBmgCompoundRequest	*parentRequest,
									const BD_ADDR			*bdAddr)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	BtlBmgDynamicRequest		*executingConnectRequest = 0;
	BtRemoteDevice			*remDev = 0;

	BTL_FUNC_START("BTL_BMG_InternalCancelConnect");
	
	executingConnectRequest = BTL_BMG_GetExecutingConnectRequest(bmgContext, parentRequest, bdAddr);

	/* If no matching connect request in progress, return with a success */
	if (0 == executingConnectRequest)
	{
		BTL_LOG_INFO(("No connect request in progress for this {context,  BD Address}"));
		BTL_RET(BT_STATUS_SUCCESS);
	}

	/* Request cancellation, using the original handler that is saved in the executing request */
	remDev = ME_FindRemoteDevice(&executingConnectRequest->base.bdAddress);
	BTL_VERIFY_FATAL(0 != remDev, BT_STATUS_INTERNAL_ERROR, ("Failed to find the remote device"));
	status = ME_CancelCreateLink(&executingConnectRequest->token.btHandler, remDev);

	if (BT_STATUS_SUCCESS == status)
	{
		BTL_LOG_INFO(("ME_CancelCreateLink Completed"));
	}
	else
	{
		BTL_LOG_ERROR(("ME_CancelCreateLink Failed, returned status: %d", pBT_Status(status)));
	}

	BTL_FUNC_END();
	
	return status;

}


BtStatus BTL_BMG_DeleteDeviceRecord(BtlBmgContext *bmgContext, 
													const BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtRemoteDevice *remDev = 0;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_DeleteDeviceRecord");
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null (bdAddr)"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	
	status = BTL_BMG_DDB_DeleteRecord(bdAddr);

	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, status, ("BTL_BMG_DDB_DeleteRecord Failed, Status = %s", pBT_Status(status))); 

	remDev = ME_FindRemoteDevice((BD_ADDR *)bdAddr);

	if(0 == remDev)
	{
		status = DS_DeleteDevice(bdAddr);

		BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, status, ("DS_DeleteDevice Failed for %s, Status = %s", 
								BTL_UTILS_LockedBdaddrNtoa(bdAddr),pBT_Status(status))); 
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BMG_CleanDeviceDB(BtlBmgContext *bmgContext)
{
	BtStatus 				status = BT_STATUS_SUCCESS;
	BtDeviceContext 		*bdc = 0;
	BtlBmgDeviceRecord	deviceRecord;
	U32					numOfDeviceRecords = 0;
	U32					recordIndex = 0;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_CleanDeviceDB");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
		BT_STATUS_INVALID_PARM, ("Invalid context"));

	status = BTL_BMG_DDB_GetNumOfDeviceRecords(&numOfDeviceRecords);
	BTL_VERIFY_FATAL(status == BT_STATUS_SUCCESS, status, ("BTL_BMG_DDB_GetNumOfDeviceRecords Failed"));

	for (recordIndex = 0; recordIndex < numOfDeviceRecords; ++recordIndex)
	{
		status = BTL_BMG_DDB_EnumDeviceRecords(recordIndex, &deviceRecord);
		BTL_VERIFY_FATAL(status == BT_STATUS_SUCCESS, status, ("BTL_BMG_DDB_EnumDeviceRecords Failed (%s)", pBT_Status(status)));

		bdc =  DS_FindDevice(&deviceRecord.bdAddr);
		BTL_VERIFY_FATAL(bdc != NULL, BT_STATUS_INTERNAL_ERROR, 
							("Device in DB but not in DS (%s)", BTL_UTILS_LockedBdaddrNtoa(&deviceRecord.bdAddr)));
		
		status = DS_DeleteDevice(&deviceRecord.bdAddr);
		BTL_VERIFY_FATAL(status == BT_STATUS_SUCCESS, status, ("DS_DeleteDevice Failed (%s)", pBT_Status(status)));
	}

	status = BTL_BMG_DDB_DeleteAllRecords();
	BTL_VERIFY_FATAL(status == BT_STATUS_SUCCESS, status, ("BTL_BMG_DDB_DeleteAllRecords Failed (%s)", pBT_Status(status)));
		
	status = BTL_BMG_DDB_GetNumOfDeviceRecords(&numOfDeviceRecords);
	BTL_VERIFY_FATAL(status == BT_STATUS_SUCCESS, status, ("BTL_BMG_DDB_GetNumOfDeviceRecords Failed"));
	BTL_VERIFY_FATAL(numOfDeviceRecords == 0, BT_STATUS_INTERNAL_ERROR, ("Device DB Still Contains Records (%d)", numOfDeviceRecords));
		
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_InitDeviceRecord(BtlBmgContext *bmgContext, BtlBmgDeviceRecord* record)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_CleanDeviceDB");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != record), BT_STATUS_INVALID_PARM, ("Null record"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid record"));

	status = BTL_BMG_DDB_InitRecord(record);
	
	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, status, ("")); 
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;


}

BOOL BTL_BMG_SecurityMode3ChangeCb(const BtEvent* event, BtlBmgRequest *request)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL isTopLevel = BTL_BMG_IsRequestTopLevel(request);
	
	BTL_FUNC_START("BTL_BMG_SecurityModeChangeCb");

	BTL_VERIFY_ERR(BTEVENT_SECURITY3_COMPLETE == event->eType, BT_STATUS_INTERNAL_ERROR, 
					("Unexpected Event: %s", pME_Event(event->eType)));
	
	BTL_LOG_DEBUG(("New Security Mode: %s, encryption: %s", pSEC_Mode(event->p.secMode.mode), 
					BTL_BMG_DEBUG_pBool(event->p.secMode.encrypt)));

	BTL_BMG_EVM_DispatchEvent(request, event, BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY);

	if (TRUE == isTopLevel)
	{
		_BTL_BMG_HandleTopLevelProcessCompletion(request);
	}
	
	BTL_FUNC_END();

	return TRUE;
}

BtStatus BTL_BMG_EnableSecurityMode3(BtlBmgCompoundRequest *parentRequest, BOOL encryption)
{
	BtStatus 					status = BT_STATUS_SUCCESS;
	BtlBmgDynamicRequest		*request = 0;
	BOOL isTopLevel = FALSE;

	BTL_FUNC_START("BTL_BMG_EnableSecurityMode3");

	status = BTL_BMG_HandleDynamicRequestCreation(	&request, 
													BTL_BMG_REQUEST_TYPE_ENABLE_SEC_MODE_3,
													parentRequest,
													parentRequest->base.bmgContext, 
													BTL_BMG_SecurityMode3ChangeCb, 
													0,
													BTL_BMG_REQUEST_HANDLER_TYPE_BT_HANDLER);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));
	
	status = SEC_EnableSecurityMode3(&request->token.btHandler, encryption);

	if (BT_STATUS_PENDING == status)
	{
		/* Do nothing - wait for completion event */
	}
	else
	{
		isTopLevel = BTL_BMG_IsRequestTopLevel(&request->base);

		if (TRUE == isTopLevel)
		{
			_BTL_BMG_HandleTopLevelProcessCompletion(&request->base);
		}
		
		if (BT_STATUS_SUCCESS != status)
		{
			BTL_ERR(status, ("SEC_EnableSecurityMode3 Failed, status = %s", pBT_Status(status)));
		}
	}

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_DisableSecurityMode3(BtlBmgCompoundRequest *parentRequest)
{
	BtStatus 					status = BT_STATUS_SUCCESS;
	BtlBmgDynamicRequest		*request = 0;

	BTL_FUNC_START("BTL_BMG_DisableSecurityMode3");
	
	status = BTL_BMG_HandleDynamicRequestCreation(	&request, 
													BTL_BMG_REQUEST_TYPE_DISABLE_SEC_MODE_3,
													parentRequest,
													parentRequest->base.bmgContext, 
													BTL_BMG_SecurityMode3ChangeCb, 
													0,
													BTL_BMG_REQUEST_HANDLER_TYPE_BT_HANDLER);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));
	
	status = SEC_DisableSecurityMode3(&request->token.btHandler);

	if (BT_STATUS_PENDING == status)
	{
		/* Do nothing - wait for completion event */
	}
	else
	{
		BTL_BMG_HandleRequestDestruction(&request->base);
		
		if (BT_STATUS_SUCCESS != status)
		{
			BTL_ERR(status, ("SEC_DisableSecurityMode3 Failed, status = %s", pBT_Status(status)));
		}
	}

	BTL_FUNC_END();
	
	return status;
}

void BTL_BMG_CompleteSetSecurityMode(	BtlBmgCompoundRequest	*request, 
												BtErrorCode 				completionCode,
												BOOL					sendCompletionEvent,
												BOOL					deRegisterGlobalEvents)
{
	BOOL isTopLevel = FALSE;

	BTL_FUNC_START("BTL_BMG_CompleteSetSecurityMode");

	if (0 != request)
	{
		isTopLevel = BTL_BMG_IsRequestTopLevel(&request->base);
	}
	
	if (BEC_NO_ERROR == completionCode)
	{
		btlBmgData.securityMode = btlBmgData.pendingSecurityMode;
		MEC(securityMode) = btlBmgData.securityMode;	/* Mirror state in ME layer */
		btlBmgData.pendingSecurityMode = _BTL_BMG_INVALID_SECURITY_MODE;
		
		btlBmgData.selfGeneratedEvent.p.secMode.mode = btlBmgData.securityMode;
	}
	
	if (TRUE == sendCompletionEvent)
	{		
		btlBmgData.selfGeneratedEvent.eType = BTEVENT_SET_SECURITY_MODE_RESULT;
		btlBmgData.selfGeneratedEvent.errCode = completionCode;

		BTL_BMG_EVM_DispatchEvent(&request->base, 
									&btlBmgData.selfGeneratedEvent, 
									BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);
	}

	if (TRUE == deRegisterGlobalEvents)
	{
		BTL_BMG_EVM_DeRegisterFromStaticEvents(	setSecModeStaticEvents, 
													numOfSetSecModeStaticEvents,
													&request->base);
	}
	
	btlBmgData.securityModeChangingInProgress = FALSE;

	if ((0 != request) && (TRUE == isTopLevel))
	{
		_BTL_BMG_HandleTopLevelProcessCompletion(&request->base);
	}
}

BOOL BTL_BMG_InternalSetSecurityModeCb(const BtEvent *event, BtlBmgRequest *request)
{
	BOOL					eventHandled = TRUE;
	BtlBmgCompoundRequest	*setSecModeRequest = (BtlBmgCompoundRequest*)request;
	
	BTL_FUNC_START("BTL_BMG_InternalSetSecurityModeCb");

	BTL_LOG_DEBUG(("Event: %s (%s)", pME_Event(event->eType), pHC_Status(event->errCode)));
	
	switch (event->eType)
	{
		case BTEVENT_SECURITY3_COMPLETE:

			if (BEC_NO_ERROR != event->errCode)
			{
				BTL_LOG_DEBUG(("Security Mode 3 Changing Failed (%s) => Set Sec Mode Failed and should be completed", 
								pHC_Status(event->errCode)));
				BTL_BMG_CompleteSetSecurityMode(setSecModeRequest, event->errCode, TRUE, TRUE);
			}
			else
			{
				BTL_LOG_DEBUG(("Set Mode 3 Succeeded, wait for  BTEVENT_SECURITY_CHANGE event to complete bond"));
			}
			
			break;

		case BTEVENT_SECURITY_CHANGE:

			/* Forward the event before the Set-Sec-Mode Completion event */
			BTL_BMG_EVM_DispatchEvent(&setSecModeRequest->base, event, BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);
			
			BTL_LOG_DEBUG(("Sec Mode Changing Succeeded => Set Sec Mode Succeeded and should be completed"));
			BTL_BMG_CompleteSetSecurityMode(setSecModeRequest, BEC_NO_ERROR, TRUE, TRUE);

			break;

		default:

			BTL_FATAL_SET_RETVAR(eventHandled = FALSE, ("Unexpected Event (%s)", pME_Event(event->eType)));
			
	};

	BTL_FUNC_END();

	return eventHandled;
}

void _BTL_BMG_MakeSureSecurityModeWillBeAppliedToChip()
{
		/* Make sure that when applying the sec mode (chip on) sec mode 3 enabling / disabling in ESI will be executed */
		if (btlBmgData.securityMode < BSM_SEC_LEVEL_3)
		{
			MEC(secModeState) = BSMS_LEVEL3;
		}
		else
		{
			MEC(secModeState) = BSMS_LEVEL2;
		}
}

void _BTL_BMG_MakeSureAccessibilityModesWillBeAppliedToChip()
{
	/* Force the stack to apply them since we cannot assume default radio accessibility */
	MEC(accModeNC) = (U8)(~btlBmgData.accessibilityData.accessibleModeNC);
	MEC(accInfoNC).inqInterval  = (U16)(~btlBmgData.accessibilityData.accessModeInfoNC.inqInterval);

	MEC(accModeC) = (U8)(~btlBmgData.accessibilityData.accessibleModeC);
	MEC(accInfoC).inqInterval  = (U16)(~btlBmgData.accessibilityData.accessModeInfoC.inqInterval);

	MEC(accModeCur) = (U8)((MEC(accModeNC) + 2) % 255);
	MEC(accInfoCur).inqInterval = (U16)((MEC(accInfoNC).inqInterval + 2) % 65535);

	if (btlBmgData.accessibilityData.accessibleModeNC == BAM_GENERAL_ACCESSIBLE)
	{
		MEC(isLiac) = TRUE;
	}
	else if (btlBmgData.accessibilityData.accessibleModeNC == BAM_LIMITED_ACCESSIBLE)
	{
		MEC(isLiac) = FALSE;
	}	
}

BtStatus BTL_BMG_InternalSetSecurityMode(	BtlBmgContext 			*bmgContext, 
												BtlBmgCompoundRequest	*parentRequest,
												const BtSecurityMode 		securityMode,
												BOOL					modifyUnconditionally)
{
	BtStatus 					status = BT_STATUS_SUCCESS;
	BtlBmgCompoundRequest	*request = 0;
	BOOL					enableMode3Security = FALSE;
	BOOL					encrypt = FALSE;
	BOOL					disableMode3Security = FALSE;
	BOOL					registerSecHandlers = FALSE;
	BOOL					deRegisterSecHandlers = FALSE;
	
	BTL_FUNC_START("BTL_BMG_InternalSetSecurityMode");

	BTL_VERIFY_ERR(securityMode != BSM_SEC_LEVEL_1, BT_STATUS_NOT_SUPPORTED, ("Security Level 1 is not supported"));
	
	/* Check if the chip is on */
	if (BTL_IsChipOn() == FALSE)
	{
		BTL_LOG_INFO(("Radio is off - Saving sec mode (%s) to be applied when radio turns on", pSEC_Mode(securityMode)));

		btlBmgData.securityMode = securityMode;
		MEC(securityMode) = btlBmgData.securityMode;	/* Mirror state in ME layer */

		_BTL_BMG_MakeSureSecurityModeWillBeAppliedToChip();
		
		BTL_RET(BT_STATUS_SUCCESS);
	}

	BTL_LOG_DEBUG(("Current securityMode = %s, Requested Sec Mode = %s", 
					pSEC_Mode(btlBmgData.securityMode), (pSEC_Mode(securityMode))));

	/* Save the desired security mode, apply when all completes successfully */
	btlBmgData.pendingSecurityMode = securityMode;
	
	/* If security mode is unchanged, there is nothing to do */
	if ((modifyUnconditionally == FALSE) && (securityMode == btlBmgData.securityMode))
	{
		BTL_LOG_INFO(("Security Mode Unchanged, Successfully Completed Operation"));

		BTL_BMG_CompleteSetSecurityMode(0, BEC_NO_ERROR, FALSE, FALSE);
		BTL_RET(BT_STATUS_SUCCESS);
	}
	
	switch (securityMode)
	{
		case BSM_SEC_DISABLED:

			deRegisterSecHandlers = TRUE;
			disableMode3Security = TRUE;

			break;

		case BSM_SEC_LEVEL_1:
		case BSM_SEC_LEVEL_2:
			
			registerSecHandlers = TRUE;		
			disableMode3Security = TRUE;
			
			break;

		case BSM_SEC_LEVEL_3:
			
			registerSecHandlers = TRUE;	
			enableMode3Security = TRUE;
			encrypt = FALSE;
			
			break;

		case BSM_SEC_ENCRYPT:

			registerSecHandlers = TRUE;	
			enableMode3Security = TRUE;
			encrypt = TRUE;
			
			break;

		default:

			BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Invalid securityMode (%d)", securityMode));
			
	};

	if (TRUE == deRegisterSecHandlers)
	{
		BTL_BMG_EVM_DeRegisterSecHandlers();
	}
	else if ((TRUE == registerSecHandlers) && (FALSE == BTL_BMG_EVM_AreSecHandlersRegistered()))
	{
		BTL_BMG_EVM_RegisterSecHandlers();
	}

	status = BTL_BMG_HandleCompoundRequestCreation(	&request, 
														BTL_BMG_REQUEST_TYPE_SET_SECURITY_MODE,
														parentRequest,
														bmgContext, 
														BTL_BMG_InternalSetSecurityModeCb, 
														0);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	status = BTL_BMG_EVM_RegisterForStaticEvents(	setSecModeStaticEvents, 
													numOfSetSecModeStaticEvents,
													&request->base);
	if (BT_STATUS_SUCCESS != status)
	{
		BTL_BMG_CompleteSetSecurityMode(request, BEC_UNSPECIFIED_ERR, FALSE, FALSE);
		BTL_FATAL(BT_STATUS_INTERNAL_ERROR, (""));
	}

	/* 
		[@ToDo][Udi] - In case sec mode 3 enable / disable attempt fails, we should undo the sec handlers 
		registration / de-registration
	*/
	if (TRUE == enableMode3Security) 
	{		
		status = BTL_BMG_EnableSecurityMode3(request, encrypt);
	}
	else if (TRUE == disableMode3Security)
	{		
		status = BTL_BMG_DisableSecurityMode3(request);
	}
	else
	{
		status = BT_STATUS_SUCCESS;
	}

	/* Operation completed successfully, change the actual security mode */
	if (BT_STATUS_PENDING == status)
	{
		BTL_LOG_DEBUG(("Sec Mode 3 Changing in progress, will continue in handler"));
		btlBmgData.securityModeChangingInProgress = TRUE;
	}
	else if (BT_STATUS_SUCCESS == status)
	{
		BTL_LOG_DEBUG(("Security Mode Change Completed Successfully"));
		BTL_BMG_CompleteSetSecurityMode(request, BEC_NO_ERROR, FALSE, TRUE);
	}
	else
	{
		BTL_LOG_ERROR(("Security Mode Changing Failed"));
		BTL_BMG_CompleteSetSecurityMode(request, BEC_UNSPECIFIED_ERR, FALSE, TRUE);
	}

	BTL_FUNC_END();

	return status;
}

BtStatus BTL_BMG_SetSecurityMode(	BtlBmgContext 		*bmgContext, 
										const BtSecurityMode 	securityMode)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_SetSecurityMode");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR(FALSE == btlBmgData.securityModeChangingInProgress, BT_STATUS_IN_PROGRESS,
					("Security Mode Changing in Progress, wait for its completion and retry"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	BTL_VERIFY_ERR(((securityMode>=BSM_SEC_DISABLED) && (securityMode <= BSM_SEC_ENCRYPT)), BT_STATUS_INVALID_PARM, ("Invalid security mode"));

	status = BTL_BMG_InternalSetSecurityMode(bmgContext, &btlBmgData.masterPseudoRequest, securityMode, FALSE);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BMG_GetSecurityMode(	BtlBmgContext *bmgContext, 
										BtSecurityMode *securityMode)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_GetSecurityMode");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR(FALSE == btlBmgData.securityModeChangingInProgress, BT_STATUS_IN_PROGRESS,
					("Security Mode Changing in Progress, wait for its completion and retry"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	BTL_VERIFY_ERR((0 != securityMode), BT_STATUS_INVALID_PARM, ("Null securityMode"));

	*securityMode = btlBmgData.securityMode;
		
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_BMG_SetDeviceRecord(BtlBmgContext *bmgContext, 
										const BtlBmgDeviceRecord *record)
{
	BtStatus 	   status = BT_STATUS_SUCCESS;
	BtDeviceContext deviceContext;
	BtDeviceContext	*currDevice = 0;
	BtlBmgDeviceRecord recordToAdd;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_SetDeviceRecord");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != record), BT_STATUS_INVALID_PARM, ("Null Record"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	currDevice = DS_FindDevice(&record->bdAddr);

	OS_MemCopy((U8*)&recordToAdd, (U8*)record, sizeof(BtlBmgDeviceRecord));
	
	/* add device to ESI data base.*/
	if(currDevice == 0)
	{
		OS_MemSet((U8 *)&deviceContext, 0, sizeof(BtDeviceContext));
		deviceContext.addr = record->bdAddr;
		status = DS_AddDevice(&deviceContext, &currDevice);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status ), status, 
					("DS_AddDevice Failed (%s) for %s", pBT_Status(status), BTL_UTILS_LockedBdaddrNtoa(&(record->bdAddr)))); 
	}

	status = BTL_BMG_DDB_AddRecord(&recordToAdd);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
					("BTL_BMG_DDB_AddRecord Failed for %s, status: %d", BTL_UTILS_LockedBdaddrNtoa(&(record->bdAddr)), status)); 


	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BMG_FindDeviceRecord(BtlBmgContext *bmgContext, 
											const BD_ADDR *bdAddr, 
											BtlBmgDeviceRecord *record)
{
	BtStatus 	   status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_FindDeviceRecord");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((0 != record), BT_STATUS_INVALID_PARM, ("Null Record"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	status = BTL_BMG_DDB_FindRecord(bdAddr, record);

	if (status != BT_STATUS_SUCCESS)
	{
		BTL_LOG_INFO(("%s Not Found in Device DB", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));
		
		BTL_RET(BT_STATUS_DEVICE_NOT_FOUND);
	}
		
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BMG_GetNumberOfDeviceRecords(BtlBmgContext *bmgContext, 
										U32 *size)
{
	BtStatus 	   status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_GetNumberOfDeviceRecords");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != size), BT_STATUS_INVALID_PARM, ("Null size"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	status = BTL_BMG_DDB_GetNumOfDeviceRecords(size);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS== status), status, 
					("BTL_BMG_DDB_GetNumOfDeviceRecords Failed , status: %d", status)); 
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;

}

BtStatus BTL_BMG_EnumDeviceRecords(BtlBmgContext *bmgContext, 
											const U32 idx, 
											BtlBmgDeviceRecord *record)
{
	BtStatus 	   status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_EnumDeviceRecords");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != record), BT_STATUS_INVALID_PARM, ("Null record"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	status = BTL_BMG_DDB_EnumDeviceRecords(idx, record);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS== status), status, 
					("BTL_BMG_DDB_EnumDeviceRecords Failed, status: %d", status)); 
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BMG_InternalCreateLink(	BtlBmgContext 				*bmgContext,
										BtlBmgCompoundRequest 		*parentRequest,
										const BD_ADDR 				*bdAddr)
{
	BtStatus 					status = BT_STATUS_SUCCESS;
	BtlBmgDynamicRequest		*request = 0;
	BtDeviceContext 			*deviceContext = 0;
	BtPageScanInfo			psi;
	BtRemoteDevice			*remDev = 0;

	BTL_FUNC_START("BTL_BMG_InternalCreateLink");

	BTL_VERIFY_FATAL((0 != parentRequest), BT_STATUS_INTERNAL_ERROR, ("Null parent"));
	
	status = BTL_BMG_HandleDynamicRequestCreation(	&request, 
													BTL_BMG_REQUEST_TYPE_CREATE_LINK,
													parentRequest,
													bmgContext, 
													BTL_BMG_InternalCreateLinkCb,
													bdAddr,
													BTL_BMG_REQUEST_HANDLER_TYPE_BT_HANDLER);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	deviceContext = DS_FindDevice(bdAddr);

	if (0 != deviceContext)
       {
       	psi = deviceContext->psi;
	}
	else
	{
       	OS_MemSet((U8*)&psi, 0, sizeof(psi));
	}

	/* Call ESI to create the link*/
	status = ME_CreateLink(&request->token.btHandler, (BD_ADDR*)bdAddr, &psi, &remDev);
	
	if (BT_STATUS_PENDING == status)
	{
		/* Do Nothing - An event will be sent when the operation completes */
		BTL_LOG_DEBUG(("Waiting for Link to |%s|to be created", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));
	}
	else if (BT_STATUS_SUCCESS == status)
	{
		/* Link already exists, operation completed successfully */
		BTL_LOG_DEBUG(("Link already exists to |%s|", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));
	}
	else
	{
 		BTL_BMG_HandleRequestDestruction(&(request->base));

		/* This verify will fail, but will log and exit orderly */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status,
						("ME_CreateLink Failed, Status = %s", pBT_Status(status)));
	}	

	BTL_FUNC_END();
	

	return status;
}

BtStatus BTL_BMG_InternalDisconnectLink(BtlBmgRequest *request)
{
	BtStatus 					status = BT_STATUS_SUCCESS;
	BtRemoteDevice			*remDev = 0;
	BtlBmgDynamicRequest		*specificRequest = (BtlBmgDynamicRequest*)request;

	BTL_FUNC_START("BTL_BMG_InternalDisconnectLink");

	BTL_VERIFY_FATAL(	BTL_BMG_REQUEST_TYPE_CREATE_LINK == request->requestType,
						BT_STATUS_INTERNAL_ERROR, ("Invalid request type (%s)", 
						BTL_BMG_DEBUG_pRequestType(request->requestType)));
	
	BTL_LOG_INFO(("Disconnecting Link to %s", BTL_UTILS_LockedBdaddrNtoa(&request->bdAddress)));
	
	remDev = ME_FindRemoteDevice(&request->bdAddress);
	BTL_VERIFY_FATAL(0 != remDev, BT_STATUS_INTERNAL_ERROR, ("Failed to find the remote device"));
	
	/* Call ESI to disconnect the link*/
	status = ME_DisconnectLink(&specificRequest->token.btHandler, remDev);
	
	if (BT_STATUS_IN_PROGRESS == status)
	{
		BTL_LOG_INFO(("Link is in the process of coming up (peer initiation). Retry later"));
	}
	else if (BT_STATUS_FAILED == status)
	{
		BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("ME_DisconnectLink returned BT_STATUS_FAILED"));
	}
	else if (BT_STATUS_SUCCESS == status)
	{
		/* Link disconnected successfully */
		BTL_LOG_DEBUG(("Link to (%s) successfully disconnected", BTL_UTILS_LockedBdaddrNtoa(&request->bdAddress)));
	}
	else
	{
		BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("ME_DisconnectLink returned an Unexpected code (%s)", pBT_Status(status)));
	}

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_InternalSendHciCommand(	BtlBmgContext 			*bmgContext, 
														BtlBmgCompoundRequest	*parentRequest,
														const U16 				hciCommand,
														const U8 				parmsLen, 
														const U8 				*parms, 
														const U8 				event)
{
	BtStatus 					status = BT_STATUS_SUCCESS;
	MeCommandToken			*token = 0;
	BtlBmgDynamicRequest		*request = 0;
	U8						*parmsLocalCopy = 0;
	
	BTL_FUNC_START("BTL_BMG_InternalSendHciCommand");
	
	status = BTL_BMG_HandleDynamicRequestCreation(	&request, 
													BTL_BMG_REQUEST_TYPE_SEND_HCI_COMMAND,
													parentRequest,
													bmgContext, 
													BTL_BMG_InternalSendHciCommandCb, 
													0,
													BTL_BMG_REQUEST_HANDLER_TYPE_ME_TOKEN);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	/* Allocate memory for the HCI Command Parms */
	status = BTL_POOL_Allocate(&btlBmgData.hciParmsPool, (void **)(&parmsLocalCopy));
	
	if (BT_STATUS_SUCCESS != status)
	{
		BTL_BMG_HandleRequestDestruction(&(request->base));
		BTL_ERR(status, (""));
	}

	/* Zero all parms area - just to clean the parms area (helps debugging) */
	OS_MemSet(parmsLocalCopy, 0, HCI_CMD_PARM_LEN);

	if (parms != 0)
	{
		/* Copy the parms to a local copy that stays valid for the duration of the HCI command execution */
		OS_MemCopy(parmsLocalCopy, parms, parmsLen);
	}
	
	/* Save the parms pointer so we can release it when destroying the request */
	request->base.userData = parmsLocalCopy;

	token = &request->token.meCmdToken;

	/* Set HCI Command parameters */
	
	token->p.general.in.hciCommand = hciCommand;
	token->p.general.in.parmLen = parmsLen;
	token->p.general.in.parms = parmsLocalCopy;
	token->p.general.in.event = event;

	/* Send the HCI command */
	status = ME_SendHciCommandSync(token);

	if (BT_STATUS_PENDING != status)
	{
		/* Sending failed, free resources and report */

		BTL_POOL_Free(&btlBmgData.hciParmsPool, &(request->base.userData));
		BTL_BMG_HandleRequestDestruction(&(request->base));

		BTL_FATAL(status, ("ME_SendHciCommandSync Failed, status: %d", status));
	}
		
	BTL_FUNC_END();
	
	return status;
}


BtStatus BTL_BMG_CompleteSetAccessibility(const BtEvent	*event)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtAccessibleMode	mode;
	BtAccessModeInfo	info;

	BTL_FUNC_START("BTL_BMG_CompleteSetAccessibility");

	if (event != 0)
	{	
		if (btlBmgData.accessibilityData.pendingNCSettingInChip == TRUE)
		{
			ME_GetAccessibleModeNC(&mode, &info);
			
			btlBmgData.accessibilityData.accessibleModeNC = mode;
			btlBmgData.accessibilityData.accessModeInfoNC = info;
		}
		else
		{
			ME_GetAccessibleModeC(&mode, &info);
			
			btlBmgData.accessibilityData.accessibleModeC = mode;
			btlBmgData.accessibilityData.accessModeInfoC = info;
		}

		BTL_BMG_EVM_DispatchEvent(btlBmgData.accessibilityData.request, event,  BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);
	}

	if (btlBmgData.accessibilityData.request != 0)
	{
		BTL_BMG_EVM_DeRegisterFromStaticEvents(setAccessibilityStaticEvents, numOfSetAccessibilityStaticEvents, btlBmgData.accessibilityData.request);

		if (BTL_BMG_IsRequestTopLevel(btlBmgData.accessibilityData.request) == TRUE)
		{
			_BTL_BMG_HandleTopLevelProcessCompletion(btlBmgData.accessibilityData.request);
		}
	}

	BTL_FUNC_END();
	
	return status;
}

BOOL BTL_BMG_InternalSetAccessibleModeCb(const BtEvent *event, BtlBmgRequest *request)
{
	BTL_FUNC_START("BTL_BMG_InternalSetAccessibleModeCb");

	UNUSED_PARAMETER(request);
	
	BTL_BMG_CompleteSetAccessibility(event);
	
	BTL_FUNC_END();

	return TRUE;
}

BtStatus BTL_BMG_InternalSetAccessibleMode(	BtlBmgContext 			*bmgContext, 
													BtlBmgCompoundRequest	*parentRequest,
													const BtAccessibleMode 		*modeNC,
													const BtAccessModeInfo 	*infoNC,
													const BtAccessibleMode 		*modeC, 
													const BtAccessModeInfo 	*infoC)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus statusC = BT_STATUS_SUCCESS;
	BtStatus statusNC = BT_STATUS_SUCCESS;

	const BtAccessModeInfo	*infoNC1 = infoNC;
	const BtAccessModeInfo	*infoC1 = infoC;
	
	BTL_FUNC_START("BTL_BMG_InternalSetAccessibleMode");

	btlBmgData.accessibilityData.request = 0;

	/* First verify that the arguments are valid and that the operation can proceed within ESI */
	if (modeNC != 0)
	{
		if (infoNC == 0)
		{
			infoNC1 = &_btlDfltAccessibleModeInfoNC;
		}
		
		status = MeCheckAccessibleModeNC(*modeNC, infoNC1);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("Verification of Not Connected Setting Feasibility Failed (%s)", pBT_Status(status)));
	}

	if (modeC != 0)
	{
		if (infoC == 0)
		{
			infoC1 = &_btlDfltAccessibleModeInfoC;
		}
		
		status = MeCheckAccessibleModeC(*modeC, infoC1);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("Verification of Connected Setting Feasibility Failed (%s)", pBT_Status(status)));
	}

	/* It should be possible now to complete successfully */

	if (BTL_IsChipOn() == TRUE)
	{
		status = BTL_BMG_HandleRequestCreation(	&btlBmgData.accessibilityData.request, 
												BTL_BMG_REQUEST_TYPE_SET_ACCESSIBILITY,
												parentRequest,
												bmgContext, 
												BTL_BMG_InternalSetAccessibleModeCb, 
												0);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("Failed creating Set Accessibility Request (%s)", pBT_Status(status)));

		status = BTL_BMG_EVM_RegisterForStaticEvents(setAccessibilityStaticEvents, numOfSetAccessibilityStaticEvents, btlBmgData.accessibilityData.request);
		BTL_VERIFY_FATAL(status == BT_STATUS_SUCCESS, status, ("Failed Registering for Set Accessibility static Events (%s)", pBT_Status(status)));
	}
	
	if (modeNC != 0)
	{
		if (BTL_IsChipOn() == FALSE)
		{
			BTL_LOG_INFO(("Radio Off: NC Accessibility Mode Changed to %d", *modeNC));
			
			btlBmgData.accessibilityData.accessibleModeNC = *modeNC;
			btlBmgData.accessibilityData.accessModeInfoNC = *infoNC1;
		}
		else
		{
			/* Accessibility may be changed in the chip - attempt to change and update values upon completion */
			statusNC = MeSetAccessibleModeNC(*modeNC, infoNC1);
			BTL_VERIFY_FATAL((BT_STATUS_PENDING == statusNC) || (BT_STATUS_SUCCESS == statusNC), statusNC,
								("ME_SetAccessibleModeNC Failed, Status = %s", pBT_Status(statusNC)));
			
			if (statusNC == BT_STATUS_SUCCESS)
			{
				BTL_LOG_INFO(("NC Accessibility Mode Successfully changed to %d", *modeNC));
				
				btlBmgData.accessibilityData.accessibleModeNC = *modeNC;
				btlBmgData.accessibilityData.accessModeInfoNC = *infoNC1;		
			}
			else
			{
				btlBmgData.accessibilityData.pendingNCSettingInChip = TRUE;
				status = BT_STATUS_PENDING;
			}
		}
	}

	if (modeC != 0)
	{
		if (BTL_IsChipOn() == FALSE)
		{
			BTL_LOG_INFO(("Radio Off: C Accessibility Mode Changed to %d", *modeC));
			
			btlBmgData.accessibilityData.accessibleModeC = *modeC;
			btlBmgData.accessibilityData.accessModeInfoC = *infoC1;
		}
		else
		{
			/* Accessibility may be changed in the chip - attempt to change and update values upon completion */
			statusC = MeSetAccessibleModeC(*modeC, infoC1);
			BTL_VERIFY_FATAL((BT_STATUS_PENDING == statusC) || (BT_STATUS_SUCCESS == statusC), statusC,
								("ME_SetAccessibleModeNC Failed, Status = %s", pBT_Status(statusC)));


			if (statusC == BT_STATUS_SUCCESS)
			{
				BTL_LOG_INFO(("C Accessibility Mode Successfully changed to %d", *modeC));
				
				btlBmgData.accessibilityData.accessibleModeC = *modeC;
				btlBmgData.accessibilityData.accessModeInfoC = *infoC1;		
			}
			else
			{
				btlBmgData.accessibilityData.pendingNCSettingInChip = FALSE;
				status = BT_STATUS_PENDING;
			}
		}
	}
	
	/* One of them must complete successfully, only one may progress asynchronously */
	BTL_VERIFY_FATAL((statusC == BT_STATUS_SUCCESS) || (statusNC == BT_STATUS_SUCCESS), BT_STATUS_INTERNAL_ERROR,
						("Both Connected & Not-Connected processes unexpectedly progress asynchronously"))

	if ((statusC == BT_STATUS_PENDING) || (statusNC == BT_STATUS_PENDING))
	{
		status = BT_STATUS_PENDING;
	}
	else
	{
		BTL_VERIFY_FATAL(status == BT_STATUS_SUCCESS, status, ("status != BT_STATUS_SUCCESS (%s)", pBT_Status(status)));
		
		BTL_BMG_CompleteSetAccessibility(0);
	}
	
	BTL_FUNC_END();
	
	return status;
}

BtStatus _BTL_BMG_InternalSetClassOfDevice(BtClassOfDevice classOfDevice)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("_BTL_BMG_InternalSetClassOfDevice");
	
	status = ME_SetClassOfDevice(classOfDevice);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("ME_SetClassOfDevice Failed"));

	BTL_FUNC_END();
	
	return status;
}
#if 0
BtStatus _BTL_BMG_InternalSetPreferredConnectionRole(BtConnectionRole role)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("_BTL_BMG_InternalSetPreferredConnectionRole");
	
	ME_SetConnectionRole(role);

	BTL_FUNC_END();
	
	return status;
}
#endif
BtStatus BTL_BMG_ControlIncomingConnectionRequests(BtlBmgContext *bmgContext, BOOL enable)
{
	BtStatus 	status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_ControlIncomingConnectionRequests");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	status = BTL_BMG_InternalControlIncomingConnectionRequests(bmgContext, &btlBmgData.masterPseudoRequest, enable);
	
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BMG_InternalControlIncomingConnectionRequests(BtlBmgContext *bmgContext, 
													BtlBmgCompoundRequest  *parentRequest,
													BOOL enable)
{
	BtStatus 		status = BT_STATUS_SUCCESS;
	BtlBmgDynamicRequest		*request = 0;
	BOOL oldState;
	BtlBmgDynamicRequest	*executingRequest = 0;
	BOOL isTopLevel = FALSE;


	BTL_FUNC_START("BTL_BMG_InternalControlIncomingConnectionRequests");

	BTL_VERIFY_FATAL((0 != parentRequest), BT_STATUS_INTERNAL_ERROR, ("Null parent"));

	BTL_BMG_GetControlIncomingConnectionRequests(bmgContext, &oldState);

	if(enable == oldState)
	{
		BTL_RET(BT_STATUS_SUCCESS);
	}

	if(TRUE == enable)
	{
		status = BTL_BMG_HandleDynamicRequestCreation(	&request, 
												BTL_BMG_REQUEST_TYPE_SET_CONTROL_INCOMING_CON_ACCEPT,
												parentRequest,
												bmgContext, 
												BTL_BMG_InternalControlIncomingConnectionRequestsCB, 
												0,
												BTL_BMG_REQUEST_HANDLER_TYPE_BT_HANDLER);

		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

		ME_RegisterAcceptHandler(&request->token.btHandler);
	}
	else
	{
		MEC(acceptHandler) = 0;
		
		executingRequest = BTL_BMG_GetExecutingControlIncomConnRequest(bmgContext, parentRequest);
		
		BTL_VERIFY_ERR((0 != executingRequest), BT_STATUS_INTERNAL_ERROR, (""));

		isTopLevel = BTL_BMG_IsRequestTopLevel(&executingRequest->base);

		if (TRUE == isTopLevel)
		{
			_BTL_BMG_HandleTopLevelProcessCompletion(&executingRequest->base);
		}
	}
	
	BTL_FUNC_END();
	
	return status;


}

static BOOL BTL_BMG_InternalControlIncomingConnectionRequestsCB(const BtEvent *event, BtlBmgRequest *request)
{	
	BTL_FUNC_START("BTL_BMG_InternalSetLinkEncryptionCB");
				
	BTL_BMG_EVM_DispatchEvent(request, event,  BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);

	BTL_FUNC_END();

	return TRUE;
}

BtStatus BTL_BMG_GetControlIncomingConnectionRequests(BtlBmgContext *bmgContext, BOOL *answer)
{
	BtlBmgDynamicRequest	*executingRequest = 0;
	BtStatus 	status = BT_STATUS_SUCCESS;


	BTL_FUNC_START("BTL_BMG_GetControlIncomingConnectionRequests");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	executingRequest = BTL_BMG_GetExecutingControlIncomConnRequest(bmgContext, &btlBmgData.masterPseudoRequest);

	*answer = ((0 == executingRequest) ? FALSE : TRUE);

	BTL_FUNC_END();

	return TRUE;
}

BtStatus BTL_BMG_IncomingConnectionRequestReply(BtlBmgContext *bmgContext,
											 BD_ADDR* bdAddr,
											 BOOL accept,
											 BtErrorCode rejectErrorCode)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	BtConnectionRole role;
	BtRemoteDevice *remDev = 0;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_IncomingConnectionRequestReply");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));

	remDev = ME_FindRemoteDevice(bdAddr);

	BTL_VERIFY_ERR(0 != remDev, BT_STATUS_DEVICE_NOT_FOUND, 
					("No remDev Found (%s)", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));

	if (TRUE == accept)
	{
		role = MEC(connectionPolicy);

		if(role == BCR_ANY)
		{
			role = BCR_SLAVE;
		}
		
		status = ME_AcceptIncomingLink(remDev, role);
	}
	else
	{
		status = ME_RejectIncomingLink(remDev, rejectErrorCode);
	}

	BTL_VERIFY_ERR(BT_STATUS_PENDING == status, status, (""));

	BTL_FUNC_END_AND_UNLOCK();

	return status;


}

BtStatus BTL_BMG_DiscoverServices(	BtlBmgContext 				*bmgContext,
										const BD_ADDR 				*bdAddr,
								  		const SdpServicesMask 	sdpServicesMask)
{
	BtStatus 			status = BT_STATUS_PENDING;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_DiscoverServices");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR(SDP_SERVICE_NONE != sdpServicesMask, BT_STATUS_INVALID_PARM,
					("No services requested"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	/* Only a single process that requires SDP query may be in progress simultaneously */
	BTL_VERIFY_ERR(	FALSE == btlBmgData.sdpQueryInProgress,
						BT_STATUS_NO_RESOURCES, 
						("A Process that involves an SDP query is already in progress"));

	status = BTL_BMG_InternalDiscoverServices(	bmgContext,
											&btlBmgData.masterPseudoRequest,
											bdAddr,
										  	sdpServicesMask);
	BTL_VERIFY_ERR(BT_STATUS_PENDING == status, status, (""));

	btlBmgData.sdpQueryInProgress = TRUE;
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BMG_InternalDiscoverServices(	BtlBmgContext 				*bmgContext,
												BtlBmgCompoundRequest 		*parentRequest,
												const BD_ADDR 				*bdAddr,
										  		const SdpServicesMask 	sdpServicesMask)
{
	BtStatus					status = BT_STATUS_PENDING;
	SdpServicesMask	nextServiceToDiscover = SDP_SERVICE_NONE;
	BtlBmgPagingActivityPointerEntry *sdpDiscoveryEntry = 0;
	
	BTL_FUNC_START("BTL_BMG_InternalDiscoverServices");

	BTL_VERIFY_ERR((0 == ((sdpServicesMask) & 
						 (SDP_SERVICE_LAP | 
						 SDP_SERVICE_SYNC | 
						 SDP_SERVICE_CTP | 
						 SDP_SERVICE_ICP | 
						 SDP_SERVICE_PANU | 
						 SDP_SERVICE_NAP | 
						 SDP_SERVICE_GN |
						 SDP_SERVICE_HCRP))), BT_STATUS_INVALID_PARM, ("Invalid sdpServicesMask"));

	BTL_VERIFY_FATAL_NORET(btlBmgData.searchParams.discoverServicesParams.discoveryRequest == 0, ("discoverServicesParams.discoveryRequest is NOT NULL"));
	
	status = BTL_BMG_HandleRequestCreation(	&btlBmgData.searchParams.discoverServicesParams.discoveryRequest, 
											BTL_BMG_REQUEST_TYPE_DISCOVER_SERVICES,
											parentRequest,
											bmgContext, 
											0, 
											bdAddr);

	btlBmgData.searchParams.discoverServicesParams.servicesMask = sdpServicesMask;
	btlBmgData.searchParams.discoverServicesParams.discoveredServices = SDP_SERVICE_NONE;
	btlBmgData.searchParams.discoverServicesParams.currentServiceToDiscover = SDP_SERVICE_NONE;
	btlBmgData.searchParams.discoverServicesParams.discoveryCancelled = FALSE;
	
	/* Find the first service to discover */
	nextServiceToDiscover =  BTL_BMG_GetNextServiceToDiscover();
	BTL_VERIFY_FATAL(SDP_SERVICE_NONE != nextServiceToDiscover,
						BT_STATUS_INTERNAL_ERROR, ("No service to discover"));

	btlBmgData.searchParams.discoverServicesParams.currentServiceToDiscover = nextServiceToDiscover;
	
	/* Prepare the query parameters for this service */
	BTL_BMG_FillServiceDiscoveryQueryParms(nextServiceToDiscover);

	status = BTL_POOL_Allocate(&btlBmgData.pagingActivitiesPointersPool, (void **)&sdpDiscoveryEntry);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	(sdpDiscoveryEntry)->pagingActivityRequestPointer = btlBmgData.searchParams.discoverServicesParams.discoveryRequest;
	InsertTailList(&btlBmgData.pagingActivitiesPointersList, &((sdpDiscoveryEntry)->node));

	status = BTL_BMG_ExecuteNextPagingActivity();
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status), status, (""));

	status = BT_STATUS_PENDING;
	
	BTL_FUNC_END();

	return status;
}


BtStatus _BTL_BMG_CancelDiscoverServices(BtlBmgContext *bmgContext)
{
	BtStatus 			status = BT_STATUS_PENDING;
	
	BTL_FUNC_START("_BTL_BMG_CancelDiscoverServices");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	BTL_VERIFY_ERR(	TRUE == btlBmgData.sdpQueryInProgress,
						BT_STATUS_NOT_FOUND, 
						("The specified discovery process is not in progress"));

	status = BTL_BMG_InternalCancelServiceDiscovery();
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status)||(BT_STATUS_SUCCESS == status), status, (""));
	
	BTL_FUNC_END();

	return status;
}

BtStatus BTL_BMG_CancelDiscoverServices(BtlBmgContext *bmgContext)
{
	BtStatus 			status = BT_STATUS_PENDING;
	
	BTL_FUNC_START_AND_LOCK("BTL_BMG_CancelDiscoverServices");

	status = _BTL_BMG_CancelDiscoverServices(bmgContext);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BMG_InternalCancelServiceDiscovery(void)
{
	BtStatus status = BT_STATUS_PENDING;
	BTIPS_RS_Status sdpStatus = BTIPS_RS_STATUS_OK;
	BTL_FUNC_START("BTL_BMG_InternalCancelServiceDiscovery");

	btlBmgData.searchParams.discoverServicesParams.discoveryCancelled = TRUE;
	
	status = BTL_BMG_RemovePendingPagingActivity(btlBmgData.searchParams.discoverServicesParams.discoveryRequest);
		
	if (BT_STATUS_SUCCESS == status)
	{
		/* discovery hasn't started yet or already finished.*/
		btlBmgData.searchParams.discoverServicesParams.discoveryRequest = 0;
		BTL_RET(status);
	}
	else if (BT_STATUS_PENDING == status)
	{	
		sdpStatus = BTIPS_RS_CancelServiceSearchAttr();
		if (sdpStatus == BTIPS_RS_STATUS_OK)
		{
			/* ACL Link creation was cancelled */
			BTL_BMG_CompleteServiceDiscovery(BTEVENT_SERVICE_SEARCH_ATTRIBUTES_CANCELLED,
																						BEC_NO_ERROR,
																						FALSE);
			BTL_RET(BT_STATUS_SUCCESS);
		} /* else the ACL connection was already created therefor cancele event will be recived*/
		
	}
	else
	{
		BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("BTL_BMG_RemovePendingPagingActivity Failed (%s)", pBT_Status(status)));
	}

	BTL_FUNC_END();

	return status;
}

BtStatus BTL_BMG_SetUuid16(U16 sourceUuid, BtlBmgUuid *destUuid)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTL_BMG_SetUuid16");

	BTL_VERIFY_ERR((sourceUuid != 0), BT_STATUS_INVALID_PARM, ("invalid sourceUuid"));
	BTL_VERIFY_ERR((destUuid != 0), BT_STATUS_INVALID_PARM, ("invalid destUuid"));

	OS_MemCopy((U8*)destUuid, (const U8 *)BT_BASE_UUID, UUID_MAX_SIZE);

	StoreBE16((U8*)destUuid + 2, sourceUuid);			

	BTL_FUNC_END();

	return status;
}


BtStatus BTL_BMG_SetUuid32(U32 sourceUuid, BtlBmgUuid *destUuid)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTL_BMG_SetUuid16");

	BTL_VERIFY_ERR((sourceUuid != 0), BT_STATUS_INVALID_PARM, ("invalid sourceUuid"));
	BTL_VERIFY_ERR((destUuid != 0), BT_STATUS_INVALID_PARM, ("invalid destUuid"));

	OS_MemCopy((U8*)destUuid, (const U8 *)BT_BASE_UUID, UUID_MAX_SIZE);

	StoreBE32(destUuid->uuid, sourceUuid);			

	BTL_FUNC_END();

	return status;
}

BtStatus BTL_BMG_SetUuid128(U8* sourceUuid, BtlBmgUuid *destUuid)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTL_BMG_SetUuid128");

	BTL_VERIFY_ERR((sourceUuid != 0), BT_STATUS_INVALID_PARM, ("invalid sourceUuid"));
	BTL_VERIFY_ERR((destUuid != 0), BT_STATUS_INVALID_PARM, ("invalid destUuid"));

	OS_MemCopy((U8*)destUuid, (U8*)sourceUuid, UUID_MAX_SIZE);

	BTL_FUNC_END();

	return status;
}


BtStatus BTL_BMG_ServiceSearchAttributeRequest(BtlBmgContext *bmgContext,
                                  				const BD_ADDR *bdAddr,
												BtlBmgUuid* uuidList,
												U8  uuidListLen,
												U16	maxServiceRecords,
												SdpAttributeId *attributeIdList,
												U16 attributeIdListLen,
												U32 *requestId)
{
	BtStatus 			status = BT_STATUS_PENDING;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_ServiceSearchAttributeRequest");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((0 != requestId), BT_STATUS_INVALID_PARM, ("Null requestId"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	BTL_VERIFY_ERR((uuidListLen != 0), BT_STATUS_INVALID_PARM, ("Empty UUID list"));
	BTL_VERIFY_ERR((uuidList != 0), BT_STATUS_INVALID_PARM, ("NULL UUID list"));
	BTL_VERIFY_ERR((attributeIdListLen != 0), BT_STATUS_INVALID_PARM, ("Empty attr list"));			
	BTL_VERIFY_ERR((attributeIdList != 0), BT_STATUS_INVALID_PARM, ("NULL attr list"));					

	/* Only a single process that requires SDP query may be in progress simultaneously */
	BTL_VERIFY_ERR(	FALSE == btlBmgData.sdpQueryInProgress,
						BT_STATUS_NO_RESOURCES, 
						("A Process that involves an SDP query is already in progress"));

	status = BTL_BMG_InternalServiceSearchAttribute(bmgContext,
											&btlBmgData.masterPseudoRequest,
											bdAddr,
											uuidList,
											uuidListLen,
											maxServiceRecords,
											attributeIdList,
											attributeIdListLen,
										  	requestId);

	if(BT_STATUS_PENDING != status)
	{
		BTL_BMG_CompleteServiceSearchAttributes(BTEVENT_SERVICE_SEARCH_ATTRIBUTES_COMPLETE, BEC_SDP_INTERNAL_ERR, FALSE);

		BTL_RET(status);
	}

	btlBmgData.sdpQueryInProgress = TRUE;
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

void BTL_BMG_CompleteServiceAttributes(	BtEventType	completionEventType,
												BtErrorCode completionCode,
												BOOL sendCompletionEvent)
{
	BOOL isTopLevel = BTL_BMG_IsRequestTopLevel(btlBmgData.searchParams.serviceAttrParams.serviceAttrRequest);
	BtlBmgRequest tmpRequest;
	
	BTL_FUNC_START("BTL_BMG_CompleteServiceAttributes");

	BTL_BMG_PagingActivityEnded(btlBmgData.searchParams.serviceAttrParams.serviceAttrRequest);

	/* A work around in order to allow the application to initiate a subsequent sdp call.
	We have to destroy the request here. In case of top level requests, 
	the function BTL_BMG_EVM_DispatchEvent uses the request field for forwarding information 
	in this case we can supply it with a copy.
	assumption: userData field of tmpRequest is NULL*/
	btlBmgData.sdpQueryInProgress = FALSE;			
	OS_MemCopy(&tmpRequest, btlBmgData.searchParams.serviceAttrParams.serviceAttrRequest, sizeof(BtlBmgRequest));
	InitializeListEntry(&(tmpRequest.node));
	
	if (TRUE == isTopLevel)
	{
		_BTL_BMG_HandleTopLevelProcessCompletion(btlBmgData.searchParams.serviceAttrParams.serviceAttrRequest);
		btlBmgData.searchParams.serviceAttrParams.serviceAttrRequest = 0;
	}

	if (TRUE == sendCompletionEvent)
	{		
		btlBmgData.selfGeneratedEvent.eType = completionEventType;
		btlBmgData.selfGeneratedEvent.errCode = completionCode;
		btlBmgData.selfGeneratedEvent.p.bdAddr = tmpRequest.bdAddress;
		btlBmgData.selfGeneratedEvent.p.serviceAttributes.resAttrDataElement = &((U8 *)btlBmgData.sdpQueryResultsBuff)[0];

		BTL_BMG_EVM_DispatchEvent(&tmpRequest, 
									&btlBmgData.selfGeneratedEvent, 
									BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY);
	}

}

BtStatus BTL_BMG_InternalServiceSearchAttribute(BtlBmgContext *bmgContext,
												BtlBmgCompoundRequest *parentRequest,
												const BD_ADDR	*bdAddr,
												BtlBmgUuid* uuidList,
												U8  uuidListLen,
												U16	maxServiceRecords,
												SdpAttributeId *attributeIdList,
												U16 attributeIdListLen,
										  		U32 *requestId)
{
	BtStatus					status = BT_STATUS_PENDING;
	BtlBmgPagingActivityPointerEntry *sdpSearchEntry;
	
	BTL_FUNC_START("BTL_BMG_InternalServiceSearchAttribute");

	*requestId = 0;

	status = BTL_BMG_HandleRequestCreation(&btlBmgData.searchParams.serviceSearchAttrParams.serviceSearchAttrRequest, 
											BTL_BMG_REQUEST_TYPE_SERVICE_SEARCH_ATTR,
											parentRequest,
											bmgContext, 
											0, 
											bdAddr);

	btlBmgData.searchParams.serviceSearchAttrParams.serviceSearchAttrCancelled = FALSE;
	
	/* Prepare the query parameters */
	BTL_BMG_FillServiceSearchAttrQueryParms(uuidList,
																				uuidListLen,
																				maxServiceRecords,
																				attributeIdList,
																				attributeIdListLen);

	status = BTL_POOL_Allocate(&btlBmgData.pagingActivitiesPointersPool, (void **)&sdpSearchEntry);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	(sdpSearchEntry)->pagingActivityRequestPointer = btlBmgData.searchParams.serviceSearchAttrParams.serviceSearchAttrRequest;
	InsertTailList(&btlBmgData.pagingActivitiesPointersList, &((sdpSearchEntry)->node));
	
	status = BTL_BMG_ExecuteNextPagingActivity();
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status), status, (""));

	status = BT_STATUS_PENDING;
	
	BTL_FUNC_END();

	return status;
}

static void BTL_BMG_ServiceSearchAttributeRequestCb(U8* respBuff, BTIPS_RS_Status queryStatus)
{
	BtStatus        status = BT_STATUS_SUCCESS;
    BtErrorCode		errorCode;
	
	BTL_FUNC_START("BTL_BMG_ServiceSearchAttributeRequestCb");
	
	UNUSED_PARAMETER(respBuff);

	BTL_LOG_DEBUG(("SDP Query Status: %s", BTL_BMG_DEBUG_pSdpRetrieveStatus(queryStatus)));


	if(BTIPS_RS_STATUS_OPERATION_CANCELLED == queryStatus)
	{
		BTL_LOG_INFO(("Service Search Attributes Cancelled"));
		
		BTL_BMG_CompleteServiceSearchAttributes(BTEVENT_SERVICE_SEARCH_ATTRIBUTES_CANCELLED,
																							BEC_NO_ERROR,
																							TRUE);
		BTL_RET_NO_RETVAR();
		
	}
	else if(BTIPS_RS_STATUS_OPERATION_END == queryStatus)
	{
		BTL_BMG_CompleteServiceSearchAttributes(BTEVENT_SERVICE_SEARCH_ATTRIBUTES_COMPLETE,
																						BEC_NO_ERROR,
																						TRUE);
	}
	else if(BTIPS_RS_STATUS_OK == queryStatus)
	{
		btlBmgData.selfGeneratedEvent.eType = BTEVENT_SERVICE_SEARCH_ATTRIBUTES_RESULT;
		btlBmgData.selfGeneratedEvent.errCode = BEC_NO_ERROR;
		btlBmgData.selfGeneratedEvent.p.serviceSearchAttributes.bdAddr = btlBmgData.searchParams.serviceSearchAttrParams.serviceSearchAttrRequest->bdAddress;
		btlBmgData.selfGeneratedEvent.p.serviceSearchAttributes.resAttrDataElement = &((U8 *)btlBmgData.sdpQueryResultsBuff)[0];
		
		BTL_BMG_EVM_DispatchEvent(btlBmgData.searchParams.serviceSearchAttrParams.serviceSearchAttrRequest, 
									&btlBmgData.selfGeneratedEvent, 
									BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY);
	}
	else
	{
		/* SDP Error.*/
		BTL_LOG_INFO(("Service Search Attributes Request failed, status:", BTL_BMG_DEBUG_pSdpRetrieveStatus(queryStatus)));
		
		errorCode = BTL_BMG_MapSdpQueryStatusToEventErrorCode(queryStatus);
		
		BTL_BMG_CompleteServiceSearchAttributes(BTEVENT_SERVICE_SEARCH_ATTRIBUTES_COMPLETE,
																						errorCode,
																						TRUE);
	}

	BTL_FUNC_END();
}


static void BTL_BMG_ServiceSearchRequestCb(U8* respBuff, U16 handlesCount , BTIPS_RS_Status queryStatus)
{
	BtErrorCode				errorCode;
	
	BTL_FUNC_START("BTL_BMG_ServiceSearchRequestCb");
	
	UNUSED_PARAMETER(respBuff);

	BTL_LOG_DEBUG(("SDP Query Status: %s", BTL_BMG_DEBUG_pSdpRetrieveStatus(queryStatus)));

	
	if(BTIPS_RS_STATUS_OPERATION_CANCELLED == queryStatus)
	{

		BTL_LOG_INFO(("Service Search Cancelled"));
		
		BTL_BMG_CompleteServiceSearch(BTEVENT_SERVICE_SEARCH_CANCELLED,
																		handlesCount,
																		BEC_NO_ERROR,
																		TRUE);
	}
	/* queryStatus can be BTIPS_RS_STATUS_OPERATION_END, or error */
	else if(BTIPS_RS_STATUS_OPERATION_END == queryStatus)
	{
		BTL_BMG_CompleteServiceSearch(BTEVENT_SERVICE_SEARCH_COMPLETE,
																						handlesCount,
																						BEC_NO_ERROR,
																						TRUE);
	}
	else
	{
		/* SDP Error.*/
		BTL_LOG_INFO(("Service Search Request failed, status:", BTL_BMG_DEBUG_pSdpRetrieveStatus(queryStatus)));
		
		errorCode = BTL_BMG_MapSdpQueryStatusToEventErrorCode(queryStatus);
		
		BTL_BMG_CompleteServiceSearch(BTEVENT_SERVICE_SEARCH_COMPLETE,
																						0,
																						errorCode,
																						TRUE);
	}

	BTL_FUNC_END();
}

static void BTL_BMG_ServiceAttributeRequestCb(U8* respBuff, BTIPS_RS_Status queryStatus)
{
	BtErrorCode				errorCode;
	
	BTL_FUNC_START("BTL_BMG_ServiceAttributeRequestCb");
	
	UNUSED_PARAMETER(respBuff);

	BTL_LOG_DEBUG(("SDP Query Status: %s", BTL_BMG_DEBUG_pSdpRetrieveStatus(queryStatus)));

	
	if(BTIPS_RS_STATUS_OPERATION_CANCELLED == queryStatus)
	{
		
		BTL_LOG_INFO(("Service Search Cancelled"));
				
		BTL_BMG_CompleteServiceAttributes(BTEVENT_SERVICE_ATTRIBUTES_CANCELLED,
															BEC_NO_ERROR,
															TRUE);
	}
	/* queryStatus can be BTIPS_RS_STATUS_OPERATION_END, or error */
	else if(BTIPS_RS_STATUS_OPERATION_END == queryStatus)
	{
		BTL_BMG_CompleteServiceAttributes(BTEVENT_SERVICE_ATTRIBUTES_COMPLETE,
																	BEC_NO_ERROR,
																			TRUE);
	}
	else
	{
		/* SDP Error.*/
		BTL_LOG_INFO(("Service Attributes Request failed, status:", BTL_BMG_DEBUG_pSdpRetrieveStatus(queryStatus)));
		
		errorCode = BTL_BMG_MapSdpQueryStatusToEventErrorCode(queryStatus);
		
		BTL_BMG_CompleteServiceAttributes(BTEVENT_SERVICE_ATTRIBUTES_COMPLETE,
																						errorCode,
																						TRUE);
	}

	BTL_FUNC_END();

}

void BTL_BMG_CompleteServiceSearchAttributes(	BtEventType	completionEventType,
												BtErrorCode completionCode,
												BOOL sendCompletionEvent)
{
	BOOL isTopLevel = BTL_BMG_IsRequestTopLevel(btlBmgData.searchParams.serviceSearchAttrParams.serviceSearchAttrRequest);
	BtlBmgRequest tmpRequest;

	BTL_FUNC_START("BTL_BMG_CompleteServiceSearchAttributes");

	BTL_BMG_PagingActivityEnded(btlBmgData.searchParams.serviceSearchAttrParams.serviceSearchAttrRequest);

	/* A work around in order to allow the application to initiate a subsequent sdp call.
	We have to destroy the request here. In case of top level requests, 
	the function BTL_BMG_EVM_DispatchEvent uses the request field for forwarding information 
	in this case we can supply it with a copy.
	assumption: userData field of tmpRequest is NULL*/
	btlBmgData.sdpQueryInProgress = FALSE;			
	OS_MemCopy(&tmpRequest, btlBmgData.searchParams.serviceSearchAttrParams.serviceSearchAttrRequest, sizeof(BtlBmgRequest));
	InitializeListEntry(&(tmpRequest.node));				

	if (TRUE == isTopLevel)
	{
		_BTL_BMG_HandleTopLevelProcessCompletion(btlBmgData.searchParams.serviceSearchAttrParams.serviceSearchAttrRequest);
		btlBmgData.searchParams.serviceSearchAttrParams.serviceSearchAttrRequest = 0;
	}

	if (TRUE == sendCompletionEvent)
	{		
		btlBmgData.selfGeneratedEvent.eType = completionEventType;
		btlBmgData.selfGeneratedEvent.errCode = completionCode;
		btlBmgData.selfGeneratedEvent.p.bdAddr = tmpRequest.bdAddress;
		
		BTL_BMG_EVM_DispatchEvent(&tmpRequest, 
									&btlBmgData.selfGeneratedEvent, 
									BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY);
	}
	
}


void BTL_BMG_CompleteServiceSearch(	BtEventType	completionEventType,
												U16 handlesCount,
												BtErrorCode completionCode,
												BOOL sendCompletionEvent)
{
	BOOL isTopLevel = BTL_BMG_IsRequestTopLevel(btlBmgData.searchParams.serviceSearchParams.serviceSearchRequest);
	BtlBmgRequest tmpRequest;
	
	BTL_FUNC_START("BTL_BMG_CompleteServiceSearch");

	BTL_BMG_PagingActivityEnded(btlBmgData.searchParams.serviceSearchParams.serviceSearchRequest);

	/* A work around in order to allow the application to initiate a subsequent sdp call.
	We have to destroy the request here. In case of top level requests, 
	the function BTL_BMG_EVM_DispatchEvent uses the request field for forwarding information 
	in this case we can supply it with a copy.
	assumption: userData field of tmpRequest is NULL*/
	btlBmgData.sdpQueryInProgress = FALSE;
	OS_MemCopy(&tmpRequest, btlBmgData.searchParams.serviceSearchParams.serviceSearchRequest, sizeof(BtlBmgRequest));
	InitializeListEntry(&(tmpRequest.node));

	if (TRUE == isTopLevel)
	{
		_BTL_BMG_HandleTopLevelProcessCompletion(btlBmgData.searchParams.serviceSearchParams.serviceSearchRequest);
		btlBmgData.searchParams.serviceSearchParams.serviceSearchRequest = 0;
	}


	if (TRUE == sendCompletionEvent)
	{		
		btlBmgData.selfGeneratedEvent.eType = completionEventType;
		btlBmgData.selfGeneratedEvent.errCode = completionCode;
		btlBmgData.selfGeneratedEvent.p.bdAddr = tmpRequest.bdAddress;
		btlBmgData.selfGeneratedEvent.p.serviceSearch.recordHandlesList = &((U8 *)btlBmgData.sdpQueryResultsBuff)[0];
		btlBmgData.selfGeneratedEvent.p.serviceSearch.recordHandlesCount = handlesCount; 

		
		BTL_BMG_EVM_DispatchEvent(&tmpRequest, 
									&btlBmgData.selfGeneratedEvent, 
									BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY);
	}
	
}
BtStatus _BTL_BMG_CancelServiceSearchAttributeRequest(BtlBmgContext *bmgContext, 
													const U32 requestId)
{
	BtStatus 			status = BT_STATUS_PENDING;
	
	BTL_FUNC_START("_BTL_BMG_CancelServiceSearchAttributeRequest");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 == requestId), BT_STATUS_INVALID_PARM, ("Invalid Request Id"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	BTL_VERIFY_ERR(	TRUE == btlBmgData.sdpQueryInProgress,
						BT_STATUS_NOT_FOUND, 
						("The specified SDP process is not in progress"));

	status = BTL_BMG_InternalCancelServiceSearchAttributeRequest();

	BTL_VERIFY_ERR((BT_STATUS_PENDING == status)||(BT_STATUS_SUCCESS == status), status, (""));
	
	BTL_FUNC_END();

	return status;

}
BtStatus BTL_BMG_CancelServiceSearchAttributeRequest(BtlBmgContext *bmgContext, 
													const U32 requestId)
{
	BtStatus			status = BT_STATUS_PENDING;
	
	BTL_FUNC_START_AND_LOCK("BTL_BMG_CancelServiceSearchAttributeRequest");

	status = _BTL_BMG_CancelServiceSearchAttributeRequest(bmgContext, requestId);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;

}





BtStatus 	BTL_BMG_InternalCancelServiceSearchAttributeRequest(void)
{
	BtStatus status = BT_STATUS_PENDING;
	BTIPS_RS_Status sdpStatus = BTIPS_RS_STATUS_OK;

	BTL_FUNC_START("BTL_BMG_InternalCancelServiceSearchAttributeRequest");

	btlBmgData.searchParams.serviceSearchAttrParams.serviceSearchAttrCancelled = TRUE;
	

	if (0 == btlBmgData.searchParams.serviceSearchAttrParams.serviceSearchAttrRequest)
	{			
		BTL_LOG_INFO(("No service search request in progress"));
		BTL_RET(BT_STATUS_SUCCESS);
	}
	else
	{
		status = BTL_BMG_RemovePendingPagingActivity(btlBmgData.searchParams.serviceSearchAttrParams.serviceSearchAttrRequest);
		if (BT_STATUS_SUCCESS == status)
		{
			/* service search hasn't started yet or already finished.*/
			btlBmgData.searchParams.serviceSearchAttrParams.serviceSearchAttrRequest = 0;
			BTL_RET(status);
		}
		else if (BT_STATUS_PENDING == status)
		{	
			sdpStatus = BTIPS_RS_CancelServiceSearchAttr();
			if (sdpStatus == BTIPS_RS_STATUS_OK)
			{
				/* ACL Link creation was cancelled */
				BTL_BMG_CompleteServiceSearchAttributes(BTEVENT_SERVICE_SEARCH_ATTRIBUTES_CANCELLED,
																							BEC_NO_ERROR,
																							FALSE);
				BTL_RET(BT_STATUS_SUCCESS);
			} /* else the ACL connection was already created therefor cancele event will be recived*/
			
		}
		else
		{
			BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("BTL_BMG_RemovePendingPagingActivity Failed (%s)", pBT_Status(status)));
		}
		
	}


	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_ServiceSearchRequest(BtlBmgContext *bmgContext,
																			BD_ADDR *addr,
																			BtlBmgUuid* uuidList,
																			U8  numOfUuids,
																			U16	maxServiceRecords,
																			U32 *requestId)
{
	BtStatus 			status = BT_STATUS_PENDING;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_ServiceSearchRequest");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != addr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((0 != requestId), BT_STATUS_INVALID_PARM, ("Null requestId"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	BTL_VERIFY_ERR((numOfUuids != 0), BT_STATUS_INVALID_PARM, ("Empty UUID list"));
	BTL_VERIFY_ERR((uuidList != 0), BT_STATUS_INVALID_PARM, ("NULL UUID list"));
	BTL_VERIFY_ERR((maxServiceRecords != 0), BT_STATUS_INVALID_PARM, ("maxServiceRecords is '0'"));

	/* Only a single process that requires SDP query may be in progress simultaneously */
	BTL_VERIFY_ERR(	FALSE == btlBmgData.sdpQueryInProgress,
						BT_STATUS_NO_RESOURCES, 
						("A Process that involves an SDP query is already in progress"));

	status = BTL_BMG_InternalServiceSearchRequest(bmgContext,
											&btlBmgData.masterPseudoRequest,
											addr,
											uuidList,
											numOfUuids,
											maxServiceRecords,
										  requestId);

	if(BT_STATUS_PENDING != status)
	{
		BTL_BMG_CompleteServiceSearch(BTEVENT_SERVICE_SEARCH_COMPLETE, 0, BEC_SDP_INTERNAL_ERR, FALSE);

		BTL_RET(status);
	}

	btlBmgData.sdpQueryInProgress = TRUE;
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BMG_InternalServiceSearchRequest(BtlBmgContext *bmgContext,
												BtlBmgCompoundRequest *parentRequest,
												const BD_ADDR	*bdAddr,
												BtlBmgUuid* uuidList,
												U8  uuidListLen,
												U16	maxServiceRecords,
									  		U32 *requestId)
{
	BtStatus					status = BT_STATUS_PENDING;
	BtlBmgPagingActivityPointerEntry *sdpSearchEntry;
	
	BTL_FUNC_START("BTL_BMG_InternalServiceSearchRequest");

	*requestId = 0;

	status = BTL_BMG_HandleRequestCreation(&btlBmgData.searchParams.serviceSearchParams.serviceSearchRequest, 
											BTL_BMG_REQUEST_TYPE_SERVICE_SEARCH,
											parentRequest,
											bmgContext, 
											0, 
											bdAddr);

	
	btlBmgData.searchParams.serviceSearchParams.serviceSearchCancelled = FALSE;
	
	/* Prepare the query parameters */
	BTL_BMG_FillServiceSearchQueryParms(uuidList,
																				uuidListLen,
																				maxServiceRecords);

	status = BTL_POOL_Allocate(&btlBmgData.pagingActivitiesPointersPool, (void **)&sdpSearchEntry);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	(sdpSearchEntry)->pagingActivityRequestPointer = btlBmgData.searchParams.serviceSearchParams.serviceSearchRequest;
	InsertTailList(&btlBmgData.pagingActivitiesPointersList, &((sdpSearchEntry)->node));
	
	status = BTL_BMG_ExecuteNextPagingActivity();
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status), status, (""));

	status = BT_STATUS_PENDING;
	
	BTL_FUNC_END();

	return status;
}

BtStatus _BTL_BMG_CancelServiceSearchRequest(BtlBmgContext *bmgContext, 
													const U32 requestId)
{
	BtStatus			status = BT_STATUS_PENDING;
	
	BTL_FUNC_START("_BTL_BMG_CancelServiceSearchRequest");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 == requestId), BT_STATUS_INVALID_PARM, ("Invalid Request Id"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	BTL_VERIFY_ERR( TRUE == btlBmgData.sdpQueryInProgress,
						BT_STATUS_NOT_FOUND, 
						("The specified SDP process is not in progress"));

	status = BTL_BMG_InternalCancelServiceSearchRequest();

	BTL_VERIFY_ERR((BT_STATUS_PENDING == status)||(BT_STATUS_SUCCESS == status), status, (""));
	
	BTL_FUNC_END();

	return status;

}
BtStatus BTL_BMG_CancelServiceSearchRequest(BtlBmgContext *bmgContext, 
													 const U32 requestId)
{
	BtStatus			status = BT_STATUS_PENDING;
	
	BTL_FUNC_START_AND_LOCK("BTL_BMG_CancelServiceSearchAttributeRequest");

	status = _BTL_BMG_CancelServiceSearchRequest(bmgContext, requestId);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;

}




BtStatus	BTL_BMG_InternalCancelServiceSearchRequest(void)
{
	BtStatus status = BT_STATUS_PENDING;
	BTIPS_RS_Status sdpStatus = BTIPS_RS_STATUS_OK;

	BTL_FUNC_START("BTL_BMG_InternalCancelServiceSearchAttributeRequest");

	btlBmgData.searchParams.serviceSearchParams.serviceSearchCancelled = TRUE;

	if (0 == btlBmgData.searchParams.serviceSearchParams.serviceSearchRequest)
	{			
		BTL_LOG_INFO(("No service search request in progress"));
		BTL_RET(BT_STATUS_SUCCESS);
	}
	else
	{
		status = BTL_BMG_RemovePendingPagingActivity(btlBmgData.searchParams.serviceSearchParams.serviceSearchRequest);

		if(BT_STATUS_PENDING != status)
		{/* service search hasn't started yet or already finished.*/
			BTL_RET(status);
		}
		else if(BT_STATUS_PENDING == status)
		{
			sdpStatus = BTIPS_RS_CancelServiceSearch();
			if (sdpStatus == BTIPS_RS_STATUS_OK)
			{
				/* ACL Link creation was cancelled */
				BTL_BMG_CompleteServiceSearch(BTEVENT_SERVICE_SEARCH_CANCELLED,
																		0,
																		BEC_NO_ERROR,
																		FALSE);
				BTL_RET(BT_STATUS_SUCCESS);
			} /* else the ACL connection was already created therefor cancele event will be recived*/
			
		}
		else
		{
			BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("BTL_BMG_RemovePendingPagingActivity Failed (%s)", pBT_Status(status)));
		}
	}

	

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_ServiceAttributeRequest(BtlBmgContext *bmgContext,
									BD_ADDR *addr,
									U32 recordHandle,
									SdpAttributeId *attributeIdList,
									U16 attributeIdListLen,
									U32 *requestId)
{
	BtStatus 			status = BT_STATUS_PENDING;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_ServiceAttributeRequest");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != addr), BT_STATUS_INVALID_PARM, ("Null addr"));
	BTL_VERIFY_ERR((0 != requestId), BT_STATUS_INVALID_PARM, ("Null requestId"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	BTL_VERIFY_ERR((attributeIdListLen != 0), BT_STATUS_INVALID_PARM, ("Empty attr list"));			
	BTL_VERIFY_ERR((attributeIdList != 0), BT_STATUS_INVALID_PARM, ("NULL attr list"));					

	/* Only a single process that requires SDP query may be in progress simultaneously */
	BTL_VERIFY_ERR(	FALSE == btlBmgData.sdpQueryInProgress,
						BT_STATUS_NO_RESOURCES, 
						("A Process that involves an SDP query is already in progress"));

	status = BTL_BMG_InternalServiceAttrRequest(bmgContext,
											&btlBmgData.masterPseudoRequest,
											addr,
											recordHandle,
											attributeIdList,
											attributeIdListLen,
										  	requestId);

	if(BT_STATUS_PENDING != status)
	{
		BTL_BMG_CompleteServiceAttributes(BTEVENT_SERVICE_ATTRIBUTES_COMPLETE, BEC_SDP_INTERNAL_ERR, FALSE);

		BTL_RET(status);
	}

	btlBmgData.sdpQueryInProgress = TRUE;
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BMG_InternalServiceAttrRequest(BtlBmgContext *bmgContext,
												BtlBmgCompoundRequest *parentRequest,
												const BD_ADDR	*bdAddr,
												U32 recordHandle,
												SdpAttributeId *attributeIdList,
												U16 attributeIdListLen,
									  		U32 *requestId)
{
	BtStatus					status = BT_STATUS_PENDING;
	BtlBmgPagingActivityPointerEntry *sdpSearchEntry;
	
	BTL_FUNC_START("BTL_BMG_InternalServiceAttrRequest");

	*requestId = 0;

	status = BTL_BMG_HandleRequestCreation(&btlBmgData.searchParams.serviceAttrParams.serviceAttrRequest, 
											BTL_BMG_REQUEST_TYPE_SERVICE_ATTR,
											parentRequest,
											bmgContext, 
											0, 
											bdAddr);

	
	btlBmgData.searchParams.serviceAttrParams.serviceAttrCancelled = FALSE;
	
	/* Prepare the query parameters */
	BTL_BMG_FillServiceAttrQueryParms(recordHandle,
																				attributeIdList,
																				attributeIdListLen);

	status = BTL_POOL_Allocate(&btlBmgData.pagingActivitiesPointersPool, (void **)&sdpSearchEntry);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	(sdpSearchEntry)->pagingActivityRequestPointer = btlBmgData.searchParams.serviceAttrParams.serviceAttrRequest;
	InsertTailList(&btlBmgData.pagingActivitiesPointersList, &((sdpSearchEntry)->node));
	
	status = BTL_BMG_ExecuteNextPagingActivity();
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status), status, (""));

	status = BT_STATUS_PENDING;
	
	BTL_FUNC_END();

	return status;
}

BtStatus _BTL_BMG_CancelServiceAttributeRequest(BtlBmgContext *bmgContext, 
													  const U32 requestId)
{
	BtStatus			status = BT_STATUS_PENDING;
	
	BTL_FUNC_START("_BTL_BMG_CancelServiceSearchRequest");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 == requestId), BT_STATUS_INVALID_PARM, ("Invalid Request Id"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	BTL_VERIFY_ERR( TRUE == btlBmgData.sdpQueryInProgress,
						BT_STATUS_NOT_FOUND, 
						("The specified SDP process is not in progress"));

	status = BTL_BMG_InternalCancelServiceAttributeRequest();

	BTL_VERIFY_ERR((BT_STATUS_PENDING == status)||(BT_STATUS_SUCCESS == status), status, (""));
	
	BTL_FUNC_END();

	return status;

}
BtStatus BTL_BMG_CancelServiceAttributeRequest(BtlBmgContext *bmgContext, 
													const U32 requestId)
{
	BtStatus			status = BT_STATUS_PENDING;
	
	BTL_FUNC_START_AND_LOCK("BTL_BMG_CancelServiceSearchAttributeRequest");

	status = _BTL_BMG_CancelServiceAttributeRequest(bmgContext, requestId);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;

}




BtStatus	BTL_BMG_InternalCancelServiceAttributeRequest(void)
{
	BtStatus status = BT_STATUS_PENDING;
	BTIPS_RS_Status sdpStatus = BTIPS_RS_STATUS_OK;
	BTL_FUNC_START("BTL_BMG_InternalCancelServiceSearchAttributeRequest");

	btlBmgData.searchParams.serviceAttrParams.serviceAttrCancelled = TRUE;

	if (0 == btlBmgData.searchParams.serviceAttrParams.serviceAttrRequest)
	{			
		BTL_LOG_INFO(("No service search request in progress"));
		BTL_RET(BT_STATUS_SUCCESS);
	}
	else
	{
		status = BTL_BMG_RemovePendingPagingActivity(btlBmgData.searchParams.serviceAttrParams.serviceAttrRequest);

		if(BT_STATUS_PENDING != status)
		{/* service search hasn't started yet or already finished.*/
			BTL_RET(status);
		}
		else if(BT_STATUS_PENDING == status)
		{
			sdpStatus = BTIPS_RS_CancelServiceAttr();
			if (sdpStatus == BTIPS_RS_STATUS_OK)
			{
				/* ACL Link creation was cancelled */
				BTL_BMG_CompleteServiceAttributes(BTEVENT_SERVICE_SEARCH_CANCELLED,
																		BEC_NO_ERROR,
																		FALSE);
				BTL_RET(BT_STATUS_SUCCESS);
			} /* else the ACL connection was already created therefor cancele event will be recived*/
			
		}
		else
		{
			BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("BTL_BMG_RemovePendingPagingActivity Failed (%s)", pBT_Status(status)));
		}
	}

	

	BTL_FUNC_END();
	
	return status;
}



BtStatus BTL_BMG_RemoveServiceRecord(BtlBmgContext *bmgContext, U32 recordHandle)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL answer;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_RemoveServiceRecord");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));

	BTL_VERIFY_ERR((0 != recordHandle), BT_STATUS_INVALID_PARM, ("Null recordHandle"));

	/* Verify that recordHandle points at a record that was added via 
		BTL_BMG_SetServiceRecord */
	BTL_POOL_IsElelementAllocated(&btlBmgData.sdpServiceRecordsPool, (const void *)recordHandle, &answer);

	BTL_VERIFY_ERR(TRUE == answer, status, ("recordHandle 0x%x is not a modifiable recordHandle"));
		
	status = SDP_RemoveRecord((SdpRecord *)recordHandle);
 
	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, status, ("Failed removing SDP record"));

	/* Free the SdpRecord Entry in the pool */
	status = BTL_POOL_Free(&btlBmgData.sdpServiceRecordsPool,  (void **)(&recordHandle));

	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, status, ("Failed freeing record memory"));
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_BMG_EnumServiceRecords(BtlBmgContext *bmgContext, U32 *recordHandle)
{
	BtStatus status = BT_STATUS_SUCCESS;
	SdpRecord *curRecord;
	SdpRecord *nextRecord;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_EnumServiceRecords");
	
	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	
	BTL_VERIFY_ERR((0 != recordHandle), BT_STATUS_INVALID_PARM, ("Null recordHandle"));
	
	
	if(*recordHandle == 0)
	{
		BTL_VERIFY_ERR((FALSE == IsListEmpty(&SDPS(records))), BT_STATUS_NOT_FOUND, ("Service records list is empty"));
		
		/* First call initialize "recordHandle" to first item on list. */
		nextRecord = (SdpRecord *)GetHeadList(&SDPS(records));
	}
	else
	{
		curRecord = (SdpRecord *)(*recordHandle);
		
		/* Subsequent calls, make sure "recordHandle" is still on list. */
		BTL_VERIFY_ERR((TRUE == IsNodeOnList(&SDPS(records), &(curRecord)->node)), BT_STATUS_FAILED, ("recordHandle is not a valid service record"));
		
		/* Update nextRecord to point to next item on list. */
		nextRecord = (SdpRecord *)GetNextNode(&(curRecord)->node);
	}
	
	
	/* nextRecord now points to the current entry being returned. */
	if(&(nextRecord->node) == &SDPS(records))
	{
		*recordHandle = 0;
		BTL_ERR(BT_STATUS_NOT_FOUND, ("No more service records on the list"));
	}
	
	*recordHandle = (U32)nextRecord;
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BMG_GetServiceRecord(BtlBmgContext *bmgContext, 
								const U32 recordHandle, 
								BtClassOfDevice *cod, 
								SdpAttribute **attributeList,
								U32 *attributeListLen)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	SdpRecord* record = 0;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_GetServiceRecord");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != cod), BT_STATUS_INVALID_PARM, ("Null classOfDevice"));
	BTL_VERIFY_ERR((0 != attributeList), BT_STATUS_INVALID_PARM, ("Null attributeList"));
	BTL_VERIFY_ERR((0 != attributeListLen), BT_STATUS_INVALID_PARM, ("Null attributeListLen"));	
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	record = (SdpRecord *)recordHandle;

	BTL_VERIFY_ERR((TRUE ==IsNodeOnList(&SDPS(records), &(record->node))), 
					BT_STATUS_FAILED, ("Invalid record handle"));

	*cod = record->classOfDevice;
	*attributeList = record->attribs;
	*attributeListLen = (record->num) * sizeof(SdpAttribute);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_BMG_GetNumberOfServiceRecords(BtlBmgContext *bmgContext, U16 *numOfServiceRecords)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	U16 count = 0;
	SdpRecord *curRecord;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_GetClassOfDevice");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != numOfServiceRecords), BT_STATUS_INVALID_PARM, ("Null numOfServiceRecords"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	IterateList(SDPS(records), curRecord, SdpRecord*)
	{
		count++;
	}

	*numOfServiceRecords = count;
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}



BtStatus BTL_BMG_GetClassOfDevice(BtlBmgContext *bmgContext, BtClassOfDevice *classOfDevice)
{
	BtStatus 	status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_GetClassOfDevice");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != classOfDevice), BT_STATUS_INVALID_PARM, ("Null classOfDevice"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
				BT_STATUS_INVALID_PARM, ("Invalid context"));


	*classOfDevice = MEC(classOfDevice) | MEC(sdpServCoD);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BMG_GetBtVersion(BtlBmgContext *bmgContext, BtlBmgBtVersion *version)
{
	BtStatus 	status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_GetBtVersion");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != version), BT_STATUS_INVALID_PARM, ("Null version"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
				BT_STATUS_INVALID_PARM, ("Invalid context"));

	*version = (U8)min(ME_GetBtVersion(), STACK_BT_VERSION);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BMG_GetBtFeatures(BtlBmgContext *bmgContext, const U8 byteIdx, U8 *byteVal)
{
	BtStatus 	status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_GetBtFeatures");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != byteVal), BT_STATUS_INVALID_PARM, ("Null byteVal"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
				BT_STATUS_INVALID_PARM, ("Invalid context"));
	BTL_VERIFY_ERR((byteIdx <= 7), BT_STATUS_INVALID_PARM, ("invalid byteIdx"));

	*byteVal = ME_GetBtFeatures(byteIdx);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

#if 0
BtStatus BTL_BMG_SetPreferredConnectionRole(BtlBmgContext *bmgContext, 
											const BtConnectionRole role)
{
	BtStatus			status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_SetPreferredConnectionRole");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	BTL_VERIFY_ERR(((BCR_MASTER == role) || (BCR_ANY == role)), BT_STATUS_INVALID_PARM, ("role can be either MASTER or ANY"));

	status = _BTL_BMG_InternalSetPreferredConnectionRole(role);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));
	
	ME_SetConnectionRole(role);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_BMG_GetPreferredConnectionRole(BtlBmgContext *bmgContext, 
											BtConnectionRole *role)
{
	BtStatus			status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_GetPreferredConnectionRole");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	BTL_VERIFY_ERR((0 != role), BT_STATUS_INVALID_PARM, ("Null role"));

	*role = MEC(connectionPolicy);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;

}
#endif
BtStatus BTL_BMG_GetCurrentRole(BtlBmgContext *bmgContext, 
								const BD_ADDR *bdAddr, 
								BtConnectionRole *role)
{
	BtStatus			status = BT_STATUS_SUCCESS;
	BtRemoteDevice *remDev = 0;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_GetCurrentRole");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((0 != role), BT_STATUS_INVALID_PARM, ("Null role"));

	remDev = ME_FindRemoteDevice((BD_ADDR *)bdAddr);

	BTL_VERIFY_ERR(0 != remDev, BT_STATUS_DEVICE_NOT_FOUND, 
					("No remDev Found (%s)", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));
	
	
	*role = (BtConnectionRole)ME_GetCurrentRole(remDev);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;

}


BtStatus BTL_BMG_SwitchRole(BtlBmgContext *bmgContext, 
							const BD_ADDR *bdAddr)
{
	BtStatus			status = BT_STATUS_SUCCESS;
	BtRemoteDevice *remDev = 0;

	BTL_FUNC_START_AND_LOCK("BTL_BMG_SwitchRole");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));

	remDev = ME_FindRemoteDevice((BD_ADDR *)bdAddr);

	BTL_VERIFY_ERR(0 != remDev, BT_STATUS_DEVICE_NOT_FOUND, 
					("No remDev Found (%s)", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));
	
	status = ME_SwitchRole(remDev);

	BTL_VERIFY_ERR((BT_STATUS_PENDING != status), status, ("ME_SwitchRole returned %s", pBT_Status(status)));
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;

}



BtStatus BTL_BMG_InternalInquiry(	BtlBmgContext 			*bmgContext,
									BtlBmgCompoundRequest 	*parentRequest,
									const BtIac 				lap, 
									const U8 					length, 
									const U8 					maxResp)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtlBmgRequest	*request = 0;

	BTL_FUNC_START("BTL_BMG_InternalInquiry");
	
	status = BTL_BMG_HandleRequestCreation(	&request, 
											BTL_BMG_REQUEST_TYPE_INQUIRY,
											parentRequest,
											bmgContext, 
											BTL_BMG_InternalInquiryCb, 
											0);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));
	
	status = BTL_BMG_EVM_RegisterForStaticEvents(	inquiryStaticEvents, 
													numOfInquiryStaticEvents,
													request);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	status = ME_Inquiry(lap, length, maxResp);
	
	if (BT_STATUS_PENDING != status)
	{
		BTL_BMG_EVM_DeRegisterFromStaticEvents(	inquiryStaticEvents, 
													numOfInquiryStaticEvents,
													request);
		
		BTL_BMG_HandleRequestDestruction(request);

		/* This verify will fail, but will log and exit orderly */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("ME_Inquiry Failed (%s)", pBT_Status(status)));
	}	

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_InternalCancelInquiryRequest(	BtlBmgContext 			*bmgContext,
														BtlBmgCompoundRequest	*parentRequest)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtlBmgRequest	*executingInquiryRequest = 0;
	
	BTL_FUNC_START("BTL_BMG_InternalCancelInquiryRequest");
	
	/* Check if there is currently an executing inquiry request */
	executingInquiryRequest = BTL_BMG_GetExecutingInquiryRequest(bmgContext, parentRequest);

	/* If no matching inquiry request in progress, return with a success */
	if (0 == executingInquiryRequest)
	{
		BTL_LOG_INFO(("No inquiry request in progress "));
		BTL_RET(BT_STATUS_SUCCESS);
	}

	/* Request cancellation */
	status = ME_CancelInquiry();

	if (BT_STATUS_PENDING == status)
	{
		/* When the inquiry request is actually cancelled, an event will be sent to the handler */
	}
	else if (BT_STATUS_SUCCESS == status)
	{
		BTL_LOG_INFO(("ME_CancelInquiry Completed Cancellation"));
	}
	else
	{
		BTL_LOG_ERROR(("ME_CancelInquiry Failed, returned status: %s", pBT_Status(status)));
	}

	BTL_FUNC_END();
	
	return status;
}

BOOL BTL_BMG_InternalInquiryCb(const BtEvent *event, BtlBmgRequest *request)
{
	BOOL isTopLevel = BTL_BMG_IsRequestTopLevel(request);

	BTL_FUNC_START("BTL_BMG_InternalInquiryCb");

	BTL_BMG_EVM_DispatchEvent(request, 
								event,  
								BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY);


	if (	(BTEVENT_INQUIRY_COMPLETE == event->eType) ||
		(BTEVENT_INQUIRY_CANCELED == event->eType))
	{
		BTL_LOG_INFO(("BTL_BMG_InternalInquiryCb: Inquiry Completed"));
		
		BTL_BMG_EVM_DeRegisterFromStaticEvents(	inquiryStaticEvents, 
													numOfInquiryStaticEvents,
													request);

		if (TRUE == isTopLevel)
		{
			_BTL_BMG_HandleTopLevelProcessCompletion(request);
		}
	}
	else if (BTEVENT_INQUIRY_RESULT == event->eType)
	{
		BTL_LOG_DEBUG(("Inquiry result, NOT destroying request"));
	}
	else
	{
		BTL_ERR_NORET(("Invalid event type: %d", event->eType));
	}

	BTL_FUNC_END();

	return TRUE;
}

BtStatus BTL_BMG_InternalNameRequest(	BtlBmgContext 			*bmgContext,
											BtlBmgCompoundRequest  	*parentRequest,
											const BD_ADDR			*bdAddr)
{
	BtStatus 					status = BT_STATUS_SUCCESS;
	BtlBmgDynamicRequest		*request = 0;
	BtlBmgDynamicRequest		*executingNameRequest = 0;
	BtlBmgPagingActivityPointerEntry *nameEntry = 0;

	BTL_FUNC_START("BTL_BMG_InternalNameRequest");

	BTL_VERIFY_FATAL((0 != parentRequest), BT_STATUS_INTERNAL_ERROR, ("Null parent"));
	
	/* Check if there is already an executing name request for this context + BD Address */
	executingNameRequest = BTL_BMG_GetExecutingNameRequest(bmgContext, parentRequest, bdAddr);

	/* Reject an attempt to initiate a conflicting name request */
	if (0 != executingNameRequest)
	{
		BTL_LOG_INFO(("Only a single name request may progress simultaneously for the same {context,  BD Address}"));
		BTL_RET(BT_STATUS_IN_PROGRESS);
	}
	
	status = BTL_BMG_HandleDynamicRequestCreation(	&request, 
													BTL_BMG_REQUEST_TYPE_NAME_REQUEST,
													parentRequest,
													bmgContext, 
													BTL_BMG_InternalNameRequestCb, 
													bdAddr,
													BTL_BMG_REQUEST_HANDLER_TYPE_ME_TOKEN);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	status = BTL_POOL_Allocate(&btlBmgData.pagingActivitiesPointersPool, (void **)&nameEntry);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	InitializeListEntry(&nameEntry->node);
	
	nameEntry->pagingActivityRequestPointer = (BtlBmgRequest *)request;

	InsertTailList(&btlBmgData.pagingActivitiesPointersList, &nameEntry->node);

	status = BTL_BMG_ExecuteNextPagingActivity();
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status), status, (""));

	if(BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status)
	{
		status = BT_STATUS_PENDING;
	}

	BTL_FUNC_END();
	
	return status;
}
	
static BtStatus BTL_BMG_InternalCancelNameRequest(	BtlBmgContext 			*bmgContext,
														BtlBmgCompoundRequest	*parentRequest,
														const BD_ADDR			*bdAddr,
														BtlBmgDynamicRequest		*executingNameRequest)
{
	BtStatus 					status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_InternalCancelNameRequest");
	
	/* Check if there is already an executing name request for this context + BD Address */
	executingNameRequest = BTL_BMG_GetExecutingNameRequest(bmgContext, parentRequest, bdAddr);
	
	/* If no matching name request in progress, return with a success */
	if (0 == executingNameRequest)
	{			
		BTL_LOG_INFO(("No name request in progress for this {context,  BD Address}"));
		BTL_RET(BT_STATUS_SUCCESS);
	}
	else
	{
		status = BTL_BMG_RemovePendingPagingActivity((BtlBmgRequest *)executingNameRequest);

		if(BT_STATUS_SUCCESS == status)
		{
			BTL_RET(BT_STATUS_SUCCESS);
		}
	}

	/* Request cancellation, using the original token that is saved in the executing request */
	status = ME_CancelGetRemoteDeviceName(&executingNameRequest->token.meCmdToken);

	BTL_LOG_DEBUG(("ME_CancelGetRemoteDeviceName returned %s", pBT_Status(status)));

	if (BT_STATUS_PENDING == status)
	{
		/* When the name request is actually cancelled, an event will be sent to the handler */
	}
	else if (BT_STATUS_SUCCESS == status)
	{
		BTL_BMG_PagingActivityEnded((BtlBmgRequest *)executingNameRequest);

		BTL_LOG_INFO(("ME_CancelGetRemoteDeviceName Completed Cancellation"));
	}
	else
	{
		BTL_LOG_ERROR(("ME_CancelGetRemoteDeviceName Failed, returned status: %d", status));
	}

	BTL_FUNC_END();
	
	return status;
}

BOOL BTL_BMG_InternalCreateLinkCb(const BtEvent *event, BtlBmgRequest *request)
{	
	BTL_FUNC_START("BTL_BMG_InternalCreateLinkCb");
				
	BTL_BMG_EVM_DispatchEvent(request, event,  BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);

	BTL_FUNC_END();

	return TRUE;
}

BOOL BTL_BMG_InternalNameRequestCb(const BtEvent *event, BtlBmgRequest *request)
{
	BOOL isTopLevel = BTL_BMG_IsRequestTopLevel(request);
	BtlBmgDeviceRecord devRec;
	U8 len;

	BTL_FUNC_START("BTL_BMG_InternalNameRequestCb");

	/* save name in device DB if exists*/
	if (event->errCode == BEC_NO_ERROR)
	{
		if (BT_STATUS_SUCCESS == BTL_BMG_DDB_FindRecord(&event->p.meToken->p.name.bdAddr, &devRec))
		{
			len = (U8)min(event->p.meToken->p.name.io.out.len, BT_MAX_REM_DEV_NAME);
			OS_MemCopy((U8 *)(&(devRec.name[0])), event->p.meToken->p.name.io.out.name, len);
			devRec.name[len] = 0;
			BTL_BMG_DDB_AddRecord(&devRec);
		}
	}

	BTL_BMG_PagingActivityEnded(request);
	
	BTL_BMG_EVM_DispatchEvent(request, event,  BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY);
		
	if (TRUE == isTopLevel)
	{
		_BTL_BMG_HandleTopLevelProcessCompletion(request);
	}

	BTL_FUNC_END();

	return TRUE;
}

static void BTL_BMG_PagingActivityEnded(BtlBmgRequest *pagingRequestActivity)
{
	BtlBmgPagingActivityPointerEntry *curEntry = 0;
	BtlBmgPagingActivityPointerEntry *foundEntry = 0;

	BTL_FUNC_START("BTL_BMG_PagingActivityEnded");

	IterateList(btlBmgData.pagingActivitiesPointersList, curEntry, BtlBmgPagingActivityPointerEntry *)
	{
		if(curEntry->pagingActivityRequestPointer == pagingRequestActivity)
		{
			foundEntry = curEntry;
			break;
		}
	}

	if(foundEntry == 0)
	{
		BTL_FATAL_NORET(("No paging activity pointer entry was found"));
	}
	else
	{
		RemoveEntryList(&foundEntry->node);
		BTL_POOL_Free(&btlBmgData.pagingActivitiesPointersPool, (void**)&foundEntry);
		btlBmgData.pagingActivityInProgress = 0;

		/* invoke next paging activity.*/
		BTL_BMG_ExecuteNextPagingActivity();
	}

	BTL_FUNC_END();
}


static BtStatus BTL_BMG_RemovePendingPagingActivity(BtlBmgRequest *requestToRemove)
{
		BtlBmgPagingActivityPointerEntry *runningEntry = btlBmgData.pagingActivityInProgress; 
		BtlBmgPagingActivityPointerEntry *curEntry;
		BtlBmgRequest *runningRequest = runningEntry->pagingActivityRequestPointer;
		BtStatus status = BT_STATUS_FAILED;

		BTL_FUNC_START("BTL_BMG_RemovePendingPagingActivity");
		
		if (runningRequest == requestToRemove)
		{
			status = BT_STATUS_PENDING;
		}
		else
		{
			/* the request haven't started yet. find entry in the list and remove it.*/
			IterateList(btlBmgData.pagingActivitiesPointersList, curEntry, BtlBmgPagingActivityPointerEntry *)
			{
				if(curEntry->pagingActivityRequestPointer == requestToRemove)
				{
					RemoveEntryList(&curEntry->node); 
					BTL_POOL_Free(&btlBmgData.pagingActivitiesPointersPool, (void**)&curEntry);
					status = BT_STATUS_SUCCESS;	
					break;
				}
			}

			BTL_VERIFY_FATAL(status == BT_STATUS_SUCCESS, BT_STATUS_INTERNAL_ERROR, 
								("nding paging activity is not on the paging activities list"));
		}

		BTL_FUNC_END();

		return status;
}

BtStatus BTL_BMG_ExecuteNextPagingActivity(void)
{
	BtlBmgPagingActivityPointerEntry *nextEntry = 0;
	BtlBmgRequest	*nextRequest;
	MeCommandToken			*token = 0;
	BtDeviceContext 			*deviceContext = 0;
	BtStatus status = BT_STATUS_SUCCESS;
	BTIPS_RS_Status sdpStatus = BTIPS_RS_STATUS_OK;
	BOOL 	isTopLevel;

	BTL_FUNC_START("BTL_BMG_ExecuteNextPagingActivity");

	while(btlBmgData.pagingActivityInProgress == 0)
	{
		if(IsListEmpty(&btlBmgData.pagingActivitiesPointersList))
		{
			BTL_LOG_DEBUG(("No more paging activities requests to execute"));
			break;
		}
		else
		{
			nextEntry = (BtlBmgPagingActivityPointerEntry *)GetHeadList(&btlBmgData.pagingActivitiesPointersList);
			
			nextRequest = nextEntry->pagingActivityRequestPointer;

			BTL_LOG_INFO(("Next paging activitiy type: %d, bdAddr: %s", nextRequest->requestType, BTL_UTILS_LockedBdaddrNtoa(&nextRequest->bdAddress)));

			switch(nextRequest->requestType)
			{
			case BTL_BMG_REQUEST_TYPE_NAME_REQUEST:
				
				token = &(((BtlBmgDynamicRequest *)nextRequest)->token.meCmdToken);
				token->p.name.bdAddr = nextRequest->bdAddress;
			
				deviceContext = DS_FindDevice(&nextRequest->bdAddress);
			
				if (0 != deviceContext)
				{
					token->p.name.io.in.psi = deviceContext->psi;
				}
				else
				{
					OS_MemSet((U8*)&token->p.name.io.in.psi, 0, sizeof(token->p.name.io.in.psi));
				}
			
				status = ME_GetRemoteDeviceName(&(((BtlBmgDynamicRequest *)nextRequest)->token.meCmdToken));
				
				break;
				
			case 	BTL_BMG_REQUEST_TYPE_DISCOVER_SERVICES:
				BTL_LOG_DEBUG(("Calling BTIPS_RS_ServiceSearchAttr"));
	
				sdpStatus = BTIPS_RS_ServiceSearchAttr(&nextRequest->bdAddress, &btlBmgData.searchParams.discoverServicesParams.queryParms, TRUE);
				BTL_VERIFY_ERR(BTIPS_RS_STATUS_PENDING == sdpStatus, BT_STATUS_INTERNAL_ERROR, 
					("BTIPS_RS_ServiceSearchAttr Failed, sdpStatus: %d", sdpStatus));
				status = BT_STATUS_PENDING;
				break;
				
			case BTL_BMG_REQUEST_TYPE_SERVICE_SEARCH_ATTR:
					BTL_LOG_DEBUG(("Calling BTIPS_RS_ServiceSearchAttr"));
	
					sdpStatus = BTIPS_RS_ServiceSearchAttr(&nextRequest->bdAddress, &btlBmgData.searchParams.serviceSearchAttrParams.queryParms, FALSE);
					BTL_VERIFY_ERR(BTIPS_RS_STATUS_PENDING == sdpStatus, BT_STATUS_INTERNAL_ERROR, 
						("BTIPS_RS_ServiceSearchAttr Failed, sdpStatus: %d", sdpStatus));
				status = BT_STATUS_PENDING;
				break;	
				
			case BTL_BMG_REQUEST_TYPE_SERVICE_SEARCH:
					BTL_LOG_DEBUG(("Calling BTIPS_RS_ServiceSearch"));
	
					sdpStatus = BTIPS_RS_ServiceSearch(&nextRequest->bdAddress, &btlBmgData.searchParams.serviceSearchParams.queryParms);
					BTL_VERIFY_ERR(BTIPS_RS_STATUS_PENDING == sdpStatus, BT_STATUS_INTERNAL_ERROR, 
						("BTIPS_RS_ServiceSearch Failed, sdpStatus: %d", sdpStatus));
				status = BT_STATUS_PENDING;
				break;

				case BTL_BMG_REQUEST_TYPE_SERVICE_ATTR:
					BTL_LOG_DEBUG(("Calling BTIPS_RS_ServiceAttr"));
	
					sdpStatus = BTIPS_RS_ServiceAttr(&nextRequest->bdAddress, &btlBmgData.searchParams.serviceAttrParams.queryParms);
					BTL_VERIFY_ERR(BTIPS_RS_STATUS_PENDING == sdpStatus, BT_STATUS_INTERNAL_ERROR, 
						("BTIPS_RS_ServiceAttr Failed, sdpStatus: %d", sdpStatus));
					status = BT_STATUS_PENDING;
				break;
				
			default:
				BTL_FATAL(BT_STATUS_FAILED,("Unknown paging activity: %d", nextRequest->requestType));
			}
			
			if (BT_STATUS_PENDING == status)
			{
				btlBmgData.pagingActivityInProgress = nextEntry;
			}
			else if (BT_STATUS_SUCCESS == status)
			{	
				RemoveEntryList(&nextEntry->node);
				
				BTL_POOL_Free(&btlBmgData.pagingActivitiesPointersPool, (void**)&nextEntry);
				
				/* ?Gili: Shouldn't you call the appropriate completion function, depending on the type of operation? 
					It seems that the completion events of the whole process may not be called */
		
				isTopLevel = BTL_BMG_IsRequestTopLevel(nextRequest);		

				if (TRUE == isTopLevel)
				{
					_BTL_BMG_HandleTopLevelProcessCompletion(nextRequest);
				}
				
				/* This verify will fail, but will log and exit orderly */
				BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));
			}
			else
			{
				BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Paging activity %s failed (%s)", 
							BTL_BMG_DEBUG_pRequestType((U32)nextRequest->requestType), pBT_Status(status)));
			}
		}
	}
	BTL_FUNC_END();

	return status;
}

BOOL BTL_BMG_InternalSendHciCommandCb(const BtEvent *event, BtlBmgRequest *request)
{	
	BOOL 	isTopLevel = BTL_BMG_IsRequestTopLevel(request);
	void		*hciParms = request->userData;
	
	BTL_FUNC_START("BTL_BMG_InternalSendHciCommandCb");

	/* Set the fields of the self-generated event using the values of the original event */


	btlBmgData.selfGeneratedEvent = *event;
	
	btlBmgData.selfGeneratedEvent.eType = BTEVENT_SEND_HCI_COMMAND_RESULT;

	/* Assume operation succeeded */
	btlBmgData.selfGeneratedEvent.errCode = BEC_NO_ERROR;

	if (BT_STATUS_SUCCESS == event->p.meToken->p.general.out.status)
	{
		if (	(HCE_COMMAND_COMPLETE != event->p.meToken->p.general.out.event) &&
			 (HCE_COMMAND_STATUS != event->p.meToken->p.general.out.event))
		{
			BTL_LOG_ERROR(("HCI Command Failed, HCI Event: %d",
							event->p.meToken->p.general.out.event));
			btlBmgData.selfGeneratedEvent.errCode = BEC_UNSPECIFIED_ERR;
		}
		else if (HCE_COMMAND_COMPLETE == event->p.meToken->p.general.out.event)
		{
			if (event->p.meToken->p.general.out.parms[3] != 0)
			{
				BTL_LOG_ERROR(("HCI Command Failed, status: %x",
								event->p.meToken->p.general.out.parms[3]));
				btlBmgData.selfGeneratedEvent.errCode = event->p.meToken->p.general.out.parms[3];
			}
		}
	}
	else
	{
		BTL_LOG_ERROR(("HCI Command Failed, status: %s", pBT_Status(event->p.meToken->p.general.out.status)));
		btlBmgData.selfGeneratedEvent.errCode = BEC_UNSPECIFIED_ERR;
	}
	
	/* [@ToDo][Udi] This is destroyed too early */
	BTL_POOL_Free(&btlBmgData.hciParmsPool, &hciParms);

	request->userData = 0;
	
	BTL_BMG_EVM_DispatchEvent(request, 
								&btlBmgData.selfGeneratedEvent,  
								BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY);
	
	if (TRUE == isTopLevel)
	{
		_BTL_BMG_HandleTopLevelProcessCompletion(request);
	}

	BTL_FUNC_END();

	return TRUE;
}

void BTL_BMG_CompleteSearch(	BtlBmgSearchCommonData *searchData, 
										BtlBmgCompoundRequest	*specificRequest,
										BOOL					sendCompletionEvent)
{
	BTL_FUNC_START("BTL_BMG_CompleteSearch");

	if (TRUE == sendCompletionEvent)
	{
		BTL_VERIFY_FATAL_NO_RETVAR(0 != specificRequest, ("Null specificRequest"));
		
		if (TRUE == searchData->searchCancellationInProgress)
		{
			btlBmgData.selfGeneratedEvent.eType = BTEVENT_SEARCH_CANCELLED;
		}
		else
		{
			btlBmgData.selfGeneratedEvent.eType = BTEVENT_SEARCH_COMPLETE;
		}
			
		btlBmgData.selfGeneratedEvent.errCode = searchData->lastErrorCode;

		if(BT_STATUS_PENDING == searchData->lastErrorCode)
		{
			btlBmgData.selfGeneratedEvent.errCode = BT_STATUS_SUCCESS;
		}
		else if(BT_STATUS_SUCCESS != searchData->lastErrorCode &&
			BT_STATUS_INVALID_PARM != searchData->lastErrorCode &&
			BT_STATUS_IN_PROGRESS != searchData->lastErrorCode)
		{
			btlBmgData.selfGeneratedEvent.errCode = BEC_PARTIAL_SUCCESS;
		}
			
		BTL_BMG_EVM_DispatchEvent(&(specificRequest->base),
									&btlBmgData.selfGeneratedEvent,  
									BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);
	}

	
	if (searchData->sdpServicesMask != SDP_SERVICE_NONE)
	{
		btlBmgData.sdpQueryInProgress = FALSE;
	}

	
	if (0 != specificRequest)
	{
		_BTL_BMG_HandleTopLevelProcessCompletion(&(specificRequest->base));
	}
	
	btlBmgData.searchRequest = 0;

	BTL_FUNC_END();
}

void BTL_BMG_CompleteServiceDiscovery(	BtEventType				completionEventType,
												BtErrorCode 				completionCode,
												BOOL					sendCompletionEvent)
{
	BOOL 				isTopLevel = BTL_BMG_IsRequestTopLevel(btlBmgData.searchParams.discoverServicesParams.discoveryRequest);
	BtlBmgDeviceRecord 	devRec;
	BD_ADDR 			*addr = &(btlBmgData.searchParams.discoverServicesParams.discoveryRequest->bdAddress);
	BtlBmgRequest		*tempRequest = btlBmgData.searchParams.discoverServicesParams.discoveryRequest; 
	
	BTL_FUNC_START("BTL_BMG_CompleteServiceDiscovery");

	btlBmgData.searchParams.discoverServicesParams.discoveryRequest = 0;

	BTL_BMG_PagingActivityEnded(tempRequest);

	btlBmgData.sdpQueryInProgress = FALSE;

	if (TRUE == sendCompletionEvent)
	{	
		/* save services in device DB if exists*/
		if (BT_STATUS_SUCCESS == BTL_BMG_DDB_FindRecord(addr, &devRec))
		{
			devRec.services |= btlBmgData.searchParams.discoverServicesParams.discoveredServices;
			BTL_BMG_DDB_AddRecord(&devRec);
		}

		btlBmgData.selfGeneratedEvent.eType = completionEventType;
		btlBmgData.selfGeneratedEvent.errCode = completionCode;
		btlBmgData.selfGeneratedEvent.p.discoveredServices.discoveredServicesMask = btlBmgData.searchParams.discoverServicesParams.discoveredServices;
		btlBmgData.selfGeneratedEvent.p.bdAddr = *addr;

		BTL_BMG_EVM_DispatchEvent(tempRequest, 
									&btlBmgData.selfGeneratedEvent, 
									BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY);
	}
			
	if (TRUE == isTopLevel)
	{
		_BTL_BMG_HandleTopLevelProcessCompletion(tempRequest);
	}

	BTL_FUNC_END();
}
	
void BTL_BMG_InternalDiscoverServicesCb(U8* respBuff, BTIPS_RS_Status queryStatus)
{
	BtStatus        status = BT_STATUS_SUCCESS;
    SdpServicesMask	nextServiceToDiscover;
	BTIPS_RS_Status			sdpStatus;
	BtErrorCode				errorCode;
	BOOL					queryVerified = FALSE;
	
	BTL_FUNC_START("BTL_BMG_InternalDiscoverServicesCb");
	
	BTL_LOG_DEBUG(("SDP Query Status: %s", BTL_BMG_DEBUG_pSdpRetrieveStatus(queryStatus)));

	BTL_VERIFY_FATAL_NO_RETVAR(btlBmgData.searchParams.discoverServicesParams.discoveryRequest != 0, ("Null discoverServicesParams.discoveryRequest"));

	if (BTIPS_RS_STATUS_OPERATION_CANCELLED == queryStatus)
	{
		BTL_LOG_INFO(("Services Discovery Cancelled"));
		
		BTL_BMG_CompleteServiceDiscovery(	BTEVENT_DISCOVER_SERVICES_CANCELLED,
				BEC_NO_ERROR,
				TRUE);
		
		BTL_RET_NO_RETVAR();
	}
	else if((BTIPS_RS_STATUS_OK != queryStatus) && 
		(BTIPS_RS_STATUS_QUERY_EMPTY != queryStatus) &&
		(BTIPS_RS_STATUS_OPERATION_END != queryStatus))
	{
		/* SDP Error.*/
		BTL_LOG_INFO(("SDP Failed"));
		
		errorCode = BTL_BMG_MapSdpQueryStatusToEventErrorCode(queryStatus);
		
		BTL_BMG_CompleteServiceDiscovery(	BTEVENT_DISCOVER_SERVICES_RESULT,
			errorCode,
			TRUE);

		BTL_RET_NO_RETVAR();
	}	
	else if (BTIPS_RS_STATUS_OK == queryStatus)
	{
		BTL_BMG_VerifyServiceDiscoveryQueryResponse(respBuff, &queryVerified);
		
		if (TRUE == queryVerified)
		{
			/* Remote device supports the current discovered service, add it to the mask */
			btlBmgData.searchParams.discoverServicesParams.discoveredServices |= 
				btlBmgData.searchParams.discoverServicesParams.currentServiceToDiscover;
		}
		
		BTL_LOG_DEBUG(("|%s| %s Supports %s", 
			BTL_UTILS_LockedBdaddrNtoa(&btlBmgData.searchParams.discoverServicesParams.discoveryRequest->bdAddress),
			(TRUE == queryVerified) ? ("") : ("Doesn't"),
			pSdpServicesMask(btlBmgData.searchParams.discoverServicesParams.currentServiceToDiscover)));

		/* a BTIPS_RS_STATUS_OPERATION_END will follow.*/
		BTL_RET_NO_RETVAR();
	}
	
	/* (BTIPS_RS_STATUS_OPERATION_END == queryStatus) || (BTIPS_RS_STATUS_QUERY_EMPTY == queryStatus).*/
	/* operation ended - continue to next service.*/
	BTL_LOG_DEBUG(("SDP Query operation end"));
	
	/* Find the next service to discover */
	nextServiceToDiscover =  BTL_BMG_GetNextServiceToDiscover();
	
	if (SDP_SERVICE_NONE != nextServiceToDiscover)
	{
		/* There are more services to discover - handle the next one */
		BTL_LOG_DEBUG(("Next service to discover is: |%s|", pSdpServicesMask(nextServiceToDiscover)));
		
		btlBmgData.searchParams.discoverServicesParams.currentServiceToDiscover = nextServiceToDiscover;
		
		/* Prepare the query parameters for this service */
		BTL_BMG_FillServiceDiscoveryQueryParms(nextServiceToDiscover);

		BTL_LOG_DEBUG(("Calling BTIPS_RS_ServiceSearchAttr"));
		
		sdpStatus = BTIPS_RS_ServiceSearchAttr(	&btlBmgData.searchParams.discoverServicesParams.discoveryRequest->bdAddress,
			&btlBmgData.searchParams.discoverServicesParams.queryParms, TRUE);
		
		if (BTIPS_RS_STATUS_PENDING != sdpStatus) 
		{
			BTL_LOG_ERROR(("BTIPS_RS_ServiceSearchAttr Failed, sdpStatus: %d", sdpStatus));

			errorCode = BTL_BMG_MapSdpQueryStatusToEventErrorCode(queryStatus);

			BTL_BMG_CompleteServiceDiscovery(	BTEVENT_DISCOVER_SERVICES_RESULT,
			errorCode,
			TRUE);

			BTL_RET_NO_RETVAR();
		}

	}
	else
	{
		/* Checked all reuqeted services, report results to parent */
		
		BTL_LOG_DEBUG(("No more services to discover"));
		
		/* [@ToDo][Udi] - Only checking last query result */
		errorCode = BTL_BMG_MapSdpQueryStatusToEventErrorCode(queryStatus);
		
		BTL_BMG_CompleteServiceDiscovery(	BTEVENT_DISCOVER_SERVICES_RESULT,
			errorCode,
			TRUE);
	}
		
	BTL_FUNC_END();
}

static BtStatus BTL_BMG_HandleRequestCreation(	BtlBmgRequest			**request,
													BtlBmgRequestType		requestType,
													BtlBmgCompoundRequest	*parentRequest,
													BtlBmgContext			*bmgContext,
													BtlBmgEvmCallBack		bmgCallback,
													const BD_ADDR			*bdAddress)
{
	BtStatus 					status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_HandleRequestCreation");

	BTL_VERIFY_FATAL((0 != parentRequest), BT_STATUS_INTERNAL_ERROR, ("Null parent"));
	
	/* Allocate memory for the new context */
	status = BTL_POOL_Allocate(&btlBmgData.requestsPool, (void **)request);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	/* Initialize the request's members */
	status = BTL_BMG_REQUEST_Create(	*request, 
										requestType,
										parentRequest,
										bmgContext, 
										bmgCallback, 
										bdAddress);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));
	
	/* Add the request to the requests list */
	InsertTailList(&(parentRequest->childRequests), &(((*request)->node)));
	
	BTL_FUNC_END();
	
	return status;
}

static BtStatus BTL_BMG_HandleDynamicRequestCreation(	
				BtlBmgDynamicRequest		**request,
				BtlBmgRequestType		requestType,
				BtlBmgCompoundRequest	*parentRequest,
				BtlBmgContext			*bmgContext,
				BtlBmgEvmCallBack		bmgCallback,
				const BD_ADDR			*bdAddress,
				BtlBmgRequestHandlerType	handlerType)
{
	BtStatus 					status = BT_STATUS_SUCCESS;
	BtCallBack				handlerCallback = 0;

	BTL_FUNC_START("BTL_BMG_HandleDynamicRequestCreation");

	BTL_VERIFY_FATAL((0 != parentRequest), BT_STATUS_INTERNAL_ERROR, ("Null parent"));
	
	/* Allocate memory for the new context */
	status = BTL_POOL_Allocate(&btlBmgData.requestsPool, (void **)request);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed to allocate request"));

	handlerCallback = BTL_BMG_EVM_GetHandler(handlerType);
	BTL_VERIFY_FATAL((0 != *handlerCallback), BT_STATUS_INTERNAL_ERROR, (""));

	/* Initialize the request's members */
	status = BTL_BMG_DYNAMIC_REQUEST_Create(	*request, 
												requestType,
												parentRequest,
												bmgContext, 
												bmgCallback, 
												bdAddress,
												handlerType,
												handlerCallback);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));
	
	/* Add the request to the requests list */
	InsertTailList(&(parentRequest->childRequests), &(((*request)->base.node)));
		
	BTL_FUNC_END();
	
	return status;
}

static BtStatus BTL_BMG_HandleCompoundRequestCreation(	
													BtlBmgCompoundRequest	**request,
													BtlBmgRequestType		requestType,
													BtlBmgCompoundRequest	*parentRequest,
													BtlBmgContext			*bmgContext,
													BtlBmgEvmCallBack		bmgCallback,
													const BD_ADDR			*bdAddress)
{
	BtStatus 					status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_HandleCompoundRequestCreation");
	
	BTL_VERIFY_FATAL((0 != parentRequest), BT_STATUS_INTERNAL_ERROR, ("Null parent"));
	
	/* Allocate memory for the new context */
	status = BTL_POOL_Allocate(&btlBmgData.requestsPool, (void **)request);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	/* Initialize the request's members */
	status = BTL_BMG_COMPOUND_REQUEST_Create(*request, 
												requestType,
												parentRequest,
												bmgContext, 
												bmgCallback, 
												bdAddress);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

	/* Add the request to the requests list */
	InsertTailList(&(parentRequest->childRequests), &(((*request)->base.node)));
	
	BTL_FUNC_END();
	
	return status;
}

void BTL_BMG_HandleRequestDestruction(BtlBmgRequest	*request)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	BOOL	isAllocated = FALSE;
	
	BTL_FUNC_START("BTL_BMG_HandleRequestDestruction");

	status = BTL_POOL_IsElelementAllocated(&btlBmgData.requestsPool, request, &isAllocated);
	BTL_VERIFY_FATAL_NO_RETVAR(BT_STATUS_SUCCESS == status, (""));
	BTL_VERIFY_FATAL_NO_RETVAR(TRUE == isAllocated, ("request has already been de-allocated"));
	
	/* Remove the request from the requests list */
	RemoveEntryList(&(request->node));

	BTL_BMG_REQUEST_CleanVirtFunc(request, BTL_BMG_FreeRequest);
	
	BTL_FUNC_END();
}

void _BTL_BMG_HandleTopLevelProcessCompletion(BtlBmgRequest	*request)
{
	
	BTL_FUNC_START("_BTL_BMG_HandleTopLevelProcessCompletion");

	BTL_LOG_DEBUG(("%s Completed", BTL_BMG_DEBUG_pRequestType(request->requestType)));
	
	if (btlBmgData.radioOffData.state != _BTL_BMG_RADIO_OFF_STATE_NONE)
	{
		_BTL_BMG_RadioOffProcessor(_BTL_BMG_RADIO_OFF_EVENT_PROCESS_COMPLETED, request);
	}
	
	BTL_BMG_HandleRequestDestruction(request);

	BTL_FUNC_END();
}

BtStatus BTL_BMG_FreeRequest(void** requestAsVoid)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	status = BTL_POOL_Free(&btlBmgData.requestsPool, requestAsVoid);
	
	return status;
}

/*
	Checks if a request is not contained by any other request
*/
BOOL BTL_BMG_IsRequestTopLevel(const BtlBmgRequest *request)
{
	BOOL answer = FALSE;
	
	BTL_FUNC_START("BTL_BMG_IsRequestTopLevel");
	
	/* Top level requests are contained by the pseudo-request */
	if (&btlBmgData.masterPseudoRequest != request->parent)
	{
		answer = FALSE;
	}
	else
	{
		answer = TRUE;
	}

	BTL_LOG_DEBUG(("Request is %s a Top Level Request", (TRUE == answer) ? ("") : ("Not")));
	
	 BTL_FUNC_END();

	 return answer;
}

/*
	Checks if requestToMatch matches a request contained by parentRequest. The first matching request 
	is returned (if any)
*/
BtlBmgRequest *BTL_BMG_FindMatchingRequest(	BtlBmgCompoundRequest 	*parentRequest, 
													BtlBmgRequest			*requestToMatch)
{
	BtlBmgRequest	*cur = 0;
	BtlBmgRequest	*matchingRequest = 0;

	BTL_FUNC_START("BTL_BMG_FindMatchingRequest");

	BTL_VERIFY_FATAL_SET_RETVAR((0 != parentRequest), matchingRequest = 0, ("Null parent"));

	/* Iterate over the contained requests, check for a match, and exit upon the first match found */
	IterateList(parentRequest->childRequests, cur, BtlBmgRequest*)
	{
		if (TRUE == BTL_BMG_REQUEST_IsMatchingVirtFunc(cur, requestToMatch))
		{
			/* Found a match, record and stop iterating */
			matchingRequest = cur;
			break;
		}
	}

	BTL_LOG_DEBUG(("%s Matching Request", (0 != matchingRequest) ? ("Found") : ("Didn't find")));

	BTL_FUNC_END();
	
	return matchingRequest;
}

BtlBmgRequest *BTL_BMG_GetExecutingRequestByType(
							const BtlBmgContext		*bmgContext,
							BtlBmgCompoundRequest  	*parentRequest,
							BtlBmgRequestType		requestType)
{
	BtStatus				status = BT_STATUS_SUCCESS;
	BtlBmgRequest		*executingRequest = 0;
	BtlBmgRequest		templateRequest;

	BTL_FUNC_START("BTL_BMG_GetExecutingRequestByType");
	
	/* Initialize the template request's members */
	status = BTL_BMG_REQUEST_Create(	&templateRequest, 
										requestType,
										0,
										(BtlBmgContext*)bmgContext, 
										0, 
										0);
	BTL_VERIFY_FATAL_SET_RETVAR((BT_STATUS_SUCCESS == status), executingRequest = 0, (""));

	executingRequest = BTL_BMG_FindMatchingRequest(parentRequest, &templateRequest);
			
	BTL_FUNC_END();

	return executingRequest;

}

BtlBmgRequest *BTL_BMG_GetExecutingRequestByTypeAndAddress(
							const BtlBmgContext		*bmgContext,
							BtlBmgCompoundRequest  	*parentRequest,
							BtlBmgRequestType		requestType,
							const BD_ADDR			*bdAddress)
{
	BtStatus				status = BT_STATUS_SUCCESS;
	BtlBmgRequest		*executingRequest = 0;
	BtlBmgRequest		templateRequest;

	BTL_FUNC_START("BTL_BMG_GetExecutingRequestByTypeAndAddress");
		
	/* Initialize the template request's members */
	status = BTL_BMG_REQUEST_Create(	&templateRequest, 
										requestType,
										0,
										(BtlBmgContext*)bmgContext, 
										0, 
										bdAddress);
	BTL_VERIFY_FATAL_SET_RETVAR((BT_STATUS_SUCCESS == status), executingRequest = 0, (""));

	executingRequest = BTL_BMG_FindMatchingRequest(parentRequest, &templateRequest);
	
	BTL_FUNC_END();

	return executingRequest;

}

BtlBmgRequest *BTL_BMG_GetExecutingInquiryRequest(
							const BtlBmgContext		*bmgContext,
							BtlBmgCompoundRequest  	*parentRequest)
{
	return BTL_BMG_GetExecutingRequestByType(	bmgContext,
												parentRequest,
												BTL_BMG_REQUEST_TYPE_INQUIRY);
}

BtlBmgDynamicRequest *BTL_BMG_GetExecutingNameRequest(
							const BtlBmgContext		*bmgContext,
							BtlBmgCompoundRequest  	*parentRequest,
							const BD_ADDR			*bdAddress)
{
	return ((BtlBmgDynamicRequest*)
				BTL_BMG_GetExecutingRequestByTypeAndAddress(	bmgContext,
																parentRequest,
																BTL_BMG_REQUEST_TYPE_NAME_REQUEST,
																bdAddress));
	
}

static BtlBmgDynamicRequest *BTL_BMG_GetExecutingControlIncomConnRequest	(
								const BtlBmgContext		*bmgContext,
								BtlBmgCompoundRequest  	*parentRequest)
{
	return ((BtlBmgDynamicRequest*)
				BTL_BMG_GetExecutingRequestByType(bmgContext,
												parentRequest,
												BTL_BMG_REQUEST_TYPE_SET_CONTROL_INCOMING_CON_ACCEPT));
}


BtlBmgDynamicRequest *BTL_BMG_GetExecutingConnectRequest(
							const BtlBmgContext		*bmgContext,
							BtlBmgCompoundRequest  	*parentRequest,
							const BD_ADDR			*bdAddress)
{
	return ((BtlBmgDynamicRequest*)
				BTL_BMG_GetExecutingRequestByTypeAndAddress(	bmgContext,
																parentRequest,
																BTL_BMG_REQUEST_TYPE_CREATE_LINK,
																bdAddress));
	
}	

BtlBmgCompoundRequest *BTL_BMG_GetExecutingBondRequest(	const BtlBmgContext		*bmgContext,
																	BtlBmgCompoundRequest  	*parentRequest,
																	const BD_ADDR			*bdAddress)
{
	return ((BtlBmgCompoundRequest*)
				BTL_BMG_GetExecutingRequestByTypeAndAddress(	bmgContext,
																parentRequest,
																BTL_BMG_REQUEST_TYPE_BOND,
																bdAddress));
}

/*
	Checks if the proposed name & len are valid as the new local device name:
	1. Name must not be null
	2. name length must be len <= BTL_BMG_MAX_NAME_LOCAL_LEN
	3. If len < BTL_BMG_MAX_LOCAL_NAME_LEN, then it must have a terminating null
*/
BOOL BTL_BMG_VerifyLocalNameParms(const U8 *name, U8 len)
{
	if (0 ==name)
	{
		return FALSE;
	}

	if  (len > BTL_BMG_MAX_LOCAL_NAME_LEN)
	{
		/* name too long */
		return FALSE;
	}
	else if ((len > 0) && (len < BTL_BMG_MAX_LOCAL_NAME_LEN))
	{
		/* name must have a terminating null */
		if (name[len - 1] != 0)
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

BtStatus BTL_BMG_CreatePools()
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_CreatePools");
	
	status = BTL_POOL_Create(	&btlBmgData.contextsPool,
								BtlBmgContextsPoolName,
								btlBmgData.contextsMemory, 
								BTL_CONFIG_BMG_MAX_NUM_OF_BMG_CONTEXTS,
								sizeof(BtlBmgContext));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Contexts pool creation failed"));
			
	status = BTL_POOL_Create(	&btlBmgData.requestsPool,
								BtlBmgRequestsPoolName,
								btlBmgData.requestsMemory, 
								BTL_BMG_MAX_NUM_OF_REQUESTS,
								BTL_BMG_MAX_REQUEST_SIZE);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Requests pool creation failed"));
	
	status = BTL_POOL_Create(	&btlBmgData.hciParmsPool,
								BtlBmgHciParmsPoolName,
								btlBmgData.hciParmsMemory, 
								BTL_CONFIG_BMG_MAX_NUM_OF_CONCURRENT_HCI_COMMANDS,
								HCI_CMD_PARM_LEN);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("HCI Parms pool creation failed"));

	status = BTL_POOL_Create(	&btlBmgData.bondParmsPool,
								BtlBmgBondParmsPoolName,
								btlBmgData.bondParmsMemory, 
								BTL_CONFIG_BMG_MAX_NUM_OF_CONCURRENT_BOND_REQUESTS,
								sizeof(BtlBmgBondData));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("BOND Parms pool creation failed"));

	status = BTL_POOL_Create(	&btlBmgData.sdpServiceRecordsPool,
								BtlBmgsdpServiceRecordsPoolName,
								btlBmgData.sdpServiceRecordsMemory, 
								BTL_CONFIG_BMG_MAX_NUM_OF_USER_SERVICE_RECORDS,
								BTL_BMG_SERVICE_RECORD_ACTUAL_SIZE);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("SDP service records pool creation failed"));

	status = BTL_POOL_Create(	&btlBmgData.sdpAttrValuesPool,
								BtlBmgsdpAttrValuesPoolName,
								btlBmgData.sdpAttrValuesMemory, 
								BTL_CONFIG_BMG_MAX_NUM_OF_ATTRIBUTES,
								BTL_CONFIG_BMG_MAX_ATTRIBUTES_VALUE_SIZE);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("SDP attribute values pool creation failed"));

	status = BTL_POOL_Create(	&btlBmgData.pagingActivitiesPointersPool,
								BtlBmgPagingActivitiesPointersPoolName,
								btlBmgData.pagingActivitiesPointersMemory, 
								BTL_BMG_MAX_NUM_OF_PAGING_ACTIVITIES,
								sizeof(BtlBmgPagingActivityPointerEntry));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Paging Activities pool creation failed"));

	
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_DestroyPools()
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_DestroyPools");
			
	status = BTL_POOL_Destroy(&btlBmgData.contextsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Contexts pool destruction failed"));
		
	status = BTL_POOL_Destroy(&btlBmgData.requestsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Requests pool destruction failed"));
		
	status = BTL_POOL_Destroy(&btlBmgData.hciParmsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("HCI Parms pool destruction failed"));

	status = BTL_POOL_Destroy(&btlBmgData.bondParmsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("BOND Parms pool destruction failed"));

	status = BTL_POOL_Destroy(&btlBmgData.sdpServiceRecordsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("SDP service records destruction failed"));
		
	status = BTL_POOL_Destroy(&btlBmgData.sdpAttrValuesPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("SDP attribute values destruction failed"));

	status = BTL_POOL_Destroy(&btlBmgData.pagingActivitiesPointersPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Paging activities pointers destruction failed"));

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_InitData()
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_InitData");

	/* Create the auxiliary pseudo-request that serves as a parent request for all top-level requests */
	BTL_BMG_COMPOUND_REQUEST_Create(	&btlBmgData.masterPseudoRequest,
											BTL_BMG_REQUEST_TYPE_UNSPECIFIED,
											0,
											0,
											0,
											0);
	
	InitializeListHead(&btlBmgData.contextsList);
	
	

	InitializeListHead(&btlBmgData.pagingActivitiesPointersList);
	btlBmgData.pagingActivityInProgress = 0;

	btlBmgData.searchRequest = 0;
	btlBmgData.sdpQueryInProgress = FALSE;
	btlBmgData.searchParams.discoverServicesParams.discoveryRequest = 0;
	btlBmgData.searchParams.serviceSearchAttrParams.serviceSearchAttrRequest = 0;

	btlBmgData.securityMode = BTL_CONFIG_BMG_DEFAULT_SECURITY_MODE;
	MEC(securityMode) = btlBmgData.securityMode;	/* Mirror state in ME layer */

   	BTL_BMG_SetLocalNameParms(  BTL_CONFIG_BMG_DFLT_DEVICE_NAME, 
                                    (U8)OS_StrLen((const char *)BTL_CONFIG_BMG_DFLT_DEVICE_NAME),
                                    &btlBmgData.localNameParms);

	MEC(localName) =  btlBmgData.localNameParms.localName;
       MEC(localNameLen) = btlBmgData.localNameParms.localNameLen;

	btlBmgData.pendingSecurityMode = _BTL_BMG_INVALID_SECURITY_MODE;
	btlBmgData.securityModeChangingInProgress = FALSE;

	btlBmgData.enableTestModeNextHciCommand = BTL_BMG_ENABLE_TEST_MODE_FIRST_HCI_COMMAND;

	btlBmgData.radioOnData.radioOnRequest = 0;

	btlBmgData.hciInitInterceptionRequest = 0;
	btlBmgData.blockHciInitEvents = TRUE;

	btlBmgData.radioOnData.state = _BTL_BMG_RADIO_ON_STATE_NONE;
	btlBmgData.radioOffData.state = _BTL_BMG_RADIO_OFF_STATE_NONE;
	
	BTL_FUNC_END();
	
	return status;
}


void BTL_BMG_SetLocalNameParms(const U8 					*name, 
										U8 						len,
										BtlBmgLocalNameParms		*localNameParms)
{
	if (BTL_BMG_MAX_LOCAL_NAME_LEN == len)
	{
		/* Copy name WITHOUT terminating null (name has maximum length => no terminating null) */
		OS_MemCopy(localNameParms->localName, name, BTL_BMG_MAX_LOCAL_NAME_LEN);
	}
	else if ((len > 0) && (len < BTL_BMG_MAX_LOCAL_NAME_LEN))
	{
		/* Copy name WITH terminating null */
		OS_MemCopy(localNameParms->localName, name, len + 1);
	}
	else /* len == 0 */
	{		
		localNameParms->localName[0] = 0;												
	}

	localNameParms->localNameLen = len;
}

static void BTL_BMG_InitSearchCommonData(BtlBmgSearchCommonData		*commonSearchData,
												BtlBmgSearchMethod 			method,
												const BOOL 					performNameRequest,
												const SdpServicesMask	sdpServicesMask)
{
	commonSearchData->method = method;
	commonSearchData->performNameRequest = performNameRequest;
	commonSearchData->sdpServicesMask = sdpServicesMask;
	commonSearchData->numOfFilteredResponses = 0;
	
	OS_MemSet((U8*)commonSearchData->filteredDevicesAddresses, 
				0, 
				BTL_BMG_MAX_NUM_OF_FILTERED_RESPONSES * BD_ADDR_SIZE);

	commonSearchData->numOfPendingNameRequests = 0;

	OS_MemSet((U8*)commonSearchData->pendingNameRequestsFlags, 
				0, 
				BTL_BMG_MAX_NUM_OF_FILTERED_RESPONSES * sizeof(U8));
	
	commonSearchData->inquiryInProgress = FALSE;
	
	if (SDP_SERVICE_NONE != sdpServicesMask)
	{
		commonSearchData->servicesDiscoveryCompleted = FALSE;
	}
	else
	{
		commonSearchData->servicesDiscoveryCompleted = TRUE;
	}
	
	commonSearchData->numOfDevicesThatCompletedServiceDiscovery = 0; 
	commonSearchData->lastErrorCode = BEC_NO_ERROR;
	commonSearchData->inquiryCancelRequested = FALSE;
	commonSearchData->searchCancellationInProgress = FALSE;
}

static BtClassOfDevice BTL_BMG_GetCodFromCodFilter(const BtlBmgCodFilter* codFilter)
{
	BtClassOfDevice	cod = 0;

	BTL_FUNC_START("BTL_BMG_GetCodFromCodFilter");
	
	if (0 != codFilter)
	{	

		if (0 != (BTL_BMG_SEARCH_COD_FILTER_SERVICE_CLS & codFilter->codFilterMask))
		{
			cod |= codFilter->serviceClass;
		}

		if (0 != (BTL_BMG_SEARCH_COD_FILTER_MAJOR_DEV_CLS & codFilter->codFilterMask))
		{
			cod |= codFilter->majorDeviceClass;
		}

		if (0 != (BTL_BMG_SEARCH_COD_FILTER_MINOR_DEV_CLS & codFilter->codFilterMask))
		{
			cod |= codFilter->minorDeviceClass;
		}
	}
	else
	{
		BTL_LOG_DEBUG(("Null filter, set COD = 0 "));
	}

	BTL_LOG_DEBUG(("Resulting COD: %x", cod));
	
	BTL_FUNC_END();
	
	return cod;
}

BOOL BTL_BMG_DoesPassCodFilter(const BtEvent *event)
{
	BOOL 			answer = TRUE;

	static const BtClassOfDevice minorPeripheralMajor = 0x000000C0;
	static const BtClassOfDevice minorPeripheralMinor = 0x0000003F;
	
	BtClassOfDevice	deviceCod = event->p.inqResult.classOfDevice;
	BtClassOfDevice	serviceDeviceCod = deviceCod & COD_SERVICE_MASK;
	BtClassOfDevice	majorDeviceCod = deviceCod & COD_MAJOR_MASK;
	BtClassOfDevice	minorDeviceCod = deviceCod & COD_MINOR_MASK;
	BtClassOfDevice	minorDeviceCodPeripheralMajor = minorDeviceCod & minorPeripheralMajor;
	BtClassOfDevice	minorDeviceCodPeripheralMinor = minorDeviceCod & minorPeripheralMinor;
	
	BtClassOfDevice	filterCod = btlBmgData.searchData.searchByCodData.codFilter;
	BtClassOfDevice	serviceFilterCod = filterCod & COD_SERVICE_MASK;
	BtClassOfDevice	majorFilterCod = filterCod & COD_MAJOR_MASK;
	BtClassOfDevice	minorFilterCod = filterCod & COD_MINOR_MASK;
	BtClassOfDevice	minorFilterCodPeripheralMajor = minorFilterCod & minorPeripheralMajor;
	BtClassOfDevice	minorFilterCodPeripheralMinor = minorFilterCod & minorPeripheralMinor;

	BTL_FUNC_START("BTL_BMG_DoesPassCodFilter");
	
	if (serviceFilterCod != 0)
	{
		if ((serviceFilterCod & serviceDeviceCod) == 0)
		{
			BTL_LOG_DEBUG(("COD Filter Failed due to service class (Filter: %x, Device: %x)", serviceFilterCod, serviceDeviceCod));
			BTL_RET_SET_RETVAR(answer = FALSE);
		}
	}

	/* Passed Service Class Filter */
	
	/* Check if we should filter using the Major Device Class */
	if (majorFilterCod != 0)
	{
		/* Major is used, check if device's Major passes filter */
		if (majorFilterCod != majorDeviceCod)
		{
			BTL_LOG_DEBUG(("COD Filter Failed due to Major class (Filter: %x, Device: %x)", majorFilterCod, majorDeviceCod));
			BTL_RET_SET_RETVAR(answer = FALSE);
		}

		/* Major filter passed, check minor */
		if (minorFilterCod != 0)
		{
			if (majorFilterCod != COD_MAJOR_PERIPHERAL)
			{
				/* Major is used, check if device's Minor passes filter */
				if (minorFilterCod != minorDeviceCod)
				{
					BTL_LOG_DEBUG(("COD Filter Failed due to Minor class (Filter: %x, Device: %x)", minorFilterCod, minorDeviceCod));
					BTL_RET_SET_RETVAR(answer = FALSE);
				}
			}
			else /* Peripheral */
			{
				if (minorFilterCodPeripheralMajor != 0)
				{
					if (minorFilterCodPeripheralMajor != minorDeviceCodPeripheralMajor)
					{
						BTL_LOG_DEBUG(("Peripheral COD Filter Failed due to Major Peripheral Part (Filter Major: %x, Device Major: %x)", 
										minorFilterCodPeripheralMajor, minorDeviceCodPeripheralMajor));
						BTL_RET_SET_RETVAR(answer = FALSE);
					}
				}

				if (minorFilterCodPeripheralMinor != 0)
				{
					if (minorFilterCodPeripheralMinor != minorDeviceCodPeripheralMinor)
					{
						BTL_LOG_DEBUG(("Peripheral COD Filter Failed due to Minor Peripheral Part (Filter Minor: %x, Device Minor: %x)", 
										 minorFilterCodPeripheralMinor, minorDeviceCodPeripheralMinor));
						BTL_RET_SET_RETVAR(answer = FALSE);
					}
				}
			}
		}
	}

	BTL_FUNC_END();

	BTL_LOG_DEBUG(("%s %s COD Filter", BTL_UTILS_LockedBdaddrNtoa(&event->p.inqResult.bdAddr), (answer ? "Passed" : "Failed")));
	
	return answer;
}

BOOL BTL_BMG_DoesPassDevicesFilter(const BtEvent *event)
{
	const BD_ADDR				*bdAddress = &event->p.inqResult.bdAddr;
	BOOL						answer = FALSE;
	U8							deviceIndex = 0;
	BtlBmgSearchByDevicesData	*searchData = &btlBmgData.searchData.searchByDevicesData;

	for (deviceIndex = 0; deviceIndex < searchData->listLength; ++deviceIndex)
	{
		if (TRUE == AreBdAddrsEqual(bdAddress, &searchData->devicesList[deviceIndex]))
		{
			answer = TRUE;
			break;
		}	
	}

	return answer;
}

BtStatus BTL_BMG_CancelInquiryDuringSearch(	BtlBmgCompoundRequest		*specificRequest,
													BtlBmgSearchCommonData		*searchData)
{
	BtStatus					status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_CancelInquiryDuringSearch");

	if (searchData->inquiryInProgress == FALSE)
	{
		BTL_LOG_DEBUG(("No inquiry in progress - cancel inquiry during searcg completed immediately"));
		BTL_RET(BT_STATUS_SUCCESS);
	}
	
	/* Cancel the inquiry only once */
	if (FALSE == searchData->inquiryCancelRequested)
	{
		status = BTL_BMG_InternalCancelInquiryRequest(	specificRequest->base.bmgContext,
														specificRequest);

		if (BT_STATUS_PENDING == status)
		{
			BTL_LOG_INFO(("Inquiry Cancellation of Search in progress"));
		}
		else if (BT_STATUS_SUCCESS == status)
		{
			BTL_LOG_INFO(("Inquiry Cancellation of Search completed"));
		}
		else
		{
			BTL_LOG_ERROR(("Inquiry Cancellation Failed, status = %d", status));
		}

		/* Reuesting cancellation only once */
		searchData->inquiryCancelRequested = TRUE;
	}

	BTL_FUNC_END();

	return status;
}

void BTL_BMG_CompleteInquiryCancellationDuringSearch(BtlBmgCompoundRequest	*specificRequest,
																BtlBmgSearchCommonData	*searchData,
																BtEventType				reportedEvent)
{
	BTL_FUNC_START("BTL_BMG_CompleteInquiryCancellationDuringSearch");
	
	BTL_LOG_DEBUG(("Inquiry Cancellation Completed"));

	searchData->inquiryInProgress = FALSE;

	btlBmgData.selfGeneratedEvent.eType = reportedEvent;
	btlBmgData.selfGeneratedEvent.errCode = BEC_NO_ERROR;
			
	/* Filteres Inquiry results should be forwarded to all applications */
	BTL_BMG_EVM_DispatchEvent(&(specificRequest->base), 
								&btlBmgData.selfGeneratedEvent, 
								BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);
	BTL_FUNC_END();
}

void BTL_BMG_FillQueryParmsForSppServiceQuery(void)
{	
	BTL_FUNC_START("BTL_BMG_FillQueryParmsForSppServiceQuery");

	BTL_BMG_SetUuid16(SC_SERIAL_PORT, &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[0]);
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfUuids = 1;
	

	btlBmgData.searchParams.discoverServicesParams.queryParms.attributeIds[0] = AID_SERVICE_CLASS_ID_LIST;
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfAttributes = 1;
	
	BTL_FUNC_END();
}

void BTL_BMG_FillQueryParmsForDunServiceQuery(void)
{
	BTL_FUNC_START("BTL_BMG_FillQueryParmsForDunServiceQuery");

	BTL_BMG_SetUuid16(SC_DIALUP_NETWORKING, &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[0]);
	
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfUuids = 1;

	btlBmgData.searchParams.discoverServicesParams.queryParms.attributeIds[0] = AID_SERVICE_CLASS_ID_LIST;
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfAttributes = 1;
	
	BTL_FUNC_END();
}

void BTL_BMG_FillQueryParmsForOppServiceQuery(void)
{
	BTL_FUNC_START("BTL_BMG_FillQueryParmsForOppServiceQuery");

	BTL_BMG_SetUuid16(SC_OBEX_OBJECT_PUSH, &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[0]);

	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfUuids = 1;

	btlBmgData.searchParams.discoverServicesParams.queryParms.attributeIds[0] = AID_SERVICE_CLASS_ID_LIST;
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfAttributes = 1;
	
	BTL_FUNC_END();
}

void BTL_BMG_FillQueryParmsForFtpServiceQuery(void)
{
	BTL_FUNC_START("BTL_BMG_FillQueryParmsForFtpServiceQuery");

	BTL_BMG_SetUuid16(SC_OBEX_FILE_TRANSFER, &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[0]);

	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfUuids = 1;

	btlBmgData.searchParams.discoverServicesParams.queryParms.attributeIds[0] = AID_SERVICE_CLASS_ID_LIST;
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfAttributes = 1;
	
	BTL_FUNC_END();
}

void BTL_BMG_FillQueryParmsForHsServiceQuery(void)
{
	BTL_FUNC_START("BTL_BMG_FillQueryParmsForHsServiceQuery");

	BTL_BMG_SetUuid16(SC_HEADSET, &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[0]);

	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfUuids = 1;

	btlBmgData.searchParams.discoverServicesParams.queryParms.attributeIds[0] = AID_SERVICE_CLASS_ID_LIST;
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfAttributes = 1;
	
	BTL_FUNC_END();
}

void BTL_BMG_FillQueryParmsForA2dpServiceQuery(void)
{
	BTL_FUNC_START("BTL_BMG_FillQueryParmsForA2dpServiceQuery");

	BTL_BMG_SetUuid16(PROT_L2CAP, &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[0]);
	BTL_BMG_SetUuid16(PROT_AVDTP, &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[1]);
	BTL_BMG_SetUuid16(SC_AUDIO_SINK, &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[2]);

	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfUuids = 3;

	btlBmgData.searchParams.discoverServicesParams.queryParms.attributeIds[0] = AID_SERVICE_CLASS_ID_LIST;
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfAttributes = 1;
	
	BTL_FUNC_END();
}

void BTL_BMG_FillQueryParmsForAvrcpServiceQuery(void)
{
	BTL_FUNC_START("BTL_BMG_FillQueryParmsForAvrcpServiceQuery");

	BTL_BMG_SetUuid16(SC_AV_REMOTE_CONTROL, &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[0]);

	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfUuids = 1;

	btlBmgData.searchParams.discoverServicesParams.queryParms.attributeIds[0] = AID_SERVICE_CLASS_ID_LIST;
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfAttributes = 1;
	
	BTL_FUNC_END();
}

void BTL_BMG_FillQueryParmsForFaxServiceQuery(void)
{
	BTL_FUNC_START("BTL_BMG_FillQueryParmsForFaxServiceQuery");

	BTL_BMG_SetUuid16(SC_FAX, &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[0]);

	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfUuids = 1;

	btlBmgData.searchParams.discoverServicesParams.queryParms.attributeIds[0] = AID_SERVICE_CLASS_ID_LIST;
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfAttributes = 1;
	
	BTL_FUNC_END();
}

void BTL_BMG_FillQueryParmsForBppServiceQuery(void)
{
	BTL_FUNC_START("BTL_BMG_FillQueryParmsForBppServiceQuery");

	BTL_BMG_SetUuid16(SC_DIRECT_PRINTING, &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[0]);

	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfUuids = 1;

	btlBmgData.searchParams.discoverServicesParams.queryParms.attributeIds[0] = AID_SERVICE_CLASS_ID_LIST;
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfAttributes = 1;
	
	BTL_FUNC_END();
}

void BTL_BMG_FillQueryParmsForBipServiceQuery(void)
{
	BTL_FUNC_START("BTL_BMG_FillQueryParmsForBipServiceQuery");

	BTL_BMG_SetUuid16(SC_IMAGING_RESPONDER, &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[0]);

	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfUuids = 1;

	btlBmgData.searchParams.discoverServicesParams.queryParms.attributeIds[0] = AID_SERVICE_CLASS_ID_LIST;
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfAttributes = 1;
	
	BTL_FUNC_END();
}

void BTL_BMG_FillQueryParmsForHfServiceQuery(void)
{
	BTL_FUNC_START("BTL_BMG_FillQueryParmsForHfServiceQuery");

	BTL_BMG_SetUuid16(SC_HANDSFREE, &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[0]);

	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfUuids = 1;

	btlBmgData.searchParams.discoverServicesParams.queryParms.attributeIds[0] = AID_SERVICE_CLASS_ID_LIST;
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfAttributes = 1;
	
	BTL_FUNC_END();
}

void BTL_BMG_FillQueryParmsForSapServiceQuery(void)
{
	BTL_FUNC_START("BTL_BMG_FillQueryParmsForSapServiceQuery");

	BTL_BMG_SetUuid16(SC_SIM_ACCESS, &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[0]);

	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfUuids = 1;

	btlBmgData.searchParams.discoverServicesParams.queryParms.attributeIds[0] = AID_SERVICE_CLASS_ID_LIST;
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfAttributes = 1;
	
	BTL_FUNC_END();
}

void BTL_BMG_FillQueryParmsForPbapServiceQuery(void)
{
	BTL_FUNC_START("BTL_BMG_FillQueryParmsForPbapServiceQuery");

	BTL_BMG_SetUuid16( SC_PBAP_CLIENT , &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[0]);

	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfUuids = 1;

	btlBmgData.searchParams.discoverServicesParams.queryParms.attributeIds[0] = AID_SERVICE_CLASS_ID_LIST;
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfAttributes = 1;
	
	BTL_FUNC_END();
}

void BTL_BMG_FillQueryParmsForHidServiceQuery(void)
{
	BTL_FUNC_START("BTL_BMG_FillQueryParmsForHidServiceQuery");

	BTL_BMG_SetUuid16(SC_HUMAN_INTERFACE_DEVICE, &btlBmgData.searchParams.discoverServicesParams.queryParms.uuids[0]);

	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfUuids = 1;

	btlBmgData.searchParams.discoverServicesParams.queryParms.attributeIds[0] = AID_SERVICE_CLASS_ID_LIST;
	btlBmgData.searchParams.discoverServicesParams.queryParms.numOfAttributes = 1;
	
	BTL_FUNC_END();
}

void BTL_BMG_FillServiceDiscoveryQueryParms(SdpServicesMask serviceMask)
{	
	BTL_FUNC_START("BTL_BMG_FillServiceDiscoveryQueryParms");
	
	btlBmgData.searchParams.discoverServicesParams.queryParms.maxServiceRecords = 1;
	btlBmgData.searchParams.discoverServicesParams.queryParms.callbackFunc = BTL_BMG_InternalDiscoverServicesCb;
	btlBmgData.searchParams.discoverServicesParams.queryParms.buff = (U8 *)btlBmgData.sdpQueryResultsBuff;
	btlBmgData.searchParams.discoverServicesParams.queryParms.len = BTL_BMG_MAX_SDP_QUERY_RESULT_BUF;

	switch (serviceMask)
	{
		case SDP_SERVICE_LAP:
		case SDP_SERVICE_SYNC:
		case SDP_SERVICE_CTP:
		case SDP_SERVICE_ICP:
		case SDP_SERVICE_PANU:
		case SDP_SERVICE_NAP:
		case SDP_SERVICE_GN:
		case SDP_SERVICE_HCRP:
			BTL_FATAL_NORET(("Unsupported service to discover"));

			break;

		case SDP_SERVICE_SPP:

			BTL_BMG_FillQueryParmsForSppServiceQuery();
			
			break;

		case SDP_SERVICE_DUN:

			BTL_BMG_FillQueryParmsForDunServiceQuery();
			
			break;

		case SDP_SERVICE_OPP:

			BTL_BMG_FillQueryParmsForOppServiceQuery();
			
			break;

		case SDP_SERVICE_FTP:

			BTL_BMG_FillQueryParmsForFtpServiceQuery();

			break;

		case SDP_SERVICE_HSP:
			
			BTL_BMG_FillQueryParmsForHsServiceQuery();
			
			break;

		case SDP_SERVICE_A2DP:
			
			BTL_BMG_FillQueryParmsForA2dpServiceQuery();
			
			break;

		case SDP_SERVICE_AVRCP:
			
			BTL_BMG_FillQueryParmsForAvrcpServiceQuery();
			
			break;

		case SDP_SERVICE_FAX:

			BTL_BMG_FillQueryParmsForFaxServiceQuery();
			
			break;

		case SDP_SERVICE_BPP:

			BTL_BMG_FillQueryParmsForBppServiceQuery();

			break;

		case SDP_SERVICE_BIP:

			BTL_BMG_FillQueryParmsForBipServiceQuery();
			
			break;

		case SDP_SERVICE_HFP:
			
			BTL_BMG_FillQueryParmsForHfServiceQuery();
			
			break;

		case SDP_SERVICE_SAP:

			BTL_BMG_FillQueryParmsForSapServiceQuery();
			
			break;

		case SDP_SERVICE_PBAP:

			BTL_BMG_FillQueryParmsForPbapServiceQuery();
			
			break;			

		case SDP_SERVICE_HID:

			BTL_BMG_FillQueryParmsForHidServiceQuery();
			
			break;


		default:

			BTL_FATAL_NORET(("Invalid service to discover: %d", serviceMask));
			
	}

	BTL_FUNC_END();
}

void BTL_BMG_FillServiceSearchAttrQueryParms(BtlBmgUuid* uuidList,
																				U8  uuidListLen,
																				U16	maxServiceRecords,
																				SdpAttributeId *attributeIdList,
																				U16 attributeIdListLen)
{
	U16 i;
	
	btlBmgData.searchParams.serviceSearchAttrParams.queryParms.maxServiceRecords = maxServiceRecords;
	btlBmgData.searchParams.serviceSearchAttrParams.queryParms.callbackFunc = BTL_BMG_ServiceSearchAttributeRequestCb;
	btlBmgData.searchParams.serviceSearchAttrParams.queryParms.buff = (U8 *)btlBmgData.sdpQueryResultsBuff;
	btlBmgData.searchParams.serviceSearchAttrParams.queryParms.len = BTL_BMG_MAX_SDP_QUERY_RESULT_BUF;

	for(i=0; i<uuidListLen; i++)
	{
		OS_MemCopy(btlBmgData.searchParams.serviceSearchAttrParams.queryParms.uuids[i].uuid, uuidList[i].uuid, sizeof(BtlBmgUuid));
	}

	for(i=0; i<attributeIdListLen; i++)
	{
		btlBmgData.searchParams.serviceSearchAttrParams.queryParms.attributeIds[i] = attributeIdList[i];
	}
	btlBmgData.searchParams.serviceSearchAttrParams.queryParms.numOfUuids = uuidListLen;

	btlBmgData.searchParams.serviceSearchAttrParams.queryParms.numOfAttributes = attributeIdListLen;
}

void BTL_BMG_FillServiceSearchQueryParms(BtlBmgUuid* uuidList,
																				U8  uuidListLen,
																				U16	maxServiceRecords)
{	
	U16 i;

	btlBmgData.searchParams.serviceSearchParams.queryParms.maxServiceRecords = maxServiceRecords;
	btlBmgData.searchParams.serviceSearchParams.queryParms.callbackFunc = BTL_BMG_ServiceSearchRequestCb;
	btlBmgData.searchParams.serviceSearchParams.queryParms.buff = (U8 *)btlBmgData.sdpQueryResultsBuff;
	btlBmgData.searchParams.serviceSearchParams.queryParms.len = BTL_BMG_MAX_SDP_QUERY_RESULT_BUF;

	for(i=0; i<uuidListLen; i++)
	{
		OS_MemCopy(btlBmgData.searchParams.serviceSearchParams.queryParms.uuids[i].uuid, uuidList[i].uuid, sizeof(BtlBmgUuid));
	}

	btlBmgData.searchParams.serviceSearchParams.queryParms.numOfUuids = uuidListLen;
}

void BTL_BMG_FillServiceAttrQueryParms	(U32 recordHandle,
																				SdpAttributeId *attributeIdList,
																				U16 attributeIdListLen)
{
	U16 i;

	btlBmgData.searchParams.serviceAttrParams.queryParms.callbackFunc = BTL_BMG_ServiceAttributeRequestCb;
	btlBmgData.searchParams.serviceAttrParams.queryParms.buff = (U8 *)btlBmgData.sdpQueryResultsBuff;
	btlBmgData.searchParams.serviceAttrParams.queryParms.len = BTL_BMG_MAX_SDP_QUERY_RESULT_BUF;

	btlBmgData.searchParams.serviceAttrParams.queryParms.recordHandle = recordHandle;
	
	for(i=0; i<attributeIdListLen; i++)
	{
		btlBmgData.searchParams.serviceAttrParams.queryParms.attributeIds[i] = attributeIdList[i];
	}
	
	btlBmgData.searchParams.serviceAttrParams.queryParms.numOfAttributes = attributeIdListLen;
}

SdpServicesMask BTL_BMG_GetNextServiceToDiscover()
{
	SdpServicesMask	nextService = SDP_SERVICE_NONE;

	BTL_FUNC_START("BTL_BMG_GetNextServiceToDiscover");
	
	/* Initialize if this is the first service */
	if (SDP_SERVICE_NONE == btlBmgData.searchParams.discoverServicesParams.currentServiceToDiscover)
	{
		nextService = SDP_SERVICE_MIN_MASK;
	}
	else
	{
		nextService = btlBmgData.searchParams.discoverServicesParams.currentServiceToDiscover * 2;
	}

	/* Check all services for a turned on service mask bit */
	while (nextService <= SDP_SERVICE_MAX_MASK)
	{
		if (0 != (nextService & btlBmgData.searchParams.discoverServicesParams.servicesMask))
		{
			break;
		}

		/* Move on to check the next service */
		nextService *= 2;
	}

	if (nextService > SDP_SERVICE_MAX_MASK)
	{
		BTL_LOG_DEBUG(("No more services to discover"));
		
		/* No more service to discover */
		nextService = SDP_SERVICE_NONE;
	}
	else
	{
		BTL_LOG_DEBUG(("Next service to discover is: |%s|", pSdpServicesMask(nextService)));
	}
	
	BTL_FUNC_END();
	
	return nextService;
}

BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponseSpp(U8 *respBuff, BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16		sppUuid = 0x0111; /* SC_SERIAL_PORT (Big Endian) */

	BTL_FUNC_START("BTL_BMG_VerifyServiceDiscoveryQueryResponseSpp");
	status = BTL_BMG_FindUuidInRespBuff(respBuff,  sppUuid, answer);		
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponseDun(U8 *respBuff, BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16		dunUuid = 0x0311; /*  SC_DIALUP_NETWORKING (Big Endian) */

	BTL_FUNC_START("BTL_BMG_VerifyServiceDiscoveryQueryResponseDun");
	status = BTL_BMG_FindUuidInRespBuff(respBuff,  dunUuid, answer);		
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponseOpp(U8 *respBuff, BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16		oppUuid = 0x0511; /* SC_OBEX_OBJECT_PUSH (Big Endian) */

	BTL_FUNC_START("BTL_BMG_VerifyServiceDiscoveryQueryResponseOpp");
	status = BTL_BMG_FindUuidInRespBuff(respBuff,  oppUuid, answer);		
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponseFtp(U8 *respBuff, BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16		ftpUuid = 0x0611; /* SC_OBEX_FILE_TRANSFER (Big Endian) */

	BTL_FUNC_START("BTL_BMG_VerifyServiceDiscoveryQueryResponseFtp");
	status = BTL_BMG_FindUuidInRespBuff(respBuff,  ftpUuid, answer);		
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponseHs(U8 *respBuff, BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16		headsetUuid = 0x0811 /* SC_HEADSET */;

	BTL_FUNC_START("BTL_BMG_VerifyServiceDiscoveryQueryResponseHs");
	status = BTL_BMG_FindUuidInRespBuff(respBuff,  headsetUuid, answer);		
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponseA2dp(U8 *respBuff, BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16		a2dpUuid = 0x0B11; /* SC_AUDIO_SINK (Big Endian) */

	BTL_FUNC_START("BTL_BMG_VerifyServiceDiscoveryQueryResponseA2dp");
	status = BTL_BMG_FindUuidInRespBuff(respBuff,  a2dpUuid, answer);	
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponseAvrcp(U8 *respBuff, BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16		avrcpUuid = 0x0E11; /* SC_AV_REMOTE_CONTROL (Big Endian) */

	BTL_FUNC_START("BTL_BMG_VerifyServiceDiscoveryQueryResponseAvrcp");
	status = BTL_BMG_FindUuidInRespBuff(respBuff,  avrcpUuid, answer);
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponseFax(U8 *respBuff, BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16		faxUuid = 0x1111; /* SC_FAX (Big Endian) */

	BTL_FUNC_START("BTL_BMG_VerifyServiceDiscoveryQueryResponseFax");
	status = BTL_BMG_FindUuidInRespBuff(respBuff,  faxUuid, answer);
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponseBpp(U8 *respBuff, BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16		bppUuid = 0x1811; /* SC_DIRECT_PRINTING (Big Endian) */

	BTL_FUNC_START("BTL_BMG_VerifyServiceDiscoveryQueryResponseBpp");
	status = BTL_BMG_FindUuidInRespBuff(respBuff,  bppUuid, answer);
	
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponseBip(U8 *respBuff, BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16		bipUuid = 0x1B11; /* SC_IMAGING_RESPONDER (Big Endian) */

	BTL_FUNC_START("BTL_BMG_VerifyServiceDiscoveryQueryResponseBip");
	status = BTL_BMG_FindUuidInRespBuff( respBuff,  bipUuid,answer);
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponseHf(U8 *respBuff, BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16		hfUuid = 0x1E11; /* SC_HANDSFREE (Big Endian) */

	BTL_FUNC_START("BTL_BMG_VerifyServiceDiscoveryQueryResponseHf");
	status = BTL_BMG_FindUuidInRespBuff( respBuff,  hfUuid,answer);
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponseSap(U8 *respBuff, BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16		sapUuid = 0x2D11; /* SC_SIM_ACCESS (Big Endian) */

	BTL_FUNC_START("BTL_BMG_VerifyServiceDiscoveryQueryResponseSap");
	status = BTL_BMG_FindUuidInRespBuff( respBuff,  sapUuid,answer);
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponsePbap(U8 *respBuff, BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16		pbapUuid = 0x2E11; /* SC_PBAP_CLIENT (Big Endian) *//*0x2F11;*//*SC_PBAP_SERVER (Big Endian)*/

	BTL_FUNC_START("BTL_BMG_VerifyServiceDiscoveryQueryResponsePbap");
	status = BTL_BMG_FindUuidInRespBuff( respBuff,  pbapUuid,answer);
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponseHid(U8 *respBuff, BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16		hidUuid = 0x2411; /* SC_HUMAN_INTERFACE_DEVICE (Big Endian) */

	BTL_FUNC_START("BTL_BMG_VerifyServiceDiscoveryQueryResponseHid");

	status = BTL_BMG_FindUuidInRespBuff( respBuff,  hidUuid,answer);
	BTL_FUNC_END();
	
	return status;
}
/*  BTL_BMG_FindUuidInRespBuff()
  *  
  *  this function was created , since the SERICE CLASS ID LIST may have 
  *  more then one UUID. In this case we are all we whant to know is that 
  *  the given UUID is one of them.
  */
static BtStatus BTL_BMG_FindUuidInRespBuff(U8 *respBuff ,U16 uuid, BOOL *answer)
{
	
	BtStatus status = BT_STATUS_SUCCESS;
	U16 	attrSize = 0;
	U8		*attrData = 0;
	U16 	offset = 0;
	U16 	offsetInAttr = 0;
	BTL_FUNC_START("BTL_BMG_FindUuidInRespBuff");
	attrData = BTIPS_RS_GetAttributeData(respBuff, AID_SERVICE_CLASS_ID_LIST, &attrSize);

	BTL_VERIFY_FATAL(0 != attrData, BT_STATUS_INTERNAL_ERROR, ("Null Attribute Data"));

	offsetInAttr = 0;
	
	while (offsetInAttr < attrSize)
	{
		/* Parse the Attribute Id*/
		U32 len = SDP_ParseDataElement((attrData + offsetInAttr), &offset);

		*answer = SdpUuidCmp((const U8*)&uuid, 2, attrData + offsetInAttr +offset, (U16)len);

		if (*answer == TRUE)
		{
			break;
		}

		offsetInAttr = (U16)(offsetInAttr + offset + (U16)len);
	}
	
	BTL_FUNC_END();
	return status;
}

static BtErrorCode BTL_BMG_MapSdpQueryStatusToEventErrorCode(BTIPS_RS_Status queryStatus)
{
	BtErrorCode	eventErrorCode;

	BTL_FUNC_START("BTL_BMG_MapSdpQueryStatusToEventErrorCode");

	switch(queryStatus)
	{
		case BTIPS_RS_STATUS_OK:

			eventErrorCode =  BEC_NO_ERROR;
			
			break;
			
		case BTIPS_RS_STATUS_BUFFER_OVERFLOW:

			eventErrorCode =  BEC_OVERFLOW;
			
			break;

		case BTIPS_RS_STATUS_QUERY_EMPTY:

			eventErrorCode =  BEC_NO_ERROR;
			
			break;
			
		case BTIPS_RS_STATUS_QUERY_FAILURE:

			eventErrorCode =  BEC_FAILED;

			break;
			
		case BTIPS_RS_STATUS_CONNECT_FAILURE:

			eventErrorCode =  BEC_CONNECTION_FAILED;

			break;

		case BTIPS_RS_STATUS_DEVICE_NOT_REGISTERED:

			eventErrorCode =  BEC_NOT_FOUND;

			break;

		case BTIPS_RS_STATUS_INTERNAL:	

			eventErrorCode =  BEC_INTERNAL_ERROR;

			break;

		case BTIPS_RS_STATUS_OPERATION_END:
		case BTIPS_RS_STATUS_OPERATION_CANCELLED:
			eventErrorCode =  BEC_NO_ERROR;
			
			break;
			
		default:

			BTL_ERR_NORET(("Unexpected or Invalid SDP Retrieve Status: %d", queryStatus));
			
			eventErrorCode =  BEC_UNSPECIFIED_ERR;

	};

	BTL_LOG_DEBUG(("Mapped Query Status (%s) to Event Status (%d", 
					BTL_BMG_DEBUG_pSdpRetrieveStatus(queryStatus), eventErrorCode));
	
	BTL_FUNC_END();
	
	return eventErrorCode;
}


BtStatus BTL_BMG_VerifyServiceDiscoveryQueryResponse(U8 *respBuff, BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16	numOfAttrs = BTIPS_RS_GetNumberOfAttributesFound(respBuff);

	BTL_FUNC_START("BTL_BMG_VerifyServiceDiscoveryQueryResponse");
	
	if (0 == numOfAttrs)
	{
		*answer = FALSE;
		BTL_LOG_DEBUG(("No records matched query => "));
		BTL_RET(BT_STATUS_SUCCESS);
	}

	*answer = TRUE;
	
	switch (btlBmgData.searchParams.discoverServicesParams.currentServiceToDiscover)
	{
		case SDP_SERVICE_LAP:
		case SDP_SERVICE_SYNC:
		case SDP_SERVICE_CTP:
		case SDP_SERVICE_ICP:
		case SDP_SERVICE_PANU:
		case SDP_SERVICE_NAP:
		case SDP_SERVICE_GN:
		case SDP_SERVICE_HCRP:
			break;

		case SDP_SERVICE_SPP:

			status = BTL_BMG_VerifyServiceDiscoveryQueryResponseSpp(respBuff, answer);
			
			break;

		case SDP_SERVICE_DUN:

			status = BTL_BMG_VerifyServiceDiscoveryQueryResponseDun(respBuff, answer);
			
			break;

		case SDP_SERVICE_OPP:

			status = BTL_BMG_VerifyServiceDiscoveryQueryResponseOpp(respBuff, answer);
			
			break;

		case SDP_SERVICE_FTP:

			status = BTL_BMG_VerifyServiceDiscoveryQueryResponseFtp(respBuff, answer);
			break;

		case SDP_SERVICE_HSP:

			status = BTL_BMG_VerifyServiceDiscoveryQueryResponseHs(respBuff, answer);
			
			break;

		case SDP_SERVICE_A2DP:	

			status = BTL_BMG_VerifyServiceDiscoveryQueryResponseA2dp(respBuff, answer);
			
			break;
			
		case SDP_SERVICE_AVRCP:

			status = BTL_BMG_VerifyServiceDiscoveryQueryResponseAvrcp(respBuff, answer);

			break;

		case SDP_SERVICE_FAX:

			status = BTL_BMG_VerifyServiceDiscoveryQueryResponseFax(respBuff, answer);
			
			break;

		case SDP_SERVICE_BPP:

			status = BTL_BMG_VerifyServiceDiscoveryQueryResponseBpp(respBuff, answer);

			break;
			
		case SDP_SERVICE_BIP:

			status = BTL_BMG_VerifyServiceDiscoveryQueryResponseBip(respBuff, answer);
			
			break;

		case SDP_SERVICE_HFP:

			status = BTL_BMG_VerifyServiceDiscoveryQueryResponseHf(respBuff, answer);
			
			break;

		case SDP_SERVICE_SAP:

			status = BTL_BMG_VerifyServiceDiscoveryQueryResponseSap(respBuff, answer);

			break;

		case SDP_SERVICE_PBAP:
			
			status = BTL_BMG_VerifyServiceDiscoveryQueryResponsePbap(respBuff, answer);
			
			break;		

		case SDP_SERVICE_HID:

			status = BTL_BMG_VerifyServiceDiscoveryQueryResponseHid(respBuff, answer);

			break;

		default:

			BTL_FATAL_NORET(("Invalid service to Verify: %d", btlBmgData.searchParams.discoverServicesParams.currentServiceToDiscover));
			
	}

	BTL_FUNC_END();

	/******************************************/
	/**answer = TRUE;*/
		
	return status;
}

BOOL BTL_BMG_ShouldSecurityHandlersBeRegistered(BtSecurityMode securityMode)
{
	if (securityMode >= BSM_SEC_LEVEL_2)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BtStatus BTL_BMG_SetServiceRecord(BtlBmgContext *bmgContext, 
									U32 *recordHandle,
									BtClassOfDevice classOfDevice,
									const SdpAttribute *attributeList, 
									const U32 attributeListLen)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	BtStatus 	newStatus = BT_STATUS_SUCCESS;	
	SdpRecord* record = 0;
	U8* temp = 0;
	U32 numOfAttributs;
	SdpAttribute* curAttr = 0;
	U8 i;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BMG_SetServiceRecord");

	BTL_VERIFY_ERR((0 != bmgContext), BT_STATUS_INVALID_PARM, ("Null bmgContext"));
	BTL_VERIFY_ERR((0 != recordHandle), BT_STATUS_INVALID_PARM, ("Null recordHandle"));
	BTL_VERIFY_ERR((0 != attributeList), BT_STATUS_INVALID_PARM, ("Null attributeList"));
	BTL_VERIFY_ERR((0 != attributeListLen), BT_STATUS_INVALID_PARM, ("attributeListLen is 0"));	
	
	BTL_VERIFY_ERR((TRUE == IsNodeOnList(&btlBmgData.contextsList, &(bmgContext->base.node))),
					BT_STATUS_INVALID_PARM, ("Invalid context"));

	numOfAttributs = attributeListLen/sizeof(SdpAttribute);
	BTL_VERIFY_ERR((numOfAttributs <= BTL_CONFIG_BMG_MAX_ATTRIBUTES_PER_SERVICE_RECORD), BT_STATUS_INVALID_PARM, ("Number of attributs per record exceeds the maximum supported. see Btl_config.h"));
	
	if (*recordHandle == 0)
	{
		U32 *tempRecordHandle;
		U32 poolCapacity, numAllocated;

		BTL_VERIFY_ERR((0 != classOfDevice), BT_STATUS_INVALID_PARM, ("classOfDevice is 0"));

		/* first check that we have enough free attribute values in the pool: */
		status = BTL_POOL_GetCapacity(&btlBmgData.sdpAttrValuesPool, &poolCapacity);
		newStatus = BTL_POOL_GetNumOfAllocatedElements(&btlBmgData.sdpAttrValuesPool, &numAllocated);
		BTL_VERIFY_ERR(((BT_STATUS_SUCCESS == status) && (BT_STATUS_SUCCESS == newStatus)),
			BT_STATUS_INVALID_PARM, ("Failed to check num of free attribute values"));
		BTL_VERIFY_ERR((((poolCapacity - numAllocated) >= numOfAttributs)),
			BT_STATUS_NO_RESOURCES, ("not enough free attribute values in the pool"));
		
		/* Create a new record.*/
		/* Allocate memory for the new record */
		status = BTL_POOL_Allocate(&btlBmgData.sdpServiceRecordsPool, (void **)(&tempRecordHandle));
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating service record"));
		(*recordHandle) = (U32) tempRecordHandle;
		
		record = (SdpRecord *) (*recordHandle);
		
		InitializeListEntry(&(record->node));
		record->num = (U8)numOfAttributs;
		record->classOfDevice = classOfDevice;

		/* start copying the record to the allocated memory in the following way:
__________________________________________________________________
|                          |                       |				|		|			   |
|      SdpRecord     | SdpAttribute 1 | SdpAttribute 2 |    ...    | SdpAttribute N  |
------------------------------------------------------------------
		temp is an auxiliary pointer for copying.*/

		/* set temp to the begining of the attributes section */ 
		temp = (U8 *) (*recordHandle);
		temp += sizeof(SdpRecord);

		/* set attribs field of SdpRecord to point to the start of the attributes section in the buffer.*/
		record->attribs = (SdpAttribute*)temp;

		/* Copy the list of attributs to the memory right after the record.*/
		OS_MemCopy(temp, (const U8 *)attributeList, attributeListLen);
		curAttr = &record->attribs[0];

		/* Copy every attribute value to the memory right after the list of attributes.*/
		for(i=0; i<numOfAttributs; i++)
		{
			if ( curAttr->len <= BTL_CONFIG_BMG_MAX_ATTRIBUTES_VALUE_SIZE )
			{
				status = BTL_POOL_Allocate(&btlBmgData.sdpAttrValuesPool, (void **)&temp);
				BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating attribute value"));
				OS_MemCopy(temp, (const U8 *) (curAttr->value), curAttr->len);
				curAttr->value = temp;
			}
			else
			{
				BTL_LOG_ERROR(("value for a new attribue is too long %d, skip adding it", curAttr->len));
			}
			curAttr++;
		}

		status = SDP_AddRecord(record);
		
		if (BT_STATUS_SUCCESS != status)
		{
			newStatus = BTL_POOL_Free(&btlBmgData.sdpServiceRecordsPool, (void **)(&tempRecordHandle));
			BTL_VERIFY_ERR((BT_STATUS_SUCCESS == newStatus), newStatus, ("Failed adding service record AND freeing record memory"));
			BTL_ERR(status, ("Failed adding service record"));
		}
	}
	else
	{
		status = Btl_Bmg_UpdateServiceRecord((SdpRecord *) (*recordHandle),  attributeList, numOfAttributs);
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;

}

BtStatus _BTL_BMG_SetDefaultSecurityMode(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("_BTL_BMG_SetDefaultSecurityMode");

	status = BTL_BMG_InternalSetSecurityMode(0, 0, BTL_CONFIG_BMG_DEFAULT_SECURITY_MODE, FALSE);

	BTL_VERIFY_FATAL((status == BT_STATUS_SUCCESS), BT_STATUS_INTERNAL_ERROR, 
						("BTL_BMG_InternalSetSecurityMode Failed (%s)", pBT_Status(status)));
	
	BTL_FUNC_END();

	return status;
}

BtStatus _BTL_BMG_SetDefaultDefaultLinkPolicy(void)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtLinkPolicy dfltLinkInPolicy = 0;
	BtLinkPolicy dfltLinkOutPolicy = 0;

	BTL_FUNC_START("_BTL_BMG_SetDefaultDefaultLinkPolicy");

	dfltLinkInPolicy |= (BTL_CONFIG_BMG_DFLT_IN_ALLOW_MASTER_SLAVE_SWITCH == BTL_CONFIG_ENABLED) ?
						BLP_MASTER_SLAVE_SWITCH : 0;
	dfltLinkInPolicy |= (BTL_CONFIG_BMG_DFLT_IN_ALLOW_HOLD == BTL_CONFIG_ENABLED) ? BLP_HOLD_MODE : 0;
	dfltLinkInPolicy |= (BTL_CONFIG_BMG_DFLT_IN_ALLOW_SNIFF == BTL_CONFIG_ENABLED) ? BLP_SNIFF_MODE : 0;
	dfltLinkInPolicy |= (BTL_CONFIG_BMG_DFLT_IN_ALLOW_PARK == BTL_CONFIG_ENABLED) ? BLP_PARK_MODE : 0;
	dfltLinkInPolicy &= (~BLP_MASK);

	dfltLinkOutPolicy |= (BTL_CONFIG_BMG_DFLT_OUT_ALLOW_MASTER_SLAVE_SWITCH == BTL_CONFIG_ENABLED) ?
						BLP_MASTER_SLAVE_SWITCH : 0;
	dfltLinkOutPolicy |= (BTL_CONFIG_BMG_DFLT_OUT_ALLOW_HOLD == BTL_CONFIG_ENABLED) ? BLP_HOLD_MODE : 0;
	dfltLinkOutPolicy |= (BTL_CONFIG_BMG_DFLT_OUT_ALLOW_SNIFF == BTL_CONFIG_ENABLED) ? BLP_SNIFF_MODE : 0;
	dfltLinkOutPolicy |= (BTL_CONFIG_BMG_DFLT_OUT_ALLOW_PARK == BTL_CONFIG_ENABLED) ? BLP_PARK_MODE : 0;
	dfltLinkOutPolicy &= (~BLP_MASK);

	status = ME_SetDefaultLinkPolicy(dfltLinkInPolicy, dfltLinkOutPolicy);
	BTL_VERIFY_FATAL((status == BT_STATUS_SUCCESS), BT_STATUS_INTERNAL_ERROR, 
						("ME_SetDefaultLinkPolicy Failed (%s)", pBT_Status(status)));

	BTL_FUNC_END();

	return status;
}

BtStatus _BTL_BMG_SetDefaultAccessibilityModes(void)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtAccessibleMode	dfltAccessibilityModeNC = BTL_CONFIG_BMG_DFLT_ACCESSIBILITY_MODE_NC;
	BtAccessibleMode	dfltAccessibilityModeC = BTL_CONFIG_BMG_DFLT_ACCESSIBILITY_MODE_C;

	BTL_FUNC_START("_BTL_BMG_SetDefaultAccessibilityModes");

	status = BTL_BMG_InternalSetAccessibleMode(	0, 
												&btlBmgData.masterPseudoRequest,
												&dfltAccessibilityModeNC, 
												0,
												&dfltAccessibilityModeC,
												0);

	BTL_VERIFY_FATAL((status == BT_STATUS_SUCCESS), BT_STATUS_INTERNAL_ERROR, 
						("BTL_BMG_InternalSetAccessibleMode Failed (%s)", pBT_Status(status)));
	
	BTL_FUNC_END();

	return status;
}

BtStatus _BTL_BMG_SetDefaultClassOfDevice(void)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtClassOfDevice	dfltCod = (BTL_CONFIG_BMG_DEFAULT_MAJOR_SERVICE_CLASS | BTL_CONFIG_BMG_DEFAULT_MAJOR_DEVICE_CLASS | BTL_CONFIG_BMG_DEFAULT_MINOR_DEVICE_CLASS);

	BTL_FUNC_START("_BTL_BMG_SetDefaultClassOfDevice");

	status = _BTL_BMG_InternalSetClassOfDevice(dfltCod);
	BTL_VERIFY_FATAL((status == BT_STATUS_SUCCESS), BT_STATUS_INTERNAL_ERROR,  ("BTL_BMG_InternalSetClassOfDevice Failed (%s)", pBT_Status(status)));
	
	BTL_FUNC_END();

	return status;
}
#if 0
BtStatus _BTL_BMG_SetDefaultPreferredConnectionRole(void)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtClassOfDevice	dfltPreferredConnectionRole = BTL_CONFIG_BMG_DEFAULT_PREFERRED_CONNECTION_ROLE;

	BTL_FUNC_START("_BTL_BMG_SetDefaultPreferredConnectionRole");

	status = _BTL_BMG_InternalSetPreferredConnectionRole(dfltPreferredConnectionRole);
	BTL_VERIFY_FATAL((status == BT_STATUS_SUCCESS), BT_STATUS_INTERNAL_ERROR,  ("BTL_BMG_InternalSetClassOfDevice Failed (%s)", pBT_Status(status)));
	
	BTL_FUNC_END();

	return status;
}
#endif	
BtStatus _BTL_BMG_BtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("_BTL_BMG_BtlNotificationsCb");
	
	BTL_LOG_DEBUG(("Notification Type: %d", notificationType));
	
	switch (notificationType)
	{
		case BTL_MODULE_NOTIFICATION_RADIO_ON:

			status = _BTL_BMG_RadioOnProcessor(_BTL_BMG_RADIO_ON_EVENT_START);
			
			break;

		case BTL_MODULE_NOTIFICATION_RADIO_OFF:

			status = _BTL_BMG_RadioOffProcessor(_BTL_BMG_RADIO_OFF_EVENT_START, 0);

			break;

		default:

			BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid notification (%d)", notificationType));
			


	};

	BTL_FUNC_END();
	
	return status;
}

void _BTL_BMG_DdbNotificationsCb(BtlBmgDdbNotificationType notificationType, BtlBmgDeviceRecord *record)
{
	BtDeviceContext	*currDevice = NULL;
		
	BTL_FUNC_START("_BTL_BMG_DdbNotificationsCb");

	currDevice = DS_FindDevice(&record->bdAddr);;
	BTL_VERIFY_FATAL_NO_RETVAR((currDevice != NULL), ("Device (%s) not found in DS DB", BTL_UTILS_LockedBdaddrNtoa(&record->bdAddr)));

	if (	(notificationType == BTL_BMG_DDB_NOTIFICATION_TYPE_RECORD_ADDED) ||
		(notificationType == BTL_BMG_DDB_NOTIFICATION_TYPE_RECORD_MODIFIED))
	{
		/* Synchronize COD between DS and DDB. A 0 COD indicates that the value was never set */
		
		if (currDevice->classOfDevice != 0)
		{
			BTL_LOG_INFO(	("DDB Record Added / Modified: Updating DDB COD of (%s) from DS to %x", 
							BTL_UTILS_LockedBdaddrNtoa(&record->bdAddr), currDevice->classOfDevice));
			
			record->classOfDevice = currDevice->classOfDevice;
		}
		else if (record->classOfDevice != 0)
		{
			BTL_LOG_INFO(	("DDB Record Added / Modified:  Updating DS COD of (%s) from DDB to %x", 
							BTL_UTILS_LockedBdaddrNtoa(&record->bdAddr), record->classOfDevice));
			
			currDevice->classOfDevice = record->classOfDevice;
		}
		else
		{
			BTL_LOG_INFO(	("DDB Record Added / Modified: COD is unknown at this stage for %s", BTL_UTILS_LockedBdaddrNtoa(&record->bdAddr)));
		}

		/* Synchronize psMode */

		if (record->psi.psMode == BTL_BMG_DDB_INVALID_PSI_MODE)
		{
			BTL_LOG_INFO(	("DDB Record Added / Modified: Updating DDB psMode of (%s) from DS to %d", 
							BTL_UTILS_LockedBdaddrNtoa(&record->bdAddr), currDevice->psi.psMode));
			
			record->psi.psMode = currDevice->psi.psMode;
		}
		else
		{
			BTL_LOG_INFO(("DDB Record Added / Modified and has some psMode value - leaving it (%s)", 
							BTL_UTILS_LockedBdaddrNtoa(&record->bdAddr)));
		}

		/* Synchronize psRepMode */
		
		if (record->psi.psRepMode == BTL_BMG_DDB_INVALID_PSI_REP_MODE)
		{
			BTL_LOG_INFO(	("DDB Record Added / Modified: Updating DDB psRepMode of (%s) from DS to %d", 
							BTL_UTILS_LockedBdaddrNtoa(&record->bdAddr), currDevice->psi.psRepMode));
			
			record->psi.psRepMode = currDevice->psi.psRepMode;
		}
		else
		{
			BTL_LOG_INFO(("DDB Record Added / Modified and has some psRepMode value - leaving it (%s)", 
							BTL_UTILS_LockedBdaddrNtoa(&record->bdAddr)));
		}
	}

	BTL_FUNC_END();		
}

BOOL _BTL_BMG_RadioOnProcessorCb(const BtEvent *event, BtlBmgRequest *request)
{
	BOOL eventHandled = TRUE;
	
	BTL_FUNC_START("_BTL_BMG_RadioOnProcessorCb");
	
	UNUSED_PARAMETER(request);

	switch (event->eType)
	{
		case BTEVENT_SECURITY_CHANGE:

			/* Ignore - Wait for the BTEVENT_SET_SECURITY_MODE_RESULT event */
			/* [@ToDo][UdiR] Forward the event to the application */
			break;
			
		case BTEVENT_SET_SECURITY_MODE_RESULT:

			_BTL_BMG_RadioOnProcessor(_BTL_BMG_RADIO_ON_EVENT_SECURITY_MODE_SET);
			
			break;

		case BTEVENT_ACCESSIBLE_CHANGE:

			_BTL_BMG_RadioOnProcessor(_BTL_BMG_RADIO_ON_EVENT_ACCESSIBILITY_MODES_SET);
			
			break;
			
		default:

			BTL_ERR_SET_RETVAR((eventHandled = FALSE), ("Invalid Event (%d)", pME_Event(event->eType)));
	};

	BTL_FUNC_END();

	return eventHandled;
}

BtStatus _BTL_BMG_RadioOnProcessor(_BtlBmgRadioOnEventType event)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	BOOL 	keepProcessing = TRUE;

	BTL_FUNC_START("_BTL_BMG_RadioOnProcessor");

	while (keepProcessing == TRUE)
	{
		keepProcessing = FALSE;
		
		switch (btlBmgData.radioOnData.state)
		{
			case _BTL_BMG_RADIO_ON_STATE_NONE:

				BTL_VERIFY_ERR((event == _BTL_BMG_RADIO_ON_EVENT_START), BT_STATUS_INTERNAL_ERROR,
								("Invalid Event (%d)", event));
				
				status = BTL_BMG_HandleCompoundRequestCreation(
							&btlBmgData.radioOnData.radioOnRequest, 
							BTL_BMG_REQUEST_TYPE_RADIO_ON,
							&btlBmgData.masterPseudoRequest,
							0, 
							_BTL_BMG_RadioOnProcessorCb, 
							0);
				BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

				btlBmgData.radioOnData.asynchronous = FALSE;

				_BTL_BMG_MakeSureAccessibilityModesWillBeAppliedToChip();
				_BTL_BMG_MakeSureSecurityModeWillBeAppliedToChip();
				
				btlBmgData.radioOnData.state = _BTL_BMG_RADIO_ON_STATE_SET_SECURITY_MODE;
				keepProcessing = TRUE;
				
				break;

			case _BTL_BMG_RADIO_ON_STATE_SET_SECURITY_MODE:

				status = BTL_BMG_InternalSetSecurityMode(	0, 
															btlBmgData.radioOnData.radioOnRequest, 
															btlBmgData.securityMode,
															TRUE);

				if (status == BT_STATUS_SUCCESS)
				{
					btlBmgData.radioOnData.state = _BTL_BMG_RADIO_ON_STATE_SET_ACCESSIBILITY_MODES;
					keepProcessing = TRUE;
				}
				else
				{
					BTL_VERIFY_ERR((status == BT_STATUS_PENDING), BT_STATUS_INTERNAL_ERROR, (""));

					btlBmgData.radioOnData.state = _BTL_BMG_RADIO_ON_STATE_SETTING_SECURITY_MODE;						
				}
				
				break;

			case _BTL_BMG_RADIO_ON_STATE_SETTING_SECURITY_MODE:

				BTL_VERIFY_ERR((event == _BTL_BMG_RADIO_ON_EVENT_SECURITY_MODE_SET),
								BT_STATUS_INTERNAL_ERROR, ("Invalid Event (%d)", event));

				btlBmgData.radioOnData.state = _BTL_BMG_RADIO_ON_STATE_SET_ACCESSIBILITY_MODES;
				keepProcessing = TRUE;

				break;

			case _BTL_BMG_RADIO_ON_STATE_SET_ACCESSIBILITY_MODES:

				status = BTL_BMG_InternalSetAccessibleMode(	0, 
															btlBmgData.radioOnData.radioOnRequest,
															&btlBmgData.accessibilityData.accessibleModeNC,
															&btlBmgData.accessibilityData.accessModeInfoNC,
															&btlBmgData.accessibilityData.accessibleModeC,
															&btlBmgData.accessibilityData.accessModeInfoC);
				
				if (status == BT_STATUS_SUCCESS)
				{
					btlBmgData.radioOnData.state = _BTL_BMG_RADIO_ON_STATE_DONE;
					keepProcessing = TRUE;
				}
				else
				{
					BTL_VERIFY_ERR((status == BT_STATUS_PENDING), BT_STATUS_INTERNAL_ERROR, (""));

					btlBmgData.radioOnData.state = _BTL_BMG_RADIO_ON_STATE_SETTING_ACCESSIBILITY_MODES;
				}

				break;

			case _BTL_BMG_RADIO_ON_STATE_SETTING_ACCESSIBILITY_MODES:

				BTL_VERIFY_ERR((event == _BTL_BMG_RADIO_ON_EVENT_ACCESSIBILITY_MODES_SET),
								BT_STATUS_INTERNAL_ERROR, ("Invalid Event (%d)", event));

				btlBmgData.radioOnData.state = _BTL_BMG_RADIO_ON_STATE_DONE;
				keepProcessing = TRUE;

				break;
				
			case _BTL_BMG_RADIO_ON_STATE_DONE:

				/* There is no need for the radio on request anymore */
				_BTL_BMG_HandleTopLevelProcessCompletion((BtlBmgRequest*)(&btlBmgData.radioOnData.radioOnRequest->base));
				
				if (btlBmgData.radioOnData.asynchronous == TRUE)
				{
					BtlModuleNotificationCompletionEvent	moduleCompletionEvent;
					moduleCompletionEvent.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_ON;
					moduleCompletionEvent.status = BT_STATUS_SUCCESS;
					
					BTL_ModuleCompleted(BTL_MODULE_TYPE_BMG, &moduleCompletionEvent);
				}

				btlBmgData.radioOnData.state = _BTL_BMG_RADIO_ON_STATE_NONE;
				
				break;

			default:

				BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid "));
		};
	}

	if (status == BT_STATUS_PENDING)
	{
		btlBmgData.radioOnData.asynchronous = TRUE;
	}
	
	BTL_FUNC_END();

	return status;
}

BtStatus _BTL_BMG_DisableScans()
{
	BtStatus 			status = BT_STATUS_SUCCESS;

    static const U8		writeScanDisableParms[] = {0x0};

	BTL_FUNC_START("_BTL_BMG_DisableScans");

	BTL_LOG_INFO(("Disabling Inquiry & Page Scans"));
	
	status = 	BTL_BMG_InternalSendHciCommand(	btlBmgData.radioOffData.radioOffRequest->base.bmgContext,
												btlBmgData.radioOffData.radioOffRequest,
												HCC_WRITE_SCAN_ENABLE,
												sizeof(writeScanDisableParms),
												writeScanDisableParms,
												HCE_COMMAND_COMPLETE);
	BTL_VERIFY_FATAL((BT_STATUS_PENDING == status), status, (""));

	BTL_FUNC_END();
	
	return status;
}

BOOL BTL_BMG_RadioOffProcessorCb(const BtEvent *event, BtlBmgRequest *request)
{
	BTL_FUNC_START("BTL_BMG_RadioOffProcessorCb");

	UNUSED_PARAMETER(request);

	BTL_VERIFY_FATAL_NORET(	request == &btlBmgData.radioOffData.radioOffRequest->base, 
								("Unexpected request (%s)", BTL_BMG_DEBUG_pRequestType(request->requestType)));
	
	_BTL_BMG_RadioOffProcessor(_BTL_BMG_RADIO_OFF_EVENT_REQUEST_CB_CALLED, (void*)event);

	BTL_FUNC_END();
	
	return TRUE;
}

BtStatus _BTL_BMG_RadioOffCancelProcessesInProgress(void)
{
	BtStatus 				status = BT_STATUS_SUCCESS;
	BtlBmgRequest		*request = 0;
	BtlBmgRequest		*nextRequest = 0;
	BtlBmgRequestType	requestType;
	BTL_FUNC_START("_BTL_BMG_RadioOffCancelProcessesInProgress");

	BTL_LOG_DEBUG(("Cancelling processes in progress"));
	
	btlBmgData.radioOffData.numOfProcessesWaitingCancellation = 0;
	
	IterateListSafe(btlBmgData.masterPseudoRequest.childRequests, request, nextRequest, BtlBmgRequest*)
	{
		status = BT_STATUS_INTERNAL_ERROR;

		requestType = request->requestType;
		
		BTL_LOG_INFO(("Cancelling %s", BTL_BMG_DEBUG_pRequestType(requestType)));

		++btlBmgData.radioOffData.numOfProcessesWaitingCancellation;
		
		switch (requestType)
		{
			case BTL_BMG_REQUEST_TYPE_SEARCH_BY_COD:
			case BTL_BMG_REQUEST_TYPE_SEARCH_BY_DEVICES:

				status = _BTL_BMG_InternalCancelSearch(request->bmgContext);
				
			break;

			case BTL_BMG_REQUEST_TYPE_INQUIRY:

				status = BT_STATUS_INTERNAL_ERROR;
					
			break;

			case BTL_BMG_REQUEST_TYPE_NAME_REQUEST:

				status = _BTL_BMG_CancelGetRemoteDeviceName(request->bmgContext, &request->bdAddress);

			break;

			case BTL_BMG_REQUEST_TYPE_SEND_HCI_COMMAND:

				/* Impossible to cancel an HCI command, just count it so that we would wait for its completion */
				status = BT_STATUS_PENDING;

			break;

			case BTL_BMG_REQUEST_TYPE_DISCOVER_SERVICES:
				status = _BTL_BMG_CancelDiscoverServices(request->bmgContext);
				
			break;

			case BTL_BMG_REQUEST_TYPE_BOND:

				status = _BTL_BMG_CancelBond(request->bmgContext, &request->bdAddress);
				
			break;

			case BTL_BMG_REQUEST_TYPE_CREATE_LINK:

				status = BT_STATUS_INTERNAL_ERROR;

			break;

			case BTL_BMG_REQUEST_TYPE_SET_SECURITY_MODE:
				
				/* Impossible to cancel a Set Security Mode Command, just count it so that we would wait for its completion */
				status = BT_STATUS_PENDING;

			break;

			case BTL_BMG_REQUEST_TYPE_ENABLE_SEC_MODE_3:
			case BTL_BMG_REQUEST_TYPE_DISABLE_SEC_MODE_3:
			case BTL_BMG_REQUEST_TYPE_AUTHENTICATE_LINK:
				
				status = BT_STATUS_INTERNAL_ERROR;

			break;

			case BTL_BMG_REQUEST_TYPE_ENABLE_TEST_MODE:

				status = BT_STATUS_INTERNAL_ERROR;
			break;
			
			case BTL_BMG_REQUEST_TYPE_RADIO_ON:

				status = BT_STATUS_INTERNAL_ERROR;
				
			break;

			case BTL_BMG_REQUEST_TYPE_SET_LINK_ENCRYPTION:

				/* Impossible to cancel a Set Link Encryption Command, just count it so that we would wait for its completion */
				status = BT_STATUS_PENDING;
				
			break;

			case BTL_BMG_REQUEST_TYPE_SET_CONTROL_INCOMING_CON_ACCEPT:

				/* Skip. This is not a process */
				--btlBmgData.radioOffData.numOfProcessesWaitingCancellation;
				status = BT_STATUS_SUCCESS;

			case BTL_BMG_REQUEST_TYPE_SERVICE_SEARCH_ATTR:

				/* 
					[@ToDo][Udi] When mutliple Search services processes may run simultaneously, we would have to supply the correct
					request id (not necessarily 0)
				*/
				status = _BTL_BMG_CancelServiceSearchAttributeRequest(request->bmgContext, 0);

			break;

			case BTL_BMG_REQUEST_TYPE_INTERCEPT_HCI_INIT_EVENTS:

				/* Skip. This is not a process */
				--btlBmgData.radioOffData.numOfProcessesWaitingCancellation;
				status = BT_STATUS_SUCCESS;

			break;

			case BTL_BMG_REQUEST_TYPE_RADIO_OFF:

				/* Skip - This is the process we are part of */
				--btlBmgData.radioOffData.numOfProcessesWaitingCancellation;
				status = BT_STATUS_SUCCESS;

			break;
			case BTL_BMG_REQUEST_TYPE_SERVICE_SEARCH:

				
				status = _BTL_BMG_CancelServiceSearchRequest(request->bmgContext, 0);

			break;
			case BTL_BMG_REQUEST_TYPE_SERVICE_ATTR:

				
				status = _BTL_BMG_CancelServiceAttributeRequest(request->bmgContext, 0);

			break;

			default:

				BTL_FATAL_NORET(("Unrecognized Request Type while cancelling processes in progress (%d)", request->requestType));

				/* Skip this process after notifying the bug*/
				--btlBmgData.radioOffData.numOfProcessesWaitingCancellation;
				status = BT_STATUS_SUCCESS;

		}

		if (status == BT_STATUS_SUCCESS)
		{
			BTL_LOG_INFO(("Request Cancelled Immediately"));
		}
		else if (status == BT_STATUS_PENDING)
		{			
			BTL_LOG_INFO(("Waiting for %s to complete, Waiting Count: %d", 
							BTL_BMG_DEBUG_pRequestType(requestType), btlBmgData.radioOffData.numOfProcessesWaitingCancellation));
		}
		else
		{
			BTL_FATAL_NORET(("Failed cancelling %s (%s)", BTL_BMG_DEBUG_pRequestType(requestType), pBT_Status(status)));
			--btlBmgData.radioOffData.numOfProcessesWaitingCancellation;
		}
	}

	BTL_FUNC_END();

	if (btlBmgData.radioOffData.numOfProcessesWaitingCancellation > 0)
	{
		BTL_LOG_INFO(("Waiting for %d processes to terminate", btlBmgData.radioOffData.numOfProcessesWaitingCancellation));
		
		status = BT_STATUS_PENDING;
	}
	else
	{
		BTL_LOG_INFO(("No process currently in progress any more"));
		
		status = BT_STATUS_SUCCESS;
	}
	
	return status;
}

BtStatus _BTL_BMG_RadioOffProcessCancelled()
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("_BTL_BMG_RadioOffProcessCancelled");

	BTL_VERIFY_FATAL(btlBmgData.radioOffData.numOfProcessesWaitingCancellation > 0, BT_STATUS_INTERNAL_ERROR,
						("All processes already cancelled"));
	
	--btlBmgData.radioOffData.numOfProcessesWaitingCancellation;

	if (btlBmgData.radioOffData.numOfProcessesWaitingCancellation == 0)
	{
		status = BT_STATUS_SUCCESS;
	}
	else
	{
		BTL_LOG_INFO(("Still waiting for %d Processes to terminate", btlBmgData.radioOffData.numOfProcessesWaitingCancellation));
		status = BT_STATUS_PENDING;
	}
	
	BTL_FUNC_END();

	return status;
}

BtStatus _BTL_BMG_RadioOffProcessor(_BtlBmgRadioOffEventType event, void *additionalData)
{
	BtStatus 							status = BT_STATUS_SUCCESS;
	BOOL							keepProcessing = TRUE;
	const BtEvent						*btEvent = 0;
	const BtlBmgRequest				*request = 0;

	BTL_FUNC_START("_BTL_BMG_RadioOffProcessor");
	
	while (keepProcessing == TRUE)
	{
		BTL_LOG_DEBUG(("State: %d, Event: %d", btlBmgData.radioOffData.state, event));

		keepProcessing = FALSE;
		
		switch (btlBmgData.radioOffData.state)
		{
			case _BTL_BMG_RADIO_OFF_STATE_NONE:

				btlBmgData.radioOffData.asynchronous = FALSE;
				btlBmgData.radioOffData.numOfProcessesWaitingCancellation = 0;
										
				status = BTL_BMG_HandleCompoundRequestCreation(	&btlBmgData.radioOffData.radioOffRequest, 
																	BTL_BMG_REQUEST_TYPE_RADIO_OFF,
																	&btlBmgData.masterPseudoRequest,
																	0, 
																	BTL_BMG_RadioOffProcessorCb, 
																	0);
				BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

				btlBmgData.radioOffData.status = BT_STATUS_SUCCESS;
				
				btlBmgData.radioOffData.state = _BTL_BMG_RADIO_OFF_STATE_DISABLE_SCANS;
				keepProcessing = TRUE;

			break;

			case _BTL_BMG_RADIO_OFF_STATE_DISABLE_SCANS:

				BTL_LOG_INFO(("Disabling Inquiry & Page Scans"));

				status = _BTL_BMG_DisableScans();

				if (status == BT_STATUS_SUCCESS)
				{
					BTL_LOG_INFO(("Scans Disabled"));
					
					btlBmgData.radioOffData.state = _BTL_BMG_RADIO_OFF_STATE_CANCEL_PROCESSES_IN_PROGRESS;
					keepProcessing = TRUE;
				}
				else if (status == BT_STATUS_PENDING)
				{
					btlBmgData.radioOffData.asynchronous = TRUE;
					btlBmgData.radioOffData.state = _BTL_BMG_RADIO_OFF_STATE_WAITING_FOR_SCANS_DISABLING;
				}
				else
				{
					btlBmgData.radioOffData.status = status;

					btlBmgData.radioOffData.state = _BTL_BMG_RADIO_OFF_STATE_DONE;
					keepProcessing = TRUE;
				}
	
			break;

			case _BTL_BMG_RADIO_OFF_STATE_WAITING_FOR_SCANS_DISABLING:

				switch (event)
				{
					case _BTL_BMG_RADIO_OFF_EVENT_PROCESS_COMPLETED:

						/* Do nothing - process completed before we reach the cancellation phase */
						BTL_LOG_DEBUG(("Completed before cancellation phase, ignoring"));

					break;

					case _BTL_BMG_RADIO_OFF_EVENT_REQUEST_CB_CALLED:

						btEvent = additionalData;
					
						BTL_VERIFY_FATAL(btEvent != 0, BT_STATUS_INTERNAL_ERROR, ("Null btEvent, State: %d", btlBmgData.radioOffData.state));
						BTL_VERIFY_FATAL(btEvent->eType == BTEVENT_SEND_HCI_COMMAND_RESULT, BT_STATUS_INTERNAL_ERROR, 
											("Unexpected Event Type (%s), State: %d", pME_Event(btEvent->eType), btlBmgData.radioOffData.state));
						BTL_VERIFY_FATAL(btEvent->errCode == BEC_NO_ERROR, BT_STATUS_INTERNAL_ERROR, 
											("Failed Sending HCI Command to Disable Scans (%s), State: %d", 
											pHC_Status(btEvent->errCode), btlBmgData.radioOffData.state));

						BTL_LOG_INFO(("Scans Disabled"));
						
						btlBmgData.radioOffData.state = _BTL_BMG_RADIO_OFF_STATE_CANCEL_PROCESSES_IN_PROGRESS;
						keepProcessing = TRUE;

					break;

					default:

						BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Unexpected Radio Off Event (%d) while waiting for scans disabling", event));
						
				};
				
			break;

			case _BTL_BMG_RADIO_OFF_STATE_CANCEL_PROCESSES_IN_PROGRESS:

				btlBmgData.radioOffData.state = _BTL_BMG_RADIO_OFF_STATE_CALLING_CANCEL_PROCESSES_IN_PROGRESS;
				
				status = _BTL_BMG_RadioOffCancelProcessesInProgress();

				if (status == BT_STATUS_SUCCESS)
				{
					BTL_LOG_INFO(("Processes in Progress Cancelled"));
					
					btlBmgData.radioOffData.state = _BTL_BMG_RADIO_OFF_STATE_DONE;
					keepProcessing = TRUE;
				}
				else if (status == BT_STATUS_PENDING)
				{
					btlBmgData.radioOffData.state = _BTL_BMG_RADIO_OFF_STATE_WAITING_FOR_PROCESSES_CANCELLATIONS;
				}
				else
				{
					btlBmgData.radioOffData.status = status;

					btlBmgData.radioOffData.state = _BTL_BMG_RADIO_OFF_STATE_DONE;
					keepProcessing = TRUE;
				}
				
			break;

			case _BTL_BMG_RADIO_OFF_STATE_CALLING_CANCEL_PROCESSES_IN_PROGRESS:			
			case _BTL_BMG_RADIO_OFF_STATE_WAITING_FOR_PROCESSES_CANCELLATIONS:

				request = additionalData;
				
				BTL_VERIFY_FATAL(event == _BTL_BMG_RADIO_OFF_EVENT_PROCESS_COMPLETED,
									BT_STATUS_INTERNAL_ERROR, ("Unexpected Event (%d) while Cancelling Processes", event));
				BTL_VERIFY_FATAL(event == _BTL_BMG_RADIO_OFF_EVENT_PROCESS_COMPLETED,
									BT_STATUS_INTERNAL_ERROR, ("Unexpected Event (%d) while Cancelling Processes", event));
				BTL_VERIFY_FATAL(request != 0, BT_STATUS_INTERNAL_ERROR, ("Null request while Cancelling Processes"));

				status = _BTL_BMG_RadioOffProcessCancelled();

				if (btlBmgData.radioOffData.state == _BTL_BMG_RADIO_OFF_STATE_WAITING_FOR_PROCESSES_CANCELLATIONS)
				{
					if (status == BT_STATUS_SUCCESS)
					{
						BTL_LOG_INFO(("Processes in Progress Cancelled"));

						btlBmgData.radioOffData.state = _BTL_BMG_RADIO_OFF_STATE_DONE;
						keepProcessing = TRUE;
					}
					else if (status != BT_STATUS_PENDING)
					{
						btlBmgData.radioOffData.status = status;

						btlBmgData.radioOffData.state = _BTL_BMG_RADIO_OFF_STATE_DONE;
						keepProcessing = TRUE;
					}
				}
				
			break;
			
			case _BTL_BMG_RADIO_OFF_STATE_DONE:

				BTL_LOG_INFO(("Completed Preparations for Radio Off"));
				
				btlBmgData.radioOffData.state = _BTL_BMG_RADIO_OFF_STATE_NONE;

				/* There is no need for the radio on request anymore */
				_BTL_BMG_HandleTopLevelProcessCompletion((BtlBmgRequest*)(&btlBmgData.radioOffData.radioOffRequest->base));

				if (btlBmgData.radioOffData.asynchronous == TRUE)
				{
					BtlModuleNotificationCompletionEvent	moduleCompletionEvent;

					BTL_LOG_INFO(("Notifying BTL Common on completion"));
					
					moduleCompletionEvent.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
					moduleCompletionEvent.status = btlBmgData.radioOffData.status;
					
					BTL_ModuleCompleted(BTL_MODULE_TYPE_BMG, &moduleCompletionEvent);
				}
			
			break;
			
			default:

				BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Unexpected Radio Off State (%d)", btlBmgData.radioOffData.state));
				
		};
	};
	
	BTL_FUNC_END();

	return status;
}

BtStatus BTL_BMG_SetHciInitEventsHandlingMode(BOOL blockEvents)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_SetHciInitEventsHandlingMode");

	btlBmgData.blockHciInitEvents = blockEvents;
	
	BTL_FUNC_END();

	return status;
}

BtStatus BTL_BMG_GenerateHciInitEvent(BtEventType eventType)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	BtEvent	event;

	BTL_FUNC_START("BTL_BMG_GenerateHciInitEvent");

	event.eType = eventType;
	event.errCode = BEC_NO_ERROR;
	
	status = BTL_BMG_EVM_DispatchEvent(0, &event, BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);
	BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, BT_STATUS_INTERNAL_ERROR, ("Failed dispatching generated event (%s)", pME_Event(eventType)));
	
	BTL_FUNC_END();

	return status;
}

BtStatus btlBmgInterceptHciEvents(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("btlBmgInterceptHciEvents");

	status = BTL_BMG_HandleRequestCreation(	&btlBmgData.hciInitInterceptionRequest, 
											BTL_BMG_REQUEST_TYPE_INTERCEPT_HCI_INIT_EVENTS,
											&btlBmgData.masterPseudoRequest,
											0, 
											btlBmgHciInitEventCb, 
											0);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Failed Creating HCI Init Interception Request (%s)", pBT_Status(status)));
	
	status = BTL_BMG_EVM_RegisterForStaticEvents(	btlBmgHciInitEvents, 
													btlBmgNumOfHciInitStaticEvents,
													btlBmgData.hciInitInterceptionRequest);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("BTL_BMG_EVM_RegisterForStaticEvents Failed (%s)", pBT_Status(status)));

	BTL_FUNC_END();

	return status;

	/* [@ToDo][UdiR] Destroy the request when de-initializing */
	
}

BOOL btlBmgHciInitEventCb(const BtEvent *event, BtlBmgRequest *request)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("btlBmgHciInitEventCb");
	
	if (btlBmgData.blockHciInitEvents == FALSE)
	{
		status = BTL_BMG_EVM_DispatchEvent(request, event, BTL_BMG_EVENT_FORWARDING_ALL_EVENTS);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, BT_STATUS_INTERNAL_ERROR, ("Failed dispatching HCI event (%s)", pME_Event(event->eType)));
	}
	else
	{
		BTL_LOG_INFO(("Blocking %s", pME_Event(event->eType)));
	}

	BTL_FUNC_END();
	
	return TRUE;
}

/*------------------------------------------------------------------
 *            Btl_Bmg_FindOrigAttr()
 *------------------------------------------------------------------
 *
 * Synopsis:  	
 *		searches for an existing attribute in an attribute list of a service record
 *		the search is according to the attribute id
 *
 * Return:    the index of the attribute if found, else 0xFF 
*
 */
U8 Btl_Bmg_FindOrigAttr(SdpRecord *sdpRecord, SdpAttributeId searchedId )
{
	U8 ret = 0xFF;
	U8 i ;

	BTL_FUNC_START("Btl_Bmg_FindOrigAttr");
	
	for ( i = 0 ; i < sdpRecord->num ; i++ )
	{
		if ( sdpRecord->attribs[i].id == searchedId )
		{
			BTL_LOG_DEBUG(("attribute id %d found in at index %d", searchedId, i));
			ret = i;
			break;
		}
	}

	if ( 0xFF == ret )
		BTL_LOG_DEBUG(("attribute id %d not found in list", searchedId));

	BTL_FUNC_END();

	return ret;
	
}

/*------------------------------------------------------------------
 *            Btl_Bmg_DeleteAttr()
 *------------------------------------------------------------------
 *
 * Synopsis:  	
 *		deletes a given existing attribute from the attribute list of a service record
 *
 */
void Btl_Bmg_DeleteAttr(SdpRecord *sdpRecord, U8  attrInd)
{
	U8 i;

	BTL_FUNC_START("Btl_Bmg_DeleteAttr");
	
	BTL_LOG_DEBUG(("deleting attribute at index %d", attrInd));

	for ( i = attrInd ; i < sdpRecord->num -1 ; i++ )
	{
		OS_MemCopy((U8*)&sdpRecord->attribs[i], (U8*)&sdpRecord->attribs[i+1], sizeof(SdpAttribute));
	}
	sdpRecord->num--;

	BTL_FUNC_END();

}


/*------------------------------------------------------------------
 *            Btl_Bmg_ModifyAttrValue()
 *------------------------------------------------------------------
 *
 * Synopsis:  	modifies an existing attribute in the attribute list of a service record
 *
 * Return:    BT_STATUS_SUCCESS if success, BT_STATUS_FAILED if failure, 
 *            and BT_STATUS_INVALID_PARM if the parameters are invalid.
 *		(if value length exceeds maximum allowed)
 */
BtStatus Btl_Bmg_ModifyAttrValue( SdpAttribute *attrToModify, const SdpAttribute *newAttr, BOOL *updated )
{
	BtStatus 	status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("Btl_Bmg_ModifyAttrValue");

	*updated = FALSE;
	
	BTL_VERIFY_ERR((newAttr->len <= BTL_CONFIG_BMG_MAX_ATTRIBUTES_VALUE_SIZE), 
		BT_STATUS_INVALID_PARM, ("new attribute value is too long (%d, max allowed %d)",
		newAttr->len, BTL_CONFIG_BMG_MAX_ATTRIBUTES_VALUE_SIZE));

	*updated = ( ! OS_MemCmp(attrToModify->value, attrToModify->len, newAttr->value, newAttr->len) );
	BTL_LOG_DEBUG(("updated = %d (lengths are equal = %d)", *updated, attrToModify->len == newAttr->len));

	OS_MemCopy( (U8*) attrToModify->value, (U8*) newAttr->value, newAttr->len);
	attrToModify->len = newAttr->len;

	BTL_FUNC_END();
	
	return status;

}


/*------------------------------------------------------------------
 *            Btl_Bmg_AddAttr()
 *------------------------------------------------------------------
 *
 * Synopsis:  	add a new attribute to the attribute list in a service record
 *
 * Return:    BT_STATUS_SUCCESS if success, BT_STATUS_FAILED if failure, 
 *            and BT_STATUS_INVALID_PARM if the parameters are invalid.
 *		(e.g. if value length exceeds maximum allowed,
 *		or if pool of attributes is already full)
 */
BtStatus Btl_Bmg_AddAttr( SdpRecord *sdpRecord, const SdpAttribute *newAttr)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	U8 *temp;

	BTL_FUNC_START("Btl_Bmg_AddAttr");

	BTL_VERIFY_ERR( (BTL_CONFIG_BMG_MAX_ATTRIBUTES_PER_SERVICE_RECORD > sdpRecord->num ), 
		BT_STATUS_INVALID_PARM, ("cannot add a new attribute, number already equals the maximum %d",
		BTL_CONFIG_BMG_MAX_ATTRIBUTES_PER_SERVICE_RECORD));
	BTL_VERIFY_ERR((NULL != newAttr->value), BT_STATUS_INVALID_PARM, ("NULL value for a new attribue"));
	BTL_VERIFY_ERR((newAttr->len <= BTL_CONFIG_BMG_MAX_ATTRIBUTES_VALUE_SIZE), 
		BT_STATUS_INVALID_PARM, ("value for a new attribue is too long %d", newAttr->len));

	status = BTL_POOL_Allocate(&btlBmgData.sdpAttrValuesPool, (void**)&temp);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating attribute value"));

	OS_MemCopy(temp, (const U8 *) (newAttr->value), newAttr->len);
		/* temp[newAttr->len] = 0; */
		/* BTL_LOG_DEBUG(("new value |%s|, %x %x %x %x %x", temp, temp[0], temp[1], temp[2], temp[3], temp[4] ));*/

	sdpRecord->attribs[sdpRecord->num].id = newAttr->id;
	sdpRecord->attribs[sdpRecord->num].len= newAttr->len;
	sdpRecord->attribs[sdpRecord->num].value = temp;

	sdpRecord->num++;

	BTL_FUNC_END();

	return status;

}
				
/*------------------------------------------------------------------
 *            Btl_Bmg_UpdateServiceRecord()
 *------------------------------------------------------------------
 *
 * Synopsis:  	update a servive record using an attribute list to
 *			add/delete/modify attributes this is done according the definition
 *			of operations in BTL_BMG_SetServiceRecord API
 *
 * Return:    BT_STATUS_SUCCESS if success, BT_STATUS_FAILED if failure, 
 *            and BT_STATUS_INVALID_PARM if the parameters are invalid.
 */
BtStatus Btl_Bmg_UpdateServiceRecord(	SdpRecord* sdpRecord,
											const SdpAttribute *updateAttributeList,
											const U32 numOfUpdateAttributs)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	U8 origAttrInd;
	const SdpAttribute *newAttr;
	U8 i;
	BOOL updated = FALSE;

	BTL_FUNC_START("Btl_Bmg_UpdateServiceRecord");

	BTL_LOG_DEBUG(("current list len %d, updating list len %d", sdpRecord->num, numOfUpdateAttributs));

	SDP_LockRecord(sdpRecord);
	
	for ( i = 0 ; i < numOfUpdateAttributs ; i++ )
	{
		newAttr = &updateAttributeList[i];
		if ( 0xFF != (origAttrInd = Btl_Bmg_FindOrigAttr(sdpRecord, newAttr->id )) )
		{
			if ( NULL == newAttr->value )
			{
				Btl_Bmg_DeleteAttr(sdpRecord, origAttrInd);
				updated = TRUE;
			}
			else
			{
				status = Btl_Bmg_ModifyAttrValue(&(sdpRecord->attribs[origAttrInd]), newAttr, &updated);
				BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed to modify attribute value"));
			}
		}
		else
		{
			status = Btl_Bmg_AddAttr( sdpRecord, newAttr);
			BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed to add a new attribute"));
			updated = TRUE;
		}
	}

	SDP_UnlockRecord(sdpRecord, updated);
	
	BTL_FUNC_END();

	return status;

}

void BtlBmgMonitorEventHandler(const BtEvent *event)
{		
	BTL_FUNC_START("BtlBmgMonitorEventHandler");
	
	if (event->eType == BTEVENT_DEVICE_ADDED)
	{
		BtlBmgDeviceRecord	bmgDdbRecord;
		
		BtDeviceContext 		*bdc = event->p.device;
		BTL_VERIFY_ERR_NO_RETVAR((bdc != NULL), ("Null bdc"));
		
		 if (BTL_BMG_DDB_FindRecord(&bdc->addr,  &bmgDdbRecord) == BT_STATUS_SUCCESS)
		 {
		 	if (bdc->classOfDevice == 0)
		 	{
		 		bdc->classOfDevice = bmgDdbRecord.classOfDevice;
		 	}
		 }
	}

	BTL_FUNC_END();
}

BtStatus _BTL_BMG_InitDsFromDdb(void)
{
	BtStatus				status;
	BtlBmgDeviceRecord	deviceRecord;
	U32					numOfDeviceRecords = 0;
	U32					recordIndex = 0;
	BtDeviceContext     	device;

	BTL_FUNC_START("_BTL_BMG_InitDsFromDdb");
	
	status = BTL_BMG_DDB_GetNumOfDeviceRecords(&numOfDeviceRecords);
	BTL_VERIFY_FATAL(status == BT_STATUS_SUCCESS, status, ("BTL_BMG_DDB_GetNumOfDeviceRecords Failed"));

	BTL_LOG_INFO(("_BTL_BMG_InitDsFromDdb: There are %d DDB Records", numOfDeviceRecords));
	
	for (recordIndex = 0; recordIndex < numOfDeviceRecords; ++recordIndex)
	{
		status = BTL_BMG_DDB_EnumDeviceRecords(recordIndex, &deviceRecord);
		BTL_VERIFY_FATAL(status == BT_STATUS_SUCCESS, status, ("BTL_BMG_DDB_EnumDeviceRecords Failed (%s)", pBT_Status(status)));

		OS_MemSet((U8 *)&device, 0, (U32)sizeof(BtDeviceContext));
		
		device.addr = deviceRecord.bdAddr;

		if (deviceRecord.psi.psMode != BTL_BMG_DDB_INVALID_PSI_MODE)
		{
			device.psi.psMode = deviceRecord.psi.psMode;
		}

		if (deviceRecord.psi.psRepMode != BTL_BMG_DDB_INVALID_PSI_REP_MODE)
		{
			device.psi.psRepMode= deviceRecord.psi.psRepMode;
		}

		/* 
			classOfDevice field of BtDeviceContext will be set in BtlBmgMonitorEventHandler that is called when 
			the record is added via the call to DS_AddDevice
		*/

		
		status = DS_AddDevice(&device, 0);
		BTL_VERIFY_FATAL(status == BT_STATUS_SUCCESS, status, ("DS_AddDevice Failed (%s)", pBT_Status(status)));
	}
		
	BTL_FUNC_END();

	return status;
}

