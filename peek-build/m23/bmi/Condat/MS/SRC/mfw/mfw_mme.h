/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_mme.h       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 2               $|
| CREATED: 11.02.99                     $Modtime:: 22.02.00 11:20   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_MME

   PURPOSE : Miscelleaneous Mobile Equipment

   EXPORT  :

   TO DO   :

   $History:: mfw_mme.h                                             $
 ************************************************************************

    Mar 22, 2007  DVT: OMAPS00121916  x0039928(sumanth)
    Description: T-Flash hot swap.
    
 	xashmic 27 Sep 2006, OMAPS00096389 
	ENH - a) Providing static menu for re-selection of enum options for enumeration
	b) Popup menu displayed on connect event, can be enabled or disabled via menu option

	xashmic 9 Sep 2006, OMAPS00092732
	USBMS ER

 *	Apr 17, 2006  ER: OMAPS00075178  x0pleela
 *   	Description: When the charger connect to phone(i-sample), the charger driver works, but the MMI and App don't work
 * 	Solution: Defined events for battery charger status
 *
 *  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
 * 	Description:	Support for various audio profiles
 *	Solution:	The support for audio device's carkit/headset/loudspeaker/handheld.
 *			The audio device is enabled as per the user actions.
 *
 * *****************  Version 2  *******************************************
 * User: Es           Date: 2.03.00    Time: 16:48
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * generate battery/signal primitives with more info from driver
 * callbacks. Catch the primitives on reception and call MMI event
 * handler. Don't use ACI any longer for this purpose.
 *
 * *****************  Version 1  *****************
 * User: Es           Date: 20.02.99   Time: 11:54
 * Created in $/GSM/DEV/MS/SRC/MFW
 * mobile equipment
*/

#ifndef _DEF_MFW_MME_H_
#define _DEF_MFW_MME_H_

typedef struct MfwMmeTag                /* Equipment CONTROL BLOCK  */
{
    MfwEvt map;                         /* selection of events      */
    MfwCb handler;                      /* event handler            */
    U32 value;                          /* event related value      */
} MfwMme;

                                        /* EQUIPMENT FLAGS & EVENTS */
#define MfwMmeSignal        1           /* signal quality change    */
#define MfwMmeBattery       2           /* battery level change     */
#define MfwMmeBaState       4           /* state of power supply    */
#define MfwMmeRtcAlarm      8           /* real time clock alarm    */
#define MfwMmeHdsDetect    16           /* headset detection        */
#define MfwMmeIrdaMsg      32           /* IRDA event               */
//  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
#ifdef FF_MMI_AUDIO_PROFILE
#define MfwMmeHeadset 64	/* headset event */
#define MfwMmeCarkit 128		/* carkit event */
#endif
#ifdef FF_MMI_FILEMANAGER
#define TFLASH_MMI_IND      0x1110

typedef enum
{
	MC_INSERT_EVENT = 0,	//MC Inserted
	MC_REMOVE_EVENT		//MC Removed
}T_MC_EVENT;

typedef struct
{
  T_MC_EVENT                      mc_status;                  
} T_TFLASH_MMI_IND;
#endif

//xashmic 9 Sep 2006, OMAPS00092732
#ifdef FF_MMI_USBMS
#define MfwMmeUSBMS	256

/*Defining the premitive*/
#define USBMS_MMI_IND      0x1000   //This value has been set keeping in mind that it would not clash with exisitng IND created via sap editor.

/* Mar 22, 2007  DVT: OMAPS00121916  x0039928 */

typedef struct
{
  U32                       usb_status;                  
} T_USBMS_MMI_IND;
typedef enum
{
	MFW_USB_ENUM = 0,	//USB Proceed With ENUM
	MFW_USB_NO_ENUM		//USB Do not Enumerate
}T_MFW_USB_ENUM_CONTROL;
typedef enum
{
	MFW_USB_TYPE_MS = 0,	
	MFW_USB_TYPE_TRACE,
	MFW_USB_TYPE_AT,
	MFW_USB_TYPE_AT_TRACE,//xashmic 27 Sep 2006, OMAPS00096389 
	MFW_USB_TYPE_MS_TRACE,//xashmic 27 Sep 2006, OMAPS00096389 
	MFW_USB_TYPE_AT_MS,//xashmic 27 Sep 2006, OMAPS00096389 
	MFW_USB_TYPE_ALL
}T_MFW_USB_ENUM_TYPE;
typedef enum
{
	MFW_USB_CABLE_CONNECT_EVENT = 0,	//USB Cable Connection Detected
	MFW_USB_CABLE_DISCONNECT_EVENT,		//USB Cable Removal Detected
	MFW_USB_ENUM_SUCCESS_EVENT,			//USB Enumeration Successful
	MFW_USB_ENUM_FAILURE_EVENT			//USB Enumeration Failed
#ifdef FF_MMI_FILEMANAGER
	,MFW_TFLASH_INSERTED_EVENT,
	MFW_TFLASH_REMOVED_EVENT
#endif	
}T_MFW_USB_EVENT;


//xashmic 27 Sep 2006, OMAPS00096389 
#define MFW_USBMS_PS_SHUTDOWN 0x01
#define MFW_USBMS_PS_NO_SHUTDOWN (~MFW_USBMS_PS_SHUTDOWN)
#define MFW_USBMS_POPUP_ENABLE 0x02
#define MFW_USBMS_POPUP_DISABLE (~MFW_USBMS_POPUP_ENABLE)
//PS shutdown disabled and Popup disabled
#define MFW_USBMS_DEFAULT MFW_USBMS_POPUP_ENABLE

#endif

//Light level settings 
#define BL_NO_LIGHT 0
#define BL_MAX_LIGHT 255
#define BL_SET_IDLE -1
#define BL_NO_CHANGE -2

//Events that may affect the backlight 
enum {
	BL_INIT,
	BL_IDLE_TIMER,
	BL_KEY_PRESS,
	BL_INCOMING_SMS,
	BL_INCOMING_CALL,
	BL_EXIT,
	BL_SPARE_OPTION_1,
	BL_SPARE_OPTION_2,
	BL_SPARE_OPTION_3,
	BL_LAST_OPTION
};
//  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
#ifdef FF_MMI_AUDIO_PROFILE
#define MFW_AUD_HANDHELD			1
#define MFW_AUD_LOUDSPEAKER		2
#define MFW_AUD_HEADSET			3
#define MFW_AUD_CARKIT			4
#endif

//x0pleela 10 Apr, 2006
//Events for battery charger status
enum{
	PWR_CHG_BEGIN= 2,  // charger begin 
	PWR_CHG_STOP =  3,  // charger end
	PWR_CHG_PLUG =  4, // charger plug 
	PWR_CHG_UNPLUG = 5 // charger unplug
};
                                        /* PROTOTYPES               */
MfwRes mmeInit (void);
MfwRes mmeExit (void);
MfwHnd mmeCreate (MfwHnd w, MfwEvt e, MfwCb f);
MfwRes mmeDelete (MfwHnd h);
void mmeSignal (U32 evt, U32 val);
void mmeBackLight (U8 level);
void mme_setBacklightEvent(int event, int lightLevel);
void mme_backlightEvent(int event);
void mmeAudioTone (U8 callTone, U8 volume, U8 toneStat);
MfwRes mmeSetVolume (U16 audioIn, U16 audioOut);
MfwRes mmeRtcInit (USHORT drvHandle, UBYTE *info);
MfwRes mmeHdsInit (void (*hds_init) ());

void mmeRxInfo (U8 intervalls, U16 level);
void mmeBattInfo (U8 intervalls, U16 level);
//  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
#ifdef FF_MMI_AUDIO_PROFILE
EXTERN void mfw_auido_device_indication(U8 notify);
EXTERN int mfw_get_current_audioDevice();
EXTERN void mfw_unset_stereo_path(int device);
EXTERN void mfw_set_stereo_path(int device);
#endif
#endif
