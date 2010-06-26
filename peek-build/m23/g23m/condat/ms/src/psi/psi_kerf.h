/*
+-----------------------------------------------------------------------------
|  File     : psi_kerf.h
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
|  Purpose  : Definitions for psi_kerf.c 
|  $Identity:$
+-----------------------------------------------------------------------------
*/

#ifndef PSI_KERF_H
#define PSI_KERF_H

EXTERN void psi_ker_assign_cause (U16* cause, U16 result);
#ifdef _SIMULATION_
EXTERN BOOL psi_ker_assign_dcb_sim (T_DIO_DCB* dcb, U32 dev_type, T_DIO_DCB_UN* sap_dcb);
#else
EXTERN BOOL psi_ker_assign_dcb (T_DIO_DCB* dcb, U32 dev_type, const T_DIO_DCB* sap_dcb);
#endif /* _SIMULATION_ */
EXTERN void psi_ker_init (void);
EXTERN void psi_ker_dti_reason_connection_closed (void);
EXTERN void psi_ker_dti_reason_connection_opened (void);
EXTERN U16 psi_ker_new_instance (U32 device);
EXTERN U16 psi_ker_search_basic_data_by_device (U32 device);
EXTERN U16 psi_ker_instance_switch(U8 instance);
EXTERN BOOL check_baudrate(U32 dio_baudrate,U32 sap_baudrate);
EXTERN BOOL check_char_frame(U32 dio_char_frame, U32 sap_char_frame);
EXTERN BOOL check_flow_control(U32 dio_flow_control, U32 sap_flow_control);
EXTERN void psi_ker_set_init_conf (void);
EXTERN void psi_ker_assign_ctrl( T_DIO_CTRL_LINES *ptr_dio_ctrl, U16 line_state);
EXTERN U32 search_highest_baudrate (T_DIO_CAP_SER * capabilities);

#endif /* !PSI_KERF_H */

