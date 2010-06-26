/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   Sounds
 $File:       MmiSounds.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       25/10/00

********************************************************************************

 Description:

    This provides the implementation of the sounds for the basic MMI

    1. Playing various system sounds such as the paging ring,
       new sms message etc.
    2. The menus used to enable or disable the system sounds
    3. The menu used to select the paging ring tone
    4. The menu used to select the volume.
    5. The menu used to select the key press tone or dtmf

  The mmi should play system sounds by calling soundExec with the appropriate
  SoundsXXXX message it is possible to go directly to the audio driver to play
  a sound but this will bypass any user settings to turn sounds on or off.

  The MelodySelect and SettingXXXX messages are used to control the key events
  during menu handling, entry to the sounds menus is through the soundsXXXX
  functions. These display the appropriate menu and handle the keypad events.

  The settings are read from PCM on startup and saved whenever a setting menu
  is selected.

********************************************************************************
 $History: MmiSounds.c

    Apr 09, 2008 DRT: OMAPS00163752 including changes from OMAPS00151417 x0086292
	Description: Rapid Scrolling of Ringer melodies will lockup phone
	Solution: Board froze when sending to many messages to AS task to stop audio, which froze
		the board whenever there were too many messages in queue. Instead we can check if 
		audio is being played internally before sending stop message.

	Nov 06, 2007 DRT: OMAPS00151698 x0056422
    Description: BMI requirments needed to support Bluetooth AVRCP in Locosto 5.x
    Solution : In mfw_audResume, Pause, Stop functions, if BT is connected, send a callback. 
               In the case of audio play, handle the callback of START_IND from audio task and 
               post a callback to BT if expected. 

	August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat)
	Description:	COMBO PLUS:   Voice can not be heard in a call when MP3 tone was set
							   Have an MP3 tone set as Ringer. Make a call to the target.
							   Accept the same, no voice can be heard from the calling 
							   device or from the board that received 
	Issue: 		  Timing Synchronization Issue between BMI-ACI-L1.
				  BMI calls sAT_A to accept the call. ACI then sends the RING_OFF signal
				  to BMI, wherein BMI does a Ringer Stop. After this, ACI calls Vocoder
				  Enable. However, the Ringer Stop goes to L1 much later after Vocoder
				  Enable reaches L1 due to which the Vocoder eventually gets disabled.
	Solution: 	  	  The Vocoder Enable Implementation is removed from ACI and
				  shall be called by BMI on receipt of AS_STOP_IND
				  (AS_STOP_IND is the message sent by AS to BMI when BMI invokes
				   as_stop for ringer_stop). This way, it is assured that the Vocoder
				   Enable Request reaches L1 only after Ringer Stop is completely done

    April 25, 2007    DVT: OMAPS128828  x0061088(Prachi)
    Description: E1 Melody support
    Solution: Now E1 Melody will be supported through ringer. 

    Mar 30, 2007    REF: OMAPS00122691  x0039928
    Description: COMBO: No melody heard on parsing over different volums in tones menu
    Solution: Filename is passed with complete path to the audio service api.
    
 	Dec 05, 2006 ERT: OMAPS00101906  x0039928
 	Description: WAV format support on Locosto
 	Solution: .wav files are populated.
 	
 	Nov 23, 2006 ERT: OMAPS00101348  x0039928
 	Description: Open BAE support for SMS for Nokia Smart Messaging melody
 	Solution: .sms files are populated.

 	Nov 07, 2006 ER: OMAPS00102732 x0pleela
	Description: FFS Support for Intel Sibley Flash - Intel 256+64 non-ADMUX (PF38F4050M0Y0C0Q)
	Solution: Closing the opened directory if readdir is not successful and the new code 
   			is under the compilation flag FF_MMI_RELIANCE_FFS
   			
 	 25/10/00      Original Condat(UK) BMI version.

 	Sep 26, 2006 DR: OMAPS00096365 x0pleela
 	Description: Play All Files option does not play all the midi files second time
 	Solution:Reset the index to -1 to avoid playing only the last file in function sounds_midi_player_start_cb()
 	
 	Sep 11, 2006 DR: OMAPS00094182 xrashmic
 	Description: midi and mp3 files is not being populated on accessing corresponding menu, but only during bootup
 	Solution: Populate files on accessing the "browse files" list

 	Aug 08, 2006    REF:DRT OMAPS00085889 x0039928
   	Description:  no sound when played voice memo just after listening a midi file
	Solution: stereo path is unset after reaching end of the file.

 	Jun 06, 2006    REF:ER OMAPS00080844 xdeepadh
   	Description:  duplication of midi files in ffs area
	Solution: The code to create and load  the midi files from FFS to LFS is 
	removed.

	Nov 23, 2005 REF : DR OMAPS00057378   x0039928
	Bug: All MIDI files are not displayed in MMI in locosto-lite
	Fix: Buffer memory allocated to copy the file contents is freed
	properly and copying file from FFS to LFS is not terminated if error is
	returned for just any one file, instead it tries to copy other files.
 	 
 	Nov 21, 2005    REF:ER OMAPS00057430 nekkareb
 	Description:   AAC Ringer Implementation
	Solution: The Midi Ringer application has been enhanced to support AAC as ringer.
 	 
 	Nov 04, 2005    REF:DRT OMAPS00053737  xdeepadh
	Description:   MIDI Ringer Linear Flash Support.
	Solution: The Midi Ringer application has been enhanced to loadand play the midi files from LFS.

 	Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
	Description:   Mp3 Ringer Implementation
	Solution: The Midi Ringer application has been enhanced to support mp3 as ringer.
	The interface  function names have been modified.

 	Sep 12, 2005    REF: SPR 33977 xdeepadh
	Description:   Midi Application does not support Mobile XMF file formats
	Solution: Mobile XMF format has been supported

	 Aug  30, 2005    REF: SPR 34018 xdeepadh
	Description: Silent Mode: Silent mode is not activated with the sequence, 'Menu, '#''
	Solution: When the silent mode is set through the key sequence, the volume is set to silent.


	Jul 20, 2005    REF: SPR 30772 xdeepadh
	Description:  Midi File formats to be supported by different variants of locosto  program
	Solution: The supported midi file formats for a particular variant will be loaded.

 	 Aug 05, 2005     xdeepadh
	Description: Configuring the audio path for midi

	Apr 05, 2005    REF: ENH 29994 xdeepadh
	Description: Implementation of Generic Midi Ringer and Modification of existing Midi Test Application
	Solution: Generic Midi Ringer and Midi Test Application were implemented.

	Nov 29, 2004    REF: CRR 25051 xkundadu
	Description: INCALL SCREEN ?ADJUSTING THE VOLUME
	Fix: Added volume level list linked to up/down keys. 
	User can select the speaker volume among those levels.        	
  Dec 23, 2005    REF: SR13873 x0020906
	Description: Selection configurable ring tones. Added for NEPTUNE
	Fix: When moving cursor in menu, stop old and play new ringtone.
		   When press of back / clear / hang up stop playing the ringtone.  
	     Initially when the get into list, start playing first ringtone - As cursor is positioned.	 	

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
#include "mfw_sys.h"

#include "mfw_mfw.h"
#include "mfw_kbd.h"
#include "mfw_tim.h"
#include "mfw_lng.h"
#include "mfw_win.h"
#include "mfw_icn.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_mnu.h"
#include "mfw_phb.h"
#include "mfw_sms.h"

//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifdef FF_MIDI_RINGER  
#include "mfw_midi.h"

#ifdef FF_MMI_MIDI_FORMAT
#include "bae/bae_options.h" //Jul 20, 2005    REF: SPR 30772 xdeepadh
#endif

#endif

#include "dspl.h"


#include "p_mmi.h"
#include "message.h"
#include "prim.h"
#include "aci_cmh.h"
#include "mfw_mme.h"

#include "MmiMmi.h"

#include "Mmiicons.h"
#include "MmiPins.h"
#include "MmiMain.h"
#include "MmiDialogs.h"
#include "MmiIdle.h"
#include "MmiStart.h"
#include "mmiCall.h"
#include "MmiSimToolkit.h"
#include "gdi.h"
#include "audio.h"
#include "MmiLists.h"
#include "MmiSounds.h"
#include "MmiResources.h"
#include "MmiTimeDate.h"

#include "cus_aci.h"
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif

#include "MmiTimers.h"

#include "mmiColours.h"
//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifdef FF_MIDI_RINGER  
#include "mmisamplemidifiles.h"
#endif

/* OMAPS00151698, x0056422 */
#ifdef FF_MMI_A2DP_AVRCP
#include "mfw_bt_api.h"
#include "mfw_bt_private.h"
#endif
/* OMAPS00151698, x0056422 */

extern char* MelodyTable[];
extern T_call call_data;

// Aug 05, 2005     xdeepadh
#ifdef FF_MMI_AUDIO_PROFILE
extern UBYTE mfwAudPlay;//flag for audio 
#endif

/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- Start */

#include "mfw_aud.h"

/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- End  */


/* OMAPS00151698, x0056422 */
#ifdef FF_MMI_A2DP_AVRCP
extern BMI_BT_STRUCTTYPE tGlobalBmiBtStruct;
#endif //FF_MMI_A2DP_AVRCP
/* OMAPS00151698, x0056422 */

/*******************************************************************************

                                Constant Definitions

*******************************************************************************/


/* Control FLUSH mechanism to PCM
*/
#define FLUSH 1
#define NO_FLUSH 0


/* Timer constants, pretty self explanatory
*/
#define TIMHALFSECOND 500
#define TIMSECOND   1000
#define TIM4SECOND      4000

#define VIBRATOR_TIME 2000  /* two second timer on the vibrator*/

/* Define a generic trace mechanism
*/
#define PTRACE(prn) { char buf[64]; ## prn ##   TRACE_EVENT(buf); }


/* Define maximum counter for vibrator
*/
#define VIBRATOR_COUNTER_MAX  4
#define NOT_VIBRATING     0

#ifdef NEPTUNE_BOARD
/* RAVI - 20-1-2005 */
/* Volume Control Definition */
#define SILENT_SETTING    0
#define MAX_VOLUME_SETTING  4
/* END - RAVI - 20-1-2005 */
#endif

#define SILENT_VOLUME      0
#define LOW_VOLUME          5
#define MEDIUM_VOLUME     25
#define HIGH_VOLUME         175
#define INCREASE_VOLUME  5


/* define an invalid tone marker
*/
#define INVALID_TONE  -1

//Define to indicate the Size of the file where the melody must start
#ifdef FF_MP3_RINGER
#define MP3_PLAY_FROM_START 	0
#endif  //FF_MP3_RINGER

//Nov 22, 2005    REF: ENH OMAPS00057430 nekkareb
//Define to indicate the Size of the file where the melody must start
#ifdef FF_AAC_RINGER
#define AAC_PLAY_FROM_START 	0
#endif  //FF_AAC_RINGER


//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifdef FF_MIDI_RINGER   

extern UBYTE mfw_player_exit_flag;//Flag to check the playall option
extern int mfw_player_currently_playing_idx;//The current playing file index
#define MT_INFO_SCRN_TIMEOUT  1500//Timeout for InfoDialog
int selected_tone=0;

/* ADDED BY RAVI - 28-11-2005 */
int resource_GetListCount(res_ResourceID_type res);
/* END RAVI - 28-11-2005 */

static int playCurrentMelody( MfwEvt e, MfwTim *tc );
static void soundsPlayMidiPlayer( int melody_id );
void startPlayerTimer();
static void stopPlayerTimer( void );
static int playCurrentVolumeTone( MfwEvt e, MfwTim *tc );
void startVolumeTimer();
void stopVolumeTimer(void);
static void soundsPlayVolumeMelody();
#endif

/*Defintion for First tone being played - RAVI - 23-12-2005 */
#ifdef NEPTUNE_BOARD
#define FIRST_TONE_PLAYED	1
#endif
/*END RAVI */

/*******************************************************************************

                            Local Function prototypes

*******************************************************************************/

static int winEvent( MfwEvt e, MfwWin *w );
static int keyEvent( MfwEvt e, MfwKbd *kc );
static int keyEventLong( MfwEvt e, MfwKbd *kc );

/* x0039928 - Lint warning fix 
static void ( *show )( void ); */

//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifndef FF_MIDI_RINGER 
static int playCurrentSound( MfwEvt e, MfwTim *tc );
#endif
static int getCallingMenu( MfwEvt e, MfwTim *tc );
static int setSelection( MfwEvt e, MfwWin *w );
#ifndef FF_MIDI_RINGER
static void delayThenPlayMelody( void );
#endif
static void stopPlayingMelody( void );

static void readSettingsFromPCM( void );
static void writeSettingsToPCM( UBYTE flush );

static void soundsCreate( MfwHnd parent );
static int VibratorTimerEvent( MfwEvt e, MfwTim *t );


void sounds (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
void sounds_dialog_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
static void sounds_dialog(void);
static int sounds_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
static int melody_win_cb (MfwEvt e, MfwWin *w);
void melody_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
void sounds_destroy (T_MFW_HND own_window);
T_MFW_HND sounds_create (T_MFW_HND parent_window);
void sounds_Exit (T_MFW_HND own_window);
T_MFW_HND sounds_Init (T_MFW_HND parent_window);
void MelodyList_cb(T_MFW_HND * Parent, ListMenuData * ListData);
static void melody_destroy(MfwHnd own_window);
static MfwHnd sounds_show_info(T_MFW_HND parent, int str1, int str2, T_VOID_FUNC callback);

#define SHOW_CONFIRM          3
#define ACTIVATE_CONFIRM       4
#define DEACTIVATE_CONFIRM     5

/*******************************************************************************

                                Local typedefs

*******************************************************************************/

/* Define the types of volume we cater for
*/
typedef enum {
   silent = (UBYTE)0,
   volLow,
   volMedium,
   volHigh,
   increasing
} VOL_ENUM;


/* And the key tones
*/
typedef enum
{
  KEYTONE_NONE,
  KEYTONE_BEEP,
  KEYTONE_DTMF
} SoundsKeyTones;


/* The following definition provides a structure to control the
   ringing operation
*/

typedef struct MMI_CURRENT_RINGER_STRUCT {
                      /* variable to browse a list */
  UBYTE ringer;             // index of permanent active tune in soundlist and ringerItem
  UBYTE vibrator;             // vibrator
  UBYTE volumeSetting;          // values (0 = silent, 4= loud, 5 = increasing)
  UBYTE keypadOn;             // on/off values
  UBYTE AlarmOn;
  UBYTE OrganiserAlert;
  UBYTE CreditLow;
  UBYTE SMSTone;
  UBYTE VoiceTone;		/*CQ 10586 - API - 19/06/03 - Add this ID*/
  UBYTE SMSBroadcast;
  UBYTE battLow;
  UBYTE earpiece;
  UBYTE language;
  UBYTE Equalizer;
} RINGER_STRUCT_BROWSER;

/*  ------------- Variables ---------------   */

static SoundSettingState soundReason=SoundsNone;
// Change behavior of menu scroll up/down, meni item selected on scrolling.
static BOOL scrollSelectMenuItem= FALSE;
#ifndef FF_MIDI_RINGER /* x0039928 - Lint warning removal */
static BOOL volumeSetting2Pcm =FALSE;
#endif
static RINGER_STRUCT_BROWSER current =
{ // default settings.
  AUDIO_MEL_TUNES,  // ringer
  0,          // vibrator - off by default
  volMedium,      // volumeSetting
  2,          // keypadOn dtmf
  1,          // AlarmOn
  1,          // OrganiserAlert
  1,          // CreditLow
  1,          // SMSTone
  1,          // SMSBroadcast
  1,          // battLow
  3,          // earpiece
  1,          // language
  1         // Equalizer
};

typedef enum
{
    E_INIT = 0x0801,
    E_BACK,
    E_ABORT,
    E_EXIT,
  E_RETURN,
  E_OK
} e_M_events;

T_MELODY_INFO sounds_data;



/*******************************************************************************

                                Module local variables

*******************************************************************************/
/* RAVI - 20-1-2006 */
/* Store the volume settings */
#ifdef NEPTUNE_BOARD
static UBYTE lastVolumeSetting;
static UBYTE scrollVolumeSetting;
#endif
/* END RAVI */
static UBYTE oldVolumeSetting;     /* Previous setting of the volume      */

/* static MfwHnd kbd;                 // our keyboard handler          x0039928 - Lint warning fix */
/* static MfwHnd kbdLong;                 // our keyboard handler for Long pressed    x0039928 - Lint warning fix */
#ifndef FF_MIDI_RINGER
static MfwHnd hRingerStartTimer;         /* timeout till start of melody    */
#endif
//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifdef FF_MIDI_RINGER
 MfwHnd hPlayerStartTimer;         /* timeout till start of midi file    */
 #endif
static MfwHnd hRingerVolSettingTimeOut;   /* 4 second timeout for volume setting menu */


/* static MfwHnd hWinDispTimer;     // timer to show result windows     x0039928 - Lint warning fix */

/* Maintain vibrator state information here
*/

/* x0039928 - Lint warning removal 
static MfwHnd VibratorTimer;  
static UBYTE  Vibrating = NOT_VIBRATING; 
static UBYTE  vibrator_counter=0; */

/* General state information
*/

/* x0039928 - Lint warning removal
static MmiState nextState;
static MfwHnd lastFocus;
*/


/* Tune details
*/
static int CurrentTune = 0;
#ifndef FF_MIDI_RINGER  
static int LastTune = 0;
#endif

static const MfwMnuAttr melody_menuAttrib =
{
    &melody_menuArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu       */
    (U8) -1,                                 /* use default font         */
    NULL,                               /* with these items         */
    0,                                   /* number of items     */
	COLOUR_LIST_XX, TxtNull, NULL, MNUATTRSPARE

};


/*******************************************************************************

                                Private Methods

*******************************************************************************/

/******************************************************************************
*$Function:     setDefaultSound
*
*$Description:  This function will read from Flash during boot up and updated the
*	              current ringer structure. So after reset the stored tones will get 
*	              played.
*
*$Returns:    none
*
*$Arguments:  none
*$Author   :  RAVI - 23-12-2005
******************************************************************************/
#ifdef NEPTUNE_BOARD
void setDefaultSound(void)
{
	if (flash_read() >= EFFS_OK)
	{
	  /* Assign the current structure with FFS stored value */
		current.ringer       = FFS_flashData.ringer;
		current.SMSTone  = FFS_flashData.SMSTone;
		current.AlarmOn   = FFS_flashData.AlarmOn;
		current.volumeSetting = FFS_flashData.volumeSetting;
	}
	/* RAVI - 20-1-2006 */
	/* Store the initial volume settings done by user */
	lastVolumeSetting    = current.volumeSetting;
	scrollVolumeSetting = 0;
	/* END RAVI */
}
#endif

USHORT getSoundsReason(void)
{
  return soundReason;
}
/*******************************************************************************

 $Function:     setSoundsReason

 $Description:  Set the reason why the volume settings was invoked. Called to indicated
        that the volume settings is being invoked from Idle or from the incoming
        call screens.

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void setSoundsReason(USHORT reason)
{

  switch(reason)
  {
   case SettingVolume:
    soundReason = SettingVolume;
    break;
   case SoundsNone:
   default:
    soundReason = SoundsNone;
    break;
  }
}
/*******************************************************************************

 $Function:     volMenuItemSelected

 $Description:  A volume menu item has been selected, store the setting into PCM.

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void volMenuItemSelected(void)
{

	TRACE_FUNCTION("volMenuItemSelected");
	
/* Apr 05, 2005    REF: ENH 29994 xdeepadh */
#ifdef FF_MIDI_RINGER   
	/*  Save the volume for a given ringer mode */
	TRACE_EVENT_P1("Volume set is %d",current.volumeSetting);
	mfw_ringer_set_volume(get_ringer_mode(),(T_AS_VOLUME)getCurrentVoulmeSettings());
#else
	volumeSetting2Pcm =TRUE; /* x0039928 - Lint warning removal */
#endif
}
/*******************************************************************************

 $Function:     getScrollSelectMenuItem

 $Description:  get the scroll/select menu status.

 $Returns:    Status

 $Arguments:  none

*******************************************************************************/
BOOL getScrollSelectMenuItem(void)
{
TRACE_FUNCTION("getScrollSelectMenuItem");
  if (scrollSelectMenuItem == TRUE)
    return TRUE;
  else
    return FALSE;
}
/*******************************************************************************

 $Function:     clearScrollSelectMenuItem

 $Description:  Reset the scroll/select menu behavior.

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void clearScrollSelectMenuItem(void)
{
	TRACE_FUNCTION("clearScrollSelectMenuItem");
	scrollSelectMenuItem=FALSE;
	// soundReason = SoundsNone; MZ this flag cleared when user exits the melody/Volume menu.
//Apr 05, 2005    REF: ENH 29994 xdeepadh	
#ifdef FF_MIDI_RINGER   
	mfw_player_stop(sounds_midi_player_stop_cb);
#else
	stopPlayingMelody();
#endif
}

/*******************************************************************************

 $Function:     soundsSetVolume

 $Description:  Function determines if user is in the Melody/SMS/Alarm ringer menu
        and has selected the vloume settings option.

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void soundsSetVolume(void)
{

	TRACE_FUNCTION("soundsSetVolume");
//Apr 05, 2005    REF: ENH 29994 xdeepadh		
#ifdef FF_MIDI_RINGER   
	//Create 0.5 second ringer delay timer.
	hPlayerStartTimer = timCreate( 0, TIMHALFSECOND, (MfwCb)playCurrentVolumeTone);
	//Set the audiopath to speaker.
	mfw_ringer_set_audiopath();
	//Set the voice limit to 32 and channel to stereo in idle mode
	mfw_player_midi_set_params(FF_MIDI_VOICE_LIMIT,MIDI_CHANNEL_STEREO);
	//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
	//Set the Mp3 Parameters .Size of the file where the melody must start is set 0(from beginning)
	//and channel is set to stereo.
#ifdef FF_MP3_RINGER		
	mfw_player_mp3_set_params(MP3_PLAY_FROM_START,MP3_CHANNEL_STEREO);
#endif		//FF_MP3_RINGER

	//Nov 21, 2005    REF:ER OMAPS00057430  nekkareb
	//Set the AAC Parameters .Size of the file where the melody must start is set 0(from beginning)
	//and channel is set to stereo.
#ifdef FF_AAC_RINGER		
	mfw_player_aac_set_params(AAC_PLAY_FROM_START,AAC_CHANNEL_STEREO);
#endif //FF_AAC_RINGER

#endif

	if(soundReason == MelodySelect || soundReason == SettingSMSTone ||
	soundReason == SettingAlarm || soundReason == SettingVolume)
	{
		// Menu items will be selected on scrolling.
		scrollSelectMenuItem = TRUE;
		// store the setting to PCM if volume setting is selected.
#ifndef FF_MIDI_RINGER
		volumeSetting2Pcm=FALSE;
#endif
	}
	else
	{
		scrollSelectMenuItem = FALSE;
	}

}



/*******************************************************************************

 $Function:     IdleScreenVolumeTimer

 $Description:  Function creates the volume timer for idle screen.

 $Returns:    none

 $Arguments:  none

*******************************************************************************/

void IdleScreenVolumeTimer()
{
TRACE_FUNCTION("IdleScreenVolumeTimer");
//Apr 05, 2005    REF: ENH 29994 xdeepadh	
#ifdef FF_MIDI_RINGER   
	//Create 0.5 second ringer delay timer.
  	hPlayerStartTimer = timCreate( 0, TIMHALFSECOND, (MfwCb)playCurrentVolumeTone);
	//Set the audiopath to speaker.
	mfw_ringer_set_audiopath();
	//Set the voice limit to 32 and channel to stereo in idle mode
	mfw_player_midi_set_params(FF_MIDI_VOICE_LIMIT,MIDI_CHANNEL_MONO);
	//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
	//Set the Mp3 Parameters .Size of the file where the melody must start is set 0(from beginning)
	//and channel is set to Mono.
#ifdef FF_MP3_RINGER
	mfw_player_mp3_set_params(MP3_PLAY_FROM_START,MP3_CHANNEL_MONO);
#endif		//FF_MP3_RINGER

	//Nov 21, 2005    REF:ER OMAPS00057430 nekkareb
	//Set the AAC Parameters .Size of the file where the melody must start is set 0(from beginning)
	//and channel is set to Mono.
#ifdef FF_AAC_RINGER
	mfw_player_aac_set_params(AAC_PLAY_FROM_START,AAC_CHANNEL_MONO);
#endif //FF_AAC_RINGER

#endif
}

/*******************************************************************************

 $Function:     setSilentModeVolumeOn

 $Description:

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void setSilentModeVolume(void)
{

//	 Aug  30, 2005    REF: SPR 34018 xdeepadh
#ifdef FF_MIDI_RINGER  

	T_AS_RINGER_INFO * ringer_info;
	T_AS_RINGER_INFO  temp_ringer_info;
	TRACE_FUNCTION("setSilentModeVolume");

	memset(&temp_ringer_info,'\0',sizeof(T_AS_RINGER_INFO)); /* x0039928 - Lint warning fix */
	// Save the volume for incoming call
	ringer_info = mfw_ringer_get_info(AS_RINGER_MODE_IC,&temp_ringer_info);
	oldVolumeSetting  = ringer_info->volume ;
	TRACE_EVENT_P1("Volume set in silent mode is %d",oldVolumeSetting);
	//Set the volume to silent
	current.volumeSetting = (UBYTE)AS_VOLUME_SILENT;
	TRACE_EVENT_P1("Volume set is %d",current.volumeSetting);
	mfw_ringer_set_volume(AS_RINGER_MODE_IC,(T_AS_VOLUME)getCurrentVoulmeSettings());
#else
	 stopPlayingMelody();
	 oldVolumeSetting = current.volumeSetting; 
	 current.volumeSetting = SILENT_VOLUME; 
	audio_SetAmplf (AUDIO_BUZZER, current.volumeSetting);
#endif  
}

/*******************************************************************************

 $Function:     setSilentModeVolumeOn

 $Description:

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void restoreSilentModeVolume(void)
{

TRACE_FUNCTION("restoreSilentModeVolume");
//	 Aug  30, 2005    REF: SPR 34018 xdeepadh
#ifdef FF_MIDI_RINGER  
//Set the volume back to the old one
	TRACE_EVENT_P1("oldVolumeSetting set is %d",oldVolumeSetting);
	mfw_ringer_set_volume(AS_RINGER_MODE_IC,(T_AS_VOLUME)oldVolumeSetting);
#else
	stopPlayingMelody();
	current.volumeSetting =oldVolumeSetting; 
	audio_SetAmplf (AUDIO_BUZZER, current.volumeSetting);
#endif
}

/*******************************************************************************

 $Function:     soundsVolSilent

 $Description:  set voulme level to silent.

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void soundsVolSilent(void)
{
//Apr 05, 2005    REF: ENH 29994 xdeepadh	
#ifdef FF_MIDI_RINGER   
	//Display silent mode in the idle screen only for the incoming call volume.
	if(soundReason == MelodySelect || soundReason == SettingVolume)
	{
		FFS_flashData.settings_status |= SettingsSilentMode;
		flash_write();
	}
#else
  FFS_flashData.settings_status |= SettingsSilentMode;
  flash_write();
#endif

	TRACE_EVENT("soundsVolSilent");
//Apr 05, 2005    REF: ENH 29994 xdeepadh	
#ifdef FF_MIDI_RINGER   
	current.volumeSetting =(UBYTE)AS_VOLUME_SILENT;
	stopVolumeTimer();
	startVolumeTimer();
#else
/* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
  current.volumeSetting =SILENT_VOLUME;
#endif
/* END - RAVI */
  stopPlayingMelody();

  if(volumeSetting2Pcm != TRUE)
  {
    current.volumeSetting =SILENT_VOLUME;
    audio_SetAmplf (AUDIO_BUZZER, current.volumeSetting);

#ifdef FF_MMI_RINGTONE /* MSL Ring tone play */
    audio_PlaySoundID(AUDIO_BUZZER, CALLTONE_SELECT, 0, AUDIO_PLAY_ONCE);    
#else

/* RAVI - 20-1-2006 */	
#ifdef NEPTUNE_BOARD
     /* Store the currently stored volume content */
    audio_PlaySoundID(AUDIO_BUZZER, current.ringer, current.volumeSetting, AUDIO_PLAY_ONCE);    
#else
    /* API - 04/09/03 - SPR2447 - Remove the call to AUDIO_PLAY_INFINITE and replace with AUDIO_PLAY_ONCE*/
    audio_PlaySoundID(AUDIO_BUZZER, current.ringer, 0, AUDIO_PLAY_ONCE);    
    /* API - 04/09/03 - SPR2447 - END*/
#endif	
/* END RAVI */
#endif	

  }
  else
  {
    //store setting into PCM
    volumeSetting2Pcm=FALSE;
  }
#endif
	
	
}
/*******************************************************************************

 $Function:     soundsVolLow

 $Description:  set voulme level to low.

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void soundsVolLow(void)
{

  FFS_flashData.settings_status &= ~SettingsSilentMode;
  flash_write();
   TRACE_EVENT("soundsVolLow");
//Apr 05, 2005    REF: ENH 29994 xdeepadh		
#ifdef FF_MIDI_RINGER   
	current.volumeSetting =(UBYTE)AS_VOLUME_LOW;
		stopVolumeTimer();
		startVolumeTimer();
#else
/* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
  current.volumeSetting = LOW_VOLUME;
#endif
/*END RAVI */
  stopPlayingMelody();
  if(volumeSetting2Pcm != TRUE)
  {
    current.volumeSetting = LOW_VOLUME;
    audio_SetAmplf (AUDIO_BUZZER, current.volumeSetting);

#ifdef FF_MMI_RINGTONE /* MSL Ring tone play */
    audio_PlaySoundID(AUDIO_BUZZER, CALLTONE_SELECT, 0, AUDIO_PLAY_ONCE); 
#else

/* RAVI - 20-1-2006 */	
#ifdef NEPTUNE_BOARD
	audio_PlaySoundID(AUDIO_BUZZER, current.ringer, current.volumeSetting, AUDIO_PLAY_ONCE); 
#else
    /* API - 04/09/03 - SPR2447 - Remove the call to AUDIO_PLAY_INFINITE and replace with AUDIO_PLAY_ONCE*/
    audio_PlaySoundID(AUDIO_BUZZER, current.ringer, 0, AUDIO_PLAY_ONCE);    
    /* API - 04/09/03 - SPR2447 - END*/    
#endif	
/* END RAVI */
#endif	

  }
  else
  {
    //store setting into PCM
    volumeSetting2Pcm=FALSE;
  }
#endif
	
	
}
/*******************************************************************************

 $Function:     soundsVolMedium

 $Description:  set voulme level to Medium

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void soundsVolMedium(void)
{

  FFS_flashData.settings_status &= ~SettingsSilentMode;
  flash_write();
  TRACE_EVENT("soundsVolMedium");
//Apr 05, 2005    REF: ENH 29994 xdeepadh		
#ifdef FF_MIDI_RINGER   
	current.volumeSetting =(UBYTE)AS_VOLUME_MEDIUM;
		stopVolumeTimer();
		startVolumeTimer();
#else
/* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
  current.volumeSetting = MEDIUM_VOLUME;
#endif
/* END RAVI */
  stopPlayingMelody();
  if(volumeSetting2Pcm != TRUE)
  {
    current.volumeSetting = MEDIUM_VOLUME;
    audio_SetAmplf (AUDIO_BUZZER, current.volumeSetting);

#ifdef FF_MMI_RINGTONE /* MSL Ring Tone Play */
     audio_PlaySoundID(AUDIO_BUZZER, CALLTONE_SELECT, 0, AUDIO_PLAY_ONCE);   
#else

/* RAVI - 20-1-2006 */	
#ifdef NEPTUNE_BOARD
	audio_PlaySoundID(AUDIO_BUZZER, current.ringer, current.volumeSetting, AUDIO_PLAY_ONCE); 
#else
    /* API - 04/09/03 - SPR2447 - Remove the call to AUDIO_PLAY_INFINITE and replace with AUDIO_PLAY_ONCE*/
    audio_PlaySoundID(AUDIO_BUZZER, current.ringer, 0, AUDIO_PLAY_ONCE);   
    /* API - 04/09/03 - SPR2447 - END*/
#endif	
/* END RAVI */
#endif

  }
  else
  {
    //store setting into PCM
    volumeSetting2Pcm=FALSE;
  }
#endif
    
	

}
/*******************************************************************************

 $Function:     soundsVolHigh

 $Description:  set voulme level to High

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void soundsVolHigh(void)
{

  FFS_flashData.settings_status &= ~SettingsSilentMode;
  flash_write();
	
    TRACE_EVENT("soundsVolHigh");
//Apr 05, 2005    REF: ENH 29994 xdeepadh		
#ifdef FF_MIDI_RINGER   
		current.volumeSetting =(UBYTE)AS_VOLUME_HIGH;
		stopVolumeTimer();
		startVolumeTimer();

#else
/* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
  current.volumeSetting = HIGH_VOLUME;
#endif

  stopPlayingMelody();
  if(volumeSetting2Pcm != TRUE)
  {
    current.volumeSetting =HIGH_VOLUME;
    audio_SetAmplf (AUDIO_BUZZER, current.volumeSetting);

#ifdef FF_MMI_RINGTONE /* MSL Ring Tone Play */
       audio_PlaySoundID(AUDIO_BUZZER, CALLTONE_SELECT, 0, AUDIO_PLAY_ONCE);
#else

/* RAVI - 20-1-2006 */	
#ifdef NEPTUNE_BOARD
	audio_PlaySoundID(AUDIO_BUZZER, current.ringer, current.volumeSetting, AUDIO_PLAY_ONCE); 
#else	
    /* API - 04/09/03 - SPR2447 - Remove the call to AUDIO_PLAY_INFINITE and replace with AUDIO_PLAY_ONCE*/
	audio_PlaySoundID(AUDIO_BUZZER, current.ringer, 0, AUDIO_PLAY_ONCE);
    /* API - 04/09/03 - SPR2447 - END*/    
#endif	
/* END RAVI */
#endif

  }
  else
  {
    //store setting into PCM
    volumeSetting2Pcm=FALSE;
  }
#endif
	

}
/*******************************************************************************

 $Function:     soundsVolInc

 $Description:  set voulme level to increasing

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifndef FF_MIDI_RINGER  
void soundsVolInc(void)
{
  FFS_flashData.settings_status &= ~SettingsSilentMode;
  flash_write();
/* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
  current.volumeSetting = INCREASE_VOLUME;
#endif
/* END RAVI */
  stopPlayingMelody();
  if(volumeSetting2Pcm != TRUE)
  {
    current.volumeSetting = INCREASE_VOLUME;
    audio_SetAmplf (AUDIO_BUZZER, current.volumeSetting);

#ifdef FF_MMI_RINGTONE /* MSL Ring Tone Play */
    audio_PlaySoundID(AUDIO_BUZZER, CALLTONE_SELECT, 0, AUDIO_PLAY_ONCE);
#else

/* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
	audio_PlaySoundID(AUDIO_BUZZER, current.ringer, current.volumeSetting, AUDIO_PLAY_ONCE); 
#else	
    /* API - 04/09/03 - SPR2447 - Remove the call to AUDIO_PLAY_INFINITE and replace with AUDIO_PLAY_ONCE*/
    audio_PlaySoundID(AUDIO_BUZZER, current.ringer, 0, AUDIO_PLAY_ONCE);
    /* API - 04/09/03 - SPR2447 - END*/ 
#endif	
/* END RAVI */
#endif

  }
  else
  {
    //store setting into PCM
    volumeSetting2Pcm=FALSE;

  }


}

#endif
/*******************************************************************************

 $Function:     soundReasonRinger

 $Description:  setup the soundReason to be Ringer Melody settings.

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void soundReasonRinger(void)
{
  soundReason = MelodySelect;
}
/*******************************************************************************

 $Function:     soundReasonSmsTone

 $Description:  Setup the soundReason to be SMS tones settings.

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void soundReasonSmsTone(void)
{
  soundReason = SettingSMSTone;
}
/*******************************************************************************

 $Function:     soundReasonAlarm

 $Description:  Setup the soundReason to be Alarm settings.

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void soundReasonAlarm(void)
{
  soundReason = SettingAlarm;
}
/*******************************************************************************

 $Function:     getcurrentSMSTone

 $Description:  Access function for the current SMS Tone.
 $Returns:    Tone Id

 $Arguments:  None

*******************************************************************************/
UBYTE getcurrentSMSTone(void)
{
  return current.SMSTone;
}/*******************************************************************************

 $Function:     getcurrentVoiceTone

 $Description:  Access function for the current SMS Tone.
 $Returns:    Tone Id

 $Arguments:  None

*******************************************************************************/
/*CQ 10586 - API - 19/06/03 - Add this function to return the curret sound for Voice mail notification*/
UBYTE getcurrentVoiceTone(void)
{
  return current.SMSTone; // At present return the SMS tone untill settings have been updated
}
/*******************************************************************************

 $Function:     getcurrentSMSTone

 $Description:  Access function for the current SMS Tone.
 $Returns:    Tone Id

 $Arguments:  None

*******************************************************************************/
UBYTE getcurrentAlarmTone(void)
{
  return current.AlarmOn;
}
/*******************************************************************************

 $Function:     getCurrentRingerSettings

 $Description:  Access function for the current ringer settings data.
 $Returns:    Ringer Id.

 $Arguments:  None

*******************************************************************************/
UBYTE getCurrentRingerSettings(void)
{
  return current.ringer;
}
/*******************************************************************************

 $Function:     getCurrentVoulmeSettings

 $Description:  Access function for the current ringer volume settings data.
 $Returns:    volume

 $Arguments:  None

*******************************************************************************/
UBYTE getCurrentVoulmeSettings(void)
{
TRACE_EVENT_P1("getCurrentVoulmeSettings is %d",current.volumeSetting);
  return current.volumeSetting;
}
/*******************************************************************************

 $Function:     getMelodyListStatus

 $Description:  Status flag indicating if a melody selection menu is active.
 $Returns:    status

 $Arguments:  None

*******************************************************************************/
BOOL getMelodyListStatus(void)
{
  if(soundReason == MelodySelect ||
    soundReason == SettingSMSTone ||
    soundReason == SettingAlarm )
     return TRUE;
  else
    return FALSE;
}

#ifndef FF_MIDI_RINGER
/*******************************************************************************

 $Function:     stopPlayingMelody

 $Description:  stop currently played ringer melody

 $Returns:    None

 $Arguments:  None

*******************************************************************************/
static void stopPlayingMelody( void )
{
  TRACE_FUNCTION( "stopPlayingMelody" );

  /* If a timer event is running we want to stop that as well,
     since we don't really want to turn the tune off and have
     it start again when the timer expires
  */
  if ( hRingerStartTimer != NULL )
    timStop( hRingerStartTimer );

  /* Are we playing a tune
  */
  if ( LastTune )
  {
  /* To stop playing the previous ring tone - RAVI - 23-12-2005 */	
  #ifdef NEPTUNE_BOARD
  #ifdef FF_MMI_RINGTONE /*MSL Stop */
     audio_StopSoundbyID( AUDIO_BUZZER, CALLTONE_SELECT); 
  #else
    audio_StopSoundbyID( AUDIO_BUZZER, LastTune - 1 ); 
  #endif
  #else
    audio_StopSoundbyID( AUDIO_BUZZER, LastTune );
  #endif
  }

  /* Need to wait for the tune to stop before trying to
     set up the next tune, so wait always
  */
  vsi_t_sleep( 0, 5 );
}

#endif

/*******************************************************************************

 $Function:     startPlayingMelody

 $Description:  Start played the ringer melody

 $Returns:    None

 $Arguments:  Melody_id

*******************************************************************************/
//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifndef FF_MIDI_RINGER  
static void startPlayingMelody(UBYTE melody_id)
{
#ifdef NEPTUNE_BOARD
	BYTE bVolumeSetting;
#endif

  if ( hRingerStartTimer != NULL )
    timStop( hRingerStartTimer );

#ifdef FF_MMI_RINGTONE /* MSL Ring tone play */
  audio_PlaySoundID(AUDIO_BUZZER, CALLTONE_SELECT, (BYTE)current.volumeSetting, AUDIO_PLAY_ONCE);
#else

  /* RAVI - 20-1-2006 */
  #ifdef NEPTUNE_BOARD
  if (soundReason == SettingVolume)
  {
  	switch (scrollVolumeSetting)
	{
	case 0:
		bVolumeSetting = SILENT_VOLUME;	
		break;
	case 1:
		bVolumeSetting = LOW_VOLUME;
		break;
	case 2:
		bVolumeSetting = MEDIUM_VOLUME;
		break;
	case 3:
		bVolumeSetting = HIGH_VOLUME;
		break;
	case 4:
		bVolumeSetting = INCREASE_VOLUME;
		break;
	default:
		bVolumeSetting = LOW_VOLUME;
		break;
  	}
  	audio_PlaySoundID(AUDIO_BUZZER, melody_id, (BYTE)bVolumeSetting, 
                                        AUDIO_PLAY_ONCE);
  }
  else 
  {
  	audio_PlaySoundID(AUDIO_BUZZER, melody_id, (BYTE)current.volumeSetting, 
                                        AUDIO_PLAY_ONCE);
  }
  #else
       audio_PlaySoundID(AUDIO_BUZZER, melody_id, (BYTE)current.volumeSetting,
                                        AUDIO_PLAY_ONCE);
  #endif
  /* END - RAVI - 20-1-2006 */	
#endif

}
#endif

/*******************************************************************************

 $Function:     soundsPlayRinger

 $Description:  Play the tune selected by melody_id, stopping any currently
        playing tunes if necessary. Remember to take into account the
        volume setting and provide a crescendo if requested

 $Returns:    None

 $Arguments:  melody_id, selects the melody to be played

*******************************************************************************/
//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifndef FF_MIDI_RINGER  
static void soundsPlayRinger( int melody_id )
{
  CurrentTune = melody_id;
  if (LastTune)
  {
  #ifdef FF_MMI_RINGTONE /*MSL Stop */
     audio_StopSoundbyID( AUDIO_BUZZER, CALLTONE_SELECT); 
  #else  
    audio_StopSoundbyID (AUDIO_BUZZER,LastTune );
  #endif
  }	
  vsi_t_sleep(0, 5);

  if (current.volumeSetting == increasing)
  {

#ifdef FF_MMI_RINGTONE  /* MSL Ring Tone Play */
    audio_PlaySoundID ( AUDIO_BUZZER, CALLTONE_SELECT, 0 , AUDIO_PLAY_CRESCENDO );
#else

  /* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
	audio_PlaySoundID(AUDIO_BUZZER, CurrentTune, current.volumeSetting, AUDIO_PLAY_ONCE); 
#else  
    audio_PlaySoundID ( AUDIO_BUZZER,CurrentTune, 0 , AUDIO_PLAY_CRESCENDO );
#endif
/* END RAVI */
#endif

  }
  else
  {

#ifdef FF_MMI_RINGTONE  /* MSL Ring Tone Play */
    audio_PlaySoundID ( AUDIO_BUZZER, CALLTONE_SELECT, 0, AUDIO_PLAY_ONCE );
#else

  /* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
	audio_PlaySoundID(AUDIO_BUZZER, CurrentTune, current.volumeSetting, AUDIO_PLAY_ONCE); 
#else  
    audio_PlaySoundID ( AUDIO_BUZZER,CurrentTune, 0, AUDIO_PLAY_ONCE );
#endif
/* END RAVI */
#endif

  }
  LastTune = CurrentTune;
}
#endif

#if(0)  /* x0039928 - Lint warning removal */
/*******************************************************************************

 $Function:     destroyDynaMenu

 $Description:  frees the memory allocated to a dynamenu (Refer to the
        buildMelodyMenu routine below

 $Returns:    None.

 $Arguments:  None.

*******************************************************************************/

static void destroyDynaMenu( void )
{

}

#endif
/*******************************************************************************

 $Function:     buildMelodyMenu

 $Description:  Builds the melody menu dynamenu from the melodies supplied
        by the resource manager

 $Returns:    None

 $Arguments:  None

*******************************************************************************/
static T_MFW_HND  buildMelodyMenu( MfwHnd parent_window)
{
  T_MELODY_INFO *  data = (T_MELODY_INFO *)ALLOC_MEMORY (sizeof (T_MELODY_INFO));
  T_MFW_WIN  * win;

    TRACE_FUNCTION (">>>> buildMelodyMenu()");

    /*
     * Create window handler
     */

    data->sounds_win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)melody_win_cb);
    if (data->sounds_win EQ NULL)
  {
     return NULL;
  }

  TRACE_EVENT(">>>> Melody window created: " );
    /*
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)melody_exec_cb;
    data->mmi_control.data   = data;
    win                      = ((T_MFW_HDR *)data->sounds_win)->data;
    win->user                = (void *)data;
    data->parent             = parent_window;
  winShow(data->sounds_win);
    /*
     * return window handle
     */

     return data->sounds_win;
}

/*******************************************************************************

 $Function:     melody_exec_cb

 $Description:  Exec callback function of the SMS read window (dynamic list of
        all the SMS)

 $Returns:    none

 $Arguments:  win - window handler
        event - mfw event
        parameter - optional data.

*******************************************************************************/
void melody_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_MELODY_INFO   * data = (T_MELODY_INFO *)win_data->user;
     UBYTE i;
     UBYTE numberOfMelodies=0;
#ifdef FF_MIDI_RINGER  
	UBYTE file_count=0;
#endif
     ListWinInfo      * mnu_data = (ListWinInfo *)parameter;

    TRACE_FUNCTION (">>>>> melody_exec_cb()");

    switch (event)
  {
  case E_INIT:

    TRACE_EVENT(">>>> melody_exec_cb() Event:E_INIT");

     /* initialization of administrative data */

    data->menu_list_data = (ListMenuData *)ALLOC_MEMORY(sizeof(ListMenuData));

    if(data->menu_list_data == 0)
    {
      TRACE_EVENT("Failed memory alloc 1 ");
      return;
    }
/*Apr 05, 2005    REF: ENH 29994 xdeepadh */
#ifdef FF_MIDI_RINGER  
	numberOfMelodies = sounds_midi_return_file_number();
	TRACE_EVENT_P1("numberOfMelodies is %d",numberOfMelodies);
			/*In case no files have been loaded display info message */
	if(numberOfMelodies <= 0)
	{
		sounds_show_info(0, TxtMidiFileNotLoaded, TxtNull, NULL);
		return;

	}
#else
    numberOfMelodies = resource_GetListCount(RES_MELODY);
#endif

    data->menu_list_data->List = (T_MFW_MNU_ITEM *)ALLOC_MEMORY( numberOfMelodies * sizeof(T_MFW_MNU_ITEM) );

    if(data->menu_list_data->List == 0)
    {
      TRACE_EVENT("Failed memory alloc 2");
      return;
    }

    for (i = 0; i < numberOfMelodies ; i++)
    {
		mnuInitDataItem(&data->menu_list_data->List[i]);
		data->menu_list_data->List[i].flagFunc = item_flag_none;

		/* Display the midi file names in the list */
		/*Apr 05, 2005    REF: ENH 29994 xdeepadh	 */		
#ifdef FF_MIDI_RINGER  
	data->menu_list_data->List[i].str  = (char *)sounds_midi_return_file_name(file_count++);
#else
      	data->menu_list_data->List[i].str  = (char *)MelodyTable[i];
#endif
    }

      data->menu_list_data->ListLength =numberOfMelodies;
    data->menu_list_data->ListPosition = 1;
    data->menu_list_data->CursorPosition = 1;
    data->menu_list_data->SnapshotSize = numberOfMelodies;
    data->menu_list_data->Font = 0;
    data->menu_list_data->LeftSoftKey = TxtSoftSelect;
    data->menu_list_data->RightSoftKey = TxtSoftBack;
    data->menu_list_data->KeyEvents = KEY_ALL;
    data->menu_list_data->Reason = 0;
    data->menu_list_data->Strings = TRUE;
    data->menu_list_data->Attr   = (MfwMnuAttr*)&melody_menuAttrib;
/* Apr 05, 2005    REF: ENH 29994 xdeepadh	*/
#ifdef FF_MIDI_RINGER  
 	        data->menu_list_data->Attr->hdrId = TxtMidiFileTitle;
#endif
    data->menu_list_data->autoDestroy    = FALSE;
  	    listDisplayListMenu(win, data->menu_list_data,(ListCbFunc)MelodyList_cb,0);

#ifdef FF_MIDI_RINGER  
	selected_tone=data->menu_list_data->ListPosition-1;
	stopPlayerTimer();
	startPlayerTimer();
#endif

/* Start playing first ringtone on initial list display - RAVI - 23-12-2005 */
#ifdef NEPTUNE_BOARD   
	    if(getMelodyListStatus()== TRUE)
	    {
			stopPlayingMelody ();
			startPlayingMelody(0); /* Play the first menu item which is being selected */
			LastTune = FIRST_TONE_PLAYED;          /* First Menu Item - Ringer is selected */
	    }
#endif		
/* RAVI - 23-12-2005	*/	

break;

  case E_START_MELODY:
/* Apr 05, 2005    REF: ENH 29994 xdeepadh */
/* Play the midi file when the user has scrolled  */
#ifdef FF_MIDI_RINGER  
		selected_tone=mnu_data->MenuData.ListPosition;
		stopPlayerTimer();
		startPlayerTimer();
#else

/* When cursor moved - Stop playing old and start playing new - RAVI - 23-12-2005 */
#ifndef NEPTUNE_BOARD
      startPlayingMelody(mnu_data->MenuData.ListPosition);  
#else
      stopPlayingMelody();  
      startPlayingMelody(mnu_data->MenuData.ListPosition);  
      LastTune = mnu_data->MenuData.ListPosition+1; /* Store the late tune in global variable */
#endif     /* END RAVI */ 
#endif

    break;

  case E_RETURN:
    break;
  default:
      break;
  }
}
/*******************************************************************************

 $Function:     MelodyList_cb

 $Description:  Callback function for the melody list.

 $Returns:    none

 $Arguments:  Parent - parent window.
        ListData - Menu item list
*******************************************************************************/

void MelodyList_cb(T_MFW_HND * Parent, ListMenuData * ListData)
{
    T_MFW_WIN       * win_data = ((T_MFW_HDR *)Parent)->data;
    T_MELODY_INFO * data     = (T_MELODY_INFO *)win_data->user;
/*Apr 05, 2005    REF: ENH 29994 xdeepadh */
#ifdef FF_MIDI_RINGER  
	T_AS_PLAYER_TYPE player_type;

#ifdef FF_MIDI_LOAD_FROM_MEM
	T_MELODY memory_pointer;
#else
	char * filename;
#endif
#endif

  if ((ListData->Reason EQ LISTS_REASON_BACK) || (ListData->Reason EQ LISTS_REASON_CLEAR)
  	|| (ListData->Reason EQ LISTS_REASON_HANGUP))	/* sbh - added hangup key */
  {

/*Stop playing ringtone on press of BACK/ CLEAR/ HANGUP - RAVI - 23-12-2005 - SR 13873 */
#ifdef NEPTUNE_BOARD
      stopPlayingMelody();  
#endif      
/* END RAVI - 23-12-2005 */

  }
  else if(ListData->Reason EQ LISTS_REASON_SELECT)
  {
    switch(soundReason)
      {
      case MelodySelect:
	  	TRACE_EVENT("selecting file");
#ifdef FF_MIDI_RINGER  /*Use Midi Player*/
#ifdef FF_MIDI_LOAD_FROM_MEM/*If Loc ULC */
	memory_pointer=sounds_midi_return_memory_location(ListData->ListPosition);
	player_type=mfw_ringer_deduce_player_type(memory_pointer.melody_name);
	mfw_ringer_set_memory(player_type,AS_RINGER_MODE_IC,(UINT32*)memory_pointer.melody,(UINT32)memory_pointer.melody_size);
#else
	filename =sounds_midi_return_file_name(ListData->ListPosition);
	player_type=mfw_ringer_deduce_player_type(filename);
	mfw_ringer_set_file(player_type,AS_RINGER_MODE_IC,filename);
#endif

#else		//Use Buzzer
        stopPlayingMelody();
        current.ringer = ListData->ListPosition;
#endif

        break;

      case SettingSMSTone:
	  	
#ifdef FF_MIDI_RINGER  //Use Midi Player
#ifdef FF_MIDI_LOAD_FROM_MEM //If ULC
	memory_pointer=sounds_midi_return_memory_location(ListData->ListPosition);
	player_type=mfw_ringer_deduce_player_type(memory_pointer.melody_name);
	mfw_ringer_set_memory(player_type,AS_RINGER_MODE_SMS,(UINT32*)memory_pointer.melody,(UINT32)memory_pointer.melody_size);
#else
	filename =sounds_midi_return_file_name(ListData->ListPosition);
	player_type=mfw_ringer_deduce_player_type(filename);
	mfw_ringer_set_file(player_type,AS_RINGER_MODE_SMS,filename);
#endif
#else/*Use Buzzer*/
        stopPlayingMelody();
        current.SMSTone = ListData->ListPosition;  /* For SMS tones - starting from 40 */
/* Store in FFS - RAVI - 23-12-2005 - SR 13873 */			
#ifdef NEPTUNE_BOARD
	FFS_flashData.SMSTone= current.SMSTone;
	flash_write(); 
#endif
/* END RAVI - 23-12-2005 */
#endif

        break;

      case SettingAlarm:
#ifdef FF_MIDI_RINGER  /* Use Midi Player */
	#ifdef FF_MIDI_LOAD_FROM_MEM
	memory_pointer=sounds_midi_return_memory_location(ListData->ListPosition);
	player_type=mfw_ringer_deduce_player_type(memory_pointer.melody_name);
	mfw_ringer_set_memory(player_type,AS_RINGER_MODE_ALARM,(UINT32*)memory_pointer.melody,(UINT32)memory_pointer.melody_size);
#else
	filename =sounds_midi_return_file_name(ListData->ListPosition);
	player_type=mfw_ringer_deduce_player_type(filename);
	mfw_ringer_set_file(player_type,AS_RINGER_MODE_ALARM,filename);
#endif
#else/*Use Buzzer*/
        // need to check alarm status MZ.
        current.AlarmOn = ListData->ListPosition;
/* Store in FFS - RAVI - 23-12-2005 - SR 13873 */	
#ifdef NEPTUNE_BOARD
	FFS_flashData.AlarmOn= current.AlarmOn;
	flash_write(); 
#endif
/* END RAVI - 23-12-2005 */
#endif

      default:
        break;
      }
/* Apr 05, 2005    REF: ENH 29994 xdeepadh */
/* Store selected melody in PCM storage. */
#ifdef FF_MIDI_RINGER  
	sounds_show_info(0, TxtSelected, TxtNull, NULL);
#endif
  }
  
#ifdef FF_MIDI_RINGER  
 	stopPlayerTimer();	
	// DRT:OMAPS00163752 including changes from OMAPS00151417 (x0086292)
	//mfw_player_stop(sounds_midi_player_stop_cb);
#else
  stopPlayingMelody();
#endif
  listsDestroy(ListData->win);
  melody_destroy(data->sounds_win);

}

/*******************************************************************************

 $Function:     melody_destroy

 $Description:  Destroy the melody window.

 $Returns:    none

 $Arguments:  own_window- current window
*******************************************************************************/

static void melody_destroy(MfwHnd own_window)
{
    T_MFW_WIN * win_data;
    T_MELODY_INFO     * data;
  UBYTE numberOfMelodies;

    TRACE_FUNCTION ("melody_destroy()");

    if (own_window)
  {
      win_data = ((T_MFW_HDR *)own_window)->data;
      data = (T_MELODY_INFO *)win_data->user;

      if (data)
    {
        /*
         * Delete WIN handler
         */
        win_delete (data->sounds_win);

        /*
         * Free Memory
         */
//Apr 05, 2005    REF: ENH 29994 xdeepadh         
#ifdef FF_MIDI_RINGER  
 	numberOfMelodies = sounds_midi_return_file_number();
#else
        numberOfMelodies = resource_GetListCount(RES_MELODY);
#endif

      if (data->menu_list_data != NULL)
      {
        FREE_MEMORY ((void *)data->menu_list_data->List, numberOfMelodies * sizeof (T_MFW_MNU_ITEM));
          FREE_MEMORY ((void *)data->menu_list_data, sizeof (ListMenuData));
      }
      FREE_MEMORY ((void *)data, sizeof (T_MELODY_INFO));
//Apr 05, 2005    REF: ENH 29994 xdeepadh	  
#ifdef FF_MIDI_RINGER  
//Delete the timer used for scrolling
	if(hPlayerStartTimer!=NULL)
		timDelete(hPlayerStartTimer);
#endif

    }
      else
    {
        TRACE_EVENT ("melody_destroy() called twice");
    }
  }
}

/*******************************************************************************

 $Function:     melody_win_cb

 $Description:  Window callback function for the melody window.

 $Returns:    none

 $Arguments:  w - mfw window handler
        e - mfw event

*******************************************************************************/

static int melody_win_cb (MfwEvt e, MfwWin *w)    /* yyy window event handler */
{
  TRACE_FUNCTION ("melody_win_cb()");
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

#if(0)  /* x0039928 - Lint warning removal */
/*******************************************************************************

 $Function:     melody_menu_cb

 $Description:  Call back function for the melody menu.

 $Returns:

 $Arguments:
*******************************************************************************/

static int melody_menu_cb (MfwEvt e, MfwMnu *m){

/*  T_MFW_HND       win  = mfwParent(mfw_header());       */       /* RAVI */
/*    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data; */ /* RAVI */

  switch (e)
        {
            case E_MNU_ESCAPE:
            break;
            default:
                return 0;
        }
  return 1;
}
#endif
/*******************************************************************************

 $Function:     resource_GetListCount

 $Description:  Returns the number of entries in resource table.

 $Returns:    Number of entries in Melody table.

 $Arguments:  res - Resource Id.

*******************************************************************************/
int resource_GetListCount(res_ResourceID_type res)
  // Returns the number of the resource type available in the phone.
{
  int index=0;
  switch (res)
  {
    case RES_MELODY:
      while(MelodyTable[index] !=0)
        index++;
      return index;
     /* break; */ /*   RAVI */

    case RES_LANGUAGE:

      break;
    case RES_STRING:

      break;
    default:

      break;

  }
  return index;
}

#if(0) /* x0039928 - Lint warning removal */
/*******************************************************************************

 $Function:     buildOnOffMenu

 $Description:  builds an on/off menu and associates it with a context

 $Returns:    None

 $Arguments:  count, number of elements in menu
        id3, prompt string
        active, context into which the menu should be associated

*******************************************************************************/

static void buildOnOffMenu( int count, int id3, int active )
{
}

/*******************************************************************************

 $Function:     buildVolumeMenu

 $Description:  builds the volume menu and associates it with the volume
        context

 $Returns:    None

 $Arguments:  volume, the context to be associated with

*******************************************************************************/

static void buildVolumeMenu( VOL_ENUM volume )
{
}

/*******************************************************************************

 $Function:     displayDynaMenu

 $Description:  show one page of the ringer list, determined by item.

 $Returns:    None

 $Arguments:  None

*******************************************************************************/

static void displayDynamenu( void )
{

}

/*******************************************************************************

 $Function:     displayRingerVolume

 $Description:  shows the volume setting determined by cVolume

 $Returns:    None

 $Arguments:  None

*******************************************************************************/

static void displayRingerVolume( void )
{
}

/*******************************************************************************

 $Function:     displayOnOffList

 $Description:  show the On Off (3rd) list, determined by item.

 $Returns:    None

 $Arguments:  None

*******************************************************************************/

static void displayOnOffList( void )
{
}

/*******************************************************************************

 $Function:     buildLanguageMenu

 $Description:  builds the dynamenu for the RES_LANGUAGE resource type

 $Returns:    None

 $Arguments:  None

*******************************************************************************/

static void buildLanguageMenu(void)
{

}

#endif

#ifndef FF_MIDI_RINGER /* x0039928 - Lint warning removal */
/*******************************************************************************

 $Function:     delayThenPlayMelody

 $Description:  play selected ringer melody after short delay

 $Returns:    None

 $Arguments:  None

*******************************************************************************/

static void delayThenPlayMelody( void )
{

  /* start timer for half a second and on timeout start everlasting play
  */
  timStart( hRingerStartTimer );
}
#endif

/*******************************************************************************

 $Function:     playCurrentSound

 $Description:  play tune which is temporarily selected, this event handler
        is invoked when the hRingerStartTimer event completes

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  e, event, tc timer context

*******************************************************************************/
//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifndef FF_MIDI_RINGER  
static int playCurrentSound( MfwEvt e, MfwTim *tc )
{
  soundsPlayRinger(current.ringer);

  // stop running timer (One shot mode only)
  if (hRingerStartTimer != NULL ) {
    timStop(hRingerStartTimer);
  }

    return MFW_EVENT_CONSUMED;
}

#endif
#ifdef MMI_LINT_WARNING_REMOVAL /* x0039928 - Lint warning removal */
/*******************************************************************************

 $Function:     setSelection

 $Description:  reactivates calling menu after confirming setting

 $Returns:    MFW_EVENT_PASSED always

 $Arguments:  e, event, m, window handle

*******************************************************************************/

static int setSelection( MfwEvt e, MfwWin *m )
{
  //int UpdatePCM = 0;   // RAVI
//  int SoundToPlay = -1;  // RAVI

  TRACE_FUNCTION("setSelection");

  /* This implements a state machine, the next setting depends
     on the reason we have been invoked

     Note the following is still pending implementation
     USSD TONE, SVC TONE, MANPLMN TONE
  */
  switch (soundReason)
  {
    case MelodySelect:
    {
    }
    break;

    case SettingVolume:
    {
    }
    break;

    default:
    {
      /* No action required
      */
    }
    break;
  }


  /* redisplay sub menu showing changed position of active marker.
  */
  winShow( win );

  return MFW_EVENT_PASSED;
}


/*******************************************************************************

 $Function:     writeSettingsToPCM

 $Description:  writes contents of global vars of sounds to PCM or
          sets default values

 $Returns:    None

 $Arguments:  flush, dictates whether or not flushing should be
        performed on the write operation

*******************************************************************************/
static void writeSettingsToPCM( U8 flush )
{
}
/*******************************************************************************

 $Function:     readSettingsFromPCM

 $Description:  inits global vars of sounds from PCM or
          sets default values

 $Returns:    None

 $Arguments:  None

*******************************************************************************/

static void readSettingsFromPCM(void)
{
}

/*******************************************************************************

 $Function:     soundsCreate

 $Description:  Create a sounds context, with a window, keyboards, timers
        etc.

 $Returns:    None

 $Arguments:  parent, handle of the parent window

*******************************************************************************/

static void soundsCreate(MfwHnd parent)
{

}


/*******************************************************************************

 $Function:     soundsDestroy

 $Description:  cleans up a context

 $Returns:    None

 $Arguments:  None

*******************************************************************************/

static void soundsDestroy(void)
{

}


/*******************************************************************************

 $Function:     getCallingMenu

 $Description:  handles transition to calling menu
          a) on timeout,
          b) during running timer when Clear, SoftKeyLeft
             or SoftKeyRight is pressed

 $Returns:    MFW_EVENT_PASSED

 $Arguments:  e, event, tc, timer context

*******************************************************************************/

static int getCallingMenu(MfwEvt e, MfwTim *tc)
{
  TRACE_FUNCTION( "BACK to invocating Menu" );

  return 0;
}


/*******************************************************************************

 $Function:     winEvent

 $Description:  window event handler, only deals with the win visible
        event, all others are ignored

 $Returns:    MFW_EVENT_PASSED or MFW_EVENT_CONSUMED depending on the
        event

 $Arguments:  e, event, w, window handle

*******************************************************************************/

static int winEvent (MfwEvt e, MfwWin *w)
{
    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     keyEventLong

 $Description:  keyboard event handler Long press

 $Returns:    MFW_EVENT_PASSED or MFW_EVENT_CONSUMED depending on the
        event

 $Arguments:  e, event, k, keyboard handle

*******************************************************************************/

static int keyEventLong (MfwEvt e, MfwKbd *k)
{
    PTRACE( sprintf( buf, "keycode in SOUND keyEvtLong %d", (int) k->code ); )
  PTRACE( sprintf( buf, "mfwEvt in SOUND Long 0x%10x", e ); )


  return MFW_EVENT_PASSED; /* give other handlers a chance */
}

/*******************************************************************************

 $Function:     keyEvent

 $Description:  keyboard event handler

 $Returns:    MFW_EVENT_PASSED or MFW_EVENT_CONSUMED depending on the
        event

 $Arguments:  e, event, k, keyboard handle

*******************************************************************************/

static int keyEvent (MfwEvt e, MfwKbd *k)
{

    PTRACE( sprintf( buf, "keycode in SOUND keyEvt %d", (int) k->code ); )
  PTRACE( sprintf( buf, "mfwEvt in SOUND 0x%10x", e ); )

  return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     stepVibrator

 $Description:  Changes the state of the vibrator from vibrating to
                not vibrating and then restarts the vibration timer

 $Returns:    None

 $Arguments:  None

*******************************************************************************/

static void stepVibrator( void )
{
  TRACE_FUNCTION("Vibrator Ringing");

}


/*******************************************************************************

 $Function:     ringVibrator

 $Description:  generates the ringing vibrations as configured

 $Returns:    New setting of the vibrator counter

 $Arguments:  c, pointer to a vibrator counter

*******************************************************************************/

static int ringVibrator( UBYTE *c )
{
  return 0;
}


/*******************************************************************************

 $Function:     VibratorTimerEvent

 $Description:  Vibrator timer event

 $Returns:    Event consumed, always

 $Arguments:  e, event, t, timer

*******************************************************************************/

static int VibratorTimerEvent ( MfwEvt e, MfwTim *t )
{

  return MFW_EVENT_CONSUMED;
}

#endif
/*******************************************************************************

 $Function:     volumeSettingTimeOut

 $Description:  The Ringer volume settings time out has occured and used has not selected
        a volume setting.

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  e, event, tc timer context

*******************************************************************************/
static int volumeSettingTimeOut( MfwEvt e, MfwTim *tc )
{
  T_MFW_HND win = mfw_parent(mfw_header());
  T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
  T_idle * idle_data = (T_idle *)win_data->user;

  TRACE_EVENT(">>>> volumeSettingTimeOut(), Vol Setiing Time out. ");

  // stop running timer.
  if (hRingerVolSettingTimeOut != NULL ) {
    timStop(hRingerVolSettingTimeOut);
  }
//When the timer times out, stop the song and  the timer.
#ifdef FF_MIDI_RINGER  //Use Midi Player
	if(soundReason==SettingVolume)
	{
		// Are we playing a tune
		if ( hPlayerStartTimer!= NULL )
			timStop( hPlayerStartTimer );
		mfw_player_stop(sounds_midi_player_stop_cb);
	}
#endif

    if(call_data.win_menu !=NULL && (call_data.call_direction == MFW_CM_MTC))
  {
    bookMenuDestroy(call_data.win_menu);
    /*
      If volume settings dialog timed out while in the incoming call, allow the
      ringer to continue ringing. MZ 9/4/01
    */
#ifdef FF_MIDI_RINGER  
	mfw_ringer_start(AS_RINGER_MODE_IC,true,sounds_midi_ringer_start_cb);
#else
    /* Volume Setting for Silent */   
    startPlayingMelody(getCurrentRingerSettings());
#endif
    

  }
  else if (idle_data->info_win != NULL )
  {
    bookMenuDestroy(idle_data->info_win);
  }

    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

                                Public Methods

*******************************************************************************/
/* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
/* ===========================================================*/
/**
* restorePreviousVolumeSetting () will restore the previous volume setting once the menu has
* been scrolled.
*
* @param None.
*
*  @return None
*
*  @see         
*/
/* =========================================================== */

void restorePreviousVolumeSetting(void)
{
	current.volumeSetting = lastVolumeSetting;
}

/* ===========================================================*/
/**
* volumeMenuScrollUp () when menu button is moved up then this function will get called which
* decrements value for the global variable used to maintain the volume setting while scrolling 
* through the menu.
*
* @param None.
*
*  @return None
*
*  @see         
*/
/* =========================================================== */
void volumeMenuScrollUp (void)
{
	/* Decrement Volume setting when scroll up */
	scrollVolumeSetting = scrollVolumeSetting - 1 ;
	
	if (scrollVolumeSetting < SILENT_SETTING)
	{
		scrollVolumeSetting = MAX_VOLUME_SETTING;
	}

/*	current.volumeSetting = scrollVolumeSetting; */
	vsi_t_sleep(0,20);
}


/* ===========================================================*/
/**
* volumeMenuScrollDown () when menu button is moved down then this function will get called which
* increments value for the global variable used to maintain the volume setting while scrolling 
* through the menu.
*
* @param None.
*
*  @return None
*
*  @see         
*/
/* =========================================================== */
void volumeMenuScrollDown (void)
{
	/* Increment Volume setting when scroll down */
	scrollVolumeSetting = scrollVolumeSetting + 1;
	
	if (scrollVolumeSetting > MAX_VOLUME_SETTING)
	{
		scrollVolumeSetting = SILENT_SETTING;
	}

/*	current.volumeSetting = scrollVolumeSetting; */
	vsi_t_sleep(0,20);
}

/* ===========================================================*/
/**
* volumeMenuStartPlaying () Will start playing the default melody with the specified volume 
* control.
*
* @param None.
*
*  @return None
*
*  @see         
*/
/* =========================================================== */
void volumeMenuStartPlaying(void)
{
	    if(soundReason == SettingVolume)
	    {
			stopPlayingMelody ();
			startPlayingMelody(current.ringer); /* Play the first menu item which is being selected */
	    }
}

/* ===========================================================*/
/**
* volumeMenuStopPlaying () Will stop playing the default melody with the specified volume 
* control.
*
* @param None.
*
*  @return None
*
*  @see         
*/
/* =========================================================== */
void volumeMenuStopPlaying()
{
	    if(soundReason == SettingVolume)
	    {
			stopPlayingMelody ();
	    }	
}
#endif
/* END RAVI */

/*******************************************************************************

 $Function:     restartVolSettingInactivityTimer

 $Description:  Restart the Volume settings timer.

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void restartVolSettingInactivityTimer(void)
{
  if ( hRingerVolSettingTimeOut != NULL )
    timStop( hRingerVolSettingTimeOut );

  timStart(hRingerVolSettingTimeOut);

}
/*******************************************************************************

 $Function:     stopRingerVolTimeOut

 $Description:  Stop the ringer volume settings timer.

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void stopRingerVolSettingInactivityTimer(void)
{

  if ( hRingerVolSettingTimeOut != NULL )
    timStop( hRingerVolSettingTimeOut );
//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifdef FF_MIDI_RINGER  //Use Midi Player
	//Stop the Timer and stop the song before timeout
	if ( hPlayerStartTimer!= NULL )
	timStop( hPlayerStartTimer );
	mfw_player_stop(sounds_midi_player_stop_cb);
#endif
}
/*******************************************************************************

 $Function:     startRingerVolumeSettingTimer

 $Description:  Create and start the ringer voulme inactivity timer.

 $Returns:    none

 $Arguments:  none

*******************************************************************************/
void startRingerVolSettingInactivityTimer(T_MFW_HND parent_win)
{
  T_MFW_HND win = mfw_parent(mfw_header());
  T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
  T_idle * idle_data = (T_idle *)win_data->user;

  TRACE_FUNCTION(">>>> startRingerVolSettingInactivityTime, Create and Start Vol timer ");
  //Create 4 second ringer delay timer.
  if(hRingerVolSettingTimeOut == NULL )
  {
    if(call_data.call_direction == MFW_CM_MTC)
    {
   /* RAVI - 20-1-2006 */ 
#ifdef NEPTUNE_BOARD
	; /* do nothing */
#else
      hRingerVolSettingTimeOut = timCreate(call_data.win, TIM4SECOND, (MfwCb)volumeSettingTimeOut);
#endif
/* END RAVI */
    }
    else
    {
#ifdef NEPTUNE_BOARD
	; /* do nothing */
#else
      hRingerVolSettingTimeOut = timCreate(idle_data->win, TIM4SECOND, (MfwCb)volumeSettingTimeOut); 
#endif
    }
  }

  if ( hRingerVolSettingTimeOut != NULL )
    timStop( hRingerVolSettingTimeOut );

#ifdef NEPTUNE_BOARD
 /* RAVI - 20-1-2006 */
  /* Initialize the scrolling variable - When volume menu is activated */
  scrollVolumeSetting = 0;
  lastVolumeSetting = current.volumeSetting;
#endif

  
#ifndef NEPTUNE_BOARD
  timStart(hRingerVolSettingTimeOut);
#endif
/* END RAVI */


}
/*******************************************************************************

 $Function:     soundsRinger

 $Description:  prepares Ringer setting screen

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  m, menu, i, item

*******************************************************************************/

int soundsRinger( MfwMnu *m, MfwMnuItem *i )
{
  T_MFW_HND win;
    T_MFW_HND parent_win = mfw_parent(mfw_header());
/*Apr 05, 2005    REF: ENH 29994 xdeepadh */
#ifdef FF_MIDI_RINGER  
	int numberOfMelodies=0;
	
	TRACE_EVENT("soundsRinger");
	//Sep 11, 2006 DR: OMAPS00094182 xrashmic
			TRACE_EVENT("populating files");
			 sounds_midi_init();


	
	numberOfMelodies = sounds_midi_return_file_number();
	TRACE_EVENT_P1("numberOfMelodies is %d",numberOfMelodies);

	if(numberOfMelodies <= 0)
	{
		sounds_show_info(0, TxtMidiFileNotLoaded, TxtNull, NULL);
		return MFW_EVENT_CONSUMED;
	}
	else
	{
		//Set the voice limit to 32 and channel to stereo in idle mode
		//Set the audiopath to speaker.
		mfw_ringer_set_audiopath();
		mfw_player_midi_set_params(FF_MIDI_VOICE_LIMIT,MIDI_CHANNEL_STEREO);
		//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
		//Set the Mp3 Parameters .Size of the file where the melody must start is set 0(from beginning)
		//and channel is set to stereo.
#ifdef FF_MP3_RINGER		
		mfw_player_mp3_set_params(MP3_PLAY_FROM_START,MP3_CHANNEL_STEREO);
#endif		//FF_MP3_RINGER

		//Nov 21, 2005    REF:ER OMAPS00057430 nekkareb
		//Set the AAC Parameters .Size of the file where the melody must start is set 0(from beginning)
		//and channel is set to stereo.
#ifdef FF_AAC_RINGER		
		mfw_player_aac_set_params(AAC_PLAY_FROM_START,AAC_CHANNEL_STEREO);
#endif //FF_AAC_RINGER


		 hPlayerStartTimer = timCreate( 0, TIMHALFSECOND, (MfwCb)playCurrentMelody);
		win = buildMelodyMenu(parent_win);
		
		if (win NEQ NULL)
		{
			SEND_EVENT (win, E_INIT, 0, 0);
		}
		return MFW_EVENT_CONSUMED;
	}
#else
  /*recources_Init (win);*/

/* Timer not requried for Neptune - RAVI 23-12-2005 */
#ifndef NEPTUNE_BOARD
   /* Create 0.5 second ringer delay timer. */
   hRingerStartTimer = timCreate( 0, TIMHALFSECOND, (MfwCb)playCurrentSound);
#endif
/* END ADD BY RAVI - 23-12-2005 */

  win = buildMelodyMenu(parent_win);

   if (win NEQ NULL)
   {
      SEND_EVENT (win, E_INIT, 0, 0);
   }

/* For Neptune Not Required - RAVI - 23-12-2005 */
#ifndef NEPTUNE_BOARD
   /* Start the 0.5 second ringer delay timer. */
   delayThenPlayMelody();
#endif
/* END ADD BY RAVI - 23-12-2005 */

  return MFW_EVENT_CONSUMED;
#endif
}


/*******************************************************************************

 $Function:     soundsVolume

 $Description:  prepares Volume setting screen

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  m, menu, i, item

*******************************************************************************/

int soundsVolume( MfwMnu *m, MfwMnuItem *i )
{

  return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     settingsVibrator

 $Description:  prepares Vibrator settings screen

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  m, menu, i, item

*******************************************************************************/

int settingsVibrator( MfwMnu *m, MfwMnuItem *i )
{

  return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:     settingsLanguages

 $Description:  prepares languages screen

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  m, menu, i, item

*******************************************************************************/

int settingsLanguages( MfwMnu *m, MfwMnuItem *i )
{
  return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:     soundsNewMessage

 $Description:  prepares NewMessage setting screen

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  m, menu, i, item

*******************************************************************************/

int soundsNewMessage( MfwMnu *m, MfwMnuItem *i )
{

  return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:     soundsNewBroadcast

 $Description:  prepares Broadcast setting screen

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  m, menu, i, item

*******************************************************************************/

int soundsNewBroadcast( MfwMnu *m, MfwMnuItem *i )
{

  return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:     soundsBatteryLow

 $Description:  prepares BatteryLow setting screen

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  m, menu, i, item

*******************************************************************************/

int soundsBatteryLow( MfwMnu *m, MfwMnuItem *i )
{

  return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     soundsCreditLow

 $Description:  prepares Credit setting screen

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  m, menu, i, item

*******************************************************************************/

int soundsCreditLow( MfwMnu *m, MfwMnuItem *i )
{

  return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:     soundsOrganiser

 $Description:  prepares Organiser setting screen

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  m, menu, i, item

*******************************************************************************/

int soundsOrganiser( MfwMnu *m, MfwMnuItem *i )
{

  return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:     soundsAlarm

 $Description:  prepares Alarm setting screen

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  m, menu, i, item

*******************************************************************************/

int soundsAlarm( MfwMnu *m, MfwMnuItem *i )
{

  return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     soundInit

 $Description:  init sounds handling

 $Returns:    None

 $Arguments:  parent, handle of parent window

*******************************************************************************/

void soundInit( MfwHnd parent )
{


}


/*******************************************************************************

 $Function:     soundExit

 $Description:  exit sounds handling

 $Returns:    None

 $Arguments:  None

*******************************************************************************/

void soundExit (void)
{

}


/*******************************************************************************

 $Function:     soundExec

 $Description:  sends messages to soundExec to get the various melodies to play

 $Returns:    None

 $Arguments:  reason, for invocation, next, state

*******************************************************************************/

void soundExec (int reason, MmiState next)

{

}


/*******************************************************************************

 $Function:     soundsKeypadSilent

 $Description:  prepares KeypadTones setting screen

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  m, menu, i, item

*******************************************************************************/

int soundsKeypadSilent(MfwMnu* m, MfwMnuItem* i)
{

   T_MFW_HND win = mfw_parent(mfw_header());

  sounds_Init(win);

  if (FFS_flashData.settings_status & SettingsKeypadClick)
  {
    FFS_flashData.settings_status &= ~SettingsKeypadClick;
  }
  else if (FFS_flashData.settings_status & SettingsKeypadDTMF)
  {
    FFS_flashData.settings_status &= ~SettingsKeypadDTMF;
  }
  flash_write();
    sounds_data.local_status = ACTIVATE_CONFIRM;

  SEND_EVENT (sounds_data.sounds_win, SHOW_CONFIRM, 0, 0);

  return 0;
}
/*******************************************************************************

 $Function:     soundsKeypadClick

 $Description:  prepares KeypadTones setting screen

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  m, menu, i, item

*******************************************************************************/
int soundsKeypadClick(MfwMnu* m, MfwMnuItem* i)
{

   T_MFW_HND win = mfw_parent(mfw_header());

  sounds_Init(win);



  if (FFS_flashData.settings_status & SettingsKeypadDTMF)
  {
    FFS_flashData.settings_status &= ~SettingsKeypadDTMF;
  }

  FFS_flashData.settings_status |= SettingsKeypadClick;

/* RAVI - 20-1-2006 */
  /* play KeypadClick */
#ifdef NEPTUNE_BOARD
	audio_PlaySoundID(0, TONES_KEYBEEP, current.volumeSetting, AUDIO_PLAY_ONCE); 
#else  
       audio_PlaySoundID(0, TONES_KEYBEEP, 5, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */

    sounds_data.local_status = ACTIVATE_CONFIRM;

  SEND_EVENT (sounds_data.sounds_win, SHOW_CONFIRM, 0, 0);
  flash_write();
  return 0;
}

/*******************************************************************************

 $Function:     soundsKeypadDTMF

 $Description:  prepares KeypadTones setting screen

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  m, menu, i, item

*******************************************************************************/
int soundsKeypadDTMF(MfwMnu* m, MfwMnuItem* i)
{

   T_MFW_HND win = mfw_parent(mfw_header());

  sounds_Init(win);

  if (FFS_flashData.settings_status & SettingsKeypadClick)
  {
    FFS_flashData.settings_status &= ~SettingsKeypadClick;
  }



  FFS_flashData.settings_status |= SettingsKeypadDTMF;
  flash_write();

    sounds_data.local_status = ACTIVATE_CONFIRM;

  SEND_EVENT (sounds_data.sounds_win, SHOW_CONFIRM, 0, 0);

  return 0;
}



/*******************************************************************************

 $Function:     sounds_Init

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
T_MFW_HND sounds_Init (T_MFW_HND parent_window)
{
  return (sounds_create (parent_window));
}
/*******************************************************************************

 $Function:     confirm_Exit

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
void sounds_Exit (T_MFW_HND own_window)
{

    T_MFW_WIN   * win_data = ((T_MFW_HDR *)own_window)->data;
    T_MELODY_INFO * data     = (T_MELODY_INFO *)win_data->user;

  sounds_destroy (data->sounds_win);
}

/*******************************************************************************

 $Function:     sounds_create

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
T_MFW_HND sounds_create (T_MFW_HND parent_window)
{
  T_MELODY_INFO * data = &sounds_data;
  T_MFW_WIN   * win;

  TRACE_FUNCTION ("sounds_create()");

  data->sounds_win = win_create (parent_window, 0, 0, (T_MFW_CB)sounds_win_cb);

  if (data->sounds_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  data->mmi_control.dialog = (T_DIALOG_FUNC)sounds;
  data->mmi_control.data   = data;
  win                      = ((T_MFW_HDR *)data->sounds_win)->data;
  win->user                = (void *) data;


  winShow(data->sounds_win);
  /*
   * return window handle
   */
  return data->sounds_win;
}

/*******************************************************************************

 $Function:     sounds_destroy

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
void sounds_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN   * win;
  T_MELODY_INFO * data;

  TRACE_FUNCTION ("sounds_destroy()");

  if (own_window)
  {
    win  = ((T_MFW_HDR *)own_window)->data;
    data = (T_MELODY_INFO *)win->user;

    if (data)
    {

      /*
       * Delete WIN handler
       */
      win_delete (data->sounds_win);
      data->sounds_win = 0;
    }
  }
}
/*******************************************************************************

 $Function:     sounds_win_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
static int sounds_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
  /*
   * Top Window has no output
   */
  return 1;
}


/*******************************************************************************

 $Function:     sounds_dialog

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
static void sounds_dialog(void)
{
  T_DISPLAY_DATA display_info;

  TRACE_FUNCTION ("sounds_dialog()");

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtNull, TxtNull , COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)sounds_dialog_cb, THREE_SECS, KEY_CLEAR | KEY_RIGHT );
  if (sounds_data.local_status EQ ACTIVATE_CONFIRM)
  {
    display_info.TextId   = TxtActivated;
  }
  else
  {
     display_info.TextId    = TxtDeActivated;
  }

  /*
   * Call Info Screen
   */
  info_dialog (sounds_data.sounds_win, &display_info);

}
/*******************************************************************************

 $Function:     confirm_dialog_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
void sounds_dialog_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
  TRACE_FUNCTION ("sounds_dialog_cb()");

      switch (reason)
      {
        case INFO_TIMEOUT:
          /* no break; */
        case INFO_KCD_HUP:
          /* no break; */
        case INFO_KCD_LEFT:
          /* no break; */
        case INFO_KCD_RIGHT:
          /* no break; */
        case INFO_KCD_CLEAR:
            sounds_Exit(win);
          break;
      }
}

/*******************************************************************************

 $Function:     sounds

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
void sounds (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
 // T_MFW_WIN         * win_data = ((T_MFW_HDR *) win)->data;               // RAVI
 // T_MELODY_INFO      * data     = (T_MELODY_INFO *)win_data->user;    // RAVI

 TRACE_FUNCTION("sounds()");

 switch (event)
  {
    case SHOW_CONFIRM:
    sounds_dialog();
    break;
  }

}

//    Nov 29, 2004    REF: CRR 25051 xkundadu
//    Description: INCALL SCREEN ?ADJUSTING THE VOLUME
//    Fix: Added these functions to set the speaker volume to various levels.

void SetVolumeLevel1(void)
{
	
			
	FFS_flashData.output_volume = OutVolumeLevel1;
	// Set the volume 
	mmeSetVolume (1, FFS_flashData.output_volume);
	//write the value to the FFS
	flash_write();
  
}
void SetVolumeLevel2(void)
{
	
	
	FFS_flashData.output_volume = OutVolumeLevel2;
	// Set the volume 
	mmeSetVolume (1, FFS_flashData.output_volume);
	//write the value to the FFS
	flash_write();
  
}
void SetVolumeLevel3(void)
{
		
	
	FFS_flashData.output_volume = OutVolumeLevel3;
	// Set the volume 
	mmeSetVolume (1, FFS_flashData.output_volume);
	//write the value to the FFS
	flash_write();
  
}
void SetVolumeLevel4(void)
{
	
	
	FFS_flashData.output_volume = OutVolumeLevel4;
	// Set the volume 
	mmeSetVolume (1, FFS_flashData.output_volume);
	//write the value to the FFS
	flash_write();
  
}
void SetVolumeLevel5(void)
{
	 
	
	FFS_flashData.output_volume = OutVolumeLevel5;
	// Set the volume 
	mmeSetVolume (1, FFS_flashData.output_volume);
	//write the value to the FFS
	flash_write();
  
}
//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifdef FF_MIDI_RINGER  
/*******************************************************************************

 $Function:     sounds_midi_init

 $Description:Function to initialise the Midi files.

 $Returns:		None

 $Arguments:		None

*******************************************************************************/
void sounds_midi_init(void)
{
	TRACE_FUNCTION("sounds_init");
#ifdef FF_MIDI_RINGER  
#ifdef FF_MIDI_LOAD_FROM_MEM
	midi_files_count=MIDI_MAX_FILES;
#else
		//Nov 04, 2005    REF:DRT OMAPS00053737  xdeepadh
#ifdef FF_MIDI_LOAD_FROM_LFS
	//Populate midi files from LFS
	TRACE_EVENT("LFS");
    //Populate the list of  midi files from LFS
	midi_files_count=sounds_populate_midi_files(midi_file_names,MIDI_MAX_FILES,LFS_CONTENT_DIR);
#else
	TRACE_EVENT("FFS");
	midi_files_count=sounds_populate_midi_files(midi_file_names,MIDI_MAX_FILES,RINGER_CONTENT_DIR);
#endif		//FF_MIDI_LOAD_FROM_LFS
#endif		//FF_MIDI_LOAD_FROM_MEM
#endif		//FF_MIDI_RINGER
}

/*******************************************************************************

 $Function:     sounds_midi_exit

 $Description:Function to uninitialise the Midi files.

 $Returns:		None

 $Arguments:		None

*******************************************************************************/

void sounds_midi_exit(void)
{
	TRACE_FUNCTION("sounds_exit");
#ifndef FF_MIDI_LOAD_FROM_MEM
	sounds_unpopulate_midi_files(midi_file_names);
#endif
}

/*******************************************************************************

 $Function:     sounds_populate_midi_files

 $Description:Populates the MIDI file names into the array

 $Returns:		The count of the files in the directory

 $Arguments:		file_names:Pointer to the arrayto store file names
				max_files_count:Max file counts to be stored in array
*******************************************************************************/

int sounds_populate_midi_files(char **file_names, int max_files_count,char* dir_name_p)
{

	T_FFS_DIR	dir                  = {0};
	char		dest_name_p[100]            = "";
	int i, files_count = 0;
	char *fileExt;

	TRACE_FUNCTION("sounds_populate_midi_files");

	if (ffs_opendir (dir_name_p, &dir) <= 0)
	{
		files_count = 0;
	}
	//Sep 11, 2006 DR: OMAPS00094182 xrashmic
	//Release previous allocated memory before allocating again
	sounds_unpopulate_midi_files(midi_file_names);
	//read the  file names
	//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
	//file_count will be used to populate the array index
	for (i = 0; ffs_readdir (&dir, dest_name_p, 100) > 0x0; i++)
	{
		
		(file_names[files_count]) = (char*)mfwAlloc(strlen(dest_name_p) + 1);
		if(file_names[files_count] != NULL)
		{
			//Jul 20, 2005    REF: SPR 30772 xdeepadh
			//Retrieve file extension.Store only the supported file formats in array
			fileExt =sounds_midi_GetExtension(dest_name_p);
			//Oct 25, 2005    MP3 Ringer xdeepadh
			//List the MP3 files also.Mp3 files hsould not be listed or ULC
#ifdef FF_MP3_RINGER
			if ( (strcmp(fileExt, "mp3") == 0))
			{
				strcpy (file_names[files_count], dest_name_p);
				files_count++;
			}
			else
#endif		//FF_MP3_RINGER

			//Nov 21, 2005    ER: OMAPS00057430, AAC Ringer support nekkareb
			//List the AAC files also.
#ifdef FF_AAC_RINGER
			if ( (strcmp(fileExt, "aac") == 0))
			{
				strcpy (file_names[files_count], dest_name_p);
				files_count++;
			}
			else
#endif //FF_AAC_RINGER

#ifdef FF_MMI_MIDI_FORMAT

			//If XMF is supported
#ifdef  PAL_ENABLE_XMF 
			if ( (strcmp(fileExt, "xmf") == 0))
			{
				strcpy (file_names[files_count], dest_name_p);
				files_count++;
			}
			else
#endif  //PAL_ENABLE_XMF 

			//If IMELODY is supported
#ifdef PAL_ENABLE_IMELODY 
			 if ( (strcmp(fileExt, "imy") == 0))
			{
				strcpy (file_names[files_count], dest_name_p);
				files_count++;
			}
			else
#endif //PAL_ENABLE_IMELODY 

			//If SMAF is supported
#ifdef PAL_ENABLE_SMAF_MA3 
			 if ( (strcmp(fileExt, "mmf") == 0))
			{
				strcpy (file_names[files_count], dest_name_p);
				files_count++;
			}
			else
#endif ////PAL_ENABLE_SMAF_MA3 

			//If MIDI is supported
#ifdef PAL_ENABLE_MIDI_NORMALIZER 
			 if ( (strcmp(fileExt, "mid") == 0))
			{
				strcpy (file_names[files_count], dest_name_p);
				 files_count++;
			}
			else
#endif ////PAL_ENABLE_MIDI_NORMALIZER 
			//Sep 12, 2005    REF: SPR 33977 xdeepadh
			//mxmf file format has been supported
#ifdef PAL_ENABLE_XMF 
			if ( (strcmp(fileExt, "mxmf") == 0))
			{
				strcpy (file_names[files_count], dest_name_p);
				files_count++;
			}
			else
#endif //PAL_ENABLE_XMF 
/* Nov 23, 2006 ERT: OMAPS00101348  x0039928
     Fix: .sms files are populated                           */
#ifdef PAL_ENABLE_SMS
			if ( (strcmp(fileExt, "sms") == 0))
			{
				strcpy (file_names[files_count], dest_name_p);
				files_count++;
			}
			else
#endif //PAL_ENABLE_SMS
 /* Dec 05, 2006 ERT: OMAPS00101906  x0039928
      Fix: .wav files are populated                           */
#ifdef PAL_ENABLE_DIGITAL_AUDIO
			if ( (strcmp(fileExt, "wav") == 0))
			{
				strcpy (file_names[files_count], dest_name_p);
				files_count++;
			}
			else
#endif //PAL_ENABLE_DIGITAL_AUDIO     

#endif
			;
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
	//Closing the directory aa per the new FFS logic
#ifdef FF_MMI_RELIANCE_FFS	
	ffs_closedir(dir);
#endif
	TRACE_EVENT_P1("read  files_count is %d",files_count);
	return files_count;
}

/*******************************************************************************

 $Function:     sounds_unpopulate_midi_files

 $Description:Frees the memory allocated for the array
 $Returns:		None

 $Arguments:		file_names:Pointer to the arrayto store file names

*******************************************************************************/
void sounds_unpopulate_midi_files(char **file_names)
{
	int i;
	TRACE_FUNCTION("sounds_unpopulate_midi_files");
	//Sep 11, 2006 DR: OMAPS00094182 xrashmic
	//Free the allocated memory for file names
	for (i = 0; i<MIDI_MAX_FILES; i++)
	{
		if(file_names[i])
		{
	           mfwFree((U8 *)file_names[i],(U16)strlen(file_names[i]) + 1);
		}
		file_names[i]=NULL;
	}
    
}

/*******************************************************************************

 $Function:     sounds_midi_return_file_number

 $Description: returns the file count 

 $Returns:		Count of the Midi files

 $Arguments:		None

*******************************************************************************/
int sounds_midi_return_file_number(void)
{
	TRACE_FUNCTION("sounds_midi_return_file_number");
		
	if(midi_files_count<=0)
		return 0;
	else
		return midi_files_count; 

}
/*******************************************************************************

 $Function:     sounds_midi_return_file_name

 $Description: Returns the file name 

 $Returns:		Name of the file 

 $Arguments:		index:The index of the file, whose name has to be retrieved

*******************************************************************************/

char* sounds_midi_return_file_name(UBYTE index)
{

	TRACE_FUNCTION("sounds_midi_return_file_name");

#ifdef FF_MIDI_LOAD_FROM_MEM
	return  (char *)mmi_midi_files[index].melody_name;
#else
	TRACE_EVENT_P2("midi_file_names[%d] is %s",index,(char *)midi_file_names[index]);
	return (char *) midi_file_names[index];
#endif

	
}

/*******************************************************************************

 $Function:     sounds_midi_ringer_start_cb

 $Description: Callback function for ringer_start

 $Returns:		None

 $Arguments:

*******************************************************************************/

 void sounds_midi_ringer_start_cb(void *parameter)
{
	TRACE_EVENT("sounds_midi_ringer_start_cb");

	/* OMAPS00151698, x0056422 */
	#ifdef FF_MMI_A2DP_AVRCP
	if (((T_AS_START_IND *)parameter)->header.msg_id == AS_START_IND )
        {
                 tGlobalBmiBtStruct.tAudioState = FM_AUD_PLAY;
	          if(tGlobalBmiBtStruct.bConnected != BMI_BT_NOTCONNECTED)           
	          {  
	                 if( NULL != tGlobalBmiBtStruct.BmiEventCallback)
	                  tGlobalBmiBtStruct.BmiEventCallback(BMI_EVENT_USER_PLAY);
	                tGlobalBmiBtStruct.tCmdSrc == BMI_BT_COMMAND_NONE;
	            }    
    }
	#endif//FF_MMI_A2DP_AVRCP
	/* OMAPS00151698, x0056422 */
	
	if (((T_AS_STOP_IND *)parameter)->header.msg_id == AS_STOP_IND )
        {

		       /* OMAPS00151698, x0056422 */
				#ifdef FF_MMI_A2DP_AVRCP
                 tGlobalBmiBtStruct.tAudioState = FM_AUD_NONE;
				if(tGlobalBmiBtStruct.bConnected != BMI_BT_NOTCONNECTED)           
				{  
	                 if( NULL != tGlobalBmiBtStruct.BmiEventCallback)
	                  tGlobalBmiBtStruct.BmiEventCallback(BMI_EVENT_USER_STOP);
	                tGlobalBmiBtStruct.tCmdSrc == BMI_BT_COMMAND_NONE;
	            }    
				#endif//FF_MMI_A2DP_AVRCP
				/* OMAPS00151698, x0056422 */
        // Aug 05, 2005     xdeepadh
        #ifdef FF_MMI_AUDIO_PROFILE
		//configure the audio to voice path.
 		 mfwAudPlay = FALSE;
  		mfw_unset_stereo_path(mfw_get_current_audioDevice());
	#endif	
	 }

}

/*******************************************************************************

 $Function:     sounds_midi_ringer_stop_cb

 $Description: Callback function for ringer_stop

 $Returns:		None

 $Arguments:

*******************************************************************************/

 void sounds_midi_ringer_stop_cb(void *parameter)
{
	
	/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- Start */
	
	E_IN_CALL_RINGER_STATUS eStatus = 0;

	TRACE_EVENT("sounds_midi_ringer_stop_cb");

	/* Retrieve the Incoming Call -- Ringer Status */
	bmi_incomingcall_get_ringer_status(&eStatus);

	/* If a Ringer Stop had been done during an Incoming Call, then, enable the */
	/* Vocoder for the Voice to be heard */
	if ( eStatus == BMI_INCOMING_CALL_RINGERSTOPPED )
	{
		/* Vocoder Enable */
		mfw_aud_l1_enable_vocoder();
		
		bmi_incomingcall_set_ringer_status(BMI_INCOMING_CALL_NONE);
	}
	
	/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- End  */

	// Aug 05, 2005     xdeepadh
	 #ifdef FF_MMI_AUDIO_PROFILE
	//configure the audio to voice path
	mfwAudPlay = FALSE;
 	mfw_unset_stereo_path(mfw_get_current_audioDevice());
	#endif
}

/*******************************************************************************

 $Function:     sounds_midi_player_start_cb

 $Description: Callback function for player_start

 $Returns:		None

 $Arguments:

*******************************************************************************/

 void sounds_midi_player_start_cb(void *parameter)
{

#ifdef FF_MMI_TEST_MIDI
	T_AS_PLAYER_TYPE player_type;
#if FF_MIDI_LOAD_FROM_MEM
	T_MELODY melody_data;
#else
	char* midi_file;
#ifdef FF_MMI_FILEMANAGER
	char temp[FILENAME_MAX_LEN];
	UINT16 midi_file_uc[FILENAME_MAX_LEN];
#endif
#endif

	TRACE_EVENT("sounds_midi_player_start_cb");
	if (((T_AS_STOP_IND *)parameter)->header.msg_id == AS_STOP_IND )
             {
	if(mfw_player_exit_flag == PLAYER_EXIT_FLAG_FALSE)
	{

	 //if we didn't reach the last file
        	if(mfw_player_currently_playing_idx<(midi_files_count-1))
             	{
		mfw_player_currently_playing_idx++;
			TRACE_EVENT_P1("mfw_player_currently_playing_idx is %d",mfw_player_currently_playing_idx);
		mfw_player_save_selected_file_idx(mfw_player_currently_playing_idx);
#ifdef FF_MIDI_RINGER  
#ifdef FF_MIDI_LOAD_FROM_MEM
	melody_data=sounds_midi_return_memory_location(mfw_player_currently_playing_idx);
	player_type=mfw_ringer_deduce_player_type(melody_data.melody_name);
	mfw_player_start_memory(player_type,(UINT32*)melody_data.melody,melody_data.melody_size,(UBYTE)AS_VOLUME_HIGH,FALSE,sounds_midi_player_start_cb);
#else
	midi_file=sounds_midi_return_file_name(mfw_player_currently_playing_idx);
	strcpy(temp, "/FFS/mmi/tones/");
	strcat(temp, midi_file);
	player_type=mfw_ringer_deduce_player_type(midi_file);
#ifdef FF_MMI_FILEMANAGER
	convert_u8_to_unicode(temp, midi_file_uc);
	mfw_player_start_file(player_type,midi_file_uc,(UBYTE)AS_VOLUME_HIGH,FALSE,sounds_midi_player_start_cb);
#else
	mfw_player_start_file(player_type,midi_file,(UBYTE)AS_VOLUME_HIGH,FALSE,sounds_midi_player_start_cb);
#endif
#endif 	
#endif			 
}
		else
		{
			//x0pleela 26 Sep, 2006  DR: OMAPS00096365
			//Reset the index to -1 to avoid playing only the last file.
			mfw_player_currently_playing_idx = -1;
			//flag set to 1 means we only play the currently selected file
			mfw_player_exit_flag = PLAYER_EXIT_FLAG_TRUE;  
			// Aug 05, 2005     xdeepadh
#ifdef FF_MMI_AUDIO_PROFILE
			//configure the audio to voice path
			mfwAudPlay = FALSE;
			mfw_unset_stereo_path(mfw_get_current_audioDevice());
#endif
		}
	}
	else
	{
	// Aug 08, 2006    REF:DRT OMAPS00085889 x0039928
	// Fix: stereo path is unset after reaching the end of file.
#ifdef FF_MMI_AUDIO_PROFILE
			//configure the audio to voice path
			mfwAudPlay = FALSE;
			mfw_unset_stereo_path(mfw_get_current_audioDevice());
#endif
	}
	  
}
#endif//#ifdef FF_MMI_TEST_MIDI	 
}

/*******************************************************************************

 $Function:     sounds_midi_player_stop_cb

 $Description: Callback function for player_stop

 $Returns:		None

 $Arguments:

*******************************************************************************/

 void sounds_midi_player_stop_cb(void *parameter)
{
	TRACE_EVENT("sounds_midi_player_stop_cb");
	// Aug 05, 2005     xdeepadh
	 #ifdef FF_MMI_AUDIO_PROFILE
	//configure the audio to voice path
	mfwAudPlay = FALSE;
 	mfw_unset_stereo_path(mfw_get_current_audioDevice());
	#endif
}


/*******************************************************************************

 $Function:     sounds_show_info
 
 $Description:Displays a dialog 
 
 $Returns:

 $Arguments:
*******************************************************************************/
static MfwHnd sounds_show_info(T_MFW_HND parent, int str1, int str2, T_VOID_FUNC callback)
{
	T_DISPLAY_DATA display_info;
    TRACE_FUNCTION ("sounds_show_info()");

	/*
	** Create a timed dialog to display the Message "Failed"
	*/
	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, str1, str2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, callback, MT_INFO_SCRN_TIMEOUT, KEY_LEFT | KEY_CLEAR | KEY_HUP);
	return info_dialog(parent, &display_info);
}


/*******************************************************************************

 $Function:    		 get_ringer_mode

 $Description:  	 Returns the current ringer mode based on soundsreason (CALL/SMS/ALARM/Volume setting)
 $Returns:   		 the current ringer_mode

 $Arguments:  	None

*******************************************************************************/

int get_ringer_mode(void)
{
	T_AS_RINGER_MODE current_mode = NULL;  /* x0039928 - Lint warning fix */

	TRACE_FUNCTION("get_ringer_mode");
	
   switch(getSoundsReason())
	{
	case MelodySelect:
		current_mode=AS_RINGER_MODE_IC;
	break;

	case SettingSMSTone:
		current_mode=AS_RINGER_MODE_SMS;
	break;

	case SettingAlarm:
		current_mode=AS_RINGER_MODE_ALARM;
	break;

	case SettingVolume:
		current_mode=AS_RINGER_MODE_IC;
	break;

	}
   return current_mode;
}

/*******************************************************************************

 $Function:     playCurrentMelody

 $Description:  play midi file which is temporarily selected, this event handler
        is invoked when the hPlayerStartTimer event completes

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  e, event, tc timer context

*******************************************************************************/

static int playCurrentMelody( MfwEvt e, MfwTim *tc )
{

	TRACE_EVENT_P1("playCurrentMelody. The selected tone is %d",selected_tone);
	/* DRT OMAPS00163752 including changes from OMAPS00151417 . Apr 09, 2008. x0086292  - Start */

	// stop running timer 
	if (hPlayerStartTimer != NULL )
	{
		timStop(hPlayerStartTimer);
	}
	
	// wait for AS to stop playing file before starting to play new one
	if (mfwAudPlay == TRUE)
	{
		startPlayerTimer();
	}
	else 
	{
		soundsPlayMidiPlayer(selected_tone);
	}
	/* DRT OMAPS00163752 including changes from OMAPS00151417 . Apr 09, 2008. x0086292 - End */

	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     soundsPlayMidiPlayer

 $Description:  Play the tune selected by melody_id, stopping any currently
        playing tunes if necessary. 
 $Returns:    None

 $Arguments:  melody_id, selects the melody to be played

*******************************************************************************/
static void soundsPlayMidiPlayer( int melody_id )
{

#ifdef FF_MIDI_RINGER
T_AS_RINGER_INFO * ringer_info;
	T_AS_RINGER_INFO  temp_ringer_info;
		T_AS_PLAYER_TYPE player_type;
#ifdef FF_MIDI_LOAD_FROM_MEM
		T_MELODY melody_data;
#else
		char* midi_file;
#ifdef FF_MMI_FILEMANAGER
	char temp[FILENAME_MAX_LEN];
	UINT16 midi_file_uc[FILENAME_MAX_LEN];
#endif		
#endif
#endif

TRACE_EVENT_P1("soundsPlayMidiPlayer.The melody_id is %d",melody_id);
  CurrentTune = melody_id;
  	memset(&temp_ringer_info,'\0',sizeof(T_AS_RINGER_INFO)); /* x0039928 - Lint warning fix */
 	ringer_info=mfw_ringer_get_info(get_ringer_mode(),&temp_ringer_info);
  TRACE_EVENT_P1("ringer_info details volume  are %d  ",ringer_info->volume);
#ifdef FF_MIDI_LOAD_FROM_MEM
	melody_data=sounds_midi_return_memory_location(CurrentTune);
	player_type=mfw_ringer_deduce_player_type(melody_data.melody_name);
	mfw_player_start_memory(player_type,(UINT32*)melody_data.melody,melody_data.melody_size,ringer_info->volume,FALSE,sounds_midi_player_start_cb);
#else
	midi_file=sounds_midi_return_file_name(CurrentTune);
	TRACE_EVENT_P1("midi_file is %s",midi_file);
	player_type=mfw_ringer_deduce_player_type(midi_file);
#ifdef FF_MMI_FILEMANAGER
	strcpy(temp, "/FFS/mmi/tones/");
	strcat(temp, midi_file);
	convert_u8_to_unicode(temp, midi_file_uc);
	mfw_player_start_file(player_type,midi_file_uc,ringer_info->volume,FALSE,sounds_midi_player_start_cb);
#else
	mfw_player_start_file(player_type,midi_file,ringer_info->volume,FALSE,sounds_midi_player_start_cb);
#endif
#endif 		


 }

/*******************************************************************************

 $Function:     startPlayerTimer

 $Description:  Start the timer for Scrolling of Midi files
 $Returns:    None

 $Arguments:  None

*******************************************************************************/

void startPlayerTimer()

{
	// start timer for 5 milli second and on timeout  play the file
	timStart( hPlayerStartTimer );
}

/*******************************************************************************

 $Function:     stopPlayerTimer

 $Description:  Stop the timer for Scrolling of Midi files.Stop the melody, if anything is playing.
 $Returns:    None

 $Arguments:  None

*******************************************************************************/

static void stopPlayerTimer( void )
{
	TRACE_FUNCTION( "stopPlayerTimer" );

	/* If a timer event is running we want to stop that as well.
	since we want to turn the tune off and have
	it start again when the timer expires*/

	if ( hPlayerStartTimer != NULL )
		timStop( hPlayerStartTimer );
	// Stop the tone
	
	/* DRT OMAPS00163752 including changes from OMAPS00151417. Apr 09, 2008. x0086292  */
		if (mfwAudPlay == TRUE) 
			mfw_player_stop(sounds_midi_player_stop_cb);
}


/*******************************************************************************

 $Function:     playCurrentVolumeTone

 $Description:  play midi file with the volume which is temporarily selected, this event handler
        is invoked when the hPlayerStartTimer event completes

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  e, event, tc timer context

*******************************************************************************/

static int playCurrentVolumeTone( MfwEvt e, MfwTim *tc )
{

	TRACE_EVENT("playCurrentVolumeTone");
	soundsPlayVolumeMelody();
	// stop running timer 
	if (hPlayerStartTimer != NULL )
	{
		timStop(hPlayerStartTimer);
	}
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:     startVolumeTimer

 $Description:  Start the timer for modifying the volume of Midi files
 $Returns:    None

 $Arguments:  None

*******************************************************************************/

void startVolumeTimer()

{
	TRACE_EVENT("startVolumeTimer");
	// start timer for 5 milli second and on timeout  play the file
	timStart( hPlayerStartTimer );
}

/*******************************************************************************

 $Function:     stopVolumeTimer

 $Description:  Stop the timer for chnaging volume of Midi files.Stop the melody, if anything is playing.
 $Returns:    None

 $Arguments:  None

*******************************************************************************/

  void stopVolumeTimer(void)
{
	TRACE_EVENT("stopVolumeTimer");
	if ( hPlayerStartTimer != NULL )
		timStop( hPlayerStartTimer );
	// Are we playing a tune
	/* DRT OMAPS00163752 including changes from OMAPS00151417 . Apr 09, 2008. x0086292  */
	if (mfwAudPlay == TRUE) 
		mfw_player_stop(sounds_midi_player_stop_cb);
}

/*******************************************************************************

 $Function:     soundsPlayVolumeMelody

 $Description:  Play the  ringer tune with the selected volume, stopping any currently
        playing tunes if necessary. 
 $Returns:    None

 $Arguments:  melody_id, selects the melody to be played

*******************************************************************************/

  static void soundsPlayVolumeMelody()
{

T_AS_RINGER_INFO * ringer_info;
	T_AS_RINGER_INFO  temp_ringer_info;

	TRACE_EVENT("soundsPlayVolumeMelody");
	TRACE_EVENT_P1("the current volume now is %d",current.volumeSetting);
#ifdef FF_MIDI_RINGER
	//If going to the volume screen from Idle screen,change the incoming call volume.
	//mfw_player_stop(sounds_midi_player_stop_cb);
	memset(&temp_ringer_info, 0x0,sizeof(temp_ringer_info)); /* x0039928 - Lint warning removal */
	ringer_info=mfw_ringer_get_info(get_ringer_mode(),&temp_ringer_info);
#ifdef FF_MIDI_LOAD_FROM_MEM
	mfw_player_start_memory(ringer_info->player_type,ringer_info->data.memory.address,ringer_info->data.memory.size,current.volumeSetting,FALSE,sounds_midi_player_start_cb);
#else
	mfw_player_start_file(ringer_info->player_type,ringer_info->data.filename,(T_AS_VOLUME)current.volumeSetting,FALSE,sounds_midi_player_start_cb);
#endif 	
#endif

}

/*******************************************************************************

 $Function:     sounds_midi_return_memory_location

 $Description: Returns the Melody structure

 $Returns:		Structure containing the memory location, size and filename

 $Arguments:		index :index whose memory location has to be retrieved

*******************************************************************************/
/*a0393213 warnings removal-the entire function put under FF_MIDI_LOAD_FROM_MEM flag*/
#ifdef FF_MIDI_LOAD_FROM_MEM
T_MELODY  sounds_midi_return_memory_location(UBYTE index)
{
	TRACE_FUNCTION("sounds_midi_return_memory_location");
	return  mmi_midi_files[index];
}
#endif
//Jul 20, 2005    REF: SPR 30772 xdeepadh
/*******************************************************************************

 $Function:     sounds_midi_GetExtension

 $Description: public function to retrieve the extension of a file

 $Returns:Extention of the filename

 $Arguments: scr- Filename

*******************************************************************************/
 char *sounds_midi_GetExtension(char *src)
{
    int i,len;

	TRACE_FUNCTION("sounds_midi_GetExtension");
    len = strlen(src);   /* x0039928 - Lint warning fix */
    for(i = 0; i < len; i++){
        if(src[i] == '.'){
            return (src+i+1);
        }
    }
    return (src+i);
}

#endif

/*******************************************************************************

                                End Of File

*******************************************************************************/
