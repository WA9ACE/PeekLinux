#define AGILENT 0
#define MICRON 1

#if (CAM_SENSOR==AGILENT)
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
 *  27-Jun-2005								Venugopal Naik																		
 *
 * (c) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */


#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "rv/rv_general.h"
#include "camd/camd_i.h"
#include "camd/camd_commands.h"
#include "camd/camd_addresses.h"



#include "i2c/i2c_api.h"
#include "clkm.h"
#include "i2c/bspI2cMultiMaster.h"
#include "i2c/bspI2c.h"
#define CAMD_AGILENT_ADCM2700 
#define CAMERA_ADDRESS 		0x053







#define CAMERA_CLK_REG(CC_XXX_REG) (*((volatile UINT32 *)(CC_XXX_REG)))
#define CAMD_SENSOR_CLOCK_REG (*((volatile UINT16 *) (0xFFFFFD90)))
#define DPLL_CLK_INIT (0x0005)
#define DPLL_CLK_ON (0x0001)

/*Forward declarations*/


void camd_init_camera(void);
void camd_get_frame(void);
void camd_change_fps(UINT8);
static T_RV_RET write_commands_to_camera (UINT8 * bytes_p, UINT8 * commands_p,
        UINT16 size_of_bytes, BOOL vf_mode);



static T_RV_RET write_command_to_camera_address (UINT16 address,
        UINT8 * data,
        UINT16 nr_of_words);


UINT8 read_status_bit();
void check_sensor(void);



T_CAMD_ENCODING camd_encoding;
UINT8 commandsRd[]={
   0x00,0x00,0x00
};


#undef READ_COMMANDS
#undef TESTING
#ifdef READ_COMMANDS
UINT8 read_commands_from_camera (void);
#endif

void camd_i2c_callback(BspI2c_TransactionId);
UINT8 I2C_complete = 0;


#ifdef CAMD_AGILENT_ADCM2700 
const T_CAMD_CAMERA_CAPABILITIES camd_camera_capabilities = {
   2,
   {
   {
           CAMD_QCIF,
           CAMD_RGB_565,
           176,
           144,
          // 5,							/* number of bits in R component */
          // 6,							/* number of bits in G component */
          // 5,							/* number of bits in B component */
           //0,							/* number of bits in Y component */
          // 0,							/* number of bits in U component */
          // 0,							/* number of bits in V component */
           0							/* 0 = no zoom supported */
       },
   {
           CAMD_VGA,
           CAMD_YUYV_INTERLEAVED,
           640,
           480,
         //  0,							/* number of bits in R component */
         //  0,							/* number of bits in G component */
         //  0,							/* number of bits in B component */
         //  8,							/* number of bits in Y component */
         //  4,							/* number of bits in U component */
        //   4,							/* number of bits in V component */
           0							/* 0 = no zoom supported */
       }
      
   }
   
};


static UINT8 camd_init_sensor_bytes[] =
{ 2, 3, 2, 5, 2, 5, 
    5,5,
    5, 5, 5, 2, 
    5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 
    2, 
    5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 
    2, 
    5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 
    2, 
    5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 
    2, 
    5,5,5,5, 5,5,5,5, 5,5,5,5,5,5,5,5,  5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 
    2, 
    5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 
    2, 
    5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 
    2, 
    5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 
    2, 
    5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 
    2, 
    5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 5,5,5,5, 

    2, 
    3, 3, 
    2, 
    3, 
    2, 
    3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 
    3, 3, 3, 3, 3, 3, 3, 3, 
    3, 3, 3, 3, 3, 3, 3, 3, 
    3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 
    3, 3, 3, 3, 3, 3, 3, 3, 
    2, 
    3, 3, 3, 3, 3, 3, 3, 3, 
    3, 3, 3, 3, 3, 3, 3, 3, 
    3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 
    2, 
    3, 3, 3, 3, 3, 3,
    0
};


static UINT8 camd_init_sensor_commands[] = {

   0xfe,0x00,
   0x04,0x00,0x00,0x00,
   0xfe,0x03,
   0x0c,0x08,0x0b,0x50,0x00,0x00,

   0xfe,0x90,
   0x18,0x02,0x00,0x20,0x42,0x00,
   0x40,0x02,0x00,0xb4,0x42,0x00,
   0x48,0x02,0x00,0x98,0x42,0x00,

   0x58,0x02,0x00,0x53,0x42,0x00,
   0xa0,0x02,0x00,0x5b,0x42,0x00,
   0xb0,0x02,0x00,0x55,0x42,0x00,

   0xfe,0x91,
   0x00,0x00,0x00,0x90,0x01,0x00,
   0x08,0x00,0x00,0x90,0x01,0x00,
   0x10,0x00,0x00,0x90,0x01,0x00,
   0x18,0x09,0x00,0x05,0x00,0x00,

   0x20,0x00,0x00,0x01,0x00,0x00,
   0x28,0x0a,0x00,0x2c,0x42,0x00,
   0x30,0x00,0x00,0x91,0x08,0x00,
   0x38,0x00,0x00,0x89,0x4a,0x00,


   0x40,0x00,0x00,0x91,0x08,0x00,
   0x48,0x00,0x00,0x99,0x4a,0x00,
   0x50,0x00,0x00,0xd1,0x08,0x00,
   0x58,0x02,0x00,0xf3,0x03,0x00,

   0x60,0x00,0x00,0x04,0x46,0x00,
   0x68,0x00,0x00,0x89,0x4a,0x00,
   0x70,0x00,0x00,0xad,0x04,0x00,
   0x78,0x00,0x00,0xac,0x04,0x00,

   0x80,0x03,0x00,0x4c,0x42,0x00,
   0x88,0x00,0x00,0xae,0x04,0x00,
   0x90,0x00,0x00,0x08,0x08,0x00,
   0x98,0x00,0x00,0x9e,0x05,0x00,

   0xa0,0x05,0x00,0x36,0x42,0x00,
   0xa8,0x00,0x00,0x2e,0x05,0x00,
   0xb0,0x00,0x00,0x89,0x6a,0x00,
   0xb8,0x00,0x00,0x04,0x46,0x00,

   0xc0,0x00,0x00,0x39,0x04,0x00,
   0xc8,0x00,0x00,0x99,0x4a,0x00,
   0xd0,0x00,0x00,0xad,0x04,0x00,
   0xd8,0x00,0x00,0xac,0x04,0x00,

   0xe0,0x03,0x00,0x4c,0x42,0x00,
   0xe8,0x00,0x00,0xae,0x04,0x00,
   0xf0,0x00,0x00,0x08,0x08,0x00,
   0xf8,0x00,0x00,0x9e,0x05,0x00,

   0xfe,0x92,
   0x00,0x05,0x00,0x42,0x42,0x00,
   0x08,0x00,0x00,0x2e,0x05,0x00,
   0x10,0x00,0x00,0x99,0x6a,0x00,
   0x18,0x03,0x00,0xc7,0x42,0x00,

   0x20,0x00,0x00,0x01,0x8a,0x00,
   0x28,0x04,0x00,0x4b,0x42,0x00,
   0x30,0x03,0x00,0xd8,0x03,0x00,
   0x38,0x09,0x00,0x40,0x01,0x00,

   0x40,0x00,0x00,0x37,0x65,0x00,
   0x48,0x09,0x00,0xf0,0x00,0x00,
   0x50,0x00,0x00,0x38,0x65,0x00,
   0x58,0x00,0x00,0xff,0x01,0x00,

   0x60,0x00,0x00,0x95,0x05,0x00,
   0x68,0x05,0x00,0x4f,0x42,0x00,
   0x70,0x00,0x00,0x2d,0x05,0x00,
   0x78,0x00,0x00,0x94,0x05,0x00,

   0x80,0x05,0x00,0x52,0x42,0x00,
   0x88,0x00,0x00,0x2c,0x05,0x00,
   0x90,0x00,0x00,0xff,0x01,0x00,
   0x98,0x03,0x00,0x2a,0x43,0x00,

   0xa0,0x02,0x00,0x82,0x01,0x00,
   0xa8,0x0e,0x00,0x0a,0x00,0x00,
   0xb0,0x00,0x00,0x77,0x00,0x00,
   0xb8,0x00,0x00,0x05,0x04,0x00,

   0xc0,0x00,0x00,0x6f,0x00,0x00,
   0xc8,0x0d,0x00,0x5a,0x00,0x00,
   0xd0,0x02,0x00,0x76,0x02,0x00,
   0xd8,0x03,0x00,0x9b,0x09,0x00,

   0xe0,0x0d,0x00,0x29,0x00,0x00,
   0xe8,0x00,0x00,0x6e,0x02,0x00,
   0xf0,0x00,0x00,0xa6,0x01,0x00,
   0xf8,0x00,0x00,0x34,0x09,0x00,

   0xfe,0x93,
   0x00,0x00,0x00,0x8a,0x56,0x00,
   0x08,0x00,0x00,0x02,0x86,0x00,
   0x10,0x05,0x00,0x64,0x42,0x00,
   0x18,0x00,0x00,0x9a,0x56,0x00,

   0x20,0x00,0x00,0xae,0x02,0x00,
   0x28,0x00,0x00,0x7e,0x02,0x00,
   0x30,0x00,0x00,0xa6,0x01,0x00,
   0x38,0x00,0x00,0x36,0x04,0x00,

   0x40,0x00,0x00,0x33,0x09,0x00,
   0x48,0x00,0x00,0x13,0x78,0x00,
   0x50,0x00,0x00,0x3e,0x04,0x00,
   0x58,0x00,0x00,0x14,0x78,0x00,

   0x60,0x00,0x00,0xa1,0x8a,0x00,
   0x68,0x04,0x00,0xf2,0x09,0x00,
   0x70,0x00,0x00,0xa5,0x02,0x00,
   0x78,0x00,0x00,0x13,0x04,0x00,

   0x80,0x00,0x00,0x5d,0x02,0x00,
   0x88,0x00,0x00,0xa5,0x01,0x00,
   0x90,0x00,0x00,0xef,0x00,0x00,
   0x98,0x00,0x00,0x17,0x04,0x00,

   0xa0,0x00,0x00,0x67,0x02,0x00,
   0xa8,0x00,0x00,0xa7,0x01,0x00,
   0xb0,0x00,0x00,0x84,0x56,0x00,
   0xb8,0x00,0x00,0x02,0x86,0x00,

   0xc0,0x05,0x00,0x7a,0x42,0x00,
   0xc8,0x00,0x00,0x94,0x56,0x00,
   0xd0,0x00,0x00,0xff,0x34,0x00,
   0xd8,0x0e,0x00,0x5a,0x00,0x00,

   0xe0,0x00,0x00,0xae,0x02,0x00,
   0xe8,0x00,0x00,0x3e,0x04,0x00,
   0xf0,0x00,0x00,0xfb,0x01,0x00,
   0xf8,0x05,0x00,0x81,0x42,0x00,

   0xfe,0x94,
   0x00,0x00,0x00,0x86,0x01,0x00,
   0x08,0x00,0x00,0x1e,0x04,0x00,
   0x10,0x00,0x00,0x6e,0x02,0x00,
   0x18,0x00,0x00,0xa6,0x01,0x00,

   0x20,0x00,0x00,0x8a,0x52,0x00,
   0x28,0x00,0x00,0x02,0x86,0x00,
   0x30,0x05,0x00,0x88,0x42,0x00,
   0x38,0x00,0x00,0x9a,0x52,0x00,

   0x40,0x00,0x00,0xf4,0x00,0x00,
   0x48,0x0e,0x00,0x26,0x00,0x00,
   0x50,0x00,0x00,0xf4,0x00,0x00,
   0x58,0x00,0x00,0x67,0x01,0x00,

   0x60,0x07,0x00,0x8e,0x42,0x00,
   0x68,0x00,0x00,0x27,0x09,0x00,
   0x70,0x00,0x00,0x26,0x09,0x00,
   0x78,0x00,0x00,0xfe,0x00,0x00,

   0x80,0x00,0x00,0x0d,0x04,0x00,
   0x88,0x00,0x00,0xae,0x02,0x00,
   0x90,0x0f,0x00,0x80,0x00,0x00,
   0x98,0x00,0x00,0x7e,0x00,0x00,

   0xa0,0x00,0x00,0x3e,0x04,0x00,
   0xa8,0x00,0x00,0x86,0x01,0x00,
   0xb0,0x0f,0x00,0x49,0x00,0x00,
   0xb8,0x02,0x00,0xeb,0x09,0x00,

   0xc0,0x0e,0x00,0x34,0x00,0x00,
   0xc8,0x00,0x00,0xd8,0x78,0x00,
   0xd0,0x0e,0x00,0x20,0x00,0x00,
   0xd8,0x00,0x00,0xd9,0x78,0x00,

   0xe0,0x0e,0x00,0x34,0x00,0x00,
   0xe8,0x00,0x00,0xdc,0x78,0x00,
   0xf0,0x0e,0x00,0x13,0x00,0x00,
   0xf8,0x00,0x00,0xdd,0x78,0x00,

   0xfe,0x95,
   0x00,0x0e,0x00,0x34,0x00,0x00,
   0x08,0x00,0x00,0xec,0x78,0x00,
   0x10,0x0e,0x00,0x16,0x00,0x00,
   0x18,0x00,0x00,0xed,0x78,0x00,

   0x20,0x0e,0x00,0x34,0x00,0x00,
   0x28,0x00,0x00,0xf0,0x78,0x00,
   0x30,0x0e,0x00,0x09,0x00,0x00,
   0x38,0x00,0x00,0xf1,0x78,0x00,

   0x40,0x0e,0x00,0x06,0x00,0x00,
   0x48,0x00,0x00,0xf6,0x78,0x00,
   0x50,0x0e,0x00,0x20,0x00,0x00,
   0x58,0x00,0x00,0xf7,0x78,0x00,

   0x60,0x0e,0x00,0x06,0x00,0x00,
   0x68,0x00,0x00,0xfa,0x78,0x00,
   0x70,0x0e,0x00,0x13,0x00,0x00,
   0x78,0x00,0x00,0xfb,0x78,0x00,

   0x80,0x0e,0x00,0x2c,0x00,0x00,
   0x88,0x00,0x00,0x21,0x78,0x00,
   0x90,0x03,0x00,0x00,0x43,0x00,
   0x98,0x02,0x00,0x1f,0x01,0x00,

   0xa0,0x00,0x00,0x48,0x06,0x00,
   0xa8,0x05,0x00,0xb8,0x42,0x00,
   0xb0,0x00,0x00,0xa1,0x9a,0x00,
   0xb8,0x05,0x00,0x36,0x0a,0x00,

   0xc0,0x00,0x00,0x28,0x06,0x00,
   0xc8,0x02,0x00,0xf9,0x09,0x00,
   0xd0,0x00,0x00,0x00,0x01,0x00,
   0xd8,0x00,0x00,0x30,0x01,0x00,

   0xe0,0x00,0x00,0x6a,0x01,0x00,
   0xe8,0x00,0x00,0xaf,0x01,0x00,
   0xf0,0x00,0x00,0x00,0x02,0x00,
   0xf8,0x00,0x00,0x61,0x02,0x00,

   0xfe,0x96,
   0x00,0x00,0x00,0xd4,0x02,0x00,
   0x08,0x00,0x00,0x5d,0x03,0x00,
   0x10,0x00,0x00,0x00,0x04,0x00,
   0x18,0x00,0x00,0xc2,0x04,0x00,

   0x20,0x00,0x00,0xa8,0x05,0x00,
   0x28,0x00,0x00,0xba,0x06,0x00,
   0x30,0x00,0x00,0x00,0x08,0x00,
   0x38,0x00,0x00,0x58,0x09,0x00,

   0x40,0x00,0x00,0x60,0x09,0x00,
   0x48,0x00,0x00,0x07,0x52,0x00,
   0x50,0x00,0x00,0x0f,0x30,0x00,
   0x58,0x0a,0x00,0x0c,0x00,0x00,

   0x60,0x00,0x00,0x54,0x01,0x00,
   0x68,0x06,0x00,0xcf,0x42,0x00,
   0x70,0x02,0x00,0xd0,0x42,0x00,
   0x78,0x0c,0x00,0x0c,0x00,0x00,

   0x80,0x0b,0x00,0xba,0x42,0x00,
   0x88,0x00,0x00,0x63,0x00,0x00,
   0x90,0x00,0x00,0x63,0x08,0x00,
   0x98,0x00,0x00,0x07,0xbe,0x00,

   0xa0,0x04,0x00,0xe5,0x42,0x00,
   0xa8,0x00,0x00,0x07,0xa2,0x00,
   0xb0,0x05,0x00,0xdd,0x42,0x00,
   0xb8,0x09,0x00,0x40,0x00,0x00,

   0xc0,0x00,0x00,0xbc,0x67,0x00,
   0xc8,0x09,0x00,0x00,0x01,0x00,
   0xd0,0x00,0x00,0xbd,0x67,0x00,
   0xd8,0x00,0x00,0xbe,0x73,0x00,

   0xe0,0x02,0x00,0xe2,0x42,0x00,
   0xe8,0x09,0x00,0x40,0x00,0x00,
   0xf0,0x00,0x00,0xbc,0x67,0x00,
   0xf8,0x00,0x00,0xbd,0x73,0x00,

   0xfe,0x97,
   0x00,0x09,0x00,0x00,0x01,0x00,
   0x08,0x00,0x00,0xbe,0x67,0x00,
   0x10,0x03,0x00,0xe4,0x07,0x00,
   0x18,0x00,0x00,0xaf,0x66,0x00,

   0x20,0x02,0x00,0xfd,0x42,0x00,
   0x28,0x00,0x00,0x60,0x09,0x00,
   0x30,0x00,0x00,0x84,0x52,0x00,
   0x38,0x00,0x00,0xff,0x30,0x00,

   0x40,0x00,0x00,0x85,0x56,0x00,
   0x48,0x00,0x00,0x67,0x5e,0x00,
   0x50,0x03,0x00,0x6f,0x02,0x00,
   0x58,0x00,0x00,0x8a,0x7e,0x00,

   0x60,0x00,0x00,0x20,0x08,0x00,
   0x68,0x00,0x00,0x07,0xa2,0x00,
   0x70,0x05,0x00,0xf5,0x42,0x00,
   0x78,0x00,0x00,0x06,0x46,0x00,

   0x80,0x00,0x00,0xbc,0x67,0x00,
   0x88,0x09,0x00,0x00,0x01,0x00,
   0x90,0x00,0x00,0xbd,0x67,0x00,
   0x98,0x00,0x00,0xbe,0x73,0x00,

   0xa0,0x02,0x00,0xfa,0x42,0x00,
   0xa8,0x00,0x00,0x06,0x46,0x00,
   0xb0,0x00,0x00,0xbc,0x67,0x00,
   0xb8,0x00,0x00,0xbd,0x73,0x00,

   0xc0,0x09,0x00,0x00,0x01,0x00,
   0xc8,0x00,0x00,0xbe,0x67,0x00,
   0xd0,0x03,0x00,0xe4,0x07,0x00,
   0xd8,0x00,0x00,0x0f,0x09,0x00,

   0xe0,0x03,0x00,0xae,0x09,0x00,
   0xe8,0x00,0x00,0x20,0x08,0x00,
   0xf0,0x00,0x00,0x18,0x08,0x00,
   0xf8,0x00,0x00,0xff,0x01,0x00,

   0xfe,0x98,
   0x00,0x00,0x00,0x58,0x09,0x00,
   0x08,0x00,0x00,0x60,0x09,0x00,
   0x10,0x00,0x00,0x68,0x09,0x00,
   0x18,0x00,0x00,0x70,0x09,0x00,

   0x20,0x0b,0x00,0x80,0x02,0x00,
   0x28,0x00,0x00,0x80,0x4a,0x00,
   0x30,0x00,0x00,0xd3,0x00,0x00,
   0x38,0x00,0x00,0x23,0x04,0x00,

   0x40,0x00,0x00,0x1d,0x09,0x00,
   0x48,0x0b,0x00,0xe0,0x01,0x00,
   0x50,0x00,0x00,0x81,0x4a,0x00,
   0x58,0x00,0x00,0xd3,0x00,0x00,

   0x60,0x00,0x00,0x23,0x04,0x00,
   0x68,0x00,0x00,0x1e,0x09,0x00,
   0x70,0x00,0x00,0x2b,0x09,0x00,
   0x78,0x00,0x00,0x0b,0x04,0x00,

   0x80,0x00,0x00,0x13,0x04,0x00,
   0x88,0x0a,0x00,0x00,0xff,0x00,
   0x90,0x00,0x00,0x13,0x03,0x00,
   0x98,0x00,0x00,0x34,0x09,0x00,

   0xa0,0x00,0x00,0x2c,0x04,0x00,
   0xa8,0x00,0x00,0x63,0x03,0x00,
   0xb0,0x0a,0x00,0x01,0x01,0x00,
   0xb8,0x00,0x00,0x53,0x00,0x00,

   0xc0,0x00,0x00,0x35,0x6d,0x00,
   0xc8,0x0b,0x00,0x88,0x02,0x00,
   0xd0,0x00,0x00,0xeb,0x00,0x00,
   0xd8,0x00,0x00,0x2b,0x04,0x00,

   0xe0,0x00,0x00,0x1d,0x09,0x00,
   0xe8,0x0b,0x00,0xe8,0x01,0x00,
   0xf0,0x00,0x00,0xf3,0x00,0x00,
   0xf8,0x00,0x00,0x2b,0x04,0x00,

   0xfe,0x99,
   0x00,0x00,0x00,0x1e,0x09,0x00,
   0x08,0x00,0x00,0x2b,0x09,0x00,
   0x10,0x00,0x00,0x1b,0x04,0x00,
   0x18,0x00,0x00,0x73,0x03,0x00,

   0x20,0x00,0x00,0x36,0x6d,0x00,
   0x28,0x00,0x00,0x30,0x08,0x00,
   0x30,0x00,0x00,0x28,0x08,0x00,
   0x38,0x00,0x00,0x20,0x08,0x00,

   0x40,0x00,0x00,0x18,0x08,0x00,
   0x48,0x00,0x00,0xff,0x01,0x00,
   0x50,0x00,0x00,0x58,0x09,0x00,
   0x58,0x00,0x00,0x60,0x09,0x00,

   0x60,0x00,0x00,0x68,0x09,0x00,
   0x68,0x00,0x00,0x70,0x09,0x00,
   0x70,0x0b,0x00,0x80,0x02,0x00,
   0x78,0x00,0x00,0x90,0x4a,0x00,

   0x80,0x00,0x00,0xd3,0x00,0x00,
   0x88,0x00,0x00,0x23,0x04,0x00,
   0x90,0x00,0x00,0x1d,0x09,0x00,
   0x98,0x0b,0x00,0xe0,0x01,0x00,

   0xa0,0x00,0x00,0x91,0x4a,0x00,
   0xa8,0x00,0x00,0xd3,0x00,0x00,
   0xb0,0x00,0x00,0x23,0x04,0x00,
   0xb8,0x00,0x00,0x1e,0x09,0x00,

   0xc0,0x00,0x00,0x2b,0x09,0x00,
   0xc8,0x00,0x00,0x0b,0x04,0x00,
   0xd0,0x00,0x00,0x13,0x04,0x00,
   0xd8,0x0a,0x00,0x00,0xff,0x00,

   0xe0,0x00,0x00,0x13,0x03,0x00,
   0xe8,0x00,0x00,0x34,0x09,0x00,
   0xf0,0x00,0x00,0x2c,0x04,0x00,
   0xf8,0x00,0x00,0x63,0x03,0x00,

   0xfe,0x9a,
   0x00,0x0a,0x00,0x01,0x01,0x00,
   0x08,0x00,0x00,0x53,0x00,0x00,
   0x10,0x00,0x00,0x35,0x6d,0x00,
   0x18,0x0b,0x00,0x88,0x02,0x00,

   0x20,0x00,0x00,0xeb,0x00,0x00,
   0x28,0x00,0x00,0x2b,0x04,0x00,
   0x30,0x00,0x00,0x1d,0x09,0x00,
   0x38,0x0b,0x00,0xe8,0x01,0x00,

   0x40,0x00,0x00,0xf3,0x00,0x00,
   0x48,0x00,0x00,0x2b,0x04,0x00,
   0x50,0x00,0x00,0x1e,0x09,0x00,
   0x58,0x00,0x00,0x2b,0x09,0x00,

   0x60,0x00,0x00,0x1b,0x04,0x00,
   0x68,0x00,0x00,0x73,0x03,0x00,
   0x70,0x00,0x00,0x36,0x6d,0x00,
   0x78,0x00,0x00,0x30,0x08,0x00,

   0x80,0x00,0x00,0x28,0x08,0x00,
   0x88,0x00,0x00,0x20,0x08,0x00,
   0x90,0x00,0x00,0x18,0x08,0x00,
   0x98,0x00,0x00,0xff,0x01,0x00,

   0xfe,0x20,
   0xdc,0x46,0x01,0x00,
   0xe0,0xfc,0x00,0x00,
   0xfe,0x21,
   0x28,0x0e,0x01,0x00,
   0xfe,0x30,
   0x00,0x43,0x00,0x00,
   0x04,0x47,0x00,0x00,
   0x08,0x4b,0x00,0x00,
   0x0c,0x50,0x00,0x00,
   0x10,0x54,0x00,0x00,
   0x14,0x59,0x00,0x00,
   0x18,0x5e,0x00,0x00,
   0x1c,0x63,0x00,0x00,
   0x20,0x69,0x00,0x00,
   0x24,0x6f,0x00,0x00,
   0x28,0x75,0x00,0x00,
   0x2c,0x7c,0x00,0x00,
   0x30,0x83,0x00,0x00,
   0x34,0x8b,0x00,0x00,
   0x38,0x93,0x00,0x00,
   0x3c,0x9b,0x00,0x00,
   0x40,0xa4,0x00,0x00,
   0x44,0xac,0x00,0x00,
   0x48,0xb6,0x00,0x00,
   0x4c,0xbf,0x00,0x00,
   0x50,0xc8,0x00,0x00,
   0x54,0xd2,0x00,0x00,
   0x58,0xdb,0x00,0x00,
   0x5c,0xe3,0x00,0x00,
   0x60,0xeb,0x00,0x00,
   0x64,0xf2,0x00,0x00,
   0x68,0xf9,0x00,0x00,
   0x6c,0xfe,0x00,0x00,
   0x70,0x01,0x01,0x00,
   0x74,0x03,0x01,0x00,
   0x78,0x04,0x01,0x00,
   0x7c,0x03,0x01,0x00,
   0x80,0x44,0x00,0x00,
   0x84,0x48,0x00,0x00,
   0x88,0x4b,0x00,0x00,
   0x8c,0x4f,0x00,0x00,
   0x90,0x52,0x00,0x00,
   0x94,0x56,0x00,0x00,
   0x98,0x5a,0x00,0x00,
   0x9c,0x5e,0x00,0x00,
   0xa0,0x63,0x00,0x00,
   0xa4,0x67,0x00,0x00,
   0xa8,0x6c,0x00,0x00,
   0xac,0x70,0x00,0x00,
   0xb0,0x75,0x00,0x00,
   0xb4,0x7b,0x00,0x00,
   0xb8,0x80,0x00,0x00,
   0xbc,0x86,0x00,0x00,
   0xc0,0x8c,0x00,0x00,
   0xc4,0x92,0x00,0x00,
   0xc8,0x98,0x00,0x00,
   0xcc,0x9f,0x00,0x00,
   0xd0,0xa6,0x00,0x00,
   0xd4,0xae,0x00,0x00,
   0xd8,0xb5,0x00,0x00,
   0xdc,0xbd,0x00,0x00,
   0xe0,0xc6,0x00,0x00,
   0xe4,0xcf,0x00,0x00,
   0xe8,0xd8,0x00,0x00,
   0xec,0xe2,0x00,0x00,
   0xf0,0xed,0x00,0x00,
   0xf4,0xf8,0x00,0x00,
   0xf8,0x03,0x01,0x00,
   0xfc,0x0f,0x01,0x00,
   0xfe,0x31,
   0x00,0x47,0x00,0x00,
   0x04,0x49,0x00,0x00,
   0x08,0x4b,0x00,0x00,
   0x0c,0x4d,0x00,0x00,
   0x10,0x50,0x00,0x00,
   0x14,0x52,0x00,0x00,
   0x18,0x55,0x00,0x00,
   0x1c,0x58,0x00,0x00,
   0x20,0x5a,0x00,0x00,
   0x24,0x5d,0x00,0x00,
   0x28,0x60,0x00,0x00,
   0x2c,0x63,0x00,0x00,
   0x30,0x66,0x00,0x00,
   0x34,0x69,0x00,0x00,
   0x38,0x6d,0x00,0x00,
   0x3c,0x71,0x00,0x00,
   0x40,0x75,0x00,0x00,
   0x44,0x79,0x00,0x00,
   0x48,0x7d,0x00,0x00,
   0x4c,0x82,0x00,0x00,
   0x50,0x87,0x00,0x00,
   0x54,0x8d,0x00,0x00,
   0x58,0x93,0x00,0x00,
   0x5c,0x9a,0x00,0x00,
   0x60,0xa1,0x00,0x00,
   0x64,0xa9,0x00,0x00,
   0x68,0xb2,0x00,0x00,
   0x6c,0xbd,0x00,0x00,
   0x70,0xc8,0x00,0x00,
   0x74,0xd6,0x00,0x00,
   0x78,0xe5,0x00,0x00,
   0x7c,0xf7,0x00,0x00,
   0xfe,0x00,
   0x10,0x81,0x06,0x00,
   0x0c,0xc8,0x32,0x00,/* 13 Mhz  clock */
   0x14,0x03,0x09,0x00,
   0x24,0x90,0x01,0x00, /*FPS setting default value */
   0x20,0x08,0x00,0x00,
   0x04,0x04,0x00,0x00,
   0x00,0x00

};

static UINT8 camd_init_sensor_commands2[] = {
   0x04,0x00,0x00,0x00,
   0xfe,0x02,
#if 0

   /* Added */
   /*0x20,0x13,0x81,0x00,*/            /* # OUTPUT_CTRL_V no free running VCLK*/
   /*0x60,0x19,0x81,0x00,*/            /* # OUTPUT_CTRL_S no free running VCLK*/
   /*--------*/

#endif

   0x24,0x80,0xca,0x00,
   0x64,0x80,0xca,0x00,
   0xfe,0x21,
   0x18,0x80,0x00,0x00,
   0xfe,0x02,
   0xf8,0x0e,0x01,0x00,
   0xfc,0x12,0x01,0x00,
   0x94,0x10,0x00,0x00,
   0xfe,0x20,
   0x50,0x4e,0x02,0x00,
   0x54,0x6d,0x0f,0x00,
   0x58,0x44,0x0f,0x00,
   0x5c,0x4d,0x0f,0x00,
   0x60,0x49,0x02,0x00,
   0x64,0x69,0x0f,0x00,
   0x68,0xf3,0x0f,0x00,
   0x6c,0xbc,0x0e,0x00,
   0x70,0x50,0x02,0x00,
   /* Added on 28/10/05 */
   0x20,0x00,0x00,0x00,
   /*------------------*/
   0xfe,0x02,
   0x34,0x00,0x00,0x00,
   //0xfe,0x00,
   //0x04,0x01,0x00,0x00,
   0x00,0x00
};

static UINT8 camd_init_sensor_bytes2[] =
{ 
    3,
    2, 
#if 0
    3, 3,/* Added */
#endif
    3, 3, 
    2,
    3, 
    2, 
    3, 3, 3, 
    2, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* Added */ 3,
    2, 
    3, 
    /*2, 
      3, */
    0
};


static UINT8 gamma_correction_array[2][104] =
/* default gamma correction */
{ {
    0xfe,0x03,
    0x80,0x00,0x00,0x17,0x00,0x00,
    0x88,0x32,0x00,0x46,0x00,0x00,
    0x90,0x56,0x00,0x64,0x00,0x00,
    0x98,0x71,0x00,0x7c,0x00,0x00,
    0xa0,0x86,0x00,0x99,0x00,0x00,
    0xa8,0xa9,0x00,0xb8,0x00,0x00,
    0xb0,0xc6,0x00,0xdf,0x00,0x00,
    0xb8,0xf5,0x00,0x09,0x01,0x00,
    0xc0,0x1b,0x01,0x3d,0x01,0x00,
    0xc8,0x5a,0x01,0x75,0x01,0x00,
    0xd0,0x8d,0x01,0xba,0x01,0x00,
    0xd8,0xe1,0x01,0x05,0x02,0x00,
    0xe0,0x25,0x02,0x61,0x02,0x00,
    0xe8,0x95,0x02,0xc5,0x02,0x00,
    0xf0,0xf1,0x02,0x3f,0x03,0x00,
    0xf8,0x85,0x03,0xc5,0x03,0x00,

    0xfe, 0x04,
    0x00,0x00,0x04,0x00 },

/* gamma correction is 3.2 */
{
    0xfe,0x03,
    0x80,0x00,0x00,0x91,0x00,0x00,
    0x88,0xb5,0x00,0xcd,0x00,0x00,
    0x90,0xe0,0x00,0xf1,0x00,0x00,
    0x98,0xff,0x00,0x0b,0x01,0x00,
    0xa0,0x17,0x01,0x2b,0x01,0x00,
    0xa8,0x3c,0x01,0x4c,0x01,0x00,
    0xb0,0x5a,0x01,0x73,0x01,0x00,
    0xb8,0x89,0x01,0x9c,0x01,0x00,
    0xc0,0xae,0x01,0xcd,0x01,0x00,
    0xc8,0xe8,0x01,0x00,0x02,0x00,
    0xd0,0x16,0x02,0x3d,0x02,0x00,
    0xd8,0x5e,0x02,0x7c,0x02,0x00,
    0xe0,0x97,0x02,0xc7,0x02,0x00,
    0xe8,0xf1,0x02,0x16,0x03,0x00,
    0xf0,0x38,0x03,0x74,0x03,0x00,
    0xf8,0xa7,0x03,0xd6,0x03,0x00,

    0xfe, 0x04,
    0x00,0x00,0x04,0x00}
    };
static UINT8 camd_gamma_bytes[]={
   2,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,2,3,0
};

#endif


#ifdef TESTING
static UINT8 extra_commands[] =
{

    0xfe, 0x00,
    0x04,0x00, 0x00,0x00,

    0xfe, 0x02,
    0x80, 0x00,0x00,0x00,0x00,0x00,

    0x20,0x1f,0x80,0x00,

    0x00,0x80,0x00,0x10,0x00,0x00,
    0x08,0x80,0x00,0x10,0x00,0x00,

    0xFE,0x20,
    0xBC, 0x03,0x00,0x00,

    0xfe,0x00,
    0x04,0x01,0x00,0x00,

    0xFE, 0x20,	
    0x10, 0x1f, 0x80,0x00,	
    0x00

};
static UINT8 extra_commands_bytes[] =
{
    2,3,
    2,5,
    3,
    5,5,
    2,3,
    2,3,
    2,3,0
};
#endif

static T_RV_RET
write_commands_to_camera (UINT8 * bytes_p, UINT8 * commands_p,
			  UINT16 size_of_bytes, BOOL vf_mode)
{


	UINT16 i;
  UINT16 nr_of_words;
	T_RV_RET ret_val = RV_OK;

  BspI2c_ScenarioDescriptor sd;
  BspI2c_TransactionRequest *tranArr;
	nr_of_words = sizeof(BspI2c_TransactionRequest);

    	if (rvf_get_buf (camd_env_ctrl_blk_p->sec_mb_id,
      		2*sizeof(BspI2c_TransactionRequest),
 			   (T_RVF_BUFFER **) &tranArr) == RVF_RED)
      {
        CAMD_SEND_TRACE ("CAMD commands out of memory", RV_TRACE_LEVEL_ERROR);
        return RV_MEMORY_ERR;
      }


	CAMD_ASSERT (bytes_p[size_of_bytes - 1] == 0);

	/* Fill up transaction details*/
  for (i = 0; (i < (size_of_bytes - 1)); i++)
	{
		/* Fill up Scenario descriptor */
  	sd.count =1; 
  	sd.deviceId = BSP_I2C_DEVICE_ID_CAMERA;
  	sd.transactionDoneCallback = NULL;
  	sd.transactionId = 0;
  	sd.delayFunctionPtr = NULL;
  	sd.delayValue = 0;

		tranArr[0].transactionType = BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE;
    tranArr[0].dataStartAddress = *commands_p;
		commands_p++;
    tranArr[0].dataPtr = commands_p;
    tranArr[0].dataCount = bytes_p[i]-1;
   	nr_of_words = (bytes_p[i] + 1) / 2;
		/* Point to next set of commands */
	  commands_p += (nr_of_words*2)-1;

		
 		if (bspI2c_submitTransaction(BSP_I2C_1,&sd,tranArr) != RV_OK)
		{
	  	CAMD_SEND_TRACE ("CAMD could not write i2c commands",
					   RV_TRACE_LEVEL_ERROR);
		ret_val = RV_INTERNAL_ERR;
		break;
		
		}
		

  }

	/* Free message */
  if (rvf_free_buf (tranArr) != RVF_OK)
  {
    CAMD_SEND_TRACE ("CAMD commands: Unable to free message",
      RV_TRACE_LEVEL_ERROR);
  }

	return ret_val;
}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_stop_sensor                          								*/
/*                                                                              */
/*    Purpose:  Sends 'stop sensor' commands to sensor.													*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*						- None.																														*/
/*    Return :                                                                  */
/*             -None.																														*/
/*                                                                              */
/*    Note:                                                                     */
/* 			None.																																		*/
/*                                                                              */
/********************************************************************************/

    void
camd_stop_sensor (void)
{

    UINT8 bytes[] = { 2, 3, 0 };


    static const UINT8 commands[] = {
       0xFE,0x00,     /*0x0002,0x0000   Camera control*/
       0x04,0x00, 0x00,0x00
    };

    CAMD_SEND_TRACE ("CAMD entering camd_stop_sensor",
            RV_TRACE_LEVEL_DEBUG_HIGH);

    write_commands_to_camera (bytes, (UINT8 *)commands, sizeof (bytes), FALSE);


    CAMD_SEND_TRACE ("CAMD leaving camd_stop_sensor",
            RV_TRACE_LEVEL_DEBUG_HIGH);

}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_change_fps                        									*/
/*                                                                              */
/*    Purpose:  Changes frames/second settings in camera register 							*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*						- mode (to set 40fps or 15fps).																		*/
/*    Return :                                                                  */
/*             -None.																														*/
/*                                                                              */
/*    Note:                                                                     */
/* 			None.																																		*/
/*                                                                              */
/********************************************************************************/

void camd_change_fps(UINT8 mode)
{
    /* This needs to be checked later(H/W contraint) */
    UINT8 bytes1[] = { 2,3,0 };
    static UINT8 commands1[] = {
       0xFE,0x00,                       /*0002 0004*/
       0x04,0x04, 0x00,0x00,
       0x00
    };


    write_commands_to_camera (bytes1, (UINT8 *)commands1, sizeof (bytes1), FALSE);
}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_switch_preview                        							*/
/*                                                                              */
/*    Purpose:  Sends 'switch preview' commands to sensor												*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*						- None.																														*/
/*    Return :                                                                  */
/*             -None.																														*/
/*                                                                              */
/*    Note:                                                                     */
/* 			None.																																		*/
/*                                                                              */
/********************************************************************************/
    void
camd_switch_preview (void)
{




    static const UINT8 bytesPre[] = { 2,3,/*2,3,*/0 };  

    UINT8 commandsPre[] = {          
       0xFE,0x00,	/*0002 0001 Run in viewfinder mode*/
       0x04,0x01,0x00,0x00,    

       0xFE, 0x20,	      /*0x1008 0x8013 Output control*/
	   0x10, 0x13, 0x90,0x00,	
       0x00  };




    if(camd_encoding == CAMD_YUYV_INTERLEAVED)

    {                      
        commandsPre[9] = 0x19;
    }


    CAMD_SEND_TRACE ("CAMD entering camd_switch_preview",RV_TRACE_LEVEL_DEBUG_HIGH);


#ifdef READ_COMMANDS
    read_commands_from_camera();
#endif
    while(read_status_bit());
    write_commands_to_camera ((UINT8 *)bytesPre, (UINT8 *)commandsPre, sizeof (bytesPre), FALSE);


    CAMD_SEND_TRACE ("CAMD leaving camd_switch_preview",RV_TRACE_LEVEL_DEBUG_HIGH);

}

void
camd_switch_snapshot (void)
{
	static const UINT8 bytesSnap[] = { 2,3,0 };
  static const UINT8 commandsSnap[] = {
	0xFE,0x00,
	0x04,0x02, 0x00,0x00
  };

	CAMD_SEND_TRACE ("CAMD entering camd_switch_snapshot",
				   RV_TRACE_LEVEL_DEBUG_HIGH);

	while(read_status_bit());
  write_commands_to_camera ((UINT8 *)bytesSnap, (UINT8 *)commandsSnap, sizeof (bytesSnap), FALSE);

	CAMD_SEND_TRACE ("CAMD leaving camd_switch_snapshot",
				   RV_TRACE_LEVEL_DEBUG_HIGH);
				   
}

void
camd_change_viewfinder_imagedim(UINT16 w, UINT16 h, T_CAMD_ENCODING encoding, BOOL flip_x, BOOL flip_y, UINT16 zoom)
{

    UINT16 m,n,p,q,a,b,c,d,iheight,iwidth,iheight_s,iwidth_s;
    UINT8 mi,ni,pi,qi,ai,bi,ci,di;
    float height_s,width_s,height,width;


    float aspect;

    static const UINT8 bytesPre[] = { 2,3,2,3,2,3,2,3,2,3,2,3,0 };  
    UINT8 commandsPre[] = {	
       0xFE, 0x00,                 /*0008 0007 Image size and orientation*/
       0x10, 0x07, 0x00, 0x00,

       0xFE,0x00,	
       0x3c,0x00,0x00,0x00,	/*001E 0x0000 output height in video mode*/

       0xFE, 0x00,	
       0x38, 0x00, 0x00,0x00,	/*0x001c output width in viewfinder mode*/

       0xFE,0x00,	
       0x34,0x00,0x00,0x00,	/*0x001A 0x0000 sensor width in viewfinder*/

       0xFE, 0x00,	
       0x30, 0x00, 0x00,0x00,	/*0x0018 0x0000 sensor height in viewfinder mode*/

       0xFE, 0x00,
       0x14, 0x03, 0x09, 0x00,/*0x000A 0x0903 RGB*/

       0x00 };

    height =h;
    width = w;
    iheight = (UINT16)height;
    iwidth = (UINT16)width;

    m = iheight&65280;
    m=m>>8;

    n = iheight&255;

    p = iwidth&65280;
    p=p>>8;

    q = iwidth&255;

    aspect = width/640;;
    width_s  =640;
    height_s = height/aspect;
    if(height_s>480)
    {
        aspect = height/480;
        height_s = 480;
        width_s = width/aspect;
    }
    if(width_s>640)
    {
        aspect = width/height;
        height_s = 480;
        width_s = aspect*height_s;
    }
    if(width_s>640)
    {
        aspect = height/width;
        width_s = 640;
        height_s = aspect * width_s;
    }

    iheight_s = (UINT16)height_s;
    iwidth_s = (UINT16)width_s;

    a = iheight_s&65280;
    a=a>>8;

    b = iheight_s&255;

    c = iwidth_s&65280;
    c=c>>8;

    d = iwidth_s&255;

    CAMD_SEND_TRACE ("CAMD entering camd_change_viewfinder_imagedim",
            RV_TRACE_LEVEL_DEBUG_HIGH);

    commandsPre[9] = n;

    commandsPre[10] = m;

    commandsPre[15] = q;

    commandsPre[16] =p;

    commandsPre[21] = b;

    commandsPre[22] = a;

    commandsPre[27] = d;

    commandsPre[28] = c;

    if(encoding == CAMD_YUYV_INTERLEAVED)
    {
        commandsPre[33] = 0x09;
    }

    if(flip_x == TRUE && flip_y == TRUE)
    {
        commandsPre[3] = 0x37;
    }
    else if(flip_x == TRUE)
    {
        commandsPre[3] = 0x17;
    }
    else if(flip_y == TRUE)
    {
        commandsPre[3] = 0x27;
    }


    while(read_status_bit());
    write_commands_to_camera ((UINT8 *)bytesPre, (UINT8 *)commandsPre, sizeof (bytesPre), FALSE);


    CAMD_SEND_TRACE ("CAMD leaving camd_change_viewfinder_imagedim",
            RV_TRACE_LEVEL_DEBUG_HIGH);

}



    void
camd_change_snapshot_imagedim(UINT16 w, UINT16 h, T_CAMD_ENCODING encoding, BOOL flip_x, BOOL flip_y, UINT16 zoom)
{

    UINT16 m,n,p,q,a,b,c,d,iheight,iwidth,iheight_s,iwidth_s;
    UINT8 mi,ni,pi,qi,ai,bi,ci,di;
    float height_s,width_s,height,width;

    float aspect;

    static const UINT8 bytesSnap[] = { 2,3,2,3,2,3,2,3,2,3,2,3,0 };  
    UINT8 commandsSnap[] = {	
       0xFE, 0x00,
       0x10, 0x00, 0x07, 0x00,/*(0008,0700) Image size and orientation*/

       0xFE,0x00,	
       0x4c,0x00,0x00,0x00,	/*(0026,0000)Output window height in still mode*/

       0xFE, 0x00,			/*(0024,0000)Output window width*/ 
       0x48, 0x00, 0x00,0x00,	

       0xFE,0x00,			/*(0022,0000)sensor window height*/
       0x44,0x00,0x00,0x00,	

       0xFE, 0x00,			/*0020,0000 sensor window width*/
       0x40, 0x00, 0x00,0x00,	

       0xFE, 0x00,			/*000A,0903 output format YUV , RGB ,ETC*/
       0x14, 0x03, 0x09, 0x00,

       0x00 };

    height =h;
    width = w;
    iheight = (UINT16)height;
    iwidth = (UINT16)width;

    m = iheight&65280;
    m=m>>8;

    n = iheight&255;

    p = iwidth&65280;
    p=p>>8;

    q = iwidth&255;

    aspect = width/640;;
    width_s  =640;
    height_s = height/aspect;
    if(height_s>480)
    {
        aspect = height/480;
        height_s = 480;
        width_s = width/aspect;
    }
    if(width_s>640)
    {
        aspect = width/height;
        height_s = 480;
        width_s = aspect*height_s;
    }
    if(width_s>640)
    {
        aspect = height/width;
        width_s = 640;
        height_s = aspect * width_s;
    }

    iheight_s = (UINT16)height_s;
    iwidth_s = (UINT16)width_s;

    a = iheight_s&65280;
    a=a>>8;

    b = iheight_s&255;

    c = iwidth_s&65280;
    c=c>>8;

    d = iwidth_s&255;

    CAMD_SEND_TRACE ("CAMD entering camd_change_snapshot_imagedim",
            RV_TRACE_LEVEL_DEBUG_HIGH);




    commandsSnap[9] = n;

    commandsSnap[10] = m;

    commandsSnap[15] = q;

    commandsSnap[16] = p;

    commandsSnap[21] = b;

    commandsSnap[22] = a;

    commandsSnap[27] = d;

    commandsSnap[28] = c;

    if(encoding == CAMD_RGB_565)
    {
        commandsSnap[34] = 0x03;
    }

    if(flip_x == TRUE && flip_y == TRUE)
    {
        commandsSnap[4] = 0x37;
    }
    else if(flip_x == TRUE)
    {
        commandsSnap[4] = 0x17;
    }
    else if(flip_y == TRUE)
    {
        commandsSnap[4] = 0x27;
    }

    while(read_status_bit());
    write_commands_to_camera ((UINT8 *)bytesSnap, (UINT8 *)commandsSnap, sizeof (bytesSnap), FALSE);


    CAMD_SEND_TRACE ("CAMD leaving camd_change_snapshot_imagedim",
            RV_TRACE_LEVEL_DEBUG_HIGH);

}


void camd_get_frame()
{

    static const UINT8 commandsFrame[]={
       0x03,0x00,0x00,
       0x00	
    };

    T_RV_RET ret_val;

    BspI2c_ScenarioDescriptor sd;
    BspI2c_TransactionRequest tranArr[1];

    /* Fill up Scenario descriptor */
    sd.count =1; 
    sd.deviceId = BSP_I2C_DEVICE_ID_CAMERA;
    sd.transactionDoneCallback = NULL;
    sd.transactionId = 0;
    sd.delayFunctionPtr = NULL;
    sd.delayValue = 0;

    tranArr[0].transactionType = BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE;
    tranArr[0].dataStartAddress = 0x08;

    tranArr[0].dataPtr = (UINT8 *)commandsFrame;
    tranArr[0].dataCount = 2;
#ifdef Bsp_I2C_DMA_Enable
    tranArr[0].dmaRequestId = (UINT8)BSP_DMA_REQ_NONE;
#endif



    if (bspI2c_submitTransaction(BSP_I2C_1,&sd,tranArr) != RV_OK)
    {
        CAMD_SEND_TRACE ("CAMD could not write i2c commands",
                RV_TRACE_LEVEL_ERROR);
    }
}



/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_init_sensor                        									*/
/*                                                                              */
/*    Purpose:  Sends 'init sensor' commands to sensor													*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*						- None.																														*/
/*    Return :                                                                  */
/*             -None.																														*/
/*                                                                              */
/*    Note:                                                                     */
/* 			None.																																		*/
/*                                                                              */
/********************************************************************************/
    void
camd_init_sensor (void)
{
    UINT8 count=0;
    I2C_complete = 0;
    CAMD_SEND_TRACE ("CAMD entering camd_init_sensor",
            RV_TRACE_LEVEL_DEBUG_HIGH);
    do{


       check_sensor();
       do {
          rvf_delay(10);	
          if(!I2C_complete)
              count++;
       }while((!I2C_complete) && (count < 5));

       if(I2C_complete)
       {
           write_commands_to_camera (camd_init_sensor_bytes,
                   (UINT8 *)camd_init_sensor_commands,
                   sizeof (camd_init_sensor_bytes), FALSE);
           while(read_status_bit());

           write_commands_to_camera (camd_init_sensor_bytes2,
                   (UINT8 *)camd_init_sensor_commands2,
                   sizeof (camd_init_sensor_bytes2), FALSE);
       }
       else
           CAMD_SEND_TRACE ("CAMD Error is writing to sensor",
                   RV_TRACE_LEVEL_ERROR);


       CAMD_SEND_TRACE ("CAMD leaving camd_init_sensor",
               RV_TRACE_LEVEL_DEBUG_HIGH);
    }while(0);
}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   read_status_bit                        									*/
/*                                                                              */
/*    Purpose:  Reads status bit of camera sesnor.															*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*						- None.																														*/
/*    Return :                                                                  */
/*             -status bit value.																								*/
/*                                                                              */
/*    Note:                                                                     */
/* 			None.																																		*/
/*                                                                              */
/********************************************************************************/
UINT8 read_status_bit()
{

    T_RV_RET ret_val;

    BspI2c_ScenarioDescriptor sd;
    BspI2c_TransactionRequest tranArr[1];

    /* Fill up Scenario descriptor */
    sd.count =1; 
    sd.deviceId = BSP_I2C_DEVICE_ID_CAMERA;

    sd.transactionDoneCallback = NULL;

    sd.transactionId = 0;
    sd.delayFunctionPtr = NULL;
    sd.delayValue = 0;

    tranArr[0].transactionType = BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_READ;
    tranArr[0].dataStartAddress = 0x05;

    tranArr[0].dataPtr = (UINT8 *)commandsRd;
    tranArr[0].dataCount = 2;
#ifdef Bsp_I2C_DMA_Enable
    tranArr[0].dmaRequestId = (UINT8)BSP_DMA_REQ_NONE;
#endif

    if (bspI2c_submitTransaction(BSP_I2C_1,&sd,tranArr) != RV_OK)
    {
        CAMD_SEND_TRACE ("CAMD could not write i2c commands",
                RV_TRACE_LEVEL_ERROR);
        return 0;
    }
    return (commandsRd[0]&0x04);
}

/********************************************************************************/
void check_sensor()
{

	T_RV_RET ret_val;

  BspI2c_ScenarioDescriptor sd;
  BspI2c_TransactionRequest tranArr[1];

		/* Fill up Scenario descriptor */
  	sd.count =1; 
  	sd.deviceId = BSP_I2C_DEVICE_ID_CAMERA;

		sd.transactionDoneCallback = camd_i2c_callback;
		
  	sd.transactionId = 0;
  	sd.delayFunctionPtr = NULL;
  	sd.delayValue = 0;

		tranArr[0].transactionType = BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_READ;
    tranArr[0].dataStartAddress = 0x05;

    tranArr[0].dataPtr = (UINT8 *)commandsRd;
    tranArr[0].dataCount = 2;
     #ifdef Bsp_I2C_DMA_Enable
    tranArr[0].dmaRequestId = BSP_DMA_REQ_NONE;
    #endif


		

 		if (bspI2c_submitTransaction(BSP_I2C_1,&sd,tranArr) != RV_OK)
		{
	  	CAMD_SEND_TRACE ("CAMD could not write i2c commands",
					   RV_TRACE_LEVEL_ERROR);
		}
}




/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_init_camera                        									*/
/*                                                                              */
/*    Purpose:  Initializes I2C to accept commands from CAMD.										*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*						- None.																														*/
/*    Return :                                                                  */
/*             -None.																														*/
/*                                                                              */
/*    Note:                                                                     */
/* 			None.																																		*/
/*                                                                              */
/********************************************************************************/
BspI2c_ConfigInfo camd_I2c_configInfo; 
void camd_init_camera(void)
{
    T_RV_RET ret_val;

    /* Writing Polled Mode Interface Only
    */


    camd_I2c_configInfo.busSpeed = BSP_I2C_DEVICE_BUS_SPEED_400K;
    camd_I2c_configInfo.addressOctetLength = BSP_I2C_ADDRESS_OCTET_LENGTH_1;
    camd_I2c_configInfo.addressAutoIncrement = BSP_I2C_ADDRESS_AUTO_INCREMENT_SUPPORTED;
    camd_I2c_configInfo.deviceAddressBitLength = BSP_I2C_DEVICE_ADDRESS_LENGTH_7BIT;
    camd_I2c_configInfo.multiByteRead = BSP_I2C_MULTI_BYTE_READ_SUPPORTED;
    camd_I2c_configInfo.addressByteOrder = BSP_I2C_ADDRESS_ORDER_LOW_TO_HIGH;
    camd_I2c_configInfo.deviceAddress = BSP_I2C_DEVICE_ADDRESS_CAMERA;


    if (bspI2c_config(BSP_I2C_DEVICE_ID_CAMERA,&camd_I2c_configInfo) != RV_OK)
    {
        CAMD_SEND_TRACE ("CAMD could not configure i2c for Camera",RV_TRACE_LEVEL_ERROR);
    }

}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_init_sensor_clock                  									*/
/*                                                                              */
/*    Purpose:  Initializes clock for camera module.														*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*						- None.																														*/
/*    Return :                                                                  */
/*             -None.																														*/
/*                                                                              */
/*    Note:                                                                     */
/* 			None.																																		*/
/*                                                                              */
/********************************************************************************/
    void
camd_init_sensor_clock (void)
{


    /* DPLL clock := 13 MHz */
    CAMERA_CLK_REG(CLKM_CNTL_CLK_CAM) |= DPLL_CLK_INIT;

}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_stop_sensor_clock                  									*/
/*                                                                              */
/*    Purpose:  Disables clock to camera module.																*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*						- None.																														*/
/*    Return :                                                                  */
/*             -None.																														*/
/*                                                                              */
/*    Note:                                                                     */
/* 			None.																																		*/
/*                                                                              */
/********************************************************************************/
    void
camd_stop_sensor_clock (void)
{


    CAMERA_CLK_REG(CLKM_CNTL_CLK_CAM) &= ~DPLL_CLK_ON;

}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_start_sensor_clock                  								*/
/*                                                                              */
/*    Purpose:  Enables clock to camera module.																	*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*						- None.																														*/
/*    Return :                                                                  */
/*             -None.																														*/
/*                                                                              */
/*    Note:                                                                     */
/* 			None.																																		*/
/*                                                                              */
/********************************************************************************/

    void
camd_start_sensor_clock (void)
{


    CAMERA_CLK_REG(CLKM_CNTL_CLK_CAM) |= DPLL_CLK_ON;

}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_write_viewfinder_gamma_correction_to_camera					*/
/*                                                                              */
/*    Purpose:  writes the viewfinder gamma correction to the camera.						*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*						- gamma_correction:	Gamma correction values.											*/
/*    Return :                                                                  */
/*             -None.																														*/
/*                                                                              */
/*    Note:                                                                     */
/* 			None.																																		*/
/*                                                                              */
/********************************************************************************/

T_RV_RET
camd_write_viewfinder_gamma_correction_to_camera (T_CAMD_GAMMA
        gamma_correction)
{
    T_RV_RET ret_val;

    ret_val = write_commands_to_camera (camd_gamma_bytes,
            gamma_correction_array[gamma_correction],
            sizeof (camd_gamma_bytes), FALSE);

    return ret_val;
}
/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_write_snapshot_gamma_correction_to_camera						*/
/*                                                                              */
/*    Purpose:  writes the snapshot gamma correction to the camera.							*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*						- gamma_correction:	Gamma correction values.											*/
/*    Return :                                                                  */
/*             -None.																														*/
/*                                                                              */
/*    Note:                                                                     */
/* 			None.																																		*/
/*                                                                              */
/********************************************************************************/

    T_RV_RET
camd_write_snapshot_gamma_correction_to_camera (T_CAMD_GAMMA gamma_correction)
{
    T_RV_RET ret_val;

    ret_val = write_commands_to_camera (camd_gamma_bytes,
            gamma_correction_array[gamma_correction],
            sizeof (camd_gamma_bytes), FALSE);

    return ret_val;
}

#ifdef READ_COMMANDS
UINT8 read_commands_from_camera (void)
{

    T_RV_RET ret_val;

    BspI2c_ScenarioDescriptor sd;
    BspI2c_TransactionRequest tranArr[1];
    UINT16 counter=0;
    UINT16 command;
    UINT16 address = 0x0100;
    UINT8 i,j,M = 64;
    UINT8 commandsWr = 0x02;

    for(i=0;i<4;i++)
    {

        /* Fill up Scenario descriptor */
        sd.count =1; 
        sd.deviceId = BSP_I2C_DEVICE_ID_CAMERA;
        sd.transactionDoneCallback = NULL;
        sd.transactionId = 0;
        sd.delayFunctionPtr = NULL;
        sd.delayValue = 0;

        tranArr[0].transactionType = BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE;
        tranArr[0].dataStartAddress = 0xFE;

        tranArr[0].dataPtr = (UINT8 *)(&commandsWr);
        tranArr[0].dataCount = 1;
        tranArr[0].dmaRequestId = BSP_DMA_REQ_NONE;


        ret_val = bspI2c_submitTransaction(BSP_I2C_1,&sd,tranArr); 

        if (ret_val != RV_OK)
        {
            CAMD_SEND_TRACE ("CAMD could not write i2c commands",
                    RV_TRACE_LEVEL_ERROR);
            return 0;
        }
        CAMD_SEND_TRACE_PARAM("Address starts from 0xFE",commandsWr,RV_TRACE_LEVEL_DEBUG_HIGH);
        CAMD_SEND_TRACE("================================================================",RV_TRACE_LEVEL_DEBUG_HIGH);
        commandsWr++;
        counter = 0;
        if(i == 3)
            M = 12;
        rvf_delay(150);
        for(j=0;j<M;j++)
        {

            /* Fill up Scenario descriptor */
            sd.count =1; 
            sd.deviceId = BSP_I2C_DEVICE_ID_CAMERA;
            sd.transactionDoneCallback = NULL;
            sd.transactionId = 0;
            sd.delayFunctionPtr = NULL;
            sd.delayValue = 0;
            tranArr[0].transactionType = BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_READ;
            tranArr[0].dataStartAddress = counter+1;

            tranArr[0].dataPtr = (UINT8 *)commandsRd;
            tranArr[0].dataCount = 2;
            tranArr[0].dmaRequestId = BSP_DMA_REQ_NONE;


            ret_val = bspI2c_submitTransaction(BSP_I2C_1,&sd,tranArr); 
            command = (commandsRd[1] << 8) | commandsRd[0];

            CAMD_SEND_TRACE_PARAM("Address = ",address,RV_TRACE_LEVEL_DEBUG_HIGH);
            CAMD_SEND_TRACE_PARAM("Data =  ",command,RV_TRACE_LEVEL_DEBUG_HIGH);
            counter+= 4;
            address += 2; 


            if (ret_val != RV_OK)
            {
                CAMD_SEND_TRACE ("CAMD could not write i2c commands",
                        RV_TRACE_LEVEL_ERROR);
                return 0;
            }
        }
        CAMD_SEND_TRACE_PARAM("Address ends",counter-4,RV_TRACE_LEVEL_DEBUG_HIGH);
        CAMD_SEND_TRACE("================================================================",RV_TRACE_LEVEL_DEBUG_HIGH);
    }

    return ret_val;
}
#endif

void camd_i2c_callback(BspI2c_TransactionId tid)
{
    if(tid!=0xFE)
        I2C_complete = 1;
}


#else
/**
 * @file  camd_commands.c
 *
 * @author  Remco Hiemstra
 * @version 0.1
 */

/* History:
 *
 * Date              Modification
 *-----------------------------------
 *  27-Jun-2005								Venugopal Naik
 *  11-Sept-2006 								Magnus Aman
 *				Added Micron Sensor MT9x112 for Locosto only 
 *				and separated code into three tracks for clarity:
 *				1 - Common code
 *				2 - Locosto platform
 *				3 - Other platforms 
 *
 * (c) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "rv/rv_general.h"
#include "camd/camd_i.h"
#include "camd/camd_commands.h"
#include "camd/camd_addresses.h"

#include "i2c/bspI2c.h"
#include "clkm.h"
#include "i2c/bspI2cMultiMaster.h"




#define CAMD_MICRON_MT9x112
#define CAMERA_ADDRESS 		0x05D	/*This is usedin bspI2c_Platform.h*/


#define CAMERA_CLK_REG(CC_XXX_REG) (*((volatile UINT32 *)(CC_XXX_REG)))

#define DPLL_CLK_INIT (0x0005)
#define DPLL_CLK_ON (0x0001)

void camd_init_camera(void);
void camd_get_frame(void);
void camd_change_fps(UINT8);
static T_RV_RET write_commands_to_camera (UINT8 * bytes_p, UINT8 * commands_p,
        UINT16 size_of_bytes, BOOL vf_mode);
UINT8 read_status_bit();


void camd_i2c_callback(BspI2c_TransactionId t_id);
UINT8 I2C_complete = 0;

UINT8 commandsRd[]={
   0x00,0x00,0x00
};
#undef READ_COMMANDS
#undef TESTING

#ifdef READ_COMMANDS
UINT8 read_commands_from_camera (void);
#endif



#ifdef CAMD_MICRON_MT9x112


static UINT16 sensor_version = 0;

const T_CAMD_CAMERA_CAPABILITIES camd_camera_capabilities = {
   3,
   {
      {
           CAMD_QCIF,
           CAMD_RGB_565,
           176,
           144,
          // 5,							/* number of bits in R component */
          // 6,							/* number of bits in G component */
          // 5,							/* number of bits in B component */
          // 0,							/* number of bits in Y component */
          // 0,							/* number of bits in U component */
          // 0,							/* number of bits in V component */
           0							/* 0 = no zoom supported */
       }, 
       {
           CAMD_VGA,
           CAMD_YUYV_INTERLEAVED,
           640,
           480,
         //  0,							/* number of bits in R component */
         //  0,							/* number of bits in G component */
         //  0,							/* number of bits in B component */
         //  8,							/* number of bits in Y component */
         //  8,							/* number of bits in U component */
         //  8,							/* number of bits in V component */
           0							/* 0 = no zoom supported */
       },
#if (CAM_SENSOR==MICRON)       
       {
           CAMD_SXGA,
           CAMD_YUYV_INTERLEAVED,
           1280,
           1024,
          // 0,							/* number of bits in R component */
         //  0,							/* number of bits in G component */
         //  0,							/* number of bits in B component */
        //   8,							/* number of bits in Y component */
          // 4,							/* number of bits in U component */
         //  4,							/* number of bits in V component */
           0							/* 0 = no zoom supported */
       }
#endif       
   }
   
};

static UINT8 camd_init_sensor_bytes[] =  
{
    3, 3, 3, 3, 3, 3, 		// Context A - Viewfinder
    3, 3, 3,				// Context B - Snapshot
    3, 3, 3, 3, 3, 3, 3, 3, 3,	// Colors and Intesity
    3,					// Noise reduction
    3,					// Auto Exposure, AWB, Lens Correction
    3, 3, 				// Vertical blanking time
    3, 3, 3, 3,			// Run preview
    0
};

static UINT8 camd_init_sensor_commands[] = 
{

    // Context A - ViewFinder QCIF in RGB565

    0xF0, 0x00, 0x01, 0x00,	// Select Register Page 1	
    0xA5, 0x40, 0x00, 0x00,	// HORIZ_PAN_RESIZE_A
    0xA8, 0x40, 0x00, 0x00,	// VERT_PAN_RESIZE_A
    0xA7, 0x00, 0xB0, 0x00,	// HORIZ_SIZE_RESIZE_A
    0xAA, 0x00, 0x90, 0x00,	// VERT_SIZE_RESIZE_A
    0x3A, 0x03, 0x00, 0x00,	// FORMAT_OUTPUT_CONTROL2A - RGB565

    // Context B - Snapshot VGA in YUV

    //	0xA1, 0x02, 0x80, 0x00,	// HORIZ_SIZE_RESIZE_B
    //	0xA4, 0x01, 0xE0, 0x00,	// VERT_SIZE_RESIZE_B
    //	0x9B, 0x02, 0x00, 0x00,	   FORMAT_OUTPUT_CONTROL2B - YUV

    // Alternate Context B - Snapshot SXGA in YUV

    0xA1, 0x05, 0x00, 0x00,	// HORIZ_SIZE_RESIZE_B
    0xA4, 0x04, 0x00, 0x00,	// VERT_SIZE_RESIZE_B
    0x9B, 0x02, 0x00, 0x00,	// FORMAT_OUTPUT_CONTROL2B  - YUV


    // Color and Intensity

    0xE2, 0x70, 0x00, 0x00,	// EFFECTS_MODE
    0x34, 0x00, 0x00, 0x00,	// LUMA_OFFSET
    0x35, 0xFF, 0x00, 0x00,	// CLIPPING_LIM_OUT_LUMA
    0x53, 0x0E, 0x04, 0x00,	// GAMMA_A_Y1_Y2
    0x54, 0x4C, 0x28, 0x00,	// GAMMA_A_Y3_Y4
    0x55, 0x97, 0x77, 0x00,	// GAMMA_A_Y5_Y6
    0x56, 0xC7, 0xB1, 0x00,	// GAMMA_A_Y7_Y8
    0x57, 0xEE, 0xDB, 0x00,	// GAMMA_A_Y9_Y10
    0x58, 0xFF, 0x00, 0x00,	// GAMMA_A_Y0_Y11

    // Noise reduction
    0x9D, 0x3C, 0xAE, 0x00,	// DEFECT_CORRECTION - Default

    // Auto exposure ON, AWB ON, Lens correction OFF
    0x06, 0x60, 0x0E, 0x00,	// MODE_CONTROL

    // Vertical blanking
    0xF0, 0x00, 0x00, 0x00,	// Select Register Page 0
    0x08, 0x00, 0x20, 0x00,	// VERT_BLANK_A  - 5ms is our target

    // Shutter speed, Run preview
    0xF0, 0x00, 0x02, 0x00, 	// Select Register Page 2
    0x37, 0x81, 0x00, 0x00,	// SHUTTER_WIDTH_LIM_AE - 15fps
    0xD2, 0x00, 0x00, 0x00,	// DEFAULT_CONFIG Context A

    0xCC, 0x00, 0x04, 0x00, 	// PROGRAM_SELECT Preview
    0xCB, 0x00, 0x01, 0x00,	// PROGRAM_ADVANCE

    0
};



#endif  // CAMD_MICRON






/*------------------------------------------------------------------------------*/
/*                                        Common platform code section begin                                           */
/*------------------------------------------------------------------------------*/



/********************************************************************************/
/*    Function Name:   camd_init_camera                        									*/
/********************************************************************************/

BspI2c_ConfigInfo CameraI2C_configInfo; 

void camd_init_camera(void)
{
    BspI2c_ReturnCode ret_val;

    /* Writing Polled Mode Interface Only  */


    CameraI2C_configInfo.busSpeed = BSP_I2C_DEVICE_BUS_SPEED_400K;
    CameraI2C_configInfo.addressOctetLength = BSP_I2C_ADDRESS_OCTET_LENGTH_1;
    CameraI2C_configInfo.addressAutoIncrement = BSP_I2C_ADDRESS_AUTO_INCREMENT_SUPPORTED;
    CameraI2C_configInfo.deviceAddressBitLength = BSP_I2C_DEVICE_ADDRESS_LENGTH_7BIT;
    CameraI2C_configInfo.multiByteRead = BSP_I2C_MULTI_BYTE_READ_SUPPORTED;
    CameraI2C_configInfo.addressByteOrder = BSP_I2C_ADDRESS_ORDER_LOW_TO_HIGH;
    CameraI2C_configInfo.deviceAddress = BSP_I2C_DEVICE_ADDRESS_CAMERA;

    ret_val = bspI2c_config(BSP_I2C_DEVICE_ID_CAMERA,&CameraI2C_configInfo);
    if (ret_val != BSP_I2C_RETURN_CODE_SUCCESS  )
    {
        CAMD_SEND_TRACE ("CAMD could not configure i2c for Camera",RV_TRACE_LEVEL_ERROR);
    }

}


/***********************************************
Write to one register
**********************************************
*/


void camd_write_sensor(UINT16 reg, UINT16 data)
{

    UINT8 local_bytesGet[] = { 3,3, 0 };
    UINT8 local_commandsGet[] = 
    {
        0xF0, 0x00, 0x00, 0x00,	// Select Register Page 0
        0x00,0x00,0x00,0x00
    };
    local_commandsGet[2]=(UINT8)(reg>>8);
    local_commandsGet[4]=(UINT8)(reg);
    local_commandsGet[5]=(UINT8)(data>>8);
    local_commandsGet[6]=(UINT8)(data);

    if(RV_OK == write_commands_to_camera ((UINT8 *)local_bytesGet, (UINT8 *)local_commandsGet, sizeof (local_bytesGet), FALSE))
        ;//CAMD_SEND_TRACE ("CAMD enablesensor op",  RV_TRACE_LEVEL_DEBUG_HIGH);
    else
        CAMD_SEND_TRACE ("CAMD camd_get_frame FAIL",  RV_TRACE_LEVEL_DEBUG_HIGH);

}


/********************************************************************************/
/*    Function Name:   camd_init_sensor                                                                                  */
/********************************************************************************/
void camd_init_sensor (void)
{
    UINT8 count=0;

    CAMD_SEND_TRACE ("CAMD entering camd_init_sensor",
            RV_TRACE_LEVEL_DEBUG_HIGH);

    I2C_complete=0;


    for(count=0;count<15;count++)
    {	
        if(RV_OK==write_commands_to_camera (camd_init_sensor_bytes,
                    (UINT8 *)camd_init_sensor_commands,
                    sizeof (camd_init_sensor_bytes), FALSE))
        { 
            I2C_complete=1;
            break; 
        }
        rvf_delay(5);
    }



}


/********************************************************************************/
/*    Function Name:   write_commands_to_camera                                                                  */
/********************************************************************************/
static T_RV_RET write_commands_to_camera (UINT8 * bytes_p, UINT8 * commands_p,
        UINT16 size_of_bytes, BOOL vf_mode)
{
    UINT16 i;
    UINT16 nr_of_words;
    BspI2c_ReturnCode ret_val;

    BspI2c_ScenarioDescriptor sd;
    BspI2c_TransactionRequest *tranArr;
    nr_of_words = sizeof(BspI2c_TransactionRequest);

    if (rvf_get_buf (camd_env_ctrl_blk_p->sec_mb_id,
                2*sizeof(BspI2c_TransactionRequest),
                (T_RVF_BUFFER **) &tranArr) == RVF_RED)
    {
        CAMD_SEND_TRACE ("CAMD commands out of memory", RV_TRACE_LEVEL_ERROR);
        return RV_MEMORY_ERR;
    }

    CAMD_ASSERT (bytes_p[size_of_bytes - 1] == 0);

    /* Fill up transaction details*/
    for (i = 0; (i < (size_of_bytes - 1)); i++)
    {
        /* Fill up Scenario descriptor */
        sd.count =1; 
        sd.deviceId = BSP_I2C_DEVICE_ID_CAMERA;
        sd.transactionDoneCallback = NULL;
        sd.transactionId = 0;
        sd.delayFunctionPtr = NULL;
        sd.delayValue = 0;

        tranArr[0].transactionType = BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE;
        tranArr[0].dataStartAddress = *commands_p;
        commands_p++;
        tranArr[0].dataPtr = commands_p;
        tranArr[0].dataCount = bytes_p[i]-1;

        nr_of_words = (bytes_p[i] + 1) / 2;
        /* Point to next set of commands */
        commands_p += (nr_of_words*2)-1;

        ret_val = bspI2c_submitTransaction((BspI2c_InstanceId)BSP_I2C_1,&sd,tranArr); 
        if (ret_val != BSP_I2C_RETURN_CODE_SUCCESS )
        {
            CAMD_SEND_TRACE ("CAMD could not write i2c commands",
                    RV_TRACE_LEVEL_ERROR);
            break;
        }

    }

    /* Free message */
    if (rvf_free_buf (tranArr) != RVF_OK)
    {
        CAMD_SEND_TRACE ("CAMD commands: Unable to free message",
                RV_TRACE_LEVEL_ERROR);
    }

    if(ret_val!=BSP_I2C_RETURN_CODE_SUCCESS) {

       return RV_INTERNAL_ERR;

    }
    else   {

       return RV_OK; 	
    }

}



/********************************************************************************/
/*    Function Name:   camd_stop_sensor                                                                                */
/********************************************************************************/
void camd_stop_sensor (void)
{

    // No implementation exist yet - placeholder for future
    ;
}



/********************************************************************************/
/*    Function Name:   camd_switch_snapshot                                                                          */
/********************************************************************************/
void camd_switch_snapshot (void)
{
    // No implementation exist yet - placeholder for future
    ;
}



/********************************************************************************/
/*    Function Name:   camd_change_fps                                                                                  */
/********************************************************************************/
void camd_change_fps(UINT8 mode)
{

    // No implementation exist yet - placeholder for future
    ;

}



/********************************************************************************/
/*    Function Name:   camd_enable_sensor_output                                                                            */
/********************************************************************************/
void camd_enable_sensor_output (void)
{

     
}


void camd_disable_sensor_output()

{

}


/********************************************************************************/
/*    Function Name:   camd_init_sensor_clock                                                                         */
/********************************************************************************/
void camd_init_sensor_clock (void)
{

    /* DPLL clock := 52 MHz */
    CAMERA_CLK_REG(CLKM_CNTL_CLK_CAM) |= DPLL_CLK_INIT;

}



/********************************************************************************/
/*    Function Name:   camd_stop_sensor_clock                                                                        */
/********************************************************************************/
void camd_stop_sensor_clock (void)
{

    CAMERA_CLK_REG(CLKM_CNTL_CLK_CAM) &= ~DPLL_CLK_ON;

}



/********************************************************************************/
/*    Function Name:   camd_start_sensor_clock                  								*/
/********************************************************************************/
void camd_start_sensor_clock (void)
{

    CAMERA_CLK_REG(CLKM_CNTL_CLK_CAM) |= DPLL_CLK_ON;

}






/********************************************************************************/
/*    Function Name:   camd_i2c_callback                                                                                */
/********************************************************************************/
void camd_i2c_callback(BspI2c_TransactionId t_id)
{
    I2C_complete = 1;
}



/********************************************************************************/
/*    Function Name:   camd_write_viewfinder_gamma_correction_to_camera					*/
/********************************************************************************/
T_RV_RET camd_write_viewfinder_gamma_correction_to_camera (T_CAMD_GAMMA
        gamma_correction)
{
    // No implementation exist yet - placeholder for future
    return RV_OK;
}



/********************************************************************************/
/*    Function Name:   camd_write_snapshot_gamma_correction_to_camera	                            */
/********************************************************************************/
T_RV_RET camd_write_snapshot_gamma_correction_to_camera (T_CAMD_GAMMA gamma_correction)
{
    // No implementation exist yet - placeholder for future
    return RV_OK;
}



/********************************************************************************/
/*    Function Name:   camd_change_viewfinder_imagedim                                                       */
/********************************************************************************/

//UINT16 Flip_xy=0x0100;
void camd_change_viewfinder_imagedim(UINT16 w, UINT16 h, T_CAMD_ENCODING encoding, BOOL flip_x, BOOL flip_y, UINT16 zoom)

{

   UINT16   Flip_xy=0x0100;
    if(encoding==CAMD_YUYV_INTERLEAVED)		{
       camd_write_sensor(0x13A,0x0200);
    }

    else if (encoding== CAMD_RGB_565){
       camd_write_sensor(0x13A,0x0300);
    }  
    else {

       CAMD_SEND_TRACE ("CAMD encoding Feature not supported",
               RV_TRACE_LEVEL_ERROR);
    }

    if(flip_x==TRUE)		{

       Flip_xy|=0x0001;

    }

    else if (flip_x== FALSE)  {
        /*Nothing needs to be done*/
    }  
    else {

       CAMD_SEND_TRACE ("CAMD flip_x Feature not supported",
               RV_TRACE_LEVEL_ERROR);
    }

    if(flip_y==TRUE)		{
       Flip_xy|=0x0002;
    }

    else if (flip_y== FALSE){
       /*Nothing to be done currently*/
    }  
    else {

       CAMD_SEND_TRACE ("CAMD flip_x Feature not supported",
               RV_TRACE_LEVEL_ERROR);
    }
	
    camd_write_sensor(0x000D,0x8008);
  camd_write_sensor(0x0020,Flip_xy);
	camd_write_sensor(0x000D,0x0008);


/*Size registers*/

camd_write_sensor(0x01A7,w);
camd_write_sensor(0x01A,h);




	

    CAMD_SEND_TRACE ("CAMD leaving camd_change_viewfinder_imagedim",
            RV_TRACE_LEVEL_DEBUG_HIGH);


		  

//    CAMD_SEND_TRACE ("CAMD leaving camd_change_viewfinder_imagedim",
    //        RV_TRACE_LEVEL_DEBUG_HIGH);
}



/********************************************************************************/
/*    Function Name:   camd_change_snapshot_imagedim                                                         */
/********************************************************************************/
void camd_change_snapshot_imagedim(UINT16 w, UINT16 h, T_CAMD_ENCODING encoding, BOOL flip_x, BOOL flip_y, UINT16 zoom)

{
    // No implementation exist yet - placeholder for future



 

    if(encoding==CAMD_YUYV_INTERLEAVED)		{
       camd_write_sensor(0x19B,0x0200);
    }

    else if (encoding== CAMD_RGB_565){
       camd_write_sensor(0x19B,0x0300);
    }  
    else {

          CAMD_SEND_TRACE ("CAMD Feature not supported",
               RV_TRACE_LEVEL_ERROR);
    }

/*Flip is applicable for both snapshot and viewfinder*/

/*Snapshot dimensions*/
 camd_write_sensor(0x01A1,w);
 camd_write_sensor(0x01A4,h);
	

	CAMD_SEND_TRACE ("CAMD leaving camd_change_snapshot_imagedim",
            RV_TRACE_LEVEL_DEBUG_HIGH);
}



/********************************************************************************/
/*    Function Name:   camd_get_frame                                                                                   */
/********************************************************************************/
void camd_get_frame()
{
    static const UINT8 bytesGet[] = { 3, 3, 3, 0 };
    static const UINT8 commandsGet[] = 
    {
        0xF0, 0x00, 0x02, 0x00,	// Select Register Page 2
        0xCC, 0x00, 0x30, 0x00,	// PROGRAM_SELECT Snapshot AUTO 00 - config A or B selected previously in switch function
        0xCB, 0x00, 0x01, 0x00, 	// PROGRAM_ADVANCE
        0x00
       
    };

    if(RV_OK == write_commands_to_camera ((UINT8 *)bytesGet, (UINT8 *)commandsGet, sizeof (bytesGet), FALSE))
        CAMD_SEND_TRACE ("CAMD camd_get_frame OK",  RV_TRACE_LEVEL_DEBUG_HIGH);
    else
        CAMD_SEND_TRACE ("CAMD camd_get_frame FAIL",  RV_TRACE_LEVEL_DEBUG_HIGH);

}



/********************************************************************************/
/*    Function Name:   read_status_bit                                                                                     */
/********************************************************************************/
UINT8 read_status_bit()
{
    // No implementation exist yet - placeholder for future
    return 0;
}




#ifdef READ_COMMANDS
/********************************************************************************/
/*    Function Name:   read_commands_from_camera                                                               */
/********************************************************************************/
UINT8 read_commands_from_camera (void)
{
    // No implementation exist yet - placeholder for future
    return RV_OK;
}

#endif  // READ_COMMANDS



#endif
