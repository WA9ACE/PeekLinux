/************* Revision Controle System Header *************
 *
 *                  GSM Layer 1 software
 * DRP_DRIVE.H
 *
 *        Filename drp_drive.h
 *        Version 0.1
 *        Date     09-14-2004
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#ifndef _DRP_DRIVE_

#define _DRP_DRIVE_

//#include "l1sw.cfg"
#include "sys.cfg"
#include "l1_confg.h"


#define DRP_API_BASE_ADDRESS 0xFFFF0000

// 1.2 [2]
#define ARM_DRP_OCP2_BASE_ADDRESS 0xFFFF0000 
#define ARM_DRP_OCP2_END_ADDRESS  0xFFFF3FFF 


#define ARM_DRP_WRAP_INT_MEM_BASE_ADDRESS 0xFFFF4000
#define ARM_DRP_WRAP_INT_MEM_END_ADDRESS  0xFFFF4FFF

#define DRP_WRAP_INT_MEM_BASE_ADDRESS 0x00010000
// EXT_BASE_ADDR + 4Kb DRP Wrapper Mem size
#define DRP_WRAP_INT_MEM_END_ADDRESS  0x00010FFF 


#define ARM_DRP_SIMU_BASE_ADDRESS 0x3100000


/* DRP Script Related */

#define DRP_SCR_ENABLE_MASK (0xFFFFFF00)
#define DRP_SCRIPT_START_MASK (0x80)
#define DRP_SCR_DISABLE_MASK (0xFFFFFF3F)

#define DRP_SCR_START_ADDR 0x0500


#define DRP_REG_ON_SCRIPT 0
#define DRP_TX_ON_SCRIPT   1
#define  DRP_RX_ON_SCRIPT  2
#define DRP_ROC_SCRIPT       4
#define DRP_IDLE_SCRIPT      15
#define DRP_AFC_SCRIPT       13
#define DRP_REG_OFF_SCRIPT 7
#define DRP_TEMP_MEAS_SCRIPT 3

#define DRP_REG_SRM_CW_ADDR 0x1E00
#define DRP_REG_IRQ_CNT_ADDR 0x1E20
#define DRP_REG_RX_PTR_START_END_ADDR  0x1E18
#define DRP_REG_TX_PTR_START_END_ADDR  0x1E1C


#define DRP_RX_REG_DATA 0x1E0C
#define DRP_TX_REG_DATA 0x1E10 


#define DRP_DBB_RX_IRQ_MASK (0xFFFE0806)

/* The following #defines are required for use of DRP. They need to be updated as and when the 
     DRP API Documents are updated.*/

/*-----------------**
** SCRIPTS CONTROL **
**-----------------*/

/*  Scripts Numbers Definition */

#define DRP_REG_ON         (0x0000)

#define DRP_TX_ON           (0x0001)
#define DRP_TX_ON_LB            (0x0001)
#define DRP_TX_ON_HB            (0x0001)

#define DRP_RX_ON                  (0x0002)
#define DRP_RX_ON_EGSM       (0x0002)
#define DRP_RX_ON_GSM         (0x0002)
#define DRP_RX_ON_DCS          (0x0002)
#define DRP_RX_ON_PCS          (0x0002)

#define DRP_TEMP_CONV   (0x0003)
#define DRP_REG_OFF        (0x0007)

#define DRP_AFC                (0x000D)   /* TBD */
#define DRP_IDLE               (0x000F)

//#define DRP_ROC                (0xFFFF)   /* TBD */

//#define SCRIPT_EN             (0x0080)

/* Macros Definition */

#define START_SCRIPT(script_nb) (SCRIPT_EN  | script_nb)
#define STOP_SCRIPT(script_nb)  (!(SCRIPT_EN) & script_nb)


/*-------------------------**
** AGC Settings Definition **
**-------------------------*/

/* Gain Compensation Enable/Disable */

#define GAIN_COMP_DISABLE    (0x0)  //Default
#define GAIN_COMP_ENABLE   (0x1)


/* AFE Gains Definition */

#define AFE_HIGH_GAIN     (0x1)       // 38 dB
#define AFE_LOW_GAIN      (0x0)       // 11 dB

/* ABE Gains Definition */

#define ABE_0_DB          (0x0)
#define ABE_2_DB          (0x1)
#define ABE_5_DB          (0x2)
#define ABE_8_DB          (0x3)
#define ABE_11_DB         (0x4)
#define ABE_14_DB         (0x5)
#define ABE_17_DB         (0x6)
#define ABE_20_DB         (0x7)

/* Macros Definition */

#define SET_AGC(afe_gain, abe_gain) ((afe_gain<<3) | abe_gain)

/* Switched Cap Filter Corner Freq Defition */

//#define SCF_170KHZ 0
//#define SCF_270KHZ 1

/*------------------**
** RETIMING CONTROL **
**------------------*/

#define RETIM_DISABLE          (0x0000)
#define RETIM_TX_ONLY          (0x0001)
#define RETIM_RX_ONLY          (0x0002)
#define RETIM_FULL             (0x0003)

/* ******** */


/**************************************/
/* Prototypes for DRP API Services                */
/**************************************/
SYS_WORD16 drp_ref_sw_upload(const SYS_UWORD8 sw[] );

#if (OP_L1_STANDALONE == 0)
SYS_WORD16 drp_ref_sw_upload_from_ffs(const SYS_UWORD8* pathname );
SYS_WORD16 drp_ref_sw_upload_to_ffs(const SYS_UWORD8* pathname );
#endif

#if (DRP_FW_BUILD == 1)
void drp_ver_efuse_read(T_DRP_SW_DATA * ptr_drp_sw_data_calib);
#endif

#if (DRP_MEM_SIMULATION == 1)

  #if (BOARD == 43)
    #if (CHIPSET == 12)

         // See l1_$(CHIPSET)_$(BOARD)_$(FLASH)_$(LONG_JUMP)$(SECURITY_EXT).template file
         // and memory map of the chipset to select the base address for simulation

        #define ARM_DRP_MEM_SIMU_BASE_ADDR 0x03100000

        // OCP Script register start address
        #define DRP_SCR_REG_START       0x0508   // starting adress for the data scenario.

        // OCP Script register last address
        #define DRP_SCR_REG_END            0x0544

        // OCP SRM start address for scripts is 2600
        #define DRP_SRM_START       0x2000   // starting adress for the data scenario.

        // OCP SRM last address for scripts is 29FF
        #define DRP_SRM_END            0x2FFF

        // OCP SRM size for script is 256 * 32
        #define DRP_SRM_SIZE        DRP_SRM_END  - DRP_SRM_START + 1

        #define REF_SW_TO_DRP_SIMU_ADDR(addr) (ARM_DRP_MEM_SIMU_BASE_ADDR + addr)
        
    #endif
   #endif
   
#else
      #define ARM_DRP_MEM_SIMU_BASE_ADDR 0x0
#endif


/************************************************************/
/* Macros for OCP to RHEA address conversion                                     */
/************************************************************/

//DRP2 Memory Map is as follows , Ref
// [1] : DRP2.0_Module_Specification_v4p1.pdf
// [2] : Address_int_dma_map_V_06.doc

/* Required Mapping

   --------------   -------------------->   -------------------
  | FFFF:0000    |                         |0000:0000         |
  |              |                         | SRM Mem          |
  |  ARM_addr    |                         |                  |
  | FFFF:3FFF    |--------------------->   _0000:3FFF---------
  | FFFF:4000    |------------------
  |              |                 |         Reserved Mem:Hole
  |              |                 ------> -------------------  0001:0000
  | FFFF:4FFF    |                         |  Wrap Mem        |
  |------------                            |  = 4K            |  
                                           ------------------- 0001:0FFF         

*/

#if (DRP_MEM_SIMULATION == 0) //DRP 

#define API_ADDRESS_ARM2DRP(arm_address)                            \
     ( (arm_address) <= (ARM_DRP_OCP2_END_ADDRESS )  ?           \
         ( (arm_address) - ARM_DRP_OCP2_BASE_ADDRESS)               \
       :\
       (DRP_WRAP_INT_MEM_BASE_ADDRESS + ( (arm_address) - ARM_DRP_OCP2_BASE_ADDRESS) )   


#define API_ADDRESS_DRP2ARM(drp_address)  ( (drp_address)>=DRP_WRAP_INT_MEM_BASE_ADDRESS) ? ( (drp_address)-DRP_WRAP_INT_MEM_BASE_ADDRESS+ ARM_DRP_WRAP_INT_MEM_BASE_ADDRESS):( (drp_address)+ARM_DRP_OCP2_BASE_ADDRESS)

#else //Simulation, on Cal+ 0x3100000 to 0x4000000 on CS4 is Available currently 

#define API_ADDRESS_ARM2DRP(arm_address)      \
     (  ( arm_address - DRP_API_BASE_ADDRESS) + ARM_DRP_MEM_SIMU_BASE_ADDR )

     
// Dont care about the Hole in the ARM Side 

#define API_ADDRESS_DRP2ARM(drp_address)         \
       ( drp_address + ARM_DRP_SIMU_BASE_ADDRESS)  

 #endif

#endif

