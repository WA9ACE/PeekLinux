/*
+--------------------------------------------------------------------+
| PROJECT:	MMI-Framework (8417)		$Workfile::	mfw_midi.c	    $|
| $Author::	NDH							$Revision::	1			    $|
| CREATED:	21.05.04		     		$Modtime::	21.05.04 14:58	$|
| STATE  :	code														 |
+--------------------------------------------------------------------+

   MODULE  : MFW_MIDI

   PURPOSE : This module contains Midi Riveria Interface functions.

   HISTORY  :

   	Aug 27, 2007 REF:DRT OMAPS00137370 x0045876
   	Description: MM: MIDI/IMY file is not played correctly after listening a 
   				 MP3 in the Browse Midi list.
   	Solution: Param Voice_Limit is changed when MP3 or AAC file is played. So before 
   			  playing MP3 or AAC file, Voice_Limit should be saved.
    Apr 10, 2007    DRT: OMAPS00125309  x0039928
    Description: MM - Mono option does not work => MMI changes required
    Solution: Selected Output channel from the settings menu is set for all the players.
    
       Mar 20, 2007    REF:DRT OMAPS00120135  x0039928
   	Description:  Midi/MP3/AAC Ringer alert failed with N5.23 pre release
	Solution: The parameter to as_deduce_player_type is corrected.
	
       Jun 06, 2006    REF:ER OMAPS00080844 xdeepadh
   	Description:  duplication of midi files in ffs area
	Solution: The code to create and load  the midi files from FFS to LFS is 
	removed.

   	Nov 21, 2005    REF:ER OMAPS00057430 nekkareb
   	Description:   AAC Ringer Implementation
	Solution: The Midi Ringer application has been enhanced to support AAC as ringer.

   	Nov 23, 2005 REF : DR OMAPS00057378   x0039928
	Bug: All MIDI files are not displayed in MMI in locosto-lite
	Fix: Buffer memory allocated to copy the file contents is freed
	properly and copying file from FFS to LFS is not terminated if error is
	returned for just any one file, instead it tries to copy other files.

	Nov 09, 2005    REF:DR OMAPS00049167  x0039928	
	Description:   The midi files cannot be heard if the audio channel selected is 'Mono'	
	Solution: Midi Player params are made global so that settings are retained across.
	
   	Nov 04, 2005    REF:DRT OMAPS00053737  xdeepadh
	Description:   MIDI Ringer Linear Flash Support.
	Solution: The Midi Ringer application has been enhanced to loadand play the midi files from LFS.
	   
   	Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
	Description:   Mp3 Ringer Implementation
	Solution: The Midi Ringer application has been enhanced to support mp3 as ringer.
	The interface  function names have been modified.
	
	Aug 17, 2005      REF: CHG 33822 xdeepadh
	Description: Configuring the Audio path from Midi Test Application Menu
	Solution: Loading the audio profile for headset and handheld.
	
	Aug 05, 2005     xdeepadh
	Description: Configuring the audio path for midi

	Apr 05, 2005    REF: ENH 29994 xdeepadh
	Description: Implementation of Generic Midi Ringer and Modification of existing Midi Test Application
	Solution: Generic Midi Ringer and Midi Test Application were implemented.

*/

#define ENTITY_MFW
/*
** Include Files
*/

#include "rv_swe.h"

/* includes */
#include <string.h>
#include "typedefs.h"
#include "mfw_midi.h"
#include "mfw_mfw.h"
#include "mfw_sys.h"
#ifdef FF_MMI_AUDIO_PROFILE
#include "mfw_mme.h"
#endif
//Nov 04, 2005    REF:DRT OMAPS00053737  xdeepadh
#ifdef FF_MIDI_LOAD_FROM_LFS
#include "ffs/ffs_api.h"
#endif	//FF_MIDI_LOAD_FROM_LFS



/*Local defines*/

#define MIDI_FIRST_FILE_INDEX 0
#define MIDI_MAX_FILES 10
/// Maximum length of the audio data filename (excluded EOS character).

/*
** Local Variable Definitions
*/
static T_RV_RETURN_PATH midi_return_path;

/* Global variables*/
INT16 player_channel = MIDI_CHANNEL_STEREO;
int mfw_player_currently_playing_idx=-1;
UBYTE mfw_player_exit_flag=PLAYER_EXIT_FLAG_TRUE;
UBYTE mfw_player_playback_loop_flag=FALSE;
//Aug 05, 2005     xdeepadh
#ifdef FF_MMI_AUDIO_PROFILE
//Flag to indicate whether audio media is being played.
extern UBYTE mfwAudPlay;
extern void mfw_audio_set_device(int type);
#endif //FF_MMI_AUDIO_PROFILE

// Nov 09, 2005    REF:DR OMAPS00049167  x0039928
// Fix : Declare player_para as global variable.
#ifdef FF_MMI_TEST_MIDI	
T_AS_PLAYER_PARAMS player_para;
#endif

/* Aug 27, 2007    DRT: OMAPS00137370  x0045876 */
/* Voice Limit parameter should be saved to a temp var before playing 
   MP3 or AAC file */
static INT16 temp_voice_limit = FF_MIDI_VOICE_LIMIT;
/*
** Public function Definitions
*/

/*******************************************************************************

 $Function:     mfw_player_init

 $Description: initialize global variables

 $Returns:

 $Arguments: 

*******************************************************************************/
void mfw_player_init(void)
{
    // Nov 09, 2005    REF:DR OMAPS00049167  x0039928
    // Fix : Initialize the player parameter to the default values.
    #ifdef FF_MMI_TEST_MIDI
     	TRACE_FUNCTION("mfw_player_init");
	player_para.midi.output_channels = MIDI_CHANNEL_MONO;	
	player_para.midi.voice_limit = FF_MIDI_VOICE_LIMIT;
	as_player_set_params(AS_PLAYER_TYPE_MIDI,&player_para);
     #endif
 }

/*
** Public function Definitions
*/

/*******************************************************************************

 $Function:    mfw_player_set_loop_on

 $Description:		Sets the playback of the midi file in loop on mode(repeat).

 $Returns:		MFW_PLAYER_TEST_OK

 $Arguments: 		None

*******************************************************************************/
SHORT mfw_player_set_loop_on(void)
{
	TRACE_FUNCTION("mfw_player_set_loop_on");
	mfw_player_playback_loop_flag = TRUE;
	return MFW_PLAYER_TEST_OK;
}


/*******************************************************************************

 $Function:     mfw_player_set_loop_off

 $Description:		Sets the playback of the midi file in loop off mode(once)

 $Returns:		MFW_PLAYER_TEST_OK

 $Arguments:		 None

*******************************************************************************/
SHORT mfw_player_set_loop_off(void)
{
	TRACE_FUNCTION("mfw_player_set_loop_off");
	mfw_player_playback_loop_flag = FALSE;
	return MFW_PLAYER_TEST_OK;
}



/*******************************************************************************

 $Function:     mfw_player_set_channel_mono

 $Description:Sets the channel to mono

 $Returns:		MFW_PLAYER_TEST_OK

 $Arguments: None 

*******************************************************************************/
SHORT mfw_player_set_channel_mono(void)
{
#ifdef FF_MMI_TEST_MIDI
	TRACE_EVENT("mfw_player_set_channel_mono");
/* Apr 10, 2007    DRT: OMAPS00125309  x0039928 */
	player_channel = MIDI_CHANNEL_MONO;
       return MFW_PLAYER_TEST_OK;
#else
       return MFW_PLAYER_TEST_OK;
#endif
}


/*******************************************************************************

 $Function:     mfw_player_set_channel_stereo

 $Description:Sets channel to stereo

 $Returns:		MFW_PLAYER_TEST_OK

 $Arguments:  None

*******************************************************************************/
SHORT mfw_player_set_channel_stereo(void)
{
#ifdef FF_MMI_TEST_MIDI
	TRACE_EVENT("mfw_player_set_channel_stereo");
/* Apr 10, 2007    DRT: OMAPS00125309  x0039928 */
	 player_channel = MIDI_CHANNEL_STEREO;
         return MFW_PLAYER_TEST_OK;
#else
         return MFW_PLAYER_TEST_OK;
#endif
}

/* Aug 27, 2007    DRT: OMAPS00137370  x0045876 */
/* Voice Limit parameter should be saved to a temp var before playing 
   MP3 or AAC file */
/*******************************************************************************

 $Function:         mfw_player_midi_get_voice_limit

 $Description:		gets the voicelimit

 $Returns:			Value of Voice Limit between 0 & 32

 $Arguments: 		None

*******************************************************************************/
INT16 mfw_player_midi_get_voice_limit(void)
{
	return temp_voice_limit;
}

/*******************************************************************************

 $Function:         mfw_player_set_voice_limit

 $Description:		sets the voicelimit

 $Returns:			None

 $Arguments: 		voice_limit:The voice limit to be set

*******************************************************************************/
void mfw_player_set_voice_limit(INT16 voice_lmt)
{
	temp_voice_limit = voice_lmt;
}

/*******************************************************************************

 $Function:            mfw_player_midi_set_voice_limit

 $Description:		Sets the voicelimit

 $Returns:		MFW_PLAYER_TEST_OK

 $Arguments: 		voice_limit:The voice limit to be set

*******************************************************************************/
SHORT mfw_player_midi_set_voice_limit(int voice_limit)
	{
#ifdef FF_MMI_TEST_MIDI
	T_AS_RET player_return;
	TRACE_EVENT_P1("mfw_player_midi_set_voice_limit is %d",voice_limit);
	player_para.midi.voice_limit=voice_limit;

	/* Aug 27, 2007    DRT: OMAPS00137370  x0045876 */
	/* Voice Limit parameter should be saved to a temp var before playing 
	   MP3 or AAC file */
	mfw_player_set_voice_limit(voice_limit);
	TRACE_EVENT_P1("Temp Voice Limit %d", temp_voice_limit);
	
	player_return=as_player_set_params(AS_PLAYER_TYPE_MIDI,&player_para);
	if(player_return==AS_OK)
		return MFW_PLAYER_TEST_OK;
	else
		return MFW_PLAYER_TEST_ERROR;
#else
    return MFW_PLAYER_TEST_OK;
#endif
}

/*******************************************************************************

 $Function:             mfw_player_save_selected_file_idx

 $Description:          save index of file which have been selected

 $Returns:		MFW_PLAYER_TEST_OK

 $Arguments: 		index: index to be saved

*******************************************************************************/
SHORT mfw_player_save_selected_file_idx(int index)
	{
     //default index is set to 0 and inputFileName to NULL at global initialisation
     //save current index of selected file
     	TRACE_EVENT_P1("mfw_player_save_selected_file_idx is %d",index);
     mfw_player_currently_playing_idx = index;
    return MFW_PLAYER_TEST_OK;

}

/*******************************************************************************

 $Function:     mfw_player_set_speaker

 $Description:Sets the player_device to Speaker


 $Returns:		MFW_PLAYER_TEST_OK

 $Arguments:  None

*******************************************************************************/
 SHORT mfw_player_set_speaker()
{
#ifdef FF_MMI_TEST_MIDI
	//	Aug 17, 2005      REF: CHG 33822 xdeepadh
	//Load the profile for the Handheld.
#ifdef FF_MMI_AUDIO_PROFILE
	mfw_audio_set_device(MFW_AUD_HANDHELD);
	return MFW_PLAYER_TEST_OK ;
#else
	T_AS_RET player_return;
	TRACE_EVENT("mfw_player_set_speaker");
	player_para.common.output_device=AS_DEVICE_SPEAKER;
	player_return=as_player_set_params(AS_PLAYER_TYPE_COMMON,&player_para);
	if(player_return==AS_OK)
	return MFW_PLAYER_TEST_OK ;

#endif //FF_MMI_AUDIO_PROFILE

#else
	 return MFW_PLAYER_TEST_OK;

#endif //FF_MMI_TEST_MIDI
}
/*******************************************************************************

 $Function:     mfw_player_set_headset

 $Description:Sets the player_device to Headset

 $Returns:		MFW_PLAYER_TEST_OK

 $Arguments:  None

*******************************************************************************/
 SHORT mfw_player_set_headset()
    {
#ifdef FF_MMI_TEST_MIDI
//	Aug 17, 2005      REF: CHG 33822 xdeepadh
//Load the profile for the Headset.
#ifdef FF_MMI_AUDIO_PROFILE
		mfw_audio_set_device(MFW_AUD_HEADSET);
		return MFW_PLAYER_TEST_OK ;
#else
    	T_AS_RET player_return;
	TRACE_EVENT("mfw_player_set_headset");
	player_para.common.output_device=AS_DEVICE_HEADSET;
	player_return=as_player_set_params(AS_PLAYER_TYPE_COMMON,&player_para);
	if(player_return==AS_OK)
		return MFW_PLAYER_TEST_OK ;
#endif	//FF_MMI_AUDIO_PROFILE
#else
         return MFW_PLAYER_TEST_OK;

#endif //FF_MMI_TEST_MIDI
}

#ifdef FF_MIDI_RINGER
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
//The function names have been modified to be generic
/*******************************************************************************

 $Function:     mfw_ringer_set_audiopath

 $Description:Sets the player_device to Speaker


  $Returns:		MFW_PLAYER_TEST_OK

 $Arguments:  None

*******************************************************************************/
 SHORT mfw_ringer_set_audiopath()
     {
#ifdef FF_MMI_TEST_MIDI
    	T_AS_RET player_return;
	TRACE_EVENT("mfw_ringer_set_audiopath");
	player_para.common.output_device=AS_DEVICE_SPEAKER;
	player_return=as_player_set_params(AS_PLAYER_TYPE_COMMON,&player_para);
	if(player_return==AS_OK)
		return MFW_PLAYER_TEST_OK ;
	else
		return MFW_PLAYER_TEST_ERROR;	
#else
         return MFW_PLAYER_TEST_OK;

 #endif
}
/*******************************************************************************

 $Function:     mfw_ringer_set_memory

 $Description:Set the ringer memory address for the given ringer mode.

 $Returns:
				AS_OK	The call is successful.
				AS_INVALID_PARAM	The player_type,ringer_mode  parameter value is invalid.
				AS_INTERNAL_ERROR	Not able to send the message request.

 $Arguments:  	                player_type:Type of the player based on the file.
 				ringer_mode: ringer mode
 				memory_pointer:pointer to midi file location
 				size:size of the midi file

*******************************************************************************/

SHORT mfw_ringer_set_memory(T_AS_PLAYER_TYPE  player_type,T_AS_RINGER_MODE ringer_mode,UINT32 *memory_pointer,UINT32 size)
{
	T_AS_RET ringer_return;
	TRACE_FUNCTION("mfw_ringer_set_memory");
	TRACE_EVENT_P1("player_type is %d",player_type);
	ringer_return=as_ringer_set_memory(ringer_mode,player_type,memory_pointer, size);
	return ringer_return;
}

/*******************************************************************************

 $Function:     mfw_ringer_set_file

 $Description:Set the ringer filename for the given ringer mode.

 $Returns:
				AS_OK	The call is successful.
				AS_INVALID_PARAM	The player_type or ringer_mode parameter value is invalid.
				AS_INTERNAL_ERROR	Not able to send the message request.

 $Arguments:
 				player_type:Type of the player based on the file.
 				ringer_mode: ringer mode
 				filename:Name of the midi file

*******************************************************************************/
SHORT mfw_ringer_set_file(T_AS_PLAYER_TYPE  player_type,T_AS_RINGER_MODE ringer_mode, const char* filename)
{
	T_AS_RET ringer_return;
	char temp_file_name[FILENAME_MAX_LEN];
#ifdef FF_MMI_FILEMANAGER
	UINT16 temp_file_name_uc[FILENAME_MAX_LEN];
#endif
	TRACE_FUNCTION("mfw_ringer_set_file");
	
	//Nov 04, 2005    REF:DRT OMAPS00053737  xdeepadh	
#ifdef FF_MIDI_LOAD_FROM_LFS 	//Set the linear file for Ringer

	//Provide the complete path of the file
	strcpy(temp_file_name, LFS_CONTENT_DIR);
	strcat(temp_file_name, filename );
	TRACE_EVENT_P1(" linear temp_filename  %s",temp_file_name);
	ringer_return=as_ringer_set_linear_file(ringer_mode,player_type,temp_file_name);
	
#else			//Set the FFS  file for Ringer
	//Provide the complete path of the file
	strcpy(temp_file_name, "/FFS");
	strcat(temp_file_name, RINGER_CONTENT_DIR);
	strcat(temp_file_name, "/");
	strcat(temp_file_name, filename );
	TRACE_EVENT_P1(" ffs temp_filename  %s",temp_file_name);
#ifdef FF_MMI_FILEMANAGER
	convert_u8_to_unicode(temp_file_name, temp_file_name_uc);
	ringer_return=as_ringer_set_file(ringer_mode,player_type,temp_file_name_uc);
#else
	ringer_return=as_ringer_set_file(ringer_mode,player_type,temp_file_name);
#endif
#endif   //FF_MIDI_LOAD_FROM_LFS
	return ringer_return;
}

/*******************************************************************************

 $Function:     mfw_ringer_set_volume

 $Description:Set the ringer volume for the given ringer mode.

 $Returns:
				AS_OK	The call is successful.
				AS_INVALID_PARAM	The ringer_mode or volume parameter value is invalid.
				AS_INTERNAL_ERROR	Not able to send the message request.

 $Arguments:  	ringer_mode: ringer mode
 				volume:Volume which has to be set for the given ringer mode

*******************************************************************************/

SHORT mfw_ringer_set_volume(T_AS_RINGER_MODE ringer_mode,T_AS_VOLUME volume)
{
	T_AS_RET ringer_return;
	TRACE_FUNCTION("mfw_ringer_set_volume");
	TRACE_EVENT_P1("the volume to be set is %d",volume);
	ringer_return=as_ringer_set_volume(ringer_mode,volume);
	return ringer_return;
}

/*******************************************************************************

 $Function:     mfw_ringer_start

 $Description:		Play a particular ringer file

 $Returns:
				AS_OK	The call is successful.
				AS_INVALID_PARAM	The ringer_mode parameter value is invalid.
				AS_INTERNAL_ERROR	Not able to send the message request.

 $Arguments:
 				ringer_mode: ringer mode
 				loop:playback in loop
 				callback_fn:callback function

*******************************************************************************/

SHORT mfw_ringer_start(T_AS_RINGER_MODE ringer_mode,BOOLEAN loop,void (*callback_fn)(void *))
{
	T_AS_RET ringer_return;
	TRACE_EVENT("mfw_ringer_start");
	//Aug 05, 2005     xdeepadh
	#ifdef FF_MMI_AUDIO_PROFILE
	//Configure the audio path to current Audio device.
	mfw_set_stereo_path(mfw_get_current_audioDevice());
 	mfwAudPlay =  TRUE; //File is playing
	#endif //FF_MMI_AUDIO_PROFILE
	mfw_ringer_configure_callback_fn(callback_fn);
	ringer_return=as_ring(ringer_mode,loop,&midi_return_path);
	return ringer_return;
}

/*******************************************************************************

 $Function:     mfw_ringer_stop

 $Description:Stop the current ringer melody playback.

 $Returns:
				AS_OK	The call is successful.
				AS_INTERNAL_ERROR	Not able to send the message request.

 $Arguments:  	callback_fn:callback function

*******************************************************************************/
	
SHORT mfw_ringer_stop(void (*callback_fn)(void *))
	{
	T_AS_RET ringer_return;
	TRACE_EVENT("mfw_ringer_stop");
	mfw_ringer_configure_callback_fn(callback_fn);
	ringer_return=as_stop(&midi_return_path);
	return ringer_return;
}

/*******************************************************************************

 $Function:     mfw_ringer_get_info

 $Description:		Gets the ringer information for the given ringer mode.

 $Returns:
				pointer to T_AS_RINGER_INFO	
 $Arguments:  	ringer_mode: ringer mode
 				ringer_info: The ringer details will be stored here.

*******************************************************************************/
T_AS_RINGER_INFO* mfw_ringer_get_info(T_AS_RINGER_MODE ringer_mode,const T_AS_RINGER_INFO* ringer_info)
{
	T_AS_RET ringer_return;
	TRACE_FUNCTION("mfw_ringer_get_info");
	ringer_return=as_ringer_get(ringer_mode,&ringer_info);
	TRACE_EVENT_P1("ringer_info volume is %d",ringer_info->volume);
	TRACE_EVENT_P1("ringer info name is %s",ringer_info->data.filename);
	if(ringer_return==AS_OK)
		return (T_AS_RINGER_INFO*)ringer_info;
	else
		return NULL;
}

/*******************************************************************************

 $Function:     mfw_ringer_configure_callback_fn

 $Description:		Configure the Riviera Return Path

 $Returns:None

 $Arguments:  Callback function

*******************************************************************************/

static void mfw_ringer_configure_callback_fn(void (*callback_fn)(void *))
{

	TRACE_EVENT("mfw_ringer_configure_callback_fn");
	midi_return_path.addr_id = 0;
	midi_return_path.callback_func = callback_fn;

	return;
}

/*******************************************************************************

 $Function:     mfw_player_start_memory

 $Description:Start the playback melody stored in the given memory location according to the given player type.

 $Returns:
				AS_OK	The call is successful.
				AS_INVALID_PARAM	The player_type or volume parameter value is invalid.
				AS_INTERNAL_ERROR	Not able to send the message request.

 $Arguments:  	player_type:Type of the player based on the file.
 				address:pointer to midi file location to be played
 				size:size of the midi file
 				volume:Volume with which to play the midi file
 				loop:playback in loop
 				callback_fn:callback function

*******************************************************************************/

SHORT mfw_player_start_memory(T_AS_PLAYER_TYPE player_type,const UINT32 * address, UINT32 size,T_AS_VOLUME volume,BOOLEAN  loop,void (*callback_fn)(void *))
	{
	T_AS_RET player_return;
	TRACE_EVENT("mfw_player_start_memory");
	//Aug 05, 2005     xdeepadh
	#ifdef FF_MMI_AUDIO_PROFILE
	//Configure the audio path to current Audio device.
	mfw_set_stereo_path(mfw_get_current_audioDevice());
 	mfwAudPlay =  TRUE; //File is playing
	#endif //FF_MMI_AUDIO_PROFILE
	mfw_ringer_configure_callback_fn(callback_fn);
	player_return=as_play_memory(player_type,address,size,volume,loop,&midi_return_path);
	return player_return;

}

/*******************************************************************************

 $Function:     mfw_player_start_file

 $Description:Start the playback melody stored in the given file according to the given player type.

 $Returns:
				AS_OK	The call is successful.
				AS_INVALID_PARAM	The player_type or volume parameter value is invalid.
				AS_FILE_ERROR	The melody file can not be accessed, or contain unrecognized data.
				AS_INTERNAL_ERROR	Not able to send the message request.


 $Arguments:  	player_type:Type of the player based on the file.
 				filename:Name of the midi file to be played
 				volume:Volume with which to play the midi file
 				loop:playback in loop
 				callback_fn:callback function
	 
*******************************************************************************/

#ifdef FF_MMI_FILEMANAGER
SHORT mfw_player_start_file(T_AS_PLAYER_TYPE player_type, UINT16 * filename,T_AS_VOLUME volume, BOOLEAN loop,void (*callback_fn)(void *))
#else
SHORT mfw_player_start_file(T_AS_PLAYER_TYPE player_type,const char * filename,T_AS_VOLUME volume, BOOLEAN loop,void (*callback_fn)(void *))
#endif
{
	T_AS_RET player_return;
	char temp_file_name[FILENAME_MAX_LEN];
#ifdef FF_MMI_FILEMANAGER
	char filename_ascii[FILENAME_MAX_LEN];
#endif	
	TRACE_EVENT("mfw_player_start_file");
	//Aug 05, 2005     xdeepadh
	#ifdef FF_MMI_AUDIO_PROFILE
	//Configure the audio path to current Audio device.
	mfw_set_stereo_path(mfw_get_current_audioDevice());
 	mfwAudPlay =  TRUE; //File is playing
	#endif //FF_MMI_AUDIO_PROFILE
	mfw_ringer_configure_callback_fn(callback_fn);
	
//Nov 04, 2005    REF:DRT OMAPS00053737  xdeepadh	
#ifdef FF_MIDI_LOAD_FROM_LFS   //Play the linear file for Player

	//Since, the file can be with or without the full path ,
	//Verify the file for full path , else provide the complete path.
	strcpy(temp_file_name, LFS_CONTENT_DIR);

	//The complete path is provided
	if(strncmp(filename,temp_file_name,strlen(temp_file_name))==0)
	{
		player_return=as_play_linear_file(player_type,filename,volume,loop,&midi_return_path);
	}
		//Provide the  complete path 
	else
	{
		strcat(temp_file_name, filename );
		player_return=as_play_linear_file(player_type,temp_file_name,volume,loop,&midi_return_path);
	}
	
#else			//Play the FFS  file for Player

	//Since, the file can be with or without the full path ,
	//Verify the file for full path , else provide the complete path.
        //The complete path is provided
#ifdef FF_MMI_FILEMANAGER
	strcpy(temp_file_name, "/FFS");
       strcat(temp_file_name, RINGER_CONTENT_DIR);
	convert_unicode_to_u8(filename, filename_ascii);
	if(strncmp(filename_ascii,temp_file_name,strlen(temp_file_name))==0)
#else
	strcpy(temp_file_name, RINGER_CONTENT_DIR);
	if(strncmp(filename,temp_file_name,strlen(temp_file_name))==0)
#endif
	{
#ifdef FF_MMI_FILEMANAGER	
		player_return=as_play_file(player_type,filename,volume,loop,0,&midi_return_path);
#else
		player_return=as_play_file(player_type,filename,volume,loop,&midi_return_path);
#endif
	}
		//Provide the  complete path 
	else
	{
		strcat(temp_file_name, "/");
#ifdef FF_MMI_FILEMANAGER		
		strcat(temp_file_name, filename_ascii);
		convert_u8_to_unicode(temp_file_name, filename);
		player_return=as_play_file(player_type,filename,volume,loop,0,&midi_return_path);
#else
		strcat(temp_file_name, filename );
		player_return=as_play_file(player_type,temp_file_name,volume,loop,&midi_return_path);
#endif
	}
#endif	//FF_MIDI_LOAD_FROM_LFS

	return player_return;
}

/*******************************************************************************

 $Function:     		mfw_ringer_deduce_player_type

 $Description:		Deduces  the player type

 $Returns: 		T_AS_PLAYER_TYPE


 $Arguments:  	filename:Name of the midi file

*******************************************************************************/

T_AS_PLAYER_TYPE mfw_ringer_deduce_player_type(const char * filename)
{
	T_AS_PLAYER_TYPE player_type;
#ifdef FF_MMI_FILEMANAGER
	char temp_file_name[FILENAME_MAX_LEN];
	UINT16 * filename_uc;
/* Mar 20, 2007    REF:DRT OMAPS00120135  x0039928 */
/* Fix: filename_uc is a pointer now instead of array   */
	filename_uc = (UINT16 *)mfwAlloc(sizeof(UINT16) * FILENAME_MAX_LEN);
	strcpy(temp_file_name, "/FFS");
	strcat(temp_file_name, RINGER_CONTENT_DIR);
	strcat(temp_file_name, "/");
	strcat(temp_file_name, filename );
	convert_u8_to_unicode(temp_file_name, filename_uc);
	as_deduce_player_type(filename_uc,&player_type);
	mfwFree((U8 *) filename_uc, sizeof(UINT16) * FILENAME_MAX_LEN);
#else	
	as_deduce_player_type(filename,&player_type);
#endif	
	TRACE_EVENT("mfw_ringer_deduce_player_type");
	return player_type;
}


/*******************************************************************************

 $Function:     mfw_player_stop

 $Description:Stop the current melody player

 $Returns:
				AS_OK	The call is successful.
				AS_INTERNAL_ERROR	Not able to send the message request.


 $Arguments:  	callback_fn:callback function

*******************************************************************************/

SHORT mfw_player_stop(void (*callback_fn)(void *))
{
	T_AS_RET player_return;
	TRACE_EVENT("mfw_player_stop");
	mfw_ringer_configure_callback_fn(callback_fn);
	player_return=as_stop(&midi_return_path);
	return player_return;
}

/*******************************************************************************

 $Function:     mfw_player_midi_set_params

 $Description:Set the specific parameters for the player of given player_type.

 $Returns:		AS_OK if call is succesfull.
 				AS_INVALID_PARAM if the player_type parameter value is invalid.

 $Arguments:  	Voicelimit:Voice limit to be set
 				channel:channel to be set

*******************************************************************************/

SHORT mfw_player_midi_set_params(INT16 voicelimit,INT16 channel)
{
	T_AS_PLAYER_PARAMS player_para;
	T_AS_RET player_return;
	TRACE_FUNCTION("mfw_player_midi_set_params");
	//Set the Voice limit and channel for Idle mode
	player_para.midi.voice_limit=voicelimit  ;
	player_para.midi.output_channels=channel;
	player_return=as_player_set_params(AS_PLAYER_TYPE_MIDI,&player_para);
	return player_return;
}

/*******************************************************************************

 $Function:     mfw_player_mp3_set_params

 $Description:Set the specific parameters for the player of given player_type.(mp3)

 $Returns:		AS_OK if call is succesfull.
 				AS_INVALID_PARAM if the player_type parameter value is invalid.

 $Arguments:  		size_file_start:size of the file where the melody must start 
 				mono_stereo:channel to be set

*******************************************************************************/
#ifdef FF_MP3_RINGER
SHORT mfw_player_mp3_set_params(UINT32  size_file_start,BOOLEAN  mono_stereo)
{
	T_AS_PLAYER_PARAMS player_para;
	T_AS_RET player_return;
	TRACE_FUNCTION("mfw_player_mp3_set_params");
	//Set the Voice limit and channel for Idle mode
	player_para.mp3.mono_stereo = mono_stereo  ;
	player_para.mp3.size_file_start = size_file_start;
	player_return=as_player_set_params(AS_PLAYER_TYPE_MP3,&player_para);
	return player_return;
}
#endif 		//FF_MP3_RINGER


//Nov 22, 2005    ER: OMAPS00057430, AAC Ringer support, nekkareb
/*******************************************************************************

 $Function:     mfw_player_aac_set_params

 $Description:Set the specific parameters for the player of given player_type.(aac)

 $Returns:	AS_OK if call is succesfull.
 			AS_INVALID_PARAM if the player_type parameter value is invalid.

 $Arguments:  size_file_start:size of the file where the melody must start 
 			 mono_stereo:channel to be set

*******************************************************************************/
#ifdef FF_AAC_RINGER
SHORT mfw_player_aac_set_params(UINT32  size_file_start,BOOLEAN  mono_stereo)
{
	T_AS_PLAYER_PARAMS player_para;
	T_AS_RET player_return;
	
	TRACE_FUNCTION("mfw_player_aac_set_params");
	
	//Set the Voice limit and channel for Idle mode
	player_para.aac.mono_stereo = mono_stereo  ;
	player_para.aac.size_file_start = size_file_start;
	
	player_return=as_player_set_params(AS_PLAYER_TYPE_AAC,&player_para);
	return player_return;
}
#endif 	//FF_AAC_RINGER


 /*******************************************************************************

 $Function:     mfw_player_get_params

 $Description:gets the specific parametersfor the player  of given player_type.

 $Returns:		AS_OK if call is succesfull.
 				AS_INVALID_PARAM if the player_type parameter value is invalid.

 $Arguments:  	player_type:Type of the player based on the file.
 				player_para:The structure containing current parameters for the player type

*******************************************************************************/

SHORT mfw_player_get_params(T_AS_PLAYER_TYPE player_type,const T_AS_PLAYER_PARAMS* player_para)
{
	T_AS_RET player_return;
	TRACE_FUNCTION("mfw_player_get_params");
	player_return=as_player_get_params(player_type,&player_para);
	return player_return;
}


#endif
