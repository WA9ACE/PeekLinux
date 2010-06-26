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
|  Purpose :  Definitions for service PSI.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_PSIF_H
#define GRR_PSIF_H

#define INIT_NEEDED 1
#define INIT_NOT_NEEDED 0

typedef enum
{
  PSI1_OK,
  PSI1_COMPLETE_ACQ,
  PSI1_PARTIAL_ACQ,
  PSI1_PBCCH_RELEASED,
  PSI1_PBCCH_ESTABLISHED
}T_PSI1_RET;

typedef enum
{
  PSI2_OK,
  PSI2_REREAD
}T_PSI2_RET;

typedef enum
{
  PSI3_OK,
  PSI3_CELL_BARRED,
  PSI3_NCELL_VALID,
  PSI3_NCELL_INVALID
}T_PSI3_RET;

typedef enum
{
  PSI3BIS_OK
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  , PSI3BIS_REREAD_PSI3_3BIS_3TER
#else
  , PSI3BIS_REREAD_PSI3_3BIS
#endif
  , PSI3BIS_REREAD_PSI3BIS,
  PSI3BIS_NCELL_VALID
}T_PSI3BIS_RET;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
typedef enum
{
  PSI3TER_OK,
  PSI3TER_REREAD_PSI3_3BIS_3TER,
  PSI3TER_REREAD_PSI3TER
}T_PSI3TER_RET;
#endif

typedef enum
{
  PSI4_OK,
  PSI4_INT_LIST_VALID,
  PSI4_REREAD
}T_PSI4_RET;

typedef enum
{
  PSI5_OK,
  PSI5_MEAS_PARAM_VALID,
  PSI5_REREAD
}T_PSI5_RET;


#ifdef REL99
typedef enum
{
  PSI8_OK,
  PSI8_CONTENTS_INVALID,
  PSI8_REREAD
}T_PSI8_RET;
#endif

typedef enum
{
  SI13_OK,
  SI13_REREAD,
  SI13_PARTIAL_SI,
  SI13_COMPLETE_SI,
  SI13_COMPLETE_PSI
}T_SI13_RET;

typedef enum
{
  PSI13_OK,
  PSI13_PBCCH_RELEASED,
  PSI13_PBCCH_ESTABLISHED,
  PSI13_PARTIAL_SI,
  PSI13_COMPLETE_SI,
  PSI13_COMPLETE_PSI
}T_PSI13_RET;


typedef enum /* T_STATES_OF_PSI*/
{
  NOT_SEND,
  RECEIPT_OK,
  NEEDED
}T_STATES_OF_PSI;

typedef enum /* T_PSI_MESSAGES*/
{
  PSI1,
  PSI2,
  PSI3,
  PSI3bis,
#ifdef REL99
  PSI3ter,
#endif
  PSI4,
  PSI5,
#ifdef REL99
  PSI8,
#endif
  PSI13
}T_PSI_MESSAGES;

typedef enum
{
  ACQ_RUNNING,
  ACQ_PART_OK,
  ACQ_COMP_OK,
  ACQ_PERIOD_OK,
  ACQ_NPSI_OK
}T_ACQ_STATE_RET;

EXTERN T_ACQ_STATE_RET psi_check_acq_state ( void );

EXTERN void psi_reset_all ( void);

EXTERN void psi_stop_10sec ( void );

EXTERN void psi_start_10sec ( void );

EXTERN void psi_partial_acq ( void );

EXTERN void psi_init ( void );

EXTERN void psi_init_params ( void );

EXTERN void psi_complete_acq ( ACQ_TYPE acq_type  );

EXTERN void psi_send_psi_status ( void );

EXTERN void psi_stop_30sec ( BOOL start_again);

EXTERN void psi_start_30sec ( void );

EXTERN void psi_start_60sec ( void );

EXTERN void psi_stop_60sec ( BOOL start_again);

EXTERN T_PSI13_RET psi_process_psi13 ( T_PSI_13 * psi13 );

EXTERN T_SI13_RET psi_process_si13( T_D_SYS_INFO_13 *si13);

EXTERN T_PSI5_RET psi_process_psi5 ( T_PSI_5 *psi5);

EXTERN T_PSI4_RET psi_process_psi4 ( T_PSI_4 *psi4);

EXTERN T_PSI3BIS_RET psi_process_psi3bis ( T_PSI_3_BIS *psi3bis);

EXTERN T_PSI3_RET psi_process_psi3 ( T_PSI_3 *psi3);

EXTERN T_PSI2_RET psi_process_psi2 ( T_PSI_2 *psi2);

EXTERN T_PSI1_RET psi_process_psi1 ( T_PSI_1 *psi1);

EXTERN void psi_receive_psi ( UBYTE  psi_reading_type);

EXTERN void psi_process_prach ( T_prach_ctrl_par *prach );

EXTERN void psi_reset_si_entries ( void );

EXTERN T_ACQ_STATE_RET psi_acq_state_of_si( T_si_states  si_states );

EXTERN void psi_check_acq_state_of_si( UBYTE updated_si);

EXTERN BOOL psi_is_update_needed ( UBYTE si_change_field );

EXTERN void psi_stop_psi_reading(ACQ_TYPE acq_type_in_idle);

EXTERN void psi_update_data_to_request( UBYTE init_needed);

EXTERN BOOL psi_is_access_class_changed(void);

EXTERN void psi_stop_timer ( void );
EXTERN BOOL psi_is_access_allowed(void);
EXTERN void psi_reread_psi2(void);
#if defined (REL99) AND defined (TI_PS_FF_EMR)
EXTERN void psi_reread_psi3_3bis_3ter(void);
#else
EXTERN void psi_reread_psi3_and_3bis(void);
#endif
EXTERN void psi_reread_psi3bis(void);
#if defined (REL99) AND defined (TI_PS_FF_EMR)
EXTERN void psi_reread_psi3ter(void);
#endif
EXTERN void psi_reread_psi4(void);
EXTERN void psi_reread_psi5(void);

EXTERN void psi_handle_psi1(T_PSI_1 * psi);
EXTERN void psi_handle_psi2(T_PSI_2 * psi);
EXTERN void psi_handle_psi3(T_PSI_3 * psi);
EXTERN void psi_handle_psi3bis(T_PSI_3_BIS *psi);
#if defined (REL99) AND defined (TI_PS_FF_EMR)
EXTERN void psi_handle_psi3ter(T_PSI_3_TER *psi);
#endif
EXTERN void psi_handle_psi4(T_PSI_4 * psi);
#ifdef REL99
EXTERN void psi_handle_psi8(T_PSI_8 * psi);
#endif
EXTERN void psi_send_access_enable_if_needed(void);
EXTERN void psi_initiate_read_complete_si(void);
EXTERN void psi_initiate_read_complete_psi( UBYTE init_needed );
EXTERN void psi_initiate_pbcch_switching(T_PSI_DISABLE_CAUSE dc);

EXTERN void psi_prepare_scell_pbcch_req(T_MPHP_SCELL_PBCCH_REQ *mphp_scell_pbcch_req, 
                                        UBYTE  psi_reading_type);

EXTERN void psi_store_rel_pos(UBYTE *dest, UBYTE rel_pos, UBYTE max_dest);

#ifdef TI_PS_FF_QUAD_BAND_SUPPORT
EXTERN void att_update_std_band_indicator (UBYTE band_indicator);
#endif

#endif /* !GRR_PSIF_H */

