/**
 * @file	camera_pwr.h
 *
 * Power management implementation for camera.
 *
 * @author	
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

#include "Armio.h" 
#include "camd/camd_i.h"
#include "drv_power.h"


#ifndef CAMERA_PWR_H_
#define CAMERA_PWR_H_

#define TRUE 1
#define FALSE 0

#if (L1_POWER_MGT != 0x00)
void camera_vote_deepsleepstatus(void);
#endif

UINT8 camera_pwr_interface(UINT8);


extern void Update_Sleep_Status( UINT8, UINT8);

#endif

