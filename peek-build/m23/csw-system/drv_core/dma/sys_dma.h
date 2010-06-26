/*                @(#) nom : sys_dma.h SID: 1.2 date : 05/23/03               */
/* Filename:      sys_dma.h                                                   */
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
 *  FILE NAME: sys_dma.h
 *
 *
 *  PURPOSE:  Include file to use the DMA drivers for CALYPSO PLUS.
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
 *  23-Oct-2002  Francois AMAND  0.0.1    First implementation
 *  23-Oct-2002  G.Leterrier     0.0.2    Type, prototype definition
 *   6-Dec-2002  G.Leterrier     0.0.3    remove base address
 *  24-Feb-2003  G.Leterrier     0.0.4    change base address include name
 *  ALGORITHM: 
 *  ALGORITHM: 
 *
 *
 *****************************************************************************/


#ifndef __SYS_DMA_H__
  #define __SYS_DMA_H__

  #include "chipset.cfg" 

  #if (CHIPSET == 12 || CHIPSET == 15)

    #include "sys_types.h"

    #include "sys_map.h"
  
    /****************************************************************************
     *                            CONSTANT DEFINITION
     ***************************************************************************/
 

/* create the constants for the EMIF priority here */
#if (CHIPSET == 15)
#define  EMIF_PRIO_REG			* (volatile SYS_UWORD16 *) 0xFFFFFB00 

#define  EMIF_PRIO_DMA_MASK     4
#define  EMIF_PRIO_MCU_MASK     0
#endif

    #define C_DMA_VERSION_DRIVER                       0x0001

    #define C_DMA_NUMBER_OF_CHANNEL                   6
    
    /*
     *  Registers offset definition
     */

    #define C_DMA_CHANNEL_SELECT_POS                  6

    /* Global registers */
    #define C_DMA_GCR_OFFSET                          0x0400
    #define C_DMA_ISR_OFFSET                          0x0402
    #define C_DMA_CAR_OFFSET                          0x0404
    #define C_DMA_SCR_OFFSET                          0x0406
    #define C_DMA_SRR_OFFSET                          0x0408
    #define C_DMA_AR_OFFSET                           0x040A
    
    /* Channel registers */
    #define C_DMA_CSDP_OFFSET(d_channel_index)        (0x0000 + (d_channel_index << C_DMA_CHANNEL_SELECT_POS))
    #define C_DMA_CCR_OFFSET(d_channel_index)         (0x0002 + (d_channel_index << C_DMA_CHANNEL_SELECT_POS))
    #define C_DMA_CICR_OFFSET(d_channel_index)        (0x0004 + (d_channel_index << C_DMA_CHANNEL_SELECT_POS))
    #define C_DMA_CSR_OFFSET(d_channel_index)         (0x0006 + (d_channel_index << C_DMA_CHANNEL_SELECT_POS))
    #define C_DMA_CSSA_L_OFFSET(d_channel_index)      (0x0008 + (d_channel_index << C_DMA_CHANNEL_SELECT_POS))
    #define C_DMA_CSSA_U_OFFSET(d_channel_index)      (0x000A + (d_channel_index << C_DMA_CHANNEL_SELECT_POS))
    #define C_DMA_CDSA_L_OFFSET(d_channel_index)      (0x000C + (d_channel_index << C_DMA_CHANNEL_SELECT_POS))
    #define C_DMA_CDSA_U_OFFSET(d_channel_index)      (0x000E + (d_channel_index << C_DMA_CHANNEL_SELECT_POS))
    #define C_DMA_CEN_OFFSET(d_channel_index)         (0x0010 + (d_channel_index << C_DMA_CHANNEL_SELECT_POS))
    #define C_DMA_CFN_OFFSET(d_channel_index)         (0x0012 + (d_channel_index << C_DMA_CHANNEL_SELECT_POS))
    #define C_DMA_CPC_OFFSET(d_channel_index)         (0x0018 + (d_channel_index << C_DMA_CHANNEL_SELECT_POS))


    /*
     *  Registers address definition
     */
    /* Global registers */
    #define C_DMA_GCR_REG                             * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_GCR_OFFSET)
    #define C_DMA_ISR_REG                             * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_ISR_OFFSET)
    #define C_DMA_CAR_REG                             * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_CAR_OFFSET)
    #define C_DMA_SCR_REG                             * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_SCR_OFFSET)
    #define C_DMA_SRR_REG                             * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_SRR_OFFSET)
    #define C_DMA_AR_REG                              * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_AR_OFFSET)

    /* Channel registers */
    #define C_DMA_CSDP_REG(d_channel_index)           * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_CSDP_OFFSET(d_channel_index))
    #define C_DMA_CCR_REG(d_channel_index)            * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_CCR_OFFSET(d_channel_index))
    #define C_DMA_CICR_REG(d_channel_index)           * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_CICR_OFFSET(d_channel_index))
    #define C_DMA_CSR_REG(d_channel_index)            * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_CSR_OFFSET(d_channel_index))
    #define C_DMA_CSSA_L_REG(d_channel_index)         * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_CSSA_L_OFFSET(d_channel_index))
    #define C_DMA_CSSA_U_REG(d_channel_index)         * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_CSSA_U_OFFSET(d_channel_index))
    #define C_DMA_CDSA_L_REG(d_channel_index)         * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_CDSA_L_OFFSET(d_channel_index))
    #define C_DMA_CDSA_U_REG(d_channel_index)         * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_CDSA_U_OFFSET(d_channel_index))
    #define C_DMA_CEN_REG(d_channel_index)            * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_CEN_OFFSET(d_channel_index))
    #define C_DMA_CFN_REG(d_channel_index)            * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_CFN_OFFSET(d_channel_index))
    #define C_DMA_CPC_REG(d_channel_index)            * (volatile SYS_UWORD16 *) (C_MAP_DMA_BASE + C_DMA_CPC_OFFSET(d_channel_index))
  
  
  
    /*
     *  DMA_GCR register definition
     */
    /* Bits position in the register */
    #define C_DMA_GCR_FREE_POS                        2
    #define C_DMA_GCR_AUTO_GATE_POS                   3
    
    /* Mask of the field in the register */
    #define C_DMA_GCR_FREE_MASK                       0x0001
    #define C_DMA_GCR_AUTO_GATE_MASK                  0x0001
  
   
   
    /*
     *  DMA_AR register definition
     */
    /* Bits position in the register */
    #define C_DMA_AR_IMIF_PRIO_POS                    0
    #define C_DMA_AR_RHEA_PRIO_POS                    3
    #define C_DMA_AR_API_PRIO_POS                     4
#if (CHIPSET == 15)
	#define C_DMA_AR_IPERIF_PRIO_POS				  5
#endif
    
    /* Mask of the field in the register */
#if (CHIPSET == 15)
    #define C_DMA_AR_IMIF_PRIO_MASK                   0x0007
#endif
    #define C_DMA_AR_IMIF_PRIO_MASK                   0x0007
    #define C_DMA_AR_RHEA_PRIO_MASK                   0x0001
    #define C_DMA_AR_API_PRIO_MASK                    0x0001
    

    /*
     *  DMA_CSDP register definition
     */
    /* Bits position in the register */
    #define C_DMA_CSDP_DATA_TYPE_POS                  0
    #define C_DMA_CSDP_SRC_POS                        2
    #define C_DMA_CSDP_SRC_PACK_POS                   6
    #define C_DMA_CSDP_SRC_BURST_EN_POS               7
    #define C_DMA_CSDP_DST_POS                        9
    #define C_DMA_CSDP_DST_PACK_POS                   13
    #define C_DMA_CSDP_DST_BURST_EN_POS               14
    
    /* Mask of the field in the register */
    #define C_DMA_CSDP_DATA_TYPE_MASK                 0x0003
    #define C_DMA_CSDP_SRC_MASK                       0x000F
    #define C_DMA_CSDP_SRC_PACK_MASK                  0x0001
    #define C_DMA_CSDP_SRC_BURST_EN_MASK              0x0003
    #define C_DMA_CSDP_DST_MASK                       0x000F
    #define C_DMA_CSDP_DST_PACK_MASK                  0x0001
    #define C_DMA_CSDP_DST_BURST_EN_MASK              0x0003


    /*
     *  DMA_CCR register definition
     */
    /* Bits position in the register */
    #define C_DMA_CCR_SYNC_POS                        0
    #define C_DMA_CCR_PRIO_POS                        6
    #define C_DMA_CCR_EN_POS                          7
    #define C_DMA_CCR_AUTO_INIT_POS                   8
    #define C_DMA_CCR_FIFO_FLUSH_POS                  10
    #define C_DMA_CCR_SRC_AMODE_POS                   12
    #define C_DMA_CCR_DST_AMODE_POS                   14
    
    /* Mask of the field in the register */
    #define C_DMA_CCR_SYNC_MASK                       0x001F
    #define C_DMA_CCR_PRIO_MASK                       0x0001
    #define C_DMA_CCR_EN_MASK                         0x0001
    #define C_DMA_CCR_AUTO_INIT_MASK                  0x0001
    #define C_DMA_CCR_FIFO_FLUSH_MASK                 0x0001
    #define C_DMA_CCR_SRC_AMODE_MASK                  0x0003
    #define C_DMA_CCR_DST_AMODE_MASK                  0x0003


    /*
     *  DMA_CICR register definition
     */
    /* Bits position in the register */
    #define C_DMA_CICR_TOUT_IE_POS                    0
    #define C_DMA_CICR_DROP_IE_POS                    1
    #define C_DMA_CICR_FRAME_IE_POS                   3
    #define C_DMA_CICR_BLOCK_IE_POS                   5
    #define C_DMA_CICR_HALF_BLOCK_IE_POS              6
    
    /* Mask of the field in the register */
    #define C_DMA_CICR_TOUT_IE_MASK                   0x0001
    #define C_DMA_CICR_DROP_IE_MASK                   0x0001
    #define C_DMA_CICR_FRAME_IE_MASK                  0x0001
    #define C_DMA_CICR_BLOCK_IE_MASK                  0x0001
    #define C_DMA_CICR_HALF_BLOCK_IE_MASK             0x0001


    /*
     *  DMA_CSR register definition
     */
    /* Bits position in the register */
    #define C_DMA_CSR_TOUT_POS                        0
    #define C_DMA_CSR_DROP_POS                        1
    #define C_DMA_CSR_FRAME_POS                       3
    #define C_DMA_CSR_BLOCK_POS                       5
    #define C_DMA_CSR_SYNC_POS                        6
    #define C_DMA_CSR_HALF_BLOCK_POS                  7
    #define C_DMA_CSR_TOUT_SRC_NDST_POS                8
    
    /* Mask of the field in the register */
    #define C_DMA_CSR_TOUT_MASK                       0x0001
    #define C_DMA_CSR_DROP_MASK                       0x0001
    #define C_DMA_CSR_FRAME_MASK                      0x0001
    #define C_DMA_CSR_BLOCK_MASK                      0x0001
    #define C_DMA_CSR_SYNC_MASK                       0x0001
    #define C_DMA_CSR_HALF_BLOCK_MASK                 0x0001
    #define C_DMA_CSR_TOUT_SRC_NDST_MASK              0x0001



    /****************************************************************************
     *                            TYPE DEFINITION
     ***************************************************************************/

    /*
    *  Channel number definition
    */
    typedef  SYS_UWORD8  T_DMA_TYPE_CHANNEL_NUMBER;
    #define   C_DMA_CHANNEL_0      0
    #define   C_DMA_CHANNEL_1      1
    #define   C_DMA_CHANNEL_2      2
    #define   C_DMA_CHANNEL_3      3
    #define   C_DMA_CHANNEL_4      4
    #define   C_DMA_CHANNEL_5      5



    #define   C_DMA_CHANNEL_0_MASK              0x01
    #define   C_DMA_CHANNEL_1_MASK              0x02
    #define   C_DMA_CHANNEL_2_MASK              0x04
    #define   C_DMA_CHANNEL_3_MASK              0x08
    #define   C_DMA_CHANNEL_4_MASK              0x10
    #define   C_DMA_CHANNEL_5_MASK              0x20


    /*
    *  Channel secure parameter definition
    */
    /*  channel secured, must be used with crypto module  */

    typedef  SYS_UWORD8  T_DMA_TYPE_CHANNEL_SECURED;
    #define    C_DMA_CHANNEL_NOT_SECURED    0    
    #define    C_DMA_CHANNEL_SECURED        1    

    /*
    *  Channel data type definition
    */

    typedef SYS_UWORD8  T_DMA_TYPE_CHANNEL_DATA_TYPE;
    #define    C_DMA_DATA_S8      0        /* byte definition */
    #define    C_DMA_DATA_S16     1        /* half word definition */
    #define    C_DMA_DATA_S32      2        /* word definition */



    /*
    *  Port definition
    */


    typedef SYS_UWORD8      T_DMA_TYPE_CHANNEL_PORT;    
    #define    C_DMA_IMIF_PORT      0        /*  IMIF PORT definition */
    #define    C_DMA_RHEA_PORT      1        /*  RHEA PORT definition */
    #define    C_DMA_API_PORT       2        /*  API PORT definition  */
#if (CHIPSET == 15)
    #define    C_DMA_IPHERIF_PORT   3		 /*  IPERIF PORT definition */
    #define    C_DMA_EMIF_PORT      4        /*  EMIF PORT definition */
#endif


    /*
    *  port packing definition
    */
    /*   port is performing packing to increase bandwidth if possible */

    typedef SYS_UWORD8  T_DMA_TYPE_CHANNEL_PACKED;
    #define    C_DMA_CHANNEL_NOT_PACKED  0      
    #define    C_DMA_CHANNEL_PACKED      1      


    /*
    *  port transfer busrt/single definition
    */


    typedef SYS_UWORD8  T_DMA_TYPE_CHANNEL_BURST_EN;
    #define    C_DMA_CHANNEL_SINGLE    0      /*  port transfer Single */
    #define    C_DMA_CHANNEL_BURST4    2      /*  port transfer burst 4 bytes, only IMIF can manage it */
  

    /*
    *  Channel synchronisation definition
    */
#if (CHIPSET == 12)
    typedef SYS_UWORD8  T_DMA_TYPE_CHANNEL_HW_SYNCH;
    #define    C_DMA_CHANNEL_NO_SYNCH        0      /*  No Synch means software synchronisation */
    #define    C_DMA_CHANNEL_RIF_TX          1
    #define    C_DMA_CHANNEL_RIF_RX          2
    #define    C_DMA_CHANNEL_LCD              3
    #define    C_DMA_CHANNEL_UART_MODEM1_TX  4
    #define    C_DMA_CHANNEL_UART_MODEM1_RX  5
    #define    C_DMA_CHANNEL_UART_MODEM2_TX  6
    #define    C_DMA_CHANNEL_UART_MODEM2_RX  7
    #define    C_DMA_CHANNEL_UART_IRDA_TX    8
    #define    C_DMA_CHANNEL_UART_IRDA_RX    9 
    #define    C_DMA_CHANNEL_USB_RX1          10
    #define    C_DMA_CHANNEL_USB_TX1          11
    #define    C_DMA_CHANNEL_USB_RX2          12
    #define    C_DMA_CHANNEL_USB_TX2          13
    #define    C_DMA_CHANNEL_USB_RX3          14
    #define    C_DMA_CHANNEL_USB_TX3          15
    #define    C_DMA_CHANNEL_MMC_SD_RX        16
    #define    C_DMA_CHANNEL_MMC_SD_TX        17
    #define    C_DMA_CHANNEL_MS_RX_TX         18
    #define    C_DMA_CHANNEL_USIM_RX          19
    #define    C_DMA_CHANNEL_USIM_TX          20
    #define    C_DMA_CHANNEL_UWIRE_RX_TX      23
    #define    C_DMA_CHANNEL_NAND_FLASH      24
    #define    C_DMA_CHANNEL_I2C_RX          25
    #define    C_DMA_CHANNEL_I2C_TX          26
    #define    C_DMA_CHANNEL_SHA1_TX         27    // channel must be set as secured channel
    #define    C_DMA_CHANNEL_DES_RX          28    // channel must be set as secured channel
    #define    C_DMA_CHANNEL_DES_TX          29    // channel must be set as secured channel
    #define    C_DMA_CHANNEL_CPORT_RX        30
    #define    C_DMA_CHANNEL_CPORT_TX        31
#elif (CHIPSET == 15)
    typedef SYS_UWORD8  T_DMA_TYPE_CHANNEL_HW_SYNCH;
    #define    C_DMA_CHANNEL_NO_SYNCH        0      /*  No Synch means software synchronisation */
    #define    C_DMA_CHANNEL_DRP_TX              1
    #define    C_DMA_CHANNEL_DRP_RX              2
    #define    C_DMA_CHANNEL_LCD                 3
    #define    C_DMA_CHANNEL_UART_IRDA_MODEM_TX  4
    #define    C_DMA_CHANNEL_UART_IRDA_MODEM_RX  5
    #define    C_DMA_CHANNEL_UART_MSSPI_TX       6
    #define    C_DMA_CHANNEL_UART_MSSPI_RX       7
    #define    C_DMA_CHANNEL_USB_RX1			    10
    #define    C_DMA_CHANNEL_USB_TX1             11
    #define    C_DMA_CHANNEL_USB_RX2             12
    #define    C_DMA_CHANNEL_USB_TX2             13
    #define    C_DMA_CHANNEL_USB_RX3             14
    #define    C_DMA_CHANNEL_USB_TX3             15
    #define    C_DMA_CHANNEL_I2C_TRITON_RX       16
    #define    C_DMA_CHANNEL_I2C_TRITON_TX       17
    #define    C_DMA_CHANNEL_USIM_RX             19
    #define    C_DMA_CHANNEL_USIM_TX             20
    #define    C_DMA_CHANNEL_CAM_THRESHOLD       21
    #define    C_DMA_CHANNEL_NAND_FLASH          24
    #define    C_DMA_CHANNEL_I2C_RX              25
    #define    C_DMA_CHANNEL_I2C_TX              26
    #define    C_DMA_CHANNEL_SHA1_TX             27    // channel must be set as secured channel
    #define    C_DMA_CHANNEL_DES_RX              28    // channel must be set as secured channel
    #define    C_DMA_CHANNEL_DES_TX              29    // channel must be set as secured channel
    #define    C_DMA_CHANNEL_CPORT_RX            30
    #define    C_DMA_CHANNEL_CPORT_TX            31

#endif
    /*
    *  Channel priority definition
    */

    typedef SYS_UWORD8  T_DMA_TYPE_CHANNEL_PRIORITY;
    #define    C_DMA_CHANNEL_PRIORITY_LOW     0    /* channel priority low */
    #define    C_DMA_CHANNEL_PRIORITY_HIGH    1    /* channel priority high */


    /*
    *  Channel auto-initialisation definition
    */
    /* channel auto-init on, transfer restart on next HW request */

    typedef SYS_UWORD8  T_DMA_TYPE_CHANNEL_AUTO_INIT;
    #define   C_DMA_CHANNEL_AUTO_INIT_OFF    0  
    #define   C_DMA_CHANNEL_AUTO_INIT_ON     1    


    /*
    *  Channel fifo-flush control
    */


    typedef SYS_UWORD8  T_DMA_TYPE_CHANNEL_FIFO_FLUSH;
    #define   C_DMA_CHANNEL_FIFO_FLUSH_OFF     0  /*  nothing happens on fifo  */
    #define   C_DMA_CHANNEL_FIFO_FLUSH_ON      1  /*  fifo is flushed  */

    /*
    *  definition addressng mode of source/destination port
    */

    typedef SYS_UWORD8  T_DMA_TYPE_CHANNEL_ADDR_MODE;
    #define    C_DMA_CHANNEL_ADDR_MODE_CONSTANT    0  /*  constant address */
    #define    C_DMA_CHANNEL_ADDR_MODE_POST_INC    1  /* post-increment address */
    #define    C_DMA_CHANNEL_ADDR_MODE_FRAME_INDEX    2  /* frame indexed address */


    /*
    *  Channel interrupt event definition
    */


    typedef SYS_UWORD8  T_DMA_TYPE_CHANNEL_IT_TIME_OUT;
    #define    C_DMA_CHANNEL_IT_TIME_OUT_OFF      0  
    #define    C_DMA_CHANNEL_IT_TIME_OUT_ON       1  /* time out event on a port*/


    /* drop event if new HW request issued, before completion of previous one */
    typedef SYS_UWORD8  T_DMA_TYPE_CHANNEL_IT_DROP;  
    #define    C_DMA_CHANNEL_IT_DROP_OFF      0    
    #define    C_DMA_CHANNEL_IT_DROP_ON       1  


    /*  frame interrupt event */
    typedef SYS_UWORD8  T_DMA_TYPE_CHANNEL_IT_FRAME;
    #define    C_DMA_CHANNEL_IT_FRAME_OFF      0      
    #define    C_DMA_CHANNEL_IT_FRAME_ON       1  


    /* end of block transfer event */
    typedef SYS_UWORD8  T_DMA_TYPE_CHANNEL_IT_BLOCK;
    #define    C_DMA_CHANNEL_IT_BLOCK_OFF      0  
    #define    C_DMA_CHANNEL_IT_BLOCK_ON       1


    /* half of block transfer event */
    typedef SYS_UWORD8  T_DMA_TYPE_CHANNEL_IT_HALF_BLOCK;  
    #define    C_DMA_CHANNEL_IT_HALF_BLOCK_OFF    0
    #define    C_DMA_CHANNEL_IT_HALF_BLOCK_ON     1


    /*
    *  DMA Definition of channel enable/disable
    */  

    typedef  SYS_UWORD8  T_DMA_TYPE_CHANNEL_ENABLE;
    #define    C_DMA_CHANNEL_ENABLE_OFF       0      
    #define    C_DMA_CHANNEL_ENABLE_ON        1    




    /*  
    *  DMA Definition of free mode for emulation
    */

    typedef  SYS_UWORD8  T_DMA_TYPE_FREE;
    #define    C_DMA_FREE_OFF         0      
    #define    C_DMA_FREE_ON          1    

    /* In FREE_ON , on-going transfers are not stopped
    when DMA receives the "suspend" signal from the processor */



    /*
    *  DMA Definition of auto-gating clock
    */

    typedef  SYS_UWORD8  T_DMA_TYPE_AUTO_GATE;
    #define    C_DMA_AUTO_GATE_OFF        0    /*  DMA clock always ON */
    #define    C_DMA_AUTO_GATE_ON         1    /*  DMA cut off clock according to activity */


    /*
    *  priority definition on API port
    */


    typedef  SYS_UWORD8  T_DMA_TYPE_API_PRIO;
    #define   C_DMA_API_PRIO_DMA         0    /* DMA has priority */
    #define    C_DMA_API_PRIO_ARM        1    /* ARM has priority */


    /*
    *  priority definition on RHEA port
    */    


    typedef  SYS_UWORD8  T_DMA_TYPE_RHEA_PRIO;
    #define    C_DMA_RHEA_PRIO_DMA        0    /* DMA has priority */
    #define    C_DMA_RHEA_PRIO_ARM        1    /* ARM has priority */




    /*
    *  priority definition on IMIF port
    */

    /*  defines the number of consecutive cycles that are allocates to the CPU before DMA steals one cycle*/

    typedef  SYS_UWORD8  T_DMA_TYPE_IMIF_PRIO;
    #define    C_DMA_IMIF_PRIO_CPU_0      0    
    #define    C_DMA_IMIF_PRIO_CPU_1      1
    #define    C_DMA_IMIF_PRIO_CPU_2      2
    #define    C_DMA_IMIF_PRIO_CPU_3      3
    #define    C_DMA_IMIF_PRIO_CPU_4      4
    #define    C_DMA_IMIF_PRIO_CPU_5      5
    #define    C_DMA_IMIF_PRIO_CPU_6      6
    #define    C_DMA_IMIF_PRIO_CPU_7      7

#if (CHIPSET == 15)
	/*
    *  priority definition on IPERIF port
    */

    /*  defines the number of consecutive cycles that are allocates to the CPU before DMA steals one cycle*/
    typedef SYS_UWORD8 T_DMA_TYPE_IPERIF_PRIO;
	#define    C_DMA_IPERIF_PRIO_CPU_0      0    
    #define    C_DMA_IPERIF_PRIO_CPU_1      1
    #define    C_DMA_IPERIF_PRIO_CPU_2      2
    #define    C_DMA_IPERIF_PRIO_CPU_3      3
    #define    C_DMA_IPERIF_PRIO_CPU_4      4
    #define    C_DMA_IPERIF_PRIO_CPU_5      5
    #define    C_DMA_IPERIF_PRIO_CPU_6      6
    #define    C_DMA_IPERIF_PRIO_CPU_7      7

	/* 
	* priority definition on EMIF DMA 
	*/
	typedef SYS_UWORD16 T_DMA_TYPE_EMIF_DMA_PRIO;
    #define C_DMA_EMIF_DMA_PRIO_0			0
	#define C_DMA_EMIF_DMA_PRIO_1			1
	#define C_DMA_EMIF_DMA_PRIO_2			2
	#define C_DMA_EMIF_DMA_PRIO_3			3
	#define C_DMA_EMIF_DMA_PRIO_4			4
	#define C_DMA_EMIF_DMA_PRIO_5			5
	#define C_DMA_EMIF_DMA_PRIO_6			6
	#define C_DMA_EMIF_DMA_PRIO_7			7
	#define C_DMA_EMIF_DMA_PRIO_8			8
	#define C_DMA_EMIF_DMA_PRIO_9			9
	#define C_DMA_EMIF_DMA_PRIO_10			10
	#define C_DMA_EMIF_DMA_PRIO_11			11
	#define C_DMA_EMIF_DMA_PRIO_12			12
	#define C_DMA_EMIF_DMA_PRIO_13			13
	#define C_DMA_EMIF_DMA_PRIO_14			14
	#define C_DMA_EMIF_DMA_PRIO_15			15
    
	/* 
	* priority definition on EMIF MCU
	*/

	typedef SYS_UWORD8 T_DMA_TYPE_EMIF_MCU_PRIO;
	#define    C_DMA_EMIF_MCU_PRIO_0      0    
    #define    C_DMA_EMIF_MCU_PRIO_1      1
    #define    C_DMA_EMIF_MCU_PRIO_2      2
    #define    C_DMA_EMIF_MCU_PRIO_3      3
    #define    C_DMA_EMIF_MCU_PRIO_4      4
    #define    C_DMA_EMIF_MCU_PRIO_5      5
    #define    C_DMA_EMIF_MCU_PRIO_6      6
    #define    C_DMA_EMIF_MCU_PRIO_7      7

#endif

    /*
    *  Definition of channel allocation between the DSP and ARM
    */


    typedef  SYS_UWORD8  T_DMA_TYPE_CHANNEL_ALLOCATION;
    #define    C_DMA_CHANNEL_ARM        0      /* channel reserved to ARM */
    #define    C_DMA_CHANNEL_DSP        1      /* channel reserved to DSP */


    /*
    * Type definition of pointer for call back function
    */


    typedef  void (*T_DMA_CALL_BACK) (SYS_UWORD16);

    /****************************************************************************
     *                            MACRO DEFINITION
     ***************************************************************************/
  


    /* enable free mode of dma */
    #define F_DMA_SUSPEND_MODE_EMULATION_FREE_ENABLE    C_DMA_GCR_REG |= (C_DMA_FREE_ON << C_DMA_GCR_FREE_POS )



    /*disable free mode of the dma */
    #define F_DMA_SUSPEND_MODE_EMULATION_FREE_DISABLE   C_DMA_GCR_REG &= ~( C_DMA_GCR_FREE_MASK << C_DMA_GCR_FREE_POS )


    /*enable channel transfer*/
    #define F_DMA_CHANNEL_ENABLE(d_dma_channel_number)  C_DMA_CCR_REG(d_dma_channel_number)  |= (C_DMA_CHANNEL_ENABLE_ON   << C_DMA_CCR_EN_POS )

 
    /*disable channel transfer*/ 
    #define F_DMA_CHANNEL_DISABLE(d_dma_channel_number)  C_DMA_CCR_REG(d_dma_channel_number)  &= ~(C_DMA_CCR_EN_MASK    << C_DMA_CCR_EN_POS )


    /*disable channel auto-initialisation mode*/
    #define F_DMA_CHANNEL_AUTO_INIT_DISABLE(d_dma_channel_number)   C_DMA_CCR_REG(d_dma_channel_number)  &= ~(C_DMA_CCR_AUTO_INIT_MASK    << C_DMA_CCR_AUTO_INIT_POS  )


    /* get channel counter */
    #define F_DMA_GET_CHANNEL_COUNTER(d_dma_channel_number)      C_DMA_CPC_REG(d_dma_channel_number) 


    /* get channel interrupt status, cleared after read. must be saved in a variable if re-use*/
    #define F_DMA_GET_CHANNEL_IT_STATUS(d_dma_channel_number)    C_DMA_CSR_REG(d_dma_channel_number)


    /* compare if interruption status is time_out event on source */
    #define F_DMA_COMPARE_CHANNEL_IT_STATUS_TIME_OUT_SRC(d_dma_status_it)   ((C_DMA_CSR_TOUT_MASK & (d_dma_status_it >>C_DMA_CSR_TOUT_POS)) & (C_DMA_CSR_TOUT_SRC_NDST_MASK & (d_dma_status_it >>C_DMA_CSR_TOUT_SRC_NDST_POS)))


    /* compare if interruption status is time_out event on destination */
    #define F_DMA_COMPARE_CHANNEL_IT_STATUS_TIME_OUT_DST(d_dma_status_it)   ((C_DMA_CSR_TOUT_MASK & (d_dma_status_it >>C_DMA_CSR_TOUT_POS)) & ( C_DMA_CSR_TOUT_SRC_NDST_MASK & ((~d_dma_status_it) >>C_DMA_CSR_TOUT_SRC_NDST_POS)))



    /* compare if interruption status is drop event ( new hw event, before completion of the previous one */
    #define F_DMA_COMPARE_CHANNEL_IT_STATUS_DROP(d_dma_status_it)   (C_DMA_CSR_DROP_MASK & (d_dma_status_it >>C_DMA_CSR_DROP_POS))



    /* compare if interruption status is an end of frame transfer event*/
    #define F_DMA_COMPARE_CHANNEL_IT_STATUS_FRAME(d_dma_status_it)   (C_DMA_CSR_FRAME_MASK & (d_dma_status_it >>C_DMA_CSR_FRAME_POS))



    /* compare if interruption status is an end of block transfer event*/
    #define F_DMA_COMPARE_CHANNEL_IT_STATUS_BLOCK(d_dma_status_it)   (C_DMA_CSR_BLOCK_MASK & (d_dma_status_it >>C_DMA_CSR_BLOCK_POS))



    /* compare if interruption status is an end of half_block transfer event*/
    #define F_DMA_COMPARE_CHANNEL_IT_STATUS_HALF_BLOCK(d_dma_status_it)   (C_DMA_CSR_HALF_BLOCK_MASK & (d_dma_status_it >>C_DMA_CSR_HALF_BLOCK_POS))


    /****************************************************************************
     *                            STRUCTURE DEFINITION
     ***************************************************************************/
  


    /* definition of parameter structure for global configuration */

    typedef struct 
        {
        T_DMA_TYPE_AUTO_GATE    d_dma_global_auto_gate;
        T_DMA_TYPE_API_PRIO     d_dma_global_api_prio;
        T_DMA_TYPE_RHEA_PRIO    d_dma_global_rhea_prio;
        T_DMA_TYPE_IMIF_PRIO    d_dma_global_imif_prio;
#if (CHIPSET == 15)
        T_DMA_TYPE_IPERIF_PRIO  d_dma_global_iperif_prio;
		T_DMA_TYPE_EMIF_DMA_PRIO d_dma_global_emif_dma_prio;
		T_DMA_TYPE_EMIF_MCU_PRIO d_dma_global_emif_mcu_prio;
#endif
        }
        T_DMA_TYPE_GLOBAL_PARAMETER;


    /* definition of parameter structure for channel configuration */


    typedef struct  
       
      {
      T_DMA_CALL_BACK               pf_dma_call_back_address;
      //void (*pf_dma_call_back_address) (SYS_UWORD16);      /* call back function address for interrupt */


      T_DMA_TYPE_CHANNEL_NUMBER       d_dma_channel_number;    /* channel parameter */
      T_DMA_TYPE_CHANNEL_SECURED      d_dma_channel_secured;
      T_DMA_TYPE_CHANNEL_DATA_TYPE    d_dma_channel_data_type;  /* type of element to be transfered */

      T_DMA_TYPE_CHANNEL_PORT        d_dma_channel_src_port;    /* source port parameter */
      T_DMA_TYPE_CHANNEL_PACKED      d_dma_src_channel_packed;
      T_DMA_TYPE_CHANNEL_BURST_EN    d_dma_src_channel_burst_en;

      T_DMA_TYPE_CHANNEL_PORT        d_dma_channel_dst_port;    /* destination port parameter */
      T_DMA_TYPE_CHANNEL_PACKED      d_dma_dst_channel_packed;
      T_DMA_TYPE_CHANNEL_BURST_EN    d_dma_dst_channel_burst_en;

      T_DMA_TYPE_CHANNEL_HW_SYNCH      d_dma_channel_hw_synch;    /* channel parameter */
      T_DMA_TYPE_CHANNEL_PRIORITY      d_dma_channel_priority;
      T_DMA_TYPE_CHANNEL_AUTO_INIT     d_dma_channel_auto_init;
      T_DMA_TYPE_CHANNEL_FIFO_FLUSH    d_dma_channel_fifo_flush;

      T_DMA_TYPE_CHANNEL_ADDR_MODE    d_dma_src_channel_addr_mode;  /* source addressing mode */
      T_DMA_TYPE_CHANNEL_ADDR_MODE    d_dma_dst_channel_addr_mode;  /* destination addressinf mode */

      T_DMA_TYPE_CHANNEL_IT_TIME_OUT   d_dma_channel_it_time_out;    /* channel interrupt setting */
      T_DMA_TYPE_CHANNEL_IT_DROP       d_dma_channel_it_drop;  
      T_DMA_TYPE_CHANNEL_IT_FRAME      d_dma_channel_it_frame;
      T_DMA_TYPE_CHANNEL_IT_BLOCK      d_dma_channel_it_block;  
      T_DMA_TYPE_CHANNEL_IT_HALF_BLOCK d_dma_channel_it_half_block;

      SYS_UWORD32              d_dma_channel_src_address;    /* source address */
      SYS_UWORD32              d_dma_channel_dst_address;    /* destination address */

      SYS_UWORD16              d_dma_channel_element_number;  /* number of element per frame */
      SYS_UWORD16              d_dma_channel_frame_number;    /* number of frame to be transfered */
      }
    T_DMA_TYPE_CHANNEL_PARAMETER;




    /****************************************************************************
     *                            PROTOTYPE DEFINITION
     ***************************************************************************/

    SYS_UWORD16 f_dma_get_version_driver(void);



    void f_dma_global_parameter_set(T_DMA_TYPE_GLOBAL_PARAMETER  *p_dma_global_parameter);



    void f_dma_channel_allocation_set(T_DMA_TYPE_CHANNEL_NUMBER d_dma_channel_number, T_DMA_TYPE_CHANNEL_ALLOCATION d_dma_channel_allocation);



    void f_dma_channel_parameter_set(T_DMA_TYPE_CHANNEL_PARAMETER    *p_dma_channel_parameter);



    void f_dma_channel_enable (T_DMA_TYPE_CHANNEL_NUMBER d_dma_channel_number);



    void f_dma_channel_disable(T_DMA_TYPE_CHANNEL_NUMBER d_dma_channel_number);



    void f_dma_channel_auto_init_disable(T_DMA_TYPE_CHANNEL_NUMBER d_dma_channel_number);


    SYS_UWORD16  f_dma_get_channel_counter(T_DMA_TYPE_CHANNEL_NUMBER d_dma_channel_number);



    void f_dma_channel_soft_reset(T_DMA_TYPE_CHANNEL_NUMBER d_dma_channel_number);


    void f_dma_default_call_back_it(SYS_UWORD16 d_dma_channel_it_status);


    void f_dma_interrupt_manager();

    void f_dma_secure_interrupt_manager();

  #endif /* (CHIPSET == 12) */

#endif /* __SYS_DMA_H__ */
