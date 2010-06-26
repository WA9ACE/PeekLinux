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
*   FILE NAME:      btl_common.c
*
*   DESCRIPTION:    This file implements the common API of the BTL component and 
*					general types, defines, and prototypes for BTL component.
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "btl_commoni.h" 
#include "btl_pool.h"
#include "btl_config.h"
#include "btl_defs.h"
#include "btl_apphandle.h"
#include "btl_utils.h"
#include "btl_debug.h"
#include "bthal_common.h"
#include "bthal_btdrv.h"
#include "bthal_pm.h"
#include "bthal_fs.h"
#include "bthal_os.h"
#include "bthal_mm.h"
#include "bthal_md.h"
#include "bthal_mc.h"
#include "bthal_vc.h"
#include "btl_bmgi.h"
#include "EBTIPS_version.h"
#include "me.h"
#include "mexp.h"
#include "medev.h"
#include "debug.h"
#include "osapi.h"
#include "chip_pm.h"
#include "ti_chip_mngr.h"
#include "utils.h"

BTL_CONFIGI_BTL_INIT_DECLARATIONS

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BTL_COMMON);

#define _BTL_MAX_NUM_OF_DISCONNECTED_DEVICES	(NUM_BT_DEVICES)
#define _BTL_INVALID_DISCONNECTED_DEVICE_INDEX	(NUM_BT_DEVICES)

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/
 
typedef enum
{
	_BTL_RADIO_ON_EVENT_START,
	_BTL_RADIO_ON_EVENT_STACK_SIGNAL,
	_BTL_RADIO_ON_EVENT_HCI_INITIALIZATION,
	_BTL_RADIO_ON_EVENT_HCI_DEINITIALIZED,
	_BTL_RADIO_ON_EVENT_MODULE_COMPLETED,
	_BTL_RADIO_ON_EVENT_CHIP_MNGR_BT_ON_COMPLETED,
	_BTL_RADIO_ON_EVENT_CHIP_MNGR_BT_ON_ABORTED,
	_BTL_RADIO_ON_EVENT_ABORT
} _BtlRadioOnEventType;

typedef enum
{
	_BTL_RADIO_ON_STATE_NONE,
	_BTL_RADIO_ON_STATE_POWER_UP_CHIP,
	_BTL_RADIO_ON_WAITING_FOR_CHIP_MNGR_AND_ME_HCI_INIT_EVENT,
	_BTL_RADIO_ON_STATE_NOTIFY_BMG,
	_BTL_RADIO_ON_STATE_BMG_PREPARING_FOR_RADIO_ON,
	_BTL_RADIO_ON_STATE_NOTIFY_NON_BMG_MODULES,
	_BTL_RADIO_ON_STATE_NON_BMG_MODULES_PREPARING_FOR_RADIO_ON,
	_BTL_RADIO_ON_STATE_ABORTING,
	_BTL_RADIO_ON_STATE_DONE
} _BtlRadioOnStateType;

typedef enum
{
	_BTL_RADIO_OFF_EVENT_START,
	_BTL_RADIO_OFF_EVENT_START_ABORT,
	_BTL_RADIO_OFF_EVENT_STACK_SIGNAL,
	_BTL_RADIO_OFF_EVENT_DISCONNECTION,
	_BTL_RADIO_OFF_EVENT_HCI_DEINITIALIZED,
	_BTL_RADIO_OFF_EVENT_MODULE_COMPLETED,
	_BTL_RADIO_OFF_EVENT_CHIP_MNGR_BT_OFF_COMPLETED,
	_BTL_RADIO_OFF_EVENT_ABORT_ON_SUCCESS,
	_BTL_RADIO_OFF_EVENT_ABORT_ON_FAIL
} _BtlRadioOffEventType;

typedef enum
{
	_BTL_RADIO_OFF_STATE_NONE,
	_BTL_RADIO_OFF_STATE_ABORT_START,		
	_BTL_RADIO_OFF_STATE_NOTIFY_BMG,
	_BTL_RADIO_OFF_STATE_BMG_PREPARING_FOR_RADIO_OFF,
	_BTL_RADIO_OFF_STATE_NOTIFY_NON_BMG_MODULES,
	_BTL_RADIO_OFF_STATE_NON_BMG_MODULES_PREPARING_FOR_RADIO_OFF,
	_BTL_RADIO_OFF_STATE_DISCONNECT_LINKS,
	_BTL_RADIO_OFF_STATE_DISCONNECTING,
	_BTL_RADIO_OFF_STATE_SEND_EVENT_TO_SHUTDOWN_RADIO,
	_BTL_RADIO_OFF_STATE_SHUTDOWN_RADIO,
	_BTL_RADIO_OFF_CALLING_RADIO_SHUT_DOWN,
	_BTL_RADIO_OFF_WAITING_FOR_RADIO_SHUTDOWN,
	_BTL_RADIO_OFF_WAITING_FOR_CHIP_MNGR_AND_ME_HCI_DEINIT_EVENT,
	_BTL_RADIO_OFF_STATE_ABORTING,
	_BTL_RADIO_OFF_STATE_DONE
} _BtlRadioOffStateType;

typedef enum
{
	_BTL_STATE_CONTROL_TYPE_NONE,
	_BTL_STATE_CONTROL_TYPE_INIT,
	_BTL_STATE_CONTROL_TYPE_DEINIT,
	_BTL_STATE_CONTROL_TYPE_RADIO_ON,
	_BTL_STATE_CONTROL_TYPE_RADIO_OFF
} _BtlStateControlProcessType;


typedef enum
{
	_BTL_RADIO_ON_ABORT_STATE_NONE,
	_BTL_RADIO_ON_ABORT_STATE_ABORTING,
	_BTL_RADIO_ON_ABORT_STATE_FAIL,
	_BTL_RADIO_ON_ABORT_STATE_SUCCESS
} _BtlRadioOnAbortStateType;

typedef enum
{
	_BTL_RADIO_OFF_ABORT_STATE_NONE,
	_BTL_RADIO_OFF_ABORT_STATE_ABORTING
} _BtlRadioOffAbortStateType;

typedef struct _BtlModuleCompletedData
{
	BtlModuleType 							moduleType;
	const BtlModuleNotificationCompletionEvent 	*event;
} BtlModuleCompletedData;

/*-------------------------------------------------------------------------------
 * BtlSubState type
 *
 *     Sub-State of the Bluetooth protocol stack and radio
 */
 typedef U8 BtlSubState;

/* Stack is not initialized */
#define BTL_SUB_STATE_NOT_INITIALIZED				(0)

/* The stack is initializing */
#define BTL_SUB_STATE_INITIALIZING					(1)

/* The stack has encountered an error while initializing the BT S/W */
#define BTL_SUB_STATE_INITIALIZATION_ERR  			(2)

/* Stack is initialized, BT radio is off => BT is not operational */
#define BTL_SUB_STATE_RADIO_OFF					(3)

/* The radio is initializing */
#define BTL_SUB_STATE_INITIALIZING_RADIO			(4)

/* The stack has encountered an error while initializing the radio H/W */
#define BTL_SUB_STATE_RADIO_INITIALIZATION_ERR	(5)

/* BT is fully operational (stack + radio) */
#define BTL_SUB_STATE_OPERATIONAL					(6)

/* The radio is initializing */
#define BTL_SUB_STATE_DEINITIALIZING_RADIO		(7)

typedef struct _BtlData
{
	BtlCallBack	appCallback;
	BtStatus		appEventStatus;
	ListEntry		appHandlesList;
	BtlAppHandle	*defaultAppHandle;
	U32			defaultAppHandleRefCount;

	BTL_POOL_DECLARE_POOL(appHandlesPool, BtlAppHandlesMemory, BTL_CONFIG_MAX_NUM_OF_APPS, sizeof(BtlAppHandle));

	BtHandler		globalHandler;

	/* Radio Off Data */

	U32					numOfDisconnectionsInProgress;
	BtRemoteDevice		*disconnectingDevices[_BTL_MAX_NUM_OF_DISCONNECTED_DEVICES];

	_BtlRadioOnStateType		radioOnState;
	_BtlRadioOnAbortStateType   radioOnAbortState;
	_BtlRadioOffStateType	    radioOffState;
	_BtlRadioOffAbortStateType  radioOffAbortState;

	BtlCommonNotificationsCallback	moduleNotificationsCbs[BTL_MODULE_TYPE_NUM_OF_MODULES];
	U32						numOfModulesThatShouldNotifyCompletion;

	_BtlStateControlProcessType		stateControlState;
} BtlData;

static BtlData	_btlData;

static const char BtlAppHandlesPoolName[] = "AppHandles";

static BtlState _btlState = BTL_STATE_NOT_INITIALIZED;

/********************************************************************************
 *
 * Forward declarations
 *
 *******************************************************************************/

static void _BTL_InitData(void);

extern void OsStackEventCallback(BthalOsEvent evtMask);
extern void BTL_StateControlCb(void);

static void BTL_InitImplementation(void);
static void BTL_DeinitImplementation(void);
	
static BtStatus BTL_InitModules(void);
static BtStatus BTL_DeinitModules(void);

const char *BtlCommonRadioOnProcessorStateAsStr(_BtlRadioOnStateType state);
const char *BtlCommonBtRadioOnProcessorEventAsStr(_BtlRadioOnEventType event);
const char *BtlCommonRadioOffProcessorStateAsStr(_BtlRadioOffStateType state);
const char *BtlCommonBtRadioOffProcessorEventAsStr(_BtlRadioOffEventType event);
const char *BtlChipMngrCallbackEventAsStr(TiChipMngrBtNotificationType event);


/*-------------------------------------------------------------------------------
 * BTL_InitInternal()
 *
 *		Initializes BTL component. It is usually called at system startup.
 *		This function must be called by the system before any other API function.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *		void
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BTL was initialized successfully.
 *
 *		BT_STATUS_FAILED -  BTL failed initialization.
 */
static BtStatus BTL_InitInternal(void);

/*-------------------------------------------------------------------------------
 * BTL_DeinitInternal()
 *
 *		Deinitializes BTL component.
 *		After calling this function, no API function can be called.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BTL was deinitialized successfully.
 *
 *		BT_STATUS_FAILED -  BTL failed deinitialization.
 */
static BtStatus BTL_DeinitInternal(void);

static void BTL_CHIP_MNGR_Callback(TiChipMngrBtNotificationType notificationType);

static BtStatus _BTL_RadioOnProcessor(_BtlRadioOnEventType radioOnEvent, const void *additionalData);
static BtStatus _BTL_RadioOffProcessor(_BtlRadioOffEventType radioOffEvent, const void *additionalData);

static BtStatus _BTL_DisconnectAllLinks();
static void _BTL_LinkDisconnected(const BtRemoteDevice *remoteDevice, BOOL *haveAllLinksDisconnected);
static U32 _BTL_FindEmptyDisconnectionEntry();

static void _BTL_HALInitCallback(const BthalEvent	*event);

static void _BTL_GlobalHandler(const BtEvent *event);


/*-------------------------------------------------------------------------------
 * BTL_GetDefaultAppHandle()
 *
 *		Returns the default application handle.
 *
 *		This function should be called whenever an instance of a module is created
 *		with 0 as the application handle. This will occur when applications have no use
 *		of an application handle.
 *	
 *		To support this usage, a single application handle is allocted the first
 *		time it is needed (lazy initialization). During subsequent calls, the same
 *		handle is returned.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		appHandle [out] - retruned default application handle.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Application handle was allocated successfully.
 *
 *     	BT_STATUS_NO_RESOURCES - operation failed because the number of
 *		allocated application handles exceeds the maximum number allowed
 *
 *		BT_STATUS_INVALID_PARM -  The operation failed due to an invalid parameter
 *
 *		BT_STATUS_INTERNAL_ERROR - Some serious internal error occurred, BTL must be reset.
 */
BtStatus BTL_GetDefaultAppHandle(const BtlAppHandle **appHandle);

BtStatus BTL_ReleaseDefaultAppHandle(void);

/*-------------------------------------------------------------------------------
 * BTL_CreateContext()
 *
 *		Initializes the "members" of a base BTL context structure instance
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		context [in / out] - context to initialize
 *
 *		appHandle [in] - application handle with which this context is associated.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Application handle was allocated successfully.
 *
 *     	BT_STATUS_NO_RESOURCES - operation failed because the number of
 *		allocated application handles exceeds the maximum number allowed
 *
 *		BT_STATUS_INVALID_PARM -  The operation failed due to an invalid parameter
 *
 *		BT_STATUS_INTERNAL_ERROR - Some serious internal error occurred, BTL must be reset.
 */
BtStatus BTL_CreateContext(BtlContext *context, BtlModuleType	moduleType, BtlAppHandle *appHandle);

BtStatus BTL_DestroyContext(BtlContext *context);

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/
BtStatus BTL_Init(BtlCallBack callback)
{
	BtStatus 		status = BT_STATUS_SUCCESS;
	BthalStatus	bthalStatus = BTHAL_STATUS_SUCCESS;
	
	/* [@ToDo][Udi] - The BTHAL_Logger must first be initialized before we start logging */
		
	BTL_FUNC_START("BTL_Init");

	BTL_VERIFY_FATAL((_btlState == BTL_STATE_NOT_INITIALIZED), BT_STATUS_ALREADY_INITIALIZED,
						("BTIPS Already Initialized"));
	BTL_VERIFY_FATAL((callback != 0), BT_STATUS_INVALID_PARM, ("Null callback"));

	_btlState = BTL_STATE_INITIALIZATION_ERR;

	_btlData.stateControlState = _BTL_STATE_CONTROL_TYPE_INIT;

	_BTL_InitData();

	_btlData.appCallback = callback;
	
	/* Print EBTIPS version */
	BTL_LOG_INFO(("BTIPS Software Version - %s%d.%d%d.%d",
				  EBTIPS_TARGET_OS,
				  EBTIPS_SOFTWARE_VERSION_X,
				  EBTIPS_SOFTWARE_VERSION_Y,
				  EBTIPS_SOFTWARE_VERSION_Z,
				  EBTIPS_SOFTWARE_VERSION_B));

	/* BTHAL OS must be initialized to be able to create the stack task and to send the event below
		In addition, it must be synchronous
	*/
	bthalStatus = BTHAL_OS_Init(_BTL_HALInitCallback);
	BTL_VERIFY_FATAL_NO_RETVAR((bthalStatus == BTHAL_STATUS_SUCCESS), 
									("BTHAL_OS_Init Failed (%d)", bthalStatus));


    /*------------------------------------------------------------
     * Create the stack task.
     */
    /* Note: We must set the init flag here so that the StackThread
     * will execute properly.
     */
  	bthalStatus = BTHAL_OS_CreateTask(BTHAL_OS_TASK_HANDLE_STACK, 
									  OsStackEventCallback, 
									  "STACK_TASK");
	BTL_VERIFY_FATAL((bthalStatus == BTHAL_STATUS_SUCCESS), BT_STATUS_INTERNAL_ERROR, 
						("Failed creating Stack Task (%d)", bthalStatus));

#ifndef BTL_CONFIG_INIT_IN_STACK_TASK

	BTL_InitImplementation();

#else

	/* Send a the Init event to the stack task to execute the init code in that context */
	bthalStatus = BTHAL_OS_SendEvent(BTHAL_OS_TASK_HANDLE_STACK, OS_EVENT_STACK_TASK_BTL_STATE_CONTROL);
	BTL_VERIFY_FATAL((BTHAL_STATUS_SUCCESS == bthalStatus), BT_STATUS_INTERNAL_ERROR,
						("Failed sending Init event 0x%x to Stack task!", OS_EVENT_STACK_TASK_BTL_STATE_CONTROL));
#endif

	BTL_FUNC_END();

	return status;
}

void BTL_StateControlCb(void)
{
	BTL_FUNC_START_AND_LOCK_COMMON("BTL_StateControlCb");

	switch (_btlData.stateControlState)
	{
		case _BTL_STATE_CONTROL_TYPE_INIT:

			BTL_InitImplementation();
			
			break;

		case _BTL_STATE_CONTROL_TYPE_DEINIT:

			BTL_DeinitImplementation();
			
			break;
			
		case _BTL_STATE_CONTROL_TYPE_RADIO_ON:

			_BTL_RadioOnProcessor(_BTL_RADIO_ON_EVENT_STACK_SIGNAL, NULL);

			break;
			
		case _BTL_STATE_CONTROL_TYPE_RADIO_OFF:

			_BTL_RadioOffProcessor(_BTL_RADIO_OFF_EVENT_STACK_SIGNAL, NULL);

			break;
			
		default:

			BTL_FATAL_NO_RETVAR(("Invalid State Control State (%d)", _btlData.stateControlState));
			
	};

	BTL_FUNC_END_AND_UNLOCK();
}

void BTL_InitImplementation(void)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	TiChipMngrStatus 	chipMngrStatus = TI_CHIP_MNGR_STATUS_SUCCESS;
	BthalStatus 		bthalStatus = BTHAL_STATUS_SUCCESS;
	BtFsStatus		bthalFsStatus = BT_STATUS_HAL_FS_SUCCESS;
	BOOL			osInitResult = FALSE;

	BTL_FUNC_START("BTL_InitImplementation");

	BTHAL_BTDRV_Init();
	
	chipMngrStatus = TI_CHIP_MNGR_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((chipMngrStatus == TI_CHIP_MNGR_STATUS_SUCCESS),  ("TI_CHIP_MNGR_Init Failed (%d)", chipMngrStatus));

	chipMngrStatus = TI_CHIP_MNGR_RegisterForBTNotifications(BTL_CHIP_MNGR_Callback);
	BTL_VERIFY_FATAL_NO_RETVAR((chipMngrStatus == TI_CHIP_MNGR_STATUS_SUCCESS),  ("TI_CHIP_MNGR_Init Failed (%d)", chipMngrStatus));
	
	/* Put initialization of other basic BTHAL components (FS...) here */

	/* Initialize BT FS */
	bthalFsStatus = BTHAL_FS_Init(_BTL_HALInitCallback);
	BTL_VERIFY_FATAL_NO_RETVAR((bthalFsStatus == BT_STATUS_HAL_FS_SUCCESS), 
									("BTHAL_FS_Init Failed (%d)", bthalFsStatus));

	/* Initialize BT Host Power Manager */
	bthalStatus = BTHAL_PM_Init(_BTL_HALInitCallback);
	BTL_VERIFY_FATAL_NO_RETVAR((bthalStatus == BTHAL_STATUS_SUCCESS), 
									("BTHAL_PM_Init Failed (%d)", bthalStatus));

	/* Initialize Multimedia */
	bthalStatus = BTHAL_MM_Init(_BTL_HALInitCallback);
	BTL_VERIFY_FATAL_NO_RETVAR((bthalStatus == BTHAL_STATUS_SUCCESS), 
									("BTHAL_MM_Init Failed (%d)", bthalStatus));

	/* Initialize Modem Data service */
	bthalStatus = BTHAL_MD_Init(_BTL_HALInitCallback);
	BTL_VERIFY_FATAL_NO_RETVAR((bthalStatus == BTHAL_STATUS_SUCCESS), 
									("BTHAL_MD_Init Failed (%d)", bthalStatus));
	
	/* Initialize BT Modem Control */
	bthalStatus = BTHAL_MC_Init(_BTL_HALInitCallback);
	BTL_VERIFY_FATAL_NO_RETVAR((bthalStatus == BTHAL_STATUS_SUCCESS), 
									("BTHAL_MC_Init Failed (%d)", bthalStatus));

	
	/* Initialize BT Voice Control */
	bthalStatus = BTHAL_VC_Init(_BTL_HALInitCallback);
	BTL_VERIFY_FATAL_NO_RETVAR((bthalStatus == BTHAL_STATUS_SUCCESS), 
									("BTHAL_VC_Init Failed (%d)", bthalStatus));


	/* Initialize CHIP PM module - chip is off */
	CHIP_PM_Init(FALSE);

	/* Initialize BT stack and radio */
	osInitResult = OS_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((osInitResult == TRUE), ("OS_Init Failed"));
	
	ME_InitHandler(&_btlData.globalHandler);
	_btlData.globalHandler.callback = _BTL_GlobalHandler;

	status = ME_IntRegisterGlobalHandler(&_btlData.globalHandler);
	BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status),
									("Failed Registering Global Handler (%s)", pBT_Status(status)));

	status = ME_IntSetEventMask(&_btlData.globalHandler, 
							BEM_LINK_CONNECT_IND | BEM_LINK_DISCONNECT | BEM_LINK_CONNECT_CNF);
	BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status),
									("ME_SetEventMask Failed (%s)", pBT_Status(status)));

	/* Initialize BTL component */
	status = BTL_InitInternal();
	BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status), (""));

	status = BTL_InitModules();
	BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status), (""));

	_btlData.stateControlState = _BTL_STATE_CONTROL_TYPE_NONE;
	
	/* Initialization completed successfully, radio is off */
	_btlState = BTL_STATE_RADIO_OFF;

	BTL_FUNC_END();
}

BtStatus BTL_InitModules(void)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTL_InitModules");
	
	status = BTL_BSC_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_BSC_Init Failed (%s)", pBT_Status(status)));

	status = BTL_BMG_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_BMG_Init Failed (%s)", pBT_Status(status)));

       status = BTL_SPP_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_SPP_Init Failed (%s)", pBT_Status(status)));
		
       status = BTL_MDG_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_MDG_Init Failed (%s)", pBT_Status(status)));
		
	status = BTL_OPPC_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_OPPC_Init Failed (%s)", pBT_Status(status)));

	status = BTL_OPPS_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_OPPS_Init Failed (%s)", pBT_Status(status)));
	
	status = BTL_BPPSND_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_BPPSND_Init Failed (%s)", pBT_Status(status)));

	status = BTL_BIPINT_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_BIPINT_Init Failed (%s)", pBT_Status(status)));

	status = BTL_BIPRSP_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_BIPRSP_Init Failed (%s)", pBT_Status(status)));
	
	status = BTL_PBAPS_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_PBAPS_Init Failed (%s)", pBT_Status(status)));

	status = BTL_A2DP_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_A2DP_Init Failed (%s)", pBT_Status(status)));

	status = BTL_AVRCPTG_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_AVRCPTG_Init Failed (%s)", pBT_Status(status)));

	status = BTL_FTPS_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_FTPS_Init Failed (%s)", pBT_Status(status)));

	status = BTL_FTPC_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_FTPC_Init Failed (%s)", pBT_Status(status)));


	status = BTL_AG_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_AG_Init Failed (%s)", pBT_Status(status)));


	status = BTL_VG_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_VG_Init Failed (%s)", pBT_Status(status)));

	status = BTL_L2CAP_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_L2CAP_Init Failed (%s)", pBT_Status(status)));

	status = BTL_RFCOMM_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_RFCOMM_Init Failed (%s)", pBT_Status(status)));

	status = BTL_HIDH_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_HIDH_Init Failed (%s)", pBT_Status(status)));

	status = BTL_SAPS_Init();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_SAPS_Init Failed (%s)", pBT_Status(status)));

    BTL_FUNC_END();
	
	return status;
}


BtStatus BTL_DeinitModules(void)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTL_DeinitModules");
	
	status = BTL_MDG_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_MDG_Deinit Failed: status %s", pBT_Status(status)));

	status = BTL_SPP_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_SPP_Deinit Failed (%s)", pBT_Status(status)));
		
	status = BTL_OPPC_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_OPPC_Deinit Failed (%s)", pBT_Status(status)));

	status = BTL_OPPS_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_OPPS_Deinit Failed (%s)", pBT_Status(status)));
	
	status = BTL_BPPSND_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_BPPSND_Deinit Failed (%s)", pBT_Status(status)));

	status = BTL_BIPINT_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_BIPINT_Deinit Failed (%s)", pBT_Status(status)));

	status = BTL_BIPRSP_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_BIPRSP_Deinit Failed (%s)", pBT_Status(status)));
	
	status = BTL_PBAPS_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_PBAPS_Deinit Failed (%s)", pBT_Status(status)));

	status = BTL_A2DP_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_A2DP_Deinit Failed (%s)", pBT_Status(status)));

	status = BTL_AVRCPTG_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_AVRCPTG_Deinit Failed (%s)", pBT_Status(status)));

	status = BTL_FTPS_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_FTPS_Deinit Failed (%s)", pBT_Status(status)));

	status = BTL_FTPC_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_FTPC_Deinit Failed (%s)", pBT_Status(status)));


	status = BTL_AG_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_AG_Deinit Failed (%s)", pBT_Status(status)));


	status = BTL_VG_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_VG_Init Failed: status %s", pBT_Status(status)));

	status = BTL_L2CAP_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_L2CAP_Deinit Failed (%s)", pBT_Status(status)));

	status = BTL_RFCOMM_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_RFCOMM_Deinit Failed (%s)", pBT_Status(status)));

	status = BTL_HIDH_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_HIDH_Deinit Failed (%s)", pBT_Status(status)));

	status = BTL_SAPS_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_SAPS_Deinit Failed (%s)", pBT_Status(status)));

	status = BTL_BMG_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_BMg_Deinit Failed (%s)", pBT_Status(status)));

	status = BTL_BSC_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_SUCCESS), ("BTL_BSC_Deinit Failed (%s)", pBT_Status(status)));

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_Deinit()
{
	BTL_FUNC_START_AND_LOCK_COMMON("BTL_Deinit");
		
	BTL_DeinitImplementation();
	
	BTL_FUNC_END();
	
	return BT_STATUS_SUCCESS;
}

void BTL_DeinitImplementation(void)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	TiChipMngrStatus 	chipMngrStatus = TI_CHIP_MNGR_STATUS_SUCCESS;
	BthalStatus 		bthalStatus = BTHAL_STATUS_SUCCESS;
	BtFsStatus		bthalFsStatus = BT_STATUS_HAL_FS_SUCCESS;

	BTL_FUNC_START("BTL_DeinitImplementation");
		
	status = BTL_DeinitModules();
	BTL_VERIFY_ERR_NORET((status == BT_STATUS_SUCCESS), ("")); 
			
	/* Deinitialize BTL component */
	status = BTL_DeinitInternal();
	BTL_VERIFY_ERR_NORET((status == BT_STATUS_SUCCESS), ("")); 
	
	/* Deinitialize OS resources */
	OS_Deinit();

	/* Deinitialize CHIP PM module */
	CHIP_PM_Deinit();

	bthalStatus = BTHAL_VC_Deinit();
	BTL_VERIFY_ERR_NORET((bthalStatus == BTHAL_STATUS_SUCCESS), (""));

	bthalStatus = BTHAL_MC_Deinit();
	BTL_VERIFY_ERR_NORET((bthalStatus == BTHAL_STATUS_SUCCESS), (""));

	bthalStatus = BTHAL_MD_Deinit();
	BTL_VERIFY_ERR_NORET((bthalStatus == BTHAL_STATUS_SUCCESS), (""));

	/* Deinitialize Multimedia */
	bthalStatus = BTHAL_MM_Deinit();
	BTL_VERIFY_ERR_NORET((bthalStatus == BTHAL_STATUS_SUCCESS), (""));
	
	/* Deinitialize BT Host Power Manager */
	bthalStatus = BTHAL_PM_Deinit();
	BTL_VERIFY_ERR_NORET((bthalStatus == BTHAL_STATUS_SUCCESS), ("")); 

	/* Deinitialize BT FS */
	bthalFsStatus = BTHAL_FS_DeInit();
	BTL_VERIFY_ERR_NORET((bthalStatus == BT_STATUS_HAL_FS_SUCCESS), ("")); 

	chipMngrStatus = TI_CHIP_MNGR_Deinit();
	BTL_VERIFY_FATAL_NO_RETVAR((chipMngrStatus == TI_CHIP_MNGR_STATUS_SUCCESS),  ("TI_CHIP_MNGR_Deinit Failed (%d)", chipMngrStatus));

	/* 
		NOT DESTROYING STACK TASK
	*/
	
	/* Deinitialize BTHAL_OS resources */
	bthalStatus = BTHAL_OS_Deinit();
	BTL_VERIFY_ERR_NORET((bthalStatus == BTHAL_STATUS_SUCCESS), ("")); 

	_btlState = BTL_STATE_NOT_INITIALIZED;

	BTL_FUNC_END();
	
}

 BtStatus BTL_RadioOn(void)
 {
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_RadioOn");
	
	BTL_VERIFY_ERR((BTL_IsInitialized() == TRUE), BT_STATUS_IMPROPER_STATE, ("BTIPS Is Not Initialized"));
	
	if (_btlState == BTL_STATE_OPERATIONAL)
	{
		BTL_LOG_INFO(("Radio is already On"));
		BTL_RET(BT_STATUS_SUCCESS);
	}

	BTL_VERIFY_ERR((_btlState == BTL_STATE_RADIO_OFF), BT_STATUS_IMPROPER_STATE, ("Radio Can't be turned on at this moment"));
	
	_btlData.stateControlState = _BTL_STATE_CONTROL_TYPE_RADIO_ON;

	status = _BTL_RadioOnProcessor(_BTL_RADIO_ON_EVENT_START, NULL);
	
	BTL_FUNC_END_AND_UNLOCK();

	BTL_LOG_DEBUG(("BTL_RadioOn: returned %s",  BTL_DEBUG_pBtStatus(status)));

	return status;
 }

 BtStatus BTL_RadioOff(void)
 {
 	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_RadioOff");

	BTL_VERIFY_ERR((BTL_IsInitialized() == TRUE), BT_STATUS_IMPROPER_STATE, ("BTIPS Is Not Initialized"));
	
	if (_btlState == BTL_STATE_RADIO_OFF)
	{
		BTL_LOG_INFO(("Radio is already Off"));
		BTL_RET(BT_STATUS_SUCCESS);
	}

	BTL_VERIFY_ERR((_btlState == BTL_STATE_OPERATIONAL) || (_btlState == BTL_STATE_INITIALIZING_RADIO), 
            BT_STATUS_IMPROPER_STATE, ("Radio Can't be turned off at this moment"));

	BTL_VERIFY_ERR((_btlData.radioOffState == _BTL_RADIO_OFF_STATE_NONE), 
            BT_STATUS_IMPROPER_STATE, ("Radio Can't be turned off at this moment"));
    
	status = _BTL_RadioOffProcessor(_BTL_RADIO_OFF_EVENT_START, NULL);
	
	BTL_FUNC_END_AND_UNLOCK();

	BTL_LOG_DEBUG(("BTL_RadioOff: returned %s",  BTL_DEBUG_pBtStatus(status)));
	
	return status;
 }

BtlState BTL_GetState(void)
{
	BtlState	state;
	
 	BTL_FUNC_START_AND_LOCK_COMMON("BTL_GetState");

	state = _btlState;
	
	BTL_FUNC_END_AND_UNLOCK();
	
 	return state;
}

void _BTL_InitData()
{
	BTHAL_UTILS_MemSet((U8*)&_btlData, 0, sizeof(_btlData));	
	
	_btlData.appCallback = 0;

	InitializeListHead(&_btlData.appHandlesList);

	_btlData.defaultAppHandle = 0;
	_btlData.defaultAppHandleRefCount = 0;	

	ME_InitHandler(&_btlData.globalHandler);
	_btlData.globalHandler.callback = 0;
	
	/* XXX _btlData.numOfDisconnectionsInProgress = 0; */

	_btlData.radioOnState = _BTL_RADIO_ON_STATE_NONE;
	_btlData.radioOnAbortState = _BTL_RADIO_ON_ABORT_STATE_NONE;
	_btlData.radioOffState = _BTL_RADIO_OFF_STATE_NONE;
	_btlData.radioOffAbortState = _BTL_RADIO_OFF_ABORT_STATE_NONE;
	_btlData.stateControlState = _BTL_STATE_CONTROL_TYPE_NONE;

	OS_MemSet((U8*)_btlData.moduleNotificationsCbs, (U8)0, sizeof(_btlData.moduleNotificationsCbs));

	_btlData.numOfModulesThatShouldNotifyCompletion = 0;
}

BtStatus BTL_InitInternal(void)
{
	BtStatus	status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_InitInternal");
	
	status = BTL_POOL_Create(	&_btlData.appHandlesPool,
								BtlAppHandlesPoolName,
								_btlData.BtlAppHandlesMemory, 
								BTL_CONFIG_MAX_NUM_OF_APPS,
								sizeof(BtlAppHandle));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_FAILED, ("App Handles pool creation failed"));
	
	BTL_FUNC_END_AND_UNLOCK();
		
	return status;
}

BtStatus BTL_DeinitInternal()
{
	BtStatus	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_DeinitInternal");

	status = BTL_POOL_Destroy(&_btlData.appHandlesPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("App Handles pool destruction failed"));
		
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BOOL BTL_IsInitialized()
{
	if ((_btlState == BTL_STATE_RADIO_OFF) || 
		(_btlState == BTL_STATE_OPERATIONAL) ||
		(_btlState == BTL_STATE_INITIALIZING_RADIO) ||
		(_btlState == BTL_STATE_RADIO_INITIALIZATION_ERR) || 
		(_btlState == BTL_STATE_DEINITIALIZING_RADIO))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL BTL_IsOperational()
{
	if (_btlState == BTL_STATE_OPERATIONAL)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL BTL_IsChipOn()
{
	BtStackState stackInitState = ME_GetStackInitState();

	/* Check if the chip is on */
	if (stackInitState ==  BTSS_INITIALIZED)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BtStatus _BTL_DisconnectAllLinks()
{
	BtStatus	status = BT_STATUS_SUCCESS;
	BtStatus	tempStatus = BT_STATUS_SUCCESS;
	
	BtDeviceContext	*deviceContext = 0;
	BtRemoteDevice	*remoteDevice = 0;

	BTL_FUNC_START("_BTL_DisconnectAllLinks");

	/* Iterate over all existing connections and force their disconnections */

	/* Set to 0 to start from the first device*/
	deviceContext = 0;
	
       while (DS_EnumDeviceList(&deviceContext) == BT_STATUS_SUCCESS)
	{
		remoteDevice = deviceContext->link;
		
		/* Check if there is a connection to the device */
		if (remoteDevice != 0)
		{
			/* There is a connection, force its disconnection */
			tempStatus = ME_ForceDisconnectLinkWithReason(0, remoteDevice, BEC_USER_TERMINATED, TRUE);

			if (tempStatus != BT_STATUS_SUCCESS)
			{
				BTL_LOG_ERROR(("ME_ForceDisconnectLinkWithReason Failed, Status: |%s|, Address: |%s|",
								pBT_Status(tempStatus), BTL_UTILS_LockedBdaddrNtoa(&deviceContext->addr)));
			}
			else
			{
				U32 freeIndex = _BTL_FindEmptyDisconnectionEntry();
				BTL_VERIFY_ERR((freeIndex != _BTL_INVALID_DISCONNECTED_DEVICE_INDEX), BT_STATUS_NO_RESOURCES, (""));

				/* Record the remote device address to match when device completes disconnection */
				_btlData.disconnectingDevices[freeIndex] = remoteDevice;
				
				/* Another disconnection in progress */
				++_btlData.numOfDisconnectionsInProgress;
				
				/* At least one disconnection in progress => caller should know operation is in progress */
				status = BT_STATUS_PENDING;
			}
		}
	}

	BTL_FUNC_END();

	return status;
}

U32 _BTL_FindEmptyDisconnectionEntry()
{
	U32 		deviceIndex = 0;
	U32		emptyIndex = _BTL_INVALID_DISCONNECTED_DEVICE_INDEX;
	
	BTL_FUNC_START("_BTL_FindEmptyDisconnectionEntry");

	/* Find the entry of the device that disconnected and update state accordingly */
	for (deviceIndex = 0; deviceIndex < _BTL_MAX_NUM_OF_DISCONNECTED_DEVICES; ++deviceIndex)
	{
		if (_btlData.disconnectingDevices[deviceIndex] == 0)
		{
			emptyIndex =  deviceIndex;
			break;
		}
	}

	BTL_VERIFY_ERR_NO_RETVAR((emptyIndex  != _BTL_INVALID_DISCONNECTED_DEVICE_INDEX), 
								("No free entry for a disconnected device (max =  %d)", 
								_BTL_MAX_NUM_OF_DISCONNECTED_DEVICES));

	BTL_FUNC_END();

	return emptyIndex;
}

void _BTL_LinkDisconnected(const BtRemoteDevice *remoteDevice, BOOL *haveAllLinksDisconnected)
{
	U32 		deviceIndex = 0;
	BOOL	entryFound = FALSE;
	
	BTL_FUNC_START("_BTL_LinkDisconnected");

	*haveAllLinksDisconnected = FALSE;

	/* Find the entry of the device that disconnected and update state accordingly */
	for (deviceIndex = 0; deviceIndex < _BTL_MAX_NUM_OF_DISCONNECTED_DEVICES; ++deviceIndex)
	{
		if (_btlData.disconnectingDevices[deviceIndex] == remoteDevice)
		{
			BTL_LOG_DEBUG(("Device Whose Index is %d Disconnected", deviceIndex));

			entryFound = TRUE;
			
			_btlData.disconnectingDevices[deviceIndex] = 0;
			--_btlData.numOfDisconnectionsInProgress;

			BTL_LOG_DEBUG(("%d Links left to complete disconnection"));

			if (_btlData.numOfDisconnectionsInProgress == 0)
			{
				*haveAllLinksDisconnected = TRUE;
			}

			break;
		}
	}

	BTL_VERIFY_ERR_NO_RETVAR((entryFound == TRUE), 
								("Disconnected Device wasn't disconnected by me (%s)", 
								BTL_UTILS_LockedBdaddrNtoa(&remoteDevice->bdAddr)));
													

	BTL_FUNC_END();
}

BtStatus _BTL_NotifyBmg(BtlModuleNotificationType notificationType)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("_BTL_NotifyBmg");

	BTL_LOG_DEBUG(("notificationType: %d", notificationType));

	_btlData.numOfModulesThatShouldNotifyCompletion = 0;

	BTL_VERIFY_FATAL(_btlData.moduleNotificationsCbs[BTL_MODULE_TYPE_BMG] != 0, BT_STATUS_INTERNAL_ERROR,
						("BMG Didn't register for Noifications with BTL_Common"));

	status = (_btlData.moduleNotificationsCbs[BTL_MODULE_TYPE_BMG])(notificationType);

	if (status == BT_STATUS_PENDING)
	{
		++_btlData.numOfModulesThatShouldNotifyCompletion;
	 }
	
	BTL_FUNC_END();

	return status;
}
	
BtStatus _BTL_NotifyNonBmgModules(BtlModuleNotificationType notificationType)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	BtStatus 	currStatus = BT_STATUS_SUCCESS;
	U32		moduleIndex = 0;
	
	BTL_FUNC_START("_BTL_NotifyNonBmgModules");

	BTL_LOG_DEBUG(("notificationType: %d", notificationType));

	for (moduleIndex = 0; moduleIndex < BTL_MODULE_TYPE_NUM_OF_MODULES; ++moduleIndex)
	{
		if ((moduleIndex != BTL_MODULE_TYPE_BMG) && (_btlData.moduleNotificationsCbs[moduleIndex] != 0))
		{
			currStatus = (_btlData.moduleNotificationsCbs[moduleIndex])(notificationType);

			if (currStatus == BT_STATUS_PENDING)
			{
				++_btlData.numOfModulesThatShouldNotifyCompletion;
				status = BT_STATUS_PENDING;
			 }
		}
	}
	
	BTL_FUNC_END();

	return status;
}

BtStatus _BTL_HandleModuleCompletedNotification(BtlModuleCompletedData *moduleCompletedData)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("_BTL_HandleModuleCompletedNotification");

	BTL_LOG_DEBUG(("Module %d Completed, %d left", moduleCompletedData->moduleType, _btlData.numOfModulesThatShouldNotifyCompletion));

	BTL_VERIFY_ERR((_btlData.numOfModulesThatShouldNotifyCompletion > 0), BT_STATUS_INTERNAL_ERROR,
					("All modules have already notified completion, and now %d notifies %d", 
					moduleCompletedData->moduleType, moduleCompletedData->event->type));
	
	--_btlData.numOfModulesThatShouldNotifyCompletion;

	if (_btlData.numOfModulesThatShouldNotifyCompletion > 0)
	{
		status = BT_STATUS_PENDING;
	}

	BTL_FUNC_END();

	return status;
}

BtStatus _BTL_RadioOnProcessor(_BtlRadioOnEventType radioOnEvent, const void *additionalData)
{
	BtStatus 					status = BT_STATUS_SUCCESS;
	TiChipMngrStatus			chipMngrStatus = TI_CHIP_MNGR_STATUS_SUCCESS;
	BOOL					keepProcessing = TRUE;
	BtlEvent					appEvent;
	BthalStatus		        	bthalStatus = BTHAL_STATUS_SUCCESS;
	BtEvent					*btEvent = 0;
	static BOOL				chipMngrOnEventReceived = FALSE;
	static BOOL				chipMngrAbortEventReceived = FALSE;
	static BOOL				hciInitEventReceived = FALSE;
	static BOOL				hciDeinitEventReceived = FALSE;
	BtlModuleCompletedData	*moduleCompletedData = 0;
	
	BTL_FUNC_START("_BTL_RadioOnProcessor");

	while (keepProcessing == TRUE)
	{
		BTL_LOG_DEBUG(("State: %s, Event: %s", BtlCommonRadioOnProcessorStateAsStr(_btlData.radioOnState), 
			BtlCommonBtRadioOnProcessorEventAsStr(radioOnEvent)));

		keepProcessing = FALSE;

		/* [@ToDo][Udi] Handle the case where the whole process completes synchronously - return PENDING if not */
		
		switch (_btlData.radioOnState)
		{
			case _BTL_RADIO_ON_STATE_NONE:

				BTL_VERIFY_FATAL((radioOnEvent == _BTL_RADIO_ON_EVENT_START), BT_STATUS_INTERNAL_ERROR, 
								("Invalid event (%d) when starting radio on processing", radioOnEvent));

				_btlState = BTL_STATE_INITIALIZING_RADIO;

				_btlData.appEventStatus = BT_STATUS_SUCCESS;
				_btlData.radioOnState = _BTL_RADIO_ON_STATE_POWER_UP_CHIP;
				_btlData.radioOnAbortState = _BTL_RADIO_ON_ABORT_STATE_NONE;

				chipMngrOnEventReceived = FALSE;
				chipMngrAbortEventReceived = FALSE;
				
				hciInitEventReceived = FALSE;
				hciDeinitEventReceived = FALSE;
				
				keepProcessing = TRUE;

				break;
				
			case _BTL_RADIO_ON_STATE_POWER_UP_CHIP:

				chipMngrStatus = TI_CHIP_MNGR_BTOn();

				if (chipMngrStatus == TI_CHIP_MNGR_STATUS_SUCCESS) 
				{
					BTL_LOG_INFO(("Radio Initialized"));

					_btlData.radioOnState = _BTL_RADIO_ON_STATE_NOTIFY_BMG;
					keepProcessing = TRUE;
				}
				else if (chipMngrStatus == TI_CHIP_MNGR_STATUS_PENDING)
				{					
					BTL_LOG_DEBUG(("Waiting for Radio to initialize"));

					_btlData.radioOnState = _BTL_RADIO_ON_WAITING_FOR_CHIP_MNGR_AND_ME_HCI_INIT_EVENT;

					BTL_DEBUG_CRO_CHECK_POINT(BTL_DEBUG_STATE_WAITING_FOR_CHIP_MNGR_AND_ME_HCI_INIT_EVENT_IDX);
				}
				else
				{
					BTL_FATAL_NORET(("Unexpected return value from TI_CHIP_MNGR_BTOn (%d)", chipMngrStatus));

					_btlData.appEventStatus = status;
					_btlData.radioOnState = _BTL_RADIO_ON_STATE_DONE;
					keepProcessing = TRUE;
				}

				break;

			case _BTL_RADIO_ON_WAITING_FOR_CHIP_MNGR_AND_ME_HCI_INIT_EVENT:

				switch (radioOnEvent)
				{
					case _BTL_RADIO_ON_EVENT_HCI_INITIALIZATION:

						btEvent = (BtEvent*)additionalData;
						
						BTL_VERIFY_ERR((btEvent != NULL), BT_STATUS_INTERNAL_ERROR, ("Null btEvent"));

						hciInitEventReceived = TRUE;
						
						if (btEvent->eType == BTEVENT_HCI_INITIALIZED)
						{
							BTL_LOG_INFO(("Radio Power-Up Completed Successfully"));
							
							if (chipMngrOnEventReceived == TRUE)
							{
								_btlData.radioOnState = _BTL_RADIO_ON_STATE_NOTIFY_BMG;
								keepProcessing = TRUE;
							}
						}
						else if (btEvent->eType == BTEVENT_HCI_FAILED)
						{
							BTL_LOG_ERROR(("Radio Power-Up FAILED"));
							
							_btlData.radioOnState = _BTL_RADIO_ON_STATE_DONE;
							_btlData.appEventStatus= BT_STATUS_HCI_INIT_ERR;

							keepProcessing = TRUE;
						}
						else
						{
							BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Unexpected event (%s)", pME_Event(btEvent->eType)));
						}
						
						break;

					case _BTL_RADIO_ON_EVENT_CHIP_MNGR_BT_ON_COMPLETED:

						chipMngrOnEventReceived = TRUE;

						if (hciInitEventReceived == TRUE)
						{
							_btlData.radioOnState = _BTL_RADIO_ON_STATE_NOTIFY_BMG;
							keepProcessing = TRUE;
						}

						BTL_DEBUG_CRO_CHECK_POINT(BTL_DEBUG_ABORT_ARRIVED_TOO_LATE_IDX);

						break;

					case _BTL_RADIO_ON_EVENT_ABORT:

						if ((FALSE == chipMngrOnEventReceived) && (FALSE == hciInitEventReceived)) 
						{
							chipMngrStatus = TI_CHIP_MNGR_BTOnAbort();

							if (chipMngrStatus == TI_CHIP_MNGR_STATUS_PENDING)
							{
								BTL_LOG_INFO(("Radio On Abort Initiated"));

								_btlData.radioOnState = _BTL_RADIO_ON_STATE_ABORTING;
								_btlData.radioOnAbortState = _BTL_RADIO_ON_ABORT_STATE_ABORTING;
								status = BT_STATUS_PENDING;
							}
							else if (chipMngrStatus == TI_CHIP_MNGR_STATUS_SUCCESS)
							{
								BTL_LOG_INFO(("Radio On Abort - Completed Successfully"));

								_btlData.radioOnState = _BTL_RADIO_ON_STATE_DONE;
								_btlData.radioOnAbortState = _BTL_RADIO_ON_ABORT_STATE_SUCCESS;
								status = BT_STATUS_PENDING;

								bthalStatus = BTHAL_OS_SendEvent(BTHAL_OS_TASK_HANDLE_STACK, OS_EVENT_STACK_TASK_BTL_STATE_CONTROL);
								BTL_VERIFY_FATAL((BTHAL_STATUS_SUCCESS == bthalStatus), BT_STATUS_INTERNAL_ERROR,
									("Failed sending Init event 0x%x to Stack task!", OS_EVENT_STACK_TASK_BTL_STATE_CONTROL));								
							}
							else
							{
								BTL_FATAL_NORET(("Unexpected return value from TI_CHIP_MNGR_BTOnAbort (%d)", chipMngrStatus));
							}
						}
						else
						{
							BTL_LOG_INFO(("Radio On Abort: pending, Arrived too late"));

							_btlData.radioOnAbortState = _BTL_RADIO_ON_ABORT_STATE_FAIL;
							status = BT_STATUS_PENDING;
						}
						break;
                    
					default:

						BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Unexpected radio On event (%d)",radioOnEvent));
				};

				break;

			case _BTL_RADIO_ON_STATE_NOTIFY_BMG:

				if (_BTL_RADIO_ON_EVENT_ABORT == radioOnEvent)
				{
					 BTL_LOG_INFO(("Radio On Abort: pending, state: _BTL_RADIO_ON_STATE_NOTIFY_BMG"));
                     
					_btlData.radioOnAbortState = _BTL_RADIO_ON_ABORT_STATE_ABORTING;
					 status = BT_STATUS_PENDING;
					 break;
				}
                
				status = _BTL_NotifyBmg(BTL_MODULE_NOTIFICATION_RADIO_ON);

				if (status == BT_STATUS_SUCCESS)
				{
					BTL_LOG_DEBUG(("BMG Is ready for Radio On"));
					
					_btlData.radioOnState = _BTL_RADIO_ON_STATE_NOTIFY_NON_BMG_MODULES;
					keepProcessing = TRUE;
				}
				else if (status == BT_STATUS_PENDING)
				{
					BTL_LOG_DEBUG(("Waiting for Modules to become ready for radio on"));
					
					_btlData.radioOnState = _BTL_RADIO_ON_STATE_BMG_PREPARING_FOR_RADIO_ON;

					BTL_DEBUG_CRO_CHECK_POINT(BTL_DEBUG_STATE_BMG_PREPARING_FOR_RADIO_ON_IDX);
				}
				else
				{
					BTL_LOG_ERROR(("_BTL_NotifyBmg Failed (%s) - nevertheless continuning", pBT_Status(status)));
					
					_btlData.radioOnState = _BTL_RADIO_ON_STATE_NOTIFY_NON_BMG_MODULES;
					keepProcessing = TRUE;
				}	

				break;
				
			case _BTL_RADIO_ON_STATE_BMG_PREPARING_FOR_RADIO_ON:

				if (_BTL_RADIO_ON_EVENT_ABORT == radioOnEvent)
				{
					BTL_LOG_INFO(("Radio On Abort: pending, state: _BTL_RADIO_ON_STATE_BMG_PREPARING_FOR_RADIO_ON"));

					_btlData.radioOnAbortState = _BTL_RADIO_ON_ABORT_STATE_ABORTING;
					status = BT_STATUS_PENDING;
					break;
				}
                
				moduleCompletedData = (BtlModuleCompletedData*)additionalData;
				
				BTL_VERIFY_FATAL((radioOnEvent == _BTL_RADIO_ON_EVENT_MODULE_COMPLETED), BT_STATUS_INTERNAL_ERROR,
									("Unexpected event ((%d)) while waiting for Modules to complete", radioOnEvent));
				BTL_VERIFY_FATAL((moduleCompletedData != 0), BT_STATUS_INTERNAL_ERROR,("No module completd data available"));
				BTL_VERIFY_FATAL((moduleCompletedData->moduleType == BTL_MODULE_TYPE_BMG), BT_STATUS_INTERNAL_ERROR,
									("Unexpected Module Notified Completion (%d)", moduleCompletedData->moduleType));

				status = _BTL_HandleModuleCompletedNotification((BtlModuleCompletedData*)additionalData);

				if (status == BT_STATUS_SUCCESS)
				{
					BTL_LOG_DEBUG(("BMG Is ready for Radio On"));
					
					_btlData.radioOnState = _BTL_RADIO_ON_STATE_NOTIFY_NON_BMG_MODULES;
					keepProcessing = TRUE;
				}
				
				break;
				
			case _BTL_RADIO_ON_STATE_NOTIFY_NON_BMG_MODULES:

				if (_BTL_RADIO_ON_EVENT_ABORT == radioOnEvent)
				{
					_btlData.radioOnAbortState = _BTL_RADIO_ON_ABORT_STATE_ABORTING;
					status = BT_STATUS_PENDING;
					break;
				}
                
				status = _BTL_NotifyNonBmgModules(BTL_MODULE_NOTIFICATION_RADIO_ON);

				if (status == BT_STATUS_SUCCESS)
				{
					BTL_LOG_DEBUG(("BTIPS Is ready for Radio On (All modules completed preparations"));
					
					_btlData.radioOnState = _BTL_RADIO_ON_STATE_DONE;
					keepProcessing = TRUE;
				}
				else if (status == BT_STATUS_PENDING)
				{
					BTL_LOG_DEBUG(("Waiting for Modules to become ready for radio on"));
					
					_btlData.radioOnState = _BTL_RADIO_ON_STATE_NON_BMG_MODULES_PREPARING_FOR_RADIO_ON;
				}
				else
				{
					BTL_LOG_ERROR(("_BTL_BMG_RadioOn Failed (%s) - nevertheless continuning", pBT_Status(status)));
					
					_btlData.radioOnState = _BTL_RADIO_ON_STATE_DONE;
					keepProcessing = TRUE;
				}	

				break;
				
			case _BTL_RADIO_ON_STATE_NON_BMG_MODULES_PREPARING_FOR_RADIO_ON:

				if (_BTL_RADIO_ON_EVENT_ABORT == radioOnEvent)
				{
					 _btlData.radioOnAbortState = _BTL_RADIO_ON_ABORT_STATE_ABORTING;
					status = BT_STATUS_PENDING;
					 break;
				}
                
				BTL_VERIFY_FATAL((radioOnEvent == _BTL_RADIO_ON_EVENT_MODULE_COMPLETED), BT_STATUS_INTERNAL_ERROR,
									("Unexpected event ((%d)) while waiting for Modules to complete", radioOnEvent));
				BTL_VERIFY_FATAL((additionalData != 0), BT_STATUS_INTERNAL_ERROR,("No module completd data available"));

				status = _BTL_HandleModuleCompletedNotification((BtlModuleCompletedData*)additionalData);

				if (status == BT_STATUS_SUCCESS)
				{
					BTL_LOG_DEBUG(("BTIPS Is ready for Radio On (All modules completed preparations"));
					
					_btlData.radioOnState = _BTL_RADIO_ON_STATE_DONE;
					keepProcessing = TRUE;
				}
				
				break;

			case _BTL_RADIO_ON_STATE_ABORTING:

				switch (radioOnEvent)
				{
					case _BTL_RADIO_ON_EVENT_HCI_INITIALIZATION:

						btEvent = (BtEvent*)additionalData;

						BTL_VERIFY_ERR((btEvent != NULL), BT_STATUS_INTERNAL_ERROR, ("Null btEvent"));

						hciInitEventReceived = TRUE;

						if (btEvent->eType == BTEVENT_HCI_INITIALIZED)
						{
							BTL_LOG_INFO(("Radio Power-Up Abort Failed"));
							
							_btlData.radioOnAbortState = _BTL_RADIO_ON_ABORT_STATE_FAIL;
							
							if (chipMngrOnEventReceived == TRUE)
							{
								_btlData.radioOnState = _BTL_RADIO_ON_STATE_NOTIFY_BMG;
								keepProcessing = TRUE;
							}
						}
						else if (btEvent->eType == BTEVENT_HCI_FAILED)
						{
							BTL_LOG_ERROR(("Radio Power-Up FAILED"));

							 _btlData.radioOnState = _BTL_RADIO_ON_STATE_DONE;
							 _btlData.appEventStatus = BT_STATUS_HCI_INIT_ERR;

							 keepProcessing = TRUE;
						}
						else
						{
							BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Unexpected event (%s)", pME_Event(btEvent->eType)));
						}
						
						break;
                
                
					 case _BTL_RADIO_ON_EVENT_HCI_DEINITIALIZED:

						btEvent = (BtEvent*)additionalData;
                        
						BTL_VERIFY_ERR((btEvent != 0), BT_STATUS_INTERNAL_ERROR, ("Null btEvent"));
						BTL_VERIFY_ERR((btEvent->eType == BTEVENT_HCI_DEINITIALIZED), BT_STATUS_INTERNAL_ERROR,
										("Unexpected event (%s)", pME_Event(btEvent->eType)));

						hciDeinitEventReceived = TRUE;

						if (btEvent->eType == BTEVENT_HCI_DEINITIALIZED)
						{
							_btlData.radioOnAbortState = _BTL_RADIO_ON_ABORT_STATE_SUCCESS;
                            
							if (chipMngrAbortEventReceived == TRUE)
							{
								BTL_LOG_INFO(("Radio Power-Up Abort Completed Successfully"));

								_btlData.radioOnState = _BTL_RADIO_ON_STATE_DONE;
                                
								_btlData.appEventStatus = BT_STATUS_SUCCESS;
								keepProcessing = TRUE;
							}
						}
						else if (btEvent->eType == BTEVENT_HCI_FAILED)
						{
							BTL_LOG_ERROR(("Radio Power-Up FAILED"));
							
							_btlData.radioOnState = _BTL_RADIO_ON_STATE_DONE;
							_btlData.appEventStatus = BT_STATUS_HCI_INIT_ERR;

							keepProcessing = TRUE;
						}
						else
						{
							BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Unexpected event (%s)", pME_Event(btEvent->eType)));
						}

						break;

					case _BTL_RADIO_ON_EVENT_CHIP_MNGR_BT_ON_COMPLETED:

						chipMngrOnEventReceived = TRUE;

						if (hciInitEventReceived == TRUE)
						{
							_btlData.radioOnAbortState = _BTL_RADIO_ON_ABORT_STATE_FAIL;
                            
							BTL_LOG_INFO(("Radio Power-Up Abort Failed"));

							_btlData.radioOnState = _BTL_RADIO_ON_STATE_NOTIFY_BMG;
							keepProcessing = TRUE;
						}

						break;

					case _BTL_RADIO_ON_EVENT_CHIP_MNGR_BT_ON_ABORTED:

						chipMngrAbortEventReceived = TRUE;

						if (hciDeinitEventReceived == TRUE)
						{
							_btlData.radioOnAbortState = _BTL_RADIO_ON_ABORT_STATE_SUCCESS;
                            
							BTL_LOG_INFO(("Radio Power-Up Abort Completed Successfully"));

							_btlData.radioOnState = _BTL_RADIO_ON_STATE_DONE;
                                
							_btlData.appEventStatus = BT_STATUS_SUCCESS;
							keepProcessing = TRUE;
						}

						break;
				};
				break;
            
			case _BTL_RADIO_ON_STATE_DONE:
				
				_btlData.radioOnState = _BTL_RADIO_ON_STATE_NONE;

				if (_BTL_RADIO_ON_ABORT_STATE_NONE != _btlData.radioOnAbortState)
				{
					_btlData.stateControlState = _BTL_STATE_CONTROL_TYPE_NONE;

					switch (_btlData.radioOnAbortState)
					{
						case _BTL_RADIO_ON_ABORT_STATE_ABORTING:
						case _BTL_RADIO_ON_ABORT_STATE_FAIL:

							status = _BTL_RadioOffProcessor(_BTL_RADIO_OFF_EVENT_ABORT_ON_FAIL, NULL);       
							break;

						case _BTL_RADIO_ON_ABORT_STATE_SUCCESS:

							status = _BTL_RadioOffProcessor(_BTL_RADIO_OFF_EVENT_ABORT_ON_SUCCESS, NULL);       
							break;

						default:
							BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Invalid state (%d) of radioOnAbortState", _btlData.radioOnAbortState)); 
					}

					_btlData.radioOnAbortState = _BTL_RADIO_ON_ABORT_STATE_NONE;
                     
					return status;
				}

				if (_btlData.appEventStatus == BT_STATUS_SUCCESS)
				{
					_btlState = BTL_STATE_OPERATIONAL;

					BTL_BMG_GenerateHciInitEvent(BTEVENT_HCI_INITIALIZED);
				}
				else
				{
					_btlState = BTL_STATE_RADIO_INITIALIZATION_ERR;

					BTL_BMG_GenerateHciInitEvent(BTEVENT_HCI_FAILED);
				}

				_btlData.stateControlState = _BTL_STATE_CONTROL_TYPE_NONE;

				BTL_LOG_INFO(("Completed Radio On - Sending event to application"));
				
				appEvent.type = BTL_EVENT_RADIO_ON_COMPLETE;
				appEvent.status= _btlData.appEventStatus;

				/* Notify application that Radio On Completed */
				_btlData.appCallback(&appEvent);

				break;
				
			default:

				BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Invalid State (%d)", _btlData.radioOnState));
				
		};
	}

	BTL_FUNC_END();

	return status;
}

BtStatus _BTL_RadioOffProcessor(_BtlRadioOffEventType radioOffEvent, const void *additionalData)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	TiChipMngrStatus	chipMngrStatus = TI_CHIP_MNGR_STATUS_SUCCESS;
	BOOL			keepProcessing = TRUE;
	BOOL			haveAllLinksDisconnected = FALSE;
	BtlEvent			appEvent;
	BthalStatus		bthalStatus = BTHAL_STATUS_SUCCESS;
	BtEvent			*btEvent = 0;
	static BOOL		chipMngrEventReceived = FALSE;
	static BOOL		hciDeinitEventReceived = FALSE;
	BtlModuleCompletedData	*moduleCompletedData = 0;

	BTL_FUNC_START("_BTL_RadioOffProcessor");

	while (keepProcessing == TRUE)
	{
        BTL_LOG_DEBUG(("State: %s, Event: %s", BtlCommonRadioOffProcessorStateAsStr(_btlData.radioOffState), 
            BtlCommonBtRadioOffProcessorEventAsStr(radioOffEvent)));

		keepProcessing = FALSE;
		
		switch (_btlData.radioOffState)
		{
			case _BTL_RADIO_OFF_STATE_NONE:

				BTL_VERIFY_ERR(((radioOffEvent == _BTL_RADIO_OFF_EVENT_START) || 
					(radioOffEvent == _BTL_RADIO_OFF_EVENT_START_ABORT)), BT_STATUS_INTERNAL_ERROR, 
						("Invalid event (%d) when starting radio off processing", radioOffEvent));

				if (_btlData.radioOnState != _BTL_RADIO_ON_STATE_NONE)
				{
					_btlData.radioOffState = _BTL_RADIO_OFF_STATE_ABORT_START;
					keepProcessing = TRUE;                    
					break;
				}
                
				if (_BTL_RADIO_OFF_EVENT_START == radioOffEvent)
				{
					_btlData.radioOffAbortState = _BTL_RADIO_OFF_ABORT_STATE_NONE;
				}

				_btlData.stateControlState = _BTL_STATE_CONTROL_TYPE_RADIO_OFF;

				_btlState = BTL_STATE_DEINITIALIZING_RADIO;
				_btlData.appEventStatus = BT_STATUS_SUCCESS;
				_btlData.radioOffState = _BTL_RADIO_OFF_STATE_NOTIFY_BMG;
                

				chipMngrEventReceived = FALSE;
				hciDeinitEventReceived = FALSE;

				/* Send a the Init event to the stack task to execute the init code in that context */
				_btlData.stateControlState = _BTL_STATE_CONTROL_TYPE_RADIO_OFF;

				status = BT_STATUS_PENDING;
				
				bthalStatus = BTHAL_OS_SendEvent(BTHAL_OS_TASK_HANDLE_STACK, OS_EVENT_STACK_TASK_BTL_STATE_CONTROL);
				BTL_VERIFY_FATAL((BTHAL_STATUS_SUCCESS == bthalStatus), BT_STATUS_INTERNAL_ERROR,
						("Failed sending Init event 0x%x to Stack task!", OS_EVENT_STACK_TASK_BTL_STATE_CONTROL));

				break;

			case _BTL_RADIO_OFF_STATE_ABORT_START:

				_btlData.stateControlState = _BTL_STATE_CONTROL_TYPE_RADIO_ON;
                    
				status = _BTL_RadioOnProcessor(_BTL_RADIO_ON_EVENT_ABORT, NULL);

				BTL_VERIFY_FATAL((BT_STATUS_PENDING == status), BT_STATUS_INTERNAL_ERROR, 
					("Invalid returned value (%d) when aborting radio on proces", status));

				_btlData.radioOffState = _BTL_RADIO_OFF_STATE_ABORTING;
				_btlData.radioOffAbortState = _BTL_RADIO_OFF_ABORT_STATE_ABORTING;

				break;

			case _BTL_RADIO_OFF_STATE_NOTIFY_BMG:

				status = _BTL_NotifyBmg(BTL_MODULE_NOTIFICATION_RADIO_OFF);

				if (status == BT_STATUS_SUCCESS)
				{
					BTL_LOG_DEBUG(("BMG Is ready for Radio Off"));
					
					_btlData.radioOffState = _BTL_RADIO_OFF_STATE_NOTIFY_NON_BMG_MODULES;
					keepProcessing = TRUE;
				}
				else if (status == BT_STATUS_PENDING)
				{
					BTL_LOG_DEBUG(("Waiting for BMG to become ready for radio off"));
					
					_btlData.radioOffState = _BTL_RADIO_OFF_STATE_BMG_PREPARING_FOR_RADIO_OFF;
				}
				else
				{
					BTL_LOG_ERROR(("_BTL_NotifyBmg Failed (%s) - nevertheless continuning", pBT_Status(status)));
					
					_btlData.radioOffState = _BTL_RADIO_OFF_STATE_NOTIFY_NON_BMG_MODULES;
					keepProcessing = TRUE;
				}	

				break;
				
			case _BTL_RADIO_OFF_STATE_BMG_PREPARING_FOR_RADIO_OFF:

				switch (radioOffEvent)
				{
					case _BTL_RADIO_OFF_EVENT_MODULE_COMPLETED:
						
						moduleCompletedData = (BtlModuleCompletedData*)additionalData;
						
						BTL_VERIFY_FATAL((moduleCompletedData != 0), BT_STATUS_INTERNAL_ERROR,("No module completd data available"));
						BTL_VERIFY_FATAL((moduleCompletedData->moduleType == BTL_MODULE_TYPE_BMG), BT_STATUS_INTERNAL_ERROR,
											("Unexpected Module Notified Completion (%d)", moduleCompletedData->moduleType));

						status = _BTL_HandleModuleCompletedNotification((BtlModuleCompletedData*)additionalData);

						if (status == BT_STATUS_SUCCESS)
						{
							BTL_LOG_DEBUG(("BMG Is ready for Radio Off"));
							
							_btlData.radioOffState = _BTL_RADIO_OFF_STATE_NOTIFY_NON_BMG_MODULES;
							keepProcessing = TRUE;
						}

					break;

					case _BTL_RADIO_OFF_EVENT_DISCONNECTION:

						/* Do nothing, link disconnected during BMG preparations- ignore */
						BTL_LOG_DEBUG(("Link Disconnected while BMG preparing for radio off - ignoring"));

					break;
					
					default:

						BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Unexpected event ((%d)) while waiting for Modules to complete", radioOffEvent));
						
				};
				
				break;

			case _BTL_RADIO_OFF_STATE_NOTIFY_NON_BMG_MODULES:

				status = _BTL_NotifyNonBmgModules(BTL_MODULE_NOTIFICATION_RADIO_OFF);

				if (status == BT_STATUS_SUCCESS)
				{
					BTL_LOG_DEBUG(("Modules Are ready for Radio Off"));
					
					_btlData.radioOffState = _BTL_RADIO_OFF_STATE_DISCONNECT_LINKS;
					/*XXXX_btlData.radioOffState = _BTL_RADIO_OFF_STATE_SEND_EVENT_TO_SHUTDOWN_RADIO;*/
					keepProcessing = TRUE;
				}
				else if (status == BT_STATUS_PENDING)
				{
					BTL_LOG_DEBUG(("Waiting for Modules to become ready for radio off"));
					
					_btlData.radioOffState = _BTL_RADIO_OFF_STATE_NON_BMG_MODULES_PREPARING_FOR_RADIO_OFF;
				}
				else
				{
					BTL_LOG_ERROR(("_BTL_NotifyModules Failed (%s) - nevertheless continuning", pBT_Status(status)));
					
					_btlData.radioOffState = _BTL_RADIO_OFF_STATE_DISCONNECT_LINKS;
					/*XXXX_btlData.radioOffState = _BTL_RADIO_OFF_STATE_SEND_EVENT_TO_SHUTDOWN_RADIO;*/
					keepProcessing = TRUE;
				}	

			break;
				
			case _BTL_RADIO_OFF_STATE_NON_BMG_MODULES_PREPARING_FOR_RADIO_OFF:

				switch (radioOffEvent)
				{
					case _BTL_RADIO_OFF_EVENT_MODULE_COMPLETED:
						
						BTL_VERIFY_FATAL((additionalData != 0), BT_STATUS_INTERNAL_ERROR,("No module completd data available"));

						status = _BTL_HandleModuleCompletedNotification((BtlModuleCompletedData*)additionalData);

						if (status == BT_STATUS_SUCCESS)
						{
							BTL_LOG_DEBUG(("BTIPS Is ready for Radio Off (All modules completed preparations"));
							
							_btlData.radioOffState = _BTL_RADIO_OFF_STATE_DISCONNECT_LINKS;
							/* XXXX_btlData.radioOffState = _BTL_RADIO_OFF_STATE_SEND_EVENT_TO_SHUTDOWN_RADIO; */
							keepProcessing = TRUE;
						}
						
					break;

					case _BTL_RADIO_OFF_EVENT_DISCONNECTION:

						/* Do nothing, link disconnected during Modules preparations - ignore */
						BTL_LOG_DEBUG(("Link Disconnected while Modules preparing for radio off - ignoring"));

					break;
					
					default:

						BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Unexpected Radio Off Event (%d) while Modules preparing for Radio Off", radioOffEvent));

				};
				
			break;

			case _BTL_RADIO_OFF_STATE_DISCONNECT_LINKS:
				
				BTL_LOG_DEBUG(("Calling _BTL_DisconnectAllLinks"));
				
				status = _BTL_DisconnectAllLinks();

				if (status == BT_STATUS_SUCCESS)
				{
					_btlData.radioOffState = _BTL_RADIO_OFF_STATE_SEND_EVENT_TO_SHUTDOWN_RADIO;
					keepProcessing = TRUE;
				}
				else
				{
					BTL_VERIFY_FATAL((status == BT_STATUS_PENDING), BT_STATUS_INTERNAL_ERROR, 
										("Unexpected return value from _BTL_DisconnectAllLinks (%s)", pBT_Status(status)));

                    BTL_LOG_DEBUG(("Waiting for connection to disconnect"));
					_btlData.radioOffState = _BTL_RADIO_OFF_STATE_DISCONNECTING;
				}

			break;
							
			case _BTL_RADIO_OFF_STATE_DISCONNECTING:

				btEvent = (BtEvent*)additionalData;
				
				BTL_VERIFY_ERR((btEvent != 0), BT_STATUS_INTERNAL_ERROR, ("Null btEvent"));
				BTL_VERIFY_ERR((btEvent->eType == BTEVENT_LINK_DISCONNECT), BT_STATUS_INTERNAL_ERROR, 
								("Unexpected Event Type (%s)", pME_Event(btEvent->eType)));

				_BTL_LinkDisconnected(	btEvent->p.disconnect.remDev, &haveAllLinksDisconnected);

				if (haveAllLinksDisconnected == TRUE)
				{
					BTL_LOG_INFO(("All links disconnected - Shutting Off Radio"));
					
					_btlData.radioOffState = _BTL_RADIO_OFF_STATE_SEND_EVENT_TO_SHUTDOWN_RADIO;
					keepProcessing = TRUE;
				}
				else
				{
					BTL_LOG_DEBUG(("%d Connection Still Need to disconnect", _btlData.numOfDisconnectionsInProgress));
				}
				
			break;

			case _BTL_RADIO_OFF_STATE_SEND_EVENT_TO_SHUTDOWN_RADIO:

				_btlData.radioOffState = _BTL_RADIO_OFF_STATE_SHUTDOWN_RADIO;
				
				/* Send a the Init event to the stack task to execute the init code in that context */
				bthalStatus = BTHAL_OS_SendEvent(BTHAL_OS_TASK_HANDLE_STACK, OS_EVENT_STACK_TASK_BTL_STATE_CONTROL);
				BTL_VERIFY_FATAL((BTHAL_STATUS_SUCCESS == bthalStatus), BT_STATUS_INTERNAL_ERROR,
						("Failed sending Init event 0x%x to Stack task!", OS_EVENT_STACK_TASK_BTL_STATE_CONTROL));

			break;
								
			case _BTL_RADIO_OFF_STATE_SHUTDOWN_RADIO:

				BTL_LOG_INFO(("Calling TI_CHIP_MNGR_BTOff"));

				/* Starting to shutdown radio. We may receive the HCI de-init event during the call to ME_RadioShutdown */
				_btlData.radioOffState = _BTL_RADIO_OFF_CALLING_RADIO_SHUT_DOWN;
				
				chipMngrStatus = TI_CHIP_MNGR_BTOff();

				if (chipMngrStatus == TI_CHIP_MNGR_STATUS_SUCCESS) 
				{
					BTL_LOG_INFO(("Radio De-Initialized"));

					_btlData.radioOffState = _BTL_RADIO_OFF_STATE_DONE;
					keepProcessing = TRUE;
				}
				else if (chipMngrStatus == TI_CHIP_MNGR_STATUS_PENDING)
				{					
					BTL_LOG_DEBUG(("Waiting for Radio to initialize"));
					_btlData.radioOffState = _BTL_RADIO_OFF_WAITING_FOR_CHIP_MNGR_AND_ME_HCI_DEINIT_EVENT;
				}
				else
				{
					BTL_FATAL_NORET(("Unexpected return value from TI_CHIP_MNGR_BTOff (%d)", chipMngrStatus));

					_btlData.appEventStatus = status;
					_btlData.radioOffState = _BTL_RADIO_OFF_STATE_DONE;
					keepProcessing = TRUE;
				}

			break;

			case _BTL_RADIO_OFF_CALLING_RADIO_SHUT_DOWN:

				btEvent = (BtEvent*)additionalData;
				
				BTL_VERIFY_ERR((btEvent != 0), BT_STATUS_INTERNAL_ERROR, ("Null btEvent"));
				BTL_VERIFY_ERR((btEvent->eType == BTEVENT_HCI_DEINITIALIZED), BT_STATUS_INTERNAL_ERROR,
								("Unexpected event (%s)", pME_Event(btEvent->eType)));

				BTL_LOG_DEBUG(("De-Init event during the call to ME_RadioShutdown, Doing Nothing"));

				/* Do nothing, let TI_CHIP_MNGR_BTOff return with BT_STATUS_SUCCESS and handle there */

			break;

			case _BTL_RADIO_OFF_WAITING_FOR_CHIP_MNGR_AND_ME_HCI_DEINIT_EVENT:

				switch (radioOffEvent)
				{
					case _BTL_RADIO_OFF_EVENT_HCI_DEINITIALIZED:

						btEvent = (BtEvent*)additionalData;
						
						BTL_VERIFY_ERR((btEvent != 0), BT_STATUS_INTERNAL_ERROR, ("Null btEvent"));
						BTL_VERIFY_ERR((btEvent->eType == BTEVENT_HCI_DEINITIALIZED), BT_STATUS_INTERNAL_ERROR,
										("Unexpected event (%s)", pME_Event(btEvent->eType)));

						hciDeinitEventReceived = TRUE;

						BTL_LOG_INFO(("Radio Power-Down Completed Successfully"));
							
						if (chipMngrEventReceived == TRUE)
						{
							_btlData.radioOffState = _BTL_RADIO_OFF_STATE_DONE;
							keepProcessing = TRUE;
						}
						
						break;

					case _BTL_RADIO_OFF_EVENT_CHIP_MNGR_BT_OFF_COMPLETED:

						chipMngrEventReceived = TRUE;

						if (hciDeinitEventReceived == TRUE)
						{
							_btlData.radioOffState = _BTL_RADIO_OFF_STATE_DONE;
							keepProcessing = TRUE;
						}

						break;

					default:

						BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Unexpected radio Off event (%d)",radioOffEvent));
						
				};

				break;
				
            
	            case _BTL_RADIO_OFF_STATE_ABORTING:

				switch (radioOffEvent)
				{
					case _BTL_RADIO_OFF_EVENT_ABORT_ON_SUCCESS:
	                        
						_btlData.radioOffState = _BTL_RADIO_OFF_STATE_DONE;

						_btlData.appEventStatus = BT_STATUS_SUCCESS;
						keepProcessing = TRUE;
	                        
						break;

					case _BTL_RADIO_OFF_EVENT_ABORT_ON_FAIL:
	                        
						/* Note: _btlData.appEventStatus will be set by _BTL_RadioOffProcessor() process */

						_btlData.radioOffState = _BTL_RADIO_OFF_STATE_NONE;
						radioOffEvent = _BTL_RADIO_OFF_EVENT_START_ABORT;
						keepProcessing = TRUE;
						break;

					default:
						BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Invalid event (%d) when processing RadioOn Abort", radioOffEvent)); 
				}

				break;

				
			case _BTL_RADIO_OFF_STATE_DONE:
				
				_btlData.radioOffState = _BTL_RADIO_OFF_STATE_NONE;
				_btlState = BTL_STATE_RADIO_OFF;

				_btlData.stateControlState = _BTL_STATE_CONTROL_TYPE_NONE;
                
				if (_BTL_RADIO_OFF_ABORT_STATE_ABORTING == _btlData.radioOffAbortState)
				{
					_btlData.radioOffAbortState = _BTL_RADIO_OFF_ABORT_STATE_NONE;
				}
				else
				{
					BTL_BMG_GenerateHciInitEvent(BTEVENT_HCI_DEINITIALIZED);
				}

				BTL_LOG_INFO(("Completed Radio Off - Sending event to application"));

				appEvent.type = BTL_EVENT_RADIO_OFF_COMPLETE;
				appEvent.status = _btlData.appEventStatus;

				/* Notify application that Radio Off Completed */
				_btlData.appCallback(&appEvent);
				
				break;
				
			default:

				BTL_ERR(BT_STATUS_INTERNAL_ERROR, (""));
				
		};
	}

	BTL_FUNC_END();

	return status;
}

BtStatus BTL_RegisterApp(BtlAppHandle **appHandle, const BtlUtf8* appName)
{
	BtStatus		status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_RegisterApp");
	
	BTL_VERIFY_ERR((0 != appHandle), BT_STATUS_INVALID_PARM, ("Null appHandle argument"));
	
	status = BTL_POOL_Allocate(&_btlData.appHandlesPool, (void **)appHandle);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed Allocating App Handle"));
	
	status = BTL_APP_HANDLE_Create(*appHandle, appName);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("App Handle creation failed"));
	
	InsertTailList(&_btlData.appHandlesList, &((*appHandle)->node));

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_DeRegisterApp(BtlAppHandle **appHandle)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_DeRegisterApp");
	
	BTL_VERIFY_ERR((0 != appHandle), BT_STATUS_INVALID_PARM, ("Null appHandle argument"));

	/* Remove the App Handle from the list of all App Handles */
	RemoveEntryList(&((*appHandle)->node));
	
	status = BTL_POOL_Free(&_btlData.appHandlesPool, (void **)appHandle);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed Freeing App Handle"));

	*appHandle = 0;
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

/********************************/
/* Internal Functions (btl_commoni.h) */
/********************************/

BtStatus BTL_HandleModuleInstanceCreation(	BtlAppHandle 		*appHandle, 
													BtlModuleType		moduleType, 
													BtlContext 		*context)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	BtlAppHandle	*actualAppHandle = appHandle;

	BTL_FUNC_START("BTL_HandleModuleInstanceCreation");

	BTL_VERIFY_FATAL((0 != context), BT_STATUS_INTERNAL_ERROR, ("Null context argument"));
	
	if (0 == appHandle)
	{
		status = BTL_GetDefaultAppHandle((const BtlAppHandle **)(&actualAppHandle));
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed Getting Default App Handle"));
	}
	
	status = BTL_CreateContext(context, moduleType, actualAppHandle);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Failed Creating base part of Context"));
	
	/* Allow easy finding of all contexts of the same applicaiton */
	status = BTL_APP_HANDLE_AddContext(actualAppHandle, context);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed Adding Context to its App Handle"));

	BTL_FUNC_END();

	return status;
}

BtStatus BTL_HandleModuleInstanceDestruction(	BtlContext *context)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	BtlAppHandle	*appHandle = 0;

	BTL_FUNC_START("BTL_HandleModuleInstanceDestruction");

	BTL_VERIFY_FATAL((0 != context), BT_STATUS_INTERNAL_ERROR, ("Null context argument"));
	BTL_VERIFY_FATAL((0 != context->appHandle), BT_STATUS_INTERNAL_ERROR, ("Null context->appHandle"));

	/* obtain the context's app handle */
	appHandle = context->appHandle;
	
	/* Allow easy finding of all contexts of the same applicaiton */
	status = BTL_APP_HANDLE_RemoveContext(appHandle, context);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed Adding Context to its App Handle"));

	status = BTL_DestroyContext(context);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed Destroying Context"));

	if (appHandle == _btlData.defaultAppHandle)
	{
		status = BTL_ReleaseDefaultAppHandle();
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));
	}
		
	BTL_FUNC_END();

	return status;
}

BtStatus BTL_CreateContext(	BtlContext 		*context, 
								BtlModuleType		moduleType,
								BtlAppHandle 		*appHandle)
{
	BtStatus	status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_CreateContext");
	
	BTL_VERIFY_FATAL((0 != context), BT_STATUS_INTERNAL_ERROR, ("Null context"));
	BTL_VERIFY_FATAL((0 != appHandle), BT_STATUS_INTERNAL_ERROR, ("Null appHandle"));

	InitializeListEntry(&context->node);
	context->moduleType = moduleType;
	context->appHandle = appHandle;
	
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_DestroyContext(BtlContext *context)
{
	BtStatus	status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_DestroyContext");
	
	BTL_VERIFY_FATAL((0 != context), BT_STATUS_INTERNAL_ERROR, ("Null context"));
	BTL_VERIFY_FATAL((TRUE == IsEntryAvailable(&(context->node))), BT_STATUS_INTERNAL_ERROR,
						("Context is destroyed while still on a linked list"));

	InitializeListEntry(&context->node);
	context->appHandle = 0;
	
	BTL_FUNC_END();
	
	return status;
}

void _BTL_HALInitCallback(const BthalEvent	*event)
{
	BTL_FUNC_START("_BTL_HALInitCallback");

	UNUSED_PARAMETER(event);

	BTL_LOG_FATAL(("Shouldn't get here - Not Supported"));				\
	BTL_ASSERT(0);	
}	

void _BTL_GlobalHandler(const BtEvent *event)
{	
	BTL_FUNC_START("_BTL_GlobalHandler");

	BTL_LOG_DEBUG(("Event: %s", pME_Event(event->eType)));
	
	switch (event->eType)
	{
		case BTEVENT_HCI_DEINITIALIZED:

			if (_btlState == BTL_STATE_DEINITIALIZING_RADIO)
			{
				_BTL_RadioOffProcessor(_BTL_RADIO_OFF_EVENT_HCI_DEINITIALIZED, event);
			}
			else if (_btlState == BTL_STATE_INITIALIZING_RADIO)
			{
				_BTL_RadioOnProcessor(_BTL_RADIO_ON_EVENT_HCI_DEINITIALIZED, event);
			}
			else
			{
				BTL_LOG_INFO(("HCI Deinitialized while BTL State is %d", _btlState));
			}
			
			break;
			
		case BTEVENT_HCI_INITIALIZED:

			if ((_btlState == BTL_STATE_INITIALIZING_RADIO) || (_btlState == BTL_STATE_RADIO_INITIALIZATION_ERR))
			{
				_BTL_RadioOnProcessor(_BTL_RADIO_ON_EVENT_HCI_INITIALIZATION, event);
			}
			else
			{
				BTL_LOG_INFO(("HCI Initialized while BTL State is %d", _btlState));
			}
			
			break;
			
		case BTEVENT_HCI_FAILED:

			if ((_btlState == BTL_STATE_INITIALIZING_RADIO) || (_btlState == BTL_STATE_RADIO_INITIALIZATION_ERR))
			{
				_BTL_RadioOnProcessor(	_BTL_RADIO_ON_EVENT_HCI_INITIALIZATION, event);
			}
			else
			{
				BTL_LOG_INFO(("HCI Initialized while BTL State is %d", _btlState));
			}

			break;
		
		case BTEVENT_HCI_FATAL_ERROR:			
		case BTEVENT_HCI_INIT_ERROR:

			break;

		case BTEVENT_LINK_DISCONNECT:

			if (_btlState == BTL_STATE_DEINITIALIZING_RADIO)
			{
				_BTL_RadioOffProcessor(_BTL_RADIO_OFF_EVENT_DISCONNECTION, event);
			}

			break;
	}

	BTL_FUNC_END();	
}

BtStatus BTL_GetDefaultAppHandle(const BtlAppHandle **appHandle)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTL_GetDefaultAppHandle");
	
	if (0 == _btlData.defaultAppHandle)
	{
		status = BTL_RegisterApp(&_btlData.defaultAppHandle,(BtlUtf8*)BTL_DEFAULT_APP_NAME);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed Allocating Default App Handle"));
	}

	*appHandle = _btlData.defaultAppHandle;
	++_btlData.defaultAppHandleRefCount;

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_ReleaseDefaultAppHandle(void)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTL_ReleaseDefaultAppHandle");
	
	--_btlData.defaultAppHandleRefCount;
	
	if (0 == _btlData.defaultAppHandleRefCount)
	{
		status = BTL_DeRegisterApp(&_btlData.defaultAppHandle);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Failed DeRegistering Default App Handle"));			
	}

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_VerifySecurityLevel(BtSecurityLevel securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTL_VerifySecurityLevel");
	
	if (securityLevel & (BSL_ENCRYPTION_IN))
	{
		BTL_VERIFY_ERR(((securityLevel & BSL_AUTHENTICATION_IN) != 0), BT_STATUS_INVALID_PARM, ("Authentication in must be ON"));
    }

	if (securityLevel & (BSL_ENCRYPTION_OUT))
	{
		BTL_VERIFY_ERR(((securityLevel & BSL_AUTHENTICATION_OUT) != 0), BT_STATUS_INVALID_PARM, ("Authentication out must be ON"));
    }

	BTL_FUNC_END();

	return (status);
}

void BTL_RegisterForCommonNotifications(BtlModuleType moduleType, BtlCommonNotificationsCallback callback)
{
	BTL_FUNC_START("BTL_RegisterForCommonNotifications");

	BTL_VERIFY_ERR_NO_RETVAR((moduleType < BTL_MODULE_TYPE_NUM_OF_MODULES),
					("Invalid Module Type (%d)", moduleType));
	BTL_VERIFY_ERR_NO_RETVAR((_btlData.moduleNotificationsCbs[(U32)moduleType] == 0),
					("%d Is already registered for Common Notifications", moduleType));
	
	_btlData.moduleNotificationsCbs[(U32)moduleType] = callback;

	BTL_FUNC_END();
}

void BTL_DeRegisterFromCommonNotifications(BtlModuleType moduleType)
{
	BTL_FUNC_START("BTL_DeRegisterFromCommonNotifications");

	BTL_VERIFY_ERR_NO_RETVAR((moduleType < BTL_MODULE_TYPE_NUM_OF_MODULES),
					("Invalid Module Type (%d)", moduleType));
	BTL_VERIFY_ERR_NO_RETVAR((_btlData.moduleNotificationsCbs[(U32)moduleType] != 0),
					("%d Is NOT registered for Common Notifications", moduleType));
	
	_btlData.moduleNotificationsCbs[(U32)moduleType] = 0;

	BTL_FUNC_END();
}

void BTL_ModuleCompleted(BtlModuleType moduleType, const BtlModuleNotificationCompletionEvent *event)
{
	BtlModuleCompletedData	moduleCompletedData;
	
	BTL_FUNC_START("BTL_ModuleCompleted");

	BTL_LOG_INFO(("%d Completed Processing Event %d, Status: %s", moduleType, event->type, pBT_Status(event->status)));

	moduleCompletedData.moduleType = moduleType;
	moduleCompletedData.event = event;
	
	switch (_btlData.stateControlState)
	{
			
		case _BTL_STATE_CONTROL_TYPE_RADIO_ON:

			_BTL_RadioOnProcessor(_BTL_RADIO_ON_EVENT_MODULE_COMPLETED, &moduleCompletedData);

			break;
			
		case _BTL_STATE_CONTROL_TYPE_RADIO_OFF:

			_BTL_RadioOffProcessor(_BTL_RADIO_OFF_EVENT_MODULE_COMPLETED, &moduleCompletedData);

			break;
			
		case _BTL_STATE_CONTROL_TYPE_INIT:
		case _BTL_STATE_CONTROL_TYPE_DEINIT:
		default:

			BTL_FATAL_NO_RETVAR(("Invalid State Control State (%d)", _btlData.stateControlState));
			
	};

	BTL_FUNC_END();
}

void BTL_CHIP_MNGR_Callback(TiChipMngrBtNotificationType notificationType)
{
	BTL_FUNC_START("BTL_CHIP_MNGR_Callback");
	
	BTL_LOG_INFO(("notificationType: %s", BtlChipMngrCallbackEventAsStr(notificationType)));
	
	switch (notificationType)
	{
		case TI_CHIP_MNGR_BT_NOTIFICATION_BT_ON_COMPLETE:
			
			_BTL_RadioOnProcessor(_BTL_RADIO_ON_EVENT_CHIP_MNGR_BT_ON_COMPLETED, NULL);

			break;

		case TI_CHIP_MNGR_BT_NOTIFICATION_BT_ON_ABORT:
			
			_BTL_RadioOnProcessor(_BTL_RADIO_ON_EVENT_CHIP_MNGR_BT_ON_ABORTED, NULL);

			break;


		case TI_CHIP_MNGR_BT_NOTIFICATION_BT_OFF_COMPLETE:

			_BTL_RadioOffProcessor(_BTL_RADIO_OFF_EVENT_CHIP_MNGR_BT_OFF_COMPLETED, NULL);
			
			break;

		default:

			BTL_ERR_NO_RETVAR(("Unexpected Chip Mngr Notification Type (%d)", notificationType));
	};

	BTL_FUNC_END();
}


const char *BtlCommonRadioOnProcessorStateAsStr(_BtlRadioOnStateType state)
{
	switch (state)
	{
		case _BTL_RADIO_ON_STATE_NONE:						                		return "NONE";
		case _BTL_RADIO_ON_STATE_POWER_UP_CHIP:					            	return "POWER_UP_CHIP";
		case _BTL_RADIO_ON_WAITING_FOR_CHIP_MNGR_AND_ME_HCI_INIT_EVENT:	return "WAITING_FOR_CHIP_MNGR_AND_ME_HCI_INIT";
		case _BTL_RADIO_ON_STATE_NOTIFY_BMG:		                        			return "NOTIFY_BMG";
		case _BTL_RADIO_ON_STATE_BMG_PREPARING_FOR_RADIO_ON:			    	return "BMG_PREPARING_FOR_RADIO_ON";
		case _BTL_RADIO_ON_STATE_NOTIFY_NON_BMG_MODULES:				    	return "NOTIFY_NON_BMG_MODULES";
		case _BTL_RADIO_ON_STATE_NON_BMG_MODULES_PREPARING_FOR_RADIO_ON:    return "NON_BMG_MODULES_PREPARING_FOR_RADIO_ON";
		case _BTL_RADIO_ON_STATE_ABORTING:						            			return "ABORTING";
		case _BTL_RADIO_ON_STATE_DONE:						                		return "DONE";
		default: 														    		return "UNKNOWN";
	};
}


const char *BtlCommonBtRadioOnProcessorEventAsStr(_BtlRadioOnEventType event)
{
	switch (event)
	{
		case _BTL_RADIO_ON_EVENT_START:					    return "START";
		case _BTL_RADIO_ON_EVENT_STACK_SIGNAL:	            return "STACK_SIGNA";
		case _BTL_RADIO_ON_EVENT_HCI_INITIALIZATION:	    return "HCI_INITIALIZATION";
		case _BTL_RADIO_ON_EVENT_HCI_DEINITIALIZED:	        return "HCI_DEINITIALIZED";
		case _BTL_RADIO_ON_EVENT_MODULE_COMPLETED:	        return "MODULE_COMPLETED";
		case _BTL_RADIO_ON_EVENT_CHIP_MNGR_BT_ON_COMPLETED:	return "CHIP_MNGR_BT_ON_COMPLETED";
		case _BTL_RADIO_ON_EVENT_CHIP_MNGR_BT_ON_ABORTED:	return "CHIP_MNGR_BT_ON_ABORTED";
		case _BTL_RADIO_ON_EVENT_ABORT:	                    return "ABORT";
		default:										    return "UNKNOWN";
	};
}
    
  
const char *BtlCommonRadioOffProcessorStateAsStr(_BtlRadioOffStateType state)
{
	switch (state)
	{
		case _BTL_RADIO_OFF_STATE_NONE:						                return "NONE";
		case _BTL_RADIO_OFF_STATE_ABORT_START:						        return "ABORT_START";
		case _BTL_RADIO_OFF_STATE_NOTIFY_BMG:					            return "NOTIFY_BMG";
		case _BTL_RADIO_OFF_STATE_BMG_PREPARING_FOR_RADIO_OFF:	            return "BMG_PREPARING_FOR_RADIO_OFF";
		case _BTL_RADIO_OFF_STATE_NOTIFY_NON_BMG_MODULES:		            return "NOTIFY_NON_BMG_MODULES";
		case _BTL_RADIO_OFF_STATE_NON_BMG_MODULES_PREPARING_FOR_RADIO_OFF:	return "NON_BMG_MODULES_PREPARING_FOR_RADIO_OFF";
		case _BTL_RADIO_OFF_STATE_DISCONNECT_LINKS:				            return "DISCONNECT_LINKS";
		case _BTL_RADIO_OFF_STATE_DISCONNECTING:                            return "DISCONNECTING";
		case _BTL_RADIO_OFF_STATE_SEND_EVENT_TO_SHUTDOWN_RADIO:				return "SEND_EVENT_TO_SHUTDOWN_RADIO";
		case _BTL_RADIO_OFF_STATE_SHUTDOWN_RADIO:						    return "SHUTDOWN_RADIO";
		case _BTL_RADIO_OFF_CALLING_RADIO_SHUT_DOWN:						return "CALLING_RADIO_SHUT_DOWN";
		case _BTL_RADIO_OFF_WAITING_FOR_RADIO_SHUTDOWN:						return "WAITING_FOR_RADIO_SHUTDOWN";
		case _BTL_RADIO_OFF_WAITING_FOR_CHIP_MNGR_AND_ME_HCI_DEINIT_EVENT:	return "WAITING_FOR_CHIP_MNGR_AND_ME_HCI_DEINIT_EVENT";
		case _BTL_RADIO_OFF_STATE_ABORTING:						            return "ABORTING";
		case _BTL_RADIO_OFF_STATE_DONE:						                return "DONE";
		default: 														    return "UNKNOWN";
	};
}


const char *BtlCommonBtRadioOffProcessorEventAsStr(_BtlRadioOffEventType event)
{
	switch (event)
	{
		case _BTL_RADIO_OFF_EVENT_START:					    return "START";
		case _BTL_RADIO_OFF_EVENT_START_ABORT:				    return "START_ABORT";
		case _BTL_RADIO_OFF_EVENT_STACK_SIGNAL:	                return "STACK_SIGNAL";
		case _BTL_RADIO_OFF_EVENT_DISCONNECTION:	            return "DISCONNECTION";
		case _BTL_RADIO_OFF_EVENT_HCI_DEINITIALIZED:	        return "HCI_DEINITIALIZED";
		case _BTL_RADIO_OFF_EVENT_MODULE_COMPLETED:	            return "MODULE_COMPLETED";
		case _BTL_RADIO_OFF_EVENT_CHIP_MNGR_BT_OFF_COMPLETED:	return "BT_OFF_COMPLETED";
		case _BTL_RADIO_OFF_EVENT_ABORT_ON_SUCCESS:	            return "ABORT_ON_SUCCESS";
		case _BTL_RADIO_OFF_EVENT_ABORT_ON_FAIL:	            return "ABORT_ON_FAIL";
		default:										        return "UNKNOWN";
	};
} 


const char *BtlChipMngrCallbackEventAsStr(TiChipMngrBtNotificationType event)
{
	switch (event)
	{
		case TI_CHIP_MNGR_BT_NOTIFICATION_BT_ON_COMPLETE:	    return "BT_ON_COMPLETE";
		case TI_CHIP_MNGR_BT_NOTIFICATION_BT_OFF_COMPLETE:      return "BT_OFF_COMPLETE";
		default:										        return "UNKNOWN";
	};
} 
