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
*   FILE NAME:      fms_task.h
*
*   DESCRIPTION:    Constant definitions and function prototypes for
*                   the fms_pei.c module 
*
*   AUTHOR:         G. Keren
*
\*******************************************************************************/

#ifndef FMS_TASK_H
#define FMS_TASK_H

/* Macro definitions */

/* The info is a part of the monitor struct. */
#define T_INFO char
#define INFO_FMS				"FMS"

/* BTS entity setup parameters */
#define FMS_STACK_SIZE			4096	/* Size of the FMS entity's stack */
#define FMS_QUEUE_SIZE			10		/* Amount of entries in the FMS queue */
#if FMS_QUEUE_SIZE < BTHAL_OS_MAX_NUM_OF_EVENTS_FM

#error FMS_QUEUE_SIZE must be greater than BTHAL_OS_MAX_NUM_OF_EVENTS_FM

#endif


#define FMS_PRIORITY			190		/* Priority of the FMS entity */
#define FMS_NUM_OF_TIMERS		1		/*  */
#define FMS_INFINITE_WAIT		0xFFFFFFFF	/* In Nucleus OS = NU_SUSPEND */

/* Opcode of the events type definitions for which are written below */
#define FMS_EVT_NOTIFY			1		/* Notify FMS that it needs processing */
#define FMS_EVT_ENABLE			2		/* Enable FM functionality */
#define FMS_EVT_DISABLE			3		/* Disable FM functionality */

/* Exported variables */
extern int fms_handle;					/* fms handle used for communication	*/
										/* from other entities to FMS */			
/* Types definitions */

/* Structure used for monitoring physical parameters */
/* Structure used for monitoring physical parameters */
typedef struct
{
  T_INFO *info;
} T_MONITOR;

#endif /* #ifndef FMS_H */

