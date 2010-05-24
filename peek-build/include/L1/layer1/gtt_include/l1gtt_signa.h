/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * L1GTT_SIGNA.H
 *
 *        Filename l1gtt_signa.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/
 
#if (L1_GTT == 1)

  #define P_GTT ( 0x19 )
  
  // Messages Upper layer <-> L1A
  #define MMI_GTT_START_REQ             ( ( P_GTT << 8 ) |   1 ) // build: trigger
  #define MMI_GTT_START_CON             ( ( P_GTT << 8 ) |   2 )
  #define MMI_GTT_STOP_REQ              ( ( P_GTT << 8 ) |   3 ) // build: trigger
  #define MMI_GTT_STOP_CON              ( ( P_GTT << 8 ) |   4 )

  // Messages L1S <-> L1A
  #define L1_GTT_START_CON              ( ( P_GTT << 8 ) |   5 )
  #define L1_GTT_STOP_CON               ( ( P_GTT << 8 ) |   6 )

  // Test messages
  #define TST_CLOSE_GTT_LOOP_REQ        ( ( P_GTT << 8 ) |   7 ) // build: T_TST_CLOSE_GTT_LOOP_REQ
  #define TST_CLOSE_GTT_LOOP_CON        ( ( P_GTT << 8 ) |   8 )
  #define TST_OPEN_GTT_LOOP_REQ         ( ( P_GTT << 8 ) |   9 ) // build: trigger
  #define TST_OPEN_GTT_LOOP_CON         ( ( P_GTT << 8 ) |  10 )
  #define TST_START_GTT_TEST_REQ        ( ( P_GTT << 8 ) |  11 ) // build: T_TST_START_GTT_TEST_REQ
  #define TST_START_GTT_TEST_CON        ( ( P_GTT << 8 ) |  12 )
  #define TST_STOP_GTT_TEST_REQ         ( ( P_GTT << 8 ) |  13 ) // build: trigger
  #define TST_STOP_GTT_TEST_CON         ( ( P_GTT << 8 ) |  14 )

#endif // L1_GTT == 1
