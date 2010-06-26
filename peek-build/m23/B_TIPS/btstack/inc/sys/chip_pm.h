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
*   FILE NAME:      chip_pm.h
*
*   DESCRIPTION:    Constant definitions and function prototypes for
*                   the chip_pm.c module.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/

#ifndef CHIP_PM_H
#define CHIP_PM_H


#include "xatypes.h"


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

void CHIP_PM_Init(BOOL isChipOn);
void CHIP_PM_Deinit(void);
void CHIP_PM_ChipOn(void);
void CHIP_PM_ChipOff(void);
void CHIP_PM_UartGoToSleep(void);
void CHIP_PM_UartWakeUp(void);
BOOL CHIP_PM_UartIsWakeUpInd(void);


#endif /* CHIP_PM_H */


