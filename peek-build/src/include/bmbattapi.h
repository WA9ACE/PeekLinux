
#ifndef _BM_BATTAPI_H_

#define _BM_BATTAPI_H_

#include "sysdefs.h"
#include "hwdbatt.h"
#ifdef __cplusplus
extern "C"
{
#endif

typedef enum  
{
    BM_BATT_LEVEL_0 = 0,
    BM_BATT_LEVEL_1,
    BM_BATT_LEVEL_2,
    BM_BATT_LEVEL_3,
    BM_BATT_LEVEL_4,
    BM_BATT_LEVEL_5,
    BM_BATT_LEVEL_6,
    BM_BATT_LEVEL_7,
    BM_BATT_LEVEL_8,
    BM_BATT_LEVEL_9,//Add by #zouyonghang
    BM_BATT_LEVEL_NUM,
    
    BM_BATT_LEVEL_NO_BATTERY= BM_BATT_LEVEL_0,
    BM_BATT_LEVEL_SHUTDOWN  = BM_BATT_LEVEL_1,
    BM_BATT_LEVEL_TOO_LOWEST  = BM_BATT_LEVEL_2,//Add by #zouyonghang
    BM_BATT_LEVEL_TOO_LOW   = BM_BATT_LEVEL_3,
    BM_BATT_LEVEL_LOW       = BM_BATT_LEVEL_4,
    BM_BATT_LEVEL_MID       = BM_BATT_LEVEL_5,
    BM_BATT_LEVEL_MIDHIGH       = BM_BATT_LEVEL_6,
    BM_BATT_LEVEL_HIGH      = BM_BATT_LEVEL_7,
    BM_BATT_LEVEL_FULL      = BM_BATT_LEVEL_8,
    BM_BATT_LEVEL_NOT_INIT = BM_BATT_LEVEL_9
} BmBattLevelT;

typedef enum 
{
    BM_DEVICE_FLAG_LCD      = 0x0001,   /*idle mode LCD IS ON*/
    BM_DEVICE_FLAG_KEYPAD   = 0x0002,
    BM_DEVICE_FLAG_TX       = 0x0004,
    BM_DEVICE_FLAG_RX       = 0x0008,
    BM_DEVICE_FLAG_RINGER   = 0x0010,
    BM_DEVICE_FLAG_MP3      = 0x0020,
    BM_DEVICE_FLAG_BB       = 0x0040,
    BM_DEVICE_FLAG_7        = 0X0080,   /* items hereafter are reserved for future use. */
    BM_DEVICE_FLAG_8        = 0X0100,
    BM_DEVICE_FLAG_9        = 0X0200,
    BM_DEVICE_FLAG_10       = 0X0400,
    BM_DEVICE_FLAG_11       = 0X0800,
    BM_DEVICE_FLAG_12       = 0X1000,
    BM_DEVICE_FLAG_13       = 0X2000,
    BM_DEVICE_FLAG_14       = 0X4000,
    BM_DEVICE_FLAG_15       = 0X8000
} BmDevFlagT;                /* each flag correspond to a device module, for example mp3 */

typedef enum
{
    BM_DEV_CONNECTED = 0,
    BM_DEV_REMOVED,
    BM_CHARGE_STATE,
    BM_BATT_VOLTAGE
} BmEventIdT;

typedef enum
{
    BM_DEV_CHARGER = 0,
    BM_DEV_BATTERY
} BmDevId;

typedef enum
{
    BM_CHARGE_STOP = 0,
    BM_CHARGE_PAUSE,
    BM_CHARGE_ONGOING,
    BM_CHARGE_FAILED,
    BM_CHARGE_COMPLETE
} BmChargeStateT;

/* API */

extern void             BmBattInit(void);

extern BmBattLevelT     BmGetBattLevel(void);
extern void             BmBattSetDevFlag(BmDevFlagT DevFlag, bool On);
extern BmChargeStateT   BmGetChargeStatus(void);

/* API from BmBatt.c */
extern void             BmBattSetGaugeInterval(uint16 Seconds);
extern void             BmDisableCharge(void);
extern void             BmEnableCharge(void);
extern bool             BmBatteryExist(void);
extern bool             BmChargerExist(void);
extern uint16           BmBattGetVolt(void);
extern HwdBattTypeT          BmBattGetBattType(void);
extern void BmBattSetNHPulseAndRate(uint16 MiliSeconds, uint16 Rate);
#ifdef __cplusplus
}
#endif

#endif

