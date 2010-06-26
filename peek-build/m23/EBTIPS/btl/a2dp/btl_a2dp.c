/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_a2dp.c
*
*   DESCRIPTION:    This file contains the BTL A2DP Source implementation.
*
*   AUTHOR:         Keren Ferdman
*
\*******************************************************************************/


/*******************************************************************************/

#define BTL_A2DP_PRAGMAS


#include "btl_config.h"
#include "btl_defs.h"


BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_A2DP);

#if BTL_CONFIG_A2DP ==   BTL_CONFIG_ENABLED

/*******************************************************************************/


#define A2DP_USE_TIMER		XA_ENABLED 


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bthal_config.h"

#include "btl_a2dp.h"
#include "bthal_mm.h"
#include "bthal_os.h"
#include "btl_pool.h"
#include "btl_commoni.h"
#include "l2cap.h"
#include "a2dpi.h"
#include "config.h"
#include "bthal_common.h"
#include "sbc.h"
#include "xastatus.h"
#include "a2alloc.h"
#include "btalloc.h"
#include "debug.h"
#include "btl_bsc.h"

#if HCI_QOS == XA_ENABLED
#include "hci.h"
#endif /* HCI_QOS == XA_ENABLED */

#if (BTL_CONFIG_A2DP_SAMPLE_RATE_CONVERTER == BTL_CONFIG_ENABLED)
#include "sample_rate_converter.h"
#endif	/* BTL_CONFIG_A2DP_SAMPLE_RATE_CONVERTER == BTL_CONFIG_ENABLED */



/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

#if BTL_CONFIG_A2DP_MAX_NUM_OF_CONTEXTS > 1
#error BTL_CONFIG_A2DP_MAX_NUM_OF_CONTEXTS can only be 1
#endif


#if BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT < 1
#error BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT must be greater than 0
#endif


/*-------------------------------------------------------------------------------
 * A2DP_MAX_NUM_STREAMS_PER_CONTEXT constant
 *
 *     Represents max number of streams per context.
 */
#define A2DP_MAX_NUM_STREAMS_PER_CONTEXT  (BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT + \
											BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT)

/*-------------------------------------------------------------------------------
 * A2DP_CUR_MAX_NUM_PACKETS_PER_CONTEXT constant
 *
 *     Represents max number of packets per context for the current codec.
 */
#define A2DP_CUR_MAX_NUM_PACKETS_PER_CONTEXT  \
			((btlA2dpData.currentCodecType == AVDTP_CODEC_TYPE_SBC)  ? 			\
				BTL_CONFIG_A2DP_MAX_NUM_SBC_PACKETS_PER_CONTEXT :			\
				BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_PACKETS_PER_CONTEXT)

/*-------------------------------------------------------------------------------
 * A2DP_CUR_MAX_NUM_STREAMS_PER_CONTEXT constant
 *
 *     Represents max number of streams per context for the current codec.
 */
#define A2DP_CUR_MAX_NUM_STREAMS_PER_CONTEXT  \
			((btlA2dpData.currentCodecType == AVDTP_CODEC_TYPE_SBC)  ? 			\
				BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT :			\
				BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT)

/*-------------------------------------------------------------------------------
 * BSL_A2DP_DEFAULT constant
 *
 *     Represents default security level for A2DP SRC.
 */
#define BSL_A2DP_DEFAULT  (BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT)


/*-------------------------------------------------------------------------------
 * MAX_NUM_OF_BYTES_SPECIFIC_INFO_ELEMENTS constant
 *
 *     Represents the max number of bytes in the specific information elements.
 */
#define MAX_NUM_OF_BYTES_SPECIFIC_INFO_ELEMENTS				(4)


/*-------------------------------------------------------------------------------
 * A2DP_SERVICE_NAME_MAX_LEN constant
 *
 *     Represents max length of service name for A2DP.
 */
#define A2DP_SERVICE_NAME_MAX_LEN  							(32)


/*-------------------------------------------------------------------------------
 * A2DP_SUPPORTED_FEATURES_ARRAY_SIZE constant
 *
 *     Represents supported features array size for A2DP.
 */
#define A2DP_SUPPORTED_FEATURES_ARRAY_SIZE  				(3)


/*-------------------------------------------------------------------------------
 * A2DP_OS_EVENT_TIMER_EXPIRED constant
 *
 *     	This event is sent from to the A2DP task when the A2DP timer expired.
 */
#define A2DP_OS_EVENT_TIMER_EXPIRED							(0x00000001)


/*-------------------------------------------------------------------------------
 * A2DP_OS_EVENT_DATA_IND constant
 *
 *     	This event is sent from the MM task to the A2DP task on each new data 
 *		indication.
 */
#define A2DP_OS_EVENT_DATA_IND								(0x00000002)


/*-------------------------------------------------------------------------------
 * A2DP_OS_EVENT_DATA_SENT constant
 *
 *     This event is sent from the stack task to the A2DP task on each sent packet.
 */
#define A2DP_OS_EVENT_DATA_SENT								(0x00000004)


/*-------------------------------------------------------------------------------
 * A2DP_OS_EVENT_CONFIG_IND constant
 *
 *     	This event is sent from the MM task to the A2DP task on each new config 
 *		indication.
 */
#define A2DP_OS_EVENT_CONFIG_IND							(0x00000008)


#define	BTL_A2DP_INC_BITPOOL_TICKS	MS_TO_TICKS(BTL_A2DP_INC_BITPOOL_MS)


/*-------------------------------------------------------------------------------
 * BTL_A2DP_MAX_MPEG1_2_AUDIO_HEADER_LENGTH
 *
 *     	Defines the max length of MP3 media header packet.
 */
#define BTL_A2DP_MAX_MPEG1_2_AUDIO_HEADER_LENGTH 							(16)


/*-------------------------------------------------------------------------------
 * BTL_A2DP_MEDIA_HEADER_LENGTH
 *
 *     	the default length of media (RTP) header. 
 *           it should be 12 but ESI profiles uses 13 in its calculations, so we align accordingly
 */
#define BTL_A2DP_MEDIA_HEADER_LENGTH									(13)


/*-------------------------------------------------------------------------------
 * 	BTL_A2DP_TIME_RESOLUTION
 *
 *     	resolution of time measurement as a fraction of  ms. 
 *           10 means 0.1 ms, 100 means 0.01 ms, etc
 */
#define BTL_A2DP_TIME_RESOLUTION	(10)


#define	TICKS_TO_MS(ticks)	BTHAL_OS_TICKS_TO_MS(ticks)


/*-------------------------------------------------------------------------------
 * 	BTL_A2DP_INCREASE_RATE
 *
 *     	optionally "increase" the speed. the effect of this is sending data faster than we believe is required.
 *           A2DP_RATE_INCREASE_FACTOR is in 0.1% units.
 */
#define	BTL_A2DP_INCREASE_RATE(rate)	((rate) * (1000 + A2DP_RATE_INCREASE_FACTOR) / 1000)


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _BtlA2dpSbcPacket 			BtlA2dpSbcPacket;
typedef struct _BtlA2dpRawBlock 			BtlA2dpRawBlock;
typedef struct _BtlA2dpStream 				BtlA2dpStream;
typedef struct _BtlA2dpData 				BtlA2dpData;

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0
typedef struct _BtlA2dpMpeg1_2_AudioPacket 	BtlA2dpMpeg1_2_AudioPacket;
#endif 


/*-------------------------------------------------------------------------------
 * BtlA2dpProcessRawBlock type
 *
 *     A function of this type is called to process raw block of a certain type 
 *	   (PCM, SBC or MP3).
 */
typedef BOOL (*BtlA2dpProcessRawBlock)(BtlA2dpRawBlock *rawBlock);


/*-------------------------------------------------------------------------------
 * BtlA2dpProcessQueue type
 *
 *     A function of this type is called to process a queue of a certain type 
 *	   (PCM, SBC or MP3).
 */
typedef  U32 ( *BtlA2dpProcessQueue)(void);


/*-------------------------------------------------------------------------------
 * BtlA2dpInitState type
 *
 *     Defines the A2DP init state.
 */
typedef U8 BtlA2dpInitState;

#define BTL_A2DP_INIT_STATE_NOT_INTIALIZED					(0x00)
#define BTL_A2DP_INIT_STATE_INITIALIZED						(0x01)
#define BTL_A2DP_INIT_STATE_INITIALIZATION_FAILED			(0x02)
#define BTL_A2DP_INIT_STATE_DEINITIALIZATION_FAILED			(0x03)


/*-------------------------------------------------------------------------------
 * BtlA2dpState type
 *
 *     Defines the A2DP state of a specific context.
 */
typedef U8 BtlA2dpState;

#define BTL_A2DP_STATE_DISABLED								(0x00)
#define BTL_A2DP_STATE_ENABLED								(0x01)
#define BTL_A2DP_STATE_DISABLING							(0x02)


/*-------------------------------------------------------------------------------
 * BtlA2dpStreamState type
 *
 *     Defines the connection A2DP state of a specific stream.
 */
typedef U8 BtlA2dpStreamState;

#define BTL_A2DP_STREAM_STATE_NONE							(0x00)	/* Context is not enabled */
#define BTL_A2DP_STREAM_STATE_DISCONNECTED					(0x01)	/* No AVDTP connection */
#define BTL_A2DP_STREAM_STATE_CONNECTING					(0x02)	/* Connecting AVDTP */
#define BTL_A2DP_STREAM_STATE_CONNECT_IND					(0x03)	/* Open request indication */
#define BTL_A2DP_STREAM_STATE_CONNECTED						(0x04)	/* AVDTP connection exists */
#define BTL_A2DP_STREAM_STATE_DISCONNECTING					(0x05)	/* Disconnecting AVDTP connection */


/*-------------------------------------------------------------------------------
 * A2dpMp3CodecInfElement0 type
 *     
 *     The first byte of an MP3 codec information element contains the
 *     layer, CRC, and Channel Mode mode of the stream.  These values
 *     can all be OR'd together to describe the capabilities of the codec.
 */
typedef U8 A2dpMp3CodecInfElement0;

#define A2DP_MP3_CODEC_LAYER_1             					(0x80)
#define A2DP_MP3_CODEC_LAYER_2            					(0x40)
#define A2DP_MP3_CODEC_LAYER_3             					(0x20)
#define A2DP_MP3_CODEC_CRC_SUPPORTED       					(0x10)
#define A2DP_MP3_CODEC_CRC_NONE            					(0x00)
#define A2DP_MP3_CODEC_MODE_MONO           					(0x08)
#define A2DP_MP3_CODEC_MODE_DUAL_CHNL      					(0x04)
#define A2DP_MP3_CODEC_MODE_STEREO         					(0x02)
#define A2DP_MP3_CODEC_MODE_JOINT          					(0x01)


/*-------------------------------------------------------------------------------
 * A2dpMp3CodecInfElement1 type
 *     
 *     The second byte of an MP3 codec information element contains the
 *     Media Packet Format and the Sampling Fequency of the stream.  These values
 *     can all be OR'd together to describe the capabilities of the codec.
 */
typedef U8 A2dpMp3CodecInfElement1;

#define A2DP_MP3_CODEC_MPF2_NONE           					(0x00)
#define A2DP_MP3_CODEC_MPF2                					(0x40)
#define A2DP_MP3_CODEC_FREQ_16000          					(0x20)
#define A2DP_MP3_CODEC_FREQ_22050          					(0x10)
#define A2DP_MP3_CODEC_FREQ_24000          					(0x08)
#define A2DP_MP3_CODEC_FREQ_32000          					(0x04)
#define A2DP_MP3_CODEC_FREQ_44100          					(0x02)
#define A2DP_MP3_CODEC_FREQ_48000          					(0x01)


/*-------------------------------------------------------------------------------
 * A2dpMp3CodecInfElement2 type
 *     
 *     The last 2 bytes of an MP3 codec information element contain the
 *     Variable Bit Rate flag and the Bit Rate Inidex of the stream.  These values
 *     can all be OR'd together to describe the capabilities of the codec.
 */
typedef U16 A2dpMp3CodecInfElement2;

#define A2DP_MP3_CODEC_VBR_NONE            					(0x0000)
#define A2DP_MP3_CODEC_VBR                 					(0x8000)
#define A2DP_MP3_CODEC_BITRATE_INDEX_14    					(0x4000)
#define A2DP_MP3_CODEC_BITRATE_INDEX_13    					(0x2000)
#define A2DP_MP3_CODEC_BITRATE_INDEX_12    					(0x1000)
#define A2DP_MP3_CODEC_BITRATE_INDEX_11    					(0x0800)
#define A2DP_MP3_CODEC_BITRATE_INDEX_10    					(0x0400)
#define A2DP_MP3_CODEC_BITRATE_INDEX_9     					(0x0200)
#define A2DP_MP3_CODEC_BITRATE_INDEX_8     					(0x0100)
#define A2DP_MP3_CODEC_BITRATE_INDEX_7     					(0x0080)
#define A2DP_MP3_CODEC_BITRATE_INDEX_6     					(0x0040)
#define A2DP_MP3_CODEC_BITRATE_INDEX_5     					(0x0020)
#define A2DP_MP3_CODEC_BITRATE_INDEX_4     					(0x0010)
#define A2DP_MP3_CODEC_BITRATE_INDEX_3     					(0x0008)
#define A2DP_MP3_CODEC_BITRATE_INDEX_2     					(0x0004)
#define A2DP_MP3_CODEC_BITRATE_INDEX_1     					(0x0002)
#define A2DP_MP3_CODEC_BITRATE_INDEX_0     					(0x0001)


/*-------------------------------------------------------------------------------
 * BtlA2dpPullDataStateMask type
 *
 *     Defines the A2DP pull data state mask.
 */
typedef U8 BtlA2dpPullDataStateMask;

#define BTL_A2DP_PULL_DATA_STATE_MASK_NONE					(0x00)
#define BTL_A2DP_PULL_DATA_STATE_MASK_PULL_ACTIVE			(0x01)
#define BTL_A2DP_PULL_DATA_STATE_MASK_PULL_ON_HOLD			(0x02)


/*-------------------------------------------------------------------------------
 * BtlA2dpDisableStateMask type
 *
 *     Defines the A2DP disable state mask.
 */
typedef U8 BtlA2dpDisableStateMask;

#define BTL_A2DP_DISABLE_STATE_MASK_NONE					(0x00)
#define BTL_A2DP_DISABLE_STATE_MASK_DISABLE_ACTIVE			(0x01)
#define BTL_A2DP_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE		(0x02)
#define BTL_A2DP_DISABLE_STATE_MASK_ENABLED					(0x04)


/*-------------------------------------------------------------------------------
 * BtlA2dpReconfigMask type
 *
 *     Defines the stream reconfig state mask.
 */
typedef U8 BtlA2dpReconfigMask;

#define BTL_A2DP_RECONFIG_STATE_MASK_NONE					(0x00)
#define BTL_A2DP_RECONFIG_STATE_MASK_IN_RECONFIG			(0x01) /* stream is open and reconfiguring now */
#define BTL_A2DP_RECONFIG_STATE_MASK_PENDING_START		(0x02) /* a StartStream ommand is pending for a reconfiguring stream */



/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * BtlA2dpSbcPacket structure
 *
 *     Represents BTL A2DP SBC packet.
 */
struct _BtlA2dpSbcPacket
{	
	ListEntry node;             /* Must be first */
	
	A2dpSbcPacket packets[BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT];

#if SBC_ENCODER == XA_ENABLED

#if HCI_QOS == XA_ENABLED
	/* pointer to the packet data memory in the array of packets */
	U8 *sbcData;
#else
	/* Actual memory for sbc packet data */
	U8 sbcData[BTL_A2DP_SBC_MAX_DATA_SIZE];
#endif /* HCI_QOS == XA_ENABLED */

#else
	U8 *sbcData;

#endif /* SBC_ENCODER == XA_ENABLED */

	 /* descriptor of SBC block from MM, used only in external SBC encoder mode */
	BthalMmDataBufferDescriptor	descriptor;

	/* Reference counter (used when packet is sent to multiple streams) */
	U32 refCounter;

};


#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

/*-------------------------------------------------------------------------------
 * BtlA2dpMpeg1_2_AudioPacket structure
 *
 *     Represents BTL A2DP MP3 packet.
 */
struct _BtlA2dpMpeg1_2_AudioPacket
{	
	ListEntry node;             /* Must be first */
	
	BtPacket packets[BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT];

	BthalMmDataBufferDescriptor	descriptor;

	/* Reference counter (used when packet is sent to multiple streams) */
	U32 refCounter;

	U16 frameOffset;
	
#if HCI_QOS == XA_ENABLED
	/* pointer to the packet data memory in the array of packets */
	U8 *mp3Data;
#endif	/* HCI_QOS == XA_ENABLED */
};

#endif

/*-------------------------------------------------------------------------------
 * BtlA2dpRawBlock structure
 *
 *     Represents BTL A2DP raw block, given by MM.
 */
struct _BtlA2dpRawBlock
{	
	ListEntry node;             /* Must be first */
	
	BthalMmDataBufferDescriptor	descriptor;
	
	U8 *data;

	U16 dataLength;

	BthalMmStreamType streamType;

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

	/* Frame offset is relevant only in case of MPEG-1,2 Audio */
	U16 frameOffset;

#endif
};


/*-------------------------------------------------------------------------------
 * BtlA2dpStream structure
 *
 *     Represents BTL A2DP stream.	
 */
struct _BtlA2dpStream
{	
	/* Current state of the stream */
	BtlA2dpStreamState state;
		
	/* Internal A2dp stream */
	A2dpStream stream;

	/* Local codec capabilities */
	AvdtpCodec localCodec;

	/* Local elements capabilities */
	U8 localElements[MAX_NUM_OF_BYTES_SPECIFIC_INFO_ELEMENTS];

	/* Remote codec capabilities */ 
	AvdtpCodec remoteCodec;

	/* Remote elements capabilities */
	U8 remoteElements[MAX_NUM_OF_BYTES_SPECIFIC_INFO_ELEMENTS];

	BOOL remoteCodecValid;

	/* Associated context */
	BtlA2dpContext *context;

	BtlA2dpStreamId streamId;

#if HCI_QOS == XA_ENABLED

	/* will be true if send packets on this stream is allowed */
	BOOL isSendAllowed;

	U8 numOfPacketsSent;
	
	S8 	devIndex;  /* index of the connected device in device table in ME */

#endif /* HCI_QOS == XA_ENABLED */

	/* stream reconfiguration state */
	BtlA2dpReconfigMask reconfigStat;
	
	BthalMmTransportChannelState lastReportedState;

	/* Indicate wheather stream reconfiguration needed event is needed to be sent */
	BOOL btlA2dpReconfigurationIndNeeded;
};


/*-------------------------------------------------------------------------------
 * BtlA2dpContext structure
 *
 *     Represents BTL A2DP context.
 */
struct _BtlA2dpContext 
{
	/* Must be first field */
	BtlContext base;
	
	/* Internal A2DP streams */
	BtlA2dpStream streams[A2DP_MAX_NUM_STREAMS_PER_CONTEXT];

	/* Associated callback with this context */
	BtlA2dpCallBack callback;

	/* Current A2DP state of the context */
	BtlA2dpState state;

	/* Current number of free raw blocks in rawBlockPool */
	U16 freeRawBlockCounter;

	/* Current number of used packets from the SBC/MP3 paket pool */
	S16 usedPacketsCounter;
	
	/* Actual memory for raw blocks */
	BtlA2dpRawBlock rawBlocks[BTL_CONFIG_A2DP_MAX_NUM_RAW_BLOCKS_PER_CONTEXT];

	/* Available empty raw blocks */
	ListEntry rawBlockPool;

	/* Raw blocks queue */
	ListEntry rawBlockQueue;

#if HCI_QOS == XA_ENABLED
	/* Actual memory for SBC or MP3 packet data */
	U8 mediaData[max(BTL_CONFIG_A2DP_MAX_NUM_SBC_PACKETS_PER_CONTEXT,BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_PACKETS_PER_CONTEXT)]
							[BTL_A2DP_SBC_MAX_DATA_SIZE];
#endif /* HCI_QOS == XA_ENABLED */

	/* Actual memory for SBC packets */
	BtlA2dpSbcPacket sbcPackets[BTL_CONFIG_A2DP_MAX_NUM_SBC_PACKETS_PER_CONTEXT];

	/* Available empty SBC packets */
	ListEntry sbcPacketPool;

	/* SBC packets queue */
	ListEntry sbcPacketQueue;

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

	/* Actual memory for MP3 packets */
	BtlA2dpMpeg1_2_AudioPacket mpeg1_2_audioPackets[BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_PACKETS_PER_CONTEXT];

	/* Available empty MP3 packets */
	ListEntry mpeg1_2_audioPacketPool;

	/* MP3 packets queue */
	ListEntry mpeg1_2_audioPacketQueue;

	/* MP3 Media Header */
	AvdtpMediaHeader mpeg1_2_audioMediaHeader;

	/* Saved MP3 layer (layer I, layer II, or layer III) */
	A2dpMp3CodecInfElement0 mpeg1_2_audioLayer;

	/* Saved MP3 sample frequency (16000, 22050, 24000, 32000, 44100, or 48000) */
	U16 mpeg1_2_audioSampleFreq;

#endif

	/* the actual bit rate of the current codec */
	U32 curBitRate;

	/* Save the current MIN media packet size among all streams */
	U16 maxPacketSize;

#if BT_SECURITY == XA_ENABLED

	/* Registered security record */
	BtSecurityRecord securityRecord;

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Service name plus 1 null-terminated char, plus 2 bytes for SDP_TEXT_8BIT */
	U8 serviceName[A2DP_SERVICE_NAME_MAX_LEN + 3];

	/* 3 bytes for SDP_UINT_16BIT */
	U8 supportedFeatures[A2DP_SUPPORTED_FEATURES_ARRAY_SIZE];

	/* This flag indicates the disable state */
	BtlA2dpDisableStateMask disableState;
};


/*-------------------------------------------------------------------------------
 * BtlA2dpData structure
 *
 *     Represents the data of th BTL A2DP module.
 */
struct _BtlA2dpData
{
	/* Pool of A2DP contexts */
	BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_CONFIG_A2DP_MAX_NUM_OF_CONTEXTS, sizeof(BtlA2dpContext));

	/* List of active A2DP contexts */
	ListEntry contextsList;

	/* Event passed to the application */
	BtlA2dpEvent event;

	/* This flag indicates the pull data state */
	BtlA2dpPullDataStateMask pullDataState;

	/* A2DP timer handle */
	BthalOsTimerHandle a2dpTimerHandle;

	/* A2DP MM semaphore handle, used to protect data between the MM task & the A2DP task  */
	BthalOsSemaphoreHandle a2dpMmSemHandle;

	/* A2DP Stack semaphore handle, used to protect data between the Stack task & the A2DP task */
	BthalOsSemaphoreHandle a2dpStackSemHandle;

	/* Current context - assumption: only one context */
	BtlA2dpContext *a2dpContext;

#if SBC_ENCODER == XA_ENABLED

	/* SbcEncoder  sbcEncoder; */

	/* high/middle quality desired */
	BthalMmSbcAudioQuality	audioQuality;

	/* apply varying bitpool scheme */
	BTHAL_BOOL varyBitPool;

	/*  min bitpool selected according to min bitpool of both SRC and SNK */
	U8 selMinBitPool;
	
	/*  max bitpool selected according to min bitpool of both SRC and SNK */
	U8 selMaxBitPool;

	/* last time (in system ticks) when bitpool was updated */
	U32 lastBpUpd;

	/* number of occurrences (of not enough SBC packets) before we decrement bitpool */
	U32 decBpOccurrencesCounter;

	/* holds the fraction of PCM data that was left from previous raw block */
	U8 remainingPcmBuf[SBC_MAX_PCM_DATA];

	/* number PCM bytes remaining in remainingPcmBuf */
	U16 numRemainingPcmBytes;

	/* current number of PCM  bytes needed to encode an SBC frame */
	U16 pcmBytesPerSbcFrame;

#endif /* SBC_ENCODER == XA_ENABLED */

	 /*  SBC stream info for ESI stack, used only in external SBC encoder mode */
	SbcStreamInfo sbcStreamInfo;

	/* Function table to process raw block of a certain type (PCM, SBC or MP3) */
	BtlA2dpProcessRawBlock processRawBlockFuncTable[3];

	/* Index into the array 'processQueueFuncTable' */
	BthalMmStreamType currentCodecType;

	/* Function table to process queue of a certain type (SBC or MP3) */
	BtlA2dpProcessQueue processQueueFuncTable[2];

	/* location of SBC encoder (built-in in BTL or external in BTHAL_MM */
	BthalMmSbcEncoderLocation sbcEncoderLocation;

	/* tokens for HCI commands */
	MeCommandToken		hciCmdToken[2];

	/* current number of streams in streaming state */
	U8 numStreamingStreams;
	
#if	A2DP_USE_TIMER == XA_ENABLED

	/* current required timer delay in ms */
	U32 timerDelay;

	/* last time (in ms) when timer was triggered */
	U32 timerTriggeredTime;

	/* time (in tenths of ms) of SBC/MP3 playback accumulated in the Q befoe starting TX */
	U32 accumulatedMediaTime;

	/* whether we can already send packets from the Q (after we accumulated enough media) */
	BOOL canSendFromQueue;

	/* time in ms of the first sent media packet */
	U32 startSendingTime;

	/* packets we already need to send according to timer, but were missing in queue */
	S32 numPacketsNeedToBeSent;

#endif	/* A2DP_USE_TIMER == XA_ENABLED */


#if (BTL_CONFIG_A2DP_SAMPLE_RATE_CONVERTER == BTL_CONFIG_ENABLED)

	/* Convert input PCM by SRC? */
	BOOL srcConvertPCM;

	/* Holds the SRC output of the PCM block */
	U32 outSrcPcmBlk[(BTL_CONFIG_A2DP_SRC_PCM_OUTPUT_BLOCK_MAX_LEN / 4)];

#endif	/* BTL_CONFIG_A2DP_SAMPLE_RATE_CONVERTER == BTL_CONFIG_ENABLED */
};


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * btlA2dpInitState
 *
 *     Represents the current init state of A2DP module.
 */
static BtlA2dpInitState btlA2dpInitState = BTL_A2DP_INIT_STATE_NOT_INTIALIZED;


/*-------------------------------------------------------------------------------
 * btlA2dpData
 *
 *     Represents the data of A2DP module.
 */
static BtlA2dpData btlA2dpData;  


#if SBC_ENCODER == XA_ENABLED
	SbcEncoder  sbcEncoder;
#endif

/*-------------------------------------------------------------------------------
 * btlA2dpContextsPoolName
 *
 *     Represents the name of the A2DP contexts pool.
 */
static const char btlA2dpContextsPoolName[] = "A2dpContexts";

/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
static BtStatus BtlA2dpDisable(BtlA2dpContext *a2dpContext);
static BtStatus BtlA2dpDeregisterAll(BtlA2dpContext *a2dpContext);
static void BtlA2dpCallback(A2dpStream *Stream, A2dpCallbackParms *Info);
static void BtlMmCallback(BthalMmCallbackParms *callbackParams);
static void BtlA2dpConvertAvdtp2Bthal(AvdtpCodec *codec, BthalMmConfigParams *codecInfo); 
static BtStatus BtlA2dpSbcSelectionAlgorithem(AvdtpCodec *supportedCodec, AvdtpCodec *selectedCodec); 
static void BtlA2dpConvertBthal2Avdtp(BthalMmConfigParams *codecInfo, AvdtpCodec *codec); 
static BtStatus BtlA2dpSelectCodec(BtlA2dpStream *btlStream, AvdtpCodec *codec);
static const char *BtlA2dpCodecType(AvdtpCodecType codecType);
static void BtlA2dpPrintCodecInfo(AvdtpCodec *codec);
static void BtlA2dpOsInit(void);
static void BtlA2dpOsDeinit(void);
static void BtlA2dpOsResetTimer(U32 time);
static void BtlA2dpOsCancelTimer(void);
static void BtlA2dpOsEventCallback(BthalOsEvent evtMask);
static BOOL btlA2dpProcessPcmBlock(BtlA2dpRawBlock *rawBlock);
static BOOL btlA2dpProcessSbcBlock(BtlA2dpRawBlock *rawBlock);
static BOOL btlA2dpProcessMpeg1_2_AudioBlock(BtlA2dpRawBlock *rawBlock);	
static BtlA2dpRawBlock *btlA2dpGetRawBlockFromQueue(void);
static void btlA2dpHandleOsEventDataSent(void);
static BtlA2dpSbcPacket *btlA2dpGetSbcPacketFromPool(void);
static void btlA2dpInsertSbcPacketToQueue(BtlA2dpSbcPacket *sbcPacket);
#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0
static BtlA2dpMpeg1_2_AudioPacket *btlA2dpGetMpeg1_2_AudioPacketFromPool(void);
static void btlA2dpInsertMpeg1_2_AudioPacketToPool(BtlA2dpMpeg1_2_AudioPacket *mp3Packet);
static void btlA2dpInsertMpeg1_2_AudioPacketToQueue(BtlA2dpMpeg1_2_AudioPacket *mp3Packet);
static BtlA2dpMpeg1_2_AudioPacket *btlA2dpGetMpeg1_2_AudioPacketFromQueue(void);
#endif
static void btlA2dpCheckAndReconfigStream(void);
static void btlA2dpCodecConfigured(BtlA2dpStream *btlStream, AvdtpCodec *Codec, BtlA2dpStreamId streamId);
static BtlA2dpSbcPacket *btlA2dpGetSbcPacketFromQueue(void);
static void btlA2dpInsertRawBlockToPool(BtlA2dpRawBlock *rawBlock);
static void btlA2dpInsertRawBlockToQueue(BtlA2dpRawBlock *rawBlock);
static BtlA2dpRawBlock *btlA2dpGetRawBlockFromPool(void);
static U32 btlA2dpProcessSbcQueue(void);
static U32 btlA2dpProcessMpeg1_2_AudioQueue(void);
static void btlA2dpInsertSbcPacketToPool(BtlA2dpSbcPacket *sbcPacket);
static A2dpError BtlA2dpCheckCodecCompatibility(AvdtpCodec *localCodecCapabilities, AvdtpCodec *remoteCodecRequest);
static void btlA2dpAdjustBitPool(S16 interval);
static  U8 btlA2dpSelectBitpool(SbcStreamInfo *streamInfo, U8 minBitPool, U8 maxBitPool);
static void btlA2dpUpdateMaxMediaPacketSize(void);
static BOOL btlA2dpIsStreamOpenAllowed(BtlA2dpStream *btlStream);
static BtStatus BtlA2dpBtlNotificationsCb(BtlModuleNotificationType notificationType);
static U16 btlA2dpGetPcmBytesPerSbcFrame (SbcStreamInfo *streamInfo);
static U8 btlA2dpHandlePcmBlockFractions(BtlA2dpRawBlock *rawBlock, SbcPcmData pcmData[], U8 *nSbcFrames);
static BtStatus BtlA2dpDisableRoleSwitch();
static void btlA2dpSwitchToMaster(A2dpStream *Stream);
static void btlA2dpSendHciCommandCB(const BtEvent *Event);
static BtStatus btlA2dpSendHciCommand(U16 hciCommand, U8 parmsLen, U8 *parms);
static void btlA2dpEnableBasebandQos(A2dpStream *Stream);
#if BTL_A2DP_ALLOW_ROLE_SWITCH == BTL_CONFIG_ENABLED
#else
static void btlA2dpDisableRoleSwitchInLinkPolicies(A2dpStream *Stream);
#endif /* BTL_A2DP_ALLOW_ROLE_SWITCH */
static void BtlA2dpSendEventToBsc(BtlBscEventType eventType,
                                  BtlA2dpStream *btlStream,
                                  BtlA2dpStreamId streamId);
static U8 btlA2dpGetNumOfStreamingStreams(void);
static U32 btlA2dpCalcSbcPlayTime(BtlA2dpSbcPacket *sbcPacket);
#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0
static U32 btlA2dpCalcMp3PlayTime(BtlA2dpMpeg1_2_AudioPacket *mp3Packet);
#endif
static void btlA2dpCheckforStartSending(U32 addPlayTime);
static void btlA2dpHandleProcessQueue(U32 curTime, BOOL fromTimer);
static void btlA2dpHandleTimedProcessQueue();
static char * pAvError(A2dpError Error);
#if HCI_QOS == XA_ENABLED
static BOOL isSendingAllowed();
#endif /*  HCI_QOS == XA_ENABLED*/
static void BtlA2dpReportTransportChannelState(BtlA2dpStream *btlStream, BthalMmTransportChannelState state);

static void btlA2dpEmptyCurPacketQueue(void);

/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_A2DP_Init()
 */
BtStatus BTL_A2DP_Init(void)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BthalStatus bthalStatus;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_A2DP_Init");
	
	BTL_VERIFY_ERR((BTL_A2DP_INIT_STATE_NOT_INTIALIZED == btlA2dpInitState), 
		BT_STATUS_FAILED, ("A2DP module is already initialized"));
	
	btlA2dpInitState = BTL_A2DP_INIT_STATE_INITIALIZATION_FAILED;

	status = BTL_POOL_Create(&btlA2dpData.contextsPool,
							btlA2dpContextsPoolName,
							btlA2dpData.contextsMemory, 
							BTL_CONFIG_A2DP_MAX_NUM_OF_CONTEXTS,
							sizeof(BtlA2dpContext));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("A2Dp contexts pool creation failed"));
	
	InitializeListHead(&btlA2dpData.contextsList);

	bthalStatus = BTHAL_MM_Register(BtlMmCallback);

	BTL_VERIFY_FATAL((BTHAL_STATUS_SUCCESS == bthalStatus), BT_STATUS_INTERNAL_ERROR, 
		("BTHAL MM register failed"));

	/* Create A2DP task, semaphores and timer */
	BtlA2dpOsInit();
		
	/* Default codec type is SBC */
	btlA2dpData.currentCodecType = AVDTP_CODEC_TYPE_SBC;

#if SBC_ENCODER == XA_ENABLED

	btlA2dpData.processRawBlockFuncTable[BTHAL_MM_STREAM_TYPE_PCM] = btlA2dpProcessPcmBlock;

#endif /* SBC_ENCODER == XA_ENABLED */
	
	btlA2dpData.processRawBlockFuncTable[BTHAL_MM_STREAM_TYPE_SBC] = btlA2dpProcessSbcBlock;	

	btlA2dpData.processQueueFuncTable[AVDTP_CODEC_TYPE_SBC] = btlA2dpProcessSbcQueue;

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

	btlA2dpData.processRawBlockFuncTable[BTHAL_MM_STREAM_TYPE_MPEG1_2_AUDIO] = btlA2dpProcessMpeg1_2_AudioBlock;
	btlA2dpData.processQueueFuncTable[AVDTP_CODEC_TYPE_MPEG1_2_AUDIO] = btlA2dpProcessMpeg1_2_AudioQueue;

#endif

	btlA2dpData.a2dpContext = 0;
		
	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_A2DP, BtlA2dpBtlNotificationsCb);
		
	btlA2dpInitState = BTL_A2DP_INIT_STATE_INITIALIZED;	

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_A2DP_Deinit()
 */
BtStatus BTL_A2DP_Deinit(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_A2DP_Deinit");
	
	BTL_VERIFY_ERR((BTL_A2DP_INIT_STATE_INITIALIZED == btlA2dpInitState), 
		BT_STATUS_FAILED, ("A2DP module is not initialized"));

	btlA2dpInitState = BTL_A2DP_INIT_STATE_DEINITIALIZATION_FAILED;

	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_A2DP);

	BTL_VERIFY_ERR((IsListEmpty(&btlA2dpData.contextsList)), 
		BT_STATUS_FAILED, ("A2DP contexts are still active"));

	status = BTL_POOL_Destroy(&btlA2dpData.contextsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("A2DP contexts pool destruction failed"));

	/* Destroy A2DP task, semaphores and timer */
	BtlA2dpOsDeinit();
		
	btlA2dpInitState = BTL_A2DP_INIT_STATE_NOT_INTIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}

 
/*-------------------------------------------------------------------------------
 * BTL_A2DP_Create()
 */
BtStatus BTL_A2DP_Create(BtlAppHandle *appHandle,
							const BtlA2dpCallBack a2dpCallback,
							const BtSecurityLevel *securityLevel,
							BtlA2dpContext **a2dpContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 idx;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_A2DP_Create");

	BTL_VERIFY_ERR((0 != a2dpCallback), BT_STATUS_INVALID_PARM, ("Null a2dpCallback"));
	BTL_VERIFY_ERR((0 != a2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));

#if BT_SECURITY == XA_ENABLED

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid A2DP securityLevel"));
	}

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Allocate memory for a unique new A2DP context */
	status = BTL_POOL_Allocate(&btlA2dpData.contextsPool, (void **)a2dpContext);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating A2DP context"));

	status = BTL_HandleModuleInstanceCreation(appHandle, BTL_MODULE_TYPE_A2DP, &(*a2dpContext)->base);
	if (BT_STATUS_SUCCESS != status)
	{
		/* a2dpContext must be freed before we exit */
		BtStatus retVal = BTL_POOL_Free(&btlA2dpData.contextsPool, (void **)a2dpContext);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == retVal), ("Failed freeing A2DP context"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed handling A2DP instance creation"));
	}

	/* Save the given callback */
	(*a2dpContext)->callback = a2dpCallback;

	/* Add the new A2DP context to the active contexts list */
	InsertTailList(&btlA2dpData.contextsList, &((*a2dpContext)->base.node));

#if BT_SECURITY == XA_ENABLED

	/* Set the security record & save the given security level, or use default */
	(*a2dpContext)->securityRecord.id = SEC_L2CAP_ID;
	(*a2dpContext)->securityRecord.channel = BT_PSM_AVDTP;
	if (securityLevel == 0)
		(*a2dpContext)->securityRecord.level = BSL_A2DP_DEFAULT;
	else
		(*a2dpContext)->securityRecord.level = (*securityLevel);
	(*a2dpContext)->securityRecord.service= SDP_SERVICE_A2DP;

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Init A2DP streams state */
	for (idx = 0; idx<A2DP_MAX_NUM_STREAMS_PER_CONTEXT; idx++)
	{	
		(*a2dpContext)->streams[idx].state = BTL_A2DP_STREAM_STATE_NONE;

		(*a2dpContext)->streams[idx].context = (*a2dpContext);
	}

	btlA2dpData.pullDataState = BTL_A2DP_PULL_DATA_STATE_MASK_NONE;

	(*a2dpContext)->disableState = BTL_A2DP_DISABLE_STATE_MASK_NONE;

	/* Update current context */
	btlA2dpData.a2dpContext = *a2dpContext;

	/* Init A2DP context state */
	(*a2dpContext)->state = BTL_A2DP_STATE_DISABLED;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_A2DP_Destroy()
 */
BtStatus BTL_A2DP_Destroy(BtlA2dpContext **a2dpContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL isAllocated = FALSE;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_A2DP_Destroy");

	BTL_VERIFY_ERR((0 != a2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));
	BTL_VERIFY_ERR((0 != *a2dpContext), BT_STATUS_INVALID_PARM, ("Null *a2dpContext"));
	BTL_VERIFY_ERR((BTL_A2DP_STATE_DISABLED == (*a2dpContext)->state), BT_STATUS_IN_USE, ("A2DP context is in use"));

	status = BTL_POOL_IsElelementAllocated(&btlA2dpData.contextsPool, *a2dpContext, &isAllocated);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed locating given A2DP context"));
	BTL_VERIFY_ERR((TRUE == isAllocated), BT_STATUS_INVALID_PARM, ("Invalid A2DP context"));

	/* Remove the context from the list of all A2DP contexts */
	RemoveEntryList(&((*a2dpContext)->base.node));

	status = BTL_HandleModuleInstanceDestruction(&(*a2dpContext)->base);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed handling A2DP instance destruction"));

	status = BTL_POOL_Free(&btlA2dpData.contextsPool, (void **)a2dpContext);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed freeing A2DP context"));

	btlA2dpData.a2dpContext = 0;

	/* Set the A2DP context to NULL */
	*a2dpContext = 0;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_A2DP_Enable()
 */
BtStatus BTL_A2DP_Enable(BtlA2dpContext *A2dpContext,
							const BtlUtf8 *serviceName,
							const BtlA2dpSupportedFeaturesMask supportedFeatures)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BthalStatus bthalStatus;
	U8 idx, pktIdx;
	U16 len = 0;
	BtlA2dpStream *btlStream;
	BthalMmConfigParams codecInfo;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_A2DP_Enable");

	BTL_VERIFY_ERR((0 != A2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));
	BTL_VERIFY_ERR((BTL_A2DP_STATE_DISABLED == A2dpContext->state), BT_STATUS_FAILED, ("A2DP context is already enabled"));
		
	if ((serviceName != 0) && ((len = OS_StrLen((char *)serviceName)) != 0))
	{
		BTL_VERIFY_ERR((len <= A2DP_SERVICE_NAME_MAX_LEN), BT_STATUS_FAILED, ("A2DP service name is too long!"));
	}

	for (idx = 0; idx<A2DP_MAX_NUM_STREAMS_PER_CONTEXT; idx++)
	{	
		/* SBC streams must come before MP3 streams! */
		if (idx < BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT)
			codecInfo.streamType = BTHAL_MM_STREAM_TYPE_SBC;
		else
			codecInfo.streamType = BTHAL_MM_STREAM_TYPE_MPEG1_2_AUDIO;
		
		bthalStatus = BTHAL_MM_GetLocalCodecCapabilities(&codecInfo, &btlA2dpData.sbcEncoderLocation);
		BTL_VERIFY_FATAL((BTHAL_STATUS_SUCCESS == bthalStatus), BT_STATUS_FAILED, ("Failed to get local codec capabilities"));

		btlStream = &(A2dpContext->streams[idx]);

		btlStream->remoteCodecValid = FALSE;
		btlStream->stream.type = A2DP_STREAM_TYPE_SOURCE;
		btlStream->localCodec.elemLen = sizeof(btlStream->localElements);
		btlStream->localCodec.elements = btlStream->localElements;
		btlStream->btlA2dpReconfigurationIndNeeded = FALSE;

		BtlA2dpConvertBthal2Avdtp(&codecInfo, &btlStream->localCodec);

		BTL_LOG_INFO(("Local codec capabilities for stream %d:", idx));
		BtlA2dpPrintCodecInfo(&btlStream->localCodec);
		
		status = A2DP_Register(&(btlStream->stream), &(btlStream->localCodec), BtlA2dpCallback);
		if (status != BT_STATUS_SUCCESS)
		{
			/* Deregister all registered streams */
			BtlA2dpDeregisterAll(A2dpContext);

			/* This verify will fail and will handle the exception gracefully */
			BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed to register A2DP stream %d", idx));
		}

		btlStream->streamId = idx;
		
		btlStream->reconfigStat = BTL_A2DP_RECONFIG_STATE_MASK_NONE;
		
		btlStream->state = BTL_A2DP_STREAM_STATE_DISCONNECTED;

		btlStream->lastReportedState = BTHAL_MM_TRANSPORT_CHANNEL_STATE_CLOSED;
	}

	/* Update serviceName */
 	if (len > 0)
 	{
  		/* The first 2 bytes are for SDP_TEXT_8BIT */
		A2dpContext->serviceName[0] = DETD_TEXT + DESD_ADD_8BITS;
		A2dpContext->serviceName[1] = (U8)(len + 1);	/* Includes '\0' */

		OS_MemCopy((U8 *)(A2dpContext->serviceName + 2), (const U8 *)serviceName, len);
		A2dpContext->serviceName[(len + 2)] = '\0';
		
  		/* now write the values into a2dpContext.a2dpSrcSdpAttribute[]*/
        A2DP(a2dpSrcSdpAttribute)[4].value = A2dpContext->serviceName;
        A2DP(a2dpSrcSdpAttribute)[4].len = (U16)(len + 3);  
 	}
	
	/* Update supportedCategories */
	A2dpContext->supportedFeatures[0] = DETD_UINT + DESD_2BYTES;
	A2dpContext->supportedFeatures[1] = (U8)(((supportedFeatures) & 0xff00) >> 8);
	A2dpContext->supportedFeatures[2] = (U8)((supportedFeatures) & 0x00ff);

    A2DP(a2dpSrcSdpAttribute)[6].value = A2dpContext->supportedFeatures;
    A2DP(a2dpSrcSdpAttribute)[6].len = sizeof(A2dpContext->supportedFeatures);

#if BT_SECURITY == XA_ENABLED

	/* Register security record */
	if (BT_STATUS_SUCCESS != (status = SEC_Register(&A2dpContext->securityRecord)))
	{		
		/* Deregister all registered channels */
		BtlA2dpDeregisterAll(A2dpContext);

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed registering A2DP security record"));
	}
	
#endif	/* BT_SECURITY == XA_ENABLED */

	A2dpContext->freeRawBlockCounter = BTL_CONFIG_A2DP_MAX_NUM_RAW_BLOCKS_PER_CONTEXT;

	/* Init raw blocks pool */
	InitializeListHead(&A2dpContext->rawBlockPool);
	for (idx = 0; idx < BTL_CONFIG_A2DP_MAX_NUM_RAW_BLOCKS_PER_CONTEXT; idx++)
	{
        InsertTailList(&A2dpContext->rawBlockPool, &(A2dpContext->rawBlocks[idx].node));
	}

	/* Init raw blocks queue */
    InitializeListHead(&A2dpContext->rawBlockQueue);

	/* Init SBC packets pool */
	InitializeListHead(&A2dpContext->sbcPacketPool);
	for (idx = 0; idx < BTL_CONFIG_A2DP_MAX_NUM_SBC_PACKETS_PER_CONTEXT; idx++)
	{
		for (pktIdx = 0; pktIdx < BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT; pktIdx++)
		{
			/* Save the BtlA2dpSbcPacket containing this A2dpSbcPacket */
			A2dpContext->sbcPackets[idx].packets[pktIdx].userContext = (void *)(&(A2dpContext->sbcPackets[idx]));
		}
		
		A2dpContext->sbcPackets[idx].refCounter = 0;
#if HCI_QOS == XA_ENABLED
		A2dpContext->sbcPackets[idx].sbcData = &(A2dpContext->mediaData[idx][0]);
#endif /*  HCI_QOS == XA_ENABLED*/
		
        InsertTailList(&A2dpContext->sbcPacketPool, &(A2dpContext->sbcPackets[idx].node));
	}

	/* Init SBC packets queue */
    InitializeListHead(&A2dpContext->sbcPacketQueue);

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

	/* Init MP3 packets pool */
	InitializeListHead(&A2dpContext->mpeg1_2_audioPacketPool);
	for (idx = 0; idx < BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_PACKETS_PER_CONTEXT; idx++)
	{
		for (pktIdx = 0; pktIdx < BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT; pktIdx++)
		{
			/* Save the BtlA2dpMpeg1_2_AudioPacket containing this BtPacket */
			A2dpContext->mpeg1_2_audioPackets[idx].packets[pktIdx].userContext = (void *)(&(A2dpContext->mpeg1_2_audioPackets[idx]));
		}
		
		A2dpContext->mpeg1_2_audioPackets[idx].refCounter = 0;
#if HCI_QOS == XA_ENABLED
		A2dpContext->mpeg1_2_audioPackets[idx].mp3Data = &(A2dpContext->mediaData[idx][0]);
#endif /*  HCI_QOS == XA_ENABLED*/

        InsertTailList(&A2dpContext->mpeg1_2_audioPacketPool, &(A2dpContext->mpeg1_2_audioPackets[idx].node));
	}

	/* Init MP3 packets queue */
    InitializeListHead(&A2dpContext->mpeg1_2_audioPacketQueue);

	/* Init MP3 media header */
	btlA2dpData.a2dpContext->mpeg1_2_audioMediaHeader.version = 2;
	btlA2dpData.a2dpContext->mpeg1_2_audioMediaHeader.padding = 0;
	btlA2dpData.a2dpContext->mpeg1_2_audioMediaHeader.marker = 0;
	btlA2dpData.a2dpContext->mpeg1_2_audioMediaHeader.payloadType = 14;
	btlA2dpData.a2dpContext->mpeg1_2_audioMediaHeader.sequenceNumber = 0;
	btlA2dpData.a2dpContext->mpeg1_2_audioMediaHeader.timestamp = 0;
	btlA2dpData.a2dpContext->mpeg1_2_audioMediaHeader.ssrc = 1;
	btlA2dpData.a2dpContext->mpeg1_2_audioMediaHeader.csrcCount = 0;
	for (idx = 0; idx<15; idx++)
		btlA2dpData.a2dpContext->mpeg1_2_audioMediaHeader.csrcList[idx] = 0;

#endif

	A2dpContext->usedPacketsCounter = 0;

	A2dpContext->maxPacketSize = L2CAP_DEFAULT_MTU;

	btlA2dpData.numStreamingStreams = 0;

	/* a PATCH to set to preferred role to master. it set the global connection policies.
	    a better way is using connection manager  */
	/*BtlA2dpDisableRoleSwitch();*/

#if (BTL_CONFIG_A2DP_SAMPLE_RATE_CONVERTER == BTL_CONFIG_ENABLED)

	/* Input PCM data is not converted by default */
	btlA2dpData.srcConvertPCM = FALSE;

#endif	/* BTL_CONFIG_A2DP_SAMPLE_RATE_CONVERTER == BTL_CONFIG_ENABLED */

	/* A2DP state is now enabled */
	A2dpContext->state = BTL_A2DP_STATE_ENABLED;

	BTL_LOG_INFO(("Source is enabled with %d streams.", A2DP_MAX_NUM_STREAMS_PER_CONTEXT));

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status); 
}


/*-------------------------------------------------------------------------------
 * BTL_A2DP_Disable()
 */
BtStatus BTL_A2DP_Disable(BtlA2dpContext *a2dpContext)
{
	BtStatus status = BT_STATUS_PENDING;
	U32 idx;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_A2DP_Disable");

	BTL_VERIFY_ERR((0 != a2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));
	BTL_VERIFY_ERR((BTL_A2DP_STATE_DISABLED != a2dpContext->state), BT_STATUS_FAILED, ("A2DP context is already disabled!"));

#if	A2DP_USE_TIMER == XA_ENABLED
	BtlA2dpOsCancelTimer();
#endif	/* A2DP_USE_TIMER == XA_ENABLED */

	if (a2dpContext->state == BTL_A2DP_STATE_DISABLING)
	{
		a2dpContext->disableState |= BTL_A2DP_DISABLE_STATE_MASK_DISABLE_ACTIVE;

		BTL_RET(BT_STATUS_PENDING);
	}

	for (idx = 0; idx<A2DP_MAX_NUM_STREAMS_PER_CONTEXT; idx++)
	{
		switch (a2dpContext->streams[idx].state)
		{
			case (BTL_A2DP_STREAM_STATE_DISCONNECTING):
			{
				/* A2DP state is now in the process of disabling */
				a2dpContext->state = BTL_A2DP_STATE_DISABLING;
				break;
			}
			case (BTL_A2DP_STREAM_STATE_CONNECTED):
			{	
				status = A2DP_CloseStream(&(a2dpContext->streams[idx].stream));
				if (BT_STATUS_PENDING == status)
				{
					a2dpContext->streams[idx].state = BTL_A2DP_STREAM_STATE_DISCONNECTING;
					
					/* A2DP state is now in the process of disabling */
					a2dpContext->state = BTL_A2DP_STATE_DISABLING;
				}
				
				break;
			}
			case (BTL_A2DP_STREAM_STATE_CONNECTING):
			{
				/* In process of connecting, let it finish and then close the stream */

				/* A2DP state is now in the process of disabling */
				a2dpContext->state = BTL_A2DP_STATE_DISABLING;
				break;
			}
			case (BTL_A2DP_STREAM_STATE_CONNECT_IND):
			{
				/* Reject incoming open request, and wait for close event */

				status = A2DP_OpenStreamRsp(&(a2dpContext->streams[idx].stream), A2DP_ERR_BAD_SERVICE, AVDTP_SRV_CAT_MEDIA_TRANSPORT);
				BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), ("Failed responding to open indication while disabling."));

				if (BT_STATUS_PENDING == status)
					a2dpContext->state = BTL_A2DP_STATE_DISABLING;
				
				break;
			}
		}
	}

	if (BTL_A2DP_STATE_DISABLING != a2dpContext->state)
	{
		/* All streams are disconnected */
		status = BtlA2dpDisable(a2dpContext);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed disabling A2DP context"));
	}
	else
	{
		a2dpContext->disableState |= BTL_A2DP_DISABLE_STATE_MASK_DISABLE_ACTIVE;
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_A2DP_ConnectStream()
 */
BtStatus BTL_A2DP_ConnectStream(BtlA2dpContext *a2dpContext,
								BD_ADDR *bdAddr, 
								BtlA2dpCodec codec,
								BtlA2dpStreamId *streamId)
{
	BtStatus status = BT_STATUS_PENDING;
	U32 idx;
	BtlA2dpStream *btlStream = 0;
	
	BTL_FUNC_START_AND_LOCK("BTL_A2DP_ConnectStream");

	BTL_VERIFY_ERR((0 != a2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR(((BTL_A2DP_CODEC_SBC == codec) || (BTL_A2DP_CODEC_MPEG1_2_AUDIO == codec)), BT_STATUS_INVALID_PARM, ("Only SBC & MP3 are supported"));
	BTL_VERIFY_ERR((0 != streamId), BT_STATUS_INVALID_PARM, ("Null streamId"));

	for (idx = 0; idx<A2DP_MAX_NUM_STREAMS_PER_CONTEXT; idx++)
	{
		btlStream = &(a2dpContext->streams[idx]);
		
		if ((btlStream->state == BTL_A2DP_STREAM_STATE_DISCONNECTED) &&
			(btlStream->localCodec.codecType == codec))
		{
			/* Found a free stream of the requested codec type */
			break;
		}
	}

	BTL_VERIFY_ERR((idx != A2DP_MAX_NUM_STREAMS_PER_CONTEXT), BT_STATUS_IN_USE, ("No free A2DP stream"));

	status = A2DP_OpenStream(&(btlStream->stream), bdAddr);
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("Failed opening A2DP stream %d", idx));

	/* A2DP state is now in the process of connecting */
	btlStream->state = BTL_A2DP_STREAM_STATE_CONNECTING;

	/* Return streamId to app */
	*streamId = idx;
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_A2DP_OpenStream()
 */
BtStatus BTL_A2DP_OpenStream(BtlA2dpContext *a2dpContext,
							BtlA2dpStreamId streamId)
{
	BtStatus status = BT_STATUS_PENDING;
	AvdtpCodec codec;
	U8 elements[MAX_NUM_OF_BYTES_SPECIFIC_INFO_ELEMENTS];
	A2dpStreamState state;
	BtlA2dpStream *btlStream;

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0
	BOOL retVal;
#endif	/* BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0 */

	BTL_FUNC_START_AND_LOCK("BTL_A2DP_OpenStream");

	BTL_VERIFY_ERR((0 != a2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));
	BTL_VERIFY_ERR((streamId < A2DP_MAX_NUM_STREAMS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid streamId"));

	btlStream = &(a2dpContext->streams[streamId]);

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

	retVal = btlA2dpIsStreamOpenAllowed(btlStream);
	BTL_VERIFY_ERR((TRUE == retVal), BT_STATUS_NO_RESOURCES, ("Stream open is not allowed!"));

#endif	/* BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0 */

	state = A2DP_GetStreamState(&(btlStream->stream));
	BTL_VERIFY_ERR((state == A2DP_STREAM_STATE_IDLE), BT_STATUS_FAILED, ("Stream state is not idle"));

	codec.elemLen = sizeof(elements);
	codec.elements = elements;

	status = BtlA2dpSelectCodec(btlStream, &codec);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_FAILED, ("Failed to select codec for stream %d", streamId));

	BTL_LOG_INFO(("Source selected codec for stream %d:", streamId));
	BtlA2dpPrintCodecInfo(&codec);
	
	status = A2DP_SetStreamConfig(&(btlStream->stream), &codec, 0);
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("Failed setting config A2DP stream %d", streamId));
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_A2DP_OpenStreamResponse()
 */
BtStatus BTL_A2DP_OpenStreamResponse(BtlA2dpContext *a2dpContext,
									BtlA2dpStreamId streamId, 
									A2dpError error)
{
	BtStatus status = BT_STATUS_PENDING;
	
	BTL_FUNC_START_AND_LOCK("BTL_A2DP_OpenStreamResponse");

	BTL_VERIFY_ERR((0 != a2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));
	BTL_VERIFY_ERR((streamId < A2DP_MAX_NUM_STREAMS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid streamId"));

	status = A2DP_OpenStreamRsp(&(a2dpContext->streams[streamId].stream), error, AVDTP_SRV_CAT_MEDIA_TRANSPORT);
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("Failed responding to open indication %d", streamId));
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_A2DP_CloseStream()
 */
BtStatus BTL_A2DP_CloseStream(BtlA2dpContext *a2dpContext, BtlA2dpStreamId streamId)
{
	BtStatus status = BT_STATUS_PENDING;
    	A2dpDevice *device;
	
	BTL_FUNC_START_AND_LOCK("BTL_A2DP_CloseStream");

	BTL_VERIFY_ERR((0 != a2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));
	BTL_VERIFY_ERR((streamId < A2DP_MAX_NUM_STREAMS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid streamId"));

    	device = a2dpContext->streams[streamId].stream.device;

    	/* Check, that device is not NULL (maybe, it was already disconnected in racing condition)
     	 * and whether this is cancellation of creating connection */
   	if (device && (BTL_A2DP_STREAM_STATE_CONNECTING == a2dpContext->streams[streamId].state))
    	{
		/* If cancellation arrives during establishment of ACL connection,
		* cancel it immediately; if - during SDP query or protocol connection,
		* it will be cancelled after they are finished */
		if (A2DP_CCC_CONN_STATE_CONNECTING_ACL == device->cccConnState)
		{
			A2dpClose(device);
			status = BT_STATUS_SUCCESS;
		}
		else
		{
			device->cancelCreateConn = TRUE;
		}
	}
	else
	{
	status = A2DP_CloseStream(&(a2dpContext->streams[streamId].stream));
	}

	if (BT_STATUS_PENDING == status)
	{
	/* A2DP state is now in the process of closing */
	a2dpContext->streams[streamId].state = BTL_A2DP_STREAM_STATE_DISCONNECTING;
	}
    else
    {
	    a2dpContext->streams[streamId].state = BTL_A2DP_STREAM_STATE_DISCONNECTED;
    }

	BTL_VERIFY_ERR(((BT_STATUS_SUCCESS == status) || (BT_STATUS_PENDING == status)),
                   status,
                   ("Failed closing A2DP stream %d", streamId));
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_A2DP_IdleStream()
 */
BtStatus BTL_A2DP_IdleStream(BtlA2dpContext *a2dpContext, BtlA2dpStreamId streamId)
{
	BtStatus status = BT_STATUS_PENDING;
	
	BTL_FUNC_START_AND_LOCK("BTL_A2DP_IdleStream");

	BTL_VERIFY_ERR((0 != a2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));
	BTL_VERIFY_ERR((streamId < A2DP_MAX_NUM_STREAMS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid streamId"));

	status = A2DP_IdleStream(&(a2dpContext->streams[streamId].stream));
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("Failed idling A2DP stream %d", streamId));
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_A2DP_StartStream()
 */
BtStatus BTL_A2DP_StartStream(BtlA2dpContext *a2dpContext, BtlA2dpStreamId streamId)
{
	BtStatus status = BT_STATUS_PENDING;
	BtlA2dpStream *btlStream;
	
	BTL_FUNC_START_AND_LOCK("BTL_A2DP_StartStream");

	BTL_VERIFY_ERR((0 != a2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));
	BTL_VERIFY_ERR((streamId < A2DP_MAX_NUM_STREAMS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid streamId"));

	btlStream = &(a2dpContext->streams[streamId]);

#if HCI_QOS == XA_ENABLED
	btlStream ->numOfPacketsSent = 0;
#endif /* HCI_QOS == XA_ENABLED */

	/* Start the stream only if it isn't in the middle of reconfiguration */
	if ( (btlStream->reconfigStat & BTL_A2DP_RECONFIG_STATE_MASK_IN_RECONFIG) == 0 )
	{
		status = A2DP_StartStream(&(btlStream->stream));
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("Failed starting A2DP stream %d", streamId));
	}
	else
	{
		BTL_LOG_INFO(("BTL_A2DP_StartStream(stream=%d) during reconfig, deferring the Start", streamId));
		btlStream->reconfigStat |= BTL_A2DP_RECONFIG_STATE_MASK_PENDING_START;
		/* status is already BT_STATUS_PENDING */
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_A2DP_StartStreamResponse()
 */
BtStatus BTL_A2DP_StartStreamResponse(BtlA2dpContext *a2dpContext, BtlA2dpStreamId streamId, A2dpError error)
{
	BtStatus status = BT_STATUS_PENDING;
	
	BTL_FUNC_START_AND_LOCK("BTL_A2DP_StartStreamResponse");

	BTL_VERIFY_ERR((0 != a2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));
	BTL_VERIFY_ERR((streamId < A2DP_MAX_NUM_STREAMS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid streamId"));

	status = A2DP_StartStreamRsp(&(a2dpContext->streams[streamId].stream), error);
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("Failed responding to start A2DP stream %d", streamId));
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_A2DP_SuspendStream()
 */
BtStatus BTL_A2DP_SuspendStream(BtlA2dpContext *a2dpContext, BtlA2dpStreamId streamId)
{
	BtStatus status = BT_STATUS_PENDING;
	
	BTL_FUNC_START_AND_LOCK("BTL_A2DP_SuspendStream");

	BTL_VERIFY_ERR((0 != a2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));
	BTL_VERIFY_ERR((streamId < A2DP_MAX_NUM_STREAMS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid streamId"));

	status = A2DP_SuspendStream(&(a2dpContext->streams[streamId].stream));
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("Failed suspending A2DP stream %d", streamId));
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_A2DP_AbortStream()
 */
BtStatus BTL_A2DP_AbortStream(BtlA2dpContext *a2dpContext, BtlA2dpStreamId streamId)
{
	BtStatus status = BT_STATUS_PENDING;
	
	BTL_FUNC_START_AND_LOCK("BTL_A2DP_AbortStream");

	BTL_VERIFY_ERR((0 != a2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));
	BTL_VERIFY_ERR((streamId < A2DP_MAX_NUM_STREAMS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid streamId"));

	status = A2DP_AbortStream(&(a2dpContext->streams[streamId].stream));
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("Failed aborting A2DP stream %d", streamId));
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_A2DP_GetConnectedDevice()
 */
BtStatus BTL_A2DP_GetConnectedDevice(BtlA2dpContext *a2dpContext, 
										BtlA2dpStreamId streamId, 
										BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtRemoteDevice *remDev;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_A2DP_GetConnectedDevice");

	BTL_VERIFY_ERR((0 != a2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((streamId < A2DP_MAX_NUM_STREAMS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid streamId"));
	BTL_VERIFY_ERR((BTL_A2DP_STREAM_STATE_CONNECTED == a2dpContext->streams[streamId].state), BT_STATUS_NO_CONNECTION, ("A2DP stream %d is not in the connected state", streamId));

	remDev = A2DP_GetRemoteDevice(&(a2dpContext->streams[streamId].stream));
	BTL_VERIFY_FATAL((0 != remDev), BT_STATUS_INTERNAL_ERROR, ("Unable to find connected remote device"));

	OS_MemCopy((U8 *)bdAddr, (const U8 *)(&(remDev->bdAddr)), sizeof(BD_ADDR));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_A2DP_SetSecurityLevel()
 */
BtStatus BTL_A2DP_SetSecurityLevel(BtlA2dpContext *a2dpContext,
								const BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_A2DP_SetSecurityLevel");

	BTL_VERIFY_ERR((0 != a2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid A2DP securityLevel"));
	}
	
	/* Apply the new security level */
	if (securityLevel == 0)
		a2dpContext->securityRecord.level = BSL_A2DP_DEFAULT;
	else
		a2dpContext->securityRecord.level = (*securityLevel);

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_A2DP_GetSecurityLevel()
 */
BtStatus BTL_A2DP_GetSecurityLevel(BtlA2dpContext *a2dpContext,
									BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_A2DP_GetSecurityLevel");

	BTL_VERIFY_ERR((0 != a2dpContext), BT_STATUS_INVALID_PARM, ("Null a2dpContext"));
	BTL_VERIFY_ERR((0 != securityLevel), BT_STATUS_INVALID_PARM, ("Null securityLevel"));

	*securityLevel = a2dpContext->securityRecord.level;
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
#endif	/* BT_SECURITY == XA_ENABLED */


/*-------------------------------------------------------------------------------
 * BtlA2dpDisable()
 *
 *		Internal function for deregistering security record and A2DP stream.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		a2dpContext [in] - pointer to the A2DP context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
static BtStatus BtlA2dpDisable(BtlA2dpContext *a2dpContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlA2dpDisable");

	/* First, try to deregister all channels */
	status = BtlA2dpDeregisterAll(a2dpContext);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_FAILED, ("Failed deregistering all A2DP streams"));

#if BT_SECURITY == XA_ENABLED

	/* Second, try to unregister security record */
	status = SEC_Unregister(&a2dpContext->securityRecord);
	BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("Failed unregistering A2DP security record"));
			
#endif	/* BT_SECURITY == XA_ENABLED */
	
	a2dpContext->state = BTL_A2DP_STATE_DISABLED;

	BTL_LOG_INFO(("Source is disabled."));

	BTL_FUNC_END();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BtlA2dpDeregisterAll()
 *
 *		Internal function for deregistering all A2DP streams.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		a2dpContext [in] - pointer to the A2DP context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
static BtStatus BtlA2dpDeregisterAll(BtlA2dpContext *a2dpContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 idx;

	BTL_FUNC_START("BtlA2dpDeregisterAll");

	/* Try to deregister all streams */
	for (idx = 0; idx<A2DP_MAX_NUM_STREAMS_PER_CONTEXT; idx++)
	{
		status = A2DP_Deregister(&(a2dpContext->streams[idx].stream));

		if (status == BT_STATUS_SUCCESS)
		{
			a2dpContext->streams[idx].state = BTL_A2DP_STREAM_STATE_NONE;	
		}
		else
		{
			BTL_LOG_ERROR(("Failed deregistering A2DP stream %d", idx));
			status = BT_STATUS_FAILED;
		}
	}

	BTL_FUNC_END();

	return (status);
}


/*-------------------------------------------------------------------------------
 * btlA2dpEmptyCurPacketQueue()
 *
 *		empty the curent packet queue and return the packets to the pakcet pool.
 *           will be performed only when there are no streaming streams left.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		none.
 *
 * Returns:
 *		void.
 */
static void btlA2dpEmptyCurPacketQueue()
{
	if (btlA2dpGetNumOfStreamingStreams() != 0)
		return;

	if ( btlA2dpData.currentCodecType != BTHAL_MM_STREAM_TYPE_MPEG1_2_AUDIO)
	{
		BtlA2dpSbcPacket *sbcPacket;
		/* empty the sbcPacketQueue */
		while ((sbcPacket = btlA2dpGetSbcPacketFromQueue()) != 0)
		{
			btlA2dpInsertSbcPacketToPool(sbcPacket);
		}
	}
	else
	{
		BtlA2dpMpeg1_2_AudioPacket *mp3Packet ;
		/* empty the mp3PacketQueue */
		while ((mp3Packet = btlA2dpGetMpeg1_2_AudioPacketFromQueue()) != 0)	
		{
			btlA2dpInsertMpeg1_2_AudioPacketToPool(mp3Packet);
		}
	}
}

/*-------------------------------------------------------------------------------
 * BtlA2dpCallback()
 *
 *		Internal callback for handling A2DP events in BTL layer.
 *		This function forwards events to registered application callback.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Stream [in] - Internal A2DP stream.
 *
 *		Info [in] - Internal A2DP event.
 *
 * Returns:
 *		void.
 */
static void BtlA2dpCallback(A2dpStream *Stream, A2dpCallbackParms *Info)
{
	BtStatus status = BT_STATUS_SUCCESS;
	A2dpError error = A2DP_ERR_NO_ERROR;
	BOOL passEventToApp = TRUE;
	BtlA2dpContext *a2dpContext;
	BtlA2dpStream *btlStream;
	U32 idx;
	BOOL sendDisableEvent = FALSE;
	BOOL notifyRadioOffComplete = FALSE;
	BtlA2dpSbcPacket *sbcPacket;
#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0
	BtlA2dpMpeg1_2_AudioPacket *mp3Packet;
#endif
	BOOL sendConfigEvent = FALSE;
	BthalMmConfigParams codecInfo;
	BthalStatus bthalStatus;
	BOOL cancelTimer = FALSE;

	/* Find BTL A2DP stream according to given A2DP stream */
	btlStream = ContainingRecord(Stream, BtlA2dpStream, stream);

	/* Find context according to BTL A2DP stream */
	a2dpContext = btlStream->context;

	/* Find stream index in the context */
	idx = btlStream->streamId;

	/* First handle 'packet sent' events */
		switch (Info->event)
		{
			case (A2DP_EVENT_STREAM_SBC_PACKET_SENT):
			{				
				/* SBC packet was sent */

				/* Find BTL A2DP packet according to given A2DP packet */
				sbcPacket = (BtlA2dpSbcPacket *)(Info->p.sbcPacket->userContext);

				sbcPacket->refCounter--;

#if HCI_QOS == XA_ENABLED
				btlStream->numOfPacketsSent--; 
#endif /* HCI_QOS == XA_ENABLED */

				if (sbcPacket->refCounter == 0)
				{
					/* Sending to all streams is finished, return packet to pool */
					btlA2dpInsertSbcPacketToPool(sbcPacket);

#if HCI_QOS == XA_ENABLED
#else
					if ( BTHAL_MM_SBC_ENCODER_EXTERNAL == btlA2dpData.sbcEncoderLocation )
					{
					bthalStatus = BTHAL_MM_FreeDataBuf(sbcPacket->descriptor); 
						BTL_VERIFY_ERR_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed to free data buffer"));
					}
#endif /*  HCI_QOS == XA_ENABLED*/
				

				if (btlA2dpData.pullDataState & BTL_A2DP_PULL_DATA_STATE_MASK_PULL_ACTIVE)
				{
					/* Send 'data sent' event to the A2DP task in case of data indication */
					bthalStatus = BTHAL_OS_SendEvent(BTHAL_OS_TASK_HANDLE_A2DP, A2DP_OS_EVENT_DATA_SENT);
					BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed sending data sent event to A2DP task!"));
				}
				}

			/* Do not pass event to app, and return immediately */
			return;
			}

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

			case (A2DP_EVENT_STREAM_PACKET_SENT):
			{
				/* Find BTL A2DP packet according to given A2DP packet */
				mp3Packet = (BtlA2dpMpeg1_2_AudioPacket *)(Info->p.btPacket->userContext);

				mp3Packet->refCounter--;

#if HCI_QOS == XA_ENABLED
				btlStream->numOfPacketsSent--; 
#endif /* HCI_QOS == XA_ENABLED */

				if (mp3Packet->refCounter == 0)
				{
					/* Sending to all streams is finished, return packet to pool */
					btlA2dpInsertMpeg1_2_AudioPacketToPool(mp3Packet);

#if HCI_QOS == XA_ENABLED
#else
					bthalStatus = BTHAL_MM_FreeDataBuf(mp3Packet->descriptor); 
				BTL_VERIFY_ERR_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed to free data buffer"));
#endif /* HCI_QOS == XA_ENABLED */

				if (btlA2dpData.pullDataState & BTL_A2DP_PULL_DATA_STATE_MASK_PULL_ACTIVE)
				{
					/* Send 'data sent' event to the A2DP task in case of data indication */
					bthalStatus = BTHAL_OS_SendEvent(BTHAL_OS_TASK_HANDLE_A2DP, A2DP_OS_EVENT_DATA_SENT);
					BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed sending data sent event to A2DP task!"));
				}
			}

			/* Do not pass event to app, and return immediately */
			return;
				}

#endif

	}

	/* Then handle special case of disabling */
	if (a2dpContext->state == BTL_A2DP_STATE_DISABLING)
	{
		switch (Info->event)
		{
			case (A2DP_EVENT_STREAM_CLOSED):
			{
				U32 i;

				btlStream->state = BTL_A2DP_STREAM_STATE_DISCONNECTED;
				btlStream->remoteCodecValid = FALSE;
				BTL_LOG_INFO(("CLOSED event arrived on stream %d with error code %s.", idx, pAvError(Info->error)));
#if HCI_QOS == XA_ENABLED
					HCI_RemoveGuaranteedChannel(btlStream->devIndex);
#endif /* HCI_QOS == XA_ENABLED */

				/* Check that all streams are disconnected */
				for (i=0; i<A2DP_MAX_NUM_STREAMS_PER_CONTEXT; i++)
				{
					if (a2dpContext->streams[i].state != BTL_A2DP_STREAM_STATE_DISCONNECTED)
						break;
				}
				
				if (i == A2DP_MAX_NUM_STREAMS_PER_CONTEXT)
				{
					/* All streams are disconnected */
					
					if (a2dpContext->disableState & BTL_A2DP_DISABLE_STATE_MASK_DISABLE_ACTIVE)
					{				
						BtlA2dpDisable(a2dpContext);
						sendDisableEvent = TRUE;
					}
					else if (a2dpContext->disableState & BTL_A2DP_DISABLE_STATE_MASK_ENABLED)
					{
						a2dpContext->state = BTL_A2DP_STATE_ENABLED;
					}

					if (a2dpContext->disableState & BTL_A2DP_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE)
					{		
						notifyRadioOffComplete = TRUE;
					}

					a2dpContext->disableState = BTL_A2DP_DISABLE_STATE_MASK_NONE;
				}

				/* Update BTHAL_MM module with the new state of the transport channel */
				BtlA2dpReportTransportChannelState(btlStream, BTHAL_MM_TRANSPORT_CHANNEL_STATE_CLOSED);

				/* Pass event to app. */
				break;
			}
			
			case (A2DP_EVENT_STREAM_OPEN_IND):
			{
				/* Stream open request has been received */
				BTL_LOG_INFO(("OPEN_IND event arrived on stream %d with error code %s.", idx, pAvError(Info->error)));
				BTL_LOG_INFO(("Stream is in disabling state, rejecting request..."));

				status = A2DP_OpenStreamRsp(Stream, A2DP_ERR_BAD_SERVICE, AVDTP_SRV_CAT_MEDIA_TRANSPORT);
				BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), ("Failed responding to open indication while disabling."));

				/* Do not pass event to app, since we already rejected the incoming open indication. */
				passEventToApp = FALSE;
				break;
				}

			case (A2DP_EVENT_GET_CONFIG_IND):
			case (A2DP_EVENT_STREAM_OPEN):
			{
				/* Stream is open */
				BTL_LOG_INFO(("OPEN / CONFIG_IND event arrived on stream %d with error code %s.", idx, pAvError(Info->error)));
				BTL_LOG_INFO(("Stream is in disabling state, closing stream..."));

				status = A2DP_CloseStream(Stream);
				BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), ("Failed closing stream %d while disabling.", idx));

				if (BT_STATUS_PENDING == status)
					btlStream->state = BTL_A2DP_STREAM_STATE_DISCONNECTING;

				/* Do not pass event to app, since we are closing the stream... */
				passEventToApp = FALSE;
				break;
			}
#if HCI_QOS == XA_ENABLED
			case (A2DP_EVENT_STREAM_ABORTED):
			case (A2DP_EVENT_STREAM_IDLE):
			case (A2DP_EVENT_STREAM_SUSPENDED):
			{
				BTL_LOG_INFO(("ABORTED/IDLE/SUSPENDED event (%d) arrived on stream %d with error code %s.", Info->event, idx, pAvError(Info->error)));
				HCI_RemoveGuaranteedChannel(btlStream->devIndex);
				break;
			}
#endif /* HCI_QOS == XA_ENABLED */
			
			default:
			{
				BTL_LOG_ERROR(("Received unexpected event %d while disabling!", Info->event));	

				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;
			}
		}
	}
	else
	{
		switch (Info->event)
		{
			case (A2DP_EVENT_STREAM_OPEN_IND):
			{
				/* Stream open request has been received */
				BTL_LOG_INFO(("OPEN IND event arrived on stream %d with error code %s.", idx, pAvError(Info->error)));

				if(Info->error == A2DP_ERR_NO_ERROR)
				{
					BTL_LOG_INFO(("requested codec:"));
				BtlA2dpPrintCodecInfo(&(Info->p.configReq->codec));

				error = BtlA2dpCheckCodecCompatibility(&(btlStream->localCodec), &(Info->p.configReq->codec));

				if (error != A2DP_ERR_NO_ERROR)
				{
					status = A2DP_OpenStreamRsp(Stream, error, AVDTP_SRV_CAT_MEDIA_CODEC);
					BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), ("Failed responding to open indication"));

					/* Do not pass event to app, since we already rejected the incoming open indication. */
					passEventToApp = FALSE;
				}
				
#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

				else if (btlA2dpIsStreamOpenAllowed(btlStream) == FALSE)
				{
					status = A2DP_OpenStreamRsp(Stream, A2DP_ERR_INSUFFICIENT_RESOURCE, AVDTP_SRV_CAT_MEDIA_TRANSPORT);
					BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), ("Failed responding to open indication"));
	
					/* Do not pass event to app, since we already rejected the incoming open indication. */
					passEventToApp = FALSE;
				}

#endif	/* BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0 */

				else
				{
					btlStream->state = BTL_A2DP_STREAM_STATE_CONNECT_IND;
				}
				}
				
				break;
			}
			
			case (A2DP_EVENT_STREAM_OPEN):
			{
				BTL_LOG_INFO(("OPEN event arrived on stream %d with error code %s.", idx, pAvError(Info->error)));

				if(Info->error == A2DP_ERR_NO_ERROR)
				{
					/* Stream is open */
				/* Assumption: Only one open codec type! */
				btlA2dpData.currentCodecType = btlStream->localCodec.codecType;

				/* handle QoS in various methods: */
#if BTL_A2DP_ALLOW_ROLE_SWITCH == BTL_CONFIG_ENABLED

#else
				BTL_LOG_INFO(("Role switch is not allowed!"));
				btlA2dpDisableRoleSwitchInLinkPolicies(&(btlStream->stream));
#endif /* BTL_A2DP_ALLOW_ROLE_SWITCH */

				btlA2dpSwitchToMaster(&(btlStream->stream));
				btlA2dpEnableBasebandQos(&(btlStream->stream));

				btlStream->state = BTL_A2DP_STREAM_STATE_CONNECTED;
					btlStream->reconfigStat = BTL_A2DP_RECONFIG_STATE_MASK_NONE;
				
				BTL_LOG_INFO(("Source stream %d is opened, configured codec:", idx));
				BtlA2dpPrintCodecInfo(&(Info->p.configReq->codec));

				/* Update max media packet size, since a new stream is connected */
				btlA2dpUpdateMaxMediaPacketSize();

				/* New CODEC configuration might be needed */
				sendConfigEvent = TRUE;

					/* Update BTHAL_MM module with the new state of the transport channel */
					BtlA2dpReportTransportChannelState(btlStream, BTHAL_MM_TRANSPORT_CHANNEL_STATE_OPEN);
				}
				
				break;
			}
			
			case (A2DP_EVENT_CODEC_INFO):
			{	
				/* Received remote codec capabilities */
				BTL_LOG_INFO(("Received remote codec capabilities on stream %d with error code %s.", idx, pAvError(Info->error)));

				if(Info->error == A2DP_ERR_NO_ERROR)
				{
				BtlA2dpPrintCodecInfo(Info->p.codec);

				/* Save remote codec capabilities */
				btlStream->remoteCodec.codecType = Info->p.codec->codecType;
				btlStream->remoteCodec.elements = btlStream->remoteElements;
				OS_MemCopy((U8 *)btlStream->remoteElements, (U8 *)Info->p.codec->elements, Info->p.codec->elemLen);

				btlStream->remoteCodecValid = TRUE;

				/* Translate remote device codec capabilities into BTHAL MM codec struct */
				BtlA2dpConvertAvdtp2Bthal(Info->p.codec, &codecInfo);
					
				/* Update BTHAL MM with the remote device codec capabilities.*/
				bthalStatus = BTHAL_MM_RemoteCodecCapabilitiesInd(&codecInfo, idx);
				BTL_VERIFY_ERR_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed to update BTHAL MM with remote device capabilities"));
				}
				
				/* Do not pass event to app. */
				passEventToApp = FALSE;
				
				break;
			}
			
			case (A2DP_EVENT_CP_INFO):
			{
				/* Received remote contect protection capabilities */
				BTL_LOG_INFO(("Received remote contect protection capabilities on stream %d with error code %s.", idx, pAvError(Info->error)));

				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;
			}
			case (A2DP_EVENT_GET_CONFIG_IND):
			{
				/* Received get config indication during outgoing connection, update application */
				BTL_LOG_INFO(("GET CONFIG IND event arrived on stream %d with error code %s.", idx, pAvError(Info->error)));

				if(Info->error == A2DP_ERR_NO_ERROR)
				btlStream->state = BTL_A2DP_STREAM_STATE_CONNECTED;
				
				break;
			}
			
			case (A2DP_EVENT_STREAM_CLOSED):
			{
				/* Stream has been closed */
				BTL_LOG_INFO(("CLOSED event  arrived on stream %d with error code %s.", idx, pAvError(Info->error)));

				if(Info->error == A2DP_ERR_NO_ERROR)
				{
				btlStream->state = BTL_A2DP_STREAM_STATE_DISCONNECTED;
				btlStream->remoteCodecValid = FALSE;

				/* Update max media packet size, since a stream is disconnected */
				btlA2dpUpdateMaxMediaPacketSize();

				/* New CODEC configuration might be needed, since a stream is disconnected */
				sendConfigEvent = TRUE;
				cancelTimer = TRUE;

				BTL_LOG_INFO(("Source stream %d is closed, error %d.", idx, Info->error));
#if HCI_QOS == XA_ENABLED
					HCI_RemoveGuaranteedChannel(btlStream->devIndex);
#endif /* HCI_QOS == XA_ENABLED */

					/* Update BTHAL_MM module with the new state of the transport channel */
					BtlA2dpReportTransportChannelState(btlStream, BTHAL_MM_TRANSPORT_CHANNEL_STATE_CLOSED);

					btlStream->reconfigStat = BTL_A2DP_RECONFIG_STATE_MASK_NONE;

					btlA2dpEmptyCurPacketQueue();

					btlStream->btlA2dpReconfigurationIndNeeded = FALSE;
				}

				break;
			}
			
			case (A2DP_EVENT_STREAM_ABORTED):
			{
				BTL_LOG_INFO(("ABORTED event  arrived on stream %d with error code %s.", idx, pAvError(Info->error)));
				
				if(Info->error == A2DP_ERR_NO_ERROR)
				{
					/* Stream has been aborted */
				/* Update max media packet size, since a stream is aborted */
				btlA2dpUpdateMaxMediaPacketSize();

				BTL_LOG_INFO(("Source stream %d is aborted.", idx));
				cancelTimer = TRUE;

#if HCI_QOS == XA_ENABLED
					HCI_RemoveGuaranteedChannel(btlStream->devIndex);
#endif /* HCI_QOS == XA_ENABLED */

					/* Update BTHAL_MM module with the new state of the transport channel */
					BtlA2dpReportTransportChannelState(btlStream, BTHAL_MM_TRANSPORT_CHANNEL_STATE_CLOSED);
					btlStream->reconfigStat = BTL_A2DP_RECONFIG_STATE_MASK_NONE;
					btlA2dpEmptyCurPacketQueue();
				}

				break;
			}

			case (A2DP_EVENT_STREAM_IDLE):
			{
				BTL_LOG_INFO(("IDLE event  arrived on stream %d with error code %s.", idx, pAvError(Info->error)));

				if(Info->error == A2DP_ERR_NO_ERROR)
				{		
				
				/* Update max media packet size, since a stream is idle */
				btlA2dpUpdateMaxMediaPacketSize();
				
				BTL_LOG_INFO(("Source stream %d is idle.", idx));
				cancelTimer = TRUE;

#if HCI_QOS == XA_ENABLED
					HCI_RemoveGuaranteedChannel(btlStream->devIndex);
#endif /* HCI_QOS == XA_ENABLED */

					/* Update BTHAL_MM module with the new state of the transport channel */
					BtlA2dpReportTransportChannelState(btlStream, BTHAL_MM_TRANSPORT_CHANNEL_STATE_CLOSED);
					btlStream->reconfigStat = BTL_A2DP_RECONFIG_STATE_MASK_NONE;
					
					btlA2dpEmptyCurPacketQueue();
				}

				break;
			}

			case (A2DP_EVENT_GET_CAP_CNF):
			{	
				/* Confirm receiving of remote device capabilities */
				BTL_LOG_INFO(("GET CAP CNF event  arrived on stream %d with error code %s.", idx, pAvError(Info->error)));

				if(Info->error == A2DP_ERR_NO_ERROR)
				{
				/* New CODEC configuration might be needed */
				sendConfigEvent = TRUE;
				}
				
				/* Do not pass event to app. */
				passEventToApp = FALSE;
				
				break;
			}

			case (A2DP_EVENT_STREAM_START_IND):
			{
				/* Remote device initiated start stream */
				BTL_LOG_INFO(("START IND event arrived on stream %d with error code %s.", idx, pAvError(Info->error)));
				
				break;
			}

			case (A2DP_EVENT_STREAM_STARTED):
			{
				BTL_LOG_INFO(("STARTED event arrived on stream %d with error code %s.", idx, pAvError(Info->error)));

				if(Info->error == A2DP_ERR_NO_ERROR)
				{
#if SBC_ENCODER == XA_ENABLED
					if ( (BTHAL_MM_SBC_ENCODER_BUILT_IN == btlA2dpData.sbcEncoderLocation) 
												&& (btlA2dpGetNumOfStreamingStreams() == 1) )
				{
				btlA2dpData.lastBpUpd = OS_GetSystemTime();
						btlA2dpData.decBpOccurrencesCounter = 0;
				btlA2dpData.numRemainingPcmBytes = 0;
				}
#endif /* SBC_ENCODER == XA_ENABLED */

	 				BTL_LOG_INFO(("Source stream %d was started.", idx));

#if HCI_QOS == XA_ENABLED
					/* Notify HCI layer that a guaranteed streaming channel was added */
					btlStream->devIndex = HCI_AddGuaranteedChannel(&(btlStream->stream.stream.conn.remDev->bdAddr));
#endif /* HCI_QOS == XA_ENABLED */

				if (btlA2dpData.pullDataState & BTL_A2DP_PULL_DATA_STATE_MASK_PULL_ON_HOLD)
				{
					btlA2dpData.pullDataState &= (~BTL_A2DP_PULL_DATA_STATE_MASK_PULL_ON_HOLD);
					
					/* Send 'data sent' event to the A2DP task in case of stream started */
					bthalStatus = BTHAL_OS_SendEvent(BTHAL_OS_TASK_HANDLE_A2DP, A2DP_OS_EVENT_DATA_SENT);
					BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed sending data sent event to A2DP task!"));
				}

				btlA2dpSwitchToMaster(&(btlStream->stream));

				/*  in case we transitioned from zero to one streaming stream: */
				if (btlA2dpGetNumOfStreamingStreams() == 1)
				{
#if	A2DP_USE_TIMER == XA_ENABLED
					btlA2dpData.accumulatedMediaTime = 0; /* from now on counting how we accumulated before started sending */
					btlA2dpData.canSendFromQueue = FALSE;
					btlA2dpData.numPacketsNeedToBeSent = 0;
#endif	/* A2DP_USE_TIMER == XA_ENABLED */
				}

					/* Update BTHAL_MM module with the new state of the transport channel */
					BtlA2dpReportTransportChannelState(btlStream, BTHAL_MM_TRANSPORT_CHANNEL_STATE_STREAMING);
				}

				break;
			}

			case (A2DP_EVENT_STREAM_SUSPENDED):
			{
				BTL_LOG_INFO(("SUSPENDED event arrived on stream %d with error code %s.", idx, pAvError(Info->error)));

				if(Info->error == A2DP_ERR_NO_ERROR)
				{
				btlA2dpGetNumOfStreamingStreams();
#if	A2DP_USE_TIMER == XA_ENABLED
				if ((btlA2dpData.pullDataState & BTL_A2DP_PULL_DATA_STATE_MASK_PULL_ACTIVE) &&
					(btlA2dpData.numStreamingStreams == 0))
				{
					btlA2dpData.pullDataState |= BTL_A2DP_PULL_DATA_STATE_MASK_PULL_ON_HOLD;
				}
#endif	/* A2DP_USE_TIMER == XA_ENABLED */

				/* New CODEC configuration might be needed */
				sendConfigEvent = TRUE;

#if HCI_QOS == XA_ENABLED
					/* Notify HCI layer that a guaranteed streaming channel was disconnected */
					HCI_RemoveGuaranteedChannel(btlStream->devIndex);
#endif /* HCI_QOS == XA_ENABLED */

					/* Update BTHAL_MM module with the new state of the transport channel */
					BtlA2dpReportTransportChannelState(btlStream, BTHAL_MM_TRANSPORT_CHANNEL_STATE_OPEN);
				}
				
				break;
			}

			case (A2DP_EVENT_STREAM_RECONFIG_IND):
			{
				/* Stream reconfig request has been received */
				BTL_LOG_INFO(("RECONFIG IND event arrived on stream %d with error code %s.", idx, pAvError(Info->error)));

				if(Info->error == A2DP_ERR_NO_ERROR)
				{
					BTL_LOG_INFO(("requested codec:", idx));
				BtlA2dpPrintCodecInfo(&(Info->p.configReq->codec));

				error = BtlA2dpCheckCodecCompatibility(&(btlStream->localCodec), &(Info->p.configReq->codec));

				status = A2DP_ReconfigStreamRsp(&(btlStream->stream), error, AVDTP_SRV_CAT_MEDIA_CODEC);
				BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), ("Failed responding to reconfig indication"));
					if (status == BT_STATUS_PENDING)
					{
						btlStream->reconfigStat |= BTL_A2DP_RECONFIG_STATE_MASK_IN_RECONFIG;
					}
				}

				/* Do not pass event to app. */
				passEventToApp = FALSE;
				
				break;
			}

			case (A2DP_EVENT_STREAM_RECONFIG_CNF):
			{
				BTL_LOG_INFO(("RECONFIG CNF event arrived on stream %d with error code %s.", idx, pAvError(Info->error)));

				if (Info->error == A2DP_ERR_NO_ERROR)
				{
					BTL_LOG_INFO(("Configured codec for stream %d:", idx));
					BtlA2dpPrintCodecInfo(&(Info->p.configReq->codec));

					if ((btlStream->reconfigStat & BTL_A2DP_RECONFIG_STATE_MASK_PENDING_START) != 0)
					{
						BTL_LOG_DEBUG(("calling A2DP_StartStream because of RECONFIG_STATE_MASK_PENDING_START"));
						status = A2DP_StartStream(&(btlStream->stream));
						BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), ("Failed starting A2DP stream %d at end of reconfig", btlStream->streamId));
						/* ^^Uzi in case did not return BT_STATUS_PENDING, need to make callback to app to notify about failure to Start stream ??? */
					}
					btlStream->reconfigStat = BTL_A2DP_RECONFIG_STATE_MASK_NONE;
					
					/* New CODEC configuration might be needed */
					sendConfigEvent = TRUE;
				}
				else
				{
					BTL_LOG_INFO(("Reconfig failed! error %d, failed capability %d", Info->error, Info->p.capability->type));
				}

				/* Do not pass event to app. */
				passEventToApp = FALSE;
				
				break;
			}

			case (A2DP_EVENT_STREAM_SECURITY_IND):
			{
				/* Remote device initiated security process */
				BTL_LOG_INFO(("SECURITY IND event arrived on stream %d with error code %s.", idx, pAvError(Info->error)));
				BTL_LOG_INFO(("Remote device initiated security process"));

				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;
			}

			case (A2DP_EVENT_STREAM_SECURITY_CNF):
			{
				/* Remote device responds to the security process request */
				BTL_LOG_INFO(("SECURITY CNF event arrived on stream %d with error code %s.", idx, pAvError(Info->error)));
				BTL_LOG_INFO(("Remote device responds to the security process request"));

				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;
			}

			default:
				BTL_LOG_ERROR(("Received unexpected event %d!", Info->event));
				break;
		}	
	}

	if (cancelTimer && (btlA2dpGetNumOfStreamingStreams() == 0))
	{
#if	A2DP_USE_TIMER == XA_ENABLED
		BtlA2dpOsCancelTimer();
		btlA2dpData.accumulatedMediaTime = 0;
		btlA2dpData.canSendFromQueue = FALSE;
		btlA2dpData.numPacketsNeedToBeSent = 0;
#endif	/* A2DP_USE_TIMER == XA_ENABLED */
	}

	if (TRUE == passEventToApp)
	{
        BtlBscEventType eventType = 0;
        BOOL sendEventToBsc = TRUE;

		/* Set the context in the event passed to app */
		btlA2dpData.event.a2dpContext = a2dpContext;

		/* Set the channel index in the event passed to app */
		btlA2dpData.event.streamId = idx;

		/* Set the internal event in the event passed to app */
		btlA2dpData.event.callbackParms = Info;

        /* Pass events to the BSC module */
        switch(Info->event)
        {
			case (A2DP_EVENT_STREAM_CLOSED):
			case (A2DP_EVENT_STREAM_IDLE):
			case (A2DP_EVENT_STREAM_ABORTED):
                /* The stream may have been stopped - pass event to the BSC module */
                BtlA2dpSendEventToBsc(BTL_BSC_EVENT_AUDIO_STREAMING_STOPPED, NULL, idx);

                /* Prepare the 2nd event for sending to BSC */
                eventType = BTL_BSC_EVENT_SLC_DISCONNECTED;
                break;

			case (A2DP_EVENT_STREAM_OPEN):
                eventType = BTL_BSC_EVENT_SLC_CONNECTED;
                break;

			case (A2DP_EVENT_STREAM_STARTED):
                eventType = BTL_BSC_EVENT_AUDIO_STREAMING_STARTED;
                break;

			case (A2DP_EVENT_STREAM_SUSPENDED):
                eventType = BTL_BSC_EVENT_AUDIO_STREAMING_STOPPED;
                break;

            default:
                sendEventToBsc = FALSE;
                break;
        }
        
        if (TRUE == sendEventToBsc)
        {
            /* Pass event to the BSC module */
            if (BTL_BSC_EVENT_SLC_DISCONNECTED == eventType)
            {
                BtlA2dpSendEventToBsc(eventType, NULL, idx);
            }
            else
            {
                BtlA2dpSendEventToBsc(eventType, btlStream, idx);
            }
        }
 
		/* Pass the event to app */
		a2dpContext->callback(&btlA2dpData.event);
	}

	if((Info->event == A2DP_EVENT_STREAM_STARTED) && 
	   (btlStream->btlA2dpReconfigurationIndNeeded == TRUE))
	{
		/* Update app that the stream need to be configured */
		btlA2dpData.event.a2dpContext = btlA2dpData.a2dpContext;
		btlA2dpData.event.streamId = btlStream->streamId;
		btlA2dpData.event.callbackParms = Info;
		btlA2dpData.event.callbackParms->event = A2DP_EVENT_STREAM_CONFIGURATION_NEEDED;
		btlA2dpData.event.callbackParms->status = BT_STATUS_SUCCESS;
		btlA2dpData.event.callbackParms->error = A2DP_ERR_NO_ERROR;

		btlStream->btlA2dpReconfigurationIndNeeded = FALSE;
										
		/* Pass the event to app */
		btlA2dpData.a2dpContext->callback(&btlA2dpData.event);
	}

	if (TRUE == sendConfigEvent)
	{
		/* Send 'config' event to the A2DP task as a result of STREAM_OPEN, STREAM_SUSPENDED, CAP_CNF, 
		RECONFIG_CNF, and CLOSE events */
		bthalStatus = BTHAL_OS_SendEvent(BTHAL_OS_TASK_HANDLE_A2DP, A2DP_OS_EVENT_CONFIG_IND);
		BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed sending config event A2DP task!"));
	}
	else if (TRUE == sendDisableEvent)
	{
		/* Change the event to indicate context was disabled */
		btlA2dpData.event.callbackParms->event = A2DP_EVENT_DISABLED;
		
		/* Pass the event to app */
		a2dpContext->callback(&btlA2dpData.event);
	}

	if (TRUE == notifyRadioOffComplete)
	{
		BtlModuleNotificationCompletionEvent moduleCompletionEvent;
		moduleCompletionEvent.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
		moduleCompletionEvent.status = BT_STATUS_SUCCESS;
		
		BTL_ModuleCompleted(BTL_MODULE_TYPE_A2DP, &moduleCompletionEvent);
	}
}


/*-------------------------------------------------------------------------------
 * BtlA2dpConvertBthal2Avdtp()
 *
 *		Convert from BTHAL MM codec to AVDTP codec.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		codecInfo [in] - Local codec info
 *
 *		codec [out] - Local codec info
 *
 * Returns:
 *		void
 */
static void BtlA2dpConvertBthal2Avdtp(BthalMmConfigParams *codecInfo, AvdtpCodec *codec)
{
	switch (codecInfo->streamType)
	{
	case (BTHAL_MM_STREAM_TYPE_SBC):

		codec->codecType = AVDTP_CODEC_TYPE_SBC;
		codec->elements[0] = (U8)(codecInfo->p.sbcInfo.samplingFreq | codecInfo->p.sbcInfo.channelMode); 
		codec->elements[1] = (U8)(codecInfo->p.sbcInfo.blockLength | codecInfo->p.sbcInfo.subbands | codecInfo->p.sbcInfo.allocationMethod); 
		codec->elements[2] = (U8)(codecInfo->p.sbcInfo.minBitpoolValue);
		codec->elements[3] = (U8)(codecInfo->p.sbcInfo.maxBitpoolValue);
		break;

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

	case (BTHAL_MM_STREAM_TYPE_MPEG1_2_AUDIO):

		codec->codecType = AVDTP_CODEC_TYPE_MPEG1_2_AUDIO;
		codec->elements[0] = (U8)(codecInfo->p.mpeg1_2_audioInfo.layer | codecInfo->p.mpeg1_2_audioInfo.crcProtection | codecInfo->p.mpeg1_2_audioInfo.channelMode); 
		codec->elements[1] = (U8)(codecInfo->p.mpeg1_2_audioInfo.mpf2 | codecInfo->p.mpeg1_2_audioInfo.samplingFreq); 
		codec->elements[2] = (U8)((codecInfo->p.mpeg1_2_audioInfo.vbr | codecInfo->p.mpeg1_2_audioInfo.bitRate) >> 8);
		codec->elements[3] = (U8)((codecInfo->p.mpeg1_2_audioInfo.bitRate) & 0x00FF);
		break;

#endif

	default:

		BTL_FATAL_NORET(("Invalid BTHAL MM codec information!"));
		break;
	}
}


/*-------------------------------------------------------------------------------
 * BtlA2dpConvertAvdtp2Bthal()
 *
 *		Convert from AVDTP codec to BTHAL MM codec.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		codec [in] - codec parameters in bit map
 *
 *		codecInfo [out] - translation of elements parameter
 *
 * Returns:
 *		void
 */
static void BtlA2dpConvertAvdtp2Bthal(AvdtpCodec *codec, BthalMmConfigParams *codecInfo)
{
	U8 *elements = codec->elements;

	switch (codec->codecType)
	{
		case (AVDTP_CODEC_TYPE_SBC):

			codecInfo->streamType = BTHAL_MM_STREAM_TYPE_SBC;
			codecInfo->p.sbcInfo.samplingFreq = (U8)(elements[0] & 0xF0);
			codecInfo->p.sbcInfo.channelMode = (U8)(elements[0] & 0x0F);
			codecInfo->p.sbcInfo.blockLength = (U8)(elements[1] & 0xF0);
			codecInfo->p.sbcInfo.subbands = (U8)(elements[1] & 0x0C);
			codecInfo->p.sbcInfo.allocationMethod = (U8)(elements[1] & 0x03);
			codecInfo->p.sbcInfo.minBitpoolValue = elements[2];
			codecInfo->p.sbcInfo.maxBitpoolValue = elements[3];
			break;

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

		case (AVDTP_CODEC_TYPE_MPEG1_2_AUDIO):

			codecInfo->streamType = BTHAL_MM_STREAM_TYPE_MPEG1_2_AUDIO;
			codecInfo->p.mpeg1_2_audioInfo.layer = (U8)(elements[0] & 0xE0);
			codecInfo->p.mpeg1_2_audioInfo.crcProtection = (U8)(elements[0] & 0x10);
			codecInfo->p.mpeg1_2_audioInfo.channelMode = (U8)(elements[0] & 0x0F);
			codecInfo->p.mpeg1_2_audioInfo.mpf2 = (U8)(elements[1] & 0x40);
			codecInfo->p.mpeg1_2_audioInfo.samplingFreq = (U8)(elements[1] & 0x3F);
			codecInfo->p.mpeg1_2_audioInfo.vbr = (U16)(((U16)(elements[2] & 0x80)) << 8);
			codecInfo->p.mpeg1_2_audioInfo.bitRate = (U16)(((((U16)(elements[2] & (~0x80))) << 8) | ((U16)(elements[3]))));
			break;

#endif
			
		default:
			
			BTL_FATAL_NORET(("Invalid AVDTP codec information!"));
			break;
	}
}


/*-------------------------------------------------------------------------------
 * BtlA2dpCheckCodecCompatibility()
 *
 *		Check if the remote CODEC request is compatible with the local CODEC capabilities.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		localCodec [in] - local codec capabilities.
 *
 *		remoteCodec [in] - remote codec request.
 *
 * Returns:
 *		A2DP_ERR_NO_ERROR if the codecs are compatible, otherwise the error.
 */
static A2dpError BtlA2dpCheckCodecCompatibility(AvdtpCodec *localCodecCapabilities, AvdtpCodec *remoteCodecRequest)
{
	A2dpError error = A2DP_ERR_NO_ERROR;
	U8 *elements = localCodecCapabilities->elements;
	U8 *reqElements = remoteCodecRequest->elements;

	if (localCodecCapabilities->codecType != remoteCodecRequest->codecType)
	{
		error = A2DP_ERR_INVALID_CODEC_TYPE;
	}
	else
	{
		switch (localCodecCapabilities->codecType)
		{
			case (AVDTP_CODEC_TYPE_SBC):

		        if (!(reqElements[0] & (elements[0] & 0xF0)))
				{
		            error = A2DP_ERR_NOT_SUPPORTED_SAMP_FREQ;
		        } 
				else if (!(reqElements[0] & (elements[0] & 0x0F))) 
				{
		            error = A2DP_ERR_NOT_SUPPORTED_CHANNEL_MODE;
		        } 
				else if (!(reqElements[1] & (elements[1] & 0xF0))) 
				{
		            error = A2DP_ERR_INVALID_BLOCK_LENGTH;
		        } 
				else if (!(reqElements[1] & (elements[1] & 0x0C))) 
				{
		            error = A2DP_ERR_NOT_SUPPORTED_SUBBANDS;
		        } 
				else if (!(reqElements[1] & (elements[1] & 0x03))) 
				{
		            error = A2DP_ERR_NOT_SUPPORTED_ALLOC_METHOD;
		        } 
				else if (reqElements[2] < elements[2]) 
				{
		            error = A2DP_ERR_NOT_SUPPORTED_MIN_BITPOOL_VALUE;
		        } 
				else if (reqElements[3] > elements[3]) 
				{
		            error = A2DP_ERR_NOT_SUPPORTED_MAX_BITPOOL_VALUE;
		        }			
				else if (reqElements[2] > reqElements[3])
				{	/* minimum is greater than maximum.. */
		            error = A2DP_ERR_INVALID_MIN_BITPOOL_VALUE;
		        }			
				break;

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

			case (AVDTP_CODEC_TYPE_MPEG1_2_AUDIO):

				if (!(reqElements[0] & (elements[0] & 0xE0)))
				{
		            error = A2DP_ERR_NOT_SUPPORTED_LAYER;
		        } 
				else if ((reqElements[0] & 0x10) && (!(elements[0] & 0x10)))
				{
		            error = A2DP_ERR_NOT_SUPPORTED_CRC;
		        } 
				else if (!(reqElements[0] & (elements[0] & 0x0F)))
				{
		            error = A2DP_ERR_NOT_SUPPORTED_CHANNEL_MODE;
		        } 
				else if ((reqElements[1] & 0x40) && (!(elements[1] & 0x40))) 
				{
		            error = A2DP_ERR_NOT_SUPPORTED_MPF;
		        } 
				else if (!(reqElements[1] & (elements[1] & 0x3F))) 
				{
		            error = A2DP_ERR_NOT_SUPPORTED_SAMP_FREQ;
		        } 
				else if ((reqElements[2] & 0x80) && (!(elements[2] & 0x80))) 
				{
		            error = A2DP_ERR_NOT_SUPPORTED_VBR;
		        }
				else if ((!(reqElements[2] & (elements[2] & 0x7F))) && (!(reqElements[3] & elements[3])))
				{
		            error = A2DP_ERR_NOT_SUPPORTED_BIT_RATE;
		        }
				break;

#endif
				
			default:
				
				BTL_FATAL_NORET(("Invalid local codec!"));
				break;
		}
	}

	return (error);
}


#if SBC_ENCODER == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BtlA2dpSbcSelectionAlgorithem()
 *
 *		The SBC codec parameters selection algotithem.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		supportedCodec [in] - the common capabilities (local and remote device)
 *
 *		selectedCodec [out] - selected codec (one bit per codec element)
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
static BtStatus BtlA2dpSbcSelectionAlgorithem(AvdtpCodec *supportedCodec, AvdtpCodec *selectedCodec)
{	
	BtStatus status = BT_STATUS_SUCCESS;
	BthalMmSbcChannelModeMask channelMode = 0;
	BthalStatus bthalStatus;
	BthalMmConfigParams configParam;

	BTL_FUNC_START("BtlA2dpSbcSelectionAlgorithem");

	configParam.streamType = BTHAL_MM_STREAM_TYPE_PCM;
	bthalStatus = BTHAL_MM_GetSelectedConfigInfo(&configParam);
	BTL_VERIFY_ERR((BTHAL_STATUS_SUCCESS == bthalStatus), BT_STATUS_FAILED, ("Failed to get selected config information!"));

#if (BTL_CONFIG_A2DP_SAMPLE_RATE_CONVERTER == BTL_CONFIG_ENABLED)

	/* Assume by default that no conversion is needed */
	btlA2dpData.srcConvertPCM = FALSE;

	if (!(configParam.p.pcmInfo.samplingFreq & ((supportedCodec->elements[0]) & 0xF0)))
	{
		/* MM requested unsupported frequency, need to convert to a supported mandatory one (44.1 or 48) */
		
		BthalStatus retVal;
		SrcSamplingFreq inSampleFreq;
		SrcSamplingFreq outSampleFreq;
		
		switch (configParam.p.pcmInfo.samplingFreq)
		{
			case (BTHAL_MM_SBC_SAMPLING_FREQ_8000_EXT):
				inSampleFreq = SRC_SAMPLING_FREQ_8000;
				break;
			case (BTHAL_MM_SBC_SAMPLING_FREQ_11025_EXT):
				inSampleFreq = SRC_SAMPLING_FREQ_11025;
				break;
			case (BTHAL_MM_SBC_SAMPLING_FREQ_12000_EXT):
				inSampleFreq = SRC_SAMPLING_FREQ_12000;
				break;
			case (BTHAL_MM_SBC_SAMPLING_FREQ_16000):
				inSampleFreq = SRC_SAMPLING_FREQ_16000;
				break;
			case (BTHAL_MM_SBC_SAMPLING_FREQ_22050_EXT):
				inSampleFreq = SRC_SAMPLING_FREQ_22050;
				break;	
			case (BTHAL_MM_SBC_SAMPLING_FREQ_24000_EXT):
				inSampleFreq = SRC_SAMPLING_FREQ_24000;
				break;
			case (BTHAL_MM_SBC_SAMPLING_FREQ_32000):
				inSampleFreq = SRC_SAMPLING_FREQ_32000;
				break;	
			default:
				BTL_ERR(BT_STATUS_FAILED, ("MM supplied invalid input sample frequency!"));
		}

		retVal = SRC_ConfigureNewSong(inSampleFreq, configParam.p.pcmInfo.numOfchannels, BTL_CONFIG_A2DP_SRC_PCM_OUTPUT_BLOCK_MAX_LEN, &outSampleFreq);
		BTL_VERIFY_FATAL((BTHAL_STATUS_SUCCESS == retVal), BT_STATUS_FAILED, ("Failed to configure SRC!"));

		/* Update MM requested sample frequency to SRC output sample frequency */
		switch (outSampleFreq)
		{
			case (SRC_SAMPLING_FREQ_44100):
				configParam.p.pcmInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_44100;
				break;
			case (SRC_SAMPLING_FREQ_48000):
				configParam.p.pcmInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_48000;
				break;
			default:
				BTL_ERR(BT_STATUS_FAILED, ("SRC supplied invalid output sample frequency!"));
		}

		/* PCM conversion will be needed */
		btlA2dpData.srcConvertPCM = TRUE;
	}

#endif	/* BTL_CONFIG_A2DP_SAMPLE_RATE_CONVERTER == BTL_CONFIG_ENABLED */

	BTL_VERIFY_ERR((configParam.p.pcmInfo.samplingFreq & supportedCodec->elements[0]), BT_STATUS_FAILED, ("MM requested unsupported sample freq!"));
	
	if (configParam.p.pcmInfo.numOfchannels == BTHAL_MM_ONE_CHANNELS)
		channelMode |= BTHAL_MM_SBC_CHANNEL_MODE_MONO;
	else 
		channelMode |= (BTHAL_MM_SBC_CHANNEL_MODE_DUAL_CHANNEL | BTHAL_MM_SBC_CHANNEL_MODE_STEREO | BTHAL_MM_SBC_CHANNEL_MODE_JOINT_STEREO);

	BTL_VERIFY_ERR((channelMode & supportedCodec->elements[0]), BT_STATUS_FAILED, ("MM requested unsupported channel mode!"));

	btlA2dpData.audioQuality= configParam.p.pcmInfo.audioQuality;
	btlA2dpData.varyBitPool = configParam.p.pcmInfo.varyBitPool;

	OS_MemSet(selectedCodec->elements, 0, MAX_NUM_OF_BYTES_SPECIFIC_INFO_ELEMENTS);

	switch (supportedCodec->codecType)
 	{
 		case BTHAL_MM_STREAM_TYPE_SBC:

			/* choose preferred sample frequency - whatever determined by the MM */
			 selectedCodec->elements[0] = configParam.p.pcmInfo.samplingFreq;

			/* choose preferred channel mode*/
			if (configParam.p.pcmInfo.numOfchannels == BTHAL_MM_ONE_CHANNELS)
				selectedCodec->elements[0] |= A2DP_SBC_CODEC_CHNL_MODE_MONO;
			else 
			{
				if (supportedCodec->elements[0] & A2DP_SBC_CODEC_CHNL_MODE_JOINT)
				{
					selectedCodec->elements[0] |= A2DP_SBC_CODEC_CHNL_MODE_JOINT;
				}
				else if (supportedCodec->elements[0] & A2DP_SBC_CODEC_CHNL_MODE_STEREO)
				{
					selectedCodec->elements[0] |= A2DP_SBC_CODEC_CHNL_MODE_STEREO;
				}
				else
				{
					selectedCodec->elements[0] |= A2DP_SBC_CODEC_CHNL_MODE_DUAL;
				}
			}

			/* 	in the following parameters, we try to select the highest possible,
				which is according to the recommended in the profile spec:
				Block length = 16, Allocation method = Loudness, Subbands = 8.
			*/

			/* choose preferred blocks length - highest supported */
		    if (supportedCodec->elements[1] & A2DP_SBC_CODEC_BLOCKS_16) 
			{
				selectedCodec->elements[1] = A2DP_SBC_CODEC_BLOCKS_16;
		    }
		    else if (supportedCodec->elements[1] & A2DP_SBC_CODEC_BLOCKS_12) 
			{
				selectedCodec->elements[1] = A2DP_SBC_CODEC_BLOCKS_12;
		    }
		    else if (supportedCodec->elements[1] & A2DP_SBC_CODEC_BLOCKS_8) 
			{
				selectedCodec->elements[1] = A2DP_SBC_CODEC_BLOCKS_8;
		    }
		    else
			{
				selectedCodec->elements[1] = A2DP_SBC_CODEC_BLOCKS_4;
		    }

			/* choose preferred subbands - highest supported */
		    if (supportedCodec->elements[1] & A2DP_SBC_CODEC_SUBBANDS_8) 
			{
				selectedCodec->elements[1] |= A2DP_SBC_CODEC_SUBBANDS_8;
		    }
		    else 
			{
				selectedCodec->elements[1] |= A2DP_SBC_CODEC_SUBBANDS_4;
		    }

			/* choose preferred alloocation method */
		    if (supportedCodec->elements[1] & A2DP_SBC_CODEC_ALLOCATION_LOUDNESS) 
			{
				selectedCodec->elements[1] |= A2DP_SBC_CODEC_ALLOCATION_LOUDNESS;
		    }
		    else
			{
				selectedCodec->elements[1] |= A2DP_SBC_CODEC_ALLOCATION_SNR;
		    }

			/* 	set bit pool min and max.
				the acutal initial bitpool will be set later in btlA2dpCodecConfigured(), according to the recomendation
				in the profile spec (per high or middle quality) 
			*/
			btlA2dpData.selMinBitPool = selectedCodec->elements[2] = supportedCodec->elements[2];
			btlA2dpData.selMaxBitPool = selectedCodec->elements[3] = supportedCodec->elements[3];
			
			break;
 	}

	BTL_FUNC_END();
	
	return (status);
}
#endif /* SBC_ENCODER == XA_ENABLED */

	
/*-------------------------------------------------------------------------------
 * BtlA2dpSbcConvertAvdtp2Btstack()
 *
 *		Convert from AVDTP codec to ESI SbcStreamInfo struct. It is assumed that onle
 *		a single bit is set for each parameter, i.e. the codec is already configured.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		codec [in] - Local codec info
 *
 *		sbcStreamInfo [out] - Local codec info
 *
 * Returns:
 *		void
 */
void BtlA2dpSbcConvertAvdtp2Btstack(AvdtpCodec *Codec, SbcStreamInfo *sbcStreamInfo)
{
	/* Pick the sampling rate supported */
	    if (Codec->elements[0] & A2DP_SBC_CODEC_FREQ_48000) {
	        sbcStreamInfo->sampleFreq = SBC_CHNL_SAMPLE_FREQ_48;
	    } else if (Codec->elements[0] & A2DP_SBC_CODEC_FREQ_44100) {
	        sbcStreamInfo->sampleFreq = SBC_CHNL_SAMPLE_FREQ_44_1;
	    } else if (Codec->elements[0] & A2DP_SBC_CODEC_FREQ_32000) {
	        sbcStreamInfo->sampleFreq = SBC_CHNL_SAMPLE_FREQ_32;
	    } else if (Codec->elements[0] & A2DP_SBC_CODEC_FREQ_16000) {
	        sbcStreamInfo->sampleFreq = SBC_CHNL_SAMPLE_FREQ_16;
	    }

	    /* Pick the channel mode */
	    sbcStreamInfo->numChannels = 2;
	    if (Codec->elements[0] & A2DP_SBC_CODEC_CHNL_MODE_JOINT) {
	        sbcStreamInfo->channelMode = SBC_CHNL_MODE_JOINT_STEREO;
	    } else if (Codec->elements[0] & A2DP_SBC_CODEC_CHNL_MODE_STEREO) {
	        sbcStreamInfo->channelMode = SBC_CHNL_MODE_STEREO;
	    } else if (Codec->elements[0] & A2DP_SBC_CODEC_CHNL_MODE_DUAL) {
	        sbcStreamInfo->channelMode = SBC_CHNL_MODE_DUAL_CHNL;
	    } else if (Codec->elements[0] & A2DP_SBC_CODEC_CHNL_MODE_MONO) {
	        sbcStreamInfo->numChannels = 1;
	        sbcStreamInfo->channelMode = SBC_CHNL_MODE_MONO;
	    }

	    /* Pick the block length */
	    if (Codec->elements[1] & A2DP_SBC_CODEC_BLOCKS_16) {
	        sbcStreamInfo->numBlocks = 16;
	    } else if (Codec->elements[1] & A2DP_SBC_CODEC_BLOCKS_12) {
	        sbcStreamInfo->numBlocks = 12;
	    } else if (Codec->elements[1] & A2DP_SBC_CODEC_BLOCKS_8) {
	        sbcStreamInfo->numBlocks = 8;
	    } else if (Codec->elements[1] & A2DP_SBC_CODEC_BLOCKS_4) {
	        sbcStreamInfo->numBlocks = 4;
	    }

	    /* Pick the number of subbands */
	    if (Codec->elements[1] & A2DP_SBC_CODEC_SUBBANDS_8) {
	        sbcStreamInfo->numSubBands = 8;
	    } else if (Codec->elements[1] & A2DP_SBC_CODEC_SUBBANDS_4) {
	        sbcStreamInfo->numSubBands = 4;
	    }

	    /* Pick the allocation method */
	    if (Codec->elements[1] & A2DP_SBC_CODEC_ALLOCATION_LOUDNESS) {
	        sbcStreamInfo->allocMethod = SBC_ALLOC_METHOD_LOUDNESS;
	    } else if (Codec->elements[1] & A2DP_SBC_CODEC_ALLOCATION_SNR) {
	        sbcStreamInfo->allocMethod = SBC_ALLOC_METHOD_SNR;
	    }

		sbcStreamInfo->bitPool = Codec->elements[2];

}

	
/*-------------------------------------------------------------------------------
 * BtlA2dpSelectCodec()
 *
 *		Select codec according to local and remote device capabilities
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		btlStream [in] - stream
 *
 *		codec [out] - selected codec 
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
static BtStatus BtlA2dpSelectCodec(BtlA2dpStream *btlStream, AvdtpCodec *codec)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BthalMmConfigParams configInfo;
	BthalStatus bthalStatus;
#if SBC_ENCODER == XA_ENABLED
	U32 idx;
	AvdtpCodec supportedCodec;
	U8 supportedElements[MAX_NUM_OF_BYTES_SPECIFIC_INFO_ELEMENTS];
#endif /* SBC_ENCODER == XA_ENABLED */

	BTL_FUNC_START("BtlA2dpSelectCodec");

	codec->codecType = btlStream->localCodec.codecType;
	configInfo.streamType = BTHAL_MM_STREAM_TYPE_MPEG1_2_AUDIO;

	BTL_VERIFY_FATAL_NORET((btlStream->remoteCodecValid == TRUE), ("Invalid remote codec!"));
	
	switch(btlStream->localCodec.codecType)
	{
		case (AVDTP_CODEC_TYPE_SBC):
			
#if SBC_ENCODER == XA_ENABLED

			if ( BTHAL_MM_SBC_ENCODER_BUILT_IN == btlA2dpData.sbcEncoderLocation )
			{
			/* btl a2dp choose the selected codec */
			supportedCodec.codecType = AVDTP_CODEC_TYPE_SBC;
			supportedCodec.elemLen = sizeof(supportedElements);
			supportedCodec.elements = supportedElements;

			/* First set codec to local capabilities */
			OS_MemCopy(supportedElements, btlStream->localCodec.elements, sizeof(supportedElements));

			/* SBC streams come before MP3 streams in the streams array, 
			so we can count until BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT */
			for(idx = 0; idx < BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT; idx++)
		    {
		    	btlStream = &(btlA2dpData.a2dpContext->streams[idx]);

				/* Select the 'intersection' between all connected streams 
				with valid remote codec capabilities and local codec capabilities */
				if (btlStream->remoteCodecValid == TRUE)
				{
					supportedElements[0] &= (btlStream->remoteCodec.elements[0]);
					supportedElements[1] &= (btlStream->remoteCodec.elements[1]);

					/* Select the MAX value of the possible 'Min bitpool' values */
					supportedElements[2] = (U8)(max(btlStream->remoteCodec.elements[2], supportedElements[2]));
				
					/* Select the MIN value of the possible 'Max bitpool' values */
					supportedElements[3] = (U8)(min(btlStream->remoteCodec.elements[3], supportedElements[3]));
				}
			}

			if ((!(supportedElements[0] & 0xF0)) || 
				(!(supportedElements[0] & 0x0F)) || 
				(!(supportedElements[1] & 0xF0)) || 
				(!(supportedElements[1] & 0xC0)) || 
				(!(supportedElements[1] & 0x03)) || 
				(supportedElements[2] > supportedElements[3]))
			{
				status = BT_STATUS_FAILED;
			}
			else				
			{
				/* select one bit for each codec capability */
				status = BtlA2dpSbcSelectionAlgorithem(&supportedCodec, codec);
			}

			break;
			}
			
#endif /* SBC_ENCODER == XA_ENABLED */

			configInfo.streamType = BTHAL_MM_STREAM_TYPE_SBC;

			/* Pass through to next case */

		case (AVDTP_CODEC_TYPE_MPEG1_2_AUDIO):

			/* Get the selected config from BTHAL MM */
			bthalStatus = BTHAL_MM_GetSelectedConfigInfo(&configInfo);
			BTL_VERIFY_ERR((BTHAL_STATUS_SUCCESS == bthalStatus), BT_STATUS_FAILED, ("Failed to get selected config information!"));

			BtlA2dpConvertBthal2Avdtp(&configInfo, codec);
			
			if ( (BTHAL_MM_SBC_ENCODER_EXTERNAL == btlA2dpData.sbcEncoderLocation) &&
				 (configInfo.streamType == BTHAL_MM_STREAM_TYPE_SBC) )
			{
				BtlA2dpSbcConvertAvdtp2Btstack(codec, &btlA2dpData.sbcStreamInfo);

				/* Configuration parameters were changed, frame length needs to be calculated again */
				btlA2dpData.sbcStreamInfo.lastSbcFrameLen = 0;
			}

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

			/* Save current MP3 layer */
			if (codec->codecType == AVDTP_CODEC_TYPE_MPEG1_2_AUDIO)
			{
				btlA2dpData.a2dpContext->mpeg1_2_audioLayer = (U8)((codec->elements[0] & 0xE0));

				switch (codec->elements[1] & 0x3F)
				{
					case (A2DP_MP3_CODEC_FREQ_16000):
						btlA2dpData.a2dpContext->mpeg1_2_audioSampleFreq = 16000;
						break;
					case (A2DP_MP3_CODEC_FREQ_22050):
						btlA2dpData.a2dpContext->mpeg1_2_audioSampleFreq = 22050;
						break;
					case (A2DP_MP3_CODEC_FREQ_24000):
						btlA2dpData.a2dpContext->mpeg1_2_audioSampleFreq = 24000;
						break;
					case (A2DP_MP3_CODEC_FREQ_32000):
						btlA2dpData.a2dpContext->mpeg1_2_audioSampleFreq = 32000;
						break;
					case (A2DP_MP3_CODEC_FREQ_44100):
						btlA2dpData.a2dpContext->mpeg1_2_audioSampleFreq = 44100;
						break;
					case (A2DP_MP3_CODEC_FREQ_48000):
						btlA2dpData.a2dpContext->mpeg1_2_audioSampleFreq = 48000;
						break;
				}
				btlA2dpData.a2dpContext->curBitRate = configInfo.p.mpeg1_2_audioInfo.curBitRate;
			}

#endif
			
			break;
	}

	BTL_FUNC_END();
	
	return status;
}


/*-------------------------------------------------------------------------------
 * BtlMmCallback()
 *
 *		BTHAL MM call back
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		callbackParams [in] - BTHAL MM call back parameters
 *
 * Returns:
 *		void
 */
static void BtlMmCallback(BthalMmCallbackParms *callbackParams)
{
	BtlA2dpRawBlock *rawBlock;
	BthalOsEvent evt = 0;
	BthalStatus bthalStatus;

	if (btlA2dpData.a2dpContext->state != BTL_A2DP_STATE_ENABLED)
		return;

	switch (callbackParams->event)
	{
	case (BTHAL_MM_EVENT_DATA_BUFFER_IND):
		if (btlA2dpData.numStreamingStreams > 0)
		{
			/* Get pcm block from the raw blocks pool */
			rawBlock = btlA2dpGetRawBlockFromPool();			
			BTL_VERIFY_FATAL_NORET((rawBlock != 0), ("No blocks avaliable at RawBlockPool"));

			/* Save the data sent from the MM */
			rawBlock->descriptor = callbackParams->p.dataBuffer.descriptor;
			rawBlock->data = callbackParams->p.dataBuffer.data;
			rawBlock->dataLength = callbackParams->p.dataBuffer.dataLength;
			rawBlock->streamType = callbackParams->p.dataBuffer.streamType;

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0
			rawBlock->frameOffset = callbackParams->p.dataBuffer.frameOffset;
#endif

			/* Put raw block into raw block queue */
			btlA2dpInsertRawBlockToQueue(rawBlock);

			/* Send data buffer event to A2DP task */
			evt = A2DP_OS_EVENT_DATA_IND;
			break;
		}
		else
		{	/* ignore this raw block, we are not streaming yet, or we already stopped streaming */
			bthalStatus = BTHAL_MM_FreeDataBuf(callbackParams->p.dataBuffer.descriptor);
			return;
		}
	case (BTHAL_MM_EVENT_CONFIG_IND):
		{	
			/* Send config event to A2DP task */
			evt = A2DP_OS_EVENT_CONFIG_IND;
			break;
		}
	case (BTHAL_MM_EVENT_START_PULL):
		{	
			btlA2dpData.pullDataState = BTL_A2DP_PULL_DATA_STATE_MASK_PULL_ACTIVE;
			BTL_LOG_INFO(("event BTHAL_MM_EVENT_START_PULL"));
			/* Send data sent event to A2DP task, so it will start pulling the data */
			evt = A2DP_OS_EVENT_DATA_SENT;
			break;
		}
	case (BTHAL_MM_EVENT_STOP_PULL):
		{	
			btlA2dpData.pullDataState = BTL_A2DP_PULL_DATA_STATE_MASK_NONE;
			BTL_LOG_INFO(("event BTHAL_MM_EVENT_STOP_PULL"));

			/* No need to send an event, A2DP will not request more data from MM */
			return;
		}
	default:
		{
			BTL_FATAL_NORET(("Unknown event"));
		}
	}

	/* Send a event to the A2DP task */
	bthalStatus = BTHAL_OS_SendEvent(BTHAL_OS_TASK_HANDLE_A2DP, evt);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed sending event 0x%x to A2DP task!", evt));
}


/*-------------------------------------------------------------------------------
 * BtlA2dpOsInit()
 *
 *		Internal function for init the A2DP OS needs.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		void.
 */
static void BtlA2dpOsInit(void)
{
	BthalStatus bthalStatus;

	BTL_FUNC_START("BtlA2dpOsInit");

	/* Create A2DP task */
	bthalStatus = BTHAL_OS_CreateTask(BTHAL_OS_TASK_HANDLE_A2DP, BtlA2dpOsEventCallback, "A2DP_TASK");
	BTL_VERIFY_FATAL_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed creating A2DP task!"));

	/* Create A2DP MM semaphore */
	bthalStatus = BTHAL_OS_CreateSemaphore("A2DP_MM_SEM", &btlA2dpData.a2dpMmSemHandle);
	BTL_VERIFY_FATAL_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed creating A2DP MM semaphore!"));

	/* Create A2DP Stack semaphore */
	bthalStatus = BTHAL_OS_CreateSemaphore("A2DP_STACK_SEM", &btlA2dpData.a2dpStackSemHandle);
	BTL_VERIFY_FATAL_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed creating A2DP Stack semaphore!"));

	/* Create A2DP timer */
	bthalStatus = BTHAL_OS_CreateTimer(BTHAL_OS_TASK_HANDLE_A2DP, "A2DP_TIMER", &btlA2dpData.a2dpTimerHandle);
	BTL_VERIFY_FATAL_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed creating A2DP timer!"));

	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * BtlA2dpOsDeinit()
 *
 *		Internal function for deinit the A2DP OS needs.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		void.
 */
static void BtlA2dpOsDeinit(void)
{
	BthalStatus bthalStatus;

	BTL_FUNC_START("BtlA2dpOsDeinit");

	/* Destroy A2DP timer */
	bthalStatus = BTHAL_OS_DestroyTimer(btlA2dpData.a2dpTimerHandle);
	BTL_VERIFY_FATAL_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed destroying A2DP timer!"));

	/* Destroy A2DP Stack semaphore */
	bthalStatus = BTHAL_OS_DestroySemaphore(btlA2dpData.a2dpStackSemHandle);
	BTL_VERIFY_FATAL_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed destroying A2DP Stack semaphore!"));

	/* Destroy A2DP MM semaphore */
	bthalStatus = BTHAL_OS_DestroySemaphore(btlA2dpData.a2dpMmSemHandle);
	BTL_VERIFY_FATAL_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed destroying A2DP MM semaphore!"));

	/* Destroy A2DP task */
	bthalStatus = BTHAL_OS_DestroyTask(BTHAL_OS_TASK_HANDLE_A2DP);
	BTL_VERIFY_FATAL_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed destroying A2DP task!"));

	BTL_FUNC_END();
}


#if	A2DP_USE_TIMER == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BtlA2dpOsResetTimer()
 *
 *		Internal function for resetting A2DP timer.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		time - number of ms until the timer fires.
 *
 * Returns:
 *		void.
 */
static void BtlA2dpOsResetTimer(U32 time)
{
	BthalStatus bthalStatus;

	/* Reset A2DP timer */
	bthalStatus = BTHAL_OS_ResetTimer(btlA2dpData.a2dpTimerHandle, 
					MS_TO_TICKS(time), A2DP_OS_EVENT_TIMER_EXPIRED);
	btlA2dpData.timerTriggeredTime = TICKS_TO_MS(OS_GetSystemTime());
	btlA2dpData.timerDelay = time;
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed resetting A2DP timer!"));
}


/*-------------------------------------------------------------------------------
 * BtlA2dpOsCancelTimer()
 *
 *		Internal function for cancelling A2DP timer.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		void.
 */
static void BtlA2dpOsCancelTimer(void)
{
	BthalStatus bthalStatus;

	/* Cancel A2DP timer */
	bthalStatus = BTHAL_OS_CancelTimer(btlA2dpData.a2dpTimerHandle);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed cancelling A2DP timer!"));
}


/*-------------------------------------------------------------------------------
 * btlA2dpHandleProcessQueue()
 *
 *		handler function for A2DP timer.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		curTime [in] - current time in ms
 *
 *		fromTimer[in]  - whether called in timer callback context
 *
 * Returns:
 *		void.
 */
static void btlA2dpHandleProcessQueue(U32 curTime, BOOL fromTimer)
{
	S32 timerLatency = curTime -(btlA2dpData.timerTriggeredTime + btlA2dpData.timerDelay);
	U32 elapsedTime = curTime - btlA2dpData.startSendingTime;

	if (btlA2dpData.canSendFromQueue)
	{
		/* send from queue is possible && packet send is allowed */
		U32 playTime;
		int sentCnt = 0;

		do
		{
			playTime = btlA2dpData.processQueueFuncTable[btlA2dpData.currentCodecType]();
#ifdef VBR
			if ((fromTimer ||(btlA2dpData.numPacketsNeedToBeSent > 0)) && 
				(playTime > 0) && (sentCnt == 0))
#else
			if (fromTimer && (playTime > 0) && (sentCnt == 0))
#endif	/* VBR */
			{
				S32 reqTimerDelayMs;
				/* subtracting timerLatency below does not compensate enough in this call if 
				the latency is larger than packet period. compensation will happen in future
				calls when we try to adjudt to the average speed */
#ifdef VBR
				if (timerLatency < 0)
					timerLatency = 0;
#endif	/* VBR */
				reqTimerDelayMs = playTime/BTL_A2DP_TIME_RESOLUTION - BTL_A2DP_SHORTEN_TIMER - timerLatency;
				if (reqTimerDelayMs < 8)
					reqTimerDelayMs = 8;
				if (!fromTimer)
					BtlA2dpOsCancelTimer();
				BtlA2dpOsResetTimer(reqTimerDelayMs);
			}
			if (playTime > 0)
			{
				sentCnt++;
				if (btlA2dpData.numPacketsNeedToBeSent > 0)
				{
					btlA2dpData.numPacketsNeedToBeSent--;
				}
				btlA2dpData.accumulatedMediaTime += playTime;
			}	
		} while ( (playTime > 0) && 
			((elapsedTime + BTL_A2DP_PREBUFFER_AT_SINK) > btlA2dpData.accumulatedMediaTime/BTL_A2DP_TIME_RESOLUTION) );

		if (fromTimer)
		{
			if (sentCnt == 0)
			{
				btlA2dpData.numPacketsNeedToBeSent = 1;
				/* set timer to same period as in previous time */
#ifdef VBR
#else
				BtlA2dpOsResetTimer(btlA2dpData.timerDelay);
#endif	/* VBR */
			}
		}

		if ( (sentCnt > 0) && (playTime == 0) && 
			((elapsedTime+BTL_A2DP_PREBUFFER_AT_SINK) > btlA2dpData.accumulatedMediaTime/BTL_A2DP_TIME_RESOLUTION) )
		{
			if (btlA2dpData.numPacketsNeedToBeSent == 0)
				btlA2dpData.numPacketsNeedToBeSent = 1;
		}
	}
	else
	{
		/* we got here because we haven't got enough data to start sending, 
		and we'll probably won't receive enough because the MM will not give us more before we free some of its blocks.
		therefore we pretend as if we are passed the threshold */
		BTL_LOG_DEBUG(("btlA2dpHandleProcessQueue: forcing start sending even though we are below threshold"));
		btlA2dpCheckforStartSending(BTL_A2DP_TIME_RESOLUTION*BTL_A2DP_PREBUFFER_BEFORE_SEND_THRESHOLD);
	}
}


/*-------------------------------------------------------------------------------
 * btlA2dpHandleTimedProcessQueue()
 *
 *		call btlA2dpHandleProcessQueue in case we haven't sent enough
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		none
 *
 * Returns:
 *		void.
 *
 */
static void btlA2dpHandleTimedProcessQueue()
{
	U32 curTime = TICKS_TO_MS(OS_GetSystemTime());
	U32 elapsedTime = curTime - btlA2dpData.startSendingTime;

	if ((btlA2dpData.numPacketsNeedToBeSent > 0) ||
		((elapsedTime + BTL_A2DP_PREBUFFER_AT_SINK) > btlA2dpData.accumulatedMediaTime/BTL_A2DP_TIME_RESOLUTION))
	{
		btlA2dpHandleProcessQueue(curTime, FALSE);
	}
}

#endif	/* A2DP_USE_TIMER == XA_ENABLED */


/*-------------------------------------------------------------------------------
 * BtlA2dpOsEventCallback()
 *
 *		Internal function for handling A2DP events.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		evtMask - events bitmask.
 *
 * Returns:
 *		void.
 */
static void BtlA2dpOsEventCallback(BthalOsEvent evtMask)
{			
	BtlA2dpRawBlock *rawBlock = 0;
	BOOL contProcess = TRUE;	/* Check whether more free SBC / MP3 packets are avaliable */
	
#if	A2DP_USE_TIMER == XA_ENABLED

	if (evtMask & A2DP_OS_EVENT_TIMER_EXPIRED)
	{
		U32 curTime = TICKS_TO_MS(OS_GetSystemTime());
		btlA2dpHandleProcessQueue(curTime, TRUE);
	}

#endif 	/* A2DP_USE_TIMER == XA_ENABLED */

	if (evtMask & A2DP_OS_EVENT_DATA_IND)
	{
#if	A2DP_USE_TIMER == XA_ENABLED
		BOOL canAlreadySend = btlA2dpData.canSendFromQueue;
#endif 	/* A2DP_USE_TIMER == XA_ENABLED */
		
		/* Get raw block from the rawBlockQueue */
		while ((contProcess == TRUE) && (rawBlock = btlA2dpGetRawBlockFromQueue()) != 0)
		{
			contProcess = btlA2dpData.processRawBlockFuncTable[rawBlock->streamType](rawBlock);

			/* Return the rawBlock to the pool */
			btlA2dpInsertRawBlockToPool(rawBlock);
		}

#if	A2DP_USE_TIMER == XA_ENABLED
		if (canAlreadySend)
		{
			btlA2dpHandleTimedProcessQueue();
		}
#else
		/* Assumption: Only one active codec type! */ 
		btlA2dpData.processQueueFuncTable[btlA2dpData.currentCodecType]();
#endif 	/* A2DP_USE_TIMER == XA_ENABLED */
	}

	if (evtMask & A2DP_OS_EVENT_DATA_SENT)
	{
	btlA2dpHandleOsEventDataSent();
	}
	
	if (evtMask & A2DP_OS_EVENT_CONFIG_IND)		
	{
		BTL_LOG_DEBUG(("Received A2DP_OS_EVENT_CONFIG_IND"));

		btlA2dpCheckAndReconfigStream();
	}
	
}


/*-------------------------------------------------------------------------------
 * btlA2dpGetNumOfStreamingStreams()
 *
 *		count how many streams in AVDTP STREAMING state we currently have.
 *
 *           updates btlA2dpData.numStreamingStreams accordingly.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		number of streams in streaming  state
 */
static U8 btlA2dpGetNumOfStreamingStreams(void)
{
	U8 idx, startIdx, endIdx;
	U8 ret = 0;

	if ( btlA2dpData.currentCodecType != BTHAL_MM_STREAM_TYPE_MPEG1_2_AUDIO)
	{
		startIdx = 0;
		endIdx = BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT;
	}
	else
	{
		startIdx = BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT;
		endIdx = A2DP_MAX_NUM_STREAMS_PER_CONTEXT;
	}

	for (idx = startIdx; idx < endIdx ; idx++)
	{
		if (btlA2dpData.a2dpContext->streams[idx].stream.stream.state == AVDTP_STRM_STATE_STREAMING)
		{
			ret++;
		}
	}

	btlA2dpData.numStreamingStreams = ret;

	return ret;
}


/*-------------------------------------------------------------------------------
 * btlA2dpProcessSbcQueue()
 *
 *		Process SBC queue.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		amount of playback time sent (in tenths of ms).
 */
static U32 btlA2dpProcessSbcQueue(void)
{
	BtStatus status;
	BtlA2dpSbcPacket *sbcPacket = 0;
	S32 idx;
	BtlA2dpStream *btlStream;
	U32 playTime = 0;
#if HCI_QOS == XA_ENABLED
	BOOL singleStream = (btlA2dpData.numStreamingStreams == 1);

	/* Get sbc packets from the sbcPacketQueue */
	while ((singleStream || (isSendingAllowed() == TRUE)) 
				&& 
			((sbcPacket = btlA2dpGetSbcPacketFromQueue()) != 0))
			 
#else /* from here HCI_QOS is FALSE */
	while ((sbcPacket = btlA2dpGetSbcPacketFromQueue()) != 0)
#endif /*HCI_QOS == XA_ENABLED  */
	{
		/* SBC streams come before MP3 streams in the streams array, 
		so we can count until BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT */
		for (idx = 0; idx<BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT; idx++)
		{
			btlStream = &(btlA2dpData.a2dpContext->streams[idx]);

#if HCI_QOS == XA_ENABLED
			/* Send SBC packet only if stream is in streaming state and state is allowed*/
			if ((btlStream->stream.stream.state == AVDTP_STRM_STATE_STREAMING) && (singleStream || btlStream->isSendAllowed))
#else
			/* Send SBC packet only if stream is in streaming state */
			if (btlStream->stream.stream.state == AVDTP_STRM_STATE_STREAMING)
#endif /* HCI_QOS == XA_ENABLED */
			{
				/* Prepare next packet for next stream */
				sbcPacket->packets[sbcPacket->refCounter].data = sbcPacket->packets[0].data;
				sbcPacket->packets[sbcPacket->refCounter].dataLen = sbcPacket->packets[0].dataLen;
				sbcPacket->packets[sbcPacket->refCounter].frameSize = sbcPacket->packets[0].frameSize;		
				status = A2DP_StreamSendSbcPacket(&(btlStream->stream), 
												   &(sbcPacket->packets[sbcPacket->refCounter]), 
#if SBC_ENCODER == XA_ENABLED
						(BTHAL_MM_SBC_ENCODER_BUILT_IN == btlA2dpData.sbcEncoderLocation) ?
						&sbcEncoder.streamInfo :
#endif /* SBC_ENCODER == XA_ENABLED */
												   &btlA2dpData.sbcStreamInfo);
				if (status == BT_STATUS_PENDING)
				{
					sbcPacket->refCounter++;

#if HCI_QOS == XA_ENABLED
						btlStream->numOfPacketsSent++; 
#endif /* HCI_QOS == XA_ENABLED */
					/*BTL_LOG_INFO(("A2DP_StreamSendSbcPacket pending, refCounter=%d, desc = %d", sbcPacket->refCounter, sbcPacket->descriptor));*/
				}
				else
					BTL_LOG_ERROR(("A2DP_StreamSendSbcPacket failed, returned %d", status));

			}
		}

		if (sbcPacket->refCounter == 0)
		{
			/* Sending failed, return packet to pool */
			btlA2dpInsertSbcPacketToPool(sbcPacket);

#if HCI_QOS == XA_ENABLED
#else
			if (BTHAL_MM_SBC_ENCODER_EXTERNAL == btlA2dpData.sbcEncoderLocation)
			{
				BthalStatus bthalStatus;
				bthalStatus = BTHAL_MM_FreeDataBuf(sbcPacket->descriptor); 
				BTL_VERIFY_ERR_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed to free SBC buffer"));
			}
#endif /* HCI_QOS == XA_ENABLED */

			if (btlA2dpData.pullDataState & BTL_A2DP_PULL_DATA_STATE_MASK_PULL_ACTIVE)
			{
				btlA2dpData.pullDataState |= BTL_A2DP_PULL_DATA_STATE_MASK_PULL_ON_HOLD;
		}
	}
#if A2DP_USE_TIMER == XA_ENABLED
		else
		{
			playTime += btlA2dpCalcSbcPlayTime(sbcPacket);
		}
		break;
#endif	/* A2DP_USE_TIMER == XA_ENABLED */

	}

	return (playTime);
}


#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

/*-------------------------------------------------------------------------------
 * btlA2dpProcessMpeg1_2_AudioQueue()
 *
 *		Process MP3 queue.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		amount of playback time sent (in tenths of ms).
 */
static U32 btlA2dpProcessMpeg1_2_AudioQueue(void)
{
	BtStatus status;
	BtlA2dpMpeg1_2_AudioPacket *mp3Packet = 0;
	U32 idx;
	BtlA2dpStream *btlStream;
	AvdtpMediaHeader *mediaHeader = &btlA2dpData.a2dpContext->mpeg1_2_audioMediaHeader;
	U32 lastTimeStamp;
	U32 playTime = 0;
#if HCI_QOS == XA_ENABLED
	BOOL singleStream = (btlA2dpData.numStreamingStreams == 1);

	/* Get MP3 packets from the mpeg1_2_audioPacketQueue */
	while ((singleStream || (isSendingAllowed() == TRUE))
			&&
			((mp3Packet = btlA2dpGetMpeg1_2_AudioPacketFromQueue()) != 0))
#else
	BthalStatus bthalStatus;

	while ((mp3Packet = btlA2dpGetMpeg1_2_AudioPacketFromQueue()) != 0)
#endif /* HCI_QOS == XA_ENABLED */
	{
		/* SBC streams come before MP3 streams in the streams array, 
		so we can count from BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT 
		till A2DP_MAX_NUM_STREAMS_PER_CONTEXT */
		for (idx = BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT; idx<A2DP_MAX_NUM_STREAMS_PER_CONTEXT; idx++)
		{
			btlStream = &(btlA2dpData.a2dpContext->streams[idx]);

#if HCI_QOS == XA_ENABLED
			/* Send MP3 packet only if stream is in streaming state and state is allowed*/
			if ((btlStream->stream.stream.state == AVDTP_STRM_STATE_STREAMING) && (singleStream || btlStream->isSendAllowed))
#else
			/* Send MP3 packet only if stream is in streaming state */
			if (btlStream->stream.stream.state == AVDTP_STRM_STATE_STREAMING)
#endif /* HCI_QOS == XA_ENABLED */
			{
				/* Prepare next packet for next stream */
				mp3Packet->packets[mp3Packet->refCounter].data = mp3Packet->packets[0].data;
				mp3Packet->packets[mp3Packet->refCounter].dataLen = mp3Packet->packets[0].dataLen;
				mp3Packet->packets[mp3Packet->refCounter].flags = mp3Packet->packets[0].flags;
				mp3Packet->packets[mp3Packet->refCounter].headerLen = mp3Packet->packets[0].headerLen;
				OS_MemCopy(&(mp3Packet->packets[mp3Packet->refCounter].header[BT_PACKET_HEADER_LEN - BTL_A2DP_MAX_MPEG1_2_AUDIO_HEADER_LENGTH]), 
							&(mp3Packet->packets[0].header[BT_PACKET_HEADER_LEN - BTL_A2DP_MAX_MPEG1_2_AUDIO_HEADER_LENGTH]), 
							BTL_A2DP_MAX_MPEG1_2_AUDIO_HEADER_LENGTH);
			
				status = A2DP_StreamSendRawPacket(&(btlStream->stream), 
												   &(mp3Packet->packets[mp3Packet->refCounter]));
				
				if (status == BT_STATUS_PENDING)
				{
					mp3Packet->refCounter++;
#if HCI_QOS == XA_ENABLED
						btlStream->numOfPacketsSent++; 
#endif /* HCI_QOS == XA_ENABLED */
				}
			}
		}

		if (mp3Packet->refCounter == 0)
		{
			/* Sending failed, return packet to pool */
			btlA2dpInsertMpeg1_2_AudioPacketToPool(mp3Packet);

#if HCI_QOS == XA_ENABLED
#else
			bthalStatus = BTHAL_MM_FreeDataBuf(mp3Packet->descriptor); 
			BTL_VERIFY_ERR_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed to free data buffer"));
#endif /* HCI_QOS == XA_ENABLED */

			if (btlA2dpData.pullDataState & BTL_A2DP_PULL_DATA_STATE_MASK_PULL_ACTIVE)
			{
				btlA2dpData.pullDataState |= BTL_A2DP_PULL_DATA_STATE_MASK_PULL_ON_HOLD;
		}
		}
		else	/* MP3 packet was sent */
		{

			/* Calculate the next timestamp */
	        if (mp3Packet->frameOffset == 0) 
			{
	            mediaHeader->marker = 0;
	            lastTimeStamp = mediaHeader->timestamp;

	            switch (btlA2dpData.a2dpContext->mpeg1_2_audioLayer) 
				{
	            case (A2DP_MP3_CODEC_LAYER_1):
	                mediaHeader->timestamp += 26 * 90000 / 1000;
	                break;
	            case (A2DP_MP3_CODEC_LAYER_2):
	            case (A2DP_MP3_CODEC_LAYER_3):
	                mediaHeader->timestamp += 1152 * 90000 / (btlA2dpData.a2dpContext->mpeg1_2_audioSampleFreq);
	                break;
	            }
	    
	            if (lastTimeStamp > mediaHeader->timestamp)
				{
	                mediaHeader->marker = 1;
	            }
	        }
			
	        /* Set the next sequence number */
	        mediaHeader->sequenceNumber++;
#if A2DP_USE_TIMER == XA_ENABLED
			playTime += btlA2dpCalcMp3PlayTime(mp3Packet);
#endif	/* A2DP_USE_TIMER == XA_ENABLED */
    	}

#if A2DP_USE_TIMER == XA_ENABLED
		break;
#endif	/* A2DP_USE_TIMER == XA_ENABLED */

}

	return (playTime);
}

#endif


#if SBC_ENCODER == XA_ENABLED
/*-------------------------------------------------------------------------------
 * btlA2dpGetPcmBytesPerSbcFrame()
 *
 *		calculate the size of a PCM block needed to encode an SBC frame
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		streamInfo [in] - the corrent SBC stream
 *
 * Returns:
 *		the size in bytes
 *
 */
static U16 btlA2dpGetPcmBytesPerSbcFrame (SbcStreamInfo *streamInfo)
{
	U8 numChannels;

	if (streamInfo->channelMode == SBC_CHNL_MODE_MONO)
		numChannels = 1;
	else
		numChannels = 2;
		
	return  (U16) ( numChannels * streamInfo->numSubBands * streamInfo->numBlocks * 2);
}

/*-------------------------------------------------------------------------------
 * btlA2dpLimitBitPoolToSbcLen()
 *
 *		decrement bitpool as needed in case the size of the PCM block results in
 *		a too large SBC packet
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		numSbcFrames [in] - how many SBC frames are going to be encoded from the PCM block
 *
 * Returns:
 *		the SBC frame length after updating bitpool
 *
 */
static U16 btlA2dpLimitBitPoolToSbcLen(U8 numSbcFrames)
{
	U16 sbcFrameLen;
	U8 prevBitPool = sbcEncoder.streamInfo.bitPool;

	/* 	Assumption !
		we need to decrement bitpool only when current bitpool is way above minimum allowed bitpool */
	while ( ((sbcFrameLen = SBC_FrameLen(&sbcEncoder.streamInfo)) * 
								numSbcFrames) > BTL_A2DP_SBC_MAX_DATA_SIZE)
	{
		sbcEncoder.streamInfo.bitPool--;
		sbcEncoder.streamInfo.lastSbcFrameLen = 0;
	}

	if (prevBitPool != sbcEncoder.streamInfo.bitPool)
	{
		BTL_LOG_INFO(("btlA2dpLimitBitPoolToSbcLen from %d to %d, FrameLen=%d", 
			prevBitPool,  sbcEncoder.streamInfo.bitPool, sbcFrameLen));
	}

	BTL_VERIFY_FATAL_NORET(
		(sbcEncoder.streamInfo.bitPool >= btlA2dpData.selMinBitPool), 
		("size of PCM block requires bitpool below the minimum bitpool"));

	return sbcFrameLen;	
}

/*-------------------------------------------------------------------------------
 * btlA2dpHandleBlockFractions()
 *
 *		handle PCM blocks which are not a whole number of SBC frames
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		rawBlock [in] - new raw PCM block received from the MM
 *		pcmData [out] - chuncks of PCM blocks holding whole number of SBC frames each
 * 		nSbcFrames [out] - how many SBC frames we can encode this time
 *
 * Returns:
 *		how many PCM chunks we have ready (can be 0, 1 or 2)
 *
 */
U8 btlA2dpHandlePcmBlockFractions(BtlA2dpRawBlock *rawBlock, SbcPcmData pcmData[], U8 *nSbcFrames)
{
	U16 totalPcmBytes = (U16) (btlA2dpData.numRemainingPcmBytes + rawBlock->dataLength);
	U8 numSbcFrames =		/* how many SBC frames we can encode now */
		(U8) (totalPcmBytes / btlA2dpData.pcmBytesPerSbcFrame);
	U16 remainingBytesForNextTime =
		 (U16) (totalPcmBytes % btlA2dpData.pcmBytesPerSbcFrame);
	U8 	numPcmChunks = 1, mainChunckIdx = 0; /* PCM data chuncks */
	U16 numUsedToFillPrevFrame = 0;

	if ( numSbcFrames == 0 )
	{	/* i.e. we don't have enough PCM samples to encode even one SBC frame */
		OS_MemCopy(&(btlA2dpData.remainingPcmBuf[btlA2dpData.numRemainingPcmBytes]), 
			rawBlock->data, rawBlock->dataLength); /* save the samples for next time */
		btlA2dpData.numRemainingPcmBytes = (U16) (btlA2dpData.numRemainingPcmBytes + rawBlock->dataLength);
		BTL_LOG_DEBUG(("Not enough PCM samples to encode an SBC frame (%d < %d)",
			btlA2dpData.numRemainingPcmBytes, btlA2dpData.pcmBytesPerSbcFrame));
		numPcmChunks = 0;
	}
	else
	{
		if ( btlA2dpData.numRemainingPcmBytes > 0 )
		{
			numUsedToFillPrevFrame = (U16) (btlA2dpData.pcmBytesPerSbcFrame - btlA2dpData.numRemainingPcmBytes);
			OS_MemCopy(&(btlA2dpData.remainingPcmBuf[btlA2dpData.numRemainingPcmBytes]), 
				rawBlock->data, numUsedToFillPrevFrame);
			pcmData[0].dataLen = btlA2dpData.pcmBytesPerSbcFrame;
			pcmData[0].data = &(btlA2dpData.remainingPcmBuf[0]);
			pcmData[0].sampleFreq = sbcEncoder.streamInfo.sampleFreq;
			pcmData[0].numChannels = sbcEncoder.streamInfo.numChannels;
			mainChunckIdx = 1;
			if ( numSbcFrames > 1)
				numPcmChunks = 2;
		}

		if ( (btlA2dpData.numRemainingPcmBytes == 0) || (numSbcFrames >= 2) )
		{
			pcmData[mainChunckIdx].dataLen = (U16) (rawBlock->dataLength
							- numUsedToFillPrevFrame - remainingBytesForNextTime);
			pcmData[mainChunckIdx].data = &(rawBlock->data[numUsedToFillPrevFrame]);
			pcmData[mainChunckIdx].sampleFreq = sbcEncoder.streamInfo.sampleFreq;
			pcmData[mainChunckIdx].numChannels = sbcEncoder.streamInfo.numChannels;
		}

	}

	btlA2dpData.numRemainingPcmBytes  = remainingBytesForNextTime;

	*nSbcFrames = numSbcFrames;
	return numPcmChunks;
}

/*-------------------------------------------------------------------------------
 * btlA2dpProcessPcmBlock()
 *
 *		Process PCM block.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		rawBlock
 *
 * Returns:
 *		BOOL
 */
static BOOL btlA2dpProcessPcmBlock(BtlA2dpRawBlock *rawBlock)
{
	SbcPcmData pcmData[2];
	BtlA2dpSbcPacket *sbcPacket = 0;
	U16 BytesEncoded;
	BthalStatus bthalStatus;
	U32 idx;
	XaStatus sbcEncoderStatus = XA_STATUS_SUCCESS;
	U8 	chunckIdx, numPcmChunks = 1; /* PCM data chuncks */
	U16 totSbcLen = 0;
	U8 numSbcFrames;
	BOOL retVal = TRUE;

#if (BTL_CONFIG_A2DP_SAMPLE_RATE_CONVERTER == BTL_CONFIG_ENABLED)

	if (btlA2dpData.srcConvertPCM == TRUE)
	{
		/* Need to convert input PCM sampling frequency */
		
		BthalStatus srcRetVal;
		BTHAL_U16 outBlkLen;

		srcRetVal = SRC_Convert(rawBlock->data, 
								rawBlock->dataLength, 
								(U8*)btlA2dpData.outSrcPcmBlk,
								&outBlkLen);
		BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == srcRetVal), ("Failed to convert PCM block via SRC!"));
	
		/* Replace original PCM block with new converted PCM block */
		rawBlock->data = (U8*)btlA2dpData.outSrcPcmBlk;
		rawBlock->dataLength = outBlkLen;
	}

#endif	/* BTL_CONFIG_A2DP_SAMPLE_RATE_CONVERTER == BTL_CONFIG_ENABLED */
	
	numPcmChunks = btlA2dpHandlePcmBlockFractions(rawBlock, &pcmData [0], &numSbcFrames);

	if ( numPcmChunks > 0)
	{
	sbcPacket = btlA2dpGetSbcPacketFromPool();
	if (sbcPacket)
	{
			sbcPacket->packets[0].frameSize = btlA2dpLimitBitPoolToSbcLen(numSbcFrames);
		
		/* Allocate memory for data */
#if HCI_QOS == XA_ENABLED
		sbcPacket->packets[0].data = sbcPacket->sbcData;
#else
		sbcPacket->packets[0].data = sbcPacket->sbcData;
#endif /* HCI_QOS == XA_ENABLED */

			for ( chunckIdx = 0 ; chunckIdx < numPcmChunks ; chunckIdx++)
			{
				U16 tmpSbcLen;
		sbcEncoderStatus = SBC_EncodeFrames(&(sbcEncoder), 
								 					&(pcmData[chunckIdx]), &BytesEncoded, 
								 					&(sbcPacket->packets[0].data[totSbcLen]), 
								 					&tmpSbcLen,
	 							 					(U16) (BTL_A2DP_SBC_MAX_DATA_SIZE - totSbcLen) );
				totSbcLen = (U16) (totSbcLen + tmpSbcLen);
				BTL_VERIFY_FATAL_NORET((BytesEncoded == pcmData[chunckIdx].dataLen), ("SBC encoding failed"));
		BTL_VERIFY_FATAL_NORET((sbcEncoderStatus == XA_STATUS_SUCCESS), ("SBC encoding failed"));
			}
			sbcPacket->packets[0].dataLen = totSbcLen;

		if (sbcEncoderStatus == XA_STATUS_SUCCESS)
		{
			/* PCM packet was successfully encoded into SBC queue */
			btlA2dpInsertSbcPacketToQueue(sbcPacket);
			btlA2dpAdjustBitPool(BTL_A2DP_INC_BITPOOL_VAL);	/* increment bitpool */
		}
		else
		{
			/* Return SBC packet to pool */
			btlA2dpInsertSbcPacketToPool(sbcPacket);
		}
	}
	else
	{
		/* In case no free SBC packets are avaliable */

		/*   SBC streams come before MP3 streams in the streams array, 
			so we can count until BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT */
		for (idx = 0; idx<BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT; idx++)
		{
			/* No SBC packet available, adjust the time stamp */
				pcmData[0].dataLen = rawBlock->dataLength; /* following call needs numChannels and dataLength */
		    		A2DP_StreamIncrementTimeStamp(&(btlA2dpData.a2dpContext->streams[idx].stream), &(pcmData[0]));
		}
		btlA2dpAdjustBitPool(BTL_A2DP_DEC_BITPOOL_VAL);	/* decrement bitpool */

			BTL_LOG_ERROR(("Not enough SBC packets! (ProcessPcmBlock)"));

			retVal = FALSE;
		}

	if ( btlA2dpData.numRemainingPcmBytes > 0)
	{
		OS_MemCopy(&(btlA2dpData.remainingPcmBuf[0]), 
			&(rawBlock->data[rawBlock->dataLength-btlA2dpData.numRemainingPcmBytes]), 
			btlA2dpData.numRemainingPcmBytes);
	}
	}

	/* Notify MM data buffer can be free */
	bthalStatus = BTHAL_MM_FreeDataBuf(rawBlock->descriptor); 
	BTL_VERIFY_ERR_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed to free data buffer"));

	return (retVal);
}

#endif /* SBC_ENCODER == XA_ENABLED */

/*-------------------------------------------------------------------------------
 * btlA2dpProcessSbcBlock()
 *
 *		Process SBC block.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		rawBlock [in] - a block of SBC frames (generated by the exterfnal SBC encoder)
 *
 * Returns:
 *		BOOL
 */
static BOOL btlA2dpProcessSbcBlock(BtlA2dpRawBlock *rawBlock)
{
	BtlA2dpSbcPacket *sbcPacket = 0;
	BthalStatus bthalStatus;
	BOOL retVal = TRUE;

	sbcPacket = btlA2dpGetSbcPacketFromPool();
	if ( sbcPacket )
	{
		U16	frameSize = SBC_FrameLen(&btlA2dpData.sbcStreamInfo);

		sbcPacket->descriptor = rawBlock->descriptor;
#if HCI_QOS == XA_ENABLED
		sbcPacket ->packets[0].data		= sbcPacket ->sbcData;
		Assert(rawBlock->dataLength <= BTL_A2DP_SBC_MAX_DATA_SIZE);
		OS_MemCopy(sbcPacket ->packets[0].data, rawBlock->data, rawBlock->dataLength);
#else
		sbcPacket ->packets[0].data		= rawBlock->data;
#endif /* HCI_QOS == XA_ENABLED */
		sbcPacket ->packets[0].dataLen		= rawBlock->dataLength;
		sbcPacket ->packets[0].frameSize	= frameSize;
		btlA2dpInsertSbcPacketToQueue(sbcPacket);
	}
	else
	{
		/* In case no free SBC packets are avaliable */
		U32 idx;

		for (idx = 0; idx<BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT; idx++)
		{
			/* No SBC packet available, adjust the time stamp */
			/* following call should be replaced with soenthing that calculates num of "samples"
			 according to the SBC packet length */
	    	/*A2DP_StreamIncrementTimeStamp(&(btlA2dpData.a2dpContext->streams[idx].stream), &(pcmData[0]));*/
		}
		BTL_LOG_ERROR(("Not enough SBC packets! (ProcessSbcBlock)"));

#if HCI_QOS == XA_ENABLED
#else
		/* Notify MM data buffer can be free */
		bthalStatus = BTHAL_MM_FreeDataBuf(rawBlock->descriptor); 
		BTL_VERIFY_ERR_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed to free data buffer"));
#endif /* HCI_QOS == XA_ENABLED */

		retVal = FALSE;
	}

#if HCI_QOS == XA_ENABLED
	/* Notify MM data buffer can be free. we free the buffer in any case because we copied the contents to our private buffer */
	bthalStatus = BTHAL_MM_FreeDataBuf(rawBlock->descriptor); 
	BTL_VERIFY_ERR_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed to free data buffer"));
#endif /* HCI_QOS == XA_ENABLED */

	return (retVal);
}


#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

/*-------------------------------------------------------------------------------
 * btlA2dpProcessMpeg1_2_AudioBlock()
 *
 *		Process MPEG-1,2 Audio block.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		rawBlock
 *
 * Returns:
 *		BOOL
 */
static BOOL btlA2dpProcessMpeg1_2_AudioBlock(BtlA2dpRawBlock *rawBlock)
{
	BtlA2dpMpeg1_2_AudioPacket *mp3Packet = 0;
	BthalStatus bthalStatus;
	AvdtpMediaHeader *mediaHeader = &btlA2dpData.a2dpContext->mpeg1_2_audioMediaHeader;
	U16 headerLen;
	BtPacket *packet;
	BOOL retVal = TRUE;

	mp3Packet = btlA2dpGetMpeg1_2_AudioPacketFromPool();

	if (mp3Packet)
	{
		/* Prepare first MP3 packet */
		packet = &(mp3Packet->packets[0]);

		/* Setup the packet header */
#if HCI_QOS == XA_ENABLED
		packet->data		= mp3Packet->mp3Data;
		Assert(rawBlock->dataLength <= BTL_A2DP_SBC_MAX_DATA_SIZE);
		OS_MemCopy(packet->data, rawBlock->data, rawBlock->dataLength);
#else
		packet->data = rawBlock->data;
#endif /* HCI_QOS == XA_ENABLED */
		
		packet->dataLen = rawBlock->dataLength;
		packet->flags = BTP_FLAG_INUSE;		

		/* Create the RTP header */
    	headerLen = (U16)(A2DP_CreateMediaHeader(mediaHeader, &packet->header[BT_PACKET_HEADER_LEN - BTL_A2DP_MAX_MPEG1_2_AUDIO_HEADER_LENGTH]) + 4);

		packet->headerLen = (U8)headerLen;

		packet->header[BT_PACKET_HEADER_LEN - 4] = 0;
	    packet->header[BT_PACKET_HEADER_LEN - 3] = 0;

		StoreBE16(&packet->header[BT_PACKET_HEADER_LEN - 2], rawBlock->frameOffset);

		/* Save descriptor & frame offset */
		mp3Packet->descriptor = rawBlock->descriptor;
		mp3Packet->frameOffset = rawBlock->frameOffset;

		btlA2dpInsertMpeg1_2_AudioPacketToQueue(mp3Packet);
	}
	else
	{
		/* In case no free MP3 packets are avaliable */

		BTL_LOG_ERROR(("Not enough MP3 packets!"));

#if HCI_QOS == XA_ENABLED
#else
		/* Notify MM data buffer can be free */
		bthalStatus = BTHAL_MM_FreeDataBuf(rawBlock->descriptor); 
		BTL_VERIFY_ERR_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed to free data buffer"));
#endif /* HCI_QOS == XA_ENABLED */

		retVal = FALSE;
	}
	
#if HCI_QOS == XA_ENABLED
	/* Notify MM data buffer can be free */
	bthalStatus = BTHAL_MM_FreeDataBuf(rawBlock->descriptor); 
	BTL_VERIFY_ERR_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed to free data buffer"));
#endif /* HCI_QOS == XA_ENABLED */

	return (retVal);
}

#endif


#if	A2DP_USE_TIMER == XA_ENABLED
/*-------------------------------------------------------------------------------
 * btlA2dpCalcSbcPlayTime()
 *
 *		calculate how much playback time an SBC packet contains
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		sbcPacket [in] - the SBC packet
 *
 * Returns:
 *		playback time in tenths of ms
 *
 */
static U32 btlA2dpCalcSbcPlayTime(BtlA2dpSbcPacket *sbcPacket)
{
	U32 playTime;
	U32 byteRate; /* bytes per second */
	static U16 sampleFreqs[] = {16000, 32000, 44100, 48000};
	SbcStreamInfo *streamInfo = 
#if SBC_ENCODER == XA_ENABLED
						(BTHAL_MM_SBC_ENCODER_BUILT_IN == btlA2dpData.sbcEncoderLocation) ?
						&sbcEncoder.streamInfo :
#endif /* SBC_ENCODER == XA_ENABLED */
						&btlA2dpData.sbcStreamInfo;
	byteRate = sbcPacket->packets[0].frameSize * sampleFreqs[streamInfo->sampleFreq] / 
		(streamInfo->numSubBands * streamInfo->numBlocks);

	byteRate = BTL_A2DP_INCREASE_RATE(byteRate);

	playTime = sbcPacket->packets[0].dataLen * 1000 * BTL_A2DP_TIME_RESOLUTION / byteRate;

	return playTime; 
}

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0
/*-------------------------------------------------------------------------------
 * btlA2dpCalcMp3PlayTime()
 *
 *		calculate how much playback time an MP3 packet contains
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		mp3Packet [in] - the MP3 packet
 *
 * Returns:
 *		playback time in tenths of ms
 *
 */
static U32 btlA2dpCalcMp3PlayTime(BtlA2dpMpeg1_2_AudioPacket *mp3Packet)
{
	U32 playTime;
	U32 bitRate = btlA2dpData.a2dpContext->curBitRate;

	bitRate = BTL_A2DP_INCREASE_RATE(bitRate);

	playTime = 8 * mp3Packet->packets[0].dataLen * 1000 * BTL_A2DP_TIME_RESOLUTION / bitRate;

	return playTime; 
}
#endif

/*-------------------------------------------------------------------------------
 * btlA2dpCheckforStartSending()
 *
 *		check if the media queue has already enough accumulated playback time.
 *           if yes - trigger the timer to start sending packets
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		addPlayTime [in] - additional accumulated play time in tenths of ms
 *
 * Returns:
 *		void
 */
static void btlA2dpCheckforStartSending(U32 addPlayTime)
{
	btlA2dpData.accumulatedMediaTime += addPlayTime;

	BtlA2dpOsCancelTimer();
	if (btlA2dpData.accumulatedMediaTime > (BTL_A2DP_TIME_RESOLUTION*BTL_A2DP_PREBUFFER_BEFORE_SEND_THRESHOLD))
	{
		btlA2dpData.canSendFromQueue = TRUE;
		btlA2dpData.accumulatedMediaTime = 0; /* from now on counting how much we sent */
		/* we assume we start sending immediately (but it will hapen in the timer callback) */
		btlA2dpData.startSendingTime = TICKS_TO_MS(OS_GetSystemTime()) + 5;
		BtlA2dpOsResetTimer(5); /* when the timer will fire we wil start sending */
	}
	else
	{
		U32 delay = (addPlayTime/BTL_A2DP_TIME_RESOLUTION) * 15 / 10; /* set timer to 150% of current packet time */

		BtlA2dpOsResetTimer(delay); /* for case we will not receive enough to pass the threshold */
	}
}
#endif	/* A2DP_USE_TIMER == XA_ENABLED */


/*-------------------------------------------------------------------------------
 * btlA2dpInsertSbcPacketToQueue()
 *
 *		Insert sbc packet to sbcPacketQueue
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		sbcPacket [in] - SBC packet
 *
 * Returns:
 *		void
 */
static void btlA2dpInsertSbcPacketToQueue(BtlA2dpSbcPacket *sbcPacket)
{
	InsertTailList(&btlA2dpData.a2dpContext->sbcPacketQueue, &(sbcPacket->node));
	
#if	A2DP_USE_TIMER == XA_ENABLED
	if ( ! btlA2dpData.canSendFromQueue )
	{
		 btlA2dpCheckforStartSending(btlA2dpCalcSbcPlayTime(sbcPacket));
	}
#endif	/* A2DP_USE_TIMER == XA_ENABLED */
}


/*-------------------------------------------------------------------------------
 * btlA2dpGetSbcPacketFromQueue()
 *
 *		Get sbc packet from sbcPacketQueue
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		None
 *
 * Returns:
 *		BtlA2dpSbcPacket pointer
 */
static BtlA2dpSbcPacket *btlA2dpGetSbcPacketFromQueue(void)
{
	BtlA2dpSbcPacket *sbcPacket = 0;

	if (!IsListEmpty(&btlA2dpData.a2dpContext->sbcPacketQueue))
	{
		sbcPacket = (BtlA2dpSbcPacket *)RemoveHeadList(&(btlA2dpData.a2dpContext->sbcPacketQueue));
	}
	
	return sbcPacket;
}


/*-------------------------------------------------------------------------------
 * btlA2dpInsertRawBlockToQueue()
 *
 *		Insert raw block to rawBlockQueue
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		rawBlock [in] - Raw block
 *
 * Returns:
 *		void
 */
static void btlA2dpInsertRawBlockToQueue(BtlA2dpRawBlock *rawBlock)
{
	BthalStatus bthalStatus;

	bthalStatus = BTHAL_OS_LockSemaphore(btlA2dpData.a2dpMmSemHandle, 0);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed locking A2DP MM semaphore"));

	InsertTailList(&btlA2dpData.a2dpContext->rawBlockQueue, &(rawBlock->node));
	
	bthalStatus = BTHAL_OS_UnlockSemaphore(btlA2dpData.a2dpMmSemHandle);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed unlocking A2DP MM semaphore"));
}


/*-------------------------------------------------------------------------------
 * btlA2dpGetRawBlockFromQueue()
 *
 *		Get raw block from rawBlockQueue
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		None
 *
 * Returns:
 *		BtlA2dpRawBlock pointer
 */
static BtlA2dpRawBlock *btlA2dpGetRawBlockFromQueue(void)
{
	BtlA2dpRawBlock *rawBlock = 0;
	BthalStatus bthalStatus;

	bthalStatus = BTHAL_OS_LockSemaphore(btlA2dpData.a2dpMmSemHandle, 0);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed locking A2DP MM semaphore"));

	if (!IsListEmpty(&btlA2dpData.a2dpContext->rawBlockQueue))
	{
		rawBlock = (BtlA2dpRawBlock *)RemoveHeadList(&(btlA2dpData.a2dpContext->rawBlockQueue));
	}
	
	bthalStatus = BTHAL_OS_UnlockSemaphore(btlA2dpData.a2dpMmSemHandle);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed unlocking A2DP MM semaphore"));

	return rawBlock;
}


/*-------------------------------------------------------------------------------
 * btlA2dpInsertRawBlockToPool()
 *
 *		Insert raw block to rawBlockPool
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		rawBlock [in] - raw block
 *
 * Returns:
 *		void
 */
static void btlA2dpInsertRawBlockToPool(BtlA2dpRawBlock *rawBlock)
{
	BthalStatus bthalStatus;
	
	bthalStatus = BTHAL_OS_LockSemaphore(btlA2dpData.a2dpMmSemHandle, 0);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed locking A2DP MM semaphore!"));

	InsertTailList(&btlA2dpData.a2dpContext->rawBlockPool, &(rawBlock->node));

	btlA2dpData.a2dpContext->freeRawBlockCounter++;

	bthalStatus = BTHAL_OS_UnlockSemaphore(btlA2dpData.a2dpMmSemHandle);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed unlocking A2DP MM semaphore!"));
}


/*-------------------------------------------------------------------------------
 * btlA2dpGetRawBlockFromPool()
 *
 *		Get raw block from the rawBlockPool
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		None
 *
 * Returns:
 *		BtlA2dpRawBlock pointer
 */
static BtlA2dpRawBlock *btlA2dpGetRawBlockFromPool(void)
{
	BtlA2dpRawBlock *rawBlock = 0;
	BthalStatus bthalStatus;

	bthalStatus = BTHAL_OS_LockSemaphore(btlA2dpData.a2dpMmSemHandle, 0);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed locking A2DP MM semaphore"));

	if (!IsListEmpty(&btlA2dpData.a2dpContext->rawBlockPool))
	{
		rawBlock = (BtlA2dpRawBlock *)RemoveHeadList(&(btlA2dpData.a2dpContext->rawBlockPool));

		btlA2dpData.a2dpContext->freeRawBlockCounter--;
	}
	
	bthalStatus = BTHAL_OS_UnlockSemaphore(btlA2dpData.a2dpMmSemHandle);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed unlocking A2DP << semaphore"));
	
	return rawBlock;
}


/*-------------------------------------------------------------------------------
 * btlA2dpGetSbcPacketFromPool()
 *
 *		Get SBC packet from sbcPacketPool
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		None
 *
 * Returns:
 *		BtlA2dpSbcPacket pointer
 */
static BtlA2dpSbcPacket *btlA2dpGetSbcPacketFromPool(void)
{
	BtlA2dpSbcPacket *sbcPacket = 0;
	BthalStatus bthalStatus;

	bthalStatus = BTHAL_OS_LockSemaphore(btlA2dpData.a2dpStackSemHandle, 0);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed locking A2DP Stack semaphore"));

	if (!IsListEmpty(&btlA2dpData.a2dpContext->sbcPacketPool))
	{
		sbcPacket = (BtlA2dpSbcPacket *)RemoveHeadList(&(btlA2dpData.a2dpContext->sbcPacketPool));
		btlA2dpData.a2dpContext->usedPacketsCounter++;
	}
	
	bthalStatus = BTHAL_OS_UnlockSemaphore(btlA2dpData.a2dpStackSemHandle);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed unlocking A2DP Stack semaphore"));
	
	return sbcPacket;
}


/*-------------------------------------------------------------------------------
 * btlA2dpInsertSbcPacketToPool()
 *
 *		Insert sbc packet to sbcPacketPool
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		sbcPacket - sbc packet
 *
 * Returns:
 *		void
 */
static void btlA2dpInsertSbcPacketToPool(BtlA2dpSbcPacket *sbcPacket)
{
	BthalStatus bthalStatus;

	bthalStatus = BTHAL_OS_LockSemaphore(btlA2dpData.a2dpStackSemHandle, 0);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed locking A2DP Stack semaphore"));

	InsertTailList(&btlA2dpData.a2dpContext->sbcPacketPool, &(sbcPacket->node));
	btlA2dpData.a2dpContext->usedPacketsCounter--;
	
	bthalStatus = BTHAL_OS_UnlockSemaphore(btlA2dpData.a2dpStackSemHandle);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed unlocking A2DP Stack semaphore"));
}


#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

/*-------------------------------------------------------------------------------
 * btlA2dpGetMpeg1_2_AudioPacketFromPool()
 *
 *		Get MP3 packet from mpeg1_2_audioPacketPool
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		None
 *
 * Returns:
 *		BtlA2dpMpeg1_2_AudioPacket pointer
 */
static BtlA2dpMpeg1_2_AudioPacket *btlA2dpGetMpeg1_2_AudioPacketFromPool(void)
{
	BtlA2dpMpeg1_2_AudioPacket *mp3Packet = 0;
	BthalStatus bthalStatus;

	bthalStatus = BTHAL_OS_LockSemaphore(btlA2dpData.a2dpStackSemHandle, 0);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed locking A2DP Stack semaphore"));

	if (!IsListEmpty(&btlA2dpData.a2dpContext->mpeg1_2_audioPacketPool))
	{
		mp3Packet = (BtlA2dpMpeg1_2_AudioPacket *)RemoveHeadList(&(btlA2dpData.a2dpContext->mpeg1_2_audioPacketPool));
		btlA2dpData.a2dpContext->usedPacketsCounter++;
	}
	
	bthalStatus = BTHAL_OS_UnlockSemaphore(btlA2dpData.a2dpStackSemHandle);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed unlocking A2DP Stack semaphore"));
	
	return mp3Packet;
}


/*-------------------------------------------------------------------------------
 * btlA2dpInsertMpeg1_2_AudioPacketToPool()
 *
 *		Insert MP3 packet to mpeg1_2_audioPacketPool
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		BtlA2dpMpeg1_2_AudioPacket - MP3 packet
 *
 * Returns:
 *		void
 */
static void btlA2dpInsertMpeg1_2_AudioPacketToPool(BtlA2dpMpeg1_2_AudioPacket *mp3Packet)
{
	BthalStatus bthalStatus;

	bthalStatus = BTHAL_OS_LockSemaphore(btlA2dpData.a2dpStackSemHandle, 0);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed locking A2DP Stack semaphore"));

	InsertTailList(&btlA2dpData.a2dpContext->mpeg1_2_audioPacketPool, &(mp3Packet->node));
	btlA2dpData.a2dpContext->usedPacketsCounter--;
	
	bthalStatus = BTHAL_OS_UnlockSemaphore(btlA2dpData.a2dpStackSemHandle);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed unlocking A2DP Stack semaphore"));
}


/*-------------------------------------------------------------------------------
 * btlA2dpInsertMpeg1_2_AudioPacketToQueue()
 *
 *		Insert MP3 packet to mpeg1_2_audioPacketQueue
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		mp3Packet [in] - MP3 packet
 *
 * Returns:
 *		void
 */
static void btlA2dpInsertMpeg1_2_AudioPacketToQueue(BtlA2dpMpeg1_2_AudioPacket *mp3Packet)
{
	InsertTailList(&btlA2dpData.a2dpContext->mpeg1_2_audioPacketQueue, &(mp3Packet->node));

#if	A2DP_USE_TIMER == XA_ENABLED
	if ( ! btlA2dpData.canSendFromQueue )
	{
		 btlA2dpCheckforStartSending(btlA2dpCalcMp3PlayTime(mp3Packet));
	}
#endif	/* A2DP_USE_TIMER == XA_ENABLED */
}


/*-------------------------------------------------------------------------------
 * btlA2dpGetMpeg1_2_AudioPacketFromQueue()
 *
 *		Get MP3 packet from mpeg1_2_audioPacketQueue
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		None
 *
 * Returns:
 *		BtlA2dpMpeg1_2_AudioPacket pointer
 */
static BtlA2dpMpeg1_2_AudioPacket *btlA2dpGetMpeg1_2_AudioPacketFromQueue(void)
{
	BtlA2dpMpeg1_2_AudioPacket *mp3Packet = 0;

	if (!IsListEmpty(&btlA2dpData.a2dpContext->mpeg1_2_audioPacketQueue))
	{
		mp3Packet = (BtlA2dpMpeg1_2_AudioPacket *)RemoveHeadList(&(btlA2dpData.a2dpContext->mpeg1_2_audioPacketQueue));
	}
	
	return mp3Packet;
}

#endif


/*-------------------------------------------------------------------------------
 * btlA2dpHandleOsEventDataSent()
 *
 *		Handle A2DP_OS_EVENT_DATA_SENT event
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		None
 *
 * Returns:
 *		void
 */
static void btlA2dpHandleOsEventDataSent(void)
{
	BthalStatus bthalStatus;
	BthalMmDataBuffer buffer;
	BtlA2dpRawBlock *rawBlock = 0;
	U16 rawBlockCounter;
	BOOL contProcess = TRUE;	/* Check whether more free SBC / MP3 packets are avaliable */

	/* Try to pull next data packet only if MM indicated it */
	if (btlA2dpData.pullDataState & BTL_A2DP_PULL_DATA_STATE_MASK_PULL_ACTIVE)
	{
		S32 freePacketCounter = 
			A2DP_CUR_MAX_NUM_PACKETS_PER_CONTEXT - btlA2dpData.a2dpContext->usedPacketsCounter;
		do {
			rawBlockCounter = 0;
			while ( (btlA2dpData.a2dpContext->freeRawBlockCounter > 0) && (freePacketCounter-- > 0) )
		{
	/* In Pull mode request more data from MM (the memory for the data will be allocated by the MM) */
	bthalStatus = BTHAL_MM_RequestMoreData(&buffer);

	if (BTHAL_STATUS_SUCCESS == bthalStatus)
	{
		/* Sync process  - get the data from MM and insert to rawBlockQueue*/
		rawBlock = btlA2dpGetRawBlockFromPool();
		BTL_VERIFY_FATAL_NORET((rawBlock != 0), ("No blocks avaliable at RawBlockPool"));

		rawBlock->descriptor = buffer.descriptor;
		rawBlock->data = buffer.data;
		rawBlock->dataLength = buffer.dataLength;
		rawBlock->streamType = buffer.streamType;
#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0
				rawBlock->frameOffset = buffer.frameOffset;
#endif

			/* First insert raw block to Q, since there might be waiting raw blocks in Q already */
		btlA2dpInsertRawBlockToQueue(rawBlock);

				rawBlockCounter++;
			}
			else if (BTHAL_STATUS_PENDING == bthalStatus)
			{
				/* Data will arrive via the MM callback event BTHAL_MM_EVENT_DATA_BUFFER_IND */
				break;
			}
			else	/* BTHAL_STATUS_FAILED */
			{
				/* request data from MM failed */
				/* If all raw blocks are in pool, then the pull is on hold */
				if (btlA2dpData.a2dpContext->freeRawBlockCounter == BTL_CONFIG_A2DP_MAX_NUM_RAW_BLOCKS_PER_CONTEXT)
				{
					btlA2dpData.pullDataState |= BTL_A2DP_PULL_DATA_STATE_MASK_PULL_ON_HOLD;
				}

				break;
			}
		}

		if (rawBlockCounter > 0)
		{
		/* Get raw block from the rawBlockQueue */
				while ((contProcess == TRUE) && (rawBlock = btlA2dpGetRawBlockFromQueue()) != 0)
		{
					contProcess = btlA2dpData.processRawBlockFuncTable[rawBlock->streamType](rawBlock);

			/* Return the rawBlock to the pool */
			btlA2dpInsertRawBlockToPool(rawBlock);
		}

#if	A2DP_USE_TIMER == XA_ENABLED
			if (btlA2dpData.canSendFromQueue)
				btlA2dpHandleTimedProcessQueue();
#else
			/* Assumption: Only one active codec type! */ 
			btlA2dpData.processQueueFuncTable[btlA2dpData.currentCodecType]();
#endif 	/* A2DP_USE_TIMER == XA_ENABLED */

			} /* rawBlockCounter > 0 */
		} while ((rawBlockCounter > 0) && 
			((freePacketCounter = (A2DP_CUR_MAX_NUM_PACKETS_PER_CONTEXT - btlA2dpData.a2dpContext->usedPacketsCounter)) > 0));
	}
}

/*-------------------------------------------------------------------------------
 * btlA2dpCheckAndReconfigStream()
 *
 *		Handle A2DP_OS_EVENT_CONFIG_IND event. For each existing stream check
 *		weather the configuration is different from the MM requested one.
 *		If so reconfig stream
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		None
 *
 * Returns:
 *		void   
 */
static void btlA2dpCheckAndReconfigStream(void)
{
	A2dpStreamState streamState;
	AvdtpCodec *codec = 0;
	AvdtpCodec selectedCodec;
	BtStatus status;
	U32 idx;
	U8 selectedElements[MAX_NUM_OF_BYTES_SPECIFIC_INFO_ELEMENTS];
	BtlA2dpStream *btlStream;
	A2dpCallbackParms Info;

	BTL_FUNC_START("btlA2dpCheckAndReconfigStream");

	selectedCodec.elemLen = sizeof(selectedElements);
	selectedCodec.elements = selectedElements; 

	for(idx = 0; idx < A2DP_MAX_NUM_STREAMS_PER_CONTEXT; idx++)
    {
    	btlStream = &(btlA2dpData.a2dpContext->streams[idx]);
		
		/* Get stream state */
	 	streamState = A2DP_GetStreamState(&(btlStream->stream));

	    if ((streamState == A2DP_STREAM_STATE_OPEN) || (streamState == A2DP_STREAM_STATE_STREAMING))
        {
        	if (btlStream->remoteCodecValid == TRUE)
    		{
    			/* Select new configuration according to the local, remote and requested codec */
				status = BtlA2dpSelectCodec(btlStream, &selectedCodec);

				if (BT_STATUS_SUCCESS == status)
				{
					/* Get the configured codec */
					codec = A2DP_GetConfiguredCodec(&(btlStream->stream));
					BTL_VERIFY_FATAL_NORET((codec != 0), ("A2DP_GetConfiguredCodec failed"));

					/* See if this stream need to be reconfigured */
					if (OS_MemCmp((const U8 *)(codec->elements), 
						MAX_NUM_OF_BYTES_SPECIFIC_INFO_ELEMENTS, 
						(const U8 *)(selectedCodec.elements), 
						MAX_NUM_OF_BYTES_SPECIFIC_INFO_ELEMENTS) == FALSE)
					{
						if (streamState == A2DP_STREAM_STATE_OPEN)
						{
							BTL_LOG_INFO(("Source reconfig codec for stream %d:", idx));
							BtlA2dpPrintCodecInfo(&selectedCodec);

							/* reconfig the stream */
							status = A2DP_ReconfigStream(&(btlStream->stream), &selectedCodec, 0);
							if(status != BT_STATUS_PENDING)
							{
								BTL_LOG_DEBUG(("Warning: Stream reconfiguration failed"));
								btlStream->btlA2dpReconfigurationIndNeeded = TRUE;
							}
							else
							{
								btlStream->reconfigStat |= BTL_A2DP_RECONFIG_STATE_MASK_IN_RECONFIG;
							}
						}
						else	/* A2DP_STREAM_STATE_STREAMING */
						{
							/* Update app that the stream need to be configured */
							btlA2dpData.event.a2dpContext = btlA2dpData.a2dpContext;
							btlA2dpData.event.streamId = btlStream->streamId;
							btlA2dpData.event.callbackParms = &Info;
							btlA2dpData.event.callbackParms->event = A2DP_EVENT_STREAM_CONFIGURATION_NEEDED;
							btlA2dpData.event.callbackParms->status = BT_STATUS_SUCCESS;
							btlA2dpData.event.callbackParms->error = A2DP_ERR_NO_ERROR;
								
							btlStream->btlA2dpReconfigurationIndNeeded = FALSE;
								
							/* Pass the event to app */
							btlA2dpData.a2dpContext->callback(&btlA2dpData.event);
						}
					}
					else if (streamState == A2DP_STREAM_STATE_OPEN)
					{
						/* Codec is configured */
						btlA2dpCodecConfigured(btlStream, codec, idx);
					}
				}
				else
				{
					BTL_LOG_ERROR(("Select codec failed!"));
				}
    		}
			else if (streamState == A2DP_STREAM_STATE_OPEN)
			{
				/* First get remote capabilities */
				status = A2DP_GetStreamCapabilities(&(btlStream->stream));
				BTL_VERIFY_ERR_NORET((status == BT_STATUS_PENDING), ("A2DP_GetStreamCapabilities failed"));
			}
        }
    }

	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * btlA2dpUpdateMaxMediaPacketSize()
 *
 *		Update max media packet size among all open streams.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void
 *
 * Returns:
 *		void   
 */
void btlA2dpUpdateMaxMediaPacketSize(void)
{
	U32 idx;
	BtlA2dpStream *btlStream;
	U16 maxPacketSize;

	BTL_FUNC_START("btlA2dpUpdateMaxMediaPacketSize");

	/* Update max media packet size */
	btlA2dpData.a2dpContext->maxPacketSize = L2CAP_DEFAULT_MTU;
	maxPacketSize = 0xFFFF;

	/* Set the MIN media packet size among all connected streams */
	for(idx = 0; idx < A2DP_MAX_NUM_STREAMS_PER_CONTEXT; idx++)
    {
    	btlStream = &(btlA2dpData.a2dpContext->streams[idx]);

	    if ((btlStream->state == BTL_A2DP_STREAM_STATE_CONNECTED) && 
			((btlStream->stream.stream.state == AVDTP_STRM_STATE_OPEN) || 
			(btlStream->stream.stream.state == AVDTP_STRM_STATE_STREAMING)))
        {
		U16 mediaPacketSize = A2DP_MediaPacketSize(&(btlStream->stream));
		maxPacketSize = (U16)(min(maxPacketSize, mediaPacketSize));
        }
    }


	if (maxPacketSize != 0xFFFF)
	{
		btlA2dpData.a2dpContext->maxPacketSize = maxPacketSize;
		BTL_LOG_INFO(("new maxPacketSize = %d", maxPacketSize));
	}

	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * btlA2dpCodecConfigured()
 *
 *		Updates the configured codec params.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Codec [in] - selecetd codec parameters
 *
 *		btlStream [in] - the stream
 *
 * Returns:
 *		void   
 */
static void btlA2dpCodecConfigured(BtlA2dpStream *btlStream, AvdtpCodec *Codec, BtlA2dpStreamId streamId)
{
	BthalMmConfigParams codecInfo;
	BthalStatus bthalStatus;
	U16 packetSize, sbcFrameLen = 0;
	A2dpCallbackParms Info;
	BTL_FUNC_START("btlA2dpCodecConfigured");

	UNUSED_PARAMETER(btlStream);

#if SBC_ENCODER == XA_ENABLED

	if (BTHAL_MM_SBC_ENCODER_BUILT_IN == btlA2dpData.sbcEncoderLocation)
	{
		SbcStreamInfo *Info = &sbcEncoder.streamInfo;
	if (btlStream->localCodec.codecType == AVDTP_CODEC_TYPE_SBC)
	{
			/* Init the SBC encoder, including lastSbcFrameLen field */
		SBC_InitEncoder(&sbcEncoder);
		
		BtlA2dpSbcConvertAvdtp2Btstack(Codec, Info);
	    /* Choose  bitpool */
	    Info->bitPool = btlA2dpSelectBitpool(Info, Codec->elements[2], Codec->elements[3]);
	    btlA2dpData.pcmBytesPerSbcFrame = btlA2dpGetPcmBytesPerSbcFrame (Info);
		
		sbcFrameLen = SBC_FrameLen(&sbcEncoder.streamInfo);

#if (BTL_CONFIG_A2DP_SAMPLE_RATE_CONVERTER == BTL_CONFIG_ENABLED)
			SRC_ResetSongHistory();
#endif /* BTL_CONFIG_A2DP_SAMPLE_RATE_CONVERTER == BTL_CONFIG_ENABLED */

	}
	}

#endif /* SBC_ENCODER == XA_ENABLED */
	
	/* Translate configured codec into BTHAL MM codec struct */
	BtlA2dpConvertAvdtp2Bthal(Codec, &codecInfo);

	packetSize = btlA2dpData.a2dpContext->maxPacketSize;
#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

	/* With MP3 packets, the max media size is minus the 16 bytes header length */
	if (Codec->codecType == AVDTP_CODEC_TYPE_MPEG1_2_AUDIO)
		packetSize -= BTL_A2DP_MAX_MPEG1_2_AUDIO_HEADER_LENGTH;
	else
#endif
		packetSize -= BTL_A2DP_MEDIA_HEADER_LENGTH;

	/* Update BTHAL MM with the configured codec */
	bthalStatus = BTHAL_MM_CodecInfoConfiguredInd(&codecInfo, packetSize, sbcFrameLen, streamId);
	BTL_VERIFY_FATAL_NORET((BTHAL_STATUS_SUCCESS == bthalStatus), ("Failed to update BTHAL MM with selected codec"));

	/* Update the app that the stream is properly configured */
	btlA2dpData.event.a2dpContext = btlA2dpData.a2dpContext;
	btlA2dpData.event.streamId = streamId;
	btlA2dpData.event.callbackParms = &Info;
	btlA2dpData.event.callbackParms->event = A2DP_EVENT_STREAM_CONFIGURED;
	btlA2dpData.event.callbackParms->status = BT_STATUS_SUCCESS;
	btlA2dpData.event.callbackParms->error = A2DP_ERR_NO_ERROR;
	btlA2dpData.event.callbackParms->p.codec = Codec;
		
	btlStream->btlA2dpReconfigurationIndNeeded = FALSE;
		
	/* Pass the event to app */
	btlA2dpData.a2dpContext->callback(&btlA2dpData.event);

	BTL_FUNC_END();
}


#if SBC_ENCODER == XA_ENABLED

/*-------------------------------------------------------------------------------
 * btlA2dpSelectBitpool()
 *
 *		select the initial bitpool for the SBC encoder. the selection is done
 *		according to the recommended values in the profile spec, subject to
 *		the feasible bitpool range of the SRC and SNK.
 *
 *		if varyBittPool is FALSE then this bitpool value shal remain valid
 *		until stream is stopped or reconfigured.
 *		else the bitpool will dynamically change in the duration of the streaming.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		streamInfo [in] - the sbc stream info used by the encoder
 *
 *		minBitPool [in] - the minimum allowed bitpool
 *
 *		maxnBitPool [in] - the maximum allowed bitpool
 *
 */
static U8 btlA2dpSelectBitpool(SbcStreamInfo *streamInfo, U8 minBitPool, U8 maxBitPool)
{
	U8 ret, desiredBitpool;

	static U8 desiredBitpoolTable[2][2][2] =
	{
		{ 	/* middle quality*/
				/* channles = 1 */
				{  19 /* all other freqs */, 18 /* freq 48 */},
				/* channles = 2 */
				{  35 /* all other freqs */, 33 /* freq 48 */},
		},
		
		{ 	/* high quality*/
				/* channles = 1 */
				{  31 /* all other freqs */, 29 /* freq 48 */},
				/* channles = 2 */
				{  53 /* all other freqs */, 51 /* freq 48 */},
		}
			
	};
	
	BOOL 	highQuality	= (btlA2dpData.audioQuality == BTHAL_MM_SBC_AUDIO_QUALITY_HIGH);
	BOOL	twoChannels	= (streamInfo->numChannels == 2);
	BOOL	freq48		= (streamInfo->sampleFreq == SBC_CHNL_SAMPLE_FREQ_48);

	desiredBitpool = desiredBitpoolTable[highQuality][twoChannels][freq48];

	if ( desiredBitpool >  maxBitPool )
	{
		ret = maxBitPool;
	}
	else 
	if ( desiredBitpool <  minBitPool )
	{
		ret = minBitPool;
	}
	else
	{
		ret = desiredBitpool;
	}
		
	BTL_LOG_INFO(("btlA2dpSelectBitpool(freq=%d, channels=%d, quality = %s) return %d (desired = %d)",
		streamInfo->sampleFreq, streamInfo->numChannels, 
		btlA2dpData.audioQuality == BTHAL_MM_SBC_AUDIO_QUALITY_HIGH ? 
		"High" : "Middle" , ret, desiredBitpool));

	return ret;
	
}

/*-------------------------------------------------------------------------------
 * btlA2dpAdjustBitPool()
 *
 *		Dynamically adjust the current bitbool value of the SBC encoder, up or down
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		interval [in] - the amount of units to increment / decrement from bitpool
 *
 */
static void btlA2dpAdjustBitPool(S16 interval)
{
	if ( btlA2dpData.varyBitPool )
	{
		U32 curTickTime = OS_GetSystemTime();
		BOOL decBitpool = FALSE;

		if (interval < 0)
		{
			/* Count the number of decrement bitpool occurrences */
			btlA2dpData.decBpOccurrencesCounter++;

			if (btlA2dpData.decBpOccurrencesCounter >= BTL_A2DP_DEC_BITPOOL_OCCURRENCES)
			{
				decBitpool = TRUE;
				btlA2dpData.decBpOccurrencesCounter = 0;
			}
		}
		
		if (	(interval < 0) || 		/* either if we need to decrement */
								/* or if enough time has elapse to increment */
			(curTickTime >  (btlA2dpData.lastBpUpd + BTL_A2DP_INC_BITPOOL_TICKS )))
		{
			if ((interval > 0) || (decBitpool == TRUE))
			{
			S16 newVal = (U16) (sbcEncoder.streamInfo.bitPool + interval);
					
			if ( newVal >  btlA2dpData.selMaxBitPool )
			{
				newVal =  btlA2dpData.selMaxBitPool;
			}
			if ( newVal <  btlA2dpData.selMinBitPool )
			{
				newVal =  btlA2dpData.selMinBitPool;
			}

			if (sbcEncoder.streamInfo.bitPool != newVal)
			BTL_LOG_INFO(("btlA2dpAdjustBitPool(%d) from %d to %d", 
				interval,  sbcEncoder.streamInfo.bitPool, newVal));

			sbcEncoder.streamInfo.bitPool = (U8) newVal;

				/* bit pool was changed, frame length needs to be calculated again */
				sbcEncoder.streamInfo.lastSbcFrameLen = 0;
			}
			
			btlA2dpData.lastBpUpd = curTickTime;
		}
	}
}

#endif /* SBC_ENCODER == XA_ENABLED */


#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

/*-------------------------------------------------------------------------------
 * btlA2dpIsStreamOpenAllowed()
 *
 *		Returns TRUE if the given stream is allowed to be opened.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		btlStream [in] - the stream.
 *
 * Returns:
 *		True if the given stream open is allowed.
 *
 */
static BOOL btlA2dpIsStreamOpenAllowed(BtlA2dpStream *btlStream)
{
	U32 idx, startIdx, endIdx;
	BOOL retVal = TRUE;

	BTL_FUNC_START("btlA2dpIsStreamOpenAllowed");

	if (btlStream->localCodec.codecType == AVDTP_CODEC_TYPE_SBC)
	{
		startIdx = BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT;
		endIdx = A2DP_MAX_NUM_STREAMS_PER_CONTEXT;
	}
	else	/* AVDTP_CODEC_TYPE_MPEG1_2_AUDIO */
	{
		startIdx = 0;
		endIdx = BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT;
	}

	/* Look for different codec type which is open already */
	for(idx = startIdx; idx < endIdx; idx++)
    {
    	btlStream = &(btlA2dpData.a2dpContext->streams[idx]);

	    if ((btlStream->state == BTL_A2DP_STREAM_STATE_CONNECTED) && 
			((btlStream->stream.stream.state == AVDTP_STRM_STATE_OPEN) || 
			(btlStream->stream.stream.state == AVDTP_STRM_STATE_STREAMING)))
        {
			retVal = FALSE;
			break;
        }
    }

	BTL_FUNC_END();

	return (retVal);
}

#endif	/* BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0 */


/*-------------------------------------------------------------------------------
 * BtlA2dpCodecType()
 *
 *		Return a pointer to a description of the codec type.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		codecType [in] - Codec type.
 *
 * Returns:
 *		ASCII String pointer.
 */
static const char *BtlA2dpCodecType(AvdtpCodecType codecType)
{
    switch (codecType) 
	{
    case AVDTP_CODEC_TYPE_SBC:
        return "SBC";
    case AVDTP_CODEC_TYPE_MPEG1_2_AUDIO:
        return "MPEG-1, 2 Audio";
    case AVDTP_CODEC_TYPE_MPEG2_4_AAC:
        return "MPEG-2, 4 Audio";
    case AVDTP_CODEC_TYPE_ATRAC:
        return "ATRAC";
    case AVDTP_CODEC_TYPE_H263:
        return "H263";
	case AVDTP_CODEC_TYPE_NON_A2DP:
        return "Non A2DP";
    }
    return "UNKNOWN";
}


/*-------------------------------------------------------------------------------
 * BtlA2dpPrintCodecInfo()
 *
 *		Internal function for printing CODEC info.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		codec [in] - pointer to the codec.
 *
 * Returns:
 *		void.
 */
static void BtlA2dpPrintCodecInfo(AvdtpCodec *codec)
{
	BTL_LOG_INFO(("Codec type: %s.", BtlA2dpCodecType(codec->codecType)));

	switch (codec->codecType)
	{
		case (AVDTP_CODEC_TYPE_SBC):
		{
			const char* freq16 = ((codec->elements[0] & A2DP_SBC_CODEC_FREQ_16000) ? ("16000") : (""));
			const char* freq32 = ((codec->elements[0] & A2DP_SBC_CODEC_FREQ_32000) ? ("32000") : (""));
			const char* freq44_1 = ((codec->elements[0] & A2DP_SBC_CODEC_FREQ_44100) ? ("44100") : (""));
			const char* freq48 = ((codec->elements[0] & A2DP_SBC_CODEC_FREQ_48000) ? ("48000") : (""));

			const char* modeMono = ((codec->elements[0] & A2DP_SBC_CODEC_CHNL_MODE_MONO) ? ("MONO") : (""));
			const char* modeDual = ((codec->elements[0] & A2DP_SBC_CODEC_CHNL_MODE_DUAL) ? ("DUAL CHANNEL") : (""));
			const char* modeStereo = ((codec->elements[0] & A2DP_SBC_CODEC_CHNL_MODE_STEREO) ? ("STEREO") : (""));
			const char* modeJoint = ((codec->elements[0] & A2DP_SBC_CODEC_CHNL_MODE_JOINT) ? ("JOINT STEREO") : (""));

			const char* block4 = ((codec->elements[1] & A2DP_SBC_CODEC_BLOCKS_4) ? ("4") : (""));
			const char* block8 = ((codec->elements[1] & A2DP_SBC_CODEC_BLOCKS_8) ? ("8") : (""));
			const char* block12 = ((codec->elements[1] & A2DP_SBC_CODEC_BLOCKS_12) ? ("12") : (""));
			const char* block16 = ((codec->elements[1] & A2DP_SBC_CODEC_BLOCKS_16) ? ("16") : (""));

			const char* subband4 = ((codec->elements[1] & A2DP_SBC_CODEC_SUBBANDS_4) ? ("4") : (""));
			const char* subband8 = ((codec->elements[1] & A2DP_SBC_CODEC_SUBBANDS_8) ? ("8") : (""));
			
			const char* allocSnr = ((codec->elements[1] & A2DP_SBC_CODEC_ALLOCATION_SNR) ? ("SNR") : (""));
			const char* allocLoud = ((codec->elements[1] & A2DP_SBC_CODEC_ALLOCATION_LOUDNESS) ? ("LOUDNESS") : (""));
			
			BTL_LOG_INFO(("Sampling Frequency [Hz]: %s, %s, %s, %s.", freq16, freq32, freq44_1, freq48));
			BTL_LOG_INFO(("Channel Mode: %s, %s, %s, %s.", modeMono, modeDual, modeStereo, modeJoint));
			BTL_LOG_INFO(("Block Length: %s, %s, %s, %s.", block4, block8, block12, block16));
			BTL_LOG_INFO(("Number of Subbands: %s, %s .", subband4, subband8));
			BTL_LOG_INFO(("Allocation Method: %s, %s ", allocSnr, allocLoud));
			BTL_LOG_INFO(("Minimum Bitpool Value: %d ", codec->elements[2]));
			BTL_LOG_INFO(("Maximum Bitpool Value: %d ", codec->elements[3]));
			break;
		}

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 0

		case (AVDTP_CODEC_TYPE_MPEG1_2_AUDIO):
		{
			const char* layer1 = ((codec->elements[0] & A2DP_MP3_CODEC_LAYER_1) ? ("Layer I ( mp1)") : (""));
			const char* layer2 = ((codec->elements[0] & A2DP_MP3_CODEC_LAYER_2) ? ("Layer II ( mp2)") : (""));
			const char* layer3 = ((codec->elements[0] & A2DP_MP3_CODEC_LAYER_3) ? ("Layer III ( mp3)") : (""));

			const char* crc = ((codec->elements[0] & A2DP_MP3_CODEC_CRC_SUPPORTED) ? ("Supported") : ("Not supported"));

			const char* modeMono = ((codec->elements[0] & A2DP_MP3_CODEC_MODE_MONO) ? ("MONO") : (""));
			const char* modeDual = ((codec->elements[0] & A2DP_MP3_CODEC_MODE_DUAL_CHNL) ? ("DUAL CHANNEL") : (""));
			const char* modeStereo = ((codec->elements[0] & A2DP_MP3_CODEC_MODE_STEREO) ? ("STEREO") : (""));
			const char* modeJoint = ((codec->elements[0] & A2DP_MP3_CODEC_MODE_JOINT) ? ("JOINT STEREO") : (""));

			const char* mpf2 = ((codec->elements[1] & A2DP_MP3_CODEC_MPF2) ? ("Supported") : ("Not supported"));

			const char* freq16 = ((codec->elements[1] & A2DP_MP3_CODEC_FREQ_16000) ? ("16000") : (""));
			const char* freq22_05 = ((codec->elements[1] & A2DP_MP3_CODEC_FREQ_22050) ? ("22050") : (""));
			const char* freq24 = ((codec->elements[1] & A2DP_MP3_CODEC_FREQ_24000) ? ("24000") : (""));
			const char* freq32 = ((codec->elements[1] & A2DP_MP3_CODEC_FREQ_32000) ? ("32000") : (""));
			const char* freq44_1 = ((codec->elements[1] & A2DP_MP3_CODEC_FREQ_44100) ? ("44100") : (""));
			const char* freq48 = ((codec->elements[1] & A2DP_MP3_CODEC_FREQ_48000) ? ("48000") : (""));

			U16 octet2_3 = (U16)(((((U16)(codec->elements[2])) << 8) | ((U16)codec->elements[3])));

			const char* vbr = ((octet2_3 & A2DP_MP3_CODEC_VBR) ? ("Supported") : ("Not supported"));

			const char* index14 = ((octet2_3 & A2DP_MP3_CODEC_BITRATE_INDEX_14) ? ("14") : (""));
			const char* index13 = ((octet2_3 & A2DP_MP3_CODEC_BITRATE_INDEX_13) ? ("13") : (""));
			const char* index12 = ((octet2_3 & A2DP_MP3_CODEC_BITRATE_INDEX_12) ? ("12") : (""));
			const char* index11 = ((octet2_3 & A2DP_MP3_CODEC_BITRATE_INDEX_11) ? ("11") : (""));
			const char* index10 = ((octet2_3 & A2DP_MP3_CODEC_BITRATE_INDEX_10) ? ("10") : (""));
			const char* index9 = ((octet2_3 & A2DP_MP3_CODEC_BITRATE_INDEX_9) ? ("9") : (""));
			const char* index8 = ((octet2_3 & A2DP_MP3_CODEC_BITRATE_INDEX_8) ? ("8") : (""));
			const char* index7 = ((octet2_3 & A2DP_MP3_CODEC_BITRATE_INDEX_7) ? ("7") : (""));
			const char* index6 = ((octet2_3 & A2DP_MP3_CODEC_BITRATE_INDEX_6) ? ("6") : (""));
			const char* index5 = ((octet2_3 & A2DP_MP3_CODEC_BITRATE_INDEX_5) ? ("5") : (""));
			const char* index4 = ((octet2_3 & A2DP_MP3_CODEC_BITRATE_INDEX_4) ? ("4") : (""));
			const char* index3 = ((octet2_3 & A2DP_MP3_CODEC_BITRATE_INDEX_3) ? ("3") : (""));
			const char* index2 = ((octet2_3 & A2DP_MP3_CODEC_BITRATE_INDEX_2) ? ("2") : (""));
			const char* index1 = ((octet2_3 & A2DP_MP3_CODEC_BITRATE_INDEX_1) ? ("1") : (""));
			const char* index0 = ((octet2_3 & A2DP_MP3_CODEC_BITRATE_INDEX_0) ? ("0") : (""));
			
			BTL_LOG_INFO(("Layer: %s, %s, %s.", layer1, layer2, layer3));
			BTL_LOG_INFO(("CRC Protection: %s.", crc));
			BTL_LOG_INFO(("Channel Mode: %s, %s, %s, %s.", modeMono, modeDual, modeStereo, modeJoint));
			BTL_LOG_INFO(("MPF2: %s.", mpf2));
			BTL_LOG_INFO(("Sampling Frequency [Hz]: %s, %s, %s, %s, %s, %s.", freq16, freq22_05, freq24, freq32, freq44_1, freq48));
			BTL_LOG_INFO(("VBR: %s.", vbr));
			BTL_LOG_INFO(("Bit Rate Index: %s, %s, %s, %s, %s.", index14, index13, index12, index11, index10));
			BTL_LOG_INFO(("Bit Rate Index Cont': %s, %s, %s, %s, %s.", index9, index8, index7, index6, index5));
			BTL_LOG_INFO(("Bit Rate Index Cont': %s, %s, %s, %s, %s.", index4, index3, index2, index1, index0));
			break;
		}

#endif

	}
}


BtStatus BtlA2dpBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 idx;
	BtlA2dpContext *a2dpContext;

	BTL_FUNC_START("BtlA2dpBtlNotificationsCb");
	
	BTL_LOG_DEBUG(("Notification Type: %d", notificationType));

	/* Assumption: We have only one context */
	a2dpContext = btlA2dpData.a2dpContext;

	if (a2dpContext)
	{
		switch (notificationType)
		{
			case BTL_MODULE_NOTIFICATION_RADIO_ON:

				/* Do nothing */
			
				break;

			case BTL_MODULE_NOTIFICATION_RADIO_OFF:

				switch (a2dpContext->state)
					{
						case BTL_A2DP_STATE_ENABLED:

							for (idx = 0; idx<A2DP_MAX_NUM_STREAMS_PER_CONTEXT; idx++)
							{
								switch (a2dpContext->streams[idx].state)
								{
									case (BTL_A2DP_STREAM_STATE_DISCONNECTING):
									{
										/* A2DP state is now in the process of disabling */
										a2dpContext->state = BTL_A2DP_STATE_DISABLING;
										break;
									}
									case (BTL_A2DP_STREAM_STATE_CONNECTED):
									{	
										status = A2DP_CloseStream(&(a2dpContext->streams[idx].stream));
										if (BT_STATUS_PENDING == status)
										{
											a2dpContext->streams[idx].state = BTL_A2DP_STREAM_STATE_DISCONNECTING;
											
											/* A2DP state is now in the process of disabling */
											a2dpContext->state = BTL_A2DP_STATE_DISABLING;
										}
										
										break;
									}
									case (BTL_A2DP_STREAM_STATE_CONNECTING):
									{
										/* In process of connecting, let it finish and then close the stream */

										/* A2DP state is now in the process of disabling */
										a2dpContext->state = BTL_A2DP_STATE_DISABLING;
										break;
									}
									case (BTL_A2DP_STREAM_STATE_CONNECT_IND):
									{
										/* Reject incoming open request, and wait for close event */

										status = A2DP_OpenStreamRsp(&(a2dpContext->streams[idx].stream), A2DP_ERR_BAD_SERVICE, AVDTP_SRV_CAT_MEDIA_TRANSPORT);
										BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), ("Failed responding to open indication while disabling."));

										if (BT_STATUS_PENDING == status)
											a2dpContext->state = BTL_A2DP_STATE_DISABLING;
										
										break;
									}
								}
							}

							if (BTL_A2DP_STATE_DISABLING != a2dpContext->state)
							{
								/* All streams are disconnected */
								break;
							}
							
							/* Remember that context was enabled */
							a2dpContext->disableState |= BTL_A2DP_DISABLE_STATE_MASK_ENABLED;
							
							/* Wait for disconnect event, pass through to next case... */

						case BTL_A2DP_STATE_DISABLING:
							
							a2dpContext->disableState |= BTL_A2DP_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE;
							status = BT_STATUS_PENDING;
							break;
					}
				
				break;

			default:

				BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid notification (%d)", notificationType));
				
		}
	}

	BTL_FUNC_END();
	
	return (status);
}


#if 0
/*-------------------------------------------------------------------------------
 * BtlA2dpDisableRoleSwitch()
 *
 *		change baseband  policies to disable role switch onincoming and outgoing connections
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		none
 *
 * Returns:
 *		BT_STATUS_SUCCESS upon success, else BT_STATUS_FAILED
 */
static BtStatus BtlA2dpDisableRoleSwitch()
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtLinkPolicy inACL, outACL;

	BTL_FUNC_START("BtlA2dpDisableRoleSwitch");
	
	/* request to always be a master, and to disable role switch in outgoing connections */
	inACL = MEC(inAclPolicy);
	outACL = MEC(outAclPolicy);
	outACL &= ~BLP_MASTER_SLAVE_SWITCH; /* disable role switch on outgoing connections */
	inACL   &= ~BLP_MASTER_SLAVE_SWITCH; /* disable role switch on incoming connections */
	status = ME_SetDefaultLinkPolicy(	inACL, outACL);
	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, status, 
					("ME_SetDefaultLinkPolicy Failed, Status = %s", pBT_Status(status))); 	
	ME_SetConnectionRole(BCR_MASTER);
	
	BTL_FUNC_END();
	
	return (status);
}
#endif

/*-------------------------------------------------------------------------------
 * btlA2dpSwitchToMaster()
 *
 *		try to switch a stream to master role in case we are slave
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Stream [in] - the A2DP stream
 *
 * Returns:
 *		none
 */
static void btlA2dpSwitchToMaster(A2dpStream *Stream)
{
	BtRemoteDevice *remDev  = A2DP_GetRemoteDevice(Stream);

	if ( remDev && (ME_GetCurrentRole(remDev) != BCR_MASTER) )
	{
		BtStatus status = ME_SwitchRole(remDev);
		BTL_LOG_INFO(("ME_SwitchRole returned %s", pBT_Status(status)));
	}
	else
		BTL_LOG_INFO(("btlA2dpSwitchToMaster - already a master"));
		
}


static void btlA2dpSendHciCommandCB(const BtEvent *Event)
{
	BTL_FUNC_START("btlA2dpSendHciCommandCB");
	
	if (Event->p.meToken->p.general.out.status == BT_STATUS_SUCCESS)
	{
		BTL_LOG_DEBUG(("HCI command 0x%x completed", 
			Event->p.meToken->p.general.in.hciCommand));	
	}

	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * btlA2dpSendHciCommand()
 *
 *		send a command to HCI
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		hciCommand [in] - the HCI comamnd
 *
 *           parmsLen [in] - length in bytes of the parameter buffer
 *
 *           parms [in] - the parameter buffer
 *
 * Returns:
 *		BT_STATUS_PENDING - the command was successfully sent,
 *           elase various failures
 */
static BtStatus btlA2dpSendHciCommand(	
									U16 	hciCommand,
									U8 		parmsLen, 
									U8 		*parms
									)
{
	BtStatus 				status = BT_STATUS_SUCCESS;
	MeCommandToken		*token;
	int 					tokIdx;
	U8					event;

	BTL_FUNC_START("btlA2dpSendHciCommand");

	switch ( hciCommand )
	{
		case HCC_WRITE_LINK_POLICY:
			tokIdx = 0;
			event = HCE_COMMAND_COMPLETE;
			break;
		case HCC_FLOW_SPECIFICATION:
			tokIdx = 1;
			event = HCE_FLOW_SPECIFICATION_COMPLETE;
			break;
		default:
			BTL_ERR(BT_STATUS_INVALID_PARM, ("invalid HCI command 0x%x", hciCommand));
	}
	token = &btlA2dpData.hciCmdToken[tokIdx];
	
	OS_MemSet((U8*) token, 0, sizeof(MeCommandToken));
	
	/* Set HCI Command parameters */
	token->callback = btlA2dpSendHciCommandCB;
	token->p.general.in.hciCommand = hciCommand;
	token->p.general.in.parmLen = parmsLen;
	token->p.general.in.parms = parms;
	token->p.general.in.event = event;

	/* Send the HCI command */
	status = ME_SendHciCommandSync(token);	

	BTL_VERIFY_ERR((BT_STATUS_PENDING == status),
			status, ("ME_SendHciCommandSync Failed, Status = %s", pBT_Status(status)));

	BTL_FUNC_END();
	
	return status;

}
 

/*-------------------------------------------------------------------------------
 * btlA2dpEnableBasebandQos()
 *
 *		activate BB QOS in favor of a stream by calling the HCI "Flow Specification Command"
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Stream [in] - the A2DP stream
 *
 * Returns:
 *		none
 */			
static void btlA2dpEnableBasebandQos(A2dpStream *Stream)
{
	/* following parameters are according to Version 2.0 + EDR BT spec */
	static U8 flowSpecificationParms[21] =
	{
		0x00, 0x00,				/* Connection_Handle  	- fill it below */
		0x00,					/* Flags 				- Reserved */
		0x00,					/* Flow_direction		- Outgoing Flow i.e. traffic send over the ACL connection */
		BQST_GUARANTEED, 		/* Service_Type 		- 0x02 Guaranteed ! */
		0x10, 0x00, 0x00, 0x00, 	/*  Token Rate 		- set to 0x00000010 */
		0x10, 0x00, 0x00, 0x00, 	/*  Peak_Bandwidth 	- set to 0x00000010 */
		0x10, 0x00, 0x00, 0x00, 	/*  Token Bucket Size 	- set to 0x00000010 */
		0x00, 0x00, 0x00, 0x00, 	/*  Access Latency 	- fill it below */
	};

	BtRemoteDevice	*remDev;
	U16				connectionHandle;
	BtStatus			status;

	BTL_FUNC_START("btlA2dpEnableBasebandQos");

	remDev  = A2DP_GetRemoteDevice(Stream);

	if (remDev )
	{
		connectionHandle = ME_GetHciConnectionHandle(remDev);

 		StoreLE16(&flowSpecificationParms[0], connectionHandle);

		/* Access Latency is in microseconds - set to 4,000 . 20,000 is the default */
		StoreLE32(&flowSpecificationParms[17], 4000); 

		status = 	btlA2dpSendHciCommand( HCC_FLOW_SPECIFICATION, 
				sizeof(flowSpecificationParms), (U8*) flowSpecificationParms );
	}

	BTL_FUNC_END();

}

#if BTL_A2DP_ALLOW_ROLE_SWITCH == BTL_CONFIG_ENABLED
#else
/*-------------------------------------------------------------------------------
 * btlA2dpDisableRoleSwitchInLinkPolicies()
 *
 *		disable role switch on the stream by calling the HCI "Write Link Policy Settings Command"
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Stream [in] - the A2DP stream
 *
 * Returns:
 *		none
 */
static void btlA2dpDisableRoleSwitchInLinkPolicies(A2dpStream *Stream)
{
	static U8 writeLinkPolicyParms[4];
	BtRemoteDevice	*remDev;
	U16				connectionHandle;
	BtStatus			status;

	BTL_FUNC_START("btlA2dpDisableRoleSwitchInLinkPolicies");

	remDev  = A2DP_GetRemoteDevice(Stream);

	if (remDev )
	{
		BtLinkPolicy linkPolicySettings = (U16) (MEC(inAclPolicy) | MEC(outAclPolicy));
		linkPolicySettings &= ~BLP_MASTER_SLAVE_SWITCH; /* disable role switch */

		connectionHandle = ME_GetHciConnectionHandle(remDev);

 		StoreLE16(&writeLinkPolicyParms[0], connectionHandle);

		StoreLE16(&writeLinkPolicyParms[2], linkPolicySettings); 

		status = 	btlA2dpSendHciCommand( HCC_WRITE_LINK_POLICY, 
				sizeof(writeLinkPolicyParms), (U8*) writeLinkPolicyParms );
	}

	BTL_FUNC_END();

}
#endif /* BTL_A2DP_ALLOW_ROLE_SWITCH */


/*-------------------------------------------------------------------------------
 * BtlA2dpSendEventToBsc()
 *
 *		Sends event to BTL_BSC module.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		eventType [in] - type of event to be sent.
 *
 *      btlStream [in] - pointer to BtlA2dpStream structure.
 *
 * Returns:
 *		none
 */
static void BtlA2dpSendEventToBsc(BtlBscEventType eventType,
                                  BtlA2dpStream *btlStream,
                                  BtlA2dpStreamId streamId)
{
    BtlBscEvent bscEvent;

	BTL_FUNC_START("BtlA2dpSendEventToBsc");

    /* Fill event's data */
    bscEvent.type = eventType;
    bscEvent.source = BTL_BSC_EVENT_SOURCE_A2DP;

    if (NULL != btlStream)
    {
        bscEvent.info.bdAddr = btlStream->stream.device->channel.conn.remDev->bdAddr;
        bscEvent.isBdAddrValid = TRUE;
    }
    else
    {
        bscEvent.isBdAddrValid = FALSE;
    }
    
    bscEvent.info.channel = streamId;

    BTL_BSC_SendEvent((const BtlBscEvent *)&bscEvent);

	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * BtlA2dpReportTransportChannelState()
 *
 *		Report (if needed) the current transport channel state to BTHAL_MM module.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *      btlStream [in] - pointer to BtlA2dpStream structure.
 *
 *		state [in] - current state of transport channel.
 *
 * Returns:
 *		none
 */
static void BtlA2dpReportTransportChannelState(BtlA2dpStream *btlStream, 
												BthalMmTransportChannelState state)
{
	BTL_FUNC_START("BtlA2dpReportTransportChannelState");

    if (btlStream->lastReportedState != state)
	{
		btlStream->lastReportedState = state;
		
		BTHAL_MM_TransportChannelStateChanged((BTHAL_U32)btlStream->streamId, state);
	}

	BTL_FUNC_END();
}


#if HCI_QOS == XA_ENABLED
/*-------------------------------------------------------------------------------
 * isSendingAllowed()
 *
 *		This function checks if send packets on a stream is allowed.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		none
 *
 * Returns:
 *		TRUE - sending is allowed, else FALSE.
 */
static BOOL isSendingAllowed()
{
	U8 stream1, stream2;
	U8 maxNumOfSendPacketAllow  = (U8) (A2DP_CUR_MAX_NUM_PACKETS_PER_CONTEXT / A2DP_CUR_MAX_NUM_STREAMS_PER_CONTEXT);
	BOOL retVal = TRUE;
	S8 numSent1, numSent2;
	/* BOOL error = FALSE; */
	BtlA2dpStream *pStream1, *pStream2;
	
	/* Assume max of 2 streams */
#if BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT > 2
#error BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT must be smaller than 3
#endif

#if BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT > 2
#error BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT must be smaller than 3
#endif

	if (btlA2dpData.currentCodecType == BTHAL_MM_STREAM_TYPE_SBC)
	{
		stream1 = 0;
	}
	else /* mp3 */
	{
		stream1 = BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT;
	}

	stream2 = (U8)(stream1 + 1);

	pStream1 =  &(btlA2dpData.a2dpContext->streams[stream1]);
	pStream2 =  &(btlA2dpData.a2dpContext->streams[stream2]);
	
	numSent1 = pStream1->numOfPacketsSent;
	numSent2 = pStream2->numOfPacketsSent;
	if( (numSent1 < maxNumOfSendPacketAllow) &&
		(numSent2 < maxNumOfSendPacketAllow) )
	{
		pStream1->isSendAllowed = TRUE;
		pStream2->isSendAllowed = TRUE;
	}
	else if( (numSent1 == maxNumOfSendPacketAllow) &&
			 (numSent2 == 0) )
	{
		/* not sending to first stream */
		pStream1->isSendAllowed = FALSE;
		pStream2->isSendAllowed = TRUE;
		/*error = TRUE;*/
	}
	else if( (numSent1 == 0) &&
			 (numSent2 == maxNumOfSendPacketAllow) )
	{
		/* not sending to second stream */
		pStream1->isSendAllowed = TRUE;
		pStream2->isSendAllowed = FALSE;
		/*error = TRUE;*/
	}
	else
	{
		/* not sending to BOTH streams */
		pStream1->isSendAllowed= FALSE;
		pStream2->isSendAllowed = FALSE;
		retVal = FALSE;
	}

/*
	if (error)
	{
		BTL_LOG_ERROR(("Discarding a frame to a stream. num sent  = %d %d", numSent1, numSent2));
	}
*/

	return (retVal);
}
#endif /* HCI_QOS == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            pAvError()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Prints A2DP error message
 *
 */
static char * pAvError(A2dpError Error)
{
    switch (Error) {
    case A2DP_ERR_NO_ERROR:
        return" A2DP_ERR_NO_ERROR";
    case A2DP_ERR_BAD_SERVICE:
        return" A2DP_ERR_BAD_SERVICE";
    case A2DP_ERR_INSUFFICIENT_RESOURCE:
        return" A2DP_ERR_INSUFFICIENT_RESOURCE";
    case A2DP_ERR_INVALID_CODEC_TYPE:
        return" A2DP_ERR_INVALID_CODEC_TYPE";
    case A2DP_ERR_NOT_SUPPORTED_CODEC_TYPE:
        return" A2DP_ERR_NOT_SUPPORTED_CODEC_TYPE";
    case A2DP_ERR_INVALID_SAMPLING_FREQUENCY:
        return" A2DP_ERR_INVALID_SAMPLING_FREQUENCY";
    case A2DP_ERR_NOT_SUPPORTED_SAMP_FREQ:
        return" A2DP_ERR_NOT_SUPPORTED_SAMP_FREQ";
    case A2DP_ERR_INVALID_CHANNEL_MODE:
        return" A2DP_ERR_INVALID_CHANNEL_MODE";
    case A2DP_ERR_NOT_SUPPORTED_CHANNEL_MODE:
        return" A2DP_ERR_NOT_SUPPORTED_CHANNEL_MODE";
    case A2DP_ERR_INVALID_SUBBANDS:
        return" A2DP_ERR_INVALID_SUBBANDS";
    case A2DP_ERR_NOT_SUPPORTED_SUBBANDS:
        return" A2DP_ERR_NOT_SUPPORTED_SUBBANDS";
    case A2DP_ERR_INVALID_ALLOCATION_METHOD:
        return" A2DP_ERR_INVALID_ALLOCATION_METHOD";
    case A2DP_ERR_NOT_SUPPORTED_ALLOC_METHOD:
        return" A2DP_ERR_NOT_SUPPORTED_ALLOC_METHOD";
    case A2DP_ERR_INVALID_MIN_BITPOOL_VALUE:
        return" A2DP_ERR_INVALID_MIN_BITPOOL_VALUE";
    case A2DP_ERR_NOT_SUPPORTED_MIN_BITPOOL_VALUE:
        return" A2DP_ERR_NOT_SUPPORTED_MIN_BITPOOL_VALUE";
    case A2DP_ERR_INVALID_MAX_BITPOOL_VALUE:
        return" A2DP_ERR_INVALID_MAX_BITPOOL_VALUE";
    case A2DP_ERR_NOT_SUPPORTED_MAX_BITPOOL_VALUE:
        return" A2DP_ERR_NOT_SUPPORTED_MAX_BITPOOL_VALUE";
    case A2DP_ERR_INVALID_LAYER:
        return" A2DP_ERR_INVALID_LAYER";
    case A2DP_ERR_NOT_SUPPORTED_LAYER:
        return" A2DP_ERR_NOT_SUPPORTED_LAYER";
    case A2DP_ERR_NOT_SUPPORTED_CRC:
        return" A2DP_ERR_NOT_SUPPORTED_CRC";
    case A2DP_ERR_NOT_SUPPORTED_MPF:
        return" A2DP_ERR_NOT_SUPPORTED_MPF";
    case A2DP_ERR_NOT_SUPPORTED_VBR:
        return" A2DP_ERR_NOT_SUPPORTED_VBR";
    case A2DP_ERR_INVALID_BIT_RATE:
        return" A2DP_ERR_INVALID_BIT_RATE";
    case A2DP_ERR_NOT_SUPPORTED_BIT_RATE:
        return" A2DP_ERR_NOT_SUPPORTED_BIT_RATE";
    case A2DP_ERR_INVALID_OBJECT_TYPE:
        return" A2DP_ERR_INVALID_OBJECT_TYPE";
    case A2DP_ERR_NOT_SUPPORTED_OBJECT_TYPE:
        return" A2DP_ERR_NOT_SUPPORTED_OBJECT_TYPE";
    case A2DP_ERR_INVALID_CHANNELS:
        return" A2DP_ERR_INVALID_CHANNELS";
    case A2DP_ERR_NOT_SUPPORTED_CHANNELS:
        return" A2DP_ERR_NOT_SUPPORTED_CHANNELS";
    case A2DP_ERR_INVALID_VERSION:
        return" A2DP_ERR_INVALID_VERSION";
    case A2DP_ERR_NOT_SUPPORTED_VERSION:
        return" A2DP_ERR_NOT_SUPPORTED_VERSION";
    case A2DP_ERR_NOT_SUPPORTED_MAXIMUM_SUL:
        return" A2DP_ERR_NOT_SUPPORTED_MAXIMUM_SUL";
    case A2DP_ERR_INVALID_BLOCK_LENGTH:
        return" A2DP_ERR_INVALID_BLOCK_LENGTH";
    case A2DP_ERR_INVALID_CP_TYPE:
        return" A2DP_ERR_INVALID_CP_TYPE";
		case A2DP_ERR_INVALID_CP_FORMAT:
        return" A2DP_ERR_INVALID_CP_FORMAT";
    default:
        return "A2DP_ERR_UNKNOWN_ERROR";
    }
}

 

#else /*BTL_CONFIG_A2DP ==   BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_A2DP_Init() - When  BTL_CONFIG_A2DP is disabled.
 */
BtStatus BTL_A2DP_Init(void)
{
    
   BTL_LOG_INFO(("BTL_A2DP_Init()  -  BTL_CONFIG_A2DP Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_A2DP_Deinit() - When  BTL_CONFIG_A2DP is disabled.
 */
BtStatus BTL_A2DP_Deinit(void)
{
    BTL_LOG_INFO(("BTL_A2DP_Deinit() -  BTL_CONFIG_A2DP Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /*BTL_CONFIG_A2DP ==   BTL_CONFIG_ENABLED*/



