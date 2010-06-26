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
*   FILE NAME:      btl_bsc.c
*
*   DESCRIPTION:    Implementation of BTL Bluetooth System Coordinator compenent.
*
*					This component implements inter-modules coordination
*                   functionality which is needed, currently, for some BT
*                   complex scenarios.
*
*   AUTHOR:         V. Abram
*
\*******************************************************************************/


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bttypes.h"
#include "osapi.h"
#include "debug.h"
#include "btl_defs.h"
#include "btl_log_modules.h"
#include "btl_pool.h"
#include "btl_bsc.h"
#include "bthal_types.h"
#include "bthal_common.h"
#include "bthal_os.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BSC);

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

/* Size of a pool for receiving events from different modules */
#define BTL_BSC_EVENTS_POOL_SIZE                        (20)

/* Definition of the BSC state machine's execution context */
#define BTL_BSC_EXECUTION_CONTEXT                       (BTHAL_OS_TASK_HANDLE_STACK)

/* Max size of a name of a watched module */
#define BTL_BSC_WATCHED_MODULE_NAME_MAX_SIZE            (16)

/* Max number of multiple connections to the same profile */
#define BTL_BSC_MAX_NUM_OF_SAME_CONNECTED_DEVICES       (2)

/* Unknown device */
#define BTL_BSC_WATCHED_DEVICE_NOT_FOUND                (0xff)

/********************************************************************************
 *
 * Internal Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlBscState type
 *
 *     Defines the states of the BTL BSC module.
 */
typedef U8 BtlBscState;

#define BTL_BSC_STATE_NOT_INTIALIZED                    (0x00)
#define BTL_BSC_STATE_INITIALIZED                       (0x01)

/*-------------------------------------------------------------------------------
 * BtlBscWatchedModuleState type
 *
 *     Defines the specific for BTL BSC states of a module which is watched and
 *     events from which are arrived to the BTL BSC module.
 */
typedef U8 BtlBscWatchedModuleState;

#define BTL_BSC_WATCHED_MODULE_STATE_IDLE	            (0x00)
#define BTL_BSC_WATCHED_MODULE_STATE_CONNECTED	        (0x01)
#define BTL_BSC_WATCHED_MODULE_STATE_ACTIVE 	        (0x02)


/********************************************************************************
 *
 * Internal Data Structures
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * BtlBscWatchedDeviceInfo structure
 *
 *	   Represents information about watched module related to connected device.
 */
typedef struct _BtlBscWatchedDeviceInfo
{
    /* A state of a watched module */
	BtlBscWatchedModuleState    state;

    /* Information about connected device */
    BtlBscWatchedInfo           info;
    
} BtlBscWatchedDeviceInfo;
 
/*-------------------------------------------------------------------------------
 * BtlBscWatchedModuleInfo structure
 *
 *	   Represents information about watched module.
 */
typedef struct _BtlBscWatchedModuleInfo
{
	/* Name of the module */
    char                        name[BTL_BSC_WATCHED_MODULE_NAME_MAX_SIZE];
    
    /* Information about connected device */
    BtlBscWatchedDeviceInfo     deviceInfo[BTL_BSC_MAX_NUM_OF_SAME_CONNECTED_DEVICES];
    
} BtlBscWatchedModuleInfo;

/*-------------------------------------------------------------------------------
 * BtlBscData structure
 *
 *	   Includes internal data required for BTL BSC functioning.
 */
typedef struct _BtlBscData
{
    /* Pool for BSC events to be sent from different modules to the BSC */
	BTL_POOL_DECLARE_POOL(eventsPool,
                          eventsMemory,
                          BTL_BSC_EVENTS_POOL_SIZE,
                          sizeof(BtlBscEvent));
    
    /* List of events received by the BSC */
    ListEntry                               eventsList;

    /* Semaphore for accessing events pool and list */
    BthalOsSemaphoreHandle                  eventsSemaphore;

    /* Voice handover configuration */
    BtlBscVoiceHandoverConfig               voiceHandoverConfig;

    /* Configuration of pausing Media Player on arriving of incoming call */
    BtlBscPauseMediaPlayerOnIncCallConfig   pauseMediaPlayerOnIncCallConfig;

    /* Flag reflecting that recommended pause event was sent */
    BOOL                                    recommendedPauseSent;

} BtlBscData;


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * btlBscEventsPoolName
 *
 *     Represents the name of the BSC servers contexts pool.
 */
static const char btlBscEventsPoolName[] = "BscEvents";

/*-------------------------------------------------------------------------------
 * btlBscState
 *
 *     Represents the current state of the BTL BSC module.
 */
static BtlBscState btlBscState = BTL_BSC_STATE_NOT_INTIALIZED;

/*-------------------------------------------------------------------------------
 * btlBscData
 *
 *     Represents the internal data of the BTL BSC module.
 */
static BtlBscData btlBscData;

/*-------------------------------------------------------------------------------
 * btlBscWatchedModulesInfo
 *
 *     Represents information about modules which are watched.
 */
static BtlBscWatchedModuleInfo  btlBscWatchedModulesInfo[BTL_BSC_MAX_NUM_OF_WATCHED_MODULES] =
{
    {"BTL_A2DP", {BTL_BSC_WATCHED_MODULE_STATE_IDLE}},
    {"BTL_AVRCPTG", {BTL_BSC_WATCHED_MODULE_STATE_IDLE}},
/* Only one of two gateway modules may be enabled */
#if BTL_CONFIG_AG == BTL_CONFIG_ENABLED
    {"BTL_AG", {BTL_BSC_WATCHED_MODULE_STATE_IDLE}}
#endif
#if BTL_CONFIG_VG == BTL_CONFIG_ENABLED
    {"BTL_VG", {BTL_BSC_WATCHED_MODULE_STATE_IDLE}}
#endif
};


/********************************************************************************
 *
 * Function prototypes
 *
 *******************************************************************************/

/* This function should be called from the main loop of the context in which
 * the BSC state machine is processed */
void BTL_BSC_ProcessEvents(void);


/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/

static BOOL BtlBscCheckConditionsForPausePlayer(BD_ADDR *agBdAddr, U8 *idx);
static U8 BtlBscFindWatchedConnectedDevice(BtlBscWatchedModuleInfo *watchedModuleInfo,
                                           BtlBscEvent *bscEvent);
static const char *pBTL_BSC_Event(BtlBscEventType event);


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * BTL_BSC_Init()
 *
 *		Initializes BTL BSC module and allocates required resources.
 */
BtStatus BTL_BSC_Init()
{
	BtStatus status = BT_STATUS_SUCCESS;
    BthalStatus bthalStatus;
    BtlBscWatchedModuleInfo *watchedModuleInfo = &btlBscWatchedModulesInfo[0];
    U8 idx1, idx2;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_BSC_Init");

	BTL_VERIFY_ERR((BTL_BSC_STATE_NOT_INTIALIZED == btlBscState),
				   BT_STATUS_FAILED,
				   ("BTL_BSC: BTL_BSC_Init() was already called"));
	
	/* Create buffers pool for events to be sent from different modules */
	status = BTL_POOL_Create(&btlBscData.eventsPool,
							 btlBscEventsPoolName,
							 btlBscData.eventsMemory, 
							 BTL_BSC_EVENTS_POOL_SIZE,
							 sizeof(BtlBscEvent));

	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
					 BT_STATUS_INTERNAL_ERROR,
					 ("BTL_BSC: BSC events pool creation failed"));
	
	InitializeListHead(&btlBscData.eventsList);

	/* Create semaphore for accessing events pool and list */
    bthalStatus = BTHAL_OS_CreateSemaphore("BTL_BSC_sem", &btlBscData.eventsSemaphore); 
    
    BTL_VERIFY_FATAL((BTHAL_STATUS_SUCCESS == bthalStatus),
				     BT_STATUS_FAILED,
				     ("BTL_BSC: BTHAL_OS_CreateSemaphore() status: %s",
                     BTHAL_StatusName(bthalStatus)));

    /* Update state of the BSC module */
    btlBscState = BTL_BSC_STATE_INITIALIZED;

    /* Reset flag which reflects whether recommended pause event was sent */
    btlBscData.recommendedPauseSent = FALSE;

    /* Update states of all watched modules for all peer devices to be watched */
    for (idx1=0; idx1<BTL_BSC_MAX_NUM_OF_WATCHED_MODULES; idx1++,watchedModuleInfo++)
    {
        for (idx2=0; idx2<BTL_BSC_MAX_NUM_OF_SAME_CONNECTED_DEVICES; idx2++)
        {
            watchedModuleInfo->deviceInfo[idx2].state = BTL_BSC_WATCHED_MODULE_STATE_IDLE;
        }
    }
		
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BSC_Deinit()
 *
 *		Deinitializes BTL BSC module and frees allocated resources.
 */
BtStatus BTL_BSC_Deinit()
{
	BtStatus status = BT_STATUS_SUCCESS;
    BthalStatus bthalStatus;
    BtlBscWatchedModuleInfo *watchedModuleInfo = &btlBscWatchedModulesInfo[0];
    U8 idx1, idx2;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_BSC_Deinit");

	BTL_VERIFY_ERR((BTL_BSC_STATE_INITIALIZED == btlBscState),
				   BT_STATUS_FAILED,
				   ("BTL_BSC: BTL_BSC_Deinit() was already called"));
	
    /* Destroy semaphore which was used for accessing events pool and list */
    bthalStatus = BTHAL_OS_DestroySemaphore(btlBscData.eventsSemaphore);

    BTL_VERIFY_FATAL((BTHAL_STATUS_SUCCESS == bthalStatus),
				     BT_STATUS_FAILED,
				     ("BTL_BSC: BTHAL_OS_DestroySemaphore() status: %s",
                     BTHAL_StatusName(bthalStatus)));

	/* Destroy pool which was used for BSC events */
	status = BTL_POOL_Destroy(&btlBscData.eventsPool);

	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
					 BT_STATUS_INTERNAL_ERROR,
					 ("BTL_BSC: BSC events pool destruction failed"));

    /* Update states of all watched modules for all peer devices to be watched */
    for (idx1=0; idx1<BTL_BSC_MAX_NUM_OF_WATCHED_MODULES; idx1++,watchedModuleInfo++)
    {
        for (idx2=0; idx2<BTL_BSC_MAX_NUM_OF_SAME_CONNECTED_DEVICES; idx2++)
        {
            watchedModuleInfo->deviceInfo[idx2].state = BTL_BSC_WATCHED_MODULE_STATE_IDLE;
        }
    }

    /* Update state of the BSC module */
    btlBscState = BTL_BSC_STATE_NOT_INTIALIZED;
		
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BSC_SetVoiceHandoverConfig()
 *
 *     	Sets configuration of the voice handover.
 */
void BTL_BSC_SetVoiceHandoverConfig(const BtlBscVoiceHandoverConfig *config)
{
    BTL_FUNC_START("BTL_BSC_SetVoiceHandoverConfig");

    /* Copy voice handover configuration */
    btlBscData.voiceHandoverConfig = *config;

	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BTL_BSC_GetVoiceHandoverConfig()
 *
 *     	Gets configuration of the voice handover.
 */
void BTL_BSC_GetVoiceHandoverConfig(BtlBscVoiceHandoverConfig *config)
{
    BTL_FUNC_START("BTL_BSC_GetVoiceHandoverConfig");

    *config = btlBscData.voiceHandoverConfig;

	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BTL_BSC_SetPauseMediaPlayerOnIncCallConfig()
 *
 *     	Sets configuration of pausing Media Player on arriving of incoming call.
 */
void BTL_BSC_SetPauseMediaPlayerOnIncCallConfig(const BtlBscPauseMediaPlayerOnIncCallConfig *config)
{
    BTL_FUNC_START("BTL_BSC_SetPauseMediaPlayerOnIncCallConfig");

    /* Copy configuration of pausing Media Player on arriving of incoming call */
    btlBscData.pauseMediaPlayerOnIncCallConfig = *config;

	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BTL_BSC_GetPauseMediaPlayerOnIncCallConfig()
 *
 *     	Gets configuration of pausing Media Player on arriving of incoming call.
 */
void BTL_BSC_GetPauseMediaPlayerOnIncCallConfig(BtlBscPauseMediaPlayerOnIncCallConfig *config)
{
    BTL_FUNC_START("BTL_BSC_GetPauseMediaPlayerOnIncCallConfig");

    *config = btlBscData.pauseMediaPlayerOnIncCallConfig;

	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BTL_BSC_SendEvent()
 *
 *     	Sends BTL_BSC event to the Bluetooth System Coordinator module.
 */
void BTL_BSC_SendEvent(const BtlBscEvent *event)
{
    BtlBscEvent *csEvent;
    BtStatus status;
    BthalStatus bthalStatus;

	BTL_FUNC_START("BTL_BSC_SendEvent");
	
    /* Get semaphore for accessing events pool and list */
    bthalStatus = BTHAL_OS_LockSemaphore(btlBscData.eventsSemaphore, 0);

    BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus),
                             ("BTL_BSC: BTHAL_OS_LockSemaphore() status: %s",
                             BTHAL_StatusName(bthalStatus)));

    /* Allocate memory for the new event */
    status = BTL_POOL_Allocate(&btlBscData.eventsPool,
                               (void **)&csEvent);

    BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS == status),
	                         ("BTL_BSC: Failed allocating BSC event"));

	/* Copy event's information */
    *csEvent = *event;
    
    /* Pass event to the BSC state machine */
    InsertTailList(&btlBscData.eventsList, &csEvent->node);

    /* Notify execution context about the event */
	bthalStatus = BTHAL_OS_SendEvent(BTL_BSC_EXECUTION_CONTEXT,
                                     OS_EVENT_STACK_TASK_BSC);

    BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == status),
		                     ("BTL_BSC: BthalNotifyContext() status: %s",
				             BTHAL_StatusName(bthalStatus)));

    /* Release semaphore for accessing events pool and list */
    bthalStatus = BTHAL_OS_UnlockSemaphore(btlBscData.eventsSemaphore);

    BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus),
                             ("BTL_BSC: BTHAL_OS_UnlockSemaphore() status: %s",
                             BTHAL_StatusName(bthalStatus)));

	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BTL_BSC_ProcessEvents()
 *
 *     	Processes events received from different modules.
 */
void BTL_BSC_ProcessEvents()
{
    BtStatus status;
    BthalStatus bthalStatus;
    BtlBscEvent *bscEvent;
    BtlBscWatchedModuleInfo *watchedModuleInfo;
	char addr[BDADDR_NTOA_SIZE];
    U8 idx;

   	BTL_FUNC_START("BTL_BSC_ProcessEvents");

    /* Get semaphore for accessing events pool and list */
    bthalStatus = BTHAL_OS_LockSemaphore(btlBscData.eventsSemaphore, 0);
    
    BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus),
                             ("BTL_BSC: BTHAL_OS_LockSemaphore() status: %s",
                             BTHAL_StatusName(bthalStatus)));

    while (!IsListEmpty(&btlBscData.eventsList))
    {
        /* Get received event */
        bscEvent = (BtlBscEvent *)RemoveHeadList(&btlBscData.eventsList);

        watchedModuleInfo = &btlBscWatchedModulesInfo[bscEvent->source];

        BTL_LOG_INFO(("BTL_BSC: Event %s is received from module %s",
                      pBTL_BSC_Event(bscEvent->type),
                      watchedModuleInfo->name));
        
        /* Process received event according to its type */
        switch(bscEvent->type)
        {
            case BTL_BSC_EVENT_SLC_CONNECTED:
                /* Check whether this device is not already in the database of
                 * this type of watched modules */
                idx = BtlBscFindWatchedConnectedDevice(watchedModuleInfo,
                                                       bscEvent);
                if (BTL_BSC_WATCHED_DEVICE_NOT_FOUND == idx)
                {
                    /* Find empty room for the new device */
                    for (idx=0; idx<BTL_BSC_MAX_NUM_OF_SAME_CONNECTED_DEVICES; idx++)
                    {
                        if (FALSE == watchedModuleInfo->deviceInfo[idx].info.isConnected)
                        {
                            /* Empty room was found */
                            break;
                        }
                    }

		            /* Check whether amount of connection is not more than allowed */
                    if (BTL_BSC_MAX_NUM_OF_SAME_CONNECTED_DEVICES == idx)
                    {
                        BTL_LOG_INFO(("BTL_BSC: Only %d connections may be watched to %s module",
                                       BTL_BSC_MAX_NUM_OF_SAME_CONNECTED_DEVICES, 
                                       watchedModuleInfo->name));
                        break;
                    }
                }
                
                /* Copy BD ADDR and possible additional information about the
                 * connected device and update state of the watched module */
                watchedModuleInfo->deviceInfo[idx].state = BTL_BSC_WATCHED_MODULE_STATE_CONNECTED;
                watchedModuleInfo->deviceInfo[idx].info = bscEvent->info;
                watchedModuleInfo->deviceInfo[idx].info.isConnected = TRUE;
                
                BTL_LOG_INFO(("BTL_BSC: Module %s is connected to %s",
		                       watchedModuleInfo->name,
		                       bdaddr_ntoa((const BD_ADDR *)&watchedModuleInfo->deviceInfo[idx].info.bdAddr,
                                           addr)));
                break;

            case BTL_BSC_EVENT_SLC_DISCONNECTED:
                /* Find the peer device which was connected to this watched module */
                idx = BtlBscFindWatchedConnectedDevice(watchedModuleInfo,
                                                       bscEvent);

		        /* Check whether the connected device was found */
                if (BTL_BSC_WATCHED_DEVICE_NOT_FOUND == idx)
                {
                    BTL_LOG_DEBUG(("BTL_BSC: Event is recevied from module which is not watched or was not connected"));
                }
                else
                {
                    watchedModuleInfo->deviceInfo[idx].state = BTL_BSC_WATCHED_MODULE_STATE_IDLE;
                    watchedModuleInfo->deviceInfo[idx].info.isConnected = FALSE;
                    
                    BTL_LOG_INFO(("BTL_BSC: Module %s is disconnected from %s",
			                       watchedModuleInfo->name,
			                       bdaddr_ntoa((const BD_ADDR *)&watchedModuleInfo->deviceInfo[idx].info.bdAddr,
                                               addr)));
                }
                break;
            
            case BTL_BSC_EVENT_AUDIO_STREAMING_STARTED:
                /* Find the peer device which was connected to this watched module */
                idx = BtlBscFindWatchedConnectedDevice(watchedModuleInfo,
                                                       bscEvent);

		        /* Check whether the connected device was found */
                if (BTL_BSC_WATCHED_DEVICE_NOT_FOUND == idx)
                {
                    BTL_LOG_DEBUG(("BTL_BSC: Event is recevied from module which is not watched or was not connected"));
                }
                else
                {
                    /* Update the state of the watched module */
                    watchedModuleInfo->deviceInfo[idx].state = BTL_BSC_WATCHED_MODULE_STATE_ACTIVE;

                    BTL_LOG_INFO(("BTL_BSC: Module %s is in ACTIVE state with %s",
			                       watchedModuleInfo->name,
			                       bdaddr_ntoa((const BD_ADDR *)&watchedModuleInfo->deviceInfo[idx].info.bdAddr,
                                               addr)));
                }
                break;

            case BTL_BSC_EVENT_AUDIO_STREAMING_STOPPED:
                /* Find the peer device which was connected to this watched module */
                idx = BtlBscFindWatchedConnectedDevice(watchedModuleInfo,
                                                       bscEvent);

		        /* Check whether the connected device was found */
                if (BTL_BSC_WATCHED_DEVICE_NOT_FOUND == idx)
                {
                    BTL_LOG_DEBUG(("BTL_BSC: Event is recevied from module which is not watched or was not connected"));
                }
                /* Pairs of events BTL_BSC_EVENT_AUDIO_STREAMING_STOPPED and
                 * BTL_BSC_EVENT_SLC_DISCONNECTED may arrive upon different state transitions of A2DP:
                 * - from 'Streaming' to 'Suspended'
                 * - from 'Streaming' to 'Aborted'
                 * - from 'Streaming' to 'Idle'
                 * - from 'Streaming' to 'Closed'.
                 * Thus, we need to update our state only once. */
                else if (BTL_BSC_WATCHED_MODULE_STATE_ACTIVE == watchedModuleInfo->deviceInfo[idx].state)
                {
                    /* Update the state of the watched module */
                    watchedModuleInfo->deviceInfo[idx].state = BTL_BSC_WATCHED_MODULE_STATE_CONNECTED;

                    BTL_LOG_INFO(("BTL_BSC: Module's %s state related to device %s is INACTIVE",
			                       watchedModuleInfo->name,
			                       bdaddr_ntoa((const BD_ADDR *)&watchedModuleInfo->deviceInfo[idx].info.bdAddr,
                                               addr)));
                }
                break;

            case BTL_BSC_EVENT_INCOMING_CALL:

#if BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED

                /* Find the peer device which was connected to this watched module */
                idx = BtlBscFindWatchedConnectedDevice(watchedModuleInfo,
                                                       bscEvent);

		        /* Check whether the connected device was found */
                if (BTL_BSC_WATCHED_DEVICE_NOT_FOUND == idx)
                {
                    BTL_LOG_DEBUG(("BTL_BSC: Event is recevied from module which is not watched or was not connected"));
                }
                /* Check conditions for sending 'pause' event to Media Player
                 * and get index of AVRCPTG device which context will be used */
                else if (TRUE == BtlBscCheckConditionsForPausePlayer(&bscEvent->info.bdAddr, &idx))
                {
                    /* Send 'pause' recommended event to Media Player */
                    status = BTL_AVRCPTG_BSC_SendRecommendedEvent(btlBscWatchedModulesInfo[BTL_BSC_EVENT_SOURCE_AVRCPTG].deviceInfo[idx].info.context,
                                                                  AVRCP_BSC_EVENT_RECOMMENDED_PAUSE);

                    BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS == status),
					                         ("BTL_BSC: BTL_AVRCPTG_BSC_SendRecommendedEvent() failed, status %s",
                                              pBT_Status(status)));

                    /* Set the flag which reflects whether recommended pause event was sent */
                    btlBscData.recommendedPauseSent = TRUE;
                }
#endif /*BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED*/
                break;

            case BTL_BSC_EVENT_CALL_FINISHED:
#if BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED 
                /* Check whether recommended pause event was sent */
                if (TRUE == btlBscData.recommendedPauseSent)
                {
                    /* Find the peer device which was connected to this watched module */
                    idx = BtlBscFindWatchedConnectedDevice(watchedModuleInfo,
                                                           bscEvent);

		            /* Check whether the connected device was found */
                    if (BTL_BSC_WATCHED_DEVICE_NOT_FOUND == idx)
                    {
                        BTL_LOG_DEBUG(("BTL_BSC: Event is recevied from module which is not watched or was not connected"));
                    }
                    else
                    {
                        /* Reset the flag which reflects whether recommended pause event was sent */
                        btlBscData.recommendedPauseSent = FALSE;

                        /* Send 'resume play' recommended event to Media Player */
                        status = BTL_AVRCPTG_BSC_SendRecommendedEvent(btlBscWatchedModulesInfo[BTL_BSC_EVENT_SOURCE_AVRCPTG].deviceInfo[idx].info.context,
                                                                      AVRCP_BSC_EVENT_RECOMMENDED_RESUME_PLAYING);

                        BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS == status),
					                             ("BTL_BSC: BTL_AVRCPTG_BSC_SendRecommendedEvent() failed, status %s",
                                                  pBT_Status(status)));
                    }
                }
#endif /*BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED*/
                break;

            default:
                BTL_LOG_DEBUG(("Unknown event type %d", bscEvent->type));
                break;
        }
    
        /* Put the buffer back on the available list */
	    status = BTL_POOL_Free(&btlBscData.eventsPool, (void **)&bscEvent);

	    BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS == status),
				                 ("BTL_BSC: Failed freeing BSC event"));
    }

    /* Release semaphore for accessing events pool and list */
    bthalStatus = BTHAL_OS_UnlockSemaphore(btlBscData.eventsSemaphore);
    
    BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus),
                             ("BTL_BSC: BTHAL_OS_UnlockSemaphore() status: %s",
                             BTHAL_StatusName(bthalStatus)));

	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BtlBscCheckConditionsForPausePlayer()
 *
 *     	Checks conditions for sending 'pause' event to Media Player and return
 *      index of found AVRCPTG device in the watched modules database.
 */

#if BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED

static BOOL BtlBscCheckConditionsForPausePlayer(BD_ADDR *agBdAddr, U8 *idx)
{
    U8 idxA2dp, idxAvrcptg;
    BtlBscWatchedModuleInfo *a2dpInfo, *avrcptgInfo;

    /* Check 'pausePlayerOnIncCall' configuration */
    if (FALSE == btlBscData.pauseMediaPlayerOnIncCallConfig.pausePlayerOnIncCall)
    {
        return FALSE;
    }
    
    /* In order to pause the streaming, module BTL_A2DP in streaming state and
     * module BTL_AVRCPTG in connected state should be connected to the same
     * remote device */
    for (idxA2dp = 0, a2dpInfo = &btlBscWatchedModulesInfo[BTL_BSC_EVENT_SOURCE_A2DP];
         idxA2dp < BTL_BSC_MAX_NUM_OF_SAME_CONNECTED_DEVICES;
         idxA2dp++)
    {
        if (BTL_BSC_WATCHED_MODULE_STATE_ACTIVE == a2dpInfo->deviceInfo[idxA2dp].state)
        {
            for (idxAvrcptg = 0, avrcptgInfo = &btlBscWatchedModulesInfo[BTL_BSC_EVENT_SOURCE_AVRCPTG];
                 idxAvrcptg < BTL_BSC_MAX_NUM_OF_SAME_CONNECTED_DEVICES;
                 idxAvrcptg++)
            {
                if (BTL_BSC_WATCHED_MODULE_STATE_CONNECTED == avrcptgInfo->deviceInfo[idxAvrcptg].state)
                {
                    if (TRUE == AreBdAddrsEqual((const BD_ADDR *)&a2dpInfo->deviceInfo[idxA2dp].info.bdAddr,
                                                (const BD_ADDR *)&avrcptgInfo->deviceInfo[idxAvrcptg].info.bdAddr))
                    {
                        /* Fill output parameter */
                        *idx = idxAvrcptg;
                        
                        /* Check 'pausePlayerOnIncCallToAnotherHeadset' configuration. If enabled,
                         * AVRCP_BSC_EVENT_RECOMMENDED_PAUSE event should be sent even if Hands-free
                         * SLC is not established to the stereo headset to which music is streaming */
                        if (TRUE == btlBscData.pauseMediaPlayerOnIncCallConfig.pausePlayerOnIncCallToAnotherHeadset)
                        {
                            return (TRUE);
                        }

                        /* Modules BTL_A2DP and BTL_AG should be connected to the same remote device.
                         * This means that music is streaming to the stereo headset to which
                         * Hands-free SLC should be also established */
                        if (TRUE == AreBdAddrsEqual((const BD_ADDR *)&a2dpInfo->deviceInfo[idxA2dp].info.bdAddr,
                                                    (const BD_ADDR *)agBdAddr))
                        {
                            return (TRUE);
                        }
                    }
                }
            }

        }
    }

    return (FALSE);
}

#endif  /*BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BtlBscFindWatchedConnectedDevice()
 *
 *     	Finds connected device's index in the database of the watched module.
 */
static U8 BtlBscFindWatchedConnectedDevice(BtlBscWatchedModuleInfo *watchedModuleInfo,
                                           BtlBscEvent *bscEvent)
{
    U8 idx;

    /* Find the module which was connected to this peer device */
    for (idx=0; idx<BTL_BSC_MAX_NUM_OF_SAME_CONNECTED_DEVICES; idx++)
    {
        if (TRUE == watchedModuleInfo->deviceInfo[idx].info.isConnected)
        {
            /* According to the BD ADDR */
            if (TRUE == bscEvent->isBdAddrValid)
            {
                if (TRUE == AreBdAddrsEqual((const BD_ADDR *)&watchedModuleInfo->deviceInfo[idx].info.bdAddr,
                                            (const BD_ADDR *)&bscEvent->info.bdAddr))
                {
                    return (idx);
                }
            }
            /* According to the channel */
            else if (bscEvent->info.channel == watchedModuleInfo->deviceInfo[idx].info.channel)
            {
                    return (idx);
            }
        }
    }

    BTL_LOG_DEBUG(("BTL_BSC: Watched device was not found"));

    return (BTL_BSC_WATCHED_DEVICE_NOT_FOUND);
}

/*-------------------------------------------------------------------------------
 * pBTL_BSC_Event()
 *
 *     	Returns name of the BTL BSC event.
 */
const char *pBTL_BSC_Event(BtlBscEventType event)
{
    switch (event)
    {
        case BTL_BSC_EVENT_SLC_CONNECTED:
            return "BTL_BSC_EVENT_SLC_CONNECTED";

        case BTL_BSC_EVENT_SLC_DISCONNECTED:
            return "BTL_BSC_EVENT_SLC_DISCONNECTED";

        case BTL_BSC_EVENT_AUDIO_STREAMING_STARTED:
            return "BTL_BSC_EVENT_AUDIO_STREAMING_STARTED";

        case BTL_BSC_EVENT_AUDIO_STREAMING_STOPPED:
            return "BTL_BSC_EVENT_AUDIO_STREAMING_STOPPED";

        case BTL_BSC_EVENT_INCOMING_CALL:
            return "BTL_BSC_EVENT_INCOMING_CALL";

        case BTL_BSC_EVENT_CALL_FINISHED:
            return "BTL_BSC_EVENT_CALL_FINISHED";

        default:
            return "UNKNOWN";
    }
}

