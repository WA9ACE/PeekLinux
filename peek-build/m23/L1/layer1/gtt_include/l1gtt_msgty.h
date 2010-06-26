/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * L1GTT_MSGTY.H
 *
 *        Filename l1gtt_msgty.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

#ifndef _L1GTT_MSGTY_H
#define _L1GTT_MSGTY_H

#if (L1_GTT == 1)

#include "l1sw.cfg"

#if (OP_RIV_AUDIO == 1)
  #include "rv_general.h"
#endif


typedef struct
{
  #if (OP_RIV_AUDIO == 1)
    T_RV_HDR  header;
  #endif  
  UWORD8 keyboard_rate;  // Type of keyboard used: 45.45bps (0) or 50bps (1) - NOT USED at the moment
  UWORD8 tty_mode; // 0-NO_TTY, 1-TTY_VCO, 2-TTY_HCO, 3-TTY_ALL
}
T_MMI_GTT_START_REQ;

typedef struct
{
  UWORD8 loop_type;  // Type of loop to be closed
}
T_TST_CLOSE_GTT_LOOP_REQ;

typedef struct
{
  UWORD8  scen_type;  // 1 = input on CTM Tx, 2 = input on Baudot encoder
                      // SIMULATION ONLY: 3 = input on CTM Rx, 4 = input on Baudot decoder
  UWORD16 rate;       // Number of frames (4.615 ms) between each new character arrival
                      // for scen_type 3 and 4: GTT input file number
}
T_TST_START_GTT_TEST_REQ;

#endif // L1_GTT

#endif
