/*
+--------------------------------------------------------------------+
| PROJECT:	MMI-Framework (8417)		$Workfile::	mfw_mp3.c	    $|
| $Author::	NDH							$Revision::	1			    $|
| CREATED:	21.05.04		     		$Modtime::	21.05.04 14:58	$|
| STATE  :	code						    						 |
+--------------------------------------------------------------------+

   MODULE  : MFW_MP3

   PURPOSE : This module contains Audio Interface functions.

   HISTORY  :

    	Nov 07, 2006 ER: OMAPS00102732 x0pleela
	Description: FFS Support for Intel Sibley Flash - Intel 256+64 non-ADMUX (PF38F4050M0Y0C0Q)
	Solution: Closing the opened directory if readdir is not successful and the new code 
   			is under the compilation flag FF_MMI_RELIANCE_FFS
   			
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
   Fix: The AUDIO_OK event recieved after mp3 file play completes is now handled 
   in mfw_mp3_test_start_cb()

   Nov 14, 2005    REF: OMAPS00044445 xdeepadh
   Description: Implementation of Test Menu for AAC
   Solution: The existing MP3 test Application has been enhanced to support the AAC file testing.
   The interfaces have been made generic to support both MP3 and AAC files.

   Nov 24,2005 REF: DR OMAPS00049192  x0039928
   Bug:MP3: The screen is not refreshed after the mp3 file completes playing
   Soln: A variable mfw_mp3_playstate is added to maintain the mfw play state. 
   
   Nov 16,2005 REF: DR OMAPS00049192  x0039928
   Bug:MP3: The screen is not refreshed after the mp3 file completes playing
   Fix: The AUDIO_OK event recieved after mp3 file play completes is now handled 
   in mfw_mp3_test_start_cb()
   
   Aug 05, 2005     xdeepadh
   Description: Configuring the audio path for midi


   Jul 18, 2005    REF: SPR 31695   xdeepadh
   Bug:Problems with MP3 test application
   Fix:The window handling of MP3 Test Application has been done properly.
	
   Apr 06, 2005	REF: ENH 30011 xdeepadh
   Description: Replacing the Test Application compilation flags with new flags. 
   Solution:  Removed the Condition to call mp3 APIs, if L1_MP3 flag is enabled.
   This file will be compiled only if L1_Mp3 flag is enabled.
*/


/*
** Include Files
*/

#define ENTITY_MFW

/* includes */
#include <string.h>

#include "typedefs.h"
#include "rv_swe.h"
#include "l1sw.cfg"

#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */
#include "rvf/rvf_pool_size.h"	/* Stack & Memory Bank sizes definitions */
#include "mfw_mfw.h"
#include "mfw_mp3test.h"
#include "mfw_sys.h" //Added for Trace functions. Jul 18, 2005    REF: SPR 31695   xdeepadh
#include "Audio/audio_api.h"

#include "ffs/ffs_api.h"

/*Local defines*/

//define maximum number of MP3 files being saved
#define PLAYER_MAX_FILES 5
#define PLAYER_DIR  "/mmi/mp3"

/*
** Local Variable Definitions
*/

static T_RV_RETURN_PATH audio_player_return_path;
char* INPUT_FILES_STRING[PLAYER_MAX_FILES];
int NUM_INPUT_FILES;

T_AUDIO_MP3_PARAMETER       mp3_parameter;
#ifdef FF_MMI_TEST_AAC
T_AUDIO_AAC_PARAMETER      aac_parameter;
#endif

#ifdef FF_MMI_AUDIO_PROFILE
//Flag to indicate whether audio media is being played.
extern UBYTE mfwAudPlay;
#endif

//Nov 16,2005 REF: DR OMAPS00049192  x0039928
MMI_RETURN_PATH mmi_audio_player_return_path;

//Nov 24,2005 REF: DR OMAPS00049192  x0039928
// Declare a variable to maintain the mp3 play state
int mfw_mp3_playstate = FALSE;

char* inputFileName; // input file
char tempFilePath[AUDIO_MP3_PATH_NAME_MAX_SIZE];
int audio_player_currently_playing_idx = -1;  //index of file to play
UBYTE IsMP3On;  //Variable to check the selected file

/*
 Function Prototypes
*/
static void mfw_audio_player_configure_callback_fn(void (*callback_fn)(void *));


/*
** Public function Definitions
*/
/*******************************************************************************

 $Function:     mfw_audio_player_populate_files

 $Description:This function is called in order to retrieve file names from   
                	Flash and save them in a table    

 $Returns: Status

 $Arguments: 		filenames: table to save file names in    
				max_files_count:number of files in the list to be filled .                  

*******************************************************************************/
int mfw_audio_player_populate_files(char **file_names, int max_files_count)
{
	/*a0393213 compiler warnings removal - variable fd removed*/
	char        dir_name_p[]         =  PLAYER_DIR;
	T_FFS_DIR	dir                  = {0};
	char		dest_name_p[100]            = "";
	int i, files_count = 0;
	char *fileExt;
	TRACE_FUNCTION("mfw_audio_player_populate_files");
    if (ffs_opendir (dir_name_p, &dir) <= 0)
	{
			files_count = 0;
	}
	//Sep 11, 2006 DR: OMAPS00094182 xrashmic
	//Release previous allocared memory before allocating again
	mfw_audio_player_unpopulate_files();
	
	//read maximum of 5  file names
	for (i = 0; ffs_readdir (&dir, dest_name_p, 100) > 0x0; i++)
	{
		(file_names[files_count]) = (char*)mfwAlloc(strlen(dest_name_p) + 1);

		if(file_names[files_count] != NULL)
		{
		//  Jul 18, 2005    REF: SPR 31695   xdeepadh
			//retrieve file extension
      			fileExt =(char*) mfw_audio_player_GetExtension(dest_name_p);	
			//Store only mp3  and aac file names in array
			if ( strcmp(fileExt, "mp3")== 0 ) 
			{
				strcpy (file_names[files_count], dest_name_p);
				files_count++;
			}
#ifdef FF_MMI_TEST_AAC 
			if ( strcmp(fileExt, "aac") == 0)
			{
				strcpy (file_names[files_count], dest_name_p);
				files_count++;
			}
#endif
		  	if(files_count >= max_files_count)
		    break;
		}
		else
		{
			//files_count = 0;
		}

    }

//x0pleela 07 Nov, 2006 DVT: OMAPS00102732
	//Adding the MMI compilation flag FF_MMI_RELIANCE_FFS	
    //x0pleela 31 Oct, 2006  
    //Closing the directory as per the new FFS logic
#ifdef FF_MMI_RELIANCE_FFS    
    ffs_closedir(dir);
#endif
    return files_count;
}
//Sep 11, 2006 DR: OMAPS00094182 xrashmic
/*******************************************************************************

 $Function:     mfw_audio_player_unpopulate_files

 $Description:Free the allocated player array

 $Returns: Status

 $Arguments: None

*******************************************************************************/
void mfw_audio_player_unpopulate_files(void)
{
	int i=0;
	TRACE_FUNCTION("mfw_audio_player_unpopulate_files");
	//Release the memory allocated for the file list
	for (i = 0; i<PLAYER_MAX_FILES; i++)
	{
		if(INPUT_FILES_STRING[i])
	           mfwFree((U8 *)INPUT_FILES_STRING[i],(U16)strlen(INPUT_FILES_STRING[i]) + 1);
		INPUT_FILES_STRING[i]=NULL;
	}
}

/*******************************************************************************

 $Function:     mfw_audio_player_init

 $Description:Populate the Player array

 $Returns: Status

 $Arguments: None

*******************************************************************************/
void mfw_audio_player_init(void)
{
	NUM_INPUT_FILES = mfw_audio_player_populate_files(INPUT_FILES_STRING, PLAYER_MAX_FILES);
	mp3_parameter.mono_stereo = AUDIO_MP3_MONO;
#ifdef FF_MMI_TEST_AAC
	aac_parameter.mono_stereo= AUDIO_AAC_MONO;
#endif
}

/*******************************************************************************

 $Function:     mfw_audio_player_set_channel_mono

 $Description:set channel to mono

 $Returns: Status

 $Arguments: None

*******************************************************************************/
SHORT mfw_audio_player_set_channel_mono(void)
{
	TRACE_FUNCTION("mfw_audio_player_set_channel_mono");

	switch(IsMP3On)
	{
	case MFW_PLAYER_MP3:
		TRACE_EVENT("MP3");
		mp3_parameter.mono_stereo = AUDIO_MP3_MONO;
	break;

#ifdef FF_MMI_TEST_AAC				
	case MFW_PLAYER_AAC:
		TRACE_EVENT("AAC");
		aac_parameter.mono_stereo= AUDIO_AAC_MONO;
	break;
#endif				
	default:
	break;
	}

	return MFW_PLAYER_TEST_OK;
}


/*******************************************************************************

 $Function:     mfw_audio_player_set_channel_stereo

 $Description:set channel to stereo

 $Returns: Status

 $Arguments: None

*******************************************************************************/
SHORT mfw_audio_player_set_channel_stereo(void)
{
TRACE_FUNCTION("mfw_audio_player_set_channel_stereo");

switch(IsMP3On)
	{
	case MFW_PLAYER_MP3:
		TRACE_EVENT("MP3");
		mp3_parameter.mono_stereo = AUDIO_MP3_STEREO;
	break;

#ifdef FF_MMI_TEST_AAC				
	case MFW_PLAYER_AAC:
		TRACE_EVENT("AAC");
		aac_parameter.mono_stereo= AUDIO_AAC_STEREO;
	break;
#endif				
	default:
	break;
	}

	return MFW_PLAYER_TEST_OK;
}




/*******************************************************************************

 $Function:     mfw_audio_player_mp3_start_cb

 $Description:callback function for starting mp3 file

 $Returns: None

 $Arguments: parameter: callback parameter

*******************************************************************************/
static void mfw_audio_player_mp3_start_cb (void *parameter)
{
 //Nov 16,2005 REF: DR OMAPS00049192  x0039928
   T_AUDIO_MP3_STATUS *param = (T_AUDIO_MP3_STATUS *)parameter;
   SHORT  event = param->status;
   UINT32 msg = param->os_hdr.msg_id;

#ifdef FF_MMI_AUDIO_PROFILE
		//configure the audio to voice path
		mfwAudPlay = FALSE;
		mfw_unset_stereo_path(mfw_get_current_audioDevice());
#endif
   
   switch(event)
   {
   // MMI callback function is called to destroy the focus window when AUDIO_OK
   // event is recieved for mp3 file play complete
   	case AUDIO_OK:
		if((mfw_mp3_playstate == TRUE) && (msg == AUDIO_MP3_STATUS_MSG))
			(* mmi_audio_player_return_path.callback)(mmi_audio_player_return_path.focus_win, AUDIO_OK);
		break;
//	Jun 02,2006 REF: DR OMAPS00079746  x0039928  
//   Fix: MP3 playback error is handled.
	case AUDIO_ERROR:
		if((mfw_mp3_playstate == TRUE) && (msg == AUDIO_MP3_STATUS_MSG))
			(* mmi_audio_player_return_path.callback)(mmi_audio_player_return_path.focus_win, AUDIO_ERROR);
		break;
	default:
		break;
    }
}
  

/*******************************************************************************

 $Function:     mfw_audio_player_aac_start_cb

 $Description:callback function for starting aac file

 $Returns: None

 $Arguments: parameter: callback parameter

*******************************************************************************/
#ifdef FF_MMI_TEST_AAC
static void mfw_audio_player_aac_start_cb(void *parameter)
{
    //Nov 16,2005 REF: DR OMAPS00049192  x0039928
   T_AUDIO_AAC_STATUS *param = (T_AUDIO_AAC_STATUS *)parameter;
   SHORT  event = param->status;
   UINT32 msg = param->os_hdr.msg_id;

#ifdef FF_MMI_AUDIO_PROFILE
		//configure the audio to voice path
		mfwAudPlay = FALSE;
		mfw_unset_stereo_path(mfw_get_current_audioDevice());
#endif

   switch(event)
   {
   // MMI callback function is called to destroy the focus window when AUDIO_OK
   // event is recieved for mp3 file play complete
   	case AUDIO_OK:
		if((mfw_mp3_playstate == TRUE) &&(msg == AUDIO_AAC_STATUS_MSG))
		(* mmi_audio_player_return_path.callback)(mmi_audio_player_return_path.focus_win, AUDIO_OK);
		break;
//	Jun 02,2006 REF: DR OMAPS00079746  x0039928  
//   Fix: MP3 playback error is handled.
	case AUDIO_ERROR:
		if((mfw_mp3_playstate == TRUE) &&(msg == AUDIO_AAC_STATUS_MSG))
			(* mmi_audio_player_return_path.callback)(mmi_audio_player_return_path.focus_win, AUDIO_ERROR);
		break;
	default:
		break;
    }
}
#endif

/*******************************************************************************

 $Function:     mfw_audio_player_configure_callback_fn

 $Description:Configure the Riviera Return Path

 $Returns:None

 $Arguments: callback function 

*******************************************************************************/

static void mfw_audio_player_configure_callback_fn(void (*callback_fn)(void *))
{
	audio_player_return_path.addr_id = 0;
	audio_player_return_path.callback_func = callback_fn;

	return;
}



/*******************************************************************************

 $Function:     mfw_audio_player_play

 $Description:play the file which was loaded
   When no file was manually selected, play file located at first index by default

 $Returns:Status

 $Arguments: None

*******************************************************************************/
SHORT mfw_audio_player_play(void)
{

	SHORT  mfw_audio_player_retVal;
	TRACE_FUNCTION("mfw_audio_player_play");

	//Aug 05, 2005     xdeepadh
	#ifdef FF_MMI_AUDIO_PROFILE
	//Configure the audio path to current Audio device.
	mfw_set_stereo_path(mfw_get_current_audioDevice());
	mfwAudPlay =  TRUE; //File is playing
 	#endif
	//Nov 24,2005 REF: DR OMAPS00049192  x0039928
	// Set mp3 play state to True
	mfw_mp3_playstate = TRUE;

	
	//Based on the file selected, call the respective APIS to start playing
	switch(IsMP3On)
		{
		case MFW_PLAYER_MP3 :
#ifdef FF_MMI_FILEMANAGER
			sprintf(tempFilePath,"/FFS/%s/%s",PLAYER_DIR, inputFileName);
			convert_u8_to_unicode(tempFilePath,mp3_parameter.mp3_name);
#else
			sprintf(tempFilePath,"%s/%s",PLAYER_DIR, inputFileName);
			strcpy(mp3_parameter.mp3_name, tempFilePath);
#endif
			mfw_audio_player_configure_callback_fn(mfw_audio_player_mp3_start_cb);
			
		if (audio_mp3_start(&mp3_parameter, audio_player_return_path))
		{
			mfw_audio_player_retVal = MFW_PLAYER_TEST_OK;
		}
		else
		{
			mfw_audio_player_retVal = MFW_PLAYER_TEST_RIVIERA_FAILED;
		}
		break;

#ifdef FF_MMI_TEST_AAC		

		case MFW_PLAYER_AAC:
#ifdef FF_MMI_FILEMANAGER
			sprintf(tempFilePath,"/FFS/%s/%s",PLAYER_DIR, inputFileName);
			convert_u8_to_unicode( tempFilePath,aac_parameter.aac_name);
#else			
			sprintf(tempFilePath,"%s/%s",PLAYER_DIR, inputFileName);
			strcpy(aac_parameter.aac_name, tempFilePath);
#endif
			mfw_audio_player_configure_callback_fn(mfw_audio_player_aac_start_cb);
			
		if (audio_aac_start(&aac_parameter, audio_player_return_path))
		{
			mfw_audio_player_retVal = MFW_PLAYER_TEST_OK;
		}
		else
		{
			mfw_audio_player_retVal = MFW_PLAYER_TEST_RIVIERA_FAILED;
		}
		break;
#endif	
		default:
			mfw_audio_player_retVal = MFW_PLAYER_TEST_RIVIERA_FAILED;
		break;
	}
		return mfw_audio_player_retVal;
}



/*******************************************************************************

 $Function:     mfw_audio_player_stop

 $Description:stop the file which was played

 $Returns:Status

 $Arguments: None

*******************************************************************************/
SHORT mfw_audio_player_stop(void)
{

	SHORT  mfw_audio_player_retVal;
	UINT32 size_played;
	// Aug 05, 2005     xdeepadh
#ifdef FF_MMI_AUDIO_PROFILE
	//configure the audio to voice path
	mfwAudPlay = FALSE;
	mfw_unset_stereo_path(mfw_get_current_audioDevice());
#endif
	TRACE_FUNCTION("mfw_audio_player_stop");
// Nov 24, 2005 REF : DR OMAPS00049192   x0039928
// Set mp3 play state to false
	mfw_mp3_playstate = FALSE;

	//Based on the file selected, call the respective APIS to stop playing
	switch(IsMP3On)
	{
	case MFW_PLAYER_MP3 :
	if (audio_mp3_stop(&size_played))
	{
		mfw_audio_player_retVal = MFW_PLAYER_TEST_OK;
	}
	else
	{
		mfw_audio_player_retVal = MFW_PLAYER_TEST_RIVIERA_FAILED;
	}
	break;
	
#ifdef FF_MMI_TEST_AAC		
	case MFW_PLAYER_AAC:
	if(audio_aac_stop(&size_played))
	{
		mfw_audio_player_retVal = MFW_PLAYER_TEST_OK;
	}
	else
	{
		mfw_audio_player_retVal = MFW_PLAYER_TEST_RIVIERA_FAILED;
	}
	break;
#endif	
	default:
		mfw_audio_player_retVal = MFW_PLAYER_TEST_RIVIERA_FAILED;	
	break;
}

	return mfw_audio_player_retVal;
}

#if (BOARD == 71)
/*******************************************************************************

 $Function:     mfw_audio_player_forward
 $Description: forward the file which is being played
 $Returns:Status

 $Arguments: None

*******************************************************************************/
SHORT mfw_audio_player_forward(UINT32 time)
{
	SHORT  mfw_audio_player_retVal;
	
	if(audio_mp3_forward(time))
		mfw_audio_player_retVal = MFW_PLAYER_TEST_OK;
	else
		mfw_audio_player_retVal = MFW_PLAYER_TEST_RIVIERA_FAILED;

	return mfw_audio_player_retVal;
}

/*******************************************************************************

 $Function:     mfw_audio_player_rewind
 $Description: rewind the file which is being played
 $Returns:Status

 $Arguments: None

*******************************************************************************/
SHORT mfw_audio_player_rewind(UINT32 time)
{
	SHORT  mfw_audio_player_retVal;
	
	if(audio_mp3_rewind(time)) 
		mfw_audio_player_retVal = MFW_PLAYER_TEST_OK;
	else
		mfw_audio_player_retVal = MFW_PLAYER_TEST_RIVIERA_FAILED;

	return mfw_audio_player_retVal;
}
#endif

/*******************************************************************************

 $Function:     mfw_audio_player_pause
 $Description:pause the file which was played
 $Returns:Status

 $Arguments: None

*******************************************************************************/
SHORT mfw_audio_player_pause(void)
{
	SHORT  mfw_audio_player_retVal;
	
	TRACE_FUNCTION("mfw_audio_player_pause");
	//Based on the file selected, call the respective APIS to pause playing		
	switch(IsMP3On)
	{
	case MFW_PLAYER_MP3 :
		if (audio_mp3_pause())
		{
			mfw_audio_player_retVal = MFW_PLAYER_TEST_OK;
		}
		else
		{
			mfw_audio_player_retVal = MFW_PLAYER_TEST_RIVIERA_FAILED;
		}
		break;
		
#ifdef FF_MMI_TEST_AAC		
	case MFW_PLAYER_AAC:
		if (audio_aac_pause())
		{
			mfw_audio_player_retVal = MFW_PLAYER_TEST_OK;
		}
		else
		{
			mfw_audio_player_retVal = MFW_PLAYER_TEST_RIVIERA_FAILED;
		}
		break;
#endif	
	default:
			mfw_audio_player_retVal = MFW_PLAYER_TEST_RIVIERA_FAILED;	
	break;
	}
	return mfw_audio_player_retVal;
}



/*******************************************************************************

 $Function:     mfw_audio_player_resume

 $Description:resume the file which was played
 $Returns:Status

 $Arguments: None

*******************************************************************************/
SHORT mfw_audio_player_resume(void)
{
	SHORT  mfw_audio_player_retVal;

	TRACE_FUNCTION("mfw_audio_player_resume");
//Based on the file selected, call the respective APIS to resume playing		
	switch(IsMP3On)
	{
	case MFW_PLAYER_MP3 :
		if (audio_mp3_resume())
		{
			mfw_audio_player_retVal = MFW_PLAYER_TEST_OK;
		}
		else
		{
			mfw_audio_player_retVal = MFW_PLAYER_TEST_RIVIERA_FAILED;
		}
		break;
		
#ifdef FF_MMI_TEST_AAC		
	case MFW_PLAYER_AAC:
		if (audio_aac_resume())
		{
			mfw_audio_player_retVal = MFW_PLAYER_TEST_OK;
		}
		else
		{
			mfw_audio_player_retVal = MFW_PLAYER_TEST_RIVIERA_FAILED;
		}
		break;
#endif	
	default:
		mfw_audio_player_retVal = MFW_PLAYER_TEST_RIVIERA_FAILED;
		break;
	}
	return mfw_audio_player_retVal;
}

/*******************************************************************************

 $Function:     mfw_audio_player_return_file_number

 $Description:resume the file which was played
 $Returns:Status

 $Arguments: None

*******************************************************************************/
int mfw_audio_player_return_file_number(void)
{
   return NUM_INPUT_FILES; 
}

/*******************************************************************************

 $Function:     mfw_audio_player_return_file_name

 $Description: return file name given an input index
 $Returns:file name of the selected index

 $Arguments: index

*******************************************************************************/
char* mfw_audio_player_return_file_name(int index)
{
      return INPUT_FILES_STRING[index];
}


/*******************************************************************************

 $Function:     mfw_audio_player_save_selected_file_idx

 $Description:return file name given an input index
 $Returns:status

 $Arguments: index

*******************************************************************************/

SHORT mfw_audio_player_save_selected_file_idx(int index)
{
    //default index is set to 0 and inputFileName to NULL at mp3 mfw initialisation
    //save currently selected file to control block
     audio_player_currently_playing_idx = index;
    inputFileName = INPUT_FILES_STRING[index];
    return MFW_PLAYER_TEST_OK;
	
}
 //  Jul 18, 2005    REF: SPR 31695   xdeepadh

 /*******************************************************************************

 $Function:     mfw_audio_player_GetExtension

 $Description: public function to retrieve the extension of a file

 $Returns:Extention of the filename

 $Arguments: scr- Filename

*******************************************************************************/
 char *mfw_audio_player_GetExtension(char *src)
{
    U8 i;
		TRACE_FUNCTION("mfw_audio_player_GetExtension");
    for(i = 0; i < strlen(src); i++){
        if(src[i] == '.'){
            return (src+i+1);
        }
    }
    return (src+i);
}


