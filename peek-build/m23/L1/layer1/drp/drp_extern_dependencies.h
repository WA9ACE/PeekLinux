/**********************************************************************************
drp_defines.h
-- Copyright (c) 2005, Texas Instruments, Inc.
-- Author: Shrinivas Gadkari

External dependencies go here

***********************************************************************************
                               Change History
************************************************************************************

Version     Date           Author(s)        Comments
-------------------------------------------------------------------------------------
Ver 0.0     12 May 2005    SG               Initial creation

************************************************************************************/

#ifndef __DRP_EXTERN_DEPENDENCIES_H__
#define __DRP_EXTERN_DEPENDENCIES_H__

#include "drp_defines.h"

//#if (DRP_FILES_GENERATION_BUILD || DRP_STANDALONE_BUILD)
#if DRP_FILES_GENERATION_BUILD
#else

// External Dependencies on Layer 1
#include "clkm.h"
#if (DRP_FW_BUILD == 1)
#include "l1_types.h"
#endif
#include "sys_types.h"
//#include "general.h"

#if LOCOSTO_VALID
#else
#include "pld.h"
#endif

#endif // #if DRP_FILES_GENERATION_BUILD

/*-------------------------------**
** BASE SRM ADDRESSES OF DRP API **
**-------------------------------*/

/* Base address for DRP Regs, API, SRAM Data */

#if LOCOSTO_VALID
  #define DRP_REGS_BASE_ADD              (0xFFFF0000)
  #define DRP_REGS_BASE_OCP_ADD          (0x00000000)
  #define DRP_SRM_API_ADD                (DRP_REGS_BASE_ADD + DRP_SRM_API_OFF)
  #define DRP_SRM_DATA_ADD               (DRP_REGS_BASE_ADD + DRP_SRM_DATA_OFF)
  #define DRP_SRM_RX_BUFFER_ADDR         (DRP_REGS_BASE_ADD + DRP_RX_BUFFER_OFF)
  #define DRP_SRM_CALC_BUFFER_ADDR       (DRP_REGS_BASE_ADD + DRP_CALC_BUFFER_OFF)
  #define DRP_SRM_SW_DATA_COPY_ADDR      (DRP_REGS_BASE_ADD + DRP_SRM_SW_DATA_COPY_OFF)
  #define DRP_EXTERNAL_MEMORY_ADD        (0xFFFF4000)
  #define DRP_EXTERNAL_MEMORY_SIZE       (0x00001000)
  #define DRP_EXTERNAL_MEMORY_OCP_ADD    (0x00010000)
  //#define DRP_SRM_DATA_ADD               (DRP_EXTERNAL_MEMORY_ADD + DRP_SRM_DATA_OFF)
  //#define DRP_SRM_RX_BUFFER_ADDR         (DRP_EXTERNAL_MEMORY_ADD + DRP_RX_BUFFER_OFF)

  #define DRP_SRM_PHE_BUFFER_ADDR        (DRP_REGS_BASE_ADD + DRP_PHE_BUFFER_OFF)

  #define DRP_R0REG_OFFSET               (0x00000600)
  #define DRP_R1REG_OFFSET               (0x00000604)
  #define DRP_R2REG_OFFSET               (0x00000608)
  #define DRP_R3REG_OFFSET               (0x0000060C)
  #define DRP_R4REG_OFFSET               (0x00000610)
  #define DRP_R5REG_OFFSET               (0x00000614)
  #define DRP_R6REG_OFFSET               (0x00000618)
  #define DRP_R7REG_OFFSET               (0x0000061C)
  #define DRP_R8REG_OFFSET               (0x00000620)
  #define DRP_R9REG_OFFSET               (0x00000624)
  #define DRP_R10REG_OFFSET              (0x00000628)
  #define DRP_R11REG_OFFSET              (0x0000062C)
  #define DRP_R12REG_OFFSET              (0x00000630)
  #define DRP_R13REG_OFFSET              (0x00000634)
  #define DRP_R14REG_OFFSET              (0x00000638)
  #define DRP_R15REG_OFFSET              (0x0000063C)

#else
  #define DRP_REGS_BASE_ADD              (DRP_REGS_BASE_OFF)
  #define DRP_REGS_BASE_OCP_ADD          (DRP_REGS_BASE_OFF)
  #define DRP_SRM_API_ADD                (DRP_SRM_API_OFF)
  #define DRP_SRM_DATA_ADD               (DRP_SRM_DATA_OFF)
  #define DRP_EXTERNAL_MEMORY_ADD        (DRP_REGS_BASE_OFF)
  #define DRP_EXTERNAL_MEMORY_OCP_ADD    (DRP_REGS_BASE_OFF)
#endif

#if LOCOSTO_VALID

  #define WRITE_BYTE_LOW(a,b) a = (b & 0x00ff) | (a & 0xff00);
  #define WRITE_BYTE_HIGH(a,b) a = (b & 0xff00) | (a & 0x00ff);
  #define WRITE_WORD(a,b) a = b;
  #define WRITE_WORD_AT_PTR(a,b) *((volatile UINT16 *) a) = (UINT16) b;
  #define READ_WORD(a,b) b = a;

#else

  #define READ_WORD(a,b) b = PLD_ReadRegister((UWORD16)&(a)); \
           wait_ARM_cycles(convert_nanosec_to_cycles(5*1000));

  #define WRITE_WORD(a,b) PLD_WriteRegister((UWORD16)&(a),b)
  #define WRITE_WORD_AT_PTR(a,b) PLD_WriteRegister((UWORD16)a,b)


  #define WRITE_BYTE_LOW(a,b) macro_temp16 = 0xff00 & PLD_ReadRegister((UWORD16)&(a)); \
                              macro_temp16 |= (b & 0x00ff); \
                              PLD_WriteRegister((UWORD16)&(a),macro_temp16);

  #define WRITE_BYTE_HIGH(a,b) macro_temp16 = 0x00ff & PLD_ReadRegister((UWORD16)&(a)); \
                              macro_temp16 |= (b & 0xff00); \
                              PLD_WriteRegister((UWORD16)&(a),macro_temp16);

#endif

#if DRP_TEST_SW
//    #define WAIT_US(a) wait_ARM_cycles(convert_nanosec_to_cycles(a*2*1000));
//    #define wait_qb(a) wait_ARM_cycles(convert_nanosec_to_cycles(a*2*923));
    #define WAIT_US(a) arm_wait_us(a);
    #define wait_qb(a) arm_wait_us(a);
#else
    #define WAIT_US(a) wait_ARM_cycles(convert_nanosec_to_cycles(a*2*1000));
    #define wait_qb(a) wait_ARM_cycles(convert_nanosec_to_cycles(a*2*923));
#endif


#endif //#ifndef __DRP_EXTERN_DEPENDENCIES_H__

