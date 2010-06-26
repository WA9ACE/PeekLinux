/*
+--------------------------------------------------------------------+
| PROJECT:	MMI-Framework (8417)		$Workfile::	mfw_midi.h	    $|
| $Author::	NDH							$Revision::	1			    $|
| CREATED:	21.05.04		     		$Modtime::	21.05.04	 14:58	$|
| STATE  :	code														 |
+--------------------------------------------------------------------+

   MODULE  : MFW_MIDI

   PURPOSE : This module contains the definitions for the Midi Riveria Interface.

   HISTORY  :
   	Aug 27, 2007 REF:DRT OMAPS00137370 x0045876
   	Description: MM: MIDI/IMY file is not played correctly after listening a 
   				 MP3 in the Browse Midi list.
   	Solution: Param Voice_Limit is changed when MP3 or AAC file is played. So before 
   			  playing MP3 or AAC file, Voice_Limit should be saved.
       Jun 06, 2006    REF:ER OMAPS00080844 xdeepadh
   	Description:  duplication of midi files in ffs area
	Solution: The code to create and load  the midi files from FFS to LFS is 
	removed.

   	Nov 21, 2005    REF:ER OMAPS00057430 nekkareb
   	Description:   AAC Ringer Implementation
	Solution: The Midi  Ringer application has been enhanced to support AAC as ringer.

   	Nov 04, 2005    REF:DRT OMAPS00053737  xdeepadh
	Description:   MIDI Ringer Linear Flash Support.
	Solution: The Midi Ringer application has been enhanced to loadand play the midi files from LFS.
	
   	Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
	Description:   Mp3 Ringer Implementation
	Solution: The Midi Ringer application has been enhanced to support mp3 as ringer.
	The interface  function names have been modified.
	
	Apr 05, 2005    REF: ENH 29994 xdeepadh
	Description: Implementation of Generic Midi Ringer and Modification of existing Midi Test Application
	Solution: Generic Midi Ringer and Midi Test Application were implemented.


*/
//Includes for Audio Services
#include "as/as_api.h" 
#include "as/as_message.h"

#ifdef FF_MMI_FILEMANAGER
#define FILENAME_MAX_LEN 250
#else
#define FILENAME_MAX_LEN 47
#endif

/*
** Midi Test Return Values
*/
#define MFW_PLAYER_TEST_OK					(0)
#define MFW_PLAYER_TEST_RIVIERA_FAILED		(-1)
#define MFW_PLAYER_TEST_MEM_FULL			(-2)
#define MFW_PLAYER_TEST_MEM_EMPTY			(-3)
#define MFW_PLAYER_TEST_ERROR					(-4)

#define PLAYER_EXIT_FLAG_FALSE  0
#define PLAYER_EXIT_FLAG_TRUE    1

#define MIDI_CHANNEL_MONO      1
#define MIDI_CHANNEL_STEREO   2

#ifdef FF_MP3_RINGER
#define MP3_CHANNEL_MONO      0
#define MP3_CHANNEL_STEREO   1
#endif  //FF_MP3_RINGER

//Nov 22, 2005    REF:ER OMAPS00057430 nekkareb
// AAC specific macros
#ifdef FF_AAC_RINGER
#define AAC_CHANNEL_MONO      0
#define AAC_CHANNEL_STEREO   1
#endif  //FF_AAC_RINGER


//Flash folder to store the tones
#define     RINGER_CONTENT_DIR          "/mmi/tones"


//Nov 04, 2005    REF:DRT OMAPS00053737  xdeepadh
#ifdef FF_MIDI_LOAD_FROM_LFS
//LFS  folder to store the tones
#define LFS_CONTENT_DIR "/"


typedef enum 
{
	TEST_PASSED					=  0, /* No error occurs						*/
	TEST_FAILED					= -2, /* An error occurs -> continu test suite	*/
	TEST_IRRECOVERABLY_FAILED	= -3  /* An error occurs -> stop test suite		*/
} T_RV_TEST_RET;

#endif	//FF_MIDI_LOAD_FROM_LFS

/*
** Midi Test Functions Prototypes
*/

SHORT mfw_player_set_loop_on(void);
SHORT mfw_player_set_loop_off(void);
SHORT mfw_player_set_channel_mono(void);
SHORT mfw_player_set_channel_stereo(void);

/* Aug 27, 2007    DRT: OMAPS00137370  x0045876 */
/* Voice Limit parameter should be saved to a temp var before playing 
   MP3 or AAC file */
INT16 mfw_player_midi_get_voice_limit(void);
void mfw_player_set_voice_limit(INT16 voice_lmt);
SHORT mfw_player_midi_set_voice_limit(int voice_limit);
SHORT mfw_player_save_selected_file_idx(int index);
void  mfw_player_init(void);
SHORT mfw_player_set_headset();
SHORT mfw_player_set_speaker();
SHORT mfw_ringer_set_audiopath();
/*
** Midi ringer Function Prototypes
*/
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
//The function names have been modified to be generic
SHORT mfw_ringer_set_memory(T_AS_PLAYER_TYPE  player_type,T_AS_RINGER_MODE ringer_mode,UINT32 *memory_pointer,UINT32 size);
SHORT mfw_ringer_set_file(T_AS_PLAYER_TYPE  player_type,T_AS_RINGER_MODE ringer_mode,const char* filename);
SHORT mfw_ringer_start(T_AS_RINGER_MODE ringer_mode,BOOLEAN  loop,void (*callback_fn)(void *));
SHORT mfw_ringer_set_volume(T_AS_RINGER_MODE ringer_mode,T_AS_VOLUME volume);
SHORT mfw_ringer_start(T_AS_RINGER_MODE ringer_mode,BOOLEAN loop,void (*callback_fn)(void *));
SHORT mfw_ringer_stop(void (*callback_fn)(void *));
T_AS_RINGER_INFO* mfw_ringer_get_info(T_AS_RINGER_MODE ringer_mode,const T_AS_RINGER_INFO* ringer_info);
static void mfw_ringer_configure_callback_fn(void (*callback_fn)(void *));
SHORT mfw_player_start_memory(T_AS_PLAYER_TYPE player_type,const  UINT32* address,  UINT32 size,T_AS_VOLUME volume,BOOLEAN loop,void (*callback_fn)(void *));
#ifdef FF_MMI_FILEMANAGER
SHORT mfw_player_start_file(T_AS_PLAYER_TYPE player_type, UINT16 * filename,T_AS_VOLUME volume,BOOLEAN  loop,void (*callback_fn)(void *));
#else
SHORT mfw_player_start_file(T_AS_PLAYER_TYPE player_type,const char * filename,T_AS_VOLUME volume,BOOLEAN  loop,void (*callback_fn)(void *));
#endif

SHORT mfw_player_stop(void (*callback_fn)(void *));
SHORT mfw_player_midi_set_params(INT16 voicelimit,INT16 channel);
#ifdef FF_MP3_RINGER
SHORT mfw_player_mp3_set_params(UINT32  size_file_start,BOOLEAN  mono_stereo);
#endif  //FF_MP3_RINGER

//Nov 22, 2005    REF:ER OMAPS00057430 nekkareb
// Added function prototype for aac set paramaters
#ifdef FF_AAC_RINGER
SHORT mfw_player_aac_set_params(UINT32  size_file_start,BOOLEAN  mono_stereo);
#endif  //FF_AAC_RINGER


T_AS_PLAYER_TYPE mfw_ringer_deduce_player_type(const char * filename);
SHORT mfw_player_get_params(T_AS_PLAYER_TYPE player_type,const T_AS_PLAYER_PARAMS* player_para);

