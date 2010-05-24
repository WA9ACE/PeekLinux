
#ifndef BALSMSAPI_H
#define BALSMSAPI_H
#ifdef __cplusplus
    extern "C" {
#endif


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "sysdefs.h"
#include "balapi.h"

/*===========================================================================

                            DECLARATIONS

===========================================================================*/

#define BAL_SMS_MAX_ADDRESS_LEN              32
#define BAL_SMS_MAX_SUBADDR_LEN              30
#define BAL_SMS_MAX_USER_DATA_LEN           255 /* changed from 200 to 255 in attempt to fix Bug 1007 (F.Hou) */
#define BAL_SMS_CALLBACK_NUMBER_MAX_DIGITS   30
#define BAL_SMS_MAX_CATEGORIES               10
#define BAL_SMS_MAX_CHAR_IN_CATEGORY         20
#define BAL_SMS_TL_MAX_MSG_LENGTH           255   /* teleservice layer pdu max len */
#define BAL_SMS_MAX_NUM_DATA_MSGS             2

#define BAL_SMS_MAX_REGISTERED_IDS            5
#define BAL_SMS_TELESRVID_COUNT              20

#define BAL_SMS_MAX_SMS_PDU_LEN             253

#define BAL_SMS_GHRC_UDH_LEN    7

typedef enum
{
  BAL_SMS_EVENT_INCOMING_MESSAGE       = 0x00000001,
  BAL_SMS_EVENT_SEND_STATUS_MESSAGE,
  BAL_SMS_EVENT_MEM_STATUS_MESSAGE,
  BAL_SMS_EVENT_SMS_WRITE_ERR_MESSAGE,     /*incoming msg write error event*/
  BAL_SMS_EVENT_SMS_SRV_READY_MESSAGE,     /*the vsms service is ready*/
  BAL_SMS_EVENT_SMS_RECEIVED_BUT_MEM_FULL, /*received a msg, but memory full, so the msg lost*/
  BAL_SMS_MAX_EVENTS
} BalSmsEventIdT;

/* val sms sending info */
typedef PACKED struct
{
  bool   Acked;
  uint16 MoSeqNum;
  uint16 StoredRecId;
} BalSmsSendInfoT;

/* val message type for sms uint */
typedef PACKED struct
{
  BalDeviceT device;
  bool       MemStatusFull;      /*TRUE for memory full, FALSE for memory enough */
} BalSmsMemStatMsgT;

/* SMS callback function structure */
typedef void (*BalSmsEventFunc) ( RegIdT         RegId,
                                  BalSmsEventIdT Event,
                                  void*          MsgP );

/* SMS Event register table structure */
typedef struct
{
  bool            IsUse;            /* if this entry is in use */
  uint8           NumTeleSrvId;     /* Teleservice Id count */
  uint16          TeleSrvId[BAL_SMS_TELESRVID_COUNT];
  BalSmsEventFunc CallBack;         /* function to call back */
} BalSmsRegTableT;

/*BAL SMS general result type   */
typedef enum
{
   BAL_SMS_SUCCESS = 0,
  BAL_SMS_ERR_SMS_NOT_READY,
  BAL_SMS_ERR_NO_MEMORY,
  BAL_SMS_ERR_MSG_FORMAT,
  BAL_SMS_ERR_INVALID_PARAMETER,
  BAL_SMS_SENT_BUT_SAVE_FAILED,
    /*sms message storage error can declare hereafter*/
  BAL_SMS_ERR_FAILED,
  BAL_SMS_ERR_BUSY_SENDING, /* while a register application send a sms message while the previous was not acknowleged*/
  BAL_SMS_ERR_DESTINATION_BUSY,
	
   BAL_SMS_ERR_NOT_PARA_INIT,		/* val SMS parameters not init(BalSMSInit has not been called) */
   
   BAL_SMS_ERR_DEV_UNSUPPORT,		/* unsupport SMS device,error device id */
   BAL_SMS_ERR_DEV_REG,				/* SMS device register error, register device is full */
   BAL_SMS_ERR_DEV_ALREADY_REG,		/* SMS device already register */
   BAL_SMS_ERR_DEV_NOT_REG,			/* SMS device not register */
   BAL_SMS_ERR_DEV_NOT_OPEN,		/* SMS device not open */
   BAL_SMS_ERR_DEV_ALREADY_OPEN,	/* SMS device already open */
   BAL_SMS_ERR_DEV_OPEN,			/* error when open SMS device open */
   BAL_SMS_ERR_DEV_CLOSE,			/* error when close SMS device failure */
   BAL_SMS_ERR_DEV_INIT_BUSY,		/* SMS device initial busy */
   
   BAL_SMS_ERR_REC_EMPTY,			/* empty record of SMS device */
   BAL_SMS_ERR_REC_READ,			/* read record  of SMS device failure */
   BAL_SMS_ERR_REC_WRITE,			/* write record of SMS device failure */
   BAL_SMS_ERR_REC_DELETE,			/* delete record of SMS device failure */
   
   BAL_SMS_ERR_NO_BAL_MEMORY,		/* val malloc failure */	
   BAL_SMS_ERR_NO_ENOUGH_MEMORY,	/* no enough memory for new SMS record */

   BAL_SMS_ERR_SEG_EMPTY,			/* not exist such segment */
  BAL_SMS_MAX_RESULT
} BalSmsResultT;

/*sms message type*/
typedef enum
{
  BAL_SMS_SMS_MSG_TYPE_POINT_TO_POINT = 0,
  BAL_SMS_SMS_MSG_TYPE_BROADCAST,
  BAL_SMS_SMS_MSG_TYPE_ACKNOWLEDGE,
  BAL_SMS_MAX_SMS_MSG_TYPE
} BalSmsTransMsgTypeT;

/* Macro to validate BalSmsMsgTypeT */
#define IS_VALID_VSMS_MSG_TYPE(a)  ((BAL_SMS_SMS_MSG_TYPE_POINT_TO_POINT == (a)) ||     \
                                     ((BAL_SMS_SMS_MSG_TYPE_POINT_TO_POINT < (a))    && \
                                    ((a) < BAL_SMS_MAX_SMS_MSG_TYPE)))

typedef enum
{
  BAL_SMS_ERRCLASS_NO_ERROR,
  BAL_SMS_ERRCLASS_TEMPORARY = 2,
  BAL_SMS_ERRCLASS_PERMANENT = 3,
  BAL_SMS_MAX_ERRCLASS
} BalSmsErrClassT;

/* Macro to validate  BalSmsErrClassT */
#define IS_VALID_SMS_ERR_CLASS(a)         \
     ((BAL_SMS_ERRCLASS_NO_ERROR == (a)) || \
         ((BAL_SMS_ERRCLASS_TEMPORARY <= (a)) && ((a) < BAL_SMS_MAX_ERRCLASS)))

#define IS_BAL_SMS_ERR_CLASS(a)      \
    ((BAL_SMS_ERRCLASS_TEMPORARY <= (a)) && ((a) < BAL_SMS_MAX_ERRCLASS))


#define IS_BAL_SMS_NO_ERR_CLASS(a)  (BAL_SMS_ERRCLASS_NO_ERROR == (a))

typedef enum
{
  BAL_SMS_NO_TERMINAL_PROBLEMS,
  BAL_SMS_DESTINATION_RESOURSE_SHORTAGE,
  BAL_SMS_DESTINATION_OUT_OF_SERVICE,
  BAL_SMS_MAX_TERM_STATUS
} BalSmsTermStatusT;

#define IS_VALID_SMS_TERM_STATUS(a) \
    ((BAL_SMS_NO_TERMINAL_PROBLEMS <= (a)) && ((a) < BAL_SMS_MAX_TERM_STATUS))

typedef enum
{
  BAL_SMS_CC_ADDRESS_VACANT = 0,
  BAL_SMS_CC_ADDRESS_TRANSLATION_FAILURE,
  BAL_SMS_CC_NETWORK_RESOURCE_SHORTAGE,
  BAL_SMS_CC_NETWORK_FAILURE,
  BAL_SMS_CC_INVALID_TELESERVICE_ID,
  BAL_SMS_CC_OTHER_NETWORK_PROBLEM,
  BAL_SMS_CC_NO_PAGE_RESPONSE_S = 32,
  BAL_SMS_CC_DESTINATION_BUSY,
  BAL_SMS_CC_NO_ACKNOWLEDGEMENT,
  BAL_SMS_CC_DESTINATION_RESOURCE_SHORTAGE,
  BAL_SMS_CC_SMS_DELIVERY_POSTPONED,
  BAL_SMS_CC_DESTINATION_OUT_OF_SERVICE,
  BAL_SMS_CC_DESTINATION_NO_LONGER_AT_THIS_ADDRESS,
  BAL_SMS_CC_OTHER_TERMINAL_PROBLEM,
  BAL_SMS_CC_RADIO_INTERFACE_RESOURCE_SHORTAGE     = 64,
  BAL_SMS_CC_RADIO_INTERFACE_INCOMPATIBILITY,
  BAL_SMS_CC_OTHER_RADIO_INTERFACE_PROBLEM,
  BAL_SMS_CC_ENCODING_PROBLEM                      = 96,
  BAL_SMS_CC_SMS_ORIGINATION_DENIED,
  BAL_SMS_CC_SMS_TERMINATION_DENIED,
  BAL_SMS_CC_SUPPLEMENTARY_SERVICE_NOT_SUPPORTED,
  BAL_SMS_CC_SMS_NOT_SUPPORTED,
  BAL_SMS_CC_MISSING_EXPECTED_PARAMETER            = 102,
  BAL_SMS_CC_MISSING_MANDATORY_PARAMETER,
  BAL_SMS_CC_UNRECOGNIZED_PARAMETER_VALUE,
  BAL_SMS_CC_UNEXPECTED_PARAMETER_VALUE,
  BAL_SMS_CC_USER_DATA_SIZE_ERROR,
  BAL_SMS_CC_OTHER_GENERAL_PROBLEMS,
  BAL_SMS_MAX_CC
} BalSmsCCT;

/* Macro for validating BalSmsCauseCodeT */
#define IS_VALID_SMS_CAUSE_CODE(a)                              \
     (((BAL_SMS_CC_ADDRESS_VACANT == (a)) || ((BAL_SMS_CC_ADDRESS_VACANT <(a)) &&                    \
                                         ((a) <= BAL_SMS_CC_OTHER_NETWORK_PROBLEM))) || \
      ((BAL_SMS_CC_NO_PAGE_RESPONSE_S <= (a)) &&                \
                                       ((a) <= BAL_SMS_CC_OTHER_TERMINAL_PROBLEM))  || \
      ((BAL_SMS_CC_RADIO_INTERFACE_RESOURCE_SHORTAGE <= (a)) && \
                                             ((a) <= BAL_SMS_CC_SMS_NOT_SUPPORTED)) || \
      ((BAL_SMS_CC_MISSING_EXPECTED_PARAMETER <= (a)) && ((a) < BAL_SMS_MAX_CC)))


typedef enum
{
  BAL_SMS_DIGIT_MODE_4_BIT   = 0,
  BAL_SMS_DIGIT_MODE_8_BIT,
  BAL_SMS_MAX_DIGIT_MODE
} BalSmsDigitModeT;

/* Macro for validating BAL_SMS digit mode type */
#define IS_VALID_SMS_DIGIT_MODE(a)  \
          ((BAL_SMS_DIGIT_MODE_4_BIT ==(a))|| ((BAL_SMS_DIGIT_MODE_4_BIT <(a)) && ((a) < BAL_SMS_MAX_DIGIT_MODE)))


typedef enum
{
  BAL_SMS_NUMBER_MODE_ANSI_T1_607 = 0,
  BAL_SMS_NUMBER_MODE_DATA_NETWORK,
  BAL_SMS_MAX_NUMBER_MODE
} BalSmsNumModeT;

/* Macro for validating vsms number mode type */
#define IS_VALID_SMS_NUMBER_MODE(a)  \
        ( (BAL_SMS_NUMBER_MODE_ANSI_T1_607 == (a)) || ((BAL_SMS_NUMBER_MODE_ANSI_T1_607 <(a)) && ((a) < BAL_SMS_MAX_NUMBER_MODE)))


typedef enum
{
  BAL_SMS_NUMBER_TYPE_UNKNOWN = 0,
  BAL_SMS_NUMBER_TYPE_INTERNATIONAL,
  BAL_SMS_NUMBER_TYPE_NATIONAL,
  BAL_SMS_NUMBER_TYPE_NETWORK_SPECIFIC,
  BAL_SMS_NUMBER_TYPE_SUBSCRIBER,
  BAL_SMS_NUMBER_TYPE_RESERVED,
  BAL_SMS_NUMBER_TYPE_ABBREVIATED,
  BAL_SMS_MAX_NUMBER_TYPE
} BalSmsNumberT;

/* Macro for validating BalSmsNumberT */
#define IS_VALID_SMS_NUMBER_TYPE(a)  \
         ((BAL_SMS_NUMBER_TYPE_UNKNOWN == (a)) || ((BAL_SMS_NUMBER_TYPE_UNKNOWN <(a)) && ((a) < BAL_SMS_MAX_NUMBER_TYPE)))


typedef enum
{
  BAL_SMS_NUMBER_PLAN_UNKNOWN = 0,
  BAL_SMS_NUMBER_PLAN_TELEPHONY,
  BAL_SMS_NUMBER_PLAN_DATA = 3,
  BAL_SMS_NUMBER_PLAN_TELEX,
  BAL_SMS_NUMBER_PLAN_PRIVATE = 9,
  BAL_SMS_MAX_NUMBER_PLAN
} BalSmsNumPlanT;

/* Macro for validating BalSmsNumPlanT */
#define IS_VALID_SMS_NUMBER_PLAN(a)  \
         ((BAL_SMS_NUMBER_PLAN_UNKNOWN == (a)) || ((BAL_SMS_NUMBER_PLAN_UNKNOWN <(a)) && ((a) < BAL_SMS_MAX_NUMBER_PLAN)))


typedef enum
{
  BAL_SMS_DATA_NUM_TYPE_UNKNOWN = 0,
  BAL_SMS_DATA_NUM_TYPE_RFC_791,       /* Internet Protocol */
  BAL_SMS_DATA_NUM_TYPE_RFC_822,       /* Internet Email Address */
  BAL_SMS_MAX_DATA_NUM_TYPE
} BalSmsDataNumberT;

/* Macro for validating BalSmsDataNumberT */
#define IS_VALID_SMS_DATA_NUMBER_TYPE(a)  \
         ((BAL_SMS_DATA_NUM_TYPE_UNKNOWN == (a)) ||((BAL_SMS_DATA_NUM_TYPE_UNKNOWN <(a)) && ((a) < BAL_SMS_MAX_DATA_NUM_TYPE)))


typedef enum
{
  BAL_SMS_SUBADDR_TYPE_NSAP    = 0,
  BAL_SMS_SUBADDR_TYPE_USER,
  BAL_SMS_MAX_SUBADDR_TYPE
} BalSmsSubAddrT;

/* Macro for validating BalSmsSubAddrT */
#define IS_VALID_SMS_SUBADDR_TYPE(a)  \
         ((BAL_SMS_SUBADDR_TYPE_NSAP ==(a)) || ((BAL_SMS_SUBADDR_TYPE_NSAP < (a)) && ((a) < BAL_SMS_MAX_SUBADDR_TYPE)))


typedef enum
{
  BAL_SMS_SVR_CAT_UNKNOWN = 0,
  BAL_SMS_SVR_CAT_EMERGENCIES,
  BAL_SMS_SVR_CAT_ADMINISTRATIVE,
  BAL_SMS_SVR_CAT_MAINTENANCE,
  BAL_SMS_SVR_CAT_GEN_NEWS_LOCAL,
  BAL_SMS_SVR_CAT_GEN_NEWS_REGIONAL,
  BAL_SMS_SVR_CAT_GEN_NEWS_NATIONAL,
  BAL_SMS_SVR_CAT_GEN_NEWS_INTERNATIONAL,
  BAL_SMS_SVR_CAT_FINANCE_NEWS_LOCAL,
  BAL_SMS_SVR_CAT_FINANCE_NEWS_REGIONAL,
  BAL_SMS_SVR_CAT_FINANCE_NEWS_NATIONAL,
  BAL_SMS_SVR_CAT_FINANCE_NEWS_INTERNATIONAL,
  BAL_SMS_SVR_CAT_SPORTS_NEWS_LOCAL,
  BAL_SMS_SVR_CAT_SPORTS_NEWS_REGIONAL,
  BAL_SMS_SVR_CAT_SPORTS_NEWS_NATIONAL,
  BAL_SMS_SVR_CAT_SPORTS_NEWS_INTERNATIONAL,
  BAL_SMS_SVR_CAT_ENTERTAINMENT_NEWS_LOCAL,
  BAL_SMS_SVR_CAT_ENTERTAINMENT_NEWS_REGIONAL,
  BAL_SMS_SVR_CAT_ENTERTAINMENT_NEWS_NATIONAL,
  BAL_SMS_SVR_CAT_ENTERTAINMENT_NEWS_INTERNATIONAL,
  BAL_SMS_SVR_CAT_LOCAL_WEATHER,
  BAL_SMS_SVR_CAT_TRAFFIC,
  BAL_SMS_SVR_CAT_FLIGHT_SCHEDULE,
  BAL_SMS_SVR_CAT_RESTAURANTS,
  BAL_SMS_SVR_CAT_LODGINGS,
  BAL_SMS_SVR_CAT_RETAIL_DIRECTORY,
  BAL_SMS_SVR_CAT_ADVERTISEMENT,
  BAL_SMS_SVR_CAT_STOCK_QUOTES,
  BAL_SMS_SVR_CAT_EMPLOYMENT_OPPORTUNITIES,
  BAL_SMS_SVR_CAT_MEDICAL_HEALTH,
  BAL_SMS_SVR_CAT_TECH_NEWS,
  BAL_SMS_SVR_CAT_MULTI_CATEGORY,
  BAL_SMS_MAX_SVR_CAT
} BalSmsSrvCatT;

/* Macro for validating BalSmsSrvCatT */
#define IS_VALID_SMS_SERVICE_CAT(a)  \
         ((BAL_SMS_SVR_CAT_UNKNOWN == (a)) || ((BAL_SMS_SVR_CAT_UNKNOWN <(a)) && ((a) < BAL_SMS_MAX_SVR_CAT)))


typedef enum
{
  /*----------------------------------------------------------------
     The following are not defined as a teleservice in IS-637,
     but they are used to indicate the teleservice type in Analog mode.
  -----------------------------------------------------------------*/
  BAL_SMS_TS_ID_IS91_PAGE        = 0,
  BAL_SMS_TS_ID_IS91_VOICE_MAIL      = 1,
  BAL_SMS_TS_ID_IS91_SHORT_MESSAGE  = 2,

  /*----------------------------------------------------------------
     Voice mail notification through Message Waiting Indication in
     CDMA mode or Analog mode
  -----------------------------------------------------------------*/
  BAL_SMS_TS_ID_VOICE_MAIL_MWI      = 3,

  /*----------------------------------------------------------------
   IS-637 Teleservices
  -----------------------------------------------------------------*/
  BAL_SMS_TS_ID_IS91        = 4096,
  BAL_SMS_TS_ID_PAGE        = 4097,
  BAL_SMS_TS_ID_MESSAGE     = 4098,
  BAL_SMS_TS_ID_VOICE_MAIL  = 4099,
  BAL_SMS_TS_ID_WAP,
  BAL_SMS_TS_ID_WEMT ,        /*add for EMS*/
  BAL_SMS_TS_ID_SCPT,         /*add for SCPT*/
  BAL_SMS_TS_ID_CATPT,       /*add for UIM card application*/
  BAL_SMS_BROAD_CAST_SRV,     /*add for the broadcast service*/
  BAL_SMS_MAX_TS_ID
} BalSmsTeleSrvIdT;

/* Macro for validating vsms teleservice id type */
#define IS_VALID_SMS_TELESERVICE_ID(a)  \
         ((BAL_SMS_TS_ID_IS91_PAGE == (a)) || ((BAL_SMS_TS_ID_IS91_PAGE < (a)) && ((a) < BAL_SMS_MAX_TS_ID)))


typedef enum
{
  BAL_SMS_MSG_TYPE_RESERVED               = 0,
  BAL_SMS_MSG_TYPE_TERMINATION_DELIVER,
  BAL_SMS_MSG_TYPE_ORIGINATION_SUBMIT,
  BAL_SMS_MSG_TYPE_ORIGINATION_CANCELLATION,
  BAL_SMS_MSG_TYPE_TERMINATION_DELIVERY_ACK,
  BAL_SMS_MSG_TYPE_USER_ACK,
  BAL_SMS_MSG_TYPE_READ_ACK,
  BAL_SMS_MAX_MSG_TYPE
} BalSmsTeleMsgT;

/* Macro for validating vsms msg type */
#define IS_VALID_SMS_MSG(a)  \
         ((BAL_SMS_MSG_TYPE_RESERVED == (a)) || ((BAL_SMS_MSG_TYPE_RESERVED < (a)) && ((a) < BAL_SMS_MAX_MSG_TYPE)))


typedef enum
{
  BAL_SMS_MSG_ENCODE_OCTET              = 0,
  BAL_SMS_MSG_ENCODE_IS91_EXT_PROT_MSG,
  BAL_SMS_MSG_ENCODE_7BIT_ASCII,
  BAL_SMS_MSG_ENCODE_IA5,
  BAL_SMS_MSG_ENCODE_UNICODE,
  BAL_SMS_MSG_ENCODE_SHIFT_JIS,
  BAL_SMS_MSG_ENCODE_KS_C_5601,
  BAL_SMS_MSG_ENCODE_LATIN_HEBREW,
  BAL_SMS_MSG_ENCODE_LATIN,
  BAL_SMS_MAX_MSG_ENCODE
} BalSmsMsgEncodeT;

/* Macro for validating vsms message encode type */
#define IS_VALID_SMS_MSG_ENCODE(a)  \
         ((BAL_SMS_MSG_ENCODE_OCTET == (a)) || ((BAL_SMS_MSG_ENCODE_OCTET <(a)) && ((a) < BAL_SMS_MAX_MSG_ENCODE)))


typedef enum
{
  BAL_SMS_PRIORITY_NORMAL      = 0,
  BAL_SMS_PRIORITY_INTERACTIVE,
  BAL_SMS_PRIORITY_URGENT,
  BAL_SMS_PRIORITY_EMERGENCY,
  BAL_SMS_MAX_PRIORITY
} BalSmsPriorityT;

/* Macro for validating vsms priority type */
#define IS_VALID_SMS_PRIORITY(a)  \
         ((BAL_SMS_PRIORITY_NORMAL == (a)) ||((BAL_SMS_PRIORITY_NORMAL <(a)) && ((a) < BAL_SMS_MAX_PRIORITY)))


typedef enum
{
  BAL_SMS_PRIVACY_NOT_RESTRICTED = 0,
  BAL_SMS_PRIVACY_RESTRICTED,
  BAL_SMS_PRIVACY_CONFIDENTIAL,
  BAL_SMS_PRIVACY_SECRET,
  BAL_SMS_MAX_PRIVACY
} BalSmsPrivacyT;

/* Macro for validating vsms privacy type */
#define IS_VALID_SMS_PRIVACY(a)  \
         ((BAL_SMS_PRIVACY_NOT_RESTRICTED == (a)) ||((BAL_SMS_PRIVACY_NOT_RESTRICTED <(a)) && ((a) < BAL_SMS_MAX_ERRCLASS)))


typedef enum
{
  BAL_SMS_ALERT_MOBILE_DEFAULT = 0,
  BAL_SMS_ALERT_LOW_PRIORITY,
  BAL_SMS_ALERT_MEDIUM_PRIORITY,
  BAL_SMS_ALERT_HIGH_PRIORITY,
  BAL_SMS_MAX_ALERT
} BalSmsAlertT;

/* Macro for validating vsms alert type */
#define IS_VALID_SMS_ALERT(a)                   \
         ((BAL_SMS_ALERT_MOBILE_DEFAULT == (a)) ||  \
          ((BAL_SMS_ALERT_MOBILE_DEFAULT < (a)) && ((a) < BAL_SMS_MAX_ALERT)))


typedef enum
{
  BAL_SMS_LANGUAGE_UNKNOWN = 0,
  BAL_SMS_LANGUAGE_ENGLISH,
  BAL_SMS_LANGUAGE_FRENCH,
  BAL_SMS_LANGUAGE_SPANISH,
  BAL_SMS_LANGUAGE_JAPANESE,
  BAL_SMS_LANGUAGE_KOREAN,
  BAL_SMS_LANGUAGE_CHINESE,
  BAL_SMS_LANGUAGE_HEBREW,
  BAL_SMS_MAX_LANGUAGE
} BalSmsLanguageT;

/* Macro for validating vsms language type */
#define IS_VALID_SMS_LANGUAGE(a)              \
         ((BAL_SMS_LANGUAGE_UNKNOWN == (a)) ||    \
          ((BAL_SMS_LANGUAGE_UNKNOWN <(a)) && ((a) < BAL_SMS_MAX_LANGUAGE)))


typedef enum
{
  BAL_SMS_DISP_MODE_IMMEDIATE = 0,
  BAL_SMS_DISP_MODE_DEFAULT,
  BAL_SMS_DISP_MODE_USER_INVOKE,
  BAL_SMS_DISP_MODE_RESERVED,
  BAL_SMS_MAX_DISP_MODE
} BalSmsDisplayModeT;

/* Macro for validating vsms mssage display mode type */
#define IS_VALID_SMS_MSG_DISP_MODE(a)           \
         ((BAL_SMS_DISP_MODE_IMMEDIATE == (a)) ||  \
          ((BAL_SMS_DISP_MODE_IMMEDIATE <(a)) && ((a) < BAL_SMS_MAX_DISP_MODE)))


typedef PACKED struct
{
  BalSmsDisplayModeT SmsDispMode;
  uint8              Reserved;
} BalSmsDispModeT;

typedef PACKED struct
{
  BalSmsDigitModeT  DigitMode;
  BalSmsNumberT     NumberType;
  BalSmsNumPlanT    NumberPlan;
  uint8             NumFields;
  uint8             Digits[BAL_SMS_CALLBACK_NUMBER_MAX_DIGITS];
} BalSmsCbNumberT;


typedef   struct
{
  BalSmsDigitModeT     DigitMode;
  BalSmsNumModeT       NumberMode;
  union
  {
    BalSmsNumberT      NumType;
    BalSmsDataNumberT  DataNumType;
  } NumberType;
  BalSmsNumPlanT       NumberPlan;
  uint8                NumFields;
  uint8                Address[BAL_SMS_MAX_ADDRESS_LEN];
} BalSmsAddressT;


typedef PACKED  struct
{
  BalSmsSubAddrT     SubaddrType;
  uint8              Odd;
  uint8              NumFields;
  uint8              Address[BAL_SMS_MAX_SUBADDR_LEN];
} BalSmsSubaddressT;


typedef PACKED  struct
{
  BalSmsMsgEncodeT   MessageEncoding;
  uint8              MessageType;
  uint8              NumFields;
  uint8              Data[BAL_SMS_MAX_USER_DATA_LEN];
} BalSmsUserDataT;

/*refer GHRC_Long_SMS_Specification_v0.1.doc*/
typedef PACKED  struct
{
    uint8 UDHL;   /*Length of UDH, It is static value of 0x07*/
    uint8 IEI;  /*Information Element Identifier of Long SMS, It is static value of 0x08*/
    uint8 IEIDL;  /*Length of Information Element ,It is static value of 0x04*/
    uint16 ReferNum;  /*Concatenated short messages, 16-bit reference number*/
    uint8 TotalNum;  /*Total number of segments in the long SMS message*/
    uint8 CurrSegNum;  /*Sequence number of the current segmented message*/
} BalSmsGHRCHeaderT;

typedef PACKED  struct
{
  bool  UserAckReq;  /*true for user ack is required*/
  bool  DakReq;      /*true for delivery ack is required*/
  bool  ReadAckReq;  /*true for read ack is required*/
} BalSmsRplOptionT;

typedef enum
{
  BAL_SMS_MSGSTATUS_ACCEPTED,
  BAL_SMS_MSGSTATUS_DEPOSITED_TO_INTERNET,
  BAL_SMS_MSGSTATUS_DELIVERED,
  BAL_SMS_MSGSTATUS_CANCELLED,
  BAL_SMS_MSGSTATUS_NETWORK_CONGESTION,
  BAL_SMS_MSGSTATUS_NETWORK_ERROR,
  BAL_SMS_MSGSTATUS_CANCEL_FAILED,
  BAL_SMS_MSGSTATUS_BLOCKED_DESTINATION,
  BAL_SMS_MSGSTATUS_TEXT_TOO_LONG,
  BAL_SMS_MSGSTATUS_DUPLICATE_MESSAGE,
  BAL_SMS_MSGSTATUS_INVALID_DESTINATION,
  BAL_SMS_MSGSTATUS_EXPIRED,
  BAL_SMS_MAX_MSGSTATUS
} BalSmsMsgStatusCodeT;


typedef PACKED  struct
{
  BalSmsErrClassT      ErrorClass;
  BalSmsMsgStatusCodeT MsgStatusCode;
} BalSmsMsgStatusT;

typedef enum
{
  BAL_SMS_CATRESULT_SUCCESS,
  BAL_SMS_CATRESULT_MEMORY_EXCEEDED,
  BAL_SMS_CATRESULT_LIMIT_EXCEEDED,
  BAL_SMS_CATRESULT_ALREADY_PROGRAMMED,
  BAL_SMS_CATRESULT_NOT_PREVIOUSLY_PROGRAMMED,
  BAL_SMS_CATRESULT_INVALID_MAX_MESSAGES,
  BAL_SMS_CATRESULT_INVALID_ALERT_OPTION,
  BAL_SMS_CATRESULT_INVALID_CATEGORY_NAME,
  BAL_SMS_CATRESULT_UNSPECIFIED_FAILURE,
  BAL_SMS_MAX_CATRESULT
} BalSmsCategoryResultCodeT;


typedef PACKED  struct
{
  BalSmsSrvCatT              Category;
  BalSmsCategoryResultCodeT  CategoryResult;
} BalSmsCategoryResultT;

typedef PACKED  struct
{
  uint8                 NumCategories;
  BalSmsCategoryResultT Cat[BAL_SMS_MAX_CATEGORIES];
} BalSmsSrvCatProgRsltT;

typedef enum
{
  BAL_SMS_OP_DELETE,
  BAL_SMS_OP_ADD,
  BAL_SMS_OP_CLEAR_ALL,
  BAL_SMS_MAX_OP
} BalSmsOperationCodeT;

typedef enum
{
  BAL_NO_ALERT                 = 0,
  BAL_MOBILE_DEFAULT_ALERT,
  BAL_VIBRATE_ALERT_ONCE,
  BAL_VIBRATE_ALERT_REPEAT,
  BAL_VISUAL_ALERT_ONCE,
  BAL_VISUAL_ALERT_REPEAT,
  BAL_LOW_PRIORITY_ALERT_ONCE,
  BAL_LOW_PRIORITY_ALERT_REPEAT,
  BAL_MED_PRIORITY_ALERT_ONCE,
  BAL_MED_PRIORITY_ALERT_REPEAT,
  BAL_HIGH_PRIORITY_ALERT_ONCE,
  BAL_HIGH_PRIORITY_ALERT_REPEAT,
  BAL_SMS_MAX_CAT_ALERT
} BalSmsCatAlertT;

typedef PACKED  struct
{
  BalSmsOperationCodeT  OperationCode;
  BalSmsSrvCatT         SrvCat;
  BalSmsLanguageT       Lang;
  /*the maximum number of messages that may be stored in the mobile station for this Service Category*/
  uint8                 MaxMsg;
  BalSmsCatAlertT       Alert;
  uint8                 NumChar;
  uint8                 Chari[BAL_SMS_MAX_CHAR_IN_CATEGORY];
} BalSmsCatDataT;

typedef PACKED  struct
{
  BalSmsMsgEncodeT MsgEncoding;
  uint8            NumCat;
  BalSmsCatDataT   Cat[BAL_SMS_MAX_CATEGORIES];
} BalSmsSrvCatProgDataT;

/* field mask definitions */
#define SERVICE_CAT_PRESENT            0x00000001
#define ADDRESS_PRESENT                0x00000002
#define SUBADDRESS_PRESENT             0x00000004
#define USER_DATA_PRESENT              0x00000008
#define USER_RESP_CODE_PRESENT         0x00000010
#define MC_TIME_STAMP_PRESENT          0x00000020
#define ABS_BAL_PERIOD_PRESENT         0x00000040
#define REL_BAL_PERIOD_PRESENT         0x00000080
#define ABS_DEF_DEL_TIME_PRESENT       0x00000100
#define REL_DEF_DEL_TIME_PRESENT       0X00000200
#define PRIORITY_IND_PRESENT           0x00000400
#define PRIVACY_IND_PRESENT            0x00000800
#define REPLY_OPTION_PRESENT           0x00001000
#define NUM_MESSAGES_PRESENT           0x00002000
#define ALERT_MSG_DEL_PRESENT          0x00004000
#define LANGUAGE_IND_PRESENT           0x00008000
#define CALLBACK_NUM_PRESENT           0x00010000
#define MSG_DISP_MODE_PRESENT          0x00020000
#define MULT_ENCODE_USER_DATA_PRESENT  0x00040000
#define MESSAGE_DEPOSIT_INDEX_PRESENT  0x00080000
#define MESSAGE_STATUS_PRESENT         0x00400000
#define SER_CAT_PROGRAM_RESULT_PRESENT 0x00800000
#define SER_CAT_PROGRAM_DATA_PRESENT   0x01000000

typedef uint16 BalSmsRecIdT;

typedef enum
{
  FREE       = 0,   /* the block has not occupied by a sms message */
  READ       = 1,
  TO_BE_READ = 3,
  SENDING = 4,
  SENT       = 5,
  PENDING =6,
  TO_BE_SENT = 7,
  CANCELED = 8,
  DRAFT_READ = 9,
  DRAFT_SENT = 11,
  DRAFT_TO_BE_SENT = 13,
  DUPLICATE_READ=15,
  DUPLICATE_TO_BE_READ=17,
  DELIVERED = 19, /* device recieve Delivery Acknowledgment received  */
  ARCHIVE_READ = 21,
  ARCHIVE_TO_BE_READ = 23,
  ARCHIVE_SENT = 25,
  ARCHIVE_TO_BE_SENT = 27,
  BAL_SMS_MAX_STATUS
} BalSmsStatT;

typedef struct
{
  /* required fields */
  uint32                FieldMask;      /* the field mask */
  BalSmsRecIdT          SmsMsgRecId;    /* the phisical position of sms record in the storage device, UI NEEDN't fill it */
  BalSmsStatT           SMSState;       /* sms message status, UI may not fill it when sending, But if UI want to write or update a message, it must fill it.*/
  uint8                 NumUserData;    /* number of user data */
  BalSmsTransMsgTypeT   TransMsgType;   /* transport layer message type, UI may not fill it */
  uint16                MsgId;          /* message id, required, UI NEEDN'T fill it */
  bool                  HeaderInd;      /* message head indication, required. UI MUST give it a correct value. True for the user data field includes a User Data Header, else set false */
  BalSmsTeleMsgT        TeleMsgType;    /* teleservice layer message type, required. UI MUST give it a correct value */
  BalSmsTeleSrvIdT      TeleSrvId;      /* teleservice id, required, UI MUST give it a correct value */
  /* following is the optional fields */
  BalSmsSrvCatT         SrvCat;         /* for broadcasting message, required, else, optional */
  BalSmsAddressT        Address;        /* required for submit and delivery message */
  BalSmsSubaddressT     Subaddress;     /* optional */
  BalSmsUserDataT       UserData[BAL_SMS_MAX_NUM_DATA_MSGS]; /* user data or multi encoding user data   */
  uint8                 UserRspCode;
  BalSmsAbsTimeT        TimeStamp;
  BalSmsAbsTimeT        BalTimeAbs;
  uint8                 RelValPeriod;
  BalSmsAbsTimeT        AbsDelTime;
  uint8                 RelDelPeriod;
  BalSmsPriorityT       Priority;
  BalSmsPrivacyT        Privacy;
  BalSmsRplOptionT      RplOp;
  uint8                 NumMsgs;        /* only for VMN message */
  BalSmsAlertT          Alert;
  BalSmsLanguageT       Lang;
  BalSmsCbNumberT       CallBackNum;
  BalSmsDispModeT       DispMode;
  uint16                MsgDepIndex;
  BalSmsMsgStatusT      MsgStatus;            /*used only for SMS delivery Acknowledgement message */
  BalSmsSrvCatProgRsltT SrvCatProgResult;     /* only for submitting message */
  BalSmsSrvCatProgDataT SrvCatData;           /*only for delivery message */
} BalSmsMessageT;

typedef enum
{
  BAL_SMS_CHANNEL_DEFAULT,
  BAL_SMS_CHANNEL_TC,
  BAL_SMS_MAX_CHANNEL
} BalSmsChannelT;

/*val sms event message type for ui*/
typedef PACKED struct
{
  BalSmsRecIdT recid;
  bool isDuplicateSms;
} BalSmsIncomingMsgT;

typedef PACKED struct
{
  uint8 SeqNum;       /* The PSW-MMI SeqNum id used in msg assembly  */
  BalSmsErrClassT  ErrClass;
  BalSmsCauseCodeT CauseCode;
} BalSmsSendStatusMsgT;

typedef PACKED struct
{
  bool       SmsMemFull;   /*TRUE for no memory, FALSE for memory enough*/
  BalDeviceT device;
} BalSmsMemStatusMsgT;

typedef PACKED struct
{
  BalDeviceT device;
} BalSmsWriteErrMsgT;

/*----------------------------------
message storage type define
------------------------------------*/

typedef enum
{
 /*BAL SMS Msg type               teleservice value in IS-637:*/

  MSG_IS91_PAGE_MSG,
  MSG_IS91_VOICE_MAIL,        /* analog mode teleservice */
  MSG_IS91_SHORT_MESSAG,
  MSG_VOICE_MAIL_MWI,

  MSG_TELE_SRV_IS91,    /* IS91_TS_ID_IS91           = 4096, */
  MSG_TELE_SRV_PAGE,    /* BAL_SMS_TS_ID_PAGE        = 4097, */
  MSG_TELE_SRV_MESSAGE, /* BAL_SMS_TS_ID_MESSAGE     = 4098, */
  MSG_TELE_SRV_VMN,     /* BAL_SMS_TS_ID_VOICE_MAIL  = 4099, */
  MSG_TELE_SRV_WAP,     /* BAL_SMS_TS_ID_WAP,    */
  MSG_TELE_SRV_WEMT,    /* BAL_SMS_TS_ID_WEMT,   */
  MSG_TELE_SRV_SCPT,    /* BAL_SMS_TS_ID_SCPT,   */
  MSG_TELE_SRV_CATPT,   /* BAL_SMS_TS_ID_CATPT   */
  MSG_BROAD_CAST_SRV,   /* broadcast SMS service, not a teleservice, here for the purpose of  management for all SMS service */
  MAX_REG_MSG
} BalSmsTeleSrvType;
#if 0
typedef PACKED struct
{
  BalSmsTeleSrvIdT TeleSrvId;
  uint8            RegIdCount;
  bool             NeedSave;
  uint8            NeedSaveCount;
} RegEvtInfoT;
#endif

typedef PACKED struct
{
	bool isUse;
	uint16 TeleSrvId;
	bool NeedSave;
}RegEvtInfoT;

#define IS_VALID_SMS_SO(a)                \
         (((a) == 0) ||                       \
          ((a) == BAL_SERVICE_OPTION_6)  ||   \
          ((a) == BAL_SERVICE_OPTION_14))


/*=================================================================================
               Type Define
==================================================================================*/
/*sms in PDU formate*/
typedef  struct
{
  uint8 SmsRecStat;
  uint8 SmsLength;
  uint8 SmsRec[BAL_SMS_MAX_SMS_PDU_LEN];
} BalSmsRecordT;

/*crc and timestamp information of a SMS record,use for read from look up table*/
typedef PACKED struct
{
  uint16           RecId;
  BalSmsStatT      SmsStat;
  uint32           TimeStamp;
  uint16           CrcForMsgidAddr;  /* the crc value for message identifier and originate */
  BalSmsTeleSrvIdT TeleSrvId; /*the teleservice id of the message*/
  BalSmsPriorityT SmsPriority;  /*Add priority by zhoujianlang*/
  uint16          IsLocked;/*Add lock info by zhoujianlang*/
  uint16        IsCallbackNumMsg;
  uint16        FolderId;
  uint16    PrevRecId; //For Long Msg.
  uint16    NextRecId;
} BalSmsCrcInfoT;

/*the memory status*/
typedef PACKED struct
{
  uint16 nFlashMaxSmsRec;
  uint16 nFlashFreeSmsRec;
  uint16 nUimMaxSmsRec;
  uint16 nUimFreeSmsRec;
} BalSmsStorParamsT;

/* ETS Messages
 */
typedef PACKED struct
{
  RegIdT           RegId;
  BalSmsTeleSrvIdT TeleSrvId;
} BalSmsRegSrvMsgT;


typedef struct
{
	uint16 DupliPolicy;
/*bit0: crc; bit1: timestamp; bit2: OrigAddr; bit3: userdata;bit4: callbackNum; bit5:msgId; bit6: SubAddr; bit7: priority; bit8:teleSrvId*/	
	uint8 Count;  /* how many items to be checked*/
}BalSmsDupliPolicyT;



/*=================================================================================
                              TYPE DEFINE FOR BAL SMS STORAGE
 ==================================================================================*/
 /* from BalDeviceT to BalSmsStorageDeviceT*/
 #define BAL_SMS_MAX_MAPPING_STORAGE_DEV_COUNT 2
 
  /* max register sms storage device count */
 #define BAL_SMS_MAX_REG_DEV_CNT 8

 /* Bal SMS device count */
 typedef struct
 {
   	uint16 SmsDevMaxRecCount;
   	uint16 SmsDevFreeCount;
 } BalSmsCountT;

 /* val sms record storage device init call back func */
 typedef void	 (*BalSmsDevInitCallBack)(void);

 /* for val sms storage data segments device */
 typedef struct
 {
 	uint16  MaxDataSegCount;
	uint8*  Bitmap;
 }BalSmsDataDevBitmapT;

 typedef uint32  (*BalSmsDataDevOpen)(void);
 typedef uint32	 (*BalSmsDataDevCacheRecord)(uint16 DataSegId,uint32 TeleSrvId);
 typedef uint32	 (*BalSmsDataDevGetFreeRecId)(uint16* DataSegId);
 typedef uint32  (*BalSmsDataDevReadRecord)(uint16 RecId, void * Buf, uint32* LengthP);
 typedef uint32  (*BalSmsDataDevUpdateRecord)(uint16 RecId,void * Buf, uint32 Length);
 typedef uint32  (*BalSmsDataDevDeleteRecord)(uint16 RecId);
 typedef uint32	 (*BalSmsDataDevClose)(void);
 
 typedef struct
 {
 	BalSmsDataDevBitmapT*	  	  SmsDataDevBitmap;
	BalSmsDataDevOpen			  SmsDataDevOpen;
	BalSmsDataDevCacheRecord	  SmsDataDevCacheRecord;
	BalSmsDataDevGetFreeRecId	  SmsDataDevGetFreeRecId;
 	BalSmsDataDevReadRecord   	  SmsDataDevReadRecord;
	BalSmsDataDevUpdateRecord     SmsDataDevUpdateRecord;
	BalSmsDataDevDeleteRecord     SmsDataDevDeleteRecord;
	BalSmsDataDevClose			  SmsDataDevClose;
 }BalSmsDataDevOptT;

 
 /* val sms record storage device opt func */
 typedef uint32	 (*BalSmsDevInit)(BalSmsDevInitCallBack CallBackP);
 typedef bool	 (*BalSmsDevIsReady)(void);
 typedef uint32  (*BalSmsDevOpen)(BalSmsCountT*  BalSmsCountP);
 typedef uint32	 (*BalSmsDevGetFreeRecId)(uint16* RecIdP);
 typedef uint32	 (*BalSmsDevGetRecInfo)(uint16 RecId, void * Buf, uint32* LengthP);
 typedef uint32  (*BalSmsDevReadRecord)(uint16 RecId, void * Buf, uint32* LengthP);
 typedef uint32  (*BalSmsDevUpdateRecord)(uint16 RecId,void * Buf, uint32 Length);
 typedef uint32  (*BalSmsDevDeleteRecord)(uint16 RecId);
 typedef uint32	 (*BalSmsDevClose)(void);
 
 typedef struct
 {
 	bool 				  SmsDevIsCache;	/* the val sms storage should build cache for this device */
	bool				  SmsDevHasDataSeg; /* the val sms storage device has data segments */
	BalSmsDataDevOptT*	  SmsDataDevOpt;   /* the val sms storage data segments dev opt */
	BalSmsDevInit		  SmsDevInit;
 	BalSmsDevIsReady	  SmsDevIsReady;
 	BalSmsDevOpen         SmsDevOpen;
	BalSmsDevGetFreeRecId SmsDevGetFreeRecId;
	BalSmsDevGetRecInfo	  SmsDevGetRecInfo;
 	BalSmsDevReadRecord	  SmsDevReadRecord;
 	BalSmsDevUpdateRecord SmsDevUpdateRecord;
 	BalSmsDevDeleteRecord SmsDevDeleteRecord;
 	BalSmsDevClose		  SmsDevClose;
	
 }BalSmsDevOptT;

 /* The struct of device list */
 typedef struct
 {
 	uint8	DevId;					/* device id */
	uint16	CurDevRecId;			/* current device record id */
 	uint16	CacheOffset;   		    /* the first cache item in look up table */
 	BalSmsCountT   BalSmsCount;		/* device storage information */
 	BalSmsDevOptT* BalSmsDevOpt;	/* device operation function */
 }BalSmsDevListT;

 /* val sms storage init state */
 typedef enum
 {
 	BAL_SMS_STORAGE_INIT_INVALID = 0x00, 		/* the sms storage dev initial invalid */
 	BAL_SMS_STORAGE_INIT_VALID,					/* the sms storage dev initial valid */
 	BAL_SMS_STORAGE_INIT_NOT_COMPLETE,			/* the sms storage init not complete */
 	BAL_SMS_STORAGE_INIT_COMPLETE				/* the sms storage init complete */
 }BalSmsStorageInitStateT;

 /* val sms storage device init state msg */
 typedef PACKED  struct 
 {
     BalSmsStorageInitStateT SmsStorageInitState;
 	
 } BalSmsStorageInitStateMsgT;


 /* val sms look up table for message, cache in ram */
 typedef struct
 {
   uint16           NextAddress;
   uint16 			DataSegId;
   uint16           CrcForMsgidAddr;  /* the crc value for message identifier and originate */
   uint32           TimeStamp;
   BalSmsStatT      SmsStat;
   BalSmsTeleSrvIdT TeleId;           
   /*add extra info by zhoujianlang*/
   BalSmsPriorityT Priority;
   bool                 IsLocked;
   uint16			FolderId;   
   bool        IsCallbackNumMsg;
   uint16    PrevRecId; //For Long Msg.
   uint16    NextRecId;
 } BalSmsLookUpTableT;

 /* val sms look up table record, save in flash */
 typedef struct
 {
   uint16		     DataSegId;
   uint16            CrcForMsgidAddr;
   uint32            TimeStamp;
   BalSmsStatT       SmsStat;
   BalSmsTeleSrvIdT  TeleId;
   /*add extra info by zhoujianlang*/
   BalSmsPriorityT Priority;
   bool                 IsLocked;
   uint16			FolderId;   
   bool         IsCallbackNumMsg;
   uint16    PrevRecId; //For Long Msg.
   uint16    NextRecId;
 } BalSmsLookUpRecT;

 /* the struct for val device mapping to val sms storage device */
 typedef struct
 {
 	uint8 StorageDevCount;
	uint8 StorageDevId[BAL_SMS_MAX_MAPPING_STORAGE_DEV_COUNT];
 }BalSmsDevMappingIdT;

 /* val sms storage memory full indication */
 typedef enum
 {
 	BAL_SMS_MEM_INDICATION_FULL = TRUE,
	BAL_SMS_MEM_INDICATION_NOT_FULL = FALSE
 }BalSmsMemFullIndicationT;

 /* cutomized initial parameters for sms */
 typedef struct
 {
 	uint8  BalSmsMaxAvaDevCount;		/* max available sms storage device count */

	uint8  BalSmsVmnDevId;				/* fixed voice mail storage device id */
 	uint16 BalSmsVmnAddress;			/* fixed voice mail address */

	BalDeviceT BalSmsExtraDevType;		/* extra sms storage device type */
	uint8  BalSmsExtraDevId;			/* extra sms storage device id */
 	uint16 BalSmsMaxRecCount;		    /* max sms block count,not include one voice mail */
 
 	BalSmsLookUpTableT*  BalSmsLookUpTableP; /* look up table address */
 
 }BalSmsCustParaT;

 /* cutomized initial parameters for sms flash dev */
 typedef struct
 {
 	bool 				BalSmsHasDataSeg;
	BalSmsDataDevOptT* 	BalSmsDataDevOpt;
	uint16				BalSmsMaxFlashDataRecCount;
	uint16				BalSmsFlashDataRecSize;
	/*if has data segment,the max flash record count only include sms.
	  Has not data segment, the max flash record count include sms&ems and one voice mail */
	uint16				BalSmsMaxFlashFileRecCount;
	uint16 			   	BalSmsMaxFlashRecCount;
	uint16				BalSmsFlashRecSize;
 }BalSmsCustFlashParaT;

 /* cutomized initial parameters for sms extra flash dev */
 typedef struct
 {
 	uint16 BalSmsExtraRecSize;			/*  extra sms  record size */
	uint16 BalSmsMaxExtraRecCount;			/* max extra sms  record count */
 }BalSmsCustExtraFlashParaT;

#define BAL_SMS_PST_MAX_REGISTERED_IDS 5

typedef enum
{
  BAL_SMS_PST_EVENT_WRITE_MESSAGE  ,
  BAL_SMS_PST_EVENT_DELETE_MESSAGE,
  BAL_SMS_PST_EVENT_UPDATE_MESSAGE,  
  BAL_SMS_PST_EVENT_RECEIVED_MESSAGE,
  BAL_SMS_PST_EVENT_SENT_MESSAGE,
  BAL_SMS_PST_MAX_EVENTS
}BalSmsPstEventIdT;

/* SMS PST callback function structure */
typedef void (*BalSmsPstEventFunc) ( RegIdT RegId, BalSmsPstEventIdT Event, void* MsgP );

/* SMS PST register table structure */
typedef struct
{
  bool IsUse;
  BalSmsPstEventFunc CallBack;         /* function to call back */
}BalSmsPstRegTableT;

/*=================================================================================
                    FUNCTION DEFINITION FOR BAL SMS REC STORAGE
 ==================================================================================*/
/* storage init api, called by val lsm app */
void BalSmsStorageVarInit( void );
void BalSmsStorageDataInit( void );
void BalSmsStorageInit(ExeRspMsgT* InitRspInfoP);
void BalSmsStorageInitRsp(void);

/* storage api */
BalSmsResultT BalSmsWriteTxtMessage( BalSmsMessageT* pSmsTxtMsg, uint16* pRecId );
BalSmsResultT BalSmsWritePduMessage( BalSmsRecordT* pSmsRecord, uint16* pRecId );
BalSmsResultT BalSmsUpdateTxtMessage( BalSmsMessageT* pSmsTxtMsg, uint16 recId );
BalSmsResultT BalSmsUpdatePduMessage( BalSmsRecordT* pSmsRecord, uint16 recId );
BalSmsResultT BalSmsReadTxtMessage( BalSmsMessageT* pSmsMessage, uint16 recId );
BalSmsResultT BalSmsReadPduMessage( BalSmsRecordT* pSmsRecord, uint16 recId );
BalSmsResultT BalSmsDelMessage( uint16 recId );
bool BalSmsIsReceivedCallBackMsg(uint16 nRecId);
bool BalSmsIsReceivedDeliverAckMsg(uint16 nRecId);

/* this api should be abandoned */
BalSmsResultT BalSmsDeleteAll( BalDeviceT deviceId );


/* voice mail message storage interface */
BalSmsResultT BalSmsReadVmnMessage(BalSmsMessageT *pSmsMessage);
BalSmsResultT BalSmsWriteVmnMessage(BalSmsMessageT *pSmsMessage);
BalSmsResultT BalSmsDelVmnMessage(void);

/* sms extra info storage interface */
BalSmsResultT BalSmsReadExtraInfo(uint16 RecId ,void *pSmsExtraInfo,uint16 ExtraSize);
BalSmsResultT BalSmsUpdateExtraInfo(uint16 RecId ,void *pSmsExtraInfo,uint16 ExtraSize);
BalSmsResultT BalSmsDelExtraInfo (uint16  RecId);

/* get val sms storage device record count */
BalSmsResultT BalSmsGetCount(BalDeviceT DevType,BalSmsCountT* DevCountP);

// Waley 20071218
BalSmsResultT BalSmsUpdateLookupTable(uint16 RecId, BalSmsCrcInfoT *Info);
bool BalSmsGetInfoFromLookupTable(uint16 RecId, BalSmsCrcInfoT *SmsCrcInfoP);

/*****************************************************************************

  FUNCTION NAME: BalSmsGetInfoFromTable

  DESCRIPTION:

      To get CRC info from the sort list.

  PARAMETERS:

   index; the logic index in the sort list, begin from 0
   pSmsCrcInfo: the buffer where to write the BalSmsCrcInfoT info

  RETURNED VALUES:
    success is true.failed is false

*****************************************************************************/
bool BalSmsGetInfoFromTable( uint8           nIndex,
                             BalSmsCrcInfoT* pSmsCrcInfo );

/*****************************************************************************

  FUNCTION NAME: BalSmsGetStorParams

  DESCRIPTION:

      To get current storage memory statue parameters.

  PARAMETERS:

   pSmsStorParams: the buffer where to write the BalSmsStorParamsT info back

  RETURNED VALUES:
    success is true.failed is false

*****************************************************************************/
bool BalSmsGetStorParams( BalSmsStorParamsT* pSmsStorParams );

/*****************************************************************************

FUNCTION  BalSmsGetSmsStatus
DESCRIPTION:  get the current val sms uint status
PARAMETERS:   void
RETURN:       TRUE: sms is available
              FALSE: sms is disavailable

*****************************************************************************/
bool BalSmsGetSmsStatus( void );

/*****************************************************************************
FUNCTION     BalSmsGetTeleSrvFromPdu
DESCRIPTION  extract the teleservice from a sms message pdu
PARAMETERS   data: the pdu
             len: the length of pdu

RETURN       BAL_SMS_MAX_TS_ID for not found teleservice id in pdu
             else, return the teleservice id
*****************************************************************************/
BalSmsTeleSrvIdT BalSmsGetTeleSrvFromPdu( uint8* data, uint8 nlength );

/*==========================================================================
                          SMS sending and receiving API
===========================================================================*/

/*===========================================================================

FUNCTION BalSmsSendTxtMessage

DESCRIPTION
  Send an Txt format SMS as defined in IS637A

DEPENDENCIES


RETURN VALUE
  BAL_SMS_SUCCESS             - if successful
  BAL_SMS_ERR_MSG_FORMAT      - if sending message format error
  BAL_SMS_ERR_NO_MEMORY       - if no enough memory
  BAL_SMS_ERR_SMS_NOT_READY   - if sms service is not available
SIDE EFFECTS
  None
===========================================================================*/
BalSmsResultT BalSmsSendTxtMessage
( RegIdT          RegId,
  BalSmsRecIdT*   recid,      /* return the stored rec id*/
  BalSmsMessageT* SmsTxtMsgP, /* Txt format Message to be sent */
  uint16*         MsgId,      /* return the sending message id for those ui who care about it*/
  bool            ToBeSave ); /* indicate if ui want to save this sending message*/

/*===========================================================================

FUNCTION BalSmsSendPduMessage

DESCRIPTION
  Send an pdu format SMS as defined in IS637A

DEPENDENCIES


RETURN VALUE
  BAL_SMS_SUCCESS             - if successful
  BAL_SMS_ERR_MSG_FORMAT      - if sending message format error
  BAL_SMS_ERR_NO_MEMORY       - if no enough memory
  BAL_SMS_ERR_SMS_NOT_READY   - if sms service is not available

SIDE EFFECTS
  None

===========================================================================*/
BalSmsResultT BalSmsSendPduMessage
( RegIdT        RegId,
  BalSmsRecIdT* recid,      /* return the stored rec id*/
  uint8*        MsgBufP,    /* pdu format message to be sent*/
  uint8         nbytes,     /* the length of the message to be sent when in PDU mode, in TXT mode, it can be set to 0*/
  uint16*       MsgId,      /* return the sending message id for those ui who care about it*/
  bool          ToBeSave ); /* indicate if the message need to be saved when sending*/


/*============================================================================

                        SMS message setting API

=============================================================================*/

/*===========================================================================

FUNCTION BalSmsSetPrefSvcOpt

DESCRIPTION

  Validate the 'sms_so'. Set preferred SMS service option with the validated
  'sms_so'.

DEPENDENCIES

  'sms_so' should be a valid one.

RETURN VALUE

  If set service option succeeds it returns BAL_SMS_SUCCESS
  else return BAL_SMS_ERR_INVALID_PARAMETER

SIDE EFFECTS
  None

===========================================================================*/
BalSmsResultT BalSmsSetPrefSvcOpt( BalServiceOptionT SmsSo );

/*****************************************************************************

  FUNCTION NAME:   SmsSetBroadcastParams

  DESCRIPTION:   Sends a broadcast parms command to protocol stack


  RETURN VALUE

  BAL_SMS_SUCCESS                   - if successful
  BAL_SMS_ERR_INVALID_PARAMETER    - otherwise

*****************************************************************************/
BalSmsResultT BalSmsSetBroadcastParams( uint32 SvcMask,
                                        uint32 LangMask,
                                        uint8  Priority );

/*********************************************************************************8
FUNCTION BalSmsSetStorage

DESCRIPTION

    set the default sending and receiving message storage device

PARAMETERS

    storeSet: sms storage device: UIM, FLASH, UIM_AND_FLASH, NONE

RETURN

    if succeed, return BAL_SMS_SUCCESS
   if sms not init complete, return BAL_SMS_ERR_SMS_NOT_READY.
   if the device to be set has no memory, return BAL_SMS_ERR_NO_MEMORY.



**************************************************************************************/
BalSmsResultT BalSmsSetStorage( BalDeviceT StoreSet );

/*********************************************************************************8
FUNCTION BalSmsGetStorage

DESCRIPTION

    get the default sending and receiving message storage device

PARAMETERS

    void

RETURN

	storeSet: sms storage device: UIM, FLASH, UIM_AND_FLASH, NONE
**************************************************************************************/

BalDeviceT BalSmsGetStorage( void );

/*************************************************************************************
FUNCTION BalSmsSetSavePolicy

DESCRIPTION

    set a certain kind of teleservice messages shall be stored or not

PARAMETERS

    TeleSrvId: the message telesrvice id , should be a valid teleservice id
    NeedSave: if this kind of message shall be saved

RETURN

    void
**************************************************************************************/
#if 0
void BalSmsSetSavePolicy( BalSmsTeleSrvIdT TeleSrvId,
                          bool             NeedSave );
#endif
void BalSmsSetSavePolicy(RegEvtInfoT*   RegInfoP );
/*============================================================================

                        SMS message registration API

=============================================================================*/
/*****************************************************************************

  FUNCTION NAME: BalSmsRegister

  DESCRIPTION:

    This routine registers SMS callback to the SMS Register table

  PARAMETERS:

    CallBack: Callback function pointer

  RETURNED VALUES:

    Register Identifier.
     -1: failed

*****************************************************************************/
RegIdT BalSmsRegister( BalSmsEventFunc CallBack );

/*****************************************************************************

  FUNCTION NAME: BalSmsUnregister

  DESCRIPTION:

    This routine unregisters callback function in the SMS event register table.

  PARAMETERS:

    RegId: assigned Register Identifier

  RETURNED VALUES:

    None

*****************************************************************************/
void BalSmsUnregister( RegIdT RegId );

/*****************************************************************************

  FUNCTION NAME: BalSmsPstRegister

  DESCRIPTION:

    This routine registers SMS callback to the SMS PST Register table

  PARAMETERS:

    CallBack: Callback function pointer

  RETURNED VALUES:

    Register Identifier.
     -1: failed

*****************************************************************************/
RegIdT BalSmsPstRegister( BalSmsPstEventFunc CallBack );

/*****************************************************************************

  FUNCTION NAME: BalSmsPstUnregister

  DESCRIPTION:

    This routine unregisters callback function in the SMS PST event register table.

  PARAMETERS:

    RegId: assigned Register Identifier

  RETURNED VALUES:

    None

*****************************************************************************/
bool BalSmsPstUnregister(RegIdT RegId );

/*===========================================================================
FUNCTION BalSmsStartRegSrv

DESCRIPTUON:

    start the registered service

PARAMETER:

    RegId: registration ID, generally means a ui
    TeleId: the teleservice to be connected, including broad cast service

Returns

    SUCCESS
  BAL_SMS_ERR_INVALID_PARAMETER if the teleserviceid is not a IS637 teleservice id

============================================================================*/
#if 0
BalSmsResultT BalSmsStartRegSrv( RegIdT           RegId,
                                 BalSmsTeleSrvIdT TeleId );
#endif
BalSmsResultT BalSmsStartRegSrv( RegIdT RegId,   RegEvtInfoT*   RegInfoP);
/*===========================================================================
FUNCTION    BalSmsStopRegSrv

Description
    Stop the registered service

Parameters
    RegID: the registration ID, with which BAL SMS can find which teleservice shall be unset.
    TeleId: the teleservice id to be stopped

Return
    void
=============================================================================*/
void BalSmsStopRegSrv( RegIdT           RegId,
                       uint16 TeleId );

/*============================================================================

                        SMS  Miscellaneous API

=============================================================================*/

/*****************************************************************************

FUNCTION   BalSmsInit

DESCRIPTION Init the current SMS service variabel

PARAMETERS
    void

RETURN
    void

*******************************************************************************/
void BalSmsInit( void );

/******************************************************************************

FUNCTION   BalSmsClose

DESCRIPTION Close the current SMS service

PARAMETERS
    void

RETURN
    void

*******************************************************************************/
void BalSmsClose( void );
#ifdef BYD_USE_SIM
uint16 BalSmsGetLastMsgId( void );
#endif
bool MsgNeedSaveByTeleSrvId(uint16 TeleSrvId);
void BalSmsSetDupliPolicy(BalSmsDupliPolicyT* MsgP);
void BalSmsSetMultiSMSMode(bool IsMultiSMS);
void BalSmsSetAutoDelete(bool AutoDel);

#ifdef __cplusplus
    }
#endif /* __cplusplus */


#endif  /* BALSMSAPI_H */

