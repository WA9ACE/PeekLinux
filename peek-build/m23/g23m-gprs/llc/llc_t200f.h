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
|  Purpose :  Definitions for llc_t200f.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_T200F_H
#define LLC_T200F_H


/*
 * Function declarations.
 */
EXTERN void t200_init (void);

EXTERN void t200_start 
(
#ifdef LL_DESC
 T_LL_UNITDESC_REQ *frame,
#else
 T_LL_UNITDATA_REQ *frame,
#endif
 UBYTE cause,
 T_SERVICE originator
);

EXTERN void t200_stop (void);

EXTERN void t200_get_timer_data 
(
#ifdef LL_DESC 
 T_LL_UNITDESC_REQ **frame, 
#else
 T_LL_UNITDATA_REQ **frame, 
#endif 
 UBYTE *cause,
 T_SERVICE *originator
);



#endif /* !LLC_T200S_H */
