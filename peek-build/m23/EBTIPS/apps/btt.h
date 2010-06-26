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
*   FILE NAME:      btt.h
*
*   DESCRIPTION:    Constant definitions and function prototypes for
*                   the btt_pei.c module 
*
*   AUTHOR:         V. Abram
*
\*******************************************************************************/

#ifndef BTT_H
#define BTT_H

/* Macro definitions */


/* The info is a part of the monitor struct. */
#define T_INFO char
#define INFO_BTT				"BTT"

/* BTT entity setup parameters */
#define BTT_STACK_SIZE			4096	/* Size of the BTT entity's stack */
#define BTT_QUEUE_SIZE			10		/* Amount of entries in the BTT queue */



#define BTT_PRIORITY			90		/* Priority of the BTT entity */
#define BTT_NUM_OF_TIMERS		0		/* BTT may maintain a list of virtual */

										/* timers by using only one 'real' timer */
										/* from the OS */
#define BTT_INFINITE_WAIT		0xFFFFFFFF	/* In Nucleus OS = NU_SUSPEND */

/* Opcode of the events type definitions for which are written below */
#define BTT_EVT_ENABLE_BT		1		/* Enable Bluetooth functionality */
#define BTT_EVT_DISABLE_BT		2		/* Disable Bluetooth functionality */

/* Exported variables */
extern int btt_handle;					/* BTT handle used for communication	
										from other entities to BTT */			

/* Types definitions */

/* Structure used for monitoring physical parameters */
typedef struct
{
  T_INFO *info;
} T_MONITOR;

#endif /* #ifndef BTT_H */
