  #if (OP_L1_STANDALONE == 0)
/*
 * INIT.C
 *
 * This module allows to initialize the board:
 *     - wait states,
 *     - unmask selected interrupts,
 *     - initialize clock,
 *     - disable watchdog.
 * Dummy functions used by the EVA3 library are defined.
 */

/* Config Files */

#if (OP_L1_STANDALONE==1)
#pragma DUPLICATE_FOR_INTERNAL_RAM_START
#endif
#ifndef PSP_FAILSAFE
#define PSP_FAILSAFE 0
#warn "PSP Failsafe Flag Not Defined in your Build, taking default"
#endif

#ifndef _WINDOWS
  #include "l1sw.cfg"
  #include "rf.cfg"
  #include "chipset.cfg"
  #include "board.cfg"
  #include "swconfig.cfg"
  #if (OP_L1_STANDALONE == 0)
    #include "rv.cfg"
    #include "sys.cfg"
    #include "debug.cfg"
    #ifdef BLUETOOTH_INCLUDED
      #include "btemobile.cfg"
    #endif
    #ifdef BLUETOOTH
      #include "bluetooth.cfg"
    #endif
  #endif

  #if (OP_L1_STANDALONE == 0)
    #include "rv/rv_defined_swe.h"
  #endif
#endif

/* Include Files */
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "nucleus.h"

#include "sys_types.h"
#include "l1_types.h"
#include "l1_confg.h"
#include "l1_const.h"
#include "pin_config.h" // added for Init tuned to Power Management
#include "armio.h"


#if TESTMODE
  #include "l1tm_defty.h"
#endif // TESTMODE

#if (AUDIO_TASK == 1)
  #include "l1audio_const.h"
  #include "l1audio_cust.h"
  #include "l1audio_defty.h"
#endif // AUDIO_TASK

#if (L1_GTT == 1)
  #include "l1gtt_const.h"
  #include "l1gtt_defty.h"
#endif

#if (L1_MP3 == 1)
  #include "l1mp3_defty.h"
#endif

#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif

#if (L1_AAC == 1)
  #include "l1aac_defty.h"
#endif
#if (L1_DYN_DSP_DWNLD == 1)
  #include "l1_dyn_dwl_defty.h"
#endif

#if (TRACE_TYPE == 4)
  #include "l1_defty.h"
#endif


#if ((OP_L1_STANDALONE == 1) && (CODE_VERSION != SIMULATION) && (PSP_STANDALONE == 0))

  #if (AUDIO_TASK == 1)
    #include "l1audio_signa.h"
    #include "l1audio_msgty.h"
  #endif // AUDIO_TASK

  #if (L1_GTT == 1)
    #include "l1gtt_signa.h"
    #include "l1gtt_msgty.h"
  #endif

  #include "l1_defty.h"
  #include "cust_os.h"
  #include "l1_msgty.h"
  #include "nu_main.h"
  #include "l1_varex.h"
  #include "l1_proto.h"
  #include "hw_debug.h"
  #include "l1_trace.h"

#endif   /* ((OP_L1_STANDALONE == 1) && (CODE_VERSION != SIMULATION) && (PSP_STANDALONE==0)) */


#include "armio/armio.h"
#include "timer/timer.h"

#include "types.h"
#include "bspI2c.h"
#include "bspTwl3029.h"
#include "bspTwl3029_I2c.h"
#include "bspTwl3029_Power.h"


#if (OP_L1_STANDALONE == 0)
  #include "bspTwl3029_Pwr_Map.h"
  #include "uicc/board/bspUicc.h"
#endif


#if (OP_L1_STANDALONE == 0)
  #include "rvf/rvf_api.h"
  #include "rvm/rvm_api.h"  /* A-M-E-N-D-E-D!	*/
  #include "sim/sim.h"
#endif

#include "dynamic_clock.h"
#if (ANLG_FAM !=11)
#include "abb/abb.h"
#endif

/* WCP Profiler */
  #if (OP_L1_STANDALONE == 0)
  #if WCP_PROF == 1
    #include "prf/prf_api.h"
  #endif
#endif

#include "inth/iq.h"
#include "tpudrv.h"
#include "memif/mem.h"
#include "clkm/clkm.h"
#include "inth/inth.h"

#if (OP_L1_STANDALONE == 0)
  void  bspUicc_Phy_intCHandler(void);
#endif

 #if (OP_L1_STANDALONE == 1)
  	#include "uart/serialswitch_core.h"
 #else
  #include "uart/serialswitch.h"
 #endif
 #include "uart/traceswitch.h"


#include "dma/dma.h"
#include "rhea/rhea_arm.h"

#include "ulpd/ulpd.h"

#if (PSP_STANDALONE == 0)
 #if (GSM_IDLE_RAM != 0)
  #if (OP_L1_STANDALONE == 1)
   #include "csmi_simul.h"
  #else
   #include "csmi/csmi.h"
  #endif
 #endif
#endif

#if (OP_WCP == 1)
  #include "memif/sys_memif.h"
#endif

  #include "timer/timer_sec.h"
  #include "dma/sys_dma.h"
  #include "conf/sys_conf.h"
  #include "inth/sys_inth.h"
#if (OP_L1_STANDALONE == 0)
  #include "swconfig.cfg"
#endif

#if ((defined(RVM_NAN_SWE)) || ((defined(RVM_DATALIGHT_SWE)) && (DATALIGHT_NAND == 1)))
    #include "nan/nan_i.h"
#endif

 #ifdef RVM_DMA_SWE
    #include "dma/board/dma_inth.h"
 #endif
 #ifdef RVM_I2C_SWE
    #include "i2c/i2c_hw_functions.h"
 #endif
 #ifdef RVM_MC_SWE
    #include "mc/board/mc_inth.h"
 #endif
 #ifdef RVM_USB_SWE
  #include "usb/usb_inth.h"
 #endif

void f_inth_uart_wakeup(void);

#if (OP_L1_STANDALONE == 0)
  #define TIMER_RESET_VALUE (0xFFFF)

  /*
   * These constants define the number of ticks per TDMA when timer 0
   * is set to pre-scale=0 and the upper limit in percent of TICKS_PER_TDMA
   * for the execution of synchronous layer 1.
   */
  #define TICKS_PER_TDMA    (1875)
  #define LIMIT_FOR_L1_SYNC (80)
#endif

UWORD16 flash_device_id;
#if (PSP_STANDALONE == 0)
 #if (OP_L1_STANDALONE == 0)
  extern void ffs_main_init(void);
  extern void create_tasks(void);
  #if TI_NUC_MONITOR == 1
    extern void ti_nuc_monitor_tdma_action( void );
  #endif

  #if WCP_PROF == 1
    #if PRF_CALIBRATION == 1
      extern NU_HISR prf_CalibrationHISR;
    #endif
  #endif

 #else
  void l1ctl_pgm_clk32(UWORD32 nb_hf, UWORD32 nb_32khz);
  extern void L1_trace_string(char *s);
 #endif   /* (OP_L1_STANDALONE) */
#endif
extern const T_INTH_CONFIG a_inth_config[C_INTH_NB_INTERRUPT];
#ifdef RVM_CAMD_SWE
extern void f_camera_interrupt_manager(void);
#endif

#if (GSM_IDLE_RAM != 0)
    // Interrupt handler called in case the interrupt requires the traffic controler active
    // These routines need to be declared here in order to put them into the a_inth_config_idle_ram structure
    extern void ext_ram_irq_inth_handler(void);
    extern void ext_ram_fiq_inth_handler(void);



    // declared for the _intram file generated by ICL470
    extern const T_INTH_CONFIG a_inth_config_idle_ram[C_INTH_NB_INTERRUPT];

    // Debug mode: irq ext shall be connected to the chipselect signals
    #if GSM_IDLE_RAM_DEBUG
      extern void flash_access_handler(void);
      extern void ext_ram_access_handler(void);
    #endif
#endif

#if (LOCOSTO_LITE==1)
#ifndef HISR_STACK_SHARING
#define HISR_STACK_SHARING
#endif
#endif

#ifdef HISR_STACK_SHARING
unsigned char HISR_STACK_PRIO2[1500]={0xFE};
#endif

#if (PSP_STANDALONE == 0)
 extern void   hisr(void);
 extern void   layer_1_sync_HISR_entry(void);

 extern NU_HISR  layer_1_sync_HISR;

 #if (CODE_VERSION != SIMULATION)
   extern void INT_DisableIRQ(void);
   extern void INT_EnableIRQ(void);
 #endif /* CODE_VERSION != SIMULATION */

#if (OP_L1_STANDALONE == 0)
  #pragma DATA_SECTION(layer_1_sync_stack,".stackandheap");
  #define LAYER_1_SYNC_STACK_SIZE (3000 /*4000*/)
  extern unsigned char layer_1_sync_stack[LAYER_1_SYNC_STACK_SIZE];
 #else
  #if (LONG_JUMP == 3)
    #pragma DATA_SECTION(layer_1_sync_stack,".HISR_stack");
  #endif

  #if TESTMODE
    extern char FAR layer_1_sync_stack[2600 /*3600*/];   // Frame interrupt task stack for EVA3
  #else
    extern char FAR layer_1_sync_stack[1600 /*2600*/];   // Frame interrupt task stack for EVA3
  #endif
 #endif   /* OP_L1_STANDALONE */

#if (FF_L1_IT_DSP_USF == 1) || (FF_L1_IT_DSP_DTX == 1)
	extern void api_modem_hisr();
#if (LONG_JUMP == 3)
	#pragma DATA_SECTION(API_MODEM_HISR_stack,".l1s_global");
#endif
	extern char FAR API_MODEM_HISR_stack[0x400]; // stack size to be tuned
	extern NU_HISR  api_modemHISR;
#endif
#ifndef __TMS470__
 #pragma DUPLICATE_FOR_INTERNAL_RAM_END
#endif
 #if !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM !=0))
   #ifndef __TMS470__
    #pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_START
	#endif

  #if (OP_L1_STANDALONE == 0)
    /*
     * Timing monitor
     *
     *
     */
    #if (TRACE_TYPE == 4)
     extern T_L1A_L1S_COM l1a_l1s_com;
     extern T_L1S_GLOBAL  l1s;
     UNSIGNED             max_cpu, fn_max_cpu;
     unsigned short       layer_1_sync_end_time;
     unsigned short       max_cpu_flag;
 // DSP CPU load measurement trace variables
     UWORD32              dsp_max_cpu_load_trace_array[4];
     UWORD32              dsp_max_cpu_load_idle_frame;
     unsigned short       l1_dsp_cpu_load_trace_flag;

    #endif
  #endif
  #ifndef __TMS470__
  #pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END
  #endif
 #endif // !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM !=0))

 #if (L1_EXT_AUDIO_MGT == 1)
  NU_HISR  EXT_AUDIO_MGT_hisr;
 #ifndef HISR_STACK_SHARING
  char FAR ext_audio_mgt_hisr_stack[500];
 #else
  #define ext_audio_mgt_hisr_stack HISR_STACK_PRIO2
 #endif
  extern void Cust_ext_audio_mgt_hisr(void);
 #endif

#if ( (L1_MP3 == 1) || (L1_MIDI == 1) || (L1_AAC == 1) || (L1_DYN_DSP_DWNLD == 1) )   // equivalent to an API_HISR flag
  extern void api_hisr(void);
  #ifndef HISR_STACK_SHARING
  #if (LONG_JUMP == 3)
    #pragma DATA_SECTION (API_HISR_stack,".l1s_global");
  #endif

  char FAR API_HISR_stack[0x400];
  #else
  #define API_HISR_stack HISR_STACK_PRIO2
  #endif
  NU_HISR apiHISR;
 #endif // (L1_MP3 == 1) || (L1_MIDI == 1) || (L1_DYN_DSP_DWNLD == 1)

#if (FF_L1_IT_DSP_USF == 1) || (FF_L1_IT_DSP_DTX == 1)
  #if !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM != 0))
   #ifndef __TMS470__
  	#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_START
   #endif
    char FAR API_MODEM_HISR_stack[0x400]; // stack size to be tuned
    NU_HISR api_modemHISR;
   #ifndef __TMS470__
	#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END
   #endif
  #endif
#endif // FF_L1_IT_DSP_USF
#endif /* PSP_STANDALONE == 0 */

#if (OP_L1_STANDALONE == 1)
  #if ((TRACE_TYPE==1) || (TRACE_TYPE==2) || (TRACE_TYPE==3)  || (TRACE_TYPE==7) || TESTMODE)
    #include "uart/uart.h"
/*
 * Serial Configuration set up.
 */

    extern char ser_cfg_info[NUMBER_OF_TR_UART];
    #include "rvt_gen.h"
    extern T_RVT_USER_ID trace_id;
  #endif
#endif   /* (OP_L1_STANDALONE == 1) */

/*
 * Serial Configuration set up.
 */

/*
** One config is:
** {XXX_BT_HCI,         // Bluetooth HCI
**  XXX_FAX_DATA,       // Fax/Data AT-Cmd
**  XXX_TRACE,          // L1/Riviera Trace Mux
**  XXX_TRACE},         // Trace PS
**
** with XXX being DUMMY, UART_IRDA or UART_MODEM
*/

#if ((((TRACE_TYPE==1) || (TRACE_TYPE==2) || (TRACE_TYPE==3)  || (TRACE_TYPE==7) ||\
       (TESTMODE)) && (OP_L1_STANDALONE == 1)) || (OP_L1_STANDALONE == 0))
  #if (OP_L1_STANDALONE == 1)
    static T_AppliSerialInfo appli_ser_cfg_info =
  #else
    T_AppliSerialInfo appli_ser_cfg_info =
  #endif   /* OP_L1_STANDALONE */
  	{
           { DUMMY_BT_HCI,
#if (UARTFAX_STATE == 1 || UARTFAX_STATE == 2)
	    UART_MODEM_FAX_DATA,
#else
            DUMMY_FAX_DATA,
#endif

            UART_IRDA_TRACE,
            DUMMY_TRACE}, // 0x0148
	       3,
	       {
		    {DUMMY_BT_HCI,
		 	DUMMY_FAX_DATA,
			UART_IRDA_TRACE,
			DUMMY_TRACE},// 0x0148
		    {DUMMY_BT_HCI,
		 	DUMMY_FAX_DATA,
			DUMMY_TRACE,
			UART_IRDA_TRACE}, // 0x1048
		    {UART_IRDA_BT_HCI,
		 	DUMMY_FAX_DATA,
			DUMMY_TRACE,
			DUMMY_TRACE}, // 0x0049
	       }
	};
  #endif   /* (TRACE_TYPE ...) || (OP_L1_STANDALONE == 0) */


#if !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM !=0))
#ifndef __TMS470__
#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_START
#endif

#if (PSP_STANDALONE == 0)
/*
 * HISR stack and semaphore needed by L1
 */

#if (OP_L1_STANDALONE == 0)
  unsigned char layer_1_sync_stack[LAYER_1_SYNC_STACK_SIZE];
#else
  #if TESTMODE
    char FAR layer_1_sync_stack[2600 /*3600*/];   // Frame interrupt task stack for EVA3
  #else
    char FAR layer_1_sync_stack[1600 /* 2600 */];   // Frame interrupt task stack for EVA3
  #endif
#endif   /* OP_L1_STANDALONE */

NU_HISR  layer_1_sync_HISR;    // Frame interrupt task stack for EVA3
#endif

 const T_INTH_CONFIG a_inth_config[C_INTH_NB_INTERRUPT] =
        { // IRQ/FIQ  LEVEL/EDGE   PRIORITY   HANDLER
        { C_INTH_IRQ, C_INTH_EDGE,   0x01,    IQ_TimerHandler             }, // 0  : WATCHDOG TIMER
        { C_INTH_IRQ, C_INTH_EDGE,   0x02,    IQ_TimerHandler1            }, // 1  : TIMER 1
        { C_INTH_IRQ, C_INTH_EDGE,   0x02,    IQ_TimerHandler2            }, // 2  : TIMER 2
        { C_INTH_FIQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 3  : MCSI
  #if (PSP_STANDALONE == 0)
        { C_INTH_IRQ, C_INTH_EDGE,   0x00,    IQ_FrameHandler             }, // 4  : TPU FRAME
  #else
	    { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 4  : TPU FRAME
  #endif
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 5  : TPU PAGE
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 6  : DRP DBB
  #else
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 6  : DRP DBB
  #endif
		{ C_INTH_IRQ, C_INTH_LEVEL,  0x02,    SER_uart_irda_handler   }, // 7  : UART_IRDA
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 8  : KEYBOARD
  #else
        { C_INTH_IRQ, C_INTH_LEVEL,  0x01,    IQ_KeypadHandler            }, // 8  : KEYBOARD
  #endif
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 9  : DRP DBB RX

    #ifdef RVM_CAMD_SWE

        { C_INTH_IRQ, C_INTH_LEVEL,  0x03,    f_camera_interrupt_manager  }, // 10 : CAMERA

   #else
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 10 : CAMERA

    #endif

  #if (PSP_STANDALONE == 0)
        { C_INTH_IRQ, C_INTH_EDGE,   0x00,    IQ_Gauging_Handler          }, // 11 : ULPD_GAUGING
  #else
		{ C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 11 : ULPD_GAUGING
  #endif
        { C_INTH_IRQ, C_INTH_EDGE,   0x08,    IQ_External                 }, // 12 : ABB_IRQ
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 13 : MSSPI
#if (OP_L1_STANDALONE == 0)
#ifdef SYS_DMA
        { C_INTH_IRQ, C_INTH_LEVEL,  0x06,    f_dma_interrupt_manager     }, // 14 : DMA
#else
	{ C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                },
#endif
#endif
#if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0x06,    f_dma_interrupt_manager     }, // 14 : DMA
#endif

  #if (PSP_STANDALONE == 0)
        { C_INTH_IRQ, C_INTH_EDGE,   0x03,    IQ_ApiHandler               }, // 15 : API
  #else
	    { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 15 : API
  #endif
  		{C_INTH_IRQ, C_INTH_EDGE,   0x01,    Al_int_handler                     }, // 16 : GPIO
        //{ C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 16 : GPIO
	#if (OP_L1_STANDALONE == 0)
        { C_INTH_IRQ, C_INTH_EDGE,   0x03,   f_inth_uart_wakeup                 }, // SER_uart_irda_handler17 : ABB_FIQ
       #endif
       #if (OP_L1_STANDALONE == 1)
        { C_INTH_FIQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 17 : ABB_FIQ
       #endif

        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy          	  }, // 18 : DRP DBB RX
  #if (PSP_STANDALONE == 0)
        { C_INTH_IRQ, C_INTH_LEVEL,  0x03,    IQ_GsmTim_Handler           }, // 19 : ULPD GSM TIMER
  #else
	    { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy	              }, // 19 : ULPD GSM TIMER
  #endif
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 20 : GEA
  #if GSM_IDLE_RAM_DEBUG
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 21 : GPIO1
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 22 : GPIO2
  #else
          { C_INTH_IRQ, C_INTH_EDGE,  0x01,    Al_int_handler                }, // 21 : GPIO1
          { C_INTH_IRQ, C_INTH_EDGE,  0x01,    Al_int_handler                }, // 22 : GPIO2

 //       { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 21 : GPIO1
 //       { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 22 : GPIO2
  #endif
        { C_INTH_FIQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 23 : CPORT
  #if(OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 24 : USIM
  #else
        { C_INTH_IRQ, C_INTH_EDGE,   0x04,    bspUicc_Phy_intCHandler     }, // 24 : USIM
  #endif
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 25 : LCD
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 26 : USB
  #else
    #ifdef RVM_USB_SWE
        { C_INTH_IRQ, C_INTH_LEVEL,  0x03,    usb_int_handler             }, // 26 : USB
    #else
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                },
    #endif
  #endif
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 27 : not used
        { C_INTH_IRQ, C_INTH_LEVEL,  0x03,    bspI2c_Handeler2		      }, // 28 : I2C TRITON
        { C_INTH_IRQ, C_INTH_LEVEL,  0x02,    f_inth_2nd_level_handler    }, // 29 : 2nd Interrupt Handler
        { C_INTH_IRQ, C_INTH_LEVEL,  0x03,    bspI2c_Handeler1		      }, // 30 : I2C
 #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 31 : NAND FLASH
  #else
    #if ((defined(RVM_NAN_SWE)) || ((defined(RVM_DATALIGHT_SWE)) && (DATALIGHT_NAND == 1)))
  	       	 { C_INTH_IRQ, C_INTH_LEVEL,  0x02,    nan_bm_IT_handler              }, // 31 : NAND FLASH
     #else
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                },
     #endif
  #endif
        { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    f_inth_dummy                }, // 0  : RNG
        { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    f_inth_dummy                }, // 1  : SHA1/MD5
        { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    f_inth_dummy                }, // 2  : EMPU


#if (OP_L1_STANDALONE == 0)
#ifdef SYS_DMA
        { C_INTH_IRQ, C_INTH_LEVEL,   0x06,    f_dma_interrupt_manager     }, // 14 : DMA
#else
	 { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                },
#endif
#endif

#if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,   0x06,    f_dma_interrupt_manager     }, // 14 : DMA
#endif

	 { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    f_inth_dummy                }  // 4  : Secure TIMER
    };
#ifndef __TMS470__
#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END
#endif
#endif // !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM !=0))


  const T_DMA_TYPE_GLOBAL_PARAMETER d_dma_global_parameter=
      {
          C_DMA_AUTO_GATE_ON,
          C_DMA_API_PRIO_ARM,
          C_DMA_RHEA_PRIO_ARM,
          C_DMA_IMIF_PRIO_CPU_4,
          C_DMA_IPERIF_PRIO_CPU_4,  // set same as the IMIF priority. Actual value need to decided
          C_DMA_EMIF_DMA_PRIO_7,        // Actual value need to be decided
          C_DMA_EMIF_MCU_PRIO_5         // Actual value need to be decided
      };



#if (GSM_IDLE_RAM != 0)

  #define IRQ_STACK_SIZE  128
  #pragma DATA_SECTION(irq_stack,".irqstk");
  UWORD32 irq_stack[IRQ_STACK_SIZE/4];
  const UWORD32 irq_stack_size = IRQ_STACK_SIZE;

  #define FIQ_STACK_SIZE  512
  #pragma DATA_SECTION(fiq_stack,".fiqstk");
  UWORD32 fiq_stack[FIQ_STACK_SIZE/4];
  const UWORD32 fiq_stack_size = FIQ_STACK_SIZE;

  #define SVC_STACK_SIZE  1024
  #pragma DATA_SECTION(svc_stack,".svcstk");
  UWORD32 svc_stack[SVC_STACK_SIZE/4];
  const UWORD32 svc_stack_size = SVC_STACK_SIZE;

  #define TIMER_HISR_STACK_SIZE  1024
  #pragma DATA_SECTION(timer_hisr_stack,".timstk");
  UWORD32 timer_hisr_stack[TIMER_HISR_STACK_SIZE/4];
  const UWORD32 timer_hisr_stack_size = TIMER_HISR_STACK_SIZE;
#endif

/* HISR_STACK_SHARING: Create global stacks to be used by all HISRs
 * having the same priority */



#if (CODE_VERSION != SIMULATION)
#if (OP_L1_STANDALONE == 0)
#ifndef SYS_DMA
void f_dma_global_parameter_set(T_DMA_TYPE_GLOBAL_PARAMETER  *p_dma_global_parameter)
      {
      if (p_dma_global_parameter->d_dma_global_auto_gate==C_DMA_AUTO_GATE_ON)
        {
        C_DMA_GCR_REG |= (C_DMA_AUTO_GATE_ON << C_DMA_GCR_AUTO_GATE_POS );
        }

      else
        {
        C_DMA_GCR_REG &= ~( C_DMA_GCR_AUTO_GATE_MASK << C_DMA_GCR_AUTO_GATE_POS );
        }

      C_DMA_AR_REG = (
                        (p_dma_global_parameter->d_dma_global_api_prio <<  C_DMA_AR_API_PRIO_POS)
                      | (p_dma_global_parameter->d_dma_global_rhea_prio << C_DMA_AR_RHEA_PRIO_POS)
                      | (p_dma_global_parameter->d_dma_global_imif_prio << C_DMA_AR_IMIF_PRIO_POS)
					  | (p_dma_global_parameter->d_dma_global_iperif_prio << C_DMA_AR_IPERIF_PRIO_POS)

                      );

      f_emif_set_priority(p_dma_global_parameter->d_dma_global_emif_dma_prio,
      						p_dma_global_parameter->d_dma_global_emif_mcu_prio);
      } /*f_dma_global_parameter_set() */
#endif
#endif


#if (!defined(RVM_USB_SWE) || (OP_L1_STANDALONE == 1))
void	Init_USB_Sleep_Enable()
{
	(*(volatile Uint16 *) 0xFFFFB00C) = 0x102; // USB SYSCON1: disable power-off circuitry and lock configuration
}
#endif


void TM_InitializeTimer()
{
    TM_DisableWatchdog();    /* Disable Watchdog */
      TM_SEC_DisableWatchdog();
	        // Enable HW Timers 1 & 2
      TM_EnableTimer (1);
      TM_EnableTimer (2);
}

void Init_clock_configuration()
{
      /*
       *  Enable/Disable of clock switch off for INTH, TIMER, BRIDGE and DPLL modules
       */
      // IRQ, Timer and bridge may SLEEP
      // In first step, same configuration as SAMSON
      //--------------------------------------------------
	  	CLKM_INITCNTL(CLKM_IRQ_DIS | CLKM_TIMER_DIS | CLKM_CPORT_EN | CLKM_BRIDGE_DIS | 0x8000 ); /* CLKM_DPLL_DIS is remove by Ranga*/
 	 *((volatile unsigned short *) (C_MAP_CLKM_BASE+0x10)) = 0x01|0x6;
	 // UART Clock from APLL
	 *((volatile unsigned short *) CLKM_CNTL_CLK_PROG_FREE_RUNN) = 0x0001;

      // Control HOM/SAM automatic switching
      //--------------------------------------------------
      *((volatile unsigned short *) CLKM_CNTL_CLK) &= ~CLKM_EN_IDLE3_FLG;
	 /* Dynamic clock configuration */
        f_dynamic_clock_cfg(C_CLOCK_CFG_DEFAULT);
	// Mark USB on 52 MHZ Clock
	 *((volatile unsigned short *) (CLKM_CNTL_CLK_USB)) = 0x02;

}

void Init_inth()
{
      /*
       *  Disable and Clear all pending interrupts
       */
        F_INTH_DISABLE_ALL_IT;           // MASK all it
        F_INTH2_VALID_NEXT(C_INTH_IRQ);  // reset current IT in INTH2 IRQ
        F_INTH_VALID_NEXT(C_INTH_IRQ);   // reset current IT in INTH IRQ
        F_INTH_VALID_NEXT(C_INTH_FIQ);   // reset current IT in INTH FIQ
        F_INTH_RESET_ALL_IT;             // reset all IRQ/FIQ source
 #if (GSM_IDLE_RAM != 0)
         f_inth_setup((T_INTH_CONFIG *)a_inth_config_idle_ram);   // setup configuration IT handlers
        #else
         f_inth_setup((T_INTH_CONFIG *)a_inth_config);   // setup configuration IT handlers
  #endif
}

void Init_sysdma()
{
#ifdef SYS_DMA
      #if (OP_L1_STANDALONE == 0)
		f_dma_global_parameter_set((T_DMA_TYPE_GLOBAL_PARAMETER *)&d_dma_global_parameter);
	  #endif
       //allocate channel 0 to DSP
	       f_dma_channel_allocation_set(C_DMA_CHANNEL_0, C_DMA_CHANNEL_DSP);
#else
       #if (OP_L1_STANDALONE == 0)
		f_dma_global_parameter_set((T_DMA_TYPE_GLOBAL_PARAMETER *)&d_dma_global_parameter);
	   #endif
       //allocate channel 0 to DSP
	       C_DMA_CAR_REG |= (C_DMA_CHANNEL_DSP << C_DMA_CHANNEL_0);
#endif


}

void Init_armio()
{
      AI_ClockEnable ();
      AI_InitIOConfig();

}


void Init_ulpd()
{
    // Freeze ULPD timer ....
    //--------------------------------------------------
    *((volatile SYS_UWORD16 *) ULDP_GSM_TIMER_INIT_REG ) = 0;
    *((volatile SYS_UWORD16 *) ULDP_GSM_TIMER_CTRL_REG ) = TPU_FREEZE;

    // reset INC_SIXTEEN and INC_FRAC
    //--------------------------------------------------
    #if (OP_L1_STANDALONE == 1)
      l1ctl_pgm_clk32(DEFAULT_HFMHZ_VALUE,DEFAULT_32KHZ_VALUE);
    #else
      ULDP_INCSIXTEEN_UPDATE(132);    //32768.29038  =>132, 	32500 => 133
                                      // 26000 --> 166
      ULDP_INCFRAC_UPDATE(15840);     //32768.29038  =>15840,	32500 => 21845
                                      // 26000 --> 43691
    #endif   /*  OP_L1_STANDALONE */

    // program ULPD WAKE-UP ....
    //=================================================
       *((volatile SYS_UWORD16 *)ULDP_SETUP_FRAME_REG)  = SETUP_FRAME;  // 3 frames
       *((volatile SYS_UWORD16 *)ULDP_SETUP_VTCXO_REG)  = SETUP_VTCXO;  // 0 periods
       *((volatile SYS_UWORD16 *)ULDP_SETUP_SLICER_REG) = SETUP_SLICER; // 31 periods
       *((volatile SYS_UWORD16 *)ULDP_SETUP_CLK13_REG)  = SETUP_CLK13;  // 31 periods
       *((volatile SYS_UWORD16 *)ULPD_SETUP_RF_REG)     = SETUP_RF;     // 31 periods
      *((volatile SYS_UWORD16 *)ULPD_DCXO_SETUP_SLEEPN)    = SETUP_SLEEPZ;    // 0
      *((volatile SYS_UWORD16 *)ULPD_DCXO_SETUP_SYSCLKEN)  = SETUP_SYSCLKEN;  // 255 clocks of 32 KHz for 7.8 ms DCXO delay for Locosto

	  CONF_ND_CE1=MUX_CFG(1,PULLOFF); //CLRZ
  	  CONF_GPIO_34=MUX_CFG(2,PULLOFF); //SLPZ
	  CONF_GPIO_33=MUX_CFG(2,PULLOFF); //SYSCLKEN
	  CONF_GPIO_31=MUX_CFG(2,PULLOFF);	  //CLK13_EN
	  CONF_GPIO_32=MUX_CFG(2,PULLOFF); //DRP_DBB_SYSCLK

    // Set Gauging versus HF (PLL)
    //=================================================
    ULDP_GAUGING_SET_HF;                // Enable gauging versus HF
    ULDP_GAUGING_HF_PLL;                // Gauging versus PLL

    // stop Gauging if any (debug purpose ...)
    //--------------------------------------------------
    if ( *((volatile SYS_UWORD16 *) ULDP_GAUGING_CTRL_REG) & ULDP_GAUGING_EN)
    {
      volatile UWORD32 j;
      ULDP_GAUGING_STOP; /* Stop the gauging */
      /* wait for gauging it*/
      // one 32khz period = 401 periods of 13Mhz
      for (j=1; j<50; j++);
      while (! (* (volatile SYS_UWORD16 *) ULDP_GAUGING_STATUS_REG) & ULDP_IT_GAUGING);
    }
}

void Configure_EMIF(void)
{
        char detect_code[80];
        typedef void (*pf_t)(UWORD32, UWORD16 *, UWORD16 *);
	extern void ffsdrv_device_id_read(UWORD32 base_addr, UWORD16 *manufact, UWORD16 *device);
        pf_t myfp;
	UWORD16 manufact;
        UWORD16 device_id[3];

        // Copy ffsdrv_device_id_read() function code to RAM. The only known
        // way to determine the size of the code is to look either in the
        // linker-generated map file or in the assember output file.
        ffsdrv_copy_code_to_ram((UWORD16 *) detect_code,
                                (UWORD16 *) &ffsdrv_device_id_read,
                                sizeof(detect_code));

        // Combine bit 0 of the thumb mode function pointer with the address
        // of the code in RAM. Then call the detect function in RAM.
        myfp = (pf_t) (((int) &ffsdrv_device_id_read & 1) | (int) detect_code);
        (*myfp)(0x06000000, &manufact, device_id);

	enable_ps_ram_burst();

#if (RELIANCE_FS==1)
	if(0x2C7e == device_id[0] )
	{
	   enable_flash_burst_mirror();
   	   flash_device_id = device_id[0];
	}
	else
	{
   	   flash_device_id = 0;
	}
#else
	if( 0x7e == device_id[0] )
	{
	   enable_flash_burst_fg();
	   flash_device_id = 0x7E;
	}
	else
	{
	   enable_flash_burst_mirror();
   	   flash_device_id = 0;
	}
#endif

	 asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");

}

void Configure_autoidle()
{
#ifndef RVM_USB_SWE
	Init_USB_Sleep_Enable();
#endif

	/* Auto-idle bit set for SPI. This has to be removed and
	 * added as part of SPI driver initialization.
	 */
	*((volatile SYS_UWORD32 *)0x09E00010) |= 0x01;

	/* Auto-idle bit set for DES. This is doen here as CRY
	 * module is not part of the sytem. Auto-idle bit is also
	 * set in the initialization of CRY module.
	 */
	 *((volatile SYS_UWORD32 *) 0x09900030) |= 0x01;

	/* Auto-idle bit set for Camera. This is doen here as CRY
	 * module is not part of the sytem. Auto-idle bit is also
	 * set in the initialization of CRY module.
	 */
	 *((volatile SYS_UWORD32 *) 0x09700010 ) |= 0x01;

	/* COnfigure SIM in 1.8V mode */
 	   *((volatile SYS_UWORD16 *)0xFFFEF01C) &= 0xF7FF;

}

/*
 * Init_Target
 *
 * Performs low-level HW Initialization.
 */
void Init_Target(void)
{
	
  /* Variable for reading the BCR for MCP RAM */
	unsigned short bcrTmpVal;

   (*(volatile Uint16 *) 0xFFFF702A) = 0x0002;//reset the UART module.
	Init_clock_configuration();
	Init_inth();
	Init_sysdma();
	Init_armio();


        /* API-RHEA control register configuration */
        f_emif_api_rhea_conf(C_RHEA_STROBE0_ACCESS_SIZE_ADAPT_ENABLE,
                             C_RHEA_STROBE1_ACCESS_SIZE_ADAPT_ENABLE,
                             C_API_ACCESS_SIZE_ADAPT_ENABLE,
                             C_ARM_DEBUG_DISABLE);


      /*
       * Initialize current DSP clock to 0 in order to pass through
       * the right DSP latency configuration (increase DSP clock)
       * in f_dynamic_clock_cfg().
       * Obviously, the real DSP clock is not 0kHz.
       * d_dsp_cur_clk will be updated after clock configuration in f_dynamic_clock_cfg().
       */
        d_dsp_cur_clk = 0;   // Used to keep track of current DSP clock.


      // Write_en_0 = 0 , Write_en_1 = 0
        RHEA_INITARM(0,0);

	 Init_ulpd();

#if (OP_L1_STANDALONE == 0)
	    /* Mark The System configuration According to I-Sample */
		/* Adding GPIO Mux Setting Here */
		pin_configuration_all(); // Init Tuned for Power Management
		/* A22 is Enabled in int.s hence not Here */
		/* FIXME: PULL_UP Enable and PULL UP Values Need to revisited */

		Configure_autoidle();

	/* Add code to find out the manufacture id of NOR flash*/
	Configure_EMIF();

	TM_InitializeTimer();

/* 25 NOP cycles for delay */

      	 asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
 	  asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
	  asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");

    #endif  /* (OP_L1_STANDALONE == 0) */


#if(OP_L1_STANDALONE == 1 && MIRROR_BIT == 1 ) //temp FIX for L1 standalone-this fix will work only for I-sample mirror bit
 Init_USB_Sleep_Enable();
   	 flash_device_id = 0;
#elif(OP_L1_STANDALONE == 1 && MIRROR_BIT == 0 )
	 flash_device_id = 0x7E;
#endif

#if (CHIPSET==15)
  	{
  	volatile SYS_UWORD16 * configReg=(volatile SYS_UWORD16 *)0xFFFEF01C; /* Set USIM_VMODE to 1.8 in CONF_CORE_REG */
	*configReg &= 0xF7FF;
  	}
  #endif


}

//--> Init Added for Power Management
/*******************************************************
	Configure ALL I/O pins
*******************************************************/
void pin_configuration_all(void)
{
	pin_configuration_mmc();	/*	Added new function to configure GPIO 12 for card Insertion/Removal detect	*/
	pin_configuration_bluetooth();
	pin_configuration_emifs();
	pin_configuration_system();
	pin_configuration_lcd_nand(0);
	pin_configuration_keypad();
	pin_configuration_sim();
	pin_configuration_radio();
 #ifdef RVM_USB_SWE
	pin_configuration_usb();
 #endif
	pin_configuration_camera_irda(1);
}


/*****************************************************
Configure GPIO pin for card Insert/Removal detection
*******************************************************/
void pin_configuration_mmc(void)
{
GPIO_DIRECTION_IN(12);
CONF_GPIO_12	= MUX_CFG(0, PULLUP);

}




/*******************************************************
	Configure Bluetooth I/O pins
*******************************************************/
void pin_configuration_bluetooth(void)
{
	GPIO_DIRECTION_OUT(37);	GPIO_CLEAR_OUTPUT(37);	// BT_nSHUTDOWN
	CONF_GPIO_43	 = MUX_CFG(1, PULLOFF);	// MCSI_CK
	CONF_GPIO_44	 = MUX_CFG(1, PULLOFF);	// MCSI_FS
	CONF_GPIO_45	 = MUX_CFG(1, PULLOFF);	// MCSI_TX
	CONF_GPIO_46	 = MUX_CFG(1, PULLOFF);	// MCSI_RX
	CONF_UART_CTS	 = MUX_CFG(0, PULLOFF);	// CTS
	CONF_UART_RX	 = MUX_CFG(0, PULLOFF);	// RX
	CONF_UART_TX	 = MUX_CFG(0, PULLOFF);	// TX
	CONF_GPIO_37	 = MUX_CFG(0, PULLOFF); // BT_SHUTDOWN
}

/*******************************************************
	Configure EMIFS I/O pins
*******************************************************/
void pin_configuration_emifs(void)
{
	CONF_ADD_21	 = MUX_CFG(0, PULLOFF);	// ADD21
	CONF_GPIO_39	 = MUX_CFG(1, PULLOFF); // ADD22
	CONF_GPIO_38	 = MUX_CFG(0, PULLOFF); // nCS0
	CONF_NCS3	 = MUX_CFG(0, PULLOFF);	// nCS3
	CONF_ADV	 = MUX_CFG(0, PULLOFF);	// ADV
	CONF_NMOE	 = MUX_CFG(0, PULLOFF);	// nMOE
	CONF_RNW	 = MUX_CFG(0, PULLOFF);	// RnW
	CONF_GPIO_42	 = MUX_CFG(1, PULLOFF); // CKM
	CONF_NRDY	 = MUX_CFG(0, PULLUP);	// nRDYMEM
}

/*******************************************************
	Configure system I/O pins
*******************************************************/
void pin_configuration_system(void)
{
     #ifdef RVM_MC_SWE
       GPIO_DIRECTION_OUT(2);
	GPIO_CLEAR_OUTPUT(2);	// SYS_RESET
       /*********************SPI+ MMC********************************/

    	CONF_GPIO_23	 = MUX_CFG(1, PULLOFF);	// SPI_CLK
	CONF_GPIO_24	 = MUX_CFG(1, PULLUP);	// SPI_DATA_MISO
	CONF_GPIO_25	 = MUX_CFG(1, PULLUP);	// SPI_DATA_MOSI
	CONF_GPIO_26	 = MUX_CFG(0, PULLOFF);	// SPI_NCS0

	GPIO_DIRECTION_OUT(23);//SPI CLK
	GPIO_DIRECTION_OUT(25);//SPI MOSI
	GPIO_DIRECTION_OUT(26);//SPI NCS0
	GPIO_DIRECTION_IN(24);//SPI MISO
     	/************************************************************/
     #else

	GPIO_DIRECTION_IN(1); // GPIO_1
	GPIO_DIRECTION_OUT(2);	GPIO_CLEAR_OUTPUT(2);	// SYS_RESET
	GPIO_DIRECTION_IN(10); // Not used (nEMU0)

      #endif

	GPIO_DIRECTION_IN(1); // GPIO_1
	GPIO_DIRECTION_OUT(2);	GPIO_CLEAR_OUTPUT(2);	// SYS_RESET
	GPIO_DIRECTION_IN(10); // Not used (nEMU0)
	CONF_CK13MHZ_EN	 = MUX_CFG(0, PULLOFF);	// CK13MHZ_EN
	CONF_ABB_IRQ	 = MUX_CFG(0, PULLUP);	// ABB_IRQ
	CONF_GPIO_5	 = MUX_CFG(1, PULLOFF); // STARTADC
	CONF_CDO	 = MUX_CFG(0, PULLOFF);	// CDO (I2S)
	CONF_CSCLK	 = MUX_CFG(0, PULLOFF);	// CSCLK (I2S)
	CONF_CSYNC	 = MUX_CFG(0, PULLOFF);	// CSYNC (I2S)
	CONF_NBSCAN	 = MUX_CFG(0, PULLUP);	// nBSCAN
	CONF_SPARE_3	 = MUX_CFG(0, PULLUP); // Spare3
	CONF_TDO	 = MUX_CFG(0, PULLOFF);	// TDO

	CONF_TCK	 = MUX_CFG(0, PULLOFF);	// TCK
	CONF_TDI	 = MUX_CFG(0, PULLOFF);	// TDI
	CONF_TMS	 = MUX_CFG(0, PULLOFF);	// TMS
	CONF_TRST	 = MUX_CFG(0, PULLOFF);	// TRST

	CONF_VDR	 = MUX_CFG(0, PULLOFF);	// VDR
	CONF_VFSRX	 = MUX_CFG(0, PULLDOWN);	// VFSRX
	CONF_GPIO_1	 = MUX_CFG(0, PULLUP); // Not used GPIO
	CONF_GPIO_2	 = MUX_CFG(0, PULLOFF); // SYS_RESET

     #ifdef RVM_MC_SWE
	 CONF_GPIO_10 = MUX_CFG(1, PULLDOWN); // used for SD Card //Not used (NEMU0)
	 GPIO_DIRECTION_OUT(1); // GPIO_1
        AI_SetBit(1);
     #else
	CONF_GPIO_10	 = MUX_CFG(1, PULLUP); // Not used (NEMU0)
     #endif

	//CONF_GPIO_12	 = MUX_CFG(2, PULLOFF); // TSPACT10


}

/*******************************************************
	Configure LCD and NAND Flash I/O pins
   Mode = 0 : LCD functional. NAND not functional
   Mode = 1 : NAND functional. LCD not functional
*******************************************************/
void pin_configuration_lcd_nand(int mode)
{
	CONF_ND_CE1	 = MUX_CFG(0, PULLOFF);
	CONF_GPIO_18	 = MUX_CFG(1, PULLOFF); // ND_WE
	CONF_GPIO_31	 = MUX_CFG(1, PULLOFF); // ND_RE
	CONF_GPIO_32	 = MUX_CFG(1, PULLOFF); // ND_CLE
	CONF_GPIO_33	 = MUX_CFG(1, PULLOFF); // ND_ALE
	CONF_GPIO_34	 = MUX_CFG(1, PULLUP); // ND_RDY
	CONF_LCD_NRST	 = MUX_CFG(0, PULLOFF);
	CONF_LCD_RNW	 = MUX_CFG(0, PULLOFF);
	CONF_LCD_RS	 = MUX_CFG(0, PULLOFF);
	CONF_LCD_STB	 = MUX_CFG(0, PULLOFF);
	CONF_GPIO_13	 = MUX_CFG(1, PULLOFF); // LCD_NCS0
	switch(mode)
		{
			case 0: // LCD
				CONF_LCD_DATA_0	 = MUX_CFG(0, PULLOFF);
				CONF_LCD_DATA_1	 = MUX_CFG(0, PULLOFF);
				CONF_LCD_DATA_2	 = MUX_CFG(0, PULLOFF);
				CONF_LCD_DATA_3	 = MUX_CFG(0, PULLOFF);
				CONF_LCD_DATA_4	 = MUX_CFG(0, PULLOFF);
				CONF_LCD_DATA_5	 = MUX_CFG(0, PULLOFF);
				CONF_LCD_DATA_6	 = MUX_CFG(0, PULLOFF);
				CONF_LCD_DATA_7	 = MUX_CFG(0, PULLOFF);
			break;
			case 1: // NAND
				CONF_LCD_DATA_0	 = MUX_CFG(3, PULLOFF);
				CONF_LCD_DATA_1	 = MUX_CFG(3, PULLOFF);
				CONF_LCD_DATA_2	 = MUX_CFG(3, PULLOFF);
				CONF_LCD_DATA_3	 = MUX_CFG(3, PULLOFF);
				CONF_LCD_DATA_4	 = MUX_CFG(3, PULLOFF);
				CONF_LCD_DATA_5	 = MUX_CFG(3, PULLOFF);
				CONF_LCD_DATA_6	 = MUX_CFG(3, PULLOFF);
				CONF_LCD_DATA_7	 = MUX_CFG(3, PULLOFF);
			break;
		}
}

/*******************************************************
	Configure keypad pins
*******************************************************/
void pin_configuration_keypad(void)
{
	CONF_GPIO_8	 = MUX_CFG(0, PULLUP); // KBR4
	CONF_GPIO_9	 = MUX_CFG(0, PULLOFF); // KBC4
	CONF_KBC_0	 = MUX_CFG(0, PULLOFF);
	CONF_KBC_1	 = MUX_CFG(0, PULLOFF);
	CONF_KBC_2	 = MUX_CFG(0, PULLOFF);
	CONF_KBC_3	 = MUX_CFG(0, PULLOFF);
	CONF_KBR_0	 = MUX_CFG(0, PULLUP);
	CONF_KBR_1	 = MUX_CFG(0, PULLUP);
	CONF_KBR_2	 = MUX_CFG(0, PULLUP);
	CONF_KBR_3	 = MUX_CFG(0, PULLUP);
}

/*******************************************************
	Configure SIM I/O pins
*******************************************************/
void pin_configuration_sim(void)
{
	CONF_SIM_CLK	 = MUX_CFG(0, PULLOFF);
	CONF_SIM_IO	 = MUX_CFG(0, PULLOFF);
	CONF_SIM_PWCTRL	 = MUX_CFG(0, PULLOFF);
	CONF_SIM_RST	 = MUX_CFG(0, PULLOFF);
}

/*******************************************************
	Configure radio I/O pins
*******************************************************/
void pin_configuration_radio(void)
{
	CONF_TSPACT_11	 = MUX_CFG(0, PULLOFF);
	CONF_TSPACT_12	 = MUX_CFG(0, PULLOFF);
	CONF_TSPACT_13	 = MUX_CFG(0, PULLOFF);
	CONF_TSPACT_14	 = MUX_CFG(0, PULLOFF);
	CONF_TSPACT_15	 = MUX_CFG(0, PULLOFF);
}

/*******************************************************
	Configure USB I/O pins
*******************************************************/
#ifdef RVM_USB_SWE
void pin_configuration_usb(void)
{
	CONF_USB_BOOT	 = MUX_CFG(0, PULLOFF);
	CONF_USB_DAT	 = MUX_CFG(0, PULLDOWN);
	CONF_USB_RCV	 = MUX_CFG(0, PULLDOWN);
	//CONF_USB_SE0	 = MUX_CFG(0, PULLUP);  // to reduce 130 uA
	CONF_USB_SE0	 = MUX_CFG(0, PULLOFF);  // to reduce 130 uA
	CONF_USB_TXEN	 = MUX_CFG(0, PULLOFF);
}
#endif
/*******************************************************
	Configure Camera and IrDA I/O pins
   Mode = 0 : IrDA functional. Camera not functional
   Mode = 1 : Camera functional. IrDA not functional
  Use mode 1 ONLY when camera is active i.e. use
  mode 0 during sleep
*******************************************************/
void pin_configuration_camera_irda(int mode)
{
	GPIO_DIRECTION_OUT(17);	GPIO_SET_OUTPUT(17);	// CAM_PWDN
	/* GPIO_DIRECTION_OUT(11);	GPIO_SET_OUTPUT(11);*/ 	// Golden eye sleep
	CONF_GPIO_35	 = MUX_CFG(1, PULLUP); // LED_TORCH
	GPIO_DIRECTION_OUT(35);	GPIO_SET_OUTPUT(35);	// LED_TRCH
	GPIO_DIRECTION_OUT(4);	GPIO_SET_OUTPUT(4);	// nCAM_RST
	GPIO_DIRECTION_OUT(20);	GPIO_CLEAR_OUTPUT(20); // CAM_D_3
	GPIO_DIRECTION_OUT(19);	GPIO_CLEAR_OUTPUT(19); // CAM_HS
	GPIO_DIRECTION_OUT(21);	GPIO_CLEAR_OUTPUT(21); // CAM_LCLK
	CONF_GPIO_36	 = MUX_CFG(1, PULLUP); // IRDA_SD
	GPIO_DIRECTION_OUT(36);	GPIO_SET_OUTPUT(36);	// IrDA_SD
	//CONF_GPIO_36	 = MUX_CFG(0, PULLOFF); // IRDA_SD
    /* CONF_GPIO_11	 = MUX_CFG(1, PULLOFF);*/  // Golden eye sleep (NEMU1)
	CONF_GPIO_4	 = MUX_CFG(0, PULLOFF); // CAM_RESET
	//CONF_GPIO_35	 = MUX_CFG(0, PULLOFF); // LED_TORCH
	//CONF_GPIO_17	 = MUX_CFG(0, PULLOFF); // CAM_PWDN

       #ifdef RVM_MC_SWE
       //CONF_GPIO_23	 = MUX_CFG(1, PULLDOWN);	// SPI_CLK
	//CONF_GPIO_24	 = MUX_CFG(1, PULLDOWN);	// SPI_DATA_MISO
	//CONF_GPIO_25	 = MUX_CFG(1, PULLDOWN);	// SPI_DATA_MOSI
	//CONF_GPIO_26	 = MUX_CFG(1, PULLUP);	// SPI_NCS0
       #else
	CONF_GPIO_23	 = MUX_CFG(1, PULLDOWN);	// SPI_CLK
	CONF_GPIO_24	 = MUX_CFG(1, PULLDOWN);	// SPI_DATA_MISO
	CONF_GPIO_25	 = MUX_CFG(1, PULLDOWN);	// SPI_DATA_MOSI
	CONF_GPIO_26	 = MUX_CFG(1, PULLUP);	// SPI_NCS0
	#endif

	CONF_GPIO_27	 = MUX_CFG(1, PULLUP);	// SPI_NCS1
	CONF_GPIO_22	 = MUX_CFG(1, PULLOFF); // CAM_XCLK
	switch(mode)
		{
			case 0: // IrDA or sleep
				CONF_GPIO_47	 = MUX_CFG(1, PULLUP); // IrDA RXIR
				CONF_GPIO_0	 = MUX_CFG(1, PULLOFF); // IrDA TXIR
				CONF_GPIO_7	 = MUX_CFG(5, PULLDOWN); // CAM_D_2
				CONF_GPIO_20	 = MUX_CFG(0, PULLOFF); // CAM_D_3 = GPIO
				CONF_ND_NWP	 = MUX_CFG(0, PULLOFF); // CAM_D_4 = ND_NWP
				CONF_GPIO_30	 = MUX_CFG(3, PULLDOWN); // CAM_D_5
				CONF_GPIO_29	 = MUX_CFG(3, PULLDOWN); // CAM_D_6
				CONF_GPIO_28	 = MUX_CFG(3, PULLUP); // CAM_D_7
				CONF_GPIO_19	 = MUX_CFG(0, PULLOFF); // CAM_HS = GPIO
				CONF_GPIO_21	 = MUX_CFG(0, PULLOFF); // CAM_LCLK = GPIO
			break;
			case 1: // Camera
				CONF_GPIO_47	 = MUX_CFG(2, PULLOFF); // CAM_D_0 + IrDA
				CONF_GPIO_0	 = MUX_CFG(2, PULLOFF); // CAM_D_1 + IrDA
				CONF_GPIO_7	 = MUX_CFG(5, PULLOFF); // CAM_D_2
				CONF_GPIO_20	 = MUX_CFG(2, PULLOFF); // CAM_D_3
				CONF_ND_NWP	 = MUX_CFG(2, PULLOFF); // CAM_D_4
				CONF_GPIO_30	 = MUX_CFG(3, PULLOFF); // CAM_D_5
				CONF_GPIO_29	 = MUX_CFG(3, PULLOFF); // CAM_D_6
				CONF_GPIO_28	 = MUX_CFG(3, PULLOFF); // CAM_D_7
				CONF_GPIO_19	 = MUX_CFG(1, PULLOFF); // CAM_HS
				CONF_GPIO_21	 = MUX_CFG(1, PULLOFF); // CAM_LCLK
			break;
		}
}


//<-- Init Added for Power Management

/*
 * Init_Drivers
 *
 * Performs Drivers Initialization.
 */

#define MS_TO_TICKS(_x)   ((((_x) * 200) + 462) / 923)

#define POWER_INIT_DONE 0xFF
#define RESET_DONE 0xFF

/* Variable to determine wether Reset occured or Power button was pressed*/

Uint8 Reset_done;
#pragma DATA_SECTION(Power_init_debounce,".powerdebounce")
UINT8 Power_init_debounce;




void Set_Switch_ON_Cause(void);
void Init_Drivers(void)
{
    UINT8 Pw;
    int i=0;
	#ifdef LOCOSTO_PG2
	   C_CONF_LCD_CAM_NAN_REG |= 0x04;
	#endif
	bspI2c_init();
	bspTwl3029_init();
	#if (OP_L1_STANDALONE == 0)
		Set_Switch_ON_Cause();
	#endif


    /*De- bounce logic for power key during bootup OMAPS00094058*/

    BspTwl3029_I2c_ReadSingle(BSP_TWL3029_I2C_PMC_MASTER,BSP_TWL_3029_MAP_PMC_MASTER_STS_HW_CONDITIONS_OFFSET,(UINT8 *)&Pw, NULL);

    /*Power_init will maintain its value after bootup
      even if a reset occurs because it is kept in internal RAM
      On I sample board we also have a RPOWN facility
    */

    if((Power_init_debounce!=POWER_INIT_DONE)&&((Pw&0x02)==0))    {
#ifdef POWER_KEY_DEBOUNCE
	for(i=0;i<50;i++)  	{

		wait_ARM_cycles(convert_nanosec_to_cycles(10000*10000));

		/*Each I2c read will itself take approximately 0.02ms*/

	    BspTwl3029_I2c_ReadSingle(BSP_TWL3029_I2C_PMC_MASTER,BSP_TWL_3029_MAP_PMC_MASTER_STS_HW_CONDITIONS_OFFSET,(UINT8 *)&Pw, NULL);
	    if((Pw&0x01)==0)	  {
		break;
	    }
	}

	BspTwl3029_I2c_ReadSingle(BSP_TWL3029_I2C_PMC_MASTER,BSP_TWL_3029_MAP_PMC_MASTER_STS_HW_CONDITIONS_OFFSET,&Pw, NULL);

	if((Pw&0x01)==0)	{

	    /*Switch off the Power supply to the entire board*/

	    BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_PMC_MASTER,BSP_TWL_3029_MAP_PMC_MASTER_P1_DEV_OFFSET,0x01, NULL);
	    while(1);
	}
#endif
    Power_init_debounce=POWER_INIT_DONE;
    Reset_done=(Uint8)(~(RESET_DONE));
    }
  else if(Power_init_debounce==POWER_INIT_DONE)   {

    Reset_done=RESET_DONE;

  }


	/* Turn on DRP We will make VRMCC to device group Modem
	 * And Switch it on.
	 */
	bspTwl3029_Power_setDevGrp(NULL,BSP_TWL3029_POWER_VRMMC,BSP_TWL3029_POWER_DEV_GRP_MODEM);
	wait_ARM_cycles(convert_nanosec_to_cycles(100000*2));
    bspTwl3029_Power_enable(NULL,BSP_TWL3029_POWER_VRMMC,BSP_TWL3029_POWER_STATE_ACTIVE);

 #if (OP_L1_STANDALONE == 0)
    /*
     * FFS main initialization.
     */

    ffs_main_init();

    /*
     * SIM Main Initialization.
     */
	bspUicc_bootInit();

  #endif
}

#if (PSP_STANDALONE == 0)
/*
 * l1_create_HISR
 *
 * Create L1 HISR.
 */
void l1_create_HISR (void)
{
  STATUS status;

  #if (OP_L1_STANDALONE == 0)
    // Fill the entire stack with the pattern 0xFE
    memset (layer_1_sync_stack, 0xFE, LAYER_1_SYNC_STACK_SIZE);
  #endif

  status = NU_Create_HISR (&layer_1_sync_HISR,
                           "L1_HISR",
                           layer_1_sync_HISR_entry,
  #if (OP_L1_STANDALONE == 0)
                           1,
                           layer_1_sync_stack,
                           LAYER_1_SYNC_STACK_SIZE);
  #else
                           1,
                           layer_1_sync_stack,
                           sizeof(layer_1_sync_stack));
  #endif

  #if (L1_EXT_AUDIO_MGT)
    // Create HISR for Ext MIDI activity
    //==================================
    status += NU_Create_HISR(&EXT_AUDIO_MGT_hisr,
                             "H_EXT_AUDIO_MGT",
                             Cust_ext_audio_mgt_hisr,
                             2,
                             ext_audio_mgt_hisr_stack,
                             sizeof(ext_audio_mgt_hisr_stack));
  #endif

  #if ( (L1_MP3 == 1) || (L1_MIDI == 1) || (L1_AAC == 1) || (L1_DYN_DSP_DWNLD == 1) )    // equivalent to an API_HISR flag
    status += NU_Create_HISR(&apiHISR,
                             "API_HISR",
                             api_hisr,
                             2,
                             API_HISR_stack,
                             sizeof(API_HISR_stack));
  #endif // (L1_MP3 == 1) || (L1_MIDI == 1) || (L1_AAC == 1) || (L1_DYN_DSP_DWNLD == 1)

  #if (FF_L1_IT_DSP_USF == 1) || (FF_L1_IT_DSP_DTX == 1) // equivalent to an API_MODEM_HISR flag
    // Create HISR for USF  DSP interrupt !!!!. This HISR needs
    // to have the highest priority since the USF status needs
    // to be known before the next block starts.
    //========================================================
    status += NU_Create_HISR(&api_modemHISR,
                             "MODEM",
                             api_modem_hisr,
                             1,
                             API_MODEM_HISR_stack,
                             sizeof(API_MODEM_HISR_stack));
  #endif

  assert (status == 0);
}


void l1_create_ISR (void)
{
  l1_create_HISR();
}
#endif

#endif /* (CODE_VERSION != SIMULATION) */



/*
 * Init_Unmask_IT
 *
 * Unmask all used interrupts.
 */

void Init_Unmask_IT (void)
{
  #if (CODE_VERSION != SIMULATION)
    /* Reset all current interrupts */
      F_INTH2_VALID_NEXT(C_INTH_IRQ);   /* Reset current IT in INTH2 IRQ */
      F_INTH_VALID_NEXT(C_INTH_IRQ);   /* Reset current IT in INTH IRQ */
      F_INTH_VALID_NEXT(C_INTH_FIQ);   /* Reset current IT in INTH FIQ */
      F_INTH_RESET_ALL_IT;   /* Reset all IRQ/FIQ source */

    // Disable all the IRQs of ARM before unmasking the interrupts. And enable them once the unmasking is
    // complete. This will ensure that no context switch happens due to coming of an already unmasked interrupt
    // before completing the unmasking of the rest of the interrupts. If the context switch happens in
    // before all the interrupts have been unmasked, then the control will never return to this function
    //since it will go to TCT_Scheduler() and so some interrupts may remain masked.
    INT_DisableIRQ();

    // SL: TEMPORARY FIX FOR BUG ON P2
    // FRAME INTERRUPT STAY MASKED

    // TEMPORARY FIX FOR BUG ON P2


    /* Enable Uart Modem and IrDA interrupts */
    #if ((((TRACE_TYPE==1) || (TRACE_TYPE==2) || (TRACE_TYPE==3) || \
          (TESTMODE) || (TRACE_TYPE==7)) && (OP_L1_STANDALONE == 1)) || \
         (OP_L1_STANDALONE == 0))

        F_INTH_ENABLE_ONE_IT (C_INTH_UART_IRDA_IT);   /* Enable UART_IRDA interrupts */
    #endif   /* (TRACE_TYPE ...) || (OP_L1_STANDALONE == 0) */

    /* Enable other interrupts */
    #if (OP_L1_STANDALONE == 1)
      if (l1_config.pwr_mngt == PWR_MNGT)
      {

        #if GEMINI_TIMER
            F_INTH_ENABLE_ONE_IT(C_INTH_TIMER1_IT);   /* Enable Timer1 interrupt */
        #endif

        #if EXT_IRQ
            F_INTH_ENABLE_ONE_IT (C_INTH_ABB_IRQ_IT);   /* Enable ABB_IRQ_IT EXT_IRQ */
         #endif
      }

    #endif

        //enable LEAD2 interrupt
        F_INTH_ENABLE_ONE_IT (C_INTH_API_IT);


    /* Enable TDMA interrupt */
	 #if (PSP_STANDALONE == 0)
      F_INTH_ENABLE_ONE_IT (C_INTH_FRAME_IT);
	 #endif

//-->  to enable DMA Interrupt for Lite
		 #if (LOCOSTO_LITE == 1)
#ifdef SYS_DMA
      			F_INTH_ENABLE_ONE_IT (C_INTH_DMA_IT);
#endif
	 	#endif
//<--  to enable DMA Interrupt for Lite

    // Enable the ARM IRQs once all the interrupts have been unmasked.
    INT_EnableIRQ();

//no need to un-mask this interrupt as wake-up signal can wake up the system

  #endif   /* NO SIMULATION */

}


/*
 * Init_Serial_Flows
 *
 * Performs Serialswitch + related serial data flows initialization.
 */

void Init_Serial_Flows (void)
{
  #if (OP_L1_STANDALONE == 0)

    /*
     * Initialize Serial Switch module.
     */
          SER_InitSerialConfig (&appli_ser_cfg_info);
    /*
     * Then Initialize the Serial Data Flows and the associated UARTs:
     *  - G2-3 Trace if GSM/GPRS Protocol Stack
     *  - AT-Cmd/Fax & Data Flow
     *
     * Layer1/Riviera Trace Flow and Bluetooth HCI Flow are initialized
     * by the appropriate SW Entities.
     *
     * G2-3 Trace => No more Used
     */
    SER_tr_Init(SER_PROTOCOL_STACK, TR_BAUD_38400, NULL);

    /*
     * Fax & Data / AT-Command Interpreter Serial Data Flow Initialization
     */

      (void) SER_fd_Initialize ();
  #else    /* OP_L1_STANDALONE */

    #if (TESTMODE || (TRACE_TYPE==1) || (TRACE_TYPE==2) || (TRACE_TYPE==3) || (TRACE_TYPE==6) || (TRACE_TYPE==7))
        ser_cfg_info[UA_UART_0] = 'G';
      ser_cfg_info[UA_UART_0] = 'R'; // Riviear Demux on UART MODEM

      /* init Uart Modem */
      SER_InitSerialConfig (&appli_ser_cfg_info);

      #if TESTMODE || (TRACE_TYPE == 1) || (TRACE_TYPE == 7)
        SER_tr_Init (SER_LAYER_1, TR_BAUD_115200, rvt_activate_RX_HISR);

        rvt_register_id("OTHER",&trace_id,(RVT_CALLBACK_FUNC)NULL);
      #else
        SER_tr_Init (SER_LAYER_1, TR_BAUD_38400, NULL);
      #endif

      L1_trace_string(" \n\r");

    #endif   /* TRACE_TYPE */

  #endif   /* OP_L1_STANDALONE */
}


// From this point, everything is compiled to execute in internal RAM

#if (((MOVE_IN_INTERNAL_RAM == 1) ^ (GSM_IDLE_RAM != 0)) && (GSM_IDLE_RAM != 0))
#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_START
#if (CODE_VERSION != SIMULATION)

    #if GSM_IDLE_RAM_DEBUG
      void flash_access_handler(void)
      {
        l1s.gsm_idle_ram_ctl.killing_flash_access++;
      }
      void ext_ram_access_handler(void)
      {
        l1s.gsm_idle_ram_ctl.killing_ext_ram_access++;
      }
    #endif

    void ext_ram_irq_inth_handler(void)
    {
      if (!READ_TRAFFIC_CONT_STATE)
      CSMI_TrafficControllerOn();
      a_inth_config[F_INTH_GET_IRQ].d_it_handler();
    }

    void ext_ram_fiq_inth_handler(void)
    {
      if (!READ_TRAFFIC_CONT_STATE)
      CSMI_TrafficControllerOn();
      a_inth_config[F_INTH_GET_FIQ].d_it_handler();
    }
#endif // (CODE_VERSION != SIMULATION)
#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END
#endif

#if !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM !=0))
#ifndef __TMS470__
#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_START
#endif
#if (PSP_STANDALONE == 0)
/*-------------------------------------------------------*/
/* TP_FrameIntHandler() Low Interrupt service routine    */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :  activate Hisr on each frame interrupt*/
/*-------------------------------------------------------*/
void TP_FrameIntHandler(void)
{

  #if (OP_L1_STANDALONE == 1)

    #if (TRACE_TYPE==1)
       if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_L1S_CPU_LOAD)
       {
         TM_ResetTimer (2, 0xFFFF, 1, 0);
         TM_StartTimer (2);
       }
    #endif

    #if (TRACE_TYPE==6)
       TM_ResetTimer (2, 0xFFFF, 1, 0);
       TM_StartTimer (2);
    #endif

    #if (TRACE_TYPE==7)   /* CPU_LOAD */
       l1_cpu_load_start();
    #endif

  #else

     #if (TRACE_TYPE == 4) && (TI_NUC_MONITOR != 1) && (WCP_PROF == 1)
              TM_ResetTimer (2, TIMER_RESET_VALUE, 1, 0);
              TM_StartTimer (2);
    #endif

    #if (TI_NUC_MONITOR == 1)
       /* Copy LISR buffer in Log buffer each end of HISR */
       ti_nuc_monitor_tdma_action();
    #endif

    #if WCP_PROF == 1
       prf_LogFNSwitch(l1s.actual_time.fn_mod42432);
    #endif

  #endif   /* OP_L1_STANDALONE */

  NU_Activate_HISR(&layer_1_sync_HISR);   /* Activate HISR interrupt */

  #if (OP_L1_STANDALONE == 0)
    #if (WCP_PROF == 1)
      #if (PRF_CALIBRATION == 1)
      NU_Activate_HISR(&prf_CalibrationHISR);
      #endif
    #endif
  #endif

}
#endif

/*
 * layer_1_sync_HISR_entry
 *
 * HISR associated to layer 1 sync.
 */

void layer_1_sync_HISR_entry (void)
{
#if (PSP_STANDALONE==0)
   // Call Synchronous Layer1
   hisr();
#endif
}
#ifndef __TMS470__
#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END
#endif
#endif // !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM !=0))

#if (PSP_STANDALONE == 1)

#include "nucleus.h"
#include "tc_defs.h"

extern TC_PROTECT TCD_System_Protect;

/*-------------------------------------------------------*/
 /* int OS_system_protect()                               */
 /*-------------------------------------------------------*/
 /* Parameters : none                                     */
 /* Return     : The Thread Control Block of the thread   */
 /*              which already owns the protection or     */
 /*              0 if no protection                       */
 /* Functionality : Checks whether the system structures  */
 /*                 are already protected or not          */
 /*-------------------------------------------------------*/
 void OS_system_protect (void)
 {
   NU_Protect((NU_PROTECT*) &TCD_System_Protect);
 }

 /*-------------------------------------------------------*/
 /* int OS_system_Unprotect()                             */
 /*-------------------------------------------------------*/
 /* Parameters : none                                     */
 /* Return     :                                          */
 /* Functionality : unprotect the system structures       */
 /*-------------------------------------------------------*/
 void OS_system_Unprotect (void)
 {
   NU_Unprotect();
 }

void bspDummy_detect(void* a,int b, void* c)
{
	return;
}

void bspDummy_remove(int a)
{
	return;
}

void Init_Watchdog_Timer(void)
{
/* This code is for PSP STANDALONE Build Only */
/* WatchDog will be used by DAR enity, so using TIMER1 for OS Ticks
   This will give tick period roughly equle to 4.5 ms which approx
   Frame Interrupt timing */
   Dtimer1_Init_cntl(0X0D, 1, 0x07, 1);
   Dtimer1_Start(1);
   F_INTH_ENABLE_ONE_IT(C_INTH_TIMER1_IT);
   bspUicc_drvRegister((BspUicc_CardPresentDetectHandler)bspDummy_detect,
   	(BspUicc_CardAbsentDetectHandler) bspDummy_remove);
}

#endif



#ifdef BOOT_TIME_MEASURE


extern Uint32 prelim_boot_time;
unsigned int Time_snapshot[BOOT_NUMBER_OF_MEASUREMENTS];


void Bsp_get_boot_time(Uint32 * Dataptr)
{
    static int num=0;
    int i;

    if(num!=0)	{
	for(i=0;i<BOOT_NUMBER_OF_MEASUREMENTS;i++)	{

	    Dataptr[i]=Time_snapshot[i];

	}

	return;

    }

    Dataptr[0]=Time_snapshot[0];

    for (i=(BOOT_NUMBER_OF_MEASUREMENTS-1);i>=1;i-- )	{

	Time_snapshot[i]=Time_snapshot[i]-Time_snapshot[i-1];
	Dataptr[i]=Time_snapshot[i];

    }
    num++;
    return;

}




Uint32 get_time(UINT8 num)
{

    if(num==PRELIM_BOOT)	{
	return (prelim_boot_time/1000);

    }
    #if (OP_L1_STANDALONE == 0)

    else	{

	return ((4.6)*gsp_get_tick_count());

    }
    #endif

}


Uint8 Bsp_Boot_Time_Take_Snapshot(Time_Intervals number)
{
#if(OP_L1_STANDALONE == 0)
    if(number>=BOOT_NUMBER_OF_MEASUREMENTS)	{

/*Wrong Input return FAILURE (on Zero Value)*/	   
        return 1;
    	   

    }

    Time_snapshot[number]=get_time(number);

/*return SUCCESS*/
      return 0;
      
#else
      return 0;
#endif
}

#endif

#ifdef TI_PS_16BIT_CPY

void api_memcpy(void *dst,void* src,size_t len)
{
  unsigned int i;
  unsigned int tempLen;
  unsigned char *cdst,*csrc;
  unsigned int *idst;
  unsigned short *ssrc,*tempPtr1,*tempPtr2;

#if (WCP_PROF == 1)
  prf_LogFunctionEntry((unsigned long)api_memcpy);
#endif

  cdst=dst;
  csrc=src;
  idst=dst;
  ssrc=src;

  /* Check Alignment Unaligned copies would be costly*/
  if((unsigned int)dst&0x03 || (unsigned int)src&0x03)
  {
    for(i=0;i<len;i++)
    {
      *cdst++=*csrc++;
    }
  }
  else
  {
    tempLen=len>>2;
    for(i=0;i<tempLen;i++)
    {
      tempPtr1=ssrc++;
      tempPtr2=ssrc++;
      *idst++=*tempPtr1|(*tempPtr2)<<16;
    }
    tempLen = len - (tempLen<<2);
    cdst=(unsigned char*)idst;
    csrc=(unsigned char*)ssrc;
    for(i=0;i<tempLen;i++)
    {
      *cdst++=*csrc++;
    }

  }

#if (WCP_PROF == 1)
  prf_LogFunctionExit((unsigned long)api_memcpy);
#endif

  return;
}

#endif

#endif //  #if (OP_L1_STANDALONE == 0)


#if (OP_L1_STANDALONE == 1)


/*
 * INIT.C
 *
 * This module allows to initialize the board:
 *     - wait states,
 *     - unmask selected interrupts,
 *     - initialize clock,
 *     - disable watchdog.
 * Dummy functions used by the EVA3 library are defined.
 */

/* Config Files */

#pragma DUPLICATE_FOR_INTERNAL_RAM_START

#ifndef PSP_FAILSAFE
#define PSP_FAILSAFE 0
#warn "PSP Failsafe Flag Not Defined in your Build, taking default"
#endif


#ifndef _WINDOWS
  #include "l1sw.cfg"
  #include "rf.cfg"
  #include "chipset.cfg"
  #include "board.cfg"
  #include "swconfig.cfg"
  #if (OP_L1_STANDALONE == 0)
    #include "rv.cfg"
    #include "sys.cfg"
    #include "debug.cfg"
    #ifdef BLUETOOTH_INCLUDED
      #include "btemobile.cfg"
    #endif
    #ifdef BLUETOOTH
      #include "bluetooth.cfg"
    #endif
  #endif

  #if (OP_L1_STANDALONE == 0)
    #include "rv/rv_defined_swe.h"
  #endif
#endif

/* Include Files */
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "nucleus.h"

#include "sys_types.h"
#include "l1_types.h"
#include "l1_confg.h"
#include "l1_const.h"
#include "pin_config.h" // added for Init tuned to Power Management

#if TESTMODE
  #include "l1tm_defty.h"
#endif // TESTMODE

#if (AUDIO_TASK == 1)
  #include "l1audio_const.h"
  #include "l1audio_cust.h"
  #include "l1audio_defty.h"
#endif // AUDIO_TASK

#if (L1_GTT == 1)
  #include "l1gtt_const.h"
  #include "l1gtt_defty.h"
#endif

#if (L1_MP3 == 1)
  #include "l1mp3_defty.h"
#endif

#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif

#if (L1_AAC == 1)
  #include "l1aac_defty.h"
#endif
#if (L1_DYN_DSP_DWNLD == 1)
  #include "l1_dyn_dwl_defty.h"
#endif

#if (TRACE_TYPE == 4)
  #include "l1_defty.h"
#endif


#if ((OP_L1_STANDALONE == 1) && (CODE_VERSION != SIMULATION) && (PSP_STANDALONE == 0))

  #if (AUDIO_TASK == 1)
    #include "l1audio_signa.h"
    #include "l1audio_msgty.h"
  #endif // AUDIO_TASK

  #if (L1_GTT == 1)
    #include "l1gtt_signa.h"
    #include "l1gtt_msgty.h"
  #endif

  #include "l1_defty.h"
  #include "cust_os.h"
  #include "l1_msgty.h"
  #include "nu_main.h"
  #include "l1_varex.h"
  #include "l1_proto.h"
  #include "hw_debug.h"
  #include "l1_trace.h"

#endif   /* ((OP_L1_STANDALONE == 1) && (CODE_VERSION != SIMULATION) && (PSP_STANDALONE==0)) */


#include "armio/armio.h"
#include "timer/timer.h"

#if (CHIPSET==15)
#include "types.h"
#include "bspI2c.h"
#include "bspTwl3029.h"
#include "bspTwl3029_I2c.h"
#include "bspTwl3029_Power.h"
#if (OP_L1_STANDALONE == 0)
  #include "uicc/board/bspUicc.h"
#endif
#endif

#if (OP_L1_STANDALONE == 0)
  #include "rvf/rvf_api.h"
  #include "rvm/rvm_api.h"  /* A-M-E-N-D-E-D!	*/
  #include "sim/sim.h"
#endif

#include "dynamic_clock.h"
#if (ANLG_FAM !=11)
#include "abb/abb.h"
#endif

#if (BOARD==35 || BOARD==46)
  #if (OP_L1_STANDALONE == 0)
    #include "csmi/csmi.h"
    #include "csmi/csmi_gsmctrl.h"
    #include "csmi/csmi_uart.h"
    #include "uart/uartfax.h"
    #include "csmi/csmi_uartfax.h"
    #include "ffs/ffs.h"
    #include "ffs/ffspcm.h"
    #include "csmi/sleep.h"
  #endif   /* (OP_L1_STANDALONE == 0) */
#endif

/* WCP Profiler */
#if (BOARD==35 || BOARD==43 || BOARD==46) && (OP_L1_STANDALONE == 0)
  #if WCP_PROF == 1
    #include "prf/prf_api.h"
  #endif
#endif

#include "inth/iq.h"
#include "tpudrv.h"
#include "memif/mem.h"
#include "clkm/clkm.h"
#include "inth/inth.h"

#if (OP_L1_STANDALONE == 0)
  void  bspUicc_Phy_intCHandler(void);
#endif

 #if (OP_L1_STANDALONE == 1)
  	#include "uart/serialswitch_core.h"
 #else
  #include "uart/serialswitch.h"
 #endif
 #include "uart/traceswitch.h"


#include "dma/dma.h"
#include "rhea/rhea_arm.h"

#include "ulpd/ulpd.h"

#if (PSP_STANDALONE == 0)
 #if (GSM_IDLE_RAM != 0)
  #if (OP_L1_STANDALONE == 1)
   #include "csmi_simul.h"
  #else
   #include "csmi/csmi.h"
  #endif
 #endif
#endif

#if (CHIPSET == 12) || (CHIPSET == 15) || ((CHIPSET == 10)  && (OP_WCP == 1))
  #include "memif/sys_memif.h"
#endif

#if ((CHIPSET == 12) || (CHIPSET == 15))
  #include "timer/timer_sec.h"
  #include "dma/sys_dma.h"
  #include "conf/sys_conf.h"
  #include "inth/sys_inth.h"
 #ifdef RVM_NAN_SWE
    #include "nan/nan_i.h"
 #endif
 #ifdef RVM_DMA_SWE
    #include "dma/board/dma_inth.h"
 #endif
 #ifdef RVM_I2C_SWE
    #include "i2c/i2c_hw_functions.h"
 #endif
 #ifdef RVM_MC_SWE
    #include "mc/board/mc_inth.h"
 #endif
 #ifdef RVM_USB_SWE
  #include "usb/usb_inth.h"
 #endif
#endif

#if (OP_L1_STANDALONE == 0)
  #define TIMER_RESET_VALUE (0xFFFF)

  /*
   * These constants define the number of ticks per TDMA when timer 0
   * is set to pre-scale=0 and the upper limit in percent of TICKS_PER_TDMA
   * for the execution of synchronous layer 1.
   */
  #define TICKS_PER_TDMA    (1875)
  #define LIMIT_FOR_L1_SYNC (80)
#endif

UWORD16 flash_device_id;
#if (PSP_STANDALONE == 0)
 #if (OP_L1_STANDALONE == 0)
  extern void ffs_main_init(void);
  extern void create_tasks(void);
  #if TI_NUC_MONITOR == 1
    extern void ti_nuc_monitor_tdma_action( void );
  #endif

  #if WCP_PROF == 1
    #if PRF_CALIBRATION == 1
      extern NU_HISR prf_CalibrationHISR;
    #endif
  #endif

 #else
  void l1ctl_pgm_clk32(UWORD32 nb_hf, UWORD32 nb_32khz);
  extern void L1_trace_string(char *s);
 #endif   /* (OP_L1_STANDALONE) */
#endif

#if (CHIPSET == 12) || (CHIPSET == 15)
extern const T_INTH_CONFIG a_inth_config[C_INTH_NB_INTERRUPT];
#ifdef RVM_CAMD_SWE
extern void f_camera_interrupt_manager(void);
#endif
#endif

#if (GSM_IDLE_RAM != 0)
  #if (CHIPSET == 12) || (CHIPSET == 15)

    // Interrupt handler called in case the interrupt requires the traffic controler active
    // These routines need to be declared here in order to put them into the a_inth_config_idle_ram structure
    extern void ext_ram_irq_inth_handler(void);
    extern void ext_ram_fiq_inth_handler(void);



    // declared for the _intram file generated by ICL470
    extern const T_INTH_CONFIG a_inth_config_idle_ram[C_INTH_NB_INTERRUPT];

    // Debug mode: irq ext shall be connected to the chipselect signals
    #if GSM_IDLE_RAM_DEBUG
      extern void flash_access_handler(void);
      extern void ext_ram_access_handler(void);
    #endif
  #endif // CHIPSET 12 || CHIPSET 15
#endif

#if (PSP_STANDALONE == 0)
 extern void   hisr(void);
 extern void   layer_1_sync_HISR_entry(void);

 extern NU_HISR  layer_1_sync_HISR;

 #if (CODE_VERSION != SIMULATION)
  #if (BOARD == 71)
   extern void INT_DisableIRQ(void);
   extern void INT_EnableIRQ(void);
  #endif /* (BOARD == 71) */
 #endif /* CODE_VERSION != SIMULATION */

#if (LOCOSTO_LITE==1)
#ifndef HISR_STACK_SHARING
#define HISR_STACK_SHARING
#endif
#endif

#ifdef HISR_STACK_SHARING
unsigned char HISR_STACK_PRIO2[1500]={0xFE};
#endif

#if (OP_L1_STANDALONE == 0)
  #pragma DATA_SECTION(layer_1_sync_stack,".stackandheap");
  #define LAYER_1_SYNC_STACK_SIZE (3000 /*4000*/)
  extern unsigned char layer_1_sync_stack[LAYER_1_SYNC_STACK_SIZE];
 #else
  #if (LONG_JUMP == 3)
    #pragma DATA_SECTION(layer_1_sync_stack,".HISR_stack");
  #endif

  #if TESTMODE
    extern char FAR layer_1_sync_stack[2600 /*3600*/];   // Frame interrupt task stack for EVA3
  #else
    extern char FAR layer_1_sync_stack[1600 /*2600*/];   // Frame interrupt task stack for EVA3
  #endif
 #endif   /* OP_L1_STANDALONE */

#if (FF_L1_IT_DSP_USF == 1) || (FF_L1_IT_DSP_DTX == 1)
	extern void api_modem_hisr();
#if (LONG_JUMP == 3)
	#pragma DATA_SECTION(API_MODEM_HISR_stack,".l1s_global");
#endif
	extern char FAR API_MODEM_HISR_stack[0x400]; // stack size to be tuned
	extern NU_HISR  api_modemHISR;
#endif

 #pragma DUPLICATE_FOR_INTERNAL_RAM_END

 #if !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM !=0))
  #pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_START
  #if (OP_L1_STANDALONE == 0)
    /*
     * Timing monitor
     *
     *
     */
    #if (TRACE_TYPE == 4)
     extern T_L1A_L1S_COM l1a_l1s_com;
     extern T_L1S_GLOBAL  l1s;
     UNSIGNED             max_cpu, fn_max_cpu;
     unsigned short       layer_1_sync_end_time;
     unsigned short       max_cpu_flag;
    #endif
  #endif
  #pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END
 #endif // !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM !=0))

 #if (L1_EXT_AUDIO_MGT == 1)
  NU_HISR  EXT_AUDIO_MGT_hisr;
 #ifndef HISR_STACK_SHARING
  char FAR ext_audio_mgt_hisr_stack[500];
 #else
  #define ext_audio_mgt_hisr_stack HISR_STACK_PRIO2
 #endif
  extern void Cust_ext_audio_mgt_hisr(void);
 #endif

#if ( (L1_MP3 == 1) || (L1_MIDI == 1) || (L1_AAC == 1) || (L1_DYN_DSP_DWNLD == 1) )   // equivalent to an API_HISR flag
  extern void api_hisr(void);
  #ifndef HISR_STACK_SHARING
  #if (LONG_JUMP == 3)
    #pragma DATA_SECTION (API_HISR_stack,".l1s_global");
  #endif

  char FAR API_HISR_stack[0x400];
  #else
  #define API_HISR_stack HISR_STACK_PRIO2
  #endif
  NU_HISR apiHISR;
 #endif // (L1_MP3 == 1) || (L1_MIDI == 1) || (L1_DYN_DSP_DWNLD == 1)

#if (FF_L1_IT_DSP_USF == 1) || (FF_L1_IT_DSP_DTX == 1)
  #if !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM != 0))
  #pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_START
    char FAR API_MODEM_HISR_stack[0x400]; // stack size to be tuned
    NU_HISR api_modemHISR;
  #pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END
  #endif
#endif // FF_L1_IT_DSP_USF
#endif /* PSP_STANDALONE == 0 */

#if (OP_L1_STANDALONE == 1)
  #if ((TRACE_TYPE==1) || (TRACE_TYPE==2) || (TRACE_TYPE==3)  || (TRACE_TYPE==7) || TESTMODE)
    #include "uart/uart.h"
/*
 * Serial Configuration set up.
 */

    extern char ser_cfg_info[NUMBER_OF_TR_UART];
    #include "rvt_gen.h"
    extern T_RVT_USER_ID trace_id;
  #endif
#endif   /* (OP_L1_STANDALONE == 1) */

/*
 * Serial Configuration set up.
 */

/*
** One config is:
** {XXX_BT_HCI,         // Bluetooth HCI
**  XXX_FAX_DATA,       // Fax/Data AT-Cmd
**  XXX_TRACE,          // L1/Riviera Trace Mux
**  XXX_TRACE},         // Trace PS
**
** with XXX being DUMMY, UART_IRDA or UART_MODEM
*/

#if ((((TRACE_TYPE==1) || (TRACE_TYPE==2) || (TRACE_TYPE==3)  || (TRACE_TYPE==7) ||\
       (TESTMODE)) && (OP_L1_STANDALONE == 1)) || (OP_L1_STANDALONE == 0))
  #if (OP_L1_STANDALONE == 1)
    static T_AppliSerialInfo appli_ser_cfg_info =
  #else
    T_AppliSerialInfo appli_ser_cfg_info =
  #endif   /* OP_L1_STANDALONE */
  #if (CHIPSET!=15)
            {
  #ifdef BTEMOBILE
    #ifdef BT_UART_USED_MODEM
             {UART_MODEM_BT_HCI,
              DUMMY_FAX_DATA,
              UART_IRDA_TRACE,
              DUMMY_TRACE},    // default config (Bluetooth on Modem) - 0x014A
    #else
             {UART_IRDA_TRACE,
              DUMMY_FAX_DATA,
              UART_MODEM_TRACE,
              DUMMY_TRACE},    // default config  (Bluetooth on IrDa) - 0x0249
    #endif // BT_UART_USED_MODEM
  #else // BTEMOBILE
              {DUMMY_BT_HCI,
               UART_MODEM_FAX_DATA,
               UART_IRDA_TRACE,
              DUMMY_TRACE},    // default config = 0x0168
  #endif
  #ifdef BTEMOBILE
             12,	// 12 serial config allowed
  #else // BTEMOBILE
             9,	// 9 serial config allowed
  #endif
             {
              // Configs with Condat Panel only
              {DUMMY_BT_HCI,
               DUMMY_FAX_DATA,
               DUMMY_TRACE,
               UART_IRDA_TRACE},   // 0x1048
              {DUMMY_BT_HCI,
               DUMMY_FAX_DATA,
               DUMMY_TRACE,
               UART_MODEM_TRACE},  // 0x2048
              // Configs with L1/Riviera Trace only
              {DUMMY_BT_HCI,
               DUMMY_FAX_DATA,
               UART_IRDA_TRACE,
               DUMMY_TRACE},       // 0x0148
              {DUMMY_BT_HCI,
               DUMMY_FAX_DATA,
               UART_MODEM_TRACE,
               DUMMY_TRACE},       // 0x0248
              // Configs with AT-Cmd only
              {DUMMY_BT_HCI,
               UART_MODEM_FAX_DATA,
               DUMMY_TRACE,
               DUMMY_TRACE},       // 0x0068
              // Configs with Condat Panel and L1/Riviera Trace
              {DUMMY_BT_HCI,
               DUMMY_FAX_DATA,
               UART_MODEM_TRACE,
               UART_IRDA_TRACE},	 // 0x1248
              {DUMMY_BT_HCI,
               DUMMY_FAX_DATA,
               UART_IRDA_TRACE,
               UART_MODEM_TRACE},	 // 0x2148
              // Configs with Condat Panel and AT-Cmd
              {DUMMY_BT_HCI,
               UART_MODEM_FAX_DATA,
               DUMMY_TRACE,
               UART_IRDA_TRACE},   // 0x1068
  #ifdef BTEMOBILE
              // Configs with L1/Riviera Trace and Bluetooth HCI
              {UART_IRDA_BT_HCI,
               DUMMY_FAX_DATA,
               UART_MODEM_TRACE,
               DUMMY_TRACE},       // 0x0249
              {UART_MODEM_BT_HCI,
               DUMMY_FAX_DATA,
               UART_IRDA_TRACE,
               DUMMY_TRACE},       // 0x014A
              // Configs with AT-Cmd and Bluetooth HCI
              {UART_IRDA_BT_HCI,
               UART_MODEM_FAX_DATA,
               DUMMY_TRACE,
               DUMMY_TRACE},       // 0x0069
  #endif // BTEMOBILE
              // Configs with L1/Riviera Trace and AT-Cmd
              {DUMMY_BT_HCI,
               UART_MODEM_FAX_DATA,
               UART_IRDA_TRACE,
               DUMMY_TRACE}        // 0x0168
             }
            };
  #else  /* CHIPSET==15 */
  	{
           {DUMMY_BT_HCI,
            DUMMY_FAX_DATA,
            UART_IRDA_TRACE,
            DUMMY_TRACE}, // 0x0148
	       3,
	       {
		    {DUMMY_BT_HCI,
		 	DUMMY_FAX_DATA,
			UART_IRDA_TRACE,
			DUMMY_TRACE},// 0x0148
		    {DUMMY_BT_HCI,
		 	DUMMY_FAX_DATA,
			DUMMY_TRACE,
			UART_IRDA_TRACE}, // 0x1048
		    {UART_IRDA_BT_HCI,
		 	DUMMY_FAX_DATA,
			DUMMY_TRACE,
			DUMMY_TRACE}, // 0x0049
	       }
	};
  #endif /* CHIPSET !=15*/
  #endif   /* (TRACE_TYPE ...) || (OP_L1_STANDALONE == 0) */


#if !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM !=0))
#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_START

#if (PSP_STANDALONE == 0)
/*
 * HISR stack and semaphore needed by L1
 */

#if (OP_L1_STANDALONE == 0)
  unsigned char layer_1_sync_stack[LAYER_1_SYNC_STACK_SIZE];
#else
  #if TESTMODE
    char FAR layer_1_sync_stack[2600 /*3600*/];   // Frame interrupt task stack for EVA3
  #else
    char FAR layer_1_sync_stack[1600 /* 2600 */];   // Frame interrupt task stack for EVA3
  #endif
#endif   /* OP_L1_STANDALONE */

NU_HISR  layer_1_sync_HISR;    // Frame interrupt task stack for EVA3
#endif

#if (CHIPSET == 12)

  const T_INTH_CONFIG a_inth_config[C_INTH_NB_INTERRUPT] =
        { // IRQ/FIQ  LEVEL/EDGE   PRIORITY   HANDLER
        { C_INTH_IRQ, C_INTH_EDGE,   0x01,    IQ_TimerHandler             }, // 0  : WATCHDOG TIMER
        { C_INTH_IRQ, C_INTH_EDGE,   0x02,    IQ_TimerHandler1            }, // 1  : TIMER 1
        { C_INTH_IRQ, C_INTH_EDGE,   0x02,    IQ_TimerHandler2            }, // 2  : TIMER 2
        { C_INTH_FIQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 3  : TSP RECEIVE
        { C_INTH_IRQ, C_INTH_EDGE,   0x00,    IQ_FrameHandler             }, // 4  : TPU FRAME
        { C_INTH_IRQ, C_INTH_EDGE,   0x04,    f_inth_dummy                }, // 5  : TPU PAGE
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 6  : SIM
  #else
        { C_INTH_IRQ, C_INTH_EDGE,   0x07,    SIM_IntHandler              }, // 6  : SIM
  #endif
        { C_INTH_IRQ, C_INTH_LEVEL,  0x02,    SER_uart_modem_handler      }, // 7  : UART_MODEM1
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0x01,    f_inth_dummy                }, // 8  : KEYBOARD
  #else
        { C_INTH_IRQ, C_INTH_LEVEL,  0x01,    IQ_KeypadHandler            }, // 8  : KEYBOARD
  #endif
        { C_INTH_IRQ, C_INTH_EDGE,   0x03,    IQ_Rtc_Handler              }, // 9  : RTC_TIMER
        { C_INTH_IRQ, C_INTH_LEVEL,  0x03,    IQ_RtcA_Handler             }, // 10 : RTC_ALARM
        { C_INTH_IRQ, C_INTH_EDGE,   0x00,    IQ_Gauging_Handler          }, // 11 : ULPD_GAUGING
        { C_INTH_IRQ, C_INTH_EDGE,   0x08,    IQ_External                 }, // 12 : ABB_IRQ
        { C_INTH_IRQ, C_INTH_EDGE,   0x05,    f_inth_dummy                }, // 13 : SPI
        { C_INTH_IRQ, C_INTH_LEVEL,  0x06,    f_dma_interrupt_manager     }, // 14 : DMA
        { C_INTH_IRQ, C_INTH_EDGE,   0x03,    IQ_ApiHandler               }, // 15 : API
        { C_INTH_IRQ, C_INTH_EDGE,   0x07,    f_inth_dummy                }, // 16 : GPIO
        { C_INTH_FIQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 17 : ABB_FIQ
        { C_INTH_IRQ, C_INTH_LEVEL,  0x02,    SER_uart_irda_handler       }, // 18 : UART_IRDA
        { C_INTH_IRQ, C_INTH_LEVEL,  0x03,    IQ_GsmTim_Handler           }, // 19 : ULPD GSM TIMER
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 20 : GEA
#if GSM_IDLE_RAM_DEBUG
        { C_INTH_IRQ, C_INTH_LEVEL,  0x01,    flash_access_handler        }, // 21 : EXTERNAL IRQ 1
        { C_INTH_IRQ, C_INTH_LEVEL,  0x01,    ext_ram_access_handler      }, // 22 : EXTERNAL IRQ 2
#else
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 21 : EXTERNAL IRQ 1
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 22 : EXTERNAL IRQ 2
#endif
  #if (OP_L1_STANDALONE == 0)
        { C_INTH_FIQ, C_INTH_LEVEL,  0x02,    bspUicc_Phy_intCHandler     }, // 23 : USIM Card Detect
        { C_INTH_IRQ, C_INTH_EDGE,   0x02,    bspUicc_Phy_intCHandler     }, // 24 : USIM
  #else
        { C_INTH_FIQ, C_INTH_LEVEL,  0x02,    f_inth_dummy                }, // 23 : USIM Card Detect
        { C_INTH_IRQ, C_INTH_EDGE,   0x02,    f_inth_dummy                }, // 24 : USIM
  #endif
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 25 : LCD
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 26 : USB
  #else
    #ifdef RVM_USB_SWE
        { C_INTH_IRQ, C_INTH_LEVEL,  0x00,    usb_int_handler             }, // 26 : USB
    #else
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                },
    #endif
  #endif
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 27 : MMC/SD/MS
  #else
    #ifdef RVM_MC_SWE
        { C_INTH_IRQ, C_INTH_LEVEL,  0x01,    mc_int_handler             }, // 27 : MMC/SD/MS
    #else
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                },
    #endif
  #endif
        { C_INTH_IRQ, C_INTH_LEVEL,  0x02,    SER_uart_modem2_handler     }, // 28 : UART_MODEM2
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_2nd_level_handler    }, // 29 : 2nd Interrupt Handler
  #else
        { C_INTH_IRQ, C_INTH_LEVEL,  0x02,    f_inth_2nd_level_handler    }, // 29 : 2nd Interrupt Handler
  #endif
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 30 : I2C
  #else
    #ifdef RVM_I2C_SWE
        { C_INTH_IRQ, C_INTH_LEVEL,  0x02,    i2c_hw_int_manager          }, // 30 : I2C
    #else
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                },
    #endif
  #endif
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 31 : NAND FLASH
  #else
    #ifdef RVM_NAN_SWE
        { C_INTH_IRQ, C_INTH_LEVEL,  0x02,    nan_IT_handler              }, // 31 : NAND FLASH
    #else
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                },
    #endif
  #endif

        { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    f_inth_dummy                }, // 0  : RNG
        { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    f_inth_dummy                }, // 1  : SHA1/MD5
        { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    f_inth_dummy                }, // 2  : EMPU
        { C_INTH_IRQ, C_INTH_LEVEL,   0x06,    f_dma_interrupt_manager     }, // 14 : DMA
        { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    f_inth_dummy                }  // 4  : Secure TIMER
      };

#if (GSM_IDLE_RAM != 0)
  const T_INTH_CONFIG a_inth_config_idle_ram[C_INTH_NB_INTERRUPT] =
        { // IRQ/FIQ  LEVEL/EDGE   PRIORITY   HANDLER
        { C_INTH_IRQ, C_INTH_EDGE,   0x01,    ext_ram_irq_inth_handler            }, // 0  : WATCHDOG TIMER
        { C_INTH_IRQ, C_INTH_EDGE,   0x02,    ext_ram_irq_inth_handler            }, // 1  : TIMER 1
        { C_INTH_IRQ, C_INTH_EDGE,   0x02,    ext_ram_irq_inth_handler            }, // 2  : TIMER 2
        { C_INTH_FIQ, C_INTH_EDGE,   0xFF,    ext_ram_fiq_inth_handler            }, // 3  : TSP RECEIVE
        { C_INTH_IRQ, C_INTH_EDGE,   0x00,    IQ_FrameHandler                     }, // 4  : TPU FRAME
        { C_INTH_IRQ, C_INTH_EDGE,   0x04,    ext_ram_irq_inth_handler            }, // 5  : TPU PAGE
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    ext_ram_irq_inth_handler            }, // 6  : SIM
  #else
        { C_INTH_IRQ, C_INTH_EDGE,   0x07,    ext_ram_irq_inth_handler            }, // 6  : SIM
  #endif
        { C_INTH_IRQ, C_INTH_LEVEL,  0x02,    ext_ram_irq_inth_handler            }, // 7  : UART_MODEM1
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0x01,    ext_ram_irq_inth_handler            }, // 8  : KEYBOARD
    #else
        { C_INTH_IRQ, C_INTH_LEVEL,  0x01,    ext_ram_irq_inth_handler            }, // 8  : KEYBOARD
    #endif
        { C_INTH_IRQ, C_INTH_EDGE,   0x03,    ext_ram_irq_inth_handler            }, // 9  : RTC_TIMER
        { C_INTH_IRQ, C_INTH_LEVEL,  0x03,    ext_ram_irq_inth_handler            }, // 10 : RTC_ALARM
        { C_INTH_IRQ, C_INTH_EDGE,   0x00,    IQ_Gauging_Handler                  }, // 11 : ULPD_GAUGING
        { C_INTH_IRQ, C_INTH_EDGE,   0x08,    ext_ram_irq_inth_handler            }, // 12 : ABB_IRQ
        { C_INTH_IRQ, C_INTH_EDGE,   0x05,    ext_ram_irq_inth_handler            }, // 13 : SPI
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0x06,    ext_ram_irq_inth_handler            }, // 14 : DMA
  #else
        { C_INTH_IRQ, C_INTH_EDGE,   0x02,    ext_ram_irq_inth_handler            }, // 14 : DMA
  #endif
        { C_INTH_IRQ, C_INTH_EDGE,   0x03,    ext_ram_irq_inth_handler            }, // 15 : API
        { C_INTH_IRQ, C_INTH_EDGE,   0x07,    ext_ram_irq_inth_handler            }, // 16 : GPIO
        { C_INTH_FIQ, C_INTH_EDGE,   0xFF,    ext_ram_fiq_inth_handler            }, // 17 : ABB_FIQ
        { C_INTH_IRQ, C_INTH_LEVEL,  0x02,    ext_ram_irq_inth_handler            }, // 18 : UART_IRDA
        { C_INTH_IRQ, C_INTH_LEVEL,  0x03,    IQ_GsmTim_Handler                   }, // 19 : ULPD GSM TIMER
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    ext_ram_irq_inth_handler            }, // 20 : GEA
#if GSM_IDLE_RAM_DEBUG
        { C_INTH_IRQ, C_INTH_LEVEL,  0x01,    flash_access_handler                }, // 21 : EXTERNAL IRQ 1
        { C_INTH_IRQ, C_INTH_LEVEL,  0x01,    ext_ram_access_handler              }, // 22 : EXTERNAL IRQ 2
#else
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    ext_ram_irq_inth_handler                        }, // 21 : EXTERNAL IRQ 1
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    ext_ram_irq_inth_handler                        }, // 22 : EXTERNAL IRQ 2
#endif
        { C_INTH_FIQ, C_INTH_LEVEL,  0xFF,    ext_ram_fiq_inth_handler            }, // 23 : USIM Card Detect
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    ext_ram_irq_inth_handler            }, // 24 : USIM
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    ext_ram_irq_inth_handler            }, // 25 : LCD
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    ext_ram_irq_inth_handler            }, // 26 : USB
  #else
        { C_INTH_IRQ, C_INTH_LEVEL,  0x00,    ext_ram_irq_inth_handler            }, // 26 : USB
  #endif
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    ext_ram_irq_inth_handler            }, // 27 : MMC/SD/MS
  #else
    #ifdef RVM_MC_SWE
        { C_INTH_IRQ, C_INTH_LEVEL,  0x01,    ext_ram_irq_inth_handler             }, // 27 : MMC/SD/MS
    #else
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    ext_ram_irq_inth_handler                },
    #endif
  #endif
        { C_INTH_IRQ, C_INTH_LEVEL,  0x02,    ext_ram_irq_inth_handler            }, // 28 : UART_MODEM2
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    ext_ram_irq_inth_handler            }, // 29 : 2nd Interrupt Handler
  #else
        { C_INTH_IRQ, C_INTH_LEVEL,  0x02,    ext_ram_irq_inth_handler            }, // 29 : 2nd Interrupt Handler
  #endif
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    ext_ram_irq_inth_handler            }, // 30 : I2C
  #else
        { C_INTH_IRQ, C_INTH_EDGE,   0x01,    ext_ram_irq_inth_handler            }, // 30 : I2C
  #endif
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    ext_ram_irq_inth_handler            }, // 31 : NAND FLASH
  #else
        { C_INTH_IRQ, C_INTH_EDGE,   0x02,    ext_ram_irq_inth_handler            }, // 31 : NAND FLASH
  #endif

        { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    ext_ram_irq_inth_handler           }, // 0  : RNG
        { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    ext_ram_irq_inth_handler           }, // 1  : SHA1/MD5
        { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    ext_ram_irq_inth_handler           }, // 2  : EMPU
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    ext_ram_irq_inth_handler            }, // 3  : Secure DMA
  #else
        { C_INTH_IRQ, C_INTH_LEVEL,  0x03,    ext_ram_irq_inth_handler            }, // 3  : Secure  DMA
  #endif
        { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    ext_ram_irq_inth_handler           }  // 4  : Secure TIMER
      };
#endif  //(GSM_IDLE_RAM != 0)

#endif   /* (CHIPSET ==12) */

#if (CHIPSET == 15)
 const T_INTH_CONFIG a_inth_config[C_INTH_NB_INTERRUPT] =
        { // IRQ/FIQ  LEVEL/EDGE   PRIORITY   HANDLER
        { C_INTH_IRQ, C_INTH_EDGE,   0x01,    IQ_TimerHandler             }, // 0  : WATCHDOG TIMER
        { C_INTH_IRQ, C_INTH_EDGE,   0x02,    IQ_TimerHandler1            }, // 1  : TIMER 1
        { C_INTH_IRQ, C_INTH_EDGE,   0x02,    IQ_TimerHandler2            }, // 2  : TIMER 2
        { C_INTH_FIQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 3  : MCSI
  #if (PSP_STANDALONE == 0)
        { C_INTH_IRQ, C_INTH_EDGE,   0x00,    IQ_FrameHandler             }, // 4  : TPU FRAME
  #else
	    { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 4  : TPU FRAME
  #endif
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 5  : TPU PAGE
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 6  : DRP DBB
  #else
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 6  : DRP DBB
  #endif
		{ C_INTH_IRQ, C_INTH_LEVEL,  0x02,    SER_uart_irda_handler       }, // 7  : UART_IRDA
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 8  : KEYBOARD
  #else
        { C_INTH_IRQ, C_INTH_LEVEL,  0x01,    IQ_KeypadHandler            }, // 8  : KEYBOARD
  #endif
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 9  : DRP DBB RX

    #ifdef RVM_CAMD_SWE

        { C_INTH_IRQ, C_INTH_LEVEL,  0x03,    f_camera_interrupt_manager  }, // 10 : CAMERA

   #else
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 10 : CAMERA

    #endif

  #if (PSP_STANDALONE == 0)
        { C_INTH_IRQ, C_INTH_EDGE,   0x00,    IQ_Gauging_Handler          }, // 11 : ULPD_GAUGING
  #else
		{ C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 11 : ULPD_GAUGING
  #endif
        { C_INTH_IRQ, C_INTH_EDGE,   0x08,    IQ_External                 }, // 12 : ABB_IRQ
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 13 : MSSPI
        { C_INTH_IRQ, C_INTH_LEVEL,  0x06,    f_dma_interrupt_manager     }, // 14 : DMA
  #if (PSP_STANDALONE == 0)
        { C_INTH_IRQ, C_INTH_EDGE,   0x03,    IQ_ApiHandler               }, // 15 : API
  #else
	    { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 15 : API
  #endif
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 16 : GPIO
        { C_INTH_FIQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 17 : ABB_FIQ
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy          	  }, // 18 : DRP DBB RX
  #if (PSP_STANDALONE == 0)
        { C_INTH_IRQ, C_INTH_LEVEL,  0x03,    IQ_GsmTim_Handler           }, // 19 : ULPD GSM TIMER
  #else
	    { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy	              }, // 19 : ULPD GSM TIMER
  #endif
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 20 : GEA
  #if GSM_IDLE_RAM_DEBUG
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 21 : GPIO1
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 22 : GPIO2
  #else
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 21 : GPIO1
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 22 : GPIO2
  #endif
        { C_INTH_FIQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 23 : CPORT
  #if(OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                }, // 24 : USIM
  #else
        { C_INTH_IRQ, C_INTH_EDGE,   0x04,    bspUicc_Phy_intCHandler     }, // 24 : USIM
  #endif
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 25 : LCD
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 26 : USB
  #else
    #ifdef RVM_USB_SWE
        { C_INTH_IRQ, C_INTH_LEVEL,  0x03,    usb_int_handler             }, // 26 : USB
    #else
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                },
    #endif
  #endif
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 27 : not used
        { C_INTH_IRQ, C_INTH_LEVEL,  0x03,    bspI2c_Handeler2		      }, // 28 : I2C TRITON
        { C_INTH_IRQ, C_INTH_LEVEL,  0x02,    f_inth_2nd_level_handler    }, // 29 : 2nd Interrupt Handler
        { C_INTH_IRQ, C_INTH_LEVEL,  0x03,    bspI2c_Handeler1		      }, // 30 : I2C
  #if (OP_L1_STANDALONE == 1)
        { C_INTH_IRQ, C_INTH_LEVEL,  0xFF,    f_inth_dummy                }, // 31 : NAND FLASH
  #else
    #ifdef RVM_NAN_SWE
        { C_INTH_IRQ, C_INTH_LEVEL,  0x02,    nan_IT_handler              }, // 31 : NAND FLASH
    #else
        { C_INTH_IRQ, C_INTH_EDGE,   0xFF,    f_inth_dummy                },
    #endif
  #endif

        { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    f_inth_dummy                }, // 0  : RNG
        { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    f_inth_dummy                }, // 1  : SHA1/MD5
        { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    f_inth_dummy                }, // 2  : EMPU
        { C_INTH_IRQ, C_INTH_LEVEL,   0x06,    f_dma_interrupt_manager     }, // 14 : DMA
        { C_INTH_IRQ, C_INTH_LEVEL,   0xFF,    f_inth_dummy                }  // 4  : Secure TIMER
    };
#endif /* CHIPSET == 15 */

#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END
#endif // !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM !=0))

#if (CHIPSET == 12)
  const T_DMA_TYPE_GLOBAL_PARAMETER d_dma_global_parameter=
      {
        #if (CHIPSET_PG == CP_PG_F751685A)
          C_DMA_AUTO_GATE_ON,
        #else
          C_DMA_AUTO_GATE_OFF,
        #endif /* endif (CHIPSET_PG == F751685A) */
        C_DMA_API_PRIO_ARM,
        C_DMA_RHEA_PRIO_ARM,
        C_DMA_IMIF_PRIO_CPU_4
      };

#endif   /* (CHIPSET ==12) */

#if (CHIPSET == 15)
  const T_DMA_TYPE_GLOBAL_PARAMETER d_dma_global_parameter=
      {
          C_DMA_AUTO_GATE_ON,
          C_DMA_API_PRIO_ARM,
          C_DMA_RHEA_PRIO_ARM,
          C_DMA_IMIF_PRIO_CPU_4,
          C_DMA_IPERIF_PRIO_CPU_4,  // set same as the IMIF priority. Actual value need to decided
          C_DMA_EMIF_DMA_PRIO_7,        // Actual value need to be decided
          C_DMA_EMIF_MCU_PRIO_5         // Actual value need to be decided
      };

#endif


#if (GSM_IDLE_RAM != 0)

  #define IRQ_STACK_SIZE  128
  #pragma DATA_SECTION(irq_stack,".irqstk");
  UWORD32 irq_stack[IRQ_STACK_SIZE/4];
  const UWORD32 irq_stack_size = IRQ_STACK_SIZE;

  #define FIQ_STACK_SIZE  512
  #pragma DATA_SECTION(fiq_stack,".fiqstk");
  UWORD32 fiq_stack[FIQ_STACK_SIZE/4];
  const UWORD32 fiq_stack_size = FIQ_STACK_SIZE;

  #define SVC_STACK_SIZE  1024
  #pragma DATA_SECTION(svc_stack,".svcstk");
  UWORD32 svc_stack[SVC_STACK_SIZE/4];
  const UWORD32 svc_stack_size = SVC_STACK_SIZE;

  #define TIMER_HISR_STACK_SIZE  1024
  #pragma DATA_SECTION(timer_hisr_stack,".timstk");
  UWORD32 timer_hisr_stack[TIMER_HISR_STACK_SIZE/4];
  const UWORD32 timer_hisr_stack_size = TIMER_HISR_STACK_SIZE;
#endif

/* HISR_STACK_SHARING: Create global stacks to be used by all HISRs
 * having the same priority */



#if (CODE_VERSION != SIMULATION)
/*
 * Init_Target
 *
 * Performs low-level HW Initialization.
 */
void Init_Target(void)
{

	
  #if (BOARD==70)|| (BOARD==71)
  /* Variable for reading the BCR for MCP RAM */
  unsigned short bcrTmpVal;
  #endif
 (*(volatile Uint16 *) 0xFFFF702A) = 0x0002;//reset the UART module.
#if (CHIPSET == 15)
        char detect_code[80];
        typedef void (*pf_t)(UWORD32, UWORD16 *, UWORD16 *);
	extern void ffsdrv_device_id_read(UWORD32 base_addr, UWORD16 *manufact, UWORD16 *device);
        pf_t myfp;
	UWORD16 manufact;
        UWORD16 device_id[3];

#endif
  #if (BOARD == 5)
    #define WS_ROM  (1)
    #define WS_RAM  (1)
    #define WS_APIF (1)
    #define WS_CS2  (7) /* LCD on EVA3. */
    #define WS_CS0  (7) /* DUART on EVA3. UART16750 and latch on A-Sample. */
    #define WS_CS1  (7) /* LCD on A-Sample. */

    IQ_InitWaitState (WS_ROM, WS_RAM, WS_APIF, WS_CS2, WS_CS0, WS_CS1);
    IQ_InitClock (2); /* Internal clock division factor. */

    IQ_MaskAll (); /* Mask all interrupts. */
    IQ_SetupInterrupts (); /* IRQ priorities. */

    TM_DisableWatchdog ();

    /*
     * Reset all TSP and DBG fdefault values
     */

    AI_ResetTspIO ();
    AI_ResetDbgReg ();
    AI_ResetIoConfig ();

    /*
     * Warning! The external reset signal is connected to the Omega and the
     * external device. If the layer 1 is used its initialization removes
     * the external reset. If the application does not use the layer 1
     * you must remove the external reset (bit 2 of the reset control
     * register 0x505808).
     */

    AI_ResetTspIO();
    AI_ResetDbgReg();
    AI_ResetIoConfig();

    /*
     * Configure all IOs (see RD300 specification).
     */

    AI_ConfigBitAsInput (1);
    AI_EnableBit (1);

    AI_ConfigBitAsOutput (2);
    AI_EnableBit (2);

    AI_ConfigBitAsInput (11);
    AI_EnableBit (11);

    AI_ConfigBitAsOutput (13);
    AI_EnableBit (13);

    AI_Power (1); /* Maintain power supply. */

  #elif (BOARD == 6) || (BOARD == 7) || (BOARD == 8) || (BOARD == 9) || \
        (BOARD == 40) || (BOARD == 41) || (BOARD == 42) || (BOARD == 43) || (BOARD == 45) || \
        (BOARD == 35) || (BOARD == 46) || (BOARD == 70) || (BOARD == 71)

#if (PSP_STANDALONE == 0)
    // RIF/SPI rising edge clock for ULYSSE
    //--------------------------------------------------
    #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3)|| (ANLG_FAM == 11))
      #if ((CHIPSET >= 3))
        #if (CHIPSET == 12)
          F_CONF_RIF_RX_RISING_EDGE;
          F_CONF_SPI_RX_RISING_EDGE;
        #elif (CHIPSET == 15)
	     //do the DRP init here for Locosto
	     #if (L1_DRP == 1)
	     //  drp_power_on(); This should be done after the script is downloaded.
	     #endif
        #else
          #if (BOARD==35)
            *((volatile SYS_UWORD16 *) ASIC_CONF) = 0x2000;
          #else
            *((volatile SYS_UWORD16 *) ASIC_CONF) = 0x6000;
          #endif   /* (BOARD == 35) */
        #endif
      #endif
    #endif   /* ANLG(ANALOG)) */

    #if (OP_L1_STANDALONE == 1)
      #if (BOARD == 40) || (BOARD == 41) || \
            (BOARD == 42) || (BOARD == 43) || (BOARD == 45)
        // enable 8 Ohm amplifier for audio on D-sample
        AI_ConfigBitAsOutput (1);
        AI_SetBit(1);
      #elif (BOARD == 70) || (BOARD == 71)
	  //Locosto I-sample or UPP costo board.BOARD
	  // Initialize the ARMIO bits as per the I-sample spec
	  // FIXME
      #endif
    #endif   /* (OP_L1_STANDALONE == 1) */
#endif /* PSP_STANDALONE ==0 */

    // Watchdog
    //--------------------------------------------------
    TM_DisableWatchdog();    /* Disable Watchdog */
    #if (CHIPSET == 12) || (CHIPSET == 15)
      TM_SEC_DisableWatchdog();
    #endif

    #if ((CHIPSET == 4) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 11) || (CHIPSET == 12) || (CHIPSET == 15))

      #if (CHIPSET == 12)

        #if 0   /* example of configuration for DMA debug */
          #if (BOARD == 6)  /* debug on EVA 4 , GPO2 must not be changed */

            /*  TPU_FRAME, NMIIT, IACKn  */
            F_DBG_IRQ_CONFIG(C_DBG_IRQ_IRQ4|C_DBG_IRQ_NMIIT|C_DBG_IRQ_IACKN);

            /*  NDMA_REQ_VIEW1, NDMA_REQ_VIEW0, DMA_V(1), DMA_S(1), DMAREQ_P1(3:0)*/
            F_DBG_DMA_P1_NDFLASH_CONFIG(C_DBG_DMA_P1_NDFLASH_NDMA_REQ_VIEW_1  |
                                        C_DBG_DMA_P1_NDFLASH_NDMA_REQ_VIEW_0  |
                                        C_DBG_DMA_P1_NDFLASH_DMA_REQ_P1_3     |
                                        C_DBG_DMA_P1_NDFLASH_DMA_REQ_P1_2     |
                                        C_DBG_DMA_P1_NDFLASH_DMA_REQ_P1_1     |
                                        C_DBG_DMA_P1_NDFLASH_DMA_REQ_P1_0     |
                                        C_DBG_DMA_P1_NDFLASH_DMA_REQ_S_1      |
                                        C_DBG_DMA_P1_NDFLASH_DMA_REQ_V1       );
            /*  DMA_REQ_S(2)*/
            F_DBG_DMA_P2_CONFIG(C_DBG_DMA_P2_DMA_REQ_S2);

            /*  DMA_CLK_REQ, BRIDGE_CLK */
            F_DBG_CLK1_CONFIG(C_DBG_CLK1_DMA_CLK_REQ  |
                              C_DBG_CLK1_BRIDGE_CLK   );

            /*  XIO_nREADY */
            F_DBG_IMIF_CONFIG(C_DBG_IMIF_XIO_NREADY_MEM);

            /* DSP_nIRQ_VIEW1, DSP_nIRQ_VIEW0, BRIDGE_EN */
            F_DBG_KB_USIM_SHD_CONFIG(C_DBG_KB_USIM_SHD_DSP_NIRQ_VIEW_1  |
                                     C_DBG_KB_USIM_SHD_DSP_NIRQ_VIEW_0  |
                                     C_DBG_KB_USIM_SHD_BRIDGE_EN        );

            /* RHEA_nREADY , RHEA_nSTROBE */
            F_DBG_USIM_CONFIG(C_DBG_USIM_RHEA_NSTROBE |
                              C_DBG_USIM_RHEA_NREADY  );

            /* XIO_STROBE */
            F_DBG_MISC2_CONFIG(C_DBG_MISC2_X_IOSTRBN);

            /* DMA_CLK_REQ */
            F_DBG_CLK2_CONFIG(C_DBG_CLK2_DMA_CLK_REQ2);

            /* DSP_IRQ_SEL0=DMA, DSP_IRQ_SEL1=DMA, DMA_REQ_SEL0=RIF_RX, DMA_REQ_SEL1=RIF_RX */
            F_DBG_VIEW_CONFIG(0,0,C_DBG_DSP_INT_DMA,
                                  C_DBG_DSP_INT_DMA,
                                  C_DMA_CHANNEL_RIF_RX,
                                  C_DMA_CHANNEL_RIF_RX);

          #endif   /* (BOARD == 6) */
        #endif   /* DMA debug example */
      #else
        /*
         *  Configure ASIC in order to output the DPLL and ARM clock
         */
       // (*( volatile UWORD16* )(0xFFFEF008)) = 0x8000; // DPLL
       // (*( volatile UWORD16* )(0xFFFEF00E)) = 0x0004; // ARM clock
       // (*( volatile UWORD16* )(0xfffef004)) = 0x0600; // DSP clock + nIACK
      #endif   /* (CHIPSET == 12) || CHIPSET == 15*/


      /*
       *  Enable/Disable of clock switch off for INTH, TIMER, BRIDGE and DPLL modules
       */
      // IRQ, Timer and bridge may SLEEP
      // In first step, same configuration as SAMSON
      //--------------------------------------------------
      #if (CHIPSET == 12)
        CLKM_INITCNTL(CLKM_IRQ_DIS | CLKM_TIMER_DIS | CLKM_BRIDGE_DIS | CLKM_DPLL_DIS);
	  #elif (CHIPSET == 15)
CLKM_INITCNTL(CLKM_IRQ_DIS | CLKM_TIMER_DIS | CLKM_CPORT_EN | CLKM_BRIDGE_DIS | 0x8000 ); /* CLKM_DPLL_DIS is remove by Ranga*/

      #else
        CLKM_INITCNTL(CLKM_IRQ_DIS | CLKM_TIMER_DIS);

        // Select VTCXO input frequency
        //--------------------------------------------------
        CLKM_UNUSED_VTCXO_26MHZ;

        // Rita RF uses 26MHz VCXO
        #if (RF_FAM == 12)
          CLKM_USE_VTCXO_26MHZ;
        #endif
        // Renesas RF uses 26MHz on F-sample but 13MHz on TEB
        #if (RF_FAM == 43) && (BOARD == 46)
          CLKM_USE_VTCXO_26MHZ;
        #endif
      #endif

	  #if (CHIPSET ==15)
 	 //Enable APLL
 	 *((volatile unsigned short *) (C_MAP_CLKM_BASE+0x10)) = 0x01|0x6;
	 // UART Clock from APLL
	 *((volatile unsigned short *) CLKM_CNTL_CLK_PROG_FREE_RUNN) = 0x0001;
         (*(volatile Uint16 *) 0xFFFF702A) = 0x0002;//reset the UART module.
	  
	#endif




	 //
      // Control HOM/SAM automatic switching
      //--------------------------------------------------
      *((volatile unsigned short *) CLKM_CNTL_CLK) &= ~CLKM_EN_IDLE3_FLG;

      /*
       *  Disable and Clear all pending interrupts
       */
      #if (CHIPSET == 12) || (CHIPSET == 15)
        F_INTH_DISABLE_ALL_IT;           // MASK all it
        F_INTH2_VALID_NEXT(C_INTH_IRQ);  // reset current IT in INTH2 IRQ
        F_INTH_VALID_NEXT(C_INTH_IRQ);   // reset current IT in INTH IRQ
        F_INTH_VALID_NEXT(C_INTH_FIQ);   // reset current IT in INTH FIQ
        F_INTH_RESET_ALL_IT;             // reset all IRQ/FIQ source
      #else
        INTH_DISABLEALLIT;
        INTH_RESETALLIT;
        INTH_CLEAR;                 /* reset IRQ/FIQ source */
      #endif

      #if (CHIPSET == 12)
        /* API-RHEA control register configuration */
        f_memif_init_api_rhea_ctrl(C_MEMIF_API_RHEA_ADAPT,
                                   C_MEMIF_API_RHEA_ADAPT,
                                   C_MEMIF_API_RHEA_ADAPT,
                                   C_MEMIF_API_RHEA_NO_DEBUG);

        #if ((BOARD == 43) || (BOARD == 45))
          // if Esample,Evaconso active extended page mode
          // With Calypso+ chipset, extended page mode can be enabled
          // only if W_A_CALYPSO_PLUS_SPR_19599 is set to one in l1_confg.h.
          // see L1_MCU-SPR-17515 and L1_MCU-SPR-19599 for more information
          f_memif_extended_page_mode_enable();
	    #endif
      #endif   /* (CHIPSET == 12) */

	  #if (CHIPSET == 15)
        /* API-RHEA control register configuration */
        f_emif_api_rhea_conf(C_RHEA_STROBE0_ACCESS_SIZE_ADAPT_ENABLE,
                             C_RHEA_STROBE1_ACCESS_SIZE_ADAPT_ENABLE,
                             C_API_ACCESS_SIZE_ADAPT_ENABLE,
                             C_ARM_DEBUG_DISABLE);
        #if (BOARD == 70) || (BOARD == 71)
	      // set the EMIF settings here for locosto
	      // We could have the default settings here and
	      // then change it after dynamic clock config
	      /* MCP RAM Setting Is being done here */
#if 0 /* This is commented out by Ranga */
		#if (PSP_FAILSAFE!=1)
		  bcrTmpVal = *((volatile unsigned char *)0x007FFFFFE);
		  bcrTmpVal +=1;
		  bcrTmpVal = *((volatile unsigned char *)0x007FFFFFE);
		  *((volatile unsigned char *)0x007FFFFFE)=0x0001;
		  *((volatile unsigned char *)0x007FFFFFE)=0x1542;

		  /* Setting NOR Flash to these 3 Wait State */
		  *((volatile char *)0x06000AAA)=0xAA;
		  *((volatile char *)0x06000555)=0x55;
		  *((volatile char *)0x06016AAA)=0xC0;
        #endif
#endif
	    #endif
      #endif   /* (CHIPSET == 15) */


      /*
       * Initialize current DSP clock to 0 in order to pass through
       * the right DSP latency configuration (increase DSP clock)
       * in f_dynamic_clock_cfg().
       * Obviously, the real DSP clock is not 0kHz.
       * d_dsp_cur_clk will be updated after clock configuration in f_dynamic_clock_cfg().
       */
      d_dsp_cur_clk = 0;   // Used to keep track of current DSP clock.

      /* Dynamic clock configuration */
      f_dynamic_clock_cfg(C_CLOCK_CFG_DEFAULT);

      // Write_en_0 = 0 , Write_en_1 = 0
      RHEA_INITARM(0,0);

	  #if (CHIPSET ==15)
	  // Mark USB on 52 MHZ Clock
	  *((volatile unsigned short *) (CLKM_CNTL_CLK_USB)) = 0x02;
	  #endif

      #if (CHIPSET == 12) || ((CHIPSET == 10) && (OP_WCP == 1))
        /* Allocate the 0.5 Mbits Shared RAM to the DSP */
        f_memif_shared_sram_allocation(C_MEMIF_DSPMS_0_5MBITS_TO_DSP);
      #endif

      // INTH
      //--------------------------------------------------
      #if (CHIPSET == 12) || (CHIPSET == 15)
        #if (GSM_IDLE_RAM != 0)
          f_inth_setup((T_INTH_CONFIG *)a_inth_config_idle_ram);   // setup configuration IT handlers
        #else
          f_inth_setup((T_INTH_CONFIG *)a_inth_config);   // setup configuration IT handlers
        #endif
      #else
        IQ_SetupInterrupts();
      #endif


      #if (CHIPSET == 12) || (CHIPSET == 15)
      #if (OP_L1_STANDALONE == 0)

        f_dma_global_parameter_set((T_DMA_TYPE_GLOBAL_PARAMETER *)&d_dma_global_parameter);
      #endif
        f_dma_channel_allocation_set(C_DMA_CHANNEL_0, C_DMA_CHANNEL_DSP);
     #if (OP_L1_STANDALONE == 1)
        f_dma_global_parameter_set((T_DMA_TYPE_GLOBAL_PARAMETER *)&d_dma_global_parameter);
        f_dma_channel_allocation_set(C_DMA_CHANNEL_0, C_DMA_CHANNEL_DSP);
     #endif

      #else
        // DMA
        //--------------------------------------------------
        // channel0 = Arm, channel1 = Lead, channel2 = forced to Arm, channel3=forced to Arm, dma_burst = 0001, priority = same
      #if (OP_L1_STANDALONE == 0)
        DMA_ALLOCDMA(1,0,1,1);  // Channel 1 used by DSP with RIF RX
      #endif
      #endif

      /* CHIPSET = 4 or 7 or 8 or 10 or 11 or 12 */

    #else

      // RHEA Bridge
      //--------------------------------------------------
      // ACCES_FAC_0 = 0, ACCES_FAC_1 = 0 ,TIMEOUT = 0x7F
      RHEA_INITRHEA(0,0,0x7F);

      #if (CHIPSET == 6)
        // WS_H = 1 , WS_L = 15
        RHEA_INITAPI(1,15);          // should be 0x01E1 for 65 Mhz
      #else
        // WS_H = 0 , WS_L = 7
        RHEA_INITAPI(0,7);           // should be 0x0101 for 65 Mhz
      #endif

      // Write_en_0 = 0 , Write_en_1 = 0
      RHEA_INITARM(0,0);

      // INTH
      //--------------------------------------------------
      INTH_DISABLEALLIT;          // MASK all it
      INTH_CLEAR;                 // reset IRQ/FIQ source
      IQ_SetupInterrupts();

      // DMA
      //--------------------------------------------------
      // channel0 = Arm, channel1 = Lead, dma_burst = 0001, priority = same
      DMA_ALLOCDMA(1,0,1,1);      // should be 0x25   (channel 1 = lead)

      #if (CHIPSET == 6)
        // Memory WS configuration for ULYSS/G1 (26 Mhz) board
        //-----------------------------------------------------
        MEM_INIT_CS2(2,MEM_DVS_16,MEM_WRITE_EN,0);
      #endif

      // CLKM
      //--------------------------------------------------
      CLKM_InitARMClock(0x00, 2); /* no low freq, no ext clock, div by 1 */

      #if (CHIPSET == 6)
        CLKM_INITCNTL(CLKM_IRQ_DIS | CLKM_BRIDGE_DIS | CLKM_TIMER_DIS | CLKM_VTCXO_26);
      #else
        CLKM_INITCNTL(CLKM_IRQ_DIS | CLKM_BRIDGE_DIS | CLKM_TIMER_DIS);
      #endif

    #endif   /* CHIPSET = 4 or 7 or 8 or 10 or 11 or 12 */

    // Freeze ULPD timer ....
    //--------------------------------------------------
    *((volatile SYS_UWORD16 *) ULDP_GSM_TIMER_INIT_REG ) = 0;
    *((volatile SYS_UWORD16 *) ULDP_GSM_TIMER_CTRL_REG ) = TPU_FREEZE;

    // reset INC_SIXTEEN and INC_FRAC
    //--------------------------------------------------
    #if (OP_L1_STANDALONE == 1)
      l1ctl_pgm_clk32(DEFAULT_HFMHZ_VALUE,DEFAULT_32KHZ_VALUE);
    #else
      ULDP_INCSIXTEEN_UPDATE(132);    //32768.29038  =>132, 	32500 => 133
                                      // 26000 --> 166
      ULDP_INCFRAC_UPDATE(15840);     //32768.29038  =>15840,	32500 => 21845
                                      // 26000 --> 43691
    #endif   /*  OP_L1_STANDALONE */

    // program ULPD WAKE-UP ....
    //=================================================
    #if (CHIPSET == 2)
       *((volatile SYS_UWORD16 *)ULDP_SETUP_FRAME_REG)  = SETUP_FRAME;  // 2 frame
       *((volatile SYS_UWORD16 *)ULDP_SETUP_VTCXO_REG)  = SETUP_VTCXO;  // 31 periods
       *((volatile SYS_UWORD16 *)ULDP_SETUP_SLICER_REG) = SETUP_SLICER; // 31 periods
       *((volatile SYS_UWORD16 *)ULDP_SETUP_CLK13_REG)  = SETUP_CLK13;  // 31 periods
    #else
       *((volatile SYS_UWORD16 *)ULDP_SETUP_FRAME_REG)  = SETUP_FRAME;  // 3 frames
       *((volatile SYS_UWORD16 *)ULDP_SETUP_VTCXO_REG)  = SETUP_VTCXO;  // 0 periods
       *((volatile SYS_UWORD16 *)ULDP_SETUP_SLICER_REG) = SETUP_SLICER; // 31 periods
       *((volatile SYS_UWORD16 *)ULDP_SETUP_CLK13_REG)  = SETUP_CLK13;  // 31 periods
       *((volatile SYS_UWORD16 *)ULPD_SETUP_RF_REG)     = SETUP_RF;     // 31 periods
    #endif

    #if (CHIPSET == 15)
      *((volatile SYS_UWORD16 *)ULPD_DCXO_SETUP_SLEEPN)    = SETUP_SLEEPZ;    // 0
      *((volatile SYS_UWORD16 *)ULPD_DCXO_SETUP_SYSCLKEN)  = SETUP_SYSCLKEN;  // 255 clocks of 32 KHz for 7.8 ms DCXO delay for Locosto
	  *((volatile SYS_UWORD16 *)0xFFFEF192) = 0x1; //CLRZ
  	  *((volatile SYS_UWORD16 *)0xFFFEF190) = 0x2; //SLPZ
	  *((volatile SYS_UWORD16 *)0xFFFEF18E)= 0x2; //SYSCLKEN
	  *((volatile SYS_UWORD16 *)0xFFFEF186) = 0x2; //CLK13_EN
	  *((volatile SYS_UWORD16 *)0xFFFEF18A) = 0x2; //DRP_DBB_SYSCLK




    #endif

    // Set Gauging versus HF (PLL)
    //=================================================
    ULDP_GAUGING_SET_HF;                // Enable gauging versus HF
    ULDP_GAUGING_HF_PLL;                // Gauging versus PLL

    // current supply for quartz oscillation
    //=================================================
    #if (OP_L1_STANDALONE == 1)
      #if ((CHIPSET != 9) && (CHIPSET != 12) && (CHIPSET !=15)) // programming model changed for Ulysse C035, stay with default value
        *(volatile SYS_UWORD16 *)QUARTZ_REG  = 0x27;
      #endif
    #else
      #if ((BOARD == 6) || (BOARD == 8) || (BOARD == 9) || (BOARD == 35) || (BOARD == 40) || (BOARD == 41))
        *((volatile SYS_UWORD16 *)QUARTZ_REG)  = 0x27;
      #elif (BOARD == 7)
        *((volatile SYS_UWORD16 *)QUARTZ_REG)  = 0x24;
      #endif
    #endif   /* OP_L1_STANDALONE */

    // stop Gauging if any (debug purpose ...)
    //--------------------------------------------------
    if ( *((volatile SYS_UWORD16 *) ULDP_GAUGING_CTRL_REG) & ULDP_GAUGING_EN)
    {
      volatile UWORD32 j;
      ULDP_GAUGING_STOP; /* Stop the gauging */
      /* wait for gauging it*/
      // one 32khz period = 401 periods of 13Mhz
      for (j=1; j<50; j++);
      while (! (* (volatile SYS_UWORD16 *) ULDP_GAUGING_STATUS_REG) & ULDP_IT_GAUGING);
    }

    #if (OP_L1_STANDALONE == 0)
      AI_ClockEnable ();

      #if (BOARD == 7)
        // IOs configuration of the B-Sample in order to optimize the power consumption
        AI_InitIOConfig();

        // Set LPG instead of DSR_MODEM
        *((volatile SYS_UWORD16 *) ASIC_CONF) |= 0x40;
        // Reset the PERM_ON bit of LCR_REG
        *((volatile SYS_UWORD16 *) MEM_LPG) &= ~(0x80);
      #elif ((BOARD == 8) || (BOARD == 9))
        // IOs configuration of the C-Sample in order to optimize the power consumption
        AI_InitIOConfig();

        // set the debug latch to 0x00.
        *((volatile SYS_UWORD8 *) 0x2800000) = 0x00;
      #elif ((BOARD == 35) || (BOARD == 46))
        AI_InitIOConfig();
        // CSMI INTERFACE
        // Initialize CSMI clients for GSM control
        // and Fax/Data services
          CSMI_Init();
          GC_Initialize();  // GSM control initialization
          CU_Initialize();  // Trace initialization
          CF_Initialize();  // Fax/Data pre-initialization
      #elif ((BOARD == 40) || (BOARD == 41))
        // IOs configuration of the D-Sample in order to optimize the power consumption
        AI_InitIOConfig();

        #ifdef BTEMOBILE
          // Reset BT chip by toggling the Island's nRESET_OUT signal
          *((volatile SYS_UWORD16 *) 0xFFFFFD04) |= 0x04;
          *((volatile SYS_UWORD16 *) 0xFFFFFD04) &= ~(0x4);
        #endif

        // set the debug latch to 0x0000.
 	    *((volatile SYS_UWORD16 *) 0x2700000) = 0x0000;
      #elif ((BOARD == 70) || (BOARD == 71))
	    AI_InitIOConfig();
	    /* Mark The System configuration According to I-Sample */
		/* Adding GPIO Mux Setting Here */
		pin_configuration_all(); // Init Tuned for Power Management
		/* A22 is Enabled in int.s hence not Here */
		/* FIXME: PULL_UP Enable and PULL UP Values Need to revisited */

	/* Add code to find out the manufacture id of NOR flash*/

        // Copy ffsdrv_device_id_read() function code to RAM. The only known
        // way to determine the size of the code is to look either in the
        // linker-generated map file or in the assember output file.
        ffsdrv_copy_code_to_ram((UWORD16 *) detect_code,
                                (UWORD16 *) &ffsdrv_device_id_read,
                                sizeof(detect_code));

        // Combine bit 0 of the thumb mode function pointer with the address
        // of the code in RAM. Then call the detect function in RAM.
        myfp = (pf_t) (((int) &ffsdrv_device_id_read & 1) | (int) detect_code);
        (*myfp)(0x06000000, &manufact, device_id);

	enable_ps_ram_burst();

	if( 0x7e == device_id[0] )
	{
	   enable_flash_burst();
	   flash_device_id = 0x7E;
	}
	else
	{
	   enable_flash_burst_mirror();
   	   flash_device_id = 0;
	}


	 asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");

#if 0	// Init Changed for tuning to Power Management	 -Old Init Commented
		/* Ball N9 Mapped to TSPACT_8 (TPU) */
		F_IO_CONFIG(C_CONF_GPIO_5,C_CONF_PUPD_EN|0x01);
		/* Ball A6 ND_WE */
		F_IO_CONFIG(C_CONF_GPIO_18,C_CONF_PUPD_VAL|C_CONF_PUPD_EN|0x01);
		/* Ball C2 ND_RDY */
		F_IO_CONFIG(C_CONF_GPIO_34,C_CONF_PUPD_VAL|C_CONF_PUPD_EN|0x01);
		/* Ball C3 ND_RE */
		F_IO_CONFIG(C_CONF_GPIO_31,C_CONF_PUPD_VAL|C_CONF_PUPD_EN|0x01);
		/* BALL E5 CAM_D_4 */
		F_IO_CONFIG(C_CONF_ND_NWP,0x02);
		/* BALL F6 ND_CLE */
		F_IO_CONFIG(C_CONF_GPIO_32,C_CONF_PUPD_EN|0x01);
		/* BALL H8 ND_ALE */
		F_IO_CONFIG(C_CONF_GPIO_33,C_CONF_PUPD_EN|0x01);
		/* BALL E10 LCD_NCS0 */
		F_IO_CONFIG(C_CONF_GPIO_13,C_CONF_PUPD_VAL|C_CONF_PUPD_EN|0x01);
		/* BALL C11 GPIO_10 */
		F_IO_CONFIG(C_CONF_GPIO_10,C_CONF_PUPD_VAL|C_CONF_PUPD_EN|0x01);
		/* BALL D10 GPIO_11 */
		F_IO_CONFIG(C_CONF_GPIO_11,C_CONF_PUPD_VAL|C_CONF_PUPD_EN|0x01);
		/* BALL M6 CAM_D_1 */
		F_IO_CONFIG(C_CONF_GPIO_0,C_CONF_PUPD_VAL|C_CONF_PUPD_EN|0x02);
		/* BALL N5 CAM_D_0 */
		F_IO_CONFIG(C_CONF_GPIO_47,C_CONF_PUPD_VAL|C_CONF_PUPD_EN|0x02);
		/* BALL A5 CAM_LCLK */
		F_IO_CONFIG(C_CONF_GPIO_21,0x01);
		/* BALL C6 CAM_XCLK */
		F_IO_CONFIG(C_CONF_GPIO_22,0x01);
		/* BALL E7 CAM_VS */
		F_IO_CONFIG(C_CONF_GPIO_20,0x01);
		/* BALL F8 CAM_HS */
		F_IO_CONFIG(C_CONF_GPIO_19,0x01);
		/* BALL K7 MCSI_TX */
		F_IO_CONFIG(C_CONF_GPIO_45,C_CONF_PUPD_VAL|C_CONF_PUPD_EN|0x01);
		/* BALL M5 MCSI_FS */
		F_IO_CONFIG(C_CONF_GPIO_44,C_CONF_PUPD_VAL|C_CONF_PUPD_EN|0x01);
		/* BALL N3 MCSI_CK */
		F_IO_CONFIG(C_CONF_GPIO_43,C_CONF_PUPD_VAL|C_CONF_PUPD_EN|0x01);
		/* BALL P2 MCSI_RX */
		F_IO_CONFIG(C_CONF_GPIO_46,C_CONF_PUPD_VAL|C_CONF_PUPD_EN|0x01);
		/* BALL B11 TSPACT_10 */
		F_IO_CONFIG(C_CONF_GPIO_12,C_CONF_PUPD_EN|0x01);
		/* BALL B3 CAM_D_5 */
		F_IO_CONFIG(C_CONF_GPIO_30,C_CONF_PUPD_EN|0x03);
		/* BALL C4 CAM_D_7 */
		F_IO_CONFIG(C_CONF_GPIO_28,C_CONF_PUPD_VAL|C_CONF_PUPD_EN|0x03);
		/* BALL C5 SPI_DATA_MOSI */
		F_IO_CONFIG(C_CONF_GPIO_25,C_CONF_PUPD_EN|0x01);
		/* BALL E6 SPI_NCS0 */
		F_IO_CONFIG(C_CONF_GPIO_26,C_CONF_PUPD_EN|0x01);
		/* BALL F7 SPI_DATA_MIS0 */
		F_IO_CONFIG(C_CONF_GPIO_24,C_CONF_PUPD_EN|0x03);
		/* BALL G6 CAM_D_2 */
		F_IO_CONFIG(C_CONF_GPIO_7,C_CONF_PUPD_EN|0x05);
		/* BALL G7 CAM_D_6 */
		F_IO_CONFIG(C_CONF_GPIO_29,C_CONF_PUPD_EN|0x03);
		/* BALL G8 SPI_NCS1 */
		F_IO_CONFIG(C_CONF_GPIO_27,C_CONF_PUPD_VAL|C_CONF_PUPD_EN|0x01);
		/* BALL G9 SPI_CLK */
		F_IO_CONFIG(C_CONF_GPIO_23,C_CONF_PUPD_EN|0x01);
		/* BALL L6 CKM */
		F_IO_CONFIG(C_CONF_GPIO_42,C_CONF_PUPD_VAL|C_CONF_PUPD_EN|0x01);

		/*By default the muxed bus is given to LCD*/
		C_CONF_LCD_CAM_NAN_REG=0x03;

#endif // for #if 0 	Init Changed for Power Management
      #endif // BOARD

      // Enable HW Timers 1 & 2
      TM_EnableTimer (1);
      TM_EnableTimer (2);

      	 asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
 	 asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
	 asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
    #endif  /* (OP_L1_STANDALONE == 0) */

  #endif   /* #if (BOARD == 5) */
#if(OP_L1_STANDALONE == 1 && MIRROR_BIT == 1 ) //temp FIX for L1 standalone-this fix will work only for I-sample mirror bit
//#if(OP_L1_STANDALONE == 1 )
	 //AI_InitIOConfig();
	 //pin_configuration_all(); // Init Tuned for Power Management
  	 //enable_ps_ram_burst();
	 //enable_flash_burst_mirror();
   	 flash_device_id = 0;
	 //asm(" NOP");
         //asm(" NOP");
         //asm(" NOP");
         //asm(" NOP");
         //asm(" NOP");
         //asm(" NOP");
         //asm(" NOP");
        //asm(" NOP");
#elif(OP_L1_STANDALONE == 1 && MIRROR_BIT == 0 )
	 flash_device_id = 0x7E;
#endif


  #if GSM_IDLE_RAM_DEBUG
  #if (CHIPSET!=15)
        *((volatile SYS_UWORD16 *) 0xFFFE4806) = (0x0020);
        AI_ConfigBitAsOutput(3);
        AI_ConfigBitAsOutput(2);
  #endif
  #endif
  #if (CHIPSET==15)
  	{
  	volatile unsigned int * configReg=(volatile unsigned int *)0xFFFEF01C;
	*configReg &= 0xF7FF;
  	}
  #endif

}

//--> Init Added for Power Management
/*******************************************************
	Configure ALL I/O pins
*******************************************************/
void pin_configuration_all(void)
{
	pin_configuration_bluetooth();
	pin_configuration_emifs();
	pin_configuration_system();
	pin_configuration_lcd_nand(0);
	pin_configuration_keypad();
	pin_configuration_sim();
	pin_configuration_radio();
	pin_configuration_usb();
	pin_configuration_camera_irda(1);
}

/*******************************************************
	Configure Bluetooth I/O pins
*******************************************************/
void pin_configuration_bluetooth(void)
{
	GPIO_DIRECTION_OUT(37);	GPIO_CLEAR_OUTPUT(37);	// BT_nSHUTDOWN
	CONF_GPIO_43	 = MUX_CFG(1, PULLOFF);	// MCSI_CK
	CONF_GPIO_44	 = MUX_CFG(1, PULLOFF);	// MCSI_FS
	CONF_GPIO_45	 = MUX_CFG(1, PULLOFF);	// MCSI_TX
	CONF_GPIO_46	 = MUX_CFG(1, PULLOFF);	// MCSI_RX
	CONF_UART_CTS	 = MUX_CFG(0, PULLOFF);	// CTS
	CONF_UART_RX	 = MUX_CFG(0, PULLOFF);	// RX
	CONF_UART_TX	 = MUX_CFG(0, PULLOFF);	// TX
	CONF_GPIO_37	 = MUX_CFG(0, PULLOFF); // BT_SHUTDOWN
}

/*******************************************************
	Configure EMIFS I/O pins
*******************************************************/
void pin_configuration_emifs(void)
{
	CONF_ADD_21	 = MUX_CFG(0, PULLOFF);	// ADD21
	CONF_GPIO_39	 = MUX_CFG(1, PULLOFF); // ADD22
	CONF_GPIO_38	 = MUX_CFG(0, PULLOFF); // nCS0
	CONF_NCS3	 = MUX_CFG(0, PULLOFF);	// nCS3
	CONF_ADV	 = MUX_CFG(0, PULLOFF);	// ADV
	CONF_NMOE	 = MUX_CFG(0, PULLOFF);	// nMOE
	CONF_RNW	 = MUX_CFG(0, PULLOFF);	// RnW
	CONF_GPIO_42	 = MUX_CFG(1, PULLOFF); // CKM
	CONF_NRDY	 = MUX_CFG(0, PULLUP);	// nRDYMEM
}

/*******************************************************
	Configure system I/O pins
*******************************************************/
void pin_configuration_system(void)
{
	GPIO_DIRECTION_IN(1); // GPIO_1
	GPIO_DIRECTION_OUT(2);	GPIO_CLEAR_OUTPUT(2);	// SYS_RESET
	GPIO_DIRECTION_IN(10); // Not used (nEMU0)
	CONF_CK13MHZ_EN	 = MUX_CFG(0, PULLOFF);	// CK13MHZ_EN
	CONF_ABB_IRQ	 = MUX_CFG(0, PULLUP);	// ABB_IRQ
	CONF_GPIO_5	 = MUX_CFG(1, PULLOFF); // STARTADC
	CONF_CDO	 = MUX_CFG(0, PULLOFF);	// CDO (I2S)
	CONF_CSCLK	 = MUX_CFG(0, PULLOFF);	// CSCLK (I2S)
	CONF_CSYNC	 = MUX_CFG(0, PULLOFF);	// CSYNC (I2S)
	CONF_NBSCAN	 = MUX_CFG(0, PULLUP);	// nBSCAN
	CONF_SPARE_3	 = MUX_CFG(0, PULLUP); // Spare3
	CONF_TDO	 = MUX_CFG(0, PULLOFF);	// TDO

// JTAG pulls are disabled on I-Sample due to external buffers.
//	CONF_TCK	 = MUX_CFG(0, PULLDOWN);	// TCK
//	CONF_TDI	 = MUX_CFG(0, PULLUP);	// TDI
//	CONF_TMS	 = MUX_CFG(0, PULLUP);	// TMS
//	CONF_TRST	 = MUX_CFG(0, PULLUP);	// TRST
	CONF_TCK	 = MUX_CFG(0, PULLOFF);	// TCK
	CONF_TDI	 = MUX_CFG(0, PULLOFF);	// TDI
	CONF_TMS	 = MUX_CFG(0, PULLOFF);	// TMS
	CONF_TRST	 = MUX_CFG(0, PULLOFF);	// TRST

	CONF_VDR	 = MUX_CFG(0, PULLOFF);	// VDR
	CONF_VFSRX	 = MUX_CFG(0, PULLDOWN);	// VFSRX
	CONF_GPIO_1	 = MUX_CFG(0, PULLUP); // Not used GPIO
	CONF_GPIO_2	 = MUX_CFG(0, PULLOFF); // SYS_RESET
	CONF_GPIO_10	 = MUX_CFG(1, PULLUP); // Not used (NEMU0)
	CONF_GPIO_12	 = MUX_CFG(2, PULLOFF); // TSPACT10
}

/*******************************************************
	Configure LCD and NAND Flash I/O pins
   Mode = 0 : LCD functional. NAND not functional
   Mode = 1 : NAND functional. LCD not functional
*******************************************************/
void pin_configuration_lcd_nand(int mode)
{
	CONF_ND_CE1	 = MUX_CFG(0, PULLOFF);
	CONF_GPIO_18	 = MUX_CFG(1, PULLOFF); // ND_WE
	CONF_GPIO_31	 = MUX_CFG(1, PULLOFF); // ND_RE
	CONF_GPIO_32	 = MUX_CFG(1, PULLOFF); // ND_CLE
	CONF_GPIO_33	 = MUX_CFG(1, PULLOFF); // ND_ALE
	CONF_GPIO_34	 = MUX_CFG(1, PULLUP); // ND_RDY
	CONF_LCD_NRST	 = MUX_CFG(0, PULLOFF);
	CONF_LCD_RNW	 = MUX_CFG(0, PULLOFF);
	CONF_LCD_RS	 = MUX_CFG(0, PULLOFF);
	CONF_LCD_STB	 = MUX_CFG(0, PULLOFF);
	CONF_GPIO_13	 = MUX_CFG(1, PULLOFF); // LCD_NCS0
	switch(mode)
		{
			case 0: // LCD
				CONF_LCD_DATA_0	 = MUX_CFG(0, PULLOFF);
				CONF_LCD_DATA_1	 = MUX_CFG(0, PULLOFF);
				CONF_LCD_DATA_2	 = MUX_CFG(0, PULLOFF);
				CONF_LCD_DATA_3	 = MUX_CFG(0, PULLOFF);
				CONF_LCD_DATA_4	 = MUX_CFG(0, PULLOFF);
				CONF_LCD_DATA_5	 = MUX_CFG(0, PULLOFF);
				CONF_LCD_DATA_6	 = MUX_CFG(0, PULLOFF);
				CONF_LCD_DATA_7	 = MUX_CFG(0, PULLOFF);
			break;
			case 1: // NAND
				CONF_LCD_DATA_0	 = MUX_CFG(3, PULLOFF);
				CONF_LCD_DATA_1	 = MUX_CFG(3, PULLOFF);
				CONF_LCD_DATA_2	 = MUX_CFG(3, PULLOFF);
				CONF_LCD_DATA_3	 = MUX_CFG(3, PULLOFF);
				CONF_LCD_DATA_4	 = MUX_CFG(3, PULLOFF);
				CONF_LCD_DATA_5	 = MUX_CFG(3, PULLOFF);
				CONF_LCD_DATA_6	 = MUX_CFG(3, PULLOFF);
				CONF_LCD_DATA_7	 = MUX_CFG(3, PULLOFF);
			break;
		}
}

/*******************************************************
	Configure keypad pins
*******************************************************/
void pin_configuration_keypad(void)
{
	CONF_GPIO_8	 = MUX_CFG(0, PULLUP); // KBR4
	CONF_GPIO_9	 = MUX_CFG(0, PULLOFF); // KBC4
	CONF_KBC_0	 = MUX_CFG(0, PULLOFF);
	CONF_KBC_1	 = MUX_CFG(0, PULLOFF);
	CONF_KBC_2	 = MUX_CFG(0, PULLOFF);
	CONF_KBC_3	 = MUX_CFG(0, PULLOFF);
	CONF_KBR_0	 = MUX_CFG(0, PULLUP);
	CONF_KBR_1	 = MUX_CFG(0, PULLUP);
	CONF_KBR_2	 = MUX_CFG(0, PULLUP);
	CONF_KBR_3	 = MUX_CFG(0, PULLUP);
}

/*******************************************************
	Configure SIM I/O pins
*******************************************************/
void pin_configuration_sim(void)
{
	CONF_SIM_CLK	 = MUX_CFG(0, PULLOFF);
	CONF_SIM_IO	 = MUX_CFG(0, PULLOFF);
	CONF_SIM_PWCTRL	 = MUX_CFG(0, PULLOFF);
	CONF_SIM_RST	 = MUX_CFG(0, PULLOFF);
}

/*******************************************************
	Configure radio I/O pins
*******************************************************/
void pin_configuration_radio(void)
{
	CONF_TSPACT_11	 = MUX_CFG(0, PULLOFF);
	CONF_TSPACT_12	 = MUX_CFG(0, PULLOFF);
	CONF_TSPACT_13	 = MUX_CFG(0, PULLOFF);
	CONF_TSPACT_14	 = MUX_CFG(0, PULLOFF);
	CONF_TSPACT_15	 = MUX_CFG(0, PULLOFF);
}

/*******************************************************
	Configure USB I/O pins
*******************************************************/
void pin_configuration_usb(void)
{
	CONF_USB_BOOT	 = MUX_CFG(0, PULLOFF);
	CONF_USB_DAT	 = MUX_CFG(0, PULLDOWN);
	CONF_USB_RCV	 = MUX_CFG(0, PULLDOWN);
	//CONF_USB_SE0	 = MUX_CFG(0, PULLUP);  // to reduce 130 uA
	CONF_USB_SE0	 = MUX_CFG(0, PULLOFF);  // to reduce 130 uA
	CONF_USB_TXEN	 = MUX_CFG(0, PULLOFF);
}

/*******************************************************
	Configure Camera and IrDA I/O pins
   Mode = 0 : IrDA functional. Camera not functional
   Mode = 1 : Camera functional. IrDA not functional
  Use mode 1 ONLY when camera is active i.e. use
  mode 0 during sleep
*******************************************************/
void pin_configuration_camera_irda(int mode)
{
	GPIO_DIRECTION_OUT(17);	GPIO_SET_OUTPUT(17);	// CAM_PWDN
	GPIO_DIRECTION_OUT(11);	GPIO_SET_OUTPUT(11);	// Golden eye sleep
	GPIO_DIRECTION_OUT(35);	GPIO_CLEAR_OUTPUT(35);	// LED_TRCH
	GPIO_DIRECTION_OUT(4);	GPIO_SET_OUTPUT(4);	// nCAM_RST
	GPIO_DIRECTION_OUT(20);	GPIO_CLEAR_OUTPUT(20); // CAM_D_3
	GPIO_DIRECTION_OUT(19);	GPIO_CLEAR_OUTPUT(19); // CAM_HS
	GPIO_DIRECTION_OUT(21);	GPIO_CLEAR_OUTPUT(21); // CAM_LCLK
	GPIO_DIRECTION_OUT(36);	GPIO_SET_OUTPUT(36);	// IrDA_SD
	CONF_GPIO_36	 = MUX_CFG(0, PULLOFF); // IRDA_SD
	CONF_GPIO_11	 = MUX_CFG(1, PULLOFF); // Golden eye sleep (NEMU1)
	CONF_GPIO_4	 = MUX_CFG(0, PULLOFF); // CAM_RESET
	CONF_GPIO_35	 = MUX_CFG(0, PULLOFF); // LED_TORCH
	CONF_GPIO_17	 = MUX_CFG(0, PULLOFF); // CAM_PWDN
	CONF_GPIO_23	 = MUX_CFG(1, PULLDOWN);	// SPI_CLK
	CONF_GPIO_24	 = MUX_CFG(1, PULLDOWN);	// SPI_DATA_MISO
	CONF_GPIO_25	 = MUX_CFG(1, PULLDOWN);	// SPI_DATA_MOSI
	CONF_GPIO_26	 = MUX_CFG(1, PULLUP);	// SPI_NCS0
	CONF_GPIO_27	 = MUX_CFG(1, PULLUP);	// SPI_NCS1
	CONF_GPIO_22	 = MUX_CFG(1, PULLOFF); // CAM_XCLK
	switch(mode)
		{
			case 0: // IrDA or sleep
				CONF_GPIO_47	 = MUX_CFG(1, PULLUP); // IrDA RXIR
				CONF_GPIO_0	 = MUX_CFG(1, PULLOFF); // IrDA TXIR
				CONF_GPIO_7	 = MUX_CFG(5, PULLDOWN); // CAM_D_2
				CONF_GPIO_20	 = MUX_CFG(0, PULLOFF); // CAM_D_3 = GPIO
				CONF_ND_NWP	 = MUX_CFG(0, PULLOFF); // CAM_D_4 = ND_NWP
				CONF_GPIO_30	 = MUX_CFG(3, PULLDOWN); // CAM_D_5
				CONF_GPIO_29	 = MUX_CFG(3, PULLDOWN); // CAM_D_6
				CONF_GPIO_28	 = MUX_CFG(3, PULLUP); // CAM_D_7
				CONF_GPIO_19	 = MUX_CFG(0, PULLOFF); // CAM_HS = GPIO
				CONF_GPIO_21	 = MUX_CFG(0, PULLOFF); // CAM_LCLK = GPIO
			break;
			case 1: // Camera
				CONF_GPIO_47	 = MUX_CFG(2, PULLOFF); // CAM_D_0 + IrDA
				CONF_GPIO_0	 = MUX_CFG(2, PULLOFF); // CAM_D_1 + IrDA
				CONF_GPIO_7	 = MUX_CFG(5, PULLOFF); // CAM_D_2
				CONF_GPIO_20	 = MUX_CFG(2, PULLOFF); // CAM_D_3
				CONF_ND_NWP	 = MUX_CFG(2, PULLOFF); // CAM_D_4
				CONF_GPIO_30	 = MUX_CFG(3, PULLOFF); // CAM_D_5
				CONF_GPIO_29	 = MUX_CFG(3, PULLOFF); // CAM_D_6
				CONF_GPIO_28	 = MUX_CFG(3, PULLOFF); // CAM_D_7
				CONF_GPIO_19	 = MUX_CFG(1, PULLOFF); // CAM_HS
				CONF_GPIO_21	 = MUX_CFG(1, PULLOFF); // CAM_LCLK
			break;
		}
}

//<-- Init Added for Power Management

/*
 * Init_Drivers
 *
 * Performs Drivers Initialization.
 */
void Set_Switch_ON_Cause(void);
void Init_Drivers(void)
{

#if (CHIPSET==15)
	bspI2c_init();
	bspTwl3029_init();

	#if (OP_L1_STANDALONE == 0)
		Set_Switch_ON_Cause();
	#endif


	/* Turn on DRP We will make VRMCC to device group Modem
	 * And Switch it on.
	 */
	bspTwl3029_Power_setDevGrp(NULL,BSP_TWL3029_POWER_VRMMC,BSP_TWL3029_POWER_DEV_GRP_MODEM);
	wait_ARM_cycles(convert_nanosec_to_cycles(100000*2));
    bspTwl3029_Power_enable(NULL,BSP_TWL3029_POWER_VRMMC,BSP_TWL3029_POWER_STATE_ACTIVE);
#endif

#if (CHIPSET!=15)
  #if ABB_SEMAPHORE_PROTECTION
    // Create the ABB semaphore
    ABB_Sem_Create();
  #endif  // SEMAPHORE_PROTECTION
#endif

  #if (OP_L1_STANDALONE == 0)
    /*
     * Initialize FFS invoking restore procedure by MPU-S
     */
    #if ((BOARD == 35) || (BOARD == 46))
      GC_FfsRestore();
    #endif

    /*
     * FFS main initialization.
     */

    ffs_main_init();


    /*
     * Initialize Riviera manager and create tasks thanks to it.
     */
#if (CHIPSET!=15) || (REMU==0)
    rvf_init();
    rvm_init();			/* A-M-E-M-D-E-D!	*/
    create_tasks();
#endif
    /*
     * SIM Main Initialization.
     */
    #if (CHIPSET!=15)
    	SIM_Initialize ();
    #else
	bspUicc_bootInit();
    #endif
  #endif
}

#if (PSP_STANDALONE == 0)
/*
 * l1_create_HISR
 *
 * Create L1 HISR.
 */
void l1_create_HISR (void)
{
  STATUS status;

  #if (OP_L1_STANDALONE == 0)
    // Fill the entire stack with the pattern 0xFE
    memset (layer_1_sync_stack, 0xFE, LAYER_1_SYNC_STACK_SIZE);
  #endif

  status = NU_Create_HISR (&layer_1_sync_HISR,
                           "L1_HISR",
                           layer_1_sync_HISR_entry,
  #if (OP_L1_STANDALONE == 0)
                           1,
                           layer_1_sync_stack,
                           LAYER_1_SYNC_STACK_SIZE);
  #else
                           1,
                           layer_1_sync_stack,
                           sizeof(layer_1_sync_stack));
  #endif

  #if (L1_EXT_AUDIO_MGT)
    // Create HISR for Ext MIDI activity
    //==================================
    status += NU_Create_HISR(&EXT_AUDIO_MGT_hisr,
                             "H_EXT_AUDIO_MGT",
                             Cust_ext_audio_mgt_hisr,
                             2,
                             ext_audio_mgt_hisr_stack,
                             sizeof(ext_audio_mgt_hisr_stack));
  #endif

  #if ( (L1_MP3 == 1) || (L1_MIDI == 1) || (L1_AAC == 1) || (L1_DYN_DSP_DWNLD == 1) )    // equivalent to an API_HISR flag
    status += NU_Create_HISR(&apiHISR,
                             "API_HISR",
                             api_hisr,
                             2,
                             API_HISR_stack,
                             sizeof(API_HISR_stack));
  #endif // (L1_MP3 == 1) || (L1_MIDI == 1) || (L1_AAC == 1) || (L1_DYN_DSP_DWNLD == 1)

  #if (FF_L1_IT_DSP_USF == 1) || (FF_L1_IT_DSP_DTX == 1) // equivalent to an API_MODEM_HISR flag
    // Create HISR for USF  DSP interrupt !!!!. This HISR needs
    // to have the highest priority since the USF status needs
    // to be known before the next block starts.
    //========================================================
    status += NU_Create_HISR(&api_modemHISR,
                             "MODEM",
                             api_modem_hisr,
                             1,
                             API_MODEM_HISR_stack,
                             sizeof(API_MODEM_HISR_stack));
  #endif

  assert (status == 0);
}


void l1_create_ISR (void)
{
  l1_create_HISR();
}
#endif

#endif /* (CODE_VERSION != SIMULATION) */



/*
 * Init_Unmask_IT
 *
 * Unmask all used interrupts.
 */

void Init_Unmask_IT (void)
{
  #if (CODE_VERSION != SIMULATION)
    /* Reset all current interrupts */
    #if (CHIPSET == 12) || (CHIPSET == 15)
      F_INTH2_VALID_NEXT(C_INTH_IRQ);   /* Reset current IT in INTH2 IRQ */
      F_INTH_VALID_NEXT(C_INTH_IRQ);   /* Reset current IT in INTH IRQ */
      F_INTH_VALID_NEXT(C_INTH_FIQ);   /* Reset current IT in INTH FIQ */
      F_INTH_RESET_ALL_IT;   /* Reset all IRQ/FIQ source */
    #elif ((CHIPSET == 4) || (CHIPSET == 5) || (CHIPSET == 6) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 9) || (CHIPSET == 10) || (CHIPSET == 11))
      INTH_RESETALLIT;
      INTH_CLEAR;                 /* reset IRQ/FIQ source */
    #endif

    // Disable all the IRQs of ARM before unmasking the interrupts. And enable them once the unmasking is
    // complete. This will ensure that no context switch happens due to coming of an already unmasked interrupt
    // before completing the unmasking of the rest of the interrupts. If the context switch happens in
    // before all the interrupts have been unmasked, then the control will never return to this function
    //since it will go to TCT_Scheduler() and so some interrupts may remain masked.
    INT_DisableIRQ();

    // SL: TEMPORARY FIX FOR BUG ON P2
    // FRAME INTERRUPT STAY MASKED
    #if ((BOARD == 35) || (BOARD == 46))
      INTH_ENABLEONEIT(IQ_FRAME);
    #endif
    // TEMPORARY FIX FOR BUG ON P2


    /* Enable Uart Modem and IrDA interrupts */
    #if ((((TRACE_TYPE==1) || (TRACE_TYPE==2) || (TRACE_TYPE==3) || \
          (TESTMODE) || (TRACE_TYPE==7)) && (OP_L1_STANDALONE == 1)) || \
         (OP_L1_STANDALONE == 0))

      #if (CHIPSET == 12) || (CHIPSET == 15)
        F_INTH_ENABLE_ONE_IT (C_INTH_UART_IRDA_IT);   /* Enable UART_IRDA interrupts */
        #if (CHIPSET == 12)
        	F_INTH_ENABLE_ONE_IT(C_INTH_UART_MODEM1_IT);   /* Enable UART_MODEM interrupts */
	#endif
      #else
      	#if ((CHIPSET == 4) || (CHIPSET == 5) || (CHIPSET == 6) || (CHIPSET == 7) || \
             (CHIPSET == 8) || (CHIPSET == 9) || (CHIPSET == 10) || (CHIPSET == 11))
          #if ((BOARD != 35) && (BOARD != 46))
            INTH_ENABLEONEIT (IQ_UART_IRDA_IT);   /* Enable UART_IRDA interrupts */
          #endif
        #endif
        	INTH_ENABLEONEIT (IQ_UART_IT);   /* Enable UART_MODEM interrupts */
      #endif

      #if (OP_L1_STANDALONE == 1)
        #if (CHIPSET == 12)
          F_INTH_ENABLE_ONE_IT (C_INTH_UART_MODEM2_IT);   /* Enable UART_MODEM2 interrupts */
        #endif
      #endif   /* (OP_L1_STANDALONE == 1) */

    #endif   /* (TRACE_TYPE ...) || (OP_L1_STANDALONE == 0) */

    /* Enable other interrupts */
    #if (OP_L1_STANDALONE == 1)
      if (l1_config.pwr_mngt == PWR_MNGT)
      {

        #if GEMINI_TIMER
          #if (CHIPSET == 12) || (CHIPSET == 15)
            F_INTH_ENABLE_ONE_IT(C_INTH_TIMER1_IT);   /* Enable Timer1 interrupt */
          #else
            INTH_ENABLEONEIT(IQ_TIM1);   /* Enable Timer1 interrupt */
          #endif
        #endif

        #if EXT_IRQ
          #if (CHIPSET == 12) || (CHIPSET == 15)
            F_INTH_ENABLE_ONE_IT (C_INTH_ABB_IRQ_IT);   /* Enable ABB_IRQ_IT EXT_IRQ */
          #else
            INTH_ENABLEONEIT(IQ_EXT);   /* Enable External interrupt */
          #endif
      #endif
      }

    #else
      #if ((CHIPSET == 12) || (CHIPSET == 15))
        F_INTH_ENABLE_ONE_IT(C_INTH_KEYBOARD_IT);
      #else
        #if ((BOARD == 35) || (BOARD == 46))
          INTH_ENABLEONEIT (IQ_ICR);
        #else
          INTH_ENABLEONEIT (IQ_ARMIO);   /* Enable Keypad/GPIO Interrupt */
        #endif
      #endif   /* (CHIPSET == 12) */
    #endif   /* OP_L1_STANDALONE */


      #if (CHIPSET == 12) || (CHIPSET == 15)
        //enable LEAD2 interrupt
        F_INTH_ENABLE_ONE_IT (C_INTH_API_IT);
      #else
        //enable LEAD2 interrupt
        INTH_ENABLEONEIT(IQ_API);
      #endif


    /* Enable TDMA interrupt */
    #if ((CHIPSET == 12) || (CHIPSET == 15))
	 #if (PSP_STANDALONE == 0)
      F_INTH_ENABLE_ONE_IT (C_INTH_FRAME_IT);
	 #endif
    #else
      INTH_ENABLEONEIT(IQ_FRAME);
    #endif
//-->  to enable DMA Interrupt for Lite
	#if ((CHIPSET == 15) )
		 #if (LOCOSTO_LITE == 1)
      			F_INTH_ENABLE_ONE_IT (C_INTH_DMA_IT);
	 	#endif
    	#endif
//<--  to enable DMA Interrupt for Lite

    #if ( (L1_MP3 == 1) || (L1_MIDI == 1) || (L1_AAC == 1) || (L1_DYN_DSP_DWNLD == 1) )
      #if (CHIPSET == 12) || (CHIPSET == 15)
        F_INTH_ENABLE_ONE_IT(C_INTH_API_IT);  // Enable API interrupt
      #elif (CHIPSET == 10)
        #if (L1_DYN_DSP_DWNLD == 1)

         //enable interrupt
         INTH_ENABLEONEIT (IQ_API);

        #endif // L1_DYN_DSP_DWNLD == 1
      #endif // CHIPSET
    #endif  // (L1_MP3 == 1) || (L1_MIDI == 1) || (L1_AAC ==1) || (L1_DYN_DSP_DWNLD == 1)
    // Enable the ARM IRQs once all the interrupts have been unmasked.
    INT_EnableIRQ();
  #endif   /* NO SIMULATION */
}


/*
 * Init_Serial_Flows
 *
 * Performs Serialswitch + related serial data flows initialization.
 */

void Init_Serial_Flows (void)
{
  #if (OP_L1_STANDALONE == 0)

    /*
     * Initialize Serial Switch module.
     */
    #if ((BOARD==35) || (BOARD == 46))
      SER_InitSerialConfig (GC_GetSerialConfig());
    #else
      SER_InitSerialConfig (&appli_ser_cfg_info);
    #endif
    /*
     * Then Initialize the Serial Data Flows and the associated UARTs:
     *  - G2-3 Trace if GSM/GPRS Protocol Stack
     *  - AT-Cmd/Fax & Data Flow
     *
     * Layer1/Riviera Trace Flow and Bluetooth HCI Flow are initialized
     * by the appropriate SW Entities.
     *
     * G2-3 Trace => No more Used
     */
    SER_tr_Init(SER_PROTOCOL_STACK, TR_BAUD_38400, NULL);

    /*
     * Fax & Data / AT-Command Interpreter Serial Data Flow Initialization
     */

    #if ((BOARD != 35) && (BOARD != 46))
      (void) SER_fd_Initialize ();
    #endif
  #else    /* OP_L1_STANDALONE */

    #if (TESTMODE || (TRACE_TYPE==1) || (TRACE_TYPE==2) || (TRACE_TYPE==3) || (TRACE_TYPE==6) || (TRACE_TYPE==7))
      #if ((BOARD == 35) || (BOARD == 46))
        ser_cfg_info[UA_UART_0] = '0';
      #else
        ser_cfg_info[UA_UART_0] = 'G';
      #endif
      #if (CHIPSET !=15)
      ser_cfg_info[UA_UART_1] = 'R'; // Riviear Demux on UART MODEM
      #else
      ser_cfg_info[UA_UART_0] = 'R'; // Riviear Demux on UART MODEM
      #endif

      /* init Uart Modem */
      SER_InitSerialConfig (&appli_ser_cfg_info);

      #if TESTMODE || (TRACE_TYPE == 1) || (TRACE_TYPE == 7)
        SER_tr_Init (SER_LAYER_1, TR_BAUD_115200, rvt_activate_RX_HISR);

        rvt_register_id("OTHER",&trace_id,(RVT_CALLBACK_FUNC)NULL);
      #else
        SER_tr_Init (SER_LAYER_1, TR_BAUD_38400, NULL);
      #endif

      L1_trace_string(" \n\r");

    #endif   /* TRACE_TYPE */

  #endif   /* OP_L1_STANDALONE */
}

// From this point, everything is compiled to execute in internal RAM

#if (((MOVE_IN_INTERNAL_RAM == 1) ^ (GSM_IDLE_RAM != 0)) && (GSM_IDLE_RAM != 0))
#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_START
#if (CODE_VERSION != SIMULATION)

  #if ((CHIPSET == 12) || (CHIPSET==15))

    #if GSM_IDLE_RAM_DEBUG
      void flash_access_handler(void)
      {
        l1s.gsm_idle_ram_ctl.killing_flash_access++;
      }
      void ext_ram_access_handler(void)
      {
        l1s.gsm_idle_ram_ctl.killing_ext_ram_access++;
      }
    #endif

    void ext_ram_irq_inth_handler(void)
    {
      if (!READ_TRAFFIC_CONT_STATE)
      CSMI_TrafficControllerOn();
      a_inth_config[F_INTH_GET_IRQ].d_it_handler();
    }

    void ext_ram_fiq_inth_handler(void)
    {
      if (!READ_TRAFFIC_CONT_STATE)
      CSMI_TrafficControllerOn();
      a_inth_config[F_INTH_GET_FIQ].d_it_handler();
    }
  #endif
#endif // (CODE_VERSION != SIMULATION)
#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END
#endif

#if !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM !=0))
#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_START

#if (PSP_STANDALONE == 0)
/*-------------------------------------------------------*/
/* TP_FrameIntHandler() Low Interrupt service routine    */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :  activate Hisr on each frame interrupt*/
/*-------------------------------------------------------*/
void TP_FrameIntHandler(void)
{

  #if (OP_L1_STANDALONE == 1)

    #if (TRACE_TYPE==1)
       if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_L1S_CPU_LOAD)
       {
         TM_ResetTimer (2, 0xFFFF, 1, 0);
         TM_StartTimer (2);
       }
    #endif

    #if (TRACE_TYPE==6)
       TM_ResetTimer (2, 0xFFFF, 1, 0);
       TM_StartTimer (2);
    #endif

    #if (TRACE_TYPE==7)   /* CPU_LOAD */
       l1_cpu_load_start();
    #endif

  #else

    #if (TRACE_TYPE == 4) && (TI_NUC_MONITOR != 1)
      // TM_ResetTimer (2, TIMER_RESET_VALUE, 1, 0);
      // TM_StartTimer (2);
    #endif


    #if (TI_NUC_MONITOR == 1)
       /* Copy LISR buffer in Log buffer each end of HISR */
       ti_nuc_monitor_tdma_action();
    #endif

    #if WCP_PROF == 1
       prf_LogFNSwitch(l1s.actual_time.fn_mod42432);
    #endif

  #endif   /* OP_L1_STANDALONE */

  NU_Activate_HISR(&layer_1_sync_HISR);   /* Activate HISR interrupt */

  #if (OP_L1_STANDALONE == 0)
    #if (WCP_PROF == 1)
      #if (PRF_CALIBRATION == 1)
      NU_Activate_HISR(&prf_CalibrationHISR);
      #endif
    #endif
  #endif

}
#endif

/*
 * layer_1_sync_HISR_entry
 *
 * HISR associated to layer 1 sync.
 */

void layer_1_sync_HISR_entry (void)
{
#if (PSP_STANDALONE==0)
   // Call Synchronous Layer1
   hisr();
#endif
}
#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END
#endif // !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM !=0))

#if (PSP_STANDALONE == 1)

#include "nucleus.h"
#include "tc_defs.h"

extern TC_PROTECT TCD_System_Protect;

/*-------------------------------------------------------*/
 /* int OS_system_protect()                               */
 /*-------------------------------------------------------*/
 /* Parameters : none                                     */
 /* Return     : The Thread Control Block of the thread   */
 /*              which already owns the protection or     */
 /*              0 if no protection                       */
 /* Functionality : Checks whether the system structures  */
 /*                 are already protected or not          */
 /*-------------------------------------------------------*/
 void OS_system_protect (void)
 {
   NU_Protect((NU_PROTECT*) &TCD_System_Protect);
 }

 /*-------------------------------------------------------*/
 /* int OS_system_Unprotect()                             */
 /*-------------------------------------------------------*/
 /* Parameters : none                                     */
 /* Return     :                                          */
 /* Functionality : unprotect the system structures       */
 /*-------------------------------------------------------*/
 void OS_system_Unprotect (void)
 {
   NU_Unprotect();
 }

void bspDummy_detect(void* a,int b, void* c)
{
	return;
}

void bspDummy_remove(int a)
{
	return;
}

void Init_Watchdog_Timer(void)
{
/* This code is for PSP STANDALONE Build Only */
/* WatchDog will be used by DAR enity, so using TIMER1 for OS Ticks
   This will give tick period roughly equle to 4.5 ms which approx
   Frame Interrupt timing */
   Dtimer1_Init_cntl(0XE9, 1, 0x07, 1);
   Dtimer1_Start(1);
   F_INTH_ENABLE_ONE_IT(C_INTH_TIMER1_IT);
   bspUicc_drvRegister((BspUicc_CardPresentDetectHandler)bspDummy_detect,
   	(BspUicc_CardAbsentDetectHandler) bspDummy_remove);
}

#endif


#endif//  #if (OP_L1_STANDALONE == 1)




