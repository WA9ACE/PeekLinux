/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRR
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
|  Purpose :  Definitions for service GFP.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_GFPF_H
#define GRR_GFPF_H

EXTERN void gfp_init            ( void                        );

EXTERN BOOL gfp_prcs_ctrl_msg   ( UBYTE* l2_frame,
                                  UBYTE  l2_channel,
                                  UBYTE  relative_position,
                                  ULONG  frame_number         );

EXTERN void gfp_send_ctrl_block ( ULONG fn_i, 
                                  UBYTE tn_i, 
                                  UBYTE rrbp_i, 
                                  UBYTE sp_i, 
                                  UBYTE pctrl_ack_i,
                                  T_MSGBUF * ptr_ctrl_block_i );

EXTERN BOOL gfp_check_tfi       ( T_D_HEADER * ptr_header_i, UBYTE tn   );



#endif /* !GRR_GFPF_H */

