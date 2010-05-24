/*-------------------------------------------------------------------
File Name : MV301xDef.h

MV301x middle level camera control routines

(C) Copyright MtekVision, Inc. 2003~2004 All right reserved.
  Confidential Information

All parts of the MtekVision Program Source are protected by copyright law 
and all rights are reserved. 
This documentation may not, in whole or in part, be copied, photocopied, 
reproduced, translated, or reduced to any electronic medium or machine 
readable form without prior consent, in writing, from the MtekVision. 

Last modification : 2005. 03. 05
----------------------------------------------------------------------*/

/*----------------------------------------------------------
    File Name   : mv301xDef.h
    Description : mv301x series definition file
    Created by  : Mtekvision Application Team
-----------------------------------------------------------*/

/*---------------------------------------------------------------------
    MV301x SRAM ADDRESS
----------------------------------------------------------------------*/
#define MV301x_PORT_LCD_RS           (MV3_BYTE_SPREAD * 0)
#define MV301x_PORT_MAIN_LCD         (MV3_BYTE_SPREAD * 1)
#define MV301x_PORT_SUB_LCD          (MV3_BYTE_SPREAD * 2)

#define MV301x_PORT_BUFFER_ADDR_H    (MV3_BYTE_SPREAD * 0)
#define MV301x_PORT_BUFFER_ADDR_L    (MV3_BYTE_SPREAD * 1)
#define MV301x_PORT_BUFFER_DATA      (MV3_BYTE_SPREAD * 2)

#define MV301x_PORT_REGISTER_ADDR    (MV3_BYTE_SPREAD * 0)
#define MV301x_PORT_REGISTER_PAR1    (MV3_BYTE_SPREAD * 1)
#define MV301x_PORT_REGISTER_PAR0    (MV3_BYTE_SPREAD * 2)

#define MV301x_PORT_SPECIAL_POWER    (MV3_BYTE_SPREAD * 0)

#define MV301x_PORT_SPECIAL_PDATA    (MV3_BYTE_SPREAD * 1)
#define MV301x_PORT_SPECIAL_PDIR     (MV3_BYTE_SPREAD * 2)

#define MV301x_PORT_BANK_SELECT      (MV3_BYTE_SPREAD * 3)

#ifdef FEATURE_MV3018B
#define MV301x_DEVICE_ID             0x301800B0
#elif defined(FEATURE_MV3019) 
#define MV301x_DEVICE_ID             0x301900A0   
#elif defined(FEATURE_MV3020)
#define MV301x_DEVICE_ID             0x301900A1
#endif


#define MV301x_BANK_SELECT(x)    MV3_WRITE(MV301x_PORT_BANK_SELECT, x);

#define MV301x_GPIO_CONTROL(x)   { \
                                    MV301x_BANK_SELECT(MV301x_BANK_SPECIAL_CONTROL); \
                                    MV3_WRITE(MV301x_PORT_SPECIAL_PDIR, x); \
                                }

#define MV301x_GPIO_DATA(x)      { \
                                    MV301x_BANK_SELECT(MV301x_BANK_SPECIAL_CONTROL); \
                                    MV3_WRITE(MV301x_PORT_SPECIAL_PDATA, x); \
                                }


/*---------------------------------------------------------------------
    MV301x SRAM ADDRESS
----------------------------------------------------------------------*/
typedef enum {
    MV301x_BANK_LCD_CONTROL,
    MV301x_BANK_MEMORY_CONTROL,
    MV301x_BANK_REGISTER_CONTROL,
    MV301x_BANK_SPECIAL_CONTROL
} MV301x_BANK_SELECT_TYPE;

typedef enum {
    MV301x_MEMORY_BURST_READ,
    MV301x_MEMORY_SINGLE_READ,
    MV301x_MEMORY_WRITE_OP_EN
} MV301x_MEMORY_ACCESS_TYPE;

#define MV301x_MEMORY_ACCESS_MASK 0xC0000000


/*=====================================================================
  MV301x Register Definitions
=====================================================================*/
typedef enum {
    MV301x_PREVIEW,
    MV301x_BITBLT,
    MV301x_MPEGVIEW,
    MV301x_SNAPSHOT,
    MV301x_CAPTURE,
    MV301x_MAKEJPEG,
    MV301x_DECODEJPEG,
    MV301x_DISPLAYJPEG,
    MV301x_MJPEGENC,
    MV301x_MJPEGDEC
} MV301x_SCENARIO;


/*=====================================================================
  MV301x Register Definitions
=====================================================================*/
typedef enum {
// MV Control Registers
    MV301x_REG_RESET_CTL = 0x00,
    MV301x_REG_OPERATION_CTL,
    
    MV301x_REG_INTERRUPT_MASK = 0x02,
    MV301x_REG_INTERRUPT_STATUS,

    MV301x_REG_MOTION_SPEED = 0x04,
    MV301x_REG_MJPEG_DEC_VSYNC,
    
    MV301x_REG_MVC_CTRL = 0x06,
#ifdef FEATURE_MV3018
    MV301x_REG_CURR_VAL,
#elif defined(FEATURE_MV3019)
    MV301x_REG_CURR_SENSOR_VAL,
    MV301x_REG_CURR_MEM_ADDR,
#endif
    MV301x_REG_MEMORY_POINTER,
#ifdef FEATURE_MV3019
    MV301x_REG_LCD_FRAME_CTL,
#endif

    MV301x_REG_HOSTIF_PARAM1 = 0x1C,
    MV301x_REG_MISC_CTL,
    MV301x_REG_PLL_CTL,
    MV301x_REG_DEVICE_ID,

// Sensor Interface Registers
    MV301x_REG_SENSOR_CTL = 0x20,
    MV301x_REG_STROBE,
    MV301x_REG_FILTER,
    MV301x_REG_SENSOR_WIDTH,
    MV301x_REG_GAMMA,
    MV301x_REG_EFFECT_CTL,
    MV301x_REG_EFFECT_SEPIA,
    MV301x_REG_EFFECT_UV,
    MV301x_REG_P_MASK,
    MV301x_REG_P_OFFSET,
    MV301x_REG_S_MASK,
    MV301x_REG_S_OFFSET,
#ifdef FEATURE_MV3019
    MV301x_REG_MASK_FILTER0,
    MV301x_REG_MASK_FILTER1,
    MV301x_REG_MASK_FILTER2,
#endif

// I2C Control Registers
    MV301x_REG_I2C_CTL = 0x30,
    MV301x_REG_I2C_CTL0,
    MV301x_REG_I2C_CTL1,
    MV301x_REG_I2C_READ,

// P-Scaler Registers
    MV301x_REG_P_RATIO = 0x40,
    MV301x_REG_P_MODE,
    MV301x_REG_P_OUT_SIZE,
    
    MV301x_REG_P_START_ADDR = 0x4A,
    MV301x_REG_P_START_ADDR_U,
    MV301x_REG_P_START_ADDR_V,
#ifdef FEATURE_MV3019
    MV301x_REG_P_END_ADDR,
    MV301x_REG_P_CUR_ADDR,
#endif

// S-Scaler Registers
    MV301x_REG_S_SCALE = 0x60,
    MV301x_REG_S_OUT_SIZE,

#ifdef FEATURE_MV3019
    MV301x_REG_AF_CTL = 0x70,
    MV301x_REG_AF_AREA1,
    MV301x_REG_AF_AREA2,
    MV301x_REG_AF_USER_SIZE,
    MV301x_REG_AF_CLIP,
    MV301x_REG_AF_LPF,
    MV301x_REG_AF_RESULT0,
    MV301x_REG_AF_RESULT1,
    MV301x_REG_AF_YSUM0,
    MV301x_REG_AF_YSUM1,
#endif

// JPEG Encoder and Decoder Registers
    MV301x_REG_ENC_Q_TABLE = 0x80,
    MV301x_REG_ENC_SIZE,
#ifdef FEATURE_MV3019
    MV301x_REG_ENC_STATUS,
#endif
    MV301x_REG_ENC_OUT_ADDR = 0x83,
    MV301x_REG_ENC_OUT_LENGTH,
    MV301x_REG_DEC_IN_ADDR,
    MV301x_REG_DEC_SPEED,
    MV301x_REG_DEC_REAL_HV,
    MV301x_REG_DEC_MASK_HV,
    MV301x_REG_HEADER_INFO,
    MV301x_REG_CODEC_CONFIG,
#if defined(FEATURE_MV3019)

    MV301x_REG_CODEC_RESTART_ADDR,
    MV301x_REG_CODEC_END_ADDR,
    MV301x_REG_DEC_PAUSE_CTL,
    MV301x_REG_DEC_CUR_ADDR,
    MV301x_REG_DEC_STATUS,
#endif

    MV301x_REG_TSTAMP_CTL = 0x90,
    MV301x_REG_TSTAMP_OFFSET,
    MV301x_REG_TSTAMP_SIZE,
    MV301x_REG_TSTAMP_COLOR0,
    MV301x_REG_TSTAMP_COLOR1,
    MV301x_REG_TSTAMP_BITMAP_ADDR,
    MV301x_REG_TSTAMP_BITMAP_MEM_DATA,

// TD-Scaler Registers
    MV301x_REG_TD_INSTRUCTION = 0xA0,
    MV301x_REG_TD_CHROMA_DATA,
    MV301x_REG_TD_PATTERN_DATA,
    MV301x_REG_TD_ALPHA_DATA,
#ifdef FEATURE_MV3019
    MV301x_REG_TD_PALLET_POINT,
    MV301x_REG_TD_PALLET_DATA,
#endif
    MV301x_REG_TD_SRC1_CTL,
    MV301x_REG_TD_SRC1_IMG_WIDTH,
    MV301x_REG_TD_SRC1_SCALE,
    MV301x_REG_TD_SRC1_START_ADDR,
    MV301x_REG_TD_SRC1_START_ADDR_U,
    MV301x_REG_TD_SRC1_START_ADDR_V,
#ifdef FEATURE_MV3019
    MV301x_REG_TD_SRC1_END_ADDR,
    MV301x_REG_TD_SRC2_IMG_WIDTH,
    MV301x_REG_TD_SRC2_SCALE,
#elif defined(FEATURE_MV3018B)
    MV301x_REG_TD_SRC2_IMG_WIDTH = 0xAB,
    MV301x_REG_TD_SRC2_SCALE,
#endif
    MV301x_REG_TD_SRC2_WINDOW1_START,
    MV301x_REG_TD_SRC2_WINDOW1_END,
    MV301x_REG_TD_SRC2_WINDOW2_START,
    MV301x_REG_TD_SRC2_WINDOW2_END,
#ifdef FEATURE_MV3019
    MV301x_REG_TD_SRC2_WINDOW3_START,
    MV301x_REG_TD_SRC2_WINDOW3_END,    
    MV301x_REG_TD_SRC2_WINDOW4_START,
    MV301x_REG_TD_SRC2_WINDOW4_END,
#endif
    MV301x_REG_TD_SRC2_START_ADDR,
#ifdef FEATURE_MV3019
    MV301x_REG_TD_OSD_WINDOW1_START_ADDR,
    MV301x_REG_TD_OSD_WINDOW2_START_ADDR,
    MV301x_REG_TD_OSD_WINDOW3_START_ADDR,
    MV301x_REG_TD_OSD_WINDOW4_START_ADDR,
#endif
    MV301x_REG_TD_DST_CTL,
    MV301x_REG_TD_DST_SIZE,
#ifndef FEATURE_MV3019    
    MV301x_REG_TD_DST_WIDTH,
#endif
    MV301x_REG_TD_DST_START_ADDR,

// LCD Controler Registers
    MV301x_REG_LCD_MODE_CTL = 0xC0,
    MV301x_REG_LCD_CMD_CTL,
    MV301x_REG_LCD_CMD6,
    MV301x_REG_LCD_CMD5,
    MV301x_REG_LCD_CMD4,
    MV301x_REG_LCD_CMD3,
    MV301x_REG_LCD_CMD2,
    MV301x_REG_LCD_CMD1,
    MV301x_REG_LCD_CMD0
} MV301x_REGISTER_TYPE;

// MV301x RESET CONTROL
#define MV301x_I2C_RESET                        0x0080
#define MV301x_HOST_RESET                       0x0040
#define MV301x_LCD_RESET                        0x0020
#define MV301x_JPEG_RESET                       0x0010
#define MV301x_TDS_RESET                        0x0008
#define MV301x_SCALER_RESET                     0x0004
#define MV301x_MVC_RESET                        0x0002
#define MV301x_SW_RESET                         0x0001

// MV301x_OPERATION_CONTROL
#define MV301x_DISPLAY_BITBLT_EN                0x0800
#define MV301x_DECODE_EN                        0x0400
#define MV301x_BITBLT_EN                        0x0200
#define MV301x_MPEGVIEW_EN                      0x0100
#define MV301x_MJPEG_DEC_EN                     0x0080
#define MV301x_MJPEG_ENC_EN                     0x0040
#define MV301x_DISPLAY_JPEG_EN                  0x0020
#define MV301x_MAKE_JPEG_EN                     0x0010
#define MV301x_CAPTURE_EN                       0x0008
#define MV301x_SNAPSHOT_EN                      0x0004
#define MV301x_PREVIEW_EN                       0x0002
#define MV301x_I2C_EN                           0x0001

// MV301x_INTERRUPT_MASK
#define MV301x_VSYNC_INT_MASK                   0x80000000
// Lib 1.24 : Inserted
#ifdef FEATURE_MV3019
#define MV301X_JPEG_PAUSED_MASK                  0x02000000
#define MV301X_SCALER_NOTCOMPLETED_MASK 0x01000000
#endif
#define MV301x_HREFCNT_ERROR_MASK               0x00800000
#define MV301x_JPEG_ENC_ERROR_MASK              0x00400000
#define MV301x_JPEG_DEC_HEADER_FAIL_MASK        0x00200000
#define MV301x_JPEG_DEC_IMG_FAIL_MASK           0x00100000
#define MV301x_MEM_PTR_INT_MASK                 0x00080000
#define MV301x_MPEGVIEW_READY_MASK              0x00040000
#define MV301x_MJPEG_DEC_READY_MASK             0x00020000
#define MV301x_MJPEG_ENC_READY_MASK             0x00010000

#define MV301x_DISPLAY_BITBLT_MASK              0x00000800
#define MV301x_DECODE_MASK                      0x00000400
#define MV301x_BITBLT_MASK                      0x00000200
#define MV301x_MPEGVIEW_MASK                    0x00000100
#define MV301x_MJPEG_DEC_MASK                   0x00000080
#define MV301x_MJPEG_ENC_MASK                   0x00000040
#define MV301x_DISPLAY_JPEG_MASK                0x00000020
#define MV301x_MAKE_JPEG_MASK                   0x00000010
#define MV301x_CAPTURE_MASK                     0x00000008
#define MV301x_SNAPSHOT_MASK                    0x00000004
#define MV301x_PREVIEW_MASK                     0x00000002
#define MV301x_I2C_MASK                         0x00000001

// MV301x_INTERRUPT_STATUS
#define MV301x_VSYNC_INT                        0x80000000
// Lib 1.24 : Inserted
#ifdef FEATURE_MV3019
#define MV301X_JPEG_PAUSED                   0x02000000
#define MV301X_SCALER_NOTCOMPLETED  0x01000000
#endif
#define MV301x_HREFCNT_ERROR                    0x00800000
#define MV301x_JPEG_ENC_ERROR                   0x00400000
#define MV301x_JPEG_DEC_HEADER_FAIL             0x00200000
#define MV301x_JPEG_DEC_IMG_FAIL                0x00100000
#define MV301x_MEM_PTR_INT                      0x00080000
#define MV301x_MPEGVIEW_READY                   0x00040000
#define MV301x_MJPEG_DEC_READY                  0x00020000
#define MV301x_MJPEG_ENC_READY                  0x00010000

#define MV301x_DISPLAY_BITBLT_END               0x00000800
#define MV301x_DECODE_END                       0x00000400
#define MV301x_BITBLT_END                       0x00000200
#define MV301x_MPEGVIEW_END                     0x00000100
#define MV301x_MJPEG_DEC_END                    0x00000080
#define MV301x_MJPEG_ENC_END                    0x00000040
#define MV301x_DISPLAY_JPEG_END                 0x00000020
#define MV301x_MAKE_JPEG_END                    0x00000010
#define MV301x_CAPTURE_END                      0x00000008
#define MV301x_SNAPSHOT_END                     0x00000004
#define MV301x_PREVIEW_END                      0x00000002
#define MV301x_I2C_END                          0x00000001

// MV301x_MOTION_SPEED
#define MV301x_MOTION_SPEED_00SKIP              0x00
#define MV301x_MOTION_SPEED_01SKIP              0x01
#define MV301x_MOTION_SPEED_02SKIP              0x02
#define MV301x_MOTION_SPEED_03SKIP              0x03
#define MV301x_MOTION_SPEED_04SKIP              0x04
#define MV301x_MOTION_SPEED_05SKIP              0x05
#define MV301x_MOTION_SPEED_06SKIP              0x06
#define MV301x_MOTION_SPEED_07SKIP              0x07
#define MV301x_MOTION_SPEED_08SKIP              0x08
#define MV301x_MOTION_SPEED_09SKIP              0x09
#define MV301x_MOTION_SPEED_10SKIP              0x0a
#define MV301x_MOTION_SPEED_11SKIP              0x0b
#define MV301x_MOTION_SPEED_12SKIP              0x0c
#define MV301x_MOTION_SPEED_13SKIP              0x0d
#define MV301x_MOTION_SPEED_14SKIP              0x0e
#define MV301x_MOTION_SPEED_15SKIP              0x0f

// MV301x_MVC_CTRL
#define MV301x_MVC_HBUS_ERR_CHK_EN              0x08
#define MV301x_MVC_HBUS_WRITE                   0x04
#define MV301x_MVC_READY_INT                    0x02
#define MV301x_MVC_MVC_STATEMACHINE_CLEAR       0x01

// MV301x_MEM_POINTER
#ifdef FEATURE_MV3018
#define MV301x_POINTER_CHK_MASK                 0x0100
#define MV301x_POINTER_MASTER_MASK              0x00E0
#define MV301x_POINTER_RW_MASK                  0x0010
#else
#define MV301x_POINTER_CHK_MASK                 0x8000
#define MV301x_POINTER_MASTER_MASK              0x7000
#define MV301x_POINTER_RW_MASK                  0x0800
#endif

// MV301x TIME STAMP
#define MV301x_TIMESTAMP_ENABLE                 0x01
#define MV301x_TIMESTAMP_ZONE0_TRANS            0x02
#define MV301x_TIMESTAMP_ZONE1_TRANS            0x04
#define MV301x_TIMESTAMP_OVERLAY_Cb0            0x0000
#define MV301x_TIMESTAMP_OVERLAY_Cr0            0xFF00
#define MV301x_TIMESTAMP_OVERLAY_Y0             0x0000
#define MV301x_TIMESTAMP_OVERLAY_Cb1            0x0000
#define MV301x_TIMESTAMP_OVERLAY_Cr1            0xFF00
#define MV301x_TIMESTAMP_OVERLAY_Y1             0x0000

typedef enum
{
    MV301x_POINTER_P_SCALER,
    MV301x_POINTER_HOST_INTERFACE,
    MV301x_POINTER_JPEG,
    MV301x_POINTER_TD_SCALER
} MV301x_POINTER_MASTER_TYPE;

// MV301x_I2C_NUMBER
#define MV301x_SONY_MASTER                      0x08

#ifdef FEATURE_MV3019
// MV301x_SENSOR_CTL
#define MV301x_SENSOR_SCALER_OFF                0x0010
#endif
#define MV301x_SENSOR_VSYNC_FALLING             0x0008
#define MV301x_SENSOR_HSYNC_CONV                0x0004
#define MV301x_SENSOR_VSYNC_CONV                0x0002
#define MV301x_SENSOR_STROBE_ON                 0x0001

// MV301x_ENHANCE0
#define MV301x_ENHANCE0_FILTER_EN_V             0x0020
#define MV301x_ENHANCE0_FILTER_EN_H             0x0010
#define MV301x_ENHANCE0_EDGE_EN                 0x0004
#define MV301x_ENHANCE0_BLUR_EN                 0x0002
#define MV301x_ENHANCE0_GAMMA_EN                0x0001

// MV301x_P_SCALER
#define MV301x_P_SHR_MASK                       0x0300
#define MV301x_P_OUT_FORMAT_MASK                0x00C0
#define MV301x_P_SI_ON_MASK                     0x0008
#define MV301x_P_ROTATE_MASK                    0x0003 /* rotate + mirror // 0x0002*/

// MV301x_S_SCALER
#ifdef FEATURE_MV3019
#define MV301x_S_SHR_MASK                       0xC0000000
#define MV301x_S_VRATIO_MASK                    0x3FFF8000
#define MV301x_S_HRATIO_MASK                    0x00007FFF
#elif defined(FEATURE_MV3018)
#define MV301x_S_SHR_MASK                       0x30000000
#define MV301x_S_VRATIO_MASK                    0x0FFFC000
#define MV301x_S_HRATIO_MASK                    0x00003FFF
#endif

// MV301x_JPEG
#ifdef FEATURE_MV3019
#define MV301x_JPEG_EMB_BANK_NOSEL              0x0008
#define MV301x_JPEG_EMB_BANK_SEL                0x0000
#endif
#define MV301x_JPEG_ENC_HEADER_ON               0x0004
#define MV301x_JPEG_ENC_HEADER_OFF              0x0000
#define MV301x_JPEG_DECODE_ENDIAN_BIG           0x0002
#define MV301x_JPEG_DECODE_ENDIAN_LITTLE        0x0000
#define MV301x_JPEG_ENCODE_ENDIAN_BIG           0x0001
#define MV301x_JPEG_ENCODE_ENDIAN_LITTLE        0x0000

#ifdef FEATURE_MV3019
// MV301x_ENC_STATUS
#define MV301x_ENC_FLAG                         0x80000000
#define MV301x_ENC_PAUSE_LINE8                  0x40000000
#define MV301x_ENC_CUR_LINE8_NUM                0x0FF00000
#define MV301x_ENC_CUR_ADDR                     0x0003FFFF

// DEC_PAUSE_CTL (HIGH WORD)
#define MV301x_DEC_PAUSE_LINE8_NUM              0xFF00
#define MV301x_DEC_NO_WRAPPING_MODE             0x0010
#define MV301x_DEC_PAUSE_MEMORY_SIZE            0x0003

// MV301x_DEC_STATUS
#define MV301x_DEC_FLAG                         0x40000000
#define MV301x_DEC_PAUSE_LINE8                  0x20000000
#define MV301x_DEC_PAUSE_MSIZE                  0x10000000
#define MV301x_DEC_CURR_LINE8_NUM               0x0FF00000
#define MV301x_DEC_DIFF_ADDRESS_MAX             0x0003FFFF
#endif

// MV301x_TD_SCALER
#define MV301x_TDS_INST_FUNC_SEL_MASK           0x0070
#define MV301x_TDS_PIXEL_FORMAT_MASK            0x0030
#define MV301x_TDS_SRC_RATIO_Y                  0xFF00
#define MV301x_TDS_SRC_RATIO_X                  0x00FF

#define MV301x_TDS_ALPHA7                       0xF000
#define MV301x_TDS_ALPHA6                       0x0F00
#define MV301x_TDS_ALPHA5                       0x00F0
#define MV301x_TDS_ALPHA4                       0x000F
#define MV301x_TDS_ALPHA3                       0xF000
#define MV301x_TDS_ALPHA2                       0x0F00
#define MV301x_TDS_ALPHA1                       0x00F0
#define MV301x_TDS_ALPHA0                       0x000F

#ifdef FEATURE_MV3019
#define MV301x_TDS_OSD_WND1_IMG_TYPE            0x0100
#define MV301x_TDS_OSD_WND2_IMG_TYPE            0x0200
#define MV301x_TDS_OSD_WND3_IMG_TYPE            0x0400
#define MV301x_TDS_OSD_WND4_IMG_TYPE            0x0800
#endif

// LCD Controller Registers
#ifdef FEATURE_MV3019
#define MV301x_LCD_DATA_HOLD_CON                0x0400
#define MV301x_LCD_LWEN_HIGH_MASK               0x03E0
#define MV301x_LCD_LWEN_LOW_MASK                0x001F
#elif defined(FEATURE_MV3018)
#define MV301x_LCD_DATA_HOLD_CON                0x0010
#define MV301x_LCD_LWEN_HIGH_MASK               0x000F
#define MV301x_LCD_LWEN_LOW_MASK                0xF000
#endif
#define MV301x_LCD_LRS_POLARITY_MASK            0x0800
#define MV301x_LCD_OUTMODE_MASK                 0x0700
#define MV301x_LCD_COMD_SEL_MASK                0x00C0
#define MV301x_LCD_DATA_WIDTH_MASK              0x0030
#define MV301x_LCD_TARGET_LCD_COLOR_MASK        0x000C
#define MV301x_LCD_FIFO_CONTROL_MASK            0x0002
#define MV301x_LCD_TARGET_LCD                   0x0001

// Power 
#define MV301x_POWER_OSC_EN                     0x0001
#define MV301x_POWER_PCLK_SEL                   0x0002
#define MV301x_POWER_PLL_EN                     0x0004
#define MV301x_POWER_SENSOR_CLK_OFF             0x0008
#define MV301x_POWER_HOST_CLK_OFF               0x0010
#define MV301x_POWER_SCALER_CLK_OFF             0x0020
#define MV301x_POWER_TDS_CLK_OFF                0x0040
#define MV301x_POWER_JPEG_CLK_OFF               0x0080

#define MV301x_POWER_CLK_SRC_INT                0x0400
#define MV301x_POWER_SCLK_SEL_PLL               0x0800
// Lib 1.23 : Insert by Kevin 20050214
#define MV301x_POWER_PCLK_INVERT                0x1000

#define MV301x_POWER_LCD_OUTPUT_OFF             0x4000
#define MV301x_POWER_LCD_CS2_OFF                0x8000

#define MV301x_POWER_DETAIL_ALL_OFF (0x00FF&(MV301x_POWER_HOST_CLK_OFF|MV301x_POWER_SCALER_CLK_OFF|MV301x_POWER_TDS_CLK_OFF|MV301x_POWER_JPEG_CLK_OFF|MV301x_POWER_SENSOR_CLK_OFF))

// MVC
#define MV301x_MEMORY_POINTER  1

// LCD
#define RESET_LCD_RS					0
#define SET_LCD_RS						1

#define MV301x_POWER_DOWN             0
#define MV301x_POWER_UP               1

typedef enum {
    MV301x_REGISTER_WRITE_NULL,
    MV301x_REGISTER_ACCESS_PAR0,
    MV301x_REGISTER_ACCESS_PAR1,
    MV301x_REGISTER_ACCESS_BOTH
} MV301x_REGISTER_ACCESS_TYPE;

typedef enum {
    MV301x_MCLK_DIV1,
    MV301x_MCLK_DIV2,
    MV301x_MCLK_DIV4,
    MV301x_MCLK_DIV8
} MV301x_MCLK_DIVISION_TYPE;

typedef enum {
    MV301x_SMCLK_OFF,
    MV301x_SMCLK_DIV1,
    MV301x_SMCLK_DIV2,
    MV301x_SMCLK_DIV4
} MV301x_SMCLK_DIVISION_TYPE;

typedef enum {
    MV301x_MCLK_SELECT,
    MV301x_PCLK_SELECT
} MV301x_CLK_SELECT_TYPE;

typedef enum {
    MV301x_SEN_PCLK,
    MV301x_SEN_MCLK
} MV301x_SEN_CLK_SELECT_TYPE;


extern void 	mv0CmdWrite(MV301x_REGISTER_TYPE register_Addr,
                        	mvUint16 par1,
                        	mvUint16 par0,
                        	MV301x_REGISTER_ACCESS_TYPE type);
extern 	mvUint32 mv0RegisterRead(MV301x_REGISTER_TYPE register_Addr,
                               MV301x_REGISTER_ACCESS_TYPE type);
extern void		mv3SwReset(void);
extern void		mv301xReset(mvUint8 resetParm);  

extern mvInt32	mv0WaitResult(mvUint32 period1ms);
extern void		mv0CmdInitialWrite(void);
extern mvStatus   mv0Power(MV3_POWER_STATE mv3Pwr);
extern mvStatus   mv0SensorPower(mvBoolean sensorPwrOn);
extern mvUint32   mv0GetJpegSize(void);
extern void     mv0SetSepiaUV(void);
extern void     mv0SetUVArea(void);
extern void     mv0SetEffect(void);
extern mvStatus   mv0PreviewStart(void);
extern mvStatus   mv0PreviewStop(void);
extern mvStatus   mv0PreviewPause(mvBoolean pause);
extern mvStatus   mv0IsPreview(void);

extern mvStatus   mv0MpegViewStart(void);
extern mvStatus   mv0MpegViewStop(void);
extern mvStatus   mv0IsMpegView(void);

extern mvStatus   mv0MJpegEncStart(void);
extern mvStatus   mv0MJpegEncStop(void);
extern mvStatus   mv0IsMJpegEnc(void);

extern mvStatus   mv0MJpegDecStart(void);
extern mvStatus   mv0MJpegDecStop(void);
extern mvStatus   mv0IsMJpegDec(void);

extern mvStatus   mv0ScrollParmGet(SCALE_STRUCT *sFactor,
                                     mvUint16 srcWidth,
                                     mvUint16 srcHeight);
extern mvStatus   mv0ImgCtrlParmGet(SCALE_STRUCT *sFactor,
                                      mvUint32 srcWidth,
                                      mvUint32 srcHeight,
                                      mvBoolean srcFreeze);
extern mvStatus   mv0Capture(void);
extern mvStatus   mv0MakeJpeg(void);
extern mvStatus   mv0Snapshot(mvUint16 *snap);
extern mvStatus   mv0Decode(void);
extern mvStatus   mv0DisplayJpeg(void);
extern void  mv0WriteOsdImg(void);
extern void  mv0WriteSuperimposeImg(MV301x_SCENARIO eScenario);


///////////////////////////////
// These are for only MV301x //
///////////////////////////////
void mv0_SensorSet(MV301x_SCENARIO scenario);
void mv0_PScalerSet(MV301x_SCENARIO scenario);
void mv0_SScalerSet(MV301x_SCENARIO scenario);
void mv0_JpegSet(MV301x_SCENARIO scenario);
void mv0_TDScalerSet(MV301x_SCENARIO scenario);
void mv0_LCDSet(MV301x_SCENARIO scenario);
void mv0_MVCSet(MV301x_SCENARIO scenario);

extern void mv3MemoryWrite1word(mvUint32 addr, mvUint16 data);
extern mvUint16 mv3MemoryRead1word(mvUint32 addr);



#ifdef FEATURE_MV3019
#define GET_MEM_ADDR  mv0RegisterRead(MV301x_REG_CURR_MEM_ADDR, MV301x_REGISTER_ACCESS_PAR0)
#define GET_HREF      mv0RegisterRead(MV301x_REG_CURR_SENSOR_VAL, MV301x_REGISTER_ACCESS_PAR0)
#define GET_FRAME     mv0RegisterRead(MV301x_REG_CURR_SENSOR_VAL, MV301x_REGISTER_ACCESS_PAR1)
#elif defined(FEATURE_MV3018)
#define GET_MEM_ADDR  mv0RegisterRead(MV301x_REG_CURR_VAL, MV301x_REGISTER_ACCESS_PAR1)
#define GET_HREF      mv0RegisterRead(MV301x_REG_CURR_VAL, MV301x_REGISTER_ACCESS_PAR0)
#endif

#define START_SETTING while(GET_HREF>100)
#define WAITVSYNC     while(GET_HREF<470)

