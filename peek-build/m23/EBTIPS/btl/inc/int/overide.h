/****************************************************************************
 *
 * File:
 *     $Workfile: overide.h $ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: 7 $
 *
 * Description:
 *     Configuration overrides for the qbeam project.
 *
 * Copyright 1999-2002 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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

/*******************************************************************************\
*     The constants in this file configure the layers of ESI code (stack, profiles, etc).
*      package.
*
*	If an existing ESI parameter should be available for BTIPS users for configuration, then a
*	corresponding parameters should be defined in btl_config.h. Then, the ESI parameter
*	should be defined in terms of the BTL parameter.
*
*	ESI constants that should not be configured by BTIPS users should have a specific value
*	explicitly defined in this file.
*
*   AUTHOR:   Udi Ron
*
*****************************************************************************/
#ifndef __OVERIDE_H
#define __OVERIDE_H

#include "btl_configi.h"

#define XA_DISABLED 									(BTL_CONFIG_DISABLED)   /* Disable the feature */
#define XA_ENABLED  									(BTL_CONFIG_ENABLED )  /* Enable the feature */

#define TI_CHANGES          XA_ENABLED

/*---------------------------------------------------------------------------
 * XA_ERROR_CHECK constant
 *
 *     Controls whether error checking code is compiled into the system.
 *
 *     When XA_ERROR_CHECK is enabled, error checking code protects the
 *     stack API from illegal usage by verifying the entrance conditions
 *     for API call. This includes checking to make sure the API was called
 *     at the correct time, and checking each parameter for correctness
 *     before proceeding with any other processing.
 *
 *     If an API returns a value marked XA_ERROR_CHECK only, the calling
 *     application has made a defective call which must be fixed.
 *
 *     When XA_ERROR_CHECK is disabled, error checking code is not
 *     compiled into the system. This means that invalid API calls may
 *     cause strange errors to occur. 
 *
 *     Disabling error checking will result in significant code size savings,
 *     but it must only be disabled if you are confident that applications
 *     will not call stack APIs at inappropriate times or with invalid
 *     parameters.
 *
 *     By default, error checking is enabled.
 */
#define XA_ERROR_CHECK 								XA_ENABLED

/*---------------------------------------------------------------------------
 * XA_DEBUG constant
 *
 *     When XA_DEBUG is enabled, debugging code is enabled throughout the
 *     stack, including OS_Report and OS_Assert calls and other code
 *     that verifies the proper behavior of the stack at run-time.
 *
 *     When XA_DEBUG is disabled, debugging code is not compiled into the
 *     stack. Asserts and diagnostic output are removed. This results
 *     in a significant code size savings.
 *
 *     You can specify the XA_DEBUG setting in the overide.h file. If it is
 *     not specified there, XA_DEBUG depends on the XADEBUG or _DEBUG
 *     constants defined at compile time. If XADEBUG or _DEBUG constants are
 *     defined, XA_DEBUG will be enabled. If the constants are not defined,
 *     XA_DEBUG will be disabled.
 */
#define XA_DEBUG			XA_ENABLED

/*---------------------------------------------------------------------------
 * XA_INTEGER_SIZE constant
 *
 *     Defines the preferred width of counter variables. In some systems,
 *     accesses to 2- or 4-byte variables is more time efficient than other
 *     sizes, even though they may take up more RAM. The stack attempts to
 *     honor this preference, except where a greater size is required.
 *
 *     For instance, if XA_INTEGER_SIZE is defined as 2, 8-bit and 16-bit
 *     counters will both be defined as 16-bit variables.
 *
 *     XA_INTEGER_SIZE may be 1, 2, or 4 and will affect the definition
 *     of I8, I16 types used internally by the stack. By default, 2-byte
 *     integers are preferred.
 */
#define XA_INTEGER_SIZE  								BTHAL_INT_SIZE

/*---------------------------------------------------------------------------
 * XA_CONTEXT_PTR constant
 *
 *     Controls whether stack RAM is allocated dynamically or statically.
 *     All RAM used by stack components is kept in a stack context structure.
 *
 *     When XA_CONTEXT_PTR is enabled, accesses to the context structure
 *     are performed using the "->" operand. This is required when
 *     the context structure is allocated dynamically (at run time). 
 *
 *     When XA_CONTEXT_PTR is disabled, accesses to the context structure
 *     are performed using the more efficient "." operand. This is allowed
 *     only if the context structure is allocated statically (at load time).
 *     This is more time- and ROM-efficient because references to stack
 *     variables are resolved at compile time rather than run time.
 *
 *     Internally, all accesses to the stack context are protected through
 *     special macros. These macros are defined based on this configuration
 *     constant.
 *
 *     By default, XA_CONTEXT_PTR is disabled.
 */
#define XA_CONTEXT_PTR 		XA_DISABLED

/*---------------------------------------------------------------------------
 * XA_LOAD_LIST constant
 */ 
#define XA_LOAD_LIST        								(BTL_CONFIGI_INIT_DECLARATIONS)

/*---------------------------------------------------------------------------
 * XA_SNIFFER constant
 *
 *     Controls whether the protocol sniffer hooks in various protocol layers
 *     are compiled into the system. The sniffer uses an endpoint registration
 *     function to manage a connection table which is used as a guide when 
 *     decoding data packets. When enabled, the function SnifferRegisterEndpoint
 *     as described in debug.h must be implemented. When disabled, the sniffer
 *     functionality can still be accessed, but protocol specific decoding will
 *     stop at the L2CAP layer.
 *
 *     By default, this option is disabled.
 */
#define XA_SNIFFER										(BTL_CONFIG_SNIFFER)

/*---------------------------------------------------------------------------
 * XA_STATISTICS constant
 *
 *     Controls internal statistics-gathering macros. When enabled,
 *     certain stack components keep track of resource usage and timing.
 *     To extract this information, a module in your system must include
 *     /inc/sys/btstats.h and call BTSTAT_Report, which will send a
 *     set of statistics to debug output.
 *
 *     For more information about the meaning of each statistic, see
 *     the Blue SDK Implementer's Guide.
 *
 *     By default, statistics-gathering is disabled to save code size.
 *     
 * Requires:
 *     XA_DEBUG enabled.
 */ 
#define XA_STATISTICS           								XA_DISABLED

/*---------------------------------------------------------------------------
 * BT_STACK constant
 *
 *     Controls whether Bluetooth-specific functionality is compiled into
 *     the system. This option is used in systems that can accommodate
 *     multiple transport mechanisms.
 */
#define BT_STACK            XA_ENABLED

/*---------------------------------------------------------------------------
 * IRDA_STACK constant
 *
 *     Controls whether IrDA-specific functionality is compiled into the
 *     system. This option is used in systems that can accommodate multiple
 *     transport mechanisms.
 *
 *     By default, the IrDA stack is disabled.
 */
#define IRDA_STACK          XA_DISABLED

/*---------------------------------------------------------------------------
 * TCP_STACK constant
 *
 *     Controls whether TCP-specific functionality is compiled into the
 *     system. This option is used in systems that can accommodate multiple
 *     transport mechanisms.
 *
 *     By default, the IrDA stack is disabled.
 */
#define TCP_STACK           								XA_DISABLED

/*---------------------------------------------------------------------------
 * FM_STACK constant
 */
#define FM_STACK            									(BTL_CONFIG_FM_STACK)

/*---------------------------------------------------------------------------
 * STACK_BT_VERSION constant
 */
#define STACK_BT_VERSION		2

/*---------------------------------------------------------------------------
 * TRANSPORT_INET constant
 */
#define TRANSPORT_INET 									(BTL_CONFIG_TRANSPORT_INET)


/*---------------------------------------------------------------------------
 * BT_DEFAULT_ACCESS_MODE_C constant
 *
 *     Specifies the default accessibility mode when connected that the
 *     ME should set the radio to during initialization. This value
 *     must be of type BtAccessibleMode, found in me.h.
 *
 *     This default is applied only when BT_ALLOW_SCAN_WHILE_CON is XA_ENABLED.
 */
#define BT_DEFAULT_ACCESS_MODE_C  					BAM_GENERAL_ACCESSIBLE

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
#define BT_ALLOW_SCAN_WHILE_CON						XA_ENABLED

/*---------------------------------------------------------------------------
 * BT_SECURITY constant
 *
 *     Determines whether code to handle BT security features is included. 
 *     This constant is typically enabled, but may be disabled for minimal
 *     applications that don't need security.
 *
 *     By default, this constant is enabled.
 */
#define BT_SECURITY						XA_ENABLED

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
#define BT_SECURITY_TIMEOUT 							(BTL_CONFIG_SECURITY_TIMEOUT)

/* Default sec Bluetooth Security Architecture White Paper: */
/* Incoming Connection - Authorization + Authentication; Outgoing connection - Authentication */
#define BT_DEFAULT_SECURITY							XA_DISABLED

/*---------------------------------------------------------------------------
 * NUM_BT_DEVICES constant
 *
 *     The number of devices with which we can connect. This value
 *     represents the maximum size of the piconet in which this device is
 *     the master, plus any master devices for which we are concurrently a
 *     slave. This value includes devices which may be parked, holding,
 *     or sniffing as well any active devices.
 */
#define NUM_BT_DEVICES  								(BTL_CONFIG_MAX_NUM_OF_CONNECTED_DEVICES)


/*---------------------------------------------------------------------------
 * NUM_KNOWN_DEVICES constant
 *
 *     Defines the maximum number of devices that the ME Device Selection
 *     manager can track. If this value is zero, the MEDEV component is
 *     disabled, resulting in a code size savings.
 */
#define NUM_KNOWN_DEVICES 							(BTL_CONFIG_MAX_NUM_KNOWN_DEVICES)

/*---------------------------------------------------------------------------
 * BT_DEFAULT_PAGE_TIMEOUT constant
 *
 *     The BT_DEFAULT_PAGE_TIMEOUT constant is the maximum time the
 *     local Link Manager will wait for a response from the remote
 *     device on a connection attempt. 
 *
 *	Set to 10.24 seconds
 */
#define BT_DEFAULT_PAGE_TIMEOUT 0x4000

/* ==================================================
						HCI definitions 
    ==================================================*/
    
/* [ToDo][UdiR] -  Check these with Ilan - may depend on the chip used / mode (edr) */

/*-------------------------------------------------------------------------
 * HCI_NUM_ACL_BUFFERS constant
 *
 *     Defines the number of buffers allocated by the HCI driver
 *     for receiving ACL data.
 *
 *     By default, 8 ACL data buffers are allocated.
 */
#define HCI_NUM_ACL_BUFFERS			35

/*-------------------------------------------------------------------------
 * HCI_NUM_SCO_BUFFERS constant
 *
 *     Defines the number of buffers allocated by the HCI driver
 *     for receiving SCO data. If the number of SCO connections allowed
 *     (NUM_SCO_CONNS) is zero, the HCI does not need to
 *     allocate any SCO buffers.
 *
 *     By default, 1 SCO data buffer is allocated, since voice over HCI is
 *	   not supported.
 */
#define HCI_NUM_SCO_BUFFERS			1

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
#define HCI_ACL_DATA_SIZE				339

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
#define HCI_NUM_PACKETS				20

/*---------------------------------------------------------------------------
 * HCI_MAX_TX_BUFFERS_FOR_BEST_EFFORT_CON_WHEN_NO_GUARANTEED constant
 *
 *     Represents the maximum number of HCI Tx buffers avaliable for  
 *	   'best efort' connections when no 'guaranteed' connection exist
 *
 *     The default number is 4 (Max exist in baseband).
 */
#define HCI_MAX_TX_BUFFERS_FOR_BEST_EFFORT_CON_WHEN_NO_GUARANTEED			4

/*---------------------------------------------------------------------------
 * HCI_MAX_TX_BUFFERS_FOR_BEST_EFFORT_CON_WHEN_ONE_GUARANTEED constant
 *
 *     Represents the maximum number of HCI Tx buffers avaliable for  
 *	   'best efort' connections when one 'guaranteed' connection exist
 *
 *     The default number is 1.
 */
#define HCI_MAX_TX_BUFFERS_FOR_BEST_EFFORT_CON_WHEN_ONE_GUARANTEED			1

/*---------------------------------------------------------------------------
 * HCI_MAX_TX_BUFFERS_FOR_BEST_EFFORT_CON_WHEN_MULTI_GUARANTEED constant
 *
 *     Represents the maximum number of HCI Tx buffers avaliable for  
 *	   'best efort' connections when multiple 'guaranteed' connection exist
 *
 *     The default number is 1.
 */
#define HCI_MAX_TX_BUFFERS_FOR_BEST_EFFORT_CON_WHEN_MULTI_GUARANTEED		1

/*---------------------------------------------------------------------------
 * HCI_MAX_TX_BUFFERS_FOR_GUARANTEED_CON_WHEN_ONE_GUARANTEED constant
 *
 *     Represents the maximum number of HCI Tx buffers avaliable for  
 *	   'guaranteed' connections when only one exist and 'best effort' might
 *	   exist
 *
 *     The default number is 4 (Max exist in baseband).
 */
#define HCI_MAX_TX_BUFFERS_FOR_GUARANTEED_CON_WHEN_ONE_GUARANTEED			4

/*---------------------------------------------------------------------------
 * HCI_MAX_TX_BUFFERS_FOR_GUARANTEED_CON_WHEN_MULTI_GUARANTEED constant
 *
 *     Represents the maximum number of HCI Tx buffers avaliable for  
 *	   one 'guaranteed' connections when more then one exist
 *
 *     The default number is 2 (Max exist in baseband).
 */
#define HCI_MAX_TX_BUFFERS_FOR_GUARANTEED_CON_WHEN_MULTI_GUARANTEED			2

/*---------------------------------------------------------------------------
 * HCI_QOS constant
 *
 *     Enable to ensure HCI QOS
 *
 */
#define HCI_QOS										XA_ENABLED							

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
#define BT_SCO_HCI_DATA								XA_DISABLED

/*---------------------------------------------------------------------------
 * BT_HCI_NUM_INIT_RETRIES constant
 *
 *     Specifies the number of times the Radio Manager should retry the 
 *     initialization of HCI when it fails to initialize.
 */
#define BT_HCI_NUM_INIT_RETRIES 			0x05

/*---------------------------------------------------------------------------
 * HCI_NUM_EVENTS constant
 *
 *     Represents the number of HCI buffers allocated for handling events
 *     from the host controller.
 *
 *     The default number of packets is 2. 
 */
#define HCI_NUM_EVENTS 				10

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
#define HCI_ALLOW_PRESCAN   							(BTL_CONFIG_SNIFFER)

/* ==================================================
						L2CAP definitions 
    ==================================================*/

/*---------------------------------------------------------------------------
 * L2CAP_NUM_PROTOCOLS constant
 *
 *     Defines the number of protocol services that may be layered above
 *     L2CAP.
 *
 *     Between 1 and 255 L2CAP protocol services are allowed.
 *
 *	The number should accomodate:
 *	1. SDP client: 				1
 *	2. SDP server:				1
 *	3. RFCOMM					1
 *	4. Profiles that register PSMs internally (TBD: Calculate according to included profiles)
 *	5. Max num of PSMs allowed for BTL_L2CAP
 *
 *	[@ToDo][UdiR] - Calculate the number
 */
#define L2CAP_NUM_PROTOCOLS							12

/*---------------------------------------------------------------------------
 * L2CAP_PING_SUPPORT constant
 *
 *     Controls whether the Ping operation (L2CAP_Ping) is supported.
 *     When disabled, the L2CAP_Ping() API is not included in the stack,
 *     resulting in code size savings.
 *
 *     By default, Ping support is enabled.
 */
#define L2CAP_PING_SUPPORT      							XA_ENABLED

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
#define L2CAP_MTU   										(BTL_CONFIG_L2CAP_MTU)

/*---------------------------------------------------------------------------
 * L2CAP_PRELUDE_SIZE constant
 * 
 *     Defines the number of bytes that are appended to the front of the
 *     L2CAP reassembly buffer. This is only recommended for use by
 *     protocols which are layered directly above L2CAP.
 *
 *     By default, L2CAP allocates zero extra space.
 */
#define L2CAP_PRELUDE_SIZE								7

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
#define L2CAP_DEREGISTER_FUNC      						XA_ENABLED

/*---------------------------------------------------------------------------
 * L2CAP_GET_INFO_SUPPORT constant
 *
 *     Controls whether the GetInfo operation (L2CAP_GetInfo) is supported.
 *     When disabled, the L2CAP_GetInfo() API is not included in the stack,
 *     resulting in code size savings.
 *
 *     By default, GetInfo support is disabled.
 */
#define L2CAP_GET_INFO_SUPPORT  						XA_ENABLED

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
#define L2CAP_FLEXIBLE_CONFIG   						XA_DISABLED

/*---------------------------------------------------------------------------
 * L2CAP_NUM_GROUPS constant
 *
 *     Defines the number of broadcast groups supported by L2CAP. When
 *     this value is zero, support for groups is removed from the stack,
 *     reducing code size.
 *
 *     Between 0 and 255 L2CAP groups are allowed. The default number of
 *     groups is zero.
 *
 *	Set to 0 since we are not using groups
 */
#define L2CAP_NUM_GROUPS        							0

/*---------------------------------------------------------------------------
 * L2CAP_PACKET_PRIORITIZATION constant
 *
 * [ToDo][UdiR] - Ask Gili about this
 */
#define L2CAP_PACKET_PRIORITIZATION					XA_ENABLED

/* 
	==================================================
						RFCOMM definitions 
    	==================================================
*/
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
#define RFCOMM_PROTOCOL         							(BTL_CONFIGI_RFCOMM)

/*-------------------------------------------------------------------------
 * NUM_RF_SERVERS constant
 *
 *     Defines the number of RFCOMM services that can be registered with
 *     RFCOMM_RegisterServerChannel.
 *
 *     This value may range from 0 to 30. The default is 4.
 *
 *	[@ToDo][UdiR] - Calculate the correct value depending on:
 *	 1. Registered servers with BTL_RFCOMM
 *	2. Internal calls to Servers
 *
 */
#define NUM_RF_SERVERS					25 

/* 
	==================================================
						SDP definitions 
    	==================================================
*/
/*-------------------------------------------------------------------------
 * SDP_SERVER_SEND_SIZE constant
 * 
 *     Defines the maximum number of bytes that the server can send in a
 *     single response.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU).
 */
#define SDP_SERVER_SEND_SIZE    						(L2CAP_MTU)


/* 
	==================================================
						HID definitions 
    	==================================================
*/

/*---------------------------------------------------------------------------
 * HID_DEVICE constant
 *     Defines the role of the device as an HID device.  HID Device code is 
 *     included if HID_DEVICE is defined as XA_ENABLED.
 */
#define HID_DEVICE                 							XA_DISABLED

/*---------------------------------------------------------------------------
 * NUM_SCO_CONNS constant
 * 
 *     Defines the number of SCO connections supported by this device.
 *     When NUM_SCO_CONNS is set to 0, SCO capabilities are disabled
 *     to save code size.
 *
 *     The default value is 0, and the maximum is 3.
 */
#define NUM_SCO_CONNS				    					(BTL_CONFIG_NUM_SCO_CONNS)

/* SBC definitions */
/*---------------------------------------------------------------------------
 * BT_PACKET_HEADER_LEN constant
 *
 *     The BT_PACKET_HEADER_LEN constant is the size of the BtPacket header.
 *     The default size is the max(L2CAP) header size (8 bytes), plus the
 *     max(RFCOMM,SDP,TCS) header size (6 bytes). If BNEP is used, you must
 *     increase the BT_PACKET_HEADER_LEN by 5 bytes: (8+6+5) in your config
 *     overide file.
 */
#define BT_PACKET_HEADER_LEN    28 /* Required for MP3 */

/* 
	==================================================
						A2DP definitions 
    	==================================================
*/
    
/*---------------------------------------------------------------------------
 * A2DP_SOURCE constant
 *
 *    This value determines whether code to support an A2DP source is included
 *    or not.  Set this value to XA_ENABLED to include the A2DP source.
 */
#define A2DP_SOURCE             							(BTL_CONFIG_A2DP)

/*---------------------------------------------------------------------------
 * A2DP_SINK constant
 *    This value determines whether code to support an A2DP sink is included
 *    or not.  Set this value to XA_ENABLED to include the A2DP sink.
 */
#define A2DP_SINK                  XA_DISABLED

/*---------------------------------------------------------------------------
 * SBC_ENCODER constant
 *     Determines whether code for encoding an SBC stream is included.
 */
#define SBC_ENCODER             							(BTL_CONFIG_A2DP_SBC_BUILT_IN_ENCODER)

/*---------------------------------------------------------------------------
 * SBC_DECODER constant
 *     Determines whether code for decoding an SBC stream is included.
*/
#define SBC_DECODER             							XA_DISABLED

/*---------------------------------------------------------------------------
 * SBC_MATH_FUNCTIONS
 *
 *     Determines the type of math operations used by the SBC Codec.  The
 *     choice of operation type should be based both on the capabilies of
 *     the target processor as well as the desired accuracy/performance of
 *     the encode/decode process.
 *
 *     If a processor has a floating-point co-processor, then the best choice
 *     may be to enable floating-point operations.
 *
 *     If there is no floating-point co-processor, then the best choice will
 *     undoubtedly be to enable fixed-point operations.  Fixed-point math for
 *     the iAnywhere Codec uses a 32-bit 17:15 format, where the high order
 *     17 bits represent the whole part of the number and the low order 15 bits
 *     represent the fractional part of the number (see Mul and Div).  In some
 *     calculations, a 2:30 format is used (see MulP).
 *
 *     If low processor usage is more important than accuracy, it is possible
 *     to use low resolution math functions.  It is important to note, however,
 *     that in order to qualify an SBC Codec it is necessary to declare the
 *     quality of the calcuations as described in section 9.4.1 of the A2DP test
 *     specification v1.0.  The quality of the low resolution functions used by
 *     the iAnywhere SBC Codec are defined as K=13 for the decoder, and K=7 for
 *     the encoder (see section 9.4.1).
 *
 *     It is also possible to use hand-coded assembly language routines for
 *     the target processor.  These routines must be provided by the developer.
 *     Sample routines are provided only for the INTEL x386 processor family.
 *     The best performance is made when routines are assembled in-line.
 *
 *     The default is to use fixed-point, high resolution math.
 */
#define SBC_MATH_FUNCTIONS 							(SBC_MATH_USE_FIXED_HI_RES)

/* 
==================================================
						OBEX definitions 
==================================================
*/

/*---------------------------------------------------------------------------
 * OBEX_TRANSPORT_FLOW_CTRL constant
 *
 *     This option enables/disables the API functions for application control
 *     over OBEX data flow. When enabled, the application can control the
 *     flow of data received by the client (during a GET) or the server
 *     (during a PUT).
 */
#define OBEX_TRANSPORT_FLOW_CTRL        XA_ENABLED

/*---------------------------------------------------------------------------
 * OBEX_AUTHENTICATION constant
 *
 *     This option enables OBEX layer support for OBEX Authentication Challenge
 *     and Response headers. OBEX Authentication support includes functions 
 *     necessary to build and parse authentication challenge and response
 *     headers. There is also a function for verifying that an authentication
 *     response used the correct password.
 *
 *	[@ToDo][UdiR] - It would be desirable to allow BTIPS users to disable
 *	OBEX authentication but BTL layer is not ready for it.
 */
#define OBEX_AUTHENTICATION             BTL_CONFIG_OBEX_AUTHENTICATION

/*---------------------------------------------------------------------------
 * OBEX_DEINIT_FUNCS constant
 *
 *     Some implementations of the OBEX API may allow applications to 
 *     dynamically acquire and release the resources of the OBEX protocol
 *     client and server parsers without shutting down the entire XTNDAccess
 *     Embedded stack. In such cases, OBEX_DEINIT_FUNCS should be enabled to
 *     provide deinitialization routines which allow the application to
 *     release the resources that it previously acquired via the OBEX init
 *     function. Otherwise, the option can be disabled to save code size.
 */
#define OBEX_DEINIT_FUNCS               XA_ENABLED

/*---------------------------------------------------------------------------
 * OBEX_PACKET_FLOW_CONTROL constant
 *
 *     This option enables application support for managing the flow of command
 *     and response packets between the client and server. When enabled, the
 *     server will generate OBSE_SEND_RESPONSE events when it is time to send a
 *     response to the client. Similarly, the client will generate OBCE_SEND_
 *     COMMAND events when it is time to send a continuing request to the
 *     server. The application must call the appropriate OBEX continue command
 *     or the exchange will pause. The call may be done in the context of the
 *     callback or it may be deferred until resource constraints or similar
 *     conditions have been relieved. 
 *
 *     Note that when this option is enabled, the protocol server changes how
 *     it processes ABORT requests. This is done to insure that the ABORT
 *     request is delivered to the server application in a timely manner. The
 *     server application must be aware that an OBSE_ABORT_START event may
 *     arrive while it is busy processing an existing request. In such a case it
 *     must stop processing the current request and respond to the remote
 *     device as indicated by OBSE_SEND_RESPONSE events. When the abort
 *     complete an OBSE_ABORTED event will be indicated to the application.
 */
#define OBEX_PACKET_FLOW_CONTROL        XA_ENABLED

/*---------------------------------------------------------------------------
 * OBEX_TLV_PARSER constant
 *
 *     The OBEX TLV Parser option is enabled to add support for parsing
 *     OBEX ByteSeq headers that contain tag-length-value encoded data. The
 *     following headers fall into this category: APP_PARAMS, AUTH_CHAL and
 *     AUTH_RESP. Enabling authentication automatically enables this option.
 *
 *     Note: In OBEX versions 3.3.6 and earlier, a similar option was known 
 *     as OBEX_BYTE_TAG_PARSER.
 *
 */
#define OBEX_TLV_PARSER 								XA_ENABLED

/*---------------------------------------------------------------------------
 * OBEX_SESSION_SUPPORT constant
 *
 *     This option indicates whether reliable OBEX Sessions will be supported
 *     in OBEX. OBEX Sessions provide a guaranteed and reliable session
 *     OBEX operations. Such a reliable session can be recovered if an
 *     unexpected transport disconnection should occur, thus allowing OBEX
 *     operations to be resumed. If the reliable session functionality is
 *     not necessary, this option can be disabled to reduce code size.
 */
#define OBEX_SESSION_SUPPORT            XA_ENABLED

/*---------------------------------------------------------------------------
 * OBEX_PROVIDE_SDP_RESULTS constant
 *
 *     This option indicates whether OBEX Clients are allowed to pass up SDP
 *     Query result information to the application layer for further SDP 
 *     parsing.  This feature is a Bluetooth specific option.  If this 
 *     functionality is not necessary, this option should be disabled to 
 *     reduce code size.
 */
#define OBEX_PROVIDE_SDP_RESULTS        XA_ENABLED

/*---------------------------------------------------------------------------
 * OBEX_DYNAMIC_OBJECT_SUPPORT constant
 *
 *     This option enables support for sending objects of unknown size.
 *     Normally, the protocol engine requires the size of the object be known
 *     when it is provided to the protocol. This requirement enables the
 *     protocol to make optimum use of the OBEX packet size and RAM resources.
 *     However, in certain cases, such as with dynamically created objects, it
 *     is difficult to know the exact size of the object in advance. Enabling
 *     this option adds a new object store read function which is permitted to
 *     read variable amounts of data. This function also signals the protocol
 *     when the entire object has been read instead of the protocol counting
 *     down the object size. Note that this option applies to object store
 *     read operations only: Client-PUT and Server-GET.
 *
 *     Refer to the documentation on OBS_ReadFlex() for more information
 *     on how to use this functionality.
 */
#define OBEX_DYNAMIC_OBJECT_SUPPORT     XA_ENABLED

/*---------------------------------------------------------------------------
 * OBEX_PERFORMANCE_MULTIPLIER constant
 * 
 *     OBEX achieves throughput gains by allowing the data source to send
 *     multiple transport PDU's before having to wait for an acknowledgement.
 *     The number of transport PDU's that can be sent is determined by the OBEX
 *     Packet size. The OBEX packet size is determined by multiplying the TPDU's
 *     size by a factor of one or more. This factor is the
 *     OBEX_PERFORMANCE_MULTIPLIER. The effectiveness of this multiplier is
 *     tied to the underlying transports data rate and technology. Both the
 *     client and server advertise their individual maximum OBEX Packet size.
 *     The values are asymmetric and are not negotiated. This value is used to
 *     calculate our receive size (aka the peer's transmit size). Our transmit
 *     size is determined by the peer device.
 * 
 *     IrDA: This value should be, at a minimum, the window size supported by
 *         the stack. Generally the OBEX packet size should grow proportionately
 *         to the IR data rate.
 *
 *     Bluetooth: This value is largely subjective, a value between 4 & 20 is
 *         recommended. This is based on a default L2CAP packet size of
 *         672 bytes.
 */
#define OBEX_PERFORMANCE_MULTIPLIER		20

/*---------------------------------------------------------------------------
 * OBEX_SERVER_CONS_SIZE constant
 *
 *     Number of Obex Server connections supported by server application.
 *     This is in addition to the default server connection. It is equal to
 *     the number of Target headers registered by the Server Application.
 *
 *     When the default value of zero is used, the OBEX Server Connection code
 *     is removed from the build.
 *
 *		Possible servers: 
 *			1 for OPP server 
 *			1 for FTP server
 *			1 for PBAP server
 *			1 for BIP responder
 *			1 for BPP sender (for object channel)
 *		Possible clients: 
 *			1 for OPP client
 *			1 for FTP client
 *			1 for BIP initiator
 *			2 for BPP sender (for job channel & status channel)
 */
#if (BTL_CONFIG_OPP == BTL_CONFIG_ENABLED)
#define OBEX_CONS_OPPC								1
#define OBEX_CONS_OPPS								1
#else
#define OBEX_CONS_OPPC								0
#define OBEX_CONS_OPPS								0
#endif

#if (BTL_CONFIG_FTPS == BTL_CONFIG_ENABLED)
#define OBEX_CONS_FTPS								1
#else
#define OBEX_CONS_FTPS								0
#endif

#if (BTL_CONFIG_FTPC == BTL_CONFIG_ENABLED)
#define OBEX_CONS_FTPC								1
#else
#define OBEX_CONS_FTPC								0
#endif

#if (BTL_CONFIG_PBAPS == BTL_CONFIG_ENABLED)
#define OBEX_CONS_PBAPS								1
#else
#define OBEX_CONS_PBAPS								0
#endif

#if (BTL_CONFIG_BIP == BTL_CONFIG_ENABLED)
#define OBEX_CONS_BIPINT							1
#define OBEX_CONS_BIPRSP							1
#else
#define OBEX_CONS_BIPINT							0
#define OBEX_CONS_BIPRSP							0
#endif

#if (BTL_CONFIG_BPPSND == BTL_CONFIG_ENABLED)
#define OBEX_CONS_BPPSND_CLIENT						2
#define OBEX_CONS_BPPSND_SERVER						1
#else
#define OBEX_CONS_BPPSND_CLIENT						0
#define OBEX_CONS_BPPSND_SERVER						0
#endif

#define OBEX_CONS_CLIENT							(OBEX_CONS_OPPC + OBEX_CONS_FTPC + OBEX_CONS_BIPINT + OBEX_CONS_BPPSND_CLIENT)
#define OBEX_CONS_SERVER							(OBEX_CONS_OPPS + OBEX_CONS_FTPS + OBEX_CONS_PBAPS + OBEX_CONS_BIPRSP + OBEX_CONS_BPPSND_SERVER)

/* ESI request Minimum OBEX_SERVER_CONS_SIZE to be equal to 2 */
#define OBEX_SERVER_CONS_SIZE               		max(OBEX_CONS_SERVER,2)


/*---------------------------------------------------------------------------
 * OBEX_MAX_REALM_LEN constant
 * 
 *     This constant limits the size of the authentication realm that can
 *     be received in an Authentication indication. The realm is sent by
 *     the challenger to convey which user id or password the claimant is
 *     to use. If OBEX_AUTHENTICATION is XA_DISABLED this option is ignored.
 *
 *     Valid range: 0 - 255.
 */
#define OBEX_MAX_REALM_LEN 				BTL_CONFIG_OBEX_MAX_REALM_LEN

/*---------------------------------------------------------------------------
 * OBEX_MAX_USERID_LEN constant
 * 
 *     This constant specifies the maximum size User Id that can be received
 *     in an Authentication confirmation. The user id field conveys the user
 *     id for the password which was used to create the request-digest. If
 *     OBEX_AUTHENTICATION is XA_DISABLED this option is ignored.
 */
#define OBEX_MAX_USERID_LEN     BTL_CONFIG_OBEX_MAX_USER_ID_LEN

/*---------------------------------------------------------------------------
 * OBEX_PUT_DELETE constant
 *
 *     Some applications of OBEX use the PUT-DELETE behavior which allows
 *     a client to delete an object on the server by sending a PUT operation
 *     without a BODY or END-OF-BODY header. Enabling this define compiles in 
 *     client API support for this operation. If this option is disabled the 
 *     client will not be able to send PUT-DELETEs. The server always handles
 *     the PUT-DELETE case.
 */
#define OBEX_PUT_DELETE					        XA_ENABLED

/*----------------------------------------------------------------------------
 * OBEX_ROLE_SERVER constant
 *
 *     When enabled this configuration option enables support for the OBEX
 *     Server functionality in the OBEX module.
 */
#define OBEX_ROLE_SERVER						XA_ENABLED 

/*----------------------------------------------------------------------------
 * OBEX_ROLE_CLIENT constant
 *
 *     When enabled this configuration option enables support for the OBEX
 *     Client functionality in the OBEX module.
 */
#define OBEX_ROLE_CLIENT						XA_ENABLED 

/*---------------------------------------------------------------------------
 * OBEX_BODYLESS_GET constant
 *
 *     Indicates whether OBEX Servers may respond to a GET request with
 *     no object using OBEX_ServerAcceptNoObject(). Normally a GET command
 *     results in the transfer of an object via Body/End-of-Body headers.
 *     If this option is enabled and OBEX_ServerAcceptNoObject() is used,
 *     the server will respond only with those headers provided using OBEXH_
 *     functions. 
 */
#define OBEX_BODYLESS_GET                       XA_ENABLED

/*---------------------------------------------------------------------------
 * OBEX_ALLOW_SERVER_TP_DISCONNECT constant
 *
 *     If a Server is allowed to create a transport connection, it will be
 *     automatically allowed to disconnect transport connections.
 */
#define OBEX_ALLOW_SERVER_TP_DISCONNECT         XA_ENABLED

/*---------------------------------------------------------------------------
 * OBEX_GET_USE_END_OF_BODY constant
 *
 *     If a Server can fit the rest of the object data into the last packet, it 
 *     will send OBRC_SUCCESS in the response code. This will prevent sending
 *     and empty END_OF_BODY in the last packet (which causes some problems 
 *     to some stacks).
 */ 
#define OBEX_GET_USE_END_OF_BODY                XA_ENABLED 
 
 
/* 
==================================================
						GOEP definitions 
==================================================
*/

/*---------------------------------------------------------------------------
 * GOEP_MAX_UNICODE_LEN constant
 *
 *     Defines the maximum number of characters possible in an object, 
 *     folder name, or queued unicode header (including the null-terminator). 
 *     The maximum value is 32,767 (or 0x7FFF). This value must be greater 
 *     than zero, however, it will in all likelihood be larger than one, 
 *     since most filenames exceed one byte in length.
 */
#define GOEP_MAX_UNICODE_LEN 							2*BTHAL_FS_MAX_FILE_NAME_LENGTH

/*---------------------------------------------------------------------------
 * GOEP_DOES_UNICODE_CONVERSIONS constant
 *
 *     Defines the format of UNICODE data passed between the GOEP API and the
 *     layered application or profile. If enabled, the GOEP layer expects
 *     all Name and Description data pointers (such as in GoepPushReq) as 
 *     well as all queued unicode header data pointers (such as a unicode 
 *     header queued via GOEP_ClientQueueHeader or GOEP_ServerQueueHeader) to 
 *     point to an ASCII string. In this case the GOEP layer will convert the 
 *     string to UNICODE before transmission. Similarly all received UNICODE 
 *     strings are converted to ASCII. If disabled, all UNICODE headers are 
 *     expected to be provided in UNICODE format.
 */
#define GOEP_DOES_UNICODE_CONVERSIONS   				XA_ENABLED

/*---------------------------------------------------------------------------
 * GOEP_NUM_OBEX_CONS constant
 *
 *     Defines the number of OBEX connections used in GOEP.  One GOEP 
 *     multiplexer is allocated for each OBEX connection.
 *
 */
#define GOEP_NUM_OBEX_CONS_TEMP              				max(OBEX_CONS_CLIENT, OBEX_CONS_SERVER)

/* ESI request Minimum GOEP_NUM_OBEX_CONS to be equal to 1 */
#define GOEP_NUM_OBEX_CONS              					max(1, GOEP_NUM_OBEX_CONS_TEMP)




/*---------------------------------------------------------------------------
 * GOEP_MAX_TYPE_LEN constant
 *
 *     Defines the maximum size, in characters of an object type (including
 *     the null-terminator). Default max type is 'text/x-vcard'. If set to
 *     zero, TYPE headers are not stored. The maximum value is 255.
 */
#define GOEP_MAX_TYPE_LEN               					128

/*---------------------------------------------------------------------------
 * GOEP_CLIENT_HB_SIZE constant
 * 
 *     Transmit Header block construction buffer size. The upper limit on the
 *     header block size is the OBEX packet size. For RAM efficiency this
 *     value should be calculated as follows. The most header data the GOEP
 *     will ever send is in a client PUT which sends a Name header and a
 *     Length header. The max name length GOEP_MAXIMUM_NAME_LEN times 2 for
 *     UNICODE + 3 bytes for the OBEX Name header. Add in a length header
 *     (5 bytes) and you get the total. 
 *     Now when authentication is enabled there is the issue of space for
 *     the Authentication Response and/or Challenge header(s). There is
 *     likely enough space already allocated by for the NAME & TYPE headers
 *     that so long as they aren't all in the same packet, the is no need
 *     to allocate additional space.
 */
#define GOEP_CLIENT_HB_SIZE                  					1000

/*---------------------------------------------------------------------------
 * GOEP_SERVER_HB_SIZE constant
 * 
 *     The server transmit header block must be large enough to send an
 *     OBEX length header (5 bytes). If Authentication is enabled, it must
 *     be large enough to hold an AuthResponse (18+18+22) or an AuthChallenge
 *     (18+3+MaxRealmLength). If we make the Header block the size of the
 *     AuthResponse then that leaves 35 bytes for MaxRealmLength, which seems
 *     like a comfortable value. So that's what is done here. (The server
 *     never sends both AuthResponse & AuthChallenge in one packet).
 */
#define GOEP_SERVER_HB_SIZE                  				1000

/* 
==================================================
						OPP definitions 
==================================================
*/

/*---------------------------------------------------------------------------
 * OPUSH_EXPANDED_API configuration option
 *
 *     This option enables support for the full OPush API.
 */
#define OPUSH_EXPANDED_API              					XA_ENABLED

/* 
	==================================================
						FTP definitions 
	==================================================
*/

/*---------------------------------------------------------------------------
 * FTP_EXPANDED_API configuration option
 *
 *     This option toggles support for the full File Transfer API.
 */
#define FTP_EXPANDED_API								XA_ENABLED

/* ==================================================
						AT Engine definitions 
    ==================================================*/

/*---------------------------------------------------------------------------
 * AT_PHONEBOOK constant
 *
 *	Configures whether we support parsing of phonebook related AT commands
 */

#if BTL_CONFIG_VG == BTL_CONFIG_ENABLED

#define AT_PHONEBOOK								XA_ENABLED

#else

#define AT_PHONEBOOK								XA_DISABLED

#endif

/*---------------------------------------------------------------------------
 * AT_ROLE_MOBILE constant
 *
 *	Configures whether we support parsing of Audio Gateway AT commands
 */

#if ((BTL_CONFIG_AG == BTL_CONFIG_ENABLED) || (BTL_CONFIG_VG == BTL_CONFIG_ENABLED))

#define AT_ROLE_MOBILE                             				XA_ENABLED

#else

#define AT_ROLE_MOBILE                             				XA_DISABLED

#endif

/*---------------------------------------------------------------------------
 * AT_HEADSET constant
 *
 */
#if ((BTL_CONFIG_AG == BTL_CONFIG_ENABLED) || (BTL_CONFIG_VG == BTL_CONFIG_ENABLED))

#define AT_HEADSET                                     				XA_ENABLED

#else

#define AT_HEADSET                             					XA_DISABLED

#endif

/*---------------------------------------------------------------------------
 * AT_HANDSFREE constant
 */
#if ((BTL_CONFIG_AG == BTL_CONFIG_ENABLED) || (BTL_CONFIG_VG == BTL_CONFIG_ENABLED))

#define AT_HANDSFREE                                 				XA_ENABLED
#else

#define AT_HANDSFREE                             					XA_DISABLED

#endif

/*---------------------------------------------------------------------------
 * AT_MAX_INDICATORS constant
 */
#define AT_MAX_INDICATORS                       				(BTL_CONFIG_HFAG_AT_MAX_INDICATORS)

/*---------------------------------------------------------------------------
 * AT_ROLE_TERMINAL constant
 *
 * Configures whether we support parsing of Terminal AT commands
 *
 *	Disabled since a cellular phone doesn't implement the unit role
 */
#define AT_ROLE_TERMINAL								XA_DISABLED

/* 
	==================================================
						HF / HS AG definitions 
	==================================================
*/

/*-------------------------------------------------------------------------
 * AG_SECURITY constant
 *
 *     Determines whether security is enabled for headset or not.  If 
 *     AG_SECURITY is defined, BT_SECURITY must be defined as well.
 *
 */
#define AG_SECURITY             								XA_DISABLED

/*-------------------------------------------------------------------------
 * AG_SCO_SETTINGS constant
 *
 *     Determines whether SCO_SetSettings() will be called before an SCO
 *     connection is established.
 */
#define AG_SCO_SETTINGS         							XA_DISABLED	

/*---------------------------------------------------------------------------
 * HFG_SDK_FEATURES constant
 *
 *     Defines the supported features of the hands-free application.  Features
 *     that will be supported by the application must be advertised to the
 *     hands-free device, so that it knows the capabilities of the audio gateway.
 *     If this value needs to be changed, it can be placed in overide.h with
 *     the features supported by the audio gateway application.
 */
#define HFG_SDK_FEATURES								(HFG_FEATURE_EXTENDED_NONE)


/*---------------------------------------------------------------------------
 * HFG_USE_RESP_HOLD constant
 *
 *	Enable this flag to support the Response and Hold HFP feature.
 *	Otherwise the AT command AT+BTRH shall not be recognized.
 *	and return an error indication to the HF Unit.
 */
 #if BTL_CONFIG_VG == BTL_CONFIG_ENABLED
 
#define HFG_USE_RESP_HOLD								XA_ENABLED

#else

#define HFG_USE_RESP_HOLD								XA_DISABLED

#endif
/* 
==================================================
						AVRCP definitions 
==================================================
*/

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
#define AVRCP_METADATA_TARGET		XA_DISABLED 

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
#define AVRCP_METADATA_CONTROLLER XA_DISABLED

/* 
==================================================
						BPP definitions 
==================================================
*/

/*---------------------------------------------------------------------------
 * BPP_NUM_SENDERS Constant
 *
 *  Defines the number of concurrent Senders supported.
 *
 *  The default value is 1. Set to 0 to disable Sender support.
 */
#if (BTL_CONFIG_BPPSND == BTL_CONFIG_ENABLED)
#define BPP_NUM_SENDERS             						1
#else
#define BPP_NUM_SENDERS             						0
#endif

/*---------------------------------------------------------------------------
 * BPP_NUM_PRINTERS Constant
 *
 *  Defines the number of concurrent Printers supported.
 *
 *  The default value is 1. Set to 0 to disable Printer support.
 */
#define BPP_NUM_PRINTERS            						0

/*---------------------------------------------------------------------------
 * BPP_MAX_PASSWORD_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication password.
 *
 */

#define BPP_MAX_PASSWORD_LEN     			BTL_CONFIG_OBEX_MAX_PASS_LEN


/*---------------------------------------------------------------------------
 * BPP_MAX_USERID_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication username.
 *
 */

#define BPP_MAX_USERID_LEN       				BTL_CONFIG_OBEX_MAX_USER_ID_LEN


/*---------------------------------------------------------------------------
 * BPP_MAX_REALM_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication realm.
 *
 */

#define BPP_MAX_REALM_LEN        				BTL_CONFIG_OBEX_MAX_REALM_LEN

/* 
==================================================
						SAPS definitions 
==================================================
*/

/*---------------------------------------------------------------------------
 * SIM_SERVER constant
 *     Defines the role of the device as a server.  Server code is included
 *     if SIM_SERVER is defined as XA_ENABLED.  A device can have both the
 *     server and client roles, if necessary.
 */
#define SIM_SERVER          								(BTL_CONFIG_SAPS)

/*---------------------------------------------------------------------------
 * SIM_CLIENT constant
 *     Defines the role of the device as a client.  Client code is included
 *     if SIM_CLIENT is defined as XA_ENABLED.  A device can have both the
 *     server and client roles, if necessary.
 */
#define SIM_CLIENT XA_DISABLED

/* 
==================================================
						BIP definitions 
==================================================
*/

/*---------------------------------------------------------------------------
 * BIP_NUM_INITIATORS Constant
 *
 *  Defines the number of concurrent Initiators supported.
 *
 *  The default value is 1. Set to 0 to disable Initiator support.
 */
#if (BTL_CONFIG_BIP == BTL_CONFIG_ENABLED)
#define BIP_NUM_INITIATORS          1
#else
#define BIP_NUM_INITIATORS          						0
#endif

/*---------------------------------------------------------------------------
 * BIP_NUM_RESPONDERS Constant
 *
 *  Defines the number of concurrent Responders supported.
 *
 *  The default value is 1. Set to 0 to disable Responder support.
 */
#if (BTL_CONFIG_BIP == BTL_CONFIG_ENABLED)
#define BIP_NUM_RESPONDERS          1
#else
#define BIP_NUM_RESPONDERS          						0
#endif

/*---------------------------------------------------------------------------
 * BIP_MAX_PASSWORD_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication password.
 *
 */

#define BIP_MAX_PASSWORD_LEN              BTL_CONFIG_OBEX_MAX_PASS_LEN


/*---------------------------------------------------------------------------
 * BIP_MAX_USERID_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication username.
 *
 */

#define BIP_MAX_USERID_LEN                    BTL_CONFIG_OBEX_MAX_USER_ID_LEN


/*---------------------------------------------------------------------------
 * BIP_MAX_REALM_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication realm.
 *
 */

#define BIP_MAX_REALM_LEN                     BTL_CONFIG_OBEX_MAX_REALM_LEN

#if BTL_CONFIG_SPP == BTL_CONFIG_ENABLED

/* 
==================================================
						SPP definitions 
==================================================
*/

/*---------------------------------------------------------------------------
 * SPP_SECURITY_CLIENT Constant
 *
 * Defines whether security will be applied to SPP client ports
 * Currently, it is disabled because of a problem with D-Link USB dongle with
 * Broadcom's protocol stack
 */
#define SPP_SECURITY_CLIENT		            (BTL_CONFIG_SPP_SECURITY_CLIENT)

/*---------------------------------------------------------------------------
 * SPP_USE_INTERIM_TX_BUFFER Constant
 *
 * Configuration constant to enable allocation interim buffer for Tx Sync data
 * path
 */
#define SPP_USE_INTERIM_TX_BUFFER           (BTL_CONFIG_SPP_USE_INTERIM_TX_BUFFER)

/*---------------------------------------------------------------------------
 * SPP_USE_INTERIM_RX_BUFFER Constant
 *
 * Configuration constant to enable allocation interim buffer for Rx Async data
 * path
 */
#define SPP_USE_INTERIM_RX_BUFFER           (BTL_CONFIG_SPP_USE_INTERIM_RX_BUFFER)

/*---------------------------------------------------------------------------
 * SPP_TX_MAX_NUM_OF_RFCOMM_PACKETS Constant
 *
 * Size of a pool with BtPacket type structures used for sending Tx data to
 * RFCOMM
 */
#define SPP_TX_MAX_NUM_OF_RFCOMM_PACKETS    (BTL_CONFIG_SPP_TX_MAX_NUM_OF_RFCOMM_PACKETS)

#endif /* BTL_CONFIG_SPP == BTL_CONFIG_ENABLED */


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
#define HCI_HOST_FLOW_CONTROL 				BTL_HCI_HOST_FLOW_CONTROL

#endif /* __OVERIDE_H */

