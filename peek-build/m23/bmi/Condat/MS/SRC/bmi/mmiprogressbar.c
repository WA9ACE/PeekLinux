/* =========================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================== */

/*==========================================================
* @file mmiprogressbar
*
* This provides the functionality of File Manager Applications.
* It supports mp3/midi file plaback progress
*
* @path  \bmi\condat\ms\src\mmi
*
* @rev  00.01
*/
/* ========================================================== */
/*===========================================================
*!
*! Revision History
*! ===================================

	July 03, 2007 DRT: OMAPS00137912 x0066814	
	Description: AAC player Forward and Rewind feature development
	Solution: In mmi_progress_dialog_kbd_cb(), under case KCD_MNURIGHT and case KCD_MNULEFT
	added a condition to check if the selected file is a AAC file.

	Apr 10, 2007 DRT: OMAPS00125151 x0039928
	Description: MMI displays Progress Bar for only one Minute not for the entire duration of media (MIDI, MP3) play.
	Solution: playback time from AS is now received in seconds instead of milliseconds.
	
	Apr 05, 2007 DRT: OMAPS00124890 x0039928
	Description: MMI back ground screen is some time blue and some time white.
	Solution: Back ground is set to White color.
	
	Apr 05, 2007   DRT: OMAPS00124881  x0039928
	Description: AAC progress bar does not work
	Solution: Progress bar is displayed if the callback is called from audio service irrespective
	of any conditions.
	
	Mar 28, 2007   ER: OMAPS00106188  x0039928
	Description: Align Midi application menu choices and audio Player menu choices 
	
* =========================================================== */

/******************************************************************************
                                Include Files
*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


//#include "\bmi\Condat\MS\SRC\aci\aci_cmh.h"
//#include "\bmi\Condat\MS\SRC\mfw\mfw_phb.h"

//#include "\bmi\Condat\MS\SRC\bmi\mmiDummy.h"

//#include "\bmi\condat\ms\src\mms\mg4def.h"
//#include "\bmi\condat\ms\src\mms\Color.h"
#include "mfw_sys.h"
#include "ATBPb.h"
#include "Mfw_mfw.h"

//#include "\bmi\Condat\MS\SRC\mfw\Mfw_win.h"
//#include "MmiDummy.h"
#include "mfw_tim.h"
#include "mfw_icn.h"
#include "Mfw_mnu.h"
#include "Mfw_sat.h"
#include "mfw_kbd.h"
#include "mmiColours.h"
#include "MmiDummy.h"
#include "mfw_fm.h"
//#include "aci_cmh.h"

//#include "\bmi\Condat\MS\SRC\atb\ATBPb.h"
//#include "\bmi\gdi\dspl.h"
#include "MmiSoftKeys.h"
#include "MmiDialogs.h"
#include "MmiLists.h"
#include "mmiFileManager.h" 
#include "Mmiprogressbar.h"

#define FWD_REW_TIME 10

#define PAUSE 0
#define RESUME 1
#define PB_FORWARD 2
#define PB_REWIND 4

UBYTE aud_state = 1;
BOOL pb_block_keys = 0;
extern T_MMI_FM_STRUCT *fm_data;
T_MFW_HND pb_win;
MfwHnd  tim;
int progress;
extern BOOL IsMP3Playing;//Daisy tang added for Real Resume feature 20071107
extern BOOL IsRealPaused;//Daisy tang added for Real Resume feature 20071107

static int mmi_progress_dialog_kbd_cb (MfwEvt e, MfwKbd *k);

void pbar_update(int playback_time, int total_time)
{
	int pp;
	
	    if(playback_time <= total_time)
	    {
	      /* Apr 10, 2007 DRT: OMAPS00125151 x0039928 */
	    	pp = (playback_time * 100)/total_time;
		MmiProgressIndication( x_screen, y_screen, bar_width, bar_height, playback_time, pp);
		pb_block_keys = 0;
		aud_state = RESUME;
	    }
}

/******************************************************************************************/
// Call-Back function

static int ProgressBar_cb(MfwEvt e, MfwWin *w)
{

	switch(e)	
		{
			case E_WIN_VISIBLE:
				//playback_percentage
				progress = 0;
				MmiProgressIndication( x_screen, y_screen, bar_width, bar_height,  download_percentage, 0);
				displaySoftKeys(TxtPlayerPause,TxtSoftBack);
		//		winShow(w);
				break;

				
			default:
				return 0;
					
		}
	
	

	return MFW_EVENT_CONSUMED;

}



// Entry Function

T_MFW_HND progress_bar(T_MFW_HND	 parent )

{
	T_MFW_WIN  * win;


	tProgressData  * data    = (tProgressData *)ALLOC_MEMORY (sizeof (tProgressData));

//	T_MFW_HND	 parent   = mfwParent( mfw_header());

	


	if (data EQ NULL)
	{
		return NULL;
	}

	
	data->win                     = win_create (parent, 0, E_WIN_VISIBLE, (T_MFW_CB)ProgressBar_cb);
	
	if (data->win EQ NULL)
	{
		
		return NULL;
	}

	pb_win = data->win;
	data->mmi_control.data   	= (void *)data;
	win                                  	= ((T_MFW_HDR *)data->win)->data;
	win->user                      	= (void *)data;
	data->parent_win     		= parent;

//	MmiProgressIndication( x_screen, y_screen, bar_width, bar_height,  download_percentage, playback_percentage);
	data->kbd = kbd_create (data->win,KEY_ALL|KEY_MAKE,(T_MFW_CB)mmi_progress_dialog_kbd_cb);
	winShow(data->win);
	return data->win;


}

// Progress Bar Implementation

void MmiProgressIndication(UBYTE x, UBYTE y, UBYTE b_width, UBYTE b_height, int pt, int pp)
{


	int txtId1, x1, y1;
	char * str1;

	char str3[10];
	// colour to fill in the Bar for Download & PlayBack Progress
//	 U32 download_Col = COL_Magenta; 
	 U32 playback_Col =  COL_Blue; 


	 // Width of Progress bar showing Download & Playback progress
//	 Float download_progress_width;
	 Float playback_progress_width; 

/* Apr 05, 2007 DRT: OMAPS00124890 x0039928 */
	dspl_SetBgdColour (COL_White);	 
	dspl_ClearAll();

	// Draw the Progress Bar whose Width = width & Height = height
	dspl_DrawFilledColRect( x,y,x+b_width, y+b_height, COL_MidGrey);

#if 0
  //     dspl_Enable(1);

	/***      DOWNLOAD PROGRESS	***/

	if(download_percentage <=0 )
		{	

			// Do nothing

			return;

		}

	else if(download_percentage >=100)
		{
			// Fill the Entire Bar
			dspl_DrawFilledColRect( x,y, x+b_width, y+b_height , download_Col);			
			//return;
	//		 dspl_Enable(1);
		}

	else
		{

	//		download_progress_width = (b_width/100)*download_percentage;

			// Fill the Bar to Show the Download progress
	//		dspl_DrawFilledColRect( x,y, x+( int )download_progress_width, y+b_height , download_Col);

	//	 dspl_Enable(1);

			/***      PLAYBACK PROGRESS  	***/

	dspl_DrawFilledColRect( x,y, x+b_width, y+b_height , download_Col);	
	
			if(playback_percentage <= 0 )
				{	
					// Do nothing 					

					return;

				}
			
			else if(playback_percentage >=100)
				{
					// Fill the Entire Bar
					dspl_DrawFilledColRect( x,y, x+b_width, y+b_height , playback_Col);			
	//				 dspl_Enable(1);
					return;

				}

			else
				{
#endif			
				
					/*		// Clear the Area to show the Playback progress
					
							dspl_Clear(x,y, x+playback_percentage,y+height);		
					*/

					
					playback_progress_width = (b_width/100.00)*pp;	
					switch(aud_state)
					{
					case PAUSE:
						txtId1 = TxtPlayerPause;
						break;
					case RESUME:
						txtId1 = TxtPlayerPlaying;
						break;
					case PB_FORWARD:
						txtId1 = TxtPlayerForwarding;
						break;
					case PB_REWIND:
						txtId1 = TxtPlayerRewinding;
						break;
					default:
						txtId1=TxtNull;						
						break;
					}
					str1 = MmiRsrcGetText(txtId1);


					// Fill the Bar to Show the Playback progress			
					dspl_DrawFilledColRect(x,y, x+(int)playback_progress_width ,y+b_height , playback_Col);
				//	displaySoftKeys(TxtSoftOptions,TxtSoftBack);
					if(aud_state == PAUSE)
						displaySoftKeys(TxtPlayerResume,TxtSoftBack);
					else
						displaySoftKeys(TxtPlayerPause,TxtSoftBack);
		
					x1= (SCREEN_SIZE_X/2) - (dspl_GetTextExtent(str1,0) / 2) ;
					y1= (SCREEN_SIZE_Y -Mmi_layout_softkeyHeight())/2 - Mmi_layout_line_height();

					sprintf(str3,"%d:%d", pt/60, pt%60);
					dspl_TextOut(x,y - 20,DSPL_TXTATTR_NORMAL,str3);
					dspl_TextOut(x1,y1,DSPL_TXTATTR_NORMAL,str1);
					dspl_Enable(1);
				
#if 0
				}
	
	
		}
#endif

}
 


/*******************************************************************************
 $Function:   		mmi_progress_dialog_kbd_cb

 $Description:	 	Handler for key events for the copy progress window

 $Returns:		Execution status

 $Arguments:		e	-window event Id
				k	-key event
*******************************************************************************/
static int mmi_progress_dialog_kbd_cb (MfwEvt e, MfwKbd *k)
{

	T_MFW_HND	 win   = mfwParent( mfw_header());

	T_FM_CURDIR *cur_dir = fm_data->cur_dir;

	TRACE_FUNCTION("mmi_progress_dialog_kbd_cb");
	if (!(e & KEY_MAKE))
	{
		return MFW_EVENT_CONSUMED;
	}

	switch (k->code)
	{
		case KCD_LEFT:
		if(aud_state == PAUSE)
		{	
			IsMP3Playing = TRUE;//Daisy tang added for Real Resume feature 20071107
			mfw_fm_audResume();
			aud_state = RESUME;
			displaySoftKeys(TxtPlayerPause,TxtSoftBack);
		}
		else
		{
			IsMP3Playing = FALSE;//Daisy tang added for Real Resume feature 20071107
			mfw_fm_audPause();
			aud_state = PAUSE;
			displaySoftKeys(TxtPlayerResume,TxtSoftBack);
		}
			break;
		case KCD_RIGHT:
		case KCD_HUP:
			IsMP3Playing = FALSE;//Daisy tang added for Real Resume feature 20071107
			mfw_fm_audStop();
			break;

		case KCD_MNURIGHT:
		if((cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_MP3)||
			(cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_MIDI)||
/*July 03, 2007 DRT: OMAPS00137912 x0066814*/
			(cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_AAC))
			if(pb_block_keys == 0)
			{
				pb_block_keys = 1;
				aud_state = PB_FORWARD;
				displayMenuKeys(MENU_KEY_WAIT);
				as_forward(FWD_REW_TIME,NULL);
			}
			break;

		case KCD_MNULEFT:
		if((cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_MP3)||
			(cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_MIDI)||
/*July 03, 2007 DRT: OMAPS00137912 x0066814*/
			(cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_AAC))
			if(pb_block_keys == 0)
			{
				pb_block_keys = 1;	
				aud_state = PB_REWIND;
				displayMenuKeys(MENU_KEY_WAIT);
				as_rewind(FWD_REW_TIME, NULL);
			}
			break;
	}
	return MFW_EVENT_CONSUMED;
}












