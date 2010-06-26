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
|  Purpose :  Definitions for entity GRR.
+----------------------------------------------------------------------------- 
*/   
#ifndef GRR_H
#define GRR_H

/*
 * Start defintions from entity RR
 */

/*
 ****************************************************************************
 * The following declarations shall be identical with the corresponding 
 * declarations located in RR. 
 ****************************************************************************
 */

EXTERN UBYTE     std;
EXTERN UBYTE     rr_csf_get_radio_access_capability( T_ra_cap *ra_cap );
EXTERN T_rf_cap* rr_csf_get_rf_capability          ( void             );
#ifdef TI_PS_FF_QUAD_BAND_SUPPORT
#ifdef _SIMULATION_ 
EXTERN void rr_csf_check_rfcap (UBYTE init);
#endif
#endif
#if defined (REL99) AND defined (TI_PS_FF_EMR)
EXTERN BOOL      rr_get_support_for_emr            ( void             );
#endif

#define MAX_CLASSES 5

EXTERN const SHORT p_dcs [MAX_CLASSES];
EXTERN const SHORT p_pcs [MAX_CLASSES];
EXTERN const SHORT p_gsm [MAX_CLASSES];

#define MAX_PCL     32

EXTERN const UBYTE p_control_gsm [MAX_PCL];
EXTERN const UBYTE p_control_dcs [MAX_PCL];
EXTERN const UBYTE p_control_pcs [MAX_PCL];

typedef struct
{
  USHORT l_f;
  USHORT o_f;
  UBYTE  b_f[32];
} T_f_range;

typedef struct
{
  UBYTE b_f[128];
} T_LIST;

EXTERN void for_create_channel_list ( T_f_range *f_range,
                                      T_LIST    *cha_list         );

EXTERN void srv_merge_list          ( T_LIST    *target_list, 
                                      T_LIST    *list             );

EXTERN int  srv_create_list         ( T_LIST    *list,
                                      USHORT    *channel_array,
                                      USHORT     size,
                                      UBYTE      start_with_zero,
                                      USHORT     start_index      );


#ifdef _TARGET_

EXTERN void srv_get_cell_alloc_list ( T_LIST    *target_list      );

#endif

#define RX_QUAL_UNAVAILABLE           0xFF

#ifdef FF_PS_RSSI
#define RX_ACCE_UNAVAILABLE           0xFF
EXTERN void RX_SetValue             ( UBYTE new_value, UBYTE new_qual, UBYTE new_access);
#else
EXTERN void RX_SetValue             ( UBYTE      new_value        );
#endif

/*
 ****************************************************************************
 * The above declarations shall be identical with the corresponding 
 * declarations located in RR. 
 ****************************************************************************
 */

/*
 * End defintions from entity RR
 */

typedef struct /* T_MS_PWR_CAP */
{
  const SHORT *p;          /* power class -> output power              */
  const UBYTE *p_control;  /* power control level -> output power      */
        UBYTE  ms_power;   /* MS power class                           */
        SHORT  pwr_offset; /* POWER_OFFSET                             */
        SHORT  pwr_max;    /* maximum nominal output power in the cell */
        SHORT  pwr_min;    /* minimum nominal output power in the cell */
        SHORT  gamma_0;    /* GAMMA 0                                  */
        UBYTE  band_ind;   /* indicates the used frequency band        */
} T_MS_PWR_CAP;

/*
 * defines the user of the vsi interface
 */
#define VSI_CALLER            GRR_handle,
#define VSI_CALLER_SINGLE     GRR_handle 

/*
 * Macros
 */
#ifdef REL99 
#define MAX_PSI                             9 /* Maximum number of PSIs: PSI1,2,3,3bis,3ter,4,5,8,13 */
#else
#define MAX_PSI                             7 /* Maximum number of PSIs: PSI1,2,3,3bis,4,5,13*/
#endif

#define ENCODE_OFFSET                       0

/*
 * Bit and byte length for encoding/decoding
 */

#define BIT_UL_CTRL_BLOCK_MAC_HEADER  8
#define BIT_UL_CTRL_BLOCK_CONTENTS    176
#define BIT_UL_CTRL_BLOCK             (BIT_UL_CTRL_BLOCK_MAC_HEADER+BIT_UL_CTRL_BLOCK_CONTENTS)

#define BYTE_UL_CTRL_BLOCK            (BIT_UL_CTRL_BLOCK/BITS_PER_BYTE)

/*
 * Constants
 */

/* 
 * Disable Get State traces only for target enviroment
 */
#ifdef _TARGET_
#undef GET_STATE
#define GET_STATE(P)        (ENTITY_DATA->P state)
#endif

#define MA_NUMBER_4_PSI13_OR_CELL_ALLOC     14 /* MA_NUMBER used to reference a Mobile Allocation
                                                  (MA) received in PSI13, which may referenced from
                                                  an assignment message or which referes to the cell
                                                  allocation defined for the cell in SI1 or PSI2*/

#define MA_NUMBER_4_ASSIGNMENT               15 /* MA_NUMBER used to reference a MA received in a previous assignent*/

#ifdef REL99
#define GRR_FREQ_PARAM_RECEIVED_IN_ASSIGNMENT 0
/* 
 * This refers to freq param that is received in non assignment
 * messages like PSI 8 and PSI 14 
 */
#define GRR_FREQ_PARAM_RECEIVED_IN_NON_ASSIGNMENT 1 
#endif

/*
 * Define the size of the GRR queue in frame
 */
#define PEI_PRIM_QUEUE_SIZE 20



#define FN_MAX           0x297000
#define GRR_INVALID_FN   0xFFFFFFFF


/*
 * Macros for handling of individual bits within a value used as flags
 */
#define SET_FLAG(val,mask,flag,type)  (val=(((val)&(~(type)(mask)))|(flag)))
#define IS_FLAGGED(val,mask,flag)     ((val & mask ) EQ flag)
#define IS_NOT_FLAGGED(val,mask,flag) ((val & mask ) NEQ flag)

/*
 * Defines for the gprs_database
 */

/*
 * Service states 
 */
#define GPRS_SERVICE_NONE        0 /* no service      */
#define GPRS_SERVICE_LIMITED     1 /* limited service */
#define GPRS_SERVICE_FULL        2 /* full service    */

typedef UBYTE T_GPRS_SERVICE;

/*
 * Access states 
 */
#define GPRS_ACCESS_BARRED       0 /* access barred   */
#define GPRS_ACCESS_ALLOWED      1 /* access allowed  */

typedef UBYTE T_ACCESS_STATUS;


 /*
  * Maximum number of IMSI digits
  */
#define MAX_IMSI_DIGITS         16

#define SEG_CTRL_BLOCK_SIZE      8

#ifdef REL99
 /*
  * Network Release 
  */
#define  BSS_NW_REL_97 0
#define  BSS_NW_REL_99 1
#define  BSS_NW_REL_04 2

#endif


/*
start constants for CTRL
*/
/* final states */
#define GLBL_PCKT_MODE_NULL                     0
#define GLBL_PCKT_MODE_SUSP                     1
#define GLBL_PCKT_MODE_SYNC                     2
#define GLBL_PCKT_MODE_IDLE                     3 
#define GLBL_PCKT_MODE_ACCESS                   4
#define GLBL_PCKT_MODE_ASSIGNED                 5
#define GLBL_PCKT_MODE_TRANSFER                 6
#define GLBL_PCKT_MODE_RELEASED                 7

/* transitions states from NULL mode to others */
#define GLBL_PCKT_MODE_NULL_IDLE                8

/* transitions states from suspended mode to others */
#define GLBL_PCKT_MODE_SUSP_IDLE                9

/* transitions states from suspended mode to others */
#define GLBL_PCKT_MODE_SYNC_IDLE               10

/* transitions states from packet idle mode to others */
#define GLBL_PCKT_MODE_IDLE_ACCESS             11 
#define GLBL_PCKT_MODE_IDLE_ASSIGNED           12
#define GLBL_PCKT_MODE_IDLE_TRANSFER           13

/* transitions states from packet access mode to others */
#define GLBL_PCKT_MODE_ACCESS_IDLE             14
#define GLBL_PCKT_MODE_ACCESS_ASSIGNED         15
#define GLBL_PCKT_MODE_ACCESS_TRANSFER         16
#define GLBL_PCKT_MODE_ACCESS_TWO_PHASE        17

/* transitions states from packet assigned mode to others */
#define GLBL_PCKT_MODE_ASSIGNED_IDLE           18
#define GLBL_PCKT_MODE_ASSIGNED_TRANSFER       19

/* transitions states from packet transfer mode to others */
#define GLBL_PCKT_MODE_TRANSFER_IDLE           20
#define GLBL_PCKT_MODE_TRANSFER_RELEASED       21
#define GLBL_PCKT_MODE_TRANSFER_ACCESS         22

/* transitions states from packet released mode to others */
#define GLBL_PCKT_MODE_RELEASED_IDLE           23
#define GLBL_PCKT_MODE_RELEASED_ACCESS         24

/* transitions states from measurement report mode to others */
#define GLBL_PCKT_MODE_MEAS_REP_IDLE           25


typedef UBYTE T_GLBL_PCKT_MODE;

#define CTRL_TASK_NONE                          0
#define CTRL_TASK_PBCCH_LEAVE_NULL              1
#define CTRL_TASK_PBCCH_LEAVE_SUSP              2
#define CTRL_TASK_PBCCH_LEAVE_SYNC              3
#define CTRL_TASK_PBCCH_LEAVE_IDLE              4
#define CTRL_TASK_PBCCH_LEAVE_ACCESS            5
#define CTRL_TASK_PBCCH_LEAVE_TRANSFER          6
#define CTRL_TASK_BCCH                          7

typedef UBYTE T_CTRL_TASK;
/*
end constants for CTRL
*/


/*
start constants for TC
*/
#define RADIO_PRIO_4         3

#define RFL_NUMBER_4_DIRECT_ENCODING_2 (RFL_NUMBER_4_DL_CTRL_MSG_MAX + 1)
/*
end constants for TC
*/

/* MAC HEADER TYPE*/
#define CTRL_BLK_NO_OPT     1
#define CTRL_BLK_OPT        2


/*
start constants for MEAS
*/
/* maybe for later use */
/* 
#define M_ROUND_UP(x,meas_acrcy) (((x)>=0)?                                        \
                                  ((((x)%(meas_acrcy))< ((meas_acrcy)/ 2))?        \
                                   ( (x)/(meas_acrcy)   ):(((x)/(meas_acrcy))+1)): \
                                  ((((x)%(meas_acrcy))<=((meas_acrcy)/-2))?        \
                                   (((x)/(meas_acrcy))-1):( (x)/(meas_acrcy)   )))
*/

#define M_ROUND_UP(x,meas_acrcy)  ((((x)%(meas_acrcy))< ((meas_acrcy)/ 2))?        \
                                   ( (x)/(meas_acrcy)   ):(((x)/(meas_acrcy))+1))
#define NORM_POW_MIN            (ULONG)1000 /* normalised math. power        */
                                            /* function maximum value        */
#define NORM_TAVGW_FAC          (ULONG)10   /* normalised Tavg_w factor      */
#define DRX_NORM_FACTOR         (NORM_TAVGW_FAC*NORM_POW_MIN)

#define MEAS_ACRCY 1000 /* accuracy */ 

#define MIN_PCL_DCS1800 28 /* minimum power control level DCS 1800 */
#define MAX_PCL_DCS1800 29 /* maximum power control level DCS 1800 */
#define MIN_PCL_GSM900  31 /* minimum power control level GSM 900  */
#define MAX_PCL_GSM900  0  /* maximum power control level GSM 900  */
#define MIN_PCL_PCS1900 21 /* minimum power control level PCS 1900 */
#define MAX_PCL_PCS1900 22 /* maximum power control level PCS 1900 */

#define GAMMA_0_DCS1800 36
#define GAMMA_0_GSM900  39
#define GAMMA_0_PCS1900 36
/*
end constants for MEAS
*/

/*
start constants for CS
*/
#define CS_RLA_P_VALID       0x0  /* RLA_P value is available and valid                   */
#define CS_RLA_P_NOT_VALID   0x1  /* RLA_P value is available but not valid.              */
#define CS_RLA_P_NOT_AVAIL   0x2  /* RLA_P value is not available and therefore not valid */


#define CS_KEEP_MEAS_RSLT    0x0         /* Keep all measurement results.  */
#define CS_DELETE_MEAS_RSLT  0x1         /* Delete all measurement results. */
#define CS_CRMM_BA_GPRS      0x0         /* Cell re-selection measurement on BA(GPRS), forward results to GRR. */
#define CS_CRMM_BA_BCCH      0x1         /* Cell re-selection measurement on BA(BCCH), forward results to ALR. */

#define CS_MIN_RLA_P_SAMPLES 5 /* At least 5 received signal measurement */
                                /* samples are required for a valid RLA_P */
                                /* value.                                 */

#define CS_MIN_RUN_AVG_PRD   1002 
                                /* RLA_P shall be a running average       */
                                /* determined using sample collected over */
                                /* a period of 5 sec. to ... sec.,        */
                                /* CS_MIN_RUN_AVG_PRD defines 5 sec.     */
                                /* minus 7.5 % in units of frames. 7.5 %  */
                                /* are substracted to be more tolerant    */
                                /* in time which allows to validate also  */
                                /* data which are colleted in less than   */
                                /* 5 sec.                                 */

#define CS_RPT_PRD_PTM       104
                                /* The MPHP_TCR_MEAS_IND primitive is     */
                                /* sent every 104 TDMA frames.            */

#define CS_NCSYNC_RPT_PRD_PIM 2080
                                /* CS_NCSYNC_RPT_PRD_PIM =2080 frame eqivalent to
                                   10 second 104 TDMA frames.            */

#define CS_MAX_MEAS_RSLT     10 
                                /* a maximum number of measurement        */
                                /* results are taken into account for     */
                                /* RLA_P calculation, exceeding samples   */
                                /* are ignored.                           */

#define CS_IDX_NOT_USED      0xFF

/* band limitations for multiband reporting */
#define CS_NO_BAND_LIMITATION   0
#define CS_EXCLUDE_SC_BAND      1
#define CS_ONLY_SC_BAND         2

/* maximum number of strongest carrier consider for cell re-selection */
#if defined FF_EOTD
  #define CS_MAX_STRONG_CARRIER    12
  #define CS_MAX_STRONG_CARRIER_CR 6

  #if ( ! ( CS_MAX_STRONG_CARRIER_CR <= CS_MAX_STRONG_CARRIER ) )
    #error CS_MAX_STRONG_CARRIER_CR <= CS_MAX_STRONG_CARRIER required
  #endif

#else  /* #if defined FF_EOTD */
  #define CS_MAX_STRONG_CARRIER    6
#endif /* #if defined FF_EOTD */

#if ( ! ( CS_MAX_STRONG_CARRIER <= RRGRR_MAX_ARFCN_NCELL_SYNC_REQ ) )
  #error CS_MAX_STRONG_CARRIER <= RRGRR_MAX_ARFCN_NCELL_SYNC_REQ required
#endif

#if defined (REL99) AND defined (TI_PS_FF_EMR) AND defined (TI_PS_FF_RTD)
#else
#if ( ! ( ( RRGRR_MEAS_REP_LIST_SIZE - 1 /* subtract serving cell */ ) <= CS_MAX_STRONG_CARRIER ) )
  #error RRGRR_MEAS_REP_LIST_SIZE <= CS_MAX_STRONG_CARRIER required
#endif
#endif

/* extrem values for signed 16 bit values */
#define CS_SMALLEST_SHORT_VALUE (-32768)

#define BSIC_NCC_MASK           0x38

#define NC_GET_TYPE(info_src)          ((info_src&0xF000)>>12)

#define NC_SET_TYPE(info_src,type)     (info_src=(USHORT)((info_src&0x0FFF)|((USHORT)type<<12)))
#define NC_SET_INSTANCE(info_src,inst) (info_src=(USHORT)((info_src&0xF0FF)|((USHORT)inst<< 8)))
#define NC_SET_NUMBER(info_src,num)    (info_src=        ((info_src&0xFF00)|((USHORT)num     )))

#define CS_RESELECT_CAUSE_CTRL_NORMAL   0
#define CS_RESELECT_CAUSE_CTRL_ABNORMAL 1
#define CS_RESELECT_CAUSE_CTRL_SCELL    2
#define CS_RESELECT_CAUSE_CS_NORMAL     3
#define CS_RESELECT_CAUSE_CTRL_DL_SIG_FAIL   4


typedef UBYTE T_CS_RESELECT_CAUSE;
typedef UBYTE T_CS_DC_STATUS;

#define MAX_NR_OF_NC_MVAL               ((2*MAX_NR_OF_NCELL)+1)
/*
end constants for CS
*/

/*
start constants for PSI
*/
#define CLIP_T_AVG(x)           ( (x) > 25 ? 25 : (x) )
#define CLIP_ALPHA(x)           ( (x) > 10 ? 10 : (x) )
#define CLIP_BS_PAG_BLKS_RES(x) ( (x) > 12 ? 0  : (x) )
#define CLIP_BS_PRACH_BLKS(x)   ( (x) > 12 ? 0  : (x) )
/*
end constants for PSI
*/

/*
start constants for interference measurements
*/

/* used in packet transfer mode */
#define MAX_IM_TRANSFER_CHANNELS 0x07

/* used in packet idle mode */
#define MAX_IM_CHAN_EMEAS      0x01
#define MAX_IM_CHAN_PCCCH      0x01

#define MAX_IM_IDLE_CHANNELS (MAX_CHAN_IMEAS + MAX_IM_CHAN_PCCCH + MAX_IM_CHAN_EMEAS)
                                    /* maximum number of channels measured in      */
                                    /* packet idle mode                            */

#define CHN_ID_INVALID         0xFF /* invalid identifier index                    */

#define CHN_ID_EM_BASE         0x00 /* EXT_FREQUENCY_LIST identifier index base    */
#define CHN_ID_PCCCH_BASE      ( CHN_ID_EM_BASE + MAX_IM_CHAN_EMEAS )
                                    /* monitored PCCCH identifier index base       */
#define CHN_ID_IM_BASE         ( CHN_ID_PCCCH_BASE + MAX_IM_CHAN_PCCCH )
                                    /* INT_MEAS_CHANNEL_LIST identifier index base */

/* used in packet idle and packet transfer mode */
#define IM_I_MASK_IDL          0x01
#define IM_I_MASK_TRNS         0x02
#define IM_I_NONE              0x00
#define IM_I_IDL               0x01
#define IM_I_TRNS              0x02

#define IM_U_MASK              0x01
#define IM_U_USED              0x01
#define IM_U_UNUSED            0x00

#define MAX_IM_HOP_ARFCN                548
#define MAX_IM_HOP_ARFCN_BIT_FIELD_SIZE ( ( ( MAX_IM_HOP_ARFCN - 1 ) / 8 ) + 1 )

#define MAX_IM_CHANNELS        ( MAX_IM_IDLE_CHANNELS + MAX_IM_TRANSFER_CHANNELS )
                                    /* maxmimum number of channels which are       */
                                    /* taken into account for interference         */
                                    /* measurement resumption                      */

/*
end constants for interference measurements
*/

/*
start constants for extended measurements
*/
#define MAX_NR_OF_INSTANCES_OF_PMO 8
#define EM_MAX_STRONG_CARRIER      6

#define EM_INVLD_BSIC              0xFF

#define EM_VLDTY_ARFCN_MASK        0x8000
#define EM_NON_VLD_ARFCN           0x8000
#define EM_VLD_ARFCN               0x0000

#define PMO_NONE_VLD               0x00
#define PMO_EXT_VLD                0x01
#define PMO_NC_VLD                 0x02
/*
end constants for extended measurements
*/

/*
start constants for NC measurements
*/

/*
end constants for NC measurements
*/

/*
start constants for packet measurement report sending
*/
#define PMR_MSG_TYPE_LEN       6
#define PMR_TLLI_LEN           32
#define PMR_FLAG_LEN           1
#define PMR_PSI5_CHNGE_MRK_LEN 2

#ifdef REL99
#define PMR_BA_IND_USED_LEN    1
#define PMR_PSI3_CHNGE_MRK_LEN 2
#define PMO_IND_USED_LEN       1
#endif

/*
end constants for packet measurement report sending
*/
#if defined  (REL99) AND defined (TI_PS_FF_EMR)
/*
 * start constants for ENH measurements
 */
#define SERVING_BAND_REPORTING_DEFAULT    3
#define NCC_PERMITTED_DEFAULT             0xFF   
#define NOT_APPLICABLE                    0xFF
#define BSIC_VALID                        0x1
#define BSIC_INVALID                      0x0
#define NC_INVBSIC_PEMR                   17
#define MSG_SIZE_PEMR                     184
#define MAND_SIZE_PEMR                    48
#define MAX_NR_OF_GSM_NC                  96    
#define REP_THRESHOLD_DEF                 0xFF                     
/*
 * end constants for ENH measurements
 */
#endif

#if defined (TI_PS_FF_RTD) AND defined (REL99)
/*
 * start constants for RTD
 */
#define RTD_DEFAULT_INDEX                 0
#define RTD_12BIT                         0x8000
#define RTD_NOT_AVAILABLE                 0xFFFF
/*
 * end constants for RTD
 */
#endif /* #if defined (TI_PS_FF_RTD) AND defined (REL99) */


/*
 * Number of instances of PSI
 */
#define MAX_NR_OF_INSTANCES_OF_PSI2       8 /* 1-8  */
#define MAX_NR_OF_INSTANCES_OF_PSI3       1 /* 1    */
#define MAX_NR_OF_INSTANCES_OF_PSI3BIS   16 /* 1-16 */
#if defined (REL99) AND defined (TI_PS_FF_EMR)
#define MAX_NR_OF_INSTANCES_OF_PSI3TER   16 /* 0-16 */
#endif
#define MAX_NR_OF_INSTANCES_OF_PSI4       8 /* 0-8  */
#define MAX_NR_OF_INSTANCES_OF_PSI5       8 /* 0-8  */
#ifdef REL99
#define MAX_NR_OF_INSTANCES_OF_PSI8       8 /* 0-8  */
#endif
#ifdef REL99
#define PBCCH_NOT_PRESENT    0x0           /* PBCCH not present in the cell  */
#define PBCCH_PRESENT        0x1           /* PBCCH present in the cell      */
#endif


#define NOT_SET 255

/*
 * Service definitions. Used to access service data with GET/SET_STATE.
 */


#define CTRL      ctrl.
#define CTRL_GLBL ctrl_glbl. /* global data, managed by CTRL, used by */
                             /* all GRR services                      */
#define TC        tc.
#define CPAP      cpap.
#define PSI       psi_assigned_data.
#define PG        pg.
#define MEAS      meas.
#define MEAS_IM   meas_im. /* service interference measurement */
#define MEAS_EM   meas_em. /* service extended measurement     */
#define CS        cs.
#define CS_MEAS   cs_meas. /* service cell re-selection measurement */
#define GFP       gfp.

#if defined (TI_PS_FF_RTD) AND defined (REL99)
typedef USHORT    T_RTD_VALUE;
#endif /* #if defined (TI_PS_FF_RTD) AND defined (REL99) */


/*
 * Timer definitions (also used by GRR_PEI.C)
 */

typedef enum
{
   T3134 = 0,       /* handled by service CTRL       */
   T3158,           /* handled by service CS         */
   T3162,           /* handled by service TC         */
   T3168,           /* handled by service TC         */
   T3170,           /* handled by service TC         */
   T3172_1,         /* handled by service TC         */

/* T3172_2, */      /* handeld by service TC         */
/* T3172_3, */      /* handeld by service TC         */
/* T3172_4, */      /* handeld by service TC         */
/* T3172_5, */      /* handeld by service TC         */
/* T3172_6, */      /* handeld by service TC         */
/* T3172_7, */      /* handeld by service TC         */

   T3174,           /* handled by service CTRL       */
   T3176,           /* handled by service CS         */
   T3178,           /* handled by service MEAS       */
   T3186,           /* handled by service TC         */
   T_60_SEC,        /* handled by service PSI        */
   T_30_SEC,        /* handled by service PSI        */
   T_10_SEC,        /* handled by service PSI        */
   T_IM_SYNC,       /* handled by service MEAS       */
   T15_SEC_CC,      /* handled by service CS         */
   T_TRANS_NON_DRX, /* handled by service PG         */
   T_NC2_NON_DRX,   /* handled by service PG         */
   T_COMP_PSI,      /* handled by service PSI        */
   T_RESELECT,      /* handled by service CS         */
#ifdef REL99
   T_POLL_TIMER,    /* handled by service CTRL       */
#endif

   TIMER_COUNT      /* !!! always the last entry !!! */    

} T_TIMER_INDEX;

/*      T3142_VALUE - assigned in message */
/*      T3158_VALUE - defined by parameter or random value */
#define T3162_VALUE       5000        /* ms */
/*      T3168_VALUE - assigned in system info (in GPRS Cell Options)
                      range 500,1000,...4000ms */
/*      T3172_VALUE - assigned in message */
#define T3174_VALUE       15000        /* ms */
#define T3176_VALUE       15000        /* ms */
/*      T3178_VALUE - defined by parameter or random value */
#define T3186_VALUE       5000        /* ms */
#define T3134_VALUE       5000        /* ms Timer to detect the lacks of respond
										from the  network or for unvavailable target cell*/

/*      T3200_VALUE - see T3198 */
/*      T_IM_SYNC_VALUE - defined by service MEAS */

#define T_10_SEC_VALUE    10000
#define T_30_SEC_VALUE    30000
#define T_60_SEC_VALUE    60000
#define T_RESELECT_VALUE  10000



/*
 * Service name definitions for trace purposes. The service abbrevation
 * (e.g. CTRL) has to be the same as above for the service definitions.
 */
#ifndef NTRACE

#define SERVICE_NAME_CTRL     "CTRL"
#define SERVICE_NAME_CTRL_GLBL "CTRL_GLBL"
#define SERVICE_NAME_TC       "TC"
#define SERVICE_NAME_CPAP     "CPAP"
#define SERVICE_NAME_PG       "PG"
#define SERVICE_NAME_MEAS     "MEAS"
#define SERVICE_NAME_MEAS_IM  "MEAS_IM"
#define SERVICE_NAME_MEAS_EM  "MEAS_EM"
#define SERVICE_NAME_PSI      "PSI"
#define SERVICE_NAME_CS       "CS"
#define SERVICE_NAME_CS_MEAS  "CS_MEAS"
#define SERVICE_NAME_GFP      "GFP"


#endif /* !NTRACE */


/*
 * Service CTRL
 */
#define CTRL_CELL_RESEL_NONE            0x00 /* no initiated cell re-selection */
#define CTRL_CELL_RESEL_RR              0x40 /* RR initiated cell re-selection */   
#define CTRL_CELL_RESEL_GRR             0x80 /* GRR initiated cell re-selection */
#define CTRL_CELL_RESEL_NW              0xC0 /* network initiated cell re-selection */

#define CTRL_CELL_RESEL_MASK            0xC0 /* bit maks for cell re-selection indication */

#define CTRL_SUSPENSION_NO              0x00 /* no suspension */
#define CTRL_SUSPENSION_YES             0x20 /* suspension */

#define CTRL_SUSPENSION_MASK            0x20 /* bit mask for suspension indication */

/* CTRL is in normal operation */
#define CTRL_NULL                       0x00 /* GPRS functionality is disabled */
#define CTRL_READY                      0x01 /* GPRS functionality is enabled, but service cannot be offered */
#define CTRL_ACCESS_DISABLED            0x02 /* service can be offered but is disabled */
#define CTRL_IN_PROGRESS                0x03 /* service is offered */

/* CTRL is waiting for several confirmations */
#define CTRL_WAIT_CS_PAGE_RES           0x04 /* waiting for response to circuit-switched paging */
#define CTRL_WAIT_FOR_CNF_OF_SUSPENSION 0x05 /* waiting for suspension of GRR sevices */
#define CTRL_WAIT_RSP_4_RR_CR           0x06 /* waiting for stopping of GRR services triggered by a RR indicated cell re-selection request */

/* CTRL is processing several cell change procedures */
#define CTRL_CR                        (0x07|CTRL_CELL_RESEL_GRR)
                                             /* GRR has triggered a MS controlled cell re-selection */
#define CTRL_FAILED_CR                  0x08 /* GRR triggered MS controlled cell re-selection has failed */
#define CTRL_RR_NC_CCO                 (0x09|CTRL_CELL_RESEL_NW)
                                             /* RR  has triggered a network controlled cell re-selection */
#define CTRL_GRR_NC_CCO                (0x0A|CTRL_CELL_RESEL_NW)
                                             /* GRR has triggered a network controlled cell re-selection */
#define CTRL_AUTO_RR_CR                (0x0B|CTRL_CELL_RESEL_RR)
                                             /* RR  has indicated a request for a cell re-selection */

/* GRR is suspended */
#define CTRL_SUSPENDED                 (0x0C|CTRL_SUSPENSION_YES)
                                              /* GRR is suspended */
#define CTRL_NO_CELL_SUSP              (0x0D|CTRL_SUSPENSION_YES)
                                              /* GRR is suspended and no cell is available */

#define CTRL_ACC_ENABLED_SUSP          (0x0E|CTRL_SUSPENSION_YES)
                                             /* GRR is suspended but after resume service is available */
#define CTRL_RR_CR_IN_SUSP             (0x0F|CTRL_CELL_RESEL_RR|CTRL_SUSPENSION_YES)
                                             /* GRR is suspended and RR has indicated a request for a cell re-selection */
#define CTRL_CR_IN_SUSP                (0x10|CTRL_CELL_RESEL_GRR|CTRL_SUSPENSION_YES)
                                             /* GRR is suspended and has triggered a cell re-selection */
#define CTRL_FAILED_CR_IN_SUSP          0x11 /* GRR is suspended and has detected a failed cell re-selection */

#define CTRL_READY_TO_PROGRESS          0x12 /* service can be offered but a cell reselection is in progress */

/*
 * Service TC
 */
#define TC_ACCESS_DISABLED          0
#define TC_CPAP                     1
#define TC_PIM                      2
#define TC_WAIT_ASSIGN              3
#define TC_POLLING                  4
#define TC_WAIT_2P_ASSIGN           5
#define TC_CONTENTION_RESOLUTION    6
#define TC_TBF_ACTIVE               7
#define TC_SINGLE_BLOCK             8
#define TC_WAIT_STOP_TASK_CNF       9



/*
 * Service CPAP
 */
#define CPAP_IDLE                 0
#define CPAP_SB_STARTED           1
#define CPAP_1P_STARTED           2
#define CPAP_WAIT_PDCH_ASSIGN     3
#define CPAP_WAIT_DEDI_SUSPEND    4
#define CPAP_WAIT_STOP_TASK_CNF   5


/*
 * Service PSI
 */
#define PSI_NULL                  0
#define PSI_IDLE                  1
#define PSI_TRANSFER              2
#define PSI_SUSPENDED             3
#define PSI_BCCH_IDLE             4
#define PSI_BCCH_TRANSFER         5

#define READ_COMPLETE                     0x0         /* Read PSI in all PBCCH blocks   */
#define READ_PSI1_IN_PSI1_REPEAT_PERIODS  0x1         /* Read PSI1 in PSI1 repeat periods */
#define PSI_IN_HR                         0x2         /* PSI in high repetition rate    */
#define PSI_IN_LR                         0x3         /* PSI in low repetition rate     */
#define PSI_IN_HR_AND_LR                  0x4         /* PSI in high and low repetition rate */
#define READ_PSI1_AND_IN_HR               0x5         /* PSI in high repetition rate and PSI1 */
#define READ_PSI1_AND_IN_LR               0x6         /* PSI in low repetition rate and PSI1 */
#define READ_PSI2                         0x7         /* Read PSI2                      */
#define READ_PSI3_3BIS                    0x8         /* Read PSI3 and PSI3bis          */
#define READ_PSI4                         0x9         /* Read PSI4                      */
#define READ_PSI5                         0xa         /* Read PSI5                      */
#ifdef REL99
#ifdef TI_PS_FF_EMR
#define READ_PSI3BIS                      0xb         /* Read PSI3bis  */
#define READ_PSI3TER                      0xc         /* Read PSI3ter  */
#define READ_PSI3_3BIS_3TER               0xd         /* Read PSI3, PSI3bis and PSI3ter */
#endif
#define READ_PSI8                         0xe         /* Read PSI8. this value is used while performing partial acquisition */  
#endif                               
                                  
/*
 * Service PG
 */
#define PG_NULL           0
#define PG_IDLE           1
#define PG_TRANSFER       2
#define PG_ACCESS         3


/*
 * Service MEAS
 */
#define MEAS_NULL         0
#define MEAS_IDLE         1
#define MEAS_ACCESS       2
#define MEAS_TRANSFER     3


/*
 * Service MEAS_IM
 */
#define MEAS_IM_NULL      0
#define MEAS_IM_START     1
#define MEAS_IM_STOP_REQ  2


/*
 * Service MEAS_EM
 */
#define MEAS_EM_NULL           0 /* no activity ongoing except that timer   */
                                 /* T3178 is maybe running                  */


#define MEAS_EM_REP_REQ        1 /* waiting for measurement report data     */
#define MEAS_EM_PMR_SENDING    2 /* measurement report is currently sent to */
                                 /* the network                             */
#define MEAS_EM_PENDING        3 /* measurements are paused, e.g. TBF is    */
                                 /* active                                  */
#define MEAS_EM_SUSPEND        4 /* measurements are suspended, e.g. RR     */
                                 /* connection establishment in progress    */

/*
 * Service CS
 */
#define CS_NULL               0 /* no activity ongoing                        */
#define CS_IDLE               1 /* service CS in idle mode, cell re-selection */
                                /* and/or NC measurements are performed       */
#define CS_CR_MOBILE          2 /* MS controlled cell re-selection ongoing    */
#define CS_CR_NETWORK         3 /* network controlled cell re-selection       */
                                /* ongoing                                    */
/*
 * Service CS_MEAS
 */
#define CS_MEAS_NULL          0 /* no activity ongoing                        */
#define CS_MEAS_REP_REQ       1 /* waiting for measurement report data        */
#define CS_MEAS_PMR_SENDING   2 /* measurement report is currently sent to    */
                                /* the network                                */

/*
 * Service CS_DC
 */
#define CS_DC_DEDICATED       0 /* CS is stopped due to dedicated connection  */
#define CS_DC_OTHER           1 /* CS is stopped due to other reasons         */


/*
 * Service GFP
 */

#define GFP_IDLE      0

#ifdef REL99
#define GRR_PCCO_POLL 1
#endif


/*==== TYPES ======================================================*/

/*
 * GRR global typedefs
 */

/* enums */
typedef enum
{
  REL_PERFORMED,
  REL_RUNNING
}T_RELEASE_STATE;

/*
 * mapping of the TC disable cause
 *
 * 7 6 5 4 3 2 1 0 - bit position
 *       | | | | |
 *       | | | +-+-- 00  OTHER
 *       | | | +-+-- 01  SUSPEND
 *       | | | +-+-- 10  CR
 *       | | |
 *       +-+-+------ 000 OTHER
 *       +-+-+------ 001 NORMAL
 *       +-+-+------ 010 IMM_REL
 *       +-+-+------ 011 NETWORK
 *       +-+-+------ 100 ABNORMAL
 */


/* disable cause class */
#define DCC_OTHER              0x00
#define DCC_SUSPEND            0x01
#define DCC_CR                 0x02

typedef UBYTE T_TC_DC_CLASS;

/* disable cause type */
#define DCT_OTHER             (0x00<<2)
#define DCT_NORMAL            (0x01<<2)
#define DCT_IMM_REL           (0x02<<2)
#define DCT_NETWORK           (0x03<<2)
#define DCT_ABNORMAL          (0x04<<2)

typedef UBYTE T_TC_DC_TYPE;

/* disable cause mask */
#define DCM_CLASS              0x03
#define DCM_TYPE              (0x07<<2)

#define TC_DC_OTHER           (DCC_OTHER  |DCT_OTHER   )
#define TC_DC_SUSPEND_NORMAL  (DCC_SUSPEND|DCT_NORMAL  )
#define TC_DC_SUSPEND_IMM_REL (DCC_SUSPEND|DCT_IMM_REL )
#define TC_DC_CR_NETWORK      (DCC_CR     |DCT_NETWORK )
#define TC_DC_CR_NORMAL       (DCC_CR     |DCT_NORMAL  ) /* RRGRR_CR_IND( CR_NORMAL   )                */
#define TC_DC_CR_IMM_REL      (DCC_CR     |DCT_IMM_REL ) /* RRGRR_CR_IND( CR_ABNORMAL )                */
#define TC_DC_CR_ABNORMAL     (DCC_CR     |DCT_ABNORMAL) /* abnormal TBF release with cell reselection */

typedef UBYTE T_TC_DISABLE_CAUSE;

typedef enum
{
  PSI_DC_OTHER,
  PSI_DC_READ_PSI,
  PSI_DC_PBCCH_RELEASED,
  PSI_DC_PBCCH_ESTABLISHED
} T_PSI_DISABLE_CAUSE;



typedef enum
{
  TRANSFER_NDRX,
  GMM_NDRX,
  NC2_NDRX
}T_NON_DRX_TYPE;

typedef enum
{
  PACKET_MODE_NULL,
  PACKET_MODE_PIM,    /* Packet Idle Mode */
  PACKET_MODE_PAM,    /* Packet Access Mode */
  PACKET_MODE_PTM,    /* Packet Transfer Mode */
  PACKET_MODE_2P_PTM  /* Packet Transfer Mode in 2P Access*/
} T_PACKET_MODE;

typedef UBYTE T_TBF_TYPE; /* macros are defined in cgrlc SAP*/


typedef enum
{
  CTRL_PTRC_NONE,
  CTRL_PTRC_PBCCH_ESTABLISHED,
  CTRL_PTRC_COMPLETE_SI_READING,
  CTRL_PTRC_COMPLETE_PSI_READING
} CTRL_PSI_TBF_REL_CAUSE;

typedef enum /* CTRL_FOLLOWING_SUSPENSION*/
{
  ACTIVATE_RR,            /*  activate RR to monitor CCCH and BCCH*/
  SEND_RR_EST_REQ,        /*  RR has to start RR establishment */
  SEND_RR_EST_RSP         /*  RR can accept the RR connection request*/
}CTRL_FOLLOWING_SUSPENSION;

typedef enum /*PG_STATES*/
{
  PG_NORMAL,
  PG_EXTENDED,
  PG_REORG
}PG_STATES;

typedef enum /*PG_CHANNEL for CS call*/
{
  PG_ON_CCCH,
  PG_ON_PCCCH
}PG_CHANNEL;

typedef enum /* ACQ_TYPE*/
{
  NONE,
  PARTIAL,
  COMPLETE,
  PERIODICAL_PSI1_READING,
  PERIODICAL_SI13_READING,
  FULL_PSI_IN_NEW_CELL
}ACQ_TYPE;


typedef enum /* T_FA_TYPE*/
{
  FA_NO_CURRENT,
  FA_NO_NEXT,
  FA_BITMAP,
  FA_REPEAT
}T_FA_TYPE;


/* start enum for TC */

/*
 * The following definitions are used for the management of single block
 * sending without TBF establishment.
 *
 * TC holds a queue of size MSG_OWNER_MAX_NUM to buffer the single blocks
 * requested by the services. Only services MEAS and CS can request single
 * blocks in parallel. In case service CTRL has requested a single block,
 * services MEAS and CS are not allowed to request any single blocks.
 *
 * Service CS uses the same buffer as the service CTRL. Only one single block
 * can be stored for both services. The service MEAS has an individual buffer.
 * At this time only one single block can be stored for this service but we are
 * free to increase the size just by increasing the value of MAX_CTRL_BLK_NUM.
 */
#define MAX_CTRL_BLK_NUM    5 /* number of single blocks that can be stored at */
                              /* the same time                                 */

#define BLK_INDEX_CTRL      0 /* Packet Cell Change Failure                    */

#define BLK_INDEX_CS        0 /* Packet Measurement Report (NC Measurements)   */

#define BLK_INDEX_TC        1 /* Packet Resource Request (Reallocation)        */

#define BLK_INDEX_MEAS      2 /* Packet Measurement Report (EXT Measurements)  */


#define BLK_STATE_NONE      0 /* no control message associated                 */
#define BLK_STATE_ALLOCATED 1 /* control message is allocated                  */
#define BLK_STATE_SENT_REQ  2 /* control message is sent, waiting for response */

typedef UBYTE T_BLK_STATE;
typedef UBYTE T_BLK_OWNER;

/* end um for TC */

/* start enum for CPAP */

/*
 * The following enum will be used for IA and IAEXT
 */
typedef enum
{
  E_IA_NULL,             /* unforeseen mesage  content*/
  E_IA_SB_WITHOUT_TBF,   /* Single Block without TBF Establishment */
  E_IA_SB_2PHASE_ACCESS, /* Allocation for 2 Phase of 2_Phase-Access */
  E_IA_UL,               /* Uplink tbf Allocation */
  E_IA_DCCH,             /* continue on DCCH */
  E_IA_TMA,              /* it is the 1st of a 2 message assignment */
  E_IA_TMA_SECOND,       /* it is the 2nd of a 2 message assignment */
  E_IA_ERROR_RA        
} T_EVAL_IA;                    

typedef enum
{
  E_IA_DL_NOT_OURS,   /* message is not adressed to MS */
  E_IA_DL_IGNORE,     /* no effect on current tbf */
  E_IA_DL_TMA,        /* it is the 1st of a 2 message dl assignment */
  E_IA_DL_TMA_SECOND, /* it is the 2nd of a 2 message dl assignment */
  E_IA_DL_DCCH,       /* continue on DCCH */
  E_IA_DL_SB,         /* single block downlink */
  E_IA_DL_ASSIGN      /* valid message send mphp_assign_req */
} T_EVAL_IA_DL;

#define IA_TYPE_UL 0 /* last immediate assignment was for uplink direction   */
#define IA_TYPE_DL 1 /* last immediate assignment was for downlink direction */

typedef UBYTE T_IA_TYPE;



/* start enum for CTRL */

typedef enum /* T_FAILURE_SIGNAL */
{
  FAIL_SIG_DSF_IND,
  FAIL_SIG_PSI1OR_PSI13_RECEIPTION_FAILURE,
  FAIL_SIG_CTRL_ACCESS_BARRED
} T_FAILURE_SIGNAL;

/* end enum for CTRL */

/* start enum for interference measurements */

#define IM_MODE_NONE          0 /* no interf. meas. at all                     */
#define IM_MODE_IDLE          1 /* interf. meas. only in Packet Idle Mode      */
#define IM_MODE_TRANSFER      2 /* interf. meas. only in Packet Transfer Mode  */
#define IM_MODE_IDLE_TRANSFER 3 /* interf. meas. in Packet Transfer and Packet */
                                /* Idle Mode                                   */
typedef UBYTE T_MEAS_IM_MODE;

#define IM_TRIG_SIGNAL_NULL   0
#define IM_TRIG_SIGNAL_PAM    2
#define IM_TRIG_SIGNAL_PAUSE  3

typedef UBYTE T_MEAS_IM_TRIG_SIGNAL;

#define M_IM_TRACE_INPUT_VALUE  0x01 /* trace mask for the input values        */
#define M_IM_TRACE_OUTPUT_VALUE 0x02 /* trace mask for the output values       */
#define M_IM_TRACE_FREQUENCY    0x04 /* trace mask for the carrier frequencies */

/* end enum for interference measurements */

/* start enum for extended measurements */

typedef enum /* T_XMEAS_ORDER */
{
  EM_EM0,
  EM_EM1,
  EM_RESET,
  EM_EMPTY
} T_XMEAS_ORDER;

/* end enum for extended measurements */

/* start enum for NC measurements */

typedef enum
{
  NC_NC0    = 0,
  NC_NC1,
  NC_NC2,
  NC_RESET,
  NC_EMPTY
} T_NC_ORDER;

/* !!! do not start with value 0, use 1 instead !!!                              */
#define INFO_TYPE_PSI3         1 /* Packet System Information Type 3             */
#define INFO_TYPE_PSI3BIS      2 /* Packet System Information Type 3bis          */
/* !!! value of INFO_TYPE_PSI3 shall be less than value of INFO_TYPE_PSI3BIS !!! */
#define INFO_TYPE_PMO          3 /* Packet Measurement Order                     */
#define INFO_TYPE_PCCO         4 /* Packet Cell Change Order                     */
#define INFO_TYPE_BA_BCCH      5 /* BA(BCCH) passed by RR                        */

typedef UBYTE T_INFO_TYPE;

/* end enum for NC measurements */

/* start enum for cell re-selection */

#define CS_MODE_IDLE                0 /* entry not touched in any way           */
#define CS_MODE_SELECTION_PERFORMED 1 /* entry selected as next cell to be used */
                                      /* for cell re-selection and we have      */
                                      /* performed reading of the new cell      */

typedef UBYTE T_CS_MODE;

#define STAT_SYNC_NONE              0 /* not synchronised                       */
#define STAT_SYNC_OK                1 /* synchronised                           */
#define STAT_SYNC_FAILED            2 /* synchronisation failed                 */

typedef UBYTE T_SYNC_STATUS;

#define NC_MVAL_STAT_ASSIGNED       0 /* on current BA(GPRS) list, no member    */
                                      /* of the six strongest cells             */
#define NC_MVAL_STAT_PENDING        1 /* not on current BA(GPRS) list, update   */
                                      /* from RR required                       */
#define NC_MVAL_STAT_NONE           2 /* not on current BA(GPRS) list, no       */
                                      /* update from RR required                */
typedef UBYTE T_NC_MVAL_STATUS;

/* end enum for cell re-selection */

/*
 * Structure
 */
typedef struct
{
  UBYTE   num; /* Current number -identification- of the rfl*/
  
  UBYTE   list[128];   /* Buffer containing received frequencies: 
                       * This is the same structure as the T_LIST in RR.h
                       */
} T_RFL_LST;

typedef struct /* T_CR_POW_PAR */
{
  UBYTE           gprs_rxlev_access_min; /* GPRS_RXLEV_ACCESS_MIN        */
  UBYTE           gprs_ms_txpwr_max_cch; /* GPRS_MS_TXPWR_MAX_CCH        */
} T_CR_POW_PAR;

typedef struct /* T_CR_PAR_1 */
{
  T_CR_POW_PAR    cr_pow_par;            /* GPRS power parameter         */
  UBYTE           v_hcs_par;             /* valid flag for HCS structure */
  T_hcs_par       hcs_par;               /* HCS structure                */
} T_CR_PAR_1;

typedef struct /* T_SCELL_PAR */
{
  UBYTE                     cell_ba;
  UBYTE                     exc_acc;
  T_CR_PAR_1                cr_par_1;
  UBYTE                     multi_band_rep;
} T_SCELL_PAR;

typedef struct /* T_UL_TBF */
{
  UBYTE             tfi;
  UBYTE             st_tfi;       /* tfi will be used after starting time elapsed */
  UBYTE             mac_mode;
  UBYTE             polling_bit;
  UBYTE             ts_mask;      /* current timeslot mask */
  UBYTE             ts_usage;     /* new assigned timeslot mask, becomes valid after starting time has elapsed */
  UBYTE             nts;          /* number of timeslots allocated for tbf */
  UBYTE             cs_mode;
  UBYTE             tlli_cs_mode;
  UBYTE             ti;
  ULONG             tbf_start_fn;
  USHORT            rlc_db_granted;

  UBYTE             access_type;
  UBYTE             nr_blocks;
  UBYTE             prio;
  UBYTE             prim_type;
  USHORT            peak;
  USHORT            rlc_oct_cnt;
} T_UL_TBF;


typedef struct /* T_DL_TBF */
{
  ULONG             tbf_start_fn;
  UBYTE             tfi;
  UBYTE             st_tfi;       /* tfi will be used after starting time elapsed */
  UBYTE             rlc_mode;
  UBYTE             mac_mode;  
  UBYTE             polling_bit;
  UBYTE             ctrl_ack_bit;
  UBYTE             ts_mask;      /* current timeslot mask */
  UBYTE             ts_usage;     /* new assigned timeslot mask, becomes valid after starting time has elapsed */
  UBYTE             nts;          /*  number of timeslots allocated for tbf */
  BOOL              t3192;        /*  if true t3192 is running, else timer is not running, triggerd by GRLC*/
  UBYTE             trans_id;     /* transaction id */

} T_DL_TBF;
typedef UBYTE T_RXLEV_AVG;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
/* Start Enhanced Measurement Data Structures */

typedef struct /* T_GPRS_REPT_PRIO_DESC */
{
  UBYTE      num_cells;                    /* No. of cells for which report priority is available */
  UBYTE      rept_prio[MAX_NR_OF_GSM_NC];  /* Report priority for cells in BA */
} T_GPRS_REPT_PRIO_DESC;

/*
 * Enhanced data for reporting
 */
typedef struct /* T_ENH_REP_DATA */
{
  UBYTE                        rep_threshold;            /* Reporting threshold */
  UBYTE                        rep_offset;               /* Reporting offset */
} T_ENH_REP_DATA;

typedef struct /* T_GRR_ENH_PARA */
{
  UBYTE                        idx;                      /* to get on which instance of message */
                                                         /* the parameters are received */
  UBYTE                        rept_type;                /* PMR or PEMR */
  UBYTE                        rep_rate;                 /* Reporting rate */
  UBYTE                        inv_bsic_enabled;         /* Is Invalid BSIC reporting enabled? */
  UBYTE                        ncc_permitted;            /* NCC permitted bitmap */
  UBYTE                        multiband_rep;            /* Number of cells, in each non-serving cell band, to be reported */
  UBYTE                        servingband_rep;          /* Number of cells, in serving cell band, to be reported */
  UBYTE                        scale_order;              /* scale order */
  T_ENH_REP_DATA               enh_rep_data[MAX_NUM_BANDS]; /* Enhanced data for reporting */
  T_GPRS_REPT_PRIO_DESC        gprs_rept_prio_desc;      /* Report priority */
} T_GRR_ENH_PARA;

/*
/ * Used to store GSM NC list formed in RR, when PBCCH is absent * /
typedef struct  / * T_BA_BCCH_NCELL_INFO * /
{
  UBYTE             index;             / * index used for measurement reporting * /
  USHORT            arfcn;             / * Radio frequency * /
  UBYTE             bsic;              / * BSIC * /
} T_BA_BCCH_NCELL_INFO;
*/

typedef struct /* T_MEAS */
{
  USHORT arfcn;   
  UBYTE  rxlev; /* Actually, this is RLA_P */
} T_MEAS;

/* To form a ARFCN-RXLEV pair for coming out with list in desc order of RXLEV */
typedef struct /* T_ENH_STRNG_ARFCN_LIST */
{
  T_MEAS meas[MAX_NR_OF_NCELL];   
  UBYTE  num; 
} T_ENH_STRNG_ARFCN_LIST;

typedef struct /* T_BSIC */
{
  UBYTE bsic;   /* Decoded BSIC */
  UBYTE status; /* Valid, if present in BA-GPRS, Else, Invalid. */
} T_BSIC ;

typedef struct /* T_ENH_CELL_LIST */
{
  UBYTE             index;        /* BA-GPRS Index */
  USHORT            arfcn; 
  T_BSIC            bsic;
  UBYTE             rla_p;   /* Actually, this is used for ENH meas criteria */
  T_RXLEV_AVG       rxlev_avg; /* running avg of RXLEV which is reported */
  UBYTE             rept_prio;
} T_ENH_CELL_LIST;

/*
typedef struct / * T_ENH_BIN * /
{
  UBYTE     num_valid;  / * No. of cells that are eligible for ENH measurement
                           reporting * /
  T_ENH_CELL_LIST *enh_cell_list[MAX_NR_OF_NCELL];
} T_ENH_BIN;
*/

typedef struct /* T_ENH_BIN */
{
  UBYTE     num_valid;  /* No. of cells that are eligible for ENH measurement
                           reporting */
  UBYTE     enh_index[MAX_NR_OF_NCELL];
} T_ENH_BIN;

typedef struct /* T_PEMR_PARAMS */
{
  UBYTE     scale_used; /* Scale used for RXLEV before ENH reporting */
} T_PEMR_PARAMS;
  
/* End Enhanced Measurement Data Structures */
#endif

typedef struct /* T_IMSI */
{
  UBYTE               number_of_digits;  /*  number of valid digits in the following digit array */
  UBYTE               digit[MAX_IMSI_DIGITS];/*  here are the valid imsi digits stored: received from GMM */
}T_IMSI;

typedef struct /* T_MS_IDENT */
{
  ULONG                 ul_tlli;          /* uplink tlli used for packet resource request and contention resolution */
  ULONG                 received_tlli;      /*  tlli with which the MS is addressed */
  ULONG                 old_tlli;         /*  old tlli received from GMM*/
  ULONG                 new_tlli;        /*  new tlli received from GMM*/
  ULONG                 old_ptmsi;     /*  current  ptmsi paging */
  ULONG                 new_ptmsi;        /*  candidate ptmsi stored: for paging */
  ULONG                 tmsi;              /*  here is the valid tmsi stored: for paging */
  T_IMSI                imsi;             /*  imsi structure is stored in this parameter*/
  T_rai                 rai;              /*  routing area code received from GMM to be passed to RR */
} T_MS_IDENT;

typedef struct /* T_MS_DATA */
{
  USHORT                access_ctrl_class; /*                      */
  UBYTE                 class_of_mode;     /* A, BC, BG, CC, CG    */
  USHORT                split_pg_cycle;    /* SPLIT_PAGING_CYCLE   */
  UBYTE                 reverts_NMO_III;   /* Class B mobile reverts into Class C mobile in NMO III or not */
  UBYTE                 tbf_mon_ccch;      /* monitoring of CCCH during TBF allowed (TRUE) or not (FALSE)  */
} T_MS_DATA;

typedef struct /* T_SCELL */
{
  USHORT        arfcn;         /* ARFCN of the serving cell          */
  UBYTE         bsic;          /* BSIC of the serving cell           */
  BOOL          pbcch_present; /* this value will be set by the procedure   */
                               /* which handles the SI13 message            */
} T_SCELL;

typedef struct /* T_PBCCH */
{
  T_SCELL       bcch;       /* frequency and BSIC of BCCH carrier        */
  T_pbcch_des   pbcch_des;  /* this value contains the PBCCH description */
} T_PBCCH;

typedef struct /* T_SPGC_CCCH_SUPP */
{
  UBYTE            ms_supp;             /* mobile station support of split  */
                                        /* paging cycle on CCCH             */
  UBYTE            nw_supp;             /* network support of split paging  */
                                        /* cycle on CCCH                    */
}T_SPGC_CCCH_SUPP;

typedef struct /* T_NETWORK_CONTROL */
{
  T_SPGC_CCCH_SUPP spgc_ccch_supp;      /* SPLIT_PG_CYCLE_SUPPORT: whether it is supported or not   */
  UBYTE            priority_access_thr; /* packet access to the network according to priority level */
  UBYTE            bs_pa_mfrms;         /* number of 51 multiframes between */
                                        /* transmission of paging messages  */
} T_NETWORK_CONTROL;

typedef enum /* T_CR_IND_RETURN */
{
  CR_IND_RETURN_NONE,
  CR_IND_RETURN_RESPONSE,
  CR_IND_RETURN_COMPLETE
} T_CR_IND_RETURN;

typedef struct /* T_RR_STATES */
{
  BOOL            monitoring_stopped; /* whether the monitoring on CCCH is released or not */
  T_CR_IND_RETURN cr_ind_return;      /* cell reselection started or not                   */
}T_RR_STATES;

typedef enum /* T_GMM_STATES */
{
  STANDBY_STATE,  /* GMM is in stand by state */
  READY_STATE     /* GMM is in ready state    */
}T_GMM_STATES;

typedef struct /* T_PCCCH: hopping and non hopping*/
{
  BOOL        is_static;  /*  indicates whether the static parameters are valid or not*/
  UBYTE       tsc;        /* common*/
  UBYTE       tn;         /* common*/
  USHORT      arfcn;
  UBYTE       maio;
  UBYTE       ma_num;     /* Mobile Allocation number*/
}T_PCCCH;

typedef struct /* T_PAGING_GROUP */
{
  UBYTE kc; /* number of PCCCHs [1...16]*/
  T_PCCCH     pccch[MAX_PCCCH_DES]; /* this structure contains ARFCN, TSC, TN, MAIO (non) hopping*/
}T_PAGING_GROUP;

typedef struct /* T_EXT_FREQ_LIST */
{
  UBYTE            number;
  USHORT           freq[RRGRR_MAX_ARFCN_EXT_MEAS];
} T_EXT_FREQ_LIST;

typedef struct /* T_XMEAS_EM1_PARAM */
{
  UBYTE            reporting_type;
  UBYTE            ncc_permitted;
  UBYTE            int_frequency; /* is set to NOT_SET, if no data available*/
  UBYTE            reporting_period;
} T_XMEAS_EM1_PARAM;

typedef struct /* T_XMEAS_EM1 */
{
  T_XMEAS_EM1_PARAM param;
  T_EXT_FREQ_LIST   list;
} T_XMEAS_EM1;

typedef struct /* T_XMEAS */
{
  UBYTE           idx;            /* to get on which instance of message */
                                  /* the parameters are received         */
  T_XMEAS_ORDER   em_order_type;
  T_XMEAS_EM1     em1;
} T_XMEAS;

typedef struct /*T_int_meas_chan_lst*/
{
  UBYTE           v_arfcn;        /* valid-flag   for arfcn         */
  USHORT          arfcn;          /* ARFCN                          */

  UBYTE           v_ma_num_maio;  /* valid-flag   for ma and maio   */
  UBYTE           ma_num;         /* MA_NUMBER                      */
  UBYTE           maio;           /* MAIO                           */

  UBYTE           ts_alloc;       /* TIMESLOT_ALLOCATION            */
} T_int_meas_chan_lst;

typedef struct /* T_si13_pbcch_loc*/
{
  UBYTE v_si13_location;            /*  valid flag for si13 location. If set: si13_location is valid.
                                     *  Otherwise pbcch_location and psi1_repeat_period are valid.
                                     */
  UBYTE si13_location;

  UBYTE pbcch_location;
  UBYTE psi1_repeat_period;
} T_si13_pbcch_loc;

typedef struct
{
  U8                        stat;  /* Status of RLA_P value         */
  U8                        lev;   /* Received signal level measurement.     */
} T_rla_p;

typedef struct /* T_CR_CRITERIONS */
{
  SHORT             c1;             /* path loss criterion                   */
  SHORT             c31;            /* signal level threshold criterion      */
  SHORT             c32;            /* cell ranking criterion                */
} T_CR_CRITERIONS;

//typedef UBYTE T_RXLEV_AVG;

typedef struct /* T_scell_info */
{
  T_rla_p           rla_p;        /* running average of receive signal level */
  T_RXLEV_AVG       rxlev_avg;    /* running average of RXLEV value          */
  T_CR_CRITERIONS   cr_crit;      /* cell re-selection criterions            */
  UBYTE             cell_barred;  /* in case CS is requested to re-select    */
                                  /* a new serving cell, this flag avoid     */
                                  /* indentifying the current serving cell   */
                                  /* as the best one and suppress re-        */
                                  /* selection                               */
} T_scell_info;


typedef struct /* T_SYNC */
{
  T_SYNC_STATUS     status;            /* synchronisation status */
  UBYTE             sync_failed_cnt;   /* number of failed synchronisation attempts */
} T_SYNC;


typedef struct /* T_SYNC_INFO */
{
  T_SYNC            sync;              /* synchronisation */
  UBYTE             bsic;              /* BSIC */
} T_SYNC_INFO;

typedef struct /* T_CR_OFFSET */
{
  UBYTE             gprs_temp_offset;  /* GPRS_TEMPORARY_OFFSET */
  UBYTE             gprs_penalty_time; /* GPRS_PENALTY_TIME */
} T_CR_OFFSET;

typedef struct /* T_CR_PAR */
{
  UBYTE             cell_ba;           /* CELL_BAR_ACCESS_2 */

  UBYTE             exc_acc;           /* EXC_ACC */

  UBYTE             same_ra_scell;     /* SAME_RA_AS_SERVING_CELL */

  T_CR_PAR_1        cr_par_1;          /* basic cell re-selection parameter */

  T_CR_OFFSET       cr_offset;         /* parameter for GPRS temporary offset */

  UBYTE             gprs_resel_off;    /* GPRS_RESELECT_OFFSET */

  UBYTE             v_si13_pbcch;      /* whether SI13 PBCCH location is present or not
                                        * If not included, SI3 and SI4 in the neighbour
                                        * cell indicates if the neighbour cell supports GPRS
                                        */
  T_si13_pbcch_loc  si13_pbcch;        /* SI13_PBCCH_LOCATION structure */

} T_CR_PAR;

typedef USHORT T_INFO_SRC;

typedef struct /* T_NC_MVAL, measured values of one neighbour cell */
{
  T_rla_p           rla_p;             /* running average of receive signal level */
  T_RXLEV_AVG       rxlev_avg;         /* running average of RXLEV value */
  T_SYNC_INFO       sync_info;         /* information about synchronisation */
  USHORT            arfcn;             /* radio frequency */
  T_NC_MVAL_STATUS  status;            /* status of the entry */
} T_NC_MVAL;

typedef struct /* T_NC_MVAL_LIST */
{
  T_NC_MVAL         nc_mval[MAX_NR_OF_NC_MVAL];
} T_NC_MVAL_LIST;

typedef struct /* T_ncell_info */
{
  UBYTE             index;             /* index used for measurement reporting */
  T_INFO_SRC        info_src;          /* indicates which air interface message */
                                       /* was used to carry neighbour cell information */
  USHORT            arfcn;             /* Radio frequency */
  UBYTE             bsic;              /* BSIC */
  UBYTE             v_cr_par;          /* indicates whether cell re-selection parameter are valid */
  T_CR_PAR          cr_par;            /* cell re-selection parameter */
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  UBYTE             last_rxlev;        /* RXLEV from the previous rep period */
  UBYTE             rep_count;         /* this is used to get how many times a
                                          cell in enhanced neighbour cell list
                                          is reported. Will be of use when 
                                          reporting low priority cells at 
                                          reduced rate */
#endif
} T_ncell_info;

typedef struct /* T_NC_CHNG_MRK */
{
  UBYTE           prev;           /* old change mark value */
  UBYTE           curr;           /* new change mark value */
} T_NC_CHNG_MRK;

typedef struct /* T_NC_PARAM */
{
  T_NC_CHNG_MRK   chng_mrk;       /* change mark                                 */
  UBYTE           idx;            /* to get on which instance of message         */
                                  /* the parameters are received                 */
  T_NC_ORDER      ctrl_order;     /* NETWORK_CONTROL_ORDER, NC0, NC1, NC2, RESET */
  UBYTE           non_drx_per;    /* NC_NON_DRX_PERIOD                           */
  UBYTE           rep_per_i;      /* NC_REPORTING_PERIOD_I                       */
  UBYTE           rep_per_t;      /* NC_REPORTING_PERIOD_T                       */
} T_NC_PARAM;

typedef struct /* T_NC_LIST */
{
  T_NC_CHNG_MRK   chng_mrk;              /* change mark               */
  UBYTE           number;                /* number of neighbour cells */
  T_ncell_info    info[MAX_NR_OF_NCELL]; /* neighbour cell info       */
} T_NC_LIST;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
typedef struct /* T_BA_BCCH_NC_LIST */
{
  UBYTE           number;                /* number of neighbour cells */
  T_ncell_info    info[MAX_NR_OF_NCELL]; /* neighbour cell info       */
} T_BA_BCCH_NC_LIST;
#endif

typedef struct /* T_NCMEAS */
{
  T_NC_PARAM      param;
  T_NC_LIST       list;
} T_NCMEAS;

typedef struct /* T_NC_RFREQ_LIST */
{
  UBYTE           number;
  UBYTE           idx[MAX_NR_OF_NCELL];
} T_NC_RFREQ_LIST;

typedef struct /* T_NCMEAS_EXTENDED */
{
  T_NCMEAS        ncmeas;
  T_NC_RFREQ_LIST rfreq;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  UBYTE           psi3_cm; /* PSI3 CM received in PCCO */
  UBYTE           ba_ind;  /* BA_IND received in PCCO */
  UBYTE           pmo_ind; /* PMO sequence number */
#endif
} T_NCMEAS_EXTENDED;

typedef struct /* T_CNT_NC_SIX_STRGST */
{
  UBYTE            strgst;     /* number of carriers which belongs to the   */
                               /* strongest, valid RLA_P is required        */
  UBYTE            candid;     /* number of carriers which belongs to the   */
                               /* strongest plus candidates which may       */
                               /* become member of the strongest, available */
                               /* but not necessarily valid RLA_P is        */
                               /* required                                  */
} T_CNT_NC_SIX_STRGST;

typedef struct /* T_NC_SIX_STRGST */
{
  UBYTE            idx;
  T_CS_MODE        mode;       /* indicates the mode of an individual entry */
                               /* of the list of the six strongest cells    */
  ULONG            avail_time; /* time when inserted in list                */
  T_CR_CRITERIONS  cr_crit;    /* cell re-selection criterions              */
} T_NC_SIX_STRGST;

typedef struct /* T_NC_REF_LIST */
{
  T_NC_PARAM      *param;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  T_GRR_ENH_PARA  *enh_param;
#endif
  UBYTE            number;
  T_ncell_info    *info[MAX_NR_OF_NCELL+1];
                                /* there are MAX_NR_OF_NCELL carriers for      */
                                /* MS controlled cell re-selection and one for */
                                /* NW controlled in case the selected cell is  */
                                /* not in the neighbour cell list              */
} T_NC_REF_LIST;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
typedef struct /* T_BA_BCCH_NC_REF_LIST */
{
  UBYTE           number;
  T_ncell_info    *info[2*MAX_NR_OF_NCELL];
} T_BA_BCCH_NC_REF_LIST;
#endif


/*start of structs for GFP */



typedef struct
{
  U8                        msg_type;           
  U8                        rrbp;               
  U8                        sp;                 
  U8                        usf;                
  U8                        v_rbsn;             
  U8                        rbsn;               
  U8                        rti;                
  U8                        fs;                 
  U8                        ac;                 
  U8                        pr;                 
  U8                        tfi;                
  U8                        d;                  
} T_D_CTRL;


typedef struct /* T_D_HEADER*/
{
  UBYTE                 payload;      /* deceides if the downlink block is a data or control block*/
  T_D_CTRL              d_ctrl;       /* saves the header of a dl rlc control block*/
  UBYTE *               ptr_block;
  UBYTE                 pctrl_ack;    /* value sent with packet control acknowledgement */
} T_D_HEADER;



typedef struct
{
  USHORT l_buf;
  USHORT o_buf;
  UBYTE  buf [BYTE_UL_CTRL_BLOCK];
} T_CS1BUF;


typedef struct
{
  USHORT l_buf;
  USHORT o_buf;
  UBYTE  buf [2 * BYTE_UL_CTRL_BLOCK];
                       /*              CAN's suggestion never more then 2 blocks */
                       /* or 8 blocks? ID's suggestion 'B_SIZE_D_PAGING_REQ\8+1' */

} T_CTRLBUF;



typedef struct /* T_CTRL_BLK */
{
  UBYTE               rbsn;
  UBYTE               rti;
  ULONG               T3200;
  T_CS1BUF            ctrl_blk;
}T_CTRL_BLK;



typedef struct /* T_SEG_CTRL_BLK */   /*saves the first part of segmented control blocks*/
{
  UBYTE               next;
  T_CTRL_BLK          blk[SEG_CTRL_BLOCK_SIZE];
}T_SEG_CTRL_BLK;

typedef struct  /* parameterts of one fixed alloction*/
{
  T_p_fixed_alloc    alloc;
  ULONG              alloc_start_fn;
  ULONG              alloc_end_fn;
  UBYTE              final_alloc;
} T_FIX_ALLOC;


typedef struct  /* fixed alloction control during uplink tbf*/
{
  T_FA_TYPE           fa_type;
  T_FIX_ALLOC         current_alloc;
  BOOL                repeat_alloc;
  UBYTE               ts_overr;
  UBYTE               dl_ctrl_ts;   /* downlink control timeslot */
} T_FA_CONTROL;

typedef struct /* T_COMPLETE_ACQ */
{
  BOOL                needed;       /* complete acquisition needed???*/
  BOOL                psi1_ok;      /* PSI1 complete read?*/
  BOOL                psi2_ok;      /* PSI2 complete read?*/
  BOOL                made_at_least_one_attempt;  /* made at least one attempt? */
}T_COMPLETE_ACQ;

typedef struct /* T_PSI_STATE*/
{
  UBYTE state; /* NEEDED, RECEIPT_OK etc.*/
}T_PSI_STATE;

typedef struct /* T_PSI1_PARAMS */
{
  UBYTE               pbcch_change_mark;    /**/
  UBYTE               psi_change_field;     /**/
  UBYTE               psi_cnt_lr;           /*low repetition rate*/
  UBYTE               psi_cnt_hr;           /*high repetition rate*/
  UBYTE               psi1_repeat_period;   /**/
  BOOL                psi_status_supported; /*Checks whether the network supports PSI STATUS message or not*/
  BOOL                first_psi1;           /*whether the received PSI1 is the first one or not*/
}T_PSI1_PARAMS;

typedef struct /* T_PSI2_PARAMS */
{
  UBYTE               psi2_change_mark;       /**/
  UBYTE               psi2_count;             /**/
  UBYTE               instances[MAX_NR_OF_INSTANCES_OF_PSI2 + 1];           /*used for checking the consistency of PSI2. instances[0]: number of instances*/
}T_PSI2_PARAMS;

typedef struct /* T_PSI3_PARAMS */
{
  UBYTE               psi3_change_mark;       /**/
  UBYTE               psi3_bis_count;             /**/
}T_PSI3_PARAMS;

typedef struct /* T_PSI3bis_PARAMS */
{
  UBYTE               psi3bis_change_mark;       /**/
  UBYTE               psi3bis_count;             /**/
  UBYTE               psi3bis_index;
  UBYTE               instances[MAX_NR_OF_INSTANCES_OF_PSI3BIS + 1];           /*used for checking the consistency of PSI3bis. instances[0]: number of instances*/
}T_PSI3BIS_PARAMS;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
typedef struct /* T_PSI3ter_PARAMS */
{
  UBYTE               psi3ter_change_mark;       /**/
  UBYTE               psi3ter_count;             /**/
  UBYTE               prev_psi3ter_index;
  UBYTE               psi3ter_index;
  UBYTE               instances[MAX_NR_OF_INSTANCES_OF_PSI3TER + 1];           /*used for checking the consistency of PSI3ter. instances[0]: number of instances*/
}T_PSI3TER_PARAMS;
#endif

typedef struct /* T_PSI4_PARAMS */
{
  UBYTE               psi4_change_mark;       /**/
  UBYTE               psi4_count;             /**/
  UBYTE               psi4_index;
  UBYTE               instances[MAX_NR_OF_INSTANCES_OF_PSI4 + 1];           /*used for checking the consistency of PSI4. instances[0]: number of instances*/
}T_PSI4_PARAMS;

typedef struct /* T_EXT_START_STOP */
{
  UBYTE start;
  UBYTE stop;
} T_EXT_START_STOP;

typedef struct /* T_PSI5_PARAMS */
{
  UBYTE               psi5_change_mark;       /**/
  UBYTE               psi5_count;             /**/
  UBYTE               psi5_index;
  UBYTE               instances[MAX_NR_OF_INSTANCES_OF_PSI5 + 1];           /*used for checking the consistency of PSI5. instances[0]: number of instances*/
  T_EXT_START_STOP    idx[MAX_NR_OF_INSTANCES_OF_PSI5]; /* used for sorting the EXT frequency lists */
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  BOOL                v_enh_rep_param_struct; /* Will be set if ENH params
                                                 are present */
#endif
}T_PSI5_PARAMS;

#ifdef REL99
typedef struct /* T_PSI8_PARAMS */
{
  UBYTE               psi8_change_mark;       /**/
  UBYTE               psi8_count;             /**/
  UBYTE               psi8_index;
  UBYTE               instances[MAX_NR_OF_INSTANCES_OF_PSI8 + 1];/*used for checking the consistency of PSI8. instances[0]: number of instances*/
}T_PSI8_PARAMS;
#endif

typedef struct /* T_PSI13_PARAMS */
{
  UBYTE               bcch_change_mark;       /**/
  UBYTE               si13_change_mark;             /**/
}T_PSI13_PARAMS;


typedef struct
{
  UBYTE  Rx;
  UBYTE  Tx;
  UBYTE  Sum;
  UBYTE  Ttb;
  UBYTE  Tra;
} T_MS_CLASS_CAP;

typedef struct
{
  UBYTE  ta_valid;  /* 1: valid ta value present  */
  UBYTE  ta_value;  /* store recent value         */
  UBYTE  ul_ta_i;   /* store uplink ta index	    */
  UBYTE  ul_ta_tn; /* store uplink ta timeslot	  */
  UBYTE  dl_ta_i;    /* store downlink ta index	  */
  UBYTE  dl_ta_tn;  /* store downlink ta timeslot */
  UBYTE  l1_ta_value;     /* ta_value stored in l1      */
  UBYTE  l1_ta_i;         /* ta_index stored in l1      */
  UBYTE  l1_ta_tn;        /* ta_timeslot stored in l1   */
} T_TA_PARAMS;

/* structure for service internal data */

/*
start structs for CS
*/

typedef struct
{
  UBYTE  cnt;                           /* number of measurements          */
  SHORT  acc;                           /* accumulated measurement results */
  USHORT rpt_prd;                       /* reporting period                */
} T_MEAS_RSLT;

typedef struct
{
  BOOL        ovrflw;                   /* indicates whether more than     */
                                        /* CS_MAX_MEAS_RSLT measurement    */
                                        /* results are already collected   */
  UBYTE       loi;                      /* last occupied index             */
  T_MEAS_RSLT meas[CS_MAX_MEAS_RSLT];   /* measurement results             */
} T_RLA_P_DATA;

typedef USHORT T_RXLEV_DATA_NBR;
typedef ULONG  T_RXLEV_DATA_ACC;

typedef struct
{
  T_RXLEV_DATA_NBR nbr;              /* number of accumulated RXLEV values */
  T_RXLEV_DATA_ACC acc;              /* accumulated RXLEV values           */
} T_RXLEV_DATA;

typedef struct 	
{
  USHORT       arfcn;                   /* BCCH carrier of neighbour cell  */
  T_RLA_P_DATA rla_p_data;              /* raw data for deriving the RLA_P */
  T_RXLEV_DATA rxlev_data;              /* raw data for deriving the RXLEV */
                                        /* average for packet measurement  */
                                        /* reporting                       */
} T_CELL;

typedef struct  
{
  UBYTE     c_cell;                     /* number of neigbhour cells       */
  T_CELL    cell[MPHP_NUMC_BA_GPRS_SC]; /* array of neighbour cells        */
} T_NC_DATA;

typedef struct /* T_CTRL_DATA */
{
  UBYTE                       state;
  UBYTE                       last_state; /* last state of CTRL: needed in case of handling circuit switched call*/
  T_RR_STATES                 rr_state;/*the state of the RR part: we may remove this variable and substitude it with another one*/
  PG_CHANNEL                  cs_page_channel; /* on which channel the MS was paged for CS call*/
  CTRL_FOLLOWING_SUSPENSION   after_suspension; /* SZML-GLBL/009 */
  CTRL_PSI_TBF_REL_CAUSE      psi_tbf_rel_cause;
  UBYTE                       imm_rel;
  USHORT                      pcc_arfcn;
  USHORT                      old_arfcn;
  UBYTE                       pcc_bsic;
  UBYTE                       old_bsic;
  UBYTE                       is_susp_needed; /* indicates whether suspension has been needed or not*/
  UBYTE                       new_cell_responded;
  BOOL                        parked_rrgrr_cr_ind;
#ifdef REL99
  UBYTE                       poll_for_msg;   /* Values 0-Invalid, 1-PCCO */
  T_D_CELL_CHAN_ORDER         pcco;
#endif
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_CTRL_DATA;


typedef struct /* T_CTRL_GLBL_DATA */
{
  T_GLBL_PCKT_MODE            state;
  T_CTRL_TASK                 ctrl_task;
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_CTRL_GLBL_DATA;


typedef struct /* T_UL_CTRL_BLOCK */
{
  UBYTE               owner;
  T_BLK_STATE         state;
  UBYTE               data[BYTE_UL_CTRL_BLOCK];
} T_UL_CTRL_BLOCK;

typedef UBYTE T_BLK_INDEX;

typedef struct /* T_UL_CTRL_BLOCK_ARRAY */
{
  T_UL_CTRL_BLOCK     blk[MAX_CTRL_BLK_NUM];
  T_BLK_INDEX         seq[MAX_CTRL_BLK_NUM];
} T_UL_CTRL_BLOCK_ARRAY;

typedef struct /* T_EVAL_ASSIGN_PARAM */
{
  UBYTE               ccch_read;
  T_TBF_TYPE          tbf_type;
  UBYTE               state;
} T_EVAL_ASSIGN_PARAM;

typedef void ( * T_TC_DISABLE_CALLBACK_FUNC )( void );


typedef struct /* T_FREQ_SET */
{
  T_p_frequency_par   freq_par;              /* frequency parameter    */
  UBYTE               tsc;                   /* training sequence code */
} T_FREQ_SET;

typedef struct /* T_TC_DATA */
{
  UBYTE               state;
  T_TC_DISABLE_CALLBACK_FUNC 
                      disable_callback_func;
  UBYTE               disable_class;
  UBYTE               dcch_present;
  UBYTE               ra_once_sent;
  UBYTE               n_acc_req;            /* number of sent packet access request in
                                               current access procedure */
  BOOL                sending_req;          /* sending of packet access requests in process */
  UBYTE               v_sb_without_tbf;
  T_UL_CTRL_BLOCK_ARRAY ul_ctrl_blk;        /* holds all relevant information for */
                                            /* sending uplink control blocks      */
  T_FA_CONTROL        fa_ctrl;              /* handles the fixed allocation parameters during uplink tbf*/
  UBYTE               two_2p_w_4_tbf_con;   /* if true TC waits for TBF_CON after 2 phase access:*/
                                            /* needed to abort single block procedure:default is true */
  UBYTE               num_of_rels_running;  /* number releas confirms that have to be received */
  UBYTE               last_rec_nb_id;       /* assignment id of single block downlink assignment*/
  ULONG               last_rec_nb_fn;       /* fn starting time(receive fn numer) of single block dl assignment*/

  T_EVAL_ASSIGN_PARAM last_eval_assign; 
  T_D_DL_ASSIGN       *p_assign;            /* pointer to dynamically allocated memory */

  T_TBF_TYPE          last_tbf_type;
  BOOL                v_freq_set;           /* frequency params from an assignment are stored   */
  T_FREQ_SET          freq_set;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
  BOOL                tbf_est_pacch;
#endif
  

#ifdef _SIMULATION_
  UBYTE               res_random;
#endif

#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_TC_DATA;

typedef struct /* T_EVAL_IA_DL_PARAM */
{
  UBYTE     state;
  T_EVAL_IA_DL eval_ia_dl;
} T_EVAL_IA_DL_PARAM;

typedef struct /* T_EVAL_IA_UL_PARAM */
{
  T_EVAL_IA eval_ia;
} T_EVAL_IA_UL_PARAM;

typedef union /* T_EVAL_IA_UNION */
{
  T_EVAL_IA_UL_PARAM param_ul;
  T_EVAL_IA_DL_PARAM param_dl;
} T_EVAL_IA_UNION;

typedef struct /* T_CPAP_DATA */
{
  UBYTE                       state;
  T_TBF_TYPE                  new_tbf_type;
  T_D_PDCH_ASS_CMD            pdch_ass_cmd;
  UBYTE                       v_tma_ia;
  T_D_IMM_ASSIGN              tma_ia;        /* for 2 message assignments */
                                             /* parameters from the first */
                                             /* have to be stored until   */
                                             /* 2nd message has arrived   */

  T_IA_TYPE                    last_ia_type;
  T_EVAL_IA_UNION              last_eval_ia;

  T_D_IMM_ASSIGN              *p_d_imm_assign; /* pointer to dynamically allocated memory */

#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_CPAP_DATA;

/*
 * The service PSI holds its state in a variable assigned to one of
 * the serving cell databases (either sc_db_1 or sc_db_2).
 * This is used to simplify the procedure of returning to the old cell
 * in case it is not possible to access the cell choosen for cell
 * re-selection.
 */
typedef struct /* T_PSI_ASSIGNED_DATA */
{
  UBYTE                       state;

#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_PSI_ASSIGNED_DATA;

/*
 * for more information see comment related to structure T_PSI_ASSIGNED_DATA
 */
typedef struct /* T_PSI_DATA */
{
  UBYTE                       last_psi_state;
  UBYTE                       is_start_of_10_sec_allowed; /* Indicates whether the starting of 10 second timer is allowed or not*/
  BOOL                        is_pbcch_req_needed; /*PBCCH req. waiting for perform*/
  BOOL                        is_pbcch_req_allowed; /*PBCCH req. allowed due to transition rules or not*/
  UBYTE                       reading_type;
  BOOL                        v_nc_param;
  UBYTE                       psi2_pos[MAX_NR_OF_INSTANCES_OF_PSI2];

#if defined (REL99) AND defined (TI_PS_FF_EMR)
  UBYTE                       psi3_set_pos[MAX_NR_OF_INSTANCES_OF_PSI3TER+
                                  MAX_NR_OF_INSTANCES_OF_PSI3BIS+
                                          MAX_NR_OF_INSTANCES_OF_PSI3]; 
                                          /* contains PSI3, 3bis and 3ter */
#else
  UBYTE                       psi3bis_pos[MAX_NR_OF_INSTANCES_OF_PSI3BIS+
                                      MAX_NR_OF_INSTANCES_OF_PSI3]; 
                                          /*contains PSI3 and 3bis*/
#endif

  UBYTE                       psi4_pos[MAX_NR_OF_INSTANCES_OF_PSI4];
  UBYTE                       psi5_pos[MAX_NR_OF_INSTANCES_OF_PSI5];
#ifdef REL99
  UBYTE                       psi8_pos[MAX_NR_OF_INSTANCES_OF_PSI8];
#endif
  T_NC_PARAM                  nc_param;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  T_GRR_ENH_PARA              enh_param; /* Temporary storage for enhanced
                                            measurement parameters received 
                                            from PSIs */
#endif

#if defined (TI_PS_FF_RTD) AND defined (REL99)
  T_RTD_VALUE                 rtd[MAX_NR_OF_NCELL];/* RTD values for the neighbour cell synchronisation */
#endif /* #if defined (TI_PS_FF_RTD) AND defined (REL99) */

} T_PSI_DATA;

typedef struct /* T_PG_DATA */
{
  UBYTE                       state;

  /* some parameters needed for RR establishment request*/
  UBYTE                       ch_needed;    /* channel needed field*/
  UBYTE                       v_emlpp_prio; /* valid flag for emlpp_priority*/
  UBYTE                       emlpp_prio;   /* value of eMLPP_PRIORITY*/
  UBYTE                       type;         /* type of paging for rr est: IMSI, TMSI*/
  USHORT                      initial_dsc;
  USHORT                      dsc;
  UBYTE                       is_l1_ref_tn_changed;
  UBYTE                       nmo;          /* network mode of operation for paging */
  T_PACKET_MODE               packet_mode;
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_PG_DATA;

typedef USHORT T_C_INDEX;
typedef USHORT T_C_VALUE;

typedef struct /* T_C_FILTER */
{
  T_C_VALUE value;                     /* current C value */
  T_C_INDEX index;                     /* iteration index */
} T_C_FILTER;

typedef struct /* T_MEAS_DATA */
{
  UBYTE                       state;

  UBYTE                       pwr_offset;       /* POWER OFFSET parameter used */
                                                /* by a DCS 1800 Class 3 MS    */
  T_C_FILTER                  c_filter;         /* filtered C value   */

#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_MEAS_DATA;

typedef USHORT T_IM_GAMMA;

typedef struct /* T_MEAS_IM_GAMMA */
{
  UBYTE      index;          /* iteration index                    */
  UBYTE      m_gamma;        /* multi-flag                         */
  T_IM_GAMMA gamma;          /* filtered interference level values */
  LONG       remain_time;    /* remaining time to run until        */
                             /* timer T_IM_SYNC expires            */
} T_MEAS_IM_GAMMA;

typedef UBYTE T_MEAS_IM_HOP_ARFCN_BIT_FIELD[MAX_IM_HOP_ARFCN_BIT_FIELD_SIZE];  

typedef struct /* T_MEAS_IM_HOP_PARAM */
{
  UBYTE                         hsn;
  UBYTE                         maio;
  T_MEAS_IM_HOP_ARFCN_BIT_FIELD arfcn_bit_field;
} T_MEAS_IM_HOP_PARAM;

typedef union /* T_MEAS_IM_ALLOC */
{
  USHORT              arfcn;
  T_MEAS_IM_HOP_PARAM hop_param;
} T_MEAS_IM_ALLOC;

typedef struct /* T_MEAS_IM_CARRIER */
{
  BOOL            hopping;
  T_MEAS_IM_ALLOC alloc;
} T_MEAS_IM_CARRIER;

typedef struct /* T_MEAS_IM_FILTER */
{
  UBYTE               used;    /* indicates whether the filter is used       */
  T_MEAS_IM_GAMMA     i_level [CGRLC_MAX_TIMESLOTS];
                               /* interference level for each timeslot       */
  T_MEAS_IM_CARRIER   carrier; /* carrier used for measurement               */
} T_MEAS_IM_FILTER;

typedef struct /* T_MEAS_IM_CHANNEL */
{
  UBYTE index;                 /* actual channel that is measured            */
  UBYTE ident[MAX_IM_IDLE_CHANNELS];
                               /* identifies the channel to be measured      */
} T_MEAS_IM_IDLE_CHN;

typedef struct /* T_MEAS_IM_DATA */
{
  UBYTE                       state;       /* sub-state of the interference  */
                                           /* measurement service            */
  T_MEAS_IM_MODE              mode;        /* mode of operation              */
  T_MEAS_IM_IDLE_CHN          idle_chan;   /* channels measured in packet    */
                                           /* idle mode                      */
  T_MEAS_IM_FILTER            filter [MAX_IM_CHANNELS];
                                           /* running average filter         */
  USHORT                      carrier_id;  /* carrier identifier             */
  T_MEAS_IM_CARRIER           carrier;     /* carrier currently measured     */
  T_MEAS_IM_TRIG_SIGNAL       trig_signal; /* trigger signal                 */
  BOOL                        v_cs_meas_active;
                                           /* indicates whether cell re-     */
                                           /* selection measurements are     */
                                           /* active                         */
  BOOL                        v_ilev_abs;  /* indicates whether at least one */
                                           /* CGRLC_INT_LEVEL_REQ primitive  */
                                           /* was sent since last reset      */
  T_ilev_abs                  ilev_abs;    /* stores the content of last     */
                                           /* sent CGRLC_INT_LEVEL_REQ       */
                                           /* primitive                      */

#if !defined (NTRACE)

  UBYTE                       n_im_trace;

#endif /* #if !defined (NTRACE) */

#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_MEAS_IM_DATA;

typedef struct /* T_MEAS_EM_RSLT */
{
  UBYTE           refidx;
  UBYTE           rxlev;
  UBYTE           bsic;
} T_MEAS_EM_RSLT;


typedef struct /* T_MEAS_EM_RSLT_LIST */
{
  UBYTE           number;
  T_MEAS_EM_RSLT  elem[RRGRR_MAX_ARFCN_EXT_MEAS];
} T_MEAS_EM_RSLT_LIST;


typedef struct /* T_MEAS_EM_STRG_LIST */
{
  UBYTE           number;
  UBYTE           refidx[RRGRR_MAX_ARFCN_EXT_MEAS];
} T_MEAS_EM_STRG_LIST;

typedef struct /* T_MEAS_PAR_PMO */
{
  UBYTE              count;
  UBYTE              instances[MAX_NR_OF_INSTANCES_OF_PMO];
  T_EXT_START_STOP   idx[MAX_NR_OF_INSTANCES_OF_PMO];
} T_MEAS_PAR_PMO;

typedef struct /* T_MEAS_MSG_PMO */
{
  T_NCMEAS_EXTENDED  nc;
  T_XMEAS            extd;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  BOOL               v_enh_meas_param_pmo; /* Set if ENH params are present */
  T_GRR_ENH_PARA     enh; /* Temporary storage for enhanced measurement 
                             parameters received from PMO*/
#endif
  T_MEAS_PAR_PMO     prm;
} T_MEAS_MSG_PMO;

typedef struct /* T_XMEAS_SET */
{
  UBYTE              ident; /* identifier of valid EXT measurement parameter set */
  T_XMEAS           *param; /* pointer to valid EXT measurement parameter set    */
} T_XMEAS_SET;

typedef struct /* T_MEAS_EM_DATA */
{
  UBYTE                       state;
  T_XMEAS_SET                 xmeas_set;
  BOOL                        v_pmo_nc_ncmeas_param;
  BOOL                        v_pmo_nc_ncmeas_list_rfreq;
  T_MEAS_MSG_PMO              pmo;     /* used to store message packet measurement */
                                       /* order temporarily                        */
  T_MEAS_EM_RSLT_LIST         rslt;    /* EXT measurement results for all carriers */
  T_MEAS_EM_STRG_LIST         strg;    /* EXT measurement results for 6 strongest  */
                                       /* carriers ( with/without BSIC decoding )  */
  UBYTE                       pmr_snd_ref;
                                       /* reference to the next EXT measurement    */
                                       /* result which chould be sent to the       */
                                       /* network                                  */
#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_MEAS_EM_DATA;

typedef struct /* T_GRR_CS_DATA */
{
  UBYTE                       state;
  T_GMM_STATES                gmm_state;
  T_TIME                      last_cr;
  T_CS_RESELECT_CAUSE         reselect_cause;
  UBYTE                       is_upd_ncell_stat_needed;
  UBYTE                       is_mval_initialized;
  UBYTE                       list_id;
  UBYTE                       stop_req;
  UBYTE                       cr_meas_mode; /* cell re-selection measurement mode */
  BOOL                        cr_meas_update;
  UBYTE                       last_assignment_id;
  USHORT                      nc_sync_rep_pd;

#if !defined (NTRACE)

  UBYTE                       v_crp_trace;

#endif /* #if !defined (NTRACE) */

#ifndef NTRACE
  char                        *name;
  char                        *state_name;
#endif
} T_GRR_CS_DATA;

typedef struct /* T_CS_MEAS_DATA */
{
  UBYTE          state;
  UBYTE          pmr_snd_ref;  /* reference to the next NC measurement result */
                               /* which chould be sent to the network         */
  UBYTE          nc_meas_cause;/*Indicates NC meas status in ALR when NC=1 or NC=2 */
  T_PACKET_MODE  packet_mode;
#ifndef NTRACE
  char          *name;
  char          *state_name;
#endif
} T_CS_MEAS_DATA;



typedef struct 
{                                 
  U8  nc2_on; /*
               * Indicates whether it is allowed to use NC1 and NC2:
               *
               * TRUE  = Use of NC1 and NC2 is allowed.
               * FALSE = Use of NC1 and NC2 is prohibited.
               */
} T_GRR_FFS;




typedef struct /* T_GFP_DATA */
{
  UBYTE                   state;
  BOOL                    tfi_check_needed; /* if the tfi in optional header is correct, 
                                               the ms shall ignore the tfi within the air message.
                                               This parameter indicates if a check for the tfi is needed(TRUE)
                                               or not(FALSE)*/

#ifndef NTRACE
  char                    *name;
  char                    *state_name;
#endif
} T_GFP_DATA;

typedef struct
{
  BOOL                        v_gamma;
  UBYTE                       gamma;
} T_GAMMA_TN;

typedef struct
{
  BOOL                        v_alpha;
  UBYTE                       alpha;
  T_GAMMA_TN                  gamma_tn[CGRLC_MAX_TIMESLOTS];
} T_PWR_PAR;

typedef struct /* T_CELL_INFO_FOR_GMM */
{
  T_cell_info         cell_info;     /* Cell information */
  T_GPRS_SERVICE      gprs_service;  /* GPRS service     */
  T_ACCESS_STATUS     access_status; /* Access status    */
} T_CELL_INFO_FOR_GMM;

typedef struct /* T_SC_DATABASE */
{
  T_PSI_ASSIGNED_DATA         psi_assigned_data;
  T_PSI1_PARAMS               psi1_params;        /**/
  T_PSI2_PARAMS               psi2_params;        /**/
  T_PSI3_PARAMS               psi3_params;        /**/
  T_PSI3BIS_PARAMS            psi3bis_params;     /**/
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  T_PSI3TER_PARAMS            psi3ter_params;        /**/
#endif
  T_PSI4_PARAMS               psi4_params;        /**/
  T_PSI5_PARAMS               psi5_params;        /**/
#ifdef REL99
  T_PSI8_PARAMS               psi8_params;         /**/
#endif
  T_PSI13_PARAMS              psi13_params;       /**/
  UBYTE                       is_access_enabled;  /* This common parameter is used by services CTRL and PSI*/
  T_COMPLETE_ACQ              complete_acq;       /**/
  ACQ_TYPE                    acq_type;             /*indicates whether the receiving of consistent set of PSI is in progress*/

  BOOL                        measurement_order;  /*FALSE: NC0, EM0 no PSI5; TRUE: PSI5 on PBCCH measurement needed*/
  BOOL                        send_psi_status;    /*Checks whether the PSI service should send PSI STATUS message or not*/

  T_PSI_STATE                 state_of_PSI[MAX_PSI];  /* */

  UBYTE                       si_to_acquire;      /*  UNSPECIFIED_SI,  UPDATE_SI1, UPDATE_SI2_SI2BIS_OR_SI2TER,
                                                      UPDATE_SI3_SI4_SI7_OR_SI8, UPDATE_SI9, UPDATE_SI13*/
  T_MPHP_SCELL_PBCCH_REQ      scell_pbcch; /* stores the primitive, and  pass it after 
                                              transition rules to layer1 */

  T_PBCCH             pbcch;            /*  PBCCH description */

  T_NETWORK_CONTROL   net_ctrl;         /*  Network control parameters*/

  BOOL                v_gprs_cell_opt;  /*  valid flag for GPRS Cell Options*/
  T_gprs_cell_opt     gprs_cell_opt;    /*  GPRS Cell Options  */

  BOOL                v_prach;          /*  valid flag for PRACH Control Parameters*/
  T_prach_ctrl_par    prach;            /*  PRACH Control Parameters  */

  BOOL                v_g_pwr_par;      /*  valid flag for Global Power Control Parameters*/
  T_g_pwr_par         g_pwr_par;        /*  Global Power Control Parameters */

  BOOL                v_pwr_par;        /*  valid flag for Power Control Parameters*/
  T_PWR_PAR           pwr_par;          /*  Power Control Parameters */

  T_pccch_org_par     pccch;            /*  PCCCH Organization Parameters  */

  T_PAGING_GROUP      paging_group;     /*  This structure contains parameters to initialize paging (static)*/

  UBYTE               last_pg_mode;     /*  last page mode*/
  UBYTE               network_pg_mode;
  BOOL                gprs_attach_is_running;     /*  indicates if the gprs procedure is running or not*/
  UBYTE               non_drx_timer;    /* This value contains the values in seconds*/
  BOOL                non_drx_timer_running;
  BOOL                nc2_non_drx_period_running;

  T_RFL_LST           rfl[MAX_RFL+1];     /*  Reference Frequency Lists      */

  BOOL                v_cell_alloc;           /*  valid flag for Cell Allocation */
  T_cell_alloc        cell_alloc[MAX_CELL_ALLOC];   /*  Cell Allocation  */

  T_gprs_ms_alloc     gprs_ms_alloc_in_assignment;    /*  GPRS Mobile Allocations received in an assignment message*/

  T_gprs_ms_alloc     gprs_ms_alloc_in_psi2_psi13[MAX_GPRS_MS_ALLOC];   /*  GPRS Mobile Allocations received in PSI2 and/or PSI13*/

#ifdef REL99
  BOOL                v_add_psi;/* valid-flag */
  T_add_psi           add_psi;  /* additional PSI Messages broad cast information*/
#endif

  BOOL                v_non_gprs_opt;   /*  valid flag for Non GPRS Cell Options */
  T_non_gprs_opt      non_gprs_opt;     /*  Non GPRS Cell Options */

  T_SCELL_PAR         scell_par;        /*  Serving Cell parameters from PSI 3 and PSI3 BIS */

  T_gen_cell_par      gen_cell_par;     /*  General Cell Selection parameter */

  UBYTE               number_of_valid_int_meas_channels;
  T_int_meas_chan_lst int_meas_chan_list[MAX_CHAN_IMEAS];  /* Channel List for Interference measurements (PSI 4) */

  BOOL                is_ext_psi5_valid; /* Is the extended structure in PSI5 is valid*/
  T_XMEAS             ext_psi5;          /* EXT measurement parameters derived from PSI5 */

  T_XMEAS             ext_pmo;           /* EXT measurement parameters derived from packet measurement order */

  T_NCMEAS            nc_cw;             /* NC measurement parameters cell wide */

  T_NCMEAS_EXTENDED   nc_ms;             /* NC measurement parameters MS specific */

  T_CELL_INFO_FOR_GMM cell_info_for_gmm; /* Cell information composed for GMM */
#ifdef REL99
#ifdef TI_PS_FF_QUAD_BAND_SUPPORT
  UBYTE               band_indicator;    /* Band indicator received in PSI 1 */
#endif
  UBYTE               sgsn_rel;          /* SGSN release of the serving cell */
  UBYTE               network_rel;        /*Network release*/
  BOOL                v_cbch_chan_desc;  /* valid fla for CBCH channel description. */
  T_cbch_chan_desc    cbch_chan_desc;    /* Stores CBCH channel description as received in PSI8 */   
  BOOL                send_cbch_info_ind;/* This flag indicates if a CBCH info update has to be sent to RR */
#ifdef TI_PS_FF_EMR
  UBYTE               ba_ind;            /* BA-BCCH sequence number */
  T_BA_BCCH_NC_LIST   nc_ba_bcch_cw;     /* GSM NC list formed in RR from BA-BCCH and BSIC */
  T_GRR_ENH_PARA      enh_cw;            /* ENH measurement parameters cell wide */
  T_GRR_ENH_PARA      enh_ms;            /* ENH measurement param */
  BOOL                si_status_ind;     /* The network supports/does not support
                                            the PACKET SI STATUS message */
#endif
#endif

#if defined (TI_PS_FF_RTD) AND defined (REL99)
  T_RTD_VALUE         rtd[MAX_NR_OF_NCELL];/* RTD values for the neighbour cell synchronisation */
#endif /* #if defined (TI_PS_FF_RTD) AND defined (REL99) */


} T_SC_DATABASE;

typedef struct /* T_NC_NW_SELECTED */
{
  T_NC_SIX_STRGST     strgst;           /* this cell is used for network controlled */
                                        /* cell re-selection but does not belong to */
                                        /* the six strongest cells                  */

} T_NC_NW_SELECTED;

typedef struct /* T_GPRS_DATABASE */
{
  T_MS_IDENT          ms_id;            /* some parameters to identify the MS: IMSI, PTMSI, TMSI */

  T_scell_info        scell_info;       /* contains measurement values for serving cell */

  T_SCELL             old_scell;        /* previous serving cell */

  T_NC_REF_LIST       nc_ref_lst;       /* list of pointers to the neighbour cell lists */

  T_NC_MVAL_LIST      nc_mval_list;     /* measured values of the neighbour cells */


  T_NC_SIX_STRGST    *cr_cell;

  T_CNT_NC_SIX_STRGST cnt_nc_six;
  T_NC_SIX_STRGST     nc_six_strgst[CS_MAX_STRONG_CARRIER];

  T_NC_NW_SELECTED    nc_nw_slctd;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
  UBYTE               cnt_enh_cell_list; /* Number of Cells in the ENH cell 
                                            list */
  
  T_ENH_CELL_LIST     enh_cell_list[MAX_NR_OF_NCELL];/* Enhanced Measurement 
                                                        Cell list */
  
  T_ENH_BIN           sorted_enh_cell_list; /* ENH cell list sorted in 
                                               descending order of rxlev */
  T_PEMR_PARAMS       pemr_params;          /* PEMR parameters used during 
                                               reporting */
  T_BA_BCCH_NC_REF_LIST   ba_bcch_nc_ref_lst;   /* list of pointers to the neighbour cell lists */
#endif

} T_GPRS_DATABASE;

typedef struct /* T_PWR_CTRL_VALID_FLAGS */
{
  BOOL v_pwr_ctrl_param;
  BOOL v_glbl_pwr_ctrl_param;
  BOOL v_freq_param;
  BOOL v_c_value;
} T_PWR_CTRL_VALID_FLAGS;

typedef struct /* T_GRR_DATA */
{
  UBYTE                   sc_db_mode;

  T_SC_DATABASE           sc_db_1;      /* database 1 for serving cell: it contains all cell relevant
                                         * information, needed for to camp on a network, access to
                                         * the network, etc.
                                         */

  T_SC_DATABASE           sc_db_2;      /* database 2 for serving cell: it contains all cell relevant
                                         * information, needed for to camp on a network, access to
                                         * the network, etc.
                                         */

  T_GPRS_DATABASE         db;           /* db is the shortname for gprs_database  */

  T_MS_DATA               ms;           /* ms specific data */

  T_UL_TBF                uplink_tbf;   /*  This structure is used by TC,CPAP
                                            during uplink-tbf */
  T_DL_TBF                downlink_tbf; /*  This structure is used by TC,CPAP
                                            during downlink-tbf */
  T_TBF_TYPE              tbf_type;

  ULONG                   ul_fn;
  ULONG                   dl_fn;

  ULONG                   l1_del_tbf_start_fn; /* When there is poll requested in a re-assignment message
                                                * sending of poll response successfully requires this
                                                * value to be sent to poll_fn+8. This is important when
                                                * the timeslot configuration changes between re-assignment.
                                                */

  T_TA_PARAMS             ta_params;
  UBYTE                   pdch_rel_ts_mask;
  USHORT                  tqi;
  T_req_ref_p             req_ref[3];   /* to save the last 3 packet request_references */
  UBYTE                   r_bit;        /* current r-bit value - set by TC - read by RU/RD*/
  T_SEG_CTRL_BLK          seg_ctrl_blk;
  T_CTRLBUF               ctrl_msg;
  T_MS_CLASS_CAP          ms_cap[12];
  /*
   * Some global parameters to control the whole functionality
   */
  BOOL                     is_pg_started; /* Checks whether paging procedure has been started or not*/
  BOOL                     cc_running;   /* Assert if Cell change Order is running or not? */
  UBYTE                    pcco_failure_cause;
  UBYTE                    test_mode;     /* indicates the testmode status */
  BOOL                     cell_res_status; /* indicates the status of GMMRR_CELL_RES primitive */

  T_NC_DATA               nc_data;  

  T_PWR_CTRL_VALID_FLAGS  pwr_ctrl_valid_flags;

  /*
   * Service data
   */
  T_GFP_DATA               gfp;
  T_CTRL_DATA              ctrl;
  T_CTRL_GLBL_DATA         ctrl_glbl;
  T_TC_DATA                tc;
  T_CPAP_DATA              cpap;
  T_PSI_DATA               psi;
  T_PG_DATA                pg;
  T_MEAS_DATA              meas;
  T_MEAS_IM_DATA           meas_im;
  T_MEAS_EM_DATA           meas_em;
  T_GRR_CS_DATA            cs;
  T_CS_MEAS_DATA           cs_meas;
  UBYTE                    nc2_on;    
 

}T_GRR_DATA;








/*==== EXPORT =====================================================*/

/*
 * data base
 */
#ifdef GRR_PEI_C
       T_GRR_DATA grr_data_base, *grr_data;
#else
EXTERN T_GRR_DATA grr_data_base, *grr_data;
#endif

/*
 * for more information see comment related to structure T_PSI_ASSIGNED_DATA
 */
#if defined GRR_PSIF_C OR defined GRR_PSIP_C OR defined GRR_PSIS_C
  #define ENTITY_DATA psc_db 
#else
  #define ENTITY_DATA grr_data 
#endif


/*
 * Communication handles (see also GRR_PEI.C)
 */
#define hCommL1             ppc_hCommMAC    
#define hCommGMM            grr_hCommGMM
#define hCommGRR            grr_hCommGRR
#define hCommGRLC           grr_hCommGRLC
#define hCommRR             grr_hCommRR
#define hCommPL             grr_hCommPL
#ifdef FF_WAP /*FMM*/
  #define hCommWAP          grr_hCommWap  /*In future an own FMM Entity is planned*/
#endif


#define _decodedMsg   grr__decodedMsg


#ifdef GRR_PEI_C
       T_HANDLE hCommL1         = VSI_ERROR;
       T_HANDLE hCommGRLC       = VSI_ERROR;
       T_HANDLE hCommGMM        = VSI_ERROR;
       T_HANDLE hCommGRR        = VSI_ERROR;
       T_HANDLE hCommRR         = VSI_ERROR;
       T_HANDLE hCommPL         = VSI_ERROR;
#ifdef FF_WAP
       T_HANDLE hCommWAP         = VSI_ERROR;
#endif
       T_HANDLE GRR_handle;



GLOBAL UBYTE          _decodedMsg [MAX_MSTRUCT_LEN_GRR];

/*
 * make the pei_create function unique
 */
#define pei_create              grr_pei_create

/*
 * make the pei_func_primitive function unique
 */
#define pei_func_primitive      grr_pei_func_primitive

#else  /* !GRR_PEI_C */
EXTERN T_HANDLE hCommL1;
EXTERN T_HANDLE hCommGRLC;
EXTERN T_HANDLE hCommGMM;
EXTERN T_HANDLE hCommGRR;
EXTERN T_HANDLE hCommLLC;
EXTERN T_HANDLE hCommRR;
EXTERN T_HANDLE hCommPL;
#ifdef FF_WAP
 EXTERN T_HANDLE hCommWAP;
#endif
EXTERN T_HANDLE GRR_handle;

#endif /* GRR_PEI_C */


EXTERN UBYTE          _decodedMsg [MAX_MSTRUCT_LEN_GRR];

#ifdef GRR_PEI_C
T_SC_DATABASE*          psc_db;       /* points to the current serving cell database  */
T_SC_DATABASE*          posc_db;      /* points to the old serving cell database      */
T_SC_DATABASE*          prsc_db;      /* points to the rejected serving cell database */
T_SC_DATABASE*          pcr_db;       /* points to the cell re-selection database     */
#else
EXTERN T_SC_DATABASE*   psc_db;
EXTERN T_SC_DATABASE*   posc_db;
EXTERN T_SC_DATABASE*   prsc_db;
EXTERN T_SC_DATABASE*   pcr_db;
#endif /* GRR_PEI_C */


#ifdef _TARGET_

#undef TRACE_FUNCTION
#define TRACE_FUNCTION(a)
#undef TRACE_ISIG
#define TRACE_ISIG(a)

#endif /* _TARGET_ */

#endif /* GRR_H */
