#ifndef _MMI_SOUNDS_H_
#define _MMI_SOUNDS_H_ 1

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Sounds
 $File:		    MmiSounds.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
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
 $History: MmiSounds.h

	25/10/00			Original Condat(UK) BMI version.	

	Nov 04, 2005    REF:DRT OMAPS00053737  xdeepadh
	Description:   MIDI Ringer Linear Flash Support.
	Solution: The Midi Ringer application has been enhanced to loadand play the midi files from LFS.

	Apr 05, 2005    REF: ENH 29994 xdeepadh
	Description: Implementation of Generic Midi Ringer and Modification of existing Midi Test Application
	Solution: Generic Midi Ringer and Midi Test Application were implemented.

	Nov 29, 2004    REF: CRR 25051 xkundadu
	Description: INCALL SCREEN – ADJUSTING THE VOLUME
	Fix: Added volume level list linked to up/down keys. 
	User can select the speaker volume among those levels.   
	
	Dec 23, 2005  REF: SR13878 x0020906
	Description:  Set Default ring tone value.
	Fix : Added a function setDefaultSound ()		       		     	

 $End

*******************************************************************************/
#include "MmiLists.h" 

#ifdef FF_MIDI_RINGER  
#include "general.h"
#endif
/*******************************************************************************
                                                                              
                                Basic definitions
                                                                              
*******************************************************************************/

/* Redefine the alloc and free memory routines for use in the sounds
   module.
*/

#define ALLOC_MEMORY mfwAlloc
#define FREE_MEMORY  mfwFree

/* Apr 05, 2005    REF: ENH 29994 xdeepadh */
/* Maximum files to be displayed in the MMI */
#define MIDI_MAX_FILES 10



/* Provide message ids for the messages we will deal with in
   the sounds module.
*/
typedef enum
{
	SoundsNone = SoundsDummy,
	SoundsStop,
	SoundsPagingStart,
	SoundsMuteClick,
	SoundsRingAlarm,
	SoundsRingOrganiser,
	SoundsCreditRing,
	SoundsSMSRing,
	SoundsSMSBroadcast,
	SoundsErrorTone,
	SoundsBattLowIdle,
	SoundsBattLowDedic,
	SoundsConnCharger,
	SoundsDisconnCharger,
	SoundsSwitchOff,
	MelodySelect,
	SettingVibrator,
	SettingKeypadTone,
	SettingSMSTone,
	SettingSMSBroadcastTone,
	SettingBatteryLowBeep,
	SettingCreditLowBeep,
	SettingOrganiserAlert,
	SettingAlarm,
	SettingVolume,
	SettingLanguage,
	SoundsDone,
	SoundsCancel
} SoundSettingState;

//    Nov 29, 2004    REF: CRR 25051 xkundadu
//    Description: INCALL SCREEN – ADJUSTING THE VOLUME
//    Fix: Added these enum variables corresponds to speaker volume evels.
//          Maximum value allowed is 255 and minimum value is 1. So eqully divided into
//		five levels.
typedef enum
{
	OutVolumeLevel1 = 1,
	OutVolumeLevel2 = 60,
	OutVolumeLevel3 = 125,
	OutVolumeLevel4 =190,
	OutVolumeLevel5 = 255
} T_OUT_VOLUME_LEVEL;

typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND        sounds_win; 				   /* MFW win handler      */
  T_MFW_HND        parent;
  T_MFW_HND        melody_handler;         /* MFW sms handler      */
  unsigned char      local_status;
  ListMenuData     * menu_list_data;
} T_MELODY_INFO;

#ifdef FF_MIDI_RINGER  
typedef struct 
{
     const char* melody_name;
     const void* melody;
     UINT32  melody_size;	
} T_MELODY;
#endif

/*******************************************************************************
                                                                              
                                Public Methods
                                                                              
*******************************************************************************/

void soundInit( MfwHnd parent );
void soundExit( void );
void soundExec( int reason, MmiState next );

/* functions setting the focus for entering configuration menus.
*/
#ifdef NEPTUNE_BOARD
void volumeMenuScrollUp(void);
void volumeMenuScrollDown(void);
void restorePreviousVolumeSetting(void);
void volumeMenuStartPlaying(void);
#endif

int settingsVibrator(   MfwMnu* m, MfwMnuItem* i );
int settingsLanguages(  MfwMnu* m, MfwMnuItem* i );
int soundsRinger(       MfwMnu* m, MfwMnuItem* i );
int soundsVolume(       MfwMnu* m, MfwMnuItem* i );

int soundsKeypadSilent(MfwMnu* m, MfwMnuItem* i);
int soundsKeypadClick(MfwMnu* m, MfwMnuItem* i);
int soundsKeypadDTMF(MfwMnu* m, MfwMnuItem* i);

int soundsNewMessage(   MfwMnu* m, MfwMnuItem* i );
int soundsNewBroadcast( MfwMnu* m, MfwMnuItem* i );
int soundsBatteryLow(   MfwMnu* m, MfwMnuItem* i );
int soundsCreditLow(    MfwMnu* m, MfwMnuItem* i );
int soundsOrganiser(    MfwMnu* m, MfwMnuItem* i );
int soundsAlarm(        MfwMnu* m, MfwMnuItem* i );

void soundReasonRinger(void);
void soundReasonSmsTone(void);
void soundReasonAlarm(void);

void soundsVolSilent(void);
void soundsVolLow(void);
void soundsVolMedium(void);
void soundsVolHigh(void);
void soundsVolInc(void);
void restoreSilentModeVolume(void);
void setSilentModeVolume(void);

UBYTE getCurrentVoulmeSettings(void);
void setSoundsReason(USHORT reason);

//    Nov 29, 2004    REF: CRR 25051 xkundadu
//    Description: INCALL SCREEN – ADJUSTING THE VOLUME
//    Fix: Prototypes for the volume setting functions.
void SetVolumeLevel1(void);
void SetVolumeLevel2(void);
void SetVolumeLevel3(void);
void SetVolumeLevel4(void);
void SetVolumeLevel5(void);

/* Apr 05, 2005    REF: ENH 29994 xdeepadh */
#ifdef FF_MIDI_RINGER  
int sounds_midi_return_file_number(void);
char* sounds_midi_return_file_name(UBYTE index);
/*a0393213 warnings removal-sounds_midi_return_memory_location function prototype put under FF_MIDI_LOAD_FROM_MEM flag*/
#ifdef FF_MIDI_LOAD_FROM_MEM
T_MELODY  sounds_midi_return_memory_location(UBYTE index);
#endif
//Nov 04, 2005    REF:DRT OMAPS00053737  xdeepadh
int sounds_populate_midi_files(char **file_names, int max_files_count,char* dir_name_p);
void sounds_midi_init(void);
void sounds_midi_exit(void);
void sounds_unpopulate_midi_files(char **file_names);
void sounds_midi_ringer_start_cb(void *parameter);
void sounds_midi_ringer_stop_cb(void *parameter);
void sounds_midi_player_start_cb(void *parameter);
void sounds_midi_player_stop_cb(void *parameter);
int get_ringer_mode(void);
void IdleScreenVolumeTimer();
 char *sounds_midi_GetExtension(char *src);//Jul 20, 2005    REF: SPR 30772 xdeepadh

 /* x0045876, 14-Aug-2006 (WR - function declared implicitly) */
 #else
int resource_GetListCount(res_ResourceID_type res);
 
#endif

/* Store into structure from FFS - RAVI - 23-12-2005 */
#ifdef NEPTUNE_BOARD
void setDefaultSound(void);
#endif

/* x0045876, 14-Aug-2006 (WR - getcurrentAlarmTone function declared implicitly) */
UBYTE getcurrentAlarmTone(void);

/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif //_MMI_SOUNDS_H_
