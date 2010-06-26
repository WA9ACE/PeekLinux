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
*   FILE NAME:		bthal_mc.c
*
*   DESCRIPTION:	Implementation of the BTHAL MC Module.
*
*   AUTHOR:			Itay Klein
*
\*******************************************************************************/


#include "btl_config"
#include "osapi.h"
#include "bthal_common.h"


#if BTL_CONFIG_VG == BTL_CONFIG_ENABLED


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include <stdio.h>

/*
#include "aci_all.h"
#include "aci_cmh.h"
*/

#include "bat.h"
#include "bthal_mc.h"
#include "bthal_log.h"
#include "bthal_fs.h"
#include "Bttypes.h"
#include "bthal_os.h"
#include "bthal_log_modules.h"

#define BTHAL_MODEM_LOG_ERROR(msg)		BTHAL_LOG_ERROR(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_MODEM, msg)	
#define BTHAL_MODEM_LOG_INFO(msg)		BTHAL_LOG_INFO(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_MODEM, msg)
#define BTHAL_MODEM_LOG_FUNCTION(msg)	BTHAL_LOG_FUNCTION(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_MODEM, msg)
#define BTHAL_MODEM_LOG_DEBUG(msg)        BTHAL_LOG_DEBUG(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_MODEM, msg)

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

/*
 *	The number of BAT Instances used by the BTHAL MC Module.
 */
#define BTHAL_MC_NUM_OF_BAT_INSTANCES	        1

/*
 *	The number of BAT Clients used by the BTHAL MC Module.
 */
#define BTHAL_MC_NUM_OF_BAT_CLIENTS		1

/*
 *	The size of the buffer given to the BAT Module for BAT Instances.
 */
#define BTHAL_MC_BAT_INST_BUF_SIZE		(BAT_INSTANCE_HEADER_SIZE * BTHAL_MC_NUM_OF_BAT_INSTANCES)

/*
 *	The size of of the buffer given to a BAT Instance for BAT Clients
 */
#define BTHAL_MC_BAT_CLIENTS_BUF_SIZE	(BAT_INSTANCE_SIZE + BAT_CLIENT_SIZE * BTHAL_MC_NUM_OF_BAT_CLIENTS)

/*
 *	Maximal number of BTHAL MC Users,
 *	Used to determine the size of a BthalMcContext array
 */
#define BTHAL_MC_NUM_OF_USERS	                    4

/*	
 *	the increment for the level is for complying with BT indicators spec (0-5) insted of 0-4
 *	in this case we loose the 0 value but its better this way.
 */
#define CONVERT_MODEM_SIGNAL_LEVEL_TO_SIG_SIGNAL_LEVEL(n) ((n)+1)

/*
 *	The minimal value of a signal level.
 */
#define BTHAL_MC_MIN_MODEM_SIGNAL_LEVEL     0 

/*
 *	The maximal value of a signal level.
 */
#define BTHAL_MC_MAX_MODEM_SIGNAL_LEVEL     4

/*
 *	A value representing a service related to a phone number,
 *	used in the response for AT+CNUM=?.
 */
#define BTHAL_MC_SUBSCRIBER_SERVICE		        4

/*
 *	The number of HFG Indicators as defined by the HFG spec
 */
#define BTHAL_MC_NUM_OF_HFG_INDICATORS	7

/*
 *	The maximal number of calls that the BTHAL MC Module can handle simultanously.
 */
#define BTHAL_MC_MAX_NUM_CALLS				2

/*
 *	Indicating an invalid value for a call index.
 */
#define BTHAL_MC_INVALID_CALL_INDEX			0xffffffff

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

typedef U32 BthalMcContextSettings;

/*	The relevant context is used by a BTHAL MC User */
#define BTHAL_MC_CONTEXT_SETTINGS_USED				0x00000001

/*	An AT Command is being processed in the relevant context */
#define BTHAL_MC_CONTEXT_SETTINGS_PROCESSING		0x00000002

/*	Indicating whether Extended Errors (+CME ERROR: <err>) is enabled */
#define BTHAL_MC_CONTEXT_SETTINGS_EXTENDED_ERR		0x00000010

/*	Indicating whether calling line identification (CLIP) is enabled */
#define BTHAL_MC_CONTEXT_SETTINGS_CLIP				0x00000020

/*	Indicating whther indicator reporting is enabled */
#define BTHAL_MC_CONTEXT_SETTINGS_IND_REPORTING	0x00000040


typedef enum _BthalMcCallState
{
	BTHAL_MC_CALL_STATE_IDLE,
	BTHAL_MC_CALL_STATE_INCOMING,
	BTHAL_MC_CALL_STATE_OUTGOING,
	BTHAL_MC_CALL_STATE_ALERTING,
	BTHAL_MC_CALL_STATE_ACTIVE,
	BTHAL_MC_CALL_STATE_HELD,
	
	BTHAL_MC_NUM_CALL_STATES
}BthalMcCallState;

typedef enum _BthalMcCallEvent
{
	BTHAL_MC_CALL_EVENT_IDLE,
	BTHAL_MC_CALL_EVENT_INCOMING,
	BTHAL_MC_CALL_EVENT_OUTGOING,
	BTHAL_MC_CALL_EVENT_ALERTING,
	BTHAL_MC_CALL_EVENT_ACTIVE,
	BTHAL_MC_CALL_EVENT_HELD,

	BTHAL_MC_NUM_CALL_EVENTS
}BthalMcCallEvent;


typedef BthalStatus (*BthalMcCallStateMachineFunction)(BTHAL_U32 callIndex);
/********************************************************************************
 *
 * Structures
 *
 *******************************************************************************/

typedef struct _BthalMcAtCommand
{
	union
	{
		T_BAT_no_parameter noparam;
		T_BAT_cmd_at_d atd;
		T_BAT_cmd_set_plus_vts vts;		
		T_BAT_cmd_set_percent_chld chld;
		T_BAT_cmd_set_plus_ccwa ccwa;
		T_BAT_cmd_set_plus_cops cops;	
		T_BAT_cmd_set_plus_cpbs cpbs;
		T_BAT_cmd_set_plus_cpbr cpbr;
		T_BAT_cmd_set_plus_cpbf cpbf;
		T_BAT_cmd_set_plus_cscs cscs;
	}paramMem;
		
	T_BAT_cmd_send cmd;

}BthalMcAtCommand;

struct _BthalMcContext
{
	ListEntry node;
	
	BthalMcCallback callback;

	void *userData;

	BthalMcContextSettings settings;
	
	BthalMcAtCommand atCmd;

	 /* If true then we are in Response and Hold state */
	BTHAL_BOOL responseHold;
};

typedef struct _BthalMcModuleData
{
	/* Memory needed for BAT Library instance */
	BTHAL_U32 batInstBuf[(BTHAL_MC_BAT_INST_BUF_SIZE + 3) / 4];

	/* Memory needed for BAT Clients */
	BTHAL_U32 batClientBuf[(BTHAL_MC_BAT_CLIENTS_BUF_SIZE + 3) / 4];

	/* BTHAL MC Contexts memory */
	BthalMcContext contextsMem[BTHAL_MC_NUM_OF_USERS];

	T_BAT_instance instance;

	T_BAT_config config;

	T_BAT_client client;

	T_BAT_client unsClient;

	/* Queue for BTHAL MC Commands */
	ListEntry cmdQue;

	/* A semphore from protecting cmdQue */
	BthalOsSemaphoreHandle queSemaphore;

	/* Variables for incoming call RING + CLIP */
	BTHAL_BOOL isRingClipTimerScheduled;

	BTHAL_U8 clip_number[BAT_MAX_CLIP_NUMBER_LEN];

	BTHAL_U8 clip_type;

	/* Actual RSSI received from the Modem */
	BTHAL_S16 actualSignalLevel;

	/* An array for saving current indicators */
	BTHAL_U8 indicators[BTHAL_MC_NUM_OF_HFG_INDICATORS];

	/* The state machine for calls handled by the BTHAL MC. */
	BthalMcCallStateMachineFunction callStateMachine[BTHAL_MC_NUM_CALL_STATES][BTHAL_MC_NUM_CALL_EVENTS];

	/* An array representing the call states of the current calls */
	BthalMcCallState currentCalls[BTHAL_MC_MAX_NUM_CALLS];

}BthalMcModuleData;

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

static BthalMcModuleData moduleData = {0};

BTHAL_U8 bthalMcRingClipTimerIndex;

#define BTHAL_MC_RING_CLIP_TIMER_INTERVAL 4000

/*This variable keeps track of whether the incoming call is a waiting one or not. 
*  The logic behind this variable depends on the BAT indications and also in the order
*  of the indications received*/
BTHAL_BOOL isCallWaiting = 0;

/********************************************************************************
 *
 * Macros
 *
 *******************************************************************************/

 #define BTHAL_MC_VERIFY_CONTEXT_IDLE(context) \
 	if (BTHAL_MC_CONTEXT_SETTINGS_PROCESSING & (context)->settings) \
 	{ \
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: The context is already processing an AT command")); \
		return BTHAL_STATUS_BUSY; \
 	}

#define BTHAL_MC_SET_CONTEXT_NOT_PROCESSING(context) \
	((context)->settings &= ~BTHAL_MC_CONTEXT_SETTINGS_PROCESSING)

/************************************************************************************
 *
 * Internal functions declarations:
 *
 ***********************************************************************************/

static void BthalMcCmdQuePushBack(BthalMcContext *context);

static void BthalMcCmdQuePopHead(void);

static BthalMcContext* BthalMcCmdQueGetHead(void);

static void BthalMcInitModuleData(void);

static BthalStatus BthalMcInitBat(void);

static BthalStatus BthalMcDeinitBat(void);

static void BthalMcOpenBatClient(void);

static void BthalMcCloseBatClient(void);

static void BthalMcNewBatInstanceSigCb(T_BAT_signal sig);

static void BthalMcClientSigCb(T_BAT_client client, T_BAT_signal sig);

static int BthalMcUnsResultCb(T_BAT_client client, T_BAT_cmd_response *rsp);

static int BthalMcClientRspCb(T_BAT_client client, T_BAT_cmd_response *rsp);

static void BthalMcSendCommand(void);

static int BthalMcHandleRingUns(void);

static int BthalMcHandleCcwaUns(const T_BAT_res_uns_plus_ccwa *ccwa);

static int BthalMcHandleCpiUns(const T_BAT_res_uns_percent_cpi *cpi);

static int BthalMcHandleCsqUns(const T_BAT_res_uns_percent_csq *csq);

static int BthalMcHandleCregUns(const T_BAT_res_uns_percent_creg *creg);

static int BthalMcHandleClipUns(const T_BAT_res_uns_plus_clip *clip);

static int BthalMcHandleOkRes(BthalMcContext *context);

static int BthalMcHandleNoCarrierRes(BthalMcContext *context);

static int BthalMcHandleBusyRes(BthalMcContext *context);

static int BthalMcHandleNoAnswerRes(BthalMcContext *context);

static int BthalMcHandleExtErrorRes(BthalMcContext *context, const T_BAT_res_plus_ext_error *extError);

static int BthalMcHandleCmeErrorRes(BthalMcContext *context, const T_BAT_res_plus_cme_error *cmeError);

static int BthalMcHandleCindRes(BthalMcContext *context, const T_BAT_res_que_plus_cind* cind);

static int BthalMcHandleClccRes(BthalMcContext *context, const T_BAT_res_que_plus_clcc* clcc);

static int BthalMcHandleCopsRes(BthalMcContext *context, const T_BAT_res_que_plus_cops* cops);

static int BthalMcHandleCnumRes(BthalMcContext *context, const T_BAT_res_set_plus_cnum *cnum);

static int BthalMcHandleCpbsRes(BthalMcContext *context, const T_BAT_res_que_plus_cpbs *cpbs);

static int BthalMcHandleCpbrRes(BthalMcContext *context, const T_BAT_res_set_plus_cpbr *cpbr);

static int BthalMcHandleTestCpbrRes(BthalMcContext *context, const T_BAT_res_tst_plus_cpbr *cpbr);

static int BthalMcHandleCpbfRes(BthalMcContext *context, const T_BAT_res_set_plus_cpbf *cpbf);

static int BthalMcHandleReadCscs(BthalMcContext *context, const T_BAT_res_que_plus_cscs *cscs);

static const char* BthalMcBatRetToString(T_BAT_return retVal);

static BthalMcContext *BthalMcClientToContext(T_BAT_client client);

static int BthalMcDialLastNumber(BthalMcContext *context, T_BAT_cmd_response * res);

static void BthalMcDialNumber(BthalMcContext *context, const BTHAL_U8 *dialString, BTHAL_U8 length);

static void BthalMcSendClipEvent(void);

static void BthalMcUpdateIndicator(HfgIndicator indicator, BTHAL_U8 value);

static void BthalMcSendAllIndicatorsToUser(BthalMcContext *context);

static BTHAL_U8 BthalMcGetIndicatorValue(HfgIndicator ind);

static void BthalMcSetIndicatorValue(HfgIndicator ind, BTHAL_U8 value);

static void BthalMcReportUnsResult(BthalMcEvent *event);

static void BthalMcReportModuleError(BthalMcContext *context, HfgCmeError errCode);

static void BthalMcPutActiveCallOnHold(BthalMcContext *context);

/* 
 *	Call state machine functions.
 */

static void BthalMcInitCallStateMachine(void);
static void BthalMcCallStateMachine(BTHAL_U32 callIndex, BthalMcCallEvent event);

static BTHAL_U32 BthalMcNumOfCallsInState(BthalMcCallState state);

static BTHAL_U32 BthalMcFindCallInState(BthalMcCallState state);

static BthalStatus BthalMcCallStateMachineIllegalEvent(BTHAL_U32 callIndex);

static BthalStatus BthalMcCallStateIdleIncoming(BTHAL_U32 callIndex);
static BthalStatus BthalMcCallStateIdleOutgoing(BTHAL_U32 callIndex);
static BthalStatus BthalMcCallStateIncomingIdle(BTHAL_U32 callIndex);
static BthalStatus BthalMcCallStateIncomingActive(BTHAL_U32 callIndex);
static BthalStatus BthalMcCallStateOutgoingIdle(BTHAL_U32 callIndex);
static BthalStatus BthalMcCallStateOutgoingAlerting(BTHAL_U32 callIndex);
static BthalStatus BthalMcCallStateOutgoingActive(BTHAL_U32 callIndex);
static BthalStatus BthalMcCallStateAlertingIdle(BTHAL_U32 callIndex);
static BthalStatus BthalMcCallStateAlertingActive(BTHAL_U32 callIndex);
static BthalStatus BthalMcCallStateActiveIdle(BTHAL_U32 callIndex);
static BthalStatus BthalMcCallStateActiveHeld(BTHAL_U32 callIndex);
static BthalStatus BthalMcCallStateHeldIdle(BTHAL_U32 callIndex);
static BthalStatus BthalMcCallStateHeldActive(BTHAL_U32 callIndex);

/************************************************************************************
 *
 * Functions definitions:
 *
 ***********************************************************************************/

BthalStatus BTHAL_MC_Init(BthalCallBack	callback)
{
	BthalStatus status;

#ifdef BTHAL_MC_REPORT_BAT
	BTHAL_MC_Report_bat_start();
#endif /* BTHAL_MC_REPORT_BAT */


	BthalMcInitModuleData();

	status = BTHAL_OS_CreateTimer(BTHAL_OS_TASK_HANDLE_STACK, "BTHAL_MC_RING_CLIP_TIMER", &bthalMcRingClipTimerIndex);
	if (BTHAL_STATUS_SUCCESS != status)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL_MC: BTHAL_OS_CreateTimer() failed: %s", BTHAL_StatusName(status)));
		return BTHAL_STATUS_FAILED;
	}

	status = BTHAL_OS_CreateSemaphore("BTHAL MC AT Commands queue semaphore", &moduleData.queSemaphore);
	if (BTHAL_STATUS_SUCCESS != status)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL_MC: BTHAL_OS_CreateSemaphore() failed: %s", BTHAL_StatusName(status)));
		return BTHAL_STATUS_FAILED;
	}

	BthalMcInitCallStateMachine();

	status = BthalMcInitBat();
	if (BTHAL_STATUS_SUCCESS != status)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL_MC: BthalMcInitBat() failed: %s", BTHAL_StatusName(status)));
		return BTHAL_STATUS_FAILED;
	}
	
	return status;
}

BthalStatus BTHAL_MC_Deinit(void)
{
	BthalStatus status = BTHAL_STATUS_SUCCESS;

	status = BthalMcDeinitBat();
	if (BTHAL_STATUS_SUCCESS != status)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: BthalMcDeinitBat() failed: %s", BTHAL_StatusName(status)));
		status = BTHAL_STATUS_FAILED;
	}

	status = BTHAL_OS_DestroyTimer(bthalMcRingClipTimerIndex);
	if (BTHAL_STATUS_SUCCESS != status)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL_MC: BTHAL_OS_DestroyTimer() failed: %s", BTHAL_StatusName(status)));
		status =  BTHAL_STATUS_FAILED;
	}

	status = BTHAL_OS_DestroySemaphore(moduleData.queSemaphore);
	if (BTHAL_STATUS_SUCCESS != status)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL_MC: BTHAL_OS_DestroySemaphore() failed: %s", BTHAL_StatusName(status)));
		status =  BTHAL_STATUS_FAILED;
	}

	return status;
}

BthalStatus BTHAL_MC_Register(BthalMcCallback callback,
							  void *userData,
							  BthalMcContext **context)
{
	BTHAL_U32 nContexts;
	BthalMcEvent event;

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_Register"));

	/* Find a free BthalMcContext structure */
	*context = moduleData.contextsMem;
	for (nContexts = BTHAL_MC_NUM_OF_USERS; nContexts > 0; --nContexts)
	{
		BthalMcContextSettings settings = (*context)->settings;
		if ( 0 == (settings & BTHAL_MC_CONTEXT_SETTINGS_USED))
		{
			break;
		}
		++*context;
	}
	if (0 == nContexts)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL_MC: No free BthalMcContexts"));
		*context = 0;
		return BTHAL_STATUS_NO_RESOURCES;
	}

	/* Initialize context's fields */
	(*context)->settings = BTHAL_MC_CONTEXT_SETTINGS_USED | BTHAL_MC_CONTEXT_SETTINGS_IND_REPORTING;
	(*context)->callback = callback;
	(*context)->userData = userData;
	(*context)->responseHold = BTHAL_FALSE;

	/* Notify the user that registration is successful */
	event.type = BTHAL_MC_EVENT_MODEM_LINK_ESTABLISHED;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = *context;
	event.userData = userData;
	callback(&event);

	/* Notify the user of all current indicators */
	BthalMcSendAllIndicatorsToUser(*context);
	
	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_Unregister(BthalMcContext **context)
{
	BthalMcEvent event;

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_Unregister"));

	/* Check if the context is registered */
	if (0 == (BTHAL_MC_CONTEXT_SETTINGS_USED & (*context)->settings))
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: In BTHAL_MC_Unregister() context argument is not registered"));
		return BTHAL_STATUS_INVALID_PARM;
	}

	/* Notify the user about the success of the operation */
	event.type = BTHAL_MC_EVENT_MODEM_LINK_RELEASED;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = *context;
	event.userData = (*context)->userData;
	(*context)->callback(&event);

	/* Reset context's fields */
	(*context)->settings = 0;
	(*context)->callback = 0;
	(*context)->userData = 0;
	*context = 0;

	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_AnswerCall(BthalMcContext *context)
{
	T_BAT_cmd_set_percent_chld *param;
	
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_AnswerCall"));
	
	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);
	
	if(0 == isCallWaiting)
	{
	context->atCmd.cmd.ctrl_params = BAT_CMD_AT_A;
	}
	else
	{
		param = (T_BAT_cmd_set_percent_chld*)(&context->atCmd.paramMem);
		param->x = (BTHAL_S16)BAT_PARAMETER_NOT_PRESENT;
		param->n = BAT_P_CHLD_N_HLDACTANDACPT;
		context->atCmd.cmd.ctrl_params = BAT_CMD_SET_PERCENT_CHLD;
	}

	BthalMcCmdQuePushBack(context);
	BthalMcSendCommand();
	
	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_DialNumber(BthalMcContext *context, const BTHAL_U8 * number, BTHAL_U8 length)
{
	BTHAL_U8 dialString[BAT_MAX_D_DIAL_LEN+1];

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_DialNumber"));
	
	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);
	
	if (0 == number)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: BTHAL_MC_DialNumber - Null number was sent"));
		return BTHAL_STATUS_FAILED;
	}

	/* last character ';' */
	if (length > BAT_MAX_D_DIAL_LEN-1)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: Dial number length is too long, several digits are ignored"));
		length = BAT_MAX_D_DIAL_LEN-1;
	}
	
	memcpy(dialString, number, length);
	dialString[length++] = ';';
	dialString[length] = 0;

	BthalMcDialNumber(context, dialString, length);
	
	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_DialMemory(BthalMcContext *context,
								const BTHAL_U8 *number,
								BTHAL_U8 length)
{
	BTHAL_U8 dialString[BAT_MAX_D_DIAL_LEN+1];

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_DialMemory"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);
	
	if (0 == number)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL_MC: BTHAL_MC_DialMemory - Null number was sent"));
		return BTHAL_STATUS_FAILED;
	}

	/* first character: '>' and last character: ';' */
	if (length > BAT_MAX_D_DIAL_LEN-2)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: Number length is too long"));
		length = BAT_MAX_D_DIAL_LEN-2;
	}

	dialString[0] = '>';
	memcpy(&dialString[1], number, length);
	dialString[length+1] = ';';
	dialString[length+2] = 0;
	length += 2;

	BthalMcDialNumber(context, dialString, length);
	
	return  BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_HangupCall(BthalMcContext *context)
{
	BTHAL_U32 activeCalls, heldCalls, outgoingCalls, incomingCalls;
	T_BAT_cmd_set_percent_chld *param;
	
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_HangupCall"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);
	
	activeCalls = BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_ACTIVE);
	heldCalls = BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_HELD);
	incomingCalls = BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_INCOMING);
	outgoingCalls = BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_OUTGOING) || 
					BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_ALERTING);
	
	param = (T_BAT_cmd_set_percent_chld*)(&context->atCmd.paramMem);
	param->x = (BTHAL_S16)BAT_PARAMETER_NOT_PRESENT;
	context->atCmd.cmd.ctrl_params = BAT_CMD_SET_PERCENT_CHLD;

	
	/*No active or held calls; only incoming or outgoing*/
	if((0 == activeCalls) && (0 == heldCalls))
	{
		context->atCmd.cmd.ctrl_params = BAT_CMD_AT_H;
	}
	/*Outgoing call in progress and another call (held/active) exist*/
	else if(0 < outgoingCalls)
	{
		param->n = BAT_P_CHLD_N_RELDIALCALL;
	}
	/*Incoming call and another call (held/active) exist*/
	else if(0 < incomingCalls)
	{
		param->n = BAT_P_CHLD_N_RELHLDORUDUB;
	}
	/*One call exist, it's held*/
	else if((0 < heldCalls) && (0 == activeCalls))
	{
		param->n = BAT_P_CHLD_N_RELHLDORUDUB;
	}
	/*One call active, one call on hold */
	else if((0 < heldCalls) && (0 < activeCalls))
	{
		param->n = BAT_P_CHLD_N_RELACTANDACPT;
	}
	/*Two active calls exist; terminates the conference*/
	else if(2 == activeCalls)
	{
		param->n = BAT_P_CHLD_N_RELACTANDACPT;
	}
	/*One active call only exist*/
	else
	{
		context->atCmd.cmd.ctrl_params = BAT_CMD_AT_H;
	}

	BthalMcCmdQuePushBack(context);
	BthalMcSendCommand();

	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_GenerateDTMF(BthalMcContext *context,
								  BTHAL_I32 dtmf)
{
	T_BAT_cmd_set_plus_vts *param;

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_GenerateDTMF"));
	
	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);
	
	param = (T_BAT_cmd_set_plus_vts*)(&context->atCmd.paramMem);
	param->dtmf = (T_BAT_dtmf)dtmf;
	context->atCmd.cmd.ctrl_params = BAT_CMD_SET_PLUS_VTS;

	BthalMcCmdQuePushBack(context);
	BthalMcSendCommand();

	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_DialLastNumber(BthalMcContext *context)
{
	const char *dialString = ">LD 1;";
	
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_DialLastNumber"));
	
	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	BthalMcDialNumber(context, (const BTHAL_U8*)dialString, (BTHAL_U8)strlen(dialString));
	
	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_HandleCallHoldAndMultiparty(BthalMcContext *context,
												 const HfgHold *holdOp)
{
	T_BAT_cmd_set_percent_chld *param;
	BTHAL_BOOL legalAction = TRUE;
	BthalStatus status = BT_STATUS_PENDING;

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_HandleCallHoldAndMultiparty"));
	
	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	param = (T_BAT_cmd_set_percent_chld*)(&context->atCmd.paramMem);
	switch (holdOp->action)
	{
	case HFG_HOLD_RELEASE_HELD_CALLS:
		param->n = BAT_P_CHLD_N_RELHLDORUDUB;
		break;

	case  HFG_HOLD_RELEASE_ACTIVE_CALLS:
		param->n = BAT_P_CHLD_N_RELACTANDACPT;
		break;

	case HFG_HOLD_HOLD_ACTIVE_CALLS:
		param->n = BAT_P_CHLD_N_HLDACTANDACPT;
		break;

	case HFG_HOLD_ADD_HELD_CALL:
		param->n = BAT_P_CHLD_N_ADDHLD;
		break;

	case HFG_HOLD_EXPLICIT_TRANSFER:
		param->n = BAT_P_CHLD_N_ECT;
		break;

	default:
		legalAction = BTHAL_FALSE;
		break;

	}
	if (BTHAL_TRUE == legalAction)
	{
		if (0 == holdOp->index)
		{
			param->x = (BTHAL_S16)BAT_PARAMETER_NOT_PRESENT;
		}
		else
		{
			param->x = (BTHAL_S16)holdOp->index;
		}
		context->atCmd.cmd.ctrl_params = BAT_CMD_SET_PERCENT_CHLD;
		BthalMcCmdQuePushBack(context);
		BthalMcSendCommand();
	}
	else
	{
		BTHAL_MODEM_LOG_ERROR(("Illegal value for holdOp->action: %d",(int)holdOp->action));
		status = BTHAL_STATUS_INVALID_PARM;
	}
	
	return status;
}

BthalStatus BTHAL_MC_SetNetworkOperatorStringFormat(BthalMcContext *context)
{
	T_BAT_cmd_set_plus_cops *param;

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_SetNetworkOperatorStringFormat"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);
	
	param = (T_BAT_cmd_set_plus_cops*)(&context->atCmd.paramMem);
	param->mode = BAT_COPS_MODE_SET_ONLY;
	param->format = BAT_COPS_FORMAT_LONG;
	context->atCmd.cmd.ctrl_params = BAT_CMD_SET_PLUS_COPS;

	BthalMcCmdQuePushBack(context);
	BthalMcSendCommand();
	
	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_SetExtendedErrors(BthalMcContext *context,
									   BTHAL_BOOL enabled)
{
	BthalMcEvent event;

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_SetExtendedErrors"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	if (BTHAL_TRUE == enabled)
	{
		BTHAL_MODEM_LOG_INFO(("BTHAL MC: Extended errors are enabled"));
		context->settings |= BTHAL_MC_CONTEXT_SETTINGS_EXTENDED_ERR;
	}
	else
	{
		BTHAL_MODEM_LOG_INFO(("BTHAL MC: Extended errors are disabled"));
		context->settings &= ~BTHAL_MC_CONTEXT_SETTINGS_EXTENDED_ERR;
	}
	
	event.type = BTHAL_MC_EVENT_OK;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = context;
	event.userData = context->userData;
	context->callback(&event);
	
	return BT_STATUS_PENDING;
}

BthalStatus BTHAL_MC_SetClipNotification(BthalMcContext *context,
									BTHAL_BOOL enabled)
{
	BthalMcEvent event;

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_SetClipNotification"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	if (BTHAL_TRUE == enabled)
	{
		context->settings |= BTHAL_MC_CONTEXT_SETTINGS_CLIP;
	}
	else
	{
		context->settings &= ~BTHAL_MC_CONTEXT_SETTINGS_CLIP;
	}
	
	event.type = BTHAL_MC_EVENT_OK;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = context;
	event.userData = context->userData;
	context->callback(&event);

	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_SetIndicatorEventsReporting(BthalMcContext *context,
												BTHAL_BOOL enabled)
{
	BthalMcEvent event;

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_SetIndicatorEventsReporting"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	if (BTHAL_TRUE == enabled)
	{
		context->settings |= BTHAL_MC_CONTEXT_SETTINGS_IND_REPORTING;
	}
	else
	{
		context->settings &= ~BTHAL_MC_CONTEXT_SETTINGS_IND_REPORTING;
	}
	
	event.type = BTHAL_MC_EVENT_OK;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = context;
	event.userData = context->userData;
	context->callback(&event);
	
	return BT_STATUS_PENDING;
}

BthalStatus BTHAL_MC_RequestCallHoldAndMultipartyOptions(BthalMcContext *context)
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_SetIndicatorEventsReporting"));
	
	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	/* TODO: */

	BTHAL_MODEM_LOG_ERROR(("BTHAL_MC_RequestCallHoldAndMultipartyOptions is not implemented!"));
	
	return BTHAL_STATUS_FAILED;
}

BthalStatus BTHAL_MC_RequestCurrentIndicatorsValue(BthalMcContext *context)
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_RequestCurrentIndicatorsValue"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);
	
	context->atCmd.cmd.ctrl_params = BAT_CMD_QUE_PLUS_CIND;

	BthalMcCmdQuePushBack(context);
	BthalMcSendCommand();

	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_RequestSupportedIndicatorsRange(BthalMcContext *context)
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_RequestSupportedIndicatorsRange"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	/* TODO */

	BTHAL_MODEM_LOG_ERROR(("BTHAL_MC_RequestSupportedIndicatorsRange is not implemented!"));

	return BTHAL_STATUS_FAILED;
}

BthalStatus BTHAL_MC_RequestCurrentCallsList(BthalMcContext *context)
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_RequestCurrentCallsList"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);
	
	context->atCmd.cmd.ctrl_params = BAT_CMD_QUE_PLUS_CLCC;

	BthalMcCmdQuePushBack(context);
	BthalMcSendCommand();
	
	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_RequestNetworkOperatorString(BthalMcContext *context)
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_RequestNetworkOperatorString"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);
	
	context->atCmd.cmd.ctrl_params = BAT_CMD_QUE_PLUS_COPS;

	BthalMcCmdQuePushBack(context);
	BthalMcSendCommand();
	
	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_RequestSubscriberNumberInformation(BthalMcContext *context)
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_RequestSubscriberNumberInformation"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	context->atCmd.cmd.ctrl_params = BAT_CMD_SET_PLUS_CNUM;
	
	BthalMcCmdQuePushBack(context);
	BthalMcSendCommand();

	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_RequestResponseAndHoldStatus(BthalMcContext *context)
{
	BthalMcEvent event;
	
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_RequestResponseAndHoldStatus"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	if (TRUE == context->responseHold)
	{
		event.type = BTHAL_MC_EVENT_READ_RESPONSE_AND_HOLD_RES;
		event.status = BTHAL_STATUS_SUCCESS;
		event.context = context;
		event.userData = context->userData;
		event.p.respHold = HFG_RESP_HOLD_STATE_HOLD;
		context->callback(&event);
	}

	event.type = BTHAL_MC_EVENT_OK;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = context;
	event.userData = context->userData;
	context->callback(&event);

	BTHAL_MODEM_LOG_ERROR(("BTHAL_MC_RequestResponseAndHoldStatus is not implemented!"));

	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_SetResponseAndHold(BthalMcContext *context,
										   HfgResponseHold option)
{
	T_BAT_cmd_set_percent_chld *param;
	
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_SetResponseAndHold"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);


	if (HFG_RESP_HOLD_STATE_HOLD == option)
	{
		context->responseHold = BTHAL_TRUE;
		return BTHAL_MC_AnswerCall(context);
	}

	param = (T_BAT_cmd_set_percent_chld*)(&context->atCmd.paramMem);
	param->x = (BTHAL_S16)BAT_PARAMETER_NOT_PRESENT;
	context->atCmd.cmd.ctrl_params = BAT_CMD_SET_PERCENT_CHLD;

	switch (option)
	{
	case HFG_RESP_HOLD_STATE_ACCEPT:
		param->n = BAT_P_CHLD_N_ADDHLD;
		break;
		
	case HFG_RESP_HOLD_STATE_REJECT:
		param->n = BAT_P_CHLD_N_RELHLDORUDUB;
		break;

	default:
		BTHAL_MODEM_LOG_ERROR(("BTHAL_MC_HandleResponseAndHold() Illegal option "));
		return BTHAL_STATUS_INVALID_PARM;
	}

	BthalMcCmdQuePushBack(context);
	BthalMcSendCommand();

	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_SetPhonebook(BthalMcContext *context,
										AtPbStorageType phonebook)
{
	T_BAT_cmd_set_plus_cpbs *param;
	BTHAL_BOOL legalPhonebook = BTHAL_TRUE;
	BthalStatus status = BTHAL_STATUS_PENDING;
	
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_SetPhonebook"));
	
	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	param = (T_BAT_cmd_set_plus_cpbs*)(&context->atCmd.paramMem);
	switch (phonebook)
	{
	case AT_PBS_DIALED_CALLS:
		param->storage = BAT_STORAGE_DC;
		break;

	case  AT_PBS_FIXED_DIAL:
		param->storage = BAT_STORAGE_FD;
		break;

	case AT_PBS_LAST_DIAL:
		param->storage = BAT_STORAGE_DC;
		break;

	case AT_PBS_MISSED_CALLS:
		param->storage = BAT_STORAGE_MC;
		break;

	case AT_PBS_ME_PHONEBOOK:
		param->storage = BAT_STORAGE_AD;
		break;

	case AT_PBS_ME_SIM_COMBINED:
		param->storage = BAT_STORAGE_AF;
		break;

	case AT_PBS_RECEIVED_CALLS:
		param->storage = BAT_STORAGE_RC;
		break;

	case AT_PBS_SIM_PHONEBOOK:
	default:
		legalPhonebook = BTHAL_FALSE;		
		break;
	}

	if (BTHAL_TRUE == legalPhonebook)
	{
		context->atCmd.cmd.ctrl_params = BAT_CMD_SET_PLUS_CPBS;
		BthalMcCmdQuePushBack(context);
		BthalMcSendCommand();
	}
	else
	{
		BTHAL_MODEM_LOG_ERROR(("Illegal phonebook value: 0x%x", (unsigned)phonebook));
		status = BTHAL_STATUS_INVALID_PARM;
	}
	
	return status;
}

BthalStatus BTHAL_MC_RequestSelectedPhonebook(BthalMcContext *context)
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_RequestSelectedPhonebook"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);
	
	context->atCmd.cmd.ctrl_params = BAT_CMD_QUE_PLUS_CPBS;
	BthalMcCmdQuePushBack(context);
	BthalMcSendCommand();

	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_RequestSupportedPhonebooks(BthalMcContext *context)
{
	BthalMcEvent event;
	
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_RequestSupportedPhonebooks"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	context->settings |= BTHAL_MC_CONTEXT_SETTINGS_PROCESSING;
	
	event.p.supportedPhonebooks =	AT_PBS_RECEIVED_CALLS |
								AT_PBS_ME_SIM_COMBINED |
								AT_PBS_ME_PHONEBOOK |
								AT_PBS_MISSED_CALLS |
								AT_PBS_LAST_DIAL |
								AT_PBS_FIXED_DIAL |
								AT_PBS_DIALED_CALLS;
	
	event.type = BTHAL_MC_EVENT_SUPPORTED_PHONEBOOKS;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = context;
	event.userData = context->userData;
	context->callback(&event);
	
	event.type = BTHAL_MC_EVENT_OK;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = context;
	event.userData = context->userData;
	context->callback(&event);

	BTHAL_MC_SET_CONTEXT_NOT_PROCESSING(context);
	
	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_ReadPhonebook(BthalMcContext *context,
											BTHAL_U16 index1,
											BTHAL_U16 index2)
{
	T_BAT_cmd_set_plus_cpbr *param;

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_ReadPhonebook"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	param = (T_BAT_cmd_set_plus_cpbr*)&context->atCmd.paramMem;
	param->index1 = (BTHAL_U8)index1;
	param->index2 = (BTHAL_S16)index2;

	context->atCmd.cmd.ctrl_params = BAT_CMD_SET_PLUS_CPBR;
	BthalMcCmdQuePushBack(context);
	BthalMcSendCommand();

	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_RequestPhonebookSupportedIndices(BthalMcContext *context)
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_RequestPhonebookSupportedIndices"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	context->atCmd.cmd.ctrl_params = BAT_CMD_TST_PLUS_CPBR;
	BthalMcCmdQuePushBack(context);
	BthalMcSendCommand();

	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_MC_FindPhonebook(BthalMcContext *context,
											const char *findText)
{
	T_BAT_cmd_set_plus_cpbf *param;
	BthalStatus status = BTHAL_STATUS_PENDING;
	BTHAL_U32 length;
	
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_FindPhonebook"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	param = (T_BAT_cmd_set_plus_cpbf*)&context->atCmd.paramMem;

	length = OS_StrLen(findText);
	if (BAT_MAX_CPBF_FIND_TEXT_LEN > length)
	{
		param->c_findtext = (U8)length;
		OS_MemCopy(param->findtext, findText, length);
		context->atCmd.cmd.ctrl_params = BAT_CMD_SET_PLUS_CPBF;
		BthalMcCmdQuePushBack(context);
		BthalMcSendCommand();
	}
	else
	{
		BTHAL_MODEM_LOG_ERROR(("Argument: 'findText' is too long"));
		status = BTHAL_STATUS_INVALID_PARM;
	}

	return status;
}

BthalStatus BTHAL_MC_SetCharSet(BthalMcContext *context,
							const char *charSet)
{
	T_BAT_cmd_set_plus_cscs *param;
	BthalStatus status = BTHAL_STATUS_PENDING;
	BTHAL_BOOL legalCharSet = BTHAL_TRUE;

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_SetCharSet"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	param = (T_BAT_cmd_set_plus_cscs*)&context->atCmd.paramMem;

	BTHAL_MODEM_LOG_INFO(("received charset: %s", charSet));

	if (0 == OS_StrCmp(charSet, "IRA"))
	{
		param->cs = BAT_CSCS_CS_IRA;
	}
	else if (0 == OS_StrCmp(charSet, "PCDN"))
	{
		param->cs = BAT_CSCS_CS_PCDN;
	}
	else if (0 == OS_StrCmp(charSet, "8859-1"))
	{
		param->cs = BAT_CSCS_CS_8859_1;
	}
	else if (0 == OS_StrCmp(charSet, "PCCP437"))
	{
		param->cs = BAT_CSCS_CS_PCCP_437;
	}
	else if (0 == OS_StrCmp(charSet, "GSM"))
	{
		param->cs = BAT_CSCS_CS_GSM;
	}
	else if (0 == OS_StrCmp(charSet, "HEX"))
	{
		param->cs = BAT_CSCS_CS_HEX;
	}
	else if (0 == OS_StrCmp(charSet, "UCS2"))
	{
		param->cs = BAT_CSCS_CS_UCS2;
	}
	else
	{
		legalCharSet = BTHAL_FALSE;
	}

	if (BTHAL_TRUE == legalCharSet)
	{
		context->atCmd.cmd.ctrl_params = BAT_CMD_SET_PLUS_CSCS;
		BthalMcCmdQuePushBack(context);
		BthalMcSendCommand();
	}
	else
	{
		status = BTHAL_STATUS_INVALID_PARM;
		BTHAL_MODEM_LOG_ERROR(("Argument: 'charSet' is invalid"));
	}

	return status;
}

BthalStatus BTHAL_MC_RequestSelectedCharSet(BthalMcContext *context)
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_RequestSelectedCharSet"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);

	context->atCmd.cmd.ctrl_params = BAT_CMD_QUE_PLUS_CSCS;
	BthalMcCmdQuePushBack(context);
	BthalMcSendCommand();

	return BTHAL_STATUS_PENDING;

}

BthalStatus BTHAL_MC_RequestSupportedCharSets(BthalMcContext *context)
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_RequestSupportedCharSets"));

	BTHAL_MC_VERIFY_CONTEXT_IDLE(context);
	
	/* TODO */

	return BTHAL_STATUS_FAILED;
}

static void BthalMcCmdQuePushBack(BthalMcContext *context)
{
	BTHAL_OS_LockSemaphore(moduleData.queSemaphore, 0);
	
	InsertTailList(&moduleData.cmdQue, &context->node);

	BTHAL_OS_UnlockSemaphore(moduleData.queSemaphore);
}

static void BthalMcCmdQuePopHead(void)
{
	BTHAL_OS_LockSemaphore(moduleData.queSemaphore, 0);

	if (!IsListEmpty(&moduleData.cmdQue))
	{
		(void)RemoveHeadList(&moduleData.cmdQue);
	}
	
	BTHAL_OS_UnlockSemaphore(moduleData.queSemaphore);
}

static BthalMcContext* BthalMcCmdQueGetHead(void)
{
	BthalMcContext *context = 0;
	
	BTHAL_OS_LockSemaphore(moduleData.queSemaphore, 0);

	if (!IsListEmpty(&moduleData.cmdQue))
	{
		context = (BthalMcContext*)GetHeadList(&moduleData.cmdQue);
	}

	BTHAL_OS_UnlockSemaphore(moduleData.queSemaphore);

	return context;
}

static void BthalMcInitModuleData(void)
{
	T_BAT_config *config;
	BTHAL_U32 nContexts;
	BthalMcContext *context;
	HfgIndicator indicator;

	config = &moduleData.config;
	config->adapter.gdd_if = gdd_func_dio;
	config->adapter.cap.dio_cap.mtu_size = 300; /*GDD_DIO_MTU_SIZE*/
	config->device = DEVICE_PACKET;
	config->l2p.protocol_id = L2P_SP_PSI;

	context = moduleData.contextsMem;
	for (nContexts = BTHAL_MC_NUM_OF_USERS; nContexts > 0; --nContexts)
	{
		context->settings = 0;
		context->atCmd.cmd.params.ptr_set_percent_als = (void*)(&context->atCmd.paramMem);
		++context;
	}

	moduleData.actualSignalLevel = 0;
	moduleData.isRingClipTimerScheduled = BTHAL_FALSE;
	InitializeListHead(&moduleData.cmdQue);

	for (indicator = BTHAL_MC_NUM_OF_HFG_INDICATORS; indicator > 0; --indicator)
	{
		BthalMcSetIndicatorValue(indicator, 0);
	}
	/* Until the real battery power indicator can be accessed */
	BthalMcSetIndicatorValue(HFG_IND_BATTERY, 5);
}

static BthalStatus BthalMcInitBat(void)
{
	T_BAT_return retVal;
	U32 sleepCounter = 0;
	
	retVal = bat_init((void *)moduleData.batInstBuf, BTHAL_MC_NUM_OF_BAT_INSTANCES);
	if (BAT_OK != retVal)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: bat_init() returned: %s", BthalMcBatRetToString(retVal)));
		return BTHAL_STATUS_FAILED;
	}

	/* The function bat_new() may return BAT_ERROR while the modem is not ready yet during init sequence, 
	since BTT might have higher priority over modem task. So, we wait here until modem is ready. */
	while ((retVal = bat_new(&moduleData.instance, (void *)moduleData.batClientBuf, BTHAL_MC_NUM_OF_BAT_CLIENTS,
				     &moduleData.config, BthalMcNewBatInstanceSigCb)) != BAT_OK)
	{
		sleepCounter++;
		BTHAL_OS_Sleep(100);

		/* Wait at most 10 sec */
		if (sleepCounter >= 300)
			break;
	}
	
	if (BAT_OK != retVal)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: bat_new() returned: %s", BthalMcBatRetToString(retVal)));
		bat_deinit();
		return BTHAL_STATUS_FAILED;
	}

	return BTHAL_STATUS_SUCCESS;
}

static BthalStatus BthalMcDeinitBat(void)
{
	T_BAT_return retVal;
	BthalStatus status = BTHAL_STATUS_SUCCESS;

	retVal = bat_delete(moduleData.instance);

	if (BAT_OK != retVal)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: bat_delete() failed: %s", BthalMcBatRetToString(retVal)));
		status = BTHAL_STATUS_FAILED;
	}

	retVal = bat_deinit();
	if (BAT_OK != retVal)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: bat_deinit() failed: %s", BthalMcBatRetToString(retVal)));
		status = BTHAL_STATUS_FAILED;
	}

	return status;
}


static void BthalMcOpenBatClient(void)
{
	T_BAT_return result;

	result = bat_uns_open(moduleData.instance, &moduleData.unsClient, BthalMcUnsResultCb);

	if (BAT_OK != result)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: bat_uns_open failed: %s", BthalMcBatRetToString(result)));
		return;
	}
		
	result = bat_open(moduleData.instance, &moduleData.client, BthalMcClientRspCb, BthalMcClientSigCb);

	if (BAT_OK != result)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: bat_open failed: %s", BthalMcBatRetToString(result)));
		bat_close(moduleData.unsClient);
	}
}

static void BthalMcCloseBatClient(void)
{
	T_BAT_return result;

	result = bat_close(moduleData.unsClient);
	if (BAT_OK != result)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: bat_close() for BAT uns client failed: %s", BthalMcBatRetToString(result)));
	}

	result = bat_close(moduleData.client);
	if (BAT_OK != result)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: bat_close() for BAT client failed: %s", BthalMcBatRetToString(result)));
	}
}

static void BthalMcSendCommand(void)
{
	T_BAT_return res;
	BthalMcContext *context;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcSendCommand"));

	context = BthalMcCmdQueGetHead();
	if (0 == context)
	{
		return;
	}
	if (BTHAL_MC_CONTEXT_SETTINGS_PROCESSING & context->settings)
	{
		return;
	}
	
	res = bat_send(moduleData.client, &context->atCmd.cmd);

	switch (res)
	{
	case BAT_OK:
	case BAT_BUSY_RESOURCE:
		context->settings |= BTHAL_MC_CONTEXT_SETTINGS_PROCESSING;
		break;

	case BAT_ERROR:
	default:
		BthalMcReportModuleError(context, HFG_CME_UNKNOWN);
		BthalMcCmdQuePopHead();
		BTHAL_MC_SET_CONTEXT_NOT_PROCESSING(context);

		/* See if there are waiting commands in the queue */
		BthalMcSendCommand();
		break;
	}
}

static void BthalMcNewBatInstanceSigCb(T_BAT_signal sig)
{
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcNewBatInstanceSigCb"));
	
	BTHAL_MODEM_LOG_INFO(("BTHAL MC: BthalMcNewBatInstanceSigCb, sig = %d",sig));
	
	if (BAT_NEW_INSTANCE_SUCCEED == sig)
	{
		BthalMcOpenBatClient();
	}
	else
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: BAT new instance failed"));
	}
}

static void BthalMcClientSigCb(T_BAT_client client, T_BAT_signal sig)
{
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcClientSigCb"));
	
	BTHAL_MODEM_LOG_INFO(("BTHAL MC: BthalMcClientSigCb() sig = %d", sig));

	switch (sig)
	{
		case BAT_OPEN_CLIENT_SUCCEED:
			break;

		case BAT_OPEN_CLIENT_FAIL:
			BTHAL_MODEM_LOG_ERROR(("BTHAL MC: Failed to create new client"));
			if (BAT_OK != bat_close(moduleData.unsClient))
			{
				BTHAL_MODEM_LOG_ERROR(("BTHAL MC: bat_close() for BAT uns client failed"));
			}
			if (BTHAL_STATUS_SUCCESS != BthalMcDeinitBat())
			{
				BTHAL_MODEM_LOG_ERROR(("BTHAL MC: BthalMcDeinitBat failed"));
			}
			(void)BTHAL_OS_DestroySemaphore(moduleData.queSemaphore);
			(void)BTHAL_OS_DestroyTimer(bthalMcRingClipTimerIndex);
			break;

		case BAT_READY_RESOURCE:
			/* AT Commands can now be sent */
			break;

		default:
			break;
	}	
}

static int BthalMcUnsResultCb(T_BAT_client client, T_BAT_cmd_response *rsp)
{
	BTHAL_U8 res = BAT_OK;

	BTHAL_MODEM_LOG_INFO(("BTHAL MC: In BthalMcUnsResultCb - result is 0x%x", rsp->ctrl_response));
	
	switch (rsp->ctrl_response)
	{
		case BAT_RES_UNS_PLUS_CRING:
			res = BthalMcHandleRingUns();
			break;

		case BAT_RES_UNS_PLUS_CLIP:
			res = BthalMcHandleClipUns( rsp->response.ptr_res_plus_clip);
			break;

		case BAT_RES_UNS_PLUS_CCWA:
			res = BthalMcHandleCcwaUns(rsp->response.ptr_res_plus_ccwa);
			break;

		case BAT_RES_UNS_PERCENT_CPI:
			res = BthalMcHandleCpiUns(rsp->response.ptr_res_percent_cpi);
			break;

		case BAT_RES_UNS_PERCENT_CSQ:
			res = BthalMcHandleCsqUns(rsp->response.ptr_res_percent_csq);
			break;

		case BAT_RES_UNS_PERCENT_CREG:
			res = BthalMcHandleCregUns(rsp->response.ptr_res_percent_creg);
			break;

		default:
			break;
	}
	return res;
}

static int BthalMcClientRspCb(T_BAT_client client, T_BAT_cmd_response *rsp)
{
	int res = BAT_OK;
	BthalMcContext *context;
	BTHAL_BOOL checkForMoreCommands = BTHAL_FALSE;

	BTHAL_MODEM_LOG_INFO(("BTHAL MC: BthalMcClientRspCb. response is 0x%x", rsp->ctrl_response));

	context = BthalMcCmdQueGetHead();
	if (0 == context)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: In BthalMcClientRspCb and 'moduleData.cmdQue' is empty"));
		return BAT_ERROR;
	}

	switch (rsp->ctrl_response)
	{
	case BAT_RES_AT_OK:
		res = BthalMcHandleOkRes(context);
		checkForMoreCommands = BTHAL_TRUE;
		break;

	case BAT_RES_AT_NO_CARRIER_FINAL:
		res = BthalMcHandleNoCarrierRes(context);
		checkForMoreCommands = BTHAL_TRUE;
		break;

	case BAT_RES_AT_BUSY:
		res = BthalMcHandleBusyRes(context);
		checkForMoreCommands = BTHAL_TRUE;
		break;

	case BAT_RES_AT_NO_ANSWER:
		res = BthalMcHandleNoAnswerRes(context);
		checkForMoreCommands = BTHAL_TRUE;
		break;

	case BAT_RES_PLUS_CME_ERROR:
		res = BthalMcHandleCmeErrorRes(context, rsp->response.ptr_plus_cme_error);
		checkForMoreCommands = BTHAL_TRUE;
		break;

	case BAT_RES_PLUS_EXT_ERROR:
		res = BthalMcHandleExtErrorRes(context, rsp->response.ptr_plus_ext_error);
		checkForMoreCommands = BTHAL_TRUE;
		break;		

	case BAT_RES_QUE_PLUS_CIND:
		res = BthalMcHandleCindRes(context, rsp->response.ptr_que_plus_cind);
		break;

	case BAT_RES_QUE_PLUS_CLCC:
		res = BthalMcHandleClccRes(context, rsp->response.ptr_que_plus_clcc);
		break;

	case BAT_RES_QUE_PLUS_COPS:
		res = BthalMcHandleCopsRes(context, rsp->response.ptr_que_plus_cops);
		break;

	case BAT_RES_SET_PLUS_CNUM:
		res = BthalMcHandleCnumRes(context, rsp->response.ptr_set_plus_cnum);
		break;

	case BAT_RES_QUE_PLUS_CPBS:
		res = BthalMcHandleCpbsRes(context, rsp->response.ptr_que_plus_cpbs);
		break;

	case BAT_RES_SET_PLUS_CPBR:
		res = BthalMcHandleCpbrRes(context, rsp->response.ptr_set_plus_cpbr);
		break;

	case BAT_RES_TST_PLUS_CPBR:
		res = BthalMcHandleTestCpbrRes(context, rsp->response.ptr_tst_plus_cpbr);
		break;

	case BAT_RES_SET_PLUS_CPBF:
		res = BthalMcHandleCpbfRes(context, rsp->response.ptr_set_plus_cpbf);
		break;

	case BAT_RES_QUE_PLUS_CSCS:
		res = BthalMcHandleReadCscs(context, rsp->response.ptr_que_plus_cscs);
		break;
		
	default:
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: In BthalMcClientRspCb unhandled AT response"));
		Assert(0);
		break;
	}

	if (BTHAL_TRUE == checkForMoreCommands)
	{
		BthalMcCmdQuePopHead();
		BTHAL_MC_SET_CONTEXT_NOT_PROCESSING(context);		
		BthalMcSendCommand();
	}

	return res;
}

static int BthalMcHandleRingUns(void)
{
	BthalMcEvent event;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleRingUns"));

	event.type = BTHAL_MC_EVENT_RING;
	event.status = BTHAL_STATUS_SUCCESS;

	BthalMcReportUnsResult(&event);
	
	return BAT_OK;
}

static int BthalMcHandleClipUns(const T_BAT_res_uns_plus_clip *clip)
{
	BTHAL_U8 length;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleClipUns"));
	
	/* save fields for sending later for HF */
	length = (clip->c_number < BAT_MAX_CLIP_NUMBER_LEN ? clip->c_number : BAT_MAX_CLIP_NUMBER_LEN);	
	memcpy((void *)moduleData.clip_number, (const void *)clip->number, length);
	moduleData.clip_number[length] = 0;
	moduleData.clip_type = clip->type;

	BthalMcSendClipEvent();

	return BAT_OK;
}

static void BthalMcSendClipEvent(void)
{
	BthalMcEvent event;
	BTHAL_U32 nContexts;
	BthalMcContext *context;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcSendClipEvent"));
	
	event.type = BTHAL_MC_EVENT_CALLING_LINE_ID_NOTIFY;
	event.status = BTHAL_STATUS_SUCCESS;
	event.p.number.type = moduleData.clip_type;
	strcpy(event.p.number.number, (char*)moduleData.clip_number);

	context = moduleData.contextsMem;	
	for (nContexts = BTHAL_MC_NUM_OF_USERS; nContexts > 0; --nContexts)
	{
		if ((BTHAL_MC_CONTEXT_SETTINGS_USED & context->settings) &&
		    (BTHAL_MC_CONTEXT_SETTINGS_CLIP & context->settings))
		{
			event.context = context;
			event.userData = context->userData;
			context->callback(&event);
		}
		++context;
	}
}

static int BthalMcHandleCcwaUns(const T_BAT_res_uns_plus_ccwa *ccwa)
{
	BthalMcEvent event;
	BTHAL_U8 length;
	BTHAL_BOOL selectDefault;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleCcwaUns"));

	isCallWaiting = TRUE;
	
	event.type = BTHAL_MC_EVENT_CALL_WAIT_NOTIFY;
	event.status = BTHAL_STATUS_SUCCESS;
	event.p.callwait.type = ccwa->type;
	event.p.callwait.classmap = 0;

	selectDefault = BTHAL_MC_MAX_NUMBER_LENGTH < ccwa->c_number;
	length = selectDefault ? BTHAL_MC_MAX_NUMBER_LENGTH : ccwa->c_number;
	event.p.callwait.length = length;
	
	OS_MemCopy((void *)event.p.callwait.number, (const void *)ccwa->number, ccwa->c_number);
	event.p.callwait.number[length] = '\0';

	BthalMcReportUnsResult(&event);

	return BAT_OK;
}

static int BthalMcHandleCpiUns(const T_BAT_res_uns_percent_cpi *cpi)
{
	BTHAL_U32 callIndex;
	
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleCpiUns"));

	callIndex = cpi->cid - 1;
	
	BTHAL_MODEM_LOG_INFO(("BthalMcHandleCpiUns: callIndex %d, msgtype %d",
                                                    callIndex,
                                                    cpi->msgtype));
    
	switch (cpi->msgtype)
	{
	case BAT_P_CPI_TYPE_SETUP:
		BTHAL_MODEM_LOG_DEBUG(("BthalMcHandleCpiUns: CPI_TYPE_SETUP - dir %d, tch %d",
                                                            cpi->dir,
                                                            cpi->tch));
        
		if (( 1 == cpi->dir) && (BTHAL_MC_CALL_STATE_INCOMING != moduleData.currentCalls[callIndex]))
		{
			BthalMcCallStateMachine(callIndex, BTHAL_MC_CALL_EVENT_INCOMING);
		}
		break;
				
	case BAT_P_CPI_TYPE_DISC:
		if (1 == cpi->dir && BTHAL_TRUE == moduleData.isRingClipTimerScheduled)
		{
			BTHAL_OS_CancelTimer(bthalMcRingClipTimerIndex);
			moduleData.isRingClipTimerScheduled = BTHAL_FALSE;
		}
		/*Incase BAT doesn't provide RELEASE indication*/
		BthalMcCallStateMachine(callIndex, BTHAL_MC_CALL_EVENT_IDLE);
		break;
					
	case BAT_P_CPI_TYPE_ALERTING:
 		if (0 == cpi->dir)
		{
			BthalMcCallStateMachine(callIndex, BTHAL_MC_CALL_EVENT_ALERTING);
		}
		break;

	case BAT_P_CPI_TYPE_PROC:
 		if (0 == cpi->dir)
		{
			BthalMcCallStateMachine(callIndex, BTHAL_MC_CALL_EVENT_OUTGOING);
		}
		break;
		
	case BAT_P_CPI_TYPE_CONNECT:
		if (1 == cpi->dir && BTHAL_TRUE == moduleData.isRingClipTimerScheduled)
		{
			BTHAL_OS_CancelTimer(bthalMcRingClipTimerIndex);
			moduleData.isRingClipTimerScheduled = BTHAL_FALSE;
		}
		BthalMcCallStateMachine(callIndex, BTHAL_MC_CALL_EVENT_ACTIVE);
		break;

	case BAT_P_CPI_TYPE_RELEASE:
		if(BTHAL_MC_CALL_STATE_IDLE != moduleData.currentCalls[callIndex])
		{
		BthalMcCallStateMachine(callIndex, BTHAL_MC_CALL_EVENT_IDLE);
		}
		break;
		
	case BAT_P_CPI_TYPE_REJECT:
		if(BTHAL_MC_CALL_STATE_IDLE != moduleData.currentCalls[callIndex])
		{
		BthalMcCallStateMachine(callIndex, BTHAL_MC_CALL_EVENT_IDLE);
		}
		break;

	case BAT_P_CPI_TYPE_HELD:
		BthalMcCallStateMachine(callIndex, BTHAL_MC_CALL_EVENT_HELD);
		break;

	default:
		break;
	}

	return BAT_OK;
}

static int BthalMcHandleCsqUns(const T_BAT_res_uns_percent_csq *csq)
{
	BTHAL_S16 actLevel;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleCsqUns"));
	
	actLevel = csq->actlevel;
				
	if (moduleData.actualSignalLevel != actLevel &&
		actLevel >= BTHAL_MC_MIN_MODEM_SIGNAL_LEVEL &&
		actLevel <= BTHAL_MC_MAX_MODEM_SIGNAL_LEVEL)
	{
		moduleData.actualSignalLevel = actLevel;
		BthalMcUpdateIndicator(HFG_IND_SIGNAL, CONVERT_MODEM_SIGNAL_LEVEL_TO_SIG_SIGNAL_LEVEL(actLevel));
	}

	return BAT_OK;
}

static int BthalMcHandleCregUns(const T_BAT_res_uns_percent_creg *creg)
{
	BTHAL_BOOL registered = BTHAL_FALSE;
	BTHAL_BOOL roaming = BTHAL_FALSE;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleCregUns"));

	switch (creg->stat)
	{
		case BAT_P_CREG_STATUS_NO_SEARCH:
		case BAT_P_CREG_STATUS_SEARCH:
		case BAT_P_CREG_STATUS_DENIED:
		case BAT_P_CREG_STATUS_UNKNOWN:
			break;

		case BAT_P_CREG_STATUS_REG:
			registered = BTHAL_TRUE;
			break;
			
		case BAT_P_CREG_STATUS_ROAM:
			registered = BTHAL_TRUE;
			roaming = BTHAL_TRUE;
			break;

		default:
			break;
	}

	
	BthalMcUpdateIndicator(HFG_IND_SERVICE, registered);
	BthalMcUpdateIndicator(HFG_IND_ROAMING, roaming);
	
	return BAT_OK;
}

static int BthalMcHandleCnumRes(BthalMcContext *context, const T_BAT_res_set_plus_cnum *cnum)
{
	BthalMcEvent event;
	BTHAL_U8 length;
	BTHAL_BOOL selectDefault;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleCnumRes"));

	event.type = BTHAL_MC_EVENT_SUBSCRIBER_NUMBER_RESPONSE;
	event.status = BTHAL_STATUS_SUCCESS;
	event.p.subscriber.service = BTHAL_MC_SUBSCRIBER_SERVICE;
	event.p.subscriber.type = cnum->type;
	selectDefault = BTHAL_MC_MAX_NUMBER_LENGTH < cnum->c_number;
	length = selectDefault ? BTHAL_MC_MAX_NUMBER_LENGTH : cnum->c_number;
	event.p.subscriber.length = length;
	OS_MemCopy((void*)event.p.subscriber.number, (const void*)cnum->number, cnum->c_number);
	event.p.subscriber.number[length] = '\0';
	event.context = context;
	event.userData = context->userData;

	context->callback(&event);

	return BAT_OK;
}

static int BthalMcHandleCopsRes(BthalMcContext *context, const T_BAT_res_que_plus_cops *cops)
{
	BthalMcEvent event;
	BTHAL_U8 length;
	BTHAL_BOOL selectDefault;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleCopsRes"));

	event.type = BTHAL_MC_EVENT_NETWORK_OPERATOR_RESPONSE;
	event.status = BTHAL_STATUS_SUCCESS;
	event.p.oper.mode = cops->mode;
	event.p.oper.format = cops->format;

	selectDefault = BTHAL_MC_MAX_OPERATOR_NAME_LENGTH < cops->c_oper;
	length = selectDefault ? BTHAL_MC_MAX_OPERATOR_NAME_LENGTH: cops->c_oper;
	
	event.p.oper.length = length;
	OS_MemCopy((void*)event.p.oper.name, cops->oper, cops->c_oper);
	event.p.oper.name[length] = '\0';
	event.context = context;
	event.userData = context->userData;
	context->callback(&event);

	return BAT_OK;
}

static int BthalMcHandleClccRes(BthalMcContext *context, const T_BAT_res_que_plus_clcc* clcc)
{
	BthalMcEvent event;
	BTHAL_U8 length = 0;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleClccRes"));

	event.type = BTHAL_MC_EVENT_CURRENT_CALLS_LIST_RESPONSE;
	event.status = BTHAL_STATUS_SUCCESS;
	event.p.calllist.dir = clcc->dir;
	event.p.calllist.index = clcc->idx;
	event.p.calllist.mode = clcc->mode;
	event.p.calllist.multiParty = clcc->mpty;
	event.p.calllist.state = clcc->stat;
	event.p.calllist.type = clcc->type;
	if (clcc->v_number != 0)
	{
		length = (clcc->c_number < BAT_MAX_CLCC_NUMBER_LEN ? clcc->c_number : BAT_MAX_CLCC_NUMBER_LEN);	
	}

	event.p.calllist.length = length;
	memcpy((void*)event.p.calllist.number, (const void*)clcc->number, length);
	event.p.calllist.number[length] = 0;
	
	event.context = context;
	event.userData = context->userData;
	context->callback(&event);

	return BAT_OK;
}

static int BthalMcHandleCindRes(BthalMcContext *context, const T_BAT_res_que_plus_cind* cind)
{
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleCindRes"));

	/* TODO */
	BTHAL_MODEM_LOG_ERROR(("BthalMcHandleCindRes() is not implemented!"));
	
	return BAT_OK;
}

static int BthalMcHandleExtErrorRes(BthalMcContext *context, const T_BAT_res_plus_ext_error *extError)
{
	BthalMcEvent event;
	BTHAL_BOOL sendExtended;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleExtErrorRes"));

	sendExtended = (BTHAL_BOOL)(BTHAL_MC_CONTEXT_SETTINGS_EXTENDED_ERR & context->settings);
	event.type = sendExtended ? BTHAL_MC_EVENT_EXTENDED_ERROR : BTHAL_MC_EVENT_ERROR;
	event.status = BTHAL_STATUS_SUCCESS;
	event.p.err = HFG_CME_UNKNOWN;
	event.context = context;
	event.userData = context->userData;
	context->callback(&event);

	return BAT_OK;
}

static int BthalMcHandleCmeErrorRes(BthalMcContext *context, const T_BAT_res_plus_cme_error *cmeError)
{
	BthalMcEvent event;
	BTHAL_BOOL sendExtended;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleCmeErrorRes"));

	sendExtended = (BTHAL_BOOL)(BTHAL_MC_CONTEXT_SETTINGS_EXTENDED_ERR & context->settings);
	event.type = sendExtended ? BTHAL_MC_EVENT_EXTENDED_ERROR : BTHAL_MC_EVENT_ERROR;
	event.status = BTHAL_STATUS_SUCCESS;
	event.p.err = cmeError->error;
	event.context = context;
	event.userData = context->userData;
	context->callback(&event);

	return BAT_OK;
}

static int BthalMcHandleNoAnswerRes(BthalMcContext *context)
{
	BthalMcEvent event;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleNoAnswerRes"));

	event.type = BTHAL_MC_EVENT_NO_ANSWER;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = context;
	event.userData = context->userData;
	context->callback(&event);

	return BAT_OK;
}

static int BthalMcHandleBusyRes(BthalMcContext *context)
{
	BthalMcEvent event;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleBusyRes"));

	event.type = BTHAL_MC_EVENT_BUSY;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = context;
	event.userData = context->userData;
	context->callback(&event);

	return BAT_OK;
}

static int BthalMcHandleNoCarrierRes(BthalMcContext *context)
{
	BthalMcEvent event;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleNoCarrierRes"));

	event.type = BTHAL_MC_EVENT_NO_CARRIER;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = context;
	event.userData = context->userData;
	context->callback(&event);

	return BAT_OK;
}

static int BthalMcHandleOkRes(BthalMcContext *context)
{
	BthalMcEvent event;
	BTHAL_U32 heldCallIndex;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleOkRes"));

	if (context->responseHold == BTHAL_TRUE && BAT_CMD_SET_PERCENT_CHLD == context->atCmd.cmd.ctrl_params)
	{
		switch (context->atCmd.cmd.params.ptr_set_percent_chld->n)
		{
		/* Reject Held call */
		case BAT_P_CHLD_N_RELHLDORUDUB:
			context->responseHold = BTHAL_FALSE;
			event.p.respHold = HFG_RESP_HOLD_STATE_REJECT;			
			break;
			
		/* Accept Held call */
		case BAT_P_CHLD_N_ADDHLD:
			context->responseHold = BTHAL_FALSE;			
			event.p.respHold = HFG_RESP_HOLD_STATE_ACCEPT;			
			break;
			
		/* Put incoming on hold */
		case BAT_P_CHLD_N_ONLYHOLD:
			event.p.respHold = HFG_RESP_HOLD_STATE_HOLD;
			break;
			
		default:
			BTHAL_MODEM_LOG_ERROR(("BTHAL MC: Illegal value for AT%CHLD while response hold is active"));
			Assert(0);
			return BAT_OK;
		}
			
		event.type = BTHAL_MC_EVENT_SET_RESPONSE_AND_HOLD_RES;
		event.status = BTHAL_STATUS_SUCCESS;
		event.context = context;
		event.userData = context->userData;
		context->callback(&event);
	}

	if (context->responseHold == BTHAL_TRUE && BAT_CMD_AT_A == context->atCmd.cmd.ctrl_params)
	{
		BthalMcPutActiveCallOnHold(context);
	}
	else
	{
		event.type = BTHAL_MC_EVENT_OK;
		event.status = BTHAL_STATUS_SUCCESS;
		event.context = context;
		event.userData = context->userData;
		context->callback(&event);

		if (BAT_CMD_SET_PERCENT_CHLD == context->atCmd.cmd.ctrl_params &&
		    BAT_P_CHLD_N_ADDHLD == context->atCmd.cmd.params.ptr_set_percent_chld->n)
		{
            /*	When switching a call from active to held, we receive a CPI event
             *	but we do not receive CPI event, when the call is set to active after
             *	AT+CHLD=3 command is sent. In this case we get only OK response to 
             *	this command.
             */
            heldCallIndex = BthalMcFindCallInState(BTHAL_MC_CALL_STATE_HELD);

            if (BTHAL_MC_INVALID_CALL_INDEX != heldCallIndex)
            {
                BthalMcCallStateMachine(heldCallIndex, BTHAL_MC_CALL_EVENT_ACTIVE);
            }
            else
            {
                BTHAL_MODEM_LOG_ERROR(("BTHAL MC: incorrect index of held call"));
            }
		}
	}

	return BAT_OK;
}

static int BthalMcHandleCpbsRes(BthalMcContext *context,
								const T_BAT_res_que_plus_cpbs *cpbs)
{
	BthalMcEvent event;
	BTHAL_BOOL sendEvent = BTHAL_TRUE;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleCpbsRes"));

	switch (cpbs->storage)
	{
	case BAT_STORAGE_FD:
		event.p.selectedPhonebook.selected = AT_PBS_FIXED_DIAL;
		break;

	case BAT_STORAGE_DC:
		event.p.selectedPhonebook.selected = AT_PBS_LAST_DIAL; 
		break;

	case BAT_STORAGE_MC:
		event.p.selectedPhonebook.selected = AT_PBS_MISSED_CALLS;
		break;

	case BAT_STORAGE_AD:
		event.p.selectedPhonebook.selected = AT_PBS_ME_PHONEBOOK;		
		break;

	case BAT_STORAGE_AF:
		event.p.selectedPhonebook.selected = AT_PBS_ME_SIM_COMBINED;
		break;

	case BAT_STORAGE_RC:
		event.p.selectedPhonebook.selected = AT_PBS_RECEIVED_CALLS;		
		break;

	case BAT_STORAGE_EN:
	case BAT_STORAGE_BD:
	case BAT_STORAGE_SD:
	case BAT_STORAGE_UD:
		sendEvent = BTHAL_FALSE;
		BTHAL_MODEM_LOG_ERROR(("Unsupported phonebook is currently selected: %d",(int)cpbs->storage));
		break;

	default:
		sendEvent = BTHAL_FALSE;
		BTHAL_MODEM_LOG_ERROR(("Unrecognized phonebook number received: %d", (int)cpbs->storage));
		break;
	}

	if (BTHAL_TRUE == sendEvent)
	{
		event.p.selectedPhonebook.used = cpbs->used;
		event.p.selectedPhonebook.total = cpbs->total;
		event.type = BTHAL_MC_EVENT_SELECTED_PHONEBOOK;
		event.status = BTHAL_STATUS_SUCCESS;
		event.context = context;
		event.userData = context->userData;
		context->callback(&event);
	}

	return BAT_OK;
}

static int BthalMcHandleCpbrRes(BthalMcContext *context, const T_BAT_res_set_plus_cpbr *cpbr)
{
	BthalMcEvent event;
	
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleCpbrRes"));

	if (BTHAL_MC_MAX_NUMBER_LENGTH > cpbr->c_number &&
		BTHAL_MC_MAX_ENTRY_TEXT_LENGTH > cpbr->c_text)
	{
		event.p.phonebookEntry.index = cpbr->index;
		event.p.phonebookEntry.type = cpbr->type;
		OS_StrnCpy(event.p.phonebookEntry.number, (const char*)cpbr->number, cpbr->c_number);
		event.p.phonebookEntry.number[cpbr->c_number] = '\0';
		OS_StrnCpy(event.p.phonebookEntry.text, (const char*)cpbr->text, cpbr->c_text);
		event.p.phonebookEntry.text[cpbr->c_text] = '\0';
		
		event.type = BTHAL_MC_EVENT_READ_PHONEBOOK_ENTRIES_RES;
		event.status = BTHAL_STATUS_SUCCESS;
		event.context = context;
		event.userData = context->userData;
		context->callback(&event);
	}
	else
	{
		BTHAL_MODEM_LOG_ERROR(("Length of entry's number or text is too long"));
	}

	return BAT_OK;
}

static int BthalMcHandleTestCpbrRes(BthalMcContext *context, const T_BAT_res_tst_plus_cpbr *cpbr)
{
	BthalMcEvent event;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleTestCpbrRes"));

	event.p.phonebookData.firstIndex = cpbr->index_f;
	event.p.phonebookData.lastIndex = cpbr->index_l;
	event.p.phonebookData.numberLength = cpbr->nlength;
	event.p.phonebookData.textLength = cpbr->tlength;

	event.type = BTHAL_MC_EVENT_TST_READ_PHONEBOOK_ENTRIES_RES;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = context;
	event.userData = context->userData;
	context->callback(&event);

	return BAT_OK;
}

static int BthalMcHandleCpbfRes(BthalMcContext *context, const T_BAT_res_set_plus_cpbf *cpbf)
{
	BthalMcEvent event;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleCpbfRes"));

	if (BTHAL_MC_MAX_NUMBER_LENGTH > cpbf->c_number &&
		BTHAL_MC_MAX_ENTRY_TEXT_LENGTH > cpbf->c_text)
	{
		event.p.phonebookEntry.index = cpbf->index;
		event.p.phonebookEntry.type = cpbf->type;
		OS_StrnCpy(event.p.phonebookEntry.number, (const char*)cpbf->number, cpbf->c_number);
		event.p.phonebookEntry.number[cpbf->c_number] = '\0';
		OS_StrnCpy(event.p.phonebookEntry.text, (const char*)cpbf->text, cpbf->c_text);
		event.p.phonebookEntry.text[cpbf->c_text] = '\0';
		
		event.type = BTHAL_MC_EVENT_FIND_PHONEBOOK_ENTRIES_RES;
		event.status = BTHAL_STATUS_SUCCESS;
		event.context = context;
		event.userData = context->userData;
		context->callback(&event);
	}
	else
	{
		BTHAL_MODEM_LOG_ERROR(("Length of entry's number or text is too long"));
	}

	return BAT_OK;

}

static int BthalMcHandleReadCscs(BthalMcContext *context, const T_BAT_res_que_plus_cscs *cscs)
{
	BthalMcEvent event;
	const char *charset;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcHandleReadCscs"));

	switch (cscs->cs)
	{
	case  BAT_CSCS_CS_IRA:
		charset = "IRA";
		break;
	case BAT_CSCS_CS_PCDN:
		charset = "PCDN";
		break;
	case BAT_CSCS_CS_8859_1:
		charset = "8859-1";
		break;
	case BAT_CSCS_CS_PCCP_437:
		charset = "PCCP437";
		break;
	case BAT_CSCS_CS_GSM:
		charset = "GSM";
		break;
	case BAT_CSCS_CS_HEX:
		charset = "HEX";
		break;
	case BAT_CSCS_CS_UCS2:
		charset = "UCS2";
		break;
	default:
		break;
	}

	event.type = BTHAL_MC_EVENT_SELECTED_CHAR_SET;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = context;
	event.userData = context->userData;
	OS_StrCpy(&event.p.charsetType[0], (const char *)charset);
	context->callback(&event);

	return BAT_OK;
}

static const char* BthalMcBatRetToString(T_BAT_return retVal)
{
	switch (retVal)
	{
	case BAT_OK:
		return  "BAT_OK";
	case BAT_BUSY_RESOURCE:
		return "BAT_BUSY_RESOURCE";
	case BAT_ERROR:
		return "BAT_ERROR";
	default:
		break;
	}
	return "Unrecognized BAT return value";
}

static void BthalMcDialNumber(BthalMcContext *context, const BTHAL_U8 *dialString, BTHAL_U8 length)
{

	T_BAT_cmd_at_d *param;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcDialNumber"));

	param = (T_BAT_cmd_at_d*)(&context->atCmd.paramMem);
	param->c_dial_string = length;
	memcpy((char*)param->dial_string, dialString, length);
	context->atCmd.cmd.ctrl_params = BAT_CMD_AT_D;

	BthalMcCmdQuePushBack(context);
	BthalMcSendCommand();
}

static void BthalMcReportUnsResult(BthalMcEvent *event)
{
	BTHAL_U32 nContext;
	BthalMcContext *context;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcReportUnsResult"));

	context = moduleData.contextsMem;
	for (nContext = BTHAL_MC_NUM_OF_USERS; nContext > 0; --nContext)
	{
		if (BTHAL_MC_CONTEXT_SETTINGS_USED & context->settings)
		{
			event->context = context;
			event->userData = context->userData;
			context->callback(event);
		}
		++context;
	}
}

static void BthalMcUpdateIndicator(HfgIndicator indicator, BTHAL_U8 value)
{
	BthalMcEvent event;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcUpdateIndicator"));

	BTHAL_MODEM_LOG_INFO(("Value for indicator: %d is %d", (int)indicator, (int)value));

	/* Verify if the indicator's value has changed */	
	if (BthalMcGetIndicatorValue(indicator) == value)
	{
		return;
	}

	BthalMcSetIndicatorValue(indicator, value);

	/* Notify all users about the indicator's new value */
	event.p.indicator.ind = indicator;
	event.p.indicator.val = value;
	event.status = BTHAL_STATUS_SUCCESS;
	event.type = BTHAL_MC_EVENT_INDICATOR_EVENT;

	BthalMcReportUnsResult(&event);
}

static void BthalMcSendAllIndicatorsToUser(BthalMcContext *context)
{
	BthalMcEvent event;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcSendAllIndicatorsToUser"));

	event.type = BTHAL_MC_EVENT_CURRENT_INDICATORS_STATUS;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = context;
	event.userData = context->userData;

	event.p.indicatorsValue.service = (BTHAL_BOOL)BthalMcGetIndicatorValue(HFG_IND_SERVICE);
	event.p.indicatorsValue.call = (BTHAL_BOOL)BthalMcGetIndicatorValue(HFG_IND_CALL);
	event.p.indicatorsValue.setup = (HfgCallSetupState)BthalMcGetIndicatorValue(HFG_IND_CALL_SETUP);
	event.p.indicatorsValue.hold = (HfgHoldState)BthalMcGetIndicatorValue(HFG_IND_CALL_HELD);
	event.p.indicatorsValue.battery = (BTHAL_U8)BthalMcGetIndicatorValue(HFG_IND_BATTERY);
	event.p.indicatorsValue.signal = (BTHAL_U8)BthalMcGetIndicatorValue(HFG_IND_SIGNAL);
	event.p.indicatorsValue.roaming = (BTHAL_BOOL)BthalMcGetIndicatorValue(HFG_IND_ROAMING);

	context->callback(&event);	
}

static BTHAL_U8 BthalMcGetIndicatorValue(HfgIndicator ind)
{
	return moduleData.indicators[ind-1];
}

static void BthalMcSetIndicatorValue(HfgIndicator ind, BTHAL_U8 value)
{
	moduleData.indicators[ind-1] = value;
}


void bthalMcActivateRingClipTimer()
{
	BTHAL_MODEM_LOG_FUNCTION(("bthalMcActivateRingClipTimer"));
		
	moduleData.isRingClipTimerScheduled = BTHAL_FALSE;
	
	BthalMcHandleRingUns();
	BthalMcSendClipEvent();
 }

BthalStatus BTHAL_MC_ScheduleRingClipEvent(void)
{
	BthalStatus bthalStatus;

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_ScheduleRingClipEvent"));

	if (BTHAL_TRUE == moduleData.isRingClipTimerScheduled)
	{
		return BTHAL_STATUS_SUCCESS;
	}
	
	bthalStatus = BTHAL_OS_ResetTimer(bthalMcRingClipTimerIndex, BTHAL_MC_RING_CLIP_TIMER_INTERVAL, 0);
	if (BTHAL_STATUS_SUCCESS != bthalStatus)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL_MC: Error scheduling ring/clip timer"));
		return BTHAL_STATUS_FAILED;
	}

	moduleData.isRingClipTimerScheduled = BTHAL_TRUE;

	return BTHAL_STATUS_SUCCESS;
}

static void BthalMcReportModuleError(BthalMcContext *context, HfgCmeError errCode)
{
	BthalMcEvent event;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcReportModuleError"));

	if (BTHAL_MC_CONTEXT_SETTINGS_EXTENDED_ERR & context->settings)
	{
		event.type = BTHAL_MC_EVENT_EXTENDED_ERROR;
		event.p.err = errCode;
	}
	else
	{
		event.type = BTHAL_MC_EVENT_ERROR;
	}

	event.context = context;
	event.userData = context->userData;
	context->callback(&event);
}

static void BthalMcPutActiveCallOnHold(BthalMcContext *context)
{
	T_BAT_cmd_set_percent_chld *param;

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_HandleCallHoldAndMultiparty"));
	
	context->atCmd.cmd.ctrl_params = BAT_CMD_SET_PERCENT_CHLD;	
	param = (T_BAT_cmd_set_percent_chld*)(&context->atCmd.paramMem);
	param->x = (BTHAL_S16)BAT_PARAMETER_NOT_PRESENT;
	param->n = BAT_P_CHLD_N_ONLYHOLD;
	
	BthalMcCmdQuePushBack(context);
}

static void BthalMcInitCallStateMachine(void)
{
	int row, col, idx;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcInitCallStateMachine"));

	for (idx = 0; idx < BTHAL_MC_MAX_NUM_CALLS; ++idx)
	{
		moduleData.currentCalls[idx] = BTHAL_MC_CALL_STATE_IDLE;
	}

	for (row = 0; row < BTHAL_MC_NUM_CALL_STATES; ++row)
	{
		for (col = 0; col < BTHAL_MC_NUM_CALL_EVENTS; ++col)
		{
			moduleData.callStateMachine[row][col] = BthalMcCallStateMachineIllegalEvent;
		}
	}
	
	isCallWaiting = FALSE;
	
	moduleData.callStateMachine[BTHAL_MC_CALL_STATE_IDLE][BTHAL_MC_CALL_EVENT_INCOMING] = BthalMcCallStateIdleIncoming;
	moduleData.callStateMachine[BTHAL_MC_CALL_STATE_IDLE][BTHAL_MC_CALL_EVENT_OUTGOING] = BthalMcCallStateIdleOutgoing;
	moduleData.callStateMachine[BTHAL_MC_CALL_STATE_INCOMING][BTHAL_MC_CALL_EVENT_IDLE] = BthalMcCallStateIncomingIdle;
	moduleData.callStateMachine[BTHAL_MC_CALL_STATE_INCOMING][BTHAL_MC_CALL_EVENT_ACTIVE] = BthalMcCallStateIncomingActive;
	moduleData.callStateMachine[BTHAL_MC_CALL_STATE_OUTGOING][BTHAL_MC_CALL_EVENT_IDLE] = BthalMcCallStateOutgoingIdle;
	moduleData.callStateMachine[BTHAL_MC_CALL_STATE_OUTGOING][BTHAL_MC_CALL_EVENT_ALERTING] = BthalMcCallStateOutgoingAlerting;
	moduleData.callStateMachine[BTHAL_MC_CALL_STATE_OUTGOING][BTHAL_MC_CALL_EVENT_ACTIVE] = BthalMcCallStateOutgoingActive;
	moduleData.callStateMachine[BTHAL_MC_CALL_STATE_ALERTING][BTHAL_MC_CALL_EVENT_IDLE] = BthalMcCallStateAlertingIdle;
	moduleData.callStateMachine[BTHAL_MC_CALL_STATE_ALERTING][BTHAL_MC_CALL_EVENT_ACTIVE] = BthalMcCallStateAlertingActive;
	moduleData.callStateMachine[BTHAL_MC_CALL_STATE_ACTIVE][BTHAL_MC_CALL_EVENT_IDLE] = BthalMcCallStateActiveIdle;
	moduleData.callStateMachine[BTHAL_MC_CALL_STATE_ACTIVE][BTHAL_MC_CALL_EVENT_HELD] = BthalMcCallStateActiveHeld;
	moduleData.callStateMachine[BTHAL_MC_CALL_STATE_HELD][BTHAL_MC_CALL_EVENT_IDLE] = BthalMcCallStateHeldIdle;
	moduleData.callStateMachine[BTHAL_MC_CALL_STATE_HELD][BTHAL_MC_CALL_EVENT_ACTIVE] = BthalMcCallStateHeldActive;


}

static void BthalMcCallStateMachine(BTHAL_U32 callIndex, BthalMcCallEvent event)
{
	BthalStatus status;
	BthalMcCallState state;
	
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcCallStateMachine"));
	
	if (BTHAL_MC_NUM_CALL_EVENTS <= event)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: Illegal call event: %d", (int)event));
		return;
	}
	if (BTHAL_MC_MAX_NUM_CALLS <= callIndex)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: Illegal call index: %d", (int)callIndex));
		return;
	}

	state = moduleData.currentCalls[callIndex];

	if (BTHAL_MC_NUM_CALL_STATES <= state)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: Illegal call state: %d", (int)state));
		return;
	}

	status = moduleData.callStateMachine[state][event](callIndex);
	if (BTHAL_STATUS_SUCCESS != status)
	{
		BTHAL_MODEM_LOG_ERROR(("BTHAL MC: State machine failed, state: %d, event:%d, call index: %d",
			(int)state, (int)event, (int)callIndex));
	}
}

static BTHAL_U32 BthalMcFindCallInState(BthalMcCallState state)
{
	BTHAL_U32 idx;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcFindCallInState"));

	for (idx =0; idx < BTHAL_MC_MAX_NUM_CALLS; ++idx)
	{
		if (state == moduleData.currentCalls[idx])
		{
			return idx;
		}
	}

	return BTHAL_MC_INVALID_CALL_INDEX;
}

static BTHAL_U32 BthalMcNumOfCallsInState(BthalMcCallState state)
{
	BTHAL_U32 idx, nCalls;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcNumOfCalls"));

	nCalls = 0;
	for (idx = 0; idx < BTHAL_MC_MAX_NUM_CALLS; ++idx)
	{
		if (state == moduleData.currentCalls[idx])
		{
			++nCalls;
		}
	}

	return nCalls;
}

static BthalStatus BthalMcCallStateMachineIllegalEvent(BTHAL_U32 callIndex)
{
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcCallStateMachineIllegalEvent"));

	BTHAL_MODEM_LOG_ERROR(("Illegal state-event combination"));

	Assert(0);

	return BTHAL_STATUS_FAILED;
}

static BthalStatus BthalMcCallStateIdleIncoming(BTHAL_U32 callIndex)
{
	BTHAL_U32 activeCalls;
	BTHAL_U32 heldCalls; 
	
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcCallStateIdleIncoming"));

	moduleData.currentCalls[callIndex] = BTHAL_MC_CALL_STATE_INCOMING;

	activeCalls = BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_ACTIVE);
	heldCalls = BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_HELD);
	
	if((0 < activeCalls) || (0 < heldCalls))
	{
		isCallWaiting = TRUE;
	}

	BthalMcUpdateIndicator(HFG_IND_CALL_SETUP, HFG_CALL_SETUP_IN);

	return BTHAL_STATUS_SUCCESS;
}

static BthalStatus BthalMcCallStateIdleOutgoing(BTHAL_U32 callIndex)
{
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcCallStateIdleOutgoing"));

	moduleData.currentCalls[callIndex] = BTHAL_MC_CALL_STATE_OUTGOING;

	BthalMcUpdateIndicator(HFG_IND_CALL_SETUP, HFG_CALL_SETUP_OUT);

	return BTHAL_STATUS_SUCCESS;
}

static BthalStatus BthalMcCallStateIncomingIdle(BTHAL_U32 callIndex)
{
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcCallStateIncomingIdle"));

	moduleData.currentCalls[callIndex] = BTHAL_MC_CALL_STATE_IDLE;

	isCallWaiting = FALSE;

	BthalMcUpdateIndicator(HFG_IND_CALL_SETUP, HFG_CALL_SETUP_NONE);

	return BTHAL_STATUS_SUCCESS;

}

static BthalStatus BthalMcCallStateIncomingActive(BTHAL_U32 callIndex)
{
	BTHAL_U32 heldCalls;
	
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcCallStateIncomingActive"));

	moduleData.currentCalls[callIndex] = BTHAL_MC_CALL_STATE_ACTIVE;

	heldCalls = BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_HELD);
	
	if (0 == heldCalls)
	{
		/*Incoming call was a waiting one*/
		if(isCallWaiting)
		{
			BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_NO_HLD);
		}
		else
		{
		/* There is only one active calls and no held calls */
		BthalMcUpdateIndicator(HFG_IND_CALL, 1);
	}
	}
	else
	{
		BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_HLD_ACT);
	}
	
	isCallWaiting = FALSE;
	
	BthalMcUpdateIndicator(HFG_IND_CALL_SETUP, HFG_CALL_SETUP_NONE);

	return BTHAL_STATUS_SUCCESS;

}

static BthalStatus BthalMcCallStateOutgoingIdle(BTHAL_U32 callIndex)
{
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcCallStateOutgoingIdle"));

	moduleData.currentCalls[callIndex] = BTHAL_MC_CALL_STATE_IDLE;

	BthalMcUpdateIndicator(HFG_IND_CALL_SETUP, HFG_CALL_SETUP_NONE);

	return BTHAL_STATUS_SUCCESS;
}
	
static BthalStatus BthalMcCallStateOutgoingAlerting(BTHAL_U32 callIndex)
{
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcCallStateOutgoingIdle"));

	moduleData.currentCalls[callIndex] = BTHAL_MC_CALL_STATE_ALERTING;

	BthalMcUpdateIndicator(HFG_IND_CALL_SETUP, HFG_CALL_SETUP_ALERT);

	return BTHAL_STATUS_SUCCESS;
}
	
static BthalStatus BthalMcCallStateOutgoingActive(BTHAL_U32 callIndex)
{
	BTHAL_U32 heldCalls;
	
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcCallStateOutgoingActive"));

	moduleData.currentCalls[callIndex] = BTHAL_MC_CALL_STATE_ACTIVE;

	heldCalls = BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_HELD);
	
	if (0 == heldCalls)
	{
		/* There is only one active calls and no held calls */
		BthalMcUpdateIndicator(HFG_IND_CALL, 1);
	}
	else
	{
		BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_HLD_ACT);
	}
	
	BthalMcUpdateIndicator(HFG_IND_CALL_SETUP, HFG_CALL_SETUP_NONE);

	return BTHAL_STATUS_SUCCESS;
}

	
static BthalStatus BthalMcCallStateAlertingIdle(BTHAL_U32 callIndex)
{
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcCallStateAlertingIdle"));

	moduleData.currentCalls[callIndex] = BTHAL_MC_CALL_STATE_IDLE;

	BthalMcUpdateIndicator(HFG_IND_CALL_SETUP, HFG_CALL_SETUP_NONE);

	return BTHAL_STATUS_SUCCESS;
}
	
static BthalStatus BthalMcCallStateAlertingActive(BTHAL_U32 callIndex)
{
	BTHAL_U32 heldCalls;
	
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcCallStateAlertingActive"));

	moduleData.currentCalls[callIndex] = BTHAL_MC_CALL_STATE_ACTIVE;

	heldCalls = BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_HELD);
	
	if (0 == heldCalls)
	{
		/* There is only one active calls and no held calls */
		BthalMcUpdateIndicator(HFG_IND_CALL, 1);
	}
	else
	{
		BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_HLD_ACT);
	}
	
	BthalMcUpdateIndicator(HFG_IND_CALL_SETUP, HFG_CALL_SETUP_NONE);

	return BTHAL_STATUS_SUCCESS;
}
	
static BthalStatus BthalMcCallStateActiveIdle(BTHAL_U32 callIndex)
{
	BTHAL_U32 activeCalls;
	BTHAL_U32 heldCallIndex;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcCallStateActiveIdle"));

	moduleData.currentCalls[callIndex] = BTHAL_MC_CALL_STATE_IDLE;

	activeCalls = BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_ACTIVE);
	if (0 == activeCalls)
	{
		BTHAL_U32 heldCalls = BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_HELD);
		
		if (0 < heldCalls)
		{
			/* We assume that held call is retreived to be active: see comment in function
			 * BthalMcCallStateActiveHeld() below. */
			heldCallIndex = BthalMcFindCallInState(BTHAL_MC_CALL_STATE_HELD);
			if (BTHAL_MC_INVALID_CALL_INDEX != heldCallIndex)
			{
                		moduleData.currentCalls[heldCallIndex] = BTHAL_MC_CALL_STATE_ACTIVE;
                		BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_NO_HLD);
			}
		}
		else
		{
			/*Waiting call is present*/
			if(isCallWaiting)
			{
				BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_NO_HLD);
			}
			/*No other calls are present*/
			else
			{
			BthalMcUpdateIndicator(HFG_IND_CALL, 0);
		}
			
		}
	}

	return BTHAL_STATUS_SUCCESS;
}
	
static BthalStatus BthalMcCallStateActiveHeld(BTHAL_U32 callIndex)
{
	BTHAL_U32 heldCallIndex;
	
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcCallStateActiveHeld"));

	/*	When switching between active and held calls we receive a CPI event
	 *	about the call which is set to held but not about the call which is 
	 *	set to active.
	 *	We are limiting the number of supported calls to 2, and when ever
	 *	a call is set to hold we check if we already have a held call,
	 *	and if so we set it to active.
	 */
	heldCallIndex = BthalMcFindCallInState(BTHAL_MC_CALL_STATE_HELD);
	if (BTHAL_MC_INVALID_CALL_INDEX != heldCallIndex)
	{
		moduleData.currentCalls[heldCallIndex] = BTHAL_MC_CALL_STATE_ACTIVE;
		moduleData.currentCalls[callIndex] = BTHAL_MC_CALL_STATE_HELD;
		BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_HLD_ACT);
		return BTHAL_STATUS_SUCCESS;
	}

	moduleData.currentCalls[callIndex] = BTHAL_MC_CALL_STATE_HELD;

	if (0 == BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_ACTIVE))
	{
		/* Check if we are in a 3-way calling scenario */
		if (0 == BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_INCOMING)		&&
			0 == BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_OUTGOING)	&&
			0 == BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_ALERTING))
		{
			BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_HLD_NO_ACT);
		}
		else
		{
			BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_HLD_ACT);
		}
	}
	else
	{
		BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_HLD_ACT);
	}

	return BTHAL_STATUS_SUCCESS;
}

static BthalStatus BthalMcCallStateHeldIdle(BTHAL_U32 callIndex)
{
	BTHAL_U32 activeCalls, heldCalls;
	
	BTHAL_MODEM_LOG_FUNCTION(("BthalMcCallStateHeldIdle"));

	moduleData.currentCalls[callIndex] = BTHAL_MC_CALL_STATE_IDLE;

	activeCalls = BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_ACTIVE);
	heldCalls = BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_HELD);

	if (0 < activeCalls)
	{
		if (0 == heldCalls)
		{
			BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_NO_HLD);
		}
		else
		{
			BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_HLD_ACT);
		}
	}
	else
	{
		if (0 == heldCalls)
		{
			BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_NO_HLD);
			BthalMcUpdateIndicator(HFG_IND_CALL, 0);
		}
		else
		{
			BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_HLD_NO_ACT);
		}
	}

	return BTHAL_STATUS_SUCCESS;
}
	
static BthalStatus BthalMcCallStateHeldActive(BTHAL_U32 callIndex)
{
	BTHAL_U32 heldCalls;

	BTHAL_MODEM_LOG_FUNCTION(("BthalMcCallStateHeldActive"));

	moduleData.currentCalls[callIndex] = BTHAL_MC_CALL_STATE_ACTIVE;

	heldCalls = BthalMcNumOfCallsInState(BTHAL_MC_CALL_STATE_HELD);

	if (0 == heldCalls)
	{
		BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_NO_HLD);
	}
	else
	{
		BthalMcUpdateIndicator(HFG_IND_CALL_HELD, HFG_HOLD_HLD_ACT);
	}

	return BTHAL_STATUS_SUCCESS;
}


int  BthalMcSendBatteryStatus(U32 batteryStatus)
{
	BTHAL_U8 value = (BTHAL_U8)batteryStatus;
	
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MC_SendBatteryStatus"));

	BthalMcUpdateIndicator(HFG_IND_BATTERY,value);

	return BT_STATUS_SUCCESS;
}


#else /* BTL_CONFIG_VG ==   BTL_CONFIG_ENABLED */

BthalStatus BTHAL_MC_Init(BthalCallBack	callback)

{
	callback = callback;
	Report(("BTHAL_MC_Init -BTL_CONFIG_VG is disabled."));
	return BTHAL_STATUS_SUCCESS;
}

BthalStatus BTHAL_MC_Deinit(void)
{
	Report(("BTHAL_MC_Deinit -BTL_CONFIG_VG is disabled."));
	return BTHAL_STATUS_SUCCESS;

}



#endif /* BTL_CONFIG_VG == BTL_CONFIG_ENABLED */




