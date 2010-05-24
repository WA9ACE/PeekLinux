#ifndef _PSWCUSTOM_H_
#define _PSWCUSTOM_H_


#define IS2000_REV_0 1 /* Activate all IS2000 Rev.0 SW */

/* VIVO Network in S. America has custom system selection requirements 
 * that differ from standard.  VIVO requires the MS remain on a redirected
 * system until a call is released.
 */
#if 0
#define VIVO_NETWORK
#endif

#define BAND_CLASS_4_SUPPORTED 
#define BAND_CLASS_6_SUPPORTED
#define BAND_CLASS_7_SUPPORTED
#define BAND_CLASS_8_SUPPORTED
#define BAND_CLASS_9_SUPPORTED
#define BAND_CLASS_10_SUPPORTED

/* this flag enables carrier specific extensions to
 * IS-95A to specify Rate set 1 or 2 for data calls
 * using one of the reserved ASSIGN Modes.
 */
#define SKT_EXTENSIONS 1

/*****************************************************************************
 * ------- Tune parameters - adjust as required                              *
 ****************************************************************************/

/* Set the product supported operating mode by OR'ing supported modes */
  /*#define ENG_CUSTOM_OP_MODE       OP_MODE0 | OP_MODE1*/
          /* above example sets PCS CDMA and Cellular CDMA */

/* !!!!< ---------  added by LSIL ---------- */

#ifdef LSI_PILOT_THRESH_VER

#define T_ADD_REL_THRESH_DB_Q1    0     /* adaptive T_ADD threshold applied relative to summation
                                          * of all active powers, in units of 0.5dB. E.g. -9 gives 
                                          * a minimum effective T_ADD which is 4.5dB below the total
                                          * Active power (sum of all actives).
                                          * Must be <=0.
                                          */
#define T_DROP_REL_THRESH_DB_Q1   0     /* adaptive T_DROP threshold applied relative to summation
                                          * of all active powers, in units of 0.5dB. E.g. -9 gives 
                                          * a minimum effective T_DROP which is 4.5dB below the total
                                          * Active power (sum of all actives). Note: since there is a 
                                          * drop timer, it is not necessarily contradictory for this
                                          * value to be the same as the relative T_ADD, but it cannot
                                          * be greater than the T_ADD.
                                          * Must be <=0.
                                          */

/* 1165 in Q16 = -17.5 dB */
#define T_ADD_MIN_THRESH_Q16     1165     /* minimum power for the adaptive T_ADD threshold, units of Ec/Io Q16 */
/* 655 in Q16 = -20 dB */
#define T_DROP_MIN_THRESH_Q16    655      /* minimum power for the adaptive T_DROP threshold, units of Ec/Io Q16 */
#endif


/* >!! -------------------------------------------- */


#if defined(SYS_OPTION_AMPS)
/* Support for PCS and Cellular CDMA and Analog  */
#define ENG_CUSTOM_OP_MODE       ( OP_MODE0 + OP_MODE1 + OP_MODE2 )
#else
/* Support for PCS and Cellular CDMA   */
#define ENG_CUSTOM_OP_MODE       ( OP_MODE0 + OP_MODE1 )
#endif

/* Min # of frames chipset must rx the action in advance of the action time */
#define CC_MIN_FRAMES_BEFORE_ACTION_TIME    4
#define TC_MIN_FRAMES_BEFORE_ACTION_TIME    2

/**********************************************************************/
/******************* CUSTOM SYSTEM SELECTION **************************/
/**********************************************************************/

/* ============================================================ */
/* =================== OUT OF SERVICE AREA ==================== */
/* ============================================================ */

#define MAX_OOSA_PHASES         4
#define OOSA_TA_TIME       120000L /* 2 minutes in msec */


/* Out of Service Area Selection Parameters */
#define CP_SELECTION_TIME1_SCALE          10   /* Units of 100 msec */
#define CP_SELECTION_TIME2_SCALE          10   /* Units of 100 msec */
#define CP_SELECTION_TIME3_SCALE          10   /* Units of 100 msec */
#define CP_SELECTION_TIME4_SCALE          10   /* Units of 100 msec */

#ifndef KDDI_EXTENSIONS
#define CP_SELECTION_PHASE1_ATTEMPTS       5   /* phase 1 attempts */
#define CP_SELECTION_PHASE2_ATTEMPTS       5   /* phase 2 attempts */
#define CP_SELECTION_PHASE3_ATTEMPTS      25   /* phase 3 attempts */
#define CP_SELECTION_PHASE4_ATTEMPTS      25   /* phase 4 attempts */

#define CP_SELECTION_PHASE1_DELAY          0   /* delay between phase 1 attempts */
#define CP_SELECTION_PHASE2_DELAY          0   /* delay between phase 2 attempts */
#define CP_SELECTION_PHASE3_DELAY         10   /* delay between phase 3 attempts */
#define CP_SELECTION_PHASE4_DELAY         30   /* delay between phase 4 attempts */

#define CP_SELECTION_PHASE1_CYCLE        100   /* # of phase 1 tries which */
                                               /* includes attempts and delay */
#define CP_SELECTION_PHASE2_CYCLE        100   /* # of phase 2 tries which */
                                               /* includes attempts and delay */
#define CP_SELECTION_PHASE3_CYCLE         40   /* # of phase 3 tries which */
                                               /* includes attempts and delay */
#else
#define CP_SELECTION_PHASE1_ATTEMPTS       2   /* phase 1 attempts */
#define CP_SELECTION_PHASE2_ATTEMPTS       2   /* phase 2 attempts */
#define CP_SELECTION_PHASE3_ATTEMPTS       2   /* phase 3 attempts */
#define CP_SELECTION_PHASE4_ATTEMPTS       2   /* phase 4 attempts */

#define CP_SELECTION_PHASE1_DELAY          0   /* delay between phase 1 attempts */
#define CP_SELECTION_PHASE2_DELAY          5   /* delay between phase 2 attempts */
#define CP_SELECTION_PHASE3_DELAY         20   /* delay between phase 3 attempts */
#define CP_SELECTION_PHASE4_DELAY         20   /* delay between phase 4 attempts */

#define CP_SELECTION_PHASE1_CYCLE         10   /* # of phase 1 tries which */
                                               /* includes attempts and delay */
#define CP_SELECTION_PHASE2_CYCLE         24   /* # of phase 2 tries which */
                                               /* includes attempts and delay */
#define CP_SELECTION_PHASE3_CYCLE        255   /* # of phase 3 tries which */
                                               /* includes attempts and delay */
#define SIMULATE_QPCH
#define ALTERNATE_PN
#endif

#if (!((CP_SELECTION_PHASE1_ATTEMPTS  > 0) &&  \
       (CP_SELECTION_PHASE2_ATTEMPTS  > 0) &&  \
       (CP_SELECTION_PHASE3_ATTEMPTS  > 0) &&  \
       (CP_SELECTION_PHASE4_ATTEMPTS  > 0) && \
       (CP_SELECTION_PHASE1_CYCLE  > 0) && \
       (CP_SELECTION_PHASE2_CYCLE  > 0) && \
       (CP_SELECTION_PHASE3_CYCLE  > 0)))
#error "Bad Out of Service Area Selection Parameters!"
#endif

/* ============================================================ */
/* =========================== SSPR =========================== */
/* ============================================================ */

/* Amala K. 12/31/01 - New System Selection */
#define PRL_MAX_SIZE      6144

/* Amala K. */
/* TEMPORARY UNTIL DBM CHANGES MADE FOR MAX_MRU_RECORDS=10 */
#define MAX_MRU_RECORDS      10

/* Number of positive and negative sids in the NAM. */
#define MAX_POSITIVE_SIDS  20
#define MAX_NEGATIVE_SIDS  10

/* Roaming types to be used for registration in register.c */
typedef enum
{
   CP_REG_NOT_ROAMING,
   CP_REG_NID_ROAMING,
   CP_REG_SID_ROAMING,
   CP_REG_UNUSED_ROAMING
} RegRoamStatus;

/* The #defines RI_HOME, RI_NIDRM, RI_SIDRM and RI_UNURM are based on RegRoamStatus.
   If any changes are made to that enum, then changes have to be made here too. 
   This has only been done to make ROAM_IND_LIST definition smaller in size. */
#define RI_HOME                 CP_REG_NOT_ROAMING
#define RI_NIDRM                CP_REG_NID_ROAMING
#define RI_SIDRM                CP_REG_SID_ROAMING
#define RI_UNURM                CP_REG_UNUSED_ROAMING

/* Total number of roaming indicators allowed */
#define NO_OF_ROAM_INDS         256

/* The numerical value of the roam indicator is to be used to index into this table. If a
   roaming indicator's definition whether it is a Home System, Sid Roaming or Nid Roaming is to
   be changed, then the change has to be made in this table. */
#define ROAM_IND_LIST  { \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*09*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*19*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*29*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*39*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*49*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*59*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*69*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*79*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*89*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*99*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*109*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*119*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*129*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*139*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*149*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*159*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*169*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*179*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*189*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*199*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*209*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*219*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*229*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*239*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME,/*249*/ \
 RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME, RI_HOME  /*255*/ }

/* Amala K. 05/02/02 - corrected CSS re-scan timer timeout values */
#define HANG_ON_TIME          30000L  /* 30  seconds */
#define RESCAN_PCS_TIME       90000L  /* 90  seconds */
#define RESCAN_CELLULAR_TIME 150000L  /* 150 seconds */

/* ============================================================ */
/* ====================== EMERGENCY MODE ====================== */
/* ============================================================ */

/* 5 minutes is Verizon requirement */
#define EMERGENCY_CALLBACK_MODE_TIMEOUT   300000L /* 300 seconds */

/* Better service rescan after redirection in 5 minutes */
#define REDIRECTION_END_TIME    300000L   /* 300 seconds */

#define CSS_EPRL_ESPM_WAIT_TIME 12000L  /* Maximum time to wait for ESPM to validate
                                         System Selection */

/**********************************************************************/
/***************** END CUSTOM SYSTEM SELECTION ************************/
/**********************************************************************/

/* PCH Layer 2 */
/* Increased to 3 seconds to fix CR301 */
#define ENG_MAX_BAD_PAGE_FRAME  150 /* 150 * 20 msec = 3 sec */

/* this sets number of base stations for which
 *     overhead info is stored
 */
#define MAX_HISTORY_RECS        7

/* new Idle Handoff Parameters */
/*AKK 05-30-02 per CR#561, replaced Idle Handoff Thresholds with that of CBP3*/
#define THR_SLOPE_RES       2  /* Q-precision resolution of specified line slopes.
                                  (2^-THR_SLOPE_RES)*(SPEC integer slope in dB(thresh)
                                  /dB(Active pwr)). */
#define THR_SLOPE_MID       2  /* (2^THR_SLOPE_RES)/2 used for slope rounding. */

#define MIN_ACT_IMMED      34  /* -17.0 dB active power below the "instant" test 
                                  threshold is THR_MIN_IMMED. */

#define THR_SLOPE_IMMED     1  /* Slope of instant threshold above active power 
                                  MIN_ACT_IMMED in units of dB(thresh)/dB(Active pwr)/
                                  2^THR_SLOPE_RES. */
#define THR_MIN_IMMED       4  /* Minimum value of the "instant" test threshold. 
                                  Units -0.5dB. */
#define MIN_ACT_DELAYED    34  /* Minimum value of the "persistance" test threshold.
                                  Units -0.5dB. */
#define THR_SLOPE_DELAYED   1  /* Active power below which the "persistance" test 
                                  threshold is THR_MIN_DELAYED. */
#define THR_MIN_DELAYED     1  /* Slope of the instant threshold above active power of
                                  MIN_ACT_DELAYED, in units of dB(thresh)/dB(active pwr)
                                  /2^THR_SLOPE_RES. */
#define NOISE_THRESHOLD    50  /* Threshold below which no neighbor pilot will be
                                  considered a valid target for handoff, units -0.5dB */
#define IHO_HTMR          500  /* 1/2 second Persistance timer. Persistance test is passed
                                  if the power is greater than the persistance threshold
                                  over this period. Units ms.*/
#define MIN_NGHBR_THR      36  /* Idle Handoff Candidates must be atleast -18.0 dB . */
/*END--AKK 05-30-02 per CR#561, replaced Idle Handoff Thresholds with that of CBP3*/

/* Access Entry HO Parameters */
#define CP_ACCESS_ENTRY_HO_WEAK_PILOT_STRENGTH  30  /* -15 dB */
#define ACCESS_ENTRY_HANDOFF_ALLOWED  1


/* Max # of Access Slots to delay due to persistence before transmit msg */
#define ENG_MAX_PSIST_ACCESS_SLOTS          6

/* Broadcast Address parameters */
#define CP_BCAST_ADDRESS_MAX_SIZE   15


/* Default Service Option */
#define CP_CUSTOM_DEFAULT_SERVICE_OPTION   3   /* Rate set 1, EVRC  */

/* Service Configuration - Maximum Number of Service Option Connections */
#define CP_MAX_SERV_OPT_CONN_REC           2

/* Maximum Service Option Control Message Record Length */
#define MAX_SERV_OPT_CNTL_RECORD_LEN       128

/* Maximum Mobile Station Supported Supplemental Code Channels */
#define CP_MAX_SUP_CODE_CHANNELS  7

/* Maximum Sizes for Messaging Elements */
#define CP_MAX_CALLED_PARTY_NUMBER_SIZE    32  /* Called Party # record   */
#define CP_MAX_CALLING_PARTY_NUMBER_SIZE   32  /* Calling Party # record  */
#define CP_MAX_REDIRECTING_NUMBER_SIZE     32  /* Redirecting # record    */
#define CP_MAX_EXTENDED_DISPLAY_SIZE       82  /* Extended Display record */
#define CP_MAX_DISPLAY_SIZE                82  /* Display record          */
#define CP_MAX_KEYPAD_FACILITY_SIZE        32  /* Keypad Facility record  */
#define CP_MAX_SUBADDRESS_SIZE             32  /* Subaddress record       */
#define CP_MAX_BURST_DTMF_DIGITS           255 /* Burst DTMF msg          */
#define CP_MAX_DATA_BURST_CHAR             249 /* Data Burst msg          */
                                                 /* 255 - Data Burst header */
#define CP_MAX_CADENCE_GROUPS_REC          16
#define CP_MAX_EXTENDED_DISPLAY_REC        2

/* Hard Handoff with Return On Failure Support */
#define CP_SUPPORT_HHO_WITH_RETURN_ON_FAILURE	0 


/* Traffic Channel Message Transmitter (tc_mtx.c) */

/* TC Transmitter Queue Sizes */
#define ACKQ_SIZE        8           /* TC Ack Queue Size          */
#define MSGQ_SIZE        8           /* TC Msg Queue Size          */

/* Traffic Channel Transmit Window Size */
#define TC_TX_WINDOW_SIZE                  4
#if ((TC_TX_WINDOW_SIZE == 0) || (TC_TX_WINDOW_SIZE > 4))
#error "Bad TC Transmit Window Size!"
#endif

/* Minimum backoff time in frames from T2m to transmit an ACK */
/*                                                                 **
**       Have to take into account potential queuing delay of      **
**       1 frame, message passing, and one frame for insurance.    **
**       Note: An Acknowledgement Message should fit in one frame: **
**       MSG_LENGTH + MSG_TYPE + ACK_FIELDS + ORDER +              **
**       ADD_REC_LEN + RESERVED + CRC = 56 bits = 7 bytes          **
**                                                                 **
**       Mux 1 9600  bps 1/2 rate  - 88  signalling bits           **
**       Mux 2 14400 bps 1/2 rate  - 138 signalling bits           **
**                                                                 */
#define TC_TX_PIGGYBACK_BACKOFF    3
#if ((TC_TX_PIGGYBACK_BACKOFF < 3) || (TC_TX_PIGGYBACK_BACKOFF > 10))
#error "Bad TC Transmit Piggyback Backoff!"
#endif

/* Minimum Signaling Bits Per Frame */ 
/* MUX OPTION 1 (RATE SET 1) 9600 bps, 1/2 rate, Dim & Burst */
#define TC_TX_MIN_MUX1_SIG_BITS_PER_FRAME  MUX1_CAT2_SIG_LEN
/* MUX OPTION 2 (RATE SET 2)   14400 bps, 1/2 rate, Dim & Burst */
#define TC_TX_MIN_MUX2_SIG_BITS_PER_FRAME  MUX2_CAT2_SIG_LEN

/* Minimum Closed Loop Power Control Step Size */
#define CP_MIN_PWR_CNTL_STEP               2

/* Traffic Channel Pilot Set Maintenance */
#define CP_T_DROP_COUNT   1

/* Amount of time to inhibit Registration after a failure */
#define CP_REG_DELAY_AFTER_FAIL 5000 /* msec */
/* Feature to perform safety net/fallback registration in
 * network conditions that may result in the network not
 * knowing the whereabouts of the MS for extended periods.
 * Feature needed to compensation for poor network configurations
 * seen on the TATA network in India.
 */
#define CP_REG_SAFETY_NET_REGISTRATION

#define ACCESS_FAIL_COUNT 5
#define DELAYED_ORIG_WAITING_TIME 70000/* time wait before abort orig attempt */
                                       /* 70sec (similar to silent retry)     */
/* SNR SR - Silent Retry defines. */
#define MAX_SILENT_RETRIES     3             /* Max no of sub-attempt permitted */
#define SILENT_RETRY_TIMEOUT   4000L         /* 4 seconds */
#define SILENT_RETRY_RESET_TIMEOUT 30000L    /* 30 Sec Failsafe Timeout to reset SR
                                              * upon expiration.
                                              */

#ifndef OTA_MAX_MDN_DIGITS
#define OTA_MAX_MDN_DIGITS 15
#endif

/* MAX_BCASTS is the number of broadcast messages that can be 
 *   checked for duplicates.  
 * If more than MAX_BCASTS distinct messages are received within
 *   four broadcast cycles, messages will not be lost but duplicates
 *   may be reported.
 */
#define MAX_BCASTS      10

/* MAX_SINGLE_AWI_SMS_WORDS is the maximum size of a
 * SMS message that can be received in a single
 * ALERT_WITH_INFO_SMS message. It is in units of FVC words
 * (each of which has 3 bytes of user data)
 * NOTE: This must not be greater than 127 !
 */
#define MAX_SINGLE_AWI_SMS_WORDS 64

/* MAX_ALERT_WITH_INFO_SMS_SIZE is the maximum size of an
 * SMS message that can be received via a series of
 * ALERT_WITH_INFO_SMS messages.
 */
#define MAX_ALERT_WITH_INFO_SMS_SIZE 256

/* Default Customer Preferred Service Option */
#define PSW_DEFAULT_LSD_SO_PREF    15
#define PSW_DEFAULT_MSD_SO_PREF    33
#define PSW_DEFAULT_ASYNC_SO       12
#define PSW_DEFAULT_FAX_SO         13
#define PSW_DEFAULT_IS126_SO       2
#define PSW_DEFAULT_MARKOV_SO      0x801E
#define PSW_DEFAULT_SUPL_LPBK_SO   30
#define PSW_DEFAULT_SMS_SO         6
#define PSW_DEFAULT_OTASP_SO       19
#define PSW_DEFAULT_CSC_SO         35
#define PSW_DEFAULT_VOICE_SO       3

/* CP enable MEID support: define - enalbe MEID handling; 
                           not define - disable MEID handling;
   Note: this directive is only used to control the support of MEID when
         P_REV_IN_USE is less than 9.  For P_REV_IN_USE >= 9, the MEID support
		 is always enabled. 
 */
#define CP_ENABLE_MEID 	1


#endif  /* _CUSTOM_H_ */
