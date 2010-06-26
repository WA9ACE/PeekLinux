/******************************************************************************
            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION           
                                                                             
   Property of Texas Instruments -- For  Unrestricted  Internal  Use  Only 
   Unauthorized reproduction and/or distribution is strictly prohibited.  This 
   product  is  protected  under  copyright  law  and  trade  secret law as an 
   unpublished work.  Created 1987, (C) Copyright 1997 Texas Instruments.  All 
   rights reserved.                                                            
                  
                                                           
   Filename       	: mem.h

   Description    	: Header file for the memory interface module

   Project        	: Drivers

   Author         	: proussel@ti.com  Patrick Roussel.

   Version number   : 1.22

   Date             : 08/22/03

   Previous delta 	: 12/19/00 14:24:11

   SCCS file      	: /db/gsm_asp/db_ht96/dsp_0/gsw/rel_0/mcu_l1/release_gprs/RELEASE_GPRS/drivers1/common/SCCS/s.mem.h

   Sccs Id  (SID)       : '@(#) mem.h 1.12 01/30/01 10:22:24 '
 
*****************************************************************************/

#ifndef _MEM_H_
#define _MEM_H_

#include "l1sw.cfg"
#include "chipset.cfg" 
#include "board.cfg"

#include "sys_types.h"

#if (CHIPSET == 15)
#include "sys_map.h"
#endif


#define MEM_APIC_REG	0xffe00000	/* APIC register address */

#define MEM_STR1_ADDR	0xfffe0000	/* Strobe 1 : address */
#define MEM_STR1_CS	32		/* Strobe 1 : number of CS */
#define MEM_STR0_ADDR	0xffff0000	/* Strobe 0 : address */
#if (CHIPSET!=15)
#define MEM_STR0_CS	31		/* Strobe 0 : number of CS */
#else
#define MEM_STR0_CS 32		/* Strobe 0 : number of CS */
#endif

/* FIXME: Not SUre about Current Value */
#define MEM_STR_LENGTH	2048		/* Strobe : length of a CS space */

#if (CHIPSET != 15)
#define MEM_UART_IRDA   0xFFFF5000
#define MEM_UART_MODEM  0xFFFF5800
#else
#define MEM_UART_IRDA   C_MAP_UART_IRDA_BASE
#endif

#if (CHIPSET != 15)
#define MEM_RIF         0xFFFF7000
#endif

#if (OP_L1_STANDALONE == 0)
#if (CHIPSET != 15)
  #define MEM_TCIF        0xFFFEA800
  #define MEM_ICR         0xFFFEB000
#endif

  /**** Generic masks ****/
  #define BIT0            0x00000001L
  #define BIT1            0x00000002L
  #define BIT2            0x00000004L
  #define BIT3            0x00000008L
  #define BIT4            0x00000010L
  #define BIT5            0x00000020L
  #define BIT6            0x00000040L
  #define BIT7            0x00000080L
  #define BIT8            0x00000100L
  #define BIT9            0x00000200L
  #define BIT10           0x00000400L
  #define BIT11           0x00000800L
  #define BIT12           0x00001000L
  #define BIT13           0x00002000L
  #define BIT14           0x00004000L
  #define BIT15           0x00008000L
  #define BIT16           0x00010000L


  #define MEM_DEV_ID0     0xFFFEF000
  #define MEM_DEV_ID1     0xFFFEF002


  // Register read and write macros.
  #define READ_REGISTER_ULONG  ( reg )      ( *(volatile unsigned long * const )( reg ) )
  #define WRITE_REGISTER_ULONG ( reg, val ) (( *(volatile unsigned long * const )( reg ) ) = ( val ))
  #define READ_REGISTER_USHORT ( reg )      ( *(volatile unsigned short * const)( reg ) )
  #define WRITE_REGISTER_USHORT( reg, val ) (( *(volatile unsigned short * const)( reg ) ) = ( val ))
  #define READ_REGISTER_UCHAR  ( reg )      ( *(volatile unsigned char * const )( reg ) )
  #define WRITE_REGISTER_UCHAR ( reg, val ) (( *(volatile unsigned char * const )( reg ) ) = ( val ))
#endif   /* OP_L1_STANDALONE == 0 */

/*
 * DSP Memory shared register
 */
 #if (BOARD == 35)
  #define MEM_DSPMS_0_5_MB_TO_DSP ( 1 )
  #define MEM_INIT_DSPMS(d_share) ( *((volatile unsigned short *) MEM_REG_DSPMS ) = (d_share & 0x0003))
#endif

/**** External memory register ****/
#if (CHIPSET!=15)
#define MEM_TIMER_ADDR	0xfffff800	/* TIMER control register */

#define MEM_RHEA_CNTL	0xfffff900	/* memory RHEA control register */
#define MEM_API_CNTL 	0xfffff902	/* memory API control register */
#define MEM_INTH_ADDR 	0xfffffa00		/* INTH registers addr. */
#define MEM_REG_ADDR 	0xfffffb00		/* memory i/f registers addr. */
#else
#define MEM_TIMER_ADDR	C_MAP_WD_TIMER_BASE /* TIMER control register */
#define MEM_RHEA_CNTL	C_MAP_RHEA_CNTL_BASE		/* memory RHEA control register */
#define MEM_API_CNTL 	C_MAP_APIC_BASE		/* memory API control register */
#define MEM_INTH_ADDR 	C_MAP_INTH_BASE		/* INTH registers addr. */
#define MEM_REG_ADDR 	C_MAP_MEMIF_BASE	/* memory i/f registers addr. */
#endif

#define MEM_ARM_RHEA 	(MEM_RHEA_CNTL+4)	/* memory ARM/RHEA control register */
#define ENHANCED_RHEA_CNTL (MEM_RHEA_CNTL+6)	/* memory ARM/RHEA control register */

#define MEM_REG_nCS0 	(MEM_REG_ADDR + 0) 	/* nCS0 register address */
#define MEM_REG_nCS1 	(MEM_REG_ADDR + 2) 	/* nCS1 register address */
#define MEM_REG_nCS2 	(MEM_REG_ADDR + 4) 	/* nCS2 register address */
#define MEM_REG_nCS3 	(MEM_REG_ADDR + 6) 	/* nCS3 register address */

#if ((CHIPSET == 3) || (CHIPSET == 5) || (CHIPSET == 6) || (CHIPSET == 9))
  #define MEM_REG_nCS4 	(MEM_REG_ADDR + 8) 	/* nCS4 register address */
  #define MEM_REG_nCS5 	(MEM_REG_ADDR + 0xa) 	/* nCS5 register address */
  #define MEM_REG_nCS6 	(MEM_REG_ADDR + 0xc) 	/* nCS6 register address */
#elif ((CHIPSET == 4) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 11))
  #define MEM_REG_nCS4 	(MEM_REG_ADDR + 0xa) 	/* nCS4 register address */
  #define MEM_REG_nCS6 	(MEM_REG_ADDR + 0xc) 	/* nCS6 register address */
  #define MEM_REG_nCS7 	(MEM_REG_ADDR + 0x8) 	/* nCS7 register address */
#else
  #if ((CHIPSET != 12) && (CHIPSET != 15))
    #define MEM_REG_nCS4 	(MEM_REG_ADDR + 8) 	  /* nCS4 register address */
    #define MEM_REG_nCS5 	(MEM_REG_ADDR + 0xa) 	/* nCS5 register address */
  #endif
#endif

#if ((BOARD == 35 ) || (BOARD == 46))
  #define MEM_REG_DSPMS   (MEM_REG_ADDR + 0x2E) /* DSPMS register address */
#endif
#define MEM_CTRL_REG	(MEM_REG_ADDR + 0xe) 	/* Control register address */

#if (CHIPSET != 12)
  #define MEM_DMA_ADDR 	0xfffffc00		/* DMA controller reg. addr. */
  #define MEM_CLKM_ADDR      0xfffffd00            /* CLKM registers addr. */
#endif

#if ((CHIPSET == 4) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET  ==  11))
  #define MEM_DPLL_ADDR   0xffff9800   /* DPLL control register */
#endif

#if (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET  ==  11)
  #define MEM_MPU_ADDR    0xFFFFFF00   /* Base address of MPU module */
#endif

#if (CHIPSET !=15)
#define RTC_XIO_START   0xfffe1800
#endif

#if (OP_L1_STANDALONE == 0)
  #define ARM_CONF_REG    0xFFFEF006
#endif

    #define MEM_SIM         0xFFFE0000
    #define MEM_TSP         0xFFFE0800
    #define MEM_TPU_REG     0xFFFE1000
    #define MEM_TPU_RAM     0xFFFE1400
    #define MEM_RTC         0xFFFE1800
    #define MEM_ULPD        0xFFFE2000
    //#define MEM_SPI         0xFFFE3000
    #define MEM_TIMER1      0xFFFE3800
    #define MEM_UWIRE       0xFFFE4000
    #define MEM_ARMIO       0xFFFE4800
#if (CHIPSET == 15)
    #define MEM_SPI          0x09E00000
    #define MEM_ARMIO1       0xFFFE5000 /* GPIO1 */
    #define MEM_ARMIO2       0xFFFE5800 /* GPIO2 */	
#endif
    #define MEM_TIMER2      0xFFFE6800
    #define MEM_LPG         0xFFFE7800
    #define MEM_PWL         0xFFFE8000
    #define MEM_PWT         0xFFFE8800
    #define MEM_JTAGID_PART	0xFFFEF000		/* JTAG ID code register */
    #define MEM_JTAGID_VER	0xFFFEF002		/* JTAG ID code register */
#if((CHIPSET != 12) && (CHIPSET!=15))
    #define MEM_IO_SEL      0xFFFEF00A
#endif

#if ((CHIPSET != 12) && (CHIPSET!=15))
/**** External memory register ****/

#define MEM_REG_WS 	0x001f 		/* number of wait states */
#define MEM_REG_DVS 0x0060 		/* device size */
#define MEM_REG_WE 	0x0080 		/* write enable */
#define MEM_REG_BIG 0x0100 		/* big endian */

#define MEM_DVS_8 	0 		/* device size = 8 bits */
#define MEM_DVS_16 	1  		/* device size = 16 bits */
#define MEM_DVS_32 	2		/* device size = 32 bits */


#define MEM_WRITE_DIS 	0  		/* write disable */
#define MEM_WRITE_EN 	1  		/* write enable */

#define MEM_LITTLE 	0  		/* little endian */
#define MEM_BIG 	1  		/* big endian */

#define MEM_NO_ADAPT 0  		/* no memory adaptation */
#define MEM_ADAPT 	 1  		/* memory adaptation */

/**** Memory control register ****/

#define MEM_CNTL_0_BIG    0x01          /* Big Endian for strobe 0 */
#define MEM_CNTL_0_ADAP   0x02          /* size adaptation for strobe 0 */
#define MEM_CNTL_1_BIG    0x04          /* Big Endian for strobe 1 */
#define MEM_CNTL_1_ADAP   0x08          /* size adaptation for strobe 1 */
#define MEM_CNTL_API_BIG  0x10          /* Big Endian for API */
#define MEM_CNTL_API_ADAP 0x20          /* size adaptation for API */
#define MEM_CNTL_DBG      0x40          /* debug */
#endif /* (CHIPSET != 12) && (CHIPSET !=15)*/

#if (CHIPSET == 4) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 11)
// for CHIPSET 12 and 15, the structure is defined in sys_memif.h, with some added parameters
    typedef struct {

      /* CONF_CSx register configuration */
      SYS_UWORD8  d_ws;
      SYS_UWORD8  d_dvs;
      SYS_UWORD8  d_we;
      SYS_UWORD8  d_dc;

    } T_MEMIF_CS_CONFIG;
#endif

#define ARM_CLK_SRC 0x04
#define ARM_MCLK_DIV 0x30
#define TPU_CLK_ENABLE 0x400

#if ((CHIPSET != 12)&&(CHIPSET!=15))
  #define ASIC_CONF         0xfffef008
#endif


// duplicate definition with MEM_ARMIO !!
//#define ARMIO_ADDR       0xfffe4800

/**** Config registers  ****/
#if ((CHIPSET != 12) && (CHIPSET!=15))
  #define QUARTZ_REG        0xfffef00c


  #define MEM_INIT_CS0(d_ws, d_dvs, d_we, d_dc) ( \
  *((volatile SYS_UWORD16 *) MEM_REG_nCS0 ) = (d_ws | (d_dvs << 5) | (d_we << 7) | (d_dc << 9)))

  #define MEM_INIT_CS1(d_ws, d_dvs, d_we, d_dc) ( \
  *((volatile SYS_UWORD16 *) MEM_REG_nCS1 ) = (d_ws | (d_dvs << 5) | (d_we << 7) | (d_dc << 9)))

  #define MEM_INIT_CS2(d_ws, d_dvs, d_we, d_dc) ( \
  *((volatile SYS_UWORD16 *) MEM_REG_nCS2 ) = (d_ws | (d_dvs << 5) | (d_we << 7) | (d_dc << 9)))

  #define MEM_INIT_CS3(d_ws, d_dvs, d_we, d_dc) ( \
  *((volatile SYS_UWORD16 *) MEM_REG_nCS3 ) = (d_ws | (d_dvs << 5) | (d_we << 7) | (d_dc << 9)))

  #define MEM_INIT_CS4(d_ws, d_dvs, d_we, d_dc) ( \
  *((volatile SYS_UWORD16 *) MEM_REG_nCS4 ) = (d_ws | (d_dvs << 5) | (d_we << 7) | (d_dc << 9)))

  #if ((CHIPSET == 3) || (CHIPSET == 4) || (CHIPSET == 5) || (CHIPSET == 6) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 9) || (CHIPSET == 10) || (CHIPSET == 11))
    #define MEM_INIT_CS6(d_ws, d_dvs, d_we, d_dc) ( \
    *((volatile SYS_UWORD16 *) MEM_REG_nCS6 ) = (d_ws | (d_dvs << 5) | (d_we << 7) | (d_dc << 9)))
  #endif

  #if ((CHIPSET == 4) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 11))
    #define MEM_INIT_CS7(d_ws, d_dvs, d_we, d_dc) ( \
    *((volatile SYS_UWORD16 *) MEM_REG_nCS7 ) = (d_ws | (d_dvs << 5) | (d_we << 7) | (d_dc << 9)))
  #endif

#endif /* endif ((CHIPSET != 12) && (CHIPSET!=15))*/



#if (CHIPSET == 12)
  #include "sys_map.h"
  
  #define MEM_UART_MODEM2  C_MAP_UART_MODEM2_BASE
  #define MEM_TIMER_SEC_ADDR     C_MAP_WD_TIMER_SEC_BASE  /* TIMER Secure control register */
  #define MEM_MPU_ADDR     C_MAP_EMPU_BASE            /* Base address of MPU module */
  #define MEM_KEYBOARD     C_MAP_KEYBOARD_BASE
  #if (OP_L1_STANDALONE == 0)
    #define MEM_MMC_SD_IO    C_MAP_MMC_SD_BASE

    /* Address of the Random Number Generator */
    #define MEM_RNG             0x09A00000
    /* Address of the DES/3DES */
    #define MEM_DES             0x09900000
  #endif   /* OP_L1_STANDALONE == 0 */
#endif   /* CHIPSET == 12 */

#if (CHIPSET == 15)
  #define MEM_UART_MODEM_IRDA   C_MAP_UART_IRDA_BASE/* Base address of UART?IrDA module */
  /* Address of the Random Number Generator */
  #define MEM_RNG             C_MAP_RNG_BASE
  /* Address of the DES/3DES */
  #define MEM_DES             C_MAP_DES_BASE
  #define MEM_TIMER_SEC_ADDR  C_MAP_WD_TIMER_SEC_BASE  /* TIMER Secure control register */
  #define MEM_MPU_ADDR     C_MAP_EMPU_BASE            /* Base address of MPU module */
  #define MEM_KEYBOARD     C_MAP_KEYBOARD_BASE
#endif

#endif /* _MEM_H_ */

