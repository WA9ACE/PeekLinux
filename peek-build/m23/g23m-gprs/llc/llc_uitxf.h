/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  
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
|  Purpose :  Definitions for llc_uitxf.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_UITXF_H
#define LLC_UITXF_H




EXTERN void uitx_init (void);

EXTERN void uitx_init_sapi (void);

EXTERN void uitx_build_ui_header 
  (
#ifdef LL_DESC                    
  T_desc_list3 *desc_list3,
#else                             
  T_sdu *sdu,
#endif
  T_BIT cr_bit, UBYTE sapi, 
  T_FRAME_NUM nu, UBYTE cipher, 
  UBYTE relclass
  );



#endif /* !LLC_UITXS_H */

