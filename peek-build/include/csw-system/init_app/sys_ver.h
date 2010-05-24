/*******************************************************************************
 *
 * SYS_VER.H
 *
 * This module defines the S/W versions for:
 *     - TI Layer1,
 *     - Condat G.2-3,
 *     - TI Ref. Design Release (GSM Platform Software Team).
 * 
 * (C) Texas Instruments 2000
 *
 ******************************************************************************/

/*
 * TI Layer1 S/W Version
 */
#include "l1_ver.h"
#include "l1p_ver.h"
#include "l1tm_ver.h"


/*
 * Condat G.2-3 GSM/GPRS New Frame S/W Version
 */

//#define G23VERSION    0x122
//#define G23VERSION    0x130
//#define G23VERSION    0x131
//#define G23VERSION    0x132
//#define G23VERSION    0x1322
//#define G23VERSION    0x1323
//#define G23VERSION    0x133  // 133e1p4
//#define G23VERSION    0x1331 // Snapshot 15
//#define G23VERSION    0x1334 // Snapshot 48
//#define G23VERSION    0x140  // 140p2
//#define G23VERSION    0x140  // Snapshot 175
//#define G23VERSION    0x140  // Snapshot 192
//#define G23VERSION    0x140  // Snapshot 211
//#define G23VERSION    0x140  // Snapshot 222
//#define G23VERSION    0x140  // Snapshot 237
//#define G23VERSION    0x140  // Snapshot 296
#define G23VERSION    0x140    // Snapshot 301


/*
 * TI Ref. Design Release
 */

//#define SYSTEMVERSION 0x100 // Works with Condat 3.2.0 & Layer1 1326 Port.14
                              // Released on June 9th 00
                              // First delivery on B-Sample board
//#define SYSTEMVERSION 0x110 // Works with Condat 3.2.0 & Layer1 1326 Port.14
                              // Released on July 12th 00
                              // Added AEC, ADC, AT-Cmd, Serialswitch
//#define SYSTEMVERSION 0x200 // Works with Condat 3.2.1 & Layer1 1332 Port.14
                              // Released on August 16th 00
                              // Added TM3, Voice Memo - Pre-FTA version
//#define SYSTEMVERSION 0x210 // Works with Condat 3.2.2 & Layer1 1332 Port.14
                              // Released on September 13th 00
                              // Added F&D, Big Sleep
//#define SYSTEMVERSION 0x300 // Works with Condat 3.2.2 & Layer1 1332 Port.14
                              // Released on November 27th 00
                              // Added FFS, VM in Flash, Riviera Environment,
                              // New SPI and LCD drivers, Deep Sleep
//#define SYSTEMVERSION 0x301 // Works with Condat 3.3.0 & Layer1 1332 Port.14
                              // Released on January 23th 01
                              // Added Multi VM in Flash, UARTs wake-up after
                              // deep sleep, Stack Monitoring
//#define SYSTEMVERSION 0x302 // Works with Condat 3.3.0 & Layer1 1332 Port.14
                              // Released on April 9th 01
                              // New Trace Integration, Testmode PCTM rework
//#define SYSTEMVERSION 0x320 // Works with Condat 3.3.2 & Layer1 1352
                              // Released on June 29th 01
                              // New Layer1 Integration, Testmode PCTM rework,
                              // Audio + RTC SW entities
                              // FFS single bank
//#define SYSTEMVERSION 0x520 // Works with Condat GPRS 1.2.2 & Layer1 1354
                              // First external GPRS release
                              // Released on July 31th 01
//#define SYSTEMVERSION 0x530 // Works with Condat GSM 3.3.3 & Layer1 1354
                              // Released on Sept 11th 01
                              // Works with Condat GPRS 1.3.0 & Layer1 1354
//#define SYSTEMVERSION 0x531 // Works with Condat GPRS 1.3.1 & Layer1 1366_112_404
                              // Released on Nov 16th 01
                              // New Layer1 Integration, Appli Init rework,
                              // Riviera v1.6, Serialswitch rework, DCD/DTR
                              // support in UARTFAX
                              // DAR + KPD + PWR + R2D SW entities
//#define SYSTEMVERSION 0x532 // Works with Condat GPRS 1.3.2 & Layer1 1372_115_404
                              // Released on Feb 06th 02
                              // New Layer1 Integration, Riviera v1.61, Audio Name
                              // Dialing & Configuration, RGUI SW entities
//#define SYSTEMVERSION 0x533 // Works with Condat GPRS 1.3.2.2 & Layer1 1379_120_404
                              // Released on March 29th 02 
                              // Melody E2 first step and D-Sample IOTA support
//#define SYSTEMVERSION 0x540 // Works with Condat GPRS 1.3.3e1p4 & Layer1 1386_124_405
                              // Released on July 19th 02 

//#define SYSTEMVERSION 0x542
#if (BOARD == 46)
  #define SYSTEMVERSION "TCS4.0_"
#else
#define SYSTEMVERSION 0x542
#endif
