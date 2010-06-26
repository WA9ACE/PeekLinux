/*******************************************************************************
##                                                                           	*
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
##																				*
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
##																				*
*******************************************************************************/
/*******************************************************************************
*
*   FILE NAME:      btl_config.h
*
*   BRIEF:          BTIPS configuration parameters
*
*   DESCRIPTION:
*
*     The constants in this file configure the layers of the BTIPS
*      package. To change a constant, simply change its value in this file
*      and recompile the entire BTIPS package.
*
*     Some constants are numeric, and others indicate whether a feature
*     is enabled (defined as BTL_CONFIG_ENABLED) or disabled (defined as
*     BTL_CONFIG_DISABLED).
*
*     Configuration constants are used to make the package more appropriate for a
*     particular environment. Constants can be modified to allow tradeoffs
*     between code size, RAM usage, functionality, and throughput. 
*
*   AUTHOR:   Udi Ron
*
*****************************************************************************/
#ifndef __BTL_CONFIG_H
#define __BTL_CONFIG_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include "bthal_types.h"


/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * Common
 *
 *     Represents common configuration parameters.
 */

/* The following defenition does as followed:
	1.Disable entering and exiting function traces.
	2.Disable DEBUG level traces */
#define  EBTIPS_RELEASE


/*
*	Enabled profiles configuration
*/


#define BTL_CONFIG_SPP 					BTL_CONFIG_ENABLED
#define BTL_CONFIG_MDG 					BTL_CONFIG_ENABLED

/* Merge OPPC  and OPPS  to one #define BTL_CONFIG_OPP*/
#define BTL_CONFIG_OPP                  BTL_CONFIG_ENABLED
#define BTL_CONFIG_BPPSND 				BTL_CONFIG_ENABLED
#define BTL_CONFIG_PBAPS 				BTL_CONFIG_ENABLED
#define BTL_CONFIG_AVRCPTG				BTL_CONFIG_ENABLED
#define BTL_CONFIG_FTPS 				BTL_CONFIG_ENABLED
#define BTL_CONFIG_FTPC 				BTL_CONFIG_ENABLED
#define BTL_CONFIG_BTL_RFCOMM 			BTL_CONFIG_DISABLED
#define BTL_CONFIG_BTL_L2CAP	 		BTL_CONFIG_ENABLED
#define BTL_CONFIG_A2DP 				BTL_CONFIG_ENABLED
#define BTL_CONFIG_SAPS 				BTL_CONFIG_DISABLED
#define BTL_CONFIG_HIDH 				BTL_CONFIG_ENABLED
#define BTL_CONFIG_AG 					BTL_CONFIG_DISABLED
#define BTL_CONFIG_VG 					BTL_CONFIG_ENABLED

/* Merge BIP_RSP and BIP_INT to one #define BTL_CONFIG_BIP*/
#define BTL_CONFIG_BIP                  BTL_CONFIG_ENABLED
#define BTL_CONFIG_FM_STACK				BTL_CONFIG_DISABLED

/*
*	Indicates that a feature is enabled
*/
#define BTL_CONFIG_ENABLED											(1)

/*
*	Indicates that a feature is disabled
*/
#define BTL_CONFIG_DISABLED											(0)

/*
*
*	The maximum number of registered BTIPS applications
*	New applications may be registered by calling BTL_RegisterApp.
*/
#define BTL_CONFIG_MAX_NUM_OF_APPS									(2)

/*
*
*	The maximum number of contexts of any profile that a single
*	application may create.
*/
#define BTL_CONFIG_MAX_NUM_OF_CONTEXTS_PER_APP						(50)

/*
*
*	The maximum length of an application name (used when creating
*	a new application)
*/
#define BTL_CONFIG_MAX_APP_NAME										(20)

/*
 *
 *     The number of devices with which we can connect. This value
 *     represents the maximum size of the piconet in which this device is
 *     the master, plus any master devices for which we are concurrently a
 *     slave. This value includes devices which may be parked, holding,
 *     or sniffing as well any active devices. The value excludes the local device.
 */
 #define BTL_CONFIG_MAX_NUM_OF_CONNECTED_DEVICES					(3)

/*
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
#define BTL_CONFIG_SECURITY_TIMEOUT									(30)

/*
 *
 *     Controls whether the protocol sniffer hooks in various protocol layers
 *     are compiled into the system. The sniffer uses an endpoint registration
 *     function to manage a connection table which is used as a guide when 
 *     decoding data packets.
 */
#define BTL_CONFIG_SNIFFER												(BTL_CONFIG_DISABLED)

/*
 *
 *     Controls whether the INET Transport is enabled or disabled 
 */
#define BTL_CONFIG_TRANSPORT_INET										(BTL_CONFIG_DISABLED)

/*
 *     Defines the number of SCO connections supported by this device.
 *     When BTL_CONFIG_NUM_SCO_CONNS is set to 0, SCO capabilities are disabled
 *     to save code size.
 *
 *     The value must be between 0 and 3.
 */
#define BTL_CONFIG_NUM_SCO_CONNS				    						(1)

/*
 * BMG
 *
 *     Represents configuration parameters for BMG module.
 */

/*
*	The maximum number of BMG contexts to be shared among all of the 
*	applications (rather than per application)
*/
#define BTL_CONFIG_BMG_MAX_NUM_OF_BMG_CONTEXTS					(10)

/*
 *	The configured value limits the number of concurrent calls to BTL_BMG_SendHciCommand
 */
#define BTL_CONFIG_BMG_MAX_NUM_OF_CONCURRENT_HCI_COMMANDS		(3)

/*
 * 	The configured value limits the number of filtered responses that may be specified in 
 * 	the call to BTL_BMG_SearchByCod
 */
#define BTL_CONFIG_BMG_MAX_NUM_OF_RESPONSES_FOR_SEARCH_BY_COD	(20)

/*
 *	The configured value limits the number of devices that may be specified for filtering in
 *	the call to BTL_BMG_SearchByDevices
 */
#define BTL_CONFIG_BMG_MAX_NUM_OF_DEVICES_FOR_SEARCH_BY_DEVICES 					(20)

/*
 * 	The size (in bytes) of the user data section in a device record.
 *	This is the size of the userData field in the BtlBmgDeviceRecord structure.
 */
#define BTL_BMG_DDB_CONFIG_DEVICE_REC_USER_DATA_SIZE									(10)

/*
 * 	The maximum number of entries in the device DB
 */
#define BTL_BMG_DDB_CONFIG_MAX_ENTRIES  													(10)

/*
 *	The default security mode of BTIPS
 *	This value may be changed after BTL BMG initialization by calling BTL_BMG_SetSecurityMode
 */
#define BTL_CONFIG_BMG_DEFAULT_SECURITY_MODE											(BSM_SEC_LEVEL_2)

/*
 * 	The configured value limits the number of concurrent calls to BTL_BMG_Bond
 */
#define BTL_CONFIG_BMG_MAX_NUM_OF_CONCURRENT_BOND_REQUESTS							(3)

/*
 * 	The maximum number of attributes in a single record in the Service Discovery Database.
 */
#define BTL_CONFIG_BMG_MAX_ATTRIBUTES_PER_SERVICE_RECORD								(40)

/* 	
*	The maximum size (in bytes) of a single attribute in a Service Discovery Database record size. 
*	The value includes the value field size.
*/
#define BTL_CONFIG_BMG_MAX_ATTRIBUTES_VALUE_SIZE										(100)

/* 
	The maximum number of attributes in all of the records in the Service Discovery Database
*/
#define BTL_CONFIG_BMG_MAX_NUM_OF_ATTRIBUTES 											(99)

/* 
	The maximum number of records in the Service Discovery Database. 
	This number excludes records that are added internally by the BTIPS package.
*/
#define BTL_CONFIG_BMG_MAX_NUM_OF_USER_SERVICE_RECORDS								(10)

/*
	The maximum number of attributes that a single service record result to an SDP query may contain.
	This number limits the value of attributeIdListLen in the calls to BTL_BMG_ServiceSearchAttributeRequest
	and BTL_BMG_ServiceAttributeRequest.
*/
#define BTL_CONFIG_BMG_MAX_NUM_OF_ATTRIBUTES_TO_SEARCH								(20)	

/*
	The default local device name
*/
#define BTL_CONFIG_BMG_DFLT_DEVICE_NAME													((U8*)"Locosto Phone")


/*
	The default Accessibility Mode when the local device is CONNECTED to a peer device
*/
#define BTL_CONFIG_BMG_DFLT_ACCESSIBILITY_MODE_C										(BAM_GENERAL_ACCESSIBLE)

/*
	The default Accessibility Mode when the local device is NOT CONNECTED  to a peer device
*/
#define BTL_CONFIG_BMG_DFLT_ACCESSIBILITY_MODE_NC										(BAM_GENERAL_ACCESSIBLE)

/*
	The default Inquiry Scan Interval value (N) when the local device is CONNECTED to a peer device
	The actual value in milliseconds is: N * 0.625 milliseconds
*/
#define BTL_CONFIG_BMG_DFLT_INQ_SCAN_INTERVAL_C											(0x1000)

/*
	The default Inquiry Scan Window value (N) when the local device is CONNECTED to a peer device
	The actual value in milliseconds is: N * 0.625 milliseconds
*/
#define BTL_CONFIG_BMG_DFLT_INQ_SCAN_WINDOW_C											(0x12)

/*
	The default Page Scan Interval value (N) when the local device is CONNECTED to a peer device
	The actual value in milliseconds is: N * 0.625 milliseconds
*/
#define BTL_CONFIG_BMG_DFLT_PAGE_SCAN_INTERVAL_C										(0x800)

/*
	The default Page Scan Window value (N) when the local device is CONNECTED to a peer device
	The actual value in milliseconds is: N * 0.625 milliseconds
*/
#define BTL_CONFIG_BMG_DFLT_PAGE_SCAN_WINDOW_C										(0x12)

/*
	The default Inquiry Scan Interval value (N) when the local device is NOT CONNECTED to a peer device
	The actual value in milliseconds is: N * 0.625 milliseconds
*/
#define BTL_CONFIG_BMG_DFLT_INQ_SCAN_INTERVAL_NC										(0x1000)

/*
	The default Inquiry Scan Window value (N) when the local device is NOT CONNECTED to a peer device
	The actual value in milliseconds is: N * 0.625 milliseconds
*/
#define BTL_CONFIG_BMG_DFLT_INQ_SCAN_WINDOW_NC										(0x12)

/*
	The default Page Scan Interval value (N) when the local device is NOT CONNECTED to a peer device
	The actual value in milliseconds is: N * 0.625 milliseconds
*/
#define BTL_CONFIG_BMG_DFLT_PAGE_SCAN_INTERVAL_NC										(0x800)

/*
	The default Page Scan Window value (N) when the local device is NOT CONNECTED to a peer device
	The actual value in milliseconds is: N * 0.625 milliseconds
*/
#define BTL_CONFIG_BMG_DFLT_PAGE_SCAN_WINDOW_NC										(0x12)


/*
	The default Major Service Class of the local device.
	Additional Major Service Class bits will be set when services are enabled in the local device
*/
#define BTL_CONFIG_BMG_DEFAULT_MAJOR_SERVICE_CLASS										(COD_TELEPHONY)

/*
	The default Major Device Class of the local device
*/
#define BTL_CONFIG_BMG_DEFAULT_MAJOR_DEVICE_CLASS										(COD_MAJOR_PHONE)

/*
	The default Minor Device Class of the local device
*/
#define BTL_CONFIG_BMG_DEFAULT_MINOR_DEVICE_CLASS										(COD_MINOR_PHONE_CELLULAR)


/*
	Defines the maximum number of devices that the ME Device Selection
	manager can track. If this value is zero, the MEDEV component is
	disabled, resulting in a code size savings.
*/
#define BTL_CONFIG_MAX_NUM_KNOWN_DEVICES												(20)


/*-------------------------------------------------------------------------------
 * AVRCPTG
 *
 *     Represents configuration parameters for AVRCPTG module.
 */

/*
*	The maximum number of AVRCPTG contexts to be shared among all of the 
*	applications (rather than per application)
*/
#define BTL_CONFIG_AVRCPTG_MAX_NUM_OF_CONTEXTS										(1)

/*
	The maximum number of AVRCPTG channels per context
*/
#define BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT								(2)

/*-------------------------------------------------------------------------------
 * HID
 *
 *     Represents configuration parameters for HID module.
 */

/*
*	The maximum number of HID contexts to be shared among all of the 
*	applications (rather than per application)
*/
#define BTL_CONFIG_HID_MAX_NUM_OF_CONTEXTS												(1)

/*
	The maximum number of HID channels per context
*/
#define BTL_CONFIG_HID_MAX_NUM_CHANNELS_PER_CONTEXT									(3)

/*
	The maximum number of interrupts that the HID Host may send the 
	peer HID device without receiving an acknowledgement.
*/	
#define BTL_CONFIG_HID_MAX_NUM_OF_TX_INTERRUPTS										(5)

/*
	The maximum number of transactions that the HID Host may send the 
	peer HID device without receiving an acknowledgement.
*/
#define BTL_CONFIG_HID_MAX_NUM_OF_TX_TRANSACTIONS             							  	(5)

/*
	The maximum number of TX Reports that the HID Host may send the 
	peer HID device without receiving an acknowledgement.
*/
#define BTL_CONFIG_HID_MAX_NUM_OF_TX_REPORTS                      							   	(3)

/*
	The maximum number of TX Report Requests that the HID Host may send the 
	peer HID device without receiving an acknowledgement.
*/
#define BTL_CONFIG_HID_MAX_NUM_OF_TX_REPORTREQ                    							(3)

/*
	The maximum number of reconnection attempts that the HID Host will make when the connection
	to the HID Device was disconnected.
*/
#define BTL_CONFIG_HID_MAX_NUMBER_OF_RECONNECTION										(3)

/*
	The time period in milliseconds between consecutive reconnection attempts
*/
#define BTL_CONFIG_HID_MAX_INTERVAL_RECONNECTION_IN_MS									(5000)

/*
	The total time period in milliseconds that the HID host will allow for reconnection attempts.
*/
#define BTL_CONFIG_HID_MAX_TIMEOUT_RECONNECTION_IN_MS									(30000)

/*
	The latency of the Quality of service
*/
#define BTL_CONFIG_HID_MAX_LATENCY_IN_MS												(25)

/*
	Injdicates whether Outgoing Quality of Service is Enabled (1) or Disabled (0)
*/							
#define BTL_CONFIG_HID_QOS_OUTGOING														(1)

/*
	Injdicates whether Incoming Quality of Service is Enabled (1) or Disabled (0)
*/
#define BTL_CONFIG_HID_QOS_INCOMING														(0)


/*-------------------------------------------------------------------------------
 * FTPS
 *
 *     Represents configuration parameters for FTPS module.
 */

/*
*	The maximum number of FTP Server contexts to be shared among all of the 
*	applications (rather than per application)
*/
#define BTL_CONFIG_FTPS_MAX_NUM_OF_CONTEXTS											(1)

/*
	The maximum number of objects (files or folders) that the local FTP Server may hide 
	from peer FTP clients
*/
#define BTL_CONFIG_FTPS_MAX_HIDE_OBJECT													(10)

/*-------------------------------------------------------------------------------
 * FTPC
 *
 *     Represents configuration parameters for FTPC module.
 */

/*
*	The maximum number of FTP Client contexts to be shared among all of the 
*	applications (rather than per application)
*/
#define BTL_CONFIG_FTPC_MAX_NUM_OF_CONTEXTS											(1)

/*
	The maximum length of a single line in the XML folder listing object that a peer FTP server returns
*/
#define BTL_CONFIG_FTPC_XML_LINE_SIZE														(256)

/*-------------------------------------------------------------------------------
 * MDG
 *
 *     Represents configuration parameters for MDG module.
 */

/*
*	The maximum number of MDG contexts to be shared among all of the 
*	applications (rather than per application)
*/
#define BTL_CONFIG_MDG_MAX_NUM_OF_CONTEXTS											(1)


/* 
 * 	Size of buffer for downloaded data - for the best performance should be not
 * 	less than SPPOS_TX_BUF_SIZE which is used for writing data to BT SPP port.
 */
/* Should it be here? Ask Vladi */
#if (BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM == BTL_CONFIG_DISABLED)

#define BTL_CONFIG_MDG_DOWNLOAD_BUF_SIZE											(SPPBUF_TX_BUF_SIZE)

#endif /* BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM == BTL_CONFIG_DISABLED */
 

/*-------------------------------------------------------------------------------
 * OPPS
 *
 *     Represents configuration parameters for OPPS module.
 */

/*
*	The maximum number of OPP Server contexts to be shared among all of the 
*	applications (rather than per application)
*/
#define BTL_CONFIG_OPPS_MAX_NUM_OF_CONTEXTS											(1)

/*-------------------------------------------------------------------------------
 * OPPC
 *
 *     Represents configuration parameters for OPPC module.
 */

/*
*	The maximum number of OPP Client contexts to be shared among all of the 
*	applications (rather than per application)
*/
#define BTL_CONFIG_OPPC_MAX_NUM_OF_CONTEXTS											(1)

 
/*
 * 	Defines how much the OPP client should wait for obex Pull or Push operations (in seconds)
 *
 *	When 0, oppc obex push or pull timer is disabled
*/
#define	BTL_CONFIG_OPPC_PUSH_PULL_TIMER                  										(30)	


/*-------------------------------------------------------------------------------
 * SPP
 *
 *     Represents configuration parameters for SPP module.
 */

/* 
 * 	The maximum number of SPP ports
 */
#define BTL_CONFIG_SPP_MAX_NUM_OF_PORTS_CONTEXTS									(9)

/* Define whether security will be applied to SPP client ports
 * Currently, it may be disabled because of a problem with D-Link USB dongle with
 * Broadcom's protocol stack */
#define BTL_CONFIG_SPP_SECURITY_CLIENT                                              (BTL_CONFIG_ENABLED)

/* Configuration constant to enable allocation interim buffer for Tx Sync data
 * path. */
#define BTL_CONFIG_SPP_USE_INTERIM_TX_BUFFER                                        (BTL_CONFIG_ENABLED)

 /* Configuration constant to enable allocation interim buffer for Rx Async data
 * path. */
#define BTL_CONFIG_SPP_USE_INTERIM_RX_BUFFER                                        (BTL_CONFIG_ENABLED)

#if BTL_CONFIG_SPP_USE_INTERIM_TX_BUFFER == BTL_CONFIG_ENABLED

/* Size of interim Tx buffer measured in RFCOMM packets of maximal size */
#define BTL_CONFIG_SPP_SIZE_OF_INTERIM_TX_BUFFER	                                (2)

#endif /* BTL_CONFIG_SPP_USE_INTERIM_TX_BUFFER == BTL_CONFIG_ENABLED */

#if BTL_CONFIG_SPP_USE_INTERIM_RX_BUFFER == BTL_CONFIG_ENABLED

/* Size of interim Rx buffer measured in RFCOMM packets of maximal size */
#define BTL_CONFIG_SPP_SIZE_OF_INTERIM_RX_BUFFER	                                (2)

#endif /* BTL_CONFIG_SPP_USE_INTERIM_RX_BUFFER == BTL_CONFIG_ENABLED */

#if (BTL_CONFIG_SPP_USE_INTERIM_TX_BUFFER == BTL_CONFIG_ENABLED)||(BTL_CONFIG_SPP_USE_INTERIM_RX_BUFFER == BTL_CONFIG_ENABLED)
/* The maximum number of SPPBUF. The SPPBUF struct holds 2 buffers which 
 *  will be used for TX_SYNC and RX_ASYNC port configuration, therefor if a 
 *  port opened with TX_ASYNC and RX_SYNC this buffer will not be used. 
 *  One can Open BTL_CONFIG_SPP_MAX_NUM_OF_PORTS_CONTEXTS ports 
 *  from which only BTL_CONFIG_SPP_MAX_NUM_OF_SPP_BUFFERS can use 
 *  TX_SYNC or RX_ASYNC (or both). 
 */
#define BTL_CONFIG_SPP_MAX_NUM_OF_INTERIM_BUFFERS										(3)
#endif
/* Size of a pool with BtPacket type structures used for sending Tx data to
 * RFCOMM */
#define BTL_CONFIG_SPP_TX_MAX_NUM_OF_RFCOMM_PACKETS                                 (20)


/*-------------------------------------------------------------------------------
 * PBAPS
 *
 *     Represents configuration parameters for PBAP module.
 */

/*
*	The maximum number of PBAP Server contexts to be shared among all of the 
*	applications (rather than per application)
*/
#define BTL_CONFIG_PBAPS_MAX_NUM_OF_CONTEXTS										(1)

/*-------------------------------------------------------------------------------
 * A2DP
 *
 *     Represents configuration parameters for A2DP module.
 */

/*
*	The maximum number of A2DP contexts to be shared among all of the 
*	applications (rather than per application)
*/
#define BTL_CONFIG_A2DP_MAX_NUM_OF_CONTEXTS											(1)

/*
	The maximum number of simultaneous SBC streams per A2DP context. 
	This actually means how many sinks can be connected simultaneously 
*/
#define BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT								(2)

/*
	The maximum number of simultaneous MP3 streams per A2DP context. 
	This actually means how many sinks can be connected simultaneously
*/
#define BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT					(2)

/*
	The maximum number of raw media blocks used. 
	The media block is from type PCM/SBC/MP3 depending on the type of the current stream(s)
	This number should be greater than the maximal number of blocks defined in BTHAL_MM.
*/
#define BTL_CONFIG_A2DP_MAX_NUM_RAW_BLOCKS_PER_CONTEXT								(12)

/*
	The maximum number of SBC packets in the internal SBC queue 
*/
#define BTL_CONFIG_A2DP_MAX_NUM_SBC_PACKETS_PER_CONTEXT								(12)

/*
	The maximum number of MP3 packets in the internal MP3 queue
*/
#define BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_PACKETS_PER_CONTEXT					(12)

/*
 *    	The maximum size of an SBC packet.
 */
#define BTL_A2DP_SBC_MAX_DATA_SIZE 														(2 * L2CAP_MTU)

/*
 *   Defines the number of milliseconds that should elapse before we increment bitpool in SBC
 */
#define	BTL_A2DP_INC_BITPOOL_MS															(1000)


/*
 *    	Defines how many bitpool units to increment once BTL_A2DP_INC_BITPOOL_MS elapsed
 */
#define	BTL_A2DP_INC_BITPOOL_VAL															(+1)

/*
 *    	Defines the number of occurrences (of not enough SBC packets) that should occur 
 *		before we decrement bitpool in SBC
 */
#define	BTL_A2DP_DEC_BITPOOL_OCCURRENCES													(4)

/*
 *    	Defines how many bitpool units to decrement once BTL_A2DP_DEC_BITPOOL_OCCURRENCES times
 *		occured
 */
#define	BTL_A2DP_DEC_BITPOOL_VAL															(-1)

/*
 *    Defines how much playback time (in ms) we should accumulate in Q before start sending 
 *    SBC/MP3 packets
 */
#define	BTL_A2DP_PREBUFFER_BEFORE_SEND_THRESHOLD										(100)

/*
 *    Defines how much playback time (in ms) we try accumulate in the buffer at the headset side 
 *    (by sending more than we should)
 */
#define	BTL_A2DP_PREBUFFER_AT_SINK														(200)


/*
 * 	Defines how much to shorten timer delay (in ms )
 *
 *	Normally should be 0
*/
#define	BTL_A2DP_SHORTEN_TIMER															(0)

/*
 * 	Defines how much to increase rate of sending  media, in tenths of percent, e.g. 15 means increase of 1.5%.
 *
 *	Normally should be 0 in all embedded system !! only on PC we allow it to be larger than 0
 */
#define A2DP_RATE_INCREASE_FACTOR   														(0)

/*
 * 	Defines whether the internal SBC encoder is included in the build.
*/
#define BTL_CONFIG_A2DP_SBC_BUILT_IN_ENCODER											(BTL_CONFIG_ENABLED)

/*
 * 	Defines whether the internal sample rate converter is included in the build.
 *	The SRC can covert the following sample frequencies:
 *	1) 8000Hz, 12000Hz, 16000Hz, 24000Hz, 32000Hz to mandatory 48000Hz.
 *	2) 11025Hz, 22050Hz to mandatory 44100Hz.
*/
#define BTL_CONFIG_A2DP_SAMPLE_RATE_CONVERTER											(BTL_CONFIG_ENABLED)

/*
 * 	Defines SRC PCM output block max length in bytes.
 *	This constant is relevant only if BTL_CONFIG_A2DP_SAMPLE_RATE_CONVERTER is enabled.
 *	It should be chosen according to the input PCM block length and the length needed
 *	to contain the output PCM block (see ratios below):
 *  8000Hz  -> 48000Hz (ratio is 6)
 *	12000Hz -> 48000Hz (ratio is 4)
 *  16000Hz -> 48000Hz (ratio is 3)
 *  24000Hz -> 48000Hz (ratio is 2)
 *  32000Hz -> 48000Hz (ratio is 1.5)
 *  11025Hz -> 44100Hz (ratio is 4)
 *  22050Hz -> 44100Hz (ratio is 2)
 *
 *	Explanation: If input PCM block length is X in F frequency with Y ratio, then the 
 *	max PCM block output length should be greater than (X * Y).
 *  Example: Input PCM block length = 1024 bytes
 *			 Input PCM block sample frequency = 22050 Hz
 *		     => Output PCM block length >= (1024 * 2) = 2048 bytes
*/
#define BTL_CONFIG_A2DP_SRC_PCM_OUTPUT_BLOCK_MAX_LEN									(24576)

/*-------------------------------------------------------------------------------
 * 	BTL_A2DP_ALLOW_ROLE_SWITCH
 *
 *     	If enable the system will allow the HS to initiate role switch
 */
#define BTL_A2DP_ALLOW_ROLE_SWITCH														(BTL_CONFIG_ENABLED)



/*
 *	The maximum number of channels per context.
 */
#define BTL_HFAG_MAX_CHANNELS 															(2)

/*
 *	The maximum number of BTL HFAG context that can be created.
 */
#define BTL_HFAG_MAX_CONTEXTS 															(1)

/*
 *	The maximum number of responses to AT commands that can be concurrently sent.
 */
#define BTL_HFAG_MAX_RESPONSES 															(10)

/*
 *	The default audio parms to use when establishing audio connection.
 */
#define BTL_HFAG_DEFAULT_AUDIO_PARMS 													(CMGR_AUDIO_PARMS_S3)

/*
 *	The default audio format used by the HFAG module.
 */
#define BTL_HFAG_DEFAULT_AUDIO_FORMAT 													(BSAS_DEFAULT)

 /* 
  *	Number of Modem + Handsfree Unit events that may be queued for transmission by HCI 
  */
#define BTL_CONFIG_HFAG_AT_MAX_INDICATORS                       									20

/*-------------------------------------------------------------------------------
 *	The maximum number of channels a context can have.
 */
#define BTL_HSAG_MAX_CHANNELS 															(2)

/*
 *	The maximum number of BTL HFAG context that can be created.
 */
#define BTL_HSAG_MAX_CONTEXTS 															(1)

/*
*	The maximum number of responses to AT commands that can be concurrently sent.
*/
#define BTL_HSAG_MAX_RESPONSES 															(3)

/*
 *	The default audio parms to use when establishing audio connection.
 */
#define BTL_HSAG_DEFAULT_AUDIO_PARMS 													(CMGR_AUDIO_PARMS_SCO)

/*
 *	The default audio format used by the HSAG module.
 */
#define BTL_HSAG_DEFAULT_AUDIO_FORMAT 													(BSAS_DEFAULT)

/*-------------------------------------------------------------------------------
 * BPPSND
 *
 *     Represents configuration parameters for BPPSND module.
 */

/*
*	The maximum number of BPP Sender contexts to be shared among all of the 
*	applications (rather than per application)
*/
#define BTL_CONFIG_BPPSND_MAX_NUM_OF_CONTEXTS											(1)

/*
*	The printer might open an object channel for referenced objects.
*	So, before we disconnect ACL, we wait for BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT ms
*	for the printer to establish the object channel.
*	If such channel is estabished, we disconnect only after the printer disconnect
*	this object channel.
*	If no such channel is established, we disconnect when the timer expired.
*	
*	This constant can be set to 0, if the sender does not want to wait before 
*	disconnecting ACL.
*
*	Default value is 1 sec.
*/
#define BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT											(1000)


/*-------------------------------------------------------------------------------
 * RFCOMM
 *
 *     Represents configuration parameters for RFCOMM module.
 */
 
/*
*	The maximum number of RFCOMM contexts to be shared among all of the 
*	applications (rather than per application)
*/
#define BTL_CONFIG_RFCOMM_MAX_NUM_OF_CONTEXTS											(2)

/*
	The maximum number of channels per context
*/
#define BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT								(4)

/*
	The maximum number of TX packets
*/
#define BTL_CONFIG_RFCOMM_MAX_NUM_OF_TX_PACKETS										(10)

/*-------------------------------------------------------------------------------
 * VG
 *
 *     Represents configuration parameters for VG module.
 */

/*
	The maximum number of Headset channels per context
*/
#define BTL_CONFIG_VG_MAXIMUM_NUMBER_OF_HEADSET_CHANNELS							(1)

/*
	The maximum number of Handsfree channels per context
*/
#define BTL_CONFIG_VG_MAXIMUM_NUMBER_OF_HANDSFREE_CHANNELS							(2)

/*-------------------------------------------------------------------------------
 * L2CAP
 *
 *     Represents configuration parameters for L2CAP module.
 */

/*
*	The maximum number of L2CAP contexts to be shared among all of the 
*	applications (rather than per application)
*/
#define BTL_CONFIG_L2CAP_MAX_NUM_OF_CONTEXTS											(2)

/* 
 * 	The total no. of connections (L2CAP channels) that L2CAP can handle  (incoming + outgoing together)                                  
 */
#define BTL_CONFIG_L2CAP_MAX_NUM_CHANNELS		       									 (8)

/*
	The maximum number of channels per context
*/
#define BTL_CONFIG_L2CAP_MAX_NUM_CHANNELS_PER_CONTEXT								(4)

/*
	The maximum number of TX packets
*/
#define BTL_CONFIG_L2CAP_MAX_NUM_OF_TX_PACKETS											(10)

/*---------------------------------------------------------------------------
 * BTL_CONFIG_L2CAP_MTU constant
 *
 *     Defines the largest receivable L2CAP data packet payload, in bytes.
 *     This limitation applies only to packets received from the remote device.
 *
 *     This constant also affects the L2CAP Connectionless MTU. Some profiles
 *     require a minimum Connectionless MTU that is greater than the L2CAP
 *     minimum of 48 bytes. When implementing such a profile it is important
 *     that the BTL_CONFIG_L2CAP_MTU constant be at least as large as the specified
 *     Connectionless MTU size.
 *
 *    This value may range from 48 to 65529. The default value is 672 bytes.
 *
 *	The current value of 2038 was chosen in order to achieve optimized EDR throughput
 */
#define BTL_CONFIG_L2CAP_MTU																(2038)

/*-------------------------------------------------------------------------------
 * SAPS
 *
 *     Represents configuration parameters for SAP server module.
 */
 
/* 
*	The maximum number of SAP Server contexts to be shared among all of the 
*	applications (rather than per application)
 */
#define BTL_CONFIG_SAPS_MAX_NUM_OF_CONTEXTS											 (1) 

/* 
 * 	The maximum number of bytes in a serviceName.    
 */
#define BTL_CONFIG_SAPS_MAX_SERVICE_NAME     												(20) 


/*-------------------------------------------------------------------------------
 * BIPINT
 *
 *     Represents configuration parameters for BIP Initiator (BIPINT) module.
 */
 
/* 
*	The maximum number of BIP Initiator contexts to be shared among all of the 
*	applications (rather than per application)
 */
#define BTL_CONFIG_BIPINT_MAX_NUM_OF_CONTEXTS						   					(1)

/*-------------------------------------------------------------------------------
 * BIPRSP
 *
 *     Represents configuration parameters for BIP Responder (BIPRSP) module.
 */

/* 
*	The maximum number of BIP Responder contexts to be shared among all of the 
*	applications (rather than per application)
 */
#define BTL_CONFIG_BIPRSP_MAX_NUM_OF_CONTEXTS   										(1)


/*---------------------------------------------------------------------------
 * OBEX_AUTHENTICATION constant
 *
 *     This option enables OBEX layer support for OBEX Authentication Challenge
 *     and Response headers. OBEX Authentication support includes functions 
 *     necessary to build and parse authentication challenge and response
 *     headers. There is also a function for verifying that an authentication
 *     response used the correct password.
 */

#define BTL_CONFIG_OBEX_AUTHENTICATION                                      BTL_CONFIG_ENABLED

#if BTL_CONFIG_OBEX_AUTHENTICATION == BTL_CONFIG_ENABLED
/*-------------------------------------------------------------------------------
 * OBEX AUTHENTICATION
 *
 *     Represents configuration parameters for OBEX  modules that use OBEX AUTHENTICATION
 */

/* 
*	The maximum OBEX USER_ID len that can be used 
*	
 */
#define BTL_CONFIG_OBEX_MAX_USER_ID_LEN        												(20)

/* 
*	The maximum OBEX PASSWROD  len that can be used 
*	
 */
#define BTL_CONFIG_OBEX_MAX_PASS_LEN        												(11)

/* 
*	The maximum OBEX REALM len that can be used 
*	
 */
#define BTL_CONFIG_OBEX_MAX_REALM_LEN        												(20)

#endif /*BTL_CONFIG_OBEX_AUTHENTICATION == BTL_CONFIG_ENABLED*/


/*---------------------------------------------------------------------------
 * BTL_HCI_HOST_FLOW_CONTROL constant
 *
 *     Controls whether HCI applies flow control to data received from the
 *     host controller.
 *
 *     When BTL_HCI_HOST_FLOW_CONTROL is enabled, the HCI layer uses
 *     HCC_HOST_NUM_COMPLETED_PACKET commands to periodically tell the
 *     radio hardware how many receive packets it can accept. This is
 *     necessary if the HCI driver's receive buffers could overflow
 *     with incoming data.
 *
 *     When BTL_HCI_HOST_FLOW_CONTROL is disabled, the HCI driver assumes
 *     that it can process data faster than the radio hardware can generate
 *     it.
 */
#define BTL_HCI_HOST_FLOW_CONTROL											BTL_CONFIG_ENABLED

#endif /* __BTL_CONFIG_H */



