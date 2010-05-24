/*****************************************************************************

 *****************************************************************************/   

#ifndef _HWDRFAPI_H_
 #define _HWDRFAPI_H_


/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/
#include "sysdefs.h"
#include "sysparm.h"

#include "hwdapi.h"

/*----------------------------------------------------------------------------
 Global Defines and Macros
----------------------------------------------------------------------------*/
/* Check to see if SYS_OPTION_RF_HW is correctly defined for target build */
#if (SYS_OPTION_RF_HW != SYS_RF_LSI_V2_7 && \
     SYS_OPTION_RF_HW != SYS_RF_MAXIM_V3 && \
     SYS_OPTION_RF_HW != SYS_RF_CONEX_V1 && \
     SYS_OPTION_RF_HW != SYS_RF_MAXIM_DCR_V4 && \
     SYS_OPTION_RF_HW != SYS_RF_PHILIPS_R2A && \
     SYS_OPTION_RF_HW != SYS_RF_GENERIC_DCR && \
     SYS_OPTION_RF_HW != SYS_RF_GCT_DCR)
   #error ERROR_RF_HW_OPTION_NOT_CORRECTLY_DEFINED
#endif

/* TSB58-B Band Class Assignments */
#define HWD_BAND_CLASS_CELLULAR   0
#define HWD_BAND_CLASS_PCS        1
#define HWD_BAND_CLASS_TACS       2
#define HWD_BAND_CLASS_JTACS      3
#define HWD_BAND_CLASS_KOREAN_PCS 4
#define HWD_BAND_CLASS_NMT        5
#define HWD_BAND_CLASS_IMT2000    6

/* #defines for idle mode AMPS RF settling time */
#define HWD_AMPS_IDLE_MODE_LATENCY_8KTICKS       2  /* 2 8K clock cycles => 250usec latency */

/* Internal PLL settling time - 1 ms */
#define HWD_INT_PLL_SETTLE_MS                  1

#if ((SYS_OPTION_RF_HW == SYS_RF_MAXIM_DCR_V4) || (SYS_OPTION_RF_HW == SYS_RF_GENERIC_DCR) \
      || (SYS_OPTION_RF_HW == SYS_RF_GCT_DCR) )
  #define HWD_AMP_FWDCH_RXAGCTARGETME_DEFAULT   4864           
  #define HWD_AMP_FWDCH_RXAGCTARGETLO_DEFAULT   3584
#else
  #define HWD_AMP_FWDCH_RXAGCTARGETME_DEFAULT   SYS_DSPV_AMP_FWDCH_RXAGCTARGETME_DEFAULT
  #define HWD_AMP_FWDCH_RXAGCTARGETLO_DEFAULT   SYS_DSPV_AMP_FWDCH_RXAGCTARGETLO_DEFAULT
#endif
/* #defines for PLL Lock Detect Circuitry */
#define HWD_PLL_POLARITY_ACTIVE_LOW   0x0000
#define HWD_PLL_POLARITY_ACTIVE_HIGH  0x0001
#define HWD_PLL_LOCK_ENABLE_MASK      0x0002

#define HWD_DEFAULT_PLL_LOCK_PULSE_THRESHOLD  7 /* Number of clk19 cycles for PLLU interrupt to occur */
                                   
/* The following global define is used during PCS to determine the threshold
** at which the RF solution switches to a high band Tx filter (on RF 2.7 this
** is controlled by GPIO_46 */
#define HWD_RF_PCS_HIGH_BAND_CHANNEL_THRESHOLD  600

/* #defines for RF register programming following deep sleep */
#define HWD_SLOTTED_RF_FORCE_LOADING   TRUE
#define HWD_SLOTTED_RF_NORMAL_LOADING  FALSE

/*----------------------------------------------------------------------------
     Delayed loading addresses     
------------------------------------------------------------------------------*/

#define DEV_CF_TXON_CMP0       0xE9C8
#define DEV_CF_TXON_CMP1       0xE9C9
#define DEV_CF_TXON_CMP2       0xE9CA
#define DEV_CF_TXON_CMP3       0xE9CB
#define DEV_CF_TXON_CMP4       0xE9CC
#define DEV_CF_TXON_CMPINT     0xE9CD

#define DEV_CF_PDM_CMP0        0xE9D0
#define DEV_CF_PDM_CMP1        0xE9D1
#define DEV_CF_PDM_CMP2        0xE9D2
#define DEV_CF_PDM_CMP3        0xE9D3
#define DEV_CF_PDM_CMP4        0xE9D4
 

/*----------------------------------------------------------------------------
    PA/LNA Configuration
------------------------------------------------------------------------------*/

#define RFON0_SELECT    0x0001
#define RFON1_SELECT    0x0002
#define RFON2_SELECT    0x0004
#define RFON3_SELECT    0x0008
#define RFON4_SELECT    0x0010
#define RFON5_SELECT    0x0020
#define RFON6_SELECT    0x0040
#define RFON7_SELECT    0x0080

/* NOTE: The following TXON select definitions must match the
** bit fields located in the DSPM DEV_TX_ON_DLYMODE_DIN register */
#define TXON0_SELECT    0x0001
#define TXON1_SELECT    0x0002
#define TXON2_SELECT    0x0004
#define TXON3_SELECT    0x0008
#define TXON4_SELECT    0x0010
#define TXINT_SELECT    0x0020

/*----------------------------------------------------------------------------
 Global Typedefs
----------------------------------------------------------------------------*/

typedef struct
{
    uint16 PaPdmAddress[4];
    uint16 PaPdmValue[4];
    uint16 PaPdmDlyMode[4];
}HwdGainControlUnitT;

/* TX AGC update types for power control bits */
typedef enum
{
   HWD_TX_AGC_INC,
   HWD_TX_AGC_DEC
} HwdTxAgcIncDecT;

/* TX AGC PA gain state types */
typedef enum
{
   HWD_LOW_GAIN_STATE,
   HWD_MED_GAIN_STATE,
   HWD_HIGH_GAIN_STATE
} HwdAgcGainStateT;

/* All values associated with TX gain. Includes TX AGC and PA */
typedef struct
{
   int32              PwrDbm;        /* Tx power at antenna, in dBm with Q = L1D_DB_Q   */
   HwdAgcGainStateT   HystState;     /* PA Hysteresis state                             */
   uint16             HwOnValue;     /* Hardware (DAC) value for TX AGC on-value        */
   uint16             HwOffValue;    /* Hardware (DAC) value for TX AGC off-value       */
} HwdTxAgcStateT;
   
typedef struct
{
   int16 TxMdmDelay;                /* delay added to the Tx timing to line up Tx waveform
                                     * with Rx waveform at the antenna. Initially defaulted to 
                                     * HWD_TX_MODEM_DLY_Q3.
                                     */
   uint16 ClockSettleTimeSlpClkCnt; /* settle time for the internal clocks, in units of 32768kHz 
                                     * sleep clock counts. This is the sum of the TCXO settle 
                                     * time and the internal PLL settle time.
                                     */
   uint16 RxRfSettleDelaysMs;       /* delays necessary for RF power up, in ms.  */
   uint16 RxVcoSettleTimeMs;        /* settle time of the RF Rx Vco ONLY, in ms.  */
   uint16 RxPllSettleTimeMs;        /* settle time of the RF Rx PLLs = 
                                       RxRfSettleDelaysMs+RxVcoSettleTimeMs+SoftwareOverhead(3ms), in ms.  */
   uint16 RxPllBackoffTimeMs;       /* PLL backoff time used for slotted calculations
                                       includes PLL settling + overhead prior to Rx AGC start, in ms.  */
   uint16 RxPllChannelSettle;       /* settle time for channel HO, in ms */
   uint16 RxPllAmpsSettle;          /* settle time for Amps VCO tuning, in ms */
   uint16 TxPllSettleTimeMs;        /* settle time of the RF Tx PLL, in ms.  */
   uint16 MiniAcqTime;              /* time for mini acq - in us */
   uint32 SlottedPagingRxPllSettle; /* Slotted Paging RF Rx Pll settle in symbols.*/
   uint32 SlottedPagingRxAgcSettle; /* Slotted Paging RxAgc settle in symbols.*/
   uint32 SlottedPagingBackoffTime; /* Slotted Paging Backoff time from the pch in ms.*/
   bool   SlottedPagingRfForceLoad; /* Boolean to determine if RF registers need to be
                                     * to be re-initialized following deep sleep - setting
                                     * of this flag is dependant on RF solution.
                                     */
} HwdRfConstantsT;

/***** NOTE: Assigned values should not be changed due to their use as offsets
             into PLL programming arrays. */

typedef enum 
{
  HWD_RF_PLL_BAND_PCS_1900      =   0,
  HWD_RF_PLL_BAND_AMPS          =   1,
  HWD_RF_PLL_BAND_CDMA_CELLULAR =   2,
  HWD_RF_PLL_BAND_CDMA_T53      =   3,
  HWD_RF_PLL_BAND_KOREAN_PCS    =   4,
#ifdef BAND_CLASS_5_SUPPORTED          /* XUGANG_450M_CHANGE*/
  HWD_RF_PLL_BAND_NMT_450       =   5, 
#endif  
  HWD_RF_PLL_BAND_UNDEFINED     
}HwdRfPllBandT;

typedef enum 
{
   HWD_CALIBR_BAND_CELLULAR = 0,  
   HWD_CALIBR_BAND_PCS      = 1,       
#ifdef BAND_CLASS_5_SUPPORTED
   HWD_CALIBR_BAND_450M     = 2,
   HWD_CALIBR_BAND_AMPS     = 3
#else
   HWD_CALIBR_BAND_AMPS     = 2
#endif 
}HwdCalibrBandT;   

   
/*----------------------------------------------------------------------------
 Global Data
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Function Prototypes
----------------------------------------------------------------------------*/

/*****************************************************************************
 
  FUNCTION NAME:   HwdRfPllChannelSet

  DESCRIPTION:     Programs the RF Plls for the specified channel.
                   NOTE: This routine should be used outside the HWD for testing or 
                   calibration purposes only!!!

  PARAMETERS:      Band    - CDMA band class
                   Channel - Channel number

  RETURNED VALUES: None

*****************************************************************************/
extern void  HwdRfPllChannelSet(HwdRfPllBandT PllBand, uint32 Channel);

/*****************************************************************************
 
  FUNCTION NAME:    HwdPllChannelSet
  
  DESCRIPTION:      This function is used by L1D to tune the RF PLLs to a desired
                    band and frequency channel. A callback function is set to
                    generate an RF tune complete signal back to L1D once the RF
                    PLLs have settled.

                    NOTE: This routine only accounts for settling of the RF PLLs
                    and not for any other RF components that may need to settle
                    because the RF transceiver may be powered down prior to tuning
                    (e.g. Rx VCO, etc.)
  
  PARAMETERS:       CdmaBand    -- CDMA Band 
                    FreqChan    -- Frequency Channel
                    RspTaskId   -- Task ID mailbox to send designated signal to
                    RspSignal   -- Signal to generate when tuning is complete
                    ForceBand   -- Boolean, used to force PLL band setup

  RETURNED VALUES:  None

*****************************************************************************/
extern void HwdPllChannelSet(HwdRfPllBandT  PllBand, 
                             uint32         FreqChan, 
                             ExeTaskIdT     RspTaskId, 
                             ExeSignalT     RspSignal, 
                             bool           ForceBand);

/*****************************************************************************

  FUNCTION NAME:   HwdRfConstantsPtrGet
  
  DESCRIPTION:     Returns pointer to the modem constants structure, which provides
	               hardware constants required by L1D.
	
  PARAMETERS:      None
	  
  RETURNED VALUES: pointer to HwdRfConstants.
		
*****************************************************************************/
extern HwdRfConstantsT* HwdRfConstantsPtrGet(void);

/*****************************************************************************
 
  FUNCTION NAME:   SetupRfConstans

  DESCRIPTION:     Setups RF Constants

  PARAMETERS:      TxOnId - TX_ON identifier using DEV_TX_ON_DLYMODE_DIN identifiers

  RETURNED VALUES: uint16 - DSPM-accessed address for TX_ON delay comporartors.

*****************************************************************************/
void SetupRfConstans(void);

/*****************************************************************************
 
  FUNCTION NAME:   HwdRfSetRxTxIQSwap

  DESCRIPTION:     Sets Rx Tx IQ swap.

  PARAMETERS:      uint32, bool, bool

  RETURNED VALUES: None

*****************************************************************************/
void HwdRfSetRxTxIQSwap(uint8 Band, uint8 RxSwap, uint8 TxSwap);

/*****************************************************************************
 
  FUNCTION NAME:   HwdRfGetRxTxIQSwap

  DESCRIPTION:     Gets Rx Tx IQ swap.

  PARAMETERS:      uint32, bool*, bool*

  RETURNED VALUES: None

*****************************************************************************/
void HwdRfGetRxTxIQSwap(uint8 Band, uint8 *RxSwap, uint8 *TxSwap);


/*****************************************************************************
 
  FUNCTION NAME:    HwdRfDcrToggleDcCal

  DESCRIPTION:      This function toggles DC Cal line.
  
  PARAMETERS:       None

  RETURNED VALUES:  None

*****************************************************************************/
void  HwdRfDcrToggleDcCal(void);






#endif
