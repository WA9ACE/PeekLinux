/*                @(#) nom : sys_map.h SID: 1.2 date : 05/23/03               */
/* Filename:      sys_map.h                                                   */
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
 *  FILE NAME: sys_map.h
 *
 *
 *  PURPOSE:  Memory mapping of the CALYPSO PLUS chip.
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
 *
 *  ALGORITHM: 
 *
 *
 *****************************************************************************/
#if (CHIPSET != 15)
#include "chipset.cfg"
#endif 

#if (CHIPSET == 12)

  #ifndef __SYS_MAP_H__
    #define __SYS_MAP_H__
    
    /****************************************************************************
     *                            STROBE 0 MAPPING
     ***************************************************************************/
  
    #define C_MAP_TPU_BASE                0xFFFF1000L   // CS2  : TPU registers
    #define C_MAP_I2C_BASE                0xFFFF2800L   // CS5  : I2C registers
    #define C_MAP_UWIRE_BASE              0xFFFF4000L   // CS8  : UWIRE registers
    #define C_MAP_UART_IRDA_BASE          0xFFFF5000L   // CS10 : UART IRDA registers
    #define C_MAP_UART_MODEM1_BASE        0xFFFF5800L   // CS11 : UART MODEM 1 registers
    #define C_MAP_UART_UIR_BASE           0xFFFF6000L   // CS12 : UART UIR register
    #define C_MAP_RIF_BASE                0xFFFF7000L   // CS14 : RIF registers
    #define C_MAP_TPU_RAM_BASE            0xFFFF9000L   // CS18 : TPU RAM
    #define C_MAP_DPLL_BASE               0xFFFF9800L   // CS19 : DPLL register
    #define C_MAP_LCD_IF_BASE             0xFFFFA000L   // CS20 : LCD registers
    #define C_MAP_USIM_BASE               0xFFFFA800L   // CS21 : USIM registers
    #define C_MAP_USB_BASE                0xFFFFB000L   // CS22 : USB registers
    #define C_MAP_GEA_BASE                0xFFFFC000L   // CS24 : GEA registers
    #define C_MAP_MMC_SD_BASE             0xFFFFC800L   // CS25 : MMC/SD registers
    #define C_MAP_MS_BASE                 0xFFFFD000L   // CS26 : Memory Stick registers
    #define C_MAP_CPORT_BASE              0xFFFFD800L   // CS27 : C-Port registers
    #define C_MAP_UART_MODEM2_BASE        0xFFFFE000L   // CS28 : UART MODEM 2 registers
    #define C_MAP_DMA_BASE                0xFFFFE800L   // CS29 : DMA registers
    #define C_MAP_WD_TIMER_BASE           0xFFFFF800L   // CS31 : Watchdog TIMER registers
    #define C_MAP_WD_TIMER_SEC_BASE       0xFFFFF880L   // CS31 : Secure Watchdog TIMER registers
    #define C_MAP_RHEA_BASE               0xFFFFF900L   // CS31 : RHEA bridge registers
    #define C_MAP_INTH_BASE               0xFFFFFA00L   // CS31 : INTH registers
    #define C_MAP_INTH_SEC_BASE           0xFFFFFA80L   // CS31 : Secure INTH registers
    #define C_MAP_MEMIF_BASE              0xFFFFFB00L   // CS31 : MEMIF registers
    #define C_MAP_PRRM_BASE               0xFFFFFC00L   // CS31 : PRRM (Protected Ressource Reset Management) registers
    #define C_MAP_CLKM_BASE               0xFFFFFD00L   // CS31 : CLOCKM registers
    #define C_MAP_JTAG_ID_CODE_BASE       0xFFFFFE00L   // CS31 : JTAG ID code registers
    #define C_MAP_EMPU_BASE               0xFFFFFF00L   // CS31 : EMPU registers
  
  
  
    /****************************************************************************
     *                            STROBE 1 MAPPING
     ***************************************************************************/
  
    #define C_MAP_SIM_BASE                0xFFFE0000L   // CS0  : SIM registers
    #define C_MAP_TSP_BASE                0xFFFE0800L   // CS1  : TSP registers
    #define C_MAP_RTC_BASE                0xFFFE1800L   // CS3  : RTC registers
    #define C_MAP_ULPD_BASE               0xFFFE2000L   // CS4  : ULPD registers
    #define C_MAP_SPI_BASE                0xFFFE3000L   // CS6  : SPI registers
    #define C_MAP_TIMER1_BASE             0xFFFE3800L   // CS7  : TIMER1 registers
    #define C_MAP_GPIO_BASE               0xFFFE4800L   // CS9  : GPIO registers
    #define C_MAP_TIMER2_BASE             0xFFFE6800L   // CS13 : TIMER2 registers
    #define C_MAP_LPG_BASE                0xFFFE7800L   // CS15 : LPG registers
    #define C_MAP_PWL_BASE                0xFFFE8000L   // CS16 : PWL registers
    #define C_MAP_PWT_BASE                0xFFFE8800L   // CS17 : PWT registers
    #define C_MAP_KEYBOARD_BASE           0xFFFEB800L   // CS23 : KEYBOARD registers
    #define C_MAP_JTAG_ID_BASE            0xFFFEF000L   // CS30 : JTAG ID register
    #define C_MAP_JTAG_VERSION_BASE       0xFFFEF002L   // CS30 : JTAG Version register
    #define C_MAP_DIE_ID_CODE_BASE        0xFFFEF004L   // CS30 : DIE IDENTIFICATION code registers
    #define C_MAP_HASH_MAN_PUB_KEY_BASE   0xFFFEF00CL   // CS30 : Hash of the Manufacturer Public Key registers
    #define C_MAP_CORE_CONF_BASE          0xFFFEF01CL   // CS30 : Core configuration registers
  
  
  
    /****************************************************************************
     *                          INTERNAL MEMORY MAPPING
     ***************************************************************************/
  
    #define C_MAP_CS6_4MBITS_BASE         0x08000000L   // Main 4Mbits of Internal SRAM
    #define C_MAP_CS6_SHD_RAM0_BASE       0x08080000L   // Shared 0.5Mbits of Internal SRAM
    #define C_MAP_CS6_SHD_RAM1_BASE       0x08090000L   // Shared 0.5Mbits of Internal SRAM
    #define C_MAP_CS6_SHD_RAM2_BASE       0x080A0000L   // Shared 0.5Mbits of Internal SRAM
  
  
  
    /****************************************************************************
     *                        INTERNAL PERIPHERAL MAPPING
     ***************************************************************************/
  
    #define C_MAP_SHA1MD5_BASE            0x09800000L   // SHA1/MD5 registers
    #define C_MAP_DES_BASE                0x09900000L   // DES/3DES registers
    #define C_MAP_RNG_BASE                0x09A00000L   // RNG registers
    #define C_MAP_NAND_FLASH_BASE         0x09D00000L   // Nand Flash Conroller registers
    #define C_MAP_PATCH_UNIT_BASE         0x09E00000L   // Patch Unit registers
    #define C_MAP_DEBUG_UNIT_BASE         0x09F00000L   // Debug Unit registers
  
  
  
    /****************************************************************************
     *                              API MAPPING
     ***************************************************************************/
  
    #define C_MAP_API_RAM_BASE            0xFFD00000L   // API RAM address on MCU side
    #define C_MAP_DSP_API_RAM_BASE        0xE000        // API RAM address on DSP side
    #define C_MAP_APIC_BASE               0xFFE00000L   // APIC register 
  
  
  #endif /* __SYS_MAP_H__ */

#endif /* (CHIPSET == 12) */

#if (CHIPSET == 15)
  #ifndef __SYS_MAP_H__
    #define __SYS_MAP_H__
    
    /****************************************************************************
     *                            STROBE 0 MAPPING
     ***************************************************************************/
  
	/* FIXME: Not Defining DRP MAP */
	#define C_MAP_UART_IRDA_BASE		  0xFFFF7000L	// CS14	: UART Register
	#define C_MAP_UART_SW_BASE			  0XFFFF7280L	// UART Switch

	#define C_MAP_MCSI_BASE				  0xFFFF7800L   // CS15 : MCSI Register
	#define C_MAP_DSW_BASE				  0xFFFF8000L   // CS16 : DSW Register

    #define C_MAP_RHEA_BASE               0xFFFF8800L   // CS17 : RHEA bridge registers
    #define C_MAP_RHEA_APC_SW_BASE		  0xFFFF8800L   // CS17	: RHEA APC Switch
    #define C_MAP_RHEA_MCSI_SW_BASE		  0xFFFF8820L	// CS17	: RHEA MCSI Switch
    #define C_MAP_RHEA_CPORT_SW_BASE	  0XFFFF8840L	// CS17	: RHEA CPORT Switch

	#define C_MAP_TPU_RAM_BASE            0xFFFF9000L   // CS18 : TPU RAM
	#define C_MAP_DPLL_BASE               0xFFFF9800L   // CS19 : DPLL register

    #define C_MAP_LCD_IF_BASE             0xFFFFA000L   // CS20 : LCD registers
    #define C_MAP_USIM_BASE               0xFFFFA800L   // CS21 : USIM registers
    #define C_MAP_USB_BASE                0xFFFFB000L   // CS22 : USB registers
    #define C_MAP_I2C_BASE                0xFFFFB800L   // CS23  : I2C registers
    #define C_MAP_GEA_BASE                0xFFFFC000L   // CS24 : GEA registers

	#define C_MAP_I2C_TRITON_BASE		  0xFFFFC800L	// CS25 : I2C2 registers
    #define C_MAP_CPORT_BASE              0xFFFFD000L   // CS26 : C-Port registers
	#define C_MAP_CPORT_FIFO_BASE		  0xFFFFD800L	// CS27	: C-Port FIFO Base
	#define C_MAP_BCM_BASE				  0xFFFFE000L	// CS28 : BCM registers
	#define C_MAP_DMA_BASE                0xFFFFE800L   // CS29 : DMA registers
	
	#define C_MAP_TPU_BASE				  0xFFFFF000L	// CS30 : TPU registers
    
    #define C_MAP_WD_TIMER_BASE           0xFFFFF800L   // CS31 : Watchdog TIMER registers
    #define C_MAP_WD_TIMER_SEC_BASE       0xFFFFF880L   // CS31 : Secure Watchdog TIMER registers

    #define C_MAP_INTH_BASE               0xFFFFFA00L   // CS31 : INTH registers
    #define C_MAP_INTH_SEC_BASE           0xFFFFFA80L   // CS31 : Secure INTH registers
    #define C_MAP_MEMIF_BASE              0xFFFFFB00L   // CS31 : MEMIF registers
    #define C_MAP_PRRM_BASE               0xFFFFFC00L   // CS31 : PRRM (Protected Ressource Reset Management) registers
    #define C_MAP_CLKM_BASE               0xFFFFFD00L   // CS31 : CLOCKM registers
    #define C_MAP_JTAG_ID_CODE_BASE       0xFFFFFE00L   // CS31 : JTAG ID code registers
    #define C_MAP_EMPU_BASE               0xFFFFFF00L   // CS31 : EMPU registers
    
    /****************************************************************************
     *                            STROBE 1 MAPPING
     ***************************************************************************/
  
    #define C_MAP_TPU2_OCP_BASE           0xFFFE0800L   // CS1  : TPU2 OCP registers
    #define C_MAP_ULPD_BASE               0xFFFE2000L   // CS4  : ULPD registers
    #define C_MAP_TIMER1_BASE             0xFFFE3800L   // CS7  : TIMER1 registers
    #define C_MAP_GPIO_BASE               0xFFFE4800L   // CS9  : GPIO registers
    #define C_MAP_GPIO1_BASE              0xFFFE5000L   // CS10 : GPIO1 registers
    #define C_MAP_GPIO2_BASE              0xFFFE5800L   // CS11 : GPIO2 registers
    #define C_MAP_TIMER2_BASE             0xFFFE6800L   // CS13 : TIMER2 registers
    #define C_MAP_LPG_BASE                0xFFFE7800L   // CS15 : LPG registers
    #define C_MAP_PWL_BASE                0xFFFE8000L   // CS16 : PWL registers
    #define C_MAP_PWT_BASE                0xFFFE8800L   // CS17 : PWT registers
    #define C_MAP_KEYBOARD_BASE           0xFFFEB800L   // CS23 : KEYBOARD registers
    #define C_MAP_JTAG_ID_BASE            0xFFFEF000L   // CS30 : JTAG ID register
    #define C_MAP_JTAG_VERSION_BASE       0xFFFEF002L   // CS30 : JTAG Version register
    #define C_MAP_DIE_ID_CODE_BASE        0xFFFEF004L   // CS30 : DIE IDENTIFICATION (63 to 0)code registers
    #define C_MAP_HASH_MAN_PUB_KEY_BASE   0xFFFEF00CL   // CS30 : Hash of the Manufacturer Public Key registers
    #define C_MAP_CORE_CONF_BASE          0xFFFEF01CL   // CS30 : Core configuration registers
    #define C_MAP_DIE_ID2_CODE_BASE       0xFFFEF040L   // CS30 : DIE IDENTIFICATION (127 to 64)code registers
    #define C_MAP_IO_CONF_BASE			  0xFFFEF100L   // CS30 : IO Configuration Registers.
    #define	C_MAP_SOFT_DEBUG_BASE		  0xFFFEF200L	// CS30 : Software Debug registers.
  
  
  
    /****************************************************************************
     *                          INTERNAL MEMORY MAPPING
     ***************************************************************************/
  
    #define C_MAP_320KBITS_BASE       0x08000000L   // Main 320 Kbits of Internal SRAM
    #define C_MAP_192KBITS_BASE		  0x08050000L   // 192 KB INternal ROM
  
  
  
    /****************************************************************************
     *                        INTERNAL PERIPHERAL MAPPING
     ***************************************************************************/

	#define C_MAP_BOOT_ROM_BASE			  0x09000000L	// Boot ROM
	#define C_MAP_CAM_BASE				  0x09700000L	// Camera Controller Registers
	#define C_MAP_SHA1MD5_BASE            0x09800000L   // SHA1/MD5 registers
    #define C_MAP_DES_BASE                0x09900000L   // DES/3DES registers
    #define C_MAP_RNG_BASE                0x09A00000L   // RNG registers
    #define C_MAP_NAND_FLASH_BASE         0x09D00000L   // Nand Flash Conroller registers
    #define C_MAP_MSSPI_UNIT_BASE         0x09E00000L   // MSSPI registers
    #define C_MAP_DEBUG_UNIT_BASE         0x09F00000L   // Debug Unit registers
  
  
  
    /****************************************************************************
     *                              API MAPPING
     ***************************************************************************/
  
    #define C_MAP_API_RAM_BASE            0xFFD00000L   // API RAM address on MCU side
    #define C_MAP_APIC_BASE               0xFFFFF902L   // APIC register 
    #define C_MAP_RHEA_CNTL_BASE		  0XFFFFF900L   // Rhea Control Base
    
    /* FIXME: Mapping may not be correct */
    #define C_MAP_DSP_API_RAM_BASE        0xE000        // API RAM address on DSP side

  
  
  #endif /* __SYS_MAP_H__ */

#endif /* (CHIPSET == 15) */


