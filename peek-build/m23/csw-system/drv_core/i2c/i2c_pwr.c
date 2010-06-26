/**
 * @file	i2c_pwr.c
 *
 * Power management implementation for i2c.
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
#include "i2c_pwr.h"
#include "drv_power.h"
#include "bspI2c.h"

extern volatile Bool bspI2c_busLocked[BSP_I2C_NUM_DEVICES];

Uint8  i2c_pwr_interface(Uint8 command)
{
	switch(command)
	{
	 case 0 : /* Clock Mask */
		{
			if(sleepFlag||bspI2c_busLocked[0]==TRUE
			#if(LOCOSTO_LITE==0)
			 	||(bspI2c_busLocked[1]==TRUE)
			#endif
					)
				{
				return DCXO_CLOCK;
				}
			else
				{
				return NO_CLOCK;
				}
	 	}
	 case 1 : /* Sleep Command */
			{
			if( (!sleepFlag) &&(bspI2c_busLocked[0]==FALSE)
				#if(LOCOSTO_LITE==0)	
				   &&(bspI2c_busLocked[1]==FALSE)
                            #endif
		 	           )

                     {
			/* Disable I2C enable bit*/
			
			return SUCCESS;
		 	}
			else
				{
				return FAILURE;
				}
		}
	 case 2 : /* Wakeup Command */
	 	{
	 		/* Enable I2C enable bit */
		
			return SUCCESS;
	 	}
		
	 default :
		 return FAILURE;
	} 
}

#if (L1_POWER_MGT != 0x00)
void i2c_vote_deepsleepstatus(void)
{
		Update_Sleep_Status( I2C_ID, sleepFlag);
}
#endif


