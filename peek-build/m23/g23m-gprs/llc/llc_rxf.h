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
|  Purpose :  Definitions for llc_rxf.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_RXF_H
#define LLC_RXF_H


/*
 * Values for frame_rej parameter of rx_interpret_frame().
 */
#define FRAME_NOT_REJ         0x00
#define FRAME_REJ_W1          0x01
#define FRAME_REJ_W3          0x04


EXTERN void rx_init (void);

EXTERN void rx_analyse_ctrl_field (T_GRLC_UNITDATA_IND *grlc_unitdata_ind,
                                   T_PDU_TYPE *frame_type,
                                   UBYTE  *protected_mode,
                                   UBYTE  *sapi,
                                   T_FRAME_NUM *ns,
                                   BOOL   *ciphering,
                                   USHORT *header_size,
                                   BOOL   *frame_ok);

EXTERN void rx_send_decipher_req (T_GRLC_UNITDATA_IND *grlc_unitdata_ind,
                                  T_PDU_TYPE frame_type,
                                  UBYTE protected_mode,
                                  T_FRAME_NUM ns,
                                  USHORT header_size,
                                  BOOL ciphering);

EXTERN void rx_interpret_frame (T_sdu *frame,
                                UBYTE *sapi, 
                                T_PDU_TYPE *pdu_type, 
                                T_COMMAND *command, 
                                T_BIT *cr_bit, 
                                T_BIT *pf_bit, 
                                T_FRAME_NUM *nr, 
                                T_FRAME_NUM *ns, 
                                BOOL *frame_ok, 
                                UBYTE *frame_rej,
                                USHORT *frame_rej_ctrl_length,
                                UBYTE  cipher);

EXTERN void rx_strip_llc_header (T_sdu *sdu, 
                                 T_PDU_TYPE pdu_type,
                                 T_COMMAND command);


#endif /* !LLC_RXS_H */
