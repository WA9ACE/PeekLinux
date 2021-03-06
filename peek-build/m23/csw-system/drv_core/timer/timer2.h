/*******************************************************************************
            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION           
                                                                             
  Property of Texas Instruments -- For  Unrestricted  Internal  Use  Only 
   Unauthorized reproduction and/or distribution is strictly prohibited.  This 
   product  is  protected  under  copyright  law  and  trade  secret law as an 
   unpublished work.  Created 1987, (C) Copyright 1997 Texas Instruments.  All 
   rights reserved.                                                            
                 
                                                           
   Filename       	: timer2.h

   Description    	:TIMER1 

   Project        	: drivers

   Author         	: pmonteil@tif.ti.com  Patrice Monteil.

   Version number	: 1.4

   Date and time	: 02/15/01 15:47:05

   Previous delta 	: 02/15/01 15:47:05

   SCCS file      	: /db/gsm_asp/db_ht96/dsp_0/gsw/rel_0/mcu_l1/release_gprs/mod/emu_p/EMU_P_FRED/drivers1/common/SCCS/s.timer2.h

   Sccs Id  (SID)       : '@(#) timer2.h 1.4 02/15/01 15:47:05 '


 
*****************************************************************************/

#include "sys_types.h"

/**** DIONE TIMERs configuration register ****/

#define D_TIMER_ADDR        0xfffe6800

#define D_TIMER_CNTL_MASK	(0x003f)

#define CNTL_D_TIMER_OFFSET	0x0000
#define LOAD_D_TIMER_OFFSET	0x0002
#define READ_D_TIMER_OFFSET	0x0004

#define D_TIMER_CNTL		(D_TIMER_ADDR+CNTL_D_TIMER_OFFSET)
#define D_TIMER_LOAD		(D_TIMER_ADDR+LOAD_D_TIMER_OFFSET)
#define D_TIMER_READ		(D_TIMER_ADDR+READ_D_TIMER_OFFSET)

#define D_TIMER_ST		0x0001		/* bit 0 */
#define D_TIMER_AR		0x0002		/* bit 1 */
#define D_TIMER_PTV		0x001c		/* bits 4:2 */
#define D_TIMER_CLK_EN		0x0020		/* bit 5  */
#define D_TIMER_RUN		0x0021		/* bit 5 ,0 */

#define LOAD_TIM		0xffff		/* bits 15:0 */





/* ----- Prototypes ----- */
SYS_UWORD16 Dtimer2_Get_cntlreg(void);

void Dtimer2_AR(SYS_UWORD16 Ar);

void Dtimer2_PTV(SYS_UWORD16 Ptv);

void Dtimer2_Clken(SYS_UWORD16 En);

void  Dtimer2_Start (SYS_UWORD16 startStop);

void Dtimer2_Init_cntl (SYS_UWORD16 St, SYS_UWORD16 Reload, SYS_UWORD16 clockScale, SYS_UWORD16 clkon);

void Dtimer2_WriteValue (SYS_UWORD16 value);

SYS_UWORD16 Dtimer2_ReadValue (void);
