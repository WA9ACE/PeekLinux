/*                @(#) nom : sys_dma_it.c SID: 1.2 date : 05/23/03            */
/* Filename:      sys_dma_it.c                                                */
/* Version:       1.2                                                         */
/******************************************************************************
 *                   WIRELESS COMMUNICATION SYSTEM DEVELOPMENT
 *
 *             (C) 2002 Texas Instruments France. All rights reserved
 *
 *                          Author : Guillaume Leterrier
 *                                   Francois Amand
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
 *  FILE NAME: sys_dma_it.c
 *
 *
 *  PURPOSE:  Interruption DMA drivers for CALYPSO PLUS.
 *    
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
*  23-Oct-2002  G.Leterrier    0.0.1    First implementation
*
*  ALGORITHM: 
*
*
*****************************************************************************/

#include "chipset.cfg"

#if(OP_L1_STANDALONE==0)
#include "rv_swe.h"
#endif

#if (defined(RVM_CAMD_SWE)) 
#include "dma/dma_api.h"
extern T_DMA_CHANNEL camd_available_dma_channel;
#endif

#if (CHIPSET == 12 || CHIPSET == 15)

#include "dma/sys_dma.h"


/*****************************************************************************/
/* global variable definition */
/***************************************************************************/


/* Array of pointer on call back function with argument SYS_UWORD16. */
/* This argument is the value of the status IT register of the channel which has generated the IT event. */
/* The size of the array is defined by the number of DMA channel available on the device */



T_DMA_CALL_BACK pf_dma_call_back_address[C_DMA_NUMBER_OF_CHANNEL]=
{f_dma_default_call_back_it,
    f_dma_default_call_back_it,
    f_dma_default_call_back_it,
    f_dma_default_call_back_it,
    f_dma_default_call_back_it,
    f_dma_default_call_back_it,
};



/******************************************************************************
 *
 *  FUNCTION NAME:f_dma_interrupt_manager
 *    The function is called when a DMA interrupt event occurs on the ARM interrupt handler. 
 *
 *  ARGUMENT LIST:
 *
 *  Argument     Type         IO  Description
 *  ----------   ----------   --  -------------------------------------------
 *  none
 *
 *
 * RETURN VALUE: none
 *
 *****************************************************************************/
int Cam_Dma_callback_count=0;
int Cam_Dma_callback_count_error=0;

void f_dma_interrupt_manager(void)
{

    SYS_UWORD16 d_dma_isr;
    SYS_UWORD8  d_dma_index;

    SYS_UWORD8 ret=0; 
    SYS_WORD16 dma_status=0;
    /* MASK to eliminate interruption from channel allocation to DSP and secure channel allocated to ARM */
    d_dma_isr=C_DMA_ISR_REG & (~C_DMA_CAR_REG) & (~C_DMA_SCR_REG);    


    /* shift ISR register from channel 0 to channel n in order to determine the channel with interrrupt active*/

    /* end condition need to be checked */
    for (d_dma_index=0;d_dma_index<C_DMA_NUMBER_OF_CHANNEL;d_dma_index++)  
    {

#if (defined(RVM_CAMD_SWE)) 

        /*The following is needed for camera's operation in continuous mode */
        if ((d_dma_isr & 0x0001)==1)
        {

            dma_status=F_DMA_GET_CHANNEL_IT_STATUS(d_dma_index);
            if(d_dma_index!=camd_available_dma_channel)  {  
               pf_dma_call_back_address[d_dma_index](dma_status);
            }
            else if (d_dma_index==camd_available_dma_channel) {


               ret=Cam_Dma_callback(dma_status);

               if(ret==0) {
                  pf_dma_call_back_address[d_dma_index](dma_status);
               }


            }
        } /* JUMP to the call back function with the status register in argument */
        d_dma_isr >>=1;

#else	
  
        if ((d_dma_isr & 0x0001)==1)
        {
	                pf_dma_call_back_address[d_dma_index](F_DMA_GET_CHANNEL_IT_STATUS(d_dma_index));
        } /* JUMP to the call back function with the status register in argument */
        d_dma_isr >>=1;

#endif
    }

}


/******************************************************************************
 *
 *  FUNCTION NAME:f_dma_secure_interrupt_manager
 *    The function is called when a secure DMA interrupt event occurs on the ARM interrupt handler. 
 *
 *  ARGUMENT LIST:
 *
 *   Argument     Type         IO  Description
 *  ----------   ----------   --  -------------------------------------------
 *  none
 *
 *
 * RETURN VALUE: none
 *
 *****************************************************************************/
void f_dma_secure_interrupt_manager(void)
{

    SYS_UWORD16 d_dma_isr;
    SYS_UWORD8  d_dma_index;
    /* MASK to eliminate interruption from channel allocation to DSP and non-secure channel allocated to ARM */
    d_dma_isr=C_DMA_ISR_REG & (~C_DMA_CAR_REG) & (C_DMA_SCR_REG);      

    /* shift ISR register from channel 0 to channel n in order to determine the channel with interrrupt active*/

    /* end condition need to be checked */
    for (d_dma_index=0;d_dma_index<C_DMA_NUMBER_OF_CHANNEL;d_dma_index++)  
    {
        if ((d_dma_isr & 0x0001)==1) 
        { 
            pf_dma_call_back_address[d_dma_index](F_DMA_GET_CHANNEL_IT_STATUS(d_dma_index));
        } /* JUMP to the call back function with the status register in argument */
        d_dma_isr >>=1;
    }

} 


/******************************************************************************
 *
 *  FUNCTION NAME:f_dma_default_call_back_it
 *    The address of that function will be called by default
 *    by the DMA interrupt handler, if no call-back function has been defined yet on this channel
 *
 *  ARGUMENT LIST:
 *
 *  Argument          Type         IO  Description
 *  ----------        ----------   --  -------------------------------------------
 *  d_dma_channel_status_it  SYS_UWORD16    I    contains a default IT status of the channel from which the event occurs.
 *
 *
 * RETURN VALUE: none
 *
 *****************************************************************************/
void f_dma_default_call_back_it(SYS_UWORD16 d_dma_channel_it_status)
{
}






#endif /* (CHIPSET == 12) */

