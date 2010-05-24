#ifndef _AIWAPI_H_
#define _AIWAPI_H_


/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/
#include "sysdefs.h"

/*----------------------------------------------------------------------------
 Global Defines and Macros
----------------------------------------------------------------------------*/
#define AIW_MAILBOX_CMD          EXE_MAILBOX_1_ID
 
#define AIW_MAX_SIZE_DATA        64
#define AIW_MAX_MON_SPY_LEN      64

#define AIW_MAX_CFG_STR_SIZE	 249 /* MAX_CFG_LEN in Ai_data.h */
#define AIW_MAX_BRSWR_DGT_SIZE   (32 +1)   
		                             /* NULL-terminated. */
#define AIW_MAX_FDL_STR_SIZE     95  /* Sect 4.4.3 IS-707-A.3 */
#define AIW_MAX_GCAP_STR_SIZE    64  /* Ai_cmd is using 65535 bytes!! */
#define AIW_MAX_FLI_STR_SIZE	 21  /* MAX_FLI_LEN in Ai_data.h */ 
#define AIW_MAX_FPA_STR_SIZE	 21  /* MAX_FPA_LEN in Ai_data.h */
#define AIW_MAX_FPI_STR_SIZE	 21  /* MAX_FPI_LEN in Ai_data.h */
#define AIW_MAX_FPW_STR_SIZE	 21  /* MAX_FPW_LEN in Ai_data.h */
#define AIW_MAX_FSA_STR_SIZE	 21  /* MAX_FSA_LEN in Ai_data.h */
#define AIW_MAX_GMI_STR_SIZE	 64  /* Sect 4.1.1 IS-131 says 2048 */
#define AIW_MAX_GMM_STR_SIZE	 64  /* Sect 4.1.2 IS-131 says 2048 */
#define AIW_MAX_GMR_STR_SIZE	 64  /* Sect 4.1.3 IS-131 says 2048 */
#define AIW_MAX_GOI_STR_SIZE	 64  /* Sect 4.1.4 IS-131 says 2048 */
#define AIW_MAX_GSN_STR_SIZE	 64  /* Sect 4.1.5 IS-131 says 2048 */
#define AIW_MAX_NUMBER_OF_TIMERS 20
#define AIW_MAX_USRID_LEN        24
#define AIW_MAX_PSWD_LEN         24
#define AIW_MAX_FIF_LEN          90
#define AIW_MAX_NUM_LINE_IN_RSP  24
#define AIW_MAX_ATPARMS          8
#define AIW_MAX_COMMAND_BODY_LEN 16

#ifdef SYS_OPTION_ENHANCEDAT
#define AIW_ENHANCED_AT_CMD_LEN     520
#endif
								 
/***********************************************************************/
/* Global Typedefs                                                     */
/***********************************************************************/
typedef enum
{
   AIW_CMP_ERROR_MSG,                /* alias: aiCmpErrorInd            */
   AIW_CMP_CMPRES_RSP_MSG,           /* alias: aiCmpCompressRsp         */
   AIW_CMP_EXPAND_DONE_MSG,          /* alias: aiCmpExpandCompleteInd   */
   AIW_CMP_EXPAND_RSP_MSG,           /* alias: aiCmpEXpandRsp           */
   AIW_CMP_FLUSH_RSP_MSG,            /* alias: aiCmpFlushRsp            */
   AIW_CP_ALERT_IND_MSG,             /* alias: aiCpAlertInd             */
   AIW_CP_CONN_IND_MSG,              /* alias: aiCpConnectInd           */
   AIW_CP_DORMANT_IND_MSG,           /* alias: aiCpDormantInd           */
   AIW_CP_PKT_CALL_EV_MSG,           /* alias: aiCpPacketCallEventInd   */
   AIW_CP_PKT_CALL_ST_MSG,           /* alias: aiCpPacketCallStateInd   */
   AIW_CP_PAGE_IND_MSG,              /* alias: aiCpPageInd              */
   AIW_CP_RELEASE_IND_MSG,           /* alias: aiCpReleaseInd           */
   AIW_CP_EVENT_IND_MSG,             /* Concentrator for all CP events  */
   AIW_CP_ORIG_FAIL_IND_MSG,         /* alias: aiCpOrigFailInd          */
   AIW_IOP_BREAK_REQ_MSG,            /* alias: aiDpdBreakReq            */
   AIW_IOP_C108_IND_MSG,             /* alias: aiDpdC108Ind             */
   AIW_IOP_ONLN_CMD_REQ_MSG,         /* alias: aiDpdOLCmdReq            */
   AIW_IOP_RATE_IND_MSG,             /* alias: aiDpdRateInd             */
   AIW_IOP_RX_RSP_MSG,               /* alias: aiDpdRxRsp               */
   AIW_IOP_TX_REQ_MSG,               /* alias: aiDpdTxReq               */
   AIW_IOP_SET_BAUD_RSP_MSG,         /* in resp to IOP_SETBAUD_DATA_MSG */
   AIW_HL_BROWSER_CONN_REQ_MSG,      /* alias: aiNspeBrowserConnectReq  */
   AIW_HL_UPB_DORMANT_REQ_MSG,       /* alias: aiNspeBrowserDormantReq  */
   AIW_HL_BROWSER_DISCONN_REQ_MSG,   /* alias: aiNspeBroqserHangupReq   */
   AIW_HL_UPB_DIGIT_MSG,             /* To receive browser dial digits  */
   AIW_HL_PACKET_RECONN_REQ_MSG,     /* alias: aiNspePktReConnectReq    */
   AIW_HL_PPP_CONN_RSP_MSG,          /* alias: aiNspePppConnectRsp      */
   AIW_HL_RX_DATA_MSG,               /* alias: aiNspeRxInd              */
   AIW_HL_RX_RSP_MSG,                /* alias: aiNspeRxRsp              */
   AIW_HL_STATUS_MSG,                /* alias: aiNspeStatusInd          */
   AIW_HL_TX_RSP_MSG,                /* alias: aiNspeTxRsp              */
   AIW_HL_TX_REQ_MSG,                /* alias: aiNspeTxReq              */
   AIW_HL_UM_PPP_STATUS_MSG,         /* alias: aiNspeUmPppConnectRsp    */
   AIW_HL_RM_PPP_STATUS_MSG,         /* alias: aiNspeRmPppConnectRsp    */
   AIW_RLP_CLOSE_RSP_MSG,            /* alias: aiRlpCloseRsp            */
   AIW_RLP_OPEN_RSP_MSG,             /* alias: aiRlpOpenRsp             */    
   AIW_RLP_IDLE_IND_MSG,             /* alias: aiRlpinactivityMsg       */
   AIW_RLP_TX_RSP_MSG,               /* alias: aiRlpTxRsp               */
   AIW_RLP_RX_DATA_MSG,              /* alias: aiRlpRxInd               */
   AIW_TIMER_EXPIRED_MSG,            /* alias: aiSysTimerExpiredInd     */
   AIW_OP_MODE_MSG,                  /* operated mode in NSPE/AppInt.   */
   AIW_DBM_WRITE_DATA_RSP_MSG,       /* DBM Ack to data-write request   */
   AIW_DBM_READ_RSP_MSG,             /* DBM send cached data to AIW.    */
   AIW_FWD_MORE_DATA_REQ_MSG,        /* AIW send this to itself in order
                                        to make engine call aiNspeRxRsp */
   AIW_SET_DEFAULT_MSG,              /* To set default on AT commands.  */
   AIW_INIT_REQ_MSG,                 /* To read AT settings from DBM.   */
   AIW_FLUSH_REQ_MSG,                /* To flush AiwNvmData to DBM.     */ 
   AIW_SET_QNC_DIAL_STR_MSG,         /* To provide QNC dial string.     */
   AIW_SET_USER_INFO_MSG,            /* To set username, password.      */
   AIW_NO_SVC_IND_MSG,               /* To indicate no service.         */
   AIW_DISABLE_MSG,
   AIW_POWER_DOWN_MSG,          /* To indicate the user plan to power down */   
   AIW_BAL_AT_CMD_RSP_MSG,                   /* To send AT command response code*/
   AIW_BAL_L1D_RSSI_MSG,           /*To indicate RSSI Value*/
   AIW_BAL_CP_STATUS_RPT_MSG,  /*To indicate Status message*/
   AIW_BAL_DATA_CALL_DISC_MSG,

   AIW_NUM_MSG_IDS				     
} AiwMsgIdT;

typedef enum 
{
  AIWOF_Intercept,
  AIWOF_Reorder,
  AIWOF_Release,
  AIWOF_Reject,
  AIWOF_Disabled,
  AIWOF_Busy,
  NUM_AIWOFs
} AiwOrigFailReason;


typedef enum
{
   AIW_AI_HL,                /* Serial port initiated data/fax call    */
   AIW_HL,                   /* Packet data mode with Rm interface.    */
   AIW_NONE                  /* data/fax call uses external PC card.   */
} AiwModeSelT;

typedef enum 
{
  AIW_ABC_Unknown,
  AIW_ABC_800MHz,
  AIW_ABC_1900MHz,
  AIW_NUM_ABCs
} AiwBandClass;

typedef enum
{
   AIW_ANS_CONNECTSUCCESS,
   AIW_ANS_CONNECTFAILTCP,
   AIW_ANS_CONNECTFAILPPP,
   AIW_ANS_CONNECTFAILRLP,
   AIW_ANS_CONNECTFAILUART,
   AIW_ANS_DISCONNECTNORMAL,
   AIW_NUM_ANS
} AiwHlStatusT;

typedef enum   /* This definition shall agree with defines found in 7074API.h */
{
  AIW_ANS_ConnectSuccess,
  AIW_ANS_ConnectFailTCP,
  AIW_ANS_ConnectFailPPP,
  AIW_ANS_ConnectFailRLP,
  AIW_ANS_ConnectFailUart,
  AIW_ANS_DisconnectNormal = 5,
  AIW_NUM_ANSs
} AiwNspeStatus;

typedef enum 
{
  AIW_BS_Success,
  AIW_BS_Busy,
  AIW_BS_Rejected,
  AIW_BS_Failed,
  AIW_BS_NoSvc,
  AIW_BS_DisconnectNormal,
  AIW_BS_ConnectionDropped,
  AIW_BS_Dormant,
  AIW_BS_Reconnected,
  AIW_NUM_BSs
} AiwBrowserConnectStatus;

#ifdef SYS_OPTION_HL
typedef PACKED struct
{
   uint8              StrChar[AIW_MAX_CFG_STR_SIZE];  
} AiwDbmCfgStrT;

typedef PACKED struct
{
   uint8              StrChar[AIW_MAX_FDL_STR_SIZE];
} AiwDbmFdlStrT;

typedef PACKED struct 
{
   uint8              StrChar[AIW_MAX_FLI_STR_SIZE];
} AiwDbmFliStrT;

typedef PACKED struct 
{
   uint8              StrChar[AIW_MAX_FPA_STR_SIZE];
} AiwDbmFpaStrT;

typedef PACKED struct 
{
   uint8              StrChar[AIW_MAX_FPI_STR_SIZE];
} AiwDbmFpiStrT;

typedef PACKED struct 
{
   uint8              StrChar[AIW_MAX_FPW_STR_SIZE];
} AiwDbmFpwStrT;

typedef PACKED struct 
{
   uint8              StrChar[AIW_MAX_FSA_STR_SIZE];
} AiwDbmFsaStrT;

typedef PACKED struct 
{
   uint8              StrChar[AIW_MAX_GCAP_STR_SIZE];
} AiwDbmGcapStrT;

typedef PACKED struct 
{
   uint8              StrChar[AIW_MAX_GMI_STR_SIZE];
} AiwDbmGmiStrT;

typedef PACKED struct 
{
   uint8              StrChar[AIW_MAX_GMM_STR_SIZE];
} AiwDbmGmmStrT;

typedef PACKED struct 
{
   uint8              StrChar[AIW_MAX_GMR_STR_SIZE];
} AiwDbmGmrStrT;

typedef PACKED struct 
{
   uint8              StrChar[AIW_MAX_GOI_STR_SIZE];
} AiwDbmGoiStrT;

typedef PACKED struct 
{
   uint8              StrChar[AIW_MAX_GSN_STR_SIZE];
} AiwDbmGsnStrT;

typedef PACKED struct
{
   uint8             Carrier[8];
} AiwDbmMaT;

typedef PACKED struct
{
   uint8            UserId[AIW_MAX_USRID_LEN];
   uint8            Pswd[AIW_MAX_PSWD_LEN];   
} AiwUserInfoT;

typedef PACKED struct
{
  uint8 bcs;
  uint8 bcl;
} AiwCbc;

typedef PACKED struct
{
  uint8  direction;
  bool   compressionNegotiation;
  uint16 maxDict;
  uint8  maxString;
} AiwDs;

typedef PACKED struct
{
  uint8 sqm;
  uint8 fer;
} AiwCsq;

typedef PACKED struct
{
  AiwBandClass bandClass;
  char band;
  uint16 sid;
} AiwCss;

typedef PACKED struct
{
  uint8 breakSelection;
  bool timed;
  uint8 defaultLen;
} AiwEb;

typedef PACKED struct
{
  uint8 origRqst;
  uint8 origFbk;
  uint8 ansFbk;
} AiwEs;

typedef PACKED struct
{
  uint8 pendingTd;
  uint8 pendingRd;
  uint8 timer;
} AiwEtbm;

typedef PACKED struct
{
  bool sub;
  bool sep;
  bool pwd;
} AiwFap;

typedef PACKED struct
{
  bool vr;
  uint8 br;
  uint8 wd;
  uint8 ln;
  uint8 df;
  bool ec;
  bool bf;
  uint8 st;
} AiwFcc;

typedef PACKED struct
{
  uint8 rq;
  uint8 tq;
} AiwFcq;

typedef PACKED struct
{
  uint8 vrc;
  uint8 dfc;
  uint8 lnc;
  uint8 wdc;
} AiwFfc;

typedef PACKED struct
{
  bool rpr;
  bool tpr;
  bool idr;
  bool nsr;
} AiwFnr;

typedef PACKED struct
{
  uint8 buf[AIW_MAX_FIF_LEN];
  uint8 len;
} AiwFif;

typedef PACKED struct
{
  uint8 pgl;
  uint8 cbl;
} AiwFrq;

typedef PACKED struct
{
  uint8 format;
  uint8 parity;
} AiwIcf;

typedef PACKED struct
{
  uint8 dceByDte;
  uint8 dteByDce;
} AiwIfc;

typedef PACKED struct
{
  uint8 carrier;
  bool automode;
  uint16 minRate;
  uint16 maxRate;
  uint16 minRxRate;
  uint16 maxRxRate;
} AiwMs;

typedef PACKED struct
{
  uint8 mode;
  uint8 dfltAnsMode;
  bool fbkTimeEnable;
} AiwMv18s;

typedef PACKED struct  
{
   bool             ParamSetFlag;
   bool             C109Parameter;
   uint8            CADParameter;
   AiwCbc           CBCParameter;
   uint32 	        CBIPParameter;
   bool             CDRParameter;
   AiwDs            CDSParameter;
   uint8            CFCParameter;
   uint32           CMIPParameter;
   uint8            CMUXParameter;
   bool             CPERParameter;
   uint16           CPSParameter;
   bool             CPSRParameter;
   uint8            CQDParameter;
   bool             CRCParameter;
   uint8            CRMParameter;
   AiwCsq           CSQParameter;
   AiwCss           CSSParameter;
   uint8            CTAParameter;
   bool             CXTParameter;
   uint8            C108Parameter;
   uint8            DParameter;
   bool             DialTypeParameter;
   bool             DRParameter;
   AiwDs            DSParameter;
   bool             EParameter;
   AiwEb            EBParameter;
   uint8            EFCSParameter;
   bool             ERParameter;
   AiwEs            ESParameter;
   uint8            ESRParameter;
   AiwEtbm          ETBMParameter;
   bool             FAAParameter;
   AiwFap           FAPParameter;
   uint8            FBOParameter;
   bool             FBUParameter;
   AiwFcc           FCCParameter;
   uint8            FCLASSParameter;
   AiwFcq           FCQParameter;
   bool             FCRParameter;
   uint8            FCTParameter;
   bool             FEAParameter;
   AiwFfc           FFCParameter;
   uint8            FHSParameter;
   bool             FIEParameter;
   AiwFcc           FISParameter;
   uint8            FLOParameter;
   bool             FLPParameter;
   uint8            FMSParameter;
   AiwFnr           FNRParameter;
   AiwFif           FNSParameter;
   bool             FPPParameter;
   uint8            FPRParameter;
   uint8            FPSParameter;
   AiwFrq           FRQParameter;
   uint8            FRYParameter;
   bool             FSPParameter;
   bool             IBCParameter;
   AiwIcf           ICFParameter;
   AiwIfc           IFCParameter;
   bool             ILRRParameter;
   uint32           IPRParameter;
   uint8            LParameter;
   uint8            MParameter;
   bool             MRParameter;
   AiwMs            MSParameter;
   bool             MV18RParameter;
   AiwMv18s         MV18SParameter;
   bool             QParameter;
   uint8            S0Parameter;
   uint8            S3Parameter;
   uint8            S4Parameter;
   uint8            S5Parameter;
   uint8            S6Parameter;
   uint8            S7Parameter;
   uint8            S8Parameter;
   uint8            S9Parameter;
   uint8            S10Parameter;
   uint8            S11Parameter;
   bool             VParameter;
   uint8            XParameter;  
#ifdef SYS_OPTION_MIP_DMU
   uint8 	          DMUVParameter;
#endif
   AiwDbmCfgStrT    CfgStr;
   AiwDbmFdlStrT	FdlStr;
   AiwDbmFliStrT	FliStr;
   AiwDbmFpaStrT	FpaStr;
   AiwDbmFpiStrT	FpiStr;
   AiwDbmFpwStrT	FpwStr;
   AiwDbmFsaStrT	FsaStr;
   AiwDbmGcapStrT	GcapStr;
   AiwDbmGmiStrT	GmiStr;
   AiwDbmGmmStrT	GmmStr;
   AiwDbmGmrStrT	GmrStr;
   AiwDbmGoiStrT	GoiStr;
   AiwDbmGsnStrT	GsnStr;
   AiwDbmMaT        MaTbl;
   AiwUserInfoT     UserInfo;
   bool                  CMEEParameter;
} AiwDbmBinDataT;


typedef enum
{
   AIW_UDP_BROWSER          = 5, /* AIS_UpBrowser in Ai_data.h      */
   AIW_UDP_BROWSER_ON_ASYNC = 6, /* AIS_AsyncUpBrowser in Ai_data.h */
   AIW_TCP_CKT_BROWSER      = 7, /* AIS_TcpBrowser in Ai_data.h     */
   AIW_TCP_PKT_BROWSER      = 8, /* AIS_TcpBrowser in Ai_data.h     */
   AIW_PKT_NTWRK_RM         = 9, /* AIS_PacketNtwkRm in Ai_data.h   */
   AIW_PPP_ONLY             = 10 /*	AIS_PPPOnly in Ai_data.h        */
} AiwBrowsersT;
#endif

/***********************************************************************/
/* Message Definitions                                                 */
/***********************************************************************/
#ifdef SYS_OPTION_HL
typedef PACKED struct
{ 
   uint8*          DataBufP;        /* Points to the first byte of data*/
   uint16          DataLen;         /* size of data in byte            */
} AiwCmpCmpresRspMsgT;

typedef PACKED struct
{
   uint8*          DataBufP;        /* Points to the first byte of data*/
   uint16          DataLen;         /* size of data in byte            */
} AiwCmpExpandRspMsgT;

typedef PACKED struct
{
   uint16          ServiceOption;   /* Service option negotiated IS-95 */
   bool            SecondaryTraffic;/* FALSE if Primary Traffic        */
} AiwCpConnIndMsgT;

typedef PACKED struct
{
   uint8             Event;         /* Table 7.4.2-2 per IS707A.3      */
} AiwCpPktCallEvMsgT;

typedef PACKED struct
{
   uint8             NewState;      /* Table 7.4.2-2 per IS707A.3      */
} AiwCpPktCallStMsgT;

typedef PACKED struct
{
   uint8              ServiceType;   /* service type in Page Response   */   
} AiwCpPageIndMsgT;

typedef PACKED struct
{ 
   uint16             Reason;        /* Release reason.                 */
} AiwCpReleaseIndMsgT;

typedef PACKED struct
{ 
   uint8              cpEvent;       /* CP event information            */
} AiwCpEventIndMsgT;

typedef PACKED struct
{
   AiwOrigFailReason  Reason;        /* CP origination fail at data service. */
} AiwCpOrigFailIndMsgT;

typedef PACKED struct
{
   AiwModeSelT        Mode;   
} AiwOpModeMsgT;

typedef PACKED struct
{ 
   uint8             BreakLen;      /* in 10 msec units                 */
} AiwIopBreakReqMsgT;
                         
typedef PACKED struct 
{ 
   bool              C108On;        /* the state of C108, DTE ready.    */
} AiwIopC108IndMsgT; 

typedef PACKED struct
{ 
   uint32            Rate;          /* Bit rate on both Tx and Rx side. */
} AiwIopRateIndMsgT;

typedef PACKED struct
{ 
   uint8*            DataBufP;      /* points to the first byte of data */
   uint16            DataLen;       /* data size in bytes.              */   
} AiwIopTxReqMsgT;



typedef PACKED struct
{
   uint8             Browser;     
} AiwHlBrowserConnReqMsgT;

typedef PACKED struct
{
   uint8             Digit[AIW_MAX_BRSWR_DGT_SIZE];
   uint16            ServiceOption;
}  AiwHlUpbDigitMsgT;               /* Digits used for browser to dial. */ 

typedef PACKED struct
{
   AiwNspeStatus     ConnResult;    /* ANS_ConnectSuccess,
                                       ANS_ConnectFailTCP,
                                       ANS_ConnectFailPPP,
                                       ANS_ConnectFailRLP,
                                       ANS_DisconnectNormal             */
   uint32            LocalIpAddr;
   uint32            RemoteIpAddr;   
} AiwHlPppConnRspMsgT;

typedef PACKED struct
{ 
   uint8*            DataBufP;      /* points to the first byte of data */
   uint16            DataLen;       /* data size in bytes.              */   
} AiwHlRxDataMsgT;

typedef PACKED struct
{ 
   AiwHlStatusT      Status;        /* Status of NSPE at transport layer*/
   uint32            MsIp;          /* mobile IP, if connected          */
   uint32            BsIp;          /* base station(IWF)IP, if connected*/
} AiwHlStatusMsgT;

typedef PACKED struct  
{
   uint8*            DataBufP;      /* points to the first byte of data */
   uint16            DataLen;       /* data size in bytes.              */   
} AiwHlTxReqMsgT;

typedef PACKED struct
{
   AiwNspeStatus     ConnResult;    /* ANS_ConnectSuccess,
                                       ANS_ConnectFailTCP,
                                       ANS_ConnectFailPPP,
                                       ANS_ConnectFailRLP,
                                       ANS_DisconnectNormal             */
   uint32            LocalIpAddr;
   uint32            RemoteIpAddr;   
} AiwHlUmPppStatusMsgT;

typedef PACKED struct
{
   AiwNspeStatus      ConnResult;    /* ANS_ConnectSuccess,
                                       ANS_ConnectFailTCP,
                                       ANS_ConnectFailPPP,
                                       ANS_ConnectFailRLP,
                                       ANS_DisconnectNormal             */
} AiwHlRmPppStatusMsgT;

typedef PACKED struct
{ 
   bool              Secondary;     /* FALSE, if primary.               */
} AiwRlpCloseRspMsgT;

typedef PACKED struct
{ 
   bool              Secondary;     /* FALSE, if primary.               */
   bool              Successful;    
} AiwRlpOpenRspMsgT;


typedef PACKED struct
{ 
   bool              Secondary;     /* FALSE, if primary.               */
} AiwRlpIdleIndMsgT;

typedef PACKED struct
{ 
   bool              Secondary;     /* FALSE, if primary.               */
} AiwRlpTxRspMsgT;

typedef PACKED struct
{ 
   uint8*            DataBufP;   /* point to the first byte of data.    */
   uint16            DataLen;    /* The size of data in byte.           */
   bool              Secondary;  /* False, if primary channel.          */
} AiwRlpRxDataMsgT;

typedef PACKED struct
{
   uint32            TimerId;                       /* Expired timer Id */
} AiwTimerExpiredMsgT;

typedef PACKED struct
{
   uint8             QNCStr[AIW_MAX_BRSWR_DGT_SIZE]; /* null-terminated */
} AiwSetQNCDialStrMsgT;

typedef PACKED struct
{
   AiwUserInfoT      User;
} AiwSetUserInfoMsgT;
typedef PACKED struct  /*shenchao add 2003/06/25*/
{
		uint8 Roam; 					  /* ROAM Status						  */
		uint16 Band;					  /* Current Operating Band 			  */
		uint16 Channel; 				  /* Current Channel Number 			  */
		uint8  Mode;					  /* current mode: PCS/Cellular/Analog	  */
		uint8  Block;					  /* current CDMA block (if CDMA system)  */
		uint8 ServingSystem;			  /* Serving System/Block				  */
		uint16 SysID;					  /* Last-Received System ID  (sid) 	  */
		uint16 LocArea; 				  /* Current Location Area ID (nid) 	  */
		uint16 PilotPn; 				  /* PILOT_PN							  */
} AiwCpStatusRptMsgT;

typedef struct 
{
  char*  data;
  uint16 len;
} AiwDataParse;

typedef union
{
   uint32        num32;
   char*         ptrCharStr;
   AiwDataParse	 buf;
} AiwParmT;

typedef PACKED struct
{
/*   AiwParmT*  ParmList[AIW_MAX_NUM_LINE_IN_RSP]; */
   char*          ParmLine[AIW_MAX_NUM_LINE_IN_RSP];  
} AiwParmArrayT;

typedef PACKED struct
{
   char           cmdName[AIW_MAX_COMMAND_BODY_LEN];
   AiwParmArrayT  LinesOfParms;
   uint8           numOfValidLines;
   bool           rspStatus;
   bool           needBufferAck;
   bool           skipResultCode;
   char*         ErrResultNum;
} AiwSendAtRespMsgT;


typedef PACKED struct
{
   char           cmdName[16];
   AiwParmT*      ParmList[AIW_MAX_ATPARMS];
} AiwSendAtMsgT;
#endif



#endif




