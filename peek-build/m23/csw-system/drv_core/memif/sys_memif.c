/*                @(#) nom : sys_memif.c SID: 1.2 date : 05/23/03             */
/* Filename:      sys_memif.c                                                 */
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
 *  FILE NAME: sys_memif.c
 *
 *
 *  PURPOSE:  Drivers to be used for ARM memory interface configuration and
 *            control for CALYPSO PLUS.
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
 *  25-Feb-2003  G.Leterrier     0.0.2    Modification for reserved bit management 
 *                                        f_memif_init_cs
 *                                        f_memif_init_api_rhea_ctrl
 *  26-Feb-2003 G.Leterrier               Disable page mode before re-configuration
 *
 *  ALGORITHM: 
 *
 *
 *****************************************************************************/

#include "sys_types.h"
#include "chipset.cfg"
#include "board.cfg"
#include "memif/sys_memif.h"

#if ( CHIPSET == 15 ) /* (CHIPSET == 12) */

  #include "sys_types.h"
  #include "sys_memif.h"
#include "pin_config.h"


   /******************************************************************************
   *
   *  FUNCTION NAME: f_emif_set_priority
   *    EMIF set priority of DMA and MCU access.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument     Type         IO  Description
   *  ----------   ----------   --  -------------------------------------------
   *  d_dma_access  SYS_UWORD8   I  Number of continous DMA access
   *  d_mpu_access  SYS_UWORD8   I  Number of continous ARM access
   *  
   * RETURN VALUE: 0 for success
   *
   *****************************************************************************/ 
  SYS_UWORD8 f_emif_set_priority ( SYS_UWORD8 d_dma_access, 
		  SYS_UWORD8 d_mpu_access) {
     
    C_EMIF_LRU_PRIORITY_REG = d_mpu_access << C_EMIF_MPU_POS | 
	    d_dma_access << C_EMIF_DMA_POS;

    return 0;
  }

  /******************************************************************************
   *
   *  FUNCTION NAME: f_emif_set_conf
   *    EMIF set priority of DMA and MCU access.
   *
   *
   *  ARGUMENT LIST:
   *
   *   Argument     	Type         		IO  Description
   *  ----------   	----------   		--  ------------------------------
   *  d_prefetch_mode  	T_EMIF_PREFETCH_MODE   	I  Prefetch mode
   *  d_pde_enable  	T_PDE_STATE   		I  Global power down enable/disable
   *  d_pwd_enable	T_PWD_STATE   		I  Power Down enable/disable
   *    
   * RETURN VALUE: 0 for success
   *
   *****************************************************************************/ 
   SYS_UWORD8 f_emif_set_conf ( T_EMIF_CONF* emif_conf ) {
    SYS_UWORD16 d_emif_conf_reg;

    d_emif_conf_reg = C_EMIF_CONF_REG;
    d_emif_conf_reg &= C_EMIF_CONF_MASK;

    C_EMIF_CONF_REG = emif_conf->write_protect << C_EMIF_WRITE_PROTECT_POS | 
		emif_conf->flush_prefetch << C_EMIF_FLUSH_PREFETCH |
		emif_conf->d_prefetch_mode << C_EMIF_PREFETCH_MODE_POS |
	    emif_conf->d_pde_enable << C_EMIF_PDE_POS |
	    emif_conf->d_pwd_enable << C_EMIF_PWD_EN_POS | d_emif_conf_reg;
	    
    return 0;
  }

   /******************************************************************************
   *
   *  FUNCTION NAME: f_emif_cs_mode
   *    Configures EMIF for the specific chip-selects.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument     	Type         	 IO  Description
   *  ----------  	----------   	 -- -------------------------------------
   *  d_cs	   	SYS_UWORD8   	 I  Chip select
   *  p_emif_cs_config  T_EMIF_CS_CONFIG I  Configuration information for each CS
   *  
   * RETURN VALUE: 0 for success
   *
   *****************************************************************************/ 
  SYS_UWORD8 f_emif_cs_mode (SYS_UWORD8 d_cs, 
		  T_EMIF_CS_CONFIG* p_emif_cs_config) {
    SYS_UWORD32 d_emif_cs_config_reg;

    d_emif_cs_config_reg = C_EMIF_CONF_CSX_REG( d_cs );
    d_emif_cs_config_reg &= C_EMIF_CONF_CSX_MASK;

    C_EMIF_CONF_CSX_REG( d_cs ) = p_emif_cs_config->d_wait_read_write_trans << C_EMIF_BTWST_POS |
	     p_emif_cs_config->d_memmode << C_EMIF_MEMMODE_POS |
	     p_emif_cs_config->d_we << C_EMIF_WELEN_POS |
	     p_emif_cs_config->d_wait_write << C_EMIF_WRWST_POS |
	     p_emif_cs_config->d_wait_read << C_EMIF_RDWST_POS |
	     p_emif_cs_config->d_retime << C_EMIF_RETIME_POS |
	     p_emif_cs_config->d_flash_clk_div << C_EMIF_FCLK_DIV_POS | 
	     d_emif_cs_config_reg;

    C_EMIF_ADV_CONF_CSX_REG( d_cs ) =  p_emif_cs_config->d_oe_setup << C_EMIF_OESETUP_POS |
	    p_emif_cs_config->d_oe_hold << C_EMIF_OEHOLD_POS |
	    p_emif_cs_config->d_adv_hold <<  C_EMIF_ADVHOLD_POS |
	    p_emif_cs_config->d_bus_turn_mode <<  C_EMIF_BTMODE_POS |
	    p_emif_cs_config->d_clk_mask << C_EMIF_CLKMSK_POS |
	    p_emif_cs_config->d_ready_configuration << C_EMIF_RC_POS;
    
    C_EMIF_DYNAMIC_WS_REG = (C_EMIF_DYNAMIC_WS_REG & (~(1<< d_cs))) | ((~p_emif_cs_config->d_non_full_handshake_mode) & 1) << d_cs;
    return 0;  		
  }

   /******************************************************************************
   *
   *  FUNCTION NAME: f_emif_abort_conf
   *    Configure EMIF timeout.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument          Type         IO  Description
   *  ----------       ----------   --  -------------------------------------------
   *  d_timeout_enable  SYS_UWORD8   I  Enable timeout
   *  d_timeout         SYS_UWORD8   I  Timeout value
   *  
   * RETURN VALUE: 0 for success
   *
   *****************************************************************************/
  SYS_UWORD8 f_emif_abort_conf(SYS_UWORD8  d_timeout_enable, 
		  SYS_UWORD8  d_timeout) {
    
    C_EMIF_TIMEOUT_REG = d_timeout << C_EMIF_TIMEOUT_VALUE_POS | 
	    d_timeout_enable << C_EMIF_TIMEOUT_EN_POS;
    
    return 0;
  }

   /******************************************************************************
   *
   *  FUNCTION NAME: f_emif_abort_status
   *    Gets the abort status.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument        Type             IO  Description
   *  ----------     ----------        --  -------------------------------------------
   *  p_abort_status T_ABORT_STATUS*    O   Abort Status structure
   *  
   * RETURN VALUE: 0 for success
   *
   *****************************************************************************/
  SYS_UWORD8 f_emif_abort_status (T_ABORT_STATUS* p_abort_status)  {
    
    SYS_UWORD32 d_emif_abort_status;
    
    d_emif_abort_status = C_EMIF_ABORT_TYPE_REG;

    p_abort_status->d_abort_state = d_emif_abort_status & C_EMIF_ABORT_TYPE_BIT;
    p_abort_status->d_abort_host = (d_emif_abort_status & C_EMIF_HOST_ID_BIT )\
				   >> C_EMIF_HOST_ID_POS;

    p_abort_status->d_abort_protect = (d_emif_abort_status & C_EMIF_PROTECT_MODE_BIT ) >> C_EMIF_PROTECT_MODE_POS;

    p_abort_status->d_abort_timeout = (d_emif_abort_status & C_EMIF_TIMEOUT_ERR_BIT ) >> C_EMIF_TIMEOUT_ERR_POS;

    p_abort_status->d_abort_address = C_EMIF_ABORT_ADDR_REG;
	    
    return 0;   	
   }

   /******************************************************************************
   *
   *  FUNCTION NAME: f_emif_protect_conf
   *    Protect Configuration.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument         Type         IO  Description
   *  ----------      ----------    --  -------------------------------------------
   *  d_bound_address  SYS_UWORD32   I  Bound address
   *  d_protect_cs     SYS_UWORD32   I  Protect chip select
   *  d_protect_mask   SYS_UWORD32   I  Protect mask
   *  
   * RETURN VALUE: 0 for success
   *
   *****************************************************************************/

  SYS_UWORD8 f_emif_protect_conf (SYS_UWORD32 d_bound_address, 
		  SYS_UWORD32 d_protect_cs, 
		  SYS_UWORD32 d_protect_mask)	{    

	/* This is being done as 32 bits access to address not in WORD 
	 *  boundary fails (32 bits).
	 */
    C_EMIF_BOUND_REG = (SYS_UWORD16) (d_bound_address & 0xffff);
	C_EMIF_BOUND_REG1 = (SYS_UWORD16) (d_bound_address & 0x3ffff) >> 16;//C_EMIF_PROTECT_BOUND_MASK);
	
    C_EMIF_MASK_REG = d_protect_mask << C_EMIF_MASK_ADDR_POS |
	    d_protect_cs << C_EMIF_PROTECT_CS_POS;
    
    return 0;
  }

   /******************************************************************************
   *
   *  FUNCTION NAME: f_emif_protect_enable
   *    Enable/Disable protect.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument     Type         IO  Description
   *  ----------   ----------   --  -------------------------------------------
   *  enable       SYS_UWORD8    I  Enable/Disable protect
   *  
   * RETURN VALUE: 0 for success
   *
   *****************************************************************************/
  SYS_UWORD8 f_emif_protect_enable (SYS_UWORD8 enable) {
    
    C_EMIF_PROTECT_MODE_REG = enable;

    return 0; 
  }

   /******************************************************************************
   *
   *  FUNCTION NAME: f_emif_api_rhea_conf
   *    Enable/Disable protect.
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument                      Type        IO  Description
   *  ----------                   ----------   --  -------------------------------------------
   *  strobe0_access_size_adapt    SYS_UWORD8    I  Enable/Disable strobe 0 access size adaptation
   *  strobe1_access_size_adapt    SYS_UWORD8    I  Enable/Disable strobe 1 access size adaptation
   *  api_access_size_adapt        SYS_UWORD8    I  Enable/Disable API access size adaptation
   *  debug_enable                 SYS_UWORD8    I  Enable/Disable ARM Debug		
   *
   * RETURN VALUE: 0 for success
   *
   *****************************************************************************/
   SYS_UWORD8 f_emif_api_rhea_conf (SYS_UWORD8 strobe0_access_size_adapt,
  			SYS_UWORD8 strobe1_access_size_adapt,
  			SYS_UWORD8 api_access_size_adapt,
  			SYS_UWORD8 debug_enable) {  
   C_EMIF_API_RHEA_CTRL_REG = strobe0_access_size_adapt << C_RHEA_STROBE0_ACCESS_SIZE_ADAPT_POS |
   				strobe1_access_size_adapt << C_RHEA_STROBE1_ACCESS_SIZE_ADAPT_POS |
   				api_access_size_adapt << C_API_ACCESS_SIZE_ADAPT_POS |
   				debug_enable << C_EMIF_DEBUG_ENABLE_POS;
    return 0; 
  }

   /******************************************************************************
   *
   *  FUNCTION NAME: f_emif_boot_mode_conf
   *    Boot mode configuration
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument               Type                IO   Description
   *  ----------          ----------     --  -------------------------------------------
   *  boot_ctrl               SYS_UWORD8    I   Gives whether the boot should be 
   *									performed from internal memory or external memory
   *  secure_mem_select SYS_UWORD8  I   Secure memory is MCU ROM/Internal RAM
   *
   * RETURN VALUE: 0 for success
   *
   *****************************************************************************/
  SYS_UWORD8 f_emif_boot_mode_conf (SYS_UWORD8 boot_ctrl, SYS_UWORD8 secure_mem_select) {
       SYS_UWORD8 register_value;
	   
    	register_value = C_EMIF_BOOT_MODE_REG;

	C_EMIF_BOOT_MODE_REG = (register_value &~(C_EMIF_BOOT_CTRL_BIT | C_EMIF_SECURE_MEM_TYPE_BIT)) | 
		(boot_ctrl << C_EMIF_BOOT_CTRL_POS) | 
		(secure_mem_select << C_EMIF_SECURE_MEM_TYPE_POS);

    return 0; 
  }

    /******************************************************************************
   *
   *  FUNCTION NAME: f_emif_debug_unit_enable
   *    Debug unit disable/enable
   *
   *
   *  ARGUMENT LIST:
   *
   *  Argument               Type                IO   Description
   *  ----------          ----------     --  -------------------------------------------
   *  enable               SYS_UWORD8    I      Enables or disables debug unit using 
   *									C_EMIF_ENABLE_DU and C_EMIF_DISABLE_DU
   * 
   * RETURN VALUE: 0 for success
   *
   *****************************************************************************/
  SYS_UWORD8 f_emif_debug_unit_enable (SYS_UWORD8 enable) {
       SYS_UWORD8 register_value;

	register_value = C_EMIF_BOOT_MODE_REG;
  
  	 C_EMIF_BOOT_MODE_REG = (register_value & ~C_EMIF_DEBUG_UNIT_BIT) |
	 	enable << C_EMIF_DEBUG_UNIT_POS;
	return 0; 	
  }





#pragma CODE_SECTION(disable_ps_ram_burst, ".emifconf")
#pragma CODE_SECTION(enable_ps_ram_burst, ".emifconf")
#pragma CODE_SECTION(disable_flash_burst, ".emifconf")
  

  #pragma CODE_SECTION(enable_burst_flash, ".emifconf")
  #pragma CODE_SECTION(enable_flash_burst_mirror, ".emifconf")
  #pragma CODE_SECTION(lld_SetConfigRegCmd, ".emifconf")


extern SYS_UWORD16 flash_device_id;

void disable_ps_ram_burst ()
{
	SET_EMIF_NO_DYN_WAIT(0, 1); // '1' = Dynamic wait state disabled
	SET_EMIF_CFG(0, 2, 0, 1, 2, 3, 0, 1); //	SET_EMIF_CFG(CS, BTWST, MEMMODE, WELEN, WRWST, RDWST, RT, FCLKDIV)
	SET_EMIF_ADVANCED_CFG(0, 0, 0, 0, 0, 0, 2);//	SET_EMIF_ADVANCED_CFG(CS, RC, CLKMSK, BTMODE, ADVHOLD, OEHOLD, OESETUP)
	// Prefetch OFF
	SET_EMIF_CONF_REG(0, 1, 0, 1, 1); //	(WP, FLUSH_PREFETCH, PREFETCH_MODE, PDE, PWD_EN)

	MEM16 (PSRAM_OFFSET + 0x007FFFF0); // Unlock sequence
	MEM16 (PSRAM_OFFSET + 0x007FFFFE);
	MEM16 (PSRAM_OFFSET + 0x007FFFFE);

       // Due to special handling of the write-strobe, it is important that this code runs undisturbed and from internal RAM
	SET_EMIF_CONF_REG(0, 1, 0, 1, 1); //	(WP, FLUSH_PREFETCH, PREFETCH_MODE, PDE, PWD_EN)
	MEM16 (PSRAM_OFFSET + 0x007FFFFE) = 0x0001;
	MEM16 (PSRAM_OFFSET + 0x007FFFFE) = 0x9d4f;	// Default async
}

void enable_ps_ram_burst ()
{
	disable_ps_ram_burst ();

	// Configure the psRAM device for burst operation:
	MEM16 (PSRAM_OFFSET + 0x007FFFF0);
	MEM16 (PSRAM_OFFSET + 0x007FFFFE);
	MEM16 (PSRAM_OFFSET + 0x007FFFFE);
       // Due to special handling of the write-strobe, it is important that this code runs undisturbed and from internal RAM
	SET_EMIF_CONF_REG(0, 1, 0, 0, 0); //	(WP, FLUSH_PREFETCH, PREFETCH_MODE, PDE, PWD_EN)
	MEM16 (PSRAM_OFFSET + 0x007FFFFE) = 0x0001;
	MEM16 (PSRAM_OFFSET + 0x007FFFFE) = 
		(PSRAM_OPERATING_MODE << 15) | 
		(PSRAM_LATENCY_COUNTER << 11) | 
		(PSRAM_WAIT_POLARITY << 10) | 
		(PSRAM_WAIT_CONFIGURATION << 8) | 
		(PSRAM_CLOCK_CONFIGURATION << 6) | 
		(PSRAM_OUTPUT_IMPEDANCE << 5) | 
		(PSRAM_BURST_WRAP << 3) | 
		(PSRAM_BURST_LENGTH);	// Enable psRAM Burst mode


	// Modified by Ranga to check if Instruction and Data prefetch works
	SET_EMIF_CONF_REG(0, 0, 0, 1, 1); //	(WP, FLUSH_PREFETCH, PREFETCH_MODE, PDE, PWD_EN)
	SET_EMIF_NO_DYN_WAIT(0, 0); // '0' = Dynamic wait state enabled
	SET_EMIF_ADVANCED_CFG(0, 0, 0, 0, 0, 0, 2);//	SET_EMIF_ADVANCED_CFG(CS, RC, CLKMSK, BTMODE, ADVHOLD, OEHOLD, OESETUP)
	SET_EMIF_CFG(0, 1, 2, 2, 0, 3, 0, 1); //	SET_EMIF_CFG(CS, BTWST, MEMMODE, WELEN, WRWST, RDWST, RT, FCLKDIV)

	SET_EMIF_CONF_REG(0, 0, 0, 0, 0); //	(WP, FLUSH_PREFETCH, PREFETCH_MODE, PDE, PWD_EN)
}

// Switch to asynchronous mode
void disable_flash_burst ()
{
	SET_EMIF_CONF_REG(0, 0, 0, 0, 0); //	(WP, FLUSH_PREFETCH, PREFETCH_MODE, PDE, PWD_EN)
	SET_EMIF_NO_DYN_WAIT(3, 1); // '1' = Dynamic wait state disabled
	SET_EMIF_CFG(3, 2, 0, 1, 2, 3, 0, 1); //	SET_EMIF_CFG(CS, BTWST, MEMMODE, WELEN, WRWST, RDWST, RT, FCLKDIV)
	SET_EMIF_ADVANCED_CFG(3, 1, 0, 0, 0, 0, 2);//	SET_EMIF_ADVANCED_CFG(CS, RC, CLKMSK, BTMODE, ADVHOLD, OEHOLD, OESETUP)
}

void enable_flash_burst_fg ()
{
	MEM16 (0xFFFEF19C) |= 0x0001;	//  Enable A22 muxing
	MEM16 (0xFFFEF1A2) |= 0x0001;	//  Enable CKM pin-muxing
	MEM16 (0xFFFEF19E) |= 0x0018;	//  Enable nRDY pull-up


	disable_flash_burst ();
	// Configure the FLASH device for burst operation:
	MEM16 (0x800000+ FLASH_OFFSET_ADR) = 0xFFFF;
	MEM16 (0x800000+ FLASH_OFFSET_ADR + 0x555 * 2) = 0xaa;
	MEM16 (0x800000+ FLASH_OFFSET_ADR + 0x2AA * 2) = 0x55;
	MEM16 (0x800000+ FLASH_OFFSET_ADR + 0xb555 * 2) = 0xc0; // Continous

	// Configure EMIF for burst operation:
	MEM16 (0xfffffb02) = 0x00;
	MEM16 (0xfffffb1c) &= 0x07;
	MEM16 (0xfffffb1a) = 0x0802;
	MEM16 (0xfffffb10) = 0x1231;
	MEM16 (0xfffffb12) = 0x0081;



	
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");

	MEM16 (0xfffffb02) = 0x00;
	MEM16 (0xfffffb02) = 0x10;


    asm(" NOP");
    asm(" NOP");
    asm(" NOP");
    asm(" NOP");
	MEM16 (0xfffffb02) = 0x13;

    asm(" NOP");
    asm(" NOP");
    asm(" NOP");
    asm(" NOP");
}










void lld_SetConfigRegCmd
(
FLASHDATA *   base_addr,
FLASHDATA value
)
{
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, WSXXX_SET_CONFIG_CMD);
  FLASH_WR(base_addr, 0, value);
}


void enable_flash_burst_mirror ()
{
	MEM16 (0xFFFEF19C) |= 0x0001;	//  Enable A22 muxing
	MEM16 (0xFFFEF1A2) |= 0x0001;	//  Enable CKM pin-muxing
	MEM16 (0xFFFEF19E) |= 0x0018;	//  Enable nRDY pull-up
	disable_flash_burst ();
	// Configure the FLASH device for burst operation:

// from LLD 
	lld_SetConfigRegCmd((FLASHDATA *) FLASH_OFFSET_ADR,
		 (FLASH_WAIT_STATE << 11) |
		 (FLASH_RDY_POLARITY << 10) | (1 << 9) |
		 (FLASH_RDY << 8) | (1 << 7) | (1 << 6) |
		 (FLASH_BURST_WRAP << 3) |
		 FLASH_BURST_LENGTH);

	// Configure EMIF for burst operation:
	SET_EMIF_CONF_REG(0, 0, 0, 0, 0); //	(WP, FLUSH_PREFETCH, PREFETCH_MODE, PDE, PWD_EN)
	SET_EMIF_NO_DYN_WAIT(3, 0); 
	SET_EMIF_ADVANCED_CFG(3, 1, 0, 0, 0, 0, 2);//	SET_EMIF_ADVANCED_CFG(CS, RC, CLKMSK, BTMODE, ADVHOLD, OEHOLD, OESETUP)
	SET_EMIF_CFG(3, 2, 1, 1, 2, 3, 0, 1); //	SET_EMIF_CFG(CS, BTWST, MEMMODE, WELEN, WRWST, RDWST, RT, FCLKDIV)

// For sibley
	SET_EMIF_ADVANCED_CFG(2, 0, 0, 0, 1, 0, 1);//	SET_EMIF_ADVANCED_CFG(CS, RC, CLKMSK, BTMODE, ADVHOLD, OEHOLD, OESETUP)
	SET_EMIF_CFG(2, 0, 0, 0x2, 0x1, 0x3, 0, 1); //	SET_EMIF_CFG(CS, BTWST, MEMMODE, WELEN, WRWST, RDWST, RT, FCLKDIV)





	    asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
	
	MEM16 (0xfffffb02) = 0x00;
	MEM16 (0xfffffb02) = 0x13;

	 asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
         asm(" NOP");
	
}


void enable_burst_flash()
{
  void (*burst_enable)(void);
  if(0x7E == flash_device_id)
  {
    burst_enable = enable_flash_burst_fg;
  }
  else
  {
   burst_enable =  enable_flash_burst_mirror;
  }
  (*burst_enable)();

}



  
#endif /* ( CHIPSET == 15 ) */


