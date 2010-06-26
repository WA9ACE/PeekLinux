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
*   FILE NAME:      hcill.h
*
*   DESCRIPTION:    Constant definitions and function prototypes for
*                   the hcill.c module.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/

#ifndef HCILL_H
#define HCILL_H

#include "overide.h"
#include <sys/uarttran.h>
#include <chip_pm.h>

#if TI_CHANGES == XA_ENABLED

/* Need host UART inactivity timer only if the BT itself does not keep such a timer */
#define HCILL_UART_INACTIVITY_TIMER		XA_DISABLED


/* HCILL packet indicators (the first and only byte in an HCILL packet) */
typedef U8 HcillPacketInd;

#define HCILL_GO_TO_SLEEP_IND	0x30
#define HCILL_GO_TO_SLEEP_ACK	0x31
#define HCILL_WAKE_UP_IND		0x32
#define HCILL_WAKE_UP_ACK		0x33 

/*---------------------------------------------------------------------------
 * HcillPacket structure
 *
 *     Represents a complete HCILL packet to be transmitted.
 */
typedef struct _HcillPacket 
{
    ListEntry    node;			/* Node element for linked list management. */

	/* Must be right after the node field as in HciPacket */
	HciBufFlags flags;			/* Flags that identify the type of the packet */

    HcillPacketInd packetType;	/* Identify the type of HCILL packet */
} HcillPacket;


/* Total number of TX HCILL packets */
#define HCILL_NUM_PACKETS	3


/* HCILL state */
typedef U8 HcillState;

#define HCILL_AWAKE					0
#define HCILL_WAIT_FOR_SLEEP_ACK	1
#define HCILL_ASLEEP				2
#define HCILL_WAIT_FOR_WAKE_UP_ACK	3 


/* HCILL Context */
typedef struct _BtHcillContext 
{
    ListEntry       hcillPacketPool;		/* Available HCILL TX packets    */

    HcillPacket     hcillPackets[HCILL_NUM_PACKETS]; 

    ListEntry		tempTxQueue;			/* Temp TX queue */

	U8				deferredPacketsCounter;	/* Number of deferred packets */

	HcillState		state;					/* Current state of the HCILL state machine */

#if HCILL_UART_INACTIVITY_TIMER == XA_ENABLED
	EvmTimer		inactivityTimer;		/* Inactivity timer for UART */
#endif /* HCILL_UART_INACTIVITY_TIMER == XA_ENABLED */
	
} BtHcillContext;


#if XA_CONTEXT_PTR == XA_ENABLED

extern BtHcillContext* hcill;
#define HCILL(s)  (hcill->s)

#else /* XA_CONTEXT_PTR == XA_ENABLED */

extern BtHcillContext bt;
#define HCILL(s)  (hcill.s)

#endif /* XA_CONTEXT_PTR */


#if HCILL_UART_INACTIVITY_TIMER == XA_ENABLED
#define HCILL_UART_INACTIVITY_TIMEOUT	300		/* in ms */
#endif /* HCILL_UART_INACTIVITY_TIMER == XA_ENABLED */


/* Function prototypes */
void HCILL_Init(void);
HcillPacketInd HCILL_GetTxType(HciPacket *packet);
void HCILL_PacketSent(HciPacket *packet);
BOOL HCILL_IsTxBlocked(HciPacket *packet);
BOOL HCILL_ReceivePacket(HcillPacketInd packetType);
BOOL HCILL_IsWakeUpInd(void);
#define HCILL_IsWakeUpInd() CHIP_PM_UartIsWakeUpInd()

#if HCILL_UART_INACTIVITY_TIMER == XA_ENABLED
void HCILL_StartInactivityTimer(void);
void HCILL_CancelInactivityTimer(void);
#endif /* HCILL_UART_INACTIVITY_TIMER == XA_ENABLED */

#endif /* TI_CHANGES == XA_ENABLED */

#endif /* HCILL_H */


