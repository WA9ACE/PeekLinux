/*******************************************************************************

          Texas Instruments

********************************************************************************

 This software product is the property of Texas Instruments and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: MMI Application to test the different Audio file formats.
 $Module:   Test  Audio Player
 $File:       MmiMp3Test.c
 $Revision:   1.0

 $Author:   Elsa Armanet-Said Texas Instruments Nice
 $Date:       17/05/04

********************************************************************************

 Description:

    This provides the MP3 Test Application interface.

********************************************************************************
 $History: MmiMp3Test.c

 	Sep 11, 2006 DR: OMAPS00094182 xrashmic
 	Description: midi and mp3 files is not being populated on accessing corresponding menu, but only during bootup
 	Solution: Populate files on accessing the "browse files" list

  	Jun 06, 2006 REF: ERT OMAPS00070659  x0039928  
       Bug:MP3 RWD and FWD function - MOT
       Fix: Forward and Rewind functionalities are handled.
       
	Jun 02,2006 REF: DR OMAPS00079746  x0039928  
       Bug:MMI displays 'Playing' when trying to play a wrong file
       Fix: MP3 playback error is handled.

  	Nov 16,2005 REF: DR OMAPS00049192  x0039928  
       Bug:MP3: The screen is not refreshed after the mp3 file completes playing
       Fix: The window is destroyed in mmi_mp3_test_start_cb() after AUDIO_OK 
       event is received in the mfw callback.

  17/05/04    
        Sep 29,2005 REF: SPR 34407 xdeepadh  
	Bug:Back soft key takes more time to return to the previous screen in MP3 application
	Fix:The list creation for  MP3 files has been modified


  	Sep 27,2005 REF: SPR 34402 xdeepadh  
	Bug:Mono option is always highlighted even when Stereo is selected
	Fix:The channel setting will be saved in the FFS, which will be retrieved 
	later and the selected channel will be higlighted.

  	Jul 18, 2005    REF: SPR 31695   xdeepadh
	Bug:Problems with MP3 test application
	Fix:The window handling of MP3 Test Application has been done properly.

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

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#include "message.h" /* included for aci_cmh.h .... */
#include "aci_cmh.h" /* included for mfw_sat.h, included for MmiDummy.h, included for MmiMenu.h */

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_icn.h"	/* included for mfw_mnu.h */
#include "mfw_mnu.h"
#include "mfw_tim.h"
#include "mfw_kbd.h"
#include "mfw_sat.h" /* included for MmiDummy.h, included for MmiMenu.h */
#include "mfw_mp3test.h"

#include "MmiBlkLangDB.h"
#include "mmiColours.h"
#include "MmiDialogs.h"

#include "MmiDummy.h" /* included for MmiMenu.h */
#include "MmiLists.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "MmiMP3Test.h"
#include "MmiMain.h"
#include "Mmiicons.h"
#include "IcnDefs.h"
 //Sep 27,2005 REF: SPR 34402 xdeepadh  
#include "MmiBookShared.h" 
#include "mfw_ffs.h"

/*******************************************************************************

                                internal data

*******************************************************************************/

/*
 * Internal events
 */
//	E_INIT event definition
#define E_INIT 1

#if (BOARD == 71)
#define MP3_INIT 1
#define MP3_RETURN 2
#endif

//Sep 27,2005 REF: SPR 34402 xdeepadh  
//defines for channels
#define PLAYER_CHANNEL_MONO		0
#define PLAYER_CHANNEL_STEREO	1

//Jul 18, 2005    REF: SPR 31695   xdeepadh
//Defines for Player events
#define PLAYER_INIT           500
#define PLAYER_DESTROY       501
#define PLAYER_PLAY           502

/*local defines*/
#define PLAYER_INFO_SCRN_TIMEOUT  1500

#if (BOARD == 71)
#define MP3_FORWARD_REWIND_TIME 10
#endif

typedef struct
{
	T_MMI_CONTROL	mmi_control;
	MfwHnd			win;
	MfwHnd       		info_win; //Jul 18, 2005    REF: SPR 31695   xdeepadh
	MfwHnd  			parent_win;//Jul 18, 2005    REF: SPR 31695   xdeepadh
	MfwHnd			mfwwin;	
	MfwHnd			callwin;		
	MfwHnd                 kbd;
#if (BOARD == 71)	
	MfwHnd			menu;
#endif
}	T_MMI_Player_Win_data;

/*menu data structure definition*/
typedef struct
{
  T_MMI_CONTROL                      mmi_control;
  T_MFW_HND                             win;
	T_MFW_HND			parent;		//Jul 18, 2005    REF: SPR 31695   xdeepadh
  T_MFW_HND                             kbd;
  T_MFW_HND                             kbd_long;
  T_MFW_HND                             player_menu;
   T_VOID_FUNC                          func_cb;
   UBYTE						player_num_file;
   UBYTE						player_file_count;
   ListCbFunc					player_list_cb;
   UBYTE						player_list_idx;
 ListMenuData   * 			menu_list_data; //Sep 29,2005 REF: SPR 34407 xdeepadh  
}T_player_menu;

//	Nov 16,2005 REF: DR OMAPS00049192  x0039928  
// Global variable that holds the mmi return path and is found in mfw_mp3test.c
extern MMI_RETURN_PATH mmi_audio_player_return_path;

/*global menu data*/
T_player_menu player_menu_data = {0};
/*Boolean to check whether file being played is paused.*/
BOOL IsPaused = FALSE;
//UBYTE IsMfwInit = 0;
extern char* inputFileName; // input file
extern  UBYTE IsMP3On;
extern int audio_player_currently_playing_idx ;

#if (BOARD == 71)
static T_MFW_HND optionwin;
#endif

/*******************************************************************************

                                Local prototypes

*******************************************************************************/
static void mmi_audio_player_menu_list_listmnu_cb(T_MFW_HND Parent, ListMenuData * ListData);
static MfwHnd mmi_audio_player_show_info(T_MFW_HND parent, int str1, int str2, T_VOID_FUNC callback);
//Sep 29,2005 REF: SPR 34407 xdeepadh  
static T_MFW_HND  mmi_audio_player_build_player_Menu( MfwHnd parent_window);


/*default static menu*/
static MfwMnuAttr menuPlayerSearchListAttr =
{
	&menuArea,
	MNU_LEFT | MNU_LIST | MNU_CUR_LINE,		/* centered page menu       */
	(U8)-1,										/* use default font         */
	NULL,									/* with these items (filled in later)  */
	0,										/* number of items (filled in leter)  */
	COLOUR_LIST_XX,							/* Colour */
	TxtNull,								/* Hdr Id */
	NULL,									/* Background */
	MNUATTRSPARE							/* unused */
};

#if (BOARD == 71)
static const MfwMnuItem Mmi_mp3_OPTItems [] =
{
	{0,0,0,(char *)TxtPlayerPause,0,(MenuFunc)M_exePause,item_flag_none},
	{0,0,0,(char *)TxtPlayerForward,0,(MenuFunc)M_exeForward,item_flag_none},	
	{0,0,0,(char *)TxtPlayerRewind,0,(MenuFunc)M_exeRewind,item_flag_none}	
};


static const MfwMnuAttr Mmi_mp3_OPTAttrib =
{
    &SmsSend_R_OPTArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu       */
    (U8)-1,                                 /* use default font         */
    (MfwMnuItem*)Mmi_mp3_OPTItems,                      /* with these items         */
    sizeof(Mmi_mp3_OPTItems)/sizeof(MfwMnuItem), /* number of items     */
    COLOUR_LIST_SMS,	TxtNull, NULL, MNUATTRSPARE

};

/*******************************************************************************

 $Function:		mmi_mp3_test_opt_win_cb

 $Description:		

 $Returns:		MFW_EVENT_CONSUMED 

 $Arguments:		event		- window handle event
 				win			- Camera window

*******************************************************************************/
int mmi_mp3_test_opt_win_cb (MfwEvt evt, MfwHnd win)
{
    switch (evt)
				{
        case MfwWinVisible:  /* window is visible  */
	    displaySoftKeys(TxtSoftSelect,TxtSoftBack);
	    break;
	case MfwWinFocussed: /* input focus / selected   */
	case MfwWinDelete:   /* window will be deleted   */
        default:
	    return MFW_EVENT_REJECTED;
				}
    return MFW_EVENT_CONSUMED;

		    }

/*******************************************************************************

 $Function:    	mmi_mp3_test_opt_kbd_cb

 $Description:		

 $Returns:		execution status

 $Arguments:	e - event id
 				k - keyboard info
*******************************************************************************/
 
static int mmi_mp3_test_opt_kbd_cb (MfwEvt e, MfwKbd *k)
 {
    T_MFW_HND    win  = mfwParent(mfw_header());
    T_MFW_WIN  * win_data = ((T_MFW_HDR *)win)->data;
    T_MMI_Player_Win_data      * data = (T_MMI_Player_Win_data *)win_data->user;


    if (e & KEY_LONG)
	{
	    switch (k->code)
		{
		case KCD_HUP: /* back to previous menu */
		    mnuEscape(data->menu);
		    break;
		case KCD_RIGHT: /* Power Down */
		    return MFW_EVENT_REJECTED; /* handled by idle */
		default: /* no response to all other keys */
		    return MFW_EVENT_CONSUMED;
		}
	}
    else
	{
	switch (k->code)
	{
		case KCD_MNUUP: /* highlight previous entry */
		    mnuUp(data->menu);
		    break;
		case KCD_MNUDOWN: /* highlight next entry */
		    mnuDown(data->menu);
		    break;
		case KCD_MNUSELECT:
		case KCD_LEFT: /* activate this entry */
		    mnuSelect(data->menu);
		    break;
		case KCD_HUP: /* back to previous menu */
		case KCD_RIGHT: /* back to previous menu */
		    mnuEscape(data->menu);
				break;
		default: /* no response to all other keys */
		    return MFW_EVENT_CONSUMED;
		}
	}
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	mmi_mp3_test_opt_mnu_cb

 $Description:	Menu callback function for the Option window

 $Returns:		Execution status

 $Arguments:	e - event id
 				m - menu handle
*******************************************************************************/

static int mmi_mp3_test_opt_mnu_cb (MfwEvt e, MfwMnu *m)
{
    switch (e)
	{
	case E_MNU_ESCAPE: /* back to previous menu */
		TRACE_EVENT("E_MNU_ESCAPE");
		SEND_EVENT (optionwin, MP3_RETURN, NULL, NULL);
		break;
	default: /* in mnuCreate() only E_MNU_ESCAPE has been enabled! */
	    return MFW_EVENT_REJECTED;
	}
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	mmi_mp3_test_opt_destroy

 $Description:		Destroy the Camera Option window
 				
 $Returns:		mfw window handle

 $Arguments:	own_window - current window

*******************************************************************************/

static void mmi_mp3_test_opt_destroy(MfwHnd own_window)
{
    T_MFW_WIN * win_data;
    T_MMI_Player_Win_data     * data;

    if (own_window)
	{
	    win_data = ((T_MFW_HDR *)own_window)->data;
	    data = (T_MMI_Player_Win_data *)win_data->user;

	    if (data)
		{

		   /*
		** Failed to start : Free Memory, and exit
		*/

		if(data->kbd != NULL)
				{
		           kbdDelete (data->kbd);
				}

		if(data->menu!= NULL)
      		{
		   mnuDelete(data->menu);
		    }

		     // Delete WIN handler
		     
			   win_delete (data->win);

		     // Free Memory
		  
		    FREE_MEMORY ((void *)data, sizeof (T_MMI_Player_Win_data));
	}
	else
			   {
		    TRACE_EVENT ("mmi_mp3_test_opt_destroy() called twice");
               }
		    }
}


/*******************************************************************************

 $Function:		mmi_mp3_test_opt_exec_cb

 $Description:		
 $Returns:		None

 $Arguments:		win :		The pointer to the window handler, so the function can reference the
							dynamic user data, if any.
 				event :		The incoming event
 				value :		Generic incoming Parameter, it will be 'event' dependant
 				parameter :	Generic incoming Parameter, it will be 'event' dependant

*******************************************************************************/
void mmi_mp3_test_opt_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN        * win_data = ((T_MFW_HDR *) win)->data;
    T_MMI_Player_Win_data  * data     = (T_MMI_Player_Win_data *)win_data->user;
     TRACE_FUNCTION ("mmi_mp3_test_opt_exec_cb()");
    switch (event)
			   {
	case MP3_INIT:
		/* initialization of administrative data */
			data->kbd =      kbdCreate(data->win,KEY_ALL,         (MfwCb)mmi_mp3_test_opt_kbd_cb);
			data->menu =     mnuCreate(data->win,(MfwMnuAttr *)&Mmi_mp3_OPTAttrib, E_MNU_ESCAPE, (MfwCb)mmi_mp3_test_opt_mnu_cb);
			mnuLang(data->menu,mainMmiLng);

			/* put the (new) dialog window on top of the window stack */
			mnuUnhide(data->menu);
			winShow(data->win);
			break;

	case MP3_RETURN:
		mmi_mp3_test_opt_destroy(optionwin);
		optionwin = NULL;
		dspl_ClearAll();
		ALIGNED_PROMPT(LEFT,Mmi_layout_line(1),0, TxtPlayerResume);
		displaySoftKeys(TxtSoftOptions, TxtStop);
	      break;

		default:
	    return;
	}
}


/*******************************************************************************

 $Function:		mmi_mp3_test_opt_win_create

 $Description:		

 $Returns:		

 $Arguments:		parent : pointer to the parent window.

*******************************************************************************/
T_MFW_HND mmi_mp3_test_opt_create(T_MFW_HND parent)
		{
	 T_MMI_Player_Win_data      * data = (T_MMI_Player_Win_data *)ALLOC_MEMORY (sizeof (T_MMI_Player_Win_data));
    T_MFW_WIN  * win;

    TRACE_FUNCTION ("mmi_mp3_test_opt_create()");

	    	/*
     * Create window handler
		    */

    data->win = 
	win_create (parent, 0, E_WIN_VISIBLE, (T_MFW_CB)mmi_mp3_test_opt_win_cb);
    if (data->win EQ NULL)
				{
	    return NULL;
	}
       	    /*
     * connect the dialog data to the MFW-window
		    */

    data->mmi_control.dialog = (T_DIALOG_FUNC)mmi_mp3_test_opt_exec_cb;
		    data->mmi_control.data = data;
		    win = ((T_MFW_HDR *)data->win)->data;
	    	win->user = (void *)data;
    data->parent_win             = parent;

		    /*
     * return window handle
		    */
	
    return data->win;
	}

/*******************************************************************************

 $Function:    	mmi_mp3_test_opt_start

 $Description:		
 				
 $Returns:		

 $Arguments:	parent_window -parent window handle
 				menuAttr - Menu attributes.

*******************************************************************************/

T_MFW_HND mmi_mp3_test_opt_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr)
{
//    T_MFW_HND win;

    TRACE_FUNCTION ("mmi_mp3_test_opt_start()");

    optionwin = (T_MFW_HND)mmi_mp3_test_opt_create (parent_window);

    if (optionwin NEQ NULL)
        {
	    SEND_EVENT (optionwin, MP3_INIT, NULL, (void *)menuAttr);
	}
    return optionwin;
	}

#endif
/*******************************************************************************

 $Function:     mmi_audio_player_menu_destroy

 $Description: destroy menu window

 $Returns:

 $Arguments:

*******************************************************************************/
static void mmi_audio_player_menu_destroy(MfwHnd window)
{
	T_MFW_WIN     * win_data  = ((T_MFW_HDR *)window)->data;
	T_player_menu * data = (T_player_menu *)win_data->user;
    TRACE_FUNCTION ("mmi_audio_player_menu_destroy()");


	if (window == NULL)
	{
		return;
	}

	if (data)
	{
		win_delete(data->win);
		
		/*
		** Free Memory
		*/
		mfwFree((void *)data, sizeof (T_player_menu));

	}

}
//Sep 29,2005 REF: SPR 34407 xdeepadh  
/*******************************************************************************
 $Function:     mmi_audio_player_menu_list_listmnu_cb

 $Description: destroy lists and menu in case of back softkey or menu exit

 $Returns:

 $Arguments:

*******************************************************************************/
static void mmi_audio_player_menu_list_listmnu_cb(T_MFW_HND Parent, ListMenuData * ListData)
{
	T_MFW_WIN			*win_data	= ((T_MFW_HDR *)Parent)->data;
	T_player_menu	*data		= (T_player_menu *)win_data->user;
	char *fileExt;

	TRACE_FUNCTION ("mmi_audio_player_menu_list_listmnu_cb()");

	if ((ListData->Reason EQ LISTS_REASON_BACK) ||
	     (ListData->Reason EQ LISTS_REASON_CLEAR) ||
	     (ListData->Reason EQ LISTS_REASON_HANGUP))
	{
		/*
		** Destroy the window --- Not sure if this is required! ... leave it out for now
		*/
		 listsDestroy(ListData->win);

		/*
		** Free the memory set aside for ALL the devices to be listed.
		*/
		if (data->menu_list_data != NULL)
		{
			FREE_MEMORY ((void *)data->menu_list_data->List, (U16)(data->player_num_file* sizeof (T_MFW_MNU_ITEM)));
			FREE_MEMORY ((void *)data->menu_list_data, sizeof (ListMenuData));
		}

     		 mmi_audio_player_menu_destroy(data->win);


	}
	//Sep 29,2005 REF: SPR 34407 xdeepadh  
	else if(ListData->Reason EQ LISTS_REASON_SELECT)
	{
		TRACE_EVENT_P1("list position is %d",ListData->ListPosition);
		//call mfw to save selected index
		mfw_audio_player_save_selected_file_idx(ListData->ListPosition);
		fileExt =(char*) mfw_audio_player_GetExtension(inputFileName);	

		//Decide whether to play Mp3 or AAC
		if ( (strcmp(fileExt, "mp3") == 0))
		{
			IsMP3On = MFW_PLAYER_MP3;
		}
#ifdef FF_MMI_TEST_AAC		
		else if ( (strcmp(fileExt, "aac") == 0))
		{
			IsMP3On = MFW_PLAYER_AAC;
		}
#endif	
		data->player_list_idx = ListData->ListPosition;
		mmi_audio_player_show_info(0, TxtPlayerFileSelected, TxtNull, NULL);
	}

	return;
}

/*******************************************************************************

 $Function:     mmi_audio_player_files_win_cb

 $Description: Window event handler

 $Returns:	MFW_EVENT_REJECTED: 
 			MFW_EVENT_CONSUMED:

 $Arguments:	

*******************************************************************************/
static int mmi_audio_player_files_win_cb (MfwEvt e, MfwWin *w)    
{
  	TRACE_FUNCTION ("mmi_audio_player_files_win_cb()");
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

 $Function:     mmi_audio_player_files_exec_cb

 $Description: Call back function

 $Returns:	none

 $Arguments:

*******************************************************************************/
void mmi_audio_player_files_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    	T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    	T_player_menu   * data = (T_player_menu *)win_data->user;    
  int max_file_number;
  int i;
  
    	TRACE_FUNCTION ("mmi_audio_player_files_exec_cb");

    	switch (event)
  	{
    		case E_INIT:

    TRACE_EVENT(">>>> mmi_audio_player_files_exec_cb() Event:E_INIT");

     /* initialization of administrative data */

    data->menu_list_data = (ListMenuData *)ALLOC_MEMORY(sizeof(ListMenuData));

    if(data->menu_list_data == 0)
    {
      TRACE_EVENT("Failed memory alloc 1 ");
      return;
    }

	  max_file_number = mfw_audio_player_return_file_number();
				//In case no files have been loaded display info message
  //In case no files have been loaded return info message
  if(max_file_number == 0)
  {
		  	      mmi_audio_player_show_info(0, TxtPlayerFileNotLoaded, TxtNull, NULL);
	return;
  }
 

    data->menu_list_data->List = (T_MFW_MNU_ITEM *)ALLOC_MEMORY( max_file_number * sizeof(T_MFW_MNU_ITEM) );

    if(data->menu_list_data->List == 0)
			{
      TRACE_EVENT("Failed memory alloc 2");
      return;
    }
//Sep 29,2005 REF: SPR 34407 xdeepadh  
	for (i = 0; i < max_file_number ; i++)
	{
		mnuInitDataItem(&data->menu_list_data->List[i]);
		data->menu_list_data->List[i].flagFunc = item_flag_none;
		//Display the Player file names in the list
		data->menu_list_data->List[i].str  = (char *)mfw_audio_player_return_file_name(data->player_file_count++);
	}

      data->menu_list_data->ListLength =max_file_number;
    data->menu_list_data->ListPosition = 1;
    data->menu_list_data->CursorPosition = 1;
    data->menu_list_data->SnapshotSize = max_file_number;
    data->menu_list_data->Font = 0;
    data->menu_list_data->LeftSoftKey = TxtSoftSelect;
    data->menu_list_data->RightSoftKey = TxtSoftBack;
    data->menu_list_data->KeyEvents = KEY_ALL;
    data->menu_list_data->Reason = 0;
    data->menu_list_data->Strings = TRUE;
    data->menu_list_data->Attr   = (MfwMnuAttr*)&menuPlayerSearchListAttr;
//Apr 05, 2005    REF: ENH 29994 xdeepadh	

 	        data->menu_list_data->Attr->hdrId = TxtPlayerTest;
    data->menu_list_data->autoDestroy    = FALSE;
  	    listDisplayListMenu(win, data->menu_list_data,(ListCbFunc)mmi_audio_player_menu_list_listmnu_cb,0);

                    	break;

  
  			default:
      			break;
  	}

}


/*******************************************************************************

 $Function:     mmi_audio_player_files_browse

 $Description: This function is called when the user selects "Browse Player Files" option

 $Returns:	MFW_EVENT_CONSUMED

 $Arguments:	
 			

*******************************************************************************/
GLOBAL int mmi_audio_player_files_browse( MfwMnu *m, MfwMnuItem *i )
{
  	T_MFW_HND win;
    	T_MFW_HND parent_win = mfw_parent(mfw_header());
	int numberOfMelodies=0;

	TRACE_FUNCTION ("mmi_audio_player_files_browse");
	//Sep 11, 2006 DR: OMAPS00094182 xrashmic	     
	     mfw_audio_player_init();

	numberOfMelodies = mfw_audio_player_return_file_number();
	TRACE_EVENT_P1("numberOfMelodies is %d",numberOfMelodies);

	if(numberOfMelodies <= 0)
	{
		mmi_audio_player_show_info(0, TxtPlayerFileNotLoaded, TxtNull, NULL);
		
	}
	else
	{
		win = mmi_audio_player_build_player_Menu(parent_win);

   	if (win NEQ NULL)
   	{
      		SEND_EVENT (win, E_INIT, 0, 0);
   	}
	
	}
   	return MFW_EVENT_CONSUMED;
}


//Sep 29,2005 REF: SPR 34407 xdeepadh  
/*******************************************************************************

 $Function:     mmi_audio_player_build_player_Menu

 $Description: To hold list window

 $Returns:	Window handle

 $Arguments:	
*******************************************************************************/

static T_MFW_HND  mmi_audio_player_build_player_Menu( MfwHnd parent_window)
{
 	T_player_menu *  data = (T_player_menu *)mfwAlloc(sizeof (T_player_menu));
  	T_MFW_WIN  * win;

    	TRACE_FUNCTION ("mmi_audio_player_build_player_Menu");

    	/*
     	* Create window handler
     	*/

    	data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)mmi_audio_player_files_win_cb);
    	if (data->win EQ NULL)
   	{
      		return NULL;
   	}

  	TRACE_EVENT("list holder window created: " );
    	/*
     	* connect the dialog data to the MFW-window
     	*/

    	data->mmi_control.dialog = (T_DIALOG_FUNC)mmi_audio_player_files_exec_cb;
    	data->mmi_control.data   = data;
    	win                      = ((T_MFW_HDR *)data->win)->data;
    	win->user                = (void *)data;
    	data->parent             = parent_window;
    	winShow(data->win);
    	/*
     	* return window handle
     	*/

     	return data->win;
}

//Sep 27,2005 REF: SPR 34402 xdeepadh  
/*******************************************************************************

 $Function:     		mmi_audio_player_set_cur_selected_channels

 $Description: 	The Menu for Player channels  will be displayed,
 				by highlighting the selected menu item.

 $Returns:None

 $Arguments:None

*******************************************************************************/

void mmi_audio_player_set_cur_selected_channels(void)
{
	UBYTE			CurSel;
	T_MFW_HND 		setting_win;
	T_MFW_HND 		win = mfw_parent(mfw_header());
	
    setting_win =(T_MFW_HND) bookMenuStart(win, (MfwMnuAttr*)PlayerChannelAttributes(), 0);

	CurSel = FFS_flashData.player_channel_state;
	SEND_EVENT(setting_win, DEFAULT_OPTION, NULL, &CurSel);
}

#if (BOARD == 71)
/*******************************************************************************

 $Function:     mmi_audio_player_pause

 $Description: set player to pause

 $Returns:

 $Arguments:

*******************************************************************************/
int M_exePause (MfwMnu* m, MfwMnuItem* i)
{

		mfw_audio_player_pause();
		IsPaused=TRUE;
		mmi_mp3_test_opt_destroy(optionwin);
		optionwin = NULL;
		dspl_ClearAll();
		ALIGNED_PROMPT(LEFT,Mmi_layout_line(1),0, TxtPlayerPause);
		displaySoftKeys(TxtPlayerResume, TxtStop);


  return 1;
}


/*******************************************************************************

 $Function:     mmi_audio_player_forward

 $Description: set player to forward

 $Returns:

 $Arguments:

*******************************************************************************/
int M_exeForward (MfwMnu* m, MfwMnuItem* i)
{
		
		mmi_mp3_test_opt_destroy(optionwin);
		optionwin = NULL;
		dspl_ClearAll();
		ALIGNED_PROMPT(LEFT,Mmi_layout_line(1),0, TxtPlayerResume);
		displaySoftKeys(TxtSoftOptions, TxtStop);
		mfw_audio_player_forward(MP3_FORWARD_REWIND_TIME);
		mmi_audio_player_show_info(0, TxtPlayerForwarding, TxtNull, NULL);
		
  return 1;
}

/*******************************************************************************

 $Function:     mmi_audio_player_rewind

 $Description: set player to rewind

 $Returns:

 $Arguments:

*******************************************************************************/
int M_exeRewind (MfwMnu* m, MfwMnuItem* i)
{
		mmi_mp3_test_opt_destroy(optionwin);
		optionwin = NULL;
		dspl_ClearAll();
		ALIGNED_PROMPT(LEFT,Mmi_layout_line(1),0, TxtPlayerResume);
		displaySoftKeys(TxtSoftOptions, TxtStop);
		mfw_audio_player_rewind(MP3_FORWARD_REWIND_TIME);
		mmi_audio_player_show_info(0, TxtPlayerRewinding, TxtNull, NULL);

  return 1;
}
#endif

/*******************************************************************************

 $Function:     mmi_audio_player_set_channel_mono

 $Description: set channels to mono

 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL int mmi_audio_player_set_channel_mono (MfwMnu* m, MfwMnuItem* i)
{

  SHORT	mfw_audio_player_retVal;
   
  TRACE_FUNCTION ("mmi_audio_player_set_channel_mono()");
 
  mfw_audio_player_retVal = mfw_audio_player_set_channel_mono();
  if(mfw_audio_player_retVal != MFW_PLAYER_TEST_OK)
  {
	TRACE_FUNCTION ("mmi_audio_player_set_channel_mono(): Riviera Failed"); 
	mmi_audio_player_show_info(0, TxtPlayerTest, TxtNull, NULL);
  }
  else
  {
	// Sep 27,2005 REF: SPR 34402 xdeepadh  
	// The channel state will be written into the flash.
	FFS_flashData.player_channel_state = PLAYER_CHANNEL_MONO ;
	flash_write();
	mmi_audio_player_show_info(0, TxtPlayerMonoChannel, TxtNull, NULL);
  }

  return 1;
}

/*******************************************************************************

 $Function:     mmi_audio_player_set_channel_stereo

 $Description: set channels to stereo

 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL int mmi_audio_player_set_channel_stereo (MfwMnu* m, MfwMnuItem* i)
{

  SHORT	mfw_audio_player_retVal;

  TRACE_FUNCTION ("mmi_audio_player_set_channel_stereo()");
  mfw_audio_player_retVal = mfw_audio_player_set_channel_stereo();
  if(mfw_audio_player_retVal != MFW_PLAYER_TEST_OK)
  {
	TRACE_FUNCTION ("mmi_audio_player_set_channel_stereo(): Riviera Failed"); 
	mmi_audio_player_show_info(0, TxtPlayerTest, TxtNull, NULL);
  }
  else
  {
	// Sep 27,2005 REF: SPR 34402 xdeepadh  
	// The channel state will be written into the flash.
	FFS_flashData.player_channel_state = PLAYER_CHANNEL_STEREO;
	flash_write();
	mmi_audio_player_show_info(0, TxtPlayerStereoChannel, TxtNull, NULL);
  }

  return 1;
}


/*******************************************************************************

 $Function:     mmi_audio_player_show_info

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
static MfwHnd mmi_audio_player_show_info(T_MFW_HND parent, int str1, int str2, T_VOID_FUNC callback)
{
	T_DISPLAY_DATA display_info;

       TRACE_FUNCTION ("mmi_audio_player_show_info()");

	/*
	** Create a timed dialog to display the Message "Failed"
	*/
	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, str1, str2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, callback, PLAYER_INFO_SCRN_TIMEOUT, KEY_LEFT | KEY_CLEAR | KEY_HUP);

	return info_dialog(parent, &display_info);
}


//Jul 18, 2005    REF: SPR 31695   xdeepadh

/*******************************************************************************

 $Function:     mmi_audio_player_play_destroy

 $Description:  Destruction of an dialog

 $Returns:    void

 $Arguments:  win

*******************************************************************************/
void mmi_audio_player_play_destroy (T_MFW_HND own_window)
{
		T_MFW_WIN     * win  = ((T_MFW_HDR *)own_window)->data;
		T_MMI_Player_Win_data * data   = (T_MMI_Player_Win_data *)win->user;

		TRACE_FUNCTION ("mmi_audio_player_play_destroy()");

		if (own_window == NULL)
		{
			TRACE_EVENT ("Error : Called with NULL Pointer");
			return;
		}

		if (data)
		{
			/*
			* Exit  KEYBOARD Handle
			*/
			kbd_delete (data->kbd);

			/*
			* Delete WIN Handler
			*/
			win_delete (data->info_win);
			/*
			* Free Memory
			*/
			FREE_MEMORY ((void *)data, sizeof (T_MMI_Player_Win_data));
		}
}

//Jul 18, 2005    REF: SPR 31695   xdeepadh

/*******************************************************************************

 $Function:     mmi_audio_player_play_kbd_cb

 $Description:  Keyboard event handler

 $Returns:    status int

 $Arguments:  window handle event, keyboard control block

*******************************************************************************/

static int mmi_audio_player_play_kbd_cb (T_MFW_EVENT event, T_MFW_KBD *keyboard)
{
  T_MFW_HND       win  = mfw_parent (mfw_header());
	T_MFW_WIN	*win_data = ((T_MFW_HDR *)win)->data;
	T_MMI_Player_Win_data	*data = (T_MMI_Player_Win_data *)win_data->user;

    TRACE_FUNCTION("mmi_audio_player_play_kbd_cb");

  switch (keyboard->code)
	{

		case KCD_LEFT:

	   if(IsPaused == FALSE)
	{
#if (BOARD == 71)
		if(IsMP3On == MFW_PLAYER_AAC)
		{
#endif		
			mfw_audio_player_pause();
			/*
			* Clear Screen
			*/
			dspl_ClearAll();
			/*
			* Print the information screen
			*/
			ALIGNED_PROMPT(LEFT,Mmi_layout_line(1),0, TxtPlayerPause);
			/*
			* Print softkeys
			*/
			displaySoftKeys(TxtPlayerResume, TxtStop);
			IsPaused=TRUE;
#if (BOARD == 71)
		}
		else
		{
			mmi_mp3_test_opt_start(data->win,(MfwMnuAttr*)&Mmi_mp3_OPTAttrib);
		}
#endif		
	}
			   else
			   {

		/*
		* Clear Screen
		*/
		dspl_ClearAll();
			      mfw_audio_player_resume();
		/*
		* Print the information screen
		*/
		ALIGNED_PROMPT(LEFT,Mmi_layout_line(1),0, TxtPlayerResume);

		/*
		* Print softkeys
		*/
#if (BOARD == 71)		
		if(IsMP3On == MFW_PLAYER_AAC)
#endif			
			displaySoftKeys(TxtPlayerPause, TxtStop);
#if (BOARD == 71)
		else	
			displaySoftKeys(TxtSoftOptions, TxtStop);
#endif
				  IsPaused = FALSE;
			   }
		   break;
	case KCD_HUP:
	case KCD_RIGHT:
				mfw_audio_player_stop();
				IsPaused = FALSE;/*a0393213 warnings removal- == made =*/
		//go back to the previous dialog
		SEND_EVENT (win, PLAYER_DESTROY, 0, 0);
	    break;

		default:
			break;
	}
	return MFW_EVENT_CONSUMED;
}
//Nov 16,2005 REF: DR OMAPS00049192  x0039928
/*******************************************************************************

 $Function:     mmi_audio_player_start_cb

 $Description:  Mmi callback function for mp3 test application
 				
 $Returns:    void

 $Arguments:  win, window handle 

*******************************************************************************/
void mmi_audio_player_start_cb(T_MFW_HND win, SHORT value)
{
// Jun 02,2006 REF: DR OMAPS00079746  x0039928
// Fix: MP3 playback error is handled.
    if (value < 0)
        mmi_audio_player_show_info(0, TxtError, TxtNull, NULL);

#if (BOARD == 71)	
   if(optionwin != NULL)
    {        mmi_mp3_test_opt_destroy(optionwin);
	 optionwin = NULL;
    }
#endif

    IsPaused = FALSE;/*a0393213 warnings removal- '==' made '='*/
 mmi_audio_player_play_destroy(win);
    dspl_Enable(TRUE);
}

//Nov 16,2005 REF: DR OMAPS00049192  x0039928
/*******************************************************************************

 $Function:     mmi_audio_player_callback_init

 $Description:  Initializes the mmi return callback path.
 				
 $Returns:    void

 $Arguments:  window handle, callback function 

*******************************************************************************/
//Nov 16,2005 REF: DR OMAPS00049192  x0039928
static void mmi_audio_player_callback_init(T_MFW_HND win, void (*callback)(T_MFW_HND, SHORT ))
{
    mmi_audio_player_return_path.focus_win = win;
    mmi_audio_player_return_path.callback = callback;
}

//Jul 18, 2005    REF: SPR 31695   xdeepadh
/*******************************************************************************

 $Function:     mmi_audio_player_play_exec_cb

 $Description:  Dialog function for Player test app dialog
 				
 $Returns:    void

 $Arguments:  win, window handle event, value, parameter

*******************************************************************************/
void mmi_audio_player_play_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	  T_MFW_WIN      * win_data     = ((T_MFW_HDR *)win)->data;
	  T_MMI_Player_Win_data    * data         = (T_MMI_Player_Win_data *)win_data->user;
	  
	  TRACE_FUNCTION ("mmi_audio_player_play_exec_cb()");


  switch (event)
	{
		case PLAYER_INIT:
			TRACE_EVENT ("PLAYER_INIT()");
	// Nov 16,2005 REF: DR OMAPS00049192  x0039928
			// Initialize the callback function to the return path.
                     mmi_audio_player_callback_init(win, mmi_audio_player_start_cb);
		

			
			// Initialize dialog
			//Create  keyboard handler
				data->kbd = kbd_create (win, KEY_ALL, (T_MFW_CB)mmi_audio_player_play_kbd_cb);
			if (value EQ PLAYER_PLAY)
			{
				TRACE_EVENT ("PLAYER_PLAY()");

				mfw_audio_player_play();
				win_show (win);
			}
		break;
	
		case PLAYER_DESTROY:
			mmi_audio_player_play_destroy(win);
	break;
	}
}

//Jul 18, 2005    REF: SPR 31695   xdeepadh

/*******************************************************************************

 $Function:    mmi_audio_player_play_win_cb

 $Description:  Callback function for information dialog

 $Returns:    void

 $Arguments:  window handle event, win
*******************************************************************************/
static int mmi_audio_player_play_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
    T_MMI_Player_Win_data * data = (T_MMI_Player_Win_data *)win->user;

    TRACE_FUNCTION ("mmi_audio_player_play_win_cb()");

    if (data EQ NULL)
 	 return NULL;

    switch (event)
  {
  case E_WIN_VISIBLE:
      if (win->flags & E_WIN_VISIBLE)
	{
		/*
		* Clear Screen
		*/
		dspl_ClearAll();
		/*
		* Print the information screen
		*/
		ALIGNED_PROMPT(LEFT,Mmi_layout_line(1),0, TxtPlayerPlaying);
		/*
		* Print softkeys
		*/
#if (BOARD == 71)		
		if(IsMP3On == MFW_PLAYER_AAC)
#endif			
			displaySoftKeys(TxtPlayerPause, TxtStop);
#if (BOARD == 71)
		else
			displaySoftKeys(TxtSoftOptions,TxtStop );
#endif		
	}
	break;

	default:
	return 0;
  }
    return 1;
}

//Jul 18, 2005    REF: SPR 31695   xdeepadh
/*******************************************************************************

 $Function:     mmi_audio_player_play_create

 $Description:  Creation of window

 $Returns:    Dialogue info win

 $Arguments:  parent win

*******************************************************************************/
static T_MFW_HND mmi_audio_player_play_create (T_MFW_HND parent_win)
	{
  T_MFW_WIN     * win_data;
  T_MMI_Player_Win_data *  data = (T_MMI_Player_Win_data *)ALLOC_MEMORY (sizeof (T_MMI_Player_Win_data));

  TRACE_FUNCTION ("mmi_audio_player_play_create()");

  data->info_win = win_create (parent_win, 0, E_WIN_VISIBLE, (T_MFW_CB)mmi_audio_player_play_win_cb);

  if (data->info_win EQ NULL)
    return NULL;

       	/*
   * Create window handler
		*/
  data->mmi_control.dialog   = (T_DIALOG_FUNC)mmi_audio_player_play_exec_cb;
		data->mmi_control.data = data;
  data->parent_win           = parent_win;
  win_data                   = ((T_MFW_HDR *)data->info_win)->data;
  win_data->user             = (void *)data;

    /*
   * return window handle
   */
  return data->info_win;
}

//Jul 18, 2005    REF: SPR 31695   xdeepadh
/*******************************************************************************

 $Function:     mmi_audio_player_play_start

 $Description:  create a new voicememo dialog

 $Returns:    Dialogue info win

 $Arguments:  parent win, display info

*******************************************************************************/
T_MFW_HND mmi_audio_player_play_start (T_MFW_HND  parent_win, SHORT ID)
{
  T_MFW_HND win;

  win = mmi_audio_player_play_create (parent_win);
  

  if (win NEQ NULL)
  {
    SEND_EVENT (win, PLAYER_INIT, ID, 0);
  }
  return win;
}


//Jul 18, 2005    REF: SPR 31695   xdeepadh

/*******************************************************************************

 $Function:     mmi_audio_player_play

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL int  mmi_audio_player_play (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND win = mfw_parent(mfw_header());
	int max_file_number;
  
	TRACE_FUNCTION ("mmi_audio_player_play()");

	//If no file is selected inform the user
	//retrieve from MFW number of files loaded
	max_file_number = mfw_audio_player_return_file_number();
	//In case no files have been loaded return info message
	if(max_file_number == 0)
	{
		mmi_audio_player_show_info(0, TxtPlayerFileNotLoaded, TxtNull, NULL);
	}
	//If no file is selected inform the user
	else if(audio_player_currently_playing_idx < 0)

	{
		mmi_audio_player_show_info(0, TxtPlayerSelectFile, TxtNull, NULL);
				
	}
	else
	{
		//create a new Player playing screen
		mmi_audio_player_play_start (win, PLAYER_PLAY);
	}

  return 1;
}

	//Sep 11, 2006 DR: OMAPS00094182 xrashmic	     
/*******************************************************************************

 $Function:     mmi_audio_player_Application_cb

 $Description: Callback function called on exiting the audio player appilcation

 $Returns:	None

 $Arguments:	parent_win : Parent window
 			identifier :    
 			reason: 
*******************************************************************************/
static void mmi_audio_player_Application_cb(T_MFW_HND parent_win, UBYTE identifier, UBYTE reason)
{
	TRACE_FUNCTION("mmi_audio_player_Application_cb");
	mfw_audio_player_unpopulate_files();
}
/*******************************************************************************

 $Function:     mmi_audio_player_Application

 $Description: Launcing the audio player application menu

 $Returns:

 $Arguments:		m : pointer to the mnu data for the menu which caused this function to be called.
 				i   : pointer to the item data from the menu.

*******************************************************************************/

GLOBAL int  mmi_audio_player_Application (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND 		win;
	T_MFW_HND 		parent_win = mfw_parent(mfw_header());
	TRACE_FUNCTION("mmi_audio_player_Application");
	win=bookMenuStart(parent_win, (MfwMnuAttr *)AudioPlayerAppMenuAttributes(),0);
	SEND_EVENT(win, ADD_CALLBACK, NULL, (void *)mmi_audio_player_Application_cb);
	return MFW_EVENT_CONSUMED;
}

