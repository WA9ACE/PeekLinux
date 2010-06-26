/*
+--------------------------------------------------------------------+
| PROJECT:	MMI-Framework (8417)		$Workfile::	mfw_ffs.h    			$|
| $Author::								$Revision::	1			    	$|
| CREATED:			     		$Modtime::								$|
| STATE  :	code														  |
+--------------------------------------------------------------------+

   MODULE  : mfw_ffs

   PURPOSE : This module contains FFS Interface function declarations.

   HISTORY:

	
 	Nov 22, 2007 DR: OMAPS00152315 x0080701(Bharat)
 	Description: MP3 - AAC cannot be disabled in Locosto
 	Solution: MP3 AAC Flags Usage have been corrected

    April 03 2007 ER:OMAPS00116772 x0061088(Prachi)
    Description:Support of 1.3 Mpixel camera on Locosto Plus
    Solution: added new member cam_auto_save in structure FlashData

	Mar 28, 2007  DR: OMAPS00122762 x0039928
	Description: MM: Deleting a PCM Voice Memo message in one particular memory, 
	delete them in all memories
	Solution: voice memo position and pcm voice memo position is provided for all the devices.
	
      Oct 30 2006, OMAPS00098881 x0039928(sumanth)
      Removal of power variant
      
      Oct 8 2006 OMAPS00097714 x0039928(sumanth)
      TTY HCO/VCO options in MMI
      

 	xashmic 27 Sep 2006, OMAPS00096389 
	ENH - a) Providing static menu for re-selection of enum options for enumeration
	b) Popup menu displayed on connect event, can be enabled or disabled via menu option

	xashmic 9 Sep 2006, OMAPS00092732
	USBMS ER

    June 27, 2006 REF:DVT OMAPS00083709  x0043642
    Description: RT: New feilds should be added at the end of the FlashData structure.
    Solution: Moved line1 and line2 attributes to the end of the structure.


    June 7, 2006 REF:ER OMAPS00078882  x0043642
    Description: RT: MTC causes TTY primitive even when TTY is not requested causing GTT crash
    Solution: Moved tty_audio_mode attribute to the end of the structure.

	Mar 15, 2006   ER: OMAPS00067709  x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: 	Added new field "pcm_voice_memo_position" into the structure "FlashData" 
			to hold the recording duration for PCM voice memo

	May 30, 2006  DR: OMAPS00070657 x0pleela
	Description: CPHS feature on Locosto-Lite
	Solution: For ALS feature,
			a) Added a new macro MAX_LINE_NAME_SIZE
			b) Added two new elements into "FlashData" structure to store the 
				alphanumeric name for line 1 and line2
	
	Apr 06, 2006    ERT: OMAPS00070660 x0039928(sumanth)
   	Description: Need to reduce flash foot-print for Locosto Lite 
   	Solution: Voice Memo feature is put under the flag #ifndef FF_NO_VOICE_MEMO to compile 
   	out voice memo feature if the above flag is enabled.
   	
	Nov 14, 2005    REF: OMAPS00044445 xdeepadh
	Description: Implementation of Test Menu for AAC
	Solution: The existing MP3 test Application has been enhanced to support the AAC file testing.
	The interfaces have been made generic to support both MP3 and AAC files.

   	Sep 27,2005 REF: SPR 34402 xdeepadh  
	Bug:Mono option is always highlighted even when Stereo is selected
	Fix:The channel setting will be saved in the FFS, which will be retrieved later
		and the selected channel will be higlighted.
	
	Aug 22 2005, xpradipg - LOCOSTO-ENH-31154
 	Description:	Application to test camera
 	Solution:	Implemented the camera application with following functionalities
 			preview, snapshot and image saving.
 			
	Apr 14, 2005	REF: CRR 29991   xpradipg
	Description:	Optimisation 5: Remove the static allocation and use dynamic 
					allocation/ deallocation for pb_list and black_list
	Solution:	The static definition is removed and replaced with the dynamic
					allocation   
   

    Aug 25, 2004  REF: CRR 20655  xnkulkar
    Description: Voice Memo functionality not working
    Solution:	  Added the prototype for flash_makedir() function.


    			 
    Mar 28, 2006 REF:ER OMAPS00071798  x0043642
    Added new options in BMI to accept the value for tty audio mode,
    and the accepted value is passed as parameter in function call sAT_PercentCTTY
*/

#ifndef MFW_FFS_H_
#define MFW_FFS_H_

/* BEGIN ADD: Sumit : Req ID: : 31-Mar-2005*/
#ifndef NEPTUNE_BOARD
/* END ADD: Sumit : Req ID: : 31-Mar-2005*/
#include "ffs/ffs.h" /*SPR 1920*/
/* BEGIN ADD: Sumit : Req ID: : 31-Mar-2005*/
#else
#include "ffs.h"
#endif
/* END ADD: Sumit : Req ID: : 31-Mar-2005*/

#ifndef PCM_2_FFS
#include "ffs_coat.h"
#endif

#ifdef FF_MMI_RINGTONE
#define MAX_RINGER_FILE_SIZE  50
#endif

//x0pleela 25 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
#define MAX_LINE_NAME_SIZE 10
#endif


typedef enum{
	SNAP_STG_FFS=0,
       SNAP_STG_NORMS,		
	SNAP_STG_NAND,
	SNAP_STG_MMC
}SNAP_STG;


typedef enum{
	CAM_RESOLUTION_VGA = 0,
    CAM_RESOLUTION_QCIF,
    CAM_RESOLUTION_SXGA
}CAM_RESOLUTION;


typedef struct
{   uint8 IMEI[16]; 	/* in ASCII*/
    uint8 IMEI_bcd[8]; 	/*in BCD*/
	/*SPR 1725 removed date time and alarm data*/
	/*Call timers*/
	uint32 last_call_duration;
	uint32 incoming_calls_duration;
	uint32 outgoing_calls_duration;

	/*MSSET data*/
	uint8 output_volume;
	uint8 external_audio;
#ifndef FF_NO_VOICE_MEMO
#ifdef FF_MMI_FILEMANAGER
	uint8 voice_memo_position[4];                /*Mar 28, 2007  DR: OMAPS00122762 x0039928 */
#else
	uint8 voice_memo_position;
#endif
#endif
//x0pleela 24 Feb, 2006  ER OMAPS00067709
//To hold the recording duration for PCM voice memo
#ifdef FF_PCM_VM_VB		
#ifdef FF_MMI_FILEMANAGER
	uint8 pcm_voice_memo_position[4];        /* Mar 28, 2007  DR: OMAPS00122762 x0039928 */
#else
	uint8 pcm_voice_memo_position;
#endif
#endif
	uint8 PLMN_selection_mode;
	uint8 CLIR; //seem to only be used for supplementary services.
//  Jun 23, 2004    REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
//              		Storing the CF state in Flash						
 uint8 CFState;
	
//Jun 23, 2004      REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
//				Comented "uint8 CLIP" as it is not being used anywhere in the code currently.
// uint8 CLIP; 
	uint8 redial_mode;
	uint8 call_info_display;
	uint8 contrast;
	uint8 brightness;
	uint8 backlight_duration;
    uint8 recent_ldn_ref;
    uint8 recent_lrn_ref;
    uint8 recent_upn_ref;
    uint8 time_format;

 	/* Network Log              */
    uint8 net_log_status;                 /* status                   */
    uint8 plmn_name[20];                /* plmn name                */
    uint8 network_name [7];             /* plmn name numeric        *//* x0039928 - Lint warning removal */

    /*Setting status*/

	uint8 settings_status;

	uint8 voice_mail[22];

	/*mailbox number*/
	uint8 mbn_AlphId[10];
	uint8 mbn_len;
	uint8 mbn_numTp;
	uint8 mbn_Num[10];

	/*ringtones etc for MMisounds*/
	uint8 ringer;							// index of permanent active tune in soundlist and ringerItem
	uint8 vibrator;							// vibrator
	uint8 volumeSetting;					// values (0 = silent, 4= loud, 5 = increasing)
	uint8 keypadOn;							// on/off values
	uint8 AlarmOn;
	uint8 OrganiserAlert;
	uint8 CreditLow;
	uint8 SMSTone;
	uint8 SMSBroadcast;
	uint8 battLow;
	uint8 earpiece;
	uint8 language;
	uint8 Equalizer;

    /*Data for MmiSmsBroadcast*/


	//Predictive text Flags
	uint8 PredTextAvailable;
	uint8 PredTextSelected;

	//API - 01/10/02
	//Concatenate Flags 
	uint8 ConcatenateStatus;

	//API - 06/12/02
	//Idle Screen Background image
	uint8 IdleScreenBgd;
	uint8 MainMenuBgd;
	uint8 ProviderNetworkShow;

	//CPHS ALS info
	uint8 als_selLine;
  	uint8 als_statLine;
  	uint8 ccbs_status;  /* Marcus: CCBS: 13/11/2002 */

  	/* MC SPR 1392, call deflection flag*/
	uint8 call_deflection;


  	/* SPR#1352 - SH - TTY */
  	uint8 ttyAlwaysOn;
	/*x0039928 OMAPS00097714 HCO/VCO option - added to store the type of TTY profile (normal/HCO/VCO)*/
#ifdef FF_TTY_HCO_VCO
	uint8 ttyPfType;
#endif

	/*API CQ10203 - Add the Flash define for Validity Period here*/
	uint8 vp_rel;
       uint8 image_usr_obj; // xrashmic 7 Dec, 2004 MMI-SPR-26161 and MMI-SPR-23965
       uint8 audio_usr_obj; // xrashmic 7 Dec, 2004 MMI-SPR-26161 and MMI-SPR-23965
//	Aug 22 2005, xpradipg - LOCOSTO-ENH-31154      
#if defined(FF_MMI_TEST_CAMERA) ||defined(FF_MMI_CAMERA_APP)
       uint8	camera_file_counter;
#endif
//Nov 14, 2005    REF: OMAPS00044445 xdeepadh

/* Nov 22, 2007 DR: OMAPS00152315 x0080701(Bharat) -- Commented #if defined */
// #if defined(FF_MMI_TEST_MP3) ||  defined(FF_MMI_TEST_AAC) 
//Sep 27,2005 REF: SPR 34402 xdeepadh   
//Variable to store the state of mp3 channel
 uint8	player_channel_state;
// #endif
/* Nov 22, 2007 DR: OMAPS00152315 x0080701(Bharat) -- Commented #endif */

#ifdef FF_MMI_RINGTONE
char cpRingerFilePath [MAX_RINGER_FILE_SIZE];
char cpSMSFilePath[MAX_RINGER_FILE_SIZE];
char cpAlarmFilePath[MAX_RINGER_FILE_SIZE];
#endif


/* June 27, 2006  REF:DVT OMAPS00083709 x0043642 */
#ifdef FF_CPHS
	char line1[MAX_LINE_NAME_SIZE+1]; // name of line1
	char line2[MAX_LINE_NAME_SIZE+1]; // name of line2
#endif


/* June 7, 2006  REF:ER OMAPS00078882 */
/*Removed tty_audio_mode from top and added here to solve OMAPS00078882*/
#ifdef NEPTUNE_BOARD
    uint8 tty_audio_mode;
#endif
//xashmic 9 Sep 2006, OMAPS00092732
//xashmic 27 Sep 2006, OMAPS00096389
#ifdef FF_MMI_USBMS
//0th bit - PS shutdown enable/disable
//1st bit - Popup menu enable/disable
uint8 usb_ms_flags;
#endif
/* To store the power management duration */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
    uint8 pm_duration; 
#endif
#endif
/*OMAPS00098881 (removal of power variant) x0039928(sumanth)
   This variable is used to store the status of LCD refresh
   1 - refresh needed
   0 - refresh not needed*/
#ifdef FF_POWER_MANAGEMENT
    BOOL refresh;
#endif
/*OMAPS00098351 (NITZ) x0066814(Geetha)*/
#ifdef FF_TIMEZONE
uint8 nitz_option;
#endif
UBYTE snap_storage;
UBYTE voicememo_storage;
UBYTE PCM_voicememo_storage;
UBYTE voicebuffer_storage;

//April 03 2007 ER:OMAPS00116772 x0061088(Prachi)
#ifdef FF_MMI_CAMERA_APP
UBYTE cam_auto_save;
//added by prachi 
#if CAM_SENSOR == 1
UBYTE cam_resolution;
UBYTE cam_changed_resolution;
#endif
#endif 
} FlashData;

extern FlashData FFS_flashData;

//flash access routines
effs_t flash_write(void);
int flash_read(void);
    /* Marcus: Issue 1719: 11/02/2003:
     * Changed return type to int: zero (EFFS_OK) if successful,
     * presumably -ve for errors, as in FFS_ERRORS (else read less than asked
     * for, but that value is internal to the function)
     */
effs_t flash_update(void);

//Generic flash access routines.
effs_t flash_data_write(const char* dir_name, const char* file_name, void* data_pointer, int data_size);
int flash_data_read(const char* dir_name, const char* file_name, void* data_pointer, int data_size);
     /* Marcus: Issue 1719: 11/02/2003:
      * Changed return type to int: data_size if successful,
      * presumably -ve for errors, as in FFS_ERRORS (else read less than asked for)
      */

     /* Aug 25, 2004  REF: CRR 20655  xnkulkar
         Added the prototype for flash_makedir() function
     */ 
void flash_makedir(char * dir_name); 
//	Apr 14, 2005	REF: CRR 29991   xpradipg
#ifdef FF_MMI_OPTIM
	int8 flash_MMI_blackList_open( );
	void flash_MMI_blackList_close(int8 handle);
	int flash_MMI_blackList_write(U8 *data, SHORT len, SHORT offset);
	int flash_MMI_blackList_read(int8 file,U8* data, SHORT len, SHORT offset);
#ifdef PCM_2_FFS
	T_FFS_SIZE ffs_ReadRecord(const char *name, void * addr, int size, int index, int recsize);
	T_FFS_RET ffs_WriteRecord(const char *name, void * addr, int size, int index, int recsize);
#endif
#endif
/* Added to remove warning Aug - 11 */
#ifdef NEPTUNE_BOARD
EXTERN int32 ffs_fread(const int8 *name, void *addr, int32 size);
EXTERN int8 ffs_fwrite(const int8 *pathname, void *src, int32 size);
#endif
/* End - remove warning Aug - 11 */
#endif
