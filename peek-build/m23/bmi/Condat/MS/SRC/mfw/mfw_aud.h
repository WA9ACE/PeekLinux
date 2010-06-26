/*
+--------------------------------------------------------------------+
| PROJECT:	MMI-Framework (8417)		$Workfile::	mfw_aud.h	    $|
| $Author::	NDH							$Revision::	1			    $|
| CREATED:	04.02.03		     		$Modtime::	10.04.00 14:58	$|
| STATE  :	code														 |
+--------------------------------------------------------------------+

   MODULE  : MFW_AUD

   PURPOSE : This module contains the definitions for the Audio Riveria Interface.
   
    HISTORY:
	Mar 31, 2006   ER: OMAPS00067709  x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Added new field play_stopped to voice buffering data to keep a check before 
			calling play stop during end call
			
	Mar 15, 2006   ER: OMAPS00067709  x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Defined new macro for PTT number length
			Defined new Enumeration "T_VOICEMEMO" (which defines different types of voice memo) 
					and "T_VOICE_BUFFERING_STATUS" (Voice buffering status)
			Defined new structure "T_voice_buffering" to handle diferent scenarios during voice buffering
			Added prototypes of functions set_voice_memo_type, get_voice_memo_type, 
					set_voice_buffering_rec_stop_reason.

*/
#ifdef FF_PCM_VM_VB
#include "mfw_mfw.h"

//x0pleela 09 Mar, 2006   ER: OMAPS00067709
//PTT number length
#define PTT_NUM_LEN	45

//x0pleela 20 Mar, 2006  ER OMAPS00067709
//Maximum recording and playing duration for PCM voice memo and voice buffering
#define PCM_VOICE_MEMO_MAX_DURATION 20					// 20 Secs

//x0pleela 28 Feb, 2006  ER: OMAPS00067709
//Enums to define different types of voice memo
typedef enum
{
  NONE = 0,
  AMR_VOICE_MEMO,
  PCM_VOICE_MEMO,
  VOICE_BUFFERING
}T_VOICEMEMO;

//x0pleela 09 Mar, 2006   ER: OMAPS00067709
//Voice buffering status 
typedef enum
{
  VB_NONE,						/* Reset value							*/
  USER_SEL_STOP,				/* User has selected "STOP" option from menu 	*/
  CALLING_PARTY_END_CALL,		/* User has ended the PTT call 				*/
  CALLED_PARTY_END_CALL,		/* Called party has ended the call			*/
  RECORDING_TIMEOUT,			/* Timeout of recording						*/
  PLAY_ERROR,					/* Error while playing						*/
  CALLED_PARTY_NOT_AVAILABLE,	/* Called party not reachable				*/
  CALL_END_BEF_SETUP			/* Called party ended call before call setup		*/
}T_VOICE_BUFFERING_STATUS;

//Voice buffering structure
typedef struct
{
  T_MFW_HND win_hnd;						/* window handler to display dialogs			*/
  T_MFW_HND voice_buffering_tim;	   			/* voice buffering timer handle				*/
  UBYTE vb_PTTnum[PTT_NUM_LEN]; 			/* PTT number to be dialed					*/
  T_VOICE_BUFFERING_STATUS rec_stop_reason; /* Reason for calling record_stop function 		*/
  UBYTE call_active;							/* States whether call is active or not 			*/
  UBYTE recording_possible;					/* States whether recording is possible or not 	*/
  UBYTE buffering_phase;						/* States whether in buffering phase or not		*/
  UBYTE incoming_call_discon;					/* States the call disconnect of incoming call	*/
  UBYTE play_stopped;						/* States whether play has stopped or not		*/
}T_voice_buffering;

//x0pleela 06 Mar, 2006  ER:OMAPS00067709
//To set the type of voice memo
void set_voice_memo_type( T_VOICEMEMO voice_memo_type);
//To get the type of voice memo
EXTERN T_VOICEMEMO get_voice_memo_type( void);
//x0pleela 09 Mar, 2006  ER:OMAPS00067709
//To set reason for calling record_stop 
EXTERN void set_voice_buffering_rec_stop_reason( T_VOICE_BUFFERING_STATUS reason);

#endif
	
/*
** Voice Memo Functions Prototypes
*/
#ifndef FF_NO_VOICE_MEMO
SHORT mfw_aud_vm_delete_file(void);
SHORT mfw_aud_vm_start_playback(void (*callback_fn)(void *));
SHORT mfw_aud_vm_stop_playback(void (*callback_fn)(void *));
SHORT mfw_aud_vm_start_record(UBYTE max_duration, void (*callback_fn)(void *));
SHORT mfw_aud_vm_stop_record(void (*callback_fn)(void *));
UBYTE mfw_aud_vm_get_duration(void);
void mfw_aud_vm_set_duration(UBYTE duration);
#endif

/*
** Layer1 Audio interface functions
*/
void mfw_aud_l1_enable_vocoder ( void );
void mfw_aud_l1_disable_vocoder ( void );



/*
** Voice Memo Return Values
*/
#ifndef FF_NO_VOICE_MEMO
#define MFW_AUD_VM_OK					(0)
#define MFW_AUD_VM_RIVIERA_FAILED		(-1)
#define MFW_AUD_VM_MEM_FULL				(-2)
#define MFW_AUD_VM_MEM_EMPTY			(-3)
#endif

