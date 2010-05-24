#ifndef _HWDMS_H_
#define _HWDMS_H_


/*----------------------------------------------------------------------------
 Global Typedefs
----------------------------------------------------------------------------*/
typedef enum
{ 
    PDM_NUMBER0 = 0,
    PDM_NUMBER1 = 1, 
    PDM_NUMBER2 = 2, 
    PDM_NUMBER3 = 3, 
    PDM_NUMBER4 = 4,
    PDM_MAX_NUM_SUPPORTED = 5
}PdmNumberT;

/* The following target Rx ADC Vrms input level identifiers are use to
** configure the Rx SD digital gain and A1/A2 select bits. */
typedef enum
{
    RX_ADC_INPUT_TARGET_8MV_RMS = 0, /* Nominal */
    RX_ADC_INPUT_TARGET_6MV_RMS = 1,
    RX_ADC_INPUT_AUTO_ADJUSTED  = 2  /* Rx ADC input level managed by DSPM (i.e. digital Rx AGC) */
} HwdRxAdcInputLevelT;

/* The following Rx DC bias rate identifiers are used to configure various
** DC Bias decimation parameters used by the DSPM */
typedef enum
{
    RX_ADC_NORMAL_DC_BIAS = 0,
    RX_ADC_FAST_DC_BIAS   = 1
} HwdRxAdcDcBiasRateT;

/* The following Rx ADC Power level identifiers are used to configure the
** power consumption in the Rx Stage1 (2nd order) and Rx Stage2 (4th order)
** filters */
typedef enum
{
    RX_ADC_PWR_QUARTER_LESS_THAN_NOMINAL = 0,
    RX_ADC_PWR_EIGHTH_LESS_THAN_NOMINAL  = 1,
    RX_ADC_PWR_NOMINAL                   = 2,
    RX_ADC_PWR_EIGHTH_MORE_THAN_NOMINAL  = 3
} HwdRxAdcStagePwrLvlT;

/* Common Voltage Source configuration options */
typedef enum
{
    TX_COMMON_MODE_VOLTAGE_EXTERNAL = 0,
    TX_COMMON_MODE_VOLTAGE_INTERNAL = 1
} HwdTxCmvSourceT;    

/* The following gain settings roughly conform (i.e. rounded) to the register
** settings used by the CF_TXSD_COARSE_GAIN_SEL hardware register */
typedef enum
{
    TX_IQ_250MV_VPTPD_SETTING  = 0,
    TX_IQ_500MV_VPTPD_SETTING  = 1,
    TX_IQ_750MV_VPTPD_SETTING  = 2,
    TX_IQ_1000MV_VPTPD_SETTING = 3,
    TX_IQ_2000MV_VPTPD_SETTING = 4
} HwdTxIQLevelT;

/* The following gain settings are used to identify a target Tx clipper
** setting which are subsequently used to configure a set of variables
** sent the the DSPM */
typedef enum
{
    TX_CLIPPER_NOMINAL_SETTING = 0,
    TX_CLIPPER_DISABLED        = 1
} HwdTxClipperLevelT;

/* The following data structure is used to configure RF-Dependent MXS variables */
typedef struct
{
   /* Target Rx ADC Vrms variables */
   uint16 RxSdBitSelect;      /* CF_RXSD_BITSEL_CDMA register setting */
   int16  RxSdA1_I;           /* CF_RXSD_GBA1_I register setting */
   int16  RxSdA2_I;           /* CF_RXSD_GBA2_I register setting */
   int16  RxSdA1_Q;           /* CF_RXSD_GBA1_Q register setting */
   int16  RxSdA2_Q;           /* CF_RXSD_GBA2_Q register setting */
   uint16 RxSdDigiGain_I;     /* CF_RXSD_BITSEL_DIGI_GAIN_I register setting */
   uint16 RxSdDigiGain_Q;     /* CF_RXSD_BITSEL_DIGI_GAIN_Q register setting */

   /* Rx ADC Power consumption setting */
   uint16 RxSdCurSw;          /* CF_RXSD_CURSW register setting */

   /* Tx Common Mode Voltage source */
   uint16 TxCmvSource;        /* CF_TXSD_CMVSEL register setting */

   /* Tx I/Q Vptp gain select */
   uint16 TxGainSelect;       /* CF_TXSD_COARSE_GAIN_SEL register setting */
   uint16 TxTrimSetting;      /* CF_TXSD_GDIQ register setting */
   uint16 TxGainCompA1;       /* DSPM TX_GAIN_COMP1 register setting */
   uint16 TxGainCompA2;       /* DSPM TX_GAIN_COMP2 register setting */
   uint16 TxGainSelectDspm;   /* DSPM TX_GAIN_SEL register (if Tx SD configured for DSPM control) */

   /* Tx DAC Clipper settings */
   uint16 TxClipperLevel;     /* Tx (reverse) Clipper level */
   uint16 TxClipperThresh;    /* Tx (reverse) Clipper threshold */
   uint16 TxClipperMaxSigmaX; /* Tx (reverse) Clipper Max allowed Sigma X (input RMS value) */
   uint16 TxClipperSigmaP;    /* Tx (reverse) Clipper Sigma P (pilot RMS value) */

   /* Phase Equalizer setting */
   uint16 PhaseEqualizerCtrl; /* CF_RXSD_PHASE_EQ register setting */

} HwdMxsConstantsT;

/*----------------------------------------------------------------------------
 Global Data
----------------------------------------------------------------------------*/

extern uint32  PdmCtrlAddressTable[PDM_MAX_NUM_SUPPORTED];
extern uint32  PdmDoutAddressTable[PDM_MAX_NUM_SUPPORTED];
extern uint32  PdmDinAddressTable[PDM_MAX_NUM_SUPPORTED];

extern HwdMxsConstantsT HwdMxsConstants;

/*----------------------------------------------------------------------------
 Global Defines 
----------------------------------------------------------------------------*/

/* PDM assignment */
/* NOTE: This one definition used to be defined in sysparm.h,
         but that file was cleaned up and it was removed
         in the process.  It is now defined here until the
         transition from 4.0 to 4.05 is complete */
#define  SYS_PDM_TX_AGC_BN          2
#define  SYS_PDM_RX_AGC_BN          3

/* Processor control select */
#define HWD_MS_CP_SELECT        0x00
#define HWD_MS_DM_SELECT        0x01
#define HWD_MS_DV_SELECT        0x02

#define HWD_MS_RXSD_REGSEL      ((HWD_MS_DM_SELECT<<2) | HWD_MS_CP_SELECT)
#define HWD_MS_TX_REGSEL        HWD_MS_CP_SELECT

/* in RAM has to have the same definition */
#define HWD_MS_PDM0_REGSEL      HWD_MS_CP_SELECT
#define HWD_MS_PDM1_REGSEL      HWD_MS_DM_SELECT
#define HWD_MS_PDM2_REGSEL      HWD_MS_DM_SELECT
#ifdef SYS_OPTION_DIGITAL_RX_AGC
#define HWD_MS_PDM3_REGSEL      HWD_MS_CP_SELECT
#else
#define HWD_MS_PDM3_REGSEL      HWD_MS_DM_SELECT
#endif
#define HWD_MS_PDM4_REGSEL      HWD_MS_CP_SELECT

#define HWD_MS_AMPS_PDM0_REGSEL     HWD_MS_CP_SELECT
#define HWD_MS_AMPS_PDM1_REGSEL     HWD_MS_DV_SELECT
#define HWD_MS_AMPS_PDM2_REGSEL     HWD_MS_CP_SELECT
#define HWD_MS_AMPS_PDM3_REGSEL     HWD_MS_DV_SELECT
#define HWD_MS_AMPS_PDM4_REGSEL     HWD_MS_CP_SELECT

/*- - - - - - - - - - - - - - - - -*/
/* Define mix signal configuration */
/*- - - - - - - - - - - - - - - - -*/

#define HWD_TX_DAC_NOT_GATED_PWR_CTRL   0
#define HWD_TX_DAC_GATED_PWR_CTRL       1
#define HWD_TX_DAC_PWR_CTRL             HWD_TX_DAC_GATED_PWR_CTRL

/* Voice ADC/DAC constants */
#define HWD_POWER_DOWN              0x01
#define HWD_POWER_UP                0x00
#define HWD_RX_2X2_DISABLED         0x02
#define HWD_RX_2X2_ENABLED          0x00
#define HWD_RX_2X2_USED             HWD_RX_2X2_ENABLED
/* HWD_RXSD_ADCPD (0x0B880000) */
#define RXSD_ADCPD                   0x0001
#define RXSD_ADCPD2                  0x0002
/* HWD_RXSD_CRSW (0x0B880010) */
#define RXSD_STAGE1_PWR_QUARTER_LESS 0x0000 /* Stage 1 current consumption 25% below nominal */
#define RXSD_STAGE1_PWR_EIGHTH_LESS  0x0001 /* Stage 1 current consumption 12.5% below nominal */
#define RXSD_STAGE1_PWR_NOMINAL      0x0002 /* Stage 1 current consumption at nominal */
#define RXSD_STAGE1_PWR_EIGHTH_MORE  0x0003 /* Stage 1 current consumption 12.5% above nominal */
#define RXSD_STAGE2_PWR_QUARTER_LESS 0x0000 /* Stage 2 current consumption 25% below nominal */
#define RXSD_STAGE2_PWR_EIGHTH_LESS  0x0004 /* Stage 2 current consumption 12.5% below nominal */
#define RXSD_STAGE2_PWR_NOMINAL      0x0008 /* Stage 2 current consumption at nominal */
#define RXSD_STAGE2_PWR_EIGHTH_MORE  0x000C /* Stage 2 current consumption 12.5% above nominal */
/* HWD_RXSD_SEL (0x0B880050) */
#define HWD_RX_SEL_REGISTER         0x00
#define HWD_RX_SEL_UNREGISTER       0x02
/* HWD_RXSD_ENABLES  (0x0B880078) */
#define HWD_MODE_CDMA               0x00
#define HWD_MODE_AMPS               0x01
#define HWD_SINGLE_LOOP_ENABLE      (1<<1)  /* automaticly disables 2x2 */
#define HWD_SINGLE_LOOP_DISABLE     (0<<1)  /* automaticly enables 2x2 */
#define HWD_STAGE2_ENABLE           (1<<2)
#define HWD_STAGE2_DISABLE          (0<<2)
#define HWD_SWAP_ENABLE             (1<<3)
#define HWD_SWAP_DISABLE            (0<<3)
#define HWD_RX_ENABLES_CDMA         (HWD_MODE_CDMA | HWD_SINGLE_LOOP_DISABLE | HWD_STAGE2_DISABLE | HWD_SWAP_DISABLE)
#define HWD_RX_ENABLES_AMPS         (HWD_MODE_AMPS | HWD_SINGLE_LOOP_DISABLE | HWD_STAGE2_DISABLE | HWD_SWAP_DISABLE)

/* HWD_TXSD_OPER (0x0B88010C) */
#define HWD_TX_AUX_DISABLE          0x01 /* Tx sigma-delta Aux ADC isolation mux disabled */
#define HWD_TX_TUNE_PD              0x02 /* Tx filter calibration, tune oscillator circuit is powered down */
#define HWD_TX_NORMAL_OPER          (HWD_TX_AUX_DISABLE | HWD_TX_TUNE_PD)
/* HWD_TXSD_CMVSEL (0x0b880118) */
#define HWD_TX_CMV_INT_SOURCE       0x01
#define HWD_TX_CMV_EXT_SOURCE       0x00
/* HWD_TXSD_GAIN_SEL (0x0b880164) */
#define HWD_TX_025_VPTOPD           0x00
#define HWD_TX_050_VPTOPD           0x01
#define HWD_TX_100_VPTOPD           0x02
#define HWD_TX_200_VPTOPD           0x03
#define HWD_TX_GAIN_SEL_AMPS        HWD_TX_025_VPTOPD
#define HWD_TX_GAIN_SEL_CDMA        HWD_TX_050_VPTOPD
/* HWD_TXSD_GDIQ (0x0b880150) */
#define HWD_TX_SD_DAC_TRIM_IQ       0x88 /* Tx SD DAC nominal output setting */
/* HWD_TXSD_TUNE (0x0b880158) */
#define HWD_TX_SD_TUNE_DEFAULT          0x0e

/* HWD_CF_AA_SCALE (0x0b880310) */
#define HWD_AA_SCALE_VALUE              0x01

/* HWD_CF_RESERVED Bit definitions */
#define HWD_CF_VC1_SEL_DV               (1<<0)
#define HWD_CF_RXSD_DIS                 (3<<1)
#define HWD_CF_TXSD_DIS                 (1<<3)
#if ((SYS_ASIC == SA_RAM) && (SYS_VERSION <= SV_REV_C3))
#define HWD_CF_PDM1_DIS                 (1<<4)
#define HWD_CF_BYPASS_HPADC             (1<<5)
#define HWD_CF_BYPASS_HPDAC             (1<<6)
#define HWD_CF_AA_BYPASS                (3<<7)
#define HWD_CF_AA_BUF0_SEL              (1<<7)
#define HWD_CF_AA_BUF1_SEL              (2<<7)
#define HWD_CF_USB_MIX_0                (1<<9)
#define HWD_CF_USB_MIX_1                (1<<10)
#define HWD_CF_USB_MIX_2                (1<<11)
#define HWD_CF_USB_MIX_3                (1<<12)
#define HWD_CF_USB_MIX_4                (1<<13)
#define HWD_CF_PDM2_DIS                 (1<<14)
#define HWD_CF_PDM3_DIS                 (1<<15)
#else
#define HWD_CF_PDM0_DIS                 (1<<4)
#define HWD_CF_PDM1_DIS                 (1<<5)
#define HWD_CF_PDM2_DIS                 (1<<6)
#define HWD_CF_PDM3_DIS                 (1<<7)
#define HWD_CF_PDM4_DIS                 (1<<8)
#define HWD_CF_USB_MIX_0                (1<<9)
#define HWD_CF_USB_MIX_1                (1<<10)
#define HWD_CF_USB_MIX_2                (1<<11)
#define HWD_CF_USB_MIX_3                (1<<12)
#define HWD_CF_USB_MIX_4                (1<<13)
#define HWD_CF_VC_DIS                   (1<<14)
#define HWD_CF_VC1_DIS                  (1<<15)
#endif

#define HWD_CF_PDMs_DIS                 (HWD_CF_PDM1_DIS | HWD_CF_PDM2_DIS | HWD_CF_PDM3_DIS)
#define HWD_CF_USB_MIX_ALL              \
(HWD_CF_USB_MIX_0 | HWD_CF_USB_MIX_1 | HWD_CF_USB_MIX_2 | HWD_CF_USB_MIX_3 | HWD_CF_USB_MIX_4)

/*------------------------------------------------------------------------
*  Voice Codec CF_VC_SEL register bit definitions
*------------------------------------------------------------------------*/

/* HWD_CF_VC_ADCPD (0x0b880400) */
#define HWD_VC_ADC_PD_MASK          0x01
#define HWD_VC_ADC_LNA_EN_MASK      0x02

/* HWD_CF_VC_DACPOWER (0x0b880404) */
#define HWD_VC_VDACDIFFSELN         0x01 /* Voice DAC Differential PA enabled  */
#define HWD_VC_VDACVCMSW            0x02 /* Disconnect VDAC and 3 voice PA inputs from Vcm */
#define HWD_VC_VDACUPDOWN           0x04 /* Select the polarity of SE N PA input to be in phase or inverted with SE P  */

/* HWD_CF_VC_SEL (0x0b880414) */
#define HWD_VADC_AUX_SEL            0x01 /* Select voice ADC input from auxiliary(1)/MIC(0) inputs */
#define HWD_VDAC_MD_SEL             0x02 /* "0" 64 ohm differential Voice DAC output low current
                                            "1" 32 ohm differential Voice DAC output high current */

/*------------------------------------------------------------------------
* Mixed signal Voice Codec CF_VC_DIGPOWER register bit definitions 
*------------------------------------------------------------------------*/
/* Bit 0 */
#define HWD_CF_VC_DIGPOWER_VC_DIGPD   0x0001

/* Bit 1 */
#define HWD_CF_VC_DIGPOWER_VDACPD     0x0002


/*------------------------------------------------------------------------
* Mixed signal Voice Codec CF_VC1_POP register bit definitions 
*------------------------------------------------------------------------*/
/* Bit 0 */
#define HWD_CF_VC1_POP_VDACNSESW      0x0001
/* Bit 1 */
#define HWD_CF_VC1_POP_VDACPSESW      0x0002
/* Bit 2 */
#define HWD_CF_VC1_POP_VDACOUTSW      0x0004
/* Bit 3-5 */
#define HWD_CF_VC1_POP_VDACG          0x0038
/* Bit 6 */
#define HWD_CF_VC1_POP_VDACNSESELN    0x0040
/* Bit 7 */
#define HWD_CF_VC1_POP_VDACPSESELN    0x0080

/* Least significant bit position of DAC gain bits in CF_VC1_POP register */ 
#define HWD_DACPGA_LSB_BIT_POS 3

#define HWD_VOICE_DAC_MUTE          1
#define HWD_VOICE_DAC_ENABLE        0
#define HWD_32OHM_DAC_OUT           1
#define HWD_64OHM_DAC_OUT           0
#define HWD_ADC_AUX_INPUT           1
#define HWD_ADC_HANDSET_INPUT       0

#define HWD_VC_DACPD_MASK           0x01
#define HWD_VC_DACAMP1_MASK         0x02
#define HWD_VC_DACAMP2_MASK         0x04

#define HWD_ANALOG_GAIN_0_DB        0x00
#define HWD_ANALOG_GAIN_3_DB        0x01
#define HWD_ANALOG_GAIN_6_DB        0x02
#define HWD_ANALOG_GAIN_9_DB        0x03
#define HWD_ANALOG_GAIN_12_DB       0x04
#define HWD_ANALOG_GAIN_15_DB       0x05
#define HWD_ANALOG_GAIN_18_DB       0x06
#define HWD_ANALOG_GAIN_21_DB       0x07

#define HWD_DIGITAL_GAIN_0_DB       0x00
#define HWD_DIGITAL_GAIN_6_DB       0x01
#define HWD_DIGITAL_GAIN_12_DB      0x02
#define HWD_DIGITAL_GAIN_18_DB      0x03
#define HWD_DIGITAL_GAIN_24_DB      0x04

#define HWD_VC_NORMAL_OPERATION     0

/*- - - - - - - - - - - - - - -	-*
 * Mix signal register interface *
 *- - - - - - - - - - - - - - - -*/
#define HwdMsSetAdcAnalogGain(gain)	\
	HwdWrite(HWD_CF_VC_ADCPGA, gain); 

#define HwdMsSetAdcDigitalGain(gain)	\
	HwdWrite(HWD_CF_VC_ADCRANGE, gain); 

#define HwdMsSetDacAnalogGain(gain)	\
	HwdWrite(HWD_CF_VC1_POP, (HwdRead(HWD_CF_VC1_POP) & ~HWD_CF_VC1_POP_VDACG) | (gain << HWD_DACPGA_LSB_BIT_POS)); 

/* bit = 0 for normal operation, 1 for mixed signal voice codec analog path;
   voice DAC output feeds into voice ADC input. Used for test mode */
#define HwdMsSetVcLoopbackA(bit)	\
	HwdWrite(HWD_CF_VC_LOOPBACKA, bit); 

/* bit = 0 for normal operation, 1 for digital loopback mode */
#define HwdMsSetVcLoopbackD(bit)	\
	HwdWrite(HWD_CF_VC_LOOPBACKD, bit); 

/* value = 00 for normal operation;
           01 for test mode, use external vsinadc input
		   10 for test mode, use output of voice DAC modulator output */
#define HwdMsSetVoiceBpA(value)		HwdWrite(HWD_CF_VC_BPA, value) 

/* bit = 1 selects DAC bypass, 0 for normal operation */
#define HwdMsSetVoiceBpD(bit)       HwdWrite(HWD_CF_VC_BP_SEL, bit); 

/*- - - - - - - -*
 * PDM Interface *
 *- - - - - - - -*/
/* sel = 00 CP control
         01 DSPM control
         10 DSPV control */
#define HwdMsPdmRegSelect(num, sel)	HwdWrite(PdmCtrlAddressTable[num], sel)

/* bit = 0 selects internal PDM generator output as input to analog PDM cell (normal operation)
         1 selects external test input (MXS test mode) */
#define HwdMsSetNormalPdmMode() 	HwdWrite(HWD_PDM_TESTSEL, 0)
#define HwdMsSetTestPdmMode() 	HwdWrite(HWD_PDM_TESTSEL, 1)

/* num - pdm number */
#define HwdMsPdmWrite(num, data)  HwdWrite(PdmDoutAddressTable[num], data)

#define HwdMsPdmPowerDown(num) \
			HwdWrite(HWD_PDM_PWR_DN, HwdRead(HWD_PDM_PWR_DN) | (1 << num))

#define HwdMsPdmPowerUp(num) \
			HwdWrite(HWD_PDM_PWR_DN, HwdRead(HWD_PDM_PWR_DN) & ~(1 << num))

/* Bandgap Interface */
#define HwdMsBgBiasPowerDown(bit)   HwdWrite(HWD_CF_BG_BIASPD, bit)
#define HwdMsBgRefPowerDown(bit)    HwdWrite(HWD_CF_BG_REFPD, bit)
#define HwdMsBgVoltageTrim(bits)    HwdWrite(HWD_CF_BG_VTRIM, bits)

/*----------------------------------------------------------------------------
 Global Function Prototypes
----------------------------------------------------------------------------*/
extern void   HwdMsInit(void);
extern void   HwdMsCalInitCal(void);
extern void   HwdMsCalRxDcBiasEtsParmSet(HwdRxDcBiasParmSetMsgT *MsgDataPtr);
extern void   HwdRxDcBiasCalStart(void);
extern void   HwdRxDcBiasMsgDisable(void);
extern void   HwdMsCalTxDacCal(void);
extern void   HwdMsDcBiasParmSend(void);
extern void   HwdDspmAliveProcessMsg(void);
extern void   HwdDspvAliveProcessMsg(void);
extern void   HwdDspmSchAlgValueConfig(bool UseDefaults, uint16 *ActSetTimer, uint16 *AverageNum, uint16 *PwrRptTimer);
extern void   HwdDspmSchAlgValue1Config(bool UseDefaults, uint16 *SysNumFingers, uint16 *MaxFingerBs, uint16 *SearchTimeMin);
extern void   HwdConfigureDSPM(void);
extern void	  HwdMsCfReservedSet( uint16 BitMask );
extern void	  HwdMsCfReservedClear( uint16 BitMask );
extern void   HwdMsConfigRxAdcTarget(HwdRxAdcInputLevelT Target);
extern void   HwdMsConfigRxDcBias(HwdRxAdcDcBiasRateT Rate, bool ZeroAdj);
extern void   HwdMsConfigRxPowerSetting(HwdRxAdcStagePwrLvlT Setting);
extern void   HwdMsConfigTxCmv(HwdTxCmvSourceT Source);
extern void   HwdMsConfigTxIQ(HwdTxIQLevelT Setting);
extern void   HwdMsConfigTxClipperLevel(HwdTxClipperLevelT Setting);
extern void   HwdMsConfigPhaseEqualizer(bool enable);



#endif
