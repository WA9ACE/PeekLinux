/******************************************************************************
            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION

   Property of Texas Instruments -- For  Unrestricted  Internal  Use  Only
   Unauthorized reproduction and/or distribution is strictly prohibited.  This
   product  is  protected  under  copyright  law  and  trade  secret law as an
   unpublished work.  Created 1987, (C) Copyright 1997 Texas Instruments.  All
   rights reserved.


   Filename         : clkm.c

   Description      : Set of functions useful to test the Saturn
                      CLKM peripheral

   Project          : drivers

   Author           : pmonteil@tif.ti.com  Patrice Monteil.

   Version number   : 1.13

   Date             : 05/23/03

   Previous delta   : 10/23/01 14:43:31

   Sccs Id  (SID)       : '@(#) clkm.c 1.11 10/23/01 14:43:31 '

*****************************************************************************/

           //############################################################
           //############################################################
           //### Be careful: this file must be placed in Flash Memory ###
           //###     and compiled in 16 bits length intructions       ###
           //###        (CF. the function wait_ARM_cycles()           ###
           //############################################################
           //############################################################

#include "chipset.cfg"
#include "board.cfg"
#include "swconfig.cfg"

#include "sys_types.h"

#include "clkm/clkm.h"

#if ((CHIPSET == 12) || (CHIPSET == 15))
    #include "memif/sys_memif.h"
#else
    #include "memif/mem.h"
#endif

#include "timer/timer.h"
#include "armio/Armio.h"

static SYS_UWORD32 ratio_wait_loop = 0;

#if (CHIPSET == 12) 
  const double dsp_div_value[CLKM_NB_DSP_DIV_VALUE] = {1, 1.5, 2, 3};
#endif

#if ((CHIPSET == 4) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 11) || (CHIPSET == 12) )
  /*--------------------------------------------------------------*/
  /*  CLKM_InitARMClock()                                         */
  /*--------------------------------------------------------------*/
  /* Parameters :  clk_src : 0x00 means DPLL selected             */
  /*                         0x01 means VTCX0 selected            */
  /*                         0x03 means CLKIN selected            */
  /*               clk_xp5 : Enable 1.5 or 2.5 division factor    */
  /*                         (0 or 1)                             */
  /*               clk_div : Division factor applied to clock     */
  /*                         source                               */
  /*             WARNING : reverse order in comparison to ULYSSE  */
  /*                                                              */
  /* Return     : none                                            */
  /* Functionality :Initialize the ARM Clock frequency            */
  /*--------------------------------------------------------------*/
  
  
/* NEW COMPILER MANAGEMENT
 * Removal of inline on CLKM_InitARMClock.
 * With new compiler, inline means static inline involving the
 * function to not be seen outside this file.
 */
  void CLKM_InitARMClock(SYS_UWORD16 clk_src, SYS_UWORD16 clk_div, SYS_UWORD16 clk_xp5)
  {
    SYS_UWORD16 cntl = * (volatile SYS_UWORD16 *) CLKM_ARM_CLK;
        
    cntl &= ~(CLKM_CLKIN0 | CLKM_CLKIN_SEL | CLKM_ARM_MCLK_XP5 | CLKM_MCLK_DIV);
    
    cntl |= ((clk_src << 1) | (clk_xp5 << 3) | (clk_div << 4));
    
    * (volatile SYS_UWORD16 *) CLKM_ARM_CLK = cntl;
  }
#elif (CHIPSET == 15)
/*--------------------------------------------------------------*/
  /*  CLKM_InitARMClock()                                         */
  /*--------------------------------------------------------------*/
  /* Parameters :  clk_src : 0x00 means DPLL selected             */
  /*                         0x01 means VTCX0 selected            */
  /*                         0x03 means CLKIN selected            */
  /* 			   clk_mode : 0x00 means 102 MHZ				  */
  /* 						  0x01 means 52 MHZ 				  */
  /*                                                              */
  /* Return     : none                                            */
  /* Functionality :Initialize the ARM Clock frequency            */
  /*--------------------------------------------------------------*/
  
  
/* NEW COMPILER MANAGEMENT
 * Removal of inline on CLKM_InitARMClock.
 * With new compiler, inline means static inline involving the
 * function to not be seen outside this file.
 */
  void CLKM_InitARMClock(SYS_UWORD16 clk_src, SYS_UWORD8 clk_mode)
  {
    SYS_UWORD16 cntl = * (volatile SYS_UWORD16 *) CLKM_ARM_CLK;
        
    cntl &= ~(CLKM_CLKIN0 | CLKM_CLKIN_SEL);
    cntl |= (clk_src << 1);
	cntl &= ~(CLKM_SWITCH_FREQ);
	cntl |= (clk_mode << 13);
        
    * (volatile SYS_UWORD16 *) CLKM_ARM_CLK = cntl;
  }
#else 
  /*--------------------------------------------------------------*/
  /*  CLKM_InitARMClock()                                         */
  /*--------------------------------------------------------------*/
  /* Parameters :  clk_src : 0x00 means CLKIN selected            */
  /*                         0x01 means 32 K selected             */
  /*                         0x02 means External clock selected   */
  /*                                                              */
  /* Return     :  none                                           */
  /* Functionality :Initialize the ARM Clock frequency            */
  /*--------------------------------------------------------------*/


/* NEW COMPILER MANAGEMENT
 * Removal of inline on CLKM_InitARMClock.
 * With new compiler, inline means static inline involving the
 * function to not be seen outside this file.
 */
  void CLKM_InitARMClock(SYS_UWORD16 clk_src, SYS_UWORD16 clk_div)
  {
    SYS_UWORD16 cntl = * (volatile SYS_UWORD16 *) CLKM_ARM_CLK;

    cntl &= ~(CLKM_LOW_FRQ | CLKM_CLKIN_SEL | CLKM_MCLK_DIV);

    cntl |= ((clk_src << 1) | (clk_div << 4));

    * (volatile SYS_UWORD16 *) CLKM_ARM_CLK = cntl;
  }

#endif

#if ((BOARD == 35) || (BOARD == 46))
/*
 * CLKM_EnableSharedMemClock
 *
 * Enable or disable shared mem clock 
 *
 * Parameter : 1 or 0
 *
 */
void CLKM_EnableSharedMemClock(int enable)
{
  if (enable) 
  {
    // request shared mem clock and wait for MPU HW acknowledge
    AI_ResetBit(4);
    while(AI_ReadBit(5)!=1); 
  }
  else 
  {
    // disable shared mem clock
    AI_SetBit(4);
  }
}

/*
 * CLKM_SetDSPclkDiv
 * 
 * Parameter : onoff, div 
 *  
 * onoff=1 -> DSP clk = DPLL / div
 * onoff=0 -> DSP clk = DPLL
 */
void CLKM_SetDSPclkDiv(int onoff, int div)
{
  SYS_UWORD16 clk_div = 0;
  
  if (onoff)
  {
    if (div == 1.5)
      clk_div = 0x01;
    else if (div == 2) 
      clk_div = 0x10;
    else if (div == 3)
      clk_div = 0x11;
    else
      clk_div = 0x00;

    *((volatile SYS_UWORD16 *) CLKM_CNTL_CLK_DSP) = clk_div;
  }
  else
    *((volatile SYS_UWORD16 *) CLKM_CNTL_CLK_DSP) = 0;
}

/*
 * convert_nanosec_to_cycles()
 *
 * parameter: time in 10E-9 seconds
 * return: Number of cycles for the wait_ARM_cycles() function
 *
 * Description:
 * ------------
 * convert x nanoseconds in y cycles used by the ASM loop
 * function . Before calling this function, call the
 * initialize_wait_loop() function
 * Called when the HardWare needs time to wait
 */
inline SYS_UWORD32 convert_nanosec_to_cycles(SYS_UWORD32 time)
{ 
  return( time / ratio_wait_loop);
}

/*
 * initialize_wait_loop()
 *
 * Description:
 * ------------
 * Calibration of the ratio_wait_loop used by wait_ARM_Cycle
 */
void initialize_wait_loop(void)
{
  unsigned long ulTimeSpent=0;

  // set up timer 2 for wait_ARM_cycles function calibration
  TM_EnableTimer (2);
  TM_ResetTimer (2, 0xFFFF, 0, 0);

  // run wait_ARM_cycles() for 10000 loops
  wait_ARM_cycles(10000);

  // time spent expressed in timer cycles
  // where 1 timer cycle = 2462 ns with prescale 0
  // 13 MHz divided by 16 = timer clkin 
  // prescale 0 -> divided by 2
  ulTimeSpent = TM_ReadTimer (2);

  TM_StopTimer (2);

  ulTimeSpent = 0xFFFF - ulTimeSpent;
  ulTimeSpent *= 2462;

  // compute ratio_wait_loop
  ratio_wait_loop = (unsigned long)(ulTimeSpent/10000.);
}
#else 
/* FIXME : Confirm this for BOARD == 70 and BOARD == 71 */
/*-------------------------------------------------------*/
/* convert_nanosec_to_cycles()                           */
/*-------------------------------------------------------*/
/* parameter: time in 10E-9 seconds                      */
/* return: Number of cycles for the wait_ARM_cycles()    */
/*         function                                      */
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* convert x nanoseconds in y cycles used by the ASM loop*/
/* function . Before calling this function, call the     */
/* initialize_wait_loop() function                       */
/* Called when the HardWare needs time to wait           */
/*-------------------------------------------------------*/
/* NEW COMPILER MANAGEMENT
 * Removal of inline on convert_nanosec_to_cycles.
 * With new compiler, inline means static inline involving the
 * function to not be seen outside this file.
 */
SYS_UWORD32 convert_nanosec_to_cycles(SYS_UWORD32 time)
{ 
  return( time / ratio_wait_loop);  
}


/*-------------------------------------------------------*/
/* initialize_wait_loop()                                */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* Init the ratio used to convert time->Cycles according */
/* to hardware parameters                                */
/* measurement time for this function (ARM 39Mhz, 3 waits*/
/* states) = 75 micoseconds                              */
/*-------------------------------------------------------*/

void initialize_wait_loop(void)
{
  #define NBR_CYCLES_IN_LOOP   5   // this value is got from an oscilloscope measurement
  
  double src_ratio;
  double final_ratio;

  SYS_UWORD16 flash_access_size;
  SYS_UWORD16 flash_wait_state;
  SYS_UWORD32 nbr;
  SYS_UWORD32 arm_clock;

  //////////////////////////////////
  //  compute the ARM clock used  //
  //////////////////////////////////
  {
    SYS_UWORD16 arm_mclk_xp5;
    SYS_UWORD16 arm_ratio;
    SYS_UWORD16 clk_src;
    SYS_UWORD16 clkm_cntl_arm_clk_reg = * (volatile SYS_UWORD16 *) CLKM_CNTL_ARM_CLK;

	#if ((CHIPSET == 4) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 11) || (CHIPSET == 12) || ( CHIPSET == 15))
      clk_src  = (clkm_cntl_arm_clk_reg & MASK_CLKIN) >> 1;
      switch (clk_src)
      {
        case 0x00: //DPLL selected 
          // select the DPLL factor
           #if ((CHIPSET == 12) || (CHIPSET == 15))
              if (((* (volatile SYS_UWORD16 *) C_MAP_DPLL_BASE) & DPLL_LOCK) != 0)
           #else
              if (((* (volatile SYS_UWORD16 *) MEM_DPLL_ADDR) & DPLL_LOCK) != 0)
           #endif 
          {
             SYS_UWORD16 dpll_div;
             SYS_UWORD16 dpll_mul;

             dpll_div=DPLL_READ_DPLL_DIV;
             dpll_mul=DPLL_READ_DPLL_MUL;
             src_ratio = (double)(dpll_mul)/(double)(dpll_div+1);
          }
          else // DPLL in bypass mode
          {
             SYS_UWORD16 dpll_div = DPLL_BYPASS_DIV;
             src_ratio= (double)(1)/(double)(dpll_div+1);
          }
          break;
        case 0x01: //VTCX0 selected 
          src_ratio = 1;
          break;
        case 0x03: //CLKIN selected   (external clock)
          src_ratio = 1;
          break;
      }

	#if(CHIPSET != 15)
      // define the division factor applied to clock source (CLKIN or VTCXO or DPLL)
      arm_ratio = (clkm_cntl_arm_clk_reg & CLKM_MCLK_DIV) >> 4;

      // check if the 1.5 or 2.5 division factor is enabled
      arm_mclk_xp5  = clkm_cntl_arm_clk_reg & CLKM_ARM_MCLK_XP5;

      if (arm_mclk_xp5 == 0) // division factor enable for ARM clock ?
      {
        if (arm_ratio == 0) 
          arm_ratio =1; 
      }
      else
        arm_ratio = ((arm_ratio>>1) & 0x0001) == 0 ? 1.5 : 2.5;
	#else
	if(( clkm_cntl_arm_clk_reg& CLKM_SWITCH_FREQ))
		arm_ratio = 2;
	else 
		arm_ratio = 1;
	#endif

 #else
      src_ratio = 1;

      // define the division factor applied to clock source (CLKIN or VTCXO or DPLL)
      arm_ratio = (clkm_cntl_arm_clk_reg & CLKM_MCLK_DIV) >> 4;

      // check if the 1.5 or 2.5 division factor is enabled
      arm_mclk_xp5  = clkm_cntl_arm_clk_reg & MASK_ARM_MCLK_1P5;

      if (arm_mclk_xp5 == 1) // division factor enable for ARM clock ?
        arm_ratio = 1.5;  
      else
      {
        if (arm_ratio == 0)
          arm_ratio = 4;
        else 
          if (arm_ratio == 1 )
            arm_ratio = 2;
          else 
            arm_ratio = 1;
      }

     #endif

   final_ratio = (src_ratio / (double) arm_ratio);

  }
  //////////////////////////////////////////
  //  compute the Flash wait states used  //
  //////////////////////////////////////////

  #if (CHIPSET == 12 || CHIPSET == 15)
     flash_access_size  =  1;
  #else
    flash_access_size  =  *((volatile SYS_UWORD16 *) MEM_REG_nCS0);
  #endif
  flash_access_size  = (flash_access_size >> 5) & 0x0003; // 0=>8bits, 1=>16 bits, 2 =>32 bits

  // the loop file is compiled in 16 bits it means
  //    flash 8  bits => 2 loads for 1 16 bits assembler instruction
  //    flash 16 bits => 1 loads for 1 16 bits assembler instruction
  //    flash/internal RAM 32 bits => 1 loads for 1 16 bits assembler instruction (ARM bus 16 bits !!)
  
  // !!!!!!!!! be careful: if this file is compile in 32 bits, change these 2 lines here after !!!
  if (flash_access_size == 0) flash_access_size = 2;
  else                        flash_access_size = 1;

  #if (CHIPSET == 12 || CHIPSET == 15)
    /*
     *  loop move to run in internal memory, due to page mode in external memory
     */
    flash_wait_state  =  0;
  #else
    flash_wait_state  =  *((volatile SYS_UWORD16 *) MEM_REG_nCS0);
    flash_wait_state &=  0x001F;
  #endif

  //////////////////////////////////////
  //  compute the length of the loop  //
  //////////////////////////////////////

  // Number of flash cycles for the assembler loop
  nbr = NBR_CYCLES_IN_LOOP;

  // Number of ARM cycles for the assembler loop
  nbr = nbr * (flash_wait_state + 1) * (flash_access_size);

  // time for the assembler loop (unit nanoseconds: 10E-9)
	  arm_clock = final_ratio * 13; // ARM clock in Mhz 	
  ratio_wait_loop = (SYS_UWORD32)((nbr*1000) / arm_clock);
}
#endif // BOARD

#if ((CHIPSET != 12) && (CHIPSET != 15) )

/*-------------------------------------------------------*/
/* wait_ARM_cycles()                                     */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* Called when the HardWare needs time to wait.          */
/* this function wait x cycles and is used with the      */
/* convert_nanosec_to_cycles() & initialize_wait_loop()  */
/*                                                       */
/*  Exemple:  wait 10 micro seconds:                     */
/*  initialize_wait_loop();                              */
/*  wait_ARM_cycles(convert_nanosec_to_cycles(10000))    */
/*                                                       */
/*  minimum time value with cpt_loop = 0  (estimated)    */
/*  and C-SAMPLE / flash 6,5Mhz  ~  1,5 micro seconds    */
/*                                                       */
/*                                                       */
/* Be careful : in order to respect the rule about the   */
/* conversion "time => number of cylcles in this loop"   */
/* (Cf the functions: convert_nanosec_to_cycles() and    */
/* initialize_wait_loop() ) respect the following rules: */
/* This function must be placed in Flash Memory and      */
/* compiled in 16 bits instructions length               */
/*-------------------------------------------------------*/
void wait_ARM_cycles(SYS_UWORD32 cpt_loop)
{
  // C code:
  // while (cpt_loop -- != 0);

  asm(" CMP       A1, #0");
  asm(" BEQ       END_FUNCTION");

  asm("LOOP_LINE:        ");
  asm(" SUB       A1, A1, #1");
  asm(" CMP       A1, #0");
  asm(" BNE       LOOP_LINE");

  asm("END_FUNCTION:        ");
} 

#endif /* (CHIPSET != 12) && (CHIPSET != 15)*/
