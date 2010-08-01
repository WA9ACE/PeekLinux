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
#include "MmiEmoMenuWindow.h"



#include "MmiEmo.h"
#include "MmiBookController.h"

#include "mmiColours.h"

//Jul 21, 2004        REF: CRR 13348  xvilliva
//This is a global variable used to store the handle, when 
//phonebook menu is created.
T_MFW_HND gPhbkMenu = NULL;
//Sep 29, 2004        REF: CRR 25041  xvilliva  
extern int menuFDN;


/*******************************************************************************
                                                                              
                                Private Methods
                                                                              
*******************************************************************************/

/*******************************************************************************

 $Function:    	emoMainDialog

 $Description:	Dialog function for the phone book application
 
 $Returns:		none

 $Arguments:	win, window handle
                event, event to be handled
				value, not used
				parameter, reason for call
 
*******************************************************************************/


extern T_MFW_HND emoMenuStart( MfwHnd parent, MfwMnuAttr *menuAttr,SHORT reason);

void emoMainDestroy( MfwHnd window )
{
        T_MFW_WIN       *win  = ((T_MFW_HDR *)window)->data;
        T_phbk          *data = (T_phbk *) win->user;

        TRACE_FUNCTION ("emoMainDestroy()");

        if ( data )
        {
                /* need to clean up all of the subordinate windows which
                   may have been created during the phone book application
                */
                data->root_win = 0;
                if (data->menu_main_win)
                        emoMenuDestroy(data->menu_main_win);

                if (data->menu_options_win)
                        emoMenuDestroy(data->menu_options_win);

                if (data->menu_options_win_2)
                        emoMenuDestroy(data->menu_options_win_2);

                if (data->menu_call_options_win)
                        emoMenuDestroy(data->menu_call_options_win);

                if (data->menu_call_options_win_2)
                {
                        menuFDN = 0 ;//         Sep 29, 2004        REF: CRR 25041  xvilliva
                        emoMenuDestroy(data->menu_call_options_win_2);
                }

                /* delete the window and free any allocated memory
                */
                winDelete (data->win);
                FREE_MEMORY( (void *) data, sizeof( T_phbk ) );
        }
        //Jul 21, 2004        REF: CRR 13348  xvilliva
        //While we destroy the menu assign the handle to NULL.
        gPhbkMenu = NULL;

}

static void emoMainDialog( T_MFW_HND win, USHORT event, SHORT value, void *parameter )
{
    T_MFW_WIN   *win_data	= ((T_MFW_HDR *) win)->data;
    T_phbk      *data		= (T_phbk *) win_data->user;
	SHORT		*reason		= (SHORT *) parameter;

    TRACE_FUNCTION ("emoMainDialog()");
    switch (event)
	{
		case PHBK_INIT:
		{
			switch ( *reason )
			{
				case PhbkNormal:
				{
					/* Normal initialisation
					*/
                    			data->menu_main_win         	= emoMenuStart( data->win, bookMainMenuAttributes(), *reason);
					data->root_win			= data->menu_main_win;
				}
				break;

				case PhbkMainMenu:
				{
					/* Normal initialisation
					*/
					data->current.index		= 1;
                    			data->menu_main_win         	= emoMenuStart( data->win, bookMainMenuAttributes(), *reason);
					data->root_win			= data->menu_main_win;
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
		default:
		{
			/* No default action required
			*/
		}
	}
}









/*******************************************************************************

 $Function:    	emoMainWindowCB

 $Description:	Window call back for the phone book application
 
 $Returns:		MFW_EVENT_CONSUMED if event is MfwWinVisible, otherwise
				MFW_EVENT_PASSED

 $Arguments:	e, event, w, window handle
 
*******************************************************************************/

static int emoMainWindowCB ( MfwEvt e, MfwWin *w )
{
        emo_printf("emoMainWindowCB() event %d", e);
	return ( e == MfwWinVisible ) ? MFW_EVENT_CONSUMED : MFW_EVENT_PASSED;
}








/*******************************************************************************

 $Function:    	emoMainCreate

 $Description:	Create the phone book application
 
 $Returns:		Handle of the newly created window or NULL if error

 $Arguments:	parent, handle of parent window
 
*******************************************************************************/

static T_MFW_HND emoMainCreate( MfwHnd parent )
{
	T_MFW_WIN   *win_data;
	T_phbk		*data;

	TRACE_FUNCTION ("emoMainCreate()");

	if ( ( data = (T_phbk *) ALLOC_MEMORY( sizeof( T_phbk ) ) ) == NULL )
		return NULL;

	if ( ( data->win = win_create( parent, 0, E_WIN_VISIBLE, (T_MFW_CB) emoMainWindowCB ) ) == NULL )
	{
		FREE_MEMORY( (void *) data, sizeof( T_phbk ) );
		return NULL;
	}

    /* Create window handler
    */
    	data->mmi_control.dialog   = (T_DIALOG_FUNC) emoMainDialog;
	data->mmi_control.data     = data;
    	win_data                   = ((T_MFW_HDR *)data->win)->data;
 	win_data->user             = (void *)data;
	winShow(data->win);

	/* initialise the options structure
	*/
	data->root_win					= 0;
    	data->menu_main_win				= 0;
	data->calls_list_win				= 0;
	data->menu_options_win				= 0;
	data->menu_options_win_2			= 0;
	data->menu_call_options_win			= 0;
	data->menu_call_options_win_2			= 0;
	data->name_details_win				= 0;
	data->search_win				= 0;
	data->parent_win				= parent;
	data->phbk					= data;

    /* return window handle
    */
    return data->win;
}





/*******************************************************************************
                                                                              
                                Public methods
                                                                              
*******************************************************************************/

/*******************************************************************************

 $Function:    	emoMainStart

 $Description:	Starts up the phone book application
 
 $Returns:		Handle of window or NULL if error

 $Arguments:	parent, window, reason, for invocation
 
*******************************************************************************/

T_MFW_HND emoMainStart( MfwHnd parent, int reason )
{
	T_MFW_HND win;

    if ( ( win = emoMainCreate( parent ) ) != NULL )
    {
        SEND_EVENT( win, PHBK_INIT, 0, (int *) &reason );
	 if(reason == PhbkMainMenu)
    		 gPhbkMenu = win;//Jul 21, 2004        REF: CRR 13348  xvilliva
    }
    return win;
}






/*******************************************************************************

 $Function:    	emoMainDestroy

 $Description:	Destroys the phone book application
 
 $Returns:		none

 $Arguments:	window, to be destroyed
 
*******************************************************************************/
/*******************************************************************************
                                                                              
                                End Of File
                                                                              
*******************************************************************************/
