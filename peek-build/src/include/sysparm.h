#ifndef _SYSPARM_H_
#define _SYSPARM_H_


/*---------------------------------------------**
** System Parameters Shared between DSP and CP **
**---------------------------------------------*/

#define SYS_NUM_FINGERS         4                  /* Number of "normal" Rx fingers. Normal
                                                    * fingers are indexed 0 -> SYS_NUM_FINGERS-1
                                                    */

#define SYS_FINGER_SYSTEM_ID    (SYS_NUM_FINGERS)  /* ID of the Rx/Tx system time finger */

#define SYS_MAX_BASE_STATIONS   6                  /* Maximum number of base stations with 
                                                    * which a mobile can be in softhandoff 
                                                    */ 

#define SYS_VOICE_PCKT_DATA_LENGTH 1

#define SYS_NUM_CHNL                     4 /* number of physical channels */

#define SYS_MAX_NUM_GAIN_POINTS_TXAGC    8 
#define SYS_MAX_NUM_GAIN_POINTS_RXAGC    8
#define SYS_MAX_NUM_HYST_STATES_TXAGC    3
#define SYS_MAX_NUM_HYST_STATES_RXAGC    3
#define SYS_LIN_APPROX_SLOPE_Q           8  /* Q of the Slope for the Linear approx of RxGainDb/DAC and DAC/TxPwrDbm */


#define SYS_MAX_CANDIDATE_FREQ_LIST     10 /* ?? guess */
#define SYS_MAX_ACTIVE_LIST_PILOTS       6 /* max number of pilot pn's in acitve list  */
#define SYS_MAX_CANDIDATE_LIST_PILOTS   10 /* max number of pilot pn's in candidate list  */
#define SYS_MAX_NEIGHBOR_LIST_PILOTS    40 /* max number of pilot pn's in neighbor list  */
#define SYS_MAX_AFLT_LIST_PILOTS         5 /* max number of pilot pn's in neighbor list  */
#define SYS_MAX_PRIORITY_LIST_PILOTS    40 /* max number of pilots in priority list */
#define SYS_MAX_REMAINING_LIST_PILOTS    5 /* max number of pilots in remaining set */

/* Tx SD DAC Output voltage range settings */
#define SYS_TX_SD_124MV_GAIN_SEL   0x00
#define SYS_TX_SD_173MV_GAIN_SEL   0x01
#define SYS_TX_SD_244MV_GAIN_SEL   0x02
#define SYS_TX_SD_345MV_GAIN_SEL   0x03
#define SYS_TX_SD_486MV_GAIN_SEL   0x04
#define SYS_TX_SD_687MV_GAIN_SEL   0x05
#define SYS_TX_SD_968MV_GAIN_SEL   0x06
#define SYS_TX_SD_1371MV_GAIN_SEL  0x07
#define SYS_TX_SD_1934MV_GAIN_SEL  0x08

/*-----------------------------------------------------------*/
/* DSPM Algorithmic Configuration Message Parameter Defaults */
/*-----------------------------------------------------------*/

/*-------------------------*/
/* Reverse Channel         */
/*-------------------------*/

#define SYS_DSPM_REV_REVMIXSIGPARM0_DEFAULT                       0xFFF0                /* DEV_TX_GAIN_COMP1 */
#define SYS_DSPM_REV_REVMIXSIGPARM1_DEFAULT                       0xFFF0                /* DEV_TX_GAIN_COMP2 */
#define SYS_DSPM_REV_REVMIXSIGPARM2_DEFAULT                       SYS_TX_SD_486MV_GAIN_SEL
#define SYS_DSPM_REV_REVMIXSIGPARM3_DEFAULT                       0x02C0                /* RevClipL */
#define SYS_DSPM_REV_REVMIXSIGPARM4_DEFAULT                       0x007F                /* RevClipMaxThres */
#define SYS_DSPM_REV_REVMIXSIGPARM5_DEFAULT                       0x03FF                /* RevClipMaxSigmaX */
#define SYS_DSPM_REV_REVMIXSIGPARM6_DEFAULT                       0x07FF                /* RevClipSigmaP */
#define SYS_DSPM_REV_REVMIXSIGPARM7_DEFAULT                       27034                 /* RevKsThresTbl[0] */
#define SYS_DSPM_REV_REVMIXSIGPARM8_DEFAULT                       19149                 /* RevKsThresTbl[1] */
#define SYS_DSPM_REV_REVMIXSIGPARM9_DEFAULT                       13517                 /* RevKsThresTbl[2] */
#define SYS_DSPM_REV_REVMIXSIGPARM10_DEFAULT                      9574                  /* RevKsThresTbl[3] */
#define SYS_DSPM_REV_REVMIXSIGPARM11_DEFAULT                      6758                  /* RevKsThresTbl[4] */
#define SYS_DSPM_REV_REVMIXSIGPARM12_DEFAULT                      4787                  /* RevKsThresTbl[5] */
#define SYS_DSPM_REV_REVMIXSIGPARM13_DEFAULT                      3379                  /* RevKsThresTbl[6] */
#define SYS_DSPM_REV_REVMIXSIGPARM14_DEFAULT                      2394                  /* RevKsThresTbl[7] */
#define SYS_DSPM_REV_REVMIXSIGPARM15_DEFAULT                      0                     /* RevKsThresTbl[8] */


/*-------------------------*/
/* Forward Power Control   */
/*-------------------------*/
    
#define SYS_DSPM_FPC_OLCOARSEK1_DEFAULT                           4000   /* Q14, 0.244 */
#define SYS_DSPM_FPC_OLCOARSEK2_DEFAULT                           0x2000 /* Q14, 0.500 */       
#define SYS_DSPM_FPC_OLFINEC1_DEFAULT                             3277   /* Q14, 0.200 */
#define SYS_DSPM_FPC_OLFINEC2_DEFAULT                             4914   /* Q14, 0.300 */
#define SYS_DSPM_FPC_TESTMODE_DEFAULT                             0
#define SYS_DSPM_FPC_AVGNUMPCG_DEFAULT                            4      /* log2(NumPcgs) */

#if ( (SYS_ASIC == SA_ROM) && (SYS_VERSION <= SV_REV_C3) )
#define SYS_DSPM_FPC_NTIIRCOEF_DEFAULT                            0xCCC  /* Q15, 0.1 */ 
#define SYS_DSPM_FPC_NTFLOOR_DEFAULT                              10     /* Q0         */
#define SYS_DSPM_FPC_MAXESNT_DEFAULT                              0x2000 /* Q8 , 32dB  , old algorithm (not used) */
#define SYS_DSPM_FPC_THRCORRVAL_DEFAULT                           0x0100 /* Q8 , 1dB   , old algorithm (not used) */
#define SYS_DSPM_FPC_STEPCORRVAL_DEFAULT                          0x0100
#define SYS_DSPM_FPC_STEPNUMFRMBITS_DEFAULT                       8
#define SYS_DSPM_FPC_STEPSIZEDEFAULT_DEFAULT                      0x0080
#define SYS_DSPM_FPC_STEPSZESTENABLE_DEFAULT                      0
#define SYS_DSPM_FPC_EBNTTHRESSUPER_DEFAULT                       0      /* Dcch DTX, Channel present if PcbEbNt > 0dB */
#define SYS_DSPM_FPC_EBNTTHRESVALBS_RC3_DEFAULT                   940    /* Viterbi Metric values, same as 0x3AC */
#define SYS_DSPM_FPC_EBNTTHRESVALBS_RC4_DEFAULT                   1056   /* Viterbi Metric values, same as 0x420 */
#define SYS_DSPM_FPC_EBNTTHRESVALBS_RC5_DEFAULT                   2331   /* Viterbi Metric values, same as 0x91B */
#define SYS_DSPM_FPC_PCBEBNTCAL_DEFAULT                           0x0480 /* Q8, 4.5dB, obsolete: 0x0200*/
#define SYS_DSPM_FPC_SCHEBNTCAL_DEFAULT                           0x061A /* Q8, 6.1dB, obsolete: 0x0200*/
#endif

#define SYS_DSPM_FPC_EBNTDELTASTEP_DEFAULT                        0x0080 /* Q8, 0.5dB */
#define SYS_DSPM_FPC_FCHEBNTNOISEOFT_DEFAULT                      0x00DA /* Q8, 0.8515625 linear */
#define SYS_DSPM_FPC_SCHEBNTNOISEOFT1_DEFAULT                     0x01EF /* Q8, 1.9336 linear */
#define SYS_DSPM_FPC_SCHEBNTNOISEOFT2_DEFAULT                     0x00FD /* Q8, 0.9875 linear */

#if ( (SYS_ASIC == SA_ROM) && (SYS_VERSION <= SV_REV_C3) )
#define SYS_DSPM_FPC_PRIEBNTDTXTHRES_DEFAULT                      0x8000 /* Q8 */
#define SYS_DSPM_FPC_SECEBNTDTXTHRES_DEFAULT                      0x0000 /* Q8 */
#else
#define SYS_DSPM_FPC_SCHEBNTDTXTHRES_16X_DEFAULT                  0x0000 /* Q8, dB */
#define SYS_DSPM_FPC_SCHEBNTDTXTHRES_8X_DEFAULT                   0x0000 /* Q8, dB */
#define SYS_DSPM_FPC_SCHEBNTDTXTHRES_4X_DEFAULT                   0x0000 /* Q8, dB */
#define SYS_DSPM_FPC_SCHEBNTDTXTHRES_2X_DEFAULT                   0x0000 /* Q8, dB */
#define SYS_DSPM_FPC_SCHEBNTDTXTHRES_1X_DEFAULT                   0x0000 /* Q8, dB */
#define SYS_DSPM_FPC_EBNTIIRALPHA_FCH_DEFAULT                     307    /* Q9, 0.6 */ 
#define SYS_DSPM_FPC_EBNTIIRALPHA_SCH_DEFAULT                     307    /* Q9, 0.6 */ 
#endif

/*-------------------------*/
/* FPC Calibration factors */
/*-------------------------*/

/* These are used to defined the Q8 FpcChCalFactor table in l1ddspmconfig.c
which is a 2-D array of [RC][rate].  RC goes from 1 to 5 and rate goes
from 153.6k down to 1.5k.  Negative values have to be typecast'ed to
(int16) to avoid compliler warnings on the CP side */
#define SYS_DSPM_FPC_CALFCTR_RC1_153k6			0x0000 
#define SYS_DSPM_FPC_CALFCTR_RC1_76k8			0x0000
#define SYS_DSPM_FPC_CALFCTR_RC1_38k4			0x0000
#define SYS_DSPM_FPC_CALFCTR_RC1_19k2			0x0000
#define SYS_DSPM_FPC_CALFCTR_RC1_9k6			((int16)0xff4d) /* -0.70dB */
#define SYS_DSPM_FPC_CALFCTR_RC1_4k8			((int16)0xff1a) /* -0.90dB */
#define SYS_DSPM_FPC_CALFCTR_RC1_2k7			((int16)0xff4d) /* -0.70dB */
#define SYS_DSPM_FPC_CALFCTR_RC1_1k5			((int16)0xff34) /* -0.80dB */

#define SYS_DSPM_FPC_CALFCTR_RC2_153k6			0x0000
#define SYS_DSPM_FPC_CALFCTR_RC2_76k8			0x0000
#define SYS_DSPM_FPC_CALFCTR_RC2_38k4			0x0000
#define SYS_DSPM_FPC_CALFCTR_RC2_19k2			0x0000
#define SYS_DSPM_FPC_CALFCTR_RC2_9k6			((int16)0xfb00) /* -5.00dB */
#define SYS_DSPM_FPC_CALFCTR_RC2_4k8			((int16)0xfa9a) /* -5.40dB */
#define SYS_DSPM_FPC_CALFCTR_RC2_2k7			((int16)0xfacd) /* -5.20dB */ 
#define SYS_DSPM_FPC_CALFCTR_RC2_1k5			((int16)0xfacd) /* -5.20dB */ 

#define SYS_DSPM_FPC_CALFCTR_RC3_153k6			0x0180 /* 1.50dB */
#define SYS_DSPM_FPC_CALFCTR_RC3_76k8			0x0180 /* 1.50dB */
#define SYS_DSPM_FPC_CALFCTR_RC3_38k4			0x0180 /* 1.50dB */
#define SYS_DSPM_FPC_CALFCTR_RC3_19k2			0x0180 /* 1.50dB */
#define SYS_DSPM_FPC_CALFCTR_RC3_9k6			0x01e6 /* 1.90dB */
#define SYS_DSPM_FPC_CALFCTR_RC3_4k8			0x0200 /* 2.00dB */
#define SYS_DSPM_FPC_CALFCTR_RC3_2k7			0x0266 /* 2.40dB */
#define SYS_DSPM_FPC_CALFCTR_RC3_1k5			0x0300 /* 3.00dB */

#define SYS_DSPM_FPC_CALFCTR_RC4_153k6			0x0360 /* 3.38dB */
#define SYS_DSPM_FPC_CALFCTR_RC4_76k8			0x0360 /* 3.38dB */
#define SYS_DSPM_FPC_CALFCTR_RC4_38k4			0x0360 /* 3.38dB */
#define SYS_DSPM_FPC_CALFCTR_RC4_19k2			0x0360 /* 3.38dB */
#define SYS_DSPM_FPC_CALFCTR_RC4_9k6			0x0266 /* 2.40dB */
#define SYS_DSPM_FPC_CALFCTR_RC4_4k8			0x0266 /* 2.40dB */
#define SYS_DSPM_FPC_CALFCTR_RC4_2k7			0x02CC /* 2.80dB */
#define SYS_DSPM_FPC_CALFCTR_RC4_1k5			0x0380 /* 3.50dB */

#define SYS_DSPM_FPC_CALFCTR_RC5_153k6			0x0300 /* need cal */
#define SYS_DSPM_FPC_CALFCTR_RC5_76k8			0x0300 /* need cal */
#define SYS_DSPM_FPC_CALFCTR_RC5_38k4                   0x0300 /* need cal */
#define SYS_DSPM_FPC_CALFCTR_RC5_19k2			0x0300 /* need cal */
#define SYS_DSPM_FPC_CALFCTR_RC5_9k6			0x03cc /* 3.80dB */
#define SYS_DSPM_FPC_CALFCTR_RC5_4k8			0x03cc /* 3.80dB */
#define SYS_DSPM_FPC_CALFCTR_RC5_2k7			0x03cc /* 3.80dB */
#define SYS_DSPM_FPC_CALFCTR_RC5_1k5			0x03cc /* 3.80dB */

/*-------------------------*/
/* Searcher                */
/*-------------------------*/

#define SYS_DSPM_SCH_NORMFACTOR_SEARCHER_DEFAULT                  0x3300
#define SYS_DSPM_SCH_NORMFACTOR_FINGER_DEFAULT                    0x2D4C
#define SYS_DSPM_SCH_FINGERALLOC_AVERAGENUM_DEFAULT               2
#define SYS_DSPM_SCH_FINGERALLOC_INTERVAL_DEFAULT                 0x600
#define SYS_DSPM_SCH_FINGERALLOC_ECIOABSTHRESH_DEFAULT            1038 
#define SYS_DSPM_SCH_FINGERALLOC_OFFSETDIST_DEFAULT               8
#define SYS_DSPM_SCH_FINGERALLOC_FNGRECIOFILTCONST_DEFAULT        0x7999
#define SYS_DSPM_SCH_FINGERALLOC_FNGRECIOINSTCONST_DEFAULT        0x77ff
#define SYS_DSPM_SCH_FINGERALLOC_SIDELOBEDIST_DEFAULT             15
#define SYS_DSPM_SCH_FINGERALLOC_ECIORELTHRESH_DEFAULT            2
#define SYS_DSPM_SCH_TIMETRACK_FASTSYSTEMTIMETRACKNUM_DEFAULT     4 
#define SYS_DSPM_SCH_TIMETRACK_FASTTIMETRACKNUM_DEFAULT           4 
#define SYS_DSPM_SCH_TIMETRACK_FGRALLOCOFFSETDISTQ3_DEFAULT       8
#define SYS_DSPM_SCH_TIMETRACK_TRKSYSTIMEMINFRAMES_DEFAULT        8         /* 160msec */
#define SYS_DSPM_SCH_TIMETRACK_TRKMINPWR_DEFAULT                  519 
#define SYS_DSPM_SCH_TIMETRACK_TIMETRACKCOEFR1_DEFAULT            0x1a3f
#define SYS_DSPM_SCH_TIMETRACK_COMPPN_DEFAULT                     2
#define SYS_DSPM_SCH_LISTPROC_ACTSETTIMER_DEFAULT                 0x360
#define SYS_DSPM_SCH_LISTPROC_PWRRPTTIMER_DEFAULT                 0x1000
#define SYS_DSPM_SCH_SPY_USABLEPATHCNTR_DEFAULT                   0xc00
#define SYS_DSPM_SCH_SPY_FINGERSTATUSCNTR_DEFAULT                 0x1000
    
/* Amala K. - CR 1894 - Modified AFLT algorithm parameters */
#define SYS_DSPM_SCH_MSETS_DEFAULT                                8
#define SYS_DSPM_SCH_COARSEDWELLLEN1_DEFAULT                      0x0f
#define SYS_DSPM_SCH_COARSEDWELLLEN2_DEFAULT                      0x2f
#define SYS_DSPM_SCH_COARSETHRESH1_DEFAULT                        0
#define SYS_DSPM_SCH_COARSETHRESH2ABS_DEFAULT                     0x190
#define SYS_DSPM_SCH_COARSETHRESH2RELSHFT_DEFAULT                 1
#define SYS_DSPM_SCH_COARSEAVERAGENUM_DEFAULT                     5
#define SYS_DSPM_SCH_COARSEWINSIZE_DEFAULT                        149
#define SYS_DSPM_SCH_FINEDWELLLEN1_DEFAULT                        0x3f
#define SYS_DSPM_SCH_FINEDWELLLEN2_DEFAULT                        0x3f
#define SYS_DSPM_SCH_FINEDWELLLENTOTAL_DEFAULT                    2048
#define SYS_DSPM_SCH_FINETHRESH1_DEFAULT                          0x16
#define SYS_DSPM_SCH_FINETHRESH2ABS_DEFAULT                       0x90
#define SYS_DSPM_SCH_FINEAVERAGENUM_DEFAULT                       4
#define SYS_DSPM_SCH_FINEWINSIZE_DEFAULT                          3
#define SYS_DSPM_SCH_SPYPILOTPN_DEFAULT                           8
#define SYS_DSPM_SCH_FINETHRESH3ABS_DEFAULT                       0 /* Supposed to be 0x204 , -16dB.  But set to 0 now to disable parm. */

#define SYS_DSPM_SCH_FASTPWRRPTTIMER                              0x180  
#define SYS_DSPM_SCH_QPCHNEIGHBORFASTPWRRPTTIMER                  1
#define SYS_DSPM_SCH_IDLE_SEARCHTIMEMIN                           0x180 /* Slow Search - used in idle */
#define SYS_DSPM_SCH_SEARCHTIMEMIN                                1     /* Continuous Search - used in traffic */
#define SYS_DSPM_SCH_EARLIESTFNGRECIOABSTHRESH                    1038
#define SYS_DSPM_SCH_NOISEFLOOR                                   52
#define SYS_DSPM_SCH_COHIAQTHRESH                                 0x51F
#define SYS_DSPM_SCH_SYSNUMFINGERS                                4
#define SYS_DSPM_SCH_SYSNUMFINGERS_153K_RATE                      4
#define SYS_DSPM_SCH_SIDELOBEDIST0                                12
#define SYS_DSPM_SCH_SIDELOBEDIST1                                20
#define SYS_DSPM_SCH_SIDELOBEDIST2                                36
#define SYS_DSPM_SCH_SIDELOBEWIN                                  4
#define SYS_DSPM_SCH_FGRALLOCMPATHTHRESH                          1
#define SYS_DSPM_SCH_FGRALLOCSOFTOHTHRESH                         2
#define SYS_DSPM_SCH_MAXFINGERBS                                  SYS_DSPM_SCH_SYSNUMFINGERS 
#define SYS_DSPM_SCH_MAXFINGERBS_153K_RATE                        SYS_DSPM_SCH_SYSNUMFINGERS_153K_RATE 
#define SYS_DSPM_SCH_MINACQWINSIZE                                31
#define SYS_DSPM_SCH_FGRALLOCDROPTIMER                            14  
#define SYS_DSPM_SCH_FGRALLOCDROPTHRESH                           0x208     /* -21dB */
#define SYS_DSPM_SCH_DECIMATIONCNTR                               128       /* 1.25msec per unit, 160msec */
#define SYS_DSPM_SCH_FGROFFSETQ3                                  2         /* 1/8 Tc resolution, fractional offset between searcher and finger */
#define SYS_DSPM_SCH_PWR_RPT_COMB_THRESH                          520       /* -21dB */
#define SYS_DSPM_SCH_3RAY_ADDFLOOR_THRESH                         325       /* (325*0.796) in Q16 = -18dB */
#define SYS_DSPM_SCH_3RAY_FLOOR                                   0xA3
#define SYS_DSPM_SCH_NCOHIAQTHRESH                                590 
#define SYS_DSPM_SCH_IAQMINPNDIST                                 64        /* Tc/2 */ 
#define SYS_DSPM_SCH_IAQRXPWRTHRESH                               0xE980    /* -90dB */

#define SYS_DSPM_SCH_NCOH_DWELLLEN1                                1
#define SYS_DSPM_SCH_NCOH_DWELLPCNT1                               8
#define SYS_DSPM_SCH_NCOH_THRESH1                                350
#define SYS_DSPM_SCH_NCOH_DWELLLEN2                                1
#define SYS_DSPM_SCH_NCOH_DWELLPCNT2                              23
#define SYS_DSPM_SCH_NCOH_SORTLISTSZ                              16
#define SYS_DSPM_SCH_NCOH_THRESHSORT                           0x19D
#define SYS_DSPM_SCH_NCOH_VERIFWIN                                16
#define SYS_DSPM_SCH_NCOH_THRESHVERIF                          0x110

#define SYS_DSPM_SCH_COH_DWELLLEN1                                15
#define SYS_DSPM_SCH_COH_DWELLLEN2                                47
#define SYS_DSPM_SCH_COH_THRESH1                                  22


/*-------------------------*/
/* RF Control              */
/*-------------------------*/

#define SYS_DSPM_RFC_RXAGCFASTSTIME_DEFAULT                       4
#define SYS_DSPM_RFC_RXAGCSLOWTIME_DEFAULT                        9
#define SYS_DSPM_RFC_RXAGCFASTDECIMMSG_DEFAULT                    4
#define SYS_DSPM_RFC_RXAGCSLOWDECIMMSG_DEFAULT                    24
    
#define SYS_DSPM_RFC_RELIABLEBSTHRESHPWR_DEFAULT                  -48
    
#define SYS_DSPM_RFC_DCBFASTDECIM_DEFAULT                         1
#define SYS_DSPM_RFC_DCBFASTDECIMMSG_DEFAULT                      2
#define SYS_DSPM_RFC_DCBFASTSHIFTUP_DEFAULT                       -1
    
#define SYS_DSPM_RFC_IIR_PDM_RECOVERY_DEFAULT                     0x6400 /* 0x0c80 in Q3 */
#define SYS_DSPM_RFC_IIR_IQ_THRESHOLD_DEFAULT                     0x4000

#define SYS_DSPM_RFC_PDM_MAX_VALUE                                0x0FFF
#define SYS_DSPM_RFC_PDM_MIN_VALUE                                0x0000

#define SYS_DSPM_DCXO_PDM_MAX_VALUE                               0x01FF
#define SYS_DSPM_DCXO_PDM_MIN_VALUE                               0x0000

#define SYS_DSPM_RFC_AFC_IAQ_POS_OFST                             0x0200
#define SYS_DSPM_RFC_AFC_IAQ_NEG_OFST                             0xFE00


/*-------------------------*/
/* DAGC                    */
/*-------------------------*/
#define SYS_DSPM_RFC_RX_DAGC_TYPE_DEFAULT                         0      /* ANALOG_RXAGC */
#define SYS_DSPM_RFC_RX_DAGC_DGAIN_MAX_DEFAULT                    0x0200 /* Max Bitsel =  1, Log2, Q8 */
#define SYS_DSPM_RFC_RX_DAGC_DGAIN_MIN_DEFAULT                    0xF200 /* Min Bitsel = -7, Log2, Q8 */ 

#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MAX_DEFAULT                    0x0008

#define SYS_DSPM_RFC_RX_DAGC_SGAIN_STEP_0_DEFAULT                 0x0000 /* 0 dB, Log2 Q8 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_STEP_1_DEFAULT                 0x0551 /* 16dB */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_STEP_2_DEFAULT                 0x0AA2 /* 32dB */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_STEP_3_DEFAULT                 0x0FF2 /* 48dB */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_STEP_4_DEFAULT                 0x1345 /* 58dB */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_STEP_5_DEFAULT                 0x1741 /* 70dB */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_STEP_6_DEFAULT                 0x1741 /* 70dB */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_STEP_7_DEFAULT                 0x1741 /* 70dB */

#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_0L_DEFAULT               0x7FFF /* upper limit, dBm, 10Log10, Q6 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_0H_DEFAULT               0xF740 /* -35dBm */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_1L_DEFAULT               0xF840 /* -31dBm */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_1H_DEFAULT               0xF340 /* -51dBm */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_2L_DEFAULT               0xF440 /* -47dBm */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_2H_DEFAULT               0xEF40 /* -67dBm */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_3L_DEFAULT               0xF040 /* -63dBm */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_3H_DEFAULT               0xEA80 /* -86dBm */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_4L_DEFAULT               0xEB80 /* -82dBm */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_4H_DEFAULT               0xE7C0 /* -97dBm */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_5L_DEFAULT               0xE8C0 /* -93dBm */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_5H_DEFAULT               0x8000 /* lower limit */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_6L_DEFAULT               0xE8C0 /* -93dBm */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_6H_DEFAULT               0x8000 /* lower limit */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_7L_DEFAULT               0xE8C0 /* -93dBm */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_THRES_7H_DEFAULT               0x8000 /* lower limit */

#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_0L_DEFAULT                 0x8000 /* Lower: 1, New State: 0 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_0H_DEFAULT                 0x0100 /* Upper: 0, New State: 1 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_1L_DEFAULT                 0x8000 /* Lower: 1, New State: 0 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_1H_DEFAULT                 0x0200 /* Upper: 0, New State: 2 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_2L_DEFAULT                 0x8100 /* Lower: 1, New State: 1 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_2H_DEFAULT                 0x0300 /* Upper: 0, New State: 3 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_3L_DEFAULT                 0x8200 /* Lower: 1, New State: 2 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_3H_DEFAULT                 0x0400 /* Upper: 0, New State: 4 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_4L_DEFAULT                 0x8300 /* Lower: 1, New State: 3 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_4H_DEFAULT                 0x0500 /* Upper: 0, New State: 5 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_5L_DEFAULT                 0x8400 /* Lower: 1, New State: 4 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_5H_DEFAULT                 0x0500 /* Upper: 0, New State: 5 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_6L_DEFAULT                 0x8500 /* Lower: 1, New State: 5 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_6H_DEFAULT                 0x0500 /* Upper: 0, New State: 5 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_7L_DEFAULT                 0x8500 /* Lower: 1, New State: 5 */
#define SYS_DSPM_RFC_RX_DAGC_SGAIN_MSC_7H_DEFAULT                 0x0500 /* Lower: 1, New State: 5 */

#define SYS_DSPM_RFC_RX_DAGC_CAL_REF_DBM_DEFAULT                  0xE6C0 /* -101dBm in 10Log10, Q6 */
#define SYS_DSPM_RFC_RX_DAGC_CAL_REF_GAIN_DEFAULT                 0x232C /* RF Gain required for -101dBm in Log2, Q8 */

#define SYS_DSPM_RFC_RX_DAGC_IIRRESET_PDM_THRES_DEFAULT           0x07F0 /* Digital gain threshold mapped to -15dB, Log2, Q8 */
#define SYS_DSPM_RFC_RX_DAGC_IIRRESET_PDM_RECVR_DEFAULT           0x0CE7 /* Digital gain recover value mapped to -30dB, Log2, Q8 */

/*-------------------------*/
/* Forward Channel         */
/*-------------------------*/
    
#define SYS_DSPM_FWD_VITSCALE_RC1_RDA_RT_FULL_DEFAULT             0x0000
#define SYS_DSPM_FWD_VITSCALE_RC1_RDA_RT_HALF_DEFAULT             0x0000
#define SYS_DSPM_FWD_VITSCALE_RC1_RDA_RT_QUARTER_DEFAULT          0x0010
#define SYS_DSPM_FWD_VITSCALE_RC1_RDA_RT_EIGHTH_DEFAULT           0x0020

#define SYS_DSPM_FWD_VITSCALE_RC2_RDA_RT_FULL_DEFAULT             0x0000
#define SYS_DSPM_FWD_VITSCALE_RC2_RDA_RT_HALF_DEFAULT             0x0000
#define SYS_DSPM_FWD_VITSCALE_RC2_RDA_RT_QUARTER_DEFAULT          0x0010
#define SYS_DSPM_FWD_VITSCALE_RC2_RDA_RT_EIGHTH_DEFAULT           0x0020

#define SYS_DSPM_FWD_VITSCALE_RC3_RDA_RT_FULL_DEFAULT             0x0010
#define SYS_DSPM_FWD_VITSCALE_RC3_RDA_RT_HALF_DEFAULT             0x0020
#define SYS_DSPM_FWD_VITSCALE_RC3_RDA_RT_QUARTER_DEFAULT          0x0020
#define SYS_DSPM_FWD_VITSCALE_RC3_RDA_RT_EIGHTH_DEFAULT           0x0020
 
#define SYS_DSPM_FWD_VITSCALE_RC4_RDA_RT_FULL_DEFAULT             0x0000
#define SYS_DSPM_FWD_VITSCALE_RC4_RDA_RT_HALF_DEFAULT             0x0000
#define SYS_DSPM_FWD_VITSCALE_RC4_RDA_RT_QUARTER_DEFAULT          0x0010
#define SYS_DSPM_FWD_VITSCALE_RC4_RDA_RT_EIGHTH_DEFAULT           0x0020

#define SYS_DSPM_FWD_VITSCALE_RC5_RDA_RT_FULL_DEFAULT             0x0000
#define SYS_DSPM_FWD_VITSCALE_RC5_RDA_RT_HALF_DEFAULT             0x0010
#define SYS_DSPM_FWD_VITSCALE_RC5_RDA_RT_QUARTER_DEFAULT          0x0020
#define SYS_DSPM_FWD_VITSCALE_RC5_RDA_RT_EIGHTH_DEFAULT           0x0020

/* 
   Fwd Rda Apriori Metrics for RC 1 to 5:
   DEF_I(uint16 FwdRdaTtbl[][FWD_RDA_T_TBL_SZ]) = {
   {2.4000, 1.7000, 1.2700, 1.7000, 1.3100, 1.2500}
   {0.9100, 0.6800, 0.4800, 0.7800, 0.6000, 0.8000}
   {0.7700, 0.6000, 0.4500, 0.8000, 0.6300, 0.7900}
   {0.8200, 0.6450, 0.4950, 0.8200, 0.6550, 0.8200}
   {0.8960, 0.6770, 0.4900, 0.7900, 0.5800, 0.7870}
   };
   Same table in <16,2,t> for RC1 and <16,0,t> for RC2to5

   DEF_I(uint16 FwdRdaTtbl[][FWD_RDA_T_TBL_SZ]) = {
   FwdRdaTtbl[RC1][FWD_RDA_T_TBL_SZ] = {0x4CCD, 0x3666, 0x28A4, 0x3666, 0x29EC, 0x2800};
   FwdRdaTtbl[RC2][FWD_RDA_T_TBL_SZ] = {0x747B, 0x570A, 0x3D71, 0x63D7, 0x4CCD, 0x6666};
   FwdRdaTtbl[RC3][FWD_RDA_T_TBL_SZ] = {0x628F, 0x4CCD, 0x399A, 0x6666, 0x50A4, 0x651F};
   FwdRdaTtbl[RC4][FWD_RDA_T_TBL_SZ] = {0x68F6, 0x528F, 0x3F5C, 0x68F6, 0x53D7, 0x68F6};
   FwdRdaTtbl[RC5][FWD_RDA_T_TBL_SZ] = {0x72B0, 0x56A8, 0x3EB8, 0x651F, 0x4A3D, 0x64BC};
   };*/
    
#define SYS_DSPM_FWD_RDATTBL_RC1_FE_DEFAULT                       0x4CCD
#define SYS_DSPM_FWD_RDATTBL_RC1_HE_DEFAULT                       0x3666
#define SYS_DSPM_FWD_RDATTBL_RC1_QE_DEFAULT                       0x28A4
#define SYS_DSPM_FWD_RDATTBL_RC1_FQ_DEFAULT                       0x3666
#define SYS_DSPM_FWD_RDATTBL_RC1_HQ_DEFAULT                       0x29EC
#define SYS_DSPM_FWD_RDATTBL_RC1_FH_DEFAULT                       0x2800
    
#define SYS_DSPM_FWD_RDATTBL_RC2_HF_DEFAULT                       0x747B
#define SYS_DSPM_FWD_RDATTBL_RC2_QF_DEFAULT                       0x570A
#define SYS_DSPM_FWD_RDATTBL_RC2_EF_DEFAULT                       0x3D71
#define SYS_DSPM_FWD_RDATTBL_RC2_QH_DEFAULT                       0x63D7
#define SYS_DSPM_FWD_RDATTBL_RC2_EH_DEFAULT                       0x4CCD
#define SYS_DSPM_FWD_RDATTBL_RC2_EQ_DEFAULT                       0x6666
    
#define SYS_DSPM_FWD_RDATTBL_RC3_HF_DEFAULT                       0x628F
#define SYS_DSPM_FWD_RDATTBL_RC3_QF_DEFAULT                       0x4CCD
#define SYS_DSPM_FWD_RDATTBL_RC3_EF_DEFAULT                       0x399A
#define SYS_DSPM_FWD_RDATTBL_RC3_QH_DEFAULT                       0x6666
#define SYS_DSPM_FWD_RDATTBL_RC3_EH_DEFAULT                       0x50A4
#define SYS_DSPM_FWD_RDATTBL_RC3_EQ_DEFAULT                       0x651F
    
#define SYS_DSPM_FWD_RDATTBL_RC4_HF_DEFAULT                       0x68F6
#define SYS_DSPM_FWD_RDATTBL_RC4_QF_DEFAULT                       0x528F
#define SYS_DSPM_FWD_RDATTBL_RC4_EF_DEFAULT                       0x3F5C
#define SYS_DSPM_FWD_RDATTBL_RC4_QH_DEFAULT                       0x68F6
#define SYS_DSPM_FWD_RDATTBL_RC4_EH_DEFAULT                       0x53D7
#define SYS_DSPM_FWD_RDATTBL_RC4_EQ_DEFAULT                       0x68F6
    
#define SYS_DSPM_FWD_RDATTBL_RC5_HF_DEFAULT                       0x72B0
#define SYS_DSPM_FWD_RDATTBL_RC5_QF_DEFAULT                       0x56A8
#define SYS_DSPM_FWD_RDATTBL_RC5_EF_DEFAULT                       0x3EB8
#define SYS_DSPM_FWD_RDATTBL_RC5_QH_DEFAULT                       0x651F
#define SYS_DSPM_FWD_RDATTBL_RC5_EH_DEFAULT                       0x4A3D
#define SYS_DSPM_FWD_RDATTBL_RC5_EQ_DEFAULT                       0x64BC
    
#define SYS_DSPM_FWD_RDACHKSTATICBITSLEN_RC1_DEFAULT              5
#define SYS_DSPM_FWD_RDACHKSTATICBITSLEN_RC2_DEFAULT              5
#define SYS_DSPM_FWD_RDACHKSTATICBITSLEN_RC3_DEFAULT              5
#define SYS_DSPM_FWD_RDACHKSTATICBITSLEN_RC4_DEFAULT              5
#define SYS_DSPM_FWD_RDACHKSTATICBITSLEN_RC5_DEFAULT              5

/* 
   Fwd Rda Aposteriori Metrics for RC 1 to 5:
   DEF_I(uint16 FwdRdaT0tbl[][FWD_RDA_T0_TBL_SZ]) = {
   {0.74,   0.29,   0.115,  0.41,   0.15,   0.00,   0.35,   0.00,   0.00,   0.00,   0.11,   0.08}
   {0.81,   0.32,   0.14,   0.39,   0.15,   0.78,   0.41,   0.36,   0.27,   0.34,   0.12,   0.08}
   {0.37,   0.25,   0.20,   0.73,   0.58,   0.38,   0.825,  0.71,   0.25,   0.65,   0.46,   0.165}
   {0.71,   0.28,   0.11,   0.39,   0.15,   0.77,   0.41,   0.38,   0.28,   0.32,   0.12,   0.08}
   {0.37,   0.12,   0.09,   0.35,   0.25,   0.37,   0.77,   0.36,   0.13,   0.63,   0.21,   0.075}
   };
   Same table in <16,0,t>

   DEF_I(uint16 FwdRdaT0tbl[][FWD_RDA_T0_TBL_SZ]) = {
   FwdRdaT0tbl[RC1][FWD_RDA_T0_TBL_SZ]) = {0x5EB8, 0x251F, 0x0EB8, 0x347B, 0x1333, 0x0000, 0x2CCD, 0x0000, 0x0000, 0x0000, 0x0E14, 0x0A3D};
   FwdRdaT0tbl[RC2][FWD_RDA_T0_TBL_SZ]) = {0x67AE, 0x28F6, 0x11EB, 0x31EC, 0x1333, 0x63D7, 0x347B, 0x2E14, 0x228F, 0x2B85, 0x0F5C, 0x0A3D};
   FwdRdaT0tbl[RC3][FWD_RDA_T0_TBL_SZ]) = {0x2F5C, 0x2000, 0x1999, 0x5D70, 0x4A30, 0x30A3, 0x699A, 0x5AE1, 0x2000, 0x5333, 0x3AE1, 0x151F};
   FwdRdaT0tbl[RC4][FWD_RDA_T0_TBL_SZ]) = {0x5AE1, 0x23D7, 0x0E14, 0x31EC, 0x1333, 0x628F, 0x347B, 0x30A4, 0x23D7, 0x28F6, 0x0F5C, 0x0A3D};
   FwdRdaT0tbl[RC5][FWD_RDA_T0_TBL_SZ]) = {0x2F5C, 0x0F5C, 0x0B85, 0x2CCD, 0x2000, 0x2F5C, 0x628F, 0x2E14, 0x10A4, 0x50A4, 0x1AE1, 0x099A};
   };*/
    
#define SYS_DSPM_FWD_RDAT0TBL_RC1_T0_HF_DEFAULT                   0x5EB8
#define SYS_DSPM_FWD_RDAT0TBL_RC1_T0_QF_DEFAULT                   0x251F
#define SYS_DSPM_FWD_RDAT0TBL_RC1_T0_EF_DEFAULT                   0x0EB8
#define SYS_DSPM_FWD_RDAT0TBL_RC1_T0_QH_DEFAULT                   0x347B
#define SYS_DSPM_FWD_RDAT0TBL_RC1_T0_EH_DEFAULT                   0x1333
#define SYS_DSPM_FWD_RDAT0TBL_RC1_TP_HF_DEFAULT                   0x0000
#define SYS_DSPM_FWD_RDAT0TBL_RC1_T0_EQ_DEFAULT                   0x2CCD
#define SYS_DSPM_FWD_RDAT0TBL_RC1_TP_QH_DEFAULT                   0x0000
#define SYS_DSPM_FWD_RDAT0TBL_RC1_TP_QF_DEFAULT                   0x0000
#define SYS_DSPM_FWD_RDAT0TBL_RC1_TP_EQ_DEFAULT                   0x0000
#define SYS_DSPM_FWD_RDAT0TBL_RC1_TP_EH_DEFAULT                   0x0E14
#define SYS_DSPM_FWD_RDAT0TBL_RC1_TP_EF_DEFAULT                   0x0A3D
    
#define SYS_DSPM_FWD_RDAT0TBL_RC2_T0_HF_DEFAULT                   0x67AE
#define SYS_DSPM_FWD_RDAT0TBL_RC2_T0_QF_DEFAULT                   0x28F6
#define SYS_DSPM_FWD_RDAT0TBL_RC2_T0_EF_DEFAULT                   0x11EB
#define SYS_DSPM_FWD_RDAT0TBL_RC2_T0_QH_DEFAULT                   0x31EC
#define SYS_DSPM_FWD_RDAT0TBL_RC2_T0_EH_DEFAULT                   0x1333
#define SYS_DSPM_FWD_RDAT0TBL_RC2_TP_HF_DEFAULT                   0x63D7
#define SYS_DSPM_FWD_RDAT0TBL_RC2_T0_EQ_DEFAULT                   0x347B
#define SYS_DSPM_FWD_RDAT0TBL_RC2_TP_QH_DEFAULT                   0x2E14
#define SYS_DSPM_FWD_RDAT0TBL_RC2_TP_QF_DEFAULT                   0x228F
#define SYS_DSPM_FWD_RDAT0TBL_RC2_TP_EQ_DEFAULT                   0x2B85
#define SYS_DSPM_FWD_RDAT0TBL_RC2_TP_EH_DEFAULT                   0x0F5C
#define SYS_DSPM_FWD_RDAT0TBL_RC2_TP_EF_DEFAULT                   0x0A3D
    
#define SYS_DSPM_FWD_RDAT0TBL_RC3_T0_HF_DEFAULT                   0x2F5C
#define SYS_DSPM_FWD_RDAT0TBL_RC3_T0_QF_DEFAULT                   0x2000
#define SYS_DSPM_FWD_RDAT0TBL_RC3_T0_EF_DEFAULT                   0x1999
#define SYS_DSPM_FWD_RDAT0TBL_RC3_T0_QH_DEFAULT                   0x5D70
#define SYS_DSPM_FWD_RDAT0TBL_RC3_T0_EH_DEFAULT                   0x4A3D
#define SYS_DSPM_FWD_RDAT0TBL_RC3_TP_HF_DEFAULT                   0x30A3
#define SYS_DSPM_FWD_RDAT0TBL_RC3_T0_EQ_DEFAULT                   0x699A
#define SYS_DSPM_FWD_RDAT0TBL_RC3_TP_QH_DEFAULT                   0x5AE1
#define SYS_DSPM_FWD_RDAT0TBL_RC3_TP_QF_DEFAULT                   0x2000
#define SYS_DSPM_FWD_RDAT0TBL_RC3_TP_EQ_DEFAULT                   0x5333
#define SYS_DSPM_FWD_RDAT0TBL_RC3_TP_EH_DEFAULT                   0x3AE1
#define SYS_DSPM_FWD_RDAT0TBL_RC3_TP_EF_DEFAULT                   0x151F
    
#define SYS_DSPM_FWD_RDAT0TBL_RC4_T0_HF_DEFAULT                   0x5AE1
#define SYS_DSPM_FWD_RDAT0TBL_RC4_T0_QF_DEFAULT                   0x23D7
#define SYS_DSPM_FWD_RDAT0TBL_RC4_T0_EF_DEFAULT                   0x0E14
#define SYS_DSPM_FWD_RDAT0TBL_RC4_T0_QH_DEFAULT                   0x31EC
#define SYS_DSPM_FWD_RDAT0TBL_RC4_T0_EH_DEFAULT                   0x1333
#define SYS_DSPM_FWD_RDAT0TBL_RC4_TP_HF_DEFAULT                   0x628F
#define SYS_DSPM_FWD_RDAT0TBL_RC4_T0_EQ_DEFAULT                   0x347B
#define SYS_DSPM_FWD_RDAT0TBL_RC4_TP_QH_DEFAULT                   0x30A4
#define SYS_DSPM_FWD_RDAT0TBL_RC4_TP_QF_DEFAULT                   0x23D7
#define SYS_DSPM_FWD_RDAT0TBL_RC4_TP_EQ_DEFAULT                   0x28F6
#define SYS_DSPM_FWD_RDAT0TBL_RC4_TP_EH_DEFAULT                   0x0F5C
#define SYS_DSPM_FWD_RDAT0TBL_RC4_TP_EF_DEFAULT                   0x0A3D
    
#define SYS_DSPM_FWD_RDAT0TBL_RC5_T0_HF_DEFAULT                   0x2F5C
#define SYS_DSPM_FWD_RDAT0TBL_RC5_T0_QF_DEFAULT                   0x0F5C
#define SYS_DSPM_FWD_RDAT0TBL_RC5_T0_EF_DEFAULT                   0x0B85
#define SYS_DSPM_FWD_RDAT0TBL_RC5_T0_QH_DEFAULT                   0x2CCD
#define SYS_DSPM_FWD_RDAT0TBL_RC5_T0_EH_DEFAULT                   0x2000
#define SYS_DSPM_FWD_RDAT0TBL_RC5_TP_HF_DEFAULT                   0x2F5C
#define SYS_DSPM_FWD_RDAT0TBL_RC5_T0_EQ_DEFAULT                   0x628F
#define SYS_DSPM_FWD_RDAT0TBL_RC5_TP_QH_DEFAULT                   0x2E14
#define SYS_DSPM_FWD_RDAT0TBL_RC5_TP_QF_DEFAULT                   0x10A4
#define SYS_DSPM_FWD_RDAT0TBL_RC5_TP_EQ_DEFAULT                   0x50A4
#define SYS_DSPM_FWD_RDAT0TBL_RC5_TP_EH_DEFAULT                   0x1AE1
#define SYS_DSPM_FWD_RDAT0TBL_RC5_TP_EF_DEFAULT                   0x099A

#define SYS_DSPM_FWD_RDADECALLRATES_RC1_DEFAULT                   1
#define SYS_DSPM_FWD_RDADECALLRATES_RC2_DEFAULT                   1
#define SYS_DSPM_FWD_RDADECALLRATES_RC3_DEFAULT                   1
#define SYS_DSPM_FWD_RDADECALLRATES_RC4_DEFAULT                   1
#define SYS_DSPM_FWD_RDADECALLRATES_RC5_DEFAULT                   1

#define SYS_DSPM_FWD_RDAYAMATBL_FWD_RDA_RT_EIGHTH                 0x0203  /* Thres1: 3, Thres2: 8 */
#define SYS_DSPM_FWD_RDAYAMATBL_FWD_RDA_RT_QUARTER                0x0246  /* Thres1: 6, Thres2: 9 */

#define SYS_DSPM_FWD_RDASER_THRES_RC1_E_RT_DEFAULT                8
#define SYS_DSPM_FWD_RDASER_THRES_RC1_Q_RT_DEFAULT                19

#define SYS_DSPM_FWD_DTX_VITMET_THRES_RC3_153k6                   0x7FFF
#define SYS_DSPM_FWD_DTX_VITMET_THRES_RC3_76k8                    0x7FFF
#define SYS_DSPM_FWD_DTX_VITMET_THRES_RC3_38k4                    0x7FFF
#define SYS_DSPM_FWD_DTX_VITMET_THRES_RC3_19k2                    0x7FFF
#define SYS_DSPM_FWD_DTX_VITMET_THRES_RC3_9k6                     0x7FFF 

#define SYS_DSPM_FWD_DTX_VITMET_THRES_RC4_153k6                   0x7FFF
#define SYS_DSPM_FWD_DTX_VITMET_THRES_RC4_76k8                    0x7FFF
#define SYS_DSPM_FWD_DTX_VITMET_THRES_RC4_38k4                    0x7FFF
#define SYS_DSPM_FWD_DTX_VITMET_THRES_RC4_19k2                    0x7FFF
#define SYS_DSPM_FWD_DTX_VITMET_THRES_RC4_9k6                     0x7FFF

#define SYS_DSPM_FWD_DTX_VITMET_THRES_RC5_230k4                   0x7FFF
#define SYS_DSPM_FWD_DTX_VITMET_THRES_RC5_115k2                   0x7FFF
#define SYS_DSPM_FWD_DTX_VITMET_THRES_RC5_57k6                    0x7FFF
#define SYS_DSPM_FWD_DTX_VITMET_THRES_RC5_28k8                    0x7FFF
#define SYS_DSPM_FWD_DTX_VITMET_THRES_RC5_14k4                    0x7FFF

#define SYS_DSPM_FWD_FINGER_NOISE_NORM_ENA_DEFAULT                1
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_ISRA0_DEFAULT              5
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_ISRB0_DEFAULT              0x7000
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_NOMIN_DEFAULT              4
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_0                  0x31D6
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_1                  0x2EE8
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_2                  0x2C4C
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_3                  0x29F8
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_4                  0x27DE
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_5                  0x25F8
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_6                  0x243E
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_7                  0x22AB
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_8                  0x2139
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_9                  0x1FE5
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_10                 0x1EAB
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_11                 0x1D88
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_12                 0x1C7A
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_13                 0x1B7F
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_14                 0x1A94
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC1_15                 0x19B9
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_0                  0x2554
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_1                  0x2323
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_2                  0x212F
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_3                  0x1F70
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_4                  0x1DDD
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_5                  0x1C71
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_6                  0x1B26
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_7                  0x19F8
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_8                  0x18E2
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_9                  0x17E4
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_10                 0x16F8
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_11                 0x161F
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_12                 0x1554
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_13                 0x1498
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_14                 0x13E8
#define SYS_DSPM_FWD_FINGER_NOISE_NORM_TBL_RC2_15                 0x1344

#define SYS_DSPM_FWD_FINGER_D3LMT_ENABLE                          1
#define SYS_DSPM_FWD_FINGER_D3LMT_MON_FREQ                        1
#define SYS_DSPM_FWD_FINGER_D3LMT_FNGR_LIMIT                      2       /* max no. of sch channel fingers */
#define SYS_DSPM_FWD_FINGER_D3LMT_FRMSZ_LIMIT                     3048    /* NumBits in frame */
#define SYS_DSPM_FWD_FINGER_D3LMT_FNGR_ECIO_FACTOR                0x1800  /* 1.5 in Q12*/

/* FixScale Mrc Fixed scaling parameters */
#define SYS_DSPM_FWD_FIXSCALE_RC1                0
#define SYS_DSPM_FWD_FIXSCALE_RC2                0
#define SYS_DSPM_FWD_FIXSCALE_RC3                0
#define SYS_DSPM_FWD_FIXSCALE_RC3_19_2           0
#define SYS_DSPM_FWD_FIXSCALE_RC3_38_4           1
#define SYS_DSPM_FWD_FIXSCALE_RC3_76_8           1
#define SYS_DSPM_FWD_FIXSCALE_RC3_153_6          2
#define SYS_DSPM_FWD_FIXSCALE_RC4                0
#define SYS_DSPM_FWD_FIXSCALE_RC4_19_2           0
#define SYS_DSPM_FWD_FIXSCALE_RC4_38_4           0
#define SYS_DSPM_FWD_FIXSCALE_RC4_76_8           1
#define SYS_DSPM_FWD_FIXSCALE_RC4_153_6          1
#define SYS_DSPM_FWD_FIXSCALE_RC4_307_2          1
#define SYS_DSPM_FWD_FIXSCALE_RC5                0
#define SYS_DSPM_FWD_FIXSCALE_RC5_28_8           0
#define SYS_DSPM_FWD_FIXSCALE_RC5_57_6           0
#define SYS_DSPM_FWD_FIXSCALE_RC5_115_2          1
#define SYS_DSPM_FWD_FIXSCALE_RC5_230_4          1
/* The default value is used where the above are not used */
#define SYS_DSPM_FWD_FIXSCALE_DEFAULT            0




/*-----------------------------------------------------------*/
/* DSPV Algorithmic Configuration Message Parameter Defaults */
/*-----------------------------------------------------------*/

/*--------------------------------------------*/
/* Acoustic Audio Compressor : Defaults only  */
/*--------------------------------------------*/
/* Default values (used by DSPV) */
#define SYS_DSPV_ACS_BLOCKSIZE_DEFAULT                            0x0020 
#define SYS_DSPV_ACS_BLOCKNUM_DEFAULT                             0x0005
#define SYS_DSPV_ACS_RMS_Q_DEFAULT                                ((int16)0xFFFB)
#define SYS_DSPV_ACS_RMS_SCAL_FCTR_DEFAULT                        0x2000
#define SYS_DSPV_ACS_GAMMA_HI_DEFAULT                             ((int16)0xFDE0)  
#define SYS_DSPV_ACS_GAMMA_LO_DEFAULT                             ((int16)0xF920)

#define SYS_DSPV_ACS_BETA_HI_DEFAULT                              0x0666
#define SYS_DSPV_ACS_BETA_LO_DEFAULT                              0x7FFF
#define SYS_DSPV_ACS_BETA_LO_Q_DEFAULT                            ((int16)0xFFFF)
#define SYS_DSPV_ACS_ATTACK_TIME_DEFAULT                          0x0377
#define SYS_DSPV_ACS_RELEASE_TIME_DEFAULT                         0x0059

#define SYS_DSPV_ASVC_MINGAIN_DEFAULT                             0x0000    /* Q8, 0.0 */  
#define SYS_DSPV_ASVC_MAXSTATE_DEFAULT                            0x0010    /* Q0 */        
#define SYS_DSPV_ASVC_NOISESTEP_DEFAULT                           0x0155                   
#define SYS_DSPV_ASVC_GAINSTEP_DEFAULT                            0x0018    /* */           
#define SYS_DSPV_ASVC_NOISETHRES_DEFAULT                          0x0800                   
#define SYS_DSPV_ASVC_HYSTERISIS_DEFAULT                          0x0100                   
#define SYS_DSPV_ASVC_NOISESCALFCTR_DEFAULT                       0x0CCC    /* 0.1, Q15 */ 
#define SYS_DSPV_ASVC_VMTHRES_DEFAULT                             35        /* Q0 */       
#define SYS_DSPV_ASVC_TCETHRES_DEFAULT                            256       /* Q0 */       
#define SYS_DSPV_ASVC_SPECDEVTHRES_DEFAULT                        1120 /*1792 */     /* 56, Q5 */   
#define SYS_DSPV_ASVC_CNTTHRES_DEFAULT                            32 /*10*/        /* Q0 */       
#define SYS_DSPV_ASVC_HYSCNTTHRES_DEFAULT                         6         /* Q0 */       
                                                                                              
#define SYS_DSPV_ACS_ASVCSPKRWEIGHT_DEFAULT                       0x7FFF                      
#define SYS_DSPV_ACS_ASVCSCALFCTRQ_DEFAULT                        -8                          
#define SYS_DSPV_ACS_ASVCSMOOTHFCTR_DEFAULT                       29491     /* 0.90 */        
#define SYS_DSPV_ACS_ASVCMAXSPKRGAIN_DEFAULT                      0x400 
#define SYS_DSPV_ACS_FRAMEPWRSTEP_DEFAULT                         0x0040 
#define SYS_DSPV_ACS_FRAMEPWRTHRES_DEFAULT                        0x0600 
#define SYS_DSPV_ACS_FRAMEPWRHYST_DEFAULT                         0x0030 
#define SYS_DSPV_ACS_SPPGAINSTEP_DEFAULT                          0x0018 
#define SYS_DSPV_ACS_SPPGAINMAXSTATE_DEFAULT                      0x0010 
#define SYS_DSPV_ACS_SPPFRMPWRBUFLEN_DEFAULT                      10    
#define SYS_DSPV_ACS_SPPFRAMECNTTHRES_DEFAULT                     150 
#define SYS_DSPV_ACS_GAINUPDATETHRES_DEFAULT                      5 
#define SYS_DSPV_ACS_GAINUPDATESWCH_DEFAULT                       1 

#define SYS_DSPV_ACS_ACPTONETHRES_DEFAULT                         0x0248   /* 17.25 dB */ 
#define SYS_DSPV_ACS_ACPTONESUPPRESS_DEFAULT                      0x0240   /* 17 dB */ 


/*-------------------------*/
/* Acoustic Echo Canceller */
/*-------------------------*/
/* Default values (used by DSPV) */
#define SYS_DSPV_AEC_FARENDPWRTHES_DEFAULT                  0x007b
#define SYS_DSPV_AEC_SPPNGYTHL_DEFAULT                      0x0002      
#define SYS_DSPV_AEC_NOISETHRES_DEFAULT                     0x0055      
#define SYS_DSPV_AEC_CHNSNRTHL_DEFAULT                      0x0140      
#define SYS_DSPV_AEC_FE2TRTHRES_DEFAULT                     0x7eb8      
#define SYS_DSPV_AEC_TR2NETHRESQ_DEFAULT                    0x0004 
#define SYS_DSPV_AEC_CHANTAPQ_DEFAULT                       0x0000      
#define SYS_DSPV_AEC_PARTITION_DEFAULT                      0x0002      
#define SYS_DSPV_AEC_OVERLAPRATIO_DEFAULT                   0x0003      
#define SYS_DSPV_AEC_DFTSIZE_DEFAULT                        0x0040      
#define SYS_DSPV_AEC_DFTNUMSTAGE_DEFAULT                    0x0004
#define SYS_DSPV_AEC_BLOCKSIZE_DEFAULT                      0x0010      
#define SYS_DSPV_AEC_BLOCKNUM_DEFAULT                       0x000a
#define SYS_DSPV_AEC_ATTNTGT_DEFAULT                        0x0000 

/* Handset values */
#define SYS_DSPV_AEC_FARENDPWRTHES_HANDSET                  0x7B     /*0x0199 */
#define SYS_DSPV_AEC_SPPNGYTHL_HANDSET                      0x0002    /*0x0008*//*0x0002*/                  
#define SYS_DSPV_AEC_NOISETHRES_HANDSET                     0x0055
#define SYS_DSPV_AEC_CHNSNRTHL_HANDSET                      0x0140      
#define SYS_DSPV_AEC_FE2TRTHRES_HANDSET                     0x7eb8                  
#define SYS_DSPV_AEC_TR2NETHRESQ_HANDSET                    0x0002                  
#define SYS_DSPV_AEC_CHANTAPQ_HANDSET                       0x0000      
#define SYS_DSPV_AEC_PARTITION_HANDSET                      0x0002      
#define SYS_DSPV_AEC_OVERLAPRATIO_HANDSET                   0x0003      
#define SYS_DSPV_AEC_DFTSIZE_HANDSET                        0x0040      
#define SYS_DSPV_AEC_DFTNUMSTAGE_HANDSET                    0x0004      
#define SYS_DSPV_AEC_BLOCKSIZE_HANDSET                      0x0010                  
#define SYS_DSPV_AEC_BLOCKNUM_HANDSET                       0x000a                  
#define SYS_DSPV_AEC_ATTNTGT_HANDSET                        0x0000      

/* Handsfree values */
#define SYS_DSPV_AEC_FARENDPWRTHES_HANDSFREE                  0x1000
#define SYS_DSPV_AEC_SPPNGYTHL_HANDSFREE                      0x0008      
#define SYS_DSPV_AEC_NOISETHRES_HANDSFREE                     0x0055      
#define SYS_DSPV_AEC_CHNSNRTHL_HANDSFREE                      0x0140      
#define SYS_DSPV_AEC_FE2TRTHRES_HANDSFREE                     0x7eb8      
#define SYS_DSPV_AEC_TR2NETHRESQ_HANDSFREE                    0x0005      
#define SYS_DSPV_AEC_CHANTAPQ_HANDSFREE                       0x0002      
#define SYS_DSPV_AEC_PARTITION_HANDSFREE                      0x0003      
#define SYS_DSPV_AEC_OVERLAPRATIO_HANDSFREE                   0x0003      
#define SYS_DSPV_AEC_DFTSIZE_HANDSFREE                        0x0080      
#define SYS_DSPV_AEC_DFTNUMSTAGE_HANDSFREE                    0x0005
#define SYS_DSPV_AEC_BLOCKSIZE_HANDSFREE                      0x0020      
#define SYS_DSPV_AEC_BLOCKNUM_HANDSFREE                       0x0005  
#define SYS_DSPV_AEC_ATTNTGT_HANDSFREE                        (int16)0xFFFF

/* Loudspeaker values */
#define SYS_DSPV_AEC_FARENDPWRTHES_LOUDSPKR                  0x1000
#define SYS_DSPV_AEC_SPPNGYTHL_LOUDSPKR                      0x0008      
#define SYS_DSPV_AEC_NOISETHRES_LOUDSPKR                     0x0055      
#define SYS_DSPV_AEC_CHNSNRTHL_LOUDSPKR                      0x0140      
#define SYS_DSPV_AEC_FE2TRTHRES_LOUDSPKR                     0x7eb8      
#define SYS_DSPV_AEC_TR2NETHRESQ_LOUDSPKR                    0x0005      
#define SYS_DSPV_AEC_CHANTAPQ_LOUDSPKR                       0x0002      
#define SYS_DSPV_AEC_PARTITION_LOUDSPKR                      0x0003      
#define SYS_DSPV_AEC_OVERLAPRATIO_LOUDSPKR                   0x0003      
#define SYS_DSPV_AEC_DFTSIZE_LOUDSPKR                        0x0080      
#define SYS_DSPV_AEC_DFTNUMSTAGE_LOUDSPKR                    0x0005
#define SYS_DSPV_AEC_BLOCKSIZE_LOUDSPKR                      0x0020      
#define SYS_DSPV_AEC_BLOCKNUM_LOUDSPKR                       0x0005  
#define SYS_DSPV_AEC_ATTNTGT_LOUDSPKR                        (int16)0xFFFF     

/*-------------------------*/
/* ANS                     */
/*-------------------------*/
/* default configuration defined as IS-127 standard */
#define SYS_DSPV_ANS_VMTHRES_DEFAULT                        0x0023
#define SYS_DSPV_ANS_TCETHRES_DEFAULT                       0x0100
#define SYS_DSPV_ANS_SPECDEVTHRES_DEFAULT                   0x0380
#define SYS_DSPV_ANS_CNTTHRES_DEFAULT                       0x0032
#define SYS_DSPV_ANS_HYSCNTTHRES_DEFAULT                    0x0006
#define SYS_DSPV_ANS_STCKCNTRTHRES_DEFAULT                  0x0300

/*-------------------------*/
/* TTY                     */
/*-------------------------*/
#define SYS_DSPV_TTY_LOGICENGYTH_DEFAULT                          (8191)
#define SYS_DSPV_TTY_SILNCEENGYTH_DEFAULT                         (2)
#define SYS_DSPV_TTY_ONSETWIN_DEFAULT                             (11+(11>>1))
#define SYS_DSPV_TTY_EARLYONSETTHRES_DEFAULT                      (7)
#define SYS_DSPV_TTY_DETCTTHRES_DEFAULT                           (11+5*11+11+(11>>1)-8)
#define SYS_DSPV_TTY_FRMSYNCMEMTH_DEFAULT                         (9)
#define SYS_DSPV_TTY_FRMSYNCSTARTTH_DEFAULT                       (7)
#define SYS_DSPV_TTY_FRMSYNCDATATH_DEFAULT                        (7)
#define SYS_DSPV_TTY_FRMSYNCSTOPTH_DEFAULT                        (8)
#define SYS_DSPV_TTY_FRMSYNCSILNCETH_DEFAULT                      (7)
#define SYS_DSPV_TTY_XMTTIME_DEFAULT                              (11+5*11)
#define SYS_DSPV_TTY_MAXHNGOVRTIME_DEFAULT                        (12*10)
#define SYS_DSPV_TTY_SILNCETH_DEFAULT                             ((9+1)>>2)
#define SYS_DSPV_TTY_TONEGENGAIN_DEFAULT                          (32768>>3)
   
/*-------------------------*/
/* VAP                     */
/*-------------------------*/
#define SYS_DSPV_VAP_VE8_MPP_SAMPOFFSET_DEFAULT                   0x40
#define SYS_DSPV_VAP_VE8_SPP_SAMPOFFSET_DEFAULT                   0x40
#define SYS_DSPV_VAP_VE8_SPP_TASKTIME_DEFAULT                     0x80
#define SYS_DSPV_VAP_V13_MPP_SAMPOFFSET_DEFAULT                   0x40
#define SYS_DSPV_VAP_V13_SPP_SAMPOFFSET_DEFAULT                   0x40
#define SYS_DSPV_VAP_V13_SPP_TASKTIME_DEFAULT                     0x80
#define SYS_DSPV_VAP_CODECINTCTRL_DEFAULT                         0x00
#define SYS_DSPV_AMP_MPP_SAMPOFFSET_DEFAULT                       0x20
#define SYS_DSPV_AMP_SPP_SAMPOFFSET_DEFAULT                       0x00
#define SYS_DSPV_AMP_SPP_TASKTIME_DEFAULT                         0x60 

/*-------------------------*/
/* MPP FIF                 */
/*-------------------------*/
#define SYS_DSPV_MPP_FIF_BQIIRMEM_N_DEFAULT                    3
#define SYS_DSPV_MPP_FIF_COEF_0_DEFAULT                     8240
#define SYS_DSPV_MPP_FIF_COEF_1_DEFAULT                   -16432
#define SYS_DSPV_MPP_FIF_COEF_2_DEFAULT                     8192
#define SYS_DSPV_MPP_FIF_COEF_3_DEFAULT                     5859
#define SYS_DSPV_MPP_FIF_COEF_4_DEFAULT                   -13841
#define SYS_DSPV_MPP_FIF_COEF_5_DEFAULT                     8192
#define SYS_DSPV_MPP_FIF_COEF_6_DEFAULT                     8192
#define SYS_DSPV_MPP_FIF_COEF_7_DEFAULT                   -16384
#define SYS_DSPV_MPP_FIF_COEF_8_DEFAULT                     8192
#define SYS_DSPV_MPP_FIF_COEF_9_DEFAULT                     6416
#define SYS_DSPV_MPP_FIF_COEF_10_DEFAULT                  -14391
#define SYS_DSPV_MPP_FIF_COEF_11_DEFAULT                    8192
#define SYS_DSPV_MPP_FIF_COEF_12_DEFAULT                    5829
#define SYS_DSPV_MPP_FIF_COEF_13_DEFAULT                  -11693
#define SYS_DSPV_MPP_FIF_COEF_14_DEFAULT                    5864
#define SYS_DSPV_MPP_FIF_COEF_15_DEFAULT                    7494
#define SYS_DSPV_MPP_FIF_COEF_16_DEFAULT                  -15452
#define SYS_DSPV_MPP_FIF_COEF_17_DEFAULT                    8192


/*-------------------------*/
/* MPP BIF                 */
/*-------------------------*/
/* Default                 */
#define SYS_DSPV_MPP_BIF_BQIIRMEM_N_DEFAULT                    3
#define SYS_DSPV_MPP_BIF_COEF_0_DEFAULT                       838
#define SYS_DSPV_MPP_BIF_COEF_1_DEFAULT                      7451
#define SYS_DSPV_MPP_BIF_COEF_2_DEFAULT                      8192
#define SYS_DSPV_MPP_BIF_COEF_3_DEFAULT                     -2184
#define SYS_DSPV_MPP_BIF_COEF_4_DEFAULT                      3445
#define SYS_DSPV_MPP_BIF_COEF_5_DEFAULT                      8192
#define SYS_DSPV_MPP_BIF_COEF_6_DEFAULT                      2439
#define SYS_DSPV_MPP_BIF_COEF_7_DEFAULT                     -9322
#define SYS_DSPV_MPP_BIF_COEF_8_DEFAULT                      8192
#define SYS_DSPV_MPP_BIF_COEF_9_DEFAULT                       849
#define SYS_DSPV_MPP_BIF_COEF_10_DEFAULT                    -4694
#define SYS_DSPV_MPP_BIF_COEF_11_DEFAULT                     8192
#define SYS_DSPV_MPP_BIF_COEF_12_DEFAULT                     4986
#define SYS_DSPV_MPP_BIF_COEF_13_DEFAULT                    -8998
#define SYS_DSPV_MPP_BIF_COEF_14_DEFAULT                     8079
#define SYS_DSPV_MPP_BIF_COEF_15_DEFAULT                     4838
#define SYS_DSPV_MPP_BIF_COEF_16_DEFAULT                    -8833
#define SYS_DSPV_MPP_BIF_COEF_17_DEFAULT                     8192

/* Handset                 */
#define SYS_DSPV_MPP_BIF_BQIIRMEM_N_HANDSET                      3
#define SYS_DSPV_MPP_BIF_COEF_0_HANDSET                        838
#define SYS_DSPV_MPP_BIF_COEF_1_HANDSET                       7451
#define SYS_DSPV_MPP_BIF_COEF_2_HANDSET                       8192
#define SYS_DSPV_MPP_BIF_COEF_3_HANDSET                      -2184
#define SYS_DSPV_MPP_BIF_COEF_4_HANDSET                       3445
#define SYS_DSPV_MPP_BIF_COEF_5_HANDSET                       8192
#define SYS_DSPV_MPP_BIF_COEF_6_HANDSET                       2439
#define SYS_DSPV_MPP_BIF_COEF_7_HANDSET                      -9322
#define SYS_DSPV_MPP_BIF_COEF_8_HANDSET                       8192
#define SYS_DSPV_MPP_BIF_COEF_9_HANDSET                        849
#define SYS_DSPV_MPP_BIF_COEF_10_HANDSET                     -4694
#define SYS_DSPV_MPP_BIF_COEF_11_HANDSET                      8192
#define SYS_DSPV_MPP_BIF_COEF_12_HANDSET                      4986
#define SYS_DSPV_MPP_BIF_COEF_13_HANDSET                     -8998
#define SYS_DSPV_MPP_BIF_COEF_14_HANDSET                      8079
#define SYS_DSPV_MPP_BIF_COEF_15_HANDSET                      4838
#define SYS_DSPV_MPP_BIF_COEF_16_HANDSET                     -8833
#define SYS_DSPV_MPP_BIF_COEF_17_HANDSET                      8192


/* Headset                 */
#define SYS_DSPV_MPP_BIF_BQIIRMEM_N_HEADSET                     3
#define SYS_DSPV_MPP_BIF_COEF_0_HEADSET                       838
#define SYS_DSPV_MPP_BIF_COEF_1_HEADSET                      7451
#define SYS_DSPV_MPP_BIF_COEF_2_HEADSET                      8192
#define SYS_DSPV_MPP_BIF_COEF_3_HEADSET                     -2184
#define SYS_DSPV_MPP_BIF_COEF_4_HEADSET                      3445
#define SYS_DSPV_MPP_BIF_COEF_5_HEADSET                      8192
#define SYS_DSPV_MPP_BIF_COEF_6_HEADSET                      2439
#define SYS_DSPV_MPP_BIF_COEF_7_HEADSET                     -9322
#define SYS_DSPV_MPP_BIF_COEF_8_HEADSET                      8192
#define SYS_DSPV_MPP_BIF_COEF_9_HEADSET                       849
#define SYS_DSPV_MPP_BIF_COEF_10_HEADSET                    -4694
#define SYS_DSPV_MPP_BIF_COEF_11_HEADSET                     8192
#define SYS_DSPV_MPP_BIF_COEF_12_HEADSET                     4986
#define SYS_DSPV_MPP_BIF_COEF_13_HEADSET                    -8998
#define SYS_DSPV_MPP_BIF_COEF_14_HEADSET                     8079
#define SYS_DSPV_MPP_BIF_COEF_15_HEADSET                     4838
#define SYS_DSPV_MPP_BIF_COEF_16_HEADSET                    -8833
#define SYS_DSPV_MPP_BIF_COEF_17_HEADSET                     8192

/* Handsfree               */
#define SYS_DSPV_MPP_BIF_BQIIRMEM_N_HANDSFREE                     3
#define SYS_DSPV_MPP_BIF_COEF_0_HANDSFREE                       838
#define SYS_DSPV_MPP_BIF_COEF_1_HANDSFREE                      7451
#define SYS_DSPV_MPP_BIF_COEF_2_HANDSFREE                      8192
#define SYS_DSPV_MPP_BIF_COEF_3_HANDSFREE                     -2184
#define SYS_DSPV_MPP_BIF_COEF_4_HANDSFREE                      3445
#define SYS_DSPV_MPP_BIF_COEF_5_HANDSFREE                      8192
#define SYS_DSPV_MPP_BIF_COEF_6_HANDSFREE                      2439
#define SYS_DSPV_MPP_BIF_COEF_7_HANDSFREE                     -9322
#define SYS_DSPV_MPP_BIF_COEF_8_HANDSFREE                      8192
#define SYS_DSPV_MPP_BIF_COEF_9_HANDSFREE                       849
#define SYS_DSPV_MPP_BIF_COEF_10_HANDSFREE                    -4694
#define SYS_DSPV_MPP_BIF_COEF_11_HANDSFREE                     8192
#define SYS_DSPV_MPP_BIF_COEF_12_HANDSFREE                     4986
#define SYS_DSPV_MPP_BIF_COEF_13_HANDSFREE                    -8998
#define SYS_DSPV_MPP_BIF_COEF_14_HANDSFREE                     8079
#define SYS_DSPV_MPP_BIF_COEF_15_HANDSFREE                     4838
#define SYS_DSPV_MPP_BIF_COEF_16_HANDSFREE                    -8833
#define SYS_DSPV_MPP_BIF_COEF_17_HANDSFREE                     8192

/* Loudspeaker             */
#define SYS_DSPV_MPP_BIF_BQIIRMEM_N_LOUDSPKR                    3
#define SYS_DSPV_MPP_BIF_COEF_0_LOUDSPKR                    -2771
#define SYS_DSPV_MPP_BIF_COEF_1_LOUDSPKR                     2142
#define SYS_DSPV_MPP_BIF_COEF_2_LOUDSPKR                     8192
#define SYS_DSPV_MPP_BIF_COEF_3_LOUDSPKR                    -1944
#define SYS_DSPV_MPP_BIF_COEF_4_LOUDSPKR                     2025
#define SYS_DSPV_MPP_BIF_COEF_5_LOUDSPKR                     8192
#define SYS_DSPV_MPP_BIF_COEF_6_LOUDSPKR                     3887
#define SYS_DSPV_MPP_BIF_COEF_7_LOUDSPKR                    -6905
#define SYS_DSPV_MPP_BIF_COEF_8_LOUDSPKR                     8192
#define SYS_DSPV_MPP_BIF_COEF_9_LOUDSPKR                     3435
#define SYS_DSPV_MPP_BIF_COEF_10_LOUDSPKR                   -6006
#define SYS_DSPV_MPP_BIF_COEF_11_LOUDSPKR                    8192
#define SYS_DSPV_MPP_BIF_COEF_12_LOUDSPKR                    3505
#define SYS_DSPV_MPP_BIF_COEF_13_LOUDSPKR                    4581
#define SYS_DSPV_MPP_BIF_COEF_14_LOUDSPKR                   10588
#define SYS_DSPV_MPP_BIF_COEF_15_LOUDSPKR                    4047
#define SYS_DSPV_MPP_BIF_COEF_16_LOUDSPKR                    4366
#define SYS_DSPV_MPP_BIF_COEF_17_LOUDSPKR                    8192


/*-------------------------*/
/* SPP FIF                 */
/*-------------------------*/
/* Default                 */
#define SYS_DSPV_SPP_FIF_BQIIRMEM_N_DEFAULT                     3
#define SYS_DSPV_SPP_FIF_COEF_0_DEFAULT                     -4671  
#define SYS_DSPV_SPP_FIF_COEF_1_DEFAULT                      2423  
#define SYS_DSPV_SPP_FIF_COEF_2_DEFAULT                      8192  
#define SYS_DSPV_SPP_FIF_COEF_3_DEFAULT                      3186  
#define SYS_DSPV_SPP_FIF_COEF_4_DEFAULT                      7211  
#define SYS_DSPV_SPP_FIF_COEF_5_DEFAULT                      8192  
#define SYS_DSPV_SPP_FIF_COEF_6_DEFAULT                      5267  
#define SYS_DSPV_SPP_FIF_COEF_7_DEFAULT                    -12287  
#define SYS_DSPV_SPP_FIF_COEF_8_DEFAULT                      8192  
#define SYS_DSPV_SPP_FIF_COEF_9_DEFAULT                      4109  
#define SYS_DSPV_SPP_FIF_COEF_10_DEFAULT                    -6162  
#define SYS_DSPV_SPP_FIF_COEF_11_DEFAULT                     8192  
#define SYS_DSPV_SPP_FIF_COEF_12_DEFAULT                     3224  
#define SYS_DSPV_SPP_FIF_COEF_13_DEFAULT                      -21  
#define SYS_DSPV_SPP_FIF_COEF_14_DEFAULT                     7432  
#define SYS_DSPV_SPP_FIF_COEF_15_DEFAULT                     6973  
#define SYS_DSPV_SPP_FIF_COEF_16_DEFAULT                   -14476  
#define SYS_DSPV_SPP_FIF_COEF_17_DEFAULT                     8192  


/* Handset                 */
#define SYS_DSPV_SPP_FIF_BQIIRMEM_N_HANDSET                     3
#define SYS_DSPV_SPP_FIF_COEF_0_HANDSET                     -4671   
#define SYS_DSPV_SPP_FIF_COEF_1_HANDSET                      2423   
#define SYS_DSPV_SPP_FIF_COEF_2_HANDSET                      8192   
#define SYS_DSPV_SPP_FIF_COEF_3_HANDSET                      3186   
#define SYS_DSPV_SPP_FIF_COEF_4_HANDSET                      7211   
#define SYS_DSPV_SPP_FIF_COEF_5_HANDSET                      8192   
#define SYS_DSPV_SPP_FIF_COEF_6_HANDSET                      5267   
#define SYS_DSPV_SPP_FIF_COEF_7_HANDSET                    -12287   
#define SYS_DSPV_SPP_FIF_COEF_8_HANDSET                      8192   
#define SYS_DSPV_SPP_FIF_COEF_9_HANDSET                      4109   
#define SYS_DSPV_SPP_FIF_COEF_10_HANDSET                    -6162   
#define SYS_DSPV_SPP_FIF_COEF_11_HANDSET                     8192   
#define SYS_DSPV_SPP_FIF_COEF_12_HANDSET                     3224   
#define SYS_DSPV_SPP_FIF_COEF_13_HANDSET                      -21   
#define SYS_DSPV_SPP_FIF_COEF_14_HANDSET                     7432   
#define SYS_DSPV_SPP_FIF_COEF_15_HANDSET                     6973   
#define SYS_DSPV_SPP_FIF_COEF_16_HANDSET                   -14476   
#define SYS_DSPV_SPP_FIF_COEF_17_HANDSET                     8192

/* Headset                 */
#define SYS_DSPV_SPP_FIF_BQIIRMEM_N_HEADSET                     3
#define SYS_DSPV_SPP_FIF_COEF_0_HEADSET                     -6874
#define SYS_DSPV_SPP_FIF_COEF_1_HEADSET                     -1301
#define SYS_DSPV_SPP_FIF_COEF_2_HEADSET                      8192
#define SYS_DSPV_SPP_FIF_COEF_3_HEADSET                     -2865
#define SYS_DSPV_SPP_FIF_COEF_4_HEADSET                      -153
#define SYS_DSPV_SPP_FIF_COEF_5_HEADSET                      8192
#define SYS_DSPV_SPP_FIF_COEF_6_HEADSET                      1916
#define SYS_DSPV_SPP_FIF_COEF_7_HEADSET                     -5108
#define SYS_DSPV_SPP_FIF_COEF_8_HEADSET                      8192
#define SYS_DSPV_SPP_FIF_COEF_9_HEADSET                      1169
#define SYS_DSPV_SPP_FIF_COEF_10_HEADSET                      713
#define SYS_DSPV_SPP_FIF_COEF_11_HEADSET                     8192
#define SYS_DSPV_SPP_FIF_COEF_12_HEADSET                     1053
#define SYS_DSPV_SPP_FIF_COEF_13_HEADSET                     2393
#define SYS_DSPV_SPP_FIF_COEF_14_HEADSET                    14344
#define SYS_DSPV_SPP_FIF_COEF_15_HEADSET                     2309
#define SYS_DSPV_SPP_FIF_COEF_16_HEADSET                    -5583
#define SYS_DSPV_SPP_FIF_COEF_17_HEADSET                     8192

/* Handsfree               */
#define SYS_DSPV_SPP_FIF_BQIIRMEM_N_HANDSFREE                   3
#define SYS_DSPV_SPP_FIF_COEF_0_HANDSFREE                   -4671
#define SYS_DSPV_SPP_FIF_COEF_1_HANDSFREE                    2423
#define SYS_DSPV_SPP_FIF_COEF_2_HANDSFREE                    8192
#define SYS_DSPV_SPP_FIF_COEF_3_HANDSFREE                    3186
#define SYS_DSPV_SPP_FIF_COEF_4_HANDSFREE                    7211
#define SYS_DSPV_SPP_FIF_COEF_5_HANDSFREE                    8192
#define SYS_DSPV_SPP_FIF_COEF_6_HANDSFREE                    5267
#define SYS_DSPV_SPP_FIF_COEF_7_HANDSFREE                  -12287
#define SYS_DSPV_SPP_FIF_COEF_8_HANDSFREE                    8192
#define SYS_DSPV_SPP_FIF_COEF_9_HANDSFREE                    4109
#define SYS_DSPV_SPP_FIF_COEF_10_HANDSFREE                  -6162
#define SYS_DSPV_SPP_FIF_COEF_11_HANDSFREE                   8192
#define SYS_DSPV_SPP_FIF_COEF_12_HANDSFREE                   3224
#define SYS_DSPV_SPP_FIF_COEF_13_HANDSFREE                    -21
#define SYS_DSPV_SPP_FIF_COEF_14_HANDSFREE                   7432
#define SYS_DSPV_SPP_FIF_COEF_15_HANDSFREE                   6973
#define SYS_DSPV_SPP_FIF_COEF_16_HANDSFREE                 -14476
#define SYS_DSPV_SPP_FIF_COEF_17_HANDSFREE                   8192

/* Loudspeaker             */
#define SYS_DSPV_SPP_FIF_BQIIRMEM_N_LOUDSPKR                    3
#define SYS_DSPV_SPP_FIF_COEF_0_LOUDSPKR                     -655
#define SYS_DSPV_SPP_FIF_COEF_1_LOUDSPKR                    -5452
#define SYS_DSPV_SPP_FIF_COEF_2_LOUDSPKR                     8192
#define SYS_DSPV_SPP_FIF_COEF_3_LOUDSPKR                     -435
#define SYS_DSPV_SPP_FIF_COEF_4_LOUDSPKR                    -1578
#define SYS_DSPV_SPP_FIF_COEF_5_LOUDSPKR                     8192
#define SYS_DSPV_SPP_FIF_COEF_6_LOUDSPKR                     3554
#define SYS_DSPV_SPP_FIF_COEF_7_LOUDSPKR                     9342
#define SYS_DSPV_SPP_FIF_COEF_8_LOUDSPKR                     8192
#define SYS_DSPV_SPP_FIF_COEF_9_LOUDSPKR                     3616
#define SYS_DSPV_SPP_FIF_COEF_10_LOUDSPKR                    9491
#define SYS_DSPV_SPP_FIF_COEF_11_LOUDSPKR                    8192
#define SYS_DSPV_SPP_FIF_COEF_12_LOUDSPKR                    5006
#define SYS_DSPV_SPP_FIF_COEF_13_LOUDSPKR                   -9028
#define SYS_DSPV_SPP_FIF_COEF_14_LOUDSPKR                    9049
#define SYS_DSPV_SPP_FIF_COEF_15_LOUDSPKR                    4703
#define SYS_DSPV_SPP_FIF_COEF_16_LOUDSPKR                   -8438
#define SYS_DSPV_SPP_FIF_COEF_17_LOUDSPKR                    8192

/*-------------------------*/
/* Ringer Default Config   */
/*-------------------------*/
#define SYS_DSPV_RINGER_BLOCKSZMS_DEFAULT                    2 
#define SYS_DSPV_RINGER_RAMPTIME_DEFAULT                    20 
#define SYS_DSPV_RINGER_CONTRLFIELD_DEFAULT                  1   

/*-------------------------*/
/* AMPS  Rev Ch            */
/*-------------------------*/
/* AMPS Reverse Channel Configuration Parameter Defaults */
#define SYS_DSPV_AMP_REVCH_CISCALE_DEFAULT                        68
#define SYS_DSPV_AMP_REVCH_CISCALE_690_TEST                       52        /* 690 test case */
#define SYS_DSPV_AMP_REVCH_PISCALE_DEFAULT                        402
#define SYS_DSPV_AMP_REVCH_STSCALE_DEFAULT                        15400
#define SYS_DSPV_AMP_REVCH_SATSCALE_DEFAULT                       57
#define SYS_DSPV_AMP_REVCH_DEVLIMFLTRSCALE_DEFAULT                520
#define SYS_DSPV_AMP_REVCH_DEVLIMTHRESH_DEFAULT                   32767
#define SYS_DSPV_AMP_REVCH_WBD_BAL_ADJ_NON_DCR_DEFAULT            0
#define SYS_DSPV_AMP_REVCH_WBD_BAL_ADJ_DCR_DEFAULT                0xFFB0

/*-------------------------*/
/* AMPS  Fwd Ch            */
/*-------------------------*/
/* AMPS Forward Channel Configuration Parameter Defaults */
#define SYS_DSPV_AMP_FWDCH_DISCRIMINATORSCALE_DEFAULT             2816
#define SYS_DSPV_AMP_FWDCH_EOSCALE_DEFAULT                        656       /* 690 test case */
#define SYS_DSPV_AMP_FWDCH_EOSCALE_690_TEST                       400
#define SYS_DSPV_AMP_FWDCH_WBDINPSCALE_DEFAULT                    64
#define SYS_DSPV_AMP_FWDCH_FOCCDOTWSTHD_DEFAULT                   2560
#define SYS_DSPV_AMP_FWDCH_FOCCDOTWSTHDADJ_DEFAULT                576
#define SYS_DSPV_AMP_FWDCH_FVCDOTTHD_DEFAULT                      3200
#define SYS_DSPV_AMP_FWDCH_FVCDOTCNTRINC_DEFAULT                  8
#define SYS_DSPV_AMP_FWDCH_FVCDOTCNTRDEC_DEFAULT                  2
#define SYS_DSPV_AMP_FWDCH_FVCDOTCNTRTHD_DEFAULT                  32
#define SYS_DSPV_AMP_FWDCH_SATISCALE_DEFAULT                      10
#define SYS_DSPV_AMP_FWDCH_AFCTARGET_DEFAULT                      0x30
#define SYS_DSPV_AMP_FWDCH_VOICEAFCCN_DEFAULT                     6
#define SYS_DSPV_AMP_FWDCH_FOCCAFCCN_DEFAULT                      63
#define SYS_DSPV_AMP_FWDCH_RXAGCTARGETLO_DEFAULT                  3584
#define SYS_DSPV_AMP_FWDCH_RXAGCTARGETME_DEFAULT                  4864
#define SYS_DSPV_AMP_FWDCH_RXAGCCORRECTFCTR_DEFAULT               320
#define SYS_DSPV_AMP_FWDCH_RXAGCAVERRATE_DEFAULT                  4
#define SYS_DSPV_AMP_FWDCH_VOICEAFCSLOPE_DEFAULT                  0x0629
#define SYS_DSPV_AMP_FWDCH_FOCCAFCSLOPE_DEFAULT                   0x0821
#define SYS_DSPV_AMP_FWDCH_DCTHRESHOLD_DEFAULT                    0x100
#define SYS_DSPV_AMP_FWDCH_DCARCLOOPGAIN_DEFAULT                  0x1A0
#define SYS_DSPV_AMP_FWDCH_SATDETECTSCALE_DEFAULT                 0x1200

#define SYS_DSPV_AMP_FWDCH_PWRCHANGEHYSTMED2_DCR                  45
#define SYS_DSPV_AMP_FWDCH_PWRCHANGEHYSTHI_DCR                    75
#define SYS_DSPV_AMP_FWDCH_PWRCHANGEHYSTMED2_DEFAULT              0x7C
#define SYS_DSPV_AMP_FWDCH_PWRCHANGEHYSTHI_DEFAULT                0x17C
#define SYS_DSPV_AMP_FWDCH_AFCOFFPWRTHRESH_DEFAULT                0x1180
#define SYS_DSPV_AMP_FWDCH_AFCCNTRLGAIN_DEFAULT                   0xc180
#define SYS_DSPV_AMP_FWDCH_FOCCLOSTSYNCCNTRTHD_DEFAULT            10
#define SYS_DSPV_AMP_FWDCH_FOCCGOODFRMTHD_DEFAULT                 30
#define SYS_DSPV_AMP_FWDCH_IDLEDSPUPTIME_DEFAULT                  10
#define SYS_DSPV_AMP_FWDCH_IDLERFFASTUPTIME_DEFAULT               2
#define SYS_DSPV_AMP_FWDCH_IDLERFSLOWUPTIME_DEFAULT               120
#define SYS_DSPV_AMP_FWDCH_FVCSYNCDETLIM_DEFAULT                  1600
#define SYS_DSPV_AMP_FWDCH_BCHERRPAT_DEFAULT                      0x00000112
#define SYS_DSPV_AMP_FWDCH_CENTERFREQ_DEFAULT                     835
#define SYS_DSPV_AMP_FWDCH_PDMPERUNITQ_DEFAULT                    8
#define SYS_DSPV_AMP_FWDCH_FOCCSLOWRFSLPNUM_DEFAULT               3
#define SYS_DSPV_AMP_FWDCH_SAT_WEIGHT_DEFAULT                     66
#define SYS_DSPV_AMP_FWDCH_EXP_ADJ_THRESH_DEFAULT                 0x1750
#define SYS_DSPV_AMP_FWDCH_EXP_ADJ_FCTR_DEFAULT                   0x4800
#define SYS_DSPV_AMP_FWDCH_AVER_ON_THRESH_DEFAULT                 24
#define SYS_DSPV_AMP_FWDCH_MIN_PDM_BAL_DEFAULT                    0x000

#define SYS_DSPV_AMP_DC_COMP_NOMALIZE_SHIFT_DEFAULT               24
#define SYS_DSPV_AMP_RXAGC_SLOPE_Q_ADJ_DEFAULT                    -14
#define SYS_DSPV_AMP_RXAGC_SLOPE_Q_ADJ_DCR                        -13
#define SYS_DSPV_AMP_DC_OFFSET_REPORT_MODE_DEFAULT                0
#define SYS_DSPV_AMP_DC_OFFSET_REPORT_MODE_DCR                    1
#define SYS_DSPV_AMP_DC_OFFSET_THRESH1_DEFAULT                    400
#define SYS_DSPV_AMP_DC_OFFSET_THRESH2_DEFAULT                    400
#define SYS_DSPV_AMP_DC_CALIB_SETTLE_FRM_CNT_DEFAULT              8



#endif
