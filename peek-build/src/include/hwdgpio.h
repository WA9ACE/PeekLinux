#ifndef _HWDGPIO_H_
#define _HWDGPIO_H_



#include "sysdefs.h"


#undef HWD_GPIO_TEST

/* GPIO Numbers */
typedef enum
{
   HWD_GPIO_0    = 0,
   HWD_GPIO_1,           
   HWD_GPIO_2,           
   HWD_GPIO_3,           
   HWD_GPIO_4,           
   HWD_GPIO_5,           
   HWD_GPIO_6,           
   HWD_GPIO_7,           
   HWD_GPIO_8,           
   HWD_GPIO_9,           
   HWD_GPIO_10,          
   HWD_GPIO_11,          
   HWD_GPIO_12,          
   HWD_GPIO_13,          
   HWD_GPIO_14,          
   HWD_GPIO_15,          
   HWD_GPIO_16,          
   HWD_GPIO_17,          
   HWD_GPIO_18,          
   HWD_GPIO_19,          
   HWD_GPIO_20,          
   HWD_GPIO_21,          
   HWD_GPIO_22,          
   HWD_GPIO_23,          
   HWD_GPIO_24,          
   HWD_GPIO_25,          
   HWD_GPIO_26,          
   HWD_GPIO_27,
   HWD_GPIO_28,
   HWD_GPIO_29,
   HWD_GPIO_30,
   HWD_GPIO_31,
   HWD_GPIO_32,
   HWD_GPIO_33,
   HWD_GPIO_34,
   HWD_GPIO_35,
   HWD_GPIO_36,
   HWD_GPIO_37,
   HWD_GPIO_38,
   HWD_GPIO_39,
   HWD_GPIO_40,
   HWD_GPIO_41,
   HWD_GPIO_42,
   HWD_GPIO_43,
   HWD_GPIO_44,
   HWD_GPIO_45,
   HWD_GPIO_46,
   HWD_GPIO_47,
    HWD_GPIO_GI_0,
    HWD_GPIO_GI_1,
    HWD_GPIO_GI_2,
    HWD_GPIO_GI_3,
    HWD_GPIO_GI_4,
    HWD_GPIO_GI_5,
    HWD_GPIO_GI_6,
    HWD_GPIO_GI_7,
    HWD_GPIO_GI_8,
    HWD_GPIO_GI_9,
    HWD_GPIO_GI_10,
    HWD_GPIO_GI_11,
    HWD_GPIO_GI_12,
    HWD_GPIO_GI_13,
    HWD_GPIO_GI_14,
   HWD_GPO_5BIT_0,
   HWD_GPO_5BIT_1,
   HWD_GPO_5BIT_2,
   HWD_GPO_5BIT_3,
   HWD_GPO_5BIT_4,
   HWD_GPIO_NUM   /* max number of GPIOs */
} HwdGpioT;

typedef enum { 
	HWD_GROUP_5BIT, 
	HWD_GROUP_7BIT, 
	HWD_GROUP_8BIT, 
	HWD_GPIO_GROUP_NUM
}GpioGroupT;

/*****************************************************************************
 
  FUNCTION NAME: HwdGpioFastClear

  DESCRIPTION:

    This macro clears a specified GPIO register to zero.  

    This should only be used in special cases when the time taken by HwdGpioClear()
    is determined to be excessive. 
    
    THIS MACRO ASSUMES THE GPIO MODE HAS BEEN PREVIOUSLY SET TO NORMAL AND THE
    DIRECTION TO OUTPUT.
    
    

  PARAMETERS:

    GpioReg  -  GPIO register address of the GPIO to clear.
                NOTE : This is NOT the same as the HwdGpioT enum.
    
  RETURNED VALUES:

    None

*****************************************************************************/
#define HwdGpioFastClear(GpioReg) HwdWrite(GpioReg,0)

/*****************************************************************************
 
  FUNCTION NAME: HwdGpioFastSet

  DESCRIPTION:

    This macro sets a specified GPIO register to one.

    This should only be used in special cases when the time taken by HwdGpioClear()
    is determined to be excessive. 
    
    THIS MACRO ASSUMES THE GPIO MODE HAS BEEN PREVIOUSLY SET TO NORMAL AND THE
    DIRECTION TO OUTPUT.
    
    

  PARAMETERS:

    GpioReg  -  GPIO register address of the GPIO to set.
                NOTE : This is NOT the same as the HwdGpioT enum.
    
  RETURNED VALUES:

    None

*****************************************************************************/
#define HwdGpioFastSet(GpioReg)   HwdWrite(GpioReg,1)

/*****************************************************************************
 
  FUNCTION NAME: HwdGpioMonitor

  DESCRIPTION:

    This routine reads GPIO without changing its direction and
	also returns the direction of the GPIO in the second byte:
	xxxxxxxD xxxxxxxV, where D is a direction bit -
	GPIO_OUTPUT=0, GPIO_INPUT=1 (direction); and V is a value
	Return Value of 0xffff means GPIO is not in use

  PARAMETERS:

    GpioNum  -  GPIO number to read
    
  RETURNED VALUES:

    None

*****************************************************************************/
extern uint32 HwdGpioMonitor(uint32 GpioNum);

/*****************************************************************************
 
  FUNCTION NAME: HwdGpioClear

  DESCRIPTION:

    This routine clears a specified GPIO register to zero.

  PARAMETERS:

    GpioNum  -  GPIO number to clear
    
  RETURNED VALUES:

    None

*****************************************************************************/

extern void HwdGpioClear(uint32 GpioNum);

/*****************************************************************************
 
  FUNCTION NAME: HwdGpioSet

  DESCRIPTION:

    This routine sets a specified GPIO register to one.

  PARAMETERS:

    GpioNum  - GPIO number to set
    
  RETURNED VALUES:

    None

*****************************************************************************/

extern void HwdGpioSet(uint32 GpioNum);

/*****************************************************************************
 
  FUNCTION NAME: HwdGpioRead

  DESCRIPTION:

    This routine reads a specified GPIO register and returns its
    value.

  PARAMETERS:

    GpioNum  - GPIO number to read
    
  RETURNED VALUES:

    bool     - TRUE if it is set, FLASE if it is not set

*****************************************************************************/

extern bool HwdGpioRead(uint32 GpioNum);

/*****************************************************************************
 
  FUNCTION NAME: HwdGpioToggle

  DESCRIPTION:

    This routine toggles a specified GPIO register.

  PARAMETERS:

    GpioNum  - GPIO number to toggle
    
  RETURNED VALUES:

    None

*****************************************************************************/

extern void HwdGpioToggle(uint32 GpioNum);

/*****************************************************************************
 
  FUNCTION NAME: HwdGpioInit

  DESCRIPTION:

    This routine configures all GPIOs to GPIO mode or functional mode.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/

void HwdGpioInit(void);


/*****************************************************************************
 
  FUNCTION NAME: HwdGpioDeepSlpPrep

  DESCRIPTION:

    This routine configures all GPIOs to GPIO mode and as inputs for power 
    saving purpose. It is called when the processor is ready for the deep sleep.
    The pins need to restore to original configuration ASAP
    after waking up from the deep sleep.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/
extern void HwdGpioDeepSlpPrep(void);


/*****************************************************************************
 
  FUNCTION NAME: HwdGpioRestore

  DESCRIPTION:

    This routine restores all GPIOs as functional or GPIO. It restores the 
    directions of all GPIOs if they are configurated as I/Os.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/
extern void HwdGpioRestore(void);

/*****************************************************************************
 
  FUNCTION NAME: HwdGroupGpioRead

  DESCRIPTION:

	This procedure reads a word from group GPIO. It also updates the 
	whole group direction to Input state. According to group type - 
	3/5/7/8-bit group - it reads appropriate number of bits. In the case 
	the 3-bit group is generally in use this routine reads 6 bit from 7 or 
	8-bit GPIO.

  PARAMETERS:

    Group  - GPIO group type

  
  RETURNED VALUES:

    Returns read value

*****************************************************************************/
extern uint32 HwdGroupGpioRead(GpioGroupT Group);

/*****************************************************************************
 
  FUNCTION NAME: HwdGroupGpioWrite

  DESCRIPTION:

	This procedure writes a word into group GPIO. It also updates the whole 
	group direction to Output state. According to group type - 3/5/7/8-bit 
	group - it writes appropriate number of bits. In the case the 3-bit group 
	is generally in use this routine writes 6 bit from 7 or 8-bit GPIO. It also 
	treats the direction register accordingly.

  PARAMETERS:

    Group  - GPIO group type
	GpioNmGpReg - GPIO MN GP Register value
    
  RETURNED VALUES:

    None

*****************************************************************************/
extern void HwdGroupGpioWrite(GpioGroupT Group, uint32 GpioValue);

/*****************************************************************************
 
  FUNCTION NAME: HwdGroupGpioInit

  DESCRIPTION:

    This routine inits all GPIO groups.

  PARAMETERS:

	None
    
  RETURNED VALUES:

    None

*****************************************************************************/
extern void HwdGroupGpioInit(void);

/*****************************************************************************
 
  FUNCTION NAME: HwdDspmvGpioInit

  DESCRIPTION:

    This routine configures all DSPM/DSPV GPIOs to GPIO mode or functional mode 
    and initialized the directions of DSPM/DSPV GPIOs (1=read, 0=write) with mask
	bits 0:1 use to set direction for DSPM GPIO
	bits 2:3 use to set direction for DSPV GPIO
	bits 4:7 use to set direction of 4 GPIOs from DSPM (if bit8=0) or DSPV (if bit8=1)
	bit 8    use to define 4 GPIOs to DSPM (if = 0) or DSPV (if = 1)

  PARAMETERS:

    HwNmGpDspMVInit, GpioInitDir
    
  RETURNED VALUES:

    None

*****************************************************************************/
void HwdDspmvGpioInit(void);

/*****************************************************************************
 
  FUNCTION NAME: HwdChngDspmvGpioDir

  DESCRIPTION:

    This routine changes the directions of DSPM/DSPV GPIOs 1=read, 0=write
	bits 0:1 use to set direction for DSPM GPIO
	bits 2:3 use to set direction for DSPV GPIO
	bits 4:7 use to set direction of 4 GPIOs from DSPM (if bit8=0) or DSPV (if bit8=1)
	bit 8    use to define 4 GPIOs to DSPM (if = 0) or DSPV (if = 1)

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/
extern void HwdChngDspmvGpioDir(uint16 BitMask, bool Change2Read);



/*****************************************************************************
 
  FUNCTION NAME: HwdGpioReadCkt108

  DESCRIPTION:

    This routine reads circuit 108 status for UART1 from one of GPIO ports.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    bool   boolean state for desired port.

*****************************************************************************/
extern bool HwdGpioReadCkt108(void);

/*****************************************************************************
  FUNCTION NAME: HwdGpioWriteCkt109

  DESCRIPTION:

    This routine write a desired state to circuit 109 for UART1 to one of 
    GPIO ports.

  PARAMETERS:

    bool   logic state
    
  RETURNED VALUES:

    none

*****************************************************************************/
extern void HwdGpioWriteCkt109(bool);

extern void HwdGpioGpIntInit( void );
extern void HwdGpioNormalModeSet( HwdGpioT GpioNum );
extern void HwdGpioFunctionalModeSet( HwdGpioT GpioNum );
extern void HwdGpioInitCust(void);



#endif
