/******************************************************************************
 * Power Task (pwr)
 * Design and coding by Svend Kristian Lindholm, skl@ti.com
 *
 * PWR MMI Interface
 *
 * $Id: pwr_api.h 1.1 Wed, 20 Aug 2003 10:22:37 +0200 skl $
 *
 ******************************************************************************/

#ifndef __LCC_API_H__
#define __LCC_API_H__

#include "rv/rv_general.h"

#ifndef BASIC_TYPES
#define BASIC_TYPES
typedef signed   char  int8;
typedef unsigned char  uint8;
typedef signed   short int16;
typedef unsigned short uint16;
typedef signed   int   int32;
typedef unsigned int   uint32;
#endif



// INFO Indication mail SENT to the MMI
struct mmi_info_ind_s {
     T_RV_HDR header;
     int      Vbat;  // Battery Voltage
     int      Tbat;  // Battery Temperature
     int      Cbat;  // Battery Capacity
     int      cause; // Reason for the charge fail
};

typedef struct mmi_info_ind_s  T_PWR_MMI_INFO_IND_EVENT;

// Corresponds to /mmi/pwr/bsie.cfg
typedef struct
{
    uint16    repetition;     // The repetition interval for sending MMI info events
} T_PWR_MMI_CFG_BLOCK;

// Events send from the PWR module
// See RIV461
 enum {
    MMI_CHG_PLUG_IND     = 1,
    MMI_CHG_UNPLUG_IND   = 2,
    MMI_CHG_START_IND    = 3,
    MMI_CHG_STOP_IND     = 4,
    MMI_CHG_UNKNOWN_IND  = 5,
    MMI_BAT_UNKNOWN_IND  = 6,
    MMI_CHG_FAILED_IND   = 7,
    MMI_BAT_SUPERVISION_INFO_IND = 10,
    MMI_USB_PLUG_IND  =11,
    MMI_USB_UNPLUG_IND  =12
};

typedef uint8 PWR_MMI_MESSAGES;

// Only one instance (e.g. the MMI) can register
void pwr_register (T_RV_RETURN_PATH *return_path, void *ptr);
uint8  lcc_pwr_interface(uint8 command);
uint16 lcc_get_batteryVotage(void);
uint16 lcc_get_chgPlugStatus(void);
uint16 lcc_get_usbPlugStatus(void);
int8 lcc_get_state(void);
void lcc_get_values(uint16 *lcc_info);
void lcc_get_timerStatus(int32 *timer_sts);
#endif


