#include "Lcd_Transport.h"
#include "Lcd_manager.h"
#include "dma/dma_api.h"
#include "dma/dma_message.h"
#include "sys_types.h"
#include "l1_types.h"
#include "memif/mem.h"
#include <stdio.h>
#include <stdlib.h>
#include "lcd_pwr.h"
#include "rvf/rvf_api.h"
// commands are part of this header file
#include "r2d/LCDs/I_Sample/lcd.h"
#if (LOCOSTO_LITE == 0)
#define R2D_USE_LCD_DMA 1	//Flag for Plus variant using DMA
#endif

#if (LOCOSTO_LITE == 1)
#define LCD_LITE_DMA  1		//Flag for Lite Variant using DMA
#endif

typedef struct
{
T_RVF_MUTEX lcd_framebuf_mutex;
} T_LCD_MUTEX;
T_LCD_MUTEX lcd_mutex_ptr;

#ifdef R2D_USE_LCD_DMA
#include "lcd_dma.h"
T_RV_RETURN r2d_dma_return_path;
volatile UINT8 r2d_lcd_dma_ok=1;


T_DMA_CHANNEL r2d_lcd_dma_channel;
T_DMA_CHANNEL_PARAMETERS r2d_lcd_dma_channel_info;
UINT32 r2d_lcd_dma_bytes_to_transfer= 0;

volatile T_R2D_LCD_DMA_STATE r2d_lcd_dma_state = R2D_LCD_DMA_UNINITILISED;

void r2d_dma_callback(void *response);

static E_LCD_IF_CALLBACK_RET f_lcd_if_dummy_callback (void);

extern T_RVF_ADDR_ID lcd_addr_id;

#endif //R2D_USE_LCD_DMA

#ifdef LCD_LITE_DMA
#include "nucleus.h"
#include "dma/sys_dma.h"

T_DMA_TYPE_CHANNEL_PARAMETER lcd_dma_lite_channel_info;
UINT32 lcd_dma_lite_bytes_to_transfer = 0;
void lcd_dma_lite_callback(SYS_UWORD16);
static E_LCD_IF_CALLBACK_RET f_lcd_if_dummy_callback (void);
extern T_RVF_ADDR_ID lcd_addr_id;
STATUS status_ret;
static NU_HISR LCD_DMA_HISR;
void LCD_DMA_HISREntry(void);

#endif //LCD_LITE_DMA


extern lcd_configparams lcd_config_params;
UINT16 *tab_temp;

void f_lcd_if_set_cs_and_data_type( E_LCD_IF_CS d_cs,
                                              E_LCD_IF_DATA_TYPE d_type,
                                              E_LCD_IF_DATA_ACCESS d_access );


E_LCD_IF_RET f_lcd_if_dma_disable( E_LCD_IF_CS         d_cs,
                                     T_LCD_IF_CALLBACK   pf_callback_sts );

void f_lcd_if_dma_enable( E_LCD_IF_CS        d_cs,
                            E_LCD_IF_FRAME_SZ  d_min_frame_sz,
                            E_LCD_IF_DATA_TYPE d_type );

T_LCD_IF_INTERNAL d_lcd_if_internal;

void r2d_lcd_power_on(void);
void r2d_lcd_power_off(void);

void r2d_nop_delay(UWORD32 ms)
{
		unsigned short a;
		while (ms-- > 0) for (a=0;a<2000;a++) __asm(" nop"); // Increased from 1000 to 2000 (JP, TI-DK)
}


void write_lcd_index_data(int index, int data)
{
	LCD_WriteSetInstruction(index);
    	r2d_nop_delay(1);
	LCD_WriteSetDisplay(data);
}

void gate_driver_serial_transfer(int index)
{
	LCD_WriteSetInstruction(0x0a); // Gate driver serial transfer
	r2d_nop_delay(1);
	LCD_WriteSetDisplay(0x0100 | index);
	r2d_nop_delay(20);
}

void read_lcd_id()
{
	LCD_WriteSetInstruction(0);
	r2d_nop_delay(1);
       LCD_DataSetType_M(LCD_DISPLAY); // select data not index
}
void  LCD_enable_lcd_mux()
{
	(*(volatile SYS_UWORD16*) CONF_LCD_CAM_NAND)|=0x3;
}


//------------------------------------------------------
// LCD : Write display data
//       Wait FIFO empty before setting data type
//------------------------------------------------------
void LCD_WriteSetDisplay(SYS_UWORD16 data)
{
  WAIT_FIFO_EMPTY;
  LCD_DataTypeSelection(LCD_DISPLAY);
  LCD_Write(data);
}

//------------------------------------------------------
// LCD : Write instruction data
//       Wait FIFO empty before setting data type
//------------------------------------------------------
void LCD_WriteSetInstruction(SYS_UWORD16 instruction)
{
  WAIT_FIFO_EMPTY;
  LCD_DataTypeSelection(LCD_INSTRUCTION);
  LCD_Write(instruction);
}

//------------------------------------------------------
// LCD : Write data into FIFO
//------------------------------------------------------
void LCD_Write(SYS_UWORD16 data)
{
( *(volatile SYS_UWORD16* ) C_LCD_IF_WR_FIFO) = data;
//  SetField16(LCD_INTERFACE_WR_FIFO, DATA_TX, data);
}


//------------------------------------------------------
// LCD : Instruction/Display data type selection
//------------------------------------------------------
void LCD_DataTypeSelection(LCD_DataTypeSelection_t value)
{
if (value)
  (*(volatile SYS_UWORD16*) C_LCD_IF_LCD_CNTL_REG)|=value<<C_LCD_IF_LCD_CNTL_REG_LCD_RS_POS;
else
  (*(volatile SYS_UWORD16*) C_LCD_IF_LCD_CNTL_REG)&=(~(1<<C_LCD_IF_LCD_CNTL_REG_LCD_RS_POS));
}

//------------------------------------------------------
// Reset LCD controller
//------------------------------------------------------
void LCD_ResetController(void)
{
  LCD_INTERFACE_LCD_CNTL_REG = (LCD_INTERFACE_LCD_CNTL_REG & 0xFFF7);
}

//------------------------------------------------------
// End LCD controller reset
//------------------------------------------------------
void LCD_EndResetController(void)
{
  LCD_INTERFACE_LCD_CNTL_REG = (LCD_INTERFACE_LCD_CNTL_REG | 0x0008);
}

//------------------------------------------------------
// Reset LCD interface
//------------------------------------------------------
void LCD_ResetInterface(void)
{
  LCD_INTERFACE_CNTL_REG = (LCD_INTERFACE_CNTL_REG & 0xFFFE);
  /* Wait the rest completion */
  do
  {}
  while ((LCD_INTERFACE_CNTL_REG & 0x1) == 0);
}

//------------------------------------------------------
// LCD : Clock 13MHz enable
//------------------------------------------------------
void LCD_Clock13MHzEnable(void)
{
  LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFFFD) | (LCD_CLOCK13_EN << LCD_INTERFACE_CNTL_REG_CLOCK13_EN_POS));
}

//------------------------------------------------------
// LCD : TX clock div
//------------------------------------------------------
void LCD_TXClockDiv(SYS_UWORD16 value)
{
//  SetField16(LCD_INTERFACE_CNTL_REG, TX_CLOCK_DIV, (value));
	 LCD_INTERFACE_CNTL_REG |= (value <<LCD_INTERFACE_CNTL_REG_TX_CLOCK_DIV_POS);
}

//------------------------------------------------------
// LCD : FIFO empty interrupt disable
//------------------------------------------------------
void LCD_FifoEmptyItDisable(void)
{
  LCD_INTERFACE_CNTL_REG = (LCD_INTERFACE_CNTL_REG & 0xFFBF);
}

//------------------------------------------------------
// LCD : Read event interrupt disable
//------------------------------------------------------
void LCD_ReadEventItDisable(void)
{
  LCD_INTERFACE_CNTL_REG = (LCD_INTERFACE_CNTL_REG & 0xFF7F);
}

//------------------------------------------------------
// LCD : DMA disable
//------------------------------------------------------
void LCD_DMADisable(void)
{
  LCD_INTERFACE_CNTL_REG = (LCD_INTERFACE_CNTL_REG & 0xFEFF);
}

//------------------------------------------------------
// LCD : 8086 interface type selected
//------------------------------------------------------
void LCD_8086InterfaceTypeSelected(void)
{
  LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFDFF) | 0x0200);
}

//------------------------------------------------------
// LCD : 6800 interface type selected
//------------------------------------------------------
void LCD_6800InterfaceTypeSelected(void)
{
  LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFDFF) & ~0x0200);
}

//------------------------------------------------------
// LCD : Write MSB First
//------------------------------------------------------
#if (VIRTIO==0)
void LCD_WriteMSBFirst(void)
{
//  LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFBFF) | 0x0400);
  LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFBFF) & ~0x0400);
}

//------------------------------------------------------
// LCD : Write MSB First
//------------------------------------------------------
void LCD_WriteLSBFirst(void)
{
  LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFBFF) | 0x0400);
}
#else
void LCD_WriteMSBFirst(void)
{
  LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFBFF) | 0x0400);
}
#endif

//------------------------------------------------------
// LCD : Tx never suspended
//------------------------------------------------------
void LCD_TxNotSuspended(void)
{
  LCD_INTERFACE_CNTL_REG = (LCD_INTERFACE_CNTL_REG & 0xF7FF);
}

//------------------------------------------------------
// LCD : 0 dummy write cycle
//------------------------------------------------------
void LCD_0DummyWriteCycle(void)
{
  LCD_INTERFACE_CNTL_REG = (LCD_INTERFACE_CNTL_REG & 0xBFFF);
}

/*============================================================================*/
/*                            Public Functions								  */
/*============================================================================*/
/*!
 * @function lcd_init
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to initialize the LCD manager.It also initialises the
 *   LCD interface and the LCD controller.
 *
 * <b> Context </b><br>
 *   This must be called before any other display services and functions
 *
 *  @result <br>
 *     The LCD manager will be initialized.
 *
 */

 ELCD_PAR_RET lcd_parallel_config(  )
 {
	UINT8 i,c;
	//LCD_enable_lcd_mux();
    /* Initialise LCD interface */
    LCD_ResetInterface(); /* Reset LCD interface */
    LCD_Clock13MHzEnable();   /* Enable clock of the interface */
    LCD_TXClockDiv(1);      /* Division factor to 1 (13 MHz) */
    LCD_FifoEmptyItDisable(); /* Disable all IT */
    LCD_ReadEventItDisable();
    LCD_DMADisable();     /* Disable DMA */
#if (VIRTIO==0)
    LCD_8086InterfaceTypeSelected();    /* Select 8086 like LCD */
#else
	LCD_6800InterfaceTypeSelected();
#endif
    LCD_WriteMSBFirst();        /* Write MSB first */
    LCD_TxNotSuspended();       /* Transmission not suspended */
    LCD_0DummyWriteCycle();       /* 0 dummy write cycle */



    LCD_ResetController();
    r2d_nop_delay(20);
    LCD_EndResetController();
    LCD_SelectController0_M;         // Select controller 0
    read_lcd_id();
    write_lcd_index_data(0x00, 0x0001); // start oscilation
    r2d_nop_delay(20);
    write_lcd_index_data(0x01, 0x011b); // Driver output control
    write_lcd_index_data(0x02, 0x0700); // Driving waveform
    write_lcd_index_data(0x03, 0x1030); // Entry mode
    write_lcd_index_data(0x04, 0x0000); // Compare register
    write_lcd_index_data(0x05, 0x0000); // Compare register
    write_lcd_index_data(0x07, 0x0000); // Display control 1 (OFF)
    write_lcd_index_data(0x08, 0x0503); // Display control 2
    write_lcd_index_data(0x0a, 0x0000); // Gate driver
    write_lcd_index_data(0x0b, 0x400c); // Frame freq
    write_lcd_index_data(0x0c, 0x0000); // External if control
    write_lcd_index_data(0x0d, 0x0700); // LTPS I/F

    write_lcd_index_data(0x40, 0x0000); // Gate scan position
    write_lcd_index_data(0x41, 0x0000); // Vertical scroll
    write_lcd_index_data(0x42, 0xdb00); // 1st screen driving pos
    write_lcd_index_data(0x43, 0x0000); // 2nd screen driving pos
    write_lcd_index_data(0x44, 0xaf00); // Horiz RAM address
    write_lcd_index_data(0x45, 0xdb00); // Vert RAM address

	write_lcd_index_data(0x21, 0x0000); // RAM address set
       write_lcd_index_data(0x23, 0x0000); // RAM write mask
       write_lcd_index_data(0x24, 0x0000); // RAM write mask

       write_lcd_index_data(0x30, 0x0000); // Gamma
       write_lcd_index_data(0x31, 0x0501); // Gamma
       write_lcd_index_data(0x32, 0x0307); // Gamma
       write_lcd_index_data(0x33, 0x0400); // Gamma
       write_lcd_index_data(0x34, 0x0004); // Gamma
       write_lcd_index_data(0x35, 0x0504); // Gamma
       write_lcd_index_data(0x36, 0x0605); // Gamma
        write_lcd_index_data(0x37, 0x0004); // Gamma


    // Power ON sequence:
        write_lcd_index_data(0x11, 0x8805); // Power control 2
        write_lcd_index_data(0x12, 0x0004); // Power control 3
        write_lcd_index_data(0x13, 0x0601); // Power control 4
        write_lcd_index_data(0x14, 0x161f); // Power control 5
        gate_driver_serial_transfer(1);
        gate_driver_serial_transfer(2);
        write_lcd_index_data(0x10, 0x2064); // Power control 1
        gate_driver_serial_transfer(0);
        write_lcd_index_data(0x14, 0x361f); // Power control 5
        gate_driver_serial_transfer(0);
        write_lcd_index_data(0x01, 0x011b); // Driver output control
        write_lcd_index_data(0x40, 0x0000); // Gate scan position
        gate_driver_serial_transfer(6);
        write_lcd_index_data(0x02, 0x0700); // Driving waveform
        gate_driver_serial_transfer(7);
        write_lcd_index_data(0x13, 0x0611); // Power control 4
        gate_driver_serial_transfer(1);

        write_lcd_index_data(0x21, 0x0000); // RAM address set
        LCD_WriteSetInstruction(0x22);
        LCD_DataTypeSelection(LCD_DISPLAY);
        for (c=0;c < 100;c++)
            for (i=0;i<22;i++)
            {
                // Red, 0x7C00. Blue, 0x001F. Yollow: 0xF800, Cyan: 0x001F
 			  LCD_Write(0x8F00);
              LCD_Write(0x8F00);
              LCD_Write(0x8F00);
              LCD_Write(0x8F00);
              LCD_Write(0x8F00);
              LCD_Write(0x8F00);
              LCD_Write(0x8F00);
            }

        for (;c < 200;c++)
            for (i=0;i<22;i++)
            {
                // Red, 0x7C00. Blue, 0x001F. Yollow: 0xF800, Cyan: 0x001F
              LCD_Write(0x700E);
              LCD_Write(0x700E);
              LCD_Write(0x700E);
              LCD_Write(0x700E);
              LCD_Write(0x700E);
              LCD_Write(0x700E);
              LCD_Write(0x700E);
              LCD_Write(0x700E);
            }

        for (;c < 220;c++)
            for (i=0;i<22;i++)
            {
                // Red, 0x7C00. Blue, 0x001F. Yollow: 0xF800, Cyan: 0x001F
              LCD_Write(0xF1);
              LCD_Write(0xF1);
              LCD_Write(0xF1);    // BLUE
              LCD_Write(0xF1);
              LCD_Write(0xF1);
              LCD_Write(0xF1);
              LCD_Write(0xF1);
              LCD_Write(0xF1);
            }


  // Turn ON sequence
        write_lcd_index_data(0x07, 0x0005); // Display control 1
        r2d_nop_delay(20);
        write_lcd_index_data(0x07, 0x0025); // Display control 1
        r2d_nop_delay(20);
        gate_driver_serial_transfer(0);
        write_lcd_index_data(0x07, 0x0027); // Display control 1
        r2d_nop_delay(20);
        write_lcd_index_data(0x07, 0x0037); // Display control 1
        r2d_nop_delay(20);

        write_lcd_index_data(0x21, 0x0000); // RAM address set
        LCD_WriteSetInstruction(0x22);
        LCD_DataTypeSelection(LCD_DISPLAY);

	rvf_initialize_static_mutex (&lcd_mutex_ptr.lcd_framebuf_mutex);

#ifdef R2D_USE_LCD_DMA

	r2d_dma_return_path.callback_func = r2d_dma_callback;

	dma_reserve_channel (DMA_CHAN_ANY,0,DMA_QUEUE_ENABLE,
		0,r2d_dma_return_path);

	r2d_lcd_dma_channel_info.data_width = DMA_DATA_S16;
	r2d_lcd_dma_channel_info.sync = DMA_SYNC_DEVICE_HW_LCD;
	r2d_lcd_dma_channel_info.hw_priority = DMA_HW_PRIORITY_HIGH;
	r2d_lcd_dma_channel_info.flush = DMA_FLUSH_DISABLED;
	r2d_lcd_dma_channel_info.nmb_frames = 1;
	r2d_lcd_dma_channel_info.nmb_elements = 128;
	r2d_lcd_dma_channel_info.dma_end_notification = DMA_NOTIFICATION;
	r2d_lcd_dma_channel_info.secure = DMA_NOT_SECURED;
	r2d_lcd_dma_channel_info.dma_mode = DMA_MODE_SINGLE;
	r2d_lcd_dma_channel_info.transfer = DMA_MODE_TRANSFER_ENABLE;

	r2d_lcd_dma_channel_info.source_address = 0;
	r2d_lcd_dma_channel_info.source_address_mode = DMA_ADDR_MODE_POST_INC;
	r2d_lcd_dma_channel_info.source_packet = DMA_NOT_PACKED;
	r2d_lcd_dma_channel_info.source_burst = DMA_NO_BURST;

	r2d_lcd_dma_channel_info.destination_address = (UINT32)C_LCD_IF_WR_FIFO;
	r2d_lcd_dma_channel_info.destination_address_mode = DMA_ADDR_MODE_CONSTANT;
	r2d_lcd_dma_channel_info.destination_packet = DMA_NOT_PACKED;
	r2d_lcd_dma_channel_info.destination_burst = DMA_NO_BURST;

#endif /* USE_LCD_DMA */

#ifdef LCD_LITE_DMA

	status_ret = NU_Create_HISR(&LCD_DMA_HISR, "LCD_DMA_HISR", LCD_DMA_HISREntry, 2,
    				HISR_STACK_PRIO2,
				HISR_STACK_PRIO2_SIZE); // lowest prty


	//Allocate Channel
	f_dma_channel_allocation_set(C_DMA_CHANNEL_2, C_DMA_CHANNEL_ARM);

	// Callback Function Registration
	 lcd_dma_lite_channel_info.pf_dma_call_back_address = lcd_dma_lite_callback;

	//Channel Parameters
        lcd_dma_lite_channel_info.d_dma_channel_number = C_DMA_CHANNEL_2;
        lcd_dma_lite_channel_info.d_dma_channel_secured = C_DMA_CHANNEL_NOT_SECURED;
        lcd_dma_lite_channel_info.d_dma_channel_data_type = C_DMA_DATA_S16;

	//Source Parameters
        lcd_dma_lite_channel_info.d_dma_channel_src_port =  C_DMA_EMIF_PORT;
        lcd_dma_lite_channel_info.d_dma_src_channel_packed = C_DMA_CHANNEL_NOT_PACKED;
        lcd_dma_lite_channel_info.d_dma_src_channel_burst_en = C_DMA_CHANNEL_SINGLE;

	//Destination parameters
	 lcd_dma_lite_channel_info.d_dma_channel_dst_port = C_DMA_RHEA_PORT;
        lcd_dma_lite_channel_info.d_dma_dst_channel_packed = C_DMA_CHANNEL_NOT_PACKED;
        lcd_dma_lite_channel_info.d_dma_dst_channel_burst_en = C_DMA_CHANNEL_SINGLE;

	//Channel Parameters
        lcd_dma_lite_channel_info.d_dma_channel_hw_synch = C_DMA_CHANNEL_LCD;
        lcd_dma_lite_channel_info.d_dma_channel_priority = C_DMA_CHANNEL_PRIORITY_HIGH;
        lcd_dma_lite_channel_info.d_dma_channel_auto_init = C_DMA_CHANNEL_AUTO_INIT_OFF;
        lcd_dma_lite_channel_info.d_dma_channel_fifo_flush = C_DMA_CHANNEL_FIFO_FLUSH_OFF;

	//Addressing Mode Settings
        lcd_dma_lite_channel_info.d_dma_src_channel_addr_mode = C_DMA_CHANNEL_ADDR_MODE_POST_INC;
        lcd_dma_lite_channel_info.d_dma_dst_channel_addr_mode = C_DMA_CHANNEL_ADDR_MODE_CONSTANT;

	//Interrupt Settings
        lcd_dma_lite_channel_info.d_dma_channel_it_time_out = C_DMA_CHANNEL_IT_TIME_OUT_OFF;
        lcd_dma_lite_channel_info.d_dma_channel_it_drop = C_DMA_CHANNEL_IT_DROP_OFF;
        lcd_dma_lite_channel_info.d_dma_channel_it_frame = C_DMA_CHANNEL_IT_FRAME_OFF;
        lcd_dma_lite_channel_info.d_dma_channel_it_block = C_DMA_CHANNEL_IT_BLOCK_ON;
        lcd_dma_lite_channel_info.d_dma_channel_it_half_block = C_DMA_CHANNEL_IT_HALF_BLOCK_OFF;

        lcd_dma_lite_channel_info.d_dma_channel_src_address = 0;
        lcd_dma_lite_channel_info.d_dma_channel_dst_address = (UINT32)C_LCD_IF_WR_FIFO;

	lcd_dma_lite_channel_info.d_dma_channel_element_number = 128;
       lcd_dma_lite_channel_info.d_dma_channel_frame_number = 1;

#endif //LCD_LITE_DMA

  return LCD_PAR_RET_OK;

}

  /******************************************************************************
   *
   *  FUNCTION NAME: f_lcd_if_poll_write
   *
   *    LCD Controller write procedure in polling mode.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Type                   IO  Description
   *  ------------   -------------------    --  ---------------------------------
   *  d_cs  		 E_LCD_IF_CS            I   LCD Controller Chip select
   *  p_data         SYS_UWORD16*           I   pointer on data buffer
   *  d_size         SYS_UWORD32            I   data buffe size
   *  d_type         E_LCD_IF_DATA_TYPE     I   Instruction / Data type selector
   *
   * RETURN VALUE: None
   *
   *****************************************************************************/

  void f_lcd_if_poll_write( E_LCD_IF_CS        d_cs,
                            SYS_UWORD16        *p_data,
                            SYS_UWORD32        d_size,
                            E_LCD_IF_DATA_TYPE d_type )
  {

    SYS_UWORD32    d_nb_words_to_copy = d_size;
    SYS_UWORD32    d_nb_words_in_loop = 0;
    SYS_UWORD16    *p_buffer          = p_data;
    SYS_UWORD8     d_data_type        = 0;
    SYS_UWORD8     i                  = 0;

    /* Switch to chip select */
    /* Set data type and access */

	 F_LCD_CAM_NANDFLASH_AQUIRE_BUS;
	 F_LCD_SET_PIN_CONFIG;

    f_lcd_if_set_cs_and_data_type( d_cs, d_type, C_LCD_IF_WRITE );

    /* infinite loop : exit loop when no words to copy and Tx FIFO empty */

    while( C_LCD_IF_INFINITE_LOOP )
	{

      /*  Wait for the LCD Interface Tx FIFO is empty LCD_IF_STS_REG:FIFO_EMPTY_STATUS_BIT */

      F_LCD_IF_WAIT_TX_FIFO_EMPTY;

   	  /* Check if all data have been copied */

      if( d_nb_words_to_copy == 0 )
	  {
		  F_LCD_CAM_NANDFLASH_RELEASE_BUS;
		  return;
	  } // End if

      /* Copy up to C_LCD_IF_TX_FIFO_SIZE words into the TX FIFO. */

	  if( d_nb_words_to_copy >= C_LCD_IF_TX_FIFO_SIZE)
	  {
	    d_nb_words_in_loop = C_LCD_IF_TX_FIFO_SIZE;
      }
	  else
	  {
	    d_nb_words_in_loop = d_nb_words_to_copy;
	  } // End if

      for( i=0; i<d_nb_words_in_loop ;i++ )
	  {
	    F_LCD_IF_WRITE_IN_FIFO( p_buffer[i] );
      } // End for

      /* update d_nb_words_to copy and p_buffer */
      d_nb_words_to_copy -=  d_nb_words_in_loop;
	  p_buffer           +=  d_nb_words_in_loop;

	} // End while

  } /* f_lcd_if_poll_write */

/******************************************************************************
   *
   *  FUNCTION NAME: f_lcd_if_dma_enable
   *
   *    LCD Controller write procedure start in DMA mode.
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Type                   IO  Description
   *  ------------   -------------------    --  ---------------------------------
   *  d_cs  		 E_LCD_IF_CS            I   LCD Controller Chip select
   *  d_min_frame_sz E_LCD_IF_FRAME_SZ      I   LCD Interface Minimum frame size
   *  d_type         E_LCD_IF_DATA_TYPE     I   Instruction / Data type selector
   *
   * RETURN VALUE: None.
   *
   *****************************************************************************/

  void f_lcd_if_dma_enable( E_LCD_IF_CS        d_cs,
                            E_LCD_IF_FRAME_SZ  d_min_frame_sz,
                            E_LCD_IF_DATA_TYPE d_type )
  {

    SYS_UWORD8 d_data_type = 0;

    /* Switch to chip select */
    /* Set data type and access */

	 F_LCD_CAM_NANDFLASH_AQUIRE_BUS;
	 F_LCD_SET_PIN_CONFIG;

    f_lcd_if_set_cs_and_data_type( d_cs, d_type, C_LCD_IF_WRITE );

    /* enables DMA capabilities */

    F_LCD_IF_ENABLE_DMA;

  } /* f_lcd_if_dma_enable */

  /******************************************************************************
   *
   *  FUNCTION NAME: f_lcd_if_dma_disable
   *
   *    LCD Controller write procedure stop in DMA mode.
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Type                   IO  Description
   *  ------------   -------------------    --  ---------------------------------
   *  d_cs  		 E_LCD_IF_CS            I   LCD Controller Chip select
   *  pf_callback_sts T_LCD_IF_CALLBACK     I   Status Callback function pointer
   *
   * RETURN VALUE: E_LCD_IF_RET function status.
   *
   *****************************************************************************/


  E_LCD_IF_RET f_lcd_if_dma_disable( E_LCD_IF_CS         d_cs,
                                     T_LCD_IF_CALLBACK   pf_callback_sts )
  {

    /* Check the callback function argument */

    if( pf_callback_sts == (T_LCD_IF_CALLBACK) NULL )
	  return( C_LCD_IF_RET_ERR );

    /* disable DMA capabilities */

    F_LCD_IF_DISABLE_DMA;

    /* install status callback function */

    d_lcd_if_internal.d_cs_struct[d_cs].d_tx.p_callback      = pf_callback_sts;

    /* set d_interrupt_state to C_LCD_IF_IT_CUST */

    d_lcd_if_internal.d_interrupt_state = C_LCD_IF_IT_CUST;

    /* Enable LCD Interface TX FIFO Empty interrupt */

    F_LCD_IF_ENABLE_TX_FIFO_EMPTY_IT;

	 F_LCD_CAM_NANDFLASH_RELEASE_BUS;

    return( C_LCD_IF_RET_OK );

  } /* f_lcd_if_dma_disable */

  /******************************************************************************
   *
   *  FUNCTION NAME: f_lcd_if_set_cs_and_data_type
   *
   *    Updates CNTL_REG and LCD_CNTL_REG to change LCD Controller addressing.
   *    Updates LCD_CNTL_REG to set data type and read or write access
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Type                   IO  Description
   *  ------------   -------------------    --  ---------------------------------
   *  d_cs  	     E_LCD_IF_CS            I   LCD Controller Chip select
   *  d_type  	     E_LCD_IF_DATA_TYPE     I   data type
   *  d_access       E_LCD_IF_DATA_ACCESS   I   Data access
   *
   * RETURN VALUE: None
   *
   *****************************************************************************/

  void f_lcd_if_set_cs_and_data_type( E_LCD_IF_CS d_cs, E_LCD_IF_DATA_TYPE d_type, E_LCD_IF_DATA_ACCESS d_access )
  {

    SYS_UWORD8 d_data_type;

    /* if the argument equals the current chip select, nothing to do */

    if( d_lcd_if_internal.d_current_cs != d_cs )
	{
      /* What is the chip select value ? */

      if( d_cs == C_LCD_IF_CS_NOT_SELECTED )
	  {
	  	rvf_send_trace("Error in LCD(not valid CS)",strlen("Error in LCD(not valid CS)"), NULL_PARAM, RV_TRACE_LEVEL_ERROR, LCD_USE_ID );
	    /* not a valid CS */
	    return;
	  } // End if

	  /* update current struct pointer */

	  //d_lcd_if_internal.p_current_struct = &d_lcd_if_internal.d_cs_struct[d_cs];


	  /* Update CNTL_REG register and LCD_CNTL_REG to the new chip select */
	  /* The CNTL_REG:CLOCK_EN bit is set to Clock enable */

	  (*(volatile SYS_UWORD16*) C_LCD_IF_CNTL_REG)     = d_lcd_if_internal.d_cs_struct[d_cs].d_cntl_reg;
	  (*(volatile SYS_UWORD16*) C_LCD_IF_LCD_CNTL_REG) = d_lcd_if_internal.d_cs_struct[d_cs].d_lcd_cntl_reg;

      /* update d_current_cs */

      d_lcd_if_internal.d_current_cs = d_cs;

	} // End if

    /* Sets LCD_CNTL_REG:LCD_RS to type defined in argument and start read */

    if( d_type == C_LCD_IF_INSTRUCTION )
    {
      d_data_type = (SYS_UWORD8) d_lcd_if_internal.d_cs_struct[d_cs].d_isll;
    }
    else
    {
      d_data_type = (SYS_UWORD8) ~d_lcd_if_internal.d_cs_struct[d_cs].d_isll;
    } // End if

    if( d_data_type == 0 )
    {
      (*(volatile SYS_UWORD16*) C_LCD_IF_LCD_CNTL_REG) &= ~( 0x01 << C_LCD_IF_LCD_CNTL_REG_LCD_RS_POS );
    }
    else
    {
      (*(volatile SYS_UWORD16*) C_LCD_IF_LCD_CNTL_REG) |= ( 0x01 << C_LCD_IF_LCD_CNTL_REG_LCD_RS_POS );
    } // End if

    // Set access type

    if( d_access == C_LCD_IF_READ )
    {
      (*(volatile SYS_UWORD16*) C_LCD_IF_LCD_CNTL_REG) |=
                  (C_LCD_IF_LCD_CNTL_REG_LCD_START_READ_EN << C_LCD_IF_LCD_CNTL_REG_LCD_START_READ_POS);
    }
    else
    {
      (*(volatile SYS_UWORD16*) C_LCD_IF_LCD_CNTL_REG) &=
                  ~(C_LCD_IF_LCD_CNTL_REG_LCD_START_READ_EN << C_LCD_IF_LCD_CNTL_REG_LCD_START_READ_POS);
    } // End if

  } /* f_lcd_if_set_cs_and_data_type */


void r2d_lcd_power_on(void)
{
}

void r2d_lcd_power_off(void)
{
}

ELCD_PAR_RET lcd_parallel_display(UINT16 *ImageDataPtrPAR)
{



	#ifndef _WINDOWS
      SYS_UWORD16 buf;
      UINT16 v;
      UINT8 nibble1,nibble2;
      UINT16 s_color,colorL,colorR;
      UINT32 i;

	UINT16 ret_val;

#ifdef R2D_USE_LCD_DMA
    UINT16 received_event;
#endif
#ifdef LCD_LITE_DMA
	UINT32 nmb_frames;
	UINT16 received_event;
	UINT32 bytes_transfered ;
#endif

	tab_temp=ImageDataPtrPAR;

    //rvf_send_trace("tab_temp",strlen("tab_temp"),tab_temp[0] ,
    //        RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );

#if (R2D_REFRESH == R2D_HORIZONTAL)
 //   rvf_send_trace("LCD REFRESH",strlen("LCD REFRESH"), NULL_PARAM,
 //           RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );


	ret_val = rvf_lock_mutex(&lcd_mutex_ptr.lcd_framebuf_mutex);
	if(ret_val != RVF_OK)
	{
	 rvf_send_trace("LCD MUTEX LOCK FAILED",strlen("LCD MUTEX LOCK FAILED"), NULL_PARAM,
                RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );
	 return LCD_PAR_RET_FAIL;
	}

	lcd_g_state=LCD_ACTIVE;
	// Enable LCD interface clock after refresh
	LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFFFD) | (LCD_CLOCK13_EN << LCD_INTERFACE_CNTL_REG_CLOCK13_EN_POS));
	 /** Set address */
	 buf = 0x21; // RAM Address
    	f_lcd_if_poll_write(C_LCD_IF_CS0, &buf, 1, C_LCD_IF_INSTRUCTION);

	buf = 0x0000;

    	f_lcd_if_poll_write(C_LCD_IF_CS0, &buf, 1, C_LCD_IF_DISPLAY);

    	buf = 0x22; // RAM Write
    	f_lcd_if_poll_write(C_LCD_IF_CS0, &buf, 1, C_LCD_IF_INSTRUCTION);

    	/* Philips LCD is configured in MSB first mode. So exchane nibbles
       of each byte */
    	for (i=0;i<176*220;i++)
    	{
       	 v= tab_temp[i];
	#if (FRAME_BUFFER_INVERTED==1)
		#warn   "LCD Refresh Getting Compiled for Negated Input Buffer"
        	v=~v;
	#endif //FRAME_BUFFER_INVERTED

	#if (FRAME_BUFFER_ENDIAN==BIG_END) // need to convert to Little endian
       	 colorL=(v&0xFF00)>>8;
	        colorR=(v&0xFF);
       	 v=(colorR<<8)|colorL;
	 #warn "LCD Refresh Getting Compiled for BIG Endian Input Buffer"

	#elif (FRAME_BUFFER_ENDIAN==LITTLE_END) //no need to conver to Little endian
	  /*"LCD Refresh is getting compiled for Little endian Input buffer*/
	#endif //FRAME_BUFFER_ENDIAN

		tab_temp[i]=v;

    	} //End of For loop

#ifdef R2D_USE_LCD_DMA
    	if(r2d_lcd_dma_ok == 1)
    	{
        	UINT32 nmb_frames;

       	 r2d_lcd_dma_bytes_to_transfer = 176 * 220 * sizeof(UINT16);
        	nmb_frames = r2d_lcd_dma_bytes_to_transfer / (C_LCD_IF_TX_FIFO_SIZE * sizeof (UINT16));
        	r2d_lcd_dma_channel_info.source_address = (UINT32)&(tab_temp[0]);
    //    rvf_send_trace("source address 1 ",strlen("source address 1"),r2d_lcd_dma_channel_info.source_address ,
    //            RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );

		r2d_lcd_dma_channel_info.nmb_frames = (nmb_frames ? nmb_frames : 1);
        	r2d_lcd_dma_channel_info.nmb_elements =
            	((C_LCD_IF_TX_FIFO_SIZE * sizeof (UINT16)) < r2d_lcd_dma_bytes_to_transfer)
            	? (C_LCD_IF_TX_FIFO_SIZE ) : r2d_lcd_dma_bytes_to_transfer / 2;
	        dma_set_channel_parameters (r2d_lcd_dma_channel, &r2d_lcd_dma_channel_info);

       	 /* Wait till we get event from dma callback  */
        	for(;;)
	        {
	            received_event = rvf_wait ( EVENT_MASK(RVF_APPL_EVT_1), 0);
	            if(received_event & EVENT_MASK(RVF_APPL_EVT_1))
	            {
	                if(r2d_lcd_dma_state == R2D_LCD_DMA_IDLE
	                        || r2d_lcd_dma_state == R2D_LCD_DMA_ERROR )
	                {

					break;
	                }
	            }
	        }
	   }
    	   else
    	   {
        		rvf_send_trace("LCD DMA_DISABLED",strlen("LCD DMA_DISABLED"), NULL_PARAM,
                	RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );

		        f_lcd_if_poll_write(C_LCD_IF_CS0, &(tab_temp[0]),176 * 220 , C_LCD_IF_DISPLAY);
    	    }
#elif LCD_LITE_DMA //Use Lite DMA


       	 lcd_dma_lite_bytes_to_transfer = 176 * 220 * sizeof(UINT16);
        	nmb_frames = lcd_dma_lite_bytes_to_transfer / (C_LCD_IF_TX_FIFO_SIZE * sizeof (UINT16));

		//Set the Source Address
        	lcd_dma_lite_channel_info.d_dma_channel_src_address = (UINT32)&(tab_temp[0]);

		//Set the Number of Frames and Number of Elements
		lcd_dma_lite_channel_info.d_dma_channel_frame_number = (nmb_frames ? nmb_frames : 1);
        	lcd_dma_lite_channel_info.d_dma_channel_element_number =
            	((C_LCD_IF_TX_FIFO_SIZE * sizeof (UINT16)) < lcd_dma_lite_bytes_to_transfer)
            	? (C_LCD_IF_TX_FIFO_SIZE ) : lcd_dma_lite_bytes_to_transfer / 2;


		//Set the Channel Parameters
	       f_dma_channel_parameter_set(&lcd_dma_lite_channel_info);
		f_dma_channel_enable(C_DMA_CHANNEL_2);
		//Enable DMA
	       f_lcd_if_dma_enable(C_LCD_IF_CS0,0,C_LCD_IF_DISPLAY);
		 /* Wait till we get event from dma callback  */
        	for(;;)
	        {
	            received_event = rvf_wait ( EVENT_MASK(RVF_APPL_EVT_1), 0);
	            if(received_event & EVENT_MASK(RVF_APPL_EVT_1))
	            {
				bytes_transfered =  ( lcd_dma_lite_channel_info.d_dma_channel_element_number
                        					* lcd_dma_lite_channel_info.d_dma_channel_frame_number
                        					* 2);
				f_lcd_if_dma_disable(C_LCD_IF_CS0,f_lcd_if_dummy_callback);
				lcd_dma_lite_bytes_to_transfer -= bytes_transfered;

      //      rvf_send_trace("LCD DMA CALLBACK", strlen("LCD DMA CALLBACK"), NULL_PARAM,
       //             RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );

  				if(lcd_dma_lite_bytes_to_transfer > 0)
       			{
        				lcd_dma_lite_channel_info.d_dma_channel_src_address= ((UINT32)&(tab_temp[0])) + bytes_transfered;
					lcd_dma_lite_channel_info.d_dma_channel_frame_number = 1;
              			lcd_dma_lite_channel_info.d_dma_channel_element_number =  lcd_dma_lite_bytes_to_transfer / 2 ;

              			//Set the Channel Parameters
	       			f_dma_channel_parameter_set(&lcd_dma_lite_channel_info);
					f_dma_channel_enable(C_DMA_CHANNEL_2);
					//Enable DMA
				       f_lcd_if_dma_enable(C_LCD_IF_CS0,0,C_LCD_IF_DISPLAY);

  				}

				else
				{
      //          rvf_send_trace("LCD DMA DONE", strlen("LCD DMA DONE"), NULL_PARAM,
      //                  RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );
				       break;
       			}


	             }
	         } // End of For loop



#else // !USE_LCD_DMA

    f_lcd_if_poll_write(C_LCD_IF_CS0, &(tab_temp[0]), 176 * 220, C_LCD_IF_DISPLAY);

#endif // USE_LCD_DMA
 	if ( rvf_unlock_mutex (&lcd_mutex_ptr.lcd_framebuf_mutex) != RVF_OK)
	{
	  	rvf_send_trace("LCD MUTEX UNLOCK FAILED",strlen("LCD MUTEX UNLOCK FAILED"), NULL_PARAM,
                RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );
		 	return LCD_PAR_RET_FAIL;
	}

#else // R2D HORIZONTAL
#error "R2D Vertical for This Chipset Not Supported"
#endif
#else // _WINDOWS
#warn "r2d_refresh Is Empty Function for WINDOWS Build"
#endif
		lcd_g_state = DISPLAY_OFF;
		//rvf_start_timer(RVF_TIMER_0,RVF_MS_TO_TICKS(5000),FALSE);
		// cutoff LCD interface clock after refresh
		LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFFFD) | (LCD_CLOCK13_DIS << LCD_INTERFACE_CNTL_REG_CLOCK13_EN_POS));
		 return LCD_PAR_RET_OK;
}


#ifdef R2D_USE_LCD_DMA
void r2d_dma_callback(void *response)
{
   UINT32 bytes_transfered;
    T_DMA_STATUS_RSP_MSG *dma_status_rsp_msg = (T_DMA_STATUS_RSP_MSG *)response;

    switch(dma_status_rsp_msg->result.status)
    {

        case DMA_RESERVE_OK:
            r2d_lcd_dma_channel = dma_status_rsp_msg->result.channel;
			rvf_send_trace("LCD DMA_RESERVE_OK",strlen("LCD DMA_RESERVE_OK"),dma_status_rsp_msg->result.status ,
                    RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );
            r2d_lcd_dma_state = R2D_LCD_DMA_IDLE;
            break;

        case DMA_PARAM_SET:
	              r2d_lcd_dma_state = R2D_LCD_DMA_CHANNEL_PARAM_SET;
    //        rvf_send_trace("LCD DMA_PARAM_SET",strlen("LCD DMA_PARAM_SET"),dma_status_rsp_msg->result.status ,
    //                RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );
       		f_lcd_if_dma_enable(C_LCD_IF_CS0,0,C_LCD_IF_DISPLAY);
            		break;

        case DMA_COMPLETED:
            {
			bytes_transfered =  ( r2d_lcd_dma_channel_info.nmb_elements
                        							* r2d_lcd_dma_channel_info.nmb_frames
                        							* 2);
	                f_lcd_if_dma_disable(C_LCD_IF_CS0,f_lcd_if_dummy_callback);
      //          rvf_send_trace("LCD DMA_COMPLETED",strlen("LCD DMA_COMPLETED"),dma_status_rsp_msg->result.status ,
      //                  RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );
              //  rvf_send_trace("LCD DMA_COMPLETED_BYTES TRANSFERED",strlen("LCD DMA_COMPLETED_BYTES TRANSFERED"),bytes_transfered,
                //        RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );

               

                	if(r2d_lcd_dma_state == R2D_LCD_DMA_CHANNEL_PARAM_SET)
                	{
                    		r2d_lcd_dma_state = R2D_LCD_DMA_IDLE;
                    		r2d_lcd_dma_bytes_to_transfer -= bytes_transfered;

                    		if(r2d_lcd_dma_bytes_to_transfer > 0)
                    		{
                        		r2d_lcd_dma_channel_info.source_address = ((UINT32)&(tab_temp[0])) + bytes_transfered;

                //        	rvf_send_trace("source address 2 ",strlen("source address 2"),r2d_lcd_dma_channel_info.source_address ,
                //          						RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );

					r2d_lcd_dma_channel_info.nmb_frames = 1;
                        		r2d_lcd_dma_channel_info.nmb_elements = r2d_lcd_dma_bytes_to_transfer / 2 ;
                        		dma_set_channel_parameters (r2d_lcd_dma_channel, &r2d_lcd_dma_channel_info);
                    		}
                    else
                    {
                        rvf_send_event( lcd_addr_id,EVENT_MASK(RVF_APPL_EVT_1));
                    }
                    break;
                }
            }
        case DMA_NO_CHANNEL:
        case DMA_TOO_MANY_REQUESTS:
        case DMA_INVALID_PARAMETER:
        case DMA_NOT_READY:
        case DMA_MEMORY_ERROR:
        case DMA_ACTION_NOT_ALLOWED:
        case DMA_CHANNEL_BUSY:
        case DMA_QUEUED:
        case DMA_TIMEOUT_SOURCE:
        case DMA_TIMEOUT_DESTINATION:
        case DMA_MISS_EVENT:
        default:

            //r2d_lcd_dma_ok = 0;
            r2d_lcd_dma_state = R2D_LCD_DMA_ERROR;
            rvf_send_trace("LCD DMA_ERROR",strlen("LCD DMA_ERROR"),dma_status_rsp_msg->result.status ,
                    RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );
            rvf_send_event( lcd_addr_id,EVENT_MASK(RVF_APPL_EVT_1));
            break;
    } /* switch dma_status_rsp_msg->result.status */

    rvf_free_buf(response);
    return;
}

static E_LCD_IF_CALLBACK_RET f_lcd_if_dummy_callback (void)
{
    /* nothing */
    return 0;
}

#endif /* USE_LCD_DMA  */



#ifdef LCD_LITE_DMA

void lcd_dma_lite_callback(SYS_UWORD16 data)
{

	status_ret = NU_Activate_HISR(&LCD_DMA_HISR);

}

static E_LCD_IF_CALLBACK_RET f_lcd_if_dummy_callback (void)
{
    /* nothing */
    return 0;
}


void LCD_DMA_HISREntry(void)
{
	f_dma_channel_disable(C_DMA_CHANNEL_2);
	rvf_send_event( lcd_addr_id,EVENT_MASK(RVF_APPL_EVT_1));
}


#endif //LCD_LITE_DMA



ELCD_PAR_RET lcd_parallel_control(T_LCD_COMMAND command,void * p_cmd_param)
{
    SYS_UWORD16 command1;
	lcd_tuningtable lcd_tuning_table;
	T_LCD_REFCONTROL T_LCD_REF_CONTROL;
		switch(command)
		{
		case 0:/*LCD_CLEAR*/
			return LCD_PAR_RET_FAIL;
		case 1:/*LCD_DISPLAYON */
			command1 = 0xAF;
  			f_lcd_if_poll_write(C_LCD_IF_CS0, &(command1), 1, C_LCD_IF_INSTRUCTION);
			break;
		case 2:/*LCD_DISPLAYOFF */
			command1 = 0xAE;
  			f_lcd_if_poll_write(C_LCD_IF_CS0, &(command1), 1, C_LCD_IF_INSTRUCTION);
			break;
		case 3:/*LCD_GET_CONFIG*/
			p_cmd_param = (lcd_tuningtable *)&lcd_tuning_table;

   //         rvf_send_trace("LCD_GET_CONFIG",strlen("LCD_GET_CONFIG"),NULL ,
   //                   RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );

			((lcd_tuningtable *)p_cmd_param)->partial_update=lcd_tuning_table.partial_update;
			((lcd_tuningtable *)p_cmd_param)->On_Screen_Display=lcd_tuning_table.On_Screen_Display;
			((lcd_tuningtable *)p_cmd_param)->dedicated_dma=lcd_tuning_table.dedicated_dma;
			((lcd_tuningtable *)p_cmd_param)->p_lcd_configparams->height=lcd_tuning_table.p_lcd_configparams->height;
			((lcd_tuningtable *)p_cmd_param)->p_lcd_configparams->width=lcd_tuning_table.p_lcd_configparams->width;
			((lcd_tuningtable *)p_cmd_param)->p_lcd_configparams->orientation=lcd_tuning_table.p_lcd_configparams->orientation;
			((lcd_tuningtable *)p_cmd_param)->p_lcd_configparams->pixel_format=lcd_tuning_table.p_lcd_configparams->pixel_format;
			((lcd_tuningtable *)p_cmd_param)->p_lcd_configparams->endianness=lcd_tuning_table.p_lcd_configparams->endianness;
			((lcd_tuningtable *)p_cmd_param)->p_lcd_configparams->refresh_control=lcd_tuning_table.p_lcd_configparams->refresh_control;
			break;
		case 4:/*LCD_SET_CONFIG*/

           // 	rvf_send_trace("LCD_SET_CONFIG",strlen("LCD_SET_CONFIG"),NULL ,
           //         RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );


			lcd_config_params.height=((lcd_configparams *)p_cmd_param)->height;
			lcd_config_params.width=((lcd_configparams *)p_cmd_param)->width;
			lcd_config_params.orientation=((lcd_configparams *)p_cmd_param)->orientation;
			lcd_config_params.pixel_format=((lcd_configparams *)p_cmd_param)->pixel_format;
			lcd_config_params.endianness=((lcd_configparams *)p_cmd_param)->endianness;
			lcd_config_params.refresh_control=((lcd_configparams *)p_cmd_param)->refresh_control;
			lcd_config_params.height=((lcd_configparams *)p_cmd_param)->height;
			break;
		case 5:/*LCD_REFCONTROL*/

        //    rvf_send_trace("LCD_REF_CONTROL",strlen("LCD_REF_CONTROL"),NULL ,
        //            RV_TRACE_LEVEL_DEBUG_HIGH, LCD_USE_ID );

			if(*((T_LCD_REFCONTROL *)p_cmd_param)==REF_ENABLED)
				lcd_config_params.refresh_control=REF_ENABLED;
			else
				lcd_config_params.refresh_control=REF_DISABLED;
			break;

		}
		return LCD_PAR_RET_OK;
}




