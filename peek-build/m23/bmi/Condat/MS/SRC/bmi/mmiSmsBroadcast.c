
/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		SMS Cell Broadcast
 $File:		    MmiSmsBroadcast.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    22/02/01                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

  Implementation of MMI SMS Cell Broadcast dynamic menu handling

********************************************************************************

 $History: MmiSmsBroadcast.c
 	Feb 16, 2007 REF: OMAPS00116175 x0pleela
       Description:  [MMI] Not possible to store CB settings CBSC with +CSAS and +CRES
       Solution: In function smscb_getCBsettings(), changed the incremental value from 2 to 1 as the 
       		successive CB parameters ( message id) was getting skipped while reading back from 
       		cell broadcast 	parameters 

	Nov 14, 2006 REF: OMAPS00099394  x0039928
       Description: TC2.1.51(BMI/MFW) - SMS - Read Broadcast message fails
       Solution: On press of back key in the message editor, it is directly taken out to the menu 
       instead of recreating the list.
                     
	Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
       Description: MFW memory size variable changed from U16 -> U32
       Solution: In function 'smscb_setCBsettings' variable 'd' is checked for limit and
                     duplication is removed. This was the reason for crash.
 
	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX

	25/10/00			Original Condat(UK) BMI version.	
	Issue Number : SPR#12722 on 29/03/04 by Deepa M.D
	Apr 29, 2004    REF: CRR 11630  Deepa M.D
	Fix:When a new entry channelId is added ,the whole list of channelIds are refreshed, 
	and the corresponding names are displayed in the list.
	   
 $End
*******************************************************************************/


/*******************************************************************************
                                                                              
                                Include Files
                                                                              
*******************************************************************************/

#define ENTITY_MFW

/* includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#include "custom.h"
#include "gsm.h"
#include "vsi.h"
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
#include "mfw_mnu.h"
#include "mfw_sat.h"
#include "mfw_tim.h"

#include "MmiMmi.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"

#include "MmiMain.h"
#include "MmiStart.h"
#include "MmiPins.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"

#include "MmiBookController.h"

#include "mmiSmsBroadcast.h"
#include "mmiSmsIdle.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif
#include "MmiIdle.h"

#include "cus_aci.h"

#include "mmiColours.h"

/* BEGIN ADD: Sumit : Req ID: : 31-Mar-2005*/
#ifndef NEPTUNE_BOARD
/* END ADD: Sumit : Req ID: : 31-Mar-2005*/
#include "ffs/ffs.h" /*SPR 1920*/
/* BEGIN ADD: Sumit : Req ID: : 31-Mar-2005*/
#else
#include "ffs.h"
#endif
/* END ADD: Sumit : Req ID: : 31-Mar-2005*/

#ifdef DEBUG_SMS_CB
#include "MmiHomezone.h"
#endif
#include "MmiSettings.h"



#define MENU_TEXT_LENGTH 16 /* count of chars to be displayed in one list entry */

/* SH - moved to .h file */

/*MC SPR 1920,FFS CB Topic name data structure*/
typedef struct
{
	uint8 name[CB_TAG_LENGTH]; 

} T_SMSCB_TOPIC_TAG;

/* storage for Cell Broadcast lists */
typedef struct 
{
    T_SMSCB_ATT 		 cb_attributes[MAX_MIDS];     /* list of attributes of messages stored in fifo */
	/*SPR 1920, replaced cb_buffer with an array of dynically allocated strings */
	char*				messages[MAX_STORED_PAGES];
	ListMenuData     	 *cb_list_attributes; 		//atttributes for the menu list

    T_SMSCB_STATUS		current_status;       	       /*read status for all messages    */
    
	UBYTE found_mid;    //indicate the already existing cb in the fifo
	UBYTE clear_mid;     //indicate the deleting process
	UBYTE result_used;	//to update the used-pointer, after deleting the cb message
	UBYTE total_number;  //this include the total number of messages in the fifo; need for Menu-list

	UBYTE multipage_number;  //this include the total page of a particular multipage; need for showing msg.
	UBYTE *multipage;        //pointer to the first page of multipage
	UBYTE start_used;	//indicate the start point of the multipage, used for the searching
	UBYTE end_used;     //indicate the end point of the multipage, used for the searching

	UBYTE used_pointer;  //start point of the free space link list
	UBYTE free_pointer;   //start point of the data link list

	
} T_SMSCB_DATA;



T_SMSCB_DATASTORE g_smscb_data; /* storage for Cell Broadcast Info lists */
T_SMSCB_DATA smscb_data; /* storage for Cell Broadcast  Message lists */



//Attributes of the list menu
static const MfwMnuAttr sms_cb_menuAttrib =
{
    &smscb_M_CBArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu       */
    /* x0045876, 14-Aug-2006 (WR - integer conversion resulted in a change of sign) */
    (U8) -1,                                 /* use default font         */
    NULL,                               /* with these items         */
    0,                                   /* number of items     */
    COLOUR_LIST_SMS, TxtNull, NULL, MNUATTRSPARE

};


#define OFFSET(index) (index*CB_PAGE_LENGTH)

//this shows traces of some variable
//#define DEBUG_SMS_CB 

#ifdef DEBUG_SMS_CB
	UBYTE DEBUG;
#endif

char* ffs_cb_tag_name(int channel_num, char* name);/*SPR1920, new function header*/
//these prototypes belongs to saving cb_messages in the fifo
UBYTE sms_cb_count_messages ( void );
UBYTE sms_cb_count_unread_messages ( void );
UBYTE sms_cb_delete_message (USHORT mid);
UBYTE sms_cb_find_termination (UBYTE *used);
UBYTE sms_cb_update_free_pointer (UBYTE temp);
UBYTE sms_cb_search_mid (USHORT mid, SHORT sn);
UBYTE sms_cb_store_message (T_MFW_SMS_CB *Message);
 UBYTE sms_cb_find_endpoint (USHORT mid, UBYTE end_used);
UBYTE sms_cb_find_startpoint (USHORT mid, UBYTE end_used);
UBYTE sms_cb_give_position_of_msg (UBYTE used, UBYTE positon);
UBYTE sms_cb_overwrite_old_message (T_MFW_SMS_CB *Message);


void sms_cb_show_cb_message(UBYTE link_list_position);
static ListMenuData * sms_cb_create_list (UBYTE number);
/*SPR 1920, removed header for function sms_cb_select_read*/
static void sms_cb_list_cb(T_MFW_HND win, ListMenuData * ListData);
void   sms_cb_store_attributes (UBYTE position, T_MFW_SMS_CB *Message);
static void sms_cb_editor_cb (T_MFW_HND win, USHORT Identifier, SHORT reason);

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
void sms_cb_loadEditDefault (T_AUI_EDITOR_DATA *editor_data);
#else /* NEW_EDITOR */
void sms_cb_loadEditDefault (T_EDITOR_DATA *editor_data);
#endif /* NEW_EDITOR */

/*********************************************************************
**********************************************************************

			cell broadcast declaration

*********************************************************************
**********************************************************************/

typedef struct
{
    int index; /* index of selected item in list */
} T_CB_index_param; 

typedef struct
{
    int index; /* index of selected item in list */
    char name[CB_TAG_LENGTH]; /* buffer for name to be associated with topic */
    char number[CB_MID_LENGTH]; /* buffer for channel number of topic */
} T_CB_EDT_MIDEDT_param;

typedef struct
{
    int index; /* index of selected item in list */
    char text[CB_TAG_LENGTH]; /* buffer for name to be associated with topic ??? size is dummy */
    char number[CB_MID_LENGTH]; /* buffer for channel number of topic ??? size is dummy */
} T_S_param; /* ??? T_S_param not to be defined here, but in "mmiSmsSend.h"! */


/* IDs used for identification of sources of events */
enum {
    smscb_M_CB_ID,
    smscb_M_CB_ACT_ID,
    smscb_M_CB_TOP_ID,
    smscb_M_CB_TOP_CONS_ID,
    smscb_M_CB_TOP_CONS_SEL_ID,
    smscb_CB_ID,
    smscb_CB_TEXTEDT_ID,
    smscb_CB_OPT_ID,
    smscb_CB_OPT_DEL_NOACK_ID,
    smscb_CB_EDT_TEXTEDT_ID,
    smscb_CB_EDT_MIDEDT_ID
} smscb_ID;



void msgsExtractPhoneNumber(UBYTE *InputString, UBYTE *Buffer);
/* forward declaration of local start routines */
/*SPR 1920 removed  header for smscb_M_CB_ACT_start()*/
static T_MFW_HND smscb_M_CB_TOP_start(T_MFW_HND parent_window, void *dummy);
static T_MFW_HND smscb_M_CB_TOP_CONS_start(T_MFW_HND parent_window, void *dummy);
static T_MFW_HND smscb_M_CB_TOP_CONS_SEL_start(T_MFW_HND parent_window, T_CB_index_param * parameter);
static T_MFW_HND smscb_CB_start(T_MFW_HND parent_window, void *dummy);
static T_MFW_HND smscb_CB_TEXTEDT_start(T_MFW_HND parent_window, T_CB_index_param * parameter);
static T_MFW_HND smscb_CB_OPT_start(T_MFW_HND parent_window, T_CB_index_param * parameter);
static T_MFW_HND smscb_CB_OPT_DEL_NOACK_start(T_MFW_HND parent_window, T_CB_index_param * parameter);
static T_MFW_HND smscb_CB_EDT_TEXTEDT_start(T_MFW_HND parent_window, T_CB_index_param * parameter);
static T_MFW_HND smscb_CB_EDT_MIDEDT_start(T_MFW_HND parent_window, T_CB_EDT_MIDEDT_param * parameter);


void info_screen (T_MFW_HND win, USHORT TextId, USHORT TextId2, T_VOID_FUNC  Callback)
    /* present info <TextId> for 3 seconds in a simple infoscreen */
{

    T_DISPLAY_DATA display_info;

    TRACE_FUNCTION ("info_screen()");

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TextId,  TextId2 , COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)Callback, THREE_SECS, 0 );
    display_info.Identifier   = 0;

    info_dialog (win, &display_info); 
}

/*********************************************************************
**********************************************************************

			cell broadcast routines DECLARATION

*********************************************************************
**********************************************************************/
static T_MFW smscb_delete_topic(UBYTE MidOffset);
/*SPR 1920, removed function temp_smscb_M_CB_ExeRead()*/
/* SH moved prototype to header file */

#if(0) /* x0039928 - Lint warning removal */
static void listCreate(MfwHnd Parent, MfwMnuAttr *a , MfwEvt e, ListCbFunc cbFunc)
{

    ListMenuData list_menu_data;

    /* initialization of the dialog data */
    list_menu_data.Attr           = a;
    list_menu_data.List           = a->items;
    list_menu_data.ListLength     = a->nItems;/* Number of entries in list menu.    */
    list_menu_data.ListPosition   = 1;/* True cursor position in list menu. */
    list_menu_data.CursorPosition = 1;/* Cursor position in list snapshot.  */
    list_menu_data.SnapshotSize   = 4;/* Number of list entries to view.    */
    list_menu_data.Font           = 0;
    list_menu_data.LeftSoftKey    = TxtSoftSelect;
    list_menu_data.RightSoftKey   = TxtSoftBack;
    /* SPR1428 - SH - Accept menu select key */
    list_menu_data.KeyEvents      = KEY_CLEAR | KEY_RIGHT | KEY_LEFT | KEY_MNUUP | KEY_MNUDOWN | KEY_MNUSELECT;
    list_menu_data.Reason         = 0;
    list_menu_data.Strings        = FALSE;
	list_menu_data.autoDestroy    = TRUE;

    listDisplayListMenu(Parent, &list_menu_data, cbFunc,0);

}
#endif
// end c020 rsa
/*SPR1920, removed declarations and functions for menu-handling*/




/*********************************************************************
**********************************************************************

		M_CB_ACT	     DYNAMIC MENU WINDOW. CONTENTS BEHAVIOUR

*********************************************************************
**********************************************************************/
/*******************************************************************************

 $Function:    	smscb_M_CB_ACT_ExeNo

 $Description:	

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
int smscb_M_CB_ACT_ExeNo(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       win  = mfwParent((MfwHdr *)mfw_header());
	/*SPR 1920, removed unneccessary variables*/
	/*GW SPR#2470 Call 'smscb_setCBsettings' so that sAT_PlusCSCB is also called.*/
    
	settingsSetStatus(SettingsCBDisable, SET_SETTING );/*Remember settings in FFS*/
	//	sms_set_mt_ind(MT_CB_NO_IDX);
	smscb_setCBsettings(SMSCB_SWITCH_OFF);

    info_screen(win, TxtDeActivated,TxtNull, NULL); 


    statusCBCH(FALSE);

	/* SPR 1920, removed window destruction code*/ 
    return MFW_EVENT_CONSUMED;
}

#ifdef DEBUG_SMS_CB
	T_MFW_HND timer;

 int test_tim_cb(MfwEvt event, MfwTim *timer_info)
 {	
	int i;
 
	test_function_to_send_cb ();
	tim_delete(timer);
	/***************************Go-lite Optimization changes Start***********************/
	//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	for (i=0; i<MAX_STORED_PAGES ; i++)	
	{
			TRACE_EVENT_P2("free_index: %d, i:%d", smscb_data.cb_attributes[i].next_link,i); 
	}
	/***************************Go-lite Optimization changes end***********************/
	TRACE_EVENT_P1("count of messages:%d", sms_cb_count_messages ()); 

	for (i=0; i<MAX_STORED_PAGES ; i++)	
	{
	
		TRACE_EVENT_P2("start_page: %d, i:%d", smscb_data.cb_attributes[i].start_page,i); 
	}

	
	TRACE_EVENT_P1("smscb_data.used_pointer%d", smscb_data.used_pointer); 
	TRACE_EVENT_P1("smscb_data.free_pointer%d", smscb_data.free_pointer); 

	return MFW_EVENT_CONSUMED;
 }

#endif 
/*******************************************************************************

 $Function:    	smscb_M_CB_ACT_ExeYes

 $Description:	

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
int smscb_M_CB_ACT_ExeYes(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       win  = mfwParent((MfwHdr *)mfw_header());
  	/*SPR 1920, removed unneccessary variables*/
	/*GW SPR#2470 Call 'smscb_setCBsettings' so that sAT_PlusCSCB is also called.*/
    
	settingsSetStatus(SettingsCBDisable, CLEAR_SETTING );/*Remember settings in FFS*/
	//sms_set_mt_ind(MT_CB_MSG);
	smscb_setCBsettings(SMSCB_SWITCH_ON);

    info_screen(win, TxtActivated,TxtNull, NULL); 


/*SPR1959, removed declarations and functions for menu-handling*/
#ifdef DEBUG_SMS_CB
    timer = tim_create(NULL, 5000, (MfwCb)test_tim_cb);
    tim_start(timer);
    




#endif //DEBUG_SMS_CB
    return MFW_EVENT_CONSUMED;
}





/*********************************************************************
**********************************************************************

		M_CB_TOP	     DYNAMIC MENU WINDOW. CONTENTS BEHAVIOUR

*********************************************************************
**********************************************************************/
/*******************************************************************************

 $Function:    	smscb_M_CB_TOP_ExeConsult

 $Description:	

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
int smscb_M_CB_TOP_ExeConsult(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       win      = mfwParent((MfwHdr *)mfw_header());
   	/*SPR 1920, removed unneccessary variables*/


#ifdef DEBUG_SMS_CB
    UBYTE ii;
   	for (ii = 0; ii < MAX_MIDS; ii++)
	{
		/***************************Go-lite Optimization changes Start***********************/
		//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
		TRACE_EVENT_P2 ("g_smscb_data.CBTopics.entry[0].msg_id:%d, i:%d", g_smscb_data.CBTopics.entry[ii].msg_id,ii);
		/***************************Go-lite Optimization changes end***********************/
	}
#endif


	if (!smsidle_get_ready_state())
	{
	    info_screen(0, TxtSimNot, TxtReady, NULL); 
	    TRACE_EVENT ("SIM is not ready !!");
		return MFW_EVENT_CONSUMED;  /* x0039928 - Lint warning fix */
	}		
	
	if (g_smscb_data.CBTopics.length EQ 0) 
	{
	    info_screen(0, TxtEmptyList,TxtNull, NULL); 

	    TRACE_EVENT ("CellBroadcast Topic-list is empty !!");
		return MFW_EVENT_CONSUMED;  /* x0039928 - Lint warning fix */
	}

    //	selection "Consult": menucreate M_CB_TOP_CONS
    return (int)smscb_M_CB_TOP_CONS_start (win, NULL);
}
T_CB_index_param CB_index_param;/*MC SPR1920 added global variable*/
/*******************************************************************************

 $Function:    	smscb_M_CB_TOP_ExeAddNew

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int smscb_M_CB_TOP_ExeAddNew(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND      win      = mfwParent((MfwHdr *)mfw_header());
 	/*SPR 1920, removed unneccessary variables*/
    int            index;

    //	selection "Add": call sms_cb_edit_topic(List=CBTopics, Index=ADD)
    index = g_smscb_data.CBTopics.length;/* get the index of the next free entry in list CBTopics */

    if (index > MAX_MIDS)
	{
	    /* all entries are already used */
	    info_screen (win, TxtFull /* ??? */,TxtNull, NULL);
	    return MFW_EVENT_CONSUMED;
	}
    else
	{


	    CB_index_param.index = index; 
	    return (int)smscb_CB_EDT_TEXTEDT_start(win, &CB_index_param);
	}
}


/*********************************************************************
**********************************************************************

			M_CB_TOP_CONS	 DYNAMIC MENU WINDOW. DECLARATION

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

    /* internal data */
    T_CB_index_param CB_index_param; /* parameter for called submenus (index in list) */
    MfwMnuItem      menuItems[MAX_MIDS]; /* List=CBTopics */
    ListMenuData    list_menu_data;
} T_M_CB_TOP_CONS;

static T_MFW_HND smscb_M_CB_TOP_CONS_create (T_MFW_HND parent);
static void smscb_M_CB_TOP_CONS_destroy (T_MFW_HND window);
static void smscb_M_CB_TOP_CONS_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * dummy);
static void smscb_M_CB_TOP_CONS_listmnu_cb (T_MFW_HND * win, ListMenuData * ListData);

/* x0045876, 14-Aug-2006 (WR - function declared implicitly) */
static int smscb_M_CB_TOP_CONS_SEL_ExeDelete(T_MFW_HND * win);

static const MfwMnuAttr smscb_M_CB_TOP_CONSAttrib =
{
    &smscb_M_CB_TOP_CONSArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu       */

    /* x0045876, 14-Aug-2006 (WR - integer conversion resulted in a change of sign) */
    (U8) -1,                                 /* use default font         */
    
    NULL,                               /* with these items         */
    0,                                   /* number of items     */
	COLOUR_LIST_SMS,	TxtNull, NULL, MNUATTRSPARE

};

/*********************************************************************
**********************************************************************

			M_CB_TOP_CONS	DYNAMIC MENU WINDOW. IMPLEMENTATION

*********************************************************************
**********************************************************************/

/*******************************************************************************

 $Function:    	smscb_M_CB_TOP_CONS_start

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
T_MFW_HND smscb_M_CB_TOP_CONS_start(T_MFW_HND parent_window, void *dummy)
{
    T_MFW_HND win;

    TRACE_FUNCTION ("smscb_M_CB_TOP_CONS_start()");

    win = smscb_M_CB_TOP_CONS_create (parent_window);

    if (win NEQ NULL)
	{
	    SEND_EVENT (win, E_INIT, smscb_M_CB_TOP_CONS_ID, NULL);
	}
    return win;
}


/*******************************************************************************

 $Function:    	smscb_M_CB_TOP_CONS_create

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static T_MFW_HND smscb_M_CB_TOP_CONS_create(MfwHnd parent_window)
{
    T_M_CB_TOP_CONS      * data = (T_M_CB_TOP_CONS *)ALLOC_MEMORY (sizeof (T_M_CB_TOP_CONS));
    T_MFW_WIN  * win;

    TRACE_FUNCTION ("smscb_M_CB_TOP_CONS_create()");

    /*
     * Create window handler
     */

    data->win = 
	win_create (parent_window, 0, 0, NULL);
    if (data->win EQ NULL)
	{
	    return NULL;
	}

    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)smscb_M_CB_TOP_CONS_exec_cb;
    data->mmi_control.data   = data;
    win                      = ((T_MFW_HDR *)data->win)->data;
    win->user                = (void *)data;
    data->parent             = parent_window;

    /*
     * return window handle
     */

    winShow(data->win);
    return data->win;
}

static void smscb_M_CB_TOP_CONS_destroy(MfwHnd own_window)
{
    T_MFW_WIN * win_data;
    T_M_CB_TOP_CONS     * data;

    TRACE_FUNCTION ("smscb_M_CB_TOP_CONS_destroy()");

    if (own_window)
	{
	    win_data = ((T_MFW_HDR *)own_window)->data;
	    data = (T_M_CB_TOP_CONS *)win_data->user;

	    if (data)
		{
		    /*
		     * Delete WIN handler
		     */ 
		    win_delete (data->win);

		    /*     
		     * Free Memory
		     */
		    FREE_MEMORY ((void *)data->list_menu_data.List, MAX_MIDS * sizeof (MfwMnuItem)); // c017 rsa
		    FREE_MEMORY ((void *)data, sizeof (T_M_CB_TOP_CONS));
		}
	    else
		{
		    TRACE_EVENT ("smscb_M_CB_TOP_CONS_destroy() called twice");
		}
	}
}

/*******************************************************************************

 $Function:    	smscb_M_CB_TOP_CONS_recreate

 $Description:	

 $Returns:		
 
 $Arguments:	
 
*******************************************************************************/
static void smscb_M_CB_TOP_CONS_recreate(T_M_CB_TOP_CONS *data)
{
    int i;
   // int len;   // RAVI

#ifdef DEBUG_SMS_CB
    UBYTE ii;
	   TRACE_EVENT ("before create a list !"); 
	for (ii = 0; ii < MAX_MIDS; ii++)
	{
	   	   TRACE_EVENT_P2 (" 01 midlist:%d, i:%d", g_smscb_data.CBTopics.entry[ii].msg_id,ii);
	}
	/***************************Go-lite Optimization changes Start***********************/
	//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P2 (" 01 midlist:%d, i:%d", g_smscb_data.CBTopics.entry[ii].msg_id,ii);
	/***************************Go-lite Optimization changes end***********************/
#endif

    //on RECREATE|INIT: menucreate dynamiclist (List=CBTopics, SoftLeft=SELECT, SoftRight=BACK) /* show: 1. Name 2. MID */ 

    // c017 rsa
    /* allocate sufficient memory to hold the maximum list of menu items */
    data->list_menu_data.List = (MfwMnuItem *)ALLOC_MEMORY (MAX_MIDS * sizeof (MfwMnuItem));
    // end c017 rsa

    for (i=0; i < g_smscb_data.CBTopics.length; i++) /* Fill Menu List */
	{
		mnuInitDataItem(&data->list_menu_data.List[i]);


	    /* get the correct entry in list of CBTopics */
	    data->list_menu_data.List[i].str = g_smscb_data.CBTopics.entry[i].name; 
	    data->list_menu_data.List[i].flagFunc = item_flag_none;


	}

	//actual number of entries in list menu
	data->list_menu_data.ListLength = g_smscb_data.CBTopics.length; 

	data->list_menu_data.autoDestroy    = TRUE;

    listDisplayListMenu(data->win, &data->list_menu_data, (ListCbFunc)smscb_M_CB_TOP_CONS_listmnu_cb,0);
}

/*******************************************************************************

 $Function:    	smscb_M_CB_TOP_CONS_exec_cb

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void smscb_M_CB_TOP_CONS_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * dummy)
    /* callback handler for events sent to to trigger execution */
{

    T_MFW_WIN         * win_data = ((T_MFW_HDR *) win)->data;
    T_M_CB_TOP_CONS   * data = (T_M_CB_TOP_CONS *)win_data->user;
    T_MFW_HND         * l_parent;
    SHORT               l_id;
//   UBYTE i;   // RAVI.

    TRACE_FUNCTION ("smscb_M_CB_TOP_CONS_exec_cb()");

    switch (event)
	{
	case E_INIT:
	    /* initialization of administrative data */
	    data->id = value;

	    /* initialization of the dialog data */
	    data->list_menu_data.ListPosition   = 1;/* True cursor position in list menu. */
	    data->list_menu_data.CursorPosition = 1;/* Cursor position in list snapshot.  */
	    data->list_menu_data.SnapshotSize   = 4;/* Number of list entries to view.    */
	    data->list_menu_data.Font           = 0;
	    data->list_menu_data.LeftSoftKey    = TxtDelete;
	    data->list_menu_data.RightSoftKey   = TxtSoftBack;
	    data->list_menu_data.KeyEvents      = KEY_CLEAR | KEY_RIGHT | KEY_LEFT | KEY_MNUUP | KEY_MNUDOWN;
	    data->list_menu_data.Reason         = 0;
	    data->list_menu_data.Strings        = TRUE; // c020 rsa
	    data->list_menu_data.Attr           = (MfwMnuAttr*)&smscb_M_CB_TOP_CONSAttrib;
        data->list_menu_data.autoDestroy    = TRUE;

	    /* NOBREAK */
	case E_RETURN: 
	case E_EXIT: /* terminate EXIT from CB_EDT_MIDEDT|M_CB_TOP_CONS_SEL here */
	    /* (re)create the dialog handler */
        TRACE_FUNCTION ("EXIT in M_CB_TOP_CONS_exec_cb");	    

        //the special case : if the user deleted the last CB item
        //check if there is any more CB in the list when not
        //give a infoscreen and go back to the CB menu
		if (g_smscb_data.CBTopics.length EQ 0) 
 		{
		    info_screen(0, TxtEmptyList,TxtNull, NULL); 
		    l_parent = data->parent;
		    l_id = data->id;
		    smscb_M_CB_TOP_CONS_destroy(data->win);
		    SEND_EVENT (l_parent, event, l_id, NULL); /* forward event to parent */

		}
		else
		{
		    smscb_M_CB_TOP_CONS_recreate(data);

		    /* put the (new) dialog window on top of the window stack */
    	    /* winUpdate(data->win); c021 rsa */
    	}
	    break;
	case E_ABORT: /* defaultreaction */
	    TRACE_EVENT ("ABORT in M_CB_TOP_CONS_exec_cb");	    

	    l_parent = data->parent;
	    l_id = data->id;
	    smscb_M_CB_TOP_CONS_destroy(data->win);
	    SEND_EVENT (l_parent, event, l_id, NULL); /* forward event to parent */
	    break;
	default:
	    return;
	}
}

/*******************************************************************************

 $Function:    	smscb_M_CB_TOP_CONS_listmnu_cb

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void smscb_M_CB_TOP_CONS_listmnu_cb (T_MFW_HND * win, ListMenuData * ListData)
    /* smscb_M_CB_TOP_CONS menu event handler */
{
    T_MFW_WIN            * win_data = ((T_MFW_HDR *)win)->data;
    T_M_CB_TOP_CONS      * data = (T_M_CB_TOP_CONS *)win_data->user;
    int index;

    TRACE_FUNCTION ("smscb_M_CB_TOP_CONS_listmnu_cb()");

    switch (ListData->Reason)
        {
	case LISTS_REASON_SELECT:
	    //on SELECT: menucreate M_CB_TOP_CONS_SEL(Index=selection)
	    index = ListData->ListPosition;
	    data->CB_index_param.index = index;

	  	smscb_M_CB_TOP_CONS_SEL_ExeDelete(win);

	    break;
	case LISTS_REASON_BACK: /* back to previous menu */
	    smscb_M_CB_TOP_CONS_destroy(data->win);
	    SEND_EVENT (data->parent, E_RETURN, data->id, NULL); // c007 rsa
	    break;
	default:
	    return;
        }
    return;
}


void ExeDelete_infoscreen_cb (T_MFW_HND win, USHORT identifier, UBYTE reason);

/*********************************************************************
**********************************************************************

			  M_CB_TOP_CONS_SEL   DYNAMIC MENU WINDOW. CONTENTS BEHAVIOUR

*********************************************************************
**********************************************************************/


/*******************************************************************************

 $Function:    	smscb_M_CB_TOP_CONS_SEL_ExeDelete

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static int smscb_M_CB_TOP_CONS_SEL_ExeDelete(T_MFW_HND * win)
{
    T_MFW_WIN          * win_data = ((T_MFW_HDR *)win)->data;
    T_M_CB_TOP_CONS    * data = (T_M_CB_TOP_CONS *)win_data->user;
//    T_MFW_HND          * l_parent;  // RAVI
//    SHORT                l_id;  // RAVI
    int index;

    //index is the selected MID
    index = data->CB_index_param.index;

    //delete now the user selected Topic MID
    smscb_delete_topic((UBYTE) index);

    //show infoscreen ("Deleted"); 
    info_screen(win, TxtDeleted, TxtNull, (T_VOID_FUNC) ExeDelete_infoscreen_cb); 


	//save the updated Topic list on the SIM
	smscb_setCBsettings(SMSCB_SWITCH_ON);

    //selfdestroy; sendevent EXIT


    return MFW_EVENT_CONSUMED;
}

void ExeDelete_infoscreen_cb (T_MFW_HND win , USHORT identifier, UBYTE reason)
{
    T_MFW_WIN          * win_data = ((T_MFW_HDR *)win)->data;
    T_M_CB_TOP_CONS    * data     = (T_M_CB_TOP_CONS *)win_data->user;
    SHORT                l_id;

    l_id     = data->id;

   //nm go back and recreate the CB list
   SEND_EVENT (win, E_EXIT, l_id, NULL);
   
}

/*********************************************************************
**********************************************************************

			CB_EDT_TEXTEDT	 DYNAMIC MENU WINDOW. DECLARATION

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

    T_MFW_HND       editor;

    /* internal data */
    T_CB_index_param * param; /* pointer to parameter space in parent (index in list) */
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_info;	/* SPR#1428 - SH - New Editor data */
#else
    T_EDITOR_DATA   editor_info; /* storage for editor attributes */
#endif
    T_CB_EDT_MIDEDT_param CB_EDT_MIDEDT_param; /* parameter space for buffer for editing */
} T_CB_EDT_TEXTEDT;

static T_MFW_HND smscb_CB_EDT_TEXTEDT_create (T_MFW_HND parent);
static void smscb_CB_EDT_TEXTEDT_destroy (T_MFW_HND window);
static void smscb_CB_EDT_TEXTEDT_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static void smscb_CB_EDT_TEXTEDT_editor_cb (T_MFW_HND win, USHORT Identifier, USHORT event);

/*********************************************************************
**********************************************************************

			CB_EDT_TEXTEDT	DYNAMIC MENU WINDOW. IMPLEMENTATION

*********************************************************************
**********************************************************************/

T_MFW_HND smscb_CB_EDT_TEXTEDT_start(T_MFW_HND parent_window, T_CB_index_param * parameter)
{
    T_MFW_HND win;

    TRACE_FUNCTION ("smscb_CB_EDT_TEXTEDT_start()");

    win = smscb_CB_EDT_TEXTEDT_create (parent_window);

    if (win NEQ NULL)
	{
	    SEND_EVENT (win, E_INIT, smscb_CB_EDT_TEXTEDT_ID, (void *)parameter);
	}
    return win;
}


/*******************************************************************************

 $Function:    	smscb_CB_EDT_TEXTEDT_create

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static T_MFW_HND smscb_CB_EDT_TEXTEDT_create(MfwHnd parent_window)
{
    T_CB_EDT_TEXTEDT      * data = (T_CB_EDT_TEXTEDT *)ALLOC_MEMORY (sizeof (T_CB_EDT_TEXTEDT));
    T_MFW_WIN             * win;

    TRACE_FUNCTION ("smscb_CB_EDT_TEXTEDT_create()");

    /*
     * Create window handler
     */

    data->win = 
	win_create (parent_window, 0, 0, NULL);
    if (data->win EQ NULL)
	{
	    return NULL;
	}

    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)smscb_CB_EDT_TEXTEDT_exec_cb;
    data->mmi_control.data   = data;
    win                      = ((T_MFW_HDR *)data->win)->data;
    win->user                = (void *)data;
    data->parent             = parent_window;

    /*
     * return window handle
     */

    winShow(data->win);
    return data->win;
}


/*******************************************************************************

 $Function:    	smscb_CB_EDT_TEXTEDT_destroy

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void smscb_CB_EDT_TEXTEDT_destroy(MfwHnd own_window)
{
    T_MFW_WIN            * win_data;
    T_CB_EDT_TEXTEDT     * data;

    TRACE_FUNCTION ("smscb_CB_EDT_TEXTEDT_destroy()");

    if (own_window)
	{
	    win_data = ((T_MFW_HDR *)own_window)->data;
	    data     = (T_CB_EDT_TEXTEDT *)win_data->user;

	    if (data)
		{
		    /*
		     * Delete WIN handler
		     */ 
		    win_delete (data->win);

		    /*     
		     * Free Memory
		     */
		    FREE_MEMORY ((void *)data, sizeof (T_CB_EDT_TEXTEDT));
		}
	    else
		{
		    TRACE_EVENT ("smscb_CB_EDT_TEXTEDT_destroy() called twice");
		}
	}
}

/*******************************************************************************

 $Function:    	smscb_CB_EDT_TEXTEDT_exec_cb

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void smscb_CB_EDT_TEXTEDT_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
    /* callback handler for events sent to to trigger execution */
{


    T_MFW_WIN            * win_data = ((T_MFW_HDR *) win)->data;
    T_CB_EDT_TEXTEDT     * data = (T_CB_EDT_TEXTEDT *)win_data->user;
    int      
   	index;

    TRACE_FUNCTION ("smscb_CB_EDT_TEXTEDT_exec_cb()");

    switch (event)
	{
	case E_INIT:
	    /* initialization of administrative data */
	    data->id = value;
	    data->param = (T_CB_index_param *)parameter; /* store the pointer to the parameter for later use */


#ifdef DEBUG_SMS_CB
		/***************************Go-lite Optimization changes Start***********************/
		//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
		TRACE_EVENT_P1( "the index is %d", data->param->index);
		/***************************Go-lite Optimization changes end***********************/
#endif

	    /* initialization of the dialog data */
	    index = data->param->index;
	    data->CB_EDT_MIDEDT_param.index = index; /* copy the index to parameter space of next menu */

data->CB_EDT_MIDEDT_param.name[0] = '\0'; /* provide empty entry */

	    //on INIT: exec "copy params to dynamic memory <data>"; RECREATE
	    /* NOBREAK */
	case E_RETURN:    //on RETURN: RECREATE  // recreate editor if back from options

		/* SPR#1428 - SH - New Editor changes */
		
#ifdef NEW_EDITOR
		AUI_edit_SetDefault(&data->editor_info);
		AUI_edit_SetDisplay(&data->editor_info, ZONE_FULL_SK_TITLE, COLOUR_EDITOR_XX, EDITOR_FONT);
		AUI_edit_SetEvents(&data->editor_info, value, TRUE, TEN_SECS,  (T_AUI_EDIT_CB)smscb_CB_EDT_TEXTEDT_editor_cb);
		AUI_edit_SetTextStr(&data->editor_info, TxtSoftOK, TxtDelete, TxtEnterName, NULL);
		// Change by Sasken ( Deepa M.d) on March 29th 2004
		// Issue Number : MMI-SPR-12722
		// Subject: Right softkey should be "Back" when the editor is empty
		// Bug : Right softkey should be "Back" when the editor is empty
		// Solution : Added alternative string for the right softkey tobe displayed when the editor is empty
		
		AUI_edit_SetAltTextStr(&data->editor_info, 0, NULL, TRUE, TxtSoftBack);
		AUI_edit_SetBuffer(&data->editor_info, ATB_DCS_ASCII, (UBYTE *)data->CB_EDT_MIDEDT_param.name, CB_TAG_LENGTH);
		AUI_edit_SetMode(&data->editor_info, ED_MODE_ALPHA, ED_CURSOR_BAR);

	    data->editor = AUI_edit_Start(data->win,&data->editor_info);  /* start the editor */
#else /* NEW_EDITOR */
	/*MC SPR 1488, got rid of hardcoded editor dimensions*/
	    data->editor_info.editor_attr.win.px   = 0;
	    data->editor_info.editor_attr.win.py   = Mmi_layout_line(2);
	    data->editor_info.editor_attr.win.sx   =SCREEN_SIZE_X;
	    data->editor_info.editor_attr.win.sy   =  2*Mmi_layout_line_height();
		data->editor_info.editor_attr.edtCol	= COLOUR_EDITOR_XX;
	    data->editor_info.editor_attr.font     = 0;
	    data->editor_info.editor_attr.mode     = edtCurBar1;
	    data->editor_info.editor_attr.controls = 0;
	    data->editor_info.editor_attr.text     = data->CB_EDT_MIDEDT_param.name;  /* buffer to be edited */
	    data->editor_info.editor_attr.size     = CB_TAG_LENGTH;
	    data->editor_info.LeftSoftKey       = TxtSoftOK;	    
	    data->editor_info.RightSoftKey      = TxtDelete;
	    data->editor_info.TextId            = TxtEnterName;
	    data->editor_info.Identifier        = value;  /* optional */
	    data->editor_info.mode              = ALPHA_MODE;
	    data->editor_info.Callback          = (T_EDIT_CB)smscb_CB_EDT_TEXTEDT_editor_cb;
		data->editor_info.destroyEditor		= TRUE;
	    data->editor_info.hide                 = FALSE;
		// Change by Sasken ( Deepa M.d) on March 29th 2004
		// Issue Number : MMI-SPR-12722
		// Subject: Right softkey should be "Back" when the editor is empty
		// Bug : Right softkey should be "Back" when the editor is empty
		// Solution : Added alternative string for the right softkey tobe displayed when the editor is empty
		data->editor_info.AlternateLeftSoftKey = TxtSoftBack;
	    data->editor_info.TextString           = NULL;
	    data->editor_info.min_enter            = 0;
	    data->editor_info.timeout              = TEN_SECS;

	    /* create the dialog handler */
	    data->editor = editor_start(data->win,&data->editor_info);  /* start the editor */
#endif /* NEW_EDITOR */

	    break;
	case E_ABORT:
	case E_EXIT: /* defaultreaction */
	    smscb_CB_EDT_TEXTEDT_destroy(data->win);
		/*SPR 1920, removed sending of exit event to parent, no longer needed*/
	    break;
	default:
	    return;
	}
}

/*******************************************************************************

 $Function:    	smscb_CB_EDT_TEXTEDT_editor_cb

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void smscb_CB_EDT_TEXTEDT_editor_cb (T_MFW_HND win, USHORT Identifier, USHORT event)
    /* smscb_CB_EDT_TEXTEDT editor event handler */
{
    T_MFW_WIN    * win_data = ((T_MFW_HDR *)win)->data;
    T_CB_EDT_TEXTEDT * data = (T_CB_EDT_TEXTEDT *)win_data->user;
    T_MFW_HND    * l_parent;
    SHORT          l_id;

    TRACE_FUNCTION ("smscb_CB_EDT_TEXTEDT_editor_cb()");

    switch(event)
    	{
	case INFO_KCD_LEFT:
	    //on OK: menucreate CB_EDT_MIDEDT(data)
	    smscb_CB_EDT_MIDEDT_start (win, &data->CB_EDT_MIDEDT_param);
	    break;
	case INFO_KCD_RIGHT:
	case INFO_KCD_HUP: /* defaultreaction */
	    l_parent = data->parent;
	    l_id = data->id;
	    smscb_CB_EDT_TEXTEDT_destroy(data->win);
	    SEND_EVENT (l_parent, E_RETURN, l_id, NULL); // c007 rsa
	    break;
	default:
	    break;
    	}
    return;
}
/*********************************************************************
**********************************************************************

			CB_EDT_MIDEDT	 DYNAMIC MENU WINDOW. DECLARATION

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

    T_MFW_HND       editor;

    /* internal data */
    T_CB_EDT_MIDEDT_param * param; /* pointer to parameter space in parent (buffer for editing) */
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_info;	/* SPR#1428 - SH - New Editor data */
#else
    T_EDITOR_DATA   editor_info; /* storage for editor attributes */
#endif
} T_CB_EDT_MIDEDT;

static T_MFW_HND smscb_CB_EDT_MIDEDT_create (T_MFW_HND parent);
static void smscb_CB_EDT_MIDEDT_destroy (T_MFW_HND window);
static void smscb_CB_EDT_MIDEDT_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static void smscb_CB_EDT_MIDEDT_editor_cb (T_MFW_HND win, USHORT Identifier, USHORT event);

/*********************************************************************
**********************************************************************

			CB_EDT_MIDEDT	DYNAMIC MENU WINDOW. IMPLEMENTATION

*********************************************************************
**********************************************************************/

T_MFW_HND smscb_CB_EDT_MIDEDT_start(T_MFW_HND parent_window, T_CB_EDT_MIDEDT_param * parameter)
{
    T_MFW_HND win;

    TRACE_FUNCTION ("smscb_CB_EDT_MIDEDT_start()");

    win = smscb_CB_EDT_MIDEDT_create (parent_window);

    if (win NEQ NULL)
	{
	    SEND_EVENT (win, E_INIT, smscb_CB_EDT_MIDEDT_ID, (void *)parameter);
	}
    return win;
}

/*******************************************************************************

 $Function:    	smscb_CB_EDT_MIDEDT_create

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static T_MFW_HND smscb_CB_EDT_MIDEDT_create(MfwHnd parent_window)
{
    T_CB_EDT_MIDEDT      * data = (T_CB_EDT_MIDEDT *)ALLOC_MEMORY (sizeof (T_CB_EDT_MIDEDT));
    T_MFW_WIN  * win;

    TRACE_FUNCTION ("smscb_CB_EDT_MIDEDT_create()");

    /*
     * Create window handler
     */

    data->win = win_create (parent_window, 0, 0, NULL);
    
    if (data->win EQ NULL)
	{
	    return NULL;
	}

    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)smscb_CB_EDT_MIDEDT_exec_cb;
    data->mmi_control.data   = data;
    win                      = ((T_MFW_HDR *)data->win)->data;
    win->user                = (void *)data;
    data->parent             = parent_window;

    /*
     * return window handle
     */

    winShow(data->win);
    return data->win;
}


/*******************************************************************************

 $Function:    	smscb_CB_EDT_MIDEDT_destroy

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void smscb_CB_EDT_MIDEDT_destroy(MfwHnd own_window)
{
    T_MFW_WIN           * win_data;
    T_CB_EDT_MIDEDT     * data;


    TRACE_FUNCTION ("smscb_CB_EDT_MIDEDT_destroy()");

    if (own_window)
	{
	    win_data = ((T_MFW_HDR *)own_window)->data;
	    data     = (T_CB_EDT_MIDEDT *)win_data->user;

	    if (data)
		{
		    /*
		     * Delete WIN handler
		     */ 
		    win_delete (data->win);

		    /*     
		     * Free Memory
		     */
		    FREE_MEMORY ((void *)data, sizeof (T_CB_EDT_MIDEDT));
		}
	    else
		{
		    TRACE_EVENT ("smscb_CB_EDT_MIDEDT_destroy() called twice");
		}
	}
}


/*******************************************************************************

 $Function:    	smscb_CB_EDT_MIDEDT_exec_cb

 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void smscb_CB_EDT_MIDEDT_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
    /* callback handler for events sent to to trigger execution */
{

    T_MFW_WIN          * win_data = ((T_MFW_HDR *) win)->data;
    T_CB_EDT_MIDEDT    * data     = (T_CB_EDT_MIDEDT *)win_data->user;
    T_MFW_HND          * l_parent;
    SHORT                l_id;
    int                  index;

    TRACE_FUNCTION ("smscb_CB_EDT_MIDEDT_exec_cb()");

    switch (event)
	{
	case E_INIT:
	    /* initialization of administrative data */
	    data->id = value;
	    data->param = (T_CB_EDT_MIDEDT_param *)parameter; /* store the pointer to the parameter for later use */

	    /* initialization of the dialog data */
	    index = data->param->index;
	    if (g_smscb_data.CBTopics.entry[index].msg_id NEQ NO_MID)
		{
		    data->param->number[0] = '\0'; /* provide empty entry */

		}
	    else
		{
		    data->param->number[0] = '\0'; /* provide empty entry */
		}
	    //on INIT: call editor (Type=numeric, Length=3, Prompt="Enter MID Number", 
	    //                      Number=data->List.Number[data->ListIndex], SoftLeft=OK, SoftRight=BACK)

		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		AUI_edit_SetDefault(&data->editor_info);
		AUI_edit_SetDisplay(&data->editor_info, ZONE_FULL_SK_TITLE, COLOUR_EDITOR, EDITOR_FONT);
		AUI_edit_SetEvents(&data->editor_info, value, TRUE, TEN_SECS,  (T_AUI_EDIT_CB)smscb_CB_EDT_MIDEDT_editor_cb);
		AUI_edit_SetTextStr(&data->editor_info, TxtSoftOK, TxtDelete, TxtEnterMid, NULL);
		AUI_edit_SetAltTextStr(&data->editor_info, 1, TxtNull, TRUE, TxtSoftBack);
		AUI_edit_SetBuffer(&data->editor_info, ATB_DCS_ASCII, (UBYTE *)data->param->number, CB_MID_LENGTH);
		AUI_edit_SetMode(&data->editor_info, 0, ED_CURSOR_BAR);

	    data->editor = AUI_edit_Start(data->win,&data->editor_info);  /* start the editor */
#else /* NEW_EDITOR */
		editor_attr_init(&data->editor_info.editor_attr, ZONE_SMALL_EDITOR, edtCurBar1, 0, data->param->number, CB_MID_LENGTH, COLOUR_EDITOR );
	    editor_data_init(&data->editor_info,(T_EDIT_CB)smscb_CB_EDT_MIDEDT_editor_cb,TxtSoftOK,TxtDelete,TxtEnterMid,1 ,DIGITS_MODE, TEN_SECS);
		data->editor_info.destroyEditor		= TRUE;
		data->editor_info.Identifier		= value;
	    data->editor_info.hide                 = FALSE;
	    data->editor_info.TextString           = NULL;

	    /* create the dialog handler */
	    data->editor = editor_start(data->win,&data->editor_info);  /* start the editor */ 
#endif /* NEW_EDITOR */
	    break;
	case E_ABORT:
	case E_EXIT: /* defaultreaction */
	    l_parent = data->parent;
	    l_id = data->id;
	    smscb_CB_EDT_MIDEDT_destroy(data->win);
        SEND_EVENT (l_parent, event, l_id, NULL); /* forward event to parent */
	    break;
	default:
	    return;
	}
}


/*******************************************************************************

 $Function:    	smscb_CB_EDT_MIDEDT_editor_cb

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void smscb_CB_EDT_MIDEDT_editor_cb (T_MFW_HND win, USHORT Identifier, USHORT event)
    /* smscb_CB_EDT_MIDEDT editor event handler */
{
    T_MFW_WIN       * win_data = ((T_MFW_HDR *)win)->data;
    T_CB_EDT_MIDEDT * data = (T_CB_EDT_MIDEDT *)win_data->user;
    T_MFW_HND       * l_parent;
    SHORT             l_id;
    int               index;
	UBYTE			ll;
    T_SMSCB_TOPIC_TAG ffs_tag;/*SPR1920*/
    char file_name[20];
    int val;
#ifdef DEBUG_SMS_CB
	UBYTE ii;
#endif

    TRACE_FUNCTION ("smscb_CB_EDT_MIDEDT_editor_cb()");

    l_parent = data->parent;
    l_id     = data->id;

    switch(event)
    	{
	case INFO_KCD_LEFT:
	    index = data->param->index;

		//nm "-1"the zero is counted as well
	    if (index < MAX_MIDS -1 )
		{
		    /* add a new entry */


		    memcpy (g_smscb_data.CBTopics.entry[index].name, data->param->name, CB_TAG_LENGTH);
		    g_smscb_data.CBTopics.entry[index].msg_id = atoi(data->param->number);
			/*SPR1959, save name to FFS*/
		      memcpy(ffs_tag.name, data->param->name, CB_TAG_LENGTH);
		    {	 val = ffs_fwrite((const char*)ffs_cb_tag_name(g_smscb_data.CBTopics.entry[index].msg_id, file_name),  (void*)&ffs_tag, sizeof(T_SMSCB_TOPIC_TAG));

				TRACE_EVENT_P1("CB tag name write status:%d", val);
 			}
			
#ifdef DEBUG_SMS_CB
			/***************************Go-lite Optimization changes Start***********************/
			//	Aug 16, 2004    REF: CRR 24323   Deepa M.D		
			TRACE_EVENT_P1 ( "the index is %d", index);
			/***************************Go-lite Optimization changes end***********************/
#endif
			
			/* add a new entry */
			index += 1;
			g_smscb_data.CBTopics.length = index;
			
			smscb_setCBsettings(SMSCB_SWITCH_ON); /* save in SIM */ 
			
			info_screen(win, TxtSaved ,TxtNull, NULL); 
			
		}
		else
		{
			info_screen (win, TxtFull, TxtNull, NULL);
		}
		
		//  Apr 29, 2004    REF: CRR 11630  Deepa M.D
		//	Refresh the list,so that if a name exists for a channelId in FFS, it is displayed.
		for(ll = 0; ll < g_smscb_data.CBTopics.length; ll++)
		{
			if (g_smscb_data.CBTopics.entry[ll].msg_id EQ NO_MID)
			{
				break;
			}
			
			/* Set default tag string, until we get the saved name from PCM */
			g_smscb_data.CBTopics.entry[ll].name[0] = '\0'; 
			/*attempt to read channel name from FFS file*/
			val = ffs_fread(ffs_cb_tag_name(g_smscb_data.CBTopics.entry[ll].msg_id, file_name),  &ffs_tag, sizeof(T_SMSCB_TOPIC_TAG));
			/*if reading successful, copy name to data structure*/
			
			if (val == sizeof(ffs_tag))
			{
				memcpy(g_smscb_data.CBTopics.entry[ll].name, ffs_tag.name, CB_TAG_LENGTH);
			}
			/*  update here g_smscb_data.CBTopics.entry[i].name from PCM */
			if (g_smscb_data.CBTopics.entry[ll].name[0] EQ '\0')
			{
				/* but if it's empty substitute it by the channel number */
				sprintf (g_smscb_data.CBTopics.entry[ll].name,"Ch%d,ID:%d",ll,g_smscb_data.CBTopics.entry[ll].msg_id);
			}
			
		}
		
		
#ifdef DEBUG_SMS_CB
		/***************************Go-lite Optimization changes Start***********************/
		//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
		TRACE_EVENT ("after add a new one !"); 
		for (ii = 0; ii < MAX_MIDS; ii++)
		{
			
			TRACE_EVENT_P2 (" 04 midlist:%d, i:%d", g_smscb_data.CBTopics.entry[ii].msg_id,ii);
		}
		
		TRACE_EVENT_P1 ("04 midlist:%d", g_smscb_data.CBTopics.length); 
		
		/***************************Go-lite Optimization changes end***********************/
#endif



    
	    smscb_CB_EDT_MIDEDT_destroy(data->win);
	    SEND_EVENT (l_parent, E_EXIT, l_id, NULL);
	    break;
	case INFO_KCD_RIGHT:
	case INFO_KCD_HUP: /* defaultreaction */
	    smscb_CB_EDT_MIDEDT_destroy(data->win);
	    SEND_EVENT (l_parent, E_RETURN, l_id, NULL); // c007 rsa
	    break;
	default:
	    break;
    	}
    return;
}

/*********************************************************************
**********************************************************************


			cell broadcast routines IMPLEMENTATION

*********************************************************************
**********************************************************************/


/*******************************************************************************

 $Function:    	smscb_InitData

 $Description:	Initialises CB message storage

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
void smscb_InitData (void)
{
    UBYTE i;

    g_smscb_data.CBTopics.length = 0;
    for (i = 0; i < MAX_MIDS; i++)
	{
	    g_smscb_data.CBTopics.entry[i].msg_id = NO_MID; /* Set MID list default values. */
	    g_smscb_data.CBTopics.entry[i].name[0] = '\0';
	    g_smscb_data.CBTopics.entry[i].dcs = 0;
	}

}

/*******************************************************************************

 $Function:    	smscb_delete_topic



 $Description: : This function is used to delete an MID from the
            current list <g_smscb_data.CBTopics>.


 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static T_MFW smscb_delete_topic(UBYTE MidOffset)
{
    SHORT Mid;
    UBYTE MidCount;
    UBYTE i;

#ifdef DEBUG_SMS_CB
    UBYTE ii;
#endif

    Mid = g_smscb_data.CBTopics.entry[MidOffset].msg_id;

    /* Find out how many messages are stored. 
     * set MidCount to first entry which contains NO_MID or 
     * set MidCount to MAX_MIDS if list is full
     */
	// 
    MidCount = g_smscb_data.CBTopics.length + 1;

    /* Move the MID data up one. */
    while (MidOffset < (MidCount)) //c005 rsa avoid copying from behind the array
	{
	    g_smscb_data.CBTopics.entry[MidOffset] = g_smscb_data.CBTopics.entry[MidOffset + 1];  
	    MidOffset++;
	}
    //c005 rsa but assign NO_MID behind the last entry
    if (MidCount > 0)
        g_smscb_data.CBTopics.entry[MidCount].msg_id = NO_MID; 
    else
        g_smscb_data.CBTopics.entry[0].msg_id = NO_MID;
    //end c005 rsa
    
    g_smscb_data.CBTopics.length--;

    /* Delete any message in FIFO bearing the MID. */
    /*MC SPR 1920, I have a suspicion this does nothing, but have replaced references
    to removed structure members with references to data stored for topics list*/
    i = 0;
    while ((i < MAX_MIDS) && (g_smscb_data.CBTopics.entry[i].msg_id != Mid))
        i++;


#ifdef DEBUG_SMS_CB
	/***************************Go-lite Optimization changes Start***********************/
	//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
    TRACE_EVENT_P1 ("i is %d", i);
	/***************************Go-lite Optimization changes end***********************/
#endif
	
	
#ifdef DEBUG_SMS_CB
	/***************************Go-lite Optimization changes Start***********************/
	//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	   TRACE_EVENT ("after deleting !"); 
	   for (ii = 0; ii < MAX_MIDS; ii++)
	   {
		   
		   TRACE_EVENT_P2 (" 03 midlist:%d, i:%d", g_smscb_data.CBTopics.entry[ii].msg_id,ii);
	   }
	   
	   TRACE_EVENT_P1 ("03 midlist:%d", g_smscb_data.CBTopics.length); 
	   /***************************Go-lite Optimization changes end***********************/
#endif



    return 1;
}

/*******************************************************************************

 $Function:    	ffs_cb_tag_name

 $Description: generates a filename string for the pathname of an FFS CB Topic tag file
i.e./MMI/cbtagXX where XX is the channel number.
Added for issue1920

 $Returns:	filename string	

 $Arguments: channel number, pointer to allocated string	
 
*******************************************************************************/
char* ffs_cb_tag_name(int channel_num, char* name)
{
	sprintf(name,"/mmi/cbtag%d",channel_num);
	return name;
}

/*******************************************************************************

 $Function:    	smscb_getCBsettings

 $Description: This function allows the MS-User to examine the
            current list of MIDs being used to filter CB messages.


 $Returns:		

 $Arguments:	
 
*******************************************************************************/
T_MFW smscb_getCBsettings(void)
{
    T_MFW status;
    UBYTE i,d=0;
    /*MC SPR 1920, need to allocate this as large data structure*/
	T_MFW_SMS_CB_INFO*   SmsCbInfo = (T_MFW_SMS_CB_INFO*)ALLOC_MEMORY(sizeof(T_MFW_SMS_CB_INFO));
    T_SMSCB_TOPIC_TAG ffs_tag;/*SPR1920*/
    char file_name[20];/*SPR1920*/
    int val;/*SPR1920*/

#ifdef FF_2TO1_PS
    return MFW_SMS_FAIL;
#endif

#ifdef DEBUG_SMS_CB  
    TRACE_EVENT (" get the list from sim");
#endif

	//clear the Topic list 
	smscb_InitData();

	//get the Topic list from the SIM
    status = sms_get_cell_broadcast_info(SmsCbInfo);    

	//set to default
	g_smscb_data.CBTopics.length = 0xFF;

	
#ifdef DEBUG_SMS_CB
	/***************************Go-lite Optimization changes Start***********************/
	//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
    for (i = 0; i <= 19; i++)
	{
		TRACE_EVENT_P2("DATA from mfw %d i:%d", SmsCbInfo->msg_id[i],i);
	}
	/***************************Go-lite Optimization changes end***********************/
#endif


    for (i = 0; i < MAX_MIDS; i++)
	{
	    /* Update MID list from SIM data. */
	    g_smscb_data.CBTopics.entry[i].msg_id = SmsCbInfo->msg_id[d];
	    
	    g_smscb_data.CBTopics.entry[i].dcs = SmsCbInfo->dcs[d]; 

	    if (g_smscb_data.CBTopics.entry[i].msg_id EQ NO_MID)
		{
		    break;
		}
	    else
		{
		    g_smscb_data.CBTopics.length = i; 
		}

	    /* Set default tag string, until we get the saved name form PCM */
	    g_smscb_data.CBTopics.entry[i].name[0] = '\0'; 
		/*SPR1920*/
		/*attempt to read channel name from FFS file*/
 		 val = ffs_fread(ffs_cb_tag_name(g_smscb_data.CBTopics.entry[i].msg_id, file_name),  &ffs_tag, sizeof(T_SMSCB_TOPIC_TAG));
		/*if reading successful, copy name to data structure*/
	    if (val == sizeof(ffs_tag))
	    {
	        memcpy(g_smscb_data.CBTopics.entry[i].name, ffs_tag.name, CB_TAG_LENGTH);
	    }
 		/*SPR1920, end*/
	    /* ??? update here g_smscb_data.CBTopics.entry[i].name from PCM */
	    if (g_smscb_data.CBTopics.entry[i].name[0] EQ '\0')
		{
		    /* but if it's empty substitute it by the channel number */
		    sprintf (g_smscb_data.CBTopics.entry[i].name,"Ch%d,ID:%d",i,g_smscb_data.CBTopics.entry[i].msg_id);
		}
		//x0pleela 16 Feb, 2007  DR: OMAPS00116175
		//Changed the incremental value from 2 to 1 as the successive CB parameters ( message id) was getting skipped while reading back 
		//from cell broadcast parameters 
		d++;

#ifdef DEBUG_SMS_CB
		/***************************Go-lite Optimization changes Start***********************/
		//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
		TRACE_EVENT_P2 ( "02 midlist:%d, i:%d", g_smscb_data.CBTopics.entry[i].msg_id,i);
		/***************************Go-lite Optimization changes end***********************/
#endif   

	}


	if (g_smscb_data.CBTopics.length EQ 0xFF)
	{
		//list is empty
		g_smscb_data.CBTopics.length = 0;
	}
	else
	{
		//+1 because the "0" was counted as well
		g_smscb_data.CBTopics.length +=1;	
	}



	/*SPR 1920, end of function, deallocate memory*/
	FREE_MEMORY((void*)SmsCbInfo,sizeof(T_MFW_SMS_CB_INFO));
    return status;
}

/*******************************************************************************

 $Function:    	smscb_setCBsettings

 $Description: This function is used to activated/de-activate the CB
            feature and to submit MIDs for filter list control.

            When the CB feature is switched ON, the stored message
            index MIDs are set to invalid, i.e ready for incoming CB
            messages and the MID list is initialised. On switch OFF,
            the MID list parameters are refreshed before the switch
            OFF command is issued, to ensure that the most recent
            MID list is used. It is not necessary to handle the
            stored message index MIDs as they will be dealt with
            next time the feature is activated.
	    usage:
                 Status = smscb_setCBsettings(SMSCB_SWITCH_OFF);
                 Status = smscb_setCBsettings(SMSCB_SWITCH_ON);
 

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
T_MFW smscb_setCBsettings(UBYTE State)
{
/* SPR759 - SH - removed 'static' so function can be called by Homezone */

    UBYTE i,d=0;
    T_MFW Status;
    /*MC SPR 1920, need to allocate this as large data structure*/
    T_MFW_SMS_CB_INFO*   SmsCbInfo = (T_MFW_SMS_CB_INFO*)ALLOC_MEMORY(sizeof(T_MFW_SMS_CB_INFO));
    
#ifndef NEPTUNE_BOARD
/* June 23, 2006 Ref: OMAPS00079445 */
    #ifdef FF_2TO1_PS
        return MFW_SMS_FAIL;
    #endif
#endif    

/* SH - moved up here otherwise does not execute*/
//nm 
	if (State EQ SMSCB_SWITCH_ON )
	            sms_set_mt_ind(MT_CB_MSG);
	else
		sms_set_mt_ind(MT_CB_NO_IDX);


    /* copy local list to SIM */
   SmsCbInfo->mode = State;


	for (i = 0; i < MAX_MIDS; i++)
	{
	    SmsCbInfo->msg_id[i] = NO_MID; /* Set MID list default values. */
	    SmsCbInfo->dcs[i]    = 0xFF;
	}

//    Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
//    Description: MFW memory size variable changed from U16 -> U32
//    Solution: 'd' is checked against 'MAX_IDENTS' to restrict increment beyond limit
    for (i = 0; (i < g_smscb_data.CBTopics.length) && (d < MAX_IDENTS); i++)
	{

	    SmsCbInfo->msg_id[d] = g_smscb_data.CBTopics.entry[i].msg_id;
	    /* ??? update here PCM with g_smscb_data.CBTopics.entry[i].name */
	   SmsCbInfo->dcs[d] = (char)g_smscb_data.CBTopics.entry[i].dcs;

		d++;

//    Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
//    Description: MFW memory size variable changed from U16 -> U32
//    Solution: Duplication removed
		
//	   SmsCbInfo->msg_id[d] = g_smscb_data.CBTopics.entry[i].msg_id;
//	    /* ??? update here PCM with g_smscb_data.CBTopics.entry[i].name */
//	   SmsCbInfo->dcs[d] = (char)g_smscb_data.CBTopics.entry[i].dcs;

//	   d++;

	    
	}


#ifdef DEBUG_SMS_CB
	/***************************Go-lite Optimization changes Start***********************/
	//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
    for (i = 0; i <= 25; i++)
	{
		TRACE_EVENT_P2 ("SAVING..SmsCbInfo.msg_id[i] %d  i:%d", SmsCbInfo->msg_id[i],i);
	}
	/***************************Go-lite Optimization changes end***********************/
#endif

	Status = sms_set_cell_broadcast_info(State, SmsCbInfo);

#ifdef DEBUG_SMS_CB
	/***************************Go-lite Optimization changes Start***********************/
	//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
    TRACE_EVENT_P1 ("SAVING.Status is %d", Status);
	/***************************Go-lite Optimization changes end***********************/
#endif
		/*SPR 1920, end of function, deallocate memory*/
		FREE_MEMORY((void*)SmsCbInfo,sizeof(T_MFW_SMS_CB_INFO));
    return Status;
}


/*******************************************************************************

 $Function:    	smscb_get_new_cb_messages_count

 $Description:	Calls from the MainMenu

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
SHORT smscb_get_new_cb_messages_count(void)
{
    int j;
    int count;

    count = 0;
    /*SPR 1920, use cb_attributes list as smsCBList removed*/
    for (j=0; j<MAX_MIDS; j++)
	{
	    if (smscb_data.cb_attributes[j].status== UNREAD_MESSAGE)
		{
		    count++; /* found an unread message */
		}
	}
    return count;
}

/*******************************************************************************

 $Function:    	msgsExtractPhoneNumber

 $Description:	 This function extracts a phone number within quotes, which
            is embedded in a text message.

 $Returns:		
 $Arguments:	
 
*******************************************************************************/
void msgsExtractPhoneNumber(UBYTE *InputString, UBYTE *Buffer)
{
    UBYTE i;
    CHAR * FirstQuote;
    CHAR * SecondQuote;

    FirstQuote = strchr((const char *)InputString, (int)"\"");

    if (FirstQuote != NULL) /* x0039928 - Lint warning removal */
    {
        SecondQuote = strchr((const char *)FirstQuote++, (int)"\"");

        if (SecondQuote != NULL) /* x0039928 - Lint warning removal */
        {
            for (i = 0; FirstQuote != SecondQuote; i++)
                Buffer[i] = FirstQuote[i];
        }

        if (ss_check_ss_string((UBYTE *)Buffer) != MFW_SS_DIAL)
            Buffer[0] = '\0';
    }
}
#ifdef DEBUG_SMS_CB
void handle_dummy_message(T_MFW_SMS_CB* Message)
{
		USHORT i;
		SHORT geograhical_scope = (SHORT)0xC000;
		char temp_buffer[MAX_CBMSG_LEN];

	    TRACE_EVENT ("E_SMS_CB");

		memset(temp_buffer, '\0', MAX_CBMSG_LEN);

		//check: does the incoming CB indicates a immediate scope
		if (! (Message->sn & geograhical_scope) )
	     {
		 //Immediate scope
		 // API - 859 - German characters were not visable in the CB message
		 TRACE_EVENT("Immediate Scope");


	     	
			 for(i=0;i <= Message->msg_len;i++)
			 {
				//check now for the termination "0x00D"
				//the message is NOT terminated with '\0'
				if (Message->cb_msg[i] EQ 0x0d)
				{
					Message->cb_msg[i]='\0';
				 }
			 }
		 // API - 859 - Convert the the incoming GSM Text to ASCII
		 ATB_convert_String(Message->cb_msg, MFW_DCS_8bits, Message->msg_len,
								  temp_buffer, MFW_ASCII, MAX_CBMSG_LEN, FALSE);
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

			{
				Message->cb_msg[MAX_CBMSG_LEN-1]='\0';
			}
			ATB_convert_String(Message->cb_msg, MFW_DCS_8bits, Message->msg_len,
					temp_buffer, MFW_ASCII, /*MAX_MSG_LEN*/MAX_CBMSG_LEN, FALSE);
			memcpy(Message->cb_msg, temp_buffer, MAX_CBMSG_LEN);
			//indicate on the idle screen the new incoming cb
		    addCBCH( NULL, IdleNewCBNormalMessage );
			statusCBCH(TRUE);
			//save the incoming cb messages in the fifo
			sms_new_incoming_cb (Message);
			
		}

	}
/*******************************************************************************

 $Function:    	test_function_to_send_cb

 $Description:	This function is only for simulation of incoming cb messages

 $Returns:		
 $Arguments:	
 
*******************************************************************************/


void test_function_to_send_cb (void)
{
	T_MFW_SMS_CB Message;
	UBYTE i;

	
	DEBUG = FALSE;

// for testing only
//#if 0
TRACE_EVENT("Testcase");
Message.sn = 60;
Message.msg_id = 60;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, "60 Toronto Blue Jays to a 4-0 victory Tuesday ");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);



Message.sn = 61;
Message.msg_id = 61;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, "61 Toronto Blue Jays to a 4-0 victory Tuesday");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);



Message.sn = 62;
Message.msg_id = 61;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, "61/2 Toronto Blue Jays to a 4-0 victory Tuesday");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 55;
Message.msg_id = 55;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, " 55 Toronto Blue Jays to a 4-0 victory Tuesday Multipage ");
Message.msg_len= strlen(Message.cb_msg);
Message.dcs = MFW_DCS_8bits;
handle_dummy_message (&Message);



Message.sn = /*56*/0xC038;
Message.msg_id = 55;
Message.page = 1;
Message.pages = 2;
strcpy (Message.cb_msg, "55/2 1/2  Toronto Blue Jays to a 4-0 victory Tuesday Multipage");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);



Message.sn = /*56*/0xC038;
Message.msg_id = 56;
Message.page = 2;
Message.pages = 2;
strcpy (Message.cb_msg, "56 2/2 Toronto Blue Jays to a 4-0 victory Tuesday Multipage");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 58;
Message.msg_id = 58;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, "58 Toronto Blue Jays to a 4-0 victory Tuesday Multipage ");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = /*56*/0xC038;
Message.msg_id = 56;
Message.page = 2;
Message.pages = 2;
strcpy (Message.cb_msg, "56 2/2 Toronto Blue Jays to a 4-0 victory Tuesday Multipage");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 45;
Message.msg_id = 45;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, "45 Toronto Blue Jays to a 4-0 victory Tuesday Multipage ");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

//#if 0

Message.sn = 16384;
Message.msg_id = 59;
Message.page = 1;
Message.pages = 15;
strcpy (Message.cb_msg, "58 1/15 Toronto Blue Jays to a 4-0 victory Tuesday Multipage***");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);



Message.sn = 16384;
Message.msg_id = 59;
Message.page = 2;
Message.pages = 15;
strcpy (Message.cb_msg, "59 2/15 Toronto Blue Jays to a 4-0 victory Tuesday Multipage ***");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 16384;
Message.msg_id = 59;
Message.page = 3;
Message.pages = 15;
strcpy (Message.cb_msg, "59 3/15 Toronto Blue Jays to a 4-0 victory Tuesday Multipage***");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);
Message.sn = 16384;
Message.msg_id = 59;
Message.page = 4;
Message.pages = 15;
strcpy (Message.cb_msg, "59 4/15 Toronto Blue Jays to a 4-0 victory Tuesday Multipage***");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);



Message.sn = 16384;
Message.msg_id = 59;
Message.page = 5;
Message.pages = 15;
strcpy (Message.cb_msg, "59 5/15 Toronto Blue Jays to a 4-0 victory Tuesday Multipage ***");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 16384;
Message.msg_id = 59;
Message.page = 6;
Message.pages = 15;
strcpy (Message.cb_msg, "59 6/15 Toronto Blue Jays to a 4-0 victory Tuesday Multipage ***");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);
Message.sn = 16384;
Message.msg_id = 59;
Message.page = 7;
Message.pages = 15;
strcpy (Message.cb_msg, "59 7/15 Toronto Blue Jays to a 4-0 victory Tuesday Multipage***");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);



Message.sn = 16384;
Message.msg_id = 59;
Message.page = 8;
Message.pages = 15;
strcpy (Message.cb_msg, "59 8/15 Toronto Blue Jays to a 4-0 victory Tuesday Multipage ***");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 16384;
Message.msg_id = 59;
Message.page = 9;
Message.pages = 15;
strcpy (Message.cb_msg, "59 9/15 Toronto Blue Jays to a 4-0 victory Tuesday Multipage ***");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);
Message.sn = 16384;
Message.msg_id = 59;
Message.page = 10;
Message.pages = 15;
strcpy (Message.cb_msg, "59 10/15 Toronto Blue Jays to a 4-0 victory Tuesday Multipage ***");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);



Message.sn = 16384;
Message.msg_id = 59;
Message.page = 11;
Message.pages = 15;
strcpy (Message.cb_msg, "59 11/15 Toronto Blue Jays to a 4-0 victory Tuesday Multipage ***");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 16384;
Message.msg_id = 59;
Message.page = 12;
Message.pages = 15;
strcpy (Message.cb_msg, "59 12/15 Toronto Blue Jays to a 4-0 victory Tuesday Multipage ***");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);
Message.sn = 16384;
Message.msg_id = 59;
Message.page = 13;
Message.pages = 15;
strcpy (Message.cb_msg, "59 13/15 Toronto Blue Jays to a 4-0 victory Tuesday Multipage ***");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);



Message.sn = 16384;
Message.msg_id = 59;
Message.page = 14;
Message.pages = 15;
strcpy (Message.cb_msg, "59 14/15 Toronto Blue Jays to a 4-0 victory Tuesday Multipage***");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 16384;
Message.msg_id = 59;
Message.page = 15;
Message.pages = 15;
strcpy (Message.cb_msg, "59 15/15 Toronto Blue Jays to a 4-0 victory Tuesday Multipage ***");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);
//#endif

#if 0




Message.sn = 63;
Message.msg_id = 63;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, "63 Toronto Blue Jays to a 4-0 victory Tuesday");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);


Message.sn = 64;
Message.msg_id = 64;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, "64 Toronto Blue Jays to a 4-0 victory Tuesday");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 65;
Message.msg_id = 65;
Message.page = 1;
Message.pages = 3;
strcpy (Message.cb_msg, "65 1/3 Toronto Blue Jays to a 4-0 victory Tuesday");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);


Message.sn = 65;
Message.msg_id = 65;
Message.page = 2;
Message.pages = 3;
strcpy (Message.cb_msg, "65 2/3 Toronto Blue Jays to a 4-0 victory Tuesday 65");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);


Message.sn = 65;
Message.msg_id = 65;
Message.page = 3;
Message.pages = 3;
strcpy (Message.cb_msg, "65 3/3 Toronto Blue Jays to a 4-0 victory Tuesday 65");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);


Message.sn = 66;
Message.msg_id = 66;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, "66 Toronto Blue Jays to a 4-0 victory Tuesday");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 67;
Message.msg_id = 67;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, " Toronto Blue Jays to a 4-0 victory Tuesday 67");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 68;
Message.msg_id = 68;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, " Toronto Blue Jays to a 4-0 victory Tuesday 68 ");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);


Message.sn = 72;
Message.msg_id = 72;
Message.page = 1;
Message.pages = 4;
strcpy (Message.cb_msg, " Toronto Blue Jays to a 4-0 victory Tuesday 72:1 ");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 72;
Message.msg_id = 72;
Message.page = 2;
Message.pages = 4;
strcpy (Message.cb_msg, " Toronto Blue Jays to a 4-0 victory Tuesday 72:2 ");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 72;
Message.msg_id = 72;
Message.page = 3;
Message.pages = 4;
strcpy (Message.cb_msg, " Toronto Blue Jays to a 4-0 victory Tuesday 72:3 ");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 72;
Message.msg_id = 72;
Message.page =4;
Message.pages = 4;
strcpy (Message.cb_msg, " Toronto Blue Jays to a 4-0 victory Tuesday 72:4 ");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 73;
Message.msg_id = 73;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, " Toronto Blue Jays to a 4-0 victory Tuesday 73 ");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);


Message.sn = 74;
Message.msg_id = 74;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, " Toronto Blue Jays to a 4-0 victory Tuesday 74");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 75;
Message.msg_id = 75;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, " Toronto Blue Jays to a 4-0 victory Tuesday 75 ");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 77;
Message.msg_id = 77;
Message.page = 1;
Message.pages = 2;
strcpy (Message.cb_msg, "1/2 Toronto Blue Jays to a 4-0 victory Tuesday 77:1");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 77;
Message.msg_id = 77;
Message.page = 2;
Message.pages = 2;
strcpy (Message.cb_msg, "2/2 Toronto Blue Jays to a 4-0 victory Tuesday 77:2 ");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);
//
Message.sn = 78;
Message.msg_id = 78;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, " Toronto Blue Jays to a 4-0 victory Tuesday 78");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 16386;
Message.msg_id = 79;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, " Toronto Blue Jays to a 4-0 victory Tuesday 79 ");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 80;
Message.msg_id = 80;
Message.page = 1;
Message.pages = 3;
strcpy (Message.cb_msg, " Toronto Blue Jays to a 4-0 victory Tuesday 80:1");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);


Message.sn = 80;
Message.msg_id = 80;
Message.page = 3;
Message.pages = 3;
strcpy (Message.cb_msg, "80 2/3 Yoronto Blue Jays to a 4-0 Tuesday:3");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

Message.sn = 80;
Message.msg_id = 80;
Message.page = 2;
Message.pages = 3;
strcpy (Message.cb_msg, "80 1/3 Toronto Blue Jays to Tuesday:2");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);

DEBUG = TRUE;

Message.sn = 83;
Message.msg_id = 83;
Message.page = 1;
Message.pages = 1;
strcpy (Message.cb_msg, " Toronto Blue Jays to a 4-0 victory Tuesday 83");
Message.msg_len= strlen(Message.cb_msg);
handle_dummy_message (&Message);
#endif
}

#endif


/*******************************************************************************

 $Function:    	sms_cb_init

 $Description:	This is initialize the CB fifo and the attributes after switch on the mobile

 $Returns:		
 $Arguments:	
 
*******************************************************************************/
void sms_cb_init (void)
{

//for tracing
#ifdef DEBUG_SMS_CB
	 char buf [100];
#endif 


	UBYTE i;

	
	//set to the default, "FF" -> no CB messages stored
	smscb_data.used_pointer = 0xFF;

	//set to the default, "0" -> free spaces on the first index in fifo
	smscb_data.free_pointer = 0;


	//set the fifo to the default
	for (i=0; i<MAX_STORED_PAGES -1 ; i++)	
	{
		smscb_data.cb_attributes[i].next_link = i + 1;
	}

	for (i=0; i<MAX_STORED_PAGES ; i++)	
	{
		smscb_data.cb_attributes[i].start_page = 0; 
		smscb_data.cb_attributes[i].header = NULL;/* SPR 1920,ensure unalloacted pointers set to NULL*/
	}



	//"FF" on the last entry means end of the free spaces list
	smscb_data.cb_attributes[MAX_STORED_PAGES-1].next_link  = 0xFF;
	smscb_getCBsettings();
	
	if (settingsGetStatus(SettingsCBDisable))
		smscb_setCBsettings(SMSCB_SWITCH_OFF);
	else
		smscb_setCBsettings(SMSCB_SWITCH_ON);
//for tracing





}

/*******************************************************************************

 $Function:    	sms_cb_exit

 $Description:	This deallocates any memory. MC added for SPR 1920

 $Returns:		
 $Arguments:	
 
*******************************************************************************/
void sms_cb_exit (void)
{
	int i;
	/*for each storage space*/
	for (i=0;i<MAX_STORED_PAGES;i++)
	{	/*if memory allocated, deallocate it*/
		if(smscb_data.messages[i] != NULL)
		{	mfwFree((U8*)smscb_data.messages[i], CB_PAGE_LENGTH);
			smscb_data.messages[i] = NULL;
		}
	}
	for(i=0; i<MAX_MIDS;i++)
	{	if (smscb_data.cb_attributes[i].header != NULL)
		{	FREE_MEMORY((void*)smscb_data.cb_attributes[i].header,sizeof(char)*25);
			smscb_data.cb_attributes[i].header = NULL;
		}
	}
}

/*******************************************************************************

 $Function:    	sms_new_incoming_cb

 $Description:	This function will be call after each NEW incoming cb messages.
				It checks for free spaces and organize to save the cb messages
				in the right order.
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void sms_new_incoming_cb (T_MFW_SMS_CB *Message)
{

//only for tracing
#ifdef DEBUG_SMS_CB
	UBYTE i, test;
#endif


	//set to default
	UBYTE result =0;
	smscb_data.found_mid = FALSE;
	smscb_data.clear_mid = FALSE;


//only for tracing
#ifdef DEBUG_SMS_CB
	TRACE_EVENT("--------------start------------");
#endif //DEBUG_SMS_CB


	//search the new MID with the MID's in the fifo.
	//used_pointer EQ 0xFF means there is no message in the fifo
	if (smscb_data.used_pointer NEQ 0xFF)
	{
		result = sms_cb_search_mid (Message->msg_id, Message->sn);
	}


	//error: in searching mid
	if (result EQ 0xAA)
	{
		TRACE_EVENT("sms cb ERROR, in searching mid");
		return;
	}


	// does the message with the same MID already exist in the fifo ?
	if(smscb_data.found_mid)
	{
		TRACE_EVENT("MID already exist");

		sms_cb_delete_message (Message->msg_id);

		
		smscb_data.clear_mid = TRUE;
	}


	// does we have free space ?
	if (smscb_data.free_pointer EQ 0xFF)
	{
		//no free spaces -> means overwrite the oldest message
		sms_cb_overwrite_old_message (Message);
		
	}
	else // does we have free space ?
	{
		//still free spaces 
		sms_cb_store_message (Message);

	}



//only for tracing
#ifdef DEBUG_SMS_CB
	/***************************Go-lite Optimization changes Start***********************/
	//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P1("5 smscb_data.used_pointer %d ", smscb_data.used_pointer); 
	
	TRACE_EVENT_P1("5 smscb_data.free_pointer %d ", smscb_data.free_pointer); 
	/***************************Go-lite Optimization changes end***********************/
	TRACE_EVENT("--------------finish------------");
	
	
#endif //DEBUG_SMS_CB

}



/*******************************************************************************

 $Function:    	sms_cb_store_message

 $Description:	This function will be call when we have still free space for saving
                 new incoming cb messages.
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
UBYTE sms_cb_store_message (T_MFW_SMS_CB *Message)
{

	TRACE_FUNCTION("sms_cb_store_message");


	//it is only for the first time
	if (smscb_data.used_pointer EQ 0xFF)
	{
		// request for a free space
		smscb_data.used_pointer = smscb_data.free_pointer;
	}


	//save the new message in the fifo
	/*MC  SPR 1920 if memory not alloacted for message, allocate some*/
	TRACE_EVENT_P1("Allocating memory to slot %d for CB message", smscb_data.free_pointer); 

	if (smscb_data.messages[smscb_data.free_pointer] == NULL)
		if ((smscb_data.messages[smscb_data.free_pointer] = (char*) mfwAlloc(sizeof(char)*CB_PAGE_LENGTH)) != NULL)
			memcpy(smscb_data.messages[smscb_data.free_pointer], Message->cb_msg,CB_PAGE_LENGTH);

	/*MC end*/
	//save the attributes of the new cb
	sms_cb_store_attributes (smscb_data.free_pointer, Message);

	//set the status of the message
	smscb_data.cb_attributes[smscb_data.free_pointer].status = UNREAD_MESSAGE;

	//was it the last free space ?
	if (smscb_data.cb_attributes[smscb_data.free_pointer].next_link EQ 0xFF)
	{
		//no more free space available

		UBYTE temp, result;
		

		//keep the link-value for later
		temp = smscb_data.used_pointer;

		//find the termination in the link list
		result = sms_cb_find_termination (&smscb_data.used_pointer);			
		
		//error: couldnt find the termination ?!
		if (result EQ 0xAA)
		{
			TRACE_EVENT("sms cb ERROR, couldnt find the termination");
			return 0;   // RAVI
		}


		//update the link-list
		smscb_data.cb_attributes[smscb_data.used_pointer].next_link = smscb_data.free_pointer;

		//no more free space
		smscb_data.free_pointer = 0xFF;

		//update the used pointer
		smscb_data.used_pointer = temp;

		//did it clear before ?
		if (smscb_data.clear_mid)
		{
			smscb_data.used_pointer = smscb_data.result_used;
		}


	}
	else //was it the last free space ?
	{
		//no, still free space available

		UBYTE temp, temp_2, result;

		//keep the usedpointer-value for later
		temp_2 = smscb_data.used_pointer;


		//keep the link-value for later
		temp = smscb_data.cb_attributes[smscb_data.free_pointer].next_link;


		//terminate the used list
		smscb_data.cb_attributes[smscb_data.free_pointer].next_link = 0xFF;


		//find the termination in the link list
		result = sms_cb_find_termination (&smscb_data.used_pointer);			

		//error: couldnt find the termination ?!
		if (result EQ 0xAA)
		{
			TRACE_EVENT("sms cb ERROR, couldnt find the termination");
			return 0;
		}


		//update the use pointer
		if (smscb_data.used_pointer NEQ smscb_data.free_pointer)
		{
			smscb_data.cb_attributes[smscb_data.used_pointer].next_link = smscb_data.free_pointer;
		}

	
		// update the free pointer as well
		smscb_data.free_pointer = temp;


		//did it clear before ?
		if (smscb_data.clear_mid)
		{
			smscb_data.used_pointer = smscb_data.result_used;
		}
		else
		{
			//update the used pointer
			 smscb_data.used_pointer = temp_2 ;
		}
	

	}
	//ADDED BY RAVI- 28-11-2005
	return 1;
	//END RAVI - 28-11-2005
}

/*******************************************************************************

 $Function:    	sms_cb_overwrite_old_message

 $Description:	This function will be call when we don't have free space for saving
                 new incoming cb messages. The new cb just overwrite the 
                 oldest cb in the fifo.

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
UBYTE sms_cb_overwrite_old_message (T_MFW_SMS_CB *Message)
{
//	UBYTE temp, temp_2, result;   // RAVI

//only tracing
#ifdef DEBUG_SMS_CB
	/***************************Go-lite Optimization changes Start***********************/
	//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P1("used_pointer= %d ", smscb_data.used_pointer); 			
	TRACE_EVENT_P1("pages = %d, ", smscb_data.cb_attributes[smscb_data.used_pointer].pages); 			
	TRACE_EVENT_P1("page = %d ", smscb_data.cb_attributes[smscb_data.used_pointer].page); 			
	TRACE_EVENT_P1( "mid = %d ", smscb_data.cb_attributes[smscb_data.used_pointer].mid); 
	/***************************Go-lite Optimization changes end***********************/
#endif //DEBUG_SMS_CB


	TRACE_EVENT("sms_cb_overwrite_old_message");

	//keep the value for later
//	temp = smscb_data.used_pointer;


	//find the page in the fifo with the same MID number & delete them
	sms_cb_delete_message (smscb_data.cb_attributes[smscb_data.used_pointer].mid);


	//now we have free space to store the new incoming cb message	
	sms_cb_store_message (Message);

	return 1;    // ADDED BY RAVI - 28-11-2005

}

/*******************************************************************************

 $Function:    	sms_cb_store_attributes

 $Description:	Store the attributes of the new incoming cb 

				Attributes are stored seperatly from the message text
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

void sms_cb_store_attributes (UBYTE position, T_MFW_SMS_CB *Message)
{
	smscb_data.cb_attributes[position].sn	= Message->sn;
	smscb_data.cb_attributes[position].mid	= Message->msg_id;
	smscb_data.cb_attributes[position].dcs	= Message->dcs;
	smscb_data.cb_attributes[position].page	= Message->page;
	smscb_data.cb_attributes[position].pages      = Message->pages;
	smscb_data.cb_attributes[position].msg_len    = Message->msg_len;	

}


/*******************************************************************************

 $Function:    	sms_cb_search_mid

 $Description:	This function searchs the new incoming message in the 
                fifo for the already existing stored messages.
                If true, the stored messages in the fifo will be removed 

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
UBYTE sms_cb_search_mid (USHORT mid ,SHORT sn)
{
	UBYTE temp, i=0;
	SHORT update_number_mask = 0x0F;

	temp = smscb_data.used_pointer;


	if (smscb_data.cb_attributes[temp].mid EQ mid)
	{
		//found the mid in the fifo
		TRACE_EVENT("Matching MID");
		if ((smscb_data.cb_attributes[temp].sn & update_number_mask) NEQ
			(sn & update_number_mask) )
		{
		   //the new cb is the update of the existing one 
			smscb_data.found_mid = TRUE;
   		   TRACE_EVENT("found = True, return");
			return temp;
		}
		/*SPR 2429, return this index as otherwise there will be two
		copies of the same message*/
		TRACE_EVENT("Serial numbers match");
		return temp;
	}


	while (smscb_data.cb_attributes[temp].next_link NEQ 0xFF)
	{
		
		// going through the link list
		temp = smscb_data.cb_attributes[temp].next_link;


		if (smscb_data.cb_attributes[temp].mid EQ mid)
		{
			//found the mid in the fifo
			TRACE_EVENT_P1("Matching MID at location:%d", temp);
			if ((smscb_data.cb_attributes[temp].sn & update_number_mask) NEQ
				(sn & update_number_mask) )
			{

			   //the new cb is the update of the existing one in the fifo
				smscb_data.found_mid = TRUE;
				return temp;
				
				
			}
		/*SPR 2429, return this index as otherwise there will be two
		copies of the same message*/
			TRACE_EVENT("Serial numbers match");
			return temp;/*SHould just replace old one with new one anyway*/
		}
		

		// only to be sure there is a ever ending loop
		i++;
		if (i > MAX_STORED_PAGES)	
			return 0xAA;
	}
	
	return temp;
}



/*******************************************************************************

 $Function:    	sms_cb_delete_message

 $Description:	This function delete the messages in the fifo, even for multipages
           
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
UBYTE sms_cb_delete_message (USHORT mid)
{

//only tracing
#ifdef DEBUG_SMS_CB
	char buf[100];
#endif 

	UBYTE temp, temp_2, result;   // , i=0;  // RAVI

	TRACE_EVENT("sms_cb_delete_message");


	//keep the value for later
	temp_2 = smscb_data.used_pointer;


	// keep the value for later
	smscb_data.start_used = smscb_data.used_pointer;

	//search for the first page of the multipages
	smscb_data.start_used = sms_cb_find_startpoint (mid, smscb_data.used_pointer);

	//error: couldnt find the termination ?!
	if (smscb_data.start_used EQ 0xAA)
	{
		TRACE_EVENT("sms cb ERROR, couldnt find the termination");
		return 0;  // RAVI
	}


	//found immediately ?
	if (smscb_data.start_used EQ smscb_data.used_pointer)
	{
		//is in the OLDEST position in the fifo

		TRACE_EVENT("message in oldest position in FIFO");		
		
		//keep the value for later
		temp = smscb_data.used_pointer;

		//search for the last page 
		smscb_data.end_used = sms_cb_find_endpoint (mid, smscb_data.used_pointer);


		//error: couldnt find the termination ?!
		if (smscb_data.end_used EQ 0xAA)
		{
			TRACE_EVENT("sms cb ERROR");
			return 0;  // RAVI
		}



		if ( (smscb_data.cb_attributes[smscb_data.used_pointer].next_link EQ 0xFF) &&
				(smscb_data.cb_attributes[smscb_data.used_pointer].mid EQ mid) ) 
		{
			//it is the oldest cb and also the only one in the fifo
			TRACE_EVENT("oldest position and the only one in the list");


			//deleted messages -> now more space availabe -> update the free pointer
			result = sms_cb_update_free_pointer (temp);

			//terminate the free pointer list 
			//smscb_data.cb_attributes[smscb_data.end_used].next_link = 0xFF;

			//error: couldnt find the termination ?!
			if (result EQ 0xAA)
			{
				TRACE_EVENT("sms cb ERROR, couldnt find the termination");
				return 0;
			}

			//no anymore cb message in the fifo
			//terminate the use list
			smscb_data.used_pointer = 0xFF;

			smscb_data.result_used = result;

		}
		else
		{
			//deleted messages -> now more space availabe -> update the free pointer
			result = sms_cb_update_free_pointer (temp);

			//terminate the free pointer list 
			smscb_data.cb_attributes[smscb_data.end_used].next_link = 0xFF;


			//error: couldnt find the termination ?!
			if (result EQ 0xAA)
			{
				TRACE_EVENT("sms cb ERROR, couldnt find the termination");
				return 0;	
			}

		}
	
	
	}
	else //found immediately ?
	{

		//keep the value for later
		temp = smscb_data.used_pointer;

		//search for the last page of the multipages
		smscb_data.end_used = sms_cb_find_endpoint (mid, smscb_data.used_pointer);

		if (smscb_data.start_used EQ 0xAA)
		{
			TRACE_EVENT("sms cb ERROR, couldnt find the termination");
			return 0;
		}


		if (smscb_data.cb_attributes[smscb_data.used_pointer].next_link EQ 0xFF &&
			smscb_data.cb_attributes[smscb_data.used_pointer].mid EQ mid)
		{
			//is in the the latest position in the fifo

			TRACE_EVENT("multipage is in the latest position");
			
			//terminate the position where the multipage began
			smscb_data.cb_attributes[smscb_data.start_used].next_link= 0xFF;


			//deleted messages -> now more space availabe -> update the free pointer
			result = sms_cb_update_free_pointer (temp);

			//terminate the free pointer list 
			smscb_data.cb_attributes[smscb_data.used_pointer].next_link = 0xFF;


			//error: couldnt find the termination ?!
			if (result EQ 0xAA)
			{
				TRACE_EVENT("sms cb ERROR, couldnt find the termination");
				return 0;
			}

			smscb_data.result_used = temp_2;
			smscb_data.used_pointer = temp_2;

			
		}
		else
		{
			//is in the middle position in the fifo

			TRACE_EVENT("multipage is in the middle position");
			
			//update the link list
			smscb_data.cb_attributes[smscb_data.start_used].next_link = smscb_data.used_pointer;		

			//deleted messages -> now more space availabe -> update the free pointer
			result = sms_cb_update_free_pointer (temp);

			//terminate the free pointer list 
			smscb_data.cb_attributes[smscb_data.end_used].next_link = 0xFF;



		


			//error: couldnt find the termination ?!
			if (result EQ 0xAA)
			{
				TRACE_EVENT("sms cb ERROR, couldnt find the termination");
				return 0;
			}


			smscb_data.result_used = temp_2;		

		}
			

	}
       return 1;   // ADDED BY RAVI - 28-11-2005

}

/*******************************************************************************

 $Function:    	sms_cb_find_startpoint

 $Description:	find the startpoint of the multipage. It is also used for singlepage.

           
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
UBYTE sms_cb_find_startpoint (USHORT mid, UBYTE start_used)
{
	UBYTE i = 0;

//only tracing
#ifdef DEBUG_SMS_CB
//	char buf[100];

	TRACE_EVENT("in sms_cb_find_startpoint ");	
#endif //DEBUG_SMS_CB


	//search for the first page of the multipages
	while (smscb_data.cb_attributes[smscb_data.used_pointer].mid NEQ mid)
	{
	
		start_used = smscb_data.used_pointer;

		// going through the link list
		smscb_data.used_pointer = smscb_data.cb_attributes[smscb_data.used_pointer].next_link;
	
			// only to be sure there is a ever ending loop
			i++;
			if (i > MAX_STORED_PAGES)	
				return 0xAA;
	}

//only tracing
TRACE_EVENT_P1("start_used: %d", start_used);

	return start_used;
}

/*******************************************************************************

 $Function:    	sms_cb_find_startpoint

 $Description:	find the lastpage of the multipage.
 				It is also used for the singlepage but in this case the 
 				end_used will be equal to used_pointer
           
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
UBYTE sms_cb_find_endpoint (USHORT mid, UBYTE end_used)
{
	UBYTE i = 0;



	//search for the last page of the multipages
	while (smscb_data.cb_attributes[smscb_data.used_pointer].mid EQ mid &&
			smscb_data.cb_attributes[smscb_data.used_pointer].next_link NEQ 0xFF )
	{
	
		end_used = smscb_data.used_pointer;

		// going through the link list
		smscb_data.used_pointer = smscb_data.cb_attributes[smscb_data.used_pointer].next_link;
	
		// only to be sure there is a ever ending loop
		i++;
		if (i > MAX_STORED_PAGES)	
			return 0xAA;
	}


TRACE_EVENT_P1("end_used: %d", end_used);

	return end_used;
}

/*******************************************************************************

 $Function:    	sms_cb_update_free_pointer

 $Description:	This function is updating the free pointer after deleting 
		 		cb messages in the fifo (space available !)

           
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
UBYTE sms_cb_update_free_pointer (UBYTE temp)
{
	UBYTE update=0, i=0;
      

	TRACE_EVENT("sms_cb_update_free_pointer");


TRACE_EVENT_P1("temp index:%d", temp); 
	if (smscb_data.free_pointer EQ 0xFF)
	{TRACE_EVENT("free pointer = 0xff"); 
		//take the old used_pointer (before of deleting the messages)
		smscb_data.free_pointer = temp;
		/*MC SPR 1920, free the message slot*/
		mfwFree( (U8*)smscb_data.messages[temp], sizeof(char)*CB_PAGE_LENGTH);
		smscb_data.messages[temp] = NULL;
	}
	else
	{
		/*MC SPR 2429 free the message slot here too*/
		mfwFree( (U8*)smscb_data.messages[temp], sizeof(char)*CB_PAGE_LENGTH);
		smscb_data.messages[temp] = NULL;
		
		//
		update = smscb_data.free_pointer;
		TRACE_EVENT_P1("free pointer:%d",smscb_data.free_pointer) ; 
		//
		while (smscb_data.cb_attributes[smscb_data.free_pointer].next_link NEQ 0xFF)
		{

			// going through the link list
			smscb_data.free_pointer = smscb_data.cb_attributes[smscb_data.free_pointer].next_link;
			TRACE_EVENT_P1("free pointer:%d",smscb_data.free_pointer) ; 
		
			// only to be sure there is a ever ending loop
			i++;
			if (i > MAX_STORED_PAGES)	
				return 0xAA;
		}


		//continue the free pointer link list to the new deleted multipage
		smscb_data.cb_attributes[smscb_data.free_pointer].next_link = temp;


		//show on the first deleted multipage 
		smscb_data.free_pointer = update;

		
	}


	//indicate that we deleted cb messages
	smscb_data.clear_mid = TRUE;


	//need for later
	smscb_data.result_used = smscb_data.used_pointer;

TRACE_EVENT_P1("used pointer: %d", smscb_data.used_pointer);
//only for tracing 


	return update;


}


/*******************************************************************************

 $Function:    	sms_cb_find_termination

 $Description:	This function search for the termination in the link list.
 				It jumps from one link to the another link until he founds 
 				the termination "0xFF"
           
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
UBYTE sms_cb_find_termination (UBYTE *used)
{
	UBYTE i=0;
	
	while (smscb_data.cb_attributes[*used].next_link NEQ 0xFF)
	{
		// going through the link list
		*used = smscb_data.cb_attributes[*used].next_link;
	
		// only to be sure there is a ever ending loop
		i++;
		if (i > MAX_STORED_PAGES)	
			return 0xAA;

	}

	return *used;
}


/*******************************************************************************

 $Function:    	sms_cb_count_messages

 $Description:	This function counts messages in the fifo.
 				! Multipage are counted as one page !
           
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
UBYTE sms_cb_count_messages ( void )
{
	UBYTE i=0, count=0, used, page, pages, mid, start_flag = 1; //, temp_used;


	for (i=0; i<MAX_STORED_PAGES ; i++)	
	{
		smscb_data.cb_attributes[i].start_page = 0; 
	}
	i=0;


	if (smscb_data.used_pointer EQ 0xFF)
	{
		//there are no cb messages in the fifo
		return count;
	}
		
	used =	smscb_data.used_pointer;

	while ( i < MAX_STORED_PAGES)
	{

		//single or multipage ?
		if(smscb_data.cb_attributes[used].pages EQ 0x01)
		{
			//Singlepage

			start_flag = 1;
			/***************************Go-lite Optimization changes Start***********************/
			//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
			TRACE_EVENT_P2("11 : used%d, count%d", used,count); 
			
			/***************************Go-lite Optimization changes end***********************/
			//indicate the first page, even for singlepage
			smscb_data.cb_attributes[used].start_page = 0x55;
			
			TRACE_EVENT_P1("1 set Flag at %d", used); 

			//check, is it the last message in the link list
			//if yes, not necessary to go through the link list
			if (smscb_data.cb_attributes[used].next_link NEQ 0xFF)
			{
				count ++;

				// going through the link list
				used = smscb_data.cb_attributes[used].next_link;		
			}

			//is it the last message in the fifo ?
			if (smscb_data.cb_attributes[used].next_link EQ 0xFF)
			{
				/***************************Go-lite Optimization changes Start***********************/
				//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
				TRACE_EVENT_P2("1 : used%d,count%d", used,count); 
				/***************************Go-lite Optimization changes end***********************/

				//indicate the first page, even for singlepage
				smscb_data.cb_attributes[used].start_page = 0x55;
					
				
				TRACE_EVENT_P1("2 set Flag at %d", used); 

				count ++;	
				return count;
			}

			
		}
		else //single or multipage ?
		{

			//Multipage
			if (start_flag)
			{

				//indicate the first page, even for singlepage
				smscb_data.cb_attributes[used].start_page = 0x55;
/***************************Go-lite Optimization changes Start***********************/
//	Aug 16, 2004    REF: CRR 24323   Deepa M.D				
				TRACE_EVENT_P1("3 set Flag at %d", used); 
/***************************Go-lite Optimization changes end***********************/			
			}
				
			
			page = 	smscb_data.cb_attributes[used].page;
			pages = smscb_data.cb_attributes[used].pages;
			mid = smscb_data.cb_attributes[used].mid;

			//check, is it the last message in the link list
			//if yes, not necessary to go through the link list
			if (smscb_data.cb_attributes[used].next_link NEQ 0xFF)
			{
				// going through the link list
				used = smscb_data.cb_attributes[used].next_link;		
			}

			
			//is it the last message in the fifo ?
			if (smscb_data.cb_attributes[used].next_link EQ 0xFF)
			{

				//The last page, does it still continue or does it complete 
				//the multipage ?
				if( smscb_data.cb_attributes[used].pages EQ pages   &&  
				   smscb_data.cb_attributes[used].page EQ (page+1) &&
				   smscb_data.cb_attributes[used].page EQ smscb_data.cb_attributes[used].pages)
				{
					//this one was the last page of the multipage and 
					//the last one in the fifo
					/***************************Go-lite Optimization changes Start***********************/
					//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
					TRACE_EVENT_P2("5 : used%d,count%d", used,count); 
					/***************************Go-lite Optimization changes end***********************/
					start_flag = 0;

					count ++;	
					return count;
				}
				else
				{
					//the multipage is not complete yet

					if (smscb_data.cb_attributes[used].mid EQ mid)
					{

						start_flag = 0;
						/***************************Go-lite Optimization changes Start***********************/
						//	Aug 16, 2004    REF: CRR 24323   Deepa M.D	
						TRACE_EVENT_P2("6 : used%d,count%d", used,count); 
						/***************************Go-lite Optimization changes end***********************/
						count ++;
						
						return count;
					}
					else
					{

						//this is a different page , it means the multipage before
						//is uncomplete !
						
						start_flag = 0;


						//indicate the first page, even for singlepage
						smscb_data.cb_attributes[used].start_page = 0x55;
						/***************************Go-lite Optimization changes Start***********************/
						//	Aug 16, 2004    REF: CRR 24323   Deepa M.D		
						TRACE_EVENT_P1("4 set Flag at %d", used); 
						/***************************Go-lite Optimization changes end***********************/

						//the page before is a uncomplete multipage
						//and the last message is a single page
						// ->count twice 
						count+=2;
						/***************************Go-lite Optimization changes Start***********************/
						//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
						TRACE_EVENT_P2("7 : used%d,count%d", used,count); 
						/***************************Go-lite Optimization changes end***********************/
						return count;

					}
				}
		
			}
			else
			{
				if( smscb_data.cb_attributes[used].pages EQ pages &&  
				   smscb_data.cb_attributes[used].page EQ (page+1))
				{
					//still continue
					/***************************Go-lite Optimization changes Start***********************/
					//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
					
					TRACE_EVENT_P2("3 : used%d,count%d", used,count); 
					/***************************Go-lite Optimization changes end***********************/
					start_flag = 0;
					
				}
				else
				{
					//there is a new page starting

					
					start_flag = 1;

					count ++;	
					/***************************Go-lite Optimization changes Start***********************/
					//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
					
					TRACE_EVENT_P2("4 : used%d,count%d", used,count); 
					/***************************Go-lite Optimization changes end***********************/
				}
			}
			
		}


		// only to be sure there is a ever ending loop
		i++;
	}

	/* x0045876, 14-Aug-2006 (WR - missing return statement at end of non-void function "sms_cb_count_messages") */
	return count;
}

/*******************************************************************************

 $Function:    	sms_cb_count_unread_messages

 $Description:	This function counts the unread messages in the fifo
           
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
UBYTE sms_cb_count_unread_messages (void)
{
	UBYTE i=0, m=0, used;

used = smscb_data.used_pointer;
	
	while (smscb_data.cb_attributes[used].next_link NEQ 0xFF)
	{
		// count only the unread messages
		if (smscb_data.cb_attributes[used].status EQ UNREAD_MESSAGE)
			m ++;
		
		// going through the link list
		used = smscb_data.cb_attributes[used].next_link;
	
		// only to be sure there is a ever ending loop
		i++;
		if (i > MAX_STORED_PAGES)	
			return 0xAA;

	}


	// have a check of the last one
	if (smscb_data.cb_attributes[used].status EQ UNREAD_MESSAGE)
		m ++;


#ifdef DEBUG_SMS_CB
	/***************************Go-lite Optimization changes Start***********************/
	//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P1("count unread %d ", m); 
	/***************************Go-lite Optimization changes end***********************/
#endif

	return m;
}


/*******************************************************************************

 $Function:    	sms_cb_give_position_of_msg

 $Description:	You give the position (order of the msg from oldest to the latest)
 				and he gives you the real positon of the link list
           
 $Returns:		

 $Arguments:	used : current used_pointer
 
*******************************************************************************/
UBYTE sms_cb_give_position_of_msg (UBYTE used, UBYTE positon)
{
	UBYTE i=0, m=0;
	
	while ( (smscb_data.cb_attributes[used].next_link NEQ 0xFF)  &&
		   (i NEQ positon) )
	{

		// going through the link list
		used= smscb_data.cb_attributes[used].next_link;

		// only to be sure there is a ever ending loop
		if (smscb_data.cb_attributes[used].start_page EQ 0x55)
		{
			i++;
		}


		
		if (m > MAX_STORED_PAGES)	
			return 0xAA;

		m++;

	}

	//return the positon in the link list 
	return  used;
}

/*******************************************************************************

 $Function:    	sms_cb_select_read

 $Description:	called by pressing the read item in SMS-Broadcast
 
 $Returns:		

 $Arguments:	


 *******************************************************************************/
int sms_cb_select_read (MfwMnu* m, MfwMnuItem* i)
{

	sms_cb_read_msg (ALL_MESSAGE);
    return 1;
}

/*******************************************************************************

 $Function:    	sms_cb_read_msg

 $Description:	This function prepare to read the cb messages
 				It could be call from everywhere.
 
 $Returns:		

 $Arguments:	


 *******************************************************************************/
int sms_cb_read_msg (T_SMSCB_STATUS status)
{
	UBYTE result;  //, i;  // RAVI


    T_MFW_HND win = mfwParent(mfw_header());

	TRACE_FUNCTION("sms_cb_read_msg");



	//count the number of the stored messages in the fifo

	result = sms_cb_count_messages ();

	if (result EQ 0x00)
	{
		TRACE_EVENT("sms cb ERROR, there are 0 cb messages");
	}


	if((smscb_data.used_pointer EQ 0xFF) || (result EQ 0x00))
	{
		TRACE_EVENT("There is no cb message stored");

		info_screen(0, TxtEmptyList ,TxtNull, NULL); 
	}
	else
	{
/***************************Go-lite Optimization changes Start***********************/
//	Aug 16, 2004    REF: CRR 24323   Deepa M.D	
		TRACE_EVENT_P1("Total msg%d ", result);			
/***************************Go-lite Optimization changes end***********************/
		if (result EQ 0xAA)
		{
			TRACE_EVENT("sms cb ERROR, couldnt find the termination");
			return 0;  // RAVI
		}


		//keep this value for the callback function from menu-list
		smscb_data.total_number = result;

		//set the reason of reading
		//read "all_message" , "unread_message", ..........
		smscb_data.current_status = status;

		//fill up the menu list attributes
		smscb_data.cb_list_attributes = sms_cb_create_list (smscb_data.total_number);


		//show the menu list on the screen now
	    listDisplayListMenu (win, smscb_data.cb_list_attributes ,(ListCbFunc)sms_cb_list_cb,0);

	}
	
    return 1;
}


/*******************************************************************************

 $Function:    	sms_cb_create_list

 $Description:	Prepare the items for the menu list
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static ListMenuData * sms_cb_create_list (UBYTE number)
{
  UBYTE i = 0, used, next_used, number_multipg =1;  // RAVI - Removed - page, pages, variable.  , control_bit=0;


  ListMenuData *list_menu_data = (ListMenuData *)ALLOC_MEMORY (sizeof(ListMenuData));

  if (list_menu_data EQ 0)
  {
	TRACE_EVENT ("failed to create the list for cb");
    return NULL;
  }


  list_menu_data->List = (T_MFW_MNU_ITEM *) ALLOC_MEMORY (number * sizeof (T_MFW_MNU_ITEM));


  if (list_menu_data->List EQ 0)
  {
	TRACE_EVENT ("failed to create the list for cb");
    return NULL;
  }

	TRACE_EVENT ("sms_cb_create_list ()");  


	//take the current position in the fifo
	used = smscb_data.used_pointer;


  /*
   * Fill Menu List
   */


  	//until to reach the end of the fifo
	while ( smscb_data.cb_attributes[used].next_link NEQ 0xFF )
	{

		if (smscb_data.cb_attributes[used].start_page EQ 0x55)
		{
			mnuInitDataItem(&list_menu_data->List[i]);

		   	next_used = used;

			// going through the link list 
			next_used = smscb_data.cb_attributes[next_used].next_link;



			//before  we put the item for a multipage, we have to
			//know how long it is.
			//This is counting the number of pages belongs to a multipage
			//Every multipage &singlepage starts with the indication 0x55
			while(smscb_data.cb_attributes[next_used].start_page NEQ 0x55)
			{
				//sprintf(buf, "number_multipg %d in while", number_multipg); 
				//TRACE_EVENT(buf);			
				
				number_multipg++;

				//go out when we reach the end of the link list
				if (smscb_data.cb_attributes[next_used].next_link EQ 0xFF)
					break;

				// going through the link list & take the next cb message
				next_used = smscb_data.cb_attributes[next_used].next_link;
			}



			//fill up the header 
			//"MID" + "available Page" + "Total page"
			//the user release the uncomplete page about the available page.
			/*SPR1920, allocate memory for header string*/
			if (smscb_data.cb_attributes[used].header == NULL)
				smscb_data.cb_attributes[used].header = (char*)ALLOC_MEMORY(sizeof(char)*25);
	    	sprintf(smscb_data.cb_attributes[used].header,"MID%d,%d/%d", smscb_data.cb_attributes[used].mid, 
   								number_multipg, smscb_data.cb_attributes[used].pages);
		
	    	list_menu_data->List[i].str =(char *) smscb_data.cb_attributes[used].header;

			//set item to visible
			if (smscb_data.current_status EQ ALL_MESSAGE)
			{
				//show all the cb messages
		        list_menu_data->List[i].flagFunc = item_flag_none;
			}
			else if (smscb_data.current_status EQ UNREAD_MESSAGE)
			{
				if (smscb_data.cb_attributes[used].status EQ smscb_data.current_status)
					list_menu_data->List[i].flagFunc = item_flag_none;
				else
					list_menu_data->List[i].flagFunc = item_flag_hide;				
			}

			i++;
			number_multipg = 1;

			
		}

		// going through the link list & take the next cb message
		used = smscb_data.cb_attributes[used].next_link;

		// only to be sure there is a ever ending loop
		if (i > MAX_STORED_PAGES)	
			return NULL;
	}


		number_multipg = 1;

		//fill up the last message of the fifo !!
		if (smscb_data.cb_attributes[used].start_page EQ 0x55)
		{
			mnuInitDataItem(&list_menu_data->List[i]);

			//fill up the header 
			//"MID" + "available Page" + "Total page"
			//the user release the uncomplete page about the available page.
			/*SPR1920, allocate memory for header string*/
			if (smscb_data.cb_attributes[used].header == NULL)
				smscb_data.cb_attributes[used].header = (char*)ALLOC_MEMORY(sizeof(char)*25);
	    	sprintf(smscb_data.cb_attributes[used].header,"MID%d,%d/%d", smscb_data.cb_attributes[used].mid, 
   								number_multipg, smscb_data.cb_attributes[used].pages);

		
	    	list_menu_data->List[i].str =(char *) smscb_data.cb_attributes[used].header;

			//set item to visible
			if (smscb_data.current_status EQ ALL_MESSAGE)
			{
				//show all the cb messages
		        list_menu_data->List[i].flagFunc = item_flag_none;
			}
			else if (smscb_data.current_status EQ UNREAD_MESSAGE)
			{
				if (smscb_data.cb_attributes[used].status EQ smscb_data.current_status)
					list_menu_data->List[i].flagFunc = item_flag_none;
				else
					list_menu_data->List[i].flagFunc = item_flag_hide;				
			}
		}





  /*
   * Fill common parameter for list handling
   */


  list_menu_data->ListLength     = number;
  list_menu_data->ListPosition     = 1;
  list_menu_data->CursorPosition  = 1;
  list_menu_data->SnapshotSize   = number;
  list_menu_data->Font           = 0;
  list_menu_data->LeftSoftKey     = TxtSoftSelect;
  list_menu_data->RightSoftKey    = TxtSoftBack;
  /* SPR#1428 - SH - Add select key to list */
  list_menu_data->KeyEvents      = KEY_CLEAR | KEY_RIGHT | KEY_LEFT | KEY_MNUUP | KEY_MNUDOWN | KEY_MNUSELECT;
  list_menu_data->Reason         = 0;
  list_menu_data->Strings        = TRUE;
  list_menu_data->Attr           = (MfwMnuAttr*)&sms_cb_menuAttrib;
  list_menu_data->autoDestroy    = TRUE; 

  return list_menu_data;
}

/*******************************************************************************

 $Function:    	sms_cb_list_cb

 $Description:	callback function of the menu list 
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static void sms_cb_list_cb(T_MFW_HND win, ListMenuData * ListData)
{
	UBYTE number, selected_postion;	
	int i;/*SPR 1920*/


	TRACE_FUNCTION ("sms_cb_list_cb()");

    switch (ListData->Reason)
    {
		case LISTS_REASON_SELECT: /*  */
	
			
			//
			selected_postion = sms_cb_give_position_of_msg (smscb_data.used_pointer, ListData->ListPosition);

			
			//display the selected message on the scree		
			sms_cb_show_cb_message (selected_postion);
			

			//destroy the list menu 
			listsDestroy(ListData->win);

			//counted the total number of cb msg before created the menu list
			number = smscb_data.total_number;

			
			FREE_MEMORY ((UBYTE *)smscb_data.cb_list_attributes->List, number * sizeof (T_MFW_MNU_ITEM));
			FREE_MEMORY ((UBYTE *)smscb_data.cb_list_attributes, sizeof (ListMenuData));
			/* SPR 1920 if any memory alloacted for list headers, deallocate it*/
			for(i=0; i<MAX_MIDS;i++)
			{	if (smscb_data.cb_attributes[i].header != NULL)
				{	FREE_MEMORY((void*)smscb_data.cb_attributes[i].header,sizeof(char)*25);
					smscb_data.cb_attributes[i].header = NULL;
				}
			}
	    break;

		case LISTS_REASON_BACK: /* back to previous menu */

			TRACE_EVENT("pressed Back-key");

			//destroy the list menu 
			listsDestroy(ListData->win);

			//counted the total number of cb msg before created the menu list
			number = smscb_data.total_number;

			FREE_MEMORY ((UBYTE *)smscb_data.cb_list_attributes->List, number * sizeof (T_MFW_MNU_ITEM));
			FREE_MEMORY ((UBYTE *)smscb_data.cb_list_attributes, sizeof (ListMenuData));
			/* SPR 1920 if any memory alloacted for list headers, deallocate it*/
			for(i=0; i<MAX_MIDS;i++)
			{	if (smscb_data.cb_attributes[i].header != NULL)
				{	FREE_MEMORY((void*)smscb_data.cb_attributes[i].header,sizeof(char)*25);
					smscb_data.cb_attributes[i].header = NULL;
				}
			}
	    break;
		default:
	    return;
    }



}


/*******************************************************************************

 $Function:    	sms_cb_list_cb

 $Description:	open a editor to show the cb message
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void sms_cb_show_cb_message(UBYTE link_list_position)
{

    T_MFW_HND win = mfwParent(mfw_header());

#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
	T_EDITOR_DATA editor_data;
#endif

	UBYTE number_multipg =1, temp, next_used; 

	//char buf[60];
		
   	next_used = link_list_position;

	//check : is it the end of the link list
	//if yes, not necassary to go through the link list
	if (smscb_data.cb_attributes[next_used].next_link NEQ 0xFF)
	{
		// going through the link list 
		next_used = smscb_data.cb_attributes[next_used].next_link;
	}

	//if the multipage contain more than one page, find out
	//how many pages !
	//This is counting the number of pages belongs to a multipage
	//Every multipage & singlepge starts with the indication 0x55
	while(smscb_data.cb_attributes[next_used].start_page NEQ 0x55)
	{
		//sprintf(buf, "number_multipg %d", number_multipg); 
		//TRACE_EVENT(buf);			

		number_multipg++;

		if (smscb_data.cb_attributes[next_used].next_link EQ 0xFF)
			break;

		// going through the link list & take the next cb message
		next_used = smscb_data.cb_attributes[next_used].next_link;
	}

	//sprintf(buf, "number_multipg %d after..", number_multipg); 
	//TRACE_EVENT(buf);			

	//keep the value for later
	smscb_data.multipage_number = number_multipg;

	sms_cb_loadEditDefault(&editor_data);

	//buffer for the multipage
	smscb_data.multipage = (UBYTE *) ALLOC_MEMORY (OFFSET(smscb_data.multipage_number) * sizeof (UBYTE));

   	next_used = link_list_position;

	//fill up the new buffer with the first page
	/*MC SPR 1920, refer to new message array*/
	strcat((char *)smscb_data.multipage, smscb_data.messages[next_used]);
	/*MC end*/
	//check : is it the end of the link list
	//if yes, not necassary to go through the link list
	if (smscb_data.cb_attributes[next_used].next_link NEQ 0xFF)
	{
		// going through the link list 
		next_used = smscb_data.cb_attributes[next_used].next_link;
	}

	temp = smscb_data.multipage_number;

	//if the multipage contain more than one page, fill up the data
	//to the new buffer
	while(temp > 1)
	{
		
		/*MC SPR 1920, refer to new message array*/
		strcat((char *)smscb_data.multipage, smscb_data.messages[next_used]);
		/*MC end*/

		// going through the link list & take the next cb message
		next_used = smscb_data.cb_attributes[next_used].next_link;

		temp --;
	}

	//set the mark
	smscb_data.cb_attributes[link_list_position].status = READ_MESSAGE;

	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, smscb_data.multipage, OFFSET(smscb_data.multipage_number));
	AUI_edit_SetEvents(&editor_data, link_list_position, TRUE, FOREVER, (T_AUI_EDIT_CB)sms_cb_editor_cb);
	AUI_edit_SetTextStr(&editor_data, TxtDelete, TxtSoftBack, 0, NULL);
	AUI_edit_SetAltTextStr(&editor_data, 0, TxtSoftOptions, FALSE, TxtNull);
	AUI_edit_SetMode(&editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);
	AUI_edit_Start(win, &editor_data);
#else /* NEW_EDITOR */
	editor_data.editor_attr.text =(char *) smscb_data.multipage;

	editor_data.Identifier            = link_list_position ; //keep the value (selected cb message)
	editor_data.editor_attr.size     = OFFSET(smscb_data.multipage_number); 
	editor_data.min_enter		 = 0; 
	editor_data.LeftSoftKey          = TxtDelete;
	editor_data.AlternateLeftSoftKey  = TxtSoftOptions;
	editor_data.RightSoftKey         = TxtSoftBack;
	editor_data.Callback            = (T_EDIT_CB)sms_cb_editor_cb;
	editor_data.mode           	 = READ_ONLY_MODE;

	editor_start(win, &editor_data);  /* start the editor */
#endif /* NEW_EDITOR */
}

/*******************************************************************************

 $Function:    	sms_cb_loadEditDefault

 $Description:	Configure T_EDITOR_DATA structure with default editor settings.

 $Returns:		none
 
 $Arguments:	editor_data - editor data.

*******************************************************************************/
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
void sms_cb_loadEditDefault (T_AUI_EDITOR_DATA *editor_data)
{
	    TRACE_FUNCTION ("sms_cb_loadEditDefault()");

		AUI_edit_SetDefault(editor_data);
		AUI_edit_SetMode(editor_data, ED_MODE_ALPHA, ED_CURSOR_NONE);
		AUI_edit_SetTextStr(editor_data, TxtSoftSelect, TxtSoftBack, 0, NULL);
		AUI_edit_SetDisplay(editor_data, 0, COLOUR_EDITOR, EDITOR_FONT);

		return;
}
#else /* NEW_EDITOR */

void sms_cb_loadEditDefault (T_EDITOR_DATA *editor_data)
{
	    TRACE_FUNCTION ("sms_cb_loadEditDefault()");

		editor_attr_init(&editor_data->editor_attr, NULL, edtCurNone, 0, 0, 0, COLOUR_EDITOR);
		editor_data_init(editor_data, NULL, TxtSoftSelect, TxtSoftBack, 0, 1, ALPHA_MODE, FOREVER);
		editor_data->hide				= FALSE;
		editor_data->AlternateLeftSoftKey   = TxtNull;
		editor_data->Identifier			=  0; 
		editor_data->TextString			= NULL;  
		editor_data->destroyEditor		= TRUE ; 
}
#endif /* NEW_EDITOR */


/*******************************************************************************

 $Function:    	sms_cb_editor_cb

 $Description:	callback function of the editor 

 $Returns:		none
 
 $Arguments:	

*******************************************************************************/
static void sms_cb_editor_cb (T_MFW_HND win, USHORT Identifier, SHORT reason)
{

//only for tracing 
#ifdef DEBUG_SMS_CB
	UBYTE w;

	/* x0045876, 14-Aug-2006 (WR - "test" & "i" was declared but never referenced) */
	UBYTE test, i;
	
#endif

	TRACE_FUNCTION ("sms_cb_editor_cb()");

	FREE_MEMORY ((UBYTE *)smscb_data.multipage, OFFSET(smscb_data.multipage_number) * sizeof (UBYTE));
	
	switch (reason)
	{
		case INFO_KCD_LEFT:


			//user selected to delete the message
			sms_cb_delete_message (smscb_data.cb_attributes[Identifier].mid);

			//update the use_pointer  after deleting
			smscb_data.used_pointer = smscb_data.result_used;

			if (smscb_data.used_pointer EQ smscb_data.free_pointer)
			{
				//no anymore messages stored
				smscb_data.used_pointer = 0xFF;
			}


			if (smscb_data.current_status EQ ALL_MESSAGE)
			{
				//go back to the cb list 
				sms_cb_read_msg (smscb_data.current_status);
			}
			else if (smscb_data.current_status EQ UNREAD_MESSAGE)
			{
//				if (sms_cb_count_unread_messages ())
//					sms_cb_read_msg (smscb_data.current_status);
		
			}


			
			break;
          
		case INFO_KCD_RIGHT:
		case INFO_KCD_HUP:


//only for tracing 
#ifdef DEBUG_SMS_CB
			/***************************Go-lite Optimization changes Start***********************/
			//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
			TRACE_EVENT_P1( ".. used_pointer %d ", smscb_data.used_pointer); 
			TRACE_EVENT_P1("..free_pointer %d ", smscb_data.free_pointer); 
			for (i=0; i<MAX_STORED_PAGES ; i++)	
			{
				TRACE_EVENT_P2("link_index: %d, i:%d", smscb_data.cb_attributes[i].next_link,i); 
			}
			test = smscb_data.used_pointer;
			while (smscb_data.cb_attributes[test].next_link NEQ 0xFF)
			{
				//sprintf(buf, "Buffer on %s", &smscb_data.cb_buffer[OFFSET(test)]); 
				/*MC*/
				TRACE_EVENT_P1("Buffer on %s", smscb_data.messages[test]); 
				test = smscb_data.cb_attributes[test].next_link;
			}
			//sprintf(buf, "Buffer on %s,i:%d ", &smscb_data.cb_buffer[OFFSET(test)], i); 
			/*MC*/
			TRACE_EVENT_P2("Buffer on %s,i:%d ", smscb_data.messages[test], i); 
			/***************************Go-lite Optimization changes end***********************/
#endif //DEBUG_SMS_CB



			if (smscb_data.current_status EQ ALL_MESSAGE)
			{
				//go back to the cb list 
		/* Nov 14, 2006 REF: OMAPS00099394  x0039928
		    Fix: List recreation is commented.
				sms_cb_read_msg (smscb_data.current_status);
              */
			}
			else if (smscb_data.current_status EQ UNREAD_MESSAGE)
			{
		
			}
			break;

		default:
			TRACE_EVENT("Err: Default");
			break;
	}
}



