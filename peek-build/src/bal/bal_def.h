/*=================================================================================

	HEADER NAME : bal_def.h
	MODULE NAME : BAL
	
	PRE-INCLUDE FILES DESCRIPTION
	
	
GENERAL DESCRIPTION
all btc adapter layer related prototype and declare 

BYD TECHFAITH Wireless Confidential Proprietary
(c) Copyright 2008 by BYD TECHFAITH Wireless. All Rights Reserved.
===================================================================================
Revision History
Modification Tracking
Date Author Number Description of changes
---------- ------------ --------- --------------------------------------------
2008-01-29 Randolph Wang create		general defines used in Bal

	
=================================================================================*/

#ifndef BAL_DEF_H
#define BAL_DEF_H

#include "ottsdefs.h"
#include "exeapi.h"
#include "balapi.h"
#include "plus/inc/dm_defs.h"
#include "exedefs.h"
#include "exepowr.h"
#include "stdarg.h"
#include "baldispapi.h"
#include "balschedulerapi.h"

#if 0	//qwert
#define DISP_LCD_128x128
#else
#define DISP_LCD_320x240
#endif
/*******************************************************************************
*    Macro Define Section
*******************************************************************************/
//define the 5 BAL,UI and Translation task entities' name
#define BAL_NAME					"BAL"
#define BALUI_NAME				"BALUI"
#define BALUIS_NAME				"BALUIS"
#define BAL_AMDB_NAME				"BAL_AMDB"

#define BALLSM_NAME				"BALLSM"
#define TRANS_NAME				"TRANS"

//bal_pei.c
#define BAL_EvGrp_NAME		"BalEvGrp"
#define BAL_Que_NAME			"BalQue"
//bal_ui_pei.c
#define VALUI_EvGrp_NAME	"UiEvGrp"
#define VALUI_Que_NAME		"UiQue"
//bal_uis_pei.c
#define BALEMS_EvGrp_NAME		"EMSEvGrp"
#define BALEMS_Que_NAME		"EMSQue"

#define BAL_AMDB_EvGrp_NAME		"AMDBEvGrp"
#define BAL_AMDB_Que_NAME		"AMDBQue"

//bal_lsm_pei.c
#define BALLSM_EvGrp_NAME	"LSMEvGrp"
#define BALLSM_Que_NAME		"LSMQue"

//define the five task entities' priority the higher the numeric value,the higher the task's priority.
#define BAL_AMDB_PRIORITY               45
#define BAL_PRIORITY                    70
#define BAL_UI_PRIORITY                 40
#define BAL_UIS_PRIORITY                48
#define BAL_TRANS_PRIORITY              75
#define BAL_LSM_PRIORITY			5

//for TI signal type
#define TRANS_TO_TRANS				0x8003
#define TRANS_FROM_ACI				0x8000	//TRANS recieve signals from ACI
#define TRANS_FROM_HAL				0x8001	//TRANS recieve signals from HAL
#define TRANS_FROM_STORAGE		0x8002	//TRANS recieve signals from STORAGE

#define QWERT_EXT_VER	"Ex:01.08.08"
#define QWERT_INT_VER	QWERT_EXT_VER"\nAM:EC2.0.51.121808\nBYD:VIN_QWERT_09w29_MP2"

#define DEV_SN_FILE 	"/NPI_conf/SN.txt"
#define DEV_SN_STR_LEN (20)
#define QWERT_MODEL_NUMBER "MP2"

/* added by zouda for realize app callback */

#define BAL_REGISTER_MAX_OPERATEID            0xFFFE  /* max operate id or invalid operate id  */


#define BAL_FLIGHT_MODE 1

#ifdef BAL_FLIGHT_MODE
typedef enum
{
	BAL_FLIGHT_MODE_FAIL = -1,
	BAL_FLIGHT_MODE_CLOSED,
	BAL_FLIGHT_MODE_OPENED
} BAL_FLIGHT_MODE_STATUS;
#endif

//add by zhihui for select plmn
#ifdef BAL_SELECT_PLMN
#define NM_PLMN_AUTOMATIC    0       /* automatic mode set */
#define NM_PLMN_MANUAL       1       /* manual mode is set */
#define MAX_PLMN          12     /* maximal PLMN number */
#define LONG_NAME         26
#endif


typedef enum tagValMsgCallBackT
{
	BAL_TO_APP_BATT_CB,
	BAL_TO_APP_SIM_CB,
	BAL_TO_APP_PLMN_CB,
	BAL_TO_APP_FLIGHTMODE_CB,
	BAL_TO_APP_CB_MAX
} BalMsgCallBackT;

typedef struct tagValEventFuncTableT
{
	RegIdT BalEventFuncId;
	BalEventFunc pfValEventFunc;
} BalEventFuncTableT;

extern BalEventFuncTableT BalEventFuncTable[];

//add by zhihui for select plmn
#ifdef BAL_SELECT_PLMN
typedef struct
{
    char nameoper [LONG_NAME];  /* operator name in long format */
    unsigned char status_indicator;
    unsigned int numoper;
} BAL_NM_PLMN_STRUCT;

typedef struct
{
    int count;                /* number of available PLMNs */
    BAL_NM_PLMN_STRUCT plmn[MAX_PLMN];  /* plmn identifications */
} BAL_NM_PLMN_LIST;

typedef struct{
BAL_NM_PLMN_LIST* list_plmn_result;
int select_plmn_result;
}BAL_NM_PLMN_RESPONSE;

typedef enum              /* PLMN functional return codes */
{
  PLMN_FAIL = -1,           /* execution of command failed */
  PLMN_CMPL,                /* execution of command completed */
  PLMN_EXCT,                /* execution of command is in progress */
  PLMN_BUSY                /* execution of command is rejected due to a busy command handler */
} BAL_PLMN_RETURN;

typedef enum                    /* CGREG command read stat */
{
  PLMN_STAT_NOT_PRESENT = -1,  /* not present, last state is not indicated */
  PLMN_STAT_NOT_REG,           /* not registered, no searching */
  PLMN_STAT_REG_HOME,          /* registered, home network */
  PLMN_STAT_SEARCHING,         /* not registered, but searching */
  PLMN_STAT_REG_DEN,           /* registration denied */
  PLMN_STAT_UNKN,              /* unknown */
  PLMN_STAT_REG_ROAM           /* registered, roaming */
} BAL_PLMN_STAT;
#endif

enum
{
	VIBRATOR_LOW = 1,
	VIBRATOR_MEDIUM = 2,
	VIBRATOR_HIGH = 4,
	VIBRATOR_OFF = 5
};

enum
{
	VOLUMN_QUIET = 5,
	VOLUMN_NORMAL = 7,
	VOLUMN_BOLD = 8,
	VOLUMN_OFF = 9
};

enum
{
	RINGTONETYPE_ORIGINAL = 1,
	RINGTONETYPE_TYPE2,
	RINGTONETYPE_TYPE3,
	RINGTONETYPE_TYPE4,
	RINGTONETYPE_TYPE5
};

#define RINGTONE_ORIGINAL "/FFS/original.mid"
#define RINGTONE_TYPE2 "/FFS/type2.mid"
#define RINGTONE_TYPE3 "/FFS/type3.mid"
#define RINGTONE_TYPE4 "/FFS/type4.mid"
#define RINGTONE_TYPE5 "/FFS/type5.mid"

/*******************************************************************************
*    Type Define Section
*******************************************************************************/
//Test msg struct,maybe no use any more.
typedef struct 
{
/** Message header. */
int		hdr;
}  T_TEST_MSG;

/*******************************************************************************
*    Prototype Declare Section
*******************************************************************************/

//for LCD show
static BalDispDevContextT mDevContext;

#ifdef __cplusplus
extern "C"
{
#endif


void bal_trace(char* string);
void bal_printf( const char* fmt, ...);
void bal_vprintf(const char* fmt, va_list args);
void traceOnLCD(int x,int y,char* str);
void balOriginateCall(uint8 *NumberP, uint16 NumberLen);
extern /*const*/ ExeTaskCbT     *ExeTaskCb[];
extern NU_MEMORY_POOL  ExeSystemMemory;
#define NU_Thread_Id get_NU_Task_HISR_Pointer()
uint32 get_NU_Task_HISR_Pointer();

int BalRssiGet( );
int BalGprsAttach();

/*=================================================================================
FUNCTION:
BalVersionGet
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
get current system version.
ARGUMENTS PASSED: void
RETURN VALUE: const char*
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
const char *strValVersionGet( );

/*=================================================================================
FUNCTION:
strValIntVersionGet
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
get current system version.
ARGUMENTS PASSED: void
RETURN VALUE: const char*
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
const char *strValIntVersionGet( );

/*=================================================================================
FUNCTION:
BalGetTimeTable
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
get the Rtc time.
ARGUMENTS PASSED: BalTimeTableT*
RETURN VALUE: bool
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool BalGetTimeTable(BalTimeTableT *pstBalTimeTable);

/*=================================================================================
FUNCTION:
BalSetTimeTable
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
set the Rtc time.
ARGUMENTS PASSED: BalTimeTableT*
RETURN VALUE: bool
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool BalSetTimeTable(BalTimeTableT *pstBalTimeTable);

/*=================================================================================
FUNCTION:
ProcessHwdRssiEvent
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
for Process Hwd RssiMsg and to notify ui level.
ARGUMENTS PASSED: void* , int
RETURN VALUE: void
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void ProcessHwdRssiEvent(void* pvData, int data_size);

/*=================================================================================
FUNCTION:
BalGetIccId
CREATE DATE:
2008-4-3
AUTHOR: 
Broad Zou
DESCRIPTION:
get the ata sim iccid.
ARGUMENTS PASSED: void
RETURN VALUE: const char *
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
const char *BalGetIccId(void);

/*=================================================================================
FUNCTION:
BalGetImsi
CREATE DATE:
2008-4-3
AUTHOR: 
Broad Zou
DESCRIPTION:
get the ata sim imsi.
ARGUMENTS PASSED: void
RETURN VALUE: const char *
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
const char *BalGetImsi(void);

/*=================================================================================
FUNCTION:
BalGetDeviceSn
CREATE DATE:
2008-5-29
AUTHOR: 
Broad Zou
DESCRIPTION:
get the device serial number.
ARGUMENTS PASSED: void
RETURN VALUE: const char *
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
const char *BalGetDeviceSn(void);

/*=================================================================================
FUNCTION:
BalGetModelNum
CREATE DATE:
2008-5-29
AUTHOR: 
Broad Zou
DESCRIPTION:
get the model number.
ARGUMENTS PASSED: void
RETURN VALUE: const char *
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
const char *BalGetModelNum(void);


/*=================================================================================
FUNCTION:
BalBattRegister
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
for app to register a notify callback to val level.
ARGUMENTS PASSED: BalEventFunc
RETURN VALUE: RegIdT
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
RegIdT BalRssiRegister(BalEventFunc EventFunc );

/*=================================================================================
FUNCTION:
BalRssiUnRegister
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
for app to unregister a notify callback to val level.
ARGUMENTS PASSED: RegIdT
RETURN VALUE: bool
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool BalRssiUnRegister( RegIdT RegId );

/*=================================================================================
FUNCTION:
BalCloseFileSystem
CREATE DATE:
2008-4-3
AUTHOR: 
Broad Zou
DESCRIPTION:
close reliance file system.
ARGUMENTS PASSED: void
RETURN VALUE: void
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void BalCloseFileSystem();

/*=================================================================================
FUNCTION:
BalSimCardPlugIn
CREATE DATE:
2008-4-3
AUTHOR: 
Broad Zou
DESCRIPTION:
get the sim card status.
ARGUMENTS PASSED: void
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool BalSimCardPlugIn(void);

int BalGetTimeZone(void);
void BalSetTimeZone(int);
bool BalSetPassword(char *password);
char *BalGetPassword(void);
int BalGetPasswordTimes(void);
bool BalSetPasswordTimes(int entertimes);
void BalSetPhoneLock(int flag);
int BalGetPhoneLock(void);
void BalSetSecurityMode(int flag);
int BalGetSecurityMode(void);
void BalSetTimeMode(int flag);
int BalGetTimeMode(void);
bool BalPasswordMatch(char *password);
void BalSetTestMode(int flag);
int BalGetTestMode(void);

#ifdef BAL_PAY_MODE
void BalSetPayMode(int flag);
int BalGetPayMode(void);
#endif
void BalSetFontMode(int flag);
int BalGetFontMode(void);
int BalGetGCFStatus(void);

void BalSetRingToneType(int ringtonetype);
int BalGetRingToneType(void);
void BalSetVolumnType(int volumntype);
int BalGetVolumnType(void);
void BalSetVibrateType(int vibratetype);
int BalGetVibrateType(void);
void BalSetRepeatMode(int repeatmode);
int BalGetRepeatMode(void);
void BalNewVibratorPlay(int vibrator);
void BalNewTonePlay(int ringtonetype, int volumn);
void BalSetTonePlayEndFlag(bool flag);
bool BalGetTonePlayEndFlag(void);
bool BalLowBatteryPowerOff(void);

//Add Flight Mode by zhihui 08-10-10    Begin
#ifdef BAL_FLIGHT_MODE
/*=================================================================================
FUNCTION:
BalSetFlightMode
CREATE DATE:
2008-10-10
AUTHOR: 
Wang Zhihui
DESCRIPTION:
Set Flight Mode.
ARGUMENTS PASSED: int
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool BalSetFlightMode(int);

/*=================================================================================
FUNCTION:
BalSetFlightMode
CREATE DATE:
2008-10-10
AUTHOR: 
Wang Zhihui
DESCRIPTION:
Get Flight Mode.
ARGUMENTS PASSED: int
RETURN VALUE: TRUE, FALSE
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
int   BalGetFlightMode();
RegIdT BalFlightModeRegister(BalEventFunc EventFunc );
bool BalFlightModeUnRegister(RegIdT RegId);
void ProcessPSWFlightModeEvent(int msgId, void* pvData, int data_size);
#endif	//BAL_FLIGHT_MODE
//Add Flight Mode by zhihui 08-10-10    End

/*=================================================================================
FUNCTION:
BalGetLacCid
CREATE DATE:
2008-12-05
AUTHOR: 
Wang Zhihui
DESCRIPTION:
Get lac and cid.
ARGUMENTS PASSED: unsigned short*
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
//Add Get lac and cid interface by zhihui 08-12-05 Begin
void BalGetLacCid(unsigned short* lac, unsigned short* cid);
//Add Get lac and cid interface by zhihui 08-12-05 End


//Add PLMN by zhihui
#ifdef BAL_SELECT_PLMN
/*=================================================================================
FUNCTION:
BalGetPlmnList
CREATE DATE:
2008-12-20
AUTHOR: 
Wang Zhihui
DESCRIPTION:
Get plmn list interface.
ARGUMENTS PASSED:None
RETURN VALUE: BAL_PLMN_RETURN
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
BAL_PLMN_RETURN BalGetPlmnList();

/*=================================================================================
FUNCTION:
BalSelectPlmn
CREATE DATE:
2008-12-20
AUTHOR: 
Wang Zhihui
DESCRIPTION:
Select plmn interface.
ARGUMENTS PASSED: SelectPlmnNum is select operater number; SelectPlmnMode is select mode
RETURN VALUE: BAL_PLMN_RETURN
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
BAL_PLMN_RETURN BalSelectPlmn(unsigned int SelectPlmnNum, int SelectPlmnMode);

/*=================================================================================
FUNCTION:
BalPlmnRegister
CREATE DATE:
2008-12-20
AUTHOR: 
Wang Zhihui
DESCRIPTION:
for app to register a notify callback to val level.
ARGUMENTS PASSED: BalEventFunc
RETURN VALUE: RegIdT
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
RegIdT BalPlmnRegister(BalEventFunc EventFunc );

/*=================================================================================
FUNCTION:
BalPlmnUnRegister
CREATE DATE:
2008-12-20
AUTHOR: 
Wang Zhui
DESCRIPTION:
for app to unregister a notify callback to val level.
ARGUMENTS PASSED: RegIdT
RETURN VALUE: bool
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool BalPlmnUnRegister(RegIdT RegId);

/*=================================================================================
FUNCTION:
ProcessPSWPlmnEvent
CREATE DATE:
2008-12-20
AUTHOR: 
Wang Zhihui
DESCRIPTION:
Process plmn event.
ARGUMENTS PASSED: int, void* , int
RETURN VALUE: void
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void ProcessPSWPlmnEvent(int msgId, void* pvData, int data_size);
const char* BalGetOperName();
#endif	//BAL_SELECT_PLMN

/*=================================================================================
FUNCTION:
BalGetNetworkDaylightSaveTime
CREATE DATE:
2009-05-07
AUTHOR: 
Fan Xujun
DESCRIPTION:
Get the network daylight save time flag.
ARGUMENTS PASSED: void
RETURN VALUE: unsigned char
USED GLOBAL VARIABLES: 
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
If network don't support network time or daylight time, the default return value is 0
=================================================================================== */
unsigned char BalGetNetworkDaylightSaveTime();

/*=================================================================================
FUNCTION:
BalSetNetworkDaylightSaveTime
CREATE DATE:
2009-05-07
AUTHOR: 
Fan Xujun
DESCRIPTION:
Set the network daylight save time flag.
ARGUMENTS PASSED: void
RETURN VALUE: unsigned char
USED GLOBAL VARIABLES: 
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
The network value will displace the setting value when change to network time mode 
=================================================================================== */
void BalSetNetworkDaylightSaveTime(unsigned char time);

#ifdef __cplusplus
}
#endif

#endif
