/**
 * @file	camera_pwr.c
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

#include "nucleus.h"
#include "camera_pwr.h"

extern T_CAMD_ENV_CTRL_BLK *camd_env_ctrl_blk_p;
#define  I_sample 27
UINT8 camera_pwr_interface(UINT8 command)
{






	switch(command)
	{
	 case 0 : /* Clock Mask */
		{
			if(camd_env_ctrl_blk_p->state == CAMD_CAMERA_DISABLED)
				return NO_CLOCK;
			else
				return DPLL_CLOCK;
	 	}
		
	 case 1 : /* Sleep Command */
	 {
		switch(camd_env_ctrl_blk_p->state)
    {
    case CAMD_CAMERA_DISABLED:
#if I_sample==27
			AI_ConfigBitAsOutput(10);	
			AI_SetBit(10);
#else
           AI_ConfigBitAsOutput(17);	
			AI_SetBit(17);

#endif
			return SUCCESS;

		case CAMD_CAMERA_ENABLED:
		case CAMD_VIEWFINDER_MODE:
		case CAMD_SNAPSHOT_MODE:
			return FAILURE;
		default:
			return SUCCESS;
		
	 }
	 }
	 case 2 : /* Wakeup Command */
#if I_sample==27
			AI_ConfigBitAsOutput(10);	
			AI_ResetBit(10);
#else
AI_ConfigBitAsOutput(17);	
			AI_ResetBit(17);


#endif
		return SUCCESS;
	 default :
		 return SUCCESS;
	} 
}


#if (L1_POWER_MGT != 0x00)
void camera_vote_deepsleepstatus(void)
{

	switch(camd_env_ctrl_blk_p->state)
    {
    case CAMD_CAMERA_DISABLED:
			Update_Sleep_Status( CAMERA_ID, FALSE);
		break;
		
		case CAMD_CAMERA_ENABLED:
		case CAMD_VIEWFINDER_MODE:
		case CAMD_SNAPSHOT_MODE:
		default:
			Update_Sleep_Status( CAMERA_ID, TRUE);
		break;
	 }
	 
}
#endif
