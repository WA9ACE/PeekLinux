/*
+-----------------------------------------------------------------------------
|  File     : psi_kerp.h
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
|  Purpose  : Definitions for psi_kerp.c 
|  $Identity:$
+-----------------------------------------------------------------------------
*/
#ifndef PSI_KERP_H
#define PSI_KERP_H
EXTERN void psi_mfree_buffers (T_dio_buffer *buffers);
EXTERN void psi_ker_free_all_buffers (void);
EXTERN void psi_ker_dti_open_req (T_PSI_DTI_OPEN_REQ *psi_dti_open_req);
EXTERN void psi_ker_dti_close_req (T_PSI_DTI_CLOSE_REQ *psi_dti_close_req);
EXTERN void psi_ker_sig_connect_ind (U32 device);
EXTERN void psi_ker_sig_disconnect_ind (U32 device);
EXTERN void psi_ker_conn_rej (T_PSI_CONN_REJ *psi_conn_rej);
EXTERN void psi_ker_conn_res (T_PSI_CONN_RES *psi_conn_res);
#ifdef _SIMULATION_
EXTERN void psi_ker_setconf_req_test (T_PSI_SETCONF_REQ_TEST *psi_setconf_req_test);
#else
EXTERN void psi_ker_setconf_req (T_PSI_SETCONF_REQ *psi_setconf_req);
#endif /* _SIMULATION_ */
EXTERN void psi_ker_line_state_req (T_PSI_LINE_STATE_REQ *psi_line_state_req);
EXTERN void psi_ker_close_req (T_PSI_CLOSE_REQ *psi_close_req);
EXTERN void psi_mfree_ctrl_ser (T_DIO_CTRL_LINES* ctrl_ser);
EXTERN void psi_malloc_dcb_ser(void);
EXTERN void psi_malloc_dcb_pkt(void);
EXTERN void psi_malloc_ctrl_ser(T_DIO_CTRL_LINES **ctrl);

#endif /* !PSI_KERP_H */

