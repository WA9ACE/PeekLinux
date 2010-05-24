/*
+-----------------------------------------------------------------------------
|  Project :  ETM-Lite
|  Modul   :  LCD
+-----------------------------------------------------------------------------
|  Copyright 2002 Texas Instruments Berlin, AG
|                 All rights reserved.
|
|                 This file is confidential and a trade secret of Texas
|                 Instruments Berlin, AG
|                 The receipt of or possession of this file does not convey
|                 any rights to reproduce or disclose its contents or to
|                 manufacture, use, or sell anything it may describe, in
|                 whole, or in part, without the specific written consent of
|                 Texas Instruments Berlin, AG.
+-----------------------------------------------------------------------------
|  Purpose :  
|
+-----------------------------------------------------------------------------
*/

#ifndef LCD_H
#define LCD_H

#ifdef FPGA_PROT
#include "map_fpga.h"
#define LCD_INTERFACE_BASE_ADDR				MAP_FPGA_LCD
#else
#endif

// Command Functions:
#define LCD_DISON 0xAF
#define LCD_DISOFF 0xAE
#define LCD_DISNOR 0xA6
#define LCD_DISINV 0xA7
#define LCD_DISCTL 0xCA
#define LCD_GCP64 0xCB
#define LCD_GCP16 0xCC
#define LCD_GSSET 0xCD
#define LCD_SLPIN 0x95
#define LCD_SLPOUT 0x94
#define LCD_SD_PSET 0x75
#define LCD_MD_PSET 0x76
#define LCD_SD_CSET 0x15
#define LCD_MD_CSET 0x16
#define LCD_DATCTL 0xBC
#define LCD_RAMWR 0x5C
#define LCD_RAMRD 0x5D
#define LCD_PTLIN 0xA8
#define LCD_PTLOUT 0xA9
#define LCD_ASCSET 0xAA
#define LCD_SCSTART 0xAB
#define LCD_VOLCTL 0xC6
#define LCD_NOP 0x25
#define LCD_OSCISEL 0x7
#define LCD_3500KSET 0xD1
#define LCD_3500KEND 0xD2
#define LCD_14MSET 0xD3
#define LCD_14MEND 0xD4

#define INIT_3500KSET 0x45
#define INIT_14MSET 0x4B
#define INIT_DATCTL 0x28 /* 5.6.5 bits for D-Sample */

#define INIT_OSCISEL 0x05
#define INIT_VOLCTL 0x7F /* Nominel "volume" */
#define INIT_VOLCTL_Ton 0x98 /* Activate power-IC timer */
#define INIT_GSSET 0x00

//******************************************************************************
// CLO
#define LCD_INTERFACE_BASE_ADDR  0xFFFFA000  // LCD interface on LOCOSTO !!
#define REG16(_addr)    (*(volatile unsigned short *)(_addr))		

//Register Offset
//---------------
#define            LCD_INTERFACE_CNTL_REG_OFFSET                           0x00
#define            LCD_INTERFACE_LCD_CNTL_REG_OFFSET                       0x02
#define            LCD_INTERFACE_LCD_IF_STS_REG_OFFSET                     0x04
#define            LCD_INTERFACE_WR_FIFO_OFFSET                            0x06
#define            LCD_INTERFACE_RD_REG_OFFSET                             0x08
									
//LCD_INTERFACE_CNTL_REG						
//----------------------				
#define LCD_INTERFACE_CNTL_REG REG16(LCD_INTERFACE_BASE_ADDR+LCD_INTERFACE_CNTL_REG_OFFSET)
#define LCD_INTERFACE_LCD_CNTL_REG REG16(LCD_INTERFACE_BASE_ADDR+LCD_INTERFACE_LCD_CNTL_REG_OFFSET)

#define SDRAM_IMAGE 1
#define IMIF_IMAGE 2

//#define FRAME_BUFFER_SDRAM_ADDRESS	0x10600000 //Located in the 6th MB of the 32MB availble (after ETM-Lite code and before the GSM code)
//#define FRAME_BUFFER_IMIF_ADDRESS	0x20000000 //Located in internal RAM
#define DMA_NUMBER_OF_CHANNEL  9

// Debug latch
#define DEBUG_LATCH (*(volatile unsigned short*)0x2400000) // D-Sample & E-Sample
#define DEBUG_LATCH_HI (*(volatile unsigned char*)0x2400001) // D-Sample & E-Sample
#define DEBUG_LATCH_LO (*(volatile unsigned char*)0x2400000) // D-Sample & E-Sample

// Configuration latch
#define CTRL_MCLK_VPP (*(volatile unsigned char*)0x2780000) // control the 13/14MHz and DC-DC

typedef enum {
  CLK_DIV_BY_1=0,
  CLK_DIV_BY_2=1,
  CLK_DIV_BY_4=2,
  CLK_DIV_BY_8=3
} T_CKCTL_DIV;
#if 0

#define BIT unsigned int
typedef struct
{
  BIT    ChannelNumb:4;

   //DMA_CSDP
   //--------------------
  BIT    TypeSize:2;

  BIT    SrcPort:3;
  BIT    DestPort:3;

  BIT    SrcPack:1;
  BIT    DestPack:1;

  BIT    SrcBurst:2;
  BIT    DestBurst:2;

   //DMA_CCR
   //-----------------------
  BIT    SyncNumb:5;
  BIT    SyncPr:1;
  BIT    EventSync:1;//fs
  BIT    Priority:1;
  BIT    Enable:1;
  BIT    Autoinit:1;
  BIT    Repeat:1;
  BIT    Fifofush:1;

  BIT    SrcAddressMode:2;
  BIT    DestAddressMode:2;

    //DMA_CICR
    //----------------------
  BIT    TimeoutIntEnable:1;
  BIT    DropIntEnable:1;
  BIT    HalfFrameIntEnable:1;
  BIT    FrameIntEnable:1;
  BIT    LastFrameIntEnable:1;
  BIT    BlockIntEnable:1;

    //DMA_CSCR
    //----------------------
  BIT    TimeoutInt:1;
  BIT    DropInt:1;
  BIT    HalfFrameInt:1;
  BIT    FrameInt:1;
  BIT    LastFrameInt:1;
  BIT    BlockInt:1;

    //DMA_CSSA L and U
    //----------------------
  UINT32 SrcAdd;

    //DMA_CDSA L and U
    //----------------------
  UINT32 DestAdd;

    //DMA_CEN 
    //----------------------
  UINT16 EltNumber;

    //DMA_CFN 
    //----------------------
  UINT16 FrameNumber;

    //DMA_CEI 
    //----------------------
  UINT16 EltIndex;

    //DMA_CFI 
    //----------------------
  UINT16 FrameIndex;

//-------------------------------------------------------------------
    //Patern use to prepare data to transfert (use for test only)
    //------------------------------------------------------------
  UINT16 Pattern;

}T_CHANNEL_DESCRIPTOR;

typedef struct
{

   //DMA_LCD_CTRL
   //--------------------
  BIT FrameMode:1;
  BIT FrameItIe:1;
  BIT BusErrorItIe:1;
  BIT Frame1ItCond:1;
  BIT Frame2ItCond:1;
  BIT BusErrorItCond:1;
  BIT LcdSrc:1;

   //DMA_LCD_TOP_F1
   //-----------------------
  UINT32    LcdTopF1;

   //DMA_LCD_BOT_F1
   //-----------------------
  UINT32    LcdBotF1;

   //DMA_LCD_TOP_F2
   //-----------------------
  UINT32    LcdTopF2;

   //DMA_LCD_BOT_F2
   //-----------------------
  UINT32    LcdBotF2;
}T_LCD_CHANNEL_DESCRIPTOR;

typedef struct
{

   //Global register
   //--------------------
  BIT Autogating_on:1;
  BIT Free:1;

  T_CHANNEL_DESCRIPTOR     DmaChannel[DMA_NUMBER_OF_CHANNEL];
  T_LCD_CHANNEL_DESCRIPTOR LcdChannel;

}T_DMA_SYSTEM_STRUCT ;

extern UINT8 PixelClockDiv;
#endif

/*=== Private function prototypes ============================================*/

void lcd_off(void);
void lcd_init(void);

void setupPinMuxLCD(void);
void setup_PixelclockFreq(void);
UINT16 lcdGetClockDivider(void);
void init_lcd_ctrl_registers(void);
void EPSON_MD_TFD_LCD_INIT(void);
void MPU_UWIRE_nCS(unsigned char nCS1);
void MPU_UWIRE_SEND(unsigned short DnC, unsigned short Data);
void displayOnOff(unsigned char onOff);
void write_image_1(UINT16 *ptr_dst);
void write_image_2(UINT16 *ptr_dst);
void write_image_3(UINT16 *ptr_dst);
void write_image_4(UINT16 *ptr_dst);
void write_image_5(UINT16 *ptr_dst);
void write_image_6(UINT16 *ptr_dst);
void enable_dma(UINT8 image_src);
void DMA_SetupSystemDma(BOOL WithOrWithoutFree );
void DMA_EnableLcdIntFlags(UINT32 FrameItIe, UINT32 BusErrorItIe);
void DMATEST_LcdConfigPort(UINT32 SrcPort, UINT32 AddTop1, UINT32 AddBot1, UINT32 AddTop2, UINT32 AddBot2);
void DMATEST_LcdFrameMode(UINT32 FrMode);
void DMA_SetupChannel(void);


void r2d_nop_delay(UINT32 ms);


extern const unsigned char INIT_DISCTL[11]; // 12
extern const unsigned char INIT_GCP64[126];
extern const unsigned char INIT_GCP16[15];	// 16
extern const unsigned char INIT_MD_PSET[4];
extern const unsigned char INIT_MD_CSET[4] ;

extern const unsigned char INIT_SD_PSET[4];
extern const unsigned char INIT_SD_CSET[4];

extern const unsigned char INIT_ASCSET[];
extern const unsigned char INIT_SCSTART[2];

void lcd_text_refresh(void);


void LCD_L2D2_Initialization(void);
// void LCD_L2D2_load_img(unsigned char *data,unsigned short stride);
void LCD_L2D2_load_img(unsigned char *data, unsigned short x_size, unsigned short y_size);
void LCD_L2D2_load_img2(unsigned char *data, unsigned short stride);


//******************************************************************************

//BEGIN INC GENERATION
//--------------------------------------

//Register Offset
//-------------------
#define            LCD_INTERFACE_CNTL_REG_OFFSET                           0x00
#define            LCD_INTERFACE_LCD_CNTL_REG_OFFSET                       0x02
#define            LCD_INTERFACE_LCD_IF_STS_REG_OFFSET                     0x04
#define            LCD_INTERFACE_WR_FIFO_OFFSET                            0x06
#define            LCD_INTERFACE_RD_REG_OFFSET                             0x08
									

//LCD_INTERFACE_CNTL_REG						
//-------------------							
#define            LCD_INTERFACE_CNTL_REG                                  REG16(LCD_INTERFACE_BASE_ADDR+LCD_INTERFACE_CNTL_REG_OFFSET)

#define            LCD_INTERFACE_CNTL_REG_N_DUMMY_POS                        14
#define            LCD_INTERFACE_CNTL_REG_N_DUMMY_NUMB                       2
#define            LCD_INTERFACE_CNTL_REG_N_DUMMY_RES_VAL                    0x1
//R/W

#define            LCD_INTERFACE_CNTL_REG_MIN_FRAME_SIZE_POS                 12
#define            LCD_INTERFACE_CNTL_REG_MIN_FRAME_SIZE_NUMB                2
#define            LCD_INTERFACE_CNTL_REG_MIN_FRAME_SIZE_RES_VAL             0x3
//R/W
#define            LCD_INTERFACE_CNTL_REG_SUSPEND_EN_POS                     11
#define            LCD_INTERFACE_CNTL_REG_SUSPEND_EN_NUMB                    1
#define            LCD_INTERFACE_CNTL_REG_SUSPEND_EN_RES_VAL                 0x1
//R/W

#define            LCD_INTERFACE_CNTL_REG_FLIP_BYTES_POS                     10
#define            LCD_INTERFACE_CNTL_REG_FLIP_BYTES_NUMB                    1
#define            LCD_INTERFACE_CNTL_REG_FLIP_BYTES_RES_VAL                 0x0
//R/W

#define            LCD_INTERFACE_CNTL_REG_MODE_POS                           9
#define            LCD_INTERFACE_CNTL_REG_MODE_NUMB                          1
#define            LCD_INTERFACE_CNTL_REG_MODE_RES_VAL                       0x0
//R/W									
									
#define            LCD_INTERFACE_CNTL_REG_DMA_EN_POS                         8
#define            LCD_INTERFACE_CNTL_REG_DMA_EN_NUMB                        1
#define            LCD_INTERFACE_CNTL_REG_DMA_EN_RES_VAL                     0x0
//R/W									
									
#define            LCD_INTERFACE_CNTL_REG_LCD_READ_EVENT_IT_EN_POS           7
#define            LCD_INTERFACE_CNTL_REG_LCD_READ_EVENT_IT_EN_NUMB          1
#define            LCD_INTERFACE_CNTL_REG_LCD_READ_EVENT_IT_EN_RES_VAL       0x0
//R/W									
									
#define            LCD_INTERFACE_CNTL_REG_FIFO_EMPTY_IT_EN_POS               6
#define            LCD_INTERFACE_CNTL_REG_FIFO_EMPTY_IT_EN_NUMB              1
#define            LCD_INTERFACE_CNTL_REG_FIFO_EMPTY_IT_EN_RES_VAL           0x0
//R/W									
									
#define            LCD_INTERFACE_CNTL_REG_RX_CLOCK_DIV_POS                   4
#define            LCD_INTERFACE_CNTL_REG_RX_CLOCK_DIV_NUMB                  2
#define            LCD_INTERFACE_CNTL_REG_RX_CLOCK_DIV_RES_VAL               0x0
//R/W									
									
#define            LCD_INTERFACE_CNTL_REG_TX_CLOCK_DIV_POS                   2
#define            LCD_INTERFACE_CNTL_REG_TX_CLOCK_DIV_NUMB                  2
#define            LCD_INTERFACE_CNTL_REG_TX_CLOCK_DIV_RES_VAL               0x0
//R/W									
									
#define            LCD_INTERFACE_CNTL_REG_CLOCK13_EN_POS                     1
#define            LCD_INTERFACE_CNTL_REG_CLOCK13_EN_NUMB                    1
#define            LCD_INTERFACE_CNTL_REG_CLOCK13_EN_RES_VAL                 0x0
//R/W									
									
#define            LCD_INTERFACE_CNTL_REG_SOFT_NRST_POS                     0
#define            LCD_INTERFACE_CNTL_REG_SOFT_NRST_NUMB                    1
#define            LCD_INTERFACE_CNTL_REG_SOFT_NRST_RES_VAL                 0x1
//R/W									
									
									
//LCD_INTERFACE_LCD_CNTL_REG						
//-------------------							
#define            LCD_INTERFACE_LCD_CNTL_REG                              REG16(LCD_INTERFACE_BASE_ADDR+LCD_INTERFACE_LCD_CNTL_REG_OFFSET)
									
									
#define            LCD_INTERFACE_LCD_CNTL_REG_LCD_NCS1_POS                   4
#define            LCD_INTERFACE_LCD_CNTL_REG_LCD_NCS1_NUMB                  1
#define            LCD_INTERFACE_LCD_CNTL_REG_LCD_NCS1_RES_VAL               0x1
//R/W									
									
#define            LCD_INTERFACE_LCD_CNTL_REG_LCD_NRESET_POS                 3
#define            LCD_INTERFACE_LCD_CNTL_REG_LCD_NRESET_NUMB                1
#define            LCD_INTERFACE_LCD_CNTL_REG_LCD_NRESET_RES_VAL             0x0
//R/W									
									
#define            LCD_INTERFACE_LCD_CNTL_REG_LCD_START_READ_POS             2
#define            LCD_INTERFACE_LCD_CNTL_REG_LCD_START_READ_NUMB            1
#define            LCD_INTERFACE_LCD_CNTL_REG_LCD_START_READ_RES_VAL         0x0
//R/W									
									
#define            LCD_INTERFACE_LCD_CNTL_REG_LCD_RS_POS                     1
#define            LCD_INTERFACE_LCD_CNTL_REG_LCD_RS_NUMB                    1
#define            LCD_INTERFACE_LCD_CNTL_REG_LCD_RS_RES_VAL                 0x0
//R/W									
									
#define            LCD_INTERFACE_LCD_CNTL_REG_LCD_NCS0_POS                   0
#define            LCD_INTERFACE_LCD_CNTL_REG_LCD_NCS0_NUMB                  1
#define            LCD_INTERFACE_LCD_CNTL_REG_LCD_NCS0_RES_VAL               0x1
//R/W									
									
									
//LCD_INTERFACE_LCD_IF_STS_REG						
//-------------------							
#define            LCD_INTERFACE_LCD_IF_STS_REG                            REG16(LCD_INTERFACE_BASE_ADDR+LCD_INTERFACE_LCD_IF_STS_REG_OFFSET)
									
									
#define            LCD_INTERFACE_LCD_IF_STS_REG_LCD_READ_EVENT_STATUS_POS     2
#define            LCD_INTERFACE_LCD_IF_STS_REG_LCD_READ_EVENT_STATUS_NUMB    1
#define            LCD_INTERFACE_LCD_IF_STS_REG_LCD_READ_EVENT_STATUS_RES_VAL 0x0
//R/C									
									
#define            LCD_INTERFACE_LCD_IF_STS_REG_FIFO_FULL_STATUS_POS         1
#define            LCD_INTERFACE_LCD_IF_STS_REG_FIFO_FULL_STATUS_NUMB        1
#define            LCD_INTERFACE_LCD_IF_STS_REG_FIFO_FULL_STATUS_RES_VAL     0x0
//R									
									
#define            LCD_INTERFACE_LCD_IF_STS_REG_FIFO_EMPTY_STATUS_POS        0
#define            LCD_INTERFACE_LCD_IF_STS_REG_FIFO_EMPTY_STATUS_NUMB       1
#define            LCD_INTERFACE_LCD_IF_STS_REG_FIFO_EMPTY_STATUS_RES_VAL     0x1
//R									
									
									
//LCD_INTERFACE_WR_FIFO							
//-------------------							
#define            LCD_INTERFACE_WR_FIFO                                   REG16(LCD_INTERFACE_BASE_ADDR+LCD_INTERFACE_WR_FIFO_OFFSET)
									
									
#define            LCD_INTERFACE_WR_FIFO_DATA_TX_POS                         0
#define            LCD_INTERFACE_WR_FIFO_DATA_TX_NUMB                        16
#define            LCD_INTERFACE_WR_FIFO_DATA_TX_RES_VAL                     0x0000
//W									
									
									
//LCD_INTERFACE_RD_REG							
//-------------------							
#define            LCD_INTERFACE_RD_REG                                    REG16(LCD_INTERFACE_BASE_ADDR+LCD_INTERFACE_RD_REG_OFFSET)
									
									
#define            LCD_INTERFACE_RD_REG_DATA_RX_POS                          0
#define            LCD_INTERFACE_RD_REG_DATA_RX_NUMB                         16
#define            LCD_INTERFACE_RD_REG_DATA_RX_RES_VAL                      None
//R

//END INC GENERATION
//--------------------------------------

//------------------------------------------------------------------
//
// LCD functions prototypes, typedef and macros
//
//------------------------------------------------------------------

//=====================================================
//
// Control Register (CNTL_REG)
//
//=====================================================
#define LCD_NRESET   0

typedef enum
{
  LCD_CLOCK13_DIS = 0,
  LCD_CLOCK13_EN  = 1 
} LCD_Clock13MHz_t;

typedef enum
{
  LCD_TX_CLOCK_DIV8 = 0,
  LCD_TX_CLOCK_DIV4 = 1,
  LCD_TX_CLOCK_DIV2 = 2,
  LCD_TX_CLOCK_DIV1 = 3
} LCD_TxClockDiv_t;

typedef enum
{
  LCD_RX_CLOCK_DIV8 = 0,
  LCD_RX_CLOCK_DIV4 = 1,
  LCD_RX_CLOCK_DIV2 = 2,
  LCD_RX_CLOCK_DIV1 = 3
} LCD_RxClockDiv_t;

typedef enum
{
  LCD_FIFO_EMPTY_IT_DIS = 0,
  LCD_FIFO_EMPTY_IT_EN  = 1
} LCD_FifoEmptyIt_t;

typedef enum
{
  LCD_READ_EMPTY_EVENT_IT_DIS = 0,
  LCD_READ_EMPTY_EVENT_IT_EN  = 1
} LCD_LcdReadEventIt_t;

typedef enum
{
  LCD_DMA_DIS = 0,
  LCD_DMA_EN  = 1
} LCD_Dma_t;

typedef enum
{
  LCD_INTERFACE_6800  = 0,
  LCD_INTERFACE_8086  = 1
} LCD_InterfaceType_t;

typedef enum
{
  LCD_WRITE_MSB_FIRST  = 0,
  LCD_WRITE_LSB_FIRST  = 1
} LCD_EndianismConfig_t;

typedef enum
{
  LCD_SUSPEND_DISABLED  = 0,
  LCD_SUSPEND_ENABLED  = 1
} LCD_SuspendConfig_t;

typedef enum
{
  LCD_MIN_FRAME_SIZE_16W  = 0,
  LCD_MIN_FRAME_SIZE_32W  = 1,
  LCD_MIN_FRAME_SIZE_64W  = 2,
  LCD_MIN_FRAME_SIZE_128W = 3
} LCD_DmaFrameSize_t;

typedef enum
{
  LCD_0_DUMMY  = 0,
  LCD_1_DUMMY  = 1,
  LCD_2_DUMMY  = 2
} LCD_DummyConfig_t;


//------------------------------------------------------
// Reset LCD interface
//------------------------------------------------------
void LCD_ResetInterface(void);

// the same with a macro
#define LCD_ResetInterface_M LCD_INTERFACE_CNTL_REG = (LCD_INTERFACE_CNTL_REG & 0xFFFE)

//------------------------------------------------------
// LCD : Clock 13MHz enable
//------------------------------------------------------
void LCD_Clock13MHzEnable(void);

// the same with a macro
#define LCD_Clock13MHzEnable_M LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFFFD) | LCD_CLOCK13_EN);

//------------------------------------------------------
// LCD : Clock 13MHz disable
//------------------------------------------------------
void LCD_Clock13MHzDisable(void);

// the same with a macro
#define LCD_Clock13MHzDisable_M LCD_INTERFACE_CNTL_REG = (LCD_INTERFACE_CNTL_REG & 0xFFFD);

//------------------------------------------------------
// LCD : Clock 13MHz status
//------------------------------------------------------
UINT16 LCD_Clock13MHzStatus(void);

// the same with a macro
#define LCD_Clock13MHzStatus_M ((LCD_INTERFACE_CNTL_REG >> LCD_INTERFACE_CNTL_REG_CLOCK13_EN_POS) & 0x0001)

//------------------------------------------------------
// LCD : TX clock div
//------------------------------------------------------
void LCD_TXClockDiv(UINT16 value);

// the same with a macro
#define LCD_TXClockDiv_M(value) SetField16(LCD_INTERFACE_CNTL_REG, TX_CLOCK_DIV, (value))

//------------------------------------------------------
// LCD : TX clock div status
//------------------------------------------------------
UINT16 LCD_TXClockDivStatus(void);

// the same with a macro
#define LCD_TXClockDivStatus_M GetField16(LCD_INTERFACE_CNTL_REG, TX_CLOCK_DIV)

//------------------------------------------------------
// LCD : FIFO empty interrupt enable
//------------------------------------------------------
void LCD_FifoEmptyItEnable(void);

// the same with a macro
#define LCD_FifoEmptyItEnable_M LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFFBF) | 0x0040)

//------------------------------------------------------
// LCD : FIFO empty interrupt disable
//------------------------------------------------------
void LCD_FifoEmptyItDisable(void);

// the same with a macro
#define LCD_FifoEmptyItDisable_M LCD_INTERFACE_CNTL_REG = (LCD_INTERFACE_CNTL_REG & 0xFFBF)

//------------------------------------------------------
// LCD : FIFO empty interrupt status
//------------------------------------------------------
UINT16 LCD_FifoEmptyItStatus(void);

// the same with a macro
#define LCD_FifoEmptyItStatus_M ((LCD_INTERFACE_CNTL_REG >> LCD_INTERFACE_CNTL_REG_FIFO_EMPTY_IT_EN_POS) & 0x0001)

//------------------------------------------------------
// LCD : Read event interrupt enable
//------------------------------------------------------
void LCD_ReadEventItEnable(void);

// the same with a macro
#define LCD_ReadEventItEnable_M LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFF7F) | 0x0080)

//------------------------------------------------------
// LCD : Read event interrupt disable
//------------------------------------------------------
void LCD_ReadEventItDisable(void);

// the same with a macro
#define LCD_ReadEventItDisable_M LCD_INTERFACE_CNTL_REG = (LCD_INTERFACE_CNTL_REG & 0xFF7F)

//------------------------------------------------------
// LCD : Read event interrupt status
//------------------------------------------------------
UINT16 LCD_ReadEventItStatus(void);
// the same with a macro
#define LCD_ReadEventItStatus_M ((LCD_INTERFACE_CNTL_REG >> LCD_INTERFACE_CNTL_REG_LCD_READ_EVENT_IT_EN_POS) & 0x0001)


//------------------------------------------------------
// LCD : DMA enable
//------------------------------------------------------
void LCD_DMAEnable(void);

// the same with a macro
#define LCD_DMAEnable_M   LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFEFF) | 0x0100)

//------------------------------------------------------
// LCD : DMA disable
//------------------------------------------------------
void LCD_DMADisable(void);

// the same with a macro
#define LCD_DMADisable_M   LCD_INTERFACE_CNTL_REG = (LCD_INTERFACE_CNTL_REG & 0xFEFF)

//------------------------------------------------------
// LCD : DMA status
//------------------------------------------------------
UINT16 LCD_DMAStatus(void);

// the same with a macro
#define LCD_DMAStatus_M ((LCD_INTERFACE_CNTL_REG >> LCD_INTERFACE_CNTL_REG_DMA_EN_POS) & 0x0001)

//------------------------------------------------------
// LCD : 6800 interface type selected
//------------------------------------------------------
void LCD_6800InterfaceTypeSelected(void);

// the same with a macro
#define LCD_6800InterfaceTypeSelected_M LCD_INTERFACE_CNTL_REG = (LCD_INTERFACE_CNTL_REG & 0xFDFF)

//------------------------------------------------------
// LCD : 8086 interface type selected
//------------------------------------------------------
void LCD_8086InterfaceTypeSelected(void);

// the same with a macro
#define LCD_8086InterfaceTypeSelected_M LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFDFF) | 0x0200)

//------------------------------------------------------
// LCD : 6800/8086 mode status
//------------------------------------------------------
UINT16 LCD_InterfaceTypeStatus(void);

// the same with a macro
#define LCD_InterfaceTypeStatus_M ((LCD_INTERFACE_CNTL_REG >> LCD_INTERFACE_CNTL_REG_MODE_POS) & 0x0001)

//------------------------------------------------------
// LCD : Write MSB First
//------------------------------------------------------
void LCD_WriteMSBFirst(void);

// the same with a macro
#define LCD_WriteMSBFirst_M LCD_INTERFACE_CNTL_REG = (LCD_INTERFACE_CNTL_REG & 0xFBFF)

//------------------------------------------------------
// LCD : Write LSB First
//------------------------------------------------------
void LCD_WriteLSBFirst(void);

// the same with a macro
#define LCD_WriteLSBFirst_M LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFBFF) | 0x0400)

//------------------------------------------------------
// LCD : Endianism Status
//------------------------------------------------------
UINT16 LCD_EndianismStatus(void);

// the same with a macro
#define LCD_EndianismStatus_M ((LCD_INTERFACE_CNTL_REG >> LCD_INTERFACE_CNTL_REG_FLIP_BYTES_POS) & 0x0001)

//------------------------------------------------------
// LCD : Tx never suspended
//------------------------------------------------------
void LCD_TxNotSuspended(void);

// the same with a macro
#define LCD_TxNotSuspended_M LCD_INTERFACE_CNTL_REG = (LCD_INTERFACE_CNTL_REG & 0xF7FF)


//------------------------------------------------------
// LCD : Allow Tx suspend
//------------------------------------------------------
 void LCD_AllowTxSuspended(void);

// the same with a macro
#define LCD_AllowTxSuspended_M LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xF7FF) | 0x0800)


//------------------------------------------------------
// LCD : Suspend status
//------------------------------------------------------
UINT16 LCD_TxSuspendStatus(void);

// the same with a macro
#define LCD_TxSuspendStatus_M ((LCD_INTERFACE_CNTL_REG >> LCD_INTERFACE_CNTL_REG_SUSPEND_EN_POS) & 0x0001)

//------------------------------------------------------
// LCD : DMA frame size set
//------------------------------------------------------
void LCD_DmaFrameSizeSet(UINT16 value);

// the same with a macro
#define LCD_DmaFrameSizeSet_M(value) SetField16(LCD_INTERFACE_CNTL_REG, MIN_FRAME_SIZE, (value))

//------------------------------------------------------
// LCD : DMA frame size status
//------------------------------------------------------
UINT16 LCD_DmaFrameSizeStatus(void);

// the same with a macro
#define LCD_DmaFrameSizeStatus_M GetField16(LCD_INTERFACE_CNTL_REG, MIN_FRAME_SIZE)



void LCD_0DummyWriteCycle(void);



//=======================================================
//
// LCD Controler Register (LCD_CNTL_REG)
//
//=======================================================

typedef enum
{
  LCD_NCS0_SEL    = 0,
  LCD_NCS0_UNSEL  = 1
} LCD_nCS0_t;

//       THE VALUE FOR SELECTING INSTRUCTION/DISPLAY
//       CONFIGURATION DEPENDS ON THE LCD CONTROLLER
							

//typedef enum
//{
// LCD_DISPLAY     = 0,        // For LH155N LCD
// LCD_INSTRUCTION = 1         // controller type
//} LCD_DataTypeSelection_t;

typedef enum
{
  LCD_INSTRUCTION = 0,      // For KS0741 LCD
  LCD_DISPLAY     = 1       // controller type
} LCD_DataTypeSelection_t;

#define LCD_START_READ    1

#define LCD_NRESET_CTRL   0


typedef enum
{
  LCD_NCS1_SEL    = 0,
  LCD_NCS1_UNSEL  = 1
} LCD_nCS1_t;




//------------------------------------------------------
// LCD : Controller 0 selected
//------------------------------------------------------
void LCD_Controller0Selected(void);

// the same with a macro
#define LCD_Controller0Selected_M LCD_INTERFACE_LCD_CNTL_REG = (LCD_INTERFACE_LCD_CNTL_REG & 0xFFFE)

//------------------------------------------------------
// LCD : Controller 0 unselected
//------------------------------------------------------
void LCD_Controller0UnSelected(void);

// the same with a macro
#define LCD_Controller0UnSelected_M LCD_INTERFACE_LCD_CNTL_REG = ((LCD_INTERFACE_LCD_CNTL_REG & 0xFFFE) | 0x0001)

//------------------------------------------------------
// LCD : Controller 0 status
//------------------------------------------------------
UINT16 LCD_Controller0Status(void);

// the same with a macro
#define LCD_Controller0Status_M (LCD_INTERFACE_LCD_CNTL_REG & 0x0001)

//------------------------------------------------------
// LCD : Instruction/Display data type selection
//------------------------------------------------------
void LCD_DataTypeSelection(LCD_DataTypeSelection_t value);

#define SetField16(_reg,_pos,_value)   (REG16((_reg)) |= (_value)<< (_reg ##_ ##_pos ##_POS ))


#define BSPUTIL_BITUTIL_BIT_FIELD_MASK16( _fieldWidth, _fieldOffset )              \
(                                                                                  \
    ( ( 0xFFFF >> ( ( 16 - (_fieldWidth) ) & 0x0F ) ) << ((_fieldOffset) & 0x0F) ) \
)

#define GetField16( _reg_,                        \
                                         _pos_)                    \
(                                                                         \
        (*((volatile UINT16 *)(_reg_))) &                              \
        BSPUTIL_BITUTIL_BIT_FIELD_MASK16( (_reg ##_ ##_pos ##_POS), (_reg ##_ ##_pos ##_NUMB) ) \
    ) >> (_pos_)                                                   \
)



// the same with a macro
#define LCD_DataTypeSelection_M(value) SetField16(LCD_INTERFACE_LCD_CNTL_REG, LCD_RS, (value))

//------------------------------------------------------
// LCD : Instruction/Display data type status
//------------------------------------------------------
UINT16 LCD_DataTypeSelectionStatus(void);

// the same with a macro
#define LCD_DataTypeSelectionStatus_M ((LCD_INTERFACE_LCD_CNTL_REG >> LCD_INTERFACE_LCD_CNTL_REG_LCD_RS_POS) & 0x0001)

//------------------------------------------------------
// LCD : Start read
//------------------------------------------------------
void LCD_StartRead(void);

// the same with a macro
#define LCD_StartRead_M LCD_INTERFACE_LCD_CNTL_REG = (LCD_INTERFACE_LCD_CNTL_REG | 0x0004)

//------------------------------------------------------
// LCD : Start read status
//------------------------------------------------------
UINT16 LCD_StartReadStatus(void);

// the same with a macro
#define LCD_StartReadStatus_M ((LCD_INTERFACE_LCD_CNTL_REG >> LCD_INTERFACE_LCD_CNTL_REG_LCD_START_READ_POS) & 0x0001)

//------------------------------------------------------
// Reset LCD controller
//------------------------------------------------------
void LCD_ResetController(void);

// the same with a macro
#define LCD_ResetController_M LCD_INTERFACE_LCD_CNTL_REG = (LCD_INTERFACE_LCD_CNTL_REG & 0xFFF7)

//------------------------------------------------------
// End LCD controller reset
//------------------------------------------------------
void LCD_EndResetController(void);

// the same with a macro
#define LCD_EndResetController_M LCD_INTERFACE_LCD_CNTL_REG = (LCD_INTERFACE_LCD_CNTL_REG | 0x0008)

//------------------------------------------------------
// Reset LCD controller status
//------------------------------------------------------
UINT16 LCD_ResetControllerStatus(void);

// the same with a macro
#define LCD_ResetControllerStatus_M ((LCD_INTERFACE_LCD_CNTL_REG >> LCD_INTERFACE_LCD_CNTL_REG_LCD_NRESET_POS) & 0x001)

//------------------------------------------------------
// LCD : Controller 1 selected
//------------------------------------------------------
void LCD_Controller1Selected(void);

// the same with a macro
#define LCD_Controller1Selected_M LCD_INTERFACE_LCD_CNTL_REG = (LCD_INTERFACE_LCD_CNTL_REG & 0xFFEF)

//------------------------------------------------------
// LCD : Controller 1 unselected
//------------------------------------------------------
void LCD_Controller1UnSelected(void);

// the same with a macro
#define LCD_Controller1UnSelected_M LCD_INTERFACE_LCD_CNTL_REG = ((LCD_INTERFACE_LCD_CNTL_REG & 0xFFEF) | 0x0010)

//------------------------------------------------------
// LCD : Controller 1 status
//------------------------------------------------------
UINT16 LCD_Controller1Status(void);

// the same with a macro
//#define LCD_Controller1Status_M GetField16(LCD_INTERFACE_LCD_CNTL_REG, LCD_NCS1)


/*******************************************************
 *
 * LCD interface Status Register (LCD_IF_STS_REG)
 *
 ******************************************************/
typedef enum
{
  LCD_FIFO_NOT_EMPTY = 0,
  LCD_FIFO_IS_EMPTY  = 1
} LCD_FifoEmptyStatus_t;


typedef enum
{
  LCD_FIFO_NOT_FULL = 0,
  LCD_FIFO_IS_FULL  = 2
} LCD_FifoFullStatus_t;

typedef enum
{
  LCD_RECEPTION_ON_GOING   = 0,
  LCD_RECEPTION_COMPLETED  = 4
} LCD_ReadEventStatus_t;

//------------------------------------------------------
// LCD : FIFO empty status
//------------------------------------------------------
LCD_FifoEmptyStatus_t LCD_FifoEmptyStatus(void);

// the same with a macro
#define LCD_FifoEmptyStatus_M (LCD_INTERFACE_LCD_IF_STS_REG & 0x0001)

//------------------------------------------------------
// LCD : FIFO full status
//------------------------------------------------------
LCD_FifoFullStatus_t LCD_FifoFullStatus(void);

// the same with a macro
#define LCD_FifoFullStatus_M (LCD_INTERFACE_LCD_IF_STS_REG & 0x0002)

//------------------------------------------------------
// LCD : Read event status
//------------------------------------------------------
LCD_ReadEventStatus_t LCD_ReadEventStatus(void);

// the same with a macro
#define LCD_ReadEventStatus_M (LCD_INTERFACE_LCD_IF_STS_REG & 0x0004)


//=======================================================
//
// Write data FIFO (WR_FIFO)
//
//=======================================================

//------------------------------------------------------
// LCD : Write data into FIFO
//------------------------------------------------------
void LCD_Write(UINT16 data);

// the same with a macro
#define LCD_Write_M(data)  LCD_INTERFACE_WR_FIFO = (data)
                              

//=======================================================
//
// Read data register (RD_REG)
//
//=======================================================

//------------------------------------------------------
// LCD : Read data from register
//------------------------------------------------------
UINT16 LCD_Read(void);

// the same with a macro
#define LCD_Read_M LCD_INTERFACE_RD_REG


//=======================================================
//
// Library functions and macros
//
//=======================================================

//------------------------------------------------------
// LCD : Read interface Status Register (LCD_IF_STS_REG)
//------------------------------------------------------
typedef enum
{
  LCD_FIFO_EMPTY_MASK = 1,
  LCD_FIFO_FULL_MASK  = 2,
  LCD_READ_EVENT_MASK = 4
} LCD_InterfaceStatusRegister_t;

UINT16 LCD_ReadInterfaceStatusRegister(void);


//------------------------------------------------------
// LCD : Wait FIFO empty
//------------------------------------------------------
//#define WAIT_FIFO_EMPTY  while (GetField16(LCD_INTERFACE_LCD_IF_STS_REG, FIFO_EMPTY_STATUS) == LCD_FIFO_NOT_EMPTY);
#define WAIT_FIFO_EMPTY  F_LCD_IF_WAIT_TX_FIFO_EMPTY

//------------------------------------------------------
// LCD : Set data type
//       Wait FIFO empty before setting type
//------------------------------------------------------
void LCD_DataSetType(LCD_DataTypeSelection_t datatype);

#define LCD_DataSetType_M(datatype) WAIT_FIFO_EMPTY                                                      \
                                    SetField16(LCD_INTERFACE_LCD_CNTL_REG, LCD_RS, (UINT16)(datatype));

//------------------------------------------------------
// LCD : Set type and write date
//       Wait FIFO empty before setting data type
//------------------------------------------------------
void LCD_WriteSetType(LCD_DataTypeSelection_t datatype, UINT16 data);

// the same with a macro
#define LCD_WriteSetType_M(datatype, data) WAIT_FIFO_EMPTY                                                        \
                                           SetField16(LCD_INTERFACE_LCD_CNTL_REG, LCD_RS, (UINT16)(datatype));   \
                                           SetField16(LCD_INTERFACE_WR_FIFO, DATA_TX, (UINT16)(data));



//------------------------------------------------------
// LCD : Write instruction
//       Wait FIFO empty before setting data type
//------------------------------------------------------
void LCD_WriteSetInstruction(UINT16 instruction);

// the same with a macro
#define LCD_WriteSetInstruction_M(instruction) WAIT_FIFO_EMPTY                                                      \
                                               SetField16(LCD_INTERFACE_LCD_CNTL_REG, LCD_RS, LCD_INSTRUCTION);     \
                                               SetField16(LCD_INTERFACE_WR_FIFO, DATA_TX, ((UINT16)(instruction)));

//------------------------------------------------------
// LCD : Write data
//       Wait FIFO empty before setting data type
//------------------------------------------------------
void LCD_WriteSetDisplay(UINT16 data);

// the same with a macro
#define LCD_WriteSetDisplay_M(data) WAIT_FIFO_EMPTY                                                  \
                                    SetField16(LCD_INTERFACE_LCD_CNTL_REG, LCD_RS, LCD_DISPLAY);     \
                                    SetField16(LCD_INTERFACE_WR_FIFO, DATA_TX, ((UINT16)(data)));

//------------------------------------------------------
// LCD : Read register
//------------------------------------------------------
UINT16 LCD_Read_Seq(void);

// the same with a macro
/*
#define LCD_Read_Seq_M(value)   WAIT_FIFO_EMPTY                                                                                    \
                            SetField16(LCD_INTERFACE_LCD_CNTL_REG, LCD_START_READ, 1);                                         \
                            while (GetField16(LCD_INTERFACE_LCD_IF_STS_REG, LCD_READ_EVENT_STATUS) == LCD_RECEPTION_ON_GOING); \
                            (value) = GetField16(LCD_INTERFACE_RD_REG, DATA_RX);
*/

//------------------------------------------------------
// LCD : Reset controller
//------------------------------------------------------
void LCD_ControllerReset(void);

void LCD_SelectController0(void);
// Same with macro
#define LCD_SelectController0_M  LCD_Controller0Selected_M;  \
                                 LCD_Controller1UnSelected_M;

void LCD_SelectController1(void);
// Same with macro
#define LCD_SelectController1_M  LCD_Controller1Selected_M;  \
                                 LCD_Controller0UnSelected_M;

#define LCD_CMD(addr,value) ((UWORD8)(addr) | (UWORD8)((0x0F & ((UWORD8)(value)))))

void read_lcd_id(void);

#endif /* LCD_H */

