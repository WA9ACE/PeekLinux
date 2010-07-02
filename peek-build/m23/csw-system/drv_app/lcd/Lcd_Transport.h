/*=============================================================================
 * Copyright 2005 Texas Instruments Incorporated. All Rights Reserved.
*/

#ifndef LCD_TRANSPORT_HEADER
#define LCD_TRANSPORT_HEADER

#include <stdlib.h>
#include "chipset.cfg"    /* for CHIPSET defination */
#include "sys_types.h"
#include "sys_conf.h"    
#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */
#include "Lcd_manager.h"
#include "rv/rv_defined_swe.h"

#ifdef RVM_DATALIGHT_SWE
    #include "swconfig.cfg" 
#endif


#define LITTLE_END 0
#define BIG_END    1 

#define FRAME_BUFFER_ENDIAN LITTLE_END 
#define FRAME_BUFFER_INVERTED 0 //input framebuffer is in inverd RGB format
#if ((defined (RVM_NAN_SWE)) || ((defined (RVM_DATALIGHT_SWE)) && ((DATALIGHT_NAND == 1) || (DATALIGHT_NAND == 2))))
#define R2D_ISAMPLE_USE_MUXING 1
#endif
  /****************************************************************************
   *                            GENERAL CONSTANTS
   ***************************************************************************/
    #ifndef C_MAP_LCD_IF_BASE
      #define C_MAP_LCD_IF_BASE                0xFFFFA000L   // LCD registers
    #endif

   /* driver version */

   #define C_LCD_IF_DRIVER_VERSION ( 0x0001 )

   /* LCD Interface Tx FIFO size */

   #define C_LCD_IF_TX_FIFO_SIZE   ( 128 )

   /* infinite while loop constant */

   #define C_LCD_IF_INFINITE_LOOP  ( 1 )

  /****************************************************************************
   *                Definition of the LCD Interface registers
   ***************************************************************************/

  /* LCD IF CNTL_REG */

  #define C_LCD_IF_CNTL_REG  	( C_MAP_LCD_IF_BASE ) 		  

    #define C_LCD_IF_CNTL_REG_SOFT_NRST_POS	            ( 0 )
    #define C_LCD_IF_CNTL_REG_SOFT_NRST_MASK	        ( 0x0001 )
    #define C_LCD_IF_CNTL_REG_SOFT_NRST_INIT	        ( 0x1 )

    #define C_LCD_IF_CNTL_REG_CLOCK13_EN_POS	        ( 1 )
    #define C_LCD_IF_CNTL_REG_CLOCK13_EN_MASK	        ( 0x0002 )
    #define C_LCD_IF_CNTL_REG_CLOCK13_EN_INIT	        ( 0x1 )

    #define C_LCD_IF_CNTL_REG_TX_CLOCK_DIV_POS	        ( 2 )
    #define C_LCD_IF_CNTL_REG_TX_CLOCK_DIV_MASK	        ( 0x000C )
    #define C_LCD_IF_CNTL_REG_TX_CLOCK_DIV_INIT	        ( 0x0 )

    #define C_LCD_IF_CNTL_REG_RX_CLOCK_DIV_POS	        ( 4 )
    #define C_LCD_IF_CNTL_REG_RX_CLOCK_DIV_MASK	        ( 0x0030 )
    #define C_LCD_IF_CNTL_REG_RX_CLOCK_DIV_INIT	        ( 0x0 )

    #define C_LCD_IF_CNTL_REG_FIFO_EMPTY_IT_EN_POS	    ( 6 )
    #define C_LCD_IF_CNTL_REG_FIFO_EMPTY_IT_EN_MASK	    ( 0x0040 )
    #define C_LCD_IF_CNTL_REG_FIFO_EMPTY_IT_EN_INIT	    ( 0x0 )

    #define C_LCD_IF_CNTL_REG_LCD_READ_EVENT_IT_EN_POS	( 7 )
    #define C_LCD_IF_CNTL_REG_LCD_READ_EVENT_IT_EN_MASK	( 0x0080 )
    #define C_LCD_IF_CNTL_REG_LCD_READ_EVENT_IT_EN_INIT	( 0x0 )

    #define C_LCD_IF_CNTL_REG_DMA_EN_POS	            ( 8 )
    #define C_LCD_IF_CNTL_REG_DMA_EN_MASK	            ( 0x0100 )
    #define C_LCD_IF_CNTL_REG_DMA_EN_INIT	            ( 0x0 )

    #define C_LCD_IF_CNTL_REG_MODE_POS	                ( 9 )
    #define C_LCD_IF_CNTL_REG_MODE_MASK	                ( 0x0200 )
    #define C_LCD_IF_CNTL_REG_MODE_INIT	                ( 0x0 )

    #define C_LCD_IF_CNTL_REG_FLIP_BYTES_POS	        ( 10 )
    #define C_LCD_IF_CNTL_REG_FLIP_BYTES_MASK	        ( 0x0400 )
    #define C_LCD_IF_CNTL_REG_FLIP_BYTES_INIT	        ( 0x0 )

    #define C_LCD_IF_CNTL_REG_SUSPEND_EN_POS	        ( 11 )
    #define C_LCD_IF_CNTL_REG_SUSPEND_EN_MASK	        ( 0x0800 )
    #define C_LCD_IF_CNTL_REG_SUSPEND_EN_INIT	        ( 0x1 )

    #define C_LCD_IF_CNTL_REG_MIN_FRAME_SIZE_POS	( 12 )
    #define C_LCD_IF_CNTL_REG_MIN_FRAME_SIZE_MASK	( 0x3000 )
    #define C_LCD_IF_CNTL_REG_MIN_FRAME_SIZE_INIT	( 0x3 )

    #define C_LCD_IF_CNTL_REG_N_DUMMY_POS	        ( 14 )
    #define C_LCD_IF_CNTL_REG_N_DUMMY_MASK	        ( 0xC000 )
    #define C_LCD_IF_CNTL_REG_N_DUMMY_INIT	        ( 0x1 )


  /* LCD IF LCD_CNTL_REG */

  #define C_LCD_IF_LCD_CNTL_REG ( C_MAP_LCD_IF_BASE + 0x02 )

    #define C_LCD_IF_LCD_CNTL_REG_LCD_NCS0_POS	        ( 0 )
    #define C_LCD_IF_LCD_CNTL_REG_LCD_NCS0_MASK	        ( 0x0001 )
    #define C_LCD_IF_LCD_CNTL_REG_LCD_NCS0_INIT	        ( 0x1 )

    #define C_LCD_IF_LCD_CNTL_REG_LCD_RS_POS	        ( 1 )
    #define C_LCD_IF_LCD_CNTL_REG_LCD_RS_MASK	        ( 0x0002 )
    #define C_LCD_IF_LCD_CNTL_REG_LCD_RS_INIT	        ( 0x0 )

    #define C_LCD_IF_LCD_CNTL_REG_LCD_START_READ_POS	( 2 )
    #define C_LCD_IF_LCD_CNTL_REG_LCD_START_READ_MASK	( 0x0004 )
    #define C_LCD_IF_LCD_CNTL_REG_LCD_START_READ_INIT	( 0x0 )
    #define C_LCD_IF_LCD_CNTL_REG_LCD_START_READ_EN	( 0x1 )

    #define C_LCD_IF_LCD_CNTL_REG_LCD_NRESET_POS	( 3 )
    #define C_LCD_IF_LCD_CNTL_REG_LCD_NRESET_MASK	( 0x0008 )
    #define C_LCD_IF_LCD_CNTL_REG_LCD_NRESET_INIT	( 0x0 )

    #define C_LCD_IF_LCD_CNTL_REG_LCD_NCS1_POS	        ( 4 )
    #define C_LCD_IF_LCD_CNTL_REG_LCD_NCS1_MASK	        ( 0x0010 )
    #define C_LCD_IF_LCD_CNTL_REG_LCD_NCS1_INIT	        ( 0x1 )

  /* LCD IF LCD_IF_STS_REG */  

  #define C_LCD_IF_STS_REG 	    ( C_MAP_LCD_IF_BASE + 0x04 )  

    #define C_LCD_IF_STS_REG_FIFO_EMPTY_STATUS_POS	    ( 0 )
    #define C_LCD_IF_STS_REG_FIFO_EMPTY_STATUS_MASK	    ( 0x0001 )

    #define C_LCD_IF_STS_REG_FIFO_FULL_STATUS_POS	    ( 1 )
    #define C_LCD_IF_STS_REG_FIFO_FULL_STATUS_MASK	    ( 0x0002 )

    #define C_LCD_IF_STS_REG_LCD_READ_EVENT_STATUS_POS	( 2 )
    #define C_LCD_IF_STS_REG_LCD_READ_EVENT_STATUS_MASK	( 0x0004 )

  /* LCD IF WR_FIFO */
  #define C_LCD_IF_WR_FIFO 	    ( C_MAP_LCD_IF_BASE + 0x06 ) 

    #define C_LCD_IF_WR_FIFO_DATA_TX_LSB_POS	        ( 0 )
    #define C_LCD_IF_WR_FIFO_DATA_TX_LSB_MASK	        ( 0x00FF )

    #define C_LCD_IF_WR_FIFO_DATA_TX_MSB_POS	        ( 8 )
    #define C_LCD_IF_WR_FIFO_DATA_TX_MSB_MASK	        ( 0xFF00 )
   
  /* LCD IF RD_REG */
  #define C_LCD_IF_RD_REG       ( C_MAP_LCD_IF_BASE + 0x08 )  

    #define C_LCD_IF_RD_REG_DATA_RX_LSB_POS	            ( 0 )
    #define C_LCD_IF_RD_REG_DATA_RX_LSB_MASK	        ( 0x00FF )

    #define C_LCD_IF_RD_REG_DATA_RX_MSB_POS	            ( 8 )
    #define C_LCD_IF_RD_REG_DATA_RX_MSB_MASK	        ( 0xFF00 )

  /* LCD IF PIN_CFG_REG */
#define C_LCD_IF_PIN_CFG_REG_BASE (0xFFFF0000)
    #define C_LCD_IF_PIN_CFG_DATA0 ( C_LCD_IF_PIN_CFG_REG_BASE + 0xF15A )
    #define C_LCD_IF_PIN_CFG_DATA1 ( C_LCD_IF_PIN_CFG_REG_BASE + 0xF15C )
    #define C_LCD_IF_PIN_CFG_DATA2 ( C_LCD_IF_PIN_CFG_REG_BASE + 0xF15E )
    #define C_LCD_IF_PIN_CFG_DATA3 ( C_LCD_IF_PIN_CFG_REG_BASE + 0xF160 )
    #define C_LCD_IF_PIN_CFG_DATA4 ( C_LCD_IF_PIN_CFG_REG_BASE + 0xF162 )
    #define C_LCD_IF_PIN_CFG_DATA5 ( C_LCD_IF_PIN_CFG_REG_BASE + 0xF164 )
    #define C_LCD_IF_PIN_CFG_DATA6 ( C_LCD_IF_PIN_CFG_REG_BASE + 0xF166 )
    #define C_LCD_IF_PIN_CFG_DATA7 ( C_LCD_IF_PIN_CFG_REG_BASE + 0xF168 )

    #define C_LCD_IF_PIN_CFG_POS       ( 0 )
    #define C_LCD_IF_PIN_CFG_MASK    ( ~( (1 << 0) | ( 1 << 1) ) )

#define CONF_LCD_CAM_NAND 0xFFFEF01E

  /****************************************************************************
   *                            MACRO DEFINITION
   ***************************************************************************/

  /****************************************************************************
   *
   *  MACRO NAME: F_LCD_IF_WRITE_IN_FIFO
   *      Write d_data into WR_FIFO register.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Description
   *  ------------   ----------------------------------------------------------
   *  d_data           16 boits word data (SYS_UWORD16)
   *
   * RETURN VALUE: None
   *
   ***************************************************************************/

   #define F_LCD_IF_WRITE_IN_FIFO( d_data )	*(volatile SYS_UWORD16* ) C_LCD_IF_WR_FIFO = d_data

  /****************************************************************************
   *
   *  MACRO NAME: F_LCD_IF_TX_FIFO_EMPTY_STATUS
   *      Return the LCD Interface TX FIFO Empty status.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Description
   *  ------------   ----------------------------------------------------------
   *  None.
   *
   * RETURN VALUE: SYS_UWORD8 : Tx FIFO empty status (0 or 1)
   *
   ***************************************************************************/

   #define F_LCD_IF_TX_FIFO_EMPTY_STATUS \
    ( SYS_UWORD8 )((* (volatile SYS_UWORD16 *) C_LCD_IF_STS_REG) & C_LCD_IF_STS_REG_FIFO_EMPTY_STATUS_MASK)

  /****************************************************************************
   *                          TYPE AND STRUCTURE DEFINITION
   ***************************************************************************/

  /* type definition for LCD Clock divider */

  typedef  SYS_UWORD8 E_LCD_IF_CLOCK_DIVIDER;

  #define  C_LCD_IF_DIV8 ( 0 )
  #define  C_LCD_IF_DIV4 ( 1 )
  #define  C_LCD_IF_DIV2 ( 2 )
  #define  C_LCD_IF_DIV1 ( 3 )

  /* type definition for LCD interface mode */

  typedef  SYS_UWORD8 E_LCD_IF_MODE;

  #define  C_LCD_IF_68000_MODE ( 0 )
  #define  C_LCD_IF_8086_MODE  ( 1 )

  /* type definition for LCD Interface flip bytes */

  typedef SYS_UWORD8 E_LCD_IF_FLIP;

  #define C_LCD_IF_MSB_FIRST   ( 0 )
  #define C_LCD_IF_LSB_FIRST   ( 1 )

  /* type definition for Instruction Selection line level */

  typedef  SYS_UWORD8 E_LCD_IF_ISLL;

  #define  C_LCD_IF_LOW  ( 0 )
  #define  C_LCD_IF_HIGH ( 1 )

  /* type definition for Minimum frame size */

  typedef  SYS_UWORD8 E_LCD_IF_FRAME_SZ;

  #define  C_LCD_IF_16_WORDS  ( 0 )
  #define  C_LCD_IF_32_WORDS  ( 1 )
  #define  C_LCD_IF_64_WORDS  ( 2 )
  #define  C_LCD_IF_128_WORDS ( 3 )

  /* type definition for Read dummy cycle */

  typedef  SYS_UWORD8  E_LCD_IF_DUMMY_CYCLE;

  #define  C_LCD_IF_0_CYCLE  ( 0 )
  #define  C_LCD_IF_1_CYCLE  ( 1 )
  #define  C_LCD_IF_2_CYCLES ( 2 )

  /* Initialization sub structure for Transmit */

  typedef struct {
        E_LCD_IF_CLOCK_DIVIDER d_clock_divider;
  } T_LCD_IF_INIT_TX;

  /* Initialization sub structure for Receive */

  typedef struct {
        E_LCD_IF_CLOCK_DIVIDER d_clock_divider;
        E_LCD_IF_DUMMY_CYCLE   d_dummy_cycles;
  } T_LCD_IF_INIT_RX;

  /* LCD Interface Driver Initialization structure */

  typedef struct {
        E_LCD_IF_MODE    d_mode;
        E_LCD_IF_FLIP    b_flip_bytes;
        E_LCD_IF_ISLL    d_isll ;
        T_LCD_IF_INIT_TX d_tx_init;
        T_LCD_IF_INIT_RX d_rx_init;
   } T_LCD_IF_INIT;

  /* type definition for the LCD Chip select */

  typedef  SYS_UWORD8 E_LCD_IF_CS;

  #define  C_LCD_IF_CS0             ( 0 )
  #define  C_LCD_IF_CS1             ( 1 )
  #define  C_LCD_IF_CS_NOT_SELECTED ( 2 )

  /* type definition for the LCD data type */

  typedef  SYS_UWORD8 E_LCD_IF_DATA_TYPE;

  #define  C_LCD_IF_INSTRUCTION ( 0 )
  #define  C_LCD_IF_DISPLAY     ( 1 )
 
  /* type definition for the current LCD driver internal state */

  typedef  SYS_UWORD8 E_LCD_IF_STATUS;

  #define  C_LCD_IF_IDLE	    ( 0 ) 
  #define  C_LCD_IF_RUNNING	    ( 1 ) 

  /* type definition for the callback function return parameter */

  typedef  SYS_UWORD8 E_LCD_IF_CALLBACK_RET;

  #define  C_LCD_IF_CALLBACK_OK 	( 0 )
  #define  C_LCD_IF_CALLBACK_DISABLE_IT	( 1 )

  /* Type definition for the ON / OFF levels */

  typedef  SYS_UWORD8 E_LCD_IF_RESET;

  #define  C_LCD_IF_ON 	( 0 )
  #define  C_LCD_IF_OFF	( 1 )

  /* Type definition for the ON / OFF levels */

  typedef  SYS_UWORD8 E_LCD_IF_CLOCK;

  #define  C_LCD_IF_CLOCK_ON 	( 1 )
  #define  C_LCD_IF_CLOCK_OFF	( 0 )

  /* type definition for callback function */

  typedef E_LCD_IF_CALLBACK_RET (*T_LCD_IF_CALLBACK)( void );

  /* type definition for the LCD IF API function return parameter */

  typedef  SYS_UWORD8 E_LCD_IF_RET;

  #define  C_LCD_IF_RET_OK    ( 0 )
  #define  C_LCD_IF_RET_ERR   ( 1 )

  /****************************************************************************
   *                            INTERANL MACRO DEFINITION
   ***************************************************************************/

  /****************************************************************************
   *
   *  MACRO NAME: F_LCD_IF_WAIT_LCD_IF_RESET
   *      Loop that wait for the read status.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Description
   *  ------------   ----------------------------------------------------------
   *  None
   *
   * RETURN VALUE: None
   *
   ***************************************************************************/

   #define F_LCD_IF_WAIT_LCD_IF_RESET \
	 while( ((*(volatile SYS_UWORD16*) C_LCD_IF_CNTL_REG) & C_LCD_IF_CNTL_REG_SOFT_NRST_MASK) \
	                                                     != C_LCD_IF_CNTL_REG_SOFT_NRST_MASK )
	                                             
  /****************************************************************************
   *
   *  MACRO NAME: F_LCD_IF_WAIT_TX_FIFO_EMPTY
   *      Loop that wait for the TX FIFO EMPTY status set to 1.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Description
   *  ------------   ----------------------------------------------------------
   *  None
   *
   * RETURN VALUE: None
   *
   ***************************************************************************/

   #define F_LCD_IF_WAIT_TX_FIFO_EMPTY \
	 while( ((*(volatile SYS_UWORD16*) C_LCD_IF_STS_REG) & C_LCD_IF_STS_REG_FIFO_EMPTY_STATUS_MASK) \
	                                                    != C_LCD_IF_STS_REG_FIFO_EMPTY_STATUS_MASK )
	                                                     

  /****************************************************************************
   *
   *  MACRO NAME: F_LCD_IF_WAIT_READ_STATUS
   *      Loop that wait for the Read status set to 1.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Description
   *  ------------   ----------------------------------------------------------
   *  None
   *
   * RETURN VALUE: None
   *
   ***************************************************************************/

   #define F_LCD_IF_WAIT_READ_STATUS \
	 while( ((*(volatile SYS_UWORD16*) C_LCD_IF_STS_REG) & C_LCD_IF_STS_REG_LCD_READ_EVENT_STATUS_MASK) \
	                                             != C_LCD_IF_STS_REG_LCD_READ_EVENT_STATUS_MASK )

  /****************************************************************************
   *
   *  MACRO NAME: F_LCD_IF_ENABLE_TX_FIFO_EMPTY_IT
   *      Enables the LCD Interface Tx FIFO Empty Interrupt.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Description
   *  ------------   ----------------------------------------------------------
   *  None
   *
   * RETURN VALUE: None
   *
   ***************************************************************************/

   #define F_LCD_IF_ENABLE_TX_FIFO_EMPTY_IT \
	 ((*(volatile SYS_UWORD16*) C_LCD_IF_CNTL_REG) |= C_LCD_IF_CNTL_REG_FIFO_EMPTY_IT_EN_MASK )
	                                             

  /****************************************************************************
   *
   *  MACRO NAME: F_LCD_IF_DISABLE_TX_FIFO_EMPTY_IT
   *      Disables the LCD Interface Tx FIFO Empty Interrupt.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Description
   *  ------------   ----------------------------------------------------------
   *  None
   *
   * RETURN VALUE: None
   *
   ***************************************************************************/

   #define F_LCD_IF_DISABLE_TX_FIFO_EMPTY_IT \
	  ((*(volatile SYS_UWORD16*) C_LCD_IF_CNTL_REG) &= ~C_LCD_IF_CNTL_REG_FIFO_EMPTY_IT_EN_MASK )

  /****************************************************************************
   *
   *  MACRO NAME: F_LCD_IF_ENABLE_DMA
   *      Enables the LCD Interface DMA capabilities.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Description
   *  ------------   ----------------------------------------------------------
   *  None
   *
   * RETURN VALUE: None
   *
   ***************************************************************************/

   #define F_LCD_IF_ENABLE_DMA \
	  ((*(volatile SYS_UWORD16*) C_LCD_IF_CNTL_REG) |= C_LCD_IF_CNTL_REG_DMA_EN_MASK )

  /****************************************************************************
   *
   *  MACRO NAME: F_LCD_IF_DISABLE_DMA
   *      Disable the LCD Interface DMA capabilities.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Description
   *  ------------   ----------------------------------------------------------
   *  None
   *
   * RETURN VALUE: None
   *
   ***************************************************************************/

   #define F_LCD_IF_DISABLE_DMA \
	  ((*(volatile SYS_UWORD16*) C_LCD_IF_CNTL_REG) &= ~C_LCD_IF_CNTL_REG_DMA_EN_MASK )
	                                             

  /****************************************************************************
   *
   *  MACRO NAME: F_LCD_CAM_NANDFLASH_AQUIRE_BUS
   *     Locks the mutex used to serialise the access to the multiplexed bus.
   *     The bus is shared between camera, lcd and nand flash.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Description
   *  ------------   ----------------------------------------------------------
   *  None
   *
   * RETURN VALUE: None
   *
   ***************************************************************************/
#ifdef R2D_ISAMPLE_USE_MUXING
   #define F_LCD_CAM_NANDFLASH_AQUIRE_BUS conf_acquire_pins(MUX_LCD);
#else
	#define F_LCD_CAM_NANDFLASH_AQUIRE_BUS
#endif

  /****************************************************************************
   *
   *  MACRO NAME: F_LCD_CAM_NANDFLASH_RELEASE_BUS
   *  	Make sure that lcd bus' are tristated, then
   *     unlocks the mutex used to serialise the access to the multiplexed bus.
   *     The bus is shared between camera, lcd and nand flash.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Description
   *  ------------   ----------------------------------------------------------
   *  None
   *
   * RETURN VALUE: None
   *
   ***************************************************************************/
#ifdef R2D_ISAMPLE_USE_MUXING

   #define F_LCD_CAM_NANDFLASH_RELEASE_BUS conf_release_pins(MUX_LCD);

#else
	#define F_LCD_CAM_NANDFLASH_RELEASE_BUS
#endif

		
  /****************************************************************************
   *
   *  MACRO NAME: F_LCD_SET_PIN_CONFIG
   *     Connect the bus pins to LCD controller.
   *     The bus is shared between camera, lcd and nand flash.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Description
   *  ------------   ----------------------------------------------------------
   *  None
   *
   * RETURN VALUE: None
   *
   ***************************************************************************/

//#ifdef R2D_ISAMPLE_USE_MUXING
#if 0

   #define  F_LCD_SET_PIN_CONFIG \
	do { \
		(*(volatile SYS_UWORD16*) C_LCD_IF_PIN_CFG_DATA0) &= C_LCD_IF_PIN_CFG_MASK; \
		(*(volatile SYS_UWORD16*) C_LCD_IF_PIN_CFG_DATA1) &= C_LCD_IF_PIN_CFG_MASK; \
		(*(volatile SYS_UWORD16*) C_LCD_IF_PIN_CFG_DATA2) &= C_LCD_IF_PIN_CFG_MASK; \
		(*(volatile SYS_UWORD16*) C_LCD_IF_PIN_CFG_DATA3) &= C_LCD_IF_PIN_CFG_MASK; \
		(*(volatile SYS_UWORD16*) C_LCD_IF_PIN_CFG_DATA4) &= C_LCD_IF_PIN_CFG_MASK; \
		(*(volatile SYS_UWORD16*) C_LCD_IF_PIN_CFG_DATA5) &= C_LCD_IF_PIN_CFG_MASK; \
		(*(volatile SYS_UWORD16*) C_LCD_IF_PIN_CFG_DATA6) &= C_LCD_IF_PIN_CFG_MASK; \
		(*(volatile SYS_UWORD16*) C_LCD_IF_PIN_CFG_DATA7) &= C_LCD_IF_PIN_CFG_MASK; \
	} while (0);
#else
	#define F_LCD_SET_PIN_CONFIG
#endif
		
		


	/****************************************************************************
   *                         INTERNAL TYPE AND STRUCTURES
   ***************************************************************************/

  /* type definition for the Data Access */

  typedef  SYS_UWORD8 E_LCD_IF_DATA_ACCESS;

  #define  C_LCD_IF_READ    ( 1 ) 
  #define  C_LCD_IF_WRITE   ( 0 ) 

  /* type definition for the current LCD driver internal state */

  typedef  SYS_UWORD8 E_LCD_IF_STATE;

  #define  C_LCD_IF_IDLE    ( 0 ) 
  #define  C_LCD_IF_IT_CUST ( 1 ) 
  #define  C_LCD_IF_IT_BUFF ( 2 )

  /* type definition Transmit sub structure */
 
  typedef struct {
        E_LCD_IF_CLOCK_DIVIDER d_clock_divider;
        T_LCD_IF_CALLBACK      p_callback;
        SYS_UWORD16            *p_buffer;
        SYS_UWORD32            d_buffer_size;
  } T_LCD_IF_TX;

  /* type definition Receive sub structure */

  typedef struct {
        E_LCD_IF_CLOCK_DIVIDER d_clock_divider;
        E_LCD_IF_DUMMY_CYCLE   d_dummy_cycles;
  } T_LCD_IF_RX;

  /* type definition LCD Interface Driver Chip select structure */

  typedef struct {
        E_LCD_IF_MODE    d_mode;
        E_LCD_IF_FLIP    b_flip_bytes;
        E_LCD_IF_ISLL    d_isll ;
        T_LCD_IF_TX      d_tx;
        T_LCD_IF_RX      d_rx;
        SYS_UWORD16      d_cntl_reg;
        SYS_UWORD16      d_lcd_cntl_reg;
  } T_LCD_IF_CS_INIT;

  /* type definition LCD Interface Driver Internal structure */

  #define C_LCD_IF_CS_NUMBER  (2)

  typedef struct {
       T_LCD_IF_CS_INIT d_cs_struct[C_LCD_IF_CS_NUMBER];
       E_LCD_IF_CS      d_current_cs;
       E_LCD_IF_STATE   d_interrupt_state;
  } T_LCD_IF_INTERNAL;


  




/*============================================================================*/
/*      	T_LCD_SELECT               selects a specific LCD				  */
/*============================================================================*/
typedef enum 
{
	LCD_PAR_RET_OK,
   	LCD_PAR_RET_FAIL
} ELCD_PAR_RET;



/******************************************************************************
 *                            PROTOTYPE DEFINITION
 ******************************************************************************
 *           The following prototypes declares the public interface.
 */
/*============================================================================*/
/*!
 * @function lcd_parallel_config
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to initialize the LCD manager.It also initialises 
 *	 the LCD display and the LCD controller driver.  
 *   
 * <b> Context </b><br>
 *   This must be called before any other display services and functions
 *
 *  @result <br>
 *     The LCD manager,LCD display and the LCD controller driver will be 
 *	   initialized.
 */
    
     ELCD_PAR_RET lcd_parallel_config();
 

/*!
 * @function lcd_parallel_display
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to initialize the LCD manager.It also initialises 
 *	 the LCD display and the LCD controller driver.  
 *   
 * <b> Context </b><br>
 *   This must be called before any other display services and functions
 *
 *  @result <br>
 *     The LCD manager,LCD display and the LCD controller driver will be 
 *	   initialized.
 */
	ELCD_PAR_RET lcd_parallel_display(UINT16 *ImageDataPtrPAR);


/*!
 * @function lcd_parallel_control
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to initialize the LCD manager.It also initialises 
 *	 the LCD display and the LCD controller driver.  
 *   
 * <b> Context </b><br>
 *   This must be called before any other display services and functions
 *
 *  @result <br>
 *     The LCD manager,LCD display and the LCD controller driver will be 
 *	   initialized.
 */

  ELCD_PAR_RET lcd_parallel_control(T_LCD_COMMAND command,void * p_cmd_param);

#endif	


