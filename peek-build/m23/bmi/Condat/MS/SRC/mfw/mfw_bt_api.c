/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      mfw_bt_api.c
*
*   DESCRIPTION:	This file implements the API between BMI (MMI) and BT applications
*
*   AUTHOR:         Anandhi Ramesh
*
*	Rev 0.1
*
\*******************************************************************************/
/* ========================================================== */
/*===========================================================
*!
*! Revision History
*! ===================================

	Nov 12, 2007 DRT :OMAPS00151698  x0056422
	Description: MM: Commands that are not recognized by MMI are
	handled improperly in BMI_NotifyBtEvent function. *!
*!
*! 17-Jan-2006 mf: Revisions appear in reverse chronological order;
*! that is, newest first.  The date format is dd-Mon-yyyy.
* =========================================================== */

/******************************************************************************  
                                Include Files
*******************************************************************************/
#ifdef FF_MMI_A2DP_AVRCP
#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif 


#include "mfw_mfw.h"
#include "mfw_phb.h"
#include "mfw_phbi.h"
#include "mfw_cm.h"
#include "mfw_cmi.h"
#include "mfw_kbd.h"

#include "mfw_nm.h"
#include "mfw_sim.h"
#include "mfw_sima.h"
#include "mfw_nmi.h"
#include "mfw_simi.h"
#include "mfw_sms.h"
#include "mfw_smsi.h"
#include "mfw_win.h"

#include "ksd.h"
#include "psa.h"

#include "mfw_bt_api.h"
#include "mfw_bt_private.h"

/*Declaration of Global BT structure for BMI*/
BMI_BT_STRUCTTYPE tGlobalBmiBtStruct; 

extern BOOL mmi_progress_bar;
/* temp comment */
extern T_AS_RET as_bt_cfg(BOOL , const T_RV_RETURN_PATH* );
/* ========================================================================== */
/**
* @fn BMI_SwitchAudioToBtHeadset : Sets BT connection status.
*     Connect or disconnect the audio output to BT headset.
*      BT will call this function to let BMI know an A2DP link to a wireless headset
*      has been connected (and is in Open state), or disconnected. 
* 
*      When link is connected, BMI will decide whether it wants to 
*      direct the audio output to the wireless headset.
*
* @param [in] connected
*        connected [in] - whether the link has just been connected or disconnected:
*                       TRUE - connected
*                       FALSE - disconnected
*
* @return BOOL
*      when connect parameter is TRUE:
*        - return TRUE when BMI approves and will use A2DP headset as the audio output
*        - return FALSE when BMI rejects using A2DP headset as the audio output
*
*      when connect parameter is FALSE - returned value is ignored
*
*  @see mfw_bt_api.h
*/
/* ========================================================================== */

BOOL BMI_SwitchAudioToBtHeadset(BOOL connected)
{
        static U8 first_time = 1;
	TRACE_FUNCTION("BMI_SwitchAudioToBtHeadset");
	if(first_time == 1)
	    {
	        tGlobalBmiBtStruct.tCmdSrc = BMI_BT_COMMAND_NONE;
	        first_time = 0;
	    }
    if(connected == 0)
    {
	TRACE_FUNCTION("BMI_BT_Trace BMI_SwitchAudioToBtHeadset OFF");
    tGlobalBmiBtStruct.bConnected = BMI_BT_NOTCONNECTED;
	}
    else
	{
        	TRACE_FUNCTION("BMI_BT_Trace BMI_SwitchAudioToBtHeadset ON");
    tGlobalBmiBtStruct.bConnected = BMI_BT_CONNECTED;
	}
    
	//send the status to AS layer. 
    as_bt_cfg(connected, NULL);
    return connected;
}
/* ========================================================================== */
/**
* @fn BMI_NotifyBtEvent : Informs BMI of any events triggered by the BT headset.
* @Description : This function is called by BT to notify MMI of any events in the headset. 
* MMI translates these events to corresponding keypad events and posts the event to itself. 
*
* @param [in] BtAppEvent event
*     The events that are recognised by MMI are
*     BT_EVENT_USER_PLAY - user has pressed play on handset, or resume. 
*     BT_EVENT_USER_STOP - user has pressed stop on handset. 
*     BT_EVENT_USER_PAUSE - user has pressed pause on handset. 
*
* @return none
*
*  @see  mfw_bt_api.h
*/
/* ========================================================================== */

void BMI_NotifyBtEvent(BtAppEvent event)
{
    U8 key_state, key_code;
    int nPresses;
   TRACE_FUNCTION("BMI_BT_Trace BMI_NotifyBtEvent Enter");
    if(tGlobalBmiBtStruct.bConnected != BMI_BT_NOTCONNECTED)
    {
    key_state = KEY_STAT_PRS;
    
    	TRACE_FUNCTION("BMI_BT_Trace BMI_NotifyBtEvent - BT connected");
    switch(event)
    {
       case BT_EVENT_USER_PLAY:
		   TRACE_FUNCTION("Play");
        	if(tGlobalBmiBtStruct.tAudioState == FM_AUD_PAUSE 
        		&& mmi_progress_bar==1)
        		  key_code = 0x0D;
        	else if (tGlobalBmiBtStruct.tAudioState == FM_AUD_STOP || 
        		      tGlobalBmiBtStruct.tAudioState == FM_AUD_NONE)
        		key_code = 0x17;   
        	else 
        		goto EXIT;
        break;
        case BT_EVENT_USER_STOP:
			TRACE_FUNCTION("Stop");
        	if(tGlobalBmiBtStruct.tAudioState == FM_AUD_PLAY 
        		&& mmi_progress_bar==0)
        		  key_code = 0x17;
        	else if(tGlobalBmiBtStruct.tAudioState != FM_AUD_NONE
        		&& mmi_progress_bar==1)
        		  key_code = 0x0E;
        	else 
        		goto EXIT;
        break;
        case BT_EVENT_USER_PAUSE:
			TRACE_FUNCTION("Pause");
        	if(tGlobalBmiBtStruct.tAudioState == FM_AUD_PLAY 
        		&& mmi_progress_bar == 1 )
        		key_code = 0x0D;
        	else 
        		goto EXIT;
        break;  

		/* OMAPS00151698  x0056422 */
	    default :
			goto EXIT;
        break;  
    }

    //set the indication that this command came from BT and not handset
    tGlobalBmiBtStruct.tCmdSrc = BMI_BT_HEADSET_COMMAND;

    //<post a keypad indication message to the MMI> 
	//Add keypress to queue of key presses
	nPresses = kbd_getNumElements();
	kbd_putMakeAndKey( (char)key_state, (char)key_code);
	/*also send a key release indicaiton for the same key. 
       If this is not done, the key press becomes a long press and is
       handled differently */
	key_state = KEY_STAT_REL;
	kbd_putMakeAndKey( (char)key_state, (char)key_code);
	if ((!kbd_stillProcessingKeys()) && (nPresses == 0))
	{	//Only this element present - create and send message.

	    sendKeyInd(key_code, KEY_STAT_PRS, 0);
	}
	if ((!kbd_stillProcessingKeys()) && (nPresses == 0))
	{	//Only this element present - create and send message.

	    sendKeyInd(key_code, KEY_STAT_REL, 0);
	}
    }
EXIT:
    return;
}

/* ========================================================================== */
/**
* @fn BMI_RegisterEventCallback : BT calls this function to register a callback with MMI
* Whenever there is a 
*
* @param [in] BtAppEvent event
*     The events that are recognised by MMI are
*     BT_EVENT_USER_PLAY - user has pressed play on handset, or resume. 
*     BT_EVENT_USER_STOP - user has pressed stop on handset. 
*     BT_EVENT_USER_PAUSE - user has pressed pause on handset. 
*
* @return none
*
*  @see  mfw_bt_api.h
*/
/* ========================================================================== */
void BMI_RegisterEventCallback(BmiEventCallback eventCallback)
{
    TRACE_FUNCTION("BMI_RegisterEventCallback");
    tGlobalBmiBtStruct.BmiEventCallback = eventCallback;
    return;
}
#endif //FF_MMI_A2DP_AVRCP

