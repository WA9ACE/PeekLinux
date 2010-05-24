#ifndef _HWDAMPSAPI_H_
#define _HWDAMPSAPI_H_


/*----------------------------------------------------------------------------
 Defines and Macros
----------------------------------------------------------------------------*/
#define CLR_BIT_0             0
#define SET_BIT_0             1
#define CDMA_MODE             CLR_BIT_0
#define AMPS_MODE             SET_BIT_0

/* Return true while in AMPS mode and false in CDMA */
#ifndef OTTS
 #ifdef SYS_OPTION_AMPS
  #define HwdAmpsModeIsActive()     (HwdRead(HWD_AMPS_MODE_SEL) == AMPS_MODE)
 #else
  #define HwdAmpsModeIsActive()     (FALSE)
 #endif
#else
#define HwdAmpsModeIsActive()		(FALSE)  /* until we simulate amps messages this will do */
#endif

#ifdef SYS_OPTION_AMPS
/* Program a value into the single shot timer hardware. The FPGA will issue
   an interrupt when this time expires. */
#define HwdAmpsSetSST(Duration)		HwdWrite(HWD_SSTIMER_INIT, Duration)

/* Set up the Amps Mode Select Register */
#define HwdAmpsSetModeSelect(ModeSel)	HwdWrite(HWD_AMPS_MODE_SEL, ModeSel)

/* Set up the TX Swap bit for AMPS */
#define HwdAmpsSetTxSwap(Bit)		if (Bit)  HwdWrite(HWD_TX_TEST_CTRL, HwdRead(HWD_TX_TEST_CTRL) | 0x02);  \
	else HwdWrite(HWD_TX_TEST_CTRL, HwdRead(HWD_TX_TEST_CTRL) & 0xfd)

/* Set up the TX format bit for AMPS: 0 => 2's complement; 1 => offset binary */
#define HwdAmpsSetTxFormat(Bit)		if (Bit) HwdWrite(HWD_TX_TEST_CTRL, HwdRead(HWD_TX_TEST_CTRL) | 0x04); \
	else HwdWrite(HWD_TX_TEST_CTRL, HwdRead(HWD_TX_TEST_CTRL) & 0xfb)

/* Set up the rate select for AMPS: 0 => 4.8 kHz data rate; 1 => 9.6 kHz data rate */
#define HwdAmpsRateSelect(Bit)		if (Bit) HwdWrite(HWD_TX_TEST_CTRL, HwdRead(HWD_TX_TEST_CTRL) | 0x40); \
	else HwdWrite(HWD_TX_TEST_CTRL, HwdRead(HWD_TX_TEST_CTRL) & 0xbf)

/* New - new value to write into the register */
#define HwdAmpsSetRxWbdFskCtrl(New) HwdWrite(HWD_RXD_CNTL, New)

/* Set up the Recieve Wide Band Data FSK Enable Register */
#define HwdAmpsSetRxWbdFskEn(New) HwdWrite(HWD_RXD_FSKEN, New)

extern uint32 temp;
/* Reset the Tx Wideband Data Enable register */
#define HwdAmpsResetRxWbdFskEn()  \
 { uint32 temp=0; /* defined here for delay in macro */ \
   SysIntDisable (SYS_ALL_INT); \
   HwdWrite(HWD_RXD_FSKEN, CLR_BIT_0); \
   temp=0;temp++;temp++; \
   HwdWrite(HWD_RXD_FSKEN, SET_BIT_0); \
   SysIntEnable (SYS_ALL_INT);  \
 }

/*----------------------------------------------------------------------------
 Typedefs
----------------------------------------------------------------------------*/

typedef enum
{
   DSPV_INIT_POWERUP = 0,
   DSPV_INIT_SWITCHOVER
} HwdAmpsDspvInitModeT;

/* Define AMPS RF Function Pointer structure */
typedef struct 
{
   void (*AmpsRadioOn)(void);
   void (*AmpsKeyRadioStage1)(void);
   void (*AmpsKeyRadioStage2)(void);
   void (*AmpsDeKeyRadio)(void);
   void (*AmpsFastRfOff)(void);
   void (*AmpsFastRfOn)(void);
   void (*AmpsSetIQLevel)(void);
   void (*AmpsRestoreIQLevel)(void);
   void (*AmpsSlowRfOff)(void);
   void (*AmpsSlowRfOn)(void);
   void (*AmpsAllRfBackOn)(void);

} AmpsRfFuncPtrTblT;

/*----------------------------------------------------------------------------
 Function prototypes
----------------------------------------------------------------------------*/
int16 HwdAmpsRssiDbToDbm ( uint16 RssiDbVal, uint16 CurrentPdmVal, uint16 Target );

void  HwdAmpsNullFunction ( void );
void  HwdAmpsFastRfOff ( void );
void  HwdAmpsFastRfOn ( void );
void  HwdAmpsSlowRfOff ( void );
void  HwdAmpsSlowRfOn ( void );
void  HwdAmpsAllRfBackOn ( void );


/*****************************************************************************
 
  FUNCTION NAME: HwdAmpsPllLockDetectEnable

  DESCRIPTION:

  PARAMETERS:
    Enable Flag
    
  RETURNED VALUES:
    None.
*****************************************************************************/
void  HwdAmpsPllLockDetectEnable ( bool Enable );

/*****************************************************************************
 
  FUNCTION NAME: HwdAmpsSetTxOn

  DESCRIPTION:
    Sets the value of TxOn.

  PARAMETERS:
    On - New value for TxOn.
    
  RETURNED VALUES:
    None

*****************************************************************************/
void HwdAmpsSetTxOn ( bool On );

/*****************************************************************************
 
  FUNCTION NAME: HwdAmpsTxOn

  DESCRIPTION:
    Return TRUE if the device is currently transmitting in AMPS mode.

  PARAMETERS:
    None
    
  RETURNED VALUES:
    Current value for TxOn flag.

*****************************************************************************/
bool HwdAmpsTxOn ( void );

/*****************************************************************************
 
  FUNCTION NAME: HwdAmpsDeKeyRadio

  DESCRIPTION:

    Deactivate the FM carrier circuit in the radio.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/
void HwdAmpsDeKeyRadio( void );

/*****************************************************************************
 
  FUNCTION NAME: HwdAmpsSetCurrentAgc

  DESCRIPTION:
    Set the new value for Tx AGC. In effect, this will set the Tx Power level.

  PARAMETERS:
    NewPowerLeveldBm - Desired Tx AGC Power in dBm with Q=L1D_DB_Q.
    
  RETURNED VALUES:
    None

*****************************************************************************/
void HwdAmpsSetCurrentAgc( int16 NewPowerLeveldBm );

/*****************************************************************************
 
  FUNCTION NAME: HwdAmpsKeyRadioStage1

  DESCRIPTION:
    Do the first stage of Key Radio.

  PARAMETERS:
    None
    
  RETURNED VALUES:
    None

*****************************************************************************/
void HwdAmpsKeyRadioStage1( void );

/*****************************************************************************
 
  FUNCTION NAME: HwdAmpsKeyRadioStage2

  DESCRIPTION:
    Do the second and final stage of Key Radio.

  PARAMETERS:
    None
    
  RETURNED VALUES:
    None

*****************************************************************************/
void HwdAmpsKeyRadioStage2( void );

/*****************************************************************************
 
  FUNCTION NAME: HwdAmpsCdmaSwitchover

  DESCRIPTION:
    Handle all the details of the Cdma to Amps and Amps to Cdma switchover.

  PARAMETERS:
    SysAmpsCdmaSelectT ModeSel = SYS_SET_CDMA_MODE, SYS_SET_AMPS_MODE
    
  RETURNED VALUES:
    none

  NOTES:

*****************************************************************************/
void HwdAmpsCdmaSwitchover(SysAmpsCdmaSelectT ModeSel);

/*****************************************************************************
 
  FUNCTION NAME: HwdAmpsDspvInit

  DESCRIPTION:
    Send all the appropriate DSP initialization messages down to the DSPV.

  PARAMETERS:
    None.
    
  RETURNED VALUES:
    None

*****************************************************************************/
void HwdAmpsDspvInit( HwdAmpsDspvInitModeT Mode, uint16 TxPolarity, uint16 RxPolarity, uint16 Stream );

#endif

void HwdAmpsCdmaSwitchoverForced(SysAmpsCdmaSelectT ModeSel);





#endif
