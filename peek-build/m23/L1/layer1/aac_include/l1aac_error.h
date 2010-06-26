/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1AAC_ERROR.H
 *
 *        Filename l1aac_error.h
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#ifndef _L1AAC_ERROR_H_
#define _L1AAC_ERROR_H_

#if (L1_AAC == 1)

#define C_AAC_ERR_NONE                           (0x00000001) /* (0x00000001<<0) */
#define C_AAC_SYNC_WORD_NOT_FOUND                (0x00000002) /* (0x00000001<<1) */
#define C_AAC_ADTS_HEADER_HAS_INVALID_SYNCWORD   (0x00000004) /* (0x00000001<<2) */
#define C_AAC_ADTS_LAYER_DATA_ERROR              (0x00000008) /* (0x00000001<<3) */
#define C_AAC_NUM_CHANNELS_EXCEEDED              (0x00000010) /* (0x00000001<<4) */
#define C_AAC_PREDICTION_DETECTED                (0x00000020) /* (0x00000001<<5) */
#define C_AAC_LFE_CHANNEL_DETECTED               (0x00000040) /* (0x00000001<<6) */
#define C_AAC_GAIN_CONTROL_DETECTED              (0x00000080) /* (0x00000001<<7) */
#define C_AAC_CHANNEL_ELEMENT_PARSE_ERROR        (0x00000100) /* (0x00000001<<8) */
#define C_AAC_PULSE_DATA_NOT_ALWD_SHORT_BLK      (0x00000200) /* (0x00000001<<9) */
#define C_AAC_MAX_SFB_TOO_LARGE_SHORT            (0x00000400) /* (0x00000001<<10) */
#define C_AAC_MAX_SFB_TOO_LARGE_LONG             (0x00000800) /* (0x00000001<<11) */
#define C_AAC_ERROR_ON_DATA_CHANNEL              (0x00001000) /* (0x00000001<<12) */
#define C_AAC_COUPLING_CHANNEL_DETECTED          (0x00002000) /* (0x00000001<<13) */
#define C_AAC_ADTS_PROFILE_ERROR                 (0x00004000) /* (0x00000001<<14) */
#define C_AAC_ADIF_PROFILE_ERROR                 (0x00008000) /* (0x00000001<<15) */
#define C_AAC_INVALID_ELEMENT_ID                 (0x00010000) /* (0x00000001<<16) */
#define C_AAC_SAMP_FREQ_NOT_SUPPORTED            (0x00020000) /* (0x00000001<<17) */
#define C_AAC_ADTS_FRAME_LEN_INCORRECT           (0x00040000) /* (0x00000001<<18) */
#define C_AAC_ALG_ERROR                          (0x00080000) /* (0x00000001<<19) */
#define C_AAC_ERR_DMA_DROP                       (0x00100000) /* (0x00000001<<20) */
#define C_AAC_ERR_DMA_TOUT_SRC                   (0x00200000) /* (0x00000001<<21) */
#define C_AAC_ERR_DMA_TOUT_DST                   (0x00400000) /* (0x00000001<<22) */
#define C_AAC_CHECK_BUFFER_KO                    (0x00800000) /* (0x00000001<<23) */

#endif    // L1_AAC

#endif    // _L1AAC_ERROR_H_
