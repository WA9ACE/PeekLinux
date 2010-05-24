#ifndef BALUIMAPI_H
#define BALUIMAPI_H

#include "buimapi.h"
#include "balapi.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define BAL_CHV_MAX_INPUT_NUM           3
#define BAL_CHV_UNBLOCK_MAX_INPUT_NUM  10
#define BAL_UIM_MAX_REG_IDS             2
#define BAL_UIM_MAX_MDN_COUNT          10 

/* CHV definition */
typedef enum
{
  BAL_CHV_UIM_DISABLE  = 0x00, /* UIM card is not ready */
  BAL_CHV_UIM_ENABLE,   /* UIM card is enabled */
  BAL_CHV_UIM_CHV1,     /*Waiting for correct chv1(pin1) entry*/
  BAL_CHV_UIM_PUK1,     /*Waiting for UNBLOCK1 entry and new CHV1 code*/
  BAL_CHV_UIM_DEADLOCK  /* UIM card deadlock */
} BalChvUimCardStatusT;

/* CHV Status read from UIM card */
typedef PACKED struct
{
  BalChvUimCardStatusT CardStatus;
  uint8                ChvSum;       /* Sum of CHV attempt */
  uint8                PukSum;       /* Sum of UNBLOCK CHV attempt */
  uint8                Chv1LeftCount;
  uint8                Chv2LeftCount;
  uint8                Puk1LeftCount;
  uint8                Puk2LeftCount;
} BalChvStatusT;

/* Define CHV type */
typedef enum
{
  BAL_CHV1 = 1,
  BAL_CHV2
} BalChvIdT;

/* Define CHV operation response result */
typedef enum
{
  BAL_CHV_OP_SUCCESS = 0,	
  BAL_CHV_NOT_INITIALIZE, /* no CHV initialized */
  BAL_CHV_OP_FAILURE_AND_PERMIT_ATTEMPT, /*  unsuccessful CHV/UNBLOCK CHV verification, at least one attempt left */
  BAL_CHV_OP_CONTRADICTION_WITH_CHV_STATUS, /* in contradiction with CHV status */
  BAL_CHV_OP_CONTRADICTION_WITH_INVALIDATION_STATE, /* in contradiction with invalidation status */
  BAL_CHV_OP_FAILURE_AND_NO_ATTEMPT, /*unsuccessful CHV/UNBLOCK CHV verification, no attempt left;CHV/UNBLOCK CHV blocked*/
  BAL_CHV_OP_FAILURE /*failure caused by other causes */
} BalChvOpResultT;

typedef enum
{
  BAL_CHV_GET_STATUS_ERR,
  BAL_CHV_GET_STATUS_PRO_ERR,
  BAL_CHV_GET_POWERUP_STATUS_PRO_ERR,
  BAL_CHV_VERIFY_ERR,
  BAL_CHV_CHANGE_ERR,
  BAL_CHV_DISABLE_ERR,
  BAL_CHV_ENABLE_ERR,
  BAL_CHV_UNBLOCK_ERR,
  BAL_CHV_PRO_RSP_ERR,
  BAL_CHV_COMMON_PRO_ERR
} BalChvErrCode1T;

typedef enum
{
  BAL_CHV_STATUS_FUNCP_NULL = 0x1001,
  BAL_CHV_PRO_FUNCP_NULL,
  BAL_CHV_STATUS_FUNCP_BEING_USED,
  BAL_CHV_PRO_FUNCP_BEING_USED,
  BAL_CHV_MSG_ERR
} BalChvErrCode2T;


#ifdef CHV_DEBUG
/* test message */
typedef PACKED struct
{
  uint8 ChvId; /* Specify the CHV */
  uint8 ChvLen;
  uint8 ChvVal[8]; /* CHV value */	
} BalChvTstReqVerifyMsgT;
/* change CHV  msg */
typedef PACKED struct 
{
  uint8 ChvId;       /* Specify the CHV */
  uint8 OldChvLen;
  uint8 OldChvVal[8];/* Old CHV value */
  uint8 NewChvLen;
  uint8 NewChvVal[8];/* New CHV value */
} BalChvTstReqChangeMsgT;

/* Disable CHV Msg */
typedef PACKED struct 
{
  uint8 ChvLen;
  uint8 Chv1Val[8];	/* CHV1 value */
} BalChvTstReqDisableMsgT;

/* Enable CHV Msg */
typedef PACKED struct 
{
  uint8 ChvLen;
  uint8 Chv1Val[8]; /* CHV1 value */
} BalChvTstReqEnableMsgT;

/* Unblock CHV Msg */
typedef PACKED struct 
{
  uint8 ChvId;        /* Specify the CHV */
  uint8 UblkChvLen;
  uint8 UblkChvVal[8];/* Unblock CHV value */
  uint8 NewChvLen;
  uint8 NewChvVal[8]; /* New CHV value */
} BalChvTstReqUnblockMsgT;

#endif /* CHV_DEBUG */
/******************** UIM MDN structures***************************/
/* Bal MDN buffer structure(MDN is BCD type) */
typedef struct
{
  uint8 MdnLen;
  uint8 Mdn[8];
} BalUimMdnT;

/* Bal MDN structure(MDN is char type) */
typedef struct
{
  uint8 MdnLen;
  uint8 Mdn[16];
} BalUimMdnRecT;

/****** Bal Uim notify register event *****/
typedef enum
{
  BAL_UIM_EVENT_NOTIFY_REGISTER 
} BalUimEventIdT;

typedef PACKED struct
{
  bool IsReady;
} BalUimNotifyMsgT;

/* ETS Test Messages */
typedef PACKED struct
{
  uint8 nIndex;
} BalUimGenericMsgT;

typedef PACKED struct
{
  uint8 nIndex;
  uint8 Name[MAXALPHALENGTH];
  uint8 PhNum[21];
} BalUimUpdatePhbRecMsgT;

typedef PACKED struct
{
  uint8 nIndex;
  uint8 nStatus;
  uint8 MsgData[255];
  uint8 MsgLen;
} BalUimUpdateSmsRecMsgT;

typedef PACKED struct
{
  ExeRspMsgT RspInfo;
} BalChvMsgT;

typedef PACKED struct
{
  bool status;
} BalChvStatusMsgT;

typedef PACKED struct
{
  ExeRspMsgT RspInfo;
  uint8      ChvId;
  uint8      ChvLen;
  uint8      Chv[8];
} BalChvVerifyMsgT;

typedef PACKED struct
{
  ExeRspMsgT RspInfo;
  uint8      ChvId;
  uint8      OldChvLen;
  uint8      OldChv[8];
  uint8      NewChvLen;
  uint8      NewChv[8];
} BalChvChangeMsgT;

typedef PACKED struct
{
  ExeRspMsgT RspInfo;
  uint8      Chv1Len;
  uint8      Chv1[8];
} BalChvEnableDisableMsgT;

typedef PACKED struct
{
  ExeRspMsgT RspInfo;
  uint8      ChvId;
  uint8      UnblockChvLen;
  uint8      UnblockChv[8];
  uint8      NewChvLen;
  uint8      NewChv[8];
} BalChvUnblockMsgT;

typedef PACKED struct
{
  ExeRspMsgT RspInfo;
} BalUimGetMaxMdnRecsMsgT;

typedef PACKED struct
{
  uint8 numRecs;
} BalUimNumRecsMsgT;

typedef enum
{
   BAL_UIM_NT_UNKNOWN        = 0,
   BAL_UIM_NT_INTERNATIONAL,
   BAL_UIM_NT_NATIONAL,
   BAL_UIM_NT_NETWORK_SPECIFIC,
   BAL_UIM_NT_SUBSCRIBER,
   BAL_UIM_NT_RESERVED,
   BAL_UIM_NT_ABBREVIATED    = 6
} BalUimNumberTypeT;

typedef enum
{
   BAL_UIM_NP_UNKNOWN = 0,
   BAL_UIM_NP_ISDN_TELEPHONY,
   BAL_UIM_NP_RESERVED2,
   BAL_UIM_NP_DATA,
   BAL_UIM_NP_TELEX,
   BAL_UIM_NP_RESERVED5,
   BAL_UIM_NP_RESERVED6,
   BAL_UIM_NP_RESERVED7,
   BAL_UIM_NP_RESERVED8,
   BAL_UIM_NP_PRIVATE = 9
} BalUimNumberPlanT;

/*****************************************************************************

  FUNCTION NAME: BalUimInit

  DESCRIPTION:

    Create BalUimEvtLock. 

  PARAMETERS:

    None.

  RETURNED VALUES:

    None

*****************************************************************************/
void BalUimInit( void );

/********************************************************************************
 
  FUNCTION NAME: BalUimMdnInit 

  DESCRIPTION:

    This routine initiates the procedure of reading MDN parameters of the UIM. 
    
  PARAMETERS:
  
    None.
    
  RETURNED VALUES:

    None.

*********************************************************************************/
void BalUimMdnInit( void );

/********************************************************************************
 
  FUNCTION NAME: BalUimGetMaxMdnRecNum 

  DESCRIPTION:

    It's invoked to Get the number of EF MDN record in UIM card.
    
  PARAMETERS:

    None.
    
  RETURNED VALUES:

    Num: the number of MDN record.

*********************************************************************************/
uint8 BalUimGetMaxMdnRecNum( void );

/********************************************************************************
 
  FUNCTION NAME: BalUimGetMdnRec 

  DESCRIPTION:

    It's invoked to Get MDN record.
    
  PARAMETERS:

    Index:      Record index, start from 1.
    MdnRecP:    Returned Mdn pointer.
    
  RETURNED VALUES:

    TRUE: this operation is successful. 
    FALSE: failure.

*********************************************************************************/
bool BalUimGetMdnRec( uint8          Index,
                      BalUimMdnRecT* MdnRecP );

/********************************************************************************
 
  FUNCTION NAME: BalUimUpdateMdnRec 

  DESCRIPTION:

    This routine sends Update Record(6F44) message to the UIM task.
  PARAMETERS:
  
    Index: Record index, start from 1.
    Len: MDN length
    MdnP: Point to MDN buffer.
   
  RETURNED VALUES:

    TRUE: this operation is successful. 
    FALSE: failure.

*********************************************************************************/
bool BalUimUpdateMdnRec( uint8  Index,
                         uint8  Len,
                         uint8* MdnP );

/*********************************************************************************
 
  FUNCTION NAME: BalUimDelMdnRec 

  DESCRIPTION:

    This routine sends Update Record(6F44) message to the UIM task.
    
  PARAMETERS:
  
    Index: Record index, start from 1.
   
  RETURNED VALUES:

    TRUE: this operation is successful. 
    FALSE: failure.

**********************************************************************************/
bool BalUimDelMdnRec( uint8 Index );

/*****************************************************************************

  FUNCTION NAME: BalUimMdnDeliverMsg

  DESCRIPTION:

    Deliver UIM MDN response messages.  It is used by valtask.

  PARAMETERS:

    MsgId: received message id   
    MsgP: message
    MsgSize: message size	

  RETURNED VALUES:

    None

*****************************************************************************/
void BalUimMdnDeliverMsg( uint32 MsgId,
                          void*  MsgP,
                          uint32 MsgSize );

/*****************************************************************************
 
  FUNCTION NAME: BalChvGetStatus 

  DESCRIPTION:

    This routine sends UIM Get Status message to the UIM task, and saves callback 
    function's pointer in order to process CHV state when the response from the
    UIM is received.
    
  PARAMETERS:

    ChvCallbackP: Pointer to callback function

  RETURNED VALUES:

    TRUE: Get Status Msg has been sent.
    FALSE: error. callback=NULL or other task is doing this operation. 

*****************************************************************************/
bool BalChvGetStatus( void(*ChvCallbackP)(BalChvStatusT) );

/*****************************************************************************
 
  FUNCTION NAME: BalChvVerify 

  DESCRIPTION:

    This routine sends App verify CHV  message to the UIM task, and saves callback 
    function's pointer in order to process the result when the response from the 
    UIM is received.
    
  PARAMETERS:
  
    ChvId: CHV1 or CHV2
    ChvLen: Length of CHV value
    ChvP: Pointer to CHV value 
    ChvCallbackP: Pointer to callback function

  RETURNED VALUES:

    TRUE: Uim App Verify Msg has been sent.
    FALSE: error. callback=NULL or other task is doing this operation. 

*****************************************************************************/
bool BalChvVerify( BalChvIdT ChvId,
                   uint8     ChvLen,
                   uint8*    ChvP,
                   void      (*ChvCallbackP)(BalChvOpResultT) );

/*****************************************************************************
 
  FUNCTION NAME: BalChvChange 

  DESCRIPTION:

    This routine sends App change CHV  message to the UIM task, and saves callback
    function's pointer in order to process the result when the response from the 
    UIM is received.
    
  PARAMETERS:
  
    ChvId: CHV1 or CHV2
    OldChvLen: Length of old CHV value
    OldChvP: Pointer to old CHV value 
    NewChvLen: Length of new CHV value
    NewChvP: Pointer to new CHV value
    ChvCallbackP: Pointer to callback function

  RETURNED VALUES:

    TRUE: Uim App change Chv Msg has been sent.
    FALSE: error. callback=NULL or other task is doing this operation. 

*****************************************************************************/
bool BalChvChange( BalChvIdT ChvId,
                   uint8     OldChvLen,
                   uint8*    OldChvP,
                   uint8     NewChvLen,
                   uint8*    NewChvP,
                   void      (*ChvCallbackP)(BalChvOpResultT) );

/*****************************************************************************
 
  FUNCTION NAME: BalChvDisable 

  DESCRIPTION:

    This routine sends App disable CHV  message to the UIM task, and saves callback 
    function's pointer in order to process the result when the response from 
    the UIM is received.
    
  PARAMETERS:
  
    Chv1Len: Length of CHV1 value
    Chv1P: Pointer to CHV1 value 
    ChvCallbackP: Pointer to callback function

  RETURNED VALUES:

    TRUE: Uim App Disable Chv Msg has been sent.
    FALSE: error. callback=NULL or other task is doing this operation. 

*****************************************************************************/
bool BalChvDisable( uint8  Chv1Len,
                    uint8* Chv1P,
                    void   (*ChvCallbackP)(BalChvOpResultT) );

/*****************************************************************************
 
  FUNCTION NAME: BalChvEnable 

  DESCRIPTION:

    This routine sends App enable CHV message to the UIM task, and saves callback 
    function's pointer in order to process the result when the response from
    the UIM is received.
    
  PARAMETERS:
  
    Chv1Len: Length of CHV1 value
    Chv1P: Pointer to CHV1 value 
    ChvCallbackP: Pointer to callback function

  RETURNED VALUES:

    TRUE: Uim App Enable Chv Msg has been sent.
    FALSE: error. callback=NULL or other task is doing this operation. 

*****************************************************************************/
bool BalChvEnable( uint8  Chv1Len,
                   uint8* Chv1P,
                   void   (*ChvCallbackP)(BalChvOpResultT) );

/*****************************************************************************
 
  FUNCTION NAME: ChvUnblock 

  DESCRIPTION:

    This routine sends App unblock CHV  message to the UIM task, and saves callback
    function's pointer in order to process the result when the response from
    the UIM is received.
    
  PARAMETERS:
  
    ChvId: CHV1 or CHV2
    UnblockChvLen: Length of UNBLOCK CHV value
    UnblockChvP: Pointer to UNBLOCK CHV value 
    NewChvLen: Length of new CHV value
    NewChvP: Pointer to new CHV value
    ChvCallbackP: Pointer to callback function

  RETURNED VALUES:

    TRUE: Uim App Unblock Chv Msg has been sent.
    FALSE: error. callback=NULL or other task is doing this operation. 

*****************************************************************************/
bool BalChvUnblock( BalChvIdT ChvId,
                    uint8     UnblockChvLen,
                    uint8*    UnblockChvP, 
                    uint8     NewChvLen,
                    uint8*    NewChvP,
                    void      (*ChvCallbackP)(BalChvOpResultT) );

/*****************************************************************************

  FUNCTION NAME: BalChvDeliverMsg

  DESCRIPTION:

    Deliver CHV response message.  It is used by valtask.

  PARAMETERS:

    MsgId: received message id   
    MsgP: message
    MsgSize: message size	

  RETURNED VALUES:

    None

*****************************************************************************/
void BalChvDeliverMsg( uint32 MsgId,
                       void*  MsgP,
                       uint32 MsgSize );

void BalUimDeliverMsg( uint32 MsgId,
                       void*  MsgP,
                       uint32 MsgSize );

/*****************************************************************************

  FUNCTION NAME: BalUimNotifyInit

  DESCRIPTION:

    Create BalUimNotifyEventLock.

  PARAMETERS:

    None.

  RETURNED VALUES:

    None

*****************************************************************************/
void BalUimNotifyInit( void );

/*****************************************************************************

  FUNCTION NAME: BalUimRegister

  DESCRIPTION:

    Other task invokes this function to register UIM  message functions
    in Bal task.

  PARAMETERS:
    EventFunc: Callback function
	
  RETURNED VALUES:

    TRegister ID
    -1: failed.

*****************************************************************************/
RegIdT BalUimRegister( BalEventFunc EventFunc );

/*****************************************************************************

  FUNCTION NAME: BalUimUnregister

  DESCRIPTION:

    Other task invokes this function to unregister UIM Notify Register message functions

  PARAMETERS:
    RegId: Register ID
	
  RETURNED VALUES:

    None.
    
*****************************************************************************/
void BalUimUnregister( RegIdT RegId );

/* Bal Uim Phb */

void BalUimGetPhbRecParams( void );
void BalUimGetPhbRecord( uint8 nIndex );
void BalUimUpdatePhbRecord( uint8  nIndex,
                            uint8* pName,
                            uint8* pPhNum,
                            BalUimNumberTypeT TON,
                            BalUimNumberPlanT NPI
                           );
void BalUimErasePhbRecord( uint8 nIndex );

/* Bal Uim Sms */
void BalUimGetSmsRecParams( void );
void BalUimGetSmsRecord( uint8 nIndex );
void BalUimUpdateSmsRecord( uint8  nIndex,
                            uint8  nStatus,
                            uint8* pMsgData,
                            uint8  MsgLen );
void BalUimEraseSmsRecord( uint8 nIndex );


#ifdef  __cplusplus
}
#endif
#endif



