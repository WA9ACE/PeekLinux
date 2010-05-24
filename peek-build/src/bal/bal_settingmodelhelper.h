/*=====================================================================================
FILE NAME   : bal_settingmodehelper.h
MODULE NAME : ASL

  
GENERAL DESCRIPTION
	
Copyright (c) 2008-2009 by BTC. All Rights Reserved.
=======================================================================================
Revision History
	  
Modification                   Tracking
Date              Author       Number       Description of changes
----------   --------------   ---------   --------------------------------------
2008-02-28     Broad Zou                       create the file
		
=====================================================================================*/

#ifndef ASL_SETTING_MODE_HELPER_H_
#define ASL_SETTING_MODE_HELPER_H_

#ifdef  __cplusplus
extern "C" {
#endif

/* Clear out the redundant trace information. Modify by #zouyonghang at 2008/6/10
#ifndef SETTING_DEBUG
#define SETTING_DEBUG
#endif
*/

#define SETTING_EOF -1
#define SIDB_STRING_MAX_LENGTH 70
#define STREAM_MAX_SIZE 1024
#define MAX_TOKEN_SIZE 128

#define SETTING_CONFIG_FILE "/setting_ini"

//#define TUNE_ID "TuneID"
//#define TUNE_NAME "TuneName"
//#define LIGHT_SCHEME_ID "LightSchemeID"

#define SndAndAlert "SndAndAlert"
#define DispTheme "DispTheme"
#define BKProfile "BKProfile"
#define SettingProfile "SettingProfile"

#define AutoLockFlag "AutoLockFlag"
#define LockInterval "LockInterval"
#define LockPassword "LockPassword"

#define Pword "Pword"
#define PwdEntertimes "Entertimes"
#define PhoneLockFlag "PhoneLockFlag"
#define SecurityModeFlag "SecurityModeFlag"
#define TimeModeFlag "TimeMode"
#define PayModeFlag "PayMode"
#define TestModeFlag "TestMode"
#define GCFStateFlag "GCFState"
#define FontModeFlag "FontMode"
#define DayLightFlag "DayLight"

#define RingtoneTypeFlag "RingtoneType"
#define VolumnTypeFlag "VolumnType"
#define VibrateTypeFlag "VibrateType"
#define RepeatModeFlag "RepeatMode"

//#define VIBRATE "Vibrate"
//#define DISPLAY_THEME_ID "DisplayThemeID"
//#define AUTO_LOCK "AutoLock"
//#define AUTO_LOCK_INTERVAL "AutoLockInterval"
//#define UNLOCK_PASSWORD "UnLockPassword"
//#define DATE_FORMAT "DateFormat"
//#define TIME_FORMAT "TimeFormat"
#define TIME_ZONE "TimeZone"
//#define TIME_SEPARATOR "TimeSeparator"
//#define IP_ADDRESS "IpAddress"
//#define PORT "Port"
//#define APN "APN"
//#define NET_NAME "NetName"
//#define NET_PASSWORD "NetPassword"
//#define SETTING_PROFILE_ID "SettingProfileId"
//#define BACK_LIGHT_SCHEME_ID "BackLightSchemeId"
//#define BACK_LIGHT_MODE_KEEP_ON "BackLightModeKeepOn"
//#define BACK_LIGHT_MODE_STANDARD_D2D "BackLightModeStandardD2D"

typedef struct tag_TokenStream
{
	uint8 *pchInputStream;
	int32 streamLen;
	int32 curPos;
	uint8 chToken[MAX_TOKEN_SIZE];
	int32 strLen;
} TokenStream;

typedef  struct  tag_SidbString
{ 
	uint8 StringLen;
	uint8 String[SIDB_STRING_MAX_LENGTH];
} SidbString;

/*typedef enum tag_TuneList
{
	eTune1,
	eTune2,
	eTune3
} TuneList;

typedef enum tag_LightSchemeList
{
	eLightScheme0,
	eLightScheme1,
	eLightScheme2,
	eLightScheme3
} LightSchemeList;

typedef enum tag_DisplayThemeList
{
	eDTSeaBlue,
	eDTRubyRed,
	eDTPaleYellow,
	eDTGrassGreen,
	eDTDiamondPurple
} DisplayThemeList;

//! Stored idle background type
typedef enum tag_BackGroundType
{
 	eBKImage,
 	eBKOn,
 	eBKOff,
 	eBKAnimation
} BKType;

typedef struct tag_LightSchemeMode
{
	uint32 uiLightInterval;
} LightSchemeMode;

typedef enum tag_VibrateValueList
{
	eVibrateOff,
	eVibrateOn
} VibrateValueList;

typedef enum tag_AutoLockValueList
{
	eAutoLockOff,
	eAutoLockOn
} AutoLockValueList;

typedef struct tag_SoundAndAlert
{
	TuneList eTuneId;
	SidbString stTuneName;
	LightSchemeList eLightSchemeId;
	LightSchemeMode stSchemeMode;
	VibrateValueList eVibrateValue;
} SoundAndAlert;

typedef struct tag_DisplayThemeMode
{
	BKType eBKType;
} DisplayThemeMode;

typedef struct tag_DisplayTheme
{
	DisplayThemeList eDTId;
	DisplayThemeMode stDTMode;
} DisplayTheme;

typedef struct tag_DeviceInfo
{
	uint32 AutoLockInval;
	AutoLockValueList eAutoLockValue;
	SidbString stPassword;
} DeviceInfo;*/

/*typedef enum tag_DateFormat
{
	DATE_FORMAT_DDMMYYYY = 0,
	DATE_FORMAT_DDMMMYYYY,
	DATE_FORMAT_DDMMYY,
	DATE_FORMAT_MMDDYYYY,
	DATE_FORMAT_MMMDDYYYY,
	DATE_FORMAT_MMDDYY,
	DATE_FORMAT_YYYYMMDD,
	DATE_FORMAT_YYYYMMMDD,
	DATE_FORMAT_YYMMDD
} DateFormat;

typedef enum tag_TimeFormat
{
	TIME_FORMAT_12,
	TIME_FORMAT_24
} TimeFormat;*/

/*typedef enum tag_TimeZone
{
	TimeZone12GMT=0,
	TimeZone11GMT,
	TimeZone10GMT,
	TimeZone930GMT,
	TimeZone9GMT,
	TimeZone830GMT,
	TimeZone8GMT,
	TimeZone7GMT,
	TimeZone6GMT,
	TimeZone5GMT,
	TimeZone4GMT,
	TimeZone330GMT,
	TimeZone3GMT,
	TimeZone2GMT,
	TimeZone1GMT,
	TimeZoneGMT,
	TimeZoneGMT1,
	TimeZoneGMT2,
	TimeZoneGMT3,
	TimeZoneGMT330,
	TimeZoneGMT4,
	TimeZoneGMT430,
	TimeZoneGMT5,
	TimeZoneGMT530,
	TimeZoneGMT6,
	TimeZoneGMT630,
	TimeZoneGMT7,
	TimeZoneGMT8,
	TimeZoneGMT9,
	TimeZoneGMT930,
	TimeZoneGMT10,
	TimeZoneGMT1030,
	TimeZoneGMT11,
	TimeZoneGMT12 
}TimeZone; 
*/
typedef struct tag_DateInfo
{
	uint32 year;
	uint32 month;
	uint32 day;
} DateInfo;
 
typedef struct tag_TimeInfo
{
	uint32 hour;
	uint32 minute;
	uint32 second;
} TimeInfo;

/*typedef enum tag_TimeSeparator
{
	TIMESEPARATOR_DOT=0, // "."
	TIMESEPARATOR_BIAS,//"/"
	TIMESEPARATOR_SHORTLINE,//"-"
	TIMESEPARATOR_SPACE //" "
}TimeSeparator;*/

typedef struct tag_DateAndTime
{
	DateInfo date;
	TimeInfo time;
	uint8 nouse;
	uint8 TimeFormatValue;
	uint8 DateFormatValue;
	uint8 TimeSeparator;
	int TimeZoneValue;	
} DateAndTime;

/*typedef struct tag_NetWork
{
	SidbString stIpAddress;
	uint32 portNum;
	SidbString stUserName;
	SidbString stNetworkPassword;
	SidbString stAPN;
} NetWork;

typedef enum tag_BackLightListT
{
	eInDoorMode,
	eOutDoorMode,
	eNightTimeMode,
} BackLightModeListT;

typedef struct tag_BackLightModeT
{
	bool fKeepLightOn;
	bool fStandardD2D;
} BackLightModeT;

typedef struct tag_LightProfileT
{
	BackLightModeListT lcdBackLightId;
	BackLightModeT stLcdBackLightMode;
} LightProfileT;*/

typedef enum tag_SndAndAlertListT
{
	eVibrate,
	eRingerHigh,
	eRingerMid,
	eRingerLow,
	eSilent,
	eRingerHigh_Vibrate,
	eRingerMid_Vibrate,
	eRingerLow_Vibrate
} SndAndAlertListT;

typedef enum tag_DispThemeListT
{
	// Change by fanxujun for set the Dusk theme as default, 20080917
	eMidnight,
	eTangerine,
	eSpring
} DispThemeListT;

typedef enum tag_BKProfileListT
{
	eStandard,
	eBatterySaver,
	eBatteryHog
} BKProfileListT;

typedef enum tag_FlightModeListT
{
	eNormalMode,
	eFlightMode
} FlightModeListT;

typedef struct tag_ScreenControlT
{
	bool autoLock;
	int interval;
	SidbString stPassword;
} ScreenControlT;

typedef struct tag_LockControlT
{
	char pword[16];
	int Entertimes;
	int PhoneLock;
	int SecurityMode;	
} LockControlT;

typedef struct tag_SoundAndAlertControlT
{
	int RingtoneType; // Original , Type2 , Type3 , Type4 , Type5
	int VolumnType;   // Quiet , Normal , Bold , Off
	int VibrateType;   // Low , Medium , High , Off
	int RepeatMode;  // Normal , Repeat
} SoundAndAlertControlT;

typedef enum tag_TimeModeSet
{
	ManualMode,
	NetworkMode
} TimeModeSet;

typedef enum tag_TestModeSet
{
	TestOff,
	TestOn
} TestModeSet;

typedef enum tag_PayModeSet
{
	PrePay,
	PostPay
} PayModeSet;

typedef enum tag_GCFStateSet
{
	GCFOff,
	GCFOn
} GCFStateSet;		

typedef enum tag_FontModeSet
{
	Normal,
	Bold
} FontModeSet;

	
typedef struct tag_SettingModeHelperInfo
{
	SndAndAlertListT eSndAndAlert;
	BKProfileListT eBKProfile;
	DispThemeListT eDispTheme;
	FlightModeListT eFlightMode;
	ScreenControlT stScreenControl;
	DateAndTime stDateTime;
	LockControlT stLockControl; 
	TimeModeSet eTimeMode;
	GCFStateSet eGCFState;
	FontModeSet eFontMode;
	SoundAndAlertControlT stSoundAndAlert;
	TestModeSet eTestMode;
	unsigned char DayLight;
	PayModeSet ePayMode;
} SettingModeHelplerInfo;

extern SettingModeHelplerInfo stCurSettingMode;

#define UI_FIELD_OFFSET(type, field)                    ((uint32)&(((type*)0)->field))
#define UI_FIELD_OFFSET2(type, field1, field2)          ((uint32)&(((type*)0)->field1.field2))
#define UI_FIELD_OFFSET3(type, field1, field2, field3)  ((uint32)&(((type*)0)->field1.field2.field3))
#define UI_FIELD_SIZE(type, field)		(sizeof((((type *)0)->field)))

/* get sound and alert mode */
#define GET_SOUND_AND_ALERT_MODE() (stCurSettingMode.eSndAndAlert)

/* get back light profile */
#define GET_BK_PROFILE() (stCurSettingMode.eBKProfile)

/* get display theme ID */
#define GET_DISP_THEME_ID() (stCurSettingMode.eDispTheme)

/* get system profile */
#define GET_FLIGHT_MODE() (stCurSettingMode.eFlightMode)

/* get auto lock flag */
#define GET_SCREEN_AUTOLOCK_FLAG() (stCurSettingMode.stScreenControl.autoLock)

/* get auto lock interval */
#define GET_SCREEN_AUTOLOCK_INTERVAL() (stCurSettingMode.stScreenControl.interval);

/* get auto lock password */
#define GET_SCREEN_AUTOLOCK_PASSWORD() (&stCurSettingMode.stScreenControl.stPassword);

/* get tune ID */
//#define GET_SOUND_AND_ALERT_RINGER_ITEM() (stCurSettingMode.stSoundAndAlert.eTuneId)

/* get tune fine name */
//#define GET_SOUND_AND_ALERT_RINGER_NAME() (&stCurSettingMode.stSoundAndAlert.stTuneName)

/* get light scheme ID */
//#define GET_SOUND_AND_ALERT_LIGHTSCHEME_ID() (stCurSettingMode.stSoundAndAlert.eLightSchemeId)

/* get light scheme mode */
//#define GET_SOUND_AND_ALERT_LIGHTSCHEME_MODE() (&(stCurSettingMode.stSoundAndAlert.stSchemeMode))

/* get whether vibrate when a new mail coming */
//#define GET_SOUND_AND_ALERT_VIBRATE() (stCurSettingMode.stSoundAndAlert.eVibrateValue)

/* get display theme ID */
//#define GET_DISPLAY_THEME_ID() (stCurSettingMode.stDiaplayTheme.eDTId)

/* get display theme mode */
//#define GET_DISPLAY_THEME_MODE() (&(stCurSettingMode.stDiaplayTheme.stDTMode))

/* get auto lock interval */
//#define GET_DEVICE_AUTOLOCK_INTERVAL() (stCurSettingMode.stDeviceInfo.AutoLockInval)

/* get whether need a password to unlock */
//#define GET_DEVICE_PWTO_UNLOCK() (stCurSettingMode.stDeviceInfo.eAutoLockValue)

/* get unlock password */
//#define GET_DEVICE_PASSWORD() (&stCurSettingMode.stDeviceInfo.stPassword)

/* these API are not confirmed by Archermind and TXTBL */
//#define GET_DATE() (&stCurSettingMode.stDateTime.date)
//#define GET_TIME() (&stCurSettingMode.stDateTime.time)
#define GET_TIME_ZONE() (stCurSettingMode.stDateTime.TimeZoneValue)
//#define GET_DATE_FORMAT() (stCurSettingMode.stDateTime.DateFormatValue)
//#define GET_TIME_FORMAT() (stCurSettingMode.stDateTime.TimeFormatValue)
//#define GET_TIME_SEPARATOR() (stCurSettingMode.stDateTime.TimeSeparator) 
#define GET_REMAIN_TIMES() (stCurSettingMode.stLockControl.Entertimes)
#define GET_PASSWORD() (stCurSettingMode.stLockControl.pword)
#define GET_PHONELOCKFLAG() (stCurSettingMode.stLockControl.PhoneLock)
#define GET_SECURITYMODE() (stCurSettingMode.stLockControl.SecurityMode)
#define GET_TIMEMODE() (stCurSettingMode.eTimeMode)
#define GET_TESTMODE() (stCurSettingMode.eTestMode)
#define GET_DAYLIGHT() (stCurSettingMode.DayLight)
#ifdef BAL_PAY_MODE
#define GET_PAYMODE() (stCurSettingMode.ePayMode)
#endif
#define GET_FONTMODE() (stCurSettingMode.eFontMode)
#define GET_GCFSTATUS() (stCurSettingMode.eGCFState)

#define GET_RINGTONETYPE() (stCurSettingMode.stSoundAndAlert.RingtoneType)
#define GET_VOLUMNTYPE() (stCurSettingMode.stSoundAndAlert.VolumnType)
#define GET_VIBRATETYPE() (stCurSettingMode.stSoundAndAlert.VibrateType)
#define GET_REPEATMODE() (stCurSettingMode.stSoundAndAlert.RepeatMode)

//#define GET_NET_PORT() (stCurSettingMode.stNetWork.portNum)
//#define GET_NET_APN() (&stCurSettingMode.stNetWork.stAPN)
//#define GET_NET_NAME() (&stCurSettingMode.stNetWork.stUserName)
//#define GET_NET_PASSWORD() (&stCurSettingMode.stNetWork.stNetworkPassword)
/* these API are not confirmed by Archermind and TXTBL */

//#define GET_SETTING_PROFILE_ID() (stCurSettingMode.stSettingProfile.profileModeId)
//#define GET_BACKLIGHT_ID() (stCurSettingMode.stBackLightProfile.lcdBackLightId)
//#define GET_BACKLIGHT_MODE() (&(stCurSettingMode.stBackLightProfile.stLcdBackLightMode))

/*=================================================================================
FUNCTION:
fResumeSettingFromConfigFile
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
read manufacturer's defalut setting value from setting config file and write setting item value to SIDB and buf.
ARGUMENTS PASSED: Void
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool fResumeSettingFromConfigFile();

/*=================================================================================
FUNCTION:
fReloadSettingInfoFromSidb
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
reload setting info value to buf form SIDB.
ARGUMENTS PASSED: Void
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool fReloadSettingInfoFromSidb();

/*=================================================================================
FUNCTION:
fSetTuneId
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set Ringer Id when a new mail coming
ARGUMENTS PASSED: enum
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
//bool fSetTuneId(uint8 eTuneId);

/*=================================================================================
FUNCTION:
fSetTuneName
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set Ringer file name when a new mail coming
ARGUMENTS PASSED: SidbString pointer
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
//bool fSetTuneName(SidbString *pstTuneName);

/*=================================================================================
FUNCTION:
fSetLightSchemeId
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set light scheme id when a new mail coming
ARGUMENTS PASSED: enum
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
//bool fSetLightSchemeId(uint8 eSchemeId);

/*=================================================================================
FUNCTION:
fSetLightSchemeMode
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set Light scheme mode value
ARGUMENTS PASSED: LightSchemeMode pointer
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
//bool fSetLightSchemeMode(LightSchemeMode *pstSchemeMode);

/*=================================================================================
FUNCTION:
fSetVibrate
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set whether need to vibrate when a new mail coming
ARGUMENTS PASSED: uint8
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
//bool fSetVibrate(uint8 eVibrate);

/*=================================================================================
FUNCTION:
fSetDisplayThemeId
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set Display theme id
ARGUMENTS PASSED: enum
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
//bool fSetDisplayThemeId(uint8 eThemeId);

/*=================================================================================
FUNCTION:
fSetDisplayThemeMode
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
 set display theme mode
ARGUMENTS PASSED: DisplayThemeMode pointer
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
//bool fSetDisplayThemeMode(DisplayThemeMode *pstDTMode);

/*=================================================================================
FUNCTION:
fSetDeviceInfoInterval
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set the interval for automatic lock
ARGUMENTS PASSED: uint32
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
//bool fSetDeviceInfoInterval(uint32 uiInterval);

/*=================================================================================
FUNCTION:
fSetDeviceInfoPwNeed
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set whether need password to unlock
ARGUMENTS PASSED: uint8
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
//bool fSetDeviceInfoPwNeed(uint8 eAutoLock);

/*=================================================================================
FUNCTION:
fSetDeviceInfoPassword
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set unlock password
ARGUMENTS PASSED: SidbString pointer
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
//bool fSetDeviceInfoPassword(SidbString *pstPassword);


/* these API are not confirmed by Archermind and TXTBL */

//bool fSetDate(DateInfo *pstDateInfo);
//bool fSetTime(TimeInfo *pstTimeInfo);
//bool fSetTimeZone(uint8 eTimeZone);
//bool fSetDateFormat(uint8 eDateFormat);
//bool fSetTimeFormat(uint8 eTimeFormat);
//bool fSetTimeSeparator(uint8 eTimeSeparator);

//bool fSetNetIpAddress(SidbString *pstNetIpAddress);
//bool fSetNetPort(uint32 uiPort);
//bool fSetNetAPN(SidbString *pstAPN);
//bool fSetNetUserName(SidbString *pstNetUserName);
//bool fSetNetPassword(SidbString *pstNetPassword);
/* these API are not confirmed by Archermind and TXTBL */

/*=================================================================================
FUNCTION:
fSetSettingProfileId
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set the setting profile
ARGUMENTS PASSED: uint8
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
//bool fSetSettingProfileId(uint8 eSettingProfileId);

/*=================================================================================
FUNCTION:
fSetBackLightModeId
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set the back light mode id
ARGUMENTS PASSED: uint8
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
//bool fSetBackLightSchemeId(uint8 eBackLightId);

/*=================================================================================
FUNCTION:
fSetBackLightModeStandardD2D
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set the back light mode standard D2D
ARGUMENTS PASSED: uint8
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
//bool fSetBackLightModeStandardD2D(uint8 fStandard);

/*=================================================================================
FUNCTION:
fSetBackLightModeKeepOn
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set the back light mode keep light on
ARGUMENTS PASSED: uint8
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
//bool fSetBackLightModeKeepOn(uint8 fKeepOn);

/*=================================================================================
FUNCTION:
fSetSndAndAlertMode
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set the sound and alert mode.
ARGUMENTS PASSED: uint8
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool fSetSndAndAlertMode(uint8 ModeId);

/*=================================================================================
FUNCTION:
fSetBKProfile
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set the back light profile
ARGUMENTS PASSED: uint8
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool fSetBKProfile(uint8 ProfileId);

/*=================================================================================
FUNCTION:
fSetDisplayTheme
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set the display theme.
ARGUMENTS PASSED: uint8
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool fSetDisplayTheme(uint8 ThemeId);

/*=================================================================================
FUNCTION:
fGetDisplayTheme
CREATE DATE:
2008-7-22
AUTHOR: 
Zou Yonghang
DESCRIPTION:
get the displaytheme
ARGUMENTS PASSED: void
RETURN VALUE: DispThemeList
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
DispThemeListT fGetDisplayTheme(void);

/*=================================================================================
FUNCTION:
fSetSettingProfileId
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set the setting profile
ARGUMENTS PASSED: uint8
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool fSetFlightMode(uint8 ProfileId);

/*=================================================================================
FUNCTION:
fSetScreenAutoLockFlag
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set the screen autolock flag
ARGUMENTS PASSED: bool
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool fSetScreenAutoLockFlag(bool flag);

/*=================================================================================
FUNCTION:
fSetScreenAutoLockInterval
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set the screen autolock interval in seconds.
ARGUMENTS PASSED: int
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool fSetScreenAutoLockInterval(int interval);

/*=================================================================================
FUNCTION:
fSetScreenAutoLockPassword
CREATE DATE:
2008-3-7
AUTHOR: 
Broad Zou
DESCRIPTION:
set the screen autolock password.
ARGUMENTS PASSED: SidbString*
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool fSetScreenAutoLockPassword(SidbString *pstPassword);

/*=================================================================================
FUNCTION:
fWrSettingModeHelplerInfo
CREATE DATE:
2008-7-8
AUTHOR:
Zouyonghang
DESCRIPTION:
Write the data of the SettingModeHelplerInfo struct and the data is thd default config.
=================================================================================== */
void fWrSettingModeHelplerInfo(void);

bool fSetTimeZone(int eTimeZone);

bool fSetPassword(char *password);

bool fSetPasswordTimes(int entertimes);

bool fSetPhoneLock(int flag);

bool fSetSecurityMode(int flag);

bool fSetTimeMode(int flag);

bool fSetTestMode(int flag);

bool fSetDayLightFlag(unsigned char flag);

#ifdef BAL_PAY_MODE
bool fSetPayMode(int flag);
#endif

bool fSetGCFState(int flag);

bool fSetFontMode(int flag);

bool fSetRingtoneType(int ringtonetype);

bool fSetVolumnType(int volumntype);

bool fSetVibrateType(int vibratetype);

bool fSetRepeatMode(int repeatmode);

#ifdef  __cplusplus
}
#endif

#endif  /* ASL_SETTING_MODE_HELPER_H_*/
