#ifndef _HWDI2CAPI_H_
#define _HWDI2CAPI_H_



#include "exeapi.h"
#include "sysdefs.h"

/*---------------------------------------------------------------
*  Declare global I2C defines
*----------------------------------------------------------------*/
/* I2C device addresses */
#define HWD_I2C_EEPROM_DEV_ADDR       0x54
#define HWD_I2C_DAC_DEV_ADDR          0x4D

enum {
   HWD_I2C_WRITE_1_BYTE    =  1,
   HWD_I2C_WRITE_2_BYTES   =  2,
   HWD_I2C_WRITE_8_BYTES   =  8,
   HWD_I2C_WRITE_16_BYTES  =  16,
   HWD_I2C_WRITE_64_BYTES  =  64
};

/*---------------------------------------------------------------
*  Declare function prototypes for global I2C routines 
*----------------------------------------------------------------*/

/*****************************************************************************
 
  FUNCTION NAME: HwdI2cInit

  DESCRIPTION:

    This routine is the init resource routine for the I2C interface. This
    routine creates the HISR task and semaphore for the I2C interface.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/
extern void HwdI2cInit(void);

/*****************************************************************************
 
  FUNCTION NAME: HwdI2cStatus

  DESCRIPTION:

    This routine checks and returns the I2C hardware interface status.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    boolean - TRUE  - I2C interface not responding error
              FALSE - I2C interface ok

*****************************************************************************/
extern bool HwdI2cStatus(void);

/*****************************************************************************
 
  FUNCTION NAME: HwdI2cLock

  DESCRIPTION:

    This routine locks the I2C interface. 
    This needs to be done to allow the interface to be shared between multiple tasks. 
    This routine uses an Exe semaphore to control access to the I2C interface. 
    To unlock the I2C interface a call to HwdI2cUnlock must be executed.

  PARAMETERS:

    TaskId    -  Task ID of the task to notify when an I2C operation is
                 complete
    SignalFlg -  Signal flag to set when I2C operation is complete
    
  RETURNED VALUES:

    None

*****************************************************************************/
extern void HwdI2cLock(void);

/*****************************************************************************
 
  FUNCTION NAME: HwdI2cUnlock

  DESCRIPTION:

    This routine unlocks the I2C interface so it can be used by another
    task. A call to HwdI2cLock must be executed before this unlock
    routine is called or an Exe error will occur. This routine uses 
    an Exe semaphore to control access to the I2C interface.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/
extern void HwdI2cUnlock(void);

/*****************************************************************************
    
  FUNCTION NAME: HwdI2cWrite

  DESCRIPTION:

     This rouine writes a data byte to the I2C physical interface. This 
     routine sets up the I2C interface for a write operation. The actual 
     write operation occurs in the I2C LISR.

  PARAMETERS:

    Address    - I2C byte address
    DataPtr    - Pointer to where to get the data 
    NumBytes   - Number of bytes to write (options are 1, 2, 8, 16, or 64)
       HWD_I2C_WRITE_1_BYTE    =  1,
       HWD_I2C_WRITE_2_BYTES   =  2,
       HWD_I2C_WRITE_8_BYTES   =  8,
       HWD_I2C_WRITE_16_BYTES  =  16,
       HWD_I2C_WRITE_64_BYTES  =  64
    DeviceNum  - I2C device number

  RETURNED VALUES:
    TRUE if success, FALSE if failure

*****************************************************************************/
extern bool HwdI2cWrite(uint32 Address, uint8* DataPtr, uint16 NumBytes, uint16 DeviceAddr);

/*****************************************************************************
    
  FUNCTION NAME: HwdI2cRead

  DESCRIPTION:

     This routine reads data bytes from the I2C physical interface. This 
     routine sets up the I2C interface for a read operation. The actual read 
     operation occurs in the I2C LISR.

  PARAMETERS:

    Address    - I2C byte address
    DataPtr    - Pointer to where to place the data 
    NumBytes   - Number of bytes to read
    DeviceNum  - I2C device number

  RETURNED VALUES:
    TRUE if success, FALSE if failure

*****************************************************************************/
extern bool HwdI2cRead(uint32 Address, uint8* DataPtr, uint16 NumBytes, uint16 DeviceAddr);

/*****************************************************************************
    
  FUNCTION NAME: HwdI2cReset

  DESCRIPTION:

     This routine resets the I2C interface logic, including CP programmable
     registers. This bit is automatically cleared by hardware.
     The CP should wait at least 1.6 usec after asserting SW_RST before
     initiating any I2C transactions.

  PARAMETERS: None

  RETURNED VALUES: None

*****************************************************************************/
extern void HwdI2cReset(void);

 extern void HwdI2cWriteNoInt(uint32 Address, uint8* DataPtr, uint16 NumBytes, uint16 DeviceAddr);


#endif
