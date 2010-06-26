/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1MP3_CONST.H
 *
 *        Filename l1mp3_const.h
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#ifndef _L1MP3_CONST_H_
#define _L1MP3_CONST_H_

// DSP MP3 background task id
#define C_BGD_MP3   8

// MP3 API base address
#define C_MP3_API_BASE_ADDRESS 0x1AD0


// MP3 header information
// ----------------------

// frequency rate
#define C_MP3_HEADER_FREQ_48000          0
#define C_MP3_HEADER_FREQ_44100          1
#define C_MP3_HEADER_FREQ_32000          2
#define C_MP3_HEADER_FREQ_24000          3
#define C_MP3_HEADER_FREQ_22050          4
#define C_MP3_HEADER_FREQ_16000          5
#define C_MP3_HEADER_FREQ_12000          6
#define C_MP3_HEADER_FREQ_11025          7
#define C_MP3_HEADER_FREQ_8000           8

// bitrate
#define C_MP3_HEADER_BITRATE_320         0
#define C_MP3_HEADER_BITRATE_256         1
#define C_MP3_HEADER_BITRATE_224         2
#define C_MP3_HEADER_BITRATE_192         3
#define C_MP3_HEADER_BITRATE_160         4
#define C_MP3_HEADER_BITRATE_128         5
#define C_MP3_HEADER_BITRATE_112         6
#define C_MP3_HEADER_BITRATE_96          7
#define C_MP3_HEADER_BITRATE_80          8
#define C_MP3_HEADER_BITRATE_64          9
#define C_MP3_HEADER_BITRATE_56         10
#define C_MP3_HEADER_BITRATE_48         11
#define C_MP3_HEADER_BITRATE_40         12
#define C_MP3_HEADER_BITRATE_32         13

// MPEG version
#define C_MP3_HEADER_MPEGID_1            1
#define C_MP3_HEADER_MPEGID_2            2
#define C_MP3_HEADER_MPEGID_2_5          3

// MPEG layer
#define C_MP3_HEADER_LAYER_I             1
#define C_MP3_HEADER_LAYER_II            2
#define C_MP3_HEADER_LAYER_III           3

// channel mode
#define C_MP3_HEADER_STEREO              0
#define C_MP3_HEADER_JSTEREO             1
#define C_MP3_HEADER_DUAL_MONO           2
#define C_MP3_HEADER_MONO                3

// emphasis
#define C_MP3_HEADER_EMPHASIS_NONE       0
#define C_MP3_HEADER_EMPHASIS_50_15      1
#define C_MP3_HEADER_EMPHASIS_CCIT_J17   2

#define C_MP3_HEADER_ERROR             255

#define C_MP3_OUTPUT_BUFFER_SIZE (1152)
#define MP3_BUFFER_COUNT   10        // buffer count has to be even number
#if (L1_MP3_SIX_BUFFER == 1)
// DMA error manager
#define C_MP3_DMA_CSR_TOUT_SRC         0x0101
#define C_MP3_DMA_CSR_DROP             0x0002
#define C_MP3_DMA_CSR_TOUT_DST         0x0001

#define C_MP3_DMA_CSR_HALF_BLOCK       0x0080
#define C_MP3_DMA_CSR_BLOCK            0x0020
#define C_MP3_DMA_CSR_ERROR_CHECK_MASK ( C_MP3_DMA_CSR_TOUT_SRC | C_MP3_DMA_CSR_DROP | C_MP3_DMA_CSR_TOUT_DST )

//DMA input buffer
#define C_MP3_MPEG1  3
#endif
#endif
