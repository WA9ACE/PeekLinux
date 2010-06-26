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
*   FILE NAME:      bthal_uart.c
*
*   DESCRIPTION:    This file is the HCI Hardware driver for the Locosto platform.
*
*   AUTHOR:         Ordina - Rene Kuiken
*   Revision:       Incorporated fixes for UART Data blocking issue 
*                   and Power Management Issue - Sasken, Gayathri 
*
\*******************************************************************************/


/****************************************************************************
 *
 * Include files
 *
 ****************************************************************************/
#include <nucleus.h>

#define BTHAL_UART_PRAGMAS
#include "bthal_config.h"

#include "osapi.h"
#include "bthal_uart.h"
#include "utils.h"
#include "inth/sys_inth.h"
#include "uart.h"
#include "hcill.h"
#include <xatypes.h>
#include <osapi.h>
#include "btu.h"
#include <os.h>
#include "bthal_pm.h"


/****************************************************************************
 *
 * Declarations of external definitions and functions from the file vsi.h
 *
 ****************************************************************************/

#ifdef MEMORY_SUPERVISION
	#define FILE_LINE_TYPE      ,const char *file, int line
	#define FILE_LINE_MACRO		,__FILE__,__LINE__
#else
	#define FILE_LINE_TYPE
	#define FILE_LINE_MACRO
#endif

#define BTS_TX_INT
/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

#ifdef DEMO
    #undef Report
    #define Report(S)
#endif


/****************************************************************************
 *
 * Global Data
 *
 ***************************************************************************/

/* Callback provided by the stack */
static UartCallback uartEventHandler = NULL;

/* Event for scheduling eventThread. */
static OS_HANDLE evTrans;
/* Event definitions for evTrans */
#define EV_TX           			0x00000001 	/*TX Event: To transmit data to the UART*/
#define EV_RX           			0x00000002 	/*RX Event: When Data is received on UART*/
#define EV_RX_WAKEUP		0x00000004	/*Rx Wakeup Event: When there is a Wakeup interrupt due to UART Rx activity*/
#define EV_EXIT         			0x80000000   /*Exit Event: To exit from event thread*/
#define EV_ALL_EVENTS   (EV_TX | EV_RX |EV_RX_WAKEUP| EV_EXIT)

    /* RX HISR variables */
    #define RX_HISR_PRIORITY        2       /* 0=high, 1=medium, 2=low */
    /* TX HISR variables */
    #define TX_HISR_PRIORITY        2       /* 0=high, 1=medium, 2=low */

/* WakeUp HISR variables */
#define RXWAKEUP_HISR_PRIORITY        2       /* 0=high, 1=medium, 2=low */


/* N Shutdown BRF6150 */
GLOBAL UINT32 NSHUT_GPIO = 37;

static NU_TASK uartTcb;

/* HISR variables */
static NU_HISR rxHisr;
static NU_HISR rxWakeupHisr;
#ifdef BTS_TX_INT
static NU_HISR txHisr;
#endif

/* Standard Nucleus allocate size */
#define HISR_STACK_SIZE     1024
#define WAKEUP_HISR_STACK_SIZE 	256

U8   RXHISR_stack[HISR_STACK_SIZE];
U8   RXWAKEUPHISR_stack[WAKEUP_HISR_STACK_SIZE];
#ifdef BTS_TX_INT
U8   TXHISR_stack[HISR_STACK_SIZE];
#endif


/* RX variables */
static U8* rxBuf;
static int rxLen;

/* TX variables */
static U8* txBuf;
static int txLen;

#ifdef BTS_TX_INT
/*************************************************
 * 				INTERRUPT IDENTIFCATION REGISTER
 *************************************************
 * Bit 0 is set to 0 if an IT is pending.
 * Bits 1 and 2 are used to identify the IT
 *************************************************/
#define IIR_MASK    						(0x3F)
#define RX_DATA          					(0x04)
#define TX_EMPTY         					(0x02)
#define RX_TIMEOUT       					(0x0C)
#endif

/****************************************************************************
 *
 * Local Prototypes
 *
 ***************************************************************************/
static BOOL isRadioPresent(void);
#ifdef BTS_TX_INT
static void xLisrFunc(SYS_UWORD8 iirStatus);
#endif
#ifndef BTS_TX_INT
static void xLisrFunc(void);
#endif
static void rxHisrFunc(void);
static void rxWakeupHisrFunc(void);
static void txHisrFunc(void);

/*---------------------------------------------------------------------------
 * BTHAL_UART_Init()
 *
 *     Called by the stack (during EVM_Init) to initialize the UART hardware
 *     driver.
 * 	   Returns:
 *	   	BTHAL_STATUS_SUCCESS - Indicates that initialization was successful.
 *     	BTHAL_STATUS_PENDING - Initialization is pending.
 *     	BTHAL_STATUS_FAILED - Initialization failed.
 *
 *	   Context: BTS
 */
BthalStatus BTHAL_UART_Init(UartCallback func)
{
    STATUS  status;

    /* Resets the BRF-6150 by toggling its hardware reset pin, for at least 5ms. */
    AI_ConfigBitAsOutput((int)NSHUT_GPIO);
    AI_ResetBit((int)NSHUT_GPIO);
	/* Delay for ~ 8 ms - The Actual sleep time measured is *~ 4 */

	NU_Sleep(2);

    memset (RXHISR_stack, 0xFE, HISR_STACK_SIZE);
    memset (RXWAKEUPHISR_stack, 0xFE, WAKEUP_HISR_STACK_SIZE);
#ifdef BTS_TX_INT
    memset (TXHISR_stack, 0xFE, HISR_STACK_SIZE);
#endif

	/* Save the callback to the stack */
	uartEventHandler = func;

    status = NU_Create_HISR(&rxHisr,
   						    "btRxHisr",
						    rxHisrFunc,
						    RX_HISR_PRIORITY,
						    RXHISR_stack,
	                        HISR_STACK_SIZE);

    if (status != NU_SUCCESS)
    {
        Report(("UART: NU_Create_HISR failed: %d.\n", status));
        return BTHAL_STATUS_FAILED;
	}

 status = NU_Create_HISR(&rxWakeupHisr,
   						    "btRxWakeupHisr",
						    rxWakeupHisrFunc,
						    RXWAKEUP_HISR_PRIORITY,
						    RXWAKEUPHISR_stack,
	                        		    WAKEUP_HISR_STACK_SIZE);

    if (status != NU_SUCCESS)
    {
        Report(("UART: NU_Create_HISR failed for RxWakeup HISR: %d.\n", status));
        return BTHAL_STATUS_FAILED;
	}

#ifdef BTS_TX_INT
    /* Create the TX HISR */
	 status = NU_Create_HISR(&txHisr,
	                         "btTxHisr",
	                         txHisrFunc,
	                         TX_HISR_PRIORITY,
	                         TXHISR_stack,
	                         HISR_STACK_SIZE);

    if (status != NU_SUCCESS)
    {
		Report(("UART: NU_Create_HISR failed: %d.\n", status));
		return BTHAL_STATUS_FAILED;
    }
#endif

      /*Performs basic UART hardware initialization including sleep mode */
	initialize_uart_sleep(UA_UART_0);

	/* Configure the port */
    UA_Init( UA_UART_0, TR_BAUD_115200, xLisrFunc);
	AI_SetBit((int)NSHUT_GPIO);
	/* Delay for ~ 12 ms - The Actual sleep time measured is *~ 4 */
	NU_Sleep(3);
	/* Make sure there's actually a radio present */
	if (isRadioPresent() == FALSE) {
		return BTHAL_STATUS_FAILED;
	}

	uartEventHandler(UE_INIT_COMPLETE);

	return (BTHAL_STATUS_PENDING);

}


/*---------------------------------------------------------------------------
 * UART_EnterSleep() 
 *      Allow UART to enter Sleep.
 *
 *	Context: BTU
 */
void UART_EnterSleep(void)
{
	UA_EnterSleep(UA_UART_0);
}

/*---------------------------------------------------------------------------
 * UART_WakeUp() 
 *     Wakeup the UART.
 *
 *	Context: BTS
 */
void UART_WakeUp(void)
{
		UA_WakeUp(UA_UART_0);
}

/*---------------------------------------------------------------------------
 * BTHAL_UART_Shutdown()
 *      Release any resources, close the connection if open.
 *
 *	Context: BTS
 */
BthalStatus BTHAL_UART_Shutdown(void)
{
   	/* Delete the HISR Structures */
	NU_Delete_HISR(&rxHisr);
	NU_Delete_HISR(&rxWakeupHisr);
#ifdef BTS_TX_INT
	NU_Delete_HISR(&txHisr);
#endif

	uartEventHandler = NULL;
	/* As BT is shutting down, no more UART activity 
	 * shall happen. So we can allow the UART to go 
	 * Deep Sleep state
	 */

         UART_EnterSleep();

	/* Now set the HCILL state to ASLEEP
	  * This is required in case when BRF6150 
	  * has an active ACL connection and Bluetooth 
	  * is switched- off by the user forcefully :
	  * in this case BRF6150 does not send the 
	  *HCILL_GO_TO_SLEEP indication to the host, 
         * so Host state is not ASLEEP. Do this explicitly 
         * so that BT can allow system-deep sleep
         */ 
	 
	 BTHAL_PM_ChangeSleepState(BTHAL_PM_PERIPHERAL_DEVICE_UART, BTHAL_PM_SLEEP_STATE_ASLEEP);
         return (BTHAL_STATUS_SUCCESS);
}



#ifdef BTS_TX_INT
/*---------------------------------------------------------------------------
 * xLisrFunc()
 *      Low Level ISR called when an rx or tx interrupt fires from the uart.
 *      It triggers the HISR "rxHisrFunc" or "txHisrFunc".
 */
static void xLisrFunc(SYS_UWORD8 iirStatus)
{
	/* Mask unecessary bits. */
	switch (iirStatus & IIR_MASK)
	{
		case RX_TIMEOUT:
		case RX_DATA:
	    		NU_Activate_HISR (&rxHisr);
	    		break;

		case TX_EMPTY:
			NU_Activate_HISR (&txHisr);
			break;
	}
}
#endif
#ifndef BTS_TX_INT
/*---------------------------------------------------------------------------
 * xLisrFunc()
 *      Low Level ISR called when an rx interrupt fires from the uart.
 *      It triggers the HISR "rxHisrFunc".
 */
static void xLisrFunc(void)
{
   NU_Activate_HISR (&rxHisr);
}
#endif

/*---------------------------------------------------------------------------
 * BTHAL_UART_wakeup_hisr()
 *      High Level ISR called from f_inth_uart_wakeup() in sys_inth32.c, when we 
 *	  received a wakeup interrupt due to a UART RX activity. 
 *	  This activates "rxWakeupHisr" to process the EV_RX_WAKEUP event 
 *	  in BTU_eventThread(). 
 */
 void BTHAL_UART_wakeup_hisr(void)
{
	NU_Activate_HISR (&rxWakeupHisr);
}

/*---------------------------------------------------------------------------
 * rxHisrFunc()
 *      High Level ISR called when an rx interrupt fires from the uart.
 *      It is scheduled once per rx int and will run after the fifo has
 *      been read into the serial driver's buffer.
 *      Signals our task to run.
 */
static void rxHisrFunc(void)
{
    /*Report(("UART: In rxHisrFunc.\n"));*/
    AssertEval(os_SetEvents(evTrans, EV_RX) == OS_OK);
}

/*---------------------------------------------------------------------------
 * rxWakeupHisrFunc() 
 *      High Level ISR called when a wakeup interrupt fires from the uart Rx activity.
 *      Signals BTU to process the event EV_RX_WAKEUP.
 */
static void rxWakeupHisrFunc(void)
{
    /*Report(("UART: In rxWakeupHisrFunc.\n"));*/
		AssertEval(os_SetEvents(evTrans, EV_RX_WAKEUP) == OS_OK);
	}

/*---------------------------------------------------------------------------
 * BTHAL_UART_Read()
 *      Read bytes from the UART
 *
 * Parameters:
 *      buffer  allocated buffer to receive the bytes
 *      length  max number of bytes to read
 *
 * Returns:
 *      number of bytes read.
 *
 *	Context: BTU
 */
BthalStatus BTHAL_UART_Read(unsigned char *buffer, unsigned short length, unsigned short *readBytes)
{
  int bytesRead = 0; /* function return. */

/* Read data from the UART. */
	bytesRead = UA_ReadNChars( UA_UART_0, (char *)buffer, length);
      /*Report (("UART: Read %d from RxBuffer. \n", bytesRead));*/
  *readBytes = bytesRead;
  return (BTHAL_STATUS_SUCCESS);

}/* end of UART_Read() */

/*---------------------------------------------------------------------------
 * UART_Write()
 *      Write a buffer to the UART
 *
 * Parameters:
 *      buffer  data to send
 *      length  bytes in buffer
 *
 * Returns:
 *      Number of bytes written.
 *
 *	Context: BTS, BTU
 */
BthalStatus BTHAL_UART_Write(const BTHAL_U8 *buffer, BTHAL_U16 length, BTHAL_U16 *writtenBytes)
{
    Assert(txBuf == 0);

/* Report(("UART: UART_Write sends %d bytes.", length));*/
	/* Save the buffer pointer and length. Signal BTU_eventThread to do the I/O */
	txBuf = (U8*)buffer;
	txLen = length;
    /* Send TX EVENT to BTU entity, to start the I/O */
        AssertEval(os_SetEvents(evTrans, EV_TX) == OS_OK);

	*writtenBytes = length;
    return (BTHAL_STATUS_PENDING);
}

/*---------------------------------------------------------------------------
 * txHisrFunc() 
 *      High Level ISR called when an tx interrupt fires to the uart when
 *      the serial drivers tranmit buffer is empty.
 */
static void txHisrFunc(void)
{
    Report(("UART: In txHisrFunc.\n"));
        AssertEval(os_SetEvents(evTrans, EV_TX) == OS_OK);
}

/*---------------------------------------------------------------------------
 * BTHAL_UART_SetSpeed()
 *     Called to change the speed of the UART.
 *
 * Parameters:
 *     speed - the speed (in KBps) at which the UART should operate.
 *
 * Context : BTS
 */
BthalStatus BTHAL_UART_SetSpeed(BthalUartSpeed speed)
{
    T_tr_Baudrate baudrate;


    Report(("UART: UART_SetSpeed %d ", speed));

	switch (speed) {
	case 0 :
		baudrate = TR_BAUD_115200;
		Report((" Baud rate 0 !!! :TR_BAUD_115200"));
		break;
	case BTHAL_UART_SPEED_115200:
		baudrate = TR_BAUD_115200;
		Report((" Baud rate :TR_BAUD_115200"));
		break;
	case BTHAL_UART_SPEED_230400:
		baudrate = TR_BAUD_230400;
		Report((" Baud rate :TR_BAUD_230400"));
		break;
	case BTHAL_UART_SPEED_460800:
		baudrate = TR_BAUD_460800;
		Report((" Baud rate :TR_BAUD_460800"));
		break;
	case BTHAL_UART_SPEED_921600:
		baudrate = TR_BAUD_921600;
		Report((" Baud rate :TR_BAUD_921600"));
		break;
	case BTHAL_UART_SPEED_1843200:
		baudrate = TR_BAUD_1843200;
		Report((" Baud rate :TR_BAUD_1843200"));
		break;
	case BTHAL_UART_SPEED_3686400:
		baudrate = TR_BAUD_3686400;
		Report((" Baud rate :TR_BAUD_3686400"));
		break;
	default:
		baudrate = TR_BAUD_115200;
		Report((" Baud rate default :TR_BAUD_115200"));
		break;
	}

    /* Configure the port */
    UA_Init( UA_UART_0, baudrate, xLisrFunc);

	return BTHAL_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------------
 * BTHAL_UART_EventThread()
 *    This thread watches for Serial events.  It receives events :
 *	TX Event: To transmit data to the UART or when a write completed 
 *	RX Event: When Data is received on UART
 *	Rx Wakeup Event: When there is a Wakeup interrupt due to UART Rx activity
 *     On receiving the above events, the upper transport layer is notified.
 */
void BTHAL_UART_EventThread(void)
{
    LONG status;
    unsigned event;
    int numWritten;
    unsigned int uartEvent;

    Report(("UART: eventThread initialized.\n"));

    /* Create an event group for communication with eventTask */
    status = os_CreateEventGroup("evBTTran", &evTrans);
    Assert(status == OS_OK);

    
	while (1) {

        /* Wait until a notification event arrives */

	status = os_RetrieveEvents(evTrans,EV_ALL_EVENTS, NU_OR_CONSUME, &event, NU_SUSPEND);
	Assert((status == OS_OK));
	
        if (! uartEventHandler) {
            continue;
        }

        OS_StopHardware();
        uartEvent |= event;
		
        if (uartEvent & EV_RX) {
            /* We have data to read.
             * Since the upper transport may not have an rx buffer free,
             * bytes received may not be read; therefore, EV_RX is cleared
             * in UART_Read
             */
   	      /*Report(("UART: In eventThread. RX Event!\n"));*/
	     uartEventHandler(UE_DATA_TO_READ);
            uartEvent &= ~EV_RX;
        }

        if (uartEvent & EV_TX) {
            /* We have data to write to the port. */

            uartEvent &= ~EV_TX;

	    if (txLen == 0){
           	txBuf = 0;
             uartEventHandler(UE_WRITE_COMPLETE);
	     /*Report(("UART: In eventThread. TX Write Complete!\n"));*/
            } else {
              numWritten = UA_WriteNChars( UA_UART_0, (char *)txBuf, txLen);
		txBuf += numWritten;
		txLen -= numWritten;
	   	/*Report(("UART: In eventThread. TX written %d bytes.\n", numWritten));*/
#ifndef BTS_TX_INT
              txHisrFunc();
#endif
           }
        }
		
	if(uartEvent & EV_RX_WAKEUP){
	  /*	We have received a wakeup interrupt due to a UART RX activity
	   *	It is seen that when waking from deep sleep the first byte on UART 
	   *	is always lost. For a BT wakeup scenario, the first byte sent by BRF 
	   *	to host upon wakeup is always HCILL_WAKE_UP_IND whic is lost.
	   *	This is a work-around to send HCILL_WAKE_UP_ACK to BRF.
	   */
	   /*Report(("UART: In eventThread. RX Wakeup Event!\n"));*/
	 uartEventHandler(UE_DATA_TO_READ);
	uartEvent &= ~EV_RX_WAKEUP;
	}

        OS_ResumeHardware();
    }
/* commented since the statement is unreachable as BTU_eventThread is active */
//	os_DeleteEventGroup(evTrans);
    
} /* End of BTHAL__UART_EventThread() */



/*---------------------------------------------------------------------------
 * isRadioPresent()
 *
 *     Determines whether there is a device connected to our serial port.
 *
 * Returns:
 *      TRUE    Yes, a device was detected
 *      FALSE   No device is present
 */
static BOOL isRadioPresent(void)
{
    /* We could check the receive-line-signal-detect signal to determine
     * whether any hardware is connected. The signal should be high when the
     * connection is live. However, some null-modem cables don't seem to
     * support this so we'll just return TRUE.
     *
     * DWORD lineStatus;
     * GetCommModemStatus(commPort, &lineStatus);
     * if ((lineStatus & MS_RLSD_ON)==0) {
     *    return FALSE;
     * }
     */
    return TRUE;

}

/*---------------------------------------------------------------------------
 * BTHAL_UART_ResetRxFifo() 
 *     Called to clear the UART RX FIFO.
 *
 * Requires:
 *
 * Parameters:
 *     none.
 *
 * Returns:
 */
BthalStatus BTHAL_UART_ResetRxFifo()
{
char buffer;
	Report(("BTHAL_UART: Reset RX FIFO.\n"));

   /*As per the BSP Locosto team, whenever HCILL_IsWakeUpInd becomes true, RxFifo will be empty.
      So no need to flush the FIFO buffer*/
      
     while(UA_ReadNChars( UA_UART_0, &buffer, 1))
     {
     	 //Read contineously until there is no data available
     }
    return BTHAL_STATUS_SUCCESS;
}


