/****************************************************************************
 *
 * File:
 *     $Workfile:hfg.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:32$
 *
 * Description: This file contains the hands free profile.
 *             
 * Created:     September 20, 2001
 *
 * Copyright 2001-2004 Extended Systems, Inc.
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

#ifndef __HFG_H_
#define __HFG_H_

#include "rfcomm.h"
#include "atp.h"
#include "conmgr.h"

/*---------------------------------------------------------------------------
 * Hands-Free Audio Gateway SDK layer
 *
 * This API allows an application to act as the "Hands-Free Audio Gateway"
 * (AG) role of the Hands-Free Profile. Devices in this role are responsible
 * for providing an interface to a telephone network (such as GSM) under
 * the direct control of a "Hands-Free Unit". For example, the AG may be
 * a mobile phone interacting with a hands-free unit installed in a car.
 *
 * Within this API, there are two "levels" of links: service and
 * audio, each with their own function calls and events. A service-level
 * connection allows the hands-free unit to send commands to the AG, such as 
 * HFG_SendRing().  An audio-level link allows full-duplex sound to be exchanged 
 * between the hands-free unit and the AG. Using this API, the service link must
 * be established BEFORE attempting to open an audio link.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HFG_FEATURE_THREE_WAY_CALLS constant
 *     Three way calls.  This value can be used when defining the capabilities 
 *     of the audio gateway application.
 *     See HFG_SDK_FEATURES.  
 */
#define HFG_FEATURE_THREE_WAY_CALLS      0x00000001

/*---------------------------------------------------------------------------
 * HFG_FEATURE_ECHO_NOISE constant
 *     Echo canceling and/or noise reduction function.  This value can be used 
 *     when defining the capabilities of the audio gateway application.
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_ECHO_NOISE           0x00000002

/*---------------------------------------------------------------------------
 * HFG_FEATURE_VOICE_RECOGNITION constant
 *     Voice recognition function.  This value can be used when defining the 
 *     capabilities of the audio gateway application.
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_VOICE_RECOGNITION    0x00000004

/*---------------------------------------------------------------------------
 * HFG_FEATURE_RING_TONE constant
 *     In-band ring tone.  This value can be used when defining the capabilities 
 *     of the audio gateway application.
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_RING_TONE            0x00000008

/*---------------------------------------------------------------------------
 * HFG_FEATURE_VOICE_TAG constant
 * Attach a phone number to a voice tag.  This value can be used when defining 
 * the capabilities of the audio gateway application.
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_VOICE_TAG            0x00000010

/*---------------------------------------------------------------------------
 * HFG_FEATURE_REJECT constant
 *     Reject a call.  This value can be used when defining the capabilities 
 *     of the audio gateway application. 
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_REJECT               0x00000020


/*---------------------------------------------------------------------------
 * HFG_FEATURE_ENHANCED_CALL_STATUS constant
 *     This device is capable of supporting enhanced call status features
 *     like call listing and call held indications.
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_ENHANCED_CALL_STATUS 0x00000040

/*---------------------------------------------------------------------------
 * HFG_FEATURE_ENHANCED_CALL_CTRL constant
 *     This device is capable of supporting enhanced call control features
 *     like call specifying specific lines to put on hold.
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_ENHANCED_CALL_CTRL   0x00000080

/*---------------------------------------------------------------------------
 * HFG_FEATURE_EXTENDED_ERRORS constant
 *     This device is capable of sending extended error codes.
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_EXTENDED_ERRORS      0x00000100

#if TI_CHANGES == XA_ENABLED

#define HFG_FEATURE_EXTENDED_NONE	   0x0

#endif /* TI_CHANGES == XA_ENABLED */


#if (TI_CHANGES == XA_ENABLED)
 
/*---------------------------------------------------------------------------
 * HFG_DEFAULT_FEATURES constant.
 *	The default features of an audio gateway as defined by the HFP specification.
 */
#define HFG_DEFAULT_FEATURES (HFG_FEATURE_THREE_WAY_CALLS | HFG_FEATURE_RING_TONE)

#endif /* TI_CHANGES == XA_ENABLED */

/*---------------------------------------------------------------------------
 * HFG_SDK_FEATURES constant
 *     Defines the supported features of the hands-free application.  Features
 *     that will be supported by the application must be advertised to the
 *     hands-free device, so that it knows the capabilities of the audio gateway.
 *     If this value needs to be changed, it can be placed in overide.h with
 *     the features supported by the audio gateway application.
 */
#if (TI_CHANGES == XA_ENABLED)

#ifdef HFG_SDK_FEATURES

#if (HFG_SDK_FEATURES != HFG_FEATURE_EXTENDED_NONE)
#error "HFG_SDK_FEATURES must not be defined or else equal HFG_FEATURE_EXTENDED_NONE"
#endif

#endif /* ifdef HFG_SDK_FEATURES */

#else /* TI_CHANGES == XA_ENABLED */

#ifndef HFG_SDK_FEATURES
#define HFG_SDK_FEATURES  (HFG_FEATURE_THREE_WAY_CALLS      | \
                           HFG_FEATURE_ECHO_NOISE           | \
                           HFG_FEATURE_VOICE_RECOGNITION    | \
                           HFG_FEATURE_RING_TONE            | \
                           HFG_FEATURE_VOICE_TAG            | \
                           HFG_FEATURE_REJECT               | \
                           HFG_FEATURE_ENHANCED_CALL_STATUS | \
                           HFG_FEATURE_ENHANCED_CALL_CTRL   | \
                           HFG_FEATURE_EXTENDED_ERRORS)
#endif /* HFG_SDK_FEATURES */

#endif /* TI_CHANGES == XA_ENABLED */


/*---------------------------------------------------------------------------
 * HFG_TX_BUFFER_SIZE constant
 *     Defines the maximum size of AT response data that can be transmitted.
 *     The default is large enough to handle all AT responses supported by
 *     the Hands-free SDK  If raw AT commands are sent that are larger than the
 *     default value, then this number must be increased.
 */
#ifndef HFG_TX_BUFFER_SIZE
#define HFG_TX_BUFFER_SIZE       1024
#endif

/*---------------------------------------------------------------------------
 * HFG_RECV_BUFFER_SIZE constant
 *     Defines the maximum size of AT command data that can be received.
 *     The default is large enough to handle all AT commands supported by
 *     the handsfree profile.
 */
#ifndef HFG_RECV_BUFFER_SIZE
#define HFG_RECV_BUFFER_SIZE     1024
#endif

/*---------------------------------------------------------------------------
 * HFG_USE_RESP_HOLD constant
 *     Enables the use of the Response and Hold feature, which is supported
 *     in the Japanese market.
 */
#ifndef HFG_USE_RESP_HOLD
#define HFG_USE_RESP_HOLD XA_DISABLED
#endif


/*---------------------------------------------------------------------------
 * HFG_SNIFF_TIMER constant
 *     Enable sniff mode after a defined timeout.  Sniff mode is entered
 *     when a service link is active, but no audio connection is up.  This
 *     is a power saving feature.  If this value is defined to -1, then
 *     sniff mode is disabled.
 *
 *	   This value is only the default value for the sniff timer, if HFG_SNIFF_TIMER >= 0
 *	   Then a sniff timer value can be set for each HFG channel.
 */
#ifndef HFG_SNIFF_TIMER
#define HFG_SNIFF_TIMER 2000
#endif

/*---------------------------------------------------------------------------
 * HFG_SNIFF_MIN_INTERVAL constant
 *     Miniumum interval for sniff mode if enabled (see HFG_SNIFF_TIMER).
 *
 */
#ifndef HFG_SNIFF_MIN_INTERVAL
#define HFG_SNIFF_MIN_INTERVAL 800
#endif

/*---------------------------------------------------------------------------
 * HFG_SNIFF_MAX_INTERVAL constant
 *     Maximum interval for sniff mode if enabled (see HFG_SNIFF_TIMER).
 *
 */
#ifndef HFG_SNIFF_MAX_INTERVAL
#define HFG_SNIFF_MAX_INTERVAL 8000
#endif

/*---------------------------------------------------------------------------
 * HFG_SNIFF_ATTEMPT constant
 *     Sniff attempts for sniff mode if enabled (see HFG_SNIFF_TIMER).
 *
 */
#ifndef HFG_SNIFF_ATTEMPT
#define HFG_SNIFF_ATTEMPT 1600
#endif

/*---------------------------------------------------------------------------
 * HFG_SNIFF_TIMEOUT constant
 *     Sniff timeout for sniff mode if enabled (see HFG_SNIFF_TIMER).
 *
 */
#ifndef HFG_SNIFF_TIMEOUT
#define HFG_SNIFF_TIMEOUT 1600
#endif

/*---------------------------------------------------------------------------
 * HFG_MEMORY_EXTERNAL constant
 *     Memory is allocated externally.  When this constant is defined as
 *     XA_ENABLED, global context memory is not allocated by the Hands-free 
 *     profile code, and therfore must be allocated external to this module.  
 *     If memory is allocated external to this module, if must be allocated 
 *     using the context variable type defined in hfgalloc.h, and the entire 
 *     structure must be initialized to 0. In addition, the macro HFG() must 
 *     be defined so that the context memory can be accessed local to this module.
 *
 *     See hfgalloc.h and hfgalloc.c for an example.
 */
#ifndef HFG_MEMORY_EXTERNAL
#define HFG_MEMORY_EXTERNAL XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * HFG_SECURITY_SETTINGS constant
 *     Defines the security settings for a hands-free connection.  The default
 *     is set to BSL_DEFAULT (see BtSecurityLevel).
 */
#ifndef HFG_SECURITY_SETTINGS
#define HFG_SECURITY_SETTINGS BSL_DEFAULT
#endif                               

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HfgEvent type
 *
 *     All indications and confirmations are sent through a callback
 *     function. Depending on the event, different elements of
 *     HfgCallbackInfo "parms" union will be valid as described below.
 */
typedef U8 HfgEvent;

/** An incoming service level connection is being established.  This happens
 *  when the hands-free device establishes the service connection.
 *  The data connection is not available yet for sending to the hands-free 
 *  device.  When the HFG_EVENT_SERVICE_CONNECTED event is received, the 
 *  channel is available for sending.
 *
 *  When this callback is received, the "HfgCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.
 */
#define HFG_EVENT_SERVICE_CONNECT_REQ        0

/** A service level connection has been established.  This can happen as the
 *  result of a call to HFG_CreateServiceLink, or if the hands-free device 
 *  establishes the service connection.  When this event has been received, a 
 *  data connection is available for sending to the hands-free device.
 *
 *  This event can also occur when an attempt to create a service level 
 *  connection (HFG_CreateServiceLink) fails.
 *
 *  When this callback is received, the "HfgCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.  In addition, the
 *  "HfgCallbackParms.errCode" fields contains the reason for disconnect.
 */
#define HFG_EVENT_SERVICE_CONNECTED          1

/** The service level connection has been released.  This can happen as the
 *  result of a call to HFG_DisconnectServiceLink, or if the hands-free device
 *  releases the service connection.  Communication with the hands-free device 
 *  is no longer possible.  In order to communicate with the hands-free device,
 *  a new service level connection must be established.
 *
 *  This event can also occur when an attempt to create a service level 
 *  connection (HFG_CreateServiceLink) fails.
 *
 *  When this callback is received, the "HfgCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.  In addition, the
 *  "HfgCallbackParms.errCode" fields contains the reason for disconnect.
 */
#define HFG_EVENT_SERVICE_DISCONNECTED       2

/** An audio connection has been established.  This event occurs whenever the
 *  audio channel (SCO) comes up, whether it is initiated by the audio gateway
 *  or the hands-free unit.
 *
 *  When this callback is received, the "HfgCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.
 */
#define HFG_EVENT_AUDIO_CONNECTED            3

/** An audio connection has been released.  This event occurs whenever the
 *  audio channel (SCO) goes down, whether it is terminated by the audio gateway
 *  or the hands-free unit.
 *
 *  When this callback is received, the "HfgCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.  In addition, the
 *  "HfgCallbackParms.errCode" fields contains the reason for disconnect.
 */
#define HFG_EVENT_AUDIO_DISCONNECTED         4
 
/** After the service level connection has been established, this event will
 *  indicate the features supported on the hands-free unit.  
 *
 *  When this callback is received, the "HfgCallbackParms.p.features" field 
 *  contains the features (see HfgFeatures).
 */
#define HFG_EVENT_HANDSFREE_FEATURES         5

#if BT_SCO_HCI_DATA == XA_ENABLED
/** Only valid if BT_SCO_HCI_DATA is set to XA_ENABLED.  Audio data has been 
 *  received from the remote device.  The data is only valid during the
 *  callback.
 *
 *  When this callback is received, the "HfCallbackParms.p.audioData" field 
 *  contains the audio data.
 */
#define HFG_EVENT_AUDIO_DATA                 6

/** Only valid if BT_SCO_HCI_DATA is set to XA_ENABLED.  Audio data has been 
 *  sent to the remote device.  This event is received by the application when
 *  the data sent by HFG_SendAudioData has been successfully sent.
 *
 *  When this callback is received, the "HfCallbackParms.p.audioPacket" field 
 *  contains the result.
 */
#define HFG_EVENT_AUDIO_DATA_SENT            7
#endif

/** The hands-free unit has requested the audio gateway to answer the incoming 
 *  call.  When the call has been answered, the application should call 
 *  HFG_SendOK().  In addtion, the appropriate indicators should be updated.
 */
#define HFG_EVENT_ANSWER_CALL                9

/** The hands-free unit has requested the audio gateway to place a call. The
 *  application should dial the number and respond with a call to HFG_SendOK().
 *  In addtion, the appropriate indicators should be updated 
 *  (see HFG_SetIndicatorValue()).
 *
 *  When this callback is received, the "HfgCallbackParms.p.phoneNumber" 
 *  parameter is valid for this event.
 */
#define HFG_EVENT_DIAL_NUMBER               10

/** The hands-free unit has requested the audio gateway to place a call to 
 *  the phone number associated with the given memory location.  The
 *  application should dial the number and respond with a call to HFG_SendOK().
 *  In addtion, the appropriate indicators should be updated 
 *  (see HFG_SetIndicatorValue()).
 *
 *  When this callback is received, the "HfgCallbackParms.p.memory" parameter 
 *  is valid for this event.
 */
#define HFG_EVENT_MEMORY_DIAL               11

/** The hands-free unit has requested the audio gateway to place a call to 
 *  the last number dialed.  The application should dial the last number dialed
 *  and respond with a call to HFG_SendOK().  In addtion, the appropriate 
 *  indicators should be updated (see HFG_SetIndicatorValue()).
 */
#define HFG_EVENT_REDIAL                    12

/** The hands-free unit has requested the audio gateway to place the current 
 *  call on hold.  The application should make the appropriate hold action
 *  and respond with a cll to HFG_SendOK().  In addtion, the appropriate 
 *  indicators should be updated (see HFG_SetIndicatorValue()).
 *
 *  When this callback is received, the "HfgCallbackParms.p.hold" parameter 
 *  is valid for this event.
 */
#define HFG_EVENT_CALL_HOLD                 13

#if HFG_USE_RESP_HOLD == XA_ENABLED
/** This event is only available when HFG_USE_RESP_HOLD is set to XA_ENABLED.
 *  The Response and Hold state has been requested from the handsfree device.
 *  The audio gateway should respond by calling HFG_SendResponseHoldState().
 *  If the the audio gateway is in any Response and Hold state, then the 
 *  application should respond by calling HFG_SendResponseHoldState() with
 *  the state set to HFG_RESP_HOLD_STATE_HOLD, followed by a call to
 *  HFG_SendOK(), othwerwise, the application should simply call HFG_SendOK()
 *  
 */
#define HFG_QUERY_RESPONSE_HOLD              14

/** This event is only available when HFG_USE_RESP_HOLD is set to XA_ENABLED.
 *  A Response and Hold request has been received from the handsfree device.
 *  The audio gateway should take the appropriate action and respond by calling
 *  HFG_SendResponseHoldState() with the state set to the requested state.  
 *  In addtion, the appropriate indicators should be updated 
 *  (see HFG_SetIndicatorValue()).
 * 
 *  When this callback is received, the "HfCallbackParms.p.respHold" field 
 *  contains the result.
 */
#define HFG_RESPONSE_HOLD                    15
#endif

/** The hands-free unit has requested the audio gateway to hang up the current 
 *  call.  The application should hang up the call and respond with a call
 *  to HFG_SendOK().  In addtion, the appropriate indicators should be updated 
 *  (see HFG_SetIndicatorValue()).
 */
#define HFG_EVENT_HANGUP                    16

/** The list of current calls has been requested from the hands-free device.
 *  The audio gateway should respond by calling HFG_SendCallListRsp() once
 *  for each line supported by the device.
 */                                     
#define HFG_EVENT_LIST_CURRENT_CALLS        17

/** The hands-free unit has requested that Calling Line Identification
 * notification be enabled or disabled.  
 *
 * The "HfgCallbackParms.p.enabled" parameter indicates the type of request. 
 * If "enabled" is FALSE, the application may call HFG_SendCallerId(), but no
 * indication will be sent to the remote device.
 */
#define HFG_EVENT_ENABLE_CALLER_ID          18

/** The hands-free unit has requested that Call Waiting notification be enabled
 *  or disabled. 
 *
 *  The "HfgCallbackParms.p.enabled" parameter indicates the type of request.  
 *  If "enabled" is FALSE, the application may call HFG_CallWaiting(), but no
 *  indication will be sent to the remote device.
 */
#define HFG_EVENT_ENABLE_CALL_WAITING       19

/** The hands-free unit has requested the audio gateway to transmit a 
 *  specific DTMF code to its network connection. The "dtmf" parameter 
 *  will contain the requested DTMF code.
 *
 *  When the tone has been sent to the network, call HFG_SendOk().
 */
#define HFG_EVENT_GENERATE_DTMF             20

#if TI_CHANGES == XA_DISABLED

#if HFG_SDK_FEATURES & HFG_FEATURE_VOICE_TAG
/** The hands-free unit has requested the phone number associated with the
 *  last voice tag in the audio gateway in order to link its own voice tag to 
 *  the number.  The hands-free unit may then dial the linked phone numbers 
 *  when a voice tag is recognized.  (This procedure is only applicable for 
 *  hands-free units that support their own voice recognition functionality).
 *  The HFG_FEATURE_VOICE_TAG bit must be set in HFG_SDK_FEATURES in order
 *  to use this event.
 *
 *  In response to this event, call HFG_VoiceTagResponse() with a number for
 *  tagging, or call HFG_SendError() to reject the request.
 */
#define HFG_EVENT_GET_LAST_VOICE_TAG        21
#endif

#else

#define HFG_EVENT_GET_LAST_VOICE_TAG        21
#endif /* TI_CHANGES == XA_ENABLED */

#if TI_CHANGES == XA_DISABLED

#if HFG_SDK_FEATURES & HFG_FEATURE_VOICE_RECOGNITION
/** Enables/disables the voice recognition function resident in the audio
 *  gateway (as indicated by the "enabled" parameter). If the HF enables voice
 *  recognition, the audio gateway must keep the voice recognition enabled 
 *  until either:
 *
 *      1) The HF disables it.
 *      2) The service link is disconnected.
 *      3) The duration of time supported by the audio gateway's 
 *         implementation has elapsed.
 *
 * In this last case the audio gateway must notify the hands-free unit that 
 * it has disabled voice recognition by calling HFG_DisableVoiceRec().
 * The HFG_FEATURE_VOICE_RECOGNITION bit must be set in HFG_SDK_FEATURES in order
 * to receive this event.
 */
#define HFG_EVENT_ENABLE_VOICE_RECOGNITION  22
#endif

#else

#define HFG_EVENT_ENABLE_VOICE_RECOGNITION  22
#endif /* TI_CHANGES == XA_ENABLED */

#if TI_CHANGES == XA_DISABLED

#if HFG_SDK_FEATURES & HFG_FEATURE_ECHO_NOISE
/** The hands-free unit has requested the audio gateway to disable the noise
 *  reduction and echo canceling (NREC) functions resident in the audio 
 *  gateway.
 *
 *  If the audio gateway supports NREC it must disable these features for 
 *  the duration of the service link.  The HFG_FEATURE_ECHO_NOISE bit must be 
 *  set in HFG_SDK_FEATURES in order to receive this event.
 */
#define HFG_EVENT_DISABLE_NREC              23
#endif

#else

#define HFG_EVENT_DISABLE_NREC              23
#endif /* TI_CHANGES == XA_DISABLED */

/** The hands-free has informed the audio gateway of its microphone volume 
 *  level. 
 *
 *  The "HfgCallbackParms.p.gain" parameter is valid.
 */
#define HFG_EVENT_REPORT_MIC_VOLUME         24

/** The hands-free has informed the audio gateway of its speaker volume 
 *  level. 
 *
 *  The "HfgCallbackParms.p.gain" parameter is valid.
 */
#define HFG_EVENT_REPORT_SPK_VOLUME         25

/** The hands-free device has requested the network operator from the
 *  audio gateway.  The audio gateway should respond by calling
 *  HFG_SendNetworkOperatorRsp().
 */
#define HFG_EVENT_QUERY_NETWORK_OPERATOR    26
 
/** The hands-free device has requested the subscriber number from the
 *  audio gateway.  The audio gateway should respond by calling
 *  HFG_SendSubscriberNumberRsp().
 */
#define HFG_EVENT_QUERY_SUBSCRIBER_NUMBER   27

/** The hands-free device has requested that extended error codes be enabled.
 *  When extended errors are enabled, a call to HFG_SendError() will send
 *  extended errors, otherwise it will only repspond with an "ERROR" response.
 */
#define HFG_EVENT_ENABLE_EXTENDED_ERRORS    28

/** An unsupported AT command has been received from the audio gateway.  This 
 *  event is received for AT commands that are not handled by the internal 
 *  Hands-free AT parser.  The application must make an appropriate response
 *  and call HFG_SendOK() to complete the response.
 *
 *  When this callback is received, the "HfgCallbackParms.p.data" field 
 *  contains the AT command data.
 */
#define HFG_EVENT_AT_COMMAND_DATA           29

/** Whenever a response has been set to the remote device, this event is
 *  received to confirm that the repsonse was sent.
 * 
 *  When this event is received, the "HfgCallbackParms.p.response" field
 *  contains a pointer to the response structure that was used to send
 *  the reponse.
 */
#define HFG_EVENT_RESPONSE_COMPLETE         30

#if TI_CHANGES == XA_ENABLED

/** Recieved when a remote HF unit requests an audio connection.
 *	the "HfgCallbackParms.p.remDev"is valid during the callback.
 *	Call HFG_AcceptAudioLink() to accept or reject the request.
 */
#define HFG_EVENT_AUDIO_CONNECT_REQ			31

#if AT_PHONEBOOK == XA_ENABLED

/**	The base value used by phonebook access using AT commands related events
 */
#define HFG_EVENT_PHONEBOOK_BASE_VALUE					32

/**	The hands-free unit has requested a list of locally supported
 *	phonebooks.
 *	Use BTL_HFAG_SendSupportedPhonebookListRsp() to send the response
 */
#define HFG_EVENT_SUPPORTED_PHONEBOOK_LIST				(HFG_EVENT_PHONEBOOK_BASE_VALUE + 0)

/** The hands-free unit has requested a information on the currently
 *	selected phonebook.
 *	Use BTL_HFAG_SendSelectedPhonebookInfoRsp() to send the response
 */
#define HFG_EVENT_SELECTED_PHONEBOOK_INFO				(HFG_EVENT_PHONEBOOK_BASE_VALUE + 1)

/** The hands-free unit has requested information on the selected phone
 *	book entries.
 *	Use BTL_HFAG_SendReadPhonebookEntriesRangeRsp() to send the respone.
 */	
#define HFG_EVENT_READ_PHONEBOOK_ENTRIES_RANGE			(HFG_EVENT_PHONEBOOK_BASE_VALUE + 2)

/**	The hands-free unit has requested to read phonebook entries
 *	The "HfgCallbackParms.p.entriesRange" field contains two fields.
 *	index1 and index2, meaning the hands-free unit has requested
 *	all the entries with index1 to index2, inclusive.
 *	Use BTL_HFAG_SendReadPhonebookEntriesRsp() to send the response.
 */
#define HFG_EVENT_READ_PHONEBOOK_ENTRIES				(HFG_EVENT_PHONEBOOK_BASE_VALUE + 3)

/**	The hands-free unit has requested to find phonebook entries.
 *	The "HfgCallbackParms.p.findText" contains a string
 *	This string is used to find entries.
 *	Use BTL_HFAG_SendFindPhonebookEntriesRsp() to send the response.
 */
#define HFG_EVENT_FIND_PHONEBOOK_ENTRIES				(HFG_EVENT_PHONEBOOK_BASE_VALUE + 4)

/** The hands-free unit has requested information regarding entries size.
 *	Use BTL_HFAG_SendFindPhonebookEntriesSize() to send the response.
 */
#define HFG_EVENT_FIND_PHONEBOOK_ENTRIES_SIZE			(HFG_EVENT_PHONEBOOK_BASE_VALUE + 5)

/**	The hands-free unit has requested informaion about phonebook entries.
 *	Use BTL_HFAG_SendWritePhonebookEntryLocationRange()
 *	to send the response.
 */
#define HFG_EVENT_WRITE_PHONEBOOK_ENTRY_LOCATION_RANGE	(HFG_EVENT_PHONEBOOK_BASE_VALUE + 6)

/**	The hands-free unit has requested information on the locally selected charset.
 *	Use BTL_HFAG_SendSelectedCharSetRsp() to send the response.
 */
#define HFG_EVENT_SELECTED_CHAR_SET						(HFG_EVENT_PHONEBOOK_BASE_VALUE + 7)

/**	The hands-free unit has requested a list of the locally supported char sets.
 *	Use BTL_HFAG_SendSupportedCharSetRsp() to send the response.
 */
#define HFG_EVENT_SUPPORTED_CHAR_SET					(HFG_EVENT_PHONEBOOK_BASE_VALUE + 8)

/**	The hands-free unit has set a char set
 *	The "HfgCallbackParms.p.charSet" field holds the char set to set.
 */
#define HFG_EVENT_SET_CHAR_SET							(HFG_EVENT_PHONEBOOK_BASE_VALUE + 9)

/** The hands-free unit has set a phonebook
 *	The "HfgCallbackParms.p.setPhonebook" fields holds the fields:
 *	phonebook - the phonebook to set.
 *	password - if a password is needed to access the phonebook.
 */
#define HFG_EVENT_SET_PHONEBOOK							(HFG_EVENT_PHONEBOOK_BASE_VALUE + 10)

#endif  /* TI_CHANGES == XA_ENABLED */

#endif /* AT_COMMAND_PHONEBOOK_ACCESS == XA_ENABLED */

/* End of HfgEvent */

/*---------------------------------------------------------------------------
 * HfgSniffExitPolicy type
 *
 * Defines the policy for exiting sniff mode.  While it is possible to both
 * send data a create SCO connections in sniff mode, it may be desired to
 * exit sniff mode for performance or compatibility reasons.
 */
typedef U8 HfgSniffExitPolicy;

/** Exit sniff mode whenever there is data to send.
 */
#define HFG_SNIFF_EXIT_ON_SEND  CMGR_SNIFF_EXIT_ON_SEND

/** Exit sniff mode whenever an audio link is being created.
 */
#define HFG_SNIFF_EXIT_ON_AUDIO CMGR_SNIFF_EXIT_ON_AUDIO

/* End of HfgSniffExitPolicy */

/*--------------------------------------------------------------------------
 * HfgCmeError type
 *
 */
typedef AtError HfgCmeError;

#define HFG_CME_NO_CONNECTION                 ATCME_NO_CONNECTION
#define HFG_CME_OP_NOT_ALLOWED                ATCME_OP_NOT_ALLOWED
#define HFG_CME_OP_NOT_SUPPORTED              ATCME_OP_NOT_SUPPORTED
#define HFG_CME_PH_SIM_PIN_REQUIRED           ATCME_PH_SIM_PIN_REQUIRED
#define HFG_CME_SIM_NOT_INSERTED              ATCME_SIM_NOT_INSERTED
#define HFG_CME_SIM_PIN_REQUIRED              ATCME_SIM_PIN_REQUIRED
#define HFG_CME_SIM_PUK_REQUIRED              ATCME_SIM_PUK_REQUIRED
#define HFG_CME_SIM_FAILURE                   ATCME_SIM_FAILURE
#define HFG_CME_SIM_BUSY                      ATCME_SIM_BUSY
#define HFG_CME_INCORRECT_PASSWORD            ATCME_INCORRECT_PASSWORD
#define HFG_CME_SIM_PIN2_REQUIRED             ATCME_SIM_PIN2_REQUIRED
#define HFG_CME_SIM_PUK2_REQUIRED             ATCME_SIM_PUK2_REQUIRED
#define HFG_CME_MEMORY_FULL                   ATCME_MEMORY_FULL
#define HFG_CME_INVALID_INDEX                 ATCME_INVALID_INDEX
#define HFG_CME_MEMORY_FAILURE                ATCME_MEMORY_FAILURE
#define HFG_CME_TEXT_STRING_TOO_LONG          ATCME_TEXT_STRING_TOO_LONG
#define HFG_CME_INVALID_CHARS_IN_TEXT_STRING  ATCME_INVALID_CHARS_IN_TEXT_STRING
#define HFG_CME_DIAL_STRING_TOO_LONG          ATCME_DIAL_STRING_TOO_LONG
#define HFG_CME_INVALID_CHARS_IN_DIAL_STRING  ATCME_INVALID_CHARS_IN_DIAL_STRING
#define HFG_CME_NO_NETWORK_SERVICE            ATCME_NO_NETWORK_SERVICE
#define HFG_CME_NETWORK_NOT_ALLOWED           ATCME_NETWORK_NOT_ALLOWED
#define HFG_CME_UNKNOWN                       ATCME_UNKNOWN

/* End of HfgCmeError */

/*--------------------------------------------------------------------------
 * HfgHandsFreeVersion type
 *      
 *     Handsfree profile version discovered during the SDP query.  The service
 *     connection will be limited to the capabilities of this profile version.
 */
typedef U16  HfgHandsFreeVersion;

/* Unable to determine the Hands Free Profile version that is supported */
#define HFG_HF_VERSION_UNKNOWN 0x0000

/* Supports Version 0.96 of the Hands Free Profile */
#define HFG_HF_VERSION_0_96    0x0100

/* Supports Version 1.0 of the Hands Free Profile */
#define HFG_HF_VERSION_1_0     0x0101

/* Supports Version 1.5 of the Hands Free Profile */
#define HFG_HF_VERSION_1_5     0x0105

/* End of HfgHandsFreeVersion */

/*---------------------------------------------------------------------------
 * HfgHandsFreeFeatures type
 *      
 *  Bit mask specifying the HF's optional feature set.
 */
typedef U32  HfgHandsFreeFeatures;

/** Echo canceling and/or noise reduction function */
#define HFG_HANDSFREE_FEATURE_ECHO_NOISE            0x00000001

/** Call-waiting and 3-way calling */
#define HFG_HANDSFREE_FEATURE_CALL_WAITING          0x00000002

/** CLI presentation capability */
#define HFG_HANDSFREE_FEATURE_CLI_PRESENTATION      0x00000004

/** Voice recognition function */
#define HFG_HANDSFREE_FEATURE_VOICE_RECOGNITION     0x00000008

/** Remote volume control */
#define HFG_HANDSFREE_FEATURE_VOLUME_CONTROL        0x00000010

/* Enhanced call status */
#define HFG_HANDSFREE_FEATURE_ENHANCED_CALL_STATUS  0x00000020

/* Enhanced call control */
#define HFG_HANDSFREE_FEATURE_ENHANCED_CALL_CTRL    0x00000040

/* End of HfgHandsFreeFeatures */

/*---------------------------------------------------------------------------
 * HfgResponse type
 *      
 *  Structure used to send AT responses.
 */
typedef AtResults HfgResponse;

/* End of HfgResponse */

/*--------------------------------------------------------------------------
 * HfgAtMode type
 *      
 *     Possible modes for AT result code event reporting from the audio gateway.
 *     Only modes 0 and 3 are covered in the HF spec.
 */
typedef U8  HfgAtMode;

/** Buffer unsolicited result codes in the gateway; if gateway's result code 
 *  buffer is full, codes can be buffered in some other place or the oldest ones can
 *  be discarded.
 */
#define HFG_AT_BUFFER_RESULTS           0x00

/** Forward unsolicited result codes directly to the hands-free unit; 
 *  gateway-handsfree  link specific inband technique used to embed result codes
 *  and data when the gateway is in on-line data mode.
 */
#define HFG_AT_FORWARD_RESULTS          0x03

/* End of HfgAtMode */


/*--------------------------------------------------------------------------
 * HfgAtReport type
 *      
 *     Possible states for AT event reporting from the AG.
 *     Only modes 0 and 1 are covered in the HF spec.
 */
typedef U8 HfgAtReport;

/** No indicator event reporting.
 */
#define HFG_AT_NO_EVENT_REPORTING       0x00

/** Indicator event reporting using result code +CIEV.
 */
#define HFG_AT_EVENT_REPORTING          0x01

/* End of HfgAtReport */

/*--------------------------------------------------------------------------
 * HfgIndicator type
 *      
 *     Possible indicator events to be reported to the HF via
 *     HFG_SetIndicatorValue.
 */
typedef U8  HfgIndicator;

/** Service indicator.  The type associated with this event is a BOOL.  The 
 *  value associated with this event is TRUE if service exists or FALSE
 *  if no service exists.
 */
#define HFG_IND_SERVICE       1

/** Call indicator.  The type associated with this event is a BOOL.  The 
 *  value associated with this event is TRUE if a call exists or FALSE
 *  if no call exists.
 */
#define HFG_IND_CALL          2

/** Call setup.  The type associated with this event is a HfgCallSetup.  
 */
#define HFG_IND_CALL_SETUP    3

/** Call held.  The type associated with this event is a HfgHoldState.  The 
 *  value associated with this event is as follows:
 *   0 - no call is held
 *   1 - one call is held and one call is active
 *   2 - either one call is held and one is inactive, or two calls are held
 */
#define HFG_IND_CALL_HELD     4

/** Battery level.  The type associated with this indicator is a U8.  
 *  The value associated with this indicator is a number between 0 and 5.
 */
#define HFG_IND_BATTERY       5

/** Signal Strength.  The type associated with this indicator is a U8.  
 *  The value associated with this indicator is a number between 0 and 5.
 */
#define HFG_IND_SIGNAL        6

/** Roaming.  The type associated with this indicator is a BOOL.  
 *  The value associated with this indicator is TRUE when roaming and 
 *  FALSE when not roaming.
 */
#define HFG_IND_ROAMING       7

/* End of HfgIndicator */

/*--------------------------------------------------------------------------
 * HfgRejectFeature type
 *      
 *     Used as the reject feature in the SDP record and BRSF command.
 */
typedef U8  HfgRejectFeature;

/** No ablility to reject call 
 */
#define HFG_REJECT_NOT_SUPPORTED      0x00

/** Ability to reject supported
 */
#define HFG_REJECT_SUPPORTED          0x01

/* End of HfgRejectFeature */


/*--------------------------------------------------------------------------
 * HfgHoldAction type
 *      
 *     Hold actions 
 */
typedef U8  HfgHoldAction;

/** Releases all held calls or sets User Determined User Busy
 * (UDUB) for a waiting call.
 */
#define HFG_HOLD_RELEASE_HELD_CALLS      0x00

/** Releases all active calls (if any exist) and accepts the other
 * (held or waiting) call.
 */
#define HFG_HOLD_RELEASE_ACTIVE_CALLS    0x01

/** Places all active calls (if any exist) on hold and accepts the
 * other (held or waiting) call.
 */
#define HFG_HOLD_HOLD_ACTIVE_CALLS       0x02

/** Adds a held call to the conversation.
 */
#define HFG_HOLD_ADD_HELD_CALL           0x03

/** Connects the two calls and disconnects the AG from
 * both calls (Explicit Call Transfer).
 */
#define HFG_HOLD_EXPLICIT_TRANSFER       0x04
/* End of HfgHoldAction */

#if HFG_USE_RESP_HOLD == XA_ENABLED
/*---------------------------------------------------------------------------
 * HfgResponseHold type
 *
 * Defines the action or state of Response and Hold.  These are only valid in the
 * when HFG_USE_RESP_HOLD is set to XA_ENABLED.
 */
typedef U8 HfgResponseHold;

/* Incoming call is put on hold */
#define HFG_RESP_HOLD_STATE_HOLD      0

/* Held Incoming call is accepted */
#define HFG_RESP_HOLD_STATE_ACCEPT    1

/* Held Incoming call is rejected */
#define HFG_RESP_HOLD_STATE_REJECT    2

/* End of HfgResponseHoldAction */
#endif

#if TI_CHANGES == XA_ENABLED
/*---------------------------------------------------------------------------
 * HfgCccConnState type
 *     
 *     State of connection establishing used in possible cancellation of
 *     creating connection
 */
typedef U8 HfgCccConnState;

#define HFG_CCC_CONN_STATE_NONE            0x00
#define HFG_CCC_CONN_STATE_CONNECTING_ACL  0x01
#define HFG_CCC_CONN_STATE_SDP_QUERY       0x02
#define HFG_CCC_CONN_STATE_CONNECTING      0x03
#define HFG_CCC_CONN_STATE_CONNECTED       0x04

/* End of HfgCccConnState */
#endif /* TI_CHANGES == XA_ENABLED */

/* Forward References */
typedef struct _HfgCallbackParms HfgCallbackParms;
typedef struct _HfgChannel HfgChannel;

/*---------------------------------------------------------------------------
 * HfgCallback type
 *
 * A function of this type is called to indicate Hands-Free events to
 * the application.
 */
typedef void (*HfgCallback)(HfgChannel *Cgabbek, HfgCallbackParms *Parms);

/* End of HfgCallback */

#if BT_SCO_HCI_DATA == XA_ENABLED
/*---------------------------------------------------------------------------
 * HfgAudioData type
 *
 * This type is only available when BT_SCO_HCI_DATA is set to XA_ENABLED.
 * Contains audio data received from the remote device.
 */
typedef CmgrAudioData HfgAudioData;

/* End of HfgAudioData */
#endif

/*---------------------------------------------------------------------------
 * HfgCallStatus type
 *
 * Defines the current state of a call. 
 */
typedef U8 HfgCallStatus;

/** An active call exists.
 */
#define HFG_CALL_STATUS_ACTIVE     0

/** The call is held.
 */
#define HFG_CALL_STATUS_HELD       1

/** A call is outgoing.
 */
#define HFG_CALL_STATUS_DIALING    2

/** The remote parting is being alerted.
 */
#define HFG_CALL_STATUS_ALERTING   3

/** A call is incoming.
 */
#define HFG_CALL_STATUS_INCOMING   4

/** The call is waiting.  This state occurs only when the audio gateway supports
 *  3-Way calling.
 */
#define HFG_CALL_STATUS_WAITING    5

/** No active call
 */
#define HFG_CALL_STATUS_NONE       0xFF
/* End of HfgCallStatus */

/*---------------------------------------------------------------------------
 * HfgCallMode type
 *
 * Defines the current mode of a call.
 */
typedef U8 HfgCallMode;

/* Voice Call */
#define HFG_CALL_MODE_VOICE     0

/* Data Call */
#define HFG_CALL_MODE_DATA      1

/* FAX Call */
#define HFG_CALL_MODE_FAX       2

/* End of HfgCallMode */

/*---------------------------------------------------------------------------
 * HfgCallSetupState type
 *
 * Defines the current call setup state indicated by the audio gateway.  
 */
typedef U8 HfgCallSetupState;

/* No outgoing or incoming call is present on the audio gateway */
#define HFG_CALL_SETUP_NONE     0

/* An incoming call is present on the audio gateway */
#define HFG_CALL_SETUP_IN       1

/* An outgoing call is present on the audio gateway */
#define HFG_CALL_SETUP_OUT      2

/* An outgoing call is being alerted on the audio gateway */
#define HFG_CALL_SETUP_ALERT    3

/* End of HfgCallSetupState */

/*---------------------------------------------------------------------------
 * HfgHoldState type
 *
 * Defines the current call setup state indicated by the audio gateway.  
 */
typedef U8 HfgHoldState;

/* No held calls is present on the audio gateway */
#define HFG_HOLD_NO_HLD         0

/* There is one held and one active call present on the audio gateway */
#define HFG_HOLD_HLD_ACT        1

/* There is at least one held call and no 
 * active calls present on the audio gateway 
 */
#define HFG_HOLD_HLD_NO_ACT     2

/* End of HfgHoldState */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HfgAtData structure
 *
 *     Defines the structure containing raw AT data.
 */
typedef struct _HfgAtData {
    U8         *rawData;

#if TI_CHANGES == XA_ENABLED
	U16         dataLen;
#else
	U8			dataLen;
#endif /* TI_CHANGES == XA_ENABLED */

} HfgAtData;

/*---------------------------------------------------------------------------
 * HfgCallWaitParms structure
 *
 *     Used to identify the waiting call.
 */
typedef struct _HfgCallWaitParms {
    /* Phone number of the waiting call */
    const char     *number;

    /* Voice parameters */
    U8              classmap;

    /* Type of address */
    U8              type;
} HfgCallWaitParms;

/*---------------------------------------------------------------------------
 * HfgCallerIdParms structure
 *
 *     Used to identify the calling number.
 */
typedef struct _HfgCallerIdParms {
    /* Phone number of the caller */
    const char     *number;

    /* Type of address */
    U8              type;
} HfgCallerIdParms;

/*---------------------------------------------------------------------------
 * HfgCallListParms structure
 *
 *     Used to identify the listed calls on the audio gateway.
 */
typedef struct _HfgCallListParms{
    /* Index of the call on the audio gateway (1 based) */
    U8              index;

    /* 0 - Mobile Originated, 1 = Mobile Terminated */
    U8              dir;

    /* Call state (see HfgCallStatus) */
    HfgCallStatus   state;

    /* Call mode (see HfgCallMode) */
    HfgCallMode     mode;

    /* 0 - Not Multiparty, 1 - Multiparty */
    U8              multiParty;
    
    /* Phone number of the call */
    const char     *number;

    /* Type of address */
    U8              type;
} HfgCallListParms;

/*---------------------------------------------------------------------------
 * HfgSubscriberNum structure
 *
 *     Used to identify the subscriber number.
 */
typedef struct _HfgSubscriberNum {
    /* String phone number of format specified by "type". */
    const char     *number;

    /* Phone number format */
    AtNumberFormat  type;

    /* Service related to the phone number. */
    U8              service;
} HfgSubscriberNum;

/*---------------------------------------------------------------------------
 * HfgNetworkOperator structure
 *
 *     Used to identify the network operator.
 */
typedef struct _HfgNetworkOperator {
    /* 0 = automatic, 1 = manual, 2 = deregister, 3 = set format only, 
     * 4 = manual/automatic.
     */
    U8              mode;

    /* Format of "oper" parameter (should be set to 0) */


    AtOperFormat    format;



    /* Numeric or long or short alphanumeric name of operator */
    const char     *oper;
} HfgNetworkOperator;

/*---------------------------------------------------------------------------
 * HfgHold structure
 *
 *     Used to describe the hold action and possibly the line for that action.
 */
typedef struct _HfgHold {
    /* Hold action to execute */
    HfgHoldAction action;

    /* Index of the call on the audio gateway (1 based).  If 0, the action
     * does not apply to any particular call.
     */
    U8            index;
} HfgHold;

#if TI_CHANGES == XA_ENABLED
/*---------------------------------------------------------------------------
 * HfgPhonebookRead structure
 *
 *     Used to describe entries to read from a phonebook
 */
typedef 	struct _HfgPhonebookRead {
	U16 first;

	U16 last;
} HfgPhonebookRead;
#endif


/*---------------------------------------------------------------------------
 * HfgChannel structure
 *
 *     The Audio gateway channel.  This structure is used to identify a
 *     connection to the hands-free device.
 */
struct _HfgChannel {

    /* === Internal use only === */

    /* Registration variables */
    ListEntry            node;  
    HfgCallback          callback;         /* Application callback function    */
    RfChannel            rfChannel;        /* RFCOMM channel used for audio
                                            * gateway connection.
                                            */
    /* Transmit Queue */
    ListEntry            rspQueue;         /* List of AT responses             */

    /* Connection State Variables */
    U8                   state;            /* Current connection state         */
    U16                  flags;            /* Current connection flags         */
    U8                   linkFlags;        /* Levels of service connected      */

    /* HandsFree State Information */
    HfgHandsFreeVersion  version;          /* Profile version parsed from SDP  */
    HfgHandsFreeFeatures hfFeatures;       /* Profile features parsed from SDP */

    /* Indicator status */
    BOOL                 indEnabled;       /* Sending of indicators is enabled */
    BOOL                 service;          /* Service indicator                */
    BOOL                 call;             /* The call indicator               */
    HfgCallSetupState    callSetup;        /* Call setup indicator             */
    HfgHoldState         held;             /* Call held indicator              */
    U8                   battery;          /* Battery charge indicator         */
    U8                   signal;           /* Signal strength indicator        */
    U8                   roaming;          /* Roaming indicator                */

    /* Feature State Information */
    BOOL                 callWaiting;      /* Call wait notification active    */
    BOOL                 callId;           /* Caller ID notification active    */
    BOOL                 extendedErrors;   /* Extended errors enabled          */
    BOOL                 nrecDisable;      /* Noise Red/Echo Cancel disabled   */
    BOOL                 voiceRec;         /* Voice Recogintion active         */
    BOOL                 ibRing;           /* In-band ringing active           */

    /* SDP variables for client */
    SdpQueryToken       sdpQueryToken;     /* Used to query the service        */
    U8                  queryFlags;        /* Defines which SDP entries were
                                            * parsed from the gateway.
                                            */
    U8                  rfServerChannel;   /* When connecting AG client        */

    /* Channel Resources */
    CmgrHandler         cmgrHandler;
    AtResults           atResults;
    AtResults          *currentAtResults;
    AtResults          *lastAtResults;
    XaBufferDesc        atBuffer;
    BtPacket            atTxPacket;
    U8                  atTxData[HFG_TX_BUFFER_SIZE];
    U16                 bytesToSend;
    U8                  atRxBuffer[HFG_RECV_BUFFER_SIZE];
    U16                 atRxLen;

#if TI_CHANGES == XA_ENABLED

	BtSecurityRecord	securityRecord;
	BOOL				isSecurityRegistered;

    /* Flag and connection establishing state used for cancellation */
    BOOL                cancelCreateConn;
    HfgCccConnState     cccConnState; 

	TimeT				sniffTimer;			/* The amount of time before
											   sniff mode is entered */
	U16 				minInterval;
	U16 				maxInterval;
	U16 				attempt;
	U16 				timeout;

#endif /* TI_CHANGES == XA_ENABLED */
};

/*---------------------------------------------------------------------------
 * HfgCallbackParms structure
 *
 * This structure is sent to the application's callback to notify it of
 * any state changes.
 */
struct _HfgCallbackParms {
    HfgEvent    event;   /* Event associated with the callback       */

    BtStatus    status;  /* Status of the callback event             */
    BtErrorCode errCode; /* Error code (reason) on disconnect events */

    /* For certain events, a single member of this union will be valid.
     * See HfgEvent documentation for more information.
     */
    union {
        void                   *ptr;
        BtRemoteDevice         *remDev;
        HfgHandsFreeFeatures    features;
        BOOL                    enabled;
        const char             *phoneNumber;
        const char             *memory;        
        U8                      dtmf;
        U8                      gain;
        HfgHold                *hold;
        HfgHandsFreeVersion     version;
        HfgResponse            *response;

#if HFG_USE_RESP_HOLD == XA_ENABLED
        HfgResponseHold         respHold;    /* Only valid if HFG_USE_RESP_HOLD
                                              * is set to XA_ENABLED.
                                              */
#endif

#if BT_SCO_HCI_DATA == XA_ENABLED
        HfgAudioData           *audioData;   /* Only valid if BT_SCO_HCI_DATA is
                                              * set to XA_ENABLED.
                                              */
        BtPacket               *audioPacket; /* Only valid if BT_SCO_HCI_DATA is
                                              * set to XA_ENABLED.
                                              */
#endif

#if TI_CHANGES == XA_ENABLED
#if AT_PHONEBOOK == XA_ENABLED

	HfgPhonebookRead *phonebookRead;
	const char *phonebookFindText;
	const char *charSet;
	AtPbStorageType phonebookStorage;


#endif /* AT_COMMAND_PHONEBOOK_ACCESS == XA_ENABLED */
#endif /* TI_CHANGES == XA_ENABLED */

        HfgAtData              *data;
    } p;

};

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HFG_Init()
 *
 *     Initialize the Audio Gateway SDK.  This function should only be called
 *     once, normally at sytem initialization time.  The calling of this 
 *     function can be specified in overide.h using the XA_LOAD_LIST macro
 *     (i.e. #define XA_LOAD_LIST XA_MODULE(HFG)).
 *
 * Returns:
 *     TRUE - Initialization was successful
 *
 *     FALSE - Initialization failed.
 */
BOOL HFG_Init(void);

/*---------------------------------------------------------------------------
 * HFG_Register()
 *
 *     Registers and initializes a channel for use in creating and receiving
 *     service level connections.  Registers the Hands-Free profile audio
 *     gateway with SDP.  The application callback function is also bound
 *     to the channel.
 *
 * Parameters:
 *     Channel - Contains a pointer to the channel structure that will be
 *         initialized and registered.
 *
 *     Callback - The application callback function that will receive events.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_IN_USE - The operation failed because the channel has already
 *         been initialized. 
 *
 *     BT_STATUS_FAILED - The operation failed because either the RFCOMM
 *         channel or the SDP record could not be registered.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_Register(HfgChannel *Channel, HfgCallback Callback);

/*---------------------------------------------------------------------------
 * HFG_Deregister()
 *
 *     Deregisters the channel.  The channel becomes unbound from RFCOMM and
 *     SDP, and can no longer be used for creating service level connections.
 *
 * Parameters:
 *     Channel - Contains a pointer to the channel structure that will be
 *         deregistered.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_BUSY - The operation failed because a service level connection 
 *         is still open to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_Deregister(HfgChannel *Channel);

/*---------------------------------------------------------------------------
 * HFG_CreateServiceLink()
 *
 *     Creates a service level connection with the hands-free unit.
 *     This includes performing SDP Queries to find the appropriate service
 *     and opening an RFCOMM channel.  The success of the operation is indicated 
 *     by the HFG_EVENT_SERVICE_CONNECTED event.  If the connection fails, the
 *     application is notified by the HFG_EVENT_SERVICE_DISCONNECTED event. 
 * 
 *     If an ACL link does not exist to the audio gateway, one will be 
 *     created first.  If desired, however, the ACL link can be established 
 *     prior to calling this function.
 *
 * Parameters:
 *
 *     Channel - Pointer to a registered channel structure.
 *
 *     Addr - The Bluetooth address of the remote device.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the connection has been created (via the callback 
 *         function registered by HFG_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_BUSY - The operation failed because a connection is already
 *         open to the remote device, or a new connection is being created.
 *
 *     BT_STATUS_FAILED - The channel has not been registered.
 *
 *     BT_STATUS_CONNECTION_FAILED - The connection failed.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_CreateServiceLink(HfgChannel *Channel, BD_ADDR *Addr);

/*---------------------------------------------------------------------------
 * HFG_DisconnectServiceLink()
 *
 *     Releases the service level connection with the hands-free unit. This will 
 *     close the RFCOMM channel and will also close the SCO and ACL links if no 
 *     other services are active, and no other link handlers are in use 
 *     (ME_CreateLink).  When the operation is complete the application will be 
 *     notified by the HFG_EVENT_SERVICE_DISCONNECTED event.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the service level connection is down (via the callback 
 *         function registered by HFG_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_DisconnectServiceLink(HfgChannel *Channel);

#if TI_CHANGES == XA_ENABLED
/*---------------------------------------------------------------------------
 * HFG_AcceptAudioLink()
 *
 *	Accpets or rejects an incoming request to establish an audio connection.
 *	Called in response to HFG_EVENT_AUDIO_CONNECTION_REQ.
 *
 * Parameters:
 *	Channel - Pointer to a registered channel structure,
 *
 *	Error - Set to BEC_NO_ERROR to accept the connection,
 *		otherwise the connection is rejected.
 *
 * Returns:
 *	BT_STATUS_PENDING - The connection will be accepted.
 *		When the connection is up, the event HFG_EVENT_AUDIO_CONNECTED 
 *		will be sent.
 *
 *	BT_STATUS_NOT_FOUND - Could not create audio link,
 *		because the HsgChannel is not registered.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter or handlers did not
 *		match (XA_ERROR_CHECK only).
 */
BtStatus HFG_AcceptAudioLink(HfgChannel *Channel, BtErrorCode Error);
#define HFG_AcceptAudioLink(c,e) CMGR_AcceptAudioLink(&(c)->cmgrHandler, (e))
#endif /* TI_CHANGES == XA_ENABLED*/

/*---------------------------------------------------------------------------
 * HFG_CreateAudioLink()
 *
 *     Creates an audio (SCO) link to the hands-free unit. The success of the 
 *     operation is indicated by the HFG_EVENT_AUDIO_CONNECTED event.  If the 
 *     connection fails, the application is notified by the 
 *     HFG_EVENT_AUDIO_DISCONNECTED event.  
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the audio link has been established (via the callback 
 *         function registered by HFG_Register).
 *
 *     BT_STATUS_SUCCESS - The audio (SCO) link already exists.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service level 
 *         connection does not exist to the audio gateway.
 *
 *     BT_STATUS_FAILED - An audio connection already exists.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_CreateAudioLink(HfgChannel *Channel);

/*---------------------------------------------------------------------------
 * HFG_DisconnectAudioLink()
 *
 *     Releases the audio connection with the hands-free unit.  When the 
 *     operation is complete, the application will be notified by the 
 *     HFG_EVENT_SERVICE_DISCONNECTED event.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the audio connection is down (via the callback 
 *         function registered by HFG_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_DisconnectAudioLink(HfgChannel *Channel);

#if BT_SCO_HCI_DATA == XA_ENABLED
/*---------------------------------------------------------------------------
 * HFG_SendAudioData()
 *
 *     Sends the specified audio data on the audio link.
 *
 * Requires:
 *     BT_SCO_HCI_DATA enabled.
 *
 * Parameters:
 *     Channel - The Channel over which to send the audio data.
 *
 *     packet - The packet of data to send. After this call, the Hands-free
 *         SDK owns the packet. When the packet has been transmitted
 *         to the host controller, HFG_EVENT_AUDIO_DATA_SENT is sent to the
 *         application
 *
 * Returns:
 *     BT_STATUS_PENDING - The packet was queued successfully.
 *
 *     BT_STATUS_NO_CONNECTION - No audio connection exists.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus HFG_SendAudioData(HfgChannel *Channel, BtPacket *Packet);
#define HFG_SendAudioData(c, p) SCO_SendData((c)->cmgrHandler.scoConnect, p)
#endif

/*---------------------------------------------------------------------------
 * HFG_SetIndicatorValue()
 *
 *     Sets the current value for an indicator.  If a service level connection
 *     is active and indicator reporting is currently enabled, the the state 
 *     of the modified indicator is reported to the hands-free device.  If no 
 *     service level connection exists, the current value is changed and will 
 *     be reported during the establishment of the service level connection.
 *     If indicator reporting is disabled, the value of the indicator will only
 *     be reported when requested by the hands-free unit (AT+CIND).
 *
 *     Upon registration of an Audio Gateway (HFG_Register()), all indicators
 *     are initialized to 0.  To properly initialize all indicators, this
 *     function must be called once for each indicator prior to establishing
 *     a service level connection.
 *
 *     Indicators must be sent to the hands-free device as specified by the
 *     hands-free v1.5 specification.  Indicators are sent in the order that
 *     calls are made to this function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Indicator - Indicator type.
 *
 *     Value - The value of the indicator.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The indicator value was set and the Response
 *         structure is available for use.
 *
 *     BT_STATUS_PENDING - The indicator value was set and queued for
 *         sending to the hands-free unit.  When the response has been sent, 
 *         the HFG_EVENT_RESPONSE_COMPLETE event will be received by the
 *         application.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized.
 */
BtStatus HFG_SetIndicatorValue(HfgChannel *Channel, HfgIndicator Indicator, 
                               U8 value, HfgResponse *Response);

/*---------------------------------------------------------------------------
 * HFG_GetIndicatorValue()
 *
 *     Gets the current value of the specified indicator.  
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Indicator - Indicator type.
 *
 *     Value - Receives the value of the indicator.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The indicator value was set and the Response
 *         structure is available for use.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized.
 */
BtStatus HFG_GetIndicatorValue(HfgChannel *Channel, HfgIndicator Indicator, 
                               U8 *value);

/* Forward Reference used by several functions defined below */
BtStatus HFG_SendHfResults(HfgChannel *Channel, AtCommand Command, 
                           U32 Parms, U16 ParmLen, HfgResponse *Response,
                           BOOL done);

/*---------------------------------------------------------------------------
 * HFG_SendOK
 *     Sends an OK response to the hands-free device.  This function must
 *     be called after receiving several events (see the description of each
 *     event).
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_PENDING - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_SendOK(HfgChannel *Channel, HfgResponse *Response);
#define HFG_SendOK(c, r) HFG_SendHfResults(c, AT_OK, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_SendError()
 *
 *     Sends an ERROR result code to the HF.  This function may be called 
 *     after receiving several events when an error condition exists (see 
 *     the description of each event).  If extended error codes are enabled,
 *     the value specified in the 'Error' parameter will be sent with the
 *     extended error response.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Error - Extended error to be sent (if enabled).
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_SendError(HfgChannel *Channel, HfgCmeError Error,
                       HfgResponse *Response);
#define HFG_SendError(c, e, r) HFG_SendHfResults(c, AT_ERROR, (U32)(e), 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_SendRing()
 *
 *     Notifies the HF of an incoming call.  This call is repeated periodically
 *     as long as the call is still incoming.  If caller ID is enabled, a
 *     call to HFG_SendCallerId() should be called after calling HFG_SendRing().
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_SendRing(HfgChannel *Channel, HfgResponse *Response);
#define HFG_SendRing(c, r) HFG_SendHfResults(c, AT_RING, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_SendCallListRsp
 *
 *     Sends the call listing response to the hands-free device (see
 *     HFG_EVENT_LIST_CURRENT_CALLS).  This function should be called for each 
 *     line supported on the audio gateway with the state of any call set 
 *     appropriately.  If no call is active on the specified line, a response 
 *     (+CLCC) will not be sent.  If a call is is any state besides 
 *     HFG_CALL_STATUS_NONE, then a response will be sent.  On the final call 
 *     to this function, FinalFlag should be set.  This will send an OK response 
 *     in addtion to +CLCC (if sent).
 *
 *     If it is known that no call exists on any line, it is acceptable to call 
 *     HFG_SendOK() instead of calling this function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Parms - A structure containing the call status information for the
 *         specified line.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 *     FinalFlag -  Set to TRUE when the final call is make to this function.
 *
 * Returns:
 *     BT_STATUS_PENDING - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_SendCallListRsp(HfgChannel *Channel, HfgCallListParms *Parms, 
                             HfgResponse *Response, BOOL FinalFlag);
#define HFG_SendCallListRsp(c, p, r, f) HFG_SendHfResults(c, AT_LIST_CURRENT_CALLS, (U32)p, sizeof(HfgCallListParms), r, f)

/*---------------------------------------------------------------------------
 * HFG_SendCallerId()
 *
 *     Sends a Calling Line Identification result code containing the phone
 *     number and type of the incoming call.  This function should be called
 *     immediately after HFG_SendRing() if Calling Line Identification Notification
 *     has been enabled by the HF.  If caller ID notification has been disabled
 *     by the remote device, no notification will be sent even if this funcion
 *     is called.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     CallId - A structure containing the number and type of the 
 *         incoming call.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_PENDING - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NOT_SUPPORTED - Caller ID notification is disabled.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_SendCallerId(HfgChannel *Channel, HfgCallerIdParms *CallId, 
                          HfgResponse *Response);
#define HFG_SendCallerId(c, i, r)  HFG_SendHfResults(c, AT_CALL_ID, (U32)i, sizeof(HfgCallerIdParms), r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_SendSubscriberNumberRsp
 *
 *     This function is called in response to a request for the subscriber
 *     number (see HFG_EVENT_QUERY_SUBSCRIBER_NUMBER).  It is not necessary 
 *     to call HFG_SendOK() after calling this function.
 * 
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     SbuNum - A structure containing the subscriber number information.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_PENDING - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_SendSubscriberNumberRsp(HfgChannel *Channel, HfgSubscriberNum *SubNum, 
                                     HfgResponse *Response);
#define HFG_SendSubscriberNumberRsp(c, s, r)  HFG_SendHfResults(c, AT_SUBSCRIBER_NUM, (U32)s, sizeof(HfgSubscriberNum), r, TRUE)

/*---------------------------------------------------------------------------
 * HFG_SendNetworkOperatorRsp
 *     This function is called in response to a request for the network 
 *     operator information (see HFG_EVENT_QUERY_NETWORK_OPERATOR).
 *     It is not necessary to call HFG_SendOK() after calling this function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Oper - A structure containing the operator information.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_PENDING - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_SendNetworkOperatorRsp(HfgChannel *Channel, HfgNetworkOperator *Oper, 
                                  HfgResponse *Response);
#define HFG_SendNetworkOperatorRsp(c, o, r)  HFG_SendHfResults(c, AT_NETWORK_OPERATOR | AT_READ, (U32)o, sizeof(HfgNetworkOperator), r, TRUE)

/*---------------------------------------------------------------------------
 * HFG_SendMicVolume()
 *
 * Notifies the HF of the AG's current microphone volume level. 
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     gain - current volume level.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The speaker volume level has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_SendMicVolume(HfgChannel *Channel, U8 Gain, HfgResponse *Response);
#define HFG_SendMicVolume(c, g, r)  HFG_SendHfResults(c, AT_MICROPHONE_GAIN, (U32)(g), 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_SendSpeakerVolume()
 *
 * Notifies the HF of the AG's current speaker volume level.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     gain - current volume level.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The speaker volume level has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_SendSpeakerVolume(HfgChannel *Channel, U8 Gain, HfgResponse *Response);
#define HFG_SendSpeakerVolume(c, g, r)  HFG_SendHfResults(c, AT_SPEAKER_GAIN, (U32)(g), 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_SendRingToneStatus()
 *
 *     Notifies the HF of in-band ring tone status.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Enabled - TRUE if in-band ring tone enabled, FALSE otherwise.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_SendRingToneStatus(HfgChannel *Channel, BOOL Enabled, 
                                HfgResponse *Response);
#define HFG_SendRingToneStatus(c, e, r)  HFG_SendHfResults(c, AT_IN_BAND_RING_TONE, (U32)(e), 0, r, FALSE)

#if HFG_USE_RESP_HOLD == XA_ENABLED
/*---------------------------------------------------------------------------
 * HFG_SendResponseHoldState()
 *
 *     Notifies the HF of state of Response and Hold.  This function is called
 *     to report the Response and Hold state in response to a request by
 *     the hands-free unit (see HFG_RESPONSE_HOLD), or upon an action taken on 
 *     the audio gateway.
 *
 *     This function is also called in respone to a query for the Response
 *     and Hold state from the hands-free unit (see HFG_QUERY_RESPONSE_HOLD).
 *     This function should be called with the 'State' parameter set to 
 *     HFG_RESP_HOLD_STATE_HOLD if the audio gateway is in the Response and 
 *     Hold state, followed by a call to HFG_SendOK().  Otherwise, the 
 *     application should simply call HFG_SendOK().
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     state - The current Resonse and Hold state.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_SendResponseHoldState(HfgChannel *Channel, 
                                   HfgResponseHold State, 
                                   HfgResponse *Response);
#define HFG_SendResponseHoldState(c, s, r)  HFG_SendHfResults(c, AT_RESPONSE_AND_HOLD, (U32)s, sizeof(HfgResponseHold), r, FALSE)
#endif

/*---------------------------------------------------------------------------
 * HFG_CallWaiting()
 *
 *     Notifies the HF of a waiting call (if the HF has enabled the Call 
 *     Waiting Notification feature)
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     CallWait - A structure containing the number, type, and class of the 
 *         incoming call.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_CallWaiting(HfgChannel *Channel, HfgCallWaitParms *CallWait,
                         HfgResponse *Response);
#define HFG_CallWaiting(c, w, r)  HFG_SendHfResults(c, AT_CALL_WAIT_NOTIFY, (U32)w, sizeof(HfgCallWaitParms), r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_EnableVoiceRec()
 *
 *     Notifies the HF that voice recognition has been disabled (if the HF has
 *     activated the voice recognition functionality in the AG)
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Enabled - TRUE if voice recognition is active, otherwise FALSE>
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_EnableVoiceRec(HfgChannel *Channel, BOOL Enabled, HfgResponse *Response);
#define HFG_EnableVoiceRec(c, e, r)  HFG_SendHfResults(c, AT_VOICE_RECOGNITION, (U32)(e), 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_VoiceTagRsp()
 *
 *     Called by the app to return the phone number associated with the VoiceTag
 *     request to the HF.  It is not necessary to call HFG_SendOK() after 
 *     calling this function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Number - A structure containing the phone number associated with the
 *         last voice tag.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_VoiceTagRsp(HfgChannel *Channel, const char *number, 
                         HfgResponse *Response);
#define HFG_VoiceTagRsp(c, n, r)  HFG_SendHfResults(c, AT_VOICE_TAG, (U32)n, sizeof(n), r, TRUE)

/*---------------------------------------------------------------------------
 * HFG_NoCarrierRsp()
 *
 *     Called by the app to send the "NO CARRIER" response to the HF.  This
 *     response can be sent in addition to the "+CME ERROR:" response.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_NoCarrierRsp(HfgChannel *Channel, HfgResponse *Response);
#define HFG_NoCarrierRsp(c, r)  HFG_SendHfResults(c, AT_NO_CARRIER, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_BusyRsp()
 *
 *     Called by the app to send the "BUSY" response to the HF.  This
 *     response can be sent in addition to the "+CME ERROR:" response.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_BusyRsp(HfgChannel *Channel, HfgResponse *Response);
#define HFG_BusyRsp(c, r)  HFG_SendHfResults(c, AT_BUSY, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_NoAnswerRsp()
 *
 *     Called by the app to send the "NO ANSER" response to the HF.  This
 *     response can be sent in addition to the "+CME ERROR:" response.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_NoAnswerRsp(HfgChannel *Channel, HfgResponse *Response);
#define HFG_NoAnswerRsp(c, r)  HFG_SendHfResults(c, AT_NO_ANSWER, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_DelayedRsp()
 *
 *     Called by the app to send the "DELAYED" response to the HF.  This
 *     response can be sent in addition to the "+CME ERROR:" response.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_DelayedRsp(HfgChannel *Channel, HfgResponse *Response);
#define HFG_DelayedRsp(c, r)  HFG_SendHfResults(c, AT_DELAYED, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_BlacklistedRsp()
 *
 *     Called by the app to send the "BLACKLISTED" response to the HF.  This
 *     response can be sent in addition to the "+CME ERROR:" response.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_BlacklistedRsp(HfgChannel *Channel, HfgResponse *Response);
#define HFG_BlacklistedRsp(c, r)  HFG_SendHfResults(c, AT_BLACKLISTED, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_SendAtResponse()
 *
 *     Sends any AT response.  The 'AtString' parameter must be initialized,
 *     and the AT response must be formatted properly.  It is not necessary
 *     to add CR/LF at the beginning and end of the string.
 *
 *     When the AT response has been sent, the HFG_EVENT_RESPONSE_COMPLETE
 *     event will be received by the application's callback function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     AtString - An properly formated AT response.
 *   
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the response has been sent (via the callback function 
 *         registered by HFG_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HFG_SendAtResponse(HfgChannel *Channel, const char *AtString,
                            HfgResponse *Response);
#define HFG_SendAtResponse(c, s, r)  HFG_SendHfResults(c, AT_RAW, (U32)s, sizeof(s), r, TRUE)

/*---------------------------------------------------------------------------
 * HFG_IsNRECEnabled()
 *
 *     Returns TRUE if Noise Reduction and Echo Cancelling is enabled in the
 *     audio gateway.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE - NREC is enabled in the AG.
 *
 *     FALSE - NREC is disabled in the AG.
 */
BOOL HFG_IsNRECEnabled(HfgChannel *Channel);

/*---------------------------------------------------------------------------
 * HFG_IsInbandRingEnabled()
 *
 *     Returns TRUE if In-band Ringing is enabled in the audio gateway.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE - In-band ringing is enabled in the AG.
 *
 *     FALSE - In-band ringing is disabled in the AG.
 */
BOOL HFG_IsInbandRingEnabled(HfgChannel *Channel);

/*---------------------------------------------------------------------------
 * HFG_IsCallIdNotifyEnabled()
 *
 *     Returns TRUE if Caller ID notification is enabled in the audio gateway.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE - Caller ID notification is enabled in the AG.
 *
 *     FALSE - Caller ID notification is disabled in the AG.
 */
BOOL HFG_IsCallIdNotifyEnabled(HfgChannel *Channel);

/*---------------------------------------------------------------------------
 * HFG_IsVoiceRecActive()
 *
 *     Returns TRUE if Voice Recognition is active in the audio gateway.  
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE - Voice Recognition is active in the AG.
 *
 *     FALSE - Voice Recognition is inactive in the AG.
 */
BOOL HFG_IsVoiceRecActive(HfgChannel *Channel);

/*---------------------------------------------------------------------------
 * HFG_IsCallWaitingActive()
 *
 *     Returns TRUE if Call Waiting is active in the audio gateway.  
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE - Call Waiting is active in the AG.
 *
 *     FALSE - Call Waiting is inactive in the AG.
 */
BOOL HFG_IsCallWaitingActive(HfgChannel *Channel);

#if HFG_SNIFF_TIMER >= 0
/*---------------------------------------------------------------------------
 * HFG_EnableSniffMode
 *
 *     Enables/Disables placing link into sniff mode.
 *
 * Requires:
 *     HFG_SNIFF_TIMER >= 0.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Enabled - If TRUE, sniff mode will be used, otherwise sniff mode is
 *               disabled
 */
BtStatus HFG_EnableSniffMode(HfgChannel *Channel, BOOL Enable);

/*---------------------------------------------------------------------------
 * HFG_IsSniffModeEnabled
 *
 *     Returns TRUE when sniff mode is enabled on the specified handler.
 *
 * Requires:
 *     HFG_SNIFF_TIMER >= 0.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE if sniff mode is enabled.
 *
 *     FALSE if sniff mode is not enabled.
 */
BOOL HFG_IsSniffModeEnabled(HfgChannel *Channel);
#define HFG_IsSniffModeEnabled(c) (CMGR_GetSniffTimer(&((c)->cmgrHandler)) > 0)
#endif

/*---------------------------------------------------------------------------
 * HFG_SetSniffExitPolicy()
 *
 *     Sets the policy for exiting sniff mode on the specified channel.  The 
 *     policy can be set to HFG_SNIFF_EXIT_ON_SEND or HFG_SNIFF_EXIT_ON_AUDIO_LINK.
 *     These values can also be OR'd together to enable both (See 
 *     HfgSniffExitPolicy).
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 * 
 *     Policy - Bitmask that defines the policy for exiting sniff mode.
 *
 * Returns:
 *
 *     BT_STATUS_SUCCESS - The policy was set successfully.
 *
 *     BT_STATUS_NOT_FOUND - Could not set the sniff policy, because  
 *         Handler is not registered.
 */
BtStatus HFG_SetSniffExitPolicy(HfgChannel *Channel, HfgSniffExitPolicy Policy);
#define HFG_SetSniffExitPolicy(c, p) CMGR_SetSniffExitPolicy(&((c)->cmgrHandler), (p));

/*---------------------------------------------------------------------------
 * HFG_GetSniffExitPolicy()
 *
 *     Gets the policy for exiting sniff mode on the specified channel.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 * 
 * Returns:
 *
 *     HfSniffExitPolicy
 */
HfgSniffExitPolicy HFG_GetSniffExitPolicy(HfgChannel *Channel);
#define HFG_GetSniffExitPolicy(c)  CMGR_GetSniffExitPolicy(&((c)->cmgrHandler))

/*---------------------------------------------------------------------------
 * HFG_SetMasterRole()
 *
 *     Attempts to keep the local device in the Master role.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Flag - TRUE if this device wants to be the master, otherwise FALSE.
 * 
 * Returns:
 *
 *     BtStatus
 */
BtStatus HFG_SetMasterRole(HfgChannel *Channel, BOOL Flag);
#define HFG_SetMasterRole(c, f)  CMGR_SetMasterRole(&((c)->cmgrHandler), f)

#if TI_CHANGES == XA_ENABLED
#if BT_SECURITY == XA_ENABLED
/*---------------------------------------------------------------------------
 * HFG_SetSecurityLevel()
 *
 *	Sets the security level for the HFG service
 *	(incoming connections).
 *
 * Parameters:
 *	Level - The security level.
 * 
 * Returns:
 */
void HFG_SetSecurityLevel(BtSecurityLevel Level);

/*---------------------------------------------------------------------------
 * HFG_GetSecurityLevel()
 *
 *	Gets the current security level for the HFG service
 *	(incoming connections).
 *
 * Parameters:
 * 
 * Returns:
 *	The current security level
 */
BtSecurityLevel HFG_GetSecurityLevel(void);
#endif /* BT_SECURITY == XA_ENABLED */

/*---------------------------------------------------------------------------
 * HFG_SetFeatures()
 *
 *	Sets the features for the HFG service.
 *
 * Parameters:
 *	Features - audio gateway features.
 * 
 * Returns:
 *	BT_STATUS_SUCCESS - Operation successful.
 *
 *	BT_STATUS_FAILED - Couldn't update the the SDP database.
 *
 *	BT_STATUS_INVALID_PARM - The HFG SDP record is not valid.
 */



BtStatus HFG_SetFeatures(AtAgFeatures Features);

/*---------------------------------------------------------------------------
 * HFG_GetFeatures()
 *
 *	Get the current features of the HFG service.
 *
 * Parameters:
 * 
 * Returns:
 *	The HFG service's features.
 */

AtAgFeatures HFG_GetFeatures(void);

/*---------------------------------------------------------------------------
 * HFG_AcceptIncomingSLC()
 *
 *	Accept an incoming SLC
 *
 * Parameters:
 * 
 * Returns:
 */
BtStatus HFG_AcceptIncomingSLC(HfgChannel *channel, BD_ADDR *addr);

/*---------------------------------------------------------------------------
 * HFG_RejectIncomingSLC()
 *
 *	Reject an incoming SLC
 *
 * Parameters:
 * 
 * Returns:
 */
BtStatus HFG_RejectIncomingSLC(HfgChannel *channel);

/*---------------------------------------------------------------------------
 * HFG_SetCallHolfFlags()
 *
 *	Set call hold flags
 *
 * Parameters:
 * 
 * Returns:
 */

BtStatus HFG_SetCallHolfFlags(AtHoldFlag callHoldFlags);


#endif /* TI_CHANGES == XA_ENABLED */

#endif /* __HFG_H_ */
