#ifndef __APC_H__
#define __APC_H__

#include "sys_types.h"

#ifndef _WINDOWS

#if (CHIPSET == 15)


  // APC SWITCH Related Definitions

  #define APC_RHEA_SWITCH_ADDRESS      (0xFFFF8800)
  #define APC_RHEA_SWITCH_REG          ((volatile SYS_UWORD16 *)(APC_RHEA_SWITCH_ADDRESS))

  #define APC_RHEA_SWITCH_MCU_MASK     (0x1)
  #define APC_RHEA_SWITCH_MCU_ACCESS   (0x1)
  #define APC_RHEA_SWITCH_DSP_MASK     (0x2)
  #define APC_RHEA_SWITCH_DSP_ACCESS   (0x2)
  #define APC_RHEA_SWITCH_MCU_GET      (APC_RHEA_SWITCH_REG |= APC_RHEA_SWITCH_MCU_ACCESS)
  #define APC_RHEA_SWITCH_MCU_RELEASE  (APC_RHEA_SWITCH_REG &= (~(APC_RHEA_SWITCH_MCU_ACCESS)))

  // APC Related Definitions

  #define APC_BASE_ADDRESS               0xFFFF5000

  #define APC_APCLEV_REG           (volatile SYS_UWORD16 *)(APC_BASE_ADDRESS + 0x2)
  #define APC_APCRAM_REG           (volatile SYS_UWORD16 *)(APC_BASE_ADDRESS + 0x4)
  #define APC_APCDEL1_REG          (volatile SYS_UWORD16 *)(APC_BASE_ADDRESS + 0x6)
  #define APC_APCDEL2_REG          (volatile SYS_UWORD16 *)(APC_BASE_ADDRESS + 0x8)
  #define APC_APCCTRL1_REG         (volatile SYS_UWORD16 *)(APC_BASE_ADDRESS + 0xA)
  #define APC_APCCTRL2_REG         (volatile SYS_UWORD16 *)(APC_BASE_ADDRESS + 0xC)
  #define APC_APCOUT_REG           (volatile SYS_UWORD16 *)(APC_BASE_ADDRESS + 0xE)

  // Band Gap  control register description
  //==========================================

  #define APC_BGEN_EN                 0x0100   // Band Gap on
  #define APC_APC_MODE                0x0001

  // APC_BAND_GAP_ON: Switch on Band Gap
  //=====================================
  #define APC_BAND_GAP_ON ( (*APC_APCCTRL2_REG) |= (APC_BGEN_EN) )

  // APC_BAND_GAP_OFF: Swithc off Band Gap
  //=====================================
  #define APC_BAND_GAP_OFF  ( (*APC_APCCTRL2_REG) &= ~(APC_BGEN_EN) )

  // APC_APCCTRL2_Write: Write APCOFFSET
  //=====================================
  #define APC_APCCTRL2_WRITE(apcctrl2)  ( (*APC_APCCTRL2_REG) = (apcctrl2) )

  // APC_APCLEV_Write: Write APCLEV
  //=====================================
  #define APC_APCLEV_WRITE(apclev) ( (*APC_APCLEV_REG)  = (apclev) )

  // APC_APCDEL1_Write: Write APCDEL1
  //=====================================
  #define APC_APCDEL1_WRITE(apcdel1) ( (*APC_APCDEL1_REG) = (apcdel1) )

  // APC_APCDEL2_Write: Write APCDEL2
  //=====================================
  #define APC_APCDEL2_WRITE(apcdel2) ( (*APC_APCDEL2_REG) = (apcdel2) )

  // APC_SET_MANUAL_MODE: set APC in manual mode
  //===============================================
  #define APC_SET_MANUAL_MODE ( (*APC_APCCTRL2_REG)  |= (APC_APC_MODE) )

  // APC_SET_AUTOMATIC_MODE: set APC in automatic mode
  //===================================================
  #define APC_SET_AUTOMATIC_MODE ( (* APC_APCCTRL2_REG)   &=  ~(APC_APC_MODE) )

  #else // _WINDOWS

  #endif // CHIPSET15

  #endif

#endif // __APC_H__
