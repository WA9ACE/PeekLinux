/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1MP3_ERROR.H
 *
 *        Filename l1mp3_error.h
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#ifndef _L1MP3_ERROR_H_
#define _L1MP3_ERROR_H_

#if (L1_MP3 == 1)

#define C_MP3_ERR_NONE           (0x0001)
#define C_MP3_SYNC_NOT_FOUND     (0x0002)
#define C_MP3_NOT_LAYER3         (0x0004)
#define C_MP3_FREE_FORMAT        (0x0008)
#define C_MP3_ALG_ERROR          (0x0010)
#define C_MP3_DECODING_DELAY     (0x0020)
#define C_MP3_DMA_DROP           (0x0040)
#define C_MP3_DMA_TOUT_SRC       (0x0080)
#define C_MP3_DMA_TOUT_DST       (0x0100)
#define C_MP3_DMA_IT_MASKED      (0x2000)
#define C_MP3_CHECK_BUFFER_KO    (0x4000)
#define C_MP3_CHECK_BUFFER_DELAY (0x8000)

#endif    // L1_MP3

#endif    // _L1MP3_ERROR_H_
