/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI                                                            
 $Module:		SMS
 $File:		    MmiSmsIdle.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description: Implementation for MMI smsidle unsolicited event handling
 
   
                        
********************************************************************************

 $History: MmiSmsIdle.c

	25/10/00			Original Condat(UK) BMI version.

      May 7, 2007 OMAPS00130278 a0393213(R.Prabakar)
      Description : CT-GCF[34.2.5.1] - MS fails to display class 0 MT SMS when it contains alphanumeric and special characters
      Solution     : Decoding of DCS was not done according to GSM spec
            
      Feb 28, 2007 OMAPS00109953 a0393213(R.Prabakar)
      Description : Need to be able to store SMS in SIM/ME
      Solution     : Earlier, index alone was sufficient to identify messages. Now memory should also be taken care of.

	Sep 04, 2006    REF:OMAPS00092060  x0039928
	Description:   COMBO : MIDI ringer not played for incoming alarm or SMS during voice call on N5.16
	Solution: A tone is played for an incoming SMS during voice call.

	July 06, 2006    REF:OMAPS00084767  x0039928
	Description:   Locosto N5.14/N12.9- Configuration without MIDI does not build
	Solution: extern T_call call_dat is moved out of FF_MIDI_RINGER flag
	
	May 10, 2006    DR: OMAPS00075379 x0021334
	Descripton: SMS  - Status Request in idle mode - using Menu - no status in pending state
	Solution: The display of pending status is network dependent. In some networks this
	status is not given. When available, it used to display 'Temp Error'. This has now been
	replaced with 'Pending'.

	Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
	Description:   Mp3 Ringer Implementation
	Solution: The Midi Ringer application has been enhanced to support mp3 as ringer.
	The interface  function names have been modified.
	
	May 27, 2005    MMI-FIX-29869 x0018858
   	Description: There is a limitation on the number of messages that can be downloaded from 
   	server even though there is space on the sim.
   	Solution: The limitation has been removed and the number of messages that can be dowloaded
   	has been modified to the maximum number that can be stored.

	Jun 14, 2005 REF: MMI-FIX-30439 x0018858
   	Description: The homezone/cityzone tags were not being displayed properly.
   	Solution: Modified to save the tags properly in caches and also made the 
	appropriate modifications to update the same properly during mobility.	

	Apr 05, 2005    REF: ENH 29994 xdeepadh
   	Description: Implementation of Generic Midi Ringer and Modification of existing Midi Test Application
   	Solution: Generic Midi Ringer and Midi Test Application were implemented. 

	x0018858 24 Mar, 2005 MMI-FIX-11321
	Issue Description :- If the inbox is full and a new message is receivd, no indication is received.
	Fix: Added support for handling indication when message box is full and
	there is a pending message.

     	Feb 16, 2005 REF: CRR MMI-SPR-27900 xnkulkar 
  	Issue description:  BMI: using the tool nowSMS the voicemail indication via sms does not work
  	Solution: Instead of identifying voice using addr[0] and pid, we use dcs. Also, messages belonging
  	              to group "1100" are deleted when voicemail icon is displayed.
	
	Apr 27, 2004    REF: CRR 21336  Deepa M.D 
	Bug:SMSMT UCS2 Class0 message content not displayed.
	Fix:The alphabet bits(UCS2) of DCS were not decoded properly.
	   
 $End

*******************************************************************************/

#define ENTITY_MFW

/* includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

#include "gdi.h"
#include "audio.h"


#include "prim.h"


#include "mfw_sys.h"

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_lng.h"
#include "mfw_icn.h"
#include "mfw_phb.h"
#include "mfw_sim.h"
#include "mfw_nm.h"

#include "ksd.h"
#include "psa.h"
#include "mfw_sms.h"
#include "mfw_smsi.h"
#include "mfw_mnu.h"
#include "mfw_sat.h"
#include "mfw_tim.h"
#include "mfw_mme.h"

#include "dspl.h"

#include "MmiMmi.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"

#include "MmiMain.h"
#include "MmiStart.h"
#include "MmiPins.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "MmiSounds.h"
#include "MmiCall.h"

#include "MmiBookController.h"


#include "MmiIdle.h"
#include "Mmiicons.h"
#include "mmiSmsIdle.h"
#include "mmiSmsBroadcast.h"

/* SPR759 - SH */
#ifdef MMI_HOMEZONE_ENABLED
#include "MmiHomezone.h"
#endif
#include "mmiSmsRead.h"  /*SPR 2512*/
#include "cus_aci.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


// Feb 16, 2005 REF: CRR MMI-SPR-27900 xnkulkar 
extern BOOL invalidVm; 
//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifdef FF_MIDI_RINGER  
#include "mfw_midi.h"
#endif

/* SMS arrival Setting */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
int iSmsArrival = 0;
#endif
#endif

/*********************************************************************
**********************************************************************

				 STATIC MENU WINDOW. DECLARATION

*********************************************************************
**********************************************************************/

typedef struct
{
    /* administrative data */

    T_MMI_CONTROL   mmi_control;
    T_MFW_HND       win;
    SHORT           id;
    T_MFW_HND       parent;

    /* associated handlers */

    T_MFW_HND       sms;

    /* internal data */

    BOOL sms_initialised;
    BOOL sms_store_free;
} T_smsidle;
#ifdef NEPTUNE_BOARD
extern T_call call_data;
#else
// July 06, 2006    REF:OMAPS00084767  x0039928
extern T_call call_data;
#endif

/*********************************************************************

						External Function Prototypes

**********************************************************************/						
extern UBYTE getcurrentSMSTone(void);
extern UBYTE getcurrentVoiceTone(void);
extern UBYTE Delete_1_2_1_vm_icon;
extern UBYTE Display_1_2_1_vm_icon;
/* cq15633 Access function returning incoming call alert window status. 23-03-04. MZ */
extern BOOL  call_IsAlertScreenActive( void );
//	May 27, 2005    MMI-FIX-29869 x0018858 - Extern function 
#ifdef FF_MMI_SMS_DYNAMIC
extern Check_max_sms_allowed(void);
#endif
/*********************************************************************

						Function Prototypes
						
**********************************************************************/
// ADDED BY RAVI - 28-11-2005
EXTERN UBYTE getCurrentRingerSettings(void);
extern UBYTE sms_check_message_pending(void);
// END RAVI - 28-11-2005
 
static SHORT smsidle_get_new_cb_messages_count(void);
static T_smsidle smsidle_data;
static int smsidle_sr_status_to_text_id(UBYTE status); /* Marcus: Issue 1170: 07/10/2002 */


// Used to count the number of deleted messages in delete all
SHORT g_ListLength2;
static char status_report[160]; /* Marcus: Issue 1170: 07/10/2002 */

static T_MFW_HND smsidle_create (T_MFW_HND parent);
static void smsidle_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
int smsidle_sms_cb (MfwEvt e, MfwWin *w);

/*******************************************************************************

 $Function:    	smsidle_start

 $Description:	This function will be call once.It is create a static sms handle
 
 $Returns:		mfw window handler 

 $Arguments:	parent_window - Parent window handler
 				param - optional data
 
*******************************************************************************/

T_MFW_HND smsidle_start(T_MFW_HND parent_window, void *param)
{
    T_MFW_HND win;

    TRACE_FUNCTION ("smsidle_start()");

    win = smsidle_create (parent_window);

    if (win NEQ NULL)
	{
	    SEND_EVENT (win, E_INIT, 0, NULL);
	}
    return win;
}

/*******************************************************************************

 $Function:    	smsidle_create

 $Description:	Create sms Idle window
 
 $Returns:		mfw window handler 

 $Arguments:	parent_window - Parent window handler
 
*******************************************************************************/
static T_MFW_HND smsidle_create(MfwHnd parent_window)
{
    T_smsidle  * data = &smsidle_data;
    T_MFW_WIN  * win;

    TRACE_FUNCTION ("smsidle_create()");

    /*
     * Create window handler
     */

    if (data->win EQ NULL)
	{
	    data->win = 
		win_create (parent_window, 0, 0, NULL);
	    if (data->win EQ NULL)
		{
		    return NULL;
		}
	}
    else
	{
	    TRACE_EVENT ("smsidle_create() called twice");
	    return NULL;
	}

    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)smsidle_exec_cb;
    data->mmi_control.data   = data;
    win                      = (T_MFW_WIN *)((T_MFW_HDR *)data->win)->data;
    win->user                = (void *)data;
    data->parent             = parent_window;

    /*
     * return window handle
     */

    win_show(data->win);
    return data->win;
}

/*******************************************************************************

 $Function:    	smsidle_destroy

 $Description:	Destroy the sms Idle window.
 
 $Returns:		none

 $Arguments:	none
 
*******************************************************************************/
void smsidle_destroy(void)
{
    T_MFW_WIN     * win_data;
    MfwHnd          own_window;
    T_smsidle     * data;

    own_window = smsidle_data.win;

    TRACE_FUNCTION ("smsidle_destroy()");

    if (own_window)
	{
	    win_data = (T_MFW_WIN *)((T_MFW_HDR *)own_window)->data;
	    data = (T_smsidle *)win_data->user;

	    if (data)
		{
		    /*
		     * Delete WIN handler
		     */ 
		    win_delete (data->win);

		    /*     
		     * Free Memory
		     */
		    /* Memory is static! */
		}
	    else
		{
		    TRACE_EVENT ("smsidle_destroy() called twice");
		    return;
		}
	}
}



/*******************************************************************************

 $Function:    	smsidle_exec_cb

 $Description:	Call back function for the sms Idle window.
 
 $Returns:		none

 $Arguments:	win - current window
 				event - window event
 				value - unique id
 				parameter - pointer to optional data.
 
*******************************************************************************/

void smsidle_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
    /* callback handler for events sent to to trigger execution */
{
    T_MFW_WIN      * win_data = (T_MFW_WIN *)((T_MFW_HDR *) win)->data;
    T_smsidle      * data     = (T_smsidle *)win_data->user;
    T_MFW_EVENT    smsEvents;

    TRACE_FUNCTION ("smsidle_exec_cb()");

    switch (event)
	{
	case E_INIT:
	    /* initialization of administrative data */
	    data->id = value;

	    /* initialization of the dialog data */

	    sms_init(); /* startup sms in MFW */
	    
	    smsEvents = 
			      E_SMS_MO_AVAIL | E_SMS_CMD_AVAIL | E_SMS_SAVE_AVAIL
				| E_SMS_MT       | E_SMS_MO        | E_SMS_MT_RECEIVED | E_SMS_STATUS
				| E_SMS_MEM      | E_SMS_OK        | E_SMS_ERR         | E_SMS_BUSY   
				| E_SMS_MEM_FULL | E_SMS_MEM_FREE  | E_SMS_RECEIVED    | E_SMS_CB
				| E_SMS_READY    | E_SMS_CB_RECEIVED;

		
	    data->sms = sms_create(win, smsEvents, (T_MFW_CB) smsidle_sms_cb);

        /* "sms_initialised"  indicates that the SMS is ready or not */
        /*api - 10-05-02 - Set To True so the SMS can be recieved (Event not Generated) */
	    data->sms_initialised  = FALSE;	/* SMS not ready yet */

	    /* "sms_store_free"  indicates that the SMS Memory is full or not */
        /* Set default to True !*/
	    data->sms_store_free   = TRUE; // ??? 

	    break;
	case E_BACK:
	case E_ABORT:
	case E_EXIT:
	    smsidle_destroy(); /* there's no default reaction */
	    sms_delete(data->sms);
	    sms_exit(); /* ??? implementation of routine is actually empty */
	    break;
	default:
	    TRACE_EVENT("unhandled event in smsidle_exec_cb()");
	    return;
	}
}
/*******************************************************************************

 $Function:    	smsidle_message_to_text

 $Description:	function to format message information into a text string. SPR2363
 
 $Returns:		none

 $Arguments:	message data structure, text buffer(should be at least MAX_EDITOR_LEN 
 				chars long)
 
*******************************************************************************/

void smsidle_message_to_text(T_MFW_SMS_MT* SmsMt, char* text_buffer)
{
	T_MFW_PHB_ENTRY pb_entry;
//	UBYTE input_type, output_type;   // RAVI
	char sms_text_buffer[STANDARD_EDITOR_SIZE];
	UBYTE index=0;
//	UBYTE msg_len;   // RAVI
	T_ATB_TEXT main_text, temp_text;
	char msg_buffer[MAX_MSG_LEN_ARRAY]; /*msg buffer which is used while converting 7-bit encoded system to ASCII*/


	/*clear buffers*/
	memset(sms_text_buffer, 0, STANDARD_EDITOR_SIZE);
	memset(text_buffer, 0, MAX_EDITOR_LEN);



	
			
/*, if new editor use nice string-handling functions*/

			main_text.data = (UBYTE*)text_buffer;
/*set up text data stucture for main display string*/
			if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
				main_text.dcs = ATB_DCS_UNICODE;
			else
				main_text.dcs = ATB_DCS_ASCII;
			ATB_string_Length(&main_text);  		


/*SPR 2539, if new editor use nice string-handling functions*/
		/*see if the sender is in the phonebook*/
		if (bookFindNameInPhonebook((const char*)SmsMt->orig_addr.number, &pb_entry) !=0)
		{	char temp_string[PHB_MAX_LEN];

				memset(temp_string, 0, PHB_MAX_LEN);
				/*copy name into a text data structure*/
				if ((pb_entry.name.dcs&0xC0) == MFW_DCS_UCS2 ||pb_entry.name.data[0] == 0x80 )
					temp_text.dcs = ATB_DCS_UNICODE;
				else
					temp_text.dcs = ATB_DCS_ASCII;
				if (temp_text.dcs != ATB_DCS_UNICODE)
				{	
					temp_text.data = (UBYTE*)pb_entry.name.data;	
				}
				
				else
				{	pb_entry.name.len-=2;
					memcpy(temp_string, &pb_entry.name.data[2], pb_entry.name.len);
					temp_text.data = (UBYTE*)temp_string;
				}
				ATB_string_Length(&temp_text);
				/*add name text into main display string*/
				ATB_string_Concat(&main_text, &temp_text);
		}



					sms_text_buffer[index++]='\n';
				
					sms_text_buffer[index++]='0'+SmsMt->sctp.day[0];
					sms_text_buffer[index++]='0'+SmsMt->sctp.day[1];
					sms_text_buffer[index++]='/';
					sms_text_buffer[index++]='0'+SmsMt->sctp.month[0];
					sms_text_buffer[index++]='0'+SmsMt->sctp.month[1];
					sms_text_buffer[index++]='/';
					sms_text_buffer[index++]='0'+SmsMt->sctp.year[0];
					sms_text_buffer[index++]='0'+SmsMt->sctp.year[1];
					sms_text_buffer[index++]=',';
					sms_text_buffer[index++]='\n';
					
					sms_text_buffer[index++] ='0'+SmsMt->sctp.hour[0];
					sms_text_buffer[index++]='0'+SmsMt->sctp.hour[1];
					sms_text_buffer[index++]=':';
					sms_text_buffer[index++]='0'+SmsMt->sctp.minute[0];
					sms_text_buffer[index++]='0'+SmsMt->sctp.minute[1];
					sms_text_buffer[index++]='\n';	

/* if new editor use nice string-handling functions*/
				
				temp_text.dcs = ATB_DCS_ASCII;
				temp_text.data = (UBYTE*)sms_text_buffer;
				ATB_string_Length(&temp_text);
				ATB_string_Concat(&main_text, &temp_text);
				//	Apr 27, 2004    REF: CRR 21336 Deepa M.D 
				//Check for the  Alphabet bits(2nd and 3rd bits) in the DCS.(GSM 03.38 version 5.6.1,Section 4) 
				//Bit 3 and 2 are equal to 1 &0 indicates UCS2 coding
				/*a0393213(R.Prabakar) OMAPS00130278 - MS fails to display class 0 MT SMS when it contains alphanumeric and special characters.
				   DCS was not properly interpreted. Now its' done according to GSM 03.38 ver 7.2.0*/

				if((SmsMt->dcs & 0xC0) == 0x00)
					{
					switch(SmsMt->dcs & 0x0C)
						{
						case 0x08:
							temp_text.dcs = ATB_DCS_UNICODE;
							temp_text.data = (UBYTE*)SmsMt->sms_msg;
							break;
						case 0x04:
							temp_text.dcs=ATB_DCS_ASCII;							
							temp_text.data = (UBYTE*)SmsMt->sms_msg;
							break;
						case 0x00:
							{
								/*default alphabet (7 bit) - convert to ASCII*/				
								convertUnpacked7To8(SmsMt->sms_msg, SmsMt->msg_len, 
													msg_buffer, sizeof(msg_buffer));
								temp_text.dcs=ATB_DCS_ASCII;	
								temp_text.data = (UBYTE*)msg_buffer;
							}
							break;
						default:
							TRACE_ERROR("smsidle_message_to_text() : Unknown DCS");
						}
					
					}
				else if((SmsMt->dcs & 0xF0) == 0xF0)
					{
					switch(SmsMt->dcs & 0x04)
						{
						case 0x04:
							temp_text.dcs=ATB_DCS_ASCII;	
							temp_text.data = (UBYTE*)SmsMt->sms_msg;							
							break;
						case 0x00:
							{
								/*default alphabet (7 bit) - convert to ASCII*/				
								convertUnpacked7To8(SmsMt->sms_msg, SmsMt->msg_len, 
													msg_buffer, sizeof(msg_buffer));
								temp_text.dcs=ATB_DCS_ASCII;	
								temp_text.data = (UBYTE*)msg_buffer;
							}	
							break;
						default:
							TRACE_ERROR("smsidle_message_to_text() : Unknown DCS");
		
						}
					}
				else
					{
					TRACE_ERROR("smsidle_message_to_text() : Unknown DCS");
					}
				
				/*if ((SmsMt->dcs&0x0C) == MFW_DCS_UCS2)
					temp_text.dcs = ATB_DCS_UNICODE;
				else
					temp_text.dcs = ATB_DCS_ASCII;*/

				ATB_string_Length(&temp_text);
				/*Add date and time to main display string*/
				ATB_string_Concat(&main_text, &temp_text);
				


	}
			

/*******************************************************************************

 $Function:    	smsidle_sms_cb

 $Description:	Call back function for the sms Idle window.
 
 $Returns:		none

 $Arguments:	event - window event
 				w - current window 
*******************************************************************************/
int smsidle_sms_cb (MfwEvt event, MfwWin *w)
    /* smsidle's sms event handler */
{
    T_MFW_HND    win      = mfwParent((MfwHdr *)mfw_header());
    T_MFW_WIN  * win_data = (T_MFW_WIN *)((T_MFW_HDR *)win)->data;
    T_smsidle  * data     = (T_smsidle *)win_data->user;
	/*cq 15633 get current ringer settings */
	UBYTE currentRinger = getCurrentRingerSettings();

	/*JVJ SPR1298 Converted to dynamic variable */
	//May 27, 2005    MMI-FIX-29869 x0018858
#ifdef FF_MMI_SMS_DYNAMIC
	T_MFW_SMS_IDX* msg_list;
	T_MFW_SMS_IDX* urd_msg_list;
#else
    T_MFW_SMS_IDX* msg_list= (T_MFW_SMS_IDX*)ALLOC_MEMORY(MAX_MESSAGES*sizeof(T_MFW_SMS_IDX));
    T_MFW_SMS_IDX* urd_msg_list=  (T_MFW_SMS_IDX*)ALLOC_MEMORY(MAX_MESSAGES*sizeof(T_MFW_SMS_IDX));
#endif
	SHORT urd_count;
//	UBYTE alphabet;    // RAVI
    /*JVJ SPR1298 Converted to dynamic variable */
    char* temp_buffer = NULL;
    int result = MFW_EVENT_CONSUMED;

	//May 27, 2005    MMI-FIX-29869 x0018858
#ifdef FF_MMI_SMS_DYNAMIC
	Check_max_sms_allowed();

	msg_list = (T_MFW_SMS_IDX*)ALLOC_MEMORY(g_max_messages*sizeof(T_MFW_SMS_IDX));
	urd_msg_list = (T_MFW_SMS_IDX*)ALLOC_MEMORY(g_max_messages*sizeof(T_MFW_SMS_IDX));	
#endif

    TRACE_EVENT ("smsidle_sms_cb()");

    switch (event)
	{
	case E_SMS_READY: 
	    TRACE_EVENT ("E_SMS_READY");
		/* now sms is now ready for using */
	    data->sms_initialised = TRUE;	   
  		sms_set_mt_ind(MT_IND_IDX); 	/* MMI-FIX-17223. Needed for class 0 messages not to be stored in SIM */  
	    	sms_cb_init();    /*SPR 2470  initialise cell broadcast now SMS has initialised*/
		smscb_getCBsettings();  /* Get CB info from SIM or phone */
#ifdef MMI_HOMEZONE_ENABLED
		//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
		//Commented the function call,
		//homezoneActivateCB();	/* SPR759 - SH - Activate Cell Broadcast channel for homezone */
#endif
   		
	    break;
	case E_SMS_MT_RECEIVED: /* new SMS received, automatically saved     */
	case E_SMS_MT:          /* new SMS received, NOT automatically saved */
	{

	     /*SPR#1408 - DS - Take the SMS data from MFW. Port of 1669 */
	    T_MFW_SMS_MT *Message = (T_MFW_SMS_MT *) w;

		// check for SMS indicating voice mail.
	//May 27, 2005    MMI-FIX-29869 x0018858
#ifdef FF_MMI_SMS_DYNAMIC
		urd_count = sms_index_list(MFW_SMS_UNREAD,urd_msg_list,g_max_messages);
		
		sms_index_list(MFW_SMS_VOICE,msg_list,g_max_messages);
#else
		urd_count=sms_index_list(MFW_SMS_UNREAD,urd_msg_list,MAX_MESSAGES);
		
		sms_index_list(MFW_SMS_VOICE,msg_list,MAX_MESSAGES);
#endif
        TRACE_EVENT_P4("urd_count %d urd_msg_list-> index %d stat %d msg_type %d",
                        urd_count,urd_msg_list[0].index,urd_msg_list[0].stat,urd_msg_list[0].msg_type);

        TRACE_EVENT_P2("msg_type: %d, Message->voicemail: %d", msg_list[0].msg_type, Message->voicemail);
        
		//Check SMS indicating voice mail is the received SMS, by checking index is the same.
		/*SPR#1408 - DS - Added check for voicemail flag. 
		  *For E_SMS_MT_RECEIVED messages the MFW flags the message as a voicemail by setting the 
		  *msg_type to MFW_SMS_VOICE. For E_SMS_MT messages the MFW flags as a voicemail by
		  *setting a boolean in the SMS MT structure. Port of 1669 (then modified).
		  */
		if ( ( (event == E_SMS_MT_RECEIVED) && (msg_list[0].msg_type == MFW_SMS_VOICE) ) || 
		    ( (event == E_SMS_MT) && (Message->voicemail == TRUE) ) )
		{	
			if(iconsGetState(iconIdVoice) == ICON_VISIBLE || (Delete_1_2_1_vm_icon == TRUE))
			{
				if (iconsGetState(iconIdVoice) == ICON_VISIBLE && Display_1_2_1_vm_icon == TRUE)
				{
					// one-2-one only functionality, do nothing as icon is already being displayed.
					break;
				}
				else 
				{
					Delete_1_2_1_vm_icon  = FALSE;
					Display_1_2_1_vm_icon = FALSE;
					iconsDeleteState(iconIdVoice); 
				}
			}
			else
			{	
				iconsSetState(iconIdVoice);      /*  display voice mail icon.*/
//Apr 05, 2005    REF: ENH 29994 xdeepadh	
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
//Play the SMS tone alert
#ifdef FF_MIDI_RINGER  
	mfw_ringer_start(AS_RINGER_MODE_SMS,false,sounds_midi_ringer_start_cb);
#else

#ifdef FF_MMI_RINGTONE /* MSL SMS Tone Play */
                           audio_PlaySoundID( AUDIO_BUZZER, SMSTONE_SELECT, 200, AUDIO_PLAY_ONCE );
#else
			    	/* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
				{
					UBYTE currentVoiceTone; 
					currentVoiceTone = getcurrentVoiceTone();					
                            	audio_PlaySoundID( AUDIO_BUZZER, currentVoiceTone, 200, 
                                                              AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
				}
#else
				{
					UBYTE currentVoiceTone; 
					currentVoiceTone = getcurrentVoiceTone();					
			    		audio_PlaySoundID( AUDIO_BUZZER, currentVoiceTone, 200, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
				}
#endif

#endif
#endif
				mme_backlightEvent(BL_INCOMING_SMS);
			}

/* Set */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
                    iSmsArrival = 1;
#endif
#endif                    
			idleEvent(IdleUpdate);		/*  update the idle screen.*/
	    	

	    	
	    		// Feb 16, 2005 REF: CRR MMI-SPR-27900 xnkulkar 
	    		// Check if this voice mail sms is marked for deletion (belonging to group "1100") 
	    		if(msg_list[0].stat == MFW_SMS_VOICEMSG_DELETE)
	    		{
	    			//Delete this voice mail sms and set 'invalidVm' flag to FALSE
				/*a0393213 OMAPS00109953 - memory passed as parameter*/
				sms_msg_delete(msg_list[0].index,msg_list[0].memory);					
				invalidVm = FALSE;
	    		}
		}
		else if(urd_count > 0 && urd_msg_list[0].stat == MFW_SMS_UNREAD && (event != E_SMS_MT))
		{

/* Set */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
                    iSmsArrival = 1;
#endif
#endif                    
        
		    /* set the status: The sms icon and  the string*/
	    	/* "message(s) received" will be displayed     */
	    	iconsSetState(iconIdSMS);      /* Indicate SMS delivery to MS-User. */
		    idleEvent(IdleNewSmsMessage);
//Apr 05, 2005    REF: ENH 29994 xdeepadh	
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
//Play the SMS tone alert
//If SMS is recieved , while in call screen,dont play the tone.


/* Added Condtion(   if((!(call_data.calls.numCalls))) ) to stop the 
     SMS tone when call is active - OMAPS00066134  
*/	
if((!(call_data.calls.numCalls)))
{
#ifdef FF_MIDI_RINGER  
	mfw_ringer_start(AS_RINGER_MODE_SMS,false,sounds_midi_ringer_start_cb);
#else
		   
#ifdef NEPTUNE_BOARD

#ifdef FF_MMI_RINGTONE /* MSL SMS Tone Play */
                  audio_PlaySoundID( AUDIO_BUZZER, SMSTONE_SELECT, 200, AUDIO_PLAY_ONCE ); 	
#else
		{
		 	UBYTE currentSMSTone;
			currentSMSTone = getcurrentSMSTone();
                  	audio_PlaySoundID( AUDIO_BUZZER, currentSMSTone,
                                                    getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */	
		}
#endif
#else
		{
			UBYTE currentSMSTone;
			currentSMSTone = getcurrentSMSTone();	
          	    	audio_PlaySoundID( AUDIO_BUZZER, currentSMSTone, 200, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */	
		}
#endif
#endif
}
// Sep 04, 2006    REF:OMAPS00092060  x0039928
// Fix: A tone is played for incoming SMS during a call.
else
{
#ifdef FF_MIDI_RINGER
	audio_PlaySoundID( AUDIO_SPEAKER, TONES_RINGING_TONE, 0 ,  AUDIO_PLAY_ONCE);
#endif
}
		   /* cq15633 If the Incoming call alert screen is active continue with the incoming call ringer after playing the
		        the SMS received tone. 23-03-04 MZ. */
//Apr 05, 2005    REF: ENH 29994 xdeepadh			        
#ifndef FF_MIDI_RINGER  
		   if(TRUE == call_IsAlertScreenActive())
		   {
#ifdef FF_MMI_RINGTONE /* MSL SMS Tone Play */
                            audio_PlaySoundID(AUDIO_BUZZER, SMSTONE_SELECT, 0 , AUDIO_PLAY_INFINITE);
#else
				/* RAVI - 20-1-2006 */		   	
/* Silent Implementation */
#ifdef NEPTUNE_BOARD
                            audio_PlaySoundID(AUDIO_BUZZER, currentRinger, 
                                                             getCurrentVoulmeSettings(), AUDIO_PLAY_INFINITE);
#else
				audio_PlaySoundID(AUDIO_BUZZER, currentRinger, 0 , AUDIO_PLAY_INFINITE);
#endif
/* END RAVI */

#endif
		   }
#endif
		   
			mme_backlightEvent(BL_INCOMING_SMS);
		}	
		/*NM p029*/
		/* API - Implemented NM changes from 3.3.3 mainline to show class 0 SMS on idle screen */
		else if(event == E_SMS_MT) /* this sms identifier "E_SMS_MT" is not saved on the SIM */
		{
			/* take the SMS data from MFW  */
            T_MFW_SMS_MT *Message = (T_MFW_SMS_MT *) w;
			char* temp_buffer_sms = (char*)ALLOC_MEMORY(MAX_EDITOR_LEN);
            /* Marcus: Issue 1650: 28/01/2003: Start */
            TRACE_EVENT ("E_SMS_MT");
            /* check for the class 0 message */
            TRACE_EVENT_P1("dcs: 0x%02X", Message->dcs);
			/*if memory allocation successful*/
			
			if (temp_buffer_sms != NULL)
			{
	            if ((Message->dcs & TEXT_CLASS_GIVEN) &&
	                ((Message->dcs & TEXT_CLASS_MASK) == TEXT_CLASS_0))
	            {
	                TRACE_EVENT ("Class 0 message");

					/*
					** SPR#1745 NDH Text from ACI may be compressed. if so
					** the text will need to be converted to ASCII before it can
					** be displayed properly
					*/
					if (Message->dcs & TEXT_COMPRESSED)
					{			 
					    
						TRACE_EVENT ("text_compressed");	

						/*
						ACI already (in case of 7bit-dcs!) extract the 7 bit to 8 bit,
						so this is what we get and we have just only to decode from
						7Bit-Default to ASCII
						*/   
		
				   /*SPR 2512, convert message into a text string and then send to idle for display*/
					smsidle_message_to_text(Message, temp_buffer_sms);
					addCBCH(temp_buffer_sms, IdleNewSmsMessage);	



						
					}
					else
					{  
						TRACE_EVENT ("text_uncompressed");

						
					/*SPR 2512, convert message into a text string and then send to idle for display*/
					smsidle_message_to_text(Message, temp_buffer_sms);
									addCBCH(temp_buffer_sms, IdleNewSmsMessage);	

					
					}
					/*
					** SPR#1745 - End
					*/
/* Added Condtion(   if((!(call_data.calls.numCalls))) ) to stop the 
     ring tone when call is active - OMAPS00066134  
*/					
#ifdef NEPTUNE_BOARD
					if((!(call_data.calls.numCalls)))
					{
#endif					
				    /*CQ10826 - API - 27/06/03 - Add audio indication for class 0 SMS*/
//Apr 05, 2005    REF: ENH 29994 xdeepadh		
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
#ifdef FF_MIDI_RINGER  
	mfw_ringer_start(AS_RINGER_MODE_SMS,false,sounds_midi_ringer_start_cb);
#else
				    
#ifdef FF_MMI_RINGTONE /* MSL SMS Tone Play */
                                audio_PlaySoundID( AUDIO_BUZZER, SMSTONE_SELECT, 200, AUDIO_PLAY_ONCE ); 
#else
				  /* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
					{
						UBYTE currentSMSTone;
				 		currentSMSTone = getcurrentSMSTone();
                                 	audio_PlaySoundID( AUDIO_BUZZER, currentSMSTone, 
                                                                   getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
					}
#else
					{
						UBYTE currentSMSTone;
				 		currentSMSTone = getcurrentSMSTone();
				    		audio_PlaySoundID( AUDIO_BUZZER, currentSMSTone, 200, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
					}
#endif

#endif
#endif
				    mme_backlightEvent(BL_INCOMING_SMS);
				    /*CQ10826 - END*/
					
/* Added Condtion(   if((!(call_data.calls.numCalls))) ) to stop the 
     SMS tone when call is active - OMAPS00066134  
*/	
#ifdef NEPTUNE_BOARD						
					}				
#endif					
					
	            }


            /*temp_buffer_sms will be freed in editor callback*/
    
		}
       }
		/*NM p029 end*/
	}

/* Re-Set */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
                    iSmsArrival = 0;
#endif
#endif                    
    
	    break;
	case E_SMS_CB: /* new CBCH received */
	{
		// USHORT i;  // RAVI
		SHORT geograhical_scope = (SHORT)0xC000;

		T_MFW_SMS_CB *Message = (T_MFW_SMS_CB *)w;

	    TRACE_EVENT ("E_SMS_CB");

		temp_buffer = (char*)ALLOC_MEMORY(MAX_CBMSG_LEN);
		memset(temp_buffer, '\0', MAX_CBMSG_LEN);

		//check: does the incoming CB indicates a immediate scope
		if (! (Message->sn & geograhical_scope) )
	     {
		 //Immediate scope
		 // API - 859 - German characters were not visable in the CB message
		 TRACE_EVENT("Immediate Scope");

		 // API - 859 - Convert the the incoming GSM Text to ASCII
		 /*SPR2175, use new function for conversion*/
		 ATB_convert_String(Message->cb_msg, MFW_DCS_8bits, Message->msg_len,
								  temp_buffer, MFW_ASCII, MAX_CBMSG_LEN, FALSE);
	     	
			/*	SPR 2429, removed search for terminating character*/

#ifdef MMI_HOMEZONE_ENABLED		// SH 11/12/01
			/* SPR759 - SH - Homezone: check to see if CB gives us the coordinates of the centre of the current cell */

			if (Message->msg_id==HZ_IDENTIFIER)
			{
				homezoneCBData(Message->cb_msg);
			}
			else
			{
#endif // HOMEZONE

		     //display now on the idle screen
		     
		     addCBCH(temp_buffer, IdleNewCBImmediateMessage);
		    statusCBCH(TRUE);
#ifdef MMI_HOMEZONE_ENABLED
			} /* SPR759 - SH */
#endif
		}
		else //Normal scope
		{
			TRACE_EVENT("Normal Scope");
			//only for single- and the last page of mulitpages
			//other pages have not the termination "0x00D"
			
			// API - 859 - Convert the the incoming GSM Text to ASCII
			/*SPR2175, use new function for conversion*/
			ATB_convert_String(Message->cb_msg, MFW_DCS_8bits, Message->msg_len,
					temp_buffer, MFW_ASCII, MAX_MSG_LEN, FALSE);

			memcpy(Message->cb_msg, temp_buffer, MAX_CBMSG_LEN);
			
			/*	SPR 2429, removed search for terminating character*/
			{
				Message->cb_msg[MAX_CBMSG_LEN - 1 ]='\0';  // RAVI - Changed as MAX_CBMSG_LEN - 1.
			}

			//indicate on the idle screen the new incoming cb
		    addCBCH( NULL, IdleNewCBNormalMessage );
			statusCBCH(TRUE);
			
			//save the incoming cb messages in the fifo
			sms_new_incoming_cb (Message);
			
		}
	    FREE_MEMORY((void*)temp_buffer,MAX_CBMSG_LEN);
	}
	break;

  case E_SMS_MEM_FULL:
        /* the memory of SMS is FULL !!*/
	    data->sms_store_free = FALSE;
	    
	    /* 	    sendevent IDLE_UPDATE; */
	    SEND_EVENT(idle_get_window(),IDLE_UPDATE,0,0);
	    break;
	case E_SMS_MEM_FREE:
	    /* the memory of SMS is FREE */
 	    data->sms_store_free = TRUE;
 	    
	    /* 	    sendevent IDLE_UPDATE; */
	    SEND_EVENT(idle_get_window(),IDLE_UPDATE,0,0);
		/* ??? */

	    break;
	case E_SMS_STATUS: /* status report from service center */
	     /* Marcus: Issue 1170: 07/10/2002: Start */
	    {
	        T_MFW_SMS_STATUS *sms_status = (T_MFW_SMS_STATUS *)w;
    	    TRACE_EVENT("E_SMS_STATUS");
//Apr 05, 2005    REF: ENH 29994 xdeepadh	
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
//Play the SMS tone alert for status report indication
#ifdef FF_MIDI_RINGER  
	mfw_ringer_start(AS_RINGER_MODE_SMS,false,sounds_midi_ringer_start_cb);
#else
 		    
#ifdef FF_MMI_RINGTONE /* MSL SMS Tone Play */
                  audio_PlaySoundID( AUDIO_BUZZER, SMSTONE_SELECT, 200, AUDIO_PLAY_ONCE );
#else
		   /* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
			{
				UBYTE currentSMSTone;
				 currentSMSTone = getcurrentSMSTone();
                  		audio_PlaySoundID( AUDIO_BUZZER, currentSMSTone, 
                                                   getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE );
			}
#else
			{
				UBYTE currentSMSTone;
				currentSMSTone = getcurrentSMSTone();
		    		audio_PlaySoundID( AUDIO_BUZZER, currentSMSTone, 200, AUDIO_PLAY_ONCE );
			}
#endif
/* END RAVI */

#endif
#endif
		    
      	    if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
    	    {
    	        /*
    	         * There is no support for Chinese in the addCBCH function,
    	         * so will be problems with class 0 SMS messages and with cell
    	         * broadcast messages that need to be addressed anyway.
    	         * A new issue will be raised to handle this problem.
    	         */
    	    }
    	    else
    	    {
    	        memset(status_report, '\0', sizeof(status_report));
    	        strcpy(status_report, MmiRsrcGetText(TxtSRR));
    	        strcat(status_report, ": ");
     	        strcat(status_report, MmiRsrcGetText(TxtStatus));
    	        strcat(status_report, ": ");
     	        strcat(status_report, MmiRsrcGetText(smsidle_sr_status_to_text_id(sms_status->status)));
     	        sprintf(&status_report[strlen(status_report)], " (%u): ", sms_status->status);
#ifdef NO_ASCIIZ
                if (sms_status->addr.len)
                {
                    memcpy(&status_report[strlen(status_report)], sms_status->addr.tag, sms_status->addr.len);
        	        strcat(status_report, " ");
                }
#else
                if (strlen(sms_status->addr.tag))
                {
                    strcat(status_report, sms_status->addr.tag);
        	        strcat(status_report, " ");
                }
#endif
                if (sms_status->addr.ton == MFW_TON_INTERNATIONAL)
                    strcat(status_report, "+");

                strcat(status_report, sms_status->addr.number);
                sprintf(&status_report[strlen(status_report)],
                    ": %1u%1u/%1u%1u/%1u%1u %1u%1u:%1u%1u:%1u%1u (%s %u)", /*a0393213 lint warnings removal-%0u changed to%1u*/
                    sms_status->rctp.day[0],
                    sms_status->rctp.day[1],
                    sms_status->rctp.month[0],
                    sms_status->rctp.month[1],
                    sms_status->rctp.year[0],
                    sms_status->rctp.year[1],
                    sms_status->rctp.hour[0],
                    sms_status->rctp.hour[1],
                    sms_status->rctp.minute[0],
                    sms_status->rctp.minute[1],
                    sms_status->rctp.second[0],
                    sms_status->rctp.second[1],
                    MmiRsrcGetText(TxtMessage),
                    sms_status->msg_ref);
                   
				addCBCH(status_report, IdleNewSmsMessage);

    		    //display now on the idle screen
    			statusCBCH(TRUE);
       	    }
    	}
	    /* Marcus: Issue 1170: 07/10/2002: End */

	    break;
	case E_SMS_MEM: /* indicates status of memory */
	    /* not used */
	    break;
	case E_SMS_MO: /* SMS sending has started */
	    /* not used */
	case E_SMS_OK:
		/* Only happens when an SMS message is deleted. */

		break;
    case E_SMS_BUSY:
	    TRACE_EVENT ("E_SMS_BUSY");    
		break;
	case E_SMS_SAVE_AVAIL:
		break;
	case E_SMS_CMD_AVAIL:
		break;
    case E_SMS_ERR:
	    TRACE_EVENT ("E_SMS_ERR");    
	    break;
        default:
	    TRACE_EVENT("unhandled event in smsidle_sms_cb()");
        result = MFW_EVENT_PASSED;
	    break;
	}
	//May 27, 2005    MMI-FIX-29869 x0018858
#ifdef FF_MMI_SMS_DYNAMIC
    FREE_MEMORY((void*)msg_list,g_max_messages*sizeof(T_MFW_SMS_IDX));
    FREE_MEMORY((void*)urd_msg_list,g_max_messages*sizeof(T_MFW_SMS_IDX));
#else
    FREE_MEMORY((void*)msg_list,MAX_MESSAGES*sizeof(T_MFW_SMS_IDX));
    FREE_MEMORY((void*)urd_msg_list,MAX_MESSAGES*sizeof(T_MFW_SMS_IDX));
#endif

    return result;
}

/*******************************************************************************

 $Function:    	smsidle_get_memory_full

 $Description:	Call back function for the sms Idle window.
 
 $Returns:		sms memory status.

 $Arguments:	none
 
*******************************************************************************/

BOOL smsidle_get_memory_full(void)
{
    return (!smsidle_data.sms_store_free);
}

/*******************************************************************************

 $Function:    	smsidle_get_new_cb_messages_count

 $Description:	Call back function for the sms Idle window.
 
 $Returns:		sms count.

 $Arguments:	none
*******************************************************************************/

SHORT smsidle_get_new_cb_messages_count(void)
{
    if (smsidle_get_ready_state())
	{
	    return smscb_get_new_cb_messages_count();
	}
    else
	{
	    return 0;
	}
}


/*******************************************************************************

 $Function:    	smsidle_get_ready_state

 $Description:	This function can be call from anywhere to get the information 
            	about the "sms_initialised"
 
 $Returns:		sms status

 $Arguments:	none
 
*******************************************************************************/
BOOL smsidle_get_ready_state(void)
{
    return (smsidle_data.sms_initialised);
}

//GW SPR#1035 - Added SAT changes
/*******************************************************************************

 $Function:    	smsidle_unset_ready_state

 $Description:	this unset the flat "sms_initialised"
 
 $Returns:		sms status

 $Arguments:	none
 
*******************************************************************************/
void smsidle_unset_ready_state(void)
{
    smsidle_data.sms_initialised = FALSE;
}

/*******************************************************************************

 $Function:    	smsidle_get_unread_sms_available

 $Description:	This function allows an external process to determine the
            	received and unread SMS PP message status.
 
 $Returns:		sms status.

 $Arguments:	none
 
*******************************************************************************/
BOOL smsidle_get_unread_sms_available(void)
{	SHORT unread_sms_count;

	//May 27, 2005    MMI-FIX-29869 x0018858
#ifdef FF_MMI_SMS_DYNAMIC
    	//begin
	qAT_PlusCPMS(CMD_SRC_LCL);
	Check_max_sms_allowed();
	//End
#endif
	TRACE_FUNCTION("smsidle_get_unread_sms_available() ");
    if (smsidle_get_ready_state())
    {
    	//API - 14-01-03 - 1403 - Remove the call to iconsSetState(iconIdSMS) to indicate new SMS incall
		unread_sms_count = sms_message_count(MFW_SMS_UNREAD);

		return (unread_sms_count > 0 && (unread_sms_count > sms_message_count(MFW_SMS_VOICE)));
	}
    else
	{

    	iconsDeleteState(iconIdSMS);
		return FALSE;
	}
}

/*******************************************************************************

 $Function:    	smsidle_display_sms_status

 $Description:	Interogates the SMS unread message count and displays
	            the result using the SMS icon and an information text.
 
 $Returns:		id of text string to be displayed 

 $Arguments:	noOfMsgs - returns number of unread messages (or 0 if there are no
					messages or the memory is full).

  GW 02/10/01 - Modified code to work with chinese unicode text.
*******************************************************************************/
int smsidle_display_sms_status(int* noOfMsgs)
{
    SHORT SmsMsgCount;
    SHORT SmsVmailCount;
	int txtMsg;

	TRACE_FUNCTION("smsidle_display_sms_status()  ");
	*noOfMsgs = 0;
	txtMsg = TxtNull;
	
    if (smsidle_get_ready_state())
	{
	    SmsVmailCount = sms_message_count(MFW_SMS_VOICE);


	    SmsMsgCount = sms_message_count (MFW_SMS_UNREAD);

	    /* Envelope icon and text only appear if a non-Voicemail SMS exists. */

	    if (smsidle_get_memory_full())
		{
			/* x0018858 24 Mar, 2005 MMI-FIX-11321
			Added a condition to check for any pending message.
			*/
			//begin -x0018858 24 Mar, 2005 MMI-FIX-11321		
			if( TRUE == sms_check_message_pending())
			{
				txtMsg = TxtSmsWaiting;
			}//end. x0018858 24 Mar, 2005 MMI-FIX-11321
			else
			{
				txtMsg =  TxtSmsListFull;
			}
		} 
	    else if ((SmsMsgCount > 0) && (SmsMsgCount > SmsVmailCount))
		{
			txtMsg = TxtNewMessage;
			*noOfMsgs = SmsMsgCount;

		}
	    else
		{
		    iconsDeleteState(iconIdSMS);   /* Turn off SMS icon. */
			txtMsg = TxtNull;
		}

	    iconsShow();
	}
	return (txtMsg);
}

/*******************************************************************************

 $Function:    	smsidle_get_unread_smscb_available

 $Description:	This function allows an external process to determine the
            	received SMS CB message status.
 
 $Returns:		execution status

 $Arguments:	none
 
*******************************************************************************/
BOOL smsidle_get_unread_smscb_available(void)
{
    if (smsidle_get_ready_state())
	return (smsidle_get_new_cb_messages_count() > 0);
    else
	return FALSE;
}

/*******************************************************************************

 $Function:    	smsidle_display_smscb_status

 $Description:	Interogates the SMS CB message received status and
            	returns the resulting string to the caller.
 
 $Returns:		none

 $Arguments:	TextString - Text String
*******************************************************************************/
void smsidle_display_smscb_status(char *TextString)
{
	SHORT count;

	/* count the unread cb messagesc */
	count = smsidle_get_new_cb_messages_count();


	/* the unread CB should have a high priority than */
	/* the class 0 CB	                              */
    if (count > 0)
	{
	    sprintf(TextString, "%d %s\0",count, GET_TEXT(TxtNewCbMessage));
        iconsSetState(iconIdSMS);      /* Indicate SMS delivery to MS-User. */

	}
}
/* Marcus: Issue 1170: 07/10/2002: Start */
/*******************************************************************************

 $Function:    	smsidle_sr_status_to_text_id

 $Description:	Returns the text ID for the given status report status
 
 $Returns:		int : text ID

 $Arguments:	UBYTE: SMS-STATUS-REPORT status
*******************************************************************************/
static int smsidle_sr_status_to_text_id(UBYTE status)
{
    int ret = TxtError;
    UBYTE group = ((status & 0x60) >> 5);
    UBYTE value = (status & 0x1F);
    switch (group)
    {
        case 0:  // Short message transaction completed
            if ((value >= 3) && (value <= 0x0F))
                ret = TxtError;
            else
                ret = TxtDelivered;
            break;

        case 1: // Temporary error, SC still trying to transfer SM
            if ((value >= 6) && (value <= 0x0F))
                ret = TxtError;
            else                  // x0021334 10 May, 2006 DR: OMAPS00075379
                ret = TxtPending; // replaced TxtTempError with TxtPending
            break;

        case 2: // Permanent error, SC is not making any more transfer attempts
        case 3: // Temporary error, SC is not making any more transfer attempts
            ret = TxtError;
            break;
    }
    return ret;
}
/* Marcus: Issue 1170: 07/10/2002: End */






