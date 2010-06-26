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
*   FILE NAME:      btav.h
*
*   DESCRIPTION:    Constant definitions and function prototypes for
*                   the btav_pei.c module 
*
*   AUTHOR:          Zvi Schneider
*
\*******************************************************************************/

#ifndef BTAV_H
#define BTAV_H

/* Macro definitions */

/* The info is a part of the monitor struct. */
#define T_INFO char
#define INFO_BTAV				"BTAV"

/* BTAV entity setup parameters */
#define BTAV_STACK_SIZE			2996	/* Size of the BTAV entity's stack */
#define BTAV_QUEUE_SIZE			10		/* Amount of entries in the BTAV queue */
#if BTAV_QUEUE_SIZE < BTHAL_OS_MAX_NUM_OF_EVENTS_A2DP

#error BTAV_QUEUE_SIZE must be greater than BTHAL_OS_MAX_NUM_OF_EVENTS_A2DP

#endif


#define BTAV_PRIORITY			200		/* Priority of the BTAV entity */
#define BTAV_NUM_OF_TIMERS		1		/* BTAV may maintain a list of virtual */
										/* timers by using only one 'real' timer */
										/* from the OS */
#define BTAV_INFINITE_WAIT		0xFFFFFFFF	/* In Nucleus OS = NU_SUSPEND */

/* Opcode of the events type definitions for which are written below */
#define BTAV_EVT_ENABLE_BT		1		/* Enable Bluetooth functionality */
#define BTAV_EVT_DISABLE_BT		2		/* Disable Bluetooth functionality */

/* Exported variables */
extern int btav_handle;					/* BTAV handle used for communication	
										from other entities to BTAV */			

/* Types definitions */

/* Structure used for monitoring physical parameters */
typedef struct
{
  T_INFO *info;
} T_MONITOR;

#endif /* BTAV_H */

