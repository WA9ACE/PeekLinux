/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiSearchWindow.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
	This module provides the phone book search window functionality for the
	phone book module of the basic MMI
                        
********************************************************************************
 $History: MmiSearchWindow.c

   23 April 2007 DR:OMAPS00121834,  x0066692 Asha 
   Description: Phonebook -> Phonebooksearch is pointing to the first entry when no of 
   entries in the phonebook is >240
   Fix: added the check to restrict the max_stop_index to max no availabe entries and
   made cursor point to the correct entry in the phonebook.
   		

    Jan 22, 2006 REF: OMAPS00061930  x0039928  
    Bug:PhoneBook -Change entry and view entry through Change option-failed
    Fix: Phonebook is populated with phone numbers after some time and not immediately while saving the changed entries.

    CRR: 25302 - xpradipg 10 Nov 2004
    Description: Should be able to select the number fro the phonebook while
    sending MMS/EMS.
    Solution: The new feature to select the number from phonebook while sending
    MMS/EMS is added
	
 	Oct 26, 2004    REF: CRR 25354 xkundadu
	Description: Phonebook: After entering precise name to search and then 
			    trying to delete using RSK, first character is not deleted
	Fix: If only one character is present in the editor and delete key is pressed,
		delete the editor buffer and update the editor view.
 
	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX

	25/10/00			Original Condat(UK) BMI version.
	20/02/02			Reinstated editor when in Chinese language by removing
						"if not Chinese" conditions SPR 1742
	   
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
#include "mfw_cnvt.h" //mfw_Gsm2SIMStr prototype

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

#include "gdi.h"
#include "audio.h"

#include "cus_aci.h"
#include "mfw_ffs.h"
#include "MmiTimers.h"


#include "MmiBookShared.h"
#include "mmiSmsMenu.h"
#include "mmismssend.h"
#include "MmiLists.h"
#include "mmiSmsRead.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#include "AUITextEntry.h"

#endif

typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND        chinese_search_win;                                    /* MFW win handler      */
  T_MFW_HND        parent;
  ListMenuData     * menu_list_data;
} T_CHINESE_SEARCH_INFO;
#include "mmiColours.h"

//CRR: 25302 - xpradipg 10 Nov 2004
//extern MmsSend_PHBK_exec_cb(T_MFW_HND win, USHORT event, void *parameter);


/*******************************************************************************
                                                                              
                                Private Methods
                                                                              
*******************************************************************************/
//#define TIME_TRACE_EVENT TRACE_EVENT
#ifndef TIME_TRACE_EVENT 
#define TIME_TRACE_EVENT
#endif

/*******************************************************************************

 $Function:    	bookSearchDialog

 $Description:	dialog handler for the search window functionality
 
 $Returns:		None

 $Arguments:	win, window handle
                event, event to be handled
				value, not used
				parameter, not used
 
*******************************************************************************/

void bookSearchDialog( T_MFW_HND win, USHORT event, SHORT value, void *parameter )
{
    T_MFW_WIN		*win_data	= ((T_MFW_HDR *) win)->data;
    tBookStandard	*data		= (tBookStandard *) win_data->user;
	tMmiPhbData		*Current    = &data->phbk->current;
	MfwMnu *mnu;
//Jan 22, 2006 REF: OMAPS00061930  x0039928
// Fix : Add a local variable search_string
    char search_string[MAX_ALPHA_LEN];
//x0066692 Date:23 April 2007 DR:OMAPS00121834, Asha
//Fix for phone book search issue - when phonebook has more than 240 entries.
	UBYTE max_search_name;
	
    TRACE_FUNCTION( "bookSearchDialog()" );

    switch( event )
	{
		case SEARCH_INIT:
		{
			/* No initialisation required
			*/
		}
		break;

		case SEARCH_SCROLL_UP:
		{	/*MC SPR 1541, flag to show whether we've scrolled off-screen and need to
		update the portion of the list displayed*/
			BOOL get_new_screen = FALSE;

		
			/* Scroll up
			*/
			if ( ( Current->index == 1 ) && ( Current->selectedName == 0 ) )
			{
				if ( Current->status.used_entries < MAX_SEARCH_NAME )
					Current->selectedName = Current->status.used_entries - 1;
				else
				{	Current->selectedName =  MAX_SEARCH_NAME - 1;
					/*SPR 1541, we're scrolling up from the first item in the list and are to go to
					the end*/
					get_new_screen = TRUE;
				}
				Current->index = Current->status.used_entries - Current->selectedName;

			}
			else
			{
				if (Current->selectedName == 0)
				{	Current->index--;
					/*SPR 1541, we've just come off the top of the screen*/
					get_new_screen = TRUE;
				}
				else
					Current->selectedName--;
			}

			mnu = (MfwMnu *) mfwControl( data->menu );
			mnu->lCursor[mnu->level] = Current->selectedName;

			/*SPR 1541, only update the list if flag set*/
			if (get_new_screen == TRUE)
				bookFindName( MAX_SEARCH_NAME, &data->phbk->current );
		}
		break;

		case SEARCH_SCROLL_DOWN:
		{	/*MC SPR 1541, flag to show whether we've scrolled off-screen and need to
		update the portion of the list displayed*/
			BOOL get_new_screen = FALSE;
		
			/* Scroll Down
			*/
			if ( ( Current->index + Current->selectedName ) == Current->status.used_entries )
			{
				Current->index = 1;
				Current->selectedName = 0;
				/*SPR 1541, we've come off the end of the list and are to go back to
				the beginning*/
				get_new_screen = TRUE;
			}
			else
			{
				if ( Current->selectedName == MAX_SEARCH_NAME - 1 )
				{	Current->index++;
				/*SPR 1541, we've just come off the bottom of the screen*/
					get_new_screen = TRUE;
				}
				else
					Current->selectedName++;
			}

			mnu = (MfwMnu *) mfwControl( data->menu );
			mnu->lCursor[mnu->level] = Current->selectedName;
			/*SPR 1541, only update the list if flag set*/
			if (get_new_screen == TRUE)
				bookFindName( MAX_SEARCH_NAME, &data->phbk->current );
		}
		break;

		case SEARCH_STRING:
		{
			{
				//Jan 22, 2006 REF: OMAPS00061930  x0039928
				//Fix : Copy the number to be searched into the search_string variable and give a search for it
				if ( value == TRUE)
					strcpy(search_string,(char*)data->phbk->edt_buf_name);
				else
					strcpy(search_string,(char*)data->edtBuf);
				
				/* Find name
				*/
				Current->index = Current->index + Current->selectedName;
				if ( bookSearchName( search_string, MAX_SEARCH_NAME, Current ) == MFW_PHB_OK )
				{
				//x0066692 Date:23 April 2007 DR:OMAPS00121834, Asha
				//Fix is added to make cursor point to the correct entry in the phonebook.
						max_search_name  = Current->status.used_entries - Current->index +1;
						if ( max_search_name > MAX_SEARCH_NAME )
							max_search_name = MAX_SEARCH_NAME; 
						
					if ( Current->index > ( Current->status.used_entries + 1 - max_search_name ) )
					{
						if ( Current->status.used_entries > MAX_SEARCH_NAME )
						{
							Current->selectedName = Current->index - Current->status.used_entries + max_search_name - 1;
							Current->index = Current->index - Current->selectedName;
						}
						else
						{
							Current->selectedName = Current->index - 1;
							Current->index=1;
						}

						/* go find the name
						*/
						
						bookFindName(MAX_SEARCH_NAME,&data->phbk->current);
					}
					else
					{	
						Current->selectedName =0;
	
					}

					/* update the menu */
				/* SPR881 - SH - Move name find to below */
			}
			else
			{/*SPR 2233, if no name found, go to beginning of list*/
				Current->index = 1;
	    		Current->selectedName = 0;
			}
			
			/* update the menu
			*/
			/* SPR881 - SH - is necessary even when BookSearchName, above, fails */

			mnu = (MfwMnu *) mfwControl( data->menu );
			mnu->lCursor[mnu->level] = Current->selectedName;
			TRACE_EVENT_P1("Current->selectedName: %d", Current->selectedName);
			/* go find the name
			*/
			bookFindName(MAX_SEARCH_NAME,&data->phbk->current);
		}
	}
		break;

		case SEARCH_UPDATE:
		{	
			/* Update, actually an initialisation/reset phase
			*/
			Current->index = 1;
			Current->selectedName = 0;

			{
				memset( data->edtBuf, '\0', sizeof( data->edtBuf ) );
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_Reset( data->editor );
#else /* NEW_EDITOR */
				edtReset( data->edt );
#endif /* NEW_EDITOR */
				SEND_EVENT( win, SEARCH_STRING, 0, 0 );
			}
		}
		break;

/* SPR#1428 - SH - New Editor: Add this to allow redraw of
 * window on request. */
 
#ifdef NEW_EDITOR
		case SEARCH_REDRAW:
			/*SPR 2104, TRACE seems to fix timing probs :/  */
			TRACE_EVENT("UPDATE SEARCH REDRAW");
			SEND_EVENT(data->win, SEARCH_STRING, 0, 0);
			win_show(data->win);
			break;
#endif /* NEW_EDITOR */

		default:
		{
			/*
			*/
		}
		break;
    }
}






/*******************************************************************************

 $Function:    	bookSearchWindowCB

 $Description:	Window call back function
 
 $Returns:		MFW_EVENT_CONSUMED, or MFW_EVENT_PASSED

 $Arguments:	e, event, w, window handle
 
*******************************************************************************/

static int bookSearchWindowCB( MfwEvt e, MfwWin *w )
{
    tBookStandard	*data		= (tBookStandard *) w->user;
	tMmiPhbData		*Current    = &data->phbk->current;
	int				index;
	char			*pchr;
	char			pchr2[PHB_MAX_LEN*2 +4];/*mc, SPR 1442*/
    TRACE_FUNCTION( "bookSearchWindowCB()" );

	// May 3, 2004        REF: CRR MMI-SPR-18555  Rashmi C N(Sasken) 
	//	When FDN is disabled we need to display only ADN entries in the phonebook menu.
	//    We just use the  preset PHB_ADN as the current book.
	//if(data->phbk->current.status.book == PHB_ADN)
	//	data->phbk->current.status.book = PHB_ADN_FDN;


	/* Deal with the visible event
	*/
	switch (e)
    {
        case MfwWinVisible:
		{
			/* Deal with the window visible event
			*/
			MmiBookShowDefault();

			{
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_Show( data->editor );
#else /* NEW_EDITOR */
				edtShow( data->edt );
#endif /* NEW_EDITOR */
			}

			/* SPR#1428 - SH - New Editor: Only update the list if we're not in multi-tap  */
#ifdef NEW_EDITOR
			if (!data->editor->multitap)
			{
#endif /* NEW_EDITOR */

			for ( index = 0; index < MAX_SEARCH_NAME; index++ )
			{
                           TRACE_EVENT( "bookSearchWindowCB()-loopstart" );
				
				pchr = (char *) MmiBookCallList( index ); 
				
				memset( pchr, '\0', NAME_SCREEN_MAX );

			/* SPR#1428 - SH - remove extraneous call to edtShow*/
			/*SPR 2123, using new macro PHONEBOOK_ITEM_LENGTH*/ 
#ifdef NO_ASCIIZ
			/*if current language chinese*/
				if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
				{	/*SPR 2653, this check should be here too*/
					if (!Current->list.entry == NULL)
					{	if ( Current->entry[ index ].name.len NEQ 0) //if name not empty
						{
							if (Current->entry[ index ].name.data[0] != 0x80)//if not unicode
							{/*we have to convert to unicode*/
								pchr2[0]= (char)0x80;/*SPR 1442, add unicode tag*//*a0393213 lint warnings removal - typecast done*/
								pchr2[1]= 0x7f;
								/*SPR2175, use new function*/
								ATB_convert_String((char*)Current->entry[ index ].name.data, MFW_ASCII, 
								(UBYTE)strlen((char*)Current->entry[ index ].name.data), &pchr2[2], MFW_DCS_UCS2, PHONEBOOK_ITEM_LENGTH-2, FALSE);
								/*SPR 1442 and now truncate the string*/
								resources_truncate_to_screen_width((char *) pchr2, Current->entry[ index ].name.len*2+2, pchr,PHONEBOOK_ITEM_LENGTH , SCREEN_SIZE_X, TRUE);
							}
							else /*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN for name strings*/
							{	
								/*SPR 1442 truncate the string*/
								resources_truncate_to_screen_width((char *) Current->entry[index].name.data, Current->entry[index].name.len, pchr, MAX_ALPHA_LEN, SCREEN_SIZE_X, TRUE);
							}
						}
						else /*we convert number string into unicode*/
						{	pchr2[0]= (char)0x80;/*SPR 1442, add unicode tag*//*a0393213 lint warnings removal-typecast done*/
							pchr2[1]= 0x7f;	
							/*SPR 2175, use new function*/
							ATB_convert_String((char*)Current->entry[ index ].number, MFW_ASCII, 
							(UBYTE)strlen((char*)Current->entry[ index ].number), &pchr2[2], MFW_DCS_UCS2, PHONEBOOK_ITEM_LENGTH-2, FALSE);
							/*SPR 1442 and now truncate the string*/
							resources_truncate_to_screen_width((char *) pchr2, strlen((char*)Current->entry[ index ].number)*2+2, pchr,PHONEBOOK_ITEM_LENGTH, SCREEN_SIZE_X, TRUE);
						}
					}

				}
					
				else
				{	/*if current language a latin language*/
/*SPR 2653, removed consitional compilation, this check applies whether we're in simulation mode or target hardware*/
	// EF check for null pointer first
				if (!Current->list.entry == NULL)
	/*SPR 2104, don't need this test*/
				
					if ( Current->entry[ index ].name.len NEQ 0&&Current->entry[ index ].name.data[0] != 0x80/*mc*/) //Should check to see if null name
					{	/*mc, SPR 1442, replaced old truncation function with new one*/ 
						resources_truncate_to_screen_width((char *) Current->entry[index].name.data, Current->entry[index].name.len, pchr, PHONEBOOK_ITEM_LENGTH, SCREEN_SIZE_X, FALSE);
					}
					else
					{/*mc, SPR 1442, replaced old truncation function with new one*/ 
						resources_truncate_to_screen_width((char *) Current->entry[index].number, strlen((char*)Current->entry[index].number), pchr, PHONEBOOK_ITEM_LENGTH, SCREEN_SIZE_X,FALSE);
					}

#else
					if ( Current->entry[ index ].name[ 0 ] != NULL) //Should check to see if null name
					{	/*mc, SPR 1442, replaced old truncation function with new one*/ 
						resources_truncate_to_screen_width((char *) Current->entry[index].name, 0, pchr, PHONEBOOK_ITEM_LENGTH, SCREEN_SIZE_X, FALSE);
					}
					else
					{	/*mc, SPR 1442, replaced old truncation function with new one*/ 
						resources_truncate_to_screen_width((char *) Current->entry[index].number, strlen((char*)Current->entry[index].number), pchr, PHONEBOOK_ITEM_LENGTH, SCREEN_SIZE_X,FALSE);
					}
	/*SPR 2123 end*/				
#endif
#ifdef NO_ASCIIZ/*MC SPR 975, opening bracket only exists when NO_ASCIIZ defined*/
				}
#endif
        	}

                    TRACE_EVENT( "bookSearchWindowCB()-end of loop" );
			mnuShow(data->menu);	
			/* SPR#1428 - SH - End of condition to check if we're in multi-tap */		
#ifdef NEW_EDITOR
			}
#endif

			/* special case if invoked from SMS or using SDN book
			*/
			if ( ( data->phbk->fromSMS )||
				( data->phbk->fromSMSSC ) || /* SPR#1428 - SH - New event for service centre editor */
				( data->phbk->fromDivert ) ||
				( data->phbk->fromDeflect )|| 
				//CRR: 25302 - xpradipg 10 Nov 2004
				//check if it was invoked from MMS/EMS
				( data->phbk->fromMmsEms)||
				( Current->status.book == PHB_SDN ) )
			{
				//displaySoftKeys( TxtSoftSelect, TxtSoftBack );
//  Jun 07, 2004        REF: CRR MMI-SPR-12818  Rashmi C N(Sasken) 
//	when the editor is empty, we need to display BACK softkey 
//    Delete softkey is displayed when it contains some characters
				if (data->editor_attr.text.len==0)
				{
					displaySoftKeys( TxtSoftSelect, TxtSoftBack );
				}
				else
				{
					displaySoftKeys( TxtSoftSelect, TxtDelete );
				}
			}
			else
			{
				//displaySoftKeys( TxtSoftOptions, TxtSoftBack );
//  Jun 07, 2004        REF: CRR MMI-SPR-12818  Rashmi C N(Sasken) 
//	when the editor is empty, we need to display BACK softkey 
//    Delete softkey is displayed when it contains some characters
				if (data->editor_attr.text.len==0)
				{
					displaySoftKeys( TxtSoftOptions, TxtSoftBack );
				}
				else
				{
					displaySoftKeys( TxtSoftOptions, TxtDelete );
				}
			}

		}
              TRACE_EVENT( "bookSearchWindowCB()-sk display end" );
		break;

	    default:
		{
			/* default is to pass event back for further handling
			*/
			return MFW_EVENT_PASSED;
		}
//		break;   // RAVI

    }

	/* consume the event
	*/
    return MFW_EVENT_CONSUMED;
}








/*******************************************************************************

 $Function:    	bookSearchKbdCB

 $Description:	Keyboard handler
 
 $Returns:		MFW_EVENT_CONSUMED always

 $Arguments:	e, event, k, key handle
 
*******************************************************************************/

static int bookSearchKbdCB( MfwEvt e, MfwKbd *k )
{
    T_MFW_HND       win			= mfwParent( mfw_header() );
    T_MFW_WIN		*win_data	= ((T_MFW_HDR *) win)->data;
    tBookStandard	*data		= (tBookStandard *) win_data->user;
	tMmiPhbData		*Current    = &data->phbk->current;

	TRACE_FUNCTION ("bookSearchKbdCB()");

	/* SPR#1428 - SH - New editor doesn't require editActivate */
	#ifndef NEW_EDITOR
	/* Set our editor active if it isn't already
	*/
	if ( activeEditor() != data->edt )
		editActivate( data->edt, TRUE);
	#endif

	/* Deal with the key and event combinatino
	*/

	
	switch( k->code )
    {
        case KCD_MNUUP:
		{
			/* scroll up
			*/
	        SEND_EVENT( data->win, SEARCH_SCROLL_UP, 0, 0 );
			winShow( data->win );
		}
		break;

        case KCD_MNUDOWN:
		{
			/* scroll down
			*/
	        SEND_EVENT( data->win, SEARCH_SCROLL_DOWN, 0, 0 );
			winShow( data->win );
		}
		break;

		

	case KCD_CALL:
			if ( data->phbk->fromSMS  )
				SEND_EVENT( data->phbk->parent_win, SMS_PHBK_NUMBER, PHBK_ID,
					(UBYTE *) Current->entry[ Current->selectedName ].number );
			else if ( data->phbk->fromSMSSC )
				SEND_EVENT( data->phbk->parent_win, SMSSC_PHBK_NUMBER, PHBK_ID,
					(UBYTE *) Current->entry[ Current->selectedName ].number );
			else
			{	if (data->phbk->fromDeflect /*SPR 1392, select number don't call it*/ )
				SEND_EVENT( data->phbk->parent_win, DEFLECT_PHBK_NUMBER, PHBK_ID,
					(UBYTE *) Current->entry[ Current->selectedName ].number );
				else
					callNumber( Current->entry[ Current->selectedName ].number );
			}
			bookPhonebookDestroy(data->phbk->win);
			break;
/* JVJ #1743 Right key deletes the characters, HUP destroys the window */
#ifdef NEW_EDITOR
		case KCD_HUP:
#else /* NEW_EDITOR */
        case KCD_HUP:
#endif /* NEW_EDITOR */
		{
			if ( data->phbk->fromSMS ){  /* Send empty string so that ptr can be set 0*/
				SEND_EVENT( data->phbk->parent_win, SMS_PHBK_NUMBER, PHBK_ID,
							"" );
				bookPhonebookDestroy(data->phbk->win);
				}
			else if ( data->phbk->fromSMSSC ){
				SEND_EVENT( data->phbk->parent_win, SMSSC_PHBK_NUMBER, PHBK_ID,
							NULL );
				bookPhonebookDestroy(data->phbk->win);
				}
			/* Marcus (mk): Issue 1010: 2002-08-22: Start */
 			else if (data->phbk->fromDivert)
			{
				bookPhonebookDestroy(data->phbk->win);
			}
 			else if (data->phbk->fromDeflect)/*MC SPR 1392*/
			{	/*Send empty string so that pointer to this window can be set to 0*/
				SEND_EVENT( data->phbk->parent_win, DEFLECT_PHBK_NUMBER, PHBK_ID,"");
				bookPhonebookDestroy(data->phbk->win);
			}
                    //CRR: 25302 - xpradipg 10 Nov 2004
                     //also handle if invoked by MMS/EMS
                     else if ( data->phbk->fromMmsEms )
                       {  /* Send empty string so that ptr can be set 0*/
				SEND_EVENT( data->phbk->parent_win, MMS_EMS_PHBK_NUMBER, PHBK_ID,"" );
				bookPhonebookDestroy(data->phbk->win);
			    }     
			/* Marcus (mk): Issue 1010: 2002-08-22: End */
			else
            	bookSearchDestroy(data->win);
            
		}
		break;

		case KCD_MNUSELECT:
        case KCD_LEFT:

		{
			/* if in SMS send the event to it
			*/
			if ( data->phbk->fromSMS )
			{	SEND_EVENT( data->phbk->parent_win, SMS_PHBK_NUMBER, PHBK_ID /*MZ 6/2/01 */,
					(UBYTE *) Current->entry[ Current->selectedName ].number );
				bookPhonebookDestroy(data->phbk->win);
			}
			else if ( data->phbk->fromSMSSC )
			{	SEND_EVENT( data->phbk->parent_win, SMSSC_PHBK_NUMBER, PHBK_ID /*MZ 6/2/01 */,
					(UBYTE *) Current->entry[ Current->selectedName ].number );
				bookPhonebookDestroy(data->phbk->win);
			}
			else if(data->phbk->fromDivert ) //MZ 26/02/01 check for divert If in Divert send event.
			{	
				SEND_EVENT( data->phbk->parent_win, DIVERT_PHBK_NUMBER, PHBK_ID /*MZ 28/2/01 */,
					(UBYTE *) Current->entry[ Current->selectedName ].number );
				bookPhonebookDestroy(data->phbk->win);
			}
			else if(data->phbk->fromDeflect ) /*SPR 1392*/
			{	
			TRACE_EVENT_P1("<< API deflection no. = %s", (char*) Current->entry[ Current->selectedName ].number);
    		SEND_EVENT( data->phbk->parent_win, DEFLECT_PHBK_NUMBER, PHBK_ID,
					(UBYTE *) Current->entry[ Current->selectedName ].number );
				bookPhonebookDestroy(data->phbk->win);
			}
                    //CRR: 25302 - xpradipg 10 Nov 2004
                    //send the number to the MMS/EMS window
                     else if(data->phbk->fromMmsEms)
                        {
                            TRACE_EVENT_P1("the value if win from MMS  is %x",data->phbk->parent_win);
                            SEND_EVENT( data->phbk->parent_win, MMS_EMS_PHBK_NUMBER,PHBK_ID,(void *) Current->entry[ Current->selectedName ].number );
		              bookPhonebookDestroy(data->phbk->win);
                        }
			else
			/* Start a new options menu
			*/
			/*check for UPN SPR 1327*/
			if (Current->status.book != PHB_UPN)
				data->phbk->menu_options_win = bookMenuStart( data->phbk->win, bookNameMenuAttributes(),0);
			else
				data->phbk->menu_options_win = bookMenuStart( data->phbk->win, bookUPNMenuAttributes(),0);
			}
		break;
		
	/* JVJ #1743 Right key deletes search character*/ 
#ifdef NEW_EDITOR
		case KCD_RIGHT:
#else /* NEW_EDITOR */
        case KCD_HUP:
#endif /* NEW_EDITOR */
		{


			
			/* If we have any characters in the buffer, step back, otherwise
			   destroy the window
			*/

			if ( data->edtBuf[0])
			{

// 	Oct 26, 2004    REF: CRR 25354 xkundadu
//	Description: Phonebook: After entering precise name to search and then 
//			    trying to delete using RSK, first character is not deleted.
//	Fix: If only one character is present in the editor and delete key is 
//		pressed, delete the editor buffer and update the editor view.

				// Find out the length of the buffer.
				if(strlen((char*)data->edtBuf) == 1)
	 			{
	 				//If only one character is present, delete and set the buffer 
	 				//to NULL.
					data->edtBuf[0] = '\0';
					data->editor_attr.text.len = 0;
					//Move the cursor accordingly.
					ATB_edit_MoveCursor(data->editor, ctrlLeft, TRUE);
					data->editor->update = ED_UPDATE_TRIVIAL;		
					SEND_EVENT( data->win, SEARCH_STRING, 0, 0);
					//Update the window.
					winShow( data->win );
					break;
				}
			
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				/* SPR#2174 - SH - If multitap is in progress, delete current character
				 * rather than previous character */

				if (data->editor->multitap)
				{
					AUI_entry_StopMultitap(data->entry_data); /* Cancel multitap */
					ATB_edit_DeleteRight(data->editor, TRUE); /* SPR#2342 - SH */
				}
				else
				{
	            			ATB_edit_DeleteLeft(data->editor, TRUE); /* SPR#2342 - SH */
				}
#else /* NEW_EDITOR */
                edtChar( data->edt,ecBack );
#endif /* NEW_EDITOR */

			    SEND_EVENT( data->win, SEARCH_STRING, 0, 0 );
				winShow( data->win );
			}
			else
			{
				/*API - 12/08/03 - CQ11268 - Add this so the phonebbok is destoryed on right key press*/ 
				if ( data->phbk->fromSMS )
				{  /* Send empty string so that ptr can be set 0*/
					bookPhonebookDestroy(data->phbk->win);
				}
				else if ( data->phbk->fromSMSSC )
				{
					bookPhonebookDestroy(data->phbk->win);
				}
				/* Marcus (mk): Issue 1010: 2002-08-22: Start */
	 			else if (data->phbk->fromDivert)
				{
					bookPhonebookDestroy(data->phbk->win);
				}
	 			else if (data->phbk->fromDeflect)/*MC SPR 1392*/
				{	/*Send empty string so that pointer to this window can be set to 0*/
					bookPhonebookDestroy(data->phbk->win);
				}
                            //CRR: 25302 - xpradipg 10 Nov 2004
                            //Handle if invoked from MMS/EMS
                            else if(data->phbk->fromMmsEms)
                            {
                                bookPhonebookDestroy(data->phbk->win);
                            }
				/* Marcus (mk): Issue 1010: 2002-08-22: End */
				else
	            	bookSearchDestroy(data->win);
				/*API - 12/08/03 - CQ11268 - END*/
			}
		}
		break;
		
		case KCD_HASH:
		{	

			{
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_DeleteLeft( data->editor, TRUE); /* SPR#2342 - SH */
#else /* NEW_EDITOR */
                edtChar( data->edt,ecBack );
#endif /* NEW_EDITOR */
			}
			
			/*NM, p016
			even updating when deleting characters
			*/
		    SEND_EVENT( data->win, SEARCH_STRING, 0, 0 );
		    winShow( data->win );
		}
		break;

        default:
		{
			
			{
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				AUI_entry_EventKey( data->entry_data, e, k );

				/* send the search event to ourselves,
				 * but only when a character is finished in multi-tap
				*/
				if (!data->editor->multitap)
				{
			    	SEND_EVENT( data->win, SEARCH_STRING, 0, 0 );
			    }
#else /* NEW_EDITOR */
				editEventKey( e, k );
				/* send the search event to ourselves,
				*/
			    SEND_EVENT( data->win, SEARCH_STRING, 0, 0 );
#endif /* NEW_EDITOR */
				winShow( data->win );
			}
		}
		break;

    }

	/* always consume the event
	*/
    return MFW_EVENT_CONSUMED;
}







/*******************************************************************************

 $Function:    	bookSearchKbdLongCB

 $Description:	long keypress event handler, handles long clear event only
 
 $Returns:		MFW_EVENT_CONSUMED always

 $Arguments:	e, event, k, key handle
 
*******************************************************************************/

static int bookSearchKbdLongCB( MfwEvt e, MfwKbd *k )
{
    T_MFW_HND       win			= mfwParent( mfw_header() );
    T_MFW_WIN		*win_data	= ((T_MFW_HDR *) win)->data;
    tBookStandard	*data		= (tBookStandard *) win_data->user;

	TRACE_FUNCTION ("bookSearchKbdLongCB()");
	
	/* deal with the clear event
	*/
	if ( ( e & KEY_CLEAR ) && ( e & KEY_LONG ) )
	{
		bookSearchDestroy( win );
		data->phbk->search_win = 0;
	}

	/* consume the event
	*/
	return MFW_EVENT_CONSUMED;
}

//Jan 22, 2006 REF: OMAPS00061930  x0039928
// Fix : Add a temporary timer handle for future reference.
T_MFW_HND PhbSearchTim;

/*******************************************************************************

 $Function:    	bookSearchTimCB

 $Description:	Callback function for the Phonebook timer.
 
 $Returns:	None

 $Arguments:	window handle event, timer control block

*******************************************************************************/

static int bookSearchTimCB (MfwEvt event, MfwTim *timer)
{
    T_MFW_HND       win			= mfwParent( mfw_header() );
    T_MFW_WIN		*win_data	= ((T_MFW_HDR *) win)->data;
    tBookStandard	*data		= (tBookStandard *) win_data->user;

       /* x0045876, 14-Aug-2006 (WR - "Current" was declared but never referenced) */
	/* tMmiPhbData		*Current    = &data->phbk->current; */
	TRACE_FUNCTION ("bookSearchTimCB()");
	
    SEND_EVENT( data->win, SEARCH_STRING, 1, 0);
		    winShow( data->win );
    	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	bookSearchCreate

 $Description:	Create the search window
 
 $Returns:		handle of window or NULL if error

 $Arguments:	parent, handle of parent window
 
*******************************************************************************/

static T_MFW_HND bookSearchCreate( MfwHnd parent )
{
	T_MFW_WIN		*parent_win_data = ( (T_MFW_HDR *) parent )->data;
	T_phbk			*phbk = (T_phbk *)parent_win_data->user;
	T_MFW_WIN		*win_data;
	tBookStandard	*data;
//	int i;   // RAVI
//	char debug[50];   // RAVI

	TRACE_FUNCTION ("bookSearchCreate()");

	/* allocate memory for our control block
	*/
	if ( ( data = (tBookStandard *) ALLOC_MEMORY( sizeof( tBookStandard ) ) ) == NULL )
		return NULL;

	/*SPR 2123, MC allocate memory for phonebook list*/
	if (create_mmi_phonebook_names_list() == NULL)
		return NULL;

	/* Create the window if we can
	*/
	if ( ( data->win = win_create( parent, 0, E_WIN_VISIBLE, (T_MFW_CB) bookSearchWindowCB ) ) == NULL )
	{
		FREE_MEMORY( (void *) data, sizeof( tBookStandard ) );
		return NULL;
	}

	/* Okay, we have created the control block and the window, so
	   we now need to configure the dialog and data pointers
	*/
    data->mmi_control.dialog	= (T_DIALOG_FUNC) bookSearchDialog;
    data->mmi_control.data		= data;
    win_data					= ((T_MFW_HDR *)data->win)->data;
 	win_data->user				= (void *) data;
	data->phbk					= phbk;
	data->parent_win			= parent;

	/* create keyboards and menus for our window
	*/
	data->kbd		= kbdCreate( data->win, KEY_ALL,            (MfwCb) bookSearchKbdCB );
        data->kbd_long	= kbdCreate( data->win, KEY_ALL | KEY_LONG, (MfwCb) bookSearchKbdLongCB );
	//Jan 22, 2006 REF: OMAPS00061930  x0039928
	// Fix: Create a timer and assign to PhbSearchTim
	data->tim = timCreate(data->win, 1000, (MfwCb)bookSearchTimCB);
	data->menu		= mnuCreate( data->win,MmiBookMenuDetailsList() , 0, 0 );
	PhbSearchTim = data->tim;
	mnuLang (data->menu,0);

	/* 1945 MZ Initialise the edtBuf   */
	memset(data->edtBuf,'\0', STANDARD_EDITOR_SIZE );
	
	{

	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
/*SPR 2233, make sure editor only displays lower-case chars*/
	AUI_edit_SetAttr(&data->editor_attr, BOOKSEARCH_EDITOR, COLOUR_EDITOR_XX,EDITOR_FONT,
    ED_MODE_ALPHA | ED_MODE_FORMATTED, ED_CURSOR_BAR, ATB_DCS_ASCII, (UBYTE*) data->edtBuf, MAX_SEARCH_CHAR);
    data->editor_attr.FormatString = "*a";
    data->editor = ATB_edit_Create( &data->editor_attr, 0 );
    data->entry_data = AUI_entry_Create(data->win, data->editor, SEARCH_REDRAW);
    /*SPR2233 end*/
    ATB_edit_Init(data->editor);	
#else /* NEW_EDITOR */
	bookSetEditAttributes(BOOKSEARCH_EDITOR,  COLOUR_EDITOR_XX, 0, edtCurBar1, 0, 
		(char *) data->edtBuf, MAX_SEARCH_CHAR, &data->attr );
		data->edt = edtCreate( data->win, &data->attr, 0, 0 );
#endif /* NEW_EDITOR */
	}


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

 $Function:    	bookSearchStart

 $Description:	Starts the search window
 
 $Returns:		handle of newly created window, or NULL if error

 $Arguments:	parent, handle of parent window
 
*******************************************************************************/

T_MFW_HND bookSearchStart( MfwHnd parent )
{
	T_MFW_HND win;
    T_MFW_WIN		*win_data;
    tBookStandard   *data;
	MfwMnu			*mnu;
	tMmiPhbData		*Current;

	/* if the phone book is still loading, we can't do anything
	   with it, so return a NULL, after showing an information
	   dialog
	*/
	if ( phb_get_mode() == PHB_LOADING )
	{
		bookShowInformation( idle_get_window(),  TxtPleaseWait ,NULL, NULL );
		return NULL;
	}

	/* try to create the window
	*/
    if ( ( win = bookSearchCreate( parent ) ) == NULL )
		return NULL;

	/* having created the window, we now need to initialise it to
	   sensible data
	*/
	win_data	= ( (T_MFW_HDR *) win)->data;
    data		= (tBookStandard *) win_data->user;
	Current		= &data->phbk->current;

	/* establish the status of the phone book
	*/
	if ( ( Current->status.book != PHB_UPN ) && ( Current->status.book != PHB_SDN ) )
	    Current->status.book = bookActiveBook(READ);
	bookGetCurrentStatus( &Current->status );

	/* try to establish if we have information in the phone book
	*/
	Current->index = 1;
	Current->selectedName = 0;
	bookFindName( MAX_SEARCH_NAME, Current );

	if ( ! Current->index )
	{
		bookSearchDestroy( win );
		bookShowInformation( idle_get_window(), TxtEmptyList, NULL, NULL );
		return NULL;
	}

	/* clear the edit buffer prior to displaying the editor and menus
	*/


		memset( data->edtBuf, '\0', sizeof( data->edtBuf ) );
		/* SPR#1428 - SH - New Editor: don't need editActivate */
#ifndef NEW_EDITOR
		editActivate( data->edt, true );
#endif /* NEW_EDITOR */

	mnu = (MfwMnu *) mfwControl( data->menu );
	mnu->lCursor[mnu->level] = Current->selectedName;
	winShow(data->win);

	/* return the handle to the newly created window
	*/
    return win;
}




/*******************************************************************************

 $Function:    	bookSearchDestroy

 $Description:	Destroy the search window
 
 $Returns:		None

 $Arguments:	Window, handle of the window to be destroyed
 
*******************************************************************************/

void bookSearchDestroy( MfwHnd window )
{
	T_MFW_WIN		*win  = ((T_MFW_HDR *) window)->data;
	tBookStandard	*data = (tBookStandard *) win->user;

	TRACE_FUNCTION( "bookSearchDestroy()" );

	/* Only destroy if the data is valid
	*/
	if ( data )
	{
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		/* Editor now not automatically destroyed by winDelete,
		 * so destroy it here. */
		if (data->entry_data)
		{
			AUI_entry_Destroy(data->entry_data);
			data->entry_data = 0;
		}
		if (data->editor)
		{
			ATB_edit_Destroy(data->editor);
			data->editor = 0;
		}
#endif /* NEW_EDITOR */

		/* If we're the root window we destroy using it's function
		*/
		if ( data->phbk->root_win == window )
		{
			bookPhonebookDestroy( data->phbk->win );
			return;
		}

		/* delete ourself
		*/
		data->phbk->search_win = 0;
		winDelete( data->win );
		FREE_MEMORY( (void *) data, sizeof( tBookStandard ) );
	}
	/*SPR2123, MC deallocate memory for phonebook list*/
	destroy_mmi_phonebook_names_list(MmiBookMenuDetailsList());
}







/*******************************************************************************

 $Function:    	bookSearchName

 $Description:	locate a requested name in the phone book
 
 $Returns:		status of the findName routine

 $Arguments:	pattern, to be searched for
				number,  of elements to limit the search to
				current, pointer to a buffer to store the results
 
*******************************************************************************/

UBYTE bookSearchName (char *pattern,UBYTE number,tMmiPhbData *current)
{
       #ifdef NO_ASCIIZ
	T_MFW_PHB_TEXT l_name;
    #else
	UBYTE l_name[PHB_MAX_LEN];
    #endif
//	UBYTE len;   // RAVI
	int	  i;
	UBYTE ret = (UBYTE)PHB_FAIL;
	
	TRACE_FUNCTION( "bookSearchName()" );

	/* if using UPN phone book, return status of findname
	*/
	if ( current->status.book == PHB_UPN )
		return ret = current->result = bookFindName( MAX_SEARCH_NAME, current );

	/* otherwise we need to be a little bit more clever with this
	*/
    memset( current->entry, 0, MAX_SEARCH_CALL_LIST * sizeof( T_MFW_PHB_ENTRY ) );
	bookGetCurrentStatus( &current->status );

	current->list.entry			= current->entry;
    current->list.num_entries	= number;

	/* convert from GSM characters
	*/
	#ifdef NO_ASCIIZ
	
	memset( l_name.data, 0, PHB_MAX_LEN );
	mfw_Gsm2SIMStr( MFW_DCS_7bits, (UBYTE *) pattern, PHB_MAX_LEN, l_name.data, &l_name.len );
	#else
    memset( l_name, 0, PHB_MAX_LEN );
	mfw_Gsm2SIMStr( MFW_DCS_7bits, (UBYTE *) pattern, PHB_MAX_LEN, l_name, &len );
	#endif
	/* perform a search to locate a match with this criteria
	*/
    if ( strlen(pattern) != 0  )
    {
#ifdef WIN32
#ifndef NO_ASCIIZ
		/***************************Go-lite Optimization changes Start***********************/
		//Aug 16, 2004    REF: CRR 24323   Deepa M.D
		TRACE_EVENT_P1("Search str: %s", l_name.data);
		/***************************Go-lite Optimization changes end***********************/
        ret = phb_find_entries( current->status.book, &current->index, MFW_PHB_ALPHA, number, (UBYTE *) l_name, len, &current->list );
#endif
#else
//not WIN32

#ifdef NO_ASCIIZ
        ret = phb_find_entries( current->status.book, &current->index, MFW_PHB_ALPHA, number, &l_name, &current->list );
#else
        ret = phb_find_entries( current->status.book, &current->index, MFW_PHB_ALPHA, number, (char *) l_name, &current->list );
#endif //NO_ASCIIZ

#endif //WIN32
    }
    else
    {
        /* we don't have any name information, so start at the beginning
        */
    	current->index = 1;
	    current->selectedName = 0;
	    bookFindName( MAX_SEARCH_NAME, current );
    }
    
	/* check for match
	*/
	if ( ( current->list.num_entries < MAX_SEARCH_NAME )
		|| ( current->list.result == MFW_NO_ENTRY ) || ( ret == (UBYTE)PHB_FAIL ) )
	{
		/* no, then try to find again
		*/
		ret = bookFindName( MAX_SEARCH_NAME, current );
	}
	else
	{	
		for ( i = 0; i < number; i++ )
#ifdef NO_ASCIIZ
{			
			/* convert from Sim to GSM and then copy to output buffer
			*/
/*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN for name strings*/
/*MC SPR 2175, using new function to convert name string*/
			if (current->entry[i].name.dcs == MFW_DCS_8bits && current->entry[i].name.data[0] != 0x80)
			{	ATB_convert_String((char*)current->entry[i].name.data, MFW_DCS_8bits, current->entry[i].name.len, (char*)l_name.data, MFW_ASCII, MAX_ALPHA_LEN, FALSE);
				memcpy( current->entry[i].name.data, l_name.data, MAX_ALPHA_LEN );
				current->entry[i].name.len = l_name.len;
				current->entry[i].name.dcs = MFW_ASCII;
			}
}
#else
/* convert from GSM to Alpha characters
			*/
			bookGsm2Alpha( (UBYTE *) current->entry[i].name );
#endif
	}

	/* return the status, and store it as part of current as well
	*/
	TRACE_EVENT_P1("BookSearchName returns: %d", ret);
	return current->result = ret;
}



/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

