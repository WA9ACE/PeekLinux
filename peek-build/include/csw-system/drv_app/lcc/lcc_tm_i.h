/******************************************************************************
 * Power Task (pwr)
 * Design and coding by Svend Kristian Lindholm, skl@ti.com
 *
 * PWR ETM interface
 *
 * $Id: pwr_tm_i.h 1.1 Wed, 20 Aug 2003 10:22:37 +0200 skl $
 *
 ******************************************************************************/

#ifndef _LCC_TM_I_H_
#define _LCC_TM_I_H_

#define PWR_CFG_ID_SIZE      2
#define PWR_COMMON_CFG_SIZE 14
#define PWR_BAT_CFG_SIZE    36
#define PWR_TEMP_CFG_SIZE   72
#define PWR_MMI_CFG_SIZE     2
#define PWR_CHG_CFG_SIZE     8
#define PWR_I2V_CAL_SIZE     6
#define PWR_VBAT_CAL_SIZE    6
#define PWR_DYNAMIC_SIZE     1
#define PWR_TMASK_SIZE       4

// This enumeration should be shared with the PC test mode side
enum {
    PWR_CFG_ID = 0,
    PWR_COMMON,
    PWR_CHG,
    PWR_BAT,
    PWR_TEMP,
    PWR_MMI,
    PWR_I2V_CAL = 10,
    PWR_VBAT_CAL,
    PWR_MMI_TEST = 15,
    PWR_DYNAMIC = 20,
    PWR_TRACE_MASK= 21
} ;

// Test mode indication mail sent to ETM
// NOTE: Also used as request mail
struct etm_tm_ind_s {
     T_RV_HDR header;
     uint8 size;
     uint8 status;
     uint8 data[127];
};

typedef struct etm_tm_ind_s    T_ETM_TM_IND; // Use same buffer in both directions

	// Compiled in variables use if there is no configuration in FFS
	// Will ALWAYS be overwritten by settings in FFS - if any

	// common.cfg
	#define LCC_PINS 6
	#define LCC_CHG_DEDIC 0
	#define LCC_SAMPLING 10000
	#define LCC_MOD_CYCLE 10000
	#define LCC_ALFA1 60
	#define LCC_ALFA2 5
	#define LCC_ALFA3 10
	#define LCC_RISE_THR 0

	// bat1.cfg
	#define LCC_BAT_TYPE 1
	#define LCC_RF_TEMP 0
	#define LCC_BAT_ID_LOW 0
	#define LCC_BAT_ID_HIGH 1023
	#define LCC_CBAT 720
	#define LCC_RATIO 51
	#define LCC_T1 10800000
	#define LCC_T2 2600000
	#define LCC_T3 30000
	#define LCC_CV_CHG_START_THR 4100
	#define LCC_CV_CHG_STOP_THR 4200
	#define LCC_CHG_CTRL_THR 20
	#define LCC_CHG_AGAIN_THR 4000 //modify by sunming 081105

	//add by sunming for LCC_TEMP_PROTECT and Topping_Charge 081027
	#define LCC_HIGH_TEMP_STOP_THR 3900-50//more safe
	#define LCC_HIGH_TEMP_CHG_AGAIN_THR 3700
	// temp1.cfg
	#define LCC_TEMP_TBAT_MIN 0
	#define LCC_TEMP_TBAT_MAX 60
	#define LCC_TEMP_A0      0
	#define LCC_TEMP_A1      0
	#define LCC_TEMP_A2      0
	#define LCC_TEMP_CAP_100 4200
	#define LCC_TEMP_CAP_90 4150
	#define LCC_TEMP_CAP_80 4050
	#define LCC_TEMP_CAP_70 3900
	#define LCC_TEMP_CAP_60 3850
	#define LCC_TEMP_CAP_50 3800
	#define LCC_TEMP_CAP_40 3750
	#define LCC_TEMP_CAP_30 3650
	#define LCC_TEMP_CAP_20 3600
	#define LCC_TEMP_CAP_10 3420
	
	// add by zip,for BTC qwert battery level
	#define LCC_BATT_LEVEL_4 3970
	#define LCC_BATT_LEVEL_3 3835
	#define LCC_BATT_LEVEL_2 3748
	#define LCC_BATT_LEVEL_1 3684
	#define LCC_BATT_LEVEL_0 3400

	#define LCC_TEMP_I_MEAS1 10
	#define LCC_TEMP_I_MEAS2 30

	#define LCC_TEMP_V2T_1_MINUS_20  0x03ff
	#define LCC_TEMP_V2T_1_MINUS_10  0x02ed
	#define LCC_TEMP_V2T_1_0         0x01b1
	#define LCC_TEMP_V2T_1_PLUS_10   0x0104
	#define LCC_TEMP_V2T_1_PLUS_20   0x00a1
	#define LCC_TEMP_V2T_1_PLUS_30   0x0068
	#define LCC_TEMP_V2T_1_PLUS_40   0x0044
	#define LCC_TEMP_V2T_1_PLUS_50   0x002e
	#define LCC_TEMP_V2T_1_PLUS_60   0x0020

	#define LCC_TEMP_V2T_2_MINUS_20  0x03ff
	#define LCC_TEMP_V2T_2_MINUS_10  0x03ff
	#define LCC_TEMP_V2T_2_0         0x03ff
	#define LCC_TEMP_V2T_2_PLUS_10   0x030b
	#define LCC_TEMP_V2T_2_PLUS_20   0x01e4
	#define LCC_TEMP_V2T_2_PLUS_30   0x0137
	#define LCC_TEMP_V2T_2_PLUS_40   0x00cd
	#define LCC_TEMP_V2T_2_PLUS_50   0x008b
	#define LCC_TEMP_V2T_2_PLUS_60   0x0060

	
	// vbat.cal default value.
	#define LCC_ALFA_NUM    1750
	#define LCC_ALFA_DENOM   1023
	#define LCC_BETA         0

	// chg1.cfg
	// PWM: 3, CI_CV: 4
	#define LCC_CHG_TYPE 1
	#define LCC_ICHG_MAX 64
	#define LCC_VCHG_LOW 4725
	#define LCC_VCHG_HIGH 5725
	
	// add by zip,use for timer set
	#define LCC_TIMER_INI				10  // initialize state 
	#define LCC_TIMER_IDLE 				35  // use for idle state
	#define LCC_TIMER_LOW_POWER 		8   // use for low power state, < 3.7V
	#define LCC_TIMER_CI				10  // use for CI state
	#define LCC_TIMER_END_CI			5   // use for CI state, when > 4.15V
	#define LCC_TIMER_CV				5   // use for CV state,

#endif //_LCC_TM_I_H_
