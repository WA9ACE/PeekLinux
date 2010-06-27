	/******************************************************************************
	 * Power Task (pwr)
	 * Design and coding by Svend Kristian Lindholm, skl@ti.com
	 *
	 * PWR MMI Interface
	 *
	 * $Id: pwr_api.c 1.1 Wed, 20 Aug 2003 10:22:37 +0200 skl $
	 *
	 ******************************************************************************/

#include "lcc/lcc.h"
#include "lcc/lcc_env.h"
#include "lcc/lcc_trace.h"
#include "rv/rv_defined_swe.h"
#include "lcc/lcc_task.h"
#include "drv_power.h"

/* Global variables */
extern uint16 lcc_charger_status;
extern uint16 lcc_USB_status;

	/******************************************************************************
	 * Function prototypes
	 ******************************************************************************/

	T_RVM_RETURN pwr_start_timer(UINT32 *timer_begin);

	// Inform the PWR process which callback function to send MMI events
	// and perform initialization of mmi related variables
	void pwr_register (T_RV_RETURN_PATH *return_path, void *ptr) {
	    ttw(ttr(TTrInit,"pwr_register  (%d)" NL, 0x00));

          while (!pwr_ctrl) rvf_delay(2);

	    // Assign return path (callback or task address id)
	    pwr_ctrl->rpath.callback_func = return_path->callback_func;
	    pwr_ctrl->rpath.addr_id       = return_path->addr_id;

	    // Pointer to mmi event memory allocated (& deallocated) by the MMI
	    pwr_ctrl->mmi_ptr = (struct mmi_info_ind_s *) ptr;

	    // Yes - the mmi has registered!
	    pwr_ctrl->flag_mmi_registered = 1;

           pwr_abb_interrupt_handler(BSP_TWL3029_INTC_SOURCE_ID_BATT_CHARGER);
	    // Start the mmi repetition timer
	    pwr_start_timer(&pwr_ctrl->time_begin_mmi_rep);
	    ttw(ttr(TTrTimerLow,"mmi repetition timer started(%d)" NL, 0));

	    ttw(ttr(TTrInit,"pwr_register  (%d)" NL, 0xFF));
}

uint8  lcc_pwr_interface(uint8 command)
{
	switch(command)
	{
	 case 0 : /* Clock Mask */
		{
			if((pwr_ctrl->state == CAL)||(pwr_ctrl->state == INI)||(pwr_ctrl->state == SUP))
				{
				return NO_CLOCK;
				}
			else  //if state is PRE,CCI,LCI,CCV or LCV   
				{
				return DCXO_CLOCK;
				}
	 	}
	 case 1 : /* Sleep Command */
			return SUCCESS;
	 case 2 : /* Wakeup Command */
	 		return SUCCESS;
	 default :
		 return FAILURE;
	} 
	}


uint16 lcc_get_batteryVotage(void)
{
   if(pwr_ctrl->state > CAL) {
	return(pwr_cfg->data.Vbat_avg_mV);
   }
   else {
   	return 0;
   }
}

uint16 lcc_get_chgPlugStatus(void)
{
   return lcc_charger_status;   
}

uint16 lcc_get_usbPlugStatus(void)
{
   return lcc_USB_status;   
}

int8 lcc_get_state(void)
{
    switch(pwr_ctrl->state) {
       case CAL:
	   	return 0;

	case PRE:
		return 1;

	case INI:
		return 2;

	case SUP:
		return 3;

	case CCI:
		return 4;

	case CCV:
		return 5;

	default:
		return -1;
    }
}

void lcc_get_values(uint16 *lcc_info)
{
   if(pwr_ctrl->state > CAL) {
      *lcc_info++ = pwr_capacity(pwr_cfg->data.Vbat_avg_mV);
      *lcc_info++ = pwr_temp_lookup(pwr_cfg->data.Tbat_avg, 0x01);
      *lcc_info++ = pwr_cfg->data.Vbat_avg_mV;
      *lcc_info++ = pwr_cfg->data.Ichg;
      *lcc_info++ = pwr_cfg->data.Vchg;
   }
   else {
      lcc_info = NULL;
   }
}


void lcc_get_timerStatus(int32 *timer_sts)
{   
   if(pwr_ctrl->state < SUP) {
      timer_sts = NULL;
   }
   else {
	*timer_sts++ = (int32)pwr_ctrl->time_elapsed_T1;
	*timer_sts++ = (int32)pwr_ctrl->time_elapsed_T2;
	*timer_sts++ = (int32)pwr_ctrl->time_elapsed_T3;
	*timer_sts++ = (int32)pwr_ctrl->time_elapsed_T4;
	*timer_sts++ = (int32)pwr_ctrl->time_elapsed_mmi_rep;
	} 
	}
