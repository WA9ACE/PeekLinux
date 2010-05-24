#ifndef _HWDERROR_H_
#define _HWDERROR_H_


/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Defines and Macros
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Typedefs
----------------------------------------------------------------------------*/

/* Hardware Driver Errors */
typedef enum
{
   HWD_ERR_TST_UNKNOWN_CMD,       /* 0x00 Test task received unknown command          */
   HWD_ERR_TST_UNKNOWN_EVENT,     /* 0x01 Test task received unknown event            */
   HWD_ERR_TST_UNKNOWN_SIGNAL,    /* 0x02 Test task received unknown signal           */
   HWD_ERR_TST_INVALID_PLL,       /* 0x03 Invalid PLL selection                       */
   HWD_ERR_TST_INVALID_DAC,       /* 0x04 Invalid DAC index                           */
   HWD_ERR_SER_LOAD_TIMEOUT,      /* 0x05 Serializer immediate load time out          */
   HWD_ERR_MS_PROG_TYPE,          /* 0x06 Invalid mixed signal device                 */
   HWD_ERR_AUX_IMMED_WRITE,       /* 0x07 Aux immediate interrupt ready timeout       */
   HWD_ERR_AUX_TIMED_WRITE,       /* 0x08 Aux timed interrupt ready timeout error     */
   HWD_ERR_PDM_TIMED_WRITE,       /* 0x09 PDM timed interrupt ready timeout error     */
   HWD_ERR_PDM_IMMED_WRITE,       /* 0x0A PDM immediate interrupt ready timeout error */
   HWD_ERR_CHAN_INVALID,          /* 0x0B Invalid Rx channel type                     */
   HWD_ERR_STARTUP_BAD_MSG,       /* 0x0C Invalid mail message at startup time        */
   HWD_ERR_STARTUP_BAD_SIGNAL,    /* 0x0D Invalid signal at startup time              */
   HWD_ERR_STARTUP_TIMEOUT,       /* 0x0E Timeout condition at startup time           */
   HWD_ERR_STARTUP_CAL_PARMS,     /* 0x0F not all cal parms were received             */
   HWD_ERR_STARTUP_SIGNAL,        /* 0x10 Startup signal not received                 */
   HWD_ERR_FLASH_ERASE,           /* 0x11 Flash memory erase error                    */
   HWD_ERR_FLASH_PROGRAM,         /* 0x12 Flash memory program error                  */
   HWD_ERR_FLASH_SECTOR_PROTECT,  /* 0x13 Flash sector is protected                   */
   HWD_ERR_GPIO_ILLEGAL,          /* 0x14 Illegal GPIO selected error                 */
   HWD_ERR_REQUEST_ILLEGAL,       /* 0x15 Illegal request...                          */
   HWD_ERR_CALIB_PARM_INVALID,    /* 0x16 calibration parameter exceeds limits        */
   HWD_ERR_ADC_TIMEOUT,           /* 0x17 ADC measurement conversion timeout          */
   HWD_ERR_BAD_VOLUME_PTR,        /* 0x18 Bad function pointer in Volume Set process  */
   HWD_ERR_FILT_BW_CAL,           /* 0x19 Invalid signal while calibrating Filter BW  */
   HWD_ERR_IQ_GAIN_CAL,           /* 0x1A Invalid signal while calibrating IQ gain imbalance */
   HWD_ERR_TX_DAC_OFST_CAL,       /* 0x1B Tx DAC DC offset calibration failure        */
   HWD_ERR_RX_ADC_OFST_CAL,       /* 0x1C Rx ADC DC offset calibration failure        */
   HWD_KEY_SCAN_ERR,              /* 0x1D Key Scan error                              */
   HWD_ERR_PLL_LOCK_LOST,         /* 0x1E RF PLL Lock Lost                            */
   HWD_ERR_DSPV_IS_CLOCKED_OFF,   /* 0x1F Dspv is clocked off                         */
   HWD_ERR_EDAI_ALREADY_ENABLED,  /* 0x20 EDAI mode is already enabled                */
   HWD_ERR_EDAI_ALREADY_DISABLED, /* 0x21 EDAI mode is already disabled               */
   HWD_ERR_READ_TEMPERATURE,      /* 0x22 Read wrong temperature from Aux Adc         */
   HWD_ERR_READ_BATTERY_VOLTAGE,  /* 0x23 Read wrong battery voltage from Aux Adc     */
   HWD_ERR_RFFUNCPTR_NULL,        /* 0x24 RF Function Pointer Table Has Null Entry    */
   HWD_ERR_TUNECNT_READ_FAILURE,  /* 0x25 Sigma-Delta Tune Count read failure         */ 
   HWD_ERR_TX_DC_OFST_CAL_IDIFF_FAILURE, /* 0x26 Tx DC offset cal I-channel failure   */
   HWD_ERR_TX_DC_OFST_CAL_QDIFF_FAILURE,  /* 0x27 Tx DC offset cal Q-channel failure  */
   HWD_ERR_AUDIO_MODE_UNSUPPORTED,/* 0x28 Audio mode requested is not supported       */
   HWD_ERR_GAIN_FIQ_CONTENTION,   /* 0x29 Gain State FIQ Resource contention error    */
   HWD_ERR_USB_CFGCORE_ERR,       /* 0x2A core gonfig did not complete                */
   HWD_ERR_DAC_DRIVER_FAILURE,    /* 0x2B External Audio DAC Driver error             */
   HWD_ERR_RF_TABLE_FORMAT_ERR,   /* 0x2C RF mapping assignments format error         */        
   HWD_ERR_AUDIO_MODE_NOT_SUPPORTED,  /* 0x2D Audio mode not supported                */

   /* MIDI ringer errors */
   HWD_ERR_MIDI_FILE_EMPTY,                     /* 0x2E */
   HWD_ERR_MIDI_NOT_INITIALIZED,                /* 0x2F */
   HWD_ERR_MIDI_BUFF_EMPTY_SIG_WHILE_IDLE,      /* 0x30 */
   HWD_ERR_MIDI_DRUM_NOTE_INVALID,              /* 0x31 */
   HWD_ERR_MIDI_PATCH_IDX_INVALID,              /* 0x32 */
   HWD_ERR_MIDI_FILE_IDX_INVALID,               /* 0x33 */
   HWD_ERR_MIDI_CMD_INV_WHILE_RNGR_ACTIVE,      /* 0x34 */
   HWD_ERR_MIDI_CMD_INV_IN_CURR_STATE,          /* 0x35 */
   HWD_ERR_MIDI_FILE_OPEN_ERR,                  /* 0x36 */
   HWD_ERR_MIDI_ENABLE_INV_IN_CURR_STATE,       /* 0x37 */
   HWD_ERR_MIDI_DSPV_INIT_FAILED,               /* 0x38 */
   HWD_ERR_MIDI_TONE_FREQ_INVALID,              /* 0x39 */
   HWD_ERR_MIDI_BANK_SELECT_NOT_SUPPORTED,      /* 0x3A */
   HWD_ERR_MIDI_LOADING_NON_EMPTY_BUFF,         /* 0x3B */

   /* Multimedia Applications errors */
   HWD_ERR_MMAPPS_INVAL_MODE_RSP_IN_CURR_STATE, /* 0x3C */ 
   HWD_ERR_MMAPPS_INVAL_MUSIC_PARAMS,           /* 0x3D */
   HWD_ERR_MMAPPS_INVAL_MUSIC_TYPE,             /* 0x3E */
   HWD_ERR_MMAPPS_INVAL_TEST_FILE_SIZE,         /* 0x3F */
   HWD_ERR_MMAPPS_READ_REQ_WHILE_NOT_ACTIVE,    /* 0x40 */
   HWD_ERR_MMAPPS_WRITE_REQ_WHILE_NOT_ACTIVE,   /* 0x41 */
   HWD_ERR_MMAPPS_FILE_INFO_REQ_WHILE_IDLE,     /* 0x42 */
   HWD_ERR_MMAPPS_MP3_STATUS_WHILE_IDLE,        /* 0x43 */
   HWD_ERR_MMAPPS_WRONG_STATE_FOR_FAST_FWD,     /* 0x44 */
   HWD_ERR_MMAPPS_WRONG_STATE_FOR_REWIND,       /* 0x45 */
   HWD_ERR_MMAPPS_WRONG_STATE_FOR_SUSPEND,      /* 0x46 */
   HWD_ERR_MMAPPS_WRONG_STATE_FOR_RESUME,       /* 0x47 */
   HWD_ERR_MMAPPS_INVAL_APP_CHAN_MSG_ID,        /* 0x48 */
   HWD_ERR_MMAPPS_INVAL_WRITE_REQ_APP_ID,       /* 0x49 */
   HWD_ERR_MMAPPS_APP_ID_ALREADY_ACTIVE,        /* 0x4A */
   HWD_ERR_MMAPPS_MUSIC_APP_ALREADY_ACTIVE,     /* 0x4B */
   HWD_ERR_MMAPPS_MAX_APPS_ALREADY_ACTIVE,      /* 0x4C */
   HWD_ERR_MMAPPS_INVALID_INPUT_FILE_ID,        /* 0x4D */
   HWD_ERR_MMAPPS_INVALID_INPUT_APP_ID,         /* 0x4E */
   HWD_ERR_MMAPPS_FILE_ID_APP_ID_MISMATCH,      /* 0x4F */
   HWD_ERR_MMAPPS_INVALID_NUM_APP_MSGS,         /* 0x50 */
   HWD_ERR_MMAPPS_INVALID_FAST_FWD_PARMS,       /* 0x51 */
   HWD_ERR_MMAPPS_INVALID_REWIND_PARMS,         /* 0x52 */
   HWD_ERR_MMAPPS_CANT_FASTFWD_WITHOUT_STATUS,  /* 0x53 */
   HWD_ERR_MMAPPS_CANT_REWIND_WITHOUT_STATUS,   /* 0x54 */
   HWD_ERR_MMAPPS_UNEXPECT_POS_ADJUST_TIMER,    /* 0x55 */
   HWD_ERR_MMAPPS_FILE_SEEK_FAIL,               /* 0x56 */
   HWD_ERR_MMAPPS_FILE_READ_FAIL,               /* 0x57 */
   HWD_ERR_MMAPPS_MODE_START_RESP_ERR,          /* 0x58 */
   HWD_ERR_MMAPPS_MODE_STOP_RESP_ERR,            /* 0x59 */

   HWD_ERR_LOUDSPEAKER_NOT_SUPPORTED,           /* 0x5a louspeaker not supported on this board */
   HWD_ERR_I2C_NOT_RESPONDING_ERR,              /* 0x5b */
   HWD_ERR_AUDIO_SET_MODE_SEM_TIMEOUT_ERR,      /* 0x5c */
   HWD_ERR_AUDIO_SET_VOL_SEM_TIMEOUT_ERR,       /* 0x5d */
   HWD_ERR_AUDIO_SET_PATH_SEM_TIMEOUT_ERR,      /* 0x5e */
   HWD_ERR_SHRD_MEM_LPBK_NOT_SUPPORTED_ERR,     /* 0x5f */
   HWD_ERR_MIDI_EVENT_DATA_TOO_LARGE,           /* 0x60 */
   HWD_ERR_IMELODY_EVENT_INVALID,               /* 0x61 */
   
   HWD_ERR_PSAVING_MODE_CFG_INVALID,            /* 0x92 */
   
   HWD_ERR_MIDI_INVALID_PITCH_RANGE = 0x97,     /* 0x97 */
   HWD_ERR_CMF_UNEXPECTED_SUBCHUNK_TYPE,        /* 0x98 */
   HWD_ERR_CMF_TOO_MANY_JUMP_POINTS,            /* 0x99 */
   HWD_ERR_CMF_JUMP_CMD_WITH_ZERO_JUMPS,        /* 0x9A */
   HWD_ERR_CMF_NO_EMPTY_NOTEOFF_TBL_ENTRIES,    /* 0x9B */
   HWD_ERR_CMF_PITCH_BEND_OVERFLOW,             /* 0x9C */
   HWD_ERR_CMF_DLS_FEATURE_NOT_SUPPORTED,       /* 0x9D */   

   HWD_ERR_MMAPPS_TX_BUFFER_OVERFLOW,           /* 0x9E */
   HWD_ERR_MMAPPS_MISALIGNED_CMD_ARGS_STRUCT,   /* 0x9F */
   HWD_ERR_MMAPPS_INVALID_SRVC_OPT,             /* 0xA0 */
   HWD_ERR_MMAPPS_VOCODER_DNLD_REQ_DURING_VOICE,/* 0xA1 */
   HWD_ERR_MMAPPS_APP_START_TIMEOUT,            /* 0xA2 */
   HWD_ERR_MMAPPS_CANT_DO_APP_DURING_DOWNLOAD   /* 0xA3 */

} HwdErrorT;

/*----------------------------------------------------------------------------
 Global Data
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Function Prototypes
----------------------------------------------------------------------------*/




#endif
