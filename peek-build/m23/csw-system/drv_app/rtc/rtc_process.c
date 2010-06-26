/********************************************************************************/
/*                                                                              */
/*      File Name:   rtc_process.c                                              */
/*                                                                              */
/*      Purpose:   This file contains routine(s) that dispatch and process      */
/*               message(s) received from other entity                          */
/*                                                                              */
/*      Note:      None.                                                        */
/*                                                                              */
/*      Revision History:                                                       */
/*      03/22/01   Laurent Sollier      Create.                                 */
/*                                                                              */
/*   (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved  */
/*                                                                              */
/********************************************************************************/

#include "rvf/rvf_api.h"
#include "rv/rv_general.h"
#include "rvm/rvm_use_id_list.h"
#include "rtc/rtc_messages_i.h"
#include "rtc/rtc_i.h"


void rtc_process(T_RV_HDR * msg_ptr)
{
   switch (msg_ptr->msg_id)
   {
      case RTC_ALARM_EVT:   
         rvf_send_trace("RTC: received RTC_ALARM event",29, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_MEDIUM, RTC_USE_ID );

         RTC_ProcessAlarmEvent();      
         /* free memory used for the RTC message */
      break;

    default:
       /* Unknow message has been received */
       rvf_send_trace("RTC: Message received unknown",29,
                      NULL_PARAM,
                      RV_TRACE_LEVEL_ERROR,
                      RTC_USE_ID );
    break;
   }
}






