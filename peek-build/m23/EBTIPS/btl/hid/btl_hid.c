/*******************************************************************************\
*                                                                           										*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT			*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_hid.c
*
*   DESCRIPTION:    This file contains the BTL HID Host implementation.
*
*   AUTHOR:         Avraham HAMU
*
\*******************************************************************************/
#include "btl_config.h"
#include "btl_log.h"
#include "btl_defs.h"
#include "btl_hid.h"
BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_HID);

#if BTL_CONFIG_HIDH == BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include <btl_pool.h>
#include <btl_commoni.h>
#include <debug.h>
#include "hci.h"


/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BSL_HID_DEFAULT constant
 *
 *     Represents default security level for HID.
 */
#define BSL_HID_DEFAULT  (BSL_NO_SECURITY)


#define	TICKS_TO_MS(ticks)	BTHAL_OS_TICKS_TO_MS(ticks) 
/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _BtlHidChannel 	BtlHidChannel;
typedef struct _BtlHidData 		BtlHidData;


/*-------------------------------------------------------------------------------
 * BtlHidInitState type
 *
 *     Defines the HID init state.
 */
typedef U8 BtlHidInitState;

#define BTL_HID_INIT_STATE_NOT_INTIALIZED				(0x00)
#define BTL_HID_INIT_STATE_INITIALIZED					(0x01)
#define BTL_HID_INIT_STATE_INITIALIZATION_FAILED		(0x02)
#define BTL_HID_INIT_STATE_DEINITIALIZATION_FAILED	(0x03)


/*-------------------------------------------------------------------------------
 * BtlHidState type
 *
 *     Defines the HID state of a specific context.
 */
typedef U8 BtlHidState;

#define BTL_HID_STATE_DISABLED							(0x00)
#define BTL_HID_STATE_ENABLED								(0x01)
#define BTL_HID_STATE_DISABLING							(0x02)
/*-------------------------------------------------------------------------------
 * BtlHidChannelState type
 *
 *     Defines a HID channel state.
 */
typedef U8 BtlHidChannelState;

#define BTL_HID_CHANNEL_STATE_IDLE						(0x00)
#define BTL_HID_CHANNEL_STATE_DISCONNECTED				(0x01)
#define BTL_HID_CHANNEL_STATE_CONNECTING				(0x02)
#define BTL_HID_CHANNEL_STATE_CONNECT_IND				(0x03)
#define BTL_HID_CHANNEL_STATE_CONNECTED					(0x04)
#define BTL_HID_CHANNEL_STATE_DISCONNECTING				(0x05)


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlHidChannel structure
 *
 *     Represents BTL HID channel.
 */
struct _BtlHidChannel
{	
	/* Internal HID channel */
	HidChannel channel;

	/* Current HID state of the channel */
	BtlHidChannelState state;

	/* Connected remote device, Null if not connected  */
    	BtRemoteDevice *remDev;

	/* Associated context */
	BtlHidContext *context;

	/* Forward reference of EvmTimer */
	EvmTimer reconnectTimer;

	/* Last device connected */
	BD_ADDR          lastConnection;

	/* Define if a reconnection is needed in case of disconnection*/
	BOOL HostTryToReconnect;

	/* Define if the first connection is valid or not*/
	BOOL firstValidCon;

	/* Allows to know if the time read is valid*/
	BOOL automatReconnect;

       /* Represents the start of the timer after a disconnection */
 	BthalOsTime startTimer;

	/* Represents the number of trials of reconnection */
	U32 numTry;

	/* tokens for HCI commands */
	MeCommandToken		hciCmdToken[BTL_CONFIG_HID_MAX_HCI_COMMAND_PER_CHANNEL_ID];

	/* Establish an ACL connection after a SDP query*/
	BOOL connectAfterSDP; 

	/* Pool of Hid Interrupts */
	BTL_POOL_DECLARE_POOL(interruptsPool, interruptsMemory, BTL_CONFIG_HID_MAX_NUM_OF_TX_INTERRUPTS, sizeof(HidInterrupt));

	/* Pool of Hid Transactions */
	BTL_POOL_DECLARE_POOL(transactionsPool, transactionsMemory, BTL_CONFIG_HID_MAX_NUM_OF_TX_TRANSACTIONS, sizeof(HidTransaction));

	/* Pool of Hid Reports */
	BTL_POOL_DECLARE_POOL(reportsPool, reportsMemory, BTL_CONFIG_HID_MAX_NUM_OF_TX_REPORTS, sizeof(HidReport));

	/* Pool of Hid Reports Requests */
	BTL_POOL_DECLARE_POOL(reportReqPool, reportReqMemory, BTL_CONFIG_HID_MAX_NUM_OF_TX_REPORTREQ, sizeof(HidReportReq));
};

/*-------------------------------------------------------------------------------
 * BtlHidContext structure
 *
 *     Represents BTL HID context.
 */
struct _BtlHidContext 
{
	/* Must be first field */
	BtlContext base;

	/* Associated callback with this context */
	BtlHidCallBack callback;
	
	/* Current HID state of the context */
	BtlHidState state;

	/* Internal HID channels */
	BtlHidChannel channels[BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT];

	/*Remote Device*/
	BtRemoteDevice      *remDev;
	
#if BT_SECURITY == XA_ENABLED

	/* Registered security record for Control channel*/
	BtSecurityRecord securityRecordCtrl;

	/* Registered security record for Interrupt channel*/
	BtSecurityRecord securityRecordIntr;


#endif	/* BT_SECURITY == XA_ENABLED */
};


/*-------------------------------------------------------------------------------
 * BtlHidData structure
 *
 *     Represents the data of th BTL HID module.
 */
struct _BtlHidData
{
	/* Pool of HID contexts */
	BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_CONFIG_HID_MAX_NUM_OF_CONTEXTS, sizeof(BtlHidContext));

	/* List of active HID contexts */
	ListEntry contextsList;

	/* Event passed to the application */
	BtlHidEvent event;
};

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * btlHidInitState
 *
 *     Represents the current init state of HID module.
 */
static BtlHidInitState btlHidInitState = BTL_HID_INIT_STATE_NOT_INTIALIZED;


/*-------------------------------------------------------------------------------
 * btlHidData
 *
 *     Represents the data of HID module.
 */
static BtlHidData btlHidData;


/*-------------------------------------------------------------------------------
 * btlHidContextsPoolName
 *
 *     Represents the name of the HID contexts pool.
 */
static const char btlHidContextsPoolName[] = "HidContexts";


/*-------------------------------------------------------------------------------
 * btlHidInterruptsPoolName
 *
 *     Represents the name of the HID interrupts pool.
 */
static const char btlHidInterruptsPoolName[] = "HidInterrupts";


/*-------------------------------------------------------------------------------
 * btlHidTransactionsPoolName
 *
 *     Represents the name of the HID transactions pool.
 */
static const char btlHidTransactionsPoolName[] = "HidTransactions";


/*-------------------------------------------------------------------------------
 * btlHidReportsPoolName
 *
 *     Represents the name of the HID reports pool.
 */
static const char btlHidReportsPoolName[] = "HidReports";


/*-------------------------------------------------------------------------------
 * btlHidReportReqPoolName
 *
 *     Represents the name of the HID reports requests pool.
 */
static const char btlHidReportReqPoolName[] = "HidReportReq";


/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/


static void BtlHidCallback(HidChannel *Channel, HidCallbackParms *Parms);
static BtStatus BtlHidDeregisterAll(BtlHidContext *hidContext);
static BtStatus BtlHidDisable(BtlHidContext *hidContext);
static char * pResultCode(HidResultCode result);
static char *pHidProtocol(HidProtocol Protocol);
static BtStatus BtlHidhBtlNotificationsCb(BtlModuleNotificationType notificationType);
static void HidReconnectTimerExpired(EvmTimer *timer);
static void BtlHidAutReconnect(BtlHidContext *hidContext, BtlHidChannelId Channel);
static void BtlHIDSendHciCommandCB(const BtEvent *Event);
static BtStatus BtlHIDSendHciCommand(BtlHidChannel *channels, U16 hciCommand, U8 parmsLen, U8  *parms, U32 tokenIdx);
static void BtlHIDEnableBasebandQos(BtlHidChannel *channels, BOOL outGoing);

/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_HIDH_Init()
 */
 
BtStatus BTL_HIDH_Init(void)
{
	BtStatus status = BT_STATUS_SUCCESS;	
	BTL_FUNC_START_AND_LOCK_COMMON("BTL_HIDH_Init");
	
	BTL_VERIFY_ERR((BTL_HID_INIT_STATE_NOT_INTIALIZED == btlHidInitState), 
		BT_STATUS_FAILED, ("HID module is already initialized"));
	
	btlHidInitState = BTL_HID_INIT_STATE_INITIALIZATION_FAILED;
	/*Pool of Context*/
	status = BTL_POOL_Create(&btlHidData.contextsPool,
							btlHidContextsPoolName,
							btlHidData.contextsMemory, 
							BTL_CONFIG_HID_MAX_NUM_OF_CONTEXTS,
							sizeof(BtlHidContext));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("HID contexts pool creation failed"));
	
	InitializeListHead(&btlHidData.contextsList);
	
	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_HID, BtlHidhBtlNotificationsCb);
		
	btlHidInitState = BTL_HID_INIT_STATE_INITIALIZED;
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


 /*-------------------------------------------------------------------------------
 * BTL_HIDH_Deinit()
 */
 
BtStatus BTL_HIDH_Deinit(void)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_COMMON("BTL_HIDH_Deinit");

	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_HID);
	
	BTL_VERIFY_ERR((BTL_HID_INIT_STATE_INITIALIZED == btlHidInitState), 
		BT_STATUS_FAILED, ("HID module is not initialized"));
	
	btlHidInitState = BTL_HID_INIT_STATE_DEINITIALIZATION_FAILED;

	BTL_VERIFY_ERR((IsListEmpty(&btlHidData.contextsList)), 
		BT_STATUS_FAILED, ("HID contexts are still active"));

	/* Destroy the Context pool */
	status = BTL_POOL_Destroy(&btlHidData.contextsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("HID contexts pool destruction failed"));

	btlHidInitState = BTL_HID_INIT_STATE_NOT_INTIALIZED ;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_HIDH_Create()
 */
 
BtStatus BTL_HIDH_Create(BtlAppHandle *appHandle,
							const BtlHidCallBack hidCallback,
							const BtSecurityLevel *securityLevel,
							BtlHidContext **hidContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 idx;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_HIDH_Create");
	BTL_VERIFY_ERR((0 != hidCallback), BT_STATUS_INVALID_PARM, ("Null hidCallback"));
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));

#if BT_SECURITY == XA_ENABLED

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid HID securityLevel"));
	}

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Allocate memory for a unique new HID context */
	status = BTL_POOL_Allocate(&btlHidData.contextsPool, (void**)hidContext);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating HID context"));

	status = BTL_HandleModuleInstanceCreation(appHandle, BTL_MODULE_TYPE_HID, &(*hidContext)->base);
	if (BT_STATUS_SUCCESS != status)
	{
		/* hidContext must be freed before we exit */
		BtStatus retVal = BTL_POOL_Free(&btlHidData.contextsPool, (void**)hidContext);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == retVal), ("Failed freeing HID context"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed handling HID instance creation"));
	}

	/* Save the given callback */
	(*hidContext)->callback = hidCallback;

	/* Add the new HID context to the active contexts list */
	InsertTailList(&btlHidData.contextsList, &((*hidContext)->base.node));

	/* Creates the Pools  per channels */
	for(idx=0; idx<BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		/*Pool of Interrupts*/
		status = BTL_POOL_Create(&(*hidContext)->channels[idx].interruptsPool,
								btlHidInterruptsPoolName,
								(*hidContext)->channels[idx].interruptsMemory, 
								BTL_CONFIG_HID_MAX_NUM_OF_TX_INTERRUPTS,
								sizeof(HidInterrupt));
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
			("HID Interrupts pool creation failed"));

		/*Pool of Transactions*/
		status = BTL_POOL_Create(&(*hidContext)->channels[idx].transactionsPool,
								btlHidTransactionsPoolName,
								(*hidContext)->channels[idx].transactionsMemory, 
								BTL_CONFIG_HID_MAX_NUM_OF_TX_TRANSACTIONS,
								sizeof(HidTransaction));
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
			("HID Transactions pool creation failed"));

		/*Pool of Reports*/
		status = BTL_POOL_Create(&(*hidContext)->channels[idx].reportsPool,
								btlHidReportsPoolName,
								(*hidContext)->channels[idx].reportsMemory, 
								BTL_CONFIG_HID_MAX_NUM_OF_TX_REPORTS,
								sizeof(HidReport));
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
			("HID Reports pool creation failed"));

		/*Pool of Reports Requests*/
		status = BTL_POOL_Create(&(*hidContext)->channels[idx].reportReqPool,
								btlHidReportReqPoolName,
								(*hidContext)->channels[idx].reportReqMemory, 
								BTL_CONFIG_HID_MAX_NUM_OF_TX_REPORTREQ,
								sizeof(HidReportReq));
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
			("HID Reports Req pool creation failed"));

	} 

#if BT_SECURITY == XA_ENABLED

	/* Set the security record & save the given security level, or use default for the Control channel*/
	(*hidContext)->securityRecordCtrl.id = SEC_L2CAP_ID;
	(*hidContext)->securityRecordCtrl.channel = BT_PSM_HID_CTRL;
	(*hidContext)->securityRecordCtrl.level = (char)((securityLevel == 0) ? (BSL_HID_DEFAULT) : (*securityLevel));
	(*hidContext)->securityRecordCtrl.service = SDP_SERVICE_HID;

	/* Set the security record & save the given security level, or use default for the Interrupt channel*/
	(*hidContext)->securityRecordIntr.id = SEC_L2CAP_ID;
	(*hidContext)->securityRecordIntr.channel = BT_PSM_HID_INTR;
	(*hidContext)->securityRecordIntr.level = (char)((securityLevel == 0) ? (BSL_HID_DEFAULT) : (*securityLevel));
	(*hidContext)->securityRecordIntr.service = SDP_SERVICE_HID;
	

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Init HID context state */
	(*hidContext)->state = BTL_HID_STATE_DISABLED;

	/* Init HID channels state to idle */
	for (idx = 0; idx<BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		(*hidContext)->channels[idx].remDev = 0;
		(*hidContext)->channels[idx].state = BTL_HID_CHANNEL_STATE_IDLE;
		(*hidContext)->channels[idx].context = (*hidContext);
	}
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_HIDH_Destroy()
 */
 
BtStatus BTL_HIDH_Destroy(BtlHidContext **hidContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL isAllocated = FALSE;
	U32        idx=0;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_HIDH_Destroy");

	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((0 != *hidContext), BT_STATUS_INVALID_PARM, ("Null *hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_DISABLED == (*hidContext)->state), BT_STATUS_IN_USE, ("HID context is in use"));

	status = BTL_POOL_IsElelementAllocated(&btlHidData.contextsPool, *hidContext, &isAllocated);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed locating given HID context"));
	BTL_VERIFY_ERR((TRUE == isAllocated), BT_STATUS_INVALID_PARM, ("Invalid HID context"));

	/* Destroys the Pools  per channels */
	for(idx=0; idx<BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		status = BTL_POOL_Destroy(&(*hidContext)->channels[idx].interruptsPool);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
			("HID interrupts pool destruction failed"));
			
		/* Destroy the Transactions pool */
		status = BTL_POOL_Destroy(&(*hidContext)->channels[idx].transactionsPool);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
			("HID transactions pool destruction failed"));

		/* Destroy the Reports pool */
		status = BTL_POOL_Destroy(&(*hidContext)->channels[idx].reportsPool);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
			("HID reports pool destruction failed"));

		/* Destroy the Reports Requests pool */
		status = BTL_POOL_Destroy(&(*hidContext)->channels[idx].reportReqPool);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
			("HID reports requests pool destruction failed"));
	}

	/* Remove the context from the list of all HID contexts */
	RemoveEntryList(&((*hidContext)->base.node));

	status = BTL_HandleModuleInstanceDestruction(&(*hidContext)->base);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed handling HID instance destruction"));

	status = BTL_POOL_Free(&btlHidData.contextsPool, (void**)hidContext);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed freeing HID context"));

	/* Set the HID context to NULL */
	*hidContext = 0;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}



/*-------------------------------------------------------------------------------
 * BTL_HIDH_Enable()
 */
 
BtStatus BTL_HIDH_Enable(BtlHidContext *hidContext,
								const BtlUtf8 *serviceName,
								const BtlHidSupportedRole supportedRole)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 idx;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_HIDH_Enable");
	UNUSED_PARAMETER(serviceName);
	BTL_VERIFY_ERR((BTL_HID_DEVICE != supportedRole), BT_STATUS_INVALID_PARM, ("Role should be host or NULL"));
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_DISABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is already enabled"));

	for (idx = 0; idx<BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{		
		status = HID_Register(&(hidContext->channels[idx].channel), BtlHidCallback, HID_ROLE_HOST);
		if (status != BT_STATUS_SUCCESS)
		{
			/* Deregister all registered channels */
			status = BtlHidDeregisterAll(hidContext);

			/* This verify will fail and will handle the exception gracefully */
			BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed to register HID channel %d", idx));
		}

		hidContext->channels[idx].state = BTL_HID_CHANNEL_STATE_DISCONNECTED;
	}
	
		
#if BT_SECURITY == XA_ENABLED

	/* Register security record for both Control and Interrupt channel*/
	if ((BT_STATUS_SUCCESS != (status = SEC_Register(&hidContext->securityRecordCtrl))) || 
		(BT_STATUS_SUCCESS != (status = SEC_Register(&hidContext->securityRecordIntr))))
	{		
		/* Deregister all registered channels */
		status = BtlHidDeregisterAll(hidContext);

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed registering HID security record"));
	}
	
#endif	/* BT_SECURITY == XA_ENABLED */

	
	/* HID state is now enabled */
	hidContext->state = BTL_HID_STATE_ENABLED;

	BTL_LOG_INFO(("BTL_HIDH: Host is enabled "));

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status); 
}


/*-------------------------------------------------------------------------------
 * BTL_HIDH_Disable()
 */
 
BtStatus BTL_HIDH_Disable(BtlHidContext *hidContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 idx;
		
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_HIDH_Disable");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));

	/* First verify all channels are in valid state */
	for (idx = 0; idx<BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		switch (hidContext->channels[idx].state)
		{
			case (BTL_HID_CHANNEL_STATE_IDLE):
			case (BTL_HID_CHANNEL_STATE_CONNECTING):
			case (BTL_HID_CHANNEL_STATE_CONNECT_IND):
			{
				/* This verify will fail and will handle the exception gracefully */
				BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED != hidContext->state), BT_STATUS_FAILED, ("Disable failed, invalid HID state"));
				break;
			}
		}
	}

	for (idx = 0; idx<BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		switch (hidContext->channels[idx].state)
		{
			case (BTL_HID_CHANNEL_STATE_DISCONNECTING):
			{
				/* HID state is now in the process of disabling */
				hidContext->state = BTL_HID_STATE_DISABLING;
				break;
			}
			case (BTL_HID_CHANNEL_STATE_CONNECTED):
			{
				status = HID_CloseConnection(&(hidContext->channels[idx].channel));
				
				if (BT_STATUS_PENDING == status)
				{
					hidContext->channels[idx].state = BTL_HID_CHANNEL_STATE_DISCONNECTING;
					
					/* HID state is now in the process of disabling */
					hidContext->state = BTL_HID_STATE_DISABLING;
				}
				
				break;
			}
		}
	}

	if (BTL_HID_STATE_DISABLING != hidContext->state)
	{
		/* All channels are disconnected */
		status = BtlHidDisable(hidContext);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed disabling HID context"));
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_HIDH_Connect()
 */
 
BtStatus BTL_HIDH_Connect(BtlHidContext *hidContext,
								BD_ADDR *bdAddr)
{

	BtStatus status = BT_STATUS_PENDING;
	U32 idx;
	
	BTL_FUNC_START_AND_LOCK("BTL_HIDH_Connect");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR(( BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));

	for (idx = 0; idx<BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		if (hidContext->channels[idx].state == BTL_HID_CHANNEL_STATE_DISCONNECTED)
		{
			/* Found a free channel */
			break;
		}
	} 

	BTL_VERIFY_ERR((idx != BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_IN_USE, ("No free HID channel"));
	/* Init the status of the connection varaibles used for the re-connection:their status will be updated later. */
	hidContext->channels[idx].firstValidCon=FALSE;
	hidContext->channels[idx].HostTryToReconnect=FALSE;

	/* Update the flag to establish a connection after the SDP query */
	hidContext->channels[idx].connectAfterSDP=TRUE;
		
	status = HID_HostQueryDevice(&(hidContext->channels[idx].channel), bdAddr);
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("        Failed runnig SDP query ( No automatic reconnection )"));
		
   
	/* HID state is now in the process of connecting */
	
	hidContext->channels[idx].state = BTL_HID_CHANNEL_STATE_CONNECTING;
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_HIDH_GetConnectedDevice()
 */
BtStatus BTL_HIDH_GetConnectedDevice(BtlHidContext *hidContext, 
										BtlHidChannelId channelId, 
										BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL retVal=FALSE;
	
	BTL_FUNC_START_AND_LOCK("BTL_HIDH_GetConnectedDevice");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_HID_CHANNEL_STATE_IDLE != hidContext->channels[channelId].state), BT_STATUS_NO_CONNECTION, ("HID channel %d is not connected", channelId));	

	retVal = HID_IsConnected(&(hidContext->channels[channelId].channel));
	if (!retVal)
	{
		BTL_LOG_INFO(("There is no connection available"));
		status=BT_STATUS_NO_CONNECTION;
	}
	else
	{
		BTL_VERIFY_FATAL((0 != hidContext->channels[channelId].remDev), BT_STATUS_INTERNAL_ERROR, ("Unable to find connected remote device"));
		OS_MemCopy((U8 *)bdAddr, (const U8 *)(&(hidContext->channels[channelId].remDev->bdAddr)), sizeof(BD_ADDR));			
	}

	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_HIDH_Disconnect()
 */

BtStatus BTL_HIDH_Disconnect(BtlHidContext *hidContext,
									BtlHidChannelId channelId)
{
	BtStatus status = BT_STATUS_PENDING;
	
	BTL_FUNC_START_AND_LOCK("BTL_HIDH_Disconnect");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	
	status=HID_CloseConnection(&(hidContext->channels[channelId].channel));
	
	if (BT_STATUS_PENDING == status)
	{
		/* HID state is now in the process of disabling */
		hidContext->channels[channelId].state = BTL_HID_CHANNEL_STATE_DISCONNECTING;
	}

	/* HID state is now in the process of disconnecting */
	BTL_FUNC_END_AND_UNLOCK();
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_HIDH_HostQueryDevice()
 */

BtStatus BTL_HIDH_HostQueryDevice(BtlHidContext *hidContext,
										BtlHidChannelId channelId,
										BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BTL_FUNC_START_AND_LOCK("BTL_HID_HostQueryDevice");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_HID_CHANNEL_STATE_IDLE != hidContext->channels[channelId].state), BT_STATUS_FAILED, ("HID channel %d is not connected", channelId));


       status = HID_HostQueryDevice(&(hidContext->channels[channelId].channel), bdAddr);
           
       BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("HID Channel could not query device, reason: %s"));  
	BTL_FUNC_END_AND_UNLOCK();

       return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_HIDH_SendInterrupt()
 */

BtStatus BTL_HIDH_SendInterrupt(BtlHidContext *hidContext, 
										BtlHidChannelId channelId,
										U8 *data,
										U16 dataLen,
										HidReportType reportType,
										HidInterrupt **cookie)
{

	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	  
	BTL_FUNC_START_AND_LOCK("BTL_HIDH_SendInterrrupt");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_HID_CHANNEL_STATE_CONNECTED == hidContext->channels[channelId].state), BT_STATUS_FAILED, ("HID channel %d is not connected", channelId));
	
	/* Allocate memory for a unique new HID interrupt */
	status = BTL_POOL_Allocate(&hidContext->channels[channelId].interruptsPool, (void**)cookie);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating Interrupt"));
	
	/* Set up the HidInterrupt cookie */
	(*cookie)->data = data;
	(*cookie)->dataLen = dataLen;
	(*cookie)->reportType = reportType;
	
	status = HID_SendInterrupt(&(hidContext->channels[channelId].channel), (*cookie));

	if (status != BT_STATUS_PENDING)
	{
		/* Sending failed, free the HidInterrupt */
		retVal = BTL_POOL_Free(&hidContext->channels[channelId].interruptsPool, (void**)cookie);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing  Interrupt"));
	}	
	
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("HID Channel could not send Interrupt, reason: %s"));
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_HIDH_HardReset()
 */

BtStatus BTL_HIDH_HardReset(BtlHidContext *hidContext,
									BtlHidChannelId channelId,
									HidTransaction**cookie)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	HidControl instControl = HID_CTRL_HARD_RESET;
	
	BTL_FUNC_START_AND_LOCK("BTL_HIDH_HardReset");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_HID_CHANNEL_STATE_CONNECTED == hidContext->channels[channelId].state), BT_STATUS_FAILED, ("HID channel %d is not connected", channelId));


	/* Allocate memory for a unique new HID transaction */
	status = BTL_POOL_Allocate(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating Transaction - HardReset"));

	/* Set up the transaction */
	(*cookie)->parm.control  = instControl;
	
       status = HID_SendControl(&(hidContext->channels[channelId].channel), (*cookie));
	
	if (status != BT_STATUS_PENDING)
	{
		/* Sending failed, free the transaction */
		retVal = BTL_POOL_Free(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing Transaction - HardReset"));
	}	
	   
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("HID Channel could not send Hard Reset, reason: %s"));
	BTL_FUNC_END_AND_UNLOCK();
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_HIDH_SoftReset()
 */

BtStatus BTL_HIDH_SoftReset(BtlHidContext *hidContext,
									BtlHidChannelId channelId,
									HidTransaction**cookie)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	HidControl instControl = HID_CTRL_SOFT_RESET;
	
	
	BTL_FUNC_START_AND_LOCK("BTL_HIDH_SoftReset");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_HID_CHANNEL_STATE_CONNECTED == hidContext->channels[channelId].state), BT_STATUS_FAILED, ("HID channel %d is not connected", channelId));


	/* Allocate memory for a unique new transaction */
	status = BTL_POOL_Allocate(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating Transaction - SoftReset"));

	/* Set up the transaction */
	(*cookie)->parm.control  = instControl;
	
       status = HID_SendControl(&(hidContext->channels[channelId].channel), (*cookie));
	
	if (status != BT_STATUS_PENDING)
	{
		/* Sending failed, free the transaction */
		retVal = BTL_POOL_Free(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing Transaction - SoftReset"));
	}	
	
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("HID Channel could not send Soft Reset, reason: %s"));
            
	BTL_FUNC_END_AND_UNLOCK();
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_HIDH_Suspend()
 */

BtStatus BTL_HIDH_Suspend(BtlHidContext *hidContext,
									BtlHidChannelId channelId,
									HidTransaction**cookie)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	HidControl instControl = HID_CTRL_SUSPEND;
	
	BTL_FUNC_START_AND_LOCK("BTL_HIDH_Suspend");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_HID_CHANNEL_STATE_CONNECTED == hidContext->channels[channelId].state), BT_STATUS_FAILED, ("HID channel %d is not connected", channelId));


	/* Allocate memory for a unique new HID transaction */
	status = BTL_POOL_Allocate(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating Transaction - Suspend"));

	/* Set up the transaction */
	(*cookie)->parm.control  = instControl;
	
       status = HID_SendControl(&(hidContext->channels[channelId].channel), (*cookie));
	
	if (status != BT_STATUS_PENDING)
	{
		/* Sending failed, free the transaction */
		retVal = BTL_POOL_Free(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing Transaction - Suspend"));
	}	
	
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("HID Channel could not send Suspend, reason: %s"));
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_HIDH_ExitSuspend()
 */

BtStatus BTL_HIDH_ExitSuspend(BtlHidContext *hidContext,
									BtlHidChannelId channelId,
									HidTransaction**cookie)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	HidControl instControl = HID_CTRL_EXIT_SUSPEND;
	
	BTL_FUNC_START_AND_LOCK("BTL_HIDH_ExitSuspend");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_HID_CHANNEL_STATE_CONNECTED == hidContext->channels[channelId].state), BT_STATUS_FAILED, ("HID channel %d is not connected", channelId));


	/* Allocate memory for a unique new HID transaction */
	status = BTL_POOL_Allocate(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating Transaction - ExitSuspend"));

	/* Set up the transaction */
	(*cookie)->parm.control  = instControl;
	
       status = HID_SendControl(&(hidContext->channels[channelId].channel), (*cookie));
	
	if (status != BT_STATUS_PENDING)
	{
		/* Sending failed, free the transaction */
		retVal = BTL_POOL_Free(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing Transaction - ExitSuspend"));
	}	
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("HID Channel could not send Exit Suspend, reason: %s"));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_HIDH_Unplug()
 */

BtStatus BTL_HIDH_Unplug(BtlHidContext *hidContext,
									BtlHidChannelId channelId,
									HidTransaction**cookie)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	HidControl instControl = HID_CTRL_VIRTUAL_CABLE_UNPLUG ;
	
	BTL_FUNC_START_AND_LOCK("BTL_HIDH_Unplug");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_HID_CHANNEL_STATE_CONNECTED == hidContext->channels[channelId].state), BT_STATUS_FAILED, ("HID channel %d is not connected", channelId));


	/* Allocate memory for a unique new HID transaction */
	status = BTL_POOL_Allocate(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating Transaction - Unplug"));

	/* Set up the transaction */
	(*cookie)->parm.control  = instControl;
	
       status = HID_SendControl(&(hidContext->channels[channelId].channel), (*cookie));
	
	if (status != BT_STATUS_PENDING)
	{
		/* Sending failed, free the transaction */
		retVal = BTL_POOL_Free(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing Transaction - Unplug"));
	}	
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("HID Channel could not send Cable Unplug, reason: %s"));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_HIDH_SetIdleRate()
 */

BtStatus BTL_HIDH_SetIdleRate(BtlHidContext *hidContext,
										BtlHidChannelId channelId,
										HidIdleRate reportIdleRate,
										HidTransaction**cookie)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	
	BTL_FUNC_START_AND_LOCK("BTL_HIDH_SetIdleRate");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_HID_CHANNEL_STATE_CONNECTED == hidContext->channels[channelId].state), BT_STATUS_FAILED, ("HID channel %d is not connected", channelId));


	/* Allocate memory for a unique new HID transaction */
	status = BTL_POOL_Allocate(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating Transaction - SetIdleRate"));

	/* Set up the transaction */
	(*cookie)->parm.idleRate = reportIdleRate;
	
       status = HID_HostSetIdleRate(&(hidContext->channels[channelId].channel), (*cookie));
	
	if (status != BT_STATUS_PENDING)
	{
		/* Sending failed, free the transaction */
		retVal = BTL_POOL_Free(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing Transaction - SetIdleRate"));
	}	

 	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("HID Channel could not send Set Idle Rate, reason: %s"));
        
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_HIDH_GetIdleRate()
 */

BtStatus BTL_HIDH_GetIdleRate(BtlHidContext *hidContext,
									BtlHidChannelId channelId,
									HidTransaction**cookie)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	
	BTL_FUNC_START_AND_LOCK("BTL_HIDH_GetIdleRate");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_HID_CHANNEL_STATE_CONNECTED == hidContext->channels[channelId].state), BT_STATUS_FAILED, ("HID channel %d is not connected", channelId));


	/* Allocate memory for a unique new HID transaction */
	status = BTL_POOL_Allocate(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating Transaction - GetIdleRate"));

       status = HID_HostGetIdleRate(&(hidContext->channels[channelId].channel), (*cookie));
	
	if (status != BT_STATUS_PENDING)
	{
		/* Sending failed, free the transaction */
		retVal = BTL_POOL_Free(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing Transaction - GetIdleRate"));
	}	

	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("HID Channel could not send Get Idle Rate, reason: %s"));
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}



/*-------------------------------------------------------------------------------
 * BTL_HIDH_SetReport()
 */

BtStatus BTL_HIDH_SetReport(BtlHidContext *hidContext,
										BtlHidChannelId channelId,
										U8 *data,
										U16 dataLen,
										HidReportType reportType,
										HidTransaction**cookie)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	HidReport *pReport;
	
	BTL_FUNC_START_AND_LOCK("BTL_HID_SetReport");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_HID_CHANNEL_STATE_CONNECTED == hidContext->channels[channelId].state), BT_STATUS_FAILED, ("HID channel %d is not connected", channelId));


	status = BTL_POOL_Allocate(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating Transaction - SetReport"));

	status = BTL_POOL_Allocate(&hidContext->channels[channelId].reportsPool, (void**)&pReport);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating report - SetReport"));
		
	/* Set up the transaction */
	(*cookie)->parm.report= pReport;
	(*cookie)->parm.report->data = data;
	(*cookie)->parm.report->dataLen = dataLen;
	(*cookie)->parm.report->reportType = reportType;

       status = HID_HostSetReport(&(hidContext->channels[channelId].channel), (*cookie));
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("HID Channel could not send Set Report, reason: %s"));

	if (status != BT_STATUS_PENDING)
	{
		/* Sending failed, free the transaction and the report */
		retVal = BTL_POOL_Free(&hidContext->channels[channelId].reportsPool, (void**)&pReport);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing Reports"));

		retVal = BTL_POOL_Free(&hidContext->channels[channelId].transactionsPool,(void**) cookie);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing Transaction - SetReport"));
	}	
	
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("HID Channel could not send Transactions, reason: %s"));
		   
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_HIDH_GetReport()
 */

BtStatus BTL_HIDH_GetReport(BtlHidContext *hidContext,
										BtlHidChannelId channelId,
										U16 bufferSize,
										HidReportType reportType,
										BOOL useId,
										U8 reportId,
										HidTransaction**cookie)
{
	BtStatus status = BT_STATUS_SUCCESS;
	HidReportReq *pReportReq;
	BtStatus retVal;
	
	BTL_FUNC_START_AND_LOCK("BTL_HID_GetReport");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_HID_CHANNEL_STATE_CONNECTED == hidContext->channels[channelId].state), BT_STATUS_FAILED, ("HID channel %d is not connected", channelId));

	status = BTL_POOL_Allocate(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating Transaction - GetReport"));

	status = BTL_POOL_Allocate(&hidContext->channels[channelId].reportReqPool, (void**)&pReportReq);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating reports - GetReport"));

	/* Set up the  report request*/
	(*cookie)->parm.reportReq= pReportReq;
	(*cookie)->parm.reportReq->bufferSize= bufferSize;
	(*cookie)->parm.reportReq->reportType = reportType;
	(*cookie)->parm.reportReq->useId = useId;
	(*cookie)->parm.reportReq->reportId = reportId;
	

       status = HID_HostGetReport(&(hidContext->channels[channelId].channel), (*cookie));
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("HID Channel could not send Set Report, reason: %s"));

	if (status != BT_STATUS_PENDING)
	{
		/* Sending failed, free the transaction and the report request */
		retVal = BTL_POOL_Free(&hidContext->channels[channelId].reportReqPool, (void**)&pReportReq);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing Report request"));

		retVal = BTL_POOL_Free(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing Transaction - GetReport"));
	}	
	   
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("HID Channel could not send Get Report, reason: %s"));
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_HIDH_SetProtocol()
 */

BtStatus BTL_HIDH_SetProtocol(BtlHidContext *hidContext,
										BtlHidChannelId channelId,
										HidProtocol reportProtocol,
										HidTransaction**cookie)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	
	BTL_FUNC_START_AND_LOCK("BTL_HIDH_SetProtocol");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_HID_CHANNEL_STATE_CONNECTED == hidContext->channels[channelId].state), BT_STATUS_FAILED, ("HID channel %d is not connected", channelId));


	/* Allocate memory for a unique new HID transaction */
	status = BTL_POOL_Allocate(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating Transaction - SetProtocol"));

	/* Set up the transaction*/
	(*cookie)->parm.protocol = reportProtocol;
	
       status = HID_HostSetProtocol(&(hidContext->channels[channelId].channel), (*cookie));
	
	if (status != BT_STATUS_PENDING)
	{
		/* Sending failed, free the transaction */
		retVal = BTL_POOL_Free(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing Transaction - SetProtocol"));
	}	
	
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("HID Channel could not send Set Protocol, reason: %s"));
		   
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_HIDH_GetProtocol()
 */

BtStatus BTL_HIDH_GetProtocol(BtlHidContext *hidContext,
									BtlHidChannelId channelId,
									HidTransaction**cookie)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	
	BTL_FUNC_START_AND_LOCK("BTL_HIDH_GetProtocol");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((BTL_HID_STATE_ENABLED == hidContext->state), BT_STATUS_FAILED, ("HID context is not enabled"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_HID_CHANNEL_STATE_CONNECTED == hidContext->channels[channelId].state), BT_STATUS_FAILED, ("HID channel %d is not connected", channelId));


	/* Allocate memory for a unique new HID transaction */
	status = BTL_POOL_Allocate(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating Transaction - GetProtocol"));

       status = HID_HostGetProtocol(&(hidContext->channels[channelId].channel), (*cookie));
	
	if (status != BT_STATUS_PENDING)
	{
		/* Sending failed, free the transaction */
		retVal = BTL_POOL_Free(&hidContext->channels[channelId].transactionsPool, (void**)cookie);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing Transaction - GetProtocol"));
	}	
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("HID Channel could not send Get Protocol, reason: %s"));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_HID_SetSecurityLevel()
 */
 
BtStatus BTL_HID_SetSecurityLevel(BtlHidContext *hidContext,
								  		const BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_HID_SetSecurityLevel");
	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid HID securityLevel"));
	}
	
	/* Apply the new security level */
	hidContext->securityRecordCtrl.level = (BtSecurityLevel)((securityLevel == 0) ? (BSL_HID_DEFAULT) : (*securityLevel));
	hidContext->securityRecordIntr.level = (BtSecurityLevel)((securityLevel == 0) ? (BSL_HID_DEFAULT) : (*securityLevel));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_HID_GetSecurityLevel()
 */
 
BtStatus BTL_HID_GetSecurityLevel(BtlHidContext *hidContext,
								  		BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_HID_GetSecurityLevel");

	BTL_VERIFY_ERR((0 != hidContext), BT_STATUS_INVALID_PARM, ("Null hidContext"));
	BTL_VERIFY_ERR((0 != securityLevel), BT_STATUS_INVALID_PARM, ("Null securityLevel"));

	*securityLevel = (BtSecurityLevel)(hidContext->securityRecordCtrl.level | hidContext->securityRecordIntr.level);
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

#endif	/* BT_SECURITY == XA_ENABLED */




/*-------------------------------------------------------------------------------
 * BtlHidCallback()
 *
 *		Internal callback for handling HID events in BTL layer.
 *		This function forwards events to registered application callback.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		chnl [in] - Internal HID channel.
 *
 *		Parms [in] - Internal HID event.
 *
 * Returns:
 *		void.
 */
 
static void BtlHidCallback(HidChannel *Channel, HidCallbackParms *Parms)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL passEventToApp = TRUE;
	BOOL sendDisableEvent = FALSE;
	BtlHidContext *hidContext;
	BtlHidChannel *btlChannel;
	char addr[BDADDR_NTOA_SIZE];
	HidInterrupt *hidInterrupt;
	HidTransaction *pTransaction;
	HidReport * pReport;
	HidReportReq * pReportReq;
	U32 idx;
	static U16 mtuSizeCtrl;
	


	BTL_FUNC_START("BtlHidCallback");

	/* Find BTL HID channel according to given HID channel */
	btlChannel = ContainingRecord(Channel, BtlHidChannel, channel);

	/* Find context according to BTL HID channel */
	hidContext = btlChannel->context;

	/* Find channel index in the context */
	for (idx = 0; idx<BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		if ((&(hidContext->channels[idx])) == btlChannel)
			break;
	}

	BTL_VERIFY_ERR((idx != BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_FAILED, ("No channel found for received event"));


	/* First handle special case of disabling */
	if (hidContext->state == BTL_HID_STATE_DISABLING)
	{
		switch (Parms->event)
		{
			case (HIDEVENT_CLOSED):
			{				
				U32 i;
								
				BTL_LOG_INFO(("BTL_HID: HID channel %d is disconnected from %s.", idx, bdaddr_ntoa(&(Parms->ptrs.remDev->bdAddr), addr)));

				btlChannel->state = BTL_HID_CHANNEL_STATE_DISCONNECTED;

				/* Check that all channels are disconnected */
				for (i=0; i<BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT; i++)
				{
					if (hidContext->channels[i].state != BTL_HID_CHANNEL_STATE_DISCONNECTED)
						break;
				}
				
				if (i == BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT)
					BtlHidDisable(hidContext);
					sendDisableEvent = TRUE;

				/* Pass event to app. */
				break;
			}
			default:
			{
				BTL_LOG_ERROR(("BTL_HID: Received unexpected event %d while disabling!", Parms->event));	
				
				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;
			}
		}
	}

	else
	{
		switch (Parms->event)
		{
			case (HIDEVENT_OPEN): 
			{
				/* Transport lower layer connection (L2CAP) has been established */
				btlChannel->state = BTL_HID_CHANNEL_STATE_CONNECTED;
				btlChannel->remDev = Parms->ptrs.remDev;
				BTL_LOG_INFO(("BTL_HID: Connection opened with BD_Addr %s.", bdaddr_ntoa(&(Parms->ptrs.remDev->bdAddr),addr)));
				BTL_LOG_INFO(("Connection opened"));

				/* Re-init these variables in case of diconnection */
				hidContext->channels[idx].numTry=BTL_CONFIG_HID_MAX_NUMBER_OF_RECONNECTION;
				hidContext->channels[idx].automatReconnect=FALSE;

				/* save the BD_address of the last device connected*/
				hidContext->channels[idx].lastConnection = Parms->ptrs.remDev->bdAddr;

				/*MTU size after the negociation between the Host and the Device*/
				mtuSizeCtrl=L2CAP_GetTxMtu(Channel->ctrlCid);

				if (BTL_CONFIG_HID_QOS_OUTGOING)
				{
					/* Enable the QoS for the current outgoing connection*/
					BtlHIDEnableBasebandQos(&(hidContext->channels[idx]), TRUE);
				}

				if(BTL_CONFIG_HID_QOS_INCOMING)
				{
					/* Enable the QoS for the current incoming connection*/
					BtlHIDEnableBasebandQos(&(hidContext->channels[idx]), FALSE);
				} 
				
				break; 
			}
		
			case (HIDEVENT_OPEN_IND):
			{
				/* Transport lower layer connection (L2CAP) request has been received */
				btlChannel->state = BTL_HID_CHANNEL_STATE_CONNECTING;
				BTL_LOG_INFO(("BTL_HID: Connection requested from BD_Addr %s.", bdaddr_ntoa(&(Parms->ptrs.remDev->bdAddr),addr)));
				 if (HID_AcceptConnection(Channel) != BT_STATUS_PENDING) {
			            BTL_LOG_ERROR(("Could Not accept the incoming connection"));
				     Parms->status= BT_STATUS_FAILED;
			        } else {
			            BTL_LOG_INFO(("Open request received"));
			        }
				break;
			}
		
		
			case HIDEVENT_CLOSE_IND: 
			        BTL_LOG_INFO(("Close indication received"));
			        break;
					
			case (HIDEVENT_CLOSED): 
			{
				/* Transport lower layer connection (L2CAP) has been disconnected */
				btlChannel->state = BTL_HID_CHANNEL_STATE_DISCONNECTED;
				btlChannel->remDev = 0;
				BTL_LOG_INFO(("BTL_HID: Hid channel is disconnected from %s.", bdaddr_ntoa(&(Parms->ptrs.remDev->bdAddr), addr)));
				BTL_LOG_INFO(("Connection closed"));

				/* start timer */
				status = BTHAL_OS_GetSystemTime(&(hidContext->channels[idx].startTimer));
				if (status != BT_STATUS_SUCCESS) 
				{
					BTL_LOG_ERROR(("Can't start the timer, status: %s", pBT_Status(status)));
					hidContext->channels[idx].automatReconnect =FALSE;
				}
				else
					hidContext->channels[idx].automatReconnect =TRUE;
				break;
			}
		
			case HIDEVENT_QUERY_CNF: 
				/* If the Host can't SDP the device don't go up to the application the old SDP data*/
				 if(BT_STATUS_SUCCESS!=Parms->status)
				 {
				 	Parms->ptrs.queryRsp->queryFlags = 0;
				 }
				 
				 BTL_LOG_INFO(("Received SDP Query Response"));	
				 if (Parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_RECONNECT_INIT)
				 {
				 	if (Parms->ptrs.queryRsp->reconnect) 
					{
				  	  BTL_LOG_INFO(("    Does Not Initiates Reconnect"));
			            	  hidContext->channels[idx].HostTryToReconnect=FALSE;
				 	}
					else
					{
					   BTL_LOG_INFO(("    Initiates Reconnect - Automatic Reconnection"));
					   hidContext->channels[idx].HostTryToReconnect=TRUE;  

					   /* This field is optionnal in SDP record */
					   if (Parms->ptrs.queryRsp->queryFlags & SDPQ_FLAG_NORM_CONNECTABLE) 
					   {
					      if (Parms->ptrs.queryRsp->normConnectable) 
					      {
				                  BTL_LOG_INFO(("    Normally Connectable - Automatic Reconnection"));
						    hidContext->channels[idx].HostTryToReconnect=TRUE;
				             } 
					      else 
					      {
				                  BTL_LOG_INFO(("    Not Normally Connectable - No Automatic Reconnection"));
						    hidContext->channels[idx].HostTryToReconnect=FALSE;	
				              }
					    }
			              } 
				 }
			
				if(TRUE == hidContext->channels[idx].connectAfterSDP)
				{
					BTL_LOG_INFO(("There is no connection available"));


					 /* After the SDP query we etablish a connection */
					status = HID_OpenConnection(&(hidContext->channels[idx].channel), &(Parms->ptrs.remDev->bdAddr));
					if (BT_STATUS_PENDING != status) 
					{
						BTL_LOG_ERROR(("Failed connecting HID channel %d", idx));
						/* Transport lower layer connection (L2CAP) has been disconnected */
						btlChannel->state = BTL_HID_CHANNEL_STATE_DISCONNECTED;
						btlChannel->remDev = 0;

						/* The event that will arrive to the application will HIDEVENT_CLOSED*/
						Parms->event = HIDEVENT_CLOSED;
					}
					else
					{
						/* Update the status of valid connection disabling by the way the automatic reconnection if the first connection failed*/
						hidContext->channels[idx].firstValidCon=TRUE;
					}
					 /* Re-init the flag which allow to establish a connection after a SDP query */
					 hidContext->channels[idx].connectAfterSDP=FALSE;
				 }
			        break;

			case HIDEVENT_INTERRUPT: 
			        BTL_LOG_INFO(("Interrupt Received:"));
				 BTL_LOG_INFO(("Data:%s",Parms->ptrs.intr->data));	
			        break;

			case HIDEVENT_INTERRUPT_COMPLETE:
			        BTL_LOG_INFO(("Interrupt complete"));
				 BTL_LOG_INFO(("Data:%s",Parms->ptrs.intr->data));		
				 /* Save locally the pointer, since it is set to null when calling BTL_POOL_Free */
				 hidInterrupt= Parms->ptrs.intr;
				 status = BTL_POOL_Free(&hidContext->channels[idx].interruptsPool, (void**)&hidInterrupt);
				 BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed freeing interrupt"));			

			        break;
			
			case HIDEVENT_TRANSACTION_IND: 
			        BTL_LOG_INFO(("Transaction Received:"));
			        switch (Parms->ptrs.trans->type) {
			        case HID_TRANS_CONTROL:
		     	            BTL_LOG_INFO(("    Control:"));
			            if (Parms->ptrs.trans->parm.control == HID_CTRL_VIRTUAL_CABLE_UNPLUG) {
		      	                BTL_LOG_INFO(("        Control Function Received: VIRTUAL CABLE UNPLUG"));
					   status=HID_CloseConnection(&(hidContext->channels[idx].channel));
					   if (BT_STATUS_PENDING == status)
					   {
						/* HID state is now in the process of disabling */
						hidContext->channels[idx].state = BTL_HID_CHANNEL_STATE_DISCONNECTING;

						/* Update the status to not perform an automatic  reconnection*/
						hidContext->channels[idx].HostTryToReconnect=FALSE;
					   }
			            } else {
		      	                BTL_LOG_ERROR(("        Unknown Control Function"));
			            }
			        }
			        break;
  		       case HIDEVENT_TRANSACTION_RSP:
  		        	BTL_LOG_INFO(("Transaction Response"));
  		        	switch (Parms->ptrs.trans->type) {
  		        		case HID_TRANS_GET_REPORT_RSP:
  		             		       BTL_LOG_INFO(("Get Report Response, Report data len = %d", Parms->ptrs.trans->parm.report->dataLen));
						break;
		  		        case HID_TRANS_GET_PROTOCOL_RSP:
			  	              BTL_LOG_INFO(("Get Protocol Response, Protocol = %s",pHidProtocol(Parms->ptrs.trans->parm.protocol)));
						break;
  		        		case HID_TRANS_GET_IDLE_RATE_RSP:
  		            			BTL_LOG_INFO(("Get Idle Rate Response, Idle Rate = %d",Parms->ptrs.trans->parm.idleRate));
						break;
		  		        }
  		        		break;
		       case HIDEVENT_TRANSACTION_COMPLETE:
			        BTL_LOG_INFO(("Transaction complete, status = %s",pBT_Status(Parms->status)));
					/* We generate an event to the application first, since we will free the memory
					 * later in the HIDEVENT_TRANSACTION_COMPLETE event handling 
					 */
					
					/* Set the context in the event passed to app */
					btlHidData.event.hidContext = hidContext;

					/* Set the channel index in the event passed to app */
					btlHidData.event.channelId = idx;
					
					/* Set the internal event in the event passed to app */
					btlHidData.event.hidEvent = Parms;

					/* Pass the event to app */
					hidContext->callback(&btlHidData.event);

					/* Avoid another event generation */ 
					passEventToApp = FALSE;
					
				switch (Parms->ptrs.trans->type) {
			        	case HID_TRANS_GET_REPORT:
			            		BTL_LOG_INFO(("Get Report, result = %s",pResultCode(Parms->ptrs.trans->resultCode)));
						pReportReq= Parms->ptrs.trans->parm.reportReq;
						status = BTL_POOL_Free(&hidContext->channels[idx].reportReqPool, (void**)&pReportReq);
						BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed freeing Report request"));														
		            			break;
		        		case HID_TRANS_GET_PROTOCOL:
		            			BTL_LOG_INFO(("Get Protocol, result = %s", pResultCode(Parms->ptrs.trans->resultCode)));
						break;
		        		case HID_TRANS_GET_IDLE_RATE:
		           			BTL_LOG_INFO(("Get Idle Rate, result = %s", pResultCode(Parms->ptrs.trans->resultCode)));
						break;
		        		case HID_TRANS_SET_REPORT:
		            			BTL_LOG_INFO(("    Set Report, result = %s", pResultCode(Parms->ptrs.trans->resultCode)));
						pReport= Parms->ptrs.trans->parm.report;
						status = BTL_POOL_Free(&hidContext->channels[idx].reportsPool, (void**)&pReport);
						BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed freeing Report"));																	
		            			break;
		        		case HID_TRANS_SET_PROTOCOL:
		                	       BTL_LOG_INFO(("Set Report, result = %s", pResultCode(Parms->ptrs.trans->resultCode)));           
						break;
		        		case HID_TRANS_SET_IDLE_RATE:
		            			BTL_LOG_INFO(("    Set Idle Rate"));    
						break;
		        		}
				
				 pTransaction= Parms->ptrs.trans;
				 status = BTL_POOL_Free(&hidContext->channels[idx].transactionsPool, (void**)&pTransaction);
				 BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed freeing  Transaction"));																				   								
			                           
			       break;
		}
	}
	if (TRUE == passEventToApp)
	{
		/* Set the context in the event passed to app */
		btlHidData.event.hidContext = hidContext;

		/* Set the channel index in the event passed to app */
		btlHidData.event.channelId = idx;
		
		/* Set the internal event in the event passed to app */
		btlHidData.event.hidEvent = Parms;
	
		/* Pass the event to app */
		hidContext->callback(&btlHidData.event);
	}

	/* To perform a reconnection we need:
	 *	1. a valid value of the timer (automatReconnect flag)
	 *    2. that the current connection isn't the first (firstValidCon flag)
	 *    3. that the disconnection isn't the result of the UNPLUG transaction (HostTryToReconnect flag)
	 */
	if((hidContext->channels[idx].HostTryToReconnect) && (hidContext->channels[idx].firstValidCon==TRUE) && (hidContext->channels[idx].automatReconnect))
	{
		BTL_LOG_INFO((" Timer started for channel :%d ", idx));
   
		BtlHidAutReconnect(hidContext, idx);
	}
 
	if (TRUE == sendDisableEvent)
	{
		Parms->event = HIDEVENT_DISABLED;
		
		/* Pass the event to app */
		hidContext->callback(&btlHidData.event);
	}

	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * BtlHidDisable()
 */
static BtStatus BtlHidDisable(BtlHidContext *hidContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlHidDisable");

	/* First, try to deregister all channels */
	status = BtlHidDeregisterAll(hidContext);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_FAILED, ("Failed deregistering all HID channels"));

#if BT_SECURITY == XA_ENABLED

	/* Second, try to unregister security record on control channel*/
	status = SEC_Unregister(&hidContext->securityRecordCtrl);
	BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("Failed unregistering HID security record on control channel"));

	/* Second, try to unregister security record on control channel*/
	status = SEC_Unregister(&hidContext->securityRecordIntr);
	BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("Failed unregistering HID security record on interrupt channel"));
			
#endif	/* BT_SECURITY == XA_ENABLED */
	
	hidContext->state = BTL_HID_STATE_DISABLED;

	BTL_LOG_INFO(("BTL_HID: Host is disabled."));

	BTL_FUNC_END();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BtlHidDeregisterAll()
 *
 *		Internal function for deregistering all HID channels.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		hidContext [in] - pointer to the HID context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
static BtStatus BtlHidDeregisterAll(BtlHidContext *hidContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 idx;

	BTL_FUNC_START("BtlHidDeregisterAll");

	/* Try to deregister all channels */
	for (idx = 0; idx<BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		status = HID_Deregister(&(hidContext->channels[idx].channel));

		if (status == BT_STATUS_SUCCESS)
		{
			hidContext->channels[idx].state = BTL_HID_CHANNEL_STATE_IDLE;	
		}
		else
		{
			BTL_LOG_ERROR(("Failed deregistering HID channel %d", idx));
			status = BT_STATUS_FAILED;
		}
	}
	
	BTL_FUNC_END();

	return (status);
}


/*-------------------------------------------------------------------------------
 * pResultCode()
 */
static char * pResultCode(HidResultCode result)
{
    switch (result) {
    case HID_RESULT_SUCCESS:
        return "HID_RESULT_SUCCESS";
    case HID_RESULT_NOT_READY:
        return "HID_RESULT_NOT_READY";
    case HID_RESULT_INVALID_REPORT_ID:
        return "HID_RESULT_INVALID_REPORT_ID";
    case HID_RESULT_UNSUPPORTED_REQUEST:
        return "HID_RESULT_UNSUPPORTED_REQUEST";
    case HID_RESULT_INVALID_PARAMETER:
        return "HID_RESULT_INVALID_PARAMETER";
    case HID_RESULT_UNKNOWN:
        return "HID_RESULT_UNKNOWN";
    case HID_RESULT_FATAL:
        return "HID_RESULT_FATAL";
    default:
        return "UNKNOWN";
    }
}


/*-------------------------------------------------------------------------------
 * pHidProtocol()
 */
static char *pHidProtocol(HidProtocol Protocol)
{
    switch (Protocol) {
    case HID_PROTOCOL_REPORT:
        return "HID_PROTOCOL_REPORT";
    case HID_PROTOCOL_BOOT:
        return "HID_PROTOCOL_BOOT";
    default:
        return "UNKNOWN";
    }
}

BtStatus BtlHidhBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlHidhBtlNotificationsCb");
	
	BTL_LOG_DEBUG(("Notification Type: %d", notificationType));
	
	switch (notificationType)
	{
		case BTL_MODULE_NOTIFICATION_RADIO_ON:

			/* Do something if necessary */
		
			break;

		case BTL_MODULE_NOTIFICATION_RADIO_OFF:

			/* Disconnect all connections , stop all connections in establishment, and any other process in progress 
				in all contexts
			*/
			
			break;

		default:

			BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid notification (%d)", notificationType));
			


	};

	BTL_FUNC_END();
	
	return status;
}


/*-------------------------------------------------------------------------------
 * BtlHidAutReconnect()
 *
 *		Internal function used to load the timer
 *
 */
static void BtlHidAutReconnect(BtlHidContext *hidContext, BtlHidChannelId Channel)
{
	
      
	TimeT interval = MS_TO_TICKS (BTL_CONFIG_HID_MAX_INTERVAL_RECONNECTION_IN_MS);
	BTL_FUNC_START("BtlHidAutReconnect");
	
	hidContext->channels[Channel].reconnectTimer.func = HidReconnectTimerExpired;
       hidContext->channels[Channel].reconnectTimer.context = (void *)hidContext;

    	if(hidContext->channels[Channel].numTry!=0)
	{
		EVM_StartTimer(&(hidContext->channels[Channel].reconnectTimer), interval);
		hidContext->channels[Channel].numTry--;
	}
	else
		BTL_LOG_INFO((" Number of reconnection is over"));
							
	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * HidReconnectTimerExpired()
 *
 *		Called when the timer is over
 */
static void HidReconnectTimerExpired(EvmTimer *timer)
{
    BtStatus status = BT_STATUS_SUCCESS;	
    BthalOsTime currentTime=0;
    U32 idx; 

   
    BtlHidContext *hidContext = (BtlHidContext *)timer->context;
    BTL_FUNC_START("HidReconnectTimerExpired");
	
    for (idx = 0; idx<BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		if (hidContext->channels[idx].state == BTL_HID_CHANNEL_STATE_DISCONNECTED)
		{
			/* Found a free channel */
			break;
		}
	}
		
    /* Get the current time */
    status = BTHAL_OS_GetSystemTime(&currentTime);
    if (status != BT_STATUS_SUCCESS) 
    {
    	BTL_LOG_ERROR(("Can't read the current time, status: %s", pBT_Status(status)));
    }
    else 
    {			
	   if(currentTime- (hidContext->channels[idx].startTimer) <=MS_TO_TICKS(BTL_CONFIG_HID_MAX_TIMEOUT_RECONNECTION_IN_MS))
	   { 
		BTL_VERIFY_ERR((idx != BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_IN_USE, ("No free HID channel"));

		/* Update the flag to establish a connection after the SDP query */
		hidContext->channels[idx].connectAfterSDP=TRUE;
		
		status = HID_HostQueryDevice(&(hidContext->channels[idx].channel), (BD_ADDR *) &(hidContext->channels[idx].lastConnection));
	
		if (status == BT_STATUS_PENDING) 
		{
			BTL_LOG_INFO(("Creating connection..."));
			/* HID state is now in the process of connecting */
			hidContext->channels[idx].state = BTL_HID_CHANNEL_STATE_CONNECTING;

			/* Update the status of valid connection disabling by the way the automatic reconnection if the first connection failed*/
			hidContext->channels[idx].firstValidCon=TRUE;
		   	EVM_CancelTimer(timer);
		}
		else 
		{
			BTL_LOG_ERROR(("Failed connecting HID channel %d", idx));
			/* Try to reconnect */
			BtlHidAutReconnect(hidContext, idx);
		}	
	    }
	    else 
	    {
		BTL_LOG_INFO((" Reconnection time out is over"));
	    }
    }
    BTL_FUNC_END();
}


static void BtlHIDSendHciCommandCB(const BtEvent *Event)
{
	BTL_FUNC_START("BtlHIDSendHciCommandCB");
	
	if (Event->p.meToken->p.general.out.status == BT_STATUS_SUCCESS)
	{
		BTL_LOG_DEBUG(("HCI command 0x%x completed", 
			Event->p.meToken->p.general.in.hciCommand));	
	}

	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * BtlHIDSendHciCommand()
 *
 *		Send a command to HCI
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		channels [in] - active channel of the open connection
 *
 *		hciCommand [in] - the HCI comamnd
 *
 *           parmsLen [in] - length in bytes of the parameter buffer
 *
 *           parms [in] - the parameter buffer
 *
 *		tokenIdx [in] - ID of the token used to execute the HCI command
 *
 * Returns:
 *		BT_STATUS_PENDING - the command was successfully sent,
 *           elase various failures
 */
static BtStatus BtlHIDSendHciCommand(BtlHidChannel *channels,
										U16	hciCommand,
										U8 	parmsLen, 
										U8 	*parms,
										U32 tokenIdx){
	BtStatus 				status = BT_STATUS_SUCCESS;
	MeCommandToken		*token;
	U8					event;

	BTL_FUNC_START("BtlHIDSendHciCommand");

	switch ( hciCommand ) 
	{
		case HCC_WRITE_LINK_POLICY:
			event = HCE_COMMAND_COMPLETE;
			break;
		case HCC_FLOW_SPECIFICATION:
			event = HCE_FLOW_SPECIFICATION_COMPLETE;
			break;
		default:
			BTL_ERR(BT_STATUS_INVALID_PARM, ("invalid HCI command 0x%x", hciCommand));
	}
	
	token = &(channels->hciCmdToken[tokenIdx]); 
		
	OS_MemSet((U8*) token, 0, sizeof(MeCommandToken));
	
	/* Set HCI Command parameters */
	token->callback = BtlHIDSendHciCommandCB;
	token->p.general.in.hciCommand = hciCommand;
	token->p.general.in.parmLen = parmsLen;
	token->p.general.in.parms = parms;
	token->p.general.in.event = event;

	/* Send the HCI command */
	status = ME_SendHciCommandSync(token);	

	BTL_VERIFY_ERR((BT_STATUS_PENDING == status),
			status, ("ME_SendHciCommandSync Failed, Status = %s", pBT_Status(status)));

	BTL_FUNC_END();
	
	return status;

}
 

/*-------------------------------------------------------------------------------
 * BtlHIDEnableBasebandQos()
 *
 *		Activate Baseband QOS for the active channel by calling the HCI "Flow Specification Command"
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		channels [in] - Active channel
 *
 *		outGoing [in] - specify the direction of the FLOW_SPECIFICATION
 *			TRUE - outgoing 
 *			FALSE - incoming
 *
 * Returns:
 *		none
 */			
static void BtlHIDEnableBasebandQos(BtlHidChannel *channels, BOOL outGoing)
{
	/* following parameters are according to Version 2.0 + EDR BT spec */	
	static U8 flowSpecificationParms[21] =
	{
		0x00, 0x00,				/* Connection_Handle  	- fill it below */
		0x00,					/* Flags 				- Reserved */
		0x00,					/* Flow_direction		- Outgoing Flow i.e. traffic send over the ACL connection */
		BQST_GUARANTEED, 		/* Service_Type 		- 0x02 Guaranteed ! */
		0x10, 0x00, 0x00, 0x00, 	/*  Token Rate 		- set to 0x00000010 */
		0x10, 0x00, 0x00, 0x00, 	/*  Peak_Bandwidth 	- set to 0x00000010 */
		0x10, 0x00, 0x00, 0x00, 	/*  Token Bucket Size 	- set to 0x00000010 */
		0x00, 0x00, 0x00, 0x00, 	/*  Access Latency 	- fill it below */
	};

	BtRemoteDevice	*remDev;
	U16				connectionHandle;
	U32 				tokenIdx;
	BtStatus			status;

	BTL_FUNC_START("BtlHIDEnableBasebandQos");

	remDev  = channels->remDev; 

	if (remDev )
	{
		connectionHandle = ME_GetHciConnectionHandle(remDev);

 		StoreLE16(&flowSpecificationParms[0], connectionHandle);

		/* Access Latency is in microseconds - set to 10,000 . 20,000 is the default */
		StoreLE32(&flowSpecificationParms[17], (BTL_CONFIG_HID_MAX_LATENCY_IN_MS)); 
		

		if(FALSE == outGoing)
		{
		/* Specify an incoming connection */
			flowSpecificationParms[3]=1; 
			tokenIdx=1;
		}
		else
		{
		/* Specify an outgoing connection */
			flowSpecificationParms[3]= 0; 
			tokenIdx=0; 
		}

		status = 	BtlHIDSendHciCommand(channels, HCC_FLOW_SPECIFICATION, 
				sizeof(flowSpecificationParms), (U8*) flowSpecificationParms, tokenIdx );
	}
	BTL_FUNC_END();
}





#else /*BTL_CONFIG_HIDH == BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_OPPC_Init() - When BTL_CONFIG_OPP Disabled
 */
BtStatus BTL_HIDH_Init(void)
{
    
   BTL_LOG_INFO(("BTL_HIDH_Init() -  BTL_CONFIG_HIDH Disabled"));
  
    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_OPPS_Deinit() - When BTL_CONFIG_OPP Disabled
 */
BtStatus BTL_HIDH_Deinit(void)
{
    BTL_LOG_INFO(("BTL_HIDH_Deinit() -  BTL_CONFIG_HIDH Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /*BTL_CONFIG_HIDH == BTL_CONFIG_ENABLED*/

