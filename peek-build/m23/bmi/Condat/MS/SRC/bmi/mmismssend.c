/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI                                                            
 $Module:		SMS
 $File:		    MmiSmsSend.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
    Implementation template for MMI yyy dynamic menu handling
                        
********************************************************************************


 $History: MmiSmsSend.c

    June 4 2007  REF: ACT_X0073106-BMI-5X-OMAPS00133906
    Description:Sending SMS fails when the sender's number contain '+' international symbol in it.
    Solution:In file mmiSmsSend.c, In SmsSend_NBEDIT_edit_cb function, case INFO_KCD_LEFT i changed  if condition. 	

    Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)
    Description:SMS alphanumeric address
    Solution:Inorder to support alphanumeric address,MMI is setting editor mode as
    "ED_MODE_ALPHA" in  the function SmsSend_R_OPTExeSend(). Also MMI is checking whether user
    have entered Numeric oralphanumeric address in the function sms_submit() and
    sms_submit before calling sAT_PlusCMGS() and sAT_PlusCMGW() respectively.
 
 	Jun 15, 2006      REF: OMAPS00067510   xreddymn
 	Description: MMI Hangs when SMS Memory is full and while reading a message
 	Solution: msgBuffer is now released after sending or saving a message, so that this heap
 	memory is now available for use by other applications.

	Jan 18, 2006      REF: OMAPS00049169 x0pleela
	Description:  MS sends SMS with RP wrong destination address. Anite TC 34.2.8 was failing.
	Solution: Removed the code where global variable reply_path_bit was reset to 0 in function 
	SmsSend_SEND_mfw_cb(), switch case E_SMS_MO_AVAIL
	
       March 2, 2005    REF: CRR 11536 x0018858
       Description: Reply path not supported while sending a reply.
       Solution: Added menu option for the reply path.

	Feb 02, 2005   REF: CRR 28479   xnkulkar
	Bug: While saving a SMS, if the user presses LSK while the 'Saved' dialog is displayed, the 'Saved' 
	dialog doesn't get dismissed.
	Solution: Only RSK and Hangup keys dismiss the 'Saved' info screen and all other key presses 
       are ignored. 
	              
	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX

	Aug 16, 2004    REF: CRR 24281  Deepa M.D 
	Bug:Go-lite  Optimization  Remove the unsed variable  temp_conv_buffer
	Fix:Removed the unused  variable static UBYTE temp_conv_buffer[MAX_MSG_LEN_ARRAY].


    Sep 01, 2004    REF: CRR 21380  Deepa M.D 
	Bug:Not getting proper display, When Messages Memory is Full.
	Fix:A new Dialog displaying "Full Not Saved" will be displayed,
	when memory is full and saving a message fails.




	Jul 13, 2004    REF: CRR 21615  Deepa M.D 
	Bug:Message once entered into message editor not storing in memory.
	Fix:When a SMS is typed in the Editor, it is stored in a temporary buffer.
	When the user accesses the editor,the temporary buffer is copied into the 
	Editor back.

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/

#define MMI_SMSSEND_C

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
#include "prim.h"


#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_tim.h"
#include "mfw_phb.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_sms.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_lng.h"
#include "mfw_sat.h"
#include "mfw_kbd.h"
#include "mfw_nm.h"
#include "mfw_str.h"

#include "dspl.h"

#include "MmiMain.h"
#include "MmiDummy.h"
#include "MmiMmi.h"
#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "phb.h"      /* FDN  */
#include "MmiBookUtils.h"
#include "MmiBookShared.h"
#include "mmiSmsMenu.h"
#include "mmismssend.h"
#include "mmiSmsIdle.h"
#include "mmiSat_i.h"
#include "mfw_ffs.h"
#ifdef EASY_TEXT_ENABLED
#include "MmiChineseInput.h"
#endif
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else /* NEW_EDITOR */
#ifdef EASY_TEXT_ENABLED
#include "MmiLatinPredText.h"
#endif /* EASY_TEXT_ENABLED */
#include "MmiEditor.h"
#endif /* NEW_EDITOR */

#include "cus_aci.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


#include "mmiColours.h"


/*********************************************************************
**********************************************************************

				 DYNAMIC MENU WINDOW. DECLARATION

*********************************************************************
**********************************************************************/
/* MZ 10202, 10758   */
static UBYTE* msgBuffer = NULL;

/* General functions */
static void SmsSend_NBEDIT_edit_cb (T_MFW_HND win, USHORT Identifier, SHORT reason);
static int  SmsSend_standard_dialog_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason);
/* SPR#1428 - SH - New Editor: no longer required */
#ifndef NEW_EDITOR
void		SmsSend_loadEditDefault(T_EDITOR_DATA * edit_info);
static void SmsSend_loadUCS2EditDefault(T_EDITOR_DATA *editor_data);
#endif

/* prototyps */
static T_MFW_HND SmsSend_SEND_create  (T_MFW_HND parent);
static void      SmsSend_SEND_destroy (T_MFW_HND window);

// Main callback function of the frame window
static void      SmsSend_main_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
// Sub-callback functions of the frame window associated to the different emitters
static void      SmsSend_SEND_exec_cb (T_MFW_HND win, USHORT event, void * parameter);
static void      SmsSend_PHBK_exec_cb (T_MFW_HND win, USHORT event, void * parameter);
static void      SmsSend_TEXTEDIT_exec_cb (T_MFW_HND win, USHORT event, void * parameter);
static void      SmsSend_NBEDIT_exec_cb (T_MFW_HND win, USHORT event, void * parameter);
static void      SmsSend_CENTREEDIT_exec_cb (T_MFW_HND win, USHORT event, void * parameter);

// Callback functions of the various editors
static void		SmsSend_TEXTEDIT_edit_cb (T_MFW_HND win, USHORT Identifier, SHORT reason);
static void		SmsSend_NBEDIT_edit_cb (T_MFW_HND win, USHORT Identifier, SHORT reason);
static void		SmsSend_CENTREEDIT_edit_cb (T_MFW_HND win, USHORT Identifier, SHORT reason);

// Callback function for the MFW events
static int       SmsSend_SEND_mfw_cb  (MfwEvt event, void *data);

// This function initiate the sending of the SMS
static int       SmsSend_send_to_mfw    (T_MFW_HND win);
/* x0039928 - Lint warning fix
static T_MFW_HND parentWindow4SMS; */

UBYTE SmsSend_set_SrvCntr_number (T_MFW_SMS_INFO *config_data);
UBYTE SmsSend_set_dcs_number (T_MFW_SMS_INFO *config_data);
//Jul 13, 2004    REF: CRR 21615  Deepa M.D 
extern UBYTE TempTextBuffer[MAX_MSG_LEN_ARRAY];

extern UBYTE reply_path_bit ;

/***************************************************
****											****
****		MANAGEMENT OF SMS OPTIONS			****
****											****
***************************************************/


typedef struct
{
    /* administrative data */

    T_MMI_CONTROL   mmi_control;
    T_MFW_HND       win;
    SHORT           id;
    T_MFW_HND       parent;

    /* associated handlers */

    /* !!! **************************************** modify here **************************************** !!!*/
    T_MFW_HND       kbd;
    T_MFW_HND       kbd_long;
    T_MFW_HND       menu;
    /* !!! **************************************** end modify here **************************************** !!! */

    /* internal data */

    /* !!! **************************************** modify here **************************************** !!!*/
    BOOL            answer; /* TRUE == YES, FALSE == NO */
	T_MFW_HND		sms;
    T_MFW_HND       dialog_win;
	T_SEND			*parent_data;
/* !!! **************************************** end modify here **************************************** !!! */
} T_SMSSEND_R_OPT;

typedef struct
{
	/* administrative data */

	T_MMI_CONTROL   mmi_control;
	T_MFW_HND       win;
	T_MFW_HND       parent;
	T_MFW_HND       child_dialog;/* to keep the window of dialog          */    
	T_MFW_HND       sms_handler;
	SHORT           id;
	/*SPR1991, removed edt_data: not needed*/
	   					/* will call from idle or phonebook  */
} T_SAVE;

static T_MFW_HND	SmsSend_SAVE_create  (T_MFW_HND parent);
static void				SmsSend_SAVE_destroy (T_MFW_HND window);
static void				SmsSend_SAVE_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int				SmsSend_SAVE_mfw_cb (MfwEvt event, void *info);

static T_MFW_HND SmsSend_R_OPT_create (T_MFW_HND parent);
static void SmsSend_R_OPT_destroy (T_MFW_HND window);
static void SmsSend_R_OPT_StandardDialog(T_MFW_HND win, char *text, ULONG duration);
static void SmsSend_R_OPT_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int SmsSend_R_OPT_win_cb (MfwEvt e, MfwWin *w);
static int SmsSend_R_OPT_mfw_cb (MfwEvt event, void *parameter);
T_MFW_HND SmsSend_OPT_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr);
static int SmsSend_R_OPT_dialog_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason);
static void SmsSend_SCNBEDIT_edit_cb (T_MFW_HND win, USHORT Identifier, SHORT reason);
/* !!! **************************************** modify here **************************************** !!!*/
static int SmsSend_R_OPT_kbd_cb (MfwEvt e, MfwKbd *k);
static int SmsSend_R_OPT_mnu_cb (MfwEvt e, MfwMnu *m);
//Sep 01, 2004    REF: CRR 21380  Deepa M.D 
static UBYTE memfull=FALSE; //Variable used check whether E_SMS_MEM_FULL event has been received.
/* !!! **************************************** end modify here **************************************** !!! */



/*********************************************************************
**********************************************************************

			     DYNAMIC MENU WINDOW. CONTENTS DECLARATION

*********************************************************************
**********************************************************************/

/* !!! **************************************** modify here **************************************** !!!*/

static int SmsSend_R_OPTExeSave(MfwMnu* m, MfwMnuItem* i);
static int SmsSend_R_OPTExeSend(MfwMnu* m, MfwMnuItem* i);
static int SmsSend_R_OPTExeDiscard(MfwMnu* m, MfwMnuItem* i);

static const MfwMnuItem SmsSend_R_OPTItems [] =
{
    {0,0,0,(char *)TxtSave,0,(MenuFunc)SmsSend_R_OPTExeSave,item_flag_none},
    {0,0,0,(char *)TxtSend,0,(MenuFunc)SmsSend_R_OPTExeSend,item_flag_none},
    {0,0,0,(char *)TxtDiscard,0,(MenuFunc)SmsSend_R_OPTExeDiscard,item_flag_none}
};

static const MfwMnuAttr SmsSend_R_OPTAttrib =
{
    &SmsSend_R_OPTArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu       */
    /* x0045876, 14-Aug-2006 (WR - integer conversion resulted in a change of sign) */
    (UBYTE) -1,                                 /* use default font         */
    (MfwMnuItem*)SmsSend_R_OPTItems,                      /* with these items         */
    sizeof(SmsSend_R_OPTItems)/sizeof(MfwMnuItem), /* number of items     */
    COLOUR_LIST_SMS,	TxtNull, NULL, MNUATTRSPARE

};


/*JVJ SPR1298 added a new output parameter to avoid stack overload */
void convertToUnicodeForOutput( UBYTE * ipText,UBYTE* outText, USHORT* outLen)
{

	int i;
    UBYTE* temp_conv_buffer = outText;

	if (ipText[0] == 0x80) 
	{	//skip first 2 chars
		*outLen = 0;
		for (i=0;i<MAX_MSG_LEN/2;i++)
		{	/*SPR 2175, bytes should not be swapped*/
			temp_conv_buffer[i*2+1] =  ipText[i*2+2+1];
			temp_conv_buffer[i*2] =ipText[i*2+2];




			if ((*outLen ==0) && 
				(temp_conv_buffer[i*2] == 0x00) && 
				(temp_conv_buffer[i*2+1] == 0x00))
				*outLen = i*2+2;
		}
		if (*outLen == 0)
		{//Reached end of message without finding 0x0000 - set last word to 0x0000
			*outLen = MAX_MSG_LEN;
			temp_conv_buffer[MAX_MSG_LEN-2] = 0x00;
			temp_conv_buffer[MAX_MSG_LEN-1] = 0x00;


		}
		return;

	}
	else
	{	
		*outLen = strlen((char*)ipText);
		/*MC SPR1319, use string's actual length to convert to GSM alphabet*/
		/*SPR2175, use new function for conversion*/
		ATB_convert_String((char*)ipText, MFW_ASCII, *outLen, (char*)temp_conv_buffer, MFW_DCS_8bits,MAX_MSG_LEN , FALSE);
		return;
	}
}

/*******************************************************************************

 $Function:    	SmsSend_R_OPTExeSend

 $Description:	Start the creation of the main window for SMS Send.
 
 $Returns:		none

 $Arguments:	m - Menu handle
 				i - Menu item selected.
 
*******************************************************************************/

static int SmsSend_R_OPTExeSend(MfwMnu* m, MfwMnuItem* i)
{	
    T_MFW_HND     	   win      = mfwParent(mfw_header());
    T_MFW_WIN        * win_data = ((T_MFW_HDR *)win)->data;
    T_SMSSEND_R_OPT  * data     = (T_SMSSEND_R_OPT *)win_data->user;
	/* access to data from parent   */
    T_SEND  * parent_data =  (T_SEND *)data->parent_data;
	

#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
	T_EDITOR_DATA editor_data;
#endif
	TRACE_EVENT("SmsSend_R_OPTExeSend");
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			AUI_edit_SetDefault(&editor_data);
			AUI_edit_SetDisplay(&editor_data, PHNO_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
			AUI_edit_SetEvents(&editor_data, SMSSEND_ID_NBEDIT, FALSE, FOREVER, (T_AUI_EDIT_CB)SmsSend_NBEDIT_edit_cb);
	// Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)
			AUI_edit_SetTextStr(&editor_data, TxtSend, TxtDelete, TxtTo, NULL);
			AUI_edit_SetAltTextStr(&editor_data, 1, TxtNames, TRUE, TxtSoftBack);
	// Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)
	//Mode is changed to alphanumeric mode		
			AUI_edit_SetMode(&editor_data, ED_MODE_ALPHA, ED_CURSOR_UNDERLINE);
			AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, parent_data->edt_data.NumberBuffer, PHB_MAX_LEN);
			parent_data->number_editor = AUI_edit_Start(parent_data->win, &editor_data);
#else /* NEW_EDITOR */

			SmsSend_loadEditDefault(&editor_data);

			bookSetEditAttributes( PHNO_EDITOR, COLOUR_EDITOR_XX, 0, edtCurBar1, 0, 
			(char *) parent_data->edt_data.NumberBuffer, /*MAX_SEARCH_CHAR*/NUMBER_LENGTH, &editor_data.editor_attr);
			editor_data.editor_attr.text     = (char *)parent_data->edt_data.NumberBuffer; 

			editor_data.editor_attr.size     = PHB_MAX_LEN; 

			editor_data.TextId               = TxtSmsEnterNumber;  				 							  
			editor_data.LeftSoftKey          = TxtSend;
			editor_data.AlternateLeftSoftKey = TxtNames;
			editor_data.RightSoftKey         = TxtDelete;
			editor_data.Identifier           = SMSSEND_ID_NBEDIT ; 
			editor_data.mode				 = E_EDIT_DIGITS_MODE;
			editor_data.Callback             = (T_EDIT_CB)SmsSend_NBEDIT_edit_cb;
			editor_data.destroyEditor		 = FALSE;
			parent_data->number_editor = editor_start(parent_data->win, &editor_data);  /* start the editor */ 
#endif /* NEW_EDITOR */
	return 1;  // ADDED BY RAVI - 28-11-2005

}

/*******************************************************************************

 $Function:    	SmsSend_R_OPTExeDiscard

 $Description:	
 
 $Returns:		execution status

 $Arguments:	m - Menu handle
 				i - Menu item selected.
 
*******************************************************************************/

static int SmsSend_R_OPTExeDiscard(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND     	   win      = mfwParent(mfw_header());
    T_MFW_WIN        * win_data = ((T_MFW_HDR *)win)->data;
    T_SMSSEND_R_OPT  * data     = (T_SMSSEND_R_OPT *)win_data->user;
	/* access to data from parent   */
    T_SEND  * parent_data =  (T_SEND *)data->parent_data;

	TRACE_EVENT("SmsSend_R_OPTExeDiscard");

	SmsSend_R_OPT_destroy(parent_data->options_win);
#if	defined(CHINESE_MMI) && defined(EASY_TEXT_ENABLED)
	 if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
	 	chinese_input_destroy(parent_data->text_editor);
	 else
#endif
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	AUI_edit_Destroy(parent_data->text_editor);
#else /* NEW_EDITOR */
	editor_destroy(parent_data->text_editor);
#endif /* NEW_EDITOR */
/*SPR2453  check memory allocated before deallocating it */
	  	if (msgBuffer !=NULL)
		{ FREE_MEMORY((void*)msgBuffer,MAX_MSG_LEN_ARRAY);
		  msgBuffer = NULL;
	  	}
	parent_data->text_editor = 0;
	SmsSend_SEND_destroy(parent_data->win);

	
	return 1;
}


/*******************************************************************************

 $Function:    	SmsSend_R_OPTExeSave

 $Description:	called when the user press Call sender in the Option menu
 
 $Returns:		execution status

 $Arguments:	m - Menu handle
 				i - Menu item selected.
 
*******************************************************************************/

static int SmsSend_R_OPTExeSave(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       win  = mfwParent(mfw_header());
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    T_SMSSEND_R_OPT         * data = (T_SMSSEND_R_OPT *)win_data->user;

	TRACE_EVENT("SmsRead_R_OPTExeCallSender");

			if (!smsidle_get_ready_state())
			{
				// SMS init processing not ready to access SIM to store the SMS
				mmi_dialog_information_screen(win, 0,"Not Ready", (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_SAVE_NOT_READY);
				// Recreate the editor on dialog callback
			}
			else
				//END TB
				// Start a save window and provide it a T_SmsUserData structure (in the future, will allow to save easily text but also number...)
			{
				SmsSend_SAVE_start(data->parent_data->win, &(data->parent_data->edt_data));
				SmsSend_R_OPT_destroy(win);
			}
    return 1;
}

/*******************************************************************************

 $Function:    	SmsSend_OPT_start

 $Description:	called when the user press Call sender in the Option menu
 
 $Returns:		execution status

 $Arguments:	parent_window -parent window handle
 				menuAttr - Menu attributes.
 
*******************************************************************************/

T_MFW_HND SmsSend_OPT_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr)
{
    T_MFW_HND win;

    TRACE_FUNCTION ("SmsSend_OPT_start()");

    win = SmsSend_R_OPT_create (parent_window);

    if (win NEQ NULL)
	{
	    SEND_EVENT (win, E_INIT, NULL, (void *)menuAttr);
	}
    return win;
}

/*******************************************************************************

 $Function:    	SmsSend_R_OPT_create

 $Description:	Create the Option window
 
 $Returns:		mfw window handle

 $Arguments:	parent_window -parent window handle
 			
*******************************************************************************/

static T_MFW_HND SmsSend_R_OPT_create(MfwHnd parent_window)
{
    T_SMSSEND_R_OPT      * data = (T_SMSSEND_R_OPT *)ALLOC_MEMORY (sizeof (T_SMSSEND_R_OPT));
    T_MFW_WIN  * win;

    TRACE_FUNCTION ("SmsSend_R_OPT_create()");

    /*
     * Create window handler
     */

    data->win = 
	win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)SmsSend_R_OPT_win_cb);
    if (data->win EQ NULL)
	{
	    return NULL;
	}
    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)SmsSend_R_OPT_exec_cb;
    data->mmi_control.data   = data;
    win                      = ((T_MFW_HDR *)data->win)->data;
    win->user                = (void *)data;
    data->parent             = parent_window;

    /*
     * return window handle
     */

    return data->win;
}

/*******************************************************************************

 $Function:    	SmsSend_R_OPT_destroy

 $Description:	Destroy the Option window
 
 $Returns:		mfw window handle

 $Arguments:	own_window - current window
 			
*******************************************************************************/

static void SmsSend_R_OPT_destroy(MfwHnd own_window)
{
    T_MFW_WIN * win_data;
    T_SMSSEND_R_OPT     * data;

    TRACE_FUNCTION ("SmsSend_R_OPT_destroy()");

    if (own_window)
	{
	    win_data = ((T_MFW_HDR *)own_window)->data;
	    data = (T_SMSSEND_R_OPT *)win_data->user;

	    if (data)
		{
		    /*
		     * Delete WIN handler
		     */ 
			data->parent_data->options_win = 0;
		    win_delete (data->win);

		    /*     
		     * Free Memory
		     */
		    FREE_MEMORY ((void *)data, sizeof (T_SMSSEND_R_OPT));
		}
	    else
		{
		    TRACE_EVENT ("SmsSend_R_OPT_destroy() called twice");
		}
	}
}

/*******************************************************************************

 $Function:    	SmsSend_R_OPT_exec_cb

 $Description:	Exec callback function for the Option window
 
 $Returns:		none

 $Arguments:	win - current window
 				event - event id
 				value - unique id
 				parameter - optional data.
 			
*******************************************************************************/

void SmsSend_R_OPT_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
    /* callback handler for events sent to to trigger execution */
{
    T_MFW_WIN        * win_data = ((T_MFW_HDR *) win)->data;
    T_SMSSEND_R_OPT  * data     = (T_SMSSEND_R_OPT *)win_data->user;
//    T_MFW_HND      * l_parent;   // RAVI
 //   SHORT            l_id;              // RAVI
    T_MFW_HDR    * parent      =  data->parent;
   	T_MFW_WIN    * win_data2   =  ((T_MFW_HDR *)parent)->data;
    T_SEND  * parent_data =  (T_SEND *)win_data2->user;

//	T_MFW_EVENT         MfwEvents;   // RAVI
	
    TRACE_FUNCTION ("SmsSend_R_OPT_exec_cb()");


    switch (event)
	{
	case E_INIT:
		
		TRACE_EVENT("SmsSend_R_OPT_exec_cb() Event:5.E_INIT");
	    /* initialization of administrative data */
		data->parent_data = parent_data;		
	    data->id = value;
		data->kbd =      kbdCreate(data->win,KEY_ALL,         (MfwCb)SmsSend_R_OPT_kbd_cb);
    	data->kbd_long = kbdCreate(data->win,KEY_ALL|KEY_LONG,(MfwCb)SmsSend_R_OPT_kbd_cb);
    	data->menu =     mnuCreate(data->win,(MfwMnuAttr*)&SmsSend_R_OPTAttrib, E_MNU_ESCAPE, (MfwCb)SmsSend_R_OPT_mnu_cb);
    	mnuLang(data->menu,mainMmiLng);
    	/* put the (new) dialog window on top of the window stack */
    	mnuUnhide(data->menu);
  	    	winShow(win);
	    break;
	default:
	    return;
	}
}

/*******************************************************************************

 $Function:    	SmsSend_R_OPT_mnu_cb

 $Description:	Menu callback function for the Option window
 
 $Returns:		none

 $Arguments:	e - event id
 				m - menu handle
*******************************************************************************/

static int SmsSend_R_OPT_mnu_cb (MfwEvt e, MfwMnu *m)
    
{
    T_MFW_HND    win  = mfwParent(mfw_header());
    T_MFW_WIN  * win_data = ((T_MFW_HDR *)win)->data;
    T_SMSSEND_R_OPT      * data = (T_SMSSEND_R_OPT *)win_data->user;
//    T_MFW_HND  * l_parent;   // RAVI
//    SHORT        l_id;		    // RAVI	

    TRACE_FUNCTION ("SmsRead_R_OPT_mnu_cb()");

    switch (e)
        {
	case E_MNU_ESCAPE: /* back to previous menu */
	    SmsSend_R_OPT_destroy(data->win);
	    break;
	default: /* in mnuCreate() only E_MNU_ESCAPE has been enabled! */
	    return MFW_EVENT_REJECTED;
        }
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	SmsSend_R_OPT_win_cb

 $Description:	Window callback function for the Option window
 
 $Returns:		execution status

 $Arguments:	e - event id
 				w - window handle 
*******************************************************************************/

static int SmsSend_R_OPT_win_cb (MfwEvt e, MfwWin *w)
{
    TRACE_FUNCTION ("SmsSend_R_OPT_win_cb()");

    switch (e)
	{
        case MfwWinVisible:  /* window is visible  */
		dspl_ClearAll();
	    displaySoftKeys(TxtSoftSelect,TxtSoftBack);
	    break;
	case MfwWinFocussed: /* input focus / selected   */
	case MfwWinDelete:   /* window will be deleted   */
        default:
	    return MFW_EVENT_REJECTED;
	}
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	SmsSend_R_OPT_kbd_cb

 $Description:	Exec callback function for the Option window
 
 $Returns:		execution status

 $Arguments:	e - event id
 				k - keyboard info
*******************************************************************************/

static int SmsSend_R_OPT_kbd_cb (MfwEvt e, MfwKbd *k)
    /* SmsRead_R_OPT keyboard event handler */
{
    T_MFW_HND    win  = mfwParent(mfw_header());
    T_MFW_WIN  * win_data = ((T_MFW_HDR *)win)->data;
    T_SMSSEND_R_OPT      * data = (T_SMSSEND_R_OPT *)win_data->user;

    TRACE_FUNCTION ("SmsSend_R_OPT_kbd_cb()");

    if (e & KEY_LONG)
	{
	    switch (k->code)
		{
		case KCD_HUP: /* back to previous menu */
		    mnuEscape(data->menu);
		    break;
		case KCD_RIGHT: /* Power Down */
		    return MFW_EVENT_REJECTED; /* handled by idle */
		default: /* no response to all other keys */
		    return MFW_EVENT_CONSUMED;
		}
	}
    else
	{
	    switch (k->code)
		{
		case KCD_MNUUP: /* highlight previous entry */
		    mnuUp(data->menu);
		    break;
		case KCD_MNUDOWN: /* highlight next entry */
		    mnuDown(data->menu);
		    break;
		case KCD_MNUSELECT:
		case KCD_LEFT: /* activate this entry */
		    mnuSelect(data->menu);
		    break;
		case KCD_HUP: /* back to previous menu */
		case KCD_RIGHT: /* back to previous menu */
		    mnuEscape(data->menu);
		    break;
		default: /* no response to all other keys */
		    return MFW_EVENT_CONSUMED;
		}
	}
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	SmsSend_SEND_start

 $Description:	Create SMS send window
 
 $Returns:		execution status

 $Arguments:	parent_window - parent window.
 				UserData - SMS data
*******************************************************************************/

T_MFW_HND SmsSend_SEND_start(T_MFW_HND parent_window, T_SmsUserData *UserData)
{
	T_MFW_HND win;

	TRACE_FUNCTION ("SmsSend_SEND_start()");
	/* x0039928 - Lint warning fix
	parentWindow4SMS = parent_window; */
	win = SmsSend_SEND_create (parent_window);

	if (win NEQ NULL)
	{
		SEND_EVENT (win, E_INIT, SEND_ID, (void *)UserData);
	}
	return win;
}

/*******************************************************************************

 $Function:    	SmsSend_SEND_create

 $Description:	Create a window for entering the text
 
 $Returns:		window handle

 $Arguments:	parent_window - parent window.
 				
*******************************************************************************/

static T_MFW_HND SmsSend_SEND_create(MfwHnd parent_window)
{
	T_SEND     * data = (T_SEND *)ALLOC_MEMORY (sizeof (T_SEND));
	T_MFW_WIN  * win;

	TRACE_FUNCTION ("SmsSend_SEND_create()");

	// Create window handler
	data->win = win_create (parent_window, 0, E_WIN_VISIBLE, NULL);
	if (data->win EQ NULL)
	{
		return NULL;
	}

	// connect the dialog data to the MFW-window
	data->mmi_control.dialog = (T_DIALOG_FUNC)SmsSend_main_exec_cb;
	data->mmi_control.data   = data;
	win                      = ((T_MFW_HDR *)data->win)->data;
	win->user                = (MfwUserDataPtr)data;
	data->parent             = parent_window;
	data->id								 = SEND_ID;

	winShow(data->win);
	return data->win;
}

/*******************************************************************************

 $Function:    	SmsSend_SEND_destroy

 $Description:	Destroy the windows for entering the text
 
 $Returns:		none

 $Arguments:	own_window - window.
 				
*******************************************************************************/

static void SmsSend_SEND_destroy(MfwHnd own_window)
{
  T_MFW_WIN * win_data;
  T_SEND    * data = NULL; 

  TRACE_FUNCTION ("SmsSend_SEND_destroy()");

  if (own_window)
	{
	  win_data = ((T_MFW_HDR *)own_window)->data;
		if (win_data != NULL) 
			data = (T_SEND *)win_data->user;

	  if (data)
		{
			
			// Delete sms handle
			if (data->sms_handler != NULL)
				sms_delete(data->sms_handler);

			if (data->child_dialog != NULL)
			{
				TRACE_EVENT("child_dialog not deleted?");
			}

			if (data->text_editor != NULL)
			{
				TRACE_EVENT("current_editor not deleted?");
				

			}


			// Delete WIN handler 
		  win_delete (data->win);

		  // Free Memory
			FREE_MEMORY ((void *)data, sizeof (T_SEND));
			//win_data->user = NULL; 
		}
	  else
		{
			TRACE_EVENT ("SmsSend_SEND_destroy() called twice");
		}
	}
}

/*******************************************************************************

 $Function:    	SmsSend_main_exec_cb

 $Description:	Main callback handler for events sent to to trigger execution
 
 $Returns:		none

 $Arguments:	win - current window
 				event - event id
 				identifier - unique id
 				parameter - optional data.
*******************************************************************************/

static void SmsSend_main_exec_cb (T_MFW_HND win, USHORT event, SHORT identifier, void * parameter)
{	
	TRACE_FUNCTION ("SmsSend_main_exec_cb()");

	/* In this field we get the identifier of the window sending the event */
	switch (identifier)
	{
		case SEND_ID:
		case SAVE_ID:	// events from the SAVE window are also managed in the SEND callback
		case SMSSEND_ID_SAVE_NOT_READY:
			SmsSend_SEND_exec_cb(win, event, parameter);
			break;
		case PHBK_ID:
			SmsSend_PHBK_exec_cb(win, event, parameter);
			break; 
		case SMSSEND_ID_TEXTEDIT:
			SmsSend_TEXTEDIT_exec_cb(win, event, parameter);
			break;
		case SMSSEND_ID_SC_NOT_READY: 
			/* MZ cq11074 delete the info dialog window. */
			SmsSend_SEND_destroy(win);
			break;
		case SMSSEND_ID_NBEDIT:
			SmsSend_NBEDIT_exec_cb(win, event, parameter);
			break;
		case SMSSEND_ID_CENTREEDIT:
			SmsSend_CENTREEDIT_exec_cb(win, event, parameter);
			break;
		default:
			TRACE_EVENT("Err: Default");
			break;
	}
}

/*******************************************************************************

 $Function:    	SmsSend_SEND_exec_cb

 $Description:	Callback handler for events sent by the window itself
 
 $Returns:		none

 $Arguments:	win - current window
 				event - event id
 				parameter - optional data.
*******************************************************************************/

static void SmsSend_SEND_exec_cb(T_MFW_HND win, USHORT event, void *parameter)
{
  T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
  T_SEND         * data = (T_SEND *)win_data->user;
  T_MFW_SMS_INFO	sms_parameter;
  T_MFW_EVENT     MfwEvents;
 //  T_DISPLAY_DATA display_info;   // RAVI

	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
    T_AUI_EDITOR_DATA	editor_data;
    USHORT				size;
#else /* NEW_EDITOR */
	T_EDITOR_DATA editor_data;
#endif /* NEW_EDITOR */

	TRACE_FUNCTION ("SmsSend_SEND_exec_cb()");

	switch (event)
	{
	case E_INIT:
			/* Create a sms handler to receive events  */
			MfwEvents = E_SMS_ERR | E_SMS_MO_AVAIL;
			data->sms_handler = sms_create(data->win,MfwEvents,(MfwCb)SmsSend_SEND_mfw_cb);
			data->child_dialog = NULL;
			
			/* store NUMBER and TEXT */
			memcpy(&(data->edt_data), (T_SmsUserData *)parameter, sizeof(T_SmsUserData));
      		/* in case of reply the sms we have to 
			   use the service center number from the
			   caller 
			 */
			 TRACE_EVENT_P1("data->edt_data.called_from_reply_item is %d",data->edt_data.called_from_reply_item);
			if (data->edt_data.called_from_reply_item EQ FALSE)
			{
			        TRACE_EVENT("RP false");
				// The SMS center is initialized to '\0'
				data->edt_data.CentreBuffer[0] = '\0';
			}
			else
				{
				TRACE_EVENT("RP true");
				}

		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#if defined(CHINESE_MMI) && defined(EASY_TEXT_ENABLED)
		/* If chinese is selected and we're opening up a new message
		 * or a unicode message, use the chinese editor */
		if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE
			&& (data->edt_data.TextBuffer[0]==0x00 || data->edt_data.TextBuffer[0] ==0x80))
		{
			T_CHINESE_DATA chinese_data;
		
			chinese_data.TextString = (char*) data->edt_data.TextBuffer;

			chinese_data.TextString[0] = (char)0x80;
			chinese_data.TextString[1] = 0x00;/*a0393213 warnings removal-MAX_MSG_LEN changed to 0x00*/
			chinese_data.Callback = (T_AUI_EDIT_CB)SmsSend_TEXTEDIT_edit_cb;
			chinese_data.Identifier = SMSSEND_ID_TEXTEDIT ;
			chinese_data.LeftSoftKey = TxtSoftOptions;
			chinese_data.DestroyEditor = FALSE;
#ifdef TI_PS_FF_CONC_SMS			
			if(FFS_flashData.ConcatenateStatus == TRUE)
				chinese_data.EditorSize = (USHORT)((MAX_MSG_LEN_CHI)/2 - 1);
			else
#endif /*TI_PS_FF_CONC_SMS*/				
				chinese_data.EditorSize = (USHORT)((MAX_MSG_LEN_SGL_CHI)/2 - 1);
			data->text_editor = chinese_input(win, &chinese_data);
		}
		else
#endif /* CHINESE_MMI && EASY_TEXT_ENABLED */
		/* We're opening up an ascii or unicode latin editor */
		{
			
			AUI_edit_SetDefault(&editor_data);

			/* Set the appropriate mode if easy text is enabled */
#ifdef EASY_TEXT_ENABLED
			AUI_edit_SetMode(&editor_data, ED_MODE_ALPHA | ED_MODE_PREDTEXT, ED_CURSOR_BAR);
#else /* EASY_TEXT_ENABLED */
			AUI_edit_SetMode(&editor_data, ED_MODE_ALPHA, ED_CURSOR_BAR);
#endif /* EASY_TEXT_ENABLED */

			/* Check if concatenated SMS is enabled */
#ifdef TI_PS_FF_CONC_SMS			
			if(FFS_flashData.ConcatenateStatus == TRUE)
				size     = MAX_MSG_LEN;
			else
#endif /*TI_PS_FF_CONC_SMS*/				
				size     = MAX_MSG_LEN_SGL;

			/* Set the DCS type of the message */
			
			if (data->edt_data.TextBuffer[0]==0x80)
			{
				TRACE_EVENT("Unicode SMS message starting 0x80");
				AUI_edit_SetBuffer(&editor_data, ATB_DCS_UNICODE, &data->edt_data.TextBuffer[2], size/2);
			}
			else
			{
				TRACE_EVENT("Normal SMS message");
				AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, data->edt_data.TextBuffer, size);
			}

			AUI_edit_SetDisplay(&editor_data, ZONE_FULL_SK_TITLE_SCROLL, COLOUR_EDITOR, EDITOR_FONT);
			AUI_edit_SetEvents(&editor_data, SMSSEND_ID_TEXTEDIT, FALSE, FOREVER, (T_AUI_EDIT_CB)SmsSend_TEXTEDIT_edit_cb);
			AUI_edit_SetTextStr(&editor_data, TxtSoftOptions, TxtDelete, NULL, NULL);
			AUI_edit_SetAltTextStr(&editor_data, 0, NULL, TRUE, TxtSoftBack);

			data->text_editor = AUI_edit_Start(win,&editor_data);  /* start the editor */
		}
#else /* NEW_EDITOR */

	/* END OF NEW EDITOR CODE */
	
 /*SPR 1434*/
#if defined(CHINESE_MMI) && defined(EASY_TEXT_ENABLED)
/*MC SPR1242, merged section in from b-sample build*/
	if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
	{	T_CHINESE_DATA chinese_data;
	
		chinese_data.TextString = (char*) data->edt_data.TextBuffer;
		/*if empty or unicode string, add unicode tag*/
		if(chinese_data.TextString[0] ==0x00 || chinese_data.TextString[0] ==0x80)
		{	chinese_data.TextString[0] = 0x80;
			chinese_data.TextString[1] = MAX_MSG_LEN;
			
			chinese_data.Callback = (T_EDIT_CB)SmsSend_TEXTEDIT_edit_cb;
		chinese_data.Identifier = SMSSEND_ID_TEXTEDIT ;
		chinese_data.LeftSoftKey = TxtSoftOptions;
		chinese_data.DestroyEditor = FALSE;
#ifdef TI_PS_FF_CONC_SMS		
		if(FFS_flashData.ConcatenateStatus == TRUE)
			chinese_data.EditorSize = (USHORT)((MAX_MSG_LEN_CHI)/2 - 1);
		else
#endif /*TI_PS_FF_CONC_SMS*/			
			chinese_data.EditorSize = (USHORT)((MAX_MSG_LEN_SGL_CHI)/2 - 1);
		data->text_editor = chinese_input(win, &chinese_data);

		}
		else/*if ascii string use ASCII editor rather than converting to unicode, MC, SPR 1292*/
		{	SmsSend_loadEditDefault(&editor_data);
			editor_data.editor_attr.text     = (char *)data->edt_data.TextBuffer;
			//1227 - API - 05/10/02 - Size of Editor dependent on whether Conc_Switch is TRUE or FALSE
#ifdef TI_PS_FF_CONC_SMS			
			if(FFS_flashData.ConcatenateStatus == TRUE)
				editor_data.editor_attr.size     = MAX_MSG_LEN;
			else
#endif /*TI_PS_FF_CONC_SMS*/				
				editor_data.editor_attr.size     = MAX_MSG_LEN_SGL;
			editor_data.min_enter						 = 0; 			 								
			editor_data.LeftSoftKey          = TxtSoftOptions;
			editor_data.AlternateLeftSoftKey = TxtSoftOptions;
			editor_data.RightSoftKey         = TxtDelete;
			editor_data.Identifier           = SMSSEND_ID_TEXTEDIT ; 
			editor_data.Callback             = (T_EDIT_CB)SmsSend_TEXTEDIT_edit_cb;
			editor_data.destroyEditor 		 = FALSE;
			data->text_editor = editor_start(win,&editor_data);  /* start the editor */
		}
	}
else
#endif /* CHINESE_MMI && EASY_TEXT_ENABLED */

{
#ifdef EASY_TEXT_ENABLED
			editor_data.hide									= FALSE;
			editor_data.TextId               = '\0';  
			editor_data.Identifier           =  0; 
			editor_data.TextString           = NULL;  
			editor_data.timeout			  = FOREVER;  // Avoid to return empty strings
			editor_data.mode                 = ALPHA_MODE; 
			editor_attr_init_pred(&editor_data.editor_attr, ZONE_FULL_SK_TITLE, edtCurBar1,  NULL, (char *)data->edt_data.TextBuffer, MAX_MSG_LEN, COLOUR_EDITOR);
#else /* EASY_TEXT_ENABLED */
			SmsSend_loadEditDefault(&editor_data);
			editor_attr_init(&editor_data.editor_attr, ZONE_FULL_SK_TITLE, edtCurBar1, NULL, (char *)data->edt_data.TextBuffer, MAX_MSG_LEN, COLOUR_EDITOR);
#endif /* EASY_TEXT_ENABLED */

			editor_data.editor_attr.text     = (char *)data->edt_data.TextBuffer;
			//1227 - API - 05/10/02 - Size of Editor dependent on whether Conc_Switch is TRUE or FALSE
#ifdef TI_PS_FF_CONC_SMS			
			if(FFS_flashData.ConcatenateStatus == TRUE)
				editor_data.editor_attr.size     = MAX_MSG_LEN;
			else
#endif /*TI_PS_FF_CONC_SMS*/				
				editor_data.editor_attr.size     = MAX_MSG_LEN_SGL;
			editor_data.min_enter						 = 0; 				 								
			editor_data.LeftSoftKey          = TxtSoftOptions;
			editor_data.AlternateLeftSoftKey = TxtSoftOptions;
			editor_data.RightSoftKey         = TxtDelete;

			editor_data.Identifier           = SMSSEND_ID_TEXTEDIT ; 
			editor_data.Callback             = (T_EDIT_CB)SmsSend_TEXTEDIT_edit_cb;
			editor_data.destroyEditor 		 = FALSE;
			data->text_editor = editor_start(win,&editor_data);  /* start the editor */
}
#endif /* NEW_EDITOR */

			/* Marcus: Issue 1170: 07/10/2002: Start */
            /*
             * Set whether status requests are to be indicated according to the
             * value of the srr field of the T_MFW_SMS_INFO structure.
             */
            SmsSend_get_config_data(&sms_parameter);

            switch (sms_parameter.srr)
            {
                case SMS_NOT_REQUESTED_SRR:
                    sms_set_mt_ind(MT_IND_SRR_OFF);
                    break;

                case SMS_REQUESTED_SRR:
                    sms_set_mt_ind(MT_IND_SRR_ON);
                    break;

                default:
                    TRACE_EVENT_P1("default: sms_parameter.srr = %d", sms_parameter.srr);
                    break;
            }
            /* Marcus: Issue 1170: 07/10/2002: Start */

			break;
	case E_RETURN: 
		break;
	case E_EXIT:
		if(data->sc_number_editor != NULL)
		{

			//GW Read config data - then set dcs parameter
			SmsSend_get_config_data(&sms_parameter);

			if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
			{	//We have a chinese text message - set dcs parameter to 
				sms_parameter.dcs = MFW_DCS_UCS2;
    		}
			else
			{
				sms_parameter.dcs = MFW_DCS_7bits;

			}

			// copy the SC number into the sms_parameter variable.
			strcpy((char*)sms_parameter.sc_addr,(char*)data->edt_data.CentreBuffer);
			// store the Service center number into the SIM.
			SmsSend_set_SrvCntr_number (&sms_parameter);
			SmsSend_set_dcs_number (&sms_parameter);

			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			AUI_edit_Destroy(data->sc_number_editor);
#else /* NEW_EDITOR */
			editor_destroy(data->sc_number_editor);
#endif /* NEW_EDITOR */
			data->sc_number_editor = NULL;
		}
		
		break;
	default:
		TRACE_EVENT("Err: Default");
		break;
	}
}

/*******************************************************************************

 $Function:    	SmsSend_PHBK_exec_cb

 $Description:	Callback handler for events sent by the phonebook window
 
 $Returns:		none

 $Arguments:	win - current window
 				event - event id
 				parameter - optional data.
*******************************************************************************/

static void SmsSend_PHBK_exec_cb(T_MFW_HND win, USHORT event, void *parameter)
{
	T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
	T_SEND         * data = (T_SEND *)win_data->user;
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	T_ATB_TEXT		text;	/* Used to insert phone numbers into editors */
#endif

	TRACE_FUNCTION ("SmsSend_PHBK_exec_cb()");

	switch (event)
	{
	/* SPR#1428 - SH - New Editor changes.  Insert phone number into editor */
#ifdef NEW_EDITOR
		case SMS_PHBK_NUMBER:		/* Normal phone number */
		case SMSSC_PHBK_NUMBER:		/* Service centre number */
			text.dcs = ATB_DCS_ASCII;
			text.len = strlen((char *)parameter);
			text.data = (UBYTE *)parameter;
			SEND_EVENT(data->number_editor, E_ED_INSERT, 0, (void *)&text);
			break;
#else /* NEW_EDITOR */
		case SMS_PHBK_NUMBER:
			memcpy(data->edt_data.NumberBuffer, (char *)parameter, MINIMUM(sizeof(data->edt_data.NumberBuffer) - 1, strlen((char *)parameter)+1));
			break;
			/* SPR#1428 - SH - Insert service centre number into editor */
		case SMSSC_PHBK_NUMBER:
			memcpy(data->edt_data.CentreBuffer, (char *)parameter, MINIMUM(sizeof(data->edt_data.CentreBuffer) - 1, strlen((char *)parameter)+1));
			break;			
#endif /* NEW_EDITOR */
		default:
			TRACE_EVENT("Err: Default");
			break;
	}
}


/*******************************************************************************

 $Function:    	SmsSend_TEXTEDIT_exec_cb

 $Description:	Callback handler for events sent by the TEXTEDIT editor
 
 $Returns:		none

 $Arguments:	win - current window
 				event - event id
 				parameter - optional data.
*******************************************************************************/

static void SmsSend_TEXTEDIT_exec_cb(T_MFW_HND win, USHORT event, void *parameter)
{
	TRACE_FUNCTION ("SmsSend_TEXTEDIT_exec_cb()");

	switch (event)
	{
		case E_RETURN:
			// Destroy itself
			SmsSend_SEND_destroy(win);
			break;
		default:
			TRACE_EVENT("Err: Default");
			break;
	}
}

/*******************************************************************************

 $Function:    	SmsSend_NBEDIT_exec_cb

 $Description:	Callback handler for events sent by the NBEDIT editor

 $Returns:		none

 $Arguments:	win - current window
 				event - event id
 				parameter - optional data.
*******************************************************************************/

static void SmsSend_NBEDIT_exec_cb(T_MFW_HND win, USHORT event, void *parameter)
{	
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_SEND         * data = (T_SEND *)win_data->user;
	T_MFW_SMS_INFO sms_parameter;
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
	T_EDITOR_DATA editor_data;
#endif

	TRACE_FUNCTION ("SmsSend_NBEDIT_exec_cb()");

	switch (event)
	{
		case E_INIT:
			if ((phb_get_mode() EQ PHB_RESTRICTED)
					&& (phb_check_number(data->edt_data.NumberBuffer) EQ MFW_PHB_FAIL))
			{
				mmi_dialog_information_screen(win,TxtNotImplemented, "NOT ALLOWED", (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_NOT_ALLOWED);
			}
			else
			{
				
				if (!smsidle_get_ready_state())
				{
					// SMS init processing not ready to access SIM to store the SMS
					/* MZ cq11074 do not invoke the editor until SMS Service Centre number has been read, 
					    display info dialog. */
					mmi_dialog_information_screen(win,TxtPleaseWait, "NOT READY", (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_SC_NOT_READY );
					return;
				// Recreate the editor on dialog callback
				}
				else
				{
					//END TB
					// First check if the SMS center is specified
					SmsSend_get_config_data(&sms_parameter);

					if(sms_parameter.sc_addr[0] EQ '\0')
					{
						memset(data->edt_data.CentreBuffer,0,NUMBER_LENGTH);
					}
					else
					{
						strncpy((char *)data->edt_data.CentreBuffer, (char *)sms_parameter.sc_addr, NUMBER_LENGTH);
					}
				}
			}

			/* SPR#1428 - SH - New Editor changes */
			
#ifdef NEW_EDITOR
			AUI_edit_SetDefault(&editor_data);
			AUI_edit_SetDisplay(&editor_data, CENTRE_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
			AUI_edit_SetEvents(&editor_data, 0, FALSE, FOREVER, (T_AUI_EDIT_CB)SmsSend_SCNBEDIT_edit_cb);
			AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtDelete, TxtServiceCentre, NULL);
			AUI_edit_SetAltTextStr(&editor_data, 1, TxtNames, TRUE, TxtSoftBack);
			AUI_edit_SetMode(&editor_data, 0, ED_CURSOR_UNDERLINE);
			AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, data->edt_data.CentreBuffer, NUMBER_LENGTH);
			data->number_editor = AUI_edit_Start(data->win, &editor_data);
#else /* NEW_EDITOR */

	    	SmsSend_loadEditDefault(&editor_data);

			bookSetEditAttributes( CENTRE_EDITOR, COLOUR_EDITOR_XX,0,edtCurBar1, 0, 
			(char *) data->edt_data.CentreBuffer, NUMBER_LENGTH, &editor_data.editor_attr);
			
			editor_data.TextId            = TxtServiceCentre;  
			editor_data.editor_attr.text     = (char *)data->edt_data.CentreBuffer;  
			editor_data.editor_attr.size     = NUMBER_LENGTH; 
			editor_data.LeftSoftKey        = TxtSoftOK;
			editor_data.AlternateLeftSoftKey = TxtNames;
			editor_data.RightSoftKey       = TxtDelete;
			editor_data.Identifier           = NULL; 
			editor_data.Callback           = (T_EDIT_CB)SmsSend_SCNBEDIT_edit_cb;
			editor_data.mode		    = E_EDIT_DIGITS_MODE;
			editor_data.destroyEditor		 = FALSE;
			data->number_editor = editor_start(data->win,&editor_data);  
#endif /* NEW_EDITOR */

		/* start the editor */ 
			
			break;
		default:
			TRACE_EVENT("Err: Default");
			break;
	}
}

/*******************************************************************************

 $Function:    	SmsSend_CENTREEDIT_exec_cb

 $Description:	Callback handler for events sent by the CENTREEDIT editor

 $Returns:		none

 $Arguments:	win - current window
 				event - event id
 				parameter - optional data.
*******************************************************************************/

static void SmsSend_CENTREEDIT_exec_cb(T_MFW_HND win, USHORT event, void *parameter)
{	
	T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
	T_SEND         * data = (T_SEND *)win_data->user;

#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
	T_EDITOR_DATA editor_data;
#endif

	TRACE_FUNCTION ("SmsSend_CENTREEDIT_exec_cb()");

	switch (event)
	{
	case E_INIT:
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			AUI_edit_SetDefault(&editor_data);
			AUI_edit_SetDisplay(&editor_data, CENTRE_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
			AUI_edit_SetEvents(&editor_data, *(USHORT*)parameter, TRUE, FOREVER, (T_AUI_EDIT_CB)SmsSend_CENTREEDIT_edit_cb);
			AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtDelete, TxtServiceCentre, NULL);
			AUI_edit_SetAltTextStr(&editor_data, 1, TxtNull, TRUE, TxtSoftBack);
			AUI_edit_SetMode(&editor_data, 0, ED_CURSOR_UNDERLINE);
			AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, data->edt_data.CentreBuffer, PHB_MAX_LEN);
			data->sc_number_editor = AUI_edit_Start(data->win, &editor_data);
#else /* NEW_EDITOR */

			SmsSend_loadEditDefault(&editor_data);
			bookSetEditAttributes( CENTRE_EDITOR, COLOUR_EDITOR_XX, 0, edtCurBar1, 0, 
			(char *) data->edt_data.CentreBuffer, NUMBER_LENGTH, &editor_data.editor_attr);
			
			editor_data.TextId            = TxtServiceCentre;  
			editor_data.editor_attr.text     = (char *)data->edt_data.CentreBuffer;  
			editor_data.editor_attr.size     = PHB_MAX_LEN; 
			editor_data.LeftSoftKey        = TxtSoftOK;
			editor_data.RightSoftKey       = TxtDelete;
			editor_data.Identifier 	    = *(USHORT*)parameter;
			editor_data.Callback           = (T_EDIT_CB)SmsSend_CENTREEDIT_edit_cb;
			editor_data.mode		    = E_EDIT_DIGITS_MODE;
			editor_data.destroyEditor        = TRUE ; 
			data->sc_number_editor = editor_start(data->win,&editor_data);  
		/* start the Service Centre Number editor */ 
#endif /* NEW_EDITOR */
		break;
		
	case E_RETURN:
		
		break;
	default:
		TRACE_EVENT("Err: Default");
		return;
	}
}

/*******************************************************************************

 $Function:    	SmsSend_TEXTEDIT_edit_cb

 $Description:	Callback function for the text editor

 $Returns:		none

 $Arguments:	win - current window
 				Identifier - unique id
 				reason - event cause
*******************************************************************************/

static void SmsSend_TEXTEDIT_edit_cb (T_MFW_HND win, USHORT Identifier, SHORT reason)
{
	T_MFW_WIN			* win_data = ((T_MFW_HDR *) win)->data;
	T_SEND				* data     = (T_SEND *)win_data->user;
	
	TRACE_FUNCTION ("SmsSend_TEXTEDIT_edit_cb()");
	
	//	Jul 13, 2004    REF: CRR 21615  Deepa M.D 
	//Copy the sms content from the editor buffer into the temporary buffer.
	memcpy(TempTextBuffer,data->edt_data.TextBuffer,MAX_MSG_LEN);
	
	switch (reason)
	{
	case INFO_KCD_ALTERNATELEFT:
	case INFO_KCD_LEFT:
		data->options_win = SmsSend_OPT_start(win,(MfwMnuAttr*)&SmsSend_R_OPTAttrib);
		
		/* Create number editor:                                 
		-the editor min length is 1
		*/
		break;
		
	case INFO_KCD_RIGHT:
	case INFO_KCD_HUP:
		// Commit suicide so come back to previous menu
#if	defined(CHINESE_MMI) && defined(EASY_TEXT_ENABLED)
		if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
			chinese_input_destroy(data->text_editor);
		else
#endif
			
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			AUI_edit_Destroy(data->text_editor);
#else /* NEW_EDITOR */
		editor_destroy(data->text_editor);
#endif /* NEW_EDITOR */
		
		data->text_editor = 0;
		SmsSend_SEND_destroy(data->win);
		break;
		
	default:
		TRACE_EVENT("Err: Default");
		break;
	}
}


/*******************************************************************************

 $Function:    	SmsSend_NBEDIT_edit_cb

 $Description:	Callback function for the number editor

 $Returns:		none

 $Arguments:	win - current window
 				Identifier - unique id
 				reason - event cause
*******************************************************************************/

static void SmsSend_NBEDIT_edit_cb (T_MFW_HND win, USHORT Identifier, SHORT reason)
{	
	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	T_SEND      * data     = (T_SEND *)win_data->user;

	T_MFW_SMS_INFO	sms_parameter;
	USHORT parameter;
	
	TRACE_FUNCTION ("SmsSend_NBEDIT_edit_cb()");

	
	switch (reason)
	{
		case INFO_KCD_LEFT:
	  		// Check if we can go to the next stage
			if ( (phb_check_number(data->edt_data.NumberBuffer) EQ MFW_PHB_FAIL) && (phb_get_mode() EQ PHB_RESTRICTED))//ACT_X0073106-BMI-5X-OMAPS00133906
			{
				
				mmi_dialog_information_screen(win,TxtNotImplemented, "NOT ALLOWED", (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_NOT_ALLOWED);

			}
			else
			{
				
				if (!smsidle_get_ready_state())
				{
					// SMS init processing not ready to access SIM to store the SMS
					mmi_dialog_information_screen(win,TxtPleaseWait, "NOT READY", (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_SC_NOT_READY);
				// Recreate the editor on dialog callback
				}
				else
				{
					//END TB
					// First check if the SMS center is specified
					SmsSend_get_config_data(&sms_parameter);

					if(sms_parameter.sc_addr[0] EQ '\0')
					{
						//setup identifier value.
						parameter = SMSSEND_ID_CENTREEDIT;
						SmsSend_CENTREEDIT_exec_cb(win, E_INIT,(void*)&parameter);
					}
					else
					{
						/*NM FTA 34.2.8*/
						if (data->edt_data.called_from_reply_item EQ FALSE)
						{
							TRACE_EVENT("take the own serviceCenternum");
							strcpy((char *)data->edt_data.CentreBuffer, (char *)sms_parameter.sc_addr);

							   {
								   /***************************Go-lite Optimization changes Start***********************/
								   //	Aug 16, 2004    REF: CRR 24323   Deepa M.D
								   TRACE_EVENT_P1("serviceNr:%s",data->edt_data.CentreBuffer);
								   /***************************Go-lite Optimization changes end***********************/
							   }
															
						}
						else
						{
							TRACE_EVENT("from the caller, reply");
						
							/* in case of reply the sms, we took already the 
							   the service center number from the caller !!!*/

								{
									/***************************Go-lite Optimization changes Start***********************/
									//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
									TRACE_EVENT_P1("serviceNr:%s",data->edt_data.CentreBuffer);
									/***************************Go-lite Optimization changes end***********************/
								}
						}
						/*NM FTA 34.2.8*/
						SmsSend_send_to_mfw(win);
					}
				}
			}

			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			AUI_edit_Destroy(data->number_editor);
#else /* NEW_EDITOR */
			editor_destroy(data->number_editor);
#endif /* NEW_EDITOR */

			data->number_editor = 0;
			SmsSend_R_OPT_destroy(data->options_win);

		break;
          
		case INFO_KCD_ALTERNATELEFT:
			bookPhonebookStart(win, PhbkFromSms);
			break;
		case INFO_KCD_RIGHT:
		case INFO_KCD_HUP:
		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			AUI_edit_Destroy(data->number_editor);
#else /* NEW_EDITOR */
			editor_destroy(data->number_editor);
#endif /* NEW_EDITOR */
			data->number_editor = 0;
			break;

		default:
			TRACE_EVENT("Err: Default");
			break;
	}
}

/*******************************************************************************

 $Function:    	SmsSend_CENTREEDIT_edit_cb

 $Description:	Callback function for the center number editor

 $Returns:		none

 $Arguments:	win - current window
 				Identifier - unique id
 				reason - event cause
*******************************************************************************/

static void SmsSend_CENTREEDIT_edit_cb (T_MFW_HND win, USHORT Identifier, SHORT reason)
{	
//	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;   // RAVI
//	T_SEND      * data     = (T_SEND *)win_data->user;           // RAVI

	TRACE_FUNCTION ("SmsSend_CENTREEDIT_edit_cb()");
	switch (reason)
	{
	case INFO_KCD_LEFT:
		TRACE_EVENT("SmsSend_CENTREEDIT_edit_cb,INFO_KCD_LEFT ");
		if (Identifier==SMSSEND_ID_CENTREEDIT)
			{
				TRACE_EVENT("SmsSend_CENTREEDIT_edit_cb,INFO_KCD_LEFT Send to mfw ");
				
				//Submit the SMS message to the service centre.
				SmsSend_send_to_mfw(win);
			}
		break;
  
	case INFO_KCD_RIGHT:
	case INFO_KCD_HUP:
		/* send an event that will lead to the display of the number edition window */
	    SEND_EVENT (win, E_RETURN, SMSSEND_ID_CENTREEDIT, NULL);
		break;
	default:
		
		break;
	}
}

/*******************************************************************************

 $Function:    	SmsSend_SEND_mfw_cb

 $Description:	Callback functyion for handling SMS events from the MFW.

 $Returns:		execution status

 $Arguments:	event - window event id
 				info -  optional info
 				reason - event cause
*******************************************************************************/

static int SmsSend_SEND_mfw_cb (MfwEvt event, void *info)
{
	/* get the top window*/
	T_MFW_HND win         = mfw_parent(mfw_header());
	T_MFW_WIN  * win_data = ((T_MFW_HDR *)win)->data;
	T_SEND     * data     = (T_SEND *)win_data->user;


	TRACE_FUNCTION ("SmsSend_SEND_mfw_cb()");

	switch (event)
	{
		case E_SMS_MO_AVAIL:
			TRACE_EVENT ("Sent was successful -> ok");
			/* Sent was successful */ 
			/* abort the current Dialog "Please wait" */ 
			if (data->child_dialog != NULL)
			{
				SEND_EVENT (data->child_dialog, DIALOG_DESTROY, 0, NULL);
				data->child_dialog = NULL; 
			}
			//GW Display sent not send
			/* FTA 34.2.2 - CQ 101443 - API & MZ pass NULL instead of win to the dialog window*/
			//Sep 01, 2004    REF: CRR 21380  Deepa M.D 
			//Second string is same as the TextId .Hence passed NULL.
			mmi_dialog_information_screen(NULL,TxtSent, NULL, (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_SENT);
			data->id = SMSSEND_ID_SENT; 

			/* xreddymn OMAPS00067510 Jun-15-2006 */
			if (msgBuffer != NULL)
			{
				FREE_MEMORY((void*)msgBuffer, MAX_MSG_LEN_ARRAY);
		  		msgBuffer = NULL;
			}
			break;
			
		case E_SMS_ERR:
			// Waiting dialog is destroyed
			if (data->child_dialog != NULL)
			{
				SEND_EVENT (data->child_dialog, DIALOG_DESTROY, 0, NULL);
				data->child_dialog = NULL; 
			}
			// and the status dialog is displayed during 3 secs.
			/* FTA 34.2.2 - CQ 101443 - API & MZ pass NULL instead of win to the dialog window */
			 //Sep 01, 2004    REF: CRR 21380  Deepa M.D 
			//Second string is same as the TextId .Hence passed NULL.
			mmi_dialog_information_screen(NULL,TxtNotSent, NULL, (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_NOT_SENT);
			data->id = SMSSEND_ID_NOT_SENT; //TB3

			/* xreddymn OMAPS00067510 Jun-15-2006 */
			if (msgBuffer != NULL)
			{
				FREE_MEMORY((void*)msgBuffer, MAX_MSG_LEN_ARRAY);
		  		msgBuffer = NULL;
			}
			break;
       	
    default:
			TRACE_EVENT("Err: default");
			return 0;      	   
   	}

	return 1;
}

/*******************************************************************************

 $Function:    	SmsSend_SAVE_start

 $Description:	Create the Save SMS text window.

 $Returns:		mfw window handle

 $Arguments:	parent_window - parent window 
 				UserData - SMS info

*******************************************************************************/

T_MFW_HND SmsSend_SAVE_start(T_MFW_HND parent_window, T_SmsUserData *UserData)
{
    T_MFW_HND win;

    TRACE_FUNCTION ("SmsSend_SAVE_start()");

    win = SmsSend_SAVE_create (parent_window);

    if (win NEQ NULL)
	{
	    SEND_EVENT (win, E_INIT, 0, (void *)UserData);
	}
    return win;
}

/*******************************************************************************

 $Function:    	SmsSend_SAVE_create

 $Description:	Create a window for info dialog

 $Returns:		mfw window handle

 $Arguments:	parent_window - parent window 

*******************************************************************************/

static T_MFW_HND SmsSend_SAVE_create(MfwHnd parent_window)
{
	T_SAVE     * data = (T_SAVE *)ALLOC_MEMORY (sizeof (T_SAVE));
	T_MFW_WIN  * win;

	TRACE_FUNCTION ("SmsSend_SAVE_create()");

	/*
	 * Create window handler
	 */
	data->win = win_create (parent_window, 0, E_WIN_VISIBLE, NULL);
	if (data->win EQ NULL)
	{
		return NULL;
	}

	/* 
	 * connect the dialog data to the MFW-window
	 */
	data->mmi_control.dialog = (T_DIALOG_FUNC)SmsSend_SAVE_exec_cb;
	data->mmi_control.data   = data;
	win                      = ((T_MFW_HDR *)data->win)->data;
	win->user                = (MfwUserDataPtr)data;
	data->parent             = parent_window;
	data->id				 = SAVE_ID;

	winShow(data->win);
	return data->win;
}

/*******************************************************************************

 $Function:    	SmsSend_SAVE_destroy

 $Description:	Destroy the windows for info dialog

 $Returns:		none

 $Arguments:	own_window - current window 

*******************************************************************************/

static void SmsSend_SAVE_destroy(MfwHnd own_window)
{
  T_MFW_WIN * win_data;
  T_SAVE    * data = NULL; 

  TRACE_FUNCTION ("SmsSend_SAVE_destroy()");

  if (own_window)
	{
	  win_data = ((T_MFW_HDR *)own_window)->data;
		if (win_data) 
	    data = (T_SAVE *)win_data->user;

	  if (data)
		{
			// Delete sms handle
			if (data->sms_handler != NULL)
				sms_delete(data->sms_handler);

			if (data->child_dialog != NULL)
			{
				TRACE_EVENT("child_dialog not deleted?");
			}

			// Delete WIN handler 
		  win_delete (data->win);

		  // Free Memory
		  FREE_MEMORY ((void *)data, sizeof (T_SAVE));
			
		}
	  else
		{
		    TRACE_EVENT ("SmsSend_SAVE_destroy() called twice");
		}
	}
}

/*******************************************************************************

 $Function:    	SmsSend_SAVE_exec_cb

 $Description:	Callback handler for events sent to to trigger execution

 $Returns:		none

 $Arguments:	own_window - current window 

*******************************************************************************/

void SmsSend_SAVE_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
	T_SAVE         * data = (T_SAVE *)win_data->user;
	T_MFW_SMS_INFO	sms_parameter;
	USHORT outLen;
	T_MFW store_status;
	T_SmsUserData* message_parameter;/*SPR1991, pointer to message text, dest number*/
 	 /*JVJ SPR1298 made dynamic to avoid stack overload in the function convertToUnicodeForOutput*/
	/* MZ 10202 Removed memory declaration and allocation*/

	TRACE_FUNCTION ("SmsSend_SAVE_exec_cb()");

	switch (event)
	{
		case E_INIT:		
			
			/*MZ issue 10202 only alloc memory when you need it */
			if (msgBuffer ==NULL)
				msgBuffer = (UBYTE*)ALLOC_MEMORY(MAX_MSG_LEN_ARRAY);
			
			data->child_dialog = NULL; 

			/* Create a sms handler to receive events  */
			data->sms_handler = sms_create(data->win,
														E_SMS_SAVE_AVAIL | E_SMS_ERR | E_SMS_MEM_FULL
														,(MfwCb)SmsSend_SAVE_mfw_cb);

			//
			/*SPR1991, set local pointer to passed parameter, rather than copying data and
			using up more dynamic memory*/
			message_parameter = parameter;
			if (message_parameter->TextBuffer[0] == 0x80)
			{	//String is unicode - 
				sms_parameter.dcs = MFW_DCS_UCS2;
			}
			else
			{
				sms_parameter.dcs = MFW_DCS_7bits;
			}

			SmsSend_set_dcs_number (&sms_parameter);

			#ifdef NO_ASCIIZ
			{
				// have to add this , later
				    convertToUnicodeForOutput(message_parameter->TextBuffer,msgBuffer, &outLen);
				TRACE_EVENT_P1("Save SMS, length: %d", outLen);

					store_status = sms_store(MFW_SMS_SUBMIT, (char*)message_parameter->NumberBuffer, msgBuffer, outLen, NULL);
			}
			#else		
			{
				convertToUnicodeForOutput(data->edt_data.TextBuffer,msgBuffer, &outLen);
				store_status = sms_store(MFW_SMS_SUBMIT, (char*)message_parameter->NumberBuffer, msgBuffer, outLen, NULL);
				// sbh - added length to the above function call, since '@' characters stored as NULL & will stop
				// strlen from working properly.
			}
			#endif

			/*  display "Please wait" for save sms */
			if (store_status EQ MFW_SMS_OK)
			{
				TRACE_EVENT ("Save operation  -> ok");
				/* show now to user "Please wait" for save*/
				// Do not start please wait if ever the MFW result event as already been received
				if (data->child_dialog == NULL)
				{
					// Feb 02, 2005   REF: CRR 28479   xnkulkar
					// Call function "mmi_dialog_information_screen_save_sms" which has only 
	       			// KEY_RIGHT and  KEY_HUP registered instead of KEY_ALL
					//data->child_dialog = mmi_dialog_information_screen(win,TxtPleaseWait, "SAVING", (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_WAIT_SAVE);
					data->child_dialog = mmi_dialog_information_screen_save_sms(win,TxtPleaseWait, "SAVING", (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_WAIT_SAVE);
				}
				
			}
			else
			{
				TRACE_EVENT ("Save operation -> failed");
				//Sep 01, 2004    REF: CRR 21380  Deepa M.D 
				//If E_SMS_MEM_FULL event  has already been received, then destroy the Messagefull Dialog
				//and dispaly a new dialog "Full NOT SAVED".
				if(memfull==TRUE)
				{
					// Feb 02, 2005   REF: CRR 28479   xnkulkar
					// Call function "mmi_dialog_information_screen_save_sms" which has only 
	       			// KEY_RIGHT and  KEY_HUP registered instead of KEY_ALL
					//mmi_dialog_information_screen(win, TxtFull,"NOT SAVED", (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_NOT_SAVED);
					mmi_dialog_information_screen_save_sms(win, TxtFull,"NOT SAVED", (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_NOT_SAVED);
					memfull=FALSE;//Set the memful flag to False.
					
				}
				// If the saving message failed due to other reason than memory full, display this message
				else if (data->child_dialog == NULL)
				{
					// Feb 02, 2005   REF: CRR 28479   xnkulkar
					// Call function "mmi_dialog_information_screen_save_sms" which has only 
	       			// KEY_RIGHT and  KEY_HUP registered instead of KEY_ALL
					//mmi_dialog_information_screen(win, TxtFailed,"NOT SAVED", (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_NOT_SAVED);
					mmi_dialog_information_screen_save_sms(win, TxtFailed,"NOT SAVED", (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_NOT_SAVED);
				}
				
			}
		/*SPr 1991, check memory allocated before deallocatting it*/
		/* MZ 10202 Remove the memory deallocation until later.*/

		break;

	default:
		TRACE_EVENT("Err: default");
	break;
	}
   
    return;
}

/*******************************************************************************

 $Function:    	SmsSend_SAVE_mfw_cb

 $Description:	Handles SMS events from the MFW.

 $Returns:		none

 $Arguments:	event - event id
 				info - optional info

*******************************************************************************/

static int SmsSend_SAVE_mfw_cb (MfwEvt event, void *info)
{
	/* get the top window*/
	T_MFW_HND win         = mfw_parent(mfw_header());
	T_MFW_WIN  * win_data = ((T_MFW_HDR *)win)->data;
	T_SAVE     * data     = (T_SAVE *)win_data->user;

//	T_DISPLAY_DATA display_info;
  
	TRACE_FUNCTION ("SmsSend_SAVE_mfw_cb");

	switch (event)
	{
		/* Save is successful */   
    case E_SMS_SAVE_AVAIL:
		// Waiting dialog is destroyed
		
		if (data->child_dialog != NULL)
		{
			SEND_EVENT (data->child_dialog, DIALOG_DESTROY, 0, NULL);
			data->child_dialog = NULL; 
		}
		//Sep 01, 2004    REF: CRR 21380  Deepa M.D 
		//Second string is same as the TextId .Hence passed NULL.

		// Feb 02, 2005   REF: CRR 28479   xnkulkar
		// Call function "mmi_dialog_information_screen_save_sms" which has only 
	       // KEY_RIGHT and  KEY_HUP registered instead of KEY_ALL
	       
		// mmi_dialog_information_screen(win, TxtSaved,NULL, (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_SAVED);
		mmi_dialog_information_screen_save_sms(win, TxtSaved,NULL, (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_SAVED);

		/* xreddymn OMAPS00067510 Jun-15-2006 */
		if (msgBuffer != NULL)
		{
			FREE_MEMORY((void*)msgBuffer, MAX_MSG_LEN_ARRAY);
	  		msgBuffer = NULL;
		}
		break;
		
	case E_SMS_MEM_FULL:
		//Sep 01, 2004    REF: CRR 21380  Deepa M.D 
		//Flag to check E_SMS_MEM_FULL event has already been received.
		memfull=TRUE;
		TRACE_EVENT ("Memory for sms save is full");
		
		if (data->child_dialog != NULL)
		{
			// Waiting dialog is destroyed
			SEND_EVENT (data->child_dialog, DIALOG_DESTROY, 0, NULL);
			data->child_dialog = NULL; 
		}
		// and the status dialog is displayed during 3 secs.
		// set data->child_dialog as message received before sms_store is ended
		//Sep 01, 2004    REF: CRR 21380  Deepa M.D 
		//Second string is same as the TextId .Hence passed NULL.
		mmi_dialog_information_screen(win,TxtFull, NULL, (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_LIST_FULL);

		/* xreddymn OMAPS00067510 Jun-15-2006 */
		if (msgBuffer != NULL)
		{
			FREE_MEMORY((void*)msgBuffer, MAX_MSG_LEN_ARRAY);
	  		msgBuffer = NULL;
		}
		return MFW_EVENT_PASSED; /*SPR 2640, pass event along to idle screen*/
	//	break;   // RAVI
		
    case E_SMS_ERR:
		TRACE_EVENT ("Save sms failed -> nok");
		
		if (data->child_dialog != NULL)
		{
			// Waiting dialog is destroyed
			SEND_EVENT (data->child_dialog, DIALOG_DESTROY, 0, NULL);
			data->child_dialog = NULL; 
		}
		// and the status dialog is displayed during 3 secs.
		// set data->child_dialog if message received before sms_store is ended
		mmi_dialog_information_screen(win,TxtFailed, "NOT SAVED", (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_NOT_SAVED);

		/* xreddymn OMAPS00067510 Jun-15-2006 */
		if (msgBuffer != NULL)
		{
			FREE_MEMORY((void*)msgBuffer, MAX_MSG_LEN_ARRAY);
	  		msgBuffer = NULL;
		}
		break;
		
	default:
		TRACE_EVENT("Err: default");
		return 0;
   	}	
	return 1;
}

/*******************************************************************************

 $Function:    	SmsSend_send_to_mfw

 $Description:	This function sends a SMS message.

 $Returns:		execution status

 $Arguments:	event - current window

 $History
  GW 09/10/01 - Added code to convert unicode strings from 8 bits to 7 bits.

*******************************************************************************/

static int SmsSend_send_to_mfw (T_MFW_HND win)
{  
	T_MFW_WIN				* win_data = ((T_MFW_HDR *)win)->data;
	T_SEND					* data     = (T_SEND *)win_data->user;
	T_MFW						submit_status;   /* Result of SMS send operation. */
	T_MFW_SMS_INFO	sms_parameter;

	USHORT outLen;

	/*SPR2453 only alloc memory when you need it */
	if (msgBuffer ==NULL)
		msgBuffer = (UBYTE*)ALLOC_MEMORY(MAX_MSG_LEN_ARRAY);
	TRACE_FUNCTION ("SmsSend_send_to_mfw()");
	if (data->edt_data.TextBuffer[0] == 0x80)
	{	//String is unicode - 
		sms_parameter.dcs = MFW_DCS_UCS2;
	}
	else
	{
		sms_parameter.dcs = MFW_DCS_7bits;
	}

	SmsSend_set_dcs_number (&sms_parameter);
	/* Send the SMS */
        
  
	
	#ifdef NO_ASCIIZ
	{

		convertToUnicodeForOutput(data->edt_data.TextBuffer,msgBuffer, &outLen);

	TRACE_EVENT_P1("Send SMS, length: %d", outLen);
			submit_status = sms_submit( MFW_SMS_SUBMIT,                  /* type of the message*/
        				(char *)data->edt_data.NumberBuffer,        /* dest. address      */
								msgBuffer,			                  /* sms message        */
								outLen,          /* lenght of message  */
								(char *)data->edt_data.CentreBuffer);/* sevice center num. */
	}
	#else
	{
		convertToUnicodeForOutput(data->edt_data.TextBuffer,msgBuffer,&outLen);
        #if defined (FF_WAP) && defined (FF_GPF_TCPIP)

    		submit_status = sms_submit( MFW_SMS_SUBMIT,  /* type of the message*/
            			(char *)data->edt_data.NumberBuffer, /* dest. address      */
            			NULL,                                /* don't specify toa for now */
		                msgBuffer,                           /* sms message        */
		                outLen,                             /* SH - added this, see function definition */
		                (char *)data->edt_data.CentreBuffer,    /* sevice center num. */
		                NULL);                              /* don't specify tosca for now */
		#else
    		submit_status = sms_submit( MFW_SMS_SUBMIT,  /* type of the message*/
				(char *)data->edt_data.NumberBuffer, /* dest. address      */
				msgBuffer,							 /* sms message        */
				outLen,								// sbh - added this, see function definition
                (char *)data->edt_data.CentreBuffer);/* sevice center num. */


		#endif

	}
	#endif
		
	

	/*  display "Please wait" for sent sms */
	if (submit_status EQ MFW_SMS_OK)
	{
		T_DISPLAY_DATA display_info;
	
		TRACE_EVENT ("to give SMS to MFW -> ok");
		/* show the use "Please wait" for send sms */
		dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtSending, TxtMessage , COLOUR_STATUS);
		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)SmsSend_standard_dialog_cb, FOREVER, KEY_LEFT );
		display_info.Identifier   = SMSSEND_ID_WAIT_SEND;

		data->child_dialog = info_dialog(win, &display_info);
		data->id = SMSSEND_ID_SENT;
	}
	else
	{
		TRACE_EVENT ("to give SMS to MFW -> failed");
		/* the result operation of sent was NOT OK*/
		//Sep 01, 2004    REF: CRR 21380  Deepa M.D 
		//Second string is same as the TextId .Hence passed NULL.
		mmi_dialog_information_screen(win,TxtNotSent, NULL, (T_VOID_FUNC)SmsSend_standard_dialog_cb, SMSSEND_ID_NOT_SENT);
		// No forever dialog to be destroyed
		data->child_dialog = NULL;
		data->id = SMSSEND_ID_NOT_SENT;
	}

    /*SPR 2453, don't deallocate memory till sending complete*/
	return 1;
}


/*******************************************************************************

 $Function:    	SmsSend_get_config_data

 $Description:	Called by external processes to read SMS PP config data.

 $Returns:		execution status

 $Arguments:	config_data - SMS info

*******************************************************************************/

UBYTE SmsSend_get_config_data (T_MFW_SMS_INFO *config_data)
{
	T_MFW Submit_Status;   /* store theh result of SMS send operation. */

  TRACE_FUNCTION ("SmsSend_get_config_data()");
  // get the current config data from SIM 
	Submit_Status = sms_parameter(config_data, TRUE);

	if (Submit_Status == MFW_SMS_FAIL)
	{
		TRACE_EVENT  ("ret MFW_SMS_FAIL");
		return FALSE;		
	}
	else
	{
		TRACE_EVENT ("ret OK");
		return TRUE;
	}
}
/*******************************************************************************

 $Function:    	SmsSend_set_SrvCntr_number

 $Description:	Called by external processes to write/Set SMS PP config data.

 $Returns:		execution status

 $Arguments:	config_data - SMS info

*******************************************************************************/

UBYTE SmsSend_set_SrvCntr_number (T_MFW_SMS_INFO *config_data)
{
	T_MFW Submit_Status;   /* store theh result of SMS send operation. */

  TRACE_FUNCTION ("SmsSend_set_SrvCntr_number()");
  // get the current config data from SIM 
	Submit_Status = sms_set_SrvCntr_number(config_data);

	if (Submit_Status == MFW_SMS_FAIL)
	{
		TRACE_EVENT  ("ret MFW_SMS_FAIL");
		return FALSE;		
	}
	else
	{
		TRACE_EVENT ("ret OK");
		return TRUE;
	}
}
UBYTE SmsSend_set_dcs_number (T_MFW_SMS_INFO *config_data)
{
	T_MFW Submit_Status;   /* store theh result of SMS send operation. */

  TRACE_EVENT ("SmsSend_set_dcs_number()");
  // get the current config data from SIM 
	Submit_Status = sms_set_dcs_number(config_data);

	if (Submit_Status == MFW_SMS_FAIL)
	{
		TRACE_EVENT  ("ret MFW_SMS_FAIL");
		return FALSE;		
	}
	else
	{
		TRACE_EVENT ("ret OK");
		return TRUE;
	}
}

/* SPR#1428 - SH - New Editor: no longer required */
#ifndef NEW_EDITOR
/*******************************************************************************

 $Function:    	SmsSend_loadEditDefault

 $Description:	Configure T_EDITOR_DATA structure with default editor settings.

 $Returns:		none
 
 $Arguments:	editor_data - editor data.

*******************************************************************************/

void SmsSend_loadEditDefault(T_EDITOR_DATA *editor_data)
{
	    TRACE_FUNCTION ("SmsSend_loadEditDefault()");

		editor_attr_init(&editor_data->editor_attr, NULL, edtCurBar1, 0, 0, 0, COLOUR_EDITOR);
		editor_data_init(editor_data, NULL, TxtSoftSelect, TxtSoftBack, 0, 1, ALPHA_MODE, FOREVER);
		editor_data->hide									= FALSE;
		editor_data->AlternateLeftSoftKey = TxtNull;
		editor_data->Identifier           =  0; 
		editor_data->TextString           = NULL;  
		editor_data->destroyEditor        = TRUE ; 
}


/*******************************************************************************

 $Function:    	SmsSend_loadUCS2EditDefault

 $Description:	Configure T_EDITOR2_DATA structure with default editor settings.

 $Returns:		none
 
 $Arguments:	editor_data - editor data.

*******************************************************************************/

static void SmsSend_loadUCS2EditDefault(T_EDITOR_DATA *editor_data)
{
	TRACE_FUNCTION ("SmsSend_loadUCS2EditDefault()");

	memset(editor_data, 0, sizeof(T_EDITOR_DATA));
	editor_data->hide							= FALSE; 
	editor_data->LeftSoftKey					= TxtSend;
	editor_data->AlternateLeftSoftKey			= TxtSend;
	editor_data->RightSoftKey					= TxtSave;
	editor_data->Identifier						= SMSSEND_ID_TEXTEDIT;
	editor_data->mode							= ALPHA_MODE;
	editor_data->min_enter						= 0;
	editor_data->timeout						= FOREVER;
	editor_data->Callback						= (T_EDIT_CB)SmsSend_TEXTEDIT_edit_cb;
}
#endif /* NEW_EDITOR */


/*******************************************************************************

 $Function:    	SmsSend_standard_dialog_cb

 $Description:	Callback function for standard dialogs in smssend module

 $Returns:		none
 
 $Arguments:	win - current window
 				Identifier - unique id 
 				Reason - event id

*******************************************************************************/

static int SmsSend_standard_dialog_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason)
{
	T_MFW_WIN   * win_data		= ((T_MFW_HDR *) win)->data;
	T_SAVE		  * save_data		= NULL;
	T_SEND			* send_data		= NULL;
  	T_MFW_HND   * l_parent		= NULL; 
  
    TRACE_FUNCTION ("SmsSend_standard_dialog_cb()");
	


	switch (Identifier)
	{
	
		case SMSSEND_ID_SAVE_NOT_READY:
		case SMSSEND_ID_SC_NOT_READY:
			// SEND an event to recreate the text editor
			// or to recreate the destination address number editor
			SEND_EVENT (win, E_RETURN, Identifier, NULL);
			break;
			
		case SMSSEND_ID_WAIT_SEND:
			if (win_data != NULL)
				{
					send_data = (T_SEND *)win_data->user;
					// Forever child dialog has been destroyed
					if (send_data != NULL)
						send_data->child_dialog = NULL;
				}
			break;

		case SMSSEND_ID_WAIT_SAVE:
			// There is no way to stop SMS send
			if (win_data != NULL)
				{
					save_data = (T_SAVE *)win_data->user;
					// Forever child dialog has been destroyed
					if (save_data != NULL)
						save_data->child_dialog = NULL;
				}
			break;
		
		// Destroy the window then send the EXIT event to the SEND window->display of the text editor
		case SMSSEND_ID_SENT:
		case SMSSEND_ID_NOT_SENT:
			if ((Reason EQ INFO_KCD_RIGHT) || (Reason EQ INFO_KCD_HUP) || (Reason EQ INFO_TIMEOUT))
				SEND_EVENT(win, E_EXIT, SEND_ID, NULL);
			// Do not reset child dialog handle, as 3s sending dialogs handle are not saved
			break;

		// Destroy the SAVE window and send an E_EXIT event to the parent window (useful if some actions are needed on return like in SEND)
		case SMSSEND_ID_SAVED:
		case SMSSEND_ID_NOT_SAVED:
		case SMSSEND_ID_LIST_FULL:
			if ((Reason EQ INFO_KCD_RIGHT) || (Reason EQ INFO_KCD_HUP) || (Reason EQ INFO_TIMEOUT))
			{
				if (win_data != NULL)
				{
					save_data = (T_SAVE *)win_data->user;
					if (save_data != NULL)
					{
						l_parent = save_data->parent;
					}
				}
				// Destroy the window
				SmsSend_SAVE_destroy(win);
				// This will make SEND frame to recreate the text editor
				if (l_parent != NULL)
					SEND_EVENT(l_parent, E_EXIT, SAVE_ID, NULL);
				
			}
			break;


		default:
			TRACE_EVENT("MFW_EVENT_REJECTED");
			return MFW_EVENT_REJECTED;
	}
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	SmsSend_convert_to_inter

 $Description:	Transform a phone number in T_MFW_SMS_ADDR format to a string 
 				with a '+' character if international number
 				
 $Returns:		none
 
 $Arguments:	source_number - calling number
 				dest_number - called number
*******************************************************************************/

void SmsSend_convert_to_inter(T_MFW_SMS_ADDR *source_number,	// The number to convert
							  char *dest_number)				// The dest number with a '+' if needed
{
	USHORT nb_char;
	USHORT i = 0;
	char *pt_digit = dest_number;

	TRACE_FUNCTION ("SmsSend_convert_to_inter()");

	nb_char = strlen(source_number->number);

	if (source_number->ton == MFW_TON_INTERNATIONAL)
	{
		*pt_digit = '+';
		pt_digit++;
	}

	for (i = 0; i <= nb_char; i++)
		pt_digit[i] = source_number->number[i];
}

/*******************************************************************************

 $Function:    	SmsSend_R_OPTExeCentreEdit

 $Description:	Create editor for entering the Service center number.
 
 $Returns:		none
 
 $Arguments:	m - menu handler
 				i - menu item selected.
*******************************************************************************/


int SmsSend_R_OPTExeCentreEdit(MfwMnu* m, MfwMnuItem* i) 
{
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	T_MFW_HND win = SmsSend_SEND_create(parent_win);
	SEND_EVENT(win,E_INIT,SMSSEND_ID_NBEDIT,0);
	return 1;   // ADDED BY RAVI-28-11-2005	
}
/*******************************************************************************

 $Function:    	SmsSend_SCNBEDIT_edit_cb

 $Description:	Callback function for the SC number editor.

 $Returns:		none

 $Arguments:	win - current window
 				Identifier - unique id
 				reason - event cause
*******************************************************************************/

static void SmsSend_SCNBEDIT_edit_cb (T_MFW_HND win, USHORT Identifier, SHORT reason)
{
	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	T_SEND      * data     = (T_SEND *)win_data->user;
	
	T_MFW_SMS_INFO	sms_parameter;


	TRACE_FUNCTION ("SmsSend_SCNBEDIT_edit_cb()");

	
	switch (reason)
	{
		case INFO_KCD_LEFT:
				TRACE_EVENT("INFO_KCD_LEFT");
				//GW Read config data to get value of dcs
				SmsSend_get_config_data(&sms_parameter);
				if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
				{	//We have a chinese text message - set dcs parameter to 
					sms_parameter.dcs = MFW_DCS_UCS2;
    			}
				else
				{
					sms_parameter.dcs = MFW_DCS_7bits;
				}
				//copy the sevice center number into the 
				
				strcpy((char*)sms_parameter.sc_addr,(char*)data->edt_data.CentreBuffer);
				// store the Service center number into the SIM.
				SmsSend_set_SrvCntr_number (&sms_parameter);
				SmsSend_set_dcs_number (&sms_parameter);
				
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				AUI_edit_Destroy(data->number_editor);
#else /* NEW_EDITOR */
				editor_destroy(data->number_editor);
#endif /* NEW_EDITOR */

				SmsSend_SEND_destroy(win);
				data->number_editor = 0;
		
		break;
          
		case INFO_KCD_ALTERNATELEFT:
			TRACE_EVENT("INFO_KCD_ALTERNATELEFT");

			bookPhonebookStart(win, PhbkFromSmsSC);	/* SH - changed from PhbFromSms */
			break;
		case INFO_KCD_RIGHT:
		case INFO_KCD_HUP:
			TRACE_EVENT("INFO_KCD_RIGHT/HUP");
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			AUI_edit_Destroy(data->number_editor);
#else /* NEW_EDITOR */
			editor_destroy(data->number_editor);
#endif /* NEW_EDITOR */

			SmsSend_SEND_destroy(win);
			data->number_editor = 0;
			
			break;

		default:
			TRACE_EVENT("Err: Default");
			break;
	}
}

// Feb 02, 2005   REF: CRR 28479   xnkulkar
// Bug: While saving a SMS, if the user presses LSK while the 'Saved' dialog is displayed, the 'Saved' 
// dialog doesn't get dismissed.
// Solution: New function "mmi_dialog_information_screen_save_sms" which has only 
// KEY_RIGHT and  KEY_HUP registered instead of KEY_ALL
/*******************************************************************************

 $Function:    	mmi_dialog_information_screen_save_sms

 $Description:	 Function to display SMS "Saved" info screen

 $Returns:	info_dialog	

 $Arguments:	

*******************************************************************************/
T_MFW_HND mmi_dialog_information_screen_save_sms(T_MFW_HND parent_win, int TxtId, char* text, T_VOID_FUNC call_back, USHORT identifier)
{
	T_DISPLAY_DATA display_info;
	dlg_initDisplayData_TextId( &display_info, TxtNull,  TxtNull, TxtId, TxtNull, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, call_back, THREE_SECS, KEY_RIGHT | KEY_HUP );
	display_info.TextString2   = text; 
	display_info.Identifier   = identifier;
	return info_dialog(parent_win,&display_info); //information screen
		
}

#undef MMI_SMSSEND_C
