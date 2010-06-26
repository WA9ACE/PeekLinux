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
*   FILE NAME:      bts.h
*
*   DESCRIPTION:    Constant definitions and function prototypes for
*                   the bts_pei.c module 
*
*   AUTHOR:         V. Abram
*
\*******************************************************************************/

#ifndef BTS_H
#define BTS_H

/* Macro definitions */

/* The info is a part of the monitor struct. */
#define T_INFO char
#define INFO_BTS				"BTS"

/* BTS entity setup parameters */
#define BTS_STACK_SIZE			4096	/* Size of the BTS entity's stack */
#define BTS_QUEUE_SIZE			10		/* Amount of entries in the BTS queue */
#if BTS_QUEUE_SIZE < BTHAL_OS_MAX_NUM_OF_EVENTS_STACK

#error BTS_QUEUE_SIZE must be greater than BTHAL_OS_MAX_NUM_OF_EVENTS_STACK

#endif


#define BTS_PRIORITY			199		/* Priority of the BTS entity */
#define BTS_NUM_OF_TIMERS		2		/* one timer for the stack and second for bthal_mc */
										/* BT stack maintains a list of virtual */
										/* timers by using only one 'real' timer */
										/* from the OS */
#define BTS_INFINITE_WAIT		0xFFFFFFFF	/* In Nucleus OS = NU_SUSPEND */

/* Opcode of the events type definitions for which are written below */
#define BTS_EVT_NOTIFY_EVM		1		/* Notify Event Manager that it needs processing */
#define BTS_EVT_ENABLE			2		/* Enable Bluetooth functionality */
#define BTS_EVT_DISABLE			3		/* Disable Bluetooth functionality */

/* Exported variables */
extern int bts_handle;					/* BTS handle used for communication	*/
										/* from other entities to BTS */			
/* Types definitions */

/* Structure used for monitoring physical parameters */
typedef struct
{
  T_INFO *info;
} T_MONITOR;

#endif /* #ifndef BTS_H */
