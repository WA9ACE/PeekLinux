/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookPhoneBook.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description

    This module provides the basic phone book functionality

  
********************************************************************************
 $History: MmiBookPhoneBook.c
    CRR: 25302 - xpradipg 10 Nov 2004
    Description: Should be able to select the number fro the phonebook while
    sending MMS/EMS.
    Solution: The new feature to select the number from phonebook while sending
    sending MMS/EMS.
    Solution: The new feature to select the number from phonebook while sending

	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX


  	Sep 29, 2004        REF: CRR 25041  xvilliva  
	Bug:		add new entry in the FDN , phone gets rebooted.
	Fix:		The global variable "menuFDN" is reset while menu is destroyed. 

    Jul 21, 2004        REF: CRR 13348  xvilliva
    Bug:	After selecting HPLMN manually, the idle screen is not shown.
    Fix:	After phonebook menu is created, we store the window handle in a 
    		global.


	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/


/******************************************************************************
                                                                              
                                Include Files
                                                                              
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
#include "MmiCall.h"
#include "Mmiicons.h"
#include "MmiIdle.h"

#include "MmiSoftKeys.h"
#include "MmiSounds.h"
#include "MmiIdle.h"
#include "MmiNetwork.h"
#include "mmiSat_i.h"
#include "MmiAoc.h"

#include "gdi.h"
#include "audio.h"

#include "cus_aci.h"
#include "mfw_ffs.h"
#include "MmiTimers.h"




#include "MmiBookShared.h"
#include "MmiBookController.h"

#include "mmiColours.h"

//Jul 21, 2004        REF: CRR 13348  xvilliva
//This is a global variable used to store the handle, when 
//phonebook menu is created.
extern T_MFW_HND gPhbkMenu;
//Sep 29, 2004        REF: CRR 25041  xvilliva  
extern int menuFDN;


/*******************************************************************************
                                                                              
                                Private Methods
                                                                              
*******************************************************************************/

/*******************************************************************************

 $Function:    	bookPhonebookDialog

 $Description:	Dialog function for the phone book application
 
 $Returns:		none

 $Arguments:	win, window handle
                event, event to be handled
				value, not used
				parameter, reason for call
 
*******************************************************************************/

static void bookPhonebookDialog( T_MFW_HND win, USHORT event, SHORT value, void *parameter )
{
    T_MFW_WIN   *win_data	= ((T_MFW_HDR *) win)->data;
    T_phbk      *data		= (T_phbk *) win_data->user;
	SHORT		*reason		= (SHORT *) parameter;

    TRACE_FUNCTION ("bookPhonebookDialog()");
    switch (event)
	{
		case PHBK_INIT:
		{
			/* Initialise the phone book application.
			   Note that we need to deal with a number of different
			   initialisation types, depending on the reason given
			*/
			memset( data->edt_buf_name,   '\0', sizeof( data->edt_buf_name   ) );
			memset( data->edt_buf_number, '\0', sizeof( data->edt_buf_number ) );

/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
			/* No longer needed for NEW_EDITOR - attributes are initialised when editors created*/
			bookSetEditAttributes( PHONEBOOK_DLG_EDITOR, COLOUR_EDITOR_XX, 0, edtCurBar1, 0,
				(char *) data->edt_buf_name,   MAX_ALPHA_LEN, &data->edt_attr_name   );
			bookSetEditAttributes( PHONEBOOK_DLG_EDITOR, COLOUR_EDITOR_XX, 0, edtCurBar1, 0,
				(char *) data->edt_buf_number, MAX_ALPHA_LEN, &data->edt_attr_number );
#endif

			/* deal with the different initialisation types
			*/
			if ((bookPhoneBookLoading()==BOOK_FAILURE)&&(( *reason )!=PhbkMainMenu))
			{
				T_DISPLAY_DATA display_info; 

				dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtPleaseWait, TxtNull , COLOUR_STATUS_WAIT);
				dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_CLEAR );

			    /* Call Info Screen
			    */
			    info_dialog( idle_get_window(), &display_info );

				bookPhonebookDestroy(data->win);
				return;
			}
			switch ( *reason )
			{
				case PhbkNormal:
				{
					/* Normal initialisation
					*/
					data->current.status.book	= bookActiveBook(READ);
					data->current.index			= 1;
                    data->menu_main_win         = bookMenuStart( data->win, bookPhonebookMenuAttributes(), *reason);
					data->root_win				= data->menu_main_win;
				}
				break;

				case PhbkMainMenu:
				{
					/* Normal initialisation
					*/
					data->current.status.book	= bookActiveBook(READ);
					data->current.index			= 1;
                    data->menu_main_win         = bookMenuStart( data->win, bookMainMenuAttributes(), *reason);
					data->root_win				= data->menu_main_win;
				}
				break;

				case PhbkReplyListNormal:
				{
				    TRACE_EVENT ("PhbkReplyListNormal");

					/* reply list
					*/
					bookCallListStart(win,PHB_LRN);
				}
				break;
				case PhbkMissedCallsListNormal:
				{
					/* reply list
					*/
					bookCallListStart(win,PHB_LMN);
				}
				break;

				case PhbkRedialListNormal:
				{
					/* Redial list
					*/
					bookCallListStart(win,PHB_LDN);
				}
				break;

				case PhbkNameEnterIdle:
				{
					/* Normal initialisation
					*/
					data->current.status.book	= bookActiveBook(READ);
					data->current.index			= 1;
                    data->menu_main_win         = bookMenuStart( data->win, bookPhonebookMenuAttributes(), *reason);
					data->root_win				= data->menu_main_win;
				    data->phbk->UpdateAction = ADD_FROM_IDLE;
				}
				break;

				case PhbkSearchNormal :
				{
					/* Search window
					*/
					data->root_win = data->phbk->search_win;
				}
				break;

				case PhbkFromSms :
				{
					/* Invoked from within SMS, need to remember this
					*/
					data->phbk->fromSMS = TRUE;
					data->root_win = data->phbk->search_win;
				    data->phbk->current.KindOfSearch = SEARCH_BY_NAME;
					data->phbk->current.status.book = bookActiveBook(READ);
					data->phbk->search_win = bookSearchStart( data->phbk->win );
					
				}
				break;
                            //CRR: 25302 - xpradipg 10 Nov 2004
                            //New case added to handle the retrival of number from MMS/EMS
				case PhbkFromMms :
				{
					/* Invoked from within MMS/EMS, need to remember this
					*/
					data->phbk->fromMmsEms = TRUE;
					data->root_win = data->phbk->search_win;
				       data->phbk->current.KindOfSearch = SEARCH_BY_NAME;
					data->phbk->current.status.book = bookActiveBook(READ);
					data->phbk->search_win = bookSearchStart( data->phbk->win );
					
				}
				break;

				/*SH*/
				case PhbkFromSmsSC :
				{
					/* Invoked from within SMS Service centre, need to remember this
					*/
					data->phbk->fromSMSSC = TRUE;
					data->root_win = data->phbk->search_win;
				    data->phbk->current.KindOfSearch = SEARCH_BY_NAME;
					data->phbk->current.status.book = bookActiveBook(READ);
					data->phbk->search_win = bookSearchStart( data->phbk->win );
					
				}
				break;

				case PhbkFromDivert:
					{
					/* Invoked from within Divert, need to remember this
					*/
					data->phbk->fromDivert = TRUE;
					data->root_win = data->phbk->search_win;
				    data->phbk->current.KindOfSearch = SEARCH_BY_NAME;
					data->phbk->current.status.book = bookActiveBook(READ);
					data->phbk->search_win = bookSearchStart( data->phbk->win );
					
					}

				break;
				/*SPR 1392, initialisation from call deflection*/
				case PhbkFromDeflect:
					{
					/* Invoked from within Divert, need to remember this
					*/
					data->phbk->fromDeflect = TRUE;
					data->root_win = data->phbk->search_win;
				    data->phbk->current.KindOfSearch = SEARCH_BY_NAME;
					data->phbk->current.status.book = bookActiveBook(READ);
					data->phbk->search_win = bookSearchStart( data->phbk->win );
					
					}

				break;

				default:
				{
					/* No default action required
					*/
				}
			}
		}
		break;
		case PHBK_SEND_NUMBER:
		    TRACE_EVENT ("PHBK_SEND_NUMBER");
			/***************************Go-lite Optimization changes Start***********************/
			//Aug 16, 2004    REF: CRR 24323   Deepa M.D
			TRACE_EVENT_P3 ("win %p parameter %s data->phbk %p",win,(char*)parameter,data->phbk);
			/***************************Go-lite Optimization changes end***********************/
			strncpy( data->phbk->edt_buf_number,   (char*)parameter, PHB_MAX_LEN );
		break;
		default:
		{
			/* No default action required
			*/
		}
	}
}









/*******************************************************************************

 $Function:    	bookPhonebookWindowCB

 $Description:	Window call back for the phone book application
 
 $Returns:		MFW_EVENT_CONSUMED if event is MfwWinVisible, otherwise
				MFW_EVENT_PASSED

 $Arguments:	e, event, w, window handle
 
*******************************************************************************/

static int bookPhonebookWindowCB ( MfwEvt e, MfwWin *w )
{
    TRACE_FUNCTION ("bookPhonebookWindowCB()");
	return ( e == MfwWinVisible ) ? MFW_EVENT_CONSUMED : MFW_EVENT_PASSED;
}








/*******************************************************************************

 $Function:    	bookPhonebookCreate

 $Description:	Create the phone book application
 
 $Returns:		Handle of the newly created window or NULL if error

 $Arguments:	parent, handle of parent window
 
*******************************************************************************/

static T_MFW_HND bookPhonebookCreate( MfwHnd parent )
{
	T_MFW_WIN   *win_data;
	T_phbk		*data;

	TRACE_FUNCTION ("bookPhonebookCreate()");

	if ( ( data = (T_phbk *) ALLOC_MEMORY( sizeof( T_phbk ) ) ) == NULL )
		return NULL;

	if ( ( data->win = win_create( parent, 0, E_WIN_VISIBLE, (T_MFW_CB) bookPhonebookWindowCB ) ) == NULL )
	{
		FREE_MEMORY( (void *) data, sizeof( T_phbk ) );
		return NULL;
	}

    /* Create window handler
    */
    data->mmi_control.dialog   = (T_DIALOG_FUNC) bookPhonebookDialog;
	data->mmi_control.data     = data;
    win_data                   = ((T_MFW_HDR *)data->win)->data;
 	win_data->user             = (void *)data;
	winShow(data->win);

	/* initialise the options structure
	*/
	data->root_win					= 0;
    data->menu_main_win				= 0;
	data->calls_list_win			= 0;
	data->menu_options_win			= 0;
	data->menu_options_win_2		= 0;
	data->menu_call_options_win		= 0;
	data->menu_call_options_win_2	= 0;
	data->name_details_win			= 0;
	data->search_win				= 0;
	data->fromSMS					= FALSE;
	data->fromSMSSC					= FALSE;
	data->fromDivert			    = FALSE;
	data->fromDeflect			    = FALSE;/*SPR 1392*/
	data->parent_win				= parent;
	data->phbk						= data;

	/* return window handle
    */
    return data->win;
}





/*******************************************************************************
                                                                              
                                Public methods
                                                                              
*******************************************************************************/

/*******************************************************************************

 $Function:    	bookPhonebookStart

 $Description:	Starts up the phone book application
 
 $Returns:		Handle of window or NULL if error

 $Arguments:	parent, window, reason, for invocation
 
*******************************************************************************/

T_MFW_HND bookPhonebookStart( MfwHnd parent, int reason )
{
	T_MFW_HND win;

    if ( ( win = bookPhonebookCreate( parent ) ) != NULL )
    {
        SEND_EVENT( win, PHBK_INIT, 0, (int *) &reason );
	 if(reason == PhbkMainMenu)
    		 gPhbkMenu = win;//Jul 21, 2004        REF: CRR 13348  xvilliva
    }
    return win;
}






/*******************************************************************************

 $Function:    	bookPhonebookDestroy

 $Description:	Destroys the phone book application
 
 $Returns:		none

 $Arguments:	window, to be destroyed
 
*******************************************************************************/

void bookPhonebookDestroy( MfwHnd window )
{
	T_MFW_WIN	*win  = ((T_MFW_HDR *)window)->data;
	T_phbk		*data = (T_phbk *) win->user;

	TRACE_FUNCTION ("bookPhonebookDestroy()");

	if ( data )
	{
		/* need to clean up all of the subordinate windows which
		   may have been created during the phone book application
		*/
		data->root_win = 0;
		if (data->search_win)
			bookSearchDestroy( data->search_win );

		if (data->menu_main_win)
			bookMenuDestroy(data->menu_main_win);

		if (data->calls_list_win)
			bookCallListDestroy(data->calls_list_win);

		if (data->menu_options_win)
			bookMenuDestroy(data->menu_options_win);

		if (data->menu_options_win_2)
			bookMenuDestroy(data->menu_options_win_2);

		if (data->menu_call_options_win)
			bookMenuDestroy(data->menu_call_options_win);

		if (data->menu_call_options_win_2)
		{
			menuFDN = 0 ;//  	Sep 29, 2004        REF: CRR 25041  xvilliva  
			bookMenuDestroy(data->menu_call_options_win_2);
		}

		if (data->name_details_win)
			bookDetailsDestroy(data->name_details_win);

		if (data->input_name_win)
			bookInputDestroy(data->input_name_win);

		if (data->input_number_win)
			bookInputDestroy(data->input_number_win);

		/* delete the window and free any allocated memory
		*/
		winDelete (data->win);
		FREE_MEMORY( (void *) data, sizeof( T_phbk ) );
	}
	//Jul 21, 2004        REF: CRR 13348  xvilliva
	//While we destroy the menu assign the handle to NULL.
	gPhbkMenu = NULL;

}


/*******************************************************************************
                                                                              
                                End Of File
                                                                              
*******************************************************************************/
