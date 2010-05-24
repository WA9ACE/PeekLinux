/*-------------------------------------------------------------------
File name : mv3Attach.h
MV400 middle level camera control routines

(C) Copyright MtekVision, Inc. 2003~2004 All right reserved.
  Confidential Information

All parts of the MtekVision Program Source are protected by copyright law
and all rights are reserved.
This documentation may not, in whole or in part, be copied, photocopied,
reproduced, translated, or reduced to any electronic medium or machine
readable form without prior consent, in writing, from the MtekVision.

Last modification : 2005. 2. 3
----------------------------------------------------------------------*/
#ifndef __MV3ATTACH_H__
#define __MV3ATTACH_H__


/*----------------------------------------------------------
    Define : Porting Point
             0) MV400 Development Board define
             1) Camera DSP
                - FEATURE_MV3018
                - FEATURE_MV3019
                - Memory Size
             2) MV3 Base Address
             3) MV3 Byte Spread
             4) JPEG Decode Patch
             5) Debug message control
             6) UNUSED GPIO direction set to output
             7) Sensor Reset or Power pin by MV400 GPIO
             8) EXIF shot base information
             9) Camera Zoom ratio value
             10) LCD size
-----------------------------------------------------------*/
/* #0. If you DO NOT use MV400 Development board, This define HAS TO BE disabled.   */
//#define MV400_Development_Board


/* #1. Select CCP (Cameca Control Processor) */
//#define FEATURE_MV3018
#define FEATURE_MV3019
//#define FEATURE_MV3020

#ifdef FEATURE_MV3018
#define FEATURE_MV3018B
#endif

#ifdef FEATURE_MV3018          // 128KByte == 1Mbit
#define MAX_MEMORY_SIZE         0x10000
#define MAX_MEMORY_SIZE_MJPEG   0x10000
#elif defined(FEATURE_MV3019) || defined(FEATURE_MV3020)  // 218KByte == 1.7Mbit
#define MAX_MEMORY_SIZE         0x18000
#define MAX_MEMORY_SIZE_MJPEG   0x1b400
#endif


/* #2. Define base address for CCP */
#define MV3_BASE_ADDR           0x02000000L


/* #3. Define byte spread */
// Shifting value as pin connection of MCU and MV3
// ARM A1 --> MV3 A0 if ARM Ax is connected to MV3 A0, set shift value as (1) ARM A2 --> MV3 (2)
#define MV3_BYTE_SPREAD             (1 << 17)

// Lib 1.15 : mv3Delay() define.
//#define mv3Delay1ms(x)              mv3Delay100us((x) * 10


/* #4. Define : JPEG Decode Patch Library */
// Lib 1.15 : Define JPEG Decode Patch
#define USE_JPEG_PATCH
#define ENABLE_GRAY_SCALE_JPEG_DECODE


/* #5. Define Debug Message Enable Option */
#define MV3_DEBUG_MSG


// Lib 1.22 : move here from mv3Attach.h
/* #6. Define UNUSED GPIO direction set to output */
#define UNUSED_GPIO (0x0)   //  If only use GPIO2, value is set to 0x10.  1 : input, 0 : output


#if 0 // for Old dev board
    // Lib 1.22 : move here from mv3Attach.h
    /* #7. Sensor Reset and Power Pin GPIO number set */
    // If you do not use sensor reset pin or power pin, this define has to be disable.
    //#define SENSOR_RESET_PIN    4           // MV400 GPIO PIN NUMBER
    #define SENSOR_RESET_PIN    2           // MV400 GPIO PIN NUMBER
    #define SENSOR_POWER_PIN   3           // MV400 GPIO PIN NUMBER
#else // for Micron MT9M111 on the New CCP Dev Board
    // Lib 1.22 : move here from mv3Attach.h
    /* #7. Sensor Reset and Power Pin GPIO number set */
    // If you do not use sensor reset pin or power pin, this define has to be disable.
    #define SENSOR_RESET_PIN    4           // MV400 GPIO PIN NUMBER
    #define SENSOR_POWER_PIN   3           // MV400 GPIO PIN NUMBER
#endif
#define LOW_RESET_SENSOR   // Hynix, Micron MT9V112, Sharp 2MP, Matsushita, Micron AU75E, MT9M111,OV9655
//#define HIGH_RESET_SENSOR   // Omnivision, 9314 + LSI 2MP, BioMorphic

// select power down polarity of image sensor
#define LOW_ACTIVE_POWERDOWN  // 9314 + LSI 2MP, Micron AU75E,Hynix, 
//#define HIGH_ACTIVE_POWERDOWN  // Omnivision, Micron, Sharp 2MP, BioMorphic, MT9V112, MT9M111,OV9655


// Lib 1.15 : EXIF base information. EXIF Library is comming soon. Probably next version.
/*----------------------------------------------------------
	Define : EXIF shot information
-----------------------------------------------------------*/
#define MV_EXIF_MAKER		"Mtekvision\0"
#define MV_EXIF_MAKER_LEN	11
#define MV_EXIF_MODEL		"MV400\0"
#define MV_EXIF_MODEL_LEN	6


/*----------------------------------------------------------
	Define : Camera Zoom ratio value
-----------------------------------------------------------*/
#define CAMERA_PARAMETER_ZOOM_IN_RATIO      ((float)0.96)


/*----------------------------------------------------------
	Define : Sensor Resolution
-----------------------------------------------------------*/
#define MAX_SENSOR_WIDTH     (640) // (1280)//
#define MAX_SENSOR_HEIGHT    (480) //  (1024)//

/*----------------------------------------------------------
	Define : Main/Sub LCD size
-----------------------------------------------------------*/
// Main LCD width & height
#define MAX_M_LCD_X          320	//240// 220// 176//144//
#define MAX_M_LCD_Y          240	//320//176// 220//176//

// Sub  LCD width & height
#define MAX_S_LCD_X             96
#define MAX_S_LCD_Y             64


/*----------------------------------------------------------
       Define : Interface define betwwen MV400 and MCU
-----------------------------------------------------------*/
#define outpw_(addr, data)        (*((volatile mvUint16 *) (addr)) = ((mvUint16) (data)))
#define inpw_(addr)               (*((volatile mvUint16 *) (addr)))


/*----------------------------------------------------------
       Define : LCD Set
-----------------------------------------------------------*/
#define MV3_LCD_WRITE_ENABLE    1
#define MV3_LCD_RS_CLS          0
#define MV3_LCD_RS_SET          1

/*------------------------------------------------------------------------------
Define : BANK 0 - LCD BYPASS
------------------------------------------------------------------------------*/
//#define __USE_16TO18_LCD__
#ifdef __USE_16TO18_LCD__
#define LCD_CMD_TYPE    0x2     // lcd command type
#define MV3_LCD_CONVERSION       (1 << 8)
#define MV3_LCD_CMD_POL          (1<<12)
#define MV3_LCD_CMD_TYPE        (LCD_CMD_TYPE << 9)      // 

#define mv3LcdSetRS()                  outpw_(MV3_BASE_ADDR, MV3_LCD_CONVERSION |\
                                                                                               MV3_LCD_RS_SET) /* MV301x_PORT_LCD_RS */
#define mv3LcdClearRS()                outpw_(MV3_BASE_ADDR, MV3_LCD_CONVERSION |\
                                                                                                MV3_LCD_CMD_TYPE|\
                                                                                                MV3_LCD_RS_CLS)/* MV301x_PORT_LCD_RS */
#define mv3LcdIndexSet()                 outpw_(MV3_BASE_ADDR, MV3_LCD_CONVERSION |\
                                                                                                  MV3_LCD_CMD_TYPE |\
                                                                                                  MV3_LCD_CMD_POL|\
                                                                                                  MV3_LCD_RS_CLS)//0x1500)

#else
/* Note: The following are hard-coded with POLARITYLRS_HIGH, OUTMODE_RGB16_16, OUTMODE_TYPE0. */
#define mv3LcdSetRS()                  outpw_(MV3_BASE_ADDR, 1) /* MV301x_PORT_LCD_RS */
#define mv3LcdClearRS()                outpw_(MV3_BASE_ADDR, 0) /* MV301x_PORT_LCD_RS */
#endif
/*----------------------------------------------------------
       Define : LCD Data Read / Write
-----------------------------------------------------------*/
// If use 18bits of data for 250K color LCD, Must use 2 Address lines
//#define __USE_18BIT_LCD__
//#define COLOR18
//#define __USE_18BIT_SUBLCD__

#if defined (__USE_18BIT_LCD__)
#define mv3LcdDataWrite(data)       outpw_(MV3_BASE_ADDR + (1 * MV3_BYTE_SPREAD)+(data&0x30000), data)
#else
#define mv3LcdDataWrite(data)       outpw_(MV3_BASE_ADDR + (1 * MV3_BYTE_SPREAD), data)
#endif

#if defined (__USE_18BIT_SUBLCD__)
#define mv3SubLcdDataWrite(data)    outpw_(MV3_BASE_ADDR + (2 * MV3_BYTE_SPREAD)+(data&0x30000), data)
#else
#define mv3SubLcdDataWrite(data)    outpw_(MV3_BASE_ADDR + (2 * MV3_BYTE_SPREAD), data)
#endif

#define mv3LcdDataRead()            inpw_(MV3_BASE_ADDR + (1 * MV3_BYTE_SPREAD))
#define mv3SubLcdDataRead()         inpw_(MV3_BASE_ADDR + (2 * MV3_BYTE_SPREAD))


#if 0
// Added by KJ Lee
#define mv3LcdCmdWrite(addr)	{mv3LcdClearRS();	mv3LcdDataWrite(addr);	mv3LcdSetRS();}

#ifdef COLOR16P2
	#define mv3WriteDataToMainLcd(data) {mv3LcdDataWrite(data >> 2); mv3LcdDataWrite(data << 14);}
#else
	#define mv3WriteDataToMainLcd(data)  mv3LcdDataWrite(data);
#endif
#endif

/*----------------------------------------------------------
	Define : Type for MV400 Library
-----------------------------------------------------------*/
typedef int	            mvStatus;

typedef unsigned char   mvBoolean;
typedef unsigned char   mvUint8;
typedef unsigned short  mvUint16;
typedef unsigned int    mvUint32;
typedef unsigned long   mvUint64;

typedef signed char     mvInt8;
typedef signed short    mvInt16;
typedef signed int      mvInt32;
typedef signed long     mvInt64;

#endif /* __MV3ATTACH_H__ */

