#ifndef MONERRS_H
#define MONERRS_H


/*------------------------------------------------------------------------
*  The following definitions are fault ids for MonFault routine.
*-----------------------------------------------------------------------*/

typedef enum 
{
    MON_MSG_ID_ERR                        = 0,
    MON_SPY_ID_ERR                        = 1,
    MON_SPY_LEN_ERR                       = 2,
    MON_TRACE_ID_ERR                      = 3,
    MON_TRACE_ARGS_ERR                    = 4,
    MON_BB_MODE_ERR                       = 5,
    MON_PRINTF_ARGS_ERR                   = 6,
    MON_PEEK_LEN_ERR                      = 7,
    MON_POKE_LEN_ERR                      = 8,
    MON_SEQ_NUM_ERR                       = 9,
    MON_FLASH_SECTION_ERR                 = 10,
    MON_CHECKSUM_ERR                      = 11,
    MON_VTST_MODE_ERR                     = 12,
    MON_ASIC_REV_ERR                      = 13,
    MON_LOOPBACK_ERR                      = 14,
    MON_RT_LIBRARY_ERR                    = 15,
    MON_CANT_JUMP_WITH_WD_ENABLED_ERR     = 16,

    MON_DSP_DNLOAD_FLASH_FILE_ID_ERR      = 20,
    MON_DSP_DNLOAD_ASIC_REVISION_ERR      = 21,
    MON_DSP_DNLOAD_ASIC_VERSION_ERR       = 22,
    MON_DSP_DNLOAD_TOO_MANY_PATCHES_ERR   = 23,
    MON_DSP_DNLOAD_NO_PATCH_ERR           = 24,
    MON_DSP_DNLOAD_TIMEOUT_ERR            = 25,
    MON_DSP_DNLOAD_CHECKSUM_ERR           = 26,
    MON_DSP_DNLOAD_COMPLETE_ERR           = 27,
    MON_DSP_DNLOAD_PATCHSET_ID_ERR        = 28,

    MON_SYSREL_ASICVER_ERR                = 29,
    MON_SYSREL_ASICTYPE_ERR               = 30,
    MON_SYSREL_CPVER_ERR                  = 31,
    MON_SYSREL_DSPVVER_ERR                = 32,
    MON_SYSREL_DSPMVER_ERR                = 33,

    MON_IRAM_WRITE_FAULT_ERR              = 34,

    MON_TEST_NOT_SUPPORTED_ERR            = 35,
    MON_TEST_ALREADY_IN_PROGRESS_ERR      = 36,
    MON_TURN_OFF_DSPV_BOOT_SWITCH_ERR     = 37,
    MON_INVALID_DSP_TEST_PATCH_ERR        = 38,
    MON_UNEXPECTED_DSPV_RESET_ERR         = 39

} MonErrsT;

typedef enum
{
   MON_ARM_RESET_EXCEPTION,
   MON_ARM_UNDEFINEDINSTR_EXCEPTION,
   MON_ARM_SWI_EXCEPTION,
   MON_ARM_PREFETCHABORT_EXCEPTION,
   MON_ARM_DATAABORT_EXCEPTION,
   MON_ARM_RESERVED_EXCEPTION
}MonExceptionTypeT;



#endif
