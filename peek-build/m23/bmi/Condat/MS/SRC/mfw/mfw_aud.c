/*
+--------------------------------------------------------------------+
| PROJECT:	MMI-Framework (8417)		$Workfile::	mfw_aud.c	    $|
| $Author::	NDH							$Revision::	1			    $|
| CREATED:	04.02.03		     		$Modtime::	10.04.00 14:58	$|
| STATE  :	code														 |
+--------------------------------------------------------------------+

   MODULE  : MFW_AUD

   PURPOSE : This module contains Audio Riveria Interface functions.

   HISTORY:

	Mar 28, 2007  DR: OMAPS00122762 x0039928
	Description: MM: Deleting a PCM Voice Memo message in one particular memory, 
	delete them in all memories
	Solution: voice memo position and pcm voice memo position is provided for all the devices.
	
    July 03,2006 REF :OMAPS00083150  x0047075
    Description :Audio muted on call swap / hold
    Solution :Function call hl_drv_enable_vocoder() and hl_drv_disable_vocoder() are replaced with hl_drv_set_vocoder_state()  
	Mar 15, 2006   ER: OMAPS00067709  x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Defined new macros of folder and file names for PCM voice memo and Voice buffering
			Defined new macros for Microphone and network gain of PCM VM and Voice buffering
			Defined new functions set_voice_memo_type, get_voice_memo_type, set_voice_buffering_rec_stop_reason
		Function: mfw_aud_vm_delete_file
		Changes:Handling file deletion forPCM voice memo also

		Function: mfw_aud_vm_start_playback, mfw_aud_vm_stop_playback, mfw_aud_vm_start_record,
				mfw_aud_vm_stop_record, mfw_aud_vm_get_duration, mfw_aud_vm_set_duration
		Changes: Added code to call respective audio APIs for PCM voice memo and voice buffering

	Feb 24, 2006    ER: OMAPS00067709 x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Defined a global variable gPcm_voice_Memo which indicates which Voice memo is active 
			and corresponding audio APIs will be invoked and duplication of code is avoided
	
	Feb 24, 2006    ER: OMAPS00067709 x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Adding new macros PCM_VM_FILE_NAME, PCM_VM_FOLDER to define new files for PCM voice memo
			Following functions are implemented to support PCM voice memo feature
				mfw_aud_vm_pcm_delete_file: Delete the file which held the PCM Voice Memo
				mfw_aud_vm_pcm_start_playback: Start playback of a previously recorded PCM Voice Memo
				mfw_aud_vm_pcm_stop_playback: Stop playback of a previously recorded PCM Voice Memo
				mfw_aud_vm_pcm_start_record: Configure Riviera and start recording a PCM Voice Memo
				mfw_aud_vm_pcm_stop_record: Stop recording a PCM Voice Memo
				mfw_aud_vm_pcm_get_duration: Get the duration of the previously recorded PCM Voice Memo
				mfw_aud_vm_pcm_set_duration: Set the duration of the previously recorded PCM Voice Memo
	
    Apr 06, 2006    ERT: OMAPS00070660 x0039928(sumanth)
    Description: Need to reduce flash foot-print for Locosto Lite 
    Solution: Voice Memo feature is put under the flag #ifndef FF_NO_VOICE_MEMO to compile 
    out voice memo feature if the above flag is enabled.

    Mar 03, 2005 REF: CRR MMI-ENH-28950 xnkulkar
    Description:  RE: Vocoder interface change
    Solution: Function call 'enable_tch_vocoder()' is replaced with new functions 'hl_drv_enable_vocoder()'
    		    and 'hl_drv_disable_vocoder()'

    Aug 25, 2004  REF: CRR 20655  xnkulkar
    Description: Voice Memo functionality not working
    Solution:	  The voice recording functionality was failing because 
    			 " mmi" folder is not present. As a solution, we create the 
    			 "mmi" folder and then proceed with recording.
*/


/*
** Include Files
*/

#define ENTITY_MFW

/* BEGIN ADD: Req ID: : Sumit : 14-Mar-05 */
#ifndef NEPTUNE_BOARD
/* END ADD: Req ID: : Sumit : 14-Mar-05 */

/* includes */
#include <string.h>

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#include "mfw_ffs.h"
#include "mfw_aud.h"
#include "mfw_fm.h"
#ifndef FF_NO_VOICE_MEMO
#include "Audio/audio_api.h"
#endif

// Mar 03, 2005 REF: CRR MMI-ENH-28950 xnkulkar
#include "hl_audio_drv.h"

//x0pleela 06 Jul, 2006 DR: OMAPS00067709
//fix from AS team
#ifdef FF_PCM_VM_VB
#include "l1audio_cust.h"
#endif 
/*
** Local Variable Definitions
*/

#ifndef FF_NO_VOICE_MEMO
static T_RV_RETURN_PATH voice_memo_return_path;


/*
** Local Macro Definitions
*/

#define VM_FILE_NAME	"vmfile"	// FFS File Name
#define VM_FOLDER		"/mmi/vm"

//x0pleela 24 Feb, 2006  ER OMAPS00067709
#ifdef FF_PCM_VM_VB
#define PCM_VM_FILE_NAME	"pcmvm"			// FFS PCM voice memo File Name
#define PCM_VM_FOLDER		"/mmi/pcmvm" 	//PCM oice memo folder

//x0pleela 09 Mar, 2006  ER OMAPS00067709
#define VOICE_BUFF_FILE_NAME	"vbfile"		// FFS Voice buffering File Name
#define VOICE_BUFF_FOLDER		"/mmi/vb" 	//Voice buffering folder

//x0pleela 01 Mar, 2006  ER OMAPS00067709
//Microphone and network gain for PCM VM and Voice buffering
#define PCM_VM_MICROPHONE_GAIN		(0x20)	// Enable recording from microphone
#define PCM_VM_NETWORK_GAIN			(0x20)	// Enable recording from network
#endif

#define VM_MICROPHONE_GAIN		(0x0100)	// Default Gain of 1
#define VM_NETWORK_GAIN			(0x0100)	// Default Gain of 1

#define VM_TONE_DURATION		(50)
#define VM_TONE_INTERVAL		(50)
#define VM_TONE_AMPLITUDE		(-24)		// Default Amplitude of -24dB

//x0pleela 27 Feb, 2006   ER:OMAPS00067709
#ifdef FF_PCM_VM_VB
static T_VOICEMEMO VoiceMemoType;			/* Voice memo type*/	

//x0pleela 09 Mar, 2006  ER OMAPS00067709
GLOBAL  T_voice_buffering voice_buffering_data; /* Voice buffering data */
#endif


/*
** Local Function Prototypes
*/

static void mfw_aud_vm_create_file(const char *folder, const char *fname, UBYTE index);
static void configure_callback_fn(void (*callback_fn)(void *));
#ifdef FF_MMI_FILEMANAGER
static void configure_vm_filename(UINT16 *vm_filename, const char *folder, const char *fname, UBYTE index);
#else
static void configure_vm_filename(char *vm_filename, const char *folder, const char *fname, UBYTE index);
#endif

/*
** Public function Definitions
*/

#ifdef FF_PCM_VM_VB
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_AUD		             |
| STATE  : code 			ROUTINE: set_voice_memo_type	         |
+--------------------------------------------------------------------+

   PURPOSE :  Function to set the type of voice memo
*/
//x0pleela 09 Mar, 2006  ER:OMAPS00067709

void set_voice_memo_type( T_VOICEMEMO voice_memo_type)
{
  TRACE_FUNCTION("set_voice_memo_type()");
  VoiceMemoType = voice_memo_type;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_AUD		             |
| STATE  : code 			ROUTINE: get_voice_memo_type	         |
+--------------------------------------------------------------------+

   PURPOSE :  Function to get the type of voice memo
*/
//x0pleela 08 Mar, 2006  ER:OMAPS00067709

GLOBAL T_VOICEMEMO get_voice_memo_type( void)
{
   TRACE_FUNCTION("get_voice_memo_type()");
   return VoiceMemoType;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_AUD		             |
| STATE  : code 			ROUTINE: set_voice_buffering_rec_stop_reason	         |
+--------------------------------------------------------------------+

   PURPOSE :  Function to set the reason to call record stop
*/
//x0pleela 07 Mar, 2006  ER:OMAPS00067709
GLOBAL void set_voice_buffering_rec_stop_reason( T_VOICE_BUFFERING_STATUS reason)
{
  TRACE_FUNCTION("set_voice_memo_type()");
  voice_buffering_data.rec_stop_reason = reason;
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_AUD		             |
| STATE  : code 			ROUTINE: mfw_aud_vm_delete_file	         |
+--------------------------------------------------------------------+


   PURPOSE :  Delete the file which held the Voice Memo

*/
SHORT mfw_aud_vm_delete_file(void)
{
#ifdef FF_MMI_FILEMANAGER
	UINT16 tmpFile[AUDIO_PATH_NAME_MAX_SIZE];
       char path[AUDIO_PATH_NAME_MAX_SIZE];
#else
	char tmpFile[AUDIO_PATH_NAME_MAX_SIZE];
#endif

	TRACE_FUNCTION("mfw_aud_vm_delete_file");
	memset(tmpFile, 0x00, AUDIO_PATH_NAME_MAX_SIZE);
#ifdef FF_PCM_VM_VB
	//x0pleela 06 Mar, 2006  ER:OMAPS00067709
 	//deleting PCM Voice memo file
	if( get_voice_memo_type() EQ PCM_VOICE_MEMO)
#ifdef FF_MMI_FILEMANAGER		
{
	switch(FFS_flashData.PCM_voicememo_storage)
	  {
	  	case SNAP_STG_FFS:
			strcpy(path, "/FFS");
			break;

	  	case SNAP_STG_NORMS:
			strcpy(path, "/NOR");
			break;
			
		case SNAP_STG_NAND:
			strcpy(path, "/NAND");
			break;

		case SNAP_STG_MMC:
			strcpy(path, "/MMC");
			break;
	  	}

	  strcat(path, PCM_VM_FOLDER);
	  configure_vm_filename(tmpFile, path, PCM_VM_FILE_NAME, 0);
}	  
#else	  
	  configure_vm_filename(tmpFile, PCM_VM_FOLDER, PCM_VM_FILE_NAME, 0);
#endif
	else 
	{
	  if (get_voice_memo_type() EQ AMR_VOICE_MEMO)
#endif	  	
 #ifdef FF_MMI_FILEMANAGER	
 {
	  switch(FFS_flashData.voicememo_storage)
	  {
	  	case SNAP_STG_FFS:
			strcpy(path, "/FFS");
			break;

		case SNAP_STG_NORMS:
			strcpy(path, "/NOR");
			break;
			
		case SNAP_STG_NAND:
			strcpy(path, "/NAND");
			break;

		case SNAP_STG_MMC:
			strcpy(path, "/MMC");
			break;
	  	}

	  strcat(path, VM_FOLDER);
	  configure_vm_filename(tmpFile, path, VM_FILE_NAME, 0);
 }	  
#else
	  configure_vm_filename(tmpFile, VM_FOLDER, VM_FILE_NAME, 0);
#endif

#ifdef FF_PCM_VM_VB
	}
#endif
#ifdef FF_MMI_FILEMANAGER
	rfs_remove(tmpFile);
#else
	ffs_remove(tmpFile);
#endif

	return MFW_AUD_VM_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_AUD		             |
| STATE  : code 			ROUTINE: mfw_aud_vm_start_playback       |
+--------------------------------------------------------------------+


   PURPOSE :  Start playback of a previously recorded Voice Memo

*/
SHORT mfw_aud_vm_start_playback(void (*callback_fn)(void *))
{
	T_AUDIO_RET						audio_riv_retVal=AUDIO_ERROR;
	T_AUDIO_VM_PLAY_PARAMETER		mfw_vm_play_param;

 #ifdef FF_MMI_FILEMANAGER	
	char path[AUDIO_PATH_NAME_MAX_SIZE];
	 T_RFS_STAT fstat;         /* Mar 28, 2007  DR: OMAPS00122762 x0039928 */
 #endif
	//x0pleela 01 Mar, 2006   ER:OMAPS00067709	
#ifdef FF_PCM_VM_VB
       T_AUDIO_VM_PCM_PLAY_PARAMETER  mfw_vm_pcm_play_param; /* Defined new variable for pcm VM */
	T_AUDIO_VBUF_PCM_PLAY_PARAMETER  mfw_vbuf_pcm_play_param; /* Defined new variable for voice buffering*/	
	UBYTE vm_type; /*to store voice memo type */
#endif

	TRACE_FUNCTION("mfw_aud_vm_start_playback");
#ifndef FF_MMI_FILEMANAGER              /* Mar 28, 2007  DR: OMAPS00122762 x0039928*/
#ifdef FF_PCM_VM_VB
	//x0pleela 23 Mar, 2006  ER:OMAPS00067709
	switch( get_voice_memo_type() )
	{
	  case AMR_VOICE_MEMO:
#endif /*  FF_PCM_VM_VB	  */
	if (FFS_flashData.voice_memo_position EQ 0)
	{
		return MFW_AUD_VM_MEM_EMPTY;
	}
#ifdef FF_PCM_VM_VB		
	  	break;

	  case PCM_VOICE_MEMO:
		if (FFS_flashData.pcm_voice_memo_position EQ 0)
		{
		  return MFW_AUD_VM_MEM_EMPTY;
	       }
	  	break;
	  default:
	  	break;
	}
#endif /*  FF_PCM_VM_VB	  */
#endif
		
//x0pleela 27 Feb, 2006   ER:OMAPS00067709
//Set up the PCM Voice Memo filename in FFS
#ifdef FF_PCM_VM_VB
	//get the voice memo type
	vm_type = get_voice_memo_type();
	if( vm_type EQ PCM_VOICE_MEMO )
 #ifdef FF_MMI_FILEMANAGER	
 {
	  switch(FFS_flashData.PCM_voicememo_storage)
	  {
	  	case SNAP_STG_FFS:
			strcpy(path, "/FFS");
			break;

	  	case SNAP_STG_NORMS:
			strcpy(path, "/NOR");
			break;

		case SNAP_STG_NAND:
			strcpy(path, "/NAND");
			break;

		case SNAP_STG_MMC:
			strcpy(path, "/MMC");
			break;
	  	}

	  strcat(path, PCM_VM_FOLDER);
	  configure_vm_filename(mfw_vm_pcm_play_param.memo_name, path, PCM_VM_FILE_NAME, 0);
/* Mar 28, 2007  DR: OMAPS00122762 x0039928 */
/* Fix: File empty is returned if the file desnot exist */
#ifdef FF_MMI_FILEMANAGER
if(rfs_stat(mfw_vm_pcm_play_param.memo_name,&fstat) != RFS_EOK)
{
	return MFW_AUD_VM_MEM_EMPTY;
}
#endif	  
 }	  
#else
	  configure_vm_filename(mfw_vm_pcm_play_param.memo_name, PCM_VM_FOLDER, PCM_VM_FILE_NAME, 0);
#endif
	//x0pleela 09 Mar, 2006   ER:OMAPS00067709
	//Set up the Voice buffering filename in FFS
	else if (vm_type EQ VOICE_BUFFERING )
 #ifdef FF_MMI_FILEMANAGER	
 {
	  switch(FFS_flashData.voicebuffer_storage)
	  {
	  	case SNAP_STG_FFS:
			strcpy(path, "/FFS");
			break;

	  	case SNAP_STG_NORMS:
			strcpy(path, "/NOR");
			break;
			
		case SNAP_STG_NAND:
			strcpy(path, "/NAND");
			break;

		case SNAP_STG_MMC:
			strcpy(path, "/MMC");
			break;
	  	}

	  strcat(path, VOICE_BUFF_FOLDER);
	  configure_vm_filename(mfw_vbuf_pcm_play_param.memo_name, path, VOICE_BUFF_FILE_NAME, 0);
 }	  
#else		
	  configure_vm_filename(mfw_vbuf_pcm_play_param.memo_name, VOICE_BUFF_FOLDER, VOICE_BUFF_FILE_NAME, 0);
#endif
	else 
	{
	  if (vm_type EQ AMR_VOICE_MEMO)
#endif
 #ifdef FF_MMI_FILEMANAGER	
 {
	  switch(FFS_flashData.voicememo_storage)
	  {
	  	case SNAP_STG_FFS:
			strcpy(path, "/FFS");
			break;

	  	case SNAP_STG_NORMS:
			strcpy(path, "/NOR");
			break;
			
		case SNAP_STG_NAND:
			strcpy(path, "/NAND");
			break;

		case SNAP_STG_MMC:
			strcpy(path, "/MMC");
			break;
	  	}

	  strcat(path, VM_FOLDER);
	  configure_vm_filename(mfw_vm_play_param.memo_name, path, VM_FILE_NAME, 0);
/* Mar 28, 2007  DR: OMAPS00122762 x0039928 */
/* Fix: File empty is returned if the file desnot exist */
#ifdef FF_MMI_FILEMANAGER
if(rfs_stat(mfw_vm_play_param.memo_name,&fstat) != RFS_EOK)
{
	return MFW_AUD_VM_MEM_EMPTY;
}
#endif
 }	  
#else
		//Set up the Voice Memo filename in FFS
		configure_vm_filename(mfw_vm_play_param.memo_name, VM_FOLDER, VM_FILE_NAME, 0);
#endif
#ifdef FF_PCM_VM_VB
	}
#endif
		// Call Riviera function to start playback.
#ifndef WIN32	// only if not in Windows
//x0pleela 27 Feb, 2006   ER:OMAPS00067709

configure_callback_fn(callback_fn);
#ifdef FF_PCM_VM_VB
	//x0pleela 06 Mar, 2006  ER:OMAPS00067709
	if( vm_type EQ PCM_VOICE_MEMO )
	{
    	  //update mfw_vm_pcm_play_param
  	  mfw_vm_pcm_play_param.memo_duration = mfw_aud_vm_get_duration();
  	  mfw_vm_pcm_play_param.speaker_gain = PCM_VM_MICROPHONE_GAIN;
  	  mfw_vm_pcm_play_param.network_gain= 0; 

	  //Start playing PCM Voice memo
  	  audio_riv_retVal = audio_vm_pcm_play_start(&mfw_vm_pcm_play_param,
											 voice_memo_return_path);
  	}
	else if ( vm_type EQ VOICE_BUFFERING )
	{
	  //update mfw_vbuf_pcm_play_param
  	  mfw_vbuf_pcm_play_param.memo_duration = PCM_VOICE_MEMO_MAX_DURATION;
  	  mfw_vbuf_pcm_play_param.speaker_gain = 0;
  	  mfw_vbuf_pcm_play_param.network_gain= PCM_VM_NETWORK_GAIN; 

	  //x0pleela 06 Jul, 2006  DR: OMAPS00067709
	  //Fix from AS team
	  vocoder_mute_ul(1);

	  //Set up the Voice buffering filename in FFS
         audio_riv_retVal = audio_voice_buffering_pcm_play_start (&mfw_vbuf_pcm_play_param,
											 voice_memo_return_path);
	}
	else
	{
	  if (vm_type EQ AMR_VOICE_MEMO)
#endif
		audio_riv_retVal = audio_vm_play_start(&mfw_vm_play_param,
							 				voice_memo_return_path);
#ifdef FF_PCM_VM_VB
	}
#endif
		// If the Riviera call failed
		if (audio_riv_retVal != RV_OK)
			return MFW_AUD_VM_RIVIERA_FAILED;
#endif
	return MFW_AUD_VM_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_AUD		             |
| STATE  : code 			ROUTINE: mfw_aud_vm_stop_playback        |
+--------------------------------------------------------------------+


   PURPOSE :  Stop playback of a previously recorded Voice Memo

*/
SHORT mfw_aud_vm_stop_playback(void (*callback_fn)(void *))
{
	T_AUDIO_RET						audio_riv_retVal=AUDIO_ERROR;
//x0pleela 06 Mar, 2006  ER:OMAPS00067709
#ifdef FF_PCM_VM_VB  
  UBYTE vm_type; //to store voice memo type
#endif

	TRACE_FUNCTION("mfw_aud_vm_stop_playback");
	configure_callback_fn(callback_fn);

#ifndef WIN32	// only if not in Windows
#ifdef FF_PCM_VM_VB
	//x0pleela 06 Mar, 2006  ER:OMAPS00067709
	//get the voice memo type
	vm_type = get_voice_memo_type();

	//x0pleela 27 Feb, 2006   ER:OMAPS00067709
	//Stop playing PCM Voice memo
	if( vm_type EQ PCM_VOICE_MEMO )
	  audio_riv_retVal = audio_vm_pcm_play_stop(voice_memo_return_path);
	//Stop playing 
	else if ( vm_type EQ VOICE_BUFFERING )
	{
	  audio_riv_retVal = audio_voice_buffering_pcm_play_stop (voice_memo_return_path); 
	  //x0pleela 06 Jul, 2006  DR: OMAPS00067709
	  //Fix from AS team
	  vocoder_mute_ul(0);  
	}
	else
	{
	   if (vm_type EQ AMR_VOICE_MEMO)
#endif
	audio_riv_retVal = audio_vm_play_stop(voice_memo_return_path);
#ifdef FF_PCM_VM_VB
	}
#endif
	// If the Riviera call failed
	if (audio_riv_retVal != RV_OK)
		return MFW_AUD_VM_RIVIERA_FAILED;

#endif

	return MFW_AUD_VM_OK;
}


#ifdef FF_MMI_FILEMANAGER
void path_init(UBYTE vmtype)
{
	T_RFS_RET ffsResult;
	T_RFS_DIR f_dir, f_dir1;
	char dir_path[FM_MAX_DIR_PATH_LENGTH];
	UINT16 dir_path_uc[FM_MAX_DIR_PATH_LENGTH];
	
	memset(dir_path, 0, FM_MAX_DIR_PATH_LENGTH);
	switch(vmtype)
	{
		case AMR_VOICE_MEMO:
			switch(FFS_flashData.voicememo_storage)
			{
				case SNAP_STG_FFS:
					strcpy(dir_path, "/FFS/mmi");
					break;

				case SNAP_STG_NORMS:
					strcpy(dir_path, "/NOR/mmi");
					break;
					
				case SNAP_STG_NAND:
					strcpy(dir_path, "/NAND/mmi");
					break;

				case SNAP_STG_MMC:
					strcpy(dir_path, "/MMC/mmi");
					break;
			}
			convert_u8_to_unicode(dir_path, dir_path_uc);
			ffsResult = rfs_opendir(dir_path_uc,&f_dir);       
			TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);

			if(RFS_ENOENT == ffsResult)
			{
				ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
				TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
				if(ffsResult == 0)
				{
					strcat(dir_path, "/vm");
					convert_u8_to_unicode(dir_path, dir_path_uc);
					ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
					TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
				}
			}
			else if(ffsResult > 0)
				{
					strcat(dir_path, "/vm");
					convert_u8_to_unicode(dir_path, dir_path_uc);
					ffsResult = rfs_opendir(dir_path_uc,&f_dir1);       
					TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);

					if(RFS_ENOENT == ffsResult)
					{
						ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
						TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
					}
					else if (ffsResult > 0)
						rfs_closedir(&f_dir1);	

					rfs_closedir(&f_dir);	
				}
			break;

		case PCM_VOICE_MEMO:
			switch(FFS_flashData.PCM_voicememo_storage)
			{
				case SNAP_STG_FFS:
					strcpy(dir_path, "/FFS/mmi");
					break;

				case SNAP_STG_NORMS:
					strcpy(dir_path, "/NOR/mmi");
					break;
					
				case SNAP_STG_NAND:
					strcpy(dir_path, "/NAND/mmi");
					break;

				case SNAP_STG_MMC:
					strcpy(dir_path, "/MMC/mmi");
					break;
			}
			convert_u8_to_unicode(dir_path, dir_path_uc);
			ffsResult = rfs_opendir(dir_path_uc,&f_dir);       
			TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);

			if(RFS_ENOENT == ffsResult)
			{
				ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
				TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
				if(ffsResult == 0)
				{
					strcat(dir_path, "/pcmvm");
					convert_u8_to_unicode(dir_path, dir_path_uc);
					ffsResult = rfs_opendir(dir_path_uc,&f_dir1);       
					TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);

					if(RFS_ENOENT == ffsResult)
					{
						ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
						TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
					}
					else if (ffsResult > 0)
						rfs_closedir(&f_dir1);	

					rfs_closedir(&f_dir);	
				}
				
			}
			else if(ffsResult > 0)
			{
				strcat(dir_path, "/pcmvm");
				convert_u8_to_unicode(dir_path, dir_path_uc);
				ffsResult = rfs_opendir(dir_path_uc,&f_dir1);       
				TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);

				if(RFS_ENOENT == ffsResult)
				{
					ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
					TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
				}
			}
		
			break;

		case VOICE_BUFFERING:
			switch(FFS_flashData.voicebuffer_storage)
			{
				case SNAP_STG_FFS:
					strcpy(dir_path, "/FFS/mmi");
					break;

				case SNAP_STG_NORMS:
					strcpy(dir_path, "/NOR/mmi");
					break;
					
				case SNAP_STG_NAND:
					strcpy(dir_path, "/NAND/mmi");
					break;

				case SNAP_STG_MMC:
					strcpy(dir_path, "/MMC/mmi");
					break;
			}
			convert_u8_to_unicode(dir_path, dir_path_uc);
			ffsResult = rfs_opendir(dir_path_uc,&f_dir);       
			TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);

			if(RFS_ENOENT == ffsResult)
			{
				ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
				TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);

				if(ffsResult == 0)
			{
				strcat(dir_path, "/vb");
				convert_u8_to_unicode(dir_path, dir_path_uc);
				ffsResult = rfs_opendir(dir_path_uc,&f_dir1);       
				TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);

				if(RFS_ENOENT == ffsResult)
				{
					ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
					TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
				}
				}
			}
			else if(ffsResult > 0)
			{
				strcat(dir_path, "/vb");
				convert_u8_to_unicode(dir_path, dir_path_uc);
				ffsResult = rfs_opendir(dir_path_uc,&f_dir1);       
				TRACE_EVENT_P2("Opendir - ffsResult   %d  Dir path %s", ffsResult,dir_path);

				if(RFS_ENOENT == ffsResult)
				{
					ffsResult = rfs_mkdir(dir_path_uc, RFS_IRWXU);
					TRACE_EVENT_P2("Makedir - ffsResult   %d  Dir path %s", ffsResult,dir_path);
				}
				else if (ffsResult > 0)
						rfs_closedir(&f_dir1);	

					rfs_closedir(&f_dir);	
			}
			break;
	}
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_AUD		             |
| STATE  : code 			ROUTINE: mfw_aud_vm_start_record         |
+--------------------------------------------------------------------+


   PURPOSE :  Configure Riviera and start recording a Voice Memo

*/
SHORT mfw_aud_vm_start_record(UBYTE max_duration, void (*callback_fn)(void *))
{
	T_AUDIO_RET						audio_riv_retVal=AUDIO_ERROR;
	T_AUDIO_VM_RECORD_PARAMETER		mfw_vm_record_param;
	T_AUDIO_TONES_PARAMETER			mfw_vm_tones_param;

 #ifdef FF_MMI_FILEMANAGER	
 	char path[FM_MAX_DIR_PATH_LENGTH];
 #else
	char * mmiDir = "/mmi"; // Aug 25, 2004  REF: CRR 20655  xnkulkar
 #endif
 
//x0pleela 27 Feb, 2006   ER:OMAPS00067709
//defining new variable for PCM recording
#ifdef FF_PCM_VM_VB
	T_AUDIO_VM_PCM_RECORD_PARAMETER mfw_vm_pcm_record_param;
	T_AUDIO_VBUF_PCM_RECORD_PARAMETER mfw_vbuf_pcm_record_param;
	UBYTE vm_type; //to store voice memo type
#endif
	TRACE_FUNCTION("mfw_aud_vm_start_record");
	configure_callback_fn(callback_fn);

	memset(path, 0, FM_MAX_DIR_PATH_LENGTH);
//x0pleela 27 Feb, 2006   ER:OMAPS00067709
#ifdef FF_PCM_VM_VB
	//x0pleela 06 Mar, 2006  ER:OMAPS00067709
	//get the voice memo type
	vm_type = get_voice_memo_type();

       // We now create the "mmi" folder and then proceed with recording.	
 #ifdef FF_MMI_FILEMANAGER
	path_init(vm_type);
 #else
       flash_makedir(mmiDir);  
 #endif
 
	if( vm_type EQ PCM_VOICE_MEMO )
 #ifdef FF_MMI_FILEMANAGER	
 {
	  switch(FFS_flashData.PCM_voicememo_storage)
	  {
	  	case SNAP_STG_FFS:
			strcpy(path, "/FFS");
			break;

	  	case SNAP_STG_NORMS:
			strcpy(path, "/NOR");
			break;
			
		case SNAP_STG_NAND:
			strcpy(path, "/NAND");
			break;

		case SNAP_STG_MMC:
			strcpy(path, "/MMC");
			break;
	  	}
	  strcat(path, PCM_VM_FOLDER);
	  configure_vm_filename(mfw_vm_pcm_record_param.memo_name, path, PCM_VM_FILE_NAME, 0);
 }	  
#else		
  	  //Set up, and create, the PCM Voice Memo filename in FFS
	  configure_vm_filename(mfw_vm_pcm_record_param.memo_name, PCM_VM_FOLDER, PCM_VM_FILE_NAME, 0);
#endif
	//x0pleela 09 Mar, 2006   ER:OMAPS00067709
	else if( vm_type EQ VOICE_BUFFERING)
 #ifdef FF_MMI_FILEMANAGER	
 {
	  switch(FFS_flashData.voicebuffer_storage)
	  {
	  	case SNAP_STG_FFS:
			strcpy(path, "/FFS");
			break;

	  	case SNAP_STG_NORMS:
			strcpy(path, "/NOR");
			break;

		case SNAP_STG_NAND:
			strcpy(path, "/NAND");
			break;

		case SNAP_STG_MMC:
			strcpy(path, "/MMC");
			break;
	  	}

	  strcat(path, VOICE_BUFF_FOLDER);
	  configure_vm_filename(mfw_vbuf_pcm_record_param.memo_name, path, VOICE_BUFF_FILE_NAME, 0);
 }	  
#else			
  	  //Set up, and create, the Voice Buffering filename in FFS
	  configure_vm_filename(mfw_vbuf_pcm_record_param.memo_name, VOICE_BUFF_FOLDER, VOICE_BUFF_FILE_NAME, 0);
#endif
	else  
	{
	  if (vm_type EQ AMR_VOICE_MEMO)
#endif /* FF_PCM_VM_VB */
 #ifdef FF_MMI_FILEMANAGER	
 {
	  switch(FFS_flashData.voicememo_storage)
	  {
	  	case SNAP_STG_FFS:
			strcpy(path, "/FFS");
			break;

	  	case SNAP_STG_NORMS:
			strcpy(path, "/NOR");
			break;

		case SNAP_STG_NAND:
			strcpy(path, "/NAND");
			break;

		case SNAP_STG_MMC:
			strcpy(path, "/MMC");
			break;
	  	}

	  strcat(path, VM_FOLDER);
	 configure_vm_filename(mfw_vm_record_param.memo_name, path, VM_FILE_NAME, 0);
 }	  
#else	
	//Set up, and create, the Voice Memo filename in FFS
	configure_vm_filename(mfw_vm_record_param.memo_name, VM_FOLDER, VM_FILE_NAME, 0);
#endif
#ifdef FF_PCM_VM_VB
	}
#endif /* FF_PCM_VM_VB */
       //Aug 25, 2004  REF: CRR 20655  xnkulkar

       
//x0pleela 27 Feb, 2006   ER:OMAPS00067709
#ifdef FF_PCM_VM_VB
	if( vm_type EQ PCM_VOICE_MEMO )
	//create PCM Voice memo file
 #ifdef FF_MMI_FILEMANAGER	
	  mfw_aud_vm_create_file(path, PCM_VM_FILE_NAME, 0);
 #else
  	  mfw_aud_vm_create_file(PCM_VM_FOLDER, PCM_VM_FILE_NAME, 0);
 #endif
	//x0pleela 09 Mar, 2006   ER:OMAPS00067709
	else if( vm_type EQ VOICE_BUFFERING )
	//create Voice Buffering file
 #ifdef FF_MMI_FILEMANAGER
 	  mfw_aud_vm_create_file(path, VOICE_BUFF_FILE_NAME, 0);
 #else
  	  mfw_aud_vm_create_file(VOICE_BUFF_FOLDER, VOICE_BUFF_FILE_NAME, 0);
 #endif
	else 
	{
	   if (vm_type EQ AMR_VOICE_MEMO)
#endif       /* FF_PCM_VM_VB */
 #ifdef FF_MMI_FILEMANAGER
 	mfw_aud_vm_create_file(path, VM_FILE_NAME, 0);
 #else
	mfw_aud_vm_create_file(VM_FOLDER, VM_FILE_NAME, 0);
 #endif
#ifdef FF_PCM_VM_VB
	}
#endif /* FF_PCM_VM_VB */

#ifndef WIN32	// only if not in Windows
//x0pleela 27 Feb, 2006   ER:OMAPS00067709
//updating fields of structure T_AUDIO_VM_PCM_RECORD_PARAMETER for PCM VM recording
#ifdef FF_PCM_VM_VB
	if( vm_type EQ PCM_VOICE_MEMO )
	{
	  mfw_vm_pcm_record_param.memo_duration = (UINT32)max_duration;
	  mfw_vm_pcm_record_param.microphone_gain = PCM_VM_MICROPHONE_GAIN;
	  mfw_vm_pcm_record_param.network_gain = 0;

  	  audio_riv_retVal = audio_vm_pcm_record_start(&mfw_vm_pcm_record_param, 
												  voice_memo_return_path);
	}
	else if( vm_type EQ VOICE_BUFFERING )
	{
	  mfw_vbuf_pcm_record_param.memo_duration = (UINT32)max_duration;
	  mfw_vbuf_pcm_record_param.microphone_gain = PCM_VM_MICROPHONE_GAIN;
	  mfw_vbuf_pcm_record_param.network_gain = 0;
	  
	  audio_riv_retVal = audio_voice_buffering_pcm_record_start( &mfw_vbuf_pcm_record_param, 
	  													voice_memo_return_path);
	}
	else
	{
	  if (vm_type EQ AMR_VOICE_MEMO ) 
	  {
#endif /* FF_PCM_VM_VB */

	// Setup the Voice Memo Tones
	mfw_vm_record_param.memo_duration = (UINT32)max_duration;
	mfw_vm_record_param.compression_mode = FALSE;	// No Compression
	mfw_vm_record_param.microphone_gain = VM_MICROPHONE_GAIN;
	mfw_vm_record_param.network_gain = VM_NETWORK_GAIN;

	mfw_vm_tones_param.tones[0].start_tone = 0;
	mfw_vm_tones_param.tones[0].stop_tone = VM_TONE_DURATION;
	mfw_vm_tones_param.tones[0].frequency_tone = 520;			// Tone 1 Frequecny in Hz
	mfw_vm_tones_param.tones[0].amplitude_tone = VM_TONE_AMPLITUDE;

	mfw_vm_tones_param.tones[1].start_tone = mfw_vm_tones_param.tones[0].stop_tone + VM_TONE_INTERVAL;
	mfw_vm_tones_param.tones[1].stop_tone = mfw_vm_tones_param.tones[1].start_tone + VM_TONE_DURATION;
	mfw_vm_tones_param.tones[1].frequency_tone = 643;			// Tone 2 Frequecny in Hz
	mfw_vm_tones_param.tones[1].amplitude_tone = VM_TONE_AMPLITUDE;

	mfw_vm_tones_param.tones[2].start_tone = mfw_vm_tones_param.tones[1].stop_tone + VM_TONE_INTERVAL;
	mfw_vm_tones_param.tones[2].stop_tone = mfw_vm_tones_param.tones[2].start_tone + VM_TONE_DURATION;
	mfw_vm_tones_param.tones[2].frequency_tone = 775;			// Tone 3 Frequecny in Hz
	mfw_vm_tones_param.tones[2].amplitude_tone = VM_TONE_AMPLITUDE;

	mfw_vm_tones_param.frame_duration = mfw_vm_tones_param.tones[2].stop_tone * 2;
	mfw_vm_tones_param.sequence_duration = mfw_vm_tones_param.frame_duration * 2;
	mfw_vm_tones_param.period_duration = mfw_vm_tones_param.sequence_duration;
	mfw_vm_tones_param.repetition = TONE_INFINITE;
	// Call Riviera function to start recording.
	// If the Riviera call failed
	audio_riv_retVal = audio_vm_record_start(&mfw_vm_record_param,
										  &mfw_vm_tones_param,
						 				  voice_memo_return_path);
#ifdef FF_PCM_VM_VB
	  }
	}
#endif /* FF_PCM_VM_VB */
	if (audio_riv_retVal != RV_OK)
		return MFW_AUD_VM_RIVIERA_FAILED;
	
#endif

	return MFW_AUD_VM_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_AUD		             |
| STATE  : code 			ROUTINE: mfw_aud_vm_stop_record          |
+--------------------------------------------------------------------+


   PURPOSE :  Stop recording a Voice Memo

*/
SHORT mfw_aud_vm_stop_record(void (*callback_fn)(void *))
{
	T_AUDIO_RET						audio_riv_retVal=AUDIO_ERROR;

//x0pleela 06 Mar, 2006  ER:OMAPS00067709
#ifdef FF_PCM_VM_VB  
  UBYTE vm_type; //to store voice memo type
#endif
	TRACE_FUNCTION("mfw_aud_vm_stop_record");
	configure_callback_fn(callback_fn);

#ifndef WIN32	// only if not in Windows
//x0pleela 27 Feb, 2006   ER:OMAPS00067709
#ifdef FF_PCM_VM_VB
	//x0pleela 06 Mar, 2006  ER:OMAPS00067709
	//get the voice memo type
	vm_type = get_voice_memo_type();

	if( vm_type EQ PCM_VOICE_MEMO )
	  audio_riv_retVal = audio_vm_pcm_record_stop (voice_memo_return_path);
	//x0pleela 09 Mar, 2006   ER:OMAPS00067709
	else if( vm_type EQ VOICE_BUFFERING)
	  audio_riv_retVal = audio_voice_buffering_pcm_record_stop (voice_memo_return_path);
	else
	{
	   if (vm_type EQ AMR_VOICE_MEMO)
#endif
	audio_riv_retVal = audio_vm_record_stop(voice_memo_return_path);
#ifdef FF_PCM_VM_VB
	}
#endif
	// If the Riviera call failed
	if (audio_riv_retVal != RV_OK)
		return MFW_AUD_VM_RIVIERA_FAILED;
#endif

	return MFW_AUD_VM_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_AUD		             |
| STATE  : code 			ROUTINE: mfw_aud_vm_get_duration         |
+--------------------------------------------------------------------+


   PURPOSE :  Get the duration of the previously recorded Voice Memo

*/
UBYTE mfw_aud_vm_get_duration(void)
{
	TRACE_FUNCTION("mfw_aud_vm_get_duration");
//x0pleela 06 Mar, 2006  ER:OMAPS00067709
#ifdef FF_PCM_VM_VB  
  //x0pleela 27 Feb, 2006   ER:OMAPS00067709
  if( get_voice_memo_type() EQ PCM_VOICE_MEMO )
  //get PCM Voice memo recorded duration
  #ifdef FF_MMI_FILEMANAGER
  /* Mar 28, 2007  DR: OMAPS00122762 x0039928 */
    return FFS_flashData.pcm_voice_memo_position[FFS_flashData.PCM_voicememo_storage];
 #else
    return FFS_flashData.pcm_voice_memo_position;
 #endif
  else
  {
    if (get_voice_memo_type() EQ AMR_VOICE_MEMO)
#endif
 #ifdef FF_MMI_FILEMANAGER
 /* Mar 28, 2007  DR: OMAPS00122762 x0039928 */
    	return FFS_flashData.voice_memo_position[FFS_flashData.voicememo_storage];
 #else
	return FFS_flashData.voice_memo_position;
 #endif
#ifdef FF_PCM_VM_VB
	}
    return 0;
#endif 
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_AUD		             |
| STATE  : code 			ROUTINE: mfw_aud_vm_get_duration         |
+--------------------------------------------------------------------+


   PURPOSE :  Set the duration of the previously recorded Voice Memo

*/
void mfw_aud_vm_set_duration(UBYTE duration)
{
	TRACE_FUNCTION("mfw_aud_vm_set_duration");
//x0pleela 06 Mar, 2006  ER:OMAPS00067709
#ifdef FF_PCM_VM_VB  
 if( get_voice_memo_type() EQ PCM_VOICE_MEMO )
    //Set PCM Voice memo recorded duration
 #ifdef FF_MMI_FILEMANAGER
 /* Mar 28, 2007  DR: OMAPS00122762 x0039928 */
    FFS_flashData.pcm_voice_memo_position[FFS_flashData.PCM_voicememo_storage] = duration;
 #else
    FFS_flashData.pcm_voice_memo_position = duration;
 #endif
  else
  {
    if ( get_voice_memo_type() EQ AMR_VOICE_MEMO)
#endif
 #ifdef FF_MMI_FILEMANAGER
 /* Mar 28, 2007  DR: OMAPS00122762 x0039928 */
    	FFS_flashData.voice_memo_position[FFS_flashData.voicememo_storage] = duration;
 #else
	FFS_flashData.voice_memo_position = duration;
 #endif
#ifdef FF_PCM_VM_VB
  }
#endif

	flash_write();
	return;
}

 void rfs_data_write(const char* dir_name, const char* file_name, void* data_pointer, int data_size)
{
    T_RFS_FD fd;
    char file[FM_MAX_DIR_PATH_LENGTH];
    UINT16 file_uc[FM_MAX_DIR_PATH_LENGTH];
// Need to check if already flash is formatted

    sprintf(file, "%s/%s", dir_name, file_name);
     convert_u8_to_unicode(file, file_uc);
     fd = rfs_open(file_uc,
                  RFS_O_CREAT | RFS_O_WRONLY | RFS_O_TRUNC | RFS_O_APPEND, 0x777);
     rfs_write(fd, data_pointer, data_size);

     rfs_close(fd);
    return;
}
	
/*
** Local Function Definitions
*/

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_AUD		             |
| STATE  : code 			ROUTINE: mfw_aud_vm_create_file	         |
+--------------------------------------------------------------------+


   PURPOSE :  Create the FFS file required for the Riviera Voice Memo

*/
static void mfw_aud_vm_create_file(const char *folder, const char *fname, UBYTE index)
{

#ifdef FF_MMI_FILEMANAGER
	UINT16 dummy = 0x00;
       rfs_data_write(folder,fname, &dummy,sizeof(dummy));
#else
	UBYTE dummy = 0x00;
	flash_data_write(folder, fname, &dummy, sizeof(dummy));
#endif
	TRACE_FUNCTION("mfw_aud_vm_create_file");
	return;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_AUD		             |
| STATE  : code 			ROUTINE: configure_callback_fn	         |
+--------------------------------------------------------------------+


   PURPOSE :  Configure the Riviera Return PAth

*/
static void configure_callback_fn(void (*callback_fn)(void *))
{
	TRACE_FUNCTION("configure_callback_fn");
	voice_memo_return_path.addr_id = 0;
	voice_memo_return_path.callback_func = callback_fn;

	return;
}



/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_AUD		             |
| STATE  : code 			ROUTINE: configure_vm_filename	         |
+--------------------------------------------------------------------+


   PURPOSE :  Create the Voice memo filename from the incoming parameters

*/
#ifdef FF_MMI_FILEMANAGER
static void configure_vm_filename(UINT16 *vm_filename, const char *folder, const char *fname, UBYTE index)
#else
static void configure_vm_filename(char *vm_filename, const char *folder, const char *fname, UBYTE index)
#endif
{
#ifdef FF_MMI_FILEMANAGER
	char vm_filename_ascii[FM_MAX_DIR_PATH_LENGTH];
//	convert_unicode_to_u8(vm_filename, vm_filename_ascii);
//	memset(vm_filename_ascii, 0x00, AUDIO_PATH_NAME_MAX_SIZE);
	strcpy(vm_filename_ascii, folder);
	strcat(vm_filename_ascii, "/");
	strcat(vm_filename_ascii, fname);
	convert_u8_to_unicode(vm_filename_ascii, vm_filename);
#else
	TRACE_FUNCTION("configure_vm_filename");
	memset(vm_filename, 0x00, AUDIO_PATH_NAME_MAX_SIZE);
	strcpy(vm_filename, folder);
	strcat(vm_filename, "/");
	strcat(vm_filename, fname);
#endif	
	return;
}
#endif

// Mar 03, 2005 REF: CRR MMI-ENH-28950 xnkulkar
// Commented as we are no more using 'enable_tch_vocoder()' from MFW
/*
#ifndef  _SIMULATION_
void enable_tch_vocoder(BOOL enabled);
#endif
*/

/**********************************************************************
**
**          MFW Riviera Vocoder Interface functions
**
**********************************************************************/

void mfw_aud_l1_enable_vocoder ( void )
{
#ifndef  _SIMULATION_
// Mar 03, 2005 REF: CRR MMI-ENH-28950 xnkulkar
// Call 'hl_drv_enable_vocoder()' instead of 'enable_tch_vocoder(TRUE)'
//	enable_tch_vocoder(TRUE);
	
// July 03, 2006    REF:DR OMAPS00083150  x0047075
//Fix:Use hl_drv_set_vocoder_state(TRUE) function instead of hl_drv_enable_vocoder() to enable the vocoder
		hl_drv_set_vocoder_state(TRUE);
#endif
	return;
}

void mfw_aud_l1_disable_vocoder ( void )
{
#ifndef  _SIMULATION_
// Mar 03, 2005 REF: CRR MMI-ENH-28950 xnkulkar
// Call 'hl_drv_disable_vocoder()' instead of 'enable_tch_vocoder(FALSE)'
//	enable_tch_vocoder(FALSE);

//July 03, 2006    REF:DR OMAPS00083150  x0047075
//Fix:Use hl_drv_set_vocoder_state(FALSE) instead of hl_drv_disable_vocoder() to disable the vocoder
            hl_drv_set_vocoder_state(FALSE);
#endif
	return;
}

/****************************************************************/
/* NEPTUNE DEFINITIONS START HERE */
/* BEGIN ADD: Req ID: : Sumit : 14-Mar-05 */
#else /* NEPTUNE_BOARD */
#include "typedefs.h"
#include "mfw_aud.h"

SHORT mfw_aud_vm_delete_file(void)
{
	return MFW_AUD_VM_OK;
}

SHORT mfw_aud_vm_start_playback(void (*callback_fn)(void *))
{
	return MFW_AUD_VM_OK;
}

SHORT mfw_aud_vm_stop_playback(void (*callback_fn)(void *))
{
	return MFW_AUD_VM_OK;
}

SHORT mfw_aud_vm_start_record(UBYTE max_duration, void (*callback_fn)(void *))
{
	return MFW_AUD_VM_OK;
}

SHORT mfw_aud_vm_stop_record(void (*callback_fn)(void *))
{
	return MFW_AUD_VM_OK;
}

UBYTE mfw_aud_vm_get_duration(void)
{
	return 0;
}

void mfw_aud_vm_set_duration(UBYTE duration)
{
	return;
}

/*
** Layer1 Audio interface functions
*/
void mfw_aud_l1_enable_vocoder ( void )
{
}

void mfw_aud_l1_disable_vocoder ( void )
{
}

#endif /* NEPTUNE_BOARD*/
/* END ADD: Req ID: : Sumit : 14-Mar-05 */

