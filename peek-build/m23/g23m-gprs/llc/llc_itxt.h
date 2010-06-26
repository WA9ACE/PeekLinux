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
|  Purpose :  Definitions for llc_itxt.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_ITXT_H
#define LLC_ITXT_H


EXTERN void itx_T201_start (T_TIME length);

EXTERN void itx_T201_stop  (void);

EXTERN void itx_timer_t201 (UBYTE sapi);



#endif /* !LLC_ITXT_H */
