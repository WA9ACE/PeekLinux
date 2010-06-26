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
*   FILE NAME:      btu_pei.c
*
*   DESCRIPTION:    This file contains the implementation of the btu_pei module
*					functionality - protocol entity interface (PEI) for the BTU
*					(Bluetooth UART) GPF body.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/

#ifndef BTU_PEI_C
#define BTU_PEI_C

#define ENTITY_BTU

/* Header files includes */
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "vsi.h"
#include "ccdapi.h"
#include "pei.h"
#include "tok.h"
#include "tools.h"
#include "btu.h"
#include "hcill.h"
#include "bthal_uart.h"
#include "bthal_pm.h"

/* Declarations of internal (static) functions */

/* Declaration of static variables */
static BOOL	first_access = TRUE;
static T_MONITOR btu_mon;				/* Struct for monitoring physical parameters */

/* Declaration of global variables */
GLOBAL T_HANDLE	btu_handle = VSI_ERROR;		/* Communication handle of BTU */

/* Declaration of external variables and functions */
#ifdef WIN32
extern void uartWaitForEvent(unsigned long msWait);
#else	/* WIN32 */
#endif	/* WIN32 */

/*******************************************************************************\
*  FUNCTION:	pei_init
*                                                                         
*  PARAMETERS:	T_HANDLE handle: handle of the entity.
*                                                                         
*  DESCRIPTION:	1. In the passive variant, this function is called by the frame
*					at initialization.
*				2. In the active variant, this function should be called by the
*					function pei_run().              
*                                                                         
*  RETURNS:		SHORT PEI_OK/PEI_ERROR - success/error.
\*******************************************************************************/
LOCAL SHORT pei_init(T_HANDLE handle)
{
	int trace_mask;
	
	btu_handle = handle;

	/* Read trace mask */
	if (vsi_gettracemask(btu_handle, btu_handle, (ULONG *)&trace_mask) == VSI_ERROR)
	{
		return PEI_ERROR;
	}

	/* Enable trace mask */
	trace_mask |= (TC_ERROR | TC_STATE | TC_PRIM | TC_EVENT | TC_FUNC);
	
	if (vsi_settracemask(btu_handle, btu_handle, trace_mask) == VSI_ERROR)
	{
		return PEI_ERROR;
	}
	
	TRACE_FUNCTION ("pei_init()");

	/* Initialize Condat coder-decoder */
	ccd_init();
	
	return PEI_OK;
}

/*******************************************************************************\
*  FUNCTION:	pei_primitive
*                                                                         
*  PARAMETERS:	void *p_prim: pointer to buffer with received primitive
*                                                                         
*  DESCRIPTION:	1. In the passive variant, this function is called by the frame
*					if a primitive for this entity has been received.
*				2. In the active variant, this function should be called by the
*					pei_run().              
*                                                                         
*  RETURNS:		SHORT PEI_OK/PEI_ERROR - primitive processed/not processed.
\*******************************************************************************/
LOCAL SHORT pei_primitive(void *p_prim)
{
	char * data;
	char token[200];
	unsigned int length;

	TRACE_FUNCTION("pei_primitive()");
  
	data = (char *)P2D(p_prim);
	length = GetNextToken (data, token, " #");
	
	/* Process TRACECLASS primitive */
	if (!strcmp (token, "TRACECLASS"))
	{
		if (length < strlen(data) - 1)
		{
			ULONG trace_mask;
			
			GetNextToken (data + length+1, token, " #");
			trace_mask = ASCIIToHex(token, CHARS_FOR_32BIT);
			if (VSI_ERROR == vsi_settracemask (btu_handle, btu_handle, trace_mask))
			{
				TRACE_ERROR("Error processing BTU trace mask");
			}
		}
	}
  
	return PEI_OK;
}

/*******************************************************************************\
*  FUNCTION:	pei_signal
*                                                                         
*  PARAMETERS:	ULONG opc: operation code of the signal;
*				void *p_data: pointer to signal data.
*                                                                         
*  DESCRIPTION:	1. In the passive variant, this function is called by the frame
*					if a signal is received. This function will never be called
*					while a pei_primitive() call is active in the same body.
*				2. In the active variant, this function should be called by the
*					pei_run() if a signal is received.     
*                                                                         
*  RETURNS:		SHORT PEI_OK/PEI_ERROR - primitive processed/not processed.
\*******************************************************************************/
LOCAL SHORT pei_signal(ULONG opc, void *p_data)
{
  //TRACE_FUNCTION_P1("pei_signal(): %d", opc);

#ifdef WIN32

#else	/* WIN32 */
/*	We are no more using the GPF signalling mechanism for signalling the BTU 
 *	from UART HISR. Instead the BTU main thread now uses Nucleus Event-Groups
 *	for the same.
 */
#if 0
  switch (opc)
  {
  case (BTU_RX_EVENT):
	  {
		  uartRXEvent();
		  break;
	  }
  case (BTU_TX_EVENT):
	  {
		  uartTXEvent();
		  break;
	  }
  default:
	  {
		  return PEI_ERROR;
	  }
  }
#endif
#endif	/* WIN32 */
  
  return PEI_OK;
}

/*******************************************************************************\
*  FUNCTION:	pei_timeout
*                                                                         
*  PARAMETERS:	USHORT timer_index: index of expired timer.
*                                                                         
*  DESCRIPTION:	1. In the passive variant, this function is called by the frame
*					if a signal is received. The parameter index is the same
*					that was transferred to the VSI when the timer was started.
*				2. In the active variant, this function should be called by the
*					pei_run() if a timeout occured.     
*                                                                         
*  RETURNS:		SHORT PEI_OK/PEI_ERROR - primitive processed/not processed.
\*******************************************************************************/
LOCAL SHORT pei_timeout(USHORT timer_index)
{

	return PEI_OK;
}

/*******************************************************************************\
*  FUNCTION:	pei_run
*                                                                         
*  PARAMETERS:	T_HANDLE task_handle: task handle;
*				T_HANDLE com_handle: handle of a task's own queue.
*                                                                         
*  DESCRIPTION:	This function is used as a main loop for an Active Body entity.     
*                                                                         
*  RETURNS:		SHORT PEI_OK/PEI_ERROR - success/error.
\*******************************************************************************/
LOCAL SHORT pei_run(T_HANDLE task_handle, T_HANDLE com_handle)
{
	//T_QMSG message;
	//int status;

	TRACE_FUNCTION("pei_run()");

	/*	Setting the default state HCILL_ASLEEP to allow the system to go to 
	 *	Deep-Sleep even if Bluetooth is not enabled.
	 */

	BTHAL_PM_ChangeSleepState(BTHAL_PM_PERIPHERAL_DEVICE_UART, BTHAL_PM_SLEEP_STATE_ASLEEP);
	
	/* Do not exit task even when Bluetooth is disabled */
	/* BTHAL_UART_EventThread is the main thread of BTU for all UART activities */
	BTHAL_UART_EventThread();

	/*	In order to replace the GPF signalling mechanism with Event-Groups, 
	 *	the following code has been disabled and the main thread of BTU will be
	 *	BTU_eventThread() which will wait for and process all the UART events.
	 *	the beneath code can be resued by the use of GPF Event-Groups mechanism.
	*/
#if 0
	while(1)
	{
		/* Wait for a message to arrive */
		status = vsi_c_await(btu_handle, btu_handle, &message, BTU_SIGNAL_WAIT_TIME);

		switch(status)
		{
			case VSI_OK:
				switch(message.MsgType)
				{
					case MSG_SIGNAL:
						pei_signal((USHORT)message.Msg.Signal.SigOPC,
								   message.Msg.Signal.SigBuffer);
						break;

					case MSG_PRIMITIVE:
						pei_primitive(message.Msg.Primitive.Prim);
						break;

					case MSG_TIMEOUT:
						pei_timeout((USHORT)message.Msg.Timer.Index);
						break;

					default:
						TRACE_ERROR("Unknown message type");
						break;
				}
				break;
#ifdef WIN32
			case VSI_TIMEOUT:
				/* Wait for UART events */
				uartWaitForEvent(BTU_DRIVER_WAIT_TIME);
				break;
#endif	/* WIN32 */
			default:
				TRACE_ERROR("Invalid communication handle");
				break;
		}
	}
	#endif
	return TRUE;
}

/*******************************************************************************\
*  FUNCTION:	pei_exit
*                                                                         
*  PARAMETERS:	None.
*                                                                         
*  DESCRIPTION:	1. In the passive variant, this function releases all resources
*					reserved by the entity, so that they are available for other
*					purposes. It closes the communication resources with other
*					entities and, eventually, frees allocated memory.
*				2. In the active variant, this function sets a flag that enables
*					the pei_run() function to exit its main loop and terminate
*					the entity.     
*                                                                         
*  RETURNS:		SHORT PEI_OK/PEI_ERROR - primitive processed/not processed.
\*******************************************************************************/
LOCAL SHORT pei_exit(void)
{
	TRACE_FUNCTION("pei_exit()");

	btu_handle = VSI_ERROR;

	/* Exit is done in BTS exit */
	return PEI_OK;
}

/*******************************************************************************\
*  FUNCTION:	pei_config
*                                                                         
*  PARAMETERS:	T_PEI_CONFIG cfg_string: configuration string.
*                                                                         
*  DESCRIPTION:	This function is used to set configuration values of a protocol
*				entity.     
*                                                                         
*  RETURNS:		SHORT PEI_OK/PEI_ERROR - primitive processed/not processed.
\*******************************************************************************/
LOCAL SHORT pei_config(T_PEI_CONFIG cfg_string)
{
#ifndef NCONFIG
	char *evt_string = cfg_string;
	TRACE_FUNCTION("pei_config()");
	TRACE_EVENT(evt_string);

	/* Initialize token parsing */
	tok_init(evt_string);

	/* Parse next keyword and number of variables */
	
	return PEI_OK;
	
#endif /* #ifndef NCONFIG */
}

/*******************************************************************************\
*  FUNCTION:	btu_pei_config
*                                                                         
*  PARAMETERS:	char *cfg_string: configuration string;
*				char *dummy: dummy parameter.
*                                                                         
*  DESCRIPTION:	This function is used to set configuration values of a protocol
*				entity.     
*                                                                         
*  RETURNS:		SHORT PEI_OK/PEI_ERROR - primitive processed/not processed.
\*******************************************************************************/
GLOBAL SHORT btu_pei_config(char *cfg_string, char *dummy)
{
	pei_config(cfg_string);

	return PEI_OK;
}

/*******************************************************************************\
*  FUNCTION:	pei_monitor
*                                                                         
*  PARAMETERS:	void **p_monitor: address of monitor structure.
*                                                                         
*  DESCRIPTION:	With this function, the environment requests the address of the
*				protocol entity's monitor structure with relevant physical para-
*				meters which could be monitored in real time.
*                                                                         
*  RETURNS:		SHORT PEI_OK/PEI_ERROR - primitive processed/not processed.
\*******************************************************************************/
LOCAL SHORT pei_monitor(void **p_monitor)
{
	TRACE_FUNCTION("pei_monitor()");

	btu_mon.info = INFO_BTU;
	*p_monitor = &btu_mon;

	return PEI_OK;
}

/*******************************************************************************\
*  FUNCTION:	btu_pei_create
*                                                                         
*  PARAMETERS:	T_PEI_INFO **p_info: entity's setup data.
*                                                                         
*  DESCRIPTION:	This function exports the start-up configuration data of a pro-
*				tocol stack entity and to register this entity in the system, so,
*				that the frame can allocate the required system resources, such
*				as an input queue or a number of timers, know the addresses of
*				the other PEI functions and can start the task with specified
*				priority and stack size.
*				This function should be called before any other pei_() function.             
*                                                                         
*  RETURNS:		SHORT PEI_OK - success.
\*******************************************************************************/
GLOBAL SHORT btu_pei_create(T_PEI_INFO **p_info)
{
	static const T_PEI_INFO pei_info =
	{
		"BTU",
		{
			pei_init,
			pei_exit,
			pei_primitive,
			pei_timeout,
			pei_signal,
			pei_run,
			pei_config,
			pei_monitor,
		},
		BTU_STACK_SIZE,									/* Stack Size */
		BTU_QUEUE_SIZE,									/* Queue Entries */
		BTU_PRIORITY,									/* Priority (0-low, 255-high) */
		BTU_NUM_OF_TIMERS,								/* Number of timers */
		COPY_BY_REF | TRC_NO_SUSPEND | PRIM_NO_SUSPEND  | INT_DATA_TASK 	/* Flags: */
														/* Active Body */
														/* Copy by reference */
														/* Discard traces if no room */
														/* Discard primitives if no room */
														/* allocate task stack and queue memory from internal RAM pool */
	};

	TRACE_FUNCTION("pei_create()");

	/* Close Resources if open */
	if (first_access)
	{
		first_access = FALSE;
	}
	else
	{
		pei_exit();
	}

	/* Export startup configuration data */
	*p_info = (T_PEI_INFO *)&pei_info;

	return PEI_OK;
}

#endif /* #ifndef BTU_PEI_C */

