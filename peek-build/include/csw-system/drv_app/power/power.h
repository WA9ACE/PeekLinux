/*******************************************************************************
 *
 * power.h
 *
 *
 * (C) Texas Instruments 2001
 *
 ******************************************************************************/

#ifndef __POWER_H__
#define __POWER_H__

#include "rv/rv_defined_swe.h"	   // for RVM_PWR_SWE


typedef unsigned short BspTwl3029_IntC_SourceId;
void Power_Key_Interrupt(BspTwl3029_IntC_SourceId id);

/* Prototypes */
void Power_ON_Button(void);
void Power_ON_Remote(void);
void Switch_ON(void);
void Switch_Off(void);
void Set_Switch_ON_Cause(void);
void Power_IT_WakeUp(void);
void Power_OFF_Button(void);
void Power_OFF_Remote(void);

#ifdef RVM_PWR_SWE
  void PWR_Charger_Plug(void);
  void PWR_Charger_Unplug(void);
#endif /* #ifdef RVM_PWR_SWE */

#endif /* __POWER_H__ */
