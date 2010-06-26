#ifndef __SPP_H
#define __SPP_H
/***************************************************************************
 *
 * File:
 *     $Workfile:spp.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:34$
 *
 * Description:
 *      Private header file for the Serial Port Profile. 
 *      This file is meant to be included by modules in the portable
 *      SPP and the OS-specific portion of the SPP (SPPOS). It defines
 *      the portable API, data types, and defines presented and required by
 *      the portable code.
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

#include "overide.h"
#if TI_CHANGES == XA_ENABLED
#include "bttypes.h"
#include "utils.h"
#include "rfcomm.h"
#include "sdp.h"
#include "conmgr.h"


/*---------------------------------------------------------------------------
 * Serial Port Profile (SPP) layer
 *
 *     The Serial Port Profile (SPP) specification defines procedures
 *     and protocols for Bluetooth devices emulating RS232 or other serial
 *     connections.
 */


/****************************************************************************
 *
 * Section: Configuration Constants
 *
 * The following defines are configuration constants that allow
 * an implementer to include/exclude functionality from SPP.
 *
 ****************************************************************************/
 
/*---------------------------------------------------------------------------
 * SPP_SERVER constant
 *      Configuration constant to enable code for Serial Port Profile
 *      server. If the device is client-only, this should be defined as
 *      XA_DISABLED in overide.h.
 */
#ifndef SPP_SERVER
#define SPP_SERVER                          (XA_ENABLED)
#endif

/*---------------------------------------------------------------------------
 * SPP_CLIENT constant
 *      Configuration constant to enable code for Serial Port Profile
 *      client. If the device is server-only, this should be defined as
 *      XA_DISABLED in overide.h.
 */
#ifndef SPP_CLIENT
#define SPP_CLIENT                          (XA_ENABLED)
#endif

/*---------------------------------------------------------------------------
 * SPP_TX_MAX_NUM_OF_RFCOMM_PACKETS constant
 *     Defines size of a pool with BtPacket type structures used for sending Tx
 *      data to RFCOMM.
 */
#ifndef SPP_TX_MAX_NUM_OF_RFCOMM_PACKETS
#define SPP_TX_MAX_NUM_OF_RFCOMM_PACKETS     (20)
#endif

/*---------------------------------------------------------------------------
 * SPP_SECURITY_SETTINGS_DEFAULT constant
 *     Defines the security settings for a SPP connection.  The default
 *     is set to BSL_DEFAULT (see BtSecurityLevel).
 */
#ifndef SPP_SECURITY_SETTINGS_DEFAULT
#define SPP_SECURITY_SETTINGS_DEFAULT       (BSL_AUTHENTICATION_IN | \
                                             BSL_AUTHENTICATION_OUT)
#endif                               

/*---------------------------------------------------------------------------
 * SPP_SECURITY_SETTINGS_NO_SECURITY constant
 *     Defines the security settings for a SPP connection without security.
 *     The default is set to BSL_DEFAULT (see BtSecurityLevel).
 */
#ifndef SPP_SECURITY_SETTINGS_NO_SECURITY
#define SPP_SECURITY_SETTINGS_NO_SECURITY   (BSL_NO_SECURITY)
#endif                               

/*---------------------------------------------------------------------------
 * SPP_SERVICE_NAME_MAX_SIZE constant
 *
 *     Defines a maximal size of a service name.
 */
#define SPP_SERVICE_NAME_MAX_SIZE		    (35)

 
/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SppPortType type
 * 
 *		Identifies the serial port as a client (outgoing) or server (incoming). 
 */
typedef U8 SppPortType;

#define SPP_SERVER_PORT                 (0x01)
#define SPP_CLIENT_PORT                 (0x02)

/* End of SppPortType */

/*---------------------------------------------------------------------------
 * SppUserType type
 * 
 *		Identifies type of the port regarding to kind of service and client/
 *		server relationship. 
 */
typedef U8 SppUserType;

#define SPP_USER_TYPE_DIALUP_SERVER     (1)
#define SPP_USER_TYPE_LAN_SERVER        (2)
#define SPP_USER_TYPE_FAX_SERVER        (3)
#define SPP_USER_TYPE_SERIAL_SERVER     (4)
#define SPP_USER_TYPE_DIALUP_CLIENT     (5)
#define SPP_USER_TYPE_LAN_CLIENT        (6)
#define SPP_USER_TYPE_FAX_CLIENT        (7)
#define SPP_USER_TYPE_SERIAL_CLIENT     (8)

/*-------------------------------------------------------------------------------
 * SppPortDataPathType type
 *
 *     Defines the SPP port data path type.
 */
typedef U8 SppPortDataPathType;

/* Data will be written using interim SPP Tx buffer and data segmentation will
 * be done internally according to the RFCOMM channel 'maxFrameSize'. */
#define SPP_PORT_DATA_PATH_TX_SYNC      (0x01)

/* Data will be written directly to the RFCOMM layer and data segmentation will
 * be done internally according to the RFCOMM channel 'maxFrameSize'.
 * The application will receive SPP_EVENT_TX_DATA_COMPLETE event, when all the
 * data is sent. */
#define SPP_PORT_DATA_PATH_TX_ASYNC     (0x02)

/* The application will receive SPP_EVENT_RX_DATA_IND event with pointer to data
 * and its length. It MUST copy the data to its buffer and release the callback
 * function, which delivered the event, as fast, as possible in order BT stack
 * may continue its operations. */
#define SPP_PORT_DATA_PATH_RX_SYNC      (0x04)

/* Rx data will be copied into interim buffers of SPP profile before the
 * application will receive SPP_EVENT_RX_DATA_IND event, in which only length
 * of the received data will be set, while pointer to the data will be set to
 * NULL. The application needs to read the data using function
 * BTL_SPP_ReadAsync(). */
#define SPP_PORT_DATA_PATH_RX_ASYNC     (0x08)

/*-------------------------------------------------------------------------------
 * SppFlushType type
 *
 *	    Type of SPP data flush.
 */
typedef U8 SppFlushType;

/* Flush will be applied only to Rx path */
#define  SPP_FLUSH_TYPE_RX              (0x01)

/* Flush will be applied only to Tx path */
#define  SPP_FLUSH_TYPE_TX              (0x02)

/* Flush will be applied to both Rx and Tx paths */
#define  SPP_FLUSH_TYPE_ALL             (0x03)

#if SPP_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * SppService structure
 *
 *      Servers need this data type for registering with RFCOMM. A particular
 *      service may have one or more ports available. For example, a device
 *      might provide access to 2 modems. It is providing a single service
 *      (modem) via 2 serial ports.
 */
typedef struct _SppService
{
    RfService    service;       /* Required for registering service w/RFCOMM */
    U8			 name[SPP_SERVICE_NAME_MAX_SIZE]; /* name of service */
    U16          numPorts;      /* Number of ports in this service */
    SdpRecord    sdpRecord;     /* Description of the service */

	/* Private copy of the SDP attributes (SPP need 6 SdpAttribute, 
	DUN need 7 SdpAttribute and FAX need 10 SdpAttribute) for service name */
	SdpAttribute sdpAttribs[10];

	/* Private copy of protocol descriptor list for RFCOMM server index */
	U8 protocolDescriptorList[14];

} SppService;

#endif /* SPP_SERVER == XA_ENABLED */

#if SPP_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * SppClient structure
 *      Contains all fields unique to clients. SppClient is a data type in
 *      device structures (SppDev) for clients.
 */
typedef struct _SppClient
{
    /* === Internal use only === */
    BtRemoteDevice      *remDev;
    U8                  serverId;
    SdpQueryToken       sdpToken;
	U8					*serviceName;
} SppClient;
#endif


/*---------------------------------------------------------------------------
 * SppEvent type
 *      
 */
typedef RfEvent SppEvent;

/* A remote device has requested a connection to a local SPP port. */
#define SPP_EVENT_OPEN_IND					(RFEVENT_OPEN_IND)

/* A channel is now open and ready for data exchange. */
#define SPP_EVENT_OPEN						(RFEVENT_OPEN)

/* A request to close a channel was received. */
#define SPP_EVENT_CLOSE_IND					(RFEVENT_CLOSE_IND)

/* The channel is closed. If disconnection was initiated by the remote device,
 * all data in received and transmit buffers should be discarded by explicit
 * call to SPP_Flush() function.
 */
#define SPP_EVENT_CLOSED					(RFEVENT_CLOSED)

/* RX event: data was received from the remote device. */
#define SPP_EVENT_RX_DATA_IND				(RFEVENT_DATA_IND)

/* TX complete event on previously sent RFCOMM packet. Indicates that more space
 * is available in transmit buffer. */
#define SPP_EVENT_TX_DATA_COMPLETE			(RFEVENT_PACKET_HANDLED)

#if RF_SEND_TEST == XA_ENABLED
/** The remote device has responded to a test sent by RF_SendTest.
 * The "status" field contains BT_STATUS_SUCCESS if the test succeeded,
 * or BT_STATUS_FAILED if the response did not match.
 *
 * This event is only available when RF_SEND_TEST is enabled.
 */
#define SPP_EVENT_TEST_CNF					(RFEVENT_TEST_CNF)
#endif /* RF_SEND_TEST enabled */

/* The remote device has requested negotiation of port settings. */
#define SPP_EVENT_PORT_NEG_IND				(RFEVENT_PORT_NEG_IND)

#if RF_SEND_CONTROL == XA_ENABLED
/* Port negotiation confirmed. */
#define SPP_EVENT_PORT_NEG_CNF				(RFEVENT_PORT_NEG_CNF)
#endif /* RF_SEND_CONTROL enabled */

/* The remote device has requested the status of the port settings. */
#define SPP_EVENT_PORT_STATUS_IND			(RFEVENT_PORT_STATUS_IND)

#if RF_SEND_CONTROL == XA_ENABLED
/* The remote device has responded to a request for its current port status. */
#define SPP_EVENT_PORT_STATUS_CNF			(RFEVENT_PORT_STATUS_CNF)
#endif /* RF_SEND_CONTROL enabled */

/* The remote device has provided modem status. */
#define SPP_EVENT_MODEM_STATUS_IND			(RFEVENT_MODEM_STATUS_IND)

/* The remote device has acknowledged new modem status settings. */
#define SPP_EVENT_MODEM_STATUS_CNF			(RFEVENT_MODEM_STATUS_CNF)

#if RF_SEND_CONTROL == XA_ENABLED
/* The remote device has provided line status information. */
#define SPP_EVENT_LINE_STATUS_IND			(RFEVENT_LINE_STATUS_IND)

/* RFCOMM has finished sending line status to the remote device. */
#define SPP_EVENT_LINE_STATUS_CNF			(RFEVENT_LINE_STATUS_CNF)
#endif /* RF_SEND_CONTROL enabled */

/* The remote device has indicated that no RFCOMM data can be processed. */
#define SPP_EVENT_FLOW_OFF_IND				(RFEVENT_FLOW_OFF_IND)

/* The remote device has indicated that RFCOMM data can be processed again. */
#define SPP_EVENT_FLOW_ON_IND				(RFEVENT_FLOW_ON_IND)

/* The application will receive this event, when the SDP query failed. */
#define SPP_EVENT_SDP_QUERY_FAILED			(RFEVENT_SDP_QUERY_FAILED)

/* The application will receive this event, when establishing of ACL link for
 * SPP client port to server port failed. */
#define SPP_EVENT_ACL_CREATION_FAILED		(RFEVENT_ACL_CREATION_FAILED)

/* Request of remote device to connect to local SPP port failed because of
 * security reason. */
#define SPP_EVENT_OPEN_IND_SEC_FAIL         (RFEVENT_OPEN_IND_SEC_FAIL)

/* Indicates that the port is disabled */
#define SPP_EVENT_DISABLED                  (22)

/*---------------------------------------------------------------------------
 * SppControlSignals type
 *
 *     Represents status of the SPP port signals. They may be set at a port
 *	   opening or later and may be received during with events
 *	   SPP_EVENT_MODEM_STATUS_IND or SPP_EVENT_MODEM_STATUS_CNF.
 */
typedef RfModemStatus SppControlSignals;


/* Group: Any of the following signals may be set or clear. They represent
 * standard TS 7.10 Terminology for expressing each possible signal.
 */

/* Set when sender is ready to communicate. */
#define SPP_CONTROL_SIGNAL_RTC				(RF_RTC)

/* Set when sender is ready to receive data. */
#define SPP_CONTROL_SIGNAL_RTR				(RF_RTR)

/* Set when a call is incoming. */
#define SPP_CONTROL_SIGNAL_IC				(RF_IC)

/* Set when valid data is being sent. */
#define SPP_CONTROL_SIGNAL_DV				(RF_DV)

/* Group: The following signal names represent a subset of the TS 7.10 signals
 * above. They represent the signals that a DCE would send to a DTE device.
 */
#define SPP_CONTROL_SIGNAL_DSR				(RF_DSR)
#define SPP_CONTROL_SIGNAL_CTS				(RF_CTS)
#define SPP_CONTROL_SIGNAL_RI				(RF_RI)
#define SPP_CONTROL_SIGNAL_CD				(RF_CD)

/* Group: The following signal names represent a subset of the TS 7.10 signals
 * above. They represent the signals that a DTE would send to a DCE device.
 */
#define SPP_CONTROL_SIGNAL_DTR				(RF_DTR)
#define SPP_CONTROL_SIGNAL_RTS				(RF_RTS)

 /*---------------------------------------------------------------------------
 * SppPortSettings type
 *
 *     Represents the SPP port settings. They may be set at a port opening or
 *	   later and may be received during port negotiation with events
 *	   SPP_EVENT_PORT_STATUS_IND or SPP_EVENT_PORT_STATUS_CNF.
 */
typedef RfPortSettings SppComSettings;

/*---------------------------------------------------------------------------
 * SppBaudRate type
 *
 *     Specifies the baud rate in an SppComSettings structure.
 */
typedef RfBaudRate SppComSettingBaudRate;

#define SPP_COM_SETTING_BAUDRATE_2400			(RF_BAUD_2400)
#define SPP_COM_SETTING_BAUDRATE_4800			(RF_BAUD_4800)
#define SPP_COM_SETTING_BAUDRATE_7200			(RF_BAUD_7200)
#define SPP_COM_SETTING_BAUDRATE_9600			(RF_BAUD_9600)
#define SPP_COM_SETTING_BAUDRATE_19200			(RF_BAUD_19200)
#define SPP_COM_SETTING_BAUDRATE_38400			(RF_BAUD_38400)
#define SPP_COM_SETTING_BAUDRATE_57600			(RF_BAUD_57600)
#define SPP_COM_SETTING_BAUDRATE_115200			(RF_BAUD_115200)
#define SPP_COM_SETTING_BAUDRATE_230400			(RF_BAUD_230400)

#define SPP_COM_SETTING_BAUDRATE_DEFAULT		(RF_BAUD_9600)

/*---------------------------------------------------------------------------
 * SppComSettingDataFormat type
 *
 *     Specifies the data bits, stop bits, and parity in SppComSettingDataFormat
 *     structure. The data bits, stop bits, and parity settings are OR'ed
 *     together in the "dataFormat" field.
 */
typedef RfDataFormat SppComSettingDataFormat;

/* Group: These values identify the number of data bits. */
#define SPP_COM_SETTING_DATA_BITS_5				(RF_DATA_BITS_5)
#define SPP_COM_SETTING_DATA_BITS_6				(RF_DATA_BITS_6)
#define SPP_COM_SETTING_DATA_BITS_7				(RF_DATA_BITS_7)
#define SPP_COM_SETTING_DATA_BITS_8				(RF_DATA_BITS_8)

/* Group: These values identify the number of stop bits. */
#define SPP_COM_SETTING_STOP_BITS_1				(RF_STOP_BITS_1)
#define SPP_COM_SETTING_STOP_BITS_1_5			(RF_STOP_BITS_1_5)

/* Group: These values identify whether of parity is used. */
#define SPP_COM_SETTING_PARITY_NONE				(RF_PARITY_NONE)
#define SPP_COM_SETTING_PARITY_ON				(RF_PARITY_ON)

/* Group: These values identify the type of parity in use. Note that
 * these values have no effect unless parity is enabled (RF_PARITY_ON). */
#define SPP_COM_SETTING_PARITY_TYPE_ODD			(RF_PARITY_TYPE_ODD)
#define SPP_COM_SETTING_PARITY_TYPE_EVEN		(RF_PARITY_TYPE_EVEN)
#define SPP_COM_SETTING_PARITY_TYPE_MARK		(RF_PARITY_TYPE_MARK)
#define SPP_COM_SETTING_PARITY_TYPE_SPACE		(RF_PARITY_TYPE_SPACE)

#define SPP_COM_SETTING_DATA_FORMAT_DEFAULT		(SPP_COM_SETTING_DATA_BITS_8 |\
												 SPP_COM_SETTING_STOP_BITS_1 |\
												 SPP_COM_SETTING_PARITY_NONE)

 /* Group: These masks allow you to select only the relevant bits from
 * the "dataFormat" field.
 */

/* AND with "dataFormat" for the data bits setting. */
#define SPP_COM_SETTING_MASK_DATA_BITS			(RF_DATA_BITS_MASK)

/* AND with "dataFormat" for the stop bits setting. */
#define SPP_COM_SETTING_MASK_STOP_BITS			(RF_STOP_BITS_MASK)

/* AND with "dataFormat" to determine whether parity is on.  */
#define SPP_COM_SETTING_MASK_PARITY				(RF_PARITY_MASK)

/* AND with "dataFormat" to determine the type of parity selected. */
#define SPP_COM_SETTING_MASK_PARITY_TYPE		(RF_PARITY_TYPE_MASK)

/*---------------------------------------------------------------------------
 * SppComSettingFlowControl type
 *
 *     Specifies the port flow control type in an SppComSettingFlowControl
 *	   structure. This refers to flow control for RS232 emulation.
 */
typedef RfFlowControl SppComSettingFlowControl;

#define SPP_COM_SETTING_FLOW_CTRL_NONE			(RF_FLOW_CTRL_NONE)
#define SPP_COM_SETTING_FLOW_CTRL_XON_ON_INPUT	(RF_XON_ON_INPUT)
#define SPP_COM_SETTING_FLOW_CTRL_XON_ON_OUTPUT	(RF_XON_ON_OUTPUT)
#define SPP_COM_SETTING_FLOW_CTRL_RTR_ON_INPUT	(RF_RTR_ON_INPUT)
#define SPP_COM_SETTING_FLOW_CTRL_RTR_ON_OUTPUT	(RF_RTR_ON_OUTPUT)
#define SPP_COM_SETTING_FLOW_CTRL_RTC_ON_INPUT	(RF_RTC_ON_INPUT)
#define SPP_COM_SETTING_FLOW_CTRL_RTC_ON_OUTPUT	(RF_RTC_ON_OUTPUT)

/* Group: These symbols define common combinations of port flow control
 * settings used in RS232 emulation
 */
#define SPP_COM_SETTING_FLOW_CTRL_RTS_CTS		(SPP_COM_SETTING_FLOW_CTRL_RTR_ON_INPUT |\
												 SPP_COM_SETTING_FLOW_CTRL_RTR_ON_OUTPUT)
#define SPP_COM_SETTING_FLOW_CTRL_DTR_DSR		(SPP_COM_SETTING_FLOW_CTRL_RTC_ON_INPUT |\
												 SPP_COM_SETTING_FLOW_CTRL_RTC_ON_OUTPUT)
#define SPP_COM_SETTING_FLOW_CTRL_XON_XOFF		(SPP_COM_SETTING_FLOW_CTRL_XON_ON_INPUT |\

#define DC1										(0x11)  /* Default XON char */
#define DC3										(0x13)	/* Default XOFF char */

#define SPP_COM_SETTING_XON_CHAR_DEFAULT		(DC1)
#define SPP_COM_SETTING_XOFF_CHAR_DEFAULT		(DC3)

#define SPP_COM_SETTING_FLOW_CONTROL_DEFAULT	(SPP_COM_SETTING_FLOW_CTRL_NONE)

/*---------------------------------------------------------------------------
 * SppComSettingsParmMask type
 *
 *     Used to specify which settings in an SppComSettings structure are
 *     valid.  Port settings are used for RS232 emulation.
 */
typedef RfPortSettingsMask SppComSettingsParmMask;

/* The "baudRate" field is valid. */
#define SPP_COM_SETTINGS_PARM_MASK_BAUDRATE		(RF_PARM_BAUDRATE)

/* The DATA_BITS part of the "dataFormat" field is valid. */
#define SPP_COM_SETTINGS_PARM_MASK_DATABITS		(RF_PARM_DATABITS)

/* The STOP_BITS_part of the "dataFormat" field is valid. */
#define SPP_COM_SETTINGS_PARM_MASK_STOPBITS		(RF_PARM_STOPBITS)

/* The PARITY_NONE or PARITY_ON part of the "dataFormat" field is valid. */
#define SPP_COM_SETTINGS_PARM_MASK_PARITY		(RF_PARM_PARITY)

/* The PARITY_TYPE part of the "dataFormat" field is valid. */
#define SPP_COM_SETTINGS_PARM_MASK_PARITY_TYPE	(RF_PARM_PARITY_TYPE)

/* All elements in the "dataFormat" field are valid. */
#define SPP_COM_SETTINGS_PARM_MASK_DATA_FORMAT	(SPP_COM_SETTINGS_PARM_MASK_DATABITS |\
												 SPP_COM_SETTINGS_PARM_MASK_STOPBITS |\
												 SPP_COM_SETTINGS_PARM_MASK_PARITY |\
												 SPP_COM_SETTINGS_PARM_MASK_PARITY_TYPE)

/* The "xonChar" field is valid. */
#define SPP_COM_SETTINGS_PARM_MASK_XON_CHAR		(RF_PARM_XON_CHAR)

/* The "xoffChar" field is valid. */
#define SPP_COM_SETTINGS_PARM_MASK_XOFF_CHAR	(RF_PARM_XOFF_CHAR)

/* Group: The following fields identify whether specific bits in the 
 * "flowControl" field are valid. They are equivalent to the values
 * described in the RfFlowControl type.
 */
#define SPP_COM_SETTINGS_PARM_MASK_XON_INPUT	(RF_PARM_XON_INPUT)
#define SPP_COM_SETTINGS_PARM_MASK_XON_OUTPUT	(RF_PARM_XON_OUTPUT)
#define SPP_COM_SETTINGS_PARM_MASK_RTR_INPUT	(RF_PARM_RTR_INPUT)
#define SPP_COM_SETTINGS_PARM_MASK_RTR_OUTPUT	(RF_PARM_RTR_OUTPUT)
#define SPP_COM_SETTINGS_PARM_MASK_RTC_INPUT	(RF_PARM_RTC_INPUT)
#define SPP_COM_SETTINGS_PARM_MASK_RTC_OUTPUT	(RF_PARM_RTC_OUTPUT)

#define SPP_COM_SETTINGS_PARM_MASK_RTS_CTS		(SPP_COM_SETTINGS_PARM_MASK_RTR_INPUT |\
												 SPP_COM_SETTINGS_PARM_MASK_RTR_OUTPUT)
#define SPP_COM_SETTINGS_PARM_MASK_DTR_DSR		(SPP_COM_SETTINGS_PARM_MASK_RTC_INPUT |\
												 SPP_COM_SETTINGS_PARM_MASK_RTC_OUTPUT)
#define SPP_COM_SETTINGS_PARM_MASK_XON_XOFF		(SPP_COM_SETTINGS_PARM_MASK_XON_INPUT |\
												 SPP_COM_SETTINGS_PARM_MASK_XON_OUTPUT)

#define SPP_COM_SETTINGS_PARM_MASK_DEFAULT		(SPP_COM_SETTINGS_PARM_MASK_DATA_FORMAT |\
												 SPP_COM_SETTINGS_PARM_MASK_DATABITS |\
												 SPP_COM_SETTINGS_PARM_MASK_STOPBITS)

/*---------------------------------------------------------------------------
 * SppLineStatus type
 *
 *     Represents the line status of the SPP port. It may be received with
 *	   events SPP_EVENT_LINE_STATUS_IND and SPP_EVENT_LINE_STATUS_CNF.
 */
typedef RfLineStatus SppLineStatus;

/* When set, one or more errors have occurred. Any of the following bits
 * may be set to indicate an error. If SPP_LINE_STATUS_ERROR is clear, no error
 * has occurred, regardless of the state of the other bits.
 */
#define SPP_LINE_STATUS_NO_ERROR				(0)

#define SPP_LINE_STATUS_ERROR					(RF_LINE_ERROR)

/* Set to indicate an overrun error. */
#define SPP_LINE_STATUS_OVERRUN					(RF_OVERRUN)

/* Set to indicate a parity error. */
#define SPP_LINE_STATUS_PARITY					(RF_PARITY)

/* Set to indicate a framing error. */
#define SPP_LINE_STATUS_FRAMING					(RF_FRAMING)


/* Forward declarations */
typedef struct _SppDev SppDev;
typedef struct _SppBuf SppBuf;
typedef struct _SppCallbackParms SppCallbackParms;

/*---------------------------------------------------------------------------
 * SppCallback type
 *
 * A function of this type is called to indicate events to the application.
 */
typedef void (*SppCallback)(SppDev *dev, SppCallbackParms *parms);

/*---------------------------------------------------------------------------
 * SppPortOpenInfo structure
 *
 *      Contains additional information passed with SPP_EVENT_OPEN event.
 */
typedef struct _SppPortOpenInfo
{
	BD_ADDR					bdAddr;     	/* BD ADDR of the remote device */
	U16						maxFrameSize;   /* Frame size used for the channel */

} SppPortOpenInfo;

/*---------------------------------------------------------------------------
 * SppPortRxDataInfo structure
 *
 *      Contains information about Rx data passed with SPP_EVENT_RX_DATA_IND event.
 */
typedef struct _SppPortRxDataInfo
{
	U8                      *rxData;     	/* Pointer to received data */
	U16					    rxDataLen;	    /* Length of received data */

} SppPortRxDataInfo;

/*---------------------------------------------------------------------------
 * SppPortTxDataInfo structure
 *
 *      Contains information about Tx data passed with SPP_EVENT_TX_DATA_COMPLETE
 *      event.
 */
typedef struct _SppPortTxDataInfo
{
	U8                      *txData;     	/* Pointer to transmitted data */
	U16					    txDataLen;	    /* Length of transmitted data */

} SppPortTxDataInfo;

/*---------------------------------------------------------------------------
 * SppCallbackParms structure
 *
 * Contains information for the application callback event.
 */
#if (TI_CHANGES == XA_ENABLED)
struct _SppCallbackParms
#else
typedef struct _SppCallbackParms
#endif
{
    SppEvent				event;			/* SPP event */
	BtStatus				status;			/* Status of the callback event */
	SppLineStatus           lineStatus;     /* Line status error may be fired,
                                             * when receiving data in case of 
                                             * local buffer overrun */

	/* Parameters */
	union {
		SppPortOpenInfo		openInfo;		/* BD ADDR, max frame size */
		SppComSettings		portSettings;	/* Port settings: baudrate, parity... */
		SppControlSignals	controlSignals;	/* Port control signals: CTS-RTS, DCD... */
		SppPortRxDataInfo	rxDataInfo;     /* Info about received data */
        SppPortTxDataInfo   txDataInfo;     /* Info about transmitted data */        
	} p;

#if (TI_CHANGES == XA_ENABLED)
} ;
#else
} SppCallbackParms;
#endif


/*---------------------------------------------------------------------------
 * SppDev structure
 *      This structure defines an SPP device. A user of SPP must allocate
 *      one SppDev for each serial device. Members should not be accessed
 *      directly.
 */
#if (TI_CHANGES == XA_ENABLED)
struct _SppDev
#else
typedef struct _SppDev
#endif
{
    SppPortDataPathType dataPathTypes;

#if (SPP_USE_INTERIM_TX_BUFFER == BTL_CONFIG_ENABLED) ||\
    (SPP_USE_INTERIM_RX_BUFFER == BTL_CONFIG_ENABLED)
    /* Pointer to interim SPP buffers information */
    SppBuf              *sppBuf;
#endif

	/* Application callback function, must be set by application  */
	SppCallback		    callback;

    /* === Internal use only === */

    /* Server / Client elements */
    SppPortType         portType;       /* SPP_SERVER or SPP_CLIENT */
	SppUserType		    userType;		/* Type of service together with server/client */
    U8                  state;          /* device state */
    RfChannel           channel;
    RfModemStatus       rModemStatus;   /* remote modem status */
    RfModemStatus       lModemStatus;   /* local modem status */
    RfPortSettings      portSettings;
    
    S16                 highWater;      /* when rRing.dataLen >= highWater,
                                           flow off rx */
    U8                  credit;         /* rx credits outstanding */
    S16                 breakLen;

    /* Transmit packets */
    ListEntry           txPacketList;

    /* Tx Packets for each device. The number is arbitrary. >1 is recommended
	 * for high rates of outbound data. */
    BtPacket	        txPacket[SPP_TX_MAX_NUM_OF_RFCOMM_PACKETS];

    /* Pointer to data to be transmitted and its length */
    U8                  *txAsyncData;
    U16                 txAsyncDataLen;
    
    /* Amount of sent and acknowledged data */
    U16                 txAsyncDataSent;
    U16                 txAsyncDataAcked;

    /* Flag indicating that flush is in progress for Tx path, therefor don't sent Tx_Complete indication */
    BOOL                blockTxCompAfterFlush;
    
    /* Maximum amount of Rx packets to which amount of RFCOMM credits may be
	 * initialized */
	U8			        rxMaxNumOfPackets;

    /* Connection Manager callback */
	CmgrHandler		    cmgrHandler;

#if BT_SECURITY == XA_ENABLED
    /* Security record */
    BtSecurityRecord    secRec;
#endif

    /* Fields specific to clients and servers */
    union
    {
#if SPP_CLIENT == XA_ENABLED
        SppClient       client;
#endif
#if SPP_SERVER == XA_ENABLED
        SppService      sppService;
#endif
    } type;
    
};


/****************************************************************************
 *
 * Section: Driver Initialization Functions
 *
 * These APIs are called typically when the driver is initialized or loaded.
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * SPP_InitDevice()
 *
 *      Initializes a Serial Port Profile device structure. Call this API
 *      for each SPP device before using it for any other SPP API. 
 *     
 * Parameters:
 *      dev - pointer to existing device structure
 */
BtStatus SPP_InitDevice(SppDev *dev);

/*---------------------------------------------------------------------------
 * SPP_DeinitDevice()
 *
 *      Deinitializes a Serial Port Profile device structure. 
 *     
 * Parameters:
 *      dev - pointer to existing device structure
 */
BtStatus SPP_DeinitDevice(SppDev *dev);

/****************************************************************************
 *
 * Section: Serial Port Profile I/O Functions
 *
 * These APIs are provided to upper layers. Normally, they are called
 * as the result of a corresponding call to the system API. For example,
 * in a POSIX interface open() would call SPP_Open().
 *
 ***************************************************************************/
 
/*---------------------------------------------------------------------------
 * SPP_Enable()
 *
 *      Opens a serial device for reading and writing.
 *
 * Parameters:
 *      dev -  pointer to existing device structure.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - device opened successfully.
 *		BT_STATUS_PENDING - opening of the port has been successfully started.
 *      BT_STATUS_INVALID_PARM - invalid parameter.
 *      BT_STATUS_FAILED - device is not in a closed state
 */
BtStatus SPP_Enable(SppDev *dev);
 
/*---------------------------------------------------------------------------
 * SPP_Disable() 
 *     
 *     Close the serial device. Requires the device to have been enabled 
 *     previously by SPP_Enable().
 *
 * Parameters:
 *      dev - pointer to existing device structure
 *
 * Returns:
 *      BT_STATUS_SUCCESS - closed successfully
 *      BT_STATUS_FAILED - device was not opened
 */
BtStatus SPP_Disable(SppDev *dev);
 
/*---------------------------------------------------------------------------
 * SPP_Connect()
 *
 *      Connects client port to the peer device.
 *
 * Parameters:
 *      dev - pointer to existing device structure.
 *      bdAddr - a pointer to BD ADDR of the remote device to which a service
 *          connection will be made.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - device connected successfully.
 *		BT_STATUS_PENDING - connecting of the port has been successfully started.
 *      BT_STATUS_INVALID_PARM - invalid parameter.
 *      BT_STATUS_FAILED - the operation failed to start.
 */
BtStatus SPP_Connect(SppDev *dev, BD_ADDR *bdAddr);

/*---------------------------------------------------------------------------
 * SPP_Disconnect()
 *
 *      Disconnects a port from the peer device.
 *
 * Parameters:
 *      dev - pointer to existing device structure.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - device disconnected successfully.
 *		BT_STATUS_PENDING - disconnecting of the port has been successfully started.
 *      BT_STATUS_INVALID_PARM - invalid parameter.
 *      BT_STATUS_FAILED - device is not in a connected state
 */
BtStatus SPP_Disconnect(SppDev *dev);

/*---------------------------------------------------------------------------
 * SPP_ReadAsync() 
 *      Reads from the serial device in case SPP_PORT_DATA_PATH_RX_ASYNC data
 *      path type was chosen.
 *     
 * Parameters:
 *      dev - pointer to existing device structure
 *      buffer - allocated buffer to receive bytes
 *      maxBytes - on input: maximum bytes to read; on successful completion:
 *                 number of bytes actually read
 *
 * Returns:
 *      BT_STATUS_SUCCESS - read was successful
 *      BT_STATUS_FAILED - device is not opened.
 */
BtStatus SPP_ReadAsync(SppDev *dev, U8 *buffer, U16 *maxBytes);

/*---------------------------------------------------------------------------
 * SPP_Write()
 *      Writes to the serial device.
 *      
 *      In case of Tx Sync data path, the port should be previously enabled.
 *      In case of Tx Async data path, the port should be connected to the
 *          peer device.
 *     
 * Parameters:
 *      dev - pointer to existing device structure.
 *      buffer - buffer containing characters to write.
 *      nBytes - on input: pointer to number of bytes in buffer; actual
 *               bytes written are returned in nBytes if it returns success
 *
 * Returns:
 *      BT_STATUS_SUCCESS - write was successful
 *      BT_STATUS_FAILED - device is not open
 */
BtStatus SPP_Write(SppDev *dev, const U8 *buffer, U16 *nBytes);

/*---------------------------------------------------------------------------
 * SPP_Flush()
 *      Flushes SPP buffers.
 *      
 *      In case of Tx Sync data path, the port should be previously enabled.
 *      In case of Tx Async data path, the port should be connected to the
 *          peer device.
 *     
 * Parameters:
 *      dev - pointer to existing device structure.
 *      flushType - type of buffers to be flushed: Rx, Tx or both.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - write was successful
 *      BT_STATUS_FAILED - device is not open
 */
BtStatus SPP_Flush(SppDev *dev, SppFlushType flushType);

/*---------------------------------------------------------------------------
 * SPP_SetModemControl()
 *      Set the modem control signals for the specified device. 
 *     
 * Parameters:
 *      dev -    pointer to existing device structure
 *      controlSignals - modem control signals and breakLen to be set
 *
 * Returns:
 *      BT_STATUS_RESTRICTED - indicates that RF_SEND_CONTROL configuration
 *          constant is not enabled and control signals were not sent to peer
 *          serial port.
 *
 *      BT_STATUS_PENDING - the operation has been successfully started.
 *
 *      BT_STATUS_IN_PROGRESS - previous request has been sent and was not
 *          acknowledged yet.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus SPP_SetModemControl(SppDev *dev, const SppControlSignals *controlSignals);

/*---------------------------------------------------------------------------
 * SPP_GetModemControl()
 *      Set the modem control signals for the specified device. 
 *     
 * Parameters:
 *      dev -    pointer to existing device structure
 *      controlSignals [out] - returned control signals of the remote modem
 *
 * Returns:
 *      BT_STATUS_SUCCESS.
 */
BtStatus SPP_GetModemControl(SppDev *dev, SppControlSignals *controlSignals);

/*---------------------------------------------------------------------------
 * SPP_SetComSettings() 
 *      This routine implements setting of device communication settings in
 *		one step.
 *     
 * Parameters:
 *      dev  - pointer to SPP device
 *      comSettings - pointer to structure with settings to be set
 *
 * Returns:
 *      BT_STATUS_RESTRICTED - indicates that RF_SEND_CONTROL configuration
 *          constant is not enabled and communication settings were not sent to
 *          peer serial port.
 *
 *      BT_STATUS_PENDING - the operation has been successfully started.
 *
 *      BT_STATUS_IN_PROGRESS - previous request has been sent and was not
 *          acknowledged yet.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus SPP_SetComSettings(SppDev *dev, const SppComSettings *comSettings);

/*---------------------------------------------------------------------------
 * SPP_GetComSettings() 
 *      This routine implements getting of device communication settings in
 *		one step.
 *     
 * Parameters:
 *      dev  - pointer to SPP device
 *      comSettings [out] - pointer to structure to copy settings into
 *
 * Returns:
 *      BT_STATUS_SUCCESS - indicates that settings were set.
 *
 *      BT_STATUS_FAILED  - failure of the operation.
 */
BtStatus SPP_GetComSettings(SppDev *dev, SppComSettings *comSettings);

/*---------------------------------------------------------------------------
 * SPP_SetLineStatus() 
 *      This routine implements sending line status to the peer serial port.
 *     
 * Parameters:
 *      dev  - pointer to SPP device
 *      lineStatus - line status to be set
 *
 * Returns:
 *      BT_STATUS_RESTRICTED - indicates that RF_SEND_CONTROL configuration
 *          constant is not enabled and line status was not sent to peer serial
 *          port.
 *
 *      BT_STATUS_PENDING - the operation has been successfully started.
 *
 *      BT_STATUS_IN_PROGRESS - previous request has been sent and was not
 *          acknowledged yet.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus SPP_SetLineStatus(SppDev *dev, SppLineStatus lineStatus);



#endif /* TI_CHANGES == XA_ENABLED */
#endif /* __SPP_H */

