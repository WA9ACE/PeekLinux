/*-------------------------------------------------------------------
File name : mv3StructDef.h

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

/*-------------------------------------
    The structures for byte swapping
--------------------------------------*/
typedef union {
    mvUint16 value;
    struct {
        mvUint8 l;		// Low
        mvUint8 h;		// High
    } w;
} WORD_STRUCT;

typedef union {
    mvUint32 value;
    struct {
        mvUint16 l;		// low
        mvUint16 h;		// High
    } w;
} DWORD_STRUCT;

typedef union {
    mvUint32 value;
    struct {
        mvUint8 b3;		// Low
        mvUint8 b2;
        mvUint8 b1;
        mvUint8 b0;		// High
    } i;
} INT_STRUCT;

typedef enum {
    MV3_POWER_FAIL      = -100,
    MV3_POWER_DOWN      = 0x00,
    MV3_POWER_MEMORY    = 0x10,
    MV3_POWER_BITBLT    = 0x20,
    MV3_POWER_MAKEJPEG  = 0x21,
    MV3_POWER_PREVIEW   = 0x22,
    MV3_POWER_MPEGVIEW  = 0x23,
    MV3_POWER_CAPTURE   = 0x24,
    MV3_POWER_SNAPSHOT  = 0x25,
    MV3_POWER_DECODEJPEG= 0x26,
    MV3_POWER_MJPEG_DE  = 0x30,
    MV3_POWER_MJPEG_EN  = 0x31
} MV3_POWER_STATE;

typedef enum {
    MV_INDEX_WIDTH,
    MV_INDEX_HEIGHT,
    MAX_NUM_ELEMENTS
} MV3_SIZE_INDEX_TYPE;

typedef struct {
    mvUint16  srcMaskStartX;			/* MV301x Scroll Command HOFFSET */
    mvUint16  srcMaskStartY;			/* MV301x Scroll Command VOFFSET */

    mvUint16  srcMaskWidth;				/* MV301x Preview HWIDTH */
    mvUint16  srcMaskHeight;			/* MV301x Preview VWIDTH */

    mvUint16  dstWidth;					/* Set LCD HSIZE, Capture HWIDTH */
    mvUint16  dstHeight;				/* Set LCD VSIZE, Capture VWIDTH */

	// If the mask size is not a number of multiple of 8 in decode(dummy2 deleted)
    mvUint8   dx;						/* dstWidth left size */
    mvUint8   dy;						/* dstHeight left size */

    mvUint8   shrMode;					/* SHRMODE */
	mvUint8   dummy1;					/* for 4 alignment */
	//mvUint16  dummy2;					/* for 4 alignment */

    mvUint16  ratioH;					/* MV301x Preview HRATIO */
    mvUint16  ratioV;					/* MV301x Preview VRATIO */

// Lib 1.20 : insert by prime
    mvUint8   autoScale;				/* Auto Scale On=1/Off=0 */
} SCALE_STRUCT;

/*-------------------------------------
    The structures for MV301x Control
--------------------------------------*/
typedef struct {
	SCALE_STRUCT scaleFactor;

/* rotate 90, 270 */
    mvUint16  orgDstWidth;				/* Preview size that cannot be affected by rotation */
    mvUint16  orgDstHeight;				/* Preview size that cannot be affected by rotation */

    mvUint8   outFormat;				/* 0 : RGB565, 1 : reserved, 2 : YUV422, 3 : YUV420 */
    mvUint8   flgSubLcd;				/* 1 : Sub LCD Preview, 0 : Main LCD Preview */
} PREVIEW_STRUCT;

typedef struct {
    mvUint8   qtable;					/* 0 ~ 255 in case of MV301x */
    MV3_SIZE_STRUCT destSize;

#ifdef FEATURE_MV3018
    mvUint32  encodeInAddr;				/* Raw image address to Encode to jpeg image. */
    mvUint32  encodeOutAddr;			/* Encoded jpeg image address */
#elif defined(FEATURE_MV3019)
#define encodeInAddr    encodeInStartAddr
#define encodeOutAddr   encodeOutStartAddr

    mvUint32 encodeInStartAddr;
    mvUint32 encodeInEndAddr;
    mvUint32 encodeOutStartAddr;
    mvUint32 encodeOutEndAddr;

    mvUint16 *inImage;
    mvUint16 *outJpeg;
#endif

    MV3_TIMESTAMP_STRUCT timeStamp;
} JPEG_ENCODE_STRUCT;

typedef struct {
	SCALE_STRUCT scaleFactor;
	mvUint16 extStartX; 		// Added By KJ Lee 640*480 JPEG을 320*240 만들기 위해
	mvUint16 extStartY;			// Mask를 하여 부분적으로 Decode & 조합을 해야함. 그러기 위해서는
								// Start X, Start Y가 JPEG_MASK property에서 정해져야하는데
								// scrollParaGet에서 유실되므로 이것을 지키기 위해 변수를 새로 둠..
    MV3_OUT_FORMAT_TYPE   outFormat;	/* Decode Out Format */

    mvUint32 vsyncPeriod;
    mvUint8   flgMirror;				/* 1 : MIRROR, 0 : NORMAL  */
    mvUint8   rotate;					/* CW, 3 : 90, 2 : 180 1 : 270, 0 : 0*/
    mvUint8	  flgFlip;

#ifdef FEATURE_MV3018
    mvUint32  decodeInAddr;				/* Jpeg image address to Decode to Raw image */
    mvUint32  decodeOutAddr;			/* Decoded raw image address */
#elif defined(FEATURE_MV3019)

#define decodeInAddr    decodeInStartAddr
#define decodeOutAddr   decodeOutStartAddr

    mvUint32  decodeInStartAddr;		/* Jpeg image Start address to Decode to Raw image */
    mvUint32  decodeInEndAddr;			/* Jpeg image End address to Decode to Raw image */
    mvUint32  decodeOutStartAddr;		/* Decoded raw Start image address */
    mvUint32  decodeOutEndAddr;			/* Decoded raw End image address */

    mvUint16  *rawImage;
#endif
} JPEG_DECODE_STRUCT;

typedef struct {
    mvUint8     gammaA;
    mvUint8     gammaB;
    mvUint8     gammaC;
    mvUint8     gammaD;
    mvUint8     gammaE;

    mvUint8     reserved1;
    mvUint8     reserved2;
    mvUint8     reserved3;
} MV3_GAMMA_SET;

typedef struct {
    mvUint32  deviceID;					/* Variable for MV3 Device identification */
    MV3_POWER_STATE pwrState;			/* mv30xx power State */

    PREVIEW_STRUCT preview;				/* Variable for Preview */
    PREVIEW_STRUCT capture;				/* Variable for Capture */

    mvUint8   flgMirror;				/* 1 : MIRROR, 0 : NORMAL  */
    mvUint8   rotate;					/* CW, 3 : 90, 2 : 180 1 : 270, 0 : 0*/
    mvUint8   flgFlip;					/* 1 : FLIP , 0 : NORMAL */
#ifdef FEATURE_MV3019
    mvUint8   isDoubleBuffer;
    mvUint32  oddAddr;
#endif
    JPEG_ENCODE_STRUCT encode;			/* Variable for MakeJpeg */
    JPEG_DECODE_STRUCT decode;			/* Variable for Decode */

    mvUint8   flgSuperimpose;			/* 1 : Superimpose Set */

    mvUint8   flgSuperimposeUpdate;
    mvUint16  *superimposeImage;
    mvUint16  superimposeKeyValue;

    mvUint32  superimposeCheckPoint1;
    mvUint32  superimposeCheckPoint2;

    mvUint16  *bitmapImage;				/* Bitmap Mask Image */
    mvUint8   motionSpeed;
    mvUint8   flgExpansionMode;

	// Added to encode after raw data is inserted.
    mvUint16  *rawImage;
    mvUint32  rawLength;

    mvUint16  *jpegImage;
    mvUint32  jpegLength;

    MV3_JPEG_INFO_STRUCT jpeg;				/* Save Inquiry Results */

    MV3_OSD_STRUCT osd;

    MV3_IMAGE_EFFECT_TYPE   imgEffect;		/* Image Effect */

    mvUint16  sepiaColor;
    mvUint16  colorSelStart;
    mvUint16  colorSelEnd;


#ifdef FEATURE_MV3018
    mvUint8     filterEn;
#elif defined(FEATURE_MV3019)
    mvUint8     ycSel;
    mvUint8     enable3x3Filter;
    mvUint8     filter3x3[9];
    mvUint8     cSmoothEn;
    mvUint8     invertY;
    mvUint8     strongC;
#endif
    mvUint8     flgGammaEn;
    mvUint16    lastMask;

    MV3_GAMMA_SET   gamma;

    MV3_TARGET_TYPE target;			/* 0 : SRAM, 1 : MAIN_LCD, 2 : SUB_LCD */
    mvUint16  lcdStartX;				/* the position of LCD to Preveiew/Decode */
    mvUint16  lcdStartY;

    mvUint16  lcdWidth;					/* Variable for LCD Max Width */
    mvUint16  lcdHeight;				/* Variable for LCD Max Height */
    mvUint16  subLcdWidth;				/* Variable for Sub LCD Max Width */
    mvUint16  subLcdHeight;				/* Variable for Sub LCD Max Height */

    MV3_LCD_OUTMODE				subLCDMode;
    MV3_LCD_OUTMODE_COMD_SEL	subLcdComdSel;       /* type 0 ~ 3 */

    MV3_LCD_OUTMODE				mainLCDMode;
    MV3_LCD_OUTMODE_COMD_SEL	mainLcdComdSel;      /* type 0 ~ 3 */

    MV3_LCD_OUTMODE_TYPE        subLcdOutMode;		/* PIXEL_ALIGN, BUS_ALIGN, MERGE */
    MV3_LCD_OUTMODE_DATA_LENGTH subLcdDataWidth;	/* 0 - 8bits, 1 - 16bits, 2 - 18bits */
    MV3_LCD_OUTMODE_COLOR       subLcdTargetColor;	/* 0 - 256bits, 1 - 16bits, 2 - 18bits */

    MV3_LCD_OUTMODE_TYPE        mainLcdOutMode;		/* PIXEL_ALIGN, BUS_ALIGN, MERGE */
    MV3_LCD_OUTMODE_DATA_LENGTH mainLcdDataWidth;	/* 0 - 8bits, 1 - 16bits, 2 - 18bits */
    MV3_LCD_OUTMODE_COLOR       mainLcdTargetColor;	/* 0 - 256bits, 1 - 16bits, 2 - 18bits */

// Lib v1.22 update by Seth and KJ Lee : for OPM and Restrict property
    mvUint16  lcdOffsetX;                           /*Preview Start X point Flag for OPM */
    mvUint16  lcdOffsetY;                           /*Preview Start Y point Flag for OPM */
    mvBoolean opmflg;                               /* OPM Operating Flag */

    mvBoolean jpegRestrictEn;                   /* OPM Operating Flag */
    mvUint32 jpegRestrictedSize;               /* JPEG Restrict size */

    mvUint32	memPtrStep;

    mvUint8		subLcdWriteSpeed;			/* Sub LCD Write Speed */
    mvUint8		mainLcdWriteSpeed;			/* Main LCD Write Speed */

    mvUint8		subLcdPolarityLRS;			/* Sub Lcd RS polarity */
    mvUint8		subLcdHighLWEN;				/* Sub Lcd WEN High period Time */
    mvUint8		subLcdLowLWEN;				/* Sub Lcd WEN Low period Time  */

    mvUint8		mainLcdPolarityLRS;			/* Sub Lcd RS polarity */
    mvUint8		mainLcdHighLWEN;			/* Sub Lcd WEN High period Time */
    mvUint8		mainLcdLowLWEN;				/* Sub Lcd WEN Low period Time  */

    mvUint8		sensorID;

    mvUint16	sensorWidth;				/* Variable for sensor Max Width */
    mvUint16	sensorHeight;				/* Variable for sensor Max Height */

    mvUint8		sonySensor;					/* Variable for Sony Sensor */
    mvUint8		sensorVsync;				/* Variable for Vsync Polarity on Initial */
    mvUint8		sensorHsync;
    mvUint16	sclkPeriod;
    mvUint8		sensorVsyncFalling;
#ifdef FEATURE_MV3019
    mvUint8		sensorI2C_SkipStop;
#endif
// Lib 1.24 : Inserted
    mvUint32 pclk_inv;


    mvUint8   mclkDivision;				/* Variable for Main Clock Division on Initial */
    mvUint8   smclkDivision;			/* Variable for Sensor Main Clock Division on Initial*/
    mvUint8   pclkSelect;				/* Variable for Clock Select*/
    mvUint8   flgMv3PwrOn;				/* Flag for MV301x Power */
    mvUint8   flgSensorPwrOn;			/* Flag for Sensor Power */
    mvUint8   flgJpegEndian;			/* Flag for Jpeg Endian */

    BITBLT_INFO_STRUCT bitBlt;			/* for BitBLT Operation */
    mvUint8   senClkSelect;				/* Variable for Sensor Clock Select*/

    mvUint8   flgUseIntClock;			/* Flag for setting clock on sensor-On */
										/* 1 - use internal clock on sensor-On(preview, capture) */
										/* 0 - use pclk of sensor on sensor-On(preview, capture) */
    mvUint8   mv3ZoomCurrent;			/* Variable for Preview Zoom */

    mvUint16  *jpegIntervalTable;		/* Pointer for Multishot/Mjpeg time interval storage buffer */
    mvUint32  *jpegOffsetTable;			/* Pointer for MultiShot/Mjpeg Size Storage Buffer */
    mvUint16  jpegCount;				/* Variable for MultiShot Count */

    mvUint32  encodeBase;				/* Variable for Multishot/MJPEG Record Base Address */

    mvUint32  memSize;                           /* Parameter to identify internal memory */

    mvUint32 bltChromaKey;

    mvUint32 offsetY;
    mvUint32 offsetU;
    mvUint32 offsetV;

    } MV3_INFO_STRUCT;