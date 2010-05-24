#ifndef VALUTKAPI_H
#define VALUTKAPI_H


/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/
#include "sysdefs.h"
#include "balapi.h"

#ifdef  __cplusplus
extern "C" {
#endif
/*----------------------------------------------------------------------------
 Local Defines and Macros
----------------------------------------------------------------------------*/
#define UTK_ITEM_MAX_COUNT 15
#define UTK_SMS_ADDRESS_LEN 30
#define UTK_SMS_SUBADDRESS_LEN 30


/*----------------------------------------------------------------------------
 Local Typedefs
----------------------------------------------------------------------------*/

/* UTK genernal result type */
typedef enum
{
  BAL_UTK_RST_CMD_OK            = 0x00, /*Command performed successfully*/
  BAL_UTK_RST_CMD_PART_OK,              /*Command performed with partial comprehension*/
  BAL_UTK_RST_CMD_MISS_OK,              /*Command performed, with missing information*/
  BAL_UTK_RST_EF_REFRESH,               /*REFRESH performed with additional EFs read*/
  BAL_UTK_RST_CMD_OK_NO_ICON,           /*Command performed successfully,
                                           but requested icon could not be displayed*/
  BAL_UTK_RST_CMD_OK_LTD_SERV   = 0x06, /*Command performed successfully, limited service*/
  BAL_UTK_RST_CMD_OK_NOTIFY,            /*Command performed with notification*/
  BAL_UTK_RST_REF_NO_NAA,               /*REFRESH performed but indicated NAA was not active*/
 
  BAL_UTK_RST_USER_TERMINATE    = 0x10, /*Proactive UICC session terminated by the user*/
  BAL_UTK_RST_USER_BACK_MOVE,           /*Backward move in the proactive UICC session
                                           requested by the user*/
  BAL_UTK_RST_USER_NO_RSP,              /*No response from user*/
  BAL_UTK_RST_USER_REQ_HELP,            /*Help information required by the user*/
 
  BAL_UTK_RST_ME_UNABLE         = 0x20, /*terminal currently unable to process command*/
  BAL_UTK_RST_NET_UNABLE,               /*Network currently unable to process command*/
  BAL_UTK_RST_USER_NOT_ACCEPT,          /*User did not accept the proactive command*/
  BAL_UTK_RST_USER_CLEAR_CALL,          /*User cleared down call before connection or network release*/
  BAL_UTK_RST_ACTION_CONT,              /*Action in contradiction with the current timer state*/
  BAL_UTK_RST_NAA_CC_TEMP_PROBLEM,      /* Interaction with call control by NAA, temporary problem; */
  BAL_UTK_RST_BROW_ERR,                 /* Launch browser generic error code */

  BAL_UTK_RST_CMD_BYD_CAP       = 0x30, /*Command beyond terminal's capabilities*/
  BAL_UTK_RST_CMD_TYPE_NOT_UND,         /*Command type not understood by terminal*/
  BAL_UTK_RST_CMD_DATA_NOT_UND,         /*Command data not understood by terminal*/
  BAL_UTK_RST_CMD_NUM_UNK,              /*Command number not known by terminal*/
 	 
  BAL_UTK_RST_REQ_BAL_MISS      = 0x36, /*Error, required values are missing*/

  BAL_UTK_RST_NULTICARD_CMD_ERR = 0x38, /* MultipleCard commands error */
  BAL_UTK_RST_NAA_CC_PERM_PROBLEM,      /* Interaction with call control by NAA by NAA, permanent problem */
  BAL_UTK_RST_BEARER_PROTOCOL_ERR,      /* Bearer Independent Protocol error */
  BAL_UTK_RST_ACCESS_TECH_UNABLE        /* Bearer Independent Protocol error */
} ValUtkGenResultT;

/* Addition information on result when UtkGenResult = BAL_UTK_RST_ME_UNABLE */
typedef enum
{
  BAL_INF_NO_CAUSE               = 0x00, /*No specific cause can be given*/
  BAL_UTK_ADD_INF_SCR_BUSY,              /*Screen is busy*/
  BAL_UTK_ADD_INF_BUSY_CALL,             /*terminal currently busy on call*/
  /*0x03 reserved for GSM/3G;*/
  BAL_UTK_ADD_INF_NO_SERVICE     = 0x04, /*No service*/
  BAL_UTK_ADD_INF_ACC_CLASS_BAR,         /*Access control class bar*/
  BAL_UTK_ADD_INF_RADIO_RES_NOT_GRANT,   /*Radio resource not granted*/
  BAL_UTK_ADD_INF_NOT_SPEECH_CALL,       /*Not in speech call*/
  /*0x08 reserved for GSM/3G;*/
  BAL_UTK_ADD_INF_BUSY_SEND_DTMF = 0x09, /*ME currently busy on SEND DTMF command*/
  BAL_UTK_ADD_INF_NO_NAA                 /*No NAA active*/
} ValUtkAddInfoT;

/* UTK result type */
typedef PACKED struct 
{
  ValUtkGenResultT GenResult;/* General result */
  bool             AddInfoPre; /* AddInfo present */
  ValUtkAddInfoT   AddInfo; /* Additional information on result */
} ValUtkResultT;

/*command detail (8.6) */
typedef PACKED struct 
{
  uint8 CmdNum;       /* Command number */   
  uint8 CmdType;      /* Command type */
  uint8 CmdQualifier; /* Command qualifier */
} ValUtkCmdDetailT;

typedef enum
{
  BAL_UTK_CODE_GSM_7BIT_PACKED         = 0, /* SMS default 7-bit coded alphabet, packed into 8-bit octets,;*/
  BAL_UTK_CODE_GSM_7BIT_CODED_UNPACKED = 4, /* SMS default 7-bit coded alphabet with bit 8 set to 0.;*/
  BAL_UTK_CODE_UNICODE_80              = 8, /* 16 bit unicode, formatting character is 0x80*/
  BAL_UTK_CODE_UNICODE_81,                  /* 16 bit unicode, formatting character is 0x81*/
  BAL_UTK_CODE_UNICODE_82                   /* 16 bit unicode, formatting character is 0x82*/
} ValUtkCodeSchemeT;

/* text string(8.15), Alpha identifier (8.2) */
typedef PACKED struct 
{
  ValUtkCodeSchemeT CodeScheme; /*Data coding scheme */
  uint8             Len;        /* length of Text, A null text string shall be coded with Len = 0 */
  uint8*            TextP;
} ValUtkTextT;

/* Item (8.9) */
typedef PACKED struct
{
  uint8             ItemId; /* Identifier of item */
  ValUtkCodeSchemeT CodeScheme;
  uint8             Len;
  uint8*            ItemTextP; /* Item */
} ValUtkItemT;

/* Items Next Action Indicator (8.24) */
typedef PACKED struct
{
  uint8  Len;
  uint8* NaiListP; /* Items Next Action Indicator list */
} ValUtkNaiT;

/* Icon Identifier(8.31)
(at present, icon is not supported, its parameters are not full ) */
typedef enum
{
  UTK_ICON_SELF_EXPLANATORY = 0, /*if displayed, it replaces the item text*/
  UTK_ICON_NOT_SELF_EXPLANATORY /*if displayed, it shall be displayed together with the item text.*/
} ValUtkIconQualiferT;

typedef PACKED struct
{
  ValUtkIconQualiferT IconQualifier; /* Icon qualifier */
  uint8               IconId; /* Icon identifier */
} ValUtkIconT;

/* Item Icon List (8.32) */
typedef PACKED struct
{
  ValUtkIconQualiferT IconQualifier;
  uint8               Len;
  uint8*              IconListIdP;
} ValUtkItemIconListT;

/* Numbering plan identification (NPI) */
typedef enum
{
  UTK_NPI_UNKNOWN = 0,
  UTK_NPI_ISDN_OR_TELEP_NUM_PLAN =1, /* ISDN/telephony numbering plan (ITU-T Recommendations E.164 [22] and E.163 [21]) */
  UTK_NPI_DATA_NUM_PLAN = 3, /*ISDN/telephony numbering plan (ITU-T Recommendations E.164 [22] and E.163 [21])*/
  UTK_NPI_TELEX_NUM_PLAN = 4,/*Telex numbering plan (ITU-T Recommendation F.69 [24]);*/
  UTK_NPI_PRIVATE_NUM_PLAN = 9,/*Private numbering plan;*/
  UTK_NPI_RESERVE_EXTENSION = 15 /*Reserved for extension;*/
    /* All other values are reserved. */
} ValUtkNpiT;

/* Type of number (TON) */
typedef enum
{
  UTK_TON_UNKNOWN = 0,
  UTK_TON_INTERNATIONAL_NUM = 1, /* International Number;*/
  UTK_TON_NATIONAL_NUM = 2, /* national Number; */
  UTK_TON_NETWORK_NUM = 3 /* Network Specific Number; */
  /* All other values are reserved. */
} ValUtkTonT;

/* Address (8.1) */
typedef PACKED struct
{
  ValUtkNpiT Npi;
  ValUtkTonT Ton;
  uint8      Len;
  uint8*     DialNumberP;
} ValUtkAddressT;

/*UTK Set Up Menu Command(6.6.7) */
typedef PACKED struct 
{
  ValUtkCmdDetailT    CmdDetail;
  ValUtkTextT         Alpha;
  uint8               ItemNum;  /* =0, indicates delete existed menu */
  ValUtkItemT         Item[UTK_ITEM_MAX_COUNT];
  bool                NaiPre; /* if Items Next Action Indicator is present, TRUE: present*/
  ValUtkNaiT          Nai;
  bool                IconPre;
  ValUtkIconT         Icon;
  bool                ItemIconListPre;
  ValUtkItemIconListT ItemIconList;
} ValUtkSetUpMenuCmdT;

/*utk select item Command(6.6.8) */
typedef PACKED  struct 
{
    ValUtkCmdDetailT    CmdDetail;
    bool                AlphaPre;
    ValUtkTextT         Alpha;
    uint8               ItemNum;
    ValUtkItemT         Item[UTK_ITEM_MAX_COUNT];
    bool                NaiPre;
    ValUtkNaiT          Nai;
    bool                DefItemIdPre;	
    uint8               DefItemId; /*default selected item */
    bool                IconPre;
    ValUtkIconT         Icon;
    bool                ItemIconListPre;
    ValUtkItemIconListT ItemIconList;
} ValUtkSelectItemCmdT;

/* Capability configuration parameters(8.4) */
typedef PACKED struct
{
  uint8  Len;
  uint8* CapCfgParamP;
} ValUtkCapCfgParamT;

/* Subaddress */
typedef PACKED struct
{
  uint8  Len;
  uint8* SubaddressP;
} ValUtkSubaddressT;

typedef enum
{
  UTK_TIME_UNIT_MINUTES = 0, /* minute; */
  UTK_TIME_UNIT_SECONDS = 1, /* second; */ 
  UTK_TIME_UNIT_TENTH_OF_SECONDS = 2 /* tenths of seconds*/
} ValUtkTimeUnitT;

/* Druation (8.8) */
typedef PACKED struct
{
  ValUtkTimeUnitT TimeUnit;
  uint8           TimeInterval;
} ValUtkDurationT;

/* UTK Set Up call Command */
typedef PACKED  struct 
{
  ValUtkCmdDetailT   CmdDetail;
  bool               ConfirmAlphaPre;
  ValUtkTextT        ConfirmAlpha;/*Alpha identifier (user confirmation phase)*/
  ValUtkAddressT     Address;
  bool               CapCfgParamPre;
  ValUtkCapCfgParamT CapCfgParam;/*Capability configuration parameters*/
  bool               SubaddressPre;
  ValUtkSubaddressT  Subaddress;
  bool               DurationPre;
  ValUtkDurationT    Duration;
  bool               ConfirmIconPre;
  ValUtkIconT        ConfirmIcon;/*Icon identifier (user confirmation phase)*/
  bool               SetupAlphaPre;
  ValUtkTextT        SetupAlpha;/* Alpha identifier (call set up phase) */
  bool               SetupIconPre;
  ValUtkIconT        SetupIcon;/*Icon identifier (call set up phase)*/
} ValUtkSetUpCallCmdT;

/*UTK display text Command */
typedef PACKED  struct 
{
  ValUtkCmdDetailT CmdDetail;
  ValUtkTextT      TextString;
  bool             IconPre;
  ValUtkIconT      Icon;
  bool             ImmediateRsp; /* Immediate response TRUE*/ 
  bool             DurationPre;
  ValUtkDurationT  Duration;
} ValUtkDisplayTextCmdT;

/* Response length (8.11)*/
typedef PACKED struct
{
  uint8 RspLenMin;/* Minimum length of response */
  uint8 RspLenMax;/* Maximum length of response */
} ValUtkRspLenT;/*The range of length is between '00' and 'FF'. A minimum length coding
of '00' indicates that there is no minimum length requirement; a maximum length coding 
of 'FF' indicates that there is no maximum length requirement. If a fixed length is
required the minimum and maximum values are identical.*/

/*UTK get input Command(6.6.3) */
typedef PACKED  struct 
{
  ValUtkCmdDetailT CmdDetail;
  ValUtkTextT      TextString; /*text for the Terminal to display in conjunction with asking the user to respond.*/
  ValUtkRspLenT    RspLen; /* Response length */
  bool             DefaultTextPre; /* if default text is present */
  ValUtkTextT      DefaultText; /*corresponds to a default text string offered by the UIM.*/
  bool             IconPre;
  ValUtkIconT      Icon;
} ValUtkGetInputCmdT;

typedef enum
{
  /* Standard supervisory tones: */
  UTK_TONE_DIAL = 0x01, /* Dial tone */
  UTK_TONE_CALLED_BUSY, /*Called subscriber busy*/
  UTK_TONE_CONGESTION,  /*Congestion*/
  UTK_TONE_RADIO_PATH_ACK, /*Radio path acknowledge;*/
  UTK_TONE_RADIO_PATH_UNAVAIL_OR_CALL_DROP, /*Radio path not available/Call dropped*/
  UTK_TONE_ERROR_OR_SPEC_INFO, /*Error/Special information*/
  UTK_TONE_CALL_WAITING, /*Call waiting tone*/
  UTK_TONE_RINGING, /*Ringing tone*/
  /*Terminal proprietary tones*/
  UTK_TONE_BEEP = 0x10, /*General beep*/
  UTK_TONE_POSITIVE_ACK, /*Positive acknowledgement tone*/
  UTK_TONE_NEG_ACK_OR_ERROR, /*Negative acknowledgement or error tone*/
  UTK_TONE_USER_SELECTED_RINGING, /*Ringing tone as selected by the user for incoming speech call;*/
  UTK_TONE_USER_SELECTED_ALERT /*Alert tone as selected by the user for incoming SMS*/
  /* All other values are reserved. */
} ValUtkToneT;

/*utk play tone Command(6.6.5) */
typedef PACKED  struct 
{
  ValUtkCmdDetailT CmdDetail;
  bool             AlphaPre;
  ValUtkTextT      Alpha;
  bool             TonePre; /*if Tone is present. If no tone is specified, then the Terminal shall default to "general beep"*/
  ValUtkToneT      Tone;
  bool             DurationPre;
  ValUtkDurationT  Duration;
  bool             IconPre;
  ValUtkIconT      Icon;
} ValUtkPlayToneCmdT;


/*utk send sms Command (6.6.9) */
typedef PACKED struct
{
  ValUtkCmdDetailT CmdDetail;
  bool             AlphaPre;
  ValUtkTextT      Alpha;
  bool             AddressPre;
  ValUtkAddressT   Address;
  uint8            SmsLen;   /* Length of SMS TPDU */
  uint8*           SmsTpduP; /* SMS TPDU Buffer */
} ValUtkSendSmsCmdT;

typedef enum
{
  UTK_NOT_SUPPORT, /*UIM card can't support UTK */
  UTK_MSG_MISSING_PARAM,  /* Message received from UIM card misses necessary parameter*/
  UTK_MSG_MISMATCH_LENGTH,/*Length of message received from UIM card is mismatch */
  UTK_MSG_PARAM_ERR,/*Parameter received from UIM card is error */	
  UTK_UIM_CARD_BUSY,/* UIM card UIM tollkit is busy */
  UTK_MSG_SW_ERR,   /* Status word of message received from UIM card is unexpected */
  UTK_SESSION_NORMAL_END /* UTK session is end normally*/
} ValUtkSessionEndCmdT;


/*sms parameters used by sms unit via utk sms download*/
/* message identifier */
typedef PACKED struct
{
  uint8  MsgType;
  uint16 MsgId;
  bool   HeaderInd;
} ValUtkSmsMsgIdT;

/* address */
typedef PACKED struct
{
  bool  DigitMode;
  bool  NumMode;
  uint8 NumType;
  uint8 NumPlan;
  uint8 NumFields;
  uint8 Address[UTK_SMS_ADDRESS_LEN];
} ValUtkSmsAddressT;

/* Subaddress */
typedef PACKED struct
{
  uint8 Type;
  bool  Odd;
  uint8 NumFields;
  uint8 Subaddress[UTK_SMS_SUBADDRESS_LEN];
} ValUtkSmsSubaddressT;
/*sms parameters end */


/* utk events */
typedef enum
{
  UTK_EVENT_DISPLAY_TEXT,
  UTK_EVENT_GET_INPUT,
  UTK_EVENT_PLAY_TONE,
  UTK_EVENT_SETUP_MENU,
  UTK_EVENT_SELECT_ITEM,
  UTK_EVENT_SEND_SMS,
  UTK_EVENT_SETUP_CALL,
  UTK_EVENT_END,
  UTK_MAX_EVENT
} ValUtkEventIdT;

typedef union 
{
  ValUtkDisplayTextCmdT DispTextCmd; 
  ValUtkSetUpMenuCmdT   SetUpMenuCmd;
  ValUtkSetUpCallCmdT   SetUpCallCmd;
  ValUtkGetInputCmdT    GetInputCmd;
  ValUtkSendSmsCmdT     SendSmsCmd;
  ValUtkPlayToneCmdT    PlayToneCmd;
  ValUtkSelectItemCmdT  SelectItemCmd;
  ValUtkSessionEndCmdT  SessionEndCmd;	
} ValUtkEventDataT;

/* Callback to support UI needs to handle UTK events */
typedef void (*UtkEventFunc) 
(
  RegIdT            RegId,
  ValUtkEventIdT    EventId, /* UTK Event which invokes the callback */
  ValUtkEventDataT* EventDataP /* Transmitted data pointer */
);

typedef PACKED struct
{
  bool  SoftKeyForSelItem; /* Soft keys support for SELECT ITEM */
  bool  SoftKeyForSetUpMenu; /* Soft Keys support for SET UP MENU */
  uint8 SoftKeyMaxNum; /* Maximum number of soft keys available,'FF' is reserved */
} ValUtkSoftKeyT;

typedef PACKED struct
{
  uint8 CharNum;/* 5 bits,Number of characters supported down the terminal display*/
  bool  SizingParam; /*Screen Sizing Parameters supported*/
} ValUtkDispHeightT;

typedef PACKED struct
{
  uint8 CharNum;/* 7 bits,Number of characters supported across the terminal display*/
  bool  VarSizeFont;/*Variable size fonts Supported*/
} ValUtkDispWidthT;

typedef PACKED struct
{
  ValUtkDispHeightT DispHightParam;
  ValUtkDispWidthT  DispWidthParam;
  bool              Resized;    /*Display can be resized*/
  bool              TextWrap;   /*Text Wrapping supported*/
  bool              TextScroll; /*Text Scrolling supported*/
  bool              ExtText;    /*Extended text supported */
  uint8             WidthReduction; /* 3 bits, Width reduction when in a menu, this value
                                       is the number of characters available across the
                                       display due to a DISPLAY TEXT proactive 
                                       command without scrolling (using the default character
                                       set specified in TS 123 038 [3]) minus the number of
                                       characters available across the display due 
                                       to a SELECT ITEM proactive command without scrolling */
} ValUtkDispParamT;

/*****************************************************************************

  FUNCTION NAME: UtkRegForEvents

  DESCRIPTION:

    Other task invokes this function to register event functions in UTK unit.

  PARAMETERS:
    EventFunc: Callback function
	
  RETURNED VALUES:

    TRegister ID
    -1: failed.

*****************************************************************************/
RegIdT ValUtkRegister( UtkEventFunc EventFunc );

/*****************************************************************************

  FUNCTION NAME: UtkUnregForEvents

  DESCRIPTION:

    Other task invokes this function to unregister event functions in UTK unit.

  PARAMETERS:
    RegId: Assigned Register ID
	
  RETURNED VALUES:

    None

*****************************************************************************/
void ValUtkUnregister( RegIdT RegId );


/*****************************************************************************

  FUNCTION NAME: ValUtkProfileInit

  DESCRIPTION:

  	UTK initialization.  UIM card revision is obtained to determine whether or not UIM card 
  	supports UTK functionality. After sending read message to UIM, UTK init  returns. Other
  	operations (e.g. read image file and sending Terminal Profile command )	will be done 
  	after receiving response from UIM.
 
  PARAMETERS:

  	DispParamP: Display parameter pointer
  	SoftKeyP:   Soft key parameter poniter
 	  used to construct UTK Profile 

  RETURNED VALUES:

	None.
	
*****************************************************************************/
void ValUtkProfileInit( ValUtkDispParamT* DispParamP,
                        ValUtkSoftKeyT*   SoftKeyP );

/*****************************************************************************

  FUNCTION NAME: ValUtkExit

  DESCRIPTION:
   
    User is waiting the further utk command after user has selected utk menu. At that 
    time, user presses exit key to exit utk session, UI invokes this function to exit utk.  

  PARAMETERS:

    None.
 	
  RETURNED VALUES:

    None
	
*****************************************************************************/
void ValUtkExit( void );

/*****************************************************************************

  FUNCTION NAME: ValUtkSmsDownload

  DESCRIPTION:

    handles SMS-PP data download Command received from the network.  

  PARAMETERS:

    ApduP: utk sms transport layer buffer
    Len: length of sms transport layer buffer
    OrigAddressP: original address buffer (utk uses it when sending ack)
    OrigSubaddressP: original subaddress buffer   NULL: absent  (utk uses it when sending ack)
    BearerReplyOption: REPLY_SEQ of Bearer replay option parameter (utk uses it when sending ack)
    MsgIdP: Message Identifier buffer (utk uses it when sending ack)
 	
  RETURNED VALUES:

    None

*****************************************************************************/
void ValUtkSmsDownload( uint8*                ApduP,
                        uint8                 Len,
                        ValUtkSmsAddressT*    OrigAddressP, 
                        ValUtkSmsSubaddressT* OrigSubaddressP,
                        uint16                MsgId );

/*****************************************************************************

  FUNCTION NAME: ValUtkSelectMenu

  DESCRIPTION:

    handles Selection Menu Command received from UI.  

  PARAMETERS:

    ItemId : item identifier
    help: if help is needed, TRUE: needed
 	
  RETURNED VALUES:

    None

*****************************************************************************/
void ValUtkSelectMenu( uint8 ItemId,
                       bool  Help );

/*****************************************************************************

  FUNCTION NAME: ValUtkCommCmdResult

  DESCRIPTION:

    If UIM sends command to UI, UI just has result value, no other return value, uses this function.

  PARAMETERS:
  
    CmdDetailP: Command detail, if it is NULL, the UTK filled it with the last command detail
    ResultP: Result
	
  RETURNED VALUES:

    None

*****************************************************************************/
void ValUtkCommCmdResult( ValUtkCmdDetailT* CmdDetailP,
                          ValUtkResultT*    ResultP );

/*****************************************************************************

  FUNCTION NAME: ValUtkGetInputDone

  DESCRIPTION:

    If UIM sends get input command to UI, UI uses this function to return text string to UIM.

  PARAMETERS:
  
    CmdDetailP: Command detail, if it is NULL, the UTK filled it with the last command detail
    TextStrP: input text by the user
	
  RETURNED VALUES:

    None

*****************************************************************************/
void ValUtkGetInputDone( ValUtkCmdDetailT* CmdDetailP,
                         ValUtkTextT*      TextStrP );

/*****************************************************************************

  FUNCTION NAME: UtkSelectItemDone

  DESCRIPTION:

    if UIM sends select item command to UI, UI uses this function to return item to UIM.

  PARAMETERS:
  
    CmdDetailP: Command detail, if it is NULL, the UTK filled it with the last command detail
    ItemId: Identifier of item chosen
    Help:  if user requests the help information for this item
	
  RETURNED VALUES:

    None

*****************************************************************************/
void ValUtkSelectItemDone( ValUtkCmdDetailT* CmdDetailP,
                           uint8             ItemId,
                           bool              Help );

/*****************************************************************************

  FUNCTION NAME: ValUtkInit

  DESCRIPTION:

    Create UtkEventsLock. this function is invoked by valtask

  PARAMETERS:

    None.

  RETURNED VALUES:

    None

*****************************************************************************/
 void ValUtkInit( void );

/*****************************************************************************

  FUNCTION NAME: UtkDeliverMail

  DESCRIPTION:

    Delivers UTK response message.  It is used by valtask.

  PARAMETERS:

    MsgId: received message id   
    MsgP: message
    MsgSize: message size	

  RETURNED VALUES:

    None

*****************************************************************************/
void ValUtkDeliverMsg( uint32 MsgId,
                       void*  MsgP,
                       uint32 MsgSize );

extern UtkEventFunc CallbackFunc;
extern RegIdT RegId;

#ifdef  __cplusplus
}
#endif
#endif /* VALUTKAPI_H */





