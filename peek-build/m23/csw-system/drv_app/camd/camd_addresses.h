/**
* @file  camd_commands.c
*
*
*
*
* @author  Remco Hiemstra
* @version 0.1
*/
/* History:
*
* Date              Modification
*-----------------------------------
*
*
* (c) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
*/


#define BLOCK_SWITCH 0x7f

#define switch_block(blocknr) (UINT16)((BLOCK_SWITCH << 9)|blocknr)

#define BLOCKNR(address) ((address&0x3FFF) >> 7)
#define OFFSET(address) ((address&0x7F) << 1)


#define CAMD_LOW_BYTE           1
#define CAMD_HIGH_BYTE          0


#define CAMD_ID_REG             0x0000
#define CAMD_CONTROL_REG        0x0002
#define CAMD_STATUS_REG         0x0004
#define CAMD_CLK_PER_REG        0x0006

#define CAMD_SIZE_REG           0x0008

 #define CAMD_HW_QQCIF          0x00
 #define CAMD_HW_QCIF	          0x01
 #define CAMD_HW_CIF	          0x02
 #define CAMD_HW_QQQVGA         0x03
 #define CAMD_HW_QQVGA	        0x04
 #define CAMD_HW_QVGA	          0x05
 #define CAMD_HW_VGA	          0x06

#define CAMD_OUTPUT_FORMAT_REG  0x000a

 #define CAMD_RGB888                                       0x00
 #define CAMD_RGB666_4_PIX_IN_9_BYTES                      0x01
 #define CAMD_RGB666_1_PIX_IN_3_BYTES                      0x02
 #define CAMD_RGB565_1_PIX_IN_2_BYTES                      0x03
 #define CAMD_RGB444_2_PIX_IN_3_BYTES_RG_BR_GB             0x04
 #define CAMD_RGB444_1_PIX_IN_2_BYTES_RG_B0_OR_0R_GB       0x05
 #define CAMD_RGB444_1_PIX_IN_3_BYTES_R0_G0_B0_OR_0R_0G_0B 0x06
 #define CAMD_RGB332_1_PIX_IN_1_BYTE                       0x07
 #define CAMD_YCBCR_YUV_4_2_2_Y1_U12_Y2_V12_ORDER          0x08
 #define CAMD_YCBCR_YUV_4_2_2_U12_Y1_V12_Y2_ORDER          0x09
 #define CAMD_YCBCR_YUV_4_2_2_Y1_V12_Y2_U12_ORDER          0x0A
 #define CAMD_YCBCR_YUV_4_2_2_V12_Y1_U12_Y2_ORDER          0x0B
 #define CAMD_YCBCR_YUV_4_4_4                              0x0C
 #define CAMD_GRAYSCALE_Y_4_0_0                            0x0D
 #define CAMD_RAWSENSOR_DATA                               0x0E
 #define CAMD_RAWSENSOR_DATA_NO_BPA_CORR                   0x0F


#define CAMD_EXPOSURE_REG       0x000c
#define CAMD_EXP_ADJ_REG        0x000e
#define CAMD_ILLUM_REG          0x0010
#define CAMD_FRAME_RATE_REG     0x0012
#define CAMD_A_FRAME_RATE_REG   0x0016
#define CAMD_SENSOR_WID_V_REG   0x0018
#define CAMD_SENSOR_HGT_V_REG   0x001a
#define CAMD_OUTPUT_WID_V_REG   0x001c
#define CAMD_OUTPUT_HGT_V_REG   0x001e
#define CAMD_SENSOR_WID_S_REG   0x0020
#define CAMD_SENSOR_HGT_S_REG   0x0022
#define CAMD_OUTPUT_WID_S_REG   0x0024
#define CAMD_OUTPUT_HGT_S_REG   0x0026

#define CAMD_FWROW_REG          0x080a
#define CAMD_FWCOL_REG          0x080b
#define CAMD_LWROW_REG          0x080c
#define CAMD_LWCOL_REG          0x080d

#define CAMD_PARALLEL_CTRL_REG  0x100A


#define VIDEO_All_TONEMAP       0x01c0
#define STILL_All_TONEMAP       0x0202


