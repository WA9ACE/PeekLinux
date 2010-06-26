/******************************************************************************
 * Power Task (pwr)
 * Design and coding by Svend Kristian Lindholm, skl@ti.com
 *
 * Timers
 *
 * $Id: pwr_handle_timers.c 1.1 Wed, 20 Aug  2003 10:22:37 +0200 skl $
 *
 ******************************************************************************/
#ifndef __PWR_HANDLE_TIMERS_C_
#define __PWR_HANDLE_TIMERS_C_

UINT32 rvf_get_tick_count(void);
void pwr_check_timers(void);
T_RVM_RETURN pwr_create_timer(UINT32 *timer_begin);
T_RVM_RETURN pwr_start_timer(UINT32 *timer_begin);
void pwr_modulate_off(void);
void pwr_send_msg(uint32 msg_id ,T_RVF_ADDR_ID src_addr_id, T_RVF_ADDR_ID  dest_addr_id);
int pwr_capacity(uint16 Vbat);
void mmi_send_msg(struct mmi_info_ind_s *event);
void end_charging_house_keeping(void);

// Timer functions

	/****************************************************************
	*Function: pwr_timer_elapsed
	*Description:
	*	-return elapsed time in [ms]
	*
	* parameters
	*	time_begin 	: the time when the timer started
	*	current_timer : the current time
	* return
	*	the elapsed time in ms
	*
	******************************************************************/
UINT32 pwr_timer_elapsed(UINT32 time_begin, UINT32 current_timer)
{

    return (current_timer - time_begin) * 60 / 13;
}

	/****************************************************************
	*Function: pwr_start_timer
	*Description:
	*	-stores the current OS tick count to "timer_begin"
	* parameters
	*	 timer_begin : a pointer to the variable for storing the OS tick count for a particular timer  
	* return
	*	RV_OK
	*
	******************************************************************/
T_RVM_RETURN pwr_start_timer(UINT32 *timer_begin)
{
    ttw(ttr(TTrTimer, "pwr_start_timer(%d)" NL, 0));

    // Start timer (will be checked each time an ADC measurement is received)
    *timer_begin  = rvf_get_tick_count();

    ttw(ttr(TTrTimer, "pwr_start_timer(%d)" NL, 0xFF));
    return (RV_OK);
}

	/****************************************************************
	*Function: pwr_start_timer
	*Description:
	*	-stores the current OS tick count to "timer_begin"
	* parameters
	*	 timer_begin : a pointer to the variable for storing the OS tick count for a particular timer  
	* return
	*	RV_OK
	*
	******************************************************************/
T_RVM_RETURN pwr_stop_timer(UINT32 *timer_begin)
{
    ttw(ttr(TTrTimer, "pwr_stop_timer(%d)" NL, 0));

    *timer_begin  = 0;

    ttw(ttr(TTrTimer, "pwr_stop_timer(%d)" NL, 0xFF));
    return (RV_OK);
}

	/****************************************************************
	*Function: pwr_stop_timers
	*Description:
	*	-stops the timers T1,T2,T3 and mod_cycle
	* parameters
	*	 None
	* return
	*	None 
	*
	******************************************************************/
void pwr_stop_timers(void)
{
    // Only stop charging related timers - not supervision timers

    pwr_stop_timer(&pwr_ctrl->time_begin_T1);
    pwr_stop_timer(&pwr_ctrl->time_begin_T2);
    pwr_stop_timer(&pwr_ctrl->time_begin_T3);
    #if(ANLG_FAM !=11)
    pwr_stop_timer(&pwr_ctrl->time_begin_T4);
    #endif

}

	/****************************************************************
	*Function: pwr_create_timer
	*Description:
	*	-creates a timer .
		- stores a zero to the variable used to store the timer begin OS count
	* parameters
	*	 timer_begin  : a pointer to the variable for storing the OS tick count when a timer starts  
	* return
	*	RV_OK
	*
	******************************************************************/
T_RVM_RETURN pwr_create_timer(UINT32 *timer_begin)
{
    ttw(ttr(TTrTimer, "pwr_create_timer(%d)" NL, 0));

    *timer_begin  = 0;

    ttw(ttr(TTrTimer, "pwr_create_timer(%d)" NL, 0xFF));
    return (RV_OK);
}

	/****************************************************************
	*Function: process_pwr_handle_T1_expiration
	*Description:
	*	- Handles the total charging time timer expiry.
	*	- stops charging and clears the flags
	* parameters
	*	  a pointer of type T_PWR_REQ  
	* return
	*	RV_OK
	*
	******************************************************************/

T_RVM_RETURN process_pwr_handle_T1_expiration (T_PWR_REQ *request)
{
    ttw(ttr(TTrTimer, "process_pwr_handle_T1_expiration(%d)" NL, 0));

    switch (pwr_ctrl->state)
    {
    case SUP :
        break;
    case CCI :
    case CCV :
    case LCI :
    case LCV :
        end_charging_house_keeping();
        break;
    default :
        {
        // Exception Handling - Unknown State
        ttr((unsigned int)TTrFatal, "process_pwr_handle_T1_expiration: Unknown State: %d" NL, pwr_ctrl->state);
        break;
        }
    }
    ttw(ttr(TTrTimer, "process_pwr_handle_T1_expiration(%d)" NL, 0xFF));
    pwr_free(request);

    return (RV_OK);	
}

	/****************************************************************
	*Function: process_pwr_handle_T2_expiration
	*Description:
	*	- Handles the CV charging time timer expiry.
	*	- stops charging and clears the flags
	* parameters
	*	  a pointer of type T_PWR_REQ  
	* return
	*	RV_OK
	*
	******************************************************************/
T_RVM_RETURN process_pwr_handle_T2_expiration (T_PWR_REQ *request)
{
    ttw(ttr(TTrTimer, "process_pwr_handle_T2_expiration(%d)" NL, 0));
    switch (pwr_ctrl->state)
    {
    case SUP :
        // Another timer did expire prior to the T2 timer - ignore this expiration
        break;
    case CCV :
    case LCV :
        // Change state
         	end_charging_house_keeping();
	break;
    default :
        {
        // Exception Handling - Unknown State
        ttr((unsigned int)TTrFatal, "process_pwr_handle_T2_expiration: Unknown State: %d" NL, pwr_ctrl->state);
        break;
        }
    }
    pwr_free(request);
    ttw(ttr(TTrTimer, "process_pwr_handle_T2_expiration(%d)" NL, 0xFF));

    return (RV_OK);	
}

	/****************************************************************
	*Function: process_pwr_handle_T3_expiration
	*Description:
	*	- Handles the T3 timer expiry.
	*	- stop and reset timers  
	* parameters
	*	  a pointer of type T_PWR_REQ  
	* return
	*	RV_OK
	*
	******************************************************************/
T_RVM_RETURN process_pwr_handle_T3_expiration (T_PWR_REQ *request)
{
    ttw(ttr(TTrTimer, "process_pwr_handle_T3_expiration(%d)" NL, 0));
    switch (pwr_ctrl->state)
    {
    case SUP :
        // Stop & reset timers
        pwr_stop_timers();
        break;
    case CCI :
    case CCV :
    case LCI :
    case LCV :

        	// Exception Handling - Unknown State
        	ttr((unsigned int)TTrFatal, "process_pwr_handle_T3_expiration: Invalid State: %d" NL, pwr_ctrl->state);
        	break;
    	default :
        // Exception Handling - Unknown State
        ttr((unsigned int)TTrFatal, "process_pwr_handle_T3_expiration: Unknown State: %d" NL, pwr_ctrl->state);
        break;
        }
    pwr_free(request);
    ttw(ttr(TTrTimer, "process_pwr_handle_T3_expiration(%d)" NL, 0xFF));

    return (RV_OK);	
}

#if(ANLG_FAM !=11)
T_RVM_RETURN process_pwr_handle_T4_expiration (T_PWR_REQ *request)
{
    ttw(ttr(TTrTimer, "process_pwr_handle_T4_expiration(%d)" NL, 0));
    switch (pwr_ctrl->state) {
    case SUP :
        // Yes - since timing issues can make this timer expire in SUP state
        // It can take a long time before we know if a charger has been unplugged
        break;
    case CCV :
    case LCV :
        pwr_stop_timer(&pwr_ctrl->time_begin_T4);
        pwr_modulate_off();
        // Wait for duty cycle timer to expire before restarting this timer
        break;
    default :
        {
        // Exception Handling - Unknown State
        ttr((unsigned int)TTrFatal, "process_pwr_handle_T4_expiration: Unknown State: %d" NL, pwr_ctrl->state);
        break;
        }
    }
    pwr_free(request);
    ttw(ttr(TTrTimer, "process_pwr_handle_T4_expiration(%d)" NL, 0xFF));

    return (RV_OK);			
}
#endif

	/****************************************************************
	*Function: process_pwr_handle_mmi_info_expiration
	*Description:
	*	- Handles the modulation cycle  timer expiry.
	*	- calculates new duty cycle based on the current battery measurement and
	*	   updates the triton registers accordingly.		
	* parameters
	*	  a pointer of type T_PWR_REQ  
	* return
	*	RV_OK
	*
	******************************************************************/
T_RVM_RETURN process_pwr_handle_mmi_info_expiration(T_PWR_REQ *request)
{
    struct mmi_info_ind_s MMI_Event;

    ttw(ttr(TTrTimer, "process_pwr_handle_mmi_info_expiration(%d)" NL, 0));
    switch (pwr_ctrl->state) {
    case SUP :
        // Send MMI info events according to configured repetition interval
        if (pwr_ctrl->flag_mmi_registered == 1) {
            pwr_ctrl->mmi_ptr->header.msg_id = MMI_BAT_SUPERVISION_INFO_IND;
            pwr_ctrl->mmi_ptr->Vbat = pwr_cfg->data.Vbat_avg_mV;
            pwr_ctrl->mmi_ptr->Tbat = pwr_cfg->data.Tbat_avg;
            pwr_ctrl->mmi_ptr->Cbat = pwr_capacity(pwr_cfg->data.Vbat_avg_mV);
            mmi_send_msg(pwr_ctrl->mmi_ptr);
        }
        // Restart timer
        ttw(ttr(TTrTimerLow,"MMI info timer started(%d)" NL, 0));
        pwr_start_timer(&pwr_ctrl->time_begin_mmi_rep);

        break;
    case INI :
    case CCI :
    case CCV :
    case LCI :
    case LCV :
        // DON'T send any events when charging or initializing
        break;
    default :
        {
        // Exception Handling - Unknown State
        ttr((unsigned int)TTrFatal, "process_pwr_handle_mmi_info_expiration: Unknown State: %d" NL, pwr_ctrl->state);
        break;
        }
    }
    pwr_free(request);
    ttw(ttr(TTrTimer, "process_pwr_handle_mmi_info_expiration(%d)" NL, 0xFF));

    return (RV_OK);	
}

	/****************************************************************
	*Function:  pwr_check_timers
	*Description:
	*	- checks every timer for expiry.
	*	- Posts a message to  LCC if any timer has expired.	
	* parameters
	*	  None 
	* return
	*	  None
	*
	******************************************************************/
void pwr_check_timers()
{
    UINT32 timer;

    ttw(ttr(TTrTimer, "pwr_check_timers(%d)" NL, 0));
    // Check timers T1, T2, T3, T4, duty cycle & mmi timer -  if expired send timer expired events
    timer  = rvf_get_tick_count();

    // T1
    if (pwr_ctrl->time_begin_T1 != 0) {
        // Started - but is it expired?
        pwr_ctrl->time_elapsed_T1  = pwr_timer_elapsed(pwr_ctrl->time_begin_T1, timer);
	ttw(ttr(TTrEventLow, "T1 elapsed (%d) < (%d):" NL, pwr_ctrl->time_elapsed_T1,pwr_cfg->bat.T1));
        if (pwr_ctrl->time_elapsed_T1 > pwr_cfg->bat.T1) {
            // EXPIRED!
            ttw(ttr(TTrTimer, "T1 expired (%d):" NL, pwr_ctrl->time_elapsed_T1));
            // 'Stop' timer
            pwr_ctrl->time_elapsed_T1 = 0;
            pwr_ctrl->time_begin_T1   = 0;
		     pwr_ctrl->flag_topping_chg = 1; // Apply topping charge frow now on and until charger is unplugged
            // Send timer expired event
            pwr_send_msg(TIMER_T1_EXPIRED, pwr_ctrl->addr_id, pwr_ctrl->addr_id);
        }
    }
    // T2
    if (pwr_ctrl->time_begin_T2 != 0) {
        // Started - but is it expired?
        pwr_ctrl->time_elapsed_T2  = pwr_timer_elapsed(pwr_ctrl->time_begin_T2, timer);
	 ttw(ttr(TTrAll, "T2 elapsed (%d) < (%d):" NL, pwr_ctrl->time_elapsed_T2,pwr_cfg->bat.T2));
        if (pwr_ctrl->time_elapsed_T2 > pwr_cfg->bat.T2) {
            // EXPIRED!
            ttw(ttr(TTrAll, "T2 expired (%d):" NL, pwr_ctrl->time_elapsed_T2));
            // 'Stop' timer
            pwr_ctrl->time_elapsed_T2 = 0;
            pwr_ctrl->time_begin_T2   = 0;
		     pwr_ctrl->flag_topping_chg = 1; // Apply topping charge frow now on and until charger is unplugged		
            // Send timer expired event
            pwr_send_msg(TIMER_T2_EXPIRED, pwr_ctrl->addr_id, pwr_ctrl->addr_id);
        }
    }
    // T3
    if (pwr_ctrl->time_begin_T3 != 0) {
        // Started - but is it expired?
        pwr_ctrl->time_elapsed_T3  = pwr_timer_elapsed(pwr_ctrl->time_begin_T3, timer);
        if (pwr_ctrl->time_elapsed_T3 > pwr_cfg->bat.T3) {
            // EXPIRED!
	            ttw(ttr(TTrAll, "T3 expired (%d):" NL, pwr_ctrl->time_elapsed_T3));
            // 'Stop' timer
            pwr_ctrl->time_elapsed_T3 = 0;
            pwr_ctrl->time_begin_T3   = 0;
            // Send timer expired event
            pwr_send_msg(TIMER_T3_EXPIRED, pwr_ctrl->addr_id, pwr_ctrl->addr_id);
        }
    }

   #if(ANLG_FAM !=11)
	// T4
    if (pwr_ctrl->time_begin_T4 != 0) {
        // Started - but is it expired?
        pwr_ctrl->time_elapsed_T4  = pwr_timer_elapsed(pwr_ctrl->time_begin_T4, timer);
        if (pwr_ctrl->time_elapsed_T4 > pwr_cfg->data.T4) {
            // EXPIRED!
            ttw(ttr(TTrTimer, "T4 expired (%d):" NL, pwr_ctrl->time_elapsed_T4));
            // 'Stop' timer
            pwr_ctrl->time_elapsed_T4 = 0;
            pwr_ctrl->time_begin_T4   = 0;
            // Send timer expired event
            pwr_send_msg(TIMER_T4_EXPIRED, pwr_ctrl->addr_id, pwr_ctrl->addr_id);
        }
    }
    #endif

    // MMI repetition timer
    if (pwr_ctrl->time_begin_mmi_rep != 0) {
        // Started - but has it expired?
        pwr_ctrl->time_elapsed_mmi_rep  = pwr_timer_elapsed(pwr_ctrl->time_begin_mmi_rep, timer);
        if (pwr_ctrl->time_elapsed_mmi_rep > pwr_cfg->mmi.repetition) {
            // EXPIRED!
            // 'Stop' timer
            pwr_ctrl->time_elapsed_mmi_rep = 0;
            pwr_ctrl->time_begin_mmi_rep   = 0;
            ttw(ttr(TTrTimer, "MMI info timer expired (%d):" NL, pwr_ctrl->time_elapsed_mmi_rep));
            // Send timer expired event
            pwr_send_msg(TIMER_MMI_INFO_EXPIRED, pwr_ctrl->addr_id, pwr_ctrl->addr_id);
        }
    }
    ttw(ttr(TTrTimer, "pwr_check_timers(%d)" NL, 0xFF));

}
#endif
