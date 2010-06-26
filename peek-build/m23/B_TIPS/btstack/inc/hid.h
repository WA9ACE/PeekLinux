#ifndef __HID_H
#define __HID_H
/*****************************************************************************
 *
 * File:
 *     $Workfile:hid.h$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision:21$
 *
 * Description: Public types, defines, and prototypes for accessing the
 *              HID layer.
 * 
 * Created:     Aug 7, 2002
 *
 * Copyright 2002-2004 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems,
 * Inc.  This work contains confidential and proprietary information of
 * Extended Systems, Inc. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include <l2cap.h>
#include <conmgr.h>

/*---------------------------------------------------------------------------
 * HID Protocol API layer
 *
 *     The Bluetooth Human Interface Device Profile allows an HID Device
 *     (such as a keyboard or mouse) to send control and interrupt information
 *     to an HID host (such as a PC) and vice versa. This API enables an
 *     application to perform either role of the HID profile.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HID_HOST constant
 *     Defines the role of the device as a host.  HID Host code is included
 *     if HID_HOST is defined as XA_ENABLED.
 */
#ifndef HID_HOST
#define HID_HOST XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * HID_DEVICE constant
 *     Defines the role of the device as an HID device.  HID Device code is 
 *     included if HID_DEVICE is defined as XA_ENABLED.
 */
#ifndef HID_DEVICE
#define HID_DEVICE XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * HID_NUM_TX_PACKETS constant
 *     Defines the number of transmit packets available for sending control
 *     or interrupt data.
 */
#ifndef HID_NUM_TX_PACKETS
#define HID_NUM_TX_PACKETS 5
#endif

#if HID_NUM_TX_PACKETS > 255
#error HID_NUM_TX_PACKETS must not exceed 255
#endif

/*---------------------------------------------------------------------------
 * HID_NUM_SDP_ATTRIBUTES constant
 *     Defines the number of attributes in the Device's SDP database.  The
 *     default value is 24, which is the number of attributes in the default
 *     database entry.  If any SDP attributes are added or removed from
 *     hid_sdp.c, then this number must be modified appropriately.
 */
#ifndef HID_NUM_SDP_ATTRIBUTES
#define HID_NUM_SDP_ATTRIBUTES 24
#endif

/*---------------------------------------------------------------------------
 * HID_DEVICE_RELEASE constant
 *     Vendor defined release version number for the device.  The default value
 *     is 0x0100 (1.0).  This information is stored in the SDP database of the
 *     HID device.
 */
#ifndef HID_DEVICE_RELEASE
#define HID_DEVICE_RELEASE 0x0100
#endif

/*---------------------------------------------------------------------------
 * HID_PARSER_VERSION constant
 *     Version number for the HID parser for which the device was designed.  
 *     The default value is 0x0111 (v1.11).  This information is stored in the
 *     SDP database of the HID device.
 */
#ifndef HID_PARSER_VERSION
#define HID_PARSER_VERSION 0x0111
#endif
      
/*---------------------------------------------------------------------------
 * HID_DEVICE_SUBCLASS constant
 *     Defines the type of device.  This is equivalent to the low order
 *     8 bytes of the Class of Device/Service Class field.  The default value
 *     is "Unspecified."  This information is stored in the SDP database of the 
 *     HID device.
 */
#ifndef HID_DEVICE_SUBCLASS
#define HID_DEVICE_SUBCLASS   ((U8)(COD_MINOR_PERIPH_UNCLASSIFIED))
#endif
      
/*---------------------------------------------------------------------------
 * HID_COUNTRY_CODE constant
 *     Country code.  The default value is 0x21 (USA).  This information is
 *     stored in the SDP database of the HID device.
 */
#ifndef HID_COUNTRY_CODE
#define HID_COUNTRY_CODE 0x21
#endif
      
/*---------------------------------------------------------------------------
 * HID_VIRTUAL_CABLE constant
 *     Indicates whether the device supports virtual connections.  Devices
 *     which set this value to TRUE indicate that they support a 1:1 bonding
 *     with the host and expect to automatically re-connect if the connection
 *     is dropped.  The default value is TRUE.  This information is stored in
 *     the SDP database of the HID device.
 */
#ifndef HID_VIRTUAL_CABLE
#define HID_VIRTUAL_CABLE TRUE
#endif
      
/*---------------------------------------------------------------------------
 * HID_RECONNECT_INITIATE constant
 *     Indicates that the device will attempt to re-connect to the host if
 *     the connection is dropped.  If set to TRUE, the device will connect
 *     to the host when the connection is dropped.  If set to FALSE, the
 *     device must be in a state to accept a connection from the host.
 *     Establishing a connection and setting the connectabililty modes is
 *     the responsibility of the application.  The default value is TRUE.  This
 *     information is stored in the SDP database of the HID device.
 */
#ifndef HID_RECONNECT_INITIATE
#define HID_RECONNECT_INITIATE TRUE
#endif
      
/*---------------------------------------------------------------------------
 * HID_DESCRIPTOR_TYPE constant
 *     Defines the type of descriptor defined by HID_DESCRIPTOR. The value
 *     can be either 0x22 (Report) or 0x23 (Physical Descriptor).
 *     A sample descriptor of type "Report" is provided.
 */
#ifndef HID_DESCRIPTOR_TYPE
#define HID_DESCRIPTOR_TYPE 0x22
#endif

/*---------------------------------------------------------------------------
 * HID_DESCRIPTOR_LEN constant
 *     Indicates the length of the descriptor defined in HID_DESCRIPTOR.  
 *     This length must match the number of elements defined in HID_DESCRIPTOR.
 *     A sample length and descriptor are provided.
 */
#ifndef HID_DESCRIPTOR_LEN
#define HID_DESCRIPTOR_LEN 50
#endif

/*---------------------------------------------------------------------------
 * HID_DESCRIPTOR constant
 *     Provides a HID descriptor.  See the explanation of this descriptor in
 *     section 7.11.6 of the Bluetooth HID Profile Specification.  The default
 *     value is a sample descriptor list for a typical mouse.  A comma must
 *     separate each 8-bit elemnt, and no comma should be placed at the very end.
 *     This information is stored in the SDP database of the HID device.
 */
#ifndef HID_DESCRIPTOR
#define HID_DESCRIPTOR \
        0x05, 0x01,  /* USAGE_PAGE(Generic Desktop)   */ \
        0x09, 0x02,  /* USAGE(Mouse)                  */ \
        0xA1, 0x01,  /* COLLECTION(Application)       */ \
        0x09, 0x01,  /*  USAGE(Pointer)               */ \
        0xA1, 0x00,  /*  COLLECTION(Physical)         */ \
        0x05, 0x01,  /*   USAGE_PAGE(Generic Desktop) */ \
        0x09, 0x30,  /*   USAGE(X)                    */ \
        0x09, 0x31,  /*   USAGE(Y)                    */ \
        0x15, 0x81,  /*   LOGICAL_MINIMUM(-127)       */ \
        0x25, 0x7F,  /*   LOGICAL_MAXIMUM(127)        */ \
        0x75, 0x08,  /*   REPORT_SIZE(8)              */ \
        0x95, 0x02,  /*   REPORT_COUNT(2)             */ \
        0x81, 0x06,  /*   INPUT(Data,Var,Rel)         */ \
        0xC0,        /*  END_COLLECTION               */ \
        0x05, 0x09,  /*  USAGE_PAGE(Button)           */ \
        0x19, 0x01,  /*  USAGE_MINIMUM(Button 1)      */ \
        0x29, 0x03,  /*  USAGE_MAXIMUM(Button 3)      */ \
        0x15, 0x00,  /*  LOGICAL_MINUMUM(0)           */ \
        0x25, 0x03,  /*  LOGICAL_MAXIMUM(1)           */ \
        0x95, 0x03,  /*  REPORT_COUNT(3)              */ \
        0x75, 0x01,  /*  REPORT_SIZE(1)               */ \
        0x81, 0x02,  /*  INPUT(Data,Var,Abs)          */ \
        0x95, 0x01,  /*  REPORT_COUNT(1)              */ \
        0x75, 0x05,  /*  REPORT_SIZE(5)               */ \
        0x81, 0x03,  /*  INPUT(Cnst,Var,Abs)          */ \
        0xC0         /* END_COLLECTION                */ 
#endif        

#if TI_CHANGES == XA_ENABLED
/*--------------------------------------------------------------------------- 
 * HID_MAX_DESCRIPTOR_LEN constant 
 * 
 *     Defines maximum storage set aside on the host for each device's 
 *     descriptor list. This value is only used when HID_HOST is enabled. 
 */ 
#ifndef HID_MAX_DESCRIPTOR_LEN 
#define HID_MAX_DESCRIPTOR_LEN 128 
#endif 
#endif  /* TI_CHANGES == XA_ENABLED */

/*---------------------------------------------------------------------------
 * HID_BATTERY_POWER constant
 *     Indicates if the device is battery powered.  If set to TRUE, the device
 *     is battery powered.  If set to FALSE, the device has continuous power
 *     from a power supply.  The default value is FALSE.  This information is 
 *     stored in the SDP database of the HID device
 */
#ifndef HID_BATTERY_POWER
#define HID_BATTERY_POWER FALSE
#endif
      
/*---------------------------------------------------------------------------
 * HID_REMOTE_WAKE constant
 *     Indicates if the host can wake the device from suspend mode.  If
 *     this value is set to be TRUE, the host can wake the device from
 *     suspend mode by sending the appropriate control message.  If this
 *     value is set to FALSE, the host can exclude this device for the
 *     set of devices that it can wake up.  The default value is TRUE.  The
 *     ability to remotely wake is device (application) specific.  This
 *     information is stored in the SDP database of the HID device.
 */
#ifndef HID_REMOTE_WAKE
#define HID_REMOTE_WAKE TRUE
#endif
      
/*---------------------------------------------------------------------------
 * HID_SUPERVISION_TIMEOUT constant
 *     Defines the recommended supervision timeout for baseband connections.
 *     The default is the default value specified by the Bluetooth specification
 *     (0x7d00).  This information is stored in the SDP database of the HID 
 *     device.
 */
#ifndef HID_SUPERVISION_TIMEOUT
#define HID_SUPERVISION_TIMEOUT 0x7D00
#endif
      
/*---------------------------------------------------------------------------
 * HID_NORMALLY_CONNECTABLE constant
 *     Defines whether the device is normally in page scan mode.  If set to
 *     TRUE, the device is available to receive connections when there is
 *     no active connection.  The default value is TRUE.  Placing the device
 *     in the connectable mode is a function of the application.  This 
 *     information is stored in the SDP database of the HID device.
 */
#ifndef HID_NORMALLY_CONNECTABLE
#define HID_NORMALLY_CONNECTABLE TRUE
#endif
      
/*---------------------------------------------------------------------------
 * HID_BOOT_DEVICE constant
 *     Indicates whether the device supports the boot protocol.  If set to
 *     TRUE, the device supports the HID_TRANS_SET_PROTOCOL and 
 *     HID_TRANS_GET_PROTOCOL transaction.  The default value is TRUE.  Support
 *     for the boot protocol is application specific.  This information is
 *     stored in the SDP database of the HID device.
 */
#ifndef HID_BOOT_DEVICE
#define HID_BOOT_DEVICE TRUE
#endif
      
/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HidEvent type
 *
 *     All event indications and confirmations are passed to a callback
 *     function of type HidCallback. The "event" field of the "HidCallbackParms"
 *     parameter will contain one of the event types below.  The "ptrs" field
 *     will contain information pertinent to each event.
 */
typedef U8 HidEvent;

/** A remote device (host or HID device) has requested a connection.  During the
 *  processing of this event, call HID_AcceptConnection or HID_RejectConnection
 *  to indicate whether the connection should be allowed.  The "ptrs.remDev"
 *  field contains a pointer to the remote device.
 */
#define HIDEVENT_OPEN_IND                1

/** A connection is open.  This event can be received as the result of a call to 
 *  HID_AcceptConnection or HID_OpenConnection.  When this event has been 
 *  received, the channel is available for sending transactions and interrupts.  
 *  When this event is received in the callback, the "ptrs.remDev" field 
 *  contains a pointer to the remote device.
 */
#define HIDEVENT_OPEN                    2

/** The remote device (host or HID device) is closing the connection.  Once the 
 *  connection is closed, an HCEVENT_HOST_CLOSED event will be received.  When 
 *  this event is received in the callback, the "ptrs.remDev" field contains a 
 *  pointer to the remote device.
 */
#define HIDEVENT_CLOSE_IND               3

/** The connection is closed.  This can come as a result of calling 
 *  HID_CloseConnection, if the remote device has closed the connection, 
 *  or if an incoming connection is rejected.  Transactions and interrupts 
 *  cannot be sent or received in this state.  When this event is received, 
 *  the "ptrs.remDev" field contains a pointer to the remote device.
 */
#define HIDEVENT_CLOSED                  4

#if HID_HOST == XA_ENABLED
/** A query response has been received from the HID device.  This event is 
 *  received as the result of a call to HID_HostQueryDevice.  The 
 *  "ptrs.queryRsp" field contains a pointer to the query response.  This
 *  structure contains important information about the device.  It is possible
 *  that the query will fail.  The "status" field contains a value of 
 *  BT_STATUS_SUCCESS when the query is successful.
 */
#define HIDEVENT_QUERY_CNF               5
#endif

/** A transaction has been received.  The "ptrs.trans" field contains a pointer
 *  to an "HidTransaction" structure.  The "type" field of this structure
 *  defines the type of transaction that has been received.  Based on the 
 *  value of the "type" field, the "parm" field of the transaction structure
 *  will point to the parameters of the transaction.  See HidTransactionType for
 *  more information about the transaction.  If the "status" field has a value
 *  of BT_STATUS_PENDING, then more data is pending on this event and 
 *  subsequent HIDEVENT_TRANSACTION_IND events will be received.  When "status" 
 *  has a value of BT_STATUS_SUCCESS, all data from the transaction has been
 *  received.
 */
#define HIDEVENT_TRANSACTION_IND         6

/** A transaction response has been received.  The "ptrs.trans" field contains
 *  a pointer to a "HidTransaction" structure.  The "type" field of the transaction
 *  structure defines the type of transaction.  Based on the value of the "type"
 *  field, the "parm" field of the transaction structure will point to the
 *  relevant parameters of the transaction.  See HidTransactionType for
 *  more information about the transaction.  If the "status" field has a value
 *  of BT_STATUS_PENDING, then more data is pending on this event and 
 *  subsequent HIDEVENT_TRANSACTION_RSP events will be received.  When "status" 
 *  has a value of BT_STATUS_SUCCESS, all data from the transaction has been
 *  received.
 */
#define HIDEVENT_TRANSACTION_RSP         7

/** A call to a transaction API has completed.  The "ptrs.trans" field contains
 *  a pointer to the transaction that was sent.  When this event is received,
 *  the transaction structure is now available for reuse.  The "status" field
 *  indicates whether the transaction was sent successfully. The 
 *  "resultCode" field of the transaction structure is valid and contains the
 *  result of the transaction.
 */
#define HIDEVENT_TRANSACTION_COMPLETE    8

/** An interrupt was received from the remote Device.  The "ptrs.intrData" field
 *  contains the interrupt data.  Interrupt data should be handled in a timely
 *  manner, because it may be overwritten by subsequent interrupts.  If the
 *  "status" field has a value of BT_STATUS_PENDING, then more interrupt data
 *  is pending on this event and  subsequent HIDEVENT_INTERRUPT events will be
 *  received.  When "status" has a value of BT_STATUS_SUCCESS, all data from the
 *  interrupt has been received.
 */
#define HIDEVENT_INTERRUPT               9

/** A call to the HID_SendInterrupt API has been sent.  The "ptrs.intrData"
 *  field contains the interrupt data that was sent, and the structure is
 *  available for reuse.
 */
#define HIDEVENT_INTERRUPT_COMPLETE      10

#if TI_CHANGES == XA_ENABLED

/* Indicates that the BTL_HIDH_Disable operation was completed and the
 * HID context is disabled. Note that this event is different from other events,
 * since it generated in the BTL layer.
 */
#define HIDEVENT_DISABLED      			11
#endif /* TI_CHANGES == XA_ENABLED */
/* End of HidEvent */

/*---------------------------------------------------------------------------
 * HidTransactionType type
 *
 *     When a transaction is received (request or response),  a transaction
 *     structure is passed to the application to indicate the type of
 *     transaction and its parameters.  The "Type" field of the transaction
 *     structure will be set to one of the following values, and will indicate
 *     which parameters are valid.
 */
typedef U8 HidTransactionType;

/** A control request has been received.  The "parm.control" field of the
 *  "HidTransaction" structure contains the current control operation.  Control
 *  operations should be executed by the receiving device.  The only valid
 *  operation that can be received by the Host is a
 *  HID_CTRL_VIRTUAL_CABLE_UNPLUG.  All operations can be received by the 
 *  HID Device.
 */
#define HID_TRANS_CONTROL           1

/** A request for a report has been received.  The "parm.reportReq" field
 *  contains the request.  The receiving device should respond to a  request by
 *  calling HID_DeviceGetReportRsp.  When an input report is requested, the
 *  device should respond with the instantaneous state of fields in the
 *  requested input report.  When an output report is requested, the device 
 *  should respond with the last output report received on the interrupt
 *  channel.  If no output report has been received, default values should be
 *  returned. If a feature report is requested, the device should return 
 *  default values or the instantaneous state of the feature report fields.
 */
#define HID_TRANS_GET_REPORT        2

/** A report has been received.  The "parm.report" field contains the report. 
 *  Multiple HIDEVENT_TRANSACTION_IND events may be received with report
 *  data.  If the "status" field of the "HidCallbackParms" structure contains
 *  a value of BT_STATUS_PENDING, more transaction events will be received.
 *  When a value of BT_STATUS_SUCCESS is received, all report data has been
 *  received.  When all report data has been received, the receiving device
 *  should respond to this request by calling HID_DeviceSetReportRsp.
 */
#define HID_TRANS_SET_REPORT        3

/** A request for the current protocol has been received.  The receiving device
 *  should respond to this request by calling HID_DeviceGetProtocolRsp.
 */
#define HID_TRANS_GET_PROTOCOL      4

/** A set protocol command has been received.  The "parm.protocol" field
 *  contains the current protocol.  The receiving device should respond to this
 *  request by calling HID_DeviceSetProtocolRsp.  If a successful response is
 *  sent, the device should begin to use the specified protocol.
 */
#define HID_TRANS_SET_PROTOCOL      5

/** A request for the current idle rate has been received.  The  receiving
 *  device should respond to this request by calling  HID_DeviceGetIdleRsp.
 *  No parameter is specified.
 */
#define HID_TRANS_GET_IDLE_RATE     6

/** A set idle request has been received.  The "parm.idleRate" field contains
 *  the current idle rate.  The device should respond to this request by calling
 *  HID_DeviceSetIdleRsp.  If a successful response is sent, the device
 *  should begin to use the specified idle rate.
 */
#define HID_TRANS_SET_IDLE_RATE     7

/** A response to a HID_TRANS_GET_REPORT transaction has been received.  
 *  If the "resultCode" field is set to HID_RESULT_SUCCESS,  then the
 *  "parm.report" field contains a pointer to  the report data.
 *  Multiple HIDEVENT_TRANSACTION_IND events may be received with report
 *  data.  If the "status" field of the "HidCallbackParms" structure contains
 *  a value of BT_STATUS_PENDING, more transaction events will be received.
 *  When a value of BT_STATUS_SUCCESS is received, all report data has been
 *  received.  
 */
#define HID_TRANS_GET_REPORT_RSP    8

/** A response to a HID_TRANS_SET_REPORT transaction has been received.  
 *  The "resultCode" field contains the results of the transaction.
 */
#define HID_TRANS_SET_REPORT_RSP    9

/** A response to a HID_TRANS_GET_PROTOCOL transaction has been received.
 *  If the "resultCode" field is set to HID_RESULT_SUCCESS, then the
 *  "parm.protocol" field contains the current protocol.
 */
#define HID_TRANS_GET_PROTOCOL_RSP  10

/** A response to a HID_TRANS_SET_PROTOCOL transaction has been received.
 *  The "resultCode" field contains the results of the transaction.
 */
#define HID_TRANS_SET_PROTOCOL_RSP  11

/** A response to a HID_TRANS_GET_IDLE_RATE transaction has been received.
 *  If the "resultCode" is set to HID_RESULT_SUCCESS, then the
 *  "parm.idleRate" field contains the current idle rate.
 */
#define HID_TRANS_GET_IDLE_RATE_RSP      12

/** A response to a HID_TRANS_SET_IDLE_RATE transaction has been received.  
 *  The "resultCode" field contains the results of the transaction.
 */
#define HID_TRANS_SET_IDLE_RATE_RSP      13

/* End of HidTransactionType */

/*---------------------------------------------------------------------------
 * HidResultCode type
 *
 * HID transactions return a status describing the success or failure of the
 * transaction.  This type describes all the possible result codes.
 */
typedef U8 HidResultCode;

#define HID_RESULT_SUCCESS              0
#define HID_RESULT_NOT_READY            1
#define HID_RESULT_INVALID_REPORT_ID    2
#define HID_RESULT_UNSUPPORTED_REQUEST  3
#define HID_RESULT_INVALID_PARAMETER    4
#define HID_RESULT_UNKNOWN              5
#define HID_RESULT_FATAL                6

/* End of HidResultCode */

/*---------------------------------------------------------------------------
 * HidIdleRate type
 *
 * This type defines the idle rate (required by keyboards).
 */
typedef U8 HidIdleRate;

/* End of HidIdleRate */

/*---------------------------------------------------------------------------
 * HidProtocol type
 *
 * This type defines the HID protocols.
 */
typedef U8 HidProtocol;

#define HID_PROTOCOL_REPORT  1
#define HID_PROTOCOL_BOOT    0

/* End of HidProtocol */

/*---------------------------------------------------------------------------
 * HidReportType type
 *
 * This defines the HID report type.
 */
typedef U8 HidReportType;

#define HID_REPORT_OTHER    0
#define HID_REPORT_INPUT    1
#define HID_REPORT_OUTPUT   2
#define HID_REPORT_FEATURE  3

/* End of HidReportType */

/*---------------------------------------------------------------------------
 * HidControl type
 *
 * This defines the control type.
 */
typedef U8 HidControl;

#define HID_CTRL_NOP                   0
#define HID_CTRL_HARD_RESET            1
#define HID_CTRL_SOFT_RESET            2
#define HID_CTRL_SUSPEND               3
#define HID_CTRL_EXIT_SUSPEND          4
#define HID_CTRL_VIRTUAL_CABLE_UNPLUG  5

/* End of HidControl */

/*---------------------------------------------------------------------------
 * HidQueryFlags type
 *
 * These flags are used to determine which values of the SDP query response
 * contain valid data (see HidQueryRsp).  Some fields may not be supported in
 * a particular device.  If no flags are set at all, then a valid query has
 * not been made.
 */
typedef U16 HidQueryFlags;

#define SDPQ_FLAG_DEVICE_RELEASE    0x0001    /* deviceRelease is valid */
#define SDPQ_FLAG_PARSER_VERSION    0x0002    /* parserVersion is valid */
#define SDPQ_FLAG_DEVICE_SUBCLASS   0x0004    /* deviceSubclass is valid */
#define SDPQ_FLAG_COUNTRY_CODE      0x0008    /* countryCode is valid */
#define SDPQ_FLAG_VIRTUAL_CABLE     0x0010    /* virtualCable is valid */
#define SDPQ_FLAG_RECONNECT_INIT    0x0020    /* reconnect is valid */
#define SDPQ_FLAG_DESCRIPTOR_LIST   0x0040    /* descriptorLen and descriptorList
                                               * are valid.
                                               */
#define SDPQ_FLAG_SDP_DISABLE       0x0080    /* sdpDisable is valid */
#define SDPQ_FLAG_BATTERY_POWER     0x0100    /* batteryPower is valid */
#define SDPQ_FLAG_REMOTE_WAKE       0x0200    /* remoteWakeup is valid */
#define SDPQ_FLAG_PROFILE_VERSION   0x0400    /* profileVersion is valid */
#define SDPQ_FLAG_SUPERV_TIMEOUT    0x0800    /* supervTimeout is valid */
#define SDPQ_FLAG_NORM_CONNECTABLE  0x1000    /* normConnectable is valid */
#define SDPQ_FLAG_BOOT_DEVICE       0x2000    /* bootDevice is valid */

/* End of HidQueryFlags */

/* HID roles */
typedef U8 HidRole;

#define HID_ROLE_DEVICE  1
#define HID_ROLE_HOST    2

/* Foreward References */
typedef struct _HidCallbackParms HidCallbackParms;
typedef struct _HidChannel HidChannel;
typedef struct _HidReport HidReport;
typedef struct _HidReportReq HidReportReq;
typedef struct _HidTransaction HidTransaction;
typedef struct _HidInterrupt HidInterrupt;

/*---------------------------------------------------------------------------
 * HidCallback type
 *
 *     Represents a callback function called by HID to indicate events and 
 *     data to the application. The event is targeted to the HID connection
 *     identified by the "Channel" parameter. Information about the 
 *     event is contained in the "Parms" parameter.
 */
typedef void (*HidCallback)(HidChannel *Channel, HidCallbackParms *Parms);
/* End of HidCallback */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

#if HID_HOST == XA_ENABLED
/*---------------------------------------------------------------------------
 * HidQueryRsp structure
 *
 *     This structure contains SDP query data from the device.  This structure
 *     only exists if HID_HOST is defined to be XA_ENABLE.  The Host has the
 *     ability to query a device's SDP entry for important HID information.
 *     During the query response, that data is passed to the application in
 *     this structure.  The data remains valid until the channel is deregistred.
 */
typedef struct _HidQueryRsp {
    HidQueryFlags     queryFlags;       /* Defines which query field contains
                                         * valid data.
                                         */
    U16               deviceRelease;    /* Vendor specified device release 
                                         * version.
                                         */
    U16               parserVersion;    /* HID parser version for which this
                                         * device is designed.
                                         */
    U8                deviceSubclass;   /* Device subclass (minor Class of 
                                         * Device).
                                         */
    U8                countryCode;      /* Country Code
                                         */
    U8                virtualCable;     /* Virtual Cable relationship is 
                                         * supported.
                                         */
    U8                reconnect;        /* Device initiates reconnect.
                                         */
    U8                sdpDisable;       /* When TRUE, the device cannot accept
                                         * an SDP query when the control/interrupt
                                         * channels are connected.
                                         */
    U8                batteryPower;     /* The device runs on battery power.
                                         */
    U8                remoteWakeup;     /* The device can be awakened remotely.
                                         */
    U16               profileVersion;   /* Version of the HID profile.
                                         */
    U16               supervTimeout;    /* Suggested supervision timeout value
                                         * for LMP connections.
                                         */
    U8                normConnectable;  /* The device is connectable when no
                                         * connection exists.
                                         */
    U8                bootDevice;       /* Boot protocol support is provided.
                                         */
    U16               descriptorLen;    /* Length of the HID descriptor list.
                                         */
#if TI_CHANGES == XA_ENABLED	

     U8                descriptorList[HID_MAX_DESCRIPTOR_LEN]; 
    
 #else  /* TI_CHANGES == XA_ENSABLED */ 
     U8               *descriptorList;   /* A list of HID descriptors (report or
                                         * physical) associated with the device.
                                         * Each element of the list is an SDP 
                                         * data element sequence, and therefore
                                         * has a header of two bytes (0x35, len)
                                         * which precedes each descriptor.
                                         */
#endif /* TI_CHANGES == XA_ENSABLED */ 
} HidQueryRsp;
#endif

/*---------------------------------------------------------------------------
 * HidCallbackParms structure
 *
 *     Describes a callback event and any data that relates to the event.
 *     Various fields in the structure may be valid or not, depending
 *     on the "event" field.
 */
struct _HidCallbackParms {
    HidEvent            event;        /* Type of the HID transport event  */     
                                                                           
    BtStatus            status;       /* Communication status or error information */

    U16                 len;          /* Length of the object pointed to 
                                       * by "ptrs"
                                       */

    union {
        /* During an HIDEVENT_HOST_OPEN_IND, HIDEVENT_HOST_OPEN,
         * HIDEVENT_DEVICE_OPEN_IND, HIDEVENT_DEVICE_OPEN, HCEVENT_HOST_CLOSED, 
         * or HCEVENT_DEVICE_CLOSED event, contains the remote device structure.
         */
        BtRemoteDevice *remDev;

        /* During an HIDEVENT_TRANSACTION and HIDEVENT_TRANSACTION_RSP
         * events, contains the transaction type and parameters.
         */
        HidTransaction *trans;

        /* During HIDEVENT_INTERRUPT and HIDEVENT_INTERRUPT_RSP
         * events, contains the interrupt data.
         */
        HidInterrupt   *intr;

        /* During HIDEVENT_QUERY_CNF, contains the SDP query response data.
         */
#if HID_HOST == XA_ENABLED
        HidQueryRsp    *queryRsp;
#endif
    } ptrs;
};

/*---------------------------------------------------------------------------
 * HidChannel structure
 *
 *     This structure is used to identify an HID connection.
 */
struct _HidChannel {
    
    /* Used internally by HID */
    ListEntry         node;

    /* === Internal use only === */

    /* State variables */
    HidCallback       callback;
    U8                state;
    U8                flags;
    U8                conCount;

    /* L2CAP variables */
    U16               ctrlCid;
    U16               intrCid;

    /* Queue of transactions and interrupts */
    ListEntry         transQueue;
    ListEntry         intrQueue;

#if HID_DEVICE == XA_ENABLED
    /* Transaction for responding with an error */
    BtPacket          rspPacket;
#endif
    /* Role of this end of the HID connection */
    HidRole           hidRole;

#if HID_HOST == XA_ENABLED
    /* SDP query token */
    SdpQueryToken     sdpQueryToken;
    HidQueryRsp       queryRsp;
#endif

    CmgrHandler       cmgrHandler;
};

/*---------------------------------------------------------------------------
 * HidReport structure
 *
 *     This structure is used to identify an HID report.  The "reportType" 
 *     field describes the type of report pointed to by the "data" field.
 */
struct _HidReport {

    HidReportType reportType;    /* Report type (input, output, or feature) */
    U8           *data;          /* Report data */
    U16           dataLen;       /* Length of the report data */
};

/*---------------------------------------------------------------------------
 * HidReportReq structure
 *
 *     This structure is used to identify an HID report request.  The 
 *     "reportType" field describes the type of report pointed to by the "data" 
 *     field.
 */
struct _HidReportReq {

    HidReportType reportType;    /* Report type (input, output, or feature) */
    BOOL          useId;         /* Set to TRUE if reportId should be used */
    U8            reportId;      /* The report ID (optional) */
    U16           bufferSize;    /* The amount of report data to be returned 
                                  * (optional). Set this value to 0 if the 
                                  * entire report should be returned. Note that
                                  * this value must be increased by 1 to 
                                  * receive the Report ID in Boot Mode.
                                  */
};

/*---------------------------------------------------------------------------
 * HidTransaction structure
 *
 *     This structure is used to identify an HID transaction.
 */
struct _HidTransaction {
    /* Used internally by HID */
    ListEntry           node;   

    /* Contains the HID Result Code */
    HidResultCode   resultCode;

    union {
        /* Contains report data */
        HidReport      *report;

        /* Contains a report data request */
        HidReportReq   *reportReq;

        /* Contains the current protocol */
        HidProtocol     protocol;

        /* Contains the idle rate */
        HidIdleRate     idleRate;

        /* Contains the control operation */
        HidControl      control;
    } parm;

    /* === Internal use only === */
    HidTransactionType  type;
    U8                  flags;
    U16                 offset;
};

/*---------------------------------------------------------------------------
 * HidInterrupt structure
 *
 *     This structure is used to identify an HID interrupt.
 */
struct _HidInterrupt {
    /* Used internally by HID */
    ListEntry     node; 

    /* Contains a pointer to interrupt data */
    U8           *data;

    /* Contains the length of interrupt data */
    U16           dataLen;

    /* Report type (input, output, or feature) */
    HidReportType reportType;    

    /* === Internal use only === */
    U8            flags;
    U16           offset;
};

/* Functions used by macros below */
BtStatus HID_Register(HidChannel *Channel, HidCallback Callback, HidRole Role);
BtStatus HID_Deregister(HidChannel *Channel);
BtStatus HID_RespondOpenInd(HidChannel *Channel, BOOL flag);
BtStatus HID_SendTransaction(HidChannel *Channel, HidTransactionType TranType, 
                             HidTransaction *Trans);

/****************************************************************************
 *
 * Section: Common Functions
 *
 ****************************************************************************
 *
 * These functions are used by either the HID Device or HID Host.
 *
 */

/*---------------------------------------------------------------------------
 * HID_Init()
 *
 *     Initialize HID.  This function must be called before calling any other
 *     HID function.  Currently, this function always returns BT_STATUS_SUCCESS,
 *     but it is good to check the return code as this may change in future
 *     releases.
 *            
 * Parameters:
 *     void
 *
 * Returns:
 *     TRUE - Initialization successful.
 *     FALSE - Initialization failed.
 */
BOOL HID_Init(void);

/*---------------------------------------------------------------------------
 * HID_OpenConnection()
 *
 *     Attempts to establish a connection with a remote device (Host or HID
 *     Device).
 * 
 * Parameters:
 *     Channel - Identifies the Channel for this action.
 *
 *     Addr - Bluetooth device address of the remote device.
 *
 * Returns:
 *     BT_STATUS_FAILED - The request was invalid.
 *
 *     BT_STATUS_PENDING - The request to open the connection was sent.
 *         If the request is accepted by the remote device, a HIDEVENT_OPEN
 *         event will be sent to the application. If the connection is 
 *         rejected, a HIDEVENT_CLOSED event will be sent to the application.
 *
 *     BT_STATUS_BUSY - The connection is open or in the process of opening.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_OpenConnection(HidChannel *Channel, BD_ADDR *Addr);

/*---------------------------------------------------------------------------
 * HID_AcceptConnection()
 *
 *     Accepts an incoming connection in response to an HIDEVENT_OPEN_IND 
 *     event.  This event occurs when a remote device (Host or HID Device) 
 *     attempts to connect to a registered Host or HID Device. Either this 
 *     function or HID_RejectConnection must be used to respond to the 
 *     connection request.
 *
 * Parameters:
 *     Channel - Identifies the channel that is accepting the connection.  This 
 *         channel is provided to the callback function as a parameter during 
 *         the HIDEVENT_OPEN_IND event.
 *
 * Returns:
 *     BT_STATUS_FAILED - The specified channel did not have a pending 
 *         connection request.
 *
 *     BT_STATUS_PENDING - The accept message will be sent. The application
 *         will receive an HIDEVENT_OPEN when the accept message has been 
 *         sent and the channel is open.
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_AcceptConnection(HidChannel *Channel);
#define HID_AcceptConnection(s) HID_RespondOpenInd(s, TRUE)

/*---------------------------------------------------------------------------
 * HID_RejectConnection()
 *
 *     Rejects an incoming connection in response to an HIDEVENT_OPEN_IND 
 *     event.  This event occurs when a remote device (Host or HID Device) 
 *     attempts to connect to a registered Host or HID Device. Either this 
 *     function or HID_AcceptConnection must be used to respond to the 
 *     connection request.
 *
 * Parameters:
 *     Channel - Identifies the channel to be rejected. This channel is
 *         provided to the callback function as a parameter during the
 *         HIDEVENT_OPEN_IND event.
 *
 * Returns:
 *     BT_STATUS_FAILED - The specified channel did not have a pending
 *         connection request.
 *
 *     BT_STATUS_PENDING - The rejection message has been sent. The application
 *         will receive an HIDEVENT_CLOSED event when the rejection is 
 *         complete.
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_RejectConnection(HidChannel *Channel);
#define HID_RejectConnection(s) HID_RespondOpenInd(s, FALSE)

/*---------------------------------------------------------------------------
 * HID_SendControl()
 *
 *     Sends a control operation to the remote device (Host or HID Device).  The
 *     "parm.control" field of the "Trans" parameter should be initialized with 
 *     the appropriate control operation. A HID device can only send the 
 *     HID_CTRL_VIRTUAL_CABLE_UNPLUG control operation.  A Host can send any 
 *     control operation.
 *
 * Parameters:
 *     Channel - Identifies the channel on which to send the request.
 *
 *     Trans - A pointer to the transaction, which describes the control
 *         operation.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request has been queued. If sent successfully, 
 *         an HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of 
 *         BT_STATUS_SUCCESS.  If the transmission fails, the same event will be 
 *         received with a status specifying the reason.  The memory pointed
 *         to by the Trans parameter must not be modified until the 
 *         transaction is complete.
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_SendControl(HidChannel *Channel, HidTransaction *Trans);
#define HID_SendControl(ch, trans) HID_SendTransaction(ch, HID_TRANS_CONTROL, trans)

/*---------------------------------------------------------------------------
 * HID_SendInterrupt()
 *
 *     Sends an interrupt (report) to the remote device (Host or HID Device).  
 *     The Interrupt parameter should be initialized with the appropriate 
 *     information.
 *
 * Parameters:
 *     Channel - Identifies the channel on which to send the interrupt.
 *
 *     Interrupt - A pointer to the interrupt structure which describes the 
 *         interrupt data.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request has been queued. If sent successfully, 
 *         an HIDEVENT_INTERRUPT_COMPLETE event will arrive with a "status" of 
 *         BT_STATUS_SUCCESS.  If the transmission fails, the same event will be 
 *         received with a status specifying the reason.  The memory pointed
 *         to by the Interrupt parameter must not be modified until the 
 *         transaction is complete.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_USE - The channel is already in use (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_SendInterrupt(HidChannel *Channel, HidInterrupt *Interrupt);

/*---------------------------------------------------------------------------
 * HID_CloseConnection()
 *
 *     Closes an HID connection between two devices.  When the connection
 *     is closed, the application will receive an HIDEVENT_CLOSED event.
 *
 *     If there are outstanding transactions or interrupts when a connection is 
 *     closed, an event will be received by the application for each one.  The 
 *     "status" field for these events will be set to BT_STATUS_NO_CONNECTION.
 *
 * Parameters:
 *     Channel - Identifies the channel connection to be closed. The 
 *         HIDEVENT_CLOSED event indicates that the connection is closed 
 *         and a new connection may be attempted.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request to close the connection will be sent.
 *         The application will receive an HIDEVENT_CLOSED event when the
 *         connection is closed.
 *
 *     BT_STATUS_FAILED - The channel is invalid or could not be
 *         disconnected.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_NO_CONNECTION - No ACL connection exists on this channel.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only). 
 *
 *     BT_STATUS_IN_PROGRESS - The channel is already disconnecting
 *         (XA_ERROR_CHECK Only).    
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_CloseConnection(HidChannel *Channel);


/****************************************************************************
 *
 * Section: HID Host Functions
 *
 ****************************************************************************
 *
 * These functions are used only HID Host implementations. HID_HOST must
 * be defined to XA_ENABLED to access them.
 * 
 */


#if HID_HOST == XA_ENABLED
/*---------------------------------------------------------------------------
 * HID_RegisterHost()
 *
 *     Registers a host for receiving HID connections.
 *
 * Parameters:
 *     Host - Host to register.
 *
 *     Callback - Callback function for receiving events related to the
 *         specified host.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The host was successfully registered.
 *
 *     BT_STATUS_FAILED - The host could not be registered, probably 
 *         because the channel is already in use.
 *
 *     BT_STATUS_BUSY - The protocol (PSM) is already in use.
 *
 *     BT_STATUS_NO_RESOURCES - Could not register the PSM with L2CAP.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_USE - The channel structure is already in use 
 *         (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_RegisterHost(HidChannel *Channel, HidCallback Callback);
#define HID_RegisterHost(ch, cb) HID_Register(ch, cb, HID_ROLE_HOST)

/*---------------------------------------------------------------------------
 * HID_DeregisterHost()
 *
 *     Deregisters an HID host channel.
 *
 * Parameters:
 *     Channel - Channel to deregister.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The host was successfully deregistered.
 *
 *     BT_STATUS_BUSY - The host has an open channel. The channel
 *         must be successfully closed with HID_CloseConnection before 
 *         calling this function.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 * 
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_DeregisterHost(HidChannel *Channel);
#define HID_DeregisterHost(ch) HID_Deregister(ch)

/*---------------------------------------------------------------------------
 * HID_HostQueryDevice()
 *
 *     Queries the Device for its SDP database entry.  The SDP database
 *     contains information about the device's capabilities.  The query
 *     information will be returned to the application with a
 *     HIDEVENT_QUERY_CNF event.  The query data is parsed and placed
 *     in memory allocated statically.  The application can save the
 *     pointer to the SDP data and reference it as long as the channel
 *     is still registered.  The data is no longer valid when the channel is
 *     deregistred.
 *
 *     It is possible that the query will fail, because some devices limited 
 *     in memory will not allow an SDP query while the HID channel is open.  
 *     It is suggested that the host query the device before opening a 
 *     connection.  After the callback, the "sdpDisable" field of the 
 *     "ptrs.queryRsp" structure tells whether sdp queries are enabled or 
 *     disabled when a HID channel is already open.
 *
 * Parameters:
 *     Channel - Identifies the Channel for this action.
 *
 *     Addr - Bluetooth device address of the remote device.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request to query the SDP entry was sent.
 *         When a response is received from the device, an HIDEVENT_QUERY_CNF
 *         event will be received.
 *
 *     BT_STATUS_BUSY - The connection is already in the process of opening.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (SDP, L2CAP or Management Entity).
 */
BtStatus HID_HostQueryDevice(HidChannel *Channel, BD_ADDR *Addr);

/*---------------------------------------------------------------------------
 * HID_HostGetReport()
 *
 *     Sends an report request to the HID device.  The "parm.reportReq" field
 *     of the "Trans" parameter should be initialized with the appropriate 
 *     information.  Requesting an input report causes the device to respond
 *     with the instantaneous state of fields in the requested input report.
 *     Requesting an output report causes the device to respond with the last
 *     output report received on the interrupt channel.  If no output report
 *     has been received, default values will be returned. Requesting a feature
 *     report causes the device to return the default values or instantaneous
 *     state of the feature report fields.
 *
 * Parameters:
 *     Channel - Identifies the channel on which to send the request.
 *
 *     Trans - A pointer to the transaction structure which describes the
 *         request.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request has been queued. If sent successfully, 
 *         at lease one HIDEVENT_TRANSACTION_RSP event will be receive with
 *         report data.  A HIDEVENT_TRANSACTION_COMPLETE event will arrive with
 *         a "status" of BT_STATUS_SUCCESS upon successful completion.
 *         If the transmission fails, the same event will be received with a
 *         status specifying the reason.  The memory pointed to by the Trans
 *         parameter must not be modified until the transaction is complete.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_HostGetReport(HidChannel *Channel, HidTransaction *Trans);
#define HID_HostGetReport(ch, trans) HID_SendTransaction(ch, HID_TRANS_GET_REPORT, trans)

/*---------------------------------------------------------------------------
 * HID_HostSetReport()
 *
 *     Sends a report to the HID device.  The "parm.report" field of the "Trans"
 *     parameter should be initialized with the appropriate report information.
 *
 * Parameters:
 *     Channel - Identifies the channel on which to send the request.
 *
 *     Trans - A pointer to the transaction, which describes the report.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request has been queued. If sent successfully, 
 *         an HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of 
 *         BT_STATUS_SUCCESS.  If the transmission fails, the same event will be 
 *         received with a status specifying the reason.  The memory pointed to
 *         by the Trans parameter must not be modified until the transaction is
 *         complete.
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_HostSetReport(HidChannel *Channel, HidTransaction *Trans);
#define HID_HostSetReport(ch, trans) HID_SendTransaction(ch, HID_TRANS_SET_REPORT, trans)

/*---------------------------------------------------------------------------
 * HID_HostGetProtocol()
 *
 *     Sends an protocol request to the HID device.  It is not necessary to
 *     initialize the "Trans" parameter.
 *
 * Parameters:
 *     Channel - Identifies the channel on which to send the request.
 *
 *     Trans - A pointer to the transaction structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request has been queued. If sent successfully, 
 *         an HIDEVENT_TRANSACTION_RSP event will be receive with protocol data.
 *         An HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of 
 *         BT_STATUS_SUCCESS upon successful complete.  If the transaction
 *         fails, the same event will be received with a status specifying the
 *         reason.  The memory pointed to by the Trans parameter must not be
 *         modified until the transaction is complete.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_HostGetProtocol(HidChannel *Channel, HidTransaction *Trans);
#define HID_HostGetProtocol(ch, trans) HID_SendTransaction(ch, HID_TRANS_GET_PROTOCOL, trans)

/*---------------------------------------------------------------------------
 * HID_HostSetProtocol()
 *
 *     Sends the current protocol to the HID device.  The "parm.protocol" field
 *     of the "Trans" parameter should be initialized with the appropriate 
 *     protocol.
 *
 * Parameters:
 *     Channel - Identifies the channel on which to send the request.
 *
 *     Trans - A pointer to the transaction, which describes the protocol.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request has been queued. If sent successfully, 
 *         an HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of 
 *         BT_STATUS_SUCCESS.  If the transmission fails, the same event will be 
 *         received with a status specifying the reason.  The memory pointed to
 *         by the Trans parameter must not be modified until the transaction is
 *         complete.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_HostSetProtocol(HidChannel *Channel, HidTransaction *Trans);
#define HID_HostSetProtocol(ch, trans) HID_SendTransaction(ch, HID_TRANS_SET_PROTOCOL, trans)

/*---------------------------------------------------------------------------
 * HID_HostGetIdleRate()
 *
 *     Sends an idle rate status request to the HID device.  It is not necessary
 *     to initialize the "Trans" parameter.
 *
 * Parameters:
 *     Channel - Identifies the channel on which to send the request.
 *
 *     Trans - A pointer to the transaction structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request has been queued. If sent successfully, 
 *         an HIDEVENT_TRANSACTION_RSP event will be receive with "idleRate" data.
 *         An HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of 
 *         BT_STATUS_SUCCESS upon successful complete.  If the transaction
 *         fails, the same event will be received with a status specifying the
 *         reason.  The memory pointed to by the Trans parameter must not be
 *         modified until the transaction is complete.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_HostGetIdleRate(HidChannel *Channel, HidTransaction *Trans);
#define HID_HostGetIdleRate(ch, trans) HID_SendTransaction(ch, HID_TRANS_GET_IDLE_RATE, trans)

/*---------------------------------------------------------------------------
 * HID_HostSetIdleRate()
 *
 *     Sends the idle rate to the HID device.  The "parm.idleRate" field of the
 *     "Trans" parameter should be initialized with the appropriate idle rate.
 *
 * Parameters:
 *     Channel - Identifies the channel on which to send the request.
 *
 *     Trans - A pointer to the transaction, which describes idle rate.
 *
 * Returns:
 *     BT_STATUS_PENDING - The idle rate has been queued. If sent successfully, 
 *         an HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of 
 *         BT_STATUS_SUCCESS.  If the transmission fails, the same event will be 
 *         received with a status specifying the reason.  The memory pointed to
 *         by the Trans parameter must not be modified until the transaction is
 *         complete.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_HostSetIdleRate(HidChannel *Channel, HidTransaction *Trans);
#define HID_HostSetIdleRate(ch, trans) HID_SendTransaction(ch, HID_TRANS_SET_IDLE_RATE, trans)
#endif

/****************************************************************************
 *
 * Section: HID Device Functions
 *
 ****************************************************************************
 *
 * These functions are used only HID Device implementations. HID_DEVICE must
 * be defined to XA_ENABLED to access them.
 * 
 */

#if HID_DEVICE == XA_ENABLED
/*---------------------------------------------------------------------------
 * HID_RegisterDevice()
 *
 *     Registers a device for receiving HID connections.
 *
 * Parameters:
 *     Channel - Channel on which the device will receive connections.
 *
 *     Callback - Callback function for receiving events related to the
 *         specified device.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The device was successfully registered.
 *
 *     BT_STATUS_FAILED - The device could not be registered, probably 
 *         because the channel is already in use.
 *
 *     BT_STATUS_BUSY - The protocol (PSM) is already in use.
 *
 *     BT_STATUS_NO_RESOURCES - Could not register the PSM with L2CAP.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_USE - The channel structure is already in use 
 *         (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_RegisterDevice(HidChannel *Channel, HidCallback Callback);
#define HID_RegisterDevice(ch, cb) HID_Register(ch, cb, HID_ROLE_DEVICE)

/*---------------------------------------------------------------------------
 * HID_DeregisterDevice()
 *
 *     Deregisters an HID device channel.
 *
 * Parameters:
 *     Channel - Channel to deregister.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The device was successfully deregistered.
 *
 *     BT_STATUS_BUSY - The device has an open channel. The channel
 *         must be successfully closed with HID_CloseConnection before 
 *         calling this function.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 * 
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_DeregisterDevice(HidChannel *Channel);
#define HID_DeregisterDevice(ch) HID_Deregister(ch)

/*---------------------------------------------------------------------------
 * HID_DeviceSetReportRsp()
 *
 *     Sends a confirmation of a report to the HID host.  This function is
 *     called in response to an HIDEVENT_DEVICE_SET_REPORT event.  The 
 *     "resultCode" field of the "Trans" parameter should be initialized 
 *     with the appropriate response code. 
 *
 * Parameters:
 *     Channel - Identifies the channel on which to send the response.
 *
 *     Trans - A pointer to the transaction, which describes the result code.
 *
 * Returns:
 *     BT_STATUS_PENDING - The response has been queued.  
 *         A HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of
 *         BT_STATUS_SUCCESS upon successful transmission.  If the transmission 
 *         fails, the same event will be received with a status specifying the
 *         reason.  The memory pointed to by the Trans parameter must not be
 *         modified until the transaction is complete.
 *
 *     BT_STATUS_PENDING - The response has been queued. If sent successfully, 
 *         an HIDEVENT_SET_REPORT_RSP event will arrive with a "status" of 
 *         BT_STATUS_SUCCESS.  If the transmission fails, the same event will be 
 *         received with a status specifying the reason.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_DeviceSetReportRsp(HidChannel *Channel, HidTransaction *Trans);
#define HID_DeviceSetReportRsp(ch, trans) HID_SendTransaction(ch, HID_TRANS_SET_REPORT_RSP, trans)

/*---------------------------------------------------------------------------
 * HID_DeviceGetReportRsp()
 *
 *     Sends a report to the HID host.  This function is called in response to
 *     the HIDEVENT_DEVICE_GET_REPORT event.  The "resultCode"
 *     field of the "Trans" parameter should be initialized with the appropriate
 *     response code.  If "resultCode" is set to HID_RESULT_SUCCESS, then
 *     the "parm.report" field of the "Trans" parameter should be initialized 
 *     with the appropriate report data. 
 *
 * Parameters:
 *     Channel - Identifies the channel on which to send the report.
 *
 *     Trans - A pointer to the transaction, which describes the report.
 *
 * Returns:
 *     BT_STATUS_PENDING - The response has been queued.  
 *         A HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of
 *         BT_STATUS_SUCCESS upon successful transmission.  If the transmission 
 *         fails, the same event will be received with a status specifying the
 *         reason.  The memory pointed to by the Trans parameter must not be
 *         modified until the transaction is complete.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_DeviceGetReportRsp(HidChannel *Channel, HidTransaction *Trans);
#define HID_DeviceGetReportRsp(ch, trans) HID_SendTransaction(ch, HID_TRANS_GET_REPORT_RSP, trans)

/*---------------------------------------------------------------------------
 * HID_DeviceSetProtocolRsp()
 *
 *     Sends a confirmation of the protocol to the HID host.  This call is made
 *     in response to the HIDEVENT_DEVICE_SET_PROTOCOL event.  The 
 *     "resultCode" field of the "Trans" parameter should be initialized 
 *     with the appropriate response code.
 *
 * Parameters:
 *     Channel - Identifies the channel on which to send the response.
 *
 *     Trans - A pointer to the transaction, which describes the result code.
 *
 * Returns:
 *     BT_STATUS_PENDING - The response has been queued.  
 *         A HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of
 *         BT_STATUS_SUCCESS upon successful transmission.  If the transmission 
 *         fails, the same event will be received with a status specifying the
 *         reason.  The memory pointed to by the Trans parameter must not be
 *         modified until the transaction is complete.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_DeviceSetProtocolRsp(HidChannel *Channel, HidTransaction *Trans);
#define HID_DeviceSetProtocolRsp(ch, trans) HID_SendTransaction(ch, HID_TRANS_SET_PROTOCOL_RSP, trans)

/*---------------------------------------------------------------------------
 * HID_DeviceGetProtocolRsp()
 *
 *     Sends a protocol response to the HID host.  This function is called in 
 *     response to the HIDEVENT_HOST_GET_PROTOCOL event.   The "resultCode"
 *     field of the "Trans" parameter should be initialized with the appropriate
 *     response code.  If "resultCode" is set to HID_RESULT_SUCCESS, then
 *     the "parm.protocol" field of the "Trans" parameter should be initialized
 *     with the appropriate protocol. 
 *
 * Parameters:
 *     Channel - Identifies the channel on which to send the response.
 *
 *     Trans - A pointer to the transaction, which describes the protocol.
 *
 * Returns:
 *     BT_STATUS_PENDING - The response has been queued.  
 *         A HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of
 *         BT_STATUS_SUCCESS upon successful transmission.  If the transmission 
 *         fails, the same event will be received with a status specifying the
 *         reason.  The memory pointed to by the Trans parameter must not be
 *         modified until the transaction is complete.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_DeviceGetProtocolRsp(HidChannel *Channel, HidTransaction *Trans);
#define HID_DeviceGetProtocolRsp(ch, trans) HID_SendTransaction(ch, HID_TRANS_GET_PROTOCOL_RSP, trans)

/*---------------------------------------------------------------------------
 * HID_DeviceSetIdleRsp()
 *
 *     Sends a confirmation of the idle rate to the HID host.  This call is made
 *     in response to the HIDEVENT_DEVICE_SET_IDLE_RATE event.  The
 *     "resultCode" field of the "Trans" parameter should be initialized
 *     with the appropriate response code. 
 *
 * Parameters:
 *     Channel - Identifies the channel on which to send the response.
 *
 *     Trans - A pointer to the transaction, which describes the result code.
 *
 * Returns:
 *     BT_STATUS_PENDING - The response has been queued.  
 *         A HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of
 *         BT_STATUS_SUCCESS upon successful transmission.  If the transmission 
 *         fails, the same event will be received with a status specifying the
 *         reason.  The memory pointed to by the Trans parameter must not be
 *         modified until the transaction is complete.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_DeviceSetIdleRsp(HidChannel *Channel, HidTransaction *Trans);
#define HID_DeviceSetIdleRsp(ch, trans) HID_SendTransaction(ch, HID_TRANS_SET_IDLE_RATE_RSP, trans)

/*---------------------------------------------------------------------------
 * HID_DeviceGetIdleRsp()
 *
 *     Sends the idle rate to the HID host.  This function is called in response 
 *     to the HIDEVENT_HOST_GET_IDLE_RATE event.  The "resultCode" field of
 *     the "Trans" parameter should be initialized with the appropriate response
 *     code.  If "resultCode" is set to HID_RESULT_SUCCESS, then the
 *     "parm.idleRate" field of the "Trans" parameter should be initialized with
 *     the appropriate information.  
 *
 * Parameters:
 *     Channel - Identifies the channel on which to send the response.
 *
 *     Trans - A pointer to the transaction, which describes the idle rate.
 *
 * Returns:
 *     BT_STATUS_PENDING - The response has been queued.  
 *         A HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of
 *         BT_STATUS_SUCCESS upon successful transmission.  If the transmission 
 *         fails, the same event will be received with a status specifying the
 *         reason.  The memory pointed to by the Trans parameter must not be
 *         modified until the transaction is complete.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus HID_DeviceGetIdleRsp(HidChannel *Channel, HidTransaction *Trans);
#define HID_DeviceGetIdleRsp(ch, trans) HID_SendTransaction(ch, HID_TRANS_GET_IDLE_RATE_RSP, trans)
#endif

/*---------------------------------------------------------------------------
 * HID_IsConnected()
 *     
 *     Returns TRUE if the channel is connected, false if it is not;
 *
 * Parameters:
 *     Channel - Identifies the channel.
 *
 * Returns:
 *     TRUE - The channel is connected.
 *
 *     FALSE - The channel is not connected.
 */
BOOL HID_IsConnected(HidChannel *Channel);

#endif /* __HID_H */

