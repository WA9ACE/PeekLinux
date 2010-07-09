#ifndef _BAL_KEYPAD_H_
#define _BAL_KEYPAD_H_

#include "syskeydef.h"
//#include "balapi.h"


/* define status in BAL */
typedef enum
{
    BAL_KEY_PRESS,
    BAL_KEY_RELEASE,
    BAL_KEY_HOLD,
    BAL_KEY_STATUS_NUM
}BalKeyStatusT;

typedef PACKED struct
{
    BalKeyStatusT   Status;
    SysKeyIdT       KeyId;
}BalKeypadMsgT;

/*****************************************************************************
    FUNCTION NAME: BalKeypadInit

    DESCRIPTION:
        this method is used to initialize keypad in BAL.

    PARAMETERS:
        MultiKey        if it is TRUE, then multi key mode, otherwise single key
                        mode.

        KeyHoldDuration Key hold time, unit is 100ms. if 0, hold time is default
                        value 20. if 0xFF, there is no hold status.

        colNum          number of keypad column, now it must be either 4 or 5
    RETURNED VALUES:
        None.
*****************************************************************************/
void    BalKeypadInit(bool MultiKey, uint8 KeyHoldDuration, uint8 colNum );   /* BAL keypad init  */

/*****************************************************************************
    FUNCTION NAME: BalKeypadRegister

    DESCRIPTION:
        this method is used to register keypad service by application.

    PARAMETERS:
        EventFuncP          Callback function provided by the application.

    RETURNED VALUES:
        RegIdT = -1         Failure.
                            otherwise, successful and register id.
*****************************************************************************/
RegIdT  BalKeypadRegister(BalEventFunc EventFuncP);             /* BAL keypad register */

/*****************************************************************************
    FUNCTION NAME: BalKeypadUnregister

    DESCRIPTION:
        this method is used to un-register keypad service by application.

    PARAMETERS:
        RegId              the valid register id through BalKeypadRegister()

    RETURNED VALUES:
        None.
*****************************************************************************/
void    BalKeypadUnregister(RegIdT RegId);                      /* BAL keypad unregister */

/*****************************************************************************
    FUNCTION NAME: BalKeypadProcess

    DESCRIPTION:
        this method is to process val keypad messsage.

    PARAMETERS:
        MsgId               the id of msg
        MsgDataP            the data pointer of msg
        MsgSize             the size of msg

    RETURNED VALUES:
        None.
*****************************************************************************/
void    BalKeypadProcess(uint32 MsgId, void * MsgDataP, uint32 MsgSize);

/*****************************************************************************
    FUNCTION NAME: BalKeyGetFlipKeyStatus

    DESCRIPTION:
        this method is to get the status of the flip key. This method should 
        be called by task in initialization and after keypad init.

        This method should be call once and then all status of the key is 
        notify by register function of application.

    PARAMETERS:
        None.

    RETURNED VALUES:
        BAL_KEY_PRESS / BAL_KEY_RELEASE
*****************************************************************************/
BalKeyStatusT BalKeypadGetFlipKeyStatus (void);                 /* BAL get flip key status */
void bal_Left_shiftkeyflag();
void bal_Right_shiftkeyflag();
int bal_Get_shiftkey();

#endif

