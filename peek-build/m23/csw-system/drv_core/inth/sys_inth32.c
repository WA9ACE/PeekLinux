/*                @(#) nom : sys_inth32.c SID: 1.2 date : 05/23/03            */
/* Filename:      sys_inth32.c                                                */
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
 *  FILE NAME: sys_inth32.c
 *
 *
 *  PURPOSE:  Interrupt Handler driver compiled in 32-bits mode.
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

#include "chipset.cfg"

#if (CHIPSET == 12 || CHIPSET == 15)
#include "sys_types.h"
#include "sys_map.h"
#include "inth/sys_inth.h"

  /*  GLOBAL CONSTANT VARIABLES:
   *
   *  Variables        Type     Description
   *  ---------------  -------  -----------------------------------------------
   *
   */
  void f_inth_dummy(void);


  /*  GLOBAL CONSTANT VARIABLES:
   *
   *  Variables        Type     Description
   *  ---------------  -------  -----------------------------------------------
   *
   */




  /*  GLOBAL VARIABLES:
   *
   *  Variables             Type         Description
   *  --------------------  -----------  --------------------------------------
   *  a_inth_it_handlers    SYS_FUNC **  Indirect function address for IRQ and
   *                                     FIQ on main interrupt handler.
   *  a_inth2_irq_handlers  SYS_FUNC *   Indirect function address for IRQ on
   *                                     2nd level interrupt handler.
   *  d_inth_dummy          SYS_UWORD32  Count the number of unwanted interrupts
   */

  SYS_FUNC a_inth_it_handlers[C_INTH_NUM_INT][2] = 
  { //  IRQ                       FIQ
     {  f_inth_dummy,             f_inth_dummy }, // 0  : WATCHDOG TIMER       
     {  f_inth_dummy,             f_inth_dummy }, // 1  : TIMER 1              
     {  f_inth_dummy,             f_inth_dummy }, // 2  : TIMER 2              
     {  f_inth_dummy,             f_inth_dummy }, // 3  : TSP RECEIVE          
     {  f_inth_dummy,             f_inth_dummy }, // 4  : TPU FRAME            
     {  f_inth_dummy,             f_inth_dummy }, // 5  : TPU PAGE             
     {  f_inth_dummy,             f_inth_dummy }, // 6  : SIM                  
     {  f_inth_dummy,             f_inth_dummy }, // 7  : UART_MODEM1          
     {  f_inth_dummy,             f_inth_dummy }, // 8  : KEYBOARD             
     {  f_inth_dummy,             f_inth_dummy }, // 9  : RTC_TIMER            
     {  f_inth_dummy,             f_inth_dummy }, // 10 : RTC_ALARM            
     {  f_inth_dummy,             f_inth_dummy }, // 11 : ULPD_GAUGING         
     {  f_inth_dummy,             f_inth_dummy }, // 12 : ABB_IRQ              
     {  f_inth_dummy,             f_inth_dummy }, // 13 : SPI                  
     {  f_inth_dummy,             f_inth_dummy }, // 14 : DMA                  
     {  f_inth_dummy,             f_inth_dummy }, // 15 : API                  
     {  f_inth_dummy,             f_inth_dummy }, // 16 : GPIO                 
     {  f_inth_dummy,             f_inth_dummy }, // 17 : ABB_FIQ              
     {  f_inth_dummy,             f_inth_dummy }, // 18 : UART_IRDA            
     {  f_inth_dummy,             f_inth_dummy }, // 19 : ULPD GSM TIMER       
     {  f_inth_dummy,             f_inth_dummy }, // 20 : GEA                  
     {  f_inth_dummy,             f_inth_dummy }, // 21 : EXTERNAL IRQ 1       
     {  f_inth_dummy,             f_inth_dummy }, // 22 : EXTERNAL IRQ 2       
     {  f_inth_dummy,             f_inth_dummy }, // 23 : USIM Card Detect     
     {  f_inth_dummy,             f_inth_dummy }, // 24 : USIM                 
     {  f_inth_dummy,             f_inth_dummy }, // 25 : LCD                  
     {  f_inth_dummy,             f_inth_dummy }, // 26 : USB                  
     {  f_inth_dummy,             f_inth_dummy }, // 27 : MMC/SD/MS            
     {  f_inth_dummy,             f_inth_dummy }, // 28 : UART_MODEM2          
     {  f_inth_2nd_level_handler, f_inth_dummy }, // 29 : 2nd Interrupt Handler
     {  f_inth_dummy,             f_inth_dummy }, // 30 : I2C                  
     {  f_inth_dummy,             f_inth_dummy }  // 31 : NAND FLASH           
  };
  
  SYS_FUNC a_inth2_irq_handlers[C_INTH_2ND_NUM_INT] = 
  {
    f_inth_dummy,              // 0  : RNG
    f_inth_dummy,              // 1  : SHA1/MD5
    f_inth_dummy,              // 2  : EMPU
    f_inth_dummy,              // 3  : Secure DMA
    f_inth_dummy               // 4  : Secure TIMER
  };

  SYS_UWORD32 d_inth_dummy = 0L;


  /******************************************************************************
   *
   *  FUNCTION NAME: f_inth_dummy
   *      Handle dummy interrupts. Call of this function should never occurs
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Type                   IO  Description
   *  ------------   -------------------    --  ---------------------------------
   *                                            chip-select.
   *
   * RETURN VALUE: None
   *
   *****************************************************************************/

  void f_inth_dummy(void) {
    d_inth_dummy++;
  } /* f_inth_dummy() */

extern  unsigned char uart_sleep_timer_enabled;
void SER_activate_timer_hisr (void);

#ifdef BTS
extern void BthalPmUartRxEventHandler(void);
#endif

void f_inth_uart_wakeup(void) {

#ifdef BTS
	//BT_activate_wakeup_hisr();	/*Activate BT Wakeup HISR during Wakeup from Deep Sleep*/
	BthalPmUartRxEventHandler(); /*Activate BT Wakeup HISR during Wakeup from Deep Sleep*/
#endif

	SER_activate_timer_hisr ();
       uart_sleep_timer_enabled = 1;
	F_INTH_DISABLE_ONE_IT(C_INTH_UART_WAKEUP_IT);
} 


  /******************************************************************************
   *
   *  FUNCTION NAME: f_inth_irq_handler
   *      Entry point of ARM IRQ sub-routine.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Type                   IO  Description
   *  ------------   -------------------    --  ---------------------------------
   *
   * RETURN VALUE: None
   *
   *****************************************************************************/

  void f_inth_irq_handler(void) {
    
    a_inth_it_handlers[F_INTH_GET_IRQ][C_INTH_IRQ](); /* ACK IT */
    F_INTH_VALID_NEXT(C_INTH_IRQ);                        /* valid next IRQ */
    
  } /* f_inth_irq_handler() */

  

  /******************************************************************************
   *
   *  FUNCTION NAME: f_inth_fiq_handler
   *      Entry point of ARM FIQ sub-routine.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Type                   IO  Description
   *  ------------   -------------------    --  ---------------------------------
   *
   * RETURN VALUE: None
   *
   *****************************************************************************/

  void f_inth_fiq_handler(void) {
    
    a_inth_it_handlers[F_INTH_GET_FIQ][C_INTH_FIQ](); /* ACK IT */
    F_INTH_VALID_NEXT(C_INTH_FIQ);                       /* valid next FIQ */
    
  } /* f_inth_fiq_handler() */

  

  /******************************************************************************
   *
   *  FUNCTION NAME: f_inth_2nd_level_handler
   *      Sub-routine to manage 2nd level interrupt handler.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Type                   IO  Description
   *  ------------   -------------------    --  ---------------------------------
   *
   * RETURN VALUE: None
   *
   *****************************************************************************/

  void f_inth_2nd_level_handler(void) {
#if (VIRTIO==0)    
    a_inth2_irq_handlers[F_INTH2_GET_IRQ]();  /* ACK IT */
    F_INTH2_VALID_NEXT(C_INTH_IRQ);              /* Valid next IRQ */
#endif
    
  } /* f_inth_2nd_level_handler() */


#endif /* (CHIPSET == 12) */
