/******************************************************************************
 * Power Config (LCC)
 * Design and coding by Laurent Sollier
 *
 * LCC Configuration
 *
 * lcc_cfg.h 1.0 Mon, 15 Dec 2003
 *
 ******************************************************************************/

#ifndef _LCC_CFG_H_
#define _LCC_CFG_H_
#include "rvm/rvm_priorities.h"

	/* This include is just needed for _RVF (Traces) */
#include "ffs.cfg"
#include "chipset.cfg"

#ifdef RVM_LCC_TASK_PRIORITY
#define LCC_TASK_PRIORITY RVM_LCC_TASK_PRIORITY
#else
#define LCC_TASK_PRIORITY (220)
#endif

#define TRACE_WANTED 1
#define USE_PWL_AS_MODULATOR 1
#define TEST_PWR_MMI_INTERFACE 0
#if (ANLG_FAM==11)
#define USE_Q401_CHG_CIRCUIT 0
#define USE_BCI_CIRCUIT      1
	//The following 3 switches Enables/Disables different types of charging
	//Enabling both CI_CV  and PWM will result in enabling only PWM instead of CV charging.
#define ENABLE_USB_CHARGING 1
#define ENABLE_CI_CV_OR_PWM 1 				//((ENABLE_AC_CI_CV_CHARGING == 1) ||(ENABLE_AC_PWM_CHARGING == 1))
											//charging mode(CI_CV or PWM) is decided based on charger type
#else
#define USE_Q401_CHG_CIRCUIT 1
#endif
#define USE_Q402_CHG_CIRCUIT 0
#endif
