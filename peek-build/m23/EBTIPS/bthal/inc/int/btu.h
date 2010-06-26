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
*   FILE NAME:      btu.h
*
*   DESCRIPTION:    Constant definitions and function prototypes for
*                   the btu_pei.c module.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/

#ifndef BTU_H
#define BTU_H

/* Macro definitions */

/* The info is a part of the monitor struct. */
#define T_INFO char
#define INFO_BTU				"BTU"

/* BTU entity setup parameters */
#define BTU_STACK_SIZE			4096	/* Size of the BTU entity's stack */
#define BTU_QUEUE_SIZE			10		/* Amount of entries in the BTU queue */
#if BTU_QUEUE_SIZE < BTHAL_OS_MAX_NUM_OF_EVENTS_TRANSPORT

#error BTU_QUEUE_SIZE must be greater than BTHAL_OS_MAX_NUM_OF_EVENTS_TRANSPORT

#endif


#define BTU_PRIORITY			201		/* Priority of the BTU entity */
#define BTU_NUM_OF_TIMERS		0		/* No timers are needed in BTU */

/* Time the BTU may sleep waiting for */
/* GPF signal - then driver's events */
/* will be checked */
#ifdef WIN32
#define BTU_SIGNAL_WAIT_TIME	100			/* 100 ms */
#else
#define BTU_SIGNAL_WAIT_TIME	0xFFFFFFFF	/* Infinity */																													
#endif	/* WIN32 */

/* Time the BTU may sleep inside Windows function */
#ifdef WIN32
#define BTU_DRIVER_WAIT_TIME	10			/* 10 ms */
#endif	/* WIN32 */	

/* Opcode of the events type definitions for which are written below */
#define BTU_RX_EVENT 1
#define BTU_TX_EVENT 2

/* Exported variables */
extern int btu_handle;			/* BTU handle used for communication	*/
								/* from other entities to BTU */			

/* Types definitions */

/* Structure used for monitoring physical parameters */
typedef struct
{
  T_INFO *info;
} T_MONITOR;


#endif /* BTU_H */

