#ifndef __TI_CHIP_MNGR_H
#define __TI_CHIP_MNGR_H

#include "bthal_types.h"
#include "hci.h"

typedef enum
{
	TI_CHIP_MNGR_STATUS_SUCCESS,
	TI_CHIP_MNGR_STATUS_FAILED,
	TI_CHIP_MNGR_STATUS_PENDING,
	TI_CHIP_MNGR_STATUS_IN_PROGRESS,
	TI_CHIP_MNGR_STATUS_INTERNAL_ERROR
} TiChipMngrStatus;

typedef enum
{
	TI_CHIP_MNGR_STATE_CHIP_OFF,
	TI_CHIP_MNGR_STATE_BT_ON,
	TI_CHIP_MNGR_STATE_FM_ON,
	TI_CHIP_MNGR_STATE_ALL_ON
} TiChipMngrState;

typedef enum
{	
	TI_CHIP_MNGR_POWER_NOTIFICATION_UART_INITIALIZED,
	TI_CHIP_MNGR_POWER_NOTIFICATION_CHIP_SHUT_DOWN
} TiChipMngrPowerNotificationType;

typedef enum
{	
	TI_CHIP_MNGR_BT_NOTIFICATION_BT_ON_COMPLETE,
	TI_CHIP_MNGR_BT_NOTIFICATION_BT_ON_ABORT,
	TI_CHIP_MNGR_BT_NOTIFICATION_BT_OFF_COMPLETE
} TiChipMngrBtNotificationType;

typedef enum
{	
	TI_CHIP_MNGR_FM_NOTIFICATION_FM_ON_COMPLETE,
	TI_CHIP_MNGR_FM_NOTIFICATION_FM_OFF_COMPLETE
} TiChipMngrFmNotificationType;

typedef enum
{
	TI_CHIP_MNGR_RADIO_NOTIFICATION_HCI_INIT_STATUS,
	TI_CHIP_MNGR_RADIO_NOTIFICATION_HCI_DEINIT_STATUS,
	TI_CHIP_MNGR_RADIO_NOTIFICATION_HCI_UART_INITIALIZED,
	TI_CHIP_MNGR_RADIO_NOTIFICATION_HCI_UART_DEINITIALIZED
} TiChipMngrRadioNotificationType;

typedef void (*TiChipMngrStateNotificationsCb)(TiChipMngrState newState);
typedef void (*TiChipMngrPowerNotificationsCb)(TiChipMngrPowerNotificationType notification);
typedef void (*TiChipMngrBtNotificationsCb)(TiChipMngrBtNotificationType notification);
typedef void (*TiChipMngrFmNotificationsCb)(TiChipMngrFmNotificationType notification);

TiChipMngrStatus TI_CHIP_MNGR_Init(void);
TiChipMngrStatus TI_CHIP_MNGR_Deinit(void);

TiChipMngrStatus TI_CHIP_MNGR_BTOn(void);
TiChipMngrStatus TI_CHIP_MNGR_BTOff(void);
TiChipMngrStatus TI_CHIP_MNGR_BTOnAbort(void);

TiChipMngrStatus TI_CHIP_MNGR_FMOn(void);
TiChipMngrStatus TI_CHIP_MNGR_FMOff(void);

TiChipMngrStatus TI_CHIP_MNGR_RegisterForPowerNotifications(TiChipMngrPowerNotificationsCb callback);
TiChipMngrStatus TI_CHIP_MNGR_RegisterForStateNotifications(TiChipMngrStateNotificationsCb callback);
TiChipMngrStatus TI_CHIP_MNGR_RegisterForBTNotifications(TiChipMngrBtNotificationsCb callback);
TiChipMngrStatus TI_CHIP_MNGR_RegisterForFMNotifications(TiChipMngrFmNotificationsCb callback);

TiChipMngrState TI_CHIP_MNGR_GetState(void);
const char *TI_CHIP_MNGR_StateAsStr(TiChipMngrState state);

void TI_CHIP_MNGR_RadioCallback(TiChipMngrRadioNotificationType notificationType, void *parms);

#endif	/* __TI_CHIP_MNGR_H */

