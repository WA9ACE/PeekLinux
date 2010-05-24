#ifndef UIMAPI_H
#define UIMAPI_H


#include "sysdefs.h"
#include "sysapi.h"
#include "pswnam.h"
#include "pswcustom.h"

/*------------------------------------------------------------------------
* Define constants used in UIM API
*------------------------------------------------------------------------*/
#define SW_OK    0x9000
#define TERM_PROFILE_LEN 20

/*------------------------------------------------------------------------
* EXE Interfaces - Definition of Signals and Mailboxes
*------------------------------------------------------------------------*/
#define UIM_STARTUP_SIGNAL   EXE_SIGNAL_1
#define UIM_RESET_SIGNAL     EXE_SIGNAL_2

/* UIM command mailbox id */
#define UIM_MAILBOX         EXE_MAILBOX_1_ID
#define UIM_MAILBOX_S       EXE_MAILBOX_2_ID

#define MAXALPHALENGTH 15
#define MAXSMSMSGLENGTH 253

#define UIM_PARAM_P_LEN 64
#define UIM_PARAM_G_LEN 20
#define UIM_MAX_BS_RESULT_LEN 64
#define UIM_MAX_PARAM_SIZE 200
#define UIM_MAX_OTA_DATA_BURST_SIZE 255

typedef enum
{
    UIM_MF,
    UIM_DF_CDMA,
    UIM_DF_TELECOM,
    UIM_DF_MAX_NUM
} UimDFIndexT;
/*------------------------------------------------------------------------
* Define UIM message interface constants and structures
*------------------------------------------------------------------------*/
#define UIM_CMD_MSG_CLASS   0x0000
#define UIM_NAM_MSG_CLASS   0x0100
#define UIM_APP_MSG_CLASS   0x0200
#define UIM_MNG_MSG_CLASS   0x0300


/* Define UIM msg command Ids */
typedef enum 
{
    UIM_RAW_CMD_MSG = UIM_CMD_MSG_CLASS,
    UIM_SELECT_MSG,
    UIM_STATUS_MSG,
    UIM_READ_BINARY_MSG,
    UIM_UPDATE_BINARY_MSG,
    UIM_READ_RECORD_MSG,
    UIM_UPDATE_RECORD_MSG,
    UIM_VERIFY_CHV_MSG,
    UIM_CHANGE_CHV_MSG,
    UIM_ENABLE_CHV_MSG,
    UIM_DISABLE_CHV_MSG,
    UIM_UNBLOCK_CHV_MSG,   
    UIM_BS_CHALLENGE_MSG,
    UIM_CONFIRM_SSD_MSG,
    UIM_RUN_CAVE_MSG,
    UIM_GENERATE_KEY_VPM_MSG,
    UIM_STORE_ESN_ME_MSG,
    UIM_TERMINAL_PROFILE_MSG,
    UIM_TERMINAL_RESPONSE_MSG,
    UIM_ENVELOPE_MSG,
    UIM_MS_KEY_REQUEST_MSG,                      
    UIM_KEY_GENERATION_REQUEST_MSG,         
    UIM_COMMIT_MSG,                                  
    UIM_VALIDATE_MSG,                                    
    UIM_CONFIGURATION_REQUEST_MSG,    
    UIM_DOWNLOAD_REQUEST_MSG,    
    UIM_SSPR_CONFIGURATION_REQUEST_MSG, 
    UIM_SSPR_DOWNLOAD_REQUEST_MSG,  
    UIM_OTAPA_REQUEST_MSG,    
    UIM_OTA_RUN_CAVE_MSG,
    UIM_PROACTIVE_REGISTER_MSG,
    

    UIM_GET_NAM_DATA_MSG = UIM_NAM_MSG_CLASS,
    UIM_UPDATE_NAM_DATA_MSG,

    UIM_GET_PHB_REC_PARAMS_MSG= UIM_APP_MSG_CLASS,
    UIM_GET_PHB_REC_MSG,
    UIM_UPDATE_PHB_REC_MSG,
    UIM_ERASE_PHB_REC_MSG,
    UIM_GET_SMS_REC_PARAMS_MSG,
    UIM_GET_SMS_REC_MSG,
    UIM_UPDATE_SMS_REC_MSG,
    UIM_ERASE_SMS_REC_MSG,

    UIM_GET_STATUS_MSG,

    /* CHV APP */
    UIM_APP_VERIFY_CHV_MSG,
    UIM_APP_CHANGE_CHV_MSG,
    UIM_APP_ENABLE_CHV_MSG,
    UIM_APP_DISABLE_CHV_MSG,
    UIM_APP_UNBLOCK_CHV_MSG,

    UIM_NOTIFY_REGISTER_MSG = UIM_MNG_MSG_CLASS
} UimMsgIdT;

/* Define UIM msg header format */
typedef PACKED struct 
{
    ExeRspMsgT      RspInfo;    
} UimMsgHeaderT;
/* Define UIM msg response for UIM_CMD_MSG_CLASS format */
typedef PACKED struct 
{
    uint16          Sw; /* status word, 0x9000 denotes ACK, 0x0000 denotes FAULT, other value see GSM11.11 */
    uint16          Len;
    uint8           Data[1];
} UimRspMsgT;

/* Define UIM raw cmd msg command */
typedef PACKED struct 
{
    uint16          Sw; /* status word, 0x9000 denotes ACK, 0x0000 denotes FAULT, other value see GSM11.11 */
    uint8            CaveSeqId;
    uint16           CaveMsgId;
    uint16          Len;
    uint8           Data[1];
} UimRspToPsMsgT;

/* Define UIM raw cmd msg command */
typedef PACKED  struct 
{
   ExeRspMsgT       RspInfo;
   uint8            Cmd[5];
   uint8            Len;
   uint8            Data[1];
 } UimRawCmdMsgT;


/* Define UIM select msg command */
typedef PACKED  struct 
{
   ExeRspMsgT       RspInfo;
   uint16           EfId;
   UimDFIndexT      DfIndex; 
} UimSelectMsgT;

/* Define UIM status msg command */
typedef PACKED  struct 
{
   ExeRspMsgT       RspInfo;
} UimStatusMsgT;

/* Define UIM read record msg command */
typedef PACKED  struct 
{
   ExeRspMsgT       RspInfo;
   uint16           EfId;
   UimDFIndexT      DfIndex; 
   uint8            RecordIndex;
} UimReadRecordMsgT;

/* Define UIM update record msg command */
typedef PACKED  struct 
{
   ExeRspMsgT       RspInfo;
   uint16           EfId;
   UimDFIndexT      DfIndex; 
   uint8            RecordIndex;
   uint8            Len;
   uint8            Data[1];
} UimUpdateRecordMsgT;

/* Define UIM read binary msg command */
typedef PACKED  struct 
{
   ExeRspMsgT       RspInfo;
   uint16           EfId;
   UimDFIndexT      DfIndex; 
   uint16           Offset;
   uint8            Len;
} UimReadBinaryMsgT;

/* Define UIM update binary msg command */
typedef PACKED  struct 
{
   ExeRspMsgT       RspInfo;
   uint16           EfId;
   UimDFIndexT      DfIndex; 
   uint16           Offset;
   uint8            Len;
   uint8            Data[1];
} UimUpdateBinaryMsgT;

/************************************
  CHV command message definition 
  ************************************/
/* verify CHV msg */
typedef PACKED struct 
{
    ExeRspMsgT      RspInfo;     /* Requesting task's id, mailbox, msg ID */ 
    uint8           ChvId;        /* Specify the CHV */
    uint8           ChvLen;
    uint8           ChvVal[8];    /* CHV value */
} UimVerifyCHVMsgT;

/* change CHV  msg */
typedef PACKED struct 
{
    ExeRspMsgT      RspInfo;     /* Requesting task's id, mailbox, msg ID */ 
    uint8           ChvId;        /* Specify the CHV */
    uint8           OldChvLen;
    uint8           OldChvVal[8];    /* Old CHV value */
    uint8           NewChvLen;
    uint8           NewChvVal[8];    /* New CHV value */
} UimChangeCHVMsgT;

/*    Disable CHV Msg */
typedef PACKED struct 
{
    ExeRspMsgT      RspInfo;     /* Requesting task's id, mailbox, msg ID */ 
    uint8           ChvLen;
    uint8           Chv1Val[8];    /* CHV1 value */
} UimDisableCHVMsgT;

/*    Enable CHV Msg*/
typedef PACKED struct 
{
    ExeRspMsgT      RspInfo;        /* Requesting task's id, mailbox, msg ID */ 
    uint8           ChvLen;
    uint8           Chv1Val[8];     /* CHV1 value */
} UimEnableCHVMsgT;

/* Unblock CHV Msg */
typedef PACKED struct 
{
    ExeRspMsgT      RspInfo;        /* Requesting task's id, mailbox, msg ID */ 
    uint8           ChvId;          /* Specify the CHV */
    uint8           UblkChvLen;
    uint8           UblkChvVal[8];  /* Unblock CHV value */
    uint8           NewChvLen;
    uint8           NewChvVal[8];   /* New CHV value */
} UimUnblockCHVMsgT;

/***********************************************
                UTK  Commands 
************************************************/
/* UIM Terminal Profile command */
typedef PACKED struct 
{
    ExeRspMsgT RspInfo;  /* Requesting task's id, mailbox, msg ID */ 
    uint8      Profile[TERM_PROFILE_LEN]; /* The list of UIM Application Toolkit 
    facilities that are supported by the ME */
} UimTermProfileMsgT;

/* Define UIM Terminal Response  command */
typedef PACKED  struct 
{
    ExeRspMsgT   RspInfo;
    uint8        Len;  /* Length of response data */
    uint8        Data[1]; /* Response data */
} UimTermRspMsgT;

/* Define UIM Envelope command */
typedef PACKED  struct 
{
    ExeRspMsgT   RspInfo;
    uint8        Len;    /* Length of envelope data */
    uint8        Data[1];/* Envelope data */
} UimEnvelopeMsgT;

/***********************************************
        based OTASP\OPAPA Commands 
  ***********************************************/
typedef PACKED  struct 
{
    ExeRspMsgT   RspInfo;
    uint8        RANDSeed[20];  
    uint8        A_KEY_P_REV;
    uint8        PLen;
    uint8        GLen;
    uint8         PARAM_P[UIM_PARAM_P_LEN];
   uint8           PARAM_G[UIM_PARAM_G_LEN];    
} UimMSKeyMsgT;

typedef PACKED  struct 
{
    ExeRspMsgT   RspInfo;
    uint8        Len;   
    uint8        Result[UIM_MAX_BS_RESULT_LEN];
} UimKeyGenMsgT;

typedef PACKED  struct 
{
    ExeRspMsgT   RspInfo;
} UimCommitMsgT;

typedef PACKED  struct 
{
    ExeRspMsgT   RspInfo;
    uint8        Block;   
    uint8        Len;
    uint8        Data[UIM_MAX_PARAM_SIZE];
} UimValidateMsgT;

typedef PACKED  struct 
{
    ExeRspMsgT   RspInfo;
    uint8        Block;   
} UimConfigurationMsgT;

typedef PACKED  struct 
{
    ExeRspMsgT   RspInfo;
    uint8        Block;   
    uint8        Len;
    uint8        Data[UIM_MAX_PARAM_SIZE];
} UimDownloadMsgT;

typedef PACKED  struct 
{
    ExeRspMsgT   RspInfo;
    uint8        Block;   
    uint16        ReqOffset;
    uint8        ReqMax;	
} UimSSPRConfigurationMsgT;

typedef PACKED  struct 
{
    ExeRspMsgT   RspInfo;
    uint8        Block;   
    uint8        Len;
    uint8        Data[UIM_MAX_OTA_DATA_BURST_SIZE];	
} UimSSPRDownloadMsgT;

typedef PACKED  struct 
{
    ExeRspMsgT   RspInfo;
    uint8        StartStop;
    uint32        RANDSeed;
} UimOTAPAMsgT;

/***********************************************
        ANSI-41-based Security-Related Commands 
  ***********************************************/
/* Base station challenge  msg command */
typedef PACKED struct 
{
    ExeRspMsgT      RspInfo;     /* Requesting task's id, mailbox, msg ID */ 
    uint32          RandSeed;    /* A random number generated by ME  */
    uint8           RandSSD[7];  /* A random number generated by BS, received
                                    in Update SSD message */
    uint8           ProcessCtrl; /* Process control */
    uint32          Esn;         /* Electronic Serial Number */
} UimBSChallengeMsgT;

/* confirm SSD msg command */
typedef PACKED struct 
{
    ExeRspMsgT      RspInfo;     /* Requesting task's id, mailbox, msg ID */ 
    uint32          AuthBS;      /* Value of AUTHBS received in Base Station 
                                    Challenge Confirmation Order */
} UimConfirmSSDMsgT;

/* Run Cave msg command */
typedef PACKED struct 
{
    ExeRspMsgT      RspInfo;     /* Requesting task's id, mailbox, msg ID */ 
    uint8           RandType;    /* Type of RandVal */
    uint32          RandVal;     /* A random number generated by BS ,
                                    Global random challenge or Unique random challenge */
    uint8           DigLen;      /* Length of digit */ 
    uint8           Digit[3];    /* A subset of (coded) dialed digits*/
    uint8           ProcessCtrl; /* Process control */
    uint32          Esn;         /* Electronic Serial Number */
    uint8           CaveSeqId;
    uint16           CaveMsgId;
} UimRunCaveMsgT;

/* Generate Key/VPM msg command */
typedef PACKED struct 
{
    uint8           VPMFirst;    /* First octet of VPM to be output */
    uint8           VPMLast;     /* Last octet of VPM to be output */
    uint8 *         KeyDataP;    /* the pointer to key buffer */
    uint8 *         VPMDataP;    /* the pointer to vpm buffer */
} UimGenerateKeyVPMMsgT;

/* Store ESN_ME */
typedef PACKED struct
{
    ExeRspMsgT      RspInfo;    
    uint8           Len;     
    uint8           Esn[7];
    bool            UsageInd; /* 0 Use ESN, 1 use MEID*/
}UimStoreEsnMeMsgT;

/*******************Related NAM data message************************/
/* Get Nam data from UIM card */
typedef PACKED  struct 
{
    ExeRspMsgT      RspInfo;    
} UimGetNamDataMsgT;

/* NAM structure in UIM */
typedef PACKED struct
{
    uint8           COUNTsp;                    /* Call Count */
    bool            IMSI_M_Valid;    /* IMSI_M    */
    IMSIType        IMSI_Mp;       
    bool            IMSI_T_Valid;    /* IMSI_T    */
    IMSIType        IMSI_Tp;      
    uint8           ASSIGNING_TMSI_ZONE_LENsp;    /* TMSI */
    uint8           ASSIGNING_TMSI_ZONEsp[ CP_MAX_TMSI_ZONE_LEN ];
    uint32          TMSI_CODEsp;    
    uint32          TMSI_EXP_TIMEsp; 
    uint16          HOME_SIDp;            /* Analog Home SID */
    uint8           EXp;                            /* Analog Operational Parameters */
    uint32          NXTREGsp;            /* Analog Location and Registration Indicators */
    uint16          SIDsp;
    uint16          LOCAIDsp;
    bool            PUREGsp;
    uint16          SIDp[MAX_POSITIVE_SIDS]; /* CDMA Home SID, NID */
    uint16          NIDp[MAX_POSITIVE_SIDS];
    uint8           NUM_POSITIVE_SID_NIDp;
    ZoneList        ZONE_LISTsp;            /* CDMA Zone-Based Registration Indicators */
    SidNidList      SID_NID_LISTsp;        /* CDMA System/Network Registration Indicators */
    int32           BASE_LAT_REGsp;     /* CDMA Distance-Based Registration Indicators */
    int32           BASE_LONG_REGsp;
    uint16          REG_DIST_REGsp; 
    uint8           ACCOLCp;                     /* access overload class */
    uint8           MOB_TERM_HOMEp;/* Call Termination Mode Preferences */  
    uint8           MOB_TERM_SIDp;   
    uint8           MOB_TERM_NIDp;     
    uint8           SLOT_CYCLE_INDEXp;/* Suggested Slot Cycle Index */
    uint16          FIRSTCHPp;                /* Analog Channel Preferences */
    uint16          FCCA; 
    uint16          FCCB;
    uint8           NUM_FCC_SCAN;
    bool            ALLOW_OTAPA;    /* OTAPA/SPC_Enable */
    bool            NAM_LOCKp;            /* NAM_LOCK */
    uint8           PREF_SERV_BAND0p;/* Service Preferences */
    uint8           PREF_MODEp;
    uint8           MDN_NUM_DIGITS;    /* EF Mobile Directory Number */
    uint8           Mdn[16];     
    uint32          UimId;    /* UIMID */
    uint8           SF_EUIMID[NAM_MEID_SIZE];
    uint8           CST[NAM_CST_SIZE];
}UimNamT;

/* Response data structure of UIM_GET_NAM_DATA_MSG */
typedef PACKED  struct 
{
    UimNamT         UimNam;    /* Nam data in UIM */ 
} UimGetNamDataRspMsgT;

/* Request the update of NAM */
typedef PACKED struct
{
    ExeRspMsgT      RspInfo;    
    UimNamT         UimNam;
}UimUpdateNamDataMsgT;

/* Response of the update of NAM */
typedef PACKED struct
{
    bool            Ack;
}UimUpdateNamDataRspMsgT;

/******************APP API Related SMS phone book msg************************/
typedef enum              /* phonebook storage */
{
    PHB_STOR_LND = 0x6f44, /*EFLND (Last number dialled)*/
    PHB_STOR_ADN = 0x6f3a, /*EFADN (Abbreviated dialling numbers)*/
    PHB_STOR_FDN = 0x6f3b, /*EFFDN (Fixed dialling numbers)*/
    PHB_STOR_SDN = 0x6f49, /*EFSDN (Service Dialling Numbers)*/
    PHB_STOR_FILE_COUNT  /* File Count */
}UimPhbStorFileT;

/* Request phone book records Params*/
typedef PACKED  struct 
{
    ExeRspMsgT      RspInfo; 
    UimPhbStorFileT FileType;
} UimGetPhbRecParamsMsgT;


/* Response data structure of UIM_GET_PHB_REC_PARAMS_MSG */
typedef PACKED  struct 
{
    bool            Ack;
    UimPhbStorFileT FileType;
    uint8           PhbRecCount;		
    uint8           PhbPerRecSize;
} UimGetPhbRecParamsRspMsgT;


/* Request a record of phone book */
typedef PACKED  struct 
{
    ExeRspMsgT      RspInfo;  
    UimPhbStorFileT FileType;
    uint8           RecordIndex;        
} UimGetPhoneRecMsgT;

/*Response data structure of UIM_GET_PHB_REC_MSG */
typedef PACKED  struct 
{
    bool            Ack;
    UimPhbStorFileT FileType;
    bool            IsFree; 
    uint8           RecordIndex;
    uint8           AlphaIdentifier[MAXALPHALENGTH];
    uint8           PhoneNumber[21];
    uint8           TON;
    uint8           NPI;
} UimGetPhoneRecRspMsgT;


  /* Request the update of a record of phone book */
typedef PACKED  struct 
{
    ExeRspMsgT      RspInfo;
    UimPhbStorFileT FileType;
    uint8           RecordIndex;
    uint8           AlphaIdentifier[MAXALPHALENGTH];
    uint8           PhoneNumber[21];
    uint8           TON;
    uint8           NPI;
} UimUpdatePhoneRecMsgT;


/* Response data of UIM_UPDATE_PHB_REC_MSG */
typedef PACKED  struct 
{
    bool            Ack;
    UimPhbStorFileT FileType;
    uint8           RecordIndex;
} UimUpdatePhoneRecRspMsgT;

/*Request to erase  a record of phone book */
typedef PACKED  struct 
{
    ExeRspMsgT      RspInfo;  
    UimPhbStorFileT FileType;
    uint8           RecordIndex;        
} UimErasePhoneRecMsgT;

/* Response data of UIM_ERASE_PHB_REC_MSG */
typedef PACKED  struct 
{
    bool            Ack;
    UimPhbStorFileT FileType;
    uint8           RecordIndex;
} UimErasePhoneRecRspMsgT;


/* Request SMS(EFSMS) records sum*/
typedef PACKED  struct 
{
    ExeRspMsgT      RspInfo;
} UimGetSmsRecParamsMsgT;


/* Response data structure of UIM_GET_SMS_REC_ PARAMS _MSG */
typedef PACKED  struct 
{
    bool            Ack;
    uint8           SmsRecCount;
    uint8           SmsPerRecSize;
} UimGetSmsRecParamsRspMsgT;


/* Request a record of SMS */
typedef PACKED  struct 
{
    ExeRspMsgT      RspInfo;
    uint8           RecordIndex;
} UimGetSmsRecMsgT;

/*Response data structure of UIM_GET_SMS_REC_MSG*/
typedef PACKED  struct 
{
    bool            Ack;
    uint8           RecordIndex;
    uint8           Status;
    uint8           MsgLen;
    uint8           MsgData[1];     
} UimGetSmsRecRspMsgT;

 /* Request the update of SMS record */
typedef PACKED  struct 
{
    ExeRspMsgT      RspInfo;
    uint8           RecordIndex;
    uint8           Status;
    uint8           MsgLen;
    uint8           MsgData[1];    
} UimUpdateSmsRecMsgT;

 /* Response data of UIM_UPDATE_SMS_REC_MSG*/
typedef PACKED  struct 
{
    bool            Ack;
    uint8           RecordIndex;
} UimUpdateSmsRecRspMsgT;

 /* Request Erase SMS record */
typedef PACKED  struct 
{
    ExeRspMsgT      RspInfo;
    uint8           RecordIndex;
} UimEraseSmsRecMsgT;

 /* Response data of UIM_ERASE_SMS_REC_MSG*/
typedef PACKED  struct 
{
    bool            Ack;
    uint8           RecordIndex;
} UimEraseSmsRecRspMsgT;
/***************End of APP API about PhoneBook and SMS***************/


/*********************************
Get Uim Card status
*********************************/
typedef enum{
   SimStatusDisable  = 0x00,
   SimStatusEnable,
   SimStatusChv1, /*Waiting for correct chv1 entry*/
   SimStatusPuk1, /*Waiting for UNBLOCK1 entry and new PIN1 code*/
   SimStatusDead,
   SimStatusNotCDMA
}UimStatusEnum;

/* Request uim card status */
typedef PACKED  struct 
{
   ExeRspMsgT RspInfo;
} UimCardStatusRecMsgT;

 /* Response to uim card status*/
typedef PACKED  struct 
{
   bool        Ack;
   UimStatusEnum eUimStatus;
   uint8 nSimPin1Count;
   uint8 nSimPin2Count;
   uint8 nSimPuk1Count;
   uint8 nSimPuk2Count;
} UimCardStatusRspMsgT;
 




/************************************
  APP CHV  message definition 
  ************************************/
typedef enum
{
    UIM_CHV_1 = 1,
    UIM_CHV_2
}UimChvIdT;

/* verify CHV msg */
typedef PACKED struct 
{
    ExeRspMsgT      RspInfo;     /* Requesting task's id, mailbox, msg ID */ 
    UimChvIdT       ChvId;        /* Specify the CHV */
    uint8           ChvLen;
    uint8           ChvVal[8];    /* CHV value */
} UimAppVerifyCHVMsgT;

/* change CHV  msg */
typedef PACKED struct 
{
    ExeRspMsgT      RspInfo;     /* Requesting task's id, mailbox, msg ID */ 
    UimChvIdT       ChvId;        /* Specify the CHV */
    uint8           OldChvLen;
    uint8           OldChvVal[8];    /* Old CHV value */
    uint8           NewChvLen;
    uint8           NewChvVal[8];    /* New CHV value */
} UimAppChangeCHVMsgT;

/*    Disable CHV Msg */
typedef PACKED struct 
{
    ExeRspMsgT      RspInfo;     /* Requesting task's id, mailbox, msg ID */ 
    uint8           Chv1Len;
    uint8           Chv1Val[8];  /* CHV1 value */
} UimAppDisableCHVMsgT;

/*    Enable CHV Msg*/
typedef PACKED struct 
{
    ExeRspMsgT      RspInfo;     /* Requesting task's id, mailbox, msg ID */ 
    uint8           Chv1Len;
    uint8           Chv1Val[8];  /* CHV1 value */
} UimAppEnableCHVMsgT;

/* Unblock CHV Msg */
typedef PACKED struct 
{
    ExeRspMsgT      RspInfo;        /* Requesting task's id, mailbox, msg ID */ 
    UimChvIdT       ChvId;          /* Specify the CHV */
    uint8           UblkChvLen;
    uint8           UblkChvVal[8];  /* Unblock CHV value */
    uint8           NewChvLen;
    uint8           NewChvVal[8];   /* New CHV value */
} UimAppUnblockCHVMsgT;

/* Define CHV operation response result */
typedef enum
{
    UIM_CHV_SUCCESS	 =0, 
    UIM_CHV_NOT_INITIALIZE,   /* no CHV initialized */
    UIM_CHV_FAILURE_AND_PERMIT_ATTEMPT, /*  unsuccessful CHV/UNBLOCK CHV verification, at least one attempt left */
    UIM_CHV_CONTRADICTION_WITH_CHV_STATUS, /* in contradiction with CHV status */
    UIM_CHV_CONTRADICTION_WITH_INVALIDATION_STATE, /* in contradiction with invalidation status */
    UIM_CHV_FAILURE_AND_NO_ATTEMPT, /*unsuccessful CHV/UNBLOCK CHV verification, no attempt left;CHV/UNBLOCK CHV blocked*/
    UIM_CHV_FAILURE
}UimChvResultT;

/* APP CHV operation response message */
typedef PACKED struct
{
    UimChvResultT Result;
}UimAppChvRspMsgT;


/************************************
  UIM_MNG_MSG_CLASS  message definition 
 ************************************/
  
/* Register/Deregister  the notify message of UIM status changed */
typedef PACKED struct
{
    ExeRspMsgT  RspInfo;
    bool        Register;
} UimNotifyRegisterMsgT;

/* Uim notify message */
typedef PACKED struct
{
    bool        IsReady;
} UimNotifyMsgT;

typedef PACKED  struct 
{
    ExeRspMsgT      RspInfo;
} UimProactiveRegiserMsgT;

extern void UimSetAuthCmdRspAllow(bool value);

#endif


