/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1MP3_DEFTY.H
 *
 *        Filename l1mp3_defty.h
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#ifndef _L1MP3_DEFTY_H_
#define _L1MP3_DEFTY_H_

#if (L1_MP3 == 1)
#include "l1mp3_const.h"
/***************************************************************************************/
/* MP3 l1a_l1s_com structure...                                                        */
/***************************************************************************************/
typedef struct
{
  BOOL start;
  BOOL stop;
} T_MP3_COMMAND;

typedef struct
{
  BOOL   stereo;
  BOOL   loopback;
  UWORD8 session_id;
#if (L1_MP3_SIX_BUFFER == 1)  
  UWORD8  dma_channel_number;
#endif
} T_MP3_PARAM;

typedef struct
{
  T_MP3_COMMAND command;
  T_MP3_PARAM   parameters;
} T_MP3_TASK;

/***************************************************************************************/
/* MP3 L1A structure to store MP3 L1A global variables                                 */
/***************************************************************************************/
typedef struct
{
  UWORD32 error_code;
  BOOL    stereopath_init_pending;
} T_L1_MP3_L1A;

/***************************************************************************************/
/* MP3 HISR - L1A structure...                                                         */
/***************************************************************************************/
typedef struct
{
  BOOL start;
  BOOL stop;
  BOOL pause;
  BOOL resume;
  BOOL restart;
} T_MP3_HISR_COMMAND;

typedef struct
{
  T_MP3_HISR_COMMAND command;
  BOOL    running;
  UWORD8  dma_channel_number;
  UWORD32 header;
} T_L1A_MP3_HISR_COM;

/***************************************************************************************/
/* MP3 HISR structure to store MP3 HISR global variables                               */
/***************************************************************************************/
typedef struct
{
  UWORD32 error_code;
  UWORD8  state;
  BOOL    on_pause;
  BOOL    wait_data;
#if (L1_MP3_SIX_BUFFER == 1)  
  UWORD16 dma_it_occurred;
  UWORD16 dsp_hint_occurred;
  UWORD16 mp3_dma_csr;
#endif  
} T_L1_MP3_API_HISR;

/***************************************************************************************/
/* MP3 MCU-DSP API                                                                     */
/***************************************************************************************/
typedef struct
{
  API d_mp3_api_header[2];
  API d_mp3_api_channel;
  API d_mp3_api_init;
  API d_mp3_api_play;
  API d_mp3_api_pause;
  API d_mp3_api_restart;
  API d_mp3_api_stop;
  API d_mp3_api_end;
  API d_mp3_api_request_index;
  API d_mp3_api_request_size;
  API d_mp3_api_provided_size;
  API d_mp3_api_error_code;
  API a_mp3_hole[3];
  API a_mp3_api_input_buffer[800];
  API a_mp3_api_output_buffer0[1152];
  API a_mp3_api_output_buffer1[1152];
#if (L1_MP3_SIX_BUFFER == 1)  
  API a_mp3_hole_1[585];
  API d_mp3_frame_decoded;  //0x2949  
  API a_mp3_hole_5[16];   
  API d_mp3_ver_id;                    //0x295A
  API a_mp3_hole_2;            
  API d_mp3_out_currentBufferID;       //0x295C 
  API a_mp3_hole_3[6146];
  API d_mp3_out_dma_it_occured;        //0x415F
#endif
} T_MP3_MCU_DSP;

typedef struct
{
   UWORD16  a_mp3_dma_input_buffer[MP3_BUFFER_COUNT][C_MP3_OUTPUT_BUFFER_SIZE]; 
  
} T_MP3_DMA_PARAM;
#endif  // L1_MP3

#endif
