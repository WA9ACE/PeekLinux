/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  cci_hw_sim.h
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
|  Purpose :  Definitions for cci_qf.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef CCI_HW_SIM_H
#define CCI_HW_SIM_H

EXTERN void ciph_hw_sim_cipher ( U16 len );

EXTERN void ciph_hw_sim_decipher ( void ) ; 

EXTERN void ciph_reg16_write_sim (void) ;

EXTERN void ciph_reg8_write_sim( void ) ;

EXTERN void ciph_reg16_read_sim (void) ;

EXTERN void ciph_reg8_read_sim( void ) ;

#endif /* !CCI_HW_SIM_H */

