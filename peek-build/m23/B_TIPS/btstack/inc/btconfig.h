#ifndef __BTCONFIG_H
#define __BTCONFIG_H

/****************************************************************************
 *
 * File:
 *     $Workfile:btconfig.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:169$
 *
 * Description:
 *     Default constants for configuring the Bluetooth protocol stack.
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

#include "config.h"

#if !defined(BT_STACK) || BT_STACK == XA_DISABLED
#error "Bluetooth Stack must be enabled!"
#endif

/*---------------------------------------------------------------------------
 * Bluetooth Configuration API layer
 *
 *     The constants in this layer configure the layers of the Bluetooth
 *     stack. To change a constant, simply #define it in your overide.h
 *     include file.
 *
 *     Configuration constants here and in the General Configuration API
 *     (config.h) are used to make the stack more appropriate for a
 *     particular environment. Constants can be modified to allow tradeoffs
 *     between code size, RAM usage, functionality, and throughput. 
 *
 *     Some constants are numeric, and others indicate whether a feature
 *     is enabled (defined as XA_ENABLED) or disabled (defined as
 *     XA_DISABLED).
 */

/****************************************************************************
 *
 * Section: General Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * NUM_BT_DEVICES constant
 *
 *     The number of devices with which we can connect. This value
 *     represents the maximum size of the piconet in which this device is
 *     the master, plus any master devices for which we are concurrently a
 *     slave. This value includes devices which may be parked, holding,
 *     or sniffing as well any active devices.
 *
 *     The default value is 2.
 */
#ifndef NUM_BT_DEVICES
#define NUM_BT_DEVICES  2
#endif

#if NUM_BT_DEVICES < 1
#error NUM_BT_DEVICES must be greater than 0
#endif

#if NUM_BT_DEVICES > 255
#error NUM_BT_DEVICES must be less than 256
#endif

/*---------------------------------------------------------------------------
 * NUM_SCO_CONNS constant
 * 
 *     Defines the number of SCO connections supported by this device.
 *     When NUM_SCO_CONNS is set to 0, SCO capabilities are disabled
 *     to save code size.
 *
 *     The default value is 0, and the maximum is 3.
 */
#ifndef NUM_SCO_CONNS
#define NUM_SCO_CONNS 0
#endif

#if NUM_SCO_CONNS > 3
#error NUM_SCO_CONNS must be 3 or less
#endif

/****************************************************************************
 *
 * Section: Management Entity Constants
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * BT_ALLOW_SCAN_WHILE_CON constant
 *
 *     Determines whether page and inquiry scans are allowed during a
 *     connection. Performing Page and Inquiry scans while connected takes
 *     bandwidth away from the connections, so it should only be used in
 *     circumstances where scatternets or master/slave switching is required
 *     (for instance, when implementing a LAN access port).
 *
 *     When BT_ALLOW_SCAN_WHILE_CON is XA_ENABLED, the Management Entity
 *     includes code for page and inquiry scans during a connection. When
 *     it is XA_DISABLED, scans are not allowed during a connection.
 *
 */
#ifndef BT_ALLOW_SCAN_WHILE_CON
#define BT_ALLOW_SCAN_WHILE_CON XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * BT_SCO_HCI_DATA constant
 *
 *     Determines whether code to send and receive SCO is included. This 
 *     constant should be defined only when SCO audio data is sent through
 *     the HCI interface. SCO connections are always supported when 
 *     NUM_SCO_CONNS is greater than zero.
 *
 *     By default, this constant is disabled.
 */
#ifndef BT_SCO_HCI_DATA
#define BT_SCO_HCI_DATA XA_DISABLED
#endif

#if BT_SCO_HCI_DATA == XA_ENABLED && NUM_SCO_CONNS == 0 
#error NUM_SCO_CONNS must be > 0 when BT_SCO_HCI_DATA is enabled
#endif

/*---------------------------------------------------------------------------
 * BT_SECURITY constant
 *
 *     Determines whether code to handle BT security features is included. 
 *     This constant is typically enabled, but may be disabled for minimal
 *     applications that don't need security.
 *
 *     By default, this constant is enabled.
 */
#ifndef BT_SECURITY
#define BT_SECURITY XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_SECURITY constant
 *
 *     Determines the security model that applies to new connections.
 *     The Bluetooth Security Architecture White Paper defines a default
 *     security setting that is to be used if no registration exists for
 *     a protocol/channel. The default security is defined as follows:
 *
 *     Incoming Connection:   Authorization and authentication required.
 *
 *     Outgoing Connection:   Authentication required.
 *
 *     If the BT_DEFAULT_SECURITY is set to XA_ENABLED then default security
 *     as defined above is used for protocol/channel access requests that
 *     have no registered security record. In this case a default
 *     authorization handler must also be registered.
 * 
 *     If BT_DEFAULT_SECURITY is set to XA_DISABLED then protocol/channel
 *     access requests that have no security record are granted access
 *     (no security).
 *
 *     By default, this constant is disabled.
 */
#ifndef BT_DEFAULT_SECURITY
#define BT_DEFAULT_SECURITY XA_DISABLED
#endif /* BT_DEFAULT_SECURITY */

#if BT_DEFAULT_SECURITY == XA_ENABLED && BT_SECURITY == XA_DISABLED 
#error BT_SECURITY must be enabled if BT_DEFAULT_SECURITY is enabled.
#endif

/*---------------------------------------------------------------------------
 * BT_SECURITY_TIMEOUT constant
 *
 *     Specifies the number of seconds to elapse before a service-specific
 *     access request (see SEC_AccessRequest) is automatically cancelled.
 *     The stack's security manager will cancel the request regardless of
 *     what is causing the delay. Cancelling the request will cause the
 *     security request to fail, which in most cases will prevent the
 *     requesting service from connecting.
 *
 *     This value does not affect authentication or encryption operations
 *     requested directly through APIs such as SEC_AuthenticateLink or
 *     SEC_SetLinkEncryption; it only affects operations initated through
 *     SEC_AccessRequest.
 *
 *     Set this value to 0 to disable security timeouts. When disabled, it
 *     may be necessary to drop the ACL connection in order to safely cancel
 *     the security requeest.
 *
 *     By default, this value is set to 80 seconds. Although any time value
 *     may be used, sufficient time should be allowed for the user of both
 *     the local and remote devices to enter PIN codes and select
 *     authorization settings if required.
 */
#ifndef BT_SECURITY_TIMEOUT
#define BT_SECURITY_TIMEOUT 80
#endif /* BT_SECURITY_TIMEOUT */


/*---------------------------------------------------------------------------
 * BT_DEFAULT_PAGE_SCAN_WINDOW constant
 *
 *     The following are used as the default values for settings in the radio
 *     module and must be 0 or match the radio defaults. When an application
 *     sets specific values for radio settings then returns to the default
 *     settings of the radio it is necessary to set the radio defaults. When
 *     setting the radio defaults these macros are used. Therefore, these
 *     values must be set appropriately for the given radio. If the values
 *     are set to 0 then that implies that these values cannot be changed
 *     for the given radio. If the values are set to 0 the API will prevent
 *     a application from attempting to change the values.
 */
#ifndef BT_DEFAULT_PAGE_SCAN_WINDOW
#define BT_DEFAULT_PAGE_SCAN_WINDOW 0
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_PAGE_SCAN_INTERVAL constant
 *
 *     See BT_DEFAULT_PAGE_SCAN_WINDOW.
 */
#ifndef BT_DEFAULT_PAGE_SCAN_INTERVAL
#define BT_DEFAULT_PAGE_SCAN_INTERVAL 0
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_INQ_SCAN_WINDOW constant
 *
 *     See BT_DEFAULT_PAGE_SCAN_WINDOW.
 */
#ifndef BT_DEFAULT_INQ_SCAN_WINDOW
#define BT_DEFAULT_INQ_SCAN_WINDOW 0
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_INQ_SCAN_INTERVAL constant
 *
 *     See BT_DEFAULT_PAGE_SCAN_WINDOW.
 */
#ifndef BT_DEFAULT_INQ_SCAN_INTERVAL
#define BT_DEFAULT_INQ_SCAN_INTERVAL 0
#endif

/* Verify that the values are in the legal range */
#define BT_PSW_TEST \
    ((BT_DEFAULT_PAGE_SCAN_WINDOW != 0) && \
    ((BT_DEFAULT_PAGE_SCAN_WINDOW < 0x0012) || \
     (BT_DEFAULT_PAGE_SCAN_WINDOW > 0x1000)))

#if BT_PSW_TEST
#error BT_DEFAULT_PAGE_SCAN_WINDOW is not legal
#endif

#define BT_PSI_TEST \
    ((BT_DEFAULT_PAGE_SCAN_INTERVAL != 0) && \
    ((BT_DEFAULT_PAGE_SCAN_INTERVAL < 0x0012) || \
     (BT_DEFAULT_PAGE_SCAN_INTERVAL > 0x1000)))

#if BT_PSI_TEST
#error BT_DEFAULT_PAGE_SCAN_INTERVAL is not legal
#endif

#define BT_ISW_TEST \
    ((BT_DEFAULT_INQ_SCAN_WINDOW != 0) && \
    ((BT_DEFAULT_INQ_SCAN_WINDOW < 0x0012) || \
     (BT_DEFAULT_INQ_SCAN_WINDOW > 0x1000)))

#if BT_ISW_TEST
#error BT_DEFAULT_INQ_SCAN_WINDOW is not legal
#endif

#define BT_ISI_TEST \
    ((BT_DEFAULT_INQ_SCAN_INTERVAL != 0) && \
    ((BT_DEFAULT_INQ_SCAN_INTERVAL < 0x0012) || \
     (BT_DEFAULT_INQ_SCAN_INTERVAL > 0x1000)))

#if BT_ISI_TEST
#error BT_DEFAULT_INQ_SCAN_INTERVAL is not legal
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_ACCESS_MODE_NC constant
 *
 *     Specifies the default accessibility mode when not-connected that
 *     the ME should set the radio to during initialization. This value
 *     must be of type BtAccessibleMode, found in me.h.
 */
#ifndef BT_DEFAULT_ACCESS_MODE_NC
#define BT_DEFAULT_ACCESS_MODE_NC  BAM_GENERAL_ACCESSIBLE
#endif /* BT_DEFAULT_ACCESS_MODE_NC */

/*---------------------------------------------------------------------------
 * BT_DEFAULT_ACCESS_MODE_C constant
 *
 *     Specifies the default accessibility mode when connected that the
 *     ME should set the radio to during initialization. This value
 *     must be of type BtAccessibleMode, found in me.h.
 *
 *     This default is applied only when BT_ALLOW_SCAN_WHILE_CON is XA_ENABLED.
 */
#ifndef BT_DEFAULT_ACCESS_MODE_C
#define BT_DEFAULT_ACCESS_MODE_C  BAM_NOT_ACCESSIBLE
#endif /* BT_DEFAULT_ACCESS_MODE_C */

/*---------------------------------------------------------------------------
 * BT_HCI_NUM_INIT_RETRIES constant
 *
 *     Specifies the number of times the Radio Manager should retry the 
 *     initialization of HCI when it fails to initialize.
 */
#ifndef BT_HCI_NUM_INIT_RETRIES
#define BT_HCI_NUM_INIT_RETRIES 0xFF
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_PAGE_TIMEOUT constant
 *
 *     The BT_DEFAULT_PAGE_TIMEOUT constant is the maximum time the
 *     local Link Manager will wait for a response from the remote
 *     device on a connection attempt. The default is 0x2000 = 5.12 sec.
 */
#ifndef BT_DEFAULT_PAGE_TIMEOUT
#define BT_DEFAULT_PAGE_TIMEOUT 0x2000
#endif

/*---------------------------------------------------------------------------
 * BT_PACKET_HEADER_LEN constant
 *
 *     The BT_PACKET_HEADER_LEN constant is the size of the BtPacket header.
 *     The default size is the max(L2CAP) header size (8 bytes), plus the
 *     max(RFCOMM,SDP,TCS) header size (6 bytes). If BNEP is used, you must
 *     increase the BT_PACKET_HEADER_LEN by 5 bytes: (8+6+5) in your config
 *     overide file.
 */
#ifndef BT_PACKET_HEADER_LEN
#define BT_PACKET_HEADER_LEN (8+6)
#endif

#if BT_PACKET_HEADER_LEN < 14 || BT_PACKET_HEADER_LEN > 255
#error BT_PACKET_HEADER_LEN must be greater than 13 and less than 256
#endif

/*---------------------------------------------------------------------------
 * NUM_KNOWN_DEVICES constant
 *
 *     Defines the maximum number of devices that the ME Device Selection
 *     manager can track. If this value is zero, the MEDEV component is
 *     disabled, resulting in a code size savings.
 */
#ifndef NUM_KNOWN_DEVICES
#define NUM_KNOWN_DEVICES       20
#endif

/*---------------------------------------------------------------------------
 * DS_NUM_SERVICES constant
 *
 *     This constant represents the maximum number of services that can
 *     be tracked within a BtDeviceContext. Each time a successful service
 *     search attribute request is performed, the UUID of the found service 
 *     is stored in the device context. This list of services is used for
 *     for selecting the best device for a specific service type. If this
 *     constant is defined to 0, service tracking is disabled and the
 *     supporting code is eliminated from the build.
 */
#ifndef DS_NUM_SERVICES
#define DS_NUM_SERVICES         4
#endif


/****************************************************************************
 *
 * Section: HCI Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HCI_RESET_TIMEOUT constant
 *
 *     Specifies the amount of time that the HCI will wait for a response
 *     to the RESET command before indicating an error to the Radio Manager.
 *     When the Radio Manager detects the error, it will attempt to reset
 *     the transports and the HCI again.
 *
 *     The default is 10 seconds.
 */
#ifndef HCI_RESET_TIMEOUT
#define HCI_RESET_TIMEOUT 10000
#endif

/*---------------------------------------------------------------------------
 * HCI_NUM_PACKETS constant
 *
 *     Represents the number of HCI packets allocated for sending data to
 *     the Host Controller. These packets are used for ACL data and
 *     SCO data. If the HCI layer runs out of packets, no more data or
 *     commands will be sent until the transport layer returns one.
 *
 *     The default number of packets is 2.
 */
#ifndef HCI_NUM_PACKETS
#define HCI_NUM_PACKETS  2
#endif


/*---------------------------------------------------------------------------
 * HCI_NUM_COMMANDS constant
 *
 *     Represents the number of HCI packets allocated for sending commands
 *     to the host controller.
 *
 *     The default number of packets is 1. 
 */
#ifndef HCI_NUM_COMMANDS
#define HCI_NUM_COMMANDS 1
#endif


/*---------------------------------------------------------------------------
 * HCI_NUM_EVENTS constant
 *
 *     Represents the number of HCI buffers allocated for handling events
 *     from the host controller.
 *
 *     The default number of packets is 2. 
 */
#ifndef HCI_NUM_EVENTS
#define HCI_NUM_EVENTS 2
#endif


/*---------------------------------------------------------------------------
 * HCI_CMD_PARM_LEN constant
 *
 *     Represents the largest size allowed for HCI command parameters.
 *
 *     The default command parameter length is 248.
 */
#ifndef HCI_CMD_PARM_LEN
#define HCI_CMD_PARM_LEN 248
#endif

/* The longest known HCI command is 248 bytes at this time.
 */
#if HCI_CMD_PARM_LEN < 248
#error HCI_CMD_PARM_LEN must be at least 248
#endif


/*---------------------------------------------------------------------------
 * HCI_HOST_FLOW_CONTROL constant
 *
 *     Controls whether HCI applies flow control to data received from the
 *     host controller.
 *
 *     When HCI_HOST_FLOW_CONTROL is enabled, the HCI layer uses
 *     HCC_HOST_NUM_COMPLETED_PACKET commands to periodically tell the
 *     radio hardware how many receive packets it can accept. This is
 *     necessary if the HCI driver's receive buffers could overflow
 *     with incoming data.
 *
 *     When HCI_HOST_FLOW_CONTROL is disabled, the HCI driver assumes
 *     that it can process data faster than the radio hardware can generate
 *     it.
 *
 *     By default, host-side flow control is disabled.
 */
#ifndef HCI_HOST_FLOW_CONTROL
#define HCI_HOST_FLOW_CONTROL XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * HCI_SCO_FLOW_CONTROL constant
 *
 *     Controls whether the host controller applies flow control to SCO data 
 *     sent by the host.
 *
 *     When HCI_SCO_FLOW_CONTROL is enabled, flow control is applied to SCO
 *     data transmitted to the host controller.
 * 
 *     When HCI_SCO_FLOW_CONTROL is disabled, no flow control mechanism is
 *     used when transmitting data to the host controller. 
 *
 *     By default, SCO flow control is disabled.
 */
#ifndef HCI_SCO_FLOW_CONTROL
#define HCI_SCO_FLOW_CONTROL XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * HCI_ALLOW_PRESCAN constant
 *
 *     Controls whether prescan handlers can be registered with HCI.  Prescan
 *     handlers allow receive and transmit data to be viewed or modified 
 *     before being sent or received by the stack.
 *
 *     When HCI_ALLOW_PRESCAN is enabled, the functions for registering
 *     prescan handlers are included in the code.
 *
 *     When HCI_ALLOW_PRESCAN is disabled, the functions for registering
 *     prescan handlers are not included in the code.
 */
#ifndef HCI_ALLOW_PRESCAN
#define HCI_ALLOW_PRESCAN XA_ENABLED
#endif

/*-------------------------------------------------------------------------
 * HCI_ACL_DATA_SIZE constant
 *
 *     Defines the maximum data size of a received ACL packet in the HCI
 *     layer. This value affects the size of receive buffers allocated
 *     by the HCI driver.
 *
 *     This value must be at least 255 and no more than 65531. The default
 *     is 800 bytes.
 */
#ifndef HCI_ACL_DATA_SIZE
#define HCI_ACL_DATA_SIZE 800
#endif

#if HCI_ACL_DATA_SIZE > 65531
#error HCI_ACL_DATA_SIZE cannot be greater than 65531
#endif

#if HCI_ACL_DATA_SIZE < 255
#error HCI_ACL_DATA_SIZE cannot be less than 255
#endif


/*-------------------------------------------------------------------------
 * HCI_SCO_DATA_SIZE constant
 *
 *     Defines the maximum data size of a received SCO packet in the HCI
 *     layer. This value affects the size of receive buffers allocated by
 *     the HCI driver. 
 *
 *     The SCO data size must be less than 256 bytes. The default size is
 *     255 bytes.
 */
#ifndef HCI_SCO_DATA_SIZE
#define HCI_SCO_DATA_SIZE 255
#endif

#if HCI_SCO_DATA_SIZE > 255
#error HCI_SCO_DATA_SIZE must be less than 256
#endif


/*-------------------------------------------------------------------------
 * HCI_NUM_ACL_BUFFERS constant
 *
 *     Defines the number of buffers allocated by the HCI driver
 *     for receiving ACL data.
 *
 *     By default, 8 ACL data buffers are allocated.
 */
#ifndef HCI_NUM_ACL_BUFFERS
#define HCI_NUM_ACL_BUFFERS      8
#endif

/*-------------------------------------------------------------------------
 * HCI_NUM_SCO_BUFFERS constant
 *
 *     Defines the number of buffers allocated by the HCI driver
 *     for receiving SCO data. If the number of SCO connections allowed
 *     (NUM_SCO_CONNS) is zero, the HCI does not need to
 *     allocate any SCO buffers.
 *
 *     By default, 8 SCO data buffers are allocated.
 */
#ifndef HCI_NUM_SCO_BUFFERS
#define HCI_NUM_SCO_BUFFERS      8
#endif

/****************************************************************************
 *
 * Section: L2CAP Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * L2CAP_NUM_PROTOCOLS constant
 *
 *     Defines the number of protocol services that may be layered above
 *     L2CAP.
 *
 *     Between 1 and 255 L2CAP protocol services are allowed. The default
 *     is three: one for the SDP client, one for the SDP server,
 *     and one for RFCOMM. 
 */
#ifndef L2CAP_NUM_PROTOCOLS
#define L2CAP_NUM_PROTOCOLS   3
#endif

#if L2CAP_NUM_PROTOCOLS < 1 || L2CAP_NUM_PROTOCOLS > 255
#error L2CAP_NUM_PROTOCOLS must be between 1 and 255
#endif


/*---------------------------------------------------------------------------
 * L2CAP_NUM_GROUPS constant
 *
 *     Defines the number of broadcast groups supported by L2CAP. When
 *     this value is zero, support for groups is removed from the stack,
 *     reducing code size.
 *
 *     Between 0 and 255 L2CAP groups are allowed. The default number of
 *     groups is zero.
 */
#ifndef L2CAP_NUM_GROUPS
#define L2CAP_NUM_GROUPS   0
#endif

#if L2CAP_NUM_GROUPS > 255
#error L2CAP_NUM_GROUPS must be no greater than 255
#endif


/*---------------------------------------------------------------------------
 * L2CAP_PING_SUPPORT constant
 *
 *     Controls whether the Ping operation (L2CAP_Ping) is supported.
 *     When disabled, the L2CAP_Ping() API is not included in the stack,
 *     resulting in code size savings.
 *
 *     By default, Ping support is enabled.
 */
#ifndef L2CAP_PING_SUPPORT
#define L2CAP_PING_SUPPORT      XA_ENABLED
#endif


/*---------------------------------------------------------------------------
 * L2CAP_GET_INFO_SUPPORT constant
 *
 *     Controls whether the GetInfo operation (L2CAP_GetInfo) is supported.
 *     When disabled, the L2CAP_GetInfo() API is not included in the stack,
 *     resulting in code size savings.
 *
 *     By default, GetInfo support is disabled.
 */
#ifndef L2CAP_GET_INFO_SUPPORT
#define L2CAP_GET_INFO_SUPPORT  XA_DISABLED
#endif


/*---------------------------------------------------------------------------
 * L2CAP_FLEXIBLE_CONFIG constant
 *
 *     Controls whether a L2CAP user is allowed to manage channel
 *     configuration options.
 *
 *     When L2CAP_FLEXIBLE_CONFIG is enabled, the application will receive
 *     events have access to functions which enable it to control the
 *     flow of configuration requests and responses.
 *
 *     When L2CAP_FLEXIBLE_CONFIG is disabled, configuration messages are
 *     handled internally by the protocol. The associated APIs are not
 *     included in the stack, resulting in code size savings.
 *
 *     By default, L2CAP flexible configuration is disabled.
 */
#ifndef L2CAP_FLEXIBLE_CONFIG
#define L2CAP_FLEXIBLE_CONFIG    XA_DISABLED
#endif


/*---------------------------------------------------------------------------
 * L2CAP_RTX_TIMEOUT constant
 *
 *     Defines the response timeout value for signaling commands, in
 *     seconds. If the timer elapses without a response from the remote
 *     device, the channel is disconnected.
 *
 *     The timeout value may range from 1 to 60 seconds. The default value
 *     is 30 seconds.
 */
#ifndef L2CAP_RTX_TIMEOUT
#define L2CAP_RTX_TIMEOUT     30
#endif

#if L2CAP_RTX_TIMEOUT < 1 || L2CAP_RTX_TIMEOUT > 60
#error L2CAP_RTX_TIMEOUT must be between 1 and 60 seconds.
#endif


/*---------------------------------------------------------------------------
 * L2CAP_ERTX_TIMEOUT constant
 *
 *     Defines the extended response timeout value for signaling commands,
 *     in seconds. Certain signaling commands (such as a Connect Response
 *     Pending) allow for a longer timeout period than the L2CAP_RTX_TIMEOUT
 *     value. When the ERTX timer elapses without a response, the channel
 *     is disconnected.
 *
 *     The extended timeout value may range from 60 to 300 seconds. The
 *     default value is 150 seconds.
 */
#ifndef L2CAP_ERTX_TIMEOUT
#define L2CAP_ERTX_TIMEOUT   150
#endif

#if L2CAP_ERTX_TIMEOUT < 60 || L2CAP_ERTX_TIMEOUT > 300
#error L2CAP_ERTX_TIMEOUT must be between 60 and 300 seconds.
#endif


/*---------------------------------------------------------------------------
 * L2CAP_FLOW_CONTROL constant
 *
 *     Enables support for L2CAP Flow Control and Retransmission modes as
 *     introduced in Bluetooth v1.2. When enabled, an L2CAP user is able to
 *     configure L2CAP connections to include support for retransmission
 *     of lost frames, SDU segmentation and reassembly and the detection of
 *     errored frames.
 *
 *     By default this option is disabled, which results in a code size 
 *     savings. This option should be enabled if L2CAP based protocols or 
 *     profiles are used which depend on this feature.
 */
#ifndef L2CAP_FLOW_CONTROL
#define L2CAP_FLOW_CONTROL      XA_DISABLED
#endif


/*---------------------------------------------------------------------------
 * L2CAP_MTU constant
 *
 *     Defines the largest receivable L2CAP data packet payload, in bytes.
 *     This limitation applies only to packets received from the remote device.
 *     When L2CAP_FLOW_CONTROL is enabled, channels requesting Flow & Error
 *     Control mode are not limited by this constant.
 *
 *     This constant also affects the L2CAP Connectionless MTU. Some profiles
 *     require a minimum Connectionless MTU that is greater than the L2CAP
 *     minimum of 48 bytes. When implementing such a profile it is important
 *     that the L2CAP_MTU constant be at least as large as the specified
 *     Connectionless MTU size.
 *
 *     This value may range from 48 to 65529. The default value is 672 bytes.
 */
#ifndef L2CAP_MTU
#define L2CAP_MTU   672
#endif 

#if L2CAP_MTU < 48 || L2CAP_MTU > 65529
#error L2CAP_MTU must be between 48 and 65529.
#endif

/*---------------------------------------------------------------------------
 * L2CAP_NUM_CHANNELS constant
 *
 *     Defines the number of L2CAP Channels, based on number of protocols and
 *     devices. This constant must be large enough to support all the channels
 *     that are needed by the upper layer protocols. There is typically one
 *     channel per protocol per supported device.
 */
#ifndef L2CAP_NUM_CHANNELS
/*
 * First calculate the number of L2CAP Auxiliary Channels. Auxiliary channels
 * are used to support the Echo (Ping) and Info (GetInfo) signalling procedures.
 */
#if (L2CAP_PING_SUPPORT == XA_ENABLED) && (L2CAP_GET_INFO_SUPPORT == XA_ENABLED)
#define AUX_CHANNELS    2
#elif (L2CAP_PING_SUPPORT == XA_ENABLED) || (L2CAP_GET_INFO_SUPPORT == XA_ENABLED)
#define AUX_CHANNELS    1
#else
#define AUX_CHANNELS    0
#endif 

#define L2CAP_NUM_CHANNELS    ((L2CAP_NUM_PROTOCOLS * NUM_BT_DEVICES) + AUX_CHANNELS)
#endif

#if L2CAP_NUM_CHANNELS  < 1
#error L2CAP_NUM_CHANNELS must be at least 1
#endif

/*---------------------------------------------------------------------------
 * L2CAP_DEREGISTER_FUNC constant
 *
 *     Controls whether the L2CAP_DeregisterPsm function is allowed. In
 *     cases where L2CAP services will be dynamically loaded or unloaded,
 *     this constant must be enabled.
 *
 *     By default, L2CAP service deregistration is disabled. Disabling this
 *     option results in code size savings.
 */
#ifndef L2CAP_DEREGISTER_FUNC
#define L2CAP_DEREGISTER_FUNC XA_DISABLED
#endif 


/*---------------------------------------------------------------------------
 * L2CAP_NUM_SIGNAL_PACKETS constant
 * 
 *     Defines the number of signaling packets allocated by L2CAP.
 *
 *     By default, L2CAP allocates two packets per device.
 */
#ifndef L2CAP_NUM_SIGNAL_PACKETS
#define L2CAP_NUM_SIGNAL_PACKETS    (NUM_BT_DEVICES * 2)
#endif

#if L2CAP_NUM_SIGNAL_PACKETS < 1 
#error L2CAP_NUM_SIGNAL_PACKETS must be at least 1
#endif

/*---------------------------------------------------------------------------
 * L2CAP_PRELUDE_SIZE constant
 * 
 *     Defines the number of bytes that are appended to the front of the
 *     L2CAP reassembly buffer. This is only recommended for use by
 *     protocols which are layered directly above L2CAP.
 *
 *     By default, L2CAP allocates zero extra space.
 */
#ifndef L2CAP_PRELUDE_SIZE
#define L2CAP_PRELUDE_SIZE  0
#endif

#if L2CAP_PRELUDE_SIZE+L2CAP_MTU > 65529
#error L2CAP_PRELUDE_SIZE plus L2CAP_MTU must be less than 65529.
#endif


/****************************************************************************
 *
 * Section: SDP Constants
 *
 ****************************************************************************/

/*-------------------------------------------------------------------------
 * SDP_CLIENT_SUPPORT constant
 *
 *     Controls whether the SDP client is included in the SDP protocol.  Set 
 *     this constant to XA_ENABLED when it is necessary to have the SDP client 
 *     compiled into your system.
 *
 *     By default, the SDP client is enabled.
 */
#ifndef SDP_CLIENT_SUPPORT
#define SDP_CLIENT_SUPPORT XA_ENABLED
#endif

/*-------------------------------------------------------------------------
 * SDP_SERVER_SUPPORT constant
 *
 *     Controls whether the SDP server is included in the SDP protocol.  Set 
 *     this constant to XA_ENABLED when it is necessary to have the SDP server 
 *     compiled into your system.
 *
 *     By default, the SDP server is enabled.
 */
#ifndef SDP_SERVER_SUPPORT
#define SDP_SERVER_SUPPORT XA_ENABLED
#endif

#if (SDP_SERVER_SUPPORT == XA_DISABLED) && (SDP_CLIENT_SUPPORT == XA_DISABLED)
#error SDP_CLIENT_SUPPORT and/or SDP_SERVER_SUPPORT must be enabled.
#endif

#if SDP_CLIENT_SUPPORT == XA_ENABLED
/*-------------------------------------------------------------------------
 * SDP_CLIENT_LOCAL_MTU constant
 *
 *     Defines the maximum L2CAP MTU used for the local SDP client.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU). 
 *     By default, this value is equal to the L2CAP_MTU size.
 */
#ifndef SDP_CLIENT_LOCAL_MTU
#define SDP_CLIENT_LOCAL_MTU L2CAP_MTU
#endif 

#if SDP_CLIENT_LOCAL_MTU < 48 || SDP_CLIENT_LOCAL_MTU > L2CAP_MTU
#error SDP_CLIENT_LOCAL_MTU must be between 48 and L2CAP_MTU.
#endif


/*-------------------------------------------------------------------------
 * SDP_CLIENT_MIN_REMOTE_MTU constant
 *
 *     Defines the minimum remote MTU size acceptable to the local
 *     SDP client. If the remote SDP server cannot support the specified
 *     MTU size, the connection attempt will fail and SDP operations will
 *     not occur.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU). 
 *     By default, this value is 48.
 */
#ifndef SDP_CLIENT_MIN_REMOTE_MTU
#define SDP_CLIENT_MIN_REMOTE_MTU 48
#endif

#if SDP_CLIENT_MIN_REMOTE_MTU < 48 || SDP_CLIENT_MIN_REMOTE_MTU > L2CAP_MTU
#error SDP_CLIENT_MIN_REMOTE_MTU must be between 48 and L2CAP_MTU.
#endif

/*-------------------------------------------------------------------------
 * SDP_PARSING_FUNCS constant
 *
 *     Controls whether the SDP response parsing is included and made 
 *     available to the application.  Set this constant to XA_ENABLED
 *     when it is necessary to have SDP response parsing available.
 *
 *     By default, SDP parsing is enabled.
 *
 */
#ifndef SDP_PARSING_FUNCS
#define SDP_PARSING_FUNCS XA_ENABLED
#endif

#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */

#if SDP_SERVER_SUPPORT == XA_ENABLED
/*-------------------------------------------------------------------------
 * SDP_NUM_CLIENTS constant
 *
 *     Defines the maximum number of clients that can connect
 *     simultaneously to the SDP server.
 *
 *     This value may range from 1 to the number of possible devices
 *     (NUM_BT_DEVICES). The default is NUM_BT_DEVICES.
 */
#ifndef SDP_NUM_CLIENTS
#define SDP_NUM_CLIENTS NUM_BT_DEVICES
#endif 

#if (SDP_NUM_CLIENTS < 1) || (SDP_NUM_CLIENTS > NUM_BT_DEVICES)
#error SDP_NUM_CLIENTS must be greater than 0 and less than or equal to NUM_BT_DEVICES
#endif

/*-------------------------------------------------------------------------
 * SDP_ACTIVE_CLIENTS constant
 *
 *     Defines the maximum number of active clients that can connect
 *     simultaneously to the SDP server. Active clients are those that
 *     are not in park.
 *
 *     This value may range from 1 to the number of possible active devices
 *     (16). The default value is NUM_BT_DEVICES.
 */
#ifndef SDP_ACTIVE_CLIENTS
#if NUM_BT_DEVICES < 16
#define SDP_ACTIVE_CLIENTS  (NUM_BT_DEVICES)
#else
#define SDP_ACTIVE_CLIENTS  16
#endif
#endif 

#if (SDP_ACTIVE_CLIENTS < 1) || (SDP_ACTIVE_CLIENTS > NUM_BT_DEVICES) || (SDP_ACTIVE_CLIENTS > 16)
#error SDP_ACTIVE_CLIENTS must be greater than 0 and less than or equal to NUM_BT_DEVICES and 16.
#endif

/*-------------------------------------------------------------------------
 * SDP_SERVER_SEND_SIZE constant
 * 
 *     Defines the maximum number of bytes that the server can send in a
 *     single response.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU). The
 *     default is 128.
 */
#ifndef SDP_SERVER_SEND_SIZE
#define SDP_SERVER_SEND_SIZE 128
#endif

#if (SDP_SERVER_SEND_SIZE < 48) || (SDP_SERVER_SEND_SIZE > L2CAP_MTU)
#error SDP_SERVER_SEND_SIZE must be between 48 and L2CAP_MTU
#endif


/*-------------------------------------------------------------------------
 * SDP_SERVER_MAX_LEVEL constant
 *
 *     Defines the maximum number of nested data element sequences in
 *     an SDP attribute. SDP uses this value internally when examining
 *     attributes. If the number of nested data elements exceeds
 *     SDP_SERVER_MAX_LEVEL, certain SDP functions will return errors.
 *
 *     The default value is four, which allows for typical attributes.
 */
#ifndef SDP_SERVER_MAX_LEVEL
#define SDP_SERVER_MAX_LEVEL  4
#endif

#if SDP_SERVER_MAX_LEVEL < 4
#error SDP_SERVER_MAX_LEVEL must be at least 4
#endif


/*-------------------------------------------------------------------------
 * SDP_SERVER_LOCAL_MTU constant
 *
 *     Defines the maximum L2CAP MTU used for the local SDP server.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU). 
 *     By default, this value is equal to the L2CAP_MTU size.
 */
#ifndef SDP_SERVER_LOCAL_MTU
#define SDP_SERVER_LOCAL_MTU L2CAP_MTU
#endif 

#if SDP_SERVER_LOCAL_MTU < 48 || SDP_SERVER_LOCAL_MTU > L2CAP_MTU
#error SDP_SERVER_LOCAL_MTU must be between 48 and L2CAP_MTU.
#endif


/*-------------------------------------------------------------------------
 * SDP_SERVER_MIN_REMOTE_MTU constant
 *
 *     Defines the minimum remote MTU size acceptable to the local
 *     SDP server. If the remote SDP client cannot support the specified
 *     MTU size, the connection attempt will fail and SDP operations will
 *     not occur.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU).
 *     By default, this value is 48.
 */
#ifndef SDP_SERVER_MIN_REMOTE_MTU
#define SDP_SERVER_MIN_REMOTE_MTU 48
#endif 

#if SDP_SERVER_MIN_REMOTE_MTU < 48 || SDP_SERVER_MIN_REMOTE_MTU > L2CAP_MTU
#error SDP_SERVER_MIN_REMOTE_MTU must be between 48 and L2CAP_MTU.
#endif

#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */


/****************************************************************************
 *
 * Section: RFCOMM Constants
 *
 ****************************************************************************/


/*-------------------------------------------------------------------------
 * RFCOMM_PROTOCOL constant
 *
 *     Controls whether the RFCOMM protocol layer will be included.  Set 
 *     this constant to XA_ENABLED when it is necessary to have RFCOMM 
 *     compiled into your system.  This is necessary for all profiles based on 
 *     the serial profile.
 *
 *     By default, RFCOMM is enabled.
 */
#ifndef RFCOMM_PROTOCOL
#define RFCOMM_PROTOCOL XA_ENABLED
#endif

#if RFCOMM_PROTOCOL == XA_ENABLED

/*-------------------------------------------------------------------------
 * RF_SECURITY constant
 *
 *     Controls whether RFCOMM protocol layer security is enabled.
 *     This operates in conjunction with BT_SECURITY.  If BT_SECURITY is
 *     enabled, then an application can enforce different levels of
 *     security from the Bluetooth stack.  Level 2 security includes
 *     the ability to trigger security on an RFCOMM service ID.  If 
 *     RF_SECURITY is disabled, then code to perform RFCOMM security
 *     checks is excluded.  It is possible to enable BT_SECURITY but 
 *     disable RF_SECURITY.  For example, if an application wishes to use 
 *     security (BT_SECURITY), but will not use RFCOMM level security, 
 *     then it should enable BT_SECURITY and disable RF_SECURITY to 
 *     minimize code size.  If BT_SECURITY is disabled, then RF_SECURITY will 
 *     be disabled as well.
 * 
 *     By default, RF_SECURITY inherits the state of BT_SECURITY.
 */
#ifndef RF_SECURITY
#define RF_SECURITY BT_SECURITY
#endif

#if BT_SECURITY != XA_ENABLED
#if RF_SECURITY == XA_ENABLED
#error RF_SECURITY requires BT_SECURITY
#endif /* RF_SECURITY */
#endif /* BT_SECURITY */

/*-------------------------------------------------------------------------
 * RF_SEND_TEST constant
 *
 *     Controls whether the stack offers support for sending an RFCOMM
 *     test. When enabled, the RF_SendTest API can be used to test
 *     the connection with the remote device.
 *
 *     By default, sending RF tests is disabled.
 */
#ifndef RF_SEND_TEST
#define RF_SEND_TEST XA_DISABLED
#endif

/*-------------------------------------------------------------------------
 * RF_SEND_CONTROL constant
 *
 *     Controls whether RFCOMM can be used to send control channel
 *     information. This includes modem status, line status, and negotiation
 *     of remote port settings.
 *
 *     If RF_SEND_CONTROL is enabled, control channel information can
 *     be sent and processed by the application. This includes support for
 *     the following events and APIs.
 *
 *     Events: RFEVENT_MODEM_STATUS_IND, RFEVENT_MODEM_STATUS_CNF,
 *     RFEVENT_LINE_STATUS_IND, RFEVENT_LINE_STATUS_CNF,
 *     RFEVENT_PORT_STATUS_CNF, RFEVENT_PORT_NEG_CNF.
 *
 *     APIs: RF_SetModemStatus, RF_SetLineStatus, RF_RequestPortSettings,
 *     RF_AcceptPortSettings, RF_RequestPortStatus, and RF_SendPortStatus.
 *
 *     If RF_SEND_CONTROL is disabled, RFCOMM will handle control channel
 *     requests received from the remote device, but will not send control
 *     channel requests to the remote device. The APIs and Events listed
 *     above are disabled, resulting in significant code size savings.
 *
 *     By default, sending control channel information is enabled.
 */
#ifndef RF_SEND_CONTROL
#define RF_SEND_CONTROL XA_ENABLED
#endif

/*-------------------------------------------------------------------------
 * NUM_RF_SERVERS constant
 *
 *     Defines the number of RFCOMM services that can be registered with
 *     RFCOMM_RegisterServerChannel.
 *
 *     This value may range from 0 to 30. The default is 4.
 */
#ifndef NUM_RF_SERVERS
#define NUM_RF_SERVERS 4
#endif

#if NUM_RF_SERVERS >= 0
#if NUM_RF_SERVERS > 30
#error NUM_RF_SERVERS must not exceed 30
#endif
#else
#error NUM_RF_SERVERS must be 0 or more
#endif


/*-------------------------------------------------------------------------
 * NUM_RF_CHANNELS constant
 *
 *     Defines the maximum number of RFCOMM channels that can be
 *     established to each device. This includes both server and client 
 *     channels.
 *
 *     The default number of channels is twice the number of local
 *     servers.
 */
#ifdef NUM_RF_CHANNELS

#if (NUM_RF_CHANNELS < NUM_RF_SERVERS) || (NUM_RF_CHANNELS > 60)
#error NUM_RF_CHANNELS value is invalid
#endif

#else

#if NUM_RF_SERVERS > 0
#define NUM_RF_CHANNELS (NUM_RF_SERVERS * 2)
#else
#define NUM_RF_CHANNELS 1
#endif

#endif

#if !(NUM_RF_CHANNELS > 0)
#error NUM_RF_CHANNELS must be greater than 0
#endif


/*-------------------------------------------------------------------------
 * RF_MAX_FRAME_SIZE constant
 *
 *     Defines the maximum frame size for an RFCOMM channel. All registered
 *     RfChannel structures must have a "maxFrameSize" equal to or less
 *     than this value.
 *
 *     This value may range between 127 bytes and 32767 bytes (or the
 *     L2CAP_MTU size minus 5, whichever is lower). The default maximum
 *     frame size is the L2CAP_MTU size, minus five bytes for RFCOMM
 *     protocol information.
 */
#ifndef RF_MAX_FRAME_SIZE
#define RF_MAX_FRAME_SIZE (L2CAP_MTU - 5)
#endif

#if RF_MAX_FRAME_SIZE > (L2CAP_MTU - 5)
#error RF_MAX_FRAME_SIZE cannot exceed the L2CAP_MTU - 5
#endif

#if RF_MAX_FRAME_SIZE < 127
#error RF_MAX_FRAME_SIZE cannot be less than 127
#endif

#if RF_MAX_FRAME_SIZE > 32767
#error RF_MAX_FRAME_SIZE cannot exceed 32767
#endif


/*-------------------------------------------------------------------------
 * RF_CONNECT_TIMEOUT constant
 *
 *     Defines the amount of time to wait for a response to a connect
 *     request for an RFCOMM channel.  Because it is not possible to know
 *     whether authentication is required, this value must allow time for
 *     user interaction.
 * 
 *     This value may range between 60000 and 300000 (60 sec and 300 sec).
 *     The default time is 60 seconds.
 */
#ifndef RF_CONNECT_TIMEOUT
#define RF_CONNECT_TIMEOUT 60000
#endif

#if RF_CONNECT_TIMEOUT < 60000
#error RF_CONNECT_TIMEOUT cannot be less than 60000
#endif

#if RF_CONNECT_TIMEOUT > 300000
#error RF_CONNECT_TIMEOUT cannot be greater than 300000
#endif

/*-------------------------------------------------------------------------
 * RF_T1_TIMEOUT constant
 *
 *     Defines the amount of time to wait for a response to an RFCOMM command.
 *     If a response is not received within the allotted time, the RFCOMM
 *     multiplexer session will be torn down.
 *
 *     This value may range between 10000 and 60000 (10 sec and 60 sec).
 *     The default time is 20 seconds.
 */
#ifndef RF_T1_TIMEOUT
#define RF_T1_TIMEOUT 20000
#endif

#if RF_T1_TIMEOUT < 10000
#error RF_T1_TIMEOUT cannot be less than 10000
#endif

#if RF_T1_TIMEOUT > 60000
#error RF_T1_TIMEOUT cannot be greater than 60000
#endif

/*-------------------------------------------------------------------------
 * RF_T2_TIMEOUT constant
 *
 *     Defines the amount of time to wait for a response to an RFCOMM
 *     control channel command.  If a response is not received within the
 *     allotted time, the RFCOMM multiplexer session will be torn down.
 *
 *     This value may range between 1000 and 60000 (1 sec and 60 sec).
 *     The default time is 20 seconds.
 */
#ifndef RF_T2_TIMEOUT
#define RF_T2_TIMEOUT 20000
#endif

#if RF_T2_TIMEOUT < 1000
#error RF_T2_TIMEOUT cannot be less than 1000
#endif

#if RF_T2_TIMEOUT > 60000
#error RF_T2_TIMEOUT cannot be greater than 60000
#endif

#endif /* RFCOMM_PROTOCOL == XA_ENABLED */


/* Internal flag for enable/disable of low level tests */
#ifndef JETTEST
#define JETTEST XA_DISABLED
#endif /* JETTEST */

/****************************************************************************
 *
 * Section: UnplugFest Testing
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * UPF_TWEAKS constant
 *
 *     This value controls the inclusion of code which enables certain
 *     behaviors for UnplugFest interoperability testing.  Certain UnplugFest
 *     tests require functionality or behaviors that may not be included in 
 *     the Blue SDK.  Set this value to XA_ENABLED to include this code.
 *
 *     This constant should always be set to XA_DISABLED, except when
 *     performing UnplugFest tests.
 */
#ifndef UPF_TWEAKS
#define UPF_TWEAKS XA_DISABLED
#endif /* UPF_TWEAKS */


#endif /* __BTCONFIG_H */


