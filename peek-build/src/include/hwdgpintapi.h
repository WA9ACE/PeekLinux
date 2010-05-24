

#ifndef _HWDGPINTAPI_H_
#define _HWDGPINTAPI_H_

#include "hwddefs.h"


/*-------------------------------------------------------------------------------------
*Macro defines
*-------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------
*Data structure defines
*-------------------------------------------------------------------------------------*/

typedef enum
{
    HWD_GPINT_0 = 0,
    HWD_GPINT_1,
    HWD_GPINT_2,
    HWD_GPINT_3,
    HWD_GPINT_4,
    HWD_GPINT_5,
    HWD_GIINT_0,
    HWD_GIINT_1,
    HWD_GIINT_2,
    HWD_GIINT_3,
    HWD_GIINT_4,
    HWD_GIINT_5,
    HWD_GIINT_6,
    HWD_GIINT_7,
    HWD_GIINT_8,
    HWD_GIINT_9,
    HWD_GIINT_10,
    HWD_GIINT_11,
    HWD_GIINT_12,
    HWD_GIINT_13,
    HWD_GIINT_14,
    HWD_GPINT_MAX
}HwdGpIntIdT;

typedef void  (*HwdGpIntHisrFunc)  (HwdGpIntIdT GpIntId);


/*-------------------------------------------------------------------------------------
*HWD Gpint APIs defines
*-------------------------------------------------------------------------------------*/

/*****************************************************************************
  FUNCTION NAME: HwdGpIntRegister

  DESCRIPTION:

    Registor a HISR function for a defined GPINT. In this function, HwdSleepWakeupEventRegister() is called
    to set deep sleep wakeup when the defined GPINT interrupt occurs. If one GPINT interrupt is registored
    use the function, In the LISR function of the GPINT, the interrupt of the GPINT will been disabled and cleared,
    then active the HISR in hwdgpint.c. 
    If nTimerMs is zero, In HISR function, call HisrFunc function. When the caller finish all 
    operation for the GPINT interrupt, it should call HwdGpintIntClr() and HwdGpintIntEnable() to clear and 
    enable the GPINT interrupt again, and call HwdGpintIntPolSet() function to set interrupt porlarity.
    If nTimerMs isn't zero, In HISR function, debounce timer will start. In debounce timer handler function,
    it clear and enable interrupt, and set interrupt porlarity. Then it call HisrFunc function. 

  PARAMETERS:
     
    GpintId:    Describe the a GPINT ID. It must is a menber of HwdGpintIdT, otherwise will return FALSE.
    HisrFunc:   A function run in GPINT or timer HISR. It is defined by caller. Notice, it run in HISR stack and
                 must return rapidly!
    DebounceTimeMS:     Debounce timer interrupt time. It's unit is millisecond. If it less than 10, the routine will set
                        the value as 10. Because the minium timer time is 10ms. If it is zero, no debounce timer.
                 
  RETURNED VALUES:

    TRUE, success for registoring HISR function. FALSE, fail to registor, the GPINT interrupt doesn't work.
 
*****************************************************************************/
bool HwdGpIntRegister(HwdGpIntIdT GpIntId, HwdGpIntHisrFunc HisrFunc, uint16 DebounceTimeMS);


/*****************************************************************************
  FUNCTION NAME: HwdGpIntEnable

  DESCRIPTION:

    Enable a defined GPINT interrupt.

  PARAMETERS:
      
    GpIntId: Describe the a GPINT ID. It must is a menber of HwdGpintIdT, otherwise will doing nothing.
                 
  RETURNED VALUES:

    None
 
*****************************************************************************/
void HwdGpIntEnable(HwdGpIntIdT GpIntId);

/*****************************************************************************
  FUNCTION NAME: HwdGpIntDisable

  DESCRIPTION:

    Disable a defined GPINT interrupt.

  PARAMETERS:
      
    GpIntId: Describe the a GPINT ID. It must is a menber of HwdGpintIdT, otherwise will doing nothing.
                 
  RETURNED VALUES:

    None
 
*****************************************************************************/
void HwdGpIntDisable(HwdGpIntIdT GpIntId);


/*****************************************************************************
  FUNCTION NAME: HwdGpIntClr

  DESCRIPTION:

    Clear a defined GPINT interrupt.

  PARAMETERS:
      
    GpIntId: Describe the a GPINT ID. It must is a menber of HwdGpintIdT, otherwise will doing nothing.
                 
  RETURNED VALUES:

    None
 
*****************************************************************************/
void HwdGpIntClr(HwdGpIntIdT GpIntId);


/*****************************************************************************
  FUNCTION NAME: HwdGpIntSetPol

  DESCRIPTION:

    Set interrupt trigger polarity of a defined GPINT.

  PARAMETERS:
      
    GpIntId: Describe the a GPINT ID. It must is a menber of HwdGpintIdT, otherwise will doing nothing.
    bPolarity: True, set positive edge triggered interrupt. False, set negative edge triggered interrupt.
                 
  RETURNED VALUES:

    None
 
*****************************************************************************/
void HwdGpIntSetPol(HwdGpIntIdT GpIntId, bool Polarity);


#endif


