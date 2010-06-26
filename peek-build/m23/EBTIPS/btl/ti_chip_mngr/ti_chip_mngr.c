#include "ti_chip_mngr.h"
#include "bthal_btdrv.h"
#include "bthal_os.h"
#include "radiomgr.h"
#include "btl_defs.h"
#include "debug.h"
#include "btl_debug.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BTL_COMMON);

typedef enum
{
	TI_CHIP_MNGR_INIT_STATE_NONE,
	TI_CHIP_MNGR_INIT_STATE_INITIALIZED
} TiChipMngrInitState;

typedef enum
{
	TI_CHIP_MNGR_BT_ON_STATE_NONE,
	TI_CHIP_MNGR_BT_ON_STATE_PENDING,
	TI_CHIP_MNGR_BT_ON_STATE_TURN_ON_TRANSPORT,
	TI_CHIP_MNGR_BT_ON_STATE_WAITING_FOR_TRANSPORT,
	TI_CHIP_MNGR_BT_ON_STATE_WAITING_FOR_TRANSPORT_ABORT,
	TI_CHIP_MNGR_BT_ON_STATE_DONE_ABORT,
	TI_CHIP_MNGR_BT_ON_STATE_DONE
} TiChipMngrBtOnState;

typedef enum
{
	TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_START,
	TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_TRANSPORT_READY,
	TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_TRANSPORT_SHUTDOWN,
	TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_ABORT
} TiChipMngrChipOffToBtOnEvent;

typedef enum
{
	TI_CHIP_MNGR_BT_OFF_STATE_NONE,
	TI_CHIP_MNGR_BT_OFF_STATE_PENDING,
	TI_CHIP_MNGR_BT_OFF_STATE_SHUT_DOWN_TRANSPORT,
	TI_CHIP_MNGR_BT_OFF_STATE_WAITING_FOR_TRANSPORT,
	TI_CHIP_MNGR_BT_OFF_STATE_DONE
} TiChipMngrBtOffState;

typedef enum
{
	TI_CHIP_MNGR_BT_ON_TO_CHIP_OFF_EVENT_START,
	TI_CHIP_MNGR_BT_ON_TO_CHIP_OFF_EVENT_TRANSPORT_SHUTDOWN
} TiChipMngrBTOnToChipOffEvent;

typedef enum
{
	TI_CHIP_MNGR_FM_ON_STATE_NONE,
	TI_CHIP_MNGR_FM_ON_STATE_PENDING,
	TI_CHIP_MNGR_FM_ON_STATE_TURN_ON_TRANSPORT,
	TI_CHIP_MNGR_FM_ON_STATE_WAITING_FOR_TRANSPORT,
	TI_CHIP_MNGR_FM_ON_STATE_DONE
} TiChipMngrFmOnState;

typedef enum
{
	TI_CHIP_MNGR_CHIP_OFF_TO_FM_ON_EVENT_START,
	TI_CHIP_MNGR_CHIP_OFF_TO_FM_ON_EVENT_TRANSPORT_READY
} TiChipMngrChipOffToFmOnEvent;

typedef enum
{
	TI_CHIP_MNGR_FM_OFF_STATE_NONE,
	TI_CHIP_MNGR_FM_OFF_STATE_PENDING,
	TI_CHIP_MNGR_FM_OFF_STATE_SHUT_DOWN_TRANSPORT,
	TI_CHIP_MNGR_FM_OFF_STATE_WAITING_FOR_TRANSPORT,
	TI_CHIP_MNGR_FM_OFF_STATE_DONE
} TiChipMngrFmOffState;

typedef enum
{
	TI_CHIP_MNGR_FM_ON_TO_CHIP_OFF_EVENT_START,
	TI_CHIP_MNGR_FM_ON_TO_CHIP_OFF_EVENT_TRANSPORT_SHUTDOWN
} TiChipMngrFMOnToChipOffEvent;

typedef struct
{
	TiChipMngrState		state;

	BthalOsSemaphoreHandle	semHandle;

	TiChipMngrBtOnState	btOnState;
	TiChipMngrBtOffState	btOffState;
	TiChipMngrFmOnState	fmOnState;
	TiChipMngrFmOffState	fmOffState;

	BTHAL_BOOL		btOnAsynchronous;
	BTHAL_BOOL		btOffAsynchronous;
	BTHAL_BOOL		fmOnAsynchronous;
	BTHAL_BOOL		fmOffAsynchronous;

	TiChipMngrPowerNotificationsCb	powerNotificationsCb;
	TiChipMngrStateNotificationsCb	stateNotificationsCb;
	TiChipMngrBtNotificationsCb		btNotificationsCb;
	TiChipMngrFmNotificationsCb		fmNotificationsCb;
} TiChipMngrData;

TiChipMngrData	tiChipMngrData;

TiChipMngrInitState	tiChipMngrInitState = TI_CHIP_MNGR_INIT_STATE_NONE;

void TiChipMngrInitData(void);

static void TiChipMngrSetState(TiChipMngrState state);

static TiChipMngrStatus TiChipMngrInternalBTOn(void);
static TiChipMngrStatus TiChipMngrInternalBTOff(void);
static TiChipMngrStatus TiChipMngrInternalFMOn(void);
static TiChipMngrStatus TiChipMngrInternalFMOff(void);

static void TiChipMngrSendPowerNotification(TiChipMngrPowerNotificationType notification);

static const char *TiChipMngrBtOnStateAsStr(TiChipMngrBtOnState state);
static const char *TiChipMngrChipOffToBtOnEventAsStr (TiChipMngrChipOffToBtOnEvent event);
static const char *TiChipMngrBtOffStateAsStr(TiChipMngrBtOffState state);
static const char *TiChipMngrBtOnToChipOffEventAsStr (TiChipMngrBTOnToChipOffEvent event);
static const char *TiChipMngrChipOffToFmOnEventAsStr (TiChipMngrChipOffToFmOnEvent event);
static const char *TiChipMngrFmOffStateAsStr(TiChipMngrFmOffState state);
static const char *TiChipMngrFmOnToChipOffEventAsStr (TiChipMngrFMOnToChipOffEvent event);
static const char *TiChipMngrRadioCallbackStateAsStr(TiChipMngrRadioNotificationType notificationType);

static void TiChipMngrStartPendingFmOperation(void);
static void TiChipMngrStartPendingBtOperation(void);

static void TiChipMngrCompleteBtOn(TiChipMngrState newChipMngrState);
static void TiChipMngrCompleteBtOff(TiChipMngrState newChipMngrState);
static void TiChipMngrCompleteFmOn(TiChipMngrState newChipMngrState);
static void TiChipMngrCompleteFmOff(TiChipMngrState newChipMngrState);

static BTHAL_BOOL TiChipMngrIsBtOnInProgress(void);
static BTHAL_BOOL TiChipMngrIsBtOffInProgress(void);
static BTHAL_BOOL TiChipMngrIsFmOnInProgress(void);
static BTHAL_BOOL TiChipMngrIsFmOffInProgress(void);

static TiChipMngrStatus TiChipMngrChipOffToBTOnProcessor(TiChipMngrChipOffToBtOnEvent event);

#define TI_CHIP_MNGR_LOCK()																				\
	BTL_VERIFY_FATAL(	(BTHAL_OS_LockSemaphore(tiChipMngrData.semHandle, 0) == BTHAL_STATUS_SUCCESS),	\
						TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Failed locking Chip Mngr semaphore"))

#define TI_CHIP_MNGR_UNLOCK()																				\
	BTL_VERIFY_FATAL(	(BTHAL_OS_UnlockSemaphore(tiChipMngrData.semHandle) == BTHAL_STATUS_SUCCESS),	\
						TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Failed Unlocking Chip Mngr semaphore"))

/* [@ToDo] [Udi]:
	1. Initialize BTHAL_OS_Init (handle 2 initializations - from BTIPS + here, or assume that BTIPS always exists in this case)
	2. Define a new semaphore for the chip mngr
	3. Use the semaphore - beware of deadlocks between the FM task and the BTS / application tasks due to the existance of 2 semaphores
*/

TiChipMngrStatus TI_CHIP_MNGR_Init(void)
{
	TiChipMngrStatus	status = TI_CHIP_MNGR_STATUS_SUCCESS;
	BthalStatus		bthalStatus = BTHAL_STATUS_SUCCESS;

	BTL_FUNC_START("TI_CHIP_MNGR_Init");

	if (tiChipMngrInitState == TI_CHIP_MNGR_INIT_STATE_INITIALIZED)
	{
		return TI_CHIP_MNGR_STATUS_SUCCESS;
	}

	TiChipMngrInitData();

	bthalStatus = BTHAL_OS_CreateSemaphore("CHIP_MNGR_SEM", &tiChipMngrData.semHandle);
	BTL_VERIFY_FATAL((BTHAL_STATUS_SUCCESS == bthalStatus), TI_CHIP_MNGR_STATUS_FAILED,
            ("Failed creating Chip Mngr Semaphore (%d)!", bthalStatus));

	tiChipMngrInitState = TI_CHIP_MNGR_INIT_STATE_INITIALIZED;

/*
	Dummy calls to debug functions that are used only in non EBTIPS RELEASE builds.
	The dummy calls removes compiler warnings
*/
#ifdef EBTIPS_RELEASE
		TiChipMngrBtOnStateAsStr(TI_CHIP_MNGR_BT_ON_STATE_NONE);
		TiChipMngrChipOffToBtOnEventAsStr(TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_START);
		TiChipMngrBtOffStateAsStr(TI_CHIP_MNGR_BT_OFF_STATE_NONE);
		TiChipMngrBtOnToChipOffEventAsStr(TI_CHIP_MNGR_BT_ON_TO_CHIP_OFF_EVENT_START);
		TiChipMngrChipOffToFmOnEventAsStr(TI_CHIP_MNGR_CHIP_OFF_TO_FM_ON_EVENT_START);
		TiChipMngrFmOffStateAsStr(TI_CHIP_MNGR_FM_OFF_STATE_NONE);
		TiChipMngrFmOnToChipOffEventAsStr(TI_CHIP_MNGR_FM_ON_TO_CHIP_OFF_EVENT_START);
		TiChipMngrRadioCallbackStateAsStr(HCI_INIT_STATUS);
#endif

	BTL_FUNC_END();

	return status;
}

TiChipMngrStatus TI_CHIP_MNGR_Deinit(void)
{
	BthalStatus		bthalStatus = BTHAL_STATUS_SUCCESS;

	BTL_FUNC_START("TI_CHIP_MNGR_Deinit");

	bthalStatus = BTHAL_OS_DestroySemaphore(tiChipMngrData.semHandle);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed destroying Chip Mngr Semaphore (%d)!", bthalStatus));

	tiChipMngrInitState = TI_CHIP_MNGR_INIT_STATE_NONE;

	BTL_FUNC_END();

	return TI_CHIP_MNGR_STATUS_SUCCESS;
}

TiChipMngrStatus TI_CHIP_MNGR_BTOn(void)
{
	TiChipMngrStatus 	status = TI_CHIP_MNGR_STATUS_SUCCESS;

	BTL_FUNC_START("TI_CHIP_MNGR_BTOn");

	TI_CHIP_MNGR_LOCK()

	if ((tiChipMngrData.btOnState != TI_CHIP_MNGR_BT_ON_STATE_NONE) || (tiChipMngrData.btOffState != TI_CHIP_MNGR_BT_ON_STATE_NONE))
	{
		BTL_FATAL_NORET(("BT On or Off Already in Progress, Can't handle another BT ON"));
		status = TI_CHIP_MNGR_STATUS_IN_PROGRESS;
	}
	else if ((tiChipMngrData.fmOnState != TI_CHIP_MNGR_FM_ON_STATE_NONE) || (tiChipMngrData.fmOffState != TI_CHIP_MNGR_FM_OFF_STATE_NONE) )
	{
		tiChipMngrData.btOnState = TI_CHIP_MNGR_BT_ON_STATE_PENDING;
		tiChipMngrData.btOnAsynchronous = BTHAL_TRUE;

		status = TI_CHIP_MNGR_STATUS_PENDING;
	}

	TI_CHIP_MNGR_UNLOCK();

	if (status == TI_CHIP_MNGR_STATUS_SUCCESS)
	{
		status = TiChipMngrInternalBTOn();
	}

	BTL_FUNC_END();

	return status;
}


TiChipMngrStatus TI_CHIP_MNGR_BTOnAbort(void)
{
	TiChipMngrStatus status = TI_CHIP_MNGR_STATUS_SUCCESS;

	BTL_FUNC_START("TI_CHIP_MNGR_BTOnAbort");

	TI_CHIP_MNGR_LOCK()

	if (TI_CHIP_MNGR_BT_ON_STATE_PENDING == tiChipMngrData.btOnState)
	{
		tiChipMngrData.btOnState = TI_CHIP_MNGR_BT_ON_STATE_NONE;
		tiChipMngrData.btOffAsynchronous = BTHAL_FALSE;
	}
	else
	{
		if (TI_CHIP_MNGR_BT_ON_STATE_WAITING_FOR_TRANSPORT == tiChipMngrData.btOnState)
		{
			status = TiChipMngrChipOffToBTOnProcessor(TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_ABORT);
		}
	}

	TI_CHIP_MNGR_UNLOCK();

	BTL_FUNC_END();

	return status;
}

TiChipMngrStatus TI_CHIP_MNGR_BTOff(void)
{
	TiChipMngrStatus 	status = TI_CHIP_MNGR_STATUS_SUCCESS;

	BTL_FUNC_START("TI_CHIP_MNGR_BTOff");

	TI_CHIP_MNGR_LOCK();

	if ((tiChipMngrData.btOnState != TI_CHIP_MNGR_BT_ON_STATE_NONE) || (tiChipMngrData.btOffState != TI_CHIP_MNGR_BT_ON_STATE_NONE))
	{
		BTL_FATAL_NORET(("BT On or Off Already in Progress, Can't handle another FM OFF"));
		status = TI_CHIP_MNGR_STATUS_IN_PROGRESS;
	}
	else if ((tiChipMngrData.fmOnState != TI_CHIP_MNGR_FM_ON_STATE_NONE) || (tiChipMngrData.fmOffState != TI_CHIP_MNGR_FM_OFF_STATE_NONE) )
	{
		tiChipMngrData.btOffState = TI_CHIP_MNGR_BT_OFF_STATE_PENDING;
		tiChipMngrData.btOffAsynchronous = BTHAL_TRUE;

		status = TI_CHIP_MNGR_STATUS_PENDING;
	}

	TI_CHIP_MNGR_UNLOCK();

	if (status == TI_CHIP_MNGR_STATUS_SUCCESS)
	{
		status = TiChipMngrInternalBTOff();
	}

	BTL_FUNC_END();

	return status;
}


TiChipMngrStatus TI_CHIP_MNGR_FMOn(void)
{
	TiChipMngrStatus 	status = TI_CHIP_MNGR_STATUS_SUCCESS;

	BTL_FUNC_START("TI_CHIP_MNGR_FMOn");

	TI_CHIP_MNGR_LOCK();

	if ((tiChipMngrData.fmOnState != TI_CHIP_MNGR_FM_ON_STATE_NONE) || (tiChipMngrData.fmOffState != TI_CHIP_MNGR_FM_OFF_STATE_NONE))
	{
		BTL_FATAL_NORET(("BT On or Off Already in Progress, Can't handle another FM ON"));
		status = TI_CHIP_MNGR_STATUS_IN_PROGRESS;
	}
	else if ((tiChipMngrData.btOnState != TI_CHIP_MNGR_BT_ON_STATE_NONE) || (tiChipMngrData.btOffState != TI_CHIP_MNGR_BT_OFF_STATE_NONE) )
	{
		tiChipMngrData.fmOnState = TI_CHIP_MNGR_FM_ON_STATE_PENDING;
		tiChipMngrData.fmOnAsynchronous = BTHAL_TRUE;

		status = TI_CHIP_MNGR_STATUS_PENDING;
	}

	TI_CHIP_MNGR_UNLOCK();

	if (status == TI_CHIP_MNGR_STATUS_SUCCESS)
	{
		status = TiChipMngrInternalFMOn();
	}

	BTL_FUNC_END();

	return status;
}

TiChipMngrStatus TI_CHIP_MNGR_FMOff(void)
{
	TiChipMngrStatus 	status = TI_CHIP_MNGR_STATUS_SUCCESS;

	BTL_FUNC_START("TI_CHIP_MNGR_FMOff");

	TI_CHIP_MNGR_LOCK();

	if ((tiChipMngrData.fmOnState != TI_CHIP_MNGR_FM_ON_STATE_NONE) || (tiChipMngrData.fmOffState != TI_CHIP_MNGR_FM_OFF_STATE_NONE))
	{
		BTL_FATAL_NORET(("BT On or Off Already in Progress, Can't handle another FM OFF"));
		status = TI_CHIP_MNGR_STATUS_IN_PROGRESS;
	}
	else if ((tiChipMngrData.btOnState != TI_CHIP_MNGR_BT_ON_STATE_NONE) || (tiChipMngrData.btOffState != TI_CHIP_MNGR_BT_OFF_STATE_NONE) )
	{
		tiChipMngrData.fmOffState = TI_CHIP_MNGR_FM_OFF_STATE_PENDING;
		tiChipMngrData.fmOffAsynchronous = BTHAL_TRUE;

		status = TI_CHIP_MNGR_STATUS_PENDING;
	}

	TI_CHIP_MNGR_UNLOCK();

	if (status == TI_CHIP_MNGR_STATUS_SUCCESS)
	{
		status = TiChipMngrInternalFMOff();
	}

	BTL_FUNC_END();

	return status;
}

TiChipMngrStatus TI_CHIP_MNGR_RegisterForPowerNotifications(TiChipMngrPowerNotificationsCb callback)
{
	tiChipMngrData.powerNotificationsCb = callback;

	return TI_CHIP_MNGR_STATUS_SUCCESS;
}

TiChipMngrStatus TI_CHIP_MNGR_RegisterForStateNotifications(TiChipMngrStateNotificationsCb callback)
{
	tiChipMngrData.stateNotificationsCb = callback;

	return TI_CHIP_MNGR_STATUS_SUCCESS;
}

TiChipMngrStatus TI_CHIP_MNGR_RegisterForBTNotifications(TiChipMngrBtNotificationsCb callback)
{
	tiChipMngrData.btNotificationsCb = callback;

	return TI_CHIP_MNGR_STATUS_SUCCESS;
}

TiChipMngrStatus TI_CHIP_MNGR_RegisterForFMNotifications(TiChipMngrFmNotificationsCb callback)
{
	tiChipMngrData.fmNotificationsCb = callback;

	return TI_CHIP_MNGR_STATUS_SUCCESS;
}

TiChipMngrState TI_CHIP_MNGR_GetState(void)
{
	return tiChipMngrData.state;
}

void TiChipMngrInitData(void)
{
	TiChipMngrSetState(TI_CHIP_MNGR_STATE_CHIP_OFF);

	tiChipMngrData.btOnState = TI_CHIP_MNGR_BT_ON_STATE_NONE;
	tiChipMngrData.btOffState = TI_CHIP_MNGR_BT_OFF_STATE_NONE;
	tiChipMngrData.fmOnState =TI_CHIP_MNGR_FM_ON_STATE_NONE;
	tiChipMngrData.fmOffState = TI_CHIP_MNGR_FM_OFF_STATE_NONE;

	tiChipMngrData.btOnAsynchronous = BTHAL_FALSE;
	tiChipMngrData.btOffAsynchronous = BTHAL_FALSE;
	tiChipMngrData.fmOnAsynchronous = BTHAL_FALSE;
	tiChipMngrData.fmOffAsynchronous = BTHAL_FALSE;

	tiChipMngrData.powerNotificationsCb = NULL;
	tiChipMngrData.stateNotificationsCb = NULL;
	tiChipMngrData.btNotificationsCb = NULL;
	tiChipMngrData.fmNotificationsCb = NULL;
}

void TiChipMngrSetState(TiChipMngrState state)
{
	BTL_LOG_INFO(("New Chip Mngr State: %s", TI_CHIP_MNGR_StateAsStr(state)));

	tiChipMngrData.state = state;

	if (tiChipMngrData.stateNotificationsCb != NULL)
	{
		(tiChipMngrData.stateNotificationsCb)(state);
	}
}

TiChipMngrStatus TiChipMngrChipOffToBTOnProcessor(TiChipMngrChipOffToBtOnEvent event)
{
	TiChipMngrStatus	status = TI_CHIP_MNGR_STATUS_SUCCESS;
	BtStatus			btStatus = BT_STATUS_SUCCESS;
	BTHAL_BOOL		keepProcessing = BTHAL_TRUE;

	BTL_FUNC_START("TiChipMngrChipOffToBTOnProcessor");

	BTL_UNUSED_PARAMETER_EBTIPS_RELEASE(event);

	BTL_LOG_DEBUG(("Event: %s", TiChipMngrChipOffToBtOnEventAsStr(event)));

	while (keepProcessing == BTHAL_TRUE)
	{
		keepProcessing = FALSE;

		BTL_LOG_DEBUG(("State: %s", TiChipMngrBtOnStateAsStr(tiChipMngrData.btOnState)));

		switch (tiChipMngrData.btOnState)
		{
			case TI_CHIP_MNGR_BT_ON_STATE_NONE:

				BTL_VERIFY_FATAL((TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_ABORT != event), TI_CHIP_MNGR_STATUS_INTERNAL_ERROR,
						("Unexpected event (%s)", TiChipMngrChipOffToBtOnEventAsStr(event)));

				/* Fall Through */

			case TI_CHIP_MNGR_BT_ON_STATE_PENDING:

				BTL_VERIFY_FATAL((TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_ABORT != event), TI_CHIP_MNGR_STATUS_INTERNAL_ERROR,
						("Unexpected event (%s)", TiChipMngrChipOffToBtOnEventAsStr(event)));

				tiChipMngrData.btOnAsynchronous = BTHAL_FALSE;
				tiChipMngrData.btOnState = TI_CHIP_MNGR_BT_ON_STATE_TURN_ON_TRANSPORT;

				BTL_LOG_INFO(("Init BT Radio (RMGR_RadioInit)"));

				btStatus =  RMGR_RadioInit();

				if (btStatus == BT_STATUS_SUCCESS)
				{
					BTL_LOG_INFO(("BT Radio Initialized"));

					tiChipMngrData.btOnState = TI_CHIP_MNGR_BT_ON_STATE_DONE;
					keepProcessing = BTHAL_TRUE;
				}
				else if (btStatus == BT_STATUS_PENDING)
				{

					BTL_LOG_INFO(("Waiting for BT Radio To Initialize"));

					tiChipMngrData.btOnAsynchronous = BTHAL_TRUE;
					tiChipMngrData.btOnState = TI_CHIP_MNGR_BT_ON_STATE_WAITING_FOR_TRANSPORT;

					status = TI_CHIP_MNGR_STATUS_PENDING;
				}
				else
				{
					BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unexpected return value from RMGR_RadioInit (%s)", pBT_Status(btStatus)));
				}

				break;

			case TI_CHIP_MNGR_BT_ON_STATE_TURN_ON_TRANSPORT:

				break;

			case TI_CHIP_MNGR_BT_ON_STATE_WAITING_FOR_TRANSPORT:

				switch (event)
				{
					case TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_TRANSPORT_READY:

						tiChipMngrData.btOnState = TI_CHIP_MNGR_BT_ON_STATE_DONE;
						keepProcessing = BTHAL_TRUE;
						break;

					case TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_ABORT:

						RMGR_RadioCancelInit();

						tiChipMngrData.btOnState = TI_CHIP_MNGR_BT_ON_STATE_WAITING_FOR_TRANSPORT_ABORT;
						status = TI_CHIP_MNGR_STATUS_PENDING;
						break;

					case TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_TRANSPORT_SHUTDOWN:

						BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unexpected event (%s)", TiChipMngrChipOffToBtOnEventAsStr(event)));

					default:
						BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unexpected event (%s)", TiChipMngrChipOffToBtOnEventAsStr(event)));
				}
				break;

			case TI_CHIP_MNGR_BT_ON_STATE_WAITING_FOR_TRANSPORT_ABORT:

				switch (event)
				{
					case TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_TRANSPORT_READY:

						tiChipMngrData.btOnState = TI_CHIP_MNGR_BT_ON_STATE_DONE;
						keepProcessing = BTHAL_TRUE;
						break;

					case TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_ABORT:

						BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unexpected event (%s)", TiChipMngrChipOffToBtOnEventAsStr(event)));


					case TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_TRANSPORT_SHUTDOWN:

						BTL_LOG_INFO(("Turning Off Chip (BTHAL_BTDRV_Shutdown)"));

						BTHAL_BTDRV_Shutdown();

						tiChipMngrData.btOnState = TI_CHIP_MNGR_BT_ON_STATE_DONE_ABORT;
						keepProcessing = BTHAL_TRUE;
						break;

					default:
						BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unexpected event (%s)", TiChipMngrChipOffToBtOnEventAsStr(event)));
				}
				break;

			case TI_CHIP_MNGR_BT_ON_STATE_DONE_ABORT:

				BTL_LOG_INFO(("Chip Off to BT On Aborted"));

				TiChipMngrCompleteBtOn(TI_CHIP_MNGR_STATE_CHIP_OFF);

				break;

			case TI_CHIP_MNGR_BT_ON_STATE_DONE:

				BTL_LOG_INFO(("Chip Off to BT On Done"));

				TiChipMngrCompleteBtOn(TI_CHIP_MNGR_STATE_BT_ON);

				break;
		}
	}

	BTL_FUNC_END();

	return status;
}

TiChipMngrStatus TiChipMngrInternalBTOn(void)
{
	TiChipMngrStatus status = TI_CHIP_MNGR_STATUS_SUCCESS;

	BTL_FUNC_START("TiChipMngrInternalBTOn");

	BTL_LOG_INFO(("Current State: %s", TI_CHIP_MNGR_StateAsStr(tiChipMngrData.state)));

	switch (tiChipMngrData.state)
	{
		case TI_CHIP_MNGR_STATE_CHIP_OFF:

			status = TiChipMngrChipOffToBTOnProcessor(TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_START);

			break;

		case TI_CHIP_MNGR_STATE_FM_ON:

			TiChipMngrCompleteBtOn(TI_CHIP_MNGR_STATE_ALL_ON);

			break;

		case TI_CHIP_MNGR_STATE_BT_ON:
		case TI_CHIP_MNGR_STATE_ALL_ON:

			BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unexpected Chip Mngr State for BT ON (%s)", TI_CHIP_MNGR_StateAsStr(tiChipMngrData.state)));

		default:

			BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unknown Chip Mngr State for BT ON (%d)", tiChipMngrData.state));

	}

	BTL_FUNC_END();

	return status;
}


TiChipMngrStatus TiChipMngrBTOnToChipOffProcessor(TiChipMngrBTOnToChipOffEvent event)
{
	TiChipMngrStatus	status = TI_CHIP_MNGR_STATUS_SUCCESS;
	BtStatus			btStatus = BT_STATUS_SUCCESS;
	BTHAL_BOOL		keepProcessing = BTHAL_TRUE;

	BTL_FUNC_START("TiChipMngrBTOnToChipOffProcessor");

	BTL_UNUSED_PARAMETER_EBTIPS_RELEASE(event);

	BTL_LOG_DEBUG(("Event: %s", TiChipMngrBtOnToChipOffEventAsStr(event)));

	while (keepProcessing == BTHAL_TRUE)
	{
		keepProcessing = FALSE;

		BTL_LOG_DEBUG(("State: %s", TiChipMngrBtOffStateAsStr(tiChipMngrData.btOffState)));

		switch (tiChipMngrData.btOffState)
		{
			case TI_CHIP_MNGR_BT_OFF_STATE_NONE:

				/* Fall Through */

			case TI_CHIP_MNGR_BT_OFF_STATE_PENDING:

				tiChipMngrData.btOffAsynchronous = BTHAL_FALSE;

				tiChipMngrData.btOffState = TI_CHIP_MNGR_BT_OFF_STATE_SHUT_DOWN_TRANSPORT;

				BTL_LOG_INFO(("Shutdown BT Radio (RMGR_RadioShutdown)"));

				btStatus =  RMGR_RadioShutdown();

				if (btStatus == BT_STATUS_SUCCESS)
				{
					BTL_LOG_INFO(("BT Radio Shut Down"));

					tiChipMngrData.btOffState = TI_CHIP_MNGR_BT_OFF_STATE_DONE;
					keepProcessing = BTHAL_TRUE;
				}
				else if (btStatus == BT_STATUS_PENDING)
				{
					BTL_LOG_INFO(("Waiting for BT Radio To Shut Down"));

					tiChipMngrData.btOffAsynchronous = BTHAL_TRUE;
					tiChipMngrData.btOffState = TI_CHIP_MNGR_BT_OFF_STATE_WAITING_FOR_TRANSPORT;

					status = TI_CHIP_MNGR_STATUS_PENDING;
				}
				else
				{
					BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unexpected return value from RMGR_RadioShutdown (%s)", pBT_Status(btStatus)));
				}

				break;

			case TI_CHIP_MNGR_BT_OFF_STATE_SHUT_DOWN_TRANSPORT:

				break;

			case TI_CHIP_MNGR_BT_OFF_STATE_WAITING_FOR_TRANSPORT:

				tiChipMngrData.btOffState = TI_CHIP_MNGR_BT_OFF_STATE_DONE;
				keepProcessing = TRUE;

				break;

			case TI_CHIP_MNGR_BT_OFF_STATE_DONE:

				BTL_LOG_INFO(("Turning Off Chip (BTHAL_BTDRV_Shutdown)"));
				BTHAL_BTDRV_Shutdown();

				TiChipMngrSendPowerNotification(TI_CHIP_MNGR_POWER_NOTIFICATION_CHIP_SHUT_DOWN);

				BTL_LOG_INFO(("BT On to Chip Off Done"));

				TiChipMngrCompleteBtOff(TI_CHIP_MNGR_STATE_CHIP_OFF);

				break;
		}
	}

	BTL_FUNC_END();

	return status;
}


TiChipMngrStatus TiChipMngrInternalBTOff(void)
{
	TiChipMngrStatus status = TI_CHIP_MNGR_STATUS_SUCCESS;

	BTL_FUNC_START("TiChipMngrInternalBTOff");

	BTL_LOG_INFO(("Current State: %s", TI_CHIP_MNGR_StateAsStr(tiChipMngrData.state)));

	switch (tiChipMngrData.state)
	{
		case TI_CHIP_MNGR_STATE_CHIP_OFF:

			/* Fall Through */

		case TI_CHIP_MNGR_STATE_FM_ON:

			BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unexpected Chip Mngr State for BT OFF (%s)", TI_CHIP_MNGR_StateAsStr(tiChipMngrData.state)));


		case TI_CHIP_MNGR_STATE_BT_ON:

			status = TiChipMngrBTOnToChipOffProcessor(TI_CHIP_MNGR_BT_ON_TO_CHIP_OFF_EVENT_START);

			break;

		case TI_CHIP_MNGR_STATE_ALL_ON:

			TiChipMngrCompleteBtOff(TI_CHIP_MNGR_STATE_FM_ON);

			break;

		default:

			BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unknown Chip Mngr State for BT OFF (%d)", tiChipMngrData.state));

	}

	BTL_FUNC_END();

	return status;
}


TiChipMngrStatus TiChipMngrChipOffToFMOnProcessor(TiChipMngrChipOffToFmOnEvent event)
{
	TiChipMngrStatus	status = TI_CHIP_MNGR_STATUS_SUCCESS;
	BtStatus			btStatus = BT_STATUS_SUCCESS;
	BTHAL_BOOL		keepProcessing = BTHAL_TRUE;

	BTL_FUNC_START("TiChipMngrChipOffToFMOnProcessor");

	BTL_UNUSED_PARAMETER_EBTIPS_RELEASE(event);

	BTL_LOG_DEBUG(("Event: %s", TiChipMngrChipOffToFmOnEventAsStr(event)));

	while (keepProcessing == BTHAL_TRUE)
	{
		keepProcessing = FALSE;

		BTL_LOG_DEBUG(("Event: %s", TiChipMngrChipOffToFmOnEventAsStr(event)));

		switch (tiChipMngrData.fmOnState)
		{
			case TI_CHIP_MNGR_FM_ON_STATE_NONE:

				/* Fall Through */

			case TI_CHIP_MNGR_FM_ON_STATE_PENDING:

				tiChipMngrData.fmOnAsynchronous = BTHAL_FALSE;

				tiChipMngrData.fmOnState = TI_CHIP_MNGR_FM_ON_STATE_TURN_ON_TRANSPORT;

				BTL_LOG_INFO(("Init BT Radio (RMGR_RadioInit)"));

				btStatus =  RMGR_RadioInit();

				if (btStatus == BT_STATUS_SUCCESS)
				{
					BTL_LOG_INFO(("BT Radio Initialized"));

					tiChipMngrData.fmOnState = TI_CHIP_MNGR_FM_ON_STATE_DONE;
					keepProcessing = BTHAL_TRUE;
				}
				else if (btStatus == BT_STATUS_PENDING)
				{
					BTL_LOG_INFO(("Waiting for BT Radio To Initialize"));

					tiChipMngrData.fmOnAsynchronous = BTHAL_TRUE;
					tiChipMngrData.fmOnState = TI_CHIP_MNGR_FM_ON_STATE_WAITING_FOR_TRANSPORT;

					status = TI_CHIP_MNGR_STATUS_PENDING;
				}
				else
				{
					BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unexpected return value from RMGR_RadioInit (%s)", pBT_Status(btStatus)));
				}

				break;

			case TI_CHIP_MNGR_FM_ON_STATE_TURN_ON_TRANSPORT:

				break;

			case TI_CHIP_MNGR_FM_ON_STATE_WAITING_FOR_TRANSPORT:

				tiChipMngrData.fmOnState = TI_CHIP_MNGR_FM_ON_STATE_DONE;
				keepProcessing = BTHAL_TRUE;

				break;

			case TI_CHIP_MNGR_FM_ON_STATE_DONE:

				BTL_LOG_INFO(("Chip Off to FM On Done"));

				TiChipMngrCompleteFmOn(TI_CHIP_MNGR_STATE_FM_ON);

				break;
		}
	}

	BTL_FUNC_END();

	return status;
}

TiChipMngrStatus TiChipMngrInternalFMOn(void)
{
	TiChipMngrStatus status = TI_CHIP_MNGR_STATUS_SUCCESS;

	BTL_FUNC_START("TiChipMngrInternalFMOn");

	BTL_LOG_INFO(("Current State: %s", TI_CHIP_MNGR_StateAsStr(tiChipMngrData.state)));

	switch (tiChipMngrData.state)
	{
		case TI_CHIP_MNGR_STATE_CHIP_OFF:

			status = TiChipMngrChipOffToFMOnProcessor(TI_CHIP_MNGR_CHIP_OFF_TO_FM_ON_EVENT_START);

			break;

		case TI_CHIP_MNGR_STATE_BT_ON:

			TiChipMngrCompleteFmOn(TI_CHIP_MNGR_STATE_ALL_ON);

			break;

		case TI_CHIP_MNGR_STATE_FM_ON:

			/* Fall Through */

		case TI_CHIP_MNGR_STATE_ALL_ON:

			BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unexpected Chip Mngr State for FM ON (%s)", TI_CHIP_MNGR_StateAsStr(tiChipMngrData.state)));

		default:

			BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unknown Chip Mngr State for FM ON (%d)", tiChipMngrData.state));

	}

	BTL_FUNC_END();

	return status;
}

TiChipMngrStatus TiChipMngrFMOnToChipOffProcessor(TiChipMngrFMOnToChipOffEvent event)
{
	TiChipMngrStatus	status = TI_CHIP_MNGR_STATUS_SUCCESS;
	BtStatus			btStatus = BT_STATUS_SUCCESS;
	BTHAL_BOOL		keepProcessing = BTHAL_TRUE;

	BTL_FUNC_START("TiChipMngrFMOnToChipOffProcessor");

	BTL_UNUSED_PARAMETER_EBTIPS_RELEASE(event);

	BTL_LOG_DEBUG(("Event: %s", TiChipMngrFmOnToChipOffEventAsStr(event)));

	while (keepProcessing == BTHAL_TRUE)
	{
		keepProcessing = FALSE;

		BTL_LOG_DEBUG(("State: %s", TiChipMngrFmOffStateAsStr(tiChipMngrData.fmOffState)));

		switch (tiChipMngrData.fmOffState)
		{
			case TI_CHIP_MNGR_FM_OFF_STATE_NONE:

				/* Fall Through */

			case TI_CHIP_MNGR_FM_OFF_STATE_PENDING:

				tiChipMngrData.fmOffAsynchronous = BTHAL_FALSE;

				tiChipMngrData.fmOffState = TI_CHIP_MNGR_FM_OFF_STATE_SHUT_DOWN_TRANSPORT;

				BTL_LOG_INFO(("Shutdown BT Radio (RMGR_RadioShutdown)"));

				btStatus =  RMGR_RadioShutdown();

				if (btStatus == BT_STATUS_SUCCESS)
				{
					BTL_LOG_INFO(("BT Radio Shut Down"));

					tiChipMngrData.fmOffState = TI_CHIP_MNGR_FM_OFF_STATE_DONE;
					keepProcessing = BTHAL_TRUE;
				}
				else if (btStatus == BT_STATUS_PENDING)
				{

					BTL_LOG_INFO(("Waiting for BT Radio To Shut Down"));

					tiChipMngrData.fmOffAsynchronous = BTHAL_TRUE;
					tiChipMngrData.fmOffState = TI_CHIP_MNGR_FM_OFF_STATE_WAITING_FOR_TRANSPORT;

					status = TI_CHIP_MNGR_STATUS_PENDING;
				}
				else
				{
					BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unexpected return value from RMGR_RadioShutdown (%s)", pBT_Status(btStatus)));
				}

				break;

			case TI_CHIP_MNGR_FM_OFF_STATE_SHUT_DOWN_TRANSPORT:

				break;

			case TI_CHIP_MNGR_FM_OFF_STATE_WAITING_FOR_TRANSPORT:

				tiChipMngrData.fmOffState = TI_CHIP_MNGR_FM_OFF_STATE_DONE;
				keepProcessing = TRUE;

				break;

			case TI_CHIP_MNGR_FM_OFF_STATE_DONE:

				BTL_LOG_INFO(("Turning Off Chip (BTHAL_BTDRV_Shutdown)"));
				BTHAL_BTDRV_Shutdown();

				TiChipMngrSendPowerNotification(TI_CHIP_MNGR_POWER_NOTIFICATION_CHIP_SHUT_DOWN);

				BTL_LOG_INFO(("FM On to Chip Off Done"));

				TiChipMngrCompleteFmOff(TI_CHIP_MNGR_STATE_CHIP_OFF);

				break;
		}
	}

	BTL_FUNC_END();

	return status;
}

TiChipMngrStatus TiChipMngrInternalFMOff(void)
{
	TiChipMngrStatus status = TI_CHIP_MNGR_STATUS_SUCCESS;

	BTL_FUNC_START("TiChipMngrInternalFMOff");

	BTL_LOG_INFO(("Current State: %s", TI_CHIP_MNGR_StateAsStr(tiChipMngrData.state)));

	switch (tiChipMngrData.state)
	{
		case TI_CHIP_MNGR_STATE_CHIP_OFF:

			/* Fall Through */

		case TI_CHIP_MNGR_STATE_BT_ON:

			BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unexpected Chip Mngr State for FM OFF (%s)", TI_CHIP_MNGR_StateAsStr(tiChipMngrData.state)));


		case TI_CHIP_MNGR_STATE_FM_ON:

			status = TiChipMngrFMOnToChipOffProcessor(TI_CHIP_MNGR_FM_ON_TO_CHIP_OFF_EVENT_START);

			break;

		case TI_CHIP_MNGR_STATE_ALL_ON:

			TiChipMngrCompleteFmOff(TI_CHIP_MNGR_STATE_BT_ON);

			break;

		default:

			BTL_FATAL(TI_CHIP_MNGR_STATUS_INTERNAL_ERROR, ("Unknown Chip Mngr State for FM OFF (%d)", tiChipMngrData.state));

	}

	BTL_FUNC_END();

	return status;
}

void TiChipMngrCompleteBtOn(TiChipMngrState newChipMngrState)
{
	BTL_FUNC_START("TiChipMngrCompleteBtOn");

	TiChipMngrSetState(newChipMngrState);

	if (tiChipMngrData.btOnAsynchronous == BTHAL_TRUE)
	{
		BTL_LOG_DEBUG(("Reporting BT notification"));

		if (tiChipMngrData.btNotificationsCb != NULL)
		{
			if (TI_CHIP_MNGR_STATE_CHIP_OFF == newChipMngrState)
			{
				(tiChipMngrData.btNotificationsCb)(TI_CHIP_MNGR_BT_NOTIFICATION_BT_ON_ABORT);
			}
			else
			{
				(tiChipMngrData.btNotificationsCb)(TI_CHIP_MNGR_BT_NOTIFICATION_BT_ON_COMPLETE);
			}
		}
	}

	tiChipMngrData.btOnAsynchronous = BTHAL_FALSE;
	tiChipMngrData.btOnState = TI_CHIP_MNGR_BT_ON_STATE_NONE;

	TiChipMngrStartPendingFmOperation();

	BTL_FUNC_END();
}

void TiChipMngrCompleteBtOff(TiChipMngrState newChipMngrState)
{
	BTL_FUNC_START("TiChipMngrCompleteBtOff");

	TiChipMngrSetState(newChipMngrState);

	if (tiChipMngrData.btOffAsynchronous == BTHAL_TRUE)
	{
		BTL_LOG_DEBUG(("Reporting BT notification"));

		if (tiChipMngrData.btNotificationsCb != NULL)
		{
			(tiChipMngrData.btNotificationsCb)(TI_CHIP_MNGR_BT_NOTIFICATION_BT_OFF_COMPLETE);
		}
	}

	tiChipMngrData.btOffAsynchronous = BTHAL_FALSE;
	tiChipMngrData.btOffState = TI_CHIP_MNGR_BT_OFF_STATE_NONE;

	TiChipMngrStartPendingFmOperation();

	BTL_FUNC_END();
}

void TiChipMngrCompleteFmOn(TiChipMngrState newChipMngrState)
{
	BTL_FUNC_START("TiChipMngrCompleteFmOn");

	TiChipMngrSetState(newChipMngrState);

	if (tiChipMngrData.fmOnAsynchronous == BTHAL_TRUE)
	{
		BTL_LOG_DEBUG(("Reporting FM notification"));

		if (tiChipMngrData.fmNotificationsCb != NULL)
		{
			(tiChipMngrData.fmNotificationsCb)(TI_CHIP_MNGR_FM_NOTIFICATION_FM_ON_COMPLETE);
		}
	}

	tiChipMngrData.fmOnAsynchronous = BTHAL_FALSE;
	tiChipMngrData.fmOnState = TI_CHIP_MNGR_FM_ON_STATE_NONE;

	TiChipMngrStartPendingBtOperation();

	BTL_FUNC_END();
}

void TiChipMngrCompleteFmOff(TiChipMngrState newChipMngrState)
{
	BTL_FUNC_START("TiChipMngrCompleteFmOff");

	TiChipMngrSetState(newChipMngrState);

	if (tiChipMngrData.fmOffAsynchronous == BTHAL_TRUE)
	{
		BTL_LOG_DEBUG(("Reporting FM notification"));

		if (tiChipMngrData.fmNotificationsCb != NULL)
		{
			(tiChipMngrData.fmNotificationsCb)(TI_CHIP_MNGR_FM_NOTIFICATION_FM_OFF_COMPLETE);
		}
	}

	tiChipMngrData.fmOffAsynchronous = BTHAL_FALSE;
	tiChipMngrData.fmOffState = TI_CHIP_MNGR_FM_OFF_STATE_NONE;

	TiChipMngrStartPendingBtOperation();

	BTL_FUNC_END();
}

void TiChipMngrStartPendingFmOperation(void)
{
	TiChipMngrStatus	status = TI_CHIP_MNGR_STATUS_SUCCESS;

	BTL_FUNC_START("TiChipMngrStartPendingFmOperation");

	BTL_VERIFY_FATAL(	((tiChipMngrData.fmOnState != TI_CHIP_MNGR_FM_ON_STATE_PENDING) ||
						(tiChipMngrData.fmOffState != TI_CHIP_MNGR_FM_OFF_STATE_PENDING)),
						TI_CHIP_MNGR_STATUS_INTERNAL_ERROR,
						("Illegal State: Both FM On & Off are pending execution"));

	if (tiChipMngrData.fmOnState == TI_CHIP_MNGR_FM_ON_STATE_PENDING)
	{
		TiChipMngrInternalFMOn();
	}
	else if (tiChipMngrData.fmOffState == TI_CHIP_MNGR_FM_OFF_STATE_PENDING)
	{
		TiChipMngrInternalFMOff();
	}

	BTL_FUNC_END();
}

void TiChipMngrStartPendingBtOperation(void)
{
	TiChipMngrStatus	status = TI_CHIP_MNGR_STATUS_SUCCESS;

	BTL_FUNC_START("TiChipMngrStartPendingBtOperation");

	BTL_VERIFY_FATAL(	((tiChipMngrData.btOnState != TI_CHIP_MNGR_BT_ON_STATE_PENDING) ||
						(tiChipMngrData.btOffState != TI_CHIP_MNGR_BT_OFF_STATE_PENDING)),
						TI_CHIP_MNGR_STATUS_INTERNAL_ERROR,
						("Illegal State: Both BT On & Off are pending execution"));

	if (tiChipMngrData.btOnState == TI_CHIP_MNGR_BT_ON_STATE_PENDING)
	{
		TiChipMngrInternalBTOn();
	}
	else if (tiChipMngrData.btOffState == TI_CHIP_MNGR_BT_OFF_STATE_PENDING)
	{
		TiChipMngrInternalBTOff();
	}

	BTL_FUNC_END();
}

BTHAL_BOOL TiChipMngrIsBtOnInProgress(void)
{
	if ((tiChipMngrData.btOnState == TI_CHIP_MNGR_BT_ON_STATE_NONE) || (tiChipMngrData.btOnState == TI_CHIP_MNGR_BT_ON_STATE_PENDING))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BTHAL_BOOL TiChipMngrIsBtOffInProgress(void)
{
	if ((tiChipMngrData.btOffState == TI_CHIP_MNGR_BT_OFF_STATE_NONE) || (tiChipMngrData.btOffState == TI_CHIP_MNGR_BT_OFF_STATE_PENDING))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


BTHAL_BOOL TiChipMngrIsFmOnInProgress(void)
{
	if ((tiChipMngrData.fmOnState == TI_CHIP_MNGR_FM_ON_STATE_NONE) || (tiChipMngrData.fmOnState == TI_CHIP_MNGR_FM_ON_STATE_PENDING))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BTHAL_BOOL TiChipMngrIsFmOffInProgress(void)
{
	if ((tiChipMngrData.fmOffState == TI_CHIP_MNGR_FM_OFF_STATE_NONE) || (tiChipMngrData.fmOffState == TI_CHIP_MNGR_FM_OFF_STATE_PENDING))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void TI_CHIP_MNGR_RadioCallback(TiChipMngrRadioNotificationType notificationType, void *parms)
{
	BTL_FUNC_START("TI_CHIP_MNGR_RadioCallback");

	UNUSED_PARAMETER(parms);

	BTL_LOG_DEBUG(("Notification Type: %s", TiChipMngrRadioCallbackStateAsStr(notificationType)));

	switch (notificationType)
	{
		case TI_CHIP_MNGR_RADIO_NOTIFICATION_HCI_INIT_STATUS:

			if (TiChipMngrIsBtOnInProgress() == BTHAL_TRUE)
			{
				TiChipMngrChipOffToBTOnProcessor(TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_TRANSPORT_READY);
			}
			else if (TiChipMngrIsFmOnInProgress() == BTHAL_TRUE)
			{
				TiChipMngrChipOffToFMOnProcessor(TI_CHIP_MNGR_CHIP_OFF_TO_FM_ON_EVENT_TRANSPORT_READY);
			}
			else
			{
				BTL_FATAL_NO_RETVAR(("HCI Init Status Received while neither FM nor BT On in progress"));
			}

			break;

		case TI_CHIP_MNGR_RADIO_NOTIFICATION_HCI_DEINIT_STATUS:

			if (TiChipMngrIsBtOffInProgress() == BTHAL_TRUE)
			{
				TiChipMngrBTOnToChipOffProcessor(TI_CHIP_MNGR_BT_ON_TO_CHIP_OFF_EVENT_TRANSPORT_SHUTDOWN);
			}
			else if (TiChipMngrIsBtOnInProgress() == BTHAL_TRUE)
			{
				TiChipMngrChipOffToBTOnProcessor(TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_TRANSPORT_SHUTDOWN);
			}
			else if (TiChipMngrIsFmOffInProgress() == BTHAL_TRUE)
			{
				TiChipMngrFMOnToChipOffProcessor(TI_CHIP_MNGR_FM_ON_TO_CHIP_OFF_EVENT_TRANSPORT_SHUTDOWN);
			}
			else
			{
				BTL_FATAL_NO_RETVAR(("HCI De-Init Status Received while neither FM nor BT processes in progress"));
			}

		case TI_CHIP_MNGR_RADIO_NOTIFICATION_HCI_UART_INITIALIZED:

			BTL_LOG_INFO(("Turning on Chip (BTHAL_BTDRV_Reset)"));
			BTHAL_BTDRV_Reset();

			TiChipMngrSendPowerNotification(TI_CHIP_MNGR_POWER_NOTIFICATION_UART_INITIALIZED);


			break;

		case TI_CHIP_MNGR_RADIO_NOTIFICATION_HCI_UART_DEINITIALIZED:

			break;

		default:

			BTL_FATAL_NO_RETVAR(("Invalid Notification Type (%d)", notificationType));

	}

	BTL_FUNC_END();

}

static void TiChipMngrSendPowerNotification(TiChipMngrPowerNotificationType notification)
{
	if (tiChipMngrData.powerNotificationsCb != NULL)
	{
		(tiChipMngrData.powerNotificationsCb)(notification);
	}
}

const char *TI_CHIP_MNGR_StateAsStr(TiChipMngrState state)
{
	switch (state)
	{
		case TI_CHIP_MNGR_STATE_CHIP_OFF:  	return "CHIP OFF";
		case TI_CHIP_MNGR_STATE_BT_ON: 		return "BT ON";
		case TI_CHIP_MNGR_STATE_FM_ON: 		return "FM ON";
		case TI_CHIP_MNGR_STATE_ALL_ON:		return "ALL ON";
		default:								return "UNKNOWN";
	};
}

const char *TiChipMngrBtOnStateAsStr(TiChipMngrBtOnState state)
{
	switch (state)
	{
		case TI_CHIP_MNGR_BT_ON_STATE_NONE:							return "NONE";
		case TI_CHIP_MNGR_BT_ON_STATE_PENDING:							return "PENDING";
		case TI_CHIP_MNGR_BT_ON_STATE_TURN_ON_TRANSPORT:				return "Turn On Transport";
		case TI_CHIP_MNGR_BT_ON_STATE_WAITING_FOR_TRANSPORT:			return "Waiting For Transport";
		case TI_CHIP_MNGR_BT_ON_STATE_WAITING_FOR_TRANSPORT_ABORT:  	return "Waiting For Transport Abort";
		case TI_CHIP_MNGR_BT_ON_STATE_DONE_ABORT:						return "Done Abort";
		case TI_CHIP_MNGR_BT_ON_STATE_DONE:							return "Done";
		default: 															return "UNKNOWN";
	};
}

const char *TiChipMngrChipOffToBtOnEventAsStr (TiChipMngrChipOffToBtOnEvent event)
{
	switch (event)
	{
		case TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_START:					return "START";
		case TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_TRANSPORT_READY:		return "Transport Ready";
		case TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_TRANSPORT_SHUTDOWN:	return "Transport Shutdown";
		case TI_CHIP_MNGR_CHIP_OFF_TO_BT_ON_EVENT_ABORT:      	return "ABORT";
		default:																return "UNKNOWN";
	};
}

const char *TiChipMngrBtOffStateAsStr(TiChipMngrBtOffState state)
{
	switch (state)
	{
		case TI_CHIP_MNGR_BT_OFF_STATE_NONE:						return "NONE";
		case TI_CHIP_MNGR_BT_OFF_STATE_PENDING:					return "PENDING";
		case TI_CHIP_MNGR_BT_OFF_STATE_SHUT_DOWN_TRANSPORT:		return "Shut Down Transport";
		case TI_CHIP_MNGR_BT_OFF_STATE_WAITING_FOR_TRANSPORT:		return "Waiting For Transport";
		case TI_CHIP_MNGR_BT_OFF_STATE_DONE:						return "Done";
		default: 														return "UNKNOWN";
	};
}

const char *TiChipMngrBtOnToChipOffEventAsStr (TiChipMngrBTOnToChipOffEvent event)
{
	switch (event)
	{
		case TI_CHIP_MNGR_BT_ON_TO_CHIP_OFF_EVENT_START:					return "START";
		case TI_CHIP_MNGR_BT_ON_TO_CHIP_OFF_EVENT_TRANSPORT_SHUTDOWN:	return "Transport Shutdown";
		default:																return "UNKNOWN";
	};
}

const char *TiChipMngrChipOffToFmOnEventAsStr (TiChipMngrChipOffToFmOnEvent event)
{
	switch (event)
	{
		case TI_CHIP_MNGR_CHIP_OFF_TO_FM_ON_EVENT_START:				return "START";
		case TI_CHIP_MNGR_CHIP_OFF_TO_FM_ON_EVENT_TRANSPORT_READY:	return "Transport Ready";
		default:															return "UNKNOWN";
	};
}

const char *TiChipMngrFmOffStateAsStr(TiChipMngrFmOffState state)
{
	switch (state)
	{
		case TI_CHIP_MNGR_FM_OFF_STATE_NONE:						return "NONE";
		case TI_CHIP_MNGR_FM_OFF_STATE_PENDING:					return "PENDING";
		case TI_CHIP_MNGR_FM_OFF_STATE_SHUT_DOWN_TRANSPORT:		return "Shut Down Transport";
		case TI_CHIP_MNGR_FM_OFF_STATE_WAITING_FOR_TRANSPORT:		return "Waiting For Transport";
		case TI_CHIP_MNGR_FM_OFF_STATE_DONE:						return "Done";
		default: 														return "UNKNOWN";
	};
}

const char *TiChipMngrFmOnToChipOffEventAsStr (TiChipMngrFMOnToChipOffEvent event)
{
	switch (event)
	{
		case TI_CHIP_MNGR_FM_ON_TO_CHIP_OFF_EVENT_START:					return "START";
		case TI_CHIP_MNGR_FM_ON_TO_CHIP_OFF_EVENT_TRANSPORT_SHUTDOWN:	return "Transport Shutdown";
		default:																return "UNKNOWN";
	};
}

const char *TiChipMngrRadioCallbackStateAsStr(TiChipMngrRadioNotificationType notificationType)
{
	switch (notificationType)
	{
		case TI_CHIP_MNGR_RADIO_NOTIFICATION_HCI_INIT_STATUS: 		return "HCI_INIT_STATUS";
		case TI_CHIP_MNGR_RADIO_NOTIFICATION_HCI_DEINIT_STATUS:		return "HCI_DEINIT_STATUS";
		case TI_CHIP_MNGR_RADIO_NOTIFICATION_HCI_UART_INITIALIZED:	return "UART INITIALIZED";
		case TI_CHIP_MNGR_RADIO_NOTIFICATION_HCI_UART_DEINITIALIZED:	return "UART DE-INITIALIZED";
		default:														return "UNKNOWN";
	};
}

