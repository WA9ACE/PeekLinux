#ifndef IPCDEFS_H
#define IPCDEFS_H
/*****************************************************************************
 
  
*****************************************************************************/

#include "ipcapi.h"
#include "exeapi.h"
#include "sysdefs.h"

/*------------------------------------------------------------------------
* Declare global constants used in IPC unit
*------------------------------------------------------------------------*/

/* Define mbox circular buffer size in uint16 units */
#define IPC_DSPM_RX_CIRCULAR_BUFFER_SIZE   (IPC_CTL_MBOX_DSPM_FWD_SIZE * 2)
#define IPC_DSPV_RX_CIRCULAR_BUFFER_SIZE   (IPC_CTL_MBOX_DSPV_FWD_SIZE * 2)

/* Define IPC task signals */
#define IPC_SIGNAL_DSPM_CTL_MBOX   EXE_SIGNAL_1
#define IPC_SIGNAL_DSPV_MBOX       EXE_SIGNAL_2
#define IPC_SIGNAL_DSPM_SYNC       EXE_SIGNAL_3
#define IPC_SIGNAL_DSPV_SYNC       EXE_SIGNAL_4
#define IPC_SIGNAL_WAKEUP          EXE_SIGNAL_5  /* Used to force a wakeup */

/* Define IPC - DSP sync character */
#define IPC_SYNC_CHAR              0xFAFA

/* Define DSP -> CP routing table structure */
typedef struct 
{
    uint32         DspMsgId; 
    ExeTaskIdT     TaskId;  
    ExeMailboxIdT  MailboxId;  
    uint32         CpMsgId; 
	bool           CPSysTime;
} IpcRoutingTableT;

/* IPC task related messages and data structures */

/* IPC task msg Ids */
typedef enum 
{
   IPC_SEND_AUDIO_SSO_CONNECT_MSG = 0,
   IPC_SEND_AUDIO_SSO_DISCONNECT_MSG,
   IPC_SET_AUDIO_LOOPBACK_MODE_MSG,
   IPC_START_ENCODER_DECODER_TEST_MSG,
   IPC_START_ENCODER_TEST_MSG,
   IPC_START_DECODER_TEST_MSG,
   IPC_CLEAR_FWD_STATS_MSG,

   /* Multimedia Application commands/responses */
   IPC_APP_MODE_MSG,                /* Start/Stop cmd from MMI/UI              */
   IPC_APP_CONFIGURE_CHANNELS_MSG,  /* Config channel cmd from DSPV            */
   IPC_APP_CHAN_DATA_RSP_MSG,       /* Shared Memory Data response from MMI/UI */
   IPC_DYN_CODE_DNLD_MSG            /* Dyn Code Download request from IPC ISR  */

} IpcMsgIdT;

typedef enum 
{
   IPC_ROUTE_SPEECH_PACKETS_NONE = 0,
   IPC_ROUTE_SPEECH_PACKETS_TO_LMD,
   IPC_ROUTE_SPEECH_PACKETS_TO_DSPV,
   IPC_ROUTE_SPEECH_PACKETS_TO_VTST
}IpcDspvSpeechRoutingTypeT;

/* IPC_APP_CHAN_DATA_RSP_MSG Structure */
typedef PACKED struct
{
   uint16      ChannelId;
   uint16      DataSize;         /* Size in WORDS of data              */
   uint16      *DataP;           /* Pointer to data to be sent to DSPV */
} IpcAppChanDataRspMsgT;
typedef IpcAppChanDataRspMsgT IpcDynCodeDnldMsgT;

   /*
    *  On a CDS4 board with a 4.05 RAM chip, the check is done
    *  before configuring DSPM/V to make sure CP won't send
    *  messages to a 'dead' DSPM/V.
    */
#if ((SYS_BOARD == SB_CDS4) && (SYS_ASIC == SA_RAM))
extern bool IpcDspmInterfaceDisabled;
#endif

#if ((SYS_BOARD == SB_CDS4) && (SYS_ASIC != SA_ROM))
extern bool IpcDspvInterfaceDisabled;
#endif

extern uint16   IpcSharedMemChanCount;
extern uint16   IpcSharedMemControl;
extern uint16   IpcSharedMemChanSize[];


/*****************************************************************************
 
  FUNCTION NAME: IpcHisrDspmCtlMbox

  DESCRIPTION:

    This routine is the control mailbox HISR for the DSPM processor. 
    It signals the IPC task that the control mailbox LISR for the DSPM 
    processor is complete. This indicates to the IPC task that data 
    from the DSPM processor ready to be processed is in the circular buffer.
  
  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/

extern void IpcHisrDspmCtlMbox(void);

/*****************************************************************************
 
  FUNCTION NAME: IpcHisrDspmDBufMbox

  DESCRIPTION:

    This routine is the direct buffer mailbox HISR for the DSPM processor. 
    It routes direct buffer mailbox messages either to PSW or to LMD based
    on the physical channel type indicated in the direct buffer mailbox
    signaling buffer. To route messages to PSW the mailbox contents are read
    into a message buffer and then sent to PSW. In this case the HISR frees
    the mailbox memory. To route to LMD, a signal is sent to LMD. LMD then
    reads the mailbox contents and it then frees the mailbox memory.
  
  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/

extern void IpcHisrDspmDBufMbox(void);


/*****************************************************************************
 
  FUNCTION NAME: IpcHisrDspv

  DESCRIPTION:

    This routine is the HISR for the DSPV processor. It signals
    the IPC task that the LISR for the DSPV processor is complete.
    This indicates to the IPC task that data from the DSPV processor 
    is ready to process in the circular buffer.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/

extern void IpcHisrDspv(void);


/*****************************************************************************
 
  FUNCTION NAME: IpcLisrDspm

  DESCRIPTION:
   
    This routine is the LISR which services the interrupt from the
    DSPM processor.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/

extern void IpcLisrDspm(void);


/*****************************************************************************
 
  FUNCTION NAME: IpcLisrDspv

  DESCRIPTION:

    This routine is the LISR which services the interrupt from the
    DSPV processor.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/

extern void IpcLisrDspv(void);

/*****************************************************************************
 
  FUNCTION NAME: IpcInit

  DESCRIPTION:

    This routine performs the IPC task message services initialization,
    which consists of initializing the IPC queue and the free buffer
    linked list.

  PARAMETERS:
      
    None.

  RETURNED VALUES:

    None.

*****************************************************************************/

extern void IpcInit(void);

/*****************************************************************************
 
  FUNCTION NAME: IpcReadDirectBuffer

  DESCRIPTION:

    This routine reads the contents of the direct buffer mailbox into memory
    supplied by the calling routine. The forward physical channel type is 
    returned as is the number of words in the channel. After the data is read 
    the direct buffer mailbox is made available again to the DSPM.

  PARAMETERS:

    DataP     - Pointer to memory into which the direct buffer mailbox's 
                contents are written.
    NumWordsP - Pointer to number of words in the direct buffer mailbox data area.
    PhysChanP - Pointer to forward physical channel type.
    DBufNum   - Identifies which direct buffer to be read

  RETURNED VALUES:
    
    None

*****************************************************************************/
extern void IpcReadDirectBuffer(uint16 *DataP, uint16 *NumWordsP, 
                                FwdChnlTypeT *PhysChanP, uint16 DBufNum);

/*****************************************************************************
 
  FUNCTION NAME: IpcWriteDirectBuffer

  DESCRIPTION:

    This routine writes data to the direct buffer mailbox from memory
    supplied by the calling routine. The reverse physical channel type is 
    passed as is the number of words, both of these are written to the
    reverse channel signaling buffer. After the data is written 
    to the direct buffer mailbox the DSPM is notified through the handshake
    register.

  PARAMETERS:

    DataP    - Pointer to memory into which the direct buffer mailbox's 
               contents are written.
    NumWords - Number of words to be writeen to the direct buffer mailbox data area.
    PhysChan - Reverse channel physical channel type.
    FundRate - Fundamental channel rate.

  RETURNED VALUES:
    
    None

*****************************************************************************/
extern void IpcWriteDirectBuffer(uint16 *DataP, uint16 NumWords, RChnlTypeT PhysChan, uint16 FundRate);
extern void IpcWriteDirectBufferMultiple(uint16 *DataP, uint16 NumWords, uint16 ChanRdy, uint16 FundRate);

/*****************************************************************************
 
  FUNCTION NAME: IpcClearFwdStats

  DESCRIPTION:

    This routine is called at power up or when an ETS command is received to
    clear the forward frame statistics (bad, total and FERs).

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/

extern void IpcClearFwdStats(void);

/*****************************************************************************
 
  FUNCTION NAME:  IpcUpdateFwdSchStats

  DESCRIPTION:    This routine updates the SCH statistics.  It is needed
                  because now LMD handles all SCH statistics.

  PARAMETERS:     TotalFrames:  TOTAL number of frames received
                  BadFrames:    number of BAD frames received

  RETURNED VALUES:  None

*****************************************************************************/

extern void IpcUpdateFwdSchStats ( uint16 TotalFrames, uint16 BadFrames );

/*****************************************************************************

  FUNCTION NAME: IpcDSPWatchdog

  DESCRIPTION:

    This is a watchdog that monitors the DSP livelihood.
    If there have been no activities from either DSP for
    SYS_OPTION_HWD_WATCHDOG_VALUE msecs the whole chip will be reset.

    NOTE:  It runs in the context of an HISR because it is called from an HISR.

  PARAMETERS:

    None

  RETURNED VALUES:

    None

*****************************************************************************/

extern void IpcDSPWatchdog ( void );

/*****************************************************************************
 
  FUNCTION NAME: IpcFwdChannelDisable

  DESCRIPTION:

    This routine is used to notify IPC when DSPm forward channels
    are being torn down.

  PARAMETERS:

    Channel - bitmap indicating the channel being disabled
    
  RETURNED VALUES:

    None

*****************************************************************************/
extern void IpcFwdChannelDisable(uint16 Channel);

/*****************************************************************************
 
  FUNCTION NAME: IpcSetWakeUpTime

  DESCRIPTION:
      This function is called when we wake up from deep sleep
      determine the system wakeup time. It's necessary to avoid fake forward
      frame missing errors.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/
extern void IpcSetWakeUpTime(void);

/*****************************************************************************
 
  FUNCTION NAME: IpcLisrSharedMem

  DESCRIPTION:

    This routine is the Shared Memory LISR.  It simply activates the
    corresondent Hisr.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/

void IpcLisrSharedMem(void);


/*****************************************************************************
 
  FUNCTION NAME: IpcHisrShareMem

  DESCRIPTION:

    This routine is the HISR for the Shared Memory. It unloads data
    from the Shared Memory to a static buffer and sends a message to
    an upper layer task to notify the arrival of data.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/

void IpcHisrShareMem(void);

void IpcSharedMemSpy (uint16 SpyId, uint16 *WordPtr,
                      uint16 AppServiceId, uint16 TotalNumBytes);
extern void    IpcAppDynDnldActive (bool Downloading);
extern uint32 *IpcAppsGetChanDataAddr (void);



#endif
