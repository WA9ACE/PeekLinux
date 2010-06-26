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
|  Purpose :  Definitions for ppp_onaf.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_ONAF_H
#define PPP_ONAF_H


EXTERN void ona_init (void);

EXTERN void ona_get_inst_index(USHORT ptype, UBYTE* index);

#endif /* !PPP_ONAF_H */
