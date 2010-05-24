/**
* @file i2c_api.h
*
* API Definition for I2C SWE.
*
* @author Remco Hiemstra (remco.hiemstra@ict.nl)
* @version 0.1
*/

/*
* History:
*
* Date        Author          Modification
* -------------------------------------------------------------------
* 12/30/2003  Remco Hiemstra (remco.hiemstra@ict.nl)    Create.
*
* (C) Copyright 2003 by ICT Automatisering, All Rights Reserved
*/

#ifndef __I2C_API_H_
#define __I2C_API_H_


#include "rvm/rvm_gen.h"    /* Generic RVM types and functions. */



#ifdef __cplusplus
extern "C"
{
#endif
  
/**
* @name I2C Return type and return values.
*
* Currently they are the standard RV return types, but they may
* be customized in the future.
  */
  /*@{*/
  typedef T_RV_RET T_I2C_RETURN;
  
#define I2C_OK          RV_OK
#define I2C_NOT_SUPPORTED   RV_NOT_SUPPORTED
#define I2C_MEMORY_ERR      RV_MEMORY_ERR
#define I2C_INTERNAL_ERR    RV_INTERNAL_ERR
  /*@}*/
  
  typedef enum
  {
    I2C_INTERRUPT = 0,
      I2C_POLLING
  } T_I2C_TRANFER_MODE;
  
  typedef enum
  {
    I2C_LITTLE_ENDIAN  = 0x0000,
      I2C_BIG_ENDIAN     = 0x4000
  } T_I2C_ENDIAN;
  
  /*Defines the behavior of the I2C device on a debugger breakpoint.*/
  typedef UINT8 T_I2C_FREERUN;
  
  /*T_I2C_FREERUN can have the following values:*/
#define I2C_STOP_MODE       (0)
#define I2C_FREE_RUN_MODE   (1)
  
  /*Enables the setting of the interrupt status bits:
  I2C_GENERAL_CALL
  I2C_TX_READY
  I2C_RX_READY
  I2C_REG_READY
  I2C_NACK
  I2C_ARB_LOST*/
  typedef UINT8 T_I2C_INT_SET;
  
  /*T_I2C_INT_SET can have the following values:*/
  
#define I2C_INT_NO_SET      (0)
#define I2C_INT_SET         (1)
  
  /*Defines the test to be configured.*/
  typedef UINT8 T_I2C_TEST;
  
  /*T_I2C_TEST can have the following values:*/
#define I2C_NO_TEST         (0x00)
#define I2C_TEST_SCL        (0x01)
#define I2C_TEST_LOOPBACK   (0x03)
  
/**
* @I2C API functions
*
* API functions declarations (bridge functions).
*/
  


/**
* I2c_read bridge functions.
* 
* This function reads a number of bytes from an i2c address.
* 
* @param             UINT16   address          i2c address
* @param             UINT16 * read_buffer_p    Where to put the read data in
* @param             UINT16   nmb_of_bytes     The number of bytes
* @param             T_I2C_ENDIAN endian       I2C_BIG_ENDIAN or I2C_SMALL_ENDIAN.
* @param             T_RV_RETURN return_path
*
* @return            I2C_OK : success
*                    I2C_NOT_READY : I2C task not ready.
*                    I2C_MEMORY_ERR : no memory
*/
T_I2C_RETURN i2c_read (UINT16   address,
                       UINT16 * read_buffer_p,
                       UINT16   nmb_of_bytes, T_I2C_ENDIAN endian, T_RV_RETURN return_path);


/**
* 
* This function writes a number of bytes to an i2c address.
* 
* @param             UINT16   address          i2c address
* @param             UINT16 * write_buffer_p   the buffer were the data must be read from.
* @param             UINT16   nmb_of_bytes     The number of bytes
* @param             T_I2C_ENDIAN endian       I2C_BIG_ENDIAN or I2C_SMALL_ENDIAN.
* @param             T_RV_RETURN return_path
*
* @return            I2C_OK : success                    
*                    I2C_NOT_READY : I2C task not ready 
*                    I2C_MEMORY_ERR : no memory         
*/
T_I2C_RETURN i2c_write (UINT16   address,
                        UINT16 * write_buffer_p, UINT16 nmb_of_bytes, T_I2C_ENDIAN endian, T_RV_RETURN return_path);


/**
* 
* Set transfermode to I2C_POLLING or I2C_INTERRUPT based.
* 
* @param             T_I2C_TRANFER_MODE transfer_mode transfermode I2C_POLLING, I2C_INTERRUPT
* @param             T_RV_RETURN return_path
*
* @return            I2C_OK : success                    
*                    I2C_NOT_READY : I2C task not ready 
*                    I2C_MEMORY_ERR : no memory         
*/
T_I2C_RETURN i2c_set_transfer_mode (T_I2C_TRANFER_MODE transfer_mode, T_RV_RETURN return_path);


/**
* 
* 
* This function is used to facilitate system-level tests by overriding some of he standard
* functional features of the peripheral. It can permit the test of SCL counters, control  
* the signals that connect to I/O pins for digital loop-back for self-test. It also provides 
* stop/no-stop functionality in debug mode.                                                  
* 
* @param             T_I2C_FREERUN  freerun_mode  With freerun_mode the behaviour of the  
*                                                  I2C controller can be defined when a    
*                                                  breakpoint is encountered in the        
*                                                  debugger.                               
* @param             T_I2C_INT_SET  int_status    With int_status the interrupt status bits
*                    as defined in T_I2C_INTERRUPT can all be 
*                                                  set to 1.                                
* @param             T_I2C_TEST test_mode         With test_mode  the device can put in two
*                                                 different test modes.                    
*                                                 - The SCL counter test mode            
*                                                   Generate continuous clock signal on    
*                                                   SCL pin. This is useful to test the    
*                                                   prescaler and SCL high/low counters.   
*                                                   (Verify speed settings). Parameters    
*                                                   scl_value and sda_value has no         
*                                                   meaning in this mode.                  
*                                                  - Loopback mode.                       
*                                                   The value of the scl_value and         
*                                                   sda_value parameters are routed to the 
*                                                   SCL and SDA lines. Use the function    
*                                                   i2c_get_loopback_value to read back the
*                                                   effect on the output lines. Possible   
*                                                   open lines or shortcuts can be detected
*                                                   in this way.                            
* @param             UINT8  scl_value       Used only in combination with the        
* @param             UINT8 sda_value        parameter test_mode and only then when it
*                         selects the loopback test mode.          
* @return                            T_I2C_RETURN
*/
T_I2C_RETURN i2c_set_system_test(T_I2C_FREERUN  freerun_mode,
                                 T_I2C_INT_SET  int_status,
                                 T_I2C_TEST test_mode, 
                                 UINT8  scl_value, UINT8 sda_value);
                                 


/**
* This function is used to read the status of the data and clock 
* lines (SDA and SCL). The returned values are only defined when 
*  the device is already set in the loopback ystem test mode.     
* 
* @param             UINT8  *scl_value_p   Must point to the location where the
* @param             UINT8  *sda_value_p   state of the SCL and SDA lines is to
*                                          be written. The returned values are 
*                                          only in valid when the system test  
*                                          mode is selected in the loopback.   
*
* @return            T_RV_RET
*/
T_I2C_RETURN i2c_get_test_data(UINT8  *scl_value_p,
                               UINT8  *sda_value_p);

/**
* 
*  Function Name:  i2c_get_sw_version                          
*                                                               
*  Purpose: This function returns the driver version.           
*  Return :                                                     
*          UINT32        [0-15] BUILD Build number         
*                        [16-23]  MINOR Minor version number 
*                        [24-31]  MAJOR Major version number 
* @param             void
*
* @return            
*/
UINT32 i2c_get_sw_version(void);


/**
*  
* Function Name:   i2c_get_hw_version                                     
*                                                                         
* Purpose:   This function returns the hardware version of the I2C device.
* Return :                                                                
*         UINT8         [0-3] MINOR Minor version number                
*                       [4-7] MAJOR Major version number                
* @param             void
*
* @return            
*/
UINT8 i2c_get_hw_version(void);


  
#ifdef __cplusplus
}
#endif


#endif /*__I2C_API_H_*/
