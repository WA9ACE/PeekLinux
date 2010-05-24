

#ifndef BALACCAPI_H
#define BALACCAPI_H



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "sysdefs.h"
#include "exeapi.h"
#include "balmiscapi.h"

#define CAT_ACC(id)  (0x00A00000 | ((id) & 0xFFFF))
typedef enum 
{
    BAL_ACC_STATUS_REMOVED = 0,
    BAL_ACC_STATUS_CONNECTED,
	
    BAL_ACC_STATUS_NUM
}BalAccStatusT;

typedef enum 
{
    BAL_ACC_CHARGER=0,
    BAL_ACC_BATTERY,
    BAL_ACC_HEADSET,
    BAL_ACC_DATA_CABLE,
    BAL_ACC_USB_CABLE,
    BAL_ACC_SD_CARD,
  
    BAL_ACC_ID_NUM = 16
} BalAccIdT;

/*****************************************************************************
 
  FUNCTION NAME: BalAccInit

  DESCRIPTION:

    Initialize the variables necessary before the send msg to Acc.

  PARAMETERS:
 
    argc      - Not used, will be remove if necessary
    argv      - Not used now

  RETURNED VALUES:

    None.
 
*****************************************************************************/
 void BalAccInit (void);

/*****************************************************************************
 
  FUNCTION NAME: AccRegister

  DESCRIPTION:

    This registers the callback funtion in Accessory event handler

  PARAMETERS:
 
    EventFunc   - Function to register for accessory msgs.

  RETURNED VALUES:

    RegIdT
 
*****************************************************************************/
RegIdT BalAccRegister( BalEventFunc EventFunc );

/*****************************************************************************
 
  FUNCTION NAME: AccUnRegister

  DESCRIPTION:

    This unregisters the callback funtion in Accessory event handler

  PARAMETERS:
 
    RegId  - ID registered in VAL

  RETURNED VALUES:

    RegIdT
 
*****************************************************************************/
void BalAccUnRegister( RegIdT RegId );

/*****************************************************************************
 
  FUNCTION NAME: BalAccStatusNotify

  DESCRIPTION:

    The other module use this API to notify the BAL accessory module that the accessory status
    changed.

  PARAMETERS:
       AccDev - The accessory device ID.
       AccStatus - The device's current status.
 
  RETURNED VALUES:
      If the device is valid, the TRUE returned, or the FALSE returned.

*****************************************************************************/
bool BalAccStatusNotify(BalAccIdT AccDev, BalAccStatusT AccStatus);

/*****************************************************************************
 
  FUNCTION NAME: BalAccGetState

  DESCRIPTION:

    Get the current state of the specified device.

  PARAMETERS:
       AccDev - The accessory device ID.
 
  RETURNED VALUES:
      The device's status, connected or removed.

*****************************************************************************/
BalAccStatusT BalAccGetStatus(BalAccIdT AccDev);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif




