/*****************************************************************************

 *****************************************************************************/   

#ifndef DBMAPI_H
 #define DBMAPI_H


#include "sysapi.h"

/*------------------------------------------------------------------------
* Define constants used in DBM API
*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------
* EXE Interfaces - Definition of Signals and Mailboxes
*------------------------------------------------------------------------*/

/* DBM command mailbox id */
#define DBM_MAILBOX         EXE_MAILBOX_1_ID
#define DBM_STARTUP_SIGNAL  EXE_SIGNAL_1

/* DBM flash marker size in bytes. Marker used to distinguish active
   flash data bases */
#define DBM_MARKER_SIZE     2

/**************************************************/
/* Define Data Base 1 (CP) Constants              */
/**************************************************/
#define DBM_CP_FLASH_OFFSET 0  /* The CP data base starts at this offset 
                                  from the flash sector address */

/* Define data base Constants. These constants must be updated when adding
** new segments to this data base - NOTE: In order optimize cache memory allocation
** for customer builds that may not support CP features, these constants are
** preprocessor compile option dependant. */
#ifdef SYS_OPTION_MIP_DMU
#define DBM_DMUPUBKEY_OFFSET 4096
#endif

/* Define CP segment sizes in bytes */
typedef enum
{
   DBM_PSW_NAM1_SIZE          = 548,
   DBM_PSW_MRU1_SIZE          = 30,
   DBM_PSW_MS_CAP_DB_SIZE     = 120,
   DBM_UI_MISC_SIZE           = 20,
   DBM_PSW_MISC_SIZE          = 20,  /* Additional bytes allocated for any future requirements */
   DBM_PSW_NAM2_SIZE          = 548, /* Same size as DBM_PSW_NAM1_SIZE */
   DBM_PSW_MRU2_SIZE          = 30,  /* Same size as DBM_PSW_MRU1_SIZE */
   DBM_SERVICE_HISTORY_SIZE   = 41,
   DBM_DBM_TIMESTAMP_SIZE     = 20
} DbmCPSegmentSizeT;

#ifdef SYS_OPTION_DUAL_NAM 
/* Define all segment constants utilizing segments
   ** designated for use with Dual NAM implementation */

/* Define CP data base segment names */
typedef enum 
{
   DBM_PSW_NAM1_SEG,
   DBM_PSW_MRU1_SEG,
   DBM_PSW_MS_CAP_DB_SEG,
   DBM_UI_MISC_SEG,
   DBM_PSW_MISC_SEG,
   DBM_PSW_NAM2_SEG,
   DBM_PSW_MRU2_SEG,
   DBM_SERVICE_HISTORY_SEG,
   DBM_DBM_TIMESTAMP_SEG,
   DBM_MAX_SEG_CP_DB
} DbmCPSegmentT;

/* Define CP data base segment sizes */
#define DBM_CP_SEGMENT_SIZES \
   DBM_PSW_NAM1_SIZE, \
   DBM_PSW_MRU1_SIZE, \
   DBM_PSW_MS_CAP_DB_SIZE, \
   DBM_UI_MISC_SIZE, \
   DBM_PSW_MISC_SIZE, \
   DBM_PSW_NAM2_SIZE, \
   DBM_PSW_MRU2_SIZE, \
   DBM_SERVICE_HISTORY_SIZE, \
   DBM_DBM_TIMESTAMP_SIZE,

   /* Define segment indexes into CP data base cache */
typedef enum
{
   DBM_CP_MARKER_INDEX        = 0,
   DBM_PSW_NAM1_INDEX         = DBM_CP_MARKER_INDEX + DBM_MARKER_SIZE,
   DBM_PSW_MRU1_INDEX         = DBM_PSW_NAM1_INDEX + DBM_PSW_NAM1_SIZE,
   DBM_PSW_MS_CAP_DB_INDEX    = DBM_PSW_MRU1_INDEX + DBM_PSW_MRU1_SIZE,
   DBM_UI_MISC_INDEX          = DBM_PSW_MS_CAP_DB_INDEX + DBM_PSW_MS_CAP_DB_SIZE,
   DBM_PSW_MISC_INDEX         = DBM_UI_MISC_INDEX + DBM_UI_MISC_SIZE,
   DBM_PSW_NAM2_INDEX         = DBM_PSW_MISC_INDEX + DBM_PSW_MISC_SIZE,
   DBM_PSW_MRU2_INDEX         = DBM_PSW_NAM2_INDEX + DBM_PSW_NAM2_SIZE,
   DBM_SERVICE_HISTORY_INDEX  = DBM_PSW_MRU2_INDEX + DBM_PSW_MRU2_SIZE,
   DBM_DBM_TIMESTAMP_INDEX    = DBM_SERVICE_HISTORY_INDEX + DBM_SERVICE_HISTORY_SIZE,
   DBM_CP_CACHE_SIZE          = DBM_DBM_TIMESTAMP_INDEX + DBM_DBM_TIMESTAMP_SIZE
} DbmCPSegmentIndexT;

/* Define CP data base segment indexes */
#define DBM_CP_SEGMENT_INDEXES \
   DBM_PSW_NAM1_INDEX, \
   DBM_PSW_MRU1_INDEX, \
   DBM_PSW_MS_CAP_DB_INDEX, \
   DBM_UI_MISC_INDEX, \
   DBM_PSW_MISC_INDEX, \
   DBM_PSW_NAM2_INDEX, \
   DBM_PSW_MRU2_INDEX, \
   DBM_SERVICE_HISTORY_INDEX, \
   DBM_DBM_TIMESTAMP_INDEX,

#else 

/* SYS_OPTION_DUAL_NAM not defined - Define all segment constants NOT including
** segments specific to Dual NAM implementation to conserve cache memory usage
** Calls to DBM will generate a MonFault if invalid segments are used and/or 
** maximum cache size is exceeded */

/* Define CP data base segment names */
typedef enum 
{
   DBM_PSW_NAM1_SEG,
   DBM_PSW_MRU1_SEG,
   DBM_PSW_MS_CAP_DB_SEG,
   DBM_UI_MISC_SEG,
   DBM_PSW_MISC_SEG,
   DBM_SERVICE_HISTORY_SEG,
   DBM_DBM_TIMESTAMP_SEG,
   DBM_MAX_SEG_CP_DB,  /* End of supported segments */
   DBM_PSW_NAM2_SEG,   /* Declared for compile purposes - not supported in DBM */
   DBM_PSW_MRU2_SEG    /* Declared for compile purposes - not supported in DBM */
} DbmCPSegmentT;

/* Define CP data base segment sizes */
#define DBM_CP_SEGMENT_SIZES \
   DBM_MARKER_SIZE,\
   DBM_PSW_NAM1_SIZE, \
   DBM_PSW_MRU1_SIZE, \
   DBM_PSW_MS_CAP_DB_SIZE, \
   DBM_UI_MISC_SIZE, \
   DBM_PSW_MISC_SIZE, \
   DBM_SERVICE_HISTORY_SIZE, \
   DBM_DBM_TIMESTAMP_SIZE,

/* Define segment indexes into CP data base cache */
typedef enum
{
   DBM_CP_MARKER_INDEX        = 0,
   DBM_PSW_NAM1_INDEX         = DBM_CP_MARKER_INDEX + DBM_MARKER_SIZE,
   DBM_PSW_MRU1_INDEX         = DBM_PSW_NAM1_INDEX + DBM_PSW_NAM1_SIZE,
   DBM_PSW_MS_CAP_DB_INDEX    = DBM_PSW_MRU1_INDEX + DBM_PSW_MRU1_SIZE,
   DBM_UI_MISC_INDEX          = DBM_PSW_MS_CAP_DB_INDEX + DBM_PSW_MS_CAP_DB_SIZE,
   DBM_PSW_MISC_INDEX         = DBM_UI_MISC_INDEX + DBM_UI_MISC_SIZE,
   DBM_SERVICE_HISTORY_INDEX  = DBM_PSW_MISC_INDEX + DBM_PSW_MISC_SIZE,
   DBM_DBM_TIMESTAMP_INDEX    = DBM_SERVICE_HISTORY_INDEX + DBM_SERVICE_HISTORY_SIZE,
   DBM_CP_CACHE_SIZE          = DBM_DBM_TIMESTAMP_INDEX + DBM_DBM_TIMESTAMP_SIZE
} DbmCPSegmentIndexT;

/* Define CP data base segment indexes */
#define DBM_CP_SEGMENT_INDEXES \
   DBM_PSW_NAM1_INDEX, \
   DBM_PSW_MRU1_INDEX, \
   DBM_PSW_MS_CAP_DB_INDEX, \
   DBM_UI_MISC_INDEX, \
   DBM_PSW_MISC_INDEX, \
   DBM_SERVICE_HISTORY_INDEX, \
   DBM_DBM_TIMESTAMP_INDEX,

#endif /* end #ifdef SYS_OPTION_DUAL_NAM */


/**************************************************/
/* Define Data Base 2 (RF) Constants              */
/**************************************************/
#define DBM_RF_FLASH_OFFSET 4096  /* The RF data base starts at this offset 
                                     from the flash sector address */

/* Define RF data base segment names */
/* 
 * IMPORTANT NOTE:
 * When RF data base segments are changed please use the following rules:
 * 1. New segments must be added to the bottom of the enum list.
 * 2. Existing segment sizes may be modified provided the segment name is unchanged.
 * 3. Existing segment names may be changed provided there is concurrence with 
 *    the APPS group and the impact to the ETS ICD is minimal.
 * 4. If as a result of a change an existing segment is no longer used then
 *    add a comment next to the segment to indicate so.
*/
typedef enum
{
   /* COMMON */
   DBM_HWD_AFC_PARMS_SEG,
   DBM_HWD_BATTERY_PARMS_SEG,
   DBM_HWD_TEMPERATURE_PARMS_SEG,
   DBM_HWD_AUX_ADC_PARMS_SEG,               /* UNUSED */
   DBM_HWD_PDM_PARMS_SEG,                   /* UNUSED */
   DBM_HWD_BANDGAP_PARMS_SEG,
   DBM_HWD_CALIB_PARMS_SEG,                 /* UNUSED */
   DBM_HWD_RX_BBF_PARMS_SEG,                /* UNUSED */

   /* CDMA PCS */
   DBM_HWD_PCS_TXAGC_SEG,
   DBM_HWD_PCS_TXAGC_FREQ_ADJ_SEG,
   DBM_HWD_PCS_TXAGC_TEMP_ADJ_SEG,
   DBM_HWD_PCS_TX_LIM_FREQ_ADJ_SEG,
   DBM_HWD_PCS_TX_HDET_SEG,
   DBM_HWD_PCS_TX_HDET_FREQ_ADJ_SEG,
   DBM_HWD_PCS_TX_HDET_TEMP_ADJ_SEG,
   DBM_HWD_PCS_TX_HDET_MAX_BATT_SEG,
   DBM_HWD_PCS_RXAGC_SEG,
   DBM_HWD_PCS_RXAGC_FREQ_ADJ_SEG,
   DBM_HWD_PCS_RXAGC_TEMP_ADJ_SEG,

   /* CDMA CELL */
   DBM_HWD_CELL_TXAGC_SEG,
   DBM_HWD_CELL_TXAGC_FREQ_ADJ_SEG,
   DBM_HWD_CELL_TXAGC_TEMP_ADJ_SEG,
   DBM_HWD_CELL_TX_LIM_FREQ_ADJ_SEG,
   DBM_HWD_CELL_TX_HDET_SEG,
   DBM_HWD_CELL_TX_HDET_FREQ_ADJ_SEG,
   DBM_HWD_CELL_TX_HDET_TEMP_ADJ_SEG,
   DBM_HWD_CELL_TX_HDET_MAX_BATT_SEG,
   DBM_HWD_CELL_RXAGC_SEG,
   DBM_HWD_CELL_RXAGC_FREQ_ADJ_SEG,
   DBM_HWD_CELL_RXAGC_TEMP_ADJ_SEG,

   /* AMPS CONFIG */
   DBM_L1D_AMPS_CFG_TIMERS_SEG,
   DBM_L1D_AMPS_CFG_POLARITY_SEG,
   DBM_L1D_AMPS_CFG_TX_VOICE_MODE_SEG,
   DBM_L1D_AMPS_CFG_RANDOM_DIST_SEG,
   DBM_L1D_AMPS_CFG_RX_WORD_SYNC_SEG,
   DBM_L1D_AMPS_CFG_RX_RSSI_SEG,
   DBM_L1D_AMPS_CFG_AFC_SEG,                /* UNUSED */
   DBM_L1D_AMPS_CFG_TX_LEVELS_SEG,

   /* AMPS CALIBR */
   DBM_HWD_AMPS_TXAGC_SEG,
   DBM_HWD_AMPS_TXAGC_FREQ_ADJ_SEG,
   DBM_HWD_AMPS_TXAGC_TEMP_ADJ_SEG,
   DBM_HWD_AMPS_TX_HDET_SEG,
   DBM_HWD_AMPS_TX_HDET_FREQ_ADJ_SEG,
   DBM_HWD_AMPS_TX_HDET_TEMP_ADJ_SEG,
   DBM_HWD_AMPS_TX_HDET_MAX_BATT_SEG,       /* UNUSED */
   DBM_HWD_AMPS_RXAGC_SEG,

   /* COMMON */
   DBM_HWD_TEMP_OFFSET_PARMS_SEG,
   DBM_HWD_BATT_PDM_PARMS_SEG,

   /* CDMA PCS */
   DBM_HWD_PCS_TXAGC_BATT_ADJ_SEG,
   DBM_HWD_PCS_TX_LIM_TEMP_ADJ_SEG,
   DBM_HWD_PCS_TX_LIM_BATT_ADJ_SEG,

   /* CDMA CELL */
   DBM_HWD_CELL_TXAGC_BATT_ADJ_SEG,
   DBM_HWD_CELL_TX_LIM_TEMP_ADJ_SEG,
   DBM_HWD_CELL_TX_LIM_BATT_ADJ_SEG,

   /* AMPS CALIBR */
   DBM_HWD_AMPS_TXAGC_BATT_ADJ_SEG,
   DBM_HWD_AMPS_RXAGC_FREQ_ADJ_SEG,
   DBM_HWD_AMPS_RXAGC_TEMP_ADJ_SEG,

   /* COMMON */
   DBM_HWD_TX_FILTER_PARM_SEG,

   /* DCXO AFC CALIBR */
   DBM_HWD_AFC_TEMP_ADJ_SEG,

   /* DCXO IAFC VALUE */
   DBM_HWD_IAFC_VALUE_SEG,

#ifdef BAND_CLASS_5_SUPPORTED
   /* Added new 450Mhz band support for Moss.  yanliu */ 	
   DBM_HWD_450M_TXAGC_SEG = 0x3c,
   DBM_HWD_450M_TXAGC_FREQ_ADJ_SEG,
   DBM_HWD_450M_TXAGC_TEMP_ADJ_SEG,
   DBM_HWD_450M_TX_LIM_FREQ_ADJ_SEG,
   DBM_HWD_450M_TX_HDET_SEG,
   DBM_HWD_450M_TX_HDET_FREQ_ADJ_SEG,
   DBM_HWD_450M_TX_HDET_TEMP_ADJ_SEG,
   DBM_HWD_450M_TX_HDET_MAX_BATT_SEG,
   DBM_HWD_450M_RXAGC_SEG,
   DBM_HWD_450M_RXAGC_FREQ_ADJ_SEG,
   DBM_HWD_450M_RXAGC_TEMP_ADJ_SEG,
   DBM_HWD_450M_TXAGC_BATT_ADJ_SEG,
   DBM_HWD_450M_TX_LIM_TEMP_ADJ_SEG,
   DBM_HWD_450M_TX_LIM_BATT_ADJ_SEG,
#endif
 
   DBM_MAX_SEG_RF_DB
} DbmRFSegmentT;

/* Define data base Constants. These constants must be updated
   when adding new segments to this data base */

/* Define RF segment sizes in bytes */
typedef enum
{
    /* COMMON */
   DBM_HWD_AFC_PARMS_SIZE              = 4,
   DBM_HWD_BATTERY_PARMS_SIZE          = 32,
   DBM_HWD_TEMPERATURE_PARMS_SIZE      = 24,
   DBM_HWD_AUX_ADC_PARMS_SIZE          = 4,
   DBM_HWD_PDM_PARMS_SIZE              = 20,
   DBM_HWD_BANDGAP_PARMS_SIZE          = 2,
   DBM_HWD_CALIB_PARMS_SIZE            = 16,
   DBM_HWD_RX_BBF_PARMS_SIZE           = 8,

    /* CDMA PCS */
   DBM_HWD_PCS_TXAGC_SIZE              = 106,
   DBM_HWD_PCS_TXAGC_FREQ_ADJ_SIZE     = 192,
   DBM_HWD_PCS_TXAGC_TEMP_ADJ_SIZE     = 72,
   DBM_HWD_PCS_TX_LIM_FREQ_ADJ_SIZE    = 64, 
   DBM_HWD_PCS_TX_HDET_SIZE            = 32,
   DBM_HWD_PCS_TX_HDET_FREQ_ADJ_SIZE   = 64,
   DBM_HWD_PCS_TX_HDET_TEMP_ADJ_SIZE   = 24,
   DBM_HWD_PCS_TX_HDET_MAX_BATT_SIZE   = 32,
   DBM_HWD_PCS_RXAGC_SIZE              = 110,
   DBM_HWD_PCS_RXAGC_FREQ_ADJ_SIZE     = 64,
   DBM_HWD_PCS_RXAGC_TEMP_ADJ_SIZE     = 24,

    /* CDMA CELL */   
   DBM_HWD_CELL_TXAGC_SIZE             = 106,
   DBM_HWD_CELL_TXAGC_FREQ_ADJ_SIZE    = 192,
   DBM_HWD_CELL_TXAGC_TEMP_ADJ_SIZE    = 72,
   DBM_HWD_CELL_TX_LIM_FREQ_ADJ_SIZE   = 64,
   DBM_HWD_CELL_TX_HDET_SIZE           = 32,
   DBM_HWD_CELL_TX_HDET_FREQ_ADJ_SIZE  = 64,
   DBM_HWD_CELL_TX_HDET_TEMP_ADJ_SIZE  = 24,
   DBM_HWD_CELL_TX_HDET_MAX_BATT_SIZE  = 32,
   DBM_HWD_CELL_RXAGC_SIZE             = 110,
   DBM_HWD_CELL_RXAGC_FREQ_ADJ_SIZE    = 64,
   DBM_HWD_CELL_RXAGC_TEMP_ADJ_SIZE    = 24,

   /* AMPS CONFIG */
   DBM_L1D_AMPS_CFG_TIMERS_SIZE        = 28,
   DBM_L1D_AMPS_CFG_POLARITY_SIZE      = 2,
   DBM_L1D_AMPS_CFG_TX_VOICE_MODE_SIZE = 2,
   DBM_L1D_AMPS_CFG_RANDOM_DIST_SIZE   = 2,
   DBM_L1D_AMPS_CFG_RX_WORD_SYNC_SIZE  = 2,
   DBM_L1D_AMPS_CFG_RX_RSSI_SIZE       = 4,
   DBM_L1D_AMPS_CFG_AFC_SIZE           = 12,
   DBM_L1D_AMPS_CFG_TX_LEVELS_SIZE     = 24,

   /* AMPS CALIBR */
   DBM_HWD_AMPS_TXAGC_SIZE             = 106,
   DBM_HWD_AMPS_TXAGC_FREQ_ADJ_SIZE    = 64,
   DBM_HWD_AMPS_TXAGC_TEMP_ADJ_SIZE    = 24,
   DBM_HWD_AMPS_TX_HDET_SIZE           = 32,
   DBM_HWD_AMPS_TX_HDET_FREQ_ADJ_SIZE  = 64,
   DBM_HWD_AMPS_TX_HDET_TEMP_ADJ_SIZE  = 24,
   DBM_HWD_AMPS_TX_HDET_MAX_BATT_SIZE  = 20,
   DBM_HWD_AMPS_RXAGC_SIZE             = 110,

   /* COMMON */
   DBM_HWD_TEMP_OFFSET_PARMS_SIZE      = 2,
   DBM_HWD_BATT_PDM_PARMS_SIZE         = 32,

   /* CDMA PCS */
   DBM_HWD_PCS_TXAGC_BATT_ADJ_SIZE     = 32,
   DBM_HWD_PCS_TX_LIM_TEMP_ADJ_SIZE    = 24,
   DBM_HWD_PCS_TX_LIM_BATT_ADJ_SIZE    = 32,

   /* CDMA CELL */
   DBM_HWD_CELL_TXAGC_BATT_ADJ_SIZE    = 32,
   DBM_HWD_CELL_TX_LIM_TEMP_ADJ_SIZE   = 24,
   DBM_HWD_CELL_TX_LIM_BATT_ADJ_SIZE   = 32,

   /* AMPS CALIBR */
   DBM_HWD_AMPS_TXAGC_BATT_ADJ_SIZE    = 32,
   DBM_HWD_AMPS_RXAGC_FREQ_ADJ_SIZE    = 64,
   DBM_HWD_AMPS_RXAGC_TEMP_ADJ_SIZE    = 24,

#ifdef BAND_CLASS_5_SUPPORTED
   /* Added new 450Mhz band support for Moss.  yanliu */ 	
   DBM_HWD_450M_TXAGC_SIZE             = 106,
   DBM_HWD_450M_TXAGC_FREQ_ADJ_SIZE    = 192,
   DBM_HWD_450M_TXAGC_TEMP_ADJ_SIZE    = 72,
   DBM_HWD_450M_TX_LIM_FREQ_ADJ_SIZE   = 64,
   DBM_HWD_450M_TX_HDET_SIZE           = 32,
   DBM_HWD_450M_TX_HDET_FREQ_ADJ_SIZE  = 64,
   DBM_HWD_450M_TX_HDET_TEMP_ADJ_SIZE  = 24,
   DBM_HWD_450M_TX_HDET_MAX_BATT_SIZE  = 32,
   DBM_HWD_450M_RXAGC_SIZE             = 110,
   DBM_HWD_450M_RXAGC_FREQ_ADJ_SIZE    = 64,
   DBM_HWD_450M_RXAGC_TEMP_ADJ_SIZE    = 24,
   DBM_HWD_450M_TXAGC_BATT_ADJ_SIZE    = 32,
   DBM_HWD_450M_TX_LIM_TEMP_ADJ_SIZE   = 24,
   DBM_HWD_450M_TX_LIM_BATT_ADJ_SIZE   = 32,
#endif

   /* COMMON */
   DBM_HWD_TX_FILTER_PARM_SIZE         = 1,

   /* DCXO AFC CALIBR */
   DBM_HWD_AFC_TEMP_ADJ_SIZE           = 34,

   /* DCXO IAFC VALUE */
   DBM_HWD_IAFC_VALUE_SIZE             = 2

} DbmRFSegmentSizeT;

/* Define RF data base segment sizes */
#ifdef BAND_CLASS_5_SUPPORTED
#define DBM_RF_SEGMENT_SIZES \
   DBM_HWD_AFC_PARMS_SIZE,\
   DBM_HWD_BATTERY_PARMS_SIZE,\
   DBM_HWD_TEMPERATURE_PARMS_SIZE,\
   DBM_HWD_AUX_ADC_PARMS_SIZE,\
   DBM_HWD_PDM_PARMS_SIZE,\
   DBM_HWD_BANDGAP_PARMS_SIZE,\
   DBM_HWD_CALIB_PARMS_SIZE,\
   DBM_HWD_RX_BBF_PARMS_SIZE,\
   DBM_HWD_PCS_TXAGC_SIZE,\
   DBM_HWD_PCS_TXAGC_FREQ_ADJ_SIZE,\
   DBM_HWD_PCS_TXAGC_TEMP_ADJ_SIZE,\
   DBM_HWD_PCS_TX_LIM_FREQ_ADJ_SIZE,\
   DBM_HWD_PCS_TX_HDET_SIZE,\
   DBM_HWD_PCS_TX_HDET_FREQ_ADJ_SIZE,\
   DBM_HWD_PCS_TX_HDET_TEMP_ADJ_SIZE,\
   DBM_HWD_PCS_TX_HDET_MAX_BATT_SIZE,\
   DBM_HWD_PCS_RXAGC_SIZE,\
   DBM_HWD_PCS_RXAGC_FREQ_ADJ_SIZE,\
   DBM_HWD_PCS_RXAGC_TEMP_ADJ_SIZE,\
   DBM_HWD_CELL_TXAGC_SIZE,\
   DBM_HWD_CELL_TXAGC_FREQ_ADJ_SIZE,\
   DBM_HWD_CELL_TXAGC_TEMP_ADJ_SIZE,\
   DBM_HWD_CELL_TX_LIM_FREQ_ADJ_SIZE,\
   DBM_HWD_CELL_TX_HDET_SIZE,\
   DBM_HWD_CELL_TX_HDET_FREQ_ADJ_SIZE,\
   DBM_HWD_CELL_TX_HDET_TEMP_ADJ_SIZE,\
   DBM_HWD_CELL_TX_HDET_MAX_BATT_SIZE,\
   DBM_HWD_CELL_RXAGC_SIZE,\
   DBM_HWD_CELL_RXAGC_FREQ_ADJ_SIZE,\
   DBM_HWD_CELL_RXAGC_TEMP_ADJ_SIZE,\
   DBM_L1D_AMPS_CFG_TIMERS_SIZE,\
   DBM_L1D_AMPS_CFG_POLARITY_SIZE,\
   DBM_L1D_AMPS_CFG_TX_VOICE_MODE_SIZE,\
   DBM_L1D_AMPS_CFG_RANDOM_DIST_SIZE,\
   DBM_L1D_AMPS_CFG_RX_WORD_SYNC_SIZE,\
   DBM_L1D_AMPS_CFG_RX_RSSI_SIZE,\
   DBM_L1D_AMPS_CFG_AFC_SIZE,\
   DBM_L1D_AMPS_CFG_TX_LEVELS_SIZE,\
   DBM_HWD_AMPS_TXAGC_SIZE,\
   DBM_HWD_AMPS_TXAGC_FREQ_ADJ_SIZE,\
   DBM_HWD_AMPS_TXAGC_TEMP_ADJ_SIZE,\
   DBM_HWD_AMPS_TX_HDET_SIZE,\
   DBM_HWD_AMPS_TX_HDET_FREQ_ADJ_SIZE,\
   DBM_HWD_AMPS_TX_HDET_TEMP_ADJ_SIZE,\
   DBM_HWD_AMPS_TX_HDET_MAX_BATT_SIZE,\
   DBM_HWD_AMPS_RXAGC_SIZE,\
   DBM_HWD_TEMP_OFFSET_PARMS_SIZE,\
   DBM_HWD_BATT_PDM_PARMS_SIZE,\
   DBM_HWD_PCS_TXAGC_BATT_ADJ_SIZE,\
   DBM_HWD_PCS_TX_LIM_TEMP_ADJ_SIZE,\
   DBM_HWD_PCS_TX_LIM_BATT_ADJ_SIZE,\
   DBM_HWD_CELL_TXAGC_BATT_ADJ_SIZE,\
   DBM_HWD_CELL_TX_LIM_TEMP_ADJ_SIZE,\
   DBM_HWD_CELL_TX_LIM_BATT_ADJ_SIZE,\
   DBM_HWD_AMPS_TXAGC_BATT_ADJ_SIZE,\
   DBM_HWD_AMPS_RXAGC_FREQ_ADJ_SIZE,\
   DBM_HWD_AMPS_RXAGC_TEMP_ADJ_SIZE, \
   DBM_HWD_TX_FILTER_PARM_SIZE,\
   DBM_HWD_AFC_TEMP_ADJ_SIZE,\
   DBM_HWD_IAFC_VALUE_SIZE,\
   DBM_HWD_450M_TXAGC_SIZE,\
   DBM_HWD_450M_TXAGC_FREQ_ADJ_SIZE,\
   DBM_HWD_450M_TXAGC_TEMP_ADJ_SIZE,\
   DBM_HWD_450M_TX_LIM_FREQ_ADJ_SIZE,\
   DBM_HWD_450M_TX_HDET_SIZE,\
   DBM_HWD_450M_TX_HDET_FREQ_ADJ_SIZE,\
   DBM_HWD_450M_TX_HDET_TEMP_ADJ_SIZE,\
   DBM_HWD_450M_TX_HDET_MAX_BATT_SIZE,\
   DBM_HWD_450M_RXAGC_SIZE,\
   DBM_HWD_450M_RXAGC_FREQ_ADJ_SIZE,\
   DBM_HWD_450M_RXAGC_TEMP_ADJ_SIZE,\
   DBM_HWD_450M_TXAGC_BATT_ADJ_SIZE,\
   DBM_HWD_450M_TX_LIM_TEMP_ADJ_SIZE,\
   DBM_HWD_450M_TX_LIM_BATT_ADJ_SIZE
 #else
#define DBM_RF_SEGMENT_SIZES \
   DBM_HWD_AFC_PARMS_SIZE,\
   DBM_HWD_BATTERY_PARMS_SIZE,\
   DBM_HWD_TEMPERATURE_PARMS_SIZE,\
   DBM_HWD_AUX_ADC_PARMS_SIZE,\
   DBM_HWD_PDM_PARMS_SIZE,\
   DBM_HWD_BANDGAP_PARMS_SIZE,\
   DBM_HWD_CALIB_PARMS_SIZE,\
   DBM_HWD_RX_BBF_PARMS_SIZE,\
   DBM_HWD_PCS_TXAGC_SIZE,\
   DBM_HWD_PCS_TXAGC_FREQ_ADJ_SIZE,\
   DBM_HWD_PCS_TXAGC_TEMP_ADJ_SIZE,\
   DBM_HWD_PCS_TX_LIM_FREQ_ADJ_SIZE,\
   DBM_HWD_PCS_TX_HDET_SIZE,\
   DBM_HWD_PCS_TX_HDET_FREQ_ADJ_SIZE,\
   DBM_HWD_PCS_TX_HDET_TEMP_ADJ_SIZE,\
   DBM_HWD_PCS_TX_HDET_MAX_BATT_SIZE,\
   DBM_HWD_PCS_RXAGC_SIZE,\
   DBM_HWD_PCS_RXAGC_FREQ_ADJ_SIZE,\
   DBM_HWD_PCS_RXAGC_TEMP_ADJ_SIZE,\
   DBM_HWD_CELL_TXAGC_SIZE,\
   DBM_HWD_CELL_TXAGC_FREQ_ADJ_SIZE,\
   DBM_HWD_CELL_TXAGC_TEMP_ADJ_SIZE,\
   DBM_HWD_CELL_TX_LIM_FREQ_ADJ_SIZE,\
   DBM_HWD_CELL_TX_HDET_SIZE,\
   DBM_HWD_CELL_TX_HDET_FREQ_ADJ_SIZE,\
   DBM_HWD_CELL_TX_HDET_TEMP_ADJ_SIZE,\
   DBM_HWD_CELL_TX_HDET_MAX_BATT_SIZE,\
   DBM_HWD_CELL_RXAGC_SIZE,\
   DBM_HWD_CELL_RXAGC_FREQ_ADJ_SIZE,\
   DBM_HWD_CELL_RXAGC_TEMP_ADJ_SIZE,\
   DBM_L1D_AMPS_CFG_TIMERS_SIZE,\
   DBM_L1D_AMPS_CFG_POLARITY_SIZE,\
   DBM_L1D_AMPS_CFG_TX_VOICE_MODE_SIZE,\
   DBM_L1D_AMPS_CFG_RANDOM_DIST_SIZE,\
   DBM_L1D_AMPS_CFG_RX_WORD_SYNC_SIZE,\
   DBM_L1D_AMPS_CFG_RX_RSSI_SIZE,\
   DBM_L1D_AMPS_CFG_AFC_SIZE,\
   DBM_L1D_AMPS_CFG_TX_LEVELS_SIZE,\
   DBM_HWD_AMPS_TXAGC_SIZE,\
   DBM_HWD_AMPS_TXAGC_FREQ_ADJ_SIZE,\
   DBM_HWD_AMPS_TXAGC_TEMP_ADJ_SIZE,\
   DBM_HWD_AMPS_TX_HDET_SIZE,\
   DBM_HWD_AMPS_TX_HDET_FREQ_ADJ_SIZE,\
   DBM_HWD_AMPS_TX_HDET_TEMP_ADJ_SIZE,\
   DBM_HWD_AMPS_TX_HDET_MAX_BATT_SIZE,\
   DBM_HWD_AMPS_RXAGC_SIZE,\
   DBM_HWD_TEMP_OFFSET_PARMS_SIZE,\
   DBM_HWD_BATT_PDM_PARMS_SIZE,\
   DBM_HWD_PCS_TXAGC_BATT_ADJ_SIZE,\
   DBM_HWD_PCS_TX_LIM_TEMP_ADJ_SIZE,\
   DBM_HWD_PCS_TX_LIM_BATT_ADJ_SIZE,\
   DBM_HWD_CELL_TXAGC_BATT_ADJ_SIZE,\
   DBM_HWD_CELL_TX_LIM_TEMP_ADJ_SIZE,\
   DBM_HWD_CELL_TX_LIM_BATT_ADJ_SIZE,\
   DBM_HWD_AMPS_TXAGC_BATT_ADJ_SIZE,\
   DBM_HWD_AMPS_RXAGC_FREQ_ADJ_SIZE,\
   DBM_HWD_AMPS_RXAGC_TEMP_ADJ_SIZE, \
   DBM_HWD_TX_FILTER_PARM_SIZE,\
   DBM_HWD_AFC_TEMP_ADJ_SIZE,\
   DBM_HWD_IAFC_VALUE_SIZE
#endif
   
/* Define segment indexes into RF data base cache */
typedef enum
{
   DBM_RF_MARKER_INDEX                  = 0,

   /* COMMON */
   DBM_HWD_AFC_PARMS_INDEX              = DBM_RF_MARKER_INDEX                  + DBM_MARKER_SIZE,
   DBM_HWD_BATTERY_PARMS_INDEX          = DBM_HWD_AFC_PARMS_INDEX              + DBM_HWD_AFC_PARMS_SIZE,
   DBM_HWD_TEMPERATURE_PARMS_INDEX      = DBM_HWD_BATTERY_PARMS_INDEX          + DBM_HWD_BATTERY_PARMS_SIZE,
   DBM_HWD_AUX_ADC_PARMS_INDEX          = DBM_HWD_TEMPERATURE_PARMS_INDEX      + DBM_HWD_TEMPERATURE_PARMS_SIZE,
   DBM_HWD_PDM_PARMS_INDEX              = DBM_HWD_AUX_ADC_PARMS_INDEX          + DBM_HWD_AUX_ADC_PARMS_SIZE,
   DBM_HWD_BANDGAP_PARMS_INDEX          = DBM_HWD_PDM_PARMS_INDEX              + DBM_HWD_PDM_PARMS_SIZE,
   DBM_HWD_CALIB_PARMS_INDEX            = DBM_HWD_BANDGAP_PARMS_INDEX          + DBM_HWD_BANDGAP_PARMS_SIZE,
   DBM_HWD_RX_BBF_PARMS_INDEX           = DBM_HWD_CALIB_PARMS_INDEX            + DBM_HWD_CALIB_PARMS_SIZE,

    /* CDMA PCS */
   DBM_HWD_PCS_TXAGC_INDEX              = DBM_HWD_RX_BBF_PARMS_INDEX           + DBM_HWD_RX_BBF_PARMS_SIZE,
   DBM_HWD_PCS_TXAGC_FREQ_ADJ_INDEX     = DBM_HWD_PCS_TXAGC_INDEX              + DBM_HWD_PCS_TXAGC_SIZE,
   DBM_HWD_PCS_TXAGC_TEMP_ADJ_INDEX     = DBM_HWD_PCS_TXAGC_FREQ_ADJ_INDEX     + DBM_HWD_PCS_TXAGC_FREQ_ADJ_SIZE,
   DBM_HWD_PCS_TX_LIM_FREQ_ADJ_INDEX    = DBM_HWD_PCS_TXAGC_TEMP_ADJ_INDEX     + DBM_HWD_PCS_TXAGC_TEMP_ADJ_SIZE,
   DBM_HWD_PCS_TX_HDET_INDEX            = DBM_HWD_PCS_TX_LIM_FREQ_ADJ_INDEX    + DBM_HWD_PCS_TX_LIM_FREQ_ADJ_SIZE,
   DBM_HWD_PCS_TX_HDET_FREQ_ADJ_INDEX   = DBM_HWD_PCS_TX_HDET_INDEX            + DBM_HWD_PCS_TX_HDET_SIZE,
   DBM_HWD_PCS_TX_HDET_TEMP_ADJ_INDEX   = DBM_HWD_PCS_TX_HDET_FREQ_ADJ_INDEX   + DBM_HWD_PCS_TX_HDET_FREQ_ADJ_SIZE,
   DBM_HWD_PCS_TX_HDET_MAX_BATT_INDEX   = DBM_HWD_PCS_TX_HDET_TEMP_ADJ_INDEX   + DBM_HWD_PCS_TX_HDET_TEMP_ADJ_SIZE,
   DBM_HWD_PCS_RXAGC_INDEX              = DBM_HWD_PCS_TX_HDET_MAX_BATT_INDEX   + DBM_HWD_PCS_TX_HDET_MAX_BATT_SIZE,
   DBM_HWD_PCS_RXAGC_FREQ_ADJ_INDEX     = DBM_HWD_PCS_RXAGC_INDEX              + DBM_HWD_PCS_RXAGC_SIZE,
   DBM_HWD_PCS_RXAGC_TEMP_ADJ_INDEX     = DBM_HWD_PCS_RXAGC_FREQ_ADJ_INDEX     + DBM_HWD_PCS_RXAGC_FREQ_ADJ_SIZE,

    /* CDMA CELL */   
   DBM_HWD_CELL_TXAGC_INDEX             = DBM_HWD_PCS_RXAGC_TEMP_ADJ_INDEX     + DBM_HWD_PCS_RXAGC_TEMP_ADJ_SIZE,
   DBM_HWD_CELL_TXAGC_FREQ_ADJ_INDEX    = DBM_HWD_CELL_TXAGC_INDEX             + DBM_HWD_CELL_TXAGC_SIZE,
   DBM_HWD_CELL_TXAGC_TEMP_ADJ_INDEX    = DBM_HWD_CELL_TXAGC_FREQ_ADJ_INDEX    + DBM_HWD_CELL_TXAGC_FREQ_ADJ_SIZE,
   DBM_HWD_CELL_TX_LIM_FREQ_ADJ_INDEX   = DBM_HWD_CELL_TXAGC_TEMP_ADJ_INDEX    + DBM_HWD_CELL_TXAGC_TEMP_ADJ_SIZE,
   DBM_HWD_CELL_TX_HDET_INDEX           = DBM_HWD_CELL_TX_LIM_FREQ_ADJ_INDEX   + DBM_HWD_CELL_TX_LIM_FREQ_ADJ_SIZE,
   DBM_HWD_CELL_TX_HDET_FREQ_ADJ_INDEX  = DBM_HWD_CELL_TX_HDET_INDEX           + DBM_HWD_CELL_TX_HDET_SIZE,
   DBM_HWD_CELL_TX_HDET_TEMP_ADJ_INDEX  = DBM_HWD_CELL_TX_HDET_FREQ_ADJ_INDEX  + DBM_HWD_CELL_TX_HDET_FREQ_ADJ_SIZE,
   DBM_HWD_CELL_TX_HDET_MAX_BATT_INDEX  = DBM_HWD_CELL_TX_HDET_TEMP_ADJ_INDEX  + DBM_HWD_CELL_TX_HDET_TEMP_ADJ_SIZE,
   DBM_HWD_CELL_RXAGC_INDEX             = DBM_HWD_CELL_TX_HDET_MAX_BATT_INDEX  + DBM_HWD_CELL_TX_HDET_MAX_BATT_SIZE,
   DBM_HWD_CELL_RXAGC_FREQ_ADJ_INDEX    = DBM_HWD_CELL_RXAGC_INDEX             + DBM_HWD_CELL_RXAGC_SIZE,
   DBM_HWD_CELL_RXAGC_TEMP_ADJ_INDEX    = DBM_HWD_CELL_RXAGC_FREQ_ADJ_INDEX    + DBM_HWD_CELL_RXAGC_FREQ_ADJ_SIZE,

   /* AMPS CONFIG */
   DBM_L1D_AMPS_CFG_TIMERS_INDEX        = DBM_HWD_CELL_RXAGC_TEMP_ADJ_INDEX    + DBM_HWD_CELL_RXAGC_TEMP_ADJ_SIZE,
   DBM_L1D_AMPS_CFG_POLARITY_INDEX      = DBM_L1D_AMPS_CFG_TIMERS_INDEX        + DBM_L1D_AMPS_CFG_TIMERS_SIZE,
   DBM_L1D_AMPS_CFG_TX_VOICE_MODE_INDEX = DBM_L1D_AMPS_CFG_POLARITY_INDEX      + DBM_L1D_AMPS_CFG_POLARITY_SIZE,
   DBM_L1D_AMPS_CFG_RANDOM_DIST_INDEX   = DBM_L1D_AMPS_CFG_TX_VOICE_MODE_INDEX + DBM_L1D_AMPS_CFG_TX_VOICE_MODE_SIZE,
   DBM_L1D_AMPS_CFG_RX_WORD_SYNC_INDEX  = DBM_L1D_AMPS_CFG_RANDOM_DIST_INDEX   + DBM_L1D_AMPS_CFG_RANDOM_DIST_SIZE,
   DBM_L1D_AMPS_CFG_RX_RSSI_INDEX       = DBM_L1D_AMPS_CFG_RX_WORD_SYNC_INDEX  + DBM_L1D_AMPS_CFG_RX_WORD_SYNC_SIZE,
   DBM_L1D_AMPS_CFG_AFC_INDEX           = DBM_L1D_AMPS_CFG_RX_RSSI_INDEX       + DBM_L1D_AMPS_CFG_RX_RSSI_SIZE,
   DBM_L1D_AMPS_CFG_TX_LEVELS_INDEX     = DBM_L1D_AMPS_CFG_AFC_INDEX           + DBM_L1D_AMPS_CFG_AFC_SIZE,

   /* AMPS CALIBR */
   DBM_HWD_AMPS_TXAGC_INDEX             = DBM_L1D_AMPS_CFG_TX_LEVELS_INDEX     + DBM_L1D_AMPS_CFG_TX_LEVELS_SIZE,
   DBM_HWD_AMPS_TXAGC_FREQ_ADJ_INDEX    = DBM_HWD_AMPS_TXAGC_INDEX             + DBM_HWD_AMPS_TXAGC_SIZE,
   DBM_HWD_AMPS_TXAGC_TEMP_ADJ_INDEX    = DBM_HWD_AMPS_TXAGC_FREQ_ADJ_INDEX    + DBM_HWD_AMPS_TXAGC_FREQ_ADJ_SIZE,
   DBM_HWD_AMPS_TX_HDET_INDEX           = DBM_HWD_AMPS_TXAGC_TEMP_ADJ_INDEX    + DBM_HWD_AMPS_TXAGC_TEMP_ADJ_SIZE,
   DBM_HWD_AMPS_TX_HDET_FREQ_ADJ_INDEX  = DBM_HWD_AMPS_TX_HDET_INDEX           + DBM_HWD_AMPS_TX_HDET_SIZE,
   DBM_HWD_AMPS_TX_HDET_TEMP_ADJ_INDEX  = DBM_HWD_AMPS_TX_HDET_FREQ_ADJ_INDEX  + DBM_HWD_AMPS_TX_HDET_FREQ_ADJ_SIZE,
   DBM_HWD_AMPS_TX_HDET_MAX_BATT_INDEX  = DBM_HWD_AMPS_TX_HDET_TEMP_ADJ_INDEX  + DBM_HWD_AMPS_TX_HDET_TEMP_ADJ_SIZE,
   DBM_HWD_AMPS_RXAGC_INDEX             = DBM_HWD_AMPS_TX_HDET_MAX_BATT_INDEX  + DBM_HWD_AMPS_TX_HDET_MAX_BATT_SIZE,

   /* COMMON */
   DBM_HWD_TEMP_OFFSET_PARMS_INDEX      = DBM_HWD_AMPS_RXAGC_INDEX             + DBM_HWD_AMPS_RXAGC_SIZE,
   DBM_HWD_BATT_PDM_PARMS_INDEX         = DBM_HWD_TEMP_OFFSET_PARMS_INDEX      + DBM_HWD_TEMP_OFFSET_PARMS_SIZE,

   /* CDMA PCS */
   DBM_HWD_PCS_TXAGC_BATT_ADJ_INDEX     = DBM_HWD_BATT_PDM_PARMS_INDEX         + DBM_HWD_BATT_PDM_PARMS_SIZE,
   DBM_HWD_PCS_TX_LIM_TEMP_ADJ_INDEX    = DBM_HWD_PCS_TXAGC_BATT_ADJ_INDEX     + DBM_HWD_PCS_TXAGC_BATT_ADJ_SIZE,
   DBM_HWD_PCS_TX_LIM_BATT_ADJ_INDEX    = DBM_HWD_PCS_TX_LIM_TEMP_ADJ_INDEX    + DBM_HWD_PCS_TX_LIM_TEMP_ADJ_SIZE,

   /* CDMA CELL */
   DBM_HWD_CELL_TXAGC_BATT_ADJ_INDEX    = DBM_HWD_PCS_TX_LIM_BATT_ADJ_INDEX    + DBM_HWD_PCS_TX_LIM_BATT_ADJ_SIZE,
   DBM_HWD_CELL_TX_LIM_TEMP_ADJ_INDEX   = DBM_HWD_CELL_TXAGC_BATT_ADJ_INDEX    + DBM_HWD_CELL_TXAGC_BATT_ADJ_SIZE,
   DBM_HWD_CELL_TX_LIM_BATT_ADJ_INDEX   = DBM_HWD_CELL_TX_LIM_TEMP_ADJ_INDEX   + DBM_HWD_CELL_TX_LIM_TEMP_ADJ_SIZE,

   /* AMPS CALIBR */
   DBM_HWD_AMPS_TXAGC_BATT_ADJ_INDEX    = DBM_HWD_CELL_TX_LIM_BATT_ADJ_INDEX   + DBM_HWD_CELL_TX_LIM_BATT_ADJ_SIZE,
   DBM_HWD_AMPS_RXAGC_FREQ_ADJ_INDEX    = DBM_HWD_AMPS_TXAGC_BATT_ADJ_INDEX    + DBM_HWD_AMPS_TXAGC_BATT_ADJ_SIZE,
   DBM_HWD_AMPS_RXAGC_TEMP_ADJ_INDEX    = DBM_HWD_AMPS_RXAGC_FREQ_ADJ_INDEX    + DBM_HWD_AMPS_RXAGC_FREQ_ADJ_SIZE,

   /* COMMON */
   DBM_HWD_TX_FILTER_PARM_INDEX         = DBM_HWD_AMPS_RXAGC_TEMP_ADJ_INDEX    + DBM_HWD_AMPS_RXAGC_TEMP_ADJ_SIZE,

   /* DCXO AFC CALIBR */
   DBM_HWD_AFC_TEMP_ADJ_INDEX           = DBM_HWD_TX_FILTER_PARM_INDEX         + DBM_HWD_TX_FILTER_PARM_SIZE,

   /* DCXO IAFC VALUE */
   DBM_HWD_IAFC_VALUE_INDEX             = DBM_HWD_AFC_TEMP_ADJ_INDEX           + DBM_HWD_AFC_TEMP_ADJ_SIZE,

#ifdef BAND_CLASS_5_SUPPORTED
   DBM_HWD_450M_TXAGC_INDEX             =  DBM_HWD_IAFC_VALUE_INDEX             + DBM_HWD_IAFC_VALUE_SIZE,
   DBM_HWD_450M_TXAGC_FREQ_ADJ_INDEX    = DBM_HWD_450M_TXAGC_INDEX             + DBM_HWD_450M_TXAGC_SIZE,
   DBM_HWD_450M_TXAGC_TEMP_ADJ_INDEX    = DBM_HWD_450M_TXAGC_FREQ_ADJ_INDEX    + DBM_HWD_450M_TXAGC_FREQ_ADJ_SIZE,
   DBM_HWD_450M_TX_LIM_FREQ_ADJ_INDEX   = DBM_HWD_450M_TXAGC_TEMP_ADJ_INDEX    + DBM_HWD_450M_TXAGC_TEMP_ADJ_SIZE,
   DBM_HWD_450M_TX_HDET_INDEX           = DBM_HWD_450M_TX_LIM_FREQ_ADJ_INDEX   + DBM_HWD_450M_TX_LIM_FREQ_ADJ_SIZE,
   DBM_HWD_450M_TX_HDET_FREQ_ADJ_INDEX  = DBM_HWD_450M_TX_HDET_INDEX           + DBM_HWD_450M_TX_HDET_SIZE,
   DBM_HWD_450M_TX_HDET_TEMP_ADJ_INDEX  = DBM_HWD_450M_TX_HDET_FREQ_ADJ_INDEX  + DBM_HWD_450M_TX_HDET_FREQ_ADJ_SIZE,
   DBM_HWD_450M_TX_HDET_MAX_BATT_INDEX  = DBM_HWD_450M_TX_HDET_TEMP_ADJ_INDEX  + DBM_HWD_450M_TX_HDET_TEMP_ADJ_SIZE,
   DBM_HWD_450M_RXAGC_INDEX             = DBM_HWD_450M_TX_HDET_MAX_BATT_INDEX  + DBM_HWD_450M_TX_HDET_MAX_BATT_SIZE,
   DBM_HWD_450M_RXAGC_FREQ_ADJ_INDEX    = DBM_HWD_450M_RXAGC_INDEX             + DBM_HWD_450M_RXAGC_SIZE,
   DBM_HWD_450M_RXAGC_TEMP_ADJ_INDEX    = DBM_HWD_450M_RXAGC_FREQ_ADJ_INDEX    + DBM_HWD_450M_RXAGC_FREQ_ADJ_SIZE,
   DBM_HWD_450M_TXAGC_BATT_ADJ_INDEX    =  DBM_HWD_450M_RXAGC_TEMP_ADJ_INDEX    + DBM_HWD_450M_RXAGC_TEMP_ADJ_SIZE,
   DBM_HWD_450M_TX_LIM_TEMP_ADJ_INDEX   = DBM_HWD_450M_TXAGC_BATT_ADJ_INDEX    + DBM_HWD_450M_TXAGC_BATT_ADJ_SIZE,
   DBM_HWD_450M_TX_LIM_BATT_ADJ_INDEX   = DBM_HWD_450M_TX_LIM_TEMP_ADJ_INDEX   + DBM_HWD_450M_TX_LIM_TEMP_ADJ_SIZE,

   DBM_RF_CACHE_SIZE                    = DBM_HWD_450M_TX_LIM_BATT_ADJ_INDEX    + DBM_HWD_450M_TX_LIM_BATT_ADJ_SIZE
#else
   DBM_RF_CACHE_SIZE                    = DBM_HWD_IAFC_VALUE_INDEX             + DBM_HWD_IAFC_VALUE_SIZE
#endif    
} DbmRFSegmentIndexT;


/* Define RF data base segment indexes */
#ifdef BAND_CLASS_5_SUPPORTED
#define DBM_RF_SEGMENT_INDEXES \
   DBM_HWD_AFC_PARMS_INDEX,\
   DBM_HWD_BATTERY_PARMS_INDEX,\
   DBM_HWD_TEMPERATURE_PARMS_INDEX,\
   DBM_HWD_AUX_ADC_PARMS_INDEX,\
   DBM_HWD_PDM_PARMS_INDEX,\
   DBM_HWD_BANDGAP_PARMS_INDEX,\
   DBM_HWD_CALIB_PARMS_INDEX,\
   DBM_HWD_RX_BBF_PARMS_INDEX,\
   DBM_HWD_PCS_TXAGC_INDEX,\
   DBM_HWD_PCS_TXAGC_FREQ_ADJ_INDEX,\
   DBM_HWD_PCS_TXAGC_TEMP_ADJ_INDEX,\
   DBM_HWD_PCS_TX_LIM_FREQ_ADJ_INDEX,\
   DBM_HWD_PCS_TX_HDET_INDEX,\
   DBM_HWD_PCS_TX_HDET_FREQ_ADJ_INDEX,\
   DBM_HWD_PCS_TX_HDET_TEMP_ADJ_INDEX,\
   DBM_HWD_PCS_TX_HDET_MAX_BATT_INDEX,\
   DBM_HWD_PCS_RXAGC_INDEX,\
   DBM_HWD_PCS_RXAGC_FREQ_ADJ_INDEX,\
   DBM_HWD_PCS_RXAGC_TEMP_ADJ_INDEX,\
   DBM_HWD_CELL_TXAGC_INDEX,\
   DBM_HWD_CELL_TXAGC_FREQ_ADJ_INDEX,\
   DBM_HWD_CELL_TXAGC_TEMP_ADJ_INDEX,\
   DBM_HWD_CELL_TX_LIM_FREQ_ADJ_INDEX,\
   DBM_HWD_CELL_TX_HDET_INDEX,\
   DBM_HWD_CELL_TX_HDET_FREQ_ADJ_INDEX,\
   DBM_HWD_CELL_TX_HDET_TEMP_ADJ_INDEX,\
   DBM_HWD_CELL_TX_HDET_MAX_BATT_INDEX,\
   DBM_HWD_CELL_RXAGC_INDEX,\
   DBM_HWD_CELL_RXAGC_FREQ_ADJ_INDEX,\
   DBM_HWD_CELL_RXAGC_TEMP_ADJ_INDEX,\
   DBM_L1D_AMPS_CFG_TIMERS_INDEX,\
   DBM_L1D_AMPS_CFG_POLARITY_INDEX,\
   DBM_L1D_AMPS_CFG_TX_VOICE_MODE_INDEX,\
   DBM_L1D_AMPS_CFG_RANDOM_DIST_INDEX,\
   DBM_L1D_AMPS_CFG_RX_WORD_SYNC_INDEX,\
   DBM_L1D_AMPS_CFG_RX_RSSI_INDEX,\
   DBM_L1D_AMPS_CFG_AFC_INDEX,\
   DBM_L1D_AMPS_CFG_TX_LEVELS_INDEX,\
   DBM_HWD_AMPS_TXAGC_INDEX,\
   DBM_HWD_AMPS_TXAGC_FREQ_ADJ_INDEX,\
   DBM_HWD_AMPS_TXAGC_TEMP_ADJ_INDEX,\
   DBM_HWD_AMPS_TX_HDET_INDEX,\
   DBM_HWD_AMPS_TX_HDET_FREQ_ADJ_INDEX,\
   DBM_HWD_AMPS_TX_HDET_TEMP_ADJ_INDEX,\
   DBM_HWD_AMPS_TX_HDET_MAX_BATT_INDEX,\
   DBM_HWD_AMPS_RXAGC_INDEX,\
   DBM_HWD_TEMP_OFFSET_PARMS_INDEX,\
   DBM_HWD_BATT_PDM_PARMS_INDEX,\
   DBM_HWD_PCS_TXAGC_BATT_ADJ_INDEX,\
   DBM_HWD_PCS_TX_LIM_TEMP_ADJ_INDEX,\
   DBM_HWD_PCS_TX_LIM_BATT_ADJ_INDEX,\
   DBM_HWD_CELL_TXAGC_BATT_ADJ_INDEX,\
   DBM_HWD_CELL_TX_LIM_TEMP_ADJ_INDEX,\
   DBM_HWD_CELL_TX_LIM_BATT_ADJ_INDEX,\
   DBM_HWD_AMPS_TXAGC_BATT_ADJ_INDEX,\
   DBM_HWD_AMPS_RXAGC_FREQ_ADJ_INDEX,\
   DBM_HWD_AMPS_RXAGC_TEMP_ADJ_INDEX,\
   DBM_HWD_TX_FILTER_PARM_INDEX,\
   DBM_HWD_AFC_TEMP_ADJ_INDEX,\
   DBM_HWD_IAFC_VALUE_INDEX,\
   DBM_HWD_IAFC_VALUE_INDEX,\
   DBM_HWD_450M_TXAGC_INDEX,\
   DBM_HWD_450M_TXAGC_FREQ_ADJ_INDEX,\
   DBM_HWD_450M_TXAGC_TEMP_ADJ_INDEX,\
   DBM_HWD_450M_TX_LIM_FREQ_ADJ_INDEX,\
   DBM_HWD_450M_TX_HDET_INDEX,\
   DBM_HWD_450M_TX_HDET_FREQ_ADJ_INDEX,\
   DBM_HWD_450M_TX_HDET_TEMP_ADJ_INDEX,\
   DBM_HWD_450M_TX_HDET_MAX_BATT_INDEX,\
   DBM_HWD_450M_RXAGC_INDEX,\
   DBM_HWD_450M_RXAGC_FREQ_ADJ_INDEX,\
   DBM_HWD_450M_RXAGC_TEMP_ADJ_INDEX,\
   DBM_HWD_450M_TXAGC_BATT_ADJ_INDEX,\
   DBM_HWD_450M_TX_LIM_TEMP_ADJ_INDEX,\
   DBM_HWD_450M_TX_LIM_BATT_ADJ_INDEX
#else
#define DBM_RF_SEGMENT_INDEXES \
   DBM_HWD_AFC_PARMS_INDEX,\
   DBM_HWD_BATTERY_PARMS_INDEX,\
   DBM_HWD_TEMPERATURE_PARMS_INDEX,\
   DBM_HWD_AUX_ADC_PARMS_INDEX,\
   DBM_HWD_PDM_PARMS_INDEX,\
   DBM_HWD_BANDGAP_PARMS_INDEX,\
   DBM_HWD_CALIB_PARMS_INDEX,\
   DBM_HWD_RX_BBF_PARMS_INDEX,\
   DBM_HWD_PCS_TXAGC_INDEX,\
   DBM_HWD_PCS_TXAGC_FREQ_ADJ_INDEX,\
   DBM_HWD_PCS_TXAGC_TEMP_ADJ_INDEX,\
   DBM_HWD_PCS_TX_LIM_FREQ_ADJ_INDEX,\
   DBM_HWD_PCS_TX_HDET_INDEX,\
   DBM_HWD_PCS_TX_HDET_FREQ_ADJ_INDEX,\
   DBM_HWD_PCS_TX_HDET_TEMP_ADJ_INDEX,\
   DBM_HWD_PCS_TX_HDET_MAX_BATT_INDEX,\
   DBM_HWD_PCS_RXAGC_INDEX,\
   DBM_HWD_PCS_RXAGC_FREQ_ADJ_INDEX,\
   DBM_HWD_PCS_RXAGC_TEMP_ADJ_INDEX,\
   DBM_HWD_CELL_TXAGC_INDEX,\
   DBM_HWD_CELL_TXAGC_FREQ_ADJ_INDEX,\
   DBM_HWD_CELL_TXAGC_TEMP_ADJ_INDEX,\
   DBM_HWD_CELL_TX_LIM_FREQ_ADJ_INDEX,\
   DBM_HWD_CELL_TX_HDET_INDEX,\
   DBM_HWD_CELL_TX_HDET_FREQ_ADJ_INDEX,\
   DBM_HWD_CELL_TX_HDET_TEMP_ADJ_INDEX,\
   DBM_HWD_CELL_TX_HDET_MAX_BATT_INDEX,\
   DBM_HWD_CELL_RXAGC_INDEX,\
   DBM_HWD_CELL_RXAGC_FREQ_ADJ_INDEX,\
   DBM_HWD_CELL_RXAGC_TEMP_ADJ_INDEX,\
   DBM_L1D_AMPS_CFG_TIMERS_INDEX,\
   DBM_L1D_AMPS_CFG_POLARITY_INDEX,\
   DBM_L1D_AMPS_CFG_TX_VOICE_MODE_INDEX,\
   DBM_L1D_AMPS_CFG_RANDOM_DIST_INDEX,\
   DBM_L1D_AMPS_CFG_RX_WORD_SYNC_INDEX,\
   DBM_L1D_AMPS_CFG_RX_RSSI_INDEX,\
   DBM_L1D_AMPS_CFG_AFC_INDEX,\
   DBM_L1D_AMPS_CFG_TX_LEVELS_INDEX,\
   DBM_HWD_AMPS_TXAGC_INDEX,\
   DBM_HWD_AMPS_TXAGC_FREQ_ADJ_INDEX,\
   DBM_HWD_AMPS_TXAGC_TEMP_ADJ_INDEX,\
   DBM_HWD_AMPS_TX_HDET_INDEX,\
   DBM_HWD_AMPS_TX_HDET_FREQ_ADJ_INDEX,\
   DBM_HWD_AMPS_TX_HDET_TEMP_ADJ_INDEX,\
   DBM_HWD_AMPS_TX_HDET_MAX_BATT_INDEX,\
   DBM_HWD_AMPS_RXAGC_INDEX,\
   DBM_HWD_TEMP_OFFSET_PARMS_INDEX,\
   DBM_HWD_BATT_PDM_PARMS_INDEX,\
   DBM_HWD_PCS_TXAGC_BATT_ADJ_INDEX,\
   DBM_HWD_PCS_TX_LIM_TEMP_ADJ_INDEX,\
   DBM_HWD_PCS_TX_LIM_BATT_ADJ_INDEX,\
   DBM_HWD_CELL_TXAGC_BATT_ADJ_INDEX,\
   DBM_HWD_CELL_TX_LIM_TEMP_ADJ_INDEX,\
   DBM_HWD_CELL_TX_LIM_BATT_ADJ_INDEX,\
   DBM_HWD_AMPS_TXAGC_BATT_ADJ_INDEX,\
   DBM_HWD_AMPS_RXAGC_FREQ_ADJ_INDEX,\
   DBM_HWD_AMPS_RXAGC_TEMP_ADJ_INDEX,\
   DBM_HWD_TX_FILTER_PARM_INDEX,\
   DBM_HWD_AFC_TEMP_ADJ_INDEX,\
   DBM_HWD_IAFC_VALUE_INDEX
#endif

/* 
   If you get this #error then the CP data base overlaps onto the RF
   data base in flash. The CP and RF data bases each are allocated 
   4 Kb of a 8 Kb flash sector. If the CP data base grows beyond that
   4 Kb limit then it will overlap the RF data base leading to unpredictable
   behavior.

                                   +------------+
           CP Data Base Start    0 |            |
                                   |            |
                                   |            |
                                   |            |
                                   |            |
                                   +------------+
           RF Data Base Start 4096 |            |
                                   |            |
                                   |            |
                                   |            |
                                   |            |
                              8192 +------------+
*/
#if 0
#if (DBM_CP_FLASH_OFFSET + DBM_CP_CACHE_SIZE >= DBM_RF_CACHE_SIZE)
#error "error occurred"
#endif
#endif

/*------------------------------------------------------------------------
* Define DBM message interface constants and structures
*------------------------------------------------------------------------*/

/* Define DBM msg command Ids */
typedef enum 
{
   DBM_CLEAR_MSG = 0,
   DBM_READ_MSG,
   DBM_WRITE_MSG,
   DBM_CACHE_MSG,
   DBM_FLUSH_MSG,
   DBM_READ_BLK_MSG,
   DBM_WRITE_BLK_MSG,
   DBM_NAM_LOCK_MSG,
   DBM_NAM_LOCK_STATUS_MSG,
   DBM_REG_MSG,
   DBM_DE_REG_MSG
} DbmMsgIdT;

/* Define DBM CP & RF data base number */
typedef enum 
{
   DBM_CP_DATA_BASE  = 0,
   DBM_RF_DATA_BASE
} DbmDataBaseIdT;

/* Define DBM ack types */
typedef enum
{
   DBM_ACK_TYPE = 0x00,
   DBM_NACK_TYPE
} DbmAckTypeT;

/* Define data base address based on segment and offset */
typedef PACKED struct 
{
   uint8         Segment; 
   uint16        Offset;  
} DbmAddressT;

/* Define DBM msg header format */
typedef PACKED struct 
{
   ExeRspMsgT     RspInfo;    
   DbmDataBaseIdT DataBaseId;
} DbmMsgHeaderT;

/* Define DBM clear msg command */
/* Response msg is just msg id  */
typedef PACKED struct 
{
   ExeRspMsgT     RspInfo;    
   DbmDataBaseIdT DataBaseId;
} DbmClearMsgT;

/* Define DBM clear msg response */
typedef PACKED struct 
{
   DbmDataBaseIdT  DataBaseId;
   DbmAckTypeT AckType;
} DbmClearRspMsgT;

/* Define DBM read msg command */
typedef PACKED struct 
{
   ExeRspMsgT     RspInfo;    
   DbmDataBaseIdT DataBaseId;
   DbmAddressT    Address;    
   uint16         NumBytes;  /* Size of data to read in bytes */
} DbmReadMsgT;

/* Define DBM read msg response */
typedef PACKED struct 
{
   DbmDataBaseIdT  DataBaseId;
   DbmAddressT     Address;    
   uint16          NumBytes;  /* Size of data read in bytes */
   uint8           Data[1];
} DbmReadRspMsgT;

/* Define DBM write msg command */
typedef PACKED struct 
{
   ExeRspMsgT     RspInfo;    
   DbmDataBaseIdT DataBaseId;
   DbmAddressT    Address;    
   uint16         NumBytes;  /* Size of data to write in bytes */
   bool           WriteThru;    
   uint8          Data[1];
} DbmWriteMsgT;

/* Define DBM write msg response */
typedef PACKED struct 
{
   DbmDataBaseIdT  DataBaseId;
   DbmAckTypeT     AckType;
} DbmWriteRspMsgT;

/* Define DBM cache msg command */
/* Response msg is just msg id  */
typedef PACKED struct 
{
   ExeRspMsgT     RspInfo;    
   DbmDataBaseIdT DataBaseId;
} DbmCacheMsgT;

/* Define DBM flush msg command */
/* Response msg is just msg id  */
typedef PACKED struct 
{
   ExeRspMsgT     RspInfo;    
   DbmDataBaseIdT DataBaseId;
} DbmFlushMsgT;

/* Define DBM Block data base id's */
typedef enum
{
  DBM_PRL1_DATA_BASE = 1,
  DBM_DS_DATA_BASE,
  DBM_VREC1_DATA_BASE,
  DBM_VREC2_DATA_BASE,
  DBM_VMEM1_DATA_BASE,
  DBM_VMEM2_DATA_BASE,
  DBM_PRL2_DATA_BASE
#ifdef SYS_OPTION_MIP_DMU
  ,
  DBM_DMUPUBKEY_DATA_BASE,
  DBM_DMUPUBKEY2_DATA_BASE,
  DBM_DMUPUBKEY_ORGID_DATA_BASE
#endif  
} DbmBlkDataBaseIdT;

/* Define DBM program msg types */
typedef enum
{
   DBM_PROG_INIT_TYPE = 0x00,
   DBM_PROG_DATA_TYPE,
   DBM_PROG_DWNLD_DONE_TYPE
} DbmProgTypeT;

/* Define DBM block write msg command */
typedef PACKED struct
{
   ExeRspMsgT         RspInfo;    
   DbmProgTypeT       MsgType; 
   DbmBlkDataBaseIdT  DataBaseId;
   uint32             Offset;
   uint16             NumBytes;
   uint32             SeqNum;
   uint8              Data[1];
} DbmBlkWriteMsgT;

/* Define DBM block write msg response */
typedef PACKED struct
{
  DbmBlkDataBaseIdT   DataBaseId;
  DbmAckTypeT         AckType;
} DbmBlkWriteRspMsgT;

/* Define DBM block read msg command */
typedef PACKED struct
{
   ExeRspMsgT         RspInfo;    
   DbmBlkDataBaseIdT  DataBaseId;
   uint32             Offset;
   uint16             NumBytes;
   uint8             *DataP;
} DbmBlkReadMsgT;

/* Define DBM block read msg response */
typedef PACKED struct
{
   DbmBlkDataBaseIdT  DataBaseId;
   uint32             DataBaseSize;
   uint32             Offset;
   uint16             NumBytes;
   uint8             *Data;
} DbmBlkReadRspMsgT;

/* Define DBM lock/unlock msg command */
typedef PACKED struct
{
   ExeRspMsgT RspInfo;    
   bool       Lock;   
   bool       RspRequired;
} DbmLockUnlockMsgT;

/* Define DBM lock/unlock msg response */
typedef PACKED struct
{
   bool       Lock;   
} DbmLockUnlockRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT RspInfo;    
} DbmLockStatusMsgT;

/* Define the DBM Reg Msg Data structure */
typedef PACKED struct 
{
   DbmReadMsgT   ReadInfo;  /* Size of data to read in bytes */
} DbmRegMsgT;

/* Define the DBM De-Reg Msg Data structure */
typedef PACKED struct
{
   ExeTaskIdT     TaskId;
   DbmDataBaseIdT DataBaseId;
   DbmAddressT    Address;

}DbmDeRegMsgT;

#ifdef SNR_ESN_PROTECTION
typedef PACKED struct
{
   uint32   NewESN;
   uint32   PwdForNewESN;
} DbmUpdateEsnMsgT;
#endif

/*------------------------------------------------------------------------
 *  Define Global Function Prototypes
 *------------------------------------------------------------------------*/
extern bool  DbmGetNamLockStatus(void);


#endif

