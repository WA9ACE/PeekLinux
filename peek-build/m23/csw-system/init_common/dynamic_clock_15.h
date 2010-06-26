/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * DYNAMIC_CLOCK_15_H
 *
 *        Filename dynamic_clock_15.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

/***********************************************************************************************
 *          Only SAMSON/CALYPSO families are considered for dynamic clock configuration.
 ***********************************************************************************************
 *
 *                         CHIPSET = 15 (LOCOSTO)                       
 *
 ***********************************************************************************************
 *                         Supported clock configuration                           
 *
 *
 *       CHIPSET                78/78/13    78/78/39   104/104/52    156/78/52   130/130/65
 *                                 (0)         (1)          (2)         (3)         (4)
 *
 *  CALYPSO PLUS c035     (12)     NA           X           X           X            X
 *
 *
 *
 *           BOARD                CHIPSET                         Access Time (ns)
 *                                                     CS0    CS1    CS2    CS3    CS4    CS5
 *  
 *
 ***********************************************************************************************/

#include "chipset.cfg"
#include "board.cfg"

#ifndef PSP_FAILSAFE
#define PSP_FAILSAFE 0
#warn "PSP Failsafe Flag Not Defined in your Build, taking default"
#endif


#if (CHIPSET == 15)

#ifndef _DYNAMIC_CLOCK_15_H_
#define _DYNAMIC_CLOCK_15_H_

#ifdef _DYNAMIC_CLOCK_C_

/***************************************************************************
 *                   C_CLOCK_CFG_104_104_104 configuration
 **************************************************************************/
const T_DYNAMIC_CLOCK_CFG d_15_104_104_104_clock = 
{
	/* Index of the present clock configuration */
	C_CLOCK_CFG_104_104_104,

	/* DSP clock in kHz */
	104000,

	/* DPLL configuration */
	DPLL_BYPASS_DIV_1, DPLL_LOCK_DIV_1, 8,

	/* ARM clock configuration */
	CLKM_SEL_DPLL,  CLKM_ARM_SWITCH_FREQ_0, 

	/* DSP latencies configuration */
	D_LAT_MCU_HOM2SAM,
	D_LAT_MCU_BRIDGE,
	D_LAT_MCU_BEF_FAST_ACCESS,
	D_LAT_DSP_AFTER_SAM,
	D_TRANSFER_RATE,

	/* API-RHEA configuration */
	/* API_WS */
	3, 2,
	// 1 WS in HOM mode to workaround HW bug 3504
	//  dma_arbtrs > Concurrent access to API from DMA and MCU
	/* RHEA Access Factor */
	// should be 0xFE00 due to no DMA limitation at 34MHz
	// Must be updated according to ARM clock configuration
	// Timeout must be set to 0xFF for chipset
	// Calypso C035 at 52MHz          
	1, 1,
	/* RHEA Timeout */
	0xFF,

	/* EMIF configuration */
	/* structure used for CHIPSET 15 is different, see definition in sys_memif.h */

#if (BOARD == 70) || (BOARD== 71)
#if (PSP_FAILSAFE==1)
	// CS0
	{
		/* CONF_CS0 register configuration */
		0,C_EMIF_ASYNC_READ_ASYNC_WRITE,15,15,15,C_EMIF_NO_RETIME,C_EMIF_CLK_DIVIDER_2,
		/* FULL HANDSHAKE register configuration */
		C_EMIF_DYN_WAIT_DISABLE,                
		/* ADV_CONF_CS0 register configuration */
		3,0,0,C_EMIF_BT_RD_TRANS,C_EMIF_CLK_NO_MASK,0      
	},
	// CS3
	{
		/* CONF_CS3 register configuration */
		0,C_EMIF_ASYNC_READ_ASYNC_WRITE,15,15,15,C_EMIF_NO_RETIME,C_EMIF_CLK_DIVIDER_2,    
		/* FULL HANDSHAKE register configuration */
		C_EMIF_DYN_WAIT_DISABLE,                
		/* ADV_CONF_CS2 register configuration */
		4,0,0,C_EMIF_BT_RD_TRANS,C_EMIF_CLK_NO_MASK,0       
	},
#else
	//CS0
	{
		/* CONF_CS0 register configuration */
		0,C_EMIF_ASYNC_READ_ASYNC_WRITE,4,3,5,C_EMIF_NO_RETIME,C_EMIF_CLK_DIVIDER_2,
		/* FULL HANDSHAKE register configuration */
		C_EMIF_DYN_WAIT_ENABLE,                
		/* ADV_CONF_CS0 register configuration */
		3,0,0,C_EMIF_BT_RD_TRANS,C_EMIF_CLK_NO_MASK,0      
	},
	// CS3
	{
		/* CONF_CS3 register configuration */
		0,C_EMIF_ASYNC_READ_ASYNC_WRITE,3,6,6,C_EMIF_NO_RETIME,C_EMIF_CLK_DIVIDER_2,    
		/* FULL HANDSHAKE register configuration */
		C_EMIF_DYN_WAIT_ENABLE,                
		/* ADV_CONF_CS2 register configuration */
		4,0,0,C_EMIF_BT_RD_TRANS,C_EMIF_CLK_NO_MASK,0       
	},
#endif
#else
#error "This BOARD configuration is not supported"
#endif
};



/***************************************************************************
 *                   C_CLOCK_CFG_104_104_52 configuration
 **************************************************************************/
const T_DYNAMIC_CLOCK_CFG d_15_104_104_52_clock = 
{
	/* Index of the present clock configuration */
	C_CLOCK_CFG_104_104_52,

	/* DSP clock in kHz */
	104000,

	/* DPLL configuration */
	DPLL_BYPASS_DIV_1, DPLL_LOCK_DIV_1, 4,

	/* ARM clock configuration */
	CLKM_SEL_DPLL,  CLKM_ARM_SWITCH_FREQ_1, 

	/* DSP latencies configuration */
	D_LAT_MCU_HOM2SAM,
	D_LAT_MCU_BRIDGE,
	D_LAT_MCU_BEF_FAST_ACCESS,
	D_LAT_DSP_AFTER_SAM,
	D_TRANSFER_RATE,

	/* API-RHEA configuration */
	/* API_WS */
	0, 0,
	// 1 WS in HOM mode to workaround HW bug 3504
	//  dma_arbtrs > Concurrent access to API from DMA and MCU
	/* RHEA Access Factor */
	// should be 0xFE00 due to no DMA limitation at 34MHz
	// Must be updated according to ARM clock configuration
	// Timeout must be set to 0xFF for chipset
	// Calypso C035 at 52MHz          
	1, 1,
	/* RHEA Timeout */
	0xFF,

	/* EMIF configuration */
	/* structure used for CHIPSET 15 is different, see definition in sys_memif.h */

#if (BOARD == 70)|| (BOARD== 71)
#if (PSP_FAILSAFE==1)
	// CS0
	{
		/* CONF_CS0 register configuration */
		0,C_EMIF_ASYNC_READ_ASYNC_WRITE,15,15,15,C_EMIF_NO_RETIME,C_EMIF_CLK_DIVIDER_2,
		/* FULL HANDSHAKE register configuration */
		C_EMIF_DYN_WAIT_DISABLE,                
		/* ADV_CONF_CS0 register configuration */
		3,0,0,C_EMIF_BT_RD_TRANS,C_EMIF_CLK_NO_MASK,0      
	},
	{
		/* CONF_CS3 register configuration */
		0,C_EMIF_ASYNC_READ_ASYNC_WRITE,15,15,15,C_EMIF_NO_RETIME,C_EMIF_CLK_DIVIDER_2,    
		/* FULL HANDSHAKE register configuration */
		C_EMIF_DYN_WAIT_DISABLE,                
		/* ADV_CONF_CS2 register configuration */
		4,0,0,C_EMIF_BT_RD_TRANS,C_EMIF_CLK_NO_MASK,0       
	},
#else
	//CS0
	{
		/* CONF_CS0 register configuration */
		0,C_EMIF_ASYNC_READ_ASYNC_WRITE,4,3,5,C_EMIF_NO_RETIME,C_EMIF_CLK_DIVIDER_1,
		/* FULL HANDSHAKE register configuration */
		C_EMIF_DYN_WAIT_ENABLE,                
		/* ADV_CONF_CS0 register configuration */
		3,0,0,C_EMIF_BT_RD_TRANS,C_EMIF_CLK_NO_MASK,0      
	},
	// CS3
	{
		/* CONF_CS3 register configuration */
		0,C_EMIF_ASYNC_READ_ASYNC_WRITE,3,6,6,C_EMIF_NO_RETIME,C_EMIF_CLK_DIVIDER_1,    
		/* FULL HANDSHAKE register configuration */
		C_EMIF_DYN_WAIT_ENABLE,                
		/* ADV_CONF_CS2 register configuration */
		4,0,0,C_EMIF_BT_RD_TRANS,C_EMIF_CLK_NO_MASK,0       
	},      
	// BOARD == 70
#endif
#else
#error "This BOARD configuration is not supported"
#endif
};

const T_DYNAMIC_CLOCK_CFG * a_dynamic_clock_cfg[C_NB_MAX_CLOCK_CONFIG] = {
	&d_15_104_104_104_clock,       /* 104/104/104 MHz */
	&d_15_104_104_52_clock,         /* 104/104/52 MHz */
	(T_DYNAMIC_CLOCK_CFG *) NULL,
};

#else
extern const T_DYNAMIC_CLOCK_CFG * a_dynamic_clock_cfg[C_NB_MAX_CLOCK_CONFIG];
#endif  /* _DYNAMIC_CLOCK_C_ */

#endif  /* _DYNAMIC_CLOCK_15_H_ */

#endif /* CHIPSET == 15 */



