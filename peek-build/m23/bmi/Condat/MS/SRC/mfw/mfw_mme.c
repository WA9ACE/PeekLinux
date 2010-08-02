/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_mme.c       $|
| $Author:: Kk  $ CONDAT GmbH           $Revision:: 5               $|
| CREATED: 11.02.99                     $Modtime:: 3.03.00 17:40    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_MME

   PURPOSE : Miscelleaneous Mobile Equipment functions

   EXPORT  :

   TO DO   :

   $History:: mfw_mme.c                                             $
 ************************************************************************

	Sep 26, 2007    Configuration of Drives for SBuild 2.3.0 Migration  x0080701 (Bharat)
	Description:   Support from MMI to configure drives (NOR, NORMS, NAND, TFLASH)
	                    is added as part of SBuild 2.3.0 Migration

    May 30, 2007  DR: OMAPS00132483 x0pleela
    Description: MM: While playing an AAC, unplug USB freezes the audio
    Solution: 1. Added the prototype to get the phone lock status
    		    2. Replaced the function call mfw_get_phlock_status with mfw_get_Phlock_status
	    		    to get the phone lock status
	
    Apr 03, 2007    ER: OMAPS00122561  x0pleela
    Description:   [ACI] Phone lock feature has to be supported by ACI
    Solution: Phone Lock ER implementation


    Mar 22, 2007  DVT: OMAPS00121916  x0039928(sumanth)
    Description: T-Flash hot swap.
    
    Oct 30 2006, OMAPS00098881 x0039928(sumanth)
    Removal of power variant
      
    Oct 05, 2006    ER: OMAPS00094496 x0061088(Prachi)
    Description:Enhance RSSI to 3 parameters: Strength, quality, min-access-level
    Solution:To solve the ER OMAPS0094496,mmeInit() is not calling rx_Init(). 


 	xashmic 27 Sep 2006, OMAPS00096389 
	ENH - a) Providing static menu for re-selection of enum options for enumeration
	b) Popup menu displayed on connect event, can be enabled or disabled via menu option

	xashmic 21 Sep 2006, OMAPS00095831
 	Description: USB enumeration does not happen in PIN/PUK/Insert SIM scenarios
 	Solution: When booting up without SIM card, or prompting for PIN/PUK/SIMP entry, 
 	enumerate all the USB ports automatically 

	xashmic 9 Sep 2006, OMAPS00092732
	USBMS ER
 *   
 *	Jun 02, 2006  DRT OMAPS00079213   x0039928(sumanth)
 *	Description: Triton PG2.2 impacts on Locosto program
 *	Solution: Software workaround is done to register headset-hook dynamically 
 *	during/before call is established and unregister it after the call is ended/disconnected.
 *
 *    xdeepadh, nekkareb - OMAPS00076996: 05 May 2006
 * 	Description: MO TTY call works only for the first time
 *	Solution: For the Loocsto-lite variant, when a TTY call is initiated, the 
 *     current audio profile is identified and when the TTY call is ended, this 
 *    profile is loaded.
 *
 *   	nekkareb - OMAPS00057367: 8 Dec 2005
 * 	Description: Fix build problems for fix of OMAPS00057367
 *	Solution: Included OMAPS00057367 changes under a flag ISAMPLE
 *
 *  	nekkareb - OMAPS00057367: 3 Dec 2005
 * 	Description: Locosto: TTY MT call does not work
 *	Solution: Extra TTY command added so as to initiate the AUDIO entity to send the 
 *	              TTY_START request to L1 only after the Vocoder enable is sent.
 *                  Current audio device is read from audio service entity functions.
 *
 *  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
 * 	Description:	Support for various audio profiles
 *	Solution:	The support for audio device's carkit/headset/loudspeaker/handheld.
 *			The audio device is enabled as per the user actions.
 *
 * *****************  Version 5  ********************************************
 * User: Kk           Date: 3.03.00    Time: 17:52
 * Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 4  *****************
 * User: Es           Date: 2.03.00    Time: 16:48
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * generate battery/signal primitives with more info from driver
 * callbacks. Catch the primitives on reception and call MMI event
 * handler. Don't use ACI any longer for this purpose.
 *
 * *****************  Version 3  *****************
 * User: Es           Date: 14.04.99   Time: 17:34
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * moved to CST
|
| *****************  Version 2  *****************
| User: Le           Date: 14.04.99   Time: 9:51
| Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 1  *****************
 * User: Es           Date: 20.02.99   Time: 11:54
 * Created in $/GSM/DEV/MS/SRC/MFW
 * mobile equipment
*/

#define ENTITY_MFW

#include <stdio.h>

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#include "mfw_mfw.h"
#include "mfw_sys.h"
#include "prim.h"
#include "message.h"
#include "prim.h"
#include "aci_cmh.h"
#include "gdi.h"
#include "audio.h"
#include "pwr.h"
#include "rx.h"
#include "light.h"
#include "mfw_acie.h"
#include "mfw_mme.h"
#include "mfw_win.h"
#include "mfw_ffs.h" /*OMAPS00098881 x0039928(sumanth)*/
//xashmic 9 Sep 2006, OMAPS00092732	
#ifdef FF_MMI_USBMS
#include "usb/usb_api.h"
#endif

//x0pleela 01 Apr, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
#include "mfw_sim.h"
#endif //FF_PHONE_LOCK

/* OMAPS00057367, 8 Dec 2005, nekkare, build fix */
#if (BOARD == 71)
#define ISAMPLE
#endif

#define hCommPL _ENTITY_PREFIXED(hCommPL)
#define hCommACI _ENTITY_PREFIXED(hCommACI)

#if defined (NEW_FRAME)
EXTERN T_HANDLE hCommPL;
EXTERN T_HANDLE hCommACI;
#else
EXTERN T_VSI_CHANDLE hCommPL;
EXTERN T_VSI_CHANDLE hCommACI;
#endif

EXTERN MfwHdr * current_mfw_elem;

                                        /* FLAG DRIVER CALLBACKS    */
#define MmeFlagRtcAlarm         1       /* rtc alarm                */
#define MmeFlagHdsDetect        2       /* headset detection        */
#define MmeFlagIrdaMsg          4       /* IRDA event               */
extern USHORT mfwMmeDrvFlag;           /* hold driver CB flags     */


static U8 battLevel = 0;                /* battery level            */
static U8 battState = 0;                /* state of power supply    */
static U16 hdsPresence = 0;				/* headset presence         */


static int mmeCommand (U32 cmd, void *h);

                                        /* driver callbacks         */
static void pwrCb (drv_SignalID_Type *signal_params);
static void rxCb (drv_SignalID_Type *signal_params);
static void rtcCb (drv_SignalID_Type *signal_params);

/* Added to remove warning Aug - 11 */
EXTERN UBYTE dspl_Enable (UBYTE   in_Enable);
/* End - remove warning Aug - 11 */ 

static BOOL mmePrimHandler (USHORT opc, void *data);
static int pSlot;                       /* primitive handler slot   */
#ifndef FF_MMI_AUDIO_PROFILE /*a0393213 warnings removal*/
static void hdsCb (drv_SignalID_Type *signal_params);
#endif
//  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
#ifdef FF_MMI_AUDIO_PROFILE
#define HANDHELD		"handheld" //0
#define LOUDSPEAKER		"handfree" // 1
#define HEADSET			"headset"    // 2
#define CARKIT			"carkit"   // 3
static UBYTE currAudioDevice = MFW_AUD_HANDHELD;

UBYTE headsetInserted = FALSE;	//indicates if the headset is inserted or not
UBYTE carkitInserted = FALSE;		//indicates if the carkit is inserted or not
UBYTE mfwAudPlay = FALSE;		//indicates if any other audio media is being played or not
extern audio_set_device(char* mode);
// Jun 02, 2006  DRT OMAPS00079213   x0039928(sumanth)
// Fix: A callback function is added that is registered for hook.
void mfw_audio_hook_indication ( T_AUDIO_ACCESSORY_NOTIFICATION notify, 
ACCESSORY_CALLBACK_PARAM callbackparam );
void mfw_audio_headSet_indication ( T_AUDIO_ACCESSORY_NOTIFICATION notify, 
ACCESSORY_CALLBACK_PARAM callbackparam );
void mfw_audio_carKit_indication ( T_AUDIO_ACCESSORY_NOTIFICATION notify, 
ACCESSORY_CALLBACK_PARAM callbackparam );
#endif

/* Mar 22, 2007  DVT: OMAPS00121916  x0039928 */
/* Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- Start */
#ifdef FF_MMI_TFLASH
void tflash_notify_register(void);
typedef void (*T_MC_EVENT_CALLBACK)(T_MC_EVENT mcEvent, void *ctx);
#endif
/* Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- End */

void mfw_audio_set_device(int type);
//xdeepadh, nekkareb - OMAPS00076996: 05 May 2006
//Array to retrieve the profile name
#ifdef MMI_LITE
char aud_cg_file[5][15] = {"handheld","handheld","handfree","headset","carkit"};
#endif

//x0pleela 29 May, 2007 DR: OMAPS00132483
#ifdef FF_PHONE_LOCK
EXTERN int mfw_get_Phlock_status(void);
#endif // FF_PHONE_LOCK

/* Mar 22, 2007  DVT: OMAPS00121916  x0039928 */
/* Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- Start */
#ifdef FF_MMI_TFLASH
void tflash_cb(T_MC_EVENT mcEvent, void *ctx)
{
	PALLOC(tflash_mmi_ind, TFLASH_MMI_IND);
	tflash_mmi_ind->mc_status = mcEvent;
	PSENDX(ACI,tflash_mmi_ind);
}
#endif
/* Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- End */

//xashmic 9 Sep 2006, OMAPS00092732	
#ifdef FF_MMI_USBMS
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : usbms_cb            |
+--------------------------------------------------------------------+

  PURPOSE : Posts the USB events to the MMI task 

*/

void usbms_cb (T_USB_EVENT usbEvent, void *ctx)
{
	PALLOC(usbms_mmi_ind,USBMS_MMI_IND);
	usbms_mmi_ind->usb_status = (U8)usbEvent;
	PSENDX(ACI,usbms_mmi_ind);
}




/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mfw_usbms_init            |
+--------------------------------------------------------------------+

  PURPOSE : Registers a callback for USB connect/disconnect events

*/
void mfw_usbms_init(void)
{
	T_USB_RETURN ret;
	TRACE_FUNCTION("mfw_usbms_init");
	ret = usb_register_notifcation( (T_USB_EVENT_CALLBACK)usbms_cb,NULL);
	switch(ret)
	{
		case USB_NOT_SUPPORTED	:
			TRACE_ERROR("USB registeration unsuccessful- Not supported");
			break;
		case USB_MEMORY_ERR:
			TRACE_ERROR("USB registeration unsuccessful - Memeory error");
			break;
		case USB_INTERNAL_ERR:
			TRACE_ERROR("USB registeration unsuccessful - Internal error");
			break;
		case USB_OK:
			TRACE_EVENT("USB registeration successful");
			break;
	}		
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mfw_usbms_enum_control            |
+--------------------------------------------------------------------+

  PURPOSE : Continues or abandons the enumeration based on user selection

*/
void mfw_usbms_enum_control(T_MFW_USB_ENUM_CONTROL usbms_enum_status, T_MFW_USB_ENUM_TYPE usb_enum_type)
{
	T_USB_ENUM_INFO  enumInfo;
	T_USB_RETURN ret;
	TRACE_FUNCTION("mfw_usbms_enum_control");
	switch(usb_enum_type)
	{
		case MFW_USB_TYPE_MS:
			enumInfo.info=USB_MS;
			break;
		case MFW_USB_TYPE_AT:
			enumInfo.info=USB_AT;
			break;
		case MFW_USB_TYPE_TRACE:
			enumInfo.info=USB_TRACE;
			break;
		case MFW_USB_TYPE_AT_MS://xashmic 27 Sep 2006, OMAPS00096389 
			enumInfo.info=USB_AT|USB_MS;
			break;
		case MFW_USB_TYPE_AT_TRACE://xashmic 27 Sep 2006, OMAPS00096389 
			enumInfo.info=USB_AT|USB_TRACE;
			break;
		case MFW_USB_TYPE_MS_TRACE://xashmic 27 Sep 2006, OMAPS00096389 
			enumInfo.info=USB_MS|USB_TRACE;
			break;
		case MFW_USB_TYPE_ALL:
			enumInfo.info=USB_MS| USB_AT|USB_TRACE;
			break;
		default:
			TRACE_ERROR("mfw_usbms_enum_control():Invalid type");
			return;
	}
	TRACE_EVENT_P1("Ports %x",enumInfo.info);
	switch(usbms_enum_status)
	{
		case MFW_USB_ENUM:
			ret=usb_enum_control(USB_ENUM,&enumInfo);
			TRACE_EVENT_P1("mfw_usbms_enum_control:ret:%d", ret);
			switch(ret)
			{
				case USB_NOT_SUPPORTED	:
					TRACE_ERROR("USB enumeration unsuccessful- Not supported");
					break;
				case USB_MEMORY_ERR:
					TRACE_ERROR("USB enumeration unsuccessful - Memeory error");
					break;
				case USB_INTERNAL_ERR:
					TRACE_ERROR("USB enumeration unsuccessful - Internal error");
					break;
				case USB_OK:
					TRACE_EVENT("USB enumeration successful");
					break;
				default:
					TRACE_ERROR("USB enumeration unsuccessful - Invalid return");
					break;
			}	
			break;
		case MFW_USB_NO_ENUM:
			ret=usb_enum_control(USB_NO_ENUM,&enumInfo);
			switch(ret)
			{
				case USB_NOT_SUPPORTED	:
					TRACE_ERROR("USB no enumeration unsuccessful- Not supported");
					break;
				case USB_MEMORY_ERR:
					TRACE_ERROR("USB no enumeration unsuccessful - Memeory error");
					break;
				case USB_INTERNAL_ERR:
					TRACE_ERROR("USB no enumeration unsuccessful - Internal error");
					break;
				case USB_OK:
					TRACE_EVENT("USB no enumeration successful");
					break;
				default:
					TRACE_ERROR("USB enumeration successful - Invalid return");
					break;
			}		
			break;
	}
}

#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mmeInit            |
+--------------------------------------------------------------------+
  PURPOSE : initialize equipment handler
*/
MfwRes mmeInit (void)
{
//	 xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
//	the callback registeration variables are populated here and passed as 
//	argument to audio_init
#ifdef FF_MMI_AUDIO_PROFILE
	T_ACCESSORY_CALLBACK audio_device_config[MAX_EXT_AUD_DEVICE];
	
	audio_device_config[CALLBACK_HEADSET].callbackFunc = mfw_audio_headSet_indication;
	audio_device_config[CALLBACK_HEADSET].callbackVal = NULL;
	audio_device_config[CALLBACK_CARKIT].callbackFunc = mfw_audio_carKit_indication;
	audio_device_config[CALLBACK_CARKIT].callbackVal = NULL;
#endif

    mfwCommand[MfwTypMme] = (MfwCb) mmeCommand;

    mfwMmeDrvFlag = 0;

    battLevel = 0;
    battState = 0;
    hdsPresence = 0;


    pwr_Init(pwrCb); //power driver
    rx_Init(rxCb);   //fieldstrength driver
    light_Init();    //light emitting driver
//  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
#ifdef FF_MMI_AUDIO_PROFILE
    audio_Init(audio_device_config);
#else
    audio_Init(hdsCb); //audio driver
#endif    

    mmeBattInfo ( 10, 0);

    mmeRxInfo(5,0);
	
    /* install prim handler     */
    pSlot = aci_create((T_PRIM_HANDLER)mmePrimHandler,NULL);
	
//xashmic 9 Sep 2006, OMAPS00092732	
//Registers a callback for event handling
#ifdef FF_MMI_USBMS
	mfw_usbms_init();

/* Mar 22, 2007  DVT: OMAPS00121916  x0039928 */
/* Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- Start */
#ifdef FF_MMI_TFLASH
	tflash_notify_register();
#endif
/* Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- End */

#endif

	return MfwResOk;
}

/* Mar 22, 2007  DVT: OMAPS00121916  x0039928 */
/* Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- Start */
#ifdef FF_MMI_TFLASH
void tflash_notify_register(void)
{
	T_RV_RET result;

	result = (T_RV_RET)mc_register_notifcation((T_MC_EVENT_CALLBACK)tflash_cb, NULL);

	if(result == RV_OK)
		TRACE_EVENT("TFLASH callback registration successful");
	else
		TRACE_ERROR("TFLASH callback registration failed");
}
#endif
/* Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- End */


// Jun 02, 2006  DRT OMAPS00079213   x0039928(sumanth)
// Fix: Two new mfw hook register and unregister functions are added
// which call the hook register and unregister functions in audio.c file
#ifdef FF_MMI_AUDIO_PROFILE
void mfw_hook_unregister(void)
{
	TRACE_FUNCTION("mfw hook unregister");
	hook_unregister();	
}

void mfw_hook_register(void)
{
	T_ACCESSORY_CALLBACK hook_device;

	TRACE_FUNCTION("mfw hook register");
	
	hook_device.callbackVal = NULL;
	hook_device.callbackFunc = mfw_audio_hook_indication;
	hook_register(hook_device);
}
#endif



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mmeExit            |
+--------------------------------------------------------------------+

  PURPOSE : finalize equipment handler

*/
MfwRes mmeExit (void)
{
    aci_delete(pSlot);                  /* remove prim handler      */
    mfwCommand[MfwTypMme] = 0;

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mmeCreate          |
+--------------------------------------------------------------------+

  PURPOSE : create equipment control

*/

MfwHnd mmeCreate (MfwHnd w, MfwEvt e, MfwCb f)
{
    MfwHdr *hdr = (MfwHdr *) mfwAlloc(sizeof(MfwHdr));
    MfwMme *mme = (MfwMme *) mfwAlloc(sizeof(MfwMme));
	MfwHdr *insert_status =0;
	
    if (!hdr || !mme)
    {
    	TRACE_ERROR("ERROR: mmeCreate() Mem Alloc Failed.");
			
	   	if(hdr)
   			mfwFree((U8*)hdr,sizeof(MfwHdr));
   		if(mme)
   			mfwFree((U8*)mme,sizeof(MfwMme));	
   		
	   	return 0;
  	}

    mme->map = e;
    mme->handler = f;
    mme->value = 0;

    hdr->data = mme;
    hdr->type = MfwTypMme;

    insert_status = mfwInsert(w,hdr);
    
    if(!insert_status)
	{
  		TRACE_ERROR("ERROR: mmeCreate() Failed to Install Handler. ");
   		mfwFree((U8*)hdr,sizeof(MfwHdr));
   		mfwFree((U8*)mme ,sizeof(MfwMme));
		return 0;
  	}
    return insert_status;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mmeDelete          |
+--------------------------------------------------------------------+

  PURPOSE : delete equipment control

*/

MfwRes mmeDelete (MfwHnd h)
{
    MfwRes res;

    if (!h)
        return MfwResIllHnd;

    res = (mfwRemove(h)) ? MfwResOk : MfwResIllHnd;

    mfwFree(((MfwHdr *) h)->data,sizeof(MfwMme));
    mfwFree(h,sizeof(MfwHdr));

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mmeHdsInit         |
+--------------------------------------------------------------------+

  PURPOSE : init the Headset detection and setup callback handler

*/

MfwRes mmeHdsInit (void (*hds_init) ())
{
    mfwMmeDrvFlag &= ~MmeFlagHdsDetect;

    return MfwResOk;/*a0393213 compiler warnings removal - 1 changed to MfwResOk*/
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mmeRtcInit         |
+--------------------------------------------------------------------+

  PURPOSE : init the RTC and setup the alarm callback handler

extern UBYTE		clk_Init						(USHORT, drv_SignalCB_Type, UBYTE*);
*/

MfwRes mmeRtcInit (USHORT drvHandle, UBYTE *info)
{
    mfwMmeDrvFlag &= ~MmeFlagRtcAlarm;

   return MfwResOk;  // RAVI /*a0393213 compiler warnings removal - 0 changed to MfwResOk*/
}


static int mme_backlightStatus[BL_LAST_OPTION];
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mme_setBacklightIdle   |
+--------------------------------------------------------------------+

  PURPOSE : called when an event may change the backlight level/status

*/
void mme_backlightEvent(int event)
{
	int light;
	if ((event >=BL_INIT) && (event <BL_LAST_OPTION))
	{
		light  = mme_backlightStatus[event];
		if (light == BL_SET_IDLE)
		{
			light_setBacklightIdle();
		}
		else if (light == BL_NO_CHANGE)
		{
			//No change
		}
		else if ((light >=BL_NO_LIGHT) && (light <=BL_MAX_LIGHT))
		{
			mmeBackLight ((U8)light);
		}	
	
	}
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mme_setBacklightIdle   |
+--------------------------------------------------------------------+

  PURPOSE : called to initialise backlight events

*/
void mme_setBacklightEvent(int event, int lightLevel)
{
	if ((event >=BL_INIT) && (event <BL_LAST_OPTION))
	{
		mme_backlightStatus[event] = lightLevel;
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mmeBackLight       |
+--------------------------------------------------------------------+

  PURPOSE : set display backlight

*/
void mmeBackLight (U8 level)
{
    light_SetStatus(LIGHT_DEVICE_BACKLIGHT,level);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mmeAudioTone       |
+--------------------------------------------------------------------+

  PURPOSE : configure the buzzer

*/

void mmeAudioTone (U8 callTone, U8 volume, U8 toneStat)
{
    if (toneStat EQ TONE_SWT_OFF)
#ifdef FF_MMI_RINGTONE /* Stop MSL Play */
        audio_StopSoundbyID(AUDIO_SPEAKER, CALLTONE_SELECT);
#else
        audio_StopSoundbyID(AUDIO_SPEAKER,callTone);
#endif
    else
#ifdef FF_MMI_RINGTONE /* Play MSL Ring tone */
        audio_PlaySoundID(AUDIO_SPEAKER,CALLTONE_SELECT,volume,toneStat);
#else
        audio_PlaySoundID(AUDIO_SPEAKER,callTone,volume,toneStat);
#endif
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mmeSetVolume       |
+--------------------------------------------------------------------+

  PURPOSE : configure mic and speaker volume

*/

MfwRes mmeSetVolume (U16 audioIn, U16 audioOut)
{
    if (audioIn > 255) audioIn = 255;
    if (audioOut > 255) audioOut = 255;

	audio_SetAmplf(AUDIO_MICROPHONE,(U8) audioIn);
    audio_SetAmplf(AUDIO_SPEAKER,(U8) audioOut);

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mmeRxInfo          |
+--------------------------------------------------------------------+

  PURPOSE : configure signal quality information:
            request from the PS an indication on change of the
            rx level with n intervalls.
            Parameters mode and level are not used in the moment.

*/

void mmeRxInfo (U8 intervalls, U16 level)
{
    rx_DCB_Type rx_DCB;

    rx_DCB.Steps = intervalls;
    rx_SetConfig(&rx_DCB);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mmeBattInfo        |
+--------------------------------------------------------------------+

  PURPOSE : configure battery level information
            

*/

void mmeBattInfo ( U8 intervalls, U16 level)
{
    pwr_DCB_Type pwr_DCB;

    if (level > 255) level = 255;

    pwr_DCB.RangeMin = (U8) level;      /* low level threshold      */
    pwr_DCB.RangeMax = 100;             /* 100 Percent              */
    pwr_DCB.Steps = intervalls;         /* n Steps                  */
    pwr_SetConfig(&pwr_DCB);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : sigExec            |
+--------------------------------------------------------------------+

  PURPOSE : execute equipment signal

*/

static int sigExec (MfwHdr *curElem, U32 evt, U32 val)
{
    MfwMme *ec;

    while (curElem)
    {
        if (curElem->type == MfwTypMme)
        {
            ec = curElem->data;
            if (ec->map & evt)
            {                           /* events match             */
                ec->value = val;
                if (ec->handler)        /* handler valid            */
                {
                  // PATCH LE 06.06.00
                  // store current mfw elem
                  current_mfw_elem = curElem;
                  // END PATCH LE 06.06.00
                  if ((*(ec->handler))(evt,ec))
                    return 1;       /* event consumed           */
                }
            }
        }
        curElem = curElem->next;
    }

    return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mmeSignal          |
+--------------------------------------------------------------------+

  PURPOSE : equipment event (called by driver/PEI)

*/

void mmeSignal (U32 evt, U32 value)
{
UBYTE temp=0;
#ifndef FF_POWER_MANAGEMENT
  temp = dspl_Enable(0);
#else
  /*OMAPS00098881 (removing power variant) a0393213(Prabakar)
  display is enabled only when lcd refresh is needed*/
  if(FFS_flashData.refresh == 1)
	temp = dspl_Enable(0);
#endif
if (mfwSignallingMethod EQ 0)
  {
    if (mfwFocus)
        if (sigExec(mfwFocus,evt,value))
		{
		#ifndef FF_POWER_MANAGEMENT
			dspl_Enable(temp);            
		#else
		       /*OMAPS00098881 (removing power variant) a0393213(Prabakar)
                   display is enabled only when lcd refresh is needed*/
		      if(FFS_flashData.refresh == 1)
				dspl_Enable(temp);
		#endif
			return;
		}
    if (mfwRoot)
        sigExec(mfwRoot,evt,value);
  }
  else
    {
      MfwHdr * h = 0;

      /*
       * Focus set, then start here
       */
      if (mfwFocus)
        h = mfwFocus;
      /*
       * Focus not set, then start root
       */
      if (!h)
        h = mfwRoot;

      /*
       * No elements available, return
       */

      while (h)


      {
        /*
         * Signal consumed, then return
         */
        if (sigExec (h, evt, value))
		{
		#ifndef FF_POWER_MANAGEMENT
			dspl_Enable(temp);            
		#else
		       /*OMAPS00098881 (removing power variant) a0393213(Prabakar)
                   display is enabled only when lcd refresh is needed*/
		      if(FFS_flashData.refresh == 1)
				dspl_Enable(temp);
		#endif
			return;
		}

        /*
         * All windows tried inclusive root
         */
        if (h == mfwRoot)
		{
		#ifndef FF_POWER_MANAGEMENT
			dspl_Enable(temp);            
		#else
		       /*OMAPS00098881 (removing power variant) a0393213(Prabakar)
                   display is enabled only when lcd refresh is needed*/
		      if(FFS_flashData.refresh == 1)
				dspl_Enable(temp);
		#endif
			return;
		}

        /*
         * get parent window
         */
        h = mfwParent(mfwParent(h));
		if(h)
			h = ((MfwWin * )(h->data))->elems;
      }
      sigExec (mfwRoot, evt, value);

  }
#ifndef FF_POWER_MANAGEMENT
  dspl_Enable(temp);            
#else
  /*OMAPS00098881 (removing power variant) a0393213(Prabakar)
     display is enabled only when lcd refresh is needed*/
  if(FFS_flashData.refresh == 1)
  dspl_Enable(temp);
#endif
return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mmeCommand         |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

static int mmeCommand (U32 cmd, void *h)
{
    switch (cmd)
    {
        case MfwCmdDelete:              /* delete me                */
            if (!h)
                return 0;
            mmeDelete(h);
            return 1;
        default:
            break;
    }

    return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : rAT_PercentSQ      |
+--------------------------------------------------------------------+

  PURPOSE : called by ACI on change of signal quality

*/

void rAT_PercentSQ (BYTE val)
{
    TRACE_FUNCTION("rAT_PercentSQ()");

//ES!! no longer needed    mmeSignal(MfwMmeSignal,val);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : rAT_PercentBC      |
+--------------------------------------------------------------------+

  PURPOSE : called by ACI on change of battery status

*/

void rAT_PercentBC (BYTE val)
{
    TRACE_FUNCTION("rAT_PercentBC()");

//ES!! no longer needed    mmeSignal(MfwMmeBattery,val);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mmePrimHandler     |
+--------------------------------------------------------------------+

  PURPOSE : MME primitive handler

*/

static BOOL mmePrimHandler (USHORT opc, void *data)
{
    U8 level;
    U16 vlevel;
    U8 state;
//  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
#ifdef FF_MMI_AUDIO_PROFILE
    U8 hdsData;
    U8 cktData;
#endif
	

//    TRACE_FUNCTION ("mmePrimHandler()");
    


    switch (opc)
    {
        case MMI_RXLEV_IND:
            level = ((T_MMI_RXLEV_IND *) data)->rxlev;
            mmeSignal(MfwMmeSignal,level);
            return TRUE;                /* processed by MME, delete */
        case MMI_BATTERY_IND:
	    vlevel = lcc_get_batteryVotage();
   	    if (vlevel  <  3500)
    		level = 0;
  	    else if ( (vlevel  < 3650) AND (vlevel  >=  3500) )
    		level = 1;
  	    else if ( ( vlevel  < 3800) AND (vlevel  >= 3650) )
    		level = 2;
            else if ( ( vlevel  < 3950) AND (vlevel  >= 3800) )
    		level = 3;
  	    else
    		level = 4;
            state = lcc_get_state();//((T_MMI_BATTERY_IND *) data)->temp;
	    emo_printf("mmePrimHandler() Battery_ind volts: %d - state: %d", level, state);
            if (battLevel != level)
            {
                battLevel = level;
                mmeSignal(MfwMmeBattery,level);
            }
            if (battState != state)
            {
                battState = state;
                mmeSignal(MfwMmeBaState,state);
            }
            return TRUE;                /* processed by MME, delete */
//  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005			
//	the corresponding callback function is called in MMI context
//	this comment will be removed once the sap for this is available
#ifdef FF_MMI_AUDIO_PROFILE
       case MMI_HEADSET_IND:
	   	TRACE_EVENT("MMI_HEADSET_IND");
           hdsData = ((T_MMI_HEADSET_IND *) data)->headset_status;
           mmeSignal(MfwMmeHeadset,hdsData);
           return TRUE;                
       case MMI_CARKIT_IND:
	   	TRACE_EVENT("MMI_CARKIT_IND");
           cktData = ((T_MMI_CARKIT_IND *) data)->carkit_status;
	    mmeSignal(MfwMmeCarkit, cktData);
           return TRUE;  
#endif
//xashmic 9 Sep 2006, OMAPS00092732	
//Handler for USB connect or disconnect events
#ifdef FF_MMI_USBMS
	case USBMS_MMI_IND:
	{
		T_USBMS_MMI_IND *usbms_mmi_ind=(T_USBMS_MMI_IND *) data;		
		switch(usbms_mmi_ind->usb_status)
		{
			case USB_CABLE_CONNECT_EVENT:
				TRACE_EVENT("USB_CABLE_CONNECT_EVENT");
				//xashmic 27 Sep 2006, OMAPS00096389 
				//xashmic 21 Sep 2006, OMAPS00095831
				//To enumerate based on user selection
				if( ( getBootUpState() > 0 ) && ( getUSBMSFlags() & MFW_USBMS_POPUP_ENABLE))
				{
					mmeSignal(MfwMmeUSBMS , MFW_USB_CABLE_CONNECT_EVENT);
				}
				//xashmic 21 Sep 2006, OMAPS00095831
				//To Enumerate automatically
				else
				{
//x0pleela 01 Apr, 2007  ER: OMAPS00122561
				#ifdef FF_PHONE_LOCK
					//x0pleela 29 May, 2007  DR: OMAPS00132483
					//Check the phone lock status
					if( mfw_get_Phlock_status() )
						mmeSignal(MfwMmeUSBMS , MFW_USB_CABLE_CONNECT_EVENT);
					else
				#endif //FF_PHONE_LOCK
					mfw_usbms_enum_control(MFW_USB_ENUM , MFW_USB_TYPE_ALL);
				}
				break;
			case USB_CABLE_DISCONNECT_EVENT:
				TRACE_EVENT("USB_CABLE_DISCONNECT_EVENT");
				mmeSignal(MfwMmeUSBMS , MFW_USB_CABLE_DISCONNECT_EVENT);
				mfw_usbms_enum_control(MFW_USB_NO_ENUM , MFW_USB_TYPE_ALL);
				break;
		}  		
		PFREE(usbms_mmi_ind);
	   	return TRUE;
	}

/* Mar 22, 2007  DVT: OMAPS00121916  x0039928 */
/* Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- Start */
#ifdef FF_MMI_TFLASH
	case TFLASH_MMI_IND:
	{
		T_TFLASH_MMI_IND *tflash_mmi_ind=(T_TFLASH_MMI_IND *) data;
		switch(tflash_mmi_ind->mc_status)
		{
			case MC_INSERT_EVENT:
				TRACE_EVENT("TFLASH card inserted");
				mmeSignal(MfwMmeUSBMS , MFW_TFLASH_INSERTED_EVENT);
				break;
			case MC_REMOVE_EVENT:
				TRACE_EVENT("TFLASH card removed");
				mmeSignal(MfwMmeUSBMS , MFW_TFLASH_REMOVED_EVENT);
				break;
		}
		PFREE(tflash_mmi_ind);
		return TRUE;
	}
#endif	
/* Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- End */

#endif
    }

    return FALSE;                       /* not processed, forward   */
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mmeFlagHandler     |
+--------------------------------------------------------------------+

  PURPOSE : MME driver flag handler

*/

void mmeFlagHandler (void)
{
    TRACE_FUNCTION ("mmeFlagHandler()");

    while (mfwMmeDrvFlag)
    {
        if (mfwMmeDrvFlag & MmeFlagRtcAlarm)
        {
            mfwMmeDrvFlag &= ~MmeFlagRtcAlarm;
            mmeSignal(MfwMmeRtcAlarm,0);
        }
        else if (mfwMmeDrvFlag & MmeFlagHdsDetect)
        {
            mfwMmeDrvFlag &= ~MmeFlagHdsDetect;
            mmeSignal(MfwMmeHdsDetect,hdsPresence);
        }
        else if (mfwMmeDrvFlag & MmeFlagIrdaMsg)
		{
		}
        else
        {
            mfwMmeDrvFlag = 0;
        }
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : pwr_callback       |
+--------------------------------------------------------------------+

  PURPOSE : Called by battery driver

*/

static void pwrCb (drv_SignalID_Type *signal_params)
{
    pwr_Status_Type *para;
    PALLOC(battery_ind, MMI_BATTERY_IND);

    para = (pwr_Status_Type *) signal_params->UserData;
    battery_ind->volt = para->BatteryLevel;
    battery_ind->temp = para->Status;

    PSENDX(ACI,battery_ind);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : rx_callback        |
+--------------------------------------------------------------------+

  PURPOSE : Called by fieldstrength driver

*/

static void rxCb (drv_SignalID_Type *signal_params)
{
    rx_Status_Type *para;
    PALLOC(rxlevel_ind,MMI_RXLEV_IND);

    para = (rx_Status_Type *) signal_params->UserData;
    rxlevel_ind->rxlev = para->actLevel;

    PSENDX(ACI,rxlevel_ind);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : rtcCb              |
+--------------------------------------------------------------------+

  PURPOSE : alarm callback from RTC

*/
/*a0393213(R.Prabakar) lint warnings removal
   warning:symbol(rtcCb) not referenced
   soln     :this function was not at all called. so this function has been put under MMI_LINT_WARNING_REMOVAL,
               which is not defined. If this function is needed this flag can be removed*/
#ifdef MMI_LINT_WARNING_REMOVAL
static void rtcCb (drv_SignalID_Type *signal_params)
{
    mfwMmeDrvFlag |= MmeFlagRtcAlarm;
    vsi_c_awake(VSI_CALLER_SINGLE);
}
#endif



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : hdsCb              |
+--------------------------------------------------------------------+

  PURPOSE : callback from headset detection

*/
#ifndef FF_MMI_AUDIO_PROFILE /*a0393213 warnings removal*/
static void hdsCb (drv_SignalID_Type *signal_params)
{
	if (signal_params->SignalType == 1)
	{
		hdsPresence = (U16)signal_params->UserData;
	    mfwMmeDrvFlag |= MmeFlagHdsDetect;
		vsi_c_awake(VSI_CALLER_SINGLE);
	}
}
#endif

#ifdef FF_MMI_AUDIO_PROFILE
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mfw_set_stereo_path              |
+--------------------------------------------------------------------+

  PURPOSE : This sets the stereo path for the current audio profile

*/

void mfw_set_stereo_path(int device)
{
	TRACE_FUNCTION("mfw_set_stereo_path()");

	TRACE_EVENT_P1("Device === %d", device);
	 switch(device)
	 {
	  case MFW_AUD_HEADSET:
	  	audio_set_path(AUDIO_STEREO_SPEAKER_HEADPHONE, TRUE);
	   	break;
	  case MFW_AUD_HANDHELD:
	   	audio_set_path(AUDIO_STEREO_SPEAKER_HANDHELD, TRUE);
	   	break;
	  case MFW_AUD_CARKIT:
	   	audio_set_path(AUDIO_STEREO_SPEAKER_CARKIT, TRUE);
		break;
	  case MFW_AUD_LOUDSPEAKER:
		audio_set_path(AUDIO_STEREO_SPEAKER_HANDFREE, TRUE);
	   	break;
	  }
}
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mfw_unset_stereo_path              |
+--------------------------------------------------------------------+

  PURPOSE : This resets the voice path for the current audio profile

*/
void mfw_unset_stereo_path(int device)
{
	TRACE_FUNCTION("mfw_unset_stereo_path()");

	TRACE_EVENT_P1("Device === %d", device);
	switch(device)
	 {
	  case MFW_AUD_HEADSET:
	   	audio_set_path(AUDIO_SPEAKER_HEADSET, FALSE);
	   	break;
	  case MFW_AUD_HANDHELD:
	   	audio_set_path(AUDIO_SPEAKER_HANDHELD, FALSE);
	   	break;
	  case MFW_AUD_CARKIT:
	   	audio_set_path(AUDIO_SPEAKER_CARKIT, FALSE);
		break;
	  case MFW_AUD_LOUDSPEAKER:
		audio_set_path(AUDIO_SPEAKER_HANDFREE, FALSE);
	   	break;
	  }
}

// Jun 02, 2006  DRT OMAPS00079213   x0039928(sumanth)
// Fix: Callback function called when mfw receives hook detect event from BSP
void mfw_audio_hook_indication ( T_AUDIO_ACCESSORY_NOTIFICATION notify, 
ACCESSORY_CALLBACK_PARAM callbackparam )
{

	PALLOC(hdsData,MMI_HEADSET_IND);
	hdsData->headset_status = (U8)notify;
	PSENDX(ACI,hdsData);

}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mfw_audio_headSet_indication              |
+--------------------------------------------------------------------+

  PURPOSE :Callback function for the events related to headset  and create and post a primitive

*/
void mfw_audio_headSet_indication ( T_AUDIO_ACCESSORY_NOTIFICATION notify, 
ACCESSORY_CALLBACK_PARAM callbackparam )
{

	PALLOC(hdsData,MMI_HEADSET_IND);
	hdsData->headset_status = (U8)notify;
	PSENDX(ACI,hdsData);

}
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mfw_audio_headSet_indication              |
+--------------------------------------------------------------------+

  PURPOSE :Callback function for the events related to carkit and create and post a primitive

*/
void mfw_audio_carKit_indication ( T_AUDIO_ACCESSORY_NOTIFICATION notify, 
	ACCESSORY_CALLBACK_PARAM callback_param )
{
	PALLOC(cktData,MMI_CARKIT_IND);
	cktData->carkit_status = (U8)notify;
	PSENDX(ACI,cktData);
}
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mfw_audio_headSet_indication              |
+--------------------------------------------------------------------+

  PURPOSE :handles the request to switch to the respective device from the 
		    Bmi when the indications of insert and removal are received

*/
void mfw_audio_device_indication( U8 notify)
{	
	TRACE_FUNCTION("mfw_audio_device_indication()");

	TRACE_EVENT_P1(" Indication Type == %d",notify);
	switch(notify)
	{	
		case AUDIO_HEADSET_PLUGGED:
			if(currAudioDevice != MFW_AUD_HEADSET)
			{
				mfw_audio_set_device(MFW_AUD_HEADSET);
					headsetInserted = TRUE;
			}
			break;
		case AUDIO_HEADSET_UNPLUGGED:
			if( currAudioDevice == MFW_AUD_HEADSET)
			{
				mfw_audio_set_device(MFW_AUD_HANDHELD );
	
			}
			headsetInserted = FALSE;
			break;
		case AUDIO_HEADSET_HOOK_DETECT:
				break;
		case AUDIO_CARKIT_PLUGGED:
			if(currAudioDevice != MFW_AUD_CARKIT)
			{
				mfw_audio_set_device(MFW_AUD_CARKIT);
				carkitInserted = TRUE;
			}
			break;
		case AUDIO_CARKIT_UNPLUGGED:
			if( currAudioDevice == MFW_AUD_CARKIT)
			{
				if(headsetInserted == TRUE)
				{
					mfw_audio_set_device (MFW_AUD_HEADSET);
				}
				else
				{
					mfw_audio_set_device (MFW_AUD_HANDHELD);
				}
			}
			carkitInserted = FALSE;
			break;
		default:
			break;
	}
}
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MME            |
| STATE   : code                        ROUTINE : mfw_audio_headSet_indication              |
+--------------------------------------------------------------------+

  PURPOSE :This function handles the switch between the handheld and loudspeaker. 
		    The type of device to be loaded will be posted from the menu selection
		    When the profile changes if there is any other media played then the 
		    speaker should be set to stereo speaker

*/
void mfw_audio_set_device(int type)
{
/*Description: this array holds the file names on the ffs in which the audio profiles are stored.
The file names are yet to be confirmed from the audio services group*/
	//char aud_cg_file[4][15] = {"handheld","loudspeaker","headset","carkit"};
	TRACE_FUNCTION("mfw_audio_set_device()");

	TRACE_EVENT_P1("type of device = %d",type);
	switch(type)
	{
		case MFW_AUD_HANDHELD:
			audio_set_device((char*)HANDHELD);// (aud_cg_file[HANDHELD] );
			currAudioDevice = MFW_AUD_HANDHELD;
			break;
		case MFW_AUD_LOUDSPEAKER:
			audio_set_device((char*) LOUDSPEAKER);//(aud_cg_file[LOUDSPEAKER]);
			currAudioDevice = MFW_AUD_LOUDSPEAKER;
			break;
		case MFW_AUD_HEADSET:
			audio_set_device((char*)HEADSET);//(aud_cg_file[HEADSET]);
			currAudioDevice = MFW_AUD_HEADSET;
			break;
		case MFW_AUD_CARKIT:
			audio_set_device((char*)CARKIT);//(aud_cg_file[CARKIT]);
			currAudioDevice = MFW_AUD_CARKIT;
		default:
			break;
	}
	if(TRUE == mfwAudPlay)
  		mfw_set_stereo_path(currAudioDevice);

}

#ifdef ISAMPLE
/* OMAPS00057367, 3 Dec 2005, nekkareb : start */
/* Function mfw_get_current_audioDevice modified to read the
     current audio device using the audio service entity function
     audio_full_access_read. This solves some synchronisation
     problems b/n Audio & MMI for the DR OMAPS00057367 */
/*returns the current active audio device*/     
int mfw_get_current_audioDevice()
{
	T_AUDIO_FULL_ACCESS_READ read;
	INT8 speaker;

	read.variable_indentifier = AUDIO_SPEAKER_MODE;
	read.data = &speaker;
	
	TRACE_FUNCTION("mfw_get_current_audioDevice()");
	audio_full_access_read(&read );

	switch(speaker)
	{
	case AUDIO_SPEAKER_NONE:
		TRACE_EVENT("mfw_get_current_audioDevice() - SPEAKER PATH NOT CONFIGURED");
		currAudioDevice = MFW_AUD_HANDHELD; //Need to check the mapping
		break;
	case AUDIO_SPEAKER_HANDHELD:
		currAudioDevice = MFW_AUD_HANDHELD;
		break;
	case AUDIO_SPEAKER_HEADSET:
		currAudioDevice = MFW_AUD_HEADSET;
		break;
	case AUDIO_SPEAKER_AUX:
		currAudioDevice = MFW_AUD_LOUDSPEAKER; //Need to check the mapping
		break;
	case AUDIO_SPEAKER_CARKIT:
		currAudioDevice = MFW_AUD_CARKIT;
		break;
	case AUDIO_SPEAKER_HANDFREE:
	case AUDIO_SPEAKER_HANDFREE_CLASS_D:
		currAudioDevice = MFW_AUD_LOUDSPEAKER;
		break;
	case AUDIO_SPEAKER_INVERTED_VOICE:
		currAudioDevice = MFW_AUD_HEADSET; //Need to check the mapping
		break;
	}

	return currAudioDevice;
}
/* OMAPS00057367, 3 Dec 2005, nekkareb : end */
#else
/*returns the current active audio device*/
int mfw_get_current_audioDevice()
{
	TRACE_FUNCTION("mfw_get_current_audioDevice()");
	return currAudioDevice;
}
#endif //ISAMPLE
#endif


