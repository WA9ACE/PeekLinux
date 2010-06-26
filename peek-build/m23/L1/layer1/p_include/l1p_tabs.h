/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * L1P_TABS.H
 *
 *        Filename l1p_tabs.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

/*********************************************************************
 * Content:
 *   This file contains  miscelaneous ROM tables for packet procedures
 *********************************************************************/

#ifdef L1_SYNC_C 
  /*-----------------------------------------------------------------*/
  /* Frame Number Paging Position.                                   */
  /*-----------------------------------------------------------------*/
   
  const UWORD8 PACKET_PG_POSITION[MAX_NBR_PG_BLKS * (MAX_PG_BLKS_INDEX+1)] =
  { 
    // BS_PAG_BLKS_RES + BS_PBCCH_BLKS_RES = 1.
    // ---------------------------------------
    // Paging
    PCCCH_1,  // Packet Paging Block Index = 0.
    PCCCH_2,  // Packet Paging Block Index = 1.
    PCCCH_3,  // Packet Paging Block Index = 2.
    PCCCH_4,  // Packet Paging Block Index = 3.
    PCCCH_5,  // Packet Paging Block Index = 4.
    PCCCH_6,  // Packet Paging Block Index = 5.
    PCCCH_7,  // Packet Paging Block Index = 6.
    PCCCH_8,  // Packet Paging Block Index = 7.
    PCCCH_9,  // Packet Paging Block Index = 8.
    PCCCH_10, // Packet Paging Block Index = 9.
    PCCCH_11, // Packet Paging Block Index = 10.

    // BS_PAG_BLKS_RES + BS_PBCCH_BLKS_RES = 2.
    // ---------------------------------------
    // Paging      
    PCCCH_1,  // Packet Paging Block Index = 0.
    PCCCH_2,  // Packet Paging Block Index = 1.
    PCCCH_3,  // Packet Paging Block Index = 2.
    PCCCH_4,  // Packet Paging Block Index = 3.
    PCCCH_5,  // Packet Paging Block Index = 4.
    PCCCH_7,  // Packet Paging Block Index = 5.
    PCCCH_8,  // Packet Paging Block Index = 6.
    PCCCH_9,  // Packet Paging Block Index = 7.
    PCCCH_10, // Packet Paging Block Index = 8.
    PCCCH_11, // Packet Paging Block Index = 9.
    NULL,     // Packet Paging Block Index = 10.

    // BS_PAG_BLKS_RES + BS_PBCCH_BLKS_RES = 3.
    // ---------------------------------------
    // Paging
    PCCCH_1,  // Packet Paging Block Index = 0.
    PCCCH_2,  // Packet Paging Block Index = 1.
    PCCCH_4,  // Packet Paging Block Index = 2.
    PCCCH_5,  // Packet Paging Block Index = 3.
    PCCCH_7,  // Packet Paging Block Index = 4.
    PCCCH_8,  // Packet Paging Block Index = 5.
    PCCCH_9,  // Packet Paging Block Index = 6.
    PCCCH_10, // Packet Paging Block Index = 7.
    PCCCH_11, // Packet Paging Block Index = 8.
    NULL,     // Packet Paging Block Index = 9.
    NULL,     // Packet Paging Block Index = 10.

    // BS_PAG_BLKS_RES + BS_PBCCH_BLKS_RES = 4.
    // ---------------------------------------
    // Paging 
    PCCCH_1,  // Packet Paging Block Index = 0.
    PCCCH_2,  // Packet Paging Block Index = 1.
    PCCCH_4,  // Packet Paging Block Index = 2.
    PCCCH_5,  // Packet Paging Block Index = 3.
    PCCCH_7,  // Packet Paging Block Index = 4.
    PCCCH_8,  // Packet Paging Block Index = 5.
    PCCCH_10, // Packet Paging Block Index = 6.
    PCCCH_11, // Packet Paging Block Index = 7.
    NULL,     // Packet Paging Block Index = 8.
    NULL,     // Packet Paging Block Index = 9.
    NULL,     // Packet Paging Block Index = 10.

    // BS_PAG_BLKS_RES + BS_PBCCH_BLKS_RES = 5.
    // ---------------------------------------
    // Paging
    PCCCH_2,   // Packet Paging Block Index = 0.
    PCCCH_4,   // Packet Paging Block Index = 1.
    PCCCH_5,   // Packet Paging Block Index = 2.
    PCCCH_7,   // Packet Paging Block Index = 3.
    PCCCH_8,   // Packet Paging Block Index = 4.
    PCCCH_10, // Packet Paging Block Index = 5.
    PCCCH_11, // Packet Paging Block Index = 6.
    NULL,     // Packet Paging Block Index = 7.
    NULL,     // Packet Paging Block Index = 8.
    NULL,     // Packet Paging Block Index = 9.
    NULL,     // Packet Paging Block Index = 10.

    // BS_PAG_BLKS_RES + BS_PBCCH_BLKS_RES = 6.
    // ---------------------------------------
    // Paging
    PCCCH_2,  // Packet Paging Block Index = 0.
    PCCCH_4,  // Packet Paging Block Index = 1.
    PCCCH_5,  // Packet Paging Block Index = 2.
    PCCCH_8,  // Packet Paging Block Index = 3.
    PCCCH_10, // Packet Paging Block Index = 4.
    PCCCH_11, // Packet Paging Block Index = 5.
    NULL,     // Packet Paging Block Index = 6.
    NULL,     // Packet Paging Block Index = 7.
    NULL,     // Packet Paging Block Index = 8.
    NULL,     // Packet Paging Block Index = 9.
    NULL,     // Packet Paging Block Index = 10.

    // BS_PAG_BLKS_RES + BS_PBCCH_BLKS_RES = 7.
    // ---------------------------------------
    // Paging
    PCCCH_2,  // Packet Paging Block Index = 0.
    PCCCH_5,  // Packet Paging Block Index = 1.
    PCCCH_8,  // Packet Paging Block Index = 2.
    PCCCH_10, // Packet Paging Block Index = 3.
    PCCCH_11, // Packet Paging Block Index = 4.
    NULL,     // Packet Paging Block Index = 5.
    NULL,     // Packet Paging Block Index = 6.
    NULL,     // Packet Paging Block Index = 7.
    NULL,     // Packet Paging Block Index = 8.
    NULL,     // Packet Paging Block Index = 9.
    NULL,     // Packet Paging Block Index = 10.

    // BS_PAG_BLKS_RES + BS_PBCCH_BLKS_RES = 8.
    // ---------------------------------------
    // Paging
    PCCCH_2,  // Packet Paging Block Index = 0.
    PCCCH_5,  // Packet Paging Block Index = 1.
    PCCCH_8,  // Packet Paging Block Index = 2.
    PCCCH_11, // Packet Paging Block Index = 3.
    NULL,     // Packet Paging Block Index = 4.
    NULL,     // Packet Paging Block Index = 5.
    NULL,     // Packet Paging Block Index = 6.
    NULL,     // Packet Paging Block Index = 7.
    NULL,     // Packet Paging Block Index = 8.
    NULL,     // Packet Paging Block Index = 9.
    NULL,     // Packet Paging Block Index = 10.

    // BS_PAG_BLKS_RES + BS_PBCCH_BLKS_RES = 9.
    // ---------------------------------------
    // Paging
    PCCCH_5,  // Packet Paging Block Index = 0.
    PCCCH_8,  // Packet Paging Block Index = 1.
    PCCCH_11, // Packet Paging Block Index = 2.
    NULL,     // Packet Paging Block Index = 3.
    NULL,     // Packet Paging Block Index = 4.
    NULL,     // Packet Paging Block Index = 5.
    NULL,     // Packet Paging Block Index = 6.
    NULL,     // Packet Paging Block Index = 7.
    NULL,     // Packet Paging Block Index = 8.
    NULL,     // Packet Paging Block Index = 9.
    NULL,     // Packet Paging Block Index = 10.

    // BS_PAG_BLKS_RES + BS_PBCCH_BLKS_RES = 10.
    // ---------------------------------------
    // Paging
    PCCCH_5,  // Packet Paging Block Index = 0.
    PCCCH_11, // Packet Paging Block Index = 1.
    NULL,     // Packet Paging Block Index = 2.
    NULL,     // Packet Paging Block Index = 3.
    NULL,     // Packet Paging Block Index = 4.
    NULL,     // Packet Paging Block Index = 5.
    NULL,     // Packet Paging Block Index = 6.
    NULL,     // Packet Paging Block Index = 7.
    NULL,     // Packet Paging Block Index = 8.
    NULL,     // Packet Paging Block Index = 9.
    NULL,     // Packet Paging Block Index = 10.

    // BS_PAG_BLKS_RES + BS_PBCCH_BLKS_RES = 11.
    // ---------------------------------------
    // Paging
    PCCCH_11, // Packet Paging Block Index = 0.
    NULL,     // Packet Paging Block Index = 1.
    NULL,     // Packet Paging Block Index = 2.
    NULL,     // Packet Paging Block Index = 3.
    NULL,     // Packet Paging Block Index = 4.
    NULL,     // Packet Paging Block Index = 5.
    NULL,     // Packet Paging Block Index = 6.
    NULL,     // Packet Paging Block Index = 7.
    NULL,     // Packet Paging Block Index = 8.
    NULL,     // Packet Paging Block Index = 9.
    NULL,     // Packet Paging Block Index = 10.
  };

  // Starting times of each block for ordered list of blocks
  const UWORD8 ORDERED_BLOCK_START_TIME[12] =
  {
     0,  // B0
    26,  // B6
    13,  // B3
    39,  // B9
     4,  // B1
    30,  // B7
    17,  // B4
    43,  // B10
     8,  // B2
    34,  // B8
    21,  // B5
    47  // B11
  };

    
  // Serving Cell PBCCH block index
  const UWORD8 PBCCH_BLK_INDEX[MAX_NBR_PB_BLKS * (MAX_PB_BLKS_INDEX+1)] =
  { 
    // BS_BPCCH_BLKS = 0
    0,
    0,
    0,
    0,

    // BS_BPCCH_BLKS = 1
    0,
    6,
    0,
    0,
    
    // BS_BPCCH_BLKS = 2
    0,
    3,
    6,
    0,

    // BS_BPCCH_BLKS = 3
    0,
    3,
    6,
    9
  };

  // Serving PBCCH and Neighbor PBCCH block position in the MF52
  const UWORD8 PBCCH_POSITION[4][4] = 
  {
    {51,00,00,00},  // BS_PBCCH_BLKS=0
    {51,25,00,00},  // BS_PBCCH_BLKS=1
    {51,12,25,00},  // BS_PBCCH_BLKS=2
    {51,12,25,38}   // BS_PBCCH_BLKS=3
  };

#else
  extern UWORD8  PACKET_PG_POSITION[MAX_NBR_PG_BLKS * (MAX_PG_BLKS_INDEX+1)];
  extern UWORD8  ORDERED_BLOCK_START_TIME[12];
  extern UWORD8  PBCCH_BLK_INDEX[MAX_NBR_PB_BLKS * (MAX_PB_BLKS_INDEX+1)]; 
  extern UWORD8  PBCCH_POSITION[4][4]; 
#endif
