/*******************************************************************************
            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION           
                                                                             
   Property of Texas Instruments -- For  Unrestricted  Internal  Use  Only 
   Unauthorized reproduction and/or distribution is strictly prohibited.  This 
   product  is  protected  under  copyright  law  and  trade  secret law as an 
   unpublished work.  Created 1987, (C) Copyright 1997 Texas Instruments.  All 
   rights reserved.                                                            
                  
                                                           
   Filename         : timer_sec.c

   Description      : timer_sec.c

   Project          : drivers

   Author           : pmonteil@tif.ti.com  Patrice Monteil.

   Version number : 1.3

   Date and time  : 07/23/98 16:25:32
   Previous delta   : 07/23/98 16:25:32

   SCCS file        : /db/gsm_asp/db_ht96/dsp_0/gsw/rel_0/mcu_l1/release1.5/mod/emu/EMU_MCMP/eva3_drivers/source/SCCS/s.timer.c

   Sccs Id  (SID)       : '@(#) timer.c 1.3 07/23/98 16:25:32 '

 
*****************************************************************************/

#include "chipset.cfg"

#if ((CHIPSET == 12)||(CHIPSET==15))

#include "sys_types.h"
#include "memif/mem.h"
#include "inth/iq.h"
#include "timer/timer.h"
#include "timer/timer_sec.h" 


/*--------------------------------------------------------------*/
/*  TIMER_Read()                                                */
/*--------------------------------------------------------------*/
/* Parameters : num of the register to be read                  */
/* Return     :value of the timer register read                 */
/* Functionality :  read one of the TIMER Secure register       */
/*--------------------------------------------------------------*/
/* NEW COMPILER MANAGEMENT
 * Removal of inline on function.
 * With new compiler, inline means static inline involving the
 * function to not be seen outside this file.
 */
 SYS_UWORD16 TIMER_SEC_Read (SYS_UWORD16 regNum) {
   volatile SYS_UWORD16 timerReg=0;

   switch (regNum) {
   case 0:
      timerReg = (( * (SYS_UWORD16 *) TIMER_SEC_CNTL_REG) & TIMER_CNTL_MASK);
      break;
   case 1:
      timerReg = ( * (SYS_UWORD16 *) TIMER_SEC_READ_REG);
      break;
   case 2:
      timerReg = (( * (SYS_UWORD16 *) TIMER_SEC_MODE_REG) & TIMER_MODE_MASK);
      break;
   default:
       break;
   }
   return(timerReg);
}


/* NEW COMPILER MANAGEMENT
 * Removal of inline on function.
 * With new compiler, inline means static inline involving the
 * function to not be seen outside this file.
 */
void TM_SEC_DisableWatchdog (void)
{
   /* volatile variable needed due C to optimization */
   volatile SYS_UWORD16 *reg = (volatile SYS_UWORD16 *) TIMER_SEC_MODE_REG;

   * reg = 0xf5;        
   * reg = 0xa0;
}


/*
 * TM_SEC_EnableWatchdog
 * 
 */
void TM_SEC_EnableWatchdog(void)
{
    * ((volatile SYS_UWORD16 *) TIMER_SEC_MODE_REG) = TIMER_WDOG;
}

/*
 * TM_SEC_ResetWatchdog
 * 
 * Parameter : Tick count 
 * Use a different value each time, otherwise watchdog bites 
 */
void TM_SEC_ResetWatchdog(SYS_UWORD16 count)
{
  * ((volatile SYS_UWORD16 *) TIMER_SEC_LOAD_REG) = count;
}


/*--------------------------------------------------------------*/
/*  TIMER_SEC_ReadValue()                                       */
/*--------------------------------------------------------------*/
/* Parameters : none                                            */
/* Return     : none                                            */
/* Functionality :  Read timer value                            */
/*--------------------------------------------------------------*/

/* NEW COMPILER MANAGEMENT
 * Removal of inline on function.
 * With new compiler, inline means static inline involving the
 * function to not be seen outside this file.
 */
SYS_UWORD16 TIMER_SEC_ReadValue (void)
{
 return(* (SYS_UWORD16 *) TIMER_SEC_READ_REG);
 
}

/*--------------------------------------------------------------*/
/*  TIMER_SEC_WriteValue()                                      */
/*--------------------------------------------------------------*/
/* Parameters : none                                            */
/* Return     : none                                            */
/* Functionality :  Write timer value                           */
/*--------------------------------------------------------------*/

/* NEW COMPILER MANAGEMENT
 * Removal of inline on function.
 * With new compiler, inline means static inline involving the
 * function to not be seen outside this file.
 */
void TIMER_SEC_WriteValue (SYS_UWORD16 value)
{
  * (SYS_UWORD16 *) TIMER_SEC_LOAD_REG = value; /*load the value */
}

#endif /* CHIPSET == 12 || CHIPSET == 15*/

