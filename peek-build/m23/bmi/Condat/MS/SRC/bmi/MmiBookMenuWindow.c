/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookMenuWindow.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    Menu handling for the phone book
                        
********************************************************************************
 $History: MmiBookMenuWindow.c
      Mar 30, 2007 OMAPS00123019 a0393213(R.Prabakar) 
      Description : Reboot of the target when you unplug/plug USB cable using camera application
      
	xrashmic 28 Jun 2006, OMAPS00084172
	On pressing back from options menu in camera, the midi stopped playing.
	Hence added stopRingerVolSettingInactivityTimer under the if(getSoundsReason() == SettingVolume)
			
	Sep 01, 2005    REF: CRR 32707  x0021334
	Description: Select Softkey dosen't work for the second time on choosing Volume option in Ringer menu
	Fix:	Select soft key operation in volume menu has been modified to fix this problem. 
	      
	Apr 05, 2005    REF: ENH 29994 xdeepadh
	Description: Implementation of Generic Midi Ringer and Modification of existing Midi Test Application
	Solution: Generic Midi Ringer and Midi Test Application were implemented.
 
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
#include "mmiColours.h"


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
#include "MmiSettings.h"  // for keypadlock
#include "MmiSounds.h"
#include "MmiIdle.h"

extern UBYTE HUPKeyOrigin;		// Variable to indicate that long end key press is coming from menu

/*******************************************************************************                       
                                External Function Prototype
                                                                              
*******************************************************************************/
/* Fix for the melody and volume settings. 3/4/01 MZ */
extern BOOL getScrollSelectMenuItem(void);
extern clearScrollSelectMenuItem(void);
extern void volMenuItemSelected(void);
extern USHORT getSoundsReason(void);
extern void startRingerVolSettingInactivityTimer(void);
extern void stopRingerVolSettingInactivityTimer(void);
extern void restartVolSettingInactivityTimer(void);
//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifdef FF_MIDI_RINGER
extern MfwHnd hPlayerStartTimer;         /* timeout till start of midi file    */
 #endif

/*******************************************************************************
                                                                              
                                Private Methods
                                                                              
*******************************************************************************/



/*******************************************************************************

 $Function:    	bookMenuMenuCB

 $Description:	Menu handler for the menu window
 
 $Returns:		MFW_EVENT_CONSUMED if event handled, otherwise
				MFW_EVENT_PASSED

 $Arguments:	e, event, m, menu handle
 
*******************************************************************************/

static int bookMenuMenuCB( MfwEvt e, MfwMnu *m )
{
    T_MFW_HND       window		= mfwParent( mfw_header() );
	T_MFW_WIN		*win_data	= ((T_MFW_HDR *)window)->data;
	tBookStandard	*data		= (tBookStandard *) win_data->user;

	TRACE_FUNCTION( "bookMenuMenuCB()" );

	/* We only handle the ESCAPE event in here
	*/
	switch( e )
    {
        case E_MNU_ESCAPE:
		{
			/* Escape requested, we need to shut down this window
			*/
			if (data->Callback)
				(data->Callback) (data->parent_win, NULL, NULL);
			bookMenuDestroy( window );
			//xrashmic 28 Jun 2006, OMAPS00084172
			//On pressing back from options menu in camera, the midi stopped playing.
			if(getSoundsReason() == SettingVolume)
				stopRingerVolSettingInactivityTimer();
		}
		break;

        default:
		{
			/* unknown event, pass it up the tree for handling elsewhere
			*/
            return MFW_EVENT_PASSED;
        }
/*		break;  */ /* RAVI */
	}

	/* We got here, event handled, prevent anyone else
	   dealing with it
	*/
	return MFW_EVENT_CONSUMED;
}






/*******************************************************************************

 $Function:    	bookMenuDialog

 $Description:	Menu window dialog function
 
 $Returns:		none

 $Arguments:	win, window handle
                event, event to be handled
				value, not used
				parameter, not used
 
*******************************************************************************/

void bookMenuDialog( T_MFW_HND win, USHORT event, SHORT value, void *parameter )
{
	T_MFW_WIN		*win_data	= ((T_MFW_HDR *)win)->data;
	tBookStandard	*data		= (tBookStandard *) win_data->user;
/*	MfwMnu			*mnu;  */ /* RAVI */
	
	TRACE_FUNCTION( "bookMenuDialog()" );

	/* The menu dialog function has to deal with initialisation
	   and responses from the PIN2 validation methods
	*/
    switch( event )
	{
		case MENU_INIT:
		{
			/* On initialisation, we can create the menus and
			   display the window
			*/
			data->menu = mnuCreate( data->win, (MfwMnuAttr *) parameter, E_MNU_ESCAPE, (MfwCb) bookMenuMenuCB );
			mnuLang( data->menu, mainMmiLng );
			mnuUnhide( data->menu );
			winShow( data->win );

			if (value EQ PhbkMainMenu)
			{
		           // start the timer for the keypadlock
					tim_start (data->menu_tim);
		            data->status_of_timer = FALSE;
		     }
		     else
		     {
					data->status_of_timer = TRUE;
		     }
		     if(value == SettingVolume)
		     {
		     	/*
				  The ringer volume settings dialog has been invoked, create and start the
				  4 second time out.
		     	*/
		     	/* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD		     	
		     	setSoundsReason(SettingVolume);
#endif
			/* END RAVI */
		     	startRingerVolSettingInactivityTimer();
		     }
			
		}
		break;

		case ADD_CALLBACK:
		{
			/* Change the callback from the default menu callback to one specified
			 * in parameter */
			data->Callback = (T_VOID_FUNC)(parameter);
		}
		break;

		case DEFAULT_OPTION:
		{
			/* Highlight an option in the menu when it is opened.  Parameter points to a UBYTE. */
			
			mnuChooseVisibleItem(data->menu, *((UBYTE *)parameter));
		}
		break;
		
		case PIN2_OK:
		{
			/* We have received a PIN2 authorisation, so we can execute the
			   the requested function
			*/
			
			TRACE_EVENT("PIN2_OK: Should carry on to perform phonebook op");
			(data->phbk->pin2_next)( data->phbk->menu, data->phbk->item);

		}
		break;
  		case E_MNU_ESCAPE:
		{
			/* Escape requested, we need to shut down this window
			*/
			bookMenuDestroy( win);
		}
		break;
		default:
		{
			/* In situations where a PIN2 abort is returned, we can handle it
			   as a default event, and just ignore it
			*/
			TRACE_EVENT("PIN2 not OK");
		}
		break;
	}
}



/*******************************************************************************

 $Function:    	bookMenuTimCB

 $Description:	Menu tim handler
 
 $Returns:		
				
 $Arguments:	
 
*******************************************************************************/

static int bookMenuTimCB (T_MFW_EVENT event, T_MFW_TIM *tc)
{
    T_MFW_HND       window		= mfwParent( mfw_header() );
	T_MFW_WIN		*win_data	= ((T_MFW_HDR *)window)->data;
	tBookStandard	*data		= (tBookStandard *) win_data->user;

	data->status_of_timer = TRUE;

	/* ADDED RAVI - 25-11-2005 */
	return 0;
	/* END RAVI */

}






/*******************************************************************************

 $Function:    	bookMenuWindowCB

 $Description:	Menu window handler
 
 $Returns:		MFW_EVENT_CONSUMED if event handled, otherwise
				MFW_EVENT_PASSED

 $Arguments:	e, event, m, window handle
 
*******************************************************************************/

static int bookMenuWindowCB( MfwEvt e, MfwWin *w )
{
	tBookStandard * data = (tBookStandard *)w->user;
	MfwMnu * mnu;

    TRACE_FUNCTION( "bookMenuWindowCB()" );

	if (!w)
	    return MFW_EVENT_PASSED;

	switch( e )
    {
        case MfwWinVisible:
		{
			dspl_ClearAll();	// sbh - clear screen before update
			mnu = (MfwMnu *) mfwControl( data->menu );

			/*
			**Check whether the data is valid, if not use 'sensible' default values
			*/
			if (mnu)
			{
				if (mnu->curAttr)
			softKeys_displayId( TxtSoftSelect, TxtSoftBack, 0, mnu->curAttr->mnuColour);
				else
					softKeys_displayId( TxtSoftSelect, TxtSoftBack, 0, COLOUR_LIST_SUBMENU);
					
			}
			else
				softKeys_displayId( TxtSoftSelect, TxtSoftBack, 0, COLOUR_LIST_SUBMENU);
		}
		break;

        default:
		{
			/* unknown event, pass it up the tree for handling elsewhere
			*/
            return MFW_EVENT_PASSED;
        }
	/*	break;  */ /* RAVI */
	}

	/* We got here, event handled, prevent anyone else
	   dealing with it
	*/
    return MFW_EVENT_CONSUMED;
}






/*******************************************************************************

 $Function:    	bookMenuKbdCB

 $Description:	tbd
 
 $Returns:		MFW_EVENT_CONSUMED always

 $Arguments:	e, event, k, key handle
 
*******************************************************************************/

static int bookMenuKbdCB( MfwEvt e, MfwKbd *k )
{
	static BOOL storeSelectedItemToPcm=FALSE;
    T_MFW_HND      window		= mfwParent( mfw_header() );
	T_MFW_WIN		*win_data	= ((T_MFW_HDR *)window)->data;
	tBookStandard	*data		= (tBookStandard *) win_data->user;
	MfwMnu			*mnu;
	U8				keycode;	/* SPR#1608 - SH */

	TRACE_FUNCTION( "bookMenuKbdCB()" );

	/* handle the keypress
	*/
	switch (k->code)
    {
		case KCD_MNUUP:
		{
			// keypadLock will not activate anymore
			data->status_of_timer = TRUE;			
			/* Update the window with the previous element
			*/
            mnuUp( data->menu );
			/*
            Special case when setting up ringer volume, the current ringer 
            is played as user scrolls the volume settings Silent -> cresendo.
            MZ.
            */
            /* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD            
	     if (getSoundsReason () == SettingVolume)
	     {
		     volumeMenuScrollUp();
	     }
#endif		 
	     /* END RAVI */
			
            if(getScrollSelectMenuItem() == TRUE ||(getSoundsReason() == SettingVolume) )
            	{
            	  mnuSelect(data->menu);
		  /* Scroll up in volume setting */ 		  
            	 restartVolSettingInactivityTimer();
            	}
		}
        break;

		case KCD_MNUDOWN:
		{
			// keypadLock will not activate anymore
			data->status_of_timer = TRUE;			
			/* Update the window with the next element
			*/
            mnuDown( data->menu );

			/*
            Special case when setting up ringer volume, the current ringer 
            is played as user scrolls the volume settings Silent -> cresendo.
            MZ.
            */
            /* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
		if (getSoundsReason () == SettingVolume)
		{
			volumeMenuScrollDown();			
		}
#endif	
	     /* END RAVI */	

            if((getScrollSelectMenuItem() == TRUE) || (getSoundsReason() == SettingVolume))
            	{
            	 	mnuSelect(data->menu);
            	 	restartVolSettingInactivityTimer(); 
            	}

	     }
        break;

		case KCD_MNUSELECT:
        case KCD_LEFT:
		{
			// keypadLock will not activate anymore
			data->status_of_timer = TRUE;	

			// Select this element
			mnuSelect( data->menu );
			/*30 Mar 2007 a0393213 (R.Prabakar)
			OMAPS00123019 ( Reboot of the target when you unplug/plug USB cable using camera application)
			The usb enumeration option window has to be destroyed automatically once the user selects an option.
			Earlier the menu was destroyed in the menu select call back function (mmi_usb_enumeration) and the same menu was
			accesed further in this function. This was resulting in crash.
			Now the menu is destroyed in this function and returned immediately.
			*/
			#ifdef FF_MMI_USBMS
			if(usb_opt_win &&
			    data->menu==((tBookStandard*)(((MfwWin*)(((T_MFW_HDR *)usb_opt_win)->data))->user))->menu)
				{
				bookMenuDestroy(usb_opt_win);
				usb_opt_win=NULL;			
				break;
				}
			#endif
			mnu = (MfwMnu *) mfwControl( data->menu );

			if (mnu != NULL)
			{
				if (mnu->curAttr != NULL)
				{
					// API - 24-01-03 - 1606 - Soft Key updating add this win Focus check
					if( winIsFocussed(window) )
						softKeys_displayId( TxtSoftSelect, TxtSoftBack, 0, mnu->curAttr->mnuColour);
				}
			}
			
			// A volume or tone setting menu has been entered. MZ 2/4/01
			if((getScrollSelectMenuItem() == TRUE) || (getSoundsReason() == SettingVolume))
			{
			TRACE_FUNCTION("@@@@ A volume or tone settin");

				/* 
				   First time pcm flag is false, subsequent selection of menu item
				   will select and terminate the volume settings menu. 3/4/01 MZ.
				*/
				if(storeSelectedItemToPcm ==TRUE || getSoundsReason() == SettingVolume )
				{
					clearScrollSelectMenuItem();
					volMenuItemSelected();
					storeSelectedItemToPcm =FALSE;
					mnuEscape( data->menu );
					if(getSoundsReason() == SettingVolume)
					{
					   	setSoundsReason(SoundsNone);
						stopRingerVolSettingInactivityTimer(); 
					}
//Apr 05, 2005    REF: ENH 29994 xdeepadh					
#ifdef FF_MIDI_RINGER
					     //Delete the timer for volume scrolling
					     	if(hPlayerStartTimer!=NULL)
						timDelete(hPlayerStartTimer);
#endif
					break;
				}
				  /*
				  	Second time the left soft key is used, store the setting in pcm and terminate the 
				  	volume settings menu.
				  */
				storeSelectedItemToPcm =TRUE;
				
			}
		}
        break;

		// POWER key takes us to idle screen.
        case KCD_POWER:
			data->status_of_timer = TRUE;	
			
			HUPKeyOrigin = 0; // sbh - we're coming from the menu screen, so don't shut down right away

        	//return to idle screen
        	
			if (data->Callback)
				(data->Callback) (data->parent_win, NULL, NULL);
			
			bookMenuDestroy(data->win);
			stopRingerVolSettingInactivityTimer();
			
        	SEND_EVENT(idle_get_window(), IDLE_UPDATE, 0, 0);
        break;
        	
        case KCD_RIGHT:
        case KCD_HUP:
		{
			// keypadLock will not activate anymore
			data->status_of_timer = TRUE;

			//	Sep 01, 2005    REF: CRR 32707  x0021334
			//	Set storeSelectedItemToPcm to FALSE to enable proper operation every time instead of
			//	handling it in alterate attempts.
			storeSelectedItemToPcm = FALSE;

			HUPKeyOrigin = 0; // sbh - we're coming from the menu screen, so don't shut down right away
			
			/* get us out of here
			*/
                     mnuEscape( data->menu );
			mnu = (MfwMnu *) mfwControl( data->menu );
			if (mnu != NULL)
			{
				if (mnu->curAttr != NULL)
				{
					// API - 24-01-03 - 1606 - Soft Key updating add this win Focus check
					if( winIsFocussed(window) )				
						softKeys_displayId( TxtSoftSelect, TxtSoftBack, 0, mnu->curAttr->mnuColour);
				}
			}
			
			// Return the menu scroll behavior to normal.
			if(getScrollSelectMenuItem() == TRUE)
			{
				clearScrollSelectMenuItem();
		}
			/*
				If user exits the volume settings menu from the Idle or Incoming call screen.
				clear the soundsReason.
			*/ 
			if(getSoundsReason() == SettingVolume)
			{
				/* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD			
				/* Restore the volume setting */
				restorePreviousVolumeSetting ();
#endif
				/* END RAVI */
				 stopRingerVolSettingInactivityTimer(); 
				setSoundsReason(SoundsNone);
//Apr 05, 2005    REF: ENH 29994 xdeepadh				
#ifdef FF_MIDI_RINGER
				//Delete teh timer for volume scrolling
	        		  if(hPlayerStartTimer!=NULL)
				  	timDelete(hPlayerStartTimer);
#endif
				}
            
		}
        break;
        
        case KCD_STAR:
		{	
		
			/*
             * Keypadlock will lock only 
             * within two seconds by press "*"
             */	
             
			if(data->status_of_timer EQ FALSE)
			{
	            TRACE_EVENT ("activate KEYpadLOCK");
    	        mnuEscape( data->menu );
				settingsKeyPadLockOn ();
			}
			
			/* SPR#1608 - SH - Otherwise, send '*' keypress to idle screen */
			
            else if (window == data->phbk->menu_main_win)
			{		
	        	/* Number key presses in phonebook menu returns user to idle screen, 
	               add key press to Idle edit screen. */

				keycode = k->code;

				bookMenuDestroy(data->win);

		        SEND_EVENT(idle_get_window(), IDLE_START_DIALLING_NUMBER, 0, &keycode);
			}
		}
        break;

        case KCD_HASH:
		{	
			/*
             * Keypadlock will lock only 
             * within two seconds by press "*"
             */			
			if(data->status_of_timer EQ FALSE)
			{
	            TRACE_EVENT ("activate Silent mode");

				if(FFS_flashData.settings_status & SettingsSilentMode)
				{
					settingsSilentModeOff();
				}
				else
				{
					settingsSilentModeOn();
				}
				mnuEscape( data->menu );
			}

			/* SPR#1608 - SH - Otherwise, send '#' keypress to idle screen */
			
            else if (window == data->phbk->menu_main_win)
			{		
	        	/* Number key presses in phonebook menu returns user to idle screen, 
	               add key press to Idle edit screen. */

				keycode = k->code;

				bookMenuDestroy(data->win);

		        SEND_EVENT(idle_get_window(), IDLE_START_DIALLING_NUMBER, 0, &keycode);
			}
		}
        break;
        
        default:
		{
			TRACE_EVENT("bookMenuKbdCb - DEFAULT!");
			/* SH - only allow dialling if we are in the main menu */
			if (window == data->phbk->menu_main_win)
			{
				/* keypadLock will not activate anymore */
				data->status_of_timer = TRUE;		

	        	/* Number key presses in phonebook menu returns user to idle screen, 
	               add key press to Idle edit screen. */

				keycode = k->code;

				bookMenuDestroy(data->win);

				/* x0045876, 14-Aug-2006 (WR - pointless comparison of unsigned integer with zero) */
				/* if ((k->code>=KCD_0 && k->code<=KCD_9)) */
				if (k->code<=KCD_9)
				{
					TRACE_EVENT("Idle Editor Start!");
		        	SEND_EVENT(idle_get_window(), IDLE_START_DIALLING_NUMBER, 0, &keycode);
				}	
			}
		}
        break;
	}

	/* always consume the event
	*/
    return MFW_EVENT_CONSUMED;
}







/*******************************************************************************

 $Function:    	bookMenuKbdLongCB

 $Description:	keyboard long press event handler
 
 $Returns:		MFW_EVENT_CONSUMED always

 $Arguments:	e, event, k, key handle
 
*******************************************************************************/

static int bookMenuKbdLongCB( MfwEvt e, MfwKbd *k )
{
    T_MFW_HND       window		= mfwParent( mfw_header() );
	T_MFW_WIN		*win_data	= ((T_MFW_HDR *)window)->data;
	tBookStandard	*data		= (tBookStandard *) win_data->user;

	TRACE_FUNCTION( "bookMenuKbdLongCB()" );

	if ( ( e & KEY_CLEAR ) && ( e & KEY_LONG ) )
	{	mnuEscape( data->menu );
		return MFW_EVENT_CONSUMED;
	}
	/*if long press on END key*/
	 if ( ( e & KEY_HUP) && ( e & KEY_LONG ) )      
		{
			U8 keycode;
			U8 uMode;			
			uMode = dspl_Enable(0);

			// keypadLock will not activate anymore
			data->status_of_timer = TRUE;		

        	//return to idle screen

			keycode = k->code;
        	
			if (data->Callback)
				(data->Callback) (data->parent_win, NULL, NULL);
			
			bookMenuDestroy(data->win);
			stopRingerVolSettingInactivityTimer();
			
        	SEND_EVENT(idle_get_window(), IDLE_UPDATE, 0, &keycode);
        			
			dspl_Enable(uMode);
		}
			
	return MFW_EVENT_CONSUMED;
}






/*******************************************************************************

 $Function:    	bookMenuCreate

 $Description:	Create the menu window
 
 $Returns:		handle of newly created window, or NULL if error

 $Arguments:	parent, handle of the parent window
 
*******************************************************************************/

static T_MFW_HND bookMenuCreate( MfwHnd parent )
{
	T_MFW_WIN		*parent_win_data = ( (T_MFW_HDR *) parent )->data;
	T_phbk			*phbk = (T_phbk *)parent_win_data->user;
	T_MFW_WIN		*win_data;
	tBookStandard	*data;

	TRACE_FUNCTION( "bookMenuCreate()" );

	/* allocate memory for our control block
	*/
	if ( ( data = (tBookStandard *) ALLOC_MEMORY( sizeof( tBookStandard ) ) ) == NULL )
		return NULL;

	/* Create the window if we can
	*/
	if ( ( data->win = win_create( parent, 0, E_WIN_VISIBLE, (T_MFW_CB) bookMenuWindowCB ) ) == NULL )
	{
		FREE_MEMORY( (void *)data, sizeof( tBookStandard ) );
		return NULL;
	}

	/* Okay, we have created the control block and the window, so
	   we now need to configure the dialog and data pointers
	*/
    data->mmi_control.dialog	= (T_DIALOG_FUNC) bookMenuDialog;
    data->mmi_control.data		= data;
    win_data					= ((T_MFW_HDR *)data->win)->data;
 	win_data->user				= (void *) data;
	data->phbk					= phbk;
	data->parent_win			= parent;

	/* create keyboards and menus for our window
	*/
	data->kbd		= kbdCreate  ( data->win, KEY_ALL,            (MfwCb) bookMenuKbdCB );
    data->kbd_long	= kbdCreate  ( data->win, KEY_ALL | KEY_LONG, (MfwCb) bookMenuKbdLongCB );
    data->menu_tim  = tim_create (data->win, THREE_SECS,          (MfwCb) bookMenuTimCB);

	data->Callback = NULL;						// Use standard menu callback (changed by sending event ADD_CALLBACK)
	
	/* And return the handle of the newly created window
	*/
    return data->win;
}





/*******************************************************************************
                                                                              
                                Public Methods
                                                                              
*******************************************************************************/

/*******************************************************************************

 $Function:     bookMenuStart

 $Description:	tbd
 
 $Returns:		tbd

 $Arguments:	tbd
 
*******************************************************************************/

T_MFW_HND bookMenuStart( MfwHnd parent, MfwMnuAttr *menuAttr,SHORT reason )
{
	T_MFW_HND win;

	win = bookMenuCreate( parent );

    if ( win != NULL )
        SEND_EVENT( win, MENU_INIT, reason, (MfwMnuAttr *) menuAttr );
    return win;
}



/*******************************************************************************

 $Function:    	bookMenuDestroy

 $Description:	Destroy the menu window 
 
 $Returns:		none

 $Arguments:	window, handle of the window being destroyed
 
*******************************************************************************/

void bookMenuDestroy( MfwHnd window )
{
	T_MFW_WIN		*win_data	= ((T_MFW_HDR *)window)->data;
	tBookStandard	*data		= (tBookStandard *) win_data->user;
	char			test_string[40];

	TRACE_FUNCTION( "bookMenuDestroy()" );

	if (window == NULL)
	{
		TRACE_EVENT ("Error : Called with NULL Pointer");
		return;
	}

	if ( data )
	{

		/* If we have the root window here then we use the main phone
		   book destroy method
		*/
		if ( data->phbk->root_win == window )
		{
			bookPhonebookDestroy( data->phbk->win );
			return;
		}

		/* Otherwise clear down our pointers and free the current
		   menu window structures
		*/
		if ( data->phbk->search_win == window )
			data->phbk->search_win = 0;
		if ( data->phbk->menu_main_win == window )
			data->phbk->menu_main_win = 0;
		if ( data->phbk->menu_options_win == window )
			data->phbk->menu_options_win = 0;
		if ( data->phbk->menu_options_win_2 == window )
			data->phbk->menu_options_win_2 = 0;
		if ( data->phbk->menu_call_options_win == window )
			data->phbk->menu_call_options_win = 0;
		if ( data->phbk->menu_call_options_win_2 == window )
			data->phbk->menu_call_options_win_2 = 0;

		winDelete( data->win );
		FREE_MEMORY( (void *)data, sizeof( tBookStandard ) );
		sprintf(test_string, "Bookmenudestroy: %d", mfwCheckMemoryLeft());
		TRACE_EVENT(test_string);
	}

}






/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/
