/******************************************************************************
 * Enhanced TestMode  (ETM) 
 * @file              etm.h
 *
 * Design and coding  Mads Meisner-Jensen, (mmj@ti.com)
 *                    Kim T. Petersn,      (ktp@ti.com)
 *
 *
 *
 ******************************************************************************/

#ifndef _ETM_H_
#define _ETM_H_


/******************************************************************************
 * Prototypes and Globals
 ******************************************************************************/

// Max size of downlink packet payload
#define ETM_PAYLOAD_MAX 256
#define TABLE_END -1

struct ioformat_s {
    const short index;  // index for identifier
    const char *name;   // parameter
    const char *format; // parameter data format
};


/******************************************************************************
 * Global Definitions
 ******************************************************************************/

// Enhanced TestMode Module IDentifiers
enum {
    ETM_TM3        = 0x00, // Use of old TM3 protocol
    ETM_CORE       = 0x01,
    ETM_TMT        = 0x02, // pseudo module
    ETM_SH         = 0x03, // pseudo module
    ETM_TM3_MISC   = 0x04, // Pseudo module - Target side   
    ETM_RF         = 0x05, 
    ETM_IMEI       = 0x06,
    ETM_FFS2       = 0x07,
    ETM_AUDIO      = 0x08,
    ETM_TPU        = 0x09, // reserved
    ETM_PWR        = 0x0A,
    ETM_BT         = 0x0B,
    ETM_L23        = 0x0C, // reserved
    ETM_GPS        = 0x0D,
    ETM_LFS        = 0x0E, // reserved
    ETM_LITE       = 0x0F, // use for tracing
    ETM_BOOT       = 0x11, // To use for ready signaling
	
    ETM_FFS1       = 0x70,

	ETM_I2CDRV     = 0x91,

    ETM_TEST       = 0xAA,
    // ETM-Lite modules
    ETMLITE_CORE    = 0xB0,
    ETMLITE_LED     = 0xB1,
    ETMLITE_LCD     = 0xB2,
    ETMLITE_NOR     = 0xB3,
    ETMLITE_NAND    = 0xB4,
    ETMLITE_SDRAM   = 0xB5,
    ETMLITE_AUDIO   = 0xB6,
    ETMLITE_USB     = 0xB7,
    ETMLITE_BT      = 0xB8,
    ETMLITE_MMC     = 0xB9,
    ETMLITE_IRDA    = 0xBA,
    ETMLITE_DBG_INT = 0xBB,
    ETMLITE_CHARGER = 0xBD,
    ETMLITE_DIP     = 0xBE,
    ETMLITE_KEYPAD  = 0xBF,
    ETMLITE_HDQ     = 0xC0,
    ETMLITE_SIM     = 0xC1,
    ETMLITE_AGPS    = 0xC2,
    ETMLITE_DEBUGCONN = 0xC3,
    ETMLITE_MDOC    = 0xC4,
    ETMLITE_CAMERA  = 0xC5
};

// Module error number bases
enum {
    ETM_CORE_ERROR   = -100 * ETM_CORE,
    ETM_TMT_ERROR    = -100 * ETM_TMT,
    ETM_SH_ERROR     = -100 * ETM_SH,
    ETM_RF_ERROR     = -100 * ETM_RF,
	ETM_FFS_ERROR    = -100 * ETM_FFS2, 
    ETM_FFS2_ERROR   = -100 * ETM_FFS2,
    ETM_AUDIO_ERROR  = -100 * ETM_AUDIO,
    ETM_TPU_ERROR    = -100 * ETM_TPU,
    ETM_IMEI_ERROR   = -100 * ETM_IMEI,
    ETM_PWR_ERROR    = -100 * ETM_PWR,
    ETM_BT_ERROR     = -100 * ETM_BT,
    ETM_GPS_ERROR    = -100 * ETM_GPS,
    ETM_LFS_ERROR    = -100 * ETM_LFS,

    ETMLITE_ERROR = -100*ETMLITE_LED,
    ETM_I2CDRV_ERROR   = -100 * ETM_I2CDRV,

    ETM_OS_ERROR     = -100 * 1000 // OS Error (specially handled!)
};

// Module private (normally target-side) errors are in the range: [ -2..-47]
// Common (normally target-side) errors are in the range:         [-48..-63]
// Common (normally PC-side) errors are in the range:             [-64..-89]
// Module private (normally PC-side) errors are in the range:     [-90..-99]

enum ETMCommonTargetErrors {
    ETM_OK               =   0,   // Ok
    ETM_FINISHED         =  -1,   // Previously started operation has finished.

    // Errors related to the Riv env.
    ETM_RV_FATAL         = -48,   // Fatal error in RIV environment eg. memory error 
    ETM_RV_NOT_SUPPORTED = -49,   // Funtionality not supported by RIV

    // Error related to L1 Test mode (FIXME: move to rf module)
    ETM_L1TESTMODE       = -50,   // Layer 1 is not in test mode

    ETM_NOT_USED1        = -51,   //
    ETM_NOT_USED2        = -52,   //
    ETM_NOT_USED3        = -53,   //
    ETM_NOT_USED4        = -54,   //

    // ETM Common Target Errors
    ETM_MESSAGE          = -55,   // Received unknown message
    ETM_NOMEM            = -56,   // Out of memory
    ETM_AGAIN            = -57,   // Not ready, try again later
    ETM_BADOP            = -58,   // Operation not possible in current context
    ETM_INVAL            = -59,   // Invalid parameter/argument - Target
    ETM_NOSYS            = -60,   // Module or function not present
    ETM_FATAL            = -61,   // System fatal error
    ETM_PACKET           = -62,   // Packet error (checksum or other)
    ETM_OK_MORE          = -63    // Ok, more data coming
};


// ETM Common PC-side Errors. The error codes from this section are not to
// be used directly but should be used like 'ETM_XXX_ERROR + ETM_INVALID',
// where XXX is the module's name
enum ETMCommonPCErrors {
    ETM_INTERNAL    = -64, // Fatal (internal) unrecoverable error
    ETM_TOOFEWARGS  = -65, // Too few arguments

    ETM_USE         = -67, // Failed to load module
    ETM_USEAGAIN    = -68, // Module already loaded
    ETM_USEVERSION  = -69, // Module version incompatible with Shell version

    ETM_INVALID     = -70, // Invalid parameter/argument - Host
    ETM_MEMORY      = -71, // Out of memory
    ETM_BUFFER      = -72, // Buffer too small (maybe internal buffer)
    ETM_BADDATA     = -73, // Bad/unexpected data in uplink packet
    ETM_NOSUPPORT   = -74, // Not supported 
    ETM_BADVALUE    = -75, // Bad argument/parameter value

    ETM_FILE_IO     = -76, // File I/O error (file not found?)
    ETM_HOST_FIO    = -78  // Host system file I/O error
};


/******************************************************************************
 * Macros
 ******************************************************************************/

#define if_error_return(error) if (error < 0) return error

// Keyword to use for exporting a DLL function
#define FEXPORT __stdcall

// Keyword to use for defining a DLL variable as private
#define PRIVATE

// Keyword to use for defining a DLL variable as private
#define PUBLIC


/******************************************************************************
 * Target and host side (tmsh) tracing
 ******************************************************************************/

enum ETMTgTraceMask {
    TgTrTest      = 0x00000001,
    TgTrEtmLow    = 0x00000002,
    TgTrEtmMed    = 0x00000004,
    TgTrEtmHigh   = 0x00000008,

    TgTrCore      = 0x00000100,
    TgTrAudio     = 0x00000200,
    TgTrFfs       = 0x00000400, 
    TgTrRf        = 0x00000800,

    TgTrFatal     = (int)0x80000000,
    TgTrAll       = (int)0xFFFFFFFF
};

// Trace module IDs
// IMPORTANT: value can not be larger that (15 << 24)
enum { 
    TrCore    = ETM_CORE   << 24,
    TrTMT     = ETM_TMT    << 24,
    TrSH      = ETM_SH     << 24,
    TrRF      = ETM_RF     << 24,
    TrIMEI    = ETM_IMEI   << 24,
    TrFFS     = ETM_FFS2   << 24,
    TrAUDIO   = ETM_AUDIO  << 24,
    TrTPU     = ETM_TPU    << 24,
    TrPWR     = ETM_PWR    << 24,
    TrBT      = ETM_BT     << 24,
    TrLFS     = ETM_LFS    << 24,
    TrGPS     = ETM_GPS    << 24,

    // Special trace IDs that cannot be derived from the module IDs
    TrTEST    = 14         << 24,
    TrETMLITE = ETM_LITE   << 24
};


/******************************************************************************
 * Types
 ******************************************************************************/

#ifndef BASIC_TYPES
#define BASIC_TYPES
typedef signed   char  int8_t;
typedef unsigned char  uint8_t;
typedef signed   short int16_t;
typedef unsigned short uint16_t;
typedef signed   int   int32_t;
typedef unsigned int   uint32_t;

typedef signed   char  int8;
typedef unsigned char  uint8;
typedef signed   short int16;
typedef unsigned short uint16;
typedef signed   int   int32;
typedef unsigned int   uint32;
#endif


/******************************************************************************
 * Supported hardware and software version - read out from Target
 *****************************************************************************/
// This section should be moved to at core header file.
enum HW_SW_revisions_fids
{
    SW_REV_ETM_TASK            = 0x01,
    SW_REV_ETM_API             = 0x02,
    SW_REV_ETM_CORE            = 0x10, 
    SW_REV_ETM_AUDIO           = 0x11,
    SW_REV_ETM_RF              = 0x12,
    SW_REV_ETM_FFS             = 0x13,
    SW_REV_ETM_PWR             = 0x14,
    SW_REV_ETM_BT              = 0x15,
    SW_REV_xx1                 = 0x16, // Not in use
    SW_REV_xx2                 = 0x18, // Not in use
    SW_REV_xx3                 = 0x19, // Not in use
    SW_MCU_TCS_PROGRAM_RELEASE = 0xC0,
    SW_MCU_TCS_OFFICIAL        = 0xC1,
    SW_MCU_TCS_INTERNAL        = 0xC2,
    SW_MCU_TM_VERSION          = 0xC3,
    SW_DSP_CODE_VERSION        = 0xD0,
    SW_DSP_PATCH_VERSION       = 0xD1,
    HW_REV_1,
    HW_REV_2,
    HW_REV_3,
    HW_REV_4,
    HW_REV_5,
    HW_REV_6,
    HW_REV_7,
    HW_REV_8
};


#endif // _ETM_H_
