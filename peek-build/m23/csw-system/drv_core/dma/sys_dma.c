/*                @(#) nom : sys_dma.c SID: 1.2 date : 05/23/03               */
/* Filename:      sys_dma.c                                                   */
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
 *  FILE NAME: sys_dma.c
 *
 *
 *  PURPOSE:  DMA drivers for CALYPSO PLUS.
 *            The drivers allows DMA module configuration and control.
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
 *   6-Feb-2003  G.Leterrier    0.0.2    Reset HW request to 0, before new setup in function "f_dma_channel_parameter_set"
 *  20-Feb-2003  G.Leterrier    0.0.3    f_dma_global_parameter_set correction on API priority bit
 *                                        disable and clear status register channel before re-configuration
 *  ALGORITHM: 
 *
 *
 *****************************************************************************/

#include "chipset.cfg"

#if (CHIPSET == 12 || CHIPSET == 15)

    #include "dma/sys_dma.h"
    #include "sys_memif.h"


    /***************************************************************************/
    /*          global variable
    *****************************************************************************/

    extern T_DMA_CALL_BACK pf_dma_call_back_address[C_DMA_NUMBER_OF_CHANNEL];

    /******************************************************************************
    *
    *  FUNCTION NAME: f_dma_get_version_driver
    *    This functionis used to get to DMA driver software version.
    *
    *  ARGUMENT LIST:
    *
    *  Argument     Type         IO  Description
    *  ----------   ----------   --  -------------------------------------------
    *  none
    *
    *
    * RETURN VALUE: SYS_UWORD16
    *
    *****************************************************************************/
    SYS_UWORD16 f_dma_get_version_driver(void)
      {
      return(C_DMA_VERSION_DRIVER);
      } /*f_dma_get_version_driver()*/

    /******************************************************************************
    *
    *  FUNCTION NAME:f_dma_global_parameter_set
    *    The function is used to setup the DMA global configuration.
    *
    *
    *  ARGUMENT LIST:
    *
    *  Argument          Type                 IO  Description
    *  ----------         ----------             --  -------------------------------------------
    *  p_dma_global_parameter  T_DMA_TYPE_GLOBAL_PARAMETER*   I    Pointer on data structure containing all global parameter of DMA configuration
    *
    *
    *
    * RETURN VALUE: None
    *
    *****************************************************************************/
    void f_dma_global_parameter_set(T_DMA_TYPE_GLOBAL_PARAMETER  *p_dma_global_parameter)
      {
      if (p_dma_global_parameter->d_dma_global_auto_gate==C_DMA_AUTO_GATE_ON)
        {
        C_DMA_GCR_REG |= (C_DMA_AUTO_GATE_ON << C_DMA_GCR_AUTO_GATE_POS );
        }

      else
        {
        C_DMA_GCR_REG &= ~( C_DMA_GCR_AUTO_GATE_MASK << C_DMA_GCR_AUTO_GATE_POS );
        }

      C_DMA_AR_REG = ( 
                        (p_dma_global_parameter->d_dma_global_api_prio <<  C_DMA_AR_API_PRIO_POS) 
                      | (p_dma_global_parameter->d_dma_global_rhea_prio << C_DMA_AR_RHEA_PRIO_POS) 
                      | (p_dma_global_parameter->d_dma_global_imif_prio << C_DMA_AR_IMIF_PRIO_POS)
#if (CHIPSET == 15)
					  | (p_dma_global_parameter->d_dma_global_iperif_prio << C_DMA_AR_IPERIF_PRIO_POS)
#endif
                      );

#if (CHIPSET == 15)
      f_emif_set_priority(p_dma_global_parameter->d_dma_global_emif_dma_prio,
      						p_dma_global_parameter->d_dma_global_emif_mcu_prio);
#if 0
	  C_EMIF_LRU_PRIORITY_REG = (
		                 (p_dma_global_parameter->d_dma_global_emif_dma_prio << C_EMIF_DMA_POS)
					   | (p_dma_global_parameter->d_dma_global_emif_mcu_prio << C_EMIF_MPU_POS)
					  );
#endif
#endif
      } /*f_dma_global_parameter_set() */

    /******************************************************************************
    *
    *  FUNCTION NAME: f_dma_channel_allocation_set
    *    This function defines the allocation of a channel.
    *  This function is used to set which channel can be configured through the ARM and which one can be configured through the DSP.
    *
    *
    *
    *  ARGUMENT LIST:
    *
    *  Argument            Type                IO  Description
    *  ----------          ----------            --  -------------------------------------------
    *  d_dma_channel_number    T_DMA_TYPE_CHANNEL_NUMBER    I  Variable providing the channel number
    *  d_dma_channel_allocation  T_DMA_TYPE_CHANNEL_ALLOCATION  I  Variable defining if the channel allocated to the ARM or the DSP.
    *
    *
    *
    * RETURN VALUE: None
    *
    *****************************************************************************/
    void f_dma_channel_allocation_set(T_DMA_TYPE_CHANNEL_NUMBER d_dma_channel_number, T_DMA_TYPE_CHANNEL_ALLOCATION d_dma_channel_allocation)
       {
       if (d_dma_channel_allocation==C_DMA_CHANNEL_DSP)
          {
            C_DMA_CAR_REG |= (C_DMA_CHANNEL_DSP << d_dma_channel_number);
          }

       else
          {
            C_DMA_CAR_REG &= ~( C_DMA_CHANNEL_DSP << d_dma_channel_number );
          }


      } /*f_dma_channel_allocation_set */

    /******************************************************************************
    *
    *  FUNCTION NAME: f_dma_channel_parameter_set
    *  The function is used to setup a DMA channel configuration.
    *
    *
    *  ARGUMENT LIST:
    *
    *  Argument              Type                 IO  Description
    *  ----------          ----------               --  -------------------------------------------
    *  p_dma_channel_parameter    T_DMA_TYPE_CHANNEL_PARAMETER *     I  Pointer on data structure containing all the parameter to setup a channel  
    *
    *
    *
    * RETURN VALUE: None
    *
    *****************************************************************************/
    void f_dma_channel_parameter_set(T_DMA_TYPE_CHANNEL_PARAMETER    *p_dma_channel_parameter)
       {
        SYS_UWORD16 d_temp_register;

        F_DMA_CHANNEL_DISABLE(p_dma_channel_parameter->d_dma_channel_number);

        d_temp_register=F_DMA_GET_CHANNEL_IT_STATUS(p_dma_channel_parameter->d_dma_channel_number);
        
        pf_dma_call_back_address[p_dma_channel_parameter->d_dma_channel_number]=p_dma_channel_parameter->pf_dma_call_back_address;



        if (p_dma_channel_parameter->d_dma_channel_secured==C_DMA_CHANNEL_SECURED)
          {
            C_DMA_SCR_REG |= (C_DMA_CHANNEL_SECURED << p_dma_channel_parameter->d_dma_channel_number);
          }

        else
          {
            C_DMA_SCR_REG &= ~( C_DMA_CHANNEL_SECURED << p_dma_channel_parameter->d_dma_channel_number );
          }

        C_DMA_CSDP_REG(p_dma_channel_parameter->d_dma_channel_number) = (

          (p_dma_channel_parameter->d_dma_channel_data_type    << C_DMA_CSDP_DATA_TYPE_POS )
        | (p_dma_channel_parameter->d_dma_channel_src_port    << C_DMA_CSDP_SRC_POS )
        | (p_dma_channel_parameter->d_dma_src_channel_packed  << C_DMA_CSDP_SRC_PACK_POS )
        | (p_dma_channel_parameter->d_dma_src_channel_burst_en  << C_DMA_CSDP_SRC_BURST_EN_POS )
        | (p_dma_channel_parameter->d_dma_channel_dst_port    << C_DMA_CSDP_DST_POS )
        | (p_dma_channel_parameter->d_dma_dst_channel_packed  << C_DMA_CSDP_DST_PACK_POS )
        | (p_dma_channel_parameter->d_dma_dst_channel_burst_en  << C_DMA_CSDP_DST_BURST_EN_POS )
                                 );

        C_DMA_CCR_REG(p_dma_channel_parameter->d_dma_channel_number) &= ~C_DMA_CCR_SYNC_MASK;



        C_DMA_CCR_REG(p_dma_channel_parameter->d_dma_channel_number) = (

          (p_dma_channel_parameter->d_dma_channel_hw_synch     << C_DMA_CCR_SYNC_POS  )
        | (p_dma_channel_parameter->d_dma_channel_priority     << C_DMA_CCR_PRIO_POS  )
        | (p_dma_channel_parameter->d_dma_channel_auto_init    << C_DMA_CCR_AUTO_INIT_POS  )
        | (p_dma_channel_parameter->d_dma_channel_fifo_flush   << C_DMA_CCR_FIFO_FLUSH_POS )
        | (p_dma_channel_parameter->d_dma_src_channel_addr_mode  << C_DMA_CCR_SRC_AMODE_POS  )
        | (p_dma_channel_parameter->d_dma_dst_channel_addr_mode  << C_DMA_CCR_DST_AMODE_POS  )
                                );

        C_DMA_CICR_REG(p_dma_channel_parameter->d_dma_channel_number) = (  

          (p_dma_channel_parameter->d_dma_channel_it_time_out  << C_DMA_CICR_TOUT_IE_POS  )
        | (p_dma_channel_parameter->d_dma_channel_it_drop      << C_DMA_CICR_DROP_IE_POS  )
        | (p_dma_channel_parameter->d_dma_channel_it_frame     <<C_DMA_CICR_FRAME_IE_POS  )                     
        | (p_dma_channel_parameter->d_dma_channel_it_block     << C_DMA_CICR_BLOCK_IE_POS  )
        | (p_dma_channel_parameter->d_dma_channel_it_half_block  << C_DMA_CICR_HALF_BLOCK_IE_POS  )
                                );

        C_DMA_CSSA_L_REG(p_dma_channel_parameter->d_dma_channel_number)= (p_dma_channel_parameter->d_dma_channel_src_address ); /* is a mask requested ? */


        C_DMA_CSSA_U_REG(p_dma_channel_parameter->d_dma_channel_number)= (p_dma_channel_parameter->d_dma_channel_src_address >> 16 ); /* is a mask requested ? */


        C_DMA_CDSA_L_REG(p_dma_channel_parameter->d_dma_channel_number)= (p_dma_channel_parameter->d_dma_channel_dst_address); /* is a mask requested ? */


        C_DMA_CDSA_U_REG(p_dma_channel_parameter->d_dma_channel_number)=  (p_dma_channel_parameter->d_dma_channel_dst_address >> 16 ); /* is a mask requested ? */

        C_DMA_CEN_REG(p_dma_channel_parameter->d_dma_channel_number) = (p_dma_channel_parameter->d_dma_channel_element_number );

        C_DMA_CFN_REG(p_dma_channel_parameter->d_dma_channel_number) = (p_dma_channel_parameter->d_dma_channel_frame_number );


        } /*f_dma_channel_parameter_set() */

    /******************************************************************************
    *
    *  FUNCTION NAME: f_dma_channel_enable 
    *    This function is used to enable a DMA transfer of a channel which could be depending on a hardware request.
    *  If there is no hardware request, the transfer starts immediately.
    *
    *  ARGUMENT LIST:
    *
    *  Argument          Type            IO  Description
    *  ----------        ----------          --  -------------------------------------------
    *  d_dma_channel_number  T_DMA_TYPE_CHANNEL_NUMBER  I  Variable to define which channel is enabled.
    *
    *
    *
    * RETURN VALUE: None
    *
    *****************************************************************************/
    void f_dma_channel_enable (T_DMA_TYPE_CHANNEL_NUMBER d_dma_channel_number)  
      {
      F_DMA_CHANNEL_ENABLE(d_dma_channel_number);
      }/* f_dma_channel_enable() */

    /******************************************************************************
    *
    *  FUNCTION NAME: f_dma_channel_disable
    *   This function is used to disable a DMA transfer of a channel
    *
    *
    *  ARGUMENT LIST:
    *
    *  Argument          Type            IO  Description
    *  ----------        ----------          --  -------------------------------------------
    *  d_dma_channel_number  T_DMA_TYPE_CHANNEL_NUMBER  I  Variable to define which channel is disabled.
    *
    *
    *
    * RETURN VALUE: None
    *
    *****************************************************************************/
    void f_dma_channel_disable(T_DMA_TYPE_CHANNEL_NUMBER d_dma_channel_number)  
      {
      F_DMA_CHANNEL_DISABLE(d_dma_channel_number);
      } /*f_dma_channel_disable() */

    /******************************************************************************
    *
    *  FUNCTION NAME: f_dma_channel_auto_init_disable
    *    This function is disabling the auto-initialization mode of the channel.
    *    The channel completes the current transfer and stops.
    *
    *  ARGUMENT LIST:
    *
    *  Argument          Type            IO  Description
    *  ----------        ----------          --  -------------------------------------------
    *  d_dma_channel_number  T_DMA_TYPE_CHANNEL_NUMBER  I  Variable to define which channel is disabled from auto-init mode.
    *
    *
    * RETURN VALUE: None
    *
    *****************************************************************************/
    void f_dma_channel_auto_init_disable(T_DMA_TYPE_CHANNEL_NUMBER d_dma_channel_number)
      {
      F_DMA_CHANNEL_AUTO_INIT_DISABLE(d_dma_channel_number);
      } /* f_dma_channel_auto_init_disable */

    /******************************************************************************
    *
    *  FUNCTION NAME: f_dma_get_channel_counter
    *      This function can be used to monitor address
    *    where the last data of a frame has been written.
    *    This function provides only the lowest16 bits of the address.
    *
    *
    *  ARGUMENT LIST:
    *
    *  Argument           Type              IO  Description
    *  ----------         ----------            --  -------------------------------------------
    *  d_dma_channel_number  T_DMA_TYPE_CHANNEL_NUMBER    I    Variable to define from which channel, we get the counter.    
    *
    * RETURN VALUE: None
    *
    *****************************************************************************/
    SYS_UWORD16  f_dma_get_channel_counter(T_DMA_TYPE_CHANNEL_NUMBER d_dma_channel_number)
        {
        return(F_DMA_GET_CHANNEL_COUNTER(d_dma_channel_number));
        } /*f_dma_get_channel_counter() */

    /******************************************************************************
    *
    *  FUNCTION NAME: f_dma_channel_soft_reset
    *      perform a soft reset of the following parameter of the channel :
    *      Source address,Destination address,Element number,Frame number,Channel counter
    *  
    * 
    *
    *  ARGUMENT LIST:
    *
    *  Argument          Type              IO  Description
    *  ----------        ----------            --  -------------------------------------------
    *  d_dma_channel_number       T_DMA_TYPE_CHANNEL_NUMBER    I   Variable to define which channel is soft reset.
    *  
    *
    * RETURN VALUE: None
    *
    *****************************************************************************/
    void f_dma_channel_soft_reset(T_DMA_TYPE_CHANNEL_NUMBER d_dma_channel_number)
       {
       C_DMA_SRR_REG |= ( 0x1 << d_dma_channel_number);

       while (C_DMA_SRR_REG & ( 0x1 << d_dma_channel_number))   /* wait for the reset is completed */
            {
            }

       } /*f_dma_channel_soft_reset() */
  #endif /* (CHIPSET == 12) */
