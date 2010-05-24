 /************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * DYNAMIC_CLOCK_H
 *
 *        Filename %M%
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

#ifndef _DYNAMIC_CLOCK_H_
  #define _DYNAMIC_CLOCK_H_

  #include "chipset.cfg"
  #include "board.cfg"

  /*
   * Only SAMSON/CALYPSO families are considered for dynamic clock configuration.
   */

  #if (CHIPSET == 4) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || \
      (CHIPSET == 11) || (CHIPSET == 12) || (CHIPSET == 15)


    #include "sys_types.h"
    #include "mem.h"
    #if (CHIPSET == 12) || (CHIPSET == 15)
      #include "sys_memif.h"
    #endif

    /* Maximal number of clock configurations */
    #define C_NB_MAX_CLOCK_CONFIG         10

    /* Value of index if no dynamic clock configuration selected */
    #define C_CLOCK_NO_CFG            0xFF
 
    /* Define the indexes of the available clock configurations */
    /* Indexes are defined as : C_CLOCK_CFG_DPLL_DSP_ARM */
    #if (CHIPSET == 15)
    //For Locosto: DSP is 104 MHz and ARM could be 104/52 MHz
    #define C_CLOCK_CFG_104_104_104   0
    #define C_CLOCK_CFG_104_104_52      1
    #else	
    #define C_CLOCK_CFG_78_78_13      0
    #define C_CLOCK_CFG_78_78_39      1
    #define C_CLOCK_CFG_104_104_52    2
    #define C_CLOCK_CFG_156_78_52     3
    #define C_CLOCK_CFG_130_130_65    4
    #endif

    /* Default clock configuration used at initialization */
    #if (CHIPSET == 4)
      /* Samson C07 family */
      #define C_CLOCK_CFG_DEFAULT       C_CLOCK_CFG_78_78_13
      
    #elif (CHIPSET == 7) || (CHIPSET == 8)
      /* Calypso C05 family */
      #define C_CLOCK_CFG_DEFAULT       C_CLOCK_CFG_78_78_39
      
    #elif (CHIPSET == 10) || (CHIPSET == 11) || (CHIPSET == 12)
      /* Calypso C035 family */
      #define C_CLOCK_CFG_DEFAULT       C_CLOCK_CFG_104_104_52
      
    #elif (CHIPSET == 15)
      /* Locosto default configuration is DPLL =104 MHz ARM=104MHz DSP = 104MHz*/
      #define C_CLOCK_CFG_DEFAULT	C_CLOCK_CFG_104_104_104
    #endif

    /* Structure used to define clock configurations               */
    /* Due to chipset and board dependencies, clock configurations */ 
    /* are pre-defined in a table of structures                    */
    typedef struct {
      /* Index of the present clock configuration */
      SYS_UWORD8 d_clock_cfg_index;
      
      /* DSP clock in kHz used in conjunction of d_dsp_cur_clk */
      SYS_UWORD32 d_dsp_clk;
      
      /* DPLL configuration */
      SYS_UWORD16 d_bypass_div;
      SYS_UWORD16 d_dpll_div;
      SYS_UWORD16 d_dpll_mult;

      /* ARM clock configuration */
      SYS_UWORD16 d_arm_clk_source;
     #if (CHIPSET == 15)
        SYS_UWORD16 d_arm_clk_mode;
     #else
       SYS_UWORD16 d_arm_clk_div;
       SYS_UWORD16 d_arm_clk_div_sel;
      #endif
     

      #if CHIPSET == 12
        /* DSP configuration */
        SYS_UWORD16 d_dsp_div_clk;
      #endif

      /* DSP latencies configuration */
      API_SIGNED d_lat_mcu_hom2sam;
      API_SIGNED d_lat_mcu_bridge;
      API_SIGNED d_lat_mcu_bef_fast_access;
      API_SIGNED d_lat_dsp_after_sam;
      API_SIGNED d_transfer_rate;

      /* API-RHEA configuration */
      SYS_UWORD16 d_api_ws_hom;
      SYS_UWORD16 d_api_ws_sam;
      SYS_UWORD16 d_rhea_af_strobe0;
      SYS_UWORD16 d_rhea_af_strobe1;
      SYS_UWORD16 d_rhea_timeout;

      /* EMIF configuration */
      #if (CHIPSET == 4) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 11)
        T_MEMIF_CS_CONFIG d_cs0;
        T_MEMIF_CS_CONFIG d_cs1;
        T_MEMIF_CS_CONFIG d_cs2;
        T_MEMIF_CS_CONFIG d_cs3;
      #elif (CHIPSET == 12)
        T_MEMIF_CS_CONFIG d_cs0;
      #endif
      
      #if ( CHIPSET != 15 )
        T_MEMIF_CS_CONFIG d_cs4;
      #else
	/* Different configuration structure for Locosto */
	T_EMIF_CS_CONFIG d_cs0;
	T_EMIF_CS_CONFIG d_cs3;
      #endif

      #if (CHIPSET == 12)
        T_MEMIF_CS_CONFIG d_cs5;
      #endif

      #if (CHIPSET == 4) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 11)
        T_MEMIF_CS_CONFIG d_cs6;
        T_MEMIF_CS_CONFIG d_cs7;
      #endif


    } T_DYNAMIC_CLOCK_CFG;

    extern T_DYNAMIC_CLOCK_CFG *p_dynamic_clock_cfg;
    extern SYS_UWORD32  d_dsp_cur_clk;   // To store current DSP clock due to DSP latency management

    extern f_dynamic_clock_cfg(SYS_UWORD8 d_clock_cfg);


  #endif /* SAMSON/CALYPSO families */

#endif /* _DYNAMIC_CLOCK_H_ */
