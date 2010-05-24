
#ifndef KPD_DRV_IF_H
#define KPD_DRV_IF_H


#include "sysdefs.h"
#include "sysKeydef.h"

/* define hardware status for key */
typedef enum
{
    HWD_KEY_PRESS,
    HWD_KEY_RELEASE
}HwdKeyStatusT;

typedef void (*HwdKeypadCallbackFuncT)(HwdKeyStatusT Status, SysKeyIdT KeyId);
/*****************************************************************************
    FUNCTION NAME: HwdKeypadInit

    DESCRIPTION:
        This method is used to initialize keypad hardware(include extern keys out
        of keypad).

    PARAMETERS:
        Column          if it is 5, then the keypad in CBP is 6 x 5.
                        other value, the the keypad in CBP is 6 x 4.

    RETURNED VALUES:
        None.
*****************************************************************************/
void HwdKeypadInit(uint8 Column, HwdKeypadCallbackFuncT CallbackFuncP);   
              
/*****************************************************************************
    FUNCTION NAME: HwdKeypadGetFlipKeyStatus

    DESCRIPTION:
        This method is to get the hardware status of flip key.

    PARAMETERS:
        None.

    RETURNED VALUES:
        None.

    Caution:
        do not change the function name and prototype. it is refrenced  
        by valkeypad.c
*****************************************************************************/
HwdKeyStatusT HwdKeypadGetFlipKeyStatus(void);

/*****************************************************************************
    FUNCTION NAME: HwdKeypadNotify

    DESCRIPTION:
        The method is to send message to hwd task.

    PARAMETERS:
        None

    RETURNED VALUES:
        None.
*****************************************************************************/
void HwdKeypadNotify(HwdKeyStatusT KeyStatus, SysKeyIdT KeyId); 

void HwdKeypadSetTtyState(bool On_Off);

SysKeyIdT MapKeyEtsToSys(EtsKeyIdT EtsKeyId);

void HwdKypadRotCB(KPD_ROT_DIRECTION_T direction);


#endif // KPD_DRV_IF_H

