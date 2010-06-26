/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		    MmiLists.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    22/02/01                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

  

********************************************************************************

 $History: MmiLists.c

    July 10, 2007    REF:  DRT OMAPS00135679  x0073106
    Description:MM: Board freezes while choosing settings menu in File management.
    Solution: Hang-up key is mapped properly while going back from audio list window.

    Mar 30, 2007    REF: OMAPS00122691  x0039928
    Description: COMBO: No melody heard on parsing over different volums in tones menu
    Solution: E_MELODY_START event is sent to play the melody.
    
 	xrashmic 8 Sep 2006, OMAPS00091618
	Removed redundant code introduced during integration
 	
    June 19, 2006    REF: DRT OMAPS00076378  x0012849 : Jagannatha
    Description: MIDI files should play while browsing
    Solution: Event will be generated upon press of Menu up and down key.

	June 05, 2006 REF:OMAPS00060424 x0045876
 	Description: Header Toggling
 	Solution: Implemented the toggling of header when displaying the length text in SELECT ITEM and SET UP MENU

 	May 18, 2006    REF: DRT OMAPS00076438  xdeepadh	
	Description: Scrolling not implemented in Imageviewer
	Solution: The support for scrolling has been provided.

       	May 03, 2006    REF: DRT OMAPS00076439  xdeepadh
	Description: Image viewer back not working 
	Solution: The Preview List will be repopulated, when the list window is 
	resumed.

       	May 03, 2006    REF: DRT OMAPS00076376  xdeepadh
	Description: File Viewer-N5.12 issues (very unstable)
	Solution: The keys are blocked until the view or preview is completed.
 	
	Dec 22, 2005    REF: ENH  xdeepadh
	Description: Image Viewer Application
	Solution: Implemeted the Image Viewer  to view the jpeg images

       xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
       Using the MenuSelect Key for requesting the help info in STK menu. Storing the info that 
       MenuSelectKey was pressed in a list view.
                
   	May 24, 2005    REF: CRR 29358  x0021334
	Description: CT_PTCRB 27.22.4.9.8 fails: wrong terminal response
	Fix:	The implementation is now based on timer. After the specified timeout period
	       the control comes back to SAT main menu, if there is no selection done by the user.

	25/10/00			Original Condat(UK) BMI version.	
	   
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

#include "pconst.cdg"
#include "mconst.cdg"
#include "message.h"
#include "ccdapi.h"
#include "prim.h"
#include "cus_aci.h"
#include "cnf_aci.h"
#include "mon_aci.h"

#include "tok.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif




#include "mfw_sys.h"

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
#include "mfw_lng.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_tim.h"

#include "mfw_sim.h"
#include "mfw_cm.h"
#include "mfw_nm.h"
#include "mfw_phb.h"
#include "mfw_mme.h"
#include "mfw_sat.h"
#include "mfw_sms.h"

#include "dspl.h"

#include "ksd.h"
#include "psa.h"

#include "MmiMain.h"
#include "MmiBookController.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"



#include "MmiMenu.h"
#include "mmiCall.h"
#include "Mmiicons.h"
#include "MmiIdle.h"

#include "MmiSoftKeys.h"
#include "MmiSounds.h"
#include "MmiIdle.h"
#include "MmiNetwork.h"
#include "mmiSat_i.h"
#include "MmiAoc.h"


#include "cus_aci.h"
#include "mfw_ffs.h"
#include "MmiTimers.h"



#include "mmiColours.h"


#ifndef MFW_EVENT_PASSED
#define MFW_EVENT_PASSED 0
#endif

/* START: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
MfwHnd g_win;		

extern MfwHnd g_title_timer;
/* END: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */

extern MfwRect menuArea;               /* menus display area      */
extern BOOL getMelodyListStatus(void);

typedef enum
{
    E_INIT,
    E_BACK,
    E_RETURN,
    E_ABORT,
    E_EXIT
} e_lists_events;



/*-----------------------------------------------------------------------------
*
* List menu handling functions:
*
*-----------------------------------------------------------------------------*/
// ADDED BY RAVI - 28-11-2005
EXTERN UBYTE mnuDCSType (MfwHnd m, UBYTE listLanguageType);
EXTERN BOOL getScrollStatus(void);
// END RAVI - 28-11-2005


static int listsKeyEvent(MfwEvt event, MfwKbd *keyboard);
static int listsWinEvent(MfwEvt e, MfwWin *w);
static int listsListCbMenu(MfwEvt EventCode, MfwMnu *MenuHandle);
static void lists_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int listsIconsKeyEvent(MfwEvt event, MfwKbd *keyboard);
static int listsIconsWinEvent(MfwEvt e, MfwWin *w);
static int listsIconsListCbMenu(MfwEvt EventCode, MfwMnu *MenuHandle);
static void listsIcons_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);


/*******************************************************************************

 $Function:  listIconsDisplayListMenu  

 $Description:	 Creates a list menu from data stored in NVRAM.

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
 MfwHnd winIconsLists;
T_MFW listIconsDisplayListMenu (MfwHnd Parent, ListMenuData *Data, ListCbFunc cbFunc,int inUnicode)
{
    // UBYTE i;  // RAVI - Not Used.
    MfwWin * win;
    MfwHnd winLists;
    T_MFW Status = LISTS_OK;

    TRACE_FUNCTION ("listIconsDisplayListMenu() Start");

    winLists = winCreate(Parent, 0, E_WIN_VISIBLE |E_WIN_RESUME|E_WIN_SUSPEND, (MfwCb)listsIconsWinEvent);
	winIconsLists=winLists;
    if (winLists == NULL)
	{
	    Status = LISTS_FAIL;				/* Inform caller of failure. */
	    TRACE_EVENT ("win creation fails");
	}
    else
	{
	    ListIconsWinInfo * ListData = (ListIconsWinInfo *)ALLOC_MEMORY (sizeof (ListIconsWinInfo));

		if (!ListData)
			{
				/* cq12719. if insufficient memory for listdata then delete window and Free allocated memory.16-03-04.MZ */
				if(NULL != winLists)
					winDelete(winLists);
				
				 TRACE_EVENT ("(ListIconsWinInfo *)ALLOC_MEMORY fails");
				 return LISTS_FAIL;
			}

	    ListData->mmi_control.dialog = (T_DIALOG_FUNC)lists_exec_cb;
	    ListData->mmi_control.data   = ListData;
	    win = ((MfwHdr *)winLists)->data;
	    win->user = (void *) ListData;

	    ListData->Parent = Parent;
	    ListData->Callback = cbFunc;
	    ListData->winLists = winLists;
	    ListData->kbdLists =      kbdCreate(winLists,Data->KeyEvents|KEY_MAKE,(MfwCb)listsIconsKeyEvent);
	    ListData->kbdLists_long = kbdCreate(winLists,Data->KeyEvents|KEY_LONG,(MfwCb)listsIconsKeyEvent);

	    if ((ListData->kbdLists == NULL) || (ListData->kbdLists_long == NULL))
		{
		    Status = LISTS_FAIL;
			TRACE_EVENT ("ListData->kbdLists fails");
		}
	    else
		{
		    /* Create initial list menu for display. */

		    /*
		     * set menu list from outside
		     */
		    ListData->MenuListsAttr = *Data->Attr; /* copy contents */

		    /* SPR#2492 - DS - Do not overwrite mode settings unless mode is zero */
		    if (ListData->MenuListsAttr.mode == 0x0000) /* Mode not yet setup */
         		    ListData->MenuListsAttr.mode = MNU_LEFT | MNU_LIST | MNU_CUR_LINE;
		    else
        		    ListData->MenuListsAttr.mode |= Data->Attr->mode;
		    
 		    ListData->MenuListsAttr.font = Data->Font; 
		    ListData->MenuListsAttr.items = Data->List;
		    ListData->MenuListsAttr.nItems = Data->ListLength;

		    /* START: x0045876, 05-June-2006 (OMAPS00060424 - Header Toggle) */
		   ListData->MenuListsAttr.header_toggle = Data->Attr->header_toggle;
		   if (ListData->MenuListsAttr.header_toggle)
		   	g_win = winLists;
		    /* END: x0045876, 05-June-2006 (OMAPS00060424 - Header Toggle) */


		    /*
		     * Intialize List Menu Data
		     */
		    ListData->MenuData = Data; /* copy contents */
		    ListData->MenuData->listCol = COLOUR_LIST_XX; //Set-up colour
		    ListData->MenuData->Attr = &ListData->MenuListsAttr; /* copy data*/
		    ListData->MenuLists = mnuCreate(winLists, &ListData->MenuListsAttr,
						    E_MNU_SELECT | E_MNU_ESCAPE| E_MNU_POSTDRAW, 
						    (MfwCb)listsIconsListCbMenu);
			ListData->MenuData->win = winLists;
			Data->win = winLists;
			mnuScrollMode(ListData->MenuLists,0);						
			if (Data->Strings)
				mnuLang (ListData->MenuLists,0);
			else
				mnuLang (ListData->MenuLists,mainMmiLng);

			/* SPR#1983 - SH - Set up whether menu is in unicode or not */
			if (inUnicode != 0)
			{
			    mnuDCSType(ListData->MenuLists, inUnicode); /* Unicode display */
			}
		    mnuChooseVisibleItem(ListData->MenuLists, Data->ListPosition-1);
		    mnuUnhide(ListData->MenuLists);
			displaySoftKeys(ListData->MenuData->LeftSoftKey,ListData->MenuData->RightSoftKey);
		    winShow(winLists);
			
		}
	}
    return Status;
}

static int listsIconsKeyEvent (MfwEvt e, MfwKbd *k)
    /* list keyboard event handler */
{
    T_MFW_HND    win  = mfwParent(mfw_header());
    T_MFW_WIN  * win_data = ((T_MFW_HDR *)win)->data;
    ListIconsWinInfo      * data = (ListIconsWinInfo *)win_data->user;
    MfwMnu *MenuHandle;

	/* This trace function was added inside the fflag by vinoj to remove compilation error: 2/6/06*/
	TRACE_FUNCTION("listsIconsKeyEvent() start");
    {
        //xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
       //Initialize selectKey to FALSE here. It will be set to TRUE only when a key event for KCD_MNUSELECT occurs.
       data->MenuData->selectKey = FALSE;
	if (!(e & KEY_MAKE))
	{
		return MFW_EVENT_CONSUMED;
	}
	if(data->MenuData->block_keys == TRUE)
	{
		return MFW_EVENT_CONSUMED;
	}
    if (e & KEY_LONG)
	{
	    switch (k->code)
		{
		case KCD_HUP: /* back to previous menu */
		    mnuEscape(data->MenuLists);
		    break;
		case KCD_RIGHT: /* Power Down */
		    return MFW_EVENT_PASSED; /* handled by idle */
		default: /* no response to all other keys */
		    return MFW_EVENT_CONSUMED;
		}
	}
    else
	{
	    switch (k->code)
		{
		case KCD_MNUUP: /* highlight previous entry */
		    mnuUp(data->MenuLists);
		      {
		      MenuHandle = mfwControl(data->MenuLists);
			 	data->MenuData->Reason = LISTS_REASON_SCROLL;
				data->MenuData->CursorPosition = MenuHandle->lCursor[MenuHandle->level];
				data->MenuData->ListPosition  = MenuHandle->lCursor[MenuHandle->level]; 
		    	  // Send event to indicate that the user has scrolled list items
		     	  SEND_EVENT(data->Parent, LISTS_REASON_SCROLL, 0, NULL);	
		      }			 			    

		    break;
		case KCD_MNUDOWN: /* highlight next entry */
		    mnuDown(data->MenuLists);
		      {
		      MenuHandle = mfwControl(data->MenuLists);
			 	data->MenuData->Reason = LISTS_REASON_SCROLL;
				data->MenuData->CursorPosition = MenuHandle->lCursor[MenuHandle->level];
				data->MenuData->ListPosition  = MenuHandle->lCursor[MenuHandle->level]; 
		    	  // Send event to indicate that the user has scrolled list items
		     	  SEND_EVENT(data->Parent, LISTS_REASON_SCROLL, 0, NULL);	
		      }			 			    
		    break;
		case KCD_MNUSELECT:
                //xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
                //Menu Select Key was pressed, storing this for future use.
                data->MenuData->selectKey = TRUE;                
                //Break is delibrately not used here. The control should pass through the KCD_LEFT case.
		case KCD_LEFT: /* activate this entry */
 	    	MenuHandle = mfwControl(data->MenuLists);
               data->MenuData->CursorPosition = MenuHandle->lCursor[MenuHandle->level];
		    mnuSelect(data->MenuLists);
		    break;
		case KCD_CALL: /* press send key */
		    data->ExitReason = LISTS_REASON_SEND;
		    data->MenuData->Reason = LISTS_REASON_SEND;
	    	MenuHandle = mfwControl(data->MenuLists);
		    data->MenuData->CursorPosition = MenuHandle->lCursor[MenuHandle->level];
		    data->MenuData->ListPosition  = MenuHandle->lCursor[MenuHandle->level]; /* True cursor position in list menu. */

		    /* SPR998 - SH - check if callback exists before calling it! */
		    if (data->Callback)
		    	data->Callback(data->Parent, data->MenuData);
		    	
			if (data->MenuData->autoDestroy)
			{
				/* START: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
				if (data->MenuListsAttr.header_toggle)
				{
					if (g_title_timer)
					{
						tim_delete(g_title_timer);
				     		g_title_timer = NULL;
					}
				}
				/* END: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
				winDelete (win);
	    			FREE_MEMORY ((void*)data, sizeof (ListIconsWinInfo));
		    	}
		break;
		case KCD_HUP: /* back to previous menu */
		    data->ExitReason = LISTS_REASON_HANGUP;
		    data->MenuData->Reason = LISTS_REASON_HANGUP;
   			/*NM, p023 */

   	    	MenuHandle = mfwControl(data->MenuLists);
		    data->MenuData->CursorPosition = MenuHandle->lCursor[MenuHandle->level];
		    data->MenuData->ListPosition  = MenuHandle->lCursor[MenuHandle->level]; /* True cursor position in list menu. */
   			/*NM, p023 end */
		     data->Callback(data->Parent, data->MenuData);
			//July 10, 2007    REF:  DRT OMAPS00135679  x0073106	
			/*if (data->MenuData->autoDestroy)
			{*/
			       /* START: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
				if (data->MenuListsAttr.header_toggle)
				{
					if (g_title_timer)
					{
						tim_delete(g_title_timer);
				     		g_title_timer = NULL;
					}
				}
				/* END: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
				/*	winDelete (win);
	    		    	FREE_MEMORY ((void*)data, sizeof (ListIconsWinInfo));
		    }*/
			break;
		case KCD_RIGHT: /* back to previous menu */
		    mnuEscape(data->MenuLists);
		    break;
		default: /* no response to all other keys */
		    return MFW_EVENT_CONSUMED;
		}
	}
        }
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:  listsWinEvent  

 $Description:	 window event handler

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
static int listsIconsWinEvent(MfwEvt e, MfwWin *w)
{
    ListIconsWinInfo * ListData = (ListIconsWinInfo *)w->user;

    TRACE_FUNCTION("listsIconsWinEvent() start");
    TRACE_EVENT_P1("ListData->Parent %x",ListData->Parent);
    switch (e)
	{
        case E_WIN_VISIBLE:
            SEND_EVENT(ListData->Parent,LISTS_REASON_VISIBLE,0,(void*)ListData); 	
 	    	break;
        case MfwWinResume:
             SEND_EVENT(ListData->Parent,LISTS_REASON_RESUME,0,(void*)ListData); 	
	 break;	
        case MfwWinSuspend:
	   SEND_EVENT(ListData->Parent,LISTS_REASON_SUSPEND,0,(void*)ListData); 	
	 break;	
  	}
    return MFW_EVENT_CONSUMED;
}
/*a0393213 lint warning removal - This function is not used currently. So it's put under the flag. If it is required just remove the flag.*/
#ifdef MMI_LINT_WARNING_REMOVAL
static void listsIcons_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    ListIconsWinInfo    * ListData = (ListIconsWinInfo *)win_data->user;
    T_MFW_HND      * l_parent;
    SHORT            l_id;

    TRACE_FUNCTION ("listsIcons_exec_cb() start");

    switch (event)
	{
	case E_INIT:
	case E_BACK:
	case E_RETURN:
	case E_ABORT:
	case E_EXIT:
	    /* defaultreaction */
	    l_parent = ListData->Parent;
	    l_id = ListData->id;

	    /* START: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
	    if (ListData->MenuListsAttr.header_toggle)
	    {
			if (g_title_timer)
			{
				tim_delete(g_title_timer);
				g_title_timer = NULL;
			}
	    }
	    /* END: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
	    winDelete (win);
	    FREE_MEMORY ((void*) ListData, sizeof (ListIconsWinInfo));
	    SEND_EVENT (l_parent, event, l_id, NULL); /* forward event to parent */
	    break;
	default:
	    return;
	}
}
#endif


/*******************************************************************************

 $Function:  listsListCbMenu  

 $Description:	 List menu handler. (mfw callback function)

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
int listsIconsListCbMenu(MfwEvt EventCode, MfwMnu *MenuHandle)
{
    int Status = LISTS_OK;
    T_MFW_HND   win      = mfw_parent (mfw_header());
    T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
    ListIconsWinInfo* ListData = (ListIconsWinInfo *)win_data->user;

    TRACE_FUNCTION("listsIconsListCbMenu() ");
    switch (EventCode)
	{
        case E_MNU_SELECT:
	    ListData->ExitReason = E_MNU_SELECT;
	    ListData->MenuData->Reason = LISTS_REASON_SELECT;// MZ 14/2/01 E_MNU_SELECT;
	    ListData->MenuData->CursorPosition = MenuHandle->lCursor[MenuHandle->level];
	    ListData->MenuData->ListPosition  = MenuHandle->lCursor[MenuHandle->level]; /* True cursor position in list menu. */
	    /* SPR998 - SH - check if callback exists before calling it! */
		if (ListData->Callback)
	    	ListData->Callback(ListData->Parent, ListData->MenuData);
		break;
	case E_MNU_ESCAPE:
	    ListData->ExitReason = LISTS_REASON_BACK;
	    ListData->MenuData->Reason = LISTS_REASON_BACK;
	    /*NM, p023*/
	    ListData->MenuData->CursorPosition = MenuHandle->lCursor[MenuHandle->level];
	    ListData->MenuData->ListPosition  = MenuHandle->lCursor[MenuHandle->level]; /* True cursor position in list menu. */
	    /*NM, p023*/
	    
	    /* SPR998 - SH - check if callback exists before calling it! */
		if (ListData->Callback)
	    	ListData->Callback(ListData->Parent, ListData->MenuData);

	    break;
	case E_MNU_POSTDRAW:
	{
		T_MFW_WIN * winIconsLists_data = ((T_MFW_HDR *)winIconsLists)->data;
	    ListIconsWinInfo* winIconsListsData = (ListIconsWinInfo *)winIconsLists_data->user;		
		    winIconsListsData->ExitReason = LISTS_REASON_DRAWCOMPLETE;
	    winIconsListsData->MenuData->Reason = LISTS_REASON_DRAWCOMPLETE;
	    winIconsListsData->MenuData->CursorPosition = MenuHandle->lCursor[MenuHandle->level];
	    winIconsListsData->MenuData->ListPosition  = MenuHandle->lCursor[MenuHandle->level]; /* True cursor position in list menu. */
		winIconsListsData->MenuData->lineHeight=  MenuHandle->lineHeight;
		if (winIconsListsData->Callback)
	    	winIconsListsData->Callback(winIconsListsData->Parent, winIconsListsData->MenuData);
	}
	    break;
        default:
	    Status = LISTS_FAIL;
	    break;
	}


    return Status;
}

void listsIconsDestroy(T_MFW_HND win)
{
   	T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
   	ListIconsWinInfo * ListData = (ListIconsWinInfo *)win_data->user;
    TRACE_EVENT_P1("listsIconsDestroy %x",win);

    	/* START: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
   	if (ListData->MenuListsAttr.header_toggle)
   	{
   		if(g_title_timer)
      		{
      			tim_delete(g_title_timer);
      			g_title_timer = NULL;
		}
	}
    	/* END: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
	    
       winDelete (win);
   	FREE_MEMORY ((void*) ListData, sizeof (ListIconsWinInfo));
    
}

/*******************************************************************************

 $Function:  listDisplayListMenu  

 $Description:	 Creates a list menu from data stored in NVRAM.

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
T_MFW listDisplayListMenu (MfwHnd Parent, ListMenuData *Data, ListCbFunc cbFunc,int inUnicode)
{
    // UBYTE i;  // RAVI - Not Used.
    MfwWin * win;
    MfwHnd winLists;
    T_MFW Status = LISTS_OK;

    TRACE_FUNCTION ("listDisplayListMenu() Start");

    winLists = winCreate(Parent, 0, E_WIN_VISIBLE |E_WIN_RESUME|E_WIN_SUSPEND, (MfwCb)listsWinEvent);

    if (winLists == NULL)
	{
	    Status = LISTS_FAIL;				/* Inform caller of failure. */
	    TRACE_EVENT ("win creation fails");
	}
    else
	{
	    ListWinInfo * ListData = (ListWinInfo *)ALLOC_MEMORY (sizeof (ListWinInfo));

		if (!ListData)
			{
				/* cq12719. if insufficient memory for listdata then delete window and Free allocated memory.16-03-04.MZ */
				if(NULL != winLists)
					winDelete(winLists);
				
				 TRACE_EVENT ("(ListWinInfo *)ALLOC_MEMORY fails");
				 return LISTS_FAIL;
			}

	    ListData->mmi_control.dialog = (T_DIALOG_FUNC)lists_exec_cb;
	    ListData->mmi_control.data   = ListData;
	    win = ((MfwHdr *)winLists)->data;
	    win->user = (void *) ListData;

	    ListData->Parent = Parent;
	    ListData->Callback = cbFunc;
	    ListData->winLists = winLists;
	    ListData->kbdLists =      kbdCreate(winLists,Data->KeyEvents|KEY_MAKE,         (MfwCb)listsKeyEvent);
	    ListData->kbdLists_long = kbdCreate(winLists,Data->KeyEvents|KEY_LONG,(MfwCb)listsKeyEvent);

	    if ((ListData->kbdLists == NULL) || (ListData->kbdLists_long == NULL))
		{
		    Status = LISTS_FAIL;
			TRACE_EVENT ("ListData->kbdLists fails");
		}
	    else
		{
		    /* Create initial list menu for display. */

		    /*
		     * set menu list from outside
		     */
		    ListData->MenuListsAttr = *Data->Attr; /* copy contents */

		    /* SPR#2492 - DS - Do not overwrite mode settings unless mode is zero */
		    if (ListData->MenuListsAttr.mode == 0x0000) /* Mode not yet setup */
         		    ListData->MenuListsAttr.mode = MNU_LEFT | MNU_LIST | MNU_CUR_LINE;
		    else
        		    ListData->MenuListsAttr.mode |= Data->Attr->mode;
		    
 		    ListData->MenuListsAttr.font = Data->Font; 
		    ListData->MenuListsAttr.items = Data->List;
		    ListData->MenuListsAttr.nItems = Data->ListLength;

		    /* START: x0045876, 05-June-2006 (OMAPS00060424 - Header Toggle) */
		   ListData->MenuListsAttr.header_toggle = Data->Attr->header_toggle;
		   if (ListData->MenuListsAttr.header_toggle)
		   	g_win = winLists;
		    /* END: x0045876, 05-June-2006 (OMAPS00060424 - Header Toggle) */


		    /*
		     * Intialize List Menu Data
		     */
		    ListData->MenuData = *Data; /* copy contents */
		    ListData->MenuData.listCol = COLOUR_LIST_XX; //Set-up colour
		    ListData->MenuData.Attr = &ListData->MenuListsAttr; /* copy data*/
		    ListData->MenuLists = mnuCreate(winLists, &ListData->MenuListsAttr,
						    E_MNU_SELECT | E_MNU_ESCAPE, 
						    (MfwCb)listsListCbMenu);
			ListData->MenuData.win = winLists;
			Data->win = winLists;
			mnuScrollMode(ListData->MenuLists,1);						
			if (Data->Strings)
				mnuLang (ListData->MenuLists,0);
			else
				mnuLang (ListData->MenuLists,mainMmiLng);

			/* SPR#1983 - SH - Set up whether menu is in unicode or not */
			if (inUnicode != 0)
			{
			    mnuDCSType(ListData->MenuLists, inUnicode); /* Unicode display */
			}
		    mnuChooseVisibleItem(ListData->MenuLists, Data->ListPosition-1);
		    mnuUnhide(ListData->MenuLists);
			TRACE_EVENT ("mmiList winShow()");
		    winShow(winLists);
		}
	}

    return Status;
}


/*******************************************************************************

 $Function:  listsKeyEvent  

 $Description:	 Keyboard event handler

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
static int listsKeyEvent (MfwEvt e, MfwKbd *k)
    /* list keyboard event handler */
{
    T_MFW_HND    win  = mfwParent(mfw_header());
    T_MFW_WIN  * win_data = ((T_MFW_HDR *)win)->data;
    ListWinInfo      * data = (ListWinInfo *)win_data->user;
    MfwMnu *MenuHandle;

	/* This trace function was added inside the fflag by vinoj to remove compilation error: 2/6/06*/

    {
        //xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
       //Initialize selectKey to FALSE here. It will be set to TRUE only when a key event for KCD_MNUSELECT occurs.
       data->MenuData.selectKey = FALSE;
	if (!(e & KEY_MAKE))
	{
		return MFW_EVENT_CONSUMED;
	}


    if (e & KEY_LONG)
	{
	    switch (k->code)
		{
		case KCD_HUP: /* back to previous menu */
		    mnuEscape(data->MenuLists);
		    break;
		case KCD_RIGHT: /* Power Down */
		    return MFW_EVENT_PASSED; /* handled by idle */
		default: /* no response to all other keys */
		    return MFW_EVENT_CONSUMED;
		}
	}
    else
	{
	    switch (k->code)
		{
		case KCD_MNUUP: /* highlight previous entry */
		    mnuUp(data->MenuLists);
//GW Not needed?		    winShow(win);
		    // Only required in melody list selection.
          /*  Mar 30, 2007    REF: OMAPS00122691  x0039928 */
            /*  This event will be used to play the selected file. */

		     if(getMelodyListStatus()== TRUE)
		    {
		    		MenuHandle = mfwControl(data->MenuLists);
		    		
		    		data->MenuData.Reason = LISTS_REASON_SCROLL;
				data->MenuData.CursorPosition = MenuHandle->lCursor[MenuHandle->level];
				data->MenuData.ListPosition  = MenuHandle->lCursor[MenuHandle->level]; 
				SEND_EVENT(data->Parent,E_START_MELODY,0,(void*)data);		    
		    }
				 
		    if(getScrollStatus() == TRUE)
		    {
		    	// Send event to indicate that the user has scrolled list items
		     	SEND_EVENT(data->Parent, LISTS_REASON_SCROLL, 0, NULL);	
		    }
		    break;
		case KCD_MNUDOWN: /* highlight next entry */
		    mnuDown(data->MenuLists);
//GW Not needed?		    winShow(win);
          /*  Mar 30, 2007    REF: OMAPS00122691  x0039928 */
            /*  This event will be used to play the selected file. */

           		 if(getMelodyListStatus()== TRUE)
			{
			 	MenuHandle = mfwControl(data->MenuLists);
			 	
			 	data->MenuData.Reason = LISTS_REASON_SCROLL;
				data->MenuData.CursorPosition = MenuHandle->lCursor[MenuHandle->level];
				data->MenuData.ListPosition  = MenuHandle->lCursor[MenuHandle->level]; 
	    		       SEND_EVENT(data->Parent,E_START_MELODY,0,(void*)data);
			 }
				 
		      if(getScrollStatus() == TRUE)
		      {
		    	  // Send event to indicate that the user has scrolled list items
		     	  SEND_EVENT(data->Parent, LISTS_REASON_SCROLL, 0, NULL);	
		      }			 			    
		    break;
		case KCD_MNUSELECT:
                //xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
                //Menu Select Key was pressed, storing this for future use.
                data->MenuData.selectKey = TRUE;                
                //Break is delibrately not used here. The control should pass through the KCD_LEFT case.
		case KCD_LEFT: /* activate this entry */
		    mnuSelect(data->MenuLists);
		    MenuHandle = mfwControl(data->MenuLists);
			if ((MenuHandle != NULL) && // Marcus: Issue 1810: 12/03/2003
			    (MenuHandle->curAttr != NULL) &&
			    winIsFocussed(win))
			{
					softKeys_displayId( TxtSoftSelect, TxtSoftBack, 0, MenuHandle->curAttr->mnuColour);
			}
		    break;
		case KCD_CALL: /* press send key */
		    data->ExitReason = LISTS_REASON_SEND;
		    data->MenuData.Reason = LISTS_REASON_SEND;
	    	MenuHandle = mfwControl(data->MenuLists);
		    data->MenuData.CursorPosition = MenuHandle->lCursor[MenuHandle->level];
		    data->MenuData.ListPosition  = MenuHandle->lCursor[MenuHandle->level]; /* True cursor position in list menu. */

		    /* SPR998 - SH - check if callback exists before calling it! */
		    if (data->Callback)
		    	data->Callback(data->Parent, &data->MenuData);
		    	
			if (data->MenuData.autoDestroy)
			{
				/* START: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
				if (data->MenuListsAttr.header_toggle)
				{
					if (g_title_timer)
					{
						tim_delete(g_title_timer);
				     		g_title_timer = NULL;
					}
				}
				/* END: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
				winDelete (win);
	    			FREE_MEMORY ((void*)data, sizeof (ListWinInfo));
		    	}
		break;
		case KCD_HUP: /* back to previous menu */
		    data->ExitReason = LISTS_REASON_HANGUP;
		    data->MenuData.Reason = LISTS_REASON_HANGUP;
   			/*NM, p023 */
   	    	MenuHandle = mfwControl(data->MenuLists);
		    data->MenuData.CursorPosition = MenuHandle->lCursor[MenuHandle->level];
		    data->MenuData.ListPosition  = MenuHandle->lCursor[MenuHandle->level]; /* True cursor position in list menu. */
   			/*NM, p023 end */
		    data->Callback(data->Parent, &data->MenuData);
			if (data->MenuData.autoDestroy)
			{
			       /* START: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
				if (data->MenuListsAttr.header_toggle)
				{
					if (g_title_timer)
					{
						tim_delete(g_title_timer);
				     		g_title_timer = NULL;
					}
				}
				/* END: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
			    	winDelete (win);
	    		    	FREE_MEMORY ((void*)data, sizeof (ListWinInfo));
		    }
			break;
		case KCD_RIGHT: /* back to previous menu */
		    mnuEscape(data->MenuLists);
		    break;
		default: /* no response to all other keys */
		    return MFW_EVENT_CONSUMED;
		}
	}
        }
    TRACE_FUNCTION("listsKeyEvent()-end");
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:  listsWinEvent  

 $Description:	 window event handler

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
static int listsWinEvent(MfwEvt e, MfwWin *w)
{
    ListWinInfo * ListData = (ListWinInfo *)w->user;

    TRACE_FUNCTION("listsWinEvent()");

    switch (e)
	{
        case E_WIN_VISIBLE:
            SEND_EVENT(ListData->Parent,LISTS_REASON_VISIBLE,0,(void*)ListData); 	
		 break;	
        case MfwWinResume:
             SEND_EVENT(ListData->Parent,LISTS_REASON_RESUME,0,(void*)ListData); 	
	 break;	
        case MfwWinSuspend:
	   SEND_EVENT(ListData->Parent,LISTS_REASON_SUSPEND,0,(void*)ListData); 	
	 break;	
	}
    return MFW_EVENT_CONSUMED;
}

static void lists_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    ListWinInfo    * ListData = (ListWinInfo *)win_data->user;
    T_MFW_HND      * l_parent;
    SHORT            l_id;

    TRACE_FUNCTION ("lists_exec_cb()");

    switch (event)
	{
	case E_INIT:
	case E_BACK:
	case E_RETURN:
	case E_ABORT:
	case E_EXIT:
	    /* defaultreaction */
	    l_parent = ListData->Parent;
	    l_id = ListData->id;

	    /* START: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
	    if (ListData->MenuListsAttr.header_toggle)
	    {
			if (g_title_timer)
			{
				tim_delete(g_title_timer);
				g_title_timer = NULL;
			}
	    }
	    /* END: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
	    winDelete (win);
	    FREE_MEMORY ((void*) ListData, sizeof (ListWinInfo));
	    SEND_EVENT (l_parent, event, l_id, NULL); /* forward event to parent */
	    break;
	default:
	    return;
	}
}



/*******************************************************************************

 $Function:  listsListCbMenu  

 $Description:	 List menu handler. (mfw callback function)

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
int listsListCbMenu(MfwEvt EventCode, MfwMnu *MenuHandle)
{
    int Status = LISTS_OK;
    T_MFW_HND   win      = mfw_parent (mfw_header());
    T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
    ListWinInfo* ListData = (ListWinInfo *)win_data->user;

    TRACE_FUNCTION("listsCbMenu()");

    switch (EventCode)
	{
        case E_MNU_SELECT:
	    ListData->ExitReason = E_MNU_SELECT;
	    ListData->MenuData.Reason = LISTS_REASON_SELECT;// MZ 14/2/01 E_MNU_SELECT;
	    ListData->MenuData.CursorPosition = MenuHandle->lCursor[MenuHandle->level];
	    ListData->MenuData.ListPosition  = MenuHandle->lCursor[MenuHandle->level]; /* True cursor position in list menu. */
	    /* SPR998 - SH - check if callback exists before calling it! */
		if (ListData->Callback)
	    	ListData->Callback(ListData->Parent, &ListData->MenuData);
	    break;

        case E_MNU_ESCAPE:
	    ListData->ExitReason = LISTS_REASON_BACK;
	    ListData->MenuData.Reason = LISTS_REASON_BACK;
	    /*NM, p023*/
	    ListData->MenuData.CursorPosition = MenuHandle->lCursor[MenuHandle->level];
	    ListData->MenuData.ListPosition  = MenuHandle->lCursor[MenuHandle->level]; /* True cursor position in list menu. */
	    /*NM, p023*/
	    
	    /* SPR998 - SH - check if callback exists before calling it! */
		if (ListData->Callback)
	    	ListData->Callback(ListData->Parent, &ListData->MenuData);
	    break;

        default:
	    Status = LISTS_FAIL;
	    TRACE_EVENT("Activation 77 error"); 
	    break;
	}

    /*
     * Free all allocated memory and handler
     */
	if (ListData->MenuData.autoDestroy)
	{
		/* START: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
		if (ListData->MenuListsAttr.header_toggle)
		{
			if (g_title_timer)
			{
				tim_delete(g_title_timer);
				g_title_timer = NULL;
			}
		}
		/* END: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
	    	winDelete (win);
    		FREE_MEMORY ((void*) ListData, sizeof (ListWinInfo));
   	}
    return Status;
}

void listsDestroy(T_MFW_HND win)
{
   	T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
   	ListWinInfo * ListData = (ListWinInfo *)win_data->user;

    	/* START: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
   	if (ListData->MenuListsAttr.header_toggle)
   	{
   		if(g_title_timer)
      		{
      			tim_delete(g_title_timer);
      			g_title_timer = NULL;
		}
	}
    	/* END: 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
	    
       winDelete (win);
   	FREE_MEMORY ((void*) ListData, sizeof (ListWinInfo));
    
}

