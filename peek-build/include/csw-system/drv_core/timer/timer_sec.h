/*******************************************************************************
            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION           
                                                                             
   Property of Texas Instruments -- For  Unrestricted  Internal  Use  Only 
   Unauthorized reproduction and/or distribution is strictly prohibited.  This 
   product  is  protected  under  copyright  law  and  trade  secret law as an 
   unpublished work.  Created 1987, (C) Copyright 1997 Texas Instruments.  All 
   rights reserved.                                                            
                  
                                                           
   Filename       	: timer_sec.h

   Description    	: timer_sec.c header

   Project        	: drivers

   Author         	: pmonteil@tif.ti.com  Patrice Monteil.

   Version number	: 1.3

   Date and time	: 07/23/98 16:25:53
   Previous delta 	: 07/23/98 16:25:52

   SCCS file      	: /db/gsm_asp/db_ht96/dsp_0/gsw/rel_0/mcu_l1/release1.5/mod/emu/EMU_MCMP/eva3_drivers/source/SCCS/s.timer.h

   Sccs Id  (SID)       : '@(#) timer.h 1.3 07/23/98 16:25:53 '

 
*****************************************************************************/

#include "chipset.cfg"

#if ((CHIPSET == 12)||(CHIPSET==15))

#include "sys_types.h"

#define TIMER_SEC_CNTL_REG  MEM_TIMER_SEC_ADDR    	/* Secure watchdog Control Timer register */ 

#define TIMER_SEC_LOAD_REG  (MEM_TIMER_SEC_ADDR + 0x02)   /* Timer load register */ 
#define TIMER_SEC_READ_REG  (MEM_TIMER_SEC_ADDR + 0x02)   /* Timer read register */ 
#define TIMER_SEC_MODE_REG  (MEM_TIMER_SEC_ADDR + 0x04)   /* Timer mode register */ 


/*---------------------------------------------------------------*/
/*   TIMER_SEC_START_STOP ()						*/
/*--------------------------------------------------------------*/
/* Parameters : start or stop command	 			*/
/* Return     :	none						*/
/* Functionality : Start or Stop the timer  			*/
/*--------------------------------------------------------------*/
#define TIMER_SEC_START_STOP(startStop) ((startStop) ? ((* (volatile SYS_UWORD16 *) TIMER_SEC_CNTL_REG) |=  TIMER_ST) : \
((* (volatile SYS_UWORD16 *) TIMER_SEC_CNTL_REG) &= ~TIMER_ST))


/* Prototype of the functions */

        void        TM_SEC_DisableWatchdog(void);
        void        TM_SEC_EnableWatchdog(void);
        void        TM_SEC_ResetWatchdog(SYS_UWORD16 cnt);


/* NEW COMPILER MANAGEMENT
 * Removal of inline on functions:
 *    - TIMER_SEC_Read
 *    - TIMER_SEC_WriteValue
 *    - TIMER_SEC_ReadValue
 * With new compiler, inline means static inline involving the
 * function to not be seen outside this file.
 */
 
SYS_UWORD16 TIMER_SEC_Read (SYS_UWORD16);
void TIMER_SEC_WriteValue (SYS_UWORD16);
SYS_UWORD16 TIMER_SEC_ReadValue (void);

#endif /* CHIPSET == 12 || CHIPSET == 15 */
