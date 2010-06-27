/*******************************************************************************
*
* power.c
*
* Purpose: This file contains the functions used for power on and off 
*          management of the board.
*          In case of RVM_PWR_SWE, this file contains also the 
*          functions used for charger plug and unplug management. 
*          They are all called after the occurence of the ABB interrupt.
*																			
* Author:  Candice Bazanegue (c-brille@ti.com)						
*																			
*																			
* (C) Texas Instruments 2001
*
******************************************************************************/

#ifndef _WINDOWS
  #include "rv.cfg"
  #include "l1sw.cfg"
  #include "chipset.cfg"
#endif

#if (ANLG_FAM!=11)
#include "abb/abb.h"
#else
#include "sys_types.h"
#include "abb/bspTwl3029.h"
#include "abb/bspTwl3029_I2c.h"
#include "abb/bspTwl3029_Power.h"
#include "abb/bspTwl3029_Intc.h"
#endif
#include "rvm/rvm_use_id_list.h"
#if (ANLG_FAM!=11)
#include "spi/spi_env.h"
#endif
#include "power/power.h"
#include "rv/rv_defined_swe.h"	   // for RVM_PWR_SWE

#ifndef _WINDOWS
  #include "kpd/kpd_power_api.h"
  #include "ffs/ffs.h"
#endif  // _WINDOWS

#ifdef RVM_PWR_SWE
  #include "spi/spi_task.h"
  #ifndef _WINDOWS
    #include "chipset.cfg"
  #endif  // _WINDOWS
  #include "pwr/pwr_messages.h"
  #include "pwr/pwr_liion_cha.h"
  #include "pwr/pwr_disch.h"
#endif /* #ifdef RVM_PWR_SWE */

#if 0
#include "ffs/ffs.h"		// Andrew
void bim_save_bit(void);		// Andrew
void btm_save_btt(void);		// Andrew
#endif
#include "rv/rv_defined_swe.h"



#ifndef _WINDOWS
  /* Declare the variable containing pressed and released callback.  */
  extern T_KPD_KEYPAD Kp;

  extern effs_t ffs_exit(void);
#endif  // _WINDOWS


/// The ABB status used to know the cause of the Switch ON event.
static SYS_UWORD16 Power_abb_status = 0;


#if (ANLG_FAM==11)

#define NU_Sleep(wait) wait_ARM_cycles(convert_nanosec_to_cycles((wait)*4.65*1000))

enum
{
	POWER_ON =0,
	POWER_OFF
};
SYS_UWORD8 power_state;
/****************************************************************/
/* Power ON/OFF key definition.                                 */
/****************************************************************/
// If <ON/OFF> key is pressed more than 20 TDMAs, it's a Hook-ON
#define SHORT_OFF_KEY_PRESSED  (20)
  
// If <ON/OFF> key is pressed more than 160 TDMAs, it's a Hook-ON AND then Power-OFF
#define OFF_LOOP_COUNT   (8)
#define LONG_OFF_KEY_PRESSED   (OFF_LOOP_COUNT * SHORT_OFF_KEY_PRESSED)
/*******************************************************************************
**
** Function         Power_Key_Interrupt
**
** Description      
**
**
*******************************************************************************/
extern T_RV_RET kpd_power_key_pressed(void);

static SYS_UWORD8 status_value, loop_count=0;

static void power_callback(Uint8 i)
{
	if (status_value & (1<<BSP_TWL3029_POWER_PWON_OFFSET))
		kpd_power_key_pressed();
}

static void Power_Key_Interrupt(BspTwl3029_IntC_SourceId id)
{
//	rtc_power_key_pressed();
    if (power_state == POWER_ON)
	{
	loop_count=0;
    	bspTwl3029_Power_SwitchOnStatusWithCallback(&status_value,power_callback);
    	}
    else
	{ 
        rvf_send_trace("IQ EXT: Power On request",24, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, SPI_USE_ID);
        Power_ON_Button();
	 }		
}

T_RV_RET Power_handle_power_key_press(void)
{
	/* Push Button from ON to OFF */
    if (power_state == POWER_ON)
    {
		// WCP Patch
      	#if (OP_WCP == 0)
       loop_count++;
	if (loop_count<OFF_LOOP_COUNT)
       {
           rvf_delay(SHORT_OFF_KEY_PRESSED);
           bspTwl3029_Power_SwitchOnStatusWithCallback(&status_value,power_callback);	   
	}  
	else
	{
            rvf_send_trace("IQ EXT: Power Off request",25, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, SPI_USE_ID);
   		    rvf_send_trace("CALLING POWER OF BUTTON",23, NULL_PARAM, RV_TRACE_LEVEL_ERROR, SPI_USE_ID);
            Power_OFF_Button();
        }
        #endif //OP_WCP
    }
    return RV_OK;
}


void Switch_Off(void)
{
	SYS_UWORD8 status_value;

	bspTwl3029_Power_SwitchOnStatus(&status_value);
	/* FIXME : Revisit This After Charger */
	#if 0
	if ((status_value & (1<<BSP_TWL3029_POWER_CHG_OFFSET)))
    {
    	// Charger present => Can't-Off the Board
		return;
  	}
	#endif
	//Wait until all necessary actions are performed (write in FFS, etc...) to power-off the board (empirical value - 30 ticks).
  	NU_Sleep (30);

    // Wait also until <ON/OFF> key is released.
    // This is needed to avoid, if the power key is pressed for a long time, to switch
    // ON-switch OFF the mobile, until the power key is released.
  
  	while ((status_value & (1<<BSP_TWL3029_POWER_PWON_OFFSET)))
    {
    	bspTwl3029_Power_SwitchOnStatus(&status_value);
		NU_Sleep (1);
    }

  	BZ_KeyBeep_OFF();

    bspTwl3029_Power_SwitchOff();
}
#endif

/*******************************************************************************
**
** Function      Power_ON_Button
**
** Description:  Informs the Protocol Stack that the Power ON key has been  
**				 pressed, and thus allows it to start.
**
*******************************************************************************/
void Power_ON_Button(void)
{
#ifndef _WINDOWS
#if (ANLG_FAM!=11)
   SPI_GBL_INFO_PTR->is_gsm_on = TRUE;
#else
   power_state=POWER_ON;
#endif   
#if ((OP_WCP == 0) && (TEST == 0))

   /* Wait until the pointers initialization by the SMI */
   while(!(Kp.pressed && Kp.released))
     rvf_delay(1);

   (Kp.pressed)(KPD_PWR);  /* tell key is pressed */
   rvf_delay(5);
   (Kp.released)();        /* allow protocol stack to start */
#endif

#else
  rvf_send_trace("Start from button", 17, NULL_PARAM, RV_TRACE_LEVEL_WARNING, LCC_USE_ID);
  SPI_GBL_INFO_PTR->is_gsm_on = TRUE;
#endif  // _WINDOWS
}



/*******************************************************************************
**
** Function      Power_OFF_Button
**
** Description:  Informs the Protocol Stack that the Power OFF key has been  
**				 pressed, and thus allows it to stop.
**
*******************************************************************************/
void Power_OFF_Button(void)
{

    T_RV_RET ret;
    #if 0
  // Andrew	///////////////////////////////////
	/**** create the gbi fast-bootup ffs directory ****/
	ret = ffs_mkdir("/gbi");
	if ((ret != EFFS_OK) && (ret != EFFS_EXISTS))
	{
		rvf_send_trace("Fail to create gbi directory in POWER OFF", 31, NULL_PARAM, RV_TRACE_LEVEL_WARNING, LCC_USE_ID);
	}
	else
	{
#if( (!defined(RVM_MC_SWE)) && (defined (RVM_NAN_SWE)) )
	    rvf_send_trace("Saving to NOR during POWER OFF",30, NULL_PARAM, RV_TRACE_LEVEL_ERROR, LCC_USE_ID);
		bim_save_bit();		// Andrew
		btm_save_btt();		// Andrew
#endif		
	}
// Andrew	///////////////////////////////////	 

	 rvf_send_trace("SAVED to NOR during POWER OFF",29, NULL_PARAM, RV_TRACE_LEVEL_ERROR, LCC_USE_ID);
#endif

#ifndef _WINDOWS
   ffs_exit ();
#endif  // _WINDOWS
   rvf_send_trace("Power off button", 16, NULL_PARAM, RV_TRACE_LEVEL_ERROR, LCC_USE_ID);
#ifndef _WINDOWS
  #if (OP_WCP == 0)

   (Kp.pressed)(KPD_PWR);  /* tell key is pressed */
   rvf_delay(5);
   (Kp.released)();        /* tell key is released */

  #endif
#endif  // _WINDOWS

#if (ANLG_FAM!=11)
   SPI_GBL_INFO_PTR->is_gsm_on = FALSE;
#else
   power_state=POWER_OFF;
#endif
}


/**
 * @brief Sets the cause of the swith ON event.
 *
 * This function get the ABB status register and stores it into the
 * global variable Power_abb_status.
 *
 * @noparam
 *
 * @noreturn
 */
void Set_Switch_ON_Cause(void)
{
#if (CHIPSET!=15)
   Power_abb_status = ABB_Read_Status();
#else
   SYS_UWORD8 status;
   bspTwl3029_Power_SwitchOnStatus(&status);
   Power_abb_status = status;

#endif
}


/*******************************************************************************
**
** Function      Switch_ON
**
** Description   This function is called by the spi task after the board is  
**               switched ON. It calls the appropriate function according
**               to the ABB status register.
**
** Warning       The Set_Switch_ON_Cause() function has to be called prior.
**
*******************************************************************************/
void Switch_ON(void)
{


#if ((ANLG_FAM == 1) || (ANLG_FAM == 2))
   if (Power_abb_status & ONBSTS)
#elif ((ANLG_FAM == 3) || (ANLG_FAM == 4))
   if (Power_abb_status & PWONBSTS)
#elif (ANLG_FAM==11)
   if (Power_abb_status & (1<<BSP_TWL3029_POWER_PWON_OFFSET))
#endif
   {
      /* Switch on Condition on ON BUTTON Push */
      rvf_send_trace("Push Button from OFF to ON",26, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID);
      Power_ON_Button();
#ifdef RVM_PWR_SWE
      pwr_handle_discharge();
#endif
   }


#if ((ANLG_FAM == 1) || (ANLG_FAM == 2))
   else if (Power_abb_status & ONRSTS) 
#elif ((ANLG_FAM == 3) || (ANLG_FAM == 4))
   else if (Power_abb_status & RPSTS)
#elif (ANLG_FAM == 11)
   else if (Power_abb_status & (1<<BSP_TWL3029_POWER_RPWON_OFFSET))
#endif
   {
      /* Switch on Condition on ON REM transition 0->1 */
      rvf_send_trace("ON Remote",9, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID); 
      Power_ON_Remote();
#ifdef RVM_PWR_SWE
      pwr_handle_discharge();
#endif
   }


#if ((ANLG_FAM == 3) || (ANLG_FAM == 4))
   else if (Power_abb_status & RP2STS)
   {
      /* Switch on Condition on ON REM 2 transition 0->1 */
      rvf_send_trace("ON Remote 2",11, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID); 
      Power_ON_Remote();
#ifdef RVM_PWR_SWE
      pwr_handle_discharge();
#endif
   }


   else if (Power_abb_status & USBSTS)
   {
      /* Switch on Condition on an USB plug */
      rvf_send_trace("USB plug",8, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID); 
   }
#endif

#if (ANLG_FAM == 11)
   else if (Power_abb_status & (1<<BSP_TWL3029_POWER_USB_OFFSET))
   {
      /* Switch on Condition on an USB plug */
      rvf_send_trace("USB plug",8, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID); 
      Power_ON_Button();	  
   }
   else if (Power_abb_status & (1<<BSP_TWL3029_POWER_CHG_OFFSET))
   {
      /* Switch on Condition on CHARGER IC PLUG */
      rvf_send_trace("PWR: Charger Plug",17, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID); 

#ifdef RVM_PWR_SWE
      PWR_Charger_Plug();
#else /* #ifdef RVM_PWR_SWE */
	Power_ON_Button();
#endif
#else
   else if (Power_abb_status & CHGSTS)
   {
      /* Switch on Condition on CHARGER IC PLUG */
      rvf_send_trace("PWR: Charger Plug",17, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID); 

#ifdef RVM_PWR_SWE
      if (Power_abb_status & CHGPRES)
      {
         /* charger still present */
         PWR_Charger_Plug();
      }
#endif /* #ifdef RVM_PWR_SWE */
#endif
   }


   else
   {
      // The reset should run the SW in the same way than a Power ON
      rvf_send_trace("Start from reset",16, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID); 
      Power_ON_Button();
#ifdef RVM_PWR_SWE
      pwr_handle_discharge();
#endif
   }
#if (ANLG_FAM==11)
/* Register the Power Key Interrupt Handler */
	bspTwl3029_IntC_setHandler(BSP_TWL3029_INTC_SOURCE_ID_PW_FALL_EDGE, Power_Key_Interrupt);
	bspTwl3029_IntC_enableSource(BSP_TWL3029_INTC_SOURCE_ID_PW_FALL_EDGE);

#endif
}



/*******************************************************************************
**
** Function         Power_ON_Remote
**
** Description      
**
**
*******************************************************************************/
void Power_ON_Remote(void)
{
	/* FIXME: Patch for I-Sample 1.1 */
	rvf_send_trace("Start from Remote, Routing to Power ON",16, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID); 
	Power_ON_Button();
}



/*******************************************************************************
**
** Function         Power_OFF_Remote
**
** Description      
**
**
*******************************************************************************/
void Power_OFF_Remote(void)
{
	/* FIXME: Patch for I-Sample 1.1 */
	rvf_send_trace("Stop from Remote, Routing to Power OFF",16, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID); 
	Power_OFF_Button();
}



/*******************************************************************************
**
** Function         Power_IT_WakeUp
**
** Description      
**
**
*******************************************************************************/
void Power_IT_WakeUp(void)
{
}



#ifdef RVM_PWR_SWE
/*******************************************************************************
**
** Function         PWR_Charger_Plug
**
** Description      
**
**
*******************************************************************************/
void PWR_Charger_Plug(void)
{

  /* Start the fast charging cycle */

  /* Since this function is called from the SPI task
   it can't be interrupt by another task
   so we can directly access the SPI through the low-level driver */

   if (SPI_GBL_INFO_PTR->is_gsm_on == FALSE) /* GSM OFF */
   {
      rvf_delay(RVF_MS_TO_TICKS(2000));
   }

   rvf_stop_timer(SPI_TIMER3);

   /* informs the upper layer that the charger has been plugged */
   pwr_send_charger_plug_event();

   /* get the type of the battery */
   pwr_get_battery_type();
}



/*******************************************************************************
**
** Function         PWR_Charger_Unplug
**
** Description      
**
**
*******************************************************************************/
void PWR_Charger_Unplug(void)
{
   /* informs the upper layer that the charger has been unplugged */
   pwr_send_charger_unplug_event(); 

   rvf_send_trace("Charger unplug", 14, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID );
   pwr_stop_charging();
   if (SPI_GBL_INFO_PTR->is_gsm_on == FALSE) /* GSM OFF */
   {
    #ifndef _WINDOWS
      #if (ANLG_FAM == 1) 
      ABB_Write_Register_on_page(PAGE0, VRPCCTL2, 0x00EE);
      #elif ((ANLG_FAM == 2) || (ANLG_FAM == 3)|| (ANLG_FAM == 4))
      ABB_Write_Register_on_page(PAGE0, VRPCDEV, 0x0001);
      #endif
    #else
      ABB_Write_Register_on_page(PAGE0, VRPCDEV, 0x00EE);
    #endif  // _WINDOWS
   }
   else
   {
      pwr_handle_discharge();
   }
}
#endif /* #ifdef RVM_PWR_SWE */

