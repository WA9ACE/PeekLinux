

#ifndef BALPSW_H
#define BALPSW_H

#include "sysdefs.h"
#include "balapi.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BAL_PSW_MAX_REG_TASK           5

/***************************************************************************
                    BAL PSW event definitions
****************************************************************************/

typedef enum
{
  BAL_PSW_EVT_INCOMING_CALL_ALERT_WITH_INFO_MSG,  
  BAL_PSW_EVT_INCOMING_CALL_ALERT_WITH_MORE_INFO_MSG, 
  BAL_PSW_EVT_CALL_DIALING_MSG,	/*A new event added by Chengzhen Huang. MS is on dialing.*/
  BAL_PSW_EVT_CALL_CONNECTED_MSG,
  BAL_PSW_EVT_CALL_ANSWERED_MSG,  /*MS is on conversation.*/
  BAL_PSW_EVT_CALL_ENDED_MSG, 	/*The call is ended.*/
  BAL_PSW_EVT_NWK_RPT_BURST_DTMF_MSG,
  BAL_PSW_EVT_FLASH_ALERT_WITH_INFO_MSG,
  BAL_PSW_EVT_FLASH_ALERT_WITH_MORE_INFO_MSG,
  BAL_PSW_EVT_FEATURE_NOTIFICATION_MSG,
  BAL_PSW_EVT_FEAT_NOTIF_MORE_INFO_MSG,  
  BAL_PSW_EVT_NWK_RPT_HANDOFF_MSG,
  BAL_PSW_EVT_NWK_RPT_LOCK_MSG,
  BAL_PSW_EVT_NWK_RPT_UNLOCK_MSG,
  BAL_PSW_EVT_NWK_RPT_MAINTENANCE_REQUIRED_MSG,
  BAL_PSW_EVT_NWK_RPT_START_CONTINUOUS_DTMF_MSG,
  BAL_PSW_EVT_NWK_RPT_STOP_CONTINUOUS_DTMF_MSG,
  BAL_PSW_EVT_ROAM_INDICATION_MSG,
  BAL_PSW_EVT_PSW_E911_MODE_MSG,
  BAL_PSW_EVT_SERVICE_MSG,
  BAL_PSW_EVT_VOICE_PRIVACY_MSG,
  BAL_PSW_EVT_FLASH_STATUS_MSG,
  BAL_PSW_EVT_PSSTACK_STATUS_MSG,
  BAL_PSW_EVT_AKEY_CHANGE_RSP_MSG,
  BAL_PSW_EVT_NAM_MSG,
  BAL_PSW_EVT_L1D_RSSI_RPT_MSG,
  BAL_PSW_EVT_EV_MAINTENANCE_MSG,
  BAL_PSW_EVT_EV_PAGE_RECEIVED,  
  /*HDQ ADDED for Keypad service provisioning module on 20040622*/
  BAL_PSW_EVT_NAM_RPT_ACTIVE_NAM_MSG,
  BAL_PSW_EVT_NAM_RPT_LOCKSTATUS_MSG,
  BAL_PSW_EVT_LOCK_NAM_RSP_MSG,
  BAL_PSW_EVT_ACTIVE_NAM_RSP_MSG,
  BAL_PSW_EVT_GET_AKEY_CHECKSUM_RSP_MSG,
  /*end*/
  BAL_PSW_EVT_OTASP_IND_MSG,
  BAL_PSW_EVT_OTASP_CON_MSG,
  BAL_PSW_EVT_OTASP_DISCON_MSG,
  BAL_PSW_EVT_SYSTEM_ERROR_IND_MSG,
  BAL_PSW_EVT_NETWOKR_PRE_MSG,
  BAL_PSW_EVT_CUR_PRL_INFO_MSG,
  BAL_PSW_EVT_NONACTIVE_PRL_INFO_MSG,
  BAL_PSW_EVT_SERVICE_STATUS_MSG,
  BAL_PSW_EVT_AMPS_EXT_PROTO_MSG,
  
  BAL_PSW_EVT_ENTER_OOSA,
  BAL_PSW_EVT_BS_CO_ORD_MSG,
  BAL_PSW_EVT_LOC_RESP_MSG,
  BAL_PSW_EVT_LOC_ERROR_MSG,
  BAL_PSW_EVT_LOC_PILOT_PHASE_MEAS_MSG,
  BAL_PSW_EVT_LOC_PSEUDO_RANGE_MSG,
  
  BAL_PSW_EVT_RPT_PKT_STATE_MSG,
  BAL_PSW_EVT_MAX
} BalPswEventIdT;

typedef enum 
{
  BAL_PSW_OTASP_A_BAND,
  BAL_PSW_OTASP_B_BAND,
  BAL_PSW_OTASP_A_BLOCK,
  BAL_PSW_OTASP_B_BLOCK,
  BAL_PSW_OTASP_C_BLOCK,
  BAL_PSW_OTASP_D_BLOCK,
  BAL_PSW_OTASP_E_BLOCK,
  BAL_PSW_OTASP_F_BLOCK,
  BAL_PSW_OTASP_PREFERRED,
  BAL_PSW_MAX_OTASP_CODE
} BalPswOtaspCodeT;


 


/***************************************************************************

  Basic Data Types for PSWF events
  
***************************************************************************/
/* Cellular System types */
#define BAL_PSW_CP_SYSTEM_PCS        0   /* Band 1 and Band 4 CDMA */
#define BAL_PSW_CP_SYSTEM_CELLULAR   1   /* Band 0 CDMA */
#define BAL_PSW_CP_SYSTEM_ANALOG     2   /* Band 0 Analog */ 

/* CDMA Blocks */
#define BAL_PSW_A_BLOCK      0
#define BAL_PSW_D_BLOCK      1
#define BAL_PSW_B_BLOCK      2
#define BAL_PSW_E_BLOCK      3
#define BAL_PSW_F_BLOCK      4
#define BAL_PSW_C_BLOCK      5
#define BAL_PSW_G_BLOCK      6
#define BAL_PSW_H_BLOCK      7

#define BAL_PSW_ALL_BLOCKS   8

#define BAL_PSW_ENABLED     1
#define BAL_PSW_DISABLED    0

#define BAL_PSW_MAX_CALLING_PARTY_NUMBER_SIZE 32

/***************************************************************************

  EVENT NAME: BAL_PSW_EVT_CALL_ENDED_MSG 
  DESCRIPTION:
    Sent when the call is ended for any reason.
  MESSAGE TYPE: 
    PswFCallEndedMsgT - Contains reason for the call ending.
  
****************************************************************************/
typedef enum
{
  BAL_PSW_ORIG_FAIL=0,
  BAL_PSW_ORIG_CANCELED_NDSS,
  BAL_PSW_INTERCEPT,
  BAL_PSW_REORDER,
  BAL_PSW_CC_RELEASE,
  BAL_PSW_CC_RELEASE_SO_REJ,
  BAL_PSW_FNM_RELEASE,
  BAL_PSW_TC_RELEASE_MS,
  BAL_PSW_TC_RELEASE_PDOWN, 
  BAL_PSW_TC_RELEASE_DISABLE, 
  BAL_PSW_TC_RELEASE_BS,
  BAL_PSW_TC_RELEASE_SO_REJECT,
  BAL_PSW_TC_RELEASE_TIMEOUT,
  BAL_PSW_TC_RELEASE_ACK_FAIL, 
  BAL_PSW_TC_RELEASE_FADE, 
  BAL_PSW_TC_RELEASE_LOCK,
  BAL_PSW_SR_FINISHED_MSG
} BalPswCallEndReasonT;

typedef PACKED struct
{
  BalPswCallEndReasonT  CallEndReason;
  PswServiceT CallType;
} BalPswCallEndedMsgT;

/***************************************************************************

  EVENT NAME: BAL_PSW_EVT_VOICE_PRIVACY_MSG
  DESCRIPTION:
    This message is used by PSW to indicate whether voice privacy is on
    or off and is sent whenever there is a state transition.
  MESSAGE TYPE: 
    PswVoicePrivacyMsgT - Indicates whether voice privacy is on or off.
          
****************************************************************************/
typedef PACKED struct
{
  bool VoicePrivacyOn;
} BalPswVoicePrivacyMsgT;


/***************************************************************************

  EVENT NAME: BAL_PSW_EVT_PSSTACK_STATUS_MSG
  DESCRIPTION:
    This message is used by PSW to indicate the current protocol stack status.
  MESSAGE TYPE: 
    PswPsStackStatusMsgT - Indicates the protocol stack status.
          
****************************************************************************/
typedef enum
{
  BAL_PSW_CP_ENABLED,
  BAL_PSW_CP_SHUTTING_DOWN,
  BAL_PSW_CP_SHUTDOWN_COMPLETE
} BalPswPsStackStatusT;

/* BAL_PSW_OTASP_IND_MSG */
typedef enum
{
  BAL_PSW_OTASP_Connect,
  BAL_PSW_OTA_Disconnect,
  BAL_PSW_OTAPA_SessionStart,
  BAL_PSW_OTAPA_SessionStop,
  BAL_PSW_OTA_SPASMUnlock,
  BAL_PSW_OTAPA_VerifyOK,
  BAL_PSW_OTA_AkeyEx,
  BAL_PSW_OTA_SPLUnlock,
  BAL_PSW_OTA_SSDUpd,
  BAL_PSW_OTA_NAMDownload,
  BAL_PSW_OTA_MDNDownload,
  BAL_PSW_OTA_IMSIDownload,
  BAL_PSW_OTA_PRLDownload,    
  BAL_PSW_OTA_DataMaskCommitToNVRAM,
  BAL_PSW_OTA_CommitOK,
  BAL_PSW_OTA_ExcessSPCFailure,
  BAL_PSW_OTA_AnalogCDMADownload,
  BAL_PSW_OTA_PRLDownloading,  
  BAL_PSW_OTA_PRLDownloadFail,
  BAL_PSW_OTA_ProgrammingFail,
  BAL_PSW_OTA_Premature_Terminate,
  BAL_PSW_OTA_VendorLock_Reset 
} BalPswOtaStatusIndT;

typedef PACKED struct
{
  uint16 ServiceOption;
  bool   ToNegotiate;
  uint8  forRc;
  uint8  revRc;
} BalPswUpdateServiceConfigMsgT;

typedef PACKED struct
{
  BalPswOtaStatusIndT status;
} BalPswOtaspIndMsgT;

typedef PACKED struct
{
  BalPswPsStackStatusT PsStackStatus;
} BalPswPsStackStatusMsgT;

/***************************************************************************

  EVENT NAME: BAL_PSW_EVT_FLASH_STATUS_MSG
  DESCRIPTION:
    This message is used by PSW to indicate whether flash succeeded or not.
  MESSAGE TYPE: 
    PswFlashStatuMsgT - Indicates whether flash succeeded or not.
          
****************************************************************************/
typedef PACKED struct
{
  bool Succeeded;
} BalPswFlashStatusMsgT;


/***************************************************************************

  EVENT NAME: BAL_PSW_EVT_CALL_DIALING_MSG 
  DESCRIPTION:
    Sent when the call is dialing 
    (For example, when AIW sends a ATD13505372343 command to originate a call, UI also should display 
     "dialing 13505372343". We uses BAL_PSW_EVT_CALL_DIALING_MSG to inform UI this status.)
  MESSAGE TYPE: 
    BalPswCallDialingMsgT
      
****************************************************************************/
typedef PACKED struct
{
  PswRequestModeT   RequestMode;     /* requested call type*/
  PswServiceOptionT ServiceOption;   /* requested service option  */
  bool              PrivacyMode;     /* voice privacy indicator */ 
  bool              Encoded;
  bool              DigitMode;       /* digit encoding indicator  */
  uint8             NumDigits;       /* number of digits to be dialled   */ 
  uint8             Digits[BAL_PSW_MAX_CALLING_PARTY_NUMBER_SIZE + 1]; 
                                     /* of  ascii digits, null terminated string */
  PswServiceT       ServiceType;
} BalPswCallDialingMsgT;        
    
 /***************************************************************************

  EVENT NAME: BAL_PSW_EVT_SERVICE_MSG
  DESCRIPTION:
    This message is used by PSW to indicate whether we have service or not.
  MESSAGE TYPE: 
    PswServiceMsgT - Indicates the type of service we have acquired.
          
****************************************************************************/

typedef enum
{
  BAL_PSW_IN_SERVICE,
  BAL_PSW_NO_SERVICE,
  BAL_PSW_OOSA,
  BAL_PSW_CP_DISABLED
} BalPswServiceStatusT;

/*----------------------------------------------------------------------------------------
|Question:                                                                             |  
| what's the difference between PSW_NO_SERVICE and PSW_OOSA?                           |
---------------------------------------------------------------------------------------| */

typedef enum
{
  BAL_PSW_CDMA_SERVICE_TYPE,
  BAL_PSW_AMPS_SERVICE_TYPE
} BalPswServiceT;

typedef enum
{
   BAL_PSW_IDLE,
   BAL_PSW_CSD,
   BAL_PSW_ONLINE_RELAY_PKT_ACTIVE,
   BAL_PSW_ONLINE_RELAY_PKT_DORMANCY,
   BAL_PSW_ONLINE_BROWSER_ACTIVE, 
   BAL_PSW_ONLINE_BROWSER_DORMANCY
}BalPswDataStatusT;
typedef PACKED struct
{
  BalPswServiceStatusT ServiceStatus;
  BalPswServiceT       ServiceType;
  uint8                Roam;          /* ROAM Status                          */
  uint16               Band;          /* Current Operating Band               */
  uint16               Channel;       /* Current Channel Number               */
  uint8                Mode;          /* current mode: PCS/CellularAnalog     */
  uint8                Block;         /* current CDMA block (if CDMA system)  */
  uint8                ServingSystem; /* Serving System/Block                 */
  uint16               SysID;         /* Last-Received System ID  (sid)       */
  uint16               LocArea;       /* Current Location Area ID (nid)       */
  uint16               PilotPn;       /* PILOT_PN  */ 
  uint8                pRevInUse;                 /* CDMA Protocol Revision that MS uses  */
} BalPswServiceMsgT;

typedef struct
{
  PswServiceT CallType;
  uint16      ServiceOption;
} BalPswEvtCallConnectedMsgT;

typedef struct _BALRECORD_INDEX
{
  UINT8* pByteOffset;
  UINT16 bitOffset;
  UINT16 recNum;
  UINT16 recSize; 
} BALRECORD_INDEX;

typedef struct
{
  bool            valid;
  UINT16          PR_LIST_SIZEs;
  UINT16          PR_LIST_IDs;
  bool            PREF_ONLYs;
  UINT8           DEF_ROAM_INDs;
  UINT16          NUM_ACQ_RECSs;
  UINT16          NUM_SYS_RECSs;
  BALRECORD_INDEX acquireTableStart;
  BALRECORD_INDEX systemTableStart;
  UINT16          PR_LIST_CRCs;
  UINT16          NUM_GEO_AREASs;
} BALPRL_HEADER;

typedef PACKED struct
{
  uint32            RegId;
  PswRequestModeT   RequestMode;
  PswServiceOptionT ServiceOpt;
  bool              PrivacyMode;
  uint8             NumDigits;
  uint8             DigitsP[32];
  PswServiceT       ServiceType;
  bool              E911;
  bool              Otasp;
  BalPswOtaspCodeT  OtaspCode;
} BalInitiateCallMsgT;

/* ETS Message Types
 */
typedef PACKED struct
{
  RegIdT RegId;
  uint64 Akey;
  uint32 Checksum;
} BalChangeAkeyMsgT;

typedef PACKED struct
{
  NamNumber         NamId;
  PswNamChangedMsgT Nam;
} BalWriteNamMsgT;

typedef PACKED struct
{
  bool Nam1PrlEnable;
  bool Nam2PrlEnable;
} BalSetPrlEnableMsgT;

typedef PACKED struct
{
  NamNumber NamId;
} BalSetActiveNamMsgT;

typedef PACKED struct
{
  uint64 Akey;
  uint32 Esn;
} BalGetAkeyChecksumMsgT;

typedef PACKED struct
{
  uint8 numRetries;
  uint8 timeBetweenAttempts;
  uint8 timeBeforeAbort;
} BalAutoRetryEnableMsgT;

/*===========================================================================
                           FUNCTION API
===========================================================================*/

/*===========================================================================

FUNCTION BalPswInit

DESCRIPTION
  Initialize the Bal PSW module.

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void BalPswInit( void );

/*===========================================================================

FUNCTION BalPswRegister

DESCRIPTION
  register ui to the BAL PSW module

DEPENDENCIES
  None

RETURN VALUE
  register id

===========================================================================*/
RegIdT BalPswRegister( BalEventFunc CallBack );

/*===========================================================================

FUNCTION BalPswUnregister

DESCRIPTION
  Deregister ui to the BAL PSW module

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void BalPswUnregister( RegIdT RegId );

/***************************************************************************
  FUNCTION NAME: BalPswProtocolStackEnable
  DESCRIPTION:
    Enables or disables the protocol stack.    
 
  PARAMETERS:
    Enable - Whether to enable or disable the protocol stack.
  RETURNS:
    None.
***************************************************************************/
void BalPswProtocolStackEnable( bool Enable );

/*****************************************************************************

   FUNCTION NAME: BalPswChangeServiceOption
   DESCRIPTION:   Sends a message to PSW to change the prefered service option
   PARAMETERS:    ServiceOption,
                  ToNegotiate - TRUE enables negotiation
                  forRc,
                  revRc
   RETURNS:       None

*****************************************************************************/
void BalPswChangeServiceOption( uint16 ServiceOption,
                                bool   ToNegotiate,
                                uint8  forRc,
                                uint8  revRc );

/***************************************************************************
  FUNCTION NAME: BalPswInitiateCall
  DESCRIPTION:
    Originates a phone call.
  PARAMETERS:
    RegId         - the client ID
    RequestMode   - requested call type                       
    PrivacyMode   - voice privacy indicator                   
    NumDigits     - number of digits to be dialled            
    DigitsP       - CP_MAX_CALLING_PARTY_NUMBER_SIZE + 1 array of ascii digits,
                     null terminated string  
  RETURNS:                                                                   
    True if the message for originating call is sent to PSW                                                                 
***************************************************************************/                                                                            
bool BalPswInitiateCall( RegIdT            RegId,         /* the client ID */
                         PswRequestModeT   RequestMode,   /* requested call type*/
                         PswServiceOptionT ServiceOption, /* requested service option  */
                         bool              PrivacyMode,   /* voice privacy indicator */                         
                         uint8             NumDigits,     /* number of digits to be dialled  */
                         uint8*            DigitsP,       /* CP_MAX_CALLING_PARTY_NUMBER_SIZE + 1 array */
                                                          /* of  ascii digits, null terminated string */
                         PswServiceT       ServiceType,  
                         bool              E911,
                         bool              Otasp,
                         BalPswOtaspCodeT  otasp_code );

/***************************************************************************
  FUNCTION NAME: BalPswAnswerCall
  DESCRIPTION:
    Answer the incoming call.
    
  PARAMETERS:
    RegId - the client ID.

  RETURNS:
    True if the message for answering call is sent to PSW 
***************************************************************************/
bool BalPswAnswerCall( RegIdT RegId );

/***************************************************************************
  FUNCTION NAME: BalPswHangupCall
  DESCRIPTION:
    End the current call.
  PARAMETERS:
    RegId - the client ID.
  RETURNS:
    True if the Nam could be written.
***************************************************************************/
bool BalPswHangupCall( RegIdT RegId );

/***************************************************************************
  RETURNS:
    PswDataStatusT=0 ---NULL;
                             1 ---ACTIVE;
                             2 ---DORMANCY.
    
***************************************************************************/
BalPswDataStatusT BalPswGetDataStatus(RegIdT RegId);
/***************************************************************************
FUNCTION NAME: BalPswHangupDataCall
  DESCRIPTION:
    To hang up data call when necessary.
  PARAMETERS:
    
  RETURNS:
    
***************************************************************************/
void BalPswHangupDataCall(RegIdT RegId);
/***************************************************************************
  FUNCTION NAME: BalPswDataCallPreArrange
  DESCRIPTION:
    To pre arrange data/fax mode.
  PARAMETERS:
    RegIdT RegId, DsPreArangT paType.
  RETURNS:
    None
    
***************************************************************************/
void BalPswDataCallPreArrange(RegIdT RegId,DsPreArangT paType);

/***************************************************************************
  FUNCTION NAME: BalPswVsSwitchSo()
  DESCRIPTION:
     Switch voice service to the appointed service option.
  PARAMETERS:
    New service option.
  RETURNS:
    None.
    
***************************************************************************/
void BalPswVsSwitchSo(uint8 so);
/***************************************************************************
  FUNCTION NAME: BalPswHookFlash
  DESCRIPTION:
    Sends a hookflash message with parameters to the base station 
    (CDMA 2000 Layer 3 2.7.4).   
    BAL_PSW_FLASH_STATUS_MSG returns the status.
  PARAMETERS:
    RegId - the client ID
    KeypadFacilityP - the information to be sent in the message "flash with information"
  RETURNS
    True if the message for ending call is sent to PSW
***************************************************************************/
bool BalPswHookFlash( RegIdT RegId,             /* the client ID */
                      uint8* KeypadFacilityP ); /* Incl NULL terminator */
                                       

/***************************************************************************
  FUNCTION NAME: BalPswSendBurstDTMF
  DESCRIPTION:
    Send burst DTMF tone to the base station.
  PARAMETERS:
    RegId - the client ID.
    NumDigits - Number of digits in the Digits array.
    DtmfOnLength - Length each tone/digit should be on for.
    DtmfOffLength - Length the tone generator should be off for between tones
    Digits - Which digits should be sent.
  RETURNS:
    True if the message for playing burst DTMF is sent to PSW
***************************************************************************/
bool BalPswSendBurstDTMF( RegIdT RegId,
                          uint8  NumDigits,
                          uint8  DtmfOnLength,
                          uint8  DtmfOffLength,
                          uint8* DigitsP );

/***************************************************************************
  FUNCTION NAME: BalPswSendContDTMFOn
  DESCRIPTION:
    Send a continuous DTMF tone to the base station.
  PARAMETERS:
    RegId - the client ID.
    Digit - Which digit should be sent.
  RETURNS:
    True if the message for playing continual DTMF is sent to PSW
***************************************************************************/
bool BalPswSendContDTMFOn( RegIdT RegId,  
                           uint8  Digit );
                       
/***************************************************************************
  FUNCTION NAME: BalPswSendContDTMFOff
  DESCRIPTION:
    Stop sending a continuous DTMF tone to the base station.
  PARAMETERS:
    RegId - the client ID.
  RETURNS:
    True if the Nam could be written.
***************************************************************************/                                              
bool BalPswSendContDTMFOff( RegIdT RegId );
                                                         
/***************************************************************************
  FUNCTION NAME: BalPswReadNam
  DESCRIPTION:
    Fills in the given NAM structure with the NAM information
----------------------------------------------------------------------------------------
|   The procedure is as follows:                                                       |
|   1. BAL sends message to get NAM and return.                                        |
|   2. After getting NAM PSW sends a message to BAL.                                   |
|   3. BAL forwords this message to UI                                                 |
---------------------------------------------------------------------------------------|    
  PARAMETERS:
    RegId - the client ID.
    NamNumber - Number of the NAM requested starting at NAM 0.
  RETURNS:
    True if the message for stopping burst DTMF is sent to PSW
***************************************************************************/                                                                                                            
bool BalPswReadNam( RegIdT    RegId,
                    NamNumber NamId );

/***************************************************************************
  FUNCTION NAME: BalPswWriteNam
  DESCRIPTION:
    Writes the given NAM structure.
----------------------------------------------------------------------------------------
|Attention:                                                                            |  
|   The procedure is as follows:                                                       |
|   1. BAL sends message to update Akey and return.                                      |
|   2. After updating Akey PSW sends a message to BAL.                                  |
|   3. BAL forwards this message to UI                                                 |
---------------------------------------------------------------------------------------|     
  PARAMETERS:
    RegId - the client ID.
    NamP - a pointer to the NAM structure that needs to be filled in.

  RETURNS:
    True if the Nam  is read.
***************************************************************************/
bool BalPswWriteNam( RegIdT             RegId,
                     PswNamChangedMsgT* NamP );

void BalPswGetActiveNam( void );

void BalPswSetActiveNam( NamNumber NamId );

/***************************************************************************
FUNCTION NAME:
  BalPswGetAKeyChecksum

DESCRIPTION:
  Send a message to the protocol stack to get the Akey Checksum. 
  This is necessary to provide the UI with a mechanism to check the checksum
   input by the user versus the calculated checksum produced by this interface.

PARAMETERS:
  Akey - 20 digit Authentication KEY
  Esn  - Electronic Serial Number

RETURN VALUE:
  <NONE>
***************************************************************************/
void BalPswGetAKeyChecksum( uint64 AKey,
                            uint32 Esn );

/***************************************************************************
  FUNCTION NAME: BalPswChangeAKey
  DESCRIPTION:
    Change the Akey and Akey checksum
----------------------------------------------------------------------------------------
|Attention:                                                                            |  
|   The procedure is as follows:                                                       |
|   1. BAL sends message to write NAM and return.                                      |
|   2. After writting NAM PSW sends a message to BAL.                                  |
|   3. BAL forwords this message to UI                                                 |
---------------------------------------------------------------------------------------|    
  PARAMETERS:
    RegId - the client ID.
    Akey - The new Akey.
    Checksum - The new Checksum that corresponds to the Akey and ESN combo.
  RETURNS:
    True if the Nam is written.
***************************************************************************/
bool BalPswChangeAKey( RegIdT RegId,  
                       uint64 Akey,     
                       uint32 Checksum );

/***************************************************************************
FUNCTION NAME:
  BalPswSetPrlEnable
  
DESCRIPTION:
  Send a message to the protocol stack to enable/disable Preferred Roaming List(s).

PARAMETERS:
  Nam1PrlEnable - TRUE to enable PRL for NAM 1
  Nam2PrlEnable - TRUE to enable PRL for NAM 2

RETURN VALUE:
  <NONE>
***************************************************************************/                  
void BalPswSetPrlEnable( bool Nam1PrlEnable,
                         bool Nam2PrlEnable );
                  
/*===========================================================================

FUNCTION BalPswDeliverMail

DESCRIPTION
  Callback to support CP messages from PSW.

DEPENDENCIES
  None

RETURN VALUE
  boolean indicating whether the message was handled.

===========================================================================*/
bool BalPswDeliverMail( uint32 msg_id,
                        void*  msg_buffer,
                        uint32 msg_size );

/*===========================================================================

FUNCTION BalPswE911Disable

DESCRIPTION:
  Send a message to the protocol stack to disable Emergency 911 Mode.

PARAMETERS:
  <NONE>

RETURN VALUE:
  <NONE>
===========================================================================*/
void BalPswE911Disable( void );



/*===========================================================================

FUNCTION BalPswAutoRetryEnable

DESCRIPTION:
  Send a message to the protocol stack to Enable Auto Retry and set the retry
  attempt parameters.

PARAMETERS:
  numRetries          - number of times to attempt to retry origination before 
                         aborting
  timeBetweenAttempts - time (in seconds) between retries
  timeBeforeAbort     - total time (in seconds) between original origination 
                         request and abort.

  NOTE: Abortion is determined by whichever comes first - number of retries 
        reaches numRetries OR time from original request reaches timeBeforeAbort.

RETURN VALUE:
  <NONE>
===========================================================================*/
void BalPswAutoRetryEnable( uint8 numAttempts,
                            uint8 timeBetweenAttempts,
                            uint8 timeBeforeAbort );

/*===========================================================================
FUNCTION
  BalPswAutoRetryDisable

DESCRIPTION:
  Send a message to the protocol stack to disable Auto Retry

PARAMETERS:
  <NONE>

RETURN VALUE:
  <NONE>
===========================================================================*/
void BalPswAutoRetryDisable( void );

/*===========================================================================
FUNCTION
  BalPswAutoRetryDisable

DESCRIPTION:
Check the Silent Retry status.

PARAMETERS:
  <NONE>

RETURN VALUE:
  Boolean indicating whether Silent Retry is enabled.
===========================================================================*/
bool BalPswIsSilentRetryEnabled( void );
/*===========================================================================
  FUNCTION 
    BalPswSetPrivacyMode

  DESCRIPTION:
    Enable/Disable Call Privacy Mode.

  PARAMETERS:
    Mode -  call privacy mode enable(TRUE)/disable(FALSE)

  RETURNS:                                                                   
  <NONE>                                                               
===========================================================================*/
void BalPswSetPrivacyMode( bool Mode );

/*===========================================================================
FUNCTION
  BalPswServiceOption

DESCRIPTION:
  Get the current service option

PARAMETERS:
  <NONE>

RETURN VALUE:
  the current service option
===========================================================================*/
PswServiceOptionT BalPswServiceOption( void );

/*===========================================================================
FUNCTION
  BalPswServiceType

DESCRIPTION:
  Send a message to the protocol stack to disable Auto Retry

PARAMETERS:
  <NONE>

RETURN VALUE:
  the current Service Type
===========================================================================*/
PswServiceT BalPswServiceType( void );

/*===========================================================================

FUNCTION BalPswLocationSvcCfg

DESCRIPTION
  Set location privacy flag in loc_svc

DEPENDENCIES
  Mode - TRUE if privacy is on

RETURN VALUE
  void

===========================================================================*/
extern void BalPswLocationSvcCfg( bool Mode );

/***************************************************************************
  FUNCTION NAME: BalPswGetServiceStatus
  DESCRIPTION:
    Get PSW service status.
  PARAMETERS:
    None.
  RETURNS:
    BAL_PSW_CP_DISABLED
    BAL_PSW_NO_SERVICE
    BAL_PSW_IN_SERVICE
***************************************************************************/
BalPswServiceStatusT BalPswGetServiceStatus(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */




#endif
