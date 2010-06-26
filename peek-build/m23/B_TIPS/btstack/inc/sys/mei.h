#ifndef __MEI_H
#define __MEI_H
/****************************************************************************
 *
 * File:
 *     $Workfile:mei.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:115$
 *
 * Description:
 *     This file contains internal definitions for the Bluetooth
 *     Management Enity.
 *
 * Created:
 *     October 18, 1999
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "me.h"
#include "sec.h"
#include "sys/mexp.h"
#include "utils.h"
#include "hcitrans.h"
#include "sys/hci.h"
#include "sys/l2capxp.h"

#if NUM_SCO_CONNS > 0
#include "mesco.h"
#endif
#if NUM_KNOWN_DEVICES > 0
#include "medev.h"
#endif

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * BtOpMask is used to indicate which operations are in progress.
 */
typedef U16 BtOpMask;    /* Used internally */
#define BOM_INQUIRY            0x0001
#define BOM_CANCEL_INQUIRY     0x0002
#define BOM_ACCESS_MODE        0x0004
#define BOM_ACCESS_MODE_AUTO   0x0008
#define BOM_HCICOMMAND_BUSY    0x0010
#define BOM_INITRADIO_BUSY     0x0020

/*---------------------------------------------------------------------------
 *
 * BtOpType defines Bluetooth operations. The typedef is in me.h
 */
#define BOP_NOP               0x00  /* No operation */
#define BOP_INQUIRY           0x01  /* Inquiry operation */
#define BOP_CANCEL_INQ        0x02  /* Cancel Inquiry operation */
#define BOP_INQUIRY_SCAN      0x03  /* Change discoverability mode */
#define BOP_PAGE_SCAN         0x04  /* Change connectability mode */
#define BOP_LINK_CONNECT      0x05  /* Initiate a link connection */
#define BOP_LINK_ACCEPT       0x06  /* Accept an incoming link */
#define BOP_LINK_REJECT       0x07  /* Reject an incoming link */
#define BOP_LINK_DISCONNECT   0x08  /* Disconnet a link */
#define BOP_AUTHENTICATE      0x09  /* Authenticate the link */
#define BOP_LINK_KEY_RSP      0x0a  /* Respond to link key request */
#define BOP_PIN_CODE_RSP      0x0b  /* Respond to pin code request */
#define BOP_AUTHORIZE         0x0c  /* Authorization  operation */
#define BOP_ENCRYPTION        0x0d  /* Encryption operation */
#define BOP_SECURITY_MODE     0x0e  /* Set security mode operation */
#define BOP_ACCESS_MODE       0x0f  /* Change accessability mode operation */
#define BOP_CANCEL_INQUIRY    0x10  /* Cancel inquiry operation */
#define BOP_REM_NAME_REQ      0x11  /* Remote name request operation */
#define BOP_LOCAL_NAME_CHG    0x12  /* Local name change */

/* Mode change operations must be continguous with no other operations
 * between. Hold must be first and exit park mode must be last.
 */
#define BOP_HOLD_MODE         0x13  /* Hold mode operation */
#define BOP_SNIFF_MODE        0x14  /* Enter sniff mode operation */
#define BOP_EXIT_SNIFF_MODE   0x15  /* Exit sniff mode */
#define BOP_PARK_MODE         0x16  /* Enter park mode */
#define BOP_EXIT_PARK_MODE    0x17  /* Exit park mode */
/* end of mode change operations */

#define BOP_SWITCH_ROLE       0x18
#define BOP_WRITE_COD         0x19  /* Write class of device */
#define BOP_GEN_COMMAND       0x1a  /* General command */
#define BOP_SCO_CONNECT       0x1b  /* Initiate a SCO link connection */
#define BOP_SCO_ACCEPT        0x1c  /* Accept an incoming link */
#define BOP_SCO_REJECT        0x1d  /* Reject an incoming link */
#define BOP_SCO_DISCONNECT    0x1e  /* Disconnet a link */
#define BOP_LINK_IN_REJECT    0x1f  /* Internal link reject */
#define BOP_SCO_VOICESET      0x20  /* Set SCO voice parameters */ 
#define BOP_WRITE_LINK_POLICY 0x21  /* Set link policy */
#define BOP_READ_BDADDR       0x22  /* Read local BD_ADDR */
#define BOP_WRITE_DEF_LNK_POL 0x23  /* Set default link policy */
#define BOP_SCO_SETUP         0x24  /* Initiate an SCO link setup (BT v1.2) */
#define BOP_CANCEL_CONNECT    0x25  /* Cancel a link connection */
#define BOP_CANCEL_REM_NAME   0x26  /* Cancel a remote name request */
#define BOP_CHANNEL_CLASS     0x27  /* Set channel classification for AFH */
#define BOP_DISCOVER_ROLE     0x28  /* Discover Role */
#define BOP_CANCEL_AUTHORIZE  0x29  /* Cancel Authorization request */
#define BOP_CHANGE_PKT_TYPE   0x2a  /* Change connection packet type */
#define BOP_BUSY              0xfe  /* Indicate op queue is busy */

/*---------------------------------------------------------------------------
 *
 * BtOpEvent defines events for the operation state machine.
 */
typedef U8 BtOpEvent;

#define BOE_NOP        0x00  /* No operation */
#define BOE_START      0x01  /* Start of operation */
#define BOE_CONT       0x02  /* Continue the operation */
#define BOE_TASK_END   0x03  /* Task finished successfully */
#define BOE_TASK_ERR   0x04  /* Task ended because of an error */

/*---------------------------------------------------------------------------
 *
 * Authenticate State of a Remote Device (BtAuthState). The typedef is in 
 * me.h
 */
#define BAS_NOT_AUTHENTICATED  0x00
#define BAS_START_AUTHENTICATE 0x01
#define BAS_WAITING_KEY_REQ    0x02
#define BAS_SENDING_KEY        0x03
#define BAS_WAITING_FOR_PIN    0x04
#define BAS_WAITING_FOR_PIN_R  0x05
#define BAS_WAITING_FOR_KEY    0x06
#define BAS_WAITING_FOR_KEY_R  0x07
#define BAS_AUTHENTICATED      0x08

/*---------------------------------------------------------------------------
 *
 * Authorize State of a Remote Device (BtAuthorizeState). The typedef is in 
 * me.h
 */
#define BAS_NOT_AUTHORIZED     0x00
#define BAS_START_AUTHORIZE    0x01
#define BAS_AUTHORIZED         0x02
#define BAS_AUTHORIZED_SERVICE 0x03

/*---------------------------------------------------------------------------
 *
 * Encyrption State of a Remote Device (BtEncryptState). The typedef is in 
 * me.h
 */
#define BES_NOT_ENCRYPTED      0x00
#define BES_START_ENCRYPT      0x01
#define BES_ENCRYPTED          0x02
#define BES_END_ENCRYPT        0x03

/*---------------------------------------------------------------------------
 *
 * Security Access State is used to keep track of the access procedure. The
 * typedef is in me.h.
 */

#define BAS_NO_ACCESS          0x00
#define BAS_AUTH_COMPLETE      0x01
#define BAS_AUTHORIZE_COMPLETE 0x02
#define BAS_ENCRYPT_COMPLETE   0x04

/*---------------------------------------------------------------------------
 *
 * Security Mode state is used to keep track of the security state. 
 */

typedef U8 BtSecurityModeState;

#define BSMS_LEVEL2       0x00
#define BSMS_START_LEVEL3 0x01
#define BSMS_LEVEL3       0x02
#define BSMS_END_LEVEL3   0x03

/*---------------------------------------------------------------------------
 *
 * Link Receive Buffer State (BtLinkRxState) is used to track the L2CAP
 * receive buffer state. The typedef is in me.h.
 */

#define BRXS_COMPLETE      0x00
#define BRXS_COPYING       0x01
#define BRXS_FORWARDING    0x02
#define BRXS_FLUSH         0x03
#define BRXS_BROADCAST     0x04
#define BRXS_DISCONNECTING 0x05

/*---------------------------------------------------------------------------
 *
 * Values used for the Scan_Enable parameter of the Write_Scan_Enable command.
 */
#define BSE_INQUIRY    0x01
#define BSE_PAGE       0x02

/*---------------------------------------------------------------------------
 *
 * Values used in flags field of MeCommandToken.
 */
#define MCTF_NEED_CHECK  0x01
#define MCTF_ASYNC       0x02

/*---------------------------------------------------------------------------
 *
 * MeTaskHandler is a pointer to a function to handle the HCI event for a
 * given task.
 */
typedef void (*MeTaskHandler)(U8 event, U8 len, U8* data);

/*---------------------------------------------------------------------------
 *
 * MeOpHandler is a pointer to a function to handle events for a
 * given operation.
 */
typedef void (*MeOpHandler)(BtOpEvent event);

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * BtInquiryOp is used to hold an inquiry operation on a queue. 
 */
typedef struct {
    BtOperation op;
    BtIac       lap;
    U8          len; /* Duration of Inquiry */
    U8          max; /* Maximum number of responses */
} BtInquiryOp;

/*---------------------------------------------------------------------------
 *
 * BtScanOp is used to hold an inquiry or page scan operation on a queue. 
 */
typedef struct {
    BtOperation        op;  /* This must be the first item */
    BOOL               nc;  /* TRUE means no connection. FALSE means connection */
    BtAccessibleMode   mode;
    BtAccessModeInfo    i;
} BtScanOp;

/*---------------------------------------------------------------------------
 *
 * BtSecurityOp is used to deal with asynchronous security requests. 
 */
typedef struct {
    BtOperation     op;
    BtRemoteDevice* remDev;
    BD_ADDR         bdAddr;
    U8              pin[16];

    /* Extra data */
    union {
        U8            pLen;
        BtEncryptMode eMode;
        
        struct {
            BtSecurityMode mode;
            BOOL           encrypt;
        } secMode;
    } x;

} BtSecurityOp;

#if NUM_BT_DEVICES == 1
/* Add one more BtSecurityOp if NUM_BT_DEVICES == 1.
 * Some radios require at least two.
 */
#define NUM_SEC_OPS     2
#else
#define NUM_SEC_OPS     NUM_BT_DEVICES
#endif

/*---------------------------------------------------------------------------
 *
 * BtConRejectOp is used to reject ACL and SCO connections. 
 */
typedef struct {
    BtOperation  op;
    BD_ADDR      bdAddr;
    U8           rejectReason;
} BtConRejectOp;

/*---------------------------------------------------------------------------
 *
 * BtMeContext contains all the data used used internally by the
 * Management Enity.
 */

typedef struct {
    ListEntry     regList;     /* List of registered global handlers */
    ListEntry     opList;      /* Operation queue */
    ListEntry     secList;     /* Security record list */
    ListEntry     cmdList;     /* List of pending commands */
    ListEntry     asyncList;   /* List of pending Async commands */
    ListEntry     pendList;    /* Pending operation list */
    BtEvent       btEvent;     /* Bluetooth event */
    BtOpMask      opMask;      /* Indicates which operations are in progress */

    BtOperation*  curOp;       /* Current operation */

    HciCommand    hciGenCmd;   /* For sending general HCI commands */
    MeTaskHandler taskHandler; /* Handle HCI events */
    U8            taskEvent;   /* Event expected for taskHandler */
    MeOpHandler   opHandler;   /* Handle operation events */

    BtHandler*    authorizeHandler;
    BtHandler*    secModeHandler;

    /* Handler for the pairing operation */
    BtHandler*    pairingHandler;

    /* Incoming connection fields */
    BtHandler*       acceptHandler;
    BtConnectionRole connectionPolicy;

    U8            stackState;
    U8            opState;
    U8            taskState;

#if NUM_SCO_CONNS > 0
    BtScoConnect        scoTable[NUM_SCO_CONNS];
    BtScoAudioSettings  vSettings;
#endif    /* NUM_SCO_CONNS > 0 */

    /* Device table */
    BtRemoteDevice devTable[NUM_BT_DEVICES];
    BtSecurityOp   secOpTable[NUM_SEC_OPS];
    U8             rxBuffTable[NUM_BT_DEVICES][L2CAP_MTU+6+L2CAP_PRELUDE_SIZE];

#if NUM_KNOWN_DEVICES > 0
    /* Non-Connected Device Selection list */
    ListEntry      deviceList;
    ListEntry      btDeviceFreeList;
    BtDeviceContext btDevices[NUM_KNOWN_DEVICES];

    BtHandler*      dsMonitor;

#if (TI_CHANGES == XA_ENABLED)
    BtHandler*      dsMonitor2;
#endif

#endif

    BtConRejectOp  rejectOp;

    U8             pendCons;   /* Number of outgoing and incoming connections in process */
    U8             activeCons; /* Number of active connections */
    U8             inHoldCons; /* Number of incoming holds */
    U8             holdCons;   /* Number of outgoing holds */

    /* Access Mode info */
    /* Current settings */
    BOOL               isLiac;   /* TRUE if LIAC or FALSE if GIAC */
    BtAccessibleMode   accModeCur;
    BtAccessModeInfo   accInfoCur;

    /* Non connected accessible info */
    BtAccessibleMode   accModeNC;
    BtAccessModeInfo   accInfoNC;

#if BT_ALLOW_SCAN_WHILE_CON == XA_ENABLED
    /* Connected accessible info */
    BtAccessibleMode    accModeC;
    BtAccessModeInfo    accInfoC;
#endif /* BT_ALLOW_SCAN_WHILE_CON */

    /* Operations */
    BtInquiryOp     inquiryOp;   /* Inquiry operation */
    BtScanOp        accOp;
    BtScanOp        accUpOp;
    BtScanOp        accDownOp;
    BtOperation     codOp;       /* Class of device operation */
    BtOperation     nameOp;      /* Write Local name operation */
    BtOperation     policyOp;    /* Write Default Link Policy operation */
    BtOperation     channelOp;   /* Write Channel Classification (AFH) operation */

    /* Security mode state and encryption setting*/
    BtSecurityModeState secModeState;
    BOOL                secModeEncrypt;

#if TI_CHANGES == XA_ENABLED

    /* Current security mode */
	BtSecurityMode		securityMode;

#endif	/* TI_CHANGES == XA_ENABLED */

    BtClassOfDevice     classOfDevice;
    BtClassOfDevice     sdpServCoD;
    BtClassOfDevice     writeCoD;

    U16             curTask;

    BtLinkPolicy    inAclPolicy;
    BtLinkPolicy    outAclPolicy;

    /* Local radio settings */
    U16             radioPageTimeout;
    BD_ADDR         bdaddr;
    const U8*       localName;
    U8              localNameLen;
    U8              btVersion;
    U8              btFeatures[8];
    EvmTimer        timer;          /* Timer for ME_RadioShutdown */
    U16             pendingEvent;   /* Pending ME_RadioShutdown event */
    BtChannelClass  channelClass;   /* Channel Classification Map for AFH */
    U8              autoMode;       /* AFH auto mode enabled */
    BOOL            commandPending;
} BtMeContext;


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Prototype:     void ME_AddOperation(BtOperation* op)
 *
 * Description:   Add an operation to the operation queue. 
 *
 * Parameters:    op - pointer to operation to add 
 *
 * Returns:       void 
 */
#define MeAddOperation(o) (InsertTailList(&MEC(opList), &((o)->op.node)))
#define MeAddOperationFront(o) (InsertHeadList(&MEC(opList), &((o)->op.node)))
#define MeAddOperationPend(o) (InsertTailList(&MEC(pendList), &((o)->op.node)))

/*---------------------------------------------------------------------------
 *
 * Prototype:     void MeStartOperation(void)
 *
 * Description:   Add an operation to the operation queue. 
 *
 * Parameters:    none 
 *
 * Returns:       void 
 */
void MeStartOperation(void);

void MeOperationEnd(void);
void MeStartInquiry(BtInquiryOp* op);
void MeReportResults(BtEventMask mask);
void MeHandleCompleteTask(U8 event, U8  len, U8* parm);

#if (TI_CHANGES == XA_ENABLED)

BtStatus MeCheckAccessibleModeNC(BtAccessibleMode mode, const BtAccessModeInfo *info);
BtStatus MeSetAccessibleModeNC(BtAccessibleMode mode, const BtAccessModeInfo *info);

#if BT_ALLOW_SCAN_WHILE_CON == XA_ENABLED
BtStatus MeCheckAccessibleModeC(BtAccessibleMode mode, const BtAccessModeInfo *info);
BtStatus MeSetAccessibleModeC(BtAccessibleMode mode, const BtAccessModeInfo *info);
#endif

#endif

void MeHandleAccessModeOp(BtOpEvent event);
BOOL MeIsScanValsLegal(U16 interval, U16 window);
void MeChangeAccessibleMode(BOOL firstCon);
BOOL MeIsAccessModeInfoEqual(const BtAccessModeInfo* info1, const BtAccessModeInfo* info2);
BtRemoteDevice* MeGetFreeRemoteDevice(void);
void MeHandleLinkConnectOp(BtOpEvent event);
void MeHandleConnectionStatusOnly(U8 event, U8  len, U8* parm);
void MeCallLinkHandlers(BtRemoteDevice* remDev);
BOOL MeHandleEvents(U8 event, U8  len, U8* parm);
void MeHandleLinkConnectReq(U8 len, U8* parm);
void MeHandleLinkAcceptOp(BtOpEvent event);
void MeHandleConnectComplete(U8 len, U8* parm);
void MeReportLinkUp(BtRemoteDevice* remDev, BtEventType eType);
BtRemoteDevice* MeMapHciToRemoteDevice(HciHandle hciHandle);
void MeDisconnectLink(BtRemoteDevice* remDev);
void MeHandleDisconnectComplete(U8 len, U8* parm);
void MeHandleLinkDisconnectOp(BtOpEvent event);
void MePendingCommandHandler(EvmTimer *timer);

BtSecurityOp* SecBtGetFreeSecurityOp(void);
#if BT_SECURITY == XA_ENABLED
void SecHandleLinkKeyReq(U8 len, U8* parm);
void SecHandlePinCodeReq(U8 len, U8* parm);
void SecHandleAuthenticateOp(BtOpEvent event);
void SecHandleLinkKeyRspOp(BtOpEvent event);
void SecHandlePinCodeRspOp(BtOpEvent event);
void SecHandleAuthenticateComplete(U8  len, U8* parm);
void SecHandleLinkKeyNotify(U8 len, U8* parm);
BtStatus SecAddAuthenticateOpToQueue(BtRemoteDevice* remDev);
BtStatus SecProcessSecurityToken(BtSecurityToken* token);
#if BT_SECURITY_TIMEOUT != 0
void SecAccessTimeoutHandler(EvmTimer *timer);
#endif /* BT_SECURITY_TIMEOUT */
void SecHandleAuthorizeOp(BtOpEvent event);
void SecHandleAuthorizeRequest(BtRemoteDevice *remDev);
void SecHandleEncryptOp(BtOpEvent event);
void SecProcessAllSecurityTokens(BtRemoteDevice* remDev);
void SecHandleEncryptChange(U8 len, U8* parm);
BtStatus SecSetSecurityMode(BtHandler* handler, BtSecurityMode mode, BOOL encrypt);
BtStatus SecSetEncryptMode(BtHandler* handler, BOOL encrypt);
void SecHandleSecurityModeOp(BtOpEvent event);
void SecReportAuthenticateResult(BtRemoteDevice* remDev);
void SecReportEncryptionResult(BtRemoteDevice* remDev, U8 mode);
void SecReportSecurityModeResult(void);
void SecHandleReturnLinkKeys(U8 len, U8* parm);
#else /* BT_SECURITY == XA_DISABLED */
void SecHandleDenyPinCodeReq(U8 len, U8 *parm);
void SecHandleDenyPinCodeRspOp(BtOpEvent event);
#endif /* BT_SECURITY */

BtRemoteDevice* MeEnumerateRemoteDevices(I8 i);
void MeReportInqCancelComplete(void);
void MeHandleRemNameReqOp(BtOpEvent event);
MeCommandToken* MeFindMatchingToken(MeCommandToken* token, ListEntry* list);
void MeReportMeCommandComplete(MeCommandToken* token);
void MeHandleRemNameReqComplete(U8 len, U8* parm);
void MeProcessToken(MeCommandToken *token);
void MeHandleLinkRejectOp(BtOpEvent event);
void MeSuspendTransmitters(void);
void MeRestartTransmitters(void);
void MeHandleHoldModeOp(BtOpEvent event);
void MeHandleModeChange(U8 len, U8* parm);
void MeHandleSniffModeOp(BtOpEvent event);
void MeHandleParkModeOp(BtOpEvent event);
void MeHandleExitParkModeOp(BtOpEvent event);
void MeHandleExitSniffModeOp(BtOpEvent event);
void MeHandleSwitchRoleOp(BtOpEvent event);
void MeHandleRoleChange(U8 len, U8* parm);
void MeHandleDiscoverRole(BtOpEvent event);
void MeWriteClassOfDevice(void);
void MeWriteDefaultLinkPolicy(void);
void MeWriteLocalName(void);
void MeChangeConnPacketTypeOp(BtOpEvent event);
BOOL MeIsValidGeneralToken(MeCommandToken* token);
void MeCheckRemDevToken(MeCommandToken *token);
BtStatus ME_SendHciCommand(MeCommandToken *token);
MeCommandToken* MeFindGeneralEvent(BtOpEvent event, U8 *parm);
void MeProcessGenToken(MeCommandToken *token);
void MeReportNMResults(void);
BtStatus MeSendHciCommand(HciCommandType opCode, U8 parmLen);
void MeHandleWriteLinkPolicyOp(BtOpEvent event);
void MeHandleCancelConnectOp(BtOpEvent event);
void MeHandleCancelRemNameOp(BtOpEvent event);
BtStatus MeCheckModeChange(BtRemoteDevice *remDev, BtOpType bop);
void MeHandleDataReqWhileNotActive(BtRemoteDevice *remDev);
U8 MeSetBits(U8 byte, U8 bit, U8 n, BOOL state);
#define MeSetBits(a, b, n, u) ((a & ((~0 << (b)) | (~(~0 << (b - n))))) |  \
                                   (((u ? 0xff : 0x00) & ~(~0 << n)) << (b - n)));


#if NUM_KNOWN_DEVICES > 0
void MeDevEventHandler(void);
void MeDevInquiryStart(void);
#if SDP_CLIENT_SUPPORT == XA_ENABLED
void MeDevSdpServiceFound(const SdpQueryToken *SdpToken);
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#else
#define MeDevEventHandler()         while (0)
#define MeDevInquiryStart()         while (0)
#define MeDevSdpServiceFound(_SDP)  while (0)
#endif /* NUM_KNOWN_DEVICES > 0 */

#if NUM_SCO_CONNS > 0
void ScoHandleLinkAcceptOp(BtOpEvent event);
BtScoConnect *ScoMapHciToConnect(HciHandle hcihandle);
void MeScoDisconnectLink(BtScoConnect *scoConnect);
void ScoHandleLinkRejectOp(BtOpEvent event);
void ScoHandleVoiceSettingsOp(BtOpEvent event);
void ScoHandleConnectComplete(U8 len, U8* parm);
void ScoHandleConnChanged(U8 len, U8* parm);
void ScoHandleLinkConnectOp(BtOpEvent event);
void ScoHandleLinkSetupOp(BtOpEvent event);
void ScoHandleLinkDisconnectOp(BtOpEvent event);
BOOL ScoHandleLinkConnectReq(U8 len, U8* parm);
BOOL ScoHandleDisconnectComplete(U8 len, U8* parm);
void ScoHandleDisconnectAcl(BtRemoteDevice *remDev);
void ScoReportScoLinkUp(BtScoConnect *scocon, BtEventType evnt);
BtScoConnect *ScoGetFreeConnect(void);

#if BT_SCO_HCI_DATA == XA_ENABLED
void SCO_Init(void);
BtStatus Sco_Send(BtScoConnect *scocon, BtPacket *Packet);
void ScoDataCallback(U8 event, HciCallbackParms *parms);
#endif /* BT_SCO_HCI_DATA == XA_ENABLED */

#endif /* NUM_SCO_CONNS */

/* -- From ME tester --------------------------------------------------- */
#if JETTEST == XA_ENABLED
extern void (*Tester_MeReceive)(U8 event, HciCallbackParms *parms);
#endif /* JETTEST == XA_ENABLED */

#endif /* __MEI_H */
