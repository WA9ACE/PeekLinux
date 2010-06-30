
#ifdef FF_MMI_FILEMANAGER

/* =========================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================== */

/*==========================================================
* @file mmi_FileManager.c
*
* This provides the functionality of File Manager Applications.
* It supports file/folder related operations on FFS and MMC
*
* @path  \bmi\condat\ms\src\bmi
*
* @rev  00.01
*/
/* ========================================================== */
/*===========================================================
*!
*! Revision History
*! ===================================

	Nov 06, 2007 DRT: OMAPS00151698 x0056422
    Description: BMI requirments needed to support Bluetooth AVRCP in Locosto 5.x
    Solution : In mfw_audResume, Pause, Stop functions, if BT is connected, send a callback. 
               In the case of audio play, handle the callback of START_IND from audio task and 
               post a callback to BT if expected. 

	August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat)
	Description:  COMBO: Board reboots on receiving call during file viewer
	Issue:          When an incoming call / SMS arrives, a winShow of Idle's
	                   window is done (to update the Icons etc..) which causes all windows
	                   chained to Idle's window to be updated. In this process, even the
	                   Image List and Image Viewer Windows' Updates happen, wherein the existing
	                   MSL Pipeline is used to draw the Viewer (ThumbNail / QCIF). During this,
	                   many of the global flags to maintain the status, command etc.. seem to get
	                   reinitialized and corrupted and this lead to a weird memory corruption and hence
	                   a crash.
	Solution:      A Status Indicator is maintained each for Image List and Image Viewer cases.
	                   This shall be updated / set whenever the window gets suspended. In such cases,
	                   the MSL Viewer Drawing shall not be done even when a WinVisible is received
	                   and this flag shall be reset during the window create/destroy/resume.
	                   Essentially, the Image List window / Image Viewer window shall call
	                   MSL_Viewer_View ONLY when it is in the foreground and shall not do the same


    	Jul 10, 2007 DRT:OMAPS00138065 x0062174(Nimitha)
    	Description: Stop button disappears while we play an audio file when progressbar is on and  the only way to stop the file is to press back.
    	STOP button is appearing for a moment if progress bar is ON.
    	Solution: STOP button should not appear in progressbar window. STOP button is displayed at proper place.
    	When progressbar is on, to stop the audio play we have to press BACK button, that is by design.

    	Jul 04, 2007 DRT:OMAPS00135749  x0062174(Nimitha)
    	Description: Once the audio file has finished automatically play button has to be pressed twice,
    	in order to be played again.
    	Solution : a) A Global variable aud_state_status is added to maintain the status of the player once the user presses Back button 
    	                  during the file playing and coming back to File Management window. Two function  mmi_set_aud_state_status and 
    	                  mmi_get_aud_state_status is added to set/get the status of the audio player.
    	               b) A condition is added to check the status of the audio player when user presses the joystick, 
    	                   audio state is made to FM_AUD_PLAY if the audio is still playing. 
    	               c) While drawing File Management window a condition is added to check the status of the audio player and proper PLAY/STOP key is displayed
    	                  according to the status of the player. 
    
	June 11, 2007 DRT:OMAPS00132505 x0061088
	Description:  Impossible to see an 303kb image
	Solution: Now  thumb generation error given by MSL  synchronously is handled MMI.  
      May28,07 DRT:OMAPS00132459  x0061560(sateeshg)
      Description: while midi is playing stop button makes restart the playing
      Solution:A flag been added when user plays the file and this flag is checked after coming back from current
      directory to pause the play using joystick.

	April 27, 2007 DRT:OMAPS00128836 x0073106
	Description: Unexpected behavior when copying images with the same names  from a memory to another.
	Solution:In function mmi_fm_copy_start() added FM_COPY_NONE flag 
	and in function mmi_copy_End() added  FM_COPY_NONE in two conditions.

	Apr 10, 2007 DRT: OMAPS00125325 x0039928
	Description: Hang up key doesn't work in File Manager audio and image lists
	Solution: Hang up key is now mapped to back operation in the list.
	
	Apr 05, 2007 DRT: OMAPS00124859 x0039928
	Description: MM: Imelody can not be moved or copied
	Solution: Copy-Move is displayed for all audio files
	
	Apr 06, 2007 DRT: OMAPS00124874 x0039928
	Description: MM: Renaming process of an Imy file is incorrect
	Solution: Seperate enum values for different audio files is added.
	
      Apr 06, 2007    DRT: OMAPS00124877  x0039928
      Description: MM: Voice limit Options doesn't work
      Solution: voice limit value is taken from the global variable set by menu options.
    
	Mar 30, 2007 DRT: OMAPS00122768 x0039928
	Description: MM: The board is blocked after having moved a file between NOR and T-FLASH
	Solution: Image viewer is deinitialized when there are no image files in image viewer.
	
	Mar 30, 2007 DRT: OMAPS00122772 x0039928
	Description: MM: Image viewer in Filemanagement is blocked when there is just a directory
	Solution: An invalid condition is removed which is checked aginst press of back key in FMA 
	
	Mar 28, 2007   ER: OMAPS00106188  x0039928
	Description: Align Midi application menu choices and audio Player menu choices 
	
	Mar 29, 2007 DRT: OMAPS00122677 x0039928
	Description: MM: By default, Storage devices in Camera Application is not set, taking a snapshot is impossible
	Solution: Root directories are created on entering camera application.
	
	Mar 15, 2007 DRT: OMAPS00120225 x0039928
	Description: File Manager:Copy, move between NOR-MS,T-FLASH,NAND failed
	Solution: A condition is added to check the source and destination device are same.
   			
	Mar 15, 2007 DRT: OMAPS00120187 x0039928
	Description: MMI shows error if back button is pressed while playing audio files with N5.23 Pre release
	Solution: mfw_fm_audStop is commented in func mmi_fm_aud_list_cb for event
	LISTS_REASON_DRAWCOMPLETE which was being called twice in the scenario.


*! 10-Apr-2006 mf:  Modified x0020906.  Change required per SRxxxx
*! to provide File Manager Functionality.
*!
*!
*! 17-Jan-2006 mf: Revisions appear in reverse chronological order;
*! that is, newest first.  The date format is dd-Mon-yyyy.

	
* =========================================================== */
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

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_icn.h"	/* included for mfw_mnu.h */
#include "mfw_mnu.h"
#include "mfw_tim.h"
#include "mfw_kbd.h"
#include "mfw_sat.h" /* included for MmiDummy.h, included for MmiMenu.h */
#include "mfw_camapp.h"
#include "mfw_fm.h"
#include "MmiBlkLangDB.h"
#include "mmiColours.h"
#include "MmiDialogs.h"
#include "MmiDummy.h" /* included for MmiMenu.h */
#include "MmiLists.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "AUIEditor.h"
#include "MmiMain.h"
#include "Mmiicons.h"
#include "MmiWindow.h"
#include "MmiCall.h"

#include "mfw_ffs.h"

#include "mmifilemanager.h"
#include "MmiBookShared.h"



/* OMAPS00151698, x0056422 */
#ifdef FF_MMI_A2DP_AVRCP
#include "mfw_bt_api.h"
#include "mfw_bt_private.h"
#endif



/*******************************************************************************

                              Defines

*******************************************************************************/
T_MMI_FM_STRUCT *fm_data;
GLOBAL BOOL mslPrimHandler (USHORT opc, void * data);
extern  MfwHnd winIconsLists;

const unsigned char file[]={
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
};
const unsigned char folder[]=
{
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,
0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,
0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,
0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,
0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,
0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,
0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,
0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,
0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,
0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,
0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
};	
const unsigned char audiofile[]=
{
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,
0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,
0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x00,0x00,0x00,0x25,
0x25,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x00,0x00,0x00,0x25,0x25,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x00,0x00,
0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x00,0x00,0x25,0x25,0x00,0x00,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x00,0x00,0x25,0x00,0x00,
0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x25,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x00,0x00,0x00,
0x25,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x00,0x00,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,
0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x00,0x00,0x00,0x00,0x25,
};
MfwIcnAttr fmIconFile = 
{
    { 0, 0,48,48 }, 1, COLOUR_ICON_XX, ICON_TYPE_256_COL, (char *) file
};
MfwIcnAttr fmIconFolder  =  
{
    { 0, 0,48,48 }, 1, COLOUR_ICON_XX, ICON_TYPE_256_COL, (char *) folder
};
MfwIcnAttr fmIconAudioFile = 
{
    { 0, 0,48,48 }, 1, COLOUR_ICON_XX, ICON_TYPE_256_COL, (char *) audiofile
};

#define PB_OFF 0
#define PB_ON 1
BOOL mmi_progress_bar = PB_ON;
//Jun 26, 2007 DRT:OMAPS00135749  x0062174(Nimitha)
/* //May28,07 DRT:OMAPS00132459  x0061560(sateeshg)
BOOL aud_state_back=0;*/

//Jun 25,2007 DRT:OMAPS00135749  x0062174(Nimitha)
int aud_state_status = 1;

//Daisy tang added for Real Resume feature 20071107
//start
BOOL IsMP3Playing = FALSE;
extern BOOL IsRealPaused;
//end

extern UBYTE aud_state;
extern T_MFW_AUD_PARA para_aud;
extern T_MFW_HND pb_win;


/* OMAPS00151698, x0056422 */
#ifdef FF_MMI_A2DP_AVRCP
extern BMI_BT_STRUCTTYPE tGlobalBmiBtStruct;
#endif //FF_MMI_A2DP_AVRCP





/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- Start --> */

static unsigned char ucImgList_Sts 		= 0;
static unsigned char ucImgViewer_Sts 	= 0;

#define MMI_IMGLIST_STS					ucImgList_Sts
#define MMI_IMGVIEWER_STS				ucImgViewer_Sts

#define MMI_IMGLIST_STS_NORMAL			0x00
#define MMI_IMGLIST_STS_SUSPENDED		0x01

#define MMI_IMGVIEWER_STS_NORMAL		0x00
#define MMI_IMGVIEWER_STS_SUSPENDED		0x01

#define MMI_IMGLIST_CHANGE_STS(a)		( ucImgList_Sts = a )

#define MMI_IMGVIEWER_CHANGE_STS(a)		( ucImgViewer_Sts = a )

/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- End -->  */
/*******************************************************************************

                                Local prototypes

*******************************************************************************/
static T_MFW_HND mmi_fm_start( T_MFW_HND parent_win);
static T_MFW_HND mmi_fm_create (T_MFW_HND parent_win);
static int mmi_fm_app_root_cb(T_MFW_EVENT evnt, void *para);
int mmi_fm_win_cb (MfwEvt e, MfwWin *w);
void mmi_fm_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static void mmi_fm_destroy(MfwHnd own_window);

void mmi_fm_drives_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
void mmi_fm_root_folders_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);

void mmi_fm_list_create( T_MFW_HND win);
static void mmi_fm_list_destroy(MfwHnd own_window);

void mmi_fm_imgThmbGen_start(void);
void mmi_fm_imgThmbGen_resume(void);
void mmi_fm_imgThmbGen(void);
void mmi_fm_imgThmbGen_identifyFile(void);

void mmi_fm_imgThmbDraw_start(void);
void mmi_fm_imgThmbDraw_resume(void)	;
void mmi_fm_imgThmbDraw_identifyFile(void);
void mmi_fm_imgThmbDraw(void);

T_MFW_HND mmi_fm_img_list_create(T_MFW_HND win);
void mmi_fm_img_list_cb(T_MFW_HND * Parent, ListMenuData * ListData);
BOOL mmi_fm_img_validate(void);
void mmi_fm_imgDraw(void);

T_MFW_HND mmi_fm_aud_list_create(T_MFW_HND win);
void mmi_fm_aud_validate(void);
void mmi_fm_aud_list_cb(T_MFW_HND * Parent, ListMenuData * ListData);

void mmi_fm_view_start(void);
T_MFW_HND mmi_fm_view_create(void);
int mmi_fm_view_win_cb (MfwEvt e, MfwWin *w);
void mmi_fm_view_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int mmi_fm_view_kbd_cb (MfwEvt e, MfwKbd *k);
void mmi_fm_view_destroy(T_MMI_FM_IMGDRAW_STRUCT  * imgDraw_data);

T_MFW_HND mmi_fm_rename_editor_create(T_MFW_HND parent_win);
static void mmi_fm_rename_editor_exec_cb(T_MFW_HND win, USHORT identifier, SHORT reason);
void mmi_fm_rename_cb (T_MFW_HND win, USHORT identifier, SHORT reason);
void mmi_fm_rename_editor_destroy(void);

T_MFW_HND mmi_fm_newdir_editor_create(T_MFW_HND parent_win);
static void mmi_fm_newdir_editor_exec_cb(T_MFW_HND win, USHORT identifier, SHORT reason);
void mmi_fm_newdir_cb (T_MFW_HND win, USHORT identifier, SHORT reason);
void mmi_fm_newdir_editor_destroy(void);

void mmi_fm_remove_confirm_cb (T_MFW_HND win, USHORT identifier, SHORT reason);
void mmi_fm_remove_cb (T_MFW_HND win, USHORT identifier, SHORT reason);

T_MFW_HND mmi_fm_properties_editor_create(T_MFW_HND parent_win);
static void mmi_fm_properties_editor_exec_cb(T_MFW_HND win, USHORT identifier, SHORT reason);
void mmi_fm_properties_editor_destroy(void);
void mmi_fm_format_confirm_cb (T_MFW_HND win, USHORT identifier, SHORT reason);

T_MFW_HND mmi_fm_memStat_editor_create(T_MFW_HND parent_win);
static void mmi_fm_memStat_editor_exec_cb(T_MFW_HND win, USHORT identifier, SHORT reason);
void mmi_fm_memStat_editor_destroy(void);

void mmi_fm_nofile_cb (T_MFW_HND win, USHORT identifier, SHORT reason);
void mmi_fm_open (void);
void mmi_fm_goback(void);
T_MFW_HND mmi_fm_app_show_info(T_MFW_HND parent_win, int str1, int str2,int skey1, int skey2, T_VOID_FUNC callback,int timer, T_MFW_EVENT keyEvents);
void mmi_fm_get_filename( char * filename);
char * mmi_fm_get_ext( char * filename);
void mmi_fm_dest_root_folders_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static T_MFW_HND mmi_fm_dest_start( T_MFW_HND parent_win);
static T_MFW_HND mmi_fm_dest_create (T_MFW_HND parent_win);
int mmi_fm_dest_win_cb (MfwEvt e, MfwWin *w);
void mmi_fm_dest_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static void mmi_fm_dest_destroy(MfwHnd own_window);
T_MFW_HND mmi_fm_dest_list_create( T_MFW_HND win);
void mmi_fm_dest_list_cb(T_MFW_HND * Parent, ListMenuData * ListData);
void mmi_fm_destnofile_cb (T_MFW_HND win, USHORT identifier, SHORT reason);

static void mmi_fm_dest_list_destroy(MfwHnd own_window);
void mmi_fm_copy_create(char * dest_folder);
T_MFW_HND mmi_fm_copy_dialog_create(T_MFW_HND parent_win);
static int mmi_fm_copy_dialog_kbd_cb (MfwEvt e, MfwKbd *k);
int mmi_fm_copy_dialog_win_cb (MfwEvt e, MfwWin *w);
void mmi_fm_copy_dialog_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
void mmi_fm_copy_dialog_destroy();
void mmi_fm_copy_progress_cb (T_MFW_HND win, USHORT identifier, SHORT reason);
void mmi_fm_copy_start(void);
void mmi_fm_copy_end(void);
int mmi_fm_copy_timer_cb ( MfwEvt e, MfwTim *m );
void mmi_fm_copy_destroy(T_FM_COPY_STATE status);
void mmi_fm_displayDestDrive_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
void mmi_fm_copy_end_cb (T_MFW_HND win, USHORT identifier, SHORT reason);
void mmi_fm_displayDestDrive(void);
void mmi_fm_destgoback(void);
void mmi_fm_set_mmi_plane_to_front(void);
/* Jun 26, 200 07, 2007 DRT: OMAPS00135749 x0062174 */
GLOBAL void mmi_set_aud_state_status( T_FM_AUD_STATE status);
T_FM_AUD_STATE mmi_get_aud_state_status( );
/* January 29, 2008 DRT:OMAPS00156759 (x0082844) For Unicode Support*/
static void unicode_to_char_array(T_WCHAR *inp, char *out);

/*******************************************************************************

 $Function:  mmi_set_aud_state_status

 $Description:	 set audio state status

 $Returns:    none.

 $Arguments:

*******************************************************************************/
//Jun 26, 2007 DRT:OMAPS00135749  x0062174(Nimitha)

GLOBAL void mmi_set_aud_state_status( T_FM_AUD_STATE status)
{
	TRACE_FUNCTION("mmi_set_aud_status()");

	aud_state_status = status;	
	TRACE_EVENT_P1("aud_state in set_aud_status %d", aud_state_status);
	
}

/*******************************************************************************

 $Function:  mmi_get_aud_state_status

 $Description:	  get audio state status

 $Returns:    none.

 $Arguments:

*******************************************************************************/
//Jun 26, 2007 DRT:OMAPS00135749  x0062174(Nimitha)

T_FM_AUD_STATE mmi_get_aud_state_status( )
{
	TRACE_FUNCTION("mmi_get_aud_status()");
	TRACE_EVENT_P1("aud_state in get_aud_status %d", aud_state_status);

	return aud_state_status;	
	
}



/*******************************************************************************
 $Function:    		mmi_fm_app_show_info

 $Description:		information Dialog function

 $Returns:		Created Window Pointer

 $Arguments:		parent_win:	 	-Parent window 
				str1,  str2	-String IDs
				skey1, skey2	-Softkey lables
				callback	 	-Callback Function 
				timer		-Timeout value
				keyEvents	-Key events to be handled
*******************************************************************************/
T_MFW_HND mmi_fm_app_show_info(T_MFW_HND parent_win, int str1, int str2,int skey1, int skey2, T_VOID_FUNC callback,int timer, T_MFW_EVENT keyEvents)
{
	T_DISPLAY_DATA display_info;
	T_MFW_HND win;
	
	TRACE_FUNCTION ("mmi_fm_app_show_info()");
	
	dlg_initDisplayData_TextId( &display_info, skey1, skey2, str1, str2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, callback, timer, keyEvents);
	win = info_dialog(parent_win, &display_info);
	return win;
}


/*******************************************************************************
 $Function:    		mmi_fm_drives

 $Description:		Display the drive options open , memory status, format 

 $Returns:		Execution status

 $Arguments:		mnu		-Pointer to current menu
 				item 	-Pointer to current menu item
*******************************************************************************/
GLOBAL int  mmi_fm_drives(MfwMnu* mnu, MfwMnuItem* item)
{
	T_MFW_HND       parent_win;
	T_MFW_HND       win; 

	TRACE_FUNCTION("mmi_fm_drives" );

	fm_data = (T_MMI_FM_STRUCT *)mfwAlloc(sizeof(T_MMI_FM_STRUCT));
	parent_win  = mfwParent(mfw_header());
	fm_data->source = (T_FM_DEVICE_TYPE)mnu->lCursor[mnu->level];
	TRACE_EVENT_P1("Selected Drive  %d",fm_data->source);
	/* Apr 06, 2007    DRT: OMAPS00124877  x0039928 */
	/* Fix: Default settings for audio player */
	mfw_player_init();
	win=(T_MFW_HND)bookMenuStart( parent_win, FMDrivesOptionsListAttributes(), 0);
	SEND_EVENT(win,ADD_CALLBACK,NULL,(void *)mmi_fm_drives_cb);

	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    		mmi_fm_drives_cb

 $Description:		Callback function for the drive option window 

 $Returns:		Execution status

 $Arguments:		win			-current window
				event		-window event Id
				value		-unique Id
				parameter	-optional data.
*******************************************************************************/
void mmi_fm_drives_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	TRACE_FUNCTION("mmi_fm_drives_cb");
	
	mfwFree((U8 *)fm_data, sizeof (T_MMI_FM_STRUCT));
	fm_data = NULL;	
}

/*******************************************************************************
 $Function:    		mmi_fm_root_folders

 $Description:		Display the root Image, Audio Directories

 $Returns:		Execution status

 $Arguments:		mnu		-Pointer to current menu
 				item 	-Pointer to current menu item
*******************************************************************************/
GLOBAL int  mmi_fm_root_folders(MfwMnu* mnu, MfwMnuItem* item)
{
	T_MFW_HND       parent_win;
	T_MFW_HND       win;    
	
	TRACE_FUNCTION("mmi_fm_root_folders" );
	
	parent_win  = mfwParent(mfw_header());
	fm_data->cur_dir = (T_FM_CURDIR *)mfwAlloc(sizeof(T_FM_CURDIR));
	win = (T_MFW_HND)bookMenuStart( parent_win, FMRootFolderAttributes(), 0);
	SEND_EVENT(win,ADD_CALLBACK,NULL,(void *)mmi_fm_root_folders_cb);
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
 $Function:    		mmi_fm_root_event_cb

 $Description:		Callback function for the root Image, Audio Directory window

 $Returns:		None

 $Arguments:		win			-current window
				event		-window event Id
				value		-unique Id
				parameter	-optional data.
*******************************************************************************/
void mmi_fm_root_folders_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	TRACE_FUNCTION("mmi_fm_root_event_cb");
	
	mfwFree((U8 *)fm_data->cur_dir , sizeof (T_FM_CURDIR));
	fm_data->cur_dir = NULL;
}
/*******************************************************************************
 $Function:    		mmi_fm_display

 $Description:		Displays the files and folders given path

 $Returns:		Execution status

 $Arguments:		mnu		-Pointer to current menu
 				item 	-Pointer to current menu item
*******************************************************************************/
GLOBAL int  mmi_fm_display(MfwMnu* mnu, MfwMnuItem* item)
{
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	T_MFW_HND parent_win = mfwParent(mfw_header());
	
	TRACE_FUNCTION("mmi_fm_display");
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state11 %d",cur_dir->dirinfo.aud.state);

	cur_dir->depth = 0;
	cur_dir->dir_type = OBJECT_TYPE_NONE;
	cur_dir->currentIndex = 0;
	cur_dir->num_objs = 0;
	cur_dir->startIndex = 0;
	switch(mnu->lCursor[mnu->level])
	{
		case 0:
			fm_data->app = FM_IMAGE;
			cur_dir->dir_type=OBJECT_TYPE_IMAGE_FOLDER;
			break;
		case 1:
			fm_data->app = FM_AUDIO;
			cur_dir->dir_type=OBJECT_TYPE_AUDIO_FOLDER;
			break;
	}
	//Based on the selected drive, load the root image, audio directory names
	switch(fm_data->source)
	{
		case FM_NOR_FLASH:
			if(cur_dir->dir_type==OBJECT_TYPE_IMAGE_FOLDER)
			{
				strcpy( cur_dir->dir_path,  NORDIR);
				strcpy( cur_dir->dir_name,  NORDIR_IMG);
			}
			else if(cur_dir->dir_type==OBJECT_TYPE_AUDIO_FOLDER)
			{
				strcpy( cur_dir->dir_path,  NORDIR);
				strcpy( cur_dir->dir_name,  NORDIR_AUD);
			}
			break;
		case FM_NORMS_FLASH:
			if(cur_dir->dir_type==OBJECT_TYPE_IMAGE_FOLDER)
			{
				strcpy( cur_dir->dir_path,  NORMSDIR);
				strcpy( cur_dir->dir_name,  NORMSDIR_IMG);
			}
			else if(cur_dir->dir_type==OBJECT_TYPE_AUDIO_FOLDER)
			{
				strcpy( cur_dir->dir_path,  NORMSDIR);
				strcpy( cur_dir->dir_name,  NORMSDIR_AUD);
			}
			break;			
		case FM_NAND_FLASH:
			if(cur_dir->dir_type==OBJECT_TYPE_IMAGE_FOLDER)
			{
				strcpy( cur_dir->dir_path,  NANDDIR);
				strcpy( cur_dir->dir_name,  NANDDIR_IMG);
			}
			else if(cur_dir->dir_type==OBJECT_TYPE_AUDIO_FOLDER)
			{
				strcpy( cur_dir->dir_path,  NANDDIR);
				strcpy( cur_dir->dir_name,  NANDDIR_AUD);
			}
			break;
		case FM_T_FLASH:
			if(cur_dir->dir_type==OBJECT_TYPE_IMAGE_FOLDER)
			{
				strcpy( cur_dir->dir_path,  TFLASHDIR);
				strcpy( cur_dir->dir_name,  TFLASHDIR_IMG);
			}
			else if(cur_dir->dir_type==OBJECT_TYPE_AUDIO_FOLDER)
			{
				strcpy( cur_dir->dir_path,  TFLASHDIR);
				strcpy( cur_dir->dir_name,  TFLASHDIR_AUD);
			}
			break;
	}	
	TRACE_EVENT_P2("Path %s Dir %s", cur_dir->dir_path,cur_dir->dir_name);
	cur_dir->depth = 1;
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state12 %d",cur_dir->dirinfo.aud.state);
	mmi_fm_start( parent_win);
	return MFW_EVENT_CONSUMED;
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state13 %d",cur_dir->dirinfo.aud.state);
}

/*******************************************************************************
 $Function:    		mmi_fm_start

 $Description:		To create the FileManager window

 $Returns:		Created Window Pointer

 $Arguments:		parent_win	-Parent Window
*******************************************************************************/
static T_MFW_HND mmi_fm_start( T_MFW_HND parent_win)
{
	TRACE_FUNCTION("mmi_fm_list_Start");
	
	//Checks and creates the root directories for image and audio
	mfw_fm_createRootDir(fm_data->source);
	
	//Creating the root window for FileManager
	fm_data->root_win=mmi_fm_create(parent_win);
	
	if (fm_data->root_win NEQ NULL)
	{
		fm_data->opt_type=FM_NONE;
	    SEND_EVENT (fm_data->root_win, FM_INIT, 0, (void *)NULL);
	}
	return fm_data->root_win;
}

/*******************************************************************************
 $Function:    		mmi_fm_create

 $Description:		To create the FileManager window

 $Returns:		Created Window Pointer

 $Arguments:		parent_win	-Parent Window
*******************************************************************************/
static T_MFW_HND mmi_fm_create (T_MFW_HND parent_win)
{

	T_MFW_WIN  * win;
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;

	TRACE_FUNCTION("mmi_fm_create");
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state31 %d",cur_dir->dirinfo.aud.state);
	
	//Creating the root window for FileManager
	fm_data->root_win = win_create (parent_win, 0, E_WIN_VISIBLE, (T_MFW_CB)mmi_fm_win_cb);
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state32 %d",cur_dir->dirinfo.aud.state);
	TRACE_EVENT_P1("fm_data->root_win %x",fm_data->root_win);
	if (fm_data->root_win EQ NULL)
	{
		mfwFree((U8 *)fm_data, (U16)sizeof(T_MMI_FM_STRUCT));
		fm_data=NULL;
		return NULL;
	}
	/*
		connect the dialog data to the MFW-window
	*/
	fm_data->mmi_control.dialog = (T_DIALOG_FUNC)mmi_fm_exec_cb;
	fm_data->mmi_control.data   = fm_data;
	fm_data->parent_win= parent_win;	
	win                           = ((T_MFW_HDR *)fm_data->root_win)->data;
	win->user                = (void *)fm_data;
	if(fm_data->mfw_win == NULL)
	{
		//Creating the mfw component for the root window for FileManager
		fm_data->mfw_win = (T_MFW_HND)mfw_fm_create(fm_data->root_win, E_FM_ALL_SERVICES, (MfwCb)mmi_fm_app_root_cb);
	}
	if (fm_data->mfw_win== NULL)
	{
		winDelete(fm_data->root_win);
		mfwFree((U8 *)fm_data, (U16)sizeof(T_MMI_FM_STRUCT));
		fm_data=NULL;
		return NULL;
	}
	
	fm_data->list_win=NULL;
	cur_dir->startIndex=0;
	winShow(fm_data->root_win);
	return fm_data->root_win;
}

/*******************************************************************************
 $Function:    		mmi_fm_app_root_cb

 $Description:		Handler for all the events from mfw for FileManager

 $Returns:		Execution status

 $Arguments:		event	-window event Id
				para		-optional data.
*******************************************************************************/
static int mmi_fm_app_root_cb(T_MFW_EVENT event, void *para)
{
	T_MFW_HND win = mfw_parent(mfw_header());
	T_MFW_WIN * win_data;
	T_MMI_FM_STRUCT *fm_data;
	T_FM_CURDIR *cur_dir ;
	T_MFW_FM_PARA *fm_para=(T_MFW_FM_PARA*) para;
	T_FM_IMG_STATE result = FM_UCP_NO_ERROR;
	
    	TRACE_FUNCTION ("mmi_fm_app_root_cb()");

	win_data = ((T_MFW_HDR *) win)->data;
	fm_data = (T_MMI_FM_STRUCT *)win_data->user;
	cur_dir = fm_data->cur_dir;

	TRACE_EVENT_P2(" Event %x, state %d",event,fm_para->img_state); 
	switch (event)
	{
		case E_FM_THMB_INIT:
			if(fm_para->img_state == FM_UCP_NO_ERROR)
			{
				//Initialization successful. Start generating thumbnails
				cur_dir->dirinfo.img.cmd = FM_UCP_THMB_GEN;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgThmb_generate();
				if(result != FM_UCP_NO_ERROR)
				{
					//Error Generating thumbnails
					//Deinitialize the Thumbnail UCP and destroy it
					TRACE_ERROR("Error generating thumbnails");
					cur_dir->dirinfo.img.cmd=FM_UCP_THMB_DEINIT;
					cur_dir->dirinfo.img.state=FM_UCP_ERROR;
					result = mfw_fm_imgThmb_deinit();
					if(result != FM_UCP_NO_ERROR)
					{
						TRACE_ERROR("Error deinitializing the thumbnail UCP");
						cur_dir->dirinfo.img.cmd=FM_UCP_NONE;
						cur_dir->dirinfo.img.state=FM_UCP_NONE;
						result = mfw_fm_imgThmb_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the thumbnail UCP");
					}
				}
			}
			else
			{
				//Error in initialization. Destroy the Thumbnail UCP 
				//Create the Viewer UCP
				result = mfw_fm_imgThmb_destroy();
				if(result != FM_UCP_NO_ERROR)
					TRACE_ERROR("Error destroying the thumbnail UCP");
				cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				cur_dir->currentIndex = cur_dir->startIndex;
				mmi_fm_img_validate();
				mmi_fm_imgThmbDraw_start();
			}
			break;
		case E_FM_THMB_GEN:
			switch(fm_para->img_state)
			{
				case FM_UCP_NO_ERROR:
					//Thumbanail generation successful. Continue generating thumbnails
					cur_dir->dirinfo.img.state = FM_UCP_NO_ERROR;
					mmi_fm_imgThmbGen();
					break;
				case FM_UCP_ERROR_BAD_STREAM:
				case FM_UCP_ERROR_UNKNOWN:
					//NonFatal Error in generation. Deinitialize and Initialize the Thumbnail UCP 
					cur_dir->dirinfo.img.cmd = FM_UCP_THMB_DEINIT;
					cur_dir->dirinfo.img.state = FM_UCP_ERROR_NONFATAL;
					result = mfw_fm_imgThmb_deinit();
					if(result != FM_UCP_NO_ERROR)
					{
						TRACE_ERROR("Error deinitializing the thumbnail UCP");
						cur_dir->dirinfo.img.cmd=FM_UCP_NONE;
						cur_dir->dirinfo.img.state=FM_UCP_NONE;
						result = mfw_fm_imgThmb_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the thumbnail UCP");
					}
					break;
				case FM_UCP_ERROR_NO_MEM:
				case FM_UCP_ERROR_INVALID_ARGUMENT:
				case FM_UCP_ERROR:
					//Fatal Error in generation. Deinitialize and Destroy the Thumbnail UCP 
					cur_dir->dirinfo.img.cmd = FM_UCP_THMB_DEINIT;
					cur_dir->dirinfo.img.state = FM_UCP_ERROR_FATAL;
					result = mfw_fm_imgThmb_deinit();
					if(result != FM_UCP_NO_ERROR)
					{
						TRACE_ERROR("Error deinitializing the thumbnail UCP");
						cur_dir->dirinfo.img.cmd=FM_UCP_NONE;
						cur_dir->dirinfo.img.state=FM_UCP_NONE;
						result = mfw_fm_imgThmb_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the thumbnail UCP");
					}
					break;
			}
			break;
		case E_FM_THMB_DEINIT:
			if(fm_para->img_state == FM_UCP_NO_ERROR)
			{
				switch(cur_dir->dirinfo.img.state)
				{
					case FM_UCP_NONE:
						//Had completed thumbnail generation successfully
						//Thumbnail UCP deinitialized, Destroy the Thumbnail UCP.
						//Create the Viewer UCP for drawing the thumbnails
						result = mfw_fm_imgThmb_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the thumbnail UCP");
						cur_dir->dirinfo.img.cmd=FM_UCP_NONE;
						cur_dir->dirinfo.img.state=FM_UCP_NONE;
						cur_dir->currentIndex=cur_dir->startIndex;
						mmi_fm_img_validate();
						mmi_fm_imgThmbDraw_start();
						break;
					case FM_UCP_ERROR_FATAL:
						//Had Encountered a fatal error. 
						//Thumbnail UCP deinitialized, Destroy the Thumbnail UCP.
						//Create the Viewer UCP for drawing the thumbnails
						result = mfw_fm_imgThmb_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the thumbnail UCP");
						cur_dir->dirinfo.img.cmd=FM_UCP_NONE;
						cur_dir->dirinfo.img.state=FM_UCP_NONE;
						cur_dir->currentIndex=cur_dir->startIndex;
						mmi_fm_img_validate();
						mmi_fm_imgThmbDraw_start();
						break;
			/*May 11, 2007 DRT:OMAPS00132505 x0061088*/	
			        case FM_UCP_ERROR:
					case FM_UCP_ERROR_NONFATAL:
						//Had Encountered a nonfatal error. Thumbnail UCP was deinitialized						
						//initialize again to continue with the next thumbnail generation
						mmi_fm_imgThmbGen_resume();
						break;
				}
			}
			else
			{
				//Error in deinitializing the Thumbnail UCP. Destroy the Thumbnail UCP.
				cur_dir->dirinfo.img.cmd=FM_UCP_NONE;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgThmb_destroy();
				if(result != FM_UCP_NO_ERROR)
					TRACE_ERROR("Error destroying the thumbnail UCP");
			}
			break;
		case E_FM_IMG_INIT:
			if(fm_para->img_state == FM_UCP_NO_ERROR)
			{
				//Viewer UCP is successfully initialized. 
				if(cur_dir->dirinfo.img.cmd == FM_UCP_THMBIMG_INIT)
				{
					//Draw the 48 X 48 thumbnail
					cur_dir->dirinfo.img.cmd = FM_UCP_THMBIMG_DRAW;
					cur_dir->dirinfo.img.state = FM_UCP_NONE;
				}
				else if (cur_dir->dirinfo.img.cmd == FM_UCP_QCIFIMG_INIT)
				{
					//Draw the QCIF Width X QCIF Height image
					cur_dir->dirinfo.img.cmd = FM_UCP_QCIFIMG_DRAW;
					cur_dir->dirinfo.img.state = FM_UCP_NONE;
				}
				result = mfw_fm_imgViewer_view();
				if(result != FM_UCP_NO_ERROR)
				{
					//Error drawing thumbnail in the Viewer UCP, deinit and destroy it
					TRACE_ERROR("Error drawing thumbnail in the Viewer UCP");
					if( cur_dir->dirinfo.img.cmd == FM_UCP_THMBIMG_DRAW)
						cur_dir->dirinfo.img.cmd = FM_UCP_THMBIMG_DEINIT;
					else
						cur_dir->dirinfo.img.cmd = FM_UCP_QCIFIMG_DEINIT;
					cur_dir->dirinfo.img.state = FM_UCP_ERROR;
					result = mfw_fm_imgViewer_deinit();
					if(result != FM_UCP_NO_ERROR)
					{
						//Error deinitializing the Viewer UCP, destroy it
						TRACE_ERROR("Error deinitializing the Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
				}
			}
			else
			{
				//Error initializing the Viewer UCP. Destroy it
				//Unlock the keys
				if(cur_dir->dirinfo.img.cmd == FM_UCP_THMBIMG_INIT)
					displayMenuKeys(MENU_KEY_UP_DOWN_CENTER);
				else
					displayMenuKeys(MENU_KEY_ALL);
				fm_data->menu_list_data->block_keys =  (BOOL)FALSE;
				result = mfw_fm_imgViewer_destroy();
				if(result != FM_UCP_NO_ERROR)
					TRACE_ERROR("Error destroying the Viewer UCP");
				cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
			}
			break;
		case E_FM_IMG_DRAW:
			switch(fm_para->img_state )
			{
				case FM_UCP_NO_ERROR:
					cur_dir->dirinfo.img.state = FM_UCP_NO_ERROR;
					if(cur_dir->dirinfo.img.cmd == FM_UCP_THMBIMG_DRAW)
					{
						//Drawing the 48 X 48 thumbnail was successful. Conitnue to the next one
						cur_dir->currentIndex++;
						mmi_fm_imgThmbDraw();
					}
					else if(cur_dir->dirinfo.img.cmd == FM_UCP_QCIFIMG_DRAW)
					{
						//Drawing the QCIF Width X QCIF Height image was successful. Unlock the keys now.
						displayMenuKeys(MENU_KEY_ALL);
					}
					break;
				case FM_UCP_ERROR_UNKNOWN:
				case FM_UCP_ERROR_BAD_STREAM:
				case FM_UCP_ERROR_READ_FAILED:
					if(cur_dir->dirinfo.img.cmd == FM_UCP_THMBIMG_DRAW)
					{
						//Error drawing the thumbnail image. Deinitialize the Viewer UCP, Initialize again to draw the next thumbnail.
						cur_dir->dirinfo.img.cmd=FM_UCP_THMBIMG_DEINIT;
						cur_dir->dirinfo.img.state = FM_UCP_ERROR_NONFATAL;
					}
					else if(cur_dir->dirinfo.img.cmd == FM_UCP_QCIFIMG_DRAW)
					{
						//Error drawing theQCIF  image. Deinitialize the Viewer UCP and destroy it
						cur_dir->dirinfo.img.cmd=FM_UCP_QCIFIMG_DEINIT;
						cur_dir->dirinfo.img.state = FM_UCP_ERROR_FATAL;
					}
					result = mfw_fm_imgViewer_deinit();				
					if(result != FM_UCP_NO_ERROR)
					{
						//Error deinitializing the Viewer UCP, destroy it
						TRACE_ERROR("Error deinitializing the Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
					break;
				case FM_UCP_ERROR_INVALID_ARGUMENT:
				case FM_UCP_ERROR :
					//Error drawing the image. Deinitialize the Viewer UCP, unblock the keys and destroy it.
					//48 X 48 Thumbnail drawing
					if(cur_dir->dirinfo.img.cmd == FM_UCP_THMBIMG_DRAW)
					{
						cur_dir->dirinfo.img.cmd=FM_UCP_THMBIMG_DEINIT;
						displayMenuKeys(MENU_KEY_UP_DOWN_CENTER);
						fm_data->menu_list_data->block_keys =  (BOOL)FALSE;
					}
					//QCIF Image Drawing 
					else if(cur_dir->dirinfo.img.cmd == FM_UCP_QCIFIMG_DRAW)
					{
						cur_dir->dirinfo.img.cmd=FM_UCP_QCIFIMG_DEINIT;
						displayMenuKeys(MENU_KEY_ALL);
					}
					cur_dir->dirinfo.img.state = FM_UCP_ERROR_FATAL;
					result = mfw_fm_imgViewer_deinit();				
					if(result != FM_UCP_NO_ERROR)
					{
						//Error deinitializing the Viewer UCP, destroy it
						TRACE_ERROR("Error deinitializing the Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
					break;
				}
			break;
		case E_FM_IMG_DEINIT:
			switch(cur_dir->dirinfo.img.state)
			{
				case FM_UCP_NONE:
					//Viewer UCP is deinitialized, destroy it
					result = mfw_fm_imgViewer_destroy();
					break;
				case FM_UCP_ERROR_NONFATAL:
					//Viewer UCP is deinitialized, initialize and continue with next one
					mmi_fm_imgThmbDraw_resume();
					break;
				case FM_UCP_ERROR_FATAL:
					//Viewer UCP is deinitialized, destroy it
					result = mfw_fm_imgViewer_destroy();
					break;
			}
			if(result != FM_UCP_NO_ERROR)
				TRACE_ERROR("Error destroying the Viewer UCP");
			cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
			cur_dir->dirinfo.img.state = FM_UCP_NONE;
			switch(fm_data->opt_type)
			{
				case FM_OPEN:
					mmi_fm_open();
					break;
				case FM_BACK:
					mmi_fm_goback();
					break;
			}
 			break;
		case E_FM_AUDIO_PROG:
			pbar_update(para_aud.aud_pt, para_aud.aud_tt);
			break;
		case E_FM_AUDIO_STOP:
			if(fm_para->aud_state != FM_AUD_NONE)
			{
				USHORT       TextId = TxtAudNotAllowed;
				//Jul 03,07 DRT:OMAPS00135749  x0062174(Nimitha)
				//May28,07 DRT:OMAPS00132459  x0061560(sateeshg)
				/*if(aud_state_back )
				{
					aud_state_back =0;
				}*/
				switch(fm_para->aud_state)
				{
					case FM_AUD_ERROR_NOTALLOWED:
							TextId = TxtAudNotAllowed;
						break;
					case FM_AUD_ERROR_AUDIO:
						TextId = TxtAudAudioError;
						break;
					case FM_AUD_ERROR_DENIED:
						TextId = TxtAudDenied;
						break;
					case FM_AUD_ERROR_PLAYER:
						TextId = TxtAudPlayerError;
						break;
					case FM_AUD_ERROR_FFS:
						TextId = TxtAudFFSError;
						break;
					case FM_AUD_ERROR_MSGING:
						TextId = TxtAudMessagingError;
						break;
					case FM_AUD_ERROR_NOTREADY:
						TextId = TxtAudNotReady;
						break;
					case FM_AUD_ERROR_INVALIDPARAM:
						TextId = TxtAudInvalidParam;
						break;
					case FM_AUD_ERROR_INTERNAL:
						TextId = TxtAudInternalError;
						break;
					case FM_AUD_ERROR_MEM:
						TextId = TxtAudMemoryError;
						break;
					case FM_AUD_ERROR_NOTSUPPORTED:
						TextId = TxtAudNotSupported;
						break;
				}
				mmi_fm_app_show_info(win,TxtError,TextId,TxtNull,TxtSoftBack,0,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
			}
			/*a0393213(R.Prabakar) OMAPS00128976 - No return to File Management screen when a corrupted file is played
			   The window was not getting destroyed in case of error. Now it's destroyed.*/
			if(!IsRealPaused)//Daisy tang added for Real Resume feature 20071107
			{
				if(mmi_progress_bar == PB_ON)
				{
					winDelete(pb_win);
					pb_win = 0;
				}
/* Mar 15, 2007 DRT: OMAPS00120187 x0039928 */
/* Fix: Proper soft keys are displayed                  */
				if(fm_data->opt_win)
					displaySoftKeys(TxtSelect, TxtSoftBack);
				else
					displayMenuKeys(MENU_KEY_UP_DOWN_PLAY);
			}	
			cur_dir->dirinfo.aud.state = FM_AUD_NONE;
			aud_state = 1;
			break;
	}
	return MFW_EVENT_CONSUMED;
}



/*******************************************************************************
 $Function:    		mmi_fm_win_cb

 $Description:	 	Handler for window events for the FileManager window

 $Returns:		Execution status

 $Arguments:		e	-window event Id
				w	-current window
*******************************************************************************/
int mmi_fm_win_cb (MfwEvt e, MfwWin *w)
{
	TRACE_FUNCTION ("mmi_fm_win_cb()");
//	TRACE_EVENT_P1("cur_dir->dirinfo.aud.state6 %d",cur_dir->dirinfo.aud.state);
	
	switch (e)
	{
	case MfwWinVisible:  
		break;
	case MfwWinFocussed: 
		break;
	case MfwWinDelete:   
		break;
	default:
		return MFW_EVENT_REJECTED;
	}
	return MFW_EVENT_CONSUMED;
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state7 %d",cur_dir->dirinfo.aud.state);
}

/*******************************************************************************
 $Function:    		mmi_fm_exec_cb

 $Description:	 	Handler for events for the FileManager window

 $Returns:		None

 $Arguments:		win			-current window
				event		-window event Id
				value		-unique Id
				parameter	-optional data.
*******************************************************************************/
void mmi_fm_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN        * win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_FM_STRUCT  * fm_data     = (T_MMI_FM_STRUCT *)win_data->user;
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;

	TRACE_FUNCTION ("mmi_fm_exec_cb()");
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state9 %d",cur_dir->dirinfo.aud.state);
	
	TRACE_EVENT_P1("Event %d",event );
	switch (event)
	{
		case FM_INIT:
			mmi_fm_list_create(fm_data->root_win);
			break;
		case FM_DESTROY:
			mmi_fm_destroy(fm_data->root_win);
			break;
		case LISTS_REASON_RESUME:
			/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- Start --> */

			MMI_IMGLIST_CHANGE_STS(MMI_IMGLIST_STS_NORMAL);

			/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- End --> */
			if(cur_dir->dir_type == OBJECT_TYPE_IMAGE_FOLDER)
			{
				displaySoftKeys(fm_data->menu_list_data->LeftSoftKey,fm_data->menu_list_data->RightSoftKey);
				mmi_fm_set_mmi_plane_to_front();
			}
			break;
		case LISTS_REASON_SUSPEND:
			/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- Start --> */

			MMI_IMGLIST_CHANGE_STS(MMI_IMGLIST_STS_SUSPENDED);

			/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- End --> */
			if(cur_dir->dir_type == OBJECT_TYPE_IMAGE_FOLDER)
			{
				mmi_fm_set_mmi_plane_to_front();
			}
			break;
		default:
			break;
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_destroy

 $Description:	 	To destroy the FileManager window

 $Returns:		None

 $Arguments:		own_window	-FileManager window
*******************************************************************************/
static void mmi_fm_destroy(MfwHnd own_window)
{
	TRACE_FUNCTION ("mmi_fm_destroy()");
//	TRACE_EVENT_P1("cur_dir->dirinfo.aud.state10 %d",cur_dir->dirinfo.aud.state);

	if (own_window)
	{
		if ( fm_data )
		{
			//Delete the mfw component of the FileManager
			if ( fm_data->mfw_win!=NULL)
			{
				mfw_fm_delete(fm_data->mfw_win);
				fm_data->mfw_win = NULL;
			}
			//Delete the FileManager window
			if ( fm_data->root_win!=NULL)
			{
				win_delete (fm_data->root_win);
				fm_data->root_win = NULL;
			}
		}
	}	
	dspl_Enable(1);
}

/*******************************************************************************
 $Function:    		mmi_fm_list_create

 $Description:	 	Start building the list

 $Returns:		None
 $Arguments:		win		-Parent window
*******************************************************************************/
void mmi_fm_list_create( T_MFW_HND win)
{
	T_MFW_FM_STATUS status;
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	T_FM_IMG_STATE result = FM_UCP_NO_ERROR;
	
	TRACE_FUNCTION("mmi_fm_list_create" );
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state34 %d",cur_dir->dirinfo.aud.state);

	//Read the objects in the given directory 
	status = mfw_fm_readDir(cur_dir->dir_path,cur_dir->dir_name,&cur_dir->num_objs,cur_dir->obj_list,fm_data->source,fm_data->app);
	if( status == FM_DIR_DOESNOTEXISTS)  
	{
		//Root directory does not exists, display dialog to user and exit FileManager application
		TRACE_EVENT("Directory does not exists");
		mmi_fm_app_show_info(win,TxtFileNotLoaded,TxtNull,TxtNull,TxtSoftBack,0,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
		mmi_fm_list_destroy(fm_data->root_win);
		SEND_EVENT(fm_data->root_win,FM_DESTROY,0,NULL);
		return;
	}
	else if( cur_dir->num_objs == 0 )
	{
		//Directory is empty. Display dialog ot teh user and return to previous directory /exit FileManager application
		TRACE_EVENT("Empty Directory");
		/* Mar 30, 2007 DRT: OMAPS00122768 x0039928 */
		/* Fix: image viewer is deinitialized */
		if( (cur_dir->dirinfo.img.cmd == FM_UCP_QCIFIMG_DRAW || cur_dir->dirinfo.img.cmd == FM_UCP_THMBIMG_DRAW) &&
		      cur_dir->dirinfo.img.state == FM_UCP_NO_ERROR) 
			{
				cur_dir->dirinfo.img.cmd = FM_UCP_THMBIMG_DEINIT;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgViewer_deinit();
				if(result != FM_UCP_NO_ERROR)
				{
					//Error deinitializing the Viewer UCP, destroy it
					TRACE_ERROR("Error deinitializing the Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
					cur_dir->dirinfo.img.state = FM_UCP_NONE;
					result = mfw_fm_imgViewer_destroy();
					if(result != FM_UCP_NO_ERROR)
						TRACE_ERROR("Error destroying the Viewer UCP");
					}
				}
		mmi_fm_app_show_info(win,TxtFileNotLoaded,TxtNull,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_nofile_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
		return;  
	}
	//Read the properties for all the files
	mfw_fm_readProperties(cur_dir->dir_path,cur_dir->dir_name,cur_dir->num_objs,cur_dir->obj_list,cur_dir->obj_properties,fm_data->source, fm_data->app);
	fm_data->menu_list_data = (ListMenuData *)ALLOC_MEMORY(sizeof(ListMenuData));
	if(fm_data->menu_list_data == NULL )
	{
		TRACE_EVENT("Memory failure.");
		return;
	}
	fm_data->menu_list_data->List = (T_MFW_MNU_ITEM *)ALLOC_MEMORY( (cur_dir->num_objs) * sizeof(T_MFW_MNU_ITEM) );
	if(fm_data->menu_list_data->List == NULL)
	{
		TRACE_EVENT("Memory failure.");
		return;
	}
	//Based on the directory type, build the corresponding list
	switch(cur_dir->dir_type)
	{
		case OBJECT_TYPE_AUDIO_FOLDER:
			fm_data->list_win=mmi_fm_aud_list_create(win);
			break;
		case OBJECT_TYPE_IMAGE_FOLDER:
			fm_data->list_win=mmi_fm_img_list_create(win);
			break;
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_list_destroy

 $Description:	 	Frees the memory allocated during list creation

 $Returns:		None

 $Arguments:		own_window	-FileManager window
*******************************************************************************/
static void mmi_fm_list_destroy(MfwHnd own_window)
{
	T_MFW_WIN * win_data;
	T_MMI_FM_STRUCT *fm_data;
	int i=0;
	T_FM_CURDIR *cur_dir;
	
	TRACE_FUNCTION ("mmi_fm_list_destroy()");
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state8 %d",cur_dir->dirinfo.aud.state);

	/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- Start --> */
	
	MMI_IMGLIST_CHANGE_STS(MMI_IMGLIST_STS_NORMAL);
	
	/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- End --> */
	if (own_window)
	{
		win_data = ((T_MFW_HDR *)own_window)->data;
		fm_data = (T_MMI_FM_STRUCT *)win_data->user;
		cur_dir = fm_data->cur_dir;
		if ( fm_data )
		{
			if (fm_data->menu_list_data != NULL)
			{
			#ifdef FF_MMI_UNICODE_SUPPORT
				if (fm_data->app == FM_AUDIO)
				{
				for (i=0; i<cur_dir->num_objs; i++)
				{
						mfwFree(fm_data->menu_list_data->List[i].str, (wstrlen(cur_dir->obj_list[i]->name_uc)+1)*2);
					}
				}
			#endif
				mfwFree( (U8 *)fm_data->menu_list_data->List,(cur_dir->num_objs) * sizeof(T_MFW_MNU_ITEM) );
				mfwFree ((U8 *)fm_data->menu_list_data, sizeof(ListMenuData));
			}
			i=0;
			while(cur_dir->obj_list[i] != NULL)
			{
				mfwFree((U8 *)cur_dir->obj_list[i], sizeof(T_FM_OBJ));
				cur_dir->obj_list[i] = NULL;
				mfwFree((U8 *)cur_dir->obj_properties[i], sizeof(T_FM_OBJ_PROPERTIES));
				cur_dir->obj_properties[i] = NULL;
				mfwFree((U8 *)cur_dir->obj_data[i], sizeof(T_FM_OBJ_DATA));
				cur_dir->obj_data[i] = NULL;
				i++;
			}
			
		}
	}

}

/*******************************************************************************
 $Function:    		mmi_fm_img_validate

 $Description:	 	Validates the files in the image folder. Checks for thumbnail and updates 
 				the flag for each object in the given image folder

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
BOOL mmi_fm_img_validate(void)
{
	USHORT  count = 0;
	char  curFilename[FM_MAX_OBJ_NAME_LENGTH];
	char  *ext;
	T_MFW_FM_STATUS status;
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	BOOL thumbanil_generation= FALSE;
	
	TRACE_FUNCTION ("mmi_fm_img_validate()");
	for (count=0; count<cur_dir->num_objs; count++)
	{
		strcpy(curFilename,cur_dir->obj_list[count]->name);
		cur_dir->obj_data[count] = (T_FM_OBJ_DATA*)mfwAlloc(sizeof(T_FM_OBJ_DATA));
		if(cur_dir->obj_list[count]->type == OBJECT_TYPE_FILE || cur_dir->obj_list[count]->type == OBJECT_TYPE_IMAGE)
		{
			ext=(char *)mmi_fm_get_ext(curFilename);
			if(strcmp((const char*)ext, "jpg") == 0 ) 
			{
				cur_dir->obj_list[count]->type = OBJECT_TYPE_IMAGE;
				mmi_fm_get_filename(curFilename);
				status=mfw_fm_checkThumbnail(curFilename,fm_data->source);
				if(status == FM_FILE_EXISTS)
				{
					TRACE_EVENT("Thumbnail exists()");
					cur_dir->obj_data[count]->image.thumbnail = TRUE;
				}
				else
				{
					TRACE_EVENT("Thumbnail does not exists()");
					cur_dir->obj_data[count]->image.thumbnail = FALSE;
					//Thumbnail generation required
					thumbanil_generation = TRUE;
				}
			}
			else
			{
				//Other unsupported files
				cur_dir->obj_data[count]->image.thumbnail=FALSE;
			}
		}
		else
		{
			cur_dir->obj_data[count]->image.thumbnail=FALSE;
		}
	}
	return thumbanil_generation;
}


/*******************************************************************************
 $Function:    		mmi_fm_img_list_create

 $Description:		Creates the image files list

 $Returns:		Created Window Pointer

 $Arguments:		win		-Parent window
*******************************************************************************/
T_MFW_HND mmi_fm_img_list_create(T_MFW_HND win)
{
	USHORT  file_count = 0, list_count=0;
	BOOL thumbanil_generation= FALSE;

	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	
	TRACE_FUNCTION ("mmi_fm_img_list_create()");
	/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- Start --> */
	
	MMI_IMGLIST_CHANGE_STS(MMI_IMGLIST_STS_NORMAL);
	
	/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- End --> */
	
	//Traverse through the list to find out valid image objects in the given directory, 
	//and if thumbnail generation is needed
	thumbanil_generation = mmi_fm_img_validate();
	cur_dir->startIndex = 0;
	for (file_count=0; file_count<cur_dir->num_objs; file_count++)
	{
		if(cur_dir->obj_list[file_count]->type == OBJECT_TYPE_FILE || cur_dir->obj_list[file_count]->type == OBJECT_TYPE_IMAGE)
		{
			mnuInitDataItem(&fm_data->menu_list_data->List[list_count]);
			fm_data->menu_list_data->List[list_count].icon=&fmIconFile;
			fm_data->menu_list_data->List[list_count].str  = (char *)cur_dir->obj_list[file_count]->name;
			fm_data->menu_list_data->List[list_count].flagFunc = item_flag_none;
			list_count++;
		}
		else if( cur_dir->obj_list[file_count]->type == OBJECT_TYPE_FOLDER)
		{
			mnuInitDataItem(&fm_data->menu_list_data->List[list_count]);
			fm_data->menu_list_data->List[list_count].icon=&fmIconFolder;
			fm_data->menu_list_data->List[list_count].str  = (char *)cur_dir->obj_list[file_count]->name;
			fm_data->menu_list_data->List[list_count].flagFunc = item_flag_none;
			list_count++;
		}
	}
	fm_data->menu_list_data->ListLength = list_count;
	fm_data->menu_list_data->ListPosition = cur_dir->startIndex+1;
	fm_data->menu_list_data->CursorPosition = cur_dir->startIndex+1;
	if(cur_dir->num_objs < 3)
		fm_data->menu_list_data->SnapshotSize =cur_dir->num_objs;// list_count;		
	else
		fm_data->menu_list_data->SnapshotSize =3;// list_count;
	fm_data->menu_list_data->Font = 1;
	fm_data->menu_list_data->LeftSoftKey = TxtSoftOptions;
	fm_data->menu_list_data->RightSoftKey = TxtSoftBack;
	fm_data->menu_list_data->KeyEvents = KEY_ALL;
	fm_data->menu_list_data->Reason = 0;
	fm_data->menu_list_data->Strings = TRUE;
	fm_data->menu_list_data->Attr   = (MfwMnuAttr*)&FMList_Attrib;
	fm_data->menu_list_data->autoDestroy    = FALSE;
	fm_data->menu_list_data->block_keys =  (BOOL)FALSE;
	fm_data->list_state = FM_LIST_CREATE;
	
	//Create the List window with default file and folder icons
	listIconsDisplayListMenu(win, fm_data->menu_list_data, (ListCbFunc)mmi_fm_img_list_cb, 0);
	dspl_Enable(1);
	cur_dir->currentIndex=cur_dir->startIndex;
	TRACE_EVENT_P2("Num of obj %d, CurrIndex %d",cur_dir->num_objs, cur_dir->currentIndex);
	if(thumbanil_generation && (fm_data->opt_type == FM_NONE || fm_data->opt_type == FM_OPEN))
	{
		//Thumbnails need to be generated
		mmi_fm_imgThmbGen_start();
	}
	else
	{
		//Thumbnails are already generated
		mmi_fm_imgThmbDraw_start();
	}
	return fm_data->menu_list_data->win;
}

/*******************************************************************************
 $Function:    		mmi_fm_img_list_cb

 $Description:		 Call back function for the image list window

 $Returns:		None

 $Arguments:		Parent		-window handler
 				List Data		-Menu list data
*******************************************************************************/
void mmi_fm_img_list_cb(T_MFW_HND * Parent, ListMenuData * ListData)
{
	T_FM_CURDIR *cur_dir ;
	T_FM_IMG_STATE result = FM_UCP_NO_ERROR;

	TRACE_FUNCTION("mmi_fm_img_list_cb");

	cur_dir = fm_data->cur_dir;
	mmi_fm_set_mmi_plane_to_front();
	TRACE_EVENT_P1("Reason %d",ListData->Reason);
	switch(ListData->Reason)
	{
		case LISTS_REASON_SELECT:
			cur_dir->currentIndex=ListData->CursorPosition;
			if(ListData->selectKey==TRUE)
			{
				fm_data->opt_type=FM_OPEN;
				switch(cur_dir->obj_list[cur_dir->currentIndex]->type) 
				{
					case OBJECT_TYPE_IMAGE :
						TRACE_EVENT("OBJECT_TYPE_IMAGE" );
						if(cur_dir->obj_data[cur_dir->currentIndex]->image.thumbnail == TRUE && 
							cur_dir->dirinfo.img.cmd == FM_UCP_THMBIMG_DRAW && 
							cur_dir->dirinfo.img.state == FM_UCP_NO_ERROR)
						{
							mmi_fm_view_start();
						}
						else
						{
							mmi_fm_app_show_info(fm_data->list_win,TxtNo,TxtThumbnail,TxtNull,TxtSoftBack,0,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
						}
					break;
					case OBJECT_TYPE_FOLDER:
					case OBJECT_TYPE_IMAGE_FOLDER:
						TRACE_EVENT("OBJECT_TYPE_FOLDER" );
						if( (cur_dir->dirinfo.img.cmd == FM_UCP_QCIFIMG_DRAW || cur_dir->dirinfo.img.cmd == FM_UCP_THMBIMG_DRAW) &&
						      cur_dir->dirinfo.img.state == FM_UCP_NO_ERROR) 
						{
							cur_dir->dirinfo.img.cmd = FM_UCP_THMBIMG_DEINIT;
							cur_dir->dirinfo.img.state = FM_UCP_NONE;
							result = mfw_fm_imgViewer_deinit();
							if(result != FM_UCP_NO_ERROR)
							{
								//Error deinitializing the Viewer UCP, destroy it
								TRACE_ERROR("Error deinitializing the Viewer UCP");
								cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
								cur_dir->dirinfo.img.state = FM_UCP_NONE;
								result = mfw_fm_imgViewer_destroy();
								if(result != FM_UCP_NO_ERROR)
									TRACE_ERROR("Error destroying the Viewer UCP");
							}
						}
						else
							mmi_fm_open();
						break;
					case OBJECT_TYPE_FILE:
						TRACE_EVENT("OBJECT_TYPE_FILE" );
						mmi_fm_app_show_info(fm_data->list_win,TxtNo,TxtThumbnail,TxtNull,TxtSoftBack,0,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
						break;
				}
			}
			else
			{
				TRACE_EVENT("Left Softkey" );
				fm_data->opt_type=FM_OPTIONS_FOLDER;
				fm_data->opt_win=(T_MFW_HND)bookMenuStart(fm_data->root_win, FMMenuOptionAttributes(),0);
			}
			break;
		case LISTS_REASON_BACK:	
		case LISTS_REASON_HANGUP:  /* Apr 10, 2007 DRT: OMAPS00125325 x0039928 */
			TRACE_EVENT("Going back" );
			fm_data->opt_type=FM_BACK;
			if( (cur_dir->dirinfo.img.cmd == FM_UCP_QCIFIMG_DRAW || cur_dir->dirinfo.img.cmd == FM_UCP_THMBIMG_DRAW) &&
			      cur_dir->dirinfo.img.state == FM_UCP_NO_ERROR) 
			{
				cur_dir->dirinfo.img.cmd=FM_UCP_THMBIMG_DEINIT;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				if(cur_dir->num_objs)
				{
					result = mfw_fm_imgViewer_deinit();
					if(result != FM_UCP_NO_ERROR)
					{
						//Error deinitializing the Viewer UCP, destroy it
						TRACE_ERROR("Error deinitializing the Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
				}
				else
					result = mfw_fm_imgViewer_destroy();
			}
			else 
				/* Mar 30, 2007 DRT: OMAPS00122772 x0039928 */
				mmi_fm_goback();  
			break;
		case LISTS_REASON_DRAWCOMPLETE:
			TRACE_EVENT("LISTS_REASON_DRAWCOMPLETE " );
			/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- Start --> */
			
			if ( MMI_IMGLIST_STS == MMI_IMGLIST_STS_SUSPENDED )
			{
				displaySoftKeys(TxtSoftOptions,TxtSoftBack);
			}
			else
			{
			
				fm_data->lineheight = ListData->lineHeight;
				displaySoftKeys(TxtSoftOptions,TxtSoftBack);
				if(fm_data->list_state == FM_LIST_UPDATE)
				{
					
					/* Aug 06, 2007 DRT: OMAPS00140423, x0081909 (Deepak)
					
					 In a list of 5 images when scrolled from 4th image to 3rd,the first file has the thumbnail of the fourth the,
					 second one has the fifth and the third one doesn't have a thumbnail */
					
					/*Now thumbnails are displayed correctly and correspond to the name of the file */
						
					if( (cur_dir->startIndex = (ListData->CursorPosition / ListData->SnapshotSize)*3) != 0) 
					{
						cur_dir->currentIndex = cur_dir->startIndex;
					}
					else
					{
						cur_dir->startIndex = 0;
						cur_dir->currentIndex = cur_dir->startIndex;
					}
					/* Aug 06, 2007 DRT: OMAPS00140423, x0081909 (Deepak) end */
					
					mmi_fm_imgThmbDraw();
				}
				else if(fm_data->list_state == FM_LIST_CREATE)
					fm_data->list_state = FM_LIST_UPDATE;
			}
			
			/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- End --> */
			break;
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_view_start

 $Description:		Starts drawing the image in QCIF width and height in a new window

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
void  mmi_fm_view_start(void)
{
	TRACE_FUNCTION("mmi_fm_view_start");
	
	fm_data->img_win = mmi_fm_view_create();
}

/*******************************************************************************
 $Function:    		mmi_fm_view_create

 $Description:		Creates a new window for drawing the image in QCIF width and height

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
T_MFW_HND mmi_fm_view_create(void)
{
	T_MMI_FM_IMGDRAW_STRUCT *imgDraw_data;
	T_MFW_WIN  * win;

	TRACE_FUNCTION("mmi_fm_view_create");
	/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- Start --> */
	
	MMI_IMGVIEWER_CHANGE_STS(MMI_IMGVIEWER_STS_NORMAL);
	
	/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- End --> */
	
	imgDraw_data =  (T_MMI_FM_IMGDRAW_STRUCT *)mfwAlloc(sizeof(T_MMI_FM_IMGDRAW_STRUCT));

	//Create a window for drawing the image in QCIF width and height
	imgDraw_data->img_win = win_create (fm_data->list_win, 0, E_WIN_VISIBLE|E_WIN_SUSPEND|E_WIN_RESUME|E_WIN_DELETE, (T_MFW_CB)mmi_fm_view_win_cb);

	imgDraw_data ->mmi_control.dialog = (T_DIALOG_FUNC)mmi_fm_view_exec_cb;
	imgDraw_data ->mmi_control.data   = imgDraw_data ;
	imgDraw_data->parent_win= fm_data->list_win;	
	win                      = (T_MFW_WIN *)getWinData(imgDraw_data->img_win);
	win->user                = (MfwUserDataPtr)imgDraw_data;
       imgDraw_data->kbd = kbd_create (imgDraw_data->img_win,KEY_ALL|KEY_MAKE,(T_MFW_CB)mmi_fm_view_kbd_cb);
	SEND_EVENT(imgDraw_data->img_win, FM_IMGDRAW_INIT, 0, (void *)NULL);
	winShow(imgDraw_data->img_win );
	return imgDraw_data->img_win ;
}

/*******************************************************************************
 $Function:    	mmi_fm_view_exec_cb

 $Description:	 Img draw call back for the

 $Returns:	None

 $Arguments:		win			-current window
				event		-window event Id
				value		-unique Id
				parameter	-optional data.
*******************************************************************************/
void mmi_fm_view_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN        * win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_FM_IMGDRAW_STRUCT  * imgDraw_data     = (T_MMI_FM_IMGDRAW_STRUCT *)win_data->user;

	TRACE_FUNCTION("mmi_fm_view_exec_cb");
	TRACE_EVENT_P1("event %d",event);
	switch (event)
	{ 
		case FM_IMGDRAW_INIT:
			dspl_ClearAll();
			displaySoftKeys(TxtSoftOptions,TxtSoftBack);
			mmi_fm_set_mmi_plane_to_front();
			imgDraw_data->zoom = MFW_FM_ZOOM_LOW; 
			imgDraw_data->rotate = MFW_FM_ROTATE_0;
			break;
		case FM_IMGDRAW_DRAW:
			/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- Start --> */
			
			if ( MMI_IMGVIEWER_STS == MMI_IMGVIEWER_STS_SUSPENDED )
			{
				displayMenuKeys(MENU_KEY_ALL);
			}
			else
			{
				displayMenuKeys(MENU_KEY_WAIT);
				mmi_fm_imgDraw();
			}

			/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- End --> */
			break;
		case FM_IMGDRAW_SUSPEND:
			mmi_fm_set_mmi_plane_to_front();
			break;
		case FM_IMGDRAW_RESUME:
			mmi_fm_set_mmi_plane_to_front();
			displaySoftKeys(TxtSoftOptions,TxtSoftBack);
			break;
		case FM_IMGDRAW_DESTROY:
			mmi_fm_view_destroy(imgDraw_data);
			break;		
	}
}

/*******************************************************************************
 $Function:    	mmi_fm_view_win_cb

 $Description:	 	Handler for window events for the image window

 $Returns:		Execution status

 $Arguments:		e	-window event Id
				w	-current window
*******************************************************************************/
int mmi_fm_view_win_cb (MfwEvt e, MfwWin *w)
{
	T_MMI_FM_IMGDRAW_STRUCT  * imgDraw_data     = (T_MMI_FM_IMGDRAW_STRUCT *)w->user;
	TRACE_FUNCTION ("mmi_fm_view_win_cb()");
	TRACE_EVENT_P1("event %d",e);
	switch (e)
	{
	case MfwWinVisible:  /* window is visible  */
		SEND_EVENT(imgDraw_data->img_win, FM_IMGDRAW_DRAW, 0, (void *)NULL);
		break;
	case MfwWinSuspend: /* input focus / selected   */
		/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- Start --> */
		
		MMI_IMGVIEWER_CHANGE_STS(MMI_IMGVIEWER_STS_SUSPENDED);
		
		/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- End --> */
		SEND_EVENT(imgDraw_data->img_win, FM_IMGDRAW_SUSPEND, 0, (void *)NULL);
		break;
	case MfwWinResume: /* input focus / selected   */
		/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- Start --> */
		
		MMI_IMGVIEWER_CHANGE_STS(MMI_IMGVIEWER_STS_NORMAL);
		
		/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- End --> */
		SEND_EVENT(imgDraw_data->img_win, FM_IMGDRAW_RESUME, 0, (void *)NULL);
		break;
	case MfwWinDelete:   /* window will be deleted   */
		/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- Start --> */
		
		MMI_IMGVIEWER_CHANGE_STS(MMI_IMGVIEWER_STS_NORMAL);
		
		/* August 03, 2007 DRT:OMAPS00138886 x0080701 (Bharat) -- End --> */
		break;
	default:
		return MFW_EVENT_REJECTED;
	}
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
 $Function:    	mmi_fm_view_kbd_cb

 $Description:	 	Handler for key events for the image window

 $Returns:		Execution status

 $Arguments:		e	-window event Id
				k	-key event
*******************************************************************************/
static int mmi_fm_view_kbd_cb (MfwEvt e, MfwKbd *k)
{
	T_MFW_HND    win  = mfwParent(mfw_header());
	T_MFW_WIN  * win_data = ((T_MFW_HDR *)win)->data;
	T_MMI_FM_IMGDRAW_STRUCT      * data = (T_MMI_FM_IMGDRAW_STRUCT *)win_data->user;
	char  currFile[FM_MAX_DIR_PATH_LENGTH];
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	T_FM_IMG_STATE result = FM_UCP_NO_ERROR;

	TRACE_FUNCTION("mmi_fm_view_kbd_cb");

	if (!(e & KEY_MAKE))
	{
		return MFW_EVENT_CONSUMED;
	}
	if(cur_dir->dirinfo.img.cmd == FM_UCP_QCIFIMG_DRAW && cur_dir->dirinfo.img.state == FM_UCP_NO_ERROR)
	{
		//Viewer UCP created successfully. Handle the key events
		switch (k->code)
		{
			case KCD_LEFT:
				fm_data->opt_type = FM_OPTIONS_FILE;
				fm_data->opt_win = (T_MFW_HND)bookMenuStart(data->img_win, FMMenuOptionAttributes(),0);
				break;
			case KCD_RIGHT:
			case KCD_HUP:
				SEND_EVENT(win,FM_IMGDRAW_DESTROY,0,(void *)NULL);
				break;		
			case KCD_MNUUP:
				data->zoom = data->zoom * 2;
				if(data->zoom > MFW_FM_ZOOM_MAX)
					data->zoom = MFW_FM_ZOOM_MAX;
				sprintf(currFile,"%s%s/%s",cur_dir->dir_path,cur_dir->dir_name,cur_dir->obj_list[cur_dir->currentIndex ]->name);
				result = mfw_fm_imgViewer_setConfigs(currFile,FM_MFW_QCIF_WIDTH,FM_MFW_QCIF_HEIGHT,0,0,data->zoom, data->rotate, fm_data->source);			
				if(result != FM_UCP_NO_ERROR)
				{
					//Error configuring the Viewer UCP, deinit and destroy it
					TRACE_ERROR("Error configuring Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_QCIFIMG_DEINIT;
					cur_dir->dirinfo.img.state = FM_UCP_ERROR;
					result = mfw_fm_imgViewer_deinit();
					if(result != FM_UCP_NO_ERROR)
					{
						//Error deinitializing the Viewer UCP, destroy it
						TRACE_ERROR("Error deinitializing the Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
					return MFW_EVENT_CONSUMED;
				}
				displayMenuKeys(MENU_KEY_WAIT);
				cur_dir->dirinfo.img.cmd = FM_UCP_QCIFIMG_DRAW;		
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgViewer_view();
				if(result != FM_UCP_NO_ERROR)
				{
					//Error drawing thumbnail in the Viewer UCP, deinit and destroy it
					TRACE_ERROR("Error drawing thumbnail in the Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_QCIFIMG_DEINIT;
					cur_dir->dirinfo.img.state = FM_UCP_ERROR;
					result = mfw_fm_imgViewer_deinit();
					if(result != FM_UCP_NO_ERROR)
					{
						//Error deinitializing the Viewer UCP, destroy it
						TRACE_ERROR("Error deinitializing the Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
				}
				break;
			case KCD_MNUDOWN:
				data->zoom = data->zoom / 2;
				if(data->zoom < MFW_FM_ZOOM_LOW)
					data->zoom = MFW_FM_ZOOM_LOW;
				sprintf(currFile,"%s%s/%s",cur_dir->dir_path,cur_dir->dir_name,cur_dir->obj_list[cur_dir->currentIndex ]->name);
				result = mfw_fm_imgViewer_setConfigs(currFile,FM_MFW_QCIF_WIDTH,FM_MFW_QCIF_HEIGHT,0,0,data->zoom, data->rotate,fm_data->source);			
				if(result != FM_UCP_NO_ERROR)
				{
					//Error configuring the Viewer UCP, deinit and destroy it
					TRACE_ERROR("Error configuring Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_QCIFIMG_DEINIT;
					cur_dir->dirinfo.img.state = FM_UCP_ERROR;
					result = mfw_fm_imgViewer_deinit();
					if(result != FM_UCP_NO_ERROR)
					{
						//Error deinitializing the Viewer UCP, destroy it
						TRACE_ERROR("Error deinitializing the Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
					return MFW_EVENT_CONSUMED;
				}
				displayMenuKeys(MENU_KEY_WAIT);
				cur_dir->dirinfo.img.cmd =FM_UCP_QCIFIMG_DRAW;		
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgViewer_view();
				if(result != FM_UCP_NO_ERROR)
				{
					//Error drawing thumbnail in the Viewer UCP, deinit and destroy it
					TRACE_ERROR("Error drawing thumbnail in the Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_QCIFIMG_DEINIT;
					cur_dir->dirinfo.img.state = FM_UCP_ERROR;
					result = mfw_fm_imgViewer_deinit();
					if(result != FM_UCP_NO_ERROR)
					{
						//Error deinitializing the Viewer UCP, destroy it
						TRACE_ERROR("Error deinitializing the Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
				}
				break;
			case KCD_MNURIGHT:
				data->rotate = (T_FM_IMG_ROTATE) (data->rotate+1);
				if(data->rotate > MFW_FM_ROTATE_270) 
					data->rotate = MFW_FM_ROTATE_0;
				sprintf(currFile,"%s%s/%s",cur_dir->dir_path,cur_dir->dir_name,cur_dir->obj_list[cur_dir->currentIndex ]->name);
				dspl_ClearAll();
				displaySoftKeys(TxtSoftOptions,TxtSoftBack);
				 dspl_Enable(1);
	 			 mmi_fm_set_mmi_plane_to_front();
				result = mfw_fm_imgViewer_setConfigs(currFile,FM_MFW_QCIF_WIDTH,FM_MFW_QCIF_HEIGHT,0,0,data->zoom, data->rotate,fm_data->source);			
				if(result != FM_UCP_NO_ERROR)
				{
					//Error configuring the Viewer UCP, deinit and destroy it
					TRACE_ERROR("Error configuring Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_QCIFIMG_DEINIT;
					cur_dir->dirinfo.img.state = FM_UCP_ERROR;
					result = mfw_fm_imgViewer_deinit();
					if(result != FM_UCP_NO_ERROR)
					{
						//Error deinitializing the Viewer UCP, destroy it
						TRACE_ERROR("Error deinitializing the Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
					return MFW_EVENT_CONSUMED;
				}
				displayMenuKeys(MENU_KEY_WAIT);
				cur_dir->dirinfo.img.cmd =FM_UCP_QCIFIMG_DRAW;		
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgViewer_view();
				if(result != FM_UCP_NO_ERROR)
				{
					//Error drawing thumbnail in the Viewer UCP, deinit and destroy it
					TRACE_ERROR("Error drawing thumbnail in the Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_QCIFIMG_DEINIT;
					cur_dir->dirinfo.img.state = FM_UCP_ERROR;
					result = mfw_fm_imgViewer_deinit();
					if(result != FM_UCP_NO_ERROR)
					{
						//Error deinitializing the Viewer UCP, destroy it
						TRACE_ERROR("Error deinitializing the Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
				}
				break;
			case KCD_MNULEFT:
				data->rotate = (T_FM_IMG_ROTATE) (data->rotate-1);
				if(data->rotate < MFW_FM_ROTATE_0)
					data->rotate = MFW_FM_ROTATE_270;
				sprintf(currFile,"%s%s/%s",cur_dir->dir_path,cur_dir->dir_name,cur_dir->obj_list[cur_dir->currentIndex ]->name);
				dspl_ClearAll();
				displaySoftKeys(TxtSoftOptions,TxtSoftBack);
				 dspl_Enable(1);
	 			 mmi_fm_set_mmi_plane_to_front();
				result = mfw_fm_imgViewer_setConfigs(currFile,FM_MFW_QCIF_WIDTH,FM_MFW_QCIF_HEIGHT,0,0,data->zoom, data->rotate,fm_data->source);			
				if(result != FM_UCP_NO_ERROR)
				{
					//Error configuring the Viewer UCP, deinit and destroy it
					TRACE_ERROR("Error configuring Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_QCIFIMG_DEINIT;
					cur_dir->dirinfo.img.state = FM_UCP_ERROR;
					result = mfw_fm_imgViewer_deinit();
					if(result != FM_UCP_NO_ERROR)
					{
						//Error deinitializing the Viewer UCP, destroy it
						TRACE_ERROR("Error deinitializing the Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
					return MFW_EVENT_CONSUMED;
				}
				displayMenuKeys(MENU_KEY_WAIT);
				cur_dir->dirinfo.img.cmd =FM_UCP_QCIFIMG_DRAW;	
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgViewer_view();
				if(result != FM_UCP_NO_ERROR)
				{
					//Error drawing thumbnail in the Viewer UCP, deinit and destroy it
					TRACE_ERROR("Error drawing thumbnail in the Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_QCIFIMG_DEINIT;
					cur_dir->dirinfo.img.state = FM_UCP_ERROR;
					result = mfw_fm_imgViewer_deinit();
					if(result != FM_UCP_NO_ERROR)
					{
						//Error deinitializing the Viewer UCP, destroy it
						TRACE_ERROR("Error deinitializing the Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
				}
				break;
		} 
	}
	else if(cur_dir->dirinfo.img.cmd == FM_UCP_NONE && cur_dir->dirinfo.img.state == FM_UCP_NONE)
	{
		//Viewer UCP was not created successfully.
		switch (k->code)
		{
			case KCD_LEFT:
				fm_data->opt_type=FM_OPTIONS_FILE;
				fm_data->opt_win=(T_MFW_HND)bookMenuStart(data->img_win, FMMenuOptionAttributes(),0);
				break;
			case KCD_RIGHT:
			case KCD_HUP:
				SEND_EVENT(win,FM_IMGDRAW_DESTROY,0,(void *)NULL);
				break;		
		}
	}
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
 $Function:    		mmi_fm_view_destroy

 $Description:		Destroys the image window and releases the allocated memory for the internal structure

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
void mmi_fm_view_destroy(T_MMI_FM_IMGDRAW_STRUCT  * imgDraw_data)
{
	TRACE_FUNCTION("mmi_fm_view_destroy");
	
	mmi_fm_set_mmi_plane_to_front();
	winDelete(imgDraw_data->img_win);
	fm_data->img_win = NULL;
	mfwFree((U8 *)imgDraw_data, sizeof(T_MMI_FM_IMGDRAW_STRUCT));

}

/*******************************************************************************
 $Function:    		mmi_fm_aud_validate

 $Description:	 	Validates the files in the audio folder. 

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
void mmi_fm_aud_validate(void)
{
	USHORT  count = 0;
	#ifdef FF_MMI_UNICODE_SUPPORT
		T_WCHAR  curFilename_uc[FM_MAX_OBJ_NAME_LENGTH];
	#else
		char curFilename_u8[FM_MAX_OBJ_NAME_LENGTH];
	#endif
	/* January 29, 2008 DRT:OMAPS00156759 (x0082844) --> End*/
		
	char ext[FM_MAX_EXT_LENGTH];
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	
	TRACE_FUNCTION ("mmi_fm_aud_validate()");
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state03 %d",cur_dir->dirinfo.aud.state);
	for (count=0; count<cur_dir->num_objs; count++)
	{
		/* January 29, 2008 DRT:OMAPS00156759 (x0082844) For Unicode Support--> Start*/
		#ifdef FF_MMI_UNICODE_SUPPORT
		wstrcpy(curFilename_uc,cur_dir->obj_list[count]->name_uc);
		#else
		strcpy(curFilename_u8,cur_dir->obj_list[count]->name);
		#endif
		/* January 29, 2008 DRT:OMAPS00156759 (x0082844) --> End*/
		cur_dir->obj_data[count] = (T_FM_OBJ_DATA*)mfwAlloc(sizeof(T_FM_OBJ_DATA));
		/* January 29, 2008 DRT:OMAPS00156759 (x0082844) For Unicode Support--> Start*/
			#ifdef FF_MMI_UNICODE_SUPPORT
				TRACE_EVENT_P2("File %s type %d",cur_dir->obj_list[count]->name_uc,cur_dir->obj_list[count]->type);
			#else
				TRACE_EVENT_P2("File %s type %d",cur_dir->obj_list[count]->name,cur_dir->obj_list[count]->type);
			#endif
		/* January 29, 2008 DRT:OMAPS00156759 (x0082844) --> End*/
		if(cur_dir->obj_list[count]->type == OBJECT_TYPE_FILE )
		{
			/* January 29, 2008 DRT:OMAPS00156759 (x0082844) For Unicode Support--> Start*/
			#ifdef FF_MMI_UNICODE_SUPPORT
				convert_unicode_to_u8(wstrchr(curFilename_uc,'.')+1, ext);
			#else 
				strcpy(ext, mmi_fm_get_ext(curFilename_u8));
			#endif
			/* January 29, 2008 DRT:OMAPS00156759 (x0082844) --> End*/
			if(strcmp((const char*)ext, "mid") == 0 ) 
			{
				cur_dir->obj_list[count]->type = OBJECT_TYPE_AUDIO_MIDI;
			}
			else if(strcmp((const char*)ext, "mp3") == 0 ) 
			{
				cur_dir->obj_list[count]->type = OBJECT_TYPE_AUDIO_MP3;
			}
			else if(strcmp((const char*)ext, "aac") == 0 ) 
			{
				cur_dir->obj_list[count]->type = OBJECT_TYPE_AUDIO_AAC;
			}  /* Apr 06, 2007 DRT: OMAPS00124874 x0039928 */
			else if(strcmp((const char*)ext, "xmf") == 0 ) 
			{
				cur_dir->obj_list[count]->type = OBJECT_TYPE_AUDIO_XMF;
			}
			else if(strcmp((const char*)ext, "imy") == 0 ) 
			{
				cur_dir->obj_list[count]->type = OBJECT_TYPE_AUDIO_IMY;
			}
			else if(strcmp((const char*)ext, "mmf") == 0 ) 
			{
				cur_dir->obj_list[count]->type = OBJECT_TYPE_AUDIO_MMF;
			}
			else if(strcmp((const char*)ext, "mxmf") == 0 ) 
			{
				cur_dir->obj_list[count]->type = OBJECT_TYPE_AUDIO_MXMF;
			}
			else if(strcmp((const char*)ext, "sms") == 0 ) 
			{
				cur_dir->obj_list[count]->type = OBJECT_TYPE_AUDIO_SMS;
			}
			else if(strcmp((const char*)ext, "wav") == 0 ) 
			{
				cur_dir->obj_list[count]->type = OBJECT_TYPE_AUDIO_WAV;
			}
			else
				cur_dir->obj_list[count]->type = OBJECT_TYPE_AUDIO;
		}
		/* January 29, 2008 DRT:OMAPS00156759 (x0082844) For Unicode Support--> Start*/
		#ifdef FF_MMI_UNICODE_SUPPORT
			TRACE_EVENT_P2("File %s type %d",cur_dir->obj_list[count]->name_uc,cur_dir->obj_list[count]->type);
		#else
			TRACE_EVENT_P2("File %s type %d",cur_dir->obj_list[count]->name,cur_dir->obj_list[count]->type);
		#endif
		/* January 29, 2008 DRT:OMAPS00156759 (x0082844) --> End*/
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_aud_list_cb

 $Description:		Call back function for the audio list window

 $Returns:		None

 $Arguments:		Parent		-window handler
 				List Data		-Menu list data
*******************************************************************************/

void mmi_fm_aud_list_cb(T_MFW_HND * Parent, ListMenuData * ListData)
{
	T_MFW_WIN       * win_data;
	T_MMI_FM_STRUCT* fm_data;
	T_FM_CURDIR *cur_dir;
	char  currFilePath[FM_MAX_DIR_PATH_LENGTH];
	T_FM_AUD_STATE aud_state;
	int state;
	
	
       win_data = ((T_MFW_HDR *)Parent)->data;
       fm_data = (T_MMI_FM_STRUCT*)win_data->user;
	cur_dir = fm_data->cur_dir;
	TRACE_EVENT_P1("Reason %d",ListData->Reason);
	switch(ListData->Reason)
	{
		case LISTS_REASON_SELECT:
			cur_dir->currentIndex=ListData->CursorPosition;
			//Jun 26, 2007 DRT:OMAPS00135749  x0062174(Nimitha)
			state = mmi_get_aud_state_status();
			if( state == FM_AUD_PLAY)
			{
			 	cur_dir->dirinfo.aud.state = FM_AUD_PLAY;
				mmi_set_aud_state_status(FM_AUD_NONE);
			}
			 /* Apr 06, 2007 DRT: OMAPS00124874 x0039928 */		

			//Jun 26, 2007 DRT:OMAPS00135749  x0062174(Nimitha)
                    //May28,07 DRT:OMAPS00132459  x0061560(sateeshg)
			/*if(aud_state_back )
			{
				displayMenuKeys(MENU_KEY_UP_DOWN_STOP);
				cur_dir->dirinfo.aud.state = FM_AUD_PLAY;  
				aud_state_back=0;
			}*/
				
			
			cur_dir->currentIndex=ListData->CursorPosition;
			 /* Apr 06, 2007 DRT: OMAPS00124874 x0039928 */
			
                   //May28,07 DRT:OMAPS00132459  x0061560(sateeshg)
			/*if(aud_state_back )
			{
				displayMenuKeys(MENU_KEY_UP_DOWN_STOP);
				cur_dir->dirinfo.aud.state = FM_AUD_PLAY;
				aud_state_back=0;
			}*/
				
			
			if(ListData->selectKey==TRUE)
			{
				if(cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO  || 
				    cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_MIDI  ||
				    cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_MP3  ||
				    cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_AAC ||
				    cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_XMF ||
				    cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_IMY ||
				    cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_MMF ||
				    cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_MXMF ||
				    cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_SMS ||
				    cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_WAV)
				{

					switch(cur_dir->dirinfo.aud.state) 
					{
						case FM_AUD_NONE:
						{
							/* January 29, 2008 DRT:OMAPS00156759 (x0082844) For Unicode Support--> Start*/
							#ifdef FF_MMI_UNICODE_SUPPORT
								T_WCHAR filename_uc[FM_MAX_DIR_PATH_LENGTH];
							#endif
							/* January 29, 2008 DRT:OMAPS00156759 (x0082844) --> End*/
							
							cur_dir->dirinfo.aud.state = FM_AUD_PLAY;
							switch(fm_data->source)
							{
								case FM_NOR_FLASH:
									/* January 29, 2008 DRT:OMAPS00156759 (x0082844) For Unicode Support--> Start*/
									#ifdef FF_MMI_UNICODE_SUPPORT
										sprintf(currFilePath,"%s%s%s/","/FFS",cur_dir->dir_path,cur_dir->dir_name);
									#else
										sprintf(currFilePath,"%s%s%s/%s","/FFS",cur_dir->dir_path,cur_dir->dir_name,cur_dir->obj_list[cur_dir->currentIndex]->name);
									#endif
									/* January 29, 2008 DRT:OMAPS00156759 (x0082844) --> End*/								
									break;

								case FM_NORMS_FLASH:
									/* January 29, 2008 DRT:OMAPS00156759 (x0082844) For Unicode Support--> Start*/
									#ifdef FF_MMI_UNICODE_SUPPORT
										sprintf(currFilePath,"%s%s%s/","/NOR",cur_dir->dir_path,cur_dir->dir_name);
									#else
										sprintf(currFilePath,"%s%s%s/%s","/NOR",cur_dir->dir_path,cur_dir->dir_name,cur_dir->obj_list[cur_dir->currentIndex]->name);
									#endif
									/* January 29, 2008 DRT:OMAPS00156759 (x0082844) --> End*/	
									break;									

								case FM_NAND_FLASH:
									/* January 29, 2008 DRT:OMAPS00156759 (x0082844) For Unicode Support--> Start*/
									#ifdef FF_MMI_UNICODE_SUPPORT
										sprintf(currFilePath,"%s%s%s/","/NAND",cur_dir->dir_path,cur_dir->dir_name);
									#else
										sprintf(currFilePath,"%s%s%s/%s","/NAND",cur_dir->dir_path,cur_dir->dir_name,cur_dir->obj_list[cur_dir->currentIndex]->name);
									#endif
									/* January 29, 2008 DRT:OMAPS00156759 (x0082844) --> End*/	
									break;

								case FM_T_FLASH:
									#ifdef FF_MMI_UNICODE_SUPPORT
										sprintf(currFilePath,"%s%s%s/","/MMC",cur_dir->dir_path,cur_dir->dir_name);
									#else
										sprintf(currFilePath,"%s%s%s/%s","/MMC",cur_dir->dir_path,cur_dir->dir_name,cur_dir->obj_list[cur_dir->currentIndex]->name);
									#endif
									break;
							}

							/* January 29, 2008 DRT:OMAPS00156759 (x0082844) For Unicode Support--> Start*/
							#ifdef FF_MMI_UNICODE_SUPPORT
								convert_u8_to_unicode(currFilePath, filename_uc);
								wstrcat(filename_uc, cur_dir->obj_list[cur_dir->currentIndex]->name_uc);
							#endif
							/* January 29, 2008 DRT:OMAPS00156759 (x0082844) --> End*/	
						if(mmi_progress_bar == PB_OFF)
						{
							IsMP3Playing = TRUE;//Daisy tang added for Real Resume feature 20071107
								/* January 29, 2008 DRT:OMAPS00156759 (x0082844) For Unicode Support--> Start*/
								#ifdef FF_MMI_UNICODE_SUPPORT
								   aud_state =mfw_fm_audPlay(filename_uc, cur_dir->obj_list[cur_dir->currentIndex]->type, mmi_progress_bar);
								#else
							        aud_state =mfw_fm_audPlay(currFilePath, cur_dir->obj_list[cur_dir->currentIndex]->type, mmi_progress_bar);
								#endif
								/* January 29, 2008 DRT:OMAPS00156759 (x0082844) --> End*/	
							//Jun 28, 2007 DRT:OMAPS00135749  x0062174(Nimitha)
							/* //May28,07 DRT:OMAPS00132459  x0061560(sateeshg)
							aud_state_back=1;*/
							if(aud_state == FM_AUD_NO_ERROR)
								displayMenuKeys(MENU_KEY_UP_DOWN_STOP);
							else
								mmi_fm_app_show_info(Parent,TxtPlay,TxtFailed,TxtNull,TxtSoftBack,NULL,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
						}
						else
						{	
							if((cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_MP3) ||
								(cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_AAC) ||
								(cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_MIDI))
							{
								IsMP3Playing = TRUE;//Daisy tang added for Real Resume feature 20071107
									/* January 29, 2008 DRT:OMAPS00156759 (x0082844) For Unicode Support--> Start*/
									#ifdef FF_MMI_UNICODE_SUPPORT
										aud_state =mfw_fm_audPlay(filename_uc, cur_dir->obj_list[cur_dir->currentIndex]->type, mmi_progress_bar);
									#else

								aud_state =mfw_fm_audPlay(currFilePath, cur_dir->obj_list[cur_dir->currentIndex]->type, mmi_progress_bar);
									#endif
								if(aud_state == FM_AUD_NO_ERROR)
								{
									//Jul 09, 2007 DRT:OMAPS00138065 x0062174(Nimitha)
									displayMenuKeys(MENU_KEY_UP_DOWN_STOP);
									progress_bar(Parent);
								}
								//Jul 09, 2007 DRT:OMAPS00138065 x0062174(Nimitha)
								else
									mmi_fm_app_show_info(Parent,TxtPlay,TxtFailed,TxtNull,TxtSoftBack,NULL,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
							}
							else
							{
								IsMP3Playing = TRUE;//Daisy tang added for Real Resume feature 20071107
									/* January 29, 2008 DRT:OMAPS00156759 (x0082844) For Unicode Support--> Start*/
									#ifdef FF_MMI_UNICODE_SUPPORT
										aud_state =mfw_fm_audPlay(filename_uc, cur_dir->obj_list[cur_dir->currentIndex]->type, PB_OFF);
									#else

								aud_state =mfw_fm_audPlay(currFilePath, cur_dir->obj_list[cur_dir->currentIndex]->type, PB_OFF);
									#endif
									/* January 29, 2008 DRT:OMAPS00156759 (x0082844) --> End*/	

								if(aud_state == FM_AUD_NO_ERROR)
									displayMenuKeys(MENU_KEY_UP_DOWN_STOP);
								else
									mmi_fm_app_show_info(Parent,TxtPlay,TxtFailed,TxtNull,TxtSoftBack,NULL,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
							}
						}
						}
						break;
						case FM_AUD_PLAY:
							cur_dir->dirinfo.aud.state = FM_AUD_NONE;
							IsMP3Playing = FALSE;//Daisy tang added for Real Resume feature 20071107
							aud_state = mfw_fm_audStop();
							if(aud_state != FM_AUD_NO_ERROR)
								mmi_fm_app_show_info(Parent,TxtError,TxtNull,TxtNull,TxtSoftBack,NULL,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
							displayMenuKeys(MENU_KEY_UP_DOWN_PLAY);
							
							break;
					}
				}
				else
				{
					mmi_fm_open();
				}
			}
			else
			{
				
				if(cur_dir->dirinfo.aud.state == FM_AUD_PLAY)
				{
					cur_dir->dirinfo.aud.state = FM_AUD_NONE;
					IsMP3Playing = FALSE;//Daisy tang added for Real Resume feature 20071107
					mfw_fm_audStop();
					
				}
				fm_data->opt_type=FM_OPTIONS_FOLDER;
				fm_data->opt_win=(T_MFW_HND)bookMenuStart(fm_data->root_win, FMMenuOptionAttributes(),0);
			}
			break;
		case LISTS_REASON_BACK:	
		case LISTS_REASON_HANGUP: /* Apr 10, 2007 DRT: OMAPS00125325 x0039928 */
			fm_data->opt_type=FM_BACK;
			
	/*		if(cur_dir->dirinfo.aud.state == FM_AUD_PLAY)
			{
				cur_dir->dirinfo.aud.state = FM_AUD_NONE;
				mfw_fm_audStop();
			}*/

			//Jun 26, 2007 DRT:OMAPS00135749  x0062174(Nimitha)
			if(cur_dir->dirinfo.aud.state == FM_AUD_PLAY)
				mmi_set_aud_state_status( FM_AUD_PLAY);
			
			mmi_fm_goback();
	
			TRACE_EVENT_P1("cur_dir->dirinfo.aud.state222 %d",cur_dir->dirinfo.aud.state);
			break;
		case LISTS_REASON_DRAWCOMPLETE:
			displaySoftKeys(TxtSoftOptions,TxtSoftBack);
			//Jul 03, 2007 DRT:OMAPS00135749  x0062174(Nimitha)
			state = mmi_get_aud_state_status();
			if( state == FM_AUD_PLAY)
				displayMenuKeys(MENU_KEY_UP_DOWN_STOP);
			else	
			displayMenuKeys(MENU_KEY_UP_DOWN_PLAY);
			if(fm_data->list_state == FM_LIST_CREATE)
				fm_data->list_state = FM_LIST_UPDATE;
	/* Mar 15, 2007 DRT: OMAPS00120187 x0039928   */
	/* Fix: mfw_fm_audStop is commented                  */
	/*		if(cur_dir->dirinfo.aud.state == FM_AUD_PLAY)
			{
				cur_dir->dirinfo.aud.state = FM_AUD_NONE;
				mfw_fm_audStop();
			}*/
			TRACE_FUNCTION("LISTS_REASON_DRAWCOMPLETE");
			break;
				
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_aud_list_create

 $Description:		Creates the audio files list

 $Returns:		Created Window Pointer

 $Arguments:		win		-Parent window
*******************************************************************************/
T_MFW_HND mmi_fm_aud_list_create(T_MFW_HND win)
{
	USHORT  file_count = 0, list_count=0;
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	
	TRACE_FUNCTION ("mmi_fm_aud_list_create()");
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state04 %d",cur_dir->dirinfo.aud.state);
	//Traverse through the list to find out valid audio objects in the given directory
	mmi_fm_aud_validate();
	
	for (file_count=0; file_count<cur_dir->num_objs; file_count++)
	{
		mnuInitDataItem(&fm_data->menu_list_data->List[list_count]);
		
		/* January 29, 2008 DRT:OMAPS00156759 (x0082844) For Unicode Support--> Start*/
		#ifdef FF_MMI_UNICODE_SUPPORT
			{
				char *filename; 
				
				filename = (char *)mfwAlloc((wstrlen(cur_dir->obj_list[file_count]->name_uc)+1)*2);
				unicode_to_char_array(cur_dir->obj_list[file_count]->name_uc, filename);
				
				fm_data->menu_list_data->List[list_count].str  = filename;
			}
		#else
			fm_data->menu_list_data->List[list_count].str  = (char *)cur_dir->obj_list[file_count]->name;
		#endif
		/* January 29, 2008 DRT:OMAPS00156759 (x0082844) --> End*/
		
		//TRACE_EVENT_P2("file %s type %d", cur_dir->obj_list[file_count]->name_uc,cur_dir->obj_list[file_count]->type);
		
		fm_data->menu_list_data->List[list_count].flagFunc = item_flag_none;
		 /* Apr 06, 2007 DRT: OMAPS00124874 x0039928 */
		switch( cur_dir->obj_list[file_count]->type)
		{
			case OBJECT_TYPE_AUDIO:
			case OBJECT_TYPE_AUDIO_MIDI:
			case OBJECT_TYPE_AUDIO_MP3:
		 	case OBJECT_TYPE_AUDIO_AAC:
			case OBJECT_TYPE_AUDIO_XMF:
			case OBJECT_TYPE_AUDIO_IMY:
			case OBJECT_TYPE_AUDIO_MMF:
			case OBJECT_TYPE_AUDIO_MXMF:
			case OBJECT_TYPE_AUDIO_SMS:
			case OBJECT_TYPE_AUDIO_WAV:
			{
				TRACE_EVENT("Audio");
				fm_data->menu_list_data->List[list_count].icon=&fmIconAudioFile;
			//	cur_dir->obj_list[file_count]->type = mfw_fm_aud_get_type(cur_dir->obj_list[file_count]->name);
			}
			list_count++;
			break;
			case OBJECT_TYPE_FILE:
			{
				TRACE_EVENT("File");
				fm_data->menu_list_data->List[list_count].icon=&fmIconFile;
			}
			list_count++;
			break;
			case OBJECT_TYPE_FOLDER:
			{
				TRACE_EVENT("Folder");
				fm_data->menu_list_data->List[list_count].icon=&fmIconFolder;
			}
			list_count++;
			break;
		}
		
	}
	TRACE_EVENT_P1("Total %d",list_count);
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state05 %d",cur_dir->dirinfo.aud.state);
	fm_data->menu_list_data->ListLength = list_count;
	fm_data->menu_list_data->ListPosition = cur_dir->startIndex+1;
	fm_data->menu_list_data->CursorPosition = cur_dir->startIndex+1;
	if(cur_dir->num_objs < 3)
		fm_data->menu_list_data->SnapshotSize =cur_dir->num_objs;// list_count;		
	else
		fm_data->menu_list_data->SnapshotSize =3;// list_count;
	fm_data->menu_list_data->Font = 1;
	fm_data->menu_list_data->LeftSoftKey = TxtSoftOptions;
	fm_data->menu_list_data->RightSoftKey = TxtSoftBack;
	fm_data->menu_list_data->KeyEvents = KEY_ALL;
	fm_data->menu_list_data->Reason = 0;
	fm_data->menu_list_data->Strings = TRUE;
	fm_data->menu_list_data->Attr   = (MfwMnuAttr*)&FMList_Attrib;
	fm_data->menu_list_data->autoDestroy    = FALSE;
	fm_data->menu_list_data->block_keys =  (BOOL)FALSE;
	cur_dir->dirinfo.aud.state = FM_AUD_NONE;

	/* OMAPS00151698,x0056422 */
	#ifdef FF_MMI_A2DP_AVRCP
	mmi_set_aud_state_status(FM_AUD_NONE);
	tGlobalBmiBtStruct.tAudioState =FM_AUD_NONE;
	#endif //FF_MMI_A2DP_AVRCP

	fm_data->list_state = FM_LIST_CREATE;
	//Create the List window with default file and folder icons
	/* January 29, 2008 DRT:OMAPS00156759 (x0082844) For Unicode Support--> Start*/
	#ifdef FF_MMI_UNICODE_SUPPORT
		listIconsDisplayListMenu(win, fm_data->menu_list_data, (ListCbFunc)mmi_fm_aud_list_cb, TRUE);
	#else
		listIconsDisplayListMenu(win, fm_data->menu_list_data, (ListCbFunc)mmi_fm_aud_list_cb, FALSE);
	#endif
	/* January 29, 2008 DRT:OMAPS00156759 (x0082844) --> End*/

	dspl_Enable(1);
	return fm_data->menu_list_data->win;
}

/*******************************************************************************
 $Function:    		mmi_fm_nofile_cb

 $Description:		Callback function for "no files loaded" dialog

 $Returns:		None

 $Arguments:		win			-current window
				identifier		-window id
				reason		-Event cause.
*******************************************************************************/
void mmi_fm_nofile_cb (T_MFW_HND win, USHORT identifier, SHORT reason)
{
	TRACE_FUNCTION("mmi_fm_nofile_cb" );
	
	mmi_fm_goback();
}

/*******************************************************************************
 $Function:    		mmi_fm_goback

 $Description:		returns to the previous directory. 

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
void mmi_fm_goback(void)
{
	char *strTempPtr;
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	
	TRACE_FUNCTION("mmi_fm_goback");
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state1 %d",cur_dir->dirinfo.aud.state);
	cur_dir->depth--;
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state2 %d",cur_dir->dirinfo.aud.state);
	mmi_fm_set_mmi_plane_to_front();
	if( cur_dir->depth <= 0 )
	{
		//Root directory, exit the list window
		TRACE_EVENT("Root directory");
		if(fm_data->list_win)
		{
			listsIconsDestroy(fm_data->list_win);
			fm_data->list_win=NULL;
			mmi_fm_list_destroy( fm_data->root_win );
		}
		if(cur_dir->dir_type == OBJECT_TYPE_IMAGE_FOLDER)
		{
			if(cur_dir->dirinfo.img.ipMsl)
			{
				aci_delete(cur_dir->dirinfo.img.ipMsl);
				cur_dir->dirinfo.img.ipMsl=0;
			}
		}
		SEND_EVENT (fm_data->root_win, FM_DESTROY, 0, (void *)NULL);
	}
	else
	{
		
		//Extract the previous directory path 
		strTempPtr=(char*) &cur_dir->dir_path + strlen(cur_dir->dir_path)-1;
		*strTempPtr='\0';
		strTempPtr--;
		while( *strTempPtr != '/'  )
		{
			strTempPtr = strTempPtr - 1;
		}
		strTempPtr++;
		strcpy(cur_dir->dir_name,strTempPtr);
		*strTempPtr='\0';
		
		//Destroy the current list
		if(fm_data->list_win)
		{
			listsIconsDestroy(fm_data->list_win);
			fm_data->list_win=NULL;
			mmi_fm_list_destroy( fm_data->root_win );
		}
		
		//Populate the new list
		fm_data->opt_type=FM_BACK;
		SEND_EVENT (fm_data->root_win, FM_INIT, 0, (void *)NULL);
		return;
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_goback

 $Description:		populates the list from the selected directory

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
void mmi_fm_open (void)
{
	char curDir[FM_MAX_DIR_PATH_LENGTH];
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	TRACE_FUNCTION ("mmi_fm_open()");
	if( (cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_IMAGE_FOLDER) ||
	     (cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_FOLDER) ||
	     (cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_FOLDER)  )
	{
		cur_dir->depth ++;
		//Update the current directory name with the selected directory
		if(cur_dir->dir_name[0]!='\0')
		{
			sprintf(curDir,"%s%s/",cur_dir->dir_path,cur_dir->dir_name);
			strcpy(cur_dir->dir_path,curDir);
			strcpy(cur_dir->dir_name , cur_dir->obj_list[cur_dir->currentIndex]->name);
		}
		//Destroy the current list
		if(fm_data->list_win)
		{
			listsIconsDestroy(fm_data->list_win);
			fm_data->list_win=NULL;
			mmi_fm_list_destroy( fm_data->root_win);
		}
		//Populate the new list
		fm_data->opt_type=FM_OPEN;
		SEND_EVENT (fm_data->root_win, FM_INIT, 0, (void *)NULL);
	}
	else if ( ( cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_IMAGE) ||
		( cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_IMAGE))
	{
		TRACE_EVENT("Invalid object found");
	}
}
 
/*******************************************************************************
 $Function:    		mmi_fm_rename

 $Description:		Creates an editor for editing the selected objects name

 $Returns:		Execution status

 $Arguments:		mnu		-Pointer to current menu
 				item 	-Pointer to current menu item
*******************************************************************************/
GLOBAL int mmi_fm_rename (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfw_parent(mfw_header());

	TRACE_FUNCTION ("mmi_fm_rename()");
	
	mmi_fm_rename_editor_create(parent_win);
	
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
 $Function:    		mmi_fm_rename_editor_create

 $Description:		Create an editor for editing the object name

 $Returns:		Created Window Pointer

 $Arguments:		parent_win		-parent_win window
*******************************************************************************/
T_MFW_HND mmi_fm_rename_editor_create(T_MFW_HND parent_win)
{
	T_MFW_HND  win;
	T_AUI_EDITOR_DATA	editor_data;
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;

	TRACE_FUNCTION ("mmi_fm_rename_editor_create");

	fm_data->edt_buf = (char *)mfwAlloc(FM_MAX_OBJ_NAME_LENGTH);

	memset(&editor_data,'\0',sizeof(editor_data));
	//Update the editor buffer with the current object name 
/* February 8, 2008 DRT:OMAPS00156759 (x0083025) For Unicode Support--> Start*/	
#ifdef FF_MMI_UNICODE_SUPPORT
	// unicode support is there only for audio files
	if (fm_data->app == FM_AUDIO)
		convert_unicode_to_u8(cur_dir->obj_list[cur_dir->currentIndex]->name_uc, fm_data->edt_buf);
	else
#endif 
	strcpy(fm_data->edt_buf, cur_dir->obj_list[cur_dir->currentIndex]->name);
 	/*
	** Initialise the editor
	*/
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)fm_data->edt_buf, FM_MAX_OBJ_NAME_LENGTH);
	AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtDelete, TxtRename, NULL);
	AUI_edit_SetAltTextStr(&editor_data, 1, TxtNull, TRUE, TxtSoftBack);
	AUI_edit_SetEvents(&editor_data, 0, TRUE, FOREVER, (T_AUI_EDIT_CB)mmi_fm_rename_editor_exec_cb);
	win = AUI_edit_Start(parent_win, &editor_data);


	return win;

}

/*******************************************************************************
 $Function:    		mmi_fm_rename_editor_exec_cb

 $Description:		Event handler for the editor

 $Returns:		None

 $Arguments:		win			-current window
				identifier		-window id
				reason		-Event cause.
*******************************************************************************/
static void mmi_fm_rename_editor_exec_cb(T_MFW_HND win, USHORT identifier, SHORT reason)
{
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	T_MFW_FM_STATUS status = FM_NO_ERROR;

	char * fileExt;
	BOOL valid_ext = FALSE;
	
	TRACE_FUNCTION ("mmi_fm_rename_editor_exec_cb()");

	
	switch (reason)
	{
		case INFO_KCD_LEFT:
		{	
			/*
			** If there is no string entered, treat this as if it was a "Cancel"
			*/
			if (fm_data->edt_buf[0] == NULL)
			{
				mmi_fm_app_show_info(win,TxtRename,TxtFailed,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_rename_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
			}
			else
			{
				//If the file extn is not jpg, send cancel event
				fileExt = (char *)mmi_fm_get_ext((char *) fm_data->edt_buf);
				switch(cur_dir->obj_list[cur_dir->currentIndex]->type)
				{
					case OBJECT_TYPE_IMAGE:
						if(strcmp((const char*)fileExt, "jpg") != 0 ) 
						{
							valid_ext = FALSE;
						}
						else
						{
							valid_ext = TRUE;
						}
						break;
					case OBJECT_TYPE_AUDIO_MIDI:
						if(strcmp((const char*)fileExt, "mid") != 0 ) 
						{
							valid_ext = FALSE;
						}
						else
						{
							valid_ext = TRUE;
						}
						break;
					case OBJECT_TYPE_AUDIO_MP3:
						if(strcmp((const char*)fileExt, "mp3") != 0 ) 
						{
							valid_ext = FALSE;
						}
						else
						{
							valid_ext = TRUE;
						}
						break;
					case OBJECT_TYPE_AUDIO_AAC:
						if(strcmp((const char*)fileExt, "aac") != 0 ) 
						{
							valid_ext = FALSE;
						}
						else
						{
							valid_ext = TRUE;
						}
						break;
				 /* Apr 06, 2007 DRT: OMAPS00124874 x0039928 */
					case OBJECT_TYPE_AUDIO_XMF:
						if(strcmp((const char*)fileExt, "xmf") != 0 ) 
						{
							valid_ext = FALSE;
						}
						else
						{
							valid_ext = TRUE;
						}
						break;
					case OBJECT_TYPE_AUDIO_IMY:
						if(strcmp((const char*)fileExt, "imy") != 0 ) 
						{
							valid_ext = FALSE;
						}
						else
						{
							valid_ext = TRUE;
						}
						break;
					case OBJECT_TYPE_AUDIO_MMF:
						if(strcmp((const char*)fileExt, "mmf") != 0 ) 
						{
							valid_ext = FALSE;
						}
						else
						{
							valid_ext = TRUE;
						}
						break;
					case OBJECT_TYPE_AUDIO_MXMF:
						if(strcmp((const char*)fileExt, "mxmf") != 0 ) 
						{
							valid_ext = FALSE;
						}
						else
						{
							valid_ext = TRUE;
						}
						break;
					case OBJECT_TYPE_AUDIO_SMS:
						if(strcmp((const char*)fileExt, "sms") != 0 ) 
						{
							valid_ext = FALSE;
						}
						else
						{
							valid_ext = TRUE;
						}
						break;
					case OBJECT_TYPE_AUDIO_WAV:
						if(strcmp((const char*)fileExt, "wav") != 0 ) 
						{
							valid_ext = FALSE;
						}
						else
						{
							valid_ext = TRUE;
						}
						break;
					case OBJECT_TYPE_FILE:
					case OBJECT_TYPE_AUDIO:
					case OBJECT_TYPE_FOLDER:
					case OBJECT_TYPE_IMAGE_FOLDER:
					case OBJECT_TYPE_AUDIO_FOLDER:
							valid_ext = TRUE;
						break;
				}
				if(valid_ext == FALSE)
				{
					TRACE_EVENT("Invalid Extension");
					mmi_fm_app_show_info( win,TxtNoExtension,TxtNull,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_rename_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
				}
				else 
				{
					if( cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_IMAGE&&
						cur_dir->obj_data[cur_dir->currentIndex]->image.thumbnail == TRUE)
					{
						char  t_oldFilename[FM_MAX_OBJ_NAME_LENGTH];
						char  t_newFilename[FM_MAX_OBJ_NAME_LENGTH];

						valid_ext = TRUE;
						strcpy(t_oldFilename,cur_dir->obj_list[cur_dir->currentIndex]->name);
						strcpy(t_newFilename,fm_data->edt_buf);
						mmi_fm_get_filename(t_oldFilename);
						mmi_fm_get_filename(t_newFilename);
						status =mfw_fm_rename_image(cur_dir->dir_path, cur_dir->dir_name,t_oldFilename,t_newFilename,fm_data->source);					
					}
					else
					{
					/* February 8, 2008 DRT:OMAPS00156759 (x0083025) For Unicode Support--> Start*/	
					#ifdef FF_MMI_UNICODE_SUPPORT
						T_WCHAR oldFilename[FM_MAX_DIR_PATH_LENGTH];
						T_WCHAR newFilename[FM_MAX_DIR_PATH_LENGTH];	

						char oldFilename_u8[FM_MAX_DIR_PATH_LENGTH];
						char newFilename_u8[FM_MAX_DIR_PATH_LENGTH];

						sprintf(oldFilename_u8,"%s%s/",cur_dir->dir_path, cur_dir->dir_name);
						sprintf(newFilename_u8,"%s%s/%s",cur_dir->dir_path, cur_dir->dir_name, fm_data->edt_buf);

						// only audio files have unicode support at the moment
						if (fm_data->app != FM_AUDIO)
							strcat(oldFilename_u8, cur_dir->obj_list[cur_dir->currentIndex]->name);
						
						convert_u8_to_unicode(oldFilename_u8, oldFilename);
						convert_u8_to_unicode(newFilename_u8, newFilename);

						if (fm_data->app == FM_AUDIO)
							wstrcat(oldFilename, cur_dir->obj_list[cur_dir->currentIndex]->name_uc);
						
					#else
						char oldFilename[FM_MAX_DIR_PATH_LENGTH];
						char newFilename[FM_MAX_DIR_PATH_LENGTH];	
						sprintf(oldFilename,"%s%s/%s",cur_dir->dir_path, cur_dir->dir_name,cur_dir->obj_list[cur_dir->currentIndex]->name);
						sprintf(newFilename,"%s%s/%s",cur_dir->dir_path, cur_dir->dir_name, fm_data->edt_buf);
					#endif
						status =mfw_fm_rename(oldFilename,newFilename,fm_data->source);
					}
					if(status == FM_NO_ERROR)
					{
						TRACE_EVENT("Rename Done");
						fm_data->list_state = FM_LIST_RECREATE;
						mmi_fm_app_show_info(win,TxtRename,TxtDone,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_rename_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
						
					}			
					else
					{
						TRACE_EVENT("Rename Error");
						//Display the error message to the user
						mmi_fm_app_show_info(win,TxtRename,TxtFailed,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_rename_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
					}
				}
			}
		}
		break;
		case INFO_KCD_RIGHT:
		case INFO_KCD_HUP:	
			mmi_fm_rename_editor_destroy( );
			break;
		default:
			/* otherwise no action to be performed
			*/
			break;	
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_rename_cb

 $Description:		Callback function for rename success/failure dialog

 $Returns:		None

 $Arguments:		win			-current window
				identifier		-window id
				reason		-Event cause.
*******************************************************************************/
void mmi_fm_rename_cb (T_MFW_HND win, USHORT identifier, SHORT reason)
{
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	
	TRACE_FUNCTION ("mmi_fm_rename_cb()");

	mmi_fm_rename_editor_destroy();

	if(fm_data->list_state == FM_LIST_RECREATE)
	{
		//Delete the options window
		if(fm_data->opt_win)
		{
			bookMenuDestroy(fm_data->opt_win);
			fm_data->opt_win = NULL;
		}
		//Delete the image window
		if(fm_data->img_win)
		{
			SEND_EVENT(fm_data->img_win,FM_IMGDRAW_DESTROY,0,(void *)NULL);
		}
		//Populate the list again
		if(fm_data->list_win)
		{
			listsIconsDestroy(fm_data->list_win);
			fm_data->list_win=NULL;
			mmi_fm_list_destroy( fm_data->root_win);
		}
		fm_data->opt_type=FM_RENAME;
		SEND_EVENT (fm_data->root_win, FM_INIT, 0, (void *)NULL);
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_rename_editor_destroy

 $Description:		Frees the allocated memory

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
void mmi_fm_rename_editor_destroy(void)
{
	TRACE_FUNCTION ("mmi_fm_rename_editor_destroy");
	
	mfwFree((U8 *)fm_data->edt_buf, FM_MAX_OBJ_NAME_LENGTH);
}

/*******************************************************************************
 $Function:    		mmi_fm_newdir

 $Description:		Creates an editor for creating a new directory

 $Returns:		Execution status

 $Arguments:		mnu		-Pointer to current menu
 				item 	-Pointer to current menu item
*******************************************************************************/
GLOBAL int mmi_fm_newdir (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfw_parent(mfw_header());

	TRACE_FUNCTION ("mmi_fm_newdir()");
	
	mmi_fm_newdir_editor_create(parent_win);
	
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
 $Function:    		mmi_fm_newdir_editor_create

 $Description:		Creates an editor for creating a new directory

 $Returns:		Execution status

 $Arguments:		parent_win		-Parent window
*******************************************************************************/
T_MFW_HND mmi_fm_newdir_editor_create(T_MFW_HND parent_win)
{
	T_MFW_HND  win;
	T_AUI_EDITOR_DATA	editor_data;

	TRACE_FUNCTION ("mmi_fm_newdir_editor_create");

	fm_data->edt_buf = (char *)mfwAlloc(FM_MAX_OBJ_NAME_LENGTH);

	memset(&editor_data,'\0',sizeof(editor_data));
	/*
	** Initialise the editor
	*/
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII,(UBYTE *) fm_data->edt_buf, FM_MAX_OBJ_NAME_LENGTH);
	AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtDelete, TxtNull, NULL);
	AUI_edit_SetAltTextStr(&editor_data, 1, TxtNull, TRUE, TxtSoftBack);
	AUI_edit_SetEvents(&editor_data, 0, TRUE, FOREVER, (T_AUI_EDIT_CB)mmi_fm_newdir_editor_exec_cb);
	win = AUI_edit_Start(parent_win, &editor_data);
	return win;
}

/*******************************************************************************
 $Function:    		mmi_fm_newdir_editor_exec_cb

 $Description:		Event handler for the editor

 $Returns:		None

 $Arguments:		win			-current window
				identifier		-window id
				reason		-Event cause.
*******************************************************************************/
static void mmi_fm_newdir_editor_exec_cb(T_MFW_HND win, USHORT identifier, SHORT reason)
{
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	T_MFW_FM_STATUS status = FM_NO_ERROR;

	TRACE_FUNCTION ("mmi_fm_newdir_exec_cb()");

	switch (reason)
	{
		case INFO_KCD_LEFT:
			if (fm_data->edt_buf[0] == NULL)
			{
				mmi_fm_app_show_info(win,TxtCreateDir,TxtFailed,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_newdir_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
			}
			else
			{
				char dirname[FM_MAX_DIR_PATH_LENGTH];
				sprintf(dirname,"%s%s/%s",cur_dir->dir_path, cur_dir->dir_name,fm_data->edt_buf);
				status =mfw_fm_newdir(dirname,fm_data->source);
				if(status == FM_DIR_EXISTS)
					mmi_fm_app_show_info(win,TxtDirectory,TxtExists,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_newdir_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
				else
				{
					fm_data->list_state = FM_LIST_RECREATE;
					mmi_fm_app_show_info(win,TxtDirectory,TxtCreated,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_newdir_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
					
				}
			}
			break;
		case INFO_KCD_RIGHT:
		case INFO_KCD_HUP:	
			mmi_fm_newdir_editor_destroy( );
			break;

		default:
			/* otherwise no action to be performed
			*/
			break;	

		
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_newdir_cb

 $Description:		Callback function for new directory create success/failure dialog

 $Returns:		None

 $Arguments:		win			-current window
				identifier		-window id
				reason		-Event cause.
*******************************************************************************/
void mmi_fm_newdir_cb (T_MFW_HND win, USHORT identifier, SHORT reason)
{
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	TRACE_FUNCTION ("mmi_fm_newdir_cb()");

	mmi_fm_rename_editor_destroy();
	if(fm_data->list_state == FM_LIST_RECREATE)
	{
		//Delete the options window
		if(fm_data->opt_win)
		{
			bookMenuDestroy(fm_data->opt_win);
			fm_data->opt_win = NULL;
		}
		//Populate the list again
		if(fm_data->list_win)
		{
			listsIconsDestroy(fm_data->list_win);
			fm_data->list_win=NULL;
			mmi_fm_list_destroy( fm_data->root_win);
		}
		fm_data->opt_type=FM_NEWDIR;
		SEND_EVENT (fm_data->root_win, FM_INIT, 0, (void *)NULL);
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_newdir_editor_destroy

 $Description:		Frees the allocated memory

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
void mmi_fm_newdir_editor_destroy(void)
{
	TRACE_FUNCTION ("mmi_fm_newdir_destroy");
	
	mfwFree((U8 *)fm_data->edt_buf, FM_MAX_OBJ_NAME_LENGTH);
}

/*******************************************************************************
 $Function:    		mmi_fm_copy

 $Description:		Removes the selected object after user confirmation and displays the updated

 $Returns:		Execution status

 $Arguments:		mnu		-Pointer to current menu
 				item 	-Pointer to current menu item
*******************************************************************************/
GLOBAL int mmi_fm_remove (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	
	TRACE_FUNCTION ("mmi_fm_remove()");
	
	mmi_fm_app_show_info(parent_win,TxtDelete,TxtFile,TxtSoftOK,TxtSoftBack,(T_VOID_FUNC)mmi_fm_remove_confirm_cb,FOREVER,KEY_LEFT|KEY_RIGHT | KEY_CLEAR | KEY_HUP);
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
 $Function:    		mmi_fm_remove_confirm_cb

 $Description:		Callback function for remove confirmation dialog

 $Returns:		None

 $Arguments:		win			-current window
				identifier		-window id
				reason		-Event cause.
*******************************************************************************/
void mmi_fm_remove_confirm_cb (T_MFW_HND win, USHORT identifier, SHORT reason)
{
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	T_MFW_FM_STATUS status = FM_NO_ERROR;
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	
	TRACE_FUNCTION ("mmi_fm_remove_confirm_cb()");

	switch(reason)
	{
		case INFO_KCD_LEFT:
			if( cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_IMAGE &&
				cur_dir->obj_data[cur_dir->currentIndex]->image.thumbnail == TRUE)
			{
				char  filename[FM_MAX_OBJ_NAME_LENGTH];
				strcpy(filename,cur_dir->obj_list[cur_dir->currentIndex]->name);
				mmi_fm_get_filename(filename);
				status =mfw_fm_remove_image(cur_dir->dir_path, cur_dir->dir_name,filename,fm_data->source);					
			}
			else
			{
			/* February 8, 2008 DRT:OMAPS00156759 (x0083025) For Unicode Support--> Start*/	
			#ifdef FF_MMI_UNICODE_SUPPORT
				T_WCHAR filename[FM_MAX_DIR_PATH_LENGTH];
				char dirPath[FM_MAX_DIR_PATH_LENGTH];
				sprintf(dirPath, "%s%s/",cur_dir->dir_path, cur_dir->dir_name);
				convert_u8_to_unicode(dirPath, filename);
				wstrcat(filename, cur_dir->obj_list[cur_dir->currentIndex]->name_uc);
			#else
				char filename[FM_MAX_DIR_PATH_LENGTH];
				sprintf(filename,"%s%s/%s",cur_dir->dir_path, cur_dir->dir_name,cur_dir->obj_list[cur_dir->currentIndex]->name);
				
			#endif
			/* February 8, 2008 DRT:OMAPS00156759 (x0083025) For Unicode Support--> End*/	
				status =mfw_fm_remove(filename, fm_data->source);
			}
			switch(status)
			{
				case FM_NO_ERROR:
					fm_data->list_state = FM_LIST_RECREATE;
					mmi_fm_app_show_info(parent_win,TxtDelete,TxtDone,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_remove_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
					
					break;
				case FM_DELETE_DIRNOTEMPTY:
					mmi_fm_app_show_info(parent_win,TxtDirectory,TxtNotEmpty,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_remove_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
					break;
				default:
					//Display the error message to the user
					mmi_fm_app_show_info(parent_win,TxtDelete,TxtFailed,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_remove_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
					break;
			}
			break;
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_remove_cb

 $Description:		Callback function for remove success/failure dialog

 $Returns:		None

 $Arguments:		win			-current window
				identifier		-window id
				reason		-Event cause.
*******************************************************************************/
void mmi_fm_remove_cb (T_MFW_HND win, USHORT identifier, SHORT reason)
{
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	
	TRACE_FUNCTION ("mmi_fm_remove_cb()");

	if(fm_data->list_state == FM_LIST_RECREATE)
	{
		//Delete the options window
		if(fm_data->opt_win)
		{
			TRACE_EVENT("bookMenuDestroy");
			bookMenuDestroy(fm_data->opt_win);
			fm_data->opt_win = NULL;
		}
		//Delete the image window
		if(fm_data->img_win)
		{
			TRACE_EVENT("FM_IMGDRAW_DESTROY");
			SEND_EVENT(fm_data->img_win,FM_IMGDRAW_DESTROY,0,(void *)NULL);
		}
		//Populate the list again
		if(fm_data->list_win)
		{
			listsIconsDestroy(fm_data->list_win);
			fm_data->list_win=NULL;
			mmi_fm_list_destroy( fm_data->root_win);
			TRACE_EVENT("List destroyed");
		}
		fm_data->opt_type=FM_REMOVE;
		SEND_EVENT (fm_data->root_win, FM_INIT, 0, (void *)NULL);
	}
		
}

/*******************************************************************************
 $Function:    		showCopyMove

 $Description:		Shows or hides the copy and move option

 $Returns:		0 - shows the item, 1 - hides the item

 $Arguments:		None
*******************************************************************************/
USHORT showCopyMove( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;

	TRACE_FUNCTION ("showCopyMove()");

	/* Apr 05, 2007 DRT: OMAPS00124859 x0039928 */
	if(cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_FILE||
		cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO ||
		cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_AAC ||
		cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_MIDI ||
		cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_MP3||
		cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_XMF||
		cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_IMY||
		cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_MMF||
		cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_MXMF||
		cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_SMS||
		cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_AUDIO_WAV||
		cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_IMAGE
	)
			return 0;
	else
			return 1;
}

/*******************************************************************************
 $Function:    		showCreate

 $Description:		Shows or hides the create directory option

 $Returns:		0 - shows the item, 1 - hides the item

 $Arguments:		None
*******************************************************************************/
USHORT showCreate( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	TRACE_FUNCTION ("showCreate()");
	
	if (fm_data->opt_type == FM_OPTIONS_FOLDER)
	{
		return 0;
	}
	else
		return 1;
}

/*******************************************************************************
 $Function:    		mmi_fm_copy

 $Description:		Copies the selected object from source to destination

 $Returns:		Execution status

 $Arguments:		mnu		-Pointer to current menu
 				item 	-Pointer to current menu item
*******************************************************************************/
GLOBAL int mmi_fm_copy (MfwMnu* m, MfwMnuItem* i)
{
	TRACE_FUNCTION ("mmi_fm_copy()");
	
	fm_data->opt_type = FM_COPY;
	mmi_fm_displayDestDrive();
	
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
 $Function:    		mmi_fm_displayDestDrive

 $Description:		Display the destination drive

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
void mmi_fm_displayDestDrive(void)
{
	T_MFW_HND parent_win  = mfwParent(mfw_header());
	
	TRACE_FUNCTION ("mmi_fm_displayDestDrive()");
	
	 fm_data->copymove.data = (T_MFW_FM_COPYMOVE_STRUCT *)mfwAlloc(sizeof(T_MFW_FM_COPYMOVE_STRUCT));
	 fm_data->dest_data = (T_MMI_FM_DEST_STRUCT *)mfwAlloc(sizeof(T_MMI_FM_DEST_STRUCT));
	fm_data->dest_data->dest_drive_win=(T_MFW_HND)bookMenuStart( parent_win, FMDestDrivesListAttributes(), 0);
	SEND_EVENT(fm_data->dest_data->dest_drive_win,ADD_CALLBACK,NULL,(void *)mmi_fm_displayDestDrive_cb);

}
/*******************************************************************************
 $Function:    		mmi_fm_displayDestDrive_cb

 $Description:		Callback function for the destination Directory window

 $Returns:		None

 $Arguments:		win			-current window
				event		-window event Id
				value		-unique Id
				parameter	-optional data.
*******************************************************************************/
void mmi_fm_displayDestDrive_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	TRACE_FUNCTION("mmi_fm_dest_root_folders_cb");

	mfwFree((U8 *)fm_data->copymove.data, sizeof(T_MFW_FM_COPYMOVE_STRUCT));
	mfwFree((U8 *)fm_data->dest_data , sizeof (T_MMI_FM_DEST_STRUCT));
	fm_data->dest_data = NULL;
}

/*******************************************************************************
 $Function:    		mmi_fm_dest_root_folders

 $Description:		Display the destination root folders

 $Returns:		Execution status

 $Arguments:		mnu		-Pointer to current menu
 				item 	-Pointer to current menu item
*******************************************************************************/
GLOBAL int  mmi_fm_dest_root_folders(MfwMnu* mnu, MfwMnuItem* item)
{
	T_MFW_HND       parent_win;

	TRACE_FUNCTION("mmi_fm_dest_root_folders" );

	parent_win  = mfwParent(mfw_header());
	fm_data->dest_data->destination= (T_FM_DEVICE_TYPE)mnu->lCursor[mnu->level];
	 fm_data->dest_data->dest_dir = (T_FM_DESTDIR *)mfwAlloc(sizeof(T_FM_DESTDIR));
	fm_data->dest_data->dest_rootfolderlist_win=(T_MFW_HND)bookMenuStart( parent_win, FMDestRootFolderAttributes(), 0);
	SEND_EVENT(fm_data->dest_data->dest_rootfolderlist_win,ADD_CALLBACK,NULL,(void *)mmi_fm_dest_root_folders_cb);
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
 $Function:    		mmi_fm_dest_root_folders_cb

 $Description:		Callback function for the root Image, Audio Directory window

 $Returns:		None

 $Arguments:		win			-current window
				event		-window event Id
				value		-unique Id
				parameter	-optional data.
*******************************************************************************/
void mmi_fm_dest_root_folders_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	TRACE_FUNCTION("mmi_fm_dest_root_folders_cb");

	mfwFree((U8 *)fm_data->dest_data->dest_dir , sizeof (T_FM_DESTDIR));
	fm_data->dest_data->dest_dir = NULL;
}

/*******************************************************************************
 $Function:    		mmi_fm_dest_display

 $Description:		Displays the folders given path

 $Returns:		Execution status

 $Arguments:		mnu		-Pointer to current menu
 				item 	-Pointer to current menu item
*******************************************************************************/
GLOBAL int  mmi_fm_dest_display(MfwMnu* mnu, MfwMnuItem* item)
{
	T_FM_DESTDIR *dest_dir = fm_data->dest_data->dest_dir;

	TRACE_FUNCTION("mmi_fm_dest_display");

	dest_dir->depth = 0;
	dest_dir->dir_type = OBJECT_TYPE_NONE;
	dest_dir->currentIndex = 0;
	dest_dir->num_objs = 0;
	switch(mnu->lCursor[mnu->level])
	{
		case 0:
			fm_data->dest_data->app = FM_IMAGE;
			dest_dir->dir_type=OBJECT_TYPE_IMAGE_FOLDER;
			break;
		case 1:
			fm_data->dest_data->app = FM_AUDIO;
			dest_dir->dir_type=OBJECT_TYPE_AUDIO_FOLDER;
			break;
	}
	//Based on the selected drive, load the root image, audio directory names
	switch(fm_data->dest_data->destination)
	{
		case FM_NOR_FLASH:
			if(dest_dir->dir_type==OBJECT_TYPE_IMAGE_FOLDER)
			{
				strcpy( dest_dir->dir_path,  NORDIR);
				strcpy( dest_dir->dir_name,  NORDIR_IMG);
			}
			else if(dest_dir->dir_type==OBJECT_TYPE_AUDIO_FOLDER)
			{
				strcpy( dest_dir->dir_path,  NORDIR);
				strcpy( dest_dir->dir_name,  NORDIR_AUD);
			}
			break;
		case FM_NORMS_FLASH:
			if(dest_dir->dir_type==OBJECT_TYPE_IMAGE_FOLDER)
			{
				strcpy( dest_dir->dir_path,  NORMSDIR);
				strcpy( dest_dir->dir_name,  NORMSDIR_IMG);
			}
			else if(dest_dir->dir_type==OBJECT_TYPE_AUDIO_FOLDER)
			{
				strcpy( dest_dir->dir_path,  NORMSDIR);
				strcpy( dest_dir->dir_name,  NORMSDIR_AUD);
			}
			break;			
		case FM_NAND_FLASH:
			if(dest_dir->dir_type==OBJECT_TYPE_IMAGE_FOLDER)
			{
				strcpy( dest_dir->dir_path,  NANDDIR);
				strcpy( dest_dir->dir_name,  NANDDIR_IMG);
			}
			else if(dest_dir->dir_type==OBJECT_TYPE_AUDIO_FOLDER)
			{
				strcpy( dest_dir->dir_path,  NANDDIR);
				strcpy( dest_dir->dir_name,  NANDDIR_AUD);
			}
			break;
		case FM_T_FLASH:
			if(dest_dir->dir_type==OBJECT_TYPE_IMAGE_FOLDER)
			{
				strcpy( dest_dir->dir_path,  TFLASHDIR);
				strcpy( dest_dir->dir_name,  TFLASHDIR_IMG);
			}
			else if(dest_dir->dir_type==OBJECT_TYPE_AUDIO_FOLDER)
			{
				strcpy( dest_dir->dir_path,  TFLASHDIR);
				strcpy( dest_dir->dir_name,  TFLASHDIR_AUD);
			}
			break;
	}	
	TRACE_EVENT_P2("Path %s Dir %s", dest_dir->dir_path,dest_dir->dir_name);
	dest_dir->depth = 1;
	mmi_fm_dest_start( fm_data->dest_data->dest_rootfolderlist_win);
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
 $Function:    		mmi_fm_dest_start

 $Description:		To create the destination FileManager window

 $Returns:		Created Window Pointer

 $Arguments:		parent_win	-Parent Window
*******************************************************************************/
static T_MFW_HND mmi_fm_dest_start( T_MFW_HND parent_win)
{
	TRACE_FUNCTION("mmi_fm_dest_start");
	
	//Creating the root window for FileManager
	fm_data->dest_data->dest_root_win=mmi_fm_dest_create(parent_win);
	
       if (fm_data->dest_data->dest_root_win NEQ NULL)
	{
	    SEND_EVENT (fm_data->dest_data->dest_root_win, FM_INIT, 0, (void *)NULL);
	}
	return fm_data->dest_data->dest_root_win;
}

/*******************************************************************************
 $Function:    		mmi_fm_dest_create

 $Description:		To create the destination FileManager window

 $Returns:		Created Window Pointer

 $Arguments:		parent_win	-Parent Window
*******************************************************************************/
static T_MFW_HND mmi_fm_dest_create (T_MFW_HND parent_win)
{

	T_MFW_WIN  * win;
	T_MMI_FM_DEST_STRUCT *dest_data = fm_data->dest_data;

	TRACE_FUNCTION("mmi_fm_dest_create");

	//Creating the root window for FileManager
	dest_data->dest_root_win = win_create (parent_win, 0, E_WIN_VISIBLE, (T_MFW_CB)mmi_fm_dest_win_cb);
	/*
		connect the dialog data to the MFW-window
	*/
	dest_data->mmi_control.dialog = (T_DIALOG_FUNC)mmi_fm_dest_exec_cb;
	dest_data->mmi_control.data   = dest_data;
	dest_data->parent_win= parent_win;	
	win                           = ((T_MFW_HDR *)dest_data->dest_root_win)->data;
	win->user                = (void *)dest_data;
	
	winShow(dest_data->dest_root_win);
	return dest_data->dest_root_win;
}

/*******************************************************************************
 $Function:    		mmi_fm_dest_win_cb

 $Description:	 	Handler for window events for the destination FileManager window

 $Returns:		Execution status

 $Arguments:		e	-window event Id
				w	-current window
*******************************************************************************/
int mmi_fm_dest_win_cb (MfwEvt e, MfwWin *w)
{
	TRACE_FUNCTION ("mmi_fm_dest_win_cb()");
	
	switch (e)
	{
	case MfwWinVisible:  
		break;
	case MfwWinFocussed: 
		break;
	case MfwWinDelete:   
		break;
	default:
		return MFW_EVENT_REJECTED;
	}
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
 $Function:    		mmi_fm_dest_exec_cb

 $Description:	 	Handler for events for the destination FileManager window

 $Returns:		None

 $Arguments:		win			-current window
				event		-window event Id
				value		-unique Id
				parameter	-optional data.
*******************************************************************************/
void mmi_fm_dest_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN        * win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_FM_DEST_STRUCT  * dest_data     = (T_MMI_FM_DEST_STRUCT *)win_data->user;


	TRACE_FUNCTION ("mmi_fm_dest_exec_cb()");
	
	TRACE_EVENT_P1("Event %d",event );
	switch (event)
	{
		case FM_INIT:
			dest_data->dest_list_win=mmi_fm_dest_list_create(dest_data->dest_root_win);
			break;
		case FM_DESTROY:
			//Destroy the current list
			mmi_fm_dest_destroy(dest_data->dest_root_win);
			break;
		default:
			break;
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_dest_destroy

 $Description:	 	To destroy the destination FileManager window

 $Returns:		None

 $Arguments:		own_window	-FileManager window
*******************************************************************************/
static void mmi_fm_dest_destroy(MfwHnd own_window)
{
	T_MMI_FM_DEST_STRUCT *dest_data;

	TRACE_FUNCTION ("mmi_fm_dest_destroy()");
	dest_data = fm_data->dest_data;

	if ( dest_data )
	{
		//Delete the dest_root_win window
		win_delete (dest_data->dest_root_win);
		dest_data->dest_root_win = NULL;
	}
	dspl_Enable(1);
}
/*******************************************************************************
 $Function:    		mmi_fm_dest_list_create

 $Description:	 	Start building the destination list

 $Returns:		Current created window
 
 $Arguments:		win		-Parent window
*******************************************************************************/
T_MFW_HND mmi_fm_dest_list_create( T_MFW_HND win)
{
	T_MMI_FM_DEST_STRUCT *dest_data = fm_data->dest_data;
	T_FM_DESTDIR *dest_dir = fm_data->dest_data->dest_dir;
	USHORT  file_count = 0, list_count=0;

	TRACE_FUNCTION("mmi_fm_dest_list_create" );
	
	//Read the objects in the given directory 
	mfw_fm_readDir(dest_dir->dir_path,dest_dir->dir_name,&dest_dir->num_objs,dest_dir->obj_list,fm_data->dest_data->destination, fm_data->dest_data->app);

	//Read the file/folder 
	mfw_fm_readFileFolders(dest_dir->dir_path,dest_dir->dir_name,dest_dir->num_objs,dest_dir->obj_list,fm_data->dest_data->destination);
	dest_data->dest_menu_list_data = (ListMenuData *)ALLOC_MEMORY(sizeof(ListMenuData));
	if(dest_data->dest_menu_list_data == NULL )
	{
		TRACE_EVENT("Memory failure.");
		return NULL;
	}
	dest_data->dest_menu_list_data->List = (T_MFW_MNU_ITEM *)ALLOC_MEMORY( (dest_dir->num_objs+1) * sizeof(T_MFW_MNU_ITEM) );
	if(dest_data->dest_menu_list_data->List == NULL)
	{
		TRACE_EVENT("Memory failure.");
		return NULL;
	}

	for (file_count=0; file_count<dest_dir->num_objs; file_count++)
	{
		if( dest_dir->obj_list[file_count]->type == OBJECT_TYPE_FOLDER)
		{
			mnuInitDataItem(&dest_data->dest_menu_list_data->List[list_count]);
			dest_data->dest_menu_list_data->List[list_count].str  = (char *)dest_dir->obj_list[file_count]->name;
			dest_data->dest_menu_list_data->List[list_count].flagFunc = item_flag_none;		
			dest_data->dest_menu_list_data->List[list_count].icon=&fmIconFolder;
			list_count++;
		}
	}
	mnuInitDataItem(&dest_data->dest_menu_list_data->List[list_count]);
	if(fm_data->opt_type == FM_COPY)
		dest_data->dest_menu_list_data->List[list_count].str  = "[Copy Here]";
	else
		dest_data->dest_menu_list_data->List[list_count].str  = "[Move Here]";
	dest_data->dest_menu_list_data->List[list_count].flagFunc = item_flag_none;		
	dest_data->dest_menu_list_data->List[list_count].icon=NULL;
	list_count++;
	TRACE_EVENT_P1("Total %d",list_count);
	dest_data->dest_menu_list_data->ListLength = list_count;
	dest_data->dest_menu_list_data->ListPosition = 1;
	dest_data->dest_menu_list_data->CursorPosition = 1;
	if((dest_dir->num_objs+1) < 3)
		dest_data->dest_menu_list_data->SnapshotSize =dest_dir->num_objs+1;// list_count;		
	else
		dest_data->dest_menu_list_data->SnapshotSize =3;// list_count;
	dest_data->dest_menu_list_data->Font = 1;
	if(fm_data->opt_type == FM_COPY)
		dest_data->dest_menu_list_data->LeftSoftKey = TxtCopy;
	else
		dest_data->dest_menu_list_data->LeftSoftKey = TxtMove;
	dest_data->dest_menu_list_data->RightSoftKey = TxtSoftBack;
	dest_data->dest_menu_list_data->KeyEvents = KEY_ALL;
	dest_data->dest_menu_list_data->Reason = 0;
	dest_data->dest_menu_list_data->Strings = TRUE;
	dest_data->dest_menu_list_data->Attr   = (MfwMnuAttr*)&FMList_Attrib;
	dest_data->dest_menu_list_data->autoDestroy    = FALSE;
	dest_data->dest_menu_list_data->block_keys =  (BOOL)FALSE;
	//Create the List window with default file and folder icons
	listIconsDisplayListMenu(win, dest_data->dest_menu_list_data, (ListCbFunc)mmi_fm_dest_list_cb, 0);
	dspl_Enable(1);
	return dest_data->dest_menu_list_data->win;
}

/*******************************************************************************
 $Function:    		mmi_fm_nofile_cb

 $Description:		Callback function for "no files loaded" dialog

 $Returns:		None

 $Arguments:		win			-current window
				identifier		-window id
				reason		-Event cause.
*******************************************************************************/
void mmi_fm_destnofile_cb (T_MFW_HND win, USHORT identifier, SHORT reason)
{
	TRACE_FUNCTION("mmi_fm_nofile_cb" );
	
	mmi_fm_destgoback();
}

/*******************************************************************************
 $Function:    		mmi_fm_dest_list_cb

 $Description:		Call back function for the destination list window

 $Returns:		None

 $Arguments:		Parent		-window handler
 				List Data		-Menu list data
*******************************************************************************/
void mmi_fm_dest_list_cb(T_MFW_HND * Parent, ListMenuData * ListData)
{
	T_MFW_WIN       * win_data;
	T_MMI_FM_DEST_STRUCT* dest_data;
	T_FM_DESTDIR *dest_dir;
	char curDir[FM_MAX_DIR_PATH_LENGTH];

	TRACE_FUNCTION("mmi_fm_dest_list_cb" );
       
	win_data = ((T_MFW_HDR *)Parent)->data;
	dest_data = (T_MMI_FM_DEST_STRUCT*)win_data->user;
	dest_dir = dest_data->dest_dir;
	TRACE_EVENT_P3("Reason %d length %d cur %d",ListData->Reason,ListData->ListLength , ListData->CursorPosition); 
	switch(ListData->Reason)
	{
		case LISTS_REASON_SELECT:
			if(ListData->CursorPosition == (ListData->ListLength-1) )
			{ //Selecting the [Copy here] item
				mmi_fm_copy_create(NULL);
			}
			else
			{
				dest_dir->currentIndex=ListData->CursorPosition;
				if(ListData->selectKey==TRUE)
				{
					//Update the current directory name with the selected directory
					if(dest_dir->dir_name[0]!='\0')
					{
						sprintf(curDir,"%s%s/",dest_dir->dir_path,dest_dir->dir_name);
						strcpy(dest_dir->dir_path,curDir);
						strcpy(dest_dir->dir_name , dest_data->dest_menu_list_data->List[dest_dir->currentIndex].str);
					}
					//Destroy the current list
					if(dest_data->dest_list_win)
					{
						listsIconsDestroy(dest_data->dest_list_win);
						dest_data->dest_list_win=NULL;
						mmi_fm_dest_list_destroy( dest_data->dest_root_win);
					}
					//Populate the new list
					SEND_EVENT (dest_data->dest_root_win, FM_INIT, 0, (void *)NULL);
				}
				else
				{
					mmi_fm_copy_create(dest_data->dest_menu_list_data->List[dest_dir->currentIndex].str);
				}
			}
			break;
		case LISTS_REASON_BACK:	
		case LISTS_REASON_HANGUP: /* Apr 10, 2007 DRT: OMAPS00125325 x0039928 */
			mmi_fm_destgoback();
			break;
		case LISTS_REASON_DRAWCOMPLETE:
			if(fm_data->opt_type == FM_COPY)
				displaySoftKeys(TxtCopy,TxtSoftBack);
			else
				displaySoftKeys(TxtMove,TxtSoftBack);
			break;
				
	}
}
/*******************************************************************************
 $Function:    		mmi_fm_destgoback

 $Description:		returns to the previous directory. 

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
void mmi_fm_destgoback(void)
{
	char *strTempPtr;
	T_FM_DESTDIR *dest_dir = fm_data->dest_data->dest_dir;
	T_MMI_FM_DEST_STRUCT* dest_data = fm_data->dest_data;
	
	TRACE_FUNCTION("mmi_fm_destgoback");
	
	dest_dir->depth--;
	mmi_fm_set_mmi_plane_to_front();
	if( dest_dir->depth <= 0 )
	{
		//Root directory, exit the list window
		TRACE_EVENT("Root directory");
		if(dest_data->dest_list_win)
		{
			listsIconsDestroy( dest_data->dest_list_win);
			 dest_data->dest_list_win=NULL;
			mmi_fm_dest_list_destroy(  dest_data->dest_root_win );
		}
		SEND_EVENT ( dest_data->dest_root_win, FM_DESTROY, 0, (void *)NULL);
		winIconsLists = fm_data->list_win;
	}
	else
	{
		
		//Extract the previous directory path 
		strTempPtr=(char*) &dest_dir->dir_path + strlen(dest_dir->dir_path)-1;
		*strTempPtr='\0';
		strTempPtr--;
		while( *strTempPtr != '/'  )
		{
			strTempPtr = strTempPtr - 1;
		}
		strTempPtr++;
		strcpy(dest_dir->dir_name,strTempPtr);
		*strTempPtr='\0';
		
		//Destroy the current list
		if( dest_data->dest_list_win)
		{
			listsIconsDestroy( dest_data->dest_list_win);
			 dest_data->dest_list_win=NULL;
			mmi_fm_dest_list_destroy( dest_data->dest_root_win );
		}
		//Populate the new list
		SEND_EVENT ( dest_data->dest_root_win, FM_INIT, 0, (void *)NULL);
		return;
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_dest_list_destroy

 $Description:	 	Frees the memory allocated during destination list creation

 $Returns:		None

 $Arguments:		own_window	-FileManager window
*******************************************************************************/
static void mmi_fm_dest_list_destroy(MfwHnd own_window)
{
	T_MFW_WIN * win_data;
	T_MMI_FM_DEST_STRUCT* dest_data;
	int i=0;
	T_FM_DESTDIR *dest_dir;
	
	TRACE_FUNCTION ("mmi_fm_dest_list_destroy()");

	if (own_window)
	{
		win_data = ((T_MFW_HDR *)own_window)->data;
		dest_data = (T_MMI_FM_DEST_STRUCT *)win_data->user;
		dest_dir = dest_data->dest_dir;
		if ( dest_data )
		{
			if (dest_data->dest_menu_list_data != NULL)
			{
				mfwFree( (U8 *)dest_data->dest_menu_list_data->List,(dest_dir->num_objs+1) * sizeof(T_MFW_MNU_ITEM) );
				mfwFree ((U8 *)dest_data->dest_menu_list_data, sizeof(ListMenuData));
			}
			i=0;
			while(dest_dir->obj_list[i] != NULL)
			{
				mfwFree((U8 *)dest_dir->obj_list[i], sizeof(T_FM_OBJ));
				dest_dir->obj_list[i] = NULL;
				i++;
			}
			
		}
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_copy_create

 $Description:	 	To start copying the file

 $Returns:		None

 $Arguments:		dest_folder - destination folder name
*******************************************************************************/
void mmi_fm_copy_create(char * dest_folder)
{
	T_FM_DESTDIR *dest_dir = fm_data->dest_data->dest_dir;
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	T_MMI_FM_DEST_STRUCT* dest_data =fm_data->dest_data;
	
	TRACE_FUNCTION ("mmi_fm_copy_create()");
	
	//User has selected the destination.
/* February 8, 2008 DRT:OMAPS00156759 (x0083025) For Unicode Support--> Start*/	
#ifdef FF_MMI_UNICODE_SUPPORT
	fm_data->copymove.data->sourceFile= (T_WCHAR *)mfwAlloc(FM_MAX_DIR_PATH_LENGTH);
	fm_data->copymove.data->destinationFile = (T_WCHAR *)mfwAlloc(FM_MAX_DIR_PATH_LENGTH);

	{
		char destFolder[FM_MAX_DIR_PATH_LENGTH];
		char sourceFolder[FM_MAX_DIR_PATH_LENGTH];
		T_WCHAR *dest, *source;
		
		dest = fm_data->copymove.data->destinationFile;
		source = fm_data->copymove.data->sourceFile;
		
		if(dest_folder == NULL)
			sprintf(destFolder,"%s%s/",dest_dir->dir_path,dest_dir->dir_name);
		else
			sprintf(destFolder,"%s%s/%s/",dest_dir->dir_path,dest_dir->dir_name,dest_folder);

		sprintf(sourceFolder,"%s%s/",cur_dir->dir_path, cur_dir->dir_name);

		// unicode support is there for audio only at the moment
		if (fm_data->app == FM_AUDIO)
		{
			//destination file
			convert_u8_to_unicode(destFolder, dest);
			wstrcat(dest, cur_dir->obj_list[cur_dir->currentIndex]->name_uc);

			//source file
			convert_u8_to_unicode(sourceFolder, source);
			wstrcat(source, cur_dir->obj_list[cur_dir->currentIndex]->name_uc);
		}
		else
		{
			//destination file
			strcat(destFolder, cur_dir->obj_list[cur_dir->currentIndex]->name);
			convert_u8_to_unicode(destFolder, dest);

			//source file
			strcat(sourceFolder, cur_dir->obj_list[cur_dir->currentIndex]->name);
			convert_u8_to_unicode(sourceFolder, source);
		}
	}		
#else
	 fm_data->copymove.data->sourceFile= (char *)mfwAlloc(FM_MAX_DIR_PATH_LENGTH);
	 fm_data->copymove.data->destinationFile = (char *)mfwAlloc(FM_MAX_DIR_PATH_LENGTH);
	 if(dest_folder == NULL)
		 sprintf( fm_data->copymove.data->destinationFile,"%s%s/%s",dest_dir->dir_path,dest_dir->dir_name,cur_dir->obj_list[cur_dir->currentIndex]->name);
	 else
		 sprintf( fm_data->copymove.data->destinationFile,"%s%s/%s/%s",dest_dir->dir_path,dest_dir->dir_name,dest_folder,cur_dir->obj_list[cur_dir->currentIndex]->name);
	 sprintf( fm_data->copymove.data->sourceFile,"%s%s/%s",cur_dir->dir_path, cur_dir->dir_name,cur_dir->obj_list[cur_dir->currentIndex]->name);
#endif
/* February 8, 2008 DRT:OMAPS00156759 (x0083025) For Unicode Support--> End*/	
	TRACE_EVENT_P2("Source %s Destination %s", fm_data->copymove.data->sourceFile, fm_data->copymove.data->destinationFile);
	if(dest_data->dest_list_win)
	{
		listsIconsDestroy(dest_data->dest_list_win);
		dest_data->dest_list_win=NULL;
		mmi_fm_dest_list_destroy( dest_data->dest_root_win );
		winIconsLists = fm_data->list_win;
	}
	SEND_EVENT (dest_data->dest_root_win, FM_DESTROY, 0, (void *)NULL);
	//Destroy the destinatin root folder list
	bookMenuDestroy(dest_data->dest_rootfolderlist_win);
	dest_data->dest_rootfolderlist_win = NULL;
	//Destroy the destination drive list
	bookMenuDestroy(dest_data->dest_drive_win);
	dest_data->dest_drive_win = NULL;
	//Free the buffer allocated for destination directory lists
	mfwFree((U8 *)dest_data->dest_dir , sizeof (T_FM_DESTDIR));
	dest_data->dest_dir = NULL;
	/* Mar 15, 2007 DRT: OMAPS00120225 x0039928 */
	/* Fix: Condition added to check source and destination device are same  */
/* February 8, 2008 DRT:OMAPS00156759 (x0083025) For Unicode Support--> Start*/	
#ifdef FF_MMI_UNICODE_SUPPORT
	if((wstrcmp(fm_data->copymove.data->sourceFile,fm_data->copymove.data->destinationFile) == 0) 
#else
	if((strcmp(fm_data->copymove.data->sourceFile,fm_data->copymove.data->destinationFile) == 0) 
#endif
/* February 8, 2008 DRT:OMAPS00156759 (x0083025) For Unicode Support--> End*/	
				&& (fm_data->source == fm_data->dest_data->destination))
	{
			mmi_fm_app_show_info(0,TxtSourceDest,TxtSame,TxtNull,TxtSoftBack,0,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
			mmi_fm_copy_destroy(fm_data->copymove.data->state);
	}
	else
	{
		mmi_fm_copy_start();
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_copy_start

 $Description:	 	To start copying the file

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
void mmi_fm_copy_start(void)
{
	T_MFW_HND parent_win  = mfwParent(mfw_header());
	TRACE_FUNCTION ("mmi_fm_copy_start()");
	
	fm_data->copymove.data->source_type = fm_data->source;
	fm_data->copymove.data->sourceFileSize = fm_data->cur_dir->obj_properties[fm_data->cur_dir->currentIndex]->size;
	fm_data->copymove.data->destination_type = fm_data->dest_data->destination;
	fm_data->copymove.data->buf = (char *)mfwAlloc(FM_COPY_BUFFER_SIZE);
	fm_data->copymove.data->state = mfw_fm_copy_start(fm_data->copymove.data);
	if(fm_data->copymove.data->state == FM_COPY_PROGRESS)
	{
		fm_data->copymove.win = mmi_fm_copy_dialog_create(parent_win);
		SEND_EVENT(fm_data->copymove.win, FM_COPY_INIT, 0, (void *)NULL);
		fm_data->copymove.timer = timCreate( 0, FM_COPYMOVE_TIMER, (MfwCb)mmi_fm_copy_timer_cb );
		timStart(fm_data->copymove.timer) ;
	}
	else if(fm_data->copymove.data->state == FM_COPY_DONE ||fm_data->copymove.data->state == FM_COPY_ERROR||fm_data->copymove.data->state == FM_COPY_NONE)//April 27, 2007 DRT:OMAPS00128836 x0073106
	{
		mmi_fm_copy_end();
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_copy_dialog_create

 $Description:		Creates a new window for showing the progress of copying

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
T_MFW_HND mmi_fm_copy_dialog_create(T_MFW_HND parent_win)
{
	T_MMI_FM_COPY_STRUCT *copy_data;
	T_MFW_WIN  * win;

	TRACE_FUNCTION("mmi_fm_copy_dialog_create");
	
	copy_data =  (T_MMI_FM_COPY_STRUCT *)mfwAlloc(sizeof(T_MMI_FM_COPY_STRUCT));

	//Create a window for drawing the image in QCIF width and height
	copy_data->win = win_create (fm_data->list_win, 0, E_WIN_VISIBLE|E_WIN_SUSPEND|E_WIN_RESUME|E_WIN_DELETE, (T_MFW_CB)mmi_fm_copy_dialog_win_cb);

	copy_data ->mmi_control.dialog = (T_DIALOG_FUNC)mmi_fm_copy_dialog_exec_cb;
	copy_data ->mmi_control.data   = copy_data ;
	copy_data->parent_win= fm_data->list_win;	
	win                      = (T_MFW_WIN *)getWinData(copy_data->win);
	win->user                = (MfwUserDataPtr)copy_data;
       copy_data->kbd = kbd_create (copy_data->win,KEY_ALL|KEY_MAKE,(T_MFW_CB)mmi_fm_copy_dialog_kbd_cb);
	winShow(copy_data->win );	
	return copy_data->win ;
}

/*******************************************************************************
 $Function:   		mmi_fm_copy_dialog_kbd_cb

 $Description:	 	Handler for key events for the copy progress window

 $Returns:		Execution status

 $Arguments:		e	-window event Id
				k	-key event
*******************************************************************************/
static int mmi_fm_copy_dialog_kbd_cb (MfwEvt e, MfwKbd *k)
{
	TRACE_FUNCTION("mmi_fm_copy_dialog_kbd_cb");

	if (!(e & KEY_MAKE))
	{
		return MFW_EVENT_CONSUMED;
	}

	switch (k->code)
	{
		case KCD_LEFT:
			break;
		case KCD_RIGHT:
		case KCD_HUP:
			break;
	}
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
 $Function:    		mmi_fm_copy_dialog_win_cb

 $Description:	 	Handler for window events for the copy progress window

 $Returns:		Execution status

 $Arguments:		e	-window event Id
				w	-current window
*******************************************************************************/
int mmi_fm_copy_dialog_win_cb (MfwEvt e, MfwWin *w)
{
	T_MMI_FM_COPY_STRUCT  * copy_data     = (T_MMI_FM_COPY_STRUCT *)w->user;

	TRACE_FUNCTION ("mmi_fm_copy_dialog_win_cb()");
	
	TRACE_EVENT_P1("event %d",e);
	switch (e)
	{
	case MfwWinVisible:  /* window is visible  */
		SEND_EVENT(copy_data->win, FM_COPY_DRAW, 0, (void *)NULL);
		break;
	case MfwWinSuspend: /* input focus / selected   */
		SEND_EVENT(copy_data->win, FM_COPY_SUSPEND, 0, (void *)NULL);
		break;
	case MfwWinResume: /* input focus / selected   */
		SEND_EVENT(copy_data->win, FM_COPY_RESUME, 0, (void *)NULL);
		break;
	case MfwWinDelete:   /* window will be deleted   */
		break;
	default:
		return MFW_EVENT_REJECTED;
	}
	return MFW_EVENT_CONSUMED;

}

/*******************************************************************************
 $Function:    		mmi_fm_copy_dialog_exec_cb

 $Description:	 	Eevnt handler copy progress window 

 $Returns:		None

 $Arguments:		win			-current window
				event		-window event Id
				value		-unique Id
				parameter	-optional data.
*******************************************************************************/
void mmi_fm_copy_dialog_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	int x1, y1, x2, y2;
	int txtId1= TxtPleaseWait, txtId2 = TxtCopying; 
	char * str1, *str2;
	
	TRACE_FUNCTION("mmi_fm_copy_dialog_exec_cb");
	
	TRACE_EVENT_P1("event %d",event);
	switch (event)
	{ 
		case FM_COPY_INIT:
			break;
		case FM_COPY_DRAW:
			dspl_ClearAll(); 
			switch(fm_data->copymove.data->state)
			{
				case FM_COPY_START:
				case FM_COPY_PROGRESS:
					txtId1 = TxtPleaseWait;
					if(fm_data->opt_type == FM_COPY)
						txtId2 = TxtCopying;
					else
						txtId2 = TxtMove;
					break;
				case FM_COPY_DONE:
					break;
			}
			str1 = MmiRsrcGetText(txtId1);
			str2 = MmiRsrcGetText(txtId2);
			x1= (SCREEN_SIZE_X/2) - (dspl_GetTextExtent(str1,0) / 2) ;
			y1= (SCREEN_SIZE_Y -Mmi_layout_softkeyHeight())/2 - Mmi_layout_line_height();
			x2= (SCREEN_SIZE_X/2) - (dspl_GetTextExtent(str2,0) / 2) ;
			y2= y1+Mmi_layout_line_height();
			dspl_TextOut(x1,y1,DSPL_TXTATTR_NORMAL,str1);
			dspl_TextOut(x2,y2,DSPL_TXTATTR_NORMAL,str2);
			displaySoftKeys(TxtNull,TxtSoftBack);
			dspl_Enable(1);	
			break;
		case FM_COPY_SUSPEND:
			TRACE_EVENT("Suspend");
			timStop(fm_data->copymove.timer) ;
			break;
		case FM_COPY_RESUME:
			TRACE_EVENT("Resume");
			timStart(fm_data->copymove.timer) ;
			break;
		case FM_COPY_DESTROY:
			mmi_fm_copy_dialog_destroy();
			break;
	}
 }

/*******************************************************************************
 $Function:    		mmi_fm_copy_dialog_destroy

 $Description:		Destroys the copy progress window and releases the allocated memory for the internal structure

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
void mmi_fm_copy_dialog_destroy(void)
{
	T_MFW_HND    win  = fm_data->copymove.win;//mfwParent(mfw_header());
	T_MFW_WIN  * win_data = ((T_MFW_HDR *)win)->data;
	T_MMI_FM_COPY_STRUCT      * copy_data = (T_MMI_FM_COPY_STRUCT *)win_data->user;
	TRACE_FUNCTION("mmi_fm_copy_dialog_destroy");
	mmi_fm_set_mmi_plane_to_front();
	winDelete(copy_data->win);
	mfwFree((U8 *)copy_data, sizeof(T_MMI_FM_COPY_STRUCT));
}

/*******************************************************************************
 $Function:    		mmi_fm_copy_end

 $Description:		Displays the result of Copy or Move operation 

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
void mmi_fm_copy_end(void)
{
	TRACE_FUNCTION ("mmi_fm_copy_end()");
	
	if(fm_data->opt_type == FM_COPY)
	{
		if(fm_data->copymove.data->state == FM_COPY_DONE) 
		{
			fm_data->list_state = FM_LIST_RECREATE;
			mmi_fm_app_show_info(0,TxtCopy,TxtDone,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_copy_end_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
		}
		else if (fm_data->copymove.data->state == FM_COPY_ERROR)
			mmi_fm_app_show_info(0,TxtCopy,TxtError,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_copy_end_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
		else if (fm_data->copymove.data->state == FM_COPY_NONE)//April 27, 2007 DRT:OMAPS00128836 x0073106
		{
		mmi_fm_app_show_info(0,TxtFile,TxtExists,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_copy_end_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
		}

	}
	else
	{
		if(fm_data->copymove.data->state == FM_COPY_DONE) 
		{
			mfw_fm_remove(fm_data->copymove.data->sourceFile, fm_data->source);
			fm_data->list_state = FM_LIST_RECREATE;
			mmi_fm_app_show_info(0,TxtMove,TxtDone,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_copy_end_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
		}
		else if (fm_data->copymove.data->state == FM_COPY_ERROR)
			mmi_fm_app_show_info(0,TxtMove,TxtError,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_copy_end_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);		
		else if (fm_data->copymove.data->state == FM_COPY_NONE)//April 27, 2007 DRT:OMAPS00128836 x0073106
		{
		mmi_fm_app_show_info(0,TxtFile,TxtFailed,TxtNull,TxtSoftBack,(T_VOID_FUNC)mmi_fm_copy_end_cb,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
		}
	}
	mfwFree((U8 *)fm_data->copymove.data->buf, FM_COPY_BUFFER_SIZE);
	
}

/*******************************************************************************
 $Function:    		mmi_fm_copy_end_cb

 $Description:		Callback function for copy info dialog

 $Returns:		None

 $Arguments:		win			-current window
				identifier		-window id
				reason		-Event cause.
*******************************************************************************/
void mmi_fm_copy_end_cb (T_MFW_HND win, USHORT identifier, SHORT reason)
{
	TRACE_EVENT_P1("state %d",fm_data->copymove.data->state );
	mmi_fm_copy_destroy(fm_data->copymove.data->state);
}

/*******************************************************************************
 $Function:    		mmi_fm_copy_destroy

 $Description:		Destroy the source list and repopulates 

 $Returns:		None

 $Arguments:		status	- Status of Copy or Move operation
*******************************************************************************/
void mmi_fm_copy_destroy(T_FM_COPY_STATE status)
{
	T_MMI_FM_DEST_STRUCT* dest_data = fm_data->dest_data;
	
	TRACE_FUNCTION ("mmi_fm_copy_destroy()");
	
	mfwFree((U8 *)fm_data->copymove.data->sourceFile, FM_MAX_DIR_PATH_LENGTH);
	mfwFree((U8 *)fm_data->copymove.data->destinationFile, FM_MAX_DIR_PATH_LENGTH);
	mfwFree((U8 *)fm_data->copymove.data, sizeof(T_MFW_FM_COPYMOVE_STRUCT));
	mfwFree((U8 *)dest_data , sizeof (T_MMI_FM_DEST_STRUCT));
	fm_data->dest_data = NULL;
	TRACE_EVENT_P1("state %d",status );
	if(fm_data->list_state == FM_LIST_RECREATE)
	{
			//Delete the options window
		if(fm_data->opt_win)
		{
			TRACE_EVENT("bookMenuDestroy");
			bookMenuDestroy(fm_data->opt_win);
			fm_data->opt_win = NULL;
		}
		//Populate the list again
		if(fm_data->list_win)
		{
			listsIconsDestroy(fm_data->list_win);
			fm_data->list_win=NULL;
			mmi_fm_list_destroy( fm_data->root_win);
			TRACE_EVENT("List destroyed");
		}
		SEND_EVENT (fm_data->root_win, FM_INIT, 0, (void *)NULL);
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_copy_timer_cb

 $Description:		Timer callback 

 $Returns:		None

 $Arguments:		e	-window event Id
 				m	- Timer Handle
*******************************************************************************/
int mmi_fm_copy_timer_cb ( MfwEvt e, MfwTim *m )
{
	TRACE_FUNCTION ("mmi_fm_copy_timer_cb()");
	fm_data->copymove.data->state = mfw_fm_copy_continue(fm_data->copymove.data);
	TRACE_EVENT_P1("state %d",fm_data->copymove.data->state );
	if(fm_data->copymove.data->state == FM_COPY_PROGRESS)
	{
		timStart(fm_data->copymove.timer) ;
	}
	else if(fm_data->copymove.data->state == FM_COPY_DONE || fm_data->copymove.data->state == FM_COPY_ERROR)
	{
		timDelete(fm_data->copymove.timer);
		fm_data->copymove.timer= NULL;
		SEND_EVENT(fm_data->copymove.win, FM_COPY_DESTROY,0, (void *)NULL);
		mmi_fm_copy_end();
	}
	   return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
 $Function:    		mmi_fm_move

 $Description:		Moves the selected object from source to destination

 $Returns:		Execution status

 $Arguments:		mnu		-Pointer to current menu
 				item 	-Pointer to current menu item
*******************************************************************************/
GLOBAL int mmi_fm_move (MfwMnu* m, MfwMnuItem* i)
{
	TRACE_FUNCTION ("mmi_fm_move()");
	fm_data->opt_type = FM_MOVE;
	mmi_fm_displayDestDrive();
	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************
 $Function:    		mmi_fm_properties

 $Description:		Display the selected object properties

 $Returns:		Execution status

 $Arguments:		mnu		-Pointer to current menu
 				item 	-Pointer to current menu item
*******************************************************************************/
GLOBAL int mmi_fm_properties(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	
	TRACE_FUNCTION ("mmi_fm_properties()");	

	mmi_fm_properties_editor_create(parent_win);
	
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
 $Function:    		mmi_fm_properties_editor_create

 $Description:		Creates an editor for creating a new directory

 $Returns:		Execution status

 $Arguments:		parent_win		-Parent window
*******************************************************************************/
T_MFW_HND mmi_fm_properties_editor_create(T_MFW_HND parent_win)
{
	T_MFW_HND  win;
	T_AUI_EDITOR_DATA	editor_data;
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
/*#ifdef FF_MMI_UNICODE_SUPPORT	
	T_WCHAR name[FM_MAX_OBJ_NAME_LENGTH];
#else
	char name[FM_MAX_OBJ_NAME_LENGTH];
#endif*/
	char size[FM_MAX_OBJ_NAME_LENGTH];
	char date[FM_MAX_OBJ_NAME_LENGTH];
	char time[FM_MAX_OBJ_NAME_LENGTH];
	
	TRACE_FUNCTION ("mmi_fm_newdir_editor_create");

	fm_data->edt_buf = (char *)mfwAlloc(FM_MAX_DETAILS_BUFFER);

	memset(&editor_data,'\0',sizeof(editor_data));
 	/*
	** Initialise the editor
	*/
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII,(UBYTE *) fm_data->edt_buf, 100);
	AUI_edit_SetMode(&editor_data,ED_MODE_ALPHA,0);
	AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtNull, TxtDetails, NULL);
	AUI_edit_SetAltTextStr(&editor_data, 1, TxtNull, TRUE, TxtNull);
	AUI_edit_SetEvents(&editor_data, 0, TRUE, FOREVER, (T_AUI_EDIT_CB)mmi_fm_properties_editor_exec_cb);
	win = AUI_edit_Start(parent_win, &editor_data);
	//Name
	AUI_edit_InsertString(win , (UBYTE *) "Name : ",ATB_DCS_ASCII);

#ifdef FF_MMI_UNICODE_SUPPORT
	//sprintf(name," : %s",cur_dir->obj_list[cur_dir->currentIndex]->name);
	if (fm_data->app == FM_AUDIO)
	{
		char *name;
		int size= (wstrlen(cur_dir->obj_list[cur_dir->currentIndex]->name_uc)+1)*2;
		name = (char *)mfwAlloc(size);
		
		unicode_to_char_array(cur_dir->obj_list[cur_dir->currentIndex]->name_uc, name);
		AUI_edit_InsertString(win , (UBYTE *)name,ATB_DCS_UNICODE);
		
		mfwFree(name, size);
	}
	else
#endif
	{
		//sprintf(name," : %s",cur_dir->obj_list[cur_dir->currentIndex]->name);
		AUI_edit_InsertString(win , (UBYTE *)cur_dir->obj_list[cur_dir->currentIndex]->name,ATB_DCS_ASCII);
	}
	
	AUI_edit_InsertChar(win , ctrlEnter);
	//size
	AUI_edit_InsertString(win , (UBYTE *) "Size",ATB_DCS_ASCII);
	sprintf(size," : %d bytes",cur_dir->obj_properties[cur_dir->currentIndex]->size);
	AUI_edit_InsertString(win , (UBYTE *)size,ATB_DCS_ASCII);
	AUI_edit_InsertChar(win , ctrlEnter);
	TRACE_EVENT("Date");
	//Date
	AUI_edit_InsertString(win , (UBYTE *) "Date",ATB_DCS_ASCII);
	sprintf(date," : %s",cur_dir->obj_properties[cur_dir->currentIndex]->date);
	AUI_edit_InsertString(win , (UBYTE *)date,ATB_DCS_ASCII);
	AUI_edit_InsertChar(win , ctrlEnter);
	//Time
	AUI_edit_InsertString(win , (UBYTE *) "Time",ATB_DCS_ASCII);
	sprintf(time," : %s",cur_dir->obj_properties[cur_dir->currentIndex]->time);
	AUI_edit_InsertString(win , (UBYTE *)time,ATB_DCS_ASCII);
	AUI_edit_InsertChar(win , ctrlEnter);
	
	AUI_edit_SetMode(&editor_data,ED_MODE_READONLY,0);
	win_show(win);
	return win;
}

/*******************************************************************************
 $Function:    		mmi_fm_properties_editor_exec_cb

 $Description:		Event handler for the editor
 
 $Returns:		None

 $Arguments:		win			-current window
				identifier		-window id
				reason		-Event cause.
*******************************************************************************/
static void mmi_fm_properties_editor_exec_cb(T_MFW_HND win, USHORT identifier, SHORT reason)
{
	TRACE_FUNCTION ("mmi_fm_properties_editor_exec_cb()");

	switch (reason)
	{
		case INFO_KCD_LEFT:
		case INFO_KCD_RIGHT:
		case INFO_KCD_HUP:	
			mmi_fm_properties_editor_destroy( );
			break;
		default:
			/* otherwise no action to be performed
			*/
			break;	
		
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_properties_editor_destroy

 $Description:		Frees the allocated memory

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
void mmi_fm_properties_editor_destroy(void)
{
	TRACE_FUNCTION ("mmi_fm_properties_editor_destroy");
	
	mfwFree((U8 *)fm_data->edt_buf, FM_MAX_DETAILS_BUFFER);
}

/*******************************************************************************
 $Function:    		mmi_fm_memStat

 $Description:		Display the selected drive's free, used space

 $Returns:		Execution status

 $Arguments:		mnu		-Pointer to current menu
 				item 	-Pointer to current menu item
*******************************************************************************/
GLOBAL int mmi_fm_memStat(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	
	TRACE_FUNCTION ("mmi_fm_memStat()");	
	
	mfw_fm_getMemStat(&fm_data->nFreeBytes, &fm_data->nUsedBytes,fm_data->source);
	mmi_fm_memStat_editor_create(parent_win);
	return MFW_EVENT_CONSUMED;	
}

/*******************************************************************************
 $Function:    		mmi_fm_memStat_editor_create

 $Description:		Creates an editor for creating a new directory

 $Returns:		Execution status

 $Arguments:		parent_win		-Parent window
*******************************************************************************/
T_MFW_HND mmi_fm_memStat_editor_create(T_MFW_HND parent_win)
{
	T_MFW_HND  win;
	T_AUI_EDITOR_DATA	editor_data;
	char free[FM_MAX_OBJ_NAME_LENGTH];
	char used[FM_MAX_OBJ_NAME_LENGTH];
	char total[FM_MAX_OBJ_NAME_LENGTH];
	
	TRACE_FUNCTION ("mmi_fm_newdir_editor_create");

	fm_data->edt_buf = (char *)mfwAlloc(FM_MAX_DETAILS_BUFFER);

	memset(&editor_data,'\0',sizeof(editor_data));
 	/*
	** Initialise the editor
	*/
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII,(UBYTE *) fm_data->edt_buf, 100);
	AUI_edit_SetMode(&editor_data,ED_MODE_ALPHA,0);
	AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtNull, TxtMemorystat, NULL);
	AUI_edit_SetAltTextStr(&editor_data, 1, TxtNull, TRUE, TxtNull);
	AUI_edit_SetEvents(&editor_data, 0, TRUE, FOREVER, (T_AUI_EDIT_CB)mmi_fm_memStat_editor_exec_cb);
	win = AUI_edit_Start(parent_win, &editor_data);

	AUI_edit_InsertString(win , (UBYTE *) "Used",ATB_DCS_ASCII);
	sprintf(used," :%d KB ",(fm_data->nUsedBytes));
	AUI_edit_InsertString(win , (UBYTE *)used,ATB_DCS_ASCII);
	AUI_edit_InsertChar(win , ctrlEnter);
	AUI_edit_InsertChar(win , ctrlEnter);
	
	AUI_edit_InsertString(win , (UBYTE *) "Free",ATB_DCS_ASCII);
	sprintf(free," :%d KB ",fm_data->nFreeBytes);
	AUI_edit_InsertString(win , (UBYTE *)free,ATB_DCS_ASCII);
	AUI_edit_InsertChar(win , ctrlEnter);
	AUI_edit_InsertChar(win , ctrlEnter);
	
	AUI_edit_InsertString(win , (UBYTE *) "Total",ATB_DCS_ASCII);
	sprintf(total," :%d KB ",(fm_data->nFreeBytes+fm_data->nUsedBytes));
	AUI_edit_InsertString(win , (UBYTE *)total,ATB_DCS_ASCII);
	AUI_edit_InsertChar(win , ctrlEnter);
	
	AUI_edit_SetMode(&editor_data,ED_MODE_READONLY, ED_CURSOR_NONE);
	win_show(win);
	return win;
}

/*******************************************************************************
 $Function:    		mmi_fm_memStat_editor_exec_cb

 $Description:		Event handler for the editor
 
 $Returns:		None

 $Arguments:		win			-current window
				identifier		-window id
				reason		-Event cause.
*******************************************************************************/
static void mmi_fm_memStat_editor_exec_cb(T_MFW_HND win, USHORT identifier, SHORT reason)
{
	TRACE_FUNCTION ("mmi_fm_memStat_editor_exec_cb()");

	switch (reason)
	{
		case INFO_KCD_LEFT:
		case INFO_KCD_RIGHT:
		case INFO_KCD_HUP:	
			mmi_fm_memStat_editor_destroy( );
			break;
		default:
			/* otherwise no action to be performed
			*/
			break;	

		
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_memStat_editor_destroy

 $Description:		Frees the allocated memory

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
void mmi_fm_memStat_editor_destroy(void)
{
	TRACE_FUNCTION ("mmi_fm_properties_editor_destroy");
	
	mfwFree((U8 *)fm_data->edt_buf, FM_MAX_DETAILS_BUFFER);
}

/*******************************************************************************
 $Function:    		mmi_fm_format

 $Description:	 	Formats the seleted drive

 $Returns:		Execution status

 $Arguments:		mnu		-Pointer to current menu
 				item 	-Pointer to current menu item
*******************************************************************************/
GLOBAL int mmi_fm_format(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	
	TRACE_FUNCTION ("mmi_fm_format()");	
	
	if( fm_data->source == FM_NOR_FLASH)
		mmi_fm_app_show_info(parent_win,TxtFormat,TxtNotAllowed,TxtNull,TxtSoftBack,0,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
	else
		mmi_fm_app_show_info(parent_win,TxtFormat,TxtNull,TxtSoftOK,TxtSoftBack,(T_VOID_FUNC)mmi_fm_format_confirm_cb,FOREVER,KEY_LEFT|KEY_RIGHT | KEY_CLEAR | KEY_HUP);
	return MFW_EVENT_CONSUMED;	
}

USHORT check_source_setting( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	if((fm_data->app == FM_AUDIO) && (fm_data->cur_dir->dirinfo.aud.state == FM_AUD_NONE))	
		return 0;
	else
		return 1;
}

USHORT check_source_audio( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	if(fm_data->app == FM_AUDIO)	
		return 0;
	else
		return 1;
}

USHORT check_source_image( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	if(fm_data->app == FM_IMAGE)	
		return 0;
	else
		return 1;
}

USHORT format_check_dev( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	if(fm_data->source == FM_NOR_FLASH)	
		return 1;
	else
		return 0;
}
USHORT check_dev( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{

char curDir[10];
UINT16 curDir_uc[10];
UINT16 objName_uc[10];
char objName[10];
T_RFS_DIR f_dir;
			
			strcpy(curDir, "/");
			convert_u8_to_unicode(curDir, curDir_uc);

			if (rfs_opendir (curDir_uc, &f_dir) < 0)
			{
				TRACE_EVENT_P1("Opening dir %s Failed",curDir);
				return FM_DIR_DOESNOTEXISTS; 
			}

			while(rfs_readdir (&f_dir, objName_uc, FM_MAX_OBJ_NAME_LENGTH) > 0x0 )
			{
				if (objName_uc[0]  == '\0')
				{
					return FM_READDIR_ERROR;
				}

				convert_unicode_to_u8(objName_uc, objName);
				
				if(strcmp(objName,"MMC")==0)
				{
					rfs_closedir(&f_dir);
					return 0;
				}
			}    
			rfs_closedir(&f_dir);
  return 1;
}

/*******************************************************************************
 $Function:    		mmi_fm_format_confirm_cb

 $Description:		Callback function for remove confirmation dialog

 $Returns:		None

 $Arguments:		win			-current window
				identifier		-window id
				reason		-Event cause.
*******************************************************************************/
void mmi_fm_format_confirm_cb (T_MFW_HND win, USHORT identifier, SHORT reason)
{
	T_MFW_FM_STATUS status = FM_NO_ERROR;
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	
	TRACE_FUNCTION ("mmi_fm_format_confirm_cb()");

	switch(reason)
	{
		case INFO_KCD_LEFT:
			status =mfw_fm_format( fm_data->source);
			switch(status)
			{
				case FM_NO_ERROR:
					mmi_fm_app_show_info(parent_win,TxtFormat,TxtDone,TxtNull,TxtSoftBack,0,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
					break;
				case FM_FORMAT_ERROR:
					mmi_fm_app_show_info(parent_win,TxtFormat,TxtError,TxtNull,TxtSoftBack,0,THREE_SECS,KEY_RIGHT | KEY_CLEAR | KEY_HUP);
					break;
			}
			break;
	}
	
}

/*******************************************************************************
 $Function:    		mmi_fm_get_filename

 $Description:	 	Extracts the filename discarding the extension

 $Returns:		None

 $Arguments:		filename		-Pointer to filename
******************************************************************************/
void mmi_fm_get_filename( char * filename)
{
	char* temp;
	int s;

	TRACE_FUNCTION("mmi_fm_img_getfilename");
	
	temp=(char *)strchr(filename,'.');
	if(filename && temp)
	{
		s = temp -filename;
		filename[s] ='\0';
	}
	return;
}

/*******************************************************************************
 $Function:    		mmi_fm_get_ext

 $Description:	 	Extracts the extension discarding the filename

 $Returns:		Extention

 $Arguments:		filename	-Pointer to filename
******************************************************************************/
char * mmi_fm_get_ext( char * filename)
{
	char * ext;
	
	TRACE_FUNCTION("mmi_fm_img_getext");
	
	ext=(char *)strchr(filename,'.');
	if(ext)
		ext++;
	return ext;
}

/*******************************************************************************
 $Function:    		mmi_fm_imgThmbGen_identifyFile

 $Description:	 	Identifies the valid image file for which the thumbnail is not yet generated
				Index is updated in cur_dir->currentIndex
				
 $Returns:		None

 $Arguments:		None
******************************************************************************/
void mmi_fm_imgThmbGen_identifyFile()
{
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	
	TRACE_FUNCTION ("mmi_fm_imgThmbGen_identifyFile()");
	
	while(cur_dir->currentIndex < cur_dir->num_objs)
	{
		if(   cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_IMAGE &&
			cur_dir->obj_data[cur_dir->currentIndex]->image.thumbnail == FALSE )
		{
			break;
		}
		cur_dir->currentIndex++;
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_imgThmbGen_start

 $Description:	 	Starts the thumbnail generation UCP
				
 $Returns:		None

 $Arguments:		None
******************************************************************************/
void mmi_fm_imgThmbGen_start(void)
{
	T_FM_IMG_STATE result = FM_UCP_NO_ERROR;
	char  currFilePath[FM_MAX_DIR_PATH_LENGTH];
	char  curFilename[FM_MAX_OBJ_NAME_LENGTH];
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;

	TRACE_FUNCTION ("mmi_fm_imgThmbGen_start()");

	cur_dir->dirinfo.img.ipMsl = aci_create(mslPrimHandler,NULL);	 
	//cur_dir->currentIndex would contain the index to the valid file for thumbnail generation
	mmi_fm_imgThmbGen_identifyFile();
	//No files for thumbnail generation or
	if( cur_dir->currentIndex < cur_dir->num_objs )
	{		
		displayMenuKeys(MENU_KEY_WAIT);
		fm_data->menu_list_data->block_keys =  (BOOL)TRUE;
		strcpy(curFilename,cur_dir->obj_list[cur_dir->currentIndex ]->name);
		mmi_fm_get_filename(curFilename);
	 	sprintf(currFilePath,"%s%s/",cur_dir->dir_path,cur_dir->dir_name);
		cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
		cur_dir->dirinfo.img.state = FM_UCP_NONE;
		result = mfw_fm_imgThmb_create();
		if(result == FM_UCP_NO_ERROR)
		{
			result = mfw_fm_imgThmb_setparams((char *)currFilePath,curFilename,FM_MFW_QCIF_WIDTH,FM_MFW_QCIF_HEIGHT, fm_data->source);
			if(result  == FM_UCP_NO_ERROR)
			{
				result = mfw_fm_imgThmb_setcallback();
				if(result  == FM_UCP_NO_ERROR)
				{
					cur_dir->dirinfo.img.cmd = FM_UCP_THMB_INIT;
					cur_dir->dirinfo.img.state = FM_UCP_NONE;
					result = mfw_fm_imgThmb_init();
					if(result  != FM_UCP_NO_ERROR)
					{
						//Error initializing thumbnail UCP
						TRACE_ERROR("Error initializing thumbnail UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgThmb_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the thumbnail UCP");
					}
				}
				else
				{
					//Error setting callback for thumbnail UCP
					TRACE_ERROR("Error setting callback for thumbnail UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
					cur_dir->dirinfo.img.state = FM_UCP_NONE;
					result = mfw_fm_imgThmb_destroy();
					if(result != FM_UCP_NO_ERROR)
						TRACE_ERROR("Error destroying the thumbnail UCP");
				}
			}
			else
			{
				//Error configuring thumbnail UCP
				TRACE_ERROR("Error configuring thumbnail UCP");
				cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgThmb_destroy();
				if(result != FM_UCP_NO_ERROR)
					TRACE_ERROR("Error destroying the thumbnail UCP");
			}		
		}
		else
		{
			//Error creating thumbnail UCP
			TRACE_ERROR("Error creating thumbnail UCP");
		}
	}
	else
	{
		//No files for thumbnail generation
		cur_dir->currentIndex=cur_dir->startIndex;
		mmi_fm_imgThmbDraw_start();
	}			

}
/*******************************************************************************
 $Function:    		mmi_fm_imgThmbGen_resume

 $Description:	 	Starts the thumbnail generation UCP
				
 $Returns:		None

 $Arguments:		None
******************************************************************************/

void mmi_fm_imgThmbGen_resume(void)
{
	T_FM_IMG_STATE result = FM_UCP_NO_ERROR;
	char  currFilePath[FM_MAX_DIR_PATH_LENGTH];
	char  curFilename[FM_MAX_OBJ_NAME_LENGTH];
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;

	TRACE_FUNCTION ("mmi_fm_imgThmbGen_resume()");
	
	cur_dir->currentIndex++;

	//cur_dir->currentIndex would contain the index to the valid file for thumbnail generation
	mmi_fm_imgThmbGen_identifyFile();
	
	if(cur_dir->currentIndex < cur_dir->num_objs )
	{
		displayMenuKeys(MENU_KEY_WAIT);
		fm_data->menu_list_data->block_keys =  (BOOL)TRUE;
		strcpy(curFilename,cur_dir->obj_list[cur_dir->currentIndex ]->name);
		mmi_fm_get_filename(curFilename);
	 	sprintf(currFilePath,"%s%s/",cur_dir->dir_path,cur_dir->dir_name);
		cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
		cur_dir->dirinfo.img.state = FM_UCP_NONE;
		result = mfw_fm_imgThmb_setparams((char *)currFilePath,curFilename,FM_MFW_QCIF_WIDTH,FM_MFW_QCIF_HEIGHT, fm_data->source);
		if(result  == FM_UCP_NO_ERROR)
		{
			result = mfw_fm_imgThmb_setcallback();
			if(result  == FM_UCP_NO_ERROR)
			{
				cur_dir->dirinfo.img.cmd = FM_UCP_THMB_INIT;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgThmb_init();
				if(result  != FM_UCP_NO_ERROR)
				{
					//Error initializing thumbnail UCP
					TRACE_ERROR("Error initializing thumbnail UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
					cur_dir->dirinfo.img.state = FM_UCP_NONE;
					result = mfw_fm_imgThmb_destroy();
					if(result != FM_UCP_NO_ERROR)
						TRACE_ERROR("Error destroying the thumbnail UCP");
				}
			}
			else
			{
				//Error setting callback for thumbnail UCP
				TRACE_ERROR("Error setting callback for thumbnail UCP");
				cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgThmb_destroy();
				if(result != FM_UCP_NO_ERROR)
					TRACE_ERROR("Error destroying the thumbnail UCP");
			}
		}
		else
		{
			//Error configuring thumbnail UCP
			TRACE_ERROR("Error configuring thumbnail UCP");
			cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
			cur_dir->dirinfo.img.state = FM_UCP_NONE;
			result = mfw_fm_imgThmb_destroy();
			if(result != FM_UCP_NO_ERROR)
				TRACE_ERROR("Error destroying the thumbnail UCP");
		}		
	}
	else
	{
		//No files for thumbnail generation
		cur_dir->currentIndex=cur_dir->startIndex;
		cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
		cur_dir->dirinfo.img.state = FM_UCP_NONE;
		result = mfw_fm_imgThmb_destroy();
		if(result != FM_UCP_NO_ERROR)
			TRACE_ERROR("Error destroying the thumbnail UCP");
		mmi_fm_imgThmbDraw_start();
	}
}
/*******************************************************************************
 $Function:    		mmi_fm_imgThmbGen_start

 $Description:	 	Generates thumbnail
				
 $Returns:		None

 $Arguments:		None
******************************************************************************/
void mmi_fm_imgThmbGen(void)
{
	char  currFilePath[FM_MAX_DIR_PATH_LENGTH];
	char  curFilename[FM_MAX_OBJ_NAME_LENGTH];
	T_FM_IMG_STATE result = FM_UCP_NO_ERROR;
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;

	TRACE_FUNCTION ("mmi_fm_imgThmbGen()");
	
	cur_dir->currentIndex++;
	//cur_dir->currentIndex would contain the index to the valid file for thumbnail generation
	mmi_fm_imgThmbGen_identifyFile();
	if(cur_dir->currentIndex < cur_dir->num_objs)
	{
		displayMenuKeys(MENU_KEY_WAIT);
		fm_data->menu_list_data->block_keys =  (BOOL)TRUE;
		strcpy(curFilename,cur_dir->obj_list[cur_dir->currentIndex]->name);
		mmi_fm_get_filename(curFilename);
	 	sprintf(currFilePath,"%s%s/",cur_dir->dir_path,cur_dir->dir_name);
		result= mfw_fm_imgThmb_setparams((char *)currFilePath,curFilename,FM_MFW_QCIF_WIDTH,FM_MFW_QCIF_HEIGHT,fm_data->source);
		if(result == FM_UCP_NO_ERROR)
		{
			cur_dir->dirinfo.img.cmd=FM_UCP_THMB_GEN;
			cur_dir->dirinfo.img.state=FM_UCP_NONE;
			result = mfw_fm_imgThmb_generate();
			if(result != FM_UCP_NO_ERROR)
			{
				//Error Generating thumbnails
				//Deinitialize the Thumbnail UCP and destroy it
				TRACE_ERROR("Error generating thumbnails");
				cur_dir->dirinfo.img.cmd=FM_UCP_THMB_DEINIT;
				cur_dir->dirinfo.img.state=FM_UCP_ERROR;
				result = mfw_fm_imgThmb_deinit();
				if(result != FM_UCP_NO_ERROR)
				{
					TRACE_ERROR("Error deinitializing the thumbnail UCP");
					cur_dir->dirinfo.img.cmd=FM_UCP_NONE;
					cur_dir->dirinfo.img.state=FM_UCP_NONE;
					result = mfw_fm_imgThmb_destroy();
					if(result != FM_UCP_NO_ERROR)
						TRACE_ERROR("Error destroying the thumbnail UCP");
				}
			}
		}
		else
		{
			//Error configuring thumbnail UCP
			//Deinitialize the Thumbnail UCP and destroy it
			TRACE_ERROR("Error configuring thumbnail UCP");
			cur_dir->dirinfo.img.cmd=FM_UCP_THMB_DEINIT;
			cur_dir->dirinfo.img.state=FM_UCP_ERROR;
			result = mfw_fm_imgThmb_deinit();
			if(result != FM_UCP_NO_ERROR)
			{
				TRACE_ERROR("Error deinitializing the thumbnail UCP");
				cur_dir->dirinfo.img.cmd=FM_UCP_NONE;
				cur_dir->dirinfo.img.state=FM_UCP_NONE;
				result = mfw_fm_imgThmb_destroy();
				if(result != FM_UCP_NO_ERROR)
					TRACE_ERROR("Error destroying the thumbnail UCP");
			}			
		}
	}
	else
	{
		//No more thumbnails to genearte
		//Deinitialize the thumbnail UCP and start the Viewer UCP for drawing the thumbnails
		TRACE_EVENT("Thumbnail generation complete");
		cur_dir->dirinfo.img.cmd=FM_UCP_THMB_DEINIT;
		cur_dir->dirinfo.img.state=FM_UCP_NONE;
		result = mfw_fm_imgThmb_deinit();
		if(result != FM_UCP_NO_ERROR)
		{
			TRACE_ERROR("Error deinitializing the thumbnail UCP");
			cur_dir->dirinfo.img.cmd=FM_UCP_NONE;
			cur_dir->dirinfo.img.state=FM_UCP_NONE;
			result = mfw_fm_imgThmb_destroy();
			if(result != FM_UCP_NO_ERROR)
				TRACE_ERROR("Error destroying the thumbnail UCP");
		}		
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_imgThmbDraw_identifyFile

 $Description:	 	Identifies the valid image file which has thumbnail
				Index is updated in cur_dir->currentIndex
				
 $Returns:		None

 $Arguments:		None
******************************************************************************/
void mmi_fm_imgThmbDraw_identifyFile()
{
	T_FM_CURDIR *cur_dir =  fm_data->cur_dir;

	TRACE_FUNCTION ("mmi_fm_imgThmbDraw_identifyFile()");

	while( (cur_dir->currentIndex < cur_dir->num_objs) )
	{
		if(   cur_dir->obj_list[cur_dir->currentIndex]->type == OBJECT_TYPE_IMAGE &&
			cur_dir->obj_data[cur_dir->currentIndex]->image.thumbnail == TRUE )
		{
			break;
		}
		cur_dir->currentIndex++;
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_imgThmbDraw_start

 $Description:	 	Starts the image display UCP
				
 $Returns:		None

 $Arguments:		None
******************************************************************************/
void mmi_fm_imgThmbDraw_start(void)	
{
	MfwMnuItem *menu_item;
	T_FM_IMG_STATE result = FM_UCP_NO_ERROR;
	char  currFilePath[FM_MAX_DIR_PATH_LENGTH];
	char  curFilename[FM_MAX_OBJ_NAME_LENGTH];
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	int x_offset=0, y_offset;
	
	TRACE_FUNCTION ("mmi_fm_imgThmbDraw_start()");
	if(!cur_dir->dirinfo.img.ipMsl)
		cur_dir->dirinfo.img.ipMsl = aci_create(mslPrimHandler,NULL);	 
	

	//cur_dir->currentIndex would contain the index to the valid file for drawing the thumbnail
	mmi_fm_imgThmbDraw_identifyFile();
	if( ((cur_dir->currentIndex - cur_dir->startIndex )< fm_data->menu_list_data->SnapshotSize) 
		&&   cur_dir->currentIndex < cur_dir->num_objs)
	{
		displayMenuKeys(MENU_KEY_WAIT);
		fm_data->menu_list_data->block_keys =  (BOOL)TRUE;
		menu_item = fm_data->menu_list_data->List;
		y_offset=menu_item[0].icon->area.py+ ((cur_dir->currentIndex % 3)  * fm_data->lineheight );
		if(cur_dir->dirinfo.img.cmd == FM_UCP_NONE && cur_dir->dirinfo.img.state == FM_UCP_NONE)
		{
			//Viewer UCP is not yet created, so create it and then draw the thumbnails
			result = mfw_fm_imgViewer_create();
			if(result == FM_UCP_NO_ERROR)
			{
				strcpy(curFilename,cur_dir->obj_list[cur_dir->currentIndex ]->name);
				mmi_fm_get_filename(curFilename);
			 	sprintf(currFilePath,"%s%s/",cur_dir->dir_path,cur_dir->dir_name);
				result = mfw_fm_imgViewer_setImageProperty((char *)currFilePath,curFilename,FM_MFW_PREVIEW_WIDTH,FM_MFW_PREVIEW_HEIGHT,x_offset,y_offset, fm_data->source);			
				if(result == FM_UCP_NO_ERROR)
				{
					result = mfw_fm_imgViewer_setcallback();
					if(result == FM_UCP_NO_ERROR)
					{
						cur_dir->dirinfo.img.cmd = FM_UCP_THMBIMG_INIT;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_init();
						if(result != FM_UCP_NO_ERROR)
						{
							//Error initializing the Viewer UCP, destroy it
							TRACE_ERROR("Error initializing the Viewer UCP");
							cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
							cur_dir->dirinfo.img.state = FM_UCP_NONE;
							result = mfw_fm_imgViewer_destroy();
							if(result != FM_UCP_NO_ERROR)
								TRACE_ERROR("Error destroying the Viewer UCP");
						}
					}
					else
					{
						//Error configuring the callback for Viewer UCP, Destroy it
						TRACE_ERROR("Error configuring the callback for Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
				}
				else
				{
					//Error configuring the Viewer UCP, Destroy it
					TRACE_ERROR("Error configuring the Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
					cur_dir->dirinfo.img.state = FM_UCP_NONE;
					result = mfw_fm_imgViewer_destroy();
					if(result != FM_UCP_NO_ERROR)
						TRACE_ERROR("Error destroying the Viewer UCP");
				}
			}
			else
			{
				//Error creating Viewer UCP
				TRACE_ERROR("Error creating Viewer UCP");
			}		
		}
		else if( (cur_dir->dirinfo.img.cmd == FM_UCP_THMBIMG_DRAW || cur_dir->dirinfo.img.cmd == FM_UCP_QCIFIMG_DRAW) && 
			cur_dir->dirinfo.img.state == FM_UCP_NO_ERROR)
		{
			//Viewer UCP is already created, so draw the thumbnails
			strcpy(curFilename,cur_dir->obj_list[cur_dir->currentIndex ]->name);
			mmi_fm_get_filename(curFilename);
		 	sprintf(currFilePath,"%s%s/",cur_dir->dir_path,cur_dir->dir_name);
			result = mfw_fm_imgViewer_setImageProperty((char *)currFilePath,curFilename,FM_MFW_PREVIEW_WIDTH,FM_MFW_PREVIEW_HEIGHT,x_offset,y_offset, fm_data->source);			
			if(result == FM_UCP_NO_ERROR)
			{
				cur_dir->dirinfo.img.cmd=FM_UCP_THMBIMG_DRAW;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgViewer_view();
				if(result != FM_UCP_NO_ERROR)
				{
					//Error drawing thumbnail in the Viewer UCP, deinit and destroy it
					TRACE_ERROR("Error drawing thumbnail in the Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_THMBIMG_DEINIT;
					cur_dir->dirinfo.img.state = FM_UCP_ERROR;
					result = mfw_fm_imgViewer_deinit();
					if(result != FM_UCP_NO_ERROR)
					{
						//Error deinitializing the Viewer UCP, destroy it
						TRACE_ERROR("Error deinitializing the Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
				}
				
			}
			else
			{
				//Error configuring the Viewer UCP, Destroy it
				TRACE_ERROR("Error configuring the Viewer UCP");
				cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgViewer_destroy();
				if(result != FM_UCP_NO_ERROR)
					TRACE_ERROR("Error destroying the Viewer UCP");
			}
		}
		else
		{
			TRACE_EVENT("UCP not valid state");
			displayMenuKeys(MENU_KEY_UP_DOWN_CENTER);
			fm_data->menu_list_data->block_keys = (BOOL)FALSE;
		}		
	}
	else
	{
		//No valid images to draw
		displayMenuKeys(MENU_KEY_UP_DOWN_CENTER);
		fm_data->menu_list_data->block_keys =  (BOOL)FALSE;
		TRACE_EVENT("No Files to draw");
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_imgThmbDraw_resume

 $Description:	 	Starts the image display UCP
				
 $Returns:		None

 $Arguments:		None
******************************************************************************/
void mmi_fm_imgThmbDraw_resume(void)	
{
	MfwMnuItem *menu_item;
	T_FM_IMG_STATE result = FM_UCP_NO_ERROR;
	char  currFilePath[FM_MAX_DIR_PATH_LENGTH];
	char  curFilename[FM_MAX_OBJ_NAME_LENGTH];
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	int x_offset=0, y_offset;
	
	TRACE_FUNCTION ("mmi_fm_imgThmbDraw_resume()");
	
	cur_dir->currentIndex++;
	
	//cur_dir->currentIndex would contain the index to the valid file for drawing the thumbnail
	mmi_fm_imgThmbDraw_identifyFile();
	if( ((cur_dir->currentIndex - cur_dir->startIndex )< fm_data->menu_list_data->SnapshotSize) 
		&&   cur_dir->currentIndex < cur_dir->num_objs)
	{
		displayMenuKeys(MENU_KEY_WAIT);
		fm_data->menu_list_data->block_keys =  (BOOL)TRUE;
		menu_item = fm_data->menu_list_data->List;
		y_offset=menu_item[0].icon->area.py+ ((cur_dir->currentIndex % 3)  * fm_data->lineheight );
		strcpy(curFilename,cur_dir->obj_list[cur_dir->currentIndex ]->name);
		mmi_fm_get_filename(curFilename);
	 	sprintf(currFilePath,"%s%s/",cur_dir->dir_path,cur_dir->dir_name);
		result = mfw_fm_imgViewer_setImageProperty((char *)currFilePath,curFilename,FM_MFW_PREVIEW_WIDTH,FM_MFW_PREVIEW_HEIGHT,x_offset,y_offset, fm_data->source);			
		if(result == FM_UCP_NO_ERROR)
		{
			result = mfw_fm_imgViewer_setcallback();
			if(result == FM_UCP_NO_ERROR)
			{
				cur_dir->dirinfo.img.cmd = FM_UCP_THMBIMG_INIT;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgViewer_init();
				if(result != FM_UCP_NO_ERROR)
				{
					//Error initializing the Viewer UCP, destroy it
					TRACE_ERROR("Error initializing the Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
					cur_dir->dirinfo.img.state = FM_UCP_NONE;
					result = mfw_fm_imgViewer_destroy();
					if(result != FM_UCP_NO_ERROR)
						TRACE_ERROR("Error destroying the Viewer UCP");
				}
			}
			else
			{
				//Error configuring the callback for Viewer UCP, Destroy it
				TRACE_ERROR("Error configuring the callback for Viewer UCP");
				cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgViewer_destroy();
				if(result != FM_UCP_NO_ERROR)
					TRACE_ERROR("Error destroying the Viewer UCP");
			}
		}
		else
		{
			//Error configuring the Viewer UCP, Destroy it
			TRACE_ERROR("Error configuring the Viewer UCP");
			cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
			cur_dir->dirinfo.img.state = FM_UCP_NONE;
			result = mfw_fm_imgViewer_destroy();
			if(result != FM_UCP_NO_ERROR)
				TRACE_ERROR("Error destroying the Viewer UCP");
		}	
	}
	else
	{
		//No valid images to draw
		displayMenuKeys(MENU_KEY_UP_DOWN_CENTER);
		fm_data->menu_list_data->block_keys =  (BOOL)FALSE;
		TRACE_EVENT("No Files to draw");
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_imgThmbGen_start

 $Description:	 	Drawa thumbnails 48 X 48
				
 $Returns:		None

 $Arguments:		None
******************************************************************************/
void mmi_fm_imgThmbDraw(void)
{
	MfwMnuItem *menu_item;
	char  currFilePath[FM_MAX_DIR_PATH_LENGTH];
	char  curFilename[FM_MAX_OBJ_NAME_LENGTH];
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	int x_offset=0, y_offset;
	T_FM_IMG_STATE result = FM_UCP_NO_ERROR;
	
	TRACE_FUNCTION ("mmi_fm_imgThmbDraw()");
	
	mmi_fm_imgThmbDraw_identifyFile();
	if( ((cur_dir->currentIndex - cur_dir->startIndex )< fm_data->menu_list_data->SnapshotSize) 
		&&   cur_dir->currentIndex < cur_dir->num_objs)
	{
		TRACE_EVENT("Inside if");
	 	displayMenuKeys(MENU_KEY_WAIT);
		fm_data->menu_list_data->block_keys = (BOOL)TRUE;
		menu_item = fm_data->menu_list_data->List;
		y_offset=menu_item[0].icon->area.py + ((cur_dir->currentIndex % 3)  * fm_data->lineheight );
		if( (cur_dir->dirinfo.img.cmd == FM_UCP_THMBIMG_DRAW || cur_dir->dirinfo.img.cmd == FM_UCP_QCIFIMG_DRAW) 
			&& cur_dir->dirinfo.img.state == FM_UCP_NO_ERROR)
		{
			//Viewer UCP is already created, so draw the thumbnails
			strcpy(curFilename,cur_dir->obj_list[cur_dir->currentIndex ]->name);
			mmi_fm_get_filename(curFilename);
		 	sprintf(currFilePath,"%s%s/",cur_dir->dir_path,cur_dir->dir_name);
			result = mfw_fm_imgViewer_setImageProperty((char *)currFilePath,curFilename,FM_MFW_PREVIEW_WIDTH,FM_MFW_PREVIEW_HEIGHT,x_offset,y_offset,fm_data->source);			
			if(result == FM_UCP_NO_ERROR)			
			{
				cur_dir->dirinfo.img.cmd=FM_UCP_THMBIMG_DRAW;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgViewer_view();
				if(result != FM_UCP_NO_ERROR)
				{
					//Error drawing thumbnail in the Viewer UCP, deinit and destroy it
					TRACE_ERROR("Error drawing thumbnail in the Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_THMBIMG_DEINIT;
					cur_dir->dirinfo.img.state = FM_UCP_ERROR;
					result = mfw_fm_imgViewer_deinit();
					if(result != FM_UCP_NO_ERROR)
					{
						//Error deinitializing the Viewer UCP, destroy it
						TRACE_ERROR("Error deinitializing the Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
				}
			}
			else
			{
				//Error configuring the Viewer UCP, Destroy it
				TRACE_ERROR("Error configuring the Viewer UCP");
				cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgViewer_destroy();
				if(result != FM_UCP_NO_ERROR)
					TRACE_ERROR("Error destroying the Viewer UCP");
			}
		}
		else if (cur_dir->dirinfo.img.cmd == FM_UCP_NONE && cur_dir->dirinfo.img.state == FM_UCP_NONE)
		{
			//Viewer UCP is not yet created, so create it and then draw the thumbnails
			result = mfw_fm_imgViewer_create();
			if(result == FM_UCP_NO_ERROR)
			{
				strcpy(curFilename,cur_dir->obj_list[cur_dir->currentIndex ]->name);
				mmi_fm_get_filename(curFilename);
			 	sprintf(currFilePath,"%s%s/",cur_dir->dir_path,cur_dir->dir_name);
				result = mfw_fm_imgViewer_setImageProperty((char *)currFilePath,curFilename,FM_MFW_PREVIEW_WIDTH,FM_MFW_PREVIEW_HEIGHT,x_offset,y_offset, fm_data->source);			
				if(result == FM_UCP_NO_ERROR)			
				{
					result = mfw_fm_imgViewer_setcallback();
					if(result == FM_UCP_NO_ERROR)
					{
						cur_dir->dirinfo.img.cmd = FM_UCP_THMBIMG_INIT;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_init();
						if(result != FM_UCP_NO_ERROR)
						{
							//Error initializing the Viewer UCP, destroy it
							TRACE_ERROR("Error initializing the Viewer UCP");
							cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
							cur_dir->dirinfo.img.state = FM_UCP_NONE;
							result = mfw_fm_imgViewer_destroy();
							if(result != FM_UCP_NO_ERROR)
								TRACE_ERROR("Error destroying the Viewer UCP");
						}
					}
					else
					{
						//Error configuring the callback for Viewer UCP, Destroy it
						TRACE_ERROR("Error configuring the callback for Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
				}
				else
				{
					//Error configuring the Viewer UCP, Destroy it
					TRACE_ERROR("Error configuring the Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
					cur_dir->dirinfo.img.state = FM_UCP_NONE;
					result = mfw_fm_imgViewer_destroy();
					if(result != FM_UCP_NO_ERROR)
						TRACE_ERROR("Error destroying the Viewer UCP");
				}
			}
			else
			{
				//Error creating Viewer UCP
				TRACE_ERROR("Error creating Viewer UCP");
			}		
		}
		else
		{
			TRACE_EVENT("UCP not valid state");
			displayMenuKeys(MENU_KEY_UP_DOWN_CENTER);
			fm_data->menu_list_data->block_keys = (BOOL)FALSE;
		}
	}
	else
	{
		displayMenuKeys(MENU_KEY_UP_DOWN_CENTER);
		fm_data->menu_list_data->block_keys =  (BOOL)FALSE;
		TRACE_EVENT("No Valid File to draw");
	}
}

/*******************************************************************************
 $Function:    		mmi_fm_imgThmbGen_start

 $Description:	 	Drawa images in QCIF width and height
				
 $Returns:		None

 $Arguments:		None
******************************************************************************/
void mmi_fm_imgDraw(void)
{
	char  currFilePath[FM_MAX_DIR_PATH_LENGTH];
	char  curFilename[FM_MAX_OBJ_NAME_LENGTH];
	T_FM_CURDIR *cur_dir = fm_data->cur_dir;
	T_FM_IMG_STATE result = FM_UCP_NO_ERROR;

	TRACE_FUNCTION ("mmi_fm_imgDraw()");
	
	 if( (cur_dir->dirinfo.img.cmd == FM_UCP_THMBIMG_DRAW || cur_dir->dirinfo.img.cmd == FM_UCP_QCIFIMG_DRAW ) && 
	 	cur_dir->dirinfo.img.state == FM_UCP_NO_ERROR)
	{
		//Viewer UCP is already created, so draw the image
		strcpy(curFilename,cur_dir->obj_list[cur_dir->currentIndex ]->name);
		mmi_fm_get_filename(curFilename);
		sprintf(currFilePath,"%s%s/",cur_dir->dir_path,cur_dir->dir_name);
		result = mfw_fm_imgViewer_setImageProperty((char *)currFilePath,curFilename,FM_MFW_QCIF_WIDTH,FM_MFW_QCIF_HEIGHT,0,0,fm_data->source);			
		if(result == FM_UCP_NO_ERROR)		
		{
			cur_dir->dirinfo.img.cmd=FM_UCP_QCIFIMG_DRAW;
			cur_dir->dirinfo.img.state = FM_UCP_NONE;  
			result = mfw_fm_imgViewer_view();
			if(result != FM_UCP_NO_ERROR)
			{
				//Error drawing thumbnail in the Viewer UCP, deinit and destroy it
				TRACE_ERROR("Error drawing thumbnail in the Viewer UCP");
				cur_dir->dirinfo.img.cmd = FM_UCP_QCIFIMG_DEINIT;
				cur_dir->dirinfo.img.state = FM_UCP_ERROR;
				result = mfw_fm_imgViewer_deinit();
				if(result != FM_UCP_NO_ERROR)
				{
					//Error deinitializing the Viewer UCP, destroy it
					TRACE_ERROR("Error deinitializing the Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
					cur_dir->dirinfo.img.state = FM_UCP_NONE;
					result = mfw_fm_imgViewer_destroy();
					if(result != FM_UCP_NO_ERROR)
						TRACE_ERROR("Error destroying the Viewer UCP");
				}
			}
		}
		else
		{
			//Error configuring the Viewer UCP, deinit and destroy it
			TRACE_ERROR("Error configuring Viewer UCP");
			cur_dir->dirinfo.img.cmd = FM_UCP_QCIFIMG_DEINIT;
			cur_dir->dirinfo.img.state = FM_UCP_ERROR;
			result = mfw_fm_imgViewer_deinit();
			if(result != FM_UCP_NO_ERROR)
			{
				//Error deinitializing the Viewer UCP, destroy it
				TRACE_ERROR("Error deinitializing the Viewer UCP");
				cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgViewer_destroy();
				if(result != FM_UCP_NO_ERROR)
					TRACE_ERROR("Error destroying the Viewer UCP");
			}
		}
	 }
	else if(cur_dir->dirinfo.img.cmd != FM_UCP_NONE)
	{
		//Viewer UCP is not yet created, so create it and then draw the thumbnails
		result = mfw_fm_imgViewer_create();
		if(result == FM_UCP_NO_ERROR)
		{
			strcpy(curFilename,cur_dir->obj_list[cur_dir->currentIndex ]->name);
			mmi_fm_get_filename(curFilename);
		 	sprintf(currFilePath,"%s%s/",cur_dir->dir_path,cur_dir->dir_name);
			result = mfw_fm_imgViewer_setImageProperty((char *)currFilePath,curFilename,FM_MFW_QCIF_WIDTH,FM_MFW_QCIF_HEIGHT,0,0,fm_data->source);			
			if(result == FM_UCP_NO_ERROR)		
			{
				result = mfw_fm_imgViewer_setcallback();
				if(result == FM_UCP_NO_ERROR)	
				{
					cur_dir->dirinfo.img.cmd = FM_UCP_QCIFIMG_INIT;
					cur_dir->dirinfo.img.state = FM_UCP_NONE;
					result = mfw_fm_imgViewer_init();
					if(result != FM_UCP_NO_ERROR)
					{
						//Error initializing the Viewer UCP, destroy it
						TRACE_ERROR("Error initializing the Viewer UCP");
						cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
						cur_dir->dirinfo.img.state = FM_UCP_NONE;
						result = mfw_fm_imgViewer_destroy();
						if(result != FM_UCP_NO_ERROR)
							TRACE_ERROR("Error destroying the Viewer UCP");
					}
					
				}
				else
				{
					//Error configuring the callback for Viewer UCP, Destroy it
					TRACE_ERROR("Error configuring the callback for Viewer UCP");
					cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
					cur_dir->dirinfo.img.state = FM_UCP_NONE;
					result = mfw_fm_imgViewer_destroy();
					if(result != FM_UCP_NO_ERROR)
						TRACE_ERROR("Error destroying the Viewer UCP");
				}
			}
			else
			{
				//Error configuring the Viewer UCP, Destroy it
				TRACE_ERROR("Error configuring the Viewer UCP");
				cur_dir->dirinfo.img.cmd = FM_UCP_NONE;
				cur_dir->dirinfo.img.state = FM_UCP_NONE;
				result = mfw_fm_imgViewer_destroy();
				if(result != FM_UCP_NO_ERROR)
					TRACE_ERROR("Error destroying the Viewer UCP");
			}
		}
		else
		{
			//Error creating Viewer UCP
			TRACE_ERROR("Error creating Viewer UCP");
		}
	}
}


void mmi_fm_set_mmi_plane_to_front(void)
{
	TRACE_FUNCTION ("mmi_fm_set_mmi_plane_to_front()");
	#ifdef FF_SSL_ADAPTATION 
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state3 %d",cur_dir->dirinfo.aud.state);
	dspl_set_mmi_to_idle_mode();
//	TRACE_EVENT_P1("cur_dir->dirinfo.aud.state4 %d",cur_dir->dirinfo.aud.state);
	dspl_set_mmi_to_execute_mode();
	//TRACE_EVENT_P1("cur_dir->dirinfo.aud.state5 %d",cur_dir->dirinfo.aud.state);
	#endif

}

GLOBAL int mmi_pb_on_off(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	
	switch(m->lCursor[m->level])
	{
		case 0:
			mmi_progress_bar = PB_ON;
			mmi_fm_app_show_info(parent_win,TxtMidiLoopOn,TxtNull,TxtNull,TxtNull,0,THREE_SECS, KEY_CLEAR);
			break;

		case 1:
			mmi_progress_bar = PB_OFF;
			mmi_fm_app_show_info(parent_win,TxtMidiLoopOff,TxtNull,TxtNull,TxtNull,0,THREE_SECS, KEY_CLEAR);
			break;
	}
	return 1;
}

/* Mar 29, 2007 DRT: OMAPS00122677 x0039928 */
/* Fix: Initialize root directories */
/*******************************************************************************
 $Function:    		mmi_fm_init

 $Description:	 	Drawa images in QCIF width and height
				
 $Returns:		None

 $Arguments:		None
******************************************************************************/
void mmi_fm_init(void)
{
	mfw_fm_createRootDir(FM_NOR_FLASH);
	mfw_fm_createRootDir(FM_NORMS_FLASH);
	mfw_fm_createRootDir(FM_NAND_FLASH);
	if(check_dev(NULL, NULL, NULL) == 0)
		mfw_fm_createRootDir(FM_T_FLASH);
}
/* February 8, 2008 DRT:OMAPS00156759 (x0083025) For Unicode Support--> Start*/	
#ifdef FF_MMI_UNICODE_SUPPORT
static void unicode_to_char_array(T_WCHAR *inp, char *out)
{
	int i;
	int size = wstrlen(inp)+1;

	for (i=0; i<size; i++)
	{
		out[i*2] = (inp[i]&0xFF00)>>8;
		out[i*2+1] = (inp[i]&0x00FF);
		//out[i*2] = *(((char *) (inp+i)) + 1);
		//out[i*2+1] = *((char *) (inp+i));
	}
}
#endif
/* February 8, 2008 DRT:OMAPS00156759 (x0083025) For Unicode Support--> End*/	

#endif
