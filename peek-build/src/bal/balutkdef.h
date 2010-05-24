

#ifndef BAL_UTK_DEF_H
#define BAL_UTK_DEF_H

/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/
#include "sysdefs.h"

/*----------------------------------------------------------------------------
 Global Defines and Macros
----------------------------------------------------------------------------*/
#define MON_UTK_FAULT_UNIT MON_BAL_FAULT_UNIT


/*define the command tag*/
#define UTK_COMMAND_DETAILS_TAG 0X01
#define UTK_DEVICE_IDENT_TAG 0X02
#define UTK_RESULT_TAG 0X03
#define UTK_DURATION_TAG 0X04
#define UTK_ALPHA_IDENT_TAG 0X05
#define UTK_ADDRESS_TAG 0X06
#define UTK_CAPA_CONF_PARA_TAG 0X07
#define UTK_SUB_ADDRESS_TAG 0x08
#define UTK_CDMA_SMS_TPDU_TAG 0X48
#define UTK_TEXT_STR_TAG 0X0D
#define UTK_TONE_TAG 0X0E
#define UTK_ITEM_TAG 0X0F
#define UTK_ITEM_IDENT_TAG 0X10
#define UTK_RESP_LENGTH_TAG 0X11
#define UTK_FILE_LIST_TAG 0X12
#define UTK_LOCATION_INFO_TAG 0X13
#define UTK_IMEI_TAG 0X14
#define UTK_HELP_REQUEST_TAG 0X15
#define UTK_NET_MEASU_RESULT_TAG 0X16
#define UTK_DEFAULT_TEXT_TAG 0X17
#define UTK_ITEM_NEXT_ACT_INDIC_TAG 0X18
#define UTK_ICON_IDENT_TAG 0X1E
#define UTK_ITEM_ICON_IDENT_LIST 0X1F
#define UTK_IMMEDIATE_RESP_TAG 0X2B
#define UTK_LANGUAGE_TAG 0X2D

/*command code*/
#define UTK_SET_UP_MENU_CMD 0x25
#define UTK_SET_UP_CALL_CMD 0x10
#define UTK_REFRESH_CMD 0x01
#define UTK_MORE_TIME_CMD 0x02
#define UTK_CDMA_SEND_SMS_CMD 0x13
#define UTK_PLAY_TONE_CMD 0x20
#define UTK_DISPLAY_TEXT_CMD 0x21
#define UTK_GET_INPUT_CMD 0x23
#define UTK_SELECT_ITEM_CMD 0X24
#define UTK_SESSION_END_CMD 0x81


/*----------------------------------------------------------------------------
      BER-TLV Tags
----------------------------------------------------------------------------*/
typedef enum
{
    UTK_PROACTIVE_COMMAND_TAG = 0xD0,
    UTK_CDMA_SMSPP_DOWNLOAD_TAG = 0xD1,	
    UTK_MENU_SELECTION_TAG = 0xD3
}UtkBerTlvT;

/*----------------------------------------------------------------------------
     Device  Identities
----------------------------------------------------------------------------*/
typedef enum
{
    UTK_KEYPAD = 0x01,
    UTK_DISPLAY = 0x02,
    UTK_EARPIECE = 0x03,
    UTK_RUIM = 0x81,
    UTK_TERMINAL = 0x82,
    UTK_NETWORK = 0x83
}UtkDeviceT;


/*****************************************************
*               UTK ETS TEST SPY STRUCT
*****************************************************/

/*this is just for mon spy Ets test*/
typedef enum
{
    UTK_MON_TYPE_SET_UP_MENU = 0X01,
    UTK_MON_TYPE_SELECT_ITEM = 0X02,
    UTK_MON_TYPE_GET_INPUT  = 0X03,
    UTK_MON_TYPE_DISPLAY_TEXT = 0X04,
    UTK_MON_TYPE_SND_CDMA_SMS = 0X05,
    UTK_MON_TYPE_PLAY_TONE = 0x06,
    UTK_MON_TYPE_SET_UP_CALL = 0x07 
}UtkMonT;

typedef PACKED struct
{
    ValUtkDispParamT DispParam;
    ValUtkSoftKeyT SoftKey;
}UtkTstInitMsgT;

typedef PACKED struct
{
    uint8 CodeScheme;
    uint8 Len;
    uint8 Text[30];
}UtkTstGetInputDoneMsgT;

typedef PACKED struct
{
    uint8 ItemId;
    bool Help;
}UtkTstSelectMenuMsgT;

typedef PACKED struct
{
    uint8 ItemId;
    bool Help;
}UtkTstSelectItemDoneMsgT;


typedef PACKED struct
{
    uint8 ItemId;
    uint8 CodeScheme;
    uint8 ItemLen;
    uint8 ItemText[30];
}ValUtkTstItem;

typedef PACKED struct
{
    ValUtkCmdDetailT Cmd;
    uint8 Alphacoding;
    uint8 AlphaLen;
    uint8 Alpha[30];
    uint8 ItemCount;
    ValUtkTstItem Item[10];
    bool NaiPre;
    bool IconPre;
    bool ItemIconListPre;
}ValUtkMonSetUpMenuT;

typedef PACKED struct
{
    ValUtkCmdDetailT Cmd;
    bool  AlphaPre;
    uint8 Alphacoding;
    uint8 AlphaLen;
    uint8 Alpha[30];
    uint8 ItemCount;
    ValUtkTstItem Item[10];
    bool DefItemIdPre;
    uint8 DefItemId;
    bool NaiPre;
    bool IconPre;
    bool ItemIconListPre;
}UtkMonSelectItemT;

typedef PACKED struct
{
    ValUtkCmdDetailT Cmd;
    uint8 TextScheme;
    uint8 TextLen;
    uint8 Text[30];
    ValUtkRspLenT RspLen;
    bool DefTextPre;
    uint8 DefTextScheme;
    uint8 DefTextLen;	
    uint8 DefText[30];
    bool IconPre;
}UtkMonGetInputT;

typedef PACKED struct
{
    ValUtkCmdDetailT Cmd;
    uint8 TextScheme;
    uint8 TextLen;
    uint8 Text[30];
    bool ImmeRsp;
    bool IconPre;
    bool DurationPre;
    ValUtkDurationT Duration;
}UtkMonDisplayTextT;

/*  Test Address  */
typedef PACKED struct
{
    ValUtkNpiT Npi;
    ValUtkTonT Ton;
    uint8 Len;
    uint8 DialNumber[20];
}UtkTstAddressT;

/*  Send SMS  */
typedef PACKED struct
{
    ValUtkCmdDetailT Cmd;
    bool AlphaPre;
    uint8 Alphacoding;
    uint8 AlphaLen;
    uint8 Alpha[30];
    bool AddressPre;
    UtkTstAddressT Address;
    uint8 SmsLen;
    uint8 SmsPdu[100];
}UtkMonSndCdmaSmsT;

typedef PACKED struct
{
    ValUtkCmdDetailT Cmd;
    bool AlphaPre;
    uint8 Alphacoding;
    uint8 AlphaLen;
    uint8 Alpha[30];
    bool TonePre;
    uint8 Tone;
    bool DurationPre;
    ValUtkDurationT Duration;
    bool IconPre;	
}UtkMonPlayToneT;

typedef PACKED struct
{
    ValUtkCmdDetailT Cmd;
    bool  ConfirmAlphaPre;
    uint8 CAlphacoding;
    uint8 ConfirmAlphaLen;
    uint8 ConfirmAlpha[30];
    UtkTstAddressT Address;
    bool CapCfgPre;
    uint8 CapCfgLen;
    uint8 CapCfg[20];
    bool SubaddressPre;
    uint8 SubaddressLen;
    uint8 Subaddress[20];
    bool DurationPre;
    ValUtkDurationT Duration;
    bool ConfirmIconPre;
    bool SetupAlphaPre;
    uint8 SAlphacoding;
    uint8 SetupAlphaLen;
    uint8 SetupAlpha[30];
    bool SetupIconPre;	
}UtkMonSetUpCallT;

/* SMS-PP data download */
typedef PACKED struct
{
    ValUtkSmsAddressT Address;
    uint16 MsgId;
    uint8 SmsLen;
    uint8 SmsPdu[100];
}UtkTstSmsDownMsgT;


typedef enum
{
    UTK_PROACTIVE_CMD_TAG_ERR = 0x50,
    UTK_PROACTIVE_CMD_LEN_ERR,
    UTK_PROACTIVE_CMD_NUM_ERR,
    UTK_PROACTIVE_CMD_TYPE_ERR,
    UTK_PROACTIVE_CMD_SIP_TAG_ERR,
    UTK_PROACTIVE_CMD_DEVICE_ERR,
    UTK_PROACTIVE_CMD_MISS_DEVICE,
    UTK_PROACTIVE_CMD_MISS_ALPHA,
    UTK_PROACTIVE_CMD_MISS_ITEM,
    UTK_PROACTIVE_CMD_MISS_ADDRESS,
    UTK_PROACTIVE_CMD_MISS_RSPLEN,	
    UTK_PROACTIVE_CMD_MISS_SMSPDU,
    UTK_CMD_DETAIL_LEN_MISMATCH,
    UTK_DEVICE_LEN_MISMATCH,    
    UTK_ICON_LEN_MISMATCH,    
    UTK_DURATION_LEN_MISMATCH,    
    UTK_TEXTRSP_LEN_MISMATCH,    
    UTK_TONE_LEN_MISMATCH,
    UTK_PROACTIVE_CMD_LEN_MISMATCH,
    UTK_PRO_MENU_SELECTION_RSP_ERR,
    UTK_PRO_TERM_RSP_RSP_ERR
}UtkErrCodeT;

 /* UTK SMS Download Cause Code IDs */
typedef enum
{
    UTK_BUSY = 33,
    UTK_DATA_DOWNLOAD_ERROR = 39
}UtkSmsUserRspCodeT;
 
#endif	
	

	
