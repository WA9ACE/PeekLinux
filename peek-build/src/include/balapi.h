

#ifndef BALAPI_H
#define BALAPI_H

#include "hwdapi.h"
#include "hwdaudioapi.h"
#include "hwdrfapi.h"
#include "ipcapi.h"
#include "pswcustom.h"
#include "sysdefs.h"
#include "sysapi.h"
#include "pswapi.h"
#include "pswsmstune.h"
#include "hlwapi.h"
#include "balfsiapi.h"

/*------------------------------------------------------------------------
 * Define constants used in BAL API
 *------------------------------------------------------------------------*/

/* macros */
#define BAL_MAKE_RECID(Index,DevType)           ((Index) | (DevType))
#define BAL_GET_INDEX_FROM_RECID(PhyAddress)    ((PhyAddress) & (0x0FFF))
#define BAL_GET_DEVTYPE_FROM_RECID(PhyAddress)  ((PhyAddress) & (0xF000))

/* BAL signals */
//xp #define BAL_STARTUP_1_SIGNAL            EXE_SIGNAL_1   /* from HWD MS, after dspv alive is rcvd */
//xp #define BAL_STARTUP_2_SIGNAL            EXE_SIGNAL_2   /* from L1D after 32kHz Osc is validated */
//xp #define BAL_VREC_TIMEOUT_SIGNAL         EXE_SIGNAL_3   /* timeout in training or recogn */

/* other signals are defined in valdefs.h, and start from _11 up */

/* BAL command mailbox id */
#define BAL_MAILBOX                     EXE_MAILBOX_1_ID
#define BAL_MAILBOX_WAIT_ID             EXE_MAILBOX_1

#define BAL_HAL_MAILBOX              EXE_MAILBOX_2_ID
#define BAL_HAL_MAILBOX_WAIT_ID      EXE_MAILBOX_2

#define BAL_FLIGHT_MODE 1
//xp #define BAL_STORAGE_MAILBOX          EXE_MAILBOX_3_ID
//xp #define BAL_STORAGE_MAILBOX_WAIT_ID  EXE_MAILBOX_3


/*------------------------------------------------------------------------
 * BAL message data structures (grouped like and in the same order of messages)
 *------------------------------------------------------------------------*/
/*------------------------------------------------------------------------
 * BAL message ids, grouped by originator
 *
 * When adding/removing/modifying BAL messages:
 * - add (or remove) associated data structure; put new structures in the
 *   same section as the message id, and in the same relative order within
 *    its section
 *  - update the BAL section in msg_id.txt for ETS if the message should be
 *    usable from ETS (BAL-subsections on ETS are the same as the ones in
 *    BALapi.h). Try to keep the msg name and ETS command as similar as
 *    possible.
 *  - always update the enum BAL MsgIds in bal\bal_msg.txt on ETS; the enum is used
 *    to display incoming BAL messages, and should always be kept in synch
 *    with BalMsgIdT.
 *  - update bal_msg.txt with the data structure associated with the new
 *    message (optiona; do it only if ETS is supposed to send this message)
 *
 *------------------------------------------------------------------------*/

typedef enum
{
   /*-----------------------------------------------------------------
   * AIWHandler
   *----------------------------------------------------------------*/
   BAL_FLUSH_FINAL_MSG = 0,
   BAL_NWK_RPT_DATA_SVC_STATUS_WORD_MSG,
      
   /*-----------------------------------------------------------------
   * HLWHandler
   *----------------------------------------------------------------*/
   BAL_TCPB_DATA_RECV_MSG = 100,
   BAL_TCPB_DATA_SEND_RSP_MSG,
   
   BAL_UPB_RECV_DATA_MSG,
   BAL_UPB_SENT_DATA_RSP_MSG,
   
   BAL_HLW_PPP_CONN_STATUS_MSG,
   BAL_HLW_PPP_CLOSE_STATUS_MSG,
   BAL_HLW_SOCKET_CREATE_STATUS_MSG,
   BAL_HLW_SOCKET_BIND_STATUS_MSG,
   BAL_HLW_SOCKET_CONN_STATUS_MSG,
   BAL_HLW_SOCKET_CLOSE_STATUS_MSG,
   BAL_HLW_SOCKET_INACT_TMO_MSG,

   BAL_TCPB_SEND_DATA_MSG,
   BAL_UDPB_SEND_DATA_MSG,
   BAL_UPB_CONN_STATUS_MSG,
   BAL_TCPB_CONN_STATUS_MSG,
   BAL_UPB_IDLE_TMO_MSG,
   BAL_HLW_SOCKET_LISTEN_STATUS_MSG,
   BAL_HLW_SOCKET_OPT_STATUS_MSG,
   BAL_HLW_SOCKET_SHUT_STATUS_MSG,
   BAL_HLW_SOCKET_LINGER_STATUS_MSG,
   BAL_DNS_NET_MSG,
   BAL_DNS_QUERY_MSG,
   /*-----------------------------------------------------------------
   * OTASPHandler
   *----------------------------------------------------------------*/
   BAL_IOTA_683_RSP_MSG = 200,
   BAL_IOTA_PRL_RSP_MSG,
   BAL_OTASP_CONN_IND_MSG,
   BAL_OTASP_DISCONN_IND_MSG,
   BAL_OTASP_IND_MSG,      /* ??? nobody is sending this message */
   BAL_OTASP_PREMATURE_TERM_IND_MSG,
   BAL_OTASP_RX_IND_MSG,
   BAL_OTASP_START_IND_MSG,
   BAL_OTASP_STOP_IND_MSG,
   BAL_OTASP_SYSTEM_ERROR_IND_MSG,
   BAL_OTASP_TX_RSP_MSG,
   
   /*-----------------------------------------------------------------
   * LocHandler
   *----------------------------------------------------------------*/
   BAL_LOC_RESP_MSG = 300,
   BAL_LOC_ERROR_MSG,
   BAL_LOC_PILOT_PHASE_MEAS_MSG,
   BAL_LOC_PSEUDO_RANGE_MSG,
   BAL_LOC_GET_SECTOR_INFO_RSP_MSG,
   
   /*-----------------------------------------------------------------
   * PSWAmpsHandler
   *----------------------------------------------------------------*/
   BAL_DTMF_TONE_GEN_MSG = 400,
   BAL_AMPS_EXT_PROTO_MSG,
   
   /*-----------------------------------------------------------------
   * PSWHandler
   *----------------------------------------------------------------*/
   BAL_ACCESS_MAX_CAPSULE_SIZE_MSG = 500,
   BAL_BS_CO_ORD_MSG,
   BAL_NAM_MSG,
   BAL_NAM_RPT_ACTIVE_NAM_MSG,
   BAL_ACTIVE_NAM_RSP_MSG,
   BAL_NAM_RPT_LOCK_STATUS_MSG,
   BAL_LOCK_NAM_RSP_MSG,
   BAL_CHANGE_AKEY_MSG,
   BAL_GET_AKEY_CHECKSUM_RSP_MSG,
   BAL_AKEY_CHANGE_RSP_MSG,
   BAL_NWK_RETRIEVE_CHANNEL_MSG,
   
   BAL_NWK_RPT_ALERT_MSG,
   BAL_NWK_RPT_ALERT_MORE_INFO_REC_MSG,
   BAL_NWK_RPT_BURST_DTMF_MSG,
   BAL_NWK_RPT_CP_EVENT_MSG,
   BAL_NWK_RPT_CP_RESPONSE_MSG,
   BAL_NWK_RPT_CP_STATUS_MSG,
   BAL_NWK_RPT_DATA_BURST_MSG,
   BAL_NWK_RPT_FEAT_NOTIF_MORE_INFO_REC_MSG,
   BAL_NWK_RPT_FEATURE_NOTIFICATION_MSG,
   BAL_NWK_RPT_FLASH_MORE_INFO_REC_MSG,
   BAL_NWK_RPT_FLASH_MSG,
   BAL_NWK_RPT_HANDOFF_MSG,
   BAL_NWK_RPT_LOCK_MSG,
   BAL_NWK_RPT_MAINTENANCE_REQUIRED_MSG,
   BAL_NWK_RPT_ORDER_MSG,
   BAL_NWK_RPT_REGISTRATION_ACCEPT_MSG,
   BAL_NWK_RPT_REGISTRATION_REJECT_MSG,
   BAL_NWK_RPT_SERVICE_CONFIG_MSG,
   BAL_NWK_RPT_SERVICE_READY_MSG,
   BAL_NWK_RPT_SERVICE_NEGOTIATION_MODE_MSG,
   BAL_NWK_RPT_START_CONTINUOUS_DTMF_MSG,
   BAL_NWK_RPT_STOP_CONTINUOUS_DTMF_MSG,
   BAL_NWK_RPT_SYSTEM_TIME_MSG,
   BAL_NWK_RPT_UNLOCK_MSG,
   BAL_NWK_RPT_PKT_STATE_MSG,
   BAL_NWK_RPT_PKT_DORM_TMR_MSG,
   
   BAL_PRL_INFO_MSG, 
   BAL_PSW_E911_MODE_MSG,
   BAL_ROAM_INDICATION_MSG,
   BAL_RPT_CQA_MSG,
   BAL_SR_FINISHED_MSG,
   BAL_SET_DEFAULT_SO_MSG,
   BAL_SET_PRIVACY_MODE_MSG,
   BAL_RPT_SILENT_RETRY_STATUS_MSG,

#ifdef KDDI_EXTENSIONS
   BAL_PSW_ANTENNA_BARS_RPT_MSG,
   BAL_EXTENDED_DATA_BURST_RX_IND_MSG,
#endif
   BAL_Locking_Feature_CHECK_ERR_MSG,
   
   /*-----------------------------------------------------------------
   * SMSHandler
   *----------------------------------------------------------------*/
   BAL_SMS_BCAST_ERROR_MSG = 600,
   BAL_SMS_BCAST_IND_PARMS_MSG,
   BAL_SMS_CAUSE_CODE_STATUS_MSG,
   BAL_SMS_DELIVER_IND_DATA_MSG,
   BAL_SMS_DELIVER_IND_PARMS_MSG,
   BAL_SMS_ERROR_MSG,
   BAL_SMS_RX_IND_MSG,
   BAL_SMS_TX_RSP_MSG,
   BAL_SMS_CAUSE_CODE_MSG,   
   BAL_SMS_ACK_L2_ACKED_MSG,
   
   BAL_SMS_TST_CONNECT_MSG,
   BAL_SMS_TST_BCAST_CONNECT_MSG,
   BAL_SMS_TST_BCAST_DISCONNECT_MSG,
   BAL_SMS_TST_BCAST_PREF_MSG,
   BAL_SMS_TST_CANCEL_MSG,
   BAL_SMS_TST_CAUSE_CODE_STATUS_MSG,
   BAL_SMS_TST_DISCONNECT_MSG,
   BAL_SMS_TST_PREF_SRV_OPT_MSG,
   BAL_SMS_TST_SUBMIT_MSG,
   BAL_SMS_TST_TERM_STATUS_MSG,
   BAL_SMS_TST_USER_ACK_MSG,
   BAL_SMS_TST_RETX_AMOUNT_MSG,
   BAL_SMS_TST_CAUSE_CODE_MSG,   
   
   /*-----------------------------------------------------------------
   * HWD Handler
   *----------------------------------------------------------------*/
   BAL_KEYPAD_MSG = 700,
   BAL_PWR_KEY_TIMEOUT,
   BAL_AUDIO_EDAI_MIC_CTRL_DATA_MSG,
   BAL_HWD_BATTERY_READ_DATA_MSG,
   BAL_HWD_TEMPERATURE_READ_DATA_MSG,
   BAL_HWD_RX_TX_POWER_INFO_DATA_MSG,
   BAL_PSW_PILOT_PWR_RPT_MSG,
   BAL_DBM_DATA_NAM_MSG,
   BAL_ETS_KEYPAD_MSG,
   
   /*-----------------------------------------------------------------
   * VMEMO Handler
   *----------------------------------------------------------------*/
   BAL_VMEMO_REC_START_TST_MSG = 800,
   BAL_VMEMO_PLAY_START_TST_MSG,         
   BAL_VMEMO_PLAY_STOP_TST_MSG,         
   BAL_VMEMO_PLAY_PAUSE_TST_MSG,
   BAL_VMEMO_PLAY_RESUME_TST_MSG,

   /* VMEMO messages used by LMD and ETS */
   BAL_VMEMO_REC_STOP_MSG,  

   /* VMEMO messages used by LMD */
   BAL_VMEMO_DATA_MSG,             
   BAL_SPEECH_PLAY_COMPLETE_MSG,
   BAL_ANSWER_PLAY_COMPLETE_MSG,
   BAL_VMEMO_SSO_NOTIFY_MSG,       /* Notify BAL of actual SSO connected */
   
   BAL_VMEMO_REC_INIT_ACK_MSG, /* dbm ack to erase command before start of recording */
   BAL_VMEMO_WRITE_ACK_MSG,    /* dbm ack to data write */
   BAL_VMEMO_PLAY_INIT_ACK_MSG, /* dbm ack to header read before playback */
   
   BAL_VMEMO_RESET_BUFFERS_MSG,   /* erase voice memo */
   
   BAL_VMEMO_ABORT_MSG, /* Stop any voice memo in progress */
   
      
   /*-----------------------------------------------------------------
   * VREC Handler
   *----------------------------------------------------------------*/
   BAL_VREC_READ_INIT_ACK_MSG = 900,
   BAL_VREC_ERASE_ACK_MSG,
   BAL_VREC_COMP_DATA_WRITE_ACK_MSG,
   BAL_VREC_TRAINING_DATA_WRITE_ACK_MSG,
   BAL_VREC_HEADER_DATA_WRITE_ACK_MSG,
   
   BAL_VREC_MIXED_DATA_MSG,
   BAL_VREC_PCM_DATA_MSG,
   
   BAL_VREC_PLAYBACK_COMPLETE_MSG,
   BAL_VREC_PLAYBACK_CANCEL_MSG,
   BAL_VREC_TRAINING_START_MSG,
   BAL_VREC_TRAINING_ACCEPT_MSG,
   BAL_VREC_TRAINING_CANCEL_MSG,
   BAL_VREC_RECOGNITION_START_MSG,
   BAL_VREC_RECOGNITION_CANCEL_MSG,
   BAL_VREC_RESET_BUFFERS_MSG,
   
   BAL_VREC_CAPTURE_CANCEL_MSG,
   BAL_VREC_MATCHING_CANCEL_MSG,
   
   BAL_VREC_READ_VOCABULARY_MSG,
   BAL_VREC_READ_TRAINCB_MSG,
   BAL_VREC_TEST_FUNCTION_MSG,
   
   /*-----------------------------------------------------------------
   * USERINFO Handler
   *----------------------------------------------------------------*/
   BAL_KPAD_DISABLED_MSG = 1000,
   BAL_EMERGENCY_MODE_TEST_MSG,
   BAL_SET_AUTO_ANSWER_DELAY_MSG,
   BAL_GET_RSSI_ACK_MSG,
   BAL_L1D_RSSI_RPT_MSG,
   
   /*-----------------------------------------------------------------
    * Sound API Handler
    *----------------------------------------------------------------*/
   BAL_SND_SET_DEVICE_MSG = 1100,          /* Sound Set Device from ETS */
   BAL_SND_SET_VOLUME_MSG,                 /* Sound Set Volume from ETS */
   BAL_SND_SOUND_STOP_MSG,                 /* Sound Stop from ETS */
   BAL_SND_TONE_PLAY_MSG,                  /* Play comfort tone from ETS */
   BAL_SND_MUSIC_PLAY_MSG,                 /* MUSIC Play from ETS */
   BAL_SND_MUSIC_FAST_FWD_MSG,             /* MUSIC Fast Fwd from ETS */
   BAL_SND_MUSIC_REWIND_MSG,               /* MUSIC Rewind from ETS */
   BAL_SND_MUSIC_SUSPEND_MSG,              /* MUSIC Suspend from ETS */
   BAL_SND_MUSIC_RESUME_MSG,               /* MUSIC Resume from ETS */
   BAL_SND_VIBRATE_START_MSG,              /* Start Vibrate */
   BAL_SND_VIBRATE_STOP_MSG,               /* Stop Vibrate  */
   BAL_SND_TEST_MSG,                       /* Sound testing for CDS6 */
   BAL_SND_FLIP_OPEN_VOL_MSG,              /* Set to flip open volume from ETS */
   BAL_SND_MUSIC_STATUS_MSG,

   /*-----------------------------------------------------------------
    * Display API Handler
    *----------------------------------------------------------------*/
   BAL_DISP_IMAGE_SHOW_MSG = 1150,
   BAL_DISP_IMAGE_CONVERT_FORMAT_MSG,
   BAL_DISP_IMAGE_GET_PROPERTIES_MSG,
   BAL_DISP_DISP_GET_DEVICE_INFO_MSG,
   BAL_DISP_BITMAP_GET_INFO_MSG,
   BAL_DISP_BITMAP_GET_DATA_MSG,
   BAL_KEYPAD_TEST_MSG,
   BAL_BM_SUSPEND_INFO_GET,
   /*-----------------------------------------------------------------
   * DBM Handler
   *----------------------------------------------------------------*/
   BAL_FLUSH_ACK_MSG = 4900,
   BAL_FLUSH_RF_ACK_MSG,
   BAL_NV_MSG_ACK_MSG,
   BAL_L1DTST_GETPHONESTATUS,
   BAL_GET_ERI_VERSION_MSG = 4904,
    BAL_SET_MOBILE_ID_MSG,     
     

#ifdef SYS_OPTION_RUIM
   /* PHB Msg */
   BAL_UIM_GET_PHB_REC_PARAMS_MSG = 6000,
   BAL_UIM_GET_PHB_REC_MSG,
   BAL_UIM_UPDATE_PHB_REC_MSG,
   BAL_UIM_ERASE_PHB_REC_MSG,
   /* Sms Msg */
   BAL_UIM_GET_SMS_REC_PARAMS_MSG,
   BAL_UIM_GET_SMS_REC_MSG,
   BAL_UIM_UPDATE_SMS_REC_MSG,
   BAL_UIM_ERASE_SMS_REC_MSG,
   /* Chv Msg */
   BAL_CHV_GET_STATUS_MSG         = 6020, 
   BAL_CHV_VERIFY_MSG,
   BAL_CHV_CHANGE_MSG,
   BAL_CHV_ENABLE_MSG,
   BAL_CHV_DISABLE_MSG,
   BAL_CHV_UNBLOCK_MSG,
#ifdef FEATURE_UTK
   /*Utk Msg */
   BAL_UTK_TERMINAL_PROFILE_MSG   = 6040,
   BAL_UTK_MENU_SELECTION_MSG,
   BAL_UTK_SMS_PP_DOWNLOAD_MSG,
   BAL_UTK_TERMINAL_RESPONSE_MSG,
   BAL_UTK_REQUEST_UIMVER_MSG,
   BAL_UTK_REQUEST_IMG_MSG,
   BAL_UTK_REQUEST_IMG_DATA_MSG,
   BAL_UTK_PROAVTIVE_CMD_MSG,
   /* Utk test */
   BAL_UTK_TST_GET_INPUT_DONE_MSG,
   BAL_UTK_TST_SELECT_ITEM_DONE_MSG,
   BAL_UTK_TST_COMMON_DONE_MSG,
   BAL_UTK_TST_SELECT_MENU_MSG,
   BAL_UTK_TST_INIT_MSG,
   BAL_UTK_TST_SMS_DOWN_MSG,
   BAL_UTK_TST_EXIT_MSG,
#endif
   
   /* BAL UIM MDN messages  */
   BAL_UIM_GET_MDN_REC_NUM_MSG    = 6060,
   BAL_UIM_READ_MDN_REC_MSG,
   BAL_UIM_UPDATE_MDN_REC_MSG,
   BAL_UIM_DEL_MDN_REC_MSG,
   BAL_UIM_NOTIFY_REGISTER_MSG,
   BAL_UIM_GET_PREF_LANG,
   BAL_UIM_GET_UIM_CARD_ID,
#endif
   /* BAL sms memory status Msg*/
   BAL_SMS_MEMORY_STATUS_MSG     = 6080,

   /*BAL extended AT Commands process msg*/
   BAL_AIW_AT_CMD_REQ_MSG          = 6100,	/* Extended AT command request code*/
   BAL_AIW_AT_CMD_BUFFER_ACK_MSG,    /* To send rsp buf ack to Bal/Mmi */

   BAL_GET_DEBUG_INFO_MSG        = 6200,
#ifdef BYD_USE_SIM
   BAL_SMS_COYOTE_SMS_MC_MEMORY_STATUS_MSG,
   BAL_SMS_COYOTE_SMS_MC_INCOMING_MSG,
   BAL_SMS_COYOTE_SMS_MC_BROAD_CAST_MSG,
#endif					
   
   /* BAL Test Mode Msgs */
   BAL_DSPM_VER_RSP_MSG          = 6300,
   BAL_DSPV_VER_RSP_MSG,
   BAL_CP_VER_RSP_MSG,

   BAL_START_REFURBISH_TEST,

   /* BAL Storage Msg */
   BAL_STORAGE_DATA_INIT_MSG			= 6400,

   BAL_RSSI_MSG                              = 6500, // zouda added for process RSSI
   BAL_VIBRATER_TIMEOUT_MSG	= 6600, // wangguo add

   //Add Flight Mode by zhihui 08-10-27 	 Begin
#ifdef BAL_FLIGHT_MODE
   BAL_FLIGHT_MODE_MSG = 6700,
   BAL_FLIGHT_MODE_SET_MSG = 6701,
   BAL_FLIGHT_MODE_REPORT_MSG = 6702,
#endif
  //Add Flight Mode by zhihui 08-10-27	  End

//add by zhihui for select plmn
#ifdef BAL_SELECT_PLMN
    BAL_PLMN_LIST_MSG = 6800,				//Plmn list report to app
    BAL_PLMN_SELECT_MSG = 6801,			//Plmn select result report to app
#endif

    BAL_NUM_MESSAGES
} BalMsgIdT;

 typedef enum
 {
     BAL_PSW_CKECK_Locking_Feature_OK = 0x00,
     BAL_PSW_CKECK_MIN_ERROR,
     BAL_PSW_CKECK_DefaultMIN_OK,
     BAL_PSW_CKECK_SID_ERROR,
     BAL_PSW_CKECK_MCC_ERROR,
     BAL_PSW_CKECK_FOR_RELIANCE_PrefNO_ERROR,
     BAL_PSW_CKECK_MIN_ERROR_OVER_MAX_RETRY_TIMES,
     BAL_PSW_CKECK_Locking_Feature_NUM
 }PswCheckForMinLockStateT;
 
 typedef PACKED struct
 {
 	uint8 nCheckRst;
 }PswCheckLockingFeatureRptT;
 

/* Generic ETS Message
 */
typedef PACKED struct
{
  ExeRspMsgT RspInfo;
  uint32     RegId;
} BalGenericMsgT;

/*-----------------------------------------------------------------
 * AIWHandler
 *----------------------------------------------------------------*/
 
/* BAL_NWK_RPT_DATA_SVC_STATUS_WORD_MSG */
typedef PACKED struct
{
  uint8 statusStr[15]; /* ??? buffer overflow in aiweng:mmiAiResult() */
} BalNwkRptDataSvcStatusWordMsgT;

/*-----------------------------------------------------------------
 * HLWHandler
 *----------------------------------------------------------------*/
 
/* BAL_TCPB_CONN_STATUS_MSG */
typedef enum
{
  BAL_TCPB_CONNECT_SUCCESS,         /* connection success       */
  BAL_TCPB_CONNECT_FAIL_TCP,        /* connection fails on TCP  */
  BAL_TCPB_CONNECT_FAIL_PPP,        /* connection fails on PPP  */
  BAL_TCPB_CONNECT_FAIL_RLP,        /* connection fails on RLP  */
  BAL_TCPB_CONNECT_FAIL_UART,       /* not use                  */
  BAL_TCPB_DISCONNECT_NORMAL,       /* disconnect from BS       */
  BAL_TCPB_DISCONNECT_MS,           /* disconnect from MS       */
  BAL_TCPB_DISCONNECT_FADE,         /* disconnect due to fading */
  BAL_TCPB_CONNECT_FAIL_TRAFFIC,    /* connection fails on traffic */
  BAL_TCPB_CONNECT_FAIL_NOSVC,      /* connection fails on no src  */
  BAL_TCPB_CONNECT_DORMANT,         /* connection dormant       */
  BAL_TCPB_CONNECT_RECONNECT,       /* reconnect after dormant  */
  BAL_TCPB_NUM_CONNECT_STATUS
} TcpbStatusT;

typedef enum
{
  BAL_IE_CLOSEPENDINGWAIT  =  6,        /* TCP close by peers, wait for user */
  BAL_IE_CLOSEPENDING      =  5,        /* TCP graceful close in progress  */
  BAL_IE_RECVMOREDATA      =  4,        /* more UDP or raw IP data         */
  BAL_IE_NODATA            =  3,        /* no data available for receive   */
  BAL_IE_CONNECTPENDING    =  2,        /* TCP connect attempt in progress */
  BAL_IE_LISTENPENDING     =  1,        /* listening for remote connect    */
  BAL_IE_SUCCESS           =  0,        /* request successful              */
  BAL_IE_INVALIDRQST       = -1,        /* invalid or unknown request      */
  BAL_IE_INVALIDSAP        = -2,        /* invalid service access point    */
  BAL_IE_INVALIDPORT       = -4,        /* invalid listen or connect port  */
  BAL_IE_INVALIDADDR       = -5,        /* invalid connect host address    */
  BAL_IE_NOMOREMBUFS       = -6,        /* no mbufs available              */
  BAL_IE_NOMORETCBS        = -7,        /* no tcbs available               */
  BAL_IE_NOLOCALADDR       = -8,        /* local host address not set      */
  BAL_IE_INVALIDSIZE       = -9,        /* invalid send or receive size    */
  BAL_IE_INVALIDDATA       = -10,       /* invalid request field           */
  BAL_IE_INVALIDOPT        = -11,       /* option incorrectly specified    */
  BAL_IE_INVALIDFLAGS      = -12,       /* invalid send/sento flags        */
  BAL_IE_INVALIDSTATE      = -13,       /* invalid TCP state               */
  BAL_TCP_TIME_OUT         = -14,       /* TCP connecting time out         */
  BAL_TCP_RESET            = -15,       /* TCP reset by remote             */
  BAL_TCP_ABORT            = -16,       /* TCP reset by local              */
  BAL_TCP_DISCNT_NORMAL    = -17        /* TCP Disconnect normal           */
} BalSocketStatusT;

typedef PACKED struct
{  
  TcpbStatusT  Status;   
} BalTcpbConnStatusMsgT;
 
/* BAL_TCPB_DATA_RECV_MSG */
typedef PACKED struct
{
  uint8   Sap;
  uint8*  DataP;
  uint16  Size;
} BalTcpbDataRecvMsgT;

/* BAL_TCPB_DATA_SEND_RSP_MSG */
typedef PACKED struct
{
  uint8            Sap;
  BalSocketStatusT status;
} BalTcpbDataSendRspMsgT;
 
/* BAL_UPB_CONN_STATUS_MSG */
typedef PACKED struct
{
  uint8  Status;   
} ValUpbConnStatusMsgT;
 
/* BAL_TCPB_SEND_DATA_MSG */
typedef PACKED struct
{
  uint8 Sap;          /* Socket ID */
  uint8 SetId;        /* Canned Data set ID. Data contents reside in:
                         SetId   Data resides in
                           0    : DataP pointer below. Reserved for LBS only.
                                  If DataP needed for other features use 
                                  new SetId.
                           1    : HTTP_GetRequest1
                           2    : HTTP_GetRequest2
                          else  : HTTP_NULL 
                       */
  uint16 Size;        /* Size of contents in DataP pointer */
  uint8* DataP;       /* Pointer to data */
} BalTcpbSendDataMsgT;
 
/* BAL_UDPB_SEND_DATA_MSG */
typedef PACKED struct
{
  uint8 Sap;          /* Socket ID */
  uint32   IpAddress;    /* 32-bit IP address. */
  uint16   Port;         /* UDP port number.   */
} BalUdpbSendDataMsgT;
 
/* BAL_UPB_SENT_DATA_RSP_MSG */
typedef PACKED struct
{
  uint8            Sap;
  BalSocketStatusT status;
} BalUdpbSentDataRspMsgT;
 
typedef PACKED struct
{
  uint32   IpAddress;    /* 32-bit IP address. */
  uint16   PortNumber;   /* UDP port number.   */
} BalAppAddrT;
 
/* BAL_UPB_RECV_DATA_MSG */
typedef PACKED struct
{
  uint8*       Data;  /* Point to data buffer.        */
  uint16       Size;  /* Size of datagram received.   */
  BalAppAddrT  FromAddress;
  uint8        Sap;
} BalUpbRecvDataMsgT;

typedef PACKED struct
{
  TcpbStatusT status;
  UINT32      LocalIPAddr;
  UINT32      RemoteIPAddr;
  UINT32      PriDNSAddr;
  UINT32      SecDNSAddr;
} BalHlwPppConnStatusMsgT;
 
typedef PACKED struct
{
  TcpbStatusT status;
} BalHlwPppCloseStatusMsgT;
 
typedef PACKED struct
{
  int16            socketId;
  uint8            sap;
  BalSocketStatusT status;
} BalHlwSocketCreateStatusMsgT;
 
typedef PACKED struct
{
  uint8            sap;
  BalSocketStatusT status;
} BalHlwSocketBindStatusMsgT;
 
typedef PACKED struct
{
  uint8            sap;
  BalSocketStatusT status;
	uint32 srcIP;
	uint32 destIP;
	uint16 srcPort;
	uint16 dstPort;
} BalHlwSocketConnStatusMsgT;
 
typedef PACKED struct
{
  int16 SockListenfd;
  uint8 SocketNum;
  int16 sockfd[5];
  uint8 sap[5];
}BalHlwSocketListenStatusMsgT;

typedef PACKED struct
 {
    uint8 sap;
    uint8 opt_type;
    uint16 size;
    BalSocketStatusT status;
 }BalHlwSocketOptStatusMsgT;

 typedef PACKED struct
 {
    uint8            sap;
    BalSocketStatusT status;
    int8           how;
  } BalHlwSocketShutDownStatusMsgT;

 typedef PACKED struct
 {
    uint8            sap;
    bool             lingerOnOff;
    uint32          lingerTime;
    BalSocketStatusT status;
 } BalHlwSocketLingerStatusMsgT;
/*-----------------------------------------------------------------
 * OTASPHandler
 *----------------------------------------------------------------*/
 
 /* BAL_IOTA_683_RSP_MSG */
 typedef PACKED struct
 {
    UINT8 *IS683Rsp;
    UINT16 Length;
 } BalIOTARspMsgT;
 
 /* BAL_IOTA_PRL_RSP_MSG */
 typedef PACKED struct
 {
    UINT8 *PRLRsp;
    UINT16 Length;
 } BalIOTAPRLRspMsgT;
 
 /* BAL_OTASP_IND_MSG */
 typedef enum
 {
    BAL_OTASP_Connect,
    BAL_OTA_Disconnect,
    BAL_OTAPA_SessionStart,
    BAL_OTAPA_SessionStop,
    BAL_OTA_SPASMUnlock,
    BAL_OTAPA_VerifyOK,
    BAL_OTA_AkeyEx,
    BAL_OTA_SPLUnlock,
    BAL_OTA_SSDUpd,
    BAL_OTA_NAMDownload,
    BAL_OTA_MDNDownload,
    BAL_OTA_IMSIDownload,
    BAL_OTA_PRLDownloading,    
    BAL_OTA_DataMaskCommitToNVRAM,  
    BAL_OTA_CommitOK,
    BAL_OTA_ExcessSPCFailure,
    BAL_OTA_AnalogCDMADownload,    
    BAL_OTA_PRLDownload,  
    BAL_OTA_PRLDownloadFail,
    BAL_OTA_ProgrammingFail,
    BAL_OTA_Premature_Terminate,
    BAL_OTA_VendorLock_Reset 
 } BalOtaStatusIndT;
 
 typedef PACKED struct
 {
    BalOtaStatusIndT  status;
 } BalOtaspIndMsgT;
 
 /* BAL_OTASP_TX_RSP_MSG */
 typedef PACKED struct
 {
    bool  Success;
 } BalOtaspTxRspMsgT;
 
 /*-----------------------------------------------------------------
 * LocHandler
 *----------------------------------------------------------------*/
 
 /* BAL_LOC_RESP_MSG */
 typedef PACKED struct
 {
    uint16 TimeRefCDMA;
    uint32 LAT;
    uint32 LONG;
    uint8  LocUncrtnyAng;
    uint8  LocUncrtnyA;
    uint8  LocUncrtnyP;
    uint8  FixType;
    bool   VelocityIncl;
    uint16 VelocityHor;
    uint16 Heading;
    uint8  VelocityVer;
    bool   ClockIncl;
    uint32 ClockBias;
    uint16 ClockDrift;
    bool   HeightIncl;
    uint16 Height;
    uint8  LocUncrtnyV;
 } BalLocRespMsgT;

 typedef PACKED struct
 {
    uint16 wSysID;             /* System Identification */
    uint16 wNetID;             /* Network Identification */
    uint16 wBaseID;            /* Base Station Identification */
    uint16 wBaseClass;         /* Base Station Class */
    uint16 wBestPN;            /* Best Pilot */
    uint16 wPacketZoneID;      /* Packet Data Service Zone Identifier */
    uint16 wMobileCountryCode; /* Mobile country code */
 } BalLbsSectorInfo;

 typedef PACKED struct
 {
    BalLbsSectorInfo SectorInfo; 
 } BalLocGetSectorInfoRspMsgT;
 
 /* BAL_LOC_ERROR_MSG */
 typedef enum
 {
    TCP_NOT_SUPP,
       MS_ORIG_NOT_SUPP,
       NUM_MULTIPLE_SESS_EXCEEDED,
       MS_ORIG_SESS_ALREADY_ACTIVE,
       NETWORK_CANCEL_RCVD,
       DSP_ERROR,
       PDE_ERROR
 } BalLocErrorTypes;
 
 typedef PACKED struct
 {
    BalLocErrorTypes Error;
 } BalLocErrorMsgT;
 
#define MAX_NUM_PSEUDO_RANGE_PARAM   9

typedef PACKED struct
{
   uint8 SvPrnNum;
   uint8 SvCno;
   uint16 SvCodePhWh;
   uint16 SvCodePhFr;
} PseudoRangeParamT;

typedef PACKED struct
{
   uint8 NumPsRanges;
   PseudoRangeParamT psRanges[MAX_NUM_PSEUDO_RANGE_PARAM];
} BalLocPseudoRangeMeasMsgT;
 /*-----------------------------------------------------------------
 * PSWAmpsHandler
 *----------------------------------------------------------------*/
 
 /* BAL_AMPS_EXT_PROTO_MSG */
 typedef enum
 {
    BAL_AMPS_MST_VMAIL,
    BAL_AMPS_MST_CLI,
    BAL_AMPS_MST_SMS
 } BalAmpsExtProtoType;
 
 typedef PACKED struct
 {
    BalAmpsExtProtoType ExtProtoType;
    bool                SoundAudibleAlertFlag;
    uint16              MsgLen;
    uint8               MsgData[1];
 } BalAmpsExtProtoMsgT;
 
 /*-----------------------------------------------------------------
 * PSWHandler
 *----------------------------------------------------------------*/
 
 /* common ds to 3 BAL_NWK_RPT_xxx messages */
 
#define BAL_MAX_DISPLAY_SIZE 62
 
typedef enum
{
   BAL_NP_UNKNOWN         = 0,
   BAL_NP_ISDN_TELEPHONY,
   BAL_NP_DATA            = 3,
   BAL_NP_TELEX,
   BAL_NP_PRIVATE         = 9,   
   BAL_NUMBER_PLAN_INVALID
} BalNumberPlanT;

typedef enum
{
   BAL_NT_UNKNOWN = 0,
   BAL_NT_INTERNATIONAL,
   BAL_NT_NATIONAL,
   BAL_NT_NETWORK_SPECIFIC,
   BAL_NT_SUBSCRIBER,
   BAL_NT_RSVD,
   BAL_NT_ABBREVIATED, 
   BAL_NUMBER_TYPE_INVALID
} BalNumberTypeT;
 
#define BAL_MAX_CALLED_PARTY_NUMBER_SIZE 15  /* ??? */
typedef PACKED struct
{
   BalNumberTypeT Type;
   BalNumberPlanT Plan;
   uint8       Number[BAL_MAX_CALLED_PARTY_NUMBER_SIZE+1];
} BalCalledPartyNumberT;

typedef enum
{
   BAL_PI_ALLOWED = 0,
   BAL_PI_RESTRICTED,
   BAL_PI_NUMBER_NA,
   BAL_PI_RSVD,
   BAL_PRESENTATION_INDICATOR_INVALID
} BalPresentationIndicatorT;

typedef enum
{
   BAL_SI_USER_PROVIDED_NOT_SCREENED = 0,
   BAL_SI_USER_PROVIDED_VERIFIED_PASSED,
   BAL_SI_USER_PROVIDED_VERIFIED_FAILED,
   BAL_SI_NETWORK_PROVIDED,
   BAL_SCREENING_INDICATOR_INVALID
} BalScreeningIndicatorT;

#define BAL_MAX_CALLING_PARTY_NUMBER_SIZE 15 /* ??? */
typedef PACKED struct
{
   BalNumberTypeT Type;
   BalNumberPlanT Plan;
   BalPresentationIndicatorT Pi;
   BalScreeningIndicatorT Si;
   uint8 Number[BAL_MAX_CALLING_PARTY_NUMBER_SIZE+1];
} BalCallingPartyNumberT;

typedef enum
{
   BAL_ST_TONE_SIGNAL = 0,
   BAL_ST_ISDN_ALERTING,
   BAL_ST_IS54B_ALERTING,
   BAL_ST_RCVD,
   BAL_SIGNAL_TYPE_INVALID
} BalSignalTypeT;

typedef enum
{
   BAL_PITCH_MEDIUM = 0,
   BAL_PITCH_HIGH,
   BAL_PITCH_LOW,
   BAL_PITCH_RSVD,
   BAL_ALERT_PITCH_INVALID
} BalAlertPitchT;

typedef PACKED struct
{
   BalSignalTypeT Type;
   BalAlertPitchT Pitch;
   uint8 Signal;
} BalSignalT;

typedef enum
{
   BAL_SUBADDRESSTYPE_NSAP = 0,
   BAL_SUBADDRESSTYPE_USER = 2,
   BAL_SUBADDRESSTYPE_RESERVED
} BalSubaddressTypeT;

typedef enum
{
   BAL_INDICATOR_EVEN = 0,
   BAL_INDICATOR_ODD
} BalOddEvenIndicatorT;
 
#define BAL_MAX_SUBADDRESS_NUMBER_SIZE BAL_MAX_CALLED_PARTY_NUMBER_SIZE /* ??? */
 typedef PACKED struct
 {
    uint8                ExtensionBit;
    BalSubaddressTypeT   SubaddressType;
    BalOddEvenIndicatorT OddEvenIndicator;
    uint8                Number[BAL_MAX_SUBADDRESS_NUMBER_SIZE + 1];
 } BalSubaddressT;
 
 typedef PACKED struct
 {
    uint8 UpLink;
    uint8 DownLink;
 } BalAudioControlT;
 
 typedef PACKED struct
 {
    bool                    RcvdDisplay;
    uint8                   Display[BAL_MAX_DISPLAY_SIZE + 1];
    
    bool                    RcvdCalledPartyNumber;
    BalCalledPartyNumberT   CalledPartyNumber;
    
    bool                    RcvdCallingPartyNumber;
    BalCallingPartyNumberT  CallingPartyNumber;
    
    bool                    RcvdConnectedNumber;
    BalCallingPartyNumberT  ConnectedNumber;   /* Same format as CallingPartyNumber */
    
    bool              RcvdSignal;
    BalSignalT        Signal;
    
    bool              RcvdMessageWaiting;
    uint8             MsgCount;
    
    bool              RcvdClirCause; /* ??? */
    uint8             ClirCause;
    
    bool              RcvdRelease;
    
    bool              RcvdAudioControl;
    BalAudioControlT  AudioControl;
    
    bool              RcvdCalledPartySubaddress;       /* added since related to Numbers */
    BalSubaddressT    CalledPartySubaddress;
    
    bool              RcvdCallingPartySubaddress;
    BalSubaddressT    CallingPartySubaddress;
    
    bool              RcvdConnectedSubaddress;
    BalSubaddressT    ConnectedSubaddress;
    
    bool              MoreData;
 } BalNwkRptInfoMsgT;
 
 /* BAL_ACCESS_MAX_CAPSULE_SIZE_MSG */
 typedef PACKED struct
 {
    uint8 Size;
 } BalAccessMaxCapsuleSizeMsgT;
 
 /* BAL_BS_CO_ORD_MSG */
 typedef PACKED struct
 {
    uint16   baseId;
    int32    baseLat;
    int32    baseLong;
 } BalBSCoOrdMsgT;
 
 /* BAL_NAM_MSG */
 typedef PACKED struct
 {
    NamNumber     namNumber;
    PswIs95NamT   namFields;
 } BalNamMsgT;
 
#ifdef KDDI_EXTENSIONS
 /* BAL_PSW_ANTENNA_BARS_RPT_MSG */
 typedef PACKED struct
 {
    uint8    antennabars;
 } BalPswAntennaBarsRptMsgT;
#endif
 
 /* BAL_NWK_RPT_ALERT_MSG */
 typedef BalNwkRptInfoMsgT BalNwkRptAlertMsgT;
 
 /* BAL_NWK_RPT_ALERT_MORE_INFO_REC_MSG */
 /* BAL_NWK_RPT_FLASH_MORE_INFO_REC_MSG */
 /* BAL_NWK_RPT_FEAT_NOTIF_MORE_INFO_REC_MSG */
 typedef enum
 {
    BAL_REDIRECTION_UNKNOWN = 0,
    BAL_REDIRECTION_BUSY,
    BAL_REDIRECTION_NO_REPLY,
    BAL_REDIRECTION_DTE_OUT_OF_ORDER = 0x9,
    BAL_REDIRECTION_FORWARDING_BY_DTE,
    BAL_REDIRECTION_UNCONDITIONAL_OR_SYSTEMATIC = 0xF
 } BalRedirectionReasonT;
 
#define BAL_MAX_REDIRECTING_NUMBER_SIZE 15
 typedef PACKED struct
 {
    uint8 ExtensionBit1;
    BalNumberTypeT Type;
    BalNumberPlanT Plan;
    
    uint8 ExtensionBit2;
    BalPresentationIndicatorT  Pi;
    BalScreeningIndicatorT     Si;
    
    uint8 ExtensionBit3;
    BalRedirectionReasonT   RedirectionReason;
    uint8 Number[BAL_MAX_REDIRECTING_NUMBER_SIZE+1];
 } BalRedirectingNumberT;

 typedef PACKED struct
 {
    uint16   PulseFrequency;
    uint8 PulseOnTime;
    uint8 PulseOffTime;
    uint8 PulseCount;
 } BalMeterPulsesT;
 
 typedef PACKED struct
 {
    uint8 Amplitude;
    uint16   Freq1;
    uint16   Freq2;
    uint8 OnTime;
    uint8 OffTime;
    uint8 Repeat;
    uint8 Delay;
 } BalCadenceGroup;
 
#define BAL_MAX_CADENCE_GROUPS_REC 6
 typedef PACKED struct
 {
    uint8       CadenceCount;
    uint8       NumGroups;
    BalCadenceGroup   CadenceGroup[BAL_MAX_CADENCE_GROUPS_REC];
 } BalParametricAlertingT;
 
 typedef PACKED struct
 {
    uint8 PolarityIncluded;
    uint8 ToggleMode;
    uint8 ReversePolarity;
    uint8 PowerDenialTime;
 } BalLineControlT;
 
 typedef enum
 {
    BAL_DISPLAYTAG_BLANK = 0x80,
    BAL_DISPLAYTAG_SKIP,
    BAL_DISPLAYTAG_CONTINUATION, 
    BAL_DISPLAYTAG_CALLED_ADDRESS, 
    BAL_DISPLAYTAG_CAUSE, 
    BAL_DISPLAYTAG_PROGRESS_INDICATOR, 
    BAL_DISPLAYTAG_NOTIFICATION_INDICATOR, 
    BAL_DISPLAYTAG_PROMPT, 
    BAL_DISPLAYTAG_ACCUMULATED_DIGITS, 
    BAL_DISPLAYTAG_STATUS, 
    BAL_DISPLAYTAG_INBAND, 
    BAL_DISPLAYTAG_CALLING_ADDRESS, 
    BAL_DISPLAYTAG_REASON, 
    BAL_DISPLAYTAG_CALLING_PARTY_NAME, 
    BAL_DISPLAYTAG_CALLED_PARTY_NAME, 
    BAL_DISPLAYTAG_ORIGINAL_CALLED_NAME, 
    BAL_DISPLAYTAG_REDIRECTING_NAME, 
    BAL_DISPLAYTAG_CONNECTED_NAME, 
    BAL_DISPLAYTAG_ORIGINATING_RESTRICTIONS, 
    BAL_DISPLAYTAG_DATE_TIME_OF_DAY, 
    BAL_DISPLAYTAG_CALL_APPEARANCE_ID, 
    BAL_DISPLAYTAG_FEATURE_ADDRESS, 
    BAL_DISPLAYTAG_REDIRECTION_NAME, 
    BAL_DISPLAYTAG_REDIRECTION_NUMBER, 
    BAL_DISPLAYTAG_REDIRECTING_NUMBER, 
    BAL_DISPLAYTAG_ORIGINAL_CALLED_NUMBER, 
    BAL_DISPLAYTAG_CONNECTED_NUMBER, 
    BAL_DISPLAYTAG_TEXT = 0x9E
 } BalDisplayTagT;
 
 typedef PACKED struct
 {
    BalDisplayTagT DisplayTag;
    uint8       DisplayLen;
    uint8       Chari[BAL_MAX_DISPLAY_SIZE + 1];
 } BalExtendedDisplayRec;
 
#define BAL_NUM_EXTENDED_DISPLAY_REC 2
 typedef PACKED struct
 {
    uint8             ExtDisplayInd;
    uint8                DisplayType;
    BalExtendedDisplayRec   ExtendedDisplayRec[BAL_NUM_EXTENDED_DISPLAY_REC];
 } BalExtendedDisplayT;
 
 typedef PACKED struct
 {
    bool  RcvdRedirectingNumber;
    BalRedirectingNumberT   RedirectingNumber;
    bool  RcvdRedirectingSubaddress;
    BalSubaddressT       RedirectingSubaddress;
    bool  RcvdMeterPulses;
    BalMeterPulsesT         MeterPulses;
    bool  RcvdParametricAlerting;
    BalParametricAlertingT  ParametricAlerting;
    bool  RcvdLineControl;
    BalLineControlT         LineControl;
    bool  RcvdExtendedDisplay;
    BalExtendedDisplayT     ExtendedDisplay;
 } BalNwkRptAddInfoMsgT;
 
 /* BAL_NWK_RPT_BURST_DTMF_MSG */
 typedef PACKED struct
 {
    uint8 DtmfOnLength;
    uint8 DtmfOffLength;
    uint8 Digits[1];
 } BalNwkRptBurstDtmfMsgT;
 
 /* BAL_NWK_RPT_CP_EVENT_MSG */
 typedef enum
 {
    BAL_EV_DISCONNECT = 1,
    BAL_EV_PAGE_RECEIVED,   
    BAL_EV_STOPALERT,
    BAL_EV_ABBR_ALERT,
    BAL_EV_NOSVC,
    BAL_EV_CDMA_INSVC,
    BAL_EV_CPENABLED,
    BAL_EV_CPDISABLED,
    BAL_EV_PDOWNACK,
    BAL_EV_CONNANALOG,
    BAL_EV_CONNDIGITAL,
    BAL_EV_ASSIGNED,
    BAL_EV_ORIG_FAIL,
    BAL_EV_NDSS_ORIG_CANCEL,
    BAL_EV_INTERCEPT,       
    BAL_EV_REORDER,       
    BAL_EV_CC_RELEASE,
    BAL_EV_CC_RELEASE_SO_REJ,
    BAL_EV_FNM_RELEASE,
    BAL_EV_DIALING_COMPLETE,
    BAL_EV_DIALING_CONTINUE,
    BAL_EV_MAINTENANCE,
    BAL_EV_VP_ON,
    BAL_EV_VP_OFF,
    BAL_EV_PSIST_FAIL,
    BAL_EV_TC_RELEASE_MS,
    BAL_EV_TC_RELEASE_PDOWN, 
    BAL_EV_TC_RELEASE_DISABLE, 
    BAL_EV_TC_RELEASE_BS,
    BAL_EV_TC_RELEASE_SO_REJECT,
    BAL_EV_TC_RELEASE_TIMEOUT,
    BAL_EV_TC_RELEASE_ACK_FAIL, 
    BAL_EV_TC_RELEASE_FADE, 
    BAL_EV_TC_RELEASE_LOCK,  
    BAL_EV_PAGE_FAIL,
    BAL_EV_RETRY_TIMER_ACTIVE,
    BAL_EV_RETRY_TIMER_INACTIVE,
    BAL_EV_AMPS_INSVC,
    BAL_EV_PKT_ACTIVE, /*cds405 status defination*/
    BAL_EV_PKT_INACTIVE, /*cds405 status defination*/
    BAL_EV_PKT_RELEASED,
    BAL_EV_PKT_DORMANT,

    BAL_EV_ORIG_USER_CANCEL = 50, /*cds405 status defination*/

    BAL_EV_FLASHFAIL = 80,
    BAL_EV_ANALOG_PAGE_RECEIVED,
    BAL_EV_AMPS_IDLE,
    BAL_EV_ABBR_INTERCEPT,
    BAL_EV_ABBR_REORDER,
    BAL_EV_CONNCOMPLETE,
    BAL_EV_ENTER_OOSA,
    BAL_EV_MSID_UPDATED,
    BAL_EV_PSIST_FAIL_PHONE_REG, /*cds405 status defination*/
     
    BAL_RPT_CP_EVENT_INVALID
 } BalRptCpEventT;
 
 typedef PACKED struct
 {
    BalRptCpEventT Msg;
 } BalRptCpEventMsgT;
 
 /* BAL_NWK_RPT_CP_RESPONSE_MSG */
 typedef PACKED struct
 {
    uint8 Response;
    uint8 Request;
 } BalNwkRptCpResponseMsgT;
 
 /* BAL_NWK_RPT_CP_STATUS_MSG */
 typedef enum
 {
    BAL_CP_DISABLED,            /* CP Disabled                              */
    BAL_CP_SYS_DETERMINATION,   /* System Determination (6.6.1.1)           */
    BAL_CP_PILOT_ACQUISITION,   /* Pilot Channel Acquisition (6.6.1.2)      */
    BAL_CP_SYNC_ACQUISITION,    /* Sync Channel Acquisition (6.6.1.3)       */
    BAL_CP_TIMING_CHANGE,       /* Timing Change (6.6.1.4)                  */
    BAL_CP_CDMA_IDLE,           /* Idle (6.6.2)                             */
    BAL_CP_UPDATE_OHD_INFO,     /* Update Overhead Information (6.6.3.2)    */
    BAL_CP_PAGE_RESPONSE,       /* Page Response (6.6.3.3)                  */
    BAL_CP_ORD_MSG_RESP,        /* Order/Message Response (6.6.3.4)         */
    BAL_CP_ORIGINATION,         /* Origination Attempt (6.6.3.5)            */
    BAL_CP_REGISTRATION,        /* Registration Attempt (6.6.3.6)           */
    BAL_CP_MSG_TRANSMISSION,    /* Message Transmission (6.6.3.7)           */
    BAL_CP_TC_INIT,             /* Traffic Channel Initialization (6.6.4.2) */
    BAL_CP_TC_WAIT_ORDER,       /* TC Waiting for Order (6.6.4.3.1)         */
    BAL_CP_TC_WAIT_ANSWER,      /* TC Waiting for MS Answer (6.6.4.3.2)     */
    BAL_CP_TC_CONVERSATION,     /* TC Conversation (6.6.4.4)                */
    BAL_CP_TC_RELEASE,          /* TC Release (6.6.4.5)                     */
    
#if (defined SYS_OPTION_AMPS)
    BAL_CP_AMPS_WBIOS_INIT,                /* Initialize hardware and L1d    */
    BAL_CP_AMPS_INIT,                      /* Initialize                     */
    BAL_CP_AMPS_CCSCAN,                    /* Scan Dedicated Control Channels*/
    BAL_CP_AMPS_CONTROL,                   /* Control Channel                */
    BAL_CP_AMPS_PCSCAN,                    /* Scan Paging Channels           */
    BAL_CP_AMPS_PAGING,                    /* Paging Channel                 */
    BAL_CP_AMPS_IDLE,                      /* Idle (Ready) State             */
    BAL_CP_AMPS_ACSCAN,                    /* Scan Access Channels           */
    BAL_CP_AMPS_ACCESS,                    /* Access Channel                 */
    BAL_CP_AMPS_AXPARS,                    /* Get Access Attempt Parameters  */
    BAL_CP_AMPS_ACOMT,                     /* Wait for OMT on Access Channel */
    BAL_CP_AMPS_SZRECC,                    /* Seize Reverse Control Channel  */
    BAL_CP_AMPS_SVCREQ,                    /* Service Request                */
    BAL_CP_AMPS_TXSTAT,                    /* Wait for Transmit Status       */
    BAL_CP_AMPS_AWAITMSG,                  /* Await Message                  */
    BAL_CP_AMPS_REGCONF,                   /* Await Registration Confirmation*/
    BAL_CP_AMPS_REGCONF_WFOMT,             /* Await Registration Confirmation*/
    BAL_CP_AMPS_SERVSYS,                   /* Serving System Determination   */
    BAL_CP_AMPS_DIRRETRY,                  /* Directed Retry                 */
    BAL_CP_AMPS_VCCONF,                    /* Confirm Initial Voice Channel  */
    BAL_CP_AMPS_WAITORD,                   /* Waiting for Order              */
    BAL_CP_AMPS_WAITANS,                   /* Waiting for Answer             */
    BAL_CP_AMPS_CONVERSE,                  /* Conversation                   */
    BAL_CP_AMPS_RELEASE,                   /* Release                        */
    BAL_CP_AMPS_RLSDELAY,                  /* Delay before Release           */
    BAL_CP_AMPS_HANDOFF,                   /* Do Handoff Procedure           */
    BAL_CP_AMPS_SYSDET_DELAY,              /* Delay before system determination */
    BAL_CP_AMPS_NUM_STATES,                /* Must be last of AMPS states    */
#endif /* SYS_OPTION_AMPS */
    
    BAL_CP_NUM_STATES,          /* Number of Call Processing States         */
    BAL_CP_STATE_INVALID        /* Used by PSW for exception handling       */
 } BalCPStateT;

 typedef PACKED struct
 {
    BalCPStateT State;                /* Call Processing State                */	
    BalCPStateT PrevState;            /* Previous Call Processing State       */
    uint8 Roam;                       /* ROAM Status                          */
    uint16 Band;                      /* Current Operating Band               */
    uint16 Channel;                   /* Current Channel Number               */
    uint8  Mode;                      /* current mode: PCS/CellularAnalog     */
    uint8  Block;                     /* current CDMA block (if CDMA system)  */
    uint8 ServingSystem;              /* Serving System/Block                 */
    uint16 SysID;                     /* Last-Received System ID  (sid)    cds405 status defination*/   
    uint16 LocArea;                   /* Current Location Area ID (nid)    cds405 status defination*/   
    uint16 PilotPn;                   /* PILOT_PN                             */
 } BalRptCpStatusMsgT;

 /* BAL_NWK_RPT_DATA_BURST_MSG */
 typedef PACKED struct
 {
    uint8 MsgNumber;
    uint8 BurstType;
    uint8 NumMsgs;
    uint8 NumFields;
    uint8 Data[1];
 } BalNwkRptDataBurstMsgT;
 
 /* BAL_NWK_RPT_FEATURE_NOTIFICATION_MSG */
 typedef BalNwkRptInfoMsgT BalNwkRptFeatureNotificationMsgT;
 
 /* BAL_NWK_RPT_FLASH_MSG */
 typedef BalNwkRptInfoMsgT BalNwkRptFlashMsgT;
 
 /* BAL_NWK_RPT_HANDOFF_MSG */
 typedef enum
 {
    BAL_IDLE_SAME_SYSTEM = 1,
    BAL_IDLE_NEW_SYSTEM,
    BAL_SOFT,
    BAL_HARD,
    BAL_TC_TO_AVC
 } BalHandoffTypeT;
 
 typedef PACKED struct
 {
    bool DisjointActiveSet;
    bool FrameOffsetChange;
    bool ChannelChange;
    bool LongCodeChange;
    bool ServCfgChange;
 } BalHandoffTCReportT;
 
 typedef PACKED struct
 {
    BalHandoffTypeT      Type;
    BalHandoffTCReportT  Report;
 } BalNwkRptHandoffMsgT;
 
 /* BAL_NWK_RPT_REGISTRATION_ACCEPT_MSG */
 typedef PACKED struct
 {
    bool RoamIndPresent;
    uint8 RoamInd;
 } BalNwkRptRegistrationAcceptMsgT;
 
 /* BAL_NWK_RPT_SERVICE_CONFIG_MSG */
 typedef enum
 {
    BAL_MUX_OPTION1 = 1,
    BAL_MUX_OPTION2,
    BAL_MUX_OPTION3,
    BAL_MUX_OPTION4,
    BAL_MUX_OPTION5,
    BAL_MUX_OPTION6,
    BAL_MUX_OPTION7,
    BAL_MUX_OPTION8,
    BAL_MUX_OPTION9,
    BAL_MUX_OPTION10,
    BAL_MUX_OPTION11,
    BAL_MUX_OPTION12,
    BAL_MUX_OPTION13,
    BAL_MUX_OPTION14,
    BAL_MUX_OPTION15,
    BAL_MUX_OPTION16
 } BalMultiplexOptionT;
 
 typedef enum
 {
    BAL_SERVICE_OPTION_2 = 2,  /* Loopback               */
    BAL_SERVICE_OPTION_3,      /* Voice                  */
    BAL_SERVICE_OPTION_4,      /* Async Data Rate Set I  */
    BAL_SERVICE_OPTION_5,      /* Asunc Fax Rate Set I   */
    BAL_SERVICE_OPTION_6,      /* SMS                    */ 
    BAL_SERVICE_OPTION_7,      /* Packet Data            */
    BAL_SERVICE_OPTION_8,      /* Packet Data            */
    BAL_SERVICE_OPTION_9,      /* Loopback               */
    BAL_SERVICE_OPTION_10,     /* Unknown                */
    BAL_SERVICE_OPTION_11,     /* Unknown                */
    BAL_SERVICE_OPTION_12,     /* Async Data Rate Set II */
    BAL_SERVICE_OPTION_13,     /* Asunc Fax Rate Set II  */
    BAL_SERVICE_OPTION_14,     /* SMS                    */
    BAL_SERVICE_OPTION_15,     /* Packet Data            */
    BAL_SERVICE_OPTION_16,     /* Packet Data            */
    BAL_SERVICE_OPTION_17,     /* Voice                  */
    BAL_SERVICE_OPTION_18,     /* OTASP                  */
    BAL_SERVICE_OPTION_19,     /* OTASP                  */
    BAL_SERVICE_OPTION_20,     /* Unknown                */
    BAL_SERVICE_OPTION_21,     /* Unknown                */
    BAL_SERVICE_OPTION_22,     /* Packet Data            */
    BAL_SERVICE_OPTION_23,     /* Packet Data            */
    BAL_SERVICE_OPTION_24,     /* Packet Data            */
    BAL_SERVICE_OPTION_25,     /* Packet Data            */
    BAL_SERVICE_OPTION_26,     /* Packet Data            */
    BAL_SERVICE_OPTION_27,     /* Packet Data            */
    BAL_SERVICE_OPTION_28,     /* Packet Data            */
    BAL_SERVICE_OPTION_29      /* Packet Data            */
 } BalServiceOptionT;
 
 typedef enum
 {
    BAL_NO_TRAFFIC = 0,
    BAL_PRIMARY_TRAFFIC,
    BAL_SECONDARY_TRAFFIC
 } BalTrafficTypeT;
 
 typedef PACKED struct
 {
    uint8       ConRef;
    uint16          ServiceOption;
    BalTrafficTypeT ForTraffic;
    BalTrafficTypeT RevTraffic;
 } BalConnectionRecordT;
 
#define BAL_MAX_CON_REC 10  /* not defined in the isotel spec */
 typedef PACKED struct
 {
    BalMultiplexOptionT  ForMuxOption;
    BalMultiplexOptionT  RevMuxOption;
    uint8              ForRates;
    uint8              RevRates;
    uint8              NumConRec;
    BalConnectionRecordT ConRec[BAL_MAX_CON_REC];
 } BalRptSvcConfigMsgT;
 
 /* BAL_NWK_RPT_SERVICE_READY_MSG */
 typedef enum
 {
    BAL_NWK_RPT_BAND_CLASS_0 = 0, /* Cellular */
    BAL_NWK_RPT_BAND_CLASS_1,     /* PCS      */
    BAL_NWK_RPT_BAND_CLASS_3 = 3, /* T53      */
    BAL_NWK_RPT_BAND_CLASS_NOT_USED
 } BalNwkRptBandClassT;
 
 typedef PACKED struct
 {
    BalNwkRptBandClassT BandClassInd;
    uint8          BandBlockInd;  
    uint16              Sid;        
    uint16              Nid;           
    uint8               NetworkType; 
    uint8               ProtRev;
 } BalNwkRptServiceReadyMsgT;          
 
 /* BAL_NWK_RPT_SYSTEM_TIME_MSG */
#define BAL_SYSTEM_TIME_LEN SYS_SYSTIME_SIZE /* from sysdefs.h */
 typedef PACKED struct
 {
    uint8 SysTime[BAL_SYSTEM_TIME_LEN];
    uint8 LpSec;
    int8  LtmOff;
    uint8 DayLt;
 } BalNwkRptSystemTimeMsgT;
 
 /* BAL_NWK_RPT_PKT_STATE_MSG */
 typedef enum
 {
    PKT_NULL,
    PKT_ORIGINATED,   /* Origination confirmation */
    PKT_CONNECTED,    /* RLP link established     */
    PKT_DORMANT
 } BalPktStateT;
 
 typedef struct
 {
    BalPktStateT state;
 } BalNwkRptPktStateMsgT;
 
 /* BAL_NWK_RPT_PKT_DORM_TMR_MSG */
 typedef struct
 {
    uint8  dormTime;   /* unit of 0.1 sec */
 } BalNwkRptPktDormTmrMsgT;
 
 /* BAL_PRL_INFO_MSG */
 typedef struct
 {
    NamNumber   namNumber;
    void     *PtrToPrl;
 } BalPrlInfoMsgT;
 
 /* Set MobileID message */
typedef PACKED struct
{
  ExeRspMsgT  RspInfo;         /* Response routing information */
  uint32 SPC;
  uint64 securitycode;
  uint8       Id;
  uint64       value;
} BalSetMobileIDMsgT;

typedef PACKED struct
{
  bool Result;
  uint8 ResponseCode;
} BalSetMobileIDResponseMsgT;

 /* BAL_PSW_E911_MODE_MSG */
 typedef PACKED struct
 {
    bool  mode;   
 } BalEmergencyModeMsgT;
 
 /* BAL_ROAM_INDICATION_MSG */
 typedef PACKED struct
 {
    uint8 RoamInd;   
 } BalRoamIndicationMsgT;
typedef PACKED struct
{
  ExeRspMsgT  RspInfo;         /* Response routing information */
} BalEriVersionNumberMsgT;
typedef PACKED struct
{
  uint16            eri_version_number;
} BalEriVersionNumberRspMsgT;
 
 
 /* BAL_RPT_CQA_MSG */
 typedef PACKED struct
 {
    bool Alarm;
 } BalReportCQAMsgT;
 
#ifdef KDDI_EXTENSIONS
 /* BAL_EXTENDED_DATA_BURST_RX_IND_MSG */
 typedef PACKED struct
 {
    uint16 MCC;
    uint8   DB_Subtype;
    uint8   Chg_Ind;
    uint8   Subunit;
    uint8   Unit;
 } BalExtendedBurstRxIndMsgT;
#endif
 
 /*-----------------------------------------------------------------
 * SMSHandler
 *----------------------------------------------------------------*/
 
/* common to a couple of messages */
typedef PACKED struct
{
  uint8 ErrorClass;
  uint8 CauseCode;
} BalSmsCauseCodeT;
   
/* BAL_SMS_CAUSE_CODE_STATUS_MSG */
typedef PACKED struct
{
  uint16           SeqNum;  /* The PSW-BAL SeqNum id used in msg assembly  */
  uint8            SmsType; /* 0 = MO and 1 = MT, so can tell which SeqNum */
  BalSmsCauseCodeT Error;   /* Error condition (if any) of last SMS message   */
} BalSmsCauseCodeStatusMsgT;
 
 /* BAL_SMS_DELIVER_IND_DATA_MSG */
#define BAL_SMS_MAX_USERDATA_LENGTH    200   /* ??? use consts to avoid recompiling PSW */
typedef PACKED struct
{
   uint16 SeqNum;
   uint8  MsgType;    
   uint8  NumMsgs;                          
   uint8  MsgEncoding;       
   uint8  NumFields;                          /* Length of data in octets        */
   uint8  Char[ BAL_SMS_MAX_USERDATA_LENGTH ];     /* User Data field of message      */
} BalSmsUserDataMsgT;
 
 /* BAL_SMS_RX_IND_MSG */
#define BAL_MAX_SMS_SIZE   256   /* ??? change this to header + (maxsize -header) */
typedef PACKED struct
{
  bool  Broadcast;
  uint8 MsgData[BAL_MAX_SMS_SIZE];
  bool  DataTruncated; /* TRUE: if ascii message is longer than BAL_MAX_SMS_SIZE */
} BalSmsRxIndMsgT;
 
/* BAL_SMS_TX_RSP_MSG */
typedef PACKED struct
{
  bool  TxStatus;
} BalSmsTxRspMsgT;

/* BAL_SMS_TST_CONNECT_MSG */
typedef PACKED struct
{
  uint16   TeleSrvId;
} BalSmsTstConnectMsgT;

/* BAL_SMS_TST_BCAST_CONNECT_MSG */
/* BAL_SMS_TST_BCAST_PREF_MSG */
typedef PACKED struct
{
  uint32   SvcMask;
  uint32   LangMask;
  uint8 Priority;
} BalSmsTstBCastParmsMsgT;

/* BAL_SMS_TST_CANCEL_MSG */
typedef PACKED struct
{
  uint16 TeleSrvId;
  uint16 MsgId;
} BalSmsTstCancelMsgT;

/* BAL_SMS_TST_DISCONNECT_MSG */
typedef PACKED struct
{
  uint16 TeleSrvId;
} BalSmsTstDisconnectMsgT;
 
/* BAL_SMS_TST_PREF_SRV_OPT_MSG */
typedef PACKED struct
{
  uint32 SrvOpt;   
} BalSmsTstPrefSrvOptMsgT;
 
/* BAL_SMS_TST_SUBMIT_MSG - ETS: Create a mobile originated (user submitted) */
/*                               SMS message                                 */
#define BAL_SMS_MAX_ADDRESS_CHARS      32 /* ??? use consts to avoid recompiling PSW */
 
typedef PACKED struct
{
  uint16   TeleSrvId;
  bool     SendOnTraffic;
  uint16   MsgId;
  uint8    DestNumDigits;
  char     DestDigits[BAL_SMS_MAX_ADDRESS_CHARS];
  uint8    CallbackNumDigits;
  uint8    CallbackDigits[BAL_SMS_MAX_ADDRESS_CHARS];
  uint8    DefaultMsgs;
  uint8    UserDefinedMsgEncoding;
  uint8    UserDefinedMsgOctets[SMS_MAX_USERDATA_LENGTH];
  bool     UserAck;
  bool     DeliveryAck;
  uint8    Validity;
  uint8    Validity_Year;
  uint8    Validity_Month;
  uint8    Validity_Day;
  uint8    Validity_Hours;
  uint8    Validity_Minutes;
  uint8    Validity_Seconds;
  uint8    Relative_Validity;
  uint8    Delivery;
  uint8    Delivery_Year;
  uint8    Delivery_Month;
  uint8    Delivery_Day;
  uint8    Delivery_Hours;
  uint8    Delivery_Minutes;
  uint8    Delivery_Seconds;
  uint8    Relative_Delivery;
  uint8    Priority;
  uint8    Privacy;
  uint8    AlertonDelivery;
  uint8    Language;
  uint16   ServCategory;
} BalSmsTstSubmitMsgT;
 
/* BAL_SMS_TST_TERM_STATUS_MSG */
typedef PACKED struct
{
  uint8 Status;
} BalSmsTstTermStatusMsgT;
 
/* BAL_SMS_TST_USER_ACK_MSG */
typedef PACKED struct
{
  uint16 TeleSrvId;
  uint8  MsgEncoding;
  uint8  NumFields;
  uint8  Char[BAL_SMS_MAX_USERDATA_LENGTH];
} BalSmsTstUserAckMsgT;
 
/* BAL_SMS_TST_RETX_AMOUNT_MSG */
typedef PACKED struct
{
  uint8 SmsTlMaxRetry;
} BalSmsTstRetxAmountMsgT;
 
/* -> ??? associated to what message in pswsms.c */
typedef PACKED struct
{
  bool  DigitMode;                       
  bool  NumberMode;
  uint8 NumberType;
  uint8 NumberPlan;
  uint8 NumFields;
  uint8 Char[ BAL_SMS_MAX_ADDRESS_CHARS ];
} BalSmsMCAddrT;
 
#define BAL_SMS_MAX_SUBADDRESS_CHARS   30 /* ??? use consts to avoid recompiling PSW */
typedef PACKED struct
{
  uint8 Type;
  bool  Odd;
  uint8 NumFields;
  uint8 Char[ BAL_SMS_MAX_SUBADDRESS_CHARS ];
} BalSmsMCSubaddrT;
 
typedef PACKED struct
{
  uint8 Year;
  uint8 Month;
  uint8 Day;
  uint8 Hours;
  uint8 Minutes;
  uint8 Seconds;
} BalSmsAbsTimeT;
 
typedef enum
{
  BAL_SMS_IMMEDIATE_DISPLAY,
  BAL_SMS_DEFAULT,
  BAL_SMS_USER_INVOKE
} BAL_SMS_DISPMODE;  /* ??? change to conformant typedef */
 
 typedef enum
 {
    BAL_CAT_UNKNOWN,
    BAL_CAT_EMERGENCIES,
    BAL_CAT_ADMINISTRATIVE,
    BAL_CAT_MAINTENANCE,
    BAL_CAT_GEN_NEWS_LOCAL,
    BAL_CAT_GEN_NEWS_REGIONAL,
    BAL_CAT_GEN_NEWS_NATIONAL,
    BAL_CAT_GEN_NEWS_INTERNATIONAL,
    BAL_CAT_FINANCE_NEWS_LOCAL,
    BAL_CAT_FINANCE_NEWS_REGIONAL,
    BAL_CAT_FINANCE_NEWS_NATIONAL,
    BAL_CAT_FINANCE_NEWS_INTERNATIONAL,
    BAL_CAT_SPORTS_NEWS_LOCAL,
    BAL_CAT_SPORTS_NEWS_REGIONAL,
    BAL_CAT_SPORTS_NEWS_NATIONAL,
    BAL_CAT_SPORTS_NEWS_INTERNATIONAL,
    BAL_CAT_ENTERTAINMENT_NEWS_LOCAL,
    BAL_CAT_ENTERTAINMENT_NEWS_REGIONAL,
    BAL_CAT_ENTERTAINMENT_NEWS_NATIONAL,
    BAL_CAT_ENTERTAINMENT_NEWS_INTERNATIONAL,
    BAL_CAT_LOCAL_WEATHER,
    BAL_CAT_TRAFFIC,
    BAL_CAT_FLIGHT_SCHEDULE,
    BAL_CAT_RESTAURANTS,
    BAL_CAT_LODGINGS,
    BAL_CAT_RETAIL_DIRECTORY,
    BAL_CAT_ADVERTISEMENT,
    BAL_CAT_STOCK_QUOTES,
    BAL_CAT_EMPLOYMENT_OPPORTUNITIES,
    BAL_CAT_MEDICAL_HEALTH,
    BAL_CAT_TECH_NEWS,
    BAL_CAT_MULTI_CATEGORY
 } BAL_SMS_SERVICE_CAT;  /* ??? change to conformant typedef */
 
 typedef PACKED struct
 {
    uint8  OperationCode;
    uint16 SrvCat;
    uint8  Lang;
    uint8  MaxMsg;
    uint8  Alert;
    uint8  NumChar;
    uint8  Char[ SMS_MAX_CHAR_CATEGORY ]; 
 } BAL_SRV_CAT_RECORD;
 
 typedef PACKED struct
 {
    uint8 msgEncoding;
    uint8 numCat;
    BAL_SRV_CAT_RECORD cat[SMS_MAX_NUM_SERVICE_CATEGORIES];
 } BAL_SRV_CAT_DATA;
 
 typedef PACKED struct
    {
       uint8 ErrorClass;
       uint8 MsgStatusCode;
    } BAL_MSG_STATUS;
 
 typedef PACKED struct
 {
    uint16            SeqNum;    
    uint16            TeleSrvId;
    uint8             NumUserDataMsgs;
    uint16            MsgId;
    uint8             Type;
    bool              HeaderInd;
    bool              UserAckPres;
    bool              UserAck;
    BalSmsMCAddrT     Addr;
    bool              SubaddrPres;
    BalSmsMCSubaddrT  Subaddr;
    bool              CatPres;
    BAL_SMS_SERVICE_CAT  Cat;
    bool              UsrRspCodePres;
    uint8             UsrRspCode;
    bool              TimeStampPres;
    BalSmsAbsTimeT    TimeStamp;
    bool              BalTimeAbsPres;
    BalSmsAbsTimeT    BalTimeAbs;
    bool              BalTimeRelPres;
    uint8             BalTimeRel;
    bool              DelTimeAbsPres;
    BalSmsAbsTimeT    DelTimeAbs;
    bool              DelTimeRelPres;
    uint8             DelTimeRel;
    bool              PriPres;
    uint8             Pri;
    bool              PrvPres;
    uint8             Prv;
    bool              ReplyOptionPres;
    bool              UserAckReq;
    bool              DakReq;
    bool              ReadAckReq;
    bool              NumMsgPres;
    uint8             NumMsg;
    bool              AlertPres;
    uint8             Alert;
    bool              LangPres;
    uint8             Lang;
    bool              CallbackPres;
    BalSmsMCAddrT     Callback;
    bool              DispModePres;
    BAL_SMS_DISPMODE  DispMode;
    bool              MsgDepIndexPres;
    uint16            MsgDepIndex;
    bool              srvCatDataPres;
    BAL_SRV_CAT_DATA  srvCatData;
	bool              msgStatusPres;
   BAL_MSG_STATUS msgStatus;
    bool              transport_ack_reqd;
 } BalSmsDeliverIndMsgT;
 
 /* ??? associated to what sms message */
typedef PACKED struct
{
  uint16           TeleSrvId;
  uint16           MsgId;
  BalSmsCauseCodeT Error;
} BalSmsErrorMsgT;
 
typedef PACKED struct
{
  uint8 SeqNum; 
} BalSmsAckL2AckedMsgT;

typedef PACKED struct
{
  uint8 MsgEncoding;    /* TSB58A defined representation of Char fields     */
  uint8 NumFields;      /* the number of Chars present (in the array below) */
  uint8 AsciiText[225]; /* the decoded user data text                       */
} BalSmsTstDecDataMsgT;
 
 /*-----------------------------------------------------------------
 * HWD Handler
 *----------------------------------------------------------------*/
/* BAL_HWD_BATTERY_READ_DATA_MSG */
typedef PACKED struct 
{
   uint16   AuxAdcValue;
   uint16   BatterVoltage;  /* in mV */
} BalBatteryReadDataMsgT;

/* BAL_HWD_TEMPERATURE_READ_DATA_MSG */
typedef PACKED struct 
{
   uint16   AuxAdcValue;
   int8     TempCelcius;  /* in C */
} BalTempInfoDataMsgT;

/* BAL_HWD_RX_TX_POWER_INFO_DATA_MSG */
typedef PACKED struct 
{
   int16    RxPowerDbmQ6;
   int16    TxPowerDbmQ6;
   uint16   RxGainState;
   uint16   TxGainState;
} BalRxTxPwrInfoMsgT;

/* BAL_PSW_PILOT_PWR_RPT_MSG */
typedef PACKED struct
{
   int16    AvgActiveEcIoQ6;
} BalPswPilotPwrRptMsgT;

/* BAL_MISC_RX_TX_PWR_INFO_EVT - this typedef is for UI callback only */
typedef PACKED struct
{
   int16    RxPowerDbmQ6;
   int16    TxPowerDbmQ6;
   uint16   RxGainState;
   uint16   TxGainState;
   int16    AvgActiveEcIoQ6;
}BalUiRxTxPowerDataT;

/*-----------------------------------------------------------------
* VMEMO Handler
*----------------------------------------------------------------*/

/* common to data messages for VMEM and VREC */
typedef PACKED struct
{
   /*IpcSpchSrvcOptRateT   Rate;  Rate of speech */
   uint8   Rate;             /* Rate of speech */
   uint8   SpchData[1];      /* Speech data (size derived from rate) */
} BalVmemDataMsgT;

/* BAL_VMEM_DATA_OFFLINE_MSG */
typedef BalVmemDataMsgT BalVmemDataOfflineMsgT;

/* BAL_VMEM_DATA_ONLINE_MSG */
typedef BalVmemDataMsgT BalVmemDataOnlineMsgT;


typedef enum 
{
   BAL_VMEMO_TYPE_OFFLINE,
   BAL_VMEMO_TYPE_ONLINE
}BalVmemoTypeT;


/*-----------------------------------------------------------------
* VREC Handler (update with msgid associated with data structure)
*----------------------------------------------------------------*/

/* BAL_VREC_MIXED_DATA_MSG */
typedef BalVmemDataMsgT BalVrecMixedDataMsgT;

/* BAL_VREC_PCM_DATA_MSG */
typedef PACKED struct
{
   uint16 SpchData[160];      /* Speech data (size is fixed at 160) */
} BalVrecPcmDataMsgT;

/* BAL_VREC_TRAINING_START_MSG */
typedef PACKED struct
{
   uint8       KeyCount;      /* number of key press digits */
   uint8       KeyPress[31];  /* key press digits */
} BalVrecTrainingStartMsgT;

/* BAL_VREC_READ_VOCABULARY_MSG */
typedef PACKED struct
{
   ExeRspMsgT  RspInfo;    
} BalVrecReadVocabMsgT;

/* msg id is specified by BAL_VREC_READ_VOCABULARY_MSG, and it goes to ETS */ 
typedef PACKED struct
{
   uint8       Sector;
   uint8       Nametag;
   uint16      ValidKey;
   uint16      TemplateSize;
   uint16      CompSpchPackets;
   uint8       KeyPress[10];
} BalVrecReadVocabRspMsgT;

/* BAL_VREC_READ_TRAINCB_MSG */
typedef PACKED struct
{
   ExeRspMsgT  RspInfo;    
} BalVrecReadTrainCbMsgT;

/* msg id is specified by BAL_VREC_READ_TRAINCB_MSG, and it goes to ETS */ 
typedef PACKED struct
{
   uint16    NameTagId;          /* index of nametag                             */
   uint16    HeaderOffset;       /* offset to HeaderInfo in flash sector         */
   uint16    TemplateOffset;     /* offset to template data in flash sector      */
   uint16    CompSpchOffset;     /* offset to comp'd speech data in flash sector */
   
   uint16   *TemplatePtr;        /* pointer to nametag data in RAM               */
   uint16    TemplateSize;       /* size of nametag data                         */
   uint16    TemplateData[4];    /* first 4 words of nametag data                */
   
   uint16    CompSpchSize;       /* comp'd speech size in bytes                  */
   uint16    CompSpchPackets;    /* count of HeaderInfo's comp'd speech packets  */
} BalVrecReadTrainCbRspMsgT;

/* BAL_VREC_TEST_FUNCTION_MSG */
typedef enum
{
   BAL_VREC_TEST_FUNC1 = 0x01,
   BAL_VREC_TEST_FUNC2 = 0x02,
   BAL_VREC_TEST_FUNC3 = 0x04,
   BAL_VREC_TEST_FUNC4 = 0x08,
   BAL_VREC_TEST_ALL   = 0xFF
} VrecTestFunctionT;

typedef PACKED struct
{
   ExeRspMsgT        RspInfo;    
   VrecTestFunctionT  FuncNum;
} BalVrecTestFunctionMsgT;

/* msg id is specified by BAL_VREC_TEST_FUNCTION_MSG, and it goes to ETS */ 
typedef PACKED struct
{
   bool              Result;
} BalVrecTestFunctionRspMsgT;
 
 /*-----------------------------------------------------------------
    * MULTIMEDIA APPLICATION Handler
    *----------------------------------------------------------------*/

/* Status & Error codes used for BAL Apps callback functions (Music, JPEG) */
typedef enum
{
   BAL_APP_OK = 0,
   BAL_APP_FILE_FORMAT_INVALID,
   BAL_APP_FILE_IO_ERROR,
   BAL_APP_FAILED_ERROR,
   BAL_APP_WRONG_PLAY_FORMAT,
   BAL_APP_CONFLICT,
   BAL_APP_MALLOC_ERROR,
   BAL_APP_DATA_SIZE_INVALID,
   BAL_APP_TONE_ID_INVALID,

   BAL_APP_IMAGE_TYPE_INVALID,
   BAL_APP_IMAGE_CONTEXT_NOT_INIT,
   BAL_APP_IMAGE_WIDTH_INVALID,
   BAL_APP_IMAGE_ACTUAL_SIZE_UNKNOWN,

   BAL_APP_VMEMO_WRONG_STATE_ERR,
   BAL_APP_VMEMO_INVALID_REC_TYPE,
   BAL_APP_VMEMO_RECORD_DEVICE_FULL,
   BAL_APP_VMEMO_INVALID_NUM_PKTS,
   BAL_APP_VMEMO_INVALID_SRVC_OPT,
   BAL_APP_VMEMO_SRVC_OPT_MISMATCH,
   BAL_APP_VMEMO_WRONG_REC_WHILE_ON_TRAFFIC,
   BAL_APP_VMEMO_INTERNAL_ERR
} BalAppStatusT;

typedef PACKED struct
{
    /* Data transfer option:
     *    Data is located either in a memory buffer (DataP) or in an open file.
     *    If DataP is not NULL, use the buffer; otherwise use the FileHandle.
     */
    void          *DataP;       /* Ptr to data buffer; if NULL, use FileHandle */
    BalFsiHandleT  FileHandle;  /* File handle of open file; used only when DataP NULL */
} BalDataLocT;

/*-----------------------------------------------------------------
* MISC Handler
*----------------------------------------------------------------*/

/* BAL_L1DTST_GETPHONESTATUS; rspmsg data structure defined by L1D */
typedef ExeRspMsgT BalL1DTstGetPhoneStatusMsgT;


/* BAL_L1D_RSSI_RPT_MSG */
typedef PACKED struct
{
   int16    Rssi;
} BalL1dRssiRptMsgT;

/* typedef for expressing time in user format */
typedef struct 
{
   uint8  Secs, Mins, Hours;     /* 00:00:00 to 23:59:59 */
   uint8  Day, Month;            /* 1...31, 1...12 */
   uint8  DoW;                   /* 0 (Sun)...6 (Sat) */
   uint16 Year;                  /* 1980...2043 */
} BalCalendarTimeT;

/*-----------------------------------------------------------------
 * ETS Handler
 *----------------------------------------------------------------*/
 
/* BAL_SEND_DATA_BURST_MSG */
/* ??? how about going to a header + data dual structure */
#define BAL_CP_MAX_DATA_BURST_CHAR 249 /* 255 - Data Burst Header */
typedef PACKED struct
{
  uint8 MsgNumber;
  uint8 BurstType;
  uint8 NumMsgs;
  uint8 NumFields;
  uint8 Data[249];  /* ??? */
  bool  Encoded;
  bool  DigitMode;
  uint8 NumDigits;
  uint8 Digits[30]; /* ??? */
} BalSendDataBurstMsgT;
  
typedef PACKED struct
{
  bool  Mode; /* TRUE: disabled */
} BalKPadDisabledMsgT;
 
/* BAL_EMERGENCY_MODE_TEST_MSG */
typedef PACKED struct
{
  ExeRspMsgT  RspInfo;
  bool        value;   
} BalEmergencyModeTestingMsgT;
 
typedef PACKED struct
{
  bool  mode;   
} BalEmergencyModeTestingResponseMsgT;
 
typedef enum
{
  BAL_DEV_NONE        = 0x0000,
  BAL_DEV_UIM         = 0x1000,
  BAL_DEV_RAM         = 0x2000,
  BAL_DEV_FLASH       = 0x3000,
  BAL_DEV_UIM_FLASH   = 0x4000
} BalDeviceT;
 
/* index used to access the CallTxtTable[] */
typedef enum {
   BAL_CT_DEFAULT,
   BAL_CT_AMPS_CALL,
   BAL_CT_DATA_CALL,
   BAL_CT_FAX_CALL,
   BAL_CT_SSO1,
   BAL_CT_SSO2,
   BAL_CT_SSO3,
   BAL_CT_SSO6,
   BAL_CT_SSO9,
   BAL_CT_SSO14,
   BAL_CT_SSO17,
   BAL_CT_SSO32768,
   BAL_CT_SSO18,
   BAL_CT_SSO19,
   BAL_CT_SSO32,
   BAL_CT_SSO33,
   BAL_CT_SSO35,
   BAL_CT_SSO36,
   BAL_CT_SSO54,
   BAL_CT_SSO55,
   BAL_CT_SSO68,
   BAL_CT_SSO32798,
   BAL_CT_SSO32799,
   BAL_CT_SSO32858,
   BAL_CT_SSO32859,
   BAL_CT_MAX
} BalCallTypeT;

/* powerdown sequence signals */
typedef enum
{
  BAL_PWRDWN_START,
  BAL_PWRDWN_PSDOWN,
  BAL_PWRDWN_DBMFLUSH,
  BAL_PWRDWN_AIWFLUSH
} BalPowerDownIdT;

typedef enum
{
  BAL_MISC_PWRDWN_FLUSH_EVT,
  BAL_MISC_BATTERY_READING_EVT, 
  BAL_MISC_TEMP_READING_EVT,
  BAL_MISC_RX_TX_PWR_INFO_EVT,
  BAL_MISC_REFURBISH_AGING_TX_OFF_IND,
  BAL_MISC_REFURBISH_AGING_TX_ON_IND
} BalMiscEventIdT;

/* cmd id for ValUserInfoControl() */
typedef enum
{
  BAL_USERINFO_INIT_CMD	 =	0x01,
  BAL_USERINFO_RESTART_CMD,
  BAL_USERINFO_STOP_CMD
} BalUserInfoCmdT;

typedef enum
{
   BAL_SO_ACCEPT = 0,
   BAL_SO_REJECT
} BalPswSoActionT;

/* message type for BAL_SET_DEFAULT_SO_MSG */
typedef struct
{
   BalPswSoActionT  Action;
   PswServiceOptionT ServiceOption;
   PswServiceT ServiceType;
} BalSetDefaultSOMsgT;

typedef struct
{
  bool  Mode;
} BalSetPrivacyModeMsgT;

#ifdef SYS_OPTION_VOICE_RECOGNITION
typedef enum
{
  BAL_VREC_TRAINING_START_EVT,
  BAL_VREC_TRAINING_COMPLETE_EVT,
  BAL_VREC_SPEECH_PLAYBACK_EVT,
  BAL_VREC_START_RECOGNITION_EVT,
  BAL_VREC_NOT_RECOGNIZED_EVT,
  BAL_VREC_RECOGNIZED_EVT,
  BAL_VERC_CANCEL_EVT,
  BAL_VREC_CAPTURE_CONFIRM_EVT,
  BAL_VREC_CAPTURE_PLAYBACK_EVT,
  BAL_VREC_CMD_DONE_EVT,
  BAL_VREC_TRAIN_CANCEL_EVT,
  BAL_VREC_CANCEL_EVT,
  BAL_VREC_TEMPLATE_COMPLETE_EVT
} BalVrecEventT;
#endif

typedef enum 
{
   BAL_RF_PCS_BAND = HWD_RF_PLL_BAND_PCS_1900,
   BAL_RF_AMPS_BAND = HWD_RF_PLL_BAND_AMPS,
   BAL_RF_CELL_BAND = HWD_RF_PLL_BAND_CDMA_CELLULAR,
   BAL_RF_T53_BAND = HWD_RF_PLL_BAND_CDMA_T53,
   BAL_RF_KPCS_BAND = HWD_RF_PLL_BAND_KOREAN_PCS,
   BAL_RF_UNDEF_BAND = HWD_RF_PLL_BAND_UNDEFINED      
}BalRfPllBandT;

/* Define BAL Display Device Info msg */
typedef PACKED struct
{
   ExeRspMsgT     RspInfo;
} BalDispDeviceInfoMsgT;

/* Define BAL Display Device response Info msg */
#define BAL_DISP_MAX_DEVICE_INFO_LEN  20
typedef PACKED struct
{
   char        Vendor [BAL_DISP_MAX_DEVICE_INFO_LEN];
   char        ModelId [BAL_DISP_MAX_DEVICE_INFO_LEN];
   uint16      BitsPerPixel;
   uint16      WidthInPixels;
   uint16      HeightInPixels;
} BalDispDeviceInfoRspMsgT;

 /* BAL Event typedefs */
typedef int16 RegIdT;
typedef void  (*BalEventFunc)  ( RegIdT RegId,
                                 uint32 MsgId, 
                                 void*  MsgBufferP );

 typedef PACKED struct
 {
    int8 RecId;
 }BalDnsQueryMsgT;

 typedef PACKED struct
 {
   uint8 DNSEvent;
   int16 DNSParam;
 }BalDnsNetMsgT;
 
 /* Structure for registering events */
typedef struct
{
  bool         IsUse;    /* if this entry is in use */
  BalEventFunc CallBack; /* function to call back */  
} BalRegTableT;
 
/* Event handler */
typedef struct
{
  BalRegTableT*  RegTableP; /* Register table pointer */
  ExeSemaphoreT* SemaphoreP; 
  int16          MaxRegIds; /* Maximum RegId count Register table supported */
} BalEventHandlerT;
typedef PACKED struct
{
   bool	Enabled;	 
}BalRptSilentRetryStatusMsgT;

 /*------------------------------------------------------------------------
 * Global function prototypes
 *------------------------------------------------------------------------*/
 
#ifdef __cplusplus
extern "C" {
#endif 

//xp void   BalGetcpStatus( void );
bool   BalStatusGet( void );
void   BalGetDebugInfo( void );

/*-----------------------------------------------------------------
 *	balmisc.c interface
 *----------------------------------------------------------------*/
//xp void   BalMiscLocSessionStart( void );
void   BalMiscPowerDown( BalPowerDownIdT Signal );
RegIdT BalMiscRegister( BalEventFunc CallBack );
void   BalMiscUnregister( RegIdT RegId );
void   BalMiscNamRequest( void );
int8 BalGetDaysInSpecifiedMonthYear( uint16 Year, uint8 Month );

//xp bool   BalAmpsModeIsActive( void );

//xp void   BalUserInfoControl( BalUserInfoCmdT CmdId );

//xp RegIdT BalVrecRegister( BalEventFunc EventFuncP );

//xp bool BalGetEmergencyModeTestingMsg( void );

//xp void   BalStorageDataInit(void);
/*-----------------------------------------------------------------
 *	Balvmemo.c interface
 *----------------------------------------------------------------*/
//void BalVoiceMemoExec( uint32 MsgId, void* MsgDataP, uint32 MsgSize );

/*-----------------------------------------------------------------
 *	Bal interfaces to HWD (valmisc.c)
 *----------------------------------------------------------------*/
void BalPwrOffBoard( void );

/*****************************************************************************
  FUNCTION NAME:    BalMalloc 

  DESCRIPTION:	    This routine allocates the buffer from the Bal pool

  PARAMETERS:	    size

  RETURNED VALUES:  ptr to block of memory
*****************************************************************************/
void* BalMalloc( uint32 size );
void BalFree(void* Ptr);
uint32 BalGetAvailableSpace(void);

/*****************************************************************************

FUNCTION NAME:  BalSetKeypadLock

DESCRIPTION:    It is used to set kaypad lock state

PARAMETERS:     BOOL

RETURNS:        NONE
*****************************************************************************/
void BalSetKeypadLock(bool isLock, bool isTurnOnLight);

/*****************************************************************************

FUNCTION NAME:  BalGetKeypadLock

DESCRIPTION:    It is used to get kaypad lock state

PARAMETERS:     NONE

RETURNS:        TRUE:
                FALSE:
*****************************************************************************/
bool BalGetKeypadLock();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BALAPI_H */


