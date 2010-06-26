/*****************************************************************************
 * File:
 *     $Workfile:avrcp.h$ for iAnywhere AV SDK, version 1.4
 *     $Revision:59$
 *
 * Description: This file contains public definitions for the Audio/Video
 *     Remote Control Profile (AVRCP).
 *             
 * Created:     Nov 19, 2003
 *
 * Copyright 2003 - 2005 Extended Systems, Inc.
  * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __AVRCP_H_
#define __AVRCP_H_

#include "avctp.h"
#include "conmgr.h"

/*---------------------------------------------------------------------------
 * AVRCP API layer
 *
 *     The Audio/Video Remote Control Profile (AVRCP) defines procedures for
 *     exchanging 1394 Trade Association AV/C commands between Bluetooth
 *     enabled Audio/Video devices. Internally, AVRCP uses the Audio/Video
 *     Control Transport Protocol (AVCTP) to exchange messages.
 *
 *     This API is designed to support AV remote control applications using
 *     the iAnywhere Blue SDK core protocol stack.  It provides an API
 *     for connection management and message handling.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AVRCP_PANEL_SUBUNIT constant
 *
 *     When set to XA_ENABLED, the AVRCP profile adds handling for
 *     panel subunit commands. Most AVRCP implementations use this subset
 *     of commands.
 *
 *     For targets, the application will receive AVRCP_EVENT_PANEL_PRESS,
 *     _HOLD, and _RELEASE events instead of the AVRCP_EVENT_COMMAND event
 *     for all panel subunit AVRCP_OPCODE_PASS_THROUGH commands
 *     except AVRCP_POP_VENDOR_UNIQUE. In addition, UNIT_INFO and
 *     SUBUNIT_INFO commands will be handled automatically, should they
 *     occur. All other commands will appear in ordinary AVRCP_EVENT_COMMAND
 *     events; responses for these commands MUST be sent.
 *
 *     For controllers, the application may invoke the AVRCP_SetPanelKey()
 *     API, and will receive AVRCP_EVENT_PANEL_CNF notifications to indicate
 *     the target's responses.
 */
#ifndef AVRCP_PANEL_SUBUNIT
#define AVRCP_PANEL_SUBUNIT XA_ENABLED
#endif /* AVRCP_PANEL_SUBUNIT */


/*---------------------------------------------------------------------------
 * AVRCP_SUPPORTED_FEATURES constant
 *
 *     This constant must be set to indicate which AVRCP commands are
 *     supported. By default, only AVRCP_FEATURES_CATEGORY_1 is set,
 *     corresponding to a Player/Recorder device which must support
 *     the "play" and "stop" panel subunit commands.
 *
 *     Any number of AvrcpFeatures bits may be OR'ed together into this
 *     constant.
 */
#ifndef AVRCP_SUPPORTED_FEATURES
#define AVRCP_SUPPORTED_FEATURES (AVRCP_FEATURES_CATEGORY_1)
#endif /* AVRCP_SUPPORTED_FEATURES */


/*---------------------------------------------------------------------------
 * AVRCP_PANEL_COMPANY_ID constant
 *
 *     Defines the company ID used in UNIT INFO commands received by a
 *     target when AVRCP_PANEL_SUBUNIT is set to XA_ENABLED. This value
 *     may be left at the default or replaced with a value assigned
 *     by the IEEE Registration Authority Committee as defined in the 1394
 *     AV/C Digital Interface Command Set, version 1.1, section 9.12.
 *
 *     This 24-bit value is expressed in a string as a three-byte value with
 *     most significant byte first.
 */
#define AVRCP_PANEL_COMPANY_ID "\xFF\xFF\xFF"


/*---------------------------------------------------------------------------
 * AVRCP_SUBUNIT_OP_QUEUE_MAX constant
 *
 *     Defines the length of the internal operation queue used on the
 *     controller side by AVRCP_SetPanelKey. To buffer n keystrokes,
 *     set this constant to (n*2)+1. Attempts to add keystrokes beyond
 *     the queue size with AVRCP_SetPanelKey() will fail.
 */
#ifndef AVRCP_SUBUNIT_OP_QUEUE_MAX
#define AVRCP_SUBUNIT_OP_QUEUE_MAX 15

#if AVRCP_SUBUNIT_OP_QUEUE_MAX > 255
#error AVRCP_SUBUNIT_OP_QUEUE_MAX must be no greater than 255.
#endif /* AVRCP_SUBUNIT_OP_QUEUE_MAX > 255 */

#endif /* AVRCP_SUBUNIT_OP_QUEUE_MAX */

/*---------------------------------------------------------------------------
 * AVRCP_METADATA_TARGET constant
 *
 *     When set to XA_ENABLED, the AVRCP profile adds handling for
 *     the Metadata feature of AVRCP for the target device.  This feature is
 *     used to exchanged media information with the remote device.
 *
 *     The application will receive AVRCP_EVENT_MDA_INFO events for certain
 *     commands sent by the controller.  Most commands will not generate
 *     events, but will be responded to automatically, based on the current
 *     media and player data that is set through API calls.  Several interfaces
 *     exist for setting the data that will be sent in the responses.
 */
#ifndef AVRCP_METADATA_TARGET
#define AVRCP_METADATA_TARGET XA_ENABLED
#endif /* AVRCP_METADATA_TARGET */

/*---------------------------------------------------------------------------
 * AVRCP_METADATA_CONTROLLER constant
 *
 *     When set to XA_ENABLED, the AVRCP profile adds handling for
 *     the Metadata feature of AVRCP for the controller device.  This feature is
 *     used to exchanged media information with the remote device.
 *
 *     For controllers, the application may invoke several Metadata functions
 *     for receiving the player settings and media information from the target
 *     device.  The application will also receive AVRCP_EVENT_MDA_RESPONSE and 
 *     AVRCP_EVENT_MDA_NOTIFY events.
 */
#ifndef AVRCP_METADATA_CONTROLLER
#define AVRCP_METADATA_CONTROLLER XA_ENABLED
#endif /* AVRCP_METADATA_CONTROLLER */

/*---------------------------------------------------------------------------
 * AVRCP_METADATA_COMPANY_ID constant
 *
 *     Defines the company ID used to identify Metadata Transfer commands.
 *     Metadata Transfer commands are sent using Vendor Dependent AV/C commands.
 *     This value was assigned by the IEEE Registration Authority Committee.
 *
 *     This 24-bit value is expressed in a string as a three-byte value with
 *     most significant byte first.
 */
#define AVRCP_METADATA_COMPANY_ID "\x00\x19\x58"

/*---------------------------------------------------------------------------
 * AVRCP_METADATA_MAX_CHAR_SETS constant
 *
 *     Defines the maximum number of supported character sets that will be
 *     read by the TG and sent by the CT.  The character sets define all the
 *     supported IANA character sets supported by the device.  UTF-8 is the 
 *     default and must be supported by all devices.
 */
#ifndef AVRCP_METADATA_MAX_CHAR_SETS
#define AVRCP_METADATA_MAX_CHAR_SETS  10
#endif /* AVRCP_METADATA_MAX_CHAR_SETS */

/*---------------------------------------------------------------------------
 * AVRCP_METADATA_RESPONSE_SIZE constant
 *
 *     Defines the maximum size of a response received by the controller or
 *     sent by the target.  This size should be large enough to handle
 *     the largest possible media attribute response.  The response includes
 *     all the attribute strings + 8 bytes per attribute and one more byte
 *     to hold the number of attributes in the response.  The supported media
 *     attributes are defined by AvrcpMediaAttrId.
 *
 *     This value should be at least 512 bytes, but it can be set to 0 if
 *     it is desired to dynamically set the buffer by calling 
 *     AVRCP_SetMetadataResponseBuffer().  If a response is too large for
 *     the allocated buffer, only part of the response will be sent.
 *
 *     The default size is 1024 bytes.
 */
#ifndef AVRCP_METADATA_RESPONSE_SIZE
#define AVRCP_METADATA_RESPONSE_SIZE    1024
#endif /* AVRCP_METADATA_RESPONSE_SIZE */

/* Other internal constants */

#define AVRCP_NUM_SEARCH_ATTRIBUTES       24

#if TI_CHANGES == XA_ENABLED
#define AVRCP_NUM_ATTRIBUTES			   6
#else
#define AVRCP_NUM_ATTRIBUTES               5
#endif /* TI_CHANGES == XA_ENABLED */

#define AVRCP_NUM_PLAYER_SETTINGS          4
#define AVRCP_NUM_METADATA_EVENTS          8
#define AVRCP_NUM_MEDIA_ELEMENTS           7
#define AVRCP_MAX_NOTIFICATIONS           15
#define AVRCP_METADATA_CMD_SIZE          512

/* Number of milliseconds to wait before deciding that a pressed key is being
 * held (target-side)
 */
#define AVRCP_PANEL_PRESSHOLD_TIME 300

/* How long to wait before assuming that the currently held key has been
 * released (target-side)
 */
#define AVRCP_PANEL_AUTORELEASE_TIME 2000

/* How long to wait between updates for a held-down key (controller-side)
 */
#define AVRCP_PANEL_HOLDUPDATE_TIME 1000

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AvrcpEvent type
 *      
 */
typedef AvctpEvent AvrcpEvent;

/** The transport layer is connected and commands/responses can now
 *  be exchanged.
 * 
 *  During this callback, the 'p.remDev' parameter is valid.
 */
#define AVRCP_EVENT_CONNECT             AVCTP_EVENT_CONNECT

/** The application will receive this event when a lower layer connection
 *  (L2CAP) has been disconnected.  Both the target and controller of the
 *  connection are notified.
 * 
 */
#define AVRCP_EVENT_DISCONNECT          AVCTP_EVENT_DISCONNECT

/** A remote device is attempting to connect the transport layer.
 *  Only the acceptor of the connection is notified.  The acceptor may
 *  call AVRCP_ConnectRsp() to either accept or reject the connection.  If
 *  AVRCP_ConnectRsp() is not called, the connection is automatically
 *  accepted.
 * 
 *  During this callback, the 'p.remDev' parameter is valid.
 */
#define AVRCP_EVENT_CONNECT_IND         AVCTP_EVENT_CONNECT_IND

/** A command was received from the remote AVCTP device (controller).
 * 
 *  During this callback, the 'p.cmdFrame' parameter is valid. It contains the
 *  the AVRCP command header information, including operands. If the "more"
 *  value is TRUE then this event only signals the first part of the operands.
 *  Subsequent AVRCP_EVENT_OPERANDS events will follow this event with
 *  additional operand data.
 *
 *  Note that the AVRCP specification requires that target devices respond to
 *  commands within 100ms after receiving the command.
 */
#define AVRCP_EVENT_COMMAND             AVCTP_EVENT_COMMAND

/** A response was received from the remote AVCTP device (target).
 * 
 * During this callback, the 'p.rspFrame' parameter is valid. It contains the
 * the AVRCP response header information, including operands. If the "more"
 * value is TRUE then this event only signals the first part of the operands.
 * Subsequent AVRCP_EVENT_OPERANDS events will follow this event with
 * additional operand data.
 */
#define AVRCP_EVENT_RESPONSE            AVCTP_EVENT_RESPONSE

/** The remote device (target) rejected the AVCTP command.
 * 
 * During this callback, the 'p.rspFrame' parameter is valid. It contains the
 * the AVRCP reject header information, including operands. If the "more"
 * value is TRUE then this event only signals the first part of the operands.
 * Subsequent AVCTP_EVENT_OPERANDS events will follow this event with
 * additional operand data.
 */
#define AVRCP_EVENT_REJECT              AVCTP_EVENT_REJECT

/** A command has timed out.
 * 
 * During this callback, the 'p.cmdFrame' parameter is valid. It contains the
 * the AVCTP command that was sent.
 */
#define AVRCP_EVENT_CMD_TIMEOUT         AVCTP_EVENT_CMD_TIMEOUT

/** A command (see AVRCP_SendCommand) or response (see AVRCP_SendResponse)
 * has been sent. Memory allocated for the operation can be freed or reused
 * after receiving this event.
 * 
 * During this callback, the 'p.cmdFrame' or 'p.rspFrame' parameter associated
 * with the sent command or response is valid. In addition, "status" will be
 * set to indicate "BT_STATUS_SUCCESS" or "BT_STATUS_FAILED" to indicate
 * whether the event was properly delivered.
 */
#define AVRCP_EVENT_TX_DONE             AVCTP_EVENT_TX_DONE

/** Additional operand data has been received for the previous
 * AVRCP_EVENT_COMMAND or AVRCP_EVENT_RESPONSE.
 * 
 * During this callback, the 'p.cmdFrame' or 'p.rspFrame' parameter associated
 * with the received command or response is valid. The "operands" and
 * "operandLen" fields indicate the chunk of operands being received for
 * the command or response. If the "more" field is set to TRUE, the full
 * operand buffer will be received in multiple _OPERANDS events and the
 * last operand buffer indicated with the "more" field set to FALSE.
 */
#define AVRCP_EVENT_OPERANDS            AVCTP_EVENT_OPERANDS

#if AVRCP_PANEL_SUBUNIT == XA_ENABLED

/** The key corresponding to a panel operation has been pressed on the
 * remote controller device. See p.panelInd for information about the
 * operation.
 *
 * AVRCP_RejectPanelOperation may be used to reject an unsupported or
 * reserved command. If the operation is not rejected during the callback
 * it is implicitly accepted. If accepted, the next panel-related event
 * for the operation will be AVRCP_EVENT_PANEL_HOLD (if the controller key
 * is held down for at least AVRCP_PANEL_PRESSHOLD_TIME) or
 * AVRCP_EVENT_PANEL_RELEASE (if the controller key is released more quickly).
 *
 * This event is only generated when AVRCP_PANEL_SUBUNIT is set to XA_ENABLED.
 */
#define AVRCP_EVENT_PANEL_PRESS        (AVCTP_EVENT_LAST + 1)

/** The key corresponding to a panel operation has been held down on the
 * remote controller device for at least AVRCP_PANEL_PRESSHOLD_TIME.
 * See p.panelInd for information about the operation.
 *
 * A target receiving this event should act as if the key was held down until
 * the AVRCP_EVENT_PANEL_RELEASE event is received.
 *
 * This event is only generated when AVRCP_PANEL_SUBUNIT is set to XA_ENABLED. 
 */
#define AVRCP_EVENT_PANEL_HOLD         (AVCTP_EVENT_LAST + 2)

/** The key corresponding to a panel operation has been released on the
 * remote controller device.  See p.panelInd for information about the
 * operation.
 *
 * This event is only generated when AVRCP_PANEL_SUBUNIT is set to XA_ENABLED.
 */
#define AVRCP_EVENT_PANEL_RELEASE      (AVCTP_EVENT_LAST + 3)

/** A panel response has been received from the remote target device.
 * All fields of "p.panelCnf" are valid.
 *
 * This event is only generated when AVRCP_PANEL_SUBUNIT is set to XA_ENABLED. 
 */
#define AVRCP_EVENT_PANEL_CNF          (AVCTP_EVENT_LAST + 4)

#endif /* AVRCP_PANEL_SUBUNIT == XA_ENABLED */

#if AVRCP_METADATA_TARGET == XA_ENABLED

/** Metadata status information has been received from the target.
 *  During this callback, "mdaOp" is set to the operation type that was
 *  received from the controller, and "p.mdaInfo" contains the relevant
 *  information based on the operation type.
 *
 *  This event is only generated when AVRCP_METADATA_TARGET is set to XA_ENABLED. 
 */
#define AVRCP_EVENT_MDA_INFO           (AVCTP_EVENT_LAST + 5)

#endif

#if AVRCP_METADATA_CONTROLLER == XA_ENABLED

/** A Metadata command was sent successfully to the target and a response was
 *  received.  During this callback, "mdaOp" is set to the operation type 
 *  that was sent to the target, and "p.mdaInfo" contains the relevant response
 *  information based on the operation type.
 * 
 *  This event is only generated when AVRCP_METADATA_CONTROLLER is set to XA_ENABLED. 
 */
#define AVRCP_EVENT_MDA_RESPONSE       (AVCTP_EVENT_LAST + 6)

/** The notification of a registered event has been received.
 *  During this callback, "p.mdaInfo.notify" contains the relevant notification
 *  information.  The "p.mdaInfo.notify.event" field contains the type of
 *  notification, and "p.mdaInfo.notify.value" contains the value of the
 *  notification.
 * 
 *  This event is only generated when AVRCP_METADATA_CONTROLLER is set to XA_ENABLED. 
 */
#define AVRCP_EVENT_MDA_NOTIFY         (AVCTP_EVENT_LAST + 7)

#endif

#define AVRCP_EVENT_LAST               (AVCTP_EVENT_LAST + 7)


#if TI_CHANGES == XA_ENABLED							
	
/** This event is received when all streams have been closed.
 */
#define AVRCP_EVENT_DISABLED				(AVRCP_EVENT_LAST + 1)

/** These events are not received from the AVRCP Controller but rather produced by
 *  the BTL_BSC (Bluetooth System Coordinator) on arriving of incoming call
 *  during A2DP streaming and on finishing of the call.
 */
#define AVRCP_BSC_EVENT_RECOMMENDED_PAUSE	(AVRCP_EVENT_LAST + 2)
#define AVRCP_BSC_EVENT_RECOMMENDED_RESUME_PLAYING	(AVRCP_EVENT_LAST + 3)

#endif /* TI_CHANGES == XA_ENABLED */  


/* End of AvrcpEvent */

/*---------------------------------------------------------------------------
 * AvrcpRole type
 *      
 */
typedef U16 AvrcpRole;

/* AVRCP Controller role */
#define AVRCP_CT    SC_AV_REMOTE_CONTROL

/* AVRCP Target role */
#define AVRCP_TG    SC_AV_REMOTE_CONTROL_TARGET
/* End of AvrcpRole */

/*---------------------------------------------------------------------------
 * AvrcpCtype type
 *
 * This type defines the AV/C ctype (command type) codes.
 */
typedef AvctpCtype AvrcpCtype;

#define AVRCP_CTYPE_CONTROL               AVCTP_CTYPE_CONTROL
#define AVRCP_CTYPE_STATUS                AVCTP_CTYPE_STATUS
#define AVRCP_CTYPE_SPECIFIC_INQUIRY      AVCTP_CTYPE_SPECIFIC_INQUIRY
#define AVRCP_CTYPE_NOTIFY                AVCTP_CTYPE_NOTIFY
#define AVRCP_CTYPE_GENERAL_INQUIRY       AVCTP_CTYPE_GENERAL_INQUIRY
/* End of AvrcpCtype */

/*---------------------------------------------------------------------------
 * AvrcpResponse type
 *
 * This type defines the AV/C response codes.
 */
typedef AvctpResponse AvrcpResponse;

#define AVRCP_RESPONSE_NOT_IMPLEMENTED    AVCTP_RESPONSE_NOT_IMPLEMENTED   
#define AVRCP_RESPONSE_ACCEPTED           AVCTP_RESPONSE_ACCEPTED          
#define AVRCP_RESPONSE_REJECTED           AVCTP_RESPONSE_REJECTED          
#define AVRCP_RESPONSE_IN_TRANSITION      AVCTP_RESPONSE_IN_TRANSITION     
#define AVRCP_RESPONSE_IMPLEMENTED_STABLE AVCTP_RESPONSE_IMPLEMENTED_STABLE
#define AVRCP_RESPONSE_CHANGED            AVCTP_RESPONSE_CHANGED           
#define AVRCP_RESPONSE_INTERIM            AVCTP_RESPONSE_INTERIM

/* This code, when received in an AVRCP_EVENT_PANEL_CNF event, indicates
 * that a "release" command was not actually delivered to the target because
 * the original "press" command was rejected.
 *
 * This value is NOT legal for use in functions that accept AvrcpResponse
 * as a parameter.
 */
#define AVRCP_RESPONSE_SKIPPED            0xF0

/* This code, when received in an AVRCP_EVENT_PANEL_CNF event, indicates
 * that the expected response message from the target was not received
 * within the expected time frame. The application may proceed normally
 * as if the command was accepted, or take some other action.
 *
 * This value is NOT legal for use in functions that accept AvrcpResponse
 * as a parameter.
 *
 * Note: The TIMEOUT response is currently not implemented. AVRCP
 * expects, as per specifications, that all commands will receive a response.
 * Commands that timeout therefore cause a service link disconnection.
 */
#define AVRCP_RESPONSE_TIMEOUT            0xF1
/* End of AvrcpResponse */

/*---------------------------------------------------------------------------
 * AvrcpOpcode type
 *
 * This type defines the AV/C Opcodes.
 */
typedef AvctpOpcode AvrcpOpcode;

#define AVRCP_OPCODE_VENDOR_DEPENDENT     AVCTP_OPCODE_VENDOR_DEPENDENT
#define AVRCP_OPCODE_UNIT_INFO            AVCTP_OPCODE_UNIT_INFO       
#define AVRCP_OPCODE_SUBUNIT_INFO         AVCTP_OPCODE_SUBUNIT_INFO    
#define AVRCP_OPCODE_PASS_THROUGH         AVCTP_OPCODE_PASS_THROUGH    
/* End of AvrcpOpcode */

/*---------------------------------------------------------------------------
 * AvrcpCmdFrame type
 *
 * Defines the parameters required for an RCP command.
 */
typedef AvctpCmdFrame AvrcpCmdFrame;

/* End of AvrcpCmdFrame */

/*---------------------------------------------------------------------------
 * AvrcpRspFrame type
 *
 * Defines the parameters required for an RCP response.
 */
typedef AvctpRspFrame AvrcpRspFrame;

/* End of AvrcpRspFrame */

/*---------------------------------------------------------------------------
 * AvrcpSubunitType type
 *
 *     Defines recognized AVRCP subunit types 
 */
typedef U8 AvrcpSubunitType;

/* Panel subunit type, as specified by the 1394 AV/C Panel Subunit
 * Specification
 */
#define AVRCP_SUBUNIT_PANEL        0x09

/* Unit type, used when requesting Unit Info or Subunit Info */
#define AVRCP_SUBUNIT_UNIT         0x1F

/* End of AvrcpSubunitType */

/*---------------------------------------------------------------------------
 * AvrcpPanelOperation type
 *
 *     Panel subunit operations that may be sent (by a controller) or
 *     received (by a target). These codes are defined by the 1394
 *     AV/C Panel Subunit Specification (version 1.1).
 */
typedef U16 AvrcpPanelOperation;

#define AVRCP_POP_SELECT            0x0000
#define AVRCP_POP_UP                0x0001
#define AVRCP_POP_DOWN              0x0002
#define AVRCP_POP_LEFT              0x0003
#define AVRCP_POP_RIGHT             0x0004
#define AVRCP_POP_RIGHT_UP          0x0005
#define AVRCP_POP_RIGHT_DOWN        0x0006
#define AVRCP_POP_LEFT_UP           0x0007
#define AVRCP_POP_LEFT_DOWN         0x0008
#define AVRCP_POP_ROOT_MENU         0x0009
#define AVRCP_POP_SETUP_MENU        0x000A
#define AVRCP_POP_CONTENTS_MENU     0x000B
#define AVRCP_POP_FAVORITE_MENU     0x000C
#define AVRCP_POP_EXIT              0x000D

#define AVRCP_POP_0                 0x0020
#define AVRCP_POP_1                 0x0021
#define AVRCP_POP_2                 0x0022
#define AVRCP_POP_3                 0x0023
#define AVRCP_POP_4                 0x0024
#define AVRCP_POP_5                 0x0025
#define AVRCP_POP_6                 0x0026
#define AVRCP_POP_7                 0x0027
#define AVRCP_POP_8                 0x0028
#define AVRCP_POP_9                 0x0029
#define AVRCP_POP_DOT               0x002A
#define AVRCP_POP_ENTER             0x002B
#define AVRCP_POP_CLEAR             0x002C

#define AVRCP_POP_CHANNEL_UP        0x0030
#define AVRCP_POP_CHANNEL_DOWN      0x0031
#define AVRCP_POP_PREVIOUS_CHANNEL  0x0032
#define AVRCP_POP_SOUND_SELECT      0x0033
#define AVRCP_POP_INPUT_SELECT      0x0034
#define AVRCP_POP_DISPLAY_INFO      0x0035
#define AVRCP_POP_HELP              0x0036
#define AVRCP_POP_PAGE_UP           0x0037
#define AVRCP_POP_PAGE_DOWN         0x0038

#define AVRCP_POP_POWER             0x0040
#define AVRCP_POP_VOLUME_UP         0x0041
#define AVRCP_POP_VOLUME_DOWN       0x0042
#define AVRCP_POP_MUTE              0x0043
#define AVRCP_POP_PLAY              0x0044
#define AVRCP_POP_STOP              0x0045
#define AVRCP_POP_PAUSE             0x0046
#define AVRCP_POP_RECORD            0x0047
#define AVRCP_POP_REWIND            0x0048
#define AVRCP_POP_FAST_FORWARD      0x0049
#define AVRCP_POP_EJECT             0x004A
#define AVRCP_POP_FORWARD           0x004B
#define AVRCP_POP_BACKWARD          0x004C

#define AVRCP_POP_ANGLE             0x0050
#define AVRCP_POP_SUBPICTURE        0x0051

#define AVRCP_POP_F1                0x0071
#define AVRCP_POP_F2                0x0072
#define AVRCP_POP_F3                0x0073
#define AVRCP_POP_F4                0x0074
#define AVRCP_POP_F5                0x0075

#define AVRCP_POP_VENDOR_UNIQUE     0x007E

#define AVRCP_POP_NEXT_GROUP        0x017E
#define AVRCP_POP_PREV_GROUP        0x027E

#define AVRCP_POP_RESERVED          0x007F

/* End of AvrcpPanelOperation */

/*---------------------------------------------------------------------------
 * AvrcpMetadataOperation type
 *
 *     Metadata operations that may be sent/received by a controller or
 *     target). 
 */
typedef U8 AvrcpMetadataOperation;

#define AVRCP_MDA_GET_CAPABILITIES              0x10

#define AVRCP_MDA_LIST_PLAYER_SETTING_ATTRIBS   0x11
#define AVRCP_MDA_LIST_PLAYER_SETTING_VALUES    0x12
#define AVRCP_MDA_GET_PLAYER_SETTING_VALUE      0x13
#define AVRCP_MDA_SET_PLAYER_SETTING_VALUE      0x14
#define AVRCP_MDA_GET_PLAYER_SETTING_ATTR_TEXT  0x15
#define AVRCP_MDA_GET_PLAYER_SETTING_VALUE_TEXT 0x16
#define AVRCP_MDA_INFORM_DISP_CHAR_SET          0x17
#define AVRCP_MDA_INFORM_BATT_STATUS            0x18

#define AVRCP_MDA_GET_MEDIA_INFO                0x20

#define AVRCP_MDA_GET_PLAY_STATUS               0x30
#define AVRCP_MDA_REGISTER_NOTIFY               0x31

#define AVRCP_MDA_REQUEST_CONT_RESP             0x40
#define AVRCP_MDA_ABORT_CONT_RESP               0x41

/* End of AvrcpMetadataOperation */


/*---------------------------------------------------------------------------
 * AvrcpMetadataErrorCode
 *
 *     Error code for Metadata acknowledgements.
 */
typedef U8 AvrcpMetadataErrorCode;

#define AVRCP_MDA_ERR_INVALID_CMD     0x01
#define AVRCP_MDA_ERR_INVALID_PARM    0x02
#define AVRCP_MDA_ERR_PARM_NOT_FOUND  0x03
#define AVRCP_MDA_ERR_INTERNAL_ERROR  0x04

/* End of AvrcpMetadataErrorCode */


/*---------------------------------------------------------------------------
 * AvrcpMetadataEventId
 *
 *     Metadata transfer events.
 */
typedef U8 AvrcpMetadataEventId;

#define AVRCP_MDA_MEDIA_STATUS_CHANGED    1  /* Change in media status      */

#define AVRCP_MDA_TRACK_CHANGED           2  /* Current track changed       */

#define AVRCP_MDA_TRACK_END               3  /* Reached end of track        */

#define AVRCP_MDA_TRACK_START             4  /* Reached track start         */

#define AVRCP_MDA_PLAY_POS_CHANGED        5  /* Change in playback position.
                                              * Returned after the specified
                                              * playback notification change
                                              * notification interval.
                                              */

#define AVRCP_MDA_BATT_STATUS_CHANGED     6  /* Change in battery status    */
#define AVRCP_MDA_SYS_STATUS_CHANGED      7  /* Change in system status     */
#define AVRCP_MDA_APP_SETTING_CHANGED     8  /* Change in player application
                                              * setting
                                              */

/* End of AvrcpMetadataEventId */

/*---------------------------------------------------------------------------
 * AvrcpMetadataEventMask
 *
 * Bitmask of supported metadata transfer events.  By default, only
 * AVRCP_ENABLE_PLAY_STATUS_CHANGED and AVRCP_ENABLE_TRACK_CHANGED are 
 * enabled when a channel is registered.  The application must explicitly
 * enable any other supported events.

 */
typedef U8 AvrcpMetadataEventMask;

#define AVRCP_ENABLE_PLAY_STATUS_CHANGED 0x01  /* Change in playback status   */
#define AVRCP_ENABLE_TRACK_CHANGED       0x02  /* Current track changed       */
#define AVRCP_ENABLE_TRACK_END           0x04  /* Reached end of track        */
#define AVRCP_ENABLE_TRACK_START         0x08  /* Reached track start         */
#define AVRCP_ENABLE_PLAY_POS_CHANGED    0x10  /* Change in playback position */
#define AVRCP_ENABLE_BATT_STATUS_CHANGED 0x20  /* Change in battery status    */
#define AVRCP_ENABLE_SYS_STATUS_CHANGED  0x40  /* Change in system status     */
#define AVRCP_ENABLE_APP_SETTING_CHANGED 0x80  /* Change in player application
                                                * setting
                                                */
/* End of AvrcpMetadataEventMask */

/*---------------------------------------------------------------------------
 * AvrcpMdaCapabilityId type
 *
 * Defines the capability ID for the AVRCP_MDA_GET_CAPABILITIES metadata
 * transfer command.
 */
typedef U8 AvrcpMdaCapabilityId;

#define AVRCP_MDA_CAPABILITY_COMPANY_ID        1
#define AVRCP_MDA_CAPABILITY_EVENTS_SUPPORTED  2

/* End of AvrcpMdaCapabilityId */


/*---------------------------------------------------------------------------
 * AvrcpPlayerAttrId type
 *
 * Defines the attibute IDs used for specific player application settings.
 */
typedef U8 AvrcpPlayerAttrId;

#define AVRCP_PLAYER_EQ_STATUS        1  /* Player equalizer status */
#define AVRCP_PLAYER_REPEAT_STATUS    2  /* Player repeat status */
#define AVRCP_PLAYER_SHUFFLE_STATUS   3  /* Player Shuffle status */
#define AVRCP_PLAYER_SCAN_STATUS      4  /* Repeat on/off */

/* End of AvrcpPlayerAttrId */

/*---------------------------------------------------------------------------
 * AvrcpPlayerAttrIdMask type
 *
 * Defines which attibute IDs are supported for player application settings.
 * By default, no attributes are enabled when a channel is registered.  The
 * application must explicitly enable and supported attributes and set the
 * appropriate values.
 */
typedef U8 AvrcpPlayerAttrIdMask;

#define AVRCP_ENABLE_PLAYER_EQ_STATUS       0x01
#define AVRCP_ENABLE_PLAYER_REPEAT_STATUS   0x02
#define AVRCP_ENABLE_PLAYER_SHUFFLE_STATUS  0x04
#define AVRCP_ENABLE_PLAYER_SCAN_STATUS     0x08

/* End of AvrcpPlayerAttrIdMask */

/*---------------------------------------------------------------------------
 * AvrcpEqValue type
 *
 * Defines values for the player equalizer status.
 */
typedef U8 AvrcpEqValue;

#define AVRCP_EQ_OFF  1
#define AVRCP_EQ_ON   2

/* End of AvrcpEqValue */

/*---------------------------------------------------------------------------
 * AvrcpRepeatValue type
 *
 * Defines values for the player repeat mode status.
 */
typedef U8 AvrcpRepeatValue;

#define AVRCP_REPEAT_OFF     1
#define AVRCP_REPEAT_SINGLE  2
#define AVRCP_REPEAT_ALL     3
#define AVRCP_REPEAT_GROUP   4

/* End of AvrcpRepeatValue */

/*---------------------------------------------------------------------------
 * AvrcpShuffleValue type
 *
 * Defines values for the player shuffle mode status.
 */
typedef U8 AvrcpShuffleValue;

#define AVRCP_SHUFFLE_OFF    1
#define AVRCP_SHUFFLE_ALL    2
#define AVRCP_SHUFFLE_GROUP  3

/* End of AvrcpShuffleValue */

/*---------------------------------------------------------------------------
 * AvrcpScanValue type
 *
 * Defines values for the player scan mode status.
 */
typedef U8 AvrcpScanValue;

#define AVRCP_SCAN_OFF    1
#define AVRCP_SCAN_ALL    2
#define AVRCP_SCAN_GROUP  3

/* End of AvrcpScanValue */

/*---------------------------------------------------------------------------
 * AvrcpBatteryStatus type
 *
 * Defines values for battery status.
 */
typedef U8 AvrcpBatteryStatus;

#define AVRCP_BATT_STATUS_NORMAL      0
#define AVRCP_BATT_STATUS_WARNING     1
#define AVRCP_BATT_STATUS_CRITICAL    2
#define AVRCP_BATT_STATUS_EXTERNAL    3
#define AVRCP_BATT_STATUS_FULL_CHARGE 4

/* End of AvrcpBatteryStatus */

/*---------------------------------------------------------------------------
 * AvrcpSystemStatus type
 *
 * Defines values for system status.
 */
typedef U8 AvrcpSystemStatus;

#define AVRCP_SYS_POWER_ON   0
#define AVRCP_SYS_POWER_OFF  1
#define AVRCP_SYS_UNPLUGGED  2

/* End of AvrcpSystemStatus */

/*---------------------------------------------------------------------------
 * AvrcpMediaAttrId type
 *
 * Defines values for the media attribute ID.
 */
typedef U32 AvrcpMediaAttrId;

#define AVRCP_MEDIA_ATTR_TITLE       0x00000001
#define AVRCP_MEDIA_ATTR_ARTIST      0x00000002
#define AVRCP_MEDIA_ATTR_ALBUM       0x00000003
#define AVRCP_MEDIA_ATTR_TRACK       0x00000004
#define AVRCP_MEDIA_ATTR_NUM_TRACKS  0x00000005
#define AVRCP_MEDIA_ATTR_GENRE       0x00000006
#define AVRCP_MEDIA_ATTR_DURATION    0x00000007

/* End of AvrcpMediaAttrId */

/*---------------------------------------------------------------------------
 * AvrcpMediaAttrIdMask type
 *
 * Defines supported values for the media attribute ID.  By default, only
 * AVRCP_ENABLE_MEDIA_ATTR_TITLE is enabled when a channel is registered.
 * The application must explicitly enable any supported attributes and
 * set the appropriate values.
 */
typedef U8 AvrcpMediaAttrIdMask;

#define AVRCP_ENABLE_MEDIA_ATTR_TITLE       0x01
#define AVRCP_ENABLE_MEDIA_ATTR_ARTIST      0x02
#define AVRCP_ENABLE_MEDIA_ATTR_ALBUM       0x04
#define AVRCP_ENABLE_MEDIA_ATTR_TRACK       0x08
#define AVRCP_ENABLE_MEDIA_ATTR_NUM_TRACKS  0x10
#define AVRCP_ENABLE_MEDIA_ATTR_GENRE       0x20
#define AVRCP_ENABLE_MEDIA_ATTR_DURATION    0x40

/* End of AvrcpMediaAttrIdMask */

/*---------------------------------------------------------------------------
 * AvrcpMediaStatus type
 *
 * Defines play status of the currently playing media.
 */
typedef U8 AvrcpMediaStatus;

#define AVRCP_MEDIA_STOPPED       0x00
#define AVRCP_MEDIA_PLAYING       0x01
#define AVRCP_MEDIA_PAUSED        0x02
#define AVRCP_MEDIA_FWD_SEEK      0x03
#define AVRCP_MEDIA_REV_SEEK      0x04
#define AVRCP_MEDIA_ERROR         0xFF

/* End of AvrcpMediaStatus */

/*---------------------------------------------------------------------------
 * AvrcpFeatures type
 *
 *     AVRCP implementations are required to register the features they
 *     support with SDP. One or more of the following values can be OR'ed
 *     together to express the features supported.
 *
 *     See the AVRCP_SUPPORTED_FEATURES configuration constant for more
 *     details on use of this type.
 */
typedef U16 AvrcpFeatures;

/* Player/recorder features supported ("play" and "stop") */
#define AVRCP_FEATURES_CATEGORY_1  0x0001

/* Monitor/amplifier features supported ("volume up" and "volume down") */
#define AVRCP_FEATURES_CATEGORY_2  0x0002

/* Tuner features supported ("channel up" and "channel down") */
#define AVRCP_FEATURES_CATEGORY_3  0x0004

/* Menu features supported ("root menu", "up", "down", "left", "right", and
 * "select")
 */
#define AVRCP_FEATURES_CATEGORY_4  0x0008

/* End of AvrcpFeatures */

#if TI_CHANGES == XA_ENABLED
/*---------------------------------------------------------------------------
 * AvrcpCccConnState type
 *     
 *     State of connection establishing used in possible cancellation of
 *     creating connection
 */
typedef U8 AvrcpCccConnState;

#define AVRCP_CCC_CONN_STATE_NONE            0x00
#define AVRCP_CCC_CONN_STATE_CONNECTING_ACL  0x01
#define AVRCP_CCC_CONN_STATE_SDP_QUERY       0x02
#define AVRCP_CCC_CONN_STATE_CONNECTING      0x03

/* End of AvrcpCccConnState */
#endif /* TI_CHANGES == XA_ENABLED */

/* Forward reference */
typedef struct _AvrcpChannel AvrcpChannel;
typedef struct _AvrcpCallbackParms AvrcpCallbackParms;
typedef struct _AvrcpMetadataPdu AvrcpMetadataPdu;
typedef struct _AvrcpPlayerSetting AvrcpPlayerSetting;
typedef struct _AvrcpPlayerStrings AvrcpPlayerStrings;
typedef struct _AvrcpEqString AvrcpEqString;
typedef struct _AvrcpRepeatString AvrcpRepeatString;
typedef struct _AvrcpShuffleString AvrcpShuffleString;
typedef struct _AvrcpScanString AvrcpScanString;
typedef struct _AvrcpMetadataCharSets AvrcpMetadataCharSets;
typedef struct _AvrcpMediaInfo AvrcpMediaInfo;
typedef struct _AvrcpMediaPlayStatus AvrcpMediaPlayStatus;

/*---------------------------------------------------------------------------
 * AvrcpCallback type
 *
 * A function of this type is called to indicate events to the application.
 */
typedef void (*AvrcpCallback)(AvrcpChannel *chnl, AvrcpCallbackParms *Parms);

/* End of AvrcpCallback */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AvrcpEqString Structure
 *
 * Contains EQ Status.
 */
struct _AvrcpEqString {
    /* Index 0 == Off, Index 1 == On */
    const char    *string[2];
    U8             len[2];
};

/*---------------------------------------------------------------------------
 * AvrcpRepeatString Structure
 *
 * Contains Repeat status.
 */
struct _AvrcpRepeatString {
    /* Index 0 == Off, Index 1 == Single, Index 2 == All, Index 3 == Group */
    const char       *string[4];
    U8                len[4];
};

/*---------------------------------------------------------------------------
 * AvrcpShuffleString Structure
 *
 * Contains Shuffle status.
 */
struct _AvrcpShuffleString {
    /* Index 0 == Off, Index 1 == All, Index 2 == Group */
    const char        *string[3];
    U8                 len[3];
};

/*---------------------------------------------------------------------------
 * AvrcpScanString Structure
 *
 * Contains Scan status.
 */
struct _AvrcpScanString {
    /* Index 0 == Off, Index 1 == All, Index 2 == Group */
    const char     *string[3];
    U8              len[3];
};

/*---------------------------------------------------------------------------
 * AvrcpPlayerSetting Structure
 *
 * Contains the Media Player device settings.
 */
struct _AvrcpPlayerSetting {
    AvrcpPlayerAttrId      attrId;     /* Media Player Attribute ID */

    union {
        AvrcpEqValue       eq;
        AvrcpRepeatValue   repeat;
        AvrcpShuffleValue  shuffle;
        AvrcpScanValue     scan;

        U8                 value;     /* Used Internally */
    } setting;
};

/*---------------------------------------------------------------------------
 * AvrcpPlayerStrings Structure
 *
 * Contains the Media Player attribute and value strings.
 */
struct _AvrcpPlayerStrings {

    const char         *attrString;  /* Media Player Attribute Text */
    U8                  attrLen;     /* Length of the Attribute Text */

    union {
        AvrcpEqString       eq;      /* Equalizer status */
        AvrcpRepeatString   repeat;  /* Repeat mode status */
        AvrcpShuffleString  shuffle; /* Shuffle mode status */
        AvrcpScanString     scan;    /* Scan mode status */
    } setting;
};

/*---------------------------------------------------------------------------
 * AvrcpMediaInfo Structure
 *
 * Contains the Media element information.
 */
struct _AvrcpMediaInfo {
    AvrcpMediaAttrId      attrId;      /* Media element attribute ID */
    const char           *string;      /* The media element */
    U16                   length;      /* Length of the media element */
};

/*---------------------------------------------------------------------------
 * AvrcpMetadataCharSets Structure
 *
 * Contains the supported character sets.
 */
struct _AvrcpMetadataCharSets {
    U8   numCharSets;
    U16  charSets[AVRCP_METADATA_MAX_CHAR_SETS];
};

/*---------------------------------------------------------------------------
 * AvrcpMediaPlayStatus structure
 *
 * Contains status of the currently playing media.
 */
struct _AvrcpMediaPlayStatus {
    U32               length;
    U32               position;
    AvrcpMediaStatus  mediaStatus;
};

/*--------------------------------------------------------------------------
 * AvrcpMetadataPdu structure
 *
 * Contains the data for a metadata PDU.
 */
struct _AvrcpMetadataPdu {

    /* Operation (PDU ID) */
    AvrcpMetadataOperation  op;

    /* Operation parameter length */
    U16 parmLen;

    /* Operation parameters */
    U8 *parms;

    /* === Internal use only === */

    BOOL more;
    U16 curLen;
    U16 bytesToSend;
    BOOL abort;
};

/*---------------------------------------------------------------------------
 * AvrcpCallbackParms structure
 *
 * Contains information for the application callback event.
 */
struct _AvrcpCallbackParms {
    /* AVRCP event */
    AvrcpEvent      event;

    /* AVRCP channel associated with the event */
    AvrcpChannel   *channel;

    /* Status of the operation */
    BtStatus        status;

#if (AVRCP_METADATA_CONTROLLER == XA_ENABLED) || (AVRCP_METADATA_TARGET == XA_ENABLED)

    /* AVRCP Metadata operation */
    AvrcpMetadataOperation   mdaOp;

#endif

    /* Callback parameter object */
    union {
        /* Remote device associated with the event */
        BtRemoteDevice *remDev;

        /* Command frame associated with the event */
        AvrcpCmdFrame  *cmdFrame;

        /* Response frame associated with the event */
        AvrcpRspFrame  *rspFrame;
        
#if AVRCP_PANEL_SUBUNIT == XA_ENABLED

        /* Panel indication received during AVRCP_EVENT_PANEL_CNF */
        struct {
            /* Operation to which the remote target responded */
            AvrcpPanelOperation operation;
            
            /* The press state of the key in the command to which
             * the target responded.
             */
            BOOL                press;

            /* Response from the target. May indicate an "extended" response
             * code such as AVRCP_RESPONSE_SKIPPED or AVRCP_RESPONSE_TIMEOUT.
             */ 
            AvrcpResponse       response;
        } panelCnf;

        /* Panel indication received during AVRCP_EVENT_PANEL_PRESS,
         * AVRCP_EVENT_PANEL_HOLD, or AVRCP_EVENT_PANEL_RELEASE
         */
        struct {
            /* Operation corresponding to the key pressed, held, or
             * released. AVRCP will only indicate a new operation
             * when the previous one has been _RELEASE'd.
             */
            AvrcpPanelOperation operation;
        } panelInd;
#endif /* AVRCP_PANEL_SUBUNIT == XA_ENABLED */

#if (AVRCP_METADATA_CONTROLLER == XA_ENABLED) || (AVRCP_METADATA_TARGET == XA_ENABLED)

       union {

#if AVRCP_METADATA_TARGET == XA_ENABLED

            /* The list of character sets displayable on the controller.
             * This is valid during the AVRCP_EVENT_MDA_INFO event
             * when mdaOp is set to AVRCP_MDA_INFORM_DISP_CHAR_SET.
             * The default is 106 (UTF-8)
             */
            AvrcpMetadataCharSets    charSet;

            /* The battery status of the controller.
             * This is valid during the AVRCP_EVENT_MDA_INFO event
             * when mdaOp is set to AVRCP_MDA_INFORM_BATT_STATUS.
             */
            AvrcpBatteryStatus       battStatus;

#endif /* AVRCP_METADATA_TARGET == XA_ENABLED */

#if AVRCP_METADATA_CONTROLLER == XA_ENABLED

            /* The capabilities of the target.
             * This is valid during the AVRCP_EVENT_MDA_RESPONSE event
             * when mdaOp is set to AVRCP_MDA_GET_CAPABILITIES.
             */
            struct {

                /* The type of capability. */
                AvrcpMdaCapabilityId type;

                /* The capability info. */
                union {

                    /* The list of company IDs.
                     * (type == AVRCP_MDA_CAPABILITY_COMPANY_ID) 
                     */
                    struct {
                        /* The number of supported company IDs. */
                        U8  numIds;

                        /* An array of company IDs (3 bytes each). */
                        U8 *ids;
                    } companyId;

                    /* A bitmask of the supported events. 
                     * (type == AVRCP_MDA_CAPABILITY_EVENTS_SUPPORTED)
                     */
                    AvrcpMetadataEventMask  eventMask;
                } info;

            } capability;

            /* A bitmask that indicates the player settings supported by
             * the target device.
             * This is valid during the AVRCP_EVENT_MDA_RESPONSE event
             * when mdaOp is set to AVRCP_MDA_LIST_PLAYER_SETTING_ATTRIBS.
             */
            AvrcpPlayerAttrIdMask  attrMask;

            /* The list of values for the requested attribute on the target
             * device.
             * This is valid during the AVRCP_EVENT_MDA_RESPONSE event
             * when mdaOp is set to AVRCP_MDA_LIST_PLAYER_SETTING_VALUES.
             */
            struct {
                /* The number of supported value IDs. */
                U8  numIds;

                /* An array of value ID (1 byte each). */
                U8 *ids;
            } attrValues;

            struct {
                /* The number of attribute IDs returned. */
                U8 numIds;

                /* An array of attribute text information */
                struct {
                    AvrcpPlayerAttrId  attrId;
                    U16                charSet;
                    U8                 length;
                    const char        *string;
                } txt[AVRCP_NUM_PLAYER_SETTINGS];
            } attrStrings;

            /* The value of each player setting.
             * This is valid during the AVRCP_EVENT_MDA_RESPONSE event
             * when mdaOp is set to AVRCP_MDA_GET_PLAYER_SETTING_VALUE.
             */
            struct {
                /* Bitmask that describes which attributes are being reported */
                AvrcpPlayerAttrIdMask  attrMask;

                /* The equalizer setting. */
                AvrcpEqValue           eq;

                /* The repeat setting. */
                AvrcpRepeatValue       repeat;

                /* The shuffle setting. */
                AvrcpShuffleValue      shuffle;

                /* The scan setting. */
                AvrcpScanValue         scan;
            } setting;

            /* The list of setting values for the requested attribute on the target.
             * This is valid during the AVRCP_EVENT_MDA_RESPONSE event
             * when mdaOp is set to AVRCP_MDA_GET_PLAYER_SETTING_VALUE_TEXT.
             */
            struct {
                /* The number of settings returned */
                U8  numSettings;

                /* An array of value text information */
                struct {
                    U8          valueId;
                    U16         charSet;
                    U8          length;
                    const char *string;
                } txt[AVRCP_NUM_PLAYER_SETTINGS];
            } settingStrings;

            /* The list of element values for the current track on the target.
             * This is valid during the AVRCP_EVENT_MDA_RESPONSE event
             * when mdaOp is set to AVRCP_MDA_GET_MEDIA_INFO.
             */
            struct {
                /* The number of elements returned */
                U8 numIds;

                /* An array of element value text information */
                struct {
                    U32         attrId;
                    U16         charSet;
                    U16         length;
                    const char *string;
                } txt[AVRCP_NUM_MEDIA_ELEMENTS];
            } element;

            /* The playback status of the current track.
             * This is valid during the AVRCP_EVENT_MDA_RESPONSE event
             * when mdaOp is set to AVRCP_MDA_GET_PLAY_STATUS.
             */
            struct {
                U32               length;
                U32               position;
                AvrcpMediaStatus  mediaStatus;
            } playStatus;

            /* Notification or status of a registered event.
             * This is valid during the AVRCP_EVENT_MDA_RESPONSE
             * when mdaOp is set to AVRCP_MDA_REGISTER_NOTIFY.  Also valid
             * during the AVRCP_EVENT_MDA_NOTIFY event.
             */
            struct {

                /* Defines the Metadata event that was received */
                AvrcpMetadataEventId  event;

                union {
                    /* Play status of the media */
                    AvrcpMediaStatus    mediaStatus;

                    /* The current track */
                    U32                 track;

                    /* The position (ms) of the current track */
                    U32                 position;

                    /* The battery status of the target */
                    AvrcpBatteryStatus  battStatus;

                    /* The system status of the target */
                    AvrcpSystemStatus   sysStatus;

                    struct {
                        /* Bitmask that describes which attributes are being reported */
                        AvrcpPlayerAttrIdMask  attrMask;

                        /* The equalizer setting. */
                        AvrcpEqValue           eq;

                        /* The repeat setting. */
                        AvrcpRepeatValue       repeat;

                        /* The shuffle setting. */
                        AvrcpShuffleValue      shuffle;

                        /* The scan setting. */
                        AvrcpScanValue         scan;
                    } setting;
                } value;

            } notify;

#endif /* AVRCP_METADATA_CONTROLLER == XA_ENABLED */

       } mdaInfo;

#endif /* (AVRCP_METADATA_CONTROLLER == XA_ENABLED) || (AVRCP_METADATA_TARGET == XA_ENABLED) */

    } p;
};

/*---------------------------------------------------------------------------
 * AvrcpChannel structure
 *
 * Defines the RCP channel.
 */
struct _AvrcpChannel {
    /* === Internal use only === */

    /* AVRCP Role */
    AvrcpRole       role;

    /* Application callback function */
    AvrcpCallback   callback;

    /* AVCTP Channel */
    AvctpChannel    chnl;

    /* Transaction IDs */
    U8              txTransId;
    U8              rxTransId;

    /* Connection Manager Handler */
    CmgrHandler     cmgrHandler;

    /* SDP Variables */
    SdpQueryToken   sdpQueryToken;
    U8              sdpSrchAttrib[AVRCP_NUM_SEARCH_ATTRIBUTES];

    /* Features of currently connected remote device */
    AvrcpFeatures   remoteFeatures;

#if TI_CHANGES == XA_ENABLED
    /* Flag and state for implementing Cancel Create Connection feature */
    AvrcpCccConnState cccConnState;
    BOOL              cancelCreateConn;
#endif /* TI_CHANGES == XA_ENABLED */

#if AVRCP_PANEL_SUBUNIT == XA_ENABLED
    U8 panelState;
    EvmTimer panelTimer;

    union {
        struct {        
            AvrcpPanelOperation opQueue[AVRCP_SUBUNIT_OP_QUEUE_MAX];
            U8 opRead, opWrite;
            AvrcpCmdFrame cmd;
#if TI_CHANGES == XA_ENABLED
            U8 data[7];
#else
            U8 data[4];
#endif
        } ct;
        
        struct {
            AvrcpPanelOperation curOp;
            AvrcpResponse curRsp;
            AvrcpRspFrame rsp;
#if TI_CHANGES == XA_ENABLED
            U8 data[7];
#else
            U8 data[5];
#endif
        } tg;
    } panel;    
#endif
    
#if (AVRCP_METADATA_TARGET == XA_ENABLED) || (AVRCP_METADATA_CONTROLLER == XA_ENABLED)

    struct {

#if AVRCP_METADATA_TARGET == XA_ENABLED

        /* Supported Metadata Transfer Events */
        AvrcpMetadataEventMask  eventMask;

        /* Supported Metadata Transfer player settings */
        AvrcpPlayerAttrIdMask   playerSettingsMask;

        /* Supported Metadata media attributes */
        AvrcpMediaAttrIdMask    mediaAttrMask;

        /* Current Metadata Transfer player settings */
        AvrcpPlayerSetting      playerSettings[AVRCP_NUM_PLAYER_SETTINGS];

        /* Current Metadata Transfer player strings */
        AvrcpPlayerStrings      playerStrings[AVRCP_NUM_PLAYER_SETTINGS];

        /* Current Metadata Transfer media info */
        AvrcpMediaInfo          mediaInfo[AVRCP_NUM_MEDIA_ELEMENTS];

        /* Current status of the playing media */
        AvrcpMediaPlayStatus    playStatus;

        /* Battery Status */
        AvrcpBatteryStatus      battStatus;

        /* System Status */
        AvrcpSystemStatus       sysStatus;

        /* Current track index */
        U32                     currentTrack;

        /* Current character set */
        AvrcpMetadataCharSets   mdaCharSets;
        U16                     defaultCharSet;

        /* Current Registered Notifications Parms */
        U8                      numNotifications;
        U32                     playbackInterval;

        /* Flag to see if the response structure is in use */
        BOOL                    mdaRspInUse;

        AvrcpRspFrame rsp;
        
#endif /* AVRCP_METADATA_TARGET == XA_ENABLED */

        /* Current Registered Notifications */
        U8                      notifications[AVRCP_MAX_NOTIFICATIONS + 1];

#if AVRCP_METADATA_CONTROLLER == XA_ENABLED

        BOOL                    mdaCmdInUse;

        AvrcpCmdFrame cmd;

#endif /* AVRCP_METADATA_CONTROLLER == XA_ENABLED */

        U8                      type; /* 0 = unknown, 1 = command, 2 = response */

        /* Command PDU */
        AvrcpMetadataPdu        mdaCmdPdu;
        U8                      mdaCmdParms[AVRCP_METADATA_CMD_SIZE];
        BOOL                    mdaCmdFragment;

        /* Response PDU */
        AvrcpMetadataPdu        mdaRspPdu;
        U8                     *mdaRspParms;
        U16                     mdaRspParmLen;
#if AVRCP_METADATA_RESPONSE_SIZE > 0
        U8                      iMdaRspParms[AVRCP_METADATA_RESPONSE_SIZE];
#endif
        BOOL                    mdaRspFragment;

    } metadata;

#endif /* (AVRCP_METADATA_TARGET == XA_ENABLED) || (AVRCP_METADATA_CONTROLLER == XA_ENABLED) */

};

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AVRCP_Init()
 *
 *     Initialize the AVRCP SDK.  This function should only be called
 *     once, normally at initialization time.  The calling of this function
 *     can be specified in overide.h using the XA_LOAD_LIST macro
 *     (i.e. #define XA_LOAD_LIST XA_MODULE(AVRCP) ... ).
 *
 * Returns:
 *     TRUE - Initialization was successful
 *
 *     FALSE - Initialization failed.
 */
BOOL AVRCP_Init(void);

/*---------------------------------------------------------------------------
 * AVRCP_Register()
 *
 *      Registers an application callback to receive AVRCP events. This 
 *      function must be called before any other AVRCP functions.
 *
 * Parameters:
 *
 *      chnl - Channel structure that receives or initiates connections.
 *
 *      callback - Identifies the application function that will be called
 *        with AVRCP events.
 *
 *      role - Channel role (CT or TG.)
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The AVRCP application callback Function was
 *      successfully registered.
 *
 *      BT_STATUS_IN_USE - The specified channel is already in use.
 *
 *      BT_STATUS_INVALID_PARM - The chnl or Callback parameter does not contain 
 *         a valid pointer. (XA_ERROR_CHECK only).
 */
BtStatus AVRCP_Register(AvrcpChannel *chnl, AvrcpCallback callback, AvrcpRole role);

/*---------------------------------------------------------------------------
 * AVRCP_Deregister()
 *
 *      De-registers the AVRCP callback. After making this call
 *      successfully, the callback specified in AVRCP_Register will
 *      receive no further events.
 *
 * Parameters:
 *
 *      chnl - Channel structure that receives or initiates connections.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The AVRCP callback was successfully deregistered.
 * 
 *      BT_STATUS_IN_USE - The specified channel is still in use.
 *
 *      BT_STATUS_NOT_FOUND - An AVRCP callback was not previously registered.
 *     
 *      BT_STATUS_INVALID_PARM - The chnl parameter does not contain a valid 
 *         pointer. (XA_ERROR_CHECK only).
 */
BtStatus AVRCP_Deregister(AvrcpChannel *chnl);

/*---------------------------------------------------------------------------
 * AVRCP_Connect()
 * 
 *     Initiates a connection to a remote AVRCP device.  This function is 
 *     used to establish the lower layer connection (L2CAP), which allows
 *     sending messages.
 *
 *     If the connection attempt is successful, the AVRCP_EVENT_CONNECT event
 *     will be received.  If the connection attempt is unsuccessful, the
 *     AVRCP_EVENT_DISCONNECT event will be received.
 *
 * Parameters:
 *
 *      chnl - Channel structure that receives or initiates connections.
 *
 *      addr - The Bluetooth address of the device to which the connection 
 *             should be made. If 0, the connection manager is used
 *             to select an appropriate device.
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The connection process has been successfully
 *         started. When the connection process is complete, the
 *         application callback will receive either the AVRCP_EVENT_CONNECT or 
 *         AVRCP_EVENT_DISCONNECT event.
 *
 *     BT_STATUS_IN_USE - This channel is already connected or is in the
 *         process of connecting.
 *
 *     BT_STATUS_INVALID_PARM - The chnl parameter does not contain a 
 *         valid pointer. (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified device was not found in the device
 *         selector database.  The device must be discovered, paired, or added
 *         manually using DS_AddDevice();
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus AVRCP_Connect(AvrcpChannel *chnl, BD_ADDR *addr);

/*---------------------------------------------------------------------------
 * AVRCP_ConnectRsp()
 * 
 *     Responds to a connection request from the remote AVRCP device.  This 
 *     function is used to establish the lower layer connection (L2CAP), 
 *     which allows the sending of commands.
 *
 * Parameters:
 *
 *     Chnl - A registered and open AVRCP channel.
 *
 *     Accept - TRUE accepts the connect or FALSE rejects the connection.
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The connection responses has been successfully
 *         sent. When the connection process is complete, the application 
 *         callback will receive the AVRCP_EVENT_CONNECT event.
 *
 *     BT_STATUS_BUSY - The connection is already connected.
 *
 *     BT_STATUS_INVALID_PARM - The Chnl parameter does not contain a 
 *         valid pointer. (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified device was not found in the device
 *         selector database.  The device must be discovered, paired, or added
 *         manually using DS_AddDevice();
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus AVRCP_ConnectRsp(AvrcpChannel *Chnl, BOOL Accept);
#define AVRCP_ConnectRsp(c, a) AVCTP_ConnectRsp(&(c)->chnl, a)

/*---------------------------------------------------------------------------
 * AVRCP_Disconnect()
 *
 *     Terminates a connection with a remote AVRCP device.  The lower layer
 *     connection (L2CAP) is disconnected.
 *
 * Parameters:
 *
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The disconnect process has been successfully
 *         started. When the disconnect process is complete, the
 *         application callback will receive the AVRCP_EVENT_DISCONNECT event.
 *
 *     BT_STATUS_INVALID_PARM - The chnl parameter does not contain a valid 
 *         pointer. (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists on the specified channel.
 *
 *     BT_STATUS_NOT_FOUND - The specified device was not found in the device
 *         selector database.  The device must be discovered, paired, or added
 *         manually using DS_AddDevice();
 *
 *     It is possible to receive other error codes, depending on the lower 
 *     layer service in use (L2CAP or Management Entity).
 */
BtStatus AVRCP_Disconnect(AvrcpChannel *chnl);

/*---------------------------------------------------------------------------
 * AVRCP_SendCommand()
 *
 *     Sends an RCP command on the specified channel. The channel must be
 *     registered as a controller (SC_AV_REMOTE_CONTROL), connected, in
 *     the open state, and without another command outstanding.
 *     The "cmdFrame" parameter must be set with valid RCP command parameters.
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     cmdFrame - An AvrcpCmdFrame structure initialized with valid
 *         RCP command parameters.
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The send command operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVRCP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_BUSY - A command is already outstanding.
 *
 *     BT_STATUS_INVALID_PARM - The chnl parameter does not contain a valid 
 *         pointer, or the channel is not registered as a controller
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_SendCommand(AvrcpChannel *chnl, AvrcpCmdFrame *cmdFrame);

/*---------------------------------------------------------------------------
 * AVRCP_SendResponse()
 *
 *     Sends an RCP command on the specified channel. The channel must be
 *     connected and in the open state. The "cmdFrame" parameter must be
 *     set with valid RCP command parameters.
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     rspFrame - An AvrcpRspFrame structure initialized with valid
 *         RCP response parameters.
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The send response operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVRCP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_FAILED - There is no outstanding command to respond to.
 *
 *     BT_STATUS_INVALID_PARM - The chnl parameter does not contain a valid 
 *         pointer. (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_SendResponse(AvrcpChannel *chnl, AvrcpRspFrame *rspFrame);

#if AVRCP_PANEL_SUBUNIT == XA_ENABLED
/*---------------------------------------------------------------------------
 * AVRCP_SetPanelKey()
 *
 *     Indicates the state of the key corresponding to the specified
 *     panel subunit operation. Successive calls to this function will
 *     queue up key events to be delivered to the target.
 *
 * Requires:
 *     AVRCP_PANEL_SUBUNIT set to XA_ENABLED
 * 
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     op - Panel operation code to send. If a previous call indicated a
 *         different "op" as pressed, calling this function with a new
 *         "op" will automatically release it.
 *
 *     press - TRUE indicates the key corresponding to operation was pressed,
 *         FALSE indicates the key was released. For FALSE, if the "op"
 *         specified was not already pressed, this call signals a
 *         single press-and-release of the key.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation was started successfully.
 *         AVRCP_EVENT_PANEL_CNF message(s) will be sent corresponding
 *         to the target's responses to the press, release, or hold messages
 *         brought about by this command.
 *
 *     BT_STATUS_NO_RESOURCES - The internal keystroke buffer is full.
 *
 *     BT_STATUS_FAILED - The channel is not connected or some other error
 *         occurred.
 *
 *     BT_STATUS_INVALID_PARM - The chnl parameter does not contain a valid 
 *         pointer. (XA_ERROR_CHECK only).
 */
BtStatus AVRCP_SetPanelKey(AvrcpChannel *chnl, AvrcpPanelOperation op,
                           BOOL press);

/*---------------------------------------------------------------------------
 * AVRCP_RejectPanelOperation()
 *
 *     Rejects a panel operation received from the connected
 *     controller. This function must be called during
 *     notification of the AVRCP_EVENT_PANEL_IND event. Further notifications
 *     corresponding to the operation (AVRCP_PS_HOLD, AVRCP_PS_RELEASE)
 *     will not be received.
 *
 *     If this function is not called during the AVRCP_EVENT_PANEL_IND
 *     the operation is automatically "ACCEPTED".
 *
 * Requires:
 *     AVRCP_PANEL_SUBUNIT set to XA_ENABLED
 * 
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     rsp - Response code. Must be one of AVRCP_RESPONSE_NOT_IMPLEMENTED
 *         or AVRCP_RESPONSE_REJECTED. Note that REJECTED is only used
 *         when the operation is already reserved by another controller.
 *         This response code does NOT indicate the result of the
 *         operation itself (for example, a PLAY command when no media
 *         is available should be accepted, but simply have no effect).
 *
 * Returns:
 *     None
 */
void AVRCP_RejectPanelOperation(AvrcpChannel *chnl, AvrcpResponse rsp);
#define AVRCP_RejectPanelOperation(c,r) (c)->panel.tg.curRsp = (r)

#endif /* AVRCP_PANEL_SUBUNIT == XA_ENABLED */


#if AVRCP_METADATA_TARGET == XA_ENABLED

/*---------------------------------------------------------------------------
 * AVRCP_TgSetMdaEventMask()
 *
 *     Allows the application to specify which event will be supported
 *     by the current media player.  When a flag is set in the event
 *     mask, then associated feature is supported.  
 *     AVRCP_ENABLE_PLAY_STATUS_CHANGED and AVRCP_ENABLE_TRACK_CHANGED
 *     must both be set, and if not specified, will be added to the
 *     mask.
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     mask - A bitmask with bits set to enable individual events.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 */
BtStatus AVRCP_TgSetMdaEventMask(AvrcpChannel *chnl, AvrcpMetadataEventMask mask);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetMdaPlayerSettingsMask()
 *
 *     Allows the application to specify which player settings are 
 *     supported by the current media player.  When a flag is set in 
 *     the event mask, then associated setting is supported.  
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     mask - A bitmask with bits set to enable individual player settings.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 */
BtStatus AVRCP_TgSetMdaPlayerSettingsMask(AvrcpChannel *chnl, 
                                          AvrcpPlayerAttrIdMask mask);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetMdaMediaAttributesMask()
 *
 *     Allows the application to specify which media attributes are
 *     supported by the current media player.  When a flag is set in 
 *     the event mask, then associated setting is supported.  
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     mask - A bitmask with bits set to enable individual media attributes.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 */
BtStatus AVRCP_TgSetMdaMediaAttributesMask(AvrcpChannel *chnl, 
                                           AvrcpMediaAttrIdMask mask);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetMdaPlayerSetting()
 *
 *     Allows the application to specify the settings for the current player.
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     setting - The new player setting.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_PENDING - The send response operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or setting parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_TgSetMdaPlayerSetting(AvrcpChannel *chnl, 
                                     AvrcpPlayerSetting *setting);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetMdaPlayerStrings()
 *
 *     Allows the application to specify the strings for the current player.
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     attrId - The player setting attribute to which the strings apply.
 *
 *     strings - A set of strings in the IANA format specified in the call to 
 *         AVRCP_TgSetMdaCharSet().  If AVRCP_TgSetMdaCharSet() has 
 *         never been called, then the default format is UTF-8.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or strings parameter is invalid.
 */
BtStatus AVRCP_TgSetMdaPlayerStrings(AvrcpChannel *chnl, 
                                     AvrcpPlayerAttrId attrId,
                                     AvrcpPlayerStrings *strings);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetMdaMediaInfo()
 *
 *     Allows the application to specify the media information for the 
 *     current track.  
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     numElements - The number of elements in the info parameter.
 *
 *     info - An array of structures containing the media information for the
 *         current track (see AvrcpMediaInfo).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or info parameter is invalid.
 */
BtStatus AVRCP_TgSetMdaMediaInfo(AvrcpChannel *chnl, U8 numElements, 
                                 AvrcpMediaInfo *info);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetMdaPlayStatus()
 *
 *     Allows the application to specify the current player status.
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     mediaStatus - The current play status of the media.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_PENDING - The send response operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_TgSetMdaPlayStatus(AvrcpChannel *chnl, 
                                  AvrcpMediaStatus mediaStatus);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetMdaPlayPosition()
 *
 *     Allows the application to specify the media play position and length. 
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     position - The current position of the media in milliseconds.
 *
 *     length - The total length of the media in milliseconds.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_PENDING - The send response operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_TgSetMdaPlayPosition(AvrcpChannel *chnl, U32 position, 
                                    U32 length);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetMdaBattStatus()
 *
 *     Allows the application to specify the current Battery status.
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     battStatus - The current battery status.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_PENDING - The send response operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_TgSetMdaBattStatus(AvrcpChannel *chnl, 
                                  AvrcpBatteryStatus battStatus);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetMdaSystemStatus()
 *
 *     Allows the application to specify the current System status.
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     sysStatus - The current system status.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_PENDING - The send response operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_TgSetMdaSystemStatus(AvrcpChannel *chnl, 
                                    AvrcpSystemStatus sysStatus);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetMdaTrack()
 *
 *     Allows the application to specify the current track.
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     track - The current track number.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_PENDING - The send response operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_TgSetMdaTrack(AvrcpChannel *chnl, U32 track);

/*---------------------------------------------------------------------------
 * AVRCP_TgSignalMdaTrackStart()
 *
 *     Allows the application to signal the track start.
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_PENDING - The send response operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_TgSignalMdaTrackStart(AvrcpChannel *chnl);

/*---------------------------------------------------------------------------
 * AVRCP_TgSignalMdaTrackEnd()
 *
 *     Allows the application to signal the end of track.
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_PENDING - The send response operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_TgSignalMdaTrackEnd(AvrcpChannel *chnl);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetCurrentMdaCharSet()
 *
 *     Allows the application to specify the default IANA character set.
 *     This number must match a character set received during the
 *     AVRCP_EVENT_MDA_CHAR_SET event, or it must be set to UTF-8 (default).
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 *     charSet - The IANA character set that will be used.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 */
BtStatus AVRCP_TgSetCurrentMdaCharSet(AvrcpChannel *chnl, U16 charSet);

/*---------------------------------------------------------------------------
 * AVRCP_TgGetCurrentMdaCharSet()
 *
 *     Retrieves the character set currently selected for display on the
 *     controller.
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *     A 16 bit value containing the current IANA character set.
 */
U16 AVRCP_TgGetCurrentMdaCharSet(AvrcpChannel *chnl);

#endif /* AVRCP_METADATA_TARGET == XA_ENABLED */

#if AVRCP_METADATA_CONTROLLER == XA_ENABLED

/*---------------------------------------------------------------------------
 * AVRCP_CtGetMdaCapabilities()
 *
 *     Get the capabilities of the target device.
 *
 * Requires:
 *     AVRCP_METADATA_CONTROLLER set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_CtGetMdaCapabilities(AvrcpChannel *chnl, 
                                         AvrcpMdaCapabilityId capabilityId);

/*---------------------------------------------------------------------------
 * AVRCP_CtListMdaPlayerSettingAttrs()
 *
 *     List the attributes for the settings of the current media 
 *     player on the target device.
 *
 * Requires:
 *     AVRCP_METADATA_CONTROLLER set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_CtListMdaPlayerSettingAttrs(AvrcpChannel *chnl);

/*---------------------------------------------------------------------------
 * AVRCP_CtListPlayerSettingValues()
 *
 *     List the values for the settings of the current media player 
 *     on the target device.
 *
 * Requires:
 *     AVRCP_METADATA_CONTROLLER set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_CtListMdaPlayerSettingValues(AvrcpChannel *chnl, 
                                            AvrcpPlayerAttrId attrId);

/*---------------------------------------------------------------------------
 * AVRCP_CtGetMdaPlayerSettingValues()
 *
 *     Get the settings values for the current media player on the 
 *     target device.
 *
 * Requires:
 *     AVRCP_METADATA_CONTROLLER set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_CtGetMdaPlayerSettingValues(AvrcpChannel *chnl, 
                                           AvrcpPlayerAttrIdMask attrMask);

/*---------------------------------------------------------------------------
 * AVRCP_CtSetMdaPlayerSettingValues()
 *
 *     Set the settings values of the current media player on
 *     the target device.
 *
 * Requires:
 *     AVRCP_METADATA_CONTROLLER set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or setting parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_CtSetMdaPlayerSettingValues(AvrcpChannel *chnl, U8 numSettings, 
                                           AvrcpPlayerSetting *setting);

/*---------------------------------------------------------------------------
 * AVRCP_CtGetMdaPlayerSettingAttrTxt()
 *
 *     Get the text for the setting attributes of the current media
 *     player on the target device.
 *
 * Requires:
 *     AVRCP_METADATA_CONTROLLER set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_CtGetMdaPlayerSettingAttrTxt(AvrcpChannel *chnl, 
                                            AvrcpPlayerAttrIdMask attrMask);

/*---------------------------------------------------------------------------
 * AVRCP_CtGetMdaPlayerSettingValueTxt()
 *
 *     Get the text for the setting values of the current media
 *     player on the target device.
 *
 * Requires:
 *     AVRCP_METADATA_CONTROLLER set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_CtGetMdaPlayerSettingValueTxt(AvrcpChannel *chnl, 
                                             AvrcpPlayerAttrId attrId);

/*---------------------------------------------------------------------------
 * AVRCP_CtInformMdaCharset()
 *
 *     Send the controller's displayable character set to the target
 *     device.
 *
 * Requires:
 *     AVRCP_METADATA_CONTROLLER set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or charsets parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_CtInformMdaCharset(AvrcpChannel *chnl, U8 numCharsets,
                                       U16 *charsets);

/*---------------------------------------------------------------------------
 * AVRCP_CtInformMdaBatteryStatus()
 *
 *     Send the controller's battery status to the target device.
 *
 * Requires:
 *     AVRCP_METADATA_CONTROLLER set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_CtInformMdaBatteryStatus(AvrcpChannel *chnl, 
                                        AvrcpBatteryStatus battStatus);

/*---------------------------------------------------------------------------
 * AVRCP_CtGetMdaMediaInfo()
 *
 *     Get the media attributes for the current track on the current 
 *     media player on target device.
 *
 * Requires:
 *     AVRCP_METADATA_CONTROLLER set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_CtGetMdaMediaInfo(AvrcpChannel *chnl, AvrcpMediaAttrIdMask mediaMask);

/*---------------------------------------------------------------------------
 * AVRCP_CtGetMdaPlayStatus()
 *
 *     Get the play status of the current media player on the target
 *     device.
 *
 * Requires:
 *     AVRCP_METADATA_CONTROLLER set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_CtGetMdaPlayStatus(AvrcpChannel *chnl);

/*---------------------------------------------------------------------------
 * AVRCP_CtRegisterMdaNotification()
 *
 *     Register for notification of events on the target device.
 *
 * Requires:
 *     AVRCP_METADATA_CONTROLLER set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVRCP_CtRegisterMdaNotification(AvrcpChannel *chnl, 
                                         AvrcpMetadataEventId eventId,
                                         U32 interval);

#endif /* AVRCP_METADATA_CONTROLLER == XA_ENABLED */

#if (AVRCP_METADATA_CONTROLLER == XA_ENABLED) || (AVRCP_METADATA_TARGET == XA_ENABLED)

/*---------------------------------------------------------------------------
 * AVRCP_SetMdaResponseBuffer()
 *
 *     Set the response buffer and size for the channel.  The response buffer
 *     is used for reassembly of responses on the controller or for
 *     assembling a response on the target.  The buffer size must be at least
 *     512 bytes.  Calling this function will override the internal buffer,
 *     if one is allocated.  Defining AVRCP_METADATA_RESPONSE_SIZE to a value
 *     greater than 0 allocates an internal response buffer.  
 *
 * Requires:
 *     AVRCP_METADATA_CONTROLLER or AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 *     buffer - a buffer to hold responses for transmit (target) or
 *              receive (controller).
 *     bufLen - the length of the buffer.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The buffer was set successfully.
 *
 *     BT_STATUS_INVALID_PARM - The channel structure or buffer was invalid, 
 *         or the buffer length was less than 512 (XA_ERROR_CHECK enabled).
 */
BtStatus AVRCP_SetMdaResponseBuffer(AvrcpChannel *chnl, U8 *buffer, U16 bufLen);


#endif /* (AVRCP_METADATA_CONTROLLER == XA_ENABLED) || (AVRCP_METADATA_TARGET == XA_ENABLED) */

#endif /* __AVRCP_H_ */

