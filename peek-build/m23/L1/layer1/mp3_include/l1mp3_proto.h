/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1MP3_PROTO.H
 *
 *        Filename l1mp3_proto.h
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#include "sys_types.h"
#if (L1_MP3 == 1)

#ifndef _L1MP3_PROTO_H_
#define _L1MP3_PROTO_H_

// Function declared in l1mp3_afunc.c
void l1a_mp3_send_confirmation(UWORD32 SignalCode, UWORD32 error_code);
void l1a_mp3_send_header_info();

// Functions declared in l1mp3_apihisr.c
void   l1mp3_apihisr();
void   l1mp3_apihisr_fillbuffer(BOOL restart);
BOOL   l1mp3_apihisr_error_handler(UWORD16 *error_code);
UWORD8 l1mp3_apihisr_fill_dsp_buffer(UWORD16 **mp3_buffer_ptr, UWORD16 **mp3_buffer_end, UWORD8 *Cust_get_pointer_status, BOOL init);
void   l1mp3_apihisr_memcpy(UWORD16 *dst, const UWORD16 *src, UWORD16 size);
#if (L1_MP3_SIX_BUFFER == 1)
void   l1mp3_apihisr_api_buffer_copy(UWORD16 *dst, const API *src, UWORD16 size);
void   l1mp3_apihisr_reset_buffer(UWORD16 *dst, UWORD16 size);
void   l1mp3_apihisr_reset_outbuffer(API *dst, UWORD16 size);
BOOL   l1mp3_apihisr_dma_error_handler();
void   l1mp3_apihisr_update_api_buffer_index();
void   l1mp3_apihisr_update_dma_buffer_index(UWORD16 dma_csr);
#endif

// Functions declared in l1mp3_func.c
UWORD16 l1mp3_get_frequencyrate(UWORD32 header);
UWORD16 l1mp3_get_bitrate(UWORD32 header);
UWORD8  l1mp3_get_mpeg_id(UWORD32 header);
UWORD8  l1mp3_get_layer(UWORD32 header);
BOOL    l1mp3_get_padding(UWORD32 header);
UWORD8  l1mp3_get_private(UWORD32 header);
UWORD8  l1mp3_get_channel_mode(UWORD32 header);
BOOL    l1mp3_get_copyright(UWORD32 header);
BOOL    l1mp3_get_original(UWORD32 header);
UWORD8  l1mp3_get_emphasis(UWORD32 header);

// Functions declared in l1mp3_init.c
void l1mp3_initialize_var();
void l1mp3_reset();
void l1mp3_reset_api();
void l1mp3_init_trace();

#if (L1_MP3_SIX_BUFFER == 1)
//function declared in l1mp3_dma_isr.c
void l1mp3_dma_it_handler(SYS_UWORD16 d_dma_channel_it_status);
#endif
#endif    // _L1MP3_PROTO_H_

#endif    // L1_MP3
