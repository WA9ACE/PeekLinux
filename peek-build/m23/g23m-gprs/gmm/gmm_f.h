/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_f.h
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  Definitions for gmm_f.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_F_H
#define GMM_F_H

/*
 * gmm data debug print mask
 * select type of information for debug print 
 */
enum
{
   GMM_DEBUG_PRINT_MASK_FULL          = 0x0fff, /*                   */
   //
   GMM_DEBUG_PRINT_MASK_COMMON        = 0x0001, /* 00000000 00000001 */
   GMM_DEBUG_PRINT_MASK_TIMER         = 0x0002, /* 00000000 00000010 */
   GMM_DEBUG_PRINT_MASK_STATE         = 0x0004, /* 00000000 00000100 */
   GMM_DEBUG_PRINT_MASK_TLLI          = 0x0008, /* 00000000 00001000 */
   GMM_DEBUG_PRINT_MASK_IMSI          = 0x0010, /* 00000000 00010000 */
   GMM_DEBUG_PRINT_MASK_PTMSI         = 0x0020, /* 00000000 00100000 */
   GMM_DEBUG_PRINT_MASK_CID           = 0x0040, /* 00000000 01000000 */
   GMM_DEBUG_PRINT_MASK_RAI           = 0x0080, /* 00000000 10000000 */
   //
   GMM_DEBUG_PRINT_MASK_KERN          = 0x0100, /* 00000001 00000000 */
   GMM_DEBUG_PRINT_MASK_KERN_ATTACH   = 0x0200, /* 00000010 00000000 */

	GMM_DEBUG_PRINT_MASK_END,
	GMM_DEBUG_PRINT_MASK_MAX = (GMM_DEBUG_PRINT_MASK_END - 1)
};
typedef U32 GMM_DEBUG_PRINT_MASK_ENUM_TYPE;

EXTERN void gmm_debug_do_print_gmm_data(GMM_DEBUG_PRINT_MASK_ENUM_TYPE print_mask);
#ifdef WIN32
#define GMM_TRACE_GMM_DATA(m) gmm_debug_do_print_gmm_data(m)
#else
#define GMM_TRACE_GMM_DATA(m)
#endif

EXTERN ULONG gmm_get_tlli ( T_TLLI_TYPE tlli_type );

EXTERN void gmm_set_current_tlli ( T_TLLI_TYPE tlli_type );

#endif /* GMM_F_H */

