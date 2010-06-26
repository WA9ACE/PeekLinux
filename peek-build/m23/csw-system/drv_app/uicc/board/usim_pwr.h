/**
 * @file	usim_pwr.h
 *
 * Power management implementation for USIM.
 *
 * @author	Saumar J Dutta
 * @version 0.1
 */
/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */
#ifndef	USIM_PWR_H_
#define	USIM_PWR_H_

#include "general.h"
#include "types.h"


#if (L1_POWER_MGT != 0x00)
void Usim_Vote_DeepSleepStatus(void);
#endif

Uint8 usim_pwr_interface(Uint8);

#endif

