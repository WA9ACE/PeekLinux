/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI                                                    
 $Module:		Frequency band selection
 $File:		    MmiBand.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    03/04/02                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the functionality for menu options to select and display the current 
    band used.
                        
********************************************************************************
 $History: MmiBand.c

	03/04/02			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/




/*******************************************************************************
                                                                              
                                Include files
                                                                              
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
#include "mfw_sys.h"

#include "cus_aci.h"

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_lng.h"
#include "mfw_tim.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_phb.h"
#include "mfw_cm.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_sat.h"
#include "mfw_ss.h" /*for convert*/
#include "mfw_phb.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_sms.h"
#include "mfw_cphs.h"
#include "mfw_sat.h"
#include "Mfw_band.h"
#include "mfw_ffs.h"

#include "dspl.h"

#include "MmiMmi.h"
#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiBand.h"
#include "MmiCPHS.h"
#include "mmiColours.h"

/***********************************Macros**********************************************/

#define  	MAX_NUMBER_OF_FREQS 5
#define 	NO_OF_BAND_MODES 2
#define 	BAND_INIT 1
#define		BAND_DESTROY 2



/*********************Structure Definitions********************************************************/
typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND        freq_win; 				   /* MFW win handler      */
  T_MFW_HND        parent;
  UBYTE				available_freqs;
  UBYTE				selected_freqs;			/* SPR#998 - SH - Used to store list of selected frequencies */
  T_MFW_BAND_MODE	mode;					/* SPR#998 - SH */
  UBYTE				type[MAX_NUMBER_OF_FREQS];
  ListMenuData     * menu_list_data;
  UBYTE 			no_of_entries;
} T_FREQ_INFO;


static const MfwMnuAttr  Freq_menuAttrib =
{
    &melody_menuArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu       */
    (U8)-1,                                 /* use default font         */
    NULL,                               /* with these items         */
    0,                                   /* number of items     */
	COLOUR_LIST_SMS,
	TxtNull,
	MNUATTRSPARE};

/* List of all possible frequencies */
	
static const int band_list[MAX_NUMBER_OF_FREQS] = {
	MFW_BAND_GSM_900,
	MFW_BAND_DCS_1800,
	MFW_BAND_PCS_1900,
	MFW_BAND_E_GSM,
	MFW_BAND_GSM_850
};	

/* SPR#998 - SH - Static handle to allow callback functions to access data */
static T_FREQ_INFO *current_data = NULL;

/*********************************Function Prototypes*****************************/

char*				Freq2Str(int freq);

int					Freq2StrID(int freq);
static T_MFW_HND 	mmi_band_build_freq_list( MfwHnd parent_window, UBYTE available_freqs);
static int			mmi_band_display_status(T_MFW_HND parent);
static int			mmi_band_display_status_cb(T_MFW_HND parent, USHORT identifier, USHORT reason);
void				mmi_band_freq_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
void				mmi_band_freq_list_cb(T_MFW_HND * Parent, ListMenuData * ListData);
static void			mmi_band_freq_list_destroy(MfwHnd own_window);
static int			mmi_band_freq_win_cb (MfwEvt e, MfwWin *w);
/* SPR#998 - SH - Functions used by new checkbox menus */
int					mmi_band_menu_func(MfwMnu* menu, MfwMnuItem* item);
USHORT				mmi_band_menu_flag( struct MfwMnuTag *menu, struct MfwMnuAttrTag *attr, struct MfwMnuItemTag *item );


/************************************Public Functions************************/


/*************************************************************************

 $Function:    	mmi_band_radio_mode_change_auto

 $Description:	Changes multiband mode to automatic.  Selected from menu.
 
 $Returns:		status int

 $Arguments:	menu and menu item (not used)
 				
 
*******************************************************************************/

int mmi_band_radio_mode_change_auto(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent = mfwParent( mfw_header());
//	T_MFW_BAND_MODE		mode;  // RAVI

	TRACE_FUNCTION("mmi_band_radio_mode_change_auto");
	
	if (band_radio_mode_switch(MFW_BAND_MODE_Auto, 0) >= MFW_BAND_OK)
	{
		ShowMessage(parent, TxtDone, TxtNull);
	}
	else
		ShowMessage(parent, TxtFailed, TxtNull);
	
	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	mmi_band_radio_mode_change_manual

 $Description:	Gets a list of available manual bands.  Selected from menu.
 
 $Returns:		status int

 $Arguments:	menu and menu item (not used)
 				
 
*******************************************************************************/

int mmi_band_radio_mode_change_manual(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND			parent = mfwParent( mfw_header());
	T_MFW_BAND_MODE		maxMode;
	UBYTE				available_freqs;
	T_MFW_HND			win;
	
	if (band_get_radio_modes(&maxMode, &available_freqs) >= MFW_BAND_OK)
	{
		if (maxMode >= MFW_BAND_MODE_Manual)		/* Make sure manual is an accepted mode */
		{
			win = mmi_band_build_freq_list(parent, available_freqs);
			if (win NEQ NULL)
			{
				SEND_EVENT (win, BAND_INIT, 0, 0);
			}
		else
			ShowMessage(parent, TxtManual, TxtNotSupported);
		}
	}
	
	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	mmi_band_radio_mode_info

 $Description:	Get current band setting
 
 $Returns:		status int

 $Arguments:	menu and menu item (not used)
 				
 
*******************************************************************************/

int mmi_band_radio_mode_info(MfwMnu* m, MfwMnuItem* i)
{	T_MFW_HND			parent = mfwParent( mfw_header());

	mmi_band_display_status(parent);
			
	return MFW_EVENT_CONSUMED;
}


/*******************************Private Functions**********************************/



/*******************************************************************************

 $Function:    	Freq2Str

 $Description:	Converts the band enum into an appropraite string
 
 $Returns:		string

 $Arguments:	Band (integer)
 				
 
*******************************************************************************/

char* Freq2Str(int freq)
{
	switch (freq)
	{
		case MFW_BAND_DCS_1800:	 return MmiRsrcGetText(TxtDCS_1800); //break;  // RAVI
		case MFW_BAND_PCS_1900:	 return MmiRsrcGetText(TxtPCS_1900); //break;  // RAVI
		case MFW_BAND_E_GSM:	return MmiRsrcGetText(TxtE_GSM); //break;  // RAVI
		case MFW_BAND_GSM_850:	return MmiRsrcGetText(TxtGSM_850); // break; // RAVI
		case MFW_BAND_GSM_900:	return MmiRsrcGetText(TxtGSM_900); // break; // RAVI
		default: return MmiRsrcGetText(TxtError); // break; // RAVI
	}
}


/*******************************************************************************

 $Function:    	Freq2Str

 $Description:	Converts the band enum into an appropraite string ID
 
 $Returns:		string ID

 $Arguments:	Band (integer)
 				
 
*******************************************************************************/

int Freq2StrID(int freq)
{
	switch (freq)
	{
		case MFW_BAND_DCS_1800:	 return TxtDCS_1800; //break;  // RAVI
		case MFW_BAND_PCS_1900:	 return TxtPCS_1900; //break;  // RAVI
		case MFW_BAND_E_GSM:	return TxtE_GSM;  //break;  // RAVI
		case MFW_BAND_GSM_850:	return TxtGSM_850; // break; // RAVI
		case MFW_BAND_GSM_900:	return TxtGSM_900; //break;  // RAVI
		default: return TxtError; //break; // RAVI
	}
}


/*******************************************************************************

 $Function:    	mmi_band_display_status
 $Description:	Displays the current band status
 $Returns:		none
 $Arguments:	parent - parent window
 
*******************************************************************************/

static int mmi_band_display_status(T_MFW_HND parent)
{
	/* SPR#998 - SH - Now store mode in data->mode */
    T_MFW_WIN		*win_data = ((T_MFW_HDR *) parent)->data;
    T_FREQ_INFO		*data = (T_FREQ_INFO *)win_data->user;
	
	if (band_get_current_radio_mode(&data->mode, &data->selected_freqs) >= MFW_BAND_OK)
	{
		if (data->mode == MFW_BAND_MODE_Auto)
		{
			ShowMessage(parent, TxtAutomatic, TxtNull);
		}
		else
		{
			ShowMessage(parent, TxtManual, TxtNull);
		}
	}
	else
	{
		ShowMessage(parent, TxtReadError, TxtNull);
	}
		
	return 1;  // RAVI - Changed return to return 1.
}

#if(0) /* x0039298 - Lint warning fix */
/*******************************************************************************

 $Function:    	mmi_band_display_status_cb
 $Description:	Callback for status list window - currently does nothing
 $Returns:		none
 $Arguments:	parent - parent window
 				identifier - length of status string
 				reason - not used
 
*******************************************************************************/

static int mmi_band_display_status_cb(T_MFW_HND parent, USHORT identifier, USHORT reason)
{
	return 1;  // RAVI - Changed return to return 1.
}
#endif

/*******************************************************************************

 $Function:    	mmi_band_build_freq_list
 $Description:	Creates window for frequency list
 $Returns:		window created
 $Arguments:	parent window
 
*******************************************************************************/

static T_MFW_HND  mmi_band_build_freq_list( MfwHnd parent_window, UBYTE available_freqs)
{
	T_FREQ_INFO	*data = (T_FREQ_INFO *)ALLOC_MEMORY (sizeof (T_FREQ_INFO));
 	T_MFW_WIN	*win;
// 	int			index;  // RAVI

 	TRACE_FUNCTION("mmi_band_build_freq_list()");
    /*
     * Create window handler
     */

    data->freq_win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)mmi_band_freq_win_cb);
    if (data->freq_win EQ NULL)
	{
		return NULL;
	}

	current_data = data;	/* SPR#998 - SH - Store handle for data; used by callbacks */
	
    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog	= (T_DIALOG_FUNC)mmi_band_freq_exec_cb;
    data->mmi_control.data		= data;
    win							= ((T_MFW_HDR *)data->freq_win)->data;
    win->user					= (void *)data;
    data->parent				= parent_window;
	data->available_freqs		= available_freqs;
		
	winShow(data->freq_win);
    /*
     * return window handle
     */

	return data->freq_win;
}


/*******************************************************************************

 $Function:    	mmi_band_freq_exec_cb

 $Description:	Exec callback function of the information Numbers window
 
 $Returns:		none

 $Arguments:	win - window handler
 				event - mfw event
 				parameter - optional data.
 
*******************************************************************************/

void mmi_band_freq_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN		*win_data = ((T_MFW_HDR *) win)->data;
    T_FREQ_INFO		*data = (T_FREQ_INFO *)win_data->user;
	UBYTE			numberOfEntries;
	int				index;
	T_MFW_BAND_MODE maxMode;
//	char			*menuItem;  // RAVI
	/* SPR#998 - SH - Remove code that implements custom checkbox implementaton,
	 * as checkboxes now handled by mfw */
	
    TRACE_FUNCTION ("mmi_band_freq_exec_cb()");
	
    switch (event)
	{
		case BAND_INIT:

			TRACE_EVENT("mmi_band_freq_exec_cb() Event:BAND_INIT");

			 /* initialization of administrative data */

			data->menu_list_data = (ListMenuData *)ALLOC_MEMORY(sizeof(ListMenuData));

			if(!data->menu_list_data)
			{
				TRACE_EVENT("Failed memory alloc 1 ");
				return;
			}

			band_get_radio_modes(&maxMode, &data->available_freqs);

			/* Run through the list of possible frequencies first to obtain
			 the number of frequencies permitted (how many will be in the menu) */
			/* SPR#998 - SH - Removed custom checkbox implementation */
			
			numberOfEntries = 0;
			
			for (index=0; index<MAX_NUMBER_OF_FREQS; index++)
			{
				if (data->available_freqs & band_list[index])
				{
					data->type[numberOfEntries] = band_list[index];
					numberOfEntries++;
				}
			}

			/* If there are no entries, show "Empty List" message and exit */
			
			if (numberOfEntries==0)
			{
				ShowMessage(data->parent, TxtEmptyList, TxtNull);
				mmi_band_freq_list_destroy(win);
				return;
			}

			/* Get the status of all the frequencies */
			/* SPR#998 - SH - Use data->mode */
			
			if (band_get_current_radio_mode(&data->mode, &data->selected_freqs) >= MFW_BAND_OK)
			{
				/* If we're not in manual choose manual */
				
				if (data->mode!=MFW_BAND_MODE_Manual)
				{
					data->selected_freqs = data->available_freqs;	/* SH - default when choosing manual is all */
					band_radio_mode_switch(MFW_BAND_MODE_Manual, data->selected_freqs);
				}
			
				/* Allocate the memory for the list structure */
				
				data->no_of_entries = numberOfEntries;
				
				data->menu_list_data->List = (T_MFW_MNU_ITEM *)ALLOC_MEMORY( numberOfEntries * sizeof(T_MFW_MNU_ITEM) );

				if(!data->menu_list_data->List)
				{
					TRACE_EVENT("Failed memory alloc 2");
					return;
				}

				/* Allocate memory for the menu texts, and populate the list structure */
				
				numberOfEntries = 0;
				
				for (index=0; index<MAX_NUMBER_OF_FREQS; index++)
				{					
					if (data->available_freqs & band_list[index])
					{
						/* SPR#998 - SH - Removed code, checkboxes now handled by MFW */
						mnuInitDataItem(&data->menu_list_data->List[numberOfEntries]);
						/* Here we specify the function that checks and unchecks the checkboxes */
						data->menu_list_data->List[numberOfEntries].func = mmi_band_menu_func;	
						data->menu_list_data->List[numberOfEntries].str  = (char *)Freq2StrID(band_list[index]);
						/* Here we specify the function that returns the checkbox status of any menu item */
						data->menu_list_data->List[numberOfEntries].flagFunc = mmi_band_menu_flag;
						numberOfEntries++;
					}
				}

		   		data->menu_list_data->ListLength =numberOfEntries;
				data->menu_list_data->ListPosition = 1;
				data->menu_list_data->CursorPosition = 1;
				data->menu_list_data->SnapshotSize = numberOfEntries;
				data->menu_list_data->Font = 0;
				data->menu_list_data->LeftSoftKey = TxtSoftSelect;
				data->menu_list_data->RightSoftKey = TxtSoftBack;
				data->menu_list_data->KeyEvents = KEY_ALL;
				data->menu_list_data->Reason = 0;
				data->menu_list_data->Strings = FALSE;		/* SPR#998 - SH - We no longer alter the menu strings */
		 		data->menu_list_data->Attr   = (MfwMnuAttr*)&Freq_menuAttrib;
				data->menu_list_data->autoDestroy    = FALSE;
				
		 		/* Create the dynamic menu window */
		  	    listDisplayListMenu(win, data->menu_list_data,(ListCbFunc)mmi_band_freq_list_cb,0);
			}
	  		else		/* We haven't been able to read the status */
			{
				ShowMessage(win, TxtReadError, TxtNull);
			}
		
			break;

		case BAND_DESTROY:
			TRACE_EVENT("mmi_band_freq_exec_cb() Event:BAND_DESTROY");
		    /*     
		     * Free Memory
		     */

		     /* SPR#998 - SH - No longer free allocated strings here */
		     
			numberOfEntries = data->no_of_entries;

			if (data->menu_list_data != NULL)
			{
				FREE_MEMORY ((void *)data->menu_list_data->List, numberOfEntries * sizeof (T_MFW_MNU_ITEM));
			    FREE_MEMORY ((void *)data->menu_list_data, sizeof (ListMenuData));			    
			}

			FREE_MEMORY ((void *)data, sizeof (T_FREQ_INFO));
			break;
			
		default:
		    break;
	}
}


/*******************************************************************************

 $Function:    	mmi_band_freq_list_cb

 $Description:	Callback function for the info num list.
 
 $Returns:		none
 
 $Arguments:	Parent - parent window.
 				ListData - Menu item list
*******************************************************************************/

void mmi_band_freq_list_cb(T_MFW_HND * Parent, ListMenuData * ListData)
{
	/* SPR#998 - SH - Selecting a checkbox is handled now by the checkbox functions,
	 * so much less is required here */
	 
    T_MFW_WIN			*win_data	= ((T_MFW_HDR *)Parent)->data;
    T_FREQ_INFO 		*data		= (T_FREQ_INFO *)win_data->user;

	TRACE_FUNCTION("mmi_band_freq_list_cb");
	
	if ((ListData->Reason EQ LISTS_REASON_BACK) || (ListData->Reason EQ LISTS_REASON_CLEAR))
	{
		listsDestroy(ListData->win);
	    mmi_band_freq_list_destroy(data->freq_win);
	}
	else if(ListData->Reason EQ LISTS_REASON_SELECT)
	{
		winShow(Parent);
	}
}


/*******************************************************************************

 $Function:    	mmi_band_freq_list_destroy

 $Description:	Destroy the info num window.
 			
 $Returns:		none

 $Arguments:	own_window- current window
*******************************************************************************/

static void mmi_band_freq_list_destroy(MfwHnd own_window)
{
    T_MFW_WIN		*win_data;
    T_FREQ_INFO		*data;
	
    TRACE_FUNCTION ("mmi_band_freq_list_destroy()");
	
    if (own_window)
	{
	    win_data = ((T_MFW_HDR *)own_window)->data;
	    data = (T_FREQ_INFO *)win_data->user;

	    if (data)
		{
		    /*     
		     * Free Memory
		     */
			SEND_EVENT (own_window, BAND_DESTROY, 0, 0);
		    /*
		     * Delete WIN handler
		     */ 
		    win_delete (own_window);
		    
		    current_data = NULL;	/* SPR#998 - SH - Set the data handle to NULL */
		}
	    else
		{
		    TRACE_EVENT ("mmi_band_freq_list_destroy() called twice");
		}
	}
}


/*******************************************************************************

 $Function:    	mmi_band_freq_win_cb

 $Description:	Window callback function for the info numbers window.
 
 $Returns:		none

 $Arguments:	w - mfw window handler
 				e - mfw event
 				
*******************************************************************************/

static int mmi_band_freq_win_cb (MfwEvt e, MfwWin *w)    /* yyy window event handler */
{
	TRACE_FUNCTION ("mmi_band_freq_win_cb()");
	switch (e)
	{
		case MfwWinVisible:  /* window is visible  */
			break;
		case MfwWinFocussed: /* input focus / selected   */	
		case MfwWinDelete:   /* window will be deleted   */   
			
		default:	    
			return MFW_EVENT_REJECTED;
	}
	return MFW_EVENT_CONSUMED;
}



/*******************************************************************************

 $Function:     mmi_band_menu_func

 $Description:  SPR#998 - SH - Added
 			  Select or deselect a checkbox item.  The appropriate band function is
 			  called, to ensure that the selection is valid.

 $Returns:    None.

 $Arguments:  m, ma, mi - not used

*******************************************************************************/

int mmi_band_menu_func(MfwMnu* menu, MfwMnuItem* item)
{	
    T_FREQ_INFO *data = current_data;
    
	TRACE_FUNCTION("mmi_band_menu_func");
	
	data->selected_freqs ^= data->type[menu->lCursor[menu->level]];	/* Toggle this band */
		
	if (band_radio_mode_switch(MFW_BAND_MODE_Manual, data->selected_freqs) >= MFW_BAND_OK)
	{
		mnuUpdate(menu);
	}
	else
	{
		data->selected_freqs ^= data->type[menu->lCursor[menu->level]]; /* Undo change */
		ShowMessage(data->freq_win, TxtFailed, TxtNull);
	}	   

	return 1;
}


/*******************************************************************************

 $Function:     mmi_band_menu_flag

 $Description:  Attribut function for menu entries.

 $Returns:    Checked or unchecked

 $Arguments:  m, ma, mi - not used

*******************************************************************************/

USHORT mmi_band_menu_flag( struct MfwMnuTag *menu, struct MfwMnuAttrTag *attr, struct MfwMnuItemTag *item )
{
	T_FREQ_INFO *data = current_data;	/* SPR#998 - SH - Get data from static handle */
 	int currentOption;

	TRACE_FUNCTION("mmi_band_menu_flag()");
 	TRACE_EVENT_P1("Freq info data: %x", data);
 	
	/* Identify current menu option */

	currentOption = mnuIdentify(attr, item);

	/* If we find it, work out if it's checked or not */
	
	if (currentOption>-1)
	{
		if (data->selected_freqs & data->type[currentOption])
			return MNU_ITEM_CHECKED;
	 	else
	 		return MNU_ITEM_UNCHECKED;
	}
	return 0;
}

