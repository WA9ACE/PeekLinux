/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * L1MP3_MSGTY.H
 *
 *        Filename l1mp3_msgty.h
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#ifndef _L1MP3_MSGTY_H_
#define _L1MP3_MSGTY_H_

#if (L1_MP3 == 1)

/***************************************************************************************/
/* MMI - L1A                                                                           */
/***************************************************************************************/
typedef struct
{
  #if (defined _WINDOWS && (OP_RIV_AUDIO == 1))
    T_RV_HDR  header;
  #endif
  UWORD8  session_id;
  BOOL    stereo;
  BOOL    loopback;
  UWORD8  dma_channel_number;
} T_MMI_MP3_REQ;

typedef struct
{
  #if (OP_RIV_AUDIO == 1)
    T_RV_HDR  header;
  #endif
  UWORD32 error_code;
} T_L1A_MP3_CON;

typedef struct
{
  #if (OP_RIV_AUDIO == 1)
    T_RV_HDR  header;
  #endif
  UWORD16 frequency;
  UWORD16 bitrate;
  UWORD8  mpeg_id;
  UWORD8  layer;
  BOOL    padding;
  UWORD8  private;
  UWORD8  channel;
  BOOL    copyright;
  BOOL    original;
  UWORD8  emphasis;
} T_L1A_MP3_INFO_CON;

/***************************************************************************************/
/* MP3 API HISR - L1A                                                                  */
/***************************************************************************************/
typedef struct
{
  #if (OP_RIV_AUDIO == 1)
    T_RV_HDR  header;
  #endif
  UWORD32 error_code;
} T_L1_API_MP3_STOP_CON;

#endif  // L1_MP3

#endif
