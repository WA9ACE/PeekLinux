
/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MmiDialogs
 $File:		    MmiDialogs.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:MMI Information dialogue handling.
 

                        
********************************************************************************

 $History: MmiDialogs.c

	May 23, 2007    DR: OMAPS00127483  x0pleela
	Description:   Phonelock - Unable to unlock the phone during incoming call with ringer...
	Solution: Made changes in the function dialog_info_win_resize_cb() to restart the ringer set for 
			incoming call or alarm after thr phone is unlocked.
 
	Apr 03, 2007    ER: OMAPS00122561  x0pleela
	Description:   [ACI] Phone lock feature has to be supported by ACI
	Solution: Phone Lock ER implementation 
 
	x0066814 06 Dec 2006, OMAPS00106825
	The window height calculation related to wrapping should be skipped when no 
	string wrapping is done

	Mar 11, 2006   REF:DR:OMAPS00061467  x0035544
	Description:   27.22.4.13.3 SET UP CALL (display of icons) fails.
	Solution: Assigned the default values for icon data  for the structure display_info in the function 
	dlg_zeroDisplayData(). copied icon data in to the structure data->IconData and provided functionality 
	in the function dialog_info_win_resize_cb() to display icon data appropriately on the dialog screen for SAT
	call setup.
	
       Sep 06 2005  REF:  MMI-SPR-33548  x0012849 Jagan
       Description : A dialog  with long message  is not displayed properly.
       Solution : The no of lines in a dialog is increased from 4 to 15.
       
 	Feb 24, 2005 REF: CRR MMI-SPR-29102 xnkulkar
       Description: The phone resets when user presses the ''End All' key while the SMSs are being deleted. 
       Solution: As the user has already confirmed that all SMS messages are to be deleted, ''End All' key 
       		press is ignored till the delete operation is over.
            xrashmic 08 Feb, 2005 MMI-SPR-27853
       Moved the T_dialog_info structure from MmiDialogs.c to MmiDialogs.h     


       xrashmic 08 Feb, 2005 MMI-SPR-27853
       Moved the T_dialog_info structure from MmiDialogs.c to MmiDialogs.h     

	Aug 25, 2004    REF: CRR 24904   Deepa M.D
	Bug:cleanup compile switches used for Go-lite
	Fix:COLOURDISPLAY compiler switch has been added to the functions which are 
	used only for the normal color build.

	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX


	Sep 01, 2004    REF: CRR 21380  Deepa M.D 
	Bug:Not getting proper display, When Messages Memory is Full.
	Fix:Assigning TextId and TextString will not display both the messages.
	Hence we need to use TextId and TextString2 or TextId2 and TextString

	Jun 04,2004 CRR:13601 xpradipg - SASKEN
	Description: CFO interrogation display : Timer to long
	Fix: Timer is enabled for the information dialog. It times out after 5 seconds from the last key
	press.
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


#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_tim.h"
#include "mfw_phb.h"
#include "mfw_sms.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_lng.h"
#include "mfw_sat.h"
#include "mfw_kbd.h"
#include "mfw_nm.h"

//x0pleela 15 May, 2007 DR: OMAPS00127483
#ifdef FF_PHONE_LOCK
#ifdef FF_MIDI_RINGER  
#include "mfw_midi.h"
#endif //FF_MIDI_RINGER 
#endif /* FF_PHONE_LOCK */
#include "dspl.h"

#include "ksd.h"
#include "psa.h"

#include "MmiMain.h"
#include "MmiDummy.h"
//JVJE #include "MmiMsgsEdit.h"
//#include "MmiLists.h"
#include "MmiMmi.h"

#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"

/* JF Modified
*/
#include "MmiBookController.h"
#include "cus_aci.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


#include "mmiColours.h"

//x0pleela 27 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
#include "MmiPins.h"
//x0pleela 15 May, 2007 DR: OMAPS00127483
#include "MmiSounds.h"
EXTERN T_call call_data; 
extern T_MFW_HND	phlock_win_handle;			/* to hold the win handle of phone unlock editor window 	*/
extern T_MFW_HND	phlock_kbd_handle;			/* to hold the kbd handle of phone unlock editor window 	*/
extern T_MFW_HND	phlock_alarm_win_handle; 	/* to hold the win handle of alarm window 				*/
extern T_MFW_HND   phlock_dialog_mtc_win_handle;/* to hold the win handle of MT call dialog window 		*/
extern int 			phlock_alarm;				/* flag to check whether alarm event has occured or not 	*/ 
int 					phlock_mtc_anim_time;		/* to hold the display info for MTC while in locked state     */

//x0pleela 15 May, 2007 DR: OMAPS00127483
#ifdef FF_MMI_AUDIO_PROFILE
extern UBYTE mfwAudPlay;//flag for audio 
#endif

static T_MFW_HND phlock_dialog_info_create (T_MFW_HND parent_win);
static int phlock_dialog_info_win_resize_cb (T_MFW_EVENT event, T_MFW_WIN * win);
void phlock_dialog_info (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static T_MFW_CB phlock_dialog_info_tim_cb (T_MFW_EVENT event, T_MFW_TIM *tc);
static int phlock_dialog_info_kbd_cb (T_MFW_EVENT event, T_MFW_KBD *keyboard);
static int phlock_dialog_info_kbd_long_cb(T_MFW_EVENT event, T_MFW_KBD *keyboard);
void phlock_dialog_info_destroy (T_MFW_HND own_window);

#endif //FF_PHONE_LOCK

void dlg_initDisplayData_type( T_DISPLAY_DATA *displayInfo, U32 displayCol, int dlgType, T_BITMAP* dlgBitmap , int dlgSKFormat );

/*
 * START NEW PART
 */

static T_MFW_HND   dialog_info_create    (T_MFW_HND  parent_win);
static void        dialog_info           (T_MFW_HND  win,
                                          USHORT event, SHORT value, void * parameter);

/* x0039928 - Lint warning fix 
static int         dialog_info_win_cb    (T_MFW_EVENT event, T_MFW_WIN * win); */
static int 	dialog_info_win_resize_cb (T_MFW_EVENT event, T_MFW_WIN * win);

static T_MFW_CB    dialog_info_tim_cb    (T_MFW_EVENT event, T_MFW_TIM * tc);
static int         dialog_info_kbd_cb    (T_MFW_EVENT event, T_MFW_KBD * keyboard);
static int         dialog_info_kbd_long_cb (T_MFW_EVENT event, T_MFW_KBD * keyboard);
void mmi_dialogs_insert_animation_CB (T_MFW_EVENT event, T_MFW_TIM *tc);
void mmi_dialogs_animation_new_CB (T_MFW_EVENT event, T_MFW_TIM *tc);


MfwRect calcLayout( int dlgType, int dataSx, int dataSy, int bmpSx, int bmpSy)
{
	MfwRect area;
	int px,py;
	int sx =0,sy = 0;

	TRACE_EVENT_P5("dlgType,= %d, dataSx = %d, dataSy = %d, bmpSx = %d, bmpSy = %d", dlgType, dataSx, dataSy, bmpSx, bmpSy);

	/*Adjust area available for drawing by the border */
	switch (dlgType & DLG_BORDER_VERTICAL)
	{
		case DLG_BORDER_VERTICAL_0:	sx =bmpSx;			break;
		case DLG_BORDER_VERTICAL_1:	sx =bmpSx*6/8;		break;
		case DLG_BORDER_VERTICAL_2:	sx =bmpSx*4/8;		break;
		case DLG_BORDER_VERTICAL_3:	sx =bmpSx*3/8;		break;
	}
	if (sx < dataSx)
		sx = dataSx;
	px = 0;

	switch (dlgType & DLG_BORDER_HORIZONTAL)
	{
		case DLG_BORDER_HORIZONTAL_0:	sy =bmpSy;			break;
		case DLG_BORDER_HORIZONTAL_1:	sy =bmpSy*6/8;		break;
		case DLG_BORDER_HORIZONTAL_2:	sy =bmpSy*4/8;		break;
		case DLG_BORDER_HORIZONTAL_3:	sy =bmpSy*3/8;		break;
	}
	if (sy < dataSy)
		sy = dataSy;
	py = 0;

	switch (dlgType & (DLG_INFO_LAYOUT_LEFT	| DLG_INFO_LAYOUT_RIGHT))
	{
		case DLG_INFO_LAYOUT_CENTRE:	//display info in centre of display
			px = px + (sx-dataSx)/2;
			sx = dataSx;
			break;
		case DLG_INFO_LAYOUT_RIGHT:	//all info is displayed at the foot of trhe display
			px = px+(sx-dataSx);
			sx = dataSx;
			break;
		case DLG_INFO_LAYOUT_LEFT:		//display all info at the top of the bitmap
			sx = dataSx;
			break;
		default:						//display info over entire area					
			break;
	}
	
	switch (dlgType & (DLG_INFO_LAYOUT_BOTTOM	| DLG_INFO_LAYOUT_TOP))
	{
		case DLG_INFO_LAYOUT_CENTRE:							//display info in centre of display
			py = py + (sy-dataSy)/2;
			sy = dataSy;
			break;
		case DLG_INFO_LAYOUT_BOTTOM:	//all info is displayed at the foot of trhe display
			py = py+(sy-dataSy);
			sy = dataSy;
			break;
		case DLG_INFO_LAYOUT_TOP:		//display all info at the top of the bitmap
			sy = dataSy;
			break;
		default:						//display info over entire area					
			break;
	}
	area.px = px;	area.py = py;
	area.sx = sx;	area.sy = sy;	//Area sx/sy must be no smaller than dataSx/Sy
	return (area);
}


/*******************************************************************************

 $Function:    	info_dialog

 $Description:	Common information dialog
 
 $Returns:		Dialogue info win

 $Arguments:	parent win, display info
 
*******************************************************************************/
T_MFW_HND info_dialog (T_MFW_HND        parent_win,
                       T_DISPLAY_DATA * display_info)
{
  T_MFW_HND win;				
  TRACE_FUNCTION("info_dialog()");
  display_info->SoftKeyStrings = FALSE;
  
  win = dialog_info_create (parent_win);  			

  if (win NEQ NULL)
  {
    SEND_EVENT (win, DIALOG_INIT, 0, display_info);
  }
  return win;
}


/*******************************************************************************

 $Function:    	info_dialog_softkeystrings

 $Description:	Common information dialog, sets "softkeystrings" to TRUE
 
 $Returns:		Dialogue info win

 $Arguments:	parent win, display info, leftsoftkey, rightsoftkey
 
*******************************************************************************/
T_MFW_HND info_dialog_softkeystrings (T_MFW_HND parent_win, T_DISPLAY_DATA * display_info)
{
  T_MFW_HND win;

  display_info->SoftKeyStrings = TRUE;
  win = dialog_info_create (parent_win);

  if (win NEQ NULL)
  {
    SEND_EVENT (win, DIALOG_INIT, 0, display_info);
  }
  return win;
}


/*******************************************************************************

 $Function:    	dialog_info_create

 $Description:	Creation of an information dialog
 
 $Returns:		Dialogue info win

 $Arguments:	parent win
 
*******************************************************************************/
static T_MFW_HND dialog_info_create (T_MFW_HND parent_win)
{
  T_MFW_WIN     * win_data;
  T_dialog_info *  data = (T_dialog_info *)ALLOC_MEMORY (sizeof (T_dialog_info));
  TRACE_FUNCTION ("dialog_info_create()");

  data->info_win = win_create (parent_win, 0, E_WIN_VISIBLE
  						//x0pleela 06 Apr, 2007  ER: OMAPS00122561
  						#ifdef FF_PHONE_LOCK
  							|E_WIN_RESUME
						#endif //FF_PHONE_LOCK 							
  							, (T_MFW_CB)dialog_info_win_resize_cb);

  if (data->info_win EQ 0)
    return 0;

 /*
   * Create window handler
   */
  data->mmi_control.dialog   = (T_DIALOG_FUNC)dialog_info;
  data->mmi_control.data     = data;
  data->parent_win           = parent_win;
  win_data                   = ((T_MFW_HDR *)data->info_win)->data;
  win_data->user             = (void *)data;

  /*
   * return window handle
   */
  return data->info_win;
}


/*******************************************************************************

 $Function:    	dialog_info_destroy

 $Description:	Destruction of an information dialog
 
 $Returns:		void

 $Arguments:	win
 
*******************************************************************************/
void dialog_info_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN     * win  = ((T_MFW_HDR *)own_window)->data;
  T_dialog_info * data = (T_dialog_info *)win->user;

  TRACE_FUNCTION ("dialog_info_destroy()");

  if (own_window == NULL)
  {
	TRACE_EVENT ("Error :- Called with NULL Pointer");
	return;
  }

if (data)
  {
    /*
     * Exit TIMER & KEYBOARD Handle
     */
    kbd_delete (data->info_kbd);
    tim_delete (data->info_tim);

    if(data->animation_timer != 0)
    {
    	timDelete(data->animation_timer);

    	if (data->icon != 0)
    		icnDelete(data->icon);
    }

    /*
     * Delete WIN Handler
     */
    win_delete (data->info_win);
    /*
     * Free Memory
     */
    FREE_MEMORY ((void *)data, sizeof (T_dialog_info));
  }
//x0pleela 05 Apr, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
	if( phlock_alarm && phlock_alarm_win_handle )
	{
		phlock_alarm_win_handle = NULL;
	}

	if((  call_data.call_direction == MFW_CM_MTC )&&  phlock_dialog_mtc_win_handle )
	{
		 phlock_dialog_mtc_win_handle = NULL;
	}
#endif //FF_PHONE_LOCK

}

void dlg_zeroDisplayData( T_DISPLAY_DATA *displayInfo)
{

	TRACE_EVENT("dlg_zeroDisplayData()");
	if (displayInfo == NULL)
		return;
	memset (displayInfo, 0x00, sizeof(T_DISPLAY_DATA));
	
	displayInfo->LeftSoftKey 	= 0;
	displayInfo->RightSoftKey 	= 0;
	displayInfo->LSKString 		= NULL;
	displayInfo->RSKString 		= NULL;
	displayInfo->TextString 	= NULL;
	displayInfo->TextString2 	= NULL;
	
    	displayInfo->WrapStrings 	= 0;
	
    	displayInfo->TextId       	= 0;
    	displayInfo->TextId2      	= 0;
    
	displayInfo->SoftKeyStrings	= FALSE;
	
    	displayInfo->Identifier   	= 0;
	displayInfo->anim_time		= 0;
	displayInfo->number_of_frames	= 0;
	displayInfo->current_frame		= 0;
//x0035544 Feb 07, 2006 DR:OMAPS00061467
#ifdef FF_MMI_SAT_ICON
       displayInfo->IconData.width 	= 0;
       displayInfo->IconData.height	= 0;
	displayInfo->IconData.dst 		= NULL;
	displayInfo->IconData.selfExplanatory 	= FALSE;
#endif	
	dlg_initDisplayData_events( displayInfo, NULL, 0, 0);
    
 	/* GW#2294 COLOUR_STATUS has been used for all popups - change this to default instead of COLOUR_POPUP .*/
 	dlg_initDisplayData_type( displayInfo, COLOUR_STATUS, 0, NULL, 0 );

	/* GW #2294 02/07/03 - The default bitmap is set-up here to allow us to disable it for some dialogs (if required) */
	displayInfo->bgdBitmap = icon_getBgdBitmap(BGD_CLOUD);
    
}

void dlg_initDisplayData_TextId( T_DISPLAY_DATA *displayInfo, int lsk, int rsk, int str1, int str2, int colId)
{
	if (displayInfo == NULL)
		return;
	dlg_zeroDisplayData(displayInfo);
	displayInfo->LeftSoftKey	= (USHORT)lsk;
	displayInfo->RightSoftKey	= (USHORT)rsk;
	displayInfo->TextId 		= str1;
	displayInfo->TextId2 		= str2;
	displayInfo->displayCol 	= colId ;
	
}

void dlg_initDisplayData_TextStr( T_DISPLAY_DATA *displayInfo, int lsk, int rsk,char *str1, char *str2, int colId)
{
	if (displayInfo == NULL)
		return;
	dlg_zeroDisplayData(displayInfo);
	displayInfo->LeftSoftKey 	= (USHORT)lsk;
	displayInfo->RightSoftKey 	= (USHORT)rsk;
	displayInfo->TextString 	= str1;
	displayInfo->TextString2 	= str2;
	displayInfo->displayCol 	= colId ;
}

void dlg_initDisplayData_events( T_DISPLAY_DATA *displayInfo, T_VOID_FUNC callback, int timer, T_MFW_EVENT keyEvents)
{
	displayInfo->Callback 		= callback;
	displayInfo->Time			= timer;
	displayInfo->KeyEvents		= keyEvents;
}

void dlg_initDisplayData_type( T_DISPLAY_DATA *displayInfo, U32 displayCol, int dlgType, T_BITMAP* dlgBitmap, int dlgSkFormat )
{
	displayInfo->displayCol	= displayCol;
	displayInfo->dlgType	= dlgType;
	displayInfo->bgdBitmap	= dlgBitmap;
}

/*******************************************************************************

 $Function:    	dialog_info

 $Description:	Dialog function for information dialog
 
 $Returns:		void

 $Arguments:	win, window handle event, value, parameter

*******************************************************************************/
void dialog_info (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
  T_MFW_WIN      * win_data     = ((T_MFW_HDR *)win)->data;
  T_dialog_info  * data         = (T_dialog_info *)win_data->user;
  T_DISPLAY_DATA * display_info = (T_DISPLAY_DATA *)parameter;  
  TRACE_FUNCTION ("dialog_info()");

  switch (event)
  {
    case DIALOG_INIT:
      /*
       * Initialize dialog
       */
#if defined (WIN32)
      {
		  /***************************Go-lite Optimization changes Start***********************/
		  
		  //Aug 16, 2004    REF: CRR 24323   Deepa M.D
		  TRACE_EVENT_P1( "Identifier %d", display_info->Identifier);
		  /***************************Go-lite Optimization changes end***********************/

      }
#endif
      /*
       * Create timer and keyboard handler
       */
	data->info_tim             = tim_create (win, display_info->Time, (T_MFW_CB)dialog_info_tim_cb);	
	data->info_kbd             = kbd_create (win, KEY_ALL, (T_MFW_CB)dialog_info_kbd_cb);
	data->info_kbd_long	= kbd_create(win,KEY_ALL|KEY_LONG,(MfwCb)dialog_info_kbd_long_cb);

//x0pleela 05 Apr, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
  	if( phlock_win_handle && 
		( ( phlock_alarm) || ( call_data.call_direction == MFW_CM_MTC ) ) )
	{
		TRACE_EVENT("dialog_info: Bef deleting kbd_delete(data->info_kbd);");
		if( phlock_alarm )
			phlock_alarm_win_handle = win;

		if( call_data.call_direction == MFW_CM_MTC )
			phlock_dialog_mtc_win_handle = win;
		
		kbd_delete(data->info_kbd);
		data->info_kbd = phlock_kbd_handle; //set the kbd handle of unlock screen
	}
#endif //FF_PHONE_LOCK

      /*
       * Store any other data
       */
      data->KeyEvents			 = display_info->KeyEvents;
	  data->TextString           = display_info->TextString;
      data->LeftSoftKey          = display_info->LeftSoftKey;
      data->RightSoftKey         = display_info->RightSoftKey;
      data->SoftKeyStrings		= display_info->SoftKeyStrings;	// SH - TRUE if using strings rather than IDs
      data->LSKString			= display_info->LSKString;		// Text for left soft key
      data->RSKString			= display_info->RSKString;		// Text for right soft key
      data->TextId               = display_info->TextId;
      data->Identifier           = display_info->Identifier;
      data->Callback             = display_info->Callback;
      data->TextString2          = display_info->TextString2;
      data->TextId2              = display_info->TextId2;
	  data->WrapStrings			= display_info->WrapStrings;	//GW-SPR#762

	  data->dlgCol				= display_info->displayCol;
	  data->dlgType				= display_info->dlgType;
	  data->dlgBgdBitmap		= display_info->bgdBitmap;
	  data->dlgSkFormat			= 0;
	  data->current_frame		= display_info->current_frame;
	  data->number_of_frames	= display_info->number_of_frames;
	  data->icon_array			= (MfwIcnAttr*)display_info->icon_array;
	  data->animation_timer		= 0;
//x0035544 Feb 07, 2006 DR:OMAPS00061467
#ifdef FF_MMI_SAT_ICON
     if(display_info->IconData.dst != NULL)
	{
         data->IconData.width		= display_info->IconData.width;
         data->IconData.height	= display_info->IconData.height;
         data->IconData.dst 	= display_info->IconData.dst;	
	  data->IconData.selfExplanatory 	= display_info->IconData.selfExplanatory;
	}
    else
 	{	
	  data->IconData.dst = NULL;
	  data->IconData.selfExplanatory = FALSE;
	  data->IconData.width =0;
	  data->IconData.height =0;
       }	  
#endif 

	  TRACE_EVENT_P1("anim_time = %d", display_info->anim_time);
	  TRACE_EVENT_P1("data->anim_time = %d", data->anim_time);
//x0pleela 27 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK	  
	phlock_mtc_anim_time = display_info->anim_time;
if( ( !phlock_win_handle) || (mmiPinsEmergencyCall()) 
	|| call_data.emergencyCall )
{
#endif //FF_PHONE_LOCK
if(display_info->anim_time != 0)
	  {
	  	data->animation_timer = timCreate(win,display_info->anim_time,(MfwCb)mmi_dialogs_animation_new_CB);
		timStart(data->animation_timer);
	  }

      if (display_info->Time NEQ FOREVER)
        tim_start (data->info_tim);
      win_show (win);

#ifdef FF_PHONE_LOCK	  
}
else
{
	 if (display_info->Time NEQ FOREVER)
	        tim_start (data->info_tim);



 	winShow(phlock_win_handle );
}
#endif //FF_PHONE_LOCK
      break;

    case DIALOG_DESTROY:
		dialog_info_destroy(win);
	break;
  }
}

#if(0)  /* x0039928 - Lint warning fix */
/*******************************************************************************

 $Function:    	dialog_info_win_cb

 $Description:	Callback function for information dialog
 
 $Returns:		void

 $Arguments:	window handle event, win
*******************************************************************************/
//GW-SPR#762-Added code to wraps txtString2 if there is room on the display to allow 
//very long numbers to be displayed.
#define MAX_LINE_CHAR MAX_LINE
static int dialog_info_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
    T_dialog_info * data = (T_dialog_info *)win->user;
	char buffer[MAX_LINE_CHAR+1];
	int yPos,len,bufPos,nLines;

    TRACE_FUNCTION ("dialog_info_win_cb()");


    if (data EQ 0)
	return 1;

    switch (event)
	{
	case E_WIN_VISIBLE:
	    if (win->flags & E_WIN_VISIBLE)
		{
		    /*
		     * Print the information dialog
		     */

		    /*
		     * Clear Screen
		     */
		    dspl_ClearAll();

		    /*
		     * Print softkeys
		     */
			icnShow(data->icon);
		    if (data->SoftKeyStrings)
		    	displayCustSoftKeys(data->LSKString, data->RSKString);	// SH - display text softkeys
		    else
				displaySoftKeys(data->LeftSoftKey, data->RightSoftKey);	// display ID softkeys
		    
		    /*
		     * Print information
		     */
		    /*MC SPR 1257, having to shift everything up 1 line as string wrapping for second
		    string causes softkey overwrite when in Chinese*/
		    if (data->TextId NEQ 0)
			{
			    ALIGNED_PROMPT(LEFT,0,DSPL_TXTATTR_CURRENT_MODE, data->TextId);
			}
		    else if (data->TextString NEQ NULL)
			{
#ifdef EASY_TEXT_ENABLED			
			    if (data->TextString[0] == (char)0x80) //MC if this is a unicode string /* x0039928 -lint warning removal */
					displayAlignedText(LEFT, 0, DSPL_TXTATTR_UNICODE, &data->TextString[2]);
			    else
#endif
			    	displayAlignedText(LEFT, 0, DSPL_TXTATTR_NORMAL, data->TextString);
			}

		    if (data->TextId2 NEQ 0)
			{
			    ALIGNED_PROMPT(LEFT,Mmi_layout_first_line()/*MC*/,DSPL_TXTATTR_CURRENT_MODE, data->TextId2);
			}
		    else if (data->TextString2 NEQ NULL)
			{	
#ifdef EASY_TEXT_ENABLED	
				if (data->TextString2[0] == (char)0x80 ) //MC if this is a unicode string /* x0039928 -Lint warning removal */
					displayAlignedText(LEFT, Mmi_layout_first_line()/*MC*/, DSPL_TXTATTR_UNICODE, &data->TextString2[2]);
			    else
#endif
			{
				if ((data->WrapStrings & WRAP_STRING_2) && (strlen(data->TextString2) > MAX_LINE_CHAR))
				{	//Display text over a number of lines
					yPos = Mmi_layout_first_line();/*MC*/
					bufPos = 0;
					nLines = Mmi_number_of_lines_without_icons_on_top()-2;//starting on 2nd line
					len = strlen(data->TextString2);
					while ((bufPos < len) && (nLines >=0))
					{
						memcpy(buffer,&data->TextString2[bufPos],MAX_LINE_CHAR);						
						bufPos = bufPos + MAX_LINE_CHAR;
						buffer[MAX_LINE_CHAR] = 0x00;
				    	displayAlignedText(LEFT, yPos, 0, buffer);
				    	yPos = yPos + Mmi_layout_line_height();
				    	nLines--;
					}
				}
				else
				{
			    	displayAlignedText(LEFT, Mmi_layout_second_line(),DSPL_TXTATTR_NORMAL, data->TextString2);
				}
			}
		}
	  }
	    break;

	default:
	    return 0;
	}
    return 1;
}
#endif

/*******************************************************************************

 $Function:    	dialog_info_win_cb

 $Description:	Callback function for information dialog
 
 $Returns:		void

 $Arguments:	window handle event, win

GW SPR#2294 02/07/03 -	Removed Unicode specific code as the 'display' routines called to calculate 
				the string length and output the text should correctly handle the 0x80 unicode tag 
				at the start.
				Modified the display code to wrap the text in the second text string.
				Modified the window drawing code so that if the text will not fit in the dialog box
				with the background bitmap, a pop-up 'box' will be drawn instead. If this will still 
				not be large enough, the entire screen is used.
				For the C-sample, there is no bitmap.
				Modified the code to use an array of values to position the text output.
GW SPR#2294 08/07/03 - Added code to expand the dialog box to include the animation (if any).
	
*******************************************************************************/
// Allow 4 lines of text (0..3) for dialogs
// Sep 06 2005  REF:  MMI-SPR-33548  x0012849
// To display a dialog properly when it contain lengthy message - No of lines increased.
#define MAX_DLG_LINES   (15)  
//Allow up to 40 chars of text on a dialog line
#define MAX_WRAP_CHAR  (42)
//Allow us to wrap text from line 2 to lines 2,3 and 4 lines
#define MAX_WRAP_LINES (MAX_DLG_LINES-1)

#define DLG_USE_BITMAP			0x0001		//use bitmap if present
#define DLG_USE_RECTANGLE		0x0002		//use a rectangle	
#define DLG_USE_FULL_SCREEN		0x0004		//use full-screen

#define DLG_LINE1 0
#define DLG_LINE2 1

static int dialog_info_win_resize_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
    T_dialog_info * data = (T_dialog_info *)win->user;
//     T_DISPLAY_DATA * display_info;   // RAVI
    int xPos[MAX_DLG_LINES];
    int yPos[MAX_DLG_LINES];
    // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
    //The  size of array  str and strlen are increased by 1, so that the last elements holds data 
    // which is used while wrapping the text.
    char* str[MAX_DLG_LINES+1];  
    int strLen[MAX_DLG_LINES+1]; 
	int i;
	char* lskStr,*rskStr;
	int wx = 0;  //,wy,sx=0,sy=0; // RAVI
	int wy = 0;   // RAVI
	int sx = 0;     // RAVI
	int sy = 0;     // RAVI
	int fullScreen = DLG_USE_RECTANGLE;
	int yOfs;
	int yBmpOfs = 0;
	MfwRect skArea;
	//x0035544 Feb 15, 2006. DR:OMAPS00061467
#ifdef FF_MMI_SAT_ICON
	USHORT		titleIconWidth = 0;	
#endif
    MfwRect area;
	int scrWidth = 0; //available width for the display // RAVI - Assignment to 0.
    // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
    // temp_ptr  is used  while wrapping the text.
    // Now str2[][] is not required.
    char *temp_ptr;     
	int nChar;

	/* t_font_bitmap bmp; */ /* Warning  Removal - x0020906 - 24-08-2006 */
//	t_font_bitmap* bmpPtr = &bmp;  // RAVI - Not Referenced.
	
	int drawWinType;
	int nLines;

	int linePos=0;
	int lineLen = 0;
	int wrapLine = 0; /*GW #2294*/ // x0066814 06 Dec 2006, OMAPS00106825

	const int charWidth = dspl_GetTextExtent("0",1);
    TRACE_FUNCTION ("dialog_info_win_resize_cb()");	 


	memset(str,'\0',sizeof(str));  /* x0039928 - Lint warning fix */
	memset(strLen,0,sizeof(strLen));
//x0035544 Feb 15, 2006. DR:OMAPS00061467
#ifdef FF_MMI_SAT_ICON
       if (data->IconData.dst != NULL)
	{
		if (data->IconData.width > TITLE_ICON_WIDTH) 
		{
			titleIconWidth = TITLE_ICON_WIDTH;
		}
		else
		{
			titleIconWidth = data->IconData.width ;
		}
	}			
	else
	{
		titleIconWidth = 0;
	}
	
#endif   	
    if (data EQ 0)
	return 1;

    switch (event)
	{
	//x0pleela 15 May, 2007 DR: OMAPS00127483
	//Added the following code to handle ringer for incoming call and alarm in phone unlocked state.
	//the ringer for incoming call or alarm is restarted here after unlocking the phone.
	case E_WIN_RESUME:

#ifdef FF_PHONE_LOCK		
		
		if( ( ( ( call_data.call_direction == MFW_CM_MTC ) && ( phlock_dialog_mtc_win_handle ) ) || ( phlock_alarm )  )
			#ifdef FF_MMI_AUDIO_PROFILE				
				&& ( !mfwAudPlay)
			#endif	
			)
		{
			if( call_data.call_direction == MFW_CM_MTC ) 
			{
			#ifdef FF_MIDI_RINGER  
				mfw_ringer_start(AS_RINGER_MODE_IC, TRUE, sounds_midi_ringer_start_cb);
			#endif
			}
			else if ( phlock_alarm )
			{
			#ifdef FF_MIDI_RINGER  
				mfw_ringer_start(AS_RINGER_MODE_ALARM, TRUE, sounds_midi_ringer_start_cb);
			#endif
			}
		}
			
#endif	// FF_PHONE_LOCK

		break;
		
	case E_WIN_VISIBLE:
	    if (win->flags & E_WIN_VISIBLE)
		{
		TRACE_EVENT("dialog_info_win_resize_cb: E_WIN_VISIBLE");
		
#ifdef FF_PHONE_LOCK		
			if( phlock_alarm && phlock_alarm_win_handle )
			{
				TRACE_EVENT("dialog_info_win_resize_cb: creating alarm kbd handler");
				data->info_kbd = kbd_create (phlock_alarm_win_handle, KEY_ALL, (T_MFW_CB)dialog_info_kbd_cb);
			}
			if( ( call_data.call_direction == MFW_CM_MTC ) && ( phlock_dialog_mtc_win_handle ) )
			{
				TRACE_EVENT("dialog_info_win_resize_cb: creating mtc kbd handler");
				data->info_kbd = kbd_create (phlock_dialog_mtc_win_handle, KEY_ALL, (T_MFW_CB)dialog_info_kbd_cb);
				if(phlock_mtc_anim_time != 0)
				  {
				  	data->animation_timer = timCreate(phlock_dialog_mtc_win_handle, phlock_mtc_anim_time,
										(MfwCb)mmi_dialogs_animation_new_CB);
					timStart(data->animation_timer);
				  }
			}
#endif //FF_PHONE_LOCK				

		    /*
		     * Print the information dialog
		     */
		    for (i=0;i<MAX_DLG_LINES;i++)
		    {
		    	xPos[i]=0;
			    yPos[i] = Mmi_layout_line_height()*i;
			    str[i]=NULL;
			    strLen[i]=0;
		    }
                // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
                // Assign the values to last elments of the array str and strLen
                str[MAX_DLG_LINES] = NULL; 
                strLen[MAX_DLG_LINES] = 0;
			yOfs = Mmi_layout_line_height()/4;
			if (data->TextId != 0)
			    str[DLG_LINE1] = MmiRsrcGetText(  data->TextId);
		    else 
				str[DLG_LINE1] = data->TextString;
			
			if (data->TextId2 != 0)
			    str[DLG_LINE2] = MmiRsrcGetText(  data->TextId2);
				else
				str[DLG_LINE2] = data->TextString2;			

		    if (data->SoftKeyStrings!=0)
		    {
		     	lskStr = data->LSKString;
		     	rskStr = data->RSKString;
		    }
		    else
		    {
		    	if (data->LeftSoftKey != TxtNull)
					lskStr = MmiRsrcGetText(data->LeftSoftKey);
				else
					lskStr = NULL; 
				
		    	if (data->RightSoftKey != TxtNull)
					rskStr = MmiRsrcGetText(data->RightSoftKey);
				else
					rskStr = NULL; 	
		    }

		    if ((str[DLG_LINE1]!= NULL) || (str[DLG_LINE2]!=NULL))
		    {	//Draw a window for status info
		    	if (str[DLG_LINE2] == NULL)
		     	{
		     		sy = Mmi_layout_line_height()+yOfs*2; // 18 or 12
		     		
		     	}
		     	else
		     	{
		     		sy = Mmi_layout_line_height()*2+yOfs*3; // 33 or 22
		     	}
		     	
		     	//MC, if (Mmi_getCurrentLanguage() != CHINESE_LANGUAGE)	 
		     	{	//calculate longest string - use this to calc width of window
			     	sx = 0;
			     	if ((lskStr!=NULL) || (rskStr!=NULL))
			     	{
			     		sy = sy+Mmi_layout_softkeyHeight()+yOfs*2; 
			     		sx = sx + 10;
				     	if (lskStr != NULL) 
				     	{
	    					sx = sx + dspl_GetTextExtent(lskStr, 0);
				     	}
				     	if (rskStr != NULL) 
				     	{
	    					sx = sx + dspl_GetTextExtent(rskStr,0);
				     	}
				     	
			     	}
					for (i=DLG_LINE1;i<=DLG_LINE2;i++)
		     		{	
			     		strLen[i] = dspl_GetTextExtent(str[i],0);
		     		}
					
			     	/* Work out the desired width of the dialog. If we can wrap line 2 and it is long, wrap it */
			     	if (data->WrapStrings & WRAP_STRING_2)
			     		nLines = MAX_WRAP_LINES;
		     		else 
			     		nLines = 1;
#ifdef COLOURDISPLAY
					if (data->dlgBgdBitmap!=NULL)
					{
				     	//we only have room to split line 2 into 'max_lines_dlg_popup' lines in the pop-up.
				     	//NB this should be calculated depending on the height of the popup bitmap.
						const int max_lines_dlg_popup=2;
				     	if (nLines>max_lines_dlg_popup)
							nLines = max_lines_dlg_popup; 
						scrWidth = (data->dlgBgdBitmap->area.sx * 3)/4; //the max width is about 3/4 of the pop-up
						if ( (strLen[DLG_LINE2] > (scrWidth-charWidth)*nLines) || 
							(strLen[DLG_LINE1] > scrWidth))
						{
							//recalculate width below.
					     	if (data->WrapStrings & WRAP_STRING_2)
					     		nLines = MAX_WRAP_LINES;
							fullScreen = DLG_USE_RECTANGLE;	
		     		}
		     		else
						{
							fullScreen = DLG_USE_BITMAP;
							//Leave width as is
						}
					}
#else
					//On a C-sample, limit pop-up lines to 2 extra (any more - use entire screen)
					{
						const int max_lines_dlg_popup=2;
				    	if (nLines>max_lines_dlg_popup)
							nLines = max_lines_dlg_popup; 
					}

#endif
					if (fullScreen == DLG_USE_RECTANGLE) 
					{	
						//see if we can display as a pop-up
						if (SCREEN_SIZE_X < 128)
							scrWidth = SCREEN_SIZE_X-6;
						else
				  			scrWidth = (SCREEN_SIZE_X*7)/8;   	
						if ( strLen[DLG_LINE2] > (scrWidth-charWidth)*nLines)
						{
							//Cannot fit in as a pop-up - use full screen
							fullScreen = DLG_USE_FULL_SCREEN;
							scrWidth = SCREEN_SIZE_X;
						}
		     		else
		     		{
							//We can fit it in - but we may want to reduce the width for a nicer look.
						}
					}
					
				    /*
				     * Wrap second text line (e.g. for long phone numbers.)
				     */
                                  if (strLen[DLG_LINE2] > scrWidth-charWidth)
                                  {
                                      //If we can fit the string on 2 lines - do it.
                                      if (data->WrapStrings & WRAP_STRING_2)
                                      {
                                          if (str[DLG_LINE2][0] != (char)0x80) //not unicode /* x0039928 -Lint warning removal */
                                          {
                                               wrapLine = 0;
                                               lineLen = strlen(str[DLG_LINE2]);
                                              // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
                                              // memset is not required any more .
                                              //Now the last element of the str[] will point to the second string of the dialog
                                             str[MAX_DLG_LINES] = str[DLG_LINE2];  // Now move the second  line of text to the last element of the arrya
                                             strLen[MAX_DLG_LINES] = strLen[DLG_LINE2];// put the right value for the length of the string
                                             str[DLG_LINE2] = NULL ;  // Assign NULL to the second element of the array.
                                             while ((wrapLine < MAX_WRAP_LINES) && (linePos<lineLen))								
                                             {
                                                 // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
                                                 // Instead of DLG_LINE2 use MAX_DLG_LINES 
                                                 nChar = dspl_GetNcharToFit (&str[MAX_DLG_LINES][linePos], (USHORT)scrWidth);  
                                                 if (nChar > MAX_WRAP_CHAR)
                                                     nChar = MAX_WRAP_CHAR;
                                                 // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
                                                 // Allocate MAX_WRAP_CHAR no of bytes 
                                                 temp_ptr = (char*)mfwAlloc(MAX_WRAP_CHAR); 
                                                 if(NULL == temp_ptr)
                                                     return 0;									  	 
                                                 // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
                                                 // Now the destination of memcpy is temp_ptr instead of str2[wrapLine] and the source is str[MAX_DLG_LINES] instead of str[DLG_LINE2]
                                                 memcpy(temp_ptr,&str[MAX_DLG_LINES][linePos],nChar);  // Now the source data is from the last element of the array.
                                                 linePos = linePos+nChar;
                                                 ////GW - even if we cannot display the entire string - it is better than what we would have otherwise 
                                                 // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
                                                // Assign the value of pointer temp_ptr to str[]
                                                str[DLG_LINE2+wrapLine] = temp_ptr;
                                                temp_ptr = NULL; 
                                                strLen[DLG_LINE2 + wrapLine] = dspl_GetTextExtent(str[DLG_LINE2+wrapLine],0);
                                                wrapLine++;
                                             }
								
								}
		     			else
							{
								//Unicode is more problematic - we need the 0x80 at the front of the string
								//for the call to 'dspl_GetNcharToFit'. This will involve copying the entire remainder 
								//of 'str2' into a temporary buffer. At the moment, we only wrap numbers so this
								//code will wait until it is required.
							//	str[3] = "Code not written!!";	//issue warning
							//	strLen[3] = dspl_GetTextExtent(str[3],0);
						     	fullScreen = DLG_USE_FULL_SCREEN; //error - use full screen			
							}
						}
					}
					
					//Calculate longest line of text (including softkeys - sx already set to the softkey width)
					for (i=0;i< MAX_DLG_LINES;i++)
			     	{
			     		if (strLen[i]>sx) 
			     			sx = strLen[i];
					}
		     		if (sx > SCREEN_SIZE_X-charWidth)
		     			sx = SCREEN_SIZE_X;
		     		else
		     		{
		     			if (sx > SCREEN_SIZE_X/2)
			     			sx = sx + charWidth;
		     			else
			     			sx = sx + charWidth*2;
		     		}
		     		if (sy> sx) 
		     			sx = sy;
		     		wx = (SCREEN_SIZE_X-sx)/2;
		     		if (wx<0)
		     			wx=0;
		     	}

		     	if (sy > SCREEN_SIZE_Y)
		     		wy = 0;
		     	else if (SCREEN_SIZE_Y > 2*sy)  
		     		wy = (SCREEN_SIZE_Y*2/3-sy)/2; //display in middle of top 2/3 of screen
		     	else 
		     		wy = (SCREEN_SIZE_Y-sy)/2; //display in middle of screen
				if (wy < 10)
					wy = wy/2;
		     		
		     	if ((wx == 0) || (wy==0))
				{	//not enough room to display as a window - use whole screen
					fullScreen = DLG_USE_FULL_SCREEN;
		     }

#ifdef COLOURDISPLAY
					if ((data->icon_array != NULL) && (fullScreen != DLG_USE_BITMAP))
					{	//The dialog has an animation - we must make the pop-up large enough to support this  
						if (sx < data->icon_array[0].area.sx)
							sx = data->icon_array[0].area.sx;
						yBmpOfs = data->icon_array[0].area.py + data->icon_array[0].area.sy;
						wy = data->icon_array[0].area.py;
						sy = sy + data->icon_array[0].area.sy;
					}
#endif
		    }
			else
			{	
				//We have no strings to display - clear entire screen
				fullScreen = DLG_USE_FULL_SCREEN;
			}


		    /******************************
		     * Draw window (full screen/pop-up)
		     *****************************/
			if (fullScreen != DLG_USE_FULL_SCREEN)
				dspl_unfocusDisplay();
			area.px = 0;
			area.py = 0;
			area.sx = sx;
			area.sy = sy;
		    resources_setColour(data->dlgCol);
			drawWinType = DSPL_WIN_CENTRE;
			
		     if (fullScreen!=DLG_USE_FULL_SCREEN)
		     {
				t_font_bitmap bmp;
				t_font_bitmap* bmpPtr = NULL;

				for (i=0;i<MAX_DLG_LINES;i++)
				{
		     		//centre text
			     	xPos[i] = wx+(sx-strLen[i])/2;
			     	yPos[i] = wy+yOfs+yBmpOfs+Mmi_layout_line_height()*i;
				}
				
#ifdef COLOURDISPLAY
				if ((data->dlgBgdBitmap != NULL) && (fullScreen==DLG_USE_BITMAP))
				{
			     	bmp.format = data->dlgBgdBitmap->icnType;
			     	bmp.height = data->dlgBgdBitmap->area.sy;
			     	bmp.width = data->dlgBgdBitmap->area.sx;
			     	bmp.bitmap = data->dlgBgdBitmap->icons;
			     	bmpPtr = &bmp;
					area = calcLayout(data->dlgType, sx, sy, bmp.width, bmp.height);

					//Adjust size of available area depending on bitmap properties/dlgType.
					//dlgType== 0 =>display info in centre of bitmap
					if (data->dlgType == 0)
			     	{
			     		//If the bmp is bigger than the data we will move the bmp to
			     		//lie over the centre of the data
			     		area.px = area.px - (bmp.width - sx)/2;
			     		area.py = area.py - (bmp.height- sy)/2;
			     	}
					for (i=0;i<MAX_DLG_LINES;i++)
					{
						xPos[i] = xPos[i]  + area.px;
			     		yPos[i] = yPos[i]  + area.py;
					}

					TRACE_EVENT_P3("wy = %d, yOfs = %d, area.py = %d",wy,yOfs,area.py);					
					if( data->dlgBgdBitmap->icnType != ICON_TYPE_1BIT_UNPACKED )				
						dspl_SetBgdColour( COL_TRANSPARENT );
						
					
				}
				else
				{
				       // x0066814 06 Dec 2006, OMAPS00106825
					//The window height calculation related to wrapping should be skipped when no 
					//string wrapping is done
					if(wrapLine>0)
				       {
						sy = sy + (wrapLine-1)*Mmi_layout_line_height();
					}
					area.sy = sy;
				}
#else
				if (wrapLine>1)
				{
					sy = sy + (wrapLine-1)*Mmi_layout_line_height();
					area.sy = sy;
				}				
#endif
                    // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
                    //If  length of the dialog is bigger than screen then start the dialog from the top corner.
                   if(sy+wy >= SCREEN_SIZE_Y && sy < SCREEN_SIZE_Y)
                       if(wy>0 && wy < sy)
                       {                     
                            wy = (SCREEN_SIZE_Y - sy ) /2 ;// centre  the dialog
                            if(wy < 0)
                                wy=0;		
                            for (i=0;i<MAX_DLG_LINES;i++)		     		
                                yPos[i] = wy+yOfs+yBmpOfs+Mmi_layout_line_height()*i;
                       }           
		     	  dspl_DrawWin( wx,wy,sx,sy, drawWinType , bmpPtr);
		     }
		     else
		     {
			    dspl_ClearAll();		     
		    	sx = 84;
				for (i=0;i<MAX_DLG_LINES;i++)
		     	{
		     		xPos[i] = 0;
			     	yPos[i] = Mmi_layout_line(i+1);
		     }
		     }
		    /******************************
		     * Print softkeys
		     *****************************/
			icnShow(data->icon);
			if ((lskStr != NULL) || (rskStr != NULL))
			{
				if (fullScreen==DLG_USE_FULL_SCREEN)
					Mmi_layout_softkeyArea( &skArea );
				else
				{
					skArea.px = area.px+wx;
				    skArea.sx = area.sx;
				    skArea.py = area.py+wy+area.sy-Mmi_layout_softkeyHeight()-1;
				    skArea.sy = Mmi_layout_softkeyHeight();					
				}
				softKeys_displayStrXY(lskStr, rskStr, data->dlgSkFormat, data->dlgCol, &skArea);							
		     }

		    /******************************
		     * Print dialog text
		     *****************************/
			for (i=0;i<MAX_DLG_LINES;i++)
			{	 /* GW#2294 No need to draw zero length strings .*/					
			    if ((str[i] != NULL) && (strLen[i] > 0))
				    {				   
			   //x0035544 Feb 15, 2006. DR:OMAPS00061467    
			   #ifdef FF_MMI_SAT_ICON	
			              if(data->IconData.selfExplanatory == FALSE)			   	
			   		dspl_TextOut(xPos[i] + titleIconWidth, yPos[i], DSPL_TXTATTR_NORMAL, str[i] );
			   #else
			   		dspl_TextOut(xPos[i] , yPos[i], DSPL_TXTATTR_NORMAL, str[i] );
			   #endif		          				   	
			    	     }
			}
		  //x0035544 Feb 15, 2006. DR:OMAPS00061467	
		 #ifdef FF_MMI_SAT_ICON
		 if(data->IconData.dst != NULL)
		 {		 
		  if(str[DLG_LINE2] == NULL)
		 	{
		 	 if ((data->IconData.width > TITLE_ICON_WIDTH) || 
			 	(data->IconData.height > TITLE_ICON_HEIGHT))
		          {
			  	dspl_BitBlt2(xPos[0], yPos[0], 8, 
					    10, (void*)SATIconQuestionMark, 0, BMP_FORMAT_256_COLOUR);
		 	   }	

		      	 else		    
		       	dspl_BitBlt2(xPos[0], yPos[0], data->IconData.width, 
			      		 data->IconData.height, (void*)data->IconData.dst, 0, BMP_FORMAT_256_COLOUR);
		 	}
		 else
		 	{
		 	if ((data->IconData.width > TITLE_ICON_WIDTH) || 
			 	(data->IconData.height > TITLE_ICON_HEIGHT))
		          {
		       dspl_BitBlt2(xPos[DLG_LINE2], yPos[DLG_LINE2], 8, 
					    10, (void*)SATIconQuestionMark, 0, BMP_FORMAT_256_COLOUR);
		 	    }
			else
				dspl_BitBlt2(xPos[DLG_LINE2], yPos[DLG_LINE2],data->IconData.width, 
			      		 data->IconData.height, (void*)data->IconData.dst, 0, BMP_FORMAT_256_COLOUR);
		 	}
				
		 }		               
		 #endif

			
		     resources_restoreColour();	
		}
     // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
     // Now free the allocated memory	 
     /* x0039928 - Lint warning removal */
    if (strLen[MAX_DLG_LINES] > scrWidth-charWidth)					
        if (data->WrapStrings & WRAP_STRING_2)					
            if (str[MAX_DLG_LINES][0] != (char)0x80)  /* x0039928 - lint warning removal */
            {
                 for(i=DLG_LINE2;i<MAX_DLG_LINES;i++)
                 {
                      if(NULL != str[i] && strLen[i] > 0)			 	
   	                   mfwFree((U8*)str[i],MAX_WRAP_CHAR);
      	           }
	           str[MAX_DLG_LINES]=NULL;		  
	      }
			
	    break;
	default:
	    return 0;
	} 

    return 1;
}

/*******************************************************************************

 $Function:    	dialog_info_tim_cb

 $Description:	Callback function for the dialog info timer.

 
 $Returns:		MFW event handler

 $Arguments:	window handle event, timer control block

*******************************************************************************/
static T_MFW_CB dialog_info_tim_cb (T_MFW_EVENT event, T_MFW_TIM *tc)
{
  T_MFW_HND       win  = mfw_parent (mfw_header());
  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
  T_dialog_info * data = (T_dialog_info *)win_data->user;

  T_MFW_HND       parent_win = data->parent_win;
  USHORT          Identifier = data->Identifier;
  void (* Callback)() = data->Callback;

	TRACE_FUNCTION("dialog_info_tim_cb()");
#if defined (WIN32)
      {
		  /***************************Go-lite Optimization changes Start***********************/

        //Aug 16, 2004    REF: CRR 24323   Deepa M.D
        TRACE_EVENT_P1 ("Identifier %d", data->Identifier);
		/***************************Go-lite Optimization changes end***********************/

      }
#endif

  /*
  ** SPR#1744 NDH
  ** Move the dialog destory in front of the Callback to make the processing consistent
  ** with the Right Keypress and Hangup Processing. Also to ensure that the dialog is 
  ** removed when the screen is redrawn.
  */
  dialog_info_destroy (win);

//TISH modified for MSIM
//  if (Callback)
  if (Callback && parent_win)
     (Callback) (parent_win, Identifier, INFO_TIMEOUT);

  return 0;
}


/*******************************************************************************

 $Function:    	dialog_info_kbd_cb

 $Description:	Keyboard event handler

 $Returns:		status int

 $Arguments:	window handle event, keyboard control block

*******************************************************************************/

static int dialog_info_kbd_cb (T_MFW_EVENT event, T_MFW_KBD *keyboard)
{
  T_MFW_HND       win  = mfw_parent (mfw_header());
  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
  T_dialog_info * data = (T_dialog_info *)win_data->user;
  T_MFW_HND       parent_win = data->parent_win;
  USHORT          Identifier = data->Identifier;
  void (* Callback)() = data->Callback;

  TRACE_FUNCTION("dialog_info_kbd_cb");
  switch (keyboard->code)
  {
    case KCD_LEFT:
		if (data->KeyEvents & KEY_LEFT )		
		{
		TRACE_EVENT("dialog_info_kbd_cb: KEY_LEFT");

			dialog_info_destroy (win);
			if (Callback)
			(Callback) (parent_win, Identifier, INFO_KCD_LEFT);
		}
       break;

    case KCD_RIGHT:
		if (data->KeyEvents & KEY_RIGHT)		
		{
			dialog_info_destroy (win);
	       if (Callback)
		     (Callback) (parent_win, Identifier, INFO_KCD_RIGHT);
		}
       break;

    case KCD_HUP:
		if (data->KeyEvents & KEY_HUP  || data->KeyEvents & INFO_KCD_ALL)
		{
	       dialog_info_destroy (win);
		   if (Callback)
			 (Callback) (parent_win, Identifier, INFO_KCD_HUP);
		}
       break;
    case KCD_MNUUP:
    	
		if (data->KeyEvents & KEY_MNUUP)
		{
		   if (Callback)
			 (Callback) (parent_win, Identifier, INFO_KCD_UP);
		}
		
       break;
    case KCD_MNUDOWN:
    	
		if (data->KeyEvents & KEY_MNUDOWN)
		{
		  if (Callback)
			 (Callback) (parent_win, Identifier, INFO_KCD_DOWN);
		}
		
       break;
       //MZ 7/3/01 used to start a call.
    case KCD_CALL:
    	if (data->KeyEvents & KEY_CALL ) 		
   		{
	       dialog_info_destroy (win);
		   if (Callback)
			 (Callback) (parent_win, Identifier, INFO_KCD_OFFHOOK);
    		}
		   break;
    default:
		if (data->KeyEvents & INFO_KCD_ALL)
		{
				TRACE_EVENT("dialog_info_kbd_cb: INFO_KCD_ALL");

		/*
		Note: The variable Identifier is overwritten with the
		keycode of the dialog;
		we need it only for the Multicall Control !!
		*/
		   Identifier = keyboard->code;
	       dialog_info_destroy (win);
	       if (Callback)
	         (Callback) (parent_win, Identifier, INFO_KCD_ALL);
	    }
       break;

  }  
  return MFW_EVENT_CONSUMED;
}

static int dialog_info_kbd_long_cb(T_MFW_EVENT event, T_MFW_KBD *keyboard){
	return MFW_EVENT_CONSUMED;
}



/*******************************************************************************

 $Function:    	mmi_dialog_information_screen

 $Description:	

 $Returns:		

 $Arguments:	

*******************************************************************************/

T_MFW_HND mmi_dialog_information_screen(T_MFW_HND parent_win, int TxtId, char* text, T_VOID_FUNC call_back, USHORT identifier)
{
	T_DISPLAY_DATA display_info;
	dlg_initDisplayData_TextId( &display_info, TxtNull,  TxtNull, TxtId, TxtNull, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, call_back, THREE_SECS, KEY_ALL );
	//Sep 01, 2004    REF: CRR 21380  Deepa M.D 
	//Assigning TextId and TextString will not display both the messages.
	//Hence we need to use TextId and TextString2 or TextId2 and TextString
	display_info.TextString2   = text; 
	display_info.Identifier   = identifier;
	return info_dialog(parent_win,&display_info); //information screen
		
}

T_MFW_HND mmi_dialog_information_screen_forever(MfwHnd win,int TextId, char* TextStr, int colour)
{
	T_DISPLAY_DATA DisplayInfo;
	dlg_initDisplayData_TextId( &DisplayInfo, TxtNull, TxtNull, TextId,  TxtNull, colour);
	dlg_initDisplayData_events( &DisplayInfo, NULL, FOREVER, KEY_HUP );
	DisplayInfo.TextString 	= TextStr;
   return info_dialog(win, &DisplayInfo);
}

/*******************************************************************************

 $Function:    	mmi_dialogs_insert_icon

 $Description:	Displays one icon in a dialog 

 $Returns:		none

 $Arguments:	win, pointer to an icon

*******************************************************************************/
void mmi_dialogs_insert_icon (T_MFW_HND win_dialog ,MfwIcnAttr *icon)
{
  T_MFW_WIN      * win_data     = ((T_MFW_HDR *)win_dialog)->data;
  T_dialog_info  * data         = (T_dialog_info *)win_data->user;


		data->icon = icnCreate(win_dialog,icon,E_ICN_VISIBLE,NULL);			
		icnUnhide(data->icon);
		winShow(win_dialog);
		
}

/*******************************************************************************

 $Function:    	mmi_dialogs_insert_animation

 $Description:	starts an animation in a dialog

 $Returns:		none

 $Arguments:	win, animation time, pointer to an icon, number of frames

*******************************************************************************/
void mmi_dialogs_insert_animation (T_MFW_HND win_dialog , S32 anim_time ,MfwIcnAttr *icon ,UBYTE number_of_frames)
{
  T_MFW_WIN      * win_data     = ((T_MFW_HDR *)win_dialog)->data;
  T_dialog_info  * data         = (T_dialog_info *)win_data->user;


		data->number_of_frames = number_of_frames;
		data->current_frame = 0;
		data->icon_array = icon;  
		data->current_icon = data->icon_array;
		data->icon = icnCreate(win_dialog,data->current_icon,E_ICN_VISIBLE,NULL);			
		icnUnhide(data->icon);

    	data->animation_timer = timCreate(win_dialog,anim_time,(MfwCb)mmi_dialogs_insert_animation_CB);
		timStart(data->animation_timer);
		winShow(win_dialog);
		
}

/*******************************************************************************

 $Function:    	mmi_dialogs_insert_animation_CB

 $Description:	destroys the current icon and starts the next icon

 $Returns:		none

 $Arguments:	timer event, timer control block

*******************************************************************************/
void mmi_dialogs_insert_animation_CB (T_MFW_EVENT event, T_MFW_TIM *tc)
{
  T_MFW_HND       win  = mfw_parent (mfw_header());
  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
  T_dialog_info * data = (T_dialog_info *)win_data->user;

  
		icnDelete(data->icon);
		if (data->current_frame < (data->number_of_frames-1))
		{
				data->current_frame++;
				data->current_icon++;				
		}
		else				    	
		{
				data->current_frame = 0;
				data->current_icon = data->icon_array;
		}
		data->icon = icnCreate(win,data->current_icon,E_ICN_VISIBLE,NULL);			
		icnUnhide(data->icon);
		timStart(data->animation_timer);
		winShow(win);
						
}
/***************************Go-lite Optimization changes Start***********************/

//Aug 25, 2004    REF: CRR 24904   Deepa M.D
//This function is used only for the Color build.Hence it is put under the 
//COLOURDISPLAY compiler switch
#ifdef COLOURDISPLAY 
void mmi_dialogs_insert_animation_new (T_DISPLAY_DATA *animateInfo , S32 anim_time ,MfwIcnAttr* icon ,UBYTE number_of_frames)
{
	TRACE_EVENT("mmi_dialogs_insert_animation_new()");

		animateInfo->number_of_frames = number_of_frames;
		animateInfo->current_frame = 0;
		animateInfo->icon_array = icon;
		animateInfo->anim_time = anim_time;
		
}
#endif
/***************************Go-lite Optimization changes end***********************/

void mmi_dialogs_animation_new_CB (T_MFW_EVENT event, T_MFW_TIM *tc)
{
  T_MFW_HND	win = mfw_parent (mfw_header());
  T_MFW_WIN     * 	win_data = ((T_MFW_HDR *)win)->data;
  T_dialog_info *   data = (T_dialog_info *)win_data->user;
  char * idData;
  int px, py, sx, sy;
  MfwIcnAttr* icn_array;
  MfwIcnAttr* icn;

  TRACE_EVENT("mmi_dialogs_insert_animation_new_CB()");

    

  data->current_frame = (data->current_frame +1) % data->number_of_frames; 
  TRACE_EVENT_P1("data->current_frame = %d", data->current_frame);  
  TRACE_EVENT_P1("number_of_frames = %d", data->number_of_frames);
  
  icn_array = (MfwIcnAttr*)data->icon_array;
  TRACE_EVENT_P1("data->icon_array = %x", (int)data->icon_array);
  
  icn = &icn_array[data->current_frame];
 
  px = icn->area.px;
  py = icn->area.py;
  sx = icn->area.sx;
  sy = icn->area.sy;
  
  TRACE_EVENT_P4("px = %d, py = %d, sx = %d, sy = %d", icn->area.px,icn->area.py,icn->area.sx,icn->area.sy);
	
  idData = icn->icons;
	
  dspl_BitBlt2(px,py,sx,sy,(void*)idData,0,0x02);
  timStart(data->animation_timer);

}
//Jun 04,2004 CRR:13601 xpradipg - SASKEN
/*******************************************************************************

 $Function:    	InfoDialogTimRestart

 $Description:	restarts the information dialog timer

 $Returns:		none

 $Arguments:	window handler

*******************************************************************************/

void InfoDialogTimRestart(T_MFW_HND win)
{
	T_MFW_WIN * win_data;
        T_dialog_info * data;
	
        win_data = ((T_MFW_HDR *)win)->data;
        data     = (T_dialog_info *)win_data->user;

	tim_stop(data->info_tim);
	tim_start(data->info_tim);
}

// Feb 24, 2005 CRR:29102 xnkulkar - SASKEN
// As the user has already confirmed that all SMS messages are to be deleted, KEY_HUP is not
// registered.
/*******************************************************************************

 $Function:    	mmi_dialog_information_screen_delete_all

 $Description:	Displays the information screen during "Delete All" operation

 $Returns:	Handle of information dialog

 $Arguments:	window handle, text id, text string and generic status window

*******************************************************************************/
T_MFW_HND mmi_dialog_information_screen_delete_all(MfwHnd win,int TextId, char* TextStr, int colour)
{
	T_DISPLAY_DATA DisplayInfo;
	
	dlg_initDisplayData_TextId( &DisplayInfo, TxtNull, TxtNull, TextId,  TxtNull, colour);
	dlg_initDisplayData_events( &DisplayInfo, NULL, FOREVER, NULL ); 
	DisplayInfo.TextString 	= TextStr;
	
   	return info_dialog(win, &DisplayInfo);
}

//x0peela 02 Apr, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
/*******************************************************************************

 $Function:    	phlock_info_dialog

 $Description:	Common information dialog
 
 $Returns:		Dialogue info win

 $Arguments:	parent win, display info
 
*******************************************************************************/
T_MFW_HND phlock_info_dialog (T_MFW_HND        parent_win,
                       T_DISPLAY_DATA * display_info)
{
  T_MFW_HND win;				
  TRACE_FUNCTION("phlock_info_dialog()");
  display_info->SoftKeyStrings = FALSE;
  
  win = phlock_dialog_info_create (parent_win);  			

  if (win NEQ NULL)
  {
    SEND_EVENT (win, DIALOG_INIT, 0, display_info);
  }
  return win;
}

/*******************************************************************************

 $Function:    	phlock_dialog_info_create

 $Description:	Creation of an information dialog
 
 $Returns:		Dialogue info win

 $Arguments:	parent win
 
*******************************************************************************/
static T_MFW_HND phlock_dialog_info_create (T_MFW_HND parent_win)
{
  T_MFW_WIN     * win_data;
  T_dialog_info *  data = (T_dialog_info *)ALLOC_MEMORY (sizeof (T_dialog_info));
  TRACE_FUNCTION ("phlock_dialog_info_create()");

  data->info_win = win_create (parent_win, 0, E_WIN_VISIBLE, (T_MFW_CB)phlock_dialog_info_win_resize_cb);

  if (data->info_win EQ 0)
    return 0;
 TRACE_EVENT_P1("phlock_dialog_info_create:data->info_win:%02x", data->info_win);
  /*
   * Create window handler
   */
  data->mmi_control.dialog   = (T_DIALOG_FUNC)phlock_dialog_info;
  data->mmi_control.data     = data;
  data->parent_win           = parent_win;
  win_data                   = ((T_MFW_HDR *)data->info_win)->data;
  win_data->user             = (void *)data;

  /*
   * return window handle
   */
  return data->info_win;
}

/*******************************************************************************

 $Function:    	phlock_dialog_info_win_resize_cb

 $Description:	Creation of an information dialog
 
 $Returns:		Dialogue info win

 $Arguments:	parent win
 
*******************************************************************************/
static int phlock_dialog_info_win_resize_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
    T_dialog_info * data = (T_dialog_info *)win->user;
//     T_DISPLAY_DATA * display_info;   // RAVI
    int xPos[MAX_DLG_LINES];
    int yPos[MAX_DLG_LINES];
    // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
    //The  size of array  str and strlen are increased by 1, so that the last elements holds data 
    // which is used while wrapping the text.
    char* str[MAX_DLG_LINES+1];  
    int strLen[MAX_DLG_LINES+1]; 
	int i;
	char* lskStr,*rskStr;
	int wx = 0;  //,wy,sx=0,sy=0; // RAVI
	int wy = 0;   // RAVI
	int sx = 0;     // RAVI
	int sy = 0;     // RAVI
	int fullScreen = DLG_USE_RECTANGLE;
	int yOfs;
	int yBmpOfs = 0;
	MfwRect skArea;
	//x0035544 Feb 15, 2006. DR:OMAPS00061467
#ifdef FF_MMI_SAT_ICON
	USHORT		titleIconWidth = 0;	
#endif
    MfwRect area;
	int scrWidth = 0; //available width for the display // RAVI - Assignment to 0.
    // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
    // temp_ptr  is used  while wrapping the text.
    // Now str2[][] is not required.
    char *temp_ptr;     
	int nChar;

	/* t_font_bitmap bmp; */ /* Warning  Removal - x0020906 - 24-08-2006 */
//	t_font_bitmap* bmpPtr = &bmp;  // RAVI - Not Referenced.
	
	int drawWinType;
	int nLines;

	int linePos=0;
	int lineLen = 0;
	int wrapLine = 0; /*GW #2294*/ // x0066814 06 Dec 2006, OMAPS00106825

	const int charWidth = dspl_GetTextExtent("0",1);
    TRACE_FUNCTION ("phlock_dialog_info_win_resize_cb()");	 

	memset(str,'\0',sizeof(str));  /* x0039928 - Lint warning fix */
	memset(strLen,0,sizeof(strLen));
//x0035544 Feb 15, 2006. DR:OMAPS00061467
#ifdef FF_MMI_SAT_ICON
       if (data->IconData.dst != NULL)
	{
		if (data->IconData.width > TITLE_ICON_WIDTH) 
		{
			titleIconWidth = TITLE_ICON_WIDTH;
		}
		else
		{
			titleIconWidth = data->IconData.width ;
		}
	}			
	else
	{
		titleIconWidth = 0;
	}
	
#endif   	
    if (data EQ 0)
	return 1;

    switch (event)
	{
	case E_WIN_VISIBLE:
	    if (win->flags & E_WIN_VISIBLE)
		{
		    /*
		     * Print the information dialog
		     */
		    for (i=0;i<MAX_DLG_LINES;i++)
		    {
		    	xPos[i]=0;
			    yPos[i] = Mmi_layout_line_height()*i;
			    str[i]=NULL;
			    strLen[i]=0;
		    }
                // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
                // Assign the values to last elments of the array str and strLen
                str[MAX_DLG_LINES] = NULL; 
                strLen[MAX_DLG_LINES] = 0;
			yOfs = Mmi_layout_line_height()/4;
			if (data->TextId != 0)
			    str[DLG_LINE1] = MmiRsrcGetText(  data->TextId);
		    else 
				str[DLG_LINE1] = data->TextString;
			
			if (data->TextId2 != 0)
			    str[DLG_LINE2] = MmiRsrcGetText(  data->TextId2);
				else
				str[DLG_LINE2] = data->TextString2;			

		    if (data->SoftKeyStrings!=0)
		    {
		     	lskStr = data->LSKString;
		     	rskStr = data->RSKString;
		    }
		    else
		    {
		    	if (data->LeftSoftKey != TxtNull)
					lskStr = MmiRsrcGetText(data->LeftSoftKey);
				else
					lskStr = NULL; 
				
		    	if (data->RightSoftKey != TxtNull)
					rskStr = MmiRsrcGetText(data->RightSoftKey);
				else
					rskStr = NULL; 	
		    }

		    if ((str[DLG_LINE1]!= NULL) || (str[DLG_LINE2]!=NULL))
		    {	//Draw a window for status info
		    	if (str[DLG_LINE2] == NULL)
		     	{
		     		sy = Mmi_layout_line_height()+yOfs*2; // 18 or 12
		     		
		     	}
		     	else
		     	{
		     		sy = Mmi_layout_line_height()*2+yOfs*3; // 33 or 22
		     	}
		     	
		     	//MC, if (Mmi_getCurrentLanguage() != CHINESE_LANGUAGE)	 
		     	{	//calculate longest string - use this to calc width of window
			     	sx = 0;
			     	if ((lskStr!=NULL) || (rskStr!=NULL))
			     	{
			     		sy = sy+Mmi_layout_softkeyHeight()+yOfs*2; 
			     		sx = sx + 10;
				     	if (lskStr != NULL) 
				     	{
	    					sx = sx + dspl_GetTextExtent(lskStr, 0);
				     	}
				     	if (rskStr != NULL) 
				     	{
	    					sx = sx + dspl_GetTextExtent(rskStr,0);
				     	}
				     	
			     	}
					for (i=DLG_LINE1;i<=DLG_LINE2;i++)
		     		{	
			     		strLen[i] = dspl_GetTextExtent(str[i],0);
		     		}
					
			     	/* Work out the desired width of the dialog. If we can wrap line 2 and it is long, wrap it */
			     	if (data->WrapStrings & WRAP_STRING_2)
			     		nLines = MAX_WRAP_LINES;
		     		else 
			     		nLines = 1;
#ifdef COLOURDISPLAY
					if (data->dlgBgdBitmap!=NULL)
					{
				     	//we only have room to split line 2 into 'max_lines_dlg_popup' lines in the pop-up.
				     	//NB this should be calculated depending on the height of the popup bitmap.
						const int max_lines_dlg_popup=2;
				     	if (nLines>max_lines_dlg_popup)
							nLines = max_lines_dlg_popup; 
						scrWidth = (data->dlgBgdBitmap->area.sx * 3)/4; //the max width is about 3/4 of the pop-up
						if ( (strLen[DLG_LINE2] > (scrWidth-charWidth)*nLines) || 
							(strLen[DLG_LINE1] > scrWidth))
						{
							//recalculate width below.
					     	if (data->WrapStrings & WRAP_STRING_2)
					     		nLines = MAX_WRAP_LINES;
							fullScreen = DLG_USE_RECTANGLE;	
		     		}
		     		else
						{
							fullScreen = DLG_USE_BITMAP;
							//Leave width as is
						}
					}
#else
					//On a C-sample, limit pop-up lines to 2 extra (any more - use entire screen)
					{
						const int max_lines_dlg_popup=2;
				    	if (nLines>max_lines_dlg_popup)
							nLines = max_lines_dlg_popup; 
					}

#endif
					if (fullScreen == DLG_USE_RECTANGLE) 
					{	
						//see if we can display as a pop-up
						if (SCREEN_SIZE_X < 128)
							scrWidth = SCREEN_SIZE_X-6;
						else
				  			scrWidth = (SCREEN_SIZE_X*7)/8;   	
						if ( strLen[DLG_LINE2] > (scrWidth-charWidth)*nLines)
						{
							//Cannot fit in as a pop-up - use full screen
							fullScreen = DLG_USE_FULL_SCREEN;
							scrWidth = SCREEN_SIZE_X;
						}
		     		else
		     		{
							//We can fit it in - but we may want to reduce the width for a nicer look.
						}
					}
					
				    /*
				     * Wrap second text line (e.g. for long phone numbers.)
				     */
                                  if (strLen[DLG_LINE2] > scrWidth-charWidth)
                                  {
                                      //If we can fit the string on 2 lines - do it.
                                      if (data->WrapStrings & WRAP_STRING_2)
                                      {
                                          if (str[DLG_LINE2][0] != (char)0x80) //not unicode /* x0039928 -Lint warning removal */
                                          {
                                               wrapLine = 0;
                                               lineLen = strlen(str[DLG_LINE2]);
                                              // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
                                              // memset is not required any more .
                                              //Now the last element of the str[] will point to the second string of the dialog
                                             str[MAX_DLG_LINES] = str[DLG_LINE2];  // Now move the second  line of text to the last element of the arrya
                                             strLen[MAX_DLG_LINES] = strLen[DLG_LINE2];// put the right value for the length of the string
                                             str[DLG_LINE2] = NULL ;  // Assign NULL to the second element of the array.
                                             while ((wrapLine < MAX_WRAP_LINES) && (linePos<lineLen))								
                                             {
                                                 // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
                                                 // Instead of DLG_LINE2 use MAX_DLG_LINES 
                                                 nChar = dspl_GetNcharToFit (&str[MAX_DLG_LINES][linePos], (USHORT)scrWidth);  
                                                 if (nChar > MAX_WRAP_CHAR)
                                                     nChar = MAX_WRAP_CHAR;
                                                 // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
                                                 // Allocate MAX_WRAP_CHAR no of bytes 
                                                 temp_ptr = (char*)mfwAlloc(MAX_WRAP_CHAR); 
                                                 if(NULL == temp_ptr)
                                                     return 0;									  	 
                                                 // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
                                                 // Now the destination of memcpy is temp_ptr instead of str2[wrapLine] and the source is str[MAX_DLG_LINES] instead of str[DLG_LINE2]
                                                 memcpy(temp_ptr,&str[MAX_DLG_LINES][linePos],nChar);  // Now the source data is from the last element of the array.
                                                 linePos = linePos+nChar;
                                                 ////GW - even if we cannot display the entire string - it is better than what we would have otherwise 
                                                 // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
                                                // Assign the value of pointer temp_ptr to str[]
                                                str[DLG_LINE2+wrapLine] = temp_ptr;
                                                temp_ptr = NULL; 
                                                strLen[DLG_LINE2 + wrapLine] = dspl_GetTextExtent(str[DLG_LINE2+wrapLine],0);
                                                wrapLine++;
                                             }
								
								}
		     			else
							{
								//Unicode is more problematic - we need the 0x80 at the front of the string
								//for the call to 'dspl_GetNcharToFit'. This will involve copying the entire remainder 
								//of 'str2' into a temporary buffer. At the moment, we only wrap numbers so this
								//code will wait until it is required.
							//	str[3] = "Code not written!!";	//issue warning
							//	strLen[3] = dspl_GetTextExtent(str[3],0);
						     	fullScreen = DLG_USE_FULL_SCREEN; //error - use full screen			
							}
						}
					}
					
					//Calculate longest line of text (including softkeys - sx already set to the softkey width)
					for (i=0;i< MAX_DLG_LINES;i++)
			     	{
			     		if (strLen[i]>sx) 
			     			sx = strLen[i];
					}
		     		if (sx > SCREEN_SIZE_X-charWidth)
		     			sx = SCREEN_SIZE_X;
		     		else
		     		{
		     			if (sx > SCREEN_SIZE_X/2)
			     			sx = sx + charWidth;
		     			else
			     			sx = sx + charWidth*2;
		     		}
		     		if (sy> sx) 
		     			sx = sy;
		     		wx = (SCREEN_SIZE_X-sx)/2;
		     		if (wx<0)
		     			wx=0;
		     	}

		     	if (sy > SCREEN_SIZE_Y)
		     		wy = 0;
		     	else if (SCREEN_SIZE_Y > 2*sy)  
		     		wy = (SCREEN_SIZE_Y*2/3-sy)/2; //display in middle of top 2/3 of screen
		     	else 
		     		wy = (SCREEN_SIZE_Y-sy)/2; //display in middle of screen
				if (wy < 10)
					wy = wy/2;
		     		
		     	if ((wx == 0) || (wy==0))
				{	//not enough room to display as a window - use whole screen
					fullScreen = DLG_USE_FULL_SCREEN;
		     }

#ifdef COLOURDISPLAY
					if ((data->icon_array != NULL) && (fullScreen != DLG_USE_BITMAP))
					{	//The dialog has an animation - we must make the pop-up large enough to support this  
						if (sx < data->icon_array[0].area.sx)
							sx = data->icon_array[0].area.sx;
						yBmpOfs = data->icon_array[0].area.py + data->icon_array[0].area.sy;
						wy = data->icon_array[0].area.py;
						sy = sy + data->icon_array[0].area.sy;
					}
#endif
		    }
			else
			{	
				//We have no strings to display - clear entire screen
				fullScreen = DLG_USE_FULL_SCREEN;
			}


		    /******************************
		     * Draw window (full screen/pop-up)
		     *****************************/
			if (fullScreen != DLG_USE_FULL_SCREEN)
				dspl_unfocusDisplay();
			area.px = 0;
			area.py = 0;
			area.sx = sx;
			area.sy = sy;
		    resources_setColour(data->dlgCol);
			drawWinType = DSPL_WIN_CENTRE;
			
		     if (fullScreen!=DLG_USE_FULL_SCREEN)
		     {
				t_font_bitmap bmp;
				t_font_bitmap* bmpPtr = NULL;

				for (i=0;i<MAX_DLG_LINES;i++)
				{
		     		//centre text
			     	xPos[i] = wx+(sx-strLen[i])/2;
			     	yPos[i] = wy+yOfs+yBmpOfs+Mmi_layout_line_height()*i;
				}
				
#ifdef COLOURDISPLAY
				if ((data->dlgBgdBitmap != NULL) && (fullScreen==DLG_USE_BITMAP))
				{
			     	bmp.format = data->dlgBgdBitmap->icnType;
			     	bmp.height = data->dlgBgdBitmap->area.sy;
			     	bmp.width = data->dlgBgdBitmap->area.sx;
			     	bmp.bitmap = data->dlgBgdBitmap->icons;
			     	bmpPtr = &bmp;
					area = calcLayout(data->dlgType, sx, sy, bmp.width, bmp.height);

					//Adjust size of available area depending on bitmap properties/dlgType.
					//dlgType== 0 =>display info in centre of bitmap
					if (data->dlgType == 0)
			     	{
			     		//If the bmp is bigger than the data we will move the bmp to
			     		//lie over the centre of the data
			     		area.px = area.px - (bmp.width - sx)/2;
			     		area.py = area.py - (bmp.height- sy)/2;
			     	}
					for (i=0;i<MAX_DLG_LINES;i++)
					{
						xPos[i] = xPos[i]  + area.px;
			     		yPos[i] = yPos[i]  + area.py;
					}

					TRACE_EVENT_P3("wy = %d, yOfs = %d, area.py = %d",wy,yOfs,area.py);					
					if( data->dlgBgdBitmap->icnType != ICON_TYPE_1BIT_UNPACKED )				
						dspl_SetBgdColour( COL_TRANSPARENT );
						
					
				}
				else
				{
				       // x0066814 06 Dec 2006, OMAPS00106825
					//The window height calculation related to wrapping should be skipped when no 
					//string wrapping is done
					if(wrapLine>0)
				       {
						sy = sy + (wrapLine-1)*Mmi_layout_line_height();
					}
					area.sy = sy;
				}
#else
				if (wrapLine>1)
				{
					sy = sy + (wrapLine-1)*Mmi_layout_line_height();
					area.sy = sy;
				}				
#endif
                    // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
                    //If  length of the dialog is bigger than screen then start the dialog from the top corner.
                   if(sy+wy >= SCREEN_SIZE_Y && sy < SCREEN_SIZE_Y)
                       if(wy>0 && wy < sy)
                       {                     
                            wy = (SCREEN_SIZE_Y - sy ) /2 ;// centre  the dialog
                            if(wy < 0)
                                wy=0;		
                            for (i=0;i<MAX_DLG_LINES;i++)		     		
                                yPos[i] = wy+yOfs+yBmpOfs+Mmi_layout_line_height()*i;
                       }           
		     	  dspl_DrawWin( wx,wy,sx,sy, drawWinType , bmpPtr);
		     }
		     else
		     {
			    dspl_ClearAll();		     
		    	sx = 84;
				for (i=0;i<MAX_DLG_LINES;i++)
		     	{
		     		xPos[i] = 0;
			     	yPos[i] = Mmi_layout_line(i+1);
		     }
		     }
		    /******************************
		     * Print softkeys
		     *****************************/
			icnShow(data->icon);
			if ((lskStr != NULL) || (rskStr != NULL))
			{
				if (fullScreen==DLG_USE_FULL_SCREEN)
					Mmi_layout_softkeyArea( &skArea );
				else
				{
					skArea.px = area.px+wx;
				    skArea.sx = area.sx;
				    skArea.py = area.py+wy+area.sy-Mmi_layout_softkeyHeight()-1;
				    skArea.sy = Mmi_layout_softkeyHeight();					
				}
				softKeys_displayStrXY(lskStr, rskStr, data->dlgSkFormat, data->dlgCol, &skArea);							
		     }

		    /******************************
		     * Print dialog text
		     *****************************/
			for (i=0;i<MAX_DLG_LINES;i++)
			{	 /* GW#2294 No need to draw zero length strings .*/					
			    if ((str[i] != NULL) && (strLen[i] > 0))
				    {				   
			   //x0035544 Feb 15, 2006. DR:OMAPS00061467    
			   #ifdef FF_MMI_SAT_ICON	
			              if(data->IconData.selfExplanatory == FALSE)			   	
			   		dspl_TextOut(xPos[i] + titleIconWidth, yPos[i], DSPL_TXTATTR_NORMAL, str[i] );
			   #else
			   		dspl_TextOut(xPos[i] , yPos[i], DSPL_TXTATTR_NORMAL, str[i] );
			   #endif		          				   	
			    	     }
			}
		  //x0035544 Feb 15, 2006. DR:OMAPS00061467	
		 #ifdef FF_MMI_SAT_ICON
		 if(data->IconData.dst != NULL)
		 {		 
		  if(str[DLG_LINE2] == NULL)
		 	{
		 	 if ((data->IconData.width > TITLE_ICON_WIDTH) || 
			 	(data->IconData.height > TITLE_ICON_HEIGHT))
		          {
			  	dspl_BitBlt2(xPos[0], yPos[0], 8, 
					    10, (void*)SATIconQuestionMark, 0, BMP_FORMAT_256_COLOUR);
		 	   }	

		      	 else		    
		       	dspl_BitBlt2(xPos[0], yPos[0], data->IconData.width, 
			      		 data->IconData.height, (void*)data->IconData.dst, 0, BMP_FORMAT_256_COLOUR);
		 	}
		 else
		 	{
		 	if ((data->IconData.width > TITLE_ICON_WIDTH) || 
			 	(data->IconData.height > TITLE_ICON_HEIGHT))
		          {
		       dspl_BitBlt2(xPos[DLG_LINE2], yPos[DLG_LINE2], 8, 
					    10, (void*)SATIconQuestionMark, 0, BMP_FORMAT_256_COLOUR);
		 	    }
			else
				dspl_BitBlt2(xPos[DLG_LINE2], yPos[DLG_LINE2],data->IconData.width, 
			      		 data->IconData.height, (void*)data->IconData.dst, 0, BMP_FORMAT_256_COLOUR);
		 	}
				
		 }		               
		 #endif

			
		     resources_restoreColour();	
		}

     // Sep 06 2005  REF:  MMI-SPR-33548  x0012849
     // Now free the allocated memory	 
     /* x0039928 - Lint warning removal */
    if (strLen[MAX_DLG_LINES] > scrWidth-charWidth)					
        if (data->WrapStrings & WRAP_STRING_2)					
            if (str[MAX_DLG_LINES][0] != (char)0x80)  /* x0039928 - lint warning removal */
            {
                 for(i=DLG_LINE2;i<MAX_DLG_LINES;i++)
                 {
                      if(NULL != str[i] && strLen[i] > 0)			 	
   	                   mfwFree((U8*)str[i],MAX_WRAP_CHAR);
      	           }
	           str[MAX_DLG_LINES]=NULL;		  
	      }
			
	    break;

	default:
	    return 0;
	} 

    return 1;
}

/*******************************************************************************

 $Function:    	phlock_dialog_info

 $Description:	Dialog function for information dialog
 
 $Returns:		void

 $Arguments:	win, window handle event, value, parameter

*******************************************************************************/
void phlock_dialog_info (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
  T_MFW_WIN      * win_data     = ((T_MFW_HDR *)win)->data;
  T_dialog_info  * data         = (T_dialog_info *)win_data->user;
  T_DISPLAY_DATA * display_info = (T_DISPLAY_DATA *)parameter;  
  TRACE_FUNCTION ("phlock_dialog_info()");

  switch (event)
  {
    case DIALOG_INIT:
      /*
       * Initialize dialog
       */
#if defined (WIN32)
      {
		  /***************************Go-lite Optimization changes Start***********************/
		  
		  //Aug 16, 2004    REF: CRR 24323   Deepa M.D
		  TRACE_EVENT_P1( "Identifier %d", display_info->Identifier);
		  /***************************Go-lite Optimization changes end***********************/

      }
#endif
      /*
       * Create timer and keyboard handler
       */
    data->info_tim             = tim_create (win, display_info->Time, (T_MFW_CB)phlock_dialog_info_tim_cb);
		data->info_kbd             = kbd_create (win, KEY_ALL, (T_MFW_CB)phlock_dialog_info_kbd_cb);
		data->info_kbd_long	= kbd_create(win,KEY_ALL|KEY_LONG,(MfwCb)phlock_dialog_info_kbd_long_cb);

      /*
       * Store any other data
       */
      data->KeyEvents			 = display_info->KeyEvents;
	  data->TextString           = display_info->TextString;
      data->LeftSoftKey          = display_info->LeftSoftKey;
      data->RightSoftKey         = display_info->RightSoftKey;
      data->SoftKeyStrings		= display_info->SoftKeyStrings;	// SH - TRUE if using strings rather than IDs
      data->LSKString			= display_info->LSKString;		// Text for left soft key
      data->RSKString			= display_info->RSKString;		// Text for right soft key
      data->TextId               = display_info->TextId;
      data->Identifier           = display_info->Identifier;
      data->Callback             = display_info->Callback;
      data->TextString2          = display_info->TextString2;
      data->TextId2              = display_info->TextId2;
	  data->WrapStrings			= display_info->WrapStrings;	//GW-SPR#762

	  data->dlgCol				= display_info->displayCol;
	  data->dlgType				= display_info->dlgType;
	  data->dlgBgdBitmap		= display_info->bgdBitmap;
	  data->dlgSkFormat			= 0;
	  data->current_frame		= display_info->current_frame;
	  data->number_of_frames	= display_info->number_of_frames;
	  data->icon_array			= (MfwIcnAttr*)display_info->icon_array;
	  data->animation_timer		= 0;
//x0035544 Feb 07, 2006 DR:OMAPS00061467
#ifdef FF_MMI_SAT_ICON
     if(display_info->IconData.dst != NULL)
	{
         data->IconData.width		= display_info->IconData.width;
         data->IconData.height	= display_info->IconData.height;
         data->IconData.dst 	= display_info->IconData.dst;	
	  data->IconData.selfExplanatory 	= display_info->IconData.selfExplanatory;
	}
    else
 	{	
	  data->IconData.dst = NULL;
	  data->IconData.selfExplanatory = FALSE;
	  data->IconData.width =0;
	  data->IconData.height =0;
       }	  
#endif 

	  TRACE_EVENT_P1("anim_time = %d", display_info->anim_time);
	  TRACE_EVENT_P1("data->anim_time = %d", data->anim_time);
if(display_info->anim_time != 0)
	  {
	  	data->animation_timer = timCreate(win,display_info->anim_time,(MfwCb)mmi_dialogs_animation_new_CB);
		timStart(data->animation_timer);
	  }

      if (display_info->Time NEQ FOREVER)
        tim_start (data->info_tim);
      win_show (win);

      break;

    case DIALOG_DESTROY:
		phlock_dialog_info_destroy(win);
	break;
  }
}

/*******************************************************************************

 $Function:    	phlock_dialog_info_tim_cb

 $Description:	Callback function for the dialog info timer.

 
 $Returns:		MFW event handler

 $Arguments:	window handle event, timer control block

*******************************************************************************/
static T_MFW_CB phlock_dialog_info_tim_cb (T_MFW_EVENT event, T_MFW_TIM *tc)
{
  T_MFW_HND       win  = mfw_parent (mfw_header());
  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
  T_dialog_info * data = (T_dialog_info *)win_data->user;

  T_MFW_HND       parent_win = data->parent_win;
  USHORT          Identifier = data->Identifier;
  void (* Callback)() = data->Callback;

	TRACE_FUNCTION("phlock_dialog_info_tim_cb()");
#if defined (WIN32)
      {
		  /***************************Go-lite Optimization changes Start***********************/

        //Aug 16, 2004    REF: CRR 24323   Deepa M.D
        TRACE_EVENT_P1 ("Identifier %d", data->Identifier););
		/***************************Go-lite Optimization changes end***********************/

      }
#endif

#ifdef FF_PHONE_LOCK
	TRACE_EVENT("Bef. destroy");
	TRACE_EVENT_P1("phlock_dialog_info_tim_cb: win: %02x", win);

#endif //FF_PHONE_LOCK
  /*
  ** SPR#1744 NDH
  ** Move the dialog destory in front of the Callback to make the processing consistent
  ** with the Right Keypress and Hangup Processing. Also to ensure that the dialog is 
  ** removed when the screen is redrawn.
  */
  
  TRACE_EVENT("phlock_dialog_info_tim_cb: Bef dialog_info_destrroy");
  phlock_dialog_info_destroy (win);

  #ifdef FF_PHONE_LOCK
	TRACE_EVENT("Aft. destroy");
	TRACE_EVENT_P1("phlock_dialog_info_tim_cb: win: %02x", win);
  #endif //FF_PHONE_LOCK
  
  if (Callback)
     (Callback) (parent_win, Identifier, INFO_TIMEOUT);

  return 0;
}

/*******************************************************************************

 $Function:    	phlock_dialog_info_kbd_cb

 $Description:	Keyboard event handler

 $Returns:		status int

 $Arguments:	window handle event, keyboard control block

*******************************************************************************/

static int phlock_dialog_info_kbd_cb (T_MFW_EVENT event, T_MFW_KBD *keyboard)
{
  T_MFW_HND       win  = mfw_parent (mfw_header());
  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
  T_dialog_info * data = (T_dialog_info *)win_data->user;
  T_MFW_HND       parent_win = data->parent_win;
  USHORT          Identifier = data->Identifier;
  void (* Callback)() = data->Callback;

  TRACE_FUNCTION("phlock_dialog_info_kbd_cb");

  switch (keyboard->code)
  {
    case KCD_LEFT:
		if (data->KeyEvents & KEY_LEFT )		
		{
			phlock_dialog_info_destroy (win);
			if (Callback)
			(Callback) (parent_win, Identifier, INFO_KCD_LEFT);
		}
       break;

    case KCD_RIGHT:
		if (data->KeyEvents & KEY_RIGHT)		
		{
			phlock_dialog_info_destroy (win);
	       if (Callback)
		     (Callback) (parent_win, Identifier, INFO_KCD_RIGHT);
		}
       break;

    case KCD_HUP:
		if (data->KeyEvents & KEY_HUP  || data->KeyEvents & INFO_KCD_ALL)
		{
	       phlock_dialog_info_destroy (win);
		   if (Callback)
			 (Callback) (parent_win, Identifier, INFO_KCD_HUP);
		}
       break;
    case KCD_MNUUP:
    	
		if (data->KeyEvents & KEY_MNUUP)
		{
		   if (Callback)
			 (Callback) (parent_win, Identifier, INFO_KCD_UP);
		}
		
       break;
    case KCD_MNUDOWN:
    	
		if (data->KeyEvents & KEY_MNUDOWN)
		{
		  if (Callback)
			 (Callback) (parent_win, Identifier, INFO_KCD_DOWN);
		}
		
       break;
       //MZ 7/3/01 used to start a call.
    case KCD_CALL:
    	if (data->KeyEvents & KEY_CALL ) 		
   		{
	       phlock_dialog_info_destroy (win);
		   if (Callback)
			 (Callback) (parent_win, Identifier, INFO_KCD_OFFHOOK);
    		}
		   break;
    default:
		if (data->KeyEvents & INFO_KCD_ALL)
		{
		/*
		Note: The variable Identifier is overwritten with the
		keycode of the dialog;
		we need it only for the Multicall Control !!
		*/
		   Identifier = keyboard->code;
	       phlock_dialog_info_destroy (win);
	       if (Callback)
	         (Callback) (parent_win, Identifier, INFO_KCD_ALL);
	    }
       break;

  }
  return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	phlock_dialog_info_kbd_long_cb

 $Description:	Keyboard event handler

 $Returns:		status int

 $Arguments:	window handle event, keyboard control block

*******************************************************************************/
static int phlock_dialog_info_kbd_long_cb(T_MFW_EVENT event, T_MFW_KBD *keyboard)
{
TRACE_FUNCTION("dialog_info_kbd_long_cb()");
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	phlock_dialog_info_destroy

 $Description:	Destruction of an information dialog
 
 $Returns:		void

 $Arguments:	win
 
*******************************************************************************/
void phlock_dialog_info_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN     * win  = ((T_MFW_HDR *)own_window)->data;
  T_dialog_info * data = (T_dialog_info *)win->user;

  TRACE_FUNCTION ("phlock_dialog_info_destroy()");

  if (own_window == NULL)
  {
	TRACE_EVENT ("Error :- Called with NULL Pointer");
	return;
  }
	
  if (data)
  {
    /*
     * Exit TIMER & KEYBOARD Handle
     */
    kbd_delete (data->info_kbd);
    tim_delete (data->info_tim);

    if(data->animation_timer != 0)
    {
    	timDelete(data->animation_timer);

    	if (data->icon != 0)
    		icnDelete(data->icon);
    }

    /*
     * Delete WIN Handler
     */
          TRACE_EVENT_P1("phlock_dialog_info_destroy: deleting:data->info_win:%02x",data->info_win);

    win_delete (data->info_win);
    /*
     * Free Memory
     */
    FREE_MEMORY ((void *)data, sizeof (T_dialog_info));
  }
}
#endif  //FF_PHONE_LOCK

