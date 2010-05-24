
/*=============================================================================
 * Copyright 2002-2003 Texas Instruments Incorporated. All Rights Reserved.
 */                                                                       

#ifndef BSP_I2C_PLATFORM_HEADER
#define BSP_I2C_PLATFORM_HEADER

#include "chipset.cfg"
#include "types.h"

/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspI2c_Platform
 *    Platform specific public interface to the I2C master device driver. 
 *
 *    This contains assignments for devices on the differnt platforms.
 */


/*=============================================================================
 *  Defines
 *============================================================================*/
/*===========================================================================*/
/*!
 * @typedef BspI2c_DeviceId
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the data type for a device on the I2C Bus. This is a subset of
 *   the 128 devices an I2C bus supports. There shuold only be as many devices
 *   as exist on the target platform
 */
#if defined(PLATFORM_TCS4100TORPHINS)
enum
{
    BSP_I2C_DEVICE_ID_TLV320AIC23 = 0,
    BSP_I2C_DEVICE_ID_NUM_DEVICES
};
typedef Uint8 BspI2c_DeviceId;
#elif defined(PLATFORM_TCS4100EVM) || \
	   defined(PLATFORM_TCS4102EVM) ||		\
      defined(PLATFORM_TCS3100CEVM) || \
      defined(PLATFORM_TCS3100EVM) || \
      defined(PLATFORM_TCS4103EVM)
enum
{
    BSP_I2C_DEVICE_ID_EEPROM_A    = 0,
    BSP_I2C_DEVICE_ID_EEPROM_B    = 1,
    BSP_I2C_DEVICE_ID_TLV320AIC23 = 2,
    BSP_I2C_DEVICE_ID_TWL3024     = 3,
    BSP_I2C_DEVICE_ID_TWL3029     = 4,
    BSP_I2C_DEVICE_ID_NUM_DEVICES
};
typedef Uint8 BspI2c_DeviceId;
#elif defined(PLATFORM_TCS4105EVM) || \
      defined(PLATFORM_TCS3100EVM) || \
      defined(PLATFORM_TCS4105STORNOWAY) || \
      defined(PLATFORM_TCS4105DBM)
enum
{
    BSP_I2C_DEVICE_ID_EEPROM_A    = 0,
    BSP_I2C_DEVICE_ID_EEPROM_B    = 1,
    BSP_I2C_DEVICE_ID_TWL3024     = 2,
    BSP_I2C_DEVICE_ID_NUM_DEVICES
};
typedef Uint8 BspI2c_DeviceId;
#elif defined(PLATFORM_OMAP1509EVM)
enum
{
    BSP_I2C_DEVICE_ID_EEPROM_A = 0, /* SDRAM Module */
    BSP_I2C_DEVICE_ID_EEPROM_B = 1, /* Flash Module */
    BSP_I2C_DEVICE_ID_RTC      = 2,
    BSP_I2C_DEVICE_ID_CAMERA   = 3,
    BSP_I2C_DEVICE_ID_NUM_DEVICES
};
typedef Uint8 BspI2c_DeviceId;
#elif defined(PLATFORM_OMAP1510INNOVATOR)
enum
{
    BSP_I2C_DEVICE_ID_EEPROM_A     = 0, /* Atmel lower page */
    BSP_I2C_DEVICE_ID_EEPROM_B     = 1, /* Atmel upper page */
    BSP_I2C_DEVICE_ID_TLV320AIC23  = 2,
    BSP_I2C_DEVICE_ID_NUM_DEVICES
};
typedef Uint8 BspI2c_DeviceId;
#elif defined(PLATFORM_OMAP1610EVM) || \
      defined(PLATFORM_OMAP1623STORNOWAY)
enum
{
    BSP_I2C_DEVICE_ID_ISP1301      = 0, /* USB OTG */
    BSP_I2C_DEVICE_ID_TPS65010     = 1, /* Power management IC */
    BSP_I2C_DEVICE_ID_TWL3024      = 2,
    BSP_I2C_DEVICE_ID_NUM_DEVICES
};
typedef Uint8 BspI2c_DeviceId;
#endif


/*===========================================================================*/
/*!  
 * @typedef BspI2c_DeviceAddress 
 *
 * @discussion
 * <b> Description </b><br>
 *   The I2C can address multiple devices. This type is used to address an
 *   individual device. Range: 0  - 127
 */
#if defined(PLATFORM_TCS4100TORPHINS)
enum
{
    BSP_I2C_DEVICE_ADDRESS_TLV320AIC23 = 0x1A
};
typedef Uint8 BspI2c_DeviceAddress;
#elif defined(PLATFORM_TCS4100EVM) || \
      defined(PLATFORM_TCS4102EVM) || \
      defined(PLATFORM_TCS3100CEVM) || \
      defined(PLATFORM_TCS3100EVM) || \
      defined(PLATFORM_TCS4103EVM)
enum
{
    BSP_I2C_DEVICE_ADDRESS_EEPROM_A          = 0x50,
    BSP_I2C_DEVICE_ADDRESS_EEPROM_B          = 0x52,
    BSP_I2C_DEVICE_ADDRESS_TLV320AIC23       = 0x1A,
    BSP_I2C_DEVICE_ADDRESS_TWL3024_PRIMARY   = 0x48,
    BSP_I2C_DEVICE_ADDRESS_TWL3024_SECONDARY = 0x4A,
    BSP_I2C_DEVICE_ADDRESS_TWL3029			 = 0x2D
};
typedef Uint8 BspI2c_DeviceAddress;
#elif defined(PLATFORM_TCS4105EVM) || \
      defined(PLATFORM_TCS3100EVM) || \
      defined(PLATFORM_TCS4105STORNOWAY) || \
      defined(PLATFORM_TCS4105DBM)
enum
{
    BSP_I2C_DEVICE_ADDRESS_EEPROM_A          = 0x57,
    BSP_I2C_DEVICE_ADDRESS_TWL3024_PRIMARY   = 0x48,
    BSP_I2C_DEVICE_ADDRESS_TWL3024_SECONDARY = 0x4A
};
typedef Uint8 BspI2c_DeviceAddress;
#elif defined(PLATFORM_OMAP1509EVM)
enum
{
    BSP_I2C_DEVICE_ADDRESS_EEPROM_A = 0x51,
    BSP_I2C_DEVICE_ADDRESS_EEPROM_B = 0x52,
    BSP_I2C_DEVICE_ADDRESS_RTC      = 0x62,
    BSP_I2C_DEVICE_ADDRESS_CAMERA   = 0x1A
};
typedef Uint8 BspI2c_DeviceAddress;
#elif defined(PLATFORM_OMAP1510INNOVATOR)
enum
{
    BSP_I2C_DEVICE_ADDRESS_EEPROM_A    = 0x50, /* Atmel lower page */
    BSP_I2C_DEVICE_ADDRESS_EEPROM_B    = 0x51, /* Atmel Upper page */
    BSP_I2C_DEVICE_ADDRESS_TLV320AIC23 = 0x1A
};
typedef Uint8 BspI2c_DeviceAddress;
#elif defined(PLATFORM_OMAP1610EVM) || \
      defined(PLATFORM_OMAP1623STORNOWAY)
enum
{
    BSP_I2C_DEVICE_ADDRESS_ISP1301     = 0x2D,
    BSP_I2C_DEVICE_ADDRESS_TPS65010    = 0x48,
    BSP_I2C_DEVICE_ADDRESS_TWL3024_PRIMARY   = 0x48,
    BSP_I2C_DEVICE_ADDRESS_TWL3024_SECONDARY = 0x4A
};
typedef Uint8 BspI2c_DeviceAddress;
#endif

#if (CHIPSET==15)
enum
{
    BSP_I2C_DEVICE_ADDRESS_TWL3029 = 0x2D,
    BSP_I2C_DEVICE_ADDRESS_I2C_1   = 0x15,
    BSP_I2C_DEVICE_ADDRESS_I2C_2   = 0x16,
    #if (CAM_SENSOR==0)
    BSP_I2C_DEVICE_ADDRESS_CAMERA  = 0x53,
    #else    
     BSP_I2C_DEVICE_ADDRESS_CAMERA  = 0x5D,
    #endif    
    BSP_I2C_DEVICE_ADDRESS_KPD = 0X42
};
typedef Uint8 BspI2c_DeviceAddress;

enum
{
    BSP_I2C_DEVICE_ID_TWL3029 = 0,
    BSP_I2C_DEVICE_ID_I2C_1   = 1,
    BSP_I2C_DEVICE_ID_I2C_2   = 2,
    BSP_I2C_DEVICE_ID_CAMERA  = 3,
    BSP_I2C_DEVICE_ID_KPD  = 4,
    BSP_I2C_DEVICE_ID_NUM_DEVICES
};
typedef Uint8 BspI2c_DeviceId;
#endif

#endif
