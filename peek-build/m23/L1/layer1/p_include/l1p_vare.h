/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * L1P_VAREX.H
 *
 *        Filename l1p_vare.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

#ifdef L1P_ASYN_C

#if (LONG_JUMP == 3)
 #pragma DATA_SECTION(l1ps,".l1s_global")
 #pragma DATA_SECTION(l1pa_l1ps_com,".l1s_global")
 #pragma DATA_SECTION(l1ps_macs_com,".l1s_global")
 #pragma DATA_SECTION(l1ps_dsp_com,".l1s_global")
#endif

  // Global Packet L1A structure
  T_L1PA_GLOBAL    l1pa;

  // Global Packet L1S structure
  T_L1PS_GLOBAL    l1ps;

  // Common structure between L1A and L1S in packet mode
  T_L1PA_L1PS_COM  l1pa_l1ps_com;
 
  // Communication between L1S and MAC-S in packet mode
  T_L1PS_MACS_COM  l1ps_macs_com;

  // MCU / DSP interface
  T_L1PS_DSP_COM  l1ps_dsp_com;

#else
  extern T_L1PA_GLOBAL    l1pa;
  extern T_L1PS_GLOBAL    l1ps;
  extern T_L1PA_L1PS_COM  l1pa_l1ps_com;
  extern T_L1PS_DSP_COM   l1ps_dsp_com;
  extern T_L1PS_MACS_COM  l1ps_macs_com;

#endif
