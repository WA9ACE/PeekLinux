/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1AAC_MSGTY.H
 *
 *        Filename l1aac_msgty.h
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#ifndef _L1AAC_MSGTY_H_
#define _L1AAC_MSGTY_H_

#if (L1_AAC == 1)

/***************************************************************************************/
/* MMI - L1A                                                                           */
/***************************************************************************************/
#ifndef _AACFRAMECOUNTIND_ 
    typedef UWORD32  (*AUDIOFRAMECOUNTIND)( void  *hComponent, void  *pAppData);
#define _AACFRAMECOUNTIND_
#endif
typedef struct
{
  #if (defined _WINDOWS && (OP_RIV_AUDIO == 1))
    T_RV_HDR  header;
  #endif
  UWORD8  session_id;
  BOOL    stereo;
  BOOL    loopback;
  UWORD8  dma_channel_number;
  void             *hMSLAVPlayer;
  AUDIOFRAMECOUNTIND aac_framecount_callback;
} T_MMI_AAC_REQ;

typedef struct
{
  #if (OP_RIV_AUDIO == 1)
    T_RV_HDR  header;
  #endif
  UWORD8 aac_format;
} T_L1A_AAC_START_CON;

typedef struct
{
  #if (OP_RIV_AUDIO == 1)
    T_RV_HDR  header;
  #endif
  UWORD32 error_code;
} T_L1A_AAC_CON;

typedef struct
{
  #if (OP_RIV_AUDIO == 1)
    T_RV_HDR  header;
  #endif
  UWORD32 bitrate;
  UWORD16 frequency;
  UWORD8  channel;
  UWORD8  aac_format;
  // UWORD8  private;
  // BOOL    copyright;
  // BOOL    original;
  // UWORD8  emphasis;
} T_L1A_AAC_INFO_CON;

/***************************************************************************************/
/* AAC API HISR - L1A                                                                  */
/***************************************************************************************/
typedef struct
{
  #if (OP_RIV_AUDIO == 1)
    T_RV_HDR  header;
  #endif
  UWORD32 error_code;
} T_L1_API_AAC_STOP_CON;

#endif  // L1_AAC

#endif
