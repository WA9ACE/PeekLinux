


#ifndef _HWDACCAPI_H_
#define _HWDACCAPI_H_

#include "exeapi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	HWD_ACC_CHARGER = 0,
	HWD_ACC_BATTERY,
	HWD_ACC_HEADSET,
    HWD_ACC_USB_CABLE,
    HWD_ACC_USB_CHARGER,
	HWD_ACC_DATA_CABLE,
    HWD_ACC_SD_CARD,
	HWD_ACC_TEST_CABLE,

	HWD_ACC_MAX_ID

} HwdAccDevIdT;

typedef enum
{
	HWD_ACC_CONNECTED = 0,
	HWD_ACC_REMOVED,

	HWD_ACC_STATE_NUM

} HwdAccStateT;

typedef struct 
{
	HwdAccDevIdT	DevId;
    HwdAccStateT    DevState;
} HwdAccEventMsgT;

typedef struct
{
    ExeTaskIdT      TaskId;
    ExeMessageT     SyncMailbox;
    ExeMailboxIdT   SyncMailboxId;
    ExeMailboxIdT   NotifyMailboxId;
    uint32          NotifyMsgId;
} HwdAccCommParamT;

extern bool HwdUsbCableExist(void);
extern bool HwdUsbCableInit(HwdAccCommParamT *param);
extern bool HwdHeadsetInit(HwdAccCommParamT *param);
extern bool HwdHeadsetExist(void);
extern void HwdHeadsetKeyProcess(uint16 AdcResult);
extern bool HwdBattChargerInit(HwdAccCommParamT *param);
extern bool HwdBattChargerExist(void);  /*Charger is plugged in and voltage is valid.*/
extern bool HwdBattChargerIsPlugged(void);  /*Charger is plugged in.*/
extern void HwdChargerNotify(HwdAccStateT State);
extern void HwdUsbCableNotify(HwdAccStateT State);
extern bool HwdUsbDevInit(void);
extern void HwdHeadsetKeyEnable(bool Enable);

#ifdef __cplusplus
}
#endif

#endif /*_HWDACCAPI_H_*/


