#ifndef __ME_H
#define __ME_H

/****************************************************************************
 *
 * File:
 *     $Workfile:me.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:207$
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     upper layer of the Management Entity.
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

#include "osapi.h"
#include "bttypes.h"
#include "radiomgr.h"

/*---------------------------------------------------------------------------
 * Management Entity (ME) layer
 *
 *     The Management Entity allows applications to inquire for
 *     remote devices, establish different types of connections, and
 *     control the access to this device by other devices.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BT_INQ_TIME_GAP100 constant
 *     Time value specified by Generic Access Profile for the length
 *     of an Inquiry process.
 */
#define BT_INQ_TIME_GAP100 8

/*---------------------------------------------------------------------------
 * BT_MAX_REM_DEV_NAME constant
 *
 *     Maximum length of a Remote Device name (friendly name) allowed by 
 *     Bluetooth. This does not include the null termination at the end.
 */
#define BT_MAX_REM_DEV_NAME 248


/*---------------------------------------------------------------------------
 *
 * The following are used to indicate the use of default values for Window
 * and interval used in inquiry and page scanning. The default values are
 * the default values of the radio module being used. These values should
 * within the range defined by the Generic Access Profile.
 */
#define BT_DEFAULT_SCAN_INTERVAL 0x0000
#define BT_DEFAULT_SCAN_WINDOW   0x0000

/*---------------------------------------------------------------------------
 * BT_INVALID_RSSI constant
 *
 *     Indicates an invalid RSSI value (used when RSSI is not reported
 *     by the controller).
 */
#define BT_INVALID_RSSI   127

#if TI_CHANGES == XA_ENABLED
	#define SAP_MIN_PIN_LEN					16
#endif


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtEventType type
 *
 *     All indications and confirmations are sent through a callback
 *     function. The event types are defined below.
 */
typedef U8 BtEventType;

/* Group: Global events sent to all handlers. These can be masked out if
 * desired.
 */

/** A remote device is found. "p.inqResult" is valid.  */
#define BTEVENT_INQUIRY_RESULT       1 

/** The Inquiry process is completed. "errCode" is valid. */
#define BTEVENT_INQUIRY_COMPLETE     2

/** The Inquiry process is canceled. */
#define BTEVENT_INQUIRY_CANCELED     3

/** An incoming ACL connection. "p.remDev" is valid. */
#define BTEVENT_LINK_CONNECT_IND     4

/** An incoming SCO connection. "p.scoConnect" is valid. */
#define BTEVENT_SCO_CONNECT_IND      5

/** An ACL connection was disconnected. "p.disconnect" and
 *  "errCode" are valid. */
#define BTEVENT_LINK_DISCONNECT      6

/* An outgoing ACL connection is up. "p.remDev" is valid. */
#define BTEVENT_LINK_CONNECT_CNF     7

/** A restricted connection is up. "p.remDev" is valid. */
#define BTEVENT_LINK_CON_RESTRICT    8

/** Indicates the mode of the link changed. "p.modeChange" is valid */
#define BTEVENT_MODE_CHANGE          9

/** Indicates that an accessibility change is complete. "errCode" and
 *  "p.aMode" are valid. */
#define BTEVENT_ACCESSIBLE_CHANGE   10

/** Indicates that a link has been authenticated. "p.remDev" is valid. */
#define BTEVENT_AUTHENTICATED       11

/** Encryption settings have changed. "p.encrypt" is valid. */
#define BTEVENT_ENCRYPTION_CHANGE   12

/** Security mode has changed. "p.secMode" is valid. */
#define BTEVENT_SECURITY_CHANGE     13

/** Indicates that a role change has occurred */
#define BTEVENT_ROLE_CHANGE         14

/** SCO link has been disconnected. "p.scoConnect" is valid. */
#define BTEVENT_SCO_DISCONNECT      15

/** An outgoing SCO link has been established. "p.scoConnect" is valid. */
#define BTEVENT_SCO_CONNECT_CNF     16


/** Indicates that an ACL connection has received an internal data transmit
 *  request while it is in hold, park or sniff mode. The data will still be 
 *  passed to the radio in park and sniff modes. However, hold mode will
 *  block data transmit. It may be necessary to return the ACL to active
 *  mode to restore normal data transfer. "p.remDev" is valid.
 */
#define BTEVENT_ACL_DATA_NOT_ACTIVE 99

/* Group: The following events are global events but cannot be masked */

/** Indicates that the HCI failed to initialize. This implies that the 
 *  Bluetooth radio is having problems or a radio may not exist. 
 *  Events will be generated with an appropriate error code for any outstanding
 *  operations.  All pending operations that use pointers to memory supplied by 
 *  applications will have a corresponding event generated indicating the 
 *  failure of the operation. Applications should not modify memory passed as 
 *  part of operations until an event signaling the end of the operation
 *  has been received. The HCI and the radio will be reset. If the reset is 
 *  successful then the BTEVENT_HCI_INITIALIZED event will be sent to all 
 *  registered handlers.
 */
#define BTEVENT_HCI_INIT_ERROR      100

/** Indicates that the HCI initialization is successful. The ME will now
 *  accept commands again if an initialization error or fatal error
 *  has occurred. This event is sent whenever HCI is successfully initialized,
 *  including when the stack is first started.
 */
#define BTEVENT_HCI_INITIALIZED     101

/** Indicates that a fatal error has occurred in the radio or the HCI transport.
 *  The HCI and the radio will be reset. If the reset is successful then the 
 *  BTEVENT_HCI_INITIALIZED event will be sent to all registered handlers.
 */
#define BTEVENT_HCI_FATAL_ERROR     102

/** Indicates that the HCI has been deinitialized.  This can happen as the 
 *  result of a call to ME_RadioShutdown, or if some fatal error was reported
 *  by the radio or HCI transport. For any pending operations, events will 
 *  be received with proper error codes prior to receiving this event. 
 *  All pending commands that used memory supplied by the application will 
 *  result in an event, so the application should not modify that memory until 
 *  the event has been received. If the HCI was deinitialized as the result of
 *  a radio or transport error, the HCI will be reset. If the reset is 
 *  successful then the BTEVENT_HCI_INITIALIZED event will be sent to all 
 *  registered handlers.
 */
#define BTEVENT_HCI_DEINITIALIZED   103

/** Indicates that the HCI cannot be initialized. This happens when
 *  the HCI has been reset BT_HCI_NUM_INIT_RETRIES without success or
 *  if some unrecoverable error occurs.
 */
#define BTEVENT_HCI_FAILED          104

/** Indicates that an HCI command has been sent to the radio. This
 *  event is intended for global handlers that would like to be informed
 *  of when certain radio processes are started. "p.hciCmd" is valid.
 */
#define BTEVENT_HCI_COMMAND_SENT    105

/* An outgoing create link has been canceled. "p.remDev" is valid. */
#define BTEVENT_LINK_CREATE_CANCEL  106

/* Group: These events are sent only to the handler that initiated the
 * operation.
 */

/** Indicates the name of a remote device or cancellation of a name request.
 *  "p.meToken" is valid.
 */
#define BTEVENT_NAME_RESULT         17

/** Incoming SCO data has been received. Fields in "p.scoDataInd" are valid
 *  and describe the data received. This data is valid only for the duration
 *  of the callback.
 */
#define BTEVENT_SCO_DATA_IND        18

/** Outgoing SCO data has been sent and the packet is free for re-use by
 *  the application. "p.scoPacketHandled.scoPacket" and
 *  "p.scoPacketHandled.scoCon" are valid.
 */
#define BTEVENT_SCO_DATA_CNF        19

/** Incoming connection request. "p.remDev" is valid. */
#define BTEVENT_LINK_CONNECT_REQ    20

/** Incoming link accept complete. "p.remDev" is valid. */
#define BTEVENT_LINK_ACCEPT_RSP     21

/** Incoming link reject complete. "p.remDev" is valid. */
#define BTEVENT_LINK_REJECT_RSP     22

/** General command (async or sync) has completed. "p.meToken" is valid */
#define BTEVENT_COMMAND_COMPLETE    23

/** Incoming connection request. "p.scoConnect" is valid. The field
 *  "p.scoConnect.scoLinkType" indicates the SCO link type (BLT_SCO or
 *  BLT_ESCO.) When the link is of type BLT_ESCO, the fields
 *  p.scoConnect.scoTxParms and p.scoConnect.scoRxParms point to the
 *  associated eSCO connection parameters.
 */
#define BTEVENT_SCO_CONNECT_REQ     24

/** Set Audio/Voice settings complete.  */
#define BTEVENT_SCO_VSET_COMPLETE   25

/** SCO link connection process started. */
#define BTEVENT_SCO_STARTED         26

/** Select Device operation complete, "p.select" is valid. */
#define BTEVENT_DEVICE_SELECTED     27

/** The eSCO connection has changed. "p.scoConnect" is valid.
 *  The structure "p.scoConnect.scoRxParms" contains the updated
 *  connection parameters.
 */
#define BTEVENT_SCO_CONN_CHNG       28

/* Group: Security-related events. */

/** Indicates access request is successful. "p.secToken" is valid. */
#define BTEVENT_ACCESS_APPROVED     40

/** Indicates access request failed. "p.secToken" is valid. */
#define BTEVENT_ACCESS_DENIED       41

/** Request authorization when "errCode" is BEC_NO_ERROR.
 *  "p.remDev" is valid.
 */
#define BTEVENT_AUTHORIZATION_REQ   42

/** Request a Pin for pairing when "errCode" is BEC_NO_ERROR.
 *  "p.remDev" is valid.
 */
#define BTEVENT_PIN_REQ             43

/** Pairing operation is complete. "p.remDev" is valid. */
#define BTEVENT_PAIRING_COMPLETE    44

/** Authentication operation complete. "p.remDev" is valid. */
#define BTEVENT_AUTHENTICATE_CNF    45

/** Encryption operation complete. "p.remDev" is valid. */
#define BTEVENT_ENCRYPT_COMPLETE    46

/** Security mode 3 operation complete. "p.secMode" is valid. */
#define BTEVENT_SECURITY3_COMPLETE  47 

/** A link key is returned. "p.bdLinkKey" is valid.  */
#define BTEVENT_RETURN_LINK_KEYS    48

/* Group: SDP Query related events. In all cases, the "p.token" parameter
 * is valid.
 */

/** SDP query completed successfully. */
#define SDEVENT_QUERY_RSP           50

/** SDP query completed with an error */
#define SDEVENT_QUERY_ERR           51

/** SDP query failed */
#define SDEVENT_QUERY_FAILED        52 

/* Group: Device Selection monitor events. These events are only sent to
 * the device selection handler. 
 */

/** Request device selection, "p.select" is valid. The device selection
 *  monitor must call DS_SelectDeviceResult() to return the provided
 *  selection request when the selection process is complete.
 */
#define BTEVENT_SELECT_DEVICE_REQ   60

/** Device added to the device database, "p.device" is valid. Note that
 *  during a device inquiry, BTEVENT_DEVICE_ADDED event may not mirror
 *  BTEVENT_INQUIRY_RESULT events. This is because duplicate devices are
 *  not added to the device database and because the device database may
 *  reach capacity, preventing the addition of new devices.
 */
#define BTEVENT_DEVICE_ADDED        61

/** Device removed from the device database, "p.device" is valid. */
#define BTEVENT_DEVICE_DELETED      62

#if TI_CHANGES == XA_ENABLED 

#define BTEVENT_OPEN				120

#define BTEVENT_SEARCH_COMPLETE				BTEVENT_OPEN
#define BTEVENT_SEARCH_CANCELLED				BTEVENT_OPEN + 1
#define BTEVENT_SEND_HCI_COMMAND_RESULT		BTEVENT_OPEN + 2
#define BTEVENT_DISCOVER_SERVICES_RESULT		BTEVENT_OPEN + 3
#define BTEVENT_DISCOVER_SERVICES_CANCELLED	BTEVENT_OPEN + 4
#define BTEVENT_BOND_RESULT					BTEVENT_OPEN + 5
#define BTEVENT_BOND_CANCELLED				BTEVENT_OPEN + 6
#define BTEVENT_ENABLE_TEST_MODE_RESULT		BTEVENT_OPEN + 7
#define BTEVENT_SET_SECURITY_MODE_RESULT	BTEVENT_OPEN + 8
#define BTEVENT_SERVICE_SEARCH_ATTRIBUTES_RESULT	BTEVENT_OPEN + 9
#define BTEVENT_SERVICE_SEARCH_ATTRIBUTES_COMPLETE	BTEVENT_OPEN + 10
#define BTEVENT_SERVICE_SEARCH_ATTRIBUTES_CANCELLED BTEVENT_OPEN + 11
#define BTEVENT_SERVICE_SEARCH_COMPLETE	BTEVENT_OPEN + 12
#define BTEVENT_SERVICE_SEARCH_CANCELLED BTEVENT_OPEN + 13
#define BTEVENT_SERVICE_ATTRIBUTES_COMPLETE	BTEVENT_OPEN + 14
#define BTEVENT_SERVICE_ATTRIBUTES_CANCELLED BTEVENT_OPEN + 15



#endif
/* End of BtEventType */


/*---------------------------------------------------------------------------
 * BtEventMask type
 *
 *     All registered handlers can receive global events. Each handler has
 *     the ability to mask all or some of the events. A handler receives only
 *     events with the mask set. They can be ORed together.
 */
typedef U16 BtEventMask;

#define BEM_NO_EVENTS           0x0000
#define BEM_INQUIRY_RESULT      0x0001  
#define BEM_INQUIRY_COMPLETE    0x0002  
#define BEM_INQUIRY_CANCELED    0x0004
#define BEM_LINK_CONNECT_IND    0x0008
#define BEM_SCO_CONNECT_IND     0x0010
#define BEM_LINK_DISCONNECT     0x0020
#define BEM_LINK_CONNECT_CNF    0x0040
#define BEM_LINK_CON_RESTRICT   0x0080
#define BEM_MODE_CHANGE         0x0100
#define BEM_ACCESSIBLE_CHANGE   0x0200
#define BEM_AUTHENTICATED       0x0400
#define BEM_ENCRYPTION_CHANGE   0x0800
#define BEM_SECURITY_CHANGE     0x1000
#define BEM_ROLE_CHANGE         0x2000
#define BEM_SCO_DISCONNECT      0x4000
#define BEM_SCO_CONNECT_CNF     0x8000

#define BEM_ALL_EVENTS          0xffff

/* End of BtEventMask */

/*---------------------------------------------------------------------------
 * BtErrorCode type
 */
typedef U8 BtErrorCode;

#define BEC_NO_ERROR             0x00 /* No error */ 
#define BEC_UNKNOWN_HCI_CMD      0x01 /* Unknown HCI Command */
#define BEC_NO_CONNECTION        0x02 /* No connection */
#define BEC_HARDWARE_FAILURE     0x03 /* Hardware Failure */
#define BEC_PAGE_TIMEOUT         0x04 /* Page timeout */
#define BEC_AUTHENTICATE_FAILURE 0x05 /* Authentication failure */
#define BEC_MISSING_KEY          0x06 /* Missing key */
#define BEC_MEMORY_FULL          0x07 /* Memory full */
#define BEC_CONNECTION_TIMEOUT   0x08 /* Connection timeout */
#define BEC_MAX_CONNECTIONS      0x09 /* Max number of connections */
#define BEC_MAX_SCO_CONNECTIONS  0x0a /* Max number of SCO connections to a device */
#define BEC_ACL_ALREADY_EXISTS   0x0b /* The ACL connection already exists. */
#define BEC_COMMAND_DISALLOWED   0x0c /* Command disallowed */
#define BEC_LIMITED_RESOURCE     0x0d /* Host rejected due to limited resources */
#define BEC_SECURITY_ERROR       0x0e /* Host rejected due to security reasons */
#define BEC_PERSONAL_DEVICE      0x0f /* Host rejected (remote is personal device) */
#define BEC_HOST_TIMEOUT         0x10 /* Host timeout */
#define BEC_UNSUPPORTED_FEATURE  0x11 /* Unsupported feature or parameter value */
#define BEC_INVALID_HCI_PARM     0x12 /* Invalid HCI command parameters */
#define BEC_USER_TERMINATED      0x13 /* Other end terminated (user) */
#define BEC_LOW_RESOURCES        0x14 /* Other end terminated (low resources) */
#define BEC_POWER_OFF            0x15 /* Other end terminated (about to power off) */
#define BEC_LOCAL_TERMINATED     0x16 /* Terminated by local host */
#define BEC_REPEATED_ATTEMPTS    0x17 /* Repeated attempts */
#define BEC_PAIRING_NOT_ALLOWED  0x18 /* Pairing not allowed */
#define BEC_UNKNOWN_LMP_PDU      0x19 /* Unknown LMP PDU */
#define BEC_UNSUPPORTED_REMOTE   0x1a /* Unsupported Remote Feature */
#define BEC_SCO_OFFSET_REJECT    0x1b /* SCO Offset Rejected */
#define BEC_SCO_INTERVAL_REJECT  0x1c /* SCO Interval Rejected */
#define BEC_SCO_AIR_MODE_REJECT  0x1d /* SCO Air Mode Rejected */
#define BEC_INVALID_LMP_PARM     0x1e /* Invalid LMP Parameters */
#define BEC_UNSPECIFIED_ERR      0x1f /* Unspecified Error */
#define BEC_UNSUPPORTED_LMP_PARM 0x20 /* Unsupported LMP Parameter Value */
#define BEC_ROLE_CHG_NOT_ALLOWED 0x21 /* Role Change Not Allowed */        
#define BEC_LMP_RESPONSE_TIMEOUT 0x22 /* LMP Response Timeout */           
#define BEC_LMP_TRANS_COLLISION  0x23 /* LMP Error Transaction Collision */
#define BEC_LMP_PDU_NOT_ALLOWED  0x24 /* LMP PDU Not Allowed */            
#define BEC_ENCRYP_MODE_NOT_ACC  0x25 /* Encryption Mode Not Acceptable */
#define BEC_UNIT_KEY_USED        0x26 /* Unit Key Used */
#define BEC_QOS_NOT_SUPPORTED    0x27 /* QoS is Not Supported */
#define BEC_INSTANT_PASSED       0x28 /* Instant Passed */
#define BEC_PAIR_UNITKEY_NO_SUPP 0x29 /* Pairing with Unit Key Not Supported */
#define BEC_NOT_FOUND            0xf1 /* Item not found */
#define BEC_REQUEST_CANCELLED    0xf2 /* Pending request cancelled */

#if TI_CHANGES == XA_ENABLED 

#define BEC_CONNECTION_FAILED		0xf3
#define BEC_PARTIAL_SUCCESS	0xf4
#define BEC_INTERNAL_ERROR			0xf5
#define BEC_EMPTY					0xf6
#define BEC_FAILED					0xf7
#define BEC_OVERFLOW				0xf8
#define BEC_SDP_QUERY_FAILED		0xf9

#endif

/* Group: The following error codes are used when the
 * SDEVENT_QUERY_FAILED event is sent.
 */
#define BEC_INVALID_SDP_PDU      0x01 /* SDP response PDU is invalid */
#define BEC_SDP_DISCONNECT       0x02 /* The SDP L2CAP channel or link disconnected */
#define BEC_SDP_NO_RESOURCES     0x03 /* Not enough L2CAP resources */
#define BEC_SDP_INTERNAL_ERR     0x04 /* Some type of internal stack error */

/* Group: The following error code is used when the
 * BTEVENT_PAIRING_COMPLETE event is sent.
 */
#define BEC_STORE_LINK_KEY_ERR   0xf0

/* End of BtErrorCode */


/*---------------------------------------------------------------------------
 * BtIac type
 *     Bluetooth Inquiry Access Codes are used to specify which types of
 *     devices should respond to inquiries. Currently there are only two
 *     defined.
 */
typedef U32 BtIac;

#define BT_IAC_GIAC 0x9E8B33   /* General/Unlimited Inquiry Access Code */
#define BT_IAC_LIAC 0x9E8B00   /* Limited Dedicated Inquiry Access Code */

/* End of BtIac */


/*---------------------------------------------------------------------------
 * BtLinkType type
 *
 *     Bluetooth Link type indicates the link type in the
 *     BtScoConnect link structure for SCO commands and operations.
 */
typedef U8 BtLinkType;

#define BLT_SCO   0x00
#define BLT_ACL   0x01
#define BLT_ESCO  0x02

/* End of BtLinkType */

/*---------------------------------------------------------------------------
 * BtAclPacketType type
 *     Bluetooth ACL Packet types are the different packet types allowed on
 *     an ACL link. They can be ORed together.
 */
typedef U16 BtAclPacketType;

#define BAPT_DM1   0x0008
#define BAPT_DH1   0x0010
#define BAPT_AUX1  0x0200
#define BAPT_DM3   0x0400
#define BAPT_DH3   0x0800
#define BAPT_DM5   0x4000
#define BAPT_DH5   0x8000

/* The set of packet types for a reliable link that follows the Serial port
 * profile.
 */
#define BAPT_STANDARD (BAPT_DM1 | BAPT_DH1 | BAPT_DM3 | BAPT_DH3 |\
                       BAPT_DM5 | BAPT_DH5)

/* End of BtAclPacketType */


/*---------------------------------------------------------------------------
 * BtQosServiceType type
 */
typedef U8 BtQosServiceType;

#define BQST_NO_TRAFFIC  0x00
#define BQST_BEST_EFFORT 0x01
#define BQST_GUARANTEED  0x02

/* End of BtQosServiceType */


/*---------------------------------------------------------------------------
 * BtLinkPolicy type
 * 
 *     Bluetooth Link Policy specifies which Link modes the Link Manager
 *     can use for a given link. Multiple policies can be enabled by
 *     performing a bit wise OR operation.
 */
typedef U16 BtLinkPolicy;

#define BLP_DISABLE_ALL         0x0000
#define BLP_MASTER_SLAVE_SWITCH 0x0001
#define BLP_HOLD_MODE           0x0002
#define BLP_SNIFF_MODE          0x0004
#define BLP_PARK_MODE           0x0008
#define BLP_SCATTER_MODE        0x0010

/* Mask must be updated if new policy values are added */
#define BLP_MASK                0xfff0  /* Disables ScatterNet bit */
#define BLP_SCATTER_MASK        0xffe0  /* Enables ScatterNet bit */

/* End of BtLinkPolicy */


/*---------------------------------------------------------------------------
 * BtAccessibleMode type
 *
 *     Bluetooth Accessibility mode includes Discoverable and connectable
 *     modes.
 */
typedef U8 BtAccessibleMode;

#define BAM_NOT_ACCESSIBLE     0x00 /* Non-discoverable or connectable */
#define BAM_GENERAL_ACCESSIBLE 0x03 /* General discoverable and connectable */
#define BAM_LIMITED_ACCESSIBLE 0x13 /* Limited discoverable and connectable */
#define BAM_CONNECTABLE_ONLY   0x02 /* Connectable but not discoverable */
#define BAM_DISCOVERABLE_ONLY  0x01 /* Discoverable but not connectable */

/* End of BtAccessibleMode */


/*---------------------------------------------------------------------------
 * BtLinkMode type
 */
typedef U8 BtLinkMode;

#define BLM_ACTIVE_MODE     0x00
#define BLM_HOLD_MODE       0x01
#define BLM_SNIFF_MODE      0x02
#define BLM_PARK_MODE       0x03
#define BLM_SCATTER_MODE    0x04

/* End of BtLinkMode */


/*---------------------------------------------------------------------------
 * BtConnectionRole type
 *
 *     Specifies the role of a ACL connection
 */
typedef U8 BtConnectionRole;

#define BCR_MASTER   0x00
#define BCR_SLAVE    0x01
#define BCR_ANY      0x02
#define BCR_UNKNOWN  0x03

/* === Internal use only === */
#define BCR_PSLAVE   0x04
#define BCR_PMASTER  0x05

/* End of BtConnectionRole */

/*---------------------------------------------------------------------------
 * BtStackState type
 *
 *     State of the Bluetooth protocol stack and radio.
 */
typedef U8 BtStackState;

/* The stack has completed initialization of the radio hardware. */
#define BTSS_NOT_INITIALIZED 0  

/* The stack is initialized. */
#define BTSS_INITIALIZED     1

/* The stack has encountered an error while initializing the radio hardware. */
#define BTSS_INITIALIZE_ERR  2

/* The stack is deinitializing. */
#define BTSS_DEINITIALIZE    3

/* End of BtStackState */

/*---------------------------------------------------------------------------
 * BtRadioFeature
 *
 *     Codes for radio features as defined in LMP.
 */
typedef U8 BtRadioFeature;

#define BRF_3_SLOT_PACKETS               0
#define BRF_5_SLOT_PACKETS               1
#define BRF_ENCRYPTION                   2
#define BRF_SLOT_OFFSET                  3
#define BRF_TIMING_ACCURACY              4
#define BRF_ROLE_SWITCH                  5
#define BRF_HOLD_MODE                    6
#define BRF_SNIFF_MODE                   7
#define BRF_PARK_STATE                   8
#define BRF_POWER_CONTROL_REQ            9
#define BRF_CQDDR                        10
#define BRF_SCO_LINK                     11
#define BRF_HV2_PACKETS                  12
#define BRF_HV3_PACKETS                  13
#define BRF_ULAW_SCO_DATA                14
#define BRF_ALAW_SCO_DATA                15
#define BRF_CVSD_SCO_DATA                16
#define BRF_PAGING_PARAM_NEG             17
#define BRF_POWER_CONTROL                18
#define BRF_TRANSPARENT_SCO_DATA         19
#define BRF_FLOW_CONTROL_LAG_1           20
#define BRF_FLOW_CONTROL_LAG_2           21
#define BRF_FLOW_CONTROL_LAG_3           22
#define BRF_BROADCAST_ENCRYPTION         23

#define BRF_ENHANCED_INQUIRY_SCAN        27
#define BRF_INTERLACED_INQUIRY_SCAN      28
#define BRF_INTERLACED_PAGE_SCAN         29
#define BRF_RSSI_WITH_INQUIRY_RESULTS    30
#define BRF_ESCO_EV3_PACKETS             31
#define BRF_ESCO_EV4_PACKETS             32
#define BRF_ESCO_EV5_PACKETS             33
        
#define BRF_AFH_CAPABLE_SLAVE            35
#define BRF_AFH_CLASSIFICATION_SLAVE     36
        
#define BRF_AFH_CAPABLE_MASTER           43
#define BRF_AFH_CLASSIFICATION_MASTER    44

#define BRF_EXTENDED_FEATURES            63

/* Forward declaration of the callback parameters */
typedef struct _BtEvent BtEvent;

/* Forward declaration of the SCO connect structures */
typedef struct _BtScoConnect BtScoConnect;
typedef struct _BtScoTxParms BtScoTxParms;
typedef struct _BtScoRxParms BtScoRxParms;

#if TI_CHANGES == XA_ENABLED 
#ifndef SDP_SERVICE_CLASS_UUID
#define SDP_SERVICE_CLASS_UUID
typedef U16 SdpServiceClassUuid;
#endif
#endif

/*---------------------------------------------------------------------------
 * BtCallBack type
 *
 *     A function of this type is called to indicate general Bluetooth and
 *     ME events.
 */
typedef void (*BtCallBack)(const BtEvent*);

/* End of BtCallBack */


/*---------------------------------------------------------------------------
 * BtProtocolId type
 *
 *     The protocol ID is a pointer to the protocol's security callback
 *     function. The user of the protocol ID should not use the protocol ID
 *     as a callback function since in the future it may be something else.
 *     The callback is a unique way to identify a protocol layer.   
 */
typedef BtCallBack BtProtocolId;

/* End of BtProtocolId */


/*---------------------------------------------------------------------------
 * BtClassOfDevice type
 *
 *     Bit pattern representing the class of device along with the 
 *     supported services. There can be more than one supported service.
 *     Service classes can be ORed together. The Device Class is composed
 *     of a major device class plus a minor device class. ORing together
 *     each service class plus one major device class plus one minor device
 *     class creates the class of device value. The minor device class is
 *     interpreted in the context of the major device class.
 */

typedef U32 BtClassOfDevice;

/* Group: Major Service Classes. Can be ORed together */
#define COD_LIMITED_DISCOVERABLE_MODE 0x00002000
#define COD_POSITIONING               0x00010000
#define COD_NETWORKING                0x00020000
#define COD_RENDERING                 0x00040000
#define COD_CAPTURING                 0x00080000
#define COD_OBJECT_TRANSFER           0x00100000
#define COD_AUDIO                     0x00200000
#define COD_TELEPHONY                 0x00400000
#define COD_INFORMATION               0x00800000

/* Group: Major Device Classes (Select one) */
#define COD_MAJOR_MISCELLANEOUS       0x00000000
#define COD_MAJOR_COMPUTER            0x00000100
#define COD_MAJOR_PHONE               0x00000200
#define COD_MAJOR_LAN_ACCESS_POINT    0x00000300
#define COD_MAJOR_AUDIO               0x00000400
#define COD_MAJOR_PERIPHERAL          0x00000500
#define COD_MAJOR_IMAGING             0x00000600
#define COD_MAJOR_UNCLASSIFIED        0x00001F00

/* Group: Minor Device Class - Computer Major class */
#define COD_MINOR_COMP_UNCLASSIFIED   0x00000000
#define COD_MINOR_COMP_DESKTOP        0x00000004
#define COD_MINOR_COMP_SERVER         0x00000008
#define COD_MINOR_COMP_LAPTOP         0x0000000C
#define COD_MINOR_COMP_HANDHELD       0x00000010
#define COD_MINOR_COMP_PALM           0x00000014
#define COD_MINOR_COMP_WEARABLE       0x00000018

/* Group: Minor Device Class - Phone Major class */
#define COD_MINOR_PHONE_UNCLASSIFIED  0x00000000
#define COD_MINOR_PHONE_CELLULAR      0x00000004
#define COD_MINOR_PHONE_CORDLESS      0x00000008
#define COD_MINOR_PHONE_SMART         0x0000000C
#define COD_MINOR_PHONE_MODEM         0x00000010
#define COD_MINOR_PHONE_ISDN          0x00000014

/* Group: Minor Device Class - LAN Access Point Major class */
#define COD_MINOR_LAN_0               0x00000000     /* fully available */
#define COD_MINOR_LAN_17              0x00000020     /* 1-17% utilized */
#define COD_MINOR_LAN_33              0x00000040     /* 17-33% utilized */
#define COD_MINOR_LAN_50              0x00000060     /* 33-50% utilized */
#define COD_MINOR_LAN_67              0x00000080     /* 50-67% utilized */
#define COD_MINOR_LAN_83              0x000000A0     /* 67-83% utilized */
#define COD_MINOR_LAN_99              0x000000C0     /* 83-99% utilized */
#define COD_MINOR_LAN_NO_SERVICE      0x000000E0     /* 100% utilized */

/* Group: Minor Device Class - Audio Major class */
#define COD_MINOR_AUDIO_UNCLASSIFIED  0x00000000
#define COD_MINOR_AUDIO_HEADSET       0x00000004
#define COD_MINOR_AUDIO_HANDSFREE     0x00000008
#define COD_MINOR_AUDIO_MICROPHONE    0x00000010
#define COD_MINOR_AUDIO_LOUDSPEAKER   0x00000014
#define COD_MINOR_AUDIO_HEADPHONES    0x00000018
#define COD_MINOR_AUDIO_PORTABLEAUDIO 0x0000001C
#define COD_MINOR_AUDIO_CARAUDIO      0x00000020
#define COD_MINOR_AUDIO_SETTOPBOX     0x00000024
#define COD_MINOR_AUDIO_HIFIAUDIO     0x00000028
#define COD_MINOR_AUDIO_VCR           0x0000002C
#define COD_MINOR_AUDIO_VIDEOCAMERA   0x00000030
#define COD_MINOR_AUDIO_CAMCORDER     0x00000034
#define COD_MINOR_AUDIO_VIDEOMONITOR  0x00000038
#define COD_MINOR_AUDIO_VIDEOSPEAKER  0x0000003C
#define COD_MINOR_AUDIO_CONFERENCING  0x00000040
#define COD_MINOR_AUDIO_GAMING        0x00000048

/* Group: Minor Device Class - Peripheral Major class */
#define COD_MINOR_PERIPH_KEYBOARD     0x00000040
#define COD_MINOR_PERIPH_POINTING     0x00000080
#define COD_MINOR_PERIPH_COMBOKEY     0x000000C0
/* Group: Minor Device Class - ORed with Peripheral Minor Device class */
#define COD_MINOR_PERIPH_UNCLASSIFIED 0x00000000
#define COD_MINOR_PERIPH_JOYSTICK     0x00000004
#define COD_MINOR_PERIPH_GAMEPAD      0x00000008
#define COD_MINOR_PERIPH_REMOTECTRL   0x0000000C
#define COD_MINOR_PERIPH_SENSING      0x00000010
#define COD_MINOR_PERIPH_DIGITIZER    0x00000014
#define COD_MINOR_PERIPH_CARD_RDR     0x00000018

/* Group: Minor Device Class - Imaging Major class */
#define COD_MINOR_IMAGE_UNCLASSIFIED  0x00000000
#define COD_MINOR_IMAGE_DISPLAY       0x00000010
#define COD_MINOR_IMAGE_CAMERA        0x00000020
#define COD_MINOR_IMAGE_SCANNER       0x00000040
#define COD_MINOR_IMAGE_PRINTER       0x00000080

/* Group: Masks used to isolate the class of device components */
#define COD_SERVICE_MASK              0x00ffC000     /* Less LIAC bit */
#define COD_MAJOR_MASK                0x00001F00
#define COD_MINOR_MASK                0x000000FC
#define COD_LIMITED_DISC_MASK         0x00002000     /* LIAC bit */

/* End of BtClassOfDevice */

/*---------------------------------------------------------------------------
 * BtLinkKeyType type
 */

typedef U8 BtLinkKeyType;

#define COMBINATION_KEY 0x00
#define LOCAL_UNIT_KEY  0x01
#define REMOTE_UNIT_KEY 0x02
#define KEY_NOT_DEFINED					0x03

/* End of BtLinkKeyType */

/*---------------------------------------------------------------------------
 * BtRemDevState type
 *
 *     Indicates the current state of a Remote Device.
 */
typedef U8 BtRemDevState;

#define BDS_DISCONNECTED  0x00
#define BDS_OUT_CON       0x01   /* Starting an out going connection */
#define BDS_IN_CON        0x02   /* In process of incoming connection */
#define BDS_CONNECTED     0x03   /* Connected */
#define BDS_OUT_DISC      0x04   /* Starting an out going disconnect */
#define BDS_OUT_DISC2     0x05   /* Disconnect status received */
#define BDS_OUT_CON2      0x06   /* In SCO, used when connection request has
                                    been sent */
#if TI_CHANGES == XA_ENABLED 
/*---------------------------------------------------------------------------
 * SdpServicesMask type
 *
 */
typedef U32 SdpServicesMask;

#define SDP_SERVICE_NONE		(0x00000000)
#define SDP_SERVICE_SPP			(0x00000001)	/* Serial Port Profile */
#define SDP_SERVICE_LAP			(0x00000002)	/* LAN Access Profile */
#define SDP_SERVICE_DUN		(0x00000004)	/* Dial-up Networking Profile */
#define SDP_SERVICE_SYNC		(0x00000008)	/* Synchronization Profile */
#define SDP_SERVICE_OPP		(0x00000010)	/* Object Push Profile */
#define SDP_SERVICE_FTP			(0x00000020)	/* File Transfer Profile */
#define SDP_SERVICE_HSP		(0x00000040)	/* Headset Profile */
#define SDP_SERVICE_CTP			(0x00000080)	/* Cordless Telephony Profile */
#define SDP_SERVICE_A2DP		(0x00000100)	/* Advanced Audio Distribution Profile */
#define SDP_SERVICE_AVRCP		(0x00000200)	/* Audio/Video Remote Control Profile */
#define SDP_SERVICE_ICP			(0x00000400)	/* Intercom Profile */
#define SDP_SERVICE_FAX		(0x00000800)	/* Fax Profile */
#define SDP_SERVICE_PANU		(0x00001000)	/* Personal Area Networking Profile, user */
#define SDP_SERVICE_NAP		(0x00002000)	/* Personal Area Networking Profile, network access point */
#define SDP_SERVICE_GN			(0x00004000)	/* Personal Area Networking Profile, group ad-hoc networking */
#define SDP_SERVICE_BPP			(0x00008000)	/* Basic Printing Profile */
#define SDP_SERVICE_BIP			(0x00010000)	/* Basic Imaging Profile */
#define SDP_SERVICE_HFP		(0x00020000)	/* Handsfree Profile */
#define SDP_SERVICE_SAP		(0x00040000)	/* SIM Access Profile */
#define SDP_SERVICE_PBAP		(0x00080000)	/* Phonebook Access Profile */
#define SDP_SERVICE_HCRP		(0x00100000)	/* Hardcopy Cable Replacement Profile */
#define SDP_SERVICE_HID			(0x00200000)	/* Human Interface Device Profile */

#define SDP_SERVICE_MIN_MASK		SDP_SERVICE_SPP /* Smallest Mask Value*/
#define SDP_SERVICE_MAX_MASK		SDP_SERVICE_HID /* Greatest Mask Value*/
#endif


/* End of BtRemDevState */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * BtPageScanInfo structure
 */
typedef struct _BtPageScanInfo
{
    U8   psRepMode;
    U8   psMode;
    U16  clockOffset;
} BtPageScanInfo;


/*---------------------------------------------------------------------------
 * BtSniffInfo structure
 *
 *     Identifies the sniff requirements during a ME_StartSniff request.
 */

typedef struct _BtSniffInfo
{
    /* Maximum acceptable interval between each consecutive sniff period.
     * May be any even number between 0x0002 and 0xFFFE, but the mandatory
     * sniff interval range for controllers is between 0x0006 and 0x0540.
     * The value is expressed in 0.625 ms increments (0x0006 = 3.75 ms).
     *
     * The actual interval selected by the radio will be returned in
     * a BTEVENT_MODE_CHANGE event.
     */
    U16 maxInterval;
    
    /* Minimum acceptable interval between each consecutive sniff period.
     * Must an even number between 0x0002 and 0xFFFE and be less than
     * "maxInterval". Like maxInterval this value is expressed in
     * 0.625 ms increments.
     */
		
    U16 minInterval;

    /* The number of master-to-slave transmission slots during which
     * a device should listen for traffic (sniff attempt).
     * Expressed in 0.625 ms increments. May be between 0x0001 and 0x7FFF.
     */
    U16 attempt;

    /* The amount of time before a sniff timeout occurs. Expressed in
     * 1.25 ms increments. May be between 0x0000 and 0x7FFF, but the mandatory
     * range for controllers is 0x0000 to 0x0028.
     */
    U16 timeout;
    
} BtSniffInfo;


/*---------------------------------------------------------------------------
 * BtAccessModeInfo structure
 */
typedef struct _BtAccessModeInfo
{
    U16 inqInterval;    /* Inquiry scan interval */
    U16 inqWindow;      /* Inquiry scan Window */
    U16 pageInterval;   /* Page scan interval */
    U16 pageWindow;     /* Page scan window */
} BtAccessModeInfo;


/*---------------------------------------------------------------------------
 * BtInquiryResult structure
 *
 *     Represents information about a single device found during an
 *     Inquiry process.
 */
typedef struct _BtInquiryResult
{
    BD_ADDR         bdAddr;           /* Device Address */
    BtPageScanInfo  psi;              /* Page scan info used for connecting */
    U8              psPeriodMode;
    BtClassOfDevice classOfDevice;

    /* RSSI in dBm (-127 to +20). Only valid when controller reports RSSI with
     * in inquiry results. Otherwise it will be set to BT_INVALID_RSSI.
     */
    S8              rssi;             
} BtInquiryResult;


/*---------------------------------------------------------------------------
 * BtChannelClass structure
 *
 *     Indicates channel classification for AFH with 79 1-bit fields in
 *     a 10-byte array. Each channel may be marked bad or unknown. The nth
 *     field (0 - 78) indicates the value for channel n. Bit 79 is unused.
 */
typedef struct _BtChannelClass
{
    U8  map[10];
} BtChannelClass;

/*---------------------------------------------------------------------------
 * BtHandler structure
 */
typedef struct _BtHandler
{
    ListEntry   node;       /* For internal stack use only. */
    BtCallBack  callback;   /* Pointer to callback function */
    BtEventMask eMask;      /* For internal stack use only. */

} BtHandler;


/* Internal types required for BtRemoteDevice structure */
typedef U8 BtAuthState;         
typedef U8 BtEncryptState;      
typedef U8 BtAuthorizeState;    
typedef U8 BtSecAccessState;
typedef U8 BtLinkRxState;
typedef U8 BtOpType;

typedef struct _BtOperation {                
    ListEntry   node;
    BtOpType    opType;
} BtOperation;


/*---------------------------------------------------------------------------
 * BtRemoteDevice structure
 *
 *     Represents a remote device. This structure is used to manage the ACL
 *     and SCO links to a remote device.
 */
typedef struct _BtRemoteDevice
{
    BtOperation      op;      /* Used internally by the stack */
    BtOperation      policyOp;/* Used internally by the stack */
    BtOperation      cancelOp;/* Used internally by the stack */
    BtOperation      roleOp;  /* Used internally by the stack */
    BtOperation      packetOp; /* Used internally by the stack */

    /* Group: Access functions are provided for accessing these values. They
     * should not be accessed directly by an application. 
     */    
    BD_ADDR          bdAddr; /* 48-bit address of the remote device */
    BtConnectionRole role;   /* The local device's role in this connection */
    BtLinkMode       mode;   /* Current mode of the device */
    BtClassOfDevice  cod;    /* COD of the device */
    void            *secRec; /* Current Security Record being processed */
		U8							pinLen;	/* length of the pin code used in current pairing transaction.*/

    /* Group: ME uses the following fields internally. */
    ListEntry        handlers;      /* List of bound handlers */
    ListEntry        scoList;       /* Head of list of SCO connections */
    ListEntry        secTokens;     /* List of security tokens */
    BtHandler       *scoHandler;    /* Handler for SCO event (1 per remDev) */
    BtHandler       *authHandler;   /* Handler for authentication */
    BtHandler       *encryptHandler;/* Handler for encryption */
    U16              hciHandle;     /* Connection handle for HCI */
    BtRemDevState    state;         /* State of the ACL link */
    U8               restricted;    /* TRUE indicates restricted link */
    U8               discFlag;      /* TRUE indicates disconnect request */
    U8               discReason;    /* Non-zero = disconnect reason */
    BtAclPacketType  aclPacketTypes; /* Current ACL packet types permitted */

    /* Group: The following are used for security */
    BtSecAccessState secAccessState;
    BtAuthState      authState;
    BtAuthorizeState authorizeState;
    BtEncryptState   encryptState;
    U8               pairingType;   /* Uses the BtPairingType type */

#if (TI_CHANGES == XA_ENABLED && HCI_QOS == XA_ENABLED)	
	U8				maxNumOfAllocatedBuffers;	/* max number of HCI Tx buffer can be used for this connection */
	S16				numOfFreeBuffers;			/* the number of HCI Tx buffers that are not in use */
	BOOL			isConGuaranteed;			/* Will be true if the connection type is guaranteed */
#endif /* TI_CHANGES == XA_ENABLED && HCI_QOS == XA_ENABLED */

    /* Group: The following are parameters to me operations */
    union {
        /* Create connection operation */
        struct {
            BtPageScanInfo   psi;
            U8               allowRoleChange;
        } con;

        /* Park & Hold mode commands */
        struct {
            U16          max;
            U16          min;
        } mode;

        BtSniffInfo      sniff;               /* Sniff command */
        BtConnectionRole acceptRole;          /* Connection accept */
        
        U8               rejectReason;        /* Connection reject */
        BtLinkPolicy     policy;              /* Write link policy */
    } parms;

    /* Group: L2CAP uses the following fields internally. */
    BOOL             okToSend;
    ListEntry        txQueue;
    U8               refCount;
    BtLinkRxState    rxState;
    U16              rxBufferLen;
    U8              *rxBuffer;
#if L2CAP_FLOW_CONTROL == XA_ENABLED
    ListEntry        txPacketList;
    BtPacket         txPackets[2];
    U8               linkModes;        /* L2capLinkMode defined in l2cap.h */
#endif

    /* Group: The SDP client uses the following fields internally. */
    
    U16              sdpClientChannel; /* L2CAP channel for SDP client */
    ListEntry        queryList;        /* List of queries */
    BtPacket         sdpClientPacket;  /* Packet for sending queries */
    U16              transId;          /* Current transaction ID */
    U8               sdpClientState;   /* State of the client */
		U8               sdpKeepAlive;     /* Keep-alive count */ 

    struct _SdpQueryToken *curToken; /* The token currently being processed. */

    /* Group: The SDP server uses the following field internally. */
    struct _SdpServerInfo *sdpServInfo;

} BtRemoteDevice;


/*---------------------------------------------------------------------------
 * BtRestrictedInfo structure
 *
 *     Used to create a restricted link to a remote device. A restricted link
 *     has a specific Quality of service, link policy, and hold policy. 
 */
typedef struct _BtRestrictedInfo
{
    BtAclPacketType  aclPacketType;
    BtQosServiceType serviceType;

    /* Range of flush time out is 0x0000 - 0x0EFF. 0xFFFF is used
     * to indicate infinite - no automatic flush. Flush time out is
     * the amount of time to wait until a transmit frame is flushed.
     * Times are specified in 0.625 us units.
     */
    U16              minRemoteFlushTo;
    U16              minLocalFlushTo;

    U32              tokenRate;      /* Token rate in bytes per second */
    U32              peakBandwidth;  /* Peak bandwidth in bytes per second */
    U32              latency;        /* Latency in microseconds */
    U32              delayVariation; /* Delay variation in microseconds */
    BtLinkPolicy     linkPolicy;     /* Link policy */

} BtRestrictedInfo;


/*---------------------------------------------------------------------------
 * BtDeviceRecord structure
 *
 *     Represents a Bluetooth Device Database record as seen by the
 *     Security manager. 
 */
#if TI_CHANGES == XA_ENABLED
typedef struct _psiToSave
{
	    U8   psRepMode;
	    U8   psMode;
	    /* No need to save the clock offset value, since it is not a constant value */
} psiToSave;	/* updated after inquiry.*/
#endif

typedef struct _BtDeviceRecord
{
    BD_ADDR         bdAddr;
    BOOL            trusted;
    U8              linkKey[16];
    BtLinkKeyType   keyType; 

#if TI_CHANGES == XA_ENABLED
	U8								pinLen;	/* length of the pin used for the last pairing.*/
	psiToSave psi;	/* updated after inquiry.*/
#endif
	
} BtDeviceRecord;


/*---------------------------------------------------------------------------
 * BtDeviceLinkKey structure
 *
 *     Represents a Bluetooth Device link key returned from an HCI
 *     ReadStoredLinkKey command.
 */
typedef struct _BtDeviceLinkKey
{
    BD_ADDR     bdAddr;           /* Device Address */
    U8          linkKey[16];
} BtDeviceLinkKey;


/*---------------------------------------------------------------------------
 * MeRemoteNameRequest structure
 *
 *     Used in an MeCommandToken when handling a "Remote Name Request"
 *     operation.
 */
typedef struct _MeRemoteNameRequest
{
    /* Common for both input and output */
    BD_ADDR   bdAddr;
    
    /* Input/Output parameters for the operation. Either the "in"
     * or "out" parameters are valid, depending on when this structure
     * is accessed.
     */      
    union {
        
        /* Fields to fill before starting the operation. */
        struct {
            BtPageScanInfo   psi;  /* Page scan information */
        } in;
        
        /* Fields to read when the operation is complete. */
        struct {
            
            /* Length of string stored in "name". */
            U8  len;
            
            /* Remote device name in UTF-8 format */
            U8* name;
        } out;
        
    } io;
    
} MeRemoteNameRequest;


/*---------------------------------------------------------------------------
 * MeGeneralHciCommand structure
 *
 *     Used in an MeCommandToken when handling general and user defined
 *     HCI commands. 
 */
typedef struct _MeGeneralHciCommand
{
    /* Fields to be filled before starting the operation */
    struct {

        /* The hciCommand is composed of the OGF and the OCF. The OGF
         * is the upper 6 bits while the OCF is the remaining 12 bits.
         */
        U16      hciCommand;   /* HCI command (see hci.h for commands) */
        U8       parmLen;      /* Length of the command parameters */
        U8       *parms;       /* Pointer to command parameters */
        U8       event;        /* Main event which signals end of operation */
    } in;

    /* Fields to read when the operation is complete */
    struct {
        /* The status must be checked to see if the remaining out parameters
         * valid. If the status is BT_STATUS_SUCCESS then the rest of the
         * out parameters is valid.
         */
        BtStatus status;
        
        U8       event;        /* Event ending operation (see hci.h for event) */
        U8       parmLen;      /* Length of event parameters */
        U8       *parms;       /* Pointer to event parameters */
    } out;
} MeGeneralHciCommand;


typedef U16 BtScoAudioSettings; /* This must match definition in mesco.h */
/*---------------------------------------------------------------------------
 * MeCommandToken structure
 *
 *     A token that represents a special HCI command, including
 *     user-defined HCI commands.
 */
typedef struct _MeCommandToken
{
    BtOperation     op;      /* For internal use by the stack. */
    BtOperation     cancelOp;/* For internal use by the stack. */
    BtRemoteDevice *remDev;  /* For internal use by the stack. */

    /* The caller must set the callback function. It is called
     * when the command is complete.
     */
    BtCallBack      callback; 

    /* One of the fields in this union is used to store information
     * about the operation.
     */
    union {

        /* Information about a "Remote Name Request" operation */
        MeRemoteNameRequest name;

        /* Information for the SCO Audio settings command */
        BtScoAudioSettings vsettings;

        /* Information used in general and user defined operations */
        MeGeneralHciCommand general;

    } p;

    /* Group: The following field is for internal stack use only */
    BtEventType     eType;
    U8              flags;

} MeCommandToken;


/*---------------------------------------------------------------------------
 * BtEvent structure
 *
 *     Used to pass information from the stack to the application. Not all
 *     fields are valid at any given time. The type of event determines which
 *     fields are valid.
 */
struct _BtEvent
{
    BtEventType   eType;       /* Event causing callback */
    BtErrorCode   errCode;     /* Error code */
    BtHandler    *handler;     /* Pointer to handler */

    /* Parameters */
    union {
        BtInquiryResult    inqResult;  /* Inquiry result */
        BtDeviceLinkKey    bdLinkKey;  /* Returned Link Key */
        
        BtAccessibleMode   aMode;      /* New access mode */
        BtRemoteDevice    *remDev;     /* Pointer to remote device */
        BD_ADDR            bdAddr;     /* Device Address */
        MeCommandToken    *meToken;    /* Me command token */

        struct _BtSecurityToken  *secToken;   /* Security Token */
        struct _SdpQueryToken    *token;

        /* Result for BTEVENT_ENCRYPTION_CHANGE event. */
        struct {
            BtRemoteDevice *remDev;
            U8              mode; /* New encryption mode (uses the
                                   * BtEncryptMode type) */
        } encrypt;

        struct {
            BtRemoteDevice *remDev;
            
             /* If disconnection was successful, contains BEC_NO_ERROR.
              * errCode will contain the disconnect reason.
              *
              * Unsuccessful disconnections will contain an error code
              * as generated by the radio. In this case, errCode can be
              * ignored.
              */
            BtErrorCode status;
        } disconnect;
        
        /* Result for the BTEVENT_SCO_DATA_CNF event */
        struct {
            BtRemoteDevice *remDev;     /* Pointer to remote device */
            BtScoConnect   *scoCon;     /* SCO connection */
            BtPacket       *scoPacket;  /* SCO Packet Handled */
        } scoPacketHandled;

        /* Result for the BTEVENT_SCO_CONNECT_CNF && BTEVENT_SCO_CONNECT_IND 
           events.
         */
        struct {
            BtRemoteDevice *remDev;     /* Pointer to remote device */
            U16             scoHandle;  /* SCO Connection handle for HCI */
            BtScoConnect   *scoCon;     /* SCO connection */
            BtLinkType      scoLinkType;/* SCO link type */
            BtScoTxParms   *scoTxParms; /* Pointer to eSCO TX parameters */
            BtScoRxParms   *scoRxParms; /* Pointer to eSCO RX parameters */
        } scoConnect;

        /* Result for the BTEVENT_SCO_DATA_IND event */
        struct {
            BtRemoteDevice *remDev;     /* Pointer to remote device */
            U16             scoHandle;  /* SCO Connection handle for HCI */
            BtScoConnect   *scoCon;     /* SCO connection. */
            U8              len;        /* SCO data len */
            U8             *ptr;        /* SCO data ptr */
        } scoDataInd;

        /* Result for the BTEVENT_SECURITY_CHANGE and
         * BTEVENT_SECURITY3_COMPLETE events
         */
        struct {
            U8    mode;    /* New security mode (uses the BtSecurityMode
                            * type). */
            BOOL  encrypt; /* Indicate if encryption set or not */
        } secMode;

        /* Results for the BTEVENT_MODE_CHANGE event */
        struct {
            BtRemoteDevice *remDev;
            BtLinkMode      curMode;
            U16             interval;
        } modeChange;

        /* Results for BTEVENT_ROLE_CHANGE */
        struct {
            BtRemoteDevice    *remDev;
            BtConnectionRole   newRole;    /* New role */
        } roleChange;

        /* Informational BTEVENT_HCI_COMMAND_SENT event. */
        struct {
            /* "HciCommandType" values found in sys/hci.h.*/
            U16                    type;

            /* Length of optional parameters */
            U8                     length;

            /* Optional parameters to HCI command. Refer to Bluetooth HCI
             * specification for command parameter descriptions.
             */
            const U8*              parms;
        } hciCmd;

        /* Results for BTEVENT_DEVICE_SELECTED event. When received by
         * the Device Selection monitor, it represents the selection query.
         */
        struct _BtSelectDeviceToken*     select;

        /* BTEVENT_DEVICE_ADDED and BTEVENT_DEVICE_DELETED events. */
        struct _BtDeviceContext*   device;

#if TI_CHANGES == XA_ENABLED 
		
		/* Results  for BTEVENT_DISCOVER_SERVICES_RESULT event */
		struct
		{
			BD_ADDR         bdAddr;
			SdpServicesMask discoveredServicesMask;
		} discoveredServices;
		
		
		/* Results  for BTEVENT_AUTHORIZATION_REQ event */
		struct  
		{
			BtRemoteDevice *remDev;
			SdpServicesMask service;	/* Associated UUID */
		} security;
		
		struct
		{
			BD_ADDR bdAddr;
			U8*	resAttrDataElement;
		}	serviceSearchAttributes;
		
		struct
		{
			BD_ADDR bdAddr;
			U8*	recordHandlesList;
			U16 recordHandlesCount;
		}	serviceSearch;

		struct
		{
			BD_ADDR bdAddr;
			U8*	resAttrDataElement;
		}	serviceAttributes;
		
#endif
    } p;    
};


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

#if (TI_CHANGES == XA_DISABLED)

/*---------------------------------------------------------------------------
 * ME_RegisterGlobalHandler()
 *
 *     Register a callback handler with the Management Entity to
 *     receive global events. The protocol stack sends a number
 *     of global events to all registered handlers (who have the
 *     proper event mask set). Clients must register a handler 
 *     in order to receive events. The list of global events for 
 *     which the callback will be called are listed in the 
 *     Callback section below. The event mask for the handler is 
 *     initialized to BEM_NO_EVENTS.
 *
 * Parameters:
 *     handler - pointer to a BtHandler structure. The callback
 *         field of the handler must point to a valid callback function.
 *         The handler must be initialized using ME_InitHandler.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation completed successfully.
 *
 *     BT_STATUS_FAILED - the operation failed because the handle is
 *         already registered, the handle is not initialized properly.
 */
BtStatus ME_RegisterGlobalHandler(BtHandler *handler);

/*---------------------------------------------------------------------------
 * ME_UnregisterGlobalHandler()
 *
 *     Unregister a global handler. Upon successful completion of this 
 *     function the handler is initialized and can be registered again 
 *     without the need for calling ME_InitHandler.
 *
 * Parameters:
 *     handler - pointer to handler to unregister
 *
 * Returns
 *     BT_STATUS_SUCCESS - operation successful
 *
 *     BT_STATUS_FAILED - operations failed because handler
 *         was not registered or 0 (error check only).
 */
BtStatus ME_UnregisterGlobalHandler(BtHandler *handler);

/*---------------------------------------------------------------------------
 * ME_SetEventMask()
 *
 *     Set the event mask for the global handler. The event mask 
 *     is for global events.
 *
 * Parameters:
 *     handler - pointer to handler.
 *
 *     mask - event mask.
 *
 * Returns
 *     BT_STATUS_SUCCESS - operation successful
 *
 *     BT_STATUS_FAILED - operations failed because handler
 *         was not registered or 0 (error check only).
 */
#if XA_ERROR_CHECK == XA_ENABLED
BtStatus ME_SetEventMask(BtHandler *handler, BtEventMask mask);
#else /* XA_ERROR_CHECK */
#define ME_SetEventMask(h,m) ((h)->eMask = (m),BT_STATUS_SUCCESS)
#endif /* XA_ERROR_CHECK */

/*---------------------------------------------------------------------------
 * ME_GetEventMask()
 *
 *     Get the event mask for the global handler. The event mask
 *     is for global events. This function does not check to
 *     see if the handler is valid.
 *
 * Parameters:
 *     handler - pointer to handler.
 *
 * Returns:
 *     The event mask 
 */
BtEventMask ME_GetEventMask(BtHandler *handler);
#define ME_GetEventMask(h) ((h)->eMask)

#endif /* TI_CHANGES == XA_DISABLED */

/*---------------------------------------------------------------------------
 * ME_InitHandler()
 *
 *     Initialize a handler. All global handlers must be initialized
 *     before they can be passed to ME functions. This function
 *     can be used or filling the contents of the handler with 0
 *     will also suffice. A handler should only need to be initialized
 *     once.
 *
 * Parameters:
 *     handler - pointer to a BtHandler structure. 
 */
void ME_InitHandler(BtHandler *handler);
#define ME_InitHandler(h) InitializeListEntry(&((h)->node))

/*---------------------------------------------------------------------------
 * ME_Inquiry()
 *
 *     Start a Bluetooth Inquiry procedure. If BT_STATUS_PENDING is
 *     returned then the results of the operation will be returned
 *     to all clients with registered global handlers. The following
 *     events will be sent to all registered handlers:
 *
 *     BTEVENT_INQUIRY_RESULT - indicates that a device was found. The
 *     "p.inqResult" field of the BtEvent contains the result.
 *
 *     BTEVENT_INQUIRY_COMPLETE - indicates that the inquiry process is
 *     complete. The "errCode" field of the BtEvent indicates whether 
 *     the operation completed without error.
 *
 *     BTEVENT_INQUIRY_CANCELED - this will be returned if the inquiry
 *     operation is canceled. BTEVENT_INQUIRY_COMPLETE will not be 
 *     returned.
 *
 *     BTEVENT_HCI_FATAL_ERROR - indicates fatal radio or HCI
 *     transport error. Assume all pending operations have failed.
 *     
 * Parameters:
 *     lap - LAP used for the Inquiry (General or Limited)
 *
 *     length - Maximum amount of time before the Inquiry is
 *         halted. Range is 0x01 to 0x30. Time is length * 1.28 sec.
 *         The Generic Access profile specifies using the value
 *         BT_INQ_TIME_GAP100.
 *
 *     maxResp - The maximum number of responses. Specify zero to receive
 *         an unlimited number of responses.
 *
 * Returns:
 *     BT_STATUS_PENDING - The Inquiry process is started results
 *         will be sent via the handler. A BTEVENT_HCI_FATAL_ERROR event
 *         indicates a fatal radio or HCI transport error and that all
 *         pending operations have failed.
 *
 *     BT_STATUS_IN_PROGRESS - An inquiry process is already in 
 *         progress. Only one Inquiry can be in progress at a time.  
 *         Keep track of the general events to get the results from 
 *         the current Inquiry or to see when it ends. If it has just
 *         ended then a cancel is also in progress so wait for
 *         the cancel to complete to start another inquiry.
 *
 *     BT_STATUS_FAILED - The operation failed. 
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_Inquiry(BtIac lap, U8 Length, U8 maxResp);

/*---------------------------------------------------------------------------
 * ME_CancelInquiry()
 *
 *     Cancel the current Inquiry process. When the inquiry
 *     operation is canceled all registered handlers will
 *     receive the BTEVENT_INQUIRY_CANCELED event. The "errCode"
 *     field of the BtEvent indicates the status of the
 *     operation.
 *
 * Returns:
 *     BT_STATUS_PENDING - The cancel operation was started
 *         successfully. The results will be sent to all clients
 *         with registered handlers. A BTEVENT_HCI_FATAL_ERROR event
 *         indicates a fatal radio or HCI transport error and that all
 *         pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - The inquiry process was canceled
 *         immediately. It actually never was started. 
 *         BTEVENT_INQUIRY_CANCELED event will be sent to all handlers 
 *         before this function returns.
 *
 *     BT_STATUS_IN_PROGRESS - A cancel Inquiry is already in
 *         progress. Only one cancel can be in progress at a time.
 *         Keep track of the general events to see when the cancel
 *         is complete. 
 * 
 *     BT_STATUS_FAILED - The operation failed because an inquiry
 *         operation was not in progress.
 */
BtStatus ME_CancelInquiry(void);

/*---------------------------------------------------------------------------
 * ME_GetRemoteDeviceName()
 *
 *     Get the name of the remote device. If an ACL connection
 *     does not exist then a temporary one will be created to
 *     get the name. If this function returns BT_STATUS_PENDING
 *     then the result will be returned via the callback in the
 *     token with the BTEVENT_NAME_RESULT event. The "errCode" field 
 *     indicates the status of the operation. "p.meToken" points
 *     to the token passed into the function. The output field of
 *     token contains the results if the status indicates success.
 *
 *     If a request to the same remote device is already in progress
 *     only one request will be made. This token will receive the
 *     results of the request in progress.
 *
 * Parameters:
 *     token - pointer to token containing input parameters and
 *         storage for output parameters. The token must be initialized 
 *         before calling this function as follows:
 *
 *         "token.callback" must be set to the callback function.
 *
 *         "token.p.name.bdAddr" must contain the 48-bit address of
 *         the remote device.
 *
 *         "token.p.name.io.in.psi" must contain the page scan information.
 *         If the page scan information is not known then all fields should
 *         be 0.
 *
 * Returns:
 *     BT_STATUS_PENDING - Operation is started and results will
 *         be sent to the callback.
 *
 *     BT_STATUS_FAILED - operation failed because the token was
 *         not properly initialized or already active (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_GetRemoteDeviceName(MeCommandToken *token);

/*---------------------------------------------------------------------------
 * ME_CancelGetRemoteDeviceName()
 *
 *     Request cancellation of a pending ME_GetRemoteDeviceName request.
 *     If this function returns BT_STATUS_SUCCESS, then "token" is
 *     free and no further events will be returned. If this function
 *     returns BT_STATUS_PENDING, then the result will be returned
 *     via the callback in "p.meToken" with the BTEVENT_NAME_RESULT event.
 *     The "errCode" field will indicate a radio-specific error code,
 *     or BEC_NO_ERROR if the remote name was received before the cancel
 *     completed.
 *
 * Parameters:
 *     token - pointer to token passed in to ME_GetRemoteDeviceName.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the operation has completed successfully.
 *         The "token" is free and no further events will be returned.
 *
 *     BT_STATUS_PENDING - Operation is started and results will
 *         be sent to the callback.
 *
 *     BT_STATUS_FAILED - operation failed because the get remote
 *         device name request is no longer in progress.
 */
BtStatus ME_CancelGetRemoteDeviceName(MeCommandToken *token);

/*---------------------------------------------------------------------------
 * ME_SetLocalDeviceName()
 *
 *     Set the local device name. This is the name that other devices will
 *     get when performing a GetRemoteDeviceName to this device. The name
 *     is a UTF-8 encoded string. The name can be up to 248 bytes in length.
 *     If the name is less than 248 bytes it must be null-terminated.
 *
 * Parameters:
 *     name - pointer to a UTF-8 encoded string. The name will remain in use
 *         until the stack is deinitialized, or until this function is called
 *         with a different name. The name string should not be modified and
 *         the pointer must remain valid while it is in use.
 *
 *     len - indicates the number of bytes in the name including the null
 *         termination byte if the name is less than 248 bytes. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation is successful.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because the name or 
 *         length were not properly initialized (XA_ERROR_CHECK only).
 */
BtStatus ME_SetLocalDeviceName(const U8 *name, U8 len);

/*---------------------------------------------------------------------------
 * ME_ReadLocalBdAddr()
 *
 *     Get the 48-bit Bluetooth device address of the local device. This
 *     request cannot be issued until the radio has been initialized. Check
 *     ME_GetStackInitState for BTSS_INITIALIZED. If it's not initialized
 *     wait for the BTEVENT_HCI_INITIALIZED event on a global handler.
 *
 * Parameters:
 *     Addr - Pointer to a BD_ADDR structure to receive the address.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful.
 *
 *     BT_STATUS_FAILED - Operation failed because the radio is not
 *         initialized yet. Monitor the global events to be notified
 *         when the radio is initialized (BTEVENT_HCI_INITIALIZED).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_ReadLocalBdAddr(BD_ADDR *Addr);

/*---------------------------------------------------------------------------
 * ME_SetClassOfDevice()
 *
 *     Set the class of device. The class of device is sent out as part of
 *     the response to inquiry. The actual bit pattern that is sent out is
 *     a composite of the value set using this function and the service
 *     class information found in registered SDP records.
 *
 * Parameters:
 *     classOfDevice - A 32-bit integer where the lower 3 bytes represents
 *         the class of device. The most significant 8 bits are ignored. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation is successful.
 */
BtStatus ME_SetClassOfDevice(BtClassOfDevice classOfDevice);

/*---------------------------------------------------------------------------
 * ME_FindRemoteDevice()
 *
 *     Return a pointer to the BtRemoteDevice structure corresponding to
 *     the device with given BD_ADDR. 0 is returned if a match is not found.
 *
 *     When this function returns successfully, the BtRemoteDevice structure
 *     returned always refers to a connected or connecting device. Use
 *     caution, as it will also return successfully if the device is in the
 *     process of disconnecting, but has not yet completed. This typically
 *     occurs if called during the notification of a link failure.
 *
 * Parameters:
 *     bdAddr - pointer to 48-bit address of the device
 *
 * Returns:
 *     Pointer to BtRemoteDevice structure if found. 0 is returned if the
 *     device is not found.
 */
BtRemoteDevice *ME_FindRemoteDevice(BD_ADDR *bdAddr);

/* Prototype for actual ME_FindRemoteDevice implementation */
BtRemoteDevice *ME_FindRemoteDeviceP(U8* bdAddr);
#define ME_FindRemoteDevice(bd) (ME_FindRemoteDeviceP((bd)->addr))

/*---------------------------------------------------------------------------
 * ME_CreateLink()
 *
 *     Create an ACL link to the device specified by "bdAddr". If
 *     this function returns BT_STATUS_PENDING the results will be sent to
 *     the handler. When the operation is complete all registered global
 *     handlers will receive BTEVENT_LINK_CONNECT_CNF as will the handler
 *     passed to this function. The "errCode" field of the BtEvent will 
 *     indicate if the operation was successful or not. If an incoming
 *     connection is in progress to this device then the handler will be
 *     bound and when the incoming connection is complete the handler will
 *     receive the BTEVENT_LINK_CONNECT_CNF event. In this case all global
 *     registered handlers will receive BTEVENT_LINK_CONNECT_IND instead of
 *     BTEVENT_LINK_CONNECT_CNF.
 *
 *     If connections already exist to other devices, then ME will attempt
 *     to put all other connections on hold, then begin a connection
 *     to the specified device.
 *
 *     If a connection already exists to the device indicated by bdAddr,
 *     this function will register the handler with the BtRemoteDevice
 *     and return BT_STATUS_SUCCESS immediately. In this way, many local
 *     handlers can be registered to the same connection. Be aware that
 *     ME_DisconnectLink will not take effect until it is called for
 *     ALL registered handlers of the BtRemoteDevice. To determine whether
 *     a connection already exists without registering a handler,
 *     use ME_FindRemoteDevice instead.
 *
 *     A slave device does not need to use this API unless it needs
 *     to register a local handler or initiate a disconnection of the link
 *     (see ME_DisconnectLink).
 *          
 *     The type of connection created to the remote device uses
 *     the quality of service parameters necessary for the RFCOMM
 *     based profiles or profiles requiring reliable, non
 *     real-time critical links. For other types of connections, use
 *     ME_CreateRestrictedLink.
 *
 * Parameters:
 *     handler - pointer to an unregistered handler to receive 
 *         events. This handler cannot be the global handler though
 *         they both can point to the same callback. The handler 
 *         must be initialized using ME_InitHandler. The handler 
 *         is in use until the link is disconnected and should not
 *         be modified while in use.
 *
 *     bdAddr - pointer to address of remote device.
 *
 *     psi - pointer to page scan information obtained during 
 *         Inquiry. Initial links to a device should set this parameter
 *         to ensure a reliable connection. If the psi is not known, a
 *         default "psi" of 0 on an initial link will use clockOffset = 0,
 *         psMode = 0, and psRepMode = 1. If a client is binding to an
 *         existing remote device, then "psi" can be set to 0.
 *
 *     remDev - pointer to memory to receive remote device pointer.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation has started successfully
 *         result will be sent to handler.
 *
 *     BT_STATUS_SUCCESS - client is successfully bound to the remote device.
 *
 *     BT_STATUS_RESTRICTED - the operation failed because this remote
 *         device already has a restricted link. The handler is free.
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because this remote
 *         device is in the process of being disconnected. The handler
 *         is free.
 *
 *     BT_STATUS_NO_RESOURCES - the operation failed because 
 *         the maximum number of connections already exist. The
 *         handler is free.
 *
 *     BT_STATUS_FAILED - operation failed because the handler is 0, the 
 *         callback is 0 or the handler is not initialized (XA_ERROR_CHECK
 *         only). The handler is free.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_CreateLink(BtHandler *handler, BD_ADDR *bdAddr,
                       BtPageScanInfo *psi, BtRemoteDevice **remDev);

/*---------------------------------------------------------------------------
 * ME_CreateRestrictedLink()
 *
 *     (Not implemented.)
 *
 *     Create a restricted Link to a remote device. A restricted
 *     link is one where only the client specified by the handler
 *     is allowed to bind to the connection. The client also has
 *     the ability to specify the quality of service parameters
 *     so a time bounded link with specific latency and bandwidth
 *     can be created.
 *
 *     If this function returns BT_STATUS_PENDING the results will be sent
 *     to the handler. When the operation is complete all registered global
 *     handlers will receive BTEVENT_LINK_CONNECT_CNF as will the handler
 *     passed to this function. The "errCode" field of the BtEvent will 
 *     indicate if the operation was successful or not. If an incoming
 *     connection is in progress to this device then the handler will be
 *     bound and when the incoming connection is complete the handler will
 *     receive the BTEVENT_LINK_CONNECT_CNF event. In this case all global
 *     registered handlers will receive BTEVENT_LINK_CONNECT_IND instead of
 *     BTEVENT_LINK_CONNECT_CNF.
 *
 *     If connections already exist to another devices then all
 *     existing connections will be put on hold while this operation
 *     takes place.
 *
 * Parameters:
 *     handler - pointer to registered handler to receive events.
 *
 *     bdAddr - address of remote device.
 *
 *     psi - pointer to page scan information obtained during 
 *         Inquiry. If this information is not known then "psi" should be 
 *         set to 0.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation was started successfully and
 *         results will be sent to the handler.
 *
 *     BT_STATUS_RESTRICTED - operation failed because this remote
 *         device already has a restricted link.
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because this
 *         remote device is in the process of being disconnected.
 *
 *     BT_STATUS_NO_RESOURCES - the operation failed because 
 *         the maximum number of connections already exist. The
 *         handler is free.
 *
 *     BT_STATUS_FAILED - operation failed because the handler
 *         is 0, the callback is 0 or the handler is not initialized
 *         (XA_ERROR_CHECK only). The handler is free.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_CreateRestrictedLink(BtHandler *handler, BD_ADDR *bdAddr,
                                 BtPageScanInfo *psi,
                                 BtRestrictedInfo *resInfo);

/*---------------------------------------------------------------------------
 * ME_ForceDisconnectLinkWithReason()
 *
 *     Force disconnection of the link regardless of registered handlers.
 *     See ME_DisconnectLink for additional information.
 *
 * Parameters:
 *     handler - pointer to registered handler (can be 0).
 *
 *     remDev - pointer to the remote device to be disconnected.
 *
 *     reason - disconnect reason. Must be one of: BEC_AUTHENTICATE_FAILURE,
 *          BEC_USER_TERMINATED, BEC_LOW_RESOURCES, BEC_POWER_OFF,
 *          BEC_UNSUPPORTED_REMOTE, or BEC_PAIR_UNITKEY_NO_SUPP.
 *
 *     forceDisconnect - If TRUE, will attempt to disconnect regardless of
 *         whether other handlers are still registered. If FALSE, behaves as
 *         ME_DisconnectLinkWithReason.
 *
 * Returns:
 *     BT_STATUS_INVALID_PARM - the operation failed because the "reason"
 *         parameter is invalid for a disconnect link operation.
 *
 *     See ME_DisconnectLink for additional returns.
 */
BtStatus ME_ForceDisconnectLinkWithReason(BtHandler *handler,
                                          BtRemoteDevice *remDev,
                                          BtErrorCode reason,
                                          BOOL forceDisconnect);

/*---------------------------------------------------------------------------
 * ME_DisconnectLinkWithReason()
 *
 *     Request disconnection of the link with a non-default reason code.
 *     See ME_DisconnectLink for additional information.
 *
 * Parameters:
 *     handler - pointer to registered handler.
 *
 *     remDev - pointer to the remote device to be disconnected.
 *
 *     reason - disconnect reason. If multiple handlers call this
 *         function or ME_DisconnectLink, the last handler's reason code
 *         will be used. If reason is BEC_POWER_OFF, disconnect is
 *         more forceful in that all the handlers need not have already
 *         been freed.
 *
 * Returns:
 *     BT_STATUS_INVALID_PARM - the operation failed because the "reason"
 *         parameter is invalid for a disconnect link operation.
 *
 *     See ME_DisconnectLink for additional returns.
 */
BtStatus ME_DisconnectLinkWithReason(BtHandler* handler,
                                     BtRemoteDevice* remDev,
                                     BtErrorCode reason);
#define ME_DisconnectLinkWithReason(ha, rd, rs) \
    (ME_ForceDisconnectLinkWithReason(ha, rd, rs, FALSE))

/*---------------------------------------------------------------------------
 * ME_DisconnectLink()
 *
 *     Request disconnection of the link, using the default reason code
 *     (BEC_USER_TERMINATED). This function also deregisters the
 *     specified handler, which was previously registered with ME_CreateLink
 *     or ME_CreateRestrictedLink.
 *
 *     The link is actually closed only when all registered handlers have
 *     requested disconnection and all L2CAP connections are closed. If
 *     a device needs to know exactly when the disconnection occurs,
 *     it should use a global handler to monitor connection state. The
 *     handler specified in this API is freed by this function, so it
 *     will not be notified of any disconnection events.
 *
 *     If the remote device initiated the connection, the local
 *     device may have no registered handler for the connection. In this
 *     case, the local device must call ME_CreateLink or
 *     ME_CreateRestrictedLink to register a handler, then call this function
 *     to request disconnection.
 *     
 * Parameters:
 *     handler - pointer to registered handler.
 *
 *     remDev - pointer to the remote device to be disconnected.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the operation has completed successfully. 
 *         The caller's handler is unbound from the link. When the link 
 *         is actually disconnected the BTEVENT_LINK_DISCONNECT event will 
 *         be sent to all registered global handlers, unless the link had 
 *         not actually come up yet, in which case no event will be 
 *         generated. The handler is free upon return from this function.
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because the
 *         link is the process of coming up. The connect request has 
 *         been made by a remote device. Wait until the 
 *         BTEVENT_LINK_CONNECT_CNF event has been received. The handler 
 *         is not free.
 *
 *     BT_STATUS_FAILED - the operation failed. The link is
 *         already disconnected, disconnect has already been
 *         issued for this handler, or the handler is not
 *         bound to the link (XA_ERROR_CHECK only).
 */
BtStatus ME_DisconnectLink(BtHandler* handler, BtRemoteDevice* remDev);
#define ME_DisconnectLink(ha, rd) \
    (ME_ForceDisconnectLinkWithReason(ha, rd, BEC_USER_TERMINATED, FALSE))

/*---------------------------------------------------------------------------
 * ME_CancelCreateLink()
 *
 *     Request cancellation of the pending ME_CreateLink. This function
 *     also deregisters the specified handler, which was previously
 *     registered when the ME_CreateLink was initiated.
 *
 * Parameters:
 *     handler - pointer to registered handler.
 *
 *     remDev - pointer to the remote device associated with the
 *          ME_CreateLink to be canceled.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the operation has completed successfully. 
 *         The caller's handler is unbound from the link. If more than one
 *         handler is bound to the link, no further events will occur.
 *         Otherwise, the BTEVENT_LINK_CREATE_CANCEL event will be sent to
 *         all registered global handlers when the cancel completes. The
 *         global handler must still check the BtEvent "errCode" field
 *         in the event structure, as the cancel may fail and the link may
 *         require further maintenance. The handler is free upon return
 *         from this function.
 *
 *     BT_STATUS_FAILED - the operation failed. The link is already
 *         connected or disconnected, or the handler is not bound to
 *         the link (XA_ERROR_CHECK only).
 */
BtStatus ME_CancelCreateLink(BtHandler* handler, BtRemoteDevice* remDev);

/*---------------------------------------------------------------------------
 * ME_RegisterAcceptHandler()
 *
 *     Register the handler that will accept incoming connections.
 *     If no handler is registered then the Management Entity will
 *     accept all incoming connections automatically. If the
 *     handler is set it will be responsible for accepting 
 *     connections by calling ME_AcceptIncomingLink. The handler
 *     will be notified with the BTEVENT_LINK_CONNECT_REQ event. The
 *     p.remDev field of the btEvent will contain a pointer to the
 *     remote device.
 *
 * Parameters:
 *     handler - pointer to handler. This can be a registered global
 *         handler but does not have to be.
 *
 * Returns:
 *     Pointer to old handler. 
 */
BtHandler *ME_RegisterAcceptHandler(BtHandler *handler);

/*---------------------------------------------------------------------------
 * ME_AcceptIncomingLink()
 *
 *     Accept an incoming link. The incoming link is signaled by
 *     a call to the accept handler with the event 
 *     BTEVENT_LINK_CONNECT_REQ. 
 *
 * Parameters:
 *     remDev - pointer to the remote device structure representing
 *         the incoming connection. This was passed in the 
 *         "btEvent.p.remDev" field of the BTEVENT_LINK_CONNECT_REQ event.
 *
 *     role - desired role. BCR_MASTER cause a master/slave switch
 *         with the local device becoming the master. BCR_SLAVE will 
 *         cause the local device to remain the slave.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully
 *         and the result will be sent to the accept handler via
 *         the BTEVENT_LINK_ACCEPT_RSP event. When complete all registered 
 *         global handlers will receive the BTEVENT_LINK_CONNECT_IND event.
 *         The "errCode" field of the BtEvent will indicate the success.
 *
 *     BT_STATUS_FAILED - the operation failed because the remote
 *         device is not in a state to accept an incoming connection.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because the
 *         "remDev" parameter is invalid or 0 or the role parameter
 *         is invalid (XA_ERROR_CHECK only).
 *     
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_AcceptIncomingLink(BtRemoteDevice *remDev, BtConnectionRole role);

/*---------------------------------------------------------------------------
 * ME_RejectIncomingLink()
 *
 *     Reject an incoming link. The incoming link is signaled by
 *     a call to the accept handler with the event 
 *     BTEVENT_LINK_CONNECT_REQ. 
 *
 * Parameters:
 *     remDev - pointer to the remote device structure representing
 *         the incoming connection. This was passed in the 
 *         "btEvent.p.remDev" field of the BTEVENT_LINK_CONNECT_REQ event.
 *
 *     reason - reason for the rejection. The only reasons allowed
 *         are as follows:
 *
 *     BEC_LIMITED_RESOURCE - Host rejected due to limited resources
 *
 *     BEC_SECURITY_ERROR   - Host rejected due to security reasons
 *
 *     BEC_PERSONAL_DEVICE  - Host rejected (remote is personal device)
 *
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully
 *         and the result will be sent to the accept handler via
 *         the BTEVENT_LINK_REJECT_RSP event. 
 *
 *     BT_STATUS_FAILED - the operation failed because the remote
 *         device is not in a state to reject an incoming connection.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because the
 *         "remDev" parameter is invalid or 0 or the reason is not
 *         valid (XA_ERROR_CHECK only).
 *     
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_RejectIncomingLink(BtRemoteDevice *remDev, BtErrorCode reason);

/*---------------------------------------------------------------------------
 * ME_SetConnectionRole()
 *
 *     Set the role that the local device prefers in both incoming
 *     and outgoing connections. This role applies only if there is no
 *     registered accept handler when the Management Entity automatically
 *     accepts incoming connections. The connection role applies to
 *     all future connections; it does not affect existing connections.
 *
 * Parameters:
 *     role - The desired role must be BCR_MASTER or BCR_ANY. When set to
 *         BCR_ANY, incoming connections default to slave role and outgoing
 *         connections default to master role, the remote device may
 *         change these roles. When set to BCR_MASTER, incoming connections
 *         force a role switch and outgoing connections do not permit a 
 *         role switch. If the remote device refuses a master/slave switch,
 *         the connection will still succeed with role unchanged.
 *
 * Returns:
 *     The old role. 
 */
BtConnectionRole ME_SetConnectionRole(BtConnectionRole role);

/*---------------------------------------------------------------------------
 * ME_GetCurrentMode()
 *
 *     Get the current mode of the link to the remote device. The
 *     value is meaningless if a connection does not exist to
 *     the remote device. This function does not check for a valid
 *     remDev pointer.
 *
 * Parameters:
 *     rm - pointer to remote device.
 *
 * Returns:
 *     The current mode if a link exists otherwise the value
 *     is meaningless.
 */
BtLinkMode ME_GetCurrentMode(BtRemoteDevice *rm);
#define ME_GetCurrentMode(r) ((r)->mode)

/*---------------------------------------------------------------------------
 * ME_GetCurrentRole()
 *
 *     Get the current role played by the local device. The
 *     value is meaningless if a connection does not exist to
 *     the remote device. This function does not check for a valid
 *     remDev pointer. When the role is currently being discovered,
 *     the role BCR_UNKNOWN will be returned. When the role discovery
 *     completes, the BTEVENT_ROLE_CHANGE event will be indicated.
 *
 * Parameters:
 *     rm - pointer to remote device.
 *
 * Returns:
 *     The current role if a link exists otherwise the value
 *     is meaningless.
 */
BtConnectionRole ME_GetCurrentRole(BtRemoteDevice *rm);
#define ME_GetCurrentRole(r) ((r)->role > BCR_UNKNOWN ? BCR_UNKNOWN : (r)->role)

/*---------------------------------------------------------------------------
 * ME_Hold()
 *
 *     Put the ACL link associated with remDev in hold mode. When the
 *     link is actually placed in hold all registered and bound handlers
 *     will receive a BTEVENT_MODE_CHANGE event signaling the change.
 *
 * Parameters:
 *     remDev - pointer to remote device
 *
 *     max - maximum length of the hold interval. The hold time
 *         is calculated as max * 0.625 ms. Range is 0.625ms to 40.9 sec.
 *
 *     min - minimum length of the hold interval. The hold time
 *         is calculated as min * 0.625 ms. Range is 0.625ms to 40.9 sec.
 *         The parameter min should not be greater than max.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device 
 *         will receive the BTEVENT_MODE_CHANGE event. The "errCode" field 
 *         of the BtEvent will indicate the success or failure of the mode 
 *         change event. The "p.modeChange" field indicates for which remote 
 *         Device the change has occurred along with the new mode and 
 *         interval. It is possible that link is disconnected before the
 *         mode change has occurred. In that case the handlers will not
 *         receive BTEVENT_MODE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed because ACL link is
 *         in wrong state.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_Hold(BtRemoteDevice *remDev, U16 max, U16 min);

/*---------------------------------------------------------------------------
 * ME_StartSniff()
 *
 *     Start sniff mode for the ACL link specified by "remDev".
 *
 * Parameters:
 *     remDev - pointer to remote device
 *
 *     info - pointer to the sniff mode parameters. This structure may
 *            be freed after ME_StartSniff returns.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device 
 *         will receive the BTEVENT_MODE_CHANGE event. The "errCode" field 
 *         of the BtEvent will indicate the success or failure of the mode 
 *         change event. The "p.modeChange" field indicates for which remote 
 *         Device the change has occurred along with the new mode and 
 *         interval. It is possible that link is disconnected before the
 *         mode change has occurred. In that case the handlers will not
 *         receive BTEVENT_MODE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_StartSniff(BtRemoteDevice *remDev, const BtSniffInfo *info);


/*---------------------------------------------------------------------------
 * ME_StopSniff()
 *
 *     Stop sniff mode and enter active mode for the ACL link
 *     specified by remDev. 
 *          
 * Parameters:
 *     remDev - pointer to remote device.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device 
 *         will receive the BTEVENT_MODE_CHANGE event. The "errCode" field 
 *         of the BtEvent will indicate the success or failure of the mode 
 *         change event. The "p.modeChange" field indicates for which remote 
 *         Device the change has occurred along with the new mode and 
 *         interval. It is possible that link is disconnected before the
 *         mode change has occurred. In that case the handlers will not
 *         receive BTEVENT_MODE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - the parameters are not valid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed. Device is not in
 *         sniff mode.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_StopSniff(BtRemoteDevice *remDev);


/*---------------------------------------------------------------------------
 * ME_StartPark()
 *
 *     Start park mode for the ACL link specified by "remDev".
 *
 * Parameters:
 *     rm - pointer to remote device
 *
 *     max - acceptable longest period between beacons. The time
 *         is calculated as max * 0.625 ms. Range is 0.625ms to 40.9 sec.
 *
 *     min - acceptable shortest period between beacons. The time
 *         is calculated as min * 0.625 ms. Range is 0.625ms to 40.9 sec.
 *         The parameter min should not be greater than max,
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device 
 *         will receive the BTEVENT_MODE_CHANGE event. The "errCode" field 
 *         of the BtEvent will indicate the success or failure of the mode 
 *         change event. The "p.modeChange" field indicates for which remote 
 *         Device the change has occurred along with the new mode and 
 *         interval. It is possible that link is disconnected before the
 *         mode change has occurred. In that case the handlers will not
 *         receive BTEVENT_MODE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - the parameters are not valid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed. 
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_StartPark(BtRemoteDevice *remDev, U16 max, U16 min);


/*---------------------------------------------------------------------------
 * ME_StopPark()
 *
 *     Stop park mode and enter active mode for the ACL link
 *     specified by "remDev". 
 *          
 * Parameters:
 *     remDev - pointer to remote device.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device 
 *         will receive the BTEVENT_MODE_CHANGE event. The "errCode" field 
 *         of the BtEvent will indicate the success or failure of the mode 
 *         change event. The "p.modeChange" field indicates for which remote 
 *         Device the change has occurred along with the new mode and 
 *         interval. It is possible that link is disconnected before the
 *         mode change has occurred. In that case the handlers will not
 *         receive BTEVENT_MODE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - the parameters are not valid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed because the link is
 *         not in park.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_StopPark(BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * ME_StartParkAll()
 *
 *     Start park mode for all ACL links.
 *
 * Parameters:
 *     max - acceptable longest period between beacons. The time
 *         is calculated as max * 0.625 ms. Range is 0.625ms to 40.9 sec.
 *
 *     min - acceptable shortest period between beacons. The time
 *         is calculated as min * 0.625 ms. Range is 0.625ms to 40.9 sec.
 *         The parameter min should not be greater than max,
 *
 *     num - pointer to an integer that receives the number of devices
 *         that were requested to enter park mode. This count can be
 *         used in conjunction with BTEVENT_MODE_CHANGE events to determine
 *         when all devices are parked.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote devices 
 *         will receive the BTEVENT_MODE_CHANGE event. The "errCode" field 
 *         of the BtEvent will indicate the success or failure of the mode 
 *         change event. The "p.modeChange" field indicates for which remote 
 *         device the change has occurred along with the new mode and 
 *         interval. It is possible that link is disconnected before the
 *         mode change has occurred. In that case the handlers will not
 *         receive BTEVENT_MODE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - the parameters are not valid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed. 
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_StartParkAll(U16 max, U16 min, U8 *num);

/*---------------------------------------------------------------------------
 * ME_StopParkAll()
 *
 *     Return all parked ACL links to active mode.
 *          
 * Parameters:
 *     num - pointer to an integer that receives the number of devices
 *         that were requested to exit park mode. This count can be
 *         used in conjunction with BTEVENT_MODE_CHANGE events to determine
 *         when all devices are unparked.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device 
 *         will receive the BTEVENT_MODE_CHANGE event. The "errCode" field 
 *         of the BtEvent will indicate the success or failure of the mode 
 *         change event. The "p.modeChange" field indicates for which remote 
 *         Device the change has occurred along with the new mode and 
 *         interval. It is possible that link is disconnected before the
 *         mode change has occurred. In that case the handlers will not
 *         receive BTEVENT_MODE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - the parameters are not valid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed because the link is
 *         not in park.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_StopParkAll(U8 *num);

/*---------------------------------------------------------------------------
 * ME_SwitchRole()
 *
 *     Switch the current role the device is performing for the ACL link
 *     specified by remDev. If the current role is slave then switch to
 *     master. If the current role is master then switch to slave. The
 *     current role can be found via remDev.role. The result of the 
 *     operation will be returned via the BTEVENT_ROLE_CHANGE event.
 *
 * Parameters:
 *     remDev - pointer to remote device
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         All registered handlers and handlers bound to the remote device 
 *         will receive the BTEVENT_ROLE_CHANGE event. The "errCode" field 
 *         of the BtEvent will indicate the success or failure of the role 
 *         change event. The "p.roleChange" field indicates for which remote 
 *         Device the change has occurred along with the new role.  It is 
 *         possible that link is disconnected before the role change has 
 *         occurred. In that case the handlers will not receive 
 *         BTEVENT_ROLE_CHANGE but instead will receive
 *         BTEVENT_LINK_DISCONNECT.
 *
 *     BT_STATUS_INVALID_PARM - the parameters are not valid
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *     BT_STATUS_FAILED - the operation failed. 
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_SwitchRole(BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * ME_SetAccessibleModeNC()
 *
 *     Set the accessibility mode when the device is not 
 *     connected. If the mode is set to a value other than 
 *     BAM_NOT_ACCESSIBLE and there are no connections then the 
 *     Bluetooth radio will enter inquiry and/or page scan mode 
 *     using the information passed in info. If info is 0 or the
 *     values in info are set to defaults (BT_DEFAULT_SCAN_INTERVAL
 *     and BT_DEFAULT_SCAN_WINDOW) the radio module default values
 *     are used. It is assumed that the macro defaults
 *     match the radio defaults (see BT_DEFAULT_PAGE_SCAN_WINDOW
 *     documentation.) So, the first call to ME_SetAccessibleModeNC
 *     with info set to 0 will not change the settings as the radio has
 *     already been initialized to its default settings. If there is
 *     a connection or a connection is in the process  of being created
 *     then mode and info are saved and applied when all connections are 
 *     disconnected.  
 *
 *     To keep other devices from finding and connecting to this 
 *     device set the mode to BAM_NOT_ACCESSIBLE. The default mode when
 *     the stack is first loaded and initialized is controlled by
 *     BT_DEFAULT_ACCESS_MODE_NC.
 *
 *     In setting the values for info. Both "inqWindow" and
 *     "inqInterval" must be set to defaults or to legitimate
 *     values. The range for values is 0x0012 to 0x1000. The time
 *     calculated by taking the value * 0.625ms. It is an error
 *     if one is a default and the other is not. This is also true
 *     for "pageInterval" and "pageWindow".
 *
 *     Any time the scan interval or window is different from 
 *     the current settings in the radio, the radio will be 
 *     instructed to change to the new settings. This means that 
 *     if there are different settings for the connected state
 *     versus the non-connected state, the radio module will be 
 *     instructed to change the settings when the first connection 
 *     comes up and when the last connection goes down
 *     (automatically). This also means that if different values
 *     for window and interval are set when going from any setting
 *     of accessible to non-accessible then the radio will be 
 *     instructed to change. In most cases it is best to use
 *     the radio defaults. In this way the radio is never told
 *     to change the scan interval or window.
 *
 * Parameters:
 *     mode - desired accessibility mode
 *
 *     info - pointer to structure containing the inquiry and page
 *         scan interval and window to use. If info is 0 then the 
 *         defaults set by the radio module are used.
 *
 * Returns:
 *     BT_STATUS_PENDING - the mode is being set. All registered 
 *         global handlers with the BEM_ACCESSIBLE_CHANGE mask set will 
 *         receive BTEVENT_ACCESSIBLE_CHANGE event when the mode change 
 *         actually takes affect or an error has occurred. The "errCode"
 *         field of the BtEvent indicates the status of the operation. 
 *         If the operation is successful the "aMode" field of BtEvent
 *         indicates the new mode.  A BTEVENT_HCI_FATAL_ERROR event
 *         indicates a fatal radio or HCI transport error and that all
 *         pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - Accessible mode is set. No event
 *         is sent out. This is returned if a connection exists and 
 *         the values are only saved or info already matches the current
 *         setting.
 *
 *     BT_STATUS_IN_PROGRESS - operation failed because a change
 *         is already in progress. Monitor the global events to 
 *         determine when the change has taken place.
 *
 *     BT_STATUS_INVALID_PARM - operation failed. The mode or info 
 *         parameter contains bad values (XA_ERROR_CHECK only)
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_SetAccessibleModeNC(BtAccessibleMode mode, const BtAccessModeInfo *info);

/*---------------------------------------------------------------------------
 * ME_GetAccessibleModeNC()
 *
 *     Return the current accessibility mode that is used when no
 *     connections exist. This is not necessarily the current mode.
 *
 * Parameters:
 *     mode - pointer to memory to receive accessibility mode. If
 *         mode is 0 then mode is not returned. If the accessible mode
 *         has not yet been set, and therefore unknown, 0xff will be returned.
 *
 *     info - pointer to structure to receive accessibility info.
 *         If info is 0 then info is not returned.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation was successful
 *
 *     BT_STATUS_IN_PROGRESS - operation failed because a change
 *         is in progress. Monitor the global events to determine when
 *         the change is finished.
 *
 *     BT_STATUS_FAILED - operation failed.
 */
BtStatus ME_GetAccessibleModeNC(BtAccessibleMode *mode,
                                BtAccessModeInfo *info);

/*---------------------------------------------------------------------------
 * ME_GetCurAccessibleMode()
 *
 *     Return the current accessibility mode. This is the mode
 *     at the very instant that call is made. It may be about
 *     to change so it is important to check the global events.
 *
 * Parameters:
 *     mode - pointer to memory to receive accessibility mode. If
 *         mode is 0 then mode is not returned. If the accessible mode
 *         has not yet been set, and therefore unknown, 0xff will be returned.
 *
 *     info - pointer to structure to receive accessibility info.
 *         If info is 0 then info is not returned.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation was successful
 *
 *     BT_STATUS_FAILED - operation failed.
 */
BtStatus ME_GetCurAccessibleMode(BtAccessibleMode *mode,
                                 BtAccessModeInfo *info);


#if BT_ALLOW_SCAN_WHILE_CON == XA_ENABLED
/*---------------------------------------------------------------------------
 * ME_SetAccessibleModeC()
 *
 *     Set the accessibility mode to be used when the device is 
 *     connected to one or more devices. If the mode is set to a 
 *     value other than BAM_NOT_ACCESSIBLE and there is a 
 *     connection to one or more devices then the Bluetooth radio 
 *     will enter inquiry and/or page scan mode using the 
 *     information passed in info. If info is 0 or the values in 
 *     info are set to defaults (BT_DEFAULT_SCAN_INTERVAL and 
 *     BT_DEFAULT_SCAN_WINDOW) the radio module default values 
 *     are used. It is assumed that the macro defaults
 *     match the radio defaults (see BT_DEFAULT_PAGE_SCAN_WINDOW
 *     documentation.) So, the first call to ME_SetAccessibleModeC
 *     with info set to 0 will not change the settings as the radio has
 *     already been initialized to its default settings. If there are
 *     no active connections then mode and info are saved and applied
 *     when the first connection comes up.  
 *
 *     To keep other devices from finding and connecting to this 
 *     device when connected set the mode to BAM_NOT_ACCESSIBLE. 
 *     The default mode when the stack is first loaded and initialized 
 *     is controlled by BT_DEFAULT_ACCESS_MODE_C.
 *
 *     In setting the values for info. Both "inqWindow" and
 *     "inqInterval" must be set to defaults or to legitimate
 *     values. The range for values is 0x0012 to 0x1000. The time
 *     is calculated by taking the value * 0.625ms. It is an error
 *     if one field (interval/window) is a default and the other is 
 *     not. This also true for "pageInterval" and "pageWindow".
 *
 *     Any time the scan interval or window is different from 
 *     the current settings in the radio, the radio will be 
 *     instructed to change to the new settings. This means that 
 *     if there are different settings for the connected state
 *     versus the non-connected state, the radio module will be 
 *     instructed to change the settings when the first connection 
 *     comes up and when the last connection goes down
 *     (automatically). This also means that if different values
 *     for window and interval are set when going from any setting
 *     of accessible to non-accessible then the radio will be 
 *     instructed to change. In most cases it is best to use
 *     the radio defaults. In this way the radio is never told
 *     to change the scan interval or window.
 *
 * Requires:
 *     BT_ALLOW_SCAN_WHILE_CON enabled.
 * 
 * Parameters:
 *     mode - desired accessibility mode
 *
 *     info - pointer to structure containing the inquiry and page
 *         scan interval and window to use. If info is 0 then the 
 *         defaults set by the radio module are used.
 *
 * Returns:
 *     BT_STATUS_PENDING - the mode is being set. All registered 
 *         global handlers with the BEM_ACCESSIBLE_CHANGE mask set will 
 *         receive BTEVENT_ACCESSIBLE_CHANGE event when the mode change 
 *         actually takes affect or an error has occurred. The "errCode"
 *         field of the BtEvent indicates the status of the operation. 
 *         If the operation is successful the "aMode" field of BtEvent
 *         indicates the new mode. A BTEVENT_HCI_FATAL_ERROR event
 *         indicates a fatal radio or HCI transport error and that all
 *         pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - Accessible mode is set. No event
 *         is sent out. This is returned if no connections exist and 
 *         the values are only saved or info already matches the current
 *         setting.
 *
 *     BT_STATUS_IN_PROGRESS - operation failed because a change
 *         is already in progress. Monitor the global events to 
 *         determine when the change has taken place.
 *
 *     BT_STATUS_INVALID_PARM - operation failed. The mode or info 
 *         parameter contains bad values (XA_ERROR_CHECK only)
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_SetAccessibleModeC(BtAccessibleMode mode, const BtAccessModeInfo *info);


/*---------------------------------------------------------------------------
 * ME_GetAccessibleModeC()
 *
 *     Return the current accessibility mode that is used when 
 *     connections exist. This is not necessarily the current 
 *     mode.
 *
 * Requires:
 *     BT_ALLOW_SCAN_WHILE_CON enabled.
 * 
 * Parameters:
 *     mode - pointer to memory to receive accessibility mode. If
 *         mode is 0 then mode is not returned. If the accessible mode
 *         has not yet been set, and therefore unknown, 0xff will be returned.
 *
 *     info - pointer to structure to receive accessibility info.
 *         If info is 0 then info is not returned.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation was successful
 *
 *     BT_STATUS_IN_PROGRESS - operation failed because a change
 *         is in progress. Monitor the global events to determine when
 *         the change is finished.
 *
 *     BT_STATUS_FAILED - operation failed.
 */
BtStatus ME_GetAccessibleModeC(BtAccessibleMode *mode, BtAccessModeInfo *info);

#endif /* BT_ALLOW_SCAN_WHILE_CON */


/*---------------------------------------------------------------------------
 * ME_SendHciCommandSync()
 *
 *     Send an HCI command synchronously. This function is used to send HCI
 *     commands not handled directly by the ME and radio specific 
 *     user defined commands. Synchronously means that the command will 
 *     block other HCI commands until it is complete. Use ME_SendHciCommandSync
 *     for HCI operations that return with the "Command Complete" event
 *     or local operations that do no require communication with a remote device.
 *     The "Command Status" event with an error will also cause the operation 
 *     to complete. If the function returns BT_STATUS_PENDING then the result 
 *     will be returned via the callback in the token with the 
 *     BTEVENT_COMMAND_COMPLETE event.
 *
 *     The caller must fill in the "in" fields of the MeCommandToken.
 *     The "out" fields of the MeCommandToken will contain the result
 *     of the operation. The "errCode" field of the BtEvent is not valid
 *     since the "errCode" is part of the event and its location in "parms"
 *     varies.
 *
 * Parameters:
 *     token - pointer to token containing input parameters and
 *         storage for output parameters. The token must be initialized 
 *         before calling this function as follows:
 *
 *         "token.callback" must be set to the callback function.
 *
 *         "token.p.general.in.hciCommand" must contain the desired command.
 *         The command can be one of the HCI commands defined in hci.h that
 *         is not supported by the ME or user defined commands.
 *
 *         "token.p.general.in.parmLen" must contain the length of the 
 *         parameter.
 *
 *         "token.p.general.in.parms" must point to the command parameters.
 *         The memory pointed to by "parms" must be valid until the operation
 *         is complete.  
 *
 *         "token.p.general.in.event" must contain the event that is expected
 *         to signal completion of the operation. The most common event is
 *         "Command Complete". See hci.h for a list of events. The "Command 
 *         Status" event is always checked. If a "Command Status" event is
 *         received with an error then the operation is considered complete.
 *         If a "Command Status" is received without an error then the command
 *         will finish when the event matches "token.p.general.in.event".
 *
 * Returns:
 *     BT_STATUS_PENDING - Operation is started and results will
 *         be sent to the callback. A BTEVENT_HCI_FATAL_ERROR event
 *         indicates a fatal radio or HCI transport error and all
 *         pending operations have failed.
 *  
 *     BT_STATUS_INVALID_TYPE - Operation failed because 
 *         "token.p.general.in.hciCommand" is a command already handled by 
 *         the ME or "token.p.general.in.event" is an asynchronous event 
 *         already handled by the ME.
 *
 *     BT_STATUS_INVALID_PARM - Operation failed because the parameter
 *         length exceeds HCI_CMD_PARM_LEN, the maximum allowed
 *         parameter size for an HCI command.
 *        
 *     BT_STATUS_FAILED - operation failed because the token was
 *         not properly initialized or already active (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_SendHciCommandSync(MeCommandToken *token);

/*---------------------------------------------------------------------------
 * ME_SendHciCommandAsync()
 *
 *     Send an HCI command asynchronously. This function is used to send HCI
 *     commands not implemented explicitly by the ME and radio specific 
 *     user defined commands. Asynchronously means that the command will not
 *     block other HCI commands. It will wait for a "Command Status" event 
 *     but then will allow other HCI commands to execute as it waits for
 *     the asynchronous event signaling the completion of the operation. 
 *     The result will be returned when the appropriate event as set in 
 *     "token.p.general.in.event" is received. If the function returns 
 *     BT_STATUS_PENDING then the result will be  returned via the callback 
 *     in the token with the BTEVENT_COMMAND_COMPLETE event.
 *
 *     The caller must fill in the "in" fields of the MeCommandToken.
 *     The "out" fields of the MeCommandToken will contain the result
 *     of the operation. The "errCode" field of the BtEvent is not valid
 *     since the "errCode" is part of the event and its location in "parms"
 *     varies.
 *
 *     Only one Async operation with the same event is allowed to
 *     execute at a time. Any additional requests are queued until the
 *     first command is complete.
 *
 * Parameters:
 *     token - pointer to token containing input parameters and
 *         storage for output parameters. The token must be initialized 
 *         before calling this function as follows:
 *
 *         "token.callback" must be set to the callback function.
 *
 *         "token.p.general.in.hciCommand" must contain the desired command.
 *         The command can be one of the HCI commands defined in hci.h that
 *         is not supported by the ME or user defined commands.
 *
 *         "token.p.general.in.parmLen" must contain the length of the 
 *         parameter.
 *
 *         "token.p.general.in.parms" must point to the command parameters.
 *         The memory pointed to by "parms" must be valid until the operation
 *         is complete.  
 *
 *         "token.p.general.in.event" must contain the event that is expected
 *         to signal completion of the operation. The "Command Status" event 
 *         is always checked. If a "Command Status" event is returned 
 *         with an error then the operation is considered complete. If a 
 *         "Command Status" is returned without an error then the command
 *         will finish when the event matches "token.p.general.in.event".
 *
 * Returns:
 *     BT_STATUS_PENDING - Operation is started and results will
 *         be sent to the callback. A BTEVENT_HCI_FATAL_ERROR event
 *         indicates a fatal radio or HCI transport error and all
 *         pending operations have failed.
 *
 *     BT_STATUS_INVALID_TYPE - Operation failed because 
 *         "token.p.general.in.hciCommand" is a command already handled by 
 *         the ME or "token.p.general.in.event" is an asynchronous event 
 *         already handled by the ME.
 *
 *     BT_STATUS_INVALID_PARM - Operation failed because the parameter
 *         length exceeds HCI_CMD_PARM_LEN, the maximum allowed
 *         parameter size for an HCI command.
 *        
 *     BT_STATUS_FAILED - operation failed because the token was
 *         not properly initialized or already active (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus ME_SendHciCommandAsync(MeCommandToken *token);


/*---------------------------------------------------------------------------
 * ME_SetDefaultLinkPolicy()
 *
 *     Set the default link policy used on outgoing and incoming ACL 
 *     connections. The link policy determines the behavior of the local
 *     link manager when it receives a request from a remote device or it
 *     determines itself to change the master-slave role or to enter hold,
 *     sniff, or park mode. The default if this function is not called 
 *     is to disable all modes.
 *
 *     Policy for incoming ACL connections is set independent of policy
 *     for outgoing connections. The default policy is applied immediately 
 *     after the link comes up. An higher level Management Entity should
 *     use this function, not applications. It should be called before any
 *     ACL connections exist and does not effect existing
 *     ACL connections.
 *
 * Parameters:
 *     inACL - default link policy applied to all incoming ACL connections.
 *         Incoming ACL connections are those initiated by a remote device.
 *
 *     outACL - default link policy applied to all outgoing ACL connections.
 *         Outgoing ACL connections are those initiated by the local device.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation was successful.
 *
 *     BT_STATUS_FAILED - operation failed because the policy settings are
 *         not valid (error checking only).
 */
BtStatus ME_SetDefaultLinkPolicy(BtLinkPolicy inACL, BtLinkPolicy outACL);

/*---------------------------------------------------------------------------
 * ME_SetConnectionPacketType()
 *
 *     Changes the allowed ACL packet types for the specified remote device.
 *
 * Parameters:
 *     remDev - pointer to remote device
 *
 *     packetTypes - allowed connection packet types.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was started successfully. The stack
 *         will attempt to change the packet types for the connected
 *         remote device. No notification will be made on success or
 *         failure of this attempt.
 *
 *     BT_STATUS_INVALID_PARM - The parameters are not valid
 *         (XA_ERROR_CHECK only).
 *        
 *     BT_STATUS_IN_PROGRESS - the operation failed because another
 *         operation is already in progress.
 *
 */
BtStatus ME_SetConnectionPacketType(BtRemoteDevice *remDev, 
                                       BtAclPacketType packetTypes);

/*---------------------------------------------------------------------------
 * ME_GetHciConnectionHandle()
 *
 *     Get the HCI connection handle for the link to the remote device. The
 *     value is meaningless if a connection does not exist to the remote 
 *     device.
 *
 * Parameters:
 *     rm - pointer to remote device.
 *
 * Returns:
 *     The HCI connection handle a link exists otherwise the value
 *     is meaningless.
 */
U16 ME_GetHciConnectionHandle(BtRemoteDevice *rm);

/*---------------------------------------------------------------------------
 * ME_GetBdAddr()
 *
 *     Get the 48-bit address of the remote device. The value is meaningless 
 *     if a connection does not exist to the remote  device.
 *
 * Parameters:
 *     rm - pointer to remote device.
 *
 *     bdAddr - pointer to a BD_ADDR structure to receive the result.
 */
void ME_GetBdAddr(BtRemoteDevice *rm, BD_ADDR* bdAddr);

/*---------------------------------------------------------------------------
 * ME_GetRemDevState()
 *
 *     Get the connection state of the remote device. This function
 *     does not check for a valid remDev pointer.
 *
 * Parameters:
 *     remDev - pointer to remote device.
 *
 * Returns:
 *     BtRemDevState
 */
BtRemDevState ME_GetRemDevState(BtRemoteDevice *remDev);
#define ME_GetRemDevState(r) ((r)->state)

/*---------------------------------------------------------------------------
 * ME_GetRemDevCOD()
 *
 *     Get the Class of Device for the remote device. This function
 *     does not check for a valid remDev pointer.
 *
 * Parameters:
 *     remDev - pointer to remDev.
 *
 * Returns:
 *     BtClassOfDevice
 */
BtClassOfDevice ME_GetRemDevCOD(BtRemoteDevice *remDev);
#define ME_GetRemDevCOD(r) ((r)->cod)

/*---------------------------------------------------------------------------
 * ME_GetRemDevEncryptState()
 *
 *     Get the encryption state of the remote device. This function
 *     does not check for a valid remDev pointer.
 *
 * Parameters:
 *     remDev - pointer to remDev.
 *
 * Returns:
 *     Zero for no encryption, non-zero for encryption.
 */
BtEncryptState ME_GetRemDevEncryptState(BtRemoteDevice *remDev);
#define ME_GetRemDevEncryptState(r) ((r)->encryptState)

/*---------------------------------------------------------------------------
 * ME_GetStackInitState()
 *
 *     Reads the initialization state of the stack.
 *
 * Returns:
 *     BtStackState - If the value is BTSS_INITIALIZED the stack is 
 *         initialized and ready.
 */
BtStackState ME_GetStackInitState(void);

/*---------------------------------------------------------------------------
 * ME_GetBtVersion()
 *
 *     Gets the Bluetooth version of the local radio.
 *
 * Returns:
 *     0 for BT v1.0.
 *     1 for BT v1.1.
 *     2 for BT v1.2.
 *     3 for BT v2.0.
 */
U8 ME_GetBtVersion(void);

/*---------------------------------------------------------------------------
 * ME_GetBtFeatures()
 *
 *     Gets the value of the specified byte of the Bluetooth features
 *     bitmask from the local radio. See the Bluetooth specification for
 *     a description of the features bitmask.
 *
 * Parameters:
 *     byte - byte to retrieve. Must be between 0 and 7.
 *
 * Returns:
 *     Value of the specified byte of the BT features bitmask.
 */
U8 ME_GetBtFeatures(U8 byte);


/*---------------------------------------------------------------------------
 * ME_CheckRadioFeature()
 *
 *     Indicates whether the local radio supports the specified feature.
 *
 * Parameters:
 *     feature - Feature code
 *
 * Returns:
 *     TRUE if radio indicates its support for the feature. FALSE if not.
 * 
 */
BOOL ME_CheckRadioFeature(BtRadioFeature feature);

/*---------------------------------------------------------------------------
 * ME_HasPendingTask()
 *
 *     Checks for pending ME tasks. The ME task is set when an HCI command
 *     is sent to the controller. ME_HasPendingTask will return the event
 *     expected to complete the pending HCI command.
 *
 * Returns:
 *     Pending HCI event associated with the completion of the current
 *     task or 0 for no pending tasks. See HciEventType in hci.h for possible
 *     return values. If not enough information is available to determine the
 *     completion event, ME_HasPendingTask returns 0xFF00.
 */
U16 ME_HasPendingTask(void);

/*---------------------------------------------------------------------------
 * ME_GetChannelClassification()
 *
 *     Gets the channel classification currently stored in ME for Adaptive
 *     Frequency Hopping (AFH.) See ME_SetChannelClassification.
 *
 * Parameters:
 *     channelClass - BtChannelClass structure containing buffer to receive
 *         the stored channel classification.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The current channel classification has been
 *         copied to the BtChannelClass structure.
 */
BtStatus ME_GetChannelClassification(BtChannelClass *channelClass);

/*---------------------------------------------------------------------------
 * ME_SetChannelClassification()
 *
 *     Sets the channel classification for Adaptive Frequency Hopping (AFH.)
 *     Each of the 79 1-MHz channels in the Bluetooth 2.4-GHz band may be
 *     marked bad or unknown. The AFH controller will not use channels
 *     marked bad. The AFH controller will evaluate for use any channels
 *     marked unknown (if automatic classification mode is enabled.) Bad
 *     channels are indicated with a 0 value. Unknown channels are
 *     indicated with a 1 value in the bit-mask.
 *
 * Parameters:
 *     channelClass - 79 1-bit fields in a 10-byte array, indicating channels
 *         marked bad or unknown. The nth field (0 - 78) indicates the value
 *         for channel n. Bit 79 is unused and must be 0.
 *
 *     autoMode - Boolean value indicating whether the controller should
 *         automatically determine channel classification for the channels
 *         marked unknown.
 *     
 * Returns:
 *     BT_STATUS_SUCCESS - the channel classification has been sent to the
 *         controller.
 *
 *     BT_STATUS_IN_PROGRESS - another ME_SetChannelClassification operation
 *         is currently in progress. Try again later.
 *
 *     BT_STATUS_INVALID_PARM - The parameters are not valid
 *         (XA_ERROR_CHECK only).
 */
BtStatus ME_SetChannelClassification(BtChannelClass *channelClass, BOOL autoMode);


/*---------------------------------------------------------------------------
 * ME_MarkAfhChannels()
 *
 *     Sets or clears the bits associated with the indicated range of
 *     channels in the BtChannelClass map. If the "initialize" parameter is
 *     TRUE, also initializes the BtChannelClass map to the "Unknown" state.
 *
 * Parameters:
 *     channelClass - BtChannelClass structure containing buffer to be
 *         modified.
 *
 *     begin - the first channel (0 - 78) in the range to set.
 *
 *     count - the number of channels to set starting at "begin".
 *
 *     state - If TRUE, the Bluetooth channel will be set to the "Unknown"
 *         state. If FALSE, the Bluetooth channel will be set to the "Bad"
 *         state.
 *
 *     initialize - If TRUE, the BtChannelClass structure will be initialized
 *         to the "Unknown" state. If FALSE, the structure will be modified
 *         as is.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The channels have been marked successfully.
 */
BtStatus ME_MarkAfhChannels(BtChannelClass *channelClass, U8 begin, U8 count,
                            BOOL state, BOOL initialize);

/*---------------------------------------------------------------------------
 * ME_MarkWiFiChannel()
 *
 *     Sets or clears the bits associated with the indicated WiFi channel in
 *     the BtChannelClass map. If the "initialize" parameter is TRUE, also
 *     initializes the BtChannelClass map to the "Unknown" state.
 *
 * Parameters:
 *     channelClass - BtChannelClass structure containing buffer to be
 *         modified.
 *
 *     wifiChannel - WiFi channel (1-14) to set.
 *
 *     state - If TRUE, the WiFi channel will be set to the "Unknown"
 *         state. If FALSE, the WiFi channel will be set to the "Bad"
 *         state.
 *
 *     initialize - If TRUE, the BtChannelClass structure will be initialized
 *         to the "Unknown" state and then set as indicated. If FALSE, the
 *         structure will be modified as is.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The channels have been marked successfully.
 */
BtStatus ME_MarkWiFiChannel(BtChannelClass *channelClass, U8 wifiChannel,
                            BOOL state, BOOL initialize);

/*---------------------------------------------------------------------------
 * ME_RadioInit()
 *
 *     Called to initialize the radio module, including the HCI 
 *     transport drivers, the HCI, and the radio module.  This function is
 *     called by the Event Manager in EVM_Init() when the stack initialization 
 *     sequence begins, but can be called by applications afterwards if 
 *     necessary (ME_RadioShutdown() has been called).  When initialization 
 *     is successful, the BTEVENT_HCI_INITIALIZED event will be received. 
 *
 * Parameters:  void
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Initialization was successful.  The application will
 *         be notified during the call that initialization is complete with
 *         BTEVENT_HCI_INITIALIZED.

 *     BT_STATUS_PENDING - Initialization was started successfully.  The
 *         application will be notified when initialization is complete.  If 
 *         initialization is successful, BTEVENT_HCI_INTIALIZED will be 
 *         received.  If initialization fails, BTEVENT_HCI_INIT_ERROR will be 
 *         received.  Initialization will be retried BT_HCI_NUM_INIT_RETRIES
 *         times when an error occurs.  After initialization has been retried 
 *         and has failed the maximum number of times, BTEVENT_HCI_FAILED will
 *         be received.

 *     BT_STATUS_FAILED - Initialization failed to start.
 */
BtStatus ME_RadioInit(void);
#define ME_RadioInit() RMGR_RadioInit()

/*---------------------------------------------------------------------------
 * ME_RadioShutdown()
 *
 *     Gracefully shuts down the radio module, including the transports and 
 *     the HCI.  ME_RadioShutdown will wait 1 second for pending operations
 *     before shutting down.  (Note: Use ME_RadioShutdownTime if you need to
 *     specify the wait time.)  When the shutdown is complete, the global
 *     handler will receive a BTEVENT_HCI_DEINITIALIZED event.
 *
 *     This function must not be called during a callback.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The radio is down. The application will be notified
 *         during the call that the shutdown was complete with 
 *         BTEVENT_HCI_DEINITIALIZED.
 *     BT_STATUS_FAILED - The radio is already down.
 *     BT_STATUS_PENDING - The radio will be shut down. The application will 
 *         be notified when shutdown is complete with BTEVENT_HCI_DEINITIALIZED.
 */
BtStatus ME_RadioShutdown(void);
BtStatus ME_RadioShutdownTime(TimeT time);
#define ME_RadioShutdown() ME_RadioShutdownTime(1000)

#endif /* __ME_H */

