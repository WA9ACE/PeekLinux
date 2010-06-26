/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1AAC_DEFTY.H
 *
 *        Filename l1aac_defty.h
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#ifndef _L1AAC_DEFTY_H_
#define _L1AAC_DEFTY_H_

#if (L1_AAC == 1)
#include "l1aac_const.h"
/***************************************************************************************/
/* AAC l1a_l1s_com structure...                                                        */
/***************************************************************************************/
typedef struct
{
  BOOL start;
  BOOL stop;
} T_AAC_COMMAND;

typedef struct
{
  BOOL   stereo;
  BOOL   loopback;
  UWORD8 session_id;
  UWORD8 dma_channel_number; //added for AAC
} T_AAC_PARAM;

typedef struct
{
  T_AAC_COMMAND command;
  T_AAC_PARAM   parameters;
} T_AAC_TASK;

/***************************************************************************************/
/* AAC L1A structure to store AAC L1A global variables                                 */
/***************************************************************************************/
typedef struct
{
  UWORD32 error_code;
  BOOL    stereopath_init_pending;
} T_L1_AAC_L1A;

/***************************************************************************************/
/* AAC HISR - L1A structure...                                                         */
/***************************************************************************************/
typedef struct
{
  BOOL start;
  BOOL stop;
  BOOL pause;
  BOOL resume;
  BOOL restart;
} T_AAC_HISR_COMMAND;

#ifndef _AACFRAMECOUNTIND_ 
    typedef UWORD32  (*AUDIOFRAMECOUNTIND)( void  *hComponent, void  *pAppData);
#define _AACFRAMECOUNTIND_
#endif
typedef struct
{
  T_AAC_HISR_COMMAND command;
  BOOL     running;
  // Information fields
  UWORD16  aac_format; // 16-bit due to copy from API
  UWORD16  frequency_index;
  UWORD16  channels;
  UWORD32  bitrate;
  void *hMSLAVPlayer;
  AUDIOFRAMECOUNTIND aac_framecount_callback;
} T_L1A_AAC_HISR_COM;

/***************************************************************************************/
/* AAC HISR structure to store AAC HISR global variables                               */
/***************************************************************************************/
typedef struct
{
  UWORD32 error_code;
  UWORD8  state;
  BOOL    on_pause;
  UWORD16 dma_csr;   //shared variable between API HISR and DMA ISR
  BOOL    wait_data;
} T_L1_AAC_API_HISR;

/***************************************************************************************/
/* AAC MCU-DSP API                                                                     */
/***************************************************************************************/
typedef struct
{
  API d_aac_api_dma_it_occured;
  API d_aac_api_init;
  API d_aac_api_play;
  API d_aac_api_pause;
  API d_aac_api_restart;
  API d_aac_api_stop;
  API d_aac_api_end;
  API d_aac_api_request_index;
  API d_aac_api_request_size;
  API d_aac_api_provided_size;
  API d_aac_api_error_code[2];
  API d_aac_api_aac_format;
  API d_aac_api_frequency_index;
  API d_aac_api_bitrate[2];
  API d_aac_api_channels;
  API a_aac_api_hole[7];
  API a_aac_api_input_buffer[824];
  API a_aac_api_output_buffer[2048];
} T_AAC_MCU_DSP;

typedef struct
{
    UWORD16  a_aac_dma_input_buffer[2][C_AAC_OUTPUT_BUFFER_SIZE];
} T_AAC_DMA_PARAM;
#endif  // L1_AAC

#endif
