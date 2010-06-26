/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1AAC_PROTO.H
 *
 *        Filename l1aac_proto.h
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#if (L1_AAC == 1)

#ifndef _L1AAC_PROTO_H_
#define _L1AAC_PROTO_H_

// Function declared in l1aac_afunc.c
void l1a_aac_send_confirmation(UWORD32 SignalCode, UWORD16 aac_format, UWORD32 error_code);
void l1a_aac_send_stream_info();

// Functions declared in l1aac_apihisr.c
void   l1aac_apihisr();
BOOL   l1aac_apihisr_error_handler(UWORD32 *error_code);
UWORD8 l1aac_apihisr_fill_dsp_buffer(UWORD8 **aac_buffer_ptr, UWORD8 **aac_buffer_end, UWORD8 *Cust_get_pointer_status, BOOL init);
void   l1aac_apihisr_memcpy(UWORD16 *dst, const UWORD16 *src, UWORD16 size);
void   l1aac_apihisr_api_buffer_copy(UWORD16 *dst, const API *src, UWORD16 size);
void   l1aac_apihisr_reset_buffer(UWORD16 *dst, UWORD16 size);
BOOL   l1aac_apihisr_dma_error_handler();
void   l1aac_apihisr_update_dma_buffer_index(UWORD16 dma_csr);

// Functions declared in l1aac_func.c
UWORD16 l1aac_get_frequencyrate(UWORD16 frequency_index);

// Functions declared in l1aac_init.c
void l1aac_initialize_var();
void l1aac_reset();
void l1aac_reset_api();

// Functions declared in l1aac_dma_isr.c
void l1aac_dma_it_handler(SYS_UWORD16 d_dma_channel_it_status);

#endif    // _L1AAC_PROTO_H_

#endif    // L1_AAC
