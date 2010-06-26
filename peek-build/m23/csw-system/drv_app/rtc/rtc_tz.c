/********************************************************************************/
/*                                                                              */
/*      File Name:   rtc_tz.c                                                  */
/*                                                                              */
/*      Purpose:   This file contains routines that will be called in order     */
/*               to:                                                            */
/*               - notify the Riviera manager of the RTC's Memory               */
/*                 Banks requirements,                                          */
/*               - initialize all the RTC's data structures,                    */
/*                                                                              */
/*      Note:      None.                                                        */
/*                                                                              */
/*      Revision History:                                                       */
/*      14/Jun/04   Sumit Saraogi      Create.                                 */
/*                                                                              */
/*   (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved  */
/*                                                                              */
/********************************************************************************/

#include "rvm/rvm_priorities.h"
#include "rvm/rvm_use_id_list.h"
#include "rtc/rtc_env.h"
#include "rtc/rtc_i.h"
#include "rtc/rtc_api.h"

#include "rtc/rtc_tz.h"
#include "ffs/ffs_api.h"

/*RV_OK - The API function was successfully executed.
  *RV_INVALID_PARAMETER - parameter passed is not a valid time range. 
  *RV_INTERNAL_ERR - Error is encountered while handling the file.
  *RTC_TIME_ERROR will be set as home time zone value.
  */

T_RV_RET RTC_SetCurrentTZ(T_RTC_TZ currentTimeZone) {

    T_FFS_FD fd;
    T_RTC_TZ timezone;
    if (currentTimeZone<RTC_GMT_NT_1200 || currentTimeZone>RTC_GMT_PT_1200)
        return RV_INVALID_PARAMETER;

    fd =   ffs_open ("/sys/rtc_timezone_data", FFS_O_WRONLY);       

/* return RV_INTERNAL_ERR for any other error. */
    if (fd<0 && fd!=EFFS_NOTFOUND) return RV_INTERNAL_ERR;

/* if the file is not present, than create the file */  
    if (fd == EFFS_NOTFOUND) {

		fd =   ffs_open ("/sys/rtc_timezone_data", FFS_O_CREATE|FFS_O_RDWR);
		if (fd < 0) return RV_INTERNAL_ERR;       
		timezone = RTC_TIME_ERROR;
		if (ffs_write (fd, (void *)&timezone, sizeof(T_RTC_TZ))<0) return RV_INTERNAL_ERR;
	}
    else {
	/* if it comes here, file is already present */
    if (ffs_seek(fd, sizeof(T_RTC_TZ), FFS_SEEK_CUR)<0) return RV_INTERNAL_ERR;

    }		

    if (ffs_write(fd, (void *)&currentTimeZone, sizeof(T_RTC_TZ))<0) return RV_INTERNAL_ERR;
    if (ffs_close(fd)<0) return RV_INTERNAL_ERR;
    return RV_OK;
}


 
T_RTC_TZ RTC_GetCurrentTZ(void) {

    T_FFS_FD fd;
    T_RTC_TZ timezone;

    fd =  ffs_open ("/sys/rtc_timezone_data", FFS_O_RDONLY);
    if (fd<0) return RTC_TIME_ERROR;

    if (ffs_seek(fd, sizeof(T_RTC_TZ), FFS_SEEK_CUR)<0) return RTC_TIME_ERROR;
    if (ffs_read(fd, (void *)&timezone, sizeof(T_RTC_TZ))<0) return RTC_TIME_ERROR;
    if (ffs_close(fd)<0) return RTC_TIME_ERROR;
    return timezone;
}

/*
  */

T_RV_RET RTC_SetHomeTZ(T_RTC_TZ timeZone) {

    T_FFS_FD fd;
    int flag = 0;	
	
    if (timeZone<RTC_GMT_NT_1200 || timeZone>RTC_GMT_PT_1200)
        return RV_INVALID_PARAMETER;

    fd =   ffs_open ("/sys/rtc_timezone_data", FFS_O_WRONLY);       

/* return RV_INTERNAL_ERR for any other error. */
    if (fd<0 && fd!=EFFS_NOTFOUND) return RV_INTERNAL_ERR;

/* if the file is not present, than create the file */  
    if (fd == EFFS_NOTFOUND) {

		fd =   ffs_open ("/sys/rtc_timezone_data", FFS_O_CREATE|FFS_O_RDWR);
		if (fd < 0) return RV_INTERNAL_ERR;   
		flag = 1;
    	}
	
    if (ffs_write(fd, (void *)&timeZone, sizeof(T_RTC_TZ))<0) return RV_INTERNAL_ERR;

    if (flag) {	
		timeZone = RTC_TIME_ERROR;
		if (ffs_write (fd, (void *)&timeZone, sizeof(T_RTC_TZ))<0) return RV_INTERNAL_ERR;
	};
    
    if (ffs_close(fd)<0) return RV_INTERNAL_ERR;
    return RV_OK;
}


 
T_RTC_TZ RTC_GetHomeTZ(void) {

    T_FFS_FD fd;
    T_RTC_TZ timezone;

    fd =  ffs_open ("/sys/rtc_timezone_data", FFS_O_RDONLY);
    if (fd<0) return RTC_TIME_ERROR;

    if (ffs_read(fd, (void *)&timezone, sizeof(T_RTC_TZ))<0) return RTC_TIME_ERROR;
    if (ffs_close(fd)<0) return RTC_TIME_ERROR;
    return timezone;
}

