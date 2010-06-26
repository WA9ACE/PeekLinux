	   /******************************************************************************
	    * Power Task (pwr)
	    * Design and coding by Svend Kristian Lindholm, skl@ti.com
	    *
	    * PWR Types and globals
	    *
	    * $Id: pwr.h 1.2 Wed, 20 Aug 2003 12:54:50 +0200 skl $
	    *
	    ******************************************************************************/
	   
	   #ifndef _LCC_H_
	   #define _LCC_H_
	   
	   #include "lcc/lcc_cfg.h"
	   
	   #ifdef _RVF
	   #include "rvf/rvf_api.h"
	   #include "rv/rv_general.h"
	   #include "lcc/lcc_api.h"
	   #include "lcc/lcc_cfg_i.h"
	   #endif
	   
	   #define PWRVERSION 0x3011
	   #define CHGPRES  1
	   #define CHGSTS   1
	   /******************************************************************************
	    * Types
	    ******************************************************************************/
	   
	   #ifndef BASIC_TYPES
	   #define BASIC_TYPES
	   typedef signed   char  int8;
	   typedef unsigned char  uint8;
	   typedef signed   short int16;
	   typedef unsigned short uint16;
	   typedef signed   int   int32;
	   typedef unsigned int   uint32;
	   #endif
	   
	   typedef int8   epwr_t;      // error type
	   
	   // pwr_req_s : Request mail
	   struct pwr_req_s {
	        T_RV_HDR header;
	   };
	   
	   // pwr_adc_ind_s : Indication mail with ADC measurements
	   struct pwr_adc_ind_s {
	        T_RV_HDR header;
	        unsigned short data[12]; // ADC measurements + status of VRPCSTS register
	   };
	   
	   //pwr_tm_req_s : Response mail sent to ETM
	   struct pwr_tm_req_s {
	        T_RV_HDR header;
	   };
	   
	   typedef epwr_t                T_PWR_RET;
	   typedef struct pwr_req_s      T_PWR_REQ;
	   
	   //T_PWR_I2V_CAL_BLOCK : a data type contains the battery charging current calibration parameters
	   typedef struct
	   {
	       uint16   alfa_num;     // Slope numerator
	       uint16   alfa_denom;   // Slope denominator
	       int16    beta;         // Offset
	   } T_PWR_I2V_CAL_BLOCK;
	   
	   //T_PWR_VBAT_CAL_BLOCK : a data type contains the battery voltage calibration parameters
	   typedef struct
	   {
	       uint16   alfa_num;     // Slope numerator
	       uint16   alfa_denom;   // Slope denominator
	       int16    beta;         // Offset
	   } T_PWR_VBAT_CAL_BLOCK;
	   
	   //T_PWR_CAL_BLOCK : a data type containing charging current and  charger voltage calibration parameters 
	   //Corresponds to /pwr/i2v.cal and /pwr/vbat.cal
	   typedef struct
	   {
	       T_PWR_I2V_CAL_BLOCK   i2v;
	       T_PWR_VBAT_CAL_BLOCK vbat;
	   } T_PWR_CAL_BLOCK;
	   
	   // T_PWR_COMMON_CFG_BLOCK : This structure holds some common data regarding the battery, 
	   //							   ADC sampling period, the pulse width modulation period for the CV state etc.
	   // Corresponds to /pwr/common.cfg
	   typedef struct
	   {
	       uint32   sampling;     /* 4 */ // The sampling time [ms] used
	       uint32   mod_cycle;    /* 4 */ // The modulation cycle time [ms] used when performing modulation (Lithium only)
	       uint8    pins;         /* 1 */ // Number of battery pins 2 - 3 - 4
	       uint8    chg_dedic;    /* 1 */ // Charging performed (TRUE) or not (FALSE) in dedicated mode
	       uint8    alfa1;        /* 1 */ // The regulation parameter N (Exponentially Weighted Moving Average) for Vbat
	       uint8    alfa2;        /* 1 */ // The regulation parameter N (Exponentially Weighted Moving Average) for Tbat
	       uint8    alfa3;        /* 1 */ // The regulation parameter N (Exponentially Weighted Moving Average) for Trf
	       uint8    rise_thr;     /* 1 */ // The battery rise temperature follows RF rise temperature threshold (Nickel only)
	   } T_PWR_COMMON_CFG_BLOCK;
	   
	   // T_PWR_BAT_CFG_BLOCK : This structure holds the battery configuration data
	   // Corresponds to /pwr/bat/bat<N>.cfg
	   typedef struct
	   {
	       uint16   type;          /* 2 */ // Battery type - Nickel or Lithium
	       uint16   rf_temp;       /* 2 */ // Use rf temperature (TRUE) or not (FALSE) if no battery temperature is availlable
	       uint32   id_low;        /* 4 */ // Low battery id limit - used for identification
	       uint32   id_high;       /* 4 */ // High battery id limit - used for identification
	       uint16   cbat;          /* 2 */ // Standard battery capacity [mAh]
	       uint16   ratio;         /* 2 */ // Time ratio of charge current [C] (%) - ratio x cbat = max charger current
	       uint32   T1;            /* 4 */ // The total time charging counter
	       uint32   T2;            /* 4 */ // The CV charging complete timer (-Nickel)
	       uint32   T3;            /* 4 */ // The re-charge attempt timer counter
	       uint16   chg_start_thr; /* 2 */ // The CV charge begin battery voltage threshold value (-Nickel)
	       uint16   chg_stop_thr;  /* 2 */ // The (CI and CV) charge end (stop) battery voltage threshold value
	       uint16   chg_ctrl_thr;  /* 2 */ // The minimal charge control ('k_min') value threshold
	       uint16   chg_again_thr; /* 2 */ // The charge again capacity threshold (t=20 degrees)
	   } T_PWR_BAT_CFG_BLOCK;
	   
	   // T_PWR_BAT_TEMP_CFG_BLOCK : This structure holds the data for battery voltage to the capacity mappings, 
	   //							     the battery thermister readings to the temperature mappings.
	   //							     The current source value for the thermister measurements etc.
	   // Corresponds to /pwr/bat/temp<N>.cfg
	   typedef struct
	   {
	       int16      tbat_min;  /* 2 */  // Minimum battery temperature allowed
	       int16      tbat_max;  /* 2*/  // Maximum battery temperature allowed
	   
	       uint16    a0;         /* 2 */  // Capacity temperature compensation a0 (a2*t*t + a1*t +a0)
	       uint16    a1;         /* 2 */  // Capacity temperature compensation a1 (a2*t*t + a1*t +a0)
	       uint16    a2;         /* 2 */  // Capacity temperature compensation a2 (a2*t*t + a1*t +a0)
	                             // Capacity compensation function relative to t=20 degrees
	                             // C(t=0) = C(t=20) + capacity compensation function(t=-20)
	   
	       uint16   cap[9+1];      /* 20 */ // Capacity as a function of voltage measured at t=20 degrees - C(t=20)
	                             // Capacity vector : 100%, 90%, 80%, 70% ,60% ,50%, 40%, 30%, 20%, 10%
	   
	       uint16     i_meas1;   /* 2 */  // The choosen measurement current, i_measure (typically 10uA and 50uA)
	       uint16     i_meas2;   /* 2 */  // The choosen measurement current, i_measure (typically 10uA and 50uA)
	       uint16     v2t_1[8+1];  /* 18 */ // The voltage to temperature mapper for i_measure
	       uint16     v2t_2[8+1];  /* 18 */ // The voltage to temperature mapper for i_measure
	                             // Temperature vector : -20, -10, 0, 10 ,20 ,30, 40, 50, 60
	                             // Interpolation is applied when mapping
	   
	   
	   } T_PWR_BAT_TEMP_CFG_BLOCK;
	   
	   // T_PWR_CHG_CFG_BLOCK : This structure holds the charger configuration supported
	   // Corresponds to /pwr/chg/chg<N>.cfg
	   typedef struct
	   {
	       uint16   type;       /* 2 */ // Charger type - Regulated (CI or CV) or unregulated (UNREGULATED)
	       uint16   ichg_max;   /* 2 */ // Maximum charger current [mA] - normally decides the fast charging current
	       uint16   vchg_low;   /* 2 */ // Low charger voltage limit  (Open circuit - used for identification )
	       uint16   vchg_high;  /* 2 */ // High charger voltage limit (Open circuit- used for identification)
	   } T_PWR_CHG_CFG_BLOCK;
	   
	   // T_PWR_DATA_BLOCK : 
	   // The power data block - dynamical data collected
	   typedef struct
	   {
	       uint16       bat_id;             // Measured battery ID
	       uint16       chg_id;             // Measured charger ID
	   
	       uint16       Vbat;               // Latest measured battery voltage
	       uint16       Vbat_avg;           // Latest _average_ battery voltage
	       uint16       Vbat_avg_mV;        // Latest _average_ battery voltage
	   
	       uint16       Tbat;               // Latest measured battery temperature
	       uint16       Tbat_avg;           // Latest _average_ battery temperature
	   
	       uint16       T_rf;               // Latest measured RF temperature
	       uint16       T_rf_avg;           // Latest _average_ RF temperature
	   
	       uint16       Vchg;               // Latest measured charger voltage
	       uint16	      USB_VBUS;	
	       uint16       Ichg;               // Latest measured charger current
	   
	       uint16       Cbat;               // Battery capacity in %
	   
	       char         cfg_id;             // Configuration ID (bat<n>.cfg)
	       char         chg_cfg_id;         // Charger Configuration ID (chg<n>.cfg)
	       char         bforce;             // Force usage of battery configuration
	       char         cforce;             // Force usage of charger configuration
	       uint16       k;                  // Modulation value
	       uint32       T4;                 // The modulation ON T4 timer counter (variable!!)
	   } T_PWR_DATA_BLOCK;
	   
	   // T_PWR_CTRL_BLOCK:
	   //The PWR control block - state and debug info
	   typedef struct
	   {
	       int      state;                  // The battery & charging state of the PWR task
	       uint8    flag_chg_plugged;       // Charger has been plugged (1) or not (0) flag
	       uint8	 flag_USB_plugged;
	       uint8    flag_chg_prev_plugged;  // The previous charger poll said plugged (1) or not plugged (0) flag
	       uint8    flag_USB_prev_plugged;
	       uint8    flag_chg_cfg_read;      // Charger configuration has been read (1) or not (0) flag
	       uint8    flag_cal_cfg_read;      // Calibration files has been read (1) or not (0) flag
	       uint8    flag_bat_unknown;       // Battery was unknown (1) - or not (0) - the MMI must be told
	       uint8    flag_chg_unknown;       // Charger plugged  was unknown (1) - or not (0) - the MMI must be told
	       uint8    flag_mmi_registered;    // MMI has registered (1) or not (0)
	       uint8    flag_prechg_started;    // Stay in precharge state PRE - Apply fast charging no matter what charger type
	       uint8    flag_chg_int_disabled;  // Charger interrupts are disabled (1) or enabled (0)
	       uint8    flag_ini_virgo;         // Entering INI state for the very first time - in order to control some very basic initialization
	       uint8    flag_topping_chg;       // Topping charge must be applied (1) or not (0)

	       uint8    cfg_id;                 // Applied battery configuration id
	       uint8    chg_cfg_id;             // Applied charger configuration id
	       uint8    count_bat_type;         // The number of battery identification measurements reported in state INI
	       uint8    count_chg_type;         // The number of charger identification measurements made in state SUP
	       uint8	 count_USB_type;
	       uint8    chg_unplug_vec[CONSECUTIVE_CHG_UNPLUGS];    // The charger unplug vector keeps the latest polled values of charger unplug
	       uint8    USB_unplug_vec[CONSECUTIVE_USB_UNPLUGS]; 
	       uint8    index;                  // The current index of the charger unplug vector
	       uint16   capacity;               // The current capacity : 0..100%
	   
	       // Timers
	       UINT32   time_begin_T0;          // T0: Sampling timer  (constant but modulo ADC updates)
	       UINT32   time_elapsed_T0;
	       UINT32   time_begin_T1;          // T1: The total charging time timer (constant)
	       UINT32   time_elapsed_T1;
	       UINT32   time_begin_T2;          // T2: The total CV charging time timer (constant)
	       UINT32   time_elapsed_T2;
	       UINT32   time_begin_T3;          // T3: The charge-again-without start-stop of charging (constant)
	       UINT32   time_elapsed_T3;
	       UINT32   time_begin_T4;          // T4: The duty cycle timer
	       UINT32   time_elapsed_T4;
	       UINT32   time_begin_mmi_rep;     // MMI repetition
	       UINT32   time_elapsed_mmi_rep;
	   
	       UINT32   tmask;                  // The trace mask used for the PWR process - activated by test mode
	   
	       T_RVF_ADDR_ID addr_id;           // Task address id assigned by Riviera framework
	       T_RVF_MB_ID   prim_id;           // Memory bank id assigned by Riviera framework
	   
	       T_RV_RETURN rpath;               // Return path of the MMI - initialized in pwr_register
	       struct mmi_info_ind_s *mmi_ptr;  // Pointer to mmi event memory assigned by the MMI
	   } T_PWR_CTRL_BLOCK;
	   
	   // T_PWR_CFG_BLOCK:
	   // The PWR configuration block - common, charger and battery
	   typedef struct
	   {
	       T_PWR_CAL_BLOCK cal;             // Calibration matrix
	       T_PWR_COMMON_CFG_BLOCK common;   // Common configuration
	       T_PWR_BAT_CFG_BLOCK bat;         // Battery configuration
	       T_PWR_BAT_TEMP_CFG_BLOCK temp;   // Battery temperature configuration
	       T_PWR_CHG_CFG_BLOCK chg;         // Charger configuration
	       T_PWR_MMI_CFG_BLOCK mmi;         // MMI configuration
	       T_PWR_DATA_BLOCK data;           // Data (dynamic) related to configuration
	   } T_PWR_CFG_BLOCK;
	   
	   // Use these values if no FFS files are found
	   // Units: [ms], [mV] and [mA]
	   #ifndef PWR_FFS_CFG
	       #define PWR_T0_THR            10000		// T0: Sampling timer  (constant but modulo ADC updates)
	       #define PWR_T1_THR            1800000   		// T1: The total charging time timer (constant)
	       #define PWR_T2_THR            25000      		// T2: The total CV charging time timer (constant)		
	       #define PWR_T3_THR            10000		// T3: The charge-again-without start-stop of charging (constant)
	       #define PWR_MOD_CYCLE_THR    10000	// The modulation cycle timer
	       #define PWR_MMI_REP_THR       60000		// MMI repetition timer
	       #define PWR_ICHG_MAX          400		//Maximum charging urrent 
	       #define EXP_MOVING_AVG_FILTER1 5	//Moving average filter coefficeient1
	       #define EXP_MOVING_AVG_FILTER2 10	//Moving average filter coefficeient2
	       #define EXP_MOVING_AVG_FILTER3 10	//Moving average filter coefficeient3
	       #define PWR_TEMPERATURE_LOW   -20	//Minimum allowable battery temperature 
	       #define PWR_TEMPERATURE_HIGH   45	//Maximum allowable battery temperature
	       #define PWR_CV_START_THR      4050		//CV charging start battery threshold
	       #define PWR_CHG_STOP_THR      4100		//Charging stop battery threshold
	       //#define PWR_RISE_THR          4			 	
	       #define PWR_MIN_K             20			//Minimum modulation value
	       #define PWR_MAX_K             200			//Maximum modulation value
	   #endif
	   
	   #define TYPEN 0x0080               /* Enables the 10uA bias current for the main battery type reading */
	   #define THERMAL_SENSOR_10uA 0x0041 /* THSENS0 , THSENS1, THSENS2 = 0, 0, 0  MESBAT = 1 */
	   #define THERMAL_SENSOR_30uA 0x0051 /* THSENS0 , THSENS1, THSENS2 = 0, 1, 0  MESBAT = 1 */
	   #define THERMAL_SENSOR_50uA 0x0061 /* THSENS0 , THSENS1, THSENS2 = 0, 0, 1  MESBAT = 1 */
	   #define THERMAL_SENSOR_80uA 0x0079 /* THSENS0 , THSENS1, THSENS2 = 1, 1, 1  MESBAT = 1 */
	   #define MESBAT              0x0001 /* Resistive divider connected to main battery */
	   #define CHARGER_IT_MSK      0x0008 /* Mask the charger plug/unplug interrupts */
	   #define ALL_IT_UMSK         0x0000 /* Unmask all interrupt 2 register related interrupts */
	   
	   // Convert functions
	   #define ADC_to_mV(ADC) ((pwr_cfg->cal.vbat.alfa_num*(ADC))/(pwr_cfg->cal.vbat.alfa_denom) + pwr_cfg->cal.vbat.beta)
	   #define ADC_to_mA(ADC) ((pwr_cfg->cal.i2v.alfa_num*(ADC))/(pwr_cfg->cal.i2v.alfa_denom) + pwr_cfg->cal.i2v.beta)
	   
	   // Inverse convert functions
	   #define mV_to_ADC(V) (pwr_cfg->cal.vbat.alfa_num*((V)/pwr_cfg->cal.vbat.alfa_denom) - pwr_cfg->cal.vbat.beta)
	   #define mA_to_ADC(I) (pwr_cfg->cal.i2v.alfa_num*((I)/pwr_cfg->cal.i2v.alfa_denom) - pwr_cfg->cal.i2v.beta)
	   
	   /******************************************************************************
	    * Errors
	    ******************************************************************************/
	   #if 0
	   enum PWR_ERRORS {
	       EPWR_OK          =  0  /* ok */
	   };
	   #endif 
	   /******************************************************************************
	    * Enumerations
	    ******************************************************************************/
	   
	   // Message IDs for all PWR module messages
	   typedef enum PWR_MESSAGES {
	       NOP = 0,
	   
	       /* Requests */
	           /* ABB */
	           /* MMI */
	       MMI_REGISTER_REQ         = 20,
	   
	       /* Timers*/
	       TIMER_T1_EXPIRED         = 30,
	       TIMER_T2_EXPIRED,
	       TIMER_T3_EXPIRED,
	       TIMER_T4_EXPIRED,
	       TIMER_MOD_CYCLE_EXPIRED,
	       TIMER_MMI_INFO_EXPIRED,
	   
	       /* Indications */
	       /* PWR */
	       PWR_CHARGER_PLUGGED_IND  = 40,
	       PWR_CHARGER_UNPLUGGED_IND,
	       PWR_USB_PLUGGED_IND,
	       PWR_USB_UNPLUGGED_IND,
	       
	       PWR_WD_OVERFLOW_IND,
	       PWR_CHARGE_END_IND,
	       /* Confirmations */
	   
	       /* Test mode */
	       /* Request */
	       PWR_TM_READ_REQ = 70,
	       PWR_TM_WRITE_REQ,
	   
	       /* Indications */
	       PWR_TM_READ_IND = 80,
	       PWR_TM_WRITE_IND,
	   
	       PWR_ADC_IND = 90
	   
	   } pwr_msg_e;
	   
	   // PWR_STATES :LCC FSM states
	   //See LCC Detailed Design Specification
	   typedef enum PWR_STATES {
	       CAL = 0, // Reading calibration file(s)
	       PRE = 1, // Pre-charging no matter which charger type
	       INI = 2, // Reading configuration files
	       SUP = 3, // No charging - supervision only
	       CCI = 4, // Charging using CI/CV charger
	       LCI = 5, // Charging using linear charger
	       CCV = 6, // Charging using CI/CV charger
	       LCV = 7  // Charging using linear charger
	   } pwr_states_e;
	   
	   // CHG_TYPES : charger types
	   typedef enum CHG_TYPES {
	       CI = 1,
	       CV = 2,
	       UNREGULATED = 3
	   } chg_types_e;
	   
	   // BAT_TYPES : battery types
	   typedef enum BAT_TYPES {
	       LITHIUM = 1,
	       NICKEL = 2
	   } bat_types_e;
	   
	   // CAUSE_TYPES : battery temperature status
	   typedef enum CAUSE_TYPES {
	       BATTERY_TEMPERATURE_OK   = 0,
	       BATTERY_TEMPERATURE_LOW  = 1,
	       BATTERY_TEMPERATURE_HIGH = 2,
	       CHARGER_VOLTAGE_HIGH     = 3,
    		CHARGER_CURRENT_HIGH     = 4
	   } cause_types_e;
	   
	   // PWR_Errors : Errors in LCC module 
	   enum PWR_Errors {
	       // PWR Common Target Errors
	       PWR_OK          =    0,  // Ok
	       PWR_INDEX       =  -1,   // Unknown index
	       PWR_OK_MORE     = -63,   // Ok, more data coming
	       PWR_PACKET      = -62,   // Packet error (checksum or other)
	       PWR_FATAL       = -61,   // System fatal error
	       PWR_NOSYS       = -60,   // Module or function not present
	       PWR_INVAL       = -59,   // Invalid parameter/argument
	       PWR_BADOP       = -58,   // Operation not possible in current context
	       PWR_AGAIN       = -57,   // Not ready, try again later
	       PWR_NOMEM       = -56,   // Out of memory
	       PWR_MESSAGE     = -55,   // Received unknown message
	   
	       // Errors that are related to the Riv env.
	       PWR_RV_NOT_SUPPORTED = -51, //
	       PWR_RV_MEMORY_ERR    = -50, //
	       PWR_RV_INTERNAL_ERR  = -49, //
	       PWR_RV_FATAL         = -48 //
	   
	   };
	   
	   //ChargingPathStatus : The charging path status
	   typedef enum
	   {
	   	AC_CHARGING_PATH_ENABLED=0,
	   	USB_CHARGING_PATH_ENABLED,
	   	NO_CHARGING_PATHS_ENABLED
	   }ChargingPathStatus;
	   
	   //MADC_INDEX : The MADC channel index.
	   typedef enum MADC_INDEX{
	       Vbat = 0,
	       Vchg = 1,
	       Ichg = 2,
	       Vbt2 = 3,
	       Type = 4,
	       Tbat = 5,
	       T_rf = 6,
	       Tc_x = 7,
	       Tc_y = 8,
	       State= 9,
	       Usb_state=10,
	       USB_VBUS=11
	   } adc_index_e;
	   
	   #endif //_LCC_H_
