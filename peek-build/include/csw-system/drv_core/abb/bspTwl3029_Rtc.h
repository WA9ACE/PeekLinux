/*=============================================================================
 *    Copyright 2003 Texas Instruments Inc. All rights reserved.
 */

/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspTwl3029_Rtc
 *   This is a framework agnostic RTC driver.
 */
 
#ifndef _BSPTWL3029_RTC_H_
#define _BSPTWL3029_RTC_H_


/* ============ Register: RTC TC UPDATE ==================
 */
enum
{
    BSP_TWL3029_RTC_TC_UPDATE_SECONDS = 1,
    BSP_TWL3029_RTC_TC_UPDATE_MINUTES = 2,
    BSP_TWL3029_RTC_TC_UPDATE_HOURS   = 3,
    BSP_TWL3029_RTC_TC_UPDATE_DAYS    = 4,
    BSP_TWL3029_RTC_TC_UPDATE_MONTHS  = 5,
    BSP_TWL3029_RTC_TC_UPDATE_YEARS   = 6,
    BSP_TWL3029_RTC_TC_UPDATE_WEEKS   = 7,
    BSP_TWL3029_RTC_TC_UPDATE_ALL     = 8
};
typedef Uint8 BspRtc_Llif_UpdateTc;

/* ============ Register: SECONDS ==================
 * ============ Register: ALARM_SECONDS ============
 * ============ Register: MINUTES ==================
 * ============ Register: ALARM_MINUTES ============
 * ============ Register: HOURS ====================
 * ============ Register: ALARM_HOURS ==============
 * ============ Register: DAYS =====================
 * ============ Register: ALARM_DAYS ===============
 * ============ Register: MONTHS ===================
 * ============ Register: ALARM_MONTHS =============
 * ============ Register: YEARS ====================
 * ============ Register: ALARM_YEARS ==============
 */
enum
{
    BSP_TWL3029_RTC_DIGIT_0    = (0),
    BSP_TWL3029_RTC_DIGIT_1    = (1),
    BSP_TWL3029_RTC_DIGIT_2    = (2),
    BSP_TWL3029_RTC_DIGIT_3    = (3),
    BSP_TWL3029_RTC_DIGIT_4    = (4),
    BSP_TWL3029_RTC_DIGIT_5    = (5),
    BSP_TWL3029_RTC_DIGIT_6    = (6),
    BSP_TWL3029_RTC_DIGIT_7    = (7),
    BSP_TWL3029_RTC_DIGIT_8    = (8),
    BSP_TWL3029_RTC_DIGIT_9    = (9)
};
typedef Uint8 BspRtc_Llif_Digit;

/* ============ Register: HOURS ====================
 */
enum
{
    BSP_TWL3029_RTC_HOURS_MERIDIAN_ANTES_AM = (0),
    BSP_TWL3029_RTC_HOURS_MERIDIAN_POST_PM  = (1)
};

/* ============ Register: WEEKDAY ====================
 */
/*=============================================================================*/
/*!
 * @typedef BspTwl3029_Rtc_Weekday
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This is an enumeration of the days of the week.
 */
enum
{
    BSP_TWL3029_RTC_WEEKDAY_SUNDAY    = (0x00),
    BSP_TWL3029_RTC_WEEKDAY_MONDAY    = (0x01),
    BSP_TWL3029_RTC_WEEKDAY_TUESDAY   = (0x02),
    BSP_TWL3029_RTC_WEEKDAY_WEDNESDAY = (0x03),
    BSP_TWL3029_RTC_WEEKDAY_THURSDAY  = (0x04),
    BSP_TWL3029_RTC_WEEKDAY_FRIDAY    = (0x05),
    BSP_TWL3029_RTC_WEEKDAY_SATURDAY  = (0x06)
};
typedef Uint8 BspTwl3029_Rtc_Weekday;



/* ============ Register: CTRL ====================
 */
enum
{
    BSP_TWL3029_RTC_CTRL_STOP_RTC_FREEZE = (0),
    BSP_TWL3029_RTC_CTRL_STOP_RTC_RUN    = (1)
};

enum
{
    BSP_TWL3029_RTC_CTRL_ROUND_30S_NO_UPDATE = (0),
    BSP_TWL3029_RTC_CTRL_ROUND_30S_UPDATE    = (1)
};

enum
{
    BSP_TWL3029_RTC_CTRL_AUTO_COMP_DISABLED = (0),
    BSP_TWL3029_RTC_CTRL_AUTO_COMP_ENABLED  = (1)
};

enum
{
    BSP_TWL3029_RTC_CTRL_MODE_24 = (0),
    BSP_TWL3029_RTC_CTRL_MODE_12 = (1)
};

enum
{
    BSP_TWL3029_RTC_CTRL_TEST_MODE_DISABLED = (0),
    BSP_TWL3029_RTC_CTRL_TEST_MODE_ENABLED  = (1)
};

enum
{
    BSP_TWL3029_RTC_CTRL_SET_COUNTER_DISABLED = (0),
    BSP_TWL3029_RTC_CTRL_SET_COUNTER_ENABLED  = (1)
};

/* ============ Register: INTERRUPTS ====================
 */
enum
{
    BSP_TWL3029_RTC_INTERRUPTS_EVERY_SECOND = (0),
    BSP_TWL3029_RTC_INTERRUPTS_EVERY_MINUTE = (1),
    BSP_TWL3029_RTC_INTERRUPTS_EVERY_HOUR   = (2),
    BSP_TWL3029_RTC_INTERRUPTS_EVERY_DAY    = (3)
};

enum
{
    BSP_TWL3029_RTC_INTERRUPTS_IT_TIMER_DISABLED = (0),
    BSP_TWL3029_RTC_INTERRUPTS_IT_TIMER_ENABLED  = (1)
};

enum
{
    BSP_TWL3029_RTC_INTERRUPTS_IT_ALARM_DISABLED = (0),
    BSP_TWL3029_RTC_INTERRUPTS_IT_ALARM_ENABLED  = (1)
};


/* ============ Register: STATUS ====================
 */
enum
{
    BSP_TWL3029_RTC_STATUS_BUSY_NOT_BUSY = (0),
    BSP_TWL3029_RTC_STATUS_BUSY_BUSY     = (1)
};

enum
{
    BSP_TWL3029_RTC_STATUS_RUN_RTC_FROZEN  = (0),
    BSP_TWL3029_RTC_STATUS_RUN_RTC_RUNNING = (1)
};

enum
{
    BSP_TWL3029_RTC_STATUS_EVENT_NOT_OCCURED = (0),
    BSP_TWL3029_RTC_STATUS_EVENT_OCCURED     = (1)
};

enum
{
    BSP_TWL3029_RTC_STATUS_POWER_UP_NO_RESET = (0),
    BSP_TWL3029_RTC_STATUS_POWER_UP_RESET    = (1)
};



/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspTwl3029_Rtc
 *   Public interface to the Real-Time Clock device.
 * 
 *   This device must be calibrated at least once against the 32KHz oscillator.
 *   The format for hours is always 24 hours, and the default calibration 
 *   compensation number is 0. 
 */

/*=============================================================================*/
/*!
 * @typedef BspTwl3029_Rtc_Second
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This is a type used to hold seconds. Range is 0 to 59.
 */
typedef Uint8 BspTwl3029_Rtc_Second;

/*=============================================================================*/
/*!
 * @typedef BspTwl3029_Rtc_Minute
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This is a type used to hold minutes. Range is 0 to 59.
 */
typedef Uint8 BspTwl3029_Rtc_Minute;

/*=============================================================================*/
/*!
 * @typedef BspTwl3029_Rtc_Hour
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This is a type used to hold hours. Range is 0 to 24.
 */
typedef Uint8 BspTwl3029_Rtc_Hour;

/*=============================================================================*/
/*!
 * @typedef BspTwl3029_Rtc_Day
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This is a type used to hold days. Range is 1 to 31.
 */
typedef Uint8 BspTwl3029_Rtc_Day;

/*=============================================================================*/
/*!
 * @typedef BspTwl3029_Rtc_Month
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This is a type used to hold months. Range is 1 to 12.
 */
enum
{
    BSP_TWL3029_RTC_MONTH_JANUARY   = (0x01),
    BSP_TWL3029_RTC_MONTH_FEBRUARY  = (0x02),
    BSP_TWL3029_RTC_MONTH_MARCH     = (0x03),
    BSP_TWL3029_RTC_MONTH_APRIL     = (0x04),
    BSP_TWL3029_RTC_MONTH_MAY       = (0x05),
    BSP_TWL3029_RTC_MONTH_JUNE      = (0x06),
    BSP_TWL3029_RTC_MONTH_JULY      = (0x07),
    BSP_TWL3029_RTC_MONTH_AUGUST    = (0x08),
    BSP_TWL3029_RTC_MONTH_SEPTEMBER = (0x09),
    BSP_TWL3029_RTC_MONTH_OCTOBER   = (0x0A),
    BSP_TWL3029_RTC_MONTH_NOVEMBER  = (0x0B),
    BSP_TWL3029_RTC_MONTH_DECEMBER  = (0x0C)
};
typedef Uint8 BspTwl3029_Rtc_Month;

/*=============================================================================*/
/*!
 * @typedef BspTwl3029_Rtc_Year
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This is a type used to hold years. Range is 0 to 99.
 */
typedef Uint8 BspTwl3029_Rtc_Year;

/*=============================================================================*/
/*!
 * @typedef BspTwl3029_Rtc_Weekday
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This is an enumeration of the days of the week.
 */
/*enum
{
    BSP_TWL3029_RTC_WEEKDAY_SUNDAY    = (0x00),
    BSP_TWL3029_RTC_WEEKDAY_MONDAY    = (0x01),
    BSP_TWL3029_RTC_WEEKDAY_TUESDAY   = (0x02),
    BSP_TWL3029_RTC_WEEKDAY_WEDNESDAY = (0x03),
    BSP_TWL3029_RTC_WEEKDAY_THURSDAY  = (0x04),
    BSP_TWL3029_RTC_WEEKDAY_FRIDAY    = (0x05),
    BSP_TWL3029_RTC_WEEKDAY_SATURDAY  = (0x06)
};
typedef Uint8 BspTwl3029_Rtc_Weekday;
*/

/*===========================================================================*/
/*!
 * @typedef bspTwl3029_Rtc_HourMode
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the hour mode.
 */
enum
{
   BSPTWL3029_RTC_12_HOURMODE = 0,
   BSPTWL3029_RTC_24_HOURMODE  = 1
};

typedef Uint8 BspTwl3029_Rtc_HourMode;

/*===========================================================================*/
/*!
 * @typedef bspTwl3029_Rtc_HourType
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the hour type.
 */
enum 
{
   BSPTWL3029_RTC_AM_HOURTYPE = 0,
   BSPTWL3029_RTC_PM_HOURTYPE = 1
};

typedef Uint8 BspTwl3029_Rtc_HourType;
/*=============================================================================*/
/*!
 * @typedef BspTwl3029_Rtc_Time
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This is a structure used to pass a time value to and from the driver.
 * 
 * @field second
 *            This is the absolute second.
 * 
 * @field minute
 *            This is the absolute minute.
 * 
 * @field hour
 *            This is the absolute hour 0 - 24.
 * 
 * @field day
 *            This is the absolute day 0 - 31.
 * 
 * @field month
 *            This is the enumerated month 1 - 12.
 * 
 * @field year
 *            This is the last 2 digits of the year.
 * 
 * @field weekday
 *            This is the enumerated weekday. Note: this isn't used when setting
 *            an Alarm.
 * * @field pm_am
 *            In AM_PM mode 1 => PM, 0=> AM
 */
typedef struct{
    BspTwl3029_Rtc_Second   second;
    BspTwl3029_Rtc_Minute   minute;
    BspTwl3029_Rtc_Hour     hour;
    BspTwl3029_Rtc_Day      day;
    BspTwl3029_Rtc_Month    month;
    BspTwl3029_Rtc_Year     year;
    BspTwl3029_Rtc_Weekday  weekday;
    BspTwl3029_Rtc_HourMode hourMode; /* hour mode - [0-1] - [12-24]*/
    BspTwl3029_Rtc_HourType hourType; /* AM or PM [0-1] [AM-PM] */
} BspTwl3029_Rtc_Time;

/*=============================================================================*/
/*!
 * @typedef BspTwl3029_Rtc_CalibrationValue
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This is a number of 1/32768 second units to add or subtract from time
 *     per hour. Range -32767 to +32767. 
 */
typedef Int16 BspTwl3029_Rtc_CalibrationValue;



/*=============================================================================*/
/*!
 * @typedef BspTwl3029_Rtc_EventCallback
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This is the type fo rthe function pointer passed in as an alarm callback.
 * 
 * @param time
 *            This is the time at wich the callback function is called.
 */
typedef void (* BspTwl3029_Rtc_EventCallback)( void );





/*=============================================================================
 *   Constant Definitions
 *=============================================================================*/

/* Register Mapping */
#define BSP_TWL3029_RTC_SECONDS_REG		0x00
#define BSP_TWL3029_RTC_MINUTES_REG		0x01
#define BSP_TWL3029_RTC_HOURS_REG		0x02
#define BSP_TWL3029_RTC_DAYS_REG		0x03
#define BSP_TWL3029_RTC_MONTHS_REG		0x04
#define BSP_TWL3029_RTC_YEARS_REG		0x05
#define BSP_TWL3029_RTC_WEEK_REG		0x06

#define BSP_TWL3029_RTC_ALARM_SECONDS_REG	0x08
#define BSP_TWL3029_RTC_ALARM_MINUTES_REG	0x09
#define BSP_TWL3029_RTC_ALARM_HOURS_REG		0x0a
#define BSP_TWL3029_RTC_ALARM_DAYS_REG		0x0b
#define BSP_TWL3029_RTC_ALARM_MONTHS_REG	0x0c
#define BSP_TWL3029_RTC_ALARM_YEARS_REG		0x0d

#define BSP_TWL3029_RTC_CTRL_REG		0x10
#define BSP_TWL3029_RTC_STATUS_REG		0x11
#define BSP_TWL3029_RTC_INTERRUPTS_REG  	0x12
#define BSP_TWL3029_RTC_COMP_LSB_REG    	0x13
#define BSP_TWL3029_RTC_COMP_MSB_REG		0x14

/* RTC control register description */
/* 1 => RTC is running */
#define BSP_TWL3029_RTC_START_RTC		0x0001	
/* Time rounded to the closest minute */
#define BSP_TWL3029_RTC_ROUND_30S		0x0002	
/* Auto compensation enabled or not */
#define BSP_TWL3029_RTC_AUTO_COMP		0x0004	
/* 12 hours mode*/
#define BSP_TWL3029_RTC_MODE_12_24		0x0008	
/* Test mode */
#define BSP_TWL3029_RTC_TEST_MODE		0x0010	
/* set 32 KHz counter with comp_reg */
#define BSP_TWL3029_RTC_SET_32_COUNTER		0x0020 	

/* RTC Interrupt register description */

/* Define period of periodic interrupt(second, minute, hour, day) */
#define BSP_TWL3029_RTC_EVERY				0x0003		
/* Enable periodic interrupt */
#define BSP_TWL3029_RTC_IT_TIMER			0x0004		
/* Alarm interrupt enabled or not */
#define BSP_TWL3029_RTC_IT_ALARM			0x0008			

/* RTC Status register description */

#define BSP_TWL3029_RTC_BUSY				0x0001
/* RTC is running */
#define BSP_TWL3029_RTC_RUN				0x0002
/* One second has occured */
#define BSP_TWL3029_RTC_1S_EVENT			0x0004		
/* One minute has occured */
#define BSP_TWL3029_RTC_1M_EVENT 			0x0008		
/* One hour has occured */
#define BSP_TWL3029_RTC_1H_EVENT			0x0010		
/* One day has occrued */
#define BSP_TWL3029_RTC_1D_EVENT			0x0020		
/* Alarm interrupt has been generated */
#define BSP_TWL3029_RTC_ALARM				0x0040		
/* Indicates that a reset occured */
#define BSP_TWL3029_RTC_POWER_UP			0x0080		

#define BSP_TWL3029_RTC_EVERY_SEC			0x0000
#define BSP_TWL3029_RTC_EVERY_MIN  		0x0001
#define BSP_TWL3029_RTC_EVERY_HR			0x0002
#define BSP_TWL3029_RTC_EVERY_DAY		0x0003

/* Clock frequency */
#define RTC_CLOCK_32K 	32768.0

/* High Frequency clock */
#define RTC_CLOCK_HF 	13000000.0

/*=============================================================================
 *   Type Definitions
 *=============================================================================*/

/*=============================================================================*/
/*!
 * @typedef bspTwl3029_Rtc_DateTime
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This is a type used to hold date and time. 
 *
 * @field second
 *            This is the absolute second - [0,59].
 * 
 * @field minute
 *            This is the absolute minute  - [0,59].
 * 
 * @field hour
 *            This is the absolute hour - [0,23].
 * 
 * @field day
 *            This is the absolute day - [1,31].
 * 
 * @field month
 *            This is the enumerated month  - [01,12].
 * 
 * @field year
 *            This is the last 2 digits of the year - [00,99].
 * 
 * @field weekday
 *            This is the enumerated weekday. Note: this isn't used when setting
 *            an Alarm - [0-6].
 *     
 */

typedef struct {  Uint8   second; /* seconds after the minute   - [0,59]  */
                  Uint8   minute; /* minutes after the hour      - [0,59]  */
                  Uint8   hour;   /* hours after the midnight   - [0,23]  */
                  Uint8   day;    /* day of the month            - [1,31]  */
                  Uint8   month;  /* months                     - [01,12] */
                  Uint8   year;   /* years                        - [00,99] */
                  Uint8   wday;   /* day of the week - [0-6]*/
		    Uint8   hourMode; /* hour mode - [0-1] - [12-24]*/
		    Uint8   hourType; /* AM or PM [0-1] [AM-PM] */
		    } bspTwl3029_Rtc_DateTime;

/*=============================================================================
 *   Function Prototypes
 *=============================================================================*/

/*=============================================================================*/
/*!
 * @typedef bspTwl3029_Rtc_Callback
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This is the type fo rthe function pointer passed in as an alarm callback.
 */
typedef void (* bspTwl3029_Rtc_Callback)( void );

/*=============================================================================
 * Public Functions
 */

/*===========================================================================
 */
/*!
 * @function bspTwl3029_Rtc_init
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This function initializes the RTC to a known normal working mode. No
 *    alarms are set or stored. Minute periodic timer is enabled after this
 *    function is called. The default mode is not to compensate with a
 *    calibration value, 24 hour time, and testMode disabled. All access to
 *    the RTC registers happen through I2C driver. <br>
 * 
 * <b> Context  </b><br>
 *    This function can be called at any time after the initialization of
 *    I2C drivers,but it must be called once at powerup before any other 
 *    RTC device driver calls.
 * 
 * @param callbackFunction
 *    This is a call back function that is called when an alarm events occurs.
 *    
 * @result
 */

BspTwl3029_ReturnCode
bspTwl3029_Rtc_init (void);

/*===========================================================================
 */
/*!
 * @function bspTwl3029_Rtc_calibrationSet
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function sets up the new calibration value and also enables the
 *   auto-calibration mechanism that is disabled at initialization.
 *   Note: The BSP_RTC hardware has 2 8-bit registers to hold the calibration value
 *         The value needs to be stored as 2's complement.
 *
 *   @param callbackFuncPtr
 *         pointer to a struct containing funtion pointer and other info used
 *         for I2C end of transaction callback (if required). Calling function
 *         can set this to NULL if no callback routine required
 *
 *   @param calibValue
 *            This is the number of (1/32768) seconds to add or remove from
 *            the real-time clock per hour.
 * 
 *   @result
 *      The RTC will begin compensating by the calibrationValue every hour.
 * 
 *  @return   BspTwl3029_ReturnCode       
 *    
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_calibrationSet( BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                               BspTwl3029_Rtc_CalibrationValue calibValue );
			       
/*===========================================================================
 */
/*!
 * @function bspTwl3029_Rtc_calibrationGet
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This function returns the current calibration value as a 1's complement
 *    integer <br>
 * 
 * <b> Context </b><br>
 *    This function can be called at any time.
 * 
 * @result
 *      The RTC will read out the current calibration compensation value and
 *      convert it to 1's complement before returning it.
 * 
 * @return BspTwl3029_ReturnCode
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_calibrationGet( BspTwl3029_Rtc_CalibrationValue* calibVal );

/*===========================================================================
 */
/*!
 * @function bspTwl3029_Rtc_timeSet
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This function sets the current time/date/weekday. This function 
 *     checks the complete validity of the time.
 *     For example one could set the time to 24:59:59, but not 25:59:59.
 *     One could also set the date ot wednesday february 29th '04 without
 *     error, but wednesday february 29th '03 would fail because february
 *     has only 28 days on non-leap years.
 *     Note: A delay of up to 15us might be intruduced within this function
 *           if the hardware is busy (in the process of incrementing the
 *           current time)
 * 
 * <b> Context  </b><br>
 *     This can be called at any time.
 *
 *  @param callbackFuncPtr
 *         pointer to a struct containing funtion pointer and other info used
 *         for I2C end of transaction callback (if required). Calling function
 *         can set this to NULL if no callback routine required
 * 
 * @param timePtr
 *            This is a pointer to a time structure that will be used to
 *            set the current time/date/weekday.
 * 
 * @result
 *      The current time/date will be set to the passed in value.
 * 
 * @return BspTwl3029_ReturnCode
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_timeSet( BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                        BspTwl3029_Rtc_Time *timePtr );

/*===========================================================================
 */
/*!
 * @function bspTwl3029_Rtc_timeGet
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This function retreives the current time/date/weekday information.
 * 
 * <b> Context  </b><br>
 *     This can be called at any time.
 *
 *  @param callbackFuncPtr
 *         pointer to a struct containing funtion pointer and other info used
 *         for I2C end of transaction callback (if required). Calling function
 *         can set this to NULL if no callback routine required
 * 
 * @param timePtr
 *            This is a pointer to the return location for the time
 *            information.
 * 
 * @result
 *      The current time/date is read and returned.
 * 
 * @return BspTwl3029_ReturnCode
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_timeGet( BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                        BspTwl3029_Rtc_Time *timePtr );
			
/*===========================================================================
 */
/*!
 * @function bspTwl3029_Rtc_alarmSet
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This function sets the current alarm time/date. Note: This is a one
 *     shot alarm. Subsequent calls of this funciton will overwrite previously
 *     set alarms.
 *     Note: A delay of up to 15us might be intruduced within this function
 *           if the hardware is busy (in the process of incrementing the
 *           current time)
 * 
 * <b> Context  </b><br>
 *     This function can be called at any time.
 *
 *  @param callbackFuncPtr
 *         pointer to a struct containing funtion pointer and other info used
 *         for I2C end of transaction callback (if required). Calling function
 *         can set this to NULL if no callback routine required
 * 
 * @param alarmCallbackFunctionPtr
 *            This is a pointer to a function that will be called by the RTC
 *            driver when the alarm expires. If this is NULL then no action
 *            is taken
 * 
 * @param timePtr
 *            This is an absolute time for the alarm to expire. If the time
 *            is invalid then no action is taken.
 * 
 * @result
 *      When the alarm occurs then the callback function will be called passing
 *      in the set alarm time. 
 * 
 * @return BspTwl3029_ReturnCode
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_alarmSet( BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                         BspTwl3029_Rtc_EventCallback alarmCallbackFunctionPtr,
                         BspTwl3029_Rtc_Time          *timePtr );


/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_alarmGet
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This gets the date and time from RTC Alarm registers.
 *    
 * @param dateTime
 *    This returns alarm date and time that is read from RTC alarm registers.
 *    
 * @result
 */
BspTwl3029_ReturnCode 
bspTwl3029_Rtc_alarmGet (BspTwl3029_Rtc_Time* dateTime);			 
/*===========================================================================
 */
/*!
 * @function bspTwl3029_Rtc_alarmClear
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This function clears the currently set alarm.
 *     Note: A delay of up to 15us might be intruduced within this function
 *           if the hardware is busy (in the process of incrementing the
 *           current time)
 * 
 * <b> Context  </b><br>
 * 
 *  @param callbackFuncPtr
 *         pointer to a struct containing funtion pointer and other info used
 *         for I2C end of transaction callback (if required). Calling function
 *         can set this to NULL if no callback routine required
 * 
 * @result
 *      The RTC will disable the currently set alarm.
 * 
 * @return BspTwl3029_ReturnCode
 */
BspTwl3029_ReturnCode
bspTwl3027_Rtc_alarmClear( BspTwl3029_I2C_CallbackPtr callbackFuncPtr );


/*==============================================================================*/
/*!
 * @function bspTwl3029_Rtc_timerIntSet
 *
 * @discussion
 * <b> Description  </b><br>
 *    sets up and enables the interrupt for the periodic timer.
 *
 * <b> Context  </b><br>
 * 
 *  @param callbackFuncPtr
 *         pointer to a struct containing funtion pointer and other info used
 *         for I2C end of transaction callback (if required). Calling function
 *         can set this to NULL if no callback routine required
 * 
 *  @param timerCallbackFunctionPtr
 *        pointer to interrrupt routine to be called on periodic timer
 *         interrrupt
 *
 *  @param period 
 *        interrupt period ( every second/min/hr or day)
 * @result
 *      The RTC will set  the periodic interrupt
 * 
 * @return BspTwl3029_ReturnCode
 */
 
BspTwl3029_ReturnCode
bspTwl3029_Rtc_timerIntEnable( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                            BspTwl3029_Rtc_EventCallback timerCallbackFunctionPtr,
                            Uint8   period );
/*==============================================================================*/
/*!
 * @function bspTwl3029_Rtc_timerIntDisable
 *
 * @discussion
 * <b> Description  </b><br>
 *    disables the interrupt for the periodic timer.
 *
 * <b> Context  </b><br>
 * 
 *  @param callbackFuncPtr
 *         pointer to a struct containing funtion pointer and other info used
 *         for I2C end of transaction callback (if required). Calling function
 *         can set this to NULL if no callback routine required
 * @result
 *      The RTC will disable  the periodic interrupt
 * 
 * @return BspTwl3029_ReturnCode
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_timerIntDisable( BspTwl3029_I2C_CallbackPtr callbackInfoPtr);


/*==============================================================================*/
/*!
 * @function bspTwl3029_Rtc_timerMinuteCallback
 *
 * @discussion
 * <b> Description  </b><br>
 *    Minute timer interrupt handler.
 *
 * <b> Context  </b><br>
 *     called during triton periodic timer ISR
 * 
 *  @param none
 *
 * @result
 *      
 * 
 * @return none
 */

 void bspTwl3029_Rtc_timerMinuteCallback (void)  ;			    			 						
////
/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_Rounding30s
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This rounds-off the time to the nearest minute.
 *    
 * @result
 */
BspTwl3029_ReturnCode 
bspTwl3029_Rtc_rounding30s(BspTwl3029_I2C_CallbackPtr callbackInfoPtr);
/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_isReset
 * 
 * @discussion
 * <b> Description  </b><br>
 *    Indicate if aRTC reset occured. <br>
 *    
 * @result
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_isReset( BspTwl3029_I2C_CallbackPtr callbackInfoPtr, Uint8 * resetStatus);
/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_Set12HourMode
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This sets the hour mode in RTC registers [12 hour or 24 hour].
 * 
 *  @param callbackFuncPtr
 *         pointer to a struct containing funtion pointer and other info used
 *         for I2C end of transaction callback (if required). Calling function
 *         can set this to NULL if no callback routine required
 *
 * @param mode12Hour
 *    This gives the hour mode to be set.    
 * 
 * @result  
 * 
 * @return BspTwl3029_ReturnCode
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_set12HourMode(BspTwl3029_I2C_CallbackPtr callbackInfoPtr, 
                             BspTwl3029_Rtc_HourMode mode12Hour);

/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_Is12HourMode
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This gets the current hour mode set in RTC registers [12 hour or 24 hour].
 *    
 * @result
 *
 * @return BSPTWL3029_RTC_12_HOURMODE or BSPTWL3029_RTC_24_HOURMODE  
 */
Uint8 bspTwl3029_Rtc_Is12HourMode(void);

////////////
// LOCOSTO function
/*===========================================================================
 */
/*!
 * @function bspTwl3029_Rtc_Init
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This function initializes the RTC to a known normal working mode. No
 *    alarms are set or stored. Minute periodic timer is enabled after this
 *    function is called. The default mode is not to compensate with a
 *    calibration value, 24 hour time, and testMode disabled. All access to
 *    the RTC registers happen through I2C driver. <br>
 * 
 * <b> Context  </b><br>
 *    This function can be called at any time after the initialization of
 *    I2C drivers,but it must be called once at powerup before any other 
 *    RTC device driver calls.
 * 
 * @param callbackFunction
 *    This is a call back function that is called when an alarm events occurs.
 *    
 * @result
 */

Int8 bspTwl3029_Rtc_Initialize (void);

/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_IsReset
 * 
 * @discussion
 * <b> Description  </b><br>
 *    Indicate if aRTC reset occured. <br>
 *    
 * @result
 */
Uint8 bspTwl3029_Rtc_IsReset ();
/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_GetDateTime
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This gets the date and time from RTC registers and gives it in BCD format.
 *    
 * @param dateTime
 *    This returns date and time that is read from RTC registers.
 *    
 * @result
 */
Int8 bspTwl3029_Rtc_GetDateTime (bspTwl3029_Rtc_DateTime* dateTime);

/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_SetDateTime
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This sets the date and time into RTC registers.
 *    
 * @param dateTime
 *    This contains date and time to set into RTC registers.
 *    
 * @result
 */
Int8 bspTwl3029_Rtc_SetDateTime (bspTwl3029_Rtc_DateTime dateTime);

/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_GetAlarm
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This gets the date and time from RTC Alarm registers.
 *    
 * @param dateTime
 *    This returns alarm date and time that is read from RTC alarm registers.
 *    
 * @result
 */
Int8 bspTwl3029_Rtc_GetAlarm (bspTwl3029_Rtc_DateTime* dateTime);

/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_SetAlarm
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This sets the alarm date and time into RTC Alarm registers and enables the
 *    alarm interrupt.
 *    
 * @param dateTime
 *    This contains alarm date and time that is set into RTC alarm registers.
 *    
 * @param callbackFunction
 *    This is the alarm callback function that is called when alarm event happens.
 *
 * @result
 */
Int8 bspTwl3029_Rtc_SetAlarm (bspTwl3029_Rtc_DateTime dateTime, 
                               bspTwl3029_Rtc_Callback callbackFunction);

/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_UnsetAlarm
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This disables the Alarm interrupt.
 *    
 * @result
 */
Int8 bspTwl3029_Rtc_UnsetAlarm ();

/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_Rounding30s
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This rounds-off the time to the nearest minute.
 *    
 * @result
 */
void bspTwl3029_Rtc_Rounding30s();


/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_Set12HourMode
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This sets the hour mode in RTC registers [12 hour or 24 hour].
 *    
 * @aram mode12Hour
 *    This gives the hour mode to be set.    
 * 
 * @result
 */
void bspTwl3029_Rtc_Set12HourMode( BspTwl3029_Rtc_HourMode mode12Hour);

/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_Is12HourMode
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This gets the current hour mode set in RTC registers [12 hour or 24 hour].
 *    
 * @result
 */
Uint8 bspTwl3029_Rtc_Is12HourMode();

/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_SetCompValue
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This sets the compensation value to the drift compensation registers.
 *    
 * @param driftCompensation
 *    This drift compensation value of 32 KHz clock with respect to high 
 *    frequency clock, averaged to an hour.    
 * 
 * @result
 */
Int8 bspTwl3029_Rtc_SetCompValue( Uint16 driftCompensation );

/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_EnableCompensation
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This enables/disables drift compensation.
 *    
 * @result
 */
Uint8 bspTwl3029_Rtc_EnableCompensation (Uint8 enableCompensation);

/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_intGauging
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This is the Gauging interrupt handler.
 */
void bspTwl3029_Rtc_intGauging(void);

#endif
