/*******************************************************************************\
*                                                                       		*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      fmradio_app.c
*
*   DESCRIPTION:	This file contains the implementation of the FM Radio Primitive Handling - USER ACTIONS*
*   
*	AUTHOR:         singh,Vikas
*
\*******************************************************************************/

/*Includes */

/* FM includes */

#include "EBTIPS_version.h"
#include "btl_config.h"
#include "osapi.h"

#if 0

#include "fms_api.h"
#include "fmradio_app.h"

/*Application includes*/
#include "lineParser.h"


#define VOLUMN_MIN 0
#define VOLUMN_MAX 61808
#define VOLUMN_INIT 30904

#define GAIN_STEP	5000

#define FMAPP_INVALID_PARAM     (0xff)

/* Global Definitions */
void APP_FMRADIO_OnOff(void);
void APP_FMRADIO_SetBand(void);
void APP_FMRADIO_Tune(void);
void APP_FMRADIO_MuteRadio(void);
void APP_FMRADIO_RfDependentMute(void);
void APP_FMRADIO_RDS_OnOff(void);
void APP_FMRADIO_AF_OnOff(void);
void APP_FMRADIO_FmSeek(void);
void APP_FMRADIO_SetVolume(void);
void APP_FMRADIO_SetRssiSeachLevel(void);
void APP_FMRADIO_GetRssiLevel(void);
void APP_FMRADIO_GetPi(void);
void APP_FMRADIO_SetMoStMode(void);
void APP_FMRADIO_SetRxAudioPath(void);
void FMRadioAppCallback(const FmEvent *event);


static void FmAppCmdDoneHandler(U8 event, FmStatus status, U32 value);
static void FmAppRadioTunedHandler(U8 status, U32 frequency);
static void FmAppPSChangedHandler(U32 freq, U8 *data);
static void FmAppAfListChangedHandler(U16 pi, U8 afListSize, U32 *afList);
static void FmAppAfJumpHandler(U8 status, U16 Pi, U32 oldFreq, U32 newFreq);
static void FmAppRadioTextHandler(BOOL changed, U8 length, U8 *radioText);
static void FmAppMoStModeChangedHandler(U8 mode);
static void FmAppRawRdsHandler(U16 len, U8 *data); 

static void getNewGain(BOOL dir);

#if XA_DEBUG == XA_ENABLED
static char *getEventString(U8 event);
static char *getStatusString(U8 status);
#endif

#define FMRADIO_COMMANDS_NUM            14
#define FM_COMMAND_MAX_STR_LEN        20    


BOOL isFmRadioOn = FALSE;  /* Is FM already Initialized */
static BOOL fmOsInitialized = FALSE; /* Is FM_OsInit() called atleast once */
static BOOL isAfOn = FALSE; /* Is Alternative Frequency function switched on */
static U32 volumeGain = 30904; 
static FmContext *fmcontext;

typedef void (*FuncType)(void);

typedef struct
{
	char 		funcName[LINE_PARSER_MAX_STR_LEN];
	FuncType    funcPtr;
} _fmCommands;

/* FM Radio commands array */
static const  _fmCommands fmCommands[] = {{"fmonoff",	    APP_FMRADIO_OnOff}, 		       
										    {"setband",      APP_FMRADIO_SetBand}, 			
										    {"tune",      APP_FMRADIO_Tune},		
										    {"setmost",      APP_FMRADIO_SetMoStMode},
										    {"mute",      APP_FMRADIO_MuteRadio},
										    {"rfmute",      APP_FMRADIO_RfDependentMute},
										    {"rdsonoff",      APP_FMRADIO_RDS_OnOff},
										    {"afonoff",      APP_FMRADIO_AF_OnOff},
										    {"seek",      APP_FMRADIO_FmSeek},
										    {"setvolume",	    APP_FMRADIO_SetVolume},
										    {"setrssi",	    APP_FMRADIO_SetRssiSeachLevel},
										    {"getrssi",      APP_FMRADIO_GetRssiLevel},
										    {"getpi",      APP_FMRADIO_GetPi},
										    {"analogaudio",      APP_FMRADIO_SetRxAudioPath}										    
										   };



/*******************************************************************************
*  FUNCTION:	FMRADIOA_UI_ProcessUserAction
*                                                                         
*  PARAMETERS:	msg - Parameters of actions.
*                                                                         
*  DESCRIPTION:	Processes primitives for the FM Radio application
*                                                                         
*  RETURNS:		None.
*******************************************************************************/
void FMRADIOA_UI_ProcessUserAction(U8 * msg)

{
	U8 i;
	char command[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS lineStatus;
	
	Report(("FMRADIOA_UI_ProcessUserAction"));

	lineStatus = LINE_PARSER_GetNextStr((U8 *)(command), LINE_PARSER_MAX_STR_LEN);

	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("Wrong FM Radio command. Can't parse line"));
		return;
	}

	/* Look for the correct function */
	for(i = 0; i < FMRADIO_COMMANDS_NUM; i++)
	{
		if (OS_StrCmp((const char *)command, fmCommands[i].funcName) == 0)
		{ 
			/*FM Radio command maches; Call the function to handle the command*/
			Report(("Starting FM Radio command '%s'", command));
			fmCommands[i].funcPtr();
			return;
		}
	}
	Report(("Wrong FM Radio command"));
}

/*******************************************************************************
*  FUNCTION:	APP_FMRADIO_OnOff
*                                                                         
*  PARAMETERS:	None
*                                                                         
*  DESCRIPTION:	Power's ON/OFF the FM Radio
*                                                                         
*  RETURNS:		
*******************************************************************************/
void APP_FMRADIO_OnOff()
{
       FmStatus status;
	U8 commandBuff[FM_COMMAND_MAX_STR_LEN];	

	(void) LINE_PARSER_GetNextStr(commandBuff,3);

	if(OS_StrCmp((const char *)commandBuff, "ON") == 0)
	{
//	  if(!fmOsInitialized)
//	  	{
			/* Initialize the FM OS module */	
//			Assert (BTHAL_STATUS_FAILED != FM_OsInit());
//			fmOsInitialized = TRUE;
//	  	}
		if(!isFmRadioOn)
		{
			status = FM_Enable(fmcontext); /* FM_Enable() should be used but 
													* this API is used to becasue ti_chip_mngr 
													* is not integrated yet */			
			Report(("FMRADIO :FM_SetPower() returned %d",status));
		}
	}
			
	else if (OS_StrCmp((const char *)commandBuff, "OFF") == 0)
	{
		if (isFmRadioOn)
		{
			status = FM_Disable(fmcontext); /*FM_Disable() should be used */
			Report(("FMRADIO :FM_Disable() returned %d",status));
		}		
	}

	else 
		Report(("FMRADIO: Invalid command"));
}
	
/*******************************************************************************
*  FUNCTION:	APP_FMRADIO_SetBand
*                                                                         
*  PARAMETERS:	None
*                                                                         
*  DESCRIPTION:	Sets the FM Band - JAPAN or EUROPE
*                                                                         
*  RETURNS:		
*******************************************************************************/
void APP_FMRADIO_SetBand()
{
    FmStatus status;
    U8 bandParam = FMAPP_INVALID_PARAM;
    U8 band[FM_COMMAND_MAX_STR_LEN];

    (void) LINE_PARSER_GetNextStr(band, 5);  /* Band : US or JAPAN */

    if(OS_StrCmp((const char *)band, "US") == 0)
    {
        bandParam = FM_EUROPE_US_BAND;
    }
    else if (OS_StrCmp((const char *)band, "JAPAN") == 0)
    {
        bandParam = FM_JAPAN_BAND;
    }
    
    if (FMAPP_INVALID_PARAM == bandParam)
    {
        Report(("FMRADIO: Invalid Band"));
    }
    else
    {
        status = FM_SetBand(fmcontext, bandParam);
        
        Report(("FM_SetBand() returned status = %s.", getStatusString(status)));
    }
}

/*******************************************************************************
*  FUNCTION:	APP_FMRADIO_SetBand
*                                                                         
*  PARAMETERS:	None
*                                                                         
*  DESCRIPTION:	Tune the FM to a Frequency 
*                                                                         
*  RETURNS:		
*******************************************************************************/
void APP_FMRADIO_Tune()
{
	FmStatus status;
	U32 frequency;
	
	(void) LINE_PARSER_GetNextU32(& frequency, FALSE);

	if (status = FM_Tune(fmcontext, frequency) != FM_STATUS_SUCCESS)
	{
		Report(("FMRADIO: APP_FMRADIO_Tune returned %d", status));
	}
}

/*******************************************************************************
*  FUNCTION:	APP_FMRADIO_SetMoStMode
*                                                                         
*  PARAMETERS:	None
*                                                                         
*  DESCRIPTION:	Sets to Mono or Stereo Mode
*                                                                         
*  RETURNS:		
*******************************************************************************/
void APP_FMRADIO_SetMoStMode()
{
    FmStatus status;
    U8 mostParam = FMAPP_INVALID_PARAM;
    U8 mostMode[FM_COMMAND_MAX_STR_LEN];

    (void) LINE_PARSER_GetNextStr(mostMode, 6);

    if(OS_StrCmp((const char *)mostMode, "MONO") == 0)
    {
        mostParam = FM_MONO_MODE;
    }
    else if (OS_StrCmp((const char *)mostMode, "STEREO") == 0)
    {
        mostParam = FM_STEREO_MODE;
    }
    
    if (FMAPP_INVALID_PARAM == mostParam)
    {
        Report(("FMRADIO: Invalid mode"));
    }
    else
    {
        status = FM_SetMoSt(fmcontext, mostParam);
        
        Report(("FM_SetMoSt() returned status = %s.", getStatusString(status)));
    }
}

/*******************************************************************************
*  FUNCTION:	APP_FMRADIO_MuteRadio
*                                                                         
*  PARAMETERS:	None
*                                                                         
*  DESCRIPTION: Mutes/UnMutes the Voice or Reduce the Voice Level
*                                                                         
*  RETURNS:		
*******************************************************************************/
void APP_FMRADIO_MuteRadio()
{
    FmStatus status;
    U8 muteParam = FMAPP_INVALID_PARAM;
    U8 voiceParam[FM_COMMAND_MAX_STR_LEN];

    (void) LINE_PARSER_GetNextStr(voiceParam, 6);

    if(OS_StrCmp((const char *)voiceParam, "MUTE") == 0)
    {
        muteParam = FM_MUTE_VOICE;
    }
    else if (OS_StrCmp((const char *)voiceParam, "UNMUTE") == 0)
    {
        muteParam = FM_UNMUTE_VOICE;
    }
    else if (OS_StrCmp((const char *)voiceParam, "REDUCE") == 0)
    {
        muteParam = FM_REDUCE_VOICE;
    }

    if (FMAPP_INVALID_PARAM == muteParam)
    {
        Report(("FMRADIO: Invalid command"));
    }
    else
    {
        status = FM_MuteRadio(fmcontext, muteParam);
        
        Report(("FM_MuteRadio() returned status = %s.", getStatusString(status)));
    }
}
	
/*******************************************************************************
*  FUNCTION:	APP_FMRADIO_RfDependentMute
*                                                                         
*  PARAMETERS:	None
*                                                                         
*  DESCRIPTION: Turns on/off RF dependent mute function
*                                                                         
*  RETURNS:		
*******************************************************************************/
void APP_FMRADIO_RfDependentMute()
{
    FmStatus status;
    U8 rfMuteParam = FMAPP_INVALID_PARAM;
    U8 voiceParam[FM_COMMAND_MAX_STR_LEN];

    (void) LINE_PARSER_GetNextStr(voiceParam, 6);

    if(OS_StrCmp((const char *)voiceParam, "ON") == 0)
    {
        rfMuteParam = FM_RF_MUTE_ON;
    }
    else if (OS_StrCmp((const char *)voiceParam, "OFF") == 0)
    {
        rfMuteParam = FM_RF_MUTE_OFF;
    }

    if (FMAPP_INVALID_PARAM == rfMuteParam)
    {
        Report(("FMRADIO: Invalid command"));
    }
    else
    {
        status = FM_RfDependentMute(fmcontext, rfMuteParam);
        
        Report(("FM_RfDependentMute() returned status = %s.", getStatusString(status)));
    }
}

/*******************************************************************************
*  FUNCTION:	APP_FMRADIO_RDS_OnOff
*                                                                         
*  PARAMETERS:	None
*                                                                         
*  DESCRIPTION:	Switch On/Off the RDS 
*                                                                         
*  RETURNS:		
*******************************************************************************/
void APP_FMRADIO_RDS_OnOff()
{
    FmStatus status;
    U8 rdsParam = FMAPP_INVALID_PARAM;
    U8 rdsState[FM_COMMAND_MAX_STR_LEN];

    (void) LINE_PARSER_GetNextStr(rdsState, 3);

    if (OS_StrCmp((const char *)rdsState, "ON") == 0)
    {
        rdsParam = FM_RDS_ON;
    }
    else if (OS_StrCmp((const char *)rdsState, "OFF") == 0)
    {
        rdsParam = FM_RDS_OFF;

    }
    
    if (FMAPP_INVALID_PARAM == rdsParam)
    {
        Report(("FMRADIO: Invalid command"));
    }
    else
    {
        if ((FM_RDS_OFF == rdsParam) && (TRUE == isAfOn))
        {
            /*Disable AF too */
            Report(("Disable AF too"));
            isAfOn = FALSE;
            status = FM_SetAF(fmcontext, FM_AF_OFF);
            
            Report(("FM_SetAF() returned status = %s.", getStatusString(status)));
        }
        
        status = FM_SetRDS(fmcontext, rdsParam);
        
        Report(("FM_SetRDS() returned status = %s.", getStatusString(status)));
    }
}

/*******************************************************************************
*  FUNCTION:	APP_FMRADIO_AF_OnOff
*                                                                         
*  PARAMETERS:	Set Altervative Frequencies feature on/off
*                                                                         
*  DESCRIPTION:	Switch On/Off the RDS 
*                                                                         
*  RETURNS:		
*******************************************************************************/
void APP_FMRADIO_AF_OnOff()
{
    FmStatus status;
    U8 afParam = FMAPP_INVALID_PARAM;
    U8 afState[FM_COMMAND_MAX_STR_LEN];

    (void) LINE_PARSER_GetNextStr(afState,3);

    if(OS_StrCmp((const char *)afState, "ON") == 0)
    {
        afParam = FM_AF_ON;
        isAfOn = TRUE;
    }
    else if (OS_StrCmp((const char *)afState, "OFF") == 0)
    {
        afParam = FM_AF_OFF;
        isAfOn = FALSE;
    }
    
    if (FMAPP_INVALID_PARAM == afParam)
    {
        Report(("FMRADIO: Invalid command"));
    }
    else
    {
        status = FM_SetAF(fmcontext, afParam);
        
        Report(("FM_SetAF() returned status = %s.", getStatusString(status)));
    }
}



/*******************************************************************************
*  FUNCTION:	APP_FMRADIO_FmSeek
*                                                                         
*  PARAMETERS:	Seek to FM Frequencies
*                                                                         
*  DESCRIPTION:	Switch On/Off the RDS 
*                                                                         
*  RETURNS:		
*******************************************************************************/

void APP_FMRADIO_FmSeek()
{
    FmStatus status;
    U8 seekFunc = FM_SEEK;
    U8 seekParam = FMAPP_INVALID_PARAM;
    U8 seek[FM_COMMAND_MAX_STR_LEN];

    (void)LINE_PARSER_GetNextStr(seek, 4);

    if(OS_StrCmp((const char *)seek, "UP") == 0)
    {
        seekParam = FM_DIR_UP;
    }
    else if(OS_StrCmp((const char *)seek, "DOWN") == 0)
    {
        seekParam = FM_DIR_DOWN;
    }
    else if (OS_StrCmp((const char *)seek, "STOP") == 0)
    {
        seekFunc = FM_STOP_SEEK;
        seekParam = FM_NO_VALUE;
    }
    
    if (FMAPP_INVALID_PARAM == seekParam)
    {
        Report(("FMRADIO: Invalid command"));
    }
    else
    {
        status = FM_Seek(fmcontext, seekFunc, seekParam);
        
        Report(("FM_Seek() returned status = %s.", getStatusString(status)));
    }
}

/*******************************************************************************
*  FUNCTION:	APP_FMRADIO_VolumeGain
*                                                                         
*  PARAMETERS:
*                                                                         
*  DESCRIPTION:	Increase or Decreases the Volume Level
*                                                                         
*  RETURNS:		
*******************************************************************************/
void APP_FMRADIO_SetVolume()
{
    FmStatus status;
    U8 volumeParam = FMAPP_INVALID_PARAM;
    U8 volume[FM_COMMAND_MAX_STR_LEN];

    (void) LINE_PARSER_GetNextStr(volume, 4);

    if(OS_StrCmp((const char *)volume,"UP") == 0)
    {
        volumeParam = FM_DIR_UP;
    }
    else if (OS_StrCmp((const char *)volume,"DOWN") == 0)
    {
        volumeParam = FM_DIR_DOWN;
    }
    
    if (FMAPP_INVALID_PARAM == volumeParam)
    {
        Report(("FMRADIO: Invalid command"));
    }
    else
    {
        /* Get the new Volume gain */
        getNewGain(volumeParam);
        
        status = FM_SetVolume(fmcontext, volumeGain);
        
        Report(("FM_SetVolume() returned status = %s.", getStatusString(status)));
    }
}


/*******************************************************************************
*  FUNCTION:	APP_FMRADIO_SetRssiSeachLevel
*                                                                         
*  PARAMETERS:	None
*                                                                         
*  DESCRIPTION:	Sets the RSSI seach Level : -127dBm to +20 dbm
*                                                                         
*  RETURNS:		
*******************************************************************************/

void APP_FMRADIO_SetRssiSeachLevel()
{
    S8 rssiLevel;
    FmStatus status;

    (void) LINE_PARSER_GetNextS8(&rssiLevel);	

    if((rssiLevel < -127) || (rssiLevel > 127))
    {
        Report((" Invalid RSSI Level - Should be between -127 to +20 dBm"));
        return;
    }	
	
    status = FM_RssiSearchLevel(fmcontext, rssiLevel);
    

    Report(("FMRADIO: FM_RssiSearchLevel() returned %d", status));			
}

/*******************************************************************************
*  FUNCTION:	APP_FMRADIO_GetRssiLevel
*                                                                         
*  PARAMETERS:	Get Current RSSI level
*                                                                         
*  DESCRIPTION:	Switch On/Off the RDS 
*                                                                         
*  RETURNS:		
*******************************************************************************/

void APP_FMRADIO_GetRssiLevel()
{
	FM_RssiGet(fmcontext);
}

/*******************************************************************************
*  FUNCTION:	APP_FMRADIO_GetRssiLevel
*                                                                         
*  PARAMETERS:	Get Current RSSI level
*                                                                         
*  DESCRIPTION:	Switch On/Off the RDS 
*                                                                         
*  RETURNS:		
*******************************************************************************/

void APP_FMRADIO_GetPi()
{
    FmStatus status;
    TIFM_U16 pi;
    
    status = FM_GetCurrentPi(fmcontext, &pi);

    if (FM_STATUS_SUCCESS == status)
    {
        Report(("Current station PI=0x%x", pi));
    }
    else
    {
        Report(("FM_GetCurrentPi() returned status = %s.", getStatusString(status)));
    }
}

/*******************************************************************************
*  FUNCTION:	APP_FMRADIO_SetRxAudioPath
*                                                                         
*  PARAMETERS:	headset, handset, or OFF
*                                                                         
*  DESCRIPTION:	Route FM Analog Audio from BRF to Triton outputs (handset, headset, or OFF)
*                                                                         
*  RETURNS:	None	
*******************************************************************************/

void APP_FMRADIO_SetRxAudioPath(void)
{
	char str[LINE_PARSER_MAX_STR_LEN];
	FmRxAudioPath audioPath;
	FmStatus status;

	if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr((U8*)str, LINE_PARSER_MAX_STR_LEN))
	{
		Report(("FM APP: LINE_PARSER_GetNextStr() failed"));
	}

	if (0 == strcmp(str,  "headset"))
	{
		audioPath = FM_RX_AUDIO_PATH_HEADSET;
	}
	else if (0 == strcmp(str, "handset"))
	{
		audioPath = FM_RX_AUDIO_PATH_HANDSET;
	}
	else if (0 == strcmp(str, "OFF"))
	{
		audioPath = FM_RX_AUDIO_PATH_OFF;
	}
	else
	{
		Report(("FM APP: illegal string parameter"));
		return;
	}

	status = FM_SetRxAudioPath(fmcontext, audioPath);

	if ((FM_STATUS_SUCCESS!= status) && (FM_STATUS_PENDING != status))
	{
		Report(("FM_SetRxAudioPath() returned status = %s.", getStatusString(status)));
	}
}

/*******************************************************************************
*  FUNCTION:	FMRADIOA_Init
*                                                                         
*  PARAMETERS:	None
*  DESCRIPTION:	Creates the a FM Context 
*
*                                                                         
*  RETURNS:		None
*******************************************************************************/

void FMRADIOA_Init()
{
    FmStatus status;

  
//  Assert(FM_STATUS_SUCCESS == FM_Init());

  
    status = FM_Create(0, FMRadioAppCallback,&fmcontext);

    Assert(FM_STATUS_SUCCESS == status);

    Report(("FMRADIOA_Init Complete"));

}

/*******************************************************************************
*  FUNCTION:	FMRADIOA_Deinit
*                                                                         
*  PARAMETERS:	
*  DESCRIPTION:	Destroys a FM Context created previously using FM_Create() 
*  RETURNS:          	
*******************************************************************************/


void FMRADIOA_Deinit()
{
	FmStatus status;

	status = FM_Destroy(&fmcontext);
	Assert(FM_STATUS_SUCCESS == status);
	FM_Deinit();

	Report(("FMRADIOA_Deinit: Completed"));
}

/*******************************************************************************
*  FUNCTION:	getNewGain
*                                                                         
*  PARAMETERS:	BOOL dir : FM_DIR_DOWN/FM_DIR_UP
*                                                                         
*  DESCRIPTION:	Volumet gain in steps of 5000
*                                                                         
*  RETURNS:	None	
*******************************************************************************/

void FMRadioAppCallback(const FmEvent *event)
{

	switch(event->type)
	{
		case FMEVENT_CMD_DONE:
			FmAppCmdDoneHandler(event->p.cmd_done.cmd, event->p.cmd_done.status, event->p.cmd_done.value);
			break;
		case FMEVENT_RADIO_TUNED:
			FmAppRadioTunedHandler(event->p.radio_tuned.status, event->p.radio_tuned.frequency);
			break;
		case FMEVENT_PS_CHANGED:
			FmAppPSChangedHandler(event->p.ps_changed.frequency, event->p.ps_changed.psName);
			break;
		case FMEVENT_AF_LIST_CHANGED:
			FmAppAfListChangedHandler(event->p.af_list_changed.pi, event->p.af_list_changed.afListSize, event->p.af_list_changed.afList);
			break;
		case FMEVENT_AF_JUMP:
			FmAppAfJumpHandler(event->p.af_jump.status, event->p.af_jump.pi, event->p.af_jump.oldFreq, event->p.af_jump.newFreq);
			break;
		case FMEVENT_RADIO_TEXT:
			FmAppRadioTextHandler(event->p.radio_text.changed, event->p.radio_text.length, event->p.radio_text.radioText);
			break;
		case FMEVENT_MOST_MODE_CHANGED:
			FmAppMoStModeChangedHandler(event->p.most_mode_changed.mode);
			break;
		case FMEVENT_RAW_RDS:
			FmAppRawRdsHandler(event->p.raw_rds.len, event->p.raw_rds.data);
			break;
	}

}

static void FmAppCmdDoneHandler(U8 event, FmStatus status, U32 value)
{
	switch(event)
		{
		case FM_CMD_POWER_ON:
			/* FM is enabled  */
			isFmRadioOn = TRUE;
			Report(("FMRADIO: Power is On."));
			break;
		case FM_CMD_POWER_OFF:
			/* FM is disabled */
			isFmRadioOn = FALSE;
			Report(("FMRADIO: Power is Off."));
			break;
		case FM_CMD_BAND_SET:
			Report(("FMRADIO: Band Set is Done."));
			break;
		case FM_CMD_SEEK:
			Report(("FMRADIO: Seek Done. Frequency = %d",value));
			break;
		case FM_CMD_RDS_SET:
			if(value == FM_RDS_OFF)
			{
				Report(("FMRADIO: RDS Off is Done."));
			}
			else
			{
				Report(("FMRADIO: RDS On is Done."));
			}
			break;
		case FM_CMD_SET_AF:
			if(value == FM_AF_OFF)
			{
				Report(("FMRADIO: AF Off is Done."));
			}
			else
			{
				Report(("FMRADIO: AF On is Done."));
			}
			break;
		case FM_CMD_RSSI_GET:
			Report(("FMRADIO: Get RSSI Done. RSSI = %d",value));
			break;
		default:
			break;
		}
}

static void FmAppRadioTunedHandler(U8 status, U32 frequency)
{
	Report(("FMRADIO: Tuned to Frequency: %d",frequency));
}

static void FmAppPSChangedHandler(U32 freq, U8 *data)
{
	Report(("FMRADIO:RDS - PS Changed %s", data));
}

static void FmAppAfListChangedHandler(U16 pi, U8 afListSize, U32 *afList)
{
	U8 index;

	if(afListSize > 0)
	{
		Report(("FMRADIO:AF List Changed. List size = %d", afListSize));
		for(index = 0; index < afListSize; index++)
		{
			Report(("FMRADIO: AF List index%d = %d.", (index+1), afList[index]));
		}
	}
}

static void FmAppAfJumpHandler(U8 status, U16 Pi, U32 oldFreq, U32 newFreq)
{
	switch(status)
	{
		case AF_JUMP_SUCCESS:
			Report(("FMRADIO: Succeeded AF Jump from %d to %d", oldFreq, newFreq));
			break;

		case AF_JUMP_FAILED_LIST_FINISHED:
			Report(("FMRADIO: Failed AF Jump from %d to %d - List Finished", oldFreq, newFreq));
			break;

		case AF_JUMP_FAILED_LIST_NOT_FINISHED:
			Report(("FMRADIO: Failed AF Jump from %d to %d - Continue", oldFreq, newFreq));
	}
}
static void FmAppRadioTextHandler(BOOL changed, U8 length, U8 *radioText)
{
	U8 text[65];

	OS_MemCopy(text, radioText, length);
	text[length] = '\0';
	Report(("FMRADIO: RDS - RadioText %s", text));
}

static void FmAppMoStModeChangedHandler(U8 mode)
{
	if(mode == FM_STEREO_MODE)
	{
		Report(("FMRADIO: MoSt Mode Changed to Stereo"));
	}
	else
	{
		Report(("FMRADIO: MoSt Mode Changed to Mono"));
	}
}

static void FmAppRawRdsHandler(U16 len, U8 *data)
{
	Report(("FMRADIO:Raw Rds was sent up"));
}


/*******************************************************************************
*  FUNCTION:	getNewGain
*                                                                         
*  PARAMETERS:	BOOL dir : FM_DIR_DOWN/FM_DIR_UP
*                                                                         
*  DESCRIPTION:	Volume gain in steps of 5000
*                                                                         
*  RETURNS:	None	
*******************************************************************************/

static void getNewGain(BOOL dir)
{
	if(dir == FM_DIR_DOWN)
	{
		if(volumeGain <= GAIN_STEP)
		{
			volumeGain = VOLUMN_MIN;
		}
		else
		{
			volumeGain -=GAIN_STEP;
		}
	}
	else
	{
		if(volumeGain >= VOLUMN_MAX - GAIN_STEP)
		{
			volumeGain = VOLUMN_MAX;
		}
		else
		{
			volumeGain +=GAIN_STEP;
		}
	}
}

#if XA_DEBUG == XA_ENABLED
static char *getEventString(U8 event)
{
	switch(event)
	{
		case FM_CMD_POWER_ON:
			return "Power On";
		case FM_CMD_POWER_OFF:
			return "Power Off";
		case FM_CMD_MOST_SET:
			return "Most Set";
		case FM_CMD_BAND_SET:
			return "Band Set";
		case FM_CMD_MUTE:
			return "Mute";
		case FM_CMD_VOLUME_SET:
			return "Volume Set";
		case FM_CMD_RDS_SET:
			return "Rds Set";
		case FM_CMD_TUNE:
			return "Tune";
		case FM_CMD_RSSI_GET:
			return "Rssi Get";
		case FM_CMD_SEEK:
			return "Seek";
		case FM_CMD_SET_AF:
			return "AF Set";
		case FM_CMD_SET_STEREO_BLEND:
			return "Stereo Blend Set";
		case FM_CMD_SET_DEEMPHASIS_MODE:
			return "Deemphasis mode Set";
		case FM_CMD_SET_RSSI_SEARCH_LEVEL:
			return "Rssi search level Set";
		case FM_CMD_SET_PAUSE_LEVEL:
			return "Pause level Set";
		case FM_CMD_SET_PAUSE_DURATION:
			return "Pause duration Set";
		case FM_CMD_SET_RDS_RBDS_MODE:
			return "Rds Rbds Set";
		case FM_CMD_MOST_GET:
			return "Most Get";

		default:
			return "default";
	}
	
}

static char *getStatusString(U8 status)
{
	switch(status)
	{
		case FM_STATUS_SUCCESS:
			return "Success";
		case FM_STATUS_FAILED:
			return "Failed";
		case FM_STATUS_PENDING:
			return "Pending";
		case FM_STATUS_INVALID_PARM:
			return "Invalid parameter";
		case FM_STATUS_BT_NOT_INITIALIZED:
			return "BT not initialized";
		case FM_STATUS_FAILED_FM_NOT_ON:
			return "Failed - Fm not on";
		case FM_STATUS_FM_ALREADY_ON:
			return "Fm already On";
		case FM_STATUS_SEEK_REACHED_BAND_LIMIT:
			return "Seek reached band limit";
		case FM_STATUS_SEEK_STOPPED:
			return "Seek stopped";
		case FM_STATUS_SEEK_SUCCESS:
			return "Seek success";
		case FM_STATUS_STOP_SEEK:
			return "Stop seek";
		case FM_STATUS_PENDING_UPDATE_CMD_PARAMS:
			return "Pending - the params were updated";
		case FM_STATUS_FAILED_ALREADY_PENDING:
			return "Command already pending";
		case FM_STATUS_INVALID_TYPE:
			return "Invalid type";
		case FM_STATUS_HCI_INIT_ERR:
			return "HCI init error";
		case FM_STATUS_IN_PROGRESS:
			return "In progress";
		case FM_STATUS_INTERNAL_ERROR:
			return "Internal error";
		case FM_STATUS_FAIL_RDS_OFF:
			return "Fail RDS off";
		case FM_STATUS_FAIL_NO_VALUE_AVAILABLE:
			return "Fail no value available";

		default:
			return "default";
	}
}
					
#endif

#else /* BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED*/

void FMRADIOA_Init()
{
	Report(("FMRADIOA_Init -BTL_CONFIG_FM_STACK is disabled "));
}
void FMRADIOA_Deinit()
{
	Report(("FMRADIOA_Deinit  - BTL_CONFIG_FM_STACK is disabled"));
}

void FMRADIOA_UI_ProcessUserAction(U8 * msg)
{
	msg=msg;
	Report(("FM_APP is disabled via BTL_CONFIG."));

}



#endif/* BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED*/

