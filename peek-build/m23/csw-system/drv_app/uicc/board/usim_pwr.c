/**
 * @file	usim_pwr.c
 *
 * Power management implementation for USIM.
 *
 * @author	 Saumar J Dutta
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
#include "drv_power.h"
#include "usim_pwr.h"
#include "bspUicc_Phy.h"
#include "bspUicc.h"
#include "bspUicc_Phy_llif.h"
#include "bspUicc_Phy_map.h"
#include "bspUicc_Power.h"
#include "bspUicc_Lnk.h"
#include "bspUicc_LnkSM.h"
extern Uint8 card_present;
extern BspUicc_Phy_Ctrl    bspUicc_Phy_ctrl;
extern BspUicc_ConfigChars configCharsPtr; //added for SIM CLK cutoff support
extern volatile SYS_BOOL bspUicc_sClk_running;
extern BspUicc_LnkSM_State bspUicc_LnkSM_state;

Uint8 usim_pwr_interface(Uint8 command)
{
	switch(command)
	{
	 case 0 : /* Clock Mask */
	 	      /* Check if SCLK is inactive */
		 if ((bspUicc_LnkSM_state != BSP_UICC_LNK_SM_STATE_CONFIGURED)||
		 	(bspUicc_sClk_running == FALSE)&&((bspUicc_Phy_ctrl.sClkLev != BSP_UICC_PHY_SCLOCK_NO_STOP) ||
		 	( (configCharsPtr.clkStopPreference & BSP_UICC_CLOCK_STOP_MASK) != BSP_UICC_CLOCK_STOP_NOT_SUPPORTED)) )
		 {
			 return NO_CLOCK;
		 }
		 else
		 {
		 	if(card_present!=1)  
			 return NO_CLOCK;
			else 
 			 return DCXO_CLOCK;
		 }
	
	 case 1 : /* Sleep Command */
		/* disable usim interface. OMAPS00077014:  removed due to impact on SIM Tcs */
		 return SUCCESS;
	
	 case 2 : /*Wakeup Command */
	/* enable usim interface. OMAPS00077014: removed due to impact on SIM Tcs */
		 return SUCCESS;
	 
	 default :
		 return SUCCESS;
	} 
}


#if (L1_POWER_MGT != 0x00)
void Usim_Vote_DeepSleepStatus()
{
	if(bspUicc_Phy_ctrl.sClkLev != BSP_UICC_PHY_SCLOCK_NO_STOP)
	{
		Update_Sleep_Status(USIM_ID,ASLEEP);
	}
	else
	{
		Update_Sleep_Status(USIM_ID,AWAKE);
	}
}
#endif

