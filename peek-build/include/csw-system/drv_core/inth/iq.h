/******************************************************************************
            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION           
                                                                             
   Property of Texas Instruments -- For  Unrestricted  Internal  Use  Only 
   Unauthorized reproduction and/or distribution is strictly prohibited.  This 
   product  is  protected  under  copyright  law  and  trade  secret law as an 
   unpublished work.  Created 1987, (C) Copyright 1997 Texas Instruments.  All 
   rights reserved.                                                            
                  
                                                           
   Filename         : iq.h

   Description      : Interrupt header

   Project          : drivers

   Author           : pmonteil@tif.ti.com  Patrice Monteil.

   Version number   : 1.24

   Date             : 05/23/03

   Previous delta 	: 12/19/00 14:22:53

   SCCS file      	: /db/gsm_asp/db_ht96/dsp_0/gsw/rel_0/mcu_l1/release_gprs/RELEASE_GPRS/drivers1/common/SCCS/s.iq.h

   Sccs Id  (SID)       : '@(#) iq.h 1.13 01/30/01 10:22:22 '

 
*****************************************************************************/

#include "l1sw.cfg"
#include "board.cfg"
#include "chipset.cfg"

#if (OP_L1_STANDALONE == 0)
  #include "debug.cfg"
#endif
#include "swconfig.cfg"

#include "sys_types.h"
#include "conf/sys_pg_configuration.h"

// Hardware driver library build number 
#define IQ_BUILD      1

#if (CHIPSET != 12 && CHIPSET != 15)
#define WS_MASK     0x001F
                     
#if (BOARD == 35) || (BOARD == 46)
  #define IQ_NUM_INT  28
#elif (CHIPSET == 4)
  #define IQ_NUM_INT     20
#elif ((CHIPSET == 5) || (CHIPSET == 6) || (CHIPSET == 9))
  #define IQ_NUM_INT     25
#elif (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 11)
  #define IQ_NUM_INT     21
#else
  #define IQ_NUM_INT     16
#endif


#define IRQ 0
#define FIQ 1

/*
 * Interrupt bit numbers
 */
#define IQ_WATCHDOG             0  

#define IQ_TIM1                 1
#define IQ_TIM2                 2
#if (((BOARD == 35) || (BOARD == 46)) && (CHIPSET == 10))
  #define IQ_TSP                  3
#endif
#define IQ_FRAME                4
#define IQ_PAGE                 5
#define IQ_SIM                  6
#define IQ_UART_IT              7
#define IQ_ARMIO                8
#define IQ_RTC_TIMER            9
#define IQ_RTC_ALARM            10
#if ((CHIPSET == 4) || (CHIPSET == 5) || (CHIPSET == 6) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 9) || (CHIPSET == 10) || (CHIPSET == 11))
  #define IQ_TGSM               19
#else
  #define IQ_TGSM               10
#endif
#define IQ_ULPD_GAUGING         11
#define IQ_EXT                  12

#if (L1_DYN_DSP_DWNLD == 1)
#if (CHIPSET == 10)
  #define IQ_API  15 
#endif // CHIPSET == 10
#endif // L1_DYN_DSP_DWNLD == 1

#if (OP_L1_STANDALONE == 0)
  #define IQ_SIM_CD               16
#endif

#if ((OP_WCP == 0) || (OP_L1_STANDALONE == 1)) 
#if ((CHIPSET == 4) || (CHIPSET == 5) || (CHIPSET == 6) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 9) || (CHIPSET == 10) || (CHIPSET == 11))
  #define IQ_UART_IRDA_IT       18
#endif
#endif

#if (OP_L1_STANDALONE == 0)
  #if (OP_WCP == 0)
    #define IQ_ICR                  20
  #else
    #define IQ_ICR                  26
  #endif
#endif                

#if ((CHIPSET == 5) || (CHIPSET == 6))
  #define IQ_GEA_IT             24
#elif ((OP_WCP == 0) || (OP_L1_STANDALONE == 1))
  #if ((CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 11))
    #define IQ_GEA_IT             20
  #endif
#endif

#endif  /* chipset != 12 || CHIPSET != 15*/

/**** JTAG ID   ****/
#define SATURN    0xB217
#define HERCRAM   0xB268   	
#define F731782   0xB2B5   // HERCROM OLD
#define F731782B  0xB2B5   // HERCROM 1M REV B
#define F731782A  0xB335   // HERCROM 1M REV A
#define F731950   0xB334   // HERCROM 2M
#if (CHIPSET == 4)
  #define F731787  0xB2AC   // HERCRAM20G
#endif
#if ((CHIPSET == 5) || (CHIPSET == 6))
  #define F741709  0xB393   // HERCROM20G1
#endif
#if (CHIPSET == 9)
  #define F751681 0xB217   // HERCROM200C035
#endif
#if (CHIPSET == 12)
  #if (CHIPSET_PG == CP_PG_F751685A)
    #define F751685A 0xB608   // HERCROM500G2C035, F751685A
  #else
    #define F751997 0xB512   // HERCROM500G2C035
  #endif /* endif (CHIPSET_PG == F751685A) */
#endif
#if (CHIPSET == 15)
	#define F751685A 0xB608 /* FIXME: */
#endif	


unsigned IQ_GetBuild(void);
#if (CHIPSET != 12 && CHIPSET != 15)
  void IQ_SetupInterrupts(void);
  void IQ_Dummy(void);
#endif 
void IQ_TimerHandler(void);     /* Watchdog timer */
void IQ_TimerHandler1(void);        /* timer 1 */
void IQ_TimerHandler2(void);        /* timer 2 */
void IQ_FrameHandler(void);     /* It Handler for TPU Frame IT NUCLEUS TICKS */
void IQ_ApiHandler(void);       /* API It handler */
#if (CHIPSET != 12 && CHIPSET != 15)
  #if (OP_L1_STANDALONE == 0)
    void IQ_IcrHandler32(void);     // 32-bit ICR interrupt handler
  #endif
  void IQ_SetupInterruptEdge(unsigned short irq_num);
  void IQ_SetupInterruptLevel(unsigned short irq_num);
  void IQ_InitWaitState(unsigned short rom, unsigned short ram, unsigned short spy, unsigned short lcd, unsigned short jtag); 
  void IQ_Unmask(unsigned irqNum);
  void IQ_Mask(unsigned irqNum);
  void IQ_MaskAll(void);
#endif
#if (OP_L1_STANDALONE == 0)
#if ((BOARD == 8) || (BOARD == 9) || (BOARD == 40) || (BOARD == 41))
  void IQ_KeypadGPIOHandler (void);
#elif ((BOARD == 35) || (BOARD == 42) || (BOARD == 43) || (BOARD == 45) || (BOARD == 46)||(BOARD==70)||(BOARD==71))
  void IQ_KeypadHandler(void);
#endif
#endif
SYS_UWORD16 IQ_GetJtagId(void);
SYS_UWORD16 IQ_GetDeviceVersion(void);
SYS_BOOL IQ_RamBasedLead(void);
SYS_UWORD16 IQ_GetRevision(void);
void IQ_Gauging_Handler(void);
void IQ_External(void);
void IQ_Rtc_Handler(void);
#if ((CHIPSET == 4) || (CHIPSET == 5) || (CHIPSET == 6) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 9) || (CHIPSET == 10) || (CHIPSET == 11) || (CHIPSET == 12) || (CHIPSET == 15))
  void IQ_RtcA_Handler(void);
  void IQ_GsmTim_Handler(void);
#else
  void IQ_RtcA_GsmTim_Handler(void);
#endif
#if ((CHIPSET == 5) || (CHIPSET == 6) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 11) || (CHIPSET == 12) || (CHIPSET == 15))
//  void IQ_GEA_Handler(void);
#endif

#if ((BOARD == 35) || (BOARD == 46))
  #if (OP_L1_STANDALONE == 0)
    void IQ_IcrHandler32   (void);  // 32-bit ICR interrupt handler
  #endif
#endif

#if (OP_L1_STANDALONE == 0)
  #if (TI_PROFILER == 1)
    void IQ_InitLevel( SYS_UWORD16 inputInt, 
                       SYS_UWORD16 FIQ_nIRQ, 
                       SYS_UWORD16 priority, 
                       SYS_UWORD16 edge );
  #endif
#endif
