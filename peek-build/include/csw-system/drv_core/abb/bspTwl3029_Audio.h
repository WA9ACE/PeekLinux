/*=============================================================================
 * Copyright 2003 Texas Instruments Incorporated. All Rights Reserved.
 */
#ifndef BSP_TWL3029_AUDIO_HEADER
#define BSP_TWL3029_AUDIO_HEADER
 
#if (ANLG_PG == 1)
#include "pg1x/bspTwl3029_Audio_pg1x.h"
#elif (ANLG_PG == 2)
#include "pg2x/bspTwl3029_Audio_pg2x.h"
#endif 

/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspTwl3029_Audio.h
 *   Public interface to the Twl3029 audio hardware subsystem.
 */
/*===========================================================================
 *  Defines and Macros
 */
 /*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_AudioRegFieldInfo
 *
 * @discussion 
 * <b> Description </b><br>
 *      regBitOffset - register address ( from address lister is map file)
 *
 *      regBitWidth - field width within a reg ( from those listed in Llif file) 
 *
 *      regBitOffset - field offset within a reg ( from those listed in Llif file) 
 */
typedef struct
{
    Uint8                      regBitOffset;
    Uint8                      regBitWidth;
    BspTwl3029_I2C_RegId       registerId;
} BspTwl3029_AudioRegFieldInfo;


/*============================================================================*/
/*!
 * @constant
 *
 * @discussion 
 * <b> Description </b><br>
 *     These are the max allowed number for BspTwl3029_Audio_InputPathId 
 */
# define INPUT_PATH_MAX                      9    
/*===========================================================================
 * Public Data
 */
 /*===========================================================================*/
/*!
 * @typedef BspTwl3029_PowerControl
 *  
 * @discussion
 * <b> Description  </b><br>
 *   This is the data type for enabling or disabling power for the Audio 
 *   subsystem in the twl3029.
 */
enum
{
    BSP_TWL3029_POWER_CONTROL_ENABLE  = 0,
    BSP_TWL3029_POWER_CONTROL_DISABLE = 1
};
typedef Uint8 BspTwl3029_Audio_PowerControl;

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Audio_PowerModule
 *
 * @discussion
 * <b> Description  </b><br>
 *    Data type identifying the audio block to which power control is to be
 *    applied.
 *
 * @constant BSP_TWL3029_AUDIO_POWER_MODULE_VOICE_VUL
 *     Enable/disable power to the voice codec uplink path.
 *
 * @constant BSP_TWL3029_AUDIO_POWER_MODULE_VOICE_VDL
 *     Enable/disable power to the voice codec downlink path.
 *
 * @constant BSP_TWL3029_AUDIO_POWER_MODULE_STEREO
 *    Enable/disable power to the stereo dac.
 */
enum
{
    BSP_TWL3029_AUDIO_POWER_MODULE_VUL    = 1,
    BSP_TWL3029_AUDIO_POWER_MODULE_VDL    = 2,    
    BSP_TWL3029_AUDIO_POWER_MODULE_STEREO = 4
};
typedef Uint8 BspTwl3029_Audio_PowerModule;


/*============================================================================*/
/*!
 * @constant
 *
 * @discussion 
 * <b> Description </b><br>
 *     These are used to enable/disable audio power with TOGB reg 
 */
 enum
{
   BSP_TWL3029_AUDIO_POWERDOWN= 0x15,  /* stereo DL, audio UL and DL  off */
   BSP_TWL3029_AUDIO_POWER_ON = 0x2A  /* stereo DL, audio UL and DL  on */
}; 
/*============================================================================*/
/*!
 * @constant
 *
 * @discussion 
 * <b> Description </b><br>
 *     These are default values for all regs for initialization. 
 */
 enum
{
   BSP_TWL_3029_MAP_AUDIO_TOGB_DEFAULT        = 0x15,   /*  AUDIO:  TOGB  */ /* Resets the voice and stereo power*/
   BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_DEFAULT = 0x00,   /*  AUDIO:  PWRONSTATUS  */
   BSP_TWL_3029_MAP_AUDIO_CTRL1_DEFAULT       = 0x00,   /*  AUDIO:  CTRL1  */
   BSP_TWL_3029_MAP_AUDIO_CTRL2_DEFAULT       = 0x00,   /*  AUDIO:  CTRL2  */
   BSP_TWL_3029_MAP_AUDIO_CTRL3_DEFAULT       = 0x00,   /*  AUDIO:  CTRL3  */
   BSP_TWL_3029_MAP_AUDIO_CTRL4_DEFAULT       = 0x00,   /*  AUDIO:  CTRL4  */
#if (ANLG_PG == 1)
   BSP_TWL_3029_MAP_AUDIO_CTRL5_DEFAULT       = 0x10,   /*  AUDIO:  CTRL5   HOOKEN hook detect enabled */
#elif (ANLG_PG == 2)
   BSP_TWL_3029_MAP_AUDIO_CTRL5_DEFAULT       = (BSP_TWL_3029_AUDIO_CTRL5_DEFAULT | 0x40),   /*  AUDIO: CTRL5 */
#endif
   BSP_TWL_3029_MAP_AUDIO_CTRL6_DEFAULT       = 0x00,   /*  AUDIO:  CTRL6  */
#if (ANLG_PG == 1)
   BSP_TWL_3029_MAP_AUDIO_POPMAN_DEFAULT      = 0x2A,   /*  AUDIO:  POPMAN  */  /*enable all post discharge except CARKIT */
   BSP_TWL_3029_MAP_AUDIO_POPAUTO_DEFAULT     = 0x07,   /*  AUDIO:  POPAUTO : auto ON (except carkit)*/
#elif (ANLG_PG == 2)
   BSP_TWL_3029_MAP_AUDIO_POPMAN_DEFAULT      = BSP_TWL_3029_AUDIO_POPMAN_DEFAULT,  /*  AUDIO: POPMAN */
   BSP_TWL_3029_MAP_AUDIO_POPAUTO_DEFAULT     = BSP_TWL_3029_AUDIO_POPAUTO_DEFAULT, /*  AUDIO: POPAUTO*/
#endif
   BSP_TWL_3029_MAP_AUDIO_SIDETONE_DEFAULT    = 0x00,   /*  AUDIO:  SIDETONE  */
   BSP_TWL_3029_MAP_AUDIO_VULGAIN_DEFAULT     = 0x00,   /*  AUDIO:  VULGAIN  */
   BSP_TWL_3029_MAP_AUDIO_VDLGAIN_DEFAULT     = 0x00,   /*  AUDIO:  VDLGAIN  */
   BSP_TWL_3029_MAP_AUDIO_OUTEN1_DEFAULT      = 0x00,   /*  AUDIO:  OUTEN1  */
   BSP_TWL_3029_MAP_AUDIO_OUTEN2_DEFAULT      = 0x00,   /*  AUDIO:  OUTEN2  */
   BSP_TWL_3029_MAP_AUDIO_OUTEN3_DEFAULT      = 0x00,   /*  AUDIO:  OUTEN3  */
   BSP_TWL_3029_MAP_AUDIO_AUDLGAIN_DEFAULT    = 0x00,   /*  AUDIO:  AUDLGAIN  */
   BSP_TWL_3029_MAP_AUDIO_AUDRGAIN_DEFAULT    = 0x00    /*  AUDIO:  AUDRGAIN  */   
} ;
typedef Uint8 BspTwl3029_Audio_RegDefault;


/*============================================================================*/
/*!
 * @constant
 *
 * @discussion 
 * <b> Description </b><br>
 *     These are the different output path Ids. 
 */
enum
{
    BSP_TWL3029_AUDIO_INPUT_PATH_ID_NONE =               0,
    BSP_TWL3029_AUDIO_INPUT_PATH_ID_HANDSET_MIC_25_6DB = 1,   /* diff */
    BSP_TWL3029_AUDIO_INPUT_PATH_ID_HEADSET_MIC_4_9_DB = 2,   /* diff */
    BSP_TWL3029_AUDIO_INPUT_PATH_ID_HEADSET_MIC_25_6DB = 3,   /* diff */
    BSP_TWL3029_AUDIO_INPUT_PATH_ID_FM                 = 4,   /* 2 x single */
    BSP_TWL3029_AUDIO_INPUT_PATH_ID_HEADSET_MIC_18DB   = 5,   /* single */
    BSP_TWL3029_AUDIO_INPUT_PATH_ID_AUX_4_9DB          = 6,   /* single */
    BSP_TWL3029_AUDIO_INPUT_PATH_ID_AUX_28_2DB         = 7,    /* single */
    BSP_TWL3029_AUDIO_INPUT_PATH_ID_FM_MONO            = 8,   /* 2 x single */
    BSP_TWL3029_AUDIO_INPUT_PATH_ID_CARKIT             = 9    /* 1 x single */
};
typedef Uint8 BspTwl3029_Audio_InputPathId;
//# define INPUT_PATH_MAX                      9    /* max allowed number for BspTwl3029_Audio_InputPathId */
              
/*============================================================================*/
/*!
 * @constant
 *
 * @discussion 
 * <b> Description </b><br>
 *     These are used to set bit 5 (MICBIASLVL ) and bit 4 (MICBIASSEL)
 *     of CTRL2 register. for use when one of the microphone IP paths
 *     is selected 
 *     MICBIASLVL 0 = 2V,      1 = 2.5V - analog bias
 *     MICBIASSEL 0 = Handset, 1 = headset 
 */
  
 #define   BSP_TWL3029_AUDIO_INPUT_MIC_BIAS_HANDSET_MIC_25_6DB_DEFAULT  0   /* diff */
 #define   BSP_TWL3029_AUDIO_INPUT_MIC_BIAS_HEADSET_MIC_4_9_DB_DEFAULT  1   /* diff */
 #define   BSP_TWL3029_AUDIO_INPUT_MIC_BIAS_HEADSET_MIC_25_6DB_DEFAULT  1   /* diff */
 #define   BSP_TWL3029_AUDIO_INPUT_MIC_BIAS_HEADSET_MIC_18DB_DEFAULT    1   /* single */

typedef Uint8 BspTwl3029_Audio_InputMicBias;

/*============================================================================*/
/*!
 * @constant
 *
 * @discussion 
 * <b> Description </b><br>
 *     These are used to set bit 5 (MICBIASLVL ) of CTRL2 register. 
 *     for use when one of the microphone IP paths is selected 
 *     MICBIASLVL 0 = 2V,      1 = 2.5V - analog bias
 */
  
 #define   BSP_TWL3029_AUDIO_INPUT_MIC_BIAS_2_0  0   
 #define   BSP_TWL3029_AUDIO_INPUT_MIC_BIAS_2_5  1

typedef Uint8 BspTwl3029_Audio_MicBiasId;

/*============================================================================*/
/*!
 * @constant
 *
 * @discussion 
 * <b> Description </b><br>
 *     These are the different output path Ids.  Multiple output paths can 
 *     be selected when configuring the output, by OR ing the ids.
 *      
 */
enum
{
    BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_NONE                = 0,
    BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HANDSET_EARPIECE    = 1,
    BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_R_EARPIECE  = 2,
    BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_L_EARPIECE  = 4,
    BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_AUX                 = 8,
    BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_CARKIT              = 16,
    BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK                 = 32,
    BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK_DCLASS          = 64
};
typedef Uint8 BspTwl3029_Audio_OutputPathId;
    
/* note: SPK_DCLASS is not actually a separate speaker . It is same speaker 
    but with a D class amp enabled.  */
   
# define OUTPUT_PATH_MAX 0x7f 


/*===========================================================================*/
/*!
 * @discussion
 *    set of defines used for referencing an array of 
 *     type  BspTwl3029_AudioOutputSource 
 *
  */
#define   BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET    0
#define   BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET  1 
#define   BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET  2
#define   BSP_TWL3029_AUDIO_OUTPUT_AUX_OFFSET                 3
#define   BSP_TWL3029_AUDIO_OUTPUT_CARKIT_OFFSET              4
#define   BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET                 5

#define   BSP_TWL3029_AUDIO_OUTPUT_SOURCE_MAX_SIZE            6

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_AudioOutputSource
 *
 * @discussion
 * <b> Description  </b><br>
 *    Data type defining the the type of audio delivered to audio output device.
 *    these are used when programming registers OUTEN1 and OUTEN3 which contain
 *    3 bit wide fields. 
 *    The fields in OUTEN2 are only 2 bits wide 
 *
 * @constant BSP_TWL3029_AUDIO_OUTPUT_SOURCE_RESET
 *     Resets/disables the audio output path.
 *
 * @constant BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE
 *    The selected output path is sourced by downlink voice(OUTEN1,OUTEN2,OUTEN3).
 *
 * @constant BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO
 *    The selected output path is sourced by  stereo audio(OUTEN1,OUTEN2,OUTEN3)..
 *
 * @constant BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM
 *    The selected output path is sourced by  stereo audio(OUTEN1,OUTEN3).
 *
 * @constant BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO
 *    The selected output path is sourced by  stereo audio + voice(OUTEN1,OUTEN2,OUTEN3).
 *
 * @constant BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM
 *    The selected output path is sourced by  voice + FM(OUTEN1,OUTEN3)
 * 
 * @constant BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM
 *    The selected output path is sourced by  stereo audio + FM(OUTEN1,OUTEN3)
 */
  
enum
{
   BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE             = 0,
   BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE            = 1,
   BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO            = 2,
   BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM               = 3,
   BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO  = 4,
   BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM     = 5,
   BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM     = 6,
   BSP_TWL3029_AUDIO_OUTPUT_SOURCE_INVERT_VOICE	    = 7
};
typedef Uint8 BspTwl3029_AudioOutputSource;



/*============================================================================*/
/*!
 * @constant
 *
 * @discussion 
 * <b> Description </b><br>
 *     audio input signal type. 
 */
enum
{
    BSP_TWL3029_AUDIO_OUTPUT_PATH_OFF         = 0,
    BSP_TWL3029_AUDIO_OUTPUT_PATH_VOICE_ONLY  = 1,
    BSP_TWL3029_AUDIO_OUTPUT_PATH_AUDIO       = 2,
    BSP_TWL3029_AUDIO_OUTPUT_PATH_VOICE_AUDIO = 3
};
typedef Uint8 BspTwl3029_Audio_OutputPathType;
/*===========================================================================*/
/*!
 * @typedef BspTwl3029_MuteControl
 *  
 * @discussion
 * <b> Description  </b><br>
 *   This is the data type for enabling or disabling the mute on the twl3029
 *   audio subsystem.
 */
enum
{
    BSP_TWL3029_MUTE_CONTROL_DISABLE  = 0,
    BSP_TWL3029_MUTE_CONTROL_ENABLE   = 1
};
typedef Uint8 BspTwl3029_Audio_MuteControl;
/*===========================================================================*/
/*!
 * @typedef  BspTwl3029_Audio_MutePath
 *
 * @discussion
 * <b> Description  </b><br>
 *    Indicates the audio path to mute.
 *
 * @constant BSP_TWL3029_AUDIO_MUTE_AUDIO_OUTPUT
 *    Mutes the audio output/downlink path.
 *
 * @constant BSP_TWL3029_AUDIO_MUTE_AUDIO_INPUT
 *    Mutes the audio input/uplink path.
 * @constant BSP_TWL3029_AUDIO_MUTE_AUDIO_INPUT_OUTPUT
 *    Mutes the both paths.
 */
enum
{
    BSP_TWL3029_AUDIO_MUTE_PATH_OUTPUT        = 1,
    BSP_TWL3029_AUDIO_MUTE_PATH_INPUT         = 2,
    BSP_TWL3029_AUDIO_MUTE_PATH_INPUT_OUTPUT  = 3    
};
typedef Uint8 BspTwl3029_Audio_MutePath;


/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Audio_GainSources
 *
 * @discussion
 * <b> Description  </b><br>
 *    bit map. bit = 1 update the audio gain for the chosen source
 *             bit = 0 don't update the audio gain
 */
 enum
{
    BSP_TWL3029_AUDIO_GAIN_UPDATE_SPEAKERAMP   = 1,  /* bit 0 = 1*/
    BSP_TWL3029_AUDIO_GAIN_UPDATE_VUL          = 2,  /* bit 1 = 1 */
    BSP_TWL3029_AUDIO_GAIN_UPDATE_VDL          = 4,
    BSP_TWL3029_AUDIO_GAIN_UPDATE_SIDETONE     = 8,
    BSP_TWL3029_AUDIO_GAIN_UPDATE_STEREO_RIGHT = 16,
    BSP_TWL3029_AUDIO_GAIN_UPDATE_STEREO_LEFT  = 32,
    BSP_TWL3029_AUDIO_GAIN_UPDATE_SOFTVOL      = 64,  /* bit 6 = 1 */
    BSP_TWL3029_AUDIO_GAIN_UPDATE_RADIO        = 128,  /* bit 7 = 1 */
    BSP_TWL3029_AUDIO_GAIN_UPDATE_EARGAIN      = 256  /* bit 8 = 1 */
};
 
typedef Uint16 BspTwl3029_Audio_GainSources;

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Audio_GainVDLSources
 *
 * @discussion
 * <b> Description  </b><br>
 *    bit map. bit = 1 update the audio gain for the chosen source
 *             bit = 0 don't update the audio gain
 */
 enum
{
    BSP_TWL3029_AUDIO_GAIN_UPDATE_VDL_COURSE          = 1,
    BSP_TWL3029_AUDIO_GAIN_UPDATE_VDL_FINE	          = 2		
};
typedef Uint8 BspTwl3029_Audio_GainVDLSources;
/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Audio_VoiceCodecUplinkGain
 *
 * @discussion
 * <b> Description  </b><br>
 *    Defines the voice codec downlink gains.
 *    Gain Range is -12dB to +12DB in steps of 1 DB.
 */
enum
{
    BSP_TWL3029_AUDIO_GAIN_DB_MINUS_12  = 0x10,
    BSP_TWL3029_AUDIO_GAIN_DB_MINUS_11  = 0x17,
    BSP_TWL3029_AUDIO_GAIN_DB_MINUS_10  = 0x18,
    BSP_TWL3029_AUDIO_GAIN_DB_MINUS_9   = 0x19,
    BSP_TWL3029_AUDIO_GAIN_DB_MINUS_8   = 0x1A,
    BSP_TWL3029_AUDIO_GAIN_DB_MINUS_7   = 0x1B,
    BSP_TWL3029_AUDIO_GAIN_DB_MINUS_6   = 0x00,
    BSP_TWL3029_AUDIO_GAIN_DB_MINUS_5   = 0x01,
    BSP_TWL3029_AUDIO_GAIN_DB_MINUS_4   = 0x02,
    BSP_TWL3029_AUDIO_GAIN_DB_MINUS_3   = 0x03,
    BSP_TWL3029_AUDIO_GAIN_DB_MINUS_2   = 0x04,
    BSP_TWL3029_AUDIO_GAIN_DB_MINUS_1   = 0x05,
    BSP_TWL3029_AUDIO_GAIN_DB_0         = 0x06,
    BSP_TWL3029_AUDIO_GAIN_DB_1         = 0x07,
    BSP_TWL3029_AUDIO_GAIN_DB_2         = 0x08,
    BSP_TWL3029_AUDIO_GAIN_DB_3         = 0x09,
    BSP_TWL3029_AUDIO_GAIN_DB_4         = 0x0A,
    BSP_TWL3029_AUDIO_GAIN_DB_5         = 0x0B,
    BSP_TWL3029_AUDIO_GAIN_DB_6         = 0x0C,
    BSP_TWL3029_AUDIO_GAIN_DB_7         = 0x11,
    BSP_TWL3029_AUDIO_GAIN_DB_8         = 0x12,
    BSP_TWL3029_AUDIO_GAIN_DB_9         = 0x13,
    BSP_TWL3029_AUDIO_GAIN_DB_10        = 0x14,
    BSP_TWL3029_AUDIO_GAIN_DB_11        = 0x15,
    BSP_TWL3029_AUDIO_GAIN_DB_12        = 0x16,
    BSP_TWL3029_AUDIO_GAIN_DB_MUTE      = 0x20   /*  bit DXEN (bit 5 VULGAIN register)*/
};
typedef Uint8 BspTwl3029_Audio_VoiceCodecUplinkGain;
/*===========================================================================*/
/*!
 * @typedef BspTwl3029Audio_VoiceCodecDownlinkGain
 *
 * @discussion
 * <b> Description  </b><br>
 *    Defines the voice codec downlink gains.
 *    Gain Range is -30DB to +6DB in steps of 1 DB.
 *     bottom 4 LSBs define fine gain ( q dB steps)
 *     and top 3 MSBs  define course gain
 *     ( 6dB steps)
 */
enum
{
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MUTE        = 0x70,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_30DB =  0x30,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_29DB =  0x31,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_28DB =  0x32,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_27DB =  0x33,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_26DB =  0x34,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_25DB =  0x35,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_24DB =  0x36,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_23DB =  0x41,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_22DB =  0x42,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_21DB =  0x43,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_20DB =  0x44,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_19DB =  0x45,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_18DB =  0x46,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_17DB =  0x01,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_16DB =  0x02,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_15DB =  0x03,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_14DB =  0x04,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_13DB =  0x05,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_12DB =  0x06,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_11DB =  0x61,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_10DB =  0x62,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_9DB  =  0x63,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_8DB  =  0x64,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_7DB  =  0x65,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_6DB  =  0x66,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_5DB  =  0x21,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_4DB  =  0x22,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_3DB  =  0x23,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_2DB  =  0x24,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_MINUS_1DB  =  0x25,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_0DB        =  0x26,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_1DB        =  0x27,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_2DB        =  0x28,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_3DB        =  0x29,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_4DB        =  0x2a,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_5DB        =  0x2b,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_GAIN_6DB        =  0x2c
};
typedef Uint8 BspTwl3029_Audio_VoiceCodecDownlinkGain;


/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Audio_SidetoneLevel
 *
 * @discussion
 * <b> Description  </b><br>
 *    Defines the audio sidetone gain values.
 *
 */
enum
{
    BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_23   = 0x0D,
    BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_20   = 0x0C,
    BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_17   = 0x06,
    BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_14   = 0x02,
    BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_11   = 0x07,
    BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_8    = 0x03,
    BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_5    = 0x00,
    BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_2    = 0x04,
    BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_1          = 0x01,
    BSP_TWL3029_AUDIO_SIDETONE_LEVEL_MUTE          = 0x08
};
typedef Uint8 BspTwl3029_Audio_SidetoneLevel;
/*===========================================================================*/
/*!
 * @typedef BspTwl3029_AudioSpeakerAmpGain
 *
 * @discussion
 * <b> Description  </b><br>
 *    Defines the audio speaker gain values.
 *
 * @constant BSP_TWL3029_AUDIO_SPEAKER_AMP_GAIN_2_5DB
 *    Speaker gain is 2.5DB.
 *
 * @constant BSP_TWL3029_AUDIO_SPEAKER_AMP_GAIN_8_5DB
 *    Speaker gain is 8.5DB.
 *
 * @constant BSP_TWL3029_AUDIO_SPEAKER_AMP_GAIN_MINUS_3_5DB
 *    Speaker gain is -3.5DB.
 *
 * @constant BSP_TWL3029_AUDIO_SPEAKER_AMP_GAIN_MINUS_22_5DB
 *    Speaker gain is -22.5DB.
 */
enum
{
   BSP_TWL3029_AUDIO_SPEAKER_AMP_GAIN_8_5DB        = 0,
   BSP_TWL3029_AUDIO_SPEAKER_AMP_GAIN_2_5DB        = 1,
   BSP_TWL3029_AUDIO_SPEAKER_AMP_GAIN_MINUS_3_5DB  = 3,
   BSP_TWL3029_AUDIO_SPEAKER_AMP_GAIN_MINUS_22_5DB = 4
   
};
typedef Uint8 BspTwl3029_Audio_SpeakerAmpGain;
/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Audio_FmRadioGain
 *
 * @discussion
 * <b> Description  </b><br>
 *    FM radio analog programmable relative gain.
 *    Volume control range is from -2DB to 14DB in steps of 2DB.
 *    Absolute gain at mic/auxi amp out = 4.9dB + relative gain
 */
enum
{
   BSP_TWL3029_AUDIO_FMRADIO_GAIN_0DB,
   BSP_TWL3029_AUDIO_FMRADIO_GAIN_2DB,
   BSP_TWL3029_AUDIO_FMRADIO_GAIN_4DB,
   BSP_TWL3029_AUDIO_FMRADIO_GAIN_6DB,
   BSP_TWL3029_AUDIO_FMRADIO_GAIN_8DB,
   BSP_TWL3029_AUDIO_FMRADIO_GAIN_10DB,
   BSP_TWL3029_AUDIO_FMRADIO_GAIN_12DB,
   BSP_TWL3029_AUDIO_FMRADIO_GAIN_14DB,
   BSP_TWL3029_AUDIO_FMRADIO_GAIN_MINUS_2DB
} ;
typedef Uint8  BspTwl3029_Audio_FmRadioGain;
/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Audio_StereoAudioVolume
 *
 * <b> Description  </b><br>
 * @discussion
 *    Data type for the stereo audio volume control.
 *    Volume control range is from 0DB to -30DB in steps of 1DB.
 */
enum
{
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_0DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_1DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_2DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_3DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_4DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_5DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_6DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_7DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_8DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_9DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_10DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_11DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_12DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_13DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_14DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_15DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_16DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_17DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_18DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_19DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_20DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_21DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_22DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_23DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_24DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_25DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_26DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_27DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_28DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_29DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MINUS_30DB,
   BSP_TWL3029_AUDIO_STEREO_AUDIO_VOLUME_CONTROL_MUTE
};

typedef Uint8 BspTwl3029_Audio_StereoAudioVolume;

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Audio_VoiceCodecDownlinkCourseGain
 *
 * <b> Description  </b><br>
 * @discussion
 *    Data type for the Voice codec downlink Course gain control.
 *    Volume control range is from 0DB to -24DB in steps of 6DB.
 */
enum
{
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_COURSE_GAIN_0DB 			= 	0x02,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_COURSE_GAIN_MINUS_6DB 	=	0x06,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_COURSE_GAIN_MINUS_12DB 	=	0x00,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_COURSE_GAIN_MINUS_18DB 	=	0x04,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_COURSE_GAIN_MINUS_24DB 	=	0x03,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_COURSE_GAIN_MUTE		 	=	0x05
};

typedef Uint8 BspTwl3029_Audio_VoiceCodecDownlinkCourseGain;

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Audio_VoiceCodecDownlinkFineGain
 *
 * <b> Description  </b><br>
 * @discussion
 *    Data type for the Voice codec downlink Course gain control.
 *    Volume control range is from 0DB to -24DB in steps of 6DB.
 */
enum
{
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_MINUS_6DB 	= 	0x00,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_MINUS_5DB  	=	0x01,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_MINUS_4DB  	=	0x02,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_MINUS_3DB  	=	0x03,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_MINUS_2DB  	=	0x04,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_MINUS_1DB   =	0x05,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_MINUS_0DB   =	0x06,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_1DB   		=	0x07,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_2DB   		=	0x08,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_3DB   		=	0x09,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_4DB   		=	0x0a,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_5DB   		=	0x0b,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_6DB   		=	0x0c
};

typedef Uint8 BspTwl3029_Audio_VoiceCodecDownlinkFineGain;
/*===========================================================================*/
/*!
 * @typedef BspTwl3029_AudioStereoSoftVolume
 *
 * <b> Description  </b><br>
 * @discussion
 *    Sets atereo audio soft volume. The format for this is
 *    1/FS*N where N is the number given at end of constants
 *   defined below
 *
 * @constant BSP_TWL3029_AUDIO_STEREO_SOFT_VOLUME_512
 * @constant BSP_TWL3029_AUDIO_STEREO_SOFT_VOLUME_128
 * @constant BSP_TWL3029_AUDIO_STEREO_SOFT_VOLUME_8
 * @constant BSP_TWL3029_AUDIO_STEREO_SOFT_VOLUME_HALF
 * @constant BSP_TWL3029_AUDIO_STEREO_SOFT_VOLUME_CONTROL_BYPASS
 *    Bypass audio stereo soft volume control. 
 */
enum
{
   BSP_TWL3029_AUDIO_STEREO_SOFT_VOLUME_512  = 0,
   BSP_TWL3029_AUDIO_STEREO_SOFT_VOLUME_128  = 0x02,
   BSP_TWL3029_AUDIO_STEREO_SOFT_VOLUME_8    = 0x04,
   BSP_TWL3029_AUDIO_STEREO_SOFT_VOLUME_HALF = 0x06,
   BSP_TWL3029_AUDIO_STEREO_SOFT_VOLUME_CONTROL_BYPASS = 0x1
};
typedef Uint8 BspTwl3029_AudioStereoSoftVolume;

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_AudioEarGain
 *
 * <b> Description  </b><br>
 * @discussion
 *    Sets the ear amplifier gain
 *
 * @constant BSP_TWL3029_AUDIO_EARGAIN_MINUS_11DB
 * @constant BSP_TWL3029_AUDIO_EARGAIN_1DB
 *
 */
enum
{
   BSP_TWL3029_AUDIO_EARGAIN_MINUS_11DB  = 0,
   BSP_TWL3029_AUDIO_EARGAIN_1DB         = 1 
};
typedef Uint8 BspTwl3029_AudioEarGain;




/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Audio_GainSettings
 *
 * <b> Description  </b><br>
 *    A struct considiting of various  gain values.
 *
 * @field speakerAmpGain
 *    Speaker amp  gain: -22.5, -3.5, 2.5, or 8.5dB .
 *
 * @field vulGain
 *    voice up line gain +/- 12 dB in 1dB steps, mute
 *
 * @field vdlGain
 *    voice dowm line gain - 30 - 0dB in 1dB steps, mute
 *
 * @field sideToneLevel
 *    sidetone level -23 - 1dB in 3db step, Mute.
 *
 * @field stereoVolRight
 *    stereo audio right level 0 - 30dB  in 1db step, Mute.
 *
 * @field stereoVolLeft
 *    stereo audio left level 0 - 30dB  in 1db step, Mute.
 *
 * @field fmRadioGain
 *    fm radio gain  - 2 - 14dB (releative) in 1dB steps, Mute.
 * @field earGain
 *    ear amp gain  1dB or -11dB
 */
typedef struct 
{    
  BspTwl3029_Audio_SpeakerAmpGain            speakerAmpGain;   
  BspTwl3029_Audio_VoiceCodecUplinkGain      vulGain;   
  BspTwl3029_Audio_VoiceCodecDownlinkGain    vdlGain; 
  BspTwl3029_Audio_SidetoneLevel             sidetoneLevel;
  BspTwl3029_Audio_StereoAudioVolume         stereoVolRight;
  BspTwl3029_Audio_StereoAudioVolume         stereoVolLeft;
  BspTwl3029_AudioStereoSoftVolume           stereoSoftVol;
  BspTwl3029_Audio_FmRadioGain               fmRadioGain;
  BspTwl3029_AudioEarGain                    earGain;
} BspTwl3029_Audio_GainSettings;

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Audio_GainSettings
 *
 * <b> Description  </b><br>
 *    A struct considiting of various  gain values.
 *
 * @field speakerAmpGain
 *    Speaker amp  gain: -22.5, -3.5, 2.5, or 8.5dB .
 *
 * @field vulGain
 *    voice up line gain +/- 12 dB in 1dB steps, mute
 *
 * @field vdlGain
 *    voice dowm line gain - 30 - 0dB in 1dB steps, mute
 */
typedef struct 
{    
  BspTwl3029_Audio_VoiceCodecDownlinkCourseGain    	vdlCourseGain;
  BspTwl3029_Audio_VoiceCodecDownlinkFineGain    	vdlFineGain;
}BspTwl3029_Audio_GainVDLSettings;

/*=============================================================================
 * Description:
 */

 /*===========================================================================*/
/*!
 * @typedef BspTwl3029FilterState
 *
 * <b> Description  </b><br>
 *    Defines the state of the audio filters. ( bits 5,6 of CTRL1 reg)
 *
 * @constant BSP_TWL3029_AUDIO_FILTER_VOICE_DOWNLINK_PATH
 *    Filter to the voice downlink path ( reg:CTRL1 , bit: VDLFBYP )
 *
 * @constant BSP_TWL3029_AUDIO_FILTER_VOICE_CODEC_HIGH_PASS
 *    High pass filter for uplink path ( reg:CTRL1 , bit: VULHPFBYP )
 */
 enum
{
   BSP_TWL3029_AUDIO_FILTER_VOICE_DOWNLINK_PATH = 1,
   BSP_TWL3029_AUDIO_FILTER_VOICE_UPLINK_HIGH_PASS = 2
};
typedef Uint8 BspTwl3029_Audio_Filter;

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Audio_FilterState
 *
 * <b> Description  </b><br>
 *    Defines the state of the audio filters.
 *
 * @constant BSP_TWL3029_AUDIO_FILTER_ENABLE
 *    Apply the filter to the audio signal.
 *
 * @constant BSP_TWL3029_AUDIO_FILTER_BYPASS
 *    Bypass the filter. The filter is not applied on the 
 *    audio signal.
 */
enum
{
    BSP_TWL3029_AUDIO_FILTER_ENABLE,
    BSP_TWL3029_AUDIO_FILTER_BYPASS
};
typedef Uint8 BspTwl3029_Audio_FilterState;
 
  
/*===========================================================================*/
/*!
 * @typedef BspTwl3029AudioClk
 *
 * <b> Description  </b><br>
 *    Data type for the stereo dac PLL reference clock.
 *
 * @constant BSP_TWL3029_AUDIO_CLOCK_13MHZ
 *    stereo dac PLL reference clock is 13MHz.
 *
 * @constant BSP_TWL3029_AUDIO_CLOCK_12MHZ
 *    stereo dac PLL reference clock is 12MHz.
 *
 * @constant BSP_TWL3029_AUDIO_CLOCK_19_2MHZ
 *    stereo dac PLL reference clock is 19.2 MHz.
 */
enum
{
   BSP_TWL3029_AUDIO_CLOCK_13MHZ,
   BSP_TWL3029_AUDIO_CLOCK_12MHZ,
   BSP_TWL3029_AUDIO_CLOCK_19_2MHZ
};
typedef Uint8 BspTwl3029_AudioClk;
 

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_AudioVoicePLLMode
 *
 * <b> Description  </b><br>
 *    Voice PLL is configured through register CTRL6. This is 
 *    used to set the folowing bits:
 *    VPLLON  - ( active hi) voice PLL ON
 *    VPLLPCHGZ - ( active lo) voice PLL pre-charge
 *
 * @constant 
 *    BSP_TWL3029_AUDIO_PLL_VOICE_PLL_ON_PRECHARGE_ON
 * @constant 
 *    BSP_TWL3029_AUDIO_PLL_VOICE_PLL_ON_PRECHARGE_OFF
 * @constant 
 *    BSP_TWL3029_AUDIO_PLL_VOICE_PLL_OFF_PRECHARGE_ON
 * @constant 
 *    BSP_TWL3029_AUDIO_PLL_VOICE_PLL_OFF_PRECHARGE_OFF
 */
enum
{
   BSP_TWL3029_AUDIO_PLL_VOICE_PLL_ON_PRECHARGE_ON =   1,  
   BSP_TWL3029_AUDIO_PLL_VOICE_PLL_ON_PRECHARGE_OFF =  3,
   BSP_TWL3029_AUDIO_PLL_VOICE_PLL_OFF_PRECHARGE_ON =  0,
   BSP_TWL3029_AUDIO_PLL_VOICE_PLL_OFF_PRECHARGE_OFF = 2
  
};
typedef Uint8 BspTwl3029_AudioVoicePllMode;
#define BSP_TWL3029_AUDIO_PLL_MAX 3
                                                   
//typedef Uint8   BspTwl3029_AudioClkSource;
/*===========================================================================*/
/*!
 * @typedef BspTwl3029_AudioVspClkFreq
 *
 * <b> Description  </b><br>
 *    Data type for voice serial port bit clock (VCK) frequency.
 *
 * @constant BSP_TWL3029_AUDIO_VCLK_FREQ_500KHZ
 *    VCK frequency is set to 500 KHz.
 *
 * @constant BSP_TWL3029_AUDIO_VCLK_FREQ_1_OR_2MHZ
 *    VCL frquency is set to 1 or 2MHz depending on the BspTwl3029VoiceCodecPathType.
 *    If voice codec is set to BSP_TWL3029_AUDIO_CODEC_PATH_NARROW_BAND then freq is 1MHz.
 *    If voice codec is set to BSP_TWL3029_AUDIO_CODEC_PATH_WIDE_BAND then freq is 2MHz.
 */
enum
{
   BSP_TWL3029_AUDIO_VCLK_FREQ_500KHZ,
   BSP_TWL3029_AUDIO_VCLK_FREQ_1_OR_2MHZ
};
typedef Uint8 BspTwl3029_AudioVspClkFreq;
                                    
/*===========================================================================*/
/*!
 * @typedef BspTwl3029_AudioVspClkMode
 *
 * <b> Description  </b><br>
 *    Defines the voice serial port bit clock (VCK) mode of the voice codec.
 *
 * @constant BSP_TWL3029_AUDIO_VCLK_MODE_BURST
 *    VCK is set to burst mode.
 *    
 * @constant BSP_TWL3029_AUDIO_VCLK_MODE_CONTINUOUS
 *    VCK is set to continuous mode.
 */
enum
{
   BSP_TWL3029_AUDIO_VCLK_MODE_BURST,
   BSP_TWL3029_AUDIO_VCLK_MODE_CONTINUOUS
};
typedef Uint8 BspTwl3029_AudioVspClkMode;
                                   
                                          
/*===========================================================================*/
/*!
 * @typedef BspTwl3029_VoiceCodecPathBandWidth
 *
 * @discussion
 *    Defines the voice speech bacb width 
 *
 * @constant BSP_TWL3029_VOICE_WIDEBAND
 *    8kHz bandwidth
 *
 * @constant BSP_TWL3029_VOICE_NARROWBAND
 *    4kHz bandwidth
 */
 enum
{
    BSP_TWL3029_AUDIO_VOICE_NARROWBAND     = 0,
    BSP_TWL3029_AUDIO_VOICE_WIDEBAND       = 1
    
};
typedef Uint8 BspTwl3029_VoiceCodecPathBandWidth;
  
/*===========================================================================*/
/*!
 * @typedef BspTwl3029_AudioStereoPLLMode
 *
 * <b> Description  </b><br>
 *    Stereo PLL is configured through register CTRL6. This is 
 *    used to set the following bits:
 *    Bit 1 - STLLON  - ( active hi) voice PLL ON
 *    Bit 3 - STPLLPCHGZ - ( active lo) voice PLL pre-charge
 *    Bit 4 - STPLLSPEEDUP - ( active hi) voice PLL pre-charge
 *
 * @constant 
 *    BSP_TWL3029_STEREO_PLL_ON_PRECHARGE_ON_SPEEDUP_ON
 * @constant 
 *    BSP_TWL3029_STEREO_PLL_ON_PRECHARGE_ON_SPEEDUP_OFF
 * @constant 
 *    BSP_TWL3029_STEREO_PLL_ON_PRECHARGE_OFF_SPEEDUP_ON
 * @constant 
 *    BSP_TWL3029_STEREO_PLL_ON_PRECHARGE_OFF_SPEEDUP_OFF
  @constant 
 *    BSP_TWL3029_STEREO_PLL_OFF_PRECHARGE_ON_SPEEDUP_ON
 * @constant 
 *    BSP_TWL3029_STEREO_PLL_OFF_PRECHARGE_OFF_SPEEDUP_ON
 * @constant 
 *    BSP_TWL3029_STEREO_PLL_OFF_PRECHARGE_ON_SPEEDUP_OFF
 * @constant 
 *    BSP_TWL3029_STEREO_PLL_OFF_PRECHARGE_OFF_SPEEDUP_ON
 * @constant 
 *    BSP_TWL3029_STEREO_PLL_OFF_PRECHARGE_OFF_SPEEDUP_OFF
 */

enum
{
 
     BSP_TWL3029_STEREO_PLL_ON_PRECHARGE_ON_SPEEDUP_ON    = 0x12,
     BSP_TWL3029_STEREO_PLL_ON_PRECHARGE_ON_SPEEDUP_OFF   = 0x02,
     BSP_TWL3029_STEREO_PLL_ON_PRECHARGE_OFF_SPEEDUP_OFF  = 0x1A,
     BSP_TWL3029_STEREO_PLL_ON_PRECHARGE_OFF_SPEEDUP_ON   = 0x0A,
     
     BSP_TWL3029_STEREO_PLL_OFF_PRECHARGE_ON_SPEEDUP_ON   = 0x10,
     BSP_TWL3029_STEREO_PLL_OFF_PRECHARGE_ON_SPEEDUP_OFF  = 0x00,
     BSP_TWL3029_STEREO_PLL_OFF_PRECHARGE_OFF_SPEEDUP_ON  = 0x18,
     BSP_TWL3029_STEREO_PLL_OFF_PRECHARGE_OFF_SPEEDUP_OFF = 0x08
  
};
typedef Uint8 BspTwl3029_Audio_StereoPllMode;
#define BSP_TWL3029_STEREO_PLL_MAX 0x1A  

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_AudioStereoSamplingRate
 *
 * <b> Description  </b><br>
 *    Data type for the stereo dac audio sampling rate frequency.
 *
 * @constant BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_48KHZ
 *
 * @constant BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_44_1KHZ
 *
 * @constant BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_32KHZ
 *
 * @constant BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_24KHZ
 *
 * @constant BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_22_05KHZ
 *
 * @constant BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_16KHZ
 *
 * @constant BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_12KHZ
 *
 * @constant BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_11_025KHZ
 *
 * @constant BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_8KHZ
 */
enum
{
   BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_8KHZ,  
   BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_11_025KHZ,
   BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_12KHZ,
   BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_16KHZ,
   BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_22_05KHZ,
   BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_24KHZ,
   BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_32KHZ,
   BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_44_1KHZ,
   BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_48KHZ
};
typedef Uint8 BspTwl3029_Audio_StereoSamplingRate;

#define BSP_TWL3029_STEREO_SAMPLING_MAX 0x08 
 

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Audio2MonoState
 *
 * <b> Description  </b><br>
 *    defines which ( if any) mono channel stereo should be converted to
 *
 *  @constant BSP_TWL3029_AUDIO_STEREO2MONO_OFF
 *
 *  @constant BSP_TWL3029_AUDIO_STEREO2MONO_RIGHT
 *
 *  @constant BSP_TWL3029_AUDIO_STEREO2MONO_LEFT
 *
 *  @constant BSP_TWL3029_AUDIO_STEREO2MONO_LEFT_AND_RIGHT
 *
 *  Note: I2S power needs to be ON for VMEMO to function
 *        VMEMO can only be used with  8kHz and 16kHz frequency
 */
enum
{ 
    BSP_TWL3029_AUDIO_STEREO2MONO_OFF,
    BSP_TWL3029_AUDIO_STEREO2MONO_RIGHT,
    BSP_TWL3029_AUDIO_STEREO2MONO_LEFT,
    BSP_TWL3029_AUDIO_STEREO2MONO_LEFT_AND_RIGHT
};
typedef Uint8 BspTwl3029_Audio2MonoState ;  
 
/*===========================================================================*/
/*!
 * @typedef BspTwl3029_AudioPopSource
 *
 * <b> Description  </b><br>
 *    enum used for selection of output for anti POP control
 *
 *  @constant BSP_TWL3029_AUDIO_POP_HSO
 *
 *  @constant BSP_TWL3029_AUDIO_POP_EAR
 *
 *  @constant BSP_TWL3029_AUDIO_POP_AUX0
 *
 *  @constant BSP_TWL3029_AUDIO_POP_CARKIT
 *
 *  
 */
enum
{ 
    BSP_TWL3029_AUDIO_POP_HSO    = 1,
    BSP_TWL3029_AUDIO_POP_EAR    = 2,
    BSP_TWL3029_AUDIO_POP_AUXO   = 4,
    BSP_TWL3029_AUDIO_POP_CARKIT = 8
};
typedef Uint8 BspTwl3029_AudioPopSource ;  
/*===========================================================================*/
/*!
 * @typedef BspTwl3029_AudioPopPrechargeEnable
 *
 * <b> Description  </b><br>
 *    enum used for seting/resing CHG bits in POPMAN register 
 *    when maunal pop control is in use.
 *
 *  @constant BSP_TWL3029_AUDIO_POP_CHG_ENABLE
 *
 *  @constant BSP_TWL3029_AUDIO_POP_CHG_DISABLE
 *
 */
enum
{ 
    BSP_TWL3029_AUDIO_POP_CHG_DISABLE,
    BSP_TWL3029_AUDIO_POP_CHG_ENABLE
    
};
typedef Uint8 BspTwl3029_AudioPopPrechargeEnable ;  

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_AudioPopPrechargeEnable
 *
 * <b> Description  </b><br>
 *    enum used for enabling anti POP 
 *
 *  @constant BSP_TWL3029_AUDIO_POP_CHG_DISABLE
 *
 *  @constant BSP_TWL3029_AUDIO_POP_CHG_ENABLE_AUTO
 *       enabled for auto mode 
 *
 *  @constant BSP_TWL3029_AUDIO_POP_CHG_ENABLE_MANUAL
 *       enabled for manual mode 
 */
enum
{ 
    BSP_TWL3029_AUDIO_POP_DISABLE = 0,
    BSP_TWL3029_AUDIO_POP_CHG_ENABLE_AUTO = 1,
    BSP_TWL3029_AUDIO_POP_CHG_ENABLE_MANUAL = 2
};
typedef Uint8 BspTwl3029_AudioPopEnable ;

#define TI_BSP_FF_AUDIO_ON_OFF
#ifdef TI_BSP_FF_AUDIO_ON_OFF
	
	/******************************* AUDIO ON/OFF *****************************/
	#define AUDIO_UPLINK_ON		0x01
	#define AUDIO_UPLINK_OFF	0x02
	#define AUDIO_UPLINK_NONE	0x00

	#define  OUTEN_ENABLE     0x00  
	#define  OUTEN_DISABLE    0x01
	#define  OUTEN_NONE       0x02  


	typedef Uint8	T_AUDIO_UPLINK_ON_OFF_CONTROL;

	#define AUDIO_DOWNLINK_ON		0x04
	#define AUDIO_DOWNLINK_OFF		0x08
	#define AUDIO_DOWNLINK_NONE		0x00

	typedef Uint8	T_AUDIO_DOWNLINK_ON_OFF_CONTROL;


	#define AUDIO_UPLINK_ON_DOWNLINK_ON		(AUDIO_UPLINK_ON | AUDIO_DOWNLINK_ON)
	#define AUDIO_UPLINK_OFF_DOWNLINK_ON	(AUDIO_UPLINK_OFF | AUDIO_DOWNLINK_ON)
	#define AUDIO_UPLINK_ON_DOWNLINK_OFF	(AUDIO_UPLINK_ON | AUDIO_DOWNLINK_OFF)
	#define AUDIO_UPLINK_OFF_DOWNLINK_OFF	(AUDIO_UPLINK_OFF | AUDIO_DOWNLINK_OFF)

	typedef Uint8 	T_AUDIO_ON_OFF_CONTROL;

	typedef void (*AUDIO_CALLBACK_FUNC) (Uint8);

	typedef struct {
		AUDIO_CALLBACK_FUNC audio_on_off_callback;
		Uint8 callback_val; 
	} T_AUDIO_ON_OFF_CONTROL_RETURN;

	enum {
	AUDIO_UPLINK_ON_INDEX,
	AUDIO_UPLINK_OFF_INDEX,
	AUDIO_DOWNLINK_ON_INDEX,
	AUDIO_DOWNLINK_OFF_INDEX,	
	AUDIO_UPLINK_ON_DOWNLINK_ON_INDEX,
	AUDIO_UPLINK_OFF_DOWNLINK_ON_INDEX,
	AUDIO_UPLINK_ON_DOWNLINK_OFF_INDEX,
	AUDIO_UPLINK_OFF_DOWNLINK_OFF_INDEX
	};

	typedef Uint8 T_AUDIO_ON_OFF_INDEX;

	#define DOWNLINK_WAIT_1_TDMA_EXPIRY 1
	#define UPLINK_WAIT_1_TDMA_EXPIRY	2

	typedef struct {
		BspTwl3029_I2C_RegData outen1;
		BspTwl3029_I2C_RegData outen2;
		BspTwl3029_I2C_RegData outen3;
		 BspTwl3029_I2C_RegData classD;
	}T_AUDIO_OUTEN_REG;

    void bspTwl3029_audio_on_off_init( );

    BspTwl3029_ReturnCode bspTwl3029_audio_on_off_control (T_AUDIO_UPLINK_ON_OFF_CONTROL uplink_on_off_control,
					T_AUDIO_DOWNLINK_ON_OFF_CONTROL downlink_on_off_control, 
					T_AUDIO_ON_OFF_CONTROL_RETURN on_off_return, BspTwl3029_I2C_RegData dl_off_control);

    void bspTwl3029_audio_on_off_callback (BspI2c_TransactionId transaction_id);

    #ifdef TI_BSP_FF_AUDIO_ON_OFF_TEST
    BspTwl3029_ReturnCode audio_on_off_test ();
    #endif

#endif
/*=============================================================================*/
/*!
 * @function bspTwl3029_Audio_init
 *
 * <b> Description  </b><br>
 *    Initialises triton audio .
 *
 * @param  - (none)
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success and
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 *
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_init( void );



/*=============================================================================*/
/*!
 * @function bspTwl3029_Audio_powerControl
 *
 * <b> Description  </b><br>
 *    Provides power control to the audio blocks.
 *
 * @param pwrControl
 *    The power control to be applied on the audio block.
 *
 * @param pwrModule
 *    power module(s) to modify (VUL/VDL/STS )
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success and
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 *
 */

BspTwl3029_ReturnCode
bspTwl3029_Audio_powerControl( BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                               BspTwl3029_Audio_PowerControl powerControl, 
			       BspTwl3029_Audio_PowerModule  pwrModule );

/*=============================================================================*/
/*!
 * @function bspTwl3029_Audio_configureInputPath
 *
 * <b> Description  </b><br>
 *    Set up the uplink path for the audio output. This sets up the 
 *    audio input device for downlink audio or stereo audio.
 *
 *   <i> More than 1 audio inputpaths can be selected </i>
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param inputPathId
 *    The input device ( speaker/headset/...) which will receive the audio/voice.
 *
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success and
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_configureInputPath( BspTwl3029_I2C_CallbackPtr callbackFuncPtr, 
                                     BspTwl3029_Audio_InputPathId   inputPathId);


/*=============================================================================*/
/*!
 * @function bspTwl3029_Audio_configureOutputPath
 *
 * <b> Description  </b><br>
 *    Set up the downlink path for the audio output. This sets up the 
 *    audio output device for downlink audio or stereo audio.
 *
 *   <i> More than 1 audio outputpaths can be enabled simultaneously.
 *   To reset an audio output path the audio source value has to be 
 *   BSP_TWL3029_AUDIO_OUTPUT_SOURCE_RESET. </i>
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 
 * @param outputPathId
 *    The output device ( speaker/headset/...) which will receive the audio/voice.
 *
 * @param audioSource
 *    Pointer to array containing audio source type for each output path
 *                ( voice/stereo/...).
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success and
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 */

BspTwl3029_ReturnCode 
bspTwl3029_Audio_configureOutputPath( BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                                      BspTwl3029_Audio_OutputPathId  outputPathId, 
				      BspTwl3029_AudioOutputSource* audioSourcePtr );

/*=============================================================================*/
/*!
 * @function bspTwl3029_Audio_configureMicBias
 *
 * <b> Description  </b><br>
 *    Set up the Mic Bias for audio inputs. 
 *
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 * 
 * @param micBiasId
 *    The Microphone Bias voltage.
 *
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success and
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 */

BspTwl3029_ReturnCode
bspTwl3029_Audio_configureMicBias( BspTwl3029_I2C_CallbackPtr callbackFuncPtr, 
                                     BspTwl3029_Audio_MicBiasId   micBiasId);

/*===========================================================================*/
/*!
 * @function bspTwl3029_Audio_gainControl
 *
* <b> Description  </b><br>
 *    Defines the voice and audio speaker gain values for the varoius 
 *    audio /voice inputs/ outputs. One or Multiple gain sources can be  with
 *    modified a sinlge call of this functions
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param gainSrc
 *    bit field uses to define on or more gain variables ( i.e AUGRGAIN VULGAIN
 *    etc..) to modify 
 *
 *  @param newGainSettings a struct containing gain settings for the various
 *    sources 
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */
 
 
BspTwl3029_ReturnCode
bspTwl3029_Audio_gainControl  ( BspTwl3029_I2C_CallbackPtr    callbackFuncPtr,
                                BspTwl3029_Audio_GainSources    gainSrc,
                                BspTwl3029_Audio_GainSettings* newGainSettings);


/*===========================================================================*/
/*!
 * @function bspTwl3029_Audio_Mute
 *
 * <b> Description  </b><br>
 *    mutes or unmutes path(s) specified by mutePath
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param muteControl
 *    specifies if mute is to be enabled or disabled
 *
 * @param BspTwl3029_Audio_MutePath 
 *    specifies path to mute - input path, output path or both   
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */

BspTwl3029_ReturnCode
bspTwl3029_Audio_Mute( BspTwl3029_I2C_CallbackPtr  callbackFuncPtr,
                               BspTwl3029_Audio_MuteControl   muteControl,
                       BspTwl3029_Audio_MutePath    mutePath );

/*=============================================================================*/
 /*!
 * @function bspTwl3029_Audio_setFilterState
 *
 * <b> Description  </b><br>
 *    Sets /resets bypassing of VUL high pass filter and/or VDL filter
 *
 * @param callbackPtr
 *
 * @param filter 0 - VDL filter
 *               1 - VULHP filter
 *               3 - both
 *
 * @param filterstate  0 - enable
 *                     1 - bypass
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */
BspTwl3029_ReturnCode 
bspTwl3029_Audio_setFilterState ( BspTwl3029_I2C_CallbackPtr  callbackPtr,
                                  BspTwl3029_Audio_Filter      filter,
                                  BspTwl3029_Audio_FilterState filterState);


 /*===========================================================================*/
/*!
 * @function bspTwl3029_Audio_voiceCodecConfigure
 *
 * <b> Description  </b><br>
 *    configures the voice codec susbsytem.
 * 
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param vspClkFreq
 *    The clock frequency of the voice serial port.
 *
 * @param vspClkMode
 *    The voice serial port clock mode.
 *
 * @param bandwidth 
 *    The bandwidth of the voice codec path.
 * 
 *
 * @result
 *    returns BspTwl3029_ReturnCode 
 *
 */

BspTwl3029_ReturnCode
bspTwl3029_Audio_voiceCodecConfigure(BspTwl3029_I2C_CallbackPtr  callbackPtr,
                                     BspTwl3029_AudioVoicePllMode       pll,                                      
                                     BspTwl3029_AudioVspClkFreq         vspClkFreq,                                     
                                     BspTwl3029_AudioVspClkMode         vspClkMode,                                    
                                     BspTwl3029_VoiceCodecPathBandWidth bandwidth );


/*=============================================================================
 * Description:
 * @function bspTwl3029_Audio_gainVDLConfig
 *
 * <b> Description  </b><br>
 *    configures the voice downlink path fine and Course gain.
 * 
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_gainVDLConfig( BspTwl3029_I2C_CallbackPtr    callbackFuncPtr,
                                BspTwl3029_Audio_GainVDLSources    gainSrc,
                                BspTwl3029_Audio_GainVDLSettings* newGainSettings);

/*===========================================================================*/
/*!
 * @function bspTwl3029_Audio_stereoDacConfigure
 *
 * <b> Description  </b><br>
 *    Updates the clk for the stereo dac susbsytem.
 *
 * @param pll
 *    The  dac PLL settings (STPLLON,STPLLCHGZ and STPLLSPEEDUP bits of CRTL6 reg).
 *
 * @param samplingRate
 *    stereo audio sampling rate frequency.
 *
 * @param  mono
 *     defines if stereo2mono is enabled and on which (L or R ) channel 
 * @param i2sOn
 *    set up  I2S power on and VMEMO.
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */

BspTwl3029_ReturnCode
bspTwl3029_Audio_stereoDacConfigure(BspTwl3029_I2C_CallbackPtr           callbackFuncPtr,
                                    BspTwl3029_Audio_StereoPllMode       pll,
                                    BspTwl3029_Audio2MonoState           mono,
                                    BspTwl3029_Audio_StereoSamplingRate  samplingRate);

/*===========================================================================*/
/*!
 * @function bspTwl3029_Audio_stereoSamplingRateConfigure
 *
 * <b> Description  </b><br>
 *    Updates Stereo sampling rate when the Stereo path is not powered-on.
 *
 * @param samplingRate
 *    stereo audio sampling rate frequency.
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_stereoSamplingRateConfigure (BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                                    BspTwl3029_Audio_StereoSamplingRate  samplingRate );

/*===========================================================================*/
/*!
 * @function bspTwl3029_Audio_stereoDacConfigure
 *
 * <b> Description  </b><br>
 *    Updates Stereo Mono configuration.
 *
 * @param BspTwl3029_Audio2MonoState
 *    Mono configuration.
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_stereoMonoConfigure (BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                                    BspTwl3029_Audio2MonoState           mono );

/*===========================================================================*/
/*!
 * @function bspTwl3029_Audio_vmemoConfigure
 * <b> Description  </b><br>
 *     set up  I2S power on and VMEMO.
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param i2sOn
 *    set up  I2S power on and VMEMO.
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */

BspTwl3029_ReturnCode
bspTwl3029_Audio_vmemoConfigure(BspTwl3029_I2C_CallbackPtr           callbackFuncPtr,
                                BspTwl3029_AudioI2sVoiceUplinkState  i2sOn );




/*===========================================================================*/
/*!
 * @function bspTwl3029_Audio_popPrecharge
 * <b> Description  </b><br>
 *     Switches antipot precharger on and off. Note this function is has no
 *     effect if ani POP auto is active. 
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param popOutput
 *    bit map of 1 or more output sources whose precharge enable status is to
 *    be modified.
 *
 * @param chgEnable
 *    prechrger enable or disable
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */

BspTwl3029_ReturnCode
bspTwl3029_Audio_popPrecharge(BspTwl3029_I2C_CallbackPtr   callbackFuncPtr,
                                BspTwl3029_AudioPopSource  popOutput,
				BspTwl3029_AudioPopPrechargeEnable chgEnable );

/*===========================================================================*/
/*!
 * @function bspTwl3029_Audio_popEnable
 * <b> Description  </b><br>
 *     Enables /disables anti POP 
 *     There is no specific bit(s) in the two anti POP registers for 
 *     enabling/disabling anti POP, however if anti POP is required the post-
 *     discharge capacitor  must always be enabled,  if not required then the 
 *     post discharger capacitoe can be left disabled  
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param popOutput
 *    bit map of 1 or more output sources whose precharge enable status is to
 *    be modified.
 *
 * @param PopEnable
 *    disable anti POP or enable to either MANUAL or AUTO mode
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */

BspTwl3029_ReturnCode
bspTwl3029_Audio_popEnable(BspTwl3029_I2C_RegData *popEnable );

/*===========================================================================*/
/*!
 * @function bspTwl3029_setClassD_mode
 * <b> Description  </b><br>
 *     Enables /disables Class D part for handfree 
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */
 BspTwl3029_ReturnCode
  bspTwl3029_Audio_setClassD_mode(Uint8);

#define TI_BSP_AUDIO_CONFIG_READ

#ifdef TI_BSP_AUDIO_CONFIG_READ
/*===========================================================================*/
/*!
 * @function bspTwl3029_Audio_getInputPath
 * <b> Description  </b><br>
 *     This returns the Input path configurations. 
 *     
 * @param inputPath
 *    [OUT] pointer to BspTwl3029_Audio_InputPathId that gives the current 
 *    input path configured.
 *
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */

BspTwl3029_ReturnCode  
bspTwl3029_Audio_getInputPath( BspTwl3029_Audio_InputPathId* inputPath );

/*===========================================================================*/
/*!
 * @function bspTwl3029_Audio_getGain
 *
* <b> Description  </b><br>
 *    Gets the voice and audio speaker gain values for the varoius 
 *    audio /voice inputs/ outputs. One or Multiple gain sources can be  with
 *    modified a sinlge call of this functions
 *
 * @param gainSrc
 *    bit field uses to define on or more gain variables ( i.e AUGRGAIN VULGAIN
 *    etc..) to modify 
 *
 *  @param newGainSettings [OUT] a struct containing gain settings for the various
 *    sources 
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */
 
BspTwl3029_ReturnCode
bspTwl3029_Audio_getGain (   BspTwl3029_Audio_GainSources    gainSrc,
                             BspTwl3029_Audio_GainSettings* newGainSettings);

/*=============================================================================
 * Description:
 * @function bspTwl3029_Audio_gainVDLConfig
 *
 * <b> Description  </b><br>
 *    Gets the voice downlink path fine and Course gain.
 * 
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_getVDLGain( BspTwl3029_Audio_GainVDLSources    gainSrc,
                             BspTwl3029_Audio_GainVDLSettings* newGainSettings);

/*=============================================================================*/
/*!
 * @function bspTwl3029_Audio_configureMicBias
 *
 * <b> Description  </b><br>
 *    Gets the Mic Bias for audio inputs. 
 *
 * @param micBiasId
 *    The Microphone Bias voltage.
 *
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success and
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 */

BspTwl3029_ReturnCode
bspTwl3029_Audio_getMicBias( BspTwl3029_Audio_MicBiasId * inputPathId);

/*=============================================================================*/
 /*!
 * @function bspTwl3029_Audio_getFilterState
 *
 * <b> Description  </b><br>
 *    Sets /resets bypassing of VUL high pass filter and/or VDL filter
 *
 * @param callbackPtr
 *
 * @param filter 0 - VDL filter
 *               1 - VULHP filter
 *               3 - both
 *
 * @param filterstate  0 - enable
 *                     1 - bypass
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */
BspTwl3029_ReturnCode 
bspTwl3029_Audio_getFilterState (BspTwl3029_Audio_Filter      filter,
                                  BspTwl3029_Audio_FilterState *filterState);

/*===========================================================================*/
/*!
 * @function bspTwl3029_Audio_getStereoSamplingRate
 *
 * <b> Description  </b><br>
 *    Updates Stereo sampling rate when the Stereo path is not powered-on.
 *
 * @param samplingRate
 *    stereo audio sampling rate frequency.
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */

BspTwl3029_ReturnCode
bspTwl3029_Audio_getStereoSamplingRate (BspTwl3029_Audio_StereoSamplingRate  *samplingRate );

/*===========================================================================*/
/*!
 * @function bspTwl3029_Audio_getStereoMonoConfigData
 *
 * <b> Description  </b><br>
 *    Updates Stereo Mono configuration.
 *
 * @param BspTwl3029_Audio2MonoState
 *    Mono configuration.
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */
 BspTwl3029_ReturnCode
bspTwl3029_Audio_getStereoMonoConfigData (BspTwl3029_Audio2MonoState *mono );

/*===========================================================================*/
/*!
 * @function bspTwl3029_getClassD_mode
 * <b> Description  </b><br>
 *     gets the state of Class D part for handfree(Reg CTRL3: bit 7) 
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */
 BspTwl3029_ReturnCode
  bspTwl3029_Audio_getClassD_mode(Uint8* classD);

#endif

BspTwl3029_ReturnCode
bspTwl3029_Audio_Stop_Stereopath(BspTwl3029_I2C_CallbackPtr callbackFuncPtr, BspTwl3029_I2C_RegData outen_control);

void BspTwl3029_cache_outen_reg (BspTwl3029_I2C_RegData outen1, BspTwl3029_I2C_RegData outen2, BspTwl3029_I2C_RegData outen3);
enum
      {
        AUDIO_HEADSET_PLUGGED                = 1,
        AUDIO_HEADSET_HOOK_DETECT = 2,
        AUDIO_HEADSET_UNPLUGGED = 3,
        AUDIO_CARKIT_PLUGGED = 4, 
        AUDIO_CARKIT_UNPLUGGED = 5
      };
      typedef unsigned char T_AUDIO_ACCESSORY_NOTIFICATION;

      enum
      {
        AUDIO_ACCESSORY_HEADSET                = 0,
        AUDIO_ACCESSORY_HOOK                      =1,
	AUDIO_ACCESSORY_CARKIT                    = 2,
        AUDIO_ACCESSORY_MAX                        = 3

	 };
      typedef unsigned char T_AUDIO_ACCESSORY_TYPE;

      typedef void* ACCESSORY_CALLBACK_PARAM;

      typedef void (*ACCESSORY_CALLBACK_FUNC)( T_AUDIO_ACCESSORY_NOTIFICATION, ACCESSORY_CALLBACK_PARAM);
	  
//	  typedef accessory_callback_func ACCESSORY_CALLBACK_FUNC;

      typedef struct 
      {
        ACCESSORY_CALLBACK_FUNC  callbackFunc; 
        ACCESSORY_CALLBACK_PARAM       callbackVal;
      } T_ACCESSORY_CALLBACK;

typedef enum 
	{
	BspAudioError=-1,  
	BspAudioSuccess=0
     } BspAudioReturn;


BspTwl3029_ReturnCode Bsptwl3029_audio_accessory_register_notification(T_ACCESSORY_CALLBACK accessory_callback, 
      	 								T_AUDIO_ACCESSORY_TYPE accessory);
    BspAudioReturn audio_accessory_int_init();
BspTwl3029_ReturnCode BspTwl3029_hook_detect_enable(BspTwl3029_Audio_MicBiasId biasval);
  	                                                                      
BspAudioReturn Bsptwl3029_audio_accessory_Unregister_notification(T_AUDIO_ACCESSORY_TYPE accessory);
BspTwl3029_ReturnCode BspTwl3029_hook_detect_disable();


#endif
