/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookDetailsWindow.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

	Provides the details handlig for the phone book
 
                        
********************************************************************************
 $History: MmiBookDetailsWindow.h

	25/10/00			Original Condat(UK) BMI version.	
	   
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

#include "mmiColours.h"



/*******************************************************************************
                                                                              
                                Private Methods
                                                                              
*******************************************************************************/


/*******************************************************************************

 $Function:    	bookDetailsDialog

 $Description:	Dialog function for the details window, doesn't currently do
				anything
 
 $Returns:		None

 $Arguments:	win, window handle
                event, event to be handled
				value, not used
				parameter, not used
 
*******************************************************************************/

static void bookDetailsDialog ( T_MFW_HND win, USHORT event, SHORT value, void *parameter )
{
	T_MFW_WIN		*win_data	= ((T_MFW_HDR *)win)->data;
	tBookStandard	*data = (tBookStandard *)win_data->user;
	T_MFW_PHB_ENTRY *entry;
	/* SPR#1428 - SH - New Editor changes
	 * Insert the ascii or unicode name into the buffer */
#ifdef NEW_EDITOR
	T_ATB_TEXT		nameText;
	T_ATB_TEXT		numberText;

	TRACE_FUNCTION("bookDetailsDialog");

	entry = &data->phbk->current.entry[ data->phbk->current.selectedName ];
	/*SPR 2175, no need to do any GSM->ASCII conversions, removed conversion code*/
	

	numberText.dcs = ATB_DCS_ASCII;
	numberText.data = entry->number;
	ATB_string_Length(&numberText);

	if ( entry->name.data[0] ==0x80)
	{
		
		/* UNICODE */
		TRACE_EVENT("***UNICODE OUTPUT***");
		nameText.dcs = ATB_DCS_UNICODE;
		nameText.data = &entry->name.data[2];
	}
	else
	{
		nameText.dcs = ATB_DCS_ASCII;
		nameText.data = entry->name.data;
	}

	ATB_string_Length(&nameText);

	ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
	ATB_edit_InsertString(data->editor, &nameText);			/* Insert name string */
	ATB_edit_Char(data->editor, ctrlEnter, FALSE);			/* Insert carriage return */
	ATB_edit_InsertString(data->editor, &numberText);		/* Insert number string */
	ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */

	/* Text has changed, so refresh word wrap */
	ATB_edit_Refresh(data->editor);

	win_show(data->win);

#endif /* NEW_EDITOR */

}




/*******************************************************************************

 $Function:    	bookDetailsWindowCB

 $Description:	Window event handler
 
 $Returns:		MFW_EVENT_CONSUMED for the window visible event,
				MFW_EVENT_PASSED otherwise

 $Arguments:	e, event, w, window handle
 
*******************************************************************************/

static int bookDetailsWindowCB ( MfwEvt e, MfwWin *w )
{
	tBookStandard	*data = (tBookStandard *)w->user;
	
/* x0045876, 14-Aug-2006 (WR - "ptr" was declared but never referenced) */
#ifndef NEW_EDITOR
	T_MFW_PHB_ENTRY *ptr; // dat;
#endif

//	char debug[50];   // RAVI

	TRACE_FUNCTION("bookDetailsWindowCB");
	switch( e )
    {
        case MfwWinVisible:
		{
			/* handle the visible event, start by shoing the default information
			*/
			MmiBookShowDefault();

			/* SPR#1428 - SH - New Editor: display editor here rather than filling up the information.  Do that in
			 * dialog function above. */
			 
#ifdef NEW_EDITOR
			ATB_edit_Show(data->editor);
			resources_setTitleColour(COLOUR_EDITOR);

			/* TITLE */
		
			dspl_Clear(0,0, SCREEN_SIZE_X-1, data->editor->attr->win_size.py-1);
			PROMPT(0,0,0,TxtDetails);
#else /* NEW_EDITOR */
			/* Build and show the editor
			*/
			ptr = &data->phbk->current.entry[ data->phbk->current.selectedName ];
#ifdef NO_ASCIIZ

#ifdef EASY_TEXT_ENABLED
				/*MC SPR 1242, changed macros to function calls e.g FIRST_LINE_CHINESE->Mmi_layout_first_line()*/
				if ( ptr->name.data[0] ==0x80)//if unicode
				{
					TRACE_EVENT("***UNICODE OUTPUT***");
					dspl_TextOut(0,0,DSPL_TXTATTR_UNICODE,(char *)&ptr->name.data[2]);
					if ((ptr->name.data[MAX_LINE+2] != 0) && ptr->name.data[MAX_LINE+3] != 0)
						dspl_TextOut(0,Mmi_layout_line(1),DSPL_TXTATTR_UNICODE,(char *)&(ptr->name.data[MAX_LINE+2]));
					dspl_TextOut(0,Mmi_layout_line(2),0,(char *)ptr->number);
					if (strlen((char*)ptr->number) > MAX_LINE)
						dspl_TextOut(0,Mmi_layout_line(1)+numberLineHeight(),0, (char*)&ptr->number[MAX_LINE]);
				}
				else
#endif /* EASY_TEXT_ENABLED */
				{
					// convert from GSM to Alpha characters
					bookGsm2Alpha( (UBYTE *) ptr->name.data );

					sprintf( data->edtBuf,"%s%s%s", (char *) ptr->name.data, "\n", (char *) ptr->number);
					edtShow(data->edt);
				}	
#else /* NO_ASCIIZ */	
				{
					sprintf( data->edtBuf,"%s%s%s", (char *) ptr->name, "\n", (char *) ptr->number);
					edtShow(data->edt);
				}
#endif /* NO_ASCIIZ */
#endif /* NEW_EDITOR */
			/* pop up the soft keys
			*/
			displaySoftKeys( TxtSoftCall, /* TxtSoftOptions */ TxtSoftBack );
		}
		break;

        default:
		{
			/* default handler just indicate event not handled
			*/
			return MFW_EVENT_PASSED;
		}
    }

    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	bookDetailsKbdCB

 $Description:	Keyboard handler
 
 $Returns:		MFW_EVENT_CONSUMED always

 $Arguments:	e, event, k, key handle
 
*******************************************************************************/

static int bookDetailsKbdCB ( MfwEvt e, MfwKbd *k )
{
    T_MFW_HND       win			= mfwParent( mfw_header() );
    T_MFW_WIN		*win_data	= ((T_MFW_HDR *) win)->data;
    tBookStandard	*data		= (tBookStandard *) win_data->user;
	tMmiPhbData		*Current    = &data->phbk->current;

	TRACE_FUNCTION("bookDetailsKbdCB()");

	/* Handle the key events
	*/
    switch (k->code)
    {
        case KCD_MNUUP:
		break;

        case KCD_MNUDOWN:
		break;

		case KCD_MNUSELECT:
		case KCD_CALL:
        case KCD_LEFT:
		{
			/* select to make the call
			*/
			callNumber( Current->entry[ Current->selectedName ].number );
			bookPhonebookDestroy( data->phbk->win );
		}
		break;

        case KCD_RIGHT:
		{
			/* start up the options menu
			*/
			//Cancel this screen
			bookDetailsDestroy( data->win );
		}
		break;

        case KCD_HUP:
		{
			/* Cancel this menu
			*/
			bookDetailsDestroy( data->win );
		}
		break;

        default:
		{
			/* default handling is to get us out of here
			*/
			memset( data->edtBuf, '\0', sizeof( data->edtBuf ) );
			sprintf( data->edtBuf, "%s%c", (char *) Current->entry[ Current->selectedName ].number, editControls[ k->code ] );
			idleDialBuffer( data->edtBuf );
			bookPhonebookDestroy( data->phbk->win );
		}
		break;
    }

    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	bookDetailsKbdLongCB

 $Description:	Keyboard Long Press event handler
 
 $Returns:		MFW_EVENT_CONSUMED always

 $Arguments:	e, event, k, keyboard handle
 
*******************************************************************************/

static int bookDetailsKbdLongCB( MfwEvt e, MfwKbd *k )
{
    T_MFW_HND		win			= mfwParent( mfw_header() );
    T_MFW_WIN		*win_data	= ((T_MFW_HDR *)win)->data;
    tBookStandard	*data		= (tBookStandard *) win_data->user;

	/* hand a long clear event only
	*/
	if ( ( e & KEY_CLEAR ) && ( e & KEY_LONG ) )
		bookDetailsDestroy( data->win );

	return MFW_EVENT_CONSUMED;
}




/*******************************************************************************

 $Function:    	bookDetailsCreate

 $Description:	create an instance of the details window
 
 $Returns:		handle of newly created window, or NULL if we fail to create

 $Arguments:	parent, handle of parent window
 
*******************************************************************************/

static T_MFW_HND bookDetailsCreate(MfwHnd parent)
{
	T_MFW_WIN		*parent_win_data = ( (T_MFW_HDR *) parent )->data;
	T_phbk			*phbk = (T_phbk *)parent_win_data->user;
	T_MFW_WIN		*win_data;
	tBookStandard	*data;

	TRACE_FUNCTION ("bookDetailsCreate()");

	/* allocate memory for our control block
	*/
	if ( ( data = (tBookStandard *) ALLOC_MEMORY( sizeof( tBookStandard ) ) ) == NULL )
		return NULL;

	/* Create the window if we can
	*/
	if ( ( data->win = win_create( parent, 0, E_WIN_VISIBLE, (T_MFW_CB) bookDetailsWindowCB ) ) == NULL )
	{
		FREE_MEMORY( (void *)data, sizeof( tBookStandard ) );
		return NULL;
	}

	/* Okay, we have created the control block and the window, so
	   we now need to configure the dialog and data pointers
	*/
    data->mmi_control.dialog	= (T_DIALOG_FUNC) bookDetailsDialog;
    data->mmi_control.data		= data;
    win_data					= ((T_MFW_HDR *)data->win)->data;
 	win_data->user				= (void *) data;
	data->phbk					= phbk;
	data->parent_win			= parent;

	/* create keyboards and menus for our window
	*/
	data->kbd		= kbdCreate( data->win, KEY_ALL,            (MfwCb) bookDetailsKbdCB );
    data->kbd_long	= kbdCreate( data->win, KEY_ALL | KEY_LONG, (MfwCb) bookDetailsKbdLongCB );

	/* 1945 MZ Initialise the edtBuf   */
	memset(data->edtBuf,'\0', STANDARD_EDITOR_SIZE );
	
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	AUI_edit_SetAttr( &data->editor_attr, BOOKDETAILS_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT, ED_MODE_READONLY, ED_CURSOR_NONE, ATB_DCS_UNICODE/*SPR1752*/, (UBYTE *)data->edtBuf, STANDARD_EDITOR_SIZE);
	data->editor = ATB_edit_Create( &data->editor_attr, 0 );
	SEND_EVENT(data->win, 0, 0, 0);
	ATB_edit_Init(data->editor);
#else /*NEW_EDITOR*/
	// API - 08-01-03 - 1540 - change edtCurBar1 to edtCurNone
	bookSetEditAttributes( BOOKDETAILS_EDITOR, COLOUR_EDITOR_XX, 0, edtCurNone, 0, data->edtBuf, STANDARD_EDITOR_SIZE, &data->attr );
	data->edt = edtCreate( data->win, &data->attr, 0, 0 );
	edtUnhide( data->edt );
#endif /*NEW_EDITOR*/

	/* allow the window and menu to be created
	*/
	mnuUnhide(data->menu);
	winShow(data->win);

	/* And return the handle of the newly created window
	*/
    return data->win;
}




/*******************************************************************************
                                                                              
                                Public Methods
                                                                              
*******************************************************************************/


/*******************************************************************************

 $Function:    	bookDetailsStart

 $Description:	Start routine for the phonebook details window
 
 $Returns:		None, assigns the window handle to the name_details_win
				hook in the current phone book

 $Arguments:	m, menu, i, menu item
 
*******************************************************************************/

void bookDetailsStart( MfwMnu* m, MfwMnuItem* i )
{
    T_MFW_HND       win			= mfwParent(mfw_header());
    T_MFW_WIN		*win_data	= ((T_MFW_HDR *)win)->data;
    tBookStandard	*data		= (tBookStandard *)win_data->user;

	TRACE_FUNCTION ("bookDetailsStart()");

	data->phbk->name_details_win = bookDetailsCreate( data->phbk->win );
}




/*******************************************************************************

 $Function:    	bookDetailsDestroy

 $Description:	Destroy the phone book details window
 
 $Returns:		None

 $Arguments:	window, handle of window to close
 
*******************************************************************************/

void bookDetailsDestroy( MfwHnd window )
{
	T_MFW_WIN     *win  = ((T_MFW_HDR *)window)->data;
	tBookStandard *data = (tBookStandard *)win->user;

	TRACE_FUNCTION ("bookDetailsdestroy()");

	/* Only need to do this if the data is valid
	*/
	if ( data )
	{
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		/* New editor no longer destroyed with winDelete,
		 * so destroy it here */
		if (data->editor)
		{
			ATB_edit_Destroy(data->editor);
		}
#endif /* NEW_EDITOR */
		data->phbk->name_details_win = 0;
		winDelete( data->win );
		FREE_MEMORY( (void *)data, sizeof( tBookStandard ) );
	}
}




/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/
