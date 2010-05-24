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
/***** H A R D W A R E   R E G I S T E R   B I T   D E F I N I T I O N S *****/


#define CHIP_ID 	(unsigned char) 0x80
#define VERSION_ID 	(unsigned char) 0x81
#define SYS_CON 	(unsigned char) 0x02
#define ICR_MSB 	(unsigned char) 0X10
#define ICR_LSB 	(unsigned char) 0x11
#define IER_MSB 	(unsigned char) 0x12
#define IER_LSB 	(unsigned char) 0x13
#define ISR_MSB 	(unsigned char) 0X14
#define ISR_LSB 	(unsigned char) 0X15
#define IEGPIOR_MSB (unsigned char) 0X16
#define IEGPIOR_MID (unsigned char) 0X17
#define ISGPIOR_LSB (unsigned char) 0X18
#define ISGPIOR_MSB (unsigned char) 0X19
#define ISGPIOR_MID (unsigned char) 0X1A
#define ISGPIOR_LSB (unsigned char) 0X1B
#define GPMR_MSB 	(unsigned char) 0XA2
#define GPMR_LSB 	(unsigned char) 0XA4
#define GPSR_MSB 	(unsigned char) 0X83
#define GPSR_CSB 	(unsigned char) 0X84
#define GPSR_LSB 	(unsigned char) 0X85
#define GPCR_MSB 	(unsigned char) 0X86
#define GPCR_CSB 	(unsigned char) 0X87
#define GPCR_LSB 	(unsigned char) 0X88
#define GPDR_MSB 	(unsigned char) 0X89 /* Direction register */
#define GPDR_CSB 	(unsigned char) 0X8A
#define GPDR_LSB 	(unsigned char) 0X8B
#define GPEDR_MSB 	(unsigned char) 0X8C
#define GPEDR_CSB 	(unsigned char) 0X8D
#define GPEDR_LSB 	(unsigned char) 0X8E
#define GPRER_MSB 	(unsigned char) 0X8F
#define GPRER_CSB 	(unsigned char) 0X90
#define GPRER_LSB 	(unsigned char) 0X91
#define GPFER_MSB 	(unsigned char) 0X92
#define GPFER_CSB 	(unsigned char) 0X93
#define GPFER_LSB 	(unsigned char) 0X94
#define GPPUR_MSB 	(unsigned char) 0X95
#define GPPUR_CSB 	(unsigned char) 0X96
#define GPPUR_LSB 	(unsigned char) 0X97
#define GPPDR_MSB 	(unsigned char) 0X98
#define GPPDR_CSB 	(unsigned char) 0X99
#define GPPDR_LSB 	(unsigned char) 0X9A
#define GPAFR_U_MSB (unsigned char) 0X9B
#define GPAFR_U_CSB (unsigned char) 0X9C
#define GPAFR_U_LSB (unsigned char) 0X9D
#define GPAFR_L_MSB (unsigned char) 0X9E
#define GPAFR_L_CSB (unsigned char) 0X9F
#define GPAFR_L_LSB (unsigned char) 0XA0
#define MUX_CTRL 	(unsigned char) 0XA1
#define PWMCS 		(unsigned char) 0X30
#define PWMIC0 		(unsigned char) 0X38
#define PWMIC1 		(unsigned char) 0X39
#define PWMIC2 		(unsigned char) 0X3A
#define KPC_COL 	(unsigned char) 0x60
#define KPC_ROW_MSB (unsigned char) 0x61
#define KPC_ROW_LSB 	(unsigned char) 0x62
#define KPC_CTRL_MSB 	(unsigned char) 0x63
#define KPC_CTRL_LSB 	(unsigned char) 0x64
#define KPC_DATA_BYTE0 	(unsigned char) 0x68
#define KPC_DATA_BYTE1 	(unsigned char) 0x69
#define KPC_DATA_BYTE2 	(unsigned char) 0x6A
#define ROTATOR_CONTROL (unsigned char) 0x70
#define ROTATOR_BUFFER 	(unsigned char) 0x72

#define NORMAL_HOT_KEYPAD 			0x21
#define ROTATOR_KEY		  			0x22
#define PE_SLAVE1_REG_ACCESS 		0x2A
#define PE_SLAVE2_REG_ACCESS		0x2B
#define PE_SLAVE3_REG_ACCESS		0x2C
#define PE_SLAVE4_REG_ACCESS		0x2D
#define PROGRAM_PWM1 				0x31
#define PROGRAM_PWM2 				0x32	
#define PROGRAM_PWM3 				0x33
#define PROGRAM_PWM4 				0x34
#define PROGRAM_PWM5 				0x35
#define PROGRAM_PWM6				0x36
#define RESET_SYSTEM	 			0x41
#define SLEEP_SYSTEM	 			0x42
#define HIBERNATE_MODE	 			0x43
#define WAKE_UP_COMMAND 			0x44
#define USB_SETFREQ_SLAVE1 			0x37
#define USB_SETFREQ_SLAVE2			0x38
#define USB_SETFREQ_SLAVE3 			0x39
#define USB_SETFREQ_SLAVE4 			0x3A

extern  void kpd_full_sleep(void);
extern  T_RV_RET
read_commands_to_kpd (UINT8 * bytes_p, UINT8 * commands_p,
			  UINT16 size_of_bytes, BOOL vf_mode);

extern T_RV_RET
write_commands_to_kpd (UINT8 * bytes_p, UINT8 * commands_p,
			  UINT16 size_of_bytes, BOOL vf_mode);
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

    int kpd_full_require_read(void* p);
/*@}*/
#endif /* #ifndef _KPD_SCAN_FUNCTIONS_H_ */
