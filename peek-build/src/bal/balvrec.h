

#ifndef BALVREC_H
#define BALVREC_H



/* Voice Rec Memory Map of a 64KB Flash Sector.
 *
 * Since a 64kB sector has been preallocated for voice recognition,
 * we can store up to 16 nametags of about 4kB each.  Each nametag
 * then contains 3 seconds worth of template and compressed speech
 * data.

   |====================|
   | Reserved by DBM    |} 4 Bytes
   |====================|              --
   | Header #1          |} 48 Bytes      |
   |--------------------|                |
   | ...                |                |
   |--------------------|                |
   | Header #N          |                |
   |====================|                |
   | Training Data #1   |} 110*T Bytes   |
   |--------------------|                 \
   | ...                |                  > N*(48+(110*T)+(1200*T))+4 <= 64 KBytes
   |--------------------|                 /  (N=16, T=3)
   | Training Data #N   |                |
   |====================|                |
   | Compressed Data #1 |} 1200*T Bytes  |
   |--------------------|                |
   | ...                |                |
   |--------------------|                |
   | Compressed Data #N |                |
   |====================|              --

*/


#if 1             /* set to '1' to use stub, set to '0' to use real VoiceRec Engine */
#define USE_STUB
#endif

#ifndef USE_STUB
#include "vsvoicetag.h"
#endif


/*------------------------------------------------------------------------
* Define constants used in BAL VRS
*------------------------------------------------------------------------*/

#define  VRS_FLASH_SECTOR_SIZE         0xFFFC   /* 64KBytes - 4 */
#define  PHRASE_WINDOW_LEN_IN_MSEC     3000
#define  TEMPLATE_DATA_BYTES_PER_SEC   110
#define  COMP_SPCH_DATA_BYTES_PER_SEC  1200

#define  TEMPLATE_DATA_SIZE \
            ((TEMPLATE_DATA_BYTES_PER_SEC * PHRASE_WINDOW_LEN_IN_MSEC) / 1000)

#define  COMP_SPCH_DATA_SIZE \
            ((COMP_SPCH_DATA_BYTES_PER_SEC * PHRASE_WINDOW_LEN_IN_MSEC) / 1000)

#define  NUM_NAMETAGS_PER_SECTOR    (VRS_FLASH_SECTOR_SIZE / \
                                     (sizeof(BalVrecHeaderInfoT) + \
                                      sizeof(BalVrecTemplateT) + \
                                      sizeof(BalVrecCompSpchT)))

#define  NUM_VREC_SECTORS           2

#define  NUM_VREC_NAMETAGS          (NUM_NAMETAGS_PER_SECTOR * NUM_VREC_SECTORS)

#define  VRS_SAMPLES_PER_FRAME      160

#define  VRS_MAX_REG_TABLE_NUM      (2)

/*------------------------------------------------------------------------
* Define BAL VRS data structures
*------------------------------------------------------------------------*/

/*
 * Validation key definitions
 */
typedef enum
{
  VRE_KEY_DELETED   = 0xDEAD,
  VRE_KEY_UNDELETED = 0xFFFF
} BalVrecDeleteKeyT;

typedef enum
{
  VRE_KEY_VALID    = 0xABCD,
  VRE_KEY_UNUSED   = 0xFFFF
} BalVrecValidKeyT;

/*
 * Nametag Key Press Sequence
 */
typedef PACKED struct
{
  uint8   Type;             /* mapped function type */
  uint8   KeyPress[31];     /* key press data, zero-terminated string */
} BalVrecDialInfoT;  /* 32 Bytes total */

/*
 * Nametag Header Data
 */
typedef PACKED struct
{
  BalVrecDeleteKeyT  DeleteKey;        /* 0xFFFF = use ValidKey, 0xDEAD = deleted */
  BalVrecValidKeyT   ValidKey;         /* 0xABCD = valid, 0xFFFF = empty          */
  uint16             TemplateSize;     /* template data size in bytes             */
  uint16             CompSpchPackets;  /* # of compressed speech packets          */
  BalVrecDialInfoT   DialInfo;         /* mapped function or dialing info         */
  uint16             Reserved[4];
} BalVrecHeaderInfoT; /* 48 Bytes total */

/*
 * Template data structure
 */
typedef PACKED struct
{
  uint8   Data[TEMPLATE_DATA_SIZE];
} BalVrecTemplateT;

/*
 * Compressed speech data structure
 */
typedef PACKED struct
{
  uint8   Data[COMP_SPCH_DATA_SIZE];
} BalVrecCompSpchT;

/*
 * Flash sector memory map
 */
typedef PACKED struct
{
  BalVrecHeaderInfoT HeaderData[NUM_NAMETAGS_PER_SECTOR];
  BalVrecTemplateT   Template[NUM_NAMETAGS_PER_SECTOR];
  BalVrecCompSpchT   CompSpch[NUM_NAMETAGS_PER_SECTOR];
} BalVrecSectorInfoT;

/*
 * General Voice Rec Data Structure
 */
#ifdef USE_STUB

typedef struct
{
   void*   ptr;    /* pointer to data buffer */
   uint16  size;   /* data buffer size */
} BalVrecDataT;

#else

typedef vs_data_t BalVrecDataT;

#endif




#endif /* BALVREC_H */
