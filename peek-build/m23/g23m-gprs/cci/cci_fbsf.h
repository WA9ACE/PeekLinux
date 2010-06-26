/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  cci_fbsf.h
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
|  Purpose :  Definitions for cci_fbsf.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef CCI_FBSF_H
#define CCI_FBSF_H

#ifdef TI_PS_OP_CIPH_DRIVER
EXTERN void ciph_cipher_req (T_CIPH_cipher_req_parms *cipher_req_parms_ptr, 
                             T_CIPH_in_data_list *in_data_ptr, 
                             T_CIPH_out_data *out_data_ptr, 
                             U8 *status);

EXTERN void ciph_init_cipher_req (T_CIPH_init_cipher_req_parms *init_cipher_req_parms_ptr, 
                                  void (*read_data) (void));
#else
EXTERN void ciph_cipher_req_sim (T_CIPH_cipher_req_parms *cipher_req_parms_ptr, 
                                 T_CIPH_in_data_list *in_data_ptr, 
                                 T_CIPH_out_data *out_data_ptr, 
                                 U8 *status);

EXTERN void ciph_init_cipher_req_sim (T_CIPH_init_cipher_req_parms *init_cipher_req_parms_ptr, 
                                      void (*read_data) (void));

#endif /* TI_PS_OP_CIPH_DRIVER */

#endif /* !CCI_FBSF_H */

