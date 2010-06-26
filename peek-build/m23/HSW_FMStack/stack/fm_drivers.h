#ifndef __FMS_DRIVERS_H
#define __FMS_DRIVERS_H

#include "fm_types.h"
#include "fm_utils.h"

/* struct for fm driver parameters */
typedef struct {
    TIFM_U16		gen_int_mask;
    TIFM_U16		opHandler_int_mask;
    
} FmDriver;


/* FM commands opcodes */

#define STEREO_GET					0x00
#define RSSI_LEVEL_GET				0x01
#define IF_COUNT_GET				0x02
#define FLAG_GET					0x03
#define RDS_SYNC_GET				0x04
#define RDS_DATA_GET				0x05
#define FREQ_SET_GET				0x0a
#define AF_FREQ_SET_GET				0x0b
#define MOST_MODE_SET_GET			0x0c
#define MOST_BLEND_SET_GET			0x0d
#define DEMPH_MODE_SET_GET			0x0e
#define SEARCH_LVL_SET_GET			0x0f
#define BAND_SET_GET				0x10
#define MUTE_STATUS_SET_GET			0x11
#define RDS_PAUSE_LVL_SET_GET		0x12
#define RDS_PAUSE_DUR_SET_GET		0x13
#define RDS_MEM_SET_GET				0x14
#define RDS_BLK_B_SET_GET			0x15
#define RDS_MSK_B_SET_GET			0x16
#define RDS_PI_MASK_SET_GET			0x17
#define RDS_PI_SET_GET				0x18
#define RDS_SYSTEM_SET_GET			0x19
#define INT_MASK_SET_GET			0x1a
#define SEARCH_DIR_SET_GET			0x1b
#define VOLUME_SET_GET				0x1c
#define AUDIO_ENABLE_SET_GET		0x1d
#define I2S_CLOCK_CONFIG_SET_GET	0x1e
#define I2S_MODE_CONFIG_SET_GET		0x1f
#define POWER_SET_GET				0x20
#define INTX_CONFIG_SET_GET			0x21
#define PULL_EN_SET_GET				0x22
#define HILO_SET_GET				0x23
#define PCE_GET						0x28
#define FIRM_VER_GET				0x29
#define ASIC_VER_GET				0x2a
#define ASIC_ID_GET					0x2b
#define MAN_ID_GET					0x2c
#define TUNER_MODE_SET				0x2d
#define STOP_SEARCH					0x2e
#define RDS_CNTRL_SET				0x2f
#define HARDWARE_REG_SET_GET		0x64
#define CODE_DOWNLOAD				0x65
#define RESET						0x66


/* commands parameters */
#define BASE_FREQ_US_EUROPE		87500
#define BASE_FREQ_JAPAN			76000
#define LAST_FREQ_US_EUROPE		108000
#define LAST_FREQ_JAPAN			90000
#define LAST_INDEX_US_EUROPE	((LAST_FREQ_US_EUROPE - BASE_FREQ_US_EUROPE)/50)
#define LAST_INDEX_JAPAN		((LAST_FREQ_JAPAN - BASE_FREQ_JAPAN)/50)

#define TUNER_MODE_STOP_SEARCH		0
#define TUNER_MODE_PRESET_MODE		1
#define TUNER_MODE_SEARCH_MODE		2
#define TUNER_MODE_AF_JUMP_MODE		3

#define MUTE_UNMUTE_MODE				0x00
#define MUTE_RF_DEP_MODE				0x01
#define MUTE_AC_MUTE_MODE				0x02
#define MUTE_HARD_MUTE_LEFT_MODE		0x04
#define MUTE_HARD_MUTE_RIGHT_MODE		0x08
#define MUTE_SOFT_MUTE_FORCE_MODE		0x10

#define POWER_SET_FM_ON_RDS_OFF		0x01
#define POWER_SET_FM_AND_RDS_ON		0x03
#define POWER_SET_FM_AND_RDS_OFF	0x00

#define FM_POWER_MODE_DISABLE		0
#define FM_POWER_MODE_ENABLE		1

#define RDS_FLUSH_FIFO				1
#define RDS_THRESHOLD				64
#define RDS_THRESHOLD_MAX			85

#define MASK_FR		0x0001	/* Tuning Operation Ended						*/
#define MASK_BL		0x0002	/* Band limit was reached during search			*/
#define MASK_RDS	0x0004	/* RDS data threshold reached in FIFO buffer	*/
#define MASK_BBLK	0x0008	/* RDS B block match condition occurred			*/
#define MASK_LSYNC	0x0010	/* RDS sync was lost							*/
#define MASK_LEV	0x0020	/* RSSI level has fallen below the threshold configured by SEARCH_LVL_SET	*/
#define MASK_IFFR	0x0040	/* Received signal frequency is out of range	*/
#define MASK_PI		0x0080	/* RDS PI match occurred						*/
#define MASK_PD		0x0100	/* Audio pause detect occurred					*/
#define MASK_STIC	0x0200	/* Stereo indication changed					*/
#define MASK_MAL	0x0400	/* Hardware malfunction							*/

/* functions prototypes */

TIFM_U16 FmFreqToIndex(TIFM_U32 freq);
TIFM_U32 FmIndexToFreq(TIFM_U16 index);
void fmDriverInit(void);
TIFM_U16 findNextIndex(TIFM_U8 dir, TIFM_U16 index);

#endif
