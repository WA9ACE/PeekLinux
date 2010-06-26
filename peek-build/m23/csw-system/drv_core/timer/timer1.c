/******************************************************************************
            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION           
                                                                             
   Property of Texas Instruments -- For  Unrestricted  Internal  Use  Only 
   Unauthorized reproduction and/or distribution is strictly prohibited.  This 
   product  is  protected  under  copyright  law  and  trade  secret law as an 
   unpublished work.  Created 1987, (C) Copyright 1997 Texas Instruments.  All 
   rights reserved.                                                            
                  
                                                           
   Filename       	: timer1.c

   Description    	: Set of useful functions for TIMER module test cases 

   Project        	: drivers

   Author         	: pmonteil@tif.ti.com  Patrice Monteil.

   Version number	: 1.4

   Date and time	: 08/18/00 14:42:35

   Previous delta 	: 06/30/00 11:40:43

   SCCS file      	: /db/gsm_asp/db_ht96/dsp_0/gsw/rel_0/mcu_l1/release_gprs/RELEASE_GPRS/drivers1/common/SCCS/s.timer1.c

   Sccs Id  (SID)       : '@(#) timer1.c 1.4 08/18/00 14:42:35 '

 
*****************************************************************************/


#include "sys_types.h"
#include "memif/mem.h" 
#include "timer/timer1.h" 
#include "general.h"
#include "inth/sys_inth.h"

void (*timer1_callback) (void)=NULL;

#define USECS_TO_TICKS(time)                        ((1659L * time) >> 12)


/*--------------------------------------------------------------*/
/*  Dtimer1_Get_cntlreg()					*/
/*--------------------------------------------------------------*/
/* Parameters : none						*/
/* Return     :	none						*/
/* Functionality : read one of the TIMER register		*/
/*--------------------------------------------------------------*/

 SYS_UWORD16 Dtimer1_Get_cntlreg(void)
 {
 volatile SYS_UWORD16 timerReg;

      timerReg = (( * (SYS_UWORD16 *) D_TIMER_CNTL) & D_TIMER_CNTL_MASK);
   return(timerReg);
}


/*--------------------------------------------------------------*/
/*  Dtimer_Init_cntl()						*/
/*--------------------------------------------------------------*/
/* Parameters :start/stop,reload yes/no,pre scale,ext clk on/off*/
/* Return     :	none						*/
/* Functionality : initialize the timer start, autoreload, Ptv 	*/
/*		   and clock enable				*/
/*--------------------------------------------------------------*/

void Dtimer1_Init_cntl (SYS_UWORD16 St, SYS_UWORD16 Reload, SYS_UWORD16 clockScale, SYS_UWORD16 clkon)
{
    SYS_UWORD16 cntl = * (SYS_UWORD16 *) D_TIMER_CNTL; 


    cntl &= ~(D_TIMER_ST | D_TIMER_PTV);    	/* stop and reset values */

    (Reload) ? (cntl |= D_TIMER_AR) : (cntl &= ~D_TIMER_AR);

    cntl |= (clockScale << 2 );

    cntl |= ( clkon << 5 );


    * (SYS_UWORD16 *) D_TIMER_LOAD = St;

    * (SYS_UWORD16 *) D_TIMER_CNTL = cntl;
    }

/*--------------------------------------------------------------*/
/*  Dtimer1_AR()						*/
/*--------------------------------------------------------------*/
/* Parameters :start/stop,reload yes/no,pre scale,ext clk on/off*/
/* Return     :	none						*/
/* Functionality : set the AR bit Ar = 0 | 1			*/
/*--------------------------------------------------------------*/

void Dtimer1_AR(SYS_UWORD16 Ar)
{
SYS_UWORD16 cntl = * (SYS_UWORD16 *) D_TIMER_CNTL; 

 cntl &= ~(D_TIMER_ST);			/* stop the timer */

 cntl &= ~(D_TIMER_AR);

 cntl |= ( Ar << 1 );

 * (SYS_UWORD16 *) D_TIMER_CNTL = cntl;
}

/*--------------------------------------------------------------*/
/*  Dtimer1_PTV()						*/
/*--------------------------------------------------------------*/
/* Parameters : pre scale					*/
/* Return     :	none						*/
/* Functionality : set the Ptv bits Ptv = 0 => 7 		*/
/*--------------------------------------------------------------*/

void Dtimer1_PTV(SYS_UWORD16 Ptv)
{
SYS_UWORD16 cntl = * (SYS_UWORD16 *) D_TIMER_CNTL;	
 
 cntl &= ~(D_TIMER_ST);			/* stop the timer */

 cntl &= ~(D_TIMER_PTV);		/* Ptv[4:0] set to 0 */

 cntl |= ( Ptv << 2 );

 * (SYS_UWORD16 *) D_TIMER_CNTL = cntl;
}

/*--------------------------------------------------------------*/
/*     Dtimer1_Clken()						*/
/*--------------------------------------------------------------*/
/* Parameters : ext clk on/off					*/
/* Return     :	none						*/
/* Functionality :  set the clock_enable bit En = 0 | 1 	*/
/*--------------------------------------------------------------*/

void Dtimer1_Clken(SYS_UWORD16 En)
{
SYS_UWORD16 cntl = * (SYS_UWORD16 *) D_TIMER_CNTL;
 
 cntl &= ( ~D_TIMER_CLK_EN);

 cntl |= ( En << 5 );

 * (SYS_UWORD16 *) D_TIMER_CNTL = cntl;
}



/*--------------------------------------------------------------*/
/*  Dtimer_Start()						*/
/*--------------------------------------------------------------*/
/* Parameters : on/off						*/
/* Return     :	none						*/
/* Functionality :  Start or stop the timer 			*/
/*--------------------------------------------------------------*/

void  Dtimer1_Start (SYS_UWORD16 startStop)
{

	
    if (startStop == D_TIMER_ST) 
		{
		((* (SYS_UWORD16 *) D_TIMER_CNTL) |=  D_TIMER_ST) ;	/* condition vrai */
		 F_INTH_ENABLE_ONE_IT(C_INTH_TIMER1_IT);
	    	}
	else
		{
		
		((* (SYS_UWORD16 *) D_TIMER_CNTL) &= ~D_TIMER_ST);    	/* condition fausse */
		 F_INTH_DISABLE_ONE_IT(C_INTH_TIMER1_IT);
		}	

}


/*--------------------------------------------------------------*/
/*  Dtimer_ReadValue()						*/
/*--------------------------------------------------------------*/
/* Parameters : on/off						*/
/* Return     :	none						*/
/* Functionality :  Read timer value				*/
/*--------------------------------------------------------------*/

/* NEW COMPILER MANAGEMENT
 * Removal of inline on function.
 * With new compiler, inline means static inline involving the
 * function to not be seen outside this file.
 */
SYS_UWORD16 Dtimer1_ReadValue (void)
{
 return(* (SYS_UWORD16 *) D_TIMER_READ);
 
}

/*--------------------------------------------------------------*/
/* Parameters : on/off						*/
/* Return     :	none						*/
/* Functionality :  Write timer value				*/
/*--------------------------------------------------------------*/

/* NEW COMPILER MANAGEMENT
 * Removal of inline on function.
 * With new compiler, inline means static inline involving the
 * function to not be seen outside this file.
 */
void Dtimer1_WriteValue (SYS_UWORD16 value)
{
 (* (SYS_UWORD16 *) D_TIMER_LOAD) = value;
}


/* minimum delay possible is 2.46us */
void Dtimer1_Register (SYS_UWORD16 timeout_value,BOOL is_continous, void (*timeout_callback)(void))
{
 Dtimer1_Start (0);
/* formula is Delay=Tclk * (LOAD_TM + 1) * 2^(PTV+1) : Tclk=1.23us, PTV=0*/
/* LOAD_TM=(timeout_value/2.46)-1;*/
 Dtimer1_WriteValue(USECS_TO_TICKS(timeout_value));
 Dtimer1_PTV(0);
 Dtimer1_AR(is_continous);
 timer1_callback=timeout_callback;
}





