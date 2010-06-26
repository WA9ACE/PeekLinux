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
*   FILE NAME:      btl_debug.h
*
*   DESCRIPTION:    This file defines various debug utilities
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/


#ifndef __BTL_DEBUG_H
#define __BTL_DEBUG_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bttypes.h"


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

#undef BTL_DEBUG_CANCEL_RADIO_ON 

/* Unmark this line to enable debugging Cancel Radio On feature */
/* #define BTL_DEBUG_CANCEL_RADIO_ON  */

#ifdef BTL_DEBUG_CANCEL_RADIO_ON

/* These are user defined for Debugging Cancel RadioOn feature */
typedef enum _BtlDebugCancelRadioOnIndx 
{
    BTL_DEBUG_STATE_WAITING_FOR_CHIP_MNGR_AND_ME_HCI_INIT_EVENT_IDX,                  
    BTL_DEBUG_ABORT_ARRIVED_TOO_LATE_IDX,                  
    BTL_DEBUG_STATE_BMG_PREPARING_FOR_RADIO_ON_IDX,                  
    BTL_DEBUG_UNLOAD_SCRIPT_IDX,           
    BTL_DEBUG_THRESHOLD_IDX, 

    BTL_DEBUG_FEATURE_DISABLED_IDX, 

    BTL_DEBUG_UNDEFINED_CP_USER_IDX = 0xfffe,
    BTL_DEBUG_UNDEFINED_CP_INTEREST_IDX = 0xffff

} BtldebugCancelRadioOnIndx;


/*
 * Cancel Radio ON operation test names
 */
#define CHECK_POINT_NAMES {                                                        \
    { "CP_STATE_WAITING_FOR_HCI" }, \
    { "CP_ABORT_ARRIVED_TOO_LATE" }, \
    { "CP_BMG_PREPARING_RADIO_ON" }, \
    { "CP_UNLOAD_INIT_SCRIPT" }, \
    { "CP_ABORT_INIT_SCRIPT_THRESHOLD" }, \
    { "CP_TEST_MODE_DISABLED" }, \
    { 0 } }


void BTL_DEBUG_CroCheckPoint(BtldebugCancelRadioOnIndx index);
void BTL_DEBUG_CroClearInitScriptCounter(void);
void BTL_DEBUG_CroIncrementInitScriptCounter(void);

#define BTL_DEBUG_CRO_INIT_SCRIPT_THRESHOLD         20

#define BTL_DEBUG_CRO_CHECK_POINT_SLEEP_TIME        100
#define BTL_DEBUG_CRO_CHECK_POINT_POLL_TIMER        10

   
#define BTL_DEBUG_CRO_CHECK_POINT(index)            BTL_DEBUG_CroCheckPoint(index)
#define BTL_DEBUG_CRO_CLEAR_COUNTER()               BTL_DEBUG_CroClearInitScriptCounter()
#define BTL_DEBUG_CRO_INCREMENT_COUNTER()           BTL_DEBUG_CroIncrementInitScriptCounter()

#else

#define BTL_DEBUG_CRO_CHECK_POINT(index)
#define BTL_DEBUG_CRO_CLEAR_COUNTER()               
#define BTL_DEBUG_CRO_INCREMENT_COUNTER()            

#endif


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/


char* BTL_DEBUG_pObStatus(I8 obStatus);

char* BTL_DEBUG_pBppEvent(U8 event);
char* BTL_DEBUG_pBppOp(U16 bppOp);
char* BTL_DEBUG_pBppChannel(U8 channel);

char* BTL_DEBUG_pBtStatus(BtStatus status);

#endif /* __BTL_DEBUG_H */


