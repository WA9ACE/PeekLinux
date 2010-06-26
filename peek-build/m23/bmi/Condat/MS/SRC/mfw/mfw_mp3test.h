/*
+--------------------------------------------------------------------+
| PROJECT:	MMI-Framework (8417)		$Workfile::	mfw_mp3test.h	$|
| $Author::	NDH							$Revision::	1			    $|
| CREATED:	21.05.04		     		$Modtime::	21.05.04	 	$|
| STATE  :	code												     |
+--------------------------------------------------------------------+

   MODULE  : MFW_MP3TEST

   PURPOSE : This module contains the definitions for the Midi Riveria Interface.

   HISTORY :  

 	Sep 11, 2006 DR: OMAPS00094182 xrashmic
 	Description: midi and mp3 files is not being populated on accessing corresponding menu, but only during bootup
 	Solution: Populate files on accessing the "browse files" list

   Jun 02,2006 REF: DR OMAPS00079746  x0039928  
   Bug:MMI displays 'Playing' when trying to play a wrong file
   Fix: MP3 playback error is handled.

   Nov 16,2005 REF: DR OMAPS00049192  x0039928
   Bug:MP3: The screen is not refreshed after the mp3 file completes playing
   Fix: The AUDIO_OK event recieved after mp3 file play completes is now handled 
   in mfw_mp3_test_start_cb()

   Nov 14, 2005    REF: OMAPS00044445 xdeepadh
   Description: Implementation of Test Menu for AAC
   Solution: The existing MP3 test Application has been enhanced to support the AAC file testing.
   The interfaces have been made generic to support both MP3 and AAC files.

*/

/*
** Midi Test Return Values
*/


#define MFW_PLAYER_MP3 	0
#define MFW_PLAYER_AAC 	1

#define MFW_PLAYER_TEST_OK					(0)
#define MFW_PLAYER_TEST_RIVIERA_FAILED		(-1)
#define MFW_PLAYER_TEST_MEM_FULL			(-2)
#define MFW_PLAYER_TEST_MEM_EMPTY			(-3)

// Nov 16,2005 REF: DR OMAPS00049192  x0039928
//  Jun 02,2006 REF: DR OMAPS00079746  x0039928  
// Fix: Added a new parameter to the callback function.
typedef struct
{
	T_MFW_HND focus_win;
	void (*callback)(T_MFW_HND, SHORT);
} MMI_RETURN_PATH;

/*
** Midi Test Functions Prototypes
*/

SHORT mfw_audio_player_set_channel_mono(void);
SHORT mfw_audio_player_set_channel_stereo(void);
SHORT mfw_audio_player_play(void);
char* mfw_audio_player_return_file_name(int index);
int   mfw_audio_player_return_file_number(void);
SHORT mfw_audio_player_save_selected_file_idx(int index);
SHORT mfw_audio_player_play(void);
SHORT mfw_audio_player_pause(void);
SHORT mfw_audio_player_resume(void);
SHORT mfw_audio_player_stop(void);
void  mfw_audio_player_init(void);
 char *mfw_audio_player_GetExtension(char *src);
void mfw_audio_player_unpopulate_files(void);//Sep 11, 2006 DR: OMAPS00094182 xrashmic





