/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * L1P_SIGN.H
 *
 *        Filename l1p_sign.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

#define P_GPRS  ( P_L1C + 2 )

// Messages Packet Transfer <-> L1A
#define MPHP_SINGLE_BLOCK_REQ             ( ( P_GPRS << 8 ) | 1  ) // build: T_MPHP_SINGLE_BLOCK_REQ
#define MPHP_SINGLE_BLOCK_CON             ( ( P_GPRS << 8 ) | 2  )
#define MPHP_STOP_SINGLE_BLOCK_REQ        ( ( P_GPRS << 8 ) | 3  ) // build: trigger
#define MPHP_STOP_SINGLE_BLOCK_CON        ( ( P_GPRS << 8 ) | 4  )
#define L1P_SINGLE_BLOCK_CON              ( ( P_GPRS << 8 ) | 5  )

#define MPHP_ASSIGNMENT_REQ               ( ( P_GPRS << 8 ) | 6  ) // build: T_MPHP_ASSIGNMENT_REQ
#define MPHP_ASSIGNMENT_CON               ( ( P_GPRS << 8 ) | 7  )

#define MPHP_TBF_RELEASE_REQ              ( ( P_GPRS << 8 ) | 8  ) // build: T_MPHP_TBF_RELEASE_REQ
#define MPHP_TBF_RELEASE_CON              ( ( P_GPRS << 8 ) | 9  )

#define MPHP_REPEAT_UL_FIXED_ALLOC_REQ    ( ( P_GPRS << 8 ) | 10 ) // build: T_MPHP_REPEAT_UL_FIXED_ALLOC_REQ
#define MPHP_REPEAT_UL_FIXED_ALLOC_CON    ( ( P_GPRS << 8 ) | 11 )

#define MPHP_PDCH_RELEASE_REQ             ( ( P_GPRS << 8 ) | 12 ) // build: T_MPHP_PDCH_RELEASE_REQ
#define MPHP_PDCH_RELEASE_CON             ( ( P_GPRS << 8 ) | 13 )

#define MPHP_TIMING_ADVANCE_REQ           ( ( P_GPRS << 8 ) | 14 ) // build: T_MPHP_TIMING_ADVANCE_REQ
#define MPHP_TIMING_ADVANCE_CON           ( ( P_GPRS << 8 ) | 15 )

#define MPHP_UPDATE_PSI_PARAM_REQ         ( ( P_GPRS << 8 ) | 16 ) // build: T_MPHP_UPDATE_PSI_PARAM_REQ
#define MPHP_UPDATE_PSI_PARAM_CON         ( ( P_GPRS << 8 ) | 17 )


#define MPHP_RA_REQ                       ( ( P_GPRS << 8 ) | 18 ) // build: T_MPHP_RA_REQ
#define MPHP_RA_CON                       ( ( P_GPRS << 8 ) | 19 )
#define MPHP_RA_STOP_REQ                  ( ( P_GPRS << 8 ) | 20 ) // build: trigger
#define MPHP_RA_STOP_CON                  ( ( P_GPRS << 8 ) | 21 )

#define MPHP_POLLING_RESPONSE_REQ         ( ( P_GPRS << 8 ) | 22 ) // build: T_MPHP_POLLING_RESPONSE_REQ
#define MPHP_POLLING_IND                  ( ( P_GPRS << 8 ) | 23 )

#define L1P_RA_DONE                       ( ( P_GPRS << 8 ) | 24 )

// Messages Packet Idle <-> L1A
#define MPHP_START_PCCCH_REQ              ( ( P_GPRS << 8 ) | 25 ) // build: T_MPHP_START_PCCCH_REQ
#define MPHP_STOP_PCCCH_REQ               ( ( P_GPRS << 8 ) | 26 ) // build: trigger
#define MPHP_STOP_PCCCH_CON               ( ( P_GPRS << 8 ) | 27 )
#define MPHP_SCELL_PBCCH_REQ              ( ( P_GPRS << 8 ) | 28 ) // build: T_MPHP_SCELL_PBCCH_REQ
#define MPHP_SCELL_PBCCH_STOP_REQ         ( ( P_GPRS << 8 ) | 29 ) // build: trigger
#define MPHP_SCELL_PBCCH_STOP_CON         ( ( P_GPRS << 8 ) | 30 )
#define MPHP_CR_MEAS_REQ                  ( ( P_GPRS << 8 ) | 31 ) // build: T_MPHP_CR_MEAS_REQ
#define MPHP_CR_MEAS_STOP_REQ             ( ( P_GPRS << 8 ) | 32 ) // build: trigger
#define MPHP_CR_MEAS_STOP_CON             ( ( P_GPRS << 8 ) | 33 )
#define MPHP_INT_MEAS_REQ                 ( ( P_GPRS << 8 ) | 34 ) // build: T_MPHP_INT_MEAS_REQ
#define MPHP_INT_MEAS_STOP_REQ            ( ( P_GPRS << 8 ) | 35 ) // build: trigger
#define MPHP_INT_MEAS_STOP_CON            ( ( P_GPRS << 8 ) | 36 )
#define MPHP_NCELL_PBCCH_REQ              ( ( P_GPRS << 8 ) | 37 ) // build: T_MPHP_NCELL_PBCCH_REQ
#define MPHP_NCELL_PBCCH_STOP_REQ         ( ( P_GPRS << 8 ) | 38 ) // build: trigger
#define MPHP_NCELL_PBCCH_STOP_CON         ( ( P_GPRS << 8 ) | 39 )

#define MPHP_DATA_IND                     ( ( P_GPRS << 8 ) | 40 )
#define MPHP_CR_MEAS_IND                  ( ( P_GPRS << 8 ) | 41 )
#define MPHP_INT_MEAS_IND                 ( ( P_GPRS << 8 ) | 42 )
#define MPHP_TINT_MEAS_IND                ( ( P_GPRS << 8 ) | 43 )
#define MPHP_NCELL_PBCCH_IND              ( ( P_GPRS << 8 ) | 44 )
#define MPHP_TCR_MEAS_REQ                 ( ( P_GPRS << 8 ) | 45 ) // build: T_MPHP_CR_MEAS_REQ
#define MPHP_TCR_MEAS_IND                 ( ( P_GPRS << 8 ) | 46 )
#define MPHP_TCR_MEAS_STOP_REQ            ( ( P_GPRS << 8 ) | 47 ) // build: trigger
#define MPHP_TCR_MEAS_STOP_CON            ( ( P_GPRS << 8 ) | 48 )

/*** L1S -> L1A communication ***/
#define L1P_PALLC_INFO                    ( ( P_GPRS << 8)  | 49)
#define L1P_PNP_INFO                      ( ( P_GPRS << 8)  | 50)
#define L1P_PEP_INFO                      ( ( P_GPRS << 8)  | 51)
#define L1P_PBCCHS_INFO                   ( ( P_GPRS << 8)  | 52)
#define L1P_PACCH_INFO                    ( ( P_GPRS << 8)  | 53)
#define L1P_CR_MEAS_DONE                  ( ( P_GPRS << 8)  | 54)
#define L1P_TRANSFER_DONE                 ( ( P_GPRS << 8)  | 55)
#define L1P_TCR_MEAS_DONE                 ( ( P_GPRS << 8)  | 56)
#define L1P_TBF_RELEASED                  ( ( P_GPRS << 8)  | 57)
#define L1P_ITMEAS_IND                    ( ( P_GPRS << 8)  | 58)
#define L1P_POLL_DONE                     ( ( P_GPRS << 8)  | 59)
#define L1P_PDCH_RELEASED                 ( ( P_GPRS << 8)  | 60)
#define L1P_TA_CONFIG_DONE                ( ( P_GPRS << 8)  | 61)
#define L1P_PBCCHN_INFO                   ( ( P_GPRS << 8)  | 62)
#define L1P_REPEAT_ALLOC_DONE             ( ( P_GPRS << 8)  | 63)
#define L1P_ALLOC_EXHAUST_DONE            ( ( P_GPRS << 8)  | 64)

