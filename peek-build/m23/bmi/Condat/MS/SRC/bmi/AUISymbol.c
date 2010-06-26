/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI                                                            
 $Module:		MMI
 $File:		    AUISymbol.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		                                                         
                                                                               
********************************************************************************
                                                                              
 Description: Symbol screen for AUI editor

  

********************************************************************************

 $History: AUISymbol.c

	Mar 23, 2005  REF: CRR 28490  xnkulkar
	Description:  Golite- 5-way navigation behaves wrongly while selecting special characters 
			   in the text editor screen.The background screen goes blank when the special 
			   characters screen comes up.
	Solution:   Commented 'dspl_ClearAll()' to make the behaviour consistent with normal variant. 

	   
 $End
  // Issue Number : SPR#12817 on 26/03/04 by Deepa M.D.

*******************************************************************************/

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

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
#include "mfw_tim.h"
#include "mfw_phb.h"
#include "mfw_sms.h"
#include "mfw_ss.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_lng.h"
#include "mfw_sat.h"
#include "mfw_kbd.h"
#include "mfw_nm.h"
#include "mfw_cm.h"

#include "dspl.h"

#include "ksd.h"
#include "psa.h"

#include "MmiDummy.h"
#include "MmiMmi.h"

#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "MmiIdle.h"

#include "cus_aci.h"
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


#include "mmiColours.h"

#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"


//data structure for punctuation symbols selection screen
typedef struct
{
    T_MMI_CONTROL		mmi_control;
    T_MFW_HND			parent;
    T_MFW_HND			win;
    T_ED_DATA			*editor;
    T_MFW_HND			kbd;

    T_AUI_EDITOR_DATA	editor_data;
    USHORT				cursor_position;
    USHORT	 			output_symbol;
    USHORT				return_event;
}
T_AUI_SYMBOL_INFO;

//events in main editor screen
typedef enum
{
	EDITOR_INIT,
	EDITOR_DEINIT,
	EDITOR_UPDATE
} T_SYMBOL_EVENTS;

/*********************************************************************
**********************************************************************

DYNAMIC EDITOR WINDOW. DECLARATION

*********************************************************************
**********************************************************************/


/* Local function protoypes */

static T_MFW_HND AUI_symbol_Create (T_MFW_HND  parent);
static int AUI_symbol_WinCb (T_MFW_EVENT event, T_MFW_WIN * win);
static int AUI_symbol_KbdCb (T_MFW_EVENT event, T_MFW_KBD *keyboard);
static void AUI_symbol_ExecCb (T_MFW_HND win, USHORT event, USHORT value, void * symbol);


/*******************************************************************************

 $Function:    	AUI_symbol_Destroy

 $Description:	Destroy thesymbol screen
 
 $Returns:		None.

 $Arguments:	window handle
 
*******************************************************************************/

void AUI_symbol_Destroy (T_MFW_HND window)
{
    T_MFW_WIN			*win	= ((T_MFW_HDR *)window)->data;
    T_AUI_SYMBOL_INFO	*data	= (T_AUI_SYMBOL_INFO *)win->user;

    TRACE_FUNCTION ("AUI_symbol_Destroy()");

    /* Send character to editor window */
    
	SEND_EVENT (data->parent, data->return_event, (SHORT)data->output_symbol, NULL); 
    
    if (data)
	{   /*SPR 2520, need to destroy ATB editor*/ 
		ATB_edit_Destroy( data->editor);
		
		/* Delete WIN Handler */
		win_delete (data->win);

        /* Free Memory */
		FREE_MEMORY ((void *)data, sizeof (T_AUI_SYMBOL_INFO));
	}
    else
    {
		TRACE_EVENT ("AUI_symbol_Destroy() called twice");
		return ;
	}
}


/*******************************************************************************

 $Function:    	AUI_symbol_Start

 $Description:	Starts the symbol screen
 
 $Returns:		handle

 $Arguments:	parent window, editor data
 
*******************************************************************************/
T_MFW_HND AUI_symbol_Start (T_MFW_HND parent, USHORT return_event)
{
    T_MFW_HND win;
  

    TRACE_FUNCTION ("AUI_symbol_Start()");

    win = AUI_symbol_Create (parent);
	
    if (win NEQ NULL)
	{
	    SEND_EVENT (win, EDITOR_INIT, return_event, 0);
	}
    return win;
}


/*******************************************************************************

 $Function:    	LatinEditorcreate

 $Description:	Creation of a symbol screen
 
 $Returns:		handle of new window

 $Arguments:	parent window
 
*******************************************************************************/

static T_MFW_HND AUI_symbol_Create (T_MFW_HND  parent)
{
    T_AUI_SYMBOL_INFO *  data = (T_AUI_SYMBOL_INFO *)ALLOC_MEMORY (sizeof (T_AUI_SYMBOL_INFO));
    T_MFW_WIN     *  win;

    TRACE_FUNCTION ("AUI_symbol_Create()");

    /* Create window handler */

    data->win = winCreate (parent, 0, E_WIN_VISIBLE, (T_MFW_CB)AUI_symbol_WinCb);

    if (data->win EQ NULL)
    {
        return NULL;
    }
 
     /* Connect the dialog data to the MFW-window */

	data->mmi_control.dialog  = (T_DIALOG_FUNC)AUI_symbol_ExecCb;
	data->mmi_control.data	= data;
	data->parent			= parent;
	win						= ((T_MFW_HDR *)data->win)->data;
	win->user				= (void *)data;
    		
	return data->win;
}


/*******************************************************************************

 $Function:    	AUI_symbol_ExecCb

 $Description:	handles events for symbol window
 
 $Returns:		None.

 $Arguments:	window, event, value, editor data
 
*******************************************************************************/

static void AUI_symbol_ExecCb (T_MFW_HND win, USHORT event, USHORT value, void *parameter)
{
    T_MFW_WIN			* win_data		= ((T_MFW_HDR *)win)->data;
    T_AUI_SYMBOL_INFO	* data			= (T_AUI_SYMBOL_INFO *)win_data->user;

 //   T_MFW_HND			parent_win		= data->parent;   // RAVI
 //   USHORT				Identifier		= data->editor_data.Identifier;   // RAVI
 //   T_AUI_EDIT_CB		Callback		= data->editor_data.Callback;  // RAVI

	/* List of symbols */
	/*SPR 1955, replaced £ sign with our ASCII code for it*/
	/*SPR2175, added previously un-enterable symbols into symbol table*/
	static char symStr[] = ". , ? ! : ; - + # * ( ) \' \" _ @ & $ \x9C % / < > = \x8F \x86 \xA5 \xA4 \x2 \x7 \x4 \x9D \xe8 \xE2 \xEF \xEA \xE3 \x5 \xe4 \xe9 \xF0 \x92 \x91 \x01 \xAD \xA8 \x06";
  
    TRACE_FUNCTION ("AUI_symbol_ExecCb()");

    switch (event)
	{
        case EDITOR_INIT:
			/* Get the return event that has been passed */

			data->return_event = value;
			
            /* Create the keyboard handler */
            data->kbd	= kbdCreate (data->win, KEY_ALL,(T_MFW_CB)AUI_symbol_KbdCb);

            /* Initialise editor variables */
            
        	AUI_edit_SetDefault(&data->editor_data);
        	AUI_edit_SetDisplay(&data->editor_data, ZONE_MIDDLE_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
			AUI_edit_SetBuffer(&data->editor_data, ATB_DCS_ASCII, (UBYTE *)symStr, strlen(symStr)+1);
			AUI_edit_SetMode(&data->editor_data, ED_MODE_ALPHA, ED_CURSOR_BLOCK);
			data->editor  = ATB_edit_Create(&data->editor_data.editor_attr,0);
            ATB_edit_Init(data->editor);  
                             
		   	ATB_edit_Char(data->editor, ctrlTop, TRUE);
            win_show(data->win);
			break;
      
        case EDITOR_DEINIT:
			AUI_symbol_Destroy (data->win);   
            break;

        default:
            break;
    }
}


/*******************************************************************************

 $Function:    	AUI_symbol_WinCb

 $Description:	Editor window event handler.
 
 $Returns:		status int

 $Arguments:	event, window
 
*******************************************************************************/
static int AUI_symbol_WinCb (T_MFW_EVENT event, T_MFW_WIN * win)
{
    T_AUI_SYMBOL_INFO * data = (T_AUI_SYMBOL_INFO *)win->user;

	TRACE_FUNCTION ("AUI_symbol__win_cb()");

    if (!data)
	{
		return MFW_EVENT_CONSUMED;
	}
   
    switch (event)
	{
        case E_WIN_VISIBLE: /* window is visible  */
            if (win->flags & E_WIN_VISIBLE)
            {
// Mar 23, 2005  REF: CRR 28490  xnkulkar
// Description:  Golite- 5-way navigation behaves wrongly while selecting special characters 
//			   in the text editor screen.The background screen goes blank when the special 
//			   characters screen comes up.
// Solution:   Commented 'dspl_ClearAll()' to make the behaviour consistent with normal variant. 
//#ifndef COLOURDISPLAY
                /* Clear Screen */
//                dspl_ClearAll();
//#endif

                /* Print the information */
				ATB_edit_Show(data->editor); 
			
			// change by Sasken ( Deepa M D) on March 26th 2004
			// Issue Number : MMI-SPR-12817
			// Subject: Wrong softkey in the symbol table.
			// Bug : While selecting the symbols in the editor 
			// for the sms write screen, the softkey dispalyed 
			// was "Options" instead of "Select"
			// Solution: Commenting the compilation flags COLOURDISPLAY.	
//#ifndef COLOURDISPLAY
                /* Print the softkeys */
				displaySoftKeys(TxtSoftSelect, TxtSoftBack);
//#endif
            }
            break;
        default:
            return MFW_EVENT_REJECTED;
	}
	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	AUI_symbol__kbd_cb

 $Description:	Symbol screen keyboard event handler
 
 $Returns:		None.

 $Arguments:	event, keyborad data
 
*******************************************************************************/

static int AUI_symbol_KbdCb (T_MFW_EVENT event, T_MFW_KBD *keyboard)
{
    T_MFW_HND       win        = mfw_parent (mfw_header());
    T_MFW_WIN     * win_data   = ((T_MFW_HDR *)win)->data;
    T_AUI_SYMBOL_INFO * data       = (T_AUI_SYMBOL_INFO *)win_data->user;

    TRACE_FUNCTION ("AUI_symbol__kbd_cb()");

    switch (keyboard->code)
    {
    /* Use left and right keys in colour display, not up and down */
    
#ifdef COLOURDISPLAY
		case KCD_MNURIGHT: 
            ATB_edit_Char(data->editor,ctrlRight, FALSE);
            ATB_edit_Char(data->editor,ctrlRight, TRUE);
            ATB_edit_Show(data->editor);
            break;

        case KCD_MNULEFT:
        	ATB_edit_Char(data->editor,ctrlLeft, FALSE);
          	ATB_edit_Char(data->editor,ctrlLeft, TRUE);
        	ATB_edit_Show(data->editor);
            break;

		case KCD_MNUUP: 
            ATB_edit_Char(data->editor,ctrlUp, TRUE);
            ATB_edit_Show(data->editor);
            break;

        case KCD_MNUDOWN:           
          	ATB_edit_Char(data->editor,ctrlDown, TRUE);
        	ATB_edit_Show(data->editor);
            break;
#else
        case KCD_MNUUP: 
            ATB_edit_Char(data->editor,ctrlRight, FALSE);
            ATB_edit_Char(data->editor,ctrlRight, TRUE);
            ATB_edit_Show(data->editor);
            break;

        case KCD_MNUDOWN:           
          	ATB_edit_Char(data->editor,ctrlLeft, FALSE);
          	ATB_edit_Char(data->editor,ctrlLeft, TRUE);
        	ATB_edit_Show(data->editor);
            break;
#endif

#ifdef COLOURDISPLAY
		case KCD_MNUSELECT:
#endif
        case KCD_LEFT:
			data->output_symbol = ATB_string_GetChar(&data->editor_data.editor_attr.text, data->editor->cursor.pos);
			AUI_symbol_Destroy(data->win);
			return MFW_EVENT_CONSUMED;
//            break;  // RAVI

		case KCD_HUP:
        case KCD_RIGHT:
            data->output_symbol = NULL;
			AUI_symbol_Destroy(data->win);
			return MFW_EVENT_CONSUMED;
//            break;  // RAVI
    }

	/* If we've landed on a space or an end-of-string, move cursor left */
	
	if ( ( ATB_edit_GetCursorChar(data->editor, 0)==UNICODE_SPACE
		|| ATB_edit_GetCursorChar(data->editor, 0)==UNICODE_EOLN )
		&& data->editor->cursor.pos>0 )
	{
			ATB_edit_Char(data->editor, ctrlLeft, TRUE);
//  Apr 27, 2004        REF: CRR MMI-SPR-12353  Rashmi C N(Sasken) 
//		When we encounter a white space the coordinates were updated but not the screen, hence an ATB_edit_Show is needed at this point  
		 ATB_edit_Show(data->editor);		
	}			
    return MFW_EVENT_CONSUMED;
}
