 /************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * DYNAMIC_CLOCK_C
 *
 *        Filename dynamic_clock.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

/*
 * Only SAMSON/CALYPSO families are considered for dynamic clock configuration.
 */

#include "chipset.cfg"
#include "board.cfg"

  #include "nucleus.h"

  /* Include file for DSP latency definition */
  #include "l1_macro.h"
  #include "l1_confg.h"
  #include "l1_types.h"
  #include "sys_types.h"
  #include "l1_const.h"


  /* Include file for l1_config variable */
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

  #include "l1_defty.h"
  #include "cust_os.h"
  #include "l1_msgty.h"
  #include "l1_varex.h"
  #include "l1_proto.h"


  /* Include file for H/W configuration impacted by clock update */
  #include "dynamic_clock.h"
  #include "clkm.h"
  #include "mem.h"
  #include "rhea_arm.h"
  #include "ulpd.h"

  #define _DYNAMIC_CLOCK_C_

    #include "dynamic_clock_15.h"


  extern void INT_DisableIRQ(void);
  extern void INT_EnableIRQ(void);
  
  T_DYNAMIC_CLOCK_CFG *p_dynamic_clock_cfg;
  SYS_UWORD32   d_dsp_cur_clk;     // Used to keep track of current DSP clock.
  INT old_int_mask;


  /***********************************************************************************************
   *          Only SAMSON/CALYPSO families are considered for dynamic clock configuration.
   ***********************************************************************************************
   *
   *                         Supported clock configuration                           
   *
   *
   *       CHIPSET                78/78/13    78/78/39   104/104/52    156/78/52   130/130/65
   *                                 (0)         (1)          (2)         (3)         (4)
   *
   *  HERCULES              (2)      NA           NA          NA          NA          NA
   *  ULYSSE/ULYSSE G0      (3)      NA           NA          NA          NA          NA
   *  ULYSSE C035           (9)      NA           NA          NA          NA          NA
   *  ULYSSE G1 (13MHz)     (5)      NA           NA          NA          NA          NA
   *  ULYSSE G1 (26MHz)     (6)      NA           NA          NA          NA          NA
   *
   *  SAMSON C07            (4)      X            ?           NA          NA          NA
   *  CALYPSO C05 Rev A     (7)      X            X           NA          NA          NA
   *  CALYPSO C05 Rev B     (8)      X            X           NA          NA          NA
   *  CALYPSO C035          (10)     NA           X           X           NA          NA
   *  CALYPSO LITE C035     (11)     NA           X           X           NA          NA
   *  CALYPSO PLUS c035     (12)     NA           X           X           X           X
   *
   *
   *
   *
   *           BOARD                CHIPSET                         Access Time (ns)
   *                                                     CS0    CS1    CS2    CS3    CS4    CS5
   *  
   *    EVA4 (RAM)        (6)     CALYPSO PLUS           100    NA     NA     NA     100    100
   *                              CALYPSO C05/C035                                          NA
   *                              SAMSON                                                    NA
   *    C-Sample (RAM)    (8)     CALYPSO C05/SAMSON     100    100    NA                   NA    (RD108, RD308) FLASH : AM29DL322T-70 (AMD) - SRAM : IC806 ????
   *    C-Sample (FLASH)  (9)     CALYPSO C05/SAMSON     100    100    NA                   NA    (RD108, RD308) FLASH : AM29DL322T-70 (AMD) - SRAM : IC806 ????
   *    D-Sample (RAM)    (40)    CALYPSO C035/SAMSON    85     70     70                   NA    (RD316, RD112) FLASH/SRAM : 28F640W30B70 (Intel) - SRAM : K1S321615M-EE10 (Samsung)
   *                              CALYPSO C05             
   *    D-Sample (FLASH)  (41)    CALYPSO C035/SAMSON    70     70     85                   NA    (RD316, RD112) FLASH/SRAM : 28F640W30B70 (Intel) - SRAM : K1S321615M-EE10 (Samsung)
   *                              CALYPSO C05
   *    E-Sample (SRAM)   (42)    CALYPSO PLUS           NA     NA     NA      NA    70     70
   *    E-Sample (FLASH)  (43)    CALYPSO PLUS           NA     NA     NA      NA    70     70
   *
   *
   ***********************************************************************************************/
  /*
   *  Before to call the function, the application must:
   *    - Check that DSP is in IDLE3 before to modify PARAM
   */
 
  /*---------------------------------------------------------*/
  /* f_dynamic_clock_cfg()                                   */
  /*---------------------------------------------------------*/
  /* Description : This function realizes the dynamic clock  */
  /*               configuration changing DPLL, ARM and DSP  */
  /*               clocks for Samson and Calypso families    */
  /*                                                         */
  /* Input:        d_clock_cfg => num of selected clock      */
  /*                              configuration              */
  /*                                                         */
  /*---------------------------------------------------------*/ 
  f_dynamic_clock_cfg(SYS_UWORD8 d_clock_cfg) {
    
    /* 
     * Local variable used to check the selected clock configuration exist 
     * and is inside the array of clock configurations.
     * It must be initialized to 0.
     */
    SYS_UWORD8 d_array_index=0;
    static SYS_UWORD8 d_first_call = 1;
    
    T_PARAM_MCU_DSP *p_mcu_dsp_param = (T_PARAM_MCU_DSP *) PARAM_ADR;

	T_EMIF_CONF 	emif_conf = {
					C_EMIF_PREFETCH_INSTRUCTION,
					C_EMIF_PDE_ENABLE,

					C_EMIF_PWD_ENABLE,
					0,
					0};


    /*
     * FOR DEBUG ONLY : use of GPIO_0 to check change of clock
     * depending on DSP or/and ARM clock activity
     */
    p_dynamic_clock_cfg = (T_DYNAMIC_CLOCK_CFG *)a_dynamic_clock_cfg[d_array_index];
    while ((p_dynamic_clock_cfg != NULL) && (d_array_index < C_NB_MAX_CLOCK_CONFIG))
    {
      if (p_dynamic_clock_cfg->d_clock_cfg_index == d_clock_cfg)
        break;
      p_dynamic_clock_cfg = (T_DYNAMIC_CLOCK_CFG *)a_dynamic_clock_cfg[++d_array_index];
    }

    /*
     *  Check that the configuration is valid
     *  if not valid, current clock configuration stays unchanged
     */
    if ((p_dynamic_clock_cfg != NULL) && (d_array_index < C_NB_MAX_CLOCK_CONFIG))
    {

      /*
       *  Save and mask all interrupts configuration
       */
      /* Fix for SRS - Startup Restart Syndrome */

      if( d_first_call != 1)
      	old_int_mask = NU_Control_Interrupts( NU_DISABLE_INTERRUPTS );


      #if (CODE_VERSION != SIMULATION)

        /*
         *  Securise access to DPLL registers for SAMSON chip
         */
          /*
         *  Configure RHEA Access Factor for Strobe 0 and Strobe 1.
         *  Configure also RHEA timeout.
         */ 
          RHEA_INITRHEA(p_dynamic_clock_cfg->d_rhea_af_strobe0, \
                        p_dynamic_clock_cfg->d_rhea_af_strobe1, \
                        p_dynamic_clock_cfg->d_rhea_timeout);

       /*
        *  Force the DPLL is Bypass mode
        */
        DPLL_RESET_PLL_ENABLE;

       /*
        *  Wait that DPLL enters in Bypass mode
        */
        while(DPLL_READ_DPLL_LOCK == DPLL_LOCK) {}


       /*
        *  Configure DPLL BYPASS mode
        */
        DPLL_INIT_BYPASS_MODE(p_dynamic_clock_cfg->d_bypass_div);

        /*
         *  Configure DPLL clock frequency
         */
        DPLL_INIT_DPLL_CLOCK(p_dynamic_clock_cfg->d_dpll_div, p_dynamic_clock_cfg->d_dpll_mult);

        /*
         *  Configure ARM clock source and Frequency
         */
          	CLKM_InitARMClock(p_dynamic_clock_cfg->d_arm_clk_source,
                          p_dynamic_clock_cfg->d_arm_clk_mode);
        /*
         *  Configure ARM External Memory (wait-state, Dummy-cycle,...)
         */
	    	f_emif_set_priority(C_EMIF_DEFAULT_DMA_ACCESS, C_EMIF_DEFAULT_MCU_ACCESS);
        /*
         *  Configure API wait-state for HOM and SAM mode
         *
         * 1 WS in SAM mode when DSP = 78MHz and MCU=39MHz
         *                    or DSP =104Mhz and MCU=52Mhz
         * Always 0 WS in HOM mode for MCU frequency up to and
         * including 39 MHz (52Mhz for c035)
         * 1 WS in HOM mode to workaround HW bug 3504
         * dma_arbtrs > Concurrent access to API from DMA and MCU
		 */
        RHEA_INITAPI(p_dynamic_clock_cfg->d_api_ws_hom, p_dynamic_clock_cfg->d_api_ws_sam);


        /*
         *  Force the DPLL in Locked mode
         */
        DPLL_SET_PLL_ENABLE;

        /*
         *  Wait that DPLL enters in Locked mode
         */
        while(DPLL_READ_DPLL_LOCK != DPLL_LOCK) {}

        /*
         *  Configure RHEA Access Factor for Strobe 0 and Strobe 1.
         *  Configure also RHEA timeout.
         *  For SAMSON only, for Calypso, it's been configure before DPLL bypass.
         */ 
      #endif /* CODE_VERSION != SIMULATION */

#if (PSP_STANDALONE == 0)
      /*
       *  Re-initialize gauging processing: this part of code is done in l1_initialize()
       *  function during L1 init.
       */
      l1_dpll_init_var();

      /*
       * Power management variables
       */
      l1_pwr_mgt_init();
#endif
      /*
       *  Re-initialize ARM wait loop: during init phase, this function is called in
       *  l1_initialize() function
       */
      initialize_wait_loop();

      /*
       *  Restore all interrupts
       */
      /* Fix for SRS - Startup Restart Syndrome */
      if( d_first_call != 1)
      	NU_Control_Interrupts( old_int_mask );
      else
	d_first_call = 2;

    }  /* ((p_dynamic_clock_cfg != NULL) && (d_tab_index < C_NB_CLOCK_CONFIG)) */
    
  } /* f_dynamic_clock_cfg() */




