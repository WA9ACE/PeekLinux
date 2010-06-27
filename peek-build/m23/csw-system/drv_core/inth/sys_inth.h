/*                @(#) nom : sys_inth.h SID: 1.2 date : 05/23/03              */
/* Filename:      sys_inth.h                                                  */
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
 *  FILE NAME: sys_inth.h
 *
 *
 *  PURPOSE:  Header file for the Interrupt Handler Driver.
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
 *   9-Oct-2003  G.Leterrier     0.0.2    add volatile access to MACRO
 *                                        F_INTH_GET_IRQ,F_INTH_GET_FIQ,
 *                                        F_INTH2_GET_IRQa
 *  19-Aug-2005  Sabyasachi	 0.0.3    F_INTH_ENABLE_ONE_IT, F_INTH_DISABLE_ONE_IT
 *                                        modified to take care of compilation warnings.
 *  ALGORITHM: 
 *
 *
 *****************************************************************************/

#include "chipset.cfg"

#if ((CHIPSET == 12 || CHIPSET == 15))

  #ifndef __SYS_INTH_H__
    #define __SYS_INTH_H__

    #include "sys_types.h"
    #include "sys_map.h"
    #include "nucleus.h"


    /*
     *  Driver version
     */
    #define C_INTH_DRIVER_VERSION 0x0002
  
  
    /*
     *  Number of interrupts
     */
    #define C_INTH_NUM_INT        32
    #define C_INTH_2ND_NUM_INT    5
    
    #define C_INTH_NB_INTERRUPT   (C_INTH_NUM_INT + C_INTH_2ND_NUM_INT)
  

#if(CHIPSET == 12)
    /*
     *  Main INTH : Interrupt bit numbers
     */
    #define C_INTH_WATCHDOG_IT       0
    #define C_INTH_TIMER1_IT         1
    #define C_INTH_TIMER2_IT         2
    #define C_INTH_TSP_RECEIVE_IT    3
    #define C_INTH_FRAME_IT          4
    #define C_INTH_PAGE_IT           5
    #define C_INTH_SIM_IT            6
    #define C_INTH_UART_MODEM1_IT    7
    #define C_INTH_KEYBOARD_IT       8
    #define C_INTH_RTC_TIMER_IT      9
    #define C_INTH_RTC_ALARM_IT      10
    #define C_INTH_ULPD_GAUGING_IT   11
    #define C_INTH_ABB_IRQ_IT        12
    #define C_INTH_SPIv              13
    #define C_INTH_DMA_IT            14
    #define C_INTH_API_IT            15
    #define C_INTH_GPIO_IT           16
    #define C_INTH_ABB_FIQ_IT        17
    #define C_INTH_UART_IRDA_IT      18
    #define C_INTH_TGSM_IT           19
    #define C_INTH_GEA_IT            20
    #define C_INTH_EXT_IRQ1_IT       21
    #define C_INTH_EXT_IRQ2_IT       22
    #define C_INTH_USIM_CD_IT        23
    #define C_INTH_USIM_IT           24
    #define C_INTH_LCD_IT            25
    #define C_INTH_USB_IT            26
    #define C_INTH_MMC_SD_MS_IT      27
    #define C_INTH_UART_MODEM2_IT    28
    #define C_INTH_2ND_INTH_IT       29
    #define C_INTH_I2C_IT            30
    #define C_INTH_NAND_FLASH_IT     31
#elif (CHIPSET == 15)
    #define C_INTH_WATCHDOG_IT       0
    #define C_INTH_TIMER1_IT         1
    #define C_INTH_TIMER2_IT         2
    #define C_INTH_MCSI_IT    	     3
    #define C_INTH_FRAME_IT          4
    #define C_INTH_PAGE_IT           5
    #define C_INTH_DRP_DBB_SIT       6
    #define C_INTH_UART_IRDA_IT      7
    #define C_INTH_KEYBOARD_IT       8
    #define C_INTH_DRP_DBB_RX_IT     9
    #define C_INTH_CAM_IT            10
    #define C_INTH_ULPD_GAUGING_IT   11
    #define C_INTH_ABB_IRQ_IT        12
    #define C_INTH_MSSPI_IT          13
    #define C_INTH_DMA_IT            14
    #define C_INTH_API_IT            15
    #define C_INTH_GPIO_IT           16
    #define C_INTH_UART_WAKEUP_IT    17
    #define C_INTH_DRP_DBB_TX_IT     18
    #define C_INTH_TGSM_IT           19
    #define C_INTH_GEA_IT            20
    #define C_INTH_GPIO1_IT          21
    #define C_INTH_GPIO2_IT          22
    #define C_INTH_CPORT_IT          23
    #define C_INTH_USIM_IT           24
    #define C_INTH_LCD_IT            25
    #define C_INTH_USB_IT            26
    #define C_INTH_I2C_TRITON_IT     28
    #define C_INTH_2ND_INTH_IT       29
    #define C_INTH_I2C_IT            30
    #define C_INTH_NAND_FLASH_IT     31

#endif 
    /*
     *  Second INTH : Interrupt bit numbers
     */
    #define C_INTH_RNG_IT            32
    #define C_INTH_SHA1_MD5_IT       33
    #define C_INTH_EMPU_IT           34
    #define C_INTH_SEC_DMA_IT        35
    #define C_INTH_SEC_TIMER_IT      36
  
  
    /*
     * Address of the Main interrupt handler registers
     */
    #define C_INTH_IT_REG1      C_MAP_INTH_BASE            /* INTH IT register 1 */
    #define C_INTH_IT_REG2     (C_MAP_INTH_BASE + 0x02)    /* INTH IT register 2 */
    #define C_INTH_MASK_REG1   (C_MAP_INTH_BASE + 0x08)    /* INTH mask register 1 */
    #define C_INTH_MASK_REG2   (C_MAP_INTH_BASE + 0x0a)    /* INTH mask register 2 */
    #define C_INTH_B_IRQ_REG   (C_MAP_INTH_BASE + 0x10)    /* INTH source binary IRQ reg. */
    #define C_INTH_B_FIQ_REG   (C_MAP_INTH_BASE + 0x12)    /* INTH source binary FIQ reg. */
    #define C_INTH_CTRL_REG    (C_MAP_INTH_BASE + 0x14)    /* INTH control register */
    #define C_INTH_EXT_REG     (C_MAP_INTH_BASE + 0x20)    /* INTH 1st external int. reg. */
  
    /*
     * Address of the Second interrupt handler registers
     */
    #define C_INTH2_IT_REG1   C_MAP_INTH_SEC_BASE          /* INTH IT register 1 */
    #define C_INTH2_MASK_REG1 (C_MAP_INTH_SEC_BASE + 0x08) /* INTH mask register 1 */
    #define C_INTH2_B_IRQ_REG (C_MAP_INTH_SEC_BASE + 0x10) /* INTH source binary IRQ reg. */
    #define C_INTH2_CTRL_REG  (C_MAP_INTH_SEC_BASE + 0x14) /* INTH control register */
    #define C_INTH2_EXT_REG   (C_MAP_INTH_SEC_BASE + 0x20) /* INTH 1st external int. reg. */
  
  
    /*
     * INTH_B_x_REG definition
     */
    #define C_INTH_SRC_NUM    0x001f
    #define C_INTH2_SRC_NUM   0x0007
  
    /*
     * ILR_IRQx_REG definition
     */
    #define C_INTH_IRQ        0
    #define C_INTH_FIQ        1
  
    #define C_INTH_LEVEL      0
    #define C_INTH_EDGE       1
  
  

    /****************************************************************************
     *                            MACRO DEFINITION
     ***************************************************************************/
  
    /****************************************************************************
     *
     *  MACRO NAME: F_INTH_ENABLE_ONE_IT
     *      Enable the interrupt specified in argument.
     *
     *
     *  ARGUMENT LIST:
     *
     *  Argument       Description
     *  ------------   ----------------------------------------------------------
     *  d_it           Interrupt index
     *
     * RETURN VALUE: None
     * NOTE:- It needs to be done in a interrupt free context , OMAPS00145223 
     *
     ***************************************************************************/

#if (VIRTIO==0)
    #define F_INTH_ENABLE_ONE_IT(d_it) {	\
	SYS_UWORD32 lockStateLocal; 	\
	lockStateLocal=NU_Local_Control_Interrupts(NU_DISABLE_INTERRUPTS);					\
	if (d_it < 16)	  \
      * (volatile SYS_UWORD16 *) C_INTH_MASK_REG1 &= ~(1 << d_it);                            \
        else if (d_it < C_INTH_NUM_INT) 												\
       * (volatile SYS_UWORD16 *) C_INTH_MASK_REG2 &= ~(1 << (d_it & 0xf) /*(d_it-16)*/); \
      else if (d_it < (C_INTH_NUM_INT + C_INTH_2ND_NUM_INT)) 							    \
       * (volatile SYS_UWORD16 *) C_INTH2_MASK_REG1 &= ~(1 << (d_it & 0x7) /*(d_it-32)*/ ); \
	NU_Local_Control_Interrupts(lockStateLocal);	\
	 \
    }
#else
	#define F_INTH_ENABLE_ONE_IT(d_it) {							 \
	SYS_UWORD32 lockStateLocal;  \
	lockStateLocal=NU_Local_Control_Interrupts(NU_DISABLE_INTERRUPTS);	\
	if (d_it < 16) 														\
        * (volatile SYS_UWORD16 *) C_INTH_MASK_REG1 &= ~(1 << d_it);		 \
      else if (d_it < C_INTH_NUM_INT) 									\
        * (volatile SYS_UWORD16 *) C_INTH_MASK_REG2 &= ~(1 << (d_it&0xF)); \
		NU_Local_Control_Interrupts(lockStateLocal);	\
		 \	
	}
#endif

  
  
  
    /****************************************************************************
     *
     *  MACRO NAME: F_INTH_DISABLE_ONE_IT
     *      Disable the interrupt specified in argument.
     *
     *
     *  ARGUMENT LIST:
     *
     *  Argument       Description
     *  ------------   ----------------------------------------------------------
     *  d_it           Interrupt index
     *
     * RETURN VALUE: None
     *
     * Note :  (d_it - 16) is replaced with (d_it & 0xf)
     *         (d_it - 32) is replaced with (d_it & 0x1f)
     *         It needs to be done in a interrupt free context , OMAPS00145223 
     ***************************************************************************/
#if (VIRTIO==0)  
    #define F_INTH_DISABLE_ONE_IT(d_it) { 										\
	SYS_UWORD32 lockStateLocal;		\
	lockStateLocal=NU_Local_Control_Interrupts(NU_DISABLE_INTERRUPTS);	\
	if (d_it < 16) 	\
	 * (volatile SYS_UWORD16 *) C_INTH_MASK_REG1 |= (1 << d_it); 				\
    else if (d_it < C_INTH_NUM_INT) 											\
     * (volatile SYS_UWORD16 *) C_INTH_MASK_REG2 |= (1 << (d_it & 0xF) /*(d_it-16)*/); \
    else if (d_it < (C_INTH_NUM_INT + C_INTH_2ND_NUM_INT)) 							\
    * (volatile SYS_UWORD16 *) C_INTH2_MASK_REG1 |= (1 << (d_it & 0x7) /*(d_it-32)*/); \
	NU_Local_Control_Interrupts(lockStateLocal);	\
	\
    }
#else
    #define F_INTH_DISABLE_ONE_IT(d_it) { 	\
	SYS_UWORD32 lockStateLocal;	\
	lockStateLocal=NU_Local_Control_Interrupts(NU_DISABLE_INTERRUPTS);				\
	  if (d_it < 16)	\
        * (volatile SYS_UWORD16 *) C_INTH_MASK_REG1 |= (1 << d_it); 				\
      else if (d_it < C_INTH_NUM_INT) 											\
      * (volatile SYS_UWORD16 *) C_INTH_MASK_REG2 |= (1 << (d_it&0xF));     			\
	NU_Local_Control_Interrupts(lockStateLocal);	\
	  \
	 }
#endif

  
  
    /****************************************************************************
     *
     *  MACRO NAME: F_INTH_DISABLE_ALL_IT
     *      Disable all interrupts.
     *
     *
     *  ARGUMENT LIST:
     *
     *  Argument       Description
     *  ------------   ----------------------------------------------------------
     *
     * RETURN VALUE: None
     *
     ***************************************************************************/
#if (VIRTIO==0)  
    #define F_INTH_DISABLE_ALL_IT { \
      * (volatile SYS_UWORD16 *) C_INTH_MASK_REG1  = 0xffff; \
      * (volatile SYS_UWORD16 *) C_INTH_MASK_REG2  = 0xffff; \
      * (volatile SYS_UWORD16 *) C_INTH2_MASK_REG1 = 0x001f; \
    }
#else
    #define F_INTH_DISABLE_ALL_IT { \
      * (volatile SYS_UWORD16 *) C_INTH_MASK_REG1  = 0xffff; \
      * (volatile SYS_UWORD16 *) C_INTH_MASK_REG2  = 0xffff; \
    }
#endif
  
  
    /****************************************************************************
     *
     *  MACRO NAME: F_INTH_RESET_ALL_IT
     *      Reset all interrupts.
     *
     *
     *  ARGUMENT LIST:
     *
     *  Argument       Description
     *  ------------   ----------------------------------------------------------
     *
     * RETURN VALUE: None
     *
     ***************************************************************************/
#if (VIRTIO==0)  
    #define F_INTH_RESET_ALL_IT { \
      * (volatile SYS_UWORD16 *) C_INTH_IT_REG1  &= 0x0000; \
      * (volatile SYS_UWORD16 *) C_INTH_IT_REG2  &= 0x0000; \
      * (volatile SYS_UWORD16 *) C_INTH2_IT_REG1 &= 0x0000; \
    }
#else
    #define F_INTH_RESET_ALL_IT { \
      * (volatile SYS_UWORD16 *) C_INTH_IT_REG1  &= 0x0000; \
      * (volatile SYS_UWORD16 *) C_INTH_IT_REG2  &= 0x0000; \
   }
#endif  

    /****************************************************************************
     *
     *  MACRO NAME: F_INTH_RESET_ONE_IT
     *      Reset the interrupt specified in argument.
     *
     *
     *  ARGUMENT LIST:
     *
     *  Argument       Description
     *  ------------   ----------------------------------------------------------
     *  d_it           Interrupt index
     *
     * RETURN VALUE: None
     *
     ***************************************************************************/
#if (VIRTIO==0)  
    #define F_INTH_RESET_ONE_IT(d_it) { \
      if (d_it < 16) \
        * (volatile SYS_UWORD16 *) C_INTH_IT_REG1 &= ~(1 << (d_it & 0xf)); \
      else if (d_it < C_INTH_NUM_INT) \
        * (volatile SYS_UWORD16 *) C_INTH_IT_REG2 &= ~(1 << (d_it & 0xf)); \
      else if (d_it < (C_INTH_NUM_INT + C_INTH_2ND_NUM_INT)) \
        * (volatile SYS_UWORD16 *) C_INTH2_IT_REG1 &= ~(1 << (d_it & 0x7)); \
    }
#else
    #define F_INTH_RESET_ONE_IT(d_it) { \
      if (d_it < 16) \
        * (volatile SYS_UWORD16 *) C_INTH_IT_REG1 &= ~(1 <<(d_it & 0xf)); \
      else if (d_it < C_INTH_NUM_INT) \
        * (volatile SYS_UWORD16 *) C_INTH_IT_REG2 &= ~(1 << (d_it & 0xf)); \
    }
#endif


    /****************************************************************************
     *
     *  MACRO NAME: F_INTH_VALID_NEXT
     *      Valid next interrupt on the main interrupt handler.
     *
     *
     *  ARGUMENT LIST:
     *
     *  Argument       Description
     *  ------------   ----------------------------------------------------------
     *  d_fiq_nirq     Source of the interrupt IRQ or FIQ
     *
     * RETURN VALUE: None
     *
     ***************************************************************************/
  
    #define F_INTH_VALID_NEXT(d_fiq_nirq)   (* (volatile SYS_UWORD16 *) C_INTH_CTRL_REG |= (1 << d_fiq_nirq))
  
  
  
    /****************************************************************************
     *
     *  MACRO NAME: F_INTH2_VALID_NEXT
     *      Valid next interrupt on the 2nd level interrupt handler.
     *
     *
     *  ARGUMENT LIST:
     *
     *  Argument       Description
     *  ------------   ----------------------------------------------------------
     *  d_fiq_nirq     Source of the interrupt IRQ or FIQ.
     *
     * RETURN VALUE: None
     *
     ***************************************************************************/
#if (VIRTIO==0)
    #define F_INTH2_VALID_NEXT(d_fiq_nirq)  (* (volatile SYS_UWORD16 *) C_INTH2_CTRL_REG |= (1 << d_fiq_nirq))
#else
    #define F_INTH2_VALID_NEXT(d_fiq_nirq)  
#endif  
  
    /****************************************************************************
     *
     *  MACRO NAME: F_INTH_GET_IRQ
     *      Return pending IRQ interrupt index on the main interrupt handler.
     *
     *
     *  ARGUMENT LIST:
     *
     *  Argument       Description
     *  ------------   ----------------------------------------------------------
     *
     * RETURN VALUE: IRQ interrupt index
     *
     ***************************************************************************/
  
    #define F_INTH_GET_IRQ  ((* (volatile SYS_UWORD16 *) C_INTH_B_IRQ_REG) & C_INTH_SRC_NUM)
  
  
  
    /****************************************************************************
     *
     *  MACRO NAME: F_INTH_GET_FIQ
     *      Return pending FIQ interrupt index on the main interrupt handler.
     *
     *
     *  ARGUMENT LIST:
     *
     *  Argument       Description
     *  ------------   ----------------------------------------------------------
     *
     * RETURN VALUE: FIQ interrupt index
     *
     ***************************************************************************/
  
    #define F_INTH_GET_FIQ  ((* (volatile SYS_UWORD16 *) C_INTH_B_FIQ_REG) & C_INTH_SRC_NUM)
  
  
  
    /****************************************************************************
     *
     *  MACRO NAME: F_INTH2_GET_IRQ
     *      Return pending IRQ interrupt index on the 2nd level interrupt handler.
     *
     *
     *  ARGUMENT LIST:
     *
     *  Argument       Description
     *  ------------   ----------------------------------------------------------
     *
     * RETURN VALUE: FIQ interrupt index
     *
     ***************************************************************************/
#if (VIRTIO==0)  
    #define F_INTH2_GET_IRQ  ((* (volatile SYS_UWORD16 *) C_INTH2_B_IRQ_REG) & C_INTH2_SRC_NUM)
#else
	#define F_INTH2_GET_IRQ
#endif
  
  
    /****************************************************************************
     *                            STRUCTURE DEFINITION
     ***************************************************************************/
    
    typedef struct {
      SYS_UWORD8    d_fiq_nirq;
      SYS_UWORD8    d_edge_nlevel;
      SYS_UWORD8    d_priority;
      SYS_FUNC      d_it_handler;
    } T_INTH_CONFIG;
  
  

    /****************************************************************************
     *                         GLOBAL VARIABLES REFERENCE
     ***************************************************************************/
  
    extern SYS_FUNC a_inth_it_handlers[C_INTH_NUM_INT][2];
    extern SYS_FUNC a_inth2_irq_handlers[C_INTH_2ND_NUM_INT];
  
  
    /****************************************************************************
     *                            PROTOTYPE DEFINITION
     ***************************************************************************/
  
    /*
     *  16-BIS functions
     */
    extern void           f_inth_setup(T_INTH_CONFIG *p_inth_config);
    extern SYS_UWORD16    f_inth_get_version(void);
  
    /*
     *  32-BIS functions
     */
    extern void           f_inth_dummy(void);
    extern void           f_inth_irq_handler(void);
    extern void           f_inth_fiq_handler(void);
    extern void           f_inth_2nd_level_handler(void);

  #endif /* __SYS_INTH_H__ */

#endif /* (CHIPSET == 12)  || (CHIPSET==15) */
