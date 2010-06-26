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
*   FILE NAME:      bthal_pm.c
*
*   DESCRIPTION:    Implementation of participation of BT Host in platform's
*					power management mechanism.
*
*   AUTHOR:         V. Abram
*
\*******************************************************************************/


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "btl_defs.h"
#include "bthal_common.h"
#include "bthal_pm.h"
#include "bthal_log.h"
#include "uart.h"
#include "bthal_log_modules.h"

#define BTHAL_PM_LOG_INFO(msg)			BTHAL_LOG_INFO(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_PM, msg)
#define BTHAL_PM_LOG_FUNCTION(msg)		BTHAL_LOG_FUNCTION(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_PM, msg)

/****************************************************************************
 *
 * Defines
 *
 ***************************************************************************/
#if XA_DEBUG == XA_ENABLED
#define BTHAL_Assert(exp)  (((exp) != 0) ? (void)0: BTHAL_UTILS_Assert(#exp,__FILE__,(U16)__LINE__))
#else
#define BTHAL_Assert(exp) (void)0
#endif

/* Device can NOT go to sleep */
#define BTHAL_PM_SLEEP_STATE_AWAKE								(0x00)

/* Device can go to sleep */
#define BTHAL_PM_SLEEP_STATE_ASLEEP								(0x01)



#define CMD_ACTIVITY_STATUS    (0)
#define RESULT_ACTIVE          (1)
#define RESULT_INACTIVE        (0)
#define RESULT_FAILURE         (0)
#define RESULT_SUCCESS         (1)

/****************************************************************************
 *
 * Global Data
 *
 ***************************************************************************/
static  BthalPmSleepState uartPmSleepState= BTHAL_PM_SLEEP_STATE_ASLEEP;


 /* UART */
void BthalPmUartRxEventHandler(void);






/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTHAL_PM_Init()
 *
 *		Initializes participation of BT Host in platform's power management
 *		mechanism.
 */
BthalStatus BTHAL_PM_Init(BthalCallBack	callback)
{
	BthalStatus bthalStatus;
	BTHAL_PM_LOG_INFO(("Initializing BT Host Power Manager"));
	//bthalStatus = BTHAL_PM_ChangeSleepState(BTHAL_PM_PERIPHERAL_DEVICE_UART, BTHAL_PM_SLEEP_STATE_ASLEEP);
	return BTHAL_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTHAL_PM_Deinit()
 *
 *		Deinitializes participation of BT Host in platform's power management
 *		mechanism.
 */
BthalStatus BTHAL_PM_Deinit()
{
	BTHAL_PM_LOG_INFO(("Deinitializing BT Host Power Manager"));
         /* We can allow the system to go to sleep, since the BT is deactivated */
	return BTHAL_STATUS_SUCCESS;
}
/*-------------------------------------------------------------------------------
 * BTHAL_PM_ChangeSleepState()
 *
 *		Indicates the given peripheral device(s) can go to sleep or wake up.
 */
BthalStatus BTHAL_PM_ChangeSleepState(BthalPmPeripheralDeviceMask deviceMask,
										BTHAL_U8 sleepState)
{
    BTHAL_PM_LOG_FUNCTION(("BTHAL_PM: ChangeSleepState"));

    BTHAL_Assert(BTHAL_PM_PERIPHERAL_DEVICE_UART == deviceMask);

    switch (sleepState)
    {
        case BTHAL_PM_SLEEP_STATE_AWAKE:

            if (deviceMask & BTHAL_PM_PERIPHERAL_DEVICE_UART)
            {
               uartPmSleepState = BTHAL_PM_SLEEP_STATE_AWAKE;
               UA_WakeUp(UA_UART_0);
            }


            break;

        case BTHAL_PM_SLEEP_STATE_ASLEEP:

            if (deviceMask & BTHAL_PM_PERIPHERAL_DEVICE_UART)
            {
               	uartPmSleepState = BTHAL_PM_SLEEP_STATE_ASLEEP;
               //Go to Sleep
                UA_EnterSleep(UA_UART_0);
            }


            break;

        default:
            BTHAL_Assert(0);
            break;
    }
	return BTHAL_STATUS_SUCCESS;
}


/*---------------------------------------------------------------------------
 * BthalPmUartRxEventHandler()
 *
 * Synopsis:  IRQ handler for UART RX event.
 *
 */
void BthalPmUartRxEventHandler()
{
	/* BTHAL_PM_LOG_FUNCTION(("BTHAL_PM: BthalPmUartRxEventHandler )); */

      /* LISR context, activate RX HISR */
	//BTHAL_Assert(BTHAL_UART_wakeup_hisr() == OS_OK);
	if(uartPmSleepState == BTHAL_PM_SLEEP_STATE_ASLEEP)
         BTHAL_UART_wakeup_hisr();
}

/*---------------------------------------------------------------------------
 *            HandleSleepManagerReq()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Interface to inform state to Locosto PM
 *
 *
 */
U8 BTHAL_PM_HandleSleepManagerReq(U8 ucCmd)
{
  U8 ucResult;

  //ucResult= RESULT_ACTIVE;

  if (CMD_ACTIVITY_STATUS == ucCmd)
  {
    if (BTHAL_PM_SLEEP_STATE_ASLEEP == uartPmSleepState)
    {
      ucResult = RESULT_INACTIVE;
    }
    else
    {
      ucResult = RESULT_ACTIVE;
    }
  }
  else
  {
      ucResult = RESULT_SUCCESS;
  }

  return (ucResult);
}