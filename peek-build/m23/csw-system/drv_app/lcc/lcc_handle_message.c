/******************************************************************************
 * Power Task (pwr)
 * Design and coding by Svend Kristian Lindholm, skl@ti.com
 *
 * PWR Message Processing
 *
 * $Id: pwr_handle_message.c 1.3 Wed, 20 Aug 2003 15:17:25 +0200 skl $
 *
 ******************************************************************************/
#include <string.h>
#include "lcc/lcc.h"
#include "lcc/lcc_api.h"
#include "lcc/lcc_trace.h"
#include "lcc/lcc_task.h"
#include "lcc/lcc_tm_i.h"
#include "lcc/lcc_cfg_i.h"
#include "lcc/lcc_cfg.h"
#include "lcc/lcc_modulate.h"
#include "rv/rv_defined_swe.h"
#include "ffs/ffs.h"
#include "etm/etm_env.h"
#include "chipset.cfg"

#if (ANLG_FAM!=11)
#include "abb/abb.h"
#else
#include "abb/bspTwl3029.h"
#include "abb/bspTwl3029_I2c.h"
#include "abb/bspTwl3029_Intc.h"
#include "abb/bspTwl3029_Madc.h"
#include "abb/bspTwl3029_Bci.h"
#include "abb/bspTwl3029_Aux_Map.h"

#include "typedefs.h"
#include "vsi.h"


//#include "abb/bspTwl3029_Power.h"

#endif

/******************************************************************************
 * Function prototypes
 ******************************************************************************/

#ifndef EMO_PROD
void ttr(unsigned trmask, char *format, ...);
void str(unsigned mask, char *string);
#endif

void *pwr_malloc(int size);

T_RVM_RETURN pwr_task_init(void);

UINT32 pwr_timer_elapsed(UINT32 time_begin, UINT32 current_timer);
void build_name(const char *ch_pre, char *cfg_id , UINT8 index, const char * ch_post,  char * name);

void pwr_modulate_init(void);
void pwr_modulate_on(void);
int pwr_capacity(uint16 Vbat);

void pwr_check_timers();
void pwr_stop_timers();
	int pwr_check_topping();

T_RVM_RETURN pwr_start_timer(UINT32 *timer_begin);
T_RVM_RETURN pwr_stop_timer(UINT32 *timer_begin);

T_RVM_RETURN pwr_check_files(void);
T_RVM_RETURN pwr_read_files(void);
T_RVM_RETURN pwr_read_chg_files(void);
T_RVM_RETURN pwr_read_cal_files(void);

void pwr_send_msg(uint32 msg_id ,T_RVF_ADDR_ID src_addr_id, T_RVF_ADDR_ID  dest_addr_id);
void mmi_send_msg(struct mmi_info_ind_s *event);

void start_q401_charge(void);
void start_q402_charge(void);
void start_bci_charge(BspTwl3029_Bci_pathType path_type);
void start_pre_charge(BspTwl3029_Bci_pathType path_type);
void stop_q401_charge(void);
void stop_q402_charge(void);
void stop_bci_charge(void);
void stop_pre_charge(void);
void charger_plug_house_keeping(void);
void charger_unplug_house_keeping(void);
void USB_unplug_house_keeping(void);
void cv_charging_house_keeping(BspTwl3029_Bci_pathType path_type);
void update_duty_cycle(void);
void end_charging_house_keeping(void);
void stop_bci_linCharge(void);
void stop_bci_pwmCharge(void);
int check_chg_unplug(void);
void dummy_callback(BspI2c_TransactionId transID);
int pwr_chg_stop_CI(int Tbat, int Vbat);
int pwr_chg_stop_CV(int Tbat, int Vbat,int ichg);
void Bci_ThermisterEnable(void);
void Set_START_ADC_period (UBYTE tx_flag, UBYTE traffic_period, UBYTE idle_period);
void pwm_charging_house_keeping(void);
uint8 pwr_check_AC_charger(void);
uint8 pwr_check_USB_charger(void);
uint8 pwr_precharge_start_check(void);

#if (TEST_PWR_MMI_INTERFACE == 1)
    #include "lcc/pwr_mmi_api.c"
#endif

extern T_PWR_CTRL_BLOCK *pwr_ctrl;
extern T_PWR_CFG_BLOCK  *pwr_cfg;
ChargingPathStatus lcc_ChgPathSts = NO_CHARGING_PATHS_ENABLED;
uint8 thermisterEnabled = 0;
uint8 ChargerVoltageAvailable = 1;
uint8 USB_VBUSAvailable = 1;
uint8 StartAdcFreqReduced = 1;
uint8 StartAdcFreqChanged = 0;
uint8 flag_CI_CV = 0;						//CI_CV mode enabled(1) or disabled(0)
uint8 flag_PWM = 0;						//PWM mode enabled(1) or disabled(0)
BspTwl3029_MadcChannelId ChannelsToRead = 0xFF; 
	Bsp_Twl3029_I2cTransReqArray transArray1;
	Bsp_Twl3029_I2cTransReqArray transArray2;
	//Bsp_Twl3029_I2cTransReqArray transArray3;
#if (ANLG_FAM==11)
	uint16 lcc_charger_status;
	uint16 lcc_USB_status;
	uint8 count_no_charge;
#endif
/******************************************************************************
 * Functions 
 ******************************************************************************/

void build_name(const char *ch_pre, char *cfg_id, UINT8 index, const char * ch_post,  char * name) 
{
    char tmp[2];

    strcpy(name, ch_pre);
    tmp[0] = *cfg_id;
    tmp[1] = 0; // null-termination of strings!
    strcpy(&(name[index]), tmp);
    strcat(name, ch_post);
    ttw(ttr(TTrInit, "build_name: '%s'" NL, name));
}

	int pwr_check_topping()
	{

	// Topping charge will only disallowed starting to charge when 
	//   1) topping charge can be applied (prereq an earlier successful charging of battery)
	//   2) charger is still plugged after successful charging
	//   3) battery capacity has dropped below the topping charge threshold (chg_again_thr)
	if (pwr_ctrl->flag_topping_chg == 1) 
	{
	    if (pwr_ctrl->capacity < pwr_cfg->bat.chg_again_thr) 
	        // Earlier successful charging of battery. We will allow the charging to start
	        return TRUE;
	    else
	        // Earlier successful charging of battery. We will NOT allow the charging to start (yet)
	        return FALSE;
	} else {
	    // No earlier successful charging of battery
	    return TRUE;    
	}
	}

	/***************************************************
	*	function: pwr_task_init()
	*	description:-
	*	Perform battery initialization of the pwr task by
	*	reading battery id and FFS configuration files
	****************************************************/
T_RVM_RETURN pwr_task_init()
{

    T_FFS_SIZE       error;

    ttw(ttr(TTrInit, "pwr_task_init(%d)" NL, 0));

	#if 0	
    // Check configuration and calibration files
    if ((error = pwr_check_files()) < 0)
        return RVM_INTERNAL_ERR;
	#endif 

    // Read configuration files
    if ((error = pwr_read_files()) < 0)
        return RVM_INTERNAL_ERR;

    ttw(ttr(TTrInit, "pwr_task_init(%d)" NL, 0xFF));
    return RVM_OK;

}

	/*****************************************************
	* function : pwr_chg_start()
	*description:-
	*    Check start conditions for charging - it is assumed that a charger is plugged
	*    This check is used in state SUP
	*    Charging start will _not_ be performed if ...
	*    1) Battery temperature is outside limits (measurement)
	*    2) Charger identity is unknown
	******************************************************/
int pwr_chg_start(int Tbat) {

	   int capacity=0;

    ttw(ttr(TTrCharge, "pwr_chg_start(%d)" NL, 0));
    // 1)
    // Battery temperature is outside limits (measurement)
    // FIXME: Check also low temperatures
    //    if ((Tbat < pwr_cfg->temp.tbat_min) || (Tbat > pwr_cfg->temp.tbat_max)) {
    if ((Tbat > pwr_cfg->temp.tbat_max)) {
        ttw(ttr(TTrCharge, "pwr_chg_start(%d) Tbat=(%d)" NL, 0xFF, Tbat));
        return FALSE;
    }

    // 2) 
    // Charger identity unknown
    if ((pwr_ctrl->flag_chg_unknown == 1) && (pwr_ctrl->flag_mmi_registered == 1)) {
        ttw(ttr(TTrCharge, "pwr_chg_start(%d) Chg unknown=(%d)" NL, 0xFF, Tbat));
        pwr_ctrl->mmi_ptr->header.msg_id = MMI_CHG_UNKNOWN_IND;
        mmi_send_msg(pwr_ctrl->mmi_ptr);
        // Send only once
        pwr_ctrl->flag_bat_unknown = 0;
        return FALSE;
    }

    ttw(ttr(TTrCharge, "pwr_chg_start(%d)" NL, 0xFF));
    return TRUE;
}
	/****************************************************
	*function : pwr_hg_stop_CI
	*desscription:
	*   Check stop conditions for charging in Constant current mode
	*   I.e.
	*   - temperature (RF+Battery) thresholds
	*   - battery voltage thresholds
	*  This check is used regularly in state CCI.
	*  Charging will be stopped if ...
	*  1) Battery temperature is outside limits
	*  2) Battery voltage is equal to - or above - charging stop battery voltage
	*****************************************************/
int pwr_chg_stop_CI(int Tbat, int Vbat) 
{

	   ttw(ttr(TTrCharge, "pwr_chg_stop(%d)" NL, 0));

	   // 1)
	   // Battery temperature is outside limits
	   // FIXME: Check also low temperatures
	   if ((Tbat < pwr_cfg->temp.tbat_min) || (Tbat > pwr_cfg->temp.tbat_max)) 
	   {
	       ttw(ttr(TTrCharge, "pwr_chg_stop(%d) Tbat=(%d)" NL, 0xFF, Tbat));

		 if (pwr_ctrl->flag_mmi_registered == 1) 
		 {
	           pwr_ctrl->mmi_ptr->header.msg_id = MMI_CHG_FAILED_IND;
	           if (Tbat > pwr_cfg->temp.tbat_max)
                	pwr_ctrl->mmi_ptr->cause         = BATTERY_TEMPERATURE_HIGH;
	    	    else
                	pwr_ctrl->mmi_ptr->cause         = BATTERY_TEMPERATURE_LOW;
				
	           mmi_send_msg(pwr_ctrl->mmi_ptr);
	       }
		 ttr(TTrAll,"CHARGE STOP: Temperature=%d" NL, Tbat);
        	ttr(TTrAll,"CHARGING TIME [ms]: t=%d" NL, pwr_ctrl->time_elapsed_T1);
		 return TRUE;
	   }

	   // 2a)
	   // Battery voltage is equal to - or above - charging stop battery voltage
	   if ((Vbat > OVER_VOLTAGE_MV)) 
	   {
	       ttw(ttr(TTrCharge, "pwr_chg_stop(%d) Vbat=(%d)" NL, 0xFF, Vbat));
	       ttw(ttr(TTrCharge, "pwr_cfg->bat.chg_stop_thr=%d" NL, 0xFF, pwr_cfg->bat.chg_stop_thr));
		ttr(TTrAll,"CHARGE STOP: Vbat=%d" NL, Vbat);
        	ttr(TTrAll,"CHARGING TIME [ms]: t=%d" NL, pwr_ctrl->time_elapsed_T1);  
		pwr_ctrl->flag_topping_chg = 1; // Apply topping charge frow now on and until charger is unplugged	
	       return TRUE;
	   }
	   // 2b)
	    // Charging voltage is equal to - or above - CRITICAL charging voltage
	    if ((pwr_cfg->data.Vchg >= VCHG_REJECT_CV_MODE )) 
	    {
	        // Critical charging voltage - inform the MMI
	        ttr(TTrAll,"Critical Vchg=%d" NL, pwr_cfg->data.Vchg);
	        if (pwr_ctrl->flag_mmi_registered == 1) 
		{
	            pwr_ctrl->mmi_ptr->header.msg_id = MMI_CHG_FAILED_IND;
	            pwr_ctrl->mmi_ptr->cause         = CHARGER_VOLTAGE_HIGH;
	            mmi_send_msg(pwr_ctrl->mmi_ptr);
	        }
	        ttr(TTrAll,"CHARGE STOP: Vchg=%d" NL, pwr_cfg->data.Vchg);
	        ttr(TTrAll,"CHARGING TIME [ms]: t=%d" NL, pwr_ctrl->time_elapsed_T1);
	        return TRUE;
	    }

	    // 2c)
	    // Charging current is equal to - or above - CRITICAL charging current
		
	    if ((pwr_cfg->data.Ichg >= ICHG_REJECT_CV_MODE )) 
	    { 
	        // Critical charging current - inform the MMI
	        ttr(TTrAll,"Critical Ichg=%d" NL, pwr_cfg->data.Ichg);
	        if (pwr_ctrl->flag_mmi_registered == 1) 
		{
	            pwr_ctrl->mmi_ptr->header.msg_id = MMI_CHG_FAILED_IND;
	            pwr_ctrl->mmi_ptr->cause         = CHARGER_CURRENT_HIGH;
	            mmi_send_msg(pwr_ctrl->mmi_ptr);
	        }
	        ttr(TTrAll,"CHARGE STOP: Ichg=%d" NL, pwr_cfg->data.Ichg);
	        ttr(TTrAll,"CHARGING TIME [ms]: t=%d" NL, pwr_ctrl->time_elapsed_T1);
	        return TRUE;
	    }

	   ttw(ttr(TTrCharge, "pwr_chg_stop(%d)" NL, 0xFF));
	   return FALSE;
	}
	/****************************************************
	* Check stop conditions for charging in Constant Voltage mode 
	*   I.e.
	*   - temperature (RF+Battery) thresholds
	*   - battery voltage thresholds
	*   - battery charging current thresholds		
	*  This check is used regularly in state CCI.
	*  Charging will be stopped if ...
	*  1) Battery temperature is outside limits
	*  2) Battery voltage is equal to - or above - charging stop battery voltage
	*  3) BAttery charging current is less than minimum current threshold 
	*****************************************************/
int pwr_chg_stop_CV(int Tbat, int Vbat,int ichg) 
{
    ttw(ttr(TTrCharge, "pwr_chg_stop(%d)" NL, 0));

    // 1)
    // Battery temperature is outside limits
    // FIXME: Check also low temperatures
    if (Tbat > pwr_cfg->temp.tbat_max) 
    {
        ttw(ttr(TTrCharge, "pwr_chg_stop(%d) Tbat=(%d)" NL, 0xFF, Tbat));
        if (pwr_ctrl->flag_mmi_registered == 1)
	 {
            pwr_ctrl->mmi_ptr->header.msg_id = MMI_CHG_FAILED_IND;
            pwr_ctrl->mmi_ptr->cause         = BATTERY_TEMPERATURE_HIGH;
            mmi_send_msg(pwr_ctrl->mmi_ptr);
        }
        return TRUE;
    }

	    // 2a)
	    // Charging voltage is equal to - or above - CRITICAL charging voltage
	    if ((pwr_cfg->data.Vchg >= VCHG_REJECT_CV_MODE )) 
	    {
	        // Critical charging voltage - inform the MMI
	        ttr(TTrAll,"Critical Vchg=%d" NL, pwr_cfg->data.Vchg);
	        if (pwr_ctrl->flag_mmi_registered == 1) 
		{
	            pwr_ctrl->mmi_ptr->header.msg_id = MMI_CHG_FAILED_IND;
	            pwr_ctrl->mmi_ptr->cause         = CHARGER_VOLTAGE_HIGH;
	            mmi_send_msg(pwr_ctrl->mmi_ptr);
	        }
	        ttr(TTrAll,"CHARGE STOP: Vchg=%d" NL, pwr_cfg->data.Vchg);
	        ttr(TTrAll,"CHARGING TIME [ms]: t=%d" NL, pwr_ctrl->time_elapsed_T1);
	        return TRUE;
	    }

	    // 2b)
	    // Charging current is equal to - or above - CRITICAL charging current
		
	    if ((pwr_cfg->data.Ichg >= ICHG_REJECT_CV_MODE )) 
	    { 
	        // Critical charging current - inform the MMI
	        ttr(TTrAll,"Critical Ichg=%d" NL, pwr_cfg->data.Ichg);
	        if (pwr_ctrl->flag_mmi_registered == 1) 
		{
	            pwr_ctrl->mmi_ptr->header.msg_id = MMI_CHG_FAILED_IND;
	            pwr_ctrl->mmi_ptr->cause         = CHARGER_CURRENT_HIGH;
	            mmi_send_msg(pwr_ctrl->mmi_ptr);
	        }
	        ttr(TTrAll,"CHARGE STOP: Ichg=%d" NL, pwr_cfg->data.Ichg);
	        ttr(TTrAll,"CHARGING TIME [ms]: t=%d" NL, pwr_ctrl->time_elapsed_T1);
	        return TRUE;
	    }	
		
	   // 2c)
    // Battery voltage is equal to - or above - charging stop battery voltage
    
    if (flag_PWM == 1) {
       if(lcc_ChgPathSts == AC_CHARGING_PATH_ENABLED) {
     	   if (Vbat > PWM_CHARGE_STOP) {
             pwr_ctrl->flag_topping_chg = 1; // Apply topping charge frow now on and until charger is unplugged
	      return TRUE;
    	   }
	}
    }
	
    if ((flag_CI_CV == 1) ||(ENABLE_USB_CHARGING == 1))	{
       if ((Vbat > OVER_VOLTAGE_MV)) {
          ttw(ttr(TTrCharge, "pwr_chg_stop(%d) Vbat=(%d)" NL, 0xFF, Vbat));
          ttw(ttr(TTrCharge, "pwr_cfg->bat.chg_stop_thr=%d" NL, 0xFF, pwr_cfg->bat.chg_stop_thr));
	   pwr_ctrl->flag_topping_chg = 1; // Apply topping charge frow now on and until charger is unplugged
	   return TRUE;
    	}
	   
       // 4)
    	//The charging current is less than the configured limit
	   
       if (ichg < ICHG_BCI_THR) {
          ttw(ttr(TTrCharge, "ichg < ICHG_BCI_THR =%d < %d" NL, ichg,ICHG_BCI_THR));
	   pwr_ctrl->flag_topping_chg = 1; // Apply topping charge frow now on and until charger is unplugged
          return TRUE;
    	}
    }	
    ttw(ttr(TTrCharge, "pwr_chg_stop(%d)" NL, 0xFF));
    return FALSE;
    }
	/**********************************************************
	* Function :pwr_chg_ci_cv_transition
	*Description:
	*      Check CI -> CV condition for charging
	*      Only for Lithium batteries
	*      Returns TRUE if CI-> CV or PWM transition should be made
	*      Returns FALSE if CI-> CV or PWM  transition should NOT be made
	***********************************************************/

int pwr_chg_ci_cv_transition(int Vbat)
{
	   
    ttw(ttr(TTrCharge, "pwr_chg_ci_cv_transition(%d)" NL, 0));

    // 1) Check CV charging start threshold
    
    
	if( lcc_ChgPathSts == AC_CHARGING_PATH_ENABLED) 	{
	   if (flag_PWM == 1) {
		if ((Vbat > PWM_CHARGE_START)) {											//pwr_cfg->bat.chg_start_thr))    //changed !!
       		 ttw(ttr(TTrCharge, "pwr_chg_ci_cv_transition(%d) Vbat=(%d)" NL, 0xFF, Vbat));                      
        		return TRUE;
    		}
	   }
	   else {
		if ((Vbat > CV_CHARGE_START)) {											//pwr_cfg->bat.chg_start_thr))    //changed !!
       	 	ttw(ttr(TTrCharge, "pwr_chg_ci_cv_transition(%d) Vbat=(%d)" NL, 0xFF, Vbat));
        		return TRUE;
    		}
	   } 
	}
   	else {
		if ((Vbat > CV_CHARGE_START)) {					//pwr_cfg->bat.chg_start_thr))    //changed !!
       		ttw(ttr(TTrCharge, "pwr_chg_ci_cv_transition(%d) Vbat=(%d)" NL, 0xFF, Vbat));
        		return TRUE;
    		} 
   	}
	
    ttw(ttr(TTrCharge, "pwr_chg_ci_cv_transition(%d)" NL, 0xFF));
    return FALSE;
}

	/**************************************************
	*function: pwr_capaity
	*Description:
	*    Calculate the capacity, C(T,V), of the battery based on
	*    - Battery Voltage Measurement
	*    Returns a value in the interval 0..100 [%]
	***************************************************/

int pwr_capacity(uint16 Vbat) {
	  
	   uint8 i =0;
	   uint8 cap= 0;
    ttw(ttr(TTrEventLow,"pwr_capacity (Vbat=%d)" NL, Vbat));
    cap = 100; // 100%
    for (i=0;i<=9;i++) 
    {
        if (Vbat < pwr_cfg->temp.cap[i] ) 
	 {
            cap -= 10; // Count down 10% for each index
            ttw(ttr(TTrEventLow,"Trying cap=%d" NL, cap));
        } 
	 else 
	 {
            // FIXME: Interpolate ... instead of truncate
            ttw(ttr(TTrEventLow,"capacity=%d" NL, cap));
            return cap;
        }
    }
    ttw(ttr(TTrEventLow,"pwr_capacity (%d)" NL, 0xFF));
    return 0; // Couldn't lookup capacity
}

	/**************************************************************************
	* Function : pwr_temp_lookup
	* Desription:
	*	returns the temperature (in degreecelcius) from the MADC BTEMP value
	*	and the current which is configured to flow through the battery thermister
	*	This function looks for a mapping of the ADC value in the lookuptable read from FFS
	**************************************************************************/
int8 pwr_temp_lookup(uint16 ADC, uint8 i_meas) {
// temperature = f(ADC, I_meas)
// f(x) is table-lized using ETM command 'pww 4 ...'

	   int i =0;
	   int8 temp =0;

#define MAX_TEMP_SIZE 9

    i = 0;
    temp = -20;
    // FIXME: Always using the second current - should also use the first...
    while (i < MAX_TEMP_SIZE && ADC < pwr_cfg->temp.v2t_2[i]) {
        ttw(ttr(TTrEventLow,"ADC=%d < pwr_cfg->temp.v2t_2[%d]=0x%x" NL, ADC,i, pwr_cfg->temp.v2t_2[i]));
        ttw(ttr(TTrEventLow,"temp=%d" NL, temp));
        i++;
        temp += 10;
    }
    if (i == MAX_TEMP_SIZE) {
        // No temperature found - return NULL value (0xFF)
        ttr(TTrEventLow, "Temperature lookup failed %d" NL, ADC);
        return (int8)0xFF;
    }
    if (i != 0) {
        // Interpolate
        if (pwr_cfg->temp.v2t_2[i-1] == 0xFFFF)
            temp += 10*(ADC - pwr_cfg->temp.v2t_2[i])/(pwr_cfg->temp.v2t_2[i] - 1024);
        else
            temp += 10*(ADC - pwr_cfg->temp.v2t_2[i])/(pwr_cfg->temp.v2t_2[i] - pwr_cfg->temp.v2t_2[i-1]);
        ttw(ttr(TTrEventLow,"Interpolated temp=%d" NL, temp));
    }
    return (temp);
}

	/**********************************************************************************
	* function:  update_duty_cyle
	* Description :
	*	-Updates the dutycycle of the PWM wave. and starts the mode_cycle_timer.
	*	-This function is called on each mode_cycle_timer expiry. 
	*	-The new duty_cycle is calculated and writes to the TRITON BCI register  
	***********************************************************************************/ 

void update_duty_cycle(void){

	uint16 duty_cycle;
       BspTwl3029_Bci_dacVal valArray[2];
	   
    	ttw(ttr(TTrEventLow,"Vbat_avg, chg_start_thr, chg_stop_thr (%d), (%d), (%d)" NL, pwr_cfg->data.Vbat_avg_mV, pwr_cfg->bat.chg_start_thr, pwr_cfg->bat.chg_stop_thr));
    	if ((pwr_cfg->data.Vbat_avg_mV - pwr_cfg->bat.chg_start_thr) <= 0) 
	{
        	// Assign k to max value if Vbat_avg value is below start value
        	pwr_cfg->data.k = PWR_MAX_K;
    	} 
	else 
	{
        	pwr_cfg->data.k = (255 * (pwr_cfg->bat.chg_stop_thr - pwr_cfg->data.Vbat_avg_mV) / (pwr_cfg->bat.chg_stop_thr - pwr_cfg->bat.chg_start_thr));
        	if (pwr_cfg->data.k <= PWR_MIN_K)
           	pwr_cfg->data.k = PWR_MIN_K;
    	}

   	duty_cycle = (1023 *pwr_cfg->data.k)/255;
   	bspTwl3029_Bci_ChangeDutyCycle(NULL , duty_cycle);  /*update the dutycycle*/

    	valArray[0] = OVER_VOLTAGE;                         
    	valArray[1] = (pwr_cfg->data.k * pwr_cfg->chg.ichg_max)/255; //modulating the reference current with respect to the 'k' value.

	bspTwl3029_Bci_dacConfig(NULL, BSP_TWL3029_BCI_DACTYPE_CURRENT, valArray);

     	ttw(ttr(TTrEventLow,"pwr_cfg->data.k, duty_cycle, CHGIREG (%d), (%d), (%d)" NL, pwr_cfg->data.k, duty_cycle, valArray[1]));
}


	/***************************************************************************
	*  Function :  process_adc_indication
	*  Desription: 
	*    This is the driving element of the LCC task  
	*    this function processes the the ADC measurements
	*    and Updates the charging FSM.
	*    Indications about various events (charger plug /unplug)  to the MMI also done here. 
	*    
	*  In CALibration state (initial state), 
	*		Battery voltage calibration files are read and
	*		take a decision on whether to go to PREcharge sate or INItialisation state.
	*  In PREcharge state, 
	*  		The battery charging is started in CI mode with a default vaue of current.
	*		Swiches to INI satate once sufficient VBAT level is reached 		
	*  In INItialisation state, 
	*		The battery configuration files ar read from FFS and switches to SUP state
	*  In SUPervision state,
	*		Checks for the charge start conditions(charger plug, battery voltage threshold,..etc)
	*		If all conditions are met , starts the charging in CI mode and switches to CCI state
	*  In CCI state,
	*		Checks for charge stop conditions and CI to CV or CI to PWM  transition conditions
	*  In CCv satate,
	*		Checks for the charge stop conditions.
	****************************************************************************/

static int boot_time = 0;

	T_RV_RET process_adc_indication (T_PWR_REQ *request)
{
    struct pwr_adc_ind_s *ind = 0;
	   int error =0, i=0, index = 0;
    int8 temp = 0;
    UINT32 timer = 0;
    #if ENABLE_CI_CV_OR_PWM
    UINT16 status = 0; // Result of charger plug - unplug - polled
    #endif  //end of ENABLE_CI_CV_OR_PWM
    #if ENABLE_USB_CHARGING
    UINT16 USB_status = 0;
    #endif 	//end of ENABLE_USB_CHARGING
    BspTwl3029_I2C_Callback  callback;
    //uint8 dummy;
    callback.callbackFunc = dummy_callback;
    ind = (struct pwr_adc_ind_s*)request;
    ttw(ttr(TTrEventLow,"State=%d" NL, ind->data[State]));

	// This switch is ment for some basic debug traces. 
	// not for the calibration state.
    switch (pwr_ctrl->state) 
    {
    	case PRE :
       	ttw(ttr(TTrAll,"Vbat=%d" NL,4*ADC_to_mV(ind->data[Vbat])));
        	ttw(ttr(TTrEventLow,"Vchg=%d" NL, ADC_to_mV(ind->data[Vchg])));
        	ttw(ttr(TTrAll,"Ichg=%d" NL, ind->data[Ichg]));
        	break;
    case INI :
    case SUP :
    case CCI :
    case LCI :
    case CCV :
    case LCV :
		#if ENABLE_USB_CHARGING
		ttw(ttr(TTrEventLow,"USB_VBUS=%d" NL, ind->data[USB_VBUS]));
		ttw(ttr(TTrEventLow,"USB_STATUS=%d" NL, ind->data[Usb_state]));
		#endif //end of ENABLE_USB_CHARGING
        	ttw(ttr(TTrEventLow,"Vbat=%d" NL, ADC_to_mV(ind->data[Vbat])));
        	ttw(ttr(TTrEventLow,"Vbat_adc=%d" NL, ind->data[Vbat]));
		#if ENABLE_CI_CV_OR_PWM
		if(ChargerVoltageAvailable == 1)
		{
			pwr_cfg->data.Vchg = 5*ADC_to_mV(ind->data[Vchg]);       /*compensating the voltage scaling in vchg line*/
			ttw(ttr(TTrAll,"Vchg=%d" NL, pwr_cfg->data.Vchg));
		}
		#endif	//end of ENABLE_CI_CV_OR_PWM
	 	ttw(ttr(TTrEventLow,"Ichg=%d" NL, ind->data[Ichg]));
        	ttw(ttr(TTrEventLow,"Type=%d" NL, ind->data[Type]));
        	ttw(ttr(TTrEventLow,"Tbat=%d" NL, ind->data[Tbat]));
        	ttw(ttr(TTrEventLow,"T_rf=%d" NL, ind->data[T_rf]));
        	ttw(ttr(TTrEventLow,"Vbt2=%d" NL, ADC_to_mV(ind->data[Vbt2])));
        break;
    }

    // Updates variables in 'service' mode - not for CAL, PRE & INI
	   // this switch is ment for updating some variables based on the MADC values and the calibration values.
	   // This not required for calibration, precharge and initialisation states.
    switch (pwr_ctrl->state) 
    {
    case SUP :
    case CCI :
    case LCI :
    case CCV :
    case LCV :
        index = pwr_ctrl->index % CONSECUTIVE_CHG_UNPLUGS;
        ttw(ttr(TTrEventLow,"Using index: (%d)" NL, index));
        pwr_ctrl->index++;
        // Get current nucleus time
        timer  = rvf_get_tick_count();
        // T0 timer expired?
        pwr_ctrl->time_elapsed_T0  = pwr_timer_elapsed(pwr_ctrl->time_begin_T0, timer);
        ttw(ttr(TTrTimerLow,"T0: %d ms elapsed " NL, pwr_ctrl->time_elapsed_T0));
        // Compare T0 with configured sampling rate
        if (pwr_ctrl->time_elapsed_T0 > pwr_cfg->common.sampling) 
	{
            ttw(ttr(TTrTimer, "T0 Reset: elapsed (%d)" NL, pwr_ctrl->time_elapsed_T0));
            pwr_ctrl->time_begin_T0 = timer;
            ttr(TTrTimer,"Vbat_avg=%d" NL, ADC_to_mV(pwr_cfg->data.Vbat_avg));
        }
        pwr_cfg->data.Vbat = ind->data[Vbat]; 
        pwr_cfg->data.Vbat_avg = (ind->data[Vbat] + pwr_cfg->common.alfa1 * pwr_cfg->data.Vbat_avg)/(pwr_cfg->common.alfa1 + 1);
	pwr_cfg->data.Vbat_avg_mV = 4*ADC_to_mV(pwr_cfg->data.Vbat_avg); //compensating the voltage scaling in Vbat line
       
	ttw(ttr(TTrAll,"Vbat_avg=%d" NL, pwr_cfg->data.Vbat_avg_mV));
        pwr_cfg->data.Tbat = ind->data[Tbat];
        pwr_cfg->data.Tbat_avg = (ind->data[Tbat] + pwr_cfg->common.alfa2 * pwr_cfg->data.Tbat_avg)/(pwr_cfg->common.alfa2 + 1);
        ttw(ttr(TTrEventLow,"Tbat_avg=%d" NL, pwr_cfg->data.Tbat_avg));
        temp = pwr_temp_lookup(pwr_cfg->data.Tbat_avg, 0x01);
        ttw(ttr(TTrAll,"temp=%d" NL, temp));

        if (pwr_ctrl->flag_mmi_registered == 1 && (!boot_time) && (pwr_cfg->data.Vbat_avg_mV > 0))
        {
		boot_time = 1;
        	pwr_ctrl->mmi_ptr->header.msg_id = MMI_BAT_SUPERVISION_INFO_IND;
		pwr_ctrl->mmi_ptr->Vbat = pwr_cfg->data.Vbat_avg_mV;
                mmi_send_msg(pwr_ctrl->mmi_ptr);
	}
	if(pwr_ctrl->state > SUP)
	{
		pwr_cfg->data.Ichg = ind->data[Ichg];
		ttw(ttr(TTrAll,"Ichg=%d" NL, ind->data[Ichg]));
	}

	#if ENABLE_USB_CHARGING
		if(USB_VBUSAvailable == 1)
		{
			pwr_cfg->data.USB_VBUS = ind->data[USB_VBUS];
			ttw(ttr(TTrEventLow,"USB_VBUS=%d" NL, ind->data[USB_VBUS]));
		}
		pwr_cfg->data.USB_VBUS = ind->data[USB_VBUS];
		USB_status = ind->data[Usb_state]; 
		
		if(USB_status & CHGPRES)
		{
			ttw(ttr(TTrEventLow,"Polled - USB chg plugged (%d)" NL, USB_status));
			pwr_ctrl->flag_USB_plugged = 1;
			pwr_ctrl->capacity = pwr_capacity(pwr_cfg->data.Vbat_avg_mV);
		}
		else
		{
			pwr_ctrl->flag_USB_plugged = 0;
			ttw(ttr(TTrEventLow,"Polled - USB not yet plugged " NL, USB_status));
		}
		
		// USB Charger plugged
            	if ((pwr_ctrl->flag_USB_plugged == 1) &&  (pwr_ctrl->flag_USB_prev_plugged == 0)) 
		{
				ttw(ttr(TTrAll,"USB chg plugged (%d)" NL, USB_status));
				pwr_ctrl->flag_USB_prev_plugged = 1;
				 
				if (pwr_ctrl->flag_mmi_registered == 1) 
    				{		
                			pwr_ctrl->mmi_ptr->header.msg_id = MMI_USB_PLUG_IND;
                			mmi_send_msg(pwr_ctrl->mmi_ptr);
				}

				if((pwr_ctrl->state == SUP) && (StartAdcFreqReduced == 1))
				{
					Set_START_ADC_period(0, 10, 1);  //increase START_ADC periodicity on USB plug
					StartAdcFreqReduced = 0;
					ttw(ttr(TTrEventLow,"START_ADC periodicity increased :%d " NL, 1));
				}

				if(thermisterEnabled == 0)  //Enable the current source for battery temperature measurement
				{
					Bci_ThermisterEnable();
					thermisterEnabled = 1;
					ttw(ttr(TTrAll,"THEN enabled  (%d)" NL, 0x00));
				}
            	}
				
            	// USB Charger unplugged
            	if ((pwr_ctrl->flag_USB_plugged == 0) && (pwr_ctrl->flag_USB_prev_plugged == 1))
		{
			ttw(ttr(TTrAll,"USB chg unplugged (%d)" NL, USB_status));
			pwr_ctrl->flag_USB_prev_plugged = 0;

			if (pwr_ctrl->flag_mmi_registered == 1) 
    			{		
                		pwr_ctrl->mmi_ptr->header.msg_id = MMI_USB_UNPLUG_IND;
                		mmi_send_msg(pwr_ctrl->mmi_ptr);
				pwr_start_timer(&pwr_ctrl->time_begin_mmi_rep);
			}
			if((thermisterEnabled == 1) && (lcc_charger_status == 0))
			{
				bspTwl3029_Bci_sensorsDisable(NULL,  BSP_TWL3029_BCI_SENS_TEMP);
				thermisterEnabled=0;
				ttw(ttr(TTrAll,"THEN disabled  (%d)" NL, 0x00));
			}
			USB_unplug_house_keeping();
			pwr_free(request);
    			return RV_OK;		
		}
	#endif 	//end of ENABLE_USB_CHARGING		
        break;
    }

	   // The 'Main' switch : This inclueds all the FSM updates of LCC.
	   // This switch implements the operations that need to be performed on each LCC FSM sate, including 
	   // checking for start of charge conditions, Enabling a particular charge scheme, checking for different timer 
	   // expiries,checking for end of charge conditions, Disabling a particular charge scheme..etc 
    ttw(ttr(TTrEventLow,"LCC FSM state=%d" NL, pwr_ctrl->state));
	
    switch (pwr_ctrl->state) 
    {
    case CAL :
        // Read calibration files
        // Decide if SW precharge should be started (state PRE) or if a normal configuration scheme should be inititated (state INI)
        error = pwr_read_cal_files();
        	if (error >= 0)
		{
            		// Save measurements - also initial values for averaging
            		pwr_cfg->data.Vbat_avg = pwr_cfg->data.Vbat = ind->data[Vbat];
            		pwr_cfg->data.Vbat_avg_mV = ADC_to_mV(pwr_cfg->data.Vbat_avg);
					
            		ttw(ttr(TTrEventLow,"Vbat=%d" NL, ADC_to_mV(ind->data[Vbat])));
			
	     		#if ENABLE_CI_CV_OR_PWM
            		ttw(ttr(TTrEventLow,"Vchg=%d" NL, ADC_to_mV(ind->data[Vchg])));
	     		// Charger was already inserted?
            		status = ind->data[State];
	     		if (status & CHGPRES) 
	     		{
             			// Charger plugged caused a wakeup
             			pwr_ctrl->flag_chg_plugged = 1;
             			pwr_ctrl->flag_chg_prev_plugged = 1;
             			ttw(ttr(TTrInitLow,"Polled - charger plugged  (%d)" NL, status));
				/*If charger is plugged while boot up, increase the START_ADC frequency to initialise the LCC quickly*/
				if(StartAdcFreqReduced == 1)
				{
					Set_START_ADC_period(0, 10, 1);
					StartAdcFreqReduced = 0;
					ttw(ttr(TTrEventLow,"START_ADC periodicity incresed :%d " NL, 10));
				}	
            		}
	     		#endif //end of ENABLE_CI_CV_OR_PWM
		 
	     		#if ENABLE_USB_CHARGING
	     		USB_status = ind->data[Usb_state]; 		
	     		if (USB_status & CHGPRES) 
	     		{
             			// Charger plugged caused a wakeup
             			pwr_ctrl->flag_USB_plugged = 1;
             			pwr_ctrl->flag_USB_prev_plugged = 1;
             			ttw(ttr(TTrInitLow,"Polled - USB charger plugged  (%d)" NL, USB_status));
				/*If charger is plugged while boot up, increase the START_ADC frequency to initialise the LCC quickly*/
				if(StartAdcFreqReduced == 1)
				{
					Set_START_ADC_period(0, 10, 1);
					StartAdcFreqReduced = 0;
				}			
            		}
	     		#endif //end of ENABLE_USB_CHARGING
		 
            		// Check Pre-charge - immediately change state to PRE
            		if (pwr_precharge_start_check() == 1)//(pwr_cfg->data.Vbat_avg_mV < VBAT_PRECHG_START) && (pwr_ctrl->flag_chg_plugged == 1)) 
	     		{
                		ttw(ttr(TTrInitLow,"precharge (%d)" NL, ADC_to_mV(ind->data[Vbat])));
                		// Charger interrupts (plug/unplug) are already masked off - in case it was a linear charger
                		pwr_ctrl->state = PRE;
            		} 
	     		else 
	     		{
                		pwr_ctrl->state = INI;
            		}
	     	}
        	break;
			
    case PRE :
        // Start fast charge immediately after 3.2V boot
        // Enter INI state - in order to read FFS configuration files - when Vbat reaches 3.6V
        // Hardcoding moving average to PRECHG_AVG_WINDOW_SIZE since we haven't read any configuration
        pwr_cfg->data.Vbat = ind->data[Vbat];
        pwr_cfg->data.Vbat_avg = (ind->data[Vbat] + PRECHG_AVG_WINDOW_SIZE  * pwr_cfg->data.Vbat_avg)/(PRECHG_AVG_WINDOW_SIZE + 1); 
        pwr_cfg->data.Vbat_avg_mV = 4*ADC_to_mV(pwr_cfg->data.Vbat_avg);
        if (pwr_cfg->data.Vbat_avg_mV > VBAT_PRECHG_STOP)
	 {
            ttw(ttr(TTrInitLow,"state PRE (%d > VBAT_PRECHG_STOP)" NL, pwr_cfg->data.Vbat_avg_mV));
            pwr_ctrl->state = INI;
            pwr_ctrl->flag_prechg_started = 0;
            stop_pre_charge();
	     lcc_ChgPathSts = NO_CHARGING_PATHS_ENABLED;		
        }
	 else
	 {
         	ttw(ttr(TTrInitLow,"state PRE (%d < VBAT_PRECHG_STOP)" NL, pwr_cfg->data.Vbat_avg_mV));
            	// Start fast charging NOW
            	#if ENABLE_CI_CV_OR_PWM
		if((pwr_ctrl->flag_chg_plugged == 1) && (pwr_ctrl->flag_prechg_started == 0))
		{
			bspTwl3029_Bci_prechargeDisable(NULL); //disable HW precharge
			start_pre_charge(BSP_TWL3029_BCI_CHARGEPATH_AC);
			lcc_ChgPathSts = AC_CHARGING_PATH_ENABLED;
			pwr_ctrl->flag_prechg_started = 1;
		}
		#endif //end of ENABLE_CI_CV_OR_PWM
		
		#if ENABLE_USB_CHARGING
		if((pwr_ctrl->flag_USB_plugged == 1) && (pwr_ctrl->flag_prechg_started == 0))
		{
			bspTwl3029_Bci_prechargeDisable(NULL); //disable HW precharge	
			start_pre_charge(BSP_TWL3029_BCI_CHARGEPATH_USB);
			lcc_ChgPathSts = USB_CHARGING_PATH_ENABLED;
			pwr_ctrl->flag_prechg_started = 1;
		}
		#endif //end of ENABLE_USB_CHARGING
        }
        break;

	case INI :
		// Charger was already inserted?
		#if ENABLE_CI_CV_OR_PWM
		status = ind->data[State];
		pwr_ctrl->chg_unplug_vec[index] = (status & CHGPRES);
		if (status & CHGPRES) 
		{
            		// Charger plugged caused a wakeup
            		pwr_ctrl->flag_chg_plugged = 1;
            		pwr_ctrl->flag_chg_prev_plugged = 1;
            		ttw(ttr(TTrInitLow,"Polled - charger plugged  (%d)" NL, status));
        	}
		#endif //end of ENABLE_CI_CV_OR_PWM

		#if ENABLE_USB_CHARGING
		USB_status = ind->data[Usb_state];	
		pwr_ctrl->USB_unplug_vec[index] = (USB_status & CHGPRES);	
		if(USB_status & CHGPRES)
		{
			pwr_ctrl->flag_USB_plugged = 1;
			pwr_ctrl->flag_USB_prev_plugged = 1;
		}
		#endif //end of ENABLE_USB_CHARGING
		
        	if (pwr_ctrl->flag_ini_virgo == 0)
		{
            		// Perform some very basic initialization

            		/* Precharge (C/20) is switched OFF since it was finished in state PRE */
			callback.callbackVal = 0x01;
   			callback.i2cTransArrayPtr = &transArray1;
			error = bspTwl3029_Bci_prechargeDisable(&callback);

			/* Disable charging */
                     error = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_PAGE0, 
	                                                                          BSP_TWL3029_MAP_BCI_BCICTL1_OFFSET,
                                                                                 0x00,
					                                              NULL);            		

		       /* Watchdog acknowledge */
                     error = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_PAGE0, 
	                                                                          BSP_TWL3029_MAP_BCI_BCIWDKEY_OFFSET,
                                                                                 BSP_TWL3029_BCI_WDKEY_INT_ACK,
					                                              NULL);

			/* Watchdog disable */
                     error = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_PAGE0, 
	                                                                          BSP_TWL3029_MAP_BCI_BCIWDKEY_OFFSET,
                                                                                 BSP_TWL3029_BCI_WDKEY_DISABLE,
					                                              NULL);

            		// Enable battery type identification
            		// Switch on battery type identification (TYPEN)
            		// NOTE: Battery type identification is ALWAYS done with 10uA (TYPEN)
            		ttw(ttr(TTrInitLow,"Enable type ident (%d)" NL, 0x00));
			callback.callbackVal = 0x02;
   			callback.i2cTransArrayPtr = &transArray2;
			bspTwl3029_Bci_sensorsEnable(&callback, BSP_TWL3029_BCI_SENS_BATTYPE, BSP_TWL3029_BCI_THERM_I_30, BSP_TWL3029_BCI_THERMSIGN_NEG);
			pwr_ctrl->flag_ini_virgo = 1;
        	}
        	// Don't read battery files before we know which battery id to use
        	// We will ignore the first BATTERY_TYPE_SLIP measurements of battery id
        	if ((pwr_ctrl->count_bat_type == BATTERY_TYPE_SLIP))
		{
            		error = pwr_task_init();
            		// Change state
            		pwr_ctrl->state = SUP;
			//Calculate the capacity here!..for charging start to speed up!!
			pwr_ctrl->capacity = pwr_capacity(pwr_cfg->data.Vbat_avg_mV);	

			//Need to enable only used channels.
			ChannelsToRead &=  ~0x0088; //disable ICHG and BTYPE
			#if ENABLE_CI_CV_OR_PWM
			
				if(pwr_ctrl->flag_chg_plugged == 0 )
				{
					ChannelsToRead &=  ~0x0100;
					ChargerVoltageAvailable = 0;
					ttw(ttr(TTrEventLow,"Charger voltage unavailable !!: %d" NL, 0));
				}
				if(pwr_ctrl->flag_chg_plugged == 1)
				{
					ChannelsToRead |=  0x0100;
					ChargerVoltageAvailable = 1;
					ttw(ttr(TTrEventLow,"Charger voltage available !!: %d" NL, 0));
				}
			#endif //end of ENABLE_CI_CV_OR_PWM
			#if ENABLE_USB_CHARGING
				if(pwr_ctrl->flag_USB_plugged == 0)
				{
					ChannelsToRead &=  ~0x0020;
					USB_VBUSAvailable = 0;
					ttw(ttr(TTrEventLow,"USBVBUS unavailable !!: %d" NL, 0));
				}
				if(pwr_ctrl->flag_USB_plugged == 1)
				{
					ChannelsToRead |=  0x0020;
					USB_VBUSAvailable = 1;
					ttw(ttr(TTrEventLow,"USBVBUS available !!: %d" NL, 0));
				}
			#endif //end of ENABLE_USB_CHARGING
			bspTwl3029_Madc_setRtChannels(NULL, ChannelsToRead);
					
            		// Readout /pwr/common.cfg
            		ttw(ttr(TTrInitLow,"common.cfg: pins: %d" NL, pwr_cfg->common.pins));
            		ttw(ttr(TTrInitLow,"chg_dedic %d" NL, pwr_cfg->common.chg_dedic));
            		ttw(ttr(TTrInitLow,"sampling %d" NL, pwr_cfg->common.sampling));
            		ttw(ttr(TTrInitLow,"mod_cycle %d" NL, pwr_cfg->common.mod_cycle));
            		ttw(ttr(TTrInitLow,"alfa1 %d" NL, pwr_cfg->common.alfa1));
            		ttw(ttr(TTrInitLow,"alfa2 %d" NL, pwr_cfg->common.alfa2));
            		ttw(ttr(TTrInitLow,"alfa3 %d" NL, pwr_cfg->common.alfa3));
            		ttw(ttr(TTrInitLow,"rise_thr %d" NL, pwr_cfg->common.rise_thr));
            		// Readout /pwr/bat/bat.cfg
            		ttw(ttr(TTrInitLow,"bat.cfg: type: %d" NL, pwr_cfg->bat.type));
            		ttw(ttr(TTrInitLow,"rf_temp: %d" NL, pwr_cfg->bat.rf_temp));
            		ttw(ttr(TTrInitLow,"id_low: %d" NL, pwr_cfg->bat.id_low));
            		ttw(ttr(TTrInitLow,"id_high: %d" NL, pwr_cfg->bat.id_high));
            		ttw(ttr(TTrInitLow,"cbat: %d" NL, pwr_cfg->bat.cbat));
            		ttw(ttr(TTrInitLow,"ratio: %d" NL, pwr_cfg->bat.ratio));
            		ttw(ttr(TTrInitLow,"T1: %d" NL, pwr_cfg->bat.T1));
            		ttw(ttr(TTrInitLow,"T2: %d" NL, pwr_cfg->bat.T2));
            		ttw(ttr(TTrInitLow,"T3: %d" NL, pwr_cfg->bat.T3));
            		ttw(ttr(TTrInitLow,"chg_start_thr: %d" NL, pwr_cfg->bat.chg_start_thr));
            		ttw(ttr(TTrInitLow,"chg_stop_thr: %d" NL, pwr_cfg->bat.chg_stop_thr));
            		ttw(ttr(TTrInitLow,"chg_ctrl_thr: %d" NL, pwr_cfg->bat.chg_ctrl_thr));
            		ttw(ttr(TTrInitLow,"chg_again_thr: %d" NL, pwr_cfg->bat.chg_again_thr));
            		// Readout /pwr/bat/temp<N>.cfg
            		ttw(ttr(TTrInitLow,"sizeof(temp): %d" NL, sizeof(pwr_cfg->temp)));
           		ttw(ttr(TTrInitLow,"sizeof(T_PWR_BAT_TEMP_CFG_BLOCK): %d" NL, sizeof(T_PWR_BAT_TEMP_CFG_BLOCK)));
            		ttw(ttr(TTrInitLow,"temp.cfg: tbat_min: %d" NL, pwr_cfg->temp.tbat_min));
            		ttw(ttr(TTrInitLow,"tbat_max: %d" NL, pwr_cfg->temp.tbat_max));
            		ttw(ttr(TTrInitLow,"i_meas1: %d" NL, pwr_cfg->temp.i_meas1));
            		for (i=0;i<=8;i++) 
			{
                		ttw(ttr(TTrInitLow,"v2t_1[]: %d" NL, pwr_cfg->temp.v2t_1[i]));
            		}
            		ttw(ttr(TTrInitLow,"i_meas2: %d" NL, pwr_cfg->temp.i_meas2));
            		for (i=0;i<=8;i++)
			{
                		ttw(ttr(TTrInitLow,"v2t_2[]: %d" NL, pwr_cfg->temp.v2t_2[i]));
            		}
            		for (i=0;i<=9;i++)
			{
                		ttw(ttr(TTrInitLow,"cap[]: %d" NL, pwr_cfg->temp.cap[i]));
            		}
            		ttw(ttr(TTrInitLow,"a0: %d" NL, pwr_cfg->temp.a0));
            		ttw(ttr(TTrInitLow,"a1: %d" NL, pwr_cfg->temp.a1));
            		ttw(ttr(TTrInitLow,"a2: %d" NL, pwr_cfg->temp.a2));

            		// Readout /mmi/pwr/bsie.cfg
            		ttw(ttr(TTrInitLow,"mmi repetition: %d" NL, pwr_cfg->mmi.repetition));
 
            		// Initialize battery temperature - must not be made in the virgo part since Tbat=0
            		pwr_cfg->data.Tbat_avg = ind->data[Tbat];
			if((lcc_charger_status == 1)||(lcc_USB_status == 1))  
			{
				//enable the current soure for battery temp. measurement if during bootup charger is plugged.
				if(thermisterEnabled == 0)  //Enable the current source for battery temperature measurement
				{
					Bci_ThermisterEnable();
					thermisterEnabled = 1;
				}
			}

			#if 0

            		// Setup thermal sensor - don't re-enable TYPEN
            		callback.callbackVal = 0x03;
   	     		callback.i2cTransArrayPtr = &transArray3;
			if (pwr_cfg->temp.i_meas2 == 50) 
			{
                		ttw(ttr(TTrInitLow,"ABB set i_current: %d" NL, pwr_cfg->temp.i_meas2));
				bspTwl3029_Bci_sensorsEnable(&callback, BSP_TWL3029_BCI_SENS_TEMP, BSP_TWL3029_BCI_THERM_I_50, BSP_TWL3029_BCI_THERMSIGN_NEG);
			} 
			else
			{
				if (pwr_cfg->temp.i_meas2 == 30) 
				{
                			ttw(ttr(TTrInitLow,"ABB set i_current: %d" NL, pwr_cfg->temp.i_meas2));
					bspTwl3029_Bci_sensorsEnable(&callback, BSP_TWL3029_BCI_SENS_TEMP, BSP_TWL3029_BCI_THERM_I_30, BSP_TWL3029_BCI_THERMSIGN_NEG);
				} 
				else 
				{
                			bspTwl3029_Bci_sensorsEnable(&callback, BSP_TWL3029_BCI_SENS_TEMP, BSP_TWL3029_BCI_THERM_I_30, BSP_TWL3029_BCI_THERMSIGN_NEG);
				}
			}	
		#endif
		//Disable TYPEN : the current source for battery type measurement
		bspTwl3029_Bci_sensorsDisable(NULL, BSP_TWL3029_BCI_SENS_BATTYPE);
		
		} 
				
		else 
		{
            		// Compare battery id with values found in FFS
            		// First value(s) are NOT to be trusted
            		pwr_ctrl->count_bat_type++;
            		pwr_cfg->data.bat_id = ind->data[Type];
        	}
        	break;
    case SUP :
        	// Charger inserted?? Poll!!
        	// Reason: 100Hz interrupts will occur if an unregulated charger is inserted & charger interrupts are enabled
        	//         This will cause an immediate crash!
        	//USB_status = ind->data[Usb_state]; 
        	
			// DO NOT Apply topping charge frow now on
			//pwr_ctrl->flag_topping_chg = 0;
	
		#if ENABLE_CI_CV_OR_PWM
		status = ind->data[State];
        	pwr_ctrl->chg_unplug_vec[index] = (status & CHGPRES);
        	if (status & CHGPRES) 
		{
            		// Charger is plugged - continue
            		ttw(ttr(TTrEventLow,"Polled - chg plugged (%d)" NL, status));
			pwr_ctrl->flag_chg_plugged = 1;
		} 
		else 
		{
            		pwr_ctrl->flag_chg_plugged = 0;
		}
		#endif  //end of ENABLE_CI_CV_OR_PWM
		if (pwr_ctrl->flag_mmi_registered == 1)  
		{
			#if ENABLE_CI_CV_OR_PWM
            		// If the charger is plugged and no charging is initiated this message will be repeated until charging has started
            		// Charger plugged
            		if ((pwr_ctrl->flag_chg_plugged == 1) &&  (pwr_ctrl->flag_chg_prev_plugged == 0)) 
			{
                		pwr_ctrl->flag_chg_prev_plugged = 1;
                		pwr_ctrl->mmi_ptr->header.msg_id = MMI_CHG_PLUG_IND;
                		mmi_send_msg(pwr_ctrl->mmi_ptr);
            		}
            		// Charger unplugged
            		if ((pwr_ctrl->flag_chg_plugged == 0) && (pwr_ctrl->flag_chg_prev_plugged == 1))
			{
                		pwr_ctrl->flag_chg_prev_plugged = 0;
                		pwr_ctrl->mmi_ptr->header.msg_id = MMI_CHG_UNPLUG_IND;
                		mmi_send_msg(pwr_ctrl->mmi_ptr);
            		}
			#endif  //end of ENABLE_CI_CV_OR_PWM
			
            		if (pwr_ctrl->flag_bat_unknown == 1) 
			{
                		// Battery unknown - inform the MMI
                		pwr_ctrl->mmi_ptr->header.msg_id = MMI_BAT_UNKNOWN_IND;
                		mmi_send_msg(pwr_ctrl->mmi_ptr);
                		// Send only once
                		pwr_ctrl->flag_bat_unknown = 0;
            		}
        	}
		
        	pwr_check_timers();
		//ttw(ttr(TTrAll,"Capacity (%d)" NL, status));

		#if ENABLE_CI_CV_OR_PWM
			#if ENABLE_USB_CHARGING //both are enabled

			if ((ChargerVoltageAvailable == 0) && ((USB_VBUSAvailable == 0)&&(pwr_ctrl->flag_USB_plugged != 1)))
			{
				ttw(ttr(TTrEventLow,"both charger voltage and USBVBUS unavailable (%d)" NL, 0));
				break; //EXIT the switch if no charger voltage is available. 
			}
			else if((USB_VBUSAvailable == 0)&&(pwr_ctrl->flag_USB_plugged == 1))
			{
				ChannelsToRead |= BSP_TWL3029_ADC_CHANNEL_6_USBVBUS;
				bspTwl3029_Madc_setRtChannels(NULL,ChannelsToRead);
				USB_VBUSAvailable = 1;
				ttw(ttr(TTrEventLow,"USBVBUS available (%d)" NL, 0));
				break;
			}
			#else //only CI_CV is enabled
			if (ChargerVoltageAvailable == 0)
			{
				ttw(ttr(TTrEventLow,"Charger voltage unavailable (%d)" NL, 0));
				break; //EXIT the switch if no charger voltage is available. 
			}
			#endif   //end of ENABLE_USB_CHARGING
		#else
			#if ENABLE_USB_CHARGING //only USB charging enabled
			if((USB_VBUSAvailable == 0)&&(pwr_ctrl->flag_USB_plugged == 1))
			{
				ChannelsToRead |= BSP_TWL3029_ADC_CHANNEL_6_USBVBUS;
				bspTwl3029_Madc_setRtChannels(NULL,ChannelsToRead);
				USB_VBUSAvailable = 1;
				ttw(ttr(TTrEventLow,"USBVBUS available (%d)" NL, 0));
				break;
			}
			#else //nocharging enabled
			break;
			#endif //end of ENABLE_USB_CHARGING
		#endif  //end of ENABLE_CI_CV_OR_PWM
		
		pwr_ctrl->capacity = pwr_capacity(pwr_cfg->data.Vbat_avg_mV);
		ttw(ttr(TTrEventLow,"Capacity: (%d)" NL, pwr_ctrl->capacity));

		
			// If charger is plugged AND charger configuration is not read AND capacity is below configured limit - charger id Vchg(Load=0) must be measured
	       		if (pwr_charge_start_check() == 1) 
			{
				#if ENABLE_CI_CV_OR_PWM
				// Measurement loop - Take the 2nd Vchg(0) measurements
	           			if ((pwr_check_AC_charger() == 1) && (pwr_check_topping() == TRUE)) 
				{
					#if 0
					if ((pwr_ctrl->flag_chg_plugged == 1) && (pwr_cfg->data.Vchg > CHARGER_TYPE_OUT_OF_BOUND_LOW))
                    				pwr_cfg->data.chg_id = pwr_cfg->data.Vchg;
					else
					{
                    				// Out of bound!!
                    				ttr(TTrWarning, "Charger id out of bound! %d" NL, pwr_cfg->data.Vchg);
                    				pwr_cfg->data.chg_id = CHARGER_TYPE_TYPICAL;
                			}
                			ttw(ttr(TTrEventLow,"Using chg id=%d" NL, pwr_cfg->data.chg_id));
					#endif 		
                			error = pwr_read_chg_files();
                			pwr_ctrl->flag_chg_cfg_read = 1;

                                   if(pwr_cfg->chg.type == UNREGULATED) {
                                   	flag_PWM = 1;			// enable pwm mode
                                   	flag_CI_CV = 0;
                                   }
                                   else {
                                   	flag_PWM = 0;			//enable ci_cv mode
                                   	flag_CI_CV = 1;
                                   }
							
                			// Initialize the circular buffer with charger plugs
                			#if 0
                			if(pwr_ctrl->flag_chg_plugged == 1)
                			{
                				for (i=0;i<= CONSECUTIVE_CHG_UNPLUGS-1 ;i++)
                    				pwr_ctrl->chg_unplug_vec[i] = CHGPRES;
                			}
					#endif 
					if (pwr_chg_start(pwr_temp_lookup(pwr_cfg->data.Tbat_avg, 0x01)) == TRUE ) 
					{
						
                        			pwr_ctrl->state = CCI;
						StartAdcFreqChanged	 = 0;		
                        			start_bci_charge(BSP_TWL3029_BCI_CHARGEPATH_AC);
						lcc_ChgPathSts = AC_CHARGING_PATH_ENABLED;
						ttw(ttr(TTrAll,"AC charging started.. (%d)" NL,0));	
						
						// Stop the mmi info repetition timer
                    				pwr_stop_timer(&pwr_ctrl->time_begin_mmi_rep);
                			}
            			}
				else 
				{
					if(pwr_ctrl->flag_chg_plugged == 1)
                			pwr_ctrl->count_chg_type++;
					ttw(ttr(TTrEventLow,"count_chg_type=%d" NL, pwr_ctrl->count_chg_type));
            			}
				#endif 	//end of ENABLE_CI_CV_OR_PWM
		
				#if ENABLE_USB_CHARGING
					if ((pwr_ctrl->state == SUP) && (pwr_check_USB_charger() == 1) && (pwr_ctrl->capacity < pwr_cfg->bat.chg_again_thr))
				{
					#if 0 
					if(pwr_ctrl->flag_USB_plugged == 1) 
						pwr_cfg->data.chg_cfg_id = pwr_cfg->data.USB_VBUS; 
                			else
					{
                    				// Out of bound!!
                    				ttr(TTrWarning, "Charger id out of bound! %d" NL, pwr_cfg->data.USB_VBUS);
                    				pwr_cfg->data.chg_id = CHARGER_TYPE_TYPICAL;
                			}
					ttw(ttr(TTrEventLow,"Using chg id=%d" NL, pwr_cfg->data.chg_id));
					#endif 
					error = pwr_read_chg_files();
                			pwr_ctrl->flag_chg_cfg_read = 1;
                			// Initialize the circular buffer with charger plugs
                			#if 0
                			if(pwr_ctrl->flag_USB_plugged == 1)
					{
						for (i=0;i<= CONSECUTIVE_USB_UNPLUGS-1 ;i++)
       	             			pwr_ctrl->USB_unplug_vec[i] = CHGPRES;
  					}
					#endif 		
					if (pwr_chg_start(pwr_temp_lookup(pwr_cfg->data.Tbat_avg, 0x01)) == TRUE ) 
					{
                        				pwr_ctrl->state = CCI;
							StartAdcFreqChanged	 = 0;			
						start_bci_charge(BSP_TWL3029_BCI_CHARGEPATH_USB);
						lcc_ChgPathSts = USB_CHARGING_PATH_ENABLED;
						ttw(ttr(TTrAll,"USB charging started.. (%d)" NL,0));	
					}
					// Stop the mmi info repetition timer
                    			pwr_stop_timer(&pwr_ctrl->time_begin_mmi_rep);
                		}
            			else 
				{
					if(pwr_ctrl->flag_USB_plugged == 1)
					pwr_ctrl->count_USB_type++;
			       	ttw(ttr(TTrEventLow,"count_chg_type=%d" NL, pwr_ctrl->count_chg_type));
            			}
				#endif //end of ENABLE_USB_CHARGING

			}
	 		else
	 		{
				/*Now, START_ADC frequency can be reduced*/
				if(StartAdcFreqReduced == 0)
				{
					Set_START_ADC_period(0, 10, 30);  //changed from a previous value of 10
					StartAdcFreqReduced = 1;
					ttw(ttr(TTrEventLow,"START_ADC periodicity reduced :%d " NL, 30));
				}
			}
     		break;
    case CCI :
		
		#if ENABLE_CI_CV_OR_PWM
		status = ind->data[State];
		#endif  //end of ENABLE_CI_CV_OR_PWM
		
		if(pwr_cfg->data.Vbat_avg_mV < MONITOR_LEVEL && StartAdcFreqChanged == 0)
		{
   		       /* battery monitoring should happen at every 10 paging during charging */
			Set_START_ADC_period(0, 10, 10);   
			ttw(ttr(TTrEventLow,"START_ADC periodicity changed :%d " NL, 10));
			StartAdcFreqChanged =1;
		}
		
		#if (USE_BCI_CIRCUIT == 1)
		if(pwr_ctrl->time_begin_T3 == 0)
		{
			#if ENABLE_CI_CV_OR_PWM
			if ((check_chg_unplug() == 1)&&(lcc_ChgPathSts == AC_CHARGING_PATH_ENABLED)) 
			{
                		// Charger is not plugged anymore - stop!!
                		ttw(ttr(TTrEventLow,"Verdict - chg not plugged (%d)" NL, status));
		  		lcc_charger_status=0;
                		charger_unplug_house_keeping(); 
                		pwr_free(request);
                		return RV_OK;
            		}
			#endif  //end of ENABLE_CI_CV_OR_PWM
			#if ENABLE_USB_CHARGING
				//check for USB unplug here!	
			#endif //end of ENABLE_USB_CHARGING
		}
		#endif
        	pwr_check_timers();
        	// We are in the middle of a CI charging process - check stop criterias
        	if (pwr_chg_stop_CI(pwr_temp_lookup(pwr_cfg->data.Tbat_avg, 0x01), pwr_cfg->data.Vbat_avg_mV) == FALSE) 
		{

                     if(pwr_cfg->data.Vbat_avg_mV >= MONITOR_LEVEL && StartAdcFreqChanged == 1) {
                           /* battery monitoring should happen at every 1 paging after this voltage */					 	
			      Set_START_ADC_period(0, 10, 1);   
			      ttw(ttr(TTrEventLow, "START_ADC periodicity changed to :%d " NL, 1));
   			      StartAdcFreqChanged = 0;
                     }
					 
            		// Continue to charge - change state?
            		if ((pwr_cfg->bat.type == LITHIUM) && (pwr_chg_ci_cv_transition(pwr_cfg->data.Vbat_avg_mV)== TRUE)) 
			{
                		// Change state to CCV
                		// Start T2 timer
                		pwr_ctrl->state = CCV;
		  		pwr_start_timer(&pwr_ctrl->time_begin_T2);
    		  		ttw(ttr(TTrAll,"T2 started(%d)" NL, pwr_cfg->bat.T2));
				if(lcc_ChgPathSts == AC_CHARGING_PATH_ENABLED)
				{
					if (flag_PWM == 1)
						pwm_charging_house_keeping();
					else
						cv_charging_house_keeping(BSP_TWL3029_BCI_CHARGEPATH_AC); 
				}
				else
				{
					cv_charging_house_keeping(BSP_TWL3029_BCI_CHARGEPATH_USB);
				}
			}
        	}
		else 
		{
            		end_charging_house_keeping();
		}
        	break;
#if 0  //Since the low cost charger is not supported currently
    		case LCI :
        	// Poll ABB for charger unplug - Linear - unregulated chargers ONLY
        	status = ind->data[State];
        	pwr_ctrl->chg_unplug_vec[index] = (status & CHGPRES);
        	if (status & CHGPRES) 
		{
            		// Charger is still plugged - continue
            		ttw(ttr(TTrEventLow,"Polled - chg plugged (%d)" NL, status));
        	} 
		else 
		{
            		if (check_chg_unplug() == 1) 
			{
                		// Charger is not plugged anymore - stop!!
                		ttw(ttr(TTrEventLow,"Verdict - chg not plugged (%d)" NL, status));
                		// Change state
                		pwr_ctrl->state = SUP;
                		charger_unplug_house_keeping(); 
                		pwr_free(request);
                		return;
            		} 
			else 
			{
                // False alarm - don't set flags
                ttw(ttr(TTrEventLow,"Polled - chg not plugged - FALSE alarm? (%d)" NL, status));
#if (USE_Q401_CHG_CIRCUIT == 1)
                		if ((ind->data[Ichg] == 0) && (pwr_cfg->data.Vchg > 0)) 
				{
                    // Charging current has disappeared due to a fast unplug??? Bug in IOTA ABB?
                    pwr_ctrl->state = SUP;
                    charger_unplug_house_keeping(); 
                    pwr_free(request);
                    return;
                }
#endif
#if (USE_Q402_CHG_CIRCUIT == 1)
                // FIXME: Really no false alarm code for Q402??
#endif
#if (USE_BCI_CIRCUIT == 1)
                // FIXME: Really no false alarm code for BCI
#endif
            }
        }
        pwr_check_timers();
        // We are in the middle of a CI charging process - check stop criterias
        	if ( pwr_chg_stop_CI(pwr_temp_lookup(pwr_cfg->data.Tbat_avg, 0x01), pwr_cfg->data.Vbat_avg_mV) == FALSE) 
		{
            		// Continue to charge - change state?
            		if ((pwr_cfg->bat.type == LITHIUM) && (pwr_chg_ci_cv_transition(pwr_cfg->data.Vbat_avg_mV)== TRUE)) 
			{
                // Change state to LCV
                // Calculate k value
                // Start T2 timer
                // Start DC timer
                // Start T4 timer based on k value
                // Modulation ON
                pwr_ctrl->state = LCV;
                cv_charging_house_keeping();
            }
        	} 
		else 
		{
            		// Change state
            		pwr_ctrl->state = SUP;
            		end_charging_house_keeping();
        	}
        	break;
#endif			
    		case CCV :
              
              if (flag_PWM == 1) {
                 if (pwr_ctrl->time_elapsed_T0 > pwr_cfg->common.sampling) {
                     update_duty_cycle(); 
                 }            
    	       }			  

        	pwr_check_timers();
        	// We are in the middle of a CV charging process - check stop criterias
        	if (pwr_chg_stop_CV(pwr_temp_lookup(pwr_cfg->data.Tbat_avg, 0x01), pwr_cfg->data.Vbat_avg_mV,pwr_cfg->data.Ichg) == FALSE) 
		{
           		// EMPTY - waiting for T4 and DC timeouts OR T2 timeout
        	} 
		else 
		{
            	 	end_charging_house_keeping();
		}
        	break;
#if 0			
    		case LCV :
        	// Poll ABB for charger unplug - Linear - unregulated chargers ONLY
        	status = ind->data[State];
        	pwr_ctrl->chg_unplug_vec[index] = (status & CHGPRES);
        	if (status & CHGPRES) 
		{
            		// Charger is still plugged - continue
            		ttw(ttr(TTrEventLow,"Polled - chg plugged (%d)" NL, status));
        	} 
		else 
		{
			#if (USE_Q401_CHG_CIRCUIT == 1)
            			// Charger is not plugged anymore - stop!!
            			ttw(ttr(TTrEventLow,"Verdict - chg not plugged (%d)" NL, status));
            			// Change state
            			pwr_ctrl->state = SUP;
            			charger_unplug_house_keeping(); 
            			pwr_free(request);
            			return;
			#endif
			#if (USE_Q402_CHG_CIRCUIT == 1)
            			if (check_chg_unplug() == 1) 
				{
                			// Charger is not plugged anymore - stop!!
                			ttw(ttr(TTrEventLow,"Verdict - chg not plugged (%d)" NL, status));
                			// Change state
                			pwr_ctrl->state = SUP;
                			charger_unplug_house_keeping(); 
                			pwr_free(request);
                			return;
            			}
			#endif
			#if (USE_BCI_CIRCUIT == 1)
            			if (check_chg_unplug() == 1)
				{
                			// Charger is not plugged anymore - stop!!
                			ttw(ttr(TTrEventLow,"Verdict - chg not plugged (%d)" NL, status));
                			// Change state
                			pwr_ctrl->state = SUP;
                			charger_unplug_house_keeping(); 
                			pwr_free(request);
                			return;
            			}
			#endif
        	}
        	pwr_check_timers();
        	// We are in the middle of a CV charging process - check stop criterias
        	if (pwr_chg_stop_CV(pwr_temp_lookup(pwr_cfg->data.Tbat_avg, 0x01), pwr_cfg->data.Vbat_avg_mV,pwr_cfg->data.Ichg) == FALSE) 
		{
           		// EMPTY - waiting for T4 and DC timeouts OR T2 timeout
        	} 
		else 
		{
            		// Change state
            		pwr_ctrl->state = SUP;
            		end_charging_house_keeping();
        	}
        	break;
#endif 			
    default :
        {
        // Exception Handling - Unknown State
	       ttr(TTrFatal, "process_adc_indication: Unknown State: %d" NL, pwr_ctrl->state);
        }
    }
    pwr_free(request);
    return RV_OK;
}

#if (ANLG_FAM==11)
	/**********************************************************
	* function:  process_ab_chg_plugged_ind
	* Description :
	* The charger plug event is processed here.
	* The battery thermister needs to be enabled and the START_ADC frequency needs to be inreased
	***********************************************************/
#if ENABLE_CI_CV_OR_PWM
T_RV_RET process_abb_chg_plugged_ind(T_PWR_REQ *pwr_request)
{
	lcc_charger_status = CHGSTS;
	ttw(ttr(TTrEventLow,"charger plugged : reset T3 (%d)" NL, 0x00));
	if(thermisterEnabled == 0)
	{
		Bci_ThermisterEnable();
		thermisterEnabled = 1;
		ttw(ttr(TTrAll,"THEN enabled  (%d)" NL, 0x00));
	}
	pwr_stop_timer(&pwr_ctrl->time_begin_T3); //reset T3 after charger plug//

	/*Request for increase in the frequency of START ADC pulse*/
	if((StartAdcFreqReduced == 1) && (pwr_ctrl->state == SUP))
	{
		Set_START_ADC_period(0, 10, 1);
		StartAdcFreqReduced = 0;
		ttw(ttr(TTrEventLow,"START_ADC periodicity increased :%d " NL, 1));
	}
	/* indicate mmi that charger is plugged in */
	
	charger_plug_house_keeping();
	
	ChannelsToRead |=  BSP_TWL3029_ADC_CHANNEL_9_VCHG;
	bspTwl3029_Madc_setRtChannels(NULL, ChannelsToRead);
	ChargerVoltageAvailable = 1;									  				   	
										  
	pwr_free(pwr_request);
	return RV_OK;
}
#endif  //end of ENABLE_CI_CV_OR_PWM
#if 0//ENABLE_USB_CHARGING
T_RV_RET process_abb_USB_plugged_ind(T_PWR_REQ *pwr_request)
{
	lcc_USB_status = CHGSTS;
	ttw(ttr(TTrEventLow,"USB charger plugged : reset T3 (%d)" NL, 0x00));
	pwr_stop_timer(&pwr_ctrl->time_begin_T3); //reset T3 after charger plug//
	pwr_free(pwr_request);
	return RV_OK;
}


T_RV_RET process_abb_USB_unplugged_ind       (T_PWR_REQ *request)
{
	lcc_USB_status = 0;
	USB_unplug_house_keeping(); 
    	pwr_free(request);
    	return RV_OK;
}
#endif
#endif
	/**********************************************************
	* function:  process_ab_chg_unplugged_ind
	* Description :
	* The charger unplug event is processed here.
	* The battery thermister needs to be disabled 
	*
	***********************************************************/
#if  ENABLE_CI_CV_OR_PWM

T_RV_RET process_abb_chg_unplugged_ind       (T_PWR_REQ *request)
{

	#if (ANLG_FAM==11)
	lcc_charger_status = 0;
	#endif
    	charger_unplug_house_keeping(); 
    	// Disable charger interrupts - they where enabled when the CI charger was connected
	if((thermisterEnabled == 1) && (lcc_USB_status == 0))
	{
		bspTwl3029_Bci_sensorsDisable(NULL,  BSP_TWL3029_BCI_SENS_TEMP);
		thermisterEnabled = 0;
		ttw(ttr(TTrAll,"THEN disabled  (%d)" NL, 0x00));
	}
	
	#if (ANLG_FAM!=11)
    	ABB_Write_Register_on_page(PAGE0, ITMASK, CHARGER_IT_MSK);
	#endif
    	pwr_free(request);
    	return RV_OK;
}
#endif   

T_RV_RET process_abb_charge_end_ind          (T_PWR_REQ *request)
{
	end_charging_house_keeping();	
    pwr_free(request);
    return RV_OK;
}
	/**********************************************************
	* function:  pwr_send_msg
	* Description :
	* sends a message with a  "msg_id"  to the "dest_addr_id"
	***********************************************************/
void pwr_send_msg(uint32 msg_id, T_RVF_ADDR_ID src_addr_id, T_RVF_ADDR_ID  dest_addr_id)
{
    struct pwr_req_s *msg;
    if ((msg = pwr_malloc(sizeof(struct pwr_req_s))) == NULL) 
    {
        return;
    }
    msg->header.msg_id        = msg_id;
    msg->header.src_addr_id   = src_addr_id;
    msg->header.dest_addr_id  = dest_addr_id;
    msg->header.callback_func = NULL;
    if (rvf_send_msg(dest_addr_id, msg) != RV_OK) 
    {
        ttr(TTrFatal, "PWR FATAL: Send failed! %d" NL, 0xFF);
    }
}
	/**********************************************************
	* function:  mmi_send_msg
	* Description :
	* sends a message to MMI indicating an "event"
	***********************************************************/
void mmi_send_msg(struct mmi_info_ind_s *event) 
{
    ttw(ttr(TTrInit,"mmi_send_msg(%d)" NL, 0));
    if (pwr_ctrl->rpath.callback_func) 
    {
        ttw(ttr(TTrInit,"Using callback (0x%x)" NL, pwr_ctrl->rpath.callback_func));
        (pwr_ctrl->rpath.callback_func) (event);
    } 
    else 
    {
        if (pwr_ctrl->rpath.addr_id) 
	{
            // TESTME
            rvf_send_msg(pwr_ctrl->rpath.addr_id, event);
        } 
	else 
	{
            ttr(TTrFatal,"PWR FATAL: mmi_send_msg(%d) No return path" NL, 0xFF);
            return;
        }
    }
    ttw(ttr(TTrInit,"mmi_send_msg(%d)" NL, 0xFF));
}

	/**********************************************************
	* function:  start_pre_charge
	* Description :
	* 
	* Starts battery charging with a default current in constant current mode.
	***********************************************************/
void start_pre_charge(BspTwl3029_Bci_pathType path_type) 
{

		BspTwl3029_Bci_dacVal valArray[2];
		BspTwl3029_I2C_Callback  callback;
		 
    	ttw(ttr(TTrInitLow,"start_pre_charge" NL, 0x00));
		
       	valArray[0] = 0x292;  	////DAC voltage setting no config value read, should be hard coded: presently loading the default value for 					  overvoltage protection
	valArray[1] = ICHG_PRECHG;                ////( DAC current setting: no config value read, should be hard coded);
    	bspTwl3029_Bci_dacConfig(NULL, BSP_TWL3029_BCI_DACTYPE_CURRENT, valArray);
	bspTwl3029_Bci_linChargeEnable(NULL, path_type,BSP_TWL3029_BCI_CHARGEMODE_CONST_I); /*enable main charge*/
	bspTwl3029_Bci_SetWatchDog(NULL,BSP_TWL3029_BCI_WDKEY_INT_ACK);  /*Disable watch dog otherwise the charging will automatically stop after 4s*/	
	
	  
    	ttw(ttr(TTrInitLow,"start_pre_charge" NL, 0xFF));
}
	/**********************************************************
	* function:  start_pre_charge
	* Description :
	* 
	* Starts battery charging with a speific  current (configurable via ETM ) 
	* in constant current mode.
	* The harging wathdog is disabled and Over voltage protection is enabled 
	***********************************************************/
	   
void start_bci_charge(BspTwl3029_Bci_pathType path_type) 
{
	BspTwl3029_Bci_dacVal valArray[2];
    	ttw(ttr(TTrEventLow,"start_bci_charge(%d)" NL, 0x00));
		
    	if (pwr_ctrl->flag_mmi_registered == 1) 
	{
        	pwr_ctrl->mmi_ptr->header.msg_id = MMI_CHG_START_IND;
        	mmi_send_msg(pwr_ctrl->mmi_ptr);
    	}
		
	valArray[0] = 0x0;
	valArray[1] = 0x0;

       /* Disable the charging */
	bspTwl3029_Bci_linChargeDisable(NULL);  //linear charge disable . 
	bspTwl3029_Bci_dacConfig(NULL, BSP_TWL3029_BCI_DACTYPE_CURRENT, valArray);

	valArray[0] = OVER_VOLTAGE;
	valArray[1] = pwr_cfg->chg.ichg_max;

       /* Configure charging current for CI mode charging */
      	bspTwl3029_Bci_dacConfig(NULL, BSP_TWL3029_BCI_DACTYPE_CURRENT, valArray);

	/* Enable CI charging */
	bspTwl3029_Bci_linChargeEnable(NULL, path_type,BSP_TWL3029_BCI_CHARGEMODE_CONST_I);

       /*Disable watchdog */	
	bspTwl3029_Bci_SetWatchDog(NULL,BSP_TWL3029_BCI_WDKEY_INT_ACK);	

       /* Disable safety configuration */	   
	bspTwl3029_Bci_SetSkey(NULL, BSP_TWL3029_BCI_SKEY6_OFF);	 
	
	/* If not already started - start the overall charger timer T1 */
    	if (pwr_ctrl->time_begin_T1 == 0) 
	{
        	pwr_start_timer(&pwr_ctrl->time_begin_T1);
        	ttw(ttr(TTrAll,"T1 started(%d)" NL, 0));
    	}
	/*Enable ICHG measurement*/
	ChannelsToRead |=BSP_TWL3029_ADC_CHANNEL_8_ICHG;
	bspTwl3029_Madc_setRtChannels(NULL, ChannelsToRead);
	ttw(ttr(TTrEventLow,"start_bci_charge(%d)" NL, 0xFF));
}

	/**********************************************************
	* function:  start_pre_charge
	* Description :
	* 
	* Starts battery charging with a speific  current (configurable via ETM ) 
	* in constant current mode.
	* After charging is started ,the charging wathdog is disabled and Over voltage protection is enabled 
	***********************************************************/  
void stop_bci_charge(void) 
{
	BspTwl3029_Bci_dacVal valArray[2];
  	ttw(ttr(TTrInitLow,"stop_bci_charge(%d)" NL, 0x00));
    	if (pwr_ctrl->flag_mmi_registered == 1) 
	{
        	pwr_ctrl->mmi_ptr->header.msg_id = MMI_CHG_STOP_IND;
        	mmi_send_msg(pwr_ctrl->mmi_ptr);
    	}

	valArray[0] = 0x0;  		////(DAC voltage setting);   reset to zero
	valArray[1] = 0x0;              ////( DAC current setting);  reset to zero

	//Acknowledge the protection
	bspTwl3029_Bci_SetSkey(NULL, BSP_TWL3029_BCI_SKEY6_OFF);
		  
	//Disable the charging
	bspTwl3029_Bci_linChargeDisable(NULL);  //linear charge disable . 
	bspTwl3029_Bci_pwmChargeDisable(NULL); //PWM charge disable . FIXME: need to do selective disabling?? 
	lcc_ChgPathSts = NO_CHARGING_PATHS_ENABLED;

	//Configure CHGVREG to 0.
	bspTwl3029_Bci_dacConfig(NULL, BSP_TWL3029_BCI_DACTYPE_VOLT, valArray);	

	//Configure CHGIREG to 0.
	bspTwl3029_Bci_dacConfig(NULL, BSP_TWL3029_BCI_DACTYPE_CURRENT, valArray);

	/*Now, START_ADC frequency can be reduced*/
	if(StartAdcFreqReduced == 0)
	{
		Set_START_ADC_period(0, 10, 30);
		StartAdcFreqReduced = 1;
		ttw(ttr(TTrEventLow,"START_ADC periodicity reduced :%d " NL, 30));
	}
	//Disable ICHG channel of MADC.
	ChannelsToRead &= ~0x0080; 
	bspTwl3029_Madc_setRtChannels(NULL,ChannelsToRead);
	pwr_cfg->data.Ichg = 0; //reset the old value
	
    	ttw(ttr(TTrInitLow,"stop_bci_charge(%d)" NL, 0xFF));
}

#if 0
void stop_bci_linCharge(void) {

   //BspTwl3029_I2C_Callback  callback;
   
   //callback.callbackFunc = dummy_callback;
   //callback.callbackVal = 0x10;
   //callback.i2cTransArrayPtr = &transArray10;
   
    ttw(ttr(TTrInitLow,"stop_bci_linCharge(%d)" NL, 0x00));
    if (pwr_ctrl->flag_mmi_registered == 1) {
        pwr_ctrl->mmi_ptr->header.msg_id = MMI_CHG_STOP_IND;
        mmi_send_msg(pwr_ctrl->mmi_ptr);
    }
            
    	bspTwl3029_Bci_linChargeDisable(NULL);  //linear charge disable while in CI mode.   
    
    ttw(ttr(TTrInitLow,"stop_bci_linCharge(%d)" NL, 0xFF));
}

void stop_bci_pwmCharge(void) {
   //BspTwl3029_I2C_Callback  callback;
   
   //callback.callbackFunc = dummy_callback;
   //callback.callbackVal = 0x11;
   //callback.i2cTransArrayPtr = &transArray11;
	

    ttw(ttr(TTrInitLow,"stop_bci_pwmCharge(%d)" NL, 0x00));
    if (pwr_ctrl->flag_mmi_registered == 1) {
        pwr_ctrl->mmi_ptr->header.msg_id = MMI_CHG_STOP_IND;
        mmi_send_msg(pwr_ctrl->mmi_ptr);
    }

    bspTwl3029_Bci_pwmChargeDisable(NULL);  //PWM charge disable while in CV mode.	

//	pwr_modulate_off();
    ttw(ttr(TTrInitLow,"stop_bci_pwmCharge(%d)" NL, 0xFF));
}
#endif  
	/**********************************************************
	* function:  stop_pre_charge
	* Description :
	* Stops the software pre charge 
	***********************************************************/  
void stop_pre_charge(void) 
{
 
	#if (ANLG_FAM!=11)
	ABB_Write_Register_on_page(PAGE0, BCICTL2, 0);
	#else
	bspTwl3029_Bci_linChargeDisable(NULL);  //since we have started main charge in start_pre_charge
	#endif
}

#if  ENABLE_CI_CV_OR_PWM
int check_chg_unplug() 
{
    int i;
    // Assume that the charger unplug was true if BOTH the charger voltage & charger current 
    // are below configurable Vchg & Ichg threshold values
    if ((pwr_cfg->data.Vchg < VCHG_BCI_THR) && (pwr_cfg->data.Ichg < ICHG_BCI_THR)) 
    {
        ttw(ttr(TTrInitLow,"check_chg_unplug: Vchg=(%d)" NL, pwr_cfg->data.Vchg));
        ttw(ttr(TTrInitLow,"check_chg_unplug: Ichg=(%d)" NL, pwr_cfg->data.Ichg));
        return 1;
    }
    return 0;
	}
#endif  


	/**********************************************************
	* function:  charger_unplug_house_keeping
	* Description :
	* 	performs the task that needs to be performed on a charger unplug
	*	-Sends charger unplug indication to MMI
	*	-Disables the VCHG channel of MADC
	*	-stops the AC charging	
	*	-resets the START_ADC periodicity to the initial value if USB_CHARGING is not happening
	*	-starts timer T3
	*
	***********************************************************/  
#if  ENABLE_CI_CV_OR_PWM
void charger_plug_house_keeping(void)
{

if (pwr_ctrl->flag_mmi_registered == 1) 
    {
        // Send charger unplug to the MMI
        pwr_ctrl->mmi_ptr->header.msg_id = MMI_CHG_PLUG_IND;
        mmi_send_msg(pwr_ctrl->mmi_ptr);
        
    }

}
void charger_unplug_house_keeping(void) 
{
    // Flag the charger unplug
    pwr_ctrl->flag_chg_plugged = 0;
    pwr_ctrl->flag_chg_prev_plugged = 0;
    // Read charger configuration next time charger is inserted
    pwr_ctrl->flag_chg_cfg_read = 0;
    // We must have more charger id measurements if the charger is re-connected
    pwr_ctrl->count_chg_type = 0;
	  // DO NOT Apply topping charge from now on
     	  pwr_ctrl->flag_topping_chg = 0;
	   
    if (pwr_ctrl->flag_mmi_registered == 1) 
    {
        // Send charger unplug to the MMI
        pwr_ctrl->mmi_ptr->header.msg_id = MMI_CHG_UNPLUG_IND;
        mmi_send_msg(pwr_ctrl->mmi_ptr);
        // Re-start the mmi info repetition timer
        pwr_start_timer(&pwr_ctrl->time_begin_mmi_rep);
    }
	
    /*Disable the VCHG channel of MADC*/
    ChannelsToRead &= ~0x0100;   	
    bspTwl3029_Madc_setRtChannels(NULL,ChannelsToRead);
    ChargerVoltageAvailable = 0;
    pwr_cfg->data.Vchg = 0; //reset the old value	
	
    // if T1 or T2 is running (which mean we have NOT had a normal charging stop)
    if (((pwr_ctrl->time_begin_T1 != 0) || (pwr_ctrl->time_begin_T2 != 0)) &&(lcc_ChgPathSts == AC_CHARGING_PATH_ENABLED)) 
    {
        // Start T3 timer if not already started
        if (pwr_ctrl->time_begin_T3 == 0) 
	 {
            	pwr_start_timer(&pwr_ctrl->time_begin_T3);
	           	ttw(ttr(TTrAll,"T3 started(%d)" NL, 0));
	       }
		pwr_ctrl->state = SUP;
		stop_bci_charge();
	   }
	 else //not charging just need to reduce the START_ADC pulse frequency.
	 {
		/*Now, START_ADC frequency can be reduced*/
		if((StartAdcFreqReduced == 0)&&(lcc_ChgPathSts != USB_CHARGING_PATH_ENABLED))
		{
			Set_START_ADC_period(0, 10, 30);   //changed from previous value of 10
			StartAdcFreqReduced = 1;
			ttw(ttr(TTrEventLow,"START_ADC periodicity reduced :%d " NL, 30));
		}	
	 }
	}
#endif //end of ENABLE_CI_CV_OR_PWM 

	/**********************************************************
	* function:  USB_unplug_house_keeping
	* Description :
	* 	performs the task that needs to be performed on a USB unplug
	*	-Disables the USBVBUS channel of MADC
	*	-stops the USB charging	
	*	-resets the START_ADC periodicity to the initial value if AC_CHARGING is not happening
	************************************************************/  
#if ENABLE_USB_CHARGING 
void USB_unplug_house_keeping(void)
{
	// We must have more charger id measurements if the charger is re-connected
        pwr_ctrl->count_USB_type = 0;

		// DO NOT Apply topping charge frow now on
     	  	pwr_ctrl->flag_topping_chg = 0;
		
	/* Disable USBVBUS channel of MADC */
	ChannelsToRead &= ~0x0020;
	bspTwl3029_Madc_setRtChannels(NULL, ChannelsToRead);
	USB_VBUSAvailable = 0;
	pwr_cfg->data.USB_VBUS = 0; //reset the old value
	// if T1 or T2 is running (which mean we have NOT had a normal charging stop)
    	if (((pwr_ctrl->time_begin_T1 != 0) || (pwr_ctrl->time_begin_T2 != 0)) && (lcc_ChgPathSts == USB_CHARGING_PATH_ENABLED)) 
    	{
            pwr_stop_timers();
            ttw(ttr(TTrTimerLow,"T3 started(%d)" NL, 0));
            // Switch of charging circuit
	    pwr_ctrl->state = SUP;
            stop_bci_charge();
        }
	else //not charging just need to reduce the START_ADC pulse frequency.
  	{
		/*Now, START_ADC frequency can be reduced*/
			if((StartAdcFreqReduced == 0)&&(lcc_ChgPathSts != AC_CHARGING_PATH_ENABLED))
		{
			Set_START_ADC_period(0, 10, 30);   //changed from previous value of 10
			StartAdcFreqReduced = 1;
			ttw(ttr(TTrEventLow,"START_ADC periodicity reduced :%d " NL, 30));
		}	
  	}
}
#endif  //end of ENABLE_USB_CHARGING

	/**********************************************************
	* function:  cv_charging_house_keeping
	* Description :
	* 	performs the task that needs to be performed  when a CI to CV transition is reached
	*	-Removes the protection
	*	-configure the DAC for the constant voltage (4.2V)	
	*	-switches to the Constant voltage charging mode
	*************************************************************/  
void cv_charging_house_keeping(BspTwl3029_Bci_pathType path_type) 
{
       BspTwl3029_Bci_dacVal valArray[2];
	BspTwl3029_I2C_RegData reg_bcictl1;	
	valArray[0] = 0x0;                      
       valArray[1] = 0x0; 
	ttw(ttr(TTrAll,"start_cv_charge(%d)" NL, 0x00));


    /* Disable the charging */
    bspTwl3029_Bci_linChargeDisable(NULL);  //linear charge disable . 
    bspTwl3029_Bci_dacConfig(NULL, BSP_TWL3029_BCI_DACTYPE_VOLT, valArray);

    /* Disable safety configuration */	   
    bspTwl3029_Bci_SetSkey(NULL, BSP_TWL3029_BCI_SKEY6_OFF);	 

    valArray[0] = 0x264;          		// old value was 0x267               
    valArray[1] = ICHG_BCI_THR; 	//modulating the refference current with respect to the 'k' value.

    /* Enable charging in CV mode */	
    bspTwl3029_Bci_dacConfig(NULL, BSP_TWL3029_BCI_DACTYPE_VOLT, valArray); //set voltage refference to 4.2V
    bspTwl3029_Bci_linChargeEnable(NULL, path_type,BSP_TWL3029_BCI_CHARGEMODE_CONST_V);

    /* Disable safety configuration */	   
    bspTwl3029_Bci_SetSkey(NULL, BSP_TWL3029_BCI_SKEY6_OFF);	 

    #if 0
    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,BSP_TWL3029_MAP_BCI_BCICTL1_OFFSET,&reg_bcictl1); 
    reg_bcictl1 = reg_bcictl1 & 0xFD;  //switch to CV charging mode.
	BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_BCI,BSP_TWL3029_MAP_BCI_BCICTL1_OFFSET,reg_bcictl1,NULL); 
    #endif
	
	//Configure CHGIREG for setting the eoc protection.	
	bspTwl3029_Bci_dacConfig(NULL, BSP_TWL3029_BCI_DACTYPE_CURRENT, valArray);
	//Configure SKEY register for end of charge protection 
	//bspTwl3029_Bci_SetSkey(NULL, BSP_TWL3029_BCI_SKEY3_EOC);	 
	}

	/**********************************************************************************
	* function:  pwm_charging_house_keeping
	* Description :
	* 	performs the task that needs to be performed  when a PWM charging needs to be started.
	*	-this function basically starts a PWM charging and disables the charging Watchdog.
	*	-a mod_cycle_timer also started (when this timer is expired, the dutycyle of the PWM wave needs to be reduced)
	***********************************************************************************/ 

void pwm_charging_house_keeping(void)
{
    // Calculate k value
    // Start DC timer
    // Start T4 timer based on k value
    // Modulation ON
    
#if (USE_BCI_CIRCUIT == 1)
    uint16 duty_cycle;
    BspTwl3029_Bci_dacVal valArray[2];
#endif

    valArray[0] = 0x0;                         
    valArray[1] = 0x0;

    /* Disable the charging */
    bspTwl3029_Bci_linChargeDisable(NULL);  //linear charge disable . 
    bspTwl3029_Bci_dacConfig(NULL, BSP_TWL3029_BCI_DACTYPE_VOLT, valArray);

    /* Disable safety configuration */	   
    bspTwl3029_Bci_SetSkey(NULL, BSP_TWL3029_BCI_SKEY6_OFF);	 

    /* Calculate duty cycle */
    ttw(ttr(TTrEventLow,"Vbat_avg, chg_start_thr, chg_stop_thr (%d), (%d), (%d)" NL, pwr_cfg->data.Vbat_avg_mV, pwr_cfg->bat.chg_start_thr, pwr_cfg->bat.chg_stop_thr));
    if ((pwr_cfg->data.Vbat_avg_mV - pwr_cfg->bat.chg_start_thr) <= 0) {
        // Assign k to max value if Vbat_avg value is below start value
        pwr_cfg->data.k = PWR_MAX_K;
    } 
    else 
    {
        pwr_cfg->data.k = (255 * (pwr_cfg->bat.chg_stop_thr - pwr_cfg->data.Vbat_avg_mV) / (pwr_cfg->bat.chg_stop_thr - pwr_cfg->bat.chg_start_thr));
        if (pwr_cfg->data.k <= PWR_MIN_K)
        pwr_cfg->data.k = PWR_MIN_K;
    }
    
    valArray[0] = OVER_VOLTAGE;                         
    valArray[1] = (pwr_cfg->data.k * pwr_cfg->chg.ichg_max)/255; //modulating the refference current with respect to the 'k' value.
 
    duty_cycle = (1023 *pwr_cfg->data.k)/255;

    /* Enable AC path */
    bspTwl3029_Bci_dacConfig(NULL, BSP_TWL3029_BCI_DACTYPE_CURRENT, valArray);	
    bspTwl3029_Bci_linChargeEnable(NULL, BSP_TWL3029_BCI_CHARGEPATH_AC, BSP_TWL3029_BCI_CHARGEMODE_CONST_I);
	
    /* Enable PWM charging */	
    bspTwl3029_Bci_pwmChargeEnable(NULL,duty_cycle,BSP_TWL3029_BCI_CHARGEMODE_CONST_I); 
 
    //callback.callbackVal = 0x14;
    //callback.i2cTransArrayPtr = &transArray14;
    bspTwl3029_Bci_SetWatchDog(NULL,BSP_TWL3029_BCI_WDKEY_INT_ACK);  /*Disable watch dog otherwise the charging will automatically stop after 4s*/

     //callback.callbackVal = 0x23;
     //callback.i2cTransArrayPtr = &transArray23;
     //bspTwl3029_Bci_SetSkey(NULL, BSP_TWL3029_BCI_SKEY4_V);	 //enable overvoltage protection.
     ttw(ttr(TTrEventLow,"pwr_cfg->data.k, duty_cycle, CHGIREG (%d), (%d), (%d)" NL, pwr_cfg->data.k, duty_cycle, valArray[1]));
}


	/**********************************************************************************
	* function:  end_charging_house_keeping
	* Description :
	*	-stops the battery harging.
	*	-restarts the mmi_repetition timer if mmi has registered.
	***********************************************************************************/ 
void end_charging_house_keeping(void) 
{
	if(pwr_ctrl->state == CCV)
	{
		bspTwl3029_IntC_disableSource(BSP_TWL3029_INTC_SOURCE_ID_BCI_CHARGE_STOP);
	}
	pwr_ctrl->state = SUP;
    	// Stop charging
	   	stop_bci_charge();
	 // Stop & reset timers
    	pwr_stop_timers();
	// Start the mmi info repetition timer
    	if (pwr_ctrl->flag_mmi_registered == 1)
        pwr_start_timer(&pwr_ctrl->time_begin_mmi_rep);
    	// Read charger configuration next time charger is inserted
    	pwr_ctrl->flag_chg_cfg_read = 0;
}

#if (TEST_PWR_MMI_INTERFACE == 1)
// Callback function used for testing MMI reporting
static void mmi_test_cb_function(void *ptr) {
T_PWR_MMI_INFO_IND_EVENT *event;


    event = (T_PWR_MMI_INFO_IND_EVENT *)ptr;

    ttw(ttr(TTrInit, "mmi_test_cb_function (%d)" NL, 0));
    ttw(ttr(TTrInit, "MMI event: (%d)" NL, event->header.msg_id));
    ttw(ttr(TTrInit, "mmi_test_cb_function (%d)" NL, 0xFF));
}
#endif

void dummy_callback(BspI2c_TransactionId transID)
{
ttw(ttr(TTrEventLow,"dummy_callback_TransID(%d)" NL, transID));
}

#if 0
T_RV_RET process_pwr_WD_overflow (T_PWR_REQ *request)
{
   //BspTwl3029_I2C_Callback  callback;
   
   //callback.callbackFunc = dummy_callback;
   //callback.callbackVal = 0x16;
   //callback.i2cTransArrayPtr = &transArray16;

   ttw(ttr(TTrTimer, "process_pwr_WD_overflow(%d)" NL, 0));
   bspTwl3029_Bci_SetWatchDog(NULL,BSP_TWL3029_BCI_WDKEY_INT_ACK); /*acknowledge the watchdog overflow -allows  to enable the CHEN again*/

   if((pwr_ctrl->state == CCI)||(pwr_ctrl->state == PRE))
   {	
	//callback.callbackVal = 0x17;
	//callback.i2cTransArrayPtr = &transArray17;
	bspTwl3029_Bci_linChargeEnable(NULL, BSP_TWL3029_BCI_CHARGEPATH_AC,BSP_TWL3029_BCI_CHARGEMODE_CONST_I); /*enable the main charge*/
   }
   if(pwr_ctrl->state == CCV)
   {
	cv_charging_house_keeping();
   }
   //callback.callbackVal = 0x18;
   //callback.i2cTransArrayPtr = &transArray18;
   bspTwl3029_Bci_SetWatchDog(NULL,BSP_TWL3029_BCI_WDKEY_8S); /*enable 8 sec watch dog*/
   pwr_free(request);
   ttw(ttr(TTrTimer, "process_pwr_WD_overflow(%d)" NL, 0xFF));
return RV_OK;
}
#endif 
	/**********************************************************************************
	* function:  Set_START_ADC_period
	* Description :
	*	-Sents MMI_ADC_REQ message to the L1.
	*	-used to change the START_ADC periodicity during GSM IDLE
	*	-CST module sets the default value and changed by LCC on different events
	* Prameters
	* 	tx_flag
	*	traffic_period
	* 	idle_period
	* Return
	*       None
	***********************************************************************************/ 

void Set_START_ADC_period (UBYTE tx_flag, UBYTE traffic_period, UBYTE idle_period)
{

extern T_HANDLE L1_hCommL1;

  typedef struct
  {
    UBYTE  tx_flag;
    UBYTE  traffic_period;
    UBYTE  idle_period;
  }
  T_MMI_ADC_REQ;

  #define MMI_ADC_REQ  (111)

  PALLOC(adc_req, MMI_ADC_REQ);

  adc_req->tx_flag  = tx_flag;
  adc_req->traffic_period  = traffic_period;
  adc_req->idle_period  = idle_period;

  PSEND(L1_hCommL1, adc_req);

}
	/**********************************************************************************
	* function:  pwr_check_AC_charger
	* Description :
	*	-Cheks the presents of AC charger
	* Prameters
	* 	  None
	* Return
	*	    1 (charger present)
	*	    0 (charger not present)
	***********************************************************************************/ 
uint8 pwr_check_AC_charger(void)
{
	if(pwr_ctrl->count_chg_type != CHARGER_TYPE_SLIP)
	{
		return(0);
	}
	if ((pwr_ctrl->flag_chg_plugged == 1) && (pwr_cfg->data.Vchg > CHARGER_TYPE_OUT_OF_BOUND_LOW))
 	{
 		pwr_cfg->data.chg_id = pwr_cfg->data.Vchg;
		return(1);	
	}
	else
		return (0);
}
	/**********************************************************************************
	* function:  pwr_check_USB_charger
	* Description :
	*	-Cheks the presents of USB charger
	* Prameters
	* 	  None
	* Return
	*	  1 (USB charger present)
	*	  0 (USB charger not present)
	***********************************************************************************/ 
uint8 pwr_check_USB_charger(void)
{

	uint16       USB_VBUS_mV;

		if(pwr_ctrl->count_USB_type != CHARGER_TYPE_SLIP)
		{
			return(0);
		}
		
	USB_VBUS_mV = 4*ADC_to_mV(pwr_cfg->data.USB_VBUS);
	
	if((pwr_ctrl->flag_USB_plugged == 1) && (USB_VBUS_mV > CHARGER_TYPE_OUT_OF_BOUND_LOW))		
	{				
		pwr_cfg->data.chg_id = pwr_cfg->data.USB_VBUS;
		return(1);
	}	
	else
		return(0);   

}

	/**********************************************************************************
	* function:  pwr_precharge_start_check
	* Description :
	*	-Cheks whether software precharge needs to be staretd or not 
	* Prameters
	* 	  None
	* Return
	*	 1 (precharge needs to be stareted)
	*	 0 (precharge need not be stareted)
	***********************************************************************************/ 
uint8 pwr_precharge_start_check(void)
{
	#if ENABLE_CI_CV_OR_PWM
		if((pwr_cfg->data.Vbat_avg_mV < VBAT_PRECHG_START) && (pwr_ctrl->flag_chg_plugged == 1))
			return(1);
		else
		{
			#if(ENABLE_USB_CHARGING == 0)
			return(0);
			#endif //end of ENABLE_USB_CHARGING
		}
	#endif //end of ENABLE_CI_CV_OR_PWM 
	
	#if ENABLE_USB_CHARGING
		if((pwr_cfg->data.Vbat_avg_mV < VBAT_PRECHG_START) && (pwr_ctrl->flag_chg_plugged == 1))
			return(1);
		else
			return(0);
	#endif //end of ENABLE_USB_CHARGING
}
	/**********************************************************************************
	* function:  Bci_ThermisterEnable
	* Description :
	*	-Enables the current source for battery temperature measurement with a 
	*	  current value which is present in  FFS. Two values (30 micro amps and 50 micro amps) 
	*	  are supported. if a differnt value is present in the FFS, a default value of 30 micro amps is used
	* Prameters
	* 	  None
	* Return
	*	None 
	***********************************************************************************/ 
void Bci_ThermisterEnable(void)
	{
		if (pwr_cfg->temp.i_meas2 == 50) 
		{
        		ttw(ttr(TTrInitLow,"ABB set i_current: %d" NL, pwr_cfg->temp.i_meas2));
			#if (ANLG_FAM!=11)
              		ABB_Write_Register_on_page(PAGE0, BCICTL1, THERMAL_SENSOR_50uA);
			#else
			bspTwl3029_Bci_sensorsEnable(NULL, BSP_TWL3029_BCI_SENS_TEMP, BSP_TWL3029_BCI_THERM_I_50, BSP_TWL3029_BCI_THERMSIGN_NEG);
			#endif
       		} 
		else
		{
			if (pwr_cfg->temp.i_meas2 == 30) 
			{
                		ttw(ttr(TTrInitLow,"ABB set i_current: %d" NL, pwr_cfg->temp.i_meas2));
				#if (ANLG_FAM!=11)
                		ABB_Write_Register_on_page(PAGE0, BCICTL1, THERMAL_SENSOR_30uA);
				#else
				bspTwl3029_Bci_sensorsEnable(NULL, BSP_TWL3029_BCI_SENS_TEMP, BSP_TWL3029_BCI_THERM_I_30, BSP_TWL3029_BCI_THERMSIGN_NEG);
				#endif
            		} 
			else 
			{
              	 		// Default 30uA
                		ttw(ttr(TTrInitLow,"ABB set i_current default: %d" NL, pwr_cfg->temp.i_meas2));
				#if (ANLG_FAM!=11)
                		ABB_Write_Register_on_page(PAGE0, BCICTL1, THERMAL_SENSOR_30uA);
				#else
				bspTwl3029_Bci_sensorsEnable(NULL, BSP_TWL3029_BCI_SENS_TEMP, BSP_TWL3029_BCI_THERM_I_30, BSP_TWL3029_BCI_THERMSIGN_NEG);
				#endif
            		}
		}	
	}
