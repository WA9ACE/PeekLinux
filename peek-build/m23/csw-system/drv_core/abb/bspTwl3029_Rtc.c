
/*=============================================================================
 *    Copyright 2003 Texas Instruments Inc. All rights reserved.
 */

#include "types.h"
#include "bspTwl3029.h"
#include "bspTwl3029_I2c.h"
#include "bspTwl3029_IntC.h"
#include "bspTwl3029_Rtc.h"
#include "bspTwl3029_Int_Llif.h"
#include "bspTwl3029_Pwr_Map.h"
#include "bspTwl3029_Pwr_Llif.h"





/*=============================================================================
 * File Contents:
 *    This file contains the functions needed to access the RTC registers on 
 *    the Triton TWL3029.
 */

/*=============================================================================
 * Variable Definition
 */


/* Number of 32 Khz clock */



/*===========================================================================
 *   Macros:
 */
 /*===========================================================================
 *  Description:
 *   This macro converts a decimal number between 0 and 99 to BCD.
 */
#define BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( _decVal ) \
(                                                 \
    ( ( ( (_decVal) / 10 ) & 0x0F ) << 4 ) |      \
    ( ( ( (_decVal) % 10 ) & 0x0F ) << 0 )        \
)

/*===========================================================================
 *  Description:
 *   This macro converts a BCD Octet into a decimal value.
 */
#define BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL( _bcdVal )   \
(                                                   \
    ( (((_bcdVal) & 0xF0) >> 4) * 10 ) +            \
    ( (((_bcdVal) & 0x0F) >> 0) *  1 )              \
)


/*===========================================================================
 *  Description:
 *   This macro validates the passed in value as a second. 00 to 59
 */
#define BSP_TWL3029_RTC_VALID_SECOND( _second ) \
(                                               \
    (_second) <= 59                             \
)

/*===========================================================================
 *  Description:
 *   This macro validates the passed in value as a minute. 00 to 59
 */
#define BSP_TWL3029_RTC_VALID_MINUTE( _minute ) \
(                                               \
    (_minute) <= 59                             \
)

/*===========================================================================
 *  Description:
 *   This macro validates the passed in value as an hours. 00 to 23
 */
#define BSP_TWL3029_RTC_VALID_HOUR( _hour ) \
(                                           \
    (_hour) <= 23                           \
)

/*===========================================================================
 *  Description:
 *   This macro validates the passed in value as an hours(12 hr clock). 01 to 12
 */
#define BSP_TWL3029_RTC_VALID_12_HOUR( _hour ) \
(                                           \
    ((_hour) <= 12) &&                     \
    ((_hour) >= 1 )                        \
)
/*===========================================================================
 *  Description:
 *   This macro validates the passed in value as a valid day. 01 to 31
 */
#define BSP_TWL3029_RTC_VALID_DAY( _day ) \
(                                         \
    ((_day) <= 31) &&                     \
    ((_day) >= 1 )                        \
)

/*===========================================================================
 *  Description:
 *   This macro validates the passed in value as a valid month.
 *   01 to 12
 */
#define BSP_TWL3029_RTC_VALID_MONTH( _month ) \
(                                             \
    ((_month) <= 12) &&                       \
    ((_month) >= 1 )                          \
)

/*===========================================================================
 *  Description:
 *   This macro validates the passed in value as a representable year.
 *   00 to 99
 */
#define BSP_TWL3029_RTC_VALID_YEAR( _year )  \
(                                            \
    (_year) <= 99                            \
)

/*===========================================================================
 *  Description:
 *   This macro validates the passed in value as a valid weekday.
 *   00 to 06
 */
#define BSP_TWL3029_RTC_VALID_WEEKDAY( _weekday ) \
(                                                 \
    (_weekday) <= 6                               \
)
/*===========================================================================
 *  Description:
 *   This macro returns 1 for timer/alarm pm time, 1 for pm.
 *   
 */
#define GET_HOURTYPE( _hour_reg ) \
(                                                 \
    (_hour_reg) >> 7                               \
)

/*===========================================================================
 *  Description:
 *   Sets  1 for timer/alarm  hour am/pm bit.
 *   
 */
#define SET_HOURTYPE_BIT( _hourType ) \
(                                                 \
    (_hourType) << 7                               \
)

/*===========================================================================
 *   Defines:
 */
/*===========================================================================
 *  Description:
 *   Include the proper interrupt controller header file and define the
 *   interrupt source ids.
 */
#define BSP_TWL3029_RTC_TIMER_SOURCE_ID BSP_TWL3029_INTC_SOURCE_ID_TIMER
#define BSP_TWL3029_RTC_ALARM_SOURCE_ID BSP_TWL3029_INTC_SOURCE_ID_ALARM

/*===========================================================================
 *   enums and typedefs:
 */
enum
{
    BSP_TWL3029_RTC_TIME_REG_OFFSET_SECONDS = 0,
    BSP_TWL3029_RTC_TIME_REG_OFFSET_MINUTES = 1,
    BSP_TWL3029_RTC_TIME_REG_OFFSET_HOURS   = 2,
    BSP_TWL3029_RTC_TIME_REG_OFFSET_DAYS    = 3,
    BSP_TWL3029_RTC_TIME_REG_OFFSET_MONTHS  = 4,
    BSP_TWL3029_RTC_TIME_REG_OFFSET_YEARS   = 5,
    BSP_TWL3029_RTC_TIME_REG_OFFSET_WEEKS   = 6,
    BSP_TWL3029_RTC_TIME_REG_NUM = 7
};

enum
{
    BSP_TWL3029_RTC_COMP_REG_OFFSET_LSB = 0,
    BSP_TWL3029_RTC_COMP_REG_OFFSET_MSB = 1,
    BSP_TWL3029_RTC_COMP_REG_NUM        = 2
};
/*===========================================================================
 *   global Data:
 */
/*extern*/ BspTwl3029_IntCInfo bspTwl3029_IntCInfoArray[16];

/*===========================================================================
 *   Local Data:
 */


/*===========================================================================
 *  Description:
 *    This is the global storage for the periodic timer callback function.
 */
LOCAL BspTwl3029_Rtc_EventCallback bspTwl3029_Rtc_timerCallback;

/*===========================================================================
 *  Description:
 *    This is the global storage for the alarm callback function.
 */
LOCAL BspTwl3029_Rtc_EventCallback bspTwl3029_Rtc_alarmCallback;


/*===========================================================================
 *   Local Functions:
 */
 /*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_intTimer
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This is the timer interrupt handler.
 */
static void bspTwl3029_Rtc_intTimer(void);






// To check if time is right
BspTwl3029_I2C_RegData dummysec;

// this is to check wether time is right
BspTwl3029_ReturnCode BspTwl3029_RTC_check(BspTwl3029_Rtc_Time* timeptr,
												  BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr,
												  BspI2c_TransactionRequest *i2cTransArrayPtr
												)
  											
															
	{											
    
BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    int i;
    Uint16 count=0;	
    BspTwl3029_I2C_RegData* timeArrayPtr = (BspTwl3029_I2C_RegData*)timeptr;
    //BspTwl3029_I2C_RegisterInfo i2cRegArray[8];
    //BspI2c_TransactionRequest i2cTransArray[9];    
    BspTwl3029_I2C_RegisterInfo* i2cRegArraylocalPtr = i2cRegArrayPtr;
    //BspI2c_TransactionRequest *i2cTransArrayPtr = i2cTransArray;   
BspTwl3029_I2C_RegId regAdr=BSP_TWL_3029_MAP_RTC_SECOND_REG_OFFSET;
//RTC_SEND_TRACE("In RTC check",RV_TRACE_LEVEL_DEBUG_HIGH);

 
    for ( i = 0 ; i < BSP_TWL3029_RTC_TIME_REG_NUM; i++)
    {
       BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_RTC, regAdr++, timeArrayPtr++,i2cRegArraylocalPtr++ );
       count++;
    }  
    
 BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_SECOND_REG_OFFSET , &dummysec,i2cRegArraylocalPtr++ );
       count++;





	 /* Now apply/commit the above changes made to the RTC registers.*/
    i2cRegArraylocalPtr =i2cRegArrayPtr;

 returnCode = BspTwl3029_I2c_regInfoSend(i2cRegArraylocalPtr,count,NULL, i2cTransArrayPtr);
    if(returnCode!=BSP_TWL3029_RETURN_CODE_SUCCESS)
    {//RTC_SEND_TRACE("ERROR in RTC CHECK ", RV_TRACE_LEVEL_DEBUG_HIGH);
         //return returnCode; 
    }
//RTC_SEND_TRACE("Exit check",RV_TRACE_LEVEL_DEBUG_HIGH);


 return returnCode;

}

/*============================================================================*/
 /*!
 * @function BspTwl3029_RtcGetTime_Callback
 * 
 * @discussion
 * <b> Description  </b><br>
 *    Triton I2c callback function for BspTwl3029_RtcGetTime function.
 *    this performs   BCD_TO_DECIMAL conversion on the values read from the TC
 *    timer registers
 */


 static void 
 BspTwl3029_RtcGetTime_Callback (BspI2c_TransactionId transNum)
 {
      Uint8 hour;
          

 
     BspTwl3029_Rtc_Time *timePtr  = bspTwl3029_IntCInfoArray[transNum].twl2029callbackFuncArgs;
 
	// Addition for Neptune
     hour = (0x7f & timePtr->hour);

   
 
     timePtr->hourType = GET_HOURTYPE(timePtr->hour);
     timePtr->second  = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL( timePtr->second );
     timePtr->minute  = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL( timePtr->minute );
     timePtr->hour    = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL( hour);
     timePtr->day     = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL( timePtr->day  );
     timePtr->month   = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL( timePtr->month );
     timePtr->year    = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL( timePtr->year );
     timePtr->weekday = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL( timePtr->weekday );  
     timePtr->hourMode = bspTwl3029_Rtc_Is12HourMode();
   

 }
/*============================================================================*/
/*!
 * @function BspTwl3029_RtcIsReset_Callback
 * 
 * @discussion
 * <b> Description  </b><br>
 *    Triton I2c callback function for BspTwl3029_RtcIsReset function.
 *    this gets the value of the BSP_TWL3029_RTC_POWER_UP bit from the  
 *    STATUS register data that has just bee read. 
 */
static void  
BspTwl3029_RtcIsReset_Callback(BspI2c_TransactionId transNum)
{  
   BspTwl3029_I2C_RegData regData = 0;   
      
   BspTwl3029_I2C_Callback  callback;
   
   static BspI2c_TransactionRequest i2cTransArray[2];    
        
   Uint8* resetStatus = (Uint8*) bspTwl3029_IntCInfoArray[transNum].twl2029callbackFuncArgs;  
   
   if ( *resetStatus & BSP_TWL3029_RTC_POWER_UP) 
   { 
     /* reset the POWER_UP bit, just use a default dummy triton callback  function as the i2c callback function*/ 
     callback.callbackFunc = BspTwl3029_I2c_callbackFunction;
     callback.callbackVal = (BspI2c_TransactionId) transNum;
     callback.i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)&i2cTransArray;
      
  
      regData |= BSP_TWL3029_RTC_POWER_UP;
         
      BspTwl3029_I2c_WriteSingle(1, BSP_TWL3029_RTC_STATUS_REG,regData,
                   &callback);
       
   }
      
      /* shift the POWER_UP  bit value  to LSB  */ 
      *resetStatus = (*resetStatus & BSP_TWL3029_RTC_POWER_UP) >> BSP_TWL3029_LLIF_RTC_RTC_STATUS_REG_POWER_UP_OFFSET;   
}





/*=============================================================================
 * Description:
 *   This function checks the validity of the passed in time/date combination.
 *   Note: Weekday is ignored!
 */
static Bool
bspTwl3029_Rtc_timeIsValid( BspTwl3029_Rtc_Time *timePtr )
{
    Bool isValid;
    Uint8 compareVal;
    Uint8 hrMode;
    
    isValid = FALSE;
    hrMode = timePtr->hourMode;
    //hrMode =  bspTwl3029_Rtc_Is12HourMode();   
    if (hrMode > BSPTWL3029_RTC_24_HOURMODE)
    {
        /* only values of 0 ( 24 hr) and 1 (12 hr) are valid */
	return   (isValid);
    }
    else if  ((hrMode == BSPTWL3029_RTC_12_HOURMODE) && ( BSP_TWL3029_RTC_VALID_12_HOUR( timePtr->hour )==0))
    {       
        return   (isValid);
    }
    else if ((hrMode == BSPTWL3029_RTC_24_HOURMODE) && ( BSP_TWL3029_RTC_VALID_HOUR( timePtr->hour )==0))
    {       
        return   (isValid);
    }  
    if ((hrMode == BSPTWL3029_RTC_12_HOURMODE) &&(timePtr->hourType > BSPTWL3029_RTC_PM_HOURTYPE))
    {
        /* only values of 0 ( am ) and 1 (0 ) are valid */
	return   (isValid);
    }  
      
    if( BSP_TWL3029_RTC_VALID_SECOND( timePtr->second ) &&
        BSP_TWL3029_RTC_VALID_MINUTE( timePtr->minute ) &&
    //    BSP_TWL3029_RTC_VALID_HOUR( timePtr->hour ) &&
        BSP_TWL3029_RTC_VALID_DAY( timePtr->day ) &&
        BSP_TWL3029_RTC_VALID_MONTH( timePtr->month ) &&
        BSP_TWL3029_RTC_VALID_YEAR( timePtr->year ) )
    {
        /*
         * Do some special checking of valid days of the month.
         * Note: The day has already been verified to be between
         *       1 and 31
         */
        switch( timePtr->month )
        {
            case BSP_TWL3029_RTC_MONTH_FEBRUARY:
            {
                /* mod by 4 to see if it's leap year */
                if( ( timePtr->year & 0x03 ) == 0 )
                {
                    compareVal = 29;
                }
                else
                {
                    compareVal = 28;
                }
            }
            break;
            
            case BSP_TWL3029_RTC_MONTH_APRIL:
            case BSP_TWL3029_RTC_MONTH_JUNE:
            case BSP_TWL3029_RTC_MONTH_SEPTEMBER:
            case BSP_TWL3029_RTC_MONTH_NOVEMBER:
            {
                compareVal = 30;
            }
            break;

            case BSP_TWL3029_RTC_MONTH_JANUARY:
            case BSP_TWL3029_RTC_MONTH_MARCH:
            case BSP_TWL3029_RTC_MONTH_MAY:
            case BSP_TWL3029_RTC_MONTH_JULY:
            case BSP_TWL3029_RTC_MONTH_AUGUST:
            case BSP_TWL3029_RTC_MONTH_OCTOBER:
            case BSP_TWL3029_RTC_MONTH_DECEMBER:
            default:            
            {
                compareVal = 31;
            }
            break;
        }
        
        if( timePtr->day <= compareVal )
        {
            isValid = TRUE;
        }
    }
    
    return( isValid );
}

/*=============================================================================
 * Description:
 *   Alarm interrupt handler.
 *   This is the BSP_RTC interrupt handler for alarm interrupts. The driver must
 *   Acknowledge the interrupt at the BSP_RTC hardware so it will reset.
 */
void bspTwl3029_Rtc_intAlarm(void)
{
 
   
    /*  resetting of alarm has  already been done  done in by writing 1 back 
        into ALARM bit in the STATUS register. This has been done as part of
	Triton IntC ISR routine ( from which this function is called ) 
     */

   /* now call external alarm callback routine ( if one defined ) */
   if( bspTwl3029_Rtc_alarmCallback != NULL )
   {
      bspTwl3029_Rtc_alarmCallback();
   }
 
   return;  
}


/*=============================================================================
 * Description:
 *   This is the BSP_RTC interrupt handler for periodic timer interrupts.
 *   Nothing special needs to be done to the BSP_RTC hardware to acknowledge this
 *   interrupt
 */

static void bspTwl3029_Rtc_intTimer(void)
{
 /* now call external alarm callback routine ( if one defined ) */
   if( bspTwl3029_Rtc_timerCallback != NULL )
   {
      bspTwl3029_Rtc_timerCallback();
   }
 
   return;       
}

/*=============================================================================
 * Description:
 *   This is the BSP_RTC interrupt handler. This function queries the 
 *   status register to ascertain the source of interrupt ( timer /alarm)
 *   and exestue the approprial routine.
 *   The driver must acknowledge the interrupt at the BSP_RTC 
 *   hardware so it will reset.  
 */
static void
bspTwl3029_Rtc_interruptHandler( BspTwl3029_IntC_SourceId sourceId )
{

    BspTwl3029_ReturnCode errorCode = BSP_TWL3029_RETURN_CODE_FAILURE;
   

    BspTwl3029_I2C_RegData statusRegData = 0; 
    BspTwl3029_I2C_RegData intRegData, period = 0;

   /* Read the ABB RTC status register and find the source of interrupt */
   /* read twice as a dummy read is neccesary for HW to upadte status bits */
   
 
   errorCode = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_STATUS_REG_OFFSET,&statusRegData);
   		   
   
  
   if ( errorCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {
      /* Call the necessary interrupt handler  */
      if ( statusRegData & (1 << BSP_TWL3029_LLIF_RTC_RTC_STATUS_REG_ALARM_OFFSET))
      {
         bspTwl3029_Rtc_intAlarm();
         
      }
   /* note: for Virtio, the period event bits don't get reset ( Virtio bug) */
#if (VIRTIO==1)
      else
      {
          bspTwl3029_Rtc_intTimer();
      }

#else    
      /* check for timer interrupt */
      /* first check if enabled */
      BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_INTERRUPTS_REG_OFFSET,&intRegData);
      
      /* if interrupt is enables and if one of the timer interrrupt events has occured */
      if (intRegData &(1 <<BSP_TWL3029_LLIF_RTC_RTC_INTERRUPTS_REG_IT_TIMER_OFFSET ))
      {        
         /* now get which type period is enabled  by gettig the two EVERY bits */
	  period = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_RTC_RTC_INTERRUPTS_REG_EVERY, &intRegData ); 
	 
         /* if the type of event corresponds to the period that has been enabled  */
         if ( ((period == BSP_TWL3029_RTC_INTERRUPTS_EVERY_SECOND) && 
               (statusRegData & (1 << BSP_TWL3029_LLIF_RTC_RTC_STATUS_REG_1S_EVENT_OFFSET))) ||
	      ((period == BSP_TWL3029_RTC_INTERRUPTS_EVERY_MINUTE) && 
               (statusRegData & (1 << BSP_TWL3029_LLIF_RTC_RTC_STATUS_REG_1M_EVENT_OFFSET))) ||
	      ((period == BSP_TWL3029_RTC_INTERRUPTS_EVERY_HOUR) && 
               (statusRegData & (1 << BSP_TWL3029_LLIF_RTC_RTC_STATUS_REG_1H_EVENT_OFFSET))) ||
	      ((period == BSP_TWL3029_RTC_INTERRUPTS_EVERY_DAY) && 
               (statusRegData & (1 << BSP_TWL3029_LLIF_RTC_RTC_STATUS_REG_1D_EVENT_OFFSET)))
	    )
	         
         {
            bspTwl3029_Rtc_intTimer();
         }
      }	 
#endif
   }
}


 
/*=============================================================================
 * Description:
 *   Minute timer interrupt handler.
 */
/*static */void bspTwl3029_Rtc_timerMinuteCallback (void)  
{
#if 0
   static double compensation = 0;
   static Uint8 nb_sample = 0;
   double delta;
   static Uint16 calibValue = 0;
   Int16 tmp_value = 0;


   static Bsp_Twl3029_I2cTransReqArray  i2cTransReqArray;
   BspTwl3029_I2C_Callback callback;
   
   
   /* Evaluate average on one hour max */
   if ( nb_sample < 60)
      nb_sample++;

   /* perform calculation of auto compensation each minute and evaluate an
      average on one hour */
   /* Number of 32 kHz clock lost in one second */
   /* Accurate operation is : delta = CLK_32 - rtc_nb_32khz*CLK_PLL/g_nb_hf */
   /* with CLK_32 = 32768 Hz and CLK_PLL depend on chipset */
   delta = RTC_CLOCK_32K - bspTwl3029_Rtc_32khz*RTC_CLOCK_HF/bspTwl3029_Rtc_hf;

   /* Number of 32 kHz clock lost in one hour */
   delta *= 3600.0;

   /* Average of the compensation to load */
   compensation = (compensation*(nb_sample-1) + delta)/nb_sample;

   if (compensation >= 0x7FFF)
      tmp_value = 0x7FFE;
   else if (compensation <= -0x7FFF)
      tmp_value = -0x7FFE;
   else
      tmp_value = (Int16) compensation;

   if (tmp_value > 0) /* if 32 Khz clock is slow */
      calibValue = tmp_value;
   if (tmp_value < 0) /* if 32 Khz clock is fast */
      calibValue = 0xFFFF + tmp_value + 1;

   //bspTwl3029_Rtc_SetCompValue( calibValue );
   
   callback.callbackFunc = BspTwl3029_I2c_callbackFunction;
   callback.callbackVal = 0;
   callback.i2cTransArrayPtr = &i2cTransReqArray;
   bspTwl3029_Rtc_calibrationSet(  &callback, calibValue );
#endif
}




////////////////


/*=============================================================================
 * Local Functions 
*/
/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_intTimer
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This is the timer interrupt handler.
 */
//static void bspTwl3029_Rtc_intTimer(void);

/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_intAlarm
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This is the alarm interrupt handler.
 */

void bspTwl3029_Rtc_intAlarm(void);

/*=============================================================================*/
/*!
 * @function bspTwl3029_Rtc_intHandler
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This is the ABB RTC interrupt handler.
 */
void bspTwl3029_Rtc_intHandler(BspTwl3029_IntC_SourceId sourceId);

/*=============================================================================
 * Function Definition
 */

/*=============================================================================
 * Description:
 *   Initialization function for RTC module.
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_init (void)
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegisterInfo i2cRegArray[5];
    BspI2c_TransactionRequest i2cTransArray[6];    
    BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
    BspI2c_TransactionRequest *i2cTransArrayPtr = i2cTransArray;
    //BspTwl3029_I2C_RegData regStatusData = 0;
    BspTwl3029_I2C_RegData regCtrlData = 0;

#if 0
    BspTwl3029_I2C_RegData timeArray[ BSP_TWL3029_RTC_TIME_REG_NUM ];
#endif

    BspTwl3029_I2C_RegId regAdr;
    Uint16 i, count = 0;
    
    bspTwl3029_Rtc_alarmCallback = NULL;
   
    
    		    
    /* stop RTC module  by resetting the STOP_RTC bit in RTC_CTRL_REG*/
    /* 
     *  First build a local copy of the ctrl register we want to write.
     * - Disable  the RTC
     * - Don't round
     * - Disable the AutoComp (will be enabled when calibrated)
     * - Mode is  24 hour
     * - TestMode is always disabled
     * - Setting the counter is forbidden during normal use.
     */
// read the rtc ctrl reg to save the hour mode 
	returnCode=BspTwl3029_I2c_ReadSingle(BSP_TWL3029_I2C_RTC,  BSP_TWL3029_RTC_CTRL_REG, 
                                           					&regCtrlData,NULL );
     BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_RTC_RTC_CTRL_REG_STOP_RTC, &regCtrlData, BSP_TWL3029_RTC_CTRL_STOP_RTC_FREEZE ); 
     BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_RTC_RTC_CTRL_REG_STOP_RTC, &regCtrlData, BSP_TWL3029_RTC_CTRL_ROUND_30S_NO_UPDATE ); 
     BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_RTC_RTC_CTRL_REG_STOP_RTC, &regCtrlData, BSP_TWL3029_RTC_CTRL_AUTO_COMP_DISABLED ); 
     //BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_RTC_RTC_CTRL_REG_STOP_RTC, &regCtrlData, BSP_TWL3029_RTC_CTRL_MODE_24 ); 
     BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_RTC_RTC_CTRL_REG_STOP_RTC, &regCtrlData, BSP_TWL3029_RTC_CTRL_TEST_MODE_DISABLED ); 
     BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_RTC_RTC_CTRL_REG_STOP_RTC, &regCtrlData, BSP_TWL3029_RTC_CTRL_SET_COUNTER_DISABLED ); 
   
    
    /*
     * Write the value to the CTRL register.
     */
     if(returnCode==BSP_TWL3029_RETURN_CODE_FAILURE)
    	{
	       return returnCode;
    	}
	 BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_CTRL_REG_OFFSET,
			   regCtrlData,i2cRegArrayPtr++ );
     count++;
    

    /* program TC registers */
    /* Time is initialized to 00:00:00 and date is initialized to Monday 01/01/01.*/
        
#if 0
    timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_SECONDS ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( 0 );
    timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_MINUTES ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( 0 );
    timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_HOURS   ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( 0 );
    timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_DAYS    ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( 1 );
    timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_MONTHS  ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( 1 );
    timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_YEARS   ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( 1 );
    timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_WEEKS   ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( 1 );

         
    regAdr = BSP_TWL_3029_MAP_RTC_SECOND_REG_OFFSET;
    
    for ( i = 0 ; i < BSP_TWL3029_RTC_TIME_REG_NUM; i++)
    {
       BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_RTC, regAdr++, timeArray[i],i2cRegArrayPtr++ );
       count++;
    }   
#endif         

    /* restart  */	
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_RTC_RTC_CTRL_REG_STOP_RTC, &regCtrlData, BSP_TWL3029_RTC_CTRL_STOP_RTC_RUN ); 
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_CTRL_REG_OFFSET,
			       regCtrlData,i2cRegArrayPtr++ );
    count++;
     
     
    /* calibartation register */
    /*
     * The calibration is disabled by default, but just put a known value of 0
     * into the calibration value
     */
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_COMP_LSB_REG_OFFSET,0,i2cRegArrayPtr++ );
    count++;
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_COMP_MSB_REG_OFFSET,0,i2cRegArrayPtr++ );
    count++;
    /*
     * Now we need to register the BSP_RTC interrupt function with the
     * interrupt controller.
     */
    
    /* Register the interrupt handlers */
    bspTwl3029_IntC_setHandler(  BSP_TWL3029_INTC_SOURCE_ID_RTC,
                             bspTwl3029_Rtc_interruptHandler );
    
    /* disable interrupt from  the RTC  interrupt sources( alarm interrupt and periodic timer interrupt) */ 
    
   
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_INTERRUPTS_REG_OFFSET,
                               0,i2cRegArrayPtr++ );
    count++;

    /* now write the whole lot to triton i2c rtc registers */
    
    i2cRegArrayPtr = i2cRegArray;
    returnCode = BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,count,NULL, i2cTransArrayPtr);
    
    
    /* Disable all IT before accessing register */
    //rvf_disable();

    if ( returnCode != BSP_TWL3029_RETURN_CODE_FAILURE)
    {
       /* Enable RTC interrupt */
       returnCode = bspTwl3029_IntC_enableSource(  BSP_TWL3029_INTC_SOURCE_ID_RTC);
    }
    

    return returnCode;
}
/*=============================================================================
 * Description:
 *   Indicate if a RTC reset occured.
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_isReset( BspTwl3029_I2C_CallbackPtr callbackInfoPtr, Uint8 * resetStatus)
{
   BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
   BspTwl3029_I2C_Callback callbackFuncInfo;
   BspTwl3029_I2C_CallbackPtr callbackFuncPtr = &callbackFuncInfo;
   
   BspTwl3029_I2C_CallbackPtr apiCallbackFuncPtr = callbackInfoPtr;
   BspTwl3029_I2C_RegData regData = 0;   
      
   /* for I2C callback mode we need to call a Triton i2c ISR to check if bit needs to be reset */
   /* this is done by setting up I2C callback to use a default isr routine defined in twl3029 I2c driver.
      this in turn will call the API defined I2C ISr routine and also a Triton ISR routine specific
      to this function
    */  
    if (apiCallbackFuncPtr != NULL)
    {	
        
	/* for interrupt callback we need to call a Triton callback function 
	   This will Read POWER UP bit to inform MMI of a possible RTC reset
	   and will then reset the bit
	 */  
	if ( bspTwl3029_I2cCallbackSetup( callbackFuncPtr,apiCallbackFuncPtr,
	                                   BspTwl3029_RtcIsReset_Callback, (BspTwl3029_I2C_RegData*)resetStatus ) == -1)
        {
	   /* error */
	   return (returnCode);	  
        } 
	
    }
    else
    {
       callbackFuncPtr = NULL;
    }
   
      
   /* Read POWER UP bit to inform MMI of a possible RTC reset */
   returnCode = BspTwl3029_I2c_ReadSingle(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_STATUS_REG,
			     resetStatus,callbackFuncPtr );
   
   /* for non I2C callback mode, return the POWER UP value to calling function. this is done
      as part of Triton I2C callback ISR for I2C  callback mode
   */
   if ( apiCallbackFuncPtr == NULL)
   {
       /* Read POWER UP bit to inform MMI of a possible RTC reset */
      if ( *resetStatus & BSP_TWL3029_RTC_POWER_UP) 
      {
         regData = (BspTwl3029_I2C_RegData) *resetStatus;
	 regData |= BSP_TWL3029_RTC_POWER_UP;
         returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_STATUS_REG,
			   regData,NULL );
      }
      
      /* shift the  value of POWER UP bit  to LSB for the calling function */ 
      *resetStatus = (*resetStatus & BSP_TWL3029_RTC_POWER_UP) >> BSP_TWL3029_LLIF_RTC_RTC_STATUS_REG_POWER_UP_OFFSET;
   }
   
   return  (returnCode);
}







/*===========================================================================
 *  Description:
 *   This function gets the current time and copies it into the passed in
 *   location.
 */




BspTwl3029_ReturnCode
bspTwl3029_Rtc_timeGet( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                        BspTwl3029_Rtc_Time *timePtr )
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_Callback callbackFuncInfo;
    BspTwl3029_I2C_CallbackPtr callbackFuncPtr = &callbackFuncInfo;
    BspTwl3029_I2C_CallbackPtr apiCallbackFuncPtr = callbackInfoPtr;
    
    BspTwl3029_I2C_RegisterInfo i2cRegArray[8];
    BspI2c_TransactionRequest i2cTransArray[9];    
    BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
    BspI2c_TransactionRequest *i2cTransArrayPtr = i2cTransArray;
    Uint8 hour = 0;
    
    BspTwl3029_I2C_RegData* timeArrayPtr = (BspTwl3029_I2C_RegData*)timePtr;
   
    BspTwl3029_I2C_RegId regAdr;
    Uint16 i, count = 0;
    //tatic int flag=0;
    
   /* for I2C callback mode we need to call a Triton i2c ISR to check if bit needs to be reset */
    /* this is done by setting up I2C callback to use a default isr routine defined in twl3029 I2c driver.
      this in turn will call the API defined I2C ISr routine and also a Triton ISR routine specific
      to this function
    */  
    if (apiCallbackFuncPtr != 0)
    {
        /* for interrupt callback we need to call a Triton callback function 
	   This Triton callback function performs the BCD to decimal conversions 
	 */  

    
	if ( bspTwl3029_I2cCallbackSetup( callbackFuncPtr, apiCallbackFuncPtr, BspTwl3029_RtcGetTime_Callback, timePtr) == -1)
        {
	   /* error */
	   returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;	  
        } 
	/* set pointer to point to correct transaction stuct - not the local one defined above */ 
	i2cTransArrayPtr = (BspI2c_TransactionRequest*) callbackFuncPtr->i2cTransArrayPtr;
    }
    else 
    {
       callbackFuncPtr = NULL;
    }
   /* 
      recast timePtr to BspTwl3029_I2C_RegData*  in order for pointer to access 
      individual elements of the struct
      this can be done 'cause BspTwl3029_I2C_RegData is of thype Uint8 and the
      struct BspTwl3029_Rtc_Time consists of 7 elements of type Uint8  
   */
   
 // dummysec=0xFF;

    regAdr = BSP_TWL_3029_MAP_RTC_SECOND_REG_OFFSET;
    for ( i = 0 ; i < BSP_TWL3029_RTC_TIME_REG_NUM; i++)
    {
       BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_RTC, regAdr++, timeArrayPtr++,i2cRegArrayPtr++ );
       count++;
    }  
    




 BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_SECOND_REG_OFFSET , &dummysec,i2cRegArrayPtr++ );
       count++;



	 /* Now apply/commit the above changes made to the RTC registers.*/
    i2cRegArrayPtr = i2cRegArray;
    returnCode = BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,count,callbackFuncPtr, i2cTransArrayPtr);
    






 //for I2C POLL mode, we now have valid data read back from Triton registers, so convert it to decimal */
if(apiCallbackFuncPtr==NULL)
{
  // this work around was done for the problem of reading seconds 
  //register and by the time if minutes reg is read if there is a interrupt then time jumps by 1 minute .
  //OMAPS00056598
  //this workaround is valid only for blocking call.
  while(dummysec<timePtr->second)
 	{
           
  	  returnCode=BspTwl3029_RTC_check(timePtr,i2cRegArrayPtr,i2cTransArrayPtr);
           
	  if(returnCode!=BSP_TWL3029_RETURN_CODE_SUCCESS)
         return returnCode;
	  
        }

    

}



    if  (( returnCode != BSP_TWL3029_RETURN_CODE_FAILURE ) && ( apiCallbackFuncPtr == NULL))
    {
        // Addition for Neptune
        hour = (0x7f & timePtr->hour);
	timePtr->hourType = GET_HOURTYPE (timePtr->hour);
	timePtr->second  = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL( timePtr->second );
        timePtr->minute  = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL( timePtr->minute );
        timePtr->hour    = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL( hour);
        timePtr->day     = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL( timePtr->day  );
        timePtr->month   = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL( timePtr->month );
        timePtr->year    = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL( timePtr->year );
        timePtr->weekday = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL( timePtr->weekday );
        
        timePtr->hourMode = bspTwl3029_Rtc_Is12HourMode();
    }
    
    
    return( returnCode );
}











/*===========================================================================
 *  Description:
 *   This function sets the current time to the passed in value.
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_timeSet( BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                        BspTwl3029_Rtc_Time *timePtr )
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    
    BspTwl3029_I2C_RegisterInfo i2cRegArray[10];
    BspI2c_TransactionRequest i2cTransArray[12];    
    BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
    BspI2c_TransactionRequest *i2cTransArrayPtr = i2cTransArray;

    
    BspTwl3029_I2C_RegId regAdr;
    BspTwl3029_I2C_RegData regData = 0;
    Uint16 i, count = 0;
    
    BspTwl3029_I2C_RegData timeArray[ BSP_TWL3029_RTC_TIME_REG_NUM ];
   
       
    
    if( ( BSP_TWL3029_RTC_VALID_WEEKDAY( timePtr->weekday ) == TRUE ) &&
        ( bspTwl3029_Rtc_timeIsValid( timePtr ) == TRUE ) )
    {
        
	/* set  am /pm bit in CTRL register */ 
        BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_CTRL_REG, &regData );
       /* first clear ROUND_30s bit (bit 1)   and SET_32_COUNTER bit ( bit 5)  ( if set in shadow reg )*/
       regData &= 0xdd;
	if ( timePtr->hourMode == BSPTWL3029_RTC_24_HOURMODE)
       {
         regData &= 0xf7;
       }
       else
       {
         regData |= 0x08;
       }
       BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_CTRL_REG, regData ,i2cRegArrayPtr++ );
          count++; 
	
	/* now set TC registers */
	timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_SECONDS ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( timePtr->second );
        timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_MINUTES ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( timePtr->minute );        

	if (timePtr->hourMode == BSPTWL3029_RTC_24_HOURMODE)
	{
          timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_HOURS ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD(timePtr->hour);
	}  
        else
        {
	   timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_HOURS] = (BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD(timePtr->hour) | 
	                                                           (timePtr->hourType << 7));
        }	 
	timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_DAYS    ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( timePtr->day );
        timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_MONTHS  ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( timePtr->month );
        timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_YEARS   ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( timePtr->year );
        timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_WEEKS   ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( timePtr->weekday );
	
	/*
         * At this point the passed in time has been completely validated, but the
         * dates month/day/weekday/year combination has not been validated.
         */
                            
       /* now program the timer registers */
       regAdr = BSP_TWL_3029_MAP_RTC_SECOND_REG_OFFSET;
    
       for ( i = 0 ; i < BSP_TWL3029_RTC_TIME_REG_NUM; i++)
       {
          BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_RTC, regAdr++, timeArray[i],i2cRegArrayPtr++ );
          count++;
       }         
        
        /* Now apply/commit the above changes made to the RTC registers.
         */
	i2cRegArrayPtr = i2cRegArray;
        returnCode = BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,count,callbackFuncPtr, i2cTransArrayPtr);
    }
    
    return( returnCode );
}
/*=============================================================================
 * Description:
 *   Get the alarm date and time.
 */
BspTwl3029_ReturnCode 
bspTwl3029_Rtc_alarmGet (BspTwl3029_Rtc_Time* dateTime)
{
   
   Uint8 hour = 0;
    
   //rvf_disable();
   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_ALARM_DAYS_REG,&dateTime->day);   
   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_ALARM_MONTHS_REG, &dateTime->month);   
   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_ALARM_YEARS_REG, &dateTime->year);   
   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_ALARM_SECONDS_REG,&dateTime->second);   
   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_ALARM_MINUTES_REG, &dateTime->minute);   
   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_ALARM_HOURS_REG,&dateTime->hour);
        
   //rvf_enable();

    // Addition for Neptune
   hour = (0x7f & dateTime->hour);

   if( bspTwl3029_Rtc_Is12HourMode() == BSPTWL3029_RTC_24_HOURMODE)
   {
	   /* 24 hour mode */
      dateTime->hourMode = BSPTWL3029_RTC_24_HOURMODE;
      dateTime->hourType = BSPTWL3029_RTC_AM_HOURTYPE;
   }
   else
   {
      /* 12 hour mode */
      dateTime->hourMode = BSPTWL3029_RTC_12_HOURMODE;
      if ((dateTime->hour & 0x80) == 0)
         dateTime->hourType = BSPTWL3029_RTC_AM_HOURTYPE;
      else
         dateTime->hourType = BSPTWL3029_RTC_PM_HOURTYPE;
   }

   dateTime->day = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL(dateTime->day);
   dateTime->month = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL(dateTime->month);
   dateTime->year = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL(dateTime->year);
   dateTime->hour = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL(hour);
   dateTime->minute = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL(dateTime->minute);
   dateTime->second = BSP_TWL3029_RTC_CONVERT_BCD_TO_DECIMAL(dateTime->second);

   return (BSP_TWL3029_RETURN_CODE_SUCCESS);
}

/*===========================================================================
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_alarmSet( BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                         BspTwl3029_Rtc_EventCallback alarmCallbackFunctionPtr,
                         BspTwl3029_Rtc_Time          *timePtr )
{
    
    
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    
    BspTwl3029_I2C_RegisterInfo i2cRegArray[10];
    BspI2c_TransactionRequest i2cTransArray[12];    
    BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
    BspI2c_TransactionRequest *i2cTransArrayPtr = i2cTransArray;
    
    BspTwl3029_I2C_RegId regAdr;
    BspTwl3029_I2C_RegData tmpReg;
    BspTwl3029_I2C_RegData timeArray[ BSP_TWL3029_RTC_TIME_REG_NUM ];
    Uint16 i, count = 0;    
    
    
    if (( bspTwl3029_Rtc_timeIsValid( timePtr ) == TRUE ) &&
        ( alarmCallbackFunctionPtr != NULL ) )
    {
                
        /*
         * Disable the alarm interrupt
         */
        BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_RTC,BSP_TWL3029_RTC_INTERRUPTS_REG,&tmpReg );
        BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_RTC_RTC_INTERRUPTS_REG_IT_ALARM, &tmpReg,
	                           BSP_TWL3029_RTC_INTERRUPTS_IT_ALARM_DISABLED);
				   
	BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_INTERRUPTS_REG, tmpReg,i2cRegArrayPtr++ );
        count++;
        /* reset alarm interrrupt ( just in case it had been set  previously) */
	BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_STATUS_REG_OFFSET, 
	                          (1) << BSP_TWL3029_LLIF_RTC_RTC_STATUS_REG_ALARM_OFFSET,i2cRegArrayPtr++ );
        count++;
        /*
         * Setup the callback function and alarm time.
         */
        bspTwl3029_Rtc_alarmCallback = alarmCallbackFunctionPtr;
        
        timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_SECONDS ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( timePtr->second );
        timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_MINUTES ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( timePtr->minute );
        if (timePtr->hourMode == BSPTWL3029_RTC_24_HOURMODE)
	{
          timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_HOURS ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD(timePtr->hour);
	}  
        else
        {
	   timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_HOURS] = (BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD(timePtr->hour) | 
	                                                           (timePtr->hourType << 7));
        }
	//timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_HOURS   ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( timePtr->hour );
        timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_DAYS    ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( timePtr->day );
        timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_MONTHS  ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( timePtr->month );
        timeArray[ BSP_TWL3029_RTC_TIME_REG_OFFSET_YEARS   ] = BSP_TWL3029_RTC_CONVERT_DECIMAL_TO_BCD( timePtr->year );
        
        
        regAdr = BSP_TWL_3029_MAP_RTC_ALARM_SECONDS_REG_OFFSET;
    
       for ( i = 0 ; i < 6; i++)
       {
          BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_RTC, regAdr++, timeArray[i],i2cRegArrayPtr++ );
          count++;
       }   
	
	            
        /*
         * enable the alarm  timer interrupt 
         */
        BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_RTC_RTC_INTERRUPTS_REG_IT_ALARM, &tmpReg,
	                           BSP_TWL3029_RTC_INTERRUPTS_IT_ALARM_ENABLED);
	
	BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_INTERRUPTS_REG, tmpReg,i2cRegArrayPtr++ );
        count++;
	
	
	 /* Now apply/commit the above changes made to the RTC registers.*/
       i2cRegArrayPtr = i2cRegArray;
       returnCode = BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,count,callbackFuncPtr, i2cTransArrayPtr);
	
    }
    
    return( returnCode );
}


/*===========================================================================
 */

BspTwl3029_ReturnCode
bspTwl3029_Rtc_alarmClear( BspTwl3029_I2C_CallbackPtr callbackFuncPtr )
{      
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
           
    BspTwl3029_I2C_RegData regData;
   
        
    /*
     * clear the callback function and set the alarm to a known value
     */
    bspTwl3029_Rtc_alarmCallback = NULL;
    
    /*
     * Disable the alarm interrupt
     */
    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_RTC,BSP_TWL3029_RTC_INTERRUPTS_REG,&regData );
    BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_RTC_RTC_INTERRUPTS_REG_IT_ALARM, &regData,
	                       BSP_TWL3029_RTC_INTERRUPTS_IT_ALARM_DISABLED);
				   
    returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_INTERRUPTS_REG,
			       regData,callbackFuncPtr );
   
        
    return( returnCode );
}

/*===========================================================================
 *  Description:
 *   This function sets up the new calibration value and also enables the
 *   auto-calibration mechanism that is disabled at initialization.
 *   Note: The BSP_RTC hardware has 2 8-bit registers to hold the calibration value
 *         The value needs to be stored as 2's complement.
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_calibrationSet( BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                               BspTwl3029_Rtc_CalibrationValue calibValue )
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    
    BspTwl3029_I2C_RegisterInfo i2cRegArray[10];
    BspI2c_TransactionRequest i2cTransArray[12];    
    BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
    BspI2c_TransactionRequest *i2cTransArrayPtr = i2cTransArray;
   
    Uint16 count = 0;
    
    BspTwl3029_I2C_RegData compArray[ BSP_TWL3029_RTC_COMP_REG_NUM ];
    Uint16 calibValue2s;
    
    /* Convert the 1's complement to 2's complement */
    if( calibValue < 0 )
    {
        calibValue2s = ( (~(Uint16)calibValue) + 1 );
    }
    else
    {
        calibValue2s = calibValue;      
    }
    
    /* Set the 2 calibration registers */
    compArray[ BSP_TWL3029_RTC_COMP_REG_OFFSET_MSB ] = ( (calibValue2s >> 8) & 0xFF );
    compArray[ BSP_TWL3029_RTC_COMP_REG_OFFSET_LSB ] = ( (calibValue2s >> 0) & 0xFF );
    
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_COMP_LSB_REG_OFFSET,
                              compArray[0],i2cRegArrayPtr++ );
    count++;
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_COMP_MSB_REG_OFFSET,
                               compArray[1],i2cRegArrayPtr++ );
    count++;
    
    /* now update Triton rtc calibration registers */ 
    if (callbackFuncPtr != 0)
    {
	i2cTransArrayPtr = (BspI2c_TransactionRequest*) callbackFuncPtr->i2cTransArrayPtr;
    }
    
    i2cRegArrayPtr = i2cRegArray;
    returnCode = BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,count,callbackFuncPtr, i2cTransArrayPtr);
    
    return( returnCode );
}

/*===========================================================================
 *  Description:
 *   This function retreives the stored calibration value. Returning it as
 *   a single 1's complement signed 16-bit integer.
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_calibrationGet( BspTwl3029_Rtc_CalibrationValue* calibVal )
{
    
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    
    Uint16 calibValue2s;
    
    BspTwl3029_I2C_RegData compArray[ BSP_TWL3029_RTC_COMP_REG_NUM ];
    
    returnCode = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_COMP_LSB_REG_OFFSET,
                              &compArray[0] );
    
    returnCode = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_COMP_MSB_REG_OFFSET,
                               &compArray[1]);
      
    /* Get the 2 calibration registers and combine them into a single value */
    /* only do this fo I2C polled mode. For callback this is done as part of Triton ISR */
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
       calibValue2s = ( ( compArray[ BSP_TWL3029_RTC_COMP_REG_OFFSET_MSB ] << 8 ) |
                     ( compArray[ BSP_TWL3029_RTC_COMP_REG_OFFSET_LSB ] << 0 ) );
    
      /* Now convert the 2's complement to 1's complement */
      if( (calibValue2s & 0x8000) != 0 )
      {
          /* The number is negative so conversion is necessary */
          *calibVal = -( ~(Uint16)( calibValue2s - 1 ) );
      }
      else
      {
          /* The number is positive so no conversion is necessary */
          *calibVal = calibValue2s;
      }
   }     
    return( returnCode );
}




/*==============================================================================*/
/*!
 * @function bspTwl3029_Rtc_timerIntEnable
 *
 * @discussion
 *    sets up and enables the interrupt for the periodic timer.
 *
 *
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_timerIntEnable( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                            BspTwl3029_Rtc_EventCallback timerCallbackFunctionPtr,
                            Uint8   period )
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegData regData = 0;   
   
    
    /* check if args are valid */
   if ( period > BSP_TWL3029_RTC_INTERRUPTS_EVERY_DAY)
   {
      return (returnCode);
   } 

    /* Setup the callback function */         
    bspTwl3029_Rtc_timerCallback = timerCallbackFunctionPtr;
 	
   /* enable the interrupt */
   BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_RTC,BSP_TWL_3029_MAP_RTC_RTC_INTERRUPTS_REG_OFFSET,&regData );
   /* set the period*/
   BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_RTC_RTC_INTERRUPTS_REG_EVERY, &regData, period);
   /* enable the interrupt */
   BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_RTC_RTC_INTERRUPTS_REG_IT_TIMER, &regData,
	                      BSP_TWL3029_RTC_INTERRUPTS_IT_TIMER_ENABLED);
   
   
   returnCode =  BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_INTERRUPTS_REG_OFFSET, 
                                            regData,callbackInfoPtr );			   
   return ( returnCode);
}   

/*==============================================================================*/
/*!
 * @function bspTwl3029_Rtc_timerIntDisable
 *
 * @discussion
 *    disables the interrupt for the periodic timer.
 *
 *
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_timerIntDisable( BspTwl3029_I2C_CallbackPtr callbackInfoPtr)
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegData regData = 0;   
   
   

    /* Setup the callback function */         
    bspTwl3029_Rtc_timerCallback = NULL;
    
    /* disble the interrupt */	  
    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_RTC,BSP_TWL_3029_MAP_RTC_RTC_INTERRUPTS_REG_OFFSET,&regData );
   
    /* disble the interrupt */
    BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_RTC_RTC_INTERRUPTS_REG_IT_TIMER, &regData,
	                      BSP_TWL3029_RTC_INTERRUPTS_IT_TIMER_DISABLED);
   
    returnCode =  BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_INTERRUPTS_REG_OFFSET, 
                                             regData,callbackInfoPtr );			   
   return ( returnCode);
}   

////////////

/*=============================================================================
 * Description:
 *   Rounds the RTC time to the nearest minute.
 */
BspTwl3029_ReturnCode 
bspTwl3029_Rtc_rounding30s(BspTwl3029_I2C_CallbackPtr callbackInfoPtr)
{
   BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
   BspTwl3029_I2C_RegData regData = 0;   

   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_CTRL_REG,&regData);
   /* first clear SET_32_COUNTER bit ( bit 5)  ( if set in shadow reg )*/
   regData &= 0xdf;
   regData |= BSP_TWL3029_RTC_ROUND_30S;
   
   returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_CTRL_REG,regData,callbackInfoPtr ); 
   return  (returnCode); 
}

/*=============================================================================
 * Description:
 *   Sets the hour mode in RTC registers.
 */
BspTwl3029_ReturnCode
bspTwl3029_Rtc_set12HourMode(BspTwl3029_I2C_CallbackPtr callbackInfoPtr, BspTwl3029_Rtc_HourMode mode12Hour)
{
   BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
   BspTwl3029_I2C_RegData regData = 0;   

   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_CTRL_REG, &regData );

   /* first clear ROUND_30s bit (bit 1)   and SET_32_COUNTER bit ( bit 5)  ( if set in shadow reg )*/
   regData &= 0xdd;
  
  if ( mode12Hour == BSPTWL3029_RTC_24_HOURMODE)
     regData &= 0xf7;
  else
     regData |= 0x08;

   returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_CTRL_REG,regData,callbackInfoPtr );
   return (returnCode);
}

/*=============================================================================
 * Description:
 *   Gets the current hour mode.
 */
Uint8 bspTwl3029_Rtc_Is12HourMode(void)
{
   BspTwl3029_I2C_RegData regData = 0;   

   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_RTC, BSP_TWL3029_RTC_CTRL_REG,&regData);  
   if( regData & BSP_TWL3029_RTC_MODE_12_24)
      return BSPTWL3029_RTC_12_HOURMODE;
   else
      return BSPTWL3029_RTC_24_HOURMODE;
	   
}



/*=============================================================================
 * Description:
 *   Gauging complete interrupt handler.
 */
void bspTwl3029_Rtc_intGauging(void)
{
    /* Number of 32 Khz clock at the end of the gauging */
   /*bspTwl3029_Rtc_32khz =  ((*(volatile Uint16 *)ULDP_COUNTER_32_MSB_REG) * 0x10000) +
             (*(volatile Uint16 *)ULDP_COUNTER_32_LSB_REG);     */

   /* Number of high frequency clock at the end of the gauging */
   /* To convert in nbr of 13 Mhz clocks (5*13=65Mhz) */
   /*bspTwl3029_Rtc_hf =   ( ((*(volatile Uint16 *)ULDP_COUNTER_HI_FREQ_MSB_REG) * 0x10000) +
            (*(volatile Uint16 *)ULDP_COUNTER_HI_FREQ_LSB_REG) );*/
}


/*=============================================================================
 * 
 *   LOCOSTO wrapper functions
 *  
 */
 
 
 /*=============================================================================
 * Description:
 *   Initialization function for RTC module.
 */
Int8 bspTwl3029_Rtc_Initialize (void)
{
   if( BSP_TWL3029_RETURN_CODE_FAILURE == bspTwl3029_Rtc_init())
       return BSP_TWL3029_RETURN_CODE_FAILURE;
   
   /* 
   //set minute timer alarm- 
   //- disabled to avoid system wakeup every 1 minute. it will consume lot of power*/

   /*
  
   if(BSP_TWL3029_RETURN_CODE_FAILURE == bspTwl3029_Rtc_timerIntEnable( NULL,
                            bspTwl3029_Rtc_timerMinuteCallback ,
                            BSP_TWL3029_RTC_INTERRUPTS_EVERY_MINUTE))
   {
	   return BSP_TWL3029_RETURN_CODE_FAILURE;
   }
   
   */
   
  return BSP_TWL3029_RETURN_CODE_SUCCESS;
}
/*=============================================================================
 * Description:
 *   Indicate if a RTC reset occured.
 */
Uint8 bspTwl3029_Rtc_IsReset()
{
   Uint8  resetStatus;
   bspTwl3029_Rtc_isReset (NULL, &resetStatus);
   return resetStatus;
}
/*=============================================================================
 * Description:
 *   Gets the current date and time.
 */
Int8 bspTwl3029_Rtc_GetDateTime (bspTwl3029_Rtc_DateTime* dateTime)
{
   return bspTwl3029_Rtc_timeGet( NULL, ( BspTwl3029_Rtc_Time *)dateTime ) ;  
}
/*=============================================================================
 * Description:
 *   Sets date and time into RTC registers.
 */
Int8 bspTwl3029_Rtc_SetDateTime (bspTwl3029_Rtc_DateTime dateTime)
{
   return bspTwl3029_Rtc_timeSet( NULL, ( BspTwl3029_Rtc_Time*)&dateTime ) ; 
}
/*=============================================================================
 * Description:
 *   Get the alarm date and time.
 */
Int8 bspTwl3029_Rtc_GetAlarm (bspTwl3029_Rtc_DateTime* dateTime) 
{
   return bspTwl3029_Rtc_alarmGet( ( BspTwl3029_Rtc_Time *)dateTime ) ; 
}  
/*=============================================================================
 * Description:
 *   Set alarm date and time into Alarm RTC registers.
 */
Int8 bspTwl3029_Rtc_SetAlarm (bspTwl3029_Rtc_DateTime dateTime, bspTwl3029_Rtc_Callback callbackFunction)
{
   return bspTwl3029_Rtc_alarmSet( NULL,(BspTwl3029_Rtc_EventCallback) callbackFunction,
                         (BspTwl3029_Rtc_Time*) &dateTime );
}
/*=============================================================================
 * Description:
 *   Disables the alarm interrupt.
 */
Int8 bspTwl3029_Rtc_UnsetAlarm ()
{
   return bspTwl3029_Rtc_alarmClear(NULL);
}
/*=============================================================================
 * Description:
 *   Rounds the RTC time to the nearest minute.
 */
void bspTwl3029_Rtc_Rounding30s()
{
   bspTwl3029_Rtc_rounding30s(NULL);
}
/*=============================================================================
 * Description:
 *   Sets the hour mode in RTC registers.
 */
void bspTwl3029_Rtc_Set12HourMode( BspTwl3029_Rtc_HourMode mode12Hour)
{
   bspTwl3029_Rtc_set12HourMode(NULL, mode12Hour); 
}
/*=============================================================================
 * Description:
 *   Sets the compensation value into drift compensation registers.
 */
Int8 bspTwl3029_Rtc_SetCompValue( Uint16 driftCompensation )
{
   return bspTwl3029_Rtc_calibrationSet( NULL,(BspTwl3029_Rtc_CalibrationValue) driftCompensation );
}			 





