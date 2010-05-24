/*                @(#) nom : sys_conf.h SID: 1.3 date : 05/23/03              */
/* Filename:      sys_conf.h                                                  */
/* Version:       1.3                                                         */
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
 *  FILE NAME: sys_conf.h
 *
 *
 *  PURPOSE:  Include file to configure CONF CORE module of CALYPSO/Locosto PLUS.
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
 *  31-May-2005	 Rohit Joshi	 0.0.2	  Modifications for Locosto
 */  


#ifndef __SYS_CONF_H__
#define __SYS_CONF_H__

  #if (CHIPSET == 12)
    #include "sys_types.h"
    #include "sys_map.h"
    #include "conf/sys_conf_dsp_int.h"
    
    
    /****************************************************************************
     *                            CONSTANT DEFINITION
     ***************************************************************************/
  
    /*
     *  Registers offset definition
     */
    #define C_CONF_CORE_OFFSET            0x00
    #define C_CONF_PULL_PWRDN_OFFSET      0x06

    #define C_DBG_CORE1_OFFSET            0x02
    #define C_DBG_CORE2_OFFSET            0x04

    #define C_DBG_IRQ_OFFSET              0x0C
    #define C_DBG_DMA_P1_NDFLASH_OFFSET   0x0E
    #define C_DBG_DMA_P2_OFFSET           0x10
    #define C_DBG_DMA_P0_OFFSET           0x12
    #define C_DBG_CLK1_OFFSET             0x14
    #define C_DBG_PATCH_ND_FLSH_OFFSET    0x16
    #define C_DBG_IMIF_OFFSET             0x18
    #define C_DBG_KB_USIM_SHD_OFFSET      0x1A
    #define C_DBG_USIM_OFFSET             0x1C
    #define C_DBG_MISC1_OFFSET            0x1E
    #define C_DBG_MISC2_OFFSET            0x20
    #define C_DBG_CLK2_OFFSET             0x22


    /*
     *  Registers address definition
     */
    #define C_CONF_CORE_REG               * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_CONF_CORE_OFFSET)
    #define C_CONF_PULL_PWRDN_REG         * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_CONF_PULL_PWRDN_OFFSET)

    #define C_DBG_CORE1_REG               * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_DBG_CORE1_OFFSET)
    #define C_DBG_CORE2_REG               * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_DBG_CORE2_OFFSET)

    #define C_DBG_IRQ_REG                 * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_DBG_IRQ_OFFSET)
    #define C_DBG_DMA_P1_NDFLASH_REG      * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_DBG_DMA_P1_NDFLASH_OFFSET)
    #define C_DBG_DMA_P2_REG              * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_DBG_DMA_P2_OFFSET)
    #define C_DBG_DMA_P0_REG              * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_DBG_DMA_P0_OFFSET)
    #define C_DBG_CLK1_REG                * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_DBG_CLK1_OFFSET)
    #define C_DBG_PATCH_ND_FLSH_REG       * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_DBG_PATCH_ND_FLSH_OFFSET)
    #define C_DBG_IMIF_REG                * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_DBG_IMIF_OFFSET)
    #define C_DBG_KB_USIM_SHD_REG         * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_DBG_KB_USIM_SHD_OFFSET)
    #define C_DBG_USIM_REG                * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_DBG_USIM_OFFSET)
    #define C_DBG_MISC1_REG               * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_DBG_MISC1_OFFSET)
    #define C_DBG_MISC2_REG               * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_DBG_MISC2_OFFSET)
    #define C_DBG_CLK2_REG                * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_DBG_CLK2_OFFSET)


    /*
     *  DBG_IRQ register definition
     */
    /* nCS2 functional pin */
    #define C_DBG_IRQ_INT4N                         0x0001
    #define C_DBG_IRQ_TPU_WAIT                      0x0002

    /* IO_8 functional pin */
    #define C_DBG_IRQ_SHD_EZ8                       0x0004
    #define C_DBG_IRQ_INT1N                         0x0008

    /* NAND_CE1 functional pin : configured in other register : DBG_USIM */
    #define C_DBG_IRQ_INT10N                        0x0010

    /* IO_15 functional pin : already configured in DBG_CLK2 */
    #define C_DBG_IRQ_NWAIT                         0x0020

    /* FDP functional pin */
    #define C_DBG_IRQ_IACKN                         0x0040

    /* IO_10 functional pin */
    #define C_DBG_IRQ_NMIIT                         0x0080

    /* IO_4 functional pin */
    #define C_DBG_IRQ_IRQ4                          0x0100

    /* nSCS1 functional pin */
    #define C_DBG_IRQ_IRQ14                         0x0200

    /* nCS1 functional pin */
    #define C_DBG_IRQ_ARM_NIRQ_VIEW0                0x0400

    /* IO_12 functional pin */
    #define C_DBG_IRQ_NCSS2                         0x0800
    #define C_DBG_IRQ_ARM_NIRQ_VIEW1                0x1000

    /* IO_9 functional pin */
    #define C_DBG_IRQ_NIRQ                          0x2000

    /* IO_5 functional pin */
    #define C_DBG_IRQ_NFIQ                          0x4000


    /*
     *  DBG_DMA_P1_NDFLASH register definition
     */
    /* KBR(3) functional pin */
    #define C_DBG_DMA_P1_NDFLASH_DMA_REQ_V1         0x0001
    #define C_DBG_DMA_P1_NDFLASH_ND_FLASH_STATE_3   0x0002

    /* KBR(5) functional pin */
    #define C_DBG_DMA_P1_NDFLASH_DMA_REQ_S_1        0x0004
    #define C_DBG_DMA_P1_NDFLASH_ND_FLASH_STATE_4   0x0008

    /* KBC(4) functional pin */
    #define C_DBG_DMA_P1_NDFLASH_DMA_REQ_P1_0       0x0010
    #define C_DBG_DMA_P1_NDFLASH_ND_FLASH_CLK_REQ   0x0020

    /* KBC(5) functional pin */
    #define C_DBG_DMA_P1_NDFLASH_DMA_REQ_P1_1       0x0040
    #define C_DBG_DMA_P1_NDFLASH_ND_FLASH_STATE_0   0x0080

    /* KBR(0) functional pin */
    #define C_DBG_DMA_P1_NDFLASH_DMA_REQ_P1_2       0x0100
    #define C_DBG_DMA_P1_NDFLASH_ND_FLASH_STATE_1   0x0200

    /* KBR(1) functional pin */
    #define C_DBG_DMA_P1_NDFLASH_DMA_REQ_P1_3       0x0400
    #define C_DBG_DMA_P1_NDFLASH_ND_FLASH_STATE_2   0x0800

    /* SDMC_DAT(2) functional pin */
    #define C_DBG_DMA_P1_NDFLASH_NDMA_REQ_VIEW_0    0x1000
    #define C_DBG_DMA_P1_NDFLASH_MMC_SPI_CS1        0x2000

    /* NAND_RnB functional pin */
    #define C_DBG_DMA_P1_NDFLASH_NDMA_REQ_VIEW_1    0x4000
    #define C_DBG_DMA_P1_NDFLASH_SIM_RnW            0x8000


    /*
     *  DBG_DMA_P2 register definition
     */
    /* ADD_24 functional pin */
    #define C_DBG_DMA_P2_DMA_REQ_V2                 0x0001
    #define C_DBG_DMA_P2_GPO_2                      0x0002

    /* ADD_25 functional pin */
    #define C_DBG_DMA_P2_DMA_REQ_S2                 0x0004
    #define C_DBG_DMA_P2_GPO_3                      0x0008

    /* TSPACT_10 functional pin */
    #define C_DBG_DMA_P2_DMA_REQUEST_P2_0           0x0010

    /* TSPACT_9 functional pin */
    #define C_DBG_DMA_P2_DMA_REQUEST_P2_1           0x0020

    /* TSPACT_8 functional pin */
    #define C_DBG_DMA_P2_DMA_REQUEST_P2_2           0x0040

    /* SDMC_DAT_1 functional pin */
    #define C_DBG_DMA_P2_DMA_REQUEST_P2_3           0x0080
    #define C_DBG_DMA_P2_MMC_SPI_CS0                0x0100


    /*
     *  DBG_DMA_P0 register definition
     */
    /* KBR_2 functional pin */
    #define C_DBG_DMA_P0_DMA_REQ_V0                 0x0001
    #define C_DBG_DMA_P0_X_A_5                      0x0002

    /* KBR_4 functional pin */
    #define C_DBG_DMA_P0_DMA_REQ_S0                 0x0004
    #define C_DBG_DMA_P0_X_A_6                      0x0008

    /* KBC_0 functional pin */
    #define C_DBG_DMA_P0_DMA_REQUEST_P0_0           0x0010

    /* KBC_1 functional pin */
    #define C_DBG_DMA_P0_DMA_REQUEST_P0_1           0x0020

    /* KBC_2 functional pin */
    #define C_DBG_DMA_P0_DMA_REQUEST_P0_2           0x0040

    /* KBC_3 functional pin */
    #define C_DBG_DMA_P0_DMA_REQUEST_P0_3           0x0080


    /*
     *  DBG_CLK1 register definition
     */
    /* IO_11 functional pin */
    #define C_DBG_CLK1_MCLK                         0x0001

    /* SPARE_1 functional pin */
    #define C_DBG_CLK1_DSP_CLKOUT                   0x0002
    #define C_DBG_CLK1_SHPM                         0x0004

    /* APLL_DIV_CLK functional pin */
    #define C_DBG_CLK1_LCD_FIFO_FULL                0x0008
    #define C_DBG_CLK1_DPLL_CLKOUT                  0x0010
    #define C_DBG_CLK1_APM                          0x0020

    /* LT functional pin */
    #define C_DBG_CLK1_PWL                          0x0040
    #define C_DBG_CLK1_BRIDGE_CLK                   0x0080

    /* BU functional pin */
    #define C_DBG_CLK1_PWT                          0x0100
    #define C_DBG_CLK1_DMA_CLK_REQ                  0x0200

    /* DSR_MODEM_1 functional pin */
    #define C_DBG_CLK1_LPG                          0x0400


    /*
     *  DBG_PATCH_ND_FLSH register definition
     */
    /* LCD_RnW functional pin */
    #define C_DBG_PATCH_ND_FLSH_PATCH_DETECT        0x0001
    #define C_DBG_PATCH_ND_FLSH_XDI_O_0             0x0002

    /* LCD_nCS0 functional pin */
    #define C_DBG_PATCH_ND_FLSH_PATCH_STATE_0       0x0004
    #define C_DBG_PATCH_ND_FLSH_XDI_O_1             0x0008

    /* LCD_ESTRB functional pin */
    #define C_DBG_PATCH_ND_FLSH_PATCH_STATE_1       0x0010
    #define C_DBG_PATCH_ND_FLSH_XDI_O_2             0x0020

    /* LCD_nCS1 functional pin */
    #define C_DBG_PATCH_ND_FLSH_PATCH_STATE_2       0x0040
    #define C_DBG_PATCH_ND_FLSH_XDI_O_3             0x0080

    /* LCD_nRESET functional pin */
    #define C_DBG_PATCH_ND_FLSH_ND_FLASH_CLK_REQ    0x0100
    #define C_DBG_PATCH_ND_FLSH_XDI_O_4             0x0200

    /* LCD_D_5 functional pin */
    #define C_DBG_PATCH_ND_FLSH_ND_FLASH_STATE_0    0x0400
    #define C_DBG_PATCH_ND_FLSH_XDI_O_5             0x0800

    /* LCD_RS functional pin */
    #define C_DBG_PATCH_ND_FLSH_ND_FLASH_STATE_1    0x1000
    #define C_DBG_PATCH_ND_FLSH_XDI_O_6             0x2000

    /* LCD_D_0 functional pin */
    #define C_DBG_PATCH_ND_FLSH_ND_FLASH_STATE_2    0x4000
    #define C_DBG_PATCH_ND_FLSH_XDI_O_7             0x8000


    /*
     *  DBG_IMIF register definition
     */
    /* LCD_D_3 functional pin */
    #define C_DBG_IMIF_GEA_WORKING                  0x0001
    #define C_DBG_IMIF_IMIF_CCS_0                   0x0002
    #define C_DBG_IMIF_X_A_2                        0x0004

    /* LCD_D_4 functional pin */
    #define C_DBG_IMIF_GEA_DL_NUL                   0x0008
    #define C_DBG_IMIF_IMIF_CCS_1                   0x0010
    #define C_DBG_IMIF_X_A_3                        0x0020

    /* LCD_D_6 functional pin */
    #define C_DBG_IMIF_IMIF_CCS_2                   0x0040
    #define C_DBG_IMIF_X_A_4                        0x0080

    /* LCD_D_7 functional pin */
    #define C_DBG_IMIF_IMIF_CCS_3                   0x0100
    #define C_DBG_IMIF_XIO_NREADY_MEM               0x0200

    /* NAND_IO_5 functional pin */
    #define C_DBG_IMIF_IMIF_CS6_0                   0x0400
    #define C_DBG_IMIF_DMA_REQ_SRC_4                0x0800

    /* NAND_nWP1 functional pin */
    #define C_DBG_IMIF_IMIF_PM                      0x1000
    #define C_DBG_IMIF_USIM_TX_STATE_1              0x4000


    /*
     *  DBG_KB_USIM_SHD register definition
     */
    /* NAND_RE functional pin */
    #define C_DBG_KB_USIM_SHD_KB_STATE_0            0x0001
    #define C_DBG_KB_USIM_SHD_SHD_EZ4KX16           0x0002
    #define C_DBG_KB_USIM_SHD_USIM_STATE_0          0x0004

    /* NAND_WE functional pin */
    #define C_DBG_KB_USIM_SHD_KB_STATE_1            0x0008
    #define C_DBG_KB_USIM_SHD_USIM_STATE_1          0x0010
    #define C_DBG_KB_USIM_SHD_BRIDGE_EN             0x0020

    /* NAND_CLE functional pin */
    #define C_DBG_KB_USIM_SHD_KB_STATE_2            0x0040
    #define C_DBG_KB_USIM_SHD_DSP_NIRQ_VIEW_0       0x0080
    #define C_DBG_KB_USIM_SHD_USIM_STATE_2          0x0100

    /* NAND_ALE functional pin */
    #define C_DBG_KB_USIM_SHD_KB_STATE_3            0x0200
    #define C_DBG_KB_USIM_SHD_DSP_NIRQ_VIEW_1       0x0400
    #define C_DBG_KB_USIM_SHD_USIM_STATE_3          0x0800


    /*
     *  DBG_USIM register definition
     */
    /* NAND_IO_8 functional pin */
    #define C_DBG_USIM_RHEA_NSTROBE                 0x0001
    #define C_DBG_USIM_USIM_RX_STATE_1              0x0002

    /* NAND_CE1 functional pin : configured in other register : DBG_IRQ */
    #define C_DBG_USIM_RHEA_NREADY                  0x0004
    #define C_DBG_USIM_USIM_TX_STATE_0              0x0008

    /* NAND_ALE functional pin */
    #define C_DBG_USIM_SHD_EZ9                      0x0010
    #define C_DBG_USIM_SHPM                         0x0020
    #define C_DBG_USIM_USIM_RX_STATE_0              0x0040

    /* NAND_ALE functional pin */
    #define C_DBG_USIM_SHD_EZ8                      0x0080
    #define C_DBG_USIM_APM                          0x0100
    #define C_DBG_USIM_USIM_START_BIT               0x0200

    /* NAND_ALE functional pin */
    #define C_DBG_USIM_ND_FLASH_STATE_3             0x0400
    #define C_DBG_USIM_X_A_0                        0x0800

    /* NAND_ALE functional pin */
    #define C_DBG_USIM_ND_FLASH_STATE_4             0x1000
    #define C_DBG_USIM_X_A_1                        0x2000


    /*
     *  DBG_MISC1 register definition
     */
    /* nCS3 functional pin */
    #define C_DBG_MISC1_TOUT1                       0x0001
    #define C_DBG_MISC1_TPU_IDLE                    0x0002

    /* MMC_CLK functional pin */
    #define C_DBG_MISC1_MS_CLK                      0x0004
    #define C_DBG_MISC1_MMC_SPI_CLK                 0x0008

    /* IO_13 functional pin */
    #define C_DBG_MISC1_MCUEN_2                     0x0010
    #define C_DBG_MISC1_MMC_SPI_FREADY              0x0020
    #define C_DBG_MISC1_ARBITRER_NWAIT              0x0040

    /* SDMC_DAT_0 functional pin */
    #define C_DBG_MISC1_MS_SDIO                     0x0080
    #define C_DBG_MISC1_MAS_1                       0x0100

    /* SDMC_DAT_3 functional pin */
    #define C_DBG_MISC1_TOUT2                       0x0200
    #define C_DBG_MISC1_MMC_SPI_CS2                 0x0400
    #define C_DBG_MISC1_MAS_0                       0x0800

    /* IO_6 functional pin */
    #define C_DBG_MISC1_XF                          0x1000

    /* IO_14 functional pin : already configured in DBG_CLK2 */
    #define C_DBG_MISC1_NMREQ                       0x2000
    #define C_DBG_MISC1_SHD_EZ9                     0x4000

    /* MMC_CMD functional pin */
    #define C_DBG_MISC1_MS_BS                       0x8000


    /*
     *  DBG_MISC2 register definition
     */
    /* SD_IRDA functional pin */
    #define C_DBG_MISC2_LCD_FIFO_EMPTY              0x0001

    /* ADD_23 functional pin */
    #define C_DBG_MISC2_GPO_1                       0x0002

    /* EXT_IRQ_1 functional pin */
    #define C_DBG_MISC2_START_BIT                   0x0004

    /* EXT_IRQ_2 functional pin */
    #define C_DBG_MISC2_X_IOSTRBN                   0x0008

    /* NAND_IO_1 functional pin */
    #define C_DBG_MISC2_NOPC                        0x0010
    #define C_DBG_MISC2_DMA_REQ_SRC_0               0x0020
    #define C_DBG_MISC2_PATCH_DETECT                0x0040

    /* NAND_IO_2 functional pin */
    #define C_DBG_MISC2_DMA_REQ_SRC_1               0x0080
    #define C_DBG_MISC2_PATCH_STATE_0               0x0100
    #define C_DBG_MISC2_MAS_1                       0x0200

    /* NAND_IO_3 functional pin */
    #define C_DBG_MISC2_MAS_0                       0x0400
    #define C_DBG_MISC2_DMA_REQ_SRC_2               0x0800
    #define C_DBG_MISC2_PATCH_STATE_1               0x1000

    /* NAND_IO_4 functional pin */
    #define C_DBG_MISC2_DMA_REQ_SRC_3               0x2000
    #define C_DBG_MISC2_PATCH_STATE_2               0x4000

    /* ADD_22 functional pin */
    #define C_DBG_MISC2_GPO_0                       0x8000


    /*
     *  DBG_CLK2 register definition
     */
    /* MCUEN_1 functional pin */
    #define C_DBG_CLK2_CPORT_CLKIN                  0x0001

    /* TSPACT_11 functional pin */
    #define C_DBG_CLK2_BCLKX                        0x0002
    #define C_DBG_CLK2_DMA_CLK_REQ2                 0x0004
    #define C_DBG_CLK2_CLKM_CLK                     0x0008

    /* RX_IRDA functional pin */
    #define C_DBG_CLK2_CLK16X_IRDA                  0x0010

    /* IO_7 functional pin */
    #define C_DBG_CLK2_CLKX_SPI                     0x0020

    /* IO_14 functional pin : already configured in DBG_MISC1 */
    #define C_DBG_CLK2_PP_CLK_IN                    0x0040

    /* IO_15 functional pin : already configured in DBG_IRQ */
    #define C_DBG_CLK2_PP_CLK_EN                    0x0080


    /*
     *  DBG_CONF1 register definition
     */
    #define C_DBG_CONF1_ARM_IRQ_SEL0_POS            0
    #define C_DBG_CONF1_ARM_IRQ_SEL1_POS            5
    #define C_DBG_CONF1_DSP_IRQ_SEL0_POS            10

    
    /*
     *  DBG_CONF2 register definition
     */
    #define C_DBG_CONF2_DSP_IRQ_SEL1_POS            0
    #define C_DBG_CONF2_DMA_REQ_SEL0_POS            5
    #define C_DBG_CONF2_DMA_REQ_SEL1_POS            10


    #define C_DBG_CONF_ARM_IRQ_MASK                 0x001F
    #define C_DBG_CONF_DSP_IRQ_MASK                 0x000F
    #define C_DBG_CONF_DMA_REQ_MASK                 0x001F


    /*
     *  CONF_CORE register definition
     */
    #define C_CONF_CORE_RIF_CLK_POL_POS             5
    #define C_CONF_CORE_SPI_CLK_POL_POS             6

    #define C_CONF_CORE_RIF_CLK_POL_MASK            0x0001
    #define C_CONF_CORE_SPI_CLK_POL_MASK            0x0001



    /****************************************************************************
     *                            MACRO DEFINITION
     ***************************************************************************/

    #define F_DBG_IRQ_CONFIG(d_config)             C_DBG_IRQ_REG = d_config
    #define F_DBG_DMA_P1_NDFLASH_CONFIG(d_config)  C_DBG_DMA_P1_NDFLASH_REG = d_config
    #define F_DBG_DMA_P2_CONFIG(d_config)          C_DBG_DMA_P2_REG = d_config
    #define F_DBG_DMA_P0_CONFIG(d_config)          C_DBG_DMA_P0_REG = d_config
    #define F_DBG_CLK1_CONFIG(d_config)            C_DBG_CLK1_REG = d_config
    #define F_DBG_PATCH_ND_FLSH_CONFIG(d_config)   C_DBG_PATCH_ND_FLSH_REG = d_config
    #define F_DBG_IMIF_CONFIG(d_config)             C_DBG_IMIF_REG = d_config
    #define F_DBG_KB_USIM_SHD_CONFIG(d_config)     C_DBG_KB_USIM_SHD_REG = d_config
    #define F_DBG_USIM_CONFIG(d_config)            C_DBG_USIM_REG = d_config
    #define F_DBG_MISC1_CONFIG(d_config)           C_DBG_MISC1_REG = d_config
    #define F_DBG_MISC2_CONFIG(d_config)           C_DBG_MISC2_REG = d_config
    #define F_DBG_CLK2_CONFIG(d_config)            C_DBG_CLK2_REG = d_config


    /**************************************************************************
     *
     *  FUNCTION NAME: F_DBG_CONFIG_VIEW
     *      Configure the ARM_nIRQ_VIEW(1:0), DSP_nIRQ_VIEW(1:0) and 
     *      nDMA_REQ_VIEW(1:0).
     *
     *
     *  ARGUMENT LIST:
     *
     *  Argument         Description
     *  --------------   --------------------------------------------------------
     *  d_arm_irq_sel0   ARM interrupt index mapped on ARM_nIRQ_VIEW(0) (use index
     *                   defined in the Interrupt Handler module).
     *  d_arm_irq_sel1   ARM interrupt index mapped on ARM_nIRQ_VIEW(1) (use index
     *                   defined in the Interrupt Handler module).
     *  d_dsp_irq_sel0   DSP interrupt index mapped on DSP_nIRQ_VIEW(0).
     *  d_dsp_irq_sel1   DSP interrupt index mapped on DSP_nIRQ_VIEW(1).
     *  d_dma_req_sel0   DMA request index mapped on nDMA_REQ_VIEW(0) (use index 
     *                   defined in the DMA module).
     *  d_dma_req_sel1   DMA request index mapped on nDMA_REQ_VIEW(0) (use index 
     *                   defined in the DMA module).
     *
     * RETURN VALUE: None
     *
     **************************************************************************/

    #define F_DBG_VIEW_CONFIG(d_arm_irq_sel0, d_arm_irq_sel1, \
                              d_dsp_irq_sel0, d_dsp_irq_sel1, \
                              d_dma_req_sel0, d_dma_req_sel1) { \
      C_DBG_CORE1_REG = ((d_arm_irq_sel0 & C_DBG_CONF_ARM_IRQ_MASK) << C_DBG_CONF1_ARM_IRQ_SEL0_POS) | \
                        ((d_arm_irq_sel1 & C_DBG_CONF_ARM_IRQ_MASK) << C_DBG_CONF1_ARM_IRQ_SEL1_POS) | \
                        ((d_dsp_irq_sel0 & C_DBG_CONF_DSP_IRQ_MASK) << C_DBG_CONF1_DSP_IRQ_SEL0_POS);  \
      C_DBG_CORE2_REG = ((d_dsp_irq_sel1 & C_DBG_CONF_DSP_IRQ_MASK) << C_DBG_CONF2_DSP_IRQ_SEL1_POS) | \
                        ((d_dma_req_sel0 & C_DBG_CONF_DMA_REQ_MASK) << C_DBG_CONF2_DMA_REQ_SEL0_POS) | \
                        ((d_dma_req_sel1 & C_DBG_CONF_DMA_REQ_MASK) << C_DBG_CONF2_DMA_REQ_SEL1_POS); \
    }



    #define F_CONF_RIF_RX_FALLING_EDGE              C_CONF_CORE_REG &= ~(C_CONF_CORE_RIF_CLK_POL_MASK << C_CONF_CORE_RIF_CLK_POL_POS)
    #define F_CONF_RIF_RX_RISING_EDGE               C_CONF_CORE_REG |= (C_CONF_CORE_RIF_CLK_POL_MASK << C_CONF_CORE_RIF_CLK_POL_POS)

    #define F_CONF_SPI_RX_FALLING_EDGE              C_CONF_CORE_REG &= ~(C_CONF_CORE_SPI_CLK_POL_MASK << C_CONF_CORE_SPI_CLK_POL_POS)
    #define F_CONF_SPI_RX_RISING_EDGE               C_CONF_CORE_REG |= (C_CONF_CORE_SPI_CLK_POL_MASK << C_CONF_CORE_SPI_CLK_POL_POS)



    /****************************************************************************
     *                            STRUCTURE DEFINITION
     ***************************************************************************/

    /****************************************************************************
     *                            PROTOTYPE DEFINITION
     ***************************************************************************/
    
 
  #endif /* (CHIPSET == 12) */
  
  #if (CHIPSET == 15)
    #include "sys_types.h"
    #include "sys_map.h"
    #include "conf/sys_conf_dsp_int.h"
    
    
    /****************************************************************************
     *                            CONSTANT DEFINITION
     ***************************************************************************/
  
    /*
     *  Registers offset definition
     */

	/* Configuration Core */
    #define C_CONF_CORE_OFFSET            0x00
    #define C_CONF_LCD_CAM_NAN_OFFSET     0x02
    #define C_CONF_DEBUG_OFFSET           0x04

	/* IO Registers */
    #define C_CONF_GPIO_0_OFFSET		  0x00
	#define C_CONF_GPIO_1_OFFSET		  0x02
	#define C_CONF_GPIO_2_OFFSET		  0x04
	#define C_CONF_SPARE_3_OFFSET		  0x06
	#define C_CONF_USB_RCV_OFFSET		  0x08
	#define C_CONF_USB_SE0_OFFSET		  0x0A
	#define C_CONF_USB_DAT_OFFSET		  0x0C
	#define C_CONF_USB_TXEN_OFFSET		  0x0E
	#define C_CONF_TRST_OFFSET		  	  0x10
	#define C_CONF_ABB_IRQ_OFFSET	  	  0x12
	#define C_CONF_CSYNC_OFFSET	  	  	  0x14
	#define C_CONF_CSCLK_OFFSET	  	  	  0x16	
	#define C_CONF_CD0_OFFSET	  	  	  0x18
	#define C_CONF_GPIO_4_OFFSET	  	  0x1A
	#define C_CONF_GPIO_5_OFFSET	  	  0x1C
	#define C_CONF_USB_BOOT_OFFSET	  	  0x1E
	#define C_CONF_SIM_RST_OFFSET	  	  0x20
	#define C_CONF_SIM_IO_OFFSET	  	  0x22
	#define C_CONF_SIM_CLK_OFFSET	  	  0x24
	#define C_CONF_SIM_PWRCTRL_OFFSET  	  0x26
	#define C_CONF_KBC_0_OFFSET		  	  0x28
	#define C_CONF_KBC_1_OFFSET		  	  0x2A
	#define C_CONF_KBC_2_OFFSET		  	  0x2C
	#define C_CONF_KBC_3_OFFSET		  	  0x2E
	#define C_CONF_TSPACT_11_OFFSET	  	  0x30	
	#define C_CONF_TSPACT_12_OFFSET	  	  0x32
	#define C_CONF_TSPACT_13_OFFSET	  	  0x34
	#define C_CONF_TSPACT_14_OFFSET	  	  0x36
	#define C_CONF_TSPACT_15_OFFSET	  	  0x38
	#define C_CONF_KBR_0_OFFSET		  	  0x3A
	#define C_CONF_KBR_1_OFFSET		  	  0x3C
	#define C_CONF_KBR_2_OFFSET		  	  0x3E
	#define C_CONF_KBR_3_OFFSET		  	  0x40
	#define C_CONF_GPIO_8_OFFSET	  	  0x42	
	#define C_CONF_GPIO_9_OFFSET	  	  0x44
	#define C_CONF_GPIO_10_OFFSET	  	  0x46
	#define C_CONF_GPIO_11_OFFSET	  	  0x48
	#define C_CONF_GPIO_12_OFFSET	  	  0x4A
	#define C_CONF_GPIO_13_OFFSET	  	  0x4C
	#define C_CONF_LCD_NRST_OFFSET	  	  0x4E
	#define C_CONF_LCD_STB_OFFSET	  	  0x50
	#define C_CONF_LCD_RNW_OFFSET	  	  0x52
	#define C_CONF_LCD_RS_OFFSET	  	  0x54
	#define C_CONF_GPIO_17_OFFSET	  	  0x56
	#define C_CONF_GPIO_18_OFFSET	  	  0x58
	#define C_CONF_LCD_DATA_0_OFFSET	  0x5A
	#define C_CONF_LCD_DATA_1_OFFSET	  0x5C
	#define C_CONF_LCD_DATA_2_OFFSET	  0x5E
	#define C_CONF_LCD_DATA_3_OFFSET	  0x60
	#define C_CONF_LCD_DATA_4_OFFSET	  0x62
	#define C_CONF_LCD_DATA_5_OFFSET	  0x64
	#define C_CONF_LCD_DATA_6_OFFSET	  0x66
	#define C_CONF_LCD_DATA_7_OFFSET	  0x68
	#define C_CONF_GPIO_19_OFFSET	  	  0x6A
	#define C_CONF_GPIO_20_OFFSET	  	  0x6C
	#define C_CONF_GPIO_21_OFFSET	  	  0x6E
	#define C_CONF_GPIO_22_OFFSET	  	  0x70
	#define C_CONF_GPIO_23_OFFSET	  	  0x72
	#define C_CONF_GPIO_24_OFFSET	  	  0x74
	#define C_CONF_GPIO_25_OFFSET	  	  0x76
	#define C_CONF_GPIO_26_OFFSET	  	  0x78
	#define C_CONF_GPIO_27_OFFSET	  	  0x7A
	#define C_CONF_GPIO_28_OFFSET	  	  0x7C
	#define C_CONF_GPIO_29_OFFSET	  	  0x7E
	#define C_CONF_GPIO_30_OFFSET	  	  0x80
	#define C_CONF_TMS_OFFSET		  	  0x82
	#define C_CONF_ND_NWP_OFFSET	  	  0x84
	#define C_CONF_GPIO_31_OFFSET	  	  0x86
	#define C_CONF_TCK_OFFSET		  	  0x88
	#define C_CONF_GPIO_32_OFFSET	  	  0x8A
	#define C_CONF_TDI_OFFSET		  	  0x8C
	#define C_CONF_GPIO_33_OFFSET	  	  0x8E
	#define C_CONF_GPIO_34_OFFSET	  	  0x90
	#define C_CONF_ND_CE1_OFFSET	  	  0x92
	#define C_CONF_GPIO_35_OFFSET	  	  0x94
	#define C_CONF_GPIO_36_OFFSET	  	  0x96
	#define C_CONF_GPIO_37_OFFSET	  	  0x98
	#define C_CONF_GPIO_38_OFFSET	  	  0x9A
	#define C_CONF_GPIO_39_OFFSET	  	  0x9C
	#define C_CONF_NRDY_OFFSET		  	  0x9E
	#define C_CONF_ADV_OFFSET		  	  0xA0
	#define C_CONF_GPIO_42_OFFSET	  	  0xA2
	#define C_CONF_GPIO_43_OFFSET	  	  0xA4
	#define C_CONF_GPIO_44_OFFSET	  	  0xA6
	#define C_CONF_GPIO_45_OFFSET	  	  0xA8
	#define C_CONF_GPIO_46_OFFSET	  	  0xAA
	#define C_CONF_UART_TX_OFFSET	  	  0xAC
	#define C_CONF_UART_RX_OFFSET	  	  0xAE
	#define C_CONF_UART_CTS_OFFSET	  	  0xB0
	#define C_CONF_NBSCAN_OFFSET	  	  0xB2
	#define C_CONF_ADD_21_OFFSET	  	  0xB4
	#define C_CONF_GPIO_47_OFFSET	  	  0xB6
	#define C_CONF_VFSRX_OFFSET	  	  	  0xB8
	#define C_CONF_GPIO_7_OFFSET	  	  0xBA
	#define C_CONF_TDO_OFFSET	  	  	  0xBC
	#define C_CONF_RNW_OFFSET		  	  0xDE
	#define C_CONF_NMOE_OFFSET		  	  0xE0
	#define C_CONF_NCS3_OFFSET		  	  0xE2
	#define C_CONF_CK13MHZ_EN_OFFSET	  0xE4
	#define C_CONF_VDR_OFFSET		  	  0xE6

	/* Define DEBUG Offset: : Add More registers Here as and when required*/
	#define C_DBG_DSP_VIEW_0_OFFSET		  0x41
	
	
    /*
     *  Registers address definition
     */
    #define C_CONF_CORE_REG               * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_CONF_CORE_OFFSET)
    #define C_CONF_LCD_CAM_NAN_REG        * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_CONF_LCD_CAM_NAN_OFFSET)
    #define C_CONF_DEBUG_REG              * (volatile SYS_UWORD16 *) (C_MAP_CORE_CONF_BASE + C_CONF_DEBUG_OFFSET)

    /*  Macro Definations for LCD and NAND 
     *
     */  
    #define MUX_NANDFLASH	0x01 
    #define MUX_LCD		0x02	

 

    /* IO Reg Defines */
	#define C_CONF_IO_REG(_name)          * (volatile SYS_UWORD16 *) (C_MAP_IO_CONF_BASE + _name##_OFFSET)

	#define C_DBG_REG(_name)			  * (volatile SYS_UWORD8 *) (C_MAP_SOFT_DEBUG_BASE + _name##_OFFSET)

	#define C_CONF_PUPD_EN				0x0008
	#define C_CONF_PUPD_VAL				0x0010
    
    /****************************************************************************
     *                            MACRO DEFINITION
     ***************************************************************************/

    #define F_IO_CONFIG(_name,d_config)             C_CONF_IO_REG(_name)= d_config
    #define F_DBG_CONFIG(_name,d_config)            C_DBG_REG(_name)= d_config

    /****************************************************************************
     *                            STRUCTURE DEFINITION
     ***************************************************************************/

    /****************************************************************************
     *                            PROTOTYPE DEFINITION
     ***************************************************************************/
    

  #endif



#endif /* __SYS_CONF_H__ */
