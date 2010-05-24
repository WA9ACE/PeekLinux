/*                @(#) nom : sys_memif.h SID: 1.2 date : 05/23/03             */
/* Filename:      sys_memif.h                                                 */
/* Version:       1.2                                                         */
/******************************************************************************
 *                   WIRELESS COMMUNICATION SYSTEM DEVELOPMENT
 *
 *             (C) 2002 Texas Instruments France. All rights reserved
 *
 *                          Author : Francois AMAND
 *
 *
 *  Important Note
 *  --------------
 *
 *  This S/W is a preliminary version. It contains information on a product 
 *  under development and is issued for evaluation purposes only. Features 
 *  characteristics, data and other information are subject to change.
 *
 *  The S/W is furnished under Non Disclosure Agreement and may be used or
 *  copied only in accordance with the terms of the agreement. It is an offence
 *  to copy the software in any way except as specifically set out in the 
 *  agreement. No part of this document may be reproduced or transmitted in any
 *  form or by any means, electronic or mechanical, including photocopying and
 *  recording, for any purpose without the express written permission of Texas
 *  Instruments Inc.
 *
 ******************************************************************************
 *
 *  FILE NAME: sys_memif.h
 *
 *
 *  PURPOSE:  Include file to use the ARM memory interface drivers for CALYPSO 
 *            PLUS.
 *            The drivers allows ARM memory interface configuration and
 *            control.
 *
 *
 *  FILE REFERENCES:
 *
 *  Name                  IO      Description
 *  -------------         --      ---------------------------------------------
 *  
 *
 *
 *  EXTERNAL VARIABLES:
 *
 *  Source:
 *
 *  Name                  Type              IO   Description
 *  -------------         ---------------   --   ------------------------------
 *
 *
 *
 *  EXTERNAL REFERENCES:
 *
 *  Name                Description
 *  ------------------  -------------------------------------------------------
 *
 *
 *
 *  ABNORMAL TERMINATION CONDITIONS, ERROR AND WARNING MESSAGES:
 *  
 *
 *
 *  ASSUMPTION, CONSTRAINTS, RESTRICTIONS:
 *  
 *
 *
 *  NOTES:
 *  
 *
 *
 *  REQUIREMENTS/FUNCTIONAL SPECIFICATION REFERENCES:
 *
 *
 *
 *
 *  DEVELOPMENT HISTORY:
 *
 *  Date         Name(s)         Version  Description
 *  -----------  --------------  -------  -------------------------------------
 *  11-Oct-2002  Francois AMAND  0.0.1    First implementation
 *  24-Feb-2003  G.Leterrier     0.0.2    change include name for mapping definition
 *  25 Feb-2003  G.Leterrier     0.0.3    Change MACRO to take in account reserved bit in registers
 *
 *  ALGORITHM: 
 *
 *
 *****************************************************************************/


#ifndef __SYS_MEMIF_H__
  #define __SYS_MEMIF_H__


#if (CHIPSET == 15 ) /* (CHIPSET == 12) */
  
   /****************************************************************************
     *                            MACRO DEFINITION
     ***************************************************************************/
     
    #define C_MAP_EMIF_BASE              0xFFFFFB00L   // EMIF registers

      /*
     *  Chip-select offset definition
     */
    #define C_EMIF_CS0_OFFSET  0
    #define C_EMIF_CS1_OFFSET  1
    #define C_EMIF_CS2_OFFSET  2
    #define C_EMIF_CS3_OFFSET  3
  
    #define C_EMIF_NB_EXTERNAL_CS  4
    
    #define C_EMIF_CS_SIZE_MAX_MB  32

    #define C_EMIF_LRU_PRIORITY_OFFSET	0x00
    #define C_EMIF_CONF_OFFSET	0x02
    #define C_EMIF_CONF_CSX_OFFSET(cs_index)     (0x04 + 4 * cs_index)
    #define C_EMIF_ADV_CONF_CSX_OFFSET(cs_index)     (0x14 + 2 * cs_index)
    #define C_EMIF_DYNAMIC_WS_OFFSET	0x1c
    #define C_EMIF_TIMEOUT_OFFSET	0x1e
    #define C_EMIF_PROTECT_MODE_OFFSET	0x20
    #define C_EMIF_BOUND_OFFSET	0x22
    #define C_EMIF_MASK_OFFSET	0x26
    #define C_EMIF_ABORT_ADDR_OFFSET	0x28
    #define C_EMIF_ABORT_TYPE_OFFSET	0x2c
    #define C_EMIF_API_RHEA_CTRL_OFFSET 0x2e
    #define C_EMIF_BOOT_MODE_OFFSET	0x30	

    /*
     *  Registers address definition
     */
    #define C_EMIF_LRU_PRIORITY_REG	  	* (volatile SYS_UWORD16 *) (C_MAP_EMIF_BASE + C_EMIF_LRU_PRIORITY_OFFSET)
    #define C_EMIF_CONF_REG	  		* (volatile SYS_UWORD16 *) (C_MAP_EMIF_BASE + C_EMIF_CONF_OFFSET)
    #define C_EMIF_CONF_CSX_REG(cs_index)    * (volatile SYS_UWORD32 *) (C_MAP_EMIF_BASE + C_EMIF_CONF_CSX_OFFSET(cs_index))
    #define C_EMIF_ADV_CONF_CSX_REG(cs_index)   * (volatile SYS_UWORD16 *) (C_MAP_EMIF_BASE + C_EMIF_ADV_CONF_CSX_OFFSET(cs_index))
    #define C_EMIF_DYNAMIC_WS_REG         * (volatile SYS_UWORD16 *) (C_MAP_EMIF_BASE + C_EMIF_DYNAMIC_WS_OFFSET)
    #define C_EMIF_TIMEOUT_REG            * (volatile SYS_UWORD16 *) (C_MAP_EMIF_BASE + C_EMIF_TIMEOUT_OFFSET)
    #define C_EMIF_PROTECT_MODE_REG       * (volatile SYS_UWORD16 *) (C_MAP_EMIF_BASE + C_EMIF_PROTECT_MODE_OFFSET)
    #define C_EMIF_BOUND_REG              * (volatile SYS_UWORD16 *) (C_MAP_EMIF_BASE + C_EMIF_BOUND_OFFSET)
    #define C_EMIF_BOUND_REG1              * (volatile SYS_UWORD16 *) (C_MAP_EMIF_BASE + C_EMIF_BOUND_OFFSET + 2)
    #define C_EMIF_MASK_REG               * (volatile SYS_UWORD16 *) (C_MAP_EMIF_BASE + C_EMIF_MASK_OFFSET)
    #define C_EMIF_ABORT_ADDR_REG         * (volatile SYS_UWORD32 *) (C_MAP_EMIF_BASE + C_EMIF_ABORT_ADDR_OFFSET)
    #define C_EMIF_ABORT_TYPE_REG         * (volatile SYS_UWORD16 *) (C_MAP_EMIF_BASE + C_EMIF_ABORT_TYPE_OFFSET)
    #define C_EMIF_API_RHEA_CTRL_REG         * (volatile SYS_UWORD16 *) (C_MAP_EMIF_BASE + C_EMIF_API_RHEA_CTRL_OFFSET)	
    #define C_EMIF_BOOT_MODE_REG         * (volatile SYS_UWORD16 *) (C_MAP_EMIF_BASE + C_EMIF_BOOT_MODE_OFFSET)	

    /*
     * LRU Priority register definition
     */
    #define C_EMIF_LRU_PRIORITY_MASK    0x0008
    
    /* Bits position in the register */
    #define C_EMIF_MPU_POS              0
    #define C_EMIF_DMA_POS        	4
	
    /*
     * EMIF configuration register definition
     */
    #define C_EMIF_CONF_MASK            0x0000
    
    /* Bits position in the register */
    #define C_EMIF_PWD_EN_POS           0
    #define C_EMIF_PDE_POS        	1
    #define C_EMIF_PREFETCH_MODE_POS    3
    #define C_EMIF_FLUSH_PREFETCH	5
    #define C_EMIF_WRITE_PROTECT_POS	6	

    
    /* Prefetch bit defintion */
    typedef SYS_UWORD8 T_EMIF_PREFETCH_MODE;
    #define C_EMIF_PREFETCH_OFF 		0
    #define C_EMIF_PREFETCH_INSTRUCTION_DATA 	1
    #define C_EMIF_PREFETCH_INSTRUCTION 	2

    /* PDE state*/
    typedef SYS_UWORD8 T_PDE_STATE;
    #define C_EMIF_PDE_DISABLE	0
    #define C_EMIF_PDE_ENABLE	1

    typedef SYS_UWORD8 T_PWD_STATE;
    #define C_EMIF_PWD_DISABLE	0
    #define C_EMIF_PWD_ENABLE	1

    /*
     * CSX Configuration definition
     */
    #define C_EMIF_CONF_CSX_MASK        0x310004
    
    /* Bits position in the register */
    #define C_EMIF_FCLK_DIV_POS         0
    #define C_EMIF_RETIME_POS        	2
    #define C_EMIF_RDWST_POS    	4
    #define C_EMIF_WRWST_POS    	8
    #define C_EMIF_WELEN_POS    	12
    #define C_EMIF_MEMMODE_POS    	16
    #define C_EMIF_BTWST_POS    	22
    
    /* MEMMODE bit defintion*/
    #define C_EMIF_ASYNC_READ_ASYNC_WRITE 	0
    #define C_EMIF_SYNC_READ_ASYNC_WRITE 	1
    #define C_EMIF_SYNC_READ_SYNC_WRITE 	2

    /* FCLK DIVIDER bit defintion*/
    #define C_EMIF_CLK_DIVIDER_1 0
    #define C_EMIF_CLK_DIVIDER_2 1
    #define C_EMIF_CLK_DIVIDER_4 2
    #define C_EMIF_CLK_DIVIDER_6 3
    
    /* Re-time bit defintion*/
    #define C_EMIF_NO_RETIME 	0
    #define C_EMIF_RETIME	1

     /*
     * CSX Configuration definition
     */
    #define C_EMIF_ADV_CONF_CSX_MASK    0x00
    
    /* Bits position in the register */
    #define C_EMIF_OESETUP_POS	        0
    #define C_EMIF_OEHOLD_POS        	4
    #define C_EMIF_ADVHOLD_POS    	8
    #define C_EMIF_BTMODE_POS    	9
    #define C_EMIF_CLKMSK_POS    	10
    #define C_EMIF_RC_POS    		11

    /* Bus turn around wait state type */
    #define C_EMIF_BT_RD_TRANS		0
    #define C_EMIF_BT_RD_WR_TRANS	1

    /* Clock mask type */
    #define C_EMIF_CLK_NO_MASK		0
    #define C_EMIF_CLK_MASK		1

    /* Bit position in Dynamic wait states register */
    #define C_EMIF_DYN_WAIT_CS0_POS     0
    #define C_EMIF_DYN_WAIT_CS1_POS	1
    #define C_EMIF_DYN_WAIT_CS2_POS	2
    #define C_EMIF_DYN_WAIT_CS3_POS	3

    /* Dynamic wait state bit definition*/
    #define C_EMIF_DYN_WAIT_DISABLE 	0
    #define C_EMIF_DYN_WAIT_ENABLE 	1

    /* Bit postion in Abort Type register*/
    #define C_EMIF_ABORT_TYPE_POS     	0
    #define C_EMIF_HOST_ID_POS		1
    #define C_EMIF_PROTECT_MODE_POS	3
    #define C_EMIF_TIMEOUT_ERR_POS	4
    
    #define C_EMIF_ABORT_TYPE_BIT     	0x01
    #define C_EMIF_HOST_ID_BIT		0x06
    #define C_EMIF_PROTECT_MODE_BIT	0x08
    #define C_EMIF_TIMEOUT_ERR_BIT	0x10
    
    #define C_EMIF_ABORT_HOST_MPU 0
    #define C_EMIF_ABORT_HOST_DMA 1

    /* Bit position in Abort timeout register*/
    #define C_EMIF_TIMEOUT_VALUE_POS    0
    #define C_EMIF_TIMEOUT_EN_POS	8
    
    /* Protect mode enable/disable */
    #define C_EMIF_PROTECT_MODE_ENABLE	       1
    #define C_EMIF_PROTECT_MODE_DISABLE	0

    /* Bit mask for Protect bound Address register*/
    #define C_EMIF_PROTECT_BOUND_MASK 0x3ffff
    
    /* Bit position in Mask register*/
    #define C_EMIF_MASK_ADDR_POS	0
    #define C_EMIF_PROTECT_CS_POS	8

    /* Protect chip selects */
    #define C_EMIF_PROTECT_CS0	0
    #define C_EMIF_PROTECT_CS1	1
    #define C_EMIF_PROTECT_CS2	2
    #define C_EMIF_PROTECT_CS3	3

    /* Default settings */
    #define C_EMIF_DEFAULT_MCU_ACCESS	3
    #define C_EMIF_DEFAULT_DMA_ACCESS	4

   /* Bit position in Boot mode configuration*/
    #define C_EMIF_BOOT_CTRL_POS	8
    #define C_EMIF_SECURE_MEM_TYPE_POS	 9
    #define C_EMIF_DEBUG_UNIT_POS	 11	

   /* Bit position in Boot mode configuration*/
    #define C_EMIF_BOOT_CTRL_BIT 0x0100
    #define C_EMIF_SECURE_MEM_TYPE_BIT	 0x0200
    #define C_EMIF_DEBUG_UNIT_BIT	 0x0800	
	
   /* Boot mode configuration */
   /* Boot memory*/
    #define C_EMIF_BOOT_INTERNAL_MEM 0
    #define C_EMIF_BOOT_EXTERNAL_MEM 1

   /* Secure memory */
    #define C_EMIF_SECURE_MEM_MCU_ROM 0
    #define C_EMIF_SECURE_MEM_INTERNAL_SRAM 1   

   /* Debug unit enable/disable */
    #define C_EMIF_ENABLE_DU 0
    #define C_EMIF_DISABLE_DU 1

   /* bit position in API RHEA Control register */
    #define C_RHEA_STROBE0_ACCESS_SIZE_ADAPT_POS	1
    #define C_RHEA_STROBE1_ACCESS_SIZE_ADAPT_POS   3
    #define C_API_ACCESS_SIZE_ADAPT_POS   5
    #define C_EMIF_DEBUG_ENABLE_POS   6

    /* RHEA strobe0 access size adaptation */
    #define C_RHEA_STROBE0_ACCESS_SIZE_ADAPT_DISABLE 0
    #define C_RHEA_STROBE0_ACCESS_SIZE_ADAPT_ENABLE 1

   /* RHEA strobe1 access size adaptation */
    #define C_RHEA_STROBE1_ACCESS_SIZE_ADAPT_DISABLE 0
    #define C_RHEA_STROBE1_ACCESS_SIZE_ADAPT_ENABLE 1

   /* API access size adaptation */
    #define C_API_ACCESS_SIZE_ADAPT_DISABLE 0
    #define C_API_ACCESS_SIZE_ADAPT_ENABLE 1

   /* Debug enable */
    #define C_ARM_DEBUG_DISABLE 0
    #define C_ARM_DEBUG_ENABLE 1
	
   /****************************************************************************
     *                            STRUCTURE DEFINITION
     ***************************************************************************/

   typedef struct {
   
	T_EMIF_PREFETCH_MODE d_prefetch_mode;
       T_PDE_STATE d_pde_enable;
       T_PWD_STATE d_pwd_enable;
	SYS_UWORD8 flush_prefetch;
	SYS_UWORD8 write_protect;
    	}T_EMIF_CONF;
				   
   typedef struct {

      /* CONF_CSx register configuration */
      SYS_UWORD8  d_wait_read_write_trans; /* read to write transition wait cycles */ 
      SYS_UWORD8  d_memmode; /* memory mode */
      SYS_UWORD8  d_we; /* Write enable cycles */
      SYS_UWORD8  d_wait_write; /*Write wait cycles*/
      SYS_UWORD8  d_wait_read; /* Read wait cycles*/
      SYS_UWORD8  d_retime; 
      SYS_UWORD8  d_flash_clk_div; /* Flash clock divider */
      SYS_UWORD8  d_non_full_handshake_mode;
      SYS_UWORD8  d_oe_setup; /* Output enable Setup */
      SYS_UWORD8  d_oe_hold; /* Output enable hold cycles */
      SYS_UWORD8  d_adv_hold; /* Address hold cycles */
      SYS_UWORD8  d_bus_turn_mode; /* Bus turn around mode*/
      SYS_UWORD8  d_clk_mask; 
      SYS_UWORD8  d_ready_configuration;
    } T_EMIF_CS_CONFIG;

  
   typedef struct {
      SYS_UWORD8   d_abort_state; /* Current abort state */
      SYS_UWORD32 d_abort_address; /* Abort address */
      SYS_UWORD8   d_abort_host; /* abort host MCU or DMA */
      SYS_UWORD8   d_abort_protect; /* Protect abort */
      SYS_UWORD8   d_abort_timeout;/* Timeout abort*/
    } T_ABORT_STATUS;

#define FLASH_OFFSET_ADR 0x06000000
#define SHADOW_OFFSET 0x06000000
#define PSRAM_OFFSET 0x00400000
#define INTRAM_OFFSET 0x08000000

#define MEM8(a) *(volatile unsigned char*) (a)
#define MEM16(a) *(volatile unsigned short*) (a)
#define MEM32(a) *(volatile unsigned long int*) (a)


#define PSRAM_OPERATING_MODE 0	// 0 = burst 1 = async
#define PSRAM_LATENCY_COUNTER 3	// latency code 3 = 4 clocks
#define PSRAM_WAIT_POLARITY 0		// 0 = active low, 1 = active high
#define PSRAM_WAIT_CONFIGURATION 1	// 0 = asserted during delay, 1 = asserted before delay
#define PSRAM_CLOCK_CONFIGURATION 1	// 1 = rising edge
#define PSRAM_OUTPUT_IMPEDANCE 0	// 0 = full drive, 1 = low drive
#define PSRAM_BURST_WRAP 0			// 0 = wrap enable, 1 = wrap disabled
#define PSRAM_BURST_LENGTH 2		// 1 = 4 words, 2 = 8 words, 3 = 16 words, 7 = continous

#define EMIF_BASE 0xfffffb00
#define EMIF_CONF_REG 0x0002
#define EMIF_CFG_CS0 0x0004
#define EMIF_CFG_CS3 0x0010
#define EMIF_DYN_WAIT_REG 0x001c
#define EMIF_ADVANCED_CFG_CS0 0x0014
#define EMIF_ADVANCED_CFG_CS3 0x001a

// For mirror-bit flash:
#define FLASH_WAIT_STATE 3	// code 3 = 5 clocks
#define FLASH_RDY_POLARITY 1		// 0 = active low, 1 = active high
#define FLASH_RDY 1	// 0 = asserted before delay, 1 = asserted during delay
#define FLASH_BURST_WRAP 1			// 1 = wrap enable, 0 = wrap disabled
#define FLASH_BURST_LENGTH 2		// 2 = 8 words, 3 = 16 words, 0 = continous

typedef SYS_UWORD16 FLASHDATA;

#define FLASH_OFFSET(b,o)       (*(( (volatile FLASHDATA*)(b) ) + (o)))

#define FLASH_WR(b,o,d)         FLASH_OFFSET((b),(o)) = (d)
#define FLASH_RD(b,o)           FLASH_OFFSET((b),(o))

#define LLD_DEV_MULTIPLIER 0x00000001
#define LLD_DB_READ_MASK   0x0000FFFF
#define LLD_DEV_READ_MASK  0x0000FFFF
#define LLD_UNLOCK_ADDR1   0x00000555
#define LLD_UNLOCK_ADDR2   0x000002AA
#define LLD_BYTES_PER_OP   0x00000002
#define LLD_BUFFER_SIZE    16

#define NOR_UNLOCK_DATA1                 ((FLASHDATA)0xAAAAAAAA)
#define NOR_UNLOCK_DATA2                 ((FLASHDATA)0x55555555)
#define WSXXX_SET_CONFIG_CMD      ((FLASHDATA)0xD0D0D0D0)
/******************************************************************************/

// SET_EMIF_CONF_REG (WP, FLUSH_PREFETCH, PREFETCH_MODE, PDE, PWD_EN)
// Writes the EMIF configuration register:
//  WP : General write-protect of external memory
//  FLUSH_PREFETCH : Flush the prefetch buffer
//  PREFETCH_MODE : 0 Prefetch off, 1 Instruction and data, 2 instruction only, 3 reserved
//  PDE : Global power down enable signal
//  PWD_EN : EMIF power down enable
#define SET_EMIF_CONF_REG(WP, FLUSH_PREFETCH, PREFETCH_MODE, PDE, PWD_EN) {\
	MEM16 (EMIF_BASE + EMIF_CONF_REG) = \
		((WP)<<6)|\
		((FLUSH_PREFETCH)<<5)|\
		((PREFETCH_MODE)<<3)|\
		((PDE)<<1)|\
		(PWD_EN);}

// SET_EMIF_CFG(CS, BTWST, MEMMODE, WELEN, WRWST, RDWST, RT, FCLKDIV)
// Writes the EMIF chip-select registers:
//  CS : Chip-select to configure
//  BTWST : Number of wait-states from read-to-write
//  MEMMODE : 0 async, 1 burst read, 2 burst read/write
//  WELEN : Length of WE pulse
//  WRWST : Write wait states
//  RDWST : Read wait states
//  RT : re-timing
//  FCLKDIV : 0..3 = divide by 1, 2, 4, 6
#define SET_EMIF_CFG(CS, BTWST, MEMMODE, WELEN, WRWST, RDWST, RT, FCLKDIV) {\
	MEM16 (EMIF_BASE + EMIF_CFG_CS0 + 2 + CS*4) =\
		((BTWST) << 6) |\
		(MEMMODE);\
	MEM16 (EMIF_BASE + EMIF_CFG_CS0 + CS*4) =\
		((WELEN) << 12) |\
		((WRWST) << 8) | \
		((RDWST) << 4) | \
		((RT) << 2) |\
		(FCLKDIV);\
			}
			

// SET_EMIF_ADVANCED_CFG(CS, RC, CLKMSK, BTMODE, ADVHOLD, OEHOLD, OESETUP)
// Writes the advanced chip-select register
//  CS : Chip-select to configure
//  RC : 0 Expect ready one clock before data valid, 1 expect ready in same cycle as data
//  CLKMSK : 0 Clock is sent during writes in synchroneous mode, 1 clock is disabled during writes
//  BTMODE : Mode selection for BTWST
//  ADVHOLD : Hold cycle for address valid
//  OEHOLD : Cycles from OE high to CS high
//  OESETUP : Cycles from CS low to OE low
#define SET_EMIF_ADVANCED_CFG(CS, RC, CLKMSK, BTMODE, ADVHOLD, OEHOLD, OESETUP) {\
	MEM16(EMIF_BASE + EMIF_ADVANCED_CFG_CS0 + CS*2) =\
		((RC)<<11)|\
		((CLKMSK)<<10)|\
		((BTMODE)<<9)|\
		((ADVHOLD)<<8)|\
		((OEHOLD)<<4)|\
		(OESETUP);}

// SET_EMIF_NO_DYN_WAIT(CS, DISABLE) 
// Modifies the dynamic wait state register
//  CS : Chip-select to configure
//  DISABLE : 0 dynamic wait enabled, 1 dynamic wait disabled
#define SET_EMIF_NO_DYN_WAIT(CS, DISABLE) 	MEM16 (EMIF_BASE + EMIF_DYN_WAIT_REG) = (MEM16 (EMIF_BASE + EMIF_DYN_WAIT_REG) & ~(1<<CS)) | (DISABLE<<CS);
  
   /****************************************************************************
     *                            PROTOTYPE DEFINITION
     ***************************************************************************/

   SYS_UWORD8 f_emif_set_priority (SYS_UWORD8 d_dma_access, 
		   SYS_UWORD8 d_mpu_access);

   SYS_UWORD8 f_emif_set_conf (T_EMIF_CONF* emif_conf);

   SYS_UWORD8 f_emif_cs_mode (SYS_UWORD8 d_cs, 
		   T_EMIF_CS_CONFIG* p_emif_cs_config);

   SYS_UWORD8 f_emif_abort_conf(SYS_UWORD8  d_timeout_enable, 
		  SYS_UWORD8  d_timeout);

   SYS_UWORD8 f_emif_abort_status (T_ABORT_STATUS* p_abort_status);

   SYS_UWORD8 f_emif_protect_conf (SYS_UWORD32 d_bound_address, 
		   SYS_UWORD32 d_protect_cs, 
		   SYS_UWORD32 d_protect_mask);

   SYS_UWORD8 f_emif_protect_enable (SYS_UWORD8 enable);

   SYS_UWORD8 f_emif_api_rhea_conf (SYS_UWORD8 strobe0_access_size_adapt,
  		SYS_UWORD8 strobe1_access_size_adapt,
  		SYS_UWORD8 api_access_size_adapt,
  		SYS_UWORD8 debug_enable);

  SYS_UWORD8 f_emif_boot_mode_conf (SYS_UWORD8 boot_ctrl, SYS_UWORD8 secure_mem_select);

  SYS_UWORD8 f_emif_debug_unit_enable (SYS_UWORD8 enable);

  #endif /* ( CHIPSET == 15 ) */
  
#endif /* __SYS_MEMIF_H__ */
