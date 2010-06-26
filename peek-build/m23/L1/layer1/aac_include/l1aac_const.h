/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1AAC_CONST.H
 *
 *        Filename l1aac_const.h
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#ifndef _L1AAC_CONST_H_
#define _L1AAC_CONST_H_

// DSP AAC background task id
#define C_BGD_AAC   11

// AAC API base address
#define C_AAC_API_BASE_ADDRESS 0x1AD0


// AAC stream information
// ----------------------

// frequency rate
#define C_AAC_STREAM_FREQ_48000          0
#define C_AAC_STREAM_FREQ_44100          1
#define C_AAC_STREAM_FREQ_32000          2
#define C_AAC_STREAM_FREQ_24000          3
#define C_AAC_STREAM_FREQ_22050          4
#define C_AAC_STREAM_FREQ_16000          5
#define C_AAC_STREAM_FREQ_12000          6
#define C_AAC_STREAM_FREQ_11025          7
#define C_AAC_STREAM_FREQ_8000           8

// bitrate
#define C_AAC_STREAM_BITRATE_384         0
#define C_AAC_STREAM_BITRATE_320         1
#define C_AAC_STREAM_BITRATE_256         2
#define C_AAC_STREAM_BITRATE_224         3
#define C_AAC_STREAM_BITRATE_192         4
#define C_AAC_STREAM_BITRATE_160         5
#define C_AAC_STREAM_BITRATE_128         6
#define C_AAC_STREAM_BITRATE_112         7
#define C_AAC_STREAM_BITRATE_96          8
#define C_AAC_STREAM_BITRATE_80          9
#define C_AAC_STREAM_BITRATE_64          10
#define C_AAC_STREAM_BITRATE_56          11
#define C_AAC_STREAM_BITRATE_48          12
#define C_AAC_STREAM_BITRATE_40          13
#define C_AAC_STREAM_BITRATE_32          14
#define C_AAC_STREAM_BITRATE_24          15
#define C_AAC_STREAM_BITRATE_16          16
#define C_AAC_STREAM_BITRATE_8           17

// AAC format
#define C_AAC_ADIF_FORMAT                0
#define C_AAC_ADTS_FORMAT                1

// channel mode
#define C_AAC_STREAM_MONO                1
#define C_AAC_STREAM_STEREO              2

// stream error
#define C_AAC_STREAM_ERROR               255

// DMA error manager
#define C_AAC_DMA_CSR_TOUT_SRC         0x0101
#define C_AAC_DMA_CSR_DROP             0x0002
#define C_AAC_DMA_CSR_TOUT_DST         0x0001

#define C_AAC_DMA_CSR_HALF_BLOCK       0x0080
#define C_AAC_DMA_CSR_BLOCK            0x0020
#define C_AAC_DMA_CSR_ERROR_CHECK_MASK ( C_AAC_DMA_CSR_TOUT_SRC | C_AAC_DMA_CSR_DROP | C_AAC_DMA_CSR_TOUT_DST )

#define C_AAC_OUTPUT_BUFFER_SIZE (2048)
#define L1_AAC_WCM          1
#endif
