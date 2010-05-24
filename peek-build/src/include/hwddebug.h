#ifndef _HWDDEBUG_H_
#define _HWDDEBUG_H_


/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/
#include "sysdefs.h"

#include "hwdapi.h"
#include "hwddefs.h"

/*----------------------------------------------------------------------------
 Global Defines and Macros
----------------------------------------------------------------------------*/


/*-------------**
** Led Control **
**-------------*/
#define HwdLedOff(LedMask) \
   HwdWrite(HWD_LED_CTRL, (HwdDbgLedStatus |= (LedMask) ))

#define HwdLedOn(LedMask) \
   HwdWrite(HWD_LED_CTRL, (HwdDbgLedStatus &= ~(LedMask) ))

#define HwdLedToggle(LedMask) \
   HwdWrite(HWD_LED_CTRL, (HwdDbgLedStatus ^= (LedMask) ))


/*------------------**
** Test Pin Control **
**------------------*/
#define HwdPinSet(PinMask)                                     \
   HwdDbgPinStatus |= (PinMask);                               \
   HwdWrite(HWD_CP_TST1, (0x0000ffff & HwdDbgPinStatus));      \
   HwdWrite(HWD_CP_TST2, (0xffff0000 & HwdDbgPinStatus) >> 16)

#define HwdPinClear(PinMask)                                   \
   HwdDbgPinStatus &= ~(PinMask);                              \
   HwdWrite(HWD_CP_TST1, (0x0000ffff & HwdDbgPinStatus));      \
   HwdWrite(HWD_CP_TST2, (0xffff0000 & HwdDbgPinStatus) >> 16)

#define HwdPinToggle(PinMask)                                  \
   HwdDbgPinStatus ^= (PinMask);                               \
   HwdWrite(HWD_CP_TST1, (0x0000ffff & HwdDbgPinStatus));      \
   HwdWrite(HWD_CP_TST2, (0xffff0000 & HwdDbgPinStatus) >> 16)

/* HwdPinProgram() programs a specified value to the specified portion of the debug word.
 * NumBits  - Specifies the data size, the number of bits to be programmed (all other bits unmodified)
 * Data     - Data to be written
 * Shift    - Specifies where within the debug word the data shall be written
 */
#define HwdPinProgram(Data, NumBits, ShiftLeft)                                          \
   HwdDbgPinStatus = (uint32 ) (   (HwdDbgPinStatus & ~(((1<<NumBits)-1) << ShiftLeft))  \
                                | ((Data & ((1<<NumBits)-1)) << ShiftLeft)               \
                              );                                                         \
   HwdWrite(HWD_CP_TST1, (0x0000ffff & HwdDbgPinStatus));                                \
   HwdWrite(HWD_CP_TST2, (0xffff0000 & HwdDbgPinStatus) >> 16)


/* -------------------------------- */
/* Test pin debug modes and pin use */
/* -------------------------------- */
/* NOTE THAT THESE PINS MAY OR MAY NOT WORK DEPENDING ON HW CONFIGURATION
 * BB2    : pins 0:2 are HW dedicated, pins 16:31 are not available.
 * CDS1.75: all 32 pins are available as of 4/21/99
 */
/*  permanent pins:               15    - CTS0 toggle, see syscts.c
 *                                14    - CTS1 toggle
 *                                13    - CTS2 toggle
 *
 *  #ifdef L1D_DEBUG_TIM_CHANGE   12:10 - Timing change state, see l1dtimch.c
 *
 *  #ifdef SYS_DEBUG_TIME_EXT     27:16 - extended system time, 12 LSBs (20ms or 26.6ms count), see systime.c
 *
 *  #ifdef SYS_DEBUG_TIME         9:5   - system time 5 LSBs (20ms or 26.6ms count), see systime.c
 *
 *  #ifdef SYS_DEBUG_TIME_BR      9:5   - bit reversed version of SYS_DEBUG_TIME, see systime.c
 *
 *  #ifdef L1D_DEBUG_RF_TRAN      2:0   - RF transition state, 3LSBs (-1 = 0x7), see l1drftrn.c
 *
 *  #ifdef L1D_DEBUG_FMBOX_ISRS   4     - inside pwr meas ISR indication, see l1dagc.c
 *
 *  #ifdef SYS_DEBUG_SYNC_PULSE   4     - generate sync pulse at (SysTime&0xfff) == 0xfff, see systime.c
 *
 *  #ifdef L1D_DEBUG_SLOTTED      12:10,3 - slotted paging state, see l1dspage.c
 *
 *  #ifdef L1D_DEBUG_SLOTTED_BR   12:10,3 - bit reversed version of L1D_DEBUG_SLOTTED, see l1dspage.c
 */

/*----------------**
** Switch Control **
**----------------*/
/*----------------**
** Switch Control **
**----------------*/

#if (SYS_BOARD == SB_CDS4)
/* The following defines the switches used to determine if the DSPM/V should
   be downloaded or not.  Note these are NOT defined if SYS_BOARD = SB_NONE since
   no switches exist. */

#define HwdSwitchRead()             HwdRead(HWD_SW_CFG)

/* DLD_CFG_READ=TRUE means download is disabled */
#define HWD_DSPM_DLD_CFG_READ       (HwdSwitchRead() & HWD_SWITCH_2)
#define HWD_DSPV_DLD_CFG_READ       (HwdSwitchRead() & HWD_SWITCH_3)

#endif

/*----------------------------------------------------------------------------
 Global Typedefs
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Data
----------------------------------------------------------------------------*/
extern uint16 HwdDbgLedStatus;
extern uint32 HwdDbgPinStatus;

/*----------------------------------------------------------------------------
 Global Function Prototypes
----------------------------------------------------------------------------*/




#endif
