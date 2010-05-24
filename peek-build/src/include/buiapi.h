#ifndef _UI_API_H_
#define _UI_API_H_

#include "monids.h"
#include "monapi.h"
#include "balfsiapi.h"
#include "baldispapi.h"

#define UI_MAIN_MAILBOX_ID          EXE_MAILBOX_1_ID

#if 0
#define UI_MAIN_MAIL_TYPE           EXE_MAILBOX_1
#define BUI_MESSAGE_TYPE           UI_MAIN_MAIL_TYPE
#define UI_ONE_SEC_TIMER_ID         (uint32)2
#define UI_AUTO_ANSWER_TIMER_ID     (uint32)6

#define UI_TIMER_RESET                      0
#define UI_TIMER_RESTART                    1
#define UI_ONE_SEC_TIMEOUT_VALUE            1000  /* msecs */

#define UI_ONE_SEC_TIMEOUT_SIGNAL           EXE_SIGNAL_11
#define UI_DEEP_SLEEP_TIME_SLICE_DEFAULT    0     /* 0 msecs (off) */
#define UI_TASK_STOP_SIG                    EXE_SIGNAL_20

#define UI_TRACE( x )  MonTrace( MON_CP_UI_GENERIC_TRACE_ID, 1, x )

enum EventCatT {
  CALL_PROCESSING_CAT  = 1,
  SMS_CAT              = 2,
  DATA_CAT             = 3,
  KEY_CAT              = 4,
  ACCESSORY_CAT        = 5,
  WINDOW_CAT           = 6,
  SOFTICON_CAT         = 7,
  APPLICATION_CAT      = 8,
  GENERAL_UI_CAT
};

enum EventBitT {
  CALL_PROCESSING_BIT  = 1 << (CALL_PROCESSING_CAT - 1),
  SMS_BIT              = 1 << (SMS_CAT - 1),
  DATA_BIT             = 1 << (DATA_CAT - 1),
  KEY_BIT              = 1 << (KEY_CAT - 1),
  ACCESSORY_BIT        = 1 << (ACCESSORY_CAT - 1),
  WINDOW_BIT           = 1 << (WINDOW_CAT - 1),
  SOFTICON_BIT         = 1 << (SOFTICON_CAT - 1),
  APPLICATION_BIT      = 1 << (APPLICATION_CAT - 1),
  GENERAL_UI_BIT       = 1 << (GENERAL_UI_CAT - 1),
  ALL_BIT              = 0x7FFFFFFF
};

#define GROUP_BIT_LOCATION 12
#define APPCAT(group,id) (0x01000000                    | \
                          (group << GROUP_BIT_LOCATION) | \
                          ((id) & 0x0FFF))
#define GROUP_MASK 0x000FF000
#define GET_MAIL_GROUP(MsgId) (((MsgId) & GROUP_MASK) >> GROUP_BIT_LOCATION)

#define MSGID_OF_GROUP_MASK 0x00000FFF
#define GET_MSGID_OF_GROUP(MsgId) ((MsgId) & MSGID_OF_GROUP_MASK)
#endif

enum MailCatT {
  CDMA_CAT            = 1,
  KEY_CAT             = 2,
  ACCESSORY_CAT       = 3,
  WINDOW_CAT          = 4,
  ANNOUNCIATOR_CAT    = 5,
  APPLICATION_CAT     = 6,
  GENERAL_CAT
};

enum MailMaskBitT {
  CDMA_BIT            = 1 << (CDMA_CAT - 1),
  KEY_BIT             = 1 << (KEY_CAT - 1),
  ACCESSORY_BIT       = 1 << (ACCESSORY_CAT - 1),
  WINDOW_BIT          = 1 << (WINDOW_CAT - 1),
  ANNOUNCIATOR_BIT    = 1 << (ANNOUNCIATOR_CAT - 1),
  APPLICATION_BIT     = 1 << (APPLICATION_CAT - 1),
  GENERAL_BIT         = 1 << (GENERAL_CAT - 1),
  ALL_BIT             = 0xFF
};

#define CAT_MASK          0x0FF00000
#define CAT_BIT_LOCATION  20
#define MAIL_CAT(CatId) \
  (((CatId) << CAT_BIT_LOCATION) & CAT_MASK)
#define GET_MAIL_CAT(MsgId) \
  (((MsgId) & CAT_MASK) >> CAT_BIT_LOCATION)

#define ID_MASK 0x000FFFFF
#define GET_MAIL_ID(MsgId)\
  ((MsgId) & ID_MASK)

#define GROUP_BIT_LOCATION 12
#define GROUP_MASK 0x000FF000
#define MSGID_OF_GROUP_MASK 0x00000FFF
#define APPCAT(group,id) \
  (MAIL_CAT(APPLICATION_CAT) | (((group)<<GROUP_BIT_LOCATION) & GROUP_MASK) | ((id) & MSGID_OF_GROUP_MASK))
#define GET_MAIL_GROUP(MsgId) \
(((MsgId) & GROUP_MASK) >> GROUP_BIT_LOCATION)

#define GET_MSGID_OF_GROUP(MsgId)\
((MsgId) & MSGID_OF_GROUP_MASK)


#define CHVAPP                  1
#define BALTESTAPP              8  /* the same as APPID_HIDEMENU (in "buiappids.h") to be modified */
/*     #define MAX_NUM_OF_APP          35  add new app before this definition and increase this value  */

#define   CHVAPP_LAUNCH                 APPCAT(CHVAPP, 1)
#define   CHVAPP_CHV_CHECK              APPCAT(CHVAPP, 2)
#define   CHVAPP_UIM_CHECK              APPCAT(CHVAPP, 3)
#define   CHVAPP_SP_CHECK               APPCAT(CHVAPP, 4)
#define   CHVAPP_AREA_CHECK             APPCAT(CHVAPP, 5)
#define   CHVAPP_ALL_PASSED             APPCAT(CHVAPP, 6)
#define   CHVAPP_ENTER_PIN              APPCAT(CHVAPP, 7)
#define   CHVAPP_CLEAR_PIN              APPCAT(CHVAPP, 8)
#define   CHVAPP_ENABLE_PIN_MENU        APPCAT(CHVAPP, 9) //TEMP USE
#define   CHVAPP_DISABLE_PIN_MENU       APPCAT(CHVAPP, 10)
#define   CHVAPP_ENTER_PUK              APPCAT(CHVAPP, 11)
#define   CHVAPP_PUK_CHECK              APPCAT(CHVAPP, 12)
#define   CHVAPP_CHANGE_PIN_MENU        APPCAT(CHVAPP, 13)
#define   CHVAPP_CHANGE_PIN_OK          APPCAT(CHVAPP, 14)
#define   CHVAPP_CANCEL_DLG             APPCAT(CHVAPP, 15)
#define   CHVAPP_GET_NAM                APPCAT(CHVAPP, 16)
#define   CHVAPP_GET_IMSI               APPCAT(CHVAPP, 17)
#define   CHVAPP_OPEN_MAIN_MENU         APPCAT(CHVAPP, 18)


#define  UI_SET_DEEP_SLEEP_TIME_SLICE_MSG  APPCAT( BALTESTAPP,  1 )

#define  UI_SET_RINGER_FILE                APPCAT( BALTESTAPP, 11 )
#define  UI_PLAY_MUSIC_FILE                APPCAT( BALTESTAPP, 12 )
#define  UI_SET_BACKGROUND                 APPCAT( BALTESTAPP, 13 )
#define  UI_DRAW_BACKGROUND                APPCAT( BALTESTAPP, 14 )
#define  UI_DRAW_ANIMATION                 APPCAT( BALTESTAPP, 15 )
#define  UI_DRAW_NEXT_ANIMATION            APPCAT( BALTESTAPP, 16 )
#define  UI_STOP_ANIMATION                 APPCAT( BALTESTAPP, 17 )
#define  UI_SET_AUTO_ANSWER_MSG            APPCAT( BALTESTAPP, 18 )
#define  UI_SET_PRIVACY_MODE_MSG           APPCAT( BALTESTAPP, 19 )
#define  UI_SET_AUTO_ANSWER_DELAY_MSG      APPCAT( BALTESTAPP, 20 )
#define  UI_SET_DTMF_TYPE_MSG              APPCAT( BALTESTAPP, 21 )
#define  UI_DBM_READ_NAM_MSG               APPCAT( BALTESTAPP, 22 )
#define  UI_PERMDATA_READ_MSG              APPCAT( BALTESTAPP, 23 )
#define  UI_START_REFUBISH_AGING_TEST_MSG APPCAT( BALTESTAPP, 24 )
#define  UI_STOP_REFUBISH_AGING_TEST_MSG  APPCAT( BALTESTAPP, 25 )
#define  UI_REFUBISH_AGING_INC_PWR_MSG    APPCAT( BALTESTAPP, 26 )
#define  UI_REFUBISH_AGING_DEC_PWR_MSG    APPCAT( BALTESTAPP, 27 )
#define  UI_GET_AUTO_ANSWER_STATUS_MSG    APPCAT( BALTESTAPP, 28 )
#define  UI_GET_ALERT_STATUS_MSG          APPCAT( BALTESTAPP, 29 )

#define  UI_BAL_NET_INITIALIZE             APPCAT( BALTESTAPP, 30 )
#define  UI_BAL_NET_PPP_OPEN               APPCAT( BALTESTAPP, 31 )
#define  UI_BAL_NET_PPP_CLOSE              APPCAT( BALTESTAPP, 32 )
#define  UI_BAL_NET_PPP_DORMANT_REQ        APPCAT( BALTESTAPP, 33 )
#define  UI_BAL_NET_GET_PPP_STATUS         APPCAT( BALTESTAPP, 34 )
#define  UI_BAL_NET_EVENT_SELECT           APPCAT( BALTESTAPP, 35 )
#define  UI_BAL_NET_SOCKET                 APPCAT( BALTESTAPP, 36 )
#define  UI_BAL_NET_CONNECT                APPCAT( BALTESTAPP, 37 )
#define  UI_BAL_NET_CLOSE                  APPCAT( BALTESTAPP, 38 )
#define  UI_BAL_NET_GET_SOCK_STATUS        APPCAT( BALTESTAPP, 39 )
#define  UI_BAL_NET_RECV                   APPCAT( BALTESTAPP, 40 )
#define  UI_BAL_NET_SEND                   APPCAT( BALTESTAPP, 41 )
#define  UI_BAL_NET_BIND                   APPCAT( BALTESTAPP, 42 )
#define  UI_BAL_NET_LISTEN                 APPCAT( BALTESTAPP, 43 )
#define  UI_BAL_NET_ACCEPT                 APPCAT( BALTESTAPP, 44 )
#define  UI_BAL_NET_SEND_TO                APPCAT( BALTESTAPP, 45 )
#define  UI_BAL_NET_RECV_FROM              APPCAT( BALTESTAPP, 46 )
#define  UI_BAL_NET_GET_HOST_BY_NAME       APPCAT( BALTESTAPP, 47 )
#define  UI_BAL_NET_INET_ADDR              APPCAT( BALTESTAPP, 48 )
#define  UI_BAL_NET_GET_PPP_AUTH_PARMS     APPCAT( BALTESTAPP, 49 )
#define  UI_BAL_NET_SET_PPP_AUTH_PARMS     APPCAT( BALTESTAPP, 50 )
#define  UI_BAL_NET_GET_DIAL_STR_PARMS     APPCAT( BALTESTAPP, 51 )
#define  UI_BAL_NET_SET_DIAL_STR_PARMS     APPCAT( BALTESTAPP, 52 )
#define  UI_BAL_NET_SET_RLP_INACT_TMO_REQ  APPCAT( BALTESTAPP, 53 )
#define  UI_BAL_NET_GET_RLP_SETTING        APPCAT( BALTESTAPP, 54 )
#define  UI_BAL_NET_SET_RLP_SETTING        APPCAT( BALTESTAPP, 55 )
#define  UI_BAL_NET_GET_IP_ADDRESS         APPCAT( BALTESTAPP, 56 )
#define  UI_BAL_NET_GET_DNS_SERVER_IP_ADDR APPCAT( BALTESTAPP, 57 )
#define  UI_BAL_NET_GET_PEER_NAME          APPCAT( BALTESTAPP, 58 )

#define  UI_BAL_PSW_INITIATE_CALL          APPCAT( BALTESTAPP, 101 )
#define  UI_BAL_PSW_ANSWER_CALL            APPCAT( BALTESTAPP, 102 )
#define  UI_BAL_PSW_HANGUP_CALL            APPCAT( BALTESTAPP, 103 )
#define  UI_BAL_CHANGE_AKEY                APPCAT( BALTESTAPP, 104 )
#define  UI_BAL_SET_PRL_ENABLE             APPCAT( BALTESTAPP, 105 )
#define  UI_BAL_GET_ACTIVE_NAM             APPCAT( BALTESTAPP, 106 )
#define  UI_BAL_SET_ACTIVE_NAM             APPCAT( BALTESTAPP, 107 )
#define  UI_BAL_GET_AKEY_CHECKSUM          APPCAT( BALTESTAPP, 108 )
#define  UI_BAL_E911_DISABLE               APPCAT( BALTESTAPP, 109 )
#define  UI_BAL_AUTO_RETRY_ENABLE          APPCAT( BALTESTAPP, 110 )
#define  UI_BAL_AUTO_RETRY_DISABLE         APPCAT( BALTESTAPP, 111 )
#define  UI_BAL_SMS_START_REG_SRV          APPCAT( BALTESTAPP, 112 )
#define  UI_BAL_SMS_STOP_REG_SRV           APPCAT( BALTESTAPP, 113 )
#define  UI_BAL_SMS_SET_PREF_SVC_OPT       APPCAT( BALTESTAPP, 114 )
#define  UI_BAL_SMS_SET_BROADCAST_PARMS    APPCAT( BALTESTAPP, 115 )
#define  UI_BAL_SMS_SET_STORAGE            APPCAT( BALTESTAPP, 116 )
#define  UI_BAL_SMS_SEND_TEXT_MESSAGE      APPCAT( BALTESTAPP, 117 )
#define  UI_BAL_SMS_SEND_PDU_MESSAGE       APPCAT( BALTESTAPP, 118 )

#define  UI_BAL_GET_PHB_REC_PARAMS         APPCAT( BALTESTAPP, 131 )
#define  UI_BAL_GET_PHB_RECORD             APPCAT( BALTESTAPP, 132 )
#define  UI_BAL_UPDATE_PHB_RECORD          APPCAT( BALTESTAPP, 133 )
#define  UI_BAL_ERASE_PHB_RECORD           APPCAT( BALTESTAPP, 134 )
#define  UI_BAL_GET_SMS_REC_PARAMS         APPCAT( BALTESTAPP, 135 )
#define  UI_BAL_GET_SMS_RECORD             APPCAT( BALTESTAPP, 136 )
#define  UI_BAL_UPDATE_SMS_RECORD          APPCAT( BALTESTAPP, 137 )
#define  UI_BAL_ERASE_SMS_RECORD           APPCAT( BALTESTAPP, 138 )
#define  UI_BAL_CHV_GET_STATUS             APPCAT( BALTESTAPP, 139 )
#define  UI_BAL_CHV_VERIFY                 APPCAT( BALTESTAPP, 140 )
#define  UI_BAL_CHV_CHANGE                 APPCAT( BALTESTAPP, 141 )
#define  UI_BAL_CHV_DISABLE                APPCAT( BALTESTAPP, 142 )
#define  UI_BAL_CHV_ENABLE                 APPCAT( BALTESTAPP, 143 )
#define  UI_BAL_CHV_UNBLOCK                APPCAT( BALTESTAPP, 144 )
#define  UI_BAL_GET_MAX_MDN_REC_NUM        APPCAT( BALTESTAPP, 145 )
#define  UI_BAL_GET_MDN_REC                APPCAT( BALTESTAPP, 146 )
#define  UI_BAL_UPDATE_MDN_REC             APPCAT( BALTESTAPP, 147 )
#define  UI_BAL_DEL_MDN_REC                APPCAT( BALTESTAPP, 148 )
#define  UI_BAL_UTK_PROFILE_INIT           APPCAT( BALTESTAPP, 149 )
#define  UI_BAL_UTK_EXIT                   APPCAT( BALTESTAPP, 150 )
#define  UI_BAL_UTK_SMS_DOWNLOAD           APPCAT( BALTESTAPP, 151 )
#define  UI_BAL_UTK_SELECT_MENU            APPCAT( BALTESTAPP, 152 )
#define  UI_BAL_UTK_COMM_CMD_RESULT        APPCAT( BALTESTAPP, 153 )
#define  UI_BAL_UTK_GET_INPUT_DONE         APPCAT( BALTESTAPP, 154 )
#define  UI_BAL_FIRMWARE_VER_RSP_MSG       APPCAT( BALTESTAPP, 155 )

#define  UI_TEST_MAX_FILE_NAME_LEN         30
#define  UI_TEST_MAX_ANIMATION_IMAGES      10

/**********************
 * MESSAGE STRUCTURES
 **********************/
/* UI_SET_DEEP_SLEEP_TIME_SLICE_MSG */
typedef PACKED struct
{
  uint32  TimeSlice;
} UiSetDeepSleepTimeSliceMsgT;

typedef PACKED struct
{
  char FileName[UI_TEST_MAX_FILE_NAME_LEN + 1];
} UiSetRingerFileMsgT;

typedef PACKED struct
{
  char FileName[UI_TEST_MAX_FILE_NAME_LEN + 1];
} UiPlayRingerFileMsgT;

typedef PACKED struct
{
  char       FileName[UI_TEST_MAX_FILE_NAME_LEN + 1];
  uint16     TextColor;
} UiDrawBackgroundMsgT;

typedef enum
{
    UI_TEST_ANIMATION_ONE_CYCLE,
    UI_TEST_ANIMATION_FOREVER
} UiAnimationCyclesT;

typedef PACKED struct
{
  BalDispFormatT     OutImageFormat;
  UiAnimationCyclesT NumCycles;
  uint16             MSecsBetweenImages;
  uint16             MSecsAfterLastImage;
  char ImageFileName [UI_TEST_MAX_ANIMATION_IMAGES] [UI_TEST_MAX_FILE_NAME_LEN + 1];
  char MusicFileName [UI_TEST_MAX_FILE_NAME_LEN + 1];
} UiDrawAnimationMsgT;

typedef PACKED struct
{
  bool  Enabled;
} UiSetAutoAnswerMsgT;

typedef PACKED struct
{
  uint16  AutoAnswerDelay;    /* msec */
} UiSetAutoAnswerDelayMsgT;

typedef PACKED struct
{
    ExeRspMsgT  RspInfo;
} UiGetAutoAnswerReqMsgT;

typedef PACKED struct
{
  bool    Enabled;
  uint16  AutoAnswerDelay;    /* msec */
} UiGetAutoAnswerRspMsgT;

typedef PACKED struct
{
  uint8  DtmfType;     /* BAL_SOUND_BURST or BAL_SOUND_CONTINUES */
} UiSetDtmfTypeMsgT;

typedef PACKED struct
{
  bool  Mode;
} UiSetPrivacyModeMsgT;
 
/* UI_BAL_FIRMWARE_VER_RSP_MSG */
typedef PACKED struct
{
  uint8  VerInfo[3];
  uint8  TimeInfo[5];
} VersionT;

typedef PACKED struct
{
   VersionT CpVersion;
   VersionT DspmVersion;
   VersionT DspvVersion;
} FirmwareVersionT;

typedef PACKED struct
{
   uint8  Band;
	uint16 Channel;
} UiRefurbishAgingTestT;

#endif


