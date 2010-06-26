/******************************************************************************
            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION

   Property of Texas Instruments -- For  Unrestricted  Internal  Use  Only
   Unauthorized reproduction and/or distribution is strictly prohibited.  This
   product  is  protected  under  copyright  law  and  trade  secret law as an
   unpublished work.  Created 1987, (C) Copyright 1997 Texas Instruments.  All
   rights reserved.


   Filename         : niq32.c

   Description      : Nucleus IQ initializations

   Project          : Drivers

   Author           : proussel@ti.com  Patrick Roussel.

   Version number   : 1.25

   Date             : 08/22/03

   Previous delta   : 12/19/00 14:24:51

*******************************************************************************/

#include "l1sw.cfg"

#include "chipset.cfg"
#include "board.cfg"
#include "rf.cfg"

#include "l1_confg.h"
#if(OP_L1_STANDALONE == 0)
  #include "debug.cfg"
  #include "rv/rv_defined_swe.h"
  #include "rtc/board/rtc_config.h"
#else
  #include "l1_macro.h"
#endif

#include "sys_types.h"

#if (CHIPSET == 12 || CHIPSET == 15)
  #include "inth/sys_inth.h"
#else
  #include "inth/inth.h"
  #include "memif/mem.h"
  #if (OP_L1_STANDALONE == 1)
    #include "uart/serialswitch_core.h"
  #else
    #include "uart/serialswitch.h"
  #endif

  #if (OP_L1_STANDALONE == 0)
    #include "sim/sim.h"
  #endif
#endif

#if (ANLG_FAM!=11)
#include "abb/abb_core_inth.h"    // for External Interrupt
#endif

#define IQ_H
#include "inth/iq.h"
#include "ulpd/ulpd.h"

#if (CHIPSET == 10)
#if(L1_DYN_DSP_DWNLD == 1)
  #include "l1_api_hisr.h"
#endif // if L1_DYN_DSP_DWNLD == 1
#endif // if CHIPSET == 10

#if (GSM_IDLE_RAM != 0)
  #if (OP_L1_STANDALONE == 1)
    #include "csmi_simul.h"
  #else
    #include "csmi/sleep.h"
  #endif
#endif

#if (OP_L1_STANDALONE==0)
#if (defined RVM_DAR_SWE) && (_GSM==1)
  extern void dar_watchdog_reset(void);
#endif
#endif

#if ((BOARD == 8) || (BOARD == 9) || (BOARD == 40) || (BOARD == 41) || (BOARD == 42) || (BOARD == 43) || (BOARD == 45))
  #include "armio/armio.h"
  #if (OP_L1_STANDALONE == 0)
    #include "uart/uartfax.h"
  #endif
#endif

/* External declaration */
extern void GAUGING_Handler(void);
extern void TMT_Timer_Interrupt(void);
#if (OP_L1_STANDALONE == 1)
  extern void TM_Timer1Handler(void);
#endif
extern void kpd_key_handler(void);
extern void TP_FrameIntHandler(void);

#if (GSM_IDLE_RAM != 0)
  #include "l1_types.h"
  #if TESTMODE
    #include "l1tm_defty.h"
  #endif // TESTMODE
  #if (AUDIO_TASK == 1)
    #include "l1audio_const.h"
    #include "l1audio_cust.h"
    #include "l1audio_defty.h"
  #endif // AUDIO_TASK

  #include "l1_const.h"

  #if (L1_MP3 == 1)
    #include "l1mp3_defty.h"
  #endif

  #if (L1_GTT == 1)
    #include "l1gtt_defty.h"
  #endif

  #include "l1_defty.h"

  extern T_L1S_GLOBAL  l1s;

  #if (CODE_VERSION != SIMULATION)
    #if (CHIPSET == 10) && (OP_WCP == 1)
      void ext_ram_inth_handler(void);  
    #endif
  #endif
#endif

#if (OP_L1_STANDALONE == 0)
  #if (defined RVM_MPM_SWE)
   extern void MPM_InterruptHandler(void);
  #endif

  #if (TI_PROFILER == 1)
    extern void ti_profiler_tdma_action(void);
  #endif

  extern void RTC_GaugingHandler(void);
  extern void RTC_ItTimerHandle(void);
  extern void RTC_ItAlarmHandle(void);
#endif

#if (L1_STEREOPATH == 1)
  extern void l1_audio_api_handler(void);
#endif
extern void l1_api_handler(void);

/* Global variables */
unsigned IQ_TimerCount1;   /* Used to check if timer is incrementing */
unsigned IQ_TimerCount2;   /* Used to check if timer is incrementing */
unsigned IQ_TimerCount;    /* Used to check if timer is incrementing */
unsigned IQ_DummyCount;    /* Used to check if dummy IT */
unsigned IQ_FrameCount;    /* Used to check if Frame IT TPU*/
unsigned IQ_GsmTimerCount; /* Used to check if GSM Timer IT */
//sundi: add a new counter.
unsigned IQ_APICount = 0;


#if (CHIPSET != 12 && CHIPSET != 15 ) 
/*--------------------------------------------------------------*/
/*    irqHandlers                                               */
/*--------------------------------------------------------------*/
/* Parameters :none                                             */
/* Return     : none                                            */
/* Functionality :  Table of interrupt handlers                 */
/* These MUST be 32-bit entries                                 */
/*--------------------------------------------------------------*/

SYS_FUNC irqHandlers[IQ_NUM_INT] = 
{
   IQ_TimerHandler,        /* Watchdog timer */
   IQ_TimerHandler1,       /* timer 1 */
   IQ_TimerHandler2,       /* timer 2 */
   IQ_Dummy,               /* AIRQ 3 */   
   IQ_FrameHandler,        /* TPU Frame It AIRQ 4 */
   IQ_Dummy,               /* AIRQ 5 */
#if (OP_L1_STANDALONE == 0)
   SIM_IntHandler,         /* AIRQ 6 */
#else
   IQ_Dummy,               /* AIRQ 6 */
#endif
#if ((CHIPSET == 2) || (CHIPSET == 3))
   SER_uart_handler,       /* AIRQ 7 */
#elif ((CHIPSET == 4) || (CHIPSET == 5) || (CHIPSET == 6) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 9) || (CHIPSET == 10) || (CHIPSET == 11))
   SER_uart_modem_handler, /* AIRQ 7 */
#endif
#if (OP_L1_STANDALONE == 0)
#if ((BOARD == 8) || (BOARD == 9) || (BOARD == 40) || (BOARD == 41))
   IQ_KeypadGPIOHandler,   /* AIRQ 8 */
#else
   IQ_KeypadHandler,       /* AIRQ 8 */
#endif
#else
   IQ_Dummy,               /* AIRQ 8 */
#endif /* OP_L1_STANDALONE */
   IQ_Rtc_Handler,         /* AIRQ 9 RTC Timer*/
#if ((CHIPSET == 2) || (CHIPSET == 3))
   IQ_RtcA_GsmTim_Handler, /* AIRQ 10 RTC ALARM OR ULPD GSM TIMER*/
#elif ((CHIPSET == 4) || (CHIPSET == 5) || (CHIPSET == 6) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 9) || (CHIPSET == 10) || (CHIPSET == 11))
   IQ_RtcA_Handler,        /* AIRQ 10 RTC ALARM */
#endif
   IQ_Gauging_Handler,     /* AIRQ 11 ULPD GAUGING */
   IQ_External,            /* AIRQ 12 */
   IQ_Dummy,               /* AIRQ 13 */
   IQ_Dummy,               /* DMA interrupt */
#if (CHIPSET == 4)
   IQ_Dummy,               /* LEAD */
   IQ_Dummy,               /* SIM card-detect fast interrupt */
   IQ_Dummy,               /* External fast interrupt */
   SER_uart_irda_handler,  /* UART IrDA interrupt */
   IQ_GsmTim_Handler       /* ULPD GSM timer */
#elif ((CHIPSET == 5) || (CHIPSET == 6))
   IQ_Dummy,               /* LEAD */
   IQ_Dummy,               /* SIM card-detect fast interrupt */
   IQ_Dummy,               /* External fast interrupt */
   SER_uart_irda_handler,  /* UART IrDA interrupt */
   IQ_GsmTim_Handler,      /* ULPD GSM timer */
       IQ_Dummy,               /* Not mapped interrupt */
   IQ_Dummy,               /* Not mapped interrupt */
   IQ_Dummy,               /* Not mapped interrupt */
   IQ_Dummy,               /* Not mapped interrupt */
   IQ_Dummy                /* GEA interrupt */
#elif ((CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 11))
#if (L1_DYN_DSP_DWNLD == 1)
    IQ_ApiHandler,         /* LEAD */ 
#else
   IQ_Dummy,               /* LEAD */
#endif // if (L1_DYN_DSP_DWNLD == 1)                    
   IQ_Dummy,               /* SIM card-detect fast interrupt */
   IQ_Dummy,               /* External fast interrupt */
   #if (((BOARD == 35) || (BOARD == 46)) && (CHIPSET == 10))
     IQ_Dummy,               /* UART IrDA interrupt */
   #else
     SER_uart_irda_handler,  /* UART IrDA interrupt */
   #endif
   IQ_GsmTim_Handler,      /* ULPD GSM timer */
   IQ_Dummy                /* GEA interrupt */
#elif (CHIPSET == 9)
   IQ_Dummy,               /* LEAD */
   IQ_Dummy,               /* SIM card-detect fast interrupt */
   IQ_Dummy,               /* External fast interrupt */
   SER_uart_irda_handler,  /* UART IrDA interrupt */
   IQ_GsmTim_Handler,      /* ULPD GSM timer */
   IQ_Dummy,               /* Not mapped interrupt */
   IQ_Dummy,               /* Not mapped interrupt */
   IQ_Dummy,               /* Not mapped interrupt */
   IQ_Dummy,               /* Not mapped interrupt */
   IQ_Dummy                /* Reserved */
#else
   IQ_Dummy                /* LEAD */
#endif
#if (((BOARD == 35) || (BOARD == 46)) && (CHIPSET == 10))
  ,IQ_Dummy,               /* AIRQ 21 Reserved */
  IQ_Dummy,                /* AIRQ 22 Reserved */
  IQ_Dummy,                /* AIRQ 23 GSM Edge Extern ARM */
  IQ_Dummy,                /* AIRQ 24 GSM protect */
  IQ_Dummy,                /* AIRQ 25 Reserved */
  #if (OP_L1_STANDALONE == 0)
    IQ_IcrHandler32,         /* AIRQ 26 ICR interrupt */
  #else
    IQ_Dummy,                /* AIRQ 26 ICR interrupt */
  #endif
  IQ_Dummy                 /* AIRQ 27 TCIF ARM7 memory access error */
#endif
};   

#if ((CHIPSET == 4) || (CHIPSET == 5) || (CHIPSET == 6) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 9) || (CHIPSET == 10) || (CHIPSET == 11))
  /*--------------------------------------------------------------*/
  /*  	fiqHandlers                                               */
  /*--------------------------------------------------------------*/
  /* Parameters :none                                             */
  /* Return     :none                                             */
  /* Functionality :  Table of interrupt handlers                 */
  /* These MUST be 32-bit entries                                 */
  /*--------------------------------------------------------------*/

  SYS_FUNC fiqHandlers[IQ_NUM_INT] = 
  {
    IQ_Dummy,          /* Watchdog timer */
    IQ_Dummy,          /* timer 1 */
    IQ_Dummy,          /* timer 2 */
    IQ_Dummy,          /* AIRQ 3 */   
    IQ_Dummy,          /* TPU Frame It AIRQ 4 */
    IQ_Dummy,          /* AIRQ 5 */
    IQ_Dummy,          /* AIRQ 6 */
    IQ_Dummy,          /* AIRQ 7 */
    IQ_Dummy,          /* AIRQ 8 */
    IQ_Dummy,          /* AIRQ 9 RTC Timer */
    IQ_Dummy,          /* AIRQ 10 RTC ALARM */
    IQ_Dummy,          /* AIRQ 11 ULPD GAUGING */
    IQ_Dummy,          /* AIRQ 12 */
    IQ_Dummy,          /* AIRQ 13 Spi Tx Rx interrupt */
    IQ_Dummy,          /* DMA interrupt */
    IQ_Dummy,          /* LEAD */
  #if (OP_L1_STANDALONE == 0)
      SIM_CD_IntHandler, /* SIM card-detect fast interrupt */
  #else
      IQ_Dummy,          /* SIM card-detect fast interrupt */
  #endif
    IQ_Dummy,          /* External fast interrupt */
    IQ_Dummy,          /* UART_IRDA interrupt */
  #if (CHIPSET == 4)
    IQ_Dummy           /* ULPD GSM timer */
  #elif ((CHIPSET == 5) || (CHIPSET == 6))
    IQ_Dummy,          /* ULPD GSM timer */
    IQ_Dummy,          /* Not mapped interrupt */
    IQ_Dummy,          /* Not mapped interrupt */
    IQ_Dummy,          /* Not mapped interrupt */
    IQ_Dummy,          /* Not mapped interrupt */
    IQ_Dummy           /* GEA interrupt */
  #elif ((CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 11))
    IQ_Dummy,          /* ULPD GSM timer */
    IQ_Dummy           /* GEA timer */
  #elif (CHIPSET == 9)
    IQ_Dummy,        /* ULPD GSM timer */
    IQ_Dummy,        /* Not mapped interrupt */
    IQ_Dummy,        /* Not mapped interrupt */
    IQ_Dummy,        /* Not mapped interrupt */
    IQ_Dummy,        /* Not mapped interrupt */
    IQ_Dummy         /* Reserved */
  #endif
  #if ((BOARD == 35) || (BOARD == 46))
    ,IQ_Dummy,         // AIRQ 21 Reserved
    IQ_Dummy,          // AIRQ 22 Reserved
    IQ_Dummy,          // AIRQ 23 GSM Edge Extern ARM
    IQ_Dummy,          // AIRQ 24 GSM protect
    IQ_Dummy,          // AIRQ 25 Reserved
    IQ_Dummy,          // AIRQ 26 ICR interrupt
    IQ_Dummy           // AIRQ 27 TCIF ARM7 memory access error
  #endif
};   
#endif
#endif /* (CHIPSET != 12) && (CHIPSET !=15)*/

#if (PSP_STANDALONE==0) /*FIXME: RTC Gauging To be routed Later */
/*--------------------------------------------------------------*/
/*  IQ_Gauging_Handler				                */
/*--------------------------------------------------------------*/
/* Parameters :none						*/
/* Return     :	none						*/
/* Functionality :   Handle unused interrupts 			*/
/*--------------------------------------------------------------*/
void IQ_Gauging_Handler(void)
{
   GAUGING_Handler();
#if (OP_L1_STANDALONE == 0)
   RTC_GaugingHandler();
#endif
} 
#endif

/*--------------------------------------------------------------*/
/*  IQ_External                                                 */
/*--------------------------------------------------------------*/
/* Parameters : none                                            */
/* Return     : none                                            */
/* Functionality : Handle External IRQ mapped on ABB.           */
/*--------------------------------------------------------------*/
void IQ_External(void)
{
   #if (CHIPSET == 12 || CHIPSET == 15)
      // Mask external interrupt 12
      F_INTH_DISABLE_ONE_IT(C_INTH_ABB_IRQ_IT);
   #else
     // Mask external interrupt 12
     IQ_Mask(IQ_EXT);
   #endif

#if (CHIPSET !=15) 
  // The external IRQ is mapped on the ABB interrupt.
  // The associated HISR ABB_Hisr is activated on reception on the external IRQ.
  if(Activate_ABB_HISR())
  {
   #if (CHIPSET == 12 || CHIPSET == 15)
      F_INTH_ENABLE_ONE_IT(C_INTH_ABB_IRQ_IT);
   #else
     // Mask external interrupt 12
     IQ_Unmask(IQ_EXT);
   #endif
  }
#else
  bspTwl3029_IntC_dispatchInterrupt_1();
#endif
}

#if (CHIPSET != 12 && CHIPSET != 15)
/*--------------------------------------------------------------*/
/*  IQ_Dummy                                                    */
/*--------------------------------------------------------------*/
/* Parameters : none                                            */
/* Return     : none                                            */
/* Functionality :   Handle unused interrupts                   */
/*--------------------------------------------------------------*/
void IQ_Dummy(void)
{
  IQ_DummyCount++;
}   
#endif

#if (CHIPSET != 15) 
/*--------------------------------------------------------------*/
/*  IQ_RTCHandler                                               */
/*--------------------------------------------------------------*/
/* Parameters : none                                            */
/* Return     : none                                            */
/* Functionality :    Handle RTC Time interrupts                */
/*--------------------------------------------------------------*/
void IQ_Rtc_Handler(void)
{
#if (OP_L1_STANDALONE == 0)
  RTC_ItTimerHandle();
#endif
}
#endif

/*--------------------------------------------------------------*/
/*  IQ_RtcA_GsmTim_Handler                                      */
/*--------------------------------------------------------------*/
/* Parameters : none                                            */
/* Return     : none                                            */
/* Functionality :    Handle RTC ALARM or GAUGING interrupts    */
/*--------------------------------------------------------------*/

#if ((CHIPSET == 4) || (CHIPSET == 5) || (CHIPSET == 6) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 9) || (CHIPSET == 10) || (CHIPSET == 11) || (CHIPSET == 12) || (CHIPSET == 15))
#if (CHIPSET!=15)
void IQ_RtcA_Handler(void)
{
  #if (OP_L1_STANDALONE == 0)
    /* INTH_DISABLEONEIT(IQ_RTC_ALARM); *//* RTC ALARM IT  */
    if ( (* (SYS_WORD8 *) RTC_STATUS_REG) & RTC_ALARM )
      RTC_ItAlarmHandle();
  #endif
}
#endif

#if (PSP_STANDALONE==0) /*FIXME: GSM timer To be routed Later */
  void IQ_GsmTim_Handler(void)
  {

    if ( (* (SYS_UWORD16 *) ULDP_GSM_TIMER_IT_REG) & ULPD_IT_TIMER_GSM  )
    {
      // it is GSM Timer it.....
      IQ_GsmTimerCount++;
    }
  }
#endif

#else
void IQ_RtcA_GsmTim_Handler(void)
{
  #if (OP_L1_STANDALONE == 0)
   if ( (* (SYS_UWORD16 *) ULDP_GSM_TIMER_IT_REG) & ULPD_IT_TIMER_GSM  )
   {
     // it is GSM Timer it.....
     IQ_GsmTimerCount++;
   }
   else
   {
     /* INTH_DISABLEONEIT(IQ_RTC_ALARM); *//* RTC ALARM IT  */
     if ( (* (SYS_WORD8 *) RTC_STATUS_REG) & RTC_ALARM )
        RTC_ItAlarmHandle();
   }   
  #endif
}
#endif

#if ((BOARD == 35) || (BOARD == 46))
  #if (OP_L1_STANDALONE == 0)
  /*
   * IQ_IcrHandler32
   *
   */
    void IQ_IcrHandler32(void)
    {
      CSMI_InterruptHandler();  
    }
  #endif
#endif

/* ProtoType */
void bspI2c_Tick(void);

extern void GC_Tick(void);
/*--------------------------------------------------------------*/
/*  IQ_TimerHandler                                             */
/*--------------------------------------------------------------*/
/* Parameters : none                                            */
/* Return     : none                                            */
/* Functionality :    Handle Timer interrupts                   */
/*--------------------------------------------------------------*/
 void IQ_TimerHandler(void)
{
   IQ_TimerCount++;
#if (OP_L1_STANDALONE==1) 
   TMT_Timer_Interrupt();
#endif

#if (CHIPSET==15 && OP_L1_STANDALONE==1)
   bspI2c_Tick();
#endif


#if (OP_L1_STANDALONE==0)
#if (defined RVM_DAR_SWE) && (_GSM==1)
     dar_watchdog_reset();
#endif
#endif
}

#if (PSP_STANDALONE == 0)
/*--------------------------------------------------------------*/
/*  IQ_FramerHandler                                            */
/*--------------------------------------------------------------*/
/* Parameters : none                                            */
/* Return     : none                                            */
/* Functionality :    Handle Timer interrupts                   */
/*--------------------------------------------------------------*/
 void IQ_FrameHandler(void)
{
   IQ_FrameCount++;
   TMT_Timer_Interrupt();
   TP_FrameIntHandler();
   #if (CHIPSET==15)
	bspI2c_Tick();
   #endif
   GC_Tick();            
   #if (OP_L1_STANDALONE == 0)
     #if (TI_PROFILER == 1)
       // TDMA treatment for profiling buffer
       ti_profiler_tdma_action();
     #endif
   #endif
}
#endif

/*--------------------------------------------------------------*/
/*  IQ_TimerHandler1                                            */
/*--------------------------------------------------------------*/
/* Parameters : none                                            */
/* Return     : none                                            */
/* Functionality :    Handle Timer 1 interrupts                 */
/*--------------------------------------------------------------*/
extern void (*timer1_callback) (void);


void IQ_TimerHandler1(void)
{
  IQ_TimerCount1++;  
  #if (OP_L1_STANDALONE == 1)
    /*TM_Timer1Handler();*/
  #endif
#if (PSP_STANDALONE==1)
  TMT_Timer_Interrupt();
  GC_Tick();              
#if (CHIPSET==15)
	bspI2c_Tick();
#endif

#endif
 #if (OP_L1_STANDALONE == 0) /* timer 1 is used by SIM driver for SIM Parity SW workaorund implementation. WIthout this timer, SIM dirver will work 
 							   but  Parity correction will not happen. So In WCP build, sim parity will not work */
  if (timer1_callback)
  	(*timer1_callback)();
#endif
}
 
/*--------------------------------------------------------------*/
/*  IQ_TimerHandler2                                            */
/*--------------------------------------------------------------*/
/* Parameters : none                                            */
/* Return     : none                                            */
/* Functionality :    Handle Timer 2 interrupts                 */
/*--------------------------------------------------------------*/
 void IQ_TimerHandler2(void)
{
  IQ_TimerCount2++;
 }


/*-------------------------------------------------------*/
/* IQ_ApiHandler()                                       */
/*-------------------------------------------------------*/
/* Parameters : none                                     */
/* Return     : none                                     */
/* Functionality : API int management                    */
/*-------------------------------------------------------*/
 void IQ_ApiHandler(void)
 {
  IQ_APICount++;
  #if ((CHIPSET == 15) && (PSP_STANDALONE==0))
   l1_api_handler();

   #if (L1_STEREOPATH == 1)
    l1_audio_api_handler();
   #endif
  #endif
 } /* IQ_ApiHandler() */


#if (CHIPSET !=12 && CHIPSET != 15)
/*--------------------------------------------------------------*/
/*  IQ_IRQ_isr                                                  */
/*--------------------------------------------------------------*/
/* Parameters : none                                            */
/* Return     : none                                            */
/* Functionality :    HHandle IRQ interrupts                    */
/*--------------------------------------------------------------*/
void IQ_IRQ_isr(void)
{
  #if (GSM_IDLE_RAM != 0)
    if (
         (((* (SYS_UWORD16 *) INTH_B_IRQ_REG) & INTH_SRC_NUM) != IQ_FRAME)        && \
         (((* (SYS_UWORD16 *) INTH_B_IRQ_REG) & INTH_SRC_NUM) != IQ_ULPD_GAUGING) && \
         (((* (SYS_UWORD16 *) INTH_B_IRQ_REG) & INTH_SRC_NUM) != IQ_TGSM)            \
       )
    {
    #if (GSM_IDLE_RAM_DEBUG == 1)
       l1s.gsm_idle_ram_ctl.nb_inth++;
       l1s.gsm_idle_ram_ctl.irq = ((* (SYS_UWORD16 *) INTH_B_IRQ_REG) & INTH_SRC_NUM);
    #endif
       ext_ram_inth_handler();
    }
  #endif

  irqHandlers[((* (SYS_UWORD16 *) INTH_B_IRQ_REG) & INTH_SRC_NUM)]();  /* ACK IT */
  * (SYS_UWORD16 *) INTH_CTRL_REG |= (1 << INTH_IRQ);	/* valid next IRQ */
}

/*--------------------------------------------------------------*/
/*  IQ_FIQ_isr                                                  */
/*--------------------------------------------------------------*/
/* Parameters : none                                            */
/* Return     : none                                            */
/* Functionality :   Handle FIQ interrupts                      */
/*--------------------------------------------------------------*/
void IQ_FIQ_isr(void)
{
    #if (GSM_IDLE_RAM != 0)
      if (
          (((* (SYS_UWORD16 *) INTH_B_IRQ_REG) & INTH_SRC_NUM) != IQ_FRAME)        && \
          (((* (SYS_UWORD16 *) INTH_B_IRQ_REG) & INTH_SRC_NUM) != IQ_ULPD_GAUGING) && \
          (((* (SYS_UWORD16 *) INTH_B_IRQ_REG) & INTH_SRC_NUM) != IQ_TGSM)            \
         )
      {
      #if (GSM_IDLE_RAM_DEBUG == 1)
        l1s.gsm_idle_ram_ctl.nb_inth++;
        l1s.gsm_idle_ram_ctl.fiq = ((* (SYS_UWORD16 *) INTH_B_IRQ_REG) & INTH_SRC_NUM);
      #endif
      ext_ram_inth_handler();
      }
    #endif

  #if ((CHIPSET == 4) || (CHIPSET == 5) || (CHIPSET == 6) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 9) || (CHIPSET == 10) || (CHIPSET == 11))
    fiqHandlers[((* (SYS_UWORD16 *) INTH_B_FIQ_REG) & INTH_SRC_NUM)]();  /* ACK IT */
  #endif
    * (SYS_UWORD16 *) INTH_CTRL_REG |= (1 << INTH_FIQ);	/* valid next FIQ */
}   
#endif /* chipset != 12 ) */

#if (OP_L1_STANDALONE == 0)

#if ((BOARD == 8) || (BOARD == 9) || (BOARD == 40) || (BOARD == 41))

/*--------------------------------------------------------------*/
/*  IQ_KeypadGPIOHandler                                        */
/*--------------------------------------------------------------*/
/* Parameters    : none                                         */
/* Return        : none                                         */
/* Functionality : Handle keypad and GPIO interrupts            */
/*--------------------------------------------------------------*/
void IQ_KeypadGPIOHandler(void)
{
    /*
     * GPIO interrupt must be checked before the keypad interrupt. The GPIO
     * status bit is reset when the register is read.
     */
     
    if (AI_CheckITSource (ARMIO_GPIO_INT))
    #ifdef RVM_MPM_SWE
      /* check if the SWE has been started */
      MPM_InterruptHandler ();
    #else
      UAF_DTRInterruptHandler ();
    #endif

    if (AI_CheckITSource (ARMIO_KEYPDAD_INT))
      kpd_key_handler ();
}   

#elif ((BOARD == 35) || (BOARD == 46) || (BOARD == 42) || (BOARD == 43) || (BOARD == 45)||(BOARD==70) || (BOARD==71))

/*--------------------------------------------------------------*/
/*  IQ_KeypadHandler                                            */
/*--------------------------------------------------------------*/
/* Parameters : none                                            */
/* Return     : none                                            */
/* Functionality :   Handle keypad interrupts                   */
/*--------------------------------------------------------------*/
void IQ_KeypadHandler(void)
{
   #if ((BOARD == 35) || (BOARD == 46))
     IQ_Mask (IQ_ARMIO);
   #else
     kpd_key_handler ();
   #endif
}   
	
#endif   /* ((BOARD == 35) || (BOARD == 46) || (BOARD == 42) || (BOARD == 43) || (BOARD == 45)) */

#endif /* OP_L1_STANDALONE */

#if (GSM_IDLE_RAM != 0)
  #if (CODE_VERSION != SIMULATION)
    #if (CHIPSET == 10) && (OP_WCP == 1)
      void ext_ram_inth_handler(void)
      {
        l1s.gsm_idle_ram_ctl.trff_ctrl_enable_cause_int = TRUE;
        if (!READ_TRAFFIC_CONT_STATE)
        {
          CSMI_TrafficControllerOn();
        }
      }
    #endif // (CHIPSET == 10) && (OP_WCP == 1)
  #endif // (CODE_VERSION != SIMULATION)
#endif // (GSM_IDLE_RAM != 0)



