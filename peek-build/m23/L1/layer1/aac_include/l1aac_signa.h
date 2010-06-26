/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1AAC_SIGNA.H
 *
 *        Filename l1aac_signa.h
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#ifndef _L1AAC_SIGNA_H_
#define _L1AAC_SIGNA_H_

#if (L1_AAC == 1)

#define P_AAC 0x42

// Messages MMI <-> L1A
#define MMI_AAC_START_REQ                ( ( P_AAC << 8 ) | 0x01 )
#define MMI_AAC_STOP_REQ                 ( ( P_AAC << 8 ) | 0x02 )
#define MMI_AAC_START_CON                ( ( P_AAC << 8 ) | 0x03 )
#define MMI_AAC_STOP_CON                 ( ( P_AAC << 8 ) | 0x04 )
#define MMI_AAC_PAUSE_REQ                ( ( P_AAC << 8 ) | 0x05 )
#define MMI_AAC_RESUME_REQ               ( ( P_AAC << 8 ) | 0x06 )
#define MMI_AAC_RESTART_REQ              ( ( P_AAC << 8 ) | 0x07 )
#define MMI_AAC_PAUSE_CON                ( ( P_AAC << 8 ) | 0x08 )
#define MMI_AAC_RESUME_CON               ( ( P_AAC << 8 ) | 0x09 )
#define MMI_AAC_RESTART_CON              ( ( P_AAC << 8 ) | 0x0A )
#define MMI_AAC_INFO_REQ                 ( ( P_AAC << 8 ) | 0x0B )
#define MMI_AAC_INFO_CON                 ( ( P_AAC << 8 ) | 0x0C )

// Messages L1S -> L1A
#define L1_AAC_ENABLE_CON                ( ( P_AAC << 8 ) | 0x0D )
#define L1_AAC_DISABLE_CON               ( ( P_AAC << 8 ) | 0x0E )

// Messages API HISR -> L1A
#define API_AAC_START_CON                ( ( P_AAC << 8 ) | 0x0F )
#define API_AAC_STOP_CON                 ( ( P_AAC << 8 ) | 0x10 )
#define API_AAC_PAUSE_CON                ( ( P_AAC << 8 ) | 0x11 )
#define API_AAC_RESUME_CON               ( ( P_AAC << 8 ) | 0x12 )
#define API_AAC_RESTART_CON              ( ( P_AAC << 8 ) | 0x13 )

#endif  // L1_AAC

#endif
