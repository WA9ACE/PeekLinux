/*-------------------------------------------------------------------
File name : mv3Lib.h

MV400 middle level camera control routines

(C) Copyright MtekVision, Inc. 2003~2004 All right reserved.
  Confidential Information

All parts of the MtekVision Program Source are protected by copyright law
and all rights are reserved.
This documentation may not, in whole or in part, be copied, photocopied,
reproduced, translated, or reduced to any electronic medium or machine
readable form without prior consent, in writing, from the MtekVision.

Last modification : 2005. 03. 05
----------------------------------------------------------------------*/
#ifndef __MV3LIB_H__
#define __MV3LIB_H__


//=========================================================
#define MV400_LIB_VERSION       "MV400 Library Version 1.32"
#define MADE_BY                 "MTEKVISION SOFTWARE TEAM"
//=========================================================


//=========================================================
#include "mv3Attach.h"
//=========================================================


//=========================================================
// * Carefully
// Enum type is character
//=========================================================
typedef enum {
    DEBUG_LEVEL_FATAL,
    DEBUG_LEVEL_ERROR,
    DEBUG_LEVEL_HIGH,
    DEBUG_LEVEL_MED,
    DEBUG_LEVEL_LOW   
} DEBUG_LEVEL_TYPE;

typedef enum {
    OSD_TRANSPARENCY_0,    //  0.0 %
    OSD_TRANSPARENCY_12_5, // 12.5 %
    OSD_TRANSPARENCY_25,   // 25.0 %
    OSD_TRANSPARENCY_37_5, // 37.5 %
    OSD_TRANSPARENCY_50,   // 50.0 %
    OSD_TRANSPARENCY_62_5, // 62.5 %
    OSD_TRANSPARENCY_75,   // 75.0 %
    OSD_TRANSPARENCY_87_5, // 87.5 %
    OSD_TRANSPARENCY_100  //100.0 %
} MV3_OSD_TRANSPARENCY_TYPE;

typedef enum{
    TIMESTAMP_DISABLE,
    TIMESTAMP_ENABLE
}MV3_TIMESTAMP_ENABLE_TYPE;

typedef enum{
    TIMESTAMP_BG_TRANSPARENCY_100,
    TIMESTAMP_BG_TRANSPARENCY_50
}MV3_TIMESTAMP_BG_TRANS_TYPE;

typedef enum{
    TIMESTAMP_TEXT_TRANSPARENCY_0,
    TIMESTAMP_TEXT_TRANSPARENCY_50
}MV3_TIMESTAMP_TEXT_TRANS_TYPE;

typedef enum{
    MAIN_LCD,
    SUB_LCD
} MV3_LCD_SELECT;

/*----------------------------------------------------------
    The types for mv3 property control
-----------------------------------------------------------*/
typedef enum {
    PD_TARGET_NULL,
    PD_TARGET_SENSOR,
    PD_TARGET_MV3,
    PD_TARGET_ALL
} MV3_POWERDOWN_TARGET_TYPE;

#if 0 // modified by Richard Kim on 20050527
typedef enum {
    MV3_RET_SUCCESS                     = 0,
    MV3_RET_INVALID_PARAMETER           = -100,
    MV3_RET_INVALID_STATE,
    MV3_RET_MASKSIZE_SMALL,
    MV3_RET_MEMORY_OVERFLOW,
    MV3_RET_HREF_ERROR,
    MV3_RET_FIFO_FULL,
    MV3_RET_FAIL,
} MV3_RETURN_TYPE;
#else
typedef enum {
    MV3_RET_SUCCESS                     = 0,
    MV3_RET_INVALID_PARAMETER           = -100,
    MV3_RET_INVALID_STATE,
    MV3_RET_MASKSIZE_SMALL,
    MV3_RET_MEMORY_OVERFLOW,
    MV3_RET_HREF_ERROR,
    MV3_RET_FIFO_FULL,
    MV3_RET_FAIL,
    // inserted by Richard Kim @ 20050527
    MV3_RET_BUFFER_EXCEEDED,
    MV3_RET_TIME_OVER,
    // inserted by Richard Kim @ 20050705
    MV3_RET_MEMORY_UNDERRUN,
    MV3_RET_MEMORY_OVERRUN
} MV3_RETURN_TYPE;
#endif

typedef enum {
    STROBE_DISABLE,
    STROBE_ENABLE
} MV3_STROBE_TYPE;

typedef enum {
    EXT_VS_DISABLE,
    EXT_VS_ENABLE
} MV3_EXT_VS_TYPE;

typedef enum {
    PVSYNC_LOW,
    PVSYNC_HIGH
} MV3_PVSYNC_TYPE;

typedef enum {
    POLARITYLRS_LOW,
    POLARITYLRS_HIGH
} MV3_POLARITYLRS_TYPE;

typedef enum {
    PHSYNC_LOW,
    PHSYNC_HIGH
} MV3_PHSYNC_TYPE;

typedef enum {
    I2C_SPEED_CLK_60 = 30,	        // 400Khz when Clock is 24Mhz
    I2C_SPEED_CLK_80 = 40,	        // 300Khz when Clock is 24Mhz
    I2C_SPEED_CLK_100 = 50,	        // 240Khz when Clock is 24Mhz
    I2C_SPEED_CLK_120 = 60,	        // 200Khz when Clock is 24Mhz
    I2C_SPEED_CLK_150 = 75,	        // 160Khz when Clock is 24Mhz
    I2C_SPEED_CLK_200 = 100,		// 120Khz when Clock is 24Mhz
    I2C_SPEED_CLK_240 = 120,		// 100Khz when Clock is 24Mhz
    I2C_SPEED_CLK_300 = 150,		// 80Khz when Clock is 24Mhz
    I2C_SPEED_CLK_400 = 200,		// 60Khz when Clock is 24Mhz
    I2C_SPEED_CLK_600 = 300,		// 40Khz when Clock is 24Mhz
    I2C_SPEED_CLK_800 = 400,		// 30Khz when Clock is 24Mhz
    I2C_SPEED_CLK_1000 = 500	// 24Khz when Clock is 24Mhz
} MV3_I2C_SPEED_TYPE;

typedef enum {
    EFFECT_NORMAL,
    EFFECT_B_AND_W,
    EFFECT_SEPIA,
    EFFECT_NEGA,
    EFFECT_EMBOSS,
    EFFECT_SKETCH,
    EFFECT_COLOR_SEL_POSITIVE,
    EFFECT_COLOR_SEL_NEGATIVE
} MV3_IMAGE_EFFECT_TYPE;

typedef enum {
    LCD_HIGH_SPEED_WRITE,
    LCD_NORMAL_SPEED_WRITE
} MV3_LCD_WRITE_SPEED_TYPE;

typedef enum {
    OUTMODE_P0_B0_M0,
    OUTMODE_P0_B0_M1,
    OUTMODE_P0_B1_M0,
    OUTMODE_P0_B1_M1,
    OUTMODE_P1_B0_M0,
    OUTMODE_P1_B0_M1,
    OUTMODE_P1_B1_M0,
    OUTMODE_P1_B1_M1
} MV3_LCD_OUTMODE_TYPE;

typedef enum {
    OUTMODE_8BITS,
    OUTMODE_16BITS,
    OUTMODE_18BITS
} MV3_LCD_OUTMODE_DATA_LENGTH;

typedef enum {
    OUTMODE_256_COLOR,
    OUTMODE_4000_COLOR,
    OUTMODE_65000_COLOR,
    OUTMODE_260000_COLOR
} MV3_LCD_OUTMODE_COLOR;

typedef enum {
    OUTMODE_TYPE0,
    OUTMODE_TYPE1,
    OUTMODE_TYPE2,
    OUTMODE_TYPE3
} MV3_LCD_OUTMODE_COMD_SEL;

// RGB COLOR, OUTPUT ORDER, BUS ALIGN(L:LEFT ALIGN(Value:1), R:RIGHT ALIGN(Value:0)), PIXEL ALIGN(F:FIRST FULL(Value:1), S:SECOND FULL(Value:0))
typedef enum {
    OUTMODE_RGB8_8,
    OUTMODE_RGB12_12,
    OUTMODE_RGB12_84LF,
    OUTMODE_RGB12_84RF,
    OUTMODE_RGB12_48LS,
    OUTMODE_RGB12_48RS,
    OUTMODE_RGB12_888,
    OUTMODE_RGB16_16,
    OUTMODE_RGB16_88,
    OUTMODE_RGB18_18,
    OUTMODE_RGB18_162LF,
    OUTMODE_RGB18_162RF,
    OUTMODE_RGB18_216LS,
    OUTMODE_RGB18_216RS,
    OUTMODE_RGB18_882LF,
    OUTMODE_RGB18_882RF,
    OUTMODE_RGB18_288LS,
    OUTMODE_RGB18_288RS,
    OUTMODE_RGB18_666L,
    OUTMODE_RGB18_666R,
    OUTMODE_RGB18_99
} MV3_LCD_OUTMODE;

typedef enum {
    MV3_TDS_FUNC_SEL_BITBLT,
    MV3_TDS_FUNC_SEL_COPYSRC1 = 0x04,
    MV3_TDS_FUNC_SEL_OSD,
    MV3_TDS_FUNC_SEL_SI_WO_CHROMAKEY,
    MV3_TDS_FUNC_SEL_SI_W_CHROMAKEY
} MV3_FUNC_SEL_TYPE;

typedef enum {
    MV3_TDS_ROP_BLACKNESS,
    MV3_TDS_ROP_N_SRC1_OR_SRC2,
    MV3_TDS_ROP_NSRC1,
    MV3_TDS_ROP_SRC1_AND_NSRC2,
    MV3_TDS_ROP_NSRC2,
    MV3_TDS_ROP_SRC2_XOR_PAT,
    MV3_TDS_ROP_SRC1_XOR_SRC2,
    MV3_TDS_ROP_SRC1_AND_SRC2,
    MV3_TDS_ROP_NSRC1_OR_SRC2,
    MV3_TDS_ROP_SRC2_AND_PAT,
    MV3_TDS_ROP_SRC1,
    MV3_TDS_ROP_SRC1_OR_SRC2,
    MV3_TDS_ROP_PAT,
    MV3_TDS_ROP_NSRC1_OR_SRC2_OR_PAT,
    MV3_TDS_ROP_WHITENESS,
    MV3_TDS_ROP_NOP
} MV3_TDS_ROP_TYPE;

/*---   MJEG   ------------------------------------------*/
typedef enum {
    MV3_MJPEG_STOP,
    MV3_MJPEG_START,
    MV3_MJPEG_PAUSE,
    MV3_MJPEG_RESUME
} MV3_MJPEG_OPERATION_TYPE;


/*---   BitBLT   ------------------------------------------*/

/* TARGET */
typedef enum {
    TARGET_MEMORY,
    TARGET_LCD,
    TARGET_JPEG,
    TARGET_LCD_JPEG
} MV3_TDS_TARGET_TYPE;

typedef enum {
    TARGET_SRAM,
    TARGET_MAIN_LCD,
    TARGET_SUB_LCD
} MV3_TARGET_TYPE;


typedef enum {
    MV3_PROP_NULL,
    MV3_PROP_LCD_SELECT,
    MV3_PROP_OSD,
    MV3_PROP_OSD_TRANSPARENCY,
    MV3_PROP_OSD_IMAGE,
    MV3_PROP_ZOOM,
    MV3_PROP_SCROLL,
    MV3_PROP_DISP,
    MV3_PROP_MIRROR,
    MV3_PROP_FLIP,
    MV3_PROP_ROTATE,
    MV3_PROP_EFFECT,
    MV3_PROP_SUPERIMPOSE,
    MV3_PROP_SUPERIMPOSE2,
    MV3_PROP_SUPERIMPOSE_CHROMA,
    MV3_PROP_BITMAP,
    MV3_PROP_RGB16,
    MV3_PROP_OUTFORMAT,
    MV3_PROP_MOTION_SPEED,
    MV3_PROP_MEM_POINTER_STEP,
    MV3_PROP_FILTER_EN,
#ifdef FEATURE_MV3019
    MV3_PROP_YC_SEL,
    MV3_PROP_3x3FILTER_EN,
    MV3_PROP_3x3FILTER_SET,
    MV3_PROP_C_SMOOTH_EN,
    MV3_PROP_INVERT_Y,
    MV3_PROP_STRONG_C,
    MV3_PROP_SMCLK,
    MV3_PROP_DOUBLE_BUFFER_EN,
#endif
    MV3_PROP_GAMMA_EN,
    MV3_PROP_GAMMA_SET,
    MV3_PROP_TARGET,

    MV3_PROP_SIZE,
    MV3_PROP_QUALITY,
    MV3_PROP_GET_DATA,
    MV3_PROP_GET_IMAGE_ADDR,
    MV3_PROP_JPEG_DATA,
    MV3_PROP_JPEG_LENGTH,
    MV3_PROP_JPEG_MASK,
    MV3_PROP_DECODE_SIZE,
    MV3_PROP_LENGTH,
    MV3_PROP_BITBLT_CHROMA_R,
    MV3_PROP_BITBLT_CHROMA_G,
    MV3_PROP_BITBLT_CHROMA_B,
    MV3_PROP_REC_TIME,

    MV3_PROP_MAIN_LCD,
    MV3_PROP_SUB_LCD,
    MV3_PROP_SENSOR,
    MV3_PROP_AUTO_INC,
    MV3_PROP_JPEG_ENDIAN,
    MV3_PROP_USE_INTERNAL_CLOCK,

	// insert raw data after for encode
    MV3_PROP_RAW_DATA,
    MV3_PROP_RAW_LENGTH,

	// multishotWork, mjpegWork changed by to one copy
    MV3_PROP_MULTISHOT_READY,

	// encode 2X zoom
    MV3_PROP_JPEG_ZOOM_2X,

	// multishot/mjpeg jpeg count
    MV3_PROP_JPEG_COUNT,

	// multishot/mjpeg jpeg data & length & frame number
    MV3_PROP_MULTISHOT_DATA,

	// get max zoom level
    MV3_PROP_ZOOM_MAX,

    // multishot/mjpg OSD update
    MV3_PROP_MULTISHOT_OSD,

    // Pll setting
    MV3_PROP_PLL,

    MV3_PROP_JPEG_RESIZE,

    MV3_PROP_ENCODE_INADDR,
    MV3_PROP_ENCODE_OUTADDR,
#if defined(FEATURE_MV3019)
    MV3_PROP_ENCODE_IN_IMAGE,
    MV3_PROP_ENCODE_IN_START_ADDR,
    MV3_PROP_ENCODE_IN_END_ADDR,
    MV3_PROP_ENCODE_OUT_START_ADDR,
    MV3_PROP_ENCODE_OUT_END_ADDR,
    MV3_PROP_ENCODE_OUT_JPEG,
#endif
    MV3_PROP_SCALE,
    MV3_PROP_DECODE_INADDR,
    MV3_PROP_DECODE_OUTADDR,
#if defined(FEATURE_MV3019)
    MV3_PROP_DECODE_IN_START_ADDR,
    MV3_PROP_DECODE_IN_END_ADDR,
    MV3_PROP_DECODE_OUT_START_ADDR,
    MV3_PROP_DECODE_OUT_END_ADDR,
    MV3_PROP_DECODE_OUT_IMAGE,
#endif
    MV3_PROP_MJPEG_DEC_VSYNC_PERIOD,
    MV3_PROP_MAX,
    MV3_PROP_TIMESTAMP,
// Lib 1.23 : Insert for input on the binary raw Image. 20050212 by Kevin
    MV3_PROP_TIMESTAMP_BINARY_IMG,

// Lib 1.22 update by Seth, Prime, KJ Lee
    MV3_PROP_AUTOSCALE,
    MV3_PROP_OPM_EN,
    MV3_PROP_RESTRICT
} MV3_PROP_TYPE;

typedef enum {
    MV3_ENCODE_ECONOMY = 124,
    MV3_ENCODE_NORMAL = 93,
    MV3_ENCODE_FINE = 46,
    MV3_ENCODE_SUPERFINE = 31,
    MV3_ENCODE_ULTRAFINE = 15
} MV3_ENCODE_QUALITY_TYPE;

//#define I2C_NUM                       0x0000
typedef enum {
    I2C_3BYTE,
    I2C_UNUSED,
    I2C_4BYTE,
    I2C_5BYTE,
    I2C_6BYTE
} MV3_I2C_NUM_TYPE;


/*----------------------------------------------------------
    The structures for mv3 property control
-----------------------------------------------------------*/
typedef struct {
    mvUint16 x;
    mvUint16 y;
} MV3_POSITION_STRUCT;

typedef struct {
    mvUint16 sx;
    mvUint16 sy;
    mvUint16 width;
    mvUint16 height;
} MV3_RECT_STRUCT;

typedef struct {
    mvUint16 width;
    mvUint16 height;
} MV3_SIZE_STRUCT;


/*----------------------------------------------------------
    Wrapper structures for mv3DevicePropSet
-----------------------------------------------------------*/
typedef struct {
    MV3_POLARITYLRS_TYPE         polarityLRS;
    mvUint8                     HighLWEN;
    mvUint8                     LowLWEN;

    MV3_LCD_OUTMODE                         LCDMode;
    MV3_LCD_OUTMODE_COMD_SEL    comdSel;                /* type 0 ~ 3 */

    MV3_SIZE_STRUCT         maxSize;
  } MV3_LCD_STRUCT;

typedef struct {
    MV3_SIZE_STRUCT         maxSize;
    MV3_PVSYNC_TYPE         sensorVsync;
    MV3_PHSYNC_TYPE         sensorHsync;
    MV3_I2C_SPEED_TYPE    	sclkPeriod;
// Lib 1.24 : Inserted
    mvUint32                pclk_inv;
    mvBoolean               sonySensor;
    mvUint8                 sensorVsyncFalling;
#ifdef FEATURE_MV3019
    mvUint8                 sensorI2C_SkipStop;	// Lib v1.20 Kevin
#endif
} MV3_SENSOR_STRUCT;

typedef enum {
    JPEG_ENDIAN_LITTLE,
    JPEG_ENDIAN_BIG
} MV3_JPEG_ENDIAN_TYPE;

typedef enum {
    GAMMA_DISABLE,
    GAMMA_ENABLE
} MV3_GAMMA_TYPE;

/*----------------------------------------------------------
    Wrapper structures for mv3PreviewPropertySet
-----------------------------------------------------------*/
typedef struct {
    mvUint32            baseAddr;

    MV3_RECT_STRUCT     osd1_win;
    MV3_RECT_STRUCT     osd2_win;
    mvUint16            *pImage;
#ifdef FEATURE_MV3019
    mvUint16            *pImage2;
    mvUint16            *pImage3;
    mvUint16            *pImage4;
    mvUint32            baseAddr2;
    mvUint32            baseAddr3;
    mvUint32            baseAddr4;
    MV3_RECT_STRUCT     osd3_win;
    MV3_RECT_STRUCT     osd4_win;
    mvUint8             osdWindow1ImgType;
    mvUint8             osdWindow2ImgType;
    mvUint8             osdWindow3ImgType;
    mvUint8             osdWindow4ImgType;
    mvUint16            *pallete; // for Pallete
#endif
    mvUint8             alpha7;
    mvUint8             alpha6;
    mvUint8             alpha5;
    mvUint8             alpha4;
    mvUint8             alpha3;
    mvUint8             alpha2;
    mvUint8             alpha1;
    mvUint8             alpha0;

    mvUint8             flgOsd;
} MV3_OSD_STRUCT;

 //Insert for OSD Transparency by Seth (Lib 1.12)
typedef struct {
    mvUint8             alpha7;
    mvUint8             alpha6;
    mvUint8             alpha5;
    mvUint8             alpha4;
    mvUint8             alpha3;
    mvUint8             alpha2;
    mvUint8             alpha1;
    mvUint8             alpha0;
} MV3_OSD_ALPHA_STRUCT;

typedef struct {
    MV3_IMAGE_EFFECT_TYPE   imgEffect;
    mvUint16  sepiaColor;

    mvUint8   point1;
    mvUint8   point2;
    mvUint8   point3;
    mvUint8   point4;
} MV3_IMAGE_EFFECT_STRUCT;

typedef enum {
    MV3_ROTATE_0DEGREE,
    MV3_ROTATE_90DEGREE,
    MV3_ROTATE_180DEGREE,
    MV3_ROTATE_270DEGREE
} MV3_ROTATE_TYPE;

typedef enum {
    MV3_OUT_FORMAT_RGB565 = 0x00,
    MV3_OUT_FORMAT_YUV422 = 0x02,
    MV3_OUT_FORMAT_YUV420 = 0x03
} MV3_OUT_FORMAT_TYPE;

typedef enum {
    MV3_OUT_FORMAT_YUV422_W_OSD = 0x2,
    MV3_OUT_FORMAT_YUV422_WO_OSD
} MV3_TDS_DST_PIXEL_TYPE;

// Lib 1.22 update by Seth and KJ Lee 
typedef struct{
    mvUint16 offsetX;
    mvUint16 offsetY;
    mvBoolean enflg;
}MV3_OPM_STRUCT;

// Lib 1.22 update by Seth and KJ Lee 
typedef struct{
    mvBoolean enflg;
    mvUint32 size;
}MV3_ENCODE_RESTRICT_STRUCT;

/*----------------------------------------------------------
    Wrapper structures for mv3Jpeg
-----------------------------------------------------------*/
typedef struct {
    mvUint16  *jpegImage;
    mvUint32  jpegLength;
} MV3_JPEG_STRUCT;

typedef struct {
    mvUint16  extWidth;                   /* JPEG Encode/Decode HSIZE (8 drainage, 8/16) */
    mvUint16  extHeight;                  /* JPEG Encode/Decode VSIZE (8 drainage, 8/16) */
    mvUint16  jpegWidth;                  /* JPEG image width */
    mvUint16  jpegHeight;                 /* JPEG iamge height */

    mvUint16  decMode;                    /* JPEG Decompressing Type */
    mvUint16  halfSize;                   /* for 4bytes alignment */
    mvUint8   component;

    mvUint16  dri;                        /* added in v1.12 for checking if there is DRI */
} MV3_JPEG_INFO_STRUCT;

/*----------------------------------------------------------
    Wrapper structures for mv3BitBlt
-----------------------------------------------------------*/
typedef struct {
    mvUint8             funSel;
    mvUint8             rop;
    mvUint16            chromaMask;

    mvUint16            chromaData;
    mvUint16            patternData;

    mvUint8             alpha7;
    mvUint8             alpha6;
    mvUint8             alpha5;
    mvUint8             alpha4;

    mvUint8             alpha3;
    mvUint8             alpha2;
    mvUint8             alpha1;
    mvUint8             alpha0;

    mvUint16            src1ImageWidth;

    mvUint16            src1RatioX;
    mvUint16            src1RatioY;

    mvUint32            src1StartYaddr;
#define src1StartUaddr src1OffsetU
#define src1StartVaddr src1OffsetV
    mvUint32            src1OffsetU;
    mvUint32            src1OffsetV;

    mvUint16            src2ImageWidth;
    mvUint16            src2RatioX;
    mvUint16            src2RatioY;

    mvUint16            src2Window1InitialX;
    mvUint16            src2Window1InitialY;
    mvUint16            src2Window1EndX;
    mvUint16            src2Window1EndY;

    mvUint16            src2Window2InitialX;
    mvUint16            src2Window2InitialY;
    mvUint16            src2Window2EndX;
    mvUint16            src2Window2EndY;
#ifdef FEATURE_MV3019
    mvUint16            src2Window3InitialX;
    mvUint16            src2Window3InitialY;
    mvUint16            src2Window3EndX;
    mvUint16            src2Window3EndY;

    mvUint16            src2Window4InitialX;
    mvUint16            src2Window4InitialY;
    mvUint16            src2Window4EndX;
    mvUint16            src2Window4EndY;
#endif
    mvUint32            src2StartAddr;
#ifdef FEATURE_MV3019
    mvUint32            osdWindow1StartAddr;
    mvUint32            osdWindow2StartAddr;
    mvUint32            osdWindow3StartAddr;
    mvUint32            osdWindow4StartAddr;

    mvUint8             osdWindow1ImgType;
    mvUint8             osdWindow2ImgType;
    mvUint8             osdWindow3ImgType;
    mvUint8             osdWindow4ImgType;
#endif

    MV3_OUT_FORMAT_TYPE  src1PixelFormat;
    MV3_OUT_FORMAT_TYPE  destPixelFormat;
    MV3_TDS_TARGET_TYPE  destTarget;

    mvUint16            destBlockX;
    mvUint16            destBlockY;
    mvUint16            destImageWidth;    // Don't need in MV3019
    mvUint32            destStartAddress;
} BITBLT_INFO_STRUCT;

/* Library 1.10 update */
typedef enum {
    OSD_ALPHA_ZONE0 = 0x0000,
    OSD_ALPHA_ZONE1 = 0x2000,
    OSD_ALPHA_ZONE2 = 0x4000,
    OSD_ALPHA_ZONE3 = 0x6000,
    OSD_ALPHA_ZONE4 = 0x8000,
    OSD_ALPHA_ZONE5 = 0xA000,
    OSD_ALPHA_ZONE6 = 0xC000,
    OSD_ALPHA_ZONE7 = 0xE000
} MV3_OSD_ALPHA_ZONE;

// Lib 1.22 : Update by Richard for 1824 convert function
typedef enum{
    MV3_BIT_DEPTH_16 = 0,
    MV3_BIT_DEPTH_18,
    MV3_BIT_DEPTH_24
}MV3_SRC_BIT_DEPTH_ENUM;

typedef struct{
    mvUint16*                       srcOsdImg;   // 16Bit only = RGB16
    mvUint16*                       dstOsdImg;
    mvUint32                         srcImgSize;
    mvUint32                         chromaKey;
    MV3_OSD_ALPHA_ZONE	transZone1;
    MV3_OSD_ALPHA_ZONE	transZone2;
// Lib 1.22 : Update by Richard for 18bit or 24bit convert function
    mvUint32*                       src24BitOsdImg;  // 18Bit or 24Bit only = RGB18 or RGB24
    MV3_SRC_BIT_DEPTH_ENUM      srcBitDepth;        
}MV3_IMG_CONVERT_STRUCT;

// Lib 1.23 : Insert OSD type selection enumeration variable
typedef enum{
    MV3_OSD_SRC_TYPE_PALLETE = 0,
    MV3_OSD_SRC_TYPE_REAL        
}MV3_OSD_SRC_TYPE_ENUM;

#if 0
typedef struct{
    mvUint8     Zone0Transparency;
    mvUint8     Zone1Transparency;
    mvUint8     enable;
    mvUint16    Voffset;
    mvUint16    Hoffset;
    mvUint16    Width;
    mvUint16    Height;
    mvUint8     OverlayCb0;
    mvUint8     OverlayCr0;
    mvUint8     OverlayY0;

    mvUint8     OverlayCb1;
    mvUint8     OverlayCr1;
    mvUint8     OverlayY1;

    mvUint8     bitmapAddr;
    mvUint16    *bitmapBuf;

}MV3_TIMESTAMP_STRUCT;
#else
typedef struct{
    mvUint16    Voffset;
    mvUint16    Hoffset;
    mvUint16    Width;
    mvUint16    Height;

    mvUint16	textColor;
    mvUint16	backgroundColor;

    MV3_TIMESTAMP_TEXT_TRANS_TYPE	textTransparency;
    MV3_TIMESTAMP_BG_TRANS_TYPE		backgroundTransparency;

    mvUint8	bEnable;
    mvUint8     bitmapAddr;

    mvUint16    *bitmapBuf;
}MV3_TIMESTAMP_STRUCT;
#endif


typedef struct {
    mvUint8		isPllSet;
    mvUint16	pllInput;
    mvUint16	pllOutput;
} MV3_PLL_SET_STRUCT;


typedef struct {
    mvUint8     lcdDataHoldCon;
    mvUint8     lcdHighLWEN;		/* Sub Lcd WEN High period Time */
    mvUint8     lcdLowLWEN;			/* Sub Lcd WEN Low period Time  */
    mvUint8     lcdPolarityLRS;		/* Sub Lcd RS polarity */

    MV3_LCD_OUTMODE_TYPE        lcdOutMode;			/* PIXEL_ALIGN, BUS_ALIGN, MERGE */
    MV3_LCD_OUTMODE_DATA_LENGTH lcdDataWidth;		/* 0 - 8bits, 1 - 16bits, 2 - 18bits */
    MV3_LCD_OUTMODE_COMD_SEL    lcdComdSel;			/* type 0 ~ 3 */
    MV3_LCD_OUTMODE_COLOR       lcdTargetColor;		/* 0 - 256bits, 1 - 16bits, 2 - 18bits */

} MV3_LCD_SETTING;



/*----------------------------------------------------------
    Wrapper structures for mv3BitBlt
-----------------------------------------------------------*/
typedef struct {
    mvUint8             funSel;
    mvUint8             rop;
    mvUint16            chromaMask;

    mvUint16            chromaData;
    mvUint16            patternData;

    mvUint8             alpha7;
    mvUint8             alpha6;
    mvUint8             alpha5;
    mvUint8             alpha4;

    mvUint8             alpha3;
    mvUint8             alpha2;
    mvUint8             alpha1;
    mvUint8             alpha0;

    mvUint16            src1ImageWidth;

    mvUint16            src1RatioX;
    mvUint16            src1RatioY;

    mvUint32            src1StartYaddr;
#define src1StartUaddr src1OffsetU
#define src1StartVaddr src1OffsetV
    mvUint32            src1OffsetU;
    mvUint32            src1OffsetV;

    mvUint16            src2ImageWidth;
    mvUint16            src2RatioX;
    mvUint16            src2RatioY;

    mvUint16            src2Window1InitialX;
    mvUint16            src2Window1InitialY;
    mvUint16            src2Window1EndX;
    mvUint16            src2Window1EndY;

    mvUint16            src2Window2InitialX;
    mvUint16            src2Window2InitialY;
    mvUint16            src2Window2EndX;
    mvUint16            src2Window2EndY;
#ifdef FEATURE_MV3019
    mvUint16            src2Window3InitialX;
    mvUint16            src2Window3InitialY;
    mvUint16            src2Window3EndX;
    mvUint16            src2Window3EndY;

    mvUint16            src2Window4InitialX;
    mvUint16            src2Window4InitialY;
    mvUint16            src2Window4EndX;
    mvUint16            src2Window4EndY;
#endif
    mvUint32            src2StartAddr;
#ifdef FEATURE_MV3019
    mvUint32            osdWindow1StartAddr;
    mvUint32            osdWindow2StartAddr;
    mvUint32            osdWindow3StartAddr;
    mvUint32            osdWindow4StartAddr;

    mvUint8             osdWindow1ImgType;
    mvUint8             osdWindow2ImgType;
    mvUint8             osdWindow3ImgType;
    mvUint8             osdWindow4ImgType;
#endif

    MV3_OUT_FORMAT_TYPE  src1PixelFormat;
    MV3_OUT_FORMAT_TYPE  destPixelFormat;
    MV3_TDS_TARGET_TYPE  destTarget;

    mvUint16            destBlockX;
    mvUint16            destBlockY;
    mvUint16            destImageWidth;    // Don't need in MV3019
    mvUint32            destStartAddress;
} MV3_BITBLT_INFO_STRUCT;

/*----------------------------------------------------------
    Function Prototype Declaration
-----------------------------------------------------------*/
extern void             mv3MainLcdInit(mvUint16 sax, mvUint16 say);
extern void             mv3SubLcdInit(mvUint16 sax, mvUint16 say);

extern void mv3SetMainLcdStart(mvUint16 sax, mvUint16 say);
//extern void mv3WriteDataToMainLcd(mvUint32 data);
extern void             mv3SetMainLcdWindow(mvUint16 sax,
                                            mvUint16 say,
                                            mvUint16 eax,
                                            mvUint16 eay);

extern void mv3SetSubLcdStart(mvUint16 sax, mvUint16 say);
extern void mv3WriteDataToSubLcd(mvUint32 data);
extern void             mv3SetSubLcdWindow(mvUint16 sax,
                                           mvUint16 say,
                                           mvUint16 eax,
                                           mvUint16 eay);

extern void mv3SensorOn(void);
extern void mv3SensorOff(void);

extern void mv3ChipClkOn(void);
extern void mv3ChipClkOff(void);

extern mvStatus mv3InitSensor(void);

// FUNCTION_CONNECT_STRUCT erased
extern mvStatus mv3Reset(int);
extern mvStatus mv3DebugOut(int dispLvl, char *fmt, ...);
extern mvStatus mv3Delay100us(int time);
extern mvStatus mv3GetTimeTick(void);


/*----------------------------------------------------------
    Function Prototype Declaration
-----------------------------------------------------------*/
extern mvUint16 gTSimgBuffer[];  // Time Stamp image buffer for image convert.

extern void mv3LcdBankSelect(void);

extern MV3_RETURN_TYPE  mv3InitMV400(void);
extern MV3_RETURN_TYPE mv3DevicePropSet(MV3_PROP_TYPE propertyID, void * parm);

extern MV3_RETURN_TYPE mv3PowerDown(MV3_POWERDOWN_TARGET_TYPE target);

extern MV3_RETURN_TYPE  mv3MemoryWrite(mvUint32 addr,
                                       mvUint16 *pData,
                                       mvUint32 length);
extern MV3_RETURN_TYPE  mv3MemoryRead(mvUint16 *pData,
                                      mvUint32 addr,
                                      mvUint32 length);

extern MV3_RETURN_TYPE mv3LcdCmdControl(mvUint16 * lcdCommand);
extern void mv3GammaWrite(void);

// I2C Read/Write
extern MV3_RETURN_TYPE  mv3I2CWrite(mvUint8 sensorID,
                                    mvUint8 *pSensorData,
                                    MV3_I2C_NUM_TYPE length);
extern mvInt32          mv3I2CRead(mvUint8 sensorID,
                                   mvUint8 *pSensorData,
                                   MV3_I2C_NUM_TYPE i2cLength);

// Preview
extern MV3_RETURN_TYPE mv3PreviewPropSet(MV3_PROP_TYPE propertyID, void * parm);
extern MV3_RETURN_TYPE mv3PreviewPropGet(MV3_PROP_TYPE propertyID, void * parm);
extern MV3_RETURN_TYPE mv3PreviewWork(mvBoolean flgStartOrStop);

// MpegView
extern MV3_RETURN_TYPE mv3MpegViewPropSet(MV3_PROP_TYPE propertyID, void * parm);
extern MV3_RETURN_TYPE mv3MpegViewWork(mvBoolean flgStartOrStop);
extern MV3_RETURN_TYPE  mv3MpegViewRead(mvUint16 *mpegImage);


// Capture
extern MV3_RETURN_TYPE mv3CapturePropSet(MV3_PROP_TYPE propertyID, void * parm);
extern MV3_RETURN_TYPE mv3CapturePropGet(MV3_PROP_TYPE propertyID, void * parm);
extern MV3_RETURN_TYPE mv3CaptureWork(void);

// Encode
extern MV3_RETURN_TYPE mv3EncodePropSet(MV3_PROP_TYPE propertyID, void * parm);
extern MV3_RETURN_TYPE mv3EncodePropGet(MV3_PROP_TYPE propertyID, void * parm);
extern MV3_RETURN_TYPE mv3EncodeWork(void);

// Snapshot
extern MV3_RETURN_TYPE mv3SnapshotPropSet(MV3_PROP_TYPE propertyID, void * parm);
extern MV3_RETURN_TYPE mv3SnapshotPropGet(MV3_PROP_TYPE propertyID, void * parm);
extern MV3_RETURN_TYPE mv3SnapshotWork(mvUint16 *snap);

// Decode
extern MV3_RETURN_TYPE mv3DecodePropSet(MV3_PROP_TYPE propertyID, void * parm);
extern MV3_RETURN_TYPE mv3DecodePropGet(MV3_PROP_TYPE propertyID, void * parm);
extern MV3_RETURN_TYPE mv3DecodeWork(void);

// MJpeg
extern MV3_RETURN_TYPE mv3MJpegEncPropSet(MV3_PROP_TYPE propertyID, void * parm);
extern MV3_RETURN_TYPE mv3MJpegEncPropGet(MV3_PROP_TYPE propertyID, void * parm);

extern MV3_RETURN_TYPE mv3MJpegEncWork(mvBoolean flgStartOrStop);
extern MV3_RETURN_TYPE mv3MJpegDecWork(mvBoolean flgStartOrStop);
extern MV3_RETURN_TYPE  mv3MJpegEncRead(mvUint16 *mjpegImage);
extern MV3_RETURN_TYPE  mv3MJpegDecWrite(MV3_JPEG_STRUCT mjpeg);

// BitBlt
extern MV3_RETURN_TYPE mv3BitBltChromaSet(MV3_PROP_TYPE propertyID, mvUint8 parm);
extern MV3_RETURN_TYPE mv3BitBlt(BITBLT_INFO_STRUCT *pStructBitBlt);

// GPIO related
extern MV3_RETURN_TYPE mv3GpioSetDir(mvUint16 value);
extern mvUint16 mv3GpioGetDir(void);
extern MV3_RETURN_TYPE mv3GpioSetData(mvUint16 value);
extern mvUint16 mv3GpioGetData(void);

extern MV3_RETURN_TYPE  mv3JpegInfoGet(mvUint16 *jpegData,
                                       mvUint32 jpegLength,
                                       MV3_JPEG_INFO_STRUCT *jpegInfoStruct);

extern mvUint8 mv3PreviewZoomMaxFind(void);
extern mvUint8 mv3MpegViewZoomMaxFind(void);
extern mvUint8 mv3ZoomSrcMaskInit(float incrementRatio);

extern void mv3ChangePvsyncPolarity(mvBoolean bRisingEdge);
extern void mv0PllSet(mvUint16 pllIn,
                                  mvUint16 pllOut,
                                  mvUint8 bPllEnable);

// OSD and SI image format convert
extern void mv3ConvertOsdImg(MV3_IMG_CONVERT_STRUCT *imgConvert);
extern MV3_RETURN_TYPE mv3ConvertOsdImg1824(MV3_IMG_CONVERT_STRUCT *imgConvert);

//JPEG Decode patch
extern mvBoolean mv3JpegHeaderAnalysis(MV3_JPEG_INFO_STRUCT *jpginquiry,
								mvUint8 *buf,
								mvUint32 *size);

extern mvBoolean mv3JpegWrite(MV3_JPEG_INFO_STRUCT *jinfo,
                              mvUint8 *jpg,
                              mvUint32 *size,
                              mvUint32 jd_in_addr);


// Lib 1.15 : Add two functions for ESD prevention
extern mvBoolean mv3CheckESD_IsHrefCountOK(void);
extern mvBoolean mv3CheckESD_IsPreviewStatus(void);
extern void mv3SetHrefCount(mvUint32 nSensorHeight);


#endif /*__MV3LIB_H__*/



