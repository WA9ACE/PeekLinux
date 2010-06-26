/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * 
 *
 *        Filename l1p_macr.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

#if (OP_L1_STANDALONE == 0)
  // First undefine the macros to avoid compilation warnings
  #undef Min
  #undef Max
#endif

// Max(x,y) definition
#define Max(x,y)  ( (x) > (y) ? (x) : (y) )

// Min(x,y) definition
#define Min(x,y)  ( (x) < (y) ? (x) : (y) )
