/*                @(#) nom : sys_inth.c SID: 1.2 date : 05/23/03              */
/* Filename:      sys_inth.c                                                  */
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
 *  FILE NAME: sys_inth.c
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
  #include "inth/sys_inth.h"


  /******************************************************************************
   *
   *  FUNCTION NAME: f_inth_init
   *      Setup interrupt handler configuration.
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

  static void f_inth_init(SYS_UWORD16 d_it_num, T_INTH_CONFIG  *p_config)
  {
    volatile SYS_UWORD16 *p_inth_level_reg;
    SYS_UWORD8 d_it_num_temp = d_it_num;

    /*
     *  Get interrupt handler main address between INTH and SEC_INTH
     */
    if (d_it_num < C_INTH_NUM_INT)
      p_inth_level_reg = (SYS_UWORD16 *) C_INTH_EXT_REG;
#if (VIRTIO==0)  
    else if (d_it_num < (C_INTH_NUM_INT + C_INTH_2ND_NUM_INT)) {
      p_inth_level_reg = (SYS_UWORD16 *) C_INTH2_EXT_REG;
      d_it_num_temp= d_it_num - C_INTH_NUM_INT;
    }
#endif
    else
      return;
  
    /*
     *  Set the configuration of the selected interrupt
     */
    p_inth_level_reg[d_it_num_temp] = p_config->d_fiq_nirq | 
                                     (p_config->d_edge_nlevel << 1) | 
                                     (p_config->d_priority << 2);
  
    /*
     *  Set indirect table
     */
    /* Is main interrupt handler? */
    if (d_it_num < C_INTH_NUM_INT)
      if (p_config->d_fiq_nirq == C_INTH_IRQ)
        a_inth_it_handlers[d_it_num_temp][C_INTH_IRQ] = p_config->d_it_handler;
      else
        a_inth_it_handlers[d_it_num_temp][C_INTH_FIQ] = p_config->d_it_handler;
    /* This is the 2nd level interrupt handler */
    else
      a_inth2_irq_handlers[d_it_num_temp] = p_config->d_it_handler;

  } /* f_inth_init() */



  /******************************************************************************
   *
   *  FUNCTION NAME: f_inth_setup
   *      Setup interrupt handler configuration.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument        Type               IO  Description
   *  -------------   ----------------   --  ----------------------------------
   *  p_inth_config   T_INTH_CONFIG *    I   Interrupt handler configuration.
   *
   * RETURN VALUE: None
   *
   *****************************************************************************/

  void f_inth_setup(T_INTH_CONFIG *p_inth_config)
  {
     SYS_UWORD8 d_i;
  
     /* Setup all interrupts to IRQ or FIQ with different levels and priority */
     for (d_i = 0 ; d_i < (C_INTH_NUM_INT + C_INTH_2ND_NUM_INT) ; d_i++) {
        if (p_inth_config[d_i].d_priority != 0xFF) {
          f_inth_init(d_i, (T_INTH_CONFIG *)&p_inth_config[d_i]);
        }
        F_INTH_DISABLE_ONE_IT(d_i);
     }
  
  } /* f_inth_setup() */



  /******************************************************************************
   *
   *  FUNCTION NAME: f_inth_get_version
   *      Return the version of the interrupt handler driver.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument       Type                   IO  Description
   *  ------------   -------------------    --  ---------------------------------
   *
   * RETURN VALUE: Version of the INTH driver
   *
   *****************************************************************************/

  SYS_UWORD16 f_inth_get_version(void) {
    return(C_INTH_DRIVER_VERSION);
  }


#endif /* (CHIPSET == 12) */

