/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (?)                  $Workfile:: mfw_nma.h       $|
| $Author:: Es                          $Revision::  1              $|
| CREATED: 14.10.98                     $Modtime:: 17.05.99 14:17   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

	PURPOSE :  Types definitions for MMI framework 
    .
*/

#ifndef DEF_MFW_NMA_HEADER
#define DEF_MFW_NMA_HEADER

/* #include "phb.h" RSA 20/12/01 - Removed */
#include"mfw_phb.h" //RSA 20/12/01 - Added

EXTERN void nma_init (void);
EXTERN BOOL nma_response_cb (ULONG opc, void * data);

EXTERN void sAT_PlusCOPSF(UBYTE ** forbidden_id, UBYTE ** fieldstrength);
#ifdef NO_ASCIIZ
EXTERN void sAT_PlusCOPSE(UBYTE *oper, UBYTE format, 
                          T_MFW_LNAME *long_name,
                          T_MFW_SNAME *short_name, 
                          UBYTE *numeric_name);
#else
EXTERN void sAT_PlusCOPSE(UBYTE *oper, UBYTE format, UBYTE *long_name, 
                          UBYTE *short_name, UBYTE *numeric_name);
#endif

#ifdef FF_2TO1_PS
EXTERN T_CAUSE_ps_cause sAT_PlusCREGE (void);
EXTERN void  nm_nitz_info_ind(T_MMREG_INFO_IND * mmr_info_ind);
#else
EXTERN USHORT sAT_PlusCREGE (void);
EXTERN void  nm_nitz_info_ind(T_MMR_INFO_IND * mmr_info_ind);
#endif

#endif
