/*******************************************************************************
            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION

  Property of Texas Instruments -- For  Unrestricted  Internal  Use  Only
   Unauthorized reproduction and/or distribution is strictly prohibited.  This
   product  is  protected  under  copyright  law  and  trade  secret law as an
   unpublished work.  Created 1987, (C) Copyright 1997 Texas Instruments.  All
   rights reserved.


   Filename       	: ulpd.h

   Description    	: Header for HYPERION/ULPD module tests
			  Target : Arm

   Project        	: Hyperion

   Author         	: smunsch@tif.ti.com  Sylvain Munsch.

   Version number	: 1.11

   Date and time	: 12/20/00 10:17:22

   Previous delta 	: 12/06/00 17:31:50

   SCCS file      	: /db/gsm_asp/db_ht96/dsp_0/gsw/rel_0/mcu_l1/release_gprs/mod/emu_p/EMU_P/drivers1/common/SCCS/s.ulpd.h

   Sccs Id  (SID)       : '@(#) ulpd.h 1.11 12/20/00 10:17:22 '


*****************************************************************************/

#ifndef _WINDOWS
   #include "chipset.cfg"
   #include "board.cfg"
   #include "rf.cfg"
#endif

#include <limits.h>
#include <float.h>

// SLEEP MODES
//=======================
#define DO_NOT_SLEEP          00
#define FRAME_STOP            01 // little BIG SLEEP (CUST5...)
#define CLOCK_STOP            02 // Deep sleep


// ULPD registers address 
//=======================

#define ULPD_XIO_START               0xfffe2000

#define ULPD_INC_FRAC_REG             (SYS_UWORD16 *)(ULPD_XIO_START)
#define ULDP_INC_SIXTEENTH_REG       ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x02))
#define ULDP_SIXTEENTH_START_REG     ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x04))
#define ULDP_SIXTEENTH_STOP_REG      ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x06))
#define ULDP_COUNTER_32_LSB_REG      ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x08))
#define ULDP_COUNTER_32_MSB_REG      ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x0A))
#define ULDP_COUNTER_HI_FREQ_LSB_REG ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x0C))
#define ULDP_COUNTER_HI_FREQ_MSB_REG ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x0E))
#define ULDP_GAUGING_CTRL_REG        ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x10))
#define ULDP_GAUGING_STATUS_REG      ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x12))
#define ULDP_GSM_TIMER_CTRL_REG      ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x14))
#define ULDP_GSM_TIMER_INIT_REG      ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x16))
#define ULDP_GSM_TIMER_VALUE_REG     ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x18))
#define ULDP_GSM_TIMER_IT_REG        ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x1A))
#define ULDP_SETUP_CLK13_REG         ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x1C))
#define ULDP_SETUP_SLICER_REG        ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x1E))
#define ULDP_SETUP_VTCXO_REG         ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x20))
#define ULDP_SETUP_FRAME_REG         ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x22))
#define ULPD_SETUP_RF_REG            ((SYS_UWORD16 *)(ULPD_XIO_START  + 0x24))

#if (CHIPSET == 15)
#define ULPD_DCXO_SETUP_SLEEPN       ((SYS_UWORD8 *)(ULPD_XIO_START  + 0x26))
#define ULPD_DCXO_SETUP_SYSCLKEN     ((SYS_UWORD8 *)(ULPD_XIO_START  + 0x28))

#define SETUP_SLEEPZ         0 
#define SETUP_SYSCLKEN       255  // For Locosto assuming DCXO delay for DRP of 7.8 ms 
				  // The value needs to be greater than 255 32 KHz clocks

#endif


// ULPD gauging control register description
//==========================================

#define ULDP_GAUGING_EN	              0x0001   // Gauging is running
#define ULDP_GAUGING_TYPE_HF          0x0002   // Gauging versus HFclock
#define ULDP_SEL_HF_PLL               0x0004   // High freq clock = PLL DSP

// ULPD gauging status register description 
//==========================================

#define ULDP_IT_GAUGING               0x0001  // Interrupt it_gauging occurence
#define ULDP_OVF_HF                   0x0002  // Overflow on the HF counter
#define ULDP_OVF_32                   0x0004  // Overflow on the 32 Khz counter

// WAKEup time
//==========================================
//  the setup time unit is the number of 32 Khz clock periods

/* FIXME : Put Correct Values for Board 70 and 71*/
//The values for the setup time for LOCOSTO or UPPCOSTO board
//These values are currently set to zero. They would change once the
// right values are decided.

#if (BOARD == 34)

#define SETUP_RF                      75      // adujstement time to minimize big_sleep duration
                                              // The SETUP_RF value must be used to delay as much as possible the true  
                                              // start time of the deep_sleep wake-up sequence for power consumption saving. 
                                              // This is required because the unit of the SETUP_FRAME counter is the  
                                              // GSM TDMA frame and not a T32K time period.

#define SETUP_VTCXO                   320     // The setup_vtcxo is the time the external RF device takes to deliver
                                              // stable signals to the VTCXO


#define SETUP_SLICER                  180     // The setup_slicer is the time that the vtcxo takes to deliver 
                                              // a stable output when vtcxo is enabled : usually 2 to 5ms
                                              // The SETUP_SLICER value should be smaller than 160(=4,8ms) but this 
                                              // parameter is directly related to the VTCXO device used in the phone
                                              // and consequently must be retrieved from the VTCXO data-sheet.

#define SETUP_CLK13                   31      // The setup_clk13 is time that the slicer takes to deliver
                                              // a stable output when slicer is enabled : max conservative value 1ms

#else

#if(CHIPSET == 15)
#define SETUP_RF					64  // For Locosto - assuming DCXO Delay for DRP is 7.8 ms


#else

#define SETUP_RF                      0       // adujstement time to minimize big_sleep duration
                                              // The SETUP_RF value must be used to delay as much as possible the true  
                                              // start time of the deep_sleep wake-up sequence for power consumption saving. 
                                              // This is required because the unit of the SETUP_FRAME counter is the  
                                              // GSM TDMA frame and not a T32K time period.
#endif

#if (CHIPSET == 2)
  #define SETUP_VTCXO                 31      // The setup_vtcxo is the time the external RF device takes to deliver
#else 
	#if (CHIPSET == 15)
	   #define SETUP_VTCXO				190    // For Locosto - Assuming DCXO Delay is 7.8 ms 
								// 
	#else                                        // stable signals to the VTCXO
	   #define SETUP_VTCXO                 1114    // 34 ms for ABB LDO stabilization before 13MHz switch ON    
	#endif
#endif  

#if(CHIPSET == 15)
		#define SETUP_SLICER              180 // For Locosto - assuming DCXO Delay for DRP is 7.8 ms
#else
	#if (BOARD == 35) || (BOARD == 40) || (BOARD == 41) || (BOARD == 42) || (BOARD == 43) || (BOARD == 45) || (BOARD == 46)
	  #if (RF_FAM==12)
		#define SETUP_SLICER              660
	  #else
		#define SETUP_SLICER              330     // about 10 ms required for VCXO stabilization
	  #endif
	#else
	  #define SETUP_SLICER                180     // The setup_slicer is the time that the vtcxo takes to deliver 
												  // a stable output when vtcxo is enabled : usually 2 to 5ms
												  // The SETUP_SLICER value should be smaller than 160(=4,8ms) but this 
												  // parameter is directly related to the VTCXO device used in the phone
												  // and consequently must be retrieved from the VTCXO data-sheet.
	#endif
#endif


#if(CHIPSET == 15)
	#define SETUP_CLK13                   63  // For Locosto - assuming DCXO Delay for DRP is 7.8 ms
#else
	#define SETUP_CLK13                   31      // The setup_clk13 is time that the slicer takes to deliver
                                              // a stable output when slicer is enabled : max conservative value 1ms
#endif

#endif // BOARD

// SETUP_FRAME:
//-------------
// CF. Reference document: ULYS015 v1.1 page 24
// 1) Nominal Frequency = 32.768 Khz => 0.03051757 ms
//    (0.03051757 ms / 4.615 ms) =  0.006612692 Frames
// 2) The use of the RFEN signal is optional. It is necessary if the VTCXO function
//    is part of an RF IC which must be first powered before enabling the VTCXO.
//    However it can be use for any other purpose.
// 3) The term (1-DBL_EPSILON) corresponds to the rounding up of SETUP_FRAME.
#ifndef DBL_EPSILON
 #define DBL_EPSILON 0  // This value is compiler dependant L1_MCU-ENH-17967
#endif
 #define SETUP_FRAME  ((( SETUP_RF+SETUP_VTCXO+SETUP_SLICER+SETUP_CLK13)*0.006612692)+(1-DBL_EPSILON))

#define MAX_GSM_TIMER                 65535   // max duration for the wake up timer


// Default values for Cell selection and CS_MODE0
//===============================================
#if (CODE_VERSION == SIMULATION)
#define DEFAULT_HFMHZ_VALUE (13000000*l1_config.dpll)
#else
#define DEFAULT_HFMHZ_VALUE (13000000/* ENABLE_LATER *l1_config.dpll */)
#endif
#define DEFAULT_32KHZ_VALUE (32768)   // real value 32768.29038 hz
//with l1ctl_pgm_clk32(DEFAULT_HFMHZ_VALUE,DEFAULT_32KHZ_VALUE) and dpll = 65Mhz
//          => DEFAULT_INCSIXTEEN           132
//          => DEFAULT_INCFRAC              15915 





// ULPD GSM timer control register description
//============================================

#define ULDP_TM_LOAD                  0x0001 // load the timer with init value
#define ULDP_TM_FREEZE                0x0002 // 1=> GSM timer is frozen
#define ULPD_IT_TIMER_GSM             0x0001 // Interrupt timer occurrence


//  ULDP_INCFRAC_UPDATE : update INCFRAC (16 bits)
//================================================
#define ULDP_INCFRAC_UPDATE(frac)  (* (volatile SYS_UWORD16 *)ULPD_INC_FRAC_REG = frac)


//  ULDP_INCSIXTEEN_UPDATE : update INCSIXTEEN (12 bits)
//======================================================
#define ULDP_INCSIXTEEN_UPDATE(inc)  (* (volatile SYS_UWORD16 *)ULDP_INC_SIXTEENTH_REG = inc)


// ULDP_GAUGING_RUN : Start the gauging
//=====================================
#define ULDP_GAUGING_RUN  (* (volatile SYS_UWORD16 *)ULDP_GAUGING_CTRL_REG |= ULDP_GAUGING_EN)


// ULDP_GAUGING_STATUS : Return if it gauging occurence
//======================================================
#define ULDP_GAUGING_STATUS ((* (volatile SYS_UWORD16 *) ULDP_GAUGING_STATUS_REG) & ULDP_GAUGING_EN )

// ULDP_GAUGING_STOP : Stop the gauging
//=====================================
#define ULDP_GAUGING_STOP (* (volatile SYS_UWORD16 *) ULDP_GAUGING_CTRL_REG &= ~ULDP_GAUGING_EN) 

// ULDP_GAUGING_START : Stop the gauging
//=====================================
#define ULDP_GAUGING_START (* (volatile SYS_UWORD16 *) ULDP_GAUGING_CTRL_REG |= ULDP_GAUGING_EN) 

// ULDP_GAUGING_SET_HF : Set the gauging versus HF clock
//======================================================
#define ULDP_GAUGING_SET_HF (* (volatile SYS_UWORD16 *) ULDP_GAUGING_CTRL_REG |= ULDP_GAUGING_TYPE_HF)

//  ULDP_GAUGING_HF_PLL : Set the gauging HF versus PLL clock
//===========================================================
#define ULDP_GAUGING_HF_PLL (* (volatile SYS_UWORD16 *) ULDP_GAUGING_CTRL_REG |= ULDP_SEL_HF_PLL)


//  ULDP_GET_IT_GAG : Return if the interrupt it gauging occurence
//================================================================
#define ULDP_GET_IT_GAG ((* (volatile SYS_UWORD16 *) ULDP_GAUGING_STATUS_REG) & ULDP_IT_GAUGING )

//  ULDP_GET_OVF_HF : Return overflow occured on the HF counter
//=============================================================
#define ULDP_GET_OVF_HF (((* (volatile SYS_UWORD16 *) ULDP_GAUGING_STATUS_REG) & ULDP_OVF_HF)>>1)

//  ULDP_GET_OVF_32 : Return overflow occured on the 32 counter
//=============================================================
#define ULDP_GET_OVF_32 (((* (volatile SYS_UWORD16 *) ULDP_GAUGING_STATUS_REG) & ULDP_OVF_32)>>2)

//  ULDP_TIMER_INIT : Load the  timer_init value
//=========================================================
#define ULDP_TIMER_INIT(value) ((* (volatile SYS_UWORD16 *)ULDP_GSM_TIMER_INIT_REG) = value)

//  READ_ULDP_TIMER_INIT : Read the  timer_init value
//=========================================================
#define READ_ULDP_TIMER_INIT (* (volatile SYS_UWORD16 *)ULDP_GSM_TIMER_INIT_REG)

//  READ_ULDP_TIMER_VALUE : Read the  timer_init value
//=========================================================
#define READ_ULDP_TIMER_VALUE (* (volatile SYS_UWORD16 *)ULDP_GSM_TIMER_VALUE_REG)

//  ULDP_TIMER_LD : Load the timer with timer_init value
//=========================================================
#define ULDP_TIMER_LD ((* (volatile SYS_UWORD16 *)ULDP_GSM_TIMER_CTRL_REG) |= ULDP_TM_LOAD)

//  ULDP_TIMER_FREEZE : Freeze the timer
//=========================================================
#define ULDP_TIMER_FREEZE ((* (volatile SYS_UWORD16 *)ULDP_GSM_TIMER_CTRL_REG) |= ULDP_TM_FREEZE)

//  ULDP_GSM_TIME_START : Run the GSM timer
//=========================================
#define ULDP_TIMER_START ((* (volatile SYS_UWORD16 *)ULDP_GSM_TIMER_CTRL_REG) &= ~ULDP_TM_FREEZE)

//  ULDP_GET_IT_TIMER : Return the it GSM timer occurence
//===========================================================
#define ULDP_GET_IT_TIMER ((* (volatile SYS_UWORD16 *) ULDP_GSM_TIMER_IT_REG) & ULPD_IT_TIMER_GSM )


