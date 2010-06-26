/**
 * @file   kpd_scan_functions.h
 *
 * Declaration of keypad interface functions.
 *
 * @author   Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  10/10/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _KPD_SCAN_FUNCTIONS_H_
#define _KPD_SCAN_FUNCTIONS_H_

#include "kpd/kpd_i.h"

#include "rv/rv_general.h"

#include "l1sw.cfg"


/* For Windows environment */
#ifdef _WINDOWS
   #ifdef __cplusplus
      extern "C"
      {
   #endif
#endif


/**
 * @name Internal functions
 *
 */
/*@{*/



#if ((CHIPSET == 12) || (CHIPSET == 15))

#if (L1_RF_KBD_FIX == 1)
#define KEYPAD_RF_ISSUE_SW_WORKAROUND 2
#endif
	      
/** 
 * function: kpd_init_ctrl_reg
 * Initialize the Control register
 */
void kpd_init_ctrl_reg(const UINT8 software_nreset, 
                       const UINT8 nsoftware_mode, 
                       const T_KPD_PTV ptv,
                       const T_KPD_EnableDetection long_key_process_en,
                       const T_KPD_EnableDetection time_out_empty_en,
                       const T_KPD_EnableDetection time_out_long_key_en, 
                       const T_KPD_EnableDetection repeat_mode_en);

/**
 * function: kpd_ctrl_repeat_int
 *
 * Set or reset the repeat interrupt enable bit   
 */

void kpd_ctrl_repeat_int(const UINT8 state);

/**
 * function: kpd_software_reset
 *
 * Reset software  
 */
void kpd_software_reset(void);

/**
 * function: kpd_set_timeout_time
 *
 * Set timeout in timeout register  
 */
void kpd_set_timeout_time(const UINT16 timeout_time);

/**
 * function: kpd_map_reg_on_layout
 *
 * This function will map a key register bit
 * on to the keyboard layout
 *
 * @param     key   key register value
 * @return          layout / physical key
 */
T_KPD_PHYSICAL_KEY_ID kpd_map_reg_on_layout(UINT8 key);


#endif

/**
 * function: kpd_initialize_keypad_hardware
 *
 * Hardware initialization (Not implemented on Riviera tool).
 */
void kpd_initialize_keypad_hardware(void);

/**
 * function: kpd_acknowledge_key_pressed
 *
 * Unmask keypad interrupt (Not implemented on Riviera tool).
 */
void kpd_acknowledge_key_pressed(void);

/**
 * function: kpd_key_pressed
 *
 * This function send the message T_KPD_KEY_PRESSED_MSG with the value of key pressed.
 *
 * @param   value  Physical key identification.
 */
void kpd_key_pressed(UINT8 value);


/**
 * function: kpd_scan_keypad
 *
 * This function returns the physical key Id for the pressed key, or
 * KPD_KEY_NULL if no key is pressed.
 *
 * @return Physical key identification. 
 */
T_KPD_PHYSICAL_KEY_ID kpd_scan_keypad(void);

#if (KEYPAD_RF_ISSUE_SW_WORKAROUND == 1 || KEYPAD_RF_ISSUE_SW_WORKAROUND == 2)
   typedef UINT32 KPD_CORRECTION_RATIO;

   void kpd_state_probe(UINT32 frameNumber);

   /**
    * function: kpd_timer_modify
    *
    * This function decreases the timer register value
    * according to the ratio supplied.
    *
    * @param   ratio  Debounce value divider.
    */
   void kpd_timer_modify(KPD_CORRECTION_RATIO ratio, UINT32 frameNumber);
#endif   

#ifdef _WINDOWS

   /**
    * function: kpd_key_handler
    *
    * This function is called when a key is pressed (Interrupt process).
    *
    */
   void kpd_key_handler(void);


   /**
    * function: kpd_key_released
    *
    * This function send the message T_KPD_KEY_PRESSED_MSG with the value of key=KPD_PKEY_NULL.
    *
    */
   void kpd_key_released(void);

   /**
    * function: kpd_set_key_value
    *
    * This function set the key identification.
    *
    * @param   value  Physical key identification.
    */
   void kpd_set_key_value(T_KPD_PHYSICAL_KEY_ID value);

   #ifdef __cplusplus
      }
   #endif
#endif

/*@}*/
#endif /* #ifndef _KPD_SCAN_FUNCTIONS_H_ */
