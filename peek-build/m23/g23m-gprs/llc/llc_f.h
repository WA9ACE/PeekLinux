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
|  Purpose :  Definitions for llc_f.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_F_H
#define LLC_F_H


EXTERN void   llc_init_parameters (void);

EXTERN void   llc_generate_input (UBYTE sapi,
                                  T_PDU_TYPE frame_type,
                                  T_FRAME_NUM lfn,
                                  ULONG *cipher_input,
                                  ULONG oc);

EXTERN ULONG  llc_build_crc24 (UBYTE *octets, 
                                  USHORT len);

EXTERN BOOL   llc_xid_value_acceptable (UBYTE sapi,
                                  UBYTE xid_parameter,
                                  ULONG xid_value);

EXTERN void   llc_init_requested_xid (void);
EXTERN void   llc_init_requested_xid_sapi (T_SAPI sapi);

#ifdef LL_DESC
EXTERN T_desc3*   llc_palloc_desc (U16 len, U16 offset);
EXTERN void llc_cl_desc3_free(T_desc3* p_desc3);
#endif

#ifndef TI_PS_OP_CIPH_DRIVER
EXTERN void llc_fbs_init ( void );
#endif

EXTERN void llc_copy_ul_data_to_list(T_CCI_CIPHER_DESC_REQ *cipher_req, 
                                 T_CIPH_in_data_list *in_data);
EXTERN void llc_copy_dl_data_to_list(T_CCI_DECIPHER_REQ *decipher_req, 
                                 T_CIPH_in_data_list *in_data_list);

EXTERN void llc_trace_desc_list3_content(T_desc_list3 desc_list3);

EXTERN void llc_trace_sdu(T_sdu* sdu);

EXTERN void llc_trace_desc_list(T_desc_list* desc_list);


#endif /* !LLC_F_H */
