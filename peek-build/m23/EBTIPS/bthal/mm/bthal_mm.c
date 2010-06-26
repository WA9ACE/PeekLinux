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
*   FILE NAME:      bthal_mm.c
*
*   DESCRIPTION:	This file defines the API of the BTHAL multimedia.
*
*   AUTHOR:         Keren Ferdman
*   AUTHOR:         Rene Kuiken 
*                   - Porting to Locosto
*                   - Added Pcm Simulation
*
\*******************************************************************************/

#include "btl_config.h"
#include "osapi.h"
#include "bthal_common.h"
#if BTL_CONFIG_A2DP ==   BTL_CONFIG_ENABLED
/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include <utils.h>
#include "bthal_mm.h"
#include "bthal_mm_debug.h"
#include "sys/debug.h"
#include "btl_log.h"
#include "Sbc.h"

#undef INLINE
#include "ffs/ffs_api.h"
#include "audio/audio_api.h"
#include "overide.h"


#if BTHAL_A2DP_MM_PCM_SUPPORT == BTL_CONFIG_ENABLED

#include "l1audio_btapi.h"
#include "mfw_bt_api.h"
void pcmCallbackFunc(L1AudioPcmBlock *pcmBlock);
void configCallbackFunc (L1AudioPcmConfig *pcmConfig);

#else

#define	L1Audio_RegisterBthal(pcmCallbackFunc, configCallbackFunc)
#define	L1Audio_InformBtAudioPathState(connected)

#endif

#undef L1_STUB

static const BtlLogModuleType btlLogModuleType = 13;

/* This constant enables reading PCM file directly from FFS - need to define also in a2dps_app.c file */
/* #define BTHAL_PCM_FILE */


/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * NUM_PCM_BLOCKS type
 *
 *     Audio playback constant
 */
#define NUM_PCM_BLOCKS								(2) /* L1 has two buffers (Ping Pong buffer) */

/*-------------------------------------------------------------------------------
 * NUM_DATA_BUFFERS 
 *
 *     number of elements in the data buffer list used in the pull mode
 
 */
#define NUM_DATA_BUFFERS							(10)


/*-------------------------------------------------------------------------------
 * bthalcodecAndFrameInfo type
 *
 *     SBC info
 */
typedef struct _bthalcodecAndFrameInfo 
{
  BTHAL_U8	bthalMmNumSubBands;
  BTHAL_U8	bthalMmNumBlocks;
  BTHAL_U16	bthalMmPacketSize;
  BTHAL_U16	bthalMmSbcFrameLen;
} bthalcodecAndFrameInfo;

#if SBC_ENCODER == XA_ENABLED

/*-------------------------------------------------------------------------------
 * bthalMmToLocostoMmParams type
 *
 *     Locostos multimedia parameters
 */
typedef struct _bthalMmToLocostoMmParams
{
	/* is MM init finished */
	BOOL										isOpen;

	/* is started was already called this flag will be TRUE */
	BOOL										isMmStarted;
	
	/* is stream 0 is in open state */
	BOOL										isStream0open;

	/* is stream 1 is in open state */
	BOOL										isStream1open;

	/* is stream 0 or 1 is in started state */
	BOOL										isStreamStarted[BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT];

	/* will be true if Locosto Mm initiated config for stream 0 */
	BOOL										isLocostoMmConfig0;

	/* will be true if Locosto Mm initiated config for stream 1 */
	BOOL										isLocostoMmConfig1;

   /* This call back is used to notify MM more data is needed */
//TODO  LocostoAudioStream_TransferStatusCallback 	requestMoreDataCB;
	
} bthalMmToLocostoMmParams;

#endif /*SBC_ENCODER == XA_ENABLED*/


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/
 
BthalMmCallback         	bthalMmCallback = NULL;
static BTHAL_U32			bthalMmInputDevice;
static BOOL emulatePullMode = FALSE;		/* whether to emulate pull mode */
static BTHAL_BOOL           audioInOpen = FALSE;
static BTHAL_BOOL			startAudio = FALSE;
static BOOL callbackTrigerred = FALSE; /* whether request PCM is called as a result of a PCM callback */
static L1AudioPcmBlock      tCBPcmBlock; /* The PCM block given in the CB */

/*-------------------------------------------------------------------------------
 * SelectedSbcCodec
 *
 *     Represents the selected cbs codec (one bit per capability)
 */
static BthalMmSbcInfo SelectedSbcCodec;

/*-------------------------------------------------------------------------------
 * bthalMmOperetionalMode
 *
 *     Represents the selected cbs codec (one bit per capability)
 */
static bthalcodecAndFrameInfo CodecAndFrameInfo;

/*-------------------------------------------------------------------------------
 * platformPcmSuportedCodec
 *
 *     Represents the PCM platform capabilities
 */
static BthalMmSbcInfo platformPcmSuportedCodec;


/*-------------------------------------------------------------------------------
 * platformMp3SuportedCodec
 *
 *     Represents the MP3 platform capabilities
 */
static BthalMmMpeg1_2_audioInfo platformMp3SuportedCodec;


/* PCM selected information */
static BthalMmPcmInfo selectedPcmInfo;

/* MP3 */


/*-------------------------------------------------------------------------------
 * NUM_MP3_BLOCKS type
 *
 *     Number of MP3 blocks.
 */
#define NUM_MP3_BLOCKS								(1)


#define MP3_MAX_READ_BUFFER  L2CAP_MTU
#define SBC_MAX_READ_BUFFER  1024

/*-------------------------------------------------------------------------------
 * NUM_SBC_BLOCKS type
 *
 *     Number of SBC blocks.
 */
#define NUM_SBC_BLOCKS								(3)

/*-------------------------------------------------------------------------------
 * SBC_FIRST_DESCRIPTOR, SBC_LAST_DESCRIPTOR 
 *
 *     descriptors of SBC blocks (rather than PCM or MP3 blocks)
 *
 */
 #define SBC_FIRST_DESCRIPTOR			(NUM_PCM_BLOCKS + NUM_MP3_BLOCKS)
 #define SBC_LAST_DESCRIPTOR			(SBC_FIRST_DESCRIPTOR + NUM_SBC_BLOCKS - 1)



/* MP3 bitrate tables */

static const U32 v1l1[16] = {
         0,  32000,  64000,  96000, 128000, 160000, 192000, 224000, 
    256000, 288000, 320000, 352000, 384000, 416000, 448000, 0
};

static const U32 v1l2[16] = {
         0,  32000,  48000,  56000,  64000,  80000,  96000, 112000, 
    128000, 160000, 192000, 224000, 256000, 320000, 384000, 0
};

static const U32 v1l3[16] = {
         0,  32000,  40000,  48000,  56000,  64000,  80000,  96000, 
    112000, 128000, 160000, 192000, 224000, 256000, 320000,  0
};

static const U32 v2l1[16] = {
         0,  32000,  48000,  56000,  64000,  80000,  96000, 112000, 
    128000, 144000, 160000, 176000, 192000, 224000, 256000, 0
};

static const U32 v2l23[16] = {
         0,   8000,  16000,  24000,  32000,  40000,  48000,  56000,  
     64000,  80000,  96000, 112000, 128000, 144000, 160000,  0
};


/* MP3 sample rate table (the 1 values in each raw are instead of zeros) */

static const U32 sampleRates[4][4] = {
   { 11025, 12000, 8000, 1 } ,		/* MP3_VERSION_25 */
   { 1, 1, 1, 1 } ,					/* reserved */
   { 22050, 24000, 16000, 1 } ,		/* MP3_VERSION_2 */
   { 44100, 48000, 32000, 1 }		/* MP3_VERSION_1 */
};


/*---------------------------------------------------------------------------
 * A2dpMp3Version type
 *     
 *     Defines the MP3 spec version as contained in an MP3 audio header.
 */
typedef U8 A2dpMp3Version;

#define MP3_VERSION_1  0x03
#define MP3_VERSION_2  0x02
#define MP3_VERSION_25 0x00

/* End of A2dpMp3Version */

/*---------------------------------------------------------------------------
 * A2dpMp3Layer type
 *     
 *     Defines the MP3 layer as contained in an MP3 audio header.
 */
typedef U8 A2dpMp3Layer;

#define MP3_LAYER_1    0x03
#define MP3_LAYER_2    0x02
#define MP3_LAYER_3    0x01

/* End of A2dpMp3Layer */

/*---------------------------------------------------------------------------
 * A2dpMp3ChannelMode type
 *     
 *     Defines the MP3 channel mode as contained in an MP3 audio header.
 */
typedef U8 A2dpMp3ChannelMode;

#define MP3_MODE_STEREO  0x00
#define MP3_MODE_JOINT   0x01
#define MP3_MODE_DUAL    0x02
#define MP3_MODE_MONO    0x03

/* End of A2dpMp3Mode */


/*---------------------------------------------------------------------------
 * A2DP_SyncSafetoHost32()
 *
 *    Converts a SynchSafe number used by MP3 to a 32 bit unsigned value.
 *
 * Parameters:
 *     ptr - pointer to memory that contains a 4 byte SynchSafe number.
 *
 * Returns:
 *     A 32 bit (U32) value.
 */
U32 A2dP_SyncSafetoHost32(U8 *ptr);
#define A2DP_SynchSafetoHost32(ptr)  (U32)( ((U32) *((U8*)(ptr))   << 21) | \
                                            ((U32) *((U8*)(ptr)+1) << 14) | \
                                            ((U32) *((U8*)(ptr)+2) << 7)  | \
                                            ((U32) *((U8*)(ptr)+3)) )


/*---------------------------------------------------------------------------
 * Mp3StreamInfo structure
 *
 * Contains MP3 audio stream information that can be optained from the frame
 * header.
 */
typedef struct _Mp3StreamInfo {
    A2dpMp3Version      version;     /* MPEG version                           */
    A2dpMp3Layer        layer;       /* MPEG audio layer                       */
    A2dpMp3ChannelMode  channelMode; /* Channel Mode                           */
    U8            		brIndex;     /* Bit rate index                         */
    U8            		srIndex;     /* Sample rate index                      */
    BOOL          		crc;         /* Specifies whether CRC protection is used
                                      *     0 specifies false
                                      *     A non-zero number specifies true
                                      */

    /* TODO: Put in an MP3 packet strucuture */
    U8            numFrames;   /* Number of frames in the current buffer */
    U16           bytesToSend; /* Bytes left to send                     */
    U16           offset;      /* Offset into current frame              */
	U16 		  frameLen;	   /* Frame length of the current frame 	 */
	U16			  totalDataLen;

} Mp3StreamInfo;


/*---------------------------------------------------------------------------
 * Mp3Block structure
 *
 * Contains MP3 audio data.
 */
typedef struct _bthalMp3Block 
{
	BOOL used;
	U8 mp3FrameBuffer[MP3_MAX_READ_BUFFER];
	
} bthalMp3Block;

/*---------------------------------------------------------------------------
 * SbcBlock structure
 *
 * Contains SBC audio data.
 */
typedef struct _bthalSbcBlock 
{
	BOOL used;
	U8 sbcFrameBuffer[SBC_MAX_READ_BUFFER];
	
} bthalSbcBlock;

#ifdef BTHAL_PCM_SIMULATOR
/*---------------------------------------------------------------------------
 * PcmBlock structure
 *
 * Contains Pcm audio data.
 */
#define MEM_BUF_SIZE			(512 * 1024)
#define WAV_FILE_DATA_OFFSET	(44)
#define PCM_BLOCK_SIZE			(7 *1024)

static U32	blockSize;
static U32	memoryBuf[(MEM_BUF_SIZE>>2)]; /* This trick makes sure the data is aligned to 32Bit */
static U8	*memIndex;
static U8	*pcmBufEnd;

#endif /* BTHAL_PCM_SIMULATOR */

/*-------------------------------------------------------------------------------
 * BthalRequestMoreData type
 *
 *     A function of this type is called to request a block of a certain type 
 *	   (PCM, SBC or MP3).
 */
typedef BthalStatus (*BthalRequestMoreData)(BthalMmDataBuffer *buffer);


/*-------------------------------------------------------------------------------
 * BthalCurrentStream type
 *
 *     Defines the current BTHAL MM stream.
 */
typedef U8 BthalCurrentStream;

#define BTHAL_A2DP_STREAM_PCM									(0x00)
#define BTHAL_A2DP_STREAM_SBC									(0x01)
#define BTHAL_A2DP_STREAM_MP3									(0x02)

/* Remote codec capabilities (0/1 are SBC and 2/3 are MP3) */
static BthalMmConfigParams remoteCodecCapabilities[4];
static BTHAL_BOOL remoteCodecCapabilitiesValid[4];

static Mp3StreamInfo mp3StreamInfo;
static bthalMp3Block mp3Blocks[NUM_MP3_BLOCKS];
static int fpMp3 = -1;
static BTHAL_U16 mp3MaxPacketSize;

static int fpSbc = -1;
static SbcStreamInfo sbcStreamInfo = {0};
static bthalSbcBlock sbcBlocks[NUM_SBC_BLOCKS];
static int sbcBlockCnt = 0;
static BthalCurrentStream currentStream;

/* Function table to process raw block of a certain type (PCM, SBC or MP3) */
static BthalRequestMoreData requestMoreDataFuncTable[3];

#if SBC_ENCODER == XA_ENABLED
static bthalMmToLocostoMmParams	bthalMm2LocostoMmParams;
static int 							bthalMmUsedPcmBocks = 0;
static BthalMmDataBufferDescriptor	bthalMmDataDescriptor = 0;

#endif /* SBC_ENCODER == XA_ENABLED */


#ifdef BTHAL_PCM_FILE

BOOL AppPcmOpenFile(const char *FileName, BTHAL_U16 sampleFreq, BTHAL_U8 numChannels);

#define PCM_FILE_WAV_DATA_OFFSET	(44)
#define PCM_FILE_BLOCK_SIZE			(4 * 1024)
#define PCM_FILE_BUFFER_SIZE		(524288)

/* This buffer holds the PCM file, to avoid FFS read during streaming */
U8 pcmFileBuffer[PCM_FILE_BUFFER_SIZE];

static int fpPcm = -1;
static U32 fileDataLength = 0;
static U32 fileDataIndex = 0;

#endif	/* BTHAL_PCM_FILE */


/*-------------------------------------------------------------------------------
 *  externalSbcEncoder
 *
 *     when TRUE, the SBC encoder is external to BTL (we read encoded SBC frames from an SBC file in this BTHAL)
 *     when FALSE - the SBC encoder is built-in inside BTL (and BTHAL provides PCM samples)
 */
#if SBC_ENCODER == XA_ENABLED
static BOOL externalSbcEncoder = FALSE;
#else
static BOOL externalSbcEncoder = TRUE;
#endif /* SBC_ENCODER == XA_ENABLED */



/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
void bthalMmSetPcmPlatformSupportedCapabilities(BthalMmSbcInfo *supportedCap);
void bthalMmSetMp3PlatformSupportedCapabilities(BthalMmMpeg1_2_audioInfo *supportedCap);
static BthalStatus bthalMmSelectMp3Parameters(BthalMmConfigParams *configInfo);
static BthalStatus  bthalMmSelectSbcParameters(BthalMmConfigParams *configInfo);

void bthalMmSetExternalSbcEncoder(BOOL externalEncoder);

static U16 AppMp3ReadFrames(U8 *buffer, U32 len, Mp3StreamInfo *streamInfo);
static U16 AppMp3GetFrameLen(U8 *frame, Mp3StreamInfo *streamInfo);
static U16 bthalMmReadSbcFrames(U8 *buffer, U16 bufferLen, SbcStreamInfo *streamInfo);
static BthalStatus BTHAL_MM_RequestMoreDataMp3(BthalMmDataBuffer *buffer);
static BthalStatus BTHAL_MM_RequestMoreDataPcm(BthalMmDataBuffer *buffer);
static BthalStatus BTHAL_MM_RequestMoreDataSbc(BthalMmDataBuffer *buffer);
static U32 AppMp3GetBitRate(Mp3StreamInfo *streamInfo); 

#if SBC_ENCODER == XA_ENABLED
void bthalMmReportHsConnectionState(BTHAL_U8 streamId, BOOL streamIsOpen);
void bthalMmReportHsStartingState(BTHAL_U8 streamId, BOOL streamIsStarted);

#endif /* SBC_ENCODER == XA_ENABLED */

#if SBC_ENCODER == XA_ENABLED
BOOL AVDRV_IsBtAvPathValid(void);
extern void APP_A2DP_StartPcmStream(BTHAL_U32 streamId);
void pcmSendData(U8 * pcmData, int len);
T_AUDIO_RET btAudio_PcmStreamStart(void);
T_AUDIO_RET btAudio_PcmStreamClose(void);
T_AUDIO_RET btAudio_PcmStreamStop(void);
T_AUDIO_RET btAudio_PcmStreamOpen(/*BspAudioStream_TransferStatusCallback pCallback*/);
T_AUDIO_RET btAudio_PcmStreamInit(void);

#endif /* SBC_ENCODER == XA_ENABLED */

BOOL AppMp3OpenFile(const char *FileName);
BOOL AppSbcOpenFile(const char *FileName);

void AppStopAudioFile(void);

BOOL AppMp3OpenFile(const char *FileName);
BOOL AppSbcOpenFile(const char *FileName);


/*-------------------------------------------------------------------------------
 * BTHAL_MM_Init()
 */
BthalStatus BTHAL_MM_Init(BthalCallBack	callback)
{	
	int i;

	/* Set random initial MP3 values */
	mp3StreamInfo.version = MP3_VERSION_1;
	mp3StreamInfo.layer = MP3_LAYER_3;
	mp3StreamInfo.channelMode = MP3_MODE_JOINT;
	mp3StreamInfo.brIndex = 9;
	mp3StreamInfo.srIndex = 0;
	mp3StreamInfo.crc = 1;

	mp3StreamInfo.numFrames = 0;
	mp3StreamInfo.bytesToSend = 0;
	mp3StreamInfo.offset = 0;

	/* Set random initial SBC values */
	sbcStreamInfo.allocMethod		= SBC_ALLOC_METHOD_LOUDNESS;
	sbcStreamInfo.sampleFreq		= SBC_CHNL_SAMPLE_FREQ_44_1;
	sbcStreamInfo.channelMode	= SBC_CHNL_MODE_JOINT_STEREO;
	sbcStreamInfo.numChannels	= 2;
	sbcStreamInfo.numBlocks		= 16;
	sbcStreamInfo.numSubBands	= 8;
	sbcStreamInfo.bitPool			= 30;
		
	currentStream = BTHAL_A2DP_STREAM_PCM;
	
	requestMoreDataFuncTable[BTHAL_A2DP_STREAM_PCM] = BTHAL_MM_RequestMoreDataPcm;
	requestMoreDataFuncTable[BTHAL_A2DP_STREAM_SBC] = BTHAL_MM_RequestMoreDataSbc;
	requestMoreDataFuncTable[BTHAL_A2DP_STREAM_MP3] = BTHAL_MM_RequestMoreDataMp3;

	selectedPcmInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_44100;
	selectedPcmInfo.numOfchannels = BTHAL_MM_TWO_CHANNELS;  
	selectedPcmInfo.audioQuality = BTHAL_MM_SBC_AUDIO_QUALITY_HIGH;
	selectedPcmInfo.varyBitPool = TRUE;

#if SBC_ENCODER == XA_ENABLED
	OS_MemSet((BTHAL_U8 *)(&bthalMm2LocostoMmParams), 0, sizeof(bthalMmToLocostoMmParams));

#endif /* SBC_ENCODER == XA_ENABLED */
	
	for (i = 0; i < NUM_SBC_BLOCKS; i++) 
	{
		sbcBlocks[i].used = FALSE;
	}

	for (i = 0; i < NUM_MP3_BLOCKS; i++) 
	{
		mp3Blocks[i].used = FALSE;
	}

#if SBC_ENCODER == XA_ENABLED
	L1Audio_RegisterBthal(pcmCallbackFunc, configCallbackFunc);
#endif /* SBC_ENCODER == XA_ENABLED */

    /* initialize the BTHAL multimedia debug module */
    BTHAL_MM_DEBUG_Init();

	return BTHAL_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTHAL_MM_Deinit()
 */
BthalStatus BTHAL_MM_Deinit(void)
{

#if SBC_ENCODER == XA_ENABLED
	L1Audio_RegisterBthal(NULL, NULL);
#endif /* SBC_ENCODER == XA_ENABLED */

    /* deinitialize the BTHAL multimedia debug module */
    BTHAL_MM_DEBUG_Deinit();

	return BTHAL_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTHAL_MM_GetLocalCodecCapabilities()
 */
BthalStatus BTHAL_MM_GetLocalCodecCapabilities(BthalMmConfigParams *codecInfo, 
											BthalMmSbcEncoderLocation *sbcEncoderLocation)
{
	switch (codecInfo->streamType)
	{
		case (BTHAL_MM_STREAM_TYPE_SBC):

			codecInfo->p.sbcInfo.samplingFreq = platformPcmSuportedCodec.samplingFreq;
			codecInfo->p.sbcInfo.channelMode = platformPcmSuportedCodec.channelMode;
			codecInfo->p.sbcInfo.blockLength = platformPcmSuportedCodec.blockLength;
			codecInfo->p.sbcInfo.subbands = platformPcmSuportedCodec.subbands;
			codecInfo->p.sbcInfo.allocationMethod = platformPcmSuportedCodec.allocationMethod;
			codecInfo->p.sbcInfo.minBitpoolValue = platformPcmSuportedCodec.minBitpoolValue;
			codecInfo->p.sbcInfo.maxBitpoolValue = platformPcmSuportedCodec.maxBitpoolValue;

			break;

		case (BTHAL_MM_STREAM_TYPE_MPEG1_2_AUDIO):

			codecInfo->p.mpeg1_2_audioInfo.layer = platformMp3SuportedCodec.layer;
			codecInfo->p.mpeg1_2_audioInfo.crcProtection = platformMp3SuportedCodec.crcProtection;
			codecInfo->p.mpeg1_2_audioInfo.channelMode = platformMp3SuportedCodec.channelMode;
			codecInfo->p.mpeg1_2_audioInfo.mpf2 = platformMp3SuportedCodec.mpf2;
			codecInfo->p.mpeg1_2_audioInfo.samplingFreq = platformMp3SuportedCodec.samplingFreq;
			codecInfo->p.mpeg1_2_audioInfo.vbr = platformMp3SuportedCodec.vbr;
			codecInfo->p.mpeg1_2_audioInfo.bitRate = platformMp3SuportedCodec.bitRate;

			break;
			
		default:

			Assert(0);
			break;
	}
	
	//Report(("BTHAL_MM_GetLocalCodecCapabilities, externalSbcEncoder=%d", externalSbcEncoder));
	*sbcEncoderLocation = 
		externalSbcEncoder ? BTHAL_MM_SBC_ENCODER_EXTERNAL : BTHAL_MM_SBC_ENCODER_BUILT_IN;
	
	return BTHAL_STATUS_SUCCESS;

}

/*-------------------------------------------------------------------------------
 * BTHAL_MM_Register()
 */
BthalStatus BTHAL_MM_Register(BthalMmCallback mmCallback)
{
	bthalMmCallback = mmCallback;

	return BTHAL_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTHAL_MM_RemoteCodecCapabilitiesInd()
 */
BthalStatus BTHAL_MM_RemoteCodecCapabilitiesInd(BthalMmConfigParams *codecInfo, BTHAL_U32 streamId)
{
	switch(codecInfo->streamType)
	{	
		case (BTHAL_MM_STREAM_TYPE_SBC):
		case (BTHAL_MM_STREAM_TYPE_MPEG1_2_AUDIO):

			remoteCodecCapabilities[streamId] = *codecInfo;
			break;

		default:

			Assert(0);
			break;
	}
	
	return BTHAL_STATUS_SUCCESS;
}

int decimalFreq(BthalMmSbcSamplingFreqMask freq)
{
	switch (freq)
	{
		case BTHAL_MM_SBC_SAMPLING_FREQ_16000:
			return 16000;
		case BTHAL_MM_SBC_SAMPLING_FREQ_32000: 
			return 32000;   	
		case BTHAL_MM_SBC_SAMPLING_FREQ_44100: 
			return 44100;	
		case BTHAL_MM_SBC_SAMPLING_FREQ_48000: 
			return 48000;
		case BTHAL_MM_SBC_SAMPLING_FREQ_8000_EXT: 
			return 8000;
		case BTHAL_MM_SBC_SAMPLING_FREQ_11025_EXT: 
			return 11025;
		case BTHAL_MM_SBC_SAMPLING_FREQ_12000_EXT: 
			return 12000;
		case BTHAL_MM_SBC_SAMPLING_FREQ_22050_EXT: 
			return 22050;
		case BTHAL_MM_SBC_SAMPLING_FREQ_24000_EXT: 
			return 24000;
		default: 
			return 99999;
	}
}

/*-------------------------------------------------------------------------------
 * BTHAL_MM_GetSelectedConfigInfo()
 */
BthalStatus BTHAL_MM_GetSelectedConfigInfo(BthalMmConfigParams *configInfo)
{
	BthalStatus bthalStatus = BTHAL_STATUS_SUCCESS;
	
	switch(configInfo->streamType)
	{
		case (BTHAL_MM_STREAM_TYPE_PCM):
			configInfo->p.pcmInfo = selectedPcmInfo;
			BTL_LOG_INFO(("BTHAL_MM_GetSelectedConfigInfo: sampleRate=%d, channels=%d", 
				decimalFreq(selectedPcmInfo.samplingFreq), 
				selectedPcmInfo.numOfchannels));	
			break;
			
		case (BTHAL_MM_STREAM_TYPE_SBC):
			bthalStatus = bthalMmSelectSbcParameters(configInfo);
			break;

		case (BTHAL_MM_STREAM_TYPE_MPEG1_2_AUDIO):
			bthalStatus = bthalMmSelectMp3Parameters(configInfo);
			BTL_LOG_INFO(("BTHAL_MM_GetSelectedConfigInfo: curBitRate = %d", configInfo->p.mpeg1_2_audioInfo.curBitRate));	
			break;

		default:

			Assert(0);
			break;
	}
	
	return bthalStatus;
}


/*-------------------------------------------------------------------------------
 * BTHAL_MM_CodecInfoConfiguredInd()
 */
BthalStatus BTHAL_MM_CodecInfoConfiguredInd(BthalMmConfigParams *codecInfo, 
											BTHAL_U16 packetSize, 
											BTHAL_U16 sbcFrameLen, 
											BTHAL_U32 streamId)
{
	switch(codecInfo->streamType)
	{
		case (BTHAL_MM_STREAM_TYPE_SBC):
			Report(("BTHAL_MM_CodecInfoConfiguredInd: packetSize=%d, sbcFrameLen=%d",
				packetSize, sbcFrameLen));
			OS_MemCopy((U8 *)(&SelectedSbcCodec), (U8 *)(&(codecInfo->p.sbcInfo)), sizeof(BthalMmSbcInfo));
			CodecAndFrameInfo.bthalMmPacketSize = packetSize; 
			CodecAndFrameInfo.bthalMmSbcFrameLen = sbcFrameLen;

			switch(codecInfo->p.sbcInfo.blockLength)
			{
				case BTHAL_MM_SBC_BLOCK_LENGTH_16:
					CodecAndFrameInfo.bthalMmNumBlocks = 16;
					break;

				case BTHAL_MM_SBC_BLOCK_LENGTH_12:
					CodecAndFrameInfo.bthalMmNumBlocks = 12;
					break;

				case BTHAL_MM_SBC_BLOCK_LENGTH_8:
					CodecAndFrameInfo.bthalMmNumBlocks = 8;
					break;

				case BTHAL_MM_SBC_BLOCK_LENGTH_4:
					CodecAndFrameInfo.bthalMmNumBlocks = 4;
					break;
			}

			if (codecInfo->p.sbcInfo.subbands == BTHAL_MM_SBC_SUBBANDS_8)
				CodecAndFrameInfo.bthalMmNumSubBands = 8;
			else
				CodecAndFrameInfo.bthalMmNumSubBands = 4;
	
#if SBC_ENCODER == XA_ENABLED
			if ( FALSE == externalSbcEncoder )
			{
				/* this code is only valid for internal encoder */

				if (streamId == 0 && bthalMm2LocostoMmParams.isLocostoMmConfig0 == TRUE)
				{
					/* When MM start streaming configuration is done. open streams needs to be changed to streaming */
#ifdef BTHAL_PCM_SIMULATOR
					APP_A2DP_StartPcmStream(streamId);
#endif /* BTHAL_PCM_SIMULATOR */
					bthalMm2LocostoMmParams.isLocostoMmConfig0 = FALSE;
				}
				else if (streamId == 1 && bthalMm2LocostoMmParams.isLocostoMmConfig1 == TRUE)
				{
					/* When MM start streaming configuration is done. open streams needs to be changed to streaming */
#ifdef BTHAL_PCM_SIMULATOR
					APP_A2DP_StartPcmStream(streamId);
#endif /* BTHAL_PCM_SIMULATOR */
					bthalMm2LocostoMmParams.isLocostoMmConfig1 = FALSE;
				}
				else
				{
					bthalMm2LocostoMmParams.isLocostoMmConfig0 = FALSE;
					bthalMm2LocostoMmParams.isLocostoMmConfig1 = FALSE;
				}
			}
#endif /* SBC_ENCODER == XA_ENABLED */
	
			break;

		case (BTHAL_MM_STREAM_TYPE_MPEG1_2_AUDIO):
			mp3MaxPacketSize = packetSize;

			break;

		default:
			Assert(0);
			break;
	}
	
	return BTHAL_STATUS_SUCCESS;
}


/*-------------------------------------------------------------------------------
 * BTHAL_MM_RequestMoreData()
 */
BthalStatus BTHAL_MM_RequestMoreData(BthalMmDataBuffer *buffer)
{
	return (requestMoreDataFuncTable[currentStream](buffer));
}


/*-------------------------------------------------------------------------------
 * BTHAL_MM_RequestMoreDataMp3()
 */
static BthalStatus BTHAL_MM_RequestMoreDataMp3(BthalMmDataBuffer *buffer)
{
	U16 len;
	U8 *ptr, i;
	U16 frameLen;
	U16 numFramesPerPacket;
	bthalMp3Block *block = &(mp3Blocks[0]);
	U8 *mp3FrameBuffer = block->mp3FrameBuffer;

	if ((block->used == TRUE) || (fpMp3 < 0))
		return (BTHAL_STATUS_FAILED);

	/* For now, assume only MP3 can be pulled sync from MM */
	buffer->streamType = BTHAL_MM_STREAM_TYPE_MPEG1_2_AUDIO;
	buffer->descriptor = NUM_PCM_BLOCKS;	/* MP3 descriptors must be greater than (NUM_PCM_BLOCKS-1) */

	if (mp3StreamInfo.bytesToSend == 0)
	{
REPEAT_MP3: 
		/* Read whole frames */
		len = AppMp3ReadFrames(mp3FrameBuffer, MP3_MAX_READ_BUFFER, &mp3StreamInfo);

		if (len)
	{
			buffer->data = mp3FrameBuffer;
			//buffer->dataLength = len;
			
			mp3StreamInfo.totalDataLen = 0;
	}
	else
	{
			/* Start playing the file from the beginning */
			AppMp3OpenFile(0);
			goto REPEAT_MP3;
		}
	}
	else
	{
		buffer->data = mp3FrameBuffer + mp3StreamInfo.totalDataLen;
	}
		
	buffer->frameOffset = 0;

	/* MP3 data is ready */
	if ((mp3StreamInfo.offset != 0) || 
        ((mp3StreamInfo.bytesToSend / mp3StreamInfo.numFrames) > mp3MaxPacketSize)) 
		{
        /* Fragmented frames */
		buffer->frameOffset = mp3StreamInfo.offset;

        if (mp3StreamInfo.offset == 0)
		{
            /* First part of frame */
            frameLen = AppMp3GetFrameLen(buffer->data, &mp3StreamInfo);
			mp3StreamInfo.frameLen = frameLen;
		}
		else
		{
			/* Frame cont', recover current frame length */
			frameLen = mp3StreamInfo.frameLen;
		}
			
        if ((frameLen - mp3StreamInfo.offset) <= mp3MaxPacketSize) 
		{
            buffer->dataLength = frameLen - mp3StreamInfo.offset;
            mp3StreamInfo.offset = 0;
            mp3StreamInfo.numFrames--;
        } 
		else 
		{
            buffer->dataLength = mp3MaxPacketSize;
            mp3StreamInfo.offset += mp3MaxPacketSize;
        }
    } 
	else 
	{
        /* Whole frames */
        ptr = buffer->data;
        buffer->dataLength = 0;
        frameLen = AppMp3GetFrameLen(ptr, &mp3StreamInfo);
        numFramesPerPacket = min(mp3StreamInfo.numFrames, mp3MaxPacketSize / frameLen);
        for (i = 0; i < numFramesPerPacket; i++) 
		{
            frameLen = AppMp3GetFrameLen(ptr, &mp3StreamInfo);
            ptr += frameLen;
            buffer->dataLength += frameLen;
            mp3StreamInfo.numFrames--;
		}
	}

	mp3StreamInfo.bytesToSend -= min(mp3StreamInfo.bytesToSend, buffer->dataLength);

	mp3StreamInfo.totalDataLen += buffer->dataLength;

	block->used = TRUE;

	return (BTHAL_STATUS_SUCCESS);
}

BOOL lastReplyWasPending = FALSE;
U8 *lastPendingBufAddr = NULL;

/*-------------------------------------------------------------------------------
 * BTHAL_MM_RequestMoreDataPcm()
 */
static BthalStatus BTHAL_MM_RequestMoreDataPcm(BthalMmDataBuffer *buffer)
{

#ifdef BTHAL_PCM_FILE

	U16 len;

	/* Report(("BTHAL_MM_RequestMoreDataPcm, fileDataIndex = %d", fileDataIndex)); */

	/* Warp around if needed */
	if (fileDataIndex >= fileDataLength)
		fileDataIndex = 0;

	if ((fileDataIndex + PCM_FILE_BLOCK_SIZE) <= fileDataLength)
	{
		len = PCM_FILE_BLOCK_SIZE;
	}
	else
	{
		len = (fileDataLength - fileDataIndex);
	}

	/* Report(("BTHAL_MM_RequestMoreDataPcm, len = %d", len)); */

	buffer->streamType	= BTHAL_MM_STREAM_TYPE_PCM;
	buffer->descriptor	= 0;	/* 0 is PCM block */
	buffer->data		= (pcmFileBuffer + fileDataIndex);
	buffer->dataLength	= len;

	fileDataIndex += len;

	return (BTHAL_STATUS_SUCCESS);

#else

#if BTHAL_A2DP_MM_PCM_SUPPORT == BTL_CONFIG_ENABLED
#if SBC_ENCODER == XA_ENABLED

	BthalStatus status = BTHAL_STATUS_PENDING;
	L1AudioPcmBlock pcmBlock;

	if (bthalMm2LocostoMmParams.isMmStarted == TRUE)
	{
		/* get a free PCM block descriptor */
		//if (bthalMmUsedPcmBocks < NUM_PCM_BLOCKS)
        if ((bthalMmUsedPcmBocks < 1) || callbackTrigerred) /* don't ask for more than one PCM at a timne buffer from L1 */
		{
            if (lastReplyWasPending)
                return BTHAL_STATUS_PENDING;

            /* 
             * This call is due to a calbback from L1, we return
             * the PCM block that was passed in the CB
             */
            if (callbackTrigerred)
            {
                callbackTrigerred = FALSE;
                buffer->data        = tCBPcmBlock.pcmBuffer;
                buffer->dataLength  = tCBPcmBlock.lengthInBytes;
                buffer->descriptor  = (BthalMmDataBufferDescriptor) bthalMmDataDescriptor;
                bthalMmDataDescriptor = (bthalMmDataDescriptor + 1) % NUM_PCM_BLOCKS; /* toggle beteen 0 and 1 */
                buffer->streamType  = BTHAL_MM_STREAM_TYPE_PCM;     
                status = BTHAL_STATUS_SUCCESS;
                lastReplyWasPending = FALSE;

                /* 
                 * notify the debug module of the new buffer content - note: this is done here instead
                 * of in the actual CB because we believe the CB might be called from a HISR, and this
                 * function might be time consuming
                 */
                BTHAL_MM_DEBUG_NotifyNewBuffer (tCBPcmBlock.pcmBuffer, tCBPcmBlock.lengthInBytes);

                return status;
            }
	
			bthalMmUsedPcmBocks++;

#ifdef BTHAL_MM_DEBUG
            /* debugging - set the maximum number of PCM blocks used concurrently*/
            if (tDbgBthalMm.uMaxPcmBlocksInIuse < bthalMmUsedPcmBocks)
            {
                tDbgBthalMm.uMaxPcmBlocksInIuse = bthalMmUsedPcmBocks;
            }
#endif /* BTHAL_MM_DEBUG */
            BTHAL_MM_DEBUG_NotifyPull();

			switch ( L1Audio_PullPcmBlock (&pcmBlock) )
			{
				case L1_PCM_READY:
					buffer->data			= pcmBlock.pcmBuffer;
					buffer->dataLength	= pcmBlock.lengthInBytes;
					buffer->descriptor	= (BthalMmDataBufferDescriptor) bthalMmDataDescriptor;
					bthalMmDataDescriptor = (bthalMmDataDescriptor + 1) % NUM_PCM_BLOCKS; /* toggle beteen 0 and 1 */
					buffer->streamType	= BTHAL_MM_STREAM_TYPE_PCM;		
					status = BTHAL_STATUS_SUCCESS;
					lastReplyWasPending = FALSE;
                    /* notify buffer reception */
                    BTHAL_MM_DEBUG_NotifyNewBuffer (pcmBlock.pcmBuffer, pcmBlock.lengthInBytes);
					break;
				case L1_PCM_PENDING:
					status = BTHAL_STATUS_PENDING;
#ifdef BTHAL_MM_DEBUG
                    tDbgBthalMm.uPendingReplyCount++;
#endif /* BTHAL_MM_DEBUG */
					lastReplyWasPending = TRUE;
					break;
				case L1_PCM_MEDIA_ENDED:
					AppStopAudioFile(); /* to call BTHAL_MM_EVENT_STOP_PULL */
					BT_NotifyBmiEvent(BMI_EVENT_TRACK_END);
					/* and fall through: */
				case L1_PCM_FAILED:
					bthalMmUsedPcmBocks--;
					status = BTHAL_STATUS_FAILED;
					break;
				default:
					BTL_LOG_ERROR(("unknown return from L1Audio_PullPcmBlock: %d"));
			}
		}
	}
	else
	{
		BTL_LOG_ERROR(("RequestMoreDataPcm when streaming not started yet"));
		status = BTHAL_STATUS_FAILED;
	}

	return (status);
	
#endif /* SBC_ENCODER == XA_ENABLED */

#else
	return (BTHAL_STATUS_FAILED);
#endif  /* BTHAL_A2DP_MM_PCM_SUPPORT == BTL_CONFIG_ENABLED */

#endif	/* BTHAL_PCM_FILE */

}

/*-------------------------------------------------------------------------------
 * BTHAL_MM_RequestMoreDataSbc()
 */
static BthalStatus BTHAL_MM_RequestMoreDataSbc(BthalMmDataBuffer *buffer)
{
	U16 len;
	U32 idx;
	bthalSbcBlock *block = NULL;

	for (idx = 0; idx < NUM_SBC_BLOCKS; idx++)
	{
		block = &(sbcBlocks[idx]);
		if (block->used == FALSE)
			break;
	}

	if (idx == NUM_SBC_BLOCKS || fpSbc < 0)
	{
		/* No more free SBC blocks */
		return BTHAL_STATUS_FAILED;
	}
	
REPEAT_SBC: 
	len = bthalMmReadSbcFrames(block->sbcFrameBuffer, CodecAndFrameInfo.bthalMmPacketSize, &sbcStreamInfo);

	if ( len > 0 )
	{
		buffer->streamType	= BTHAL_MM_STREAM_TYPE_SBC;
		buffer->descriptor	= SBC_FIRST_DESCRIPTOR + idx;
		buffer->data			= block->sbcFrameBuffer;
		buffer->dataLength	= len;
		block->used = TRUE;
		sbcBlockCnt++;
		//Report(("BTHAL_MM_RequestMoreDataSbc block idx %d returns %d bytes (cnt=%d)", idx, len, sbcBlockCnt));
		return (BTHAL_STATUS_SUCCESS);
	}
	
	/* Start playing the file from the beginning */
	AppSbcOpenFile(0);
	goto REPEAT_SBC;
}


/*-------------------------------------------------------------------------------
 * BTHAL_MM_FreeDataBuf()
 */
BthalStatus BTHAL_MM_FreeDataBuf(BthalMmDataBufferDescriptor dataDescriptor)
{	
	/* MP3 blocks descriptors range is [NUM_PCM_BLOCKS, (NUM_PCM_BLOCKS + NUM_MP3_BLOCKS - 1)] */
	if (dataDescriptor < NUM_PCM_BLOCKS)	/* PCM block */
	{

#ifdef BTHAL_PCM_FILE
#else

#if SBC_ENCODER == XA_ENABLED
		bthalMmUsedPcmBocks--;
#endif /*SBC_ENCODER == XA_ENABLED*/

#endif	/* BTHAL_PCM_FILE */
		
	}
	else if ( (dataDescriptor >= SBC_FIRST_DESCRIPTOR) && (dataDescriptor <=  SBC_LAST_DESCRIPTOR))
	{
		/* SBC */
		sbcBlocks[(dataDescriptor - SBC_FIRST_DESCRIPTOR)].used = FALSE;
	}
	else	/* MP3 block */
	{
		mp3Blocks[(dataDescriptor - NUM_PCM_BLOCKS)].used = FALSE;
	}

	return BTHAL_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTHAL_MM_TransportChannelStateChanged()
 */
void BTHAL_MM_TransportChannelStateChanged(BTHAL_U32 streamId, BthalMmTransportChannelState newState)
{
	streamId = streamId;
	newState = newState;
}


/*-------------------------------------------------------------------------------
 * bthalMmSetPcmPlatformSupportedCapabilities()
 *
 *		Set the PCM specific SBC capabilities supported in this platform
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		supportedCap - supported capabilities
 *
 * Returns:
 *		void
 */
void bthalMmSetPcmPlatformSupportedCapabilities(BthalMmSbcInfo *supportedCap)
{
	platformPcmSuportedCodec.samplingFreq = supportedCap->samplingFreq;
	platformPcmSuportedCodec.channelMode = supportedCap->channelMode;
	platformPcmSuportedCodec.blockLength = supportedCap->blockLength;
	platformPcmSuportedCodec.subbands = supportedCap->subbands;
	platformPcmSuportedCodec.allocationMethod = supportedCap->allocationMethod;
	platformPcmSuportedCodec.minBitpoolValue = supportedCap->minBitpoolValue;
	platformPcmSuportedCodec.maxBitpoolValue = supportedCap->maxBitpoolValue;
}


/*-------------------------------------------------------------------------------
 * bthalMmSetMp3PlatformSupportedCapabilities()
 *
 *		Set the MP3 specific SBC capabilities supported in this platform
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		supportedCap - supported capabilities
 *
 * Returns:
 *		void
 */
void bthalMmSetMp3PlatformSupportedCapabilities(BthalMmMpeg1_2_audioInfo *supportedCap)
{
	if(supportedCap->layer != 0)
	{
		platformMp3SuportedCodec.layer = supportedCap->layer;
	}
	else
	{
		platformMp3SuportedCodec.layer = (BTHAL_MM_MPEG1_2_AUDIO_LAYER_1 | BTHAL_MM_MPEG1_2_AUDIO_LAYER_2 | BTHAL_MM_MPEG1_2_AUDIO_LAYER_3);
		Report(("BTHAL_MM: Illigal layer, default value (1, 2, 3) was set"));
	}

	if(supportedCap->channelMode!= 0)
	{
		platformMp3SuportedCodec.channelMode = supportedCap->channelMode;
	}
	else
	{
		platformMp3SuportedCodec.channelMode = (BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_MONO | BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_DUAL_CHANNEL | BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_STEREO | BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_JOINT_STEREO);
		Report(("BTHAL_MM: Illigal channel mode, default value (all) was set"));
	}

	if(supportedCap->bitRate!= 0)
	{
		platformMp3SuportedCodec.bitRate = supportedCap->bitRate;
	}
	else
	{
		platformMp3SuportedCodec.bitRate= 0x7FFF;
		Report(("BTHAL_MM: Illigal bit rate, default value (0, 1, 2 and 3) was set"));
	}
		
	platformMp3SuportedCodec.crcProtection = supportedCap->crcProtection;
	platformMp3SuportedCodec.mpf2 = supportedCap->mpf2;
	platformMp3SuportedCodec.samplingFreq = supportedCap->samplingFreq;
	platformMp3SuportedCodec.vbr = supportedCap->vbr;
}


/*---------------------------------------------------------------------------
 *           MP3
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
 *            AppMp3GetBitRate()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the bit rate based on the stream info structure.
 *
 */
static U32 AppMp3GetBitRate(Mp3StreamInfo *streamInfo)
{
    U32 bitRate = 0;

    if (streamInfo->version == MP3_VERSION_1) {
        /* Version 1 */
        switch (streamInfo->layer) {
        case MP3_LAYER_1:
            bitRate = v1l1[streamInfo->brIndex];
            break;
        case MP3_LAYER_2:
            bitRate = v1l2[streamInfo->brIndex];
            break;
        case MP3_LAYER_3:
            bitRate = v1l3[streamInfo->brIndex];
            break;
        }
    } else {
        /* Version 2 or 2.5 */
        switch (streamInfo->layer) {
        case MP3_LAYER_1:
            bitRate = v2l1[streamInfo->brIndex];
            break;
        case MP3_LAYER_2:
        case MP3_LAYER_3:
            bitRate = v2l23[streamInfo->brIndex];
            break;
        }
    }

    return bitRate;
}


/*---------------------------------------------------------------------------
 *            MpGetFrameLen()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the frame length based on the stream info structure.
 *
 */
static U16 AppMp3GetFrameLen(U8 *frame, Mp3StreamInfo *streamInfo)
{
    U16 frameLen;
    U32 bitRate;
    U32 sampleRate;
    U8  padding;
	U32 header = BEtoHost32(frame);

	/* Update current frame info */
	streamInfo->version = (U8)((header & 0x00180000) >> 19);
	streamInfo->layer = (U8)((header & 0x00060000) >> 17);
	streamInfo->brIndex = (U8)((header & 0x0000F000) >> 12);
	streamInfo->srIndex = (U8)((header & 0x00000C00) >> 10);
	padding = (U8)((header & 0x00000200) >> 9);

	bitRate = AppMp3GetBitRate(streamInfo);
	sampleRate = sampleRates[streamInfo->version][streamInfo->srIndex];
	
    switch (streamInfo->layer) {
    case MP3_LAYER_1:
        frameLen = (U16)(12 * bitRate / sampleRate + padding) * 4;
        break;
    case MP3_LAYER_2:
    case MP3_LAYER_3:
        frameLen = (U16)(144 * bitRate / sampleRate + padding);
        break;
    default:
        frameLen = 1;	/* Zero value might cause divide by zero */
        break;
    }

    return frameLen;
}


/*---------------------------------------------------------------------------
 *            AppMp3GetStreamInfo()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Gets the MP3 stream information based on the 4 byte header.
 *
 */
static BtStatus AppMp3GetStreamInfo(U8 *frame, Mp3StreamInfo *streamInfo)
{
    BtStatus status = BT_STATUS_SUCCESS;
    U32 header = BEtoHost32(frame);

    streamInfo->version     = (U8)((header & 0x00180000) >> 19);
    streamInfo->layer       = (U8)((header & 0x00060000) >> 17);
    streamInfo->crc         = (U8) ! ((header & 0x00010000) >> 16); 
    streamInfo->brIndex     = (U8)((header & 0x0000F000) >> 12);
    streamInfo->srIndex     = (U8)((header & 0x00000C00) >> 10);
    streamInfo->channelMode = (U8)((header & 0x000000C0) >> 6);
	
    return status;
}


/*---------------------------------------------------------------------------
 *            AppMp3ReadFrames()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Reads whole frames from an MP3 file.  Returns the amount of data
 *            actually read.  If at least one entire frame cannot be read, 
 *            this function returns 0.  If at least 4 bytes can be read, 
 *            this function will return valid information in the streamInfo
 *            parameter.  The streamInfo parameter applies to the last frame
 *            that was read.
 *
 */
static U16 AppMp3ReadFrames(U8 *buffer, U32 len, Mp3StreamInfo *streamInfo)
{
    S32 length = 0;
    U16 totalLen = 0;
    U32 header;
    U16 frameLen;

    streamInfo->numFrames = 0;

    do { 
        /* Search for the beginning of the audio frame */
        if (ffs_read(fpMp3, buffer, 4) == 4) {
            header = BEtoHost32(buffer);
            while ((header & 0xFFE00000) != 0xFFE00000) {
                ffs_seek(fpMp3, -3, FFS_SEEK_CUR);
                if (ffs_read(fpMp3, buffer, 4) != 4) {
                    goto error_exit;
                }
                header = BEtoHost32(buffer);
            }
    
            /* Set to the beginning of the frame */
            ffs_seek(fpMp3, -4, FFS_SEEK_CUR);
        } else {
            /* End of data */
            goto error_exit;
        }

        if (AppMp3GetStreamInfo(buffer, streamInfo) == BT_STATUS_SUCCESS) {
            frameLen = AppMp3GetFrameLen(buffer, streamInfo);
		if ( (len == 4) && (frameLen > MP3_MAX_READ_BUFFER) )
		{
			BTL_LOG_ERROR(("AppMp3ReadFrames: MP3 frame length (%d) is greater than max buffer (%d)", frameLen, MP3_MAX_READ_BUFFER));
		}
        } else {
            goto error_exit;
        }

        /* Read the frame */
        if (len >= frameLen) {
            if ((U32)(length = ffs_read(fpMp3, buffer, frameLen)) != frameLen) {
                ffs_seek(fpMp3, -length, FFS_SEEK_CUR);
                goto error_exit;
            }

			len -= length;
	        buffer += length;
	        totalLen += (U16)length;
			streamInfo->numFrames++;
            }

        streamInfo->offset = 0;

    } while (len > frameLen);  

    goto exit;

error_exit:

    length = 0;

exit:

    streamInfo->bytesToSend = totalLen;

    return totalLen;
}


/*---------------------------------------------------------------------------
 *            AppMp3OpenFile()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Opens an MP3 file and jumps to the beginning of the audio
 *            data if ID3 metadata exists.  Fills the streamInfo of the
 *            first frame in the file.
 *
 */
BOOL AppMp3OpenFile(const char *FileName)
{
    int fp = -1;
    U8    buffer[4];
    U8    flags;
    U32   length;
    BOOL  id3 = FALSE;
	Mp3StreamInfo *streamInfo;
	BTHAL_S8 ret;

	Report(("AppMp3OpenFile(%s), fpMp3 was %d", FileName ? FileName : "", fpMp3));
	
	streamInfo = &mp3StreamInfo;

	mp3StreamInfo.numFrames = 0;
	mp3StreamInfo.bytesToSend = 0;
	mp3StreamInfo.offset = 0;
	mp3StreamInfo.totalDataLen= 0;

	if (FileName != 0)
	{
    fp = ffs_open((const char*)FileName, FFS_O_RDONLY);
    Report(("ffs_open returned %d",  fp));
	}
	else
	{
		fp = fpMp3;
		ffs_seek(fp, 0, FFS_SEEK_SET);
	}

    if (fp >= 0) {

        /* Look for the ID3 tag */
        if (ffs_read(fp, buffer, 4) == 4) {
            while ((BEtoHost32(buffer) & 0xFFE00000) != 0xFFE00000) {
                /* Not MP3 audio data */
                buffer[3] = 0;
                if (OS_MemCmp(buffer, 3, "ID3", 3) == TRUE) {  
                    /* Found the tag */
                    id3 = TRUE;
                    break;
                } else {
                    ffs_seek(fp, -3, FFS_SEEK_CUR);
                    if (ffs_read(fp, buffer, 4) != 4) {
                        /* End of data */
                        goto error_exit;
                    }
                }
            }

            if (id3) {
                /* Get the ID3 tag flags */
                if (ffs_seek(fp, 5, FFS_SEEK_SET) >= 0) {
                    if (ffs_read(fp, &flags, 1) == 0) {
                       /* End of data */
                       goto error_exit;
                    }
                } else {
                    /* End of data */
                    goto error_exit;
                }

                /* Get the length of the ID3 tag */
                if (ffs_read(fp, buffer, 4) == 4) {
                    length = A2DP_SynchSafetoHost32(buffer);

                    if (flags & 0x10) {
                        /* Footer is present */
                        length += 20;
                    } else {
                        length += 10;
                    }

                    /* Go past the tag data */
                    if (ffs_seek(fp, length, FFS_SEEK_SET) < 0) {
                        /* End of data */
                        goto error_exit;
                    }
                }
            } else {
		  Report((" id3 NOT found "));
                ffs_seek(fp, 0, FFS_SEEK_SET);
            }
        } else {
            /* End of data */
		Report(("failed to read 4 bytes (A)"));
            goto error_exit;
        }
    } else {
	BTL_LOG_ERROR(("failed to open file %s", FileName));
        return FALSE; //goto exit;
    }

    /* Search for the first audio frame */
    if (ffs_read(fp, buffer, 4) == 4) {
        while ((BEtoHost32(buffer) & 0xFFE00000) != 0xFFE00000) {
            ffs_seek(fp, -3, FFS_SEEK_CUR);
            if (ffs_read(fp, buffer, 4) != 4) {
                goto error_exit;
            }
        }

        /* Set to the beginning of audio data */
        ffs_seek(fp, -4, FFS_SEEK_CUR);
    } else {
        /* End of data */
        goto error_exit;
    }

	fpMp3 = fp;

    AppMp3ReadFrames(buffer, 4, streamInfo);

    goto exit;

error_exit:

    /* Error, close the file and return 0 */
    ret = ffs_close(fp);
    if(ret != EFFS_OK)
	{
		BTL_LOG_ERROR(("closing MP3 file failed (error_exit:), ret=%d", ret));
	}
    fpMp3 = -1;
	return FALSE;

exit:


	if ((fpMp3 >= 0) && (FileName != 0))
	{
		BthalMmCallbackParms callbackParams;
		BthalMmConfigParams configInfo;

		if (bthalMmSelectMp3Parameters(&configInfo) == BTHAL_STATUS_SUCCESS)
		{
			callbackParams.event = BTHAL_MM_EVENT_CONFIG_IND;
			
			/* Pass the event to btl_a2dp */
			bthalMmCallback(&callbackParams);

			currentStream = BTHAL_A2DP_STREAM_MP3;
		}
		else
		{
			/* Error, close the file and return 0 */
		    ret = ffs_close(fp);
		    if(ret != EFFS_OK)
			{
				BTL_LOG_ERROR(("closing MP3 file failed (bthalMmSelectMp3Parameters failed), ret=%d", ret));
			}
	
		    fpMp3 = -1;
			return FALSE;
		}
	}

	return TRUE;
}


/*---------------------------------------------------------------------------
 *            AppMp3CloseFile()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close an MP3 file.
 *
 */
void AppMp3CloseFile(void)
{
	BTHAL_S8 ret ;

	Report(("AppMp3CloseFile(%d)", fpMp3));
	
	if ( fpMp3 >= 0)
	{
		ret = ffs_close(fpMp3);
		if(ret != EFFS_OK)
		{
			BTL_LOG_ERROR(("closing MP3 file failed, ret=%d", ret));
		}
	}
	else
	{
		BTL_LOG_ERROR((" MP3 file is not open"));
	}


    fpMp3 = -1;
}


/*---------------------------------------------------------------------------
 *            AppMp3PlayFile()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Play an MP3 file.
 *
 */
void AppMp3PlayFile(void)
{
	BthalMmCallbackParms callbackParams;

	Report(("AppMp3PlayFile"));

	if (fpMp3 >= 0)
	{
		callbackParams.event = BTHAL_MM_EVENT_START_PULL;
		
		/* Pass the event to btl_a2dp */
		bthalMmCallback(&callbackParams);
	}
	else
	{
		BTL_LOG_ERROR(("cannot play MP3 file because it is not open"));
	}
}

/*---------------------------------------------------------------------------
 *            AppStopAudioFile()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Stop playing an audio file.
 *
 */
void AppStopAudioFile(void)
{
	BthalMmCallbackParms callbackParams;

	BTL_LOG_INFO(("AppStopAudioFile, call BTHAL_MM_EVENT_STOP_PULL"));

#if SBC_ENCODER == XA_ENABLED
	bthalMm2LocostoMmParams.isMmStarted = FALSE;
#endif /*SBC_ENCODER == XA_ENABLED*/

	callbackParams.event = BTHAL_MM_EVENT_STOP_PULL;
	
    BTHAL_MM_DEBUG_NotifyPlayStop();

	/* Pass the event to btl_a2dp */
	bthalMmCallback(&callbackParams);
}


#ifdef BTHAL_PCM_FILE
	
/*---------------------------------------------------------------------------
 *            AppPcmOpenFile()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Opens an PCM file and jumps to the beginning of the audio data.
 *
 */
BOOL AppPcmOpenFile(const char *FileName, BTHAL_U16 sampleFreq, BTHAL_U8 numChannels)
{
    int fp = -1;
    U8    buffer[PCM_FILE_WAV_DATA_OFFSET];
	U32 bytesRead;
	T_FFS_STAT	         stat;	/* Used to obtain the file length */
	T_FFS_RET            retVal;

	Report(("AppPcmOpenFile(%s, %d, %d)", FileName, sampleFreq, numChannels));

    /* First obtain the file length */
	retVal = ffs_stat(FileName, &stat);
	if (retVal != EFFS_OK)
	{
		Report(("Error reading file stat"));
		return FALSE;
	}

	Report(("File size = %d ", stat.size));

	if (stat.size <= PCM_FILE_WAV_DATA_OFFSET)
	{
		Report(("Error - File size(%d) too small", stat.size));
		return FALSE;
	}
	
	fileDataLength = (stat.size - PCM_FILE_WAV_DATA_OFFSET);

	Report(("fileDataLength = %d ", fileDataLength));

	if (fileDataLength > PCM_FILE_BUFFER_SIZE)
	{
		Report(("Error - File size(%d) too big", stat.size));
		return FALSE;
	}

	fp = ffs_open((const char*)FileName, FFS_O_RDONLY);
    Report(("ffs_open returned %d",  fp));

    if (fp >= 0) 
	{
		/* First, skip the WAV file header */
		bytesRead = ffs_read(fp, (void*)buffer, PCM_FILE_WAV_DATA_OFFSET);

		if (bytesRead != PCM_FILE_WAV_DATA_OFFSET)
		{
			BTL_LOG_ERROR(("failed to read PCM file header "));
        	goto error_exit;
		}

		/* Now read all file into memory */
		bytesRead = ffs_read(fp, (void*)pcmFileBuffer, fileDataLength);

		if (bytesRead != fileDataLength)
		{
			BTL_LOG_ERROR(("failed to read PCM file data "));
        	goto error_exit;
		}

		fileDataIndex = 0;

		Report(("All PCM file data %d was read to memory ", fileDataLength));
	}
	else 
	{
		BTL_LOG_ERROR(("failed to open file %s", FileName));
        goto error_exit;
    }

	fpPcm = fp;

    goto exit;

error_exit:

    /* Error, close the file and return 0 */
    retVal = ffs_close(fp);
    if(retVal != EFFS_OK)
	{
		BTL_LOG_ERROR(("closing PCM file failed (error_exit:), retVal=%d", retVal));
	}
    fpPcm = -1;
	return FALSE;

exit:

	if (fpPcm >= 0)
	{
		BthalMmCallbackParms callbackParams;

		switch (sampleFreq)
		{
			case 16000:
				selectedPcmInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_16000;
				break;
			case 32000:
				selectedPcmInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_32000;
				break;
			case 44100:
				selectedPcmInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_44100;
				break;
			case 48000:
				selectedPcmInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_48000;
				break;
			case 8000:
				selectedPcmInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_8000_EXT;
				break;
			case 11025:
				selectedPcmInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_11025_EXT;
				break;
			case 12000:
				selectedPcmInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_12000_EXT;
				break;
			case 22050:
				selectedPcmInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_22050_EXT;
				break;
			case 24000:
				selectedPcmInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_24000_EXT;
				break;
			default:
				BTL_LOG_ERROR(("BTHAL_MM: Invalid sample frequency -%d Hz", sampleFreq));
				goto error_exit;
		}

		switch (numChannels)
		{
			case 1:
				selectedPcmInfo.numOfchannels = BTHAL_MM_ONE_CHANNELS;
				break;
			case 2:
				selectedPcmInfo.numOfchannels = BTHAL_MM_TWO_CHANNELS;
				break;
			default:
				BTL_LOG_ERROR(("BTHAL_MM: Invalid num of channels -%d ", numChannels));
				goto error_exit;
		}

		callbackParams.event = BTHAL_MM_EVENT_CONFIG_IND;
			
		/* Pass the event to btl_a2dp */
		bthalMmCallback(&callbackParams);

		currentStream = BTHAL_A2DP_STREAM_PCM;
	}

	return TRUE;
}


/*---------------------------------------------------------------------------
 *            AppPcmCloseFile()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close an PCM file.
 *
 */
void AppPcmCloseFile(void)
{
	BTHAL_S8 ret ;

	Report(("AppPcmCloseFile(%d)", fpPcm));
	
	if ( fpPcm >= 0)
	{
		ret = ffs_close(fpPcm);
		if(ret != EFFS_OK)
		{
			BTL_LOG_ERROR(("closing PCM file failed, ret=%d", ret));
		}
	}
	else
	{
		BTL_LOG_ERROR((" PCM file is not open"));
	}


    fpPcm = -1;
}


/*---------------------------------------------------------------------------
 *            AppPcmPlayFile()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Play an PCM file.
 *
 */
void AppPcmPlayFile(void)
{
	BthalMmCallbackParms callbackParams;

	Report(("AppPcmPlayFile"));

	callbackParams.event = BTHAL_MM_EVENT_START_PULL;
		
	/* Pass the event to btl_a2dp */
	bthalMmCallback(&callbackParams);
}

#endif	/* BTHAL_PCM_FILE */


/*---------------------------------------------------------------------------
 *            bthalMmStreamStateIsConnected()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  update the BTHAL MM with the current stream state.
 *
 */
void bthalMmStreamStateIsConnected(U32 streamId, BOOL isConnected)
{
	remoteCodecCapabilitiesValid[streamId] = isConnected;
}


/*---------------------------------------------------------------------------
 *            bthalMmSetExternalSbcEncoder()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  set whether we are with external SBC encoder (i.e. read SBC file in BTHAL),
 *                 or with built-in SBC encoder (inside BTL)
 *
 */
void bthalMmSetExternalSbcEncoder(BOOL externalEncoder)
{
	Report(("bthalMmSetExternalSbcEncoder, externalEncoder = %d", externalEncoder));

#if SBC_ENCODER == XA_ENABLED

	externalSbcEncoder = externalEncoder;

#else

	/* don't change anything, it must be an external encoder */

#endif /* SBC_ENCODER == XA_ENABLED */
}
 
/*---------------------------------------------------------------------------
 *            bthalMmReadSbcFrames()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Reads whole frames from an SBC file.  Returns the amount of data
 *            actually read.  If at least one entire frame cannot be read, 
 *            this function returns 0.
 */
U16 bthalMmReadSbcFrames(U8 *buffer, U16 bufferLen, SbcStreamInfo *streamInfo)
{
	U16 numOfBytesRead = 0;
	U16 numOfFrames = 0;
	U16 remainder = 0;
	U16 frameLen = SBC_FrameLen(streamInfo);

	if (sbcBlockCnt == 0)
		Report(("SBC_FrameLen = %d", frameLen));

	/* numOfFrames will contain the integer number of frames to read from the file */
	/* We assume fixed bit pool */
	numOfFrames = (U16)(bufferLen/frameLen);

	/* Read few frames from the file */
	numOfBytesRead = (U16)(ffs_read(fpSbc, buffer, (numOfFrames*frameLen)));

	/* in case we arrived to the end of the file and we read not a whole number of frames leftover will be different from 0 */
	remainder = numOfBytesRead % frameLen;

	/* return the number of send bytes which are a whole number of frames */
	return (numOfBytesRead - remainder);

}

/*---------------------------------------------------------------------------
 *            SbcParseHeader()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Parses an SBC header for frame information.
 * 			info is put in global variable sbcStreamInfo
 */
static BOOL SbcParseHeaderInfo(U8 *Buffer )
{
	U8 *ptr = Buffer + 1; /* to skip the frame sync byte 0x9C */
	SbcStreamInfo *streamInfo = &sbcStreamInfo;

	/* Sampling Frequency */
	streamInfo->sampleFreq = *ptr >> 6;

	/* Number of blocks */
	switch ((*ptr >> 4) & 0x03)
	{
		case 0:
			streamInfo->numBlocks = 4;
			break;
		case 1:
			streamInfo->numBlocks = 8;
			break;
		case 2:
			streamInfo->numBlocks = 12;
			break;
		case 3:
			streamInfo->numBlocks = 16;
			break;
		default:
			return FALSE;
	}

	/* Channel mode and number of channels */
	streamInfo->channelMode =  ((*ptr >> 2) & 0x03);
	switch (streamInfo->channelMode)
	{
		case 0:
			streamInfo->numChannels = 1;
			break;
		case 1:
		case 2:
		case 3:
			streamInfo->numChannels = 2;
			break;
		default:
			return FALSE;
	}

	/* Allocation Method */
	streamInfo->allocMethod = (*ptr >> 1) & 0x01;

	streamInfo->lastSbcFrameLen = 0;

	/* Subbands */
	switch (*ptr++ & 0x01)
	{
		case 0:
			streamInfo->numSubBands = 4;
			break;
		case 1:
			streamInfo->numSubBands = 8;
			break;
		default:
			return FALSE;
	}

	/* Bitpool */
	streamInfo->bitPool = *ptr++;

	return TRUE;

}

/*---------------------------------------------------------------------------
 *            AppSbcOpenFile()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Opens an SBC file and finds the streamInfo of the first frame 
 *            in the file.
 *
 *	return TRUE upon success, else FALSE
 *
 */
 BOOL AppSbcOpenFile(const char *FileName)
{
	U8          headerBuffer[8]; /* actually 3 bytes are enough */
	int       	fp = -1;
	BthalMmCallbackParms callbackParams;
	BTHAL_S8 ret ;

	Report(("AppSbcOpenFile(%s), fpSbc was %d", FileName ? FileName : "", fpSbc));
	
	if (!externalSbcEncoder) 
	{
		Report(("must use an external SBC encoder to play SBC files"));
		return FALSE;
	}

	/* Read the first header to get stream settings */
	if (FileName != 0)
	{
	fp = ffs_open((const char *)FileName, FFS_O_RDONLY);
    	Report(("ffs_open returned %d",  fp));
	}
	else
    {
		fp = fpSbc;
		ffs_seek(fp, 0, FFS_SEEK_SET);
    }

	if (fp < 0) 
	{
		BTL_LOG_ERROR(("failed to open file %s", FileName));
		return FALSE;
	}

	ffs_read(fp, headerBuffer, sizeof(headerBuffer));
	if ( ! SbcParseHeaderInfo(headerBuffer) )
	{
		Report(("failed to parse header in SBC file"));
		ret = ffs_close(fp);
		if(ret != EFFS_OK)
		{
			BTL_LOG_ERROR(("closing SBC file failed (SbcParseHeaderInfo failed), ret=%d", ret));
		}
		return FALSE;
	}
	else
	{
		Report(("SBC header: freq %d,ChnlMode %d, BlkLen %d, subbands %d, AllocMethod %d, BitPool %d",
			sbcStreamInfo.sampleFreq, sbcStreamInfo.channelMode, sbcStreamInfo.numBlocks,
			sbcStreamInfo.numSubBands,	sbcStreamInfo.allocMethod, sbcStreamInfo.bitPool));
	}

	ffs_seek(fp, 0, FFS_SEEK_SET);

	fpSbc = fp;

	if ((fpSbc >= 0) && (FileName != 0))
	{
		BthalMmConfigParams configInfo;

		if (bthalMmSelectSbcParameters(&configInfo) == BTHAL_STATUS_SUCCESS)
		{
			callbackParams.event = BTHAL_MM_EVENT_CONFIG_IND;
			/* Pass the event to btl_a2dp */
			bthalMmCallback(&callbackParams);
			currentStream = BTHAL_A2DP_STREAM_SBC;
		}
		else
		{
			/* Error, close the file and return fail */
			ret = ffs_close(fp);
			if(ret != EFFS_OK)
			{
				BTL_LOG_ERROR(("closing SBC file failed (bthalMmSelectSbcParameters failed), ret=%d", ret));
			}
			fpSbc = -1;
			return FALSE;
		}
	}

	return TRUE;
}


/*---------------------------------------------------------------------------
 *            AppSbcCloseFile()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close an SBC file.
 *
 */
void AppSbcCloseFile(void)
{
	BTHAL_S8 ret ;

	Report(("AppSbcCloseFile(%d)", fpSbc));
	
	if ( fpSbc >= 0)
	{
		ret = ffs_close(fpSbc);
		if(ret != EFFS_OK)
		{
			BTL_LOG_ERROR(("closing SBC file failed, ret=%d", ret));
		}
	}
	else
	{
		BTL_LOG_ERROR((" SBC file is not open"));
	}

    	fpSbc = -1;
}


/*---------------------------------------------------------------------------
 *            AppSbcPlayFile()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Play an SBC file.
 *
 */
void AppSbcPlayFile(void)
{

	BthalMmCallbackParms callbackParams;
	
	Report(("AppSbcPlayFile"));

	if (fpSbc  >= 0)
	{
		callbackParams.event = BTHAL_MM_EVENT_START_PULL;
		
		sbcBlockCnt = 0;
		/* Pass the event to btl_a2dp */
		bthalMmCallback(&callbackParams);
	}
	else
	{
		BTL_LOG_ERROR(("cannot play SBC file because it is not open"));
	}
	
}


/*---------------------------------------------------------------------------
 *            bthalMmSelectMp3Parameters()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Fills the given struct with the selected MP3 parameters.
 *
 */
static BthalStatus bthalMmSelectMp3Parameters(BthalMmConfigParams *configInfo)
{
	BthalStatus bthalStatus = BTHAL_STATUS_SUCCESS;
	U32 idx;
	
    /* Set the configuration based on the stream info in the file. 
    Implicity select no CRC, no MPF-2 and no VBR. */
                     
	switch (mp3StreamInfo.layer) 
	{
    case MP3_LAYER_1:
		configInfo->p.mpeg1_2_audioInfo.layer = BTHAL_MM_MPEG1_2_AUDIO_LAYER_1;
        break;
    case MP3_LAYER_2:
        configInfo->p.mpeg1_2_audioInfo.layer = BTHAL_MM_MPEG1_2_AUDIO_LAYER_2;
        break;
    case MP3_LAYER_3:
        configInfo->p.mpeg1_2_audioInfo.layer = BTHAL_MM_MPEG1_2_AUDIO_LAYER_3;
        break;
    default:
        Assert(0);
        break;
    }
	
	switch (mp3StreamInfo.channelMode) 
	{
    case MP3_MODE_STEREO: 
        configInfo->p.mpeg1_2_audioInfo.channelMode = BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_STEREO;
        break;
    case MP3_MODE_JOINT:
        configInfo->p.mpeg1_2_audioInfo.channelMode = BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_JOINT_STEREO;
        break;
    case MP3_MODE_DUAL:
        configInfo->p.mpeg1_2_audioInfo.channelMode = BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_DUAL_CHANNEL;
        break;
    case MP3_MODE_MONO:
        configInfo->p.mpeg1_2_audioInfo.channelMode = BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_MONO;
        break;
    default:
        Assert(0);
        break;
	}

	switch (sampleRates[mp3StreamInfo.version][mp3StreamInfo.srIndex]) 
	{
        case 48000:
            configInfo->p.mpeg1_2_audioInfo.samplingFreq = BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_48000;
            break;
        case 44100:
            configInfo->p.mpeg1_2_audioInfo.samplingFreq = BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_44100;
            break;
        case 32000:
            configInfo->p.mpeg1_2_audioInfo.samplingFreq = BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_32000;
            break;
        case 24000:
            configInfo->p.mpeg1_2_audioInfo.samplingFreq = BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_24000;
            break;
        case 22050:
            configInfo->p.mpeg1_2_audioInfo.samplingFreq = BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_22050;
            break;
        case 16000:
            configInfo->p.mpeg1_2_audioInfo.samplingFreq = BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_16000;
            break;
        default:
            Assert(0);
            break;
 	}

	if (mp3StreamInfo.brIndex < 15) 
	{
		configInfo->p.mpeg1_2_audioInfo.bitRate = (U16)(1 << mp3StreamInfo.brIndex);
		//StoreBE16((U8*)(&(configInfo->p.mpeg1_2_audioInfo.bitRate)), (U16)(1 << mp3StreamInfo.brIndex));
    } 
	else 
	{
        Assert(0);
    }

	configInfo->p.mpeg1_2_audioInfo.curBitRate = AppMp3GetBitRate(&mp3StreamInfo); 

	if (mp3StreamInfo.crc)
		configInfo->p.mpeg1_2_audioInfo.crcProtection = BTHAL_MM_MPEG1_2_AUDIO_CRC_PROTECTION_SUPPORTED;
	else
		configInfo->p.mpeg1_2_audioInfo.crcProtection = BTHAL_MM_MPEG1_2_AUDIO_CRC_PROTECTION_NOT_SUPPORTED;
	
	configInfo->p.mpeg1_2_audioInfo.mpf2 = BTHAL_MM_MPEG1_2_AUDIO_MPF2_NOT_SUPPORTED;
	configInfo->p.mpeg1_2_audioInfo.vbr = BTHAL_MM_MPEG1_2_AUDIO_VBR_NOT_SUPPORTED;

	/* Verify the selected codec is supported with local capablities */
	configInfo->p.mpeg1_2_audioInfo.layer &= platformMp3SuportedCodec.layer;
	configInfo->p.mpeg1_2_audioInfo.channelMode &= platformMp3SuportedCodec.channelMode;
	configInfo->p.mpeg1_2_audioInfo.samplingFreq &= platformMp3SuportedCodec.samplingFreq;
	configInfo->p.mpeg1_2_audioInfo.bitRate &= platformMp3SuportedCodec.bitRate;

	/* Verify the selected codec is supported with remote capablities for streams 2/3 */
	for (idx=2; idx<4; idx++)
	{
		if (remoteCodecCapabilitiesValid[idx] == TRUE)
		{
			configInfo->p.mpeg1_2_audioInfo.layer &= remoteCodecCapabilities[idx].p.mpeg1_2_audioInfo.layer;
			configInfo->p.mpeg1_2_audioInfo.channelMode &= remoteCodecCapabilities[idx].p.mpeg1_2_audioInfo.channelMode;
			configInfo->p.mpeg1_2_audioInfo.samplingFreq &= remoteCodecCapabilities[idx].p.mpeg1_2_audioInfo.samplingFreq;
			configInfo->p.mpeg1_2_audioInfo.bitRate &= remoteCodecCapabilities[idx].p.mpeg1_2_audioInfo.bitRate;
		}
	}

	if ((!configInfo->p.mpeg1_2_audioInfo.layer) || 
		(!configInfo->p.mpeg1_2_audioInfo.channelMode) || 
		(!configInfo->p.mpeg1_2_audioInfo.samplingFreq) || 
		(!configInfo->p.mpeg1_2_audioInfo.bitRate))
	{
		Report(("Invalid MP3 file parameters!"));
		bthalStatus =  BTHAL_STATUS_FAILED;
	}

	return bthalStatus;
}


/*---------------------------------------------------------------------------
 *            bthalMmSelectSbcParameters()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Fills the given struct with the selected SBC parameters.
 *
 */
BthalStatus  bthalMmSelectSbcParameters(BthalMmConfigParams *configInfo)
{
	BthalStatus bthalStatus = BTHAL_STATUS_SUCCESS;
	U8 maxBitpool, minBitpool;
	U32 idx;

	switch (sbcStreamInfo.channelMode) 
	{
		case SBC_CHNL_MODE_MONO: 
			configInfo->p.sbcInfo.channelMode = BTHAL_MM_SBC_CHANNEL_MODE_MONO;
			break;
		case SBC_CHNL_MODE_DUAL_CHNL:
			configInfo->p.sbcInfo.channelMode = BTHAL_MM_SBC_CHANNEL_MODE_DUAL_CHANNEL;
			break;
		case SBC_CHNL_MODE_STEREO:
			configInfo->p.sbcInfo.channelMode = BTHAL_MM_SBC_CHANNEL_MODE_STEREO;
			break;
		case SBC_CHNL_MODE_JOINT_STEREO:
			configInfo->p.sbcInfo.channelMode = BTHAL_MM_SBC_CHANNEL_MODE_JOINT_STEREO;
			break;
		default:
			Assert(0);
			break;
	}

	switch (sbcStreamInfo.sampleFreq) 
	{
		case SBC_CHNL_SAMPLE_FREQ_16:
			configInfo->p.sbcInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_16000;
			break;
		case SBC_CHNL_SAMPLE_FREQ_32:
			configInfo->p.sbcInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_32000;
			break;
		case SBC_CHNL_SAMPLE_FREQ_44_1:
			configInfo->p.sbcInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_44100;
			break;
		case SBC_CHNL_SAMPLE_FREQ_48:
			configInfo->p.sbcInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_48000;
			break;
		default:
			Assert(0);
			break;
	}
	
	switch (sbcStreamInfo.numSubBands) 
	{
		case 4:
			configInfo->p.sbcInfo.subbands = BTHAL_MM_SBC_SUBBANDS_4;
			break;
		case 8:
			configInfo->p.sbcInfo.subbands = BTHAL_MM_SBC_SUBBANDS_8;
			break;
		default:
			Assert(0);
			break;
	}
	
	switch (sbcStreamInfo.numBlocks) 
	{
		case 4:
			configInfo->p.sbcInfo.blockLength = BTHAL_MM_SBC_BLOCK_LENGTH_4;
			break;
		case 8:
			configInfo->p.sbcInfo.blockLength = BTHAL_MM_SBC_BLOCK_LENGTH_8;
			break;
		case 12:
			configInfo->p.sbcInfo.blockLength = BTHAL_MM_SBC_BLOCK_LENGTH_12;
			break;
		case 16:
			configInfo->p.sbcInfo.blockLength = BTHAL_MM_SBC_BLOCK_LENGTH_16;
			break;
		default:
			Assert(0);
			break;
	}

	switch (sbcStreamInfo.allocMethod) 
	{
		case SBC_ALLOC_METHOD_LOUDNESS:
			configInfo->p.sbcInfo.allocationMethod = BTHAL_MM_SBC_ALLOCATION_METHOD_LOUDNESS;
			break;
		case SBC_ALLOC_METHOD_SNR:
			configInfo->p.sbcInfo.allocationMethod = BTHAL_MM_SBC_ALLOCATION_METHOD_SNR;
			break;
		default:
			Assert(0);
			break;
	}
	
	configInfo->p.sbcInfo.maxBitpoolValue = sbcStreamInfo.bitPool;
	configInfo->p.sbcInfo.minBitpoolValue  = sbcStreamInfo.bitPool;

	/* Verify the selected codec is supported with local capablities */
	configInfo->p.sbcInfo.channelMode &= platformPcmSuportedCodec.channelMode;
	configInfo->p.sbcInfo.samplingFreq &= platformPcmSuportedCodec.samplingFreq;
	configInfo->p.sbcInfo.blockLength &= platformPcmSuportedCodec.blockLength;
	configInfo->p.sbcInfo.subbands &= platformPcmSuportedCodec.subbands;
	configInfo->p.sbcInfo.allocationMethod &= platformPcmSuportedCodec.allocationMethod;
	minBitpool = platformPcmSuportedCodec.minBitpoolValue;
	maxBitpool = platformPcmSuportedCodec.maxBitpoolValue;

	/* Verify the selected codec is supported with remote capablities for streams 0/1 */
	for (idx=0; idx<2; idx++)
	{
		if (remoteCodecCapabilitiesValid[idx] == TRUE)
		{
			configInfo->p.sbcInfo.channelMode &= remoteCodecCapabilities[idx].p.sbcInfo.channelMode;
			configInfo->p.sbcInfo.samplingFreq &= remoteCodecCapabilities[idx].p.sbcInfo.samplingFreq;
			configInfo->p.sbcInfo.blockLength &= remoteCodecCapabilities[idx].p.sbcInfo.blockLength;
			configInfo->p.sbcInfo.subbands &= remoteCodecCapabilities[idx].p.sbcInfo.subbands;
			configInfo->p.sbcInfo.allocationMethod &= remoteCodecCapabilities[idx].p.sbcInfo.allocationMethod;
			minBitpool = max(remoteCodecCapabilities[idx].p.sbcInfo.minBitpoolValue, minBitpool);
			maxBitpool = min(remoteCodecCapabilities[idx].p.sbcInfo.maxBitpoolValue, maxBitpool);
		}
	}


	if ( 	(!configInfo->p.sbcInfo.channelMode) 	||
		(!configInfo->p.sbcInfo.samplingFreq) 	||
		(!configInfo->p.sbcInfo.blockLength) 	||
		(!configInfo->p.sbcInfo.subbands) 		||
		(!configInfo->p.sbcInfo.allocationMethod)||
		( sbcStreamInfo.bitPool < minBitpool ) 	||
		( sbcStreamInfo.bitPool > maxBitpool ))
	{
		bthalStatus =  BTHAL_STATUS_FAILED;
		BTL_LOG_ERROR(("bthalMmSelectSbcParameters failed. masks: "
				"chnlMode %x, Freq %x, blkLen %x, subbands %x, allocMethod %x. bp: %d, min %d, max %d",
			(configInfo->p.sbcInfo.channelMode),
			(configInfo->p.sbcInfo.samplingFreq),
			(configInfo->p.sbcInfo.blockLength),
			(configInfo->p.sbcInfo.subbands),
			(configInfo->p.sbcInfo.allocationMethod),
			sbcStreamInfo.bitPool, minBitpool, maxBitpool));
	}
	return bthalStatus;

}

#if SBC_ENCODER == XA_ENABLED
/****************************************************************************************
*				PCM 
*****************************************************************************************/

#if NEPTUNE_LIKE == XA_ENABLED

/*---------------------------------------------------------------------------
 *            AVDRV_IsBtAvPathValid()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Check if there is a PCM stream in started state.
 *
 */
BOOL AVDRV_IsBtAvPathValid(void)
{
	if (!externalSbcEncoder) 
	{
		if( (bthalMm2LocostoMmParams.isStream0started == TRUE ||
			 bthalMm2LocostoMmParams.isStream1started == TRUE) &&
			 bthalMm2LocostoMmParams.isOpen == TRUE)
		{
			return TRUE;
		}
	}
	else
	{
		BTL_LOG_ERROR(("Internal SBC Encoder is disabled"));
	}
	return FALSE;

}

/*---------------------------------------------------------------------------
 *            btAudio_PCMStreamInit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Init interface with MM
 *
 */
T_AUDIO_RET	btAudio_PcmStreamInit(void)
{	
	if (!externalSbcEncoder) 
	{
		Report(("BTHAL_MM: btAudio_PCMStreamInit()"));

		selectedPcmInfo.samplingFreq = 0;
		
		return AUDIO_OK;
	}
	else
	{
		BTL_LOG_ERROR(("Internal SBC Encoder is disabled"));
		return AUDIO_ERROR;
	}
	
}


/*---------------------------------------------------------------------------
 *            btAudio_PcmStreamOpen()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  open path from Locosto MM to BT
 *
 */
T_AUDIO_RET btAudio_PcmStreamOpen(/*LocostoAudioStream_TransferStatusCallback pCallback*/)
{
	//BtStatus retVal = AUDIO_API_AUDIO_STREAM_RETURN_CODE_SUCCESS; Need to know returncodes
	T_AUDIO_RET retVal = AUDIO_OK;
	if (!externalSbcEncoder) 
	{
	
		Report(("BTHAL_MM: btAudio_PcmStreamOpen"));

		if(bthalMm2LocostoMmParams.isStream0open || bthalMm2LocostoMmParams.isStream1open)
		{
			/*request more data callback from Locosto MM*/
			//bthalMm2LocostoMmParams.requestMoreDataCB = pCallback;

			if(selectedPcmInfo.samplingFreq != 0)
			{
				bthalMm2LocostoMmParams.isOpen = TRUE;
				retVal = AUDIO_OK;
			}
			else
			{
				retVal = AUDIO_ERROR;
			}

		}
		else
		{
				retVal = AUDIO_ERROR;
		}
	}
	else
	{
		BTL_LOG_ERROR(("Internal SBC Encoder is disabled"));
		retVal = AUDIO_ERROR;
	}
		return(retVal);
}


/*---------------------------------------------------------------------------
 *            btAudio_PcmStreamStop()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  stop MM data streaming
 *
 */
T_AUDIO_RET btAudio_PcmStreamStop(void)
{
	BthalMmCallbackParms callbackParams;
	T_AUDIO_RET retVal = AUDIO_OK;
	if (!externalSbcEncoder) 
	{

		Report(("BTHAL_MM: btAudio_PcmStreamStop()"));
			
		bthalMm2LocostoMmParams.isMmStarted = FALSE;
			
		callbackParams.event = BTHAL_MM_EVENT_STOP_PULL;

			/* Pass the event to btl_a2dp */
			bthalMmCallback(&callbackParams);

		return AUDIO_OK;
    //need to find correct returncodes AUDIO_API_AUDIO_STREAM_RETURN_CODE_SUCCESS ?
	}
	else
	{
		BTL_LOG_ERROR(("Internal SBC Encoder is disabled"));
		retVal = AUDIO_ERROR;
		return(retVal);
	}
}


/*---------------------------------------------------------------------------
 *            btAudio_PcmStreamClose()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close the data path from Locosto MM to BT
 *
 */
T_AUDIO_RET btAudio_PcmStreamClose(void)
{
	T_AUDIO_RET retVal = AUDIO_OK;
	if (!externalSbcEncoder) 
	{
		Report(("BTHAL_MM: btAudio_PcmStreamClose()"));

		bthalMm2LocostoMmParams.isOpen = FALSE;

		return AUDIO_OK;
    //need to find correct returncodes AUDIO_API_AUDIO_STREAM_RETURN_CODE_SUCCESS ?
	}
	else
	{
		BTL_LOG_ERROR(("Internal SBC Encoder is disabled"));
		retVal = AUDIO_ERROR;
		return(retVal);
	}
}
	

/*---------------------------------------------------------------------------
 *            btAudio_PcmStreamStart()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  start MM data streaming
 *
 */
T_AUDIO_RET btAudio_PcmStreamStart(void)
{
	BthalMmCallbackParms callbackParams;
	T_AUDIO_RET retVal = AUDIO_OK;
	if (!externalSbcEncoder) 
	{
		
		if(bthalMm2LocostoMmParams.isMmStarted == FALSE)
		{
			bthalMm2LocostoMmParams.isMmStarted = TRUE;
		
			Report(("BTHAL_MM: btAudio_stream_write_start()"));

			callbackParams.event = BTHAL_MM_EVENT_START_PULL;

			/* Pass the event to btl_a2dp */
			bthalMmCallback(&callbackParams);
			
			return AUDIO_OK;
		    //need to find correct returncodes AUDIO_API_AUDIO_STREAM_RETURN_CODE_SUCCESS ?
		}
	}
	else
	{
		BTL_LOG_ERROR(("Internal SBC Encoder is disabled"));
		retVal = AUDIO_ERROR;
		return(retVal);
	}

	return(retVal);
}

#endif /* NEPTUNE_LIKE */


/*---------------------------------------------------------------------------
 *            AV_SendPcmData()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  process MM data
 *
 */
#if 0
void pcmSendData(U8 * pcmData, int len)
{
	BthalMmCallbackParms callbackParams;
	
	if (!externalSbcEncoder) 
	{
		if(bthalMm2LocostoMmParams.isMmStarted == TRUE)
		{
			/*sending data event to btl_a2dp */
	
			callbackParams.event = BTHAL_MM_EVENT_DATA_BUFFER_IND;
			callbackParams.p.dataBuffer.dataLength = (BTHAL_U16)len;
			callbackParams.p.dataBuffer.data = pcmData;
			callbackParams.p.dataBuffer.streamType = BTHAL_MM_STREAM_TYPE_PCM;
			callbackParams.p.dataBuffer.descriptor = bthalMmDataDescriptor;
			bthalMmDataDescriptor = (bthalMmDataDescriptor + 1) % NUM_PCM_BLOCKS; /* toggle beteen 0 and 1 */
			/* Pass the event to btl_a2dp */
			bthalMmCallback(&callbackParams);
		}
		else
		{
			BTL_LOG_ERROR(("BTHAL_MM: pcmSendData() : Data wasn't sent. streaming not started yet"));
		}
	}
	else
	{
		BTL_LOG_ERROR(("Internal SBC Encoder is disabled"));
	}
		
}
#endif
	
void bthalMmReportHsConnectionState(BTHAL_U8 streamId, BOOL streamIsOpen)
{
	if (streamId == 0)
		bthalMm2LocostoMmParams.isStream0open = streamIsOpen;
	else
		bthalMm2LocostoMmParams.isStream1open = streamIsOpen;
}

void bthalMmReportHsStartingState(BTHAL_U8 streamId, BOOL streamIsStarted)
{
	if ((streamId >= BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT) || externalSbcEncoder)
	{
		BTL_LOG_INFO(("bthalMmReportHsStartingState(%d, %d), not PCM !!", streamId, streamIsStarted));
		return;
	}

#ifdef BTHAL_PCM_FILE

#else

	if (bthalMm2LocostoMmParams.isStreamStarted[streamId] != streamIsStarted)
	{
		bthalMm2LocostoMmParams.isStreamStarted[streamId] = streamIsStarted;
		BTL_LOG_INFO(("calling L1Audio_InformBtAudioPathState(streamIsStarted=%d), !!",  streamIsStarted));
		L1Audio_InformBtAudioPathState(streamIsStarted); /* ??? take care of multi stream ??? */
		if (streamIsStarted)
		{
			if(bthalMm2LocostoMmParams.isMmStarted == FALSE)
			{
				BthalMmCallbackParms callbackParams;
				
#ifdef BTHAL_MM_DEBUG
                tDbgBthalMm.uPcmMemIndex = 0;
#endif /* BTHAL_MM_DEBUG */
				bthalMm2LocostoMmParams.isMmStarted = TRUE;
				callbackParams.event = BTHAL_MM_EVENT_START_PULL;
				/* Pass the event to btl_a2dp */
				BTL_LOG_INFO(("calling BTHAL_MM_EVENT_START_PULL !!"));
				bthalMmCallback(&callbackParams);
			}
		}
		else
		{ 	/* ??? take care of multi stream - stop only if all streams are not streaming anymore ??? */
			AppStopAudioFile(); /* to call BTHAL_MM_EVENT_STOP_PULL */
		}
	}

#endif	/* BTHAL_PCM_FILE */
	
}

#ifdef BTHAL_PCM_SIMULATOR // can be removed ???
/*---------------------------------------------------------------------------
 *            AppPcmSimOpenFile()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Opens an PCM/WAV file from FFS and copy it to memory 
 *             (to rule out FFS timing issues)
 *
 *	return TRUE upon success, else FALSE
 *
 */
 BOOL AppPcmSimOpenFile(const char *FileName)
{
    BthalMmCallbackParms callbackParams;
    T_FFS_FD	         fd;
    T_FFS_STAT	         stat;	/* Used to obtain the file length */
    T_FFS_RET            retVal;
    U32                  copyLen, bytesCopied;

    // Stream is not started yet
    bthalMm2LocostoMmParams.isMmStarted = FALSE;
    
	// Align size to 16 bit data samples
	blockSize = PCM_BLOCK_SIZE;
	blockSize = blockSize >> 1;
	blockSize = blockSize << 1;

    /* First obtain the file length */
	retVal = ffs_stat(FileName, &stat);
	if (retVal != EFFS_OK)
	{
		Report(("BTHAL PCMSIM: Error reading file stat"));
		return FALSE;
	}

	if (stat.size <= WAV_FILE_DATA_OFFSET)
	{
		Report(("BTHAL PCMSIM: Error - File size(0x%x) too small", stat.size));
		return FALSE;
	}

	fd = ffs_open(FileName, FFS_O_RDONLY);

	if (fd < 0)
	{
		Report(("BTHAL PCMSIM: Error - File open failed"));
		return FALSE;
	}

	/* First, skip the WAV file header */
	bytesCopied = ffs_read(fd, (void*)memoryBuf, WAV_FILE_DATA_OFFSET);

	copyLen = stat.size - WAV_FILE_DATA_OFFSET;

	Report(("BTHAL PCMSIM: Copying 0x%x bytes into memory", copyLen));

    /* Load the wave file from FFS to memory (to rule out FFS timing issues) */
	bytesCopied = ffs_read(fd, (void*)memoryBuf, copyLen);
	if (bytesCopied != copyLen)
	{
		Report(("BTHAL PCMSIM: Error reading from file"));
	}

	pcmBufEnd = (U8*)memoryBuf + bytesCopied;
	memIndex = (U8*)memoryBuf;

	Report(("BTHAL PCMSIM: PCM Buf =0x%x", (U32)memoryBuf));
	Report(("BTHAL PCMSIM: PCM Buf index=0x%x", (U32)memIndex));
	Report(("BTHAL PCMSIM: PCM Buf end=0x%x", (U32)pcmBufEnd));
	Report(("BTHAL PCMSIM: PCM block size=0x%x (%d)", blockSize, blockSize));
    
	retVal = ffs_close(fd);
	if (retVal != EFFS_OK)
	{
		Report(("BTHAL PCMSIM: Error closing the file"));
	}

    /* This is a fixed setting for our wave file (for the time-being) */
	selectedPcmInfo.samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_44100;
	selectedPcmInfo.numOfchannels = BTHAL_MM_TWO_CHANNELS;
	bthalMm2LocostoMmParams.isLocostoMmConfig0 = TRUE;
	bthalMm2LocostoMmParams.isLocostoMmConfig1 = TRUE;
    
	Report(("BTHAL_MM: Selected sample frequency (0x%x)", selectedPcmInfo.samplingFreq));
    
	/* Pass the event to btl_a2dp to configure the stream */
	callbackParams.event = BTHAL_MM_EVENT_CONFIG_IND;
	bthalMmCallback(&callbackParams);
	currentStream = BTHAL_A2DP_STREAM_PCM;

	return TRUE;
}
	

/*---------------------------------------------------------------------------
 *            AppPcmSimCloseFile()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close an PCM Sim file.
 *
 */
void AppPcmSimCloseFile(void)
{
   // We actually do not need this since we run from memory,
   // but keep it if we want to test from FFS
}

/*---------------------------------------------------------------------------
 *            AppPcmSimPlayFile()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Play an PCM file from FFS/RAM.
 *
 */
void AppPcmSimPlayFile(void)
{
	BthalMmCallbackParms callbackParams;

	if (!bthalMm2LocostoMmParams.isMmStarted)
{
		bthalMm2LocostoMmParams.isMmStarted = TRUE;

    	/* Pass the event to btl_a2dp so btl_a2dp can start pulling the PCM samples */
    	callbackParams.event = BTHAL_MM_EVENT_START_PULL;
    	bthalMmCallback(&callbackParams);
	}
	else
	{
		Report(("BTHAL PCMSIM: Already running!"));
	}
}

/*---------------------------------------------------------------------------
 *            AppPcmSimStopFile()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Stop playing an PCM file from FFS/RAM.
 *
 */
void AppPcmSimStopFile(void)
{
	BthalMmCallbackParms callbackParams;
    
	if (bthalMm2LocostoMmParams.isMmStarted)
{
		bthalMm2LocostoMmParams.isMmStarted = FALSE;
        
    	/* Pass the event to btl_a2dp so btl_a2dp stops pulling the PCM samples */
    	callbackParams.event = BTHAL_MM_EVENT_STOP_PULL;
    	bthalMmCallback(&callbackParams);		
	}
	else
	{
		Report(("BTHAL_PCMSIM: Already stopped!"));
	}
}

#endif /* BTHAL_PCM_SIMULATOR */


#if BTHAL_A2DP_MM_PCM_SUPPORT == BTL_CONFIG_ENABLED

void pcmCallbackFunc(L1AudioPcmBlock *pcmBlock)
{
    BthalMmCallbackParms callbackParams;

    /* Keep the PCM block given to be used later when requesting more data */
    tCBPcmBlock = *pcmBlock;

    /* debugging - increase number of callbacks, check same buffer */
#ifdef BTHAL_MM_DEBUG
    tDbgBthalMm.uPcmCBCount++;
#endif /* BTHAL_MM_DEBUG */

	lastReplyWasPending = FALSE;
	lastPendingBufAddr = pcmBlock->pcmBuffer;
    callbackTrigerred = TRUE;

    /* Pass the event to btl_a2dp */
		callbackParams.event = BTHAL_MM_EVENT_START_PULL;
		bthalMmCallback(&callbackParams);
    
	return;
}

void configCallbackFunc (L1AudioPcmConfig *pcmConfig)
{
	BthalMmCallbackParms callbackParams;
	BOOL error = FALSE;

	if (!externalSbcEncoder) 
	{
		
		BthalMmSbcSamplingFreqMask samplingFreq;
		BthalMmNumberOfChannels numOfchannels;

        	BTL_LOG_INFO( ("BTHAL_MM: configCallbackFunc(sampleRate=%d)", pcmConfig->sampleRate) );

		/* Translate bit rate parameteres */		
		switch (pcmConfig->sampleRate)
		{
			case 16000:
				samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_16000;
				break;
			case 32000:
				samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_32000;
				break;
			case 44100:
				samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_44100;
				break;
			case 48000:
				samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_48000;
				break;
			case 8000:
				samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_8000_EXT;
				break;
			case 11025:
				samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_11025_EXT;
				break;
			case 12000:
				samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_12000_EXT;
				break;
			case 22050:
				samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_22050_EXT;
				break;
			case 24000:
				samplingFreq = BTHAL_MM_SBC_SAMPLING_FREQ_24000_EXT;
				break;
			default:
				BTL_LOG_ERROR(("BTHAL_MM: Invalid sample frequency -%d Hz", pcmConfig->sampleRate));
				error = TRUE;
		}

		switch (pcmConfig->numChannels)
		{
			case 1:
				numOfchannels = BTHAL_MM_ONE_CHANNELS;
				break;
			case 2:
				numOfchannels = BTHAL_MM_TWO_CHANNELS;
				break;
			default:
				BTL_LOG_ERROR(("BTHAL_MM: Invalid num of channels -%d", pcmConfig->numChannels));
				error = TRUE;
		}

		if (pcmConfig->bitsPerSample != 16)
		{
			BTL_LOG_ERROR(("bits per sample must be 16 (requested: %d)", pcmConfig->bitsPerSample));
			error = TRUE;
		}

		if (error)
		{
			return;
		}

		BTL_LOG_INFO(("BTHAL_MM: Selected sample frequency=%d, num channels=%d", samplingFreq, numOfchannels));
	
		if ((selectedPcmInfo.samplingFreq == samplingFreq) && (selectedPcmInfo.numOfchannels == numOfchannels))
		{
			return; /* no need to callback BTL, no change in config */
		}
			
		selectedPcmInfo.samplingFreq = samplingFreq;
		selectedPcmInfo.numOfchannels = numOfchannels;
			
		bthalMm2LocostoMmParams.isLocostoMmConfig0= TRUE;
		bthalMm2LocostoMmParams.isLocostoMmConfig1= TRUE;
		
		//Send config indication from MM
		callbackParams.event = BTHAL_MM_EVENT_CONFIG_IND;

		/* Pass the event to btl_a2dp */
		bthalMmCallback(&callbackParams);

	}
	else
	{
		BTL_LOG_ERROR(("Internal SBC Encoder is disabled"));
	}	
}

#endif  /* BTHAL_A2DP_MM_PCM_SUPPORT == BTL_CONFIG_ENABLED */
	
#ifdef L1_STUB

static BOOL btConnected = FALSE;
static L1AudioPcmCallback		btPcmCallback	= NULL;
static L1AudioConfigureCallback	btConfigCallback	= NULL;
	
BOOL L1Audio_InformBtAudioPathState (BOOL connected)
{
	btConnected = connected;
	return TRUE;
}


/****************************************************/
void L1Audio_RegisterBthal (L1AudioPcmCallback pcmCallback, L1AudioConfigureCallback configCallback)
{
	btPcmCallback = pcmCallback;
	btConfigCallback = configCallback;
}


/****************************************************/
L1AudioPcmStatus L1Audio_PullPcmBlock (L1AudioPcmBlock *pcmBlock)
{
	return L1_PCM_PENDING;
}
 
void l1audio_CallConfigCallback(int sampleRate)
{
	L1AudioPcmConfig pcmConfig;

	if (configCallbackFunc != NULL)
	{
		pcmConfig.sampleRate = sampleRate;
		pcmConfig.numChannels = 2;
		pcmConfig.bitsPerSample = 16;
		
		configCallbackFunc (&pcmConfig);
	}
	else
	{
		BTL_LOG_ERROR(("l1audio_CallConfigCallback -configCallbackFunc is  N U L L !! "));
	}
}

#endif /* L1_STUB */


#endif /*SBC_ENCODER == XA_ENABLED*/


#else /*BTL_CONFIG_A2DP ==   BTL_CONFIG_ENABLED*/

BthalStatus BTHAL_MM_Init(BthalCallBack	callback)

{
	callback = callback;
	Report(("BTHAL_MM_Init -BTL_CONFIG_A2DP is disabled."));
	return BTHAL_STATUS_SUCCESS;
}

BthalStatus BTHAL_MM_Deinit(void)
{
	Report(("BTHAL_MM_Deinit -BTL_CONFIG_A2DP is disabled."));
	return BTHAL_STATUS_SUCCESS;

}


#endif /*BTL_CONFIG_A2DP ==   BTL_CONFIG_ENABLED*/



