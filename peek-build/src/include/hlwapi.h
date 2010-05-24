/*****************************************************************************

 *****************************************************************************/   

#ifndef _HLWAPI_H_
 #define _HLWAPI_H_
 /*****************************************************************************
 * 
  *****************************************************************************/
 
 /*----------------------------------------------------------------------------
  Include Files
 ----------------------------------------------------------------------------*/
 #include "sysdefs.h"
 #include "exeapi.h"
 #include "pswcustom.h"
 
 /*----------------------------------------------------------------------------
  Global Defines and Macros
 ----------------------------------------------------------------------------*/
 #define HLW_MAILBOX_CMD          EXE_MAILBOX_1_ID
#ifdef SYS_OPTION_MIP_DMU
#define HLW_MAILBOX2_CMD         EXE_MAILBOX_2_ID
#endif
#define HLW_STARTUP_SIGNAL       EXE_SIGNAL_1
#define HLW_MAX_SIZE_DATA        240
#define HLW_MAX_USRID_LEN        24
#define HLW_MAX_PSWD_LEN         24
#define HLW_MAX_BWSR_DIG_LEN     (CP_MAX_CALLING_PARTY_NUMBER_SIZE +1) 
#define HLW_MAX_SPY_LEN          64
#define HLW_MAX_SOCKET           10

 #if 0
 #define TCPB_TEST 
 #endif
      
 
 /***********************************************************************/
 /* Global Typedefs                                                     */
 /**********************************************************************/
 typedef enum
 {
   HLW_RLP_OPEN_ACK_MSG,       /* alias: is7074RlpConnectStatus        */                             
   HLW_RLP_CLOSE_ACK_MSG,      /* alias: is7074RlpConnectionClosed     */                             
    HLW_RLP_RESET_IND_MSG,      /* alias: is7074RlpResetCompleteInd     */
   HLW_RLP_FWD_DATA_MSG,	    /* alias: is7074RlpHDLCPacketRxd        */
    HLW_RLP_REV_DATA_RSP_MSG,   /* alias: is7074RlpTxComplete           */  
    HLW_RLP_IDLE_IND_MSG,       /* alias: is7074RlpInactivityInd        */                           
    HLW_UPB_CONN_STATUS_MSG,    /* alias: is7074AppIntConnectStatus     */
    HLW_UPB_CONN_REQ_MSG,       /* alias: is7074BrowserBeginConnection  */
    HLW_UPB_END_MSG,            /* alias: is7074BrowserEndConnection    */
    HLW_UPB_SEND_DATA_MSG,      /* alias: is7074BrowserSendDatagram     */
    HLW_UPB_RECV_DATA_RSP_MSG,  /* alias: is7074BrowserRxRsp            */
    HLW_AIW_BEGIN_UDP_MSG,       /* alias: is7074AppIntBeginUDPConnection*/
    HLW_AIW_CONN_STATUS_MSG,    /* alias: is7074AppIntConnectStatus     */
    HLW_AIW_INACT_TMO_REQ_MSG,  /* alias: is7074AppIntInactivityTmoReq  */
    HLW_AIW_CONN_DROP_MSG,      /* alias: is7074AppIntConnectionDropped */
   HLW_AIW_HDLC_RX_MSG,        /* alias: is7074AppIntHDLCPacketRxd     */
    HLW_AIW_TX_RSP_MSG,         /* alias: is7074AppIntSendDataRsp       */
    HLW_AIW_BEGIN_TCP_MSG,      /* alias: is7074AppIntBeginTCPConnection*/  
    HLW_AIW_END_CONN_MSG,       /* alias: is7074AppIntEndConnection     */
    HLW_AIW_DISCONN_IND_MSG,    /* alias: is7074AppIntDisconnectInd     */
    HLW_AIW_RX_RSP_MSG,         /* alias: is7074AppIntRxRsp             */
    HLW_AIW_TX_REQ_MSG,         /* alias: is7074AppIntTxReq             */
    HLW_RM_PPP_BEGIN_MSG,       /* alias: is7074RmPppConnect            */
    HLW_RM_DISCONNECT_MSG,      /* alias: is7074RmDisconnectInd         */
    HLW_RM_CONNECT_STATUS_MSG,  /* alias: is7074RmConnectStatus         */
    HLW_RM_TX_DATA_RSP_MSG,     /* alias: is7074UartTxComplete          */
    HLW_UM_CONNECT_STATUS_MSG,  /* alias: is7074UmConnectStatus         */
    HLW_UM_DISCONN_MSG,         /* alias: is7074UmDisconnectInd         */
    HLW_UM_PPP_BEGIN_MSG,       /* alias: is7074UmPppConnectReq         */
    HLW_UM_RX_DATA_REQ_MSG,     /* alias: is7074UmRecvData              */
    HLW_UM_TX_DATA_MSG,         /* alias: is7074UmSendData              */
    HLW_UM_RESTART_RLP_MSG,     /* alias: is7074RestartRlp              */
    HLW_UM_PKT_ZONE_IND_MSG,    /* Packet zone ID change notification   */
    HLW_UM_ABORT_REQ_MSG,       /* To reset PPP for nspe.rlpApp         */
    HLW_TIMER_EXPIRED_MSG,      /* alias: is7074TimerExpired            */
 
    HLW_TCPB_CONN_REQ_MSG,      /* alias: is7074TcpbBeginConnection     */
    HLW_TCPB_CONN_END_MSG,      /* alias: is7074TcpbEndConnection       */
    HLW_TCPB_DATA_SEND_REQ_MSG, /* alias: is7074TcpbSendDataReq         */
    HLW_TCPB_DATA_RECV_RSP_MSG, /* alias: is7074TcpbRecvDataRsp         */
 
   HLW_BAL_INACT_TMO_REQ_MSG,  /* alias: is7074AppIntInactivityTmoReq  */
   HLW_BAL_PPP_CONNECT_MSG,    /* start up PPP at Um for all sockets 
                                  meant to be used by MMI APPs.        */
   HLW_BAL_PPP_CLOSE_MSG,      /* close PPP at Um instructed by MMI APP*/
   HLW_BAL_SOCKET_CREATE_MSG,  /* create a TCP/UDP SAP for client.     */
   HLW_BAL_SOCKET_BIND_MSG,    /* bind address/port to a SAP that's
                                  created by a socket                  */
   HLW_BAL_SOCKET_CONNECT_MSG, /* connect a created socket             */
   HLW_BAL_SOCKET_CLOSE_MSG,   /* close a connected socket             */
   HLW_BAL_DORMANT_REQ_MSG,    /* alias: is7074BrowserDormantReq       */
 
    HLW_IOP_DATA_SEND_REQ_MSG,  /* test:  is7074TcpbSendDataReq         */
    HLW_IOP_DATA_RECV_RSP_MSG,  /* test:  is7074TcpbRecvDataRsp         */
    HLW_USERNAME_PASSWD_MSG,    /* To acquire username/password         */
    HLW_UART_READY_MSG,         /* alias: is7074UartPortOpenStatus      */
 
    HLW_MIP_RRP_MSG,            /* alias to MIP is7074 calls            */
    HLW_MIP_AGENT_ADV_MSG,      /* alias to MIP is7074 calls            */
    HLW_MIP_UM_PPP_STATUS_MSG,  /* alias to MIP is7074 calls            */
    HLW_MIP_TIMER_CALL_BACK_MSG,/* alias to MIP is7074 calls            */
 
    /* -----------------------ETS test messages-------------------------*/
    HLW_TEST_BROWSER_CONNECT_MSG,/* Test message for browser connect    */
    HLW_TEST_PPP_OPEN_MSG,       /* Test message for PPP open.          */
    HLW_TEST_PPP_CLOSE_MSG,      /* Test message for PPP close.         */
    HLW_TEST_SOCKET_CREATE_MSG,  /* Test message for socket creation    */   
    HLW_TEST_SOCKET_BIND_MSG,    /* Test message for binding socket     */
    HLW_TEST_SOCKET_CONNECT_MSG, /* Test message for socket connection  */
    HLW_TEST_SOCKET_CLOSE_MSG,   /* Test message for socket closure     */

#ifdef SYS_OPTION_MIP_DMU
   HLW_DBM_DATA_DMUPUBKEY_MSG,
   HLW_DBM_DATA_DMUPUBKEYORGID_MSG,
   HLW_AIW_DMUV_SET_MSG,
   HLW_RESET_MN_AUTH_MSG,
   HLW_PSWS_DMU_KEYGEN_RSP_MSG,
   HLW_PSWS_DMU_KEYENC_RSP_MSG,
   HLW_MN_AUTHENTICATOR_SAVE_RSP_MSG,
#endif
    HLW_BAL_SOCKET_LISTEN_MSG,
    HLW_BAL_SOCKET_OPTION_MSG,
    HLW_BAL_SOCKET_SHUTDOWN_MSG,
    HLW_BAL_SOCKET_LINGER_MSG,
    HLW_NUM_MSG_IDS
 } HlwMsgIdT;
 
typedef enum
{
   HLW_PRIMARY_CHANNEL = 0,       /* Do not change this.               */
   HLW_SECONDARY_CHANNEL
}  HlwChannelSelT;

typedef enum
{
   HLW_CTRL_PORT,				 /* Messages coming from control port */
   HLW_DATA_PORT                  /* Messages coming from data port    */
} HlwPortT;
 
 typedef PACKED struct
 {
    uint8	         Data[HLW_MAX_SIZE_DATA];     /* Data array        */
 } HlwDataT;
 
 typedef bool    HlwSoStatusT; 
 typedef uint16  HlwSoT; 
 typedef uint8   HlwMuxOptT; 
 
 typedef enum 
 { 
    HLW_PRIMARY = 0, 
    HLW_SECONDARY 
 } HlwTrafficT; 
 
 typedef PACKED struct
 {
   uint8*             DataBufP;     /* Point to the first byte          */
   uint16             DataLen;      /* size of data in byte             */
   uint8              Flags;        /* if IP_MOREDATA is set in the flag
                                       it indicates that datagram is not
 				                      able to fit into allocated buffer
 				                      Datagram is truncated to fit.    */
 } HlwDatagramT;
 
 typedef PACKED struct
 {
   uint32             IpAddr;       /* 32-bit IP address.               */
   uint16             UdpPort;      /* UDP port address.                */
 } HlwDatagramAddrT;
 
 typedef enum
 {
    HLW_SINGLE_STACK,               /* single stack. */
    HLW_BWSR_ON_ASYNC               /* double stack. */
 } HlwBwsrT;
 
 typedef enum
 {
   HLW_RELEASE_NORMAL = 0,
   HLW_RELEASE_FADE   = 1
 } HlwReleaseT;



 /***********************************************************************/
 /* Message Definitions                                                 */
 /***********************************************************************/

typedef PACKED struct
{ 
   HlwChannelSelT     Channel;       /* Primary or Secondary Channel.   */ 
   bool               Success;       /* succeed or fail.                */
} HlwRlpOpenAckMsgT;

typedef PACKED struct
{
   HlwChannelSelT     Channel;       /* Primary or Secondary Channel.   */                                                      
} HlwRlpCloseAckMsgT;
 
typedef PACKED struct
{ 
   HlwChannelSelT     Channel;       /* Primary or Secondary Channel.   */ 
   bool               Status;        /* RLP Open Response status.       */
} HlwRlpResetIndMsgT;
 /* HLW_RLP_REV_DATA_RSP_MSG */
 
typedef PACKED struct
{
   HlwChannelSelT   Channel;        /* Primary or Secondary Channel.  */
   uint8*           DataBufP;       /* Pointer to data buffer         */
   uint16           DataLen;        /* Length of data in bytes        */
   uint8            DataLost;       /* Data lost in bytes             */
} HlwRlpFwdDataMsgT;
typedef PACKED struct 
{
   HlwChannelSelT    Channel;       /* Primary or Secondary Channel.  */
} HlwRlpRevDataRspMsgT;
                        
                         
 typedef PACKED struct
 {
    HlwChannelSelT    Channel;       /* Primary or Secondary Channel.  */
 } HlwRlpIdleIndMsgT;
 
 typedef PACKED struct
 { 
   uint8              Result;     /* to indicate whether the underlying 
                                     connection is successful or not. The
                                     result code is to be determined     */
 } HlwUpbConnStatusMsgT;
 
 typedef PACKED struct
 { 
    HlwBwsrT         Mode;           /* 0=single stack, 1=double stack. */
    uint16           ServiceOption;
    uint8            UserId[HLW_MAX_USRID_LEN];/* ASCII chars, NULL terminated*/
    uint8            Pswd[HLW_MAX_PSWD_LEN];   /* ASCII chars, NULL terminated*/
    uint8            CallNumber[HLW_MAX_BWSR_DIG_LEN]; /* ASCII chars, NULL       
                                                          terminated */
 } HlwUpbConnReqMsgT;
 
 typedef PACKED struct
 { 
    HlwDatagramT      SendData;    /* Datagram itself..                  */
    HlwDatagramAddrT  To;          /* The address of receipent.          */
    uint8             Sap;         /* SAP to TCB                         */
 } HlwUpbSendDataMsgT;
 
 typedef PACKED struct
 { 
   uint8              Result;     /* to indicate whether the underlying 
                                     connection is successful or not. The
                                     result code is to be determined     */
 } HlwAiwConnStatusMsgT;
 
 typedef PACKED struct
 {
    HlwChannelSelT    Channel;       /* Primary or Secondary Channel.  */
    uint16            TimeOut;       /* 0: disabled, else: in second.  */   
 } HlwAiwInactTmoReqMsgT;
 
 typedef PACKED struct
 { 
    uint8             Reason;     /* to indicate the underlying connection
                                     is dropped due to a reason, which 
 									code is to be dertermined.          */
 } HlwAiwConnDropMsgT;
 
typedef PACKED struct
{ 
   uint8*            DataBufP;   /* Point to the first byte of HDLC.    */
   uint16            DataLen;    /* the size of HDLC packet in byte.    */
   bool              NtwkPkt;    /* Network Rm Packet or not */
} HlwAiwHdlcRxMsgT;
 
 typedef PACKED struct
 {
    bool              NtwkPkt;    /* Network Rm Packet or not */
 } HlwAiwRxRspMsgT;
 
 typedef PACKED struct
 {
    HlwChannelSelT    Channel;    /* Primary or Secondary Channel.       */
 } HlwAiwTxRspMsgT;
 
 typedef PACKED struct
 { 
    bool              Passive;    /* TRUE, a listen; FALSE, a connect.   */
    bool              IpMode;     /* TRUE, low delay TOS; FALSE, a high 
                                     throughput TOS                      */
    bool              RlpMode;    /* TRUE, transparent mode; FALSE, non-
                                     transparent mode.                   */
    bool              SecondaryTraffic;  /* FALSE if primary channel.    */                                                                           
 } HlwAiwBeginTcpMsgT;
 
 typedef PACKED struct
 { 
    bool              Graceful;   /* TRUE, FIN to close TCP; FALSE, RST
                                     to close TCP.                       */
 } HlwAiwEndConnMsgT;
 
 typedef PACKED struct
 { 
    uint8*            DataBufP;   /* point to the first byte of data.    */
    uint16            DataLen;    /* The size of data in byte.           */
    bool              Push;       /* TRUE, send immediately; FALSE, TCP
                                     determine by itself.                */
 } HlwAiwTxReqMsgT;
 
 typedef PACKED struct
 { 
    uint32            LocalIp;    /* the local IP address.               */
    uint32            RemoteIp;   /* the remote IP address.              */
 } HlwRmPppBeginMsgT;
 
 typedef PACKED struct
 {
    uint8             status;     /* See _BrowserStatus in USER_API.H    */
 } HlwRmConnectStatusMsgT;
 
 typedef PACKED struct
 {
    uint8             status;     /* See _BrowserStatus in USER_API.H    */
 } HlwUmConnectStatusMsgT;
 
 typedef PACKED struct
 {
    bool              UseMnrp;	 /* TRUE:use MNRP (not supported)       */ 
    bool              UseRlp;     /* TRUE/FALSE :single/double stack bwsr*/
    bool              RlpChannel; /* Secondary channel?                  */
    uint8             IdleTmo;    /* Inactivity timer                    */
    uint32            LocalIpAddr;/* Local 32-bit IP address.            */
    bool              pppOnly;    /* TRUE=only PPP (for browser)         */
 } HlwUmPppBeginMsgT;
 
 typedef PACKED struct
 { 
    uint8*            DataBufP;   /* Points to the first byte of data    */
    uint16            DataLen;    /* The size of data in byte.           */
    uint16            ProtNum;	 /* Protocol number.                    */
 } HlwUmTxDataMsgT;
 
 typedef PACKED struct
 { 
    uint16            TimerID;    /* The timer ID which was just expired */
 } HlwTimerExpiredMsgT;
 
 typedef PACKED struct
 {
    uint8            BrwsrType;  /* 2=UP-Browser, 4=Browser over Async, 5=TCP Browser */
    bool             Passive;    /* TRUE, a listen; FALSE, a connect.    */
    bool             IpMode;     /* TRUE, low delay TOS; FALSE, a high  
                                    throughput TOS                       */
    bool             RlpMode;    /* TRUE, transparent mode; FALSE, non-
                                    transparent mode.                    */
    bool             SecondaryTraffic;  /* FALSE if primary channel.     */
    uint32           DestIPAddress;  /* IP address for dest.             */
    uint16           DestPortNumber; /* Port number for dest.            */
    uint16           ServiceOption;
    uint8            UserStrings[HLW_MAX_USRID_LEN+
                                 HLW_MAX_PSWD_LEN+
                                 HLW_MAX_BWSR_DIG_LEN];                                                
 } HlwTestBrowserConnectMsgT;
 
 typedef PACKED struct
 {
    uint16           ServiceOption;
    uint8            UserStrings[HLW_MAX_USRID_LEN+
                                 HLW_MAX_PSWD_LEN+
                                 HLW_MAX_BWSR_DIG_LEN];                                                
 } HlwTestPppOpenMsgT;
 
 typedef PACKED struct
 {
    uint32           IpAddr;    /* Net-endian IP address       */
    uint16           Port;      /* Net-endian application port */
 } HlwAppAddrT;
 
 
 typedef PACKED struct
 { 
    uint8            UserId[HLW_MAX_USRID_LEN];
    uint8            Pswd[HLW_MAX_PSWD_LEN];   
    uint8            CalledNumber[HLW_MAX_BWSR_DIG_LEN]; 
    HlwAppAddrT      DestAddr;
    bool             RlpChannel;
    uint16           ServiceOption;
    ExeRspMsgT       TcpbConnReqRspInfo;    
 } HlwTcpbConnReqMsgT;
 
 typedef enum
 {
    	HLW_TCPB_CONNECT_SUCCESS,         /* connection success       */
    	HLW_TCPB_CONNECT_FAIL_TCP,        /* connection fails on TCP  */
    	HLW_TCPB_CONNECT_FAIL_PPP,        /* connection fails on PPP  */
    	HLW_TCPB_CONNECT_FAIL_RLP,        /* connection fails on RLP  */
    	HLW_TCPB_CONNECT_FAIL_UART,       /* not use                  */
    	HLW_TCPB_DISCONNECT_NORMAL,       /* disconnect from BS       */
    	HLW_TCPB_DISCONNECT_MS,           /* disconnect from MS       */
    	HLW_TCPB_DISCONNECT_FADE,         /* disconnect due to fading */
    	HLW_TCPB_CONNECT_FAIL_TRAFFIC,    /* connection fails on traffic */
    	HLW_TCPB_CONNECT_FAIL_NOSVC,      /* connection fails on no src  */
    	HLW_TCPB_NUM_CONNECT_STATUS
 } HlwTcpbStatusT;
 
 typedef PACKED struct
 {  
    HlwTcpbStatusT  Status;   
 } HlwTcpbConnStatusMsgT;
 
 typedef PACKED struct
 {
    uint8    Sap;
    uint8	*DataP;
    uint16	Size;
 } HlwTcpbFwdDataMsgT;
 
 
 typedef PACKED struct
 {
    uint8    Sap;
 } HlwTcpbRecvRspMsgT;
 
 typedef PACKED struct
 { 
    bool             Graceful;
 } HlwTcpbConnEndMsgT;
 
 typedef PACKED struct
 {
    uint8    Sap;
 }HlwUpbRecvDataRspMsgT;
 
 typedef PACKED struct
 { 
    uint8*           DataP;   /* point to the first byte of data.    */
    uint16           Size;    /* The size of data in byte.           */
    bool             Push;    /* TRUE, send immediately;             */
                              /* FALSE, TCP determine by itself.     */
    uint8            Sap;     /* Socket of which the data is going to 
                                 be sent.                            */
 } HlwTcpbDataSendReqMsgT;
 
 typedef PACKED struct
 {
   uint16           ServiceOption;
   uint8            UserId[HLW_MAX_USRID_LEN];/* ASCII chars, NULL terminated*/
   uint8            Pswd[HLW_MAX_PSWD_LEN];   /* ASCII chars, NULL terminated*/
   uint8            CallNumber[HLW_MAX_BWSR_DIG_LEN]; /* ASCII chars, NULL terminated */       
   uint8            IdleTmo;                  /* Inactivity timer in second  */
   ExeRspMsgT       BalPPPConnReqRspInfo;    
} HlwValPppConnectMsgT;

typedef PACKED struct
{
   ExeRspMsgT       BalPPPCloseReqRspInfo;    
   bool             Graceful;   
} HlwValPppCloseMsgT;

typedef PACKED struct
{
   bool             TcpType;   /* TRUE=TCP, FALSE=UDP */
   int16            socketId;  /* input socket Id, will be used in the response message */
   ExeRspMsgT       BalSocketCreateRspInfo;      
   ExeRspMsgT       BalSocketConnRspInfo;      
   ExeRspMsgT       BalSocketCloseRspInfo;      
} HlwValSocketCreateMsgT;

typedef PACKED struct
{
   uint8            Sap;
   HlwAppAddrT      ResourceAddr;
   ExeRspMsgT       BalSocketBindRspInfo;      
} HlwValSocketBindMsgT;

typedef PACKED struct
{
   uint8            Sap;
   bool             Passive;  /* FALSE=active, TURE=listen                   */
   HlwAppAddrT      DestAddr; /* IP and port number if active mode is chosen */
} HlwValSocketConnectMsgT;

typedef PACKED struct
{
   uint8            Sap;
   bool             Graceful;
} HlwValSocketCloseMsgT;
typedef PACKED struct
{
   bool             Graceful;
} HlwTestPppCloseMsgT;

 typedef PACKED struct
 {
    bool             TcpType;   /* TRUE=TCP, FALSE=UDP */
    int16            socketId;  /* input socket Id, will be used in the response message */
 } HlwTestSocketCreateMsgT;
 
 typedef PACKED struct
 {
    uint8            Sap;
    bool             Graceful;
 } HlwTestSocketCloseMsgT;
 
 typedef PACKED struct
 {
    uint8            Sap;
 } HlwTestSocketBindMsgT;
 
 typedef PACKED struct
 {
    uint8            Sap;
    bool             Passive;
    uint32           ipAddr;
    uint16           Port;
 } HlwTestSocketConnMsgT;
 
 typedef PACKED struct
 {
   uint8             Sap;   /*server's sap*/
   int16 SockListenfd;
   int16 sockfd[5];
   uint8 SocketNum;
   ExeRspMsgT       BalSocketListenRspInfo;
 } HlwValSocketListenMsgT;

 typedef PACKED struct
 {
   uint8 Sap;
   UINT8 opt_type;           /* Specifies which option is being set */
   UINT16 size;
 }HlwValOptionRequestMsgT; 

 typedef PACKED struct
{
   uint8            Sap;
   int8             how;
} HlwValSocketShutDownMsgT;

typedef PACKED struct
{
  uint8  Sap;
  bool lingerOnOff;
  uint32 lingerTime;
}HlwValSocketLingerMsgT;
 typedef HlwAiwInactTmoReqMsgT HlwMmiInactTmoReqMsgT;
 
 typedef PACKED struct
 { 
    uint8*            DataBufP; /* points to the first byte of data */
    uint16            DataLen;  /* data size in bytes.              */   
 } HlwIopTxReqMsgT;
 
 typedef PACKED struct
 {
    HlwReleaseT       ReleaseReason;
 } HlwAiwDisconnectIndMsgT;
 
 typedef PACKED struct
 {
    uint8            UserId[HLW_MAX_USRID_LEN];
    uint8            Pswd[HLW_MAX_PSWD_LEN];   
 } HlwUsernamePasswdMsgT;
 
 typedef PACKED struct
 {
    uint32   SrcIp;
    uint32   DestIp;
    uint16   SrcPort;
    uint16   DestPort;
    uint16   DataLength; 
    uint8   *DataPtr;
 } HlwMipRrpMsgT;
 
 typedef PACKED struct
 {
    uint32   SrcIp;
    uint32   DestIp;
    uint16   DataLength; 
    uint8   *DataPtr;
 } HlwMipAgentAdvMsgT;
 
 typedef PACKED struct
 {
    uint8    Status;
    uint32   LocalIp;
    uint32   RemoteIp; 
 } HlwMipUmPppStatusMsgT;
 
 typedef PACKED struct
 {
    uint32   TimerId;
 } HlwMipTimerCallBackMsgT;
 
#ifdef SYS_OPTION_MIP_DMU
typedef PACKED struct 
{
   uint8     nDMUV;
} HlwAiwDMUVSetMsgT;
#endif


 

 #endif
 
 
 
 
