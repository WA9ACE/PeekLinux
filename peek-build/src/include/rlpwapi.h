#ifndef _RLPWAPI_H_
#define _RLPWAPI_H_


/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/
#include "sysdefs.h"
#include "lmdapi.h" 

/*----------------------------------------------------------------------------
 Global Defines and Macros
----------------------------------------------------------------------------*/
#define RLP_MAX_SPY_LEN         64   /* Max size for Monspy output.         */
#define RLP_MAX_NUM_SUPL_CHAN   7
#define RLP_IOP_MAX_REV_SIZE    120  /* Max. size per IOP->RLP data xfer
                                        at reverse link.                 	*/
#define RLP_MAILBOX_CMD         EXE_MAILBOX_1_ID
#define RLP_STARTUP_SIGNAL      EXE_SIGNAL_1       

/***********************************************************************/
/* Global Typedefs                                                     */
/***********************************************************************/
/* Message ID define for all mail messages of RLPW task.               */
typedef enum
{
   RLP_FRAME_IND_MSG,            /* from LMD                          */
   RLP_SUP_FRAME_IND_MSG,        /* from LMD                          */
   RLP_FWD_SUPL_FRAME_MSG,       /* from LMD                          */
   RLP_FRAME_REQ_MSG,            /* from LMD                          */
   RLP_SUP_FRAME_REQ_MSG,        /* from LMD                          */	
   RLP_DTCH_FWD_DATA_MSG,        /* from LMD                          */
   RLP_SCH_FWD_DATA_MSG,         /* from LMD                          */ 
   RLP_SCCH_FWD_DATA_MSG,        /* from LMD                          */ 
   RLP_FCH_REV_DATA_REQ_MSG,     /* from LMD                          */ 
   RLP_DCCH_REV_DATA_REQ_MSG,    /* from LMD                          */ 
   RLP_SCCH_REV_DATA_REQ_MSG,    /* from LMD                          */ 
   RLP_SCH_REV_DATA_RSP_MSG,     /* from LMD                          */ 
   RLP_SCCH_REV_DATA_RSP_MSG,    /* from LMD                          */ 
   RLP_AI_BYPASS_FWD_DATA_MSG,   /* from AI                           */
   RLP_OPEN_MSG,                 /* from LMD                          */
   RLP_CLOSE_MSG,                /* from LMD                          */
   RLP_FWD_DATA_RSP_MSG,         /* from HL/AI                        */
   RLP_REV_DATA_MSG,             /* from HL/AI/IOP                    */	
   RLP_IOP_REV_DATA_MSG,         /* from IOP                          */
   RLP_REV_DATA_RSP_MSG,         /* from RLP itself                   */	
   RLP_FWD_MORE_DATA_REQ_MSG,    /* from RLP itself                   */
   RLP_INIT_MSG,                 /* from PS                           */
   RLP_RATE_CONTROL_MSG,         /* from PS                           */
   RLP_SERV_CONF_MSG,         	 /* from PS                           */
   RLP_REINSTATE_RATE_MSG,       /* from AI                           */
   RLP_INACT_TMO_REQ_MSG,        /* from HL/AI                        */
   RLP_RESET_STAT_MSG,           /* from ETS                          */
   RLP_RESET_FRAME_COUNT_MSG,    /* from ETS                          */
   RLP_PEEK_FRAME_STAT_MSG,		 /* from ETS                          */
   RLP_DISABLE_MSG,
   RLP_NUM_MSG_IDS               /* # of messages in the task RLP     */
} RlpMsgIdT;

typedef enum
{
   RLP_PRIMARY_CHANNEL = 0,     /* Do not change this.                */
   RLP_SECONDARY_CHANNEL
} RlpChannelSelT;

typedef enum
{
   RLP_NON_TRANSPARENT = 0,    /* Do not change this, either.        */
   RLP_TRANSPARENT
} RlpModeT;

typedef enum
{
   RLP_RATE_SET1 = 0,           /* according to ISOTEL's API.          */
   RLP_RATE_SET2
} RlpRateSetT;

typedef enum
{
   RLP_TYPE_1 = 0,              /* according to IS707                 */
   RLP_TYPE_2,
   RLP_TYPE_3
} RlpTypeT;


typedef PACKED struct
{
   bool RateAllowed[SYS_RLP_RATE_TOTAL]; /* Table entries to be read   */
                                         /* in the order of entries    */
                                         /* "SysRlpFrameRateT". TRUE   */
                                         /* if rate is allowed; FALSE, */
                                         /* otherwise.  */           
} RlpFrameRateTabT;


/***********************************************************************/
/* Message Definitions                                                 */
/***********************************************************************/

/* 
   this message is sent from LMD to request RLP transmit a new frame 
   on the reverse FCH or DCCH
*/
typedef PACKED struct
{
  ExeRspMsgT       RspMsg;        /* required response message         */ 
  uint8            PrimarySrId;	  /* SR_ID for the primary channel     */ 
  SysRlpFrameRateT PrimaryRate;   /* available frame rate  for primary */
  uint8            SecondarySrId; /* SR_ID for the secondary channel   */
  SysRlpFrameRateT SecondaryRate; /* available frame rate for secondary*/
} RlpFrameReqMsgT;


/*
   this message is sent from LMD to indicate a new data frame received
   on the forward FCH or DCCH
*/
typedef PACKED struct
{
  uint8            PrimarySrId;   /* SR_ID for the primary channel     */    
  SysRlpFrameRateT PrimaryRate;   /* frame rate received for primary   */ 
  SysRlpFrameBufT  PrimaryData;   /* data received for primary, null   */
                                  /* if PrimaryRate is blank or bad    */
  uint8            SecondarySrId; /* SR_ID for the secondary channel   */
  SysRlpFrameRateT SecondaryRate; /* frame rate received for secondary */
  SysRlpFrameBufT  SecondaryData; /* data received for secondary, null */
                                  /* if SecondaryRate is blank or bad  */
} RlpFrameIndMsgT;


/* 
   this message is sent from LMD to request RLP to transmit a new frame 
   on the reverse SCH
*/
typedef PACKED struct
{
  ExeRspMsgT       RspMsg;        /* required response message              */ 
  uint8            SupSrId;	      /* SR_ID for supplemental channel         */ 
  SysRlpFrameRateT SupRate;       /* available frame rate  for supplemental */
} RlpSupFrameReqMsgT;


/*
   this message is sent from LMD to indicate a new frame received on
   the forward SCH
*/
typedef PACKED struct
{
   uint8            SupSrId;      /* SR_ID for supplemental channel      */
   SysRlpFrameRateT SupRate;	  /* frame rate received on supplemental */
   SysRlpFrameBufT  SupData;      /* frame data received on supplemental */
                                  /* null if SupRate is blank or bad     */
} RlpSupFrameIndMsgT;


typedef PACKED struct
{
  uint8           NumChannels;  /* number of supplemental channel.     */
  RlpChannelSelT  ChannelType;  /* primary or secondary channel.       */
  SysRlpFrameBufT SuplFrame[RLP_MAX_NUM_SUPL_CHAN]; /* frame data on   */
                                                    /* each suppl'l ch.*/
} RlpFwdSuplFrameMsgT;

typedef PACKED struct
{
   bool            Mode;            /* bypass or not                  */
} RlpAiBypassFwdDataT;

typedef enum
{
   RLP_FOR_HLW,                     /* This RLP is opened by HL       */
   RLP_FOR_AIW,					    /* This RLP is opened by AI       */
   RLP_FOR_PSW,						/* This RLP is opened by PS       */
   RLP_AS_IS						/* Unspecified                    */
}RlpClientT;

typedef PACKED struct
{
  uint8			   SrId;		    /* SR_ID                          */
  RlpChannelSelT   ChannelSel;      /* Primary or Secondary Channel.  */
  RlpModeT         ModeType;        /* Non/transparent mode.          */
  uint16           InActTimeOut;    /* 0 = disable, non_zero = number
	                                   of 20 msec frames.             */
  RlpClientT       Client;
  bool            reOpen;          /* indicate it's a reopen triggered by RLP open failed. */												  
} RlpOpenMsgT;				   

typedef PACKED struct 
{
  uint8            SrId;            /* SR_ID                          */
  RlpChannelSelT   ChannelSel;      /* Primary or Secondary Channel.  */
  bool             FlushPending;    /* TRUE, to flush any pending RLP */
                                    /* data frame.                    */
                                    /* FALSE, RLP to transmit any     */
                                    /* pending data before close.     */               
} RlpCloseMsgT;

typedef PACKED struct
{
   RlpChannelSelT   ChannelSel;      /* Primary or Secondary Channel.  */
} RlpFwdDataRspMsgT;

typedef PACKED struct
{
   RlpChannelSelT   ChannelSel;      /* Primary or Secondary Channel.  */
} RlpFwdMoreDataReqMsgT;

typedef PACKED struct
{
   RlpChannelSelT   ChannelSel;      /* Primary or Secondary Channel.  */
   uint8*           DataBufP;        /* Pointer to data buffer.        */
   uint16           DataLen;         /* Length of RLP data to transmit */
} RlpRevDataMsgT;

typedef PACKED struct
{
   uint8       DataLen;	 				     /* Length of data in byte.*/
   uint8       DataBuf[RLP_IOP_MAX_REV_SIZE];   /* max. size of a RLP  */
} RlpIopRevDataMsgT;						 

typedef PACKED struct
{
   RlpChannelSelT   ChannelSel;      /* Primary or Secondary Channel.  */
} RlpRevDataRspMsgT;


typedef PACKED struct 
{
   uint8            SrId;            /* SR_ID                          */
   RlpChannelSelT   ChannelSel;      /* Primary or Secondary Channel.  */
   RlpRateSetT      FwdRateSet;      /* Rate set for forward link.     */
   RlpRateSetT      RevRateSet;      /* Rate set for reverse link.     */
   UINT16           FwdSchMuxOpt;    /* Mux option for fwd SCH         */
   UINT16           RevSchMuxOpt;    /* Mux option for rev SCH         */
   RlpTypeT         RlpType;         /* RLP_TYPE_1, 2 and 3            */
   bool             paramValid;      /* TRUE: valid in next two params */
   uint32           Rand;	         /* To generate LTable.            */
   uint8            SSDB[8];         /* To generate Data Key.          */
} RlpInitMsgT;

typedef PACKED struct
{
   uint8     RateCtrlParam;          /* Rate control parameter.        */
} RlpRateControlMsgT;



/* Note that the Max_rounds are defined in PS, RLP engine (rlp_prot.h), and here! */
#define	RLP3_MAX_ROUNDS_FWD		7
#define	RLP3_MAX_ROUNDS_REV		7

typedef PACKED struct 
{ 
  bool			   revSCHsupported;		/* Whether RSCH is supported by BS or not */

  /* The following are negotiated rlp blob params by the PS */
  UINT8            rlpBlobType;
  UINT8            rlpVersion;
  UINT8            rtt;
  bool			   initVar;
  UINT32           bsExtSeqM;
  UINT32           msExtSeqM;
  UINT8            maxMsNakRoundsFwd;
  UINT8            maxMsNakRoundsRev;
  UINT8            nakRoundsFwd;
  UINT8            nakRoundsRev;
  UINT8            nakPerRoundFwd[ RLP3_MAX_ROUNDS_FWD ];
  UINT8            nakPerRoundRev[ RLP3_MAX_ROUNDS_REV ];
} RlpServConfMsgT; 




typedef PACKED struct
{
   bool 			fromHl;          /* TRUE: HL sends this, FALSE: AI 
                                        sends this.                    */
   RlpChannelSelT   ChannelSel;      /* Primary or Secondary Channel.  */
   uint16           timeOut;         /* number of 20 msec frames.      */  
} RlpInactTmoReqMsgT;		
                          
typedef PACKED struct 
{ 
   uint8            PriSrId;    /* SR_ID for the primary traffic       */ 
   SysRlpFrameRateT PriRate;    /* frame rate received for primary     */ 
   SysRlpFrameBufT  PriData;    /* data received for primary traffic   */ 
   uint8            SecSrId;    /* SR_ID for the secondary traffic     */ 
   SysRlpFrameRateT SecRate;    /* frame rate received for secondary   */ 
   SysRlpFrameBufT  SecData;/* data received for secondary traffic */ 
}RlpDtchFwdDataMsgT;
 
#if 0 /* wyf */
typedef PACKED struct 
{ 
   uint8            PriSrId;    /* SR_ID for the primary traffic       */ 
   SysRlpFrameRateT PriRate;    /* frame rate received for primary     */ 
   SysRlpFrameBufT  PriData;    /* data received for primary traffic   */ 
   uint8            SecSrId;    /* SR_ID for the secondary traffic     */ 
   SysRlpFrameRateT SecRate;    /* frame rate received for secondary   */ 
   SysRlpFrameBufT  SecFrameBuf;/* data received for secondary traffic */ 
} RlpDcchFwdDataMsgT;  
#endif

typedef PACKED struct 
{ 
   ExeRspMsgT    RspMsg;         /* required response message          */ 
   uint8         Page;           /* Page in the buffer to be read      */
   uint8         NumberOfSchPDU; /* Number of PDU data packets         */ 
   LmdPDUOctetT  PDUData[MAX_PDU_NUM];  /* data type defined in PDUOctet */ 
} RlpSchFwdDataMsgT; 

typedef PACKED struct 
{ 
   bool       IsSecondary;    /* False=Primary, True=Secondary */ 
   uint8      NumberOfSCCH;   /* Number of SCCH included       */ 
   uint8*     Data[7];        /* data pointed by DataByte      */ 
} RlpScchFwdDataMsgT; 

typedef PACKED struct
{
   uint8      Page;          /* To indicate buffer ready to be reused. */
} RlpSchRevDataRspMsgT;

typedef PACKED struct 
{ 
  ExeRspMsgT       RspMsg;   /* required response message           */ 
  uint8            PriSrId;  /* SR_ID for the primary channel       */ 
  RlpFrameRateTabT PriRate;  /* available frame rate for primary    */ 
  uint8            SecSrId; /* SR_ID for the secondary channel      */ 
  RlpFrameRateTabT SecRate; /* available frame rate for secondary   */
  ExeRspMsgT       SchRspMsg; /* The Rev Sch's required resp msg    */
  uint8            NumberOfSchPDU; /* Number of PDU's allowed       */ 
  SysRlpFrameRateT PDURate[MAX_PDU_NUM]; /* rate allowed for each MuxPDU     */ 
} RlpDtchRevDataReqMsgT; 

typedef PACKED struct 
{
   ExeRspMsgT      RspInfo;    
} RlpPeekFrameStatMsgT;

typedef PACKED struct
{
  uint32           FwdFrameMissing;
  uint32           FwdFrameReceived;
  uint32           RevFrameMissing;
  uint32           RevFrameSent;
  uint32           MaxBlockMissing;   
  uint32           FwdTotal8PDUFrame;
  uint32	       FwdTotal4PDUFrame;
  uint32		   FwdTotal2PDUFrame;
  uint32		   FwdTotal1PDUFrame;
  uint32           TotalResets;
} RlpPeekFrameStatRspMsgT;


#if 0  /* wyf */
typedef PACKED struct 
{ 
  ExeRspMsgT       RspMsg;      /* required response message         */ 
  uint8            PriSrId;     /* SR_ID for the primary channel     */ 
  RlpFrameRateTabT PriRate;     /* available frame rate for primary  */ 
  uint8            SecSrId;     /* SR_ID for the secondary channel   */ 
  RlpFrameRateTabT SecRate;     /* available frame rate for secondary*/ 
  uint8            NumberOfSchPDU; /* Number of PDU allowed at most  */ 
  SysRlpFrameRateT PDURate[MAX_PDU_NUM];  /* rate allowed for each MuxPDU   */ 
} RlpDcchRevDataReqMsgT; 

#endif 
typedef PACKED struct 
{ 
  ExeRspMsgT       RspMsg;      /* required response message        */ 
  uint8            NumberOfSCCH;/* Number of SCCH allowed at most   */ 
} RlpScchRevDataReqMsgT; 

/************** For RLP Frame Spies, moved from rlp_data.h ***********/

enum EcdmaSysRlpFramePhyChan_enum
{
  CDMA_SYS_FCH,
  CDMA_SYS_DCCH,
  CDMA_SYS_SCH0,
  CDMA_SYS_SCH1
};

enum EcdmaSysRlpFrameMuxPdu_enum
{
  MUX_PDU1 = 1,
  MUX_PDU2,
  MUX_PDU3
}; 

enum EcdmaSysRlpFramePhyChanRate_enum
{
  CDMA_SYS_PHY_CHAN_RATE_9600,
  CDMA_SYS_PHY_CHAN_RATE_14400,
  CDMA_SYS_PHY_CHAN_RATE_19200,
  CDMA_SYS_PHY_CHAN_RATE_28800,
  CDMA_SYS_PHY_CHAN_RATE_38400,
  CDMA_SYS_PHY_CHAN_RATE_57600,
  CDMA_SYS_PHY_CHAN_RATE_76800,
  CDMA_SYS_PHY_CHAN_RATE_115200,
  CDMA_SYS_PHY_CHAN_RATE_153600,
  CDMA_SYS_PHY_CHAN_RATE_1200,
  CDMA_SYS_PHY_CHAN_RATE_1800,
  CDMA_SYS_PHY_CHAN_RATE_2400,
  CDMA_SYS_PHY_CHAN_RATE_3600,
  CDMA_SYS_PHY_CHAN_RATE_4800,
  CDMA_SYS_PHY_CHAN_RATE_7200,
  CDMA_SYS_PHY_CHAN_RATE_BLANK
 };

#ifdef  SYS_OPTION_SCH_RAM_INUSE
#define MUX_PDU_DATA_SIZE  346
#define MaxPduNum           8
#define SysMaxFsch          1
#define RLP_MAX_FRAME_LEN ((MUX_PDU_DATA_SIZE * MaxPduNum * SysMaxFsch)/8)
#else
#define RLP_MAX_FRAME_LEN   ((266 + 7)/8)  /* 266 bits for RS2 */
#endif

typedef PACKED struct
{
  enum EcdmaSysRlpFramePhyChan_enum CdmaSysRlpFramePhyChan;
  enum EcdmaSysRlpFrameMuxPdu_enum  CdmaSysRlpFrameMuxPdu;
  enum EcdmaSysRlpFramePhyChanRate_enum  CdmaSysRlpFramePhyChanRate;
  UINT8 CdmaSysRlpFrameTimeDelta;
  INT16 CdmaSysRlpFrameLen;
  UINT8 CdmaSysRlpFramePayload[RLP_MAX_FRAME_LEN];

} EcdmaSysRlpFrameData;

typedef PACKED struct
{
  INT32 CdmaSysRlpServiceId;
  EcdmaSysRlpFrameData CdmaSysRlpFrames;
	
} EcdmaSysRlpData;

typedef PACKED struct
{
  UINT32          timestamp;
  EcdmaSysRlpData FrameData;
} RlpFrameEvent;

/* for RLP Rx Frame Spy */
typedef enum EcdmaSysRlpFramePhyChanRate_enum RlpRxFrameRateT;
typedef enum EcdmaSysRlpFrameMuxPdu_enum      RlpFrameMuxPduTypeT;
typedef enum EcdmaSysRlpFramePhyChan_enum     RlpFramePhyChanT;



#endif


