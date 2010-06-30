/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1MP3_SIGNA.H
 *
 *        Filename l1mp3_signa.h
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#ifndef _L1MP3_SIGNA_H_
#define _L1MP3_SIGNA_H_

//#if (L1_MP3 == 1)

#define P_MP3 0x40

// Messages MMI <-> L1A
#define MMI_MP3_START_REQ                ( ( P_MP3 << 8 ) | 0x01 )
#define MMI_MP3_STOP_REQ                 ( ( P_MP3 << 8 ) | 0x02 )
#define MMI_MP3_START_CON                ( ( P_MP3 << 8 ) | 0x03 )
#define MMI_MP3_STOP_CON                 ( ( P_MP3 << 8 ) | 0x04 )
#define MMI_MP3_PAUSE_REQ                ( ( P_MP3 << 8 ) | 0x05 )
#define MMI_MP3_RESUME_REQ               ( ( P_MP3 << 8 ) | 0x06 )
#define MMI_MP3_RESTART_REQ              ( ( P_MP3 << 8 ) | 0x07 )
#define MMI_MP3_PAUSE_CON                ( ( P_MP3 << 8 ) | 0x08 )
#define MMI_MP3_RESUME_CON               ( ( P_MP3 << 8 ) | 0x09 )
#define MMI_MP3_RESTART_CON              ( ( P_MP3 << 8 ) | 0x0A )
#define MMI_MP3_INFO_REQ                 ( ( P_MP3 << 8 ) | 0x0B )
#define MMI_MP3_INFO_CON                 ( ( P_MP3 << 8 ) | 0x0C )

// Messages L1S -> L1A
#define L1_MP3_ENABLE_CON                ( ( P_MP3 << 8 ) | 0x0D )
#define L1_MP3_DISABLE_CON               ( ( P_MP3 << 8 ) | 0x0E )

// Messages API HISR -> L1A
#define API_MP3_START_CON                ( ( P_MP3 << 8 ) | 0x0F )
#define API_MP3_STOP_CON                 ( ( P_MP3 << 8 ) | 0x10 )
#define API_MP3_PAUSE_CON                ( ( P_MP3 << 8 ) | 0x11 )
#define API_MP3_RESUME_CON               ( ( P_MP3 << 8 ) | 0x12 )
#define API_MP3_RESTART_CON              ( ( P_MP3 << 8 ) | 0x13 )

//#endif  // L1_MP3

#endif
