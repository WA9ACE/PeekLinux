#ifndef _BAL_IPCAPI_H_                    //added by wangran
#define _BAL_IPCAPI_H_


/****************************************************************************
 Include Files
*****************************************************************************/

#include "ipcmbox.h"
#include "sysdefs.h"
#include "sysparm.h"

/*****************************************************************************
 Global Defines and Macros
*****************************************************************************/

/* Q value for DB parms */
#define  IPC_DB_Q                   6

/* DspM initialization type */
#define SYS_INIT_COLD 0   /* initialize on powerup */
#define SYS_INIT_WARM 1   /* initialize after runtime */

#define IPC_DSPM_MBOX_SIZE         1536
#define IPC_DSPV_MBOX_SIZE         512
/*-------------------------------**
** Data Mailbox Sizes And Macros **
**-------------------------------*/

/* Forward channel signaling buffer size in uint16 */
#define IPC_DTA_MBOX_FWD_SIGBUF_SIZE  8

/* Forward channel modem channel buffer sizes in uint16 */
#define IPC_DTA_MBOX_FWD_CH0_SIZE 17
#define IPC_DTA_MBOX_FWD_CH1_SIZE 17
#define IPC_DTA_MBOX_FWD_CH2_SIZE 47
#define IPC_DTA_MBOX_FWD_CH3_SIZE 383

/* Forward channel data mailbox size in uint16 */
#define IPC_DTA_MBOX_FWD_SIZE (IPC_DTA_MBOX_FWD_SIGBUF_SIZE + IPC_DTA_MBOX_FWD_CH0_SIZE + IPC_DTA_MBOX_FWD_CH1_SIZE + IPC_DTA_MBOX_FWD_CH2_SIZE + IPC_DTA_MBOX_FWD_CH3_SIZE)

/* Reverse channel signaling buffer size in uint16 */
#define IPC_DTA_MBOX_REV_SIGBUF_SIZE  5

/* Reverse channel modem channel buffer sizes in uint16 */
#define IPC_DTA_MBOX_REV_CH0_SIZE 17
#define IPC_DTA_MBOX_REV_CH1_SIZE 17
#define IPC_DTA_MBOX_REV_CH2_SIZE 383

/* Reverse channel data mailbox size in uint16 */
#define IPC_DTA_MBOX_REV_SIZE (IPC_DTA_MBOX_REV_SIGBUF_SIZE + IPC_DTA_MBOX_REV_CH0_SIZE + IPC_DTA_MBOX_REV_CH1_SIZE + IPC_DTA_MBOX_REV_CH2_SIZE)

/*-------------------------------------**
** Forward channel data mailbox macros **
**-------------------------------------*/

/* Macro to return the forward channel signaling buffer address */
extern const void *IpcFwdSigBufAddr;
#define IPC_FWD_SIG_BUF_ADDR IpcFwdSigBufAddr;

/*---------------------------------------------------------------**
** Macro to return the modem channel number based on the forward **
** physical channel. The parameter must be of type FwdChnlTypeT  **
**---------------------------------------------------------------*/
extern uint16 IpcFwdPhysChanToMdmChan[];
#define IPC_FWD_PHYS_CHAN_TO_MDM_CHAN(FwdPhysChan) IpcFwdPhysChanToMdmChan[FwdPhysChan];

/*---------------------------------------------------------------**
** Macro to return the modem channel address based on the forward**
** modem channel number. The parameter must be a forward modem   **
** channel number.                                               **
**---------------------------------------------------------------*/
extern void *IpcFwdMdmChanToAddr[];
#define IPC_FWD_MDM_CHAN_TO_ADDR(MdmChan) IpcFwdMdmChanToAddr[MdmChan];

/*-------------------------------------**
** Reverse channel data mailbox macros **
**-------------------------------------*/

/* Macro to return the reverse channel signaling buffer address */
extern const void *IpcRevSigBufAddr;
#define IPC_REV_SIG_BUF_ADDR IpcRevSigBufAddr;

/*---------------------------------------------------------------**
** Macro to return the modem channel number based on the reverse **
** physical channel. The parameter must be of type RChnlTypeT    **
**---------------------------------------------------------------*/
extern uint16 IpcRevPhysChanToMdmChan[];
#define IPC_REV_PHYS_CHAN_TO_MDM_CHAN(RPhysChan) IpcRevPhysChanToMdmChan[RPhysChan];

/*---------------------------------------------------------------**
** Macro to return the modem channel address based on the reverse**
** modem channel number. The parameter must be a reverse modem   **
** channel number.                                               **
**---------------------------------------------------------------*/
extern void *IpcRevMdmChanToAddr[];
#define IPC_REV_MDM_CHAN_TO_ADDR(MdmChan) IpcRevMdmChanToAddr[MdmChan];

/*---------------------------------------------------------------**
** Macro to return the DSPm forward mailbox partition address    **
** The partitions are:                                           **
**  control mailbox partition                                    **
**  dagc mailbox partition                                       **
**  smart mailbox antenna partition                              **
**---------------------------------------------------------------*/
extern void *IpcFwdCtrlMboxAddr[];
#define IPC_FWD_MDM_MBOX_ADDR(MboxPartition) IpcFwdCtrlMboxAddr[MboxPartition];

typedef enum 
{
   IPC_FWD_MDM_CTRL,
   IPC_FWD_MDM_DAGC,
   IPC_FWD_MDM_SMART_ANTENNA
#if ( ((SYS_ASIC == SA_ROM) && (SYS_VERSION >  SV_REV_C3)) || (SYS_ASIC == SA_RAM) || (SYS_ASIC == SA_CBP55) )
   , IPC_FWD_MDM_DCXO,
   IPC_FWD_MDM_TXAGC
#endif
}FwdMboxPartitionTypeT;

/*-----------------------------------**
** Control Mailbox Sizes and Offsets **
**-----------------------------------*/

/* Define mailbox and message header size in uint16 */
#define IPC_MBOX_HEADER_SIZE     2
#define IPC_MSG_HEADER_SIZE      2

/* Size of dspm fwd control mailbox in uint16 */
#define IPC_CTL_MBOX_DSPM_FWD_SIZE    (IPC_DSPM_MBOX_SIZE - IPC_DTA_MBOX_FWD_SIZE - IPC_DTA_MBOX_REV_SIZE - IPC_CTL_MBOX_DSPM_REV_SIZE - IPC_CTL_MBOX_DSPM_FWD_SMART_ANT_SIZE - IPC_CTL_MBOX_DSPM_DAGC_SIZE)

/* Size of dspm fwd smart antenna mailbox in uint16 */
#define IPC_CTL_MBOX_DSPM_FWD_SMART_ANT_SIZE    26

/* Size of dspm fwd dagc mailbox in uint16 */
#if ( ((SYS_ASIC == SA_ROM) && (SYS_VERSION >  SV_REV_C3)) || (SYS_ASIC == SA_RAM) || (SYS_ASIC == SA_CBP55) )
#define IPC_CTL_MBOX_DSPM_DAGC_SIZE       3 
#define IPC_CTL_MBOX_DSPM_DAGC_DCXO_POS   0 
#define IPC_CTL_MBOX_DSPM_DAGC_TXAGC_POS  1 
#define IPC_CTL_MBOX_DSPM_DAGC_RXAGC_POS  2 
#else
#define IPC_CTL_MBOX_DSPM_DAGC_SIZE       1 
#endif

/* Total size of dspm fwd control mailboxes */
#define IPC_CTL_MBOX_DSPM_FWD_TOTAL_SIZE (IPC_CTL_MBOX_DSPM_FWD_SIZE + IPC_CTL_MBOX_DSPM_FWD_SMART_ANT_SIZE + IPC_CTL_MBOX_DSPM_DAGC_SIZE)

/* Size of dspm rev control mailbox in uint16 */
#define IPC_CTL_MBOX_DSPM_REV_SIZE    346

/* Define dspm fwd control mailbox offset */
#define IPC_CTL_MBOX_DSPM_FWD_OFFSET    0
/* Define dspm fwd dagc mailbox offset */
#if ( ((SYS_ASIC == SA_ROM) && (SYS_VERSION >  SV_REV_C3)) || (SYS_ASIC == SA_RAM) || (SYS_ASIC == SA_CBP55) )
#define IPC_CTL_MBOX_DSPM_FWD_DCXO_OFFSET  (IPC_CTL_MBOX_DSPM_FWD_OFFSET + IPC_CTL_MBOX_DSPM_FWD_SIZE + IPC_CTL_MBOX_DSPM_DAGC_DCXO_POS)
#define IPC_CTL_MBOX_DSPM_FWD_TXAGC_OFFSET (IPC_CTL_MBOX_DSPM_FWD_OFFSET + IPC_CTL_MBOX_DSPM_FWD_SIZE + IPC_CTL_MBOX_DSPM_DAGC_TXAGC_POS)
#define IPC_CTL_MBOX_DSPM_FWD_DAGC_OFFSET  (IPC_CTL_MBOX_DSPM_FWD_OFFSET + IPC_CTL_MBOX_DSPM_FWD_SIZE + IPC_CTL_MBOX_DSPM_DAGC_RXAGC_POS)
#else
#define IPC_CTL_MBOX_DSPM_FWD_DAGC_OFFSET (IPC_CTL_MBOX_DSPM_FWD_OFFSET + IPC_CTL_MBOX_DSPM_FWD_SIZE)
#endif
/* Define dspm fwd smart antenna mailbox offset */
#if ( ((SYS_ASIC == SA_ROM) && (SYS_VERSION >  SV_REV_C3)) || (SYS_ASIC == SA_RAM) || (SYS_ASIC == SA_CBP55) )
#define IPC_CTL_MBOX_DSPM_FWD_SMART_ANT_ADDR_OFFSET (IPC_CTL_MBOX_DSPM_FWD_DCXO_OFFSET + IPC_CTL_MBOX_DSPM_DAGC_SIZE)
#else
#define IPC_CTL_MBOX_DSPM_FWD_SMART_ANT_ADDR_OFFSET (IPC_CTL_MBOX_DSPM_FWD_DAGC_OFFSET + IPC_CTL_MBOX_DSPM_DAGC_SIZE)
#endif

/* Size of dspv rev control mailbox in uint16 */
#define IPC_CTL_MBOX_DSPV_REV_SIZE    256

/* Size of dspv rev control mailbox in uint16 */
#define IPC_CTL_MBOX_DSPV_FWD_SIZE    (IPC_DSPV_MBOX_SIZE - IPC_CTL_MBOX_DSPV_REV_SIZE)

/* Maximum dspm fwd control mailbox message size (minus mailbox header and message header sizes) in uint16 */
#define IPC_MSG_DSPM_FWD_MAX_SIZE (IPC_CTL_MBOX_DSPM_FWD_SIZE - IPC_MBOX_HEADER_SIZE  - IPC_MSG_HEADER_SIZE)
/* Maximum dspm rev control mailbox message size (minus mailbox header and message header sizes) in uint16 */
#define IPC_MSG_DSPM_REV_MAX_SIZE (IPC_CTL_MBOX_DSPM_REV_SIZE - IPC_MBOX_HEADER_SIZE  - IPC_MSG_HEADER_SIZE)
/* Maximum dspv fwd control mailbox message size (minus mailbox header and message header sizes) in uint16 */
#define IPC_MSG_DSPV_FWD_MAX_SIZE (IPC_CTL_MBOX_DSPV_FWD_SIZE - IPC_MBOX_HEADER_SIZE  - IPC_MSG_HEADER_SIZE)
/* Maximum dspv rev control mailbox message size (minus mailbox header and message header sizes) in uint16 */
#define IPC_MSG_DSPV_REV_MAX_SIZE (IPC_CTL_MBOX_DSPV_REV_SIZE - IPC_MBOX_HEADER_SIZE  - IPC_MSG_HEADER_SIZE)

/* Define IPC max sizes in uint16 */
#define IPC_MAX_PRINTF_STR_SIZE  11
#define IPC_MAX_PRINTF_ARG_SIZE  5
#define IPC_MAX_TRACE_ARG_SIZE   5

/*-------------------------------------**
** CP to DSP Message Range Information **
**-------------------------------------*/

#define IPC_DSP_MON_MIN_MSG        0x000 /* Minimum message Id associated with the Mon Task */
#define IPC_DSP_MON_MAX_MSG        0x0FF /* Maximum message Id associated with the Mon Task */

#define IPC_DSM_FWD_MIN_MSG        0x100 /* Minimum message Id associated with the Forward Task */
#define IPC_DSM_FWD_MAX_MSG        0x1FF /* Maximum message Id associated with the Forward Task */

#define IPC_DSM_REV_MIN_MSG        0x200 /* Minimum message Id associated with the Reverse Task */
#define IPC_DSM_REV_MAX_MSG        0x2FF /* Maximum message Id associated with the Reverse Task */

#define IPC_DSM_RFC_MIN_MSG        0x300 /* Minimum message Id associated with the RF Control Task */
#define IPC_DSM_RFC_MAX_MSG        0x3FF /* Maximum message Id associated with the RF Control Task */

#define IPC_DSM_MSC_MIN_MSG        0x400 /* Minimum message Id associated with the Miscellaneous Task */
#define IPC_DSM_MSC_MAX_MSG        0x47F /* Maximum message Id associated with the Miscellaneous Task */

#define IPC_DSM_HGH_MIN_MSG        0x480 /* Minimum message Id associated with the Miscellaneous Task */
#define IPC_DSM_HGH_MAX_MSG        0x4FF /* Maximum message Id associated with the Miscellaneous Task */

#define IPC_DSM_SCH_MIN_MSG        0x500 /* Minimum message Id associated with the Searcher Task */
#define IPC_DSM_SCH_MAX_MSG        0x5FF /* Maximum message Id associated with the Searcher Task */

#define IPC_DSV_RNGR_MIN_MSG       0x0700   /* Minimum message Id associated with the DSPV */
#define IPC_DSV_RNGR_MAX_MSG       0x077F

#define IPC_DSV_APP_MIN_MSG        0x0780  
#define IPC_DSV_APP_MAX_MSG        0x07FF

#define IPC_DSV_VAP_MPP_MIN_MSG     0x0800  
#define IPC_DSV_VAP_MPP_MAX_MSG     0x08FF

#define IPC_DSV_VAP_SPP_MIN_MSG     0x0900
#define IPC_DSV_VAP_SPP_MAX_MSG     0x09FF

#define IPC_DSV_VAP_AUDIO_MIN_MSG   0x0A00
#define IPC_DSV_VAP_AUDIO_MAX_MSG   0x0AFF

#define IPC_DSV_AMP_REV_CH_MIN_MSG  0x0B00
#define IPC_DSV_AMP_REV_CH_MAX_MSG  0x0BFF

#define IPC_DSV_AMP_FWD_CH_MIN_MSG  0x0C00
#define IPC_DSV_AMP_FWD_CH_MAX_MSG  0x0CFF  /* Maximum message Id associated with the DSPV */

#define IPC_CP_FROM_DSM_MIN_MSG     0x0D00
#define IPC_CP_FROM_DSM_MAX_MSG     0x0DFF

#define IPC_CP_FROM_DSV_MIN_MSG     0x0F00
#define IPC_CP_FROM_DSV_MAX_MSG     0x0FFF

/*****************************************************************************
 Global Typedefs
*****************************************************************************/

/*---------------------------------**
** Control mailbox data structures **
**---------------------------------*/

typedef PACKED struct
{
   uint16       NumWords;              
   uint16       NumCmds;              
} IpcMboxHeaderT;

typedef PACKED struct
{
   uint16       MsgId;              
   uint16       MsgSize;              
} IpcMsgHeaderT;

/*-------------------------**
** Types used in Messages  **
**-------------------------*/

/* IPC Processor ids */
typedef enum
{
   IPC_CP_PROC     = 0,
   IPC_DSPM_PROC,
   IPC_DSPV_PROC
} IpcProcIdT;

/*---------------------------------------------------------------**
** Control Mailbox Message Ids: Control Processor to DSP (_DS*_) **
**---------------------------------------------------------------*/
typedef enum
{
   /* Monitor Task Messages */
   IPC_DSP_DOWN_INIT_MSG                         = IPC_DSP_MON_MIN_MSG,
   IPC_DSP_CODE_MSG,
   IPC_DSP_VER_MSG,
   IPC_DSP_PEEK_MSG,
   IPC_DSP_POKE_MSG,
   IPC_DSP_LOOPBACK_MSG,
   IPC_DSP_MEMTEST_MSG,
   IPC_DSP_SPY_MSG,
   IPC_DSP_TRACE_MSG,
   IPC_DSP_FAULT_CTRL_MSG,
   IPC_DSP_HEART_BEAT_CTRL_MSG,
   IPC_DSP_DBUF_LOOPBACK_MSG,
   IPC_DSP_RESERVED_1,
   IPC_DSP_RESERVED_2,  
   IPC_DSP_BITWISE_OP_MSG, 
   IPC_DSP_CODE_PEEK_MSG,
   IPC_DSP_CODE_POKE_MSG,
   IPC_DSP_CODE_MEMTEST_MSG,
   IPC_DSP_MIXED_SIG_REG_INIT_MSG,
   IPC_DSP_JTAG_PWR_CFG_MSG,
   IPC_DSP_SHARED_MEM_LOOPBACK_MSG,

   /* Forward Task Messages */
   IPC_DSM_FWD_CHNL_CFG_MSG                      = IPC_DSM_FWD_MIN_MSG,
   IPC_DSM_FWD_RDA_H_METRICS_CFG_MSG,
   IPC_DSM_FWD_RDA_P_METRICS_CFG_MSG,
   IPC_DSM_FWD_CHNL_PC_MSG,
   IPC_DSM_FWD_CHNL_SCRAMBLE_MSG,
   IPC_DSM_FWD_FNGR_NOISE_NORM_MSG,  /* This is a Parms ReCfg message sent down at MMI power down */

   /* Reverse Task Messages */
   IPC_DSM_REV_CHNL_CFG_MSG                      = IPC_DSM_REV_MIN_MSG,
   IPC_DSM_REV_PREAMBLE_CFG_MSG,
   IPC_DSM_REV_MIXSIG_PARM_MSG,

   /* RF Control Task Messages */
   IPC_DSM_AFC_PARM_MSG                          = IPC_DSM_RFC_MIN_MSG,
   IPC_DSM_AFC_CAL_PARM_MSG,
   IPC_DSM_AFC_CONFIG_MSG,
   IPC_DSM_DCB_PARM_MSG,
   IPC_DSM_DCB_CONFIG_MSG,
   IPC_DCB_GAINBAL_BITSEL_MSG,
   IPC_DSM_RX_CAL_MEASURE_MSG,
   IPC_DSM_RX_CAL_OVERRIDE_MSG,
   IPC_DSM_RX_AGC_GET_MSG,
   IPC_DSM_RXAGC_CONFIG_MSG,
   IPC_DSM_RX_CAL_PARM_MSG,
   IPC_DSM_AGC_PARM_MSG,
   IPC_DSM_TX_CAL_PARM_MSG,
   IPC_DSM_TXAGC_PARM_MSG,
   IPC_DSM_TXAGC_CAL_PARM_MSG,
   IPC_DSM_TX_AGC_GET_MSG,
   IPC_DSM_TXAGC_CONFIG_MSG,
   IPC_DSM_AFC_GET_PARMS_MSG,
   IPC_DSM_RFON_TABLE,
   IPC_DSM_TXON_TABLE,
   IPC_DSM_RXAGC_SETTLE_TIME,
   IPC_DSM_TXAGC_RELBS_THRSH,
   IPC_DSM_TXAGC_CR_STATUS,
   IPC_DSM_DCB_FAST_DECIM,
   IPC_DSM_RFC_RESERVED,
   IPC_DSM_MAG_SMRT_ANT_MODE_MSG, /* Support  Smart Antenna */
   IPC_DSM_RX_SD_PARMS_CFG,
   IPC_DSM_RX_DAGC_PARMS_CFG,
   IPC_DSM_RX_DAGC_GAIN_STATE,

#if ( ((SYS_ASIC == SA_ROM) && (SYS_VERSION >  SV_REV_C3)) || (SYS_ASIC == SA_RAM) || (SYS_ASIC == SA_CBP55) )
   IPC_DSM_DCB_SW_CNTRL,
   IPC_DSM_RXAGC_SW_CNTRL,
   IPC_DSM_TXAGC_SW_CNTRL,
   IPC_DSM_AFC_SW_CNTRL,
#endif
   /* Miscellaneous Task Messages */
   IPC_DSM_MDM_RST_MSG                           = IPC_DSM_MSC_MIN_MSG,
   IPC_DSM_SYNC_SYS_TIME_MSG,
   IPC_DSM_WAKE_ACQ_SYNC_MSG,
   IPC_DSM_WAKE_STATES_MSG,
   IPC_DSM_FPC_PARM_MSG,
   IPC_DSM_SLOTTED_MODE_MSG,
   IPC_DSM_SIM_MSG,
   IPC_DSM_MSC_PARM_MSG,
   IPC_DSM_OUTERLOOP_REQ_MSG,
   IPC_DSM_FPC_ALG_PARM_MSG,
   IPC_DSM_CAND_FREQ_START_MSG,

   /* High Task Messages */
   IPC_DSM_HGH_RESERVE1_MSG                      = IPC_DSM_HGH_MIN_MSG,

   /* Searcher Task Messages */
   IPC_DSM_PILOT_ACQUIRE_MSG                     = IPC_DSM_SCH_MIN_MSG,
   IPC_DSM_SEARCH_PARM_MSG,
   IPC_DSM_SEARCH_WINDOW_SIZE_MSG,
   IPC_DSM_ACQ_PARM_MSG,
   IPC_DSM_ACTIVE_CAND_PILOT_SET_MSG,
   IPC_DSM_NEIGHBOR_PILOT_SET_MSG,
   IPC_DSM_TEST_ALG_VALUE_MSG,
   IPC_DSM_AFLT_PILOT_SET_MSG, 
   IPC_DSM_AFLT_PILOT_SET_ABORT_MSG,
   IPC_DSM_AFLT_ALG_VALUE_MSG,
   IPC_DSM_QUERY_TIMING_MSG,
   IPC_DSM_TEST_ALG_VALUE1_MSG,
   IPC_DSM_TEST_ALG_VALUE2_MSG,

   /* Ringer Task message */
   IPC_DSV_RNGR_INIT_MSG                         =IPC_DSV_RNGR_MIN_MSG,
   IPC_DSV_RNGR_MODE_MSG,
   IPC_DSV_RNGR_CONFIG_MSG,
#if (SYS_ASIC == SA_CBP55)
   IPC_DSV_RESERVED_30,
   IPC_DSV_RESERVED_31,
#else
   IPC_DSV_SEND_RNGR_VOL_SCAL_FCTR_MSG,
   IPC_DSV_SEND_RNGR_LDSPKR_VOL_SCAL_FCTR_MSG,
#endif


   /* Application Task message */
   IPC_DSV_APP_MODE_MSG                          =IPC_DSV_APP_MIN_MSG,  
   IPC_DSV_APP_CONFIGURE_APPCHANNELS_RSP_MSG,
   IPC_DSV_APP_IMAGE_SIZE_RSP_MSG,
   IPC_DSV_APP_VOL_SCAL_FCTR_MSG,
#if (SYS_ASIC == SA_CBP55)
   IPC_DSV_APP_INFO_QUERY_MSG,
   IPC_DSV_SEND_RNGR_VOL_SCAL_FCTR_MSG,
   IPC_DSV_SEND_RNGR_LDSPKR_VOL_SCAL_FCTR_MSG,
   IPC_DSV_I2S_DELAY_MODE_MSG,
#endif

   /* Microphone Path (encoder) */
   IPC_DSV_SET_MIC_FIF_MODE_MSG                  =IPC_DSV_VAP_MPP_MIN_MSG,
   IPC_DSV_SEND_MIC_FIF_CFG_PARAMS_MSG,
   IPC_DSV_SET_MIC_ANS_MODE_MSG,
   IPC_DSV_SEND_MIC_ANS_CFG_PARAMS_MSG,
   IPC_DSV_SET_MIC_BIF_MODE_MSG,
   IPC_DSV_SEND_MIC_BIF_CFG_PARAMS_MSG,
   IPC_DSV_SET_MIC_VOL_SCAL_FCTR_MSG,
   IPC_DSV_RESERVED_1,
   IPC_DSV_RESERVED_2,
   IPC_DSV_SEND_MPP_TONE_GEN_PARAMS_MSG,
   IPC_DSV_SET_MIC_VOICE_ENC_MAX_RATE_MSG,
   IPC_DSV_SET_MIC_VOICE_ENC_MIN_RATE_MSG,
   IPC_DSV_RESERVED_3,
   IPC_DSV_SEND_MIC_SIM_MSG_MSG,
   IPC_DSV_RESERVED_4,
   IPC_DSV_RESERVED_5,    
   IPC_DSV_SET_REV_CH_SSO_CONFIG_MSG,  
   IPC_DSV_SET_MIC_AMP_REV_VOL_SCAL_FCTR_MSG,
      
   /* Speaker Path (decoder) */
   IPC_DSV_SEND_SPKR_VOICE_FWD_CH_PCKT_DATA_MSG  =IPC_DSV_VAP_SPP_MIN_MSG,
   IPC_DSV_RESERVED_6,
   IPC_DSV_RESERVED_7,
   IPC_DSV_SEND_SPKR_VOICE_RECORD_PB_PCKT_DATA_MSG,
   IPC_DSV_SEND_SPP_TONE_GEN_PARAMS_MSG,
   IPC_DSV_RESERVED_8,
   IPC_DSV_SET_SPKR_FIF_MODE_MSG,
   IPC_DSV_SEND_SPKR_FIF_CFG_PARAMS_MSG,
   IPC_DSV_SEND_SPKR_VOL_SCAL_FCTR_MSG,
   IPC_DSV_SEND_FWD_CH_SSO_CONFIGURATION_MSG,
   IPC_DSV_RESERVED_25,
   IPC_DSV_SEND_SPKR_AMP_FWD_VOL_SCAL_FCTR_MSG,
   
   /* General Audio Path (both microphone and speaker) */
   IPC_DSV_SET_AUDIO_AIR_INTERFACE_MODE_MSG      =IPC_DSV_VAP_AUDIO_MIN_MSG,
   IPC_DSV_SEND_AUDIO_SSO_CONNECT_MSG,
   IPC_DSV_SEND_AUDIO_SSO_DISCONNECT_MSG,
   IPC_DSV_SEND_AUDIO_SSO_CTRL_PARAMS_MSG,
   IPC_DSV_SEND_AUDIO_ISR_SCAL_FCTR_MSG,
   IPC_DSV_SET_AUDIO_LOOPBACK_MODE_MSG,
   IPC_DSV_SEND_AUDIO_CONFIG_QUERY_MSG,
   IPC_DSV_SEND_AUDIO_SPCH_ROUTING_MSG,
   IPC_DSV_SET_AUDIO_AEC_MODE_MSG,
   IPC_DSV_SEND_AUDIO_AEC_MAIN_CFG_PARAMS_MSG,
   IPC_DSV_SET_AUDIO_RECORD_PLAYBACK_MODE_MSG,
   IPC_DSV_SEND_SPKR_SIM_MSG_MSG,

   IPC_DSV_SEND_AUDIO_EDAI_CONFIG_MSG,
   IPC_DSV_SEND_AUDIO_EDAI_SPKR_CTRL_DATA_MSG,
   IPC_DSV_SET_AUDIO_TTY_MODE_MSG,
   IPC_DSV_SEND_AUDIO_TTY_CFG_PARAMS_MSG,
   IPC_DSV_SEND_AUDIO_VAP_CFG_PARAMS_MSG,
   IPC_DSV_SEND_AUDIO_ACP_CFG_PARAMS_MSG,
   IPC_DSV_SET_AUDIO_ACP_MODE_MSG,
   IPC_DSV_RESERVED_14,
   IPC_DSV_RESERVED_15,
   IPC_DSV_RESERVED_23,
   IPC_DSV_RESERVED_24,
   IPC_DSV_SEND_AUDIO_PATH_CFG_MSG,
   IPC_DSV_RESERVED_16,
   IPC_DSV_RESERVED_17,
   IPC_DSV_SEND_AUDIO_AEC_AUX_CFG_PARAMS_MSG,
#if !(SYS_ASIC == SA_CBP55)
   IPC_DSV_I2S_DELAY_MODE_MSG,
#endif
   
   /* AMPS Related IPC message IDs */
   IPC_DSV_AMP_SEND_REV_CH_SIM_MSG            =IPC_DSV_AMP_REV_CH_MIN_MSG,
   IPC_DSV_AMP_SEND_WBD_RECC_MSG,  
   IPC_DSV_AMP_SEND_WBD_RVC_MSG,
   IPC_DSV_AMP_RECC_ABORT_MSG,
   IPC_DSV_AMP_REV_CH_PARAMS_CONFIG_MSG,       /* Configuration message */
   IPC_DSV_AMP_COMPRS_MODE_MSG,
   IPC_DSV_AMP_TRANSMIT_FIXED_FREQ_MSG,
   IPC_DSV_AMP_ST_MODE_MSG,
   IPC_DSV_AMP_REV_CH_CTRL_CONFIG_MSG,
   IPC_DSV_AMP_GET_COMP_EXP_STATUS_MSG,

   IPC_DSV_AMP_SEND_FWD_CH_SIM_MSG            =IPC_DSV_AMP_FWD_CH_MIN_MSG,
   IPC_DSV_AMP_MODE_MSG,
   IPC_DSV_AMP_BUSY_IDLE_PROCESS_MSG,
   IPC_DSV_AMP_GET_FM_SIGNAL_STRENGTH_MSG,
   IPC_DSV_AMP_FWD_IQ_DC_COMP_MSG,
   IPC_DSV_AMP_RX_AGC_CTRL_MSG,
   IPC_DSV_AMP_AFC_CTRL_MSG,
   IPC_DSV_AMP_FWD_CH_COUNTER_MSG,
   IPC_DSV_AMP_FWD_CH_PARAMS_CONFIG_MSG,       /* Configuration message */
   IPC_DSV_AMP_FWD_CH_AFC_CALIB_MSG,
   IPC_DSV_RESERVED_11,                        
   IPC_DSV_AMP_EXPANDER_MODE_MSG,
   IPC_DSV_AMP_HPF_MODE_MSG,
   IPC_DSV_AMP_SAT_MODE_MSG,
   IPC_DSV_AMP_SET_WS_COUNTER_MSG,
   IPC_DSV_AMP_GET_WS_COUNTER_MSG,
   IPC_DSV_AMP_FWD_LPF_MODE_MSG, 
   IPC_DSV_AMP_SET_PDM_MSG,
   IPC_DSV_AMP_FWD_CH_CTRL_CONFIG_MSG, 
   IPC_DSV_AMP_FWD_CH_AGC_CALIB_MSG,
   IPC_DSV_RESERVED_12,
   IPC_DSV_RESERVED_13,
   IPC_DSV_AMP_FWD_BUSY_IDLE_STATUS_MSG
} IpcDspMsgIdT;

/*---------------------------------------------------------------**
** Control Mailbox Message Ids: DSP to Control Processor (_CP_)  **
**---------------------------------------------------------------*/

#define  IPC_ACF_LEN_MAX            17


typedef enum
{
   /* Monitor Task Messages */
   IPC_CP_DOWN_COMP_MSG                     = 0x000,
   IPC_CP_ALIVE_MSG,
   IPC_CP_VER_RSP_MSG,
   IPC_CP_PEEK_RSP_MSG,
   IPC_CP_POKE_RSP_MSG,
   IPC_CP_LOOPBACK_RSP_MSG,
   IPC_CP_MEMTEST_RSP_MSG,
   IPC_CP_SPY_RSP_MSG,
   IPC_CP_TRACE_RSP_MSG,
   IPC_CP_FAULT_MSG,
   IPC_CP_BITWISE_OP_RSP_MSG,                      
   IPC_CP_HEART_BEAT_MSG,
   IPC_CP_CONFIG_MSG,
   IPC_CP_DBUF_LOOPBACK_RSP_MSG,
   IPC_CP_CODE_PEEK_RSP_MSG,
   IPC_CP_CODE_POKE_RSP_MSG,
   IPC_CP_CODE_MEMTEST_RSP_MSG,
   IPC_DSP_SHARED_MEM_LOOPBACK_RSP_MSG,

   /* DSPM to CP Messages */
   IPC_CP_RX_POWER_MSG                       =IPC_CP_FROM_DSM_MIN_MSG,
   IPC_CP_SEARCH_RSLT_MSG,
   IPC_CP_FPC_REP_MSG,
   IPC_CP_FWD_CHNL_PC_RSP_MSG,
   IPC_CP_FWD_CHNL_SCRAMBLE_RSP_MSG,
   IPC_CP_AFLT_MEAS_MSG,
   IPC_CP_TIMING_RSP_MSG,
   IPC_CP_RX_AGC_GET_RSP_MSG,
   IPC_CP_TX_AGC_GET_RSP_MSG,
   IPC_CP_AFC_GET_PARMS_RSP_MSG,
   IPC_CP_TXAGC_CR_STATUS_RSP_MSG,
   IPC_CP_SPAGE_RSP_MSG,
   IPC_CP_MDM_ACK_MSG,
   IPC_CP_MAG_SMRT_ANT_ACK_MSG,
   IPC_CP_AFC_UPDATE_OFT_MSG,


   IPC_CP_HW_MBOX_CFG_PARAMS_MSG             =IPC_CP_FROM_DSV_MIN_MSG,
   IPC_CP_SPCH_DATA_REV_CH_PACKET_MSG,
   IPC_CP_AUDIO_SSO_CONNECT_RSP_MSG,
   IPC_CP_AUDIO_SSO_DISCONNECT_RSP_MSG,
   IPC_CP_SEND_RECORD_PLAYBACK_DATA_MSG,     
   IPC_CP_SEND_AUDIO_PATH_CFG_RSP_MSG,
   IPC_CP_AMP_MODE_RSP_MSG,
   IPC_CP_SEND_AUDIO_EDAI_MIC_CTRL_DATA_MSG,
   IPC_CP_AMP_SAT_DET_RSP_MSG,
   IPC_CP_AMP_FWD_CH_DATA_RSP_MSG,
   IPC_CP_AMP_BUSY_IDLE_CHANGE_RSP_MSG,
   IPC_CP_AMP_SIGNAL_STRENGTH_RSP_MSG,   
   IPC_CP_SEND_AUDIO_CONFIG_QUERY_RSP_MSG,
   IPC_CP_AMP_WORD_SYNC_STATUS_RSP_MSG,
   IPC_CP_AMP_COMP_EXP_STATUS_RSP_MSG,
   IPC_CP_AMP_AFC_STATUS_RSP_MSG,
   IPC_CP_AMP_FWD_BUSY_IDLE_STATUS_RSP_MSG,
   IPC_CP_DV_RNGR_STATUS_MSG,     /* DV Signal for passing IpcDsvRingerStatusMsgT to CP */
   IPC_CP_APP_MODE_RSP_MSG,
   IPC_CP_APP_CONFIGURE_APPCHANNELS_MSG,
   IPC_CP_APP_IMAGE_SIZE_MSG,
   IPC_CP_APP_STATUS_MSG,
   IPC_CP_APP_INFO_QUERY_RSP_MSG
} IpcCpMsgIdT;

typedef enum
{
   IPC_SPCH_SRVC_OPTION_NULL  = 0,   /* Invalid service option  */
   IPC_SPCH_SRVC_OPTION1  = 1,       /* Not Supported. QCELP8  */
   IPC_SPCH_SRVC_OPTION3  = 3,       /* EVRC-A  */
   IPC_SPCH_SRVC_OPTION17 = 17,      /* QCELP13 */
   IPC_SPCH_SRVC_OPTION68 = 68       /* EVRC-B  */
} IpcSpchSrvcOptT;

typedef enum 
{
   IPC_SPCH_BLANK       = 0x0,
   IPC_SPCH_EIGHTH      = 0x1,
   IPC_SPCH_QUARTER     = 0x2,   /* not used for SSO 3 */
   IPC_SPCH_HALF        = 0x3,
   IPC_SPCH_FULL        = 0x4,
   IPC_SPCH_ERASURE     = 0xE,
   IPC_SPCH_FULL_LIKELY = 0xF    /* used only for SSO's 1&3 */
} IpcSpchSrvcOptRateT;

typedef enum 
{
   IPC_FWD_FRAME_BLANK       = 0x0,
   IPC_FWD_FRAME_EIGHTH      = 0x1,
   IPC_FWD_FRAME_QUARTER     = 0x2,   /* not used for SSO 3 */
   IPC_FWD_FRAME_HALF        = 0x3,
   IPC_FWD_FRAME_FULL        = 0x4,
   IPC_FWD_FRAME_ERASURE     = 0xE,
   IPC_FWD_FRAME_FULL_LIKELY = 0xF    /* used only for SSO's 1&3 */
} IpcFwdFrameRateT;

typedef enum 
{
   IPC_MS_REG_ID_RX, /* Rx Mixed Signal block */
   IPC_MS_REG_ID_TX, /* Rx Mixed Signal block */
   IPC_MS_REG_ID_VC  /* Voice Codec Mixed Signal block */
}IpcMsRegIdT;   

/*---------------------------------------------------------------------**
** Control Mailbox Message Stuctures: Control Processor to DSP (_DSP_) **
**---------------------------------------------------------------------*/

/*-------------------------------------------**
** CP to DSP Monitor Task Message Structures **
**-------------------------------------------*/

/* Download Init Message */
typedef PACKED struct
{
   uint16       LoadAddr;              
   uint16       CodeSize;
   uint16       MboxOffset;
} IpcDspDownInitMsgT;

#if (SYS_ASIC == SA_CBP55)
/* Download Init Message for CBP5.5 */
typedef PACKED struct
{
   uint16       DataSizeLo;
   uint16       DataSizeHi;
   uint16       MboxOffset;
   uint16       HeaderSize;
   uint16       Header[1];
} IpcDspHdrDownInitMsgT;
#endif

/* Code Download Message */
typedef PACKED struct
{
   uint16       Code[1];
} IpcDspCodeMsgT;

/* Peek Message */
typedef PACKED struct
{
   uint16       StartAddr;              
   uint16       NumWords;              
   uint16       RspTaskId; /* CP task to which the peek response should be routed */
   uint16       RspMboxId; /* CP mailbox to which the peek response should be routed */
} IpcDspPeekMsgT;

/* Poke Message */
typedef PACKED struct
{
   uint16       StartAddr;              
   uint16       NumWords;              
   uint16       Data[1];
} IpcDspPokeMsgT;

typedef enum
{
   MON_DSP_BITOP_TOGGLE = 0,
   MON_DSP_BITOP_SET,
   MON_DSP_BITOP_CLEAR
} BitwiseOpT;

/* Bitwise Operation Message */
typedef PACKED struct
{
   uint16       Addr;              
   uint16       Operator;              
   uint16       Mask;
} IpcDspBitwiseOpMsgT;

/* HW mailbox loopback message */
typedef PACKED struct
{
   uint16       Data[1];
} IpcDspMboxLoopMsgT;

/* RAM test Message */
typedef PACKED struct
{
   uint16       StartAddr;              
   uint16       NumWords;              
} IpcDspMemTestMsgT;

/* Spy Message */
typedef PACKED struct 
{
   uint16       SpyId;
   uint16       SpyActive;
} IpcDspSpyMsgT;

/* Trace Message */
typedef PACKED struct 
{
   uint16       TraceId;
   uint16       TraceActive;
} IpcDspTraceMsgT;

/* Fault Control Message values */
typedef enum
{
   IPC_FAULT_CTRL_DISABLE = FALSE,
   IPC_FAULT_CTRL_ENABLE  = TRUE
} IpcDspFaultCtrlT;

/* Fault Control Message */
typedef PACKED struct 
{
   uint16       Control;
} IpcDspFaultCtrlMsgT;

/* Heart Beat Control Message values */
typedef enum
{
   IPC_HEART_BEAT_CTRL_DISABLE = FALSE,
   IPC_HEART_BEAT_CTRL_ENABLE  = TRUE
} IpcDspHeartBeatCtrlT;

/* Heart Beat Control Message */
typedef PACKED struct 
{
   uint16       Control;
} IpcDspHeartBeatCtrlMsgT;

/* Direct Buffer Mailbox Loopback Message */
typedef PACKED struct
{
   uint32       NumLoops;
   uint16       NumWords;
   uint16       Data[1];
} IpcDspDBufLoopbackMsgT;

/* Profile Control Message */
typedef PACKED struct 
{
   uint16       Control;
} IpcProfileCtrlMsgT;

/* Mixed Signal Register Initialization Message */
typedef PACKED struct
{
   uint16 Offset; /* Number of words from start of register set to this block */
   uint16 NumInitValues;
   uint16 InitData[1];      /* Actual data in array sized from 1 to NumInitValues */
}IpcMsRegBlockT;   
   
/* Mixed Signal Register Initialization Message */
typedef PACKED struct 
{
   uint16             MsRegSetId;
   uint16             NumRegBlocks;
   IpcMsRegBlockT     Block[1]; /* Actual data in array sized from 1 to NumRegBlocks */
} IpcMixedSigRegInitMsgT;

/* Shared memory loopback message */
typedef PACKED struct
{
    uint16  NumWords;   /* Number of shared memory words to loop back */
} IpcDspSharedMemLoopbackMsgT;

/*----------------------------------------------**
** End CP to DSP Monitor Task Message Structures**
**----------------------------------------------*/

/*--------------------------------------------**
** CP to DSPM Forward Task Message Structures **
**--------------------------------------------*/

/*-------------------------------------------------------------------**
** IpcDsmFwdChnlCfgMsgT                                              **
** This message sets up the channel configuration only. Info specific**
** to basestations comes from the ActiveSetMsg (walsh codes)         **
**                                                                   **
** NOTE: Changes to this list must be accompanied by a corresponding **
**       change to the mapping tables in ipctask.c for both the CP & **
**       DSP code                                                    **
**                                                                   **
**-------------------------------------------------------------------*/
typedef enum 
{
   FSYNC,
   FPAGE,
   FBCCH,
   FQPCH,
   FCPCCH,
   FCACH,
   FCCCH,
   FDCCH,
   FFCH,   /* also for traffic channel in IS95 FTCH */
   FSCH,
   F_MON_CHNL,
   FWD_MAX_CHNL_TYPS
} FwdChnlTypeT;

typedef enum 
{
   RC1,
   RC2,
   RC3,
   RC4,
   RC5,
   RC_MAX_TYPS,
   RC_NONE        /* this value is used for channel types where no RC value is assigned */
} RadioConfigT;

typedef enum 
{
   F80MS,
   F40MS,
   F26MS,
   F20MS,
   F10MS,
   F5MS,
   F160MS,
   FWD_MAX_FRM_DURATION
} FrameDurationT;

typedef enum {
   DIV_1,
   DIV_2,
   DIV_4,
   DIV_8
} VitScaleT;

typedef enum {
   NO_ENCODE,
   HALF_RT,
   THIRD_RT,
   QUARTER_RT
} RateT;

typedef enum {
   NO_CRC,
   CRC_6_RC2,
   CRC_6,
   CRC_8,
   CRC_10,
   CRC_12,
   CRC_16
} CrcIdxT;

typedef enum {
   LC_MASK_0,
   LC_MASK_1,
   LC_MASK_DONT_CARE
} LcMaskIdxT;

typedef PACKED struct 
{
   uint16         ChnlTyp;     /*  FwdChnlTypeT */
   uint16         NumBits[4];  /* Number of data bits (including reserve bit) 
                                    NumBits_Full
                                    NumBits_Half
                                    NumBits_Quarter
                                    NumBits_Eighth
                               */
   uint16         FrameDuration;  /* (FrameDurationT) Frame duration */
   uint16         CrcIdx;    /* Index into CRC table 
                                [0:3]   CrcIdx_Full
                                [4:7]   CrcIdx_Half
                                [8:11]  CrcIdx_Quarter
                                [12:15] CrcIdx_Eighth
                             */
   uint16         Rate;      /* Convolution code rate */
   uint16         Rep1;      /* Repetition at output of convolutional code. (derepetition per formed by SW (Viterbi ISR)) */
                             /* If ChnlType = FFCH
                                 [0:3]   Rep1_Full
                                 [4:7]   Rep1_Half
                                 [8:11]  Rep1_Quarter
                                 [12:15] Rep1_Eighth
                                else
                                 [15] - 0=normal , 1=flexible repetition
                                 [14:0] - repetition, in flexible rep this is 'M-1'
                             */
   uint16         PuncIdx;   /* Index into puncture table
                                [0:3]   PuncIdx_Full
                                [4:7]   PuncIdx_Half
                                [8:11]  PuncIdx_Quarter
                                [12:15] PuncIdx_Eighth
                             */
   uint16         IntrlvMJ;  /* [0:7] m, [8:10] J0, [11:13] J1, J=2^J0 + 2^J1 J parameter  */
   uint16         Rep2;      /* Repetition at output of puncture (derepetition performed by HwRxAccel) */
   uint16         LcMaskIdx; /* index for Long Code Mask */
   uint16         SymbolLen; /* Symbol length (chips/symbol), redundant but saves calculation */
   uint16         VitScale;  /* Viterbi scale parameter */
    int16         FixScale;  /* Mrc Fixed scaling parameter */
   uint16         RdaDecAllRates; /* 0: standard Rda, 1: Viterbi Decode 4 times + Aposteriori Rda */
   uint16         DtxEnabled;/* DtxEnabled = 0 for FSYNC,FPAGE,FFCH. DtxEnabled = 1 for FDCCH, FSCH. */
} FChnlCfgT;

typedef PACKED struct 
{
   uint16       Disable;     /* one bit for each of channel: 0-do nothing, 1-disable */
   uint16       Dest;        /* destination of response message, this applies to all */
                             /* all channels */
   uint16       ChnlNum;     /* channel number: 0, 1, 2, 3 */
   uint16       Change;      /* one bit for each of fields below */
   uint16       RC;          /* Radio Config -- will contain enum type RC_1, etc. (RC_1 = 0) */
   uint16       FrameOffset; /* frame offset in x1.25ms */
   uint16       LcMask0[3];
   uint16       LcMask1[3];
   FChnlCfgT    ChnlCfg;     /* walsh code for channel & pilot in ActiveSetMsg */
} IpcDsmFwdChnlCfgMsgT;

/*-----------------------------------------------------------**
** IpcDsmFwdRdaHmetricsCfgMsgT                               **
** for IPC_DSM_FWD_RDA_H_METRICS_CFG_MSG                     **
** This message reconfigures the Viterib Scale and           **
** Rate Determination Apriori parameters in the Fwd unit.    **
**                                                           **
**-----------------------------------------------------------*/
typedef enum {
   FWD_RDA_RT_EIGHTH,
   FWD_RDA_RT_QUARTER,
   FWD_RDA_RT_HALF,
   FWD_RDA_RT_FULL,
   FWD_RDA_RT_MAX
}  FwdRdaRtT;

typedef enum {

   FE,
   HE,
   QE,
   FQ,
   HQ,
   FH,
   FWD_RDA_T_TBL_SZ
} FwdRdaTtblRC1IdxT;

typedef enum {
   VHF,
   VQF,
   VEF,
   VQH,
   VEH,
   VEQ
} FwdRdaTtblRC2to5IdxT; //added by wangran

typedef PACKED struct {
   uint16  FwdVitScale[RC_MAX_TYPS][FWD_RDA_RT_MAX];
   uint16  FwdRdaTtbl[RC_MAX_TYPS][FWD_RDA_T_TBL_SZ];
}  IpcDsmFwdRdaHmetricsCfgMsgT;


/*-----------------------------------------------------------**
** IpcDsmFwdRdaPmetricsCfgMsgT                               **
** for IPC_DSM_FWD_RDA_P_METRICS_CFG_MSG                     **
** This message reconfigures Rate Determination              **
** Static Bits Length and Aposteriori parameters.            **
**                                                           **
**-----------------------------------------------------------*/
#define FWD_RDA_YAMA_SZ      2
#define FWD_VITMET_RC_SZ     3
#define FWD_VITMET_RT_SZ     5
typedef enum {
   T0_HF,
   T0_QF,
   T0_EF,
   T0_QH,
   T0_EH,
   TP_HF,
   T0_EQ,
   TP_QH,
   TP_QF,
   TP_EQ,
   TP_EH,
   TP_EF,
   FWD_RDA_T0_TBL_SZ
} FwdRdaT0tblIdxT;

typedef PACKED struct {
   uint16  FwdRdaChkStaticBitsLen[RC_MAX_TYPS];
   uint16  FwdRdaYamatbl[FWD_RDA_YAMA_SZ];
   uint16  FwdRdaT0tbl[RC_MAX_TYPS][FWD_RDA_T0_TBL_SZ];
   uint16  FwdRdaSerThres[FWD_RDA_YAMA_SZ];
   uint16  FwdVitMetThresTbl[FWD_VITMET_RC_SZ][FWD_VITMET_RT_SZ];
}  IpcDsmFwdRdaPmetricsCfgMsgT;


/*-----------------------------------------------------------**
** IpcDsmFwdInitFwmParmsMsgT for                             **
** IPC_DSM_FWD_INIT_FWM_PARMS_MSG                            **
** This message reconfigures Fixed Scaling parameters.       **
** Received symbols are shifted left by the amount indicated **
** in the Fixed Scaling parameter before input to the        **
** Hardware Accelerator.                                     **
**                                                           **
**-----------------------------------------------------------*/
typedef PACKED struct {
   uint16  FwdPdaScaling[SYS_NUM_CHNL];
}  IpcDsmFwdInitFwmParmsMsgT;

/* IpcDsmFwdChnlPcMsgT: allows the forward channel puncture state to be set */
typedef PACKED struct 
{
   uint16       Change;      /* When non-zero the Enable field will be transferred to 
                                the forward channel scramble state.  Otherwise the 
                                Enable field is ignored. 
                             */
   uint16       Enable;      /* Set to non-zero to enable and zero to disable.
                             */
} IpcDsmFwdChnlPcMsgT; 

/* IpcDsmFwdChnlScrambleMsgT: message allows the forward channel scramble state to be set */
typedef PACKED struct 
{
   uint16       Change;      /* When non-zero the Enable field will be transferred to 
                                the forward channel scramble state.  Otherwise the 
                                Enable field is ignored. 
                             */
   uint16       Enable;      /* Set to non-zero to enable and zero to disable.
                             */
} IpcDsmFwdChnlScrambleMsgT; 

/* IpcDsmPdaParmMsgT post despread agc (soft scaling) parameters */
typedef PACKED struct 
{
   uint16       BlkShift[SYS_NUM_CHNL];      
   uint16       Alpha;         
   uint16       Q1UpdateConst; /* = 18 - log2((1-Alpha)*Fr/Fact_3) */
} IpcDsmPdaParmMsgT; 

/* IpcDsmPlfParmMsgT pilot filter parameters */
typedef PACKED struct 
{
   uint16       FirLen;        /* filter length */
   uint16       SymbolDelay;   /* filter delay */
} IpcDsmPlfParmMsgT;

/*-------------------------------------------------------------------**
** IpcDsmFwdFngrNoiseNormMsgT                                        **
** for IPC_DSM_FWD_FNGR_NOISE_NORM_MSG                               **
** This message reconfigures the Finger Noise Norm Parms & Table     **
**                                                                   **
**-------------------------------------------------------------------*/
#define FWD_FNGR_NO_NORM_MULT_TBL_SZ  16

/* IpcDsmFwdFngrNoiseNormMsgT finger noise normalization parameters */
typedef PACKED struct 
{
   uint16  FngrNoNormEnable;
   int16   IsrA0;
   int16   IsrB0;
   int16   FngrNoMin;
   uint16  FwdFngrNoNormMultTbl[RC3][FWD_FNGR_NO_NORM_MULT_TBL_SZ];
   uint16  D3LmtEnable;    /* 0: Disable, 1: Enable */
   int16   MonFreq;        /* In (n-1) multiples of 20ms */
   uint16  FngrLimit;      /* Max no. of Sch fingers at high data rate */
   uint16  FrmSzThres;     /* NumBits per frame */
   int16   FngrEcIoFactor; /* Finger EcIo Factor, linear number, Q12 */
}  IpcDsmFwdFngrNoiseNormMsgT;



/*------------------------------------------------**
** End CP to DSPM Forward Task Message Structures **
**------------------------------------------------*/

/*--------------------------------------------**
** CP to DSPM Reverse Task Message Structures **
**--------------------------------------------*/

/* IpcDsmRevChnlCfgMsgT */

/*
**                                                                   **
** NOTE: Changes to this list must be accompanied by a corresponding **
**       change to the mapping tables in ipctask.c for both the CP & **
**       DSP code                                                    **
**                                                                   **
*/
typedef enum 
{
   RFCH,     /* variable rate */
   RDCCH,
   RSCH,
   RACCESS,
   RHEACH,
   REACH,
   RCCCH,
   R_MON_CHNL
} RChnlTypeT;

typedef PACKED struct 
{
   uint16         ChnlTyp;     /* (RChnlTypeT) channel type */
   uint16         NumBits[4];  /* Number of data bits (including reserve bit) 
                                    NumBits_Full
                                    NumBits_Half
                                    NumBits_Quarter
                                    NumBits_Eighth
                               */
   uint16         FrameDuration;  /* (FrameDurationT) Frame duration */
   uint16         CrcIdx;      /* Index into CRC table 
                                  [0:3]   CrcIdx_Full
                                  [4:7]   CrcIdx_Half
                                  [8:11]  CrcIdx_Quarter
                                  [12:15] CrcIdx_Eighth
                               */
   uint16         Rate;        /* bit8 - Turbo/Conv, [0:7]=code rate */
   uint16         Rep1;        /* Repetition at output of convolutional code. */
                               /* If normal repitition (see IntrlvMJ[15])
                                   [0:3]   Rep1_Full    M-1  for all channels
                                  for RFCH
                                   [4:7]   Rep1_Half    M-1
                                   [8:11]  Rep1_Quarter M-1
                                   [12:15] Rep1_Eighth  M-1
                                  else
                                    [14:0] - repetition, in flexible rep this is 'M-1'
                               */
   uint16         PuncIdx;     /* Index into puncture table
                                  [0:3]   PuncIdx_Full
                                  [4:7]   PuncIdx_Half
                                  [8:11]  PuncIdx_Quarter
                                  [12:15] PuncIdx_Eighth
                               */
   uint16         IntrlvMJ;    /* [0:7] m, [8:10] J0, [11:13] J1, J=2^J0 + 2^J1 J parameter  
                                  [15] - 0=normal, 1=flexible repetition   */
   uint16         ChipOffset;  /* frame offset in chip increments (for ACCESS & EACH) */
                               /* note EACH delay is in increments of 1.25 ms */
} RChnlCfgT;


typedef PACKED struct 
{
   uint16         PilotInfo;    /* [0:3] 0-full, 1-1/2, 2-1/4; [4] 1=enable pilot, 0=disable pilot */
                                /* [8] 1 -> subtract mean input power from OpenLoopPwrCorr */
                                /* probably not used. only need to be within 500 ms. */
   uint16         OpenLoopPwrCorr; /* Open Loop Tx power correction from PE in dB Q6 */
} RPilotCfgT;

typedef PACKED struct 
{
   uint16         Disable;        /* one bit for each of channel: 0-do nothing, 1-disable */
   uint16         ChnlNum;        /* physical channel number: 0, 1, 2  */
   uint16         Change;         /* one bit for each of fields below */
   uint16         RC;             /* Radio Config -- use RC_NONE enum value for channels for which RC does not apply */
   uint16         LcMask[3];      /* TX long code mask */
   RPilotCfgT     PilotCfg;       /* pilot config */
   RChnlCfgT      ChnlCfg;        /* channel config */
   uint16         ChnlWalsh;      /* [0:1] walsh cover */
   uint16         ChTxPwrToPilot[4];   /* channel tx power relative to pilot in linear Q6*/
                                  /* FCH: Full, Half, Quarter, Eighth rate (if variable rate) */
                                  /* or SCH:  the 3 rates for supplemental channel. */
                                  /* or DCCH: Full rate */
                                  /* ChTxPwrToPilot[0] = 1 when no pilot, else = pwr relative to pilot */
   uint16         RevFchGatPwrCntlStep; /* [0:1] RevPwrCntlDelay; [7] RevFchGatingMode  [8:9:10] PwrCntlStep */
} IpcDsmRevChnlCfgMsgT; 

/*------------------------------------------------------------------**
** IpcDsmRevChnlDtaMsgT                                             **
** Data actually in shared memory. This is an indicator to tell the **
** dsp that the data is ready.                                      **
**------------------------------------------------------------------*/

typedef enum                      /* Rev fch rate */
{
  R_FULL = 0,
  R_HALF,
  R_QUARTER,
  R_EIGHTH
} RevFundRateTypeT;

typedef PACKED struct 
{
   uint16       ChnlDataRdy;  /* bit field indicating channels are ready */
   uint16       FundRate;     /* fund chnl rate if assigned */
} RevChnlDtaMsgT;

/* IpcDsmRevPreambleCfgMsgT */
typedef PACKED struct 
{
   uint16       Enable;               /* enable, disable */
   uint16       MeanOutputPwr;
   uint16       UseCurrMip;           /* flag indicates whether to use current Mean Input Power */
   uint16       Type;                 /* 0-Pilot, 1-use parms below,
                                         2-Rc1&2 Preamble */
   uint16       StartSymbol;          /* [15]-Next20msFrame, [8:0]-SymbolCount */
   uint16       PreambleNumFrac;
   uint16       PreambleFracDuration;
   uint16       PreambleOffDuration;
   uint16       PreambleAddDuration;
} IpcDsmRevPreambleCfgMsgT; 
/*
  StartSymbol(0:383)- Is the SymbolCount in 20ms Frames of when the preamble should
  start. When Next20msFrame bit is set the preamble starts at SymbolCount in the
  following 20ms frame.
*/

/* IpcDsmRevMixSigParmMsgT */
typedef PACKED struct 
{
   uint16       RevMixSigParm[16];  /* GainComp1,2, GainSel, ClipLvl,ClipMaxthres,ClipMaxSigmaX,ClipSigmaP,KsThres[9] */
} IpcDsmRevMixSigParmMsgT;

/*------------------------------------------------**
** End CP to DSPM Reverse Task Message Structures **
**------------------------------------------------*/

/*-----------------------------------------------**
** CP to DSPM RF Control Task Message Structures **
**-----------------------------------------------*/
typedef enum
{
#if ( ((SYS_ASIC == SA_ROM) && (SYS_VERSION >  SV_REV_C3)) || (SYS_ASIC == SA_RAM) || (SYS_ASIC == SA_CBP55) )
   RFC_DISABLE=0,
   RFC_ENABLE,
   RFC_SAVE_DISABLE,            /* GPS_START     */
   RFC_RESTORE_DISABLE,         /* GPS_CONT      */
   RFC_RESTORE_ENABLE
#else
   RFC_DISABLE=1,
   RFC_ENABLE=0,
   RFC_SAVE_DISABLE=4,          /* GPS_START     */
   RFC_RESTORE_DISABLE=5,       /* GPS_CONT      */
   RFC_RESTORE_ENABLE=6
#endif
} RfcSwCntrlModeEnum;

typedef enum
{
   ETS_AUTOMATIC=0,
   ETS_DISABLE,
   ETS_MANUAL
} RfcEtsConfigModeEnum;

/*--------------------------------------------------------------------------------**/
/*                         Software control messages                               */
/*--------------------------------------------------------------------------------**/
/* IPC_DSM_DCB_SW_CNTRL */
typedef PACKED struct
{
   uint16  CtrlMode;  /* mapped to RfcSwCntrlModeEnum */
} IpcDsmDCBSWControlMsgT;

/* IPC_DSM_RXAGC_SW_CNTRL */
typedef PACKED struct
{
   uint16  CtrlMode;  /* mapped to RfcSwCntrlModeEnum */
} IpcDsmRxAgcSWControlMsgT;

/* IPC_DSM_TXAGC_SW_CNTRL */
typedef PACKED struct
{
   uint16  CtrlMode;  /* mapped to RfcSwCntrlModeEnum */
} IpcDsmTxAgcSWControlMsgT;

/* IPC_DSM_AFC_SW_CNTRL */
typedef PACKED struct
{
   uint16  CtrlMode;  /* mapped to RfcSwCntrlModeEnum */
} IpcDsmAfcSWControlMsgT;


/*--------------------------------------------------------------------------------**
** IpcDsmAfcParmMsgT afc parameters                                               **
** Afc only performed on receiving frequency when fingers are assigned. Afc not   **
** performed during candidate freq search. So only one PpmPerHz value is required.**
** This message needs to be sent for each frequency band to change PpmPerHz.      **
**--------------------------------------------------------------------------------*/ 
typedef enum
{
   AFC_VCTCXO=0,
   AFC_DCXO,  /* AFC controlled by SPI */
   AFC_VCXO,  /* AFC controlled by Pdm */
   AFC_INVALID = 0xFFFF     
} AfcDcxoModeEnum;

typedef PACKED struct    /* FQQ */
{
   uint16       Decim;        /* samples btw phase calc */
   uint16       DecimMsg;     /* #phase sums */
   int16        Shift;
   int16        CorrGain;     /* AR1 filter of phase */
   int16        CorrGainQ;    /*  */
   int16        PdmPpmSlopeQ; /* */
   int16        PpmOfst;  
   int16        PpmQ;        /* 11 to accommodate +/-15 ppm*/
   int32        RStatPwrTh;   /* Threshold for disabling VCO updates */
   int16        PpmPerRot;    /* PpmPerRotation = IsrFreq/Decim/NumFgr/CarrierFrq*/
   int16        PpmPerRotQ;   /* max=19200/8/1/196 = 12.2 => Q7 */
   int16        PpmUpdateScaleQ; /* Q for update coef */
   int16        AfcFgrCombInd;/* Bit field indicating which fingers are used for AFC combining LSB=>finger 0*/
#if ( ((SYS_ASIC == SA_ROM) && (SYS_VERSION >  SV_REV_C3)) || (SYS_ASIC == SA_RAM) || (SYS_ASIC == SA_CBP55) )
   AfcDcxoModeEnum AfcDcxoMode;
   int16        PdmMin;
   int16        PdmMax;
#endif
} IpcDsmAfcParmMsgT; 

/*--------------------------------------------------------------------------------**
** IpcDsmAfcCalibrParmMsgT afc parameters                                         **
**--------------------------------------------------------------------------------*/ 
#define IPC_AFC_DCXO_PPM_SLOPE_NUM    8

typedef PACKED struct
{
   int16        BoundaryLeftPpm;
   int16        SlopeDcxoPerPpm;
   int16        InterceptDcxo; 
} DcxoLinearRegionT; 

typedef PACKED struct    /* FQQ */
{
   int16        PdmPpmSlope;  /* conversion factor, Ppm frequency error => PDM setting */
   int16        PdmOfst;      /* IAFC_Value, Afc default offset */
#if ( ((SYS_ASIC == SA_ROM) && (SYS_VERSION >  SV_REV_C3)) || (SYS_ASIC == SA_RAM) || (SYS_ASIC == SA_CBP55) )
   int16        AfcIaqPosOfst;/* Afc Positive offset to be used before non-coherent search */
   int16        AfcIaqNegOfst;/* Afc Negative offset to be used before non-coherent search */
   int16        DcxoSlopeNum;
   DcxoLinearRegionT DcxoLinearRegion[IPC_AFC_DCXO_PPM_SLOPE_NUM];
#endif
} IpcDsmAfcCalibrParmMsgT; 

/*--------------------------------------------------------------------------------**
** IpcDsmAgcParmMsgMsgT AGC parameters                                            **
** This is copied from CBP3.0. Some parameters commented out                      **
** because these are generated by the DSP now using the                           **
** Rx calibration table.                                                          **
**--------------------------------------------------------------------------------*/ 
typedef PACKED struct 
{ 
   int16        HWDacVal;
   int16        Target;
   uint16       PdmMin;
   uint16       PdmMax;
} IpcDsmAgcParmMsgT; 

/*------------------------------------------------------------**
** IpcDsmDcBiasParmMsgT DC bias parameters                    **
** Note that this only effects our chip internally so should  **
** not depend on external hw. This should probably report the **
** DC bias to the CP:L1 in case there are external calib that **
** need to be performed.                                      **
**------------------------------------------------------------*/
typedef PACKED struct 
{
   uint16       Decim;
   uint16       DecimMsg;
   int16        Shift;
#if ( ((SYS_ASIC == SA_ROM) && (SYS_VERSION >  SV_REV_C3)) || (SYS_ASIC == SA_RAM) || (SYS_ASIC == SA_CBP55) )
   uint16       ZeroAdj;
#endif
} IpcDsmDcBiasParmMsgT; 

/* DC Bias Config message */
typedef PACKED struct 
{
    uint16       CtrlMode;
    int16        DcOffsetI;
    int16        DcOffsetQ;
} IpcDsmDcBiasConfigMsgT; 

/*--------------------------------------------------------------------------------**
** IpcDsmTxAgcParmMsgT TxAGC parameters                                            **
**--------------------------------------------------------------------------------*/ 
typedef PACKED struct 
{ 
   uint16       OLCorrGainQ;
   uint16       OLMaxSlewQ;
} IpcDsmTxAgcParmMsgT; 

typedef PACKED struct 
{ 
   int16        PwrMaxdBQ;
   int16        PwrMindBQ;
   int16        TxAgcCalibrAdjdBQ[SYS_MAX_NUM_HYST_STATES_TXAGC];     /* CPt + CPf + CPv:Calibration Adjustments at nominal tx levels */
   int16        TxAgcMaxCalibrAdjdBQ;    /* CPMAXt + CPMAXf + CPMAXv:Calibration Adjustments at max tx levels  */
} IpcDsmTxAgcPwrMsgT; 

/*--------------------------------------------**
** IpcDsmRxCalParmMsgT                        **
** The most often used parms are in DSP.      **
** The slow corrections caused by Temp & Freq **
** are computed in the CP and passed down.    **
**--------------------------------------------*/
#define RX_CAL_PARMS_CHANGE_RF_BAND_BIT     0x0001 
#define RX_CAL_PARMS_CHANGE_RX_IQ_SWAP_BIT  0x0002
#define RX_CAL_PARMS_CHANGE_BASE_CH_ADJ_BIT 0x0004
#define RX_CAL_PARMS_CHANGE_THRESH_BIT      0x0008
#define RX_CAL_PARMS_CHANGE_CALIB_LINE_BIT  0x0010 

typedef PACKED struct
{
    int16       BoundaryLeftStep;/* left boundary of the region, units DAC Step--RxAgcHWDacVal in Q5 precision */
    int16       SlopeDbPerStep;  /* slope of the linear region, units dBGainQ/DAC step (Q = SYS_LIN_APPROX_SLOPE_Q(=8) + RFC_DB_Q(=6)= 14) */
    int16       InterceptdB;     /* dB Gain intercept of region, units dB, Q=RFC_DB_Q */
} DbToDacStepLinearRegionT;      /* structure for a region of the piecewise linear transfer function */
                                 /* of a dBGain to DAC step conversion (Rx gain) */

typedef PACKED struct
{
    uint16  RxAgcLNAGainStates;
    int16   RxAgcHystHighThresh1Db; /* thresh to switch the RxAGC 1st hyst state to medium gain,RxGain in dB with Q = RFC_DB_Q*/
    int16   RxAgcHystLowThresh1Db;  /* thresh to switch the RxAGC 1st hyst state to low gain,RxGain in dB with Q = RFC_DB_Q*/
    int16   RxAgcHystHighThresh2Db; /* thresh to switch the RxAGC 2nd hyst state to high gain,RxGain in dB with Q = RFC_DB_Q */
    int16   RxAgcHystLowThresh2Db;  /* thresh to switch the RxAGC 2nd hyst state to medium gain,RxGain in dB with Q = RFC_DB_Q*/
    int16   RxAgcHystDelayThresh;   /* thresh to switch the RxAGC from higher state to lower with delay */
    uint16  RxAgcHystDelayCount;    /* delay count defines the number of PCG to be checked before RxAgcHystDelayThresh works */
    int16   Low2MedPdmCorrection;   /* PDM correction used while switching LNA state - Q5 */
    int16   Med2LowPdmCorrection;   /* PDM correction used while switching LNA state - Q5 */
    int16   Med2HighPdmCorrection;  /* PDM correction used while switching LNA state - Q5 */
    int16   High2MedPdmCorrection;  /* PDM correction used while switching LNA state - Q5 */
} RxAgcThreshT;

typedef PACKED struct 
{
    uint16                    Change;               /* bit field to indicate changes in each of the elements below */
    uint16                    RfBand;               /* RF Band */
    uint16                    RxIQSwap;             /* Flag indicating if Rx IQ needs to be swapped, 0: no swap, 1: swap */
    int16                     RxAgcBaseChAdjustdB;  /* temp & freq adjustment for base channel */
    int16                     RxAgcCandChAdjustdB;  /* temp & freq adjustment for candidate channel */
    RxAgcThreshT              RxAgcThresh;          /* dB Gain Threshold values */ 
    DbToDacStepLinearRegionT  RxAgcCalibLine[SYS_MAX_NUM_HYST_STATES_RXAGC][SYS_MAX_NUM_GAIN_POINTS_RXAGC];
} IpcDsmRxCalParmMsgT;
 
/*--------------------------------------------**
** IpcDsmTxCalParmMsgT                        **
** The most often used parms are in DSP.      **
** The slow corrections caused by Temp & Freq **
** are computed in the CP and passed down.    **
**--------------------------------------------*/
#define TX_CAL_PARMS_CHANGE_RF_BAND_BIT     0x0001
#define TX_CAL_PARMS_CHANGE_BASE_CH_ADJ_BIT 0x0002
#define TX_CAL_PARMS_CHANGE_THRESH_BIT      0x0004 
#define TX_CAL_PARMS_CHANGE_CALIB_LINE_BIT  0x0008 

typedef PACKED struct
{
   int16        BoundaryLeftDbm;  /* left boundary of the region, units dBm, Q=RFC_DB_Q */
   uint16       SlopeStepPerDbm;  /* slope of the linear region, units DAC step/dBmQ (Q = SYS_LIN_APPROX_SLOPE_Q) */
   int16        InterceptStep;    /* DAC Step intercept of region, Q0 */
} DacStepToDbmLinearRegionT;      /* structure for a region of the piecewise linear transfer function */
                                  /* of a DAC step to Dbm conversion (Tx power ) */

typedef PACKED struct
{
    uint16  TxAgcPAGainStates;
    int16   TxAgcHystHighThresh1Dbm; /* thresh to switch the TxAGC 1st hyst state to med gain,TxGain in dBm with Q = RFC_DB_Q*/
    int16   TxAgcHystLowThresh1Dbm;  /* thresh to switch the TxAGC 1st hyst state to low gain,TxGain in dBm with Q = RFC_DB_Q*/
    int16   TxAgcHystHighThresh2Dbm; /* thresh to switch the TxAGC 2nd hyst state to high gain,TxGain in dBm with Q = RFC_DB_Q */
    int16   TxAgcHystLowThresh2Dbm;  /* thresh to switch the TxAGC 2nd hyst state to med gain,TxGain in dBm with Q = RFC_DB_Q*/
} TxAgcThreshT;

/* IPC_DSM_TXAGC_CAL_PARM_MSG Message type */
/* Message structure which supports multi-state Tx AGC adjustments */
typedef PACKED struct 
{
    uint16                      Change;               /* bit field to indicate changes in each of the elements below */
    uint16                      RfBand;               /* RF Band */
    int16                       TxAgcChnlAdjustdB[SYS_MAX_NUM_HYST_STATES_TXAGC];    
                                                      /* temp & freq adjustment for base channel */
    TxAgcThreshT                TxAgcThresh;          /* dB Gain Threshold values */ 
    DacStepToDbmLinearRegionT   TxPwrCalibLine[SYS_MAX_NUM_HYST_STATES_TXAGC][SYS_MAX_NUM_GAIN_POINTS_TXAGC];
} IpcDsmTxCalParmMsgT; 

/*----------------------------------**
** IPC_DSM_TXAGC_CONFIG_MSG Message **
**-----------------------------------*/
typedef PACKED struct
{
   uint16          CtrlMode;
   uint16          Method;
   uint16          PdmValue;
   int16           HystState;
   uint16          Power;
} IpcDsmTxAgcConfigMsgT;

/* Method */
#define IPC_HW_VALUE    0  
#define IPC_DB_GAIN     1  

/* IPC_DSM_TX_AGC_GET_MSG Message type */
typedef PACKED struct
{
    uint16    Power;
    uint16    HwValue;
} IpcDsmTxAgcGetMsgT;

/*----------------------------------**
** IPC_DSM_RXAGC_CONFIG_MSG Message **
**-----------------------------------*/
typedef PACKED struct
{
    uint16          CtrlMode;
    uint16          PdmValue;
#if ( ((SYS_ASIC == SA_ROM) && (SYS_VERSION >  SV_REV_C3)) || (SYS_ASIC == SA_RAM) || (SYS_ASIC == SA_CBP55) )
    int16           BitSel;
    int16           DGainIdx;
#endif
} IpcDsmRxAgcConfigMsgT;

/* IPC_DSM_AFC_CONFIG_MSG Message type */
typedef PACKED struct
{
    uint16          CtrlMode;
    uint16          PdmValue;
} IpcDsmAfcConfigMsgT;

/* DC Bias Scaling message - IPC_DCB_GAINBAL_BITSEL_MSG */
typedef PACKED struct 
{
   uint16 BitSel;
   int16 Gba1I;
   int16 Gba2I;
   int16 Gba1Q;
   int16 Gba2Q;
} IpcDsmDcBiasBitSelMsgT; 

/* IPC_DSM_RX_CAL_MEASURE_MSG Message type */
typedef PACKED struct
{
    uint16    CalMode;
    int16     HystState;
    uint16    SettleTime;
} IpcDsmRxCalMeasureMsgT;

/* IPC_DSM_RX_CAL_OVERRIDE_MSG Message type */
typedef PACKED struct
{
    uint16    UseNvramData;
    uint16    Slope;
} IpcDsmRxCalOverrideMsgT;

/* IPC_DSM_RX_AGC_GET_MSG Message type */
typedef PACKED struct
{
    uint16    Power;
    uint16    HwValue;
} IpcDsmRxAgcGetMsgT;

/* IPC_DSM_RFON_TABLE Message type */
typedef PACKED struct
{
    uint16    RxRfonBitMask[3];
} IpcDsmRfonTableMsgT;

typedef enum {
    LOW_TO_MED_TRANS=0,
    MED_TO_LOW_TRANS,
    MED_TO_HIGH_TRANS,
    HIGH_TO_MED_TRANS,
    NUMBER_OF_TRANS
}TransitionType;

/* IPC_DSM_TXON_TABLE Message type */
typedef PACKED struct
{
    uint16    TxonBitMask[3];
    uint16    Pa1CmpAddress[NUMBER_OF_TRANS];
    uint16    Pa1CompareValue[NUMBER_OF_TRANS];
    uint16    Pa2CmpAddress[NUMBER_OF_TRANS];
    uint16    Pa2CompareValue[NUMBER_OF_TRANS];
    uint16    PdmCmpAddress[NUMBER_OF_TRANS];
    uint16    PdmCompareValue[NUMBER_OF_TRANS];
    uint16    TxonDlyLdBitMask[NUMBER_OF_TRANS];
    uint16    PdmDlyLdBitMask[NUMBER_OF_TRANS];
} IpcDsmTxonTableMsgT;


/* IPC_DSM_RXAGC_SETTLE_TIME Message type */
typedef PACKED struct
{
    uint16    RxAgcFastSTime;
    uint16    RxAgcSlowSTime;
    uint16    RxAgcFastDecimMsg;
    uint16    RxAgcSlowDecimMsg;
} IpcDsmRxAgcSettleTimeMsgT;


/* IPC_DSM_TXAGC_RELBS_THRSH Message type */
typedef PACKED struct
{
    int16    ReliableBsThreshPwr;
} IpcDsmTxAgcRelibBsThrshMsgT;

typedef PACKED struct
{
    uint16  DcbFastDecim;
    uint16  DcbFastDecimMsg;
    int16   DcbFastShiftUp;
} IpcDsmDcbFastDecimMsgT;

/* IPC_DSM_IIR_RESET_CTRL */
typedef PACKED struct
{
    uint16  RfcRxIirEnb;
    uint16  RfcRxPdmThreshold; /* in Q3 */
    uint16  RfcRxPdmRecovery; /* in Q3 */
    uint16  RfcRxIQThreshold;
} IpcDsmIirResetCtrlMsgT;


/*---------------------------------------------**
** IPC_DSM_RX_SD_PARMS_CFG: IpcDsmRxSdParmMsgT **
**---------------------------------------------*/
#define RFC_RX_SD_PARMS_SZ           22
typedef enum 
{
   ANALOG_RXAGC,              /* RxAGC using PDM control */
   DIGITAL_RXAGC_USING_RFON,  /* Digital stepped-gain RxAGC using RF_ON interface */
   DIGITAL_RXAGC_USING_SPI,   /* Digital stepped-gain RxAGC using SPI interface */
   MAX_RXAGC_ALG_TYPES
} RxAgcAlgTypeT;

typedef PACKED struct
{
  uint16 RxAgcAlgType;  /* Rx AGC control scheme - indentified using RxAgcAlgTypeT enum */
  uint16 CpFiqDly;      /* CP Fast Interrupt Delay, in units of symbol time (52us) */
} IpcDsmRxSdParmMsgT;


/*-------------------------------------------------**
** IPC_DSM_RX_DAGC_PARMS_CFG: IpcDsmRxDAgcParmMsgT **
**-------------------------------------------------*/
#define RFC_RX_DAGC_DGAIN_SZ         11
#define RFC_RX_DAGC_SGAIN_SZ         8
#define RFC_RX_DAGC_SGAIN_XTNS_SZ    (RFC_RX_DAGC_SGAIN_SZ << 1)
typedef PACKED struct 
{
    uint16 Change;               /* bit field to indicate changes in each of the elements below */
    uint16 RfBand;               /* RF Band */
    uint16 RxIQSwap;             /* Flag indicating if Rx IQ needs to be swapped, 0: no swap, 1: swap */
    int16  RxAgcBaseChAdjustdB;  /* temp & freq adjustment for base channel */
    int16  RxAgcCandChAdjustdB;  /* temp & freq adjustment for candidate channel */
    int16  SGainMax;                              /* Max number of SGain states */
    int16  SGainStep[RFC_RX_DAGC_SGAIN_SZ];       /* SGain steps in Log2, Q8 */
    int16  SGainThres[RFC_RX_DAGC_SGAIN_XTNS_SZ]; /* SGain Thres in dBm,  10Log10 */
    int16  SGainMsc[RFC_RX_DAGC_SGAIN_XTNS_SZ];   /* b[7:0]  = HystDly, 
                                                     b[14:8] = NewGainState
                                                     b15     = CmpGe: 1: greater than or equal to , 0: less than
                                                  */
    int16  PwrdBmRef;                             /* Reference power in dBm, 10Log10, Q6*/
    int16  SGainRef;                              /* Gain State 0..7 at Reference power */
    uint16 DGainRef;                              /* DGain at Reference power 
                                                     b[15:8] = DigiGain, b[7:0] = DigiBitsel
                                                  */
} IpcDsmRxDAgcParmMsgT;

/*-------------------------------------------------------**
** IPC_DSM_RX_DAGC_GAIN_STATE: IpcDsmRxDAgcGainStateMsgT **
**-------------------------------------------------------*/
typedef PACKED struct
{
    uint16 RxDAgcEnable;
    uint16 SGainState;
} IpcDsmRxDAgcGainStateMsgT;

/*---------------------------------------------------**
** End CP to DSPM RF Control Task Message Structures **
**---------------------------------------------------*/

/*---------------------------------------------------**
** CP to DSPM Miscellaneous Task Message Structures  **
**---------------------------------------------------*/

/* IpcDsmMdmRstMsgT modem reset - reset various modem functions */
typedef enum {
  FWD_INIT_BN=0,
  MSC_INIT_BN=1,
  REV_INIT_BN=2,
  RFC_INIT_BN=3,
  SCH_INIT_BN=4
 } IpcDsmMdmRstMsgResetBnT;

#define IPC_FWD_TASK_RESET   (1 << FWD_INIT_BN)
#define IPC_MSC_TASK_RESET   (1 << MSC_INIT_BN)
#define IPC_REV_TASK_RESET   (1 << REV_INIT_BN)
#define IPC_RFC_TASK_RESET   (1 << RFC_INIT_BN)
#define IPC_SCH_TASK_RESET   (1 << SCH_INIT_BN)
#define IPC_ALL_TASKS_RESET  (IPC_FWD_TASK_RESET | \
   IPC_MSC_TASK_RESET | IPC_REV_TASK_RESET | IPC_RFC_TASK_RESET | IPC_SCH_TASK_RESET)

/* This message requires an ack */
typedef PACKED struct 
{
   uint16      SeqNum;
   uint16      Reset;  /* bit field: with IpcDsmMdmRstMsgResetBnT */

   uint16      Type;   /* 0 - cold, 1 - warm */
} IpcDsmMdmRstMsgT;

typedef PACKED struct 
{
   uint16      MsgId;
   uint16      SeqNum;
} IpcDsmMdmAckMsgT;

/* IpcDsmSyncSysTimeMsgT */
typedef PACKED struct 
{
   uint16       Immediate;   /* 1 => sync operation should take place as soon as possible */
                             /* 0 => allow for receive delay in Timing Change sync case */ 
   uint16       PilotPn;     /* Tc/2 resolution */
   uint16       LCState[3]; 
} IpcDsmSyncSysTimeMsgT; 


/* IpcDsmWakeAcqSyncMsgT */
typedef PACKED struct 
{
   uint16       CdmaBand;  /* band (1st relase always PCS) */
   uint16       FreqChan;  /* frequency channel */
   uint16       PNOffset;  /* base PN offset*/
                           /* allow hard handoff to be performed on wakeup*/
} BaseChnlT;

typedef PACKED struct 
{
   uint16       Change;    /* one bit for each of fields below */
   BaseChnlT    BaseChnl; 
   uint16       WinSize;    /* search window size */
   uint32       SysCtrInit;
   uint16       LCState[3];
} IpcDsmWakeAcqSyncMsgT; 

/*-------------------------------------------------------------------------**
** IpcDsmWakeStatesMsgT                                                    **
** Notes:                                                                  **
**   The dsp will program the LC and PN states immediately. There may      **
**   be slight timing differences due to time tracking (1/8 chips in 80ms).** 
**   The search window at wakeup will take care of this.                   **
**-------------------------------------------------------------------------*/
typedef PACKED struct 
{
   uint16       PNOffset;  /* offset of PN state at wakeup from current */
                           /* DSP uses this to calculate the PN state */
                           /* to initialize the PN states upon wakeup */
   uint16       LCState[3];
} IpcDsmWakeStatesMsgT; 

/*-------------------------------------------------------------**
** IpcDsmFPCParmMsgT forward power control parameters          **
**  should be all parms from Protocol Stack API related to FPC **
**  from L1D_TRAFFIC_CHAN_START_MSG, L1D_SERVICE_CONFIG_NN_MSG **
**  This is completely performed in the DSP                    **
**-------------------------------------------------------------*/
#define IPC_FWD_RATE_16X             0
#define IPC_FWD_RATE_8X              1
#define IPC_FWD_RATE_4X              2
#define IPC_FWD_RATE_2X              3
#define IPC_FWD_RATE_1X              4
#define IPC_FWD_RATE_05X             5
#define IPC_FWD_RATE_025X            6
#define IPC_FWD_RATE_0125X           7
/* IpcDsmFpcParmMsg.ChChanged definition */
#define IPC_FPC_FCH_INCL             0x0001
#define IPC_FPC_DCCH_INCL            0x0002
#define IPC_FPC_SCH_INCL             0x0004
#define IPC_FPC_MODE_INCL            0x0010
#define IPC_FPC_PRI_CHAN_INCL        0x0020
#define IPC_FPC_PRI_THRESH_INCL      0x0040
#define IPC_FPC_SEC_THRESH_INCL      0x0080
#define IPC_FPC_SUBCHAN_GAIN_INCL    0x0100
#define IPC_FPC_SCH_RATE_INCL        0x0200
#define IPC_FPC_CAL_INCL             0x0400
#define IPC_FPC_SCH_INIT_SETPT_OP_INCL 0x0800

typedef PACKED struct 
{
   uint16  ChnlNum;   /* physical channel number 0..3 */
   int16   InitSetPt; /* initial setpoint,  in 0.125 dB unit */
                      /* 0xffff means ignore */
   uint16  Fer;       /* Frame Error Rate, in 0.1% unit (range 2-300) */
   int16   MinSetPt;  /* minimum setpoint, in 0.125 dB unit. */
                      /* 0xffff means set min setpoint to current set point    */
   int16   MaxSetPt;  /* maximum setpoint, in 0.125 dB unit. */
                      /* 0xffff means set max setpoint to current set point */
}FpcParmsT;

typedef PACKED struct 
{
   uint16 RC;         /* RC_1 to RC_5 */
   uint16 ChIncl;     /* not used */
   uint16 ChChanged;  /* See definition above */ 
   uint16 PriChan;    /* 0=FCH,1=DCCH, */
   uint16 FpcMode;    /* FPC_MODE, value (000,...110) */
   uint16 SchRate;    /* index:0 highest rate (4 chip), 7 lowest (1/8 rate). See definition above. */
   FpcParmsT  FpcFch;
   FpcParmsT  FpcDcch;
   FpcParmsT  FpcSch;
   uint16  SetPtThres;     /* in 0.125 dB unit. If the change in setpoint goes */
                           /* beyond this threshold, report setpoint to BS */
   uint16  SetPtThresSch;  /* in 0.125 dB unit. If the change in SCH */
                           /* setpoint goes beyond this threshold, report setpoint to BS. */
   uint16 FpcSubchanGain;  /* FPC_SUBCHAN_GAIN set by BS in unit of 0.25 dB (>=0, default=0)*/
   int16  ChCalFactor[8];  /* unit dB in Q10 format */
                           /* ChCalFactor[0] = cal for FSCH rate other than full rate */
                           /* ChCalFactor[1] = cal for FSCH full rate */
                           /* ChCalFactor[2] = cal for FSCH full rate */
                           /* ChCalFactor[3] = cal for FDCCH full rate */
                           /* ChCalFactor[4] = cal for FCH full rate */
                           /* ChCalFactor[5] = cal for FCH half rate */
                           /* ChCalFactor[6] = cal for FCH quarter rate */
                           /* ChCalFactor[7] = cal for FCH eighth rate */
} IpcDsmFpcParmMsgT;

typedef PACKED struct 
{
#if ( ((SYS_ASIC == SA_ROM) && (SYS_VERSION >  SV_REV_C3)) || (SYS_ASIC == SA_RAM) || (SYS_ASIC == SA_CBP55) )
  uint16   TestMode;              /* 0:normal op; 1:Gpc1=0; 2:sending alt bits; 3:use Pcb EbNt */
  uint16   AvgNumPcg;             /* number of PCG's for average */
  uint16   OLCoarseK1;            /* FPC outer loop coarse adj K1*1000 Q0 */
  uint16   OLCoarseK2;            /* FPC outer loop coarse adj K1 Q14 */
  uint16   OLFineC1;              /* FPC outer loop fine adj C1 Q14 */
  uint16   OLFineC2;              /* FPC outer loop fine adj C2 Q14 */
  int16    EbNtDeltaStep;         /* Step size for EbNt Delta Modulator in Rfc, dB in Q8 */
  int16    FchEbNtCalNoiseOft;    /* FPC inner loop EbNt calibration to offset finger noise, linear in Q8*/
  int16    SchEbNtCalNoiseOft[2]; /* FPC inner loop EbNt calibration to offset finger noise, linear in Q8*/
  int16    SchEbNtDtxThresTbl[5]; /* FPC SCH DTX EbNt Thres per data rate: 16x, 8x, 4x, 2x, 1x, dB in Q8 */
  int16    EbNtIIRAlpha[2];       /* FPC inner loop EbNt IIR filter [0]-FCH, [1]-SCH, alpha in Q9 */
#else
  uint16   OLCoarseK1;            /* FPC outer loop coarse adj K1*1000 Q0 */
  uint16   OLCoarseK2;            /* FPC outer loop coarse adj K1 Q14 */
  uint16   OLFineC1;              /* FPC outer loop fine adj C1 Q14 */
  uint16   OLFineC2;              /* FPC outer loop fine adj C2 Q14 */
  uint16   NtIIRCoef;             /* FPC inner loop EsNt est. IIR coef Q14 */
  uint16   NtFloor;               /* FPC inner loop EsNt est. noise floor Q0 */
  uint16   MaxEsNt;               /* FPC inner loop max EsNt value  */
  uint16   ThrCorrVal;            /* FPC inner loop EbNt correction threshold val Q8 */
  uint16   StepCorrVal;           /* FPC inner loop EbNt Step val Q8 */
  uint16   StepNumFrmBits;        /* FPC inner loop PC stepzise est. number of frames = 1<<StepNumFrmBits  */
  uint16   StepSizeDefault;       /* FPC inner loop PC stepzise default */
  uint16   StepSzEstEnable;       /* Enable step size estimation */
  uint16   TestMode;              /* 0:normal op; 1:Gpc1=0; 2:sending alt bits; 3:use Pcb EbNt */
  uint16   AvgNumPcg;             /* number of PCG's for average */
  uint16   EbNtThresSuper;        /* EbNt threshold for supervision */
  int16    PcbEbNtCal;            /* Pcb EbNt Cal value */    
  int16    SchEbNtCal;
  int16    EbNtDeltaStep;         /* Step size for EbNt Delta Modulator in Rfc, dB in Q8 */
#endif
} IpcDsmFpcAlgParmMsgT;

typedef PACKED struct 
{
   uint16  FchSetPtIncl;   /* 1 = FPC_FCH_CURR_SETPT included */
   int16   FchCurrSetPt;   /* FPC_FCH_CURR_SETPT, in 0.125 dB unit */
   uint16  DcchSetPtIncl;  /* 1 = FPC_DCCH_CURR_SETPT included */
   int16   DcchCurrSetPt;  /* FPC_DCCH_CURR_SETPT, in 0.125 dB unit */
   uint16  SchSetPtIncl;   /* 1 = FPC_SCH_CURR_SETPT included */
   int16   SchCurrSetPt;   /* FPC_SCH_CURR_SETPT, in 0.125dB unit */
}IpcCpFpcRepMsgT;

/* IpcDsmRPCParmMsgT */
typedef PACKED struct 
{
   uint16       Mode;    /* 0-disable/1-enable/2...-debug modes */
   uint16       PwrCntlStep;
   uint16       PwrCntlDelay;
} IpcDsmRPCParmMsgT; 

/* IpcDsmSlottedModeMsgT */
typedef PACKED struct 
{
   uint16       Enable;    /* enable, disable */
} IpcDsmSlottedModeMsgT; 

/* IpcDsmQuickPageMsgT */
typedef PACKED struct 
{
   uint16       Enable;
   uint16       BitPos;    /* symbol location of first bit */
} IpcDsmQuickPageMsgT; 

/* IpcDsmTrkParmMsgT time track parameters */
typedef PACKED struct 
{
   uint16       Decim;
   int16        Shift;
   uint16       A0;
   uint16       A1;
   uint16       DecimMsg;
   uint16       UnlockCnt;
} IpcDsmTrkParmMsgT; 

/*------------------------------------------------------**
** End CP to DSPM Miscellaneous Task Message Structures **
**------------------------------------------------------*/

/*---------------------------------------------**
** CP to DSPM Searcher Task Message Structures **
**---------------------------------------------*/

/* IpcDsmTestAlgValueMsgT */
typedef PACKED struct
{
   uint16   Searcher;               /* Searcher Ec/Io normalization factor */
   uint16   Finger;                 /* Finger Ec/Io normalization factor */
}NormFactorT;

typedef PACKED struct
{
   uint16   AverageNum;             /* Number of Active set searcher to complete (with averaging) 
                                       before making allocation decisions
                                    */
   uint16   Interval;               /* Number of Active set symbols searcher to complete (time lapse)
                                       before making allocation decisions
                                    */
   uint16   EcIoAbsThresh;          /* Minimum Ec/Io level at which to allocate a finger. Any 
                                       searcher or finger result which is less than this value
                                       will not be/remain allocated
                                    */
   uint16   MinOffsetDist;          /* Minimum offet distance for allocation. Fingers will not 
                                       be allocated less than this distance from each other (uints 
                                       Tc/8 to allow flexibility if this vlaue changes)
                                    */
   int16   FngrEcIoFiltConst;      /* Finger filtered power filter time constant */
   int16   FngrEcIoInstConst;      /* Finger Instant power filter time constant */
   uint16   SideLobeDist;           /* Distance from a higher power entry within which an allocation
                                       phase is considered to potentially be a filter response sidelobe
                                    */
   uint16   EcIoRelThresh;          /* Threshold relative to the power of a higher powered entry below 
                                       which  a finger will not be allocated if it has an offset less than
                                       SideLobeDist from the higher powered entry
                                    */
}FingerAllocT;

typedef PACKED struct
{
   uint16   FastSystemTimeTrackNum; /* Number of initial fast system time tracking when
                                       the first finger is allocated
                                    */
   uint16   FastTimeTrackNum;       /* Number of initial fast time tracking when
                                       a new finger is allocated */
   uint16   FgrAllocOffsetDistQ3;   /* Number of chips a lower power finger is allowed to
                              approach a higher power finger */
   uint16   TrkSysTimeMinFrames;    /* minimum number frames between track of sys time */
   uint16   TrkMinPwr;              /* minimum time tracking powr */
   int16    TimeTrackCoefR1;        /* early late gate time tracking constant */ 
   uint16   CompPn;                 /* Tc/2, Search PN compensation to align with Finger */
}TimeTrackT;

typedef PACKED struct
{
   int16   ActSetTimer;            /* Timer value for which Active Set must be searched upon expired */
   int16   PwrRptTimer;            /* Power Measurement Report Timer */  
}ListProcT;

typedef PACKED struct
{
   uint16   UsablePathCntr;             /* Usable Path Spy Cntr */
   uint16   FingerStatusCntr;           /* FingerStatus Spy Cntr */
}SchSpyT;

typedef PACKED struct
{
   NormFactorT    NormFactor;       /* Variables related to Ec/Io power normalization */
   FingerAllocT   FingerAlloc;      /* Variables related to finger allocation */
   TimeTrackT     TimeTrack;        /* Variables related to time tracking */
   ListProcT      ListProc;   
   SchSpyT        Spy;              /* Variables control spy output rate */
} IpcDsmTestAlgValueMsgT;

/* IpcDsmTestAlgValue1MsgT, in addition to TestAlgValue */
typedef PACKED struct
{
    uint16 FastPwrRptTimer;            
    uint16 QpchNeighborFastPwrRptTimer;
    uint16 SearchTimeMin;
    uint16 EarliestFngrEcIoAbsThresh;
    uint16 NoiseFloor;
    uint16 CohIaqThresh;                  
    uint16 SysNumFingers;
    uint16 SideLobeDist0;             
    uint16 SideLobeDist1;             
    uint16 SideLobeDist2;             
    uint16 SideLobeWin;             
    uint16 FgrAllocMPathThresh;
    uint16 FgrAllocSoftHOThresh;       
    uint16 MaxFingerBs;      
    uint16 MinAcqWinSize;
    int16  FgrAllocDropTimer;
    uint16 FgrAllocDropThresh;
    uint16 DecimationCntr;
    int16  FgrOffsetQ3;
    uint16 SchPwrRptCombThresh;
    uint16 Sch3RayAddFloorThresh;
    uint16 Sch3RayFloor;
    uint16 NCohIaqThresh;                  
    uint16 IaqMinPnDist;                  
#if ( ((SYS_ASIC == SA_ROM) && (SYS_VERSION >  SV_REV_C3)) || (SYS_ASIC == SA_RAM) || (SYS_ASIC == SA_CBP55) )
    int16  IaqRxPwrThresh;
#endif
} IpcDsmTestAlgValue1MsgT;


/* IpcDsmPilotAcquireMsgT */
typedef PACKED struct 
{
   uint16       CdmaBand;  /* band (1st relase always PCS) */
   uint16       FreqChan;  /* frequency channel */
   uint16       NumCycles; /* number of cycles before abort */
} IpcDsmPilotAcquireMsgT;

/* IpcDsmSearchParmMsgT */
typedef PACKED struct 
{
   uint16       DwellLen1;
   uint16       DwellLen2;
   uint16       Thresh1;
} IpcDsmSearchParmMsgT; 

/* IpcDsmSearchWinSizeMsgT */
typedef PACKED struct 
{
   uint16       ActWinSize;
   uint16       NeighWinSize;
   uint16       RemainWinSize;
} IpcDsmSearchWinSizeMsgT;



/* IpcDsmAcqParmMsgT acquisition parameters */
typedef PACKED struct 
{
   uint16       DwellLen1;     /* dwell length & thresh for 2 dwell search */
   uint16       DwellPCnt1;    
   uint16       Thresh1;
   uint16       DwellLen2;
   uint16       DwellPCnt2;    
   uint16       SortListSz;    /* sort list size */
   uint16       ThreshSort;    /* sort threshold */
   uint16       VerifWin;      /* verif window */
   uint32       ThreshVerif;   /* verif threshold */
} IpcDsmAcqParmMsgT; 




/*-------------------------------------------------------------------------------**
** IpcDsmActiveCandPilotSetMsgT                                                  **
** This message updates the DSP information about the active and candidate       **
** set basestations.                                                             **
** The dsp performs finger allocation after receiving this message.              **
**                                                                               **
** PilotWalsh                                                                    **
** [11:13] 3b - WalshLen (len = 64*2^(WalshLen))                                 **
** [9:10]  2b - QOF                                                              **
** [0:8]   9b - Walsh Id                                                         **
**-------------------------------------------------------------------------------*/
typedef PACKED struct 
{
   uint16       PilotPN;                  /* 64 Tc resolution */
   uint16       PilotPhase;               /* Chip resolution */
   uint16       PilotWalsh;               /* WalshLen, QOF & Walsh for pilot */
   uint16       PwrCombInd;
   uint16       ChnlWalsh[SYS_NUM_CHNL];  /* QOF & Walsh for channel         */
                                          /* [15]    1b - 1-Channel Disabled  */
                                          /* [9:10]  2b - QOF                 */
                                          /* [0:8]   9b - Walsh Id            */
} IpcActiveConfigT;

typedef PACKED struct {
   uint16       PilotPN;    /* 64 Tc resolution */
   uint16       PilotPhase; /* Chip resolution */
   uint16       PilotWalsh; /* WalshLen, QOF & Walsh for pilot */
} IpcCandConfigT;

typedef PACKED struct 
{
   uint16           NumPilots;
   IpcActiveConfigT Data[SYS_MAX_ACTIVE_LIST_PILOTS];
} IpcActiveListT; 

typedef PACKED struct 
{
   uint16           NumPilots;
   IpcCandConfigT   Data[SYS_MAX_CANDIDATE_LIST_PILOTS];
} IpcCandListT; 

typedef PACKED struct
{
   uint16         SeqNum;     /* Sequence number for co-ordination with Search Result Messages */
   uint16         NextFrame;  /* bit 0  0=immediate, 1=apply in frame(20ms) */ 
   IpcActiveListT Active;     /* active set list  */
   IpcCandListT   Candidate;  /* candidate set list  */
} IpcDsmActiveCandPilotSetMsgT;



/*---------------------------------------------------------------------------**
** IpcDsmNeighborPilotSetMsgT                                                **
** This message updates the DSP information about base stations in the       **
** neighbor list.                                                            **
**---------------------------------------------------------------------------*/


typedef PACKED struct 
{
   uint16       PilotPN;           /* 64 Tc resolution */
   uint16       PilotWalsh;        /* WalshLen, QOF & Walsh for pilot */
   uint16       NeighborWinData;   /* bits 0-9 Neighbor Window Size, bits 10-12 Neighbor Search Offset */
} IpcNghbrConfigT;


typedef PACKED struct 
{
   uint16           NumPilots;
   IpcNghbrConfigT  Data[SYS_MAX_NEIGHBOR_LIST_PILOTS];   
} IpcNghbrListT;

 
typedef PACKED struct
{
   uint16           SeqNum;        /* Sequence number for co-ordination with Search Result Messages */
   IpcNghbrListT    Neighbor;      /* active set list  */
   uint16           PilotInc;      /* increment for remaing set  */
} IpcDsmNeighborPilotSetMsgT;
 


/*--------------------------------------------------------------------**
** IpcDsmPriorityPilotSetMsgT                                         **
** This search preempts the normal search                             **
** The CP uses this to perform priority search                        **
** and candidate frequency search                                     **
** The choice of 3 sets is arbitrary. This lets the CP set the search **
** window for the 3 sets.                                             **
**--------------------------------------------------------------------*/
typedef PACKED struct 
{
   uint16       NumPilots;
   uint16       PilotPN[SYS_MAX_PRIORITY_LIST_PILOTS];    /* 64 Tc resolution */
   uint16       PilotWalsh[SYS_MAX_PRIORITY_LIST_PILOTS]; /* QOF & Walsh for auxiliary pilot channel */
} IpcPilotListT; 

typedef PACKED struct
{
   int16          StartSymbol;    /* symbol to start search, 0xffff = immediate */
   uint16         CdmaBand;
   uint16         FreqChan;
   uint16         NumPilotsA;
   uint16         NumPilotsB;
   uint16         NumPilotsC;
   uint16         WinSizeA;
   uint16         WinSizeB;
   uint16         WinSizeC;
   IpcPilotListT  Pilots;   /* pilot list */
} IpcDsmPriorityPilotSetMsgT; 

typedef PACKED struct
{
   uint16         QpchWalshCh;       /* Walsh Code */
   uint16         QpchRate;          /* 0 = 4800, 1 = 9600 */
   uint16         QpchSymbolBit;     /* Symbol Location */
} IpcDsmQpchMsgT; 

typedef PACKED struct
{
  uint16 Data[40];
} IpcDsmSimMsgT;

typedef PACKED struct
{
  int16 RxTxOffsetQ3;  /* required (Rx_time - Tx_time) in 1/8 chip 
                          to account for the filter delay
                        */

} IpcDsmMscParmMsgT;


/*---------------------------------------------------------------------------**
  IPC_DSM_AFLT_PILOT_SET_MSG
  IpcDsmAfltPilotSetMsgT
  This message provides the DSP the AFLT neighbor list (not regular neighbor
  list) received in the Position Determination BS Almanac message, and requests
  DSP to do pilot strength, and phase measurements on given neighbor list.
**---------------------------------------------------------------------------*/
  
typedef PACKED struct 
{
  /* individual PILOT_PN from BS ALMANAC             */
  /* first one will always be REF_PN from BS ALMANAC */
  uint16  PilotPN;
  /* individual TIME_CORRECTION from BS ALMANAC                 */
  /* first one will always be TIME_CRRCTION_REF from BS ALMANAC */
   int16 TimeCorrect;
} IpcAfltConfigT;


typedef PACKED struct 
{
   uint16          NumPilots;
   IpcAfltConfigT  Data[SYS_MAX_AFLT_LIST_PILOTS];   
} IpcAfltListT;

 
typedef PACKED struct
{
   uint16         SeqNum;
   uint16         SrchWin;
   IpcAfltListT     Aflt;
} IpcDsmAfltPilotSetMsgT;


/*---------------------------------------------------------------------------**
  IPC_DSM_AFLT_PILOT_SET_ABORT_MSG
  IpcDsmAfltPilotSetAbortMsgT
  This message requests DSP to abort the AFLT search measurements for the
  given sequence number. 
**---------------------------------------------------------------------------*/

typedef PACKED struct 
{
    uint16           SeqNum;
} IpcDsmAfltPilotSetAbortMsgT;

/*---------------------------------------------------------------------------**
   SchMsgAfltAlgValueT
   for IPC_DSM_AFLT_ALG_VALUE_MSG 
   This message reconfigures AFLT search parameters.
**---------------------------------------------------------------------------*/
typedef PACKED struct
{
   uint16 Msets;              /* valid values are [1..8] */
   uint16 CoarseDwellLen1;    /* (CoarseDwellLen1+1)*16 + (CoarseDwellLen2+1)*16 = 1024 */
   uint16 CoarseDwellLen2;    /* Usually CoarseDwellLen1 = 0xf; CoarseDwellLen2 = 0x2f */
   uint16 CoarseThresh1;
   uint16 CoarseThresh2Abs;   /* Absolute power threshold for finding earliest path */
   uint16 CoarseThresh2RelShft; /* Shift down of detected peak power for finding earliest path */
   uint16 CoarseAverageNum;
   uint16 CoarseWinSize;      /* one sided window in Tc/2 */
   uint16 FineDwellLen1;      /* (FineDwellLen1+1)*16 + (FineDwellLen2+1)*16 = 1024 */
   uint16 FineDwellLen2;      /* 1024 is used because double buffer (2048) is not functioning */ 
   uint16 FineDwellLenTotal;  /* should be 1024 */
   uint16 FineThresh1;
   uint16 FineThresh2Abs;     /* Absolute power threshold for finding earliest path */
   uint16 FineAverageNum;
   uint16 FineWinSize;        /* one sided window in Tc/2 */
   uint16 SpyPilotPN;         /* display coarse and fine search result of this PilotPN in Spy, default: 0 */
   uint16 FineThresh3Abs;     /* Absolute power threshold for finding earliest path */
} SchMsgAfltAlgValueT;


/*---------------------------------------------------------------------------**
  IPC_DSM_QUERY_TIMING_MSG
  IpcDsmQueryTimingMsgT
  This message requests DSP to send up system and finger timing information. 
**---------------------------------------------------------------------------*/

typedef PACKED struct 
{
    uint16           SeqNum;
} IpcDsmQueryTimingMsgT;

/*--------------------------------------------------------------------**
** IpcDsmCandFreqStartMsgT                                            **
** This message contains the necessary information used to initiate   **
** candidate frequency search.                                        **
**--------------------------------------------------------------------*/
typedef enum /* Enum matches L1dCfSrchModeT in <l1dapi.h> */
{
   IPC_CF_SEARCH_CDMA,
   IPC_CF_SEARCH_ANALOG
} IpcCfSrchModeT;

typedef PACKED struct
{
   uint16         CfSrchMode;
   /* Absolute time in symbol units: [0, 383] */
   uint16         TuneCfSmbNum;       
   uint16         CfPllSettleSmbNum;
   uint16         TuneSfSmbNum;
   uint16         SfPllSettleSmbNum;
} IpcDsmCandFreqStartMsgT;


/* IPC_DSM_TEST_ALG_VALUE2_MSG Message type */
typedef PACKED struct
{
    uint16    VcxoCompEnable;  /* Sloppy VCXO Compensation Mode 1: enabled */
                               /*                               0: disabled */
    uint16    AvePilotPwrReportFlg; /* 1- QPCH Average pilot power reporting */
                                    /* 0- Non-average power */
} IpcDsmTestAlgValue2MsgT;


/*-------------------------------------------------**
** End CP to DSPM Searcher Task Message Structures **
**-------------------------------------------------*/


/*******************************************************
** Message Stuctures: DSP to Control Processor (_CP_) **
*******************************************************/

/*-----------------------**
** Monitor Task Messages **
**-----------------------*/

/* Download complete msg */
typedef PACKED struct
{
   uint16       Checksum;    
} IpcCpDownCompMsgT;

/* Version response message */
typedef PACKED struct
{
   uint16       VersionMajor;              
   uint16       VersionMinor;              
   uint16       VersionPatch;
   uint16       Date[5];
} IpcCpVerRspMsgT;

/* Peek response message */
typedef PACKED struct 
{
   uint16       StartAddr;
   uint16       NumWords;
   uint16       RspTaskId; /* CP task to which the peek response should be routed */
   uint16       RspMboxId; /* CP mailbox to which the peek response should be routed */
   uint16       Data[1];
} IpcCpPeekRspMsgT;

/* Poke response message */
typedef PACKED struct
{
   uint16       StartAddr;              
   uint16       NumWords;              
} IpcCpPokeRspMsgT;

/* Bitwise Operation Message */
typedef PACKED struct
{
   uint16       OldValue;              
   uint16       NewValue;
} IpcCpBitwiseOpRspMsgT;

/* DSP Jtag Power Config Message */
typedef PACKED struct
{
   uint16       JtagPwrCfgMode;              
} IpcCpDspJtagPwrCfgMsgT;

typedef enum
{
   IPC_DSP_JTAG_PWR_OFF,
   IPC_DSP_JTAG_PWR_ON
} IpcDspJtagPwrCfgT;

/* Fault message */
typedef PACKED struct
{
   uint16       UnitId;
   uint32       SystemTime;
   uint16       FaultId1;
   uint16       FaultId2;
   uint16       FaultType;
} IpcCpFaultMsgT;

/* HW mailbox loopback response message */
typedef PACKED struct
{
   uint16       Data[1];
} IpcCpMboxLoopRspMsgT;

/* RAM test Message */
typedef PACKED struct
{
   uint16       Result;
   uint16       ErrAddr;
} IpcCpMemTestRspMsgT;

/* Spy response message */
typedef PACKED struct 
{
   uint16       SpyId;
   uint32       SystemTime;
   uint16       Data[1];
} IpcCpSpyRspMsgT;

/* Trace response message */
typedef PACKED struct 
{
   uint16       TraceId;
   uint32       SystemTime;
   uint16       Args[1];
} IpcCpTraceRspMsgT;

/* Printf message */
typedef PACKED struct 
{
   uint16       String[IPC_MAX_PRINTF_STR_SIZE];
   uint16       Args[1];
} IpcCpPrintfMsgT;

/* Heart Beat message */
typedef PACKED struct 
{
   uint16       ProcessorId;
} IpcCpHeartBeatMsgT;

/* Configuration message */
typedef PACKED struct 
{
   uint16       ProcessorId;
   uint16       ConfigDataL;
   uint16       ConfigDataH;
} IpcCpConfigMsgT;

/* Direct buffer mailbox loopback test response message */
typedef PACKED struct 
{
   uint16       Result;
} IpcCpDBufLoopbackRspMsgT;

/*--------------------------------------**
** DSPM to CP Message Data Structures   **
**--------------------------------------*/

/* IpcCpSearchResultMsgT (from engRelayPilotMeasureReport) */
typedef PACKED struct
{
   uint16       PnPhase;        /* pilot pn relative to zero offset pilot */
   uint16       Strength;       /* pilot power in Q16 linear units -- Bit 15 is Stale Bit 1=Stale */
} PilotMeasurementT;

typedef PACKED struct
{
   uint16              SeqNum;        /* Sequence nuumber for co-ordination with list messages*/
   uint16              NumActive;    /* number of pilots in active list */
   uint16              NumCandidate; /* number of pilots in candidate list */
   uint16              NumNeighbor;  /* number of pilots in neighbor list */
   uint16              NumRemaining; /* number of pilots in remaining list */
   PilotMeasurementT   ActiveList[SYS_MAX_ACTIVE_LIST_PILOTS];
   PilotMeasurementT   CandidateList[SYS_MAX_CANDIDATE_LIST_PILOTS];
   PilotMeasurementT   NeighborList[SYS_MAX_NEIGHBOR_LIST_PILOTS];
   PilotMeasurementT   RemainingList[SYS_MAX_REMAINING_LIST_PILOTS];
   uint16              EarliestPilotPN;    /* Earliest finger's pilot pn 64Tc */
   int16               SysTimeOffsetQ3;    /* total system time offset in Tc/8 */
} IpcCpSearchResultMsgT;
 

/*---------------------------------------------------------------------------**
  IPC_CP_AFLT_MEAS_MSG
  IpcCpAfltMeasurementsMsgT
  This message is returned from the after the CP requests DSP to do pilot
  strength, and phase measurements on given AFLT neighbor list.
**---------------------------------------------------------------------------*/

typedef PACKED struct
{
   /* PnPhaseREF_PN is PnPhase returned by DSP of  REF_PN received in BS ALMANAC */
   /* ((PilotPN * 64 * 8) + (PnPhase - PnPhaseREF_PN) ) * 2 = PILOT_PN_PHASE     */
   /*  in 1/16 chip unit sent in PROVIDE PILOT PHASE MEASUREMENTS                */
   uint16       PilotPN;
   /* correction relative to PilotPN in 1/8 chip                  */
   int16        PnPhaseOffset;
   /* | -2 * 10 log10 pilot strength | = PILOT_STRENGTH           */   
   /*  sent in PROVIDE PILOT PHASE MEASUREMENTS                   */
   /* Pilot strength in Q16 linear units (Bits 14-0)              */
   /* Stale (Bit 15) 1=Stale                                      */
   uint16       Strength; 
} AfltMeasurementT;

typedef PACKED struct
{
   uint16              SeqNum;
   /* REF_PN sent in PROVIDE PILOT PHASE MEASUREMENTS                */
   /* REF_PN's pilot strength which is found in                      */
   /*  either in AFLT pilot list or ACTIVE pilot list = REF_STRENGTH */ 
   /*  sent in PROVIDE PILOT PHASE MEASUREMENTS                      */
   uint16              ReferencePN;
   /* SUPERFRAME = SYSTEM TIME / 4                                     */
   /* ( ( TimingCnt / (4 * 384) ) / 80mS ) + SUPERFRAME = TIME_REF_MS  */
   /*  sent in PROVIDE PILOT PHASE MEASUREMENTS                        */
   /* symbol number of last searcher buffer capture (0 to 0x5FF)       */
   uint16              TimingCnt; 
   uint16              NumAflt;
   AfltMeasurementT    AfltList[SYS_MAX_AFLT_LIST_PILOTS];
} IpcCpAfltMeasurementsMsgT;


/*---------------------------------------------------------------------------**
  IPC_CP_TIMING_RSP_MSG
  IpcCpTimingRspMsgT
  This message is a response to IPC_DSM_QUERY_TIMING_MSG.
  It returns the DSM system and finger timing information to CP.
**---------------------------------------------------------------------------*/

typedef PACKED struct
{
   uint16 PilotPN;            /* Base Station Pilot PN */
   int16  OffsetQ3;           /* Finger Offset from Base in Tc/8 */
   uint16 Power1[3];          /* power of short term finger pilot filter: ontime, early, late */
   uint16 Power2;             /* power of long term finger pilot filter */
} FingerTmT;

typedef PACKED struct
{
   int16      SysTimeOffsetQ3;/* cumulative sum of system time adjustments in Tc/8 */
   FingerTmT  FingerTm[4];
   uint16     FingerStat;     /* [11:8] latest, [7:4] strongest, [3:0] earliest */
} IpcCpTimingRspMsgT;


/* IpcCpFwdChnlDtaMsgT */
typedef PACKED struct 
{
   uint16       ChnlRdy;      /* bit field indicating which channels are ready */
   uint16       ChnlDataQlty; /* bit field indicating channels are good (CRC or rate decn) */
   uint16       FundRate;     /* fund chnl rate if assigned */
   uint16       ChnlType;     /* channel type, FwdChnlTypeT, 4 bits per channel */
                              /* [0:3] channel 0 type */
                              /* [4:7] channel 1 type */
                              /* [8:11] channel 2 type */
                              /* [12:15] channel 3 type */
   uint16       ChnlSize[4];  /* channel size, 1 word per channel */
                              /* ChnlSize[0]: channel 0 size, in bits */
                              /* ChnlSize[1]: channel 1 size, in bits */
                              /* ChnlSize[2]: channel 2 size, in bits */
                              /* ChnlSize[3]: channel 3 size, in bits */
} IpcCpFwdChnlDtaMsgT;
 
/* IpcCpFwdChnlPcRspMsgT */
typedef PACKED struct 
{
   uint16 Enabled;             /* Indicates the current value of the forward channel puncture state.
                               */
} IpcCpFwdChnlPcRspMsgT;

 
/* IpcCpFwdChnlScrambleRspMsgT */
typedef PACKED struct 
{
   uint16 Enabled;             /* Indicates the current value of the forward channel scramble state
                               */
} IpcCpFwdChnlScrambleRspMsgT;

/* IpcCpRxPowerMsgT */
typedef enum
{
   IPC_SERVING_FREQ_RX_PWR,
   IPC_CANDIDATE_FREQ_RX_PWR
} IpcRxPwrReportT;

typedef PACKED struct 
{
   int16   RxPowerdBm;      /* Q=7 */
   uint16  RxPwrReportType;
   int16   TxPowerdBm;
} IpcCpRxPowerMsgT; 

/*-----------------------------------------------------------------------**
** IpcCpDcBiasMsgT                                                       **
** Reports the DC bias in case there external calibration which needs to **
** be performed                                                          **
**-----------------------------------------------------------------------*/
typedef PACKED struct 
{
   int16        DcI;   /* codewords Q4 */
   int16        DcQ;   /* codewords Q4 */
} IpcCpDcBiasMsgT; 

/* IpcCpQuickPageBitsMsgT  */
typedef PACKED struct 
{
   uint16       Bits;   /* bit0, bit1 */
   int16        SBit0;  /* scalar bit0 */
   int16        SBit1;  /* scalar bit1 */
} IpcCpQuickPageBitsMsgT; 

/*--------------------------------------**
** DSPV to CP Message Data Structures   **
**--------------------------------------*/

/* Size of mailbox in uint16 */
#define IPC_MAILBOX_DSPV_SIZE    0x100
#define IPC_MAILBOX_MAX_SIZE     0x200
#define IPC_MAILBOX_MIN_SIZE       6

/* Max msg data size in uint16 i.e.  - mailbox header & first message header */
#define IPC_MSG_DSPV_MAX_SIZE    (IPC_MAILBOX_DSPV_SIZE - IPC_MBOX_HEADER_SIZE  - IPC_MSG_HEADER_SIZE)

/*----------------------------**
** Voice And Audio Processing **
**----------------------------*/

#define  IPC_SPCH_PCKT_LEN_MAX      18
#define  IPC_RS_PCKT_LEN_MAX        5

typedef enum
{
   IPC_FIF_MODE_OFF = 0,
   IPC_FIF_MODE_ON  = 1
} IpcFrontendFilterModeT;

typedef enum
{
   IPC_NOISE_SUPPRESS_OFF = 0,
   IPC_NOISE_SUPPRESS_ON  = 1
} IpcNoiseSuppressModeT;

typedef enum
{
   IPC_BIF_MODE_OFF = 0,
   IPC_BIF_MODE_ON  = 1
} IpcBackendFilterModeT;

typedef enum
{
   /* Q8 format, 0.75db per step */
   IPC_VOLSCALE_MUTE   = 0x0000, /* fully mute */
   IPC_VOLSCALE_0_00DB = 0x0100, /* 0db, DEFAULT */
   IPC_VOLSCALE_0_75DB = 0x00EB, /* 0.75db */
   IPC_VOLSCALE_1_50DB = 0x00D7, /* 1.50db */
   IPC_VOLSCALE_2_25DB = 0x00C6  /* 2.25db */
} IpcVolScaleFactorT;

typedef enum
{
   IPC_AUDIO_AGC_OFF = 0,
   IPC_AUDIO_AGC_ON  = 1
} IpcAudioAgcModeT;

typedef enum
{
   IPC_AUDIO_ACP_COMP_OFF = 0,
   IPC_AUDIO_ACP_COMP_ON  = 1
} IpcAudioAcpCompModeT;

typedef enum
{
   IPC_AUDIO_ACP_ASVC_OFF = 0,
   IPC_AUDIO_ACP_ASVC_ON  = 1
} IpcAudioAcpAsvcModeT;


#define IPC_FRAC_PITCH_OFF 0
#define IPC_FRAC_PITCH_ON  1

#define IPC_LPC_IR_MAX_LEN 20
#define IPC_LPC_IR_MIN_LEN 0

#define  IPC_LPC_IR_OPT_EN_OFF FALSE
#define  IPC_LPC_IR_OPT_EN_ON  TRUE

#define  IPC_FCB_SUB_SAMP_EN_OFF  FALSE
#define  IPC_FCB_SUB_SAMP_EN_ON   TRUE

typedef enum
{
   IPC_PITCH_PRE_FLTR_OFF = 0,
   IPC_PITCH_PRE_FLTR_ON  = 1
} IpcPitchPreFltrModeT;

typedef enum
{
   IPC_SYN_POST_FLTR_OFF = 0,
   IPC_SYN_POST_FLTR_ON  = 1
} IpcPostSynFltrModeT;

typedef enum
{
   IPC_AIR_INTERFACE_MODE_CDMA = 0,
   IPC_AIR_INTERFACE_MODE_AMPS = 1
} IpcAirInterfaceModeT;

#define  IPC_SSO_CTRL_WRD_SIZ_MAX   1
#define  IPC_SSO_CTRL_RRM_MSK       0x00E0
#define  IPC_SSO_CTRL_RRM_SHFT      5
#define  IPC_SSO_CTRL_RSVD_MSK      0x001C
#define  IPC_SSO_CTRL_RSVD_SHFT     2
#define  IPC_SSO_CTRL_M2M_MSK       0x0002
#define  IPC_SSO_CTRL_M2M_SHFT      1
#define  IPC_SSO_CTRL_INIT_MSK      0x0001
#define  IPC_SSO_CTRL_INIT_SHFT     0

typedef enum 
{
   IPC_RS1_RRM_LEVEL0 = 0,    /* 9.6 kbps average encoding rate */
   IPC_RS1_RRM_LEVEL1 = 1,    /* 8.4 kbps average encoding rate */
   IPC_RS1_RRM_LEVEL2 = 2,    /* 7.2 kbps average encoding rate */
   IPC_RS1_RRM_LEVEL3 = 3,    /* 6.0 kbps average encoding rate */
   IPC_RS1_RRM_LEVEL4 = 4     /* 4.8 kbps average encoding rate */
} IpcRs1RrmLevelT;

typedef enum 
{
   IPC_RS2_RRM_LEVEL0 = 0,    /* 14.4 kbps average encoding rate */
   IPC_RS2_RRM_LEVEL1 = 1,    /* 12.2 kbps average encoding rate */
   IPC_RS2_RRM_LEVEL2 = 2,    /* 11.2 kbps average encoding rate */
   IPC_RS2_RRM_LEVEL3 = 3,    /*  9.0 kbps average encoding rate */
   IPC_RS2_RRM_LEVEL4 = 4     /*  7.2 kbps average encoding rate */
} IpcRs2RrmLevelT;

typedef enum 
{
   IPC_M2M_PROC_OFF = 0,
   IPC_M2M_PROC_ON  = 1
} IpcM2mModeT;

typedef enum 
{
   IPC_INIT_CODEC_OFF = 0,
   IPC_INIT_CODEC_ON  = 1
} IpcInitCodecModeT;

typedef enum
{
    IPC_DISABLE_LPBK               = 0, /* all loopback modes disabled */
    IPC_MIXED_SIGNAL_HW_LPBK       = 1, /* mixed-signal hw loopback mode enabled */
    IPC_DIGITAL_HW_LPBK            = 2, /* digital hw loopback mode enabled */
    IPC_SAMPLE_ISR_LPBK            = 3, /* sample isr loopback mode enabled */
    IPC_SSO_LPBK                   = 4, /* speech service option (vocoder) loopback mode enabled */
   IPC_AMP_COMPEXP_LPBK           = 5, /* AMPS compressor-expander loopback mode enabled */
   IPC_AMP_POST_DL_TO_LPF_LPBK    = 6, /* AMPS post-deviation limiter to lowpass filter loopback mode enabled */
    IPC_MULTIPLEX_SUBLAYER_LPBK    = 7, /* multiplex sublayer loopback mode enabled */
   IPC_MPP_SPP_LPBK               = 8, /* Mpp to Spp voice loopback without any audio processing */
   IPC_AMP_PRE_COMP_POST_EXP_LPBK = 9  /* AMPS pre-compressor to post-expander loopback */
} IpcAudioLoopBackModeT;

typedef enum
{
   IPC_HANDSFREE_OFF = 0,
   IPC_HANDSFREE_ON  = 1
} IpcHandsFreeModeT;

typedef enum 
{
   IPC_ECHO_CANCEL_OFF     = 0,
   IPC_ECHO_CANCEL_ON      = 1

} IpcEchoCancelModeT;


typedef enum
{
   IPC_RECORD_PLAYBACK_DISABLED    = 0,
   IPC_VOICE_RECORD_MODE           = 1,
   IPC_VOICE_PLAYBACK_MODE         = 2,
   IPC_VOICE_RECORD_PLAYBACK_MODE  = 3
} IpcVoiceRecordPlaybackModeT;

typedef enum
{
   IPC_VOICE_PACKETS_ONLY      = 0,
   IPC_VOICE_PCM_ONLY          = 1,
   IPC_VOICE_PACKETS_PCM_BOTH  = 2
} IpcVoiceRecPlayDataTypeT;

typedef enum
{
   IPC_VOICE_REC_SEL_PATH_REV       = 0,
   IPC_VOICE_REC_SEL_PATH_FWD       = 1
} IpcVoiceRecSelPathModeT;

typedef enum
{
   IPC_VOICE_MUTE_AT_PLAYBACK        = 0,
   IPC_VOICE_MIXING_AT_PLAYBACK      = 1
} IpcVoiceRecPlaybackVoiceMixT;

typedef enum
{
   IPC_TTY_OFF = 0,
   IPC_TTY_ON  = 1
} IpcTtyModeT;

/*------------------------**
** Monitor and Diagnostic **
**------------------------*/
/* Mailbox size msg */
typedef PACKED struct
{
   uint16       ProcessorId;
   uint16       DspHwMboxUplinkSize;    
   uint16       DspHwMboxDownlinkSize;    
} IpcCpHWMboxCfgParamsMsgT;

/* This is the dynamic mailbox test message */
typedef PACKED struct
{
   uint16       ProcessorId;
   uint16       DspHwMboxUplinkSize;    
   uint16       DspHwMboxDownlinkSize;    
} IpcDspMboxSizeMsgT;

/*** ======================== ***/
/*** Microphone Path Messages ***/
/*** ======================== ***/

typedef PACKED struct
{
   uint16   MicFifMode;             /* front-end IIR filter mode switch */
} IpcDsvSetMicFifModeMsgT;

typedef PACKED struct
{
   uint16   MicFifNumBqSects;       /* number of biquad filter sections */
   uint16   MicFifBqCoefs[1];       /* starting address of filter coefficients */
} IpcDsvSendMicFifCfgParamsMsgT;

typedef PACKED struct
{
   uint16   MicAnsMode;             /* acoustic noise suppression mode switch */
} IpcDsvSetMicAnsModeMsgT;


typedef PACKED struct
{
   uint16   AnsVmThres;
   uint16   AnsTceThres;
   uint16   AnsSpecDevThres;
   uint16   AnsCntThres;
   uint16   AnsHysCntThres;
   uint16   AnsStckCntrThres;
} IpcDsvSendMicAnsCfgParamsMsgT;

typedef PACKED struct
{
   uint16   MicBifMode;             /* back-end IIR filter mode switch */
} IpcDsvSetMicBifModeMsgT;

typedef PACKED struct
{
   uint16   MicBifNumBqSects;       /* number of biquad filter sections */
   uint16   MicBifBqCoefs[1];       /* starting address of filter coefficients */
} IpcDsvSendMicBifCfgParamsMsgT;

typedef PACKED struct
{
   uint16   MicVolScalFctr;         /* software volume scale factor */
} IpcDsvSendMicVolScalFctrMsgT;

typedef PACKED struct
{
   uint16   AmpsRevVolScalFctr;         /* software volume scale factor */
} IpcDsvSendMicAmpsRevVolScalFctrMsgT;


typedef PACKED struct
{
   uint16   MicAgcMode;             /* microphone AGC mode switch */
} IpcDsvSetMicAudioAgcModeMsgT;

typedef PACKED struct
{
   uint16   tbd;
} IpcDsvSendMicAudioAgcCfgParamsMsgT;

/* OBSOLETE MSG INTERFACE - Can delete once CP software has been updated */
typedef PACKED struct
{
   int16    PoleCoefLo;                /* low frequency tone pole coefficient */
   int16    VolLo;                     /* low frequency tone volume coefficient */
   int16    PoleCoefHi;                /* high frequency tone pole coefficient */
   int16    VolHi;                     /* high frequency tone volume coefficient */
   uint16   SeqOnTime;                 /* tone sample sequence on time (in samples) */
   uint16   MuteVoice;                 /* tone sequence with/without voice control switch */
   uint16   ContSeq;                   /* tone sequence on time control switch */
} IpcDsvSendMicPlmnToneGenParamsMsgT;

typedef PACKED struct
{
   uint16   MicVoiceEncMaxRate;     /* maximum voice encode rate for connected SSO */
} IpcDsvSetMicVoiceEncMaxRateMsgT;

typedef PACKED struct
{
   uint16   MicVoiceEncMinRate;     /* minimum voice encode rate for connected SSO */
} IpcDsvSetMicVoiceEncMinRateMsgT;

typedef PACKED struct
{
   uint16   MicVoiceEncFracPitchMode;     /* voice encode fractional pitch search mode switch */
} IpcDsvSetMicVoiceEncFracPitchModeMsgT;

typedef uint16 IpcLpcIROptEnT;
typedef uint16 IpcLpcIRMaxLenT;
typedef uint16 IpcLpcIRMinLenT;
typedef uint16 IpcLpcIRPctPwrT;
typedef uint16 IpcFcbSubSampEnT;
typedef uint16 IpcFcbSubSampParmT;
typedef uint16 IpcFracPitchModeT;

typedef PACKED struct
{
    IpcLpcIROptEnT  LpcIROptEn;
    IpcLpcIRMaxLenT LpcIRMaxLen;
    IpcLpcIRMinLenT LpcIRMinLen;
    IpcLpcIRPctPwrT LpcIRPctPwr;
} IpcDsvSetLpcIROptMsgT;

typedef PACKED struct
{
    IpcFcbSubSampEnT FcbSubSampEn;
    IpcFcbSubSampParmT  FcbSubSampParm;

} IpcDsvSetFcbSubSampMsgT;

typedef PACKED struct 
{
   uint16 MicSimMsgId;              /* identification number of simulated message */
   PACKED union 
   {
      IpcDsvSetMicFifModeMsgT                 SetMicFifModeMsg;
      IpcDsvSendMicFifCfgParamsMsgT           SendMicFifCfgParamsMsg;
      IpcDsvSetMicAnsModeMsgT                 SetMicAnsModeMsg;
      IpcDsvSendMicAnsCfgParamsMsgT           SendMicAnsCfgParamsMsg;
      IpcDsvSetMicBifModeMsgT                 SetMicBifModeMsg;
      IpcDsvSendMicBifCfgParamsMsgT           SendMicBifCfgParamsMsg;
      IpcDsvSendMicVolScalFctrMsgT            SendMicVolScalFctrMsg;
      IpcDsvSendMicVolScalFctrMsgT            SendMicAmpsRevVolScalFctrMsg;
      IpcDsvSetMicAudioAgcModeMsgT            SetMicAudioAgcModeMsg;
      IpcDsvSendMicAudioAgcCfgParamsMsgT      SendMicAudioAgcCfgParamsMsg;
      IpcDsvSendMicPlmnToneGenParamsMsgT      SendMicPlmnToneGenParamsMsg;
      IpcDsvSetMicVoiceEncMaxRateMsgT         SetMicVoiceEncMaxRateMsg;
      IpcDsvSetMicVoiceEncMinRateMsgT         SetMicVoiceEncMinRateMsg;
      IpcDsvSetMicVoiceEncFracPitchModeMsgT   SetMicVoiceEncFracPitchModeMsg;
   } Msg;
} IpcDsvSendMicSimMsgMsgT;

/*** ===================== ***/
/*** Speaker Path Messages ***/
/*** ===================== ***/

typedef enum
{
   IPC_DSV_AUDIO_CFG_DAC_DIFF = 0x0001,
   IPC_DSV_AUDIO_CFG_DAC_SE   = 0x0002,
   IPC_DSV_AUDIO_CFG_EDAI     = 0x0004,
   IPC_DSV_AUDIO_CFG_PDM_DIFF = 0x0008,
   IPC_DSV_AUDIO_CFG_PDM_SE   = 0x0010,
   IPC_DSV_AUDIO_CFG_PWM_DIFF = 0x0020,
   IPC_DSV_AUDIO_CFG_PWM_SE   = 0x0040,
   IPC_DSV_AUDIO_CFG_I2S      = 0x0080
} IpcDsvAudioCfgEnum;

typedef PACKED struct
{
   uint16   SpkrPath;  /* Use IpcDsvAudioCfgEnum */
} IpcDsvSendAudioPathCfgMsgT;

typedef   enum
{
   SSO_STATUS_SUCCESS           =0,
   SSO_STATUS_FAIL              =1
}AudioSsoStatusT ;

typedef PACKED struct
{
   uint16 Status;                     
} IpcCpAudioSsoConnectRspMsgT;

typedef PACKED struct
{
   uint16 Status;                     
} IpcCpAudioSsoDisConnectRspMsgT;

typedef PACKED struct
{
   uint16    SpkrPath;
   uint16    Status;                     
} IpcCpAudioPathCfgRspMsgT;

/* DSP I2S delay mode Message */
typedef PACKED struct
{
   uint16       I2sDelayMode;
} IpcCpDsvI2sDelayModeMsgT;

typedef enum
{
   IPC_DSP_I2S_NO_DELAY,
   IPC_DSP_I2S_1_DELAY
} IpcCpDsvI2sDelayModeT;

/*** ============================= ***/
/*** Polyphonic Ringer Definitions ***/
/*** ============================= ***/

/*
 *    Ringer shared memory data format.
 */
typedef struct        /* 20 16-bit words total size */
{
   uint16 State;      /* Use VoiceStateT enum */

   struct
   {
       uint16 AmPmMultLevels;  /* KSL(15:14)|DAM(13)|DVB(12)|AM(7)|VIB(6)|EGT(5)|KSR(4)|MULT(3:0) */
       uint16 EnvlpRates;      /* RR(11:8)|AR(7:4)|DR(3:0) */
       uint16 BlockFnum;       /* Block(14:12)|FNUM(9:0)   */
       int16  TotalLevel;      /* Envelop of total level   */
       int16  SustainLevel;    /* Envelop of sustain level */
       uint16 WaveForm;        /* Up to 8 waveforms to choose from */
   } Opt[2];

   uint16 KOnNtsFbAlgSelect;   /* KOn(9)|NTS(8)|FB(7:5)|Algo(4:0) */
   uint16 RChlLChlLevels;      /* 0:255 represents ampl of 0 to 1 */
   uint16 Alpha;               /* Alpha(15:0) Make 16 bits for the highest resolution
                                  so we can play with it */
   uint16 PitchBendQ;          /* Pitch Bend Q14 */
   uint16 ReleaseEnvLevel;     /* Reporting envelop of voice during Release only */
} IpcDsvRngrSharedMemT;

/* Ringer Voice States */

#define IPC_RINGER_UNALLOC_VOICE_STATE      0   /* Empty voice bin -- Set by DSV or CP */
#define IPC_RINGER_START_VOICE_STATE        1   /* Start note, init w/ voice info -- set by CP */
#define IPC_RINGER_PROCESS_VOICE_STATE      2   /* Voice Processing -- set by DSV */
#define IPC_RINGER_RELEASE_VOICE_STATE      3   /* Key off: release and start fade -- set by DSV or CP */
#define IPC_RINGER_DEALLOCATE_VOICE_STATE   4   /* Bad note Frequency, do not process -- set by DSV */
#define IPC_RINGER_LOCK_VOICE_STATE         5   /* Don't use this bin for new voices; this is a
                                                 * temporary lock-out state when DSPV short on MIPS 
                                                 * set by CP */


/**************************
    IPC_DSV_RNGR_INIT_MSG
***************************/

/* RngrCtrlReg bit definitions */
/* --------------------------- */

/* Bit [2:0] SampleRate */
   /* to be defined */

/* Bit [3] RingerEnable */
#define IPC_RNGR_ENABLED             (0<<3)
#define IPC_RNGR_DISABLED            (1<<3)

/* Bit [4] I2SEnable */
#define IPC_RNGR_I2S_DISABLED        (0<<4)
#define IPC_RNGR_I2S_ENABLED         (1<<4)

/* Bit [5] PWMEnable */
#define IPC_RNGR_PWM_DISABLED        (0<<5)
#define IPC_RNGR_PWM_ENABLED         (1<<5)

/* Bit [6] PDM0Enable */
#define IPC_RNGR_PDM0_DISABLED       (0<<6)
#define IPC_RNGR_PDM0_ENABLED        (1<<6)

/* Bit [7] PDM1Enable */
#define IPC_RNGR_PDM1_DISABLED       (0<<7)
#define IPC_RNGR_PDM1_ENABLED        (1<<7)

/* Bit [8] BufferSelectLeft */
#define IPC_RNGR_BUFFSEL_RT          (0<<8)
#define IPC_RNGR_BUFFSEL_LFT         (1<<8)

/* Bit [9] SODDelay */
#define IPC_RNGR_NO_DELAY            (0<<9)
#define IPC_RNGR_DELAY               (1<<9)

/* Bit [10] SOIInvert */
#define IPC_RNGR_SOI_NO_INV          (0<<10)
#define IPC_RNGR_SOI_INV             (1<<10)

/* Bit [11] Mono */
#define IPC_RNGR_MONO_DISABLED       (0<<11)
#define IPC_RNGR_MONO_ENABLED        (1<<11)

/* Bit [12] PwmDiffOrSE */
#define IPC_RNGR_PWMDIFFSE_DISABLED  (0<<12)
#define IPC_RNGR_PWMDIFFSE_ENABLED   (1<<12)

/* Bit [13] SMPRepeatEnable */
#define IPC_RNGR_SMP_NO_REPEAT       (0<<13)
#define IPC_RNGR_SMP_REPEAT          (1<<13)

/* Bit [14] MaxSmpCntrlRngr */
#define IPC_RNGR_MIN_SMP_RINGER      (0<<14)
#define IPC_RNGR_MAX_SMP_RINGER      (1<<14)

/* Bit [15] FifoPtrReset */
#define IPC_RNGR_WFIFO_PTR_NO_RST    (0<<15)
#define IPC_RNGR_WFIFO_PTR_RST       (1<<15)

typedef enum
{
   IPC_DSV_NO_DTMF       = 0x0000,
   IPC_DSV_DTMF_SPKR     = 0x0001,      /* bit 0 */
   IPC_DSV_DTMF_MIC      = 0x0002,      /* bit 1 */
   IPC_DSV_DTMF_MIC_SPKR = 0x0003       /* bit 0 AND bit 1 (DSPV uses bit fields) */
} IpcDsvDtmfPathEnum;

typedef PACKED struct
{
    uint16 MaxNumVoices;            /* 1:64 */
    uint16 SampleRate;              /* kHz */
    uint16 RngrCtrlReg;             /*  SmpMaxCtrl(14)|PwmSmpMode(13)|PwmDiffSE(12)|Mono(11)|
                                        LfRtInver(10)|DataDelay(9)|BuffSel(8)|PdmEn1(7)|PdmEn0(6)
                                        PwmEn(5)|I2SEn(4)|RingerEnable(3)|SampleRate(2:0) */ 
    uint16 DtmfPath;                /* DTMF Path: Use IpcDsvDtmfPathEnum */
} IpcDsvRngrInitMsgT;


/*   IPC_DSV_RNGR_CONFIG_MSG */
typedef PACKED struct
{
    uint16 BlockSzMs;               /*  milliSeconds                        */
    uint16 RampTime;                /*  Ramp Time for PWM                   */
    uint16 CntrlField;               /*  CntrlField: ON=1, OFF=0             */
                                    /*   Interpolation filter        [0]    */
} IpcDsvRngrConfigMsgT;             /*   WaveTable Accelerator       [1]    */
                                    /*   Noise Shaping Filter        [2]    */
                                    /*   Dither Generator            [3]    */
                                    /*   10/11bit resolution PDM/PWM [4]    */
                                    /*   HwTest--write out data      [5]    */

/* Ringer Control Fields Definition */
#define IPC_RNGR_INTRP_FLTR_BN          0
#define IPC_RNGR_WAVTBL_ACCELR_BN       1
#define IPC_RNGR_NOISE_SHAPE_FLTR_BN    2
#define IPC_RNGR_DITHER_FLTR_BN         3
#define IPC_RNGR_PDM_HIGHBIT_RESL_BN    4 
#define IPC_RNGR_HW_TEST_BN             5  

#define IPC_RNGR_INTRP_FLTR         (1<<IPC_RNGR_INTRP_FLTR_BN)
#define IPC_RNGR_WAVTBL_ACCELR      (1<<IPC_RNGR_WAVTBL_ACCELR_BN)
#define IPC_RNGR_NOISE_SHAPE_FLTR   (1<<IPC_RNGR_NOISE_SHAPE_FLTR_BN)
#define IPC_RNGR_DITHER_FLTR        (1<<IPC_RNGR_DITHER_FLTR_BN)
#define IPC_RNGR_PDM_HIGHBIT_RESL   (1<<IPC_RNGR_PDM_HIGHBIT_RESL_BN)  
                                            /* 10-bit resolution if OFF */
                                            /* 11-bit resol (HIGH) if ON */
#define IPC_RNGR_HW_TEST            (1<<IPC_RNGR_HW_TEST_BN)  

/*   IPC_DSV_SEND_RNGR_VOL_SCAL_FCTR_MSG */
typedef PACKED struct
{
   uint16   RngrVolScalFctr;        /* software volume scale factor */
} IpcDsvSendRngrVolScalFctrMsgT;

typedef PACKED struct
{
   uint16   VolScalFctr;        /* software volume scale factor */
} IpcDsvSendAppVolScalFctrMsgT;

typedef enum
{
   IPC_DV_RNGR_ON = 0,
   IPC_DV_RNGR_OFF,
   IPC_DV_RNGR_RELEASE,
   IPC_DV_RNGR_ABORT
} IpcDsvRngrCntrlModeEnum;

/*   IPC_DSV_RNGR_MODE_MSG */
typedef PACKED struct
{
    uint16 RngrCtrlMode;   /* bit field: of type IpcDsvRngrCntrlModeEnum */
} IpcDsvRngrModeMsgT;

/*   IPC_CP_DV_RNGR_STATUS_MSG */
typedef PACKED struct
{
    uint16 RngrStatus;     /* bit field: of type IpcDsvRngrCntrlModeEnum: 0:on,1=Off */
} IpcDsvRngrStatusMsgT;

/********* End of POLYPHONIC RINGER definitions ************/

typedef PACKED struct
{
   uint16   SpkrVoiceFwdChPcktRate;    /* forward channel voice packet rate */
   uint16   SpkrVoiceFwdChPcktSize;    /* forward channel voice packet size (in x16 words) */
   uint16   SpkrVoiceFwdChPcktData[1]; /* forward channel voice packet data */
} IpcDsvSendSpkrVoiceFwdChPcktDataMsgT;

typedef PACKED struct
{
   uint16   SpkrVoiceDecPitchPreFltrMode;  /* voice decode pitch pre-filter mode switch */
} IpcDsvSetSpkrVoiceDecPitchPreFltrModeMsgT;

typedef PACKED struct
{
   uint16   SpkrVoiceDecSynPostFltrMode;  /* voice decode synthesis post-filter mode switch */
} IpcDsvSetSpkrVoiceDecSynPostFltrModeMsgT;

typedef PACKED struct
{
   uint16   SpkrVoicePlayPcktRate;    /* voice record playback packet rate */
   uint16   SpkrVoicePlayPcktSize;    /* voice record playback packet size (in x16 words) */
   uint16   SpkrVoicePlayPcktData[1]; /* voice record playback packet data */
} IpcDsvSendSpkrVoicePlayPcktDataMsgT;

/* OBSOLETE MSG INTERFACE - Can delete once CP software has been updated */
typedef PACKED struct
{
   int16    PoleCoefLo;             /* low frequency tone pole coefficient */
   int16    VolLo;                  /* low frequency tone volume coefficient */
   int16    PoleCoefHi;             /* high frequency tone pole coefficient */
   int16    VolHi;                  /* high frequency tone volume coefficient */
   uint16   SeqOnTime;              /* tone sample sequence on time (in samples) */
   uint16   MuteVoice;              /* tone sequence with/without voice control switch */
   uint16   ContSeq;                /* tone sequence on time control switch */
} IpcDsvSendSpkrPlmnToneGenParamsMsgT;

/* OBSOLETE MSG INTERFACE - Can delete once CP software has been updated */
typedef PACKED struct
{
   int16    PoleCoefLo;             /* low frequency tone pole coefficient */
   int16    VolLo;                  /* low frequency tone volume coefficient */
   int16    PoleCoefHi;             /* high frequency tone pole coefficient */
   int16    VolHi;                  /* high frequency tone volume coefficient */
   uint16   SeqOnTime;              /* tone sample sequence on time (in samples) */
   uint16   MuteVoice;              /* tone sequence with/without voice control switch */
   uint16   ContSeq;                /* tone sequence on time control switch */
} IpcDsvSendSpkrRngrToneGenParamsMsgT;

typedef PACKED struct
{
   uint16   SpkrFifMode;            /* front-end IIR mode switch */
} IpcDsvSetSpkrFifModeMsgT;

typedef PACKED struct
{
   uint16   SpkrFifNumBqSects;      /* number of biquad filter sections */
   uint16   SpkrFifBqCoefs[1];      /* starting address of filter coefficients */
} IpcDsvSendSpkrFifCfgParamsMsgT;

typedef PACKED struct
{
   uint16   SpkrVolScalFctr;        /* software volume scale factor */
} IpcDsvSendSpkrVolScalFctrMsgT;

typedef PACKED struct
{
   uint16   AmpsFwdVolScalFctr;        /* software volume scale factor */
} IpcDsvSendSpkrAmpsFwdVolScalFctrMsgT;


/*** =========================== ***/
/*** General Audio Path Messages ***/
/*** =========================== ***/

typedef PACKED struct
{
   uint16   AudioAirInterfaceMode;  /* air interface mode switch */
} IpcDsvSetAudioAirInterfaceModeMsgT;

typedef PACKED struct
{
   uint16   AudioSpchSrvcOpt;          /* speech service option connection type */
   uint16   AudioSpchSrvcOptMaxRate;   /* maximum voice encode rate */
} IpcDsvSendAudioSsoConnectMsgT;

typedef PACKED struct
{
   uint16   RoutingDisabled;           /* TRUE if speech routing disabled */
} IpcDsvSendAudioSpchRoutingMsgT;

typedef PACKED struct
{
   uint16   AudioSsoCtrlRrmLevel;      /* rate reduced mode bit-field */
   uint16   AudioSsoCtrlM2mMode;       /* mobile-to-mobile mode bit-field */
   uint16   AudioSsoCtrlInitCodecMode; /* initialize codec mode bit-field */
} IpcDsvSendSsoCtrlParamsMsgT;

#define IPC_NUM_TONE_GEN 6

typedef PACKED struct
{
   uint16   ToneGenId;     /* Tone generator to which the structure applies */
   int16    PoleCoef;      /* tone pole coefficient */
   int16    VolCoef;       /* tone volume coefficient */
   uint32   SeqOnTime;     /* Long timer for tones    */
} IpcDsvToneCfgT;

typedef PACKED struct
{
   uint16         MuteVoice;     /* tone sequence with/without voice control switch */
   uint16         NumTones;      /* Number of Tones to be activated  */
   IpcDsvToneCfgT ToneParms[1];    
} IpcDsvToneGenParamsMsgT;

typedef PACKED struct
{
   uint16   AudioSideToneScalFctr;     
   uint16   SpkrBufSoftLimitScalFctr;      
} IpcDsvSendAudioIsrScalFctrMsgT;

typedef PACKED struct
{
   uint16   AudioLoopbackMode;      /* audio path loop-back mode switch */
} IpcDsvSetAudioLoopbackModeMsgT;

typedef PACKED struct
{
   uint16   AudioHandsFreeMode;     /* hands-free audio mode switch */
} IpcDsvSetAudioHandsFreeModeMsgT;

typedef PACKED struct
{
   uint16   tbd;
} IpcDsvSendAudioHandsFreeCfgParamsMsgT;

typedef PACKED struct
{
   uint16   AudioAecMode;           /* acoustic echo cancellation mode switch */
} IpcDsvSetAudioAecModeMsgT;

typedef PACKED struct
{
   int16    FarEndPwrThres;     /* threshold of farend spch engy to resume adaptation */
   int16    SppNgyThL;
   int16    NoiseThres;
   int16    ChnSnrThL;
   int16    Fe2TrThres;         
   int16    Tr2NeThresQ; 
   int16    ChanTapQ;
   int16    Partition;
   int16    OverlapRatio;
   int16    DftSize;
   int16    DftNumStage;
   int16    BlockSize;
   int16    BlockNum;
   int16    AttnTgt;                
} IpcDsvSendAudioAecMainCfgParamsMsgT;

typedef PACKED struct 
{
   int16    EncDecDelay;         /* encoder - decoder delay */
   int16    FrmPwrScalFctrQ;     /* forgetting factor used for update on long term frame power */
   int16    LngAvgScalFctr;
   int16    DblTlkCorrLen;       /* doubletalk correlation length */
   int16    DblTalkCorrIndx;     /* doubletalk correlation index */
   int16    DblTlkScalFctr;      /* doubletalk forgetting factor */
   int16    DblTlkWghtUpdThres;  /* threshold to resume adaptation */
   int16    SysDstnceScalFctr;   
   int16    ACFScalFctr;         
   int16    MaxModeCount;
   int16    FeMdeHngOvr;
   int16    NeMdeHngOvr;
   int16    ErleScalFctr; 
   int16    StepSize;
   int16    ChnNgyScalFctr;
   int16    NzeScalFctr;
   int16    CeptScalFctr;
   int16    EstEchScalQ;
   int16    EngyScalFctr;
} IpcDsvSendAudioAecAuxCfgParamsMsgT;  

typedef PACKED struct
{
   int16    BlockSize;               /* -- ACP configuration parameters-- */        
   int16    BlockNum;
   int16    RmsQ;
   int16    RmsScalFctr;
   int16    GammaHi;
   int16    GammaLo;
   int16    BetaHi;
   int16    BetaLo;
   int16    BetaLoQ;
   int16    AttackTime;
   int16    ReleaseTime;

   uint16   AsvcMinGain;             /* -- Mpp Backgroun noise level estimatimation -- */ 
   uint16   AsvcMaxState;
   uint16   AsvcNoiseStep;
   uint16   AsvcGainStep;
   uint16   AsvcNoiseThres;
   uint16   AsvcHysterisis;
   int16    AsvcNoiseScalFctr;
   uint16   AsvcVmThres;
   uint16   AsvcTceThres;
   uint16   AsvcSpecDevThres;
   uint16   AsvcCntThres;
   uint16   AsvcHysCntThres;

   int16    AsvcSpkrWeight;
   int16    AsvcScalFctrQ;
   int16    AsvcSmoothFctr;
   int16    AsvcMaxSpkrGain;         
   uint16   FramePwrStep;            /* -- Spp frame power estimatimation -- */
   uint16   FramePwrThres;
   uint16   FramePwrHyst;
   uint16   SppGainStep;
   uint16   SppGainMaxState;
   uint16   SppFrmPwrBufLen;
   uint16   SppFrameCntThres;
   uint16   GainUpdateThres;
   uint16   GainUpdateSwch;
#if ( ((SYS_ASIC == SA_ROM) && (SYS_VERSION >  SV_REV_C3)) || (SYS_ASIC == SA_RAM) || (SYS_ASIC == SA_CBP55) )
   uint16   AcpToneThres;           /* --- Acp Acoustic Shock tone threshold and suppress target */
   uint16   AcpToneSuppress;
#endif
} IpcDsvSendAudioAcpCfgParamsMsgT;

typedef PACKED struct
{
   uint16   AudioAcpCompMode;
   uint16   AudioAcpAsvcMppMode;
   uint16   AudioAcpAsvcSppMode;
} IpcDsvSetAudioAcpModeMsgT;

typedef PACKED struct
{
   uint16  AudioRecordPlaybackMode;        /* Use IpcVoiceRecordPlaybackModeT     voice record playback mode */
   uint16  AudioRecordPlaybackVocoder;     /* Use IpcSpchSrvcOptT                 voice record playback vocoder type */
   uint16  AudioRecordPlaybackDataType;    /* LSB : Use IpcVoiceRecPlayDataTypeT  voice record playback data type */
                                           /* MSB : Use EdaiCompndT               When LSB is PCM, MSB determine between LinearPCM, Mu-lawPCM or A-lawPCM */
   uint16  AudioRecordPlaybackMaxRate;     /* Use IpcSpchSrvcOptRateT             voice record playback maximum encode rate */
   uint16  AudioRecordPlaybackMinRate;     /* Use IpcSpchSrvcOptRateT             voice record playback minimum encode rate */
   uint16  AudioRecordPathSel;             /* Use IpcVoiceRecSelPathModeT         voice record select between rev ch and fwd ch in AMPS mode*/
   uint16  AudioPlaybackVoiceMix;          /* Use IpcVoiceRecPlaybackVoiceMixT    select voice mix or playback pcm only */
} IpcDsvSetAudioRecordPlaybackModeMsgT;

typedef PACKED struct
{
   uint16   AudioTtyMode;           /* TTY mode switch */
} IpcDsvSetAudioTtyModeMsgT;

typedef PACKED struct
{
   int16 TtyLogicEngyTh;
    int16   TtySilnceEngyTh;
   int16 TtyOnsetWin;
   int16 TtyEarlyOnsetThres;
   int16 TtyDetctThres;                   
   int16 TtyFrmSyncMemTh;                 
   int16 TtyFrmSyncStartTh;               
   int16 TtyFrmSyncDataTh;                   
   int16 TtyFrmSyncStopTh;                   
   int16 TtyFrmSyncSilnceTh;              
   int16 TtyXmtTime;                      
   int16 TtyMaxHngOvrTime;                
   int16 TtySilnceTh;
   int16 TtyToneGenGain;
} IpcDsvSendAudioTtyCfgParamsMsgT;

typedef PACKED struct
{
    uint16  Ve8MppSampOffset;
    uint16  Ve8SppSampOffset;
    uint16  Ve8SppTaskTime;
    uint16  V13MppSampOffset;
    uint16  V13SppSampOffset;
    uint16  V13SppTaskTime;
    uint16  CodecIntCtrl;
    uint16  AmpMppSampOffset;
    uint16  AmpSppSampOffset;
    uint16  AmpSppTaskTime; 
} IpcDsvSendAudioVapCfgParamsMsgT;

typedef PACKED struct 
{
   uint16 SpeakerSimMsgId;          /* identification number of simulated message */
   PACKED union 
   {
      IpcDsvSendSpkrVoiceFwdChPcktDataMsgT       SendSpkrVoiceFwdChPcktDataMsg;
      IpcDsvSetSpkrVoiceDecPitchPreFltrModeMsgT  SetSpkrVoiceDecPitchPreFltrModeMsg;
      IpcDsvSetSpkrVoiceDecSynPostFltrModeMsgT   SetSpkrVoiceDecSynPostFltrModeMsg;
      IpcDsvSendSpkrVoicePlayPcktDataMsgT        SendSpkrVoiceRecPlayPcktDataMsg;
      IpcDsvSendSpkrPlmnToneGenParamsMsgT        SendSpkrPlmnToneGenParamsMsg;
      IpcDsvSendSpkrRngrToneGenParamsMsgT        SendRngrPlmnToneGenParamsMsg;
      IpcDsvSetSpkrFifModeMsgT                   SetSpkrFifModeMsg;
      IpcDsvSendSpkrFifCfgParamsMsgT             SendSpkrFifCfgParamsMsg;
      IpcDsvSendSpkrVolScalFctrMsgT              SendSpkrVolScalFctrMsg;
      IpcDsvSetAudioAirInterfaceModeMsgT         SetAudioAirInterfaceModeMsg;
      IpcDsvSendAudioSsoConnectMsgT              SendAudioSsoConnectMsg;
      IpcDsvSendAudioSpchRoutingMsgT             SendAudioSpchRoutingMsg;
      IpcDsvSendSsoCtrlParamsMsgT                SendSsoCtrlParamsMsg;
      IpcDsvSendAudioIsrScalFctrMsgT             SendAudioIsrScalFctrMsg;
      IpcDsvSetAudioLoopbackModeMsgT             SetAudioLoopbackModeMsg;
      IpcDsvSetAudioHandsFreeModeMsgT            SetAudioHandsFreeModeMsg;
      IpcDsvSendAudioHandsFreeCfgParamsMsgT      SendAudioHandsFreeCfgParamsMsg;
      IpcDsvSetAudioAecModeMsgT                  SetAudioAecModeMsg;
      IpcDsvSendAudioAecMainCfgParamsMsgT        SendAudioAecMainCfgParamsMsg;
      IpcDsvSendAudioAecAuxCfgParamsMsgT         SendAudioAecAuxCfgParamsMsg;
      IpcDsvSetAudioRecordPlaybackModeMsgT       SetAudioVoiceRecModeMsg;
      IpcDsvSetAudioTtyModeMsgT                  SetAudioTtyModeMsg;
      IpcDsvSendAudioTtyCfgParamsMsgT            SendAudioTtyCfgParamsMsg;
   } Msg;
} IpcDsvSendSpkrSimMsgMsgT;


/*******************************************************
** Message Stuctures: DSP to Control Processor (_CP_) **
*******************************************************/

/*----------------------------**
** Voice And Audio Processing **
**----------------------------*/

typedef PACKED struct
{
   uint16 MppSpchRate;                 /* Encoder rate decision indicator */
   uint16 NumMppSpchData;              /* Number of Tx Speech Data */
   uint16 MppSpchData[1];              /* Starting address of Tx Speech Data */
} IpcCpSpchDataRevChPacketMsgT;

typedef PACKED struct
{ 
   uint16 MppSpchDataType;             /* Record data type */
   uint16 MppSpchRate;                 /* Encoder rate decision indicator */
   uint16 NumMppSpchData;              /* Number of Tx Speech Data */
   uint16 MppSpchData[1];              /* Starting address of Tx Speech Data */
} IpcCpSpchDataRecordPacketMsgT;

typedef PACKED struct
{
   int32   BwExpandAcf[IPC_ACF_LEN_MAX];  /* bandwidth expanded autocorrelation coefficients, Rw */
} IpcCpSpchDataMppAgcMsgT;

typedef PACKED struct
{
   uint16   MppParmType;                  /* encoder diagnostic parameter type */
   uint16   MppParmLen;                   /* encoder diagnostic parameter length */
   uint16   MppParms[1];                  /* encoder diagnostic parameters */
   uint16   SppParmType;                  /* decoder diagnostic parameter type */
   uint16   SppParmLen;                   /* decoder diagnostic parameter length */
   uint16   SppParms[1];                  /* decoder diagnostic parameters */
} IpcCpSpchDataDiagMsgT;

typedef PACKED struct
{
   uint16 ModuleId;                    /* Audio module ids */
   uint16 NumCfgData;                  /* Number of configurable parameters Data */
   uint16 AudioCfgData[1];             /* Audio Config Data */
} IpcCpAudioCfgParamsMsgT;

/***********************************************************
** Message Field Defines: Control Processor to DSP (_DSV_) **
************************************************************/
/*-------------------------------------------------**
** AMPS related message data structures CP -> DSPV **
**-------------------------------------------------*/
typedef   enum
{
   NORMAL_MODE          =0,
   CONTINUOUS_TEST_MODE =1
} ReccTstModeT ;

typedef PACKED struct
{
   uint16 Mode;
   uint16 Dcc;
   uint16 MsgLen;
   uint16 Data[1];
} IpcDsvSendAmpWbdReccMessageMsgT;

typedef PACKED struct
{
   uint16 MsgLen;
   uint16 Data[1];
} IpcDsvSendAmpWbdRvcMessageMsgT ;

/* Voice mode: Rvc Mode message type structure: */
typedef PACKED struct
{
   uint16 StOn;
} IpcDsvSetAmpStControlMsgT ;

typedef PACKED struct
{ 
   uint16   RevChAbortOn;
} IpcDsvSetAmpRevChAbortMsgT; 

typedef enum
{
   IPC_AMP_COMPRESSOR_ON        =1,
   IPC_AMP_COMPRESSOR_OFF       =0 
} CompressorCtrlT;

typedef PACKED struct
{
   uint16 CompOn;
} IpcDsvSetAmpCompControlMsgT ;


typedef enum
{
   REV_CH_POLARITY_NORMAL =0,
   REV_CH_POLARITY_INVERT =1  
}RevChPolarityT ;

typedef enum
{
  REV_CH_MODULATION_BASEBAND_IQ = 0,
  REV_CH_MODULATION_VCO         = 1,
  REV_CH_MODULATION_DIF_MODE    = 2
} RevChModulationT;



/*------------------------------------------------------------------------------*/

/* Structure for Amps parameters configuration message. All the voice path (RevCh and FwdCh
   voice, Sat , and St parameters are defined in this data structure */



typedef PACKED struct
{
   int16      Ciscale;     
   int16      Piscale;     
   int16      STscale;     
   int16      SATscale;     
   int16      DevLimFltrscale;     
   int16      DevLimThresh;
   uint16     wbdscaladj;
} IpcAmpRevChScalCtlT ;    /* Amps parameters scale and control configuration data */

/*
typedef PACKED struct
{

   int16      InterpCoefs[8];
   int16      PostDLCoefs[7];
   int16      PreEmphCoefs[55];
   int16      CompCoefs[2]; 
} IpcAmpRevChFiltCoefsT ;  */  /* Amps parameters filter coefficients configuration data */

typedef PACKED struct
{
   IpcAmpRevChScalCtlT     ScalCtlData;     
/*   int16                   ManchTable[32];
   IpcAmpRevChFiltCoefsT   FiltCoefs;     */
} IpcDsvSetAmpRevChParamsConfigMsgT ;    /* Amps parameters configuration data type */

typedef PACKED struct
{
   int16                   RevChPolarity;
   int16                   RevChModulationType;
} IpcDsvSetAmpRevChCtrlConfigMsgT ;    /* Amps control configuration data type */

typedef PACKED struct
{
   uint16  DcOffsetMode;
   uint16  DcOffsetVal;
} IpcDsvSetAmpFixedFreqMsgT;

typedef PACKED struct
{
   uint16  Status;
} IpcDsvGetAmpCompExpStatusMsgT;

/* structure for Reverse Channel simulation message */
typedef PACKED struct 
{
   uint16 RevChSimMsgId;
   PACKED union 
   {
      IpcDsvSendAmpWbdReccMessageMsgT             SendAmpWbdReccMessageMsg;
      IpcDsvSendAmpWbdRvcMessageMsgT              SendAmpWbdRvcMessageMsg;
      IpcDsvSetAmpRevChAbortMsgT                  SetAmpRevChAbortMsg;
      IpcDsvSetAmpRevChParamsConfigMsgT           SetAmpRevChParamsConfigMsg;
      IpcDsvSetAmpCompControlMsgT                 SetAmpCompControlMsg;
      IpcDsvSetAmpStControlMsgT                   SetAmpStControlMsg;
      IpcDsvSetAmpFixedFreqMsgT                   SendAmpFixedFreqMsg;
      IpcDsvSetAmpRevChCtrlConfigMsgT             SetAmpRevChCtrlConfigMsg;
      IpcDsvGetAmpCompExpStatusMsgT               GetAmpCompExpStatusMsg;
   } Msg;
} IpcDsvAmpSendRevChSimMsgMsgT;

/* Rx message structuctures */

typedef enum
{
   FOCC_DEMOD_OFF = 0,
   FOCC_DEMOD_ON
} WideBandDataDemodControlT;

typedef enum
{
   POWER_SAVING_MODE_OFF = 0,
   POWER_SAVING_MODE_ON
} PowerSavingModeControlT;

typedef enum
{
   STREAM_A = 0,
   STREAM_B
} WideBandDataStreamT;

typedef enum
{
   IPC_AMP_MODE_DISABLED = 0,
   IPC_AMP_MODE_FOCC     = 1,
   IPC_AMP_MODE_VOICE    = 2,
   IPC_AMP_MODE_FOCC_RECC,
   IPC_AMP_MODE_VOICE_RVC,
   IPC_AMP_MODE_VOICE_FVC
} IpcAmpModeT;

typedef enum
{
   IPC_AMP_IDLE_MODE_DISABLE = 0,
   IPC_AMP_IDLE_MODE_ENABLE  = 0xC000
} IpcAmpIdleModeControlT;

/* Voice mode/FOCC Mode message type structure: */
typedef PACKED struct
{
   uint16 Mode;
   uint16 FoccStream;
   uint16 FoccType;
} IpcDsvSetAmpModeMsgT;

typedef PACKED struct
{
   uint16 Mode;
} IpcCpSetAmpModeRspMsgT;


typedef PACKED struct
{
   uint16  BusyIdleProcessOn;
} IpcDsvSetAmpBusyIdleProcessMsgT;


typedef PACKED struct
{
   uint16 FrameCounts;
} IpcDsvAmpGetFmSignalStrengthMsgT;


typedef PACKED struct
{
   uint16  DcBiasCalcMode;
   uint16  FrameCnt;
   int16   FreqOffset;   
} IpcDsvSetAmpDcOffsetCompMsgT;

typedef enum
{
   IPC_AMP_DC_CALC_DISABLED    =0,
   IPC_AMP_DC_AVER_COMP        =1,
   IPC_AMP_DC_AVER_ARC_COMP    =2,
   IPC_AMP_DC_AVER_ONLY        =3,
   IPC_AMP_DC_AVER_ARC_ONLY    =4,
   IPC_AMP_DC_COMP_ONLY        =5
} IpcDsvAmpDcBiasCalcModeT;

typedef PACKED struct
{
   uint16  RxAgcMode;
   uint16  AssertPdmTarget;
   uint16  PdmVal;
   uint16  TargetMode;
} IpcDsvSetAmpRxAgcCtrlMsgT;

typedef enum
{
   IPC_AMP_ASSERT_NOTHING          =0,
   IPC_AMP_ASSERT_PDM_ONLY         =1,
   IPC_AMP_ASSERT_TARGET_ONLY      =2,
   IPC_AMP_ASSERT_PDM_TARGET_BOTH  =3
} IpcDsvAmpRxAgcAssertPdmTargetT;

typedef enum
{
   IPC_AMP_RX_AGC_LO_TARGET   =0,
   IPC_AMP_RX_AGC_ME_TARGET   =1,
   IPC_AMP_RX_AGC_HI_TARGET   =2,
   IPC_AMP_RX_AGC_NUM_TARGETS =3
} IpcDsvAmpRxAgcTargetModeT;

typedef PACKED struct
{
   uint16  AfcMode;
   uint16  AssertPdm;
   uint16  PdmVal;
} IpcDsvSetAmpAfcCtrlMsgT;

typedef PACKED struct
{
   uint16  WordSyncCounter;
} IpcDsvSetAmpFwdChCounterMsgT;


/* Expander message */
typedef enum
{
   IPC_AMP_EXPANDER_ON        =1,
   IPC_AMP_EXPANDER_OFF       =0 
 } ExpanderCtrlT;

typedef PACKED struct
{
   uint16            ExpanderCtrl;
} IpcDsvSetAmpExpControlMsgT ;

/* Hpf message */
typedef enum
{
   IPC_AMP_HPF_ON             =1,
   IPC_AMP_HPF_OFF            =0 
 } HpfCtrlT;

typedef PACKED struct
{
   uint16            HpfCtrl;
} IpcDsvSetAmpHpfControlMsgT ;

/* SAT control message */
typedef enum
{                                /* Table Form:   Det    AddSat   Gen/Trans        */
   IPC_AMP_SAT_DET_TRANS   =7,   /*               1       1       1   =     7      */
   IPC_AMP_SAT_DET_GEN     =6,   /*               1       1       0   =     6      */
   IPC_AMP_SAT_GEN_ONLY    =2,   /*               0       1       0   =     2      */
   IPC_AMP_SAT_ALL_OFF     =0    /*               0       0       0   =     0      */
} SatModeT ;

typedef PACKED struct
{
   uint16 SatMode;
} IpcDsvSetAmpSatControlMsgT ;

/* SAT Raw Data Control message */
typedef enum
{
   IPC_AMP_SAT_RAW_DATA_ON    =1,
   IPC_AMP_SAT_RAW_DATA_OFF   =0 
 } SatRawDataCtrlT;

typedef     enum
{
   FWD_CH_DC_BIAS_ENABLE    =0,   
   FWD_CH_DC_BIAS_DISABLE   =1
 } FwdChDcBiasT ;

typedef enum
{
   FWD_CH_POLARITY_NORMAL = 0,
   FWD_CH_POLARITY_INVERT = 1  
}FwdChPolarityT;

typedef PACKED struct
{
   uint16      Discriminatorscale;
    
   uint16      WbdInpScale;
   uint16      FoccDotWsThd;
   uint16      FoccDotWsThdAdj;
   uint16      FvcDotThd;
   uint16      FvcDotCntrInc;
   uint16      FvcDotCntrDec;
   uint16      FvcDotCntrThd;

   uint16      Satiscale;
   
   int16       AfcTarget;
   int16       VoiceAfcCn;        
   int16       FoccAfcCn;

   int16       RxAgcTargetLo;
   int16       RxAgcTargetMe;
   int16       RxAgcCorrectFctr;
   int16       VoiceAfcSlope;
   int16       FoccAfcSlope;   

   uint16      DcThreshold;
   uint16      SatDetectScale;
   uint16      PwrChangeHystMed2;
   uint16      PwrChangeHystHi;
   uint16      AfcOffPwrThresh;
   int16       AfcCntrlGain;
   int16       FoccLostSyncCntrThd;
   uint16      FoccGoodFrmThd;
   uint16      IdleDspUpTime;
   uint16      IdleRfFastUpTime;
   uint16      IdleRfSlowUpTime;
   uint16      FvcSyncDetLim;
   uint32      BchErrPat;
   uint16      CenterFreq;
   uint16      PdmPerUnitQ;
   uint16      FoccSlowRFSlpNum;
   uint16      SatWeight;
   uint16      ExpCorrectThresh;
   uint16      ExpCorrectFctr;
   uint16      AverOnThresh;
   uint16      MinPdmVal;
   uint16      DcCompNormShift;
   int16       RxAgcSlopeQAdjFctr;
   uint16      DcOffsetReportMode;
   uint16      DcOffsetThresh1;
   uint16      DcOffsetThresh2;
   uint16      DcCalSettleFrmCnt;

} IpcAmpFwdChScalCtlT;    /* Amps parameters configuration data type */

/*
typedef  PACKED struct
{ 
   int16     LpfDeEmphCoefs[12];
   int16     ExpCoefs[4];
   int16     HpfCoefs[8];
   int16     DcfCoefs[8];
   int16     Lpf5thOrderDemodCoefs[16];
   int16     SatBpfLpfCoefs[16];
} IpcAmpFwdChFiltCoefsT;
*/

typedef PACKED struct
{
   IpcAmpFwdChScalCtlT     ScalCtlData;
/*   IpcAmpFwdChFiltCoefsT   FiltCoefs;  */      
} IpcDsvSetAmpFwdChParamsConfigMsgT ;    /* Amps parameters configuration data type */

typedef PACKED struct
{
   uint16      FwdChPolarity;
   uint16      FoccStreamAB;
   uint16      IdleModeStatusInit;
   uint16      Eoscale;     
   uint16      RxAgcAverRate;
   uint16      DcArcLoopGain;

} IpcDsvSetAmpFwdChCtrlConfigMsgT ;    /* Amps control configuration data type */

typedef  IpcDsmAfcCalibrParmMsgT  IpcDsvSetAmpFwdChAfcCalibParamsMsgT;

typedef PACKED struct
{
   int16  AgcStepPerDb[SYS_MAX_NUM_HYST_STATES_RXAGC];
   int16  NumberOfTargetStates;
} IpcDsvSetAmpFwdChAgcCalibParamsMsgT;

typedef PACKED struct
{
   uint16  WsCounter;
} IpcDsvSetAmpWordSyncCounterMsgT;

typedef PACKED struct
{
   uint16  Status;
} IpcDsvGetAmpWordSyncStatusMsgT;

typedef PACKED struct
{
   uint16  LpfMode;
} IpcDsvSetAmpFwdLpfModeMsgT;

typedef PACKED struct
{
   uint16  IpcPdmTarget;
   uint16  IpcPdmValue;
} IpcDsvSetAmpPdmValueMsgT;


typedef PACKED struct
{
   uint16  Status;
} IpcDsvGetAmpAfcStatusMsgT;

typedef PACKED struct
{
   uint16  LpfMode;
} IpcDsvSetAmpFwdFeFirLpfModeMsgT;

/*------------------------------------------------------------------------------*/
/* structure for Rx simulation message */
/*------------------------------------------------------------------------------*/

typedef PACKED struct 
{
   uint16 FwdChSimMsgId;
   PACKED union 
   {
      IpcDsvSetAmpModeMsgT                    SetAmpModeMsg;
      IpcDsvSetAmpBusyIdleProcessMsgT         SetAmpBusyIdleProcessModeMsg;
      IpcDsvAmpGetFmSignalStrengthMsgT        GetAmpFmSignalStrengthMsg;
      IpcDsvSetAmpDcOffsetCompMsgT            SetAmpDcOffsetCompMsg;
      IpcDsvSetAmpRxAgcCtrlMsgT               SetAmpRxAgcCtrlMsg;
      IpcDsvSetAmpAfcCtrlMsgT                 SetAmpAfcCtrlMsg;
      IpcDsvSetAmpFwdChCounterMsgT            SetAmpFwdChCounterMsg; 
      IpcDsvSetAmpFwdChParamsConfigMsgT       SetAmpFwdChParamsConfigMsg;
      IpcDsvSetAmpFwdChAfcCalibParamsMsgT     SetAmpFwdChAfcCalibParamsMsg;
      IpcDsvSetAmpExpControlMsgT              SetAmpExpControlMsg;
      IpcDsvSetAmpHpfControlMsgT              SetAmpHpfControlMsg;
      IpcDsvSetAmpSatControlMsgT              SetAmpSatControlMsg;
      IpcDsvSetAmpWordSyncCounterMsgT         SetAmpWordSyncCounterMsg;
      IpcDsvGetAmpWordSyncStatusMsgT          GetAmpWordSyncStatusMsg;
      IpcDsvSetAmpFwdLpfModeMsgT              SetAmpFwdLpfModeMsg;
      IpcDsvSetAmpPdmValueMsgT                SetAmpPdmValueMsg;
      IpcDsvSetAmpFwdChCtrlConfigMsgT         SetAmpFwdChCtrlConfigMsg;
      IpcDsvSetAmpFwdChAgcCalibParamsMsgT     SetAmpFwdChAgcCalibParamsMsg;
      IpcDsvGetAmpAfcStatusMsgT               GetAmpAfcStatusMsg;
      IpcDsvSetAmpFwdFeFirLpfModeMsgT         SetAmpFwdFeFirLpfModeMsg;

   } Msg;
} IpcDsvAmpSendFwdChSimMsgMsgT ;


/*-------------------------------------------------**
** AMPS related message data structures DSPV -> CP **
**-------------------------------------------------*/
typedef enum
{
   FOCC_DATA = 0,
   FVC_DATA
} IpcCpAmpFwdChDataT;

typedef PACKED struct
{
   uint16   FwdChDataType;
   uint16   FwdChDemodBit27To16;
   uint16   FwdChDemodBit15To0;
   uint16   FwdControlChNumWords;
   uint16   FwdChBchErr;
   uint16   FwdChSyncLock;
} IpcCpAmpFwdChDataRspMsgT;

typedef PACKED struct
{
   uint16 SatDet;
} IpcCpAmpSatDetRspMsgT;



typedef enum
{
   AMPS_BIS_IDLE = 0,
   AMPS_BIS_BUSY
} BusyIdleT;

typedef PACKED struct
{
   uint16 BusyIdleStatus;
} IpcCpAmpBusyIdleRspMsgT;

typedef PACKED struct
{
   uint16 IpcNumGoodSync;
   uint16 IpcNumBadSync;
} IpcCpAmpWordSyncRspMsgT;


/* FAST MBOX: CP -> DSPV:
 * List of commands for the CP to use to drive the operation of the FIQ.
 * The Command ID is written as the 1st word of the TX Fmbox HWD memory.
 */
typedef enum
{
    IPC_DSV_FMB_APP_DATA_CMD = 1,
    IPC_DSV_FMB_APP_ABORT_CMD
} IpcDsvFMBoxCmdT;
 
/* FAST MBOX: DSPV -> CP:
 * List of commands for the DSPV to use to manage the Idle Mode Power Saving */
/* These codes are to be used to drive the operation of the FIQ */
typedef enum
{
   NULL_FUNCTION = 0,
   ALL_RF_ON,
   FAST_OFF,
   FAST_ON,
   SLOW_OFF,
   SLOW_ON,
   NUM_IDLE_MODE_ENTRIES
} IpcCpAmpFwdChIdleModeRspT;

typedef PACKED struct
{
   uint16 RawRssiVal;       /* Raw Rssi 16 bit unsigned */
   uint16 PdmVal;           /* Pdm value */
   uint16 AgcTarget;
} IpcCpFmSignalStrengthRspMsgT;

typedef PACKED struct
{
   uint16 IpcTimer20ms;
   uint16 IpcTimer125us;
   uint16 IpcSampleOffset;
} IpcCpAmpFrameSyncDetectRspMsgT;

typedef PACKED struct
{
   uint16  CompMode;
   uint16  ExpMode;
} IpcCpAmpCompExpStatusRspMsgT;

typedef PACKED struct
{
   int16  AfcPdmVal;
   int16  FreqErr;
} IpcCpAmpAfcStatusRspMsgT;

typedef PACKED struct
{
   uint16 NumBusyIdleBits;
   uint16 NumBusy;
   uint16 NumIdle;
} IpcCpAmpFwdBusyIdleStatusRspMsgT;

typedef PACKED struct
{
   uint16 NumBusyIdleBits;
} IpcDsvAmpFwdBusyIdleStatusMsgT;

typedef PACKED struct
{
    IpcLpcIROptEnT          LpcIROptEn;
    IpcLpcIRMaxLenT         LpcIRMaxLen;
    IpcLpcIRMinLenT         LpcIRMinLen;
    IpcLpcIRPctPwrT         LpcIRPctPwr;

    IpcFcbSubSampEnT        FcbSubSampEn;
    IpcFcbSubSampParmT      FcbSubSampParm;
    IpcFracPitchModeT       MicVoiceEncFracPitchMode;
    uint16                  V13ScaleFactor;                                    
} IpcDsvSendRevChSsoConfigurationMsgT; 

typedef PACKED struct
{
   uint16   SpkrVoiceDecPitchPreFltrMode;  /* voice decode pitch pre-filter mode switch */
   uint16   SpkrVoiceDecSynPostFltrMode;  /* voice decode synthesis post-filter mode switch */
   uint16   FrameErasureFcbExicitionMode;
} IpcDsvSendFwdChSsoConfigurationMsgT;


/* DSPV EDAI Companding format message*/
typedef enum
{    
 IPC_VAP_EDAI_LINEAR_PCM        =0x00,              /* should be set to IPC_VAP_EDAI_LINEAR_PCM when internal codec is used  */
 IPC_VAP_EDAI_COMPAND_ALAW      =0x01,
 IPC_VAP_EDAI_COMPAND_MULAW     =0x02
} EdaiCompndT ;

typedef EdaiCompndT  IpcVoiceCompandDataT;

typedef PACKED struct
{
   uint16 EdaiModeConfig;
} IpcDsvSendAudioEdaiConfigMsgT;

typedef struct
 {
    uint16 SpkrCtrlData;
 } IpcDsvVapAscEdaiSpkrControlDataMsgT ;

typedef PACKED struct
{
   uint16 MicControlData;
} IpcCpVapEdaiMicControlDataRspMsgT ;

typedef PACKED struct
{
   uint16 ModuleId;
} IpcDsvVapAudioCfgQueryMsgT;

typedef PACKED struct
{
   uint16 CurrApp;
}IpcCpAppInfoQueryRspMsgT;


/*---------------------------*
 | Slotted Paging interface  |
 |       CP -> DSPM          |
 *---------------------------*/

typedef enum 
{
    INIT_CMD,                 /* Receive Initialization parameters */
    STOP_CMD,                 /* Prepare for sleep */
    DECODE_PCH_CONT_CMD,      /* Decode the Paging CHannel Continuously until commanded to stop (sleep) */
    DECODE_PCH_NO_ACQ_CMD,    /* This requires QPCH support. The assumption is that the QPCH has already acquired the
                                 pilot not longer than 100mS, so there is no need to re-acquire the pilot to decode the
                                 paging channel */
    QPCH_1_CMD,               /* Decode the Quick Paging CHannel bit, and also conduct a neighbor search */
    QPCH_2_CMD,               /* The first indicator was indeterminate, so Decode the Quick Paging CHannel again, but
                                 do not conduct a neighbor search. */
    NGHBR_SRCH_ONLY_CMD,      /* Conduct a Candidate Frequency Check */
    QUERY_LONG_CODE_CMD,      /* Initially the CP needs the long code to calculate the long code for a future time when
                                 we will hopefully wake up prior to! */
    DLY_TMR_EXPRD,            /* Signaled in MscIsr.asm, and sent by MscTask.c, but put here so that all messages sent
                                 to the MscPage sub-unit are unique. */
    SRCH_BUFF_FULL,
    SRCH_CMPLT,
    SRCH_ACT_LIST_CMPLT,
    SRCH_NGHBR_LIST_CMPLT,
    SRCH_QPCH_DECODE_CMPLT,
    SRCH_CCI_DECODE_CMPLT,
    CCI_CMD
} IpcDsmMscSpageCmdIdT;

typedef enum 
{
    MSC_MODE_CDMA,
    MSC_MODE_GPS
} ModeT;                    /* For GPS: Indicate CDMA or GPS mode in IpcDsmMscSpageInitCmdMsgT */

typedef PACKED struct
{
    uint16                  MsgCmd;          /* INIT_CMD */

    uint16                  RxAgcDly;        /* Automatic Gain Control steady state delay. Asynchronously timed with the CP */
    uint16                   Mode;            /* MSC_MODE_CDMA: Init to Normal CDMA mode or MSC_MODE_GPS: GPS mode */
} IpcDsmMscSpageInitCmdMsgT;


typedef PACKED struct
{
    uint16                  MsgCmd;          /* STOP_CMD */
    uint16                  SysCntInit1;   /*Resync high order value for system counter, set before sleep, in TC/8  */  
    uint16                  SysCntInit0;   /*Resync low order value for system counter , set before sleep  in TC/8  */
    uint16                  SysTimeInTC;     /* System Time in TC. */
} IpcDsmMscSpageStopCmdMsgT;
    

typedef PACKED struct
{                                            /* DECODE_PCH_NO_ACQ_CMD or  */
   uint16                  MsgCmd;          /* DECODE_PCH_CONT_CMD       */
   uint16                  AgcActSymbTime;  /* (Absolute 80mS) symbol time when Agc shall be started (52uS ticks)*/
   uint16                  LongCode2;       /* current long code to be programmed at the superframe boundary */
    uint16                  LongCode1;       /* is received upon wakeup from sleep and programmed into the    */
    uint16                  LongCode0;       /* DSPM timer control registers */
} IpcDsmMscSpageDecodePchMsgT;


typedef PACKED struct
{                                            /* QPCH_1_CMD or */
    uint16                  MsgCmd;          /* QPCH_2_CMD    */
    uint16                  AgcActSymbTime;  /* (Absolute 80mS) symbol time when Agc shall be started (52uS ticks)*/    
    uint16                  QpageCh;         /* needs to be converted to Walsh Code */
    uint16                  QpchRate;        /* 0 =4800, 1=9600 */
} IpcDsmMscSpageQpchCmdMsgT;

typedef PACKED struct                                        
{                                            
    uint16                  MsgCmd;          /* CCI_CMD    */
    uint16                  ActSymbTime;     /* (Absolute 80mS) symbol time when Agc shall be started (52uS ticks)*/    
    uint16                  CCICh;           /* needs to be converted to Walsh Code */
    uint16                  CCIRate;         /* 0 =4800, 1=9600 */
} IpcDsmMscSpageCCICmdMsgT;

typedef PACKED struct
{
    uint16                  MsgCmd;
} IpcDsmMscSpageQueryLongCodeMsgT;

typedef PACKED struct
{
    uint16                  MsgCmd;
} IpcDsmMscSpageSignalMsgT;

/*---------------------------*
 | Slotted Paging interface  |
 |       DSPM -> CP          |
 *---------------------------*/
typedef enum
{
    QPCH_IND,                 /* Quick Paging Result Indicator */
    STOP_CMD_ACK,             /* After preparing for sleep, acknowledge the request for sleep */
    SYS_TIME_ERR_AFT_ACQ,     /* Return the differnce in system timing from the calculated and the finger's acquired time */
    LONG_CODE_STATE,
    PWR_OFF_MIX_SIG
} IpcCpSpageRspnsIdMsgT;

typedef PACKED struct
{
    uint16  MsgResponse;      /* Response identifier = QPCH_IND */
    int16    QpchPwr;          /* Soft Indication of detection of Quick Paging (Pi1,Pi2,CCI*/
    int16    PilotPwr;         /* Active Pilot Power */
    uint16   Ec_Io;            /* Pilot Ec/Io */
    int16    Qpch_I_Energy;     /* Qpch bit I energy  */ 
    int16    Qpch_Q_Energy;     /* Qpch bit Q energy  */
    int16    Pilot_I_Energy;    /* Pilot I energy */
    int16    Pilot_Q_Energy;    /* Pilot Q energy */
    uint16   Qpch_Pilot_PN;     /* Pilot PN offset */
} IpcCpSpageQpchIndMsgT;

typedef PACKED struct
{
    uint16  MsgResponse;      /* Response identifier = STOP_CMD_ACK */
} IpcCpSpageStopCmdAckMsgT;

typedef PACKED struct
{
    uint16                 MsgResponse;      /* Response identifier = SYS_TIME_ERR_AFT_ACQ */
    int16                  SysClkDevCntTC;   /* Actual Received Time counts in 1.2288Mchp/sec 14:0 bits*/
    int16                  SysClkDevCntTC8;   /* Actual Received Time counts in 9.8304MHz  2:0 bits*/
} IpcCpSpageSysTimeDevMsgT;

typedef PACKED struct
{
    uint16                 MsgResponse;     /* Response identifier = LONG_CODE_STATE */
    uint16                 LongCode2;        /* long code bits 41:32                  */
    uint16                 LongCode1;        /* long code bits 31:16                  */
    uint16                 LongCode0;        /* long code bits 15:00                  */
} IpcCpSpageLongCodeStateMsgT;


/* ****************************************************************************************/
/* This section below supports DSPM smart antenna implemenation*/
/* ****************************************************************************************/

#define DISABLE_CMD 0   /*Stop sending the smart antenna data from DSPM to CP*/
#define ENABLE_CMD  1   /*Start sending the smart antenna data from DSPM to CP*/


/*---------------------------*
 |  Smart Antenna    |
 |       CP -> DSPM          |
 *---------------------------*/

typedef PACKED struct
{
   uint16    MsgCmd;              /* Start: MsgCmd=Enable_Cmd;  Stop: MsgCmd=Disable_Cmd*/         
   uint16    StrobeStart;         /* CP send a SMART_ANTENNA_STROBE_SETUP parameter          */  
                                  /* to DSPM in the unit of symbols in the range [0-23] */
   uint16    Reserve;             /* This parameter is reserved for future use */
}DsmSmartAntennaCmdMsgT;      

/*---------------------------*
 |  Smart Antenna    |
 |       DSPM -> CP          |
 *---------------------------*/

typedef PACKED struct
{
   uint16    MsgCmd;              /* Dspm return ack message to the start/stop message */
} CpSmartAntennaAckCmdMsgT;

typedef PACKED struct
{
    uint16     ValidData;              /* The DSPM sets this bit to 1-new packet                      */
                                       /* Cp set this bit to 0 after reading the packet               */
                                       /* Cp monitor if packets have been dropped due to heavy Cp load*/  
    uint16     ActiveFingerPN[4];      /* PN coded assigned to each finger, 0xFFFF=>Inactive finger   */
    uint16     EsNt [4];               /* Es/Nt measured at each finger in Q6, linear                             */
    uint16     BS2MobPwr;              /* PC bit send from the BS to MS in every PCG, 0-False, 1-True */
                                       /* Note: a bitmapped field, Bit 0=> fngr 0, Bit x=> fngr x     */
    uint16     Mob2BSPwr;              /* PC bit send from MS to BS, the BS may raise/lower pwr by    */
                                       /* arbitrary number of dB in response to command, 1/2 dB step  */ 
    int16      Izero;                  /* Total received pwr in Q6, dBm                               */
    int16      EbNtFilter[2];          /* Filter Eb/Nt summed over all 4-fingers in Q8, dB            */
    uint16      NtFinger1_0;                 /* Parameters for calculating Nt in CP */
                                        /* packed: fngr 1 => bit[15:8]; fngr 0=>bit[7:0]*/                     
    uint16      NtFinger3_2;              /* Parameters for calculating Nt in CP */
                                   /* packed: fngr 3 => bit[15:8]; fngr 2=>bit[7:0]*/    
    int16      TxPwr;                  /* Mobile transmit power in Q6, dBm                            */
    uint16     TxGainState;            /* Mobile Tx gain state in Q0                                  */
    int16      PilotDataFingerI[4];    /* Q0, Pilot data real (I) per finger in linear                */
    int16      PilotDataFingerQ[4];    /* Q0, Pilot data imaginary (Q) per finger in linear           */

} SmartAntennaParmMsgT;


/* ===== DSP Multimedia Applications structures ===== */


#define  IPC_NUM_DSPAPP_CHANNELS    1  /* Total number of app channels available on this HW */
#define  IPC_APPCHAN0_MAX_WORDS     896   
#define IPC_MAX_NUM_APP_MSGS        6


typedef enum 
{
   IPC_APP_CHAN_SERVICE_CODE_DOWNLOAD = 1,
   IPC_APP_CHAN_SERVICE_APP_DATA
}IpcDspAppChannelServiceT;


typedef enum 
{
   IPC_APP_CHAN_OK = 0,
   IPC_APP_CHAN_ERR = -2000,
   IPC_APP_CHAN_ERR_INVALID_CHAN_IDX = -2001,
   IPC_APP_CHAN_ERR_INVALID_CHAN_STATE = -2002,
   IPC_APP_CHAN_ERR_INVALID_MSG_IDX = -2003,
   IPC_APP_CHAN_ERR_MSG_NOT_FOUND = -2004,
   IPC_APP_CHAN_ERR_NO_MEMORY_TO_ADD_MSG = -2005,
   IPC_APP_CHAN_ERR_FREAD = -2006,
   IPC_APP_CHAN_ERR_FWRITE = -2007,
   IPC_APP_CHAN_ERR_FSEEK = -2008,
    IPC_APP_CHAN_ERR_INVALID_FILE_ID = -2009,
    IPC_APP_CHAN_ERR_INVALID_APP_ID = -2010,
    IPC_APP_CHAN_ERR_APP_NOT_FOUND = -2011
} IpcDspAppChanErrCodesT;




typedef PACKED struct
{
    uint16    NumAppChannelsUsed;
    uint16    ChanWordSize[1];  /* Size of each of NumAppChannelsUsed channels */
} IpcCpConfigureAppchannelsMsgT; 


typedef enum {
   /* App messages from DSP to CP */
   IPC_APP_CP_REQ_FREAD = 1,
   IPC_APP_CP_REQ_FWRITE,
   IPC_APP_CP_REQ_FCLOSE,
   IPC_APP_CP_REQ_FILEINFO,
   IPC_APP_CP_REQ_DISPLAY_PIXELS,
   IPC_APP_CP_MP3_STATUS,         /* Status specific to MP3 App */
   IPC_APP_CP_APP_STATUS,         /* General APP status, used for all Apps */
   IPC_APP_CP_SET_COLOR_MAPPING,
   IPC_APP_CP_SOURCE_IMAGE_SIZE,
   IPC_APP_CP_IMAGE_SCALING,
   IPC_APP_CP_REQ_DISPLAY_PIXELS_SAME_COLOR,

   /* App messages from CP to DSP */
   IPC_APP_DSP_RSP_FREAD = 0x1001,
   IPC_APP_DSP_RSP_FWRITE,
   IPC_APP_DSP_RSP_FCLOSE,
   IPC_APP_DSP_RSP_FILEINFO

}IpcDspAppMsgIdT;

typedef enum
{
   IPC_GRAPHICS_FORMAT_BMP = 1,
   IPC_GRAPHICS_FORMAT_WBMP,
   IPC_GRAPHICS_FORMAT_GIF,
   IPC_GRAPHICS_FORMAT_TIF,
   IPC_GRAPHICS_FORMAT_JPEG
}IpcDspGraphicsFormatT;

/* ======================================================== */
/* The following structures are sent through the AppChannel */
/* ======================================================== */

/* Request structures: from DSP to CP */
/* ---------------------------------- */

/* IPC_APP_CP_REQ_FREAD message structure */
typedef PACKED struct {
   uint16      FileId;  /* CP identifier of this file */
   uint32      AbsFileOffsetBytes;
   uint16      NumBytesToRead;
   uint16      RefNum;  /* DSP-assigned request id. */
} IpcCpFreadRequestT;

/* IPC_APP_CP_REQ_FWRITE message structure */
typedef PACKED struct {
   uint16      FileId;  /* CP identifier of this file */
   uint32      AbsFileOffsetBytes;
   uint16      NumBytesToWrite;
   uint16      RefNum;  /* DSP-assigned request id. */
} IpcCpFwriteRequestT;

/* IPC_APP_CP_REQ_FCLOSE message structure */
typedef PACKED struct {
   uint16      FileId;  /* CP identifier of this file */
} IpcCpFcloseRequestT;

/* IPC_APP_CP_REQ_FILEINFO message structure */
typedef PACKED struct {
   uint16      FileId;  /* CP identifier of this file */
} IpcCpFileInfoRequestT;


/* IPC_APP_CP_REQ_DISPLAY_PIXELS message structure */
typedef PACKED struct {
   /* 0-based coordinate of the top left corner, relative */
   /* to the display area size specified in an earlier    */
   /* IpcCpDisplayImageSize message.                      */
   uint16      Xleft;
   uint16      Ytop;

   /* Width and height of this block. */
   uint16      Xpixels;
   uint16      Ypixels;


   /* The data will consist of N 24-bit RGB triplets, packaged */
   /* into two 16-bit words, for a total size of 2N words,     */
   /* where N=Xpixels*Ypixels.                                  */
} IpcCpDisplayPixelBlock;



typedef PACKED struct {
   /* Band frequency range. */
   uint16      BandMinHz;
   uint16      BandMaxHz;

   /* Some relative strength number */
   uint16      BandAttenuation; 
} IpcCpEqualizerBandT;


/* IPC_APP_CP_MP3_STATUS message structure */
typedef enum
{
    IPC_MP3_OK,
    IPC_MP3_FRAME_SYNC_NOT_FOUND,
    IPC_MP3_LOSS_OF_FRAME_SYNC,
    IPC_MP3_DATA_OUT_OF_SEQUENCE,
    IPC_MP3_RAMPING_DOWN,
    IPC_MP3_TERMINATED,
    IPC_MP3_INSUFFICIENT_MAIN_DATA
} IpcMp3StatusCodesT; 

typedef PACKED struct {
   uint16      FileId;       /* CP identifier of this file */
    uint16      Mp3Status;   /* Use IpcMp3StatusCodesT */

   /* Playback status information for CP */
   uint16      AvgBytesPerSecond;
    uint32      FirstFrameOffsetBytes;
   uint32      BitRate;
   uint16      SamplesPerSecond;
   uint16      NumEqualizerBands;

   /* The data will consist of N IpcCpEqualizerBandT structures */
   /* where N=NumEqualizerBands.                                */
} IpcCpMP3PlaybackStatusT;

/* IPC_APP_CP_APP_STATUS message structure */
typedef enum
{
    IPC_APP_CHAN_NOT_ENOUGH_DATA
} IpcApplicationStatusT; 

typedef PACKED struct
{
    uint16  Status;   /* General APP Status (IpcApplicationStatusT) */
} IpcCpApplicationStatusT;

/* IPC_CP_DSPM_GET_PARMS_RSP_MSG message structure */
typedef PACKED struct
{
   uint16  AfcVal;
} IpcCpDspmAfcParmsRspMsgT;


/* Response structures: from CP to DSP */
/* ----------------------------------- */

typedef PACKED struct {
   uint16      FileId;  /* CP identifier of this file */
   uint32      AbsFileOffsetBytes;
   uint16      NumBytesToRead;
   int16       NumBytesReadOrError;
   uint16      RefNum;  /* DSP-assigned request id. */
   /* Data bytes follow here */
} IpcDspFreadResponseT;

typedef PACKED struct {
   uint16      FileId;  /* CP identifier of this file */
   uint32      AbsFileOffsetBytes;
   uint16      NumBytesToWrite;
   int16       NumBytesWrittenOrError;
   uint16      RefNum;  /* DSP-assigned request id. */
} IpcDspFwriteResponseT;


typedef PACKED struct {
   uint16      FileId;  /* CP identifier of this file */
   int16       ResultOrError;
} IpcDspFcloseResponseT;

typedef PACKED struct {
   uint16      FileId;  /* CP identifier of this file */
   uint32      FileLengthBytes;
   uint32      CurAbsByteOffset;
   /* 
      Can add other info if needed: date/time of creation, of last modification, 
   etc.
   */

} IpcDspFileInfoResponseT;

/* Bit mask values for setting image transformation options. */
typedef enum
{
    IPC_IMG_TRANSFORM_SCALE = 0x0001, 				 /* 0 = clip, 1 = scale */
    IPC_IMG_TRANSFORM_PRESERVE_ASPECT_RATIO = 0x0002 /* Matters only in scale mode */
} IpcImageTransformationOptionsT;

typedef PACKED struct {
   /* DSP will tell CP how big the displayed image will be (after clipping or scaling).
   	This size will be less than or equal to DestWidthPixels x DestHeightPixels
	specified by the IpcDspImageTransformT message to DSP
	*/
   uint16      NumXpixelsToBeUsed;		
   uint16      NumYpixelsToBeUsed;

	/* DSP will tell CP how much of compression (fraction < 1) or expansion */
	/* will be performed on each dimension.								    */
   uint16      XscaleNumerator;		
   uint16      XscaleDenominator;		
   uint16      YscaleNumerator;		
   uint16      YscaleDenominator;		

} IpcCpDisplayImageSizeT;


/* IPC_APP_CP_SET_COLOR_MAPPING message structure */
typedef PACKED struct {
   uint16      NumBitsPerPixel;	/* 1,2,4, or 8 */

   /* The data is a mapping table from index to RGB, where   */
   /* index varies from 0 to M-1, where M=2^NumBitsPerPixel. */
   /* The data will consist of M RGB16 values (i.e. M words) */
} IpcCpColorMappingRequestT;


typedef PACKED struct {
   /* DSP will tell CP how big the source image is. */
   /* Full size of the original image without clipping or scaling. */
   uint16      NumOrigImageXpixels;
   uint16      NumOrigImageYpixels;
   uint16      NumOrigImageBitsPerPixel;
   /* Typically 1-24, ranging from monochrome to 24-bit RGB */
} IpcCpSourceImagePropertiesT;

/* IPC_APP_CP_REQ_DISPLAY_PIXELS message structure */
typedef PACKED struct {
   /* 0-based coordinate of the top left corner, relative */
   /* to the display area size specified in an earlier    */
   /* IpcCpDisplayImageSize message.                      */
   uint16      Xleft;
   uint16      Ytop;

   /* Width and height of this block. */
   uint16      Xpixels;
   uint16      Ypixels;

   /* The data will consist of N=Xpixels*Ypixels pixel     */
   /* descriptors. In the default 16-bit RGB case, the	   */
   /* data will consist of N 16-bit RGB triplets for a     */
   /* total size of N words. 							   */

   /* In case an IpcCpColorMappingRequestT has established */
   /* a mapping from K bits to RGB, the data here will be  */
   /* made up of K-bit entries stuffed into 16-bit words.  */
   /* We will constrain K to be a divisor of 8 (1,2,4,8).  */
   /* Hence, given that pixels are transmitted in row-major*/
   /* order, the total number of words here will be        */
   /*    Ypixels * CEIL (Xpixels * K / 16)				   */

} IpcCpDisplayPixelBlockT;

typedef  PACKED   struct
{
   uint16   InputFileId;
   uint16   BitmapType;		/* To differentiate between various bitmap formats */

   /* Specify the source image rectangle to display.            */
   /* Use X,Y=(0,0) and DX,DY=(0,0) to display the WHOLE IMAGE. */

   uint16   SourceImageStartLeftX;
   uint16   SourceImageStartTopY;
   uint16   SourceImageDX;
   uint16   SourceImageDY;


	/*  Size of the destination rectangle to map the source image to. 
		The coordinates in the returned pixel data from DSP to CP 
		will be relative to this destination rectangle, 0-based.
	 */
   uint16   DestWidthPixels;
   uint16   DestHeightPixels;

   uint16   TransformationOptions;	 /* IpcImageTransformationOptionsT */

   uint16   ConversionPreferences; 	/* IpcDspImageConversionPreferenceT -->
   									  How CP prefers to receive pixel data: 
   									  with/without fixed color blocks, 
   									  use/not use palettes, etc. */
} IpcDsvStartBmpAppT;


/* This is a dummy message to make sure that the size of the IpcDspAppCmdArgsT 
 * structure is a multiple of 2 words; this is required so that the first DATA field is 
 * always aligned on a 32-bit boundary. The number of words in the "Dummy" field must
 * be EVEN, and must equal or exceed the largest sub-structure in the union.
 */
typedef PACKED struct {
    uint16 Dummy [10];
} IpcAppMaxCmdArgsT;


typedef PACKED union
{
  IpcCpFreadRequestT             FreadReq;
  IpcDspFreadResponseT           FreadRsp;

  IpcCpFwriteRequestT            FwriteReq;
  IpcDspFwriteResponseT          FwriteRsp;

  IpcCpFcloseRequestT            FcloseReq;
  IpcDspFcloseResponseT          FcloseRsp;

  IpcCpFileInfoRequestT          FileInfoReq;
  IpcDspFileInfoResponseT        FileInfoRsp;

  IpcCpSourceImagePropertiesT    SourceImageProperties;
  IpcCpDisplayImageSizeT         DisplaySizeUsedByImage;

  IpcCpColorMappingRequestT      ColorMappingReq;
  IpcCpDisplayPixelBlockT        DisplayPixelsReq;

  IpcCpApplicationStatusT        ApplicationStatus;
  IpcCpMP3PlaybackStatusT        MP3PlaybackStatus;

#if (SYS_ASIC == SA_CBP55)
  IpcAppMaxCmdArgsT              MaxCmdArgs;      /* Dummy structure used for alignment */
#endif

} IpcDspAppCmdArgsT;

/* It is assumed that every message consists of 3 parts:
   the first 3 words are the same for every message: Msg Id, App Id, Data Size;
    the next structure differs for different command, but is part of the same union type, 
    and therefore has the same size in every message;
    and finally the third section contains 0 or more data words that may be needed to 
    specify the message (e.g., an FWRITE message carries data to be written)
 */

typedef PACKED struct 
{
    PACKED struct
    {
      uint16             MsgId;        /* use IpcDspAppMsgIdT */
      uint16             AppId;       /* Which application (IpcDspAppIdT) */
      uint16             NumDataWords; /* K>=0 : size of data following the command */
      IpcDspAppCmdArgsT  CmdArgs;
    } CmdHdr;

    uint16 Data[1];    /* K (0 or more) 16-bit-words */

} IpcDspAppMessageT;


typedef PACKED struct
{
   uint16            NumAppMsgs;  /* 1 or more IpcDspAppMessageT structures */
   IpcDspAppMessageT AppMsg [1];  /* Message array of size 'NumAppMsgs'     */

} IpcDspAppChanDataT;

typedef PACKED struct
{
    uint16  AppServiceId;   /* use IpcDspAppChannelServiceT */
    uint16  TotalNumBytes;  /* Total number of bytes used in Shared Memory 
                        including THESE first 6 header bytes */

    IpcDspAppChanDataT AppData;  /* Application channel data */

} IpcDspAppChannelT;




/* ============================================================= */
/* The following structures are sent through the IPC Mailbox I/F */
/* ============================================================= */

typedef enum
{
    IPC_APP_NULL  = 0x0000,   
    IPC_APP_TEST  = 0x0001,              
    IPC_APP_MIDI  = 0x0002,             
    IPC_APP_MP3   = 0x0004,     
    IPC_APP_AAC   = 0x0008,           /* ... Not supported yet */
    IPC_APP_JPEG  = 0x0010,           /* ... Not supported yet */
    IPC_APP_BMP   = 0x0020, 
    IPC_APP_SO3   = 0x0040,           /* EVRC-A   */
    IPC_APP_SO17  = 0x0080,           /* QCELP13K */
    IPC_APP_SO56  = 0x0100,           /* SMV... Not supported yet */
    IPC_APP_SO62  = 0x0200,           /* VMRWB... Not supported yet */
    IPC_APP_SO68  = 0x0400,           /* EVRC-B   */
    IPC_APP_SO70  = 0x0800            /* EVRC-WB.. Not supported yet */
} IpcDspAppIdT;

typedef enum
{
    IPC_IMG_CONV_PREF_ALLOW_FIXED_BLOCKS  = 0x0001, /* 0 means do not use fixed color blocks */
    IPC_IMG_CONV_PREF_PALETTE_IF_POSSIBLE = 0x0002 /* 0 means do not use palettes, just RGBs */
} IpcDspImageConversionPreferenceT;

/*
  App mode commands from CP to DSP
  All apps must support at least START and STOP
 */
typedef enum
{
    IPC_APPMODE_STOP        = 0,                 
    IPC_APPMODE_START
} IpcDspAppModeT;

/*
  App mode responses from DSP to CP
 */
typedef enum
{
   IPC_APPMODE_RSP_STARTED = 1,
   IPC_APPMODE_RSP_STOPPED,
   IPC_APPMODE_RSP_APPLICATION_NOT_FOUND,
   IPC_APPMODE_RSP_UNABLE_TO_START,
   IPC_APPMODE_RSP_UNABLE_TO_STOP
} IpcCpAppModeRspT;

typedef  PACKED   struct
{
   uint16   InputFileId;
   uint16   OutputFileId;
   uint16   ByteTransferSize; /* How many bytes read/written each time */
} IpcDsvStartTestAppT;


typedef  PACKED   struct
{
   uint16   InputFileId;
   uint32   InitialFileOffsetBytes;
} IpcDsvStartMP3AppT;


typedef  PACKED   struct
{
   uint16   InputFileId;
   uint16   MaxAvailableWidthPixels;
   uint16   MaxAvailableHeightPixels;
} IpcDsvStartJPEGAppT;

typedef PACKED struct
{
    uint16   InputFileId;
} IpcDsvStartVocoderAppT;


typedef PACKED union
{
   IpcDsvStartTestAppT     StartTestApp;
   IpcDsvStartMP3AppT      StartMP3App;
   IpcDsvStartJPEGAppT     StartJPEGApp;
#if (SYS_ASIC == SA_CBP55)
   IpcDsvStartBmpAppT      StartBmpApp;
   IpcDsvStartVocoderAppT  StartVocoderApp;
#endif
} IpcDsvAppParamsT;

/* App mode messaging is done through regular IPC mailbox mechanism */

/* IPC_DSV_APP_MODE_MSG message structure */
typedef PACKED struct 
{
   uint16            AppId;               /* Which application (IpcDspAppIdT)  */
   uint16            AppMode;             /* Start, stop, etc. (IpcDspAppModeT)*/

   /* 
      App-specific data, 0 or more 16-bit words. Depends on the state of application
   (i.e. resuming an app may involve sending different data than starting an app)

   For example, to start MP3 only one 16-bit word is needed -- input file id.

   When resuming an app, CP will send back the data that the DSP passed to CP
   when it was suspended.
   */
                         
   IpcDsvAppParamsT  AppParams;  
} IpcDsvSendAppModeMsgT;

/* IPC_CP_APP_MODE_RSP_MSG message structure */
typedef PACKED struct 
{
   uint16         AppId;               /* Which application (IpcDspAppIdT) */
   uint16         AppModeRsp;       /* Response code (IpcCpAppModeRspT) */

   /*
   DSP may use this area to pass back to CP any app-specific data.
   For example, 
   may be used to save app state at SUSPEND instant (not interpreted by CP) or
   to pass back some status information at the START (interpreted by CP).
    */
   uint16         AppDataNumWords;                          
   uint16         AppData[1]; 
} IpcCpAppModeRspMsgT;

/* IPC_CP_APP_STATUS_MSG message structure */
typedef enum
{
    IPC_APP_CHAN_NOT_READY_ERR
} IpcAppStatusT;

typedef PACKED struct 
{
   uint16           AppId;        /* Which application (IpcDspAppIdT)   */
   uint16           AppStatus;    /* Application status (IpcAppStatusT) */
   uint16           FatalErrFlag; /* TRUE if this is a fatal error      */
#if (SYS_ASIC == SA_CBP55)
   uint16           ErrCount;     /* # times this error occurred        */
#endif
} IpcCpAppStatusMsgT;






/* Image display related messages (for JPEG and possibly other */
/* imaging applications).                             */

/* Regular IPC message. */
typedef PACKED struct {
   /* DSP will tell CP how many pixels will be occupied */
   /* by an image (based on the initial DSP message     */
   /* that specified the available screen real estate). */
   /* Obviously the width and height here must not      */
   /* exceed the available area.                        */
   /* CP will decide how to position the image within   */
   /* the available space (e.g. center it in a larger   */
   /* rectangle).                             */
   uint16      NumXpixelsToBeUsed;
   uint16      NumYpixelsToBeUsed;
} IpcCpDisplayImageSize;


/* Regular IPC message. */
typedef PACKED struct {
   /* Do we need it at all??????? Probably not! */

   uint16      NumXpixelsToBeUsed;
   uint16      NumYpixelsToBeUsed;
} IpcDspDisplayImageSizeResponseT;

#if (SYS_ASIC == SA_CBP55)
 
typedef struct
{
    uint16 DftSize;
    uint16 DftStage;
    uint16 *Fft1LupP;
    uint16 *PhsTblP;
    uint16 *BitRevTabP;	
} DftCfgParmsT;

typedef PACKED struct
{
   int16    Indx;              
   int16    BufIndxStep;      
   int16    DftBufSize;       
   int16    BufOfst;          
   int16    LastStep;          
   int16    LastExp;           
   int16    AdjExp;            
   int16    ChanPtr;           
   int16    StepSzeExp;        
   int16    CorrIndx;          
   int16    PartCntlPtr;       
   int16    ErrPtr;            
   int16    TmpBufSize;       
   int16    RndValue;         
   int16    TmpQ;              
   int16    *CoeffPtr;         
   int16    DC_Value;          
   int16    FO_Value;          
   int16    PartNum;           
   int16    ConstrPos;        
} IpcDsvAudioAecTempParamsT;

/* When AppServiceId = IPC_APP_CHAN_SERVICE_CODE_DOWNLOAD, the DspAppMessageP->CmdHdr.AppId
indicates whether we're doing DSPV or DSPM dynamic code download */
typedef enum
{
   IPC_APP_CHAN_CODE_DOWNLOAD_SOURCE_DSPV = 1,
   IPC_APP_CHAN_CODE_DOWNLOAD_SOURCE_DSPM
}IpcDspAppChannelCodeDownloadSourceT;


#endif




#endif
