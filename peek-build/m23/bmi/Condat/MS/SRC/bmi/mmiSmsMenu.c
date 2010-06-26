/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI
 $Module:   SMS
 $File:       MmiSmsMenu.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       25/10/00

********************************************************************************

 Description:

    Implementation SMS dynamic menu handling

********************************************************************************

 $History: MmiSmsMenu.c

      Feb 28, 2007 OMAPS00109953 a0393213(R.Prabakar)
      Description : Need to be able to store SMS in SIM/ME
      Solution     : Earlier, index alone was sufficient to identify messages. Now memory should also be taken care of.

	May 27, 2005    MMI-FIX-29869 x0018858
   	Description: There is a limitation on the number of messages that can be downloaded from 
   	server even though there is space on the sim.
   	Solution: The limitation has been removed and the number of messages that can be dowloaded
   	has been modified to the masimum number that can be stored.

      	March 31, 2005    REF: CRR MMI-SPR-24511   x0012852
	Bug:The phone crashes when the user selects the 'Delete All' option 
       almost the same time as there's an incoming message.
       Fix:Check added for part message before deleting all messages.

 	Feb 24, 2005 REF: CRR MMI-SPR-29102 xnkulkar
       Description: The phone resets when user presses the ''End All' key while the SMSs are being deleted. 
       Solution: As the user has already confirmed that all SMS messages are to be deleted, ''End All' key 
       		press is ignored till the delete operation is over.
       		
	Jul 13, 2004    REF: CRR 21615  Deepa M.D 
	Bug:Message once entered into message editor not storing in memory.
	Fix:When a SMS is typed in the Editor, it is stored in a temporary buffer.
	When the user accesses the editor,the temporary buffer is copied back into 
	the Editor .

	Mar 29, 2004    REF: CRR 12722  Deepa M.D 
	Mar 24, 2004    REF: CRR 15746  Deepa M.D 
	25/10/00      Original Condat(UK) BMI version.

 $End


*******************************************************************************/

#define MMI_SMSMENU_C

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
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_tim.h"
#include "mfw_phb.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_sms.h"
#include "mfw_smsi.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_lng.h"
#include "mfw_sat.h"
#include "mfw_kbd.h"
#include "mfw_nm.h"


#include "dspl.h"

#include "MmiMain.h"
#include "MmiDummy.h"
#include "MmiMmi.h"

#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif
#include "MmiBookShared.h"
#include "mmiSmsMenu.h"
#include "mmiSmsRead.h"
#include "mmismssend.h"
#include "mmiSmsIdle.h"
#include "mmiSmsBroadcast.h"	/* reqd for info_screen function prototype */

#include "cus_aci.h"

#include "mfw_ffs.h"
//#include "pcm.h"

#include "mmiColours.h"

//May 27, 2005    MMI-FIX-29869 x0018858
#ifdef FF_MMI_SMS_DYNAMIC
int g_max_messages = 0;
#endif


/* =================================================== */
/* BEGIN ADD: Sumit: Locosto: eZiText */
T_MFW_HND eZiTextWin;
/* END ADD: Sumit: Locosto: eZiText */
/* =================================================== */

extern T_MFW_HND SmsRead_R_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr);

// Start functions of the windows
T_MFW_HND M_SND_SAV_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr);
T_MFW_HND M_DEL_ALL_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr);
T_MFW_HND M_DEL_ALL_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr);
T_MFW_HND V_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr);

// Standard callback function for temporary dialog window
int SmsMenu_StandardDialog_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason);
static void SmsMenu_StandardEditor_cb (T_MFW_HND win, USHORT Identifier, SHORT reason);

/* x0039928 - Lint warning fix */
int SmsMenu_sms_delete_all(T_MFW_HND win);

/* This structure is used by 'Delete All' function that is common for SMS and SMS-CB */
typedef enum SmsType
{
  SMS_TYPE, /* interfered with macro -> renamed SMS to SMS_TYPE */
  SMSCB_TYPE
} MySmsType;

/* ID of the different windows (used in SEND_EVENT function to provide the callee the ID of the caller) */
typedef enum
{
  /* Logical frames */
  M_DEL_ALL_ID,
  M_SND_SAV_ID,
  V_ID_SET,
  V_ID_CALL,
  V_NUM_ID,
  /* Dialog windows */
  SMSMENU_ID_NOTREADY,
  SMSMENU_ID_DELETEALL,
  SMSMENU_ID_DELOK,
  SMSMENU_ID_DELERR,
  SMSMENU_ID_NOMESSAGES,
  SMSMENU_ID_WAIT
} sms_menu_id;

typedef enum              /* numbering plan identifier */
{
  MFW_MB_LINE1      = 1,
  MFW_MB_LINE2      = 2,
  MFW_MB_FAX        = 3,
  MFW_MB_DATA       = 4
} T_MFW_MAILBOX_ID;



typedef enum
{
  SMS_DELETING,
  SMS_DISPLAY,
  SMS_READY
} T_SmsMenu_state;

typedef struct
{
    /* administrative data */

    T_MMI_CONTROL   mmi_control;
    T_MFW_HND       win;
    SHORT           id;
    T_MFW_HND       parent;

    /* associated handlers */

    T_MFW_HND       kbd;
    T_MFW_HND       kbd_long;
    T_MFW_HND       menu;

    /* internal data */

  T_SmsUserData * user_data; // text and phone number of the message
} T_M_SND_SAV;

typedef struct
{
    /* administrative data */

    T_MMI_CONTROL   mmi_control;
    T_MFW_HND       win;
    SHORT           id;
    T_MFW_HND       parent;
  T_MFW_HND       waiting_win;

    /* associated handlers */
  T_MFW_HND       sms_hnd;         /* MFW sms handler      */

    /* internal data */

  MySmsType  type; // The type of the SMS to delete (SMS or SMS-CB)
  //May 27, 2005    MMI-FIX-29869 x0018858
#ifdef FF_MMI_SMS_DYNAMIC
  T_MFW_SMS_MSG *g_SmsMenu_messages;
#else
  T_MFW_SMS_MSG g_SmsMenu_messages[MAX_MESSAGES];
#endif
  T_MFW_HND   child_dialog;
} T_M_DEL_ALL;

typedef struct
{
    /* administrative data */

    T_MMI_CONTROL   mmi_control;
    T_MFW_HND       win;
    SHORT           id;
    T_MFW_HND       parent;

    /* associated handlers */

    T_MFW_HND       kbd;
    T_MFW_HND       kbd_long;
    T_MFW_HND       menu;


} T_V;



typedef struct
{
    /* administrative data */

    T_MMI_CONTROL   mmi_control;
    T_MFW_HND       win;
    SHORT           id;
    T_MFW_HND       parent;

    /* associated handlers */


    /* internal data */
    UBYTE voice_number_buffer[NUMBER_LENGTH];

} T_V_NUM;

static T_MFW_HND M_SND_SAV_create (T_MFW_HND parent);
static void M_SND_SAV_destroy (T_MFW_HND window);
static void M_SND_SAV_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int M_SND_SAV_win_cb (MfwEvt e, MfwWin *w);
static int M_SND_SAV_kbd_cb (MfwEvt e, MfwKbd *k);
static int M_SND_SAV_mnu_cb (MfwEvt e, MfwMnu *m);

static int ExeSms_sendM_SND_SAV(MfwMnu *, MfwMnuItem *);

static T_MFW_HND M_DEL_ALL_create (T_MFW_HND parent);
static void M_DEL_ALL_destroy (T_MFW_HND window);
static void M_DEL_ALL_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int M_DEL_ALL_mfw_cb (MfwEvt event, void *parameter);
static int M_DEL_ALL_win_cb (MfwEvt e, MfwWin *w);

static T_MFW_HND V_create (T_MFW_HND parent);
static void V_destroy (T_MFW_HND window);
static void V_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int V_win_cb (MfwEvt e, MfwWin *w);
static int V_kbd_cb (MfwEvt e, MfwKbd *k);
static int V_mnu_cb (MfwEvt e, MfwMnu *m);


/* SPR#1428 - SH - New Editor: not required */
#ifndef NEW_EDITOR
void SmsMenu_loadEditDefault(T_EDITOR_DATA *editor_data);
#endif

// State of the SMS menu management
T_SmsMenu_state g_SmsMenu_state = SMS_DISPLAY;
MfwHnd g_ChildWindow2;
UBYTE g_CenterNumber[NUMBER_LENGTH];
//Jul 13, 2004    REF: CRR 21615  Deepa M.D 
UBYTE TempTextBuffer[MAX_MSG_LEN_ARRAY]; //Temporary buffer to store the SMS content

/*******************************************************************************

 $Function:     M_exeSendNew

 $Description:  This function is called when the user press Send in the
        main SMS menu.

 $Returns:    Execution status

 $Arguments:  m - menu handler
        i - Menu item selected

*******************************************************************************/

int M_exeSendNew(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       win  = eZiTextWin;// SUMIT: mfwParent(mfw_header());
    T_SmsUserData   SmsData;
	
    /* Copy the number and name information to the SMS buffer
    */ 
	/*MC SPR 1257, clear whole buffer in case of Unicode entry*/
	
    /*SPR 2587, check that SMS has been initialised*/
	if (!smsidle_get_ready_state() || phb_get_mode() == PHB_LOADING)
	{	
		
		mmi_dialog_information_screen(0, TxtPleaseWait, NULL, NULL, NULL);
		return 0;
	}	
	//	Jul 13, 2004    REF: CRR 21615  Deepa M.D 
	//  Copy the content from the temporary buffer into the editor,so that the 
	//  previously typed content is retained. 
#ifdef TI_PS_FF_CONC_SMS		
    if(FFS_flashData.ConcatenateStatus == TRUE)
   	{
		memset(SmsData.TextBuffer, '\0', MAX_MSG_LEN);
		memcpy(SmsData.TextBuffer, TempTextBuffer,MAX_MSG_LEN);
   	}
	
    else
#endif /*TI_PS_FF_CONC_SMS*/		
	{
		memset(SmsData.TextBuffer, '\0', MAX_MSG_LEN_SGL);
		//Copy only 160 characters ofthe message.
		memcpy(SmsData.TextBuffer, TempTextBuffer,MAX_MSG_LEN_SGL-1); 
	}
    
    SmsData.NumberBuffer[0]='\0'; //set the number to null.
    SmsData.called_from_reply_item = FALSE;
    
	SmsSend_SEND_start(win, (void *) &SmsData);
	return 1;
}

/*******************************************************************************

 $Function:     M_exeRead

 $Description:  This function is called when the user press Read in the
          main SMS menu

 $Returns:    Execution status

 $Arguments:  m - menu handler
        i - Menu item selected

*******************************************************************************/

int M_exeRead(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       win  = mfwParent(mfw_header());
  T_MFW_SMS_STAT  sms_list_type = MFW_SMS_ALL;

  /* Call the function that manages SMS reading */
  SmsRead_R_start(win, (MfwMnuAttr*)&sms_list_type);

  return 1;
}

/*******************************************************************************

 $Function:     M_exeDelAll

 $Description:  This function is called when the user press Delete all in the
          main SMS menu

 $Returns:    Execution status

 $Arguments:  m - menu handler
        i - Menu item selected

*******************************************************************************/

int M_exeDelAll(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       win  = mfwParent(mfw_header());
  MySmsType sms_type;

  sms_type = SMS_TYPE;
  // Here we have to pass the type of sms to delete (SMS or SMS-CB)
  M_DEL_ALL_start(win, (void *)&sms_type);

  return 1;
}

/*******************************************************************************

 $Function:     M_exeSendSaved

 $Description:  This function is called when the user press Saved messages
          in the main SMS menu

 $Returns:    Execution status

 $Arguments:  m - menu handler
        i - Menu item selected

*******************************************************************************/

int M_exeSendSaved(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       win  = mfwParent(mfw_header());

  M_SND_SAV_start(win, 0);

  return 1;
}



/*******************************************************************************

 $Function:     voice_mail_init

 $Description:  initialization the voice mail buffer


 $Returns:

 $Arguments:


*******************************************************************************/

int voice_mail_init (void)
{

  //nm, later have to read the voice mail number from FFS after switching-on the mobile

  //to clear the buffer is only temporary until we are able to save on Flash
  //memset(voice_mail, '\0', sizeof(voice_mail));



  return 1;
}




/*******************************************************************************

 $Function:     M_exeVoice

 $Description:  This function is called when the user press Voice Mail
          in the main SMS menu

 $Returns:    Execution status

 $Arguments:  m - menu handler
        i - Menu item selected

*******************************************************************************/

int M_exeVoice(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND win;

    T_MFW_HND     parent_window  = mfwParent(mfw_header());

    TRACE_FUNCTION ("M_exeVoice()");

    win = V_create (parent_window);

    if (win NEQ NULL)
  {
      SEND_EVENT (win, E_INIT, V_ID_SET, 0);
  }
  return 1;
}


/*******************************************************************************

 $Function:     M_callVoice

 $Description:  This function is called when the user press set Voice Mail
          in the main SMS menu

 $Returns:    Execution status

 $Arguments:  m - menu handler
        i - Menu item selected

*******************************************************************************/

int M_callVoice(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       win;
    T_MFW_HND       parent_win  = mfwParent(mfw_header());


    TRACE_FUNCTION ("M_callVoice()");

    win = V_create (parent_win);

    if (win NEQ NULL)
  {
      SEND_EVENT (win, E_INIT, V_ID_CALL, 0);
  }

  return 1;
}


/*SPR 1920 removed obsolete function*/

#if(0) /* x0039928 - Lint warning removal */
/*******************************************************************************

 $Function:     ExeSms_sendM_SND_SAV

 $Description:  Called when the user press Saved SMS from the Send SMS menu

 $Returns:    Execution status

 $Arguments:  m - menu handler
        i - Menu item selected

*******************************************************************************/

static int ExeSms_sendM_SND_SAV(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       win  = mfwParent(mfw_header());

    return 1;
}
#endif

/*******************************************************************************

 $Function:     M_SND_SAV_start

 $Description:  Start the creation of the saved SMS menu window

 $Returns:    Execution status

 $Arguments:  parent_window - parent window
        menuAttr - Menu attributes.

*******************************************************************************/

T_MFW_HND M_SND_SAV_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr)
{
    T_MFW_HND win;

    TRACE_FUNCTION ("M_SND_SAV_start()");

    win = M_SND_SAV_create (parent_window);

    if (win NEQ NULL)
  {
      SEND_EVENT (win, E_INIT, M_SND_SAV_ID, (void *)menuAttr);
  }
    return win;
}

/*******************************************************************************

 $Function:     M_SND_SAV_create

 $Description:  Create the saved SMS menu window

 $Returns:    Execution status

 $Arguments:  parent_window - parent window

*******************************************************************************/

static T_MFW_HND M_SND_SAV_create(MfwHnd parent_window)
{
    T_M_SND_SAV      * data = (T_M_SND_SAV *)ALLOC_MEMORY (sizeof (T_M_SND_SAV));
    T_MFW_WIN  * win;

    TRACE_FUNCTION ("M_SND_SAV_create()");


    /*
     * Create window handler
     */

    data->win =
  win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)M_SND_SAV_win_cb);
    
    if (data->win EQ NULL)
  {
      return NULL;
  }
    /*
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)M_SND_SAV_exec_cb;
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

 $Function:     M_SND_SAV_destroy

 $Description:  Destroy the saved SMS menu window

 $Returns:    none

 $Arguments:  own_window - current window

*******************************************************************************/

static void M_SND_SAV_destroy(MfwHnd own_window)
{
    T_MFW_WIN * win_data;
    T_M_SND_SAV     * data;

    TRACE_FUNCTION ("M_SND_SAV_destroy()");

    if (own_window)
  {
      win_data = ((T_MFW_HDR *)own_window)->data;
      data = (T_M_SND_SAV *)win_data->user;

      if (data)
    {
        /*
         * Delete WIN handler
         */
        win_delete (data->win);

        /*
         * Free Memory
         */
        FREE_MEMORY ((void *)data->user_data, sizeof (T_SmsUserData));
      FREE_MEMORY ((void *)data, sizeof (T_M_SND_SAV));
    }
      else
    {
        TRACE_EVENT ("M_SND_SAV_destroy() called twice");
    }
  }
}

/*******************************************************************************

 $Function:     M_SND_SAV_exec_cb

 $Description:  Exec callback for the saved SMS menu window

 $Returns:    none

 $Arguments:  win - current window
        event - window event id
        value - Unique Id
        parameter - optional data.

*******************************************************************************/

void M_SND_SAV_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
    /* callback handler for events sent to to trigger execution */
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_M_SND_SAV          * data = (T_M_SND_SAV *)win_data->user;
    T_MFW_HND      * l_parent;
    SHORT            l_id;

    TRACE_FUNCTION ("M_SND_SAV_exec_cb()");

    switch (event)
  {
  case E_INIT:
      /* initialization of administrative data */
      data->id = value;
    data->user_data = (T_SmsUserData *)ALLOC_MEMORY (sizeof (T_SmsUserData));
    /*MC SPR 1257, clear whole buffer in case of Unicode entry*/
#ifdef TI_PS_FF_CONC_SMS	
 	if(FFS_flashData.ConcatenateStatus == TRUE)
 		memset( data->user_data->TextBuffer, '\0', MAX_MSG_LEN_SGL);
    else
#endif /*TI_PS_FF_CONC_SMS*/		
    	memset( data->user_data->TextBuffer, '\0', MAX_MSG_LEN);
    data->user_data->NumberBuffer[0] = '\0';

      /* initialization of the dialog data */

      /* create the dialog handler */
      data->kbd =      kbdCreate(data->win,KEY_ALL,         (MfwCb)M_SND_SAV_kbd_cb);
      data->kbd_long = kbdCreate(data->win,KEY_ALL|KEY_LONG,(MfwCb)M_SND_SAV_kbd_cb);

      /* put the (new) dialog window on top of the window stack */
      mnuUnhide(data->menu);
        winShow(win);
      break;
  case E_ABORT:
  case E_EXIT:
      l_parent = data->parent;
      l_id = data->id;
      M_SND_SAV_destroy(data->win);
      SEND_EVENT (l_parent, event, l_id, NULL); /* forward event to parent */
      break;

  case E_RETURN:
  default:
      return;
  }
}

#if(0) /* x0039928 - Lint warning fix */
/*******************************************************************************

 $Function:     M_SND_SAV_mnu_cb

 $Description:  Menu callback for the saved SMS menu window

 $Returns:    Execution status

 $Arguments:  e - event id
        m - menu handle

*******************************************************************************/

static int M_SND_SAV_mnu_cb (MfwEvt e, MfwMnu *m)
    /* M_SND_SAV menu event handler */
{
    T_MFW_HND    win  = mfwParent(mfw_header());
    T_MFW_WIN  * win_data = ((T_MFW_HDR *)win)->data;
    T_M_SND_SAV      * data = (T_M_SND_SAV *)win_data->user;

    TRACE_FUNCTION ("M_SND_SAV_mnu_cb()");

    switch (e)
        {
  case E_MNU_ESCAPE: /* back to previous menu */
      M_SND_SAV_destroy(data->win);
      SEND_EVENT (data->parent, E_RETURN, data->id, NULL);
      break;
  default: /* in mnuCreate() only E_MNU_ESCAPE has been enabled! */
      return MFW_EVENT_REJECTED;
        }
    return MFW_EVENT_CONSUMED;
}
#endif

/*******************************************************************************

 $Function:     M_SND_SAV_win_cb

 $Description:  Window callback for the saved SMS menu window

 $Returns:    Execution status

 $Arguments:  e - event id
        m - window handle

*******************************************************************************/

static int M_SND_SAV_win_cb (MfwEvt e, MfwWin *w)
    /* M_SND_SAV window event handler */
{
    TRACE_FUNCTION ("M_SND_SAV_win_cb()");

    switch (e)
  {
        case MfwWinVisible:  /* window is visible  */
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

 $Function:     M_SND_SAV_kbd_cb

 $Description:  Keyboard callback for the saved SMS menu window

 $Returns:    Execution status

 $Arguments:  e - event id
        k - keyboard info

*******************************************************************************/

static int M_SND_SAV_kbd_cb (MfwEvt e, MfwKbd *k)
    /* M_SND_SAV keyboard event handler */
{
    T_MFW_HND    win  = mfwParent(mfw_header());
    T_MFW_WIN  * win_data = ((T_MFW_HDR *)win)->data;
    T_M_SND_SAV      * data = (T_M_SND_SAV *)win_data->user;

    TRACE_FUNCTION ("M_SND_SAV_kbd_cb()");

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
        winShow(win);
        break;
    case KCD_MNUDOWN: /* highlight next entry */
        mnuDown(data->menu);
        winShow(win);
        break;
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

 $Function:     M_DEL_ALL_start

 $Description:  Start the delete all window dialog.

 $Returns:    window handler

 $Arguments:  parent_window - parent window
        menuAttr - Menu attributes.

*******************************************************************************/

T_MFW_HND M_DEL_ALL_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr)
{
    T_MFW_HND win;

    TRACE_FUNCTION ("M_DEL_ALL_start()");

    win = M_DEL_ALL_create (parent_window);

    if (win NEQ NULL)
  {
      SEND_EVENT (win, E_INIT, M_DEL_ALL_ID, (void *)menuAttr);
  }
    return win;
}

/*******************************************************************************

 $Function:     M_DEL_ALL_create

 $Description:  Create the delete all window

 $Returns:    window handler

 $Arguments:  parent_window - parent window

*******************************************************************************/

static T_MFW_HND M_DEL_ALL_create(MfwHnd parent_window)
{
    T_M_DEL_ALL      * data = (T_M_DEL_ALL *)ALLOC_MEMORY (sizeof (T_M_DEL_ALL));
    T_MFW_WIN  * win;

    TRACE_FUNCTION ("M_DEL_ALL_create()");

    /*
     * Create window handler
     */

    data->win =
  win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)M_DEL_ALL_win_cb);

    if (data->win EQ NULL)
  {
      return NULL;
  }
    /*
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)M_DEL_ALL_exec_cb;
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

 $Function:     M_DEL_ALL_destroy

 $Description:  Destroy the delete all window

 $Returns:    none

 $Arguments:  own_window - current window

*******************************************************************************/

static void M_DEL_ALL_destroy(MfwHnd own_window)
{
    T_MFW_WIN * win_data;
    T_M_DEL_ALL     * data;

    TRACE_FUNCTION ("M_DEL_ALL_destroy()");
	
    if (own_window)
  {
      win_data = ((T_MFW_HDR *)own_window)->data;
      data = (T_M_DEL_ALL *)win_data->user;

      if (data)
    {
        /*
         * Delete WIN handler
         */

      if  (data->waiting_win)
      {
               SEND_EVENT(data->waiting_win,DIALOG_DESTROY,0,0 );
        data->waiting_win = 0;
      }

        win_delete (data->win);

        /*
         * Free Memory
         */
        FREE_MEMORY ((void *)data, sizeof (T_M_DEL_ALL));
    }
      else
    {
        TRACE_EVENT ("M_DEL_ALL_destroy() called twice");
    }
  }
}

/*******************************************************************************

 $Function:     M_DEL_ALL_exec_cb

 $Description:  Exec callback function for the Delete All window

 $Returns:    none

 $Arguments:  win - current window
        event - event id
        value - Unique id
        parameter - optional data.

*******************************************************************************/

void M_DEL_ALL_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
    /* callback handler for events sent to to trigger execution */
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_M_DEL_ALL    * data = (T_M_DEL_ALL *)win_data->user;
  T_MFW_EVENT      MfwEvents;

    T_DISPLAY_DATA  DisplayInfo;

    TRACE_FUNCTION ("M_DEL_ALL_exec_cb()");
	
    switch (event)
  {
  case E_INIT:
      /* initialization of administrative data */
      data->id = value;

      /* initialization of the dialog data */
    data->type = (MySmsType)(int)parameter;  /* x0039928 - Lint warning fix */

	/*SPR2139, initialise sms handle to NULL*/
   data->sms_hnd = NULL;

    /* create the dialog handler */
    // Display the dialog window to delete all the SMS
    SmsMenu_loadDialogDefault(&DisplayInfo);

    //NM 29.8 Check before deleting, is SIM ready ?!  */
    if (!smsidle_get_ready_state())
    {
      DisplayInfo.TextId     = TxtWaiting;
      DisplayInfo.Identifier = SMSMENU_ID_NOTREADY;
      }
    else
    {	/*SPR 2139, if there are no SMSs on the SIM*/
		if ( sms_message_count(MFW_SMS_ALL) == 0)
		{	/*tell user*/
			DisplayInfo.TextId     = TxtEmptyList;
      		DisplayInfo.Identifier = SMSMENU_ID_NOMESSAGES;
		}
		else
		{ 	/*SPR 2139, only create an SMS event handler if sms initialised and sms list is not empty*/
			MfwEvents = E_SMS_MO_AVAIL | E_SMS_ERR | E_SMS_OK;
    		data->sms_hnd = sms_create(data->win,MfwEvents,(MfwCb)M_DEL_ALL_mfw_cb);
    		
     	 	DisplayInfo.TextId       = TxtOkToDelete;
      		DisplayInfo.Time         = FIVE_SECS;
      		DisplayInfo.KeyEvents    = KEY_RIGHT|KEY_LEFT|KEY_CLEAR;
     		 DisplayInfo.Identifier   = SMSMENU_ID_DELETEALL;
      		DisplayInfo.LeftSoftKey  = TxtSoftOK;
      		DisplayInfo.RightSoftKey = TxtSoftBack;
		}
    }
    // Generic function for info messages

        (void) info_dialog(win, &DisplayInfo);

    winShow(win);

    break;

  case E_ABORT:
  case E_EXIT:
      M_DEL_ALL_destroy (data->win);
      break;

  case E_RETURN:
  default:
      return;
  }
}

/*******************************************************************************

 $Function:     M_DEL_ALL_win_cb

 $Description:  Window callback function for the delete all window

 $Returns:    none

 $Arguments:  e - current window
        w - event id

*******************************************************************************/

static int M_DEL_ALL_win_cb (MfwEvt e, MfwWin *w)
    /* V window event handler */
{
    TRACE_FUNCTION ("M_DEL_ALL_win_cb()");

    switch (e)
  {
        case MfwWinVisible:  /* window is visible  */
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

 $Function:     M_DEL_ALL_mfw_cb

 $Description:  Exec callback function for the DEL_ALL window


 $Returns:    none

 $Arguments:  event - event id
        parameter - optional data.

*******************************************************************************/

int M_DEL_ALL_mfw_cb (MfwEvt event, void *parameter)
{
    T_MFW_HND         win       = mfwParent((MfwHdr *)mfw_header());
    T_MFW_WIN   * win_data = ((T_MFW_HDR *)win)->data;
    T_M_DEL_ALL     * data = (T_M_DEL_ALL *)win_data->user;

  // T_MFW_EVENT       MfwEvents;  // RAVI


  T_DISPLAY_DATA DisplayInfo;

    TRACE_FUNCTION ("M_DEL_ALL_ACK_mfw_cb()");

    switch (event)
  {

  case E_SMS_ERR:

    g_SmsMenu_state = SMS_READY;

    SmsMenu_loadDialogDefault(&DisplayInfo);
    DisplayInfo.Identifier = SMSMENU_ID_DELERR;
    DisplayInfo.TextId     = TxtFailed;
    info_dialog(win,&DisplayInfo);
    break;

  case E_SMS_OK:

   TRACE_EVENT("Event: E_SMS_OK");

    //        March 31, 2005    REF: CRR MMI-SPR-24511   x0012852
    //  	 Check added for part message before deleting all messages.
    if((g_ListLength2 > 0) && (!((data->g_SmsMenu_messages[g_ListLength2-1].concat_status == MFW_SMS_CONC_PART)
     && (data->g_SmsMenu_messages[g_ListLength2-1].index == data->g_SmsMenu_messages[g_ListLength2-1].first_index)
     && (data->g_SmsMenu_messages[g_ListLength2-1].mem== data->g_SmsMenu_messages[g_ListLength2-1].first_mem) /*a0393213 OMAPS00109953*/
	)))
    {
   	  g_ListLength2--;
	  /*a0393213 OMAPS00109953 - memory passed as parameter*/
	  sms_msg_delete(data->g_SmsMenu_messages[g_ListLength2].index,data->g_SmsMenu_messages[g_ListLength2].mem);
    }
    else
    {
	
      /* there is now anymore sms to delete !! */
      g_SmsMenu_state = SMS_READY;

	/* cq16875 Destroy the previous info dialog if there is one before displaying the new dialog.
	    When a concatenated message is deleted using the "Delete All" menu option the E_SMS_OK 
	    event is received for each Concat message part that is deleted.
	    12-02-04 MZ. */
	if(data->child_dialog != NULL)
	{	
		SEND_EVENT (data->child_dialog, DIALOG_DESTROY, 0,NULL);
		data->child_dialog = NULL;
	}

      SmsMenu_loadDialogDefault(&DisplayInfo);
      DisplayInfo.Identifier = SMSMENU_ID_DELOK;
      DisplayInfo.TextId     = TxtDeleted;
	 /*SPR2139, removed sms_delete() from here, this is now donw in dialog callback*/
	 /*cq16875  Store the info dialog handler 12-02-04 MZ. */
       data->child_dialog = info_dialog(win,&DisplayInfo);
    }

    break;

  default:
      return MFW_EVENT_REJECTED;

  }
    return MFW_EVENT_CONSUMED;

}

/*******************************************************************************

 $Function:     V_start

 $Description:  Start the voice mailbox dialog window.


 $Returns:    window handle

 $Arguments:  parent_window - parent window handle
        menuAttr - Menu attributes

*******************************************************************************/

T_MFW_HND V_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr)
{
    T_MFW_HND win;

    TRACE_FUNCTION ("V_start()");

    win = V_create (parent_window);

    if (win NEQ NULL)
  {
      SEND_EVENT (win, E_INIT, V_ID_SET, (void *)menuAttr);
  }
    return win;
}

/*******************************************************************************

 $Function:     V_create

 $Description:  create the voice mailbox window

 $Returns:    window handle

 $Arguments:  parent_window - parent window handle

*******************************************************************************/

static T_MFW_HND V_create(MfwHnd parent_window)
{
    T_V      * data = (T_V *)ALLOC_MEMORY (sizeof (T_V));
    T_MFW_WIN  * win;

    TRACE_FUNCTION ("V_create()");

    /*
     * Create window handler
     */

    data->win =
  win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)V_win_cb);
    if (data->win EQ NULL)
  {
      return NULL;
  }
    /*
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)V_exec_cb;
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

 $Function:     V_destroy

 $Description:  Destroy the voice mailbox window

 $Returns:    none

 $Arguments:  own_window - current window

*******************************************************************************/

static void V_destroy(MfwHnd own_window)
{
    T_MFW_WIN * win_data;
    T_V     * data;

    TRACE_FUNCTION ("V_destroy()");

    if (own_window)
  {
      win_data = ((T_MFW_HDR *)own_window)->data;
      data = (T_V *)win_data->user;

      if (data)
    {
        /*
         * Delete WIN handler
         */
        win_delete (data->win);

        /*
         * Free Memory
         */
        FREE_MEMORY ((void *)data, sizeof (T_V));
    }
      else
    {
        TRACE_FUNCTION ("V_destroy() called twice");
    }
  }
}

/*******************************************************************************

 $Function:     V_exec_cb

 $Description:  Exec callback function for the voice mailbox window

 $Returns:    none

 $Arguments:  win - current window
        event - event id
        value - Unique id
        parameter - optional data.

*******************************************************************************/

void V_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
    /* callback handler for events sent to to trigger execution */
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_V          * data = (T_V *)win_data->user;

 // char mail_alpha[10+1] = ""; // size hard coded in pcm.h   // RAVI

#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
    T_EDITOR_DATA editor_data;
#endif

    TRACE_FUNCTION ("V_exec_cb()");

  switch (event)
  {
    // On exit from the editor, we try again to call the number
    case E_INIT:

      if ( (FFS_flashData.voice_mail[0] EQ '\0') || (value EQ V_ID_SET) )
      {
      /* initialization of administrative data */
      data->id = value;

	/* SPR#1428 - SH - New Editor changes */
	
#ifdef NEW_EDITOR
		AUI_edit_SetDefault(&editor_data);
		AUI_edit_SetDisplay(&editor_data, ZONE_FULL_SK_TITLE, COLOUR_EDITOR_XX, EDITOR_FONT);
		AUI_edit_SetEvents(&editor_data, value, TRUE, FOREVER, (T_AUI_EDIT_CB)SmsMenu_StandardEditor_cb);
		AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtDelete, TxtVoiceMail, NULL); /* SPR#2141 - DS - Changed RSK to "Delete". */
		// Change by Sasken ( Deepa M.d) on March 29th 2004
		// Issue Number : MMI-SPR-12722
		// Subject: Right softkey should be "Back" when the editor is empty
		// Bug : Right softkey should be "Back" when the editor is empty
		// Solution : Added alternative string for the right softkey tobe displayed when the editor is empty
		
		AUI_edit_SetAltTextStr(&editor_data, 0, NULL, TRUE, TxtSoftBack);
		AUI_edit_SetMode(&editor_data, 0, ED_CURSOR_UNDERLINE);
		AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)FFS_flashData.voice_mail, PHB_MAX_LEN);

		 /* SPR#2141 - DS - Set RSK to "Back" when the editor is empty. */
             AUI_edit_SetAltTextStr(&editor_data, 1, NULL, TRUE, TxtSoftBack);
             
        	AUI_edit_Start(win, &editor_data);
#else /* NEW_EDITOR */
        SmsMenu_loadEditDefault(&editor_data);

        editor_data.editor_attr.text    = (char *)FFS_flashData.voice_mail;
        editor_data.editor_attr.size    = PHB_MAX_LEN;
        editor_data.TextId          = TxtVoiceMail;
        editor_data.LeftSoftKey       = TxtSoftOK;
		// Change by Sasken ( Deepa M.d) on March 29th 2004
		// Issue Number : MMI-SPR-12722
		// Subject: Right softkey should be "Back" when the editor is empty
		// Bug : Right softkey should be "Back" when the editor is empty
		// Solution : Added alternative string for the right softkey tobe displayed when the editor is empty
		
		editor_data.RightSoftKey         = TxtSoftBack;
        editor_data.Identifier        = value;

        editor_start(win,&editor_data);  /* start the editor */
#endif /* NEW_EDITOR */

        winShow(win);
      }
      else
      {
        if (value EQ V_ID_CALL)
        {
          callNumber(FFS_flashData.voice_mail);
          V_destroy(win);
        }
      }


      break;

    case E_ABORT:
        V_destroy(win);

        break;

    case E_RETURN:
    case E_EXIT:

    default:
        return;
  }
}

/*******************************************************************************

 $Function:     V_win_cb

 $Description:  Window callback function for the voice mailbox window

 $Returns:    Execution status

 $Arguments:  e - event id
        w - mfw window handle

*******************************************************************************/

static int V_win_cb (MfwEvt e, MfwWin *w)
    /* V window event handler */
{
    TRACE_FUNCTION ("V_win_cb()");

    switch (e)
  {
        case MfwWinVisible:  /* window is visible  */
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

 $Function:     SmsMenu_standard_dialog_cb

 $Description:  Handler function for standard dialogs in smsmenu module

 $Returns:    Execution status

 $Arguments:    win - current window
        Identifier - unique id
        Reason- event id

*******************************************************************************/

int SmsMenu_standard_dialog_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason)
{
  T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_M_DEL_ALL    * data = (T_M_DEL_ALL *)win_data->user;

    TRACE_FUNCTION ("SmsMenu_standard_dialog_cb()");


  switch (Identifier)
  {
  case SMSMENU_ID_DELETEALL:

    switch (Reason)
    {
    // User want to delete the SMS
    case INFO_KCD_LEFT:
      // Trigger the deletion
      SmsMenu_sms_delete_all(win);
      break;

    case INFO_KCD_RIGHT:
    case INFO_KCD_HUP:
    case INFO_TIMEOUT:
	  // API - 17-01-03 - 1571 - Add this call to sms_delete() to free memory
      sms_delete(data->sms_hnd);
	  data->sms_hnd = NULL;
      /* Destroy the owner of the dialog box */
      M_DEL_ALL_destroy(win);
      /* Send E_RETURN event to parent of M_DEL_ALL */
     /*SPR2139 removed the sending of a destroy event to parent window*/
      break;

    default:
      return MFW_EVENT_REJECTED;
    }
    break;
    /*SPR 2139, if no messages or sms not initialised, just destroy the delete all window*/
  case SMSMENU_ID_NOMESSAGES:
  case SMSMENU_ID_NOTREADY:
  	    M_DEL_ALL_destroy(win);
    break;

  case SMSMENU_ID_DELOK:
  case SMSMENU_ID_DELERR:
/*SPR2139 Add this call to sms_delete() to free memory*/
      sms_delete(data->sms_hnd);
	  data->sms_hnd = NULL;
    /* Destroy the owner of the dialog box */
    M_DEL_ALL_destroy(win);
    break;

  case SMSMENU_ID_WAIT:
    // No reaction to any event (wait to be destroyed when MFW event received)
    break;

  default:
    return MFW_EVENT_REJECTED;
  }
  return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     SmsMenu_loadDialogDefault

 $Description:  Loads standard default data for SMS information dialogs.

 $Returns:    Execution status

 $Arguments:    DisplayInfo - display settings

*******************************************************************************/

void SmsMenu_loadDialogDefault(T_DISPLAY_DATA * DisplayInfo)
{
	dlg_initDisplayData_TextId( DisplayInfo, TxtNull, TxtNull, TxtNull,  TxtNull, COLOUR_STATUS);
	dlg_initDisplayData_events( DisplayInfo, (T_VOID_FUNC)SmsMenu_standard_dialog_cb, THREE_SECS, KEY_CLEAR);
	DisplayInfo->Identifier   = DIALOGS_REASON_NULL;
}


/*******************************************************************************

 $Function:     SmsMenu_call_voice

 $Description:  dummy function.

 $Returns:    none

 $Arguments:    none

*******************************************************************************/

void SmsMenu_call_voice(void)
{
    TRACE_FUNCTION ("call_voice");
}

/*******************************************************************************

 $Function:     SmsMenu_startcall

 $Description:  dummy function.

 $Returns:    Execution status

 $Arguments:    DisplayInfo - display settings

*******************************************************************************/

void SmsMenu_startcall(UBYTE * VoiceNumber)
{
    TRACE_FUNCTION ("startcall");
}

/*******************************************************************************

 $Function:     SmsMenu_sms_delete_all

 $Description:  Delete all SMS messages.

 $Returns:    Execution status

 $Arguments:    win - current window.

*******************************************************************************/

int SmsMenu_sms_delete_all(T_MFW_HND win)
{
    //T_MFW_HND  win  = mfw_parent (mfw_header());
   //  T_DISPLAY_DATA DisplayInfo;
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_M_DEL_ALL    * data = (T_M_DEL_ALL *)win_data->user;

    TRACE_FUNCTION("SmsMenu_sms_delete_all()");
      //May 27, 2005    MMI-FIX-29869 x0018858
#ifdef FF_MMI_SMS_DYNAMIC
	data->g_SmsMenu_messages = (T_MFW_SMS_MSG*)ALLOC_MEMORY (g_max_messages * sizeof(T_MFW_SMS_MSG)  );
	g_ListLength2 = sms_message_list(MFW_SMS_ALL, data->g_SmsMenu_messages, g_max_messages);
#else    
    g_ListLength2 = sms_message_list(MFW_SMS_ALL, data->g_SmsMenu_messages, MAX_MESSAGES);
#endif   
    //NM 22.8 msgsLoadInfoDefaults2(&DisplayInfo);


    if (g_ListLength2 > 0)
    {
    	/* Display "Please wait" on the screen*/
    	/* it is start now to delete sms*/
   	/* SPR 2139, Removed call to dialog handler, as not needed*/

	// Feb 24, 2005 REF: CRR MMI-SPR-29102 xnkulkar
	// As the user has already confirmed that all SMS messages are to be deleted, ''End All' key 
       // press is to be ignored. Call function mmi_dialog_information_screen_delete_all() instead of
       // mmi_dialog_information_screen_forever().
       
	// data->waiting_win = mmi_dialog_information_screen_forever(win,TxtPleaseWait, NULL, COLOUR_STATUS_WAIT);

	data->waiting_win = mmi_dialog_information_screen_delete_all(win,TxtPleaseWait, NULL, COLOUR_STATUS_WAIT);

	/* delete now the first sms and than wait */
      	/* for the MFW EVENT "E_SMS_OK"           */

    	g_ListLength2 --;
	 /*a0393213 OMAPS00109953 - memory passed as parameter*/	
    	sms_msg_delete(data->g_SmsMenu_messages[g_ListLength2].index,data->g_SmsMenu_messages[g_ListLength2].mem);	
    	}
	//May 27, 2005    MMI-FIX-29869 x0018858
#ifdef FF_MMI_SMS_DYNAMIC
        if (g_ListLength2 == 0)
	FREE_MEMORY((U8*)data->g_SmsMenu_messages, g_max_messages * sizeof(T_MFW_SMS_MSG));
#endif	
	/*SPR 2139, don't need "else" as empty list case handled earlier*/
    return 1;

}

/*******************************************************************************

 $Function:     SmsRead_convertMailbox

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
UBYTE SmsRead_convertMailbox(T_MFW_MAILBOX_ID i, char *mail_number, char *mail_alpha)
{
//  USHORT max_rcd;  // RAVI
  USHORT nlen;

  char *international = mail_number; // Used to add '+' to the number if international

    TRACE_FUNCTION ("SmsRead_convertMailbox()");


    // If international number, add the '+' symbol
    if (FFS_flashData.mbn_numTp & 0x10)
    {
      international[0] = '+';
      international[1] = '\0';
      international ++;
    }
    // Convert BCD number into string
    cmhPHB_getAdrStr(international, PHB_MAX_LEN - 1, FFS_flashData.mbn_Num, FFS_flashData.mbn_len);

    // If the same number then we get the alpha identifier
    cmhPHB_getMfwTagNt((UBYTE*)FFS_flashData.mbn_AlphId, 10, (UBYTE*)mail_alpha, &nlen);
	
    mail_alpha[nlen] = '\0';

    return TRUE;

}

static void SmsMenu_StandardEditor_cb (T_MFW_HND win, USHORT Identifier, SHORT reason)
{
 // T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;   // RAVI
//  T_V     * voice_data = (T_V *)win_data->user; // RAVI

  TRACE_FUNCTION ("SmsMenu_StandardEditor_cb()"); //Marcus: Issue 1490: 13/12/2002

  switch (reason)
  {
    case INFO_KCD_LEFT:


      if (Identifier EQ V_ID_CALL )
      {
        callNumber(FFS_flashData.voice_mail);
        flash_write(); /*SPR#1929 - DS - Store voicemail number in flash. Port of 1888 */
      }
      /* Marcus: Issue 1490: 13/12/2002: Start */
      else if (Identifier EQ V_ID_SET)
      {
      	  /*
      	  ** There is no need to copy the value entered into FFS_flashData.voicemail as the editor
      	  ** has already put the value there.
      	  */
  	  flash_write();
      }
      /* Marcus: Issue 1490: 13/12/2002: End */
      V_destroy(win);


      break;

    case INFO_KCD_RIGHT:
    case INFO_KCD_CLEAR:
    case INFO_KCD_HUP:
    V_destroy(win);
    default:
      break;
  }
}

/* SPR#1428 - SH - New Editor: not required */
#ifndef NEW_EDITOR
/*******************************************************************************

 $Function:     SmsMenu_loadEditDefault

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

void SmsMenu_loadEditDefault(T_EDITOR_DATA *editor_data)
{
    TRACE_FUNCTION ("SmsMenu_loadEditDefault()");

      editor_data->editor_attr.win.px   = 6;
      editor_data->editor_attr.win.py   = 12;
      editor_data->editor_attr.win.sx   = 80;
      editor_data->editor_attr.win.sy   = 20;
      editor_data->editor_attr.edtCol = COLOUR_EDITOR_XX;
      editor_data->editor_attr.font     = 0;
      editor_data->editor_attr.mode    = edtCurBar1;
      editor_data->editor_attr.controls   = 0;
      editor_data->editor_attr.text      = '\0';
        editor_data->editor_attr.size       = 0;
    editor_data->hide         = FALSE;
      editor_data->Identifier            = 0;  /* optional */
      editor_data->mode               = DIGITS_MODE;
    editor_data->destroyEditor      = TRUE;
    editor_data->LeftSoftKey          = TxtSoftSelect;
    editor_data->AlternateLeftSoftKey   = TxtNull;
    editor_data->RightSoftKey         = TxtSoftBack;
    editor_data->Identifier            =  0;
    editor_data->TextId              = '\0';
    editor_data->TextString           = NULL;
    editor_data->min_enter         = 1;  // Avoid to return empty strings
      editor_data->timeout              = FOREVER;
    editor_data->Callback             = (T_EDIT_CB)SmsMenu_StandardEditor_cb;

}

#endif /* NEW_EDITOR */

/* SPR#1428 - SH - New Editor: These functions moved here from MmiEditor.c */
#ifdef EASY_TEXT_ENABLED

/*******************************************************************************

 $Function:     eZitextIcon

 $Description:  Icon to be displayed if eZiText has been selected.

 $Returns:    None.

 $Arguments:  None.

*******************************************************************************/

int showeZiTextIcon(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND       win  = mfwParent(mfw_header());

	/* BEGIN ADD: Sumit: Locosto*/
		eZiTextWin = win;
	/* END ADD: Sumit: Locosto*/
	TRACE_FUNCTION ("showeZiTextIcon()");

	if(Easy_Text_Available == TRUE)
	{
	  if(FFS_flashData.PredTextSelected == TRUE)
	  {
	    T_DISPLAY_DATA display_info;
		dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtNull,  TxtNull, COLOUR_STATUS_EZITEXT);
		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)M_exeSendNew, FOUR_SECS, 0 );

	    mmi_dialogs_insert_animation(info_dialog(win, &display_info), 400, (MfwIcnAttr*)&IconTexteZiText,1);
	  }
	  else if(FFS_flashData.PredTextSelected == FALSE)
	  {
	    M_exeSendNew(m,i);
	  }
	}
	else
	    M_exeSendNew(m,i);
	return 1; /*a0393213 warnings removal-return statement added*/
}


/*******************************************************************************

 $Function:     EasyActivate

 $Description:  Icon to be displayed if eZiText has been selected.

 $Returns:    MFW_EVENT_CONSUMED.

 $Arguments:  None.

*******************************************************************************/

int Easy_Activate(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND win = mfwParent(mfw_header());
	T_DISPLAY_DATA display_info;

	if(FFS_flashData.PredTextSelected == FALSE)
	{
	  FFS_flashData.PredTextSelected = TRUE;
	  flash_write();

		dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtEasyText,  TxtActivated, COLOUR_STATUS_EZITEXT);
		dlg_initDisplayData_events( &display_info, NULL, THREE_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );

	  info_dialog(win, &display_info);

	  return MFW_EVENT_CONSUMED;
	}
	return MFW_EVENT_CONSUMED;/*a0393213 warnings removal -  added return statement*/
}

/*******************************************************************************

 $Function:     EasyActivate

 $Description:  Icon to be displayed if eZiText has been selected.

 $Returns:    MFW_EVENT_CONSUMED.

 $Arguments:  None.

*******************************************************************************/

int Easy_DeActivate(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND win = mfwParent(mfw_header());
	T_DISPLAY_DATA display_info;

	if(FFS_flashData.PredTextSelected == TRUE)
	{
	  FFS_flashData.PredTextSelected =  FALSE;
	  flash_write();

	  dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtEasyText,  TxtDeActivated, COLOUR_STATUS_EZITEXT);
	  dlg_initDisplayData_events( &display_info, NULL, THREE_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );
	  info_dialog(win, &display_info);

	  return MFW_EVENT_CONSUMED;

	}
	return MFW_EVENT_CONSUMED;/*a0393213 warnings removal*/
}
#endif /* EASY_TEXT_ENABLED */

#ifdef TI_PS_FF_CONC_SMS
/*******************************************************************************

 $Function:     Concatenate_Status

 $Description:  Displays the Concatenation Satus

 $Returns:    MFW_EVENT_CONSUMED.

 $Arguments:  None.

*******************************************************************************/

int Concatenate_Status(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND win = mfwParent(mfw_header());

	USHORT TextId2;

	// change by Sasken on March 24th 2004
	// Issue Number : SPR#15746
	// TextId2 string changed to TxtActivated from TxtActivate
	
	if (FFS_flashData.ConcatenateStatus ==  TRUE )
		TextId2    = TxtActivated;
	else
		TextId2    = TxtDeActivated;

	/*
	** Display a standard dialog, with no keys processed, for 3 secs
	*/
	info_screen(win, TxtConcat, TextId2, (T_VOID_FUNC) NULL);

	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     ConcatenateDeActivate

 $Description:  Deactivates the Concatenation Switch

 $Returns:    MFW_EVENT_CONSUMED.

 $Arguments:  None.

*******************************************************************************/

int Concatenate_DeActivate(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND win = mfwParent(mfw_header());
//	T_DISPLAY_DATA display_info;   // RAVI

	FFS_flashData.ConcatenateStatus =  FALSE;
	flash_write();

	/*
	** Display a standard dialog, with no keys processed, for 3 secs
	*/
	info_screen(win, TxtConcat, TxtDeActivated, (T_VOID_FUNC) NULL);

	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     ConcatenateActivate

 $Description:  Deactivates the Concatenation Switch

 $Returns:    MFW_EVENT_CONSUMED.

 $Arguments:  None.

*******************************************************************************/

int Concatenate_Activate(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND win = mfwParent(mfw_header());
//	T_DISPLAY_DATA display_info;   // RAVI

	FFS_flashData.ConcatenateStatus =  TRUE;
	flash_write();

	/*
	** Display a standard dialog, with no keys processed, for 3 secs
	*/
	info_screen(win, TxtConcat, TxtActivated, (T_VOID_FUNC) NULL);

	return MFW_EVENT_CONSUMED;
}
#endif /*TI_PS_FF_CONC_SMS*/

/*******************************************************************************

 $Function:     ConcatenateDisplay

 $Description:  Returns the status whether to display the menu item or not

 $Returns:    .

 $Arguments:  None.

*******************************************************************************/

USHORT ConcatenateDisplay( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
#ifdef TI_PS_FF_CONC_SMS
  return 0;
#else /*TI_PS_FF_CONC_SMS*/
  return 1;
#endif /*TI_PS_FF_CONC_SMS*/
}
#undef MMI_SMSMENU_C
