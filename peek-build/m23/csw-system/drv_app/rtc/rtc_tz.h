/********************************************************************************/
/*                                                                            */
/*      File Name:   rtc_tz.h                                         */
/*                            							  */
/*      Purpose:   This file contains prototypes of RTC's functions related to TimeZone. */
/*                                                                            */
/*      Note:      None.                                                     */
/*                                                                              */
/*      Revision History:                                                       */
/*      14/Jun/04   Sumit Saraogi      Create.                                 */
/*                                                                              */
/*   (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved  */
/*                                                                              */
/********************************************************************************/

#ifndef _RTC_TZ_H_
#define _RTC_TZ_H_


/********************************************************************************/
/*                                                                              */
/*   Generic functions declarations.                                            */
/*                                                                              */
/********************************************************************************/


/* DEFAULT time zones */

#define RTC_TZR_DEFAULT_HOMETZ RTC_GMT_PT_0000 /* Default home time zone being initialized to GMT +0:00  */
#define RTC_TZR_DEFAULT_CURRENTTZ RTC_TZR_DEFAULT_HOMETZ /* Default current time zone being initialized to Home time zone. */ 

/* Enum containing GMT   */
typedef enum { 
RTC_GMT_NT_1200 = -48, /* Read as GMT -12:00 hrs */
RTC_GMT_NT_1145 = -47, /* Read as GMT -11:45 hrs */
RTC_GMT_NT_1130 = -46,
RTC_GMT_NT_1115 = -45,
RTC_GMT_NT_1100 = -44,
RTC_GMT_NT_1045 = -43,
RTC_GMT_NT_1030 = -42,
RTC_GMT_NT_1015 = -41,
RTC_GMT_NT_1000 = -40,
RTC_GMT_NT_0945 = -39,
RTC_GMT_NT_0930 = -38,
RTC_GMT_NT_0915 = -37,
RTC_GMT_NT_0900 = -36,
RTC_GMT_NT_0845 = -35,
RTC_GMT_NT_0830 = -34,
RTC_GMT_NT_0815 = -33,
RTC_GMT_NT_0800 = -32,
RTC_GMT_NT_0745 = -31,
RTC_GMT_NT_0730 = -30,
RTC_GMT_NT_0715 = -29,
RTC_GMT_NT_0700 = -28,
RTC_GMT_NT_0645 = -27,
RTC_GMT_NT_0630 = -26,
RTC_GMT_NT_0615 = -25,
RTC_GMT_NT_0600 = -24,
RTC_GMT_NT_0545 = -23,
RTC_GMT_NT_0530 = -22,
RTC_GMT_NT_0515 = -21,
RTC_GMT_NT_0500 = -20,
RTC_GMT_NT_0445 = -19,
RTC_GMT_NT_0430 = -18,
RTC_GMT_NT_0415 = -17,
RTC_GMT_NT_0400 = -16,
RTC_GMT_NT_0345 = -15,
RTC_GMT_NT_0330 = -14,
RTC_GMT_NT_0315 = -13,
RTC_GMT_NT_0300 = -12,
RTC_GMT_NT_0245 = -11,
RTC_GMT_NT_0230 = -10,
RTC_GMT_NT_0215 = -9,
RTC_GMT_NT_0200 = -8,
RTC_GMT_NT_0145 = -7,
RTC_GMT_NT_0130 = -6,
RTC_GMT_NT_0115 = -5,
RTC_GMT_NT_0100 = -4,
RTC_GMT_NT_0045 = -3,
RTC_GMT_NT_0030 = -2,
RTC_GMT_NT_0015 = -1,

RTC_GMT_PT_0000 = 0, /* Read as GMT +0:00 */


RTC_GMT_PT_0015 = 1,
RTC_GMT_PT_0030 = 2,
RTC_GMT_PT_0045 = 3,
RTC_GMT_PT_0100 = 4,
RTC_GMT_PT_0115 = 5,
RTC_GMT_PT_0130 = 6,
RTC_GMT_PT_0145 = 7,
RTC_GMT_PT_0200 = 8,
RTC_GMT_PT_0215 = 9,
RTC_GMT_PT_0230 = 10,
RTC_GMT_PT_0245 = 11,
RTC_GMT_PT_0300 = 12,
RTC_GMT_PT_0315 = 13,
RTC_GMT_PT_0330 = 14,
RTC_GMT_PT_0345 = 15,
RTC_GMT_PT_0400 = 16,
RTC_GMT_PT_0415 = 17,
RTC_GMT_PT_0430 = 18,
RTC_GMT_PT_0445 = 19,
RTC_GMT_PT_0500 = 20,
RTC_GMT_PT_0515 = 21,
RTC_GMT_PT_0530 = 22,
RTC_GMT_PT_0545 = 23,
RTC_GMT_PT_0600 = 24,
RTC_GMT_PT_0615 = 25,
RTC_GMT_PT_0630 = 26,
RTC_GMT_PT_0645 = 27,
RTC_GMT_PT_0700 = 28,
RTC_GMT_PT_0715 = 29,
RTC_GMT_PT_0730 = 30,
RTC_GMT_PT_0745 = 31,
RTC_GMT_PT_0800 = 32,
RTC_GMT_PT_0815 = 33,
RTC_GMT_PT_0830 = 34,
RTC_GMT_PT_0845 = 35,
RTC_GMT_PT_0900 = 36,
RTC_GMT_PT_0915 = 37,
RTC_GMT_PT_0930 = 38,
RTC_GMT_PT_0945 = 39,
RTC_GMT_PT_1000 = 40,
RTC_GMT_PT_1015 = 41,
RTC_GMT_PT_1030 = 42,
RTC_GMT_PT_1045 = 43,
RTC_GMT_PT_1100 = 44,
RTC_GMT_PT_1115 = 45,
RTC_GMT_PT_1130 = 46,
RTC_GMT_PT_1145 = 47,
RTC_GMT_PT_1200 = 48, /* Read as GMT +12:00 */



RTC_TIME_ERROR = -100
} T_RTC_TZ;


/*******************************************************************************
 *
 *                               RTC_SetCurrentTZ
 * 
 * Purpose  : This Function will set the argument passed as 'Current Time Zone'. The function will 
 * update the time zone specific file present in the flash. if  the file is not present, the function wil
 * create the file. It will set homeTimezone value to RTC_TIME_ERROR.
 * 
 * Arguments: In : currentTimeZone
 *
 * Returns: 
 * RV_OK - The API function was successfully executed.
 * RV_INVALID_PARAMETER - Value passed is invalid.
 * RV_INTERNAL_ERR - Error is encountered while handling the file.
 *
 ******************************************************************************/

T_RV_RET RTC_SetCurrentTZ(T_RTC_TZ currentTimeZone);


/*******************************************************************************
 *
 *                               RTC_GetCurrentTZ
 * 
 * Purpose  : This Function will return the current time zone, the value for the same will be fetched 
 * from the flash file. 
 * 
 * Arguments: In : None
 * Out: current time zone would be returned.
 *
 * Returns: Value of current time zone will be returned. 
 * In case of any error RTC_TIME_ERROR will be returned. 
 * 
  ******************************************************************************/

T_RTC_TZ RTC_GetCurrentTZ(void);


/*******************************************************************************
 *
 *                               RTC_SetHomeTZ
 * 
 * Purpose  : This Function will set the argument passed as homeTimeZone.The function will
 * update the time zone specific file present in the flash. If the file is not present, the function
 * will create the file. It will set current time zone value to RTC_TIME_ERROR.
 *
 * Arguments: In : homeTimeZone
 *
 * Returns: 
 * RV_OK - The API function was successfully executed.
 * RV_INVALID_PARAMETER - Value passed is invalid.
 * RV_INTERNAL_ERR - Error is encountered while handling the file.
 *
 ******************************************************************************/

T_RV_RET RTC_SetHomeTZ(T_RTC_TZ homeTimeZone);


/*******************************************************************************
 *
 *                               RTC_GetHomeTZ
 * 
 * Purpose  : This Function will return the home time zone, the value for the same will be fetched 
 * from the flash file. 
 * 
 * Arguments: In : None
 * Out: home time zone would be returned.
 *
 * Returns: Value of home time zone will be returned. 
 * In case of any error RTC_GMT_TIME_ERROR will be returned. 
 *
 *
 ******************************************************************************/

T_RTC_TZ RTC_GetHomeTZ(void);


#endif /* #ifndef _RTC_TZ_H_ */
