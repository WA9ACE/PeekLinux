/**********************************************************************************
drp_calib_main.c
-- Copyright (c) 2004, Texas Instruments, Inc.
-- Author: Sumeer Bhatara, Shrinivas Gadkari and Pradeep P

Calibration routines file.
***********************************************************************************
                               Change History
************************************************************************************

Version     Date           Author(s)        Comments
-------------------------------------------------------------------------------------
Ver 0.0     25 Nov 2004    SG, SB and PP    Locosto version based on DRP ver 0.23
Ver 1.01    29 Dec 2004    SG               Based on DRP Ref SW 1.0, with addresses
                                            of srm variables and tables aligned to
                                            locosto scripts version 129.03
Ver 2.00   5 July 2005       Locosto Test SW release
************************************************************************************/
#define __DRP_FILE__

#include <stdio.h>
#include <math.h>
#if (DRP_FW_BUILD==1)
#include "l1_drp_if.h"
#endif
#include "drp_defines.h"
#include "drp_extern_dependencies.h"
#if (DRP_FW_BUILD == 0)
  #define GENERAL_H
#endif
#include "drp_api.h"
#include "drp_calib_main.h"
#if (DRP_FW_BUILD==1)
#include "drp_main.h"
#endif

#if DRP_LOCOSTO_DEBUG
  extern UINT16 *drp_debug_ptr;
#endif

#if _DEBUG
SINT16 measured_temp[NUM_AFE_GAINS];
#endif

#define FLYBACK_DEBUG 0

#if DRP_BURNIN
#define C_MAP_DPLL_BASE               0xFFFF9800L   // CS19 : DPLL register
#define C_MAP_CLKM_BASE               0xFFFFFD00L   // CS31 : CLOCKM registers
#define CLKM_CNTL_ARM_CLK             (C_MAP_CLKM_BASE + 0x00)
#define MASK_CLKIN                    0x0006
#define DPLL_LOCK                     0x0001     // Mask of DPLL lock status
#define DPLL_BYPASS_DIV               0x000C     // Mask of bypass mode configuration
#define DPLL_PLL_DIV                  0x0060     // Mask of division factor configuration
#define DPLL_PLL_MULT                 0x0F80     // Mask of multiply factor configuration
#define DPLL_PLL_DIV_OFFSET           5     // Offset of division bits configuration
#define DPLL_PLL_MULT_OFFSET          7     // Offset of multiply bits configuration
#define DPLL_READ_DPLL_DIV            ( ((* (volatile SYS_UWORD16 *) C_MAP_DPLL_BASE) & DPLL_PLL_DIV) >> DPLL_PLL_DIV_OFFSET)
#define DPLL_READ_DPLL_MUL            ( ((* (volatile SYS_UWORD16 *) C_MAP_DPLL_BASE) & DPLL_PLL_MULT)>> DPLL_PLL_MULT_OFFSET)
#define CLKM_SWITCH_FREQ              0x2000        // Mask to Switch Arm frequency
static SYS_UWORD32 ratio_wait_loop = 0;
#endif

/*******************************************************************************
                              INITIALIZATION
*******************************************************************************/
#pragma DATA_SECTION(drp_sw_data_init, ".drp_flash_data");
const T_DRP_SW_DATA drp_sw_data_init =
                    {
                      //  UINT16 length;              // length of the structure in bytes
                      (UINT16) sizeof(T_DRP_CALIB),
                      0x0000,                         //dummy1
                      //T_DRP_CALIB calib;
                      {
                        //T_DRP_TXRXCOMMON_CALIB txrx_common
                        {
                          0x0567,       //perinv_value
                          0x1300,       //dlo_at_value
                          //UINT16 pvt_value[2*NUM_BANDS];
                          {
                            0x0022, 0x0000, 0x0065, 0x0008,
                            0x0009, 0x0019, 0x007E, 0x001B
                          },
                          //UINT16 kdco_inv[2*NUM_BANDS];
                          {
                            0x0715, 0x0876, 0x07A3, 0x0685,
                            0x0627, 0x0744, 0x06AC, 0x05C0
                          }
#if DRP_FLYBACK_IBIAS_CALIB
                          //uint16 subband_rffreq[NUM_BANDS*NUM_FLYBACK_SUBBANDS];
                          ,{
                            0x229A, 0x230E, 0x2382,
                            0x2058, 0x20AC, 0x2100,
                            0x4348, 0x4442, 0x453C,
                            0x48A8, 0x4970, 0x4A38
                          },
                          //uint16 ckv_dly[NUM_BANDS*NUM_FLYBACK_SUBBANDS];
                          {
                            3, 3, 3,
                            3, 3, 3,
                            3, 3, 3,
                            3, 3, 3
                          },
                          //uint16 perinv_thresh[NUM_BANDS*NUM_FLYBACK_SUBBANDS][3];
                          {
                             0x0463, 0x0486, 0x04A8,
                             0x0474, 0x0498, 0x04BB,
                             0x0483, 0x04A7, 0x04CA,

                             0x0419, 0x043A, 0x045A,
                             0x0423, 0x0444, 0x0464,
                             0x042F, 0x0451, 0x0472,

                             0x0444, 0x0466, 0x0487,
                             0x0455, 0x0478, 0x049A,
                             0x0465, 0x0488, 0x04AA,

                             0x049E, 0x04C3, 0x04E7,
                             0x04AA, 0x04CF, 0x04F3,
                             0x04B8, 0x04DE, 0x0503
                          },
                          //uint16 ibias[NUM_BANDS*NUM_FLYBACK_SUBBANDS];
                          {
                            0x3C, 0x3C, 0x3C,
                            0x3C, 0x3C, 0x3C,
                            0x3C, 0x3C, 0x3C,
                            0x3C, 0x3C, 0x3C
                          }
#endif
                        },
                        //T_DRP_RX_CALIB         rx;
                        {
                          //T_DRP_ABE_CALIB abe;
                          {
                            //UINT32 abe_calib_table[NUM_ABE_GAINS];
                            //ABE calibration table.
                            //bits(31:16) indicate gain and bits(15:0) indicate alpha
                            //The gain is given the following form 2^GAIN_SCALE * (log2(10^(gain_dB/10)))
                            //gain_dB is the gain in dB scale.
                            //The alpha is given as : 2^ALPHA_SCALE * (alpha_k), alpha_k is the real valued
                            //temperature coefficient
                            {
                              0x291DFF2D,
                              0x24B3FEF2,
                              0x204DFECC,
                              0x1C39FEB7,
                              0x1840FEE0,
                              0x1417FEFF,
                              0x109FFF63,
                              0x0C36FF29,
                              0x07D4FF04,
                              0x03BCFEED,
                              0x0110FEE5,
                              0xFE31FEDE,
                              0xFA83FF28,
                              0xF75DFF2B
                            }
                          },
                          //T_DRP_AFE_CALIB afe;
                          {
                            // The gain is stored in the following format:
                            //     gain_log2 = 2^GAIN_SCALE * (log2(10^(gain_dB/10)))
                            //     The GAIN_SCALE provides 10 fractional bits.
                            //
                            //     gain_dB = 10*log(2^(gain_log2/1024))
                            //             = 3.0103 * gain_log2/1024
                            //             = gain_log2 * 0.002939746051406
                            // SINT16 gain[NUM_BANDS][NUM_AFE_GAINS];
                            {
                               0x2CC8, 0x2B0D, 0x28CB, 0x2579,
                               0x2A1F, 0x2865, 0x2623, 0x22D0,
                               0x2777, 0x25BD, 0x237A, 0x2028,
                               0x24CF, 0x2314, 0x20D2, 0x1D80,
                               0x1234, 0x107A, 0x0E38, 0x0AE5,
                               0x0A3B, 0x0881, 0x063F, 0x02EC,
                               0x0242, 0x0088, 0xFE46, 0xFAF3,

                               0x2CC8, 0x2B0D, 0x28CB, 0x2579,
                               0x2A1F, 0x2865, 0x2623, 0x22D0,
                               0x2777, 0x25BD, 0x237A, 0x2028,
                               0x24CF, 0x2314, 0x20D2, 0x1D80,
                               0x1234, 0x107A, 0x0E38, 0x0AE5,
                               0x0A3B, 0x0881, 0x063F, 0x02EC,
                               0x0242, 0x0088, 0xFE46, 0xFAF3,

                               0x2CC8, 0x2B0D, 0x28CB, 0x2579,
                               0x2A1F, 0x2865, 0x2623, 0x22D0,
                               0x2777, 0x25BD, 0x237A, 0x2028,
                               0x24CF, 0x2314, 0x20D2, 0x1D80,
                               0x1234, 0x107A, 0x0E38, 0x0AE5,
                               0x0A3B, 0x0881, 0x063F, 0x02EC,
                               0x0242, 0x0088, 0xFE46, 0xFAF3,

                               0x2CC8, 0x2B0D, 0x28CB, 0x2579,
                               0x2A1F, 0x2865, 0x2623, 0x22D0,
                               0x2777, 0x25BD, 0x237A, 0x2028,
                               0x24CF, 0x2314, 0x20D2, 0x1D80,
                               0x1234, 0x107A, 0x0E38, 0x0AE5,
                               0x0A3B, 0x0881, 0x063F, 0x02EC,
                               0x0242, 0x0088, 0xFE46, 0xFAF3

                            }
                          },
                          //T_DRP_LNA_CALIB lna;
                          {
                            //UINT16 cfreq[NUM_BANDS];
                            {
                              0xC300,      // EGSM
                              0xC600,      // GSM
                              0xC300,      // DCS
                              0xC300       // PCS
                            }
                          },
                          //T_DRP_IQMC_CALIB iqmc;
                          {
                            //UINT16 coeff[NUM_BANDS][2];
                            {
                              {0x0227, 0xF455},
                              {0x0161, 0xF48F},
                              {0xFE6A, 0xEE65},
                              {0x0092, 0xF19F}
                            }
                          },
                          //T_DRP_SCFPOLE_CALIB scf_pole;
                          {
                            //UINT32 ANA_MIX_LB_IQ_table[NUM_BANDS];
                            {
                              0xFFFEFFFE,     // EGSM
                              0xFFFEFFFE,     // GSM
                              0xFFFEFFFE,     // DCS
                              0xFFFEFFFE      // PCS
                            },
                            //UINT32 ANA_SCF_IQ_0_table[2*NUM_BANDS];
                            {
                              0x74A474A4,                           //EGSM 270kHz, 270kHz
                              0x6C986C98,                           //GSM  270kHz, 270kHz
                              0x78A478A4,                           //DCS  270kHz, 270kHz
                              0x7CB07CB0,                           //PCS  270kHz, 270kHz
                              0x746C746C,                           //EGSM 400kHz, 400kHz
                              0x6C666C66,                           //GSM  400kHz, 400kHz
                              0x74667466,                           //DCS  400kHz, 400kHz
                              0x7C707C70                            //PCS  400kHz, 400kHz
                            }
                          }
                        },
                        //T_DRP_DCXO_CALIB       dcxo;
                        {
                          0x0481,             //cfa_value
                          0x013A              //idac_value
                        },
                        //T_DRP_TEMP_CALIB       temperature;
                        //temperature lookup table - initialized to nominal values
                        {
                          //SINT8 temperature[NUM_TEMPERATURE];
                          {
                            0x54, 0x54, 0x54, 0x54,               // Deg C ( 84,  84,  84,  84), A/D Reading ( 0,  1,  2,  3)
                            0x54, 0x54, 0x54, 0x54,               // Deg C ( 84,  84,  84,  84), A/D Reading ( 4,  5,  6,  7)
                            0x54, 0x54, 0x54, 0x54,               // Deg C ( 84,  84,  84,  84), A/D Reading ( 8,  9, 10, 11)
                            0x54, 0x4C, 0x40, 0x34,               // Deg C ( 84,  76,  64,  52), A/D Reading (12, 13, 14, 15)
                            0x29, 0x1F, 0x14, 0x09,               // Deg C ( 41,  31,  20,   9), A/D Reading (16, 17, 18, 19)
                            0xFF, 0xF5, 0xEA, 0xEA,               // Deg C ( -1, -11, -22, -22), A/D Reading (20, 21, 22, 23)
                            0xEA, 0xEA, 0xEA, 0xEA,               // Deg C (-22, -22, -22, -22), A/D Reading (24, 25, 26, 27)
                            0xEA, 0xEA, 0xEA, 0xEA                // Deg C (-22, -22, -22 ,-22), A/D Reading (28, 29, 30, 31)
                         }
                        },
                        //T_DRP_SW_DATA_VER_EFUSE drp_sw_data_ver_efuse;
                        {
                          0x8A0A,       //version
                          0x3FD3,       //EFUSE0[15:0]
                          0xEAAE,       //EFUSE0[31:16]
                          0x0032,       //EFUSE1[15:0]
                          0x0000,       //EFUSE1[31:16]
                          0x0000        //dummy1
                        },
                        //T_DRP_PPA              ppa;
                        {
                          // magnitude
                          {
                            0x0200,  // GSM900  (EU)
                            0x0200,  // GSM850  (US)
                            0x0200,  // GSM1800 DCS (EU)
                            0x0200   // GSM1900 PCS (US)
                          },
                          // slope (signed value: sign, 3 integer, 12 fractional)
                          {
                            0xF000,  // GSM900  (EU)      default=0xF1FC=-1.000
                            0xF000,  // GSM850  (US)      default=0xF302=-1.000
                            0xE800,  // GSM1800 DCS (EU)  default=0xED02=-1.500
                            0xE000   // GSM1900 PCS (US)  default=0xE31B=-2.000
                        }
                      },
                        //T_FLYBACK_DELAY_CALIB  flyback;
                        {
                          { //SINT16 xmax_table[2]
                            0x0301,
                            0x0604
                          },
                          { //UINT16 cf_table[2];
                            (UINT16)0xFF62,
                            (UINT16)0xFF62
                          },
                          { //UINT16 ct_table[2];
                            0xE01C,
                            0xD422
                          },
                          { //UINT16 perinvcal_table[2];
                            0x0504,
                            0x05A0
                          },
                          { //UINT16 cal_table[2];
                            0x445C,
                            0x4970
                          }
                        }
                      },
                      //T_DRP_LOG_CONST log_const;
                      { //UINT16 logarithm_table[33];
                        {
                          0x0000, 0x002D, 0x005A, 0x0084,
                          0x00AE, 0x00D6, 0x00FE, 0x0124,
                          0x014A, 0x016E, 0x0192, 0x01B4,
                          0x01D6, 0x01F8, 0x0218, 0x0238,
                          0x0257, 0x0275, 0x0293, 0x02B1,
                          0x02CD, 0x02E9, 0x0305, 0x0320,
                          0x033B, 0x0355, 0x036F, 0x0388,
                          0x03A1, 0x03B9, 0x03D1, 0x03E9,
                          0x0400
                        },
                        0x0000           //dummy1
                      },
                      //T_DRP_RX_CONST rx_const
                      {
                        //T_DRP_ABE_CONST abe;
                        {
                          // ABE Step vs. Settings        1.1 Silicon                              1.0 Silicon
                          //    ABE Step             ANA_CTA_0     ANA_CTA_1                   ANA_CTA_0     ANA_CTA_1
                          //    0                      0x9FFF        0x0217                      0xFFFF        0x0217
                          //    1                      0x8FFF        0x0217                      0xFFBF        0x0217
                          //    2                      0x87FF        0x0217                      0xFF9F        0x0217
                          //    3                      0x83FF        0x0217                      0xFF8F        0x0217
                          //    4                      0x007F        0x020B                      0xBF8F        0x0217
                          //    5                      0x003F        0x020B                      0x9F8F        0x0217
                          //    6                      0x001F        0x020B                      0x807F        0x020B
                          //    7                      0x000F        0x020B                      0x803F        0x020B
                          //    8                      0x0007        0x020B                      0x801F        0x020B
                          //    9                      0x0001        0x020B                      0x800F        0x020B
                          //    10                     0x8081        0x0217                      0x8187        0x020B
                          //    11                     0x8001        0x0217                      0x8183        0x0217
                          //    12                     0x8001        0x0217                      0x8181        0x0217
                          //    13                     0x8001        0x0217                      0x8101        0x0217
                          //
                          //UINT16 gain_table_0[NUM_ABE_GAINS];
                          // 1.1 Silicon
                          {
                            0x9FFF, 0x8FFF,
                            0x87FF, 0x83FF,
                            0x007F, 0x003F,
                            0x001F, 0x000F,
                            0x0007, 0x0001,
                            0x8081, 0x8001,
                            0x8001, 0x8001
                          },
                          // UINT16 gain_table_1[DRP_NUM_ABE_GAINS];
                          {
                            0x0217, 0x0217,
                            0x0217, 0x0217,
                            0x020B, 0x020B,
                            0x020B, 0x020B,
                            0x020B, 0x020B,
                            0x0217, 0x0217,
                            0x0217, 0x0217
                          },
#if DRP1_0
                          //UINT16 gain_table_0[NUM_ABE_GAINS];
                          {                                             // ANA_CTA_I/Q_1 - ANA_CTA_I/Q_0  gain(dB)   BBLNA Gain (dB)   TRAN Gain (dB)
                            0xFFFF, 0xFFBF,                             // 0x0217        - FFFF            29            18                11
                            0xFF9F, 0xFF8F,                             // 0x0217        - FFBF            26            18                 8
                            0xBF8F, 0x9F8F,                             // 0x0217        - FF9F            23            18                 5
                            0x807F, 0x803F,                             // 0x0217        - FF8F            20            18                 2
                            0x801F, 0x800F,                             // 0x0217        - BF8F            17            15                 2
                            0x8187, 0x8183,                             // 0x0217        - 9F8F            14            12                 2
                            0x8081, 0x8001                              // 0x020B        - 807F            11            -4 (bypassed)     11
                          },
                          // UINT16 gain_table_1[NUM_ABE_GAINS];     // 0x020B        - 803F             8            -4 (bypassed)      8
                          {                                             // 0x020B        - 801F             5            -4 (bypassed)      5
                            0x0217, 0x0217,                             // 0x020B        - 800F             2            -4 (bypassed)      2
                            0x0217, 0x0217,                             // 0x020B        - 8187             0             0 (bypassed)      0
                            0x0217, 0x0217,                             // 0x0217        - 8183            -2             0                -2
                            0x020B, 0x020B,                             // 0x0217        - 8081            -5            -2                -3
                            0x020B, 0x020B,                             // 0x0217        - 8001            -7            -4                -3
                            0x020B, 0x0217,
                            0x0217, 0x0217
                          },
#endif
                          // UINT16 abe_meas_input_strength[NUM_ABE_GAINS];
                          {
                            0x0007, 0x0007,                              // 29dB , 26dB
                            0x0008, 0x0008,                              // 23dB , 20dB
                            0x0009, 0x0009,                              // 17dB , 14dB
                            0x0009, 0x000A,                              // 11dB ,  8dB
                            0x000A, 0x000B,                              //  5dB ,  2dB
                            0x000B, 0x000B,                              //  0dB , -2dB
                            0x000B, 0x000B                               // -5dB , -7dB
                          }
                        },
                        //T_DRP_AFE_CONST afe;
                        {
                          //UINT16 gain_table[NUM_AFE_GAINS];
                          {                                             //     LNA gains       TA gains
                            //0xFF1F, 0xFF1D, 0xFD1F, 0xFF0D,             //
                            //0xFD1D, 0xF91F, 0xFD0D, 0xF91D,             // value    gain  # value    gain
                            //0xFF15, 0xF11F, 0xF90D, 0xFD15,             // 0xff      24   # 0x1F      MAX
                            //0xF11D, 0xF915, 0xF10D, 0xF115,             // 0xfd      22   # 0x1D      MID
                            //0xE11F, 0xE11D, 0xE10D, 0xC11F,             // 0xf9      20   # 0x0D      LOW
                            //0xC11D, 0xE115, 0xC10D, 0xC115,             // 0xf1      18   # 0x15      MIN
                            //0x811F, 0x811D, 0x810D, 0x8115,             // 0xe1       4
                            //0x011F, 0x011D, 0x010D, 0x0115              // 0xc1       1

                                                                          // LB gains for Locosto
                                                                          //---------------------------------------
                                                                          //     LNA        #      TA
                                                                          //---------------------------------------
                            0xFD1D, 0xFD0D, 0xFD15, 0xFD05,               // 0xfd      24   # 0x1D      MAX (9.7dB)
                            0xF91D, 0xF90D, 0xF915, 0xF905,               // 0xf9      22   # 0x0D      MID (8.4dB)
                            0xF11D, 0xF10D, 0xF115, 0xF105,               // 0xf1      20   # 0x15      LOW (6.7dB)
                            0xE11D, 0xE10D, 0xE115, 0xE105,               // 0xe1      18   # 0x05      MIN (4.2dB)
                            0xC11D, 0xC10D, 0xC115, 0xC105,               // 0xc1       4
                            0x811D, 0x810D, 0x8115, 0x8105,               // 0x81      -2
                            0x011D, 0x010D, 0x0115, 0x0105                // 0x01      -8

                                                                          // HB gains for Locosto
                                                                          //---------------------------------------
                                                                          //     LNA        #      TA
                                                                          //---------------------------------------
                                                                          // 0xfd      24   # 0x1D      MAX (9.7dB)
                                                                          // 0xf9      22   # 0x0D      MID (8.4dB)
                                                                          // 0xf1      20   # 0x15      LOW (6.7dB)
                                                                          // 0xe1      18   # 0x05      MIN (4.2dB)
                                                                          // 0xc1       5.6
                                                                          // 0x81      -0.6
                                                                          // 0x01     -10
                          }
#if DRP1_0
                           //UINT16 gain_table[NUM_AFE_GAINS];
                          {                                             //     LNA gains       TA gains
                            0xFF1D, 0xFF0D, 0xFD1D, 0xFF15,             //
                            0xFD0D, 0xF91D, 0xFD15, 0xF90D,             // value    gain  # value    gain
                            0xFF05, 0xF11D, 0xF915, 0xFD05,             // 0xff      24   # 0x1D      MAX (9.7dB)
                            0xF10D, 0xF905, 0xF115, 0xF105,             // 0xfd      22   # 0x0D      MID (8.4dB)
                            0xE11D, 0xE10D, 0xE115, 0xC11D,             // 0xf9      20   # 0x15      LOW (6.7dB)
                            0xC10D, 0xE105, 0xC115, 0xC105,             // 0xf1      18   # 0x05      MIN (4.2dB)
                            0x811D, 0x810D, 0x8115, 0x8105,             // 0xe1       4
                            0x011D, 0x010D, 0x0115, 0x0105              // 0xc1       1
                          }                                             // 0x81      -5
#endif                                                                  // 0x01     -11
                        }
                      },
                      //T_DRP_SIMU  simulation;
                      {
                        //T_DRP_RX_SIMU rx;
                        {
                          //T_DRP_ABE_SIMU abe;
                          {
                            //UINT16 beta[NUM_ABE_GAINS];
                            {
                              0x4E10, 0x4690,
                              0x3C1C, 0x2BF8,
                              0x316B, 0x2F70,
                              0x2465, 0x2100,
                              0x1533, 0x0E1C,
                              0x089B, 0x044C,
                              0x239F, 0x1FE3
                            },
                            //ABE calibration table.
                            //bits(31:16) indicate gain and bits(15:0) indicate alpha
                            //The gain is given the following form 2^GAIN_SCALE * (log2(10^(gain_dB/10)))
                            //gain_dB is the gain in dB scale.
                            //The alpha is given as : 2^ALPHA_SCALE * (alpha_k), alpha_k is the real valued
                            //temperature coefficient
                            //UINT32 abe_calib_table_weak[NUM_ABE_GAINS];
                            {
                              0x2204FF07, 0x1D0FFECC,
                              0x1853FEAD, 0x1421FE99,
                              0x103FFEC4, 0x0C20FEE5,
                              0x090DFF51, 0x0415FF17,
                              0xFF5FFEF7, 0xFB26FEE6,
                              0xF875FEE0, 0xF59FFEDC,
                              0xF2B6FF14, 0xEF8CFF1C
                            }
                          }
                        },
                        //T_DRP_TEMP_SIMU temperature;
                        {
                          //SINT8 temp_code_weak[NUM_TEMPERATURE];
                          {
                            127,  127,  127,  127,  127,  127,  127,  127,
                            127,  123,  110,   95,   84,   72,   59,   45,
                             32,   20,    6,   -9,  -21,  -34,  -47,  -60,
                            -73,  -86,  -99, -112, -125, -128, -128, -128
                          },
                          //SINT8 temp_code_strong[NUM_TEMPERATURE];
                          {
                            127,  127,  127,  127,  127,  127,  127,  127,
                            127,  123,  110,   95,   84,   72,   59,   45,
                             32,   20,    6,   -9,  -21,  -34,  -47,  -60,
                            -73,  -86,  -99, -112, -125, -128, -128, -128
                          }
                        }
                      }
                    };

#ifdef _DEBUG
UINT32 gainArr[16], gainInx = 0;
#endif

/***************************************************
*******  External Structures and variables *********
****************************************************/

#if LOCOSTO_VALID
#else
extern UINT16 macro_temp16;//for Macro definition purposes
#endif

#if (DRP_FW_BUILD==0)

extern T_DRP_SW_DATA *drp_sw_data;
extern T_DRP_REGS_STR *drp_regs;
extern T_DRP_SRM_DATA *drp_srm_data;
extern T_DRP_SRM_API  *drp_srm_api;
extern UINT16 g_pcb_config;
#endif

/***************************************************
********* Test SW Variable definitions *************
***************************************************/

#if DRP_TEST_SW
#if DRP_BENCH_SW
#include "compute_fft_pre.h"

const tFloatingPointComplex twiddle_factor_LUT[MAX_NB_SAMPLES] =  /* The precomputed twiddle factors LUT that will be used */
  {
    PRE_twiddle_factor_LUT
  };
#endif //#if DRP_BENCH_SW

float g_Buffer[17*FFT_BUF_SIZE];
SINT16 g_rx_data_buffer_decimated[FFT_BUF_SIZE*2];

T_DRP_SRM_DCOFFSET drp_srm_dc_offset[512];
SINT8 drp_measured_temp[512];

SINT16 g_rx_data_buffer_decimated_acum[256*FFT_BUF_SIZE*2];
UINT32 FFT_Pwr_I_ate[FFT_BUF_SIZE];
UINT32 FFT_Pwr_Q_ate[FFT_BUF_SIZE];
UINT32 FFT_Pwr_IQ_ate[FFT_BUF_SIZE];
float  FFT_Pwr_Avg_I_ate[FFT_BUF_SIZE];
float  FFT_Pwr_Avg_Q_ate[FFT_BUF_SIZE];
float  FFT_Pwr_Avg_IQ_ate[FFT_BUF_SIZE];
float  PhaseMismatch_avg_ate;

    #include "fft.h"
    extern S16 CosTable[FFT_BUF_SIZE];
    extern U16 BitReverseTable[128];
#endif //#if DRP_TEST_SW

//#if (DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT))

#if (DRP_FW_BUILD==0)
#define DRP_ZIF_LUT_SIZE     128

const short ziflut[DRP_ZIF_LUT_SIZE] = {
  201,     603,  1005,  1407,  1809,  2210,  2611,  3012,
  3412,   3812,  4211,  4609,  5007,  5404,  5800,  6195,
  6590,   6983,  7376,  7767,  8157,  8546,  8933,  9319,
  9704,  10088, 10469, 10850, 11228, 11605, 11980, 12354,
  12725, 13095, 13463, 13828, 14192, 14553, 14912, 15269,
  15624, 15976, 16326, 16673, 17018, 17361, 17700, 18037,
  18372, 18703, 19032, 19358, 19681, 20001, 20318, 20632,
  20943, 21251, 21555, 21856, 22154, 22449, 22740, 23028,
  23312, 23593, 23870, 24144, 24414, 24680, 24943, 25202,
  25457, 25708, 25956, 26199, 26439, 26674, 26906, 27133,
  27357, 27576, 27791, 28002, 28209, 28411, 28610, 28803,
  28993, 29178, 29359, 29535, 29707, 29875, 30038, 30196,
  30350, 30499, 30644, 30784, 30920, 31050, 31177, 31298,
  31415, 31527, 31634, 31737, 31834, 31927, 32015, 32099,
  32177, 32251, 32319, 32383, 32442, 32496, 32546, 32590,
  32629, 32664, 32693, 32718, 32738, 32753, 32762, 32767
};

//#endif //#if (DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT))

// ABE estimated gain table
SINT16 abe_est_table[NUM_ABE_GAINS] = {
  0x0000, 0x0000,
  0x0000, 0x0000,
  0x0000, 0x0000,
  0x0000, 0x0000,
  0x0000, 0x0000,
  0x0000, 0x0000,
  0x0000, 0x0000
};

// AFE estimated gain table
SINT16 afe_est_table[NUM_AFE_GAINS] = {
  0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000
};

// Target ABE gain table
SINT16 tar_abe_gain_table[DRP_NUM_ABE_GAIN_STEPS] = {
  DRP_GAIN_0_DB,
  DRP_GAIN_2_0_DB,
  DRP_GAIN_5_0_DB,
  DRP_GAIN_8_0_DB,
  DRP_GAIN_11_0_DB,
  DRP_GAIN_14_0_DB,
  DRP_GAIN_17_0_DB,
  DRP_GAIN_20_0_DB,
  DRP_GAIN_23_0_DB,
  DRP_GAIN_26_0_DB,
  DRP_GAIN_29_0_DB
};

// Target AFE gain table
SINT16 tar_afe_gain_table[2][DRP_NUM_AFE_GAIN_STEPS] = {
  {
    DRP_GAIN_11_0_DB,  //LB
    DRP_GAIN_36_5_DB,  //LB
  },
  {
    DRP_GAIN_11_0_DB,  //HB
    DRP_GAIN_36_5_DB,  //HB
  }
};


UINT16 g_band_cntr_ch[NUM_BANDS] = {
   EGSM_CENTER_CH,
   GSM_CENTER_CH,
   DCS_CENTER_CH,
   PCS_CENTER_CH
};

UINT16 default_ABE_step_table[DRP_NUM_ABE_GAIN_STEPS] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
UINT16 default_AFE_step_table[2][DRP_NUM_AFE_GAIN_STEPS] = {17, 0,
                                                            18, 0};
#endif
#define IQMC_ROC_DEBUG 1
#define ROC_DEBUG 0
#if ROC_DEBUG
CMPLX16 zif_tone_gen1[160], zif_tone_gen2[160], subtracted_tone[128];
#endif

// Save afe estimated gain from 2nd compensation (the one that uses measured ABE gain).
#if (DRP_FW_BUILD==0)
SINT16 g_afe_gain;
  extern T_DRP_SW_DATA drp_sw_data_calib;
#endif

extern const unsigned char drp_phe_anayser_dft[];
#if (DRP_FW_BUILD == 1)
  extern const UINT16 g_band_cntr_ch[];
  extern const UINT16 default_ABE_step_table[DRP_NUM_ABE_GAIN_STEPS];
  extern const UINT16 default_AFE_step_table[2][DRP_NUM_AFE_GAIN_STEPS];
#endif

#if ((DRP_TEST_SW && !DRP_BURNIN) || (!DRP_TEST_SW))
/**************************************************************************************/
/*                              FUNCTION DEFINITIONS                                  */
/**************************************************************************************/

/**************************************************************************************/
/* Common routine between TX and RX. Does not require any external input.             */
/* It does the following calibration for both TX and RX bands.                        */
/*   1. Period Inverse                                                                */
/*   2. DLO Acquisition Tracking Calibration                                          */
/*   3. PVT offset                                                                    */
/*   4. KDCO calibration                                                              */
/*   5. DCO retiming adjustment (Not used right now)                                  */
/*   6. DCO Ibias               (Not used right now)                                  */
/**************************************************************************************/
void tx_rx_common_calib(UINT16 pcb_config) {
  UINT16 band_index, temp16, rxon_input_saved, dbbif_setting;
  UINT16 txrx_mode;
  UINT16 mem_data;
  UINT16 tx_ana_dco_egsm, tx_ana_dco_gsm, tx_ana_dco_dcs, tx_ana_dco_pcs;

  // save these values as they will get overwritten later on for KDCO calibration in RX band
  READ_WORD(drp_srm_data->tables.tx_dco[EGSM_BAND],tx_ana_dco_egsm);
  READ_WORD(drp_srm_data->tables.tx_dco[GSM_BAND],tx_ana_dco_gsm);
  READ_WORD(drp_srm_data->tables.tx_dco[DCS_BAND],tx_ana_dco_dcs);
  READ_WORD(drp_srm_data->tables.tx_dco[PCS_BAND],tx_ana_dco_pcs);

#if DRP_PHASE2_CALIBRATION
  /***************************
  * Temperature Calibration
  ****************************/
  temp_sensor_calibration((SINT16) TREF);
#endif

  /***************************
  * Transmitter Calibrations
  ****************************/

  txrx_mode = TX_MODE;

  for (band_index = 0; band_index < NUM_BANDS; band_index++) {
    // Write the value of the RF_FREQ register
    dbb_write_rffreq(band_index, g_band_cntr_ch[band_index], txrx_mode);

    // PVT bank calibration
    pvt_calibration(band_index, txrx_mode);
  }

  perinv_calibration();

  dlo_a_t_calibration();

  for (band_index = 0; band_index < NUM_BANDS; band_index++) {
    // Write the value of the RF_FREQ register
    dbb_write_rffreq(band_index, g_band_cntr_ch[band_index], txrx_mode);

    KDCO_calibration(band_index, txrx_mode);

#if DRP_PHASE2_CALIBRATION
    // Interaction between the tuning word alignment calibration and
    // the DCO bias current calibration will need to be studied post silicion.

    //DCO tuning word retiming alignment adjustment calibration
    dco_retiming_calibration((txrx_mode*NUM_BANDS) + band_index);

    //DCO bias current calibration
    dco_ibias_calibration((txrx_mode*NUM_BANDS) + band_index);
#endif
  }

  /***************************
  * Receiver Calibrations
  ****************************/

  txrx_mode = RX_MODE;

  READ_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);

  for (band_index = 0; band_index < NUM_BANDS; band_index++) {
    // Write the value of the RF_FREQ register
    dbb_write_rffreq(band_index, g_band_cntr_ch[band_index], txrx_mode);

    //DBBIF setting generation and passing to rxon_input
    READ_WORD(drp_srm_api->inout.rx.rxon_input, temp16);
    temp16 = temp16 & (~0x0300);
    // dbbif setting
    #if DRP_TEST_SW
      dbbif_setting = drp_generate_dbbif_setting(pcb_config, band_index);
    #else
      dbbif_setting = drp_generate_dbbif_setting(g_pcb_config, band_index);
    #endif
    temp16 = (temp16 & (~0x0300)) | ((dbbif_setting & 0x3) << 8);
    WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

    //PVT bank calibration
    pvt_calibration(band_index, txrx_mode);

    //re-initialize ANA_DCO settings for RX bands before doing KDCO calibration
    //turn on the inductor for EGSM (RX) band and DCS (RX) band
    //if ((band_index == EGSM_BAND) || (band_index == DCS_BAND)) {
    //  mem_data = *(volatile ioport UINT16 *)(DRP_DRP_OFFSET + (DRP_TX_DCO_TABLE >> 1) + band_index);
    //  mem_data = mem_data | 0x8000;
    //  *(volatile ioport UINT16 *)(DRP_DRP_OFFSET + (DRP_TX_DCO_TABLE >> 1) + band_index) = mem_data;
    //}

    // update the TX calibrated PVT values with the calibrated PVT values for RX band
    //
    // TX_ON sets the DBBIF register as follows
    //
    //                 LOW BAND            1000MHz          HIGH BAND
    //   |-----------------|-----------------|-----------------|-----------------|
    //                    860MHz                             1800MHz
    //       GSM850               EGSM             DCS                 PCS
    //
    // Center of RX bands are
    //       GSM850     881.4MHz
    //       EGSM       942.4MHz
    //       DCS       1842.4MHz
    //       PCS       1960.0MHz
    //
    // So when we call KDCO calibration for RX bands, for GSM850 the TX_ON will assume it to be EGSM band
    //                                                for DCS    the TX_ON will assume it to be PCS band
    //
    // Hence we need to write the calibrated PVT offset at appropriate locations as given below.
    // This is not the clean way of doing this, but it is OK for now.
    //
    switch (band_index) {
      case EGSM_BAND:
        WRITE_WORD(drp_srm_data->tx.pvt[EGSM_BAND], drp_sw_data->calib.txrx_common.pvt_value[EGSM_BAND + NUM_BANDS]);
        // update ANA_DCO value
        READ_WORD(drp_srm_data->tables.tx_dco[EGSM_BAND], mem_data);
        mem_data = mem_data | 0x8000;
        WRITE_WORD(drp_srm_data->tables.tx_dco[EGSM_BAND], mem_data);
      break;

      case GSM_BAND:
        WRITE_WORD(drp_srm_data->tx.pvt[EGSM_BAND], drp_sw_data->calib.txrx_common.pvt_value[GSM_BAND + NUM_BANDS]);
        // update ANA_DCO value
        READ_WORD(drp_srm_data->tables.tx_dco[EGSM_BAND], mem_data);
        mem_data = mem_data & 0x7fff;
        WRITE_WORD(drp_srm_data->tables.tx_dco[EGSM_BAND], mem_data);
      break;

      case DCS_BAND:
        WRITE_WORD(drp_srm_data->tx.pvt[PCS_BAND], drp_sw_data->calib.txrx_common.pvt_value[DCS_BAND + NUM_BANDS]);
        // update ANA_DCO value
        READ_WORD(drp_srm_data->tables.tx_dco[PCS_BAND], mem_data);
        mem_data = mem_data | 0x8000;
        WRITE_WORD(drp_srm_data->tables.tx_dco[PCS_BAND], mem_data);
      break;

      case PCS_BAND:
        WRITE_WORD(drp_srm_data->tx.pvt[PCS_BAND], drp_sw_data->calib.txrx_common.pvt_value[PCS_BAND + NUM_BANDS]);
        // update ANA_DCO value
        READ_WORD(drp_srm_data->tables.tx_dco[PCS_BAND], mem_data);
        mem_data = mem_data | 0x8000;
        WRITE_WORD(drp_srm_data->tables.tx_dco[PCS_BAND], mem_data);
      break;

      default:
      break;
    }

    KDCO_calibration(band_index, txrx_mode);

#if DRP_PHASE2_CALIBRATION
    // Interaction between the tuning word alignment calibration and
    // the DCO bias current calibration will need to be studied post silicion.

    //DCO tuning word retiming alignment adjustment calibration
    dco_retiming_calibration((txrx_mode*NUM_BANDS) + band_index);

    //DCO bias current calibration
    dco_ibias_calibration((txrx_mode*NUM_BANDS) + band_index);
#endif
  }

  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);

  //initializa SRM with calibrated KDCO values
  for (band_index = 0; band_index < 2*NUM_BANDS; band_index++) {
    if (band_index < NUM_BANDS) {
      // Restore the TX PVT offset values
      WRITE_WORD(drp_srm_data->tx.pvt[band_index], drp_sw_data->calib.txrx_common.pvt_value[band_index]);
      WRITE_WORD(drp_srm_data->tx.kdco_t[band_index],drp_sw_data->calib.txrx_common.kdco_inv[band_index]);
    }
    else {
      WRITE_WORD(drp_srm_data->rx.pvt[band_index - NUM_BANDS], drp_sw_data->calib.txrx_common.pvt_value[band_index]);
      WRITE_WORD(drp_srm_data->rx.kdco_t[band_index - NUM_BANDS], drp_sw_data->calib.txrx_common.kdco_inv[band_index]);
    }
  }

  //turn off inductors for EGSM and DCS bands for TX (they have been turned on for KDCO calibration for RX bands)
  WRITE_WORD(drp_srm_data->tables.tx_dco[EGSM_BAND],tx_ana_dco_egsm);
  WRITE_WORD(drp_srm_data->tables.tx_dco[GSM_BAND],tx_ana_dco_gsm);
  WRITE_WORD(drp_srm_data->tables.tx_dco[DCS_BAND],tx_ana_dco_dcs);
  WRITE_WORD(drp_srm_data->tables.tx_dco[PCS_BAND],tx_ana_dco_pcs);

  return;
}

/**************************************************************************************/
/* Period Inverse Calibration                                                         */
/**************************************************************************************/
void perinv_calibration(void)
{
  UINT16 mem_data;

  WRITE_WORD(drp_regs->RF_FREQL, 0x45BA);

  //trigger TX_ON
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0081);

  WAIT_US(200);

  //read bits (12:0) of DLO_PERINV register
  READ_WORD(drp_regs->DLO_PERINVL, mem_data);

  drp_sw_data->calib.txrx_common.perinv_value = (UINT16) (mem_data & 0x1FFF);

  //trigger the idle script
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
  WAIT_US(20);

  //write the calibrated period inverse value
  WRITE_WORD(drp_srm_data->txrx.perinv,drp_sw_data->calib.txrx_common.perinv_value);
  WRITE_WORD(drp_regs->DLO_PERINVL, drp_sw_data->calib.txrx_common.perinv_value);

  return;
}

/**************************************************************************************/
/* PVT bank Calibration                                                               */
/**************************************************************************************/
void pvt_calibration(UINT16 band, UINT16 txrx_mode)
{
  UINT16 index;
  SINT16 mem_data, curr_offset = 0, sum_offset = 0, calib_offset;

  //initialize initial Calibrated Offset set in SRM to 0
  if (txrx_mode == TX_MODE)
  {
    WRITE_WORD(drp_srm_data->tx.pvt[band],0);
  }
  else
  {
    WRITE_WORD(drp_srm_data->rx.pvt[band],0);
  }
#ifdef _DEBUG
  gainInx = 0;
#endif

  for (index = 0; index < 8; index++)
  {
    if (txrx_mode == TX_MODE)
    {
      WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0081);
    }
    else
    {
      set_abe_gain(13);
      set_afe_gain(27, band);
      rx_on_calib_timeline(band, g_band_cntr_ch[band]);
    }

    WAIT_US(200);

    //read the PVT offset from RDLO_TUNE(22:16) bits. This is a signed number
    READ_WORD(drp_regs->RDLO_TUNEH, mem_data);
    mem_data = (mem_data & 0x7F);
    if (mem_data >> 6) mem_data = -(0x0080 - mem_data);

    //do not update the DLO_OFFSET for the last 4 runs, average the RDLO_TUNE(22:16) and
    //add it to the DLO_OFFSET obtained after 4th run.
    if (index < 4)
    {
    curr_offset += mem_data;

    if (txrx_mode == TX_MODE)
    {
      WRITE_WORD(drp_srm_data->tx.pvt[band],(curr_offset & 0x007F));
    }
    else
    {
      WRITE_WORD(drp_srm_data->rx.pvt[band],(curr_offset & 0x007F));
    }
#ifdef _DEBUG
    gainArr[gainInx++] = mem_data;
#endif
    }
    else
    {
      sum_offset += mem_data;
    }

    //trigger the idle script
    WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
    WAIT_US(20);
  }

  calib_offset = ((sum_offset + 2) >> 2) + curr_offset;

  //store calibration results
  if (txrx_mode == TX_MODE)
  {
    drp_sw_data->calib.txrx_common.pvt_value[band] = calib_offset & 0x007F;
    WRITE_WORD(drp_srm_data->tx.pvt[band],drp_sw_data->calib.txrx_common.pvt_value[band]);
  }
  else
  {
    drp_sw_data->calib.txrx_common.pvt_value[band + NUM_BANDS] = calib_offset & 0x007F;
    WRITE_WORD(drp_srm_data->rx.pvt[band],drp_sw_data->calib.txrx_common.pvt_value[band + NUM_BANDS]);
  }

  return;
}

/**************************************************************************************/
/* Acquisition/Tracking calibration                                                   */
/*                                                                                    */
/* The calibration algorithm is run for 4 times and Acquisition Tracking ratio        */
/* is averaged for the 4 runs. The 'average' value is stored in DSP memory            */
/* and in the DLO_TUNE_A_T register.                                                  */
/*                                                                                    */
/* Note: The loop_count, N (value 4) should not to be changed unless the the          */
/*       number (N=4) over which the dlo_a_t is averaged is changed and unless        */
/*       the shift value {log2(N)=2} of sum_dlo_a_t is changed.                       */
/*                                                                                    */
/*       Keep N a power of 2 so that division is a simple bit shift.                  */
/**************************************************************************************/
void dlo_a_t_calibration(void) {

  SINT32 num1, num2, sum_dlo_a_t, dlo_a_t;
  UINT16 mem_data, num_clocks, avg_index;
  UINT32 temp32;


  WRITE_WORD(drp_regs->RF_FREQL, 0x45BA);

  //trigger TX_ON
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0081);

  WAIT_US(200);

  sum_dlo_a_t = 0;
  for (avg_index = 0; avg_index < 4; avg_index++) {
  //start modulation (+ve value)
  WRITE_BYTE_HIGH(drp_regs->DLO_SEQL, 0x1001);
  //wait for settling
  WAIT_US(20);
  //start KDCO acquisition calculation 1
  WRITE_BYTE_HIGH(drp_regs->DLO_SEQL, 0x2001);
  //This wait corresponds to the min. time taken for num tune_t clocks to complete at 26MHz
  WAIT_US(12);

  //Read accumulated value of tune_ti and tune_tf
  READ_WORD(drp_regs->RTUNE_ACCH, mem_data);
  temp32 = (mem_data << 16);
  READ_WORD(drp_regs->RTUNE_ACCL, mem_data);
  temp32 |= (mem_data & 0xFFFF);
  num1 = (SINT32)temp32;

  //start modulation (-ve value)
  WRITE_BYTE_HIGH(drp_regs->DLO_SEQL, 0x3001);
  //wait for settling
  WAIT_US(20);
  //start KDCO acquisition calculation 2
  WRITE_BYTE_HIGH(drp_regs->DLO_SEQL, 0x4001);
  //This wait corresponds to the min. time taken for num tune_t clocks to complete at 26MHz
  WAIT_US(12);

  //Read accumulated value of tune_ti and tune_tf
  READ_WORD(drp_regs->RTUNE_ACCH, mem_data);
  temp32 = (mem_data << 16);
  READ_WORD(drp_regs->RTUNE_ACCL, mem_data);
  temp32 |= (mem_data & 0xFFFF);
  num2 = (SINT32)temp32;

  READ_WORD(drp_regs->DLO_CALIBL, mem_data);
  //read bits (11:8) of DLO_CALIB. These correspond to number of tune_t samples
  //used to accumulate for averaging.
  num_clocks = (mem_data >> 8) & 0xF;

  //[((+ve acc) - (-ve acc))/2^(no. tune_t samples)]/2
    dlo_a_t = (num1 - num2) >> (num_clocks + 1);

      sum_dlo_a_t = sum_dlo_a_t + dlo_a_t;
  }

  // store the average value
  mem_data = ((sum_dlo_a_t + 2) >> 2) & 0xFFFF;
  drp_sw_data->calib.txrx_common.dlo_at_value = mem_data;
  WRITE_WORD(drp_regs->DLO_TUNE_A_TL, mem_data);
  //WRITE_WORD(drp_srm_data->txrx.dlo_tune_at,drp_sw_data->calib.txrx_common.dlo_at_value);

  //trigger Idle script
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
  WAIT_US(20);

  return;
}

#endif //#if ((DRP_TEST_SW && !DRP_BURNIN) || (!DRP_TEST_SW))

#if ((DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN)) || (!DRP_TEST_SW))

/**************************************************************************************/
/* This function sets the proper resonator settings for LNA                           */
/**************************************************************************************/
void lna_cfreq_calibration(UINT16 band) {
  UINT16 index1, index2, index3, gain;
  UINT16 resonator_setting = 0;
  SINT16 gainlog2, max_gain = 0, tmpgainlog2_1, tmpgainlog2_2;
  UINT32 vpp, gainsum32[16];
  UINT16 id_version;

  // Reset LNA CFREQ value to default when starting calibration
  // Save Bias bits ([15:14]) and clear other bits ([13:8])
  drp_sw_data->calib.rx.lna.cfreq[band] = drp_sw_data->calib.rx.lna.cfreq[band] & 0xF000;

  // Read Version ID register to determine device type
  READ_WORD(drp_regs->DRP2_ID_1L, id_version);
  // If Silicon version 1.1 and and High Band, set bits [9:8] only: DCS=11, PCS=11
  if((id_version == 3) && (band >= 2)) {
      drp_sw_data->calib.rx.lna.cfreq[band] = drp_sw_data->calib.rx.lna.cfreq[band] | 0x0300;
    }
  else {

    // Perform RX ON to required timeline with full offset correction
    //   Offset done 3 additional times
    //   Leaves ARX Bus Active
    // Set the gains corresponding to AGC step 1
    set_abe_gain(4);
    set_afe_gain(4, band);

    rx_on_calib_timeline(band, g_band_cntr_ch[band]);

    // outer loop on Q reducing resistor (0=no additional, 1=first DeQ, 2=second DeQ Only, 3=both DeQ)
    for (index1 = 0; index1 <= 3; index1++) {

    // set the Qreducingresistor
      dbb_enable_LNA_Qreducingresistor(index1);

      // clear gainsum32 array
      for (index2=0;index2<16;index2++) gainsum32[index2] = 0;

      // Find Maximum Gain for current Q reducing resistor value
      // index2 controls the number of measurements made per resonator setting.
      for (index2=0;index2<16;index2++) {

        // this loop does one gain measurement for each resonator setting.
        // the measured gain for each resonator setting is accumulated individually
        // in gainsum32
        for (index3 = 0; index3 < 16; index3++) {
          dbb_set_LNA_resonator_setting(index3);
          vpp = dbb_measure_total_gain();

          // Save only I portion of data (high 16-bits of return val is I channel)
          gainsum32[index3] += (vpp >> 16);
        }
      }

      // Determine the maximum gain

      max_gain = 0;
      resonator_setting = 0;

      // average the 16
      for (index2=0;index2<16;index2++) {

#ifdef _DEBUG
        gainArr[index2] = gainsum32[index2] >> 4;
#endif

        // Find average gain over the 16 samples accumulated (shift by log2(num_measurements)
        gain = (UINT16)(gainsum32[index2] >> 4);
      gainlog2 = 2*calc_log(gain);

        // track setting that has max gain.
        if (gainlog2 > max_gain) {
        max_gain = gainlog2;
        resonator_setting = index2;
      }
    }

      // Initialize adjacent to equal max (used if resonator_setting= 0 or 15)
      tmpgainlog2_1 = max_gain;
      tmpgainlog2_2 = max_gain;

      if(resonator_setting > 0) {
        gain = (UINT16)(gainsum32[resonator_setting-1] >> 4);
        tmpgainlog2_1 = 2*calc_log(gain);
      }

      if(resonator_setting < 15) {
        gain = (UINT16)(gainsum32[resonator_setting+1] >> 4);
        tmpgainlog2_2 = 2*calc_log(gain);
      }

      // when gain drop is less than 0.3 dB on either side of the max gain setting, save value and stop looking
      //if (((max_gain - tmpgainlog2_1) < GAIN_0_3_DB) && ((max_gain - tmpgainlog2_2) < GAIN_0_3_DB)) {
      if (((max_gain - tmpgainlog2_1) < GAIN_20_0_DB) && ((max_gain - tmpgainlog2_2) < GAIN_20_0_DB)) {
      // save the resonator_setting
        //drp_sw_data->calib.rx.lna.cfreq[band] &= 0x00FF;
        drp_sw_data->calib.rx.lna.cfreq[band] |= (resonator_setting << 8);
        // save the Qreducingresistor
        drp_sw_data->calib.rx.lna.cfreq[band] |= (index1 << 12);
      break;
    }
  }

  //trigger IDLE script
    WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
    wait_qb(22);   // ~20us

  }
  return;

}


/**************************************************************************************/
/* This is IQMC calibration function                                                  */
/**************************************************************************************/
//Apply a -60dBm sine wave (freq = center of the band) at the phone input
void iqmc_calibration(UINT16 band) {
  UINT16 mem_data, roccw_save;

  //trigger the temperature measurement script
  dbb_rx_burst1();
  wait_qb(21);

  // use step 2 for iqmc calibration
  // for Locosto, choose ABE gain = 20dB, AFE gain = 38dB, SCF 3dB pole at 173kHz and disable gain_compensation
  dbb_rx_burst2(7, 1, 0, band, 0);

  // Perform RX ON to required timeline with full offset correction
  rx_on_calib_timeline(band, g_band_cntr_ch[band]);

  // ------ Setup DRP for IQMC calibration ------
  // Do set for IQMC after RX_ON to override Values set by RX_ON script
  // same roc_cw
  READ_WORD(drp_regs->ROC_CWL,roccw_save);
  // enable HPF (iqmc calibration requires it).
  WRITE_WORD(drp_regs->ROC_CWL,0x1c40);

  // set threshold and mu
  WRITE_WORD(drp_regs->IQMC_THRESHL, 0x2000);
  WRITE_BYTE_LOW(drp_regs->IQMC_MUL, 0x0005);
  WRITE_WORD(drp_regs->IQMC_W0_INL, 0x0000);
  WRITE_WORD(drp_regs->IQMC_W0_INH, 0x0000);

  // flush IQMC previous state
  WRITE_BYTE_LOW(drp_regs->IQMC_CWL, 0x0007);
  // wait for everything to settle (need steady state through HPF to IQMC)>
  //wait_qb(750);   // ~650qb
  WAIT_US(1000);

  // start IQMC adaptation
  WRITE_BYTE_LOW(drp_regs->IQMC_CWL, 0x0015);

  // wait for 100 ms to complete IQMC calibration.
  // use wait_us, because wait_qb can wait that long.
  WAIT_US(100000);

  // stop adaptation
  WRITE_BYTE_LOW(drp_regs->IQMC_CWL, 0x0014);

  // Store the IQMC coefficient, Real and Imaginary parts
  READ_WORD(drp_regs->IQMC_W0_OUTH, mem_data);
  drp_sw_data->calib.rx.iqmc.coeff[band][0] = mem_data;          //Real part
  WRITE_WORD_AT_PTR((((UINT16 *)&(drp_srm_data->rx.iqmc[band]))+1),mem_data);

  READ_WORD(drp_regs->IQMC_W0_OUTL, mem_data);
  drp_sw_data->calib.rx.iqmc.coeff[band][1] = mem_data;          //Imaginary part
  WRITE_WORD_AT_PTR(((UINT16 *)&(drp_srm_data->rx.iqmc[band])),mem_data);

  // enable correction use IQMC_W0_IN register for coefficients and use adapt engine for correction.
  //WRITE_BYTE_LOW(drp_regs->IQMC_CWL, 0x0016);

  //Put IQMC in adaptation mode for now (as the ROC code does not handle this)
  WRITE_BYTE_LOW(drp_regs->IQMC_CWL, 0x0016);

  //trigger IDLE script
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
  WAIT_US(20);

  // Restore original ROC_CW
  WRITE_WORD(drp_regs->ROC_CWL, roccw_save);

  return;
}

/**************************************************************************************/
/* Functionality: SCF Pole Calibration for the mixer pole                             */
/*                                                                                    */
/* This function writes the settings for                                              */
/* ANA_MIX_LB_I_0(15:0), and ANA_MIX_LB_Q_0(15:0) packed into                         */
/* a 32-bit word. AS (ANA_MIX_LB_I_0(15:0) << 16) | ANA_MIX_LB_Q_0(15:0)              */
/**************************************************************************************/
void mixer_pole_calibration(UINT16 band) {
  UINT32 vpp;
  UINT16 amp_3_db,amp_current;
  UINT16 current_setting;
  SINT16 index;
  UINT32 setting_ret;

  // Perform RX ON to required timeline with full offset correction
  set_abe_gain(4);
  set_afe_gain(4, band);
  rx_on_calib_timeline(band, g_band_cntr_ch[band]);

  // Here we assume that the rotating capacitor values for I and Q
  // channels, are set to nominal values by the RX_ON script. It is
  // important that the RX_ON script values used for CR1 and CR2 by the
  // calibration script and the phone mode script be the same.
  // CR2 == ANA_SCF_I/Q_1(B11:B8)
  // CR1 == ANA_SCF_I/Q_1(B7:B4) - Verify this, missing in CTL bits spreadsheet

  /* CH, CB1, and CB2 in minimum capacitance settings */
  WRITE_WORD(drp_regs->ANA_MIX_LB_IL,0x0001);
  WRITE_WORD(drp_regs->ANA_MIX_LB_QL,0x0001);

  WRITE_WORD(drp_regs->ANA_SCF_I_0L, 0x0000);
  WRITE_WORD(drp_regs->ANA_SCF_Q_0L, 0x0000);

  // ********** Tune CH for the I Channel **********
  // VPP 16 times and compute 3dB loss
  vpp=dbb_measure_total_gain_16x_i();

  // 3db is ~1.4125 linear
  // Vpp/16/1.4125 = Vpp/(16*1.4125) ~= VPP/(16384/725)
  amp_3_db = (UINT16)(((vpp * 725) >> 14) & 0x0000FFFF);
#ifdef _DEBUG
    gainArr[0] = amp_3_db;
    for(index=1; index < 16;index++) gainArr[index] = 0;
  #endif

  current_setting = 0x0001;

  // Thermometer coded, 15 bits, increase value until lower than 3db value.
  for(index=0; index < 15;index++) {

    current_setting = (current_setting << 1) | 0x0001;
    // apply the setting
    WRITE_WORD(drp_regs->ANA_MIX_LB_IL,current_setting);

    // measure amplitude (Average over 16)
    vpp=dbb_measure_total_gain_16x_i();
    amp_current = (UINT16)((vpp >> 4) & 0x0000FFFF);

    #ifdef _DEBUG
      gainArr[index+1] = amp_current;
    #endif

    if(amp_current <= amp_3_db) break;

  } // for loop

  setting_ret = ((UINT32)current_setting << 16);

  // ************* Tune CH for the Q Channel *************
  vpp=dbb_measure_total_gain_16x_q();
  amp_3_db = (UINT16)(((vpp * 725) >> 14) & 0x0000FFFF);
#ifdef _DEBUG
    gainArr[0] = amp_3_db;
    for(index=1; index < 16;index++) gainArr[index] = 0;
  #endif

  current_setting = 0x0001;

  // Thermometer coded, 15 bits, increase value until lower than 3db value.

  for(index=0; index < 15;index++){

    current_setting = (current_setting << 1) | 0x0001;
    // apply the setting
    WRITE_WORD(drp_regs->ANA_MIX_LB_QL,current_setting);

    // measure amplitude (Average over 16)
    vpp=dbb_measure_total_gain_16x_q();
    amp_current = (UINT16)((vpp >> 4) & 0x0000FFFF);
#ifdef _DEBUG
      gainArr[index+1] = amp_current;
    #endif

    if(amp_current <= amp_3_db) break;

  } // for loop

  // Set output
  setting_ret = setting_ret | ((UINT32)current_setting);
  drp_sw_data->calib.rx.scf_pole.ANA_MIX_LB_IQ_table[band] = setting_ret;

  //trigger IDLE script
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
  WAIT_US(20);

  return;
}

/**************************************************************************************/
/* Functionality: SCF Pole Calibration CB1                                            */
/* Calibrates for 1 pole                                                */
/* The test equipment must supply the correct input signal for calibration.           */
/*                                                                                    */
/* This function writes the settings for                                              */
/* ANA_SCF_I_0(7:0), and ANA_SCF_Q_0(7:0) packed into                                 */
/* a 32-bit word as ANA_SCF_I_0(7:0) << 16 | ANA_SCF_Q_0(7:0)                         */
/**************************************************************************************/
void scf_pole_calibrationCB1(UINT16 band, UINT32* setting_ret) {
  UINT32 vpp, vpp_i, vpp_q;
  UINT16 amp_3_db_i, amp_3_db_q;
  UINT16 amp_curr_i, amp_curr_q;
  UINT16 curr_set_i, curr_set_q;
  UINT16 min_set_i,  min_set_q;
  UINT16 max_set_i,  max_set_q;
  UINT16 tmp_set_i,  tmp_set_q;
  SINT16 index, index1;

  // Perform RX ON to required timeline with full offset correction
  set_abe_gain(4);
  set_afe_gain(4,band);
  rx_on_calib_timeline(band,g_band_cntr_ch[band]);

  // Here we assume that the rotating capacitor values for I and Q
  // channels, are set to nominal values by the RX_ON script. It is
  // important that the RX_ON script values used for CR1 and CR2 by the
  // calibration script and the phone mode script be the same.
  // CR2 == ANA_SCF_I/Q_1(B8,B3:B0)
  // CR1 == ANA_SCF_I/Q_1(B7:B4) - Verify this, missing in CTL bits spreadsheet

  // CH in minimum capacitance settings
  WRITE_WORD(drp_regs->ANA_MIX_LB_IL,0x0001);
  WRITE_WORD(drp_regs->ANA_MIX_LB_QL,0x0001);

  //---- CB1 Pole -----------------------------------------------
  // CB1, and CB2 in minimum capacitance settings
  WRITE_WORD(drp_regs->ANA_SCF_I_0L, 0x0000);
  WRITE_WORD(drp_regs->ANA_SCF_Q_0L, 0x0000);

  // Get 3db value for I and Q
  vpp_i = 0;
  vpp_q = 0;
  // measure vpp 16 times and average
  for(index1=0; index1 < 16; index1++) {
    vpp = dbb_measure_total_gain();
    vpp_i += ((vpp >> 16) & 0x0000FFFF);
    vpp_q += (vpp & 0x0000FFFF);
  }
  amp_curr_i = (UINT16)((vpp_i >> 4) & 0x0000FFFF);  // A/16
  amp_curr_q = (UINT16)((vpp_q >> 4) & 0x0000FFFF);
  amp_3_db_i = (UINT16)((vpp_i * 725) >> 14);         // (A/16)/1.4124 ~= A*725/1024
  amp_3_db_q = (UINT16)((vpp_q * 725) >> 14);

  // Loop initial conditions
  curr_set_i  = 0x00;
  min_set_i   = 0x00;
  max_set_i   = 0x3F;
  curr_set_q  = 0x00;
  min_set_q   = 0x00;
  max_set_q   = 0x3F;

  // perform a derivative of binary search to get the CB1 setting
  for(index=0;index<6;index++) {

    if(amp_curr_i>amp_3_db_i) min_set_i = curr_set_i;
    else max_set_i = curr_set_i;

    if(amp_curr_q>amp_3_db_q) min_set_q = curr_set_q;
    else max_set_q = curr_set_q;

    // Curr = round ((Max + Min) / 2)
    curr_set_i = ((max_set_i + min_set_i + 1) >> 1);
    curr_set_q = ((max_set_q + min_set_q + 1) >> 1);

    tmp_set_i = curr_set_i << 2 ;
    tmp_set_q = curr_set_q << 2 ;

    // apply the setting
    WRITE_BYTE_LOW(drp_regs->ANA_SCF_I_0L,tmp_set_i);
    WRITE_BYTE_LOW(drp_regs->ANA_SCF_Q_0L,tmp_set_q);

    // measure amplitude
    // Average over 16 periods
    vpp_i = 0;
    vpp_q = 0;
    for(index1=0; index1 < 16; index1++) {
      vpp = dbb_measure_total_gain();
      vpp_i += ((vpp >> 16) & 0x0000FFFF);
      vpp_q += (vpp & 0x0000FFFF);
    }
    amp_curr_i = (UINT16)((vpp_i >> 4) & 0x0000FFFF);
    amp_curr_q = (UINT16)((vpp_q >> 4) & 0x0000FFFF);
  }

  if(amp_curr_i > amp_3_db_i) curr_set_i = max_set_i;
  if(amp_curr_q > amp_3_db_q) curr_set_q = max_set_q;

  curr_set_i = curr_set_i << 2 ;
  curr_set_q = curr_set_q << 2 ;

  // Try using 2 LSBs as needed.

  // apply the setting
  WRITE_BYTE_LOW(drp_regs->ANA_SCF_I_0L,curr_set_i);
  WRITE_BYTE_LOW(drp_regs->ANA_SCF_Q_0L,curr_set_q);

  // measure amplitude
  // Average over 16 periods
  vpp_i = 0;
  vpp_q = 0;
  for(index1=0; index1 < 16; index1++) {
    vpp = dbb_measure_total_gain();
    vpp_i += ((vpp >> 16) & 0x0000FFFF);
    vpp_q += (vpp & 0x0000FFFF);
  }
  vpp_i = (vpp_i >> 4);
  vpp_q = (vpp_q >> 4);

  amp_curr_i = (UINT16)(vpp_i & 0x0000FFFF);
  amp_curr_q = (UINT16)(vpp_q & 0x0000FFFF);

  if(amp_curr_i > amp_3_db_i) curr_set_i = curr_set_i | 0x02;
  if(amp_curr_q > amp_3_db_q) curr_set_q = curr_set_q | 0x02;


   // apply the setting
  WRITE_BYTE_LOW(drp_regs->ANA_SCF_I_0L,curr_set_i);
  WRITE_BYTE_LOW(drp_regs->ANA_SCF_Q_0L,curr_set_q);

  // measure amplitude 16 times and average
  vpp_i = 0;
  vpp_q = 0;
  for(index1=0; index1 < 16; index1++) {
    vpp = dbb_measure_total_gain();
    vpp_i += (vpp >> 16);
    vpp_q += (vpp & 0x0000FFFF);
  }
  vpp_i = (vpp_i >> 4);
  vpp_q = (vpp_q >> 4);

  amp_curr_i = (UINT16)(vpp_i & 0x0000FFFF);
  amp_curr_q = (UINT16)(vpp_q & 0x0000FFFF);

  if(amp_curr_i > amp_3_db_i) curr_set_i = curr_set_i | 0x01;
  if(amp_curr_q > amp_3_db_q) curr_set_q = curr_set_q | 0x01;

  // #@# add another req. test here and return(error) if setting doesn't work.

  // Save CB1 Values
  *setting_ret = *setting_ret | (((UINT32)curr_set_i) << 16);
  //*setting_ret = *setting_ret |  ((UINT32)curr_set_q);
  *setting_ret = *setting_ret |  ((UINT32)curr_set_i);

  //trigger IDLE script
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
  WAIT_US(20);

  return;
}

/**************************************************************************************/
/* Functionality: SCF Pole Calibration CB2                                            */
/* Calibrates for 1 pole (150/270 KHz)                                                */
/* The test equipment must supply the correct input signal for calibration.           */
/*                                                                                    */
/* This function writes the settings for                                              */
/* ANA_SCF_I_0(15:8), and ANA_SCF_Q_0(15:8) packed into                               */
/* a 32-bit word as ANA_SCF_I_0(15:8) << 16 | ANA_SCF_Q_0(15:8)                       */
/*                                                                                    */
/* This function also writes the calibrated pole settings into SRM memory and also in */
/* drp_sw_data structure.                                                             */
/**************************************************************************************/
void scf_pole_calibrationCB2(UINT16 band, UINT32* setting_ret, UINT16 pole_num) {
  UINT32 vpp, vpp_i, vpp_q;
  UINT16 amp_3_db_i, amp_3_db_q;
  UINT16 amp_curr_i, amp_curr_q;
  UINT16 curr_set_i, curr_set_q;
  UINT16 min_set_i,  min_set_q;
  UINT16 max_set_i,  max_set_q;
  UINT16 tmp_set_i,  tmp_set_q;
  SINT16 index, index1;

  // Perform RX ON to required timeline with full offset correction
  set_abe_gain(4);
  set_afe_gain(4,band);
  rx_on_calib_timeline(band,g_band_cntr_ch[band]);

  // Here we assume that the rotating capacitor values for I and Q
  // channels, are set to nominal values by the RX_ON script. It is
  // important that the RX_ON script values used for CR1 and CR2 by the
  // calibration script and the phone mode script be the same.
  // CR2 == ANA_SCF_I/Q_1(B8,B3:B0)
  // CR1 == ANA_SCF_I/Q_1(B7:B4) - Verify this, missing in CTL bits spreadsheet

  // CH in minimum capacitance settings
  WRITE_WORD(drp_regs->ANA_MIX_LB_IL,0x0001);
  WRITE_WORD(drp_regs->ANA_MIX_LB_QL,0x0001);

  //---- CB2 Pole -----------------------------------------------
  // CB1, and CB2 in minimum capacitance settings
  WRITE_WORD(drp_regs->ANA_SCF_I_0L, 0x0000);
  WRITE_WORD(drp_regs->ANA_SCF_Q_0L, 0x0000);

  // Get 3db value for I and Q
  // Average over 16 periods
  vpp_i = 0;
  vpp_q = 0;
  for(index1=0; index1 < 16; index1++) {
    vpp = dbb_measure_total_gain();
    vpp_i += ((vpp >> 16) & 0x0000FFFF);
    vpp_q += (vpp & 0x0000FFFF);
  }
  amp_curr_i = (UINT16)((vpp_i >> 4) & 0x0000FFFF);
  amp_curr_q = (UINT16)((vpp_q >> 4) & 0x0000FFFF);
  amp_3_db_i = (UINT16)(((vpp_i * 725) >> 14) & 0x0000FFFF);  // (A/16)/1.4124 ~= A*725/1024
  amp_3_db_q = (UINT16)(((vpp_q * 725) >> 14) & 0x0000FFFF);  // (A/16)/1.4124 ~= A*725/1024

  // Loop initial conditions
  curr_set_i  = 0x00;
  min_set_i   = 0x00;
  max_set_i   = 0x3F;
  curr_set_q  = 0x00;
  min_set_q   = 0x00;
  max_set_q   = 0x3F;

  // perform a derivative of binary search to get the CB2 setting

  for(index=0;index<6;index++) {

    if(amp_curr_i > amp_3_db_i) min_set_i = curr_set_i;
    else max_set_i = curr_set_i;

    if(amp_curr_q > amp_3_db_q) min_set_q = curr_set_q;
    else max_set_q = curr_set_q;

    // Curr = round ((Max + Min) / 2)
    curr_set_i = ((max_set_i + min_set_i + 1) >> 1);
    curr_set_q = ((max_set_q + min_set_q + 1) >> 1);

    // Value to set = currr << 10 ;
    tmp_set_i = curr_set_i << 10 ;
    tmp_set_q = curr_set_q << 10 ;

    // apply the setting
    WRITE_BYTE_HIGH(drp_regs->ANA_SCF_I_0L,tmp_set_i);
    WRITE_BYTE_HIGH(drp_regs->ANA_SCF_Q_0L,tmp_set_q);

    // measure amplitude
    // Average over 16 periods
    vpp_i = 0;
    vpp_q = 0;
    for(index1=0; index1 < 16; index1++) {
      vpp = dbb_measure_total_gain();
      vpp_i += ((vpp >> 16) & 0x0000FFFF);
      vpp_q += (vpp & 0x0000FFFF);
  }
    amp_curr_i = (UINT16)((vpp_i >> 4) & 0x0000FFFF);
    amp_curr_q = (UINT16)((vpp_q >> 4) & 0x0000FFFF);
  }

  // Check the result from the last setting (curr_set_iq).
  // Keep the current setting if it meets the requirement (it's less
  // than max_set), else use max_set.
  if(amp_curr_i > amp_3_db_i) curr_set_i = max_set_i;
  if(amp_curr_q > amp_3_db_q) curr_set_q = max_set_q;

  curr_set_i = curr_set_i << 10 ;
  curr_set_q = curr_set_q << 10 ;

  // The next code uses the 2 LSBs (The LSBs are evenly weighted).

  // two cases:
  //      CASE: max_set has met the requirement (is not 3f)
  //         curr_set will meet the requirement.
  //         Adding CAPs will increase the attenuation.
  //      CASE: max_set hasn't met the requirement (unchanged from 3f)
  //         max_set is 3f  curr_set is not FC00
  //         if curr_sett doens't meet the requirement
  //         add lsb CAPs until it does (or they all been tried).
  // #@# optimization is possible (e.g. lsb caps can be tested for best fit).

  // apply the setting
  WRITE_BYTE_HIGH(drp_regs->ANA_SCF_I_0L,curr_set_i);
  WRITE_BYTE_HIGH(drp_regs->ANA_SCF_Q_0L,curr_set_q);

  // measure amplitude. Average over 16 periods
  // check if current setting meets the requirements.
  vpp_i = 0;
  vpp_q = 0;
  for(index1=0;index1<16;index1++) {
    vpp = dbb_measure_total_gain();
    vpp_i += (vpp >> 16);
    vpp_q += (vpp & 0x0000FFFF);
  }
  amp_curr_i = (UINT16)((vpp_i >> 4) & 0x0000FFFF);
  amp_curr_q = (UINT16)((vpp_q >> 4) & 0x0000FFFF);

  // try bit 1 cap if it doesn't
  if(amp_curr_i > amp_3_db_i) curr_set_i = curr_set_i | 0x0200;
  if(amp_curr_q > amp_3_db_q) curr_set_q = curr_set_q | 0x0200;

   // apply the setting
  WRITE_BYTE_HIGH(drp_regs->ANA_SCF_I_0L,curr_set_i);
  WRITE_BYTE_HIGH(drp_regs->ANA_SCF_Q_0L,curr_set_q);

  // measure amplitude. Average over 16 periods
  // check the requirement.
  vpp_i = 0;
  vpp_q = 0;
  for(index1=0; index1 < 16; index1++) {
    vpp = dbb_measure_total_gain();
    vpp_i += (vpp >> 16);
    vpp_q += (vpp & 0x0000FFFF);
  }
  amp_curr_i = (UINT16)((vpp_i >> 4) & 0x0000FFFF);
  amp_curr_q = (UINT16)((vpp_q >> 4) & 0x0000FFFF);

  // use bit 0 cap if it doesn't
  if(amp_curr_i > amp_3_db_i) curr_set_i = curr_set_i | 0x0100;
  if(amp_curr_q > amp_3_db_q) curr_set_q = curr_set_q | 0x0100;

  // #@# add another req. test here and return(error) if setting doesn't work.

  // Save CB2 Values
  *setting_ret = *setting_ret | (((UINT32)curr_set_i >> 8) << 24);
  //*setting_ret = *setting_ret | (((UINT32)curr_set_q >> 8) << 8);
  *setting_ret = *setting_ret | (((UINT32)curr_set_i >> 8) << 8);

  //---- Save CB1 and CB2 -----------------------------------------------
  //drp_sw_data->calib.rx.scf_pole.ANA_SCF_IQ_0_table[(NUM_BANDS*pole_num) + band] = *setting_ret;

  //trigger IDLE script
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
  WAIT_US(20);

  return;
}

#endif //#if ((DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN)) || (!DRP_TEST_SW))

#if ((DRP_TEST_SW && !DRP_BURNIN) || (!DRP_TEST_SW))
/**************************************************************************************/
/* This is KDCO calibration function                                                  */
/**************************************************************************************/
void KDCO_calibration(UINT16 band, UINT16 txrx_mode)
{
  UINT16 mem_data, index, flag = 0;

 // initialize initial KDCO seed values
  if(((drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[0] & 0x0001) == 0x0001) &&
     ((drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[2] & 0xC000) == 0x4000) &&
     ((drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[1] & 0x8000) == 0x8000)) {
    if (txrx_mode == RX_MODE) {
      // RX
      if (band == GSM_BAND) {
        // GSM850 band in RX mode is treated as GSM900 band
        WRITE_WORD(drp_srm_data->tx.kdco_t[EGSM_BAND], 0x0800);
      } else {
        if (band == EGSM_BAND) {
          WRITE_WORD(drp_srm_data->tx.kdco_t[EGSM_BAND], 0x0680);
        } else {
          if (band == DCS_BAND) {
            // DCS band in RX mode is treated as PCS band
            WRITE_WORD(drp_srm_data->tx.kdco_t[PCS_BAND],0x0730);
          } else {
            // PCS
            WRITE_WORD(drp_srm_data->tx.kdco_t[PCS_BAND],0x0640);
          }
        }
      }
    } else {
      // TX
      if (band == GSM_BAND) {
        WRITE_WORD(drp_srm_data->tx.kdco_t[band], 0x0800);
      } else {
        if (band == EGSM_BAND) {
          WRITE_WORD(drp_srm_data->tx.kdco_t[band], 0x0680);
        } else {
          if (band == DCS_BAND) {
            // DCS band in RX mode is treated as PCS band
            WRITE_WORD(drp_srm_data->tx.kdco_t[band], 0x0730);
          } else {
            // PCS
            WRITE_WORD(drp_srm_data->tx.kdco_t[band], 0x0640);
          }
        }
      }
    }
  } else {
  //initialize initial KDCO values in SRM to 0x0700
  if (txrx_mode == TX_MODE){
    if (band == GSM_BAND)
    {
      WRITE_WORD(drp_srm_data->tx.kdco_t[band],0x07D0);
    }
    else
    {
    WRITE_WORD(drp_srm_data->tx.kdco_t[band],0x0700);
  }
  }
  else
  {
    if (band == GSM_BAND)
    {
      WRITE_WORD(drp_srm_data->tx.kdco_t[EGSM_BAND],0x07D0);
    }
    else if (band == EGSM_BAND)
    {
      WRITE_WORD(drp_srm_data->tx.kdco_t[EGSM_BAND],0x0700);
    }
    else
    {
      WRITE_WORD(drp_srm_data->tx.kdco_t[PCS_BAND],0x0700);
  }
  }
  }

  for (index = 0; index < 8; index++)
  {
    //do not write the updated KDCO values for last 4 runs, these are just averaged
    if ((index > 0) && (index < 4))
    {
      if (txrx_mode == TX_MODE)
    {
        WRITE_WORD(drp_srm_data->tx.kdco_t[band],mem_data);
      }
      else
      {
        if((band == GSM_BAND) || (band == EGSM_BAND))
        {
          WRITE_WORD(drp_srm_data->tx.kdco_t[EGSM_BAND],mem_data);
        }
        else
        {
          WRITE_WORD(drp_srm_data->tx.kdco_t[PCS_BAND],mem_data);
    }
      }
    }

    WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0081);

    WAIT_US(200);

    READ_WORD(drp_regs->RCAL_TL, mem_data);

    // Check KDCO value for convergence
    // If first time of non-convergence, then set flag and restart again with orginal seed
    if(((mem_data < 0x03B0) || (mem_data > 0x0A30)) && (flag == 0)) {
      if (band == GSM_BAND) {
        mem_data = 0x07D0;
      }
      else {
        mem_data = 0x0700;
      }
      flag = 1;
      index = 0;
    }

    //trigger the idle script
    WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
    WAIT_US(20);
  }

  //initializa SRM with calibrated KDCO values
  if (txrx_mode == TX_MODE)
  {
    drp_sw_data->calib.txrx_common.kdco_inv[band] = mem_data;
  }
  else
  {
    drp_sw_data->calib.txrx_common.kdco_inv[band + NUM_BANDS] = mem_data;
  }
}


/**************************************************************************************/
/* Common Function                                                                    */
/**************************************************************************************/
/**************************************************************************************/
/* This function does an RX ON                                                        */
/**************************************************************************************/
void rx_on_calib_timeline(UINT16 band, UINT16 channel) {

  //UINT16 rxon_input_saved, temp16;
  //UINT16 dbbif_setting;

  //DBBIF setting generation and passing to rxon_input
  //READ_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);
  //temp16 = rxon_input_saved & (~0x0300);
  //dbbif_setting = drp_generate_dbbif_setting(g_pcb_config, band);
  //temp16 = (temp16 & (~0x0300)) | ((dbbif_setting & 0x3) << 8);
  //WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

  dbb_write_rffreq(band, channel, RX_MODE);

  // Trigger RX_ON script
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0082);

  // #@# reduce to ~175 us
  // wait fo RX_ON script to finish.
  wait_qb(271);   // ~250us

  // Enable writing to ARX (RX_ON script disables ARX Bus
  WRITE_WORD(drp_regs->SRM_CWL, 0x0340);

  //WRITE_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);

}
#endif //#if ((DRP_TEST_SW && !DRP_BURNIN) || (!DRP_TEST_SW))

#if ((DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT)) || (!DRP_TEST_SW))
/**************************************************************************************/
/* Function Name: calc_log                                                            */
/* Input: num - an unsigned short integer whose log needs to be computed              */
/* Return: returns the log base(2) of (num) scaled by 1024                            */
/* Uses a piecewise linear approximation using 32 linear segments.                    */
/**************************************************************************************/
SINT16 calc_log(UINT16 num)
{
  UINT16 count = 0;
  UINT16 intlog_x, fraclog_x;
  UINT16 index,quantized_num;
  UINT16 *log_table = &drp_sw_data->log_const.logarithm_table[0];


  if (num == 0)
    return 0;

  // Find the integer part of the logarithm and scale the number to be
  // between 2^15 1nd 2^16

  while (num < CONST_2_15)
  {
    count++;
    num = num << 1;
  }

  intlog_x = (15 - count) << LOG_SCALE;

  // Now divide the interval 2^15 - 2^16 into 32 segments, find the index of the
  // segment where the scled input - num, lies.
  num = num - CONST_2_15;
  index = num >> LOG_SHIFT;
  quantized_num = index << LOG_SHIFT;

  // construct the fractional part of the log using linear interpolation of data in log_table[]

  fraclog_x = ((log_table[index + 1] - log_table[index]) * (num - quantized_num)) >> LOG_SHIFT;

  fraclog_x = fraclog_x + log_table[index];

  return (SINT16)(intlog_x + fraclog_x);
}

/**************************************************************************************/
/* This function sets the LNA resonator setting based on the index in ANA_LNA_XB(11:8)*/
/**************************************************************************************/
void dbb_set_LNA_resonator_setting(UINT16 index)
{
  UINT16 mem_data;

  READ_WORD(drp_regs->ANA_LNA_XBL, mem_data);
  mem_data = (mem_data & 0xF0FF) | (index << 8);
  WRITE_WORD(drp_regs->ANA_LNA_XBL, mem_data);
}

/**************************************************************************************/
/* This function turns on the Q reducing resistor in LNA.                             */
/* bit(13:12) of ANA_LNA_XB, control this functionality                               */
/* For index = 0, bit(13:12) = 00                                                     */
/*     index = 1, bit(13:12) = 01                                                     */
/*     index = 2, bit(13:12) = 11                                                     */
/**************************************************************************************/
void dbb_enable_LNA_Qreducingresistor(UINT16 index) {
  UINT16 mem_data;

  index = index & 0x3;

  READ_WORD(drp_regs->ANA_LNA_XBL, mem_data);
  mem_data = mem_data & ~(0x3 << 12);
  mem_data = mem_data | (index << 12);
  WRITE_WORD(drp_regs->ANA_LNA_XBL, mem_data);

  return;
}

#endif //#if ((DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT)) || (!DRP_TEST_SW))

#if ((DRP_TEST_SW && (!DRP_BURNIN)) || (!DRP_TEST_SW))
/**************************************************************************************/
/* This function writes the appropriate band information into RF_FREQ register        */
/**************************************************************************************/
void dbb_write_rffreq(UINT16 band, UINT16 channel, UINT16 txrx_mode) {
  UINT16 startfreq, rf_freq;

  if(txrx_mode == TX_MODE) {
    switch (band) {
    case 0:
        startfreq = (UINT16) EGSM_TX_START;
    break;
    case 1:
        startfreq = (UINT16) GSM_TX_START;
    break;
    case 2:
        startfreq = (UINT16) DCS_TX_START;
    break;
    case 3:
    default:
        startfreq = (UINT16) PCS_TX_START;
  }
}
  else {
    switch (band) {
    case 0:
        startfreq = (UINT16) EGSM_RX_START;
    break;
    case 1:
        startfreq = (UINT16) GSM_RX_START;
    break;
    case 2:
        startfreq = (UINT16) DCS_RX_START;
    break;
    case 3:
    default:
        startfreq = (UINT16) PCS_RX_START;
    }
  }

  rf_freq = startfreq + (channel << 1);
  WRITE_WORD(drp_regs->RF_FREQL, rf_freq);
}

/**************************************************************************************/
/* Common Function                                                                    */
/**************************************************************************************/
/**************************************************************************************/
/*  This function applies the ABE gain based on the index                             */
/**************************************************************************************/
void set_abe_gain(UINT16 cta_gain_index)
{
  UINT16 cta_gain_0, cta_gain_1;
  UINT16 abe_meas_input_strength;

  //get the gain word for stage 0
  cta_gain_0 = drp_sw_data->rx_const.abe.gain_table_0[cta_gain_index];

  //get the gain word for stage 1
  cta_gain_1 = drp_sw_data->rx_const.abe.gain_table_1[cta_gain_index];

  // write CTA gain settings
  WRITE_WORD(drp_regs->ANA_CTA_I_0L, cta_gain_0);
  WAIT_US(1);
  WRITE_WORD(drp_regs->ANA_CTA_I_1L, cta_gain_1);

  WRITE_WORD(drp_regs->ANA_CTA_Q_0L, cta_gain_0);
  WAIT_US(1);
  WRITE_WORD(drp_regs->ANA_CTA_Q_1L, cta_gain_1);

  // write the strength of the input signal to use for abe gain measurement.
  wait_qb(1);
  abe_meas_input_strength = drp_sw_data->rx_const.abe.abe_meas_input_strength[cta_gain_index];
  WRITE_WORD(drp_srm_data->abe_gain_comp_index, abe_meas_input_strength);
}

/**************************************************************************************/
/* Common Function                                                                    */
/**************************************************************************************/
/**************************************************************************************/
/*  This function applies the AFE gain.                                               */
/*  It reads the afe gain entry form the afe gain table.                              */
/*  The entry has the following format:                                               */
/*             high_byte = lna gain setting                                           */
/*             low_byte = ta gain setting                                             */
/*  This function sets the low byte of the ANA_LNA_XB register to the                 */
/*  specified LNA gain. It sets the high byte of ANA_LNA_XB to calibrated             */
/*  lna center frequency setting (based on the band).                                 */
/*                                                                                    */
/*  This function sets the low byte of the ANA_TA_XB register to the                  */
/*  specified TA gain (it does a read-modify-write to preserve the                    */
/*  non-gain bits).                                                                   */
/**************************************************************************************/

void set_afe_gain(UINT16 afe_gain_index, UINT16 band) {
  UINT16 afe_gain, lna_gain, ta_gain, mem_data;

  //Extract the AFE gain word corresponding to afe_gain_index
  afe_gain = drp_sw_data->rx_const.afe.gain_table[afe_gain_index];

  // get the LNA gain (upper 8 bits) and the center freq setting.
  lna_gain = afe_gain >> 8;
  lna_gain |= drp_sw_data->calib.rx.lna.cfreq[band];
  // get the TA gain word (lower 8 bits)
  ta_gain = afe_gain & 0xFF;

  // write afe settings.

  WRITE_WORD(drp_regs->ANA_LNA_XBL, lna_gain);

  // do read-modify-write to preserve the TA non-gain bits.
  READ_WORD(drp_regs->ANA_TA_XB_IL, mem_data);
  mem_data = (mem_data & 0xFFE0) | ta_gain;
  WRITE_WORD(drp_regs->ANA_TA_XB_IL, mem_data);

  READ_WORD(drp_regs->ANA_TA_XB_QL, mem_data);
  mem_data = (mem_data & 0xFFE0) | ta_gain;
  WRITE_WORD(drp_regs->ANA_TA_XB_QL, mem_data);

}

#endif //#if ((DRP_TEST_SW && (!DRP_BURNIN)) || (!DRP_TEST_SW))

#if ((DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT)) || (!DRP_TEST_SW))
/**************************************************************************************/
/* This function measures the total RX gain of DRP                                    */
/* The return value is a UINT32.                                                      */
/* The high 16-bits returned is the I Channel gain.                                   */
/* The low 16-bits returned is the Q Channel gain.                                    */
/**************************************************************************************/
UINT32 dbb_measure_total_gain(void) {
  UINT16 mem_data;
  SINT16 minI, minQ, maxI, maxQ;
  UINT16 gainI, gainQ;
  UINT32 gain_ret;
  UINT16 dcest_save;

  // Set DCEST Block for no IIR. (Save value and restore later).
  READ_WORD (drp_regs->DCEST_CWL, dcest_save);
  WRITE_WORD(drp_regs->DCEST_CWL, 0x0019);

  //wait for the signal to settle
  wait_qb(54);   // ~50us

  //Set the min-max reset bit in DCEST_CW (keep IIR settings untouched)
  WRITE_BYTE_LOW(drp_regs->DCEST_CWH, 0x0008);
  wait_qb(1);
  //clear the min-max reset bit in DCEST_CW
  WRITE_BYTE_LOW(drp_regs->DCEST_CWH, 0x0000);

  // wait for about 1.5 cycles of the signal (assume signal is 100KHz)
  wait_qb(17);  // ~16 us
  // #@# delete the the 2 lines
  // #@# wait for longer so we can have a low freq signal
  //wait_qb(93);  // ~86 us

  //set the freeze bit in DCEST_CW
  WRITE_BYTE_LOW(drp_regs->DCEST_CWH, 0x0004);
  wait_qb(1);

  READ_WORD(drp_regs->DCEST_MIN_OUTH, mem_data);
  minI = (SINT16)mem_data;
  READ_WORD(drp_regs->DCEST_MIN_OUTL, mem_data);
  minQ = (SINT16)mem_data;

  READ_WORD(drp_regs->DCEST_MAX_OUTH, mem_data);
  maxI = (SINT16)mem_data;
  READ_WORD(drp_regs->DCEST_MAX_OUTL, mem_data);
  maxQ = (SINT16)mem_data;

  //clear the freeze bit in DCEST_CW
  WRITE_BYTE_LOW(drp_regs->DCEST_CWH, 0x0000);

  // restore DCEST IIR settings
  WRITE_WORD(drp_regs->DCEST_CWL, dcest_save);

  gainI = maxI - minI;
  gainQ = maxQ - minQ;
  gain_ret = ((UINT32)gainI) << 16;
  gain_ret = gain_ret | ((UINT32)gainQ & 0x0000FFFF);

  return(gain_ret);
}

/****************************************************************************/
// This function measures the total Vpp of the I channel 16 times
// and returns teh accumulated sum
/****************************************************************************/
UINT32 dbb_measure_total_gain_16x_i(void) {
  SINT16 index;
  UINT32 vpp_sum;

  vpp_sum = 0;
  for(index=0;index<16;index++) vpp_sum+=(dbb_measure_total_gain() >> 16);
  return(vpp_sum);
}

/****************************************************************************/
// This function measures the total Vpp of the Q channel 16 times
// and returns teh accumulated sum
/****************************************************************************/
UINT32 dbb_measure_total_gain_16x_q(void) {
  SINT16 index;
  UINT32 vpp_sum;

  vpp_sum = 0;
  for(index=0;index<16;index++) vpp_sum+=(dbb_measure_total_gain() & 0x0000FFFF);
  return(vpp_sum);
}

// #@# not used yet.
/****************************************************************************/
// This function measures the total RX gain of DRP by averaging
// the results of 16 measurements.
// The high 16-bits returned is the I Channel gain.
// The low 16-bits returned is the Q Channel gain.
/****************************************************************************/
UINT32 dbb_measure_total_gain_16x(void) {
  SINT16 index;
  UINT32 vpp_sum;
  UINT32 vpp_sum_i;
  UINT32 vpp_sum_q;

  vpp_sum_i = 0;
  vpp_sum_q = 0;
  for(index=0;index<16;index++) {
    vpp_sum = dbb_measure_total_gain();
    vpp_sum_i += ((vpp_sum >> 16) & 0x0000FFFF);
    vpp_sum_q += (vpp_sum & 0x0000FFFF);
  }
  // round
  vpp_sum_i+=8;
  vpp_sum_q+=8;
  vpp_sum_i = vpp_sum_i >> 4;
  vpp_sum = (vpp_sum_i << 16) | (vpp_sum_q >> 4);
  return(vpp_sum);
}

#if 0
/**************************************************************************************/
//  This function resets the bit 0 of the register reg
/**************************************************************************************/
void dbb_disable(volatile UINT16 * reg)
{
  UINT16 mem_data;

  READ_WORD(*reg, mem_data);

  mem_data = mem_data & 0xFFFE;

  WRITE_WORD(*reg, mem_data);
}
#endif

#if DRP_PHASE2_CALIBRATION

/**************************************************************************************/
// Temperature sensor calibration function
/**************************************************************************************/
void temp_sensor_calibration(SINT16 calib_temp) {
  UINT16 mem_data, adc_code,table_index;
  SINT16 temp_data;
  UINT16 index;
  SINT16 process_strength, process_strength_1;
  SINT16 temp_weak,temp_strong;


  // start read_temperature script.
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0083);

  // Wait for temperature script to run.
  wait_qb(27);  // ~25 us

//  mem_data = *(volatile ioport UINT16 *)(DRP_DRP_OFFSET + (DRP_MEAS_TEMP >> 1));
  READ_WORD(drp_srm_api->inout.temperature.output,mem_data)

  // high byte is adc code; low byte is temp degrees C.
  //#@# temp is not compliant with API - need to change API or change script.
  //#@# for now mask the ADC_CODE (HIGH BYTE).
  adc_code = (mem_data >> 8);

  temp_weak   = drp_sw_data->simulation.temperature.temp_code_weak[adc_code];
  temp_strong = drp_sw_data->simulation.temperature.temp_code_strong[adc_code];

    // calib_temp is the calibration temperature input as a parameter
    // to this calibration routine

    // compute the process strength as: (calib_temp - temp_strong)/(temp_weak - temp_strong)

    temp_data = (calib_temp - temp_strong) << 8;
    process_strength = temp_data / (temp_weak - temp_strong);
    process_strength_1 = (1 << 8) - process_strength;

    // Now linearly interpolate the strong and weak process temperature tables
  for (index = 0; index < DRP_NUM_TEMPERATURE; index++) {
      temp_data = drp_sw_data->simulation.temperature.temp_code_weak[index] * process_strength;
      temp_data += drp_sw_data->simulation.temperature.temp_code_strong[index] * process_strength_1;
      drp_sw_data->calib.temperature.temperature[index] = (SINT8)(temp_data >> 8);
    }
  }

#endif // DRP_PHASE2_CALIBRATION

#if DRP_FLYBACK_IBIAS_CALIB

/**************************************************************************************/
/* This is DCO tuning word retiming alignment calibration function                    */
/**************************************************************************************/
void dco_retiming_calibration(void) {

  volatile UINT16 indx, indx1;
  volatile UINT16 min_flyback;
  volatile UINT16 rf_freq, mem_data;
  volatile UINT32 perinv, perinv_high, perinv_low;
  volatile UINT32 sum_32;
  volatile UINT32 min_avg;
  volatile UINT32 rms_array[NUM_FLYBACK_DELAY];

  for(indx=0;indx < (NUM_BANDS * NUM_FLYBACK_SUBBANDS);indx++){

    rf_freq = drp_sw_data->calib.txrx_common.subband_rffreq[indx];

    // apply max I bias current
    // dbb_apply_ibias(0x7F);

    rms_phe_compute((UINT16)rf_freq, (UINT32 *)rms_array, 0, FLYBACK_SELECT);
    min_avg = 0x0000ffff;
    for(indx1=1;indx1 < (NUM_FLYBACK_DELAY-1);indx1++){
      sum_32 = (rms_array[indx1-1]/5) + (rms_array[indx1+1]/5) + (rms_array[indx1]*3/5);
      if(sum_32 < min_avg){
        min_avg = sum_32;
        min_flyback = indx1;
      }
    }

    drp_sw_data->calib.txrx_common.ckv_dly[indx] = min_flyback;
    //read bits (12:0) of DLO_PERINV register
    READ_WORD(drp_regs->DLO_PERINVL, mem_data);

    perinv = (UINT32) (mem_data & 0x1FFF);

    // for PCS band thresholds at +- 1.7%
    if((indx == 9) || (indx == 10) || (indx == 11))
  {
      perinv_high = perinv * 1017/1000;
      perinv_low = perinv * 983/1000;
    }
    else{  // for DCS band and LB thresholds at +- 1.5%
      perinv_high = perinv * 1015/1000;
      perinv_low = perinv * 985/1000;
  }

    drp_sw_data->calib.txrx_common.perinv_thresh[indx][0] = (UINT16) (perinv_low);
    drp_sw_data->calib.txrx_common.perinv_thresh[indx][1] = (UINT16) (perinv);
    drp_sw_data->calib.txrx_common.perinv_thresh[indx][2] = (UINT16) (perinv_high);

  }

  return;

}

#if DRP_TEST_SW

/**************************************************************************************/
/* This is DCO tuning word retiming alignment calibration function                    */
/**************************************************************************************/
void dco_retiming_data_collection(UWORD16 rf_freq, UWORD16 extended_tx_enable) {

  volatile UINT32 rms_array[NUM_FLYBACK_DELAY];

  // apply max I bias current
  // dbb_apply_ibias(0x7F);

  rms_phe_compute((UINT16)rf_freq, (UINT32 *)rms_array, extended_tx_enable, FLYBACK_SELECT);

  return;

}

#endif // #if DRP_TEST_SW

#endif // #if DRP_FLYBACK_IBIAS_CALIB

/**************************************************************************************/
/* This is RX ABE Gain calibration function                                           */
/**************************************************************************************/
void rxgain_abe_calibration(UINT16 band) {
  UINT16 abe_gain_index, vpp, gain_comp_offset;
  SINT16 temp, gainlog2, gain_n, alpha;

  for (abe_gain_index = 0; abe_gain_index < NUM_ABE_GAINS; abe_gain_index++) {

    dbb_run_temp();
    // #@# need to determine how long the temperature script takes to run.
    wait_qb(100);
    temp = dbb_read_temp();

#if _DEBUG
    measured_temp[abe_gain_index] = temp;
#endif


    //measure the ABE gain
    vpp = dbb_meas_abe_vpp(band, abe_gain_index);

    // calculate gain as follows.
    // gain_dB = 20*log10(Vpp_out/Vpp_in) + SCF_CAL_LOSS
    //         = 20*log10(Vpp_out) - 20*log10(Vpp_in) + SCF_CAL_LOSS
    //
    // Input signal is from -2^abe_meas_input_strength to +2^abe_meas_input_strength
    // i.e., Vpp_in is 2^(abe_meas_input_strength + 1)
    //
    // Since we are taking log2, the second term in the above expression becomes
    //      2 * (2^LOG_SCALE) * log2(2^(abe_meas_input_strength + 1))
    // or   2 * (2^LOG_SCALE) * (abe_meas_input_strength + 1)
    // or   (abe_meas_input_strength + 1) << (LOG_SCALE + 1)
    //
    // Note: calc_log(num) will return (2^LOG_SCALE)*log2(num)

    gain_comp_offset = (drp_sw_data->rx_const.abe.abe_meas_input_strength[abe_gain_index] + 1) << (LOG_SCALE + 1);

    gainlog2 = 2*calc_log(vpp) - gain_comp_offset + SCF_CAL_LOSS;

    //find the temperature coefficient
    alpha = dbb_find_temp_coeff(abe_gain_index, gainlog2);

    //normalize the gain to TREF
    gain_n = dbb_normalize_gain(gainlog2, alpha, temp);

    //force the last two gain values to be -12dB to prevent RSSI errors
    if (abe_gain_index >= (NUM_ABE_GAINS - 2))
      gain_n = -4096;

    //store the calibrated data, i.e. normalized gain and alpha values
    drp_sw_data->calib.rx.abe.gain_alpha_table[abe_gain_index] = ((UINT32) gain_n << 16) | (alpha & 0xFFFF);
  }

  return;
}

/**************************************************************************************/
/* This is RX AFE Gain calibration function                                           */
/**************************************************************************************/
void rxgain_afe_calibration(UINT16 step, UINT16 band) {
  UINT16 afe_gain_index, vpp, gain_comp_offset, temp16, pole_num;
  SINT16 temp, abe_gainlog2, total_gainlog2, afe_gainlog2, afe_gain_n;
  SINT16 *afe_calib_table_ptr;
  UINT32 vpp32;

  afe_calib_table_ptr = &drp_sw_data->calib.rx.afe.gain[band][0];

  //Apply a -60dBm signal at the LNA input for all steps 0-31

  // Set the SCF pole index
  pole_num = 0;
  READ_WORD(drp_srm_api->inout.rx.rxon_input, temp16);
  temp16 = (temp16 & ~0x0080) | ((pole_num & 0x1) << 7);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

  for(afe_gain_index = step; afe_gain_index < step + 14; afe_gain_index++) {
    dbb_run_temp();
    // #@# need to determine how long the temperature script takes to run.
    wait_qb(100);
    temp = dbb_read_temp();

#if _DEBUG
    measured_temp[afe_gain_index] = temp;
#endif

    // measure the ABE gain          ABE Gain Setting
    if (step < 14)
      vpp = dbb_meas_abe_vpp(band, 4);   // set CTA gain to 17dB +/-1.5dB
    else
      vpp = dbb_meas_abe_vpp(band, 1);   // set CTA gain to 26dB +/-1.5dB


    // gain_dB = 20*log10(Vpp_out/Vpp_in) + SCF_CAL_LOSS
    //         = 20*log10(Vpp_out) - 20*log10(Vpp_in) + SCF_CAL_LOSS
    //
    // Input signal is from -2^abe_meas_input_strength to +2^abe_meas_input_strength
    // i.e., Vpp_in is 2^(abe_meas_input_strength + 1)
    //
    // Since we are taking log2, the second term in the above expression becomes
    //      2 * (2^LOG_SCALE) * log2(2^(abe_meas_input_strength + 1))
    // or   2 * (2^LOG_SCALE) * (abe_meas_input_strength + 1)
    // or   (abe_meas_input_strength + 1) << (LOG_SCALE + 1)
    //
    // Note: calc_log(num) will return (2^LOG_SCALE)*log2(num)
    if (step < 14)
    gain_comp_offset = (drp_sw_data->rx_const.abe.abe_meas_input_strength[4] + 1) << (LOG_SCALE + 1);
    else
      gain_comp_offset = (drp_sw_data->rx_const.abe.abe_meas_input_strength[1] + 1) << (LOG_SCALE + 1);

    abe_gainlog2 = 2*calc_log(vpp) - gain_comp_offset + SCF_CAL_LOSS;

    if (step < 14)
      set_abe_gain(4);
    else
      set_abe_gain(1);

    set_afe_gain(afe_gain_index, band);
    rx_on_calib_timeline(band, g_band_cntr_ch[band]);

    vpp32 = dbb_measure_total_gain_16x_i();
    vpp = (UINT16)((vpp32 >> 4) & 0x0000FFFF);

    // Trigger IDLE script
    WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
    wait_qb(22);   // ~20us

    // For a -60dBm input signal, amplitude of the signal is sqrt(2*50*10^(dBm-30)/10)
    // i.e. signal amplitude is 316.223uV, so peak to peak signal amplitude is 632.455uV
    //
    // The peak to peak signal amplitude (Vpp) determined from DCEST block is in 16 bit format
    // Signal amplitude (in volts) = Vpp x 0.9/(2^15 - 1)
    //
    // Total gain (in dB) = 20*log10(Vpp x 0.9/32767) - 20*log10(632.455e-6)
    //                    = 20*log10(Vpp) - 20*log10(632.455e-6 x 32767/0.9)
    //                    = 20*log10(Vpp) - 20*log10(23.0263)

    total_gainlog2 = 2*calc_log(vpp) - LOG_60DBM_OFFSET;   // -60dBm input used for all steps 0-31

    afe_gainlog2 = total_gainlog2 - abe_gainlog2;

    //normalize the afe gain to TREF
    afe_gain_n = dbb_normalize_gain(afe_gainlog2, (SINT16) AFE_ALPHA, temp);

    //store the calibrated data
    afe_calib_table_ptr[afe_gain_index] = afe_gain_n;
  }

  return;
}

/**************************************************************************************/
/* This function updates the deQ settings for LNA based on EFUSE                      */
/*                                                                                    */
/* EFUSE1(25:23) control LNA deQ settings                                               */
/*             Number of deQ bits in band                                             */
/*         GSM850   EGSM900   DCS1800   PCS1900                                       */
/* "000" -    0        0         0         0                                          */
/* "001" -    1        1         0         0                                          */
/* "010" -    1        0         0         0                                          */
/* "011" -    0        1         0         0                                          */
/*  100  -    1        1         1         0                                          */
/*  101  -    1        1         0         1                                          */
/*  110  -    1        1         1         1                                          */
/*  111  -    2        2         1         1                                          */
/*                                                                                    */
/* For Test porposes, DRP memory 0x2304 is treated as EFUSE1 register                 */
/**************************************************************************************/
void drp_efuse_update_deq(UINT16 deqSetting)
{
  UINT16 temp16, mem_data, band;

  temp16 = deqSetting;

  for (band = 0; band < NUM_BANDS; band++)
  {
  READ_WORD(drp_srm_data->rx.cfreq[band], mem_data);

  switch (temp16)
  {
    case 0:
      //don't do anything
    break;

    case 1:
      if ((band == GSM_BAND) || (band == EGSM_BAND))
        mem_data = (mem_data & 0xCFFF) | 0x1000;
    break;

    case 2:
      if (band == GSM_BAND)
        mem_data = (mem_data & 0xCFFF) | 0x1000;
    break;

    case 3:
      if (band == EGSM_BAND)
        mem_data = (mem_data & 0xCFFF) | 0x1000;
    break;

    case 4:
      if ((band == GSM_BAND) || (band == EGSM_BAND) || (band == DCS_BAND))
        mem_data = (mem_data & 0xCFFF) | 0x1000;
    break;

    case 5:
      if ((band == GSM_BAND) || (band == EGSM_BAND) || (band == PCS_BAND))
        mem_data = (mem_data & 0xCFFF) | 0x1000;
    break;

    case 6:
      if ((band == GSM_BAND) || (band == EGSM_BAND) || (band == DCS_BAND) || (band == PCS_BAND))
        mem_data = (mem_data & 0xCFFF) | 0x1000;
    break;

    case 7:
      if ((band == GSM_BAND) || (band == EGSM_BAND))
        mem_data = (mem_data & 0xCFFF) | 0x3000;
      else
        mem_data = (mem_data & 0xCFFF) | 0x1000;
    break;
  }

  WRITE_WORD(drp_srm_data->rx.cfreq[band], mem_data);
  WRITE_WORD(drp_sw_data_calib.calib.rx.lna.cfreq[band], mem_data);
  }

  return;
}

#if DRP_FLYBACK_IBIAS_CALIB

/**************************************************************************************/
/* This is DCO tuning word retiming alignment calibration function                    */
/**************************************************************************************/
void dco_ibias_calibration(void) {
  volatile UINT16 indx, indx1;
  volatile UINT16 min_ibias;
  volatile UINT16 rf_freq;
  volatile UINT32 min_rms;
  volatile UINT16 ibias_index[NUM_DCO_IBIAS] = {4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,100,110,127};
  volatile UINT16 min_indx, mem_data;
  volatile UINT32 rms_array[NUM_DCO_IBIAS];

  for(indx=0;indx < (NUM_BANDS * NUM_FLYBACK_SUBBANDS);indx++){

    rf_freq = drp_sw_data->calib.txrx_common.subband_rffreq[indx];
    dbb_apply_ckv_delay(drp_sw_data->calib.txrx_common.ckv_dly[indx]);

    rms_phe_compute((UINT16)rf_freq, (UINT32 *)rms_array, 0, IBIAS_SELECT);
    min_rms = 0x0000ffff;
    min_indx = 0;
    for(indx1=0;indx1 < NUM_DCO_IBIAS;indx1++){
      if(rms_array[indx1] < min_rms){
        min_rms = rms_array[indx1];
   min_indx = indx1;
      }
    }

    dbb_apply_ibias(ibias_index[min_indx]);
    READ_WORD(drp_regs->ANA_DCOH, mem_data);

    mem_data = (mem_data & 0x01FC) >> 2;
    drp_sw_data->calib.txrx_common.ibias[indx] = mem_data;

  }

}

#if DRP_TEST_SW

/**************************************************************************************/
/* This is DCO tuning word retiming alignment calibration function                    */
/**************************************************************************************/
void dco_ibias_data_collection(UWORD16 rf_freq, UWORD16 extended_tx_enable) {

  volatile UINT32 rms_array[NUM_DCO_IBIAS];

  rms_phe_compute((UINT16)rf_freq, (UINT32 *)rms_array, extended_tx_enable, IBIAS_SELECT);
  return;

}

#endif

#if 1
/**************************************************************************************/
/* Compute the RMS phase error routine                                                                           */
/**************************************************************************************/
void rms_phe_compute(UINT16 rf_freq, UINT32 *rms_array_ptr, UINT16 extended_tx_enable, UINT16 flyback_ibias_select) {
  volatile UINT16 indx1, indx2;
  volatile UINT16 indxavg,indx;
  volatile UINT16 cnt_burst;
  volatile UINT16 txrx_mode;
  volatile UINT16 temp16;
  volatile UINT32 mean_phe;
  volatile SINT32 mean_phe32;
  volatile SINT32 scr_mean_32;
  volatile UINT32 sum_32;
  volatile UINT32 scr_sum_32;
  volatile UINT16 array_size;
  volatile UINT16 ibias_index[NUM_DCO_IBIAS] = {4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,100,110,127};
  volatile UINT32 rms_array[NUM_DCO_IBIAS];
  volatile UINT32 mse_array[NUM_DCO_IBIAS];
  volatile UINT32 variance[NUM_DCO_IBIAS], std_dev[NUM_DCO_IBIAS];
  volatile UINT16 mem_data;
  volatile UINT32 mem_data32;
  volatile UINT16 tx_burst1[TX_BUFFER_LENGTH] = {0x4f40,0x5746,0x7390,0x83a6,0x8874,0x9c74,0x4ca0,0x9b5b,0xa9d7,0xfff0};
  volatile SINT16 phe_samples[NUM_PHE_SAMPLES];
  volatile UINT32 *calc_ptr;
  volatile UINT16 data_prev;
  volatile UINT32 temp32;
  volatile UINT16 burst_shift;
  volatile UINT32 *ptr_srm;
  volatile UINT32 srm_data_saved[SRM_DATA_SAVED_LEN];
#if FLYBACK_DEBUG
  volatile UINT32 dbg_buffer[PHE_AVG_CNT];
#endif

  if(flyback_ibias_select == FLYBACK_SELECT)
    array_size = NUM_FLYBACK_DELAY;

  if(flyback_ibias_select == IBIAS_SELECT)
    array_size = NUM_DCO_IBIAS;

  txrx_mode = TX_MODE;
  WRITE_WORD(drp_regs->RF_FREQL, rf_freq);

  burst_shift = 5;
  data_prev = 0;
  for(indx=0;indx < TX_BUFFER_LENGTH;indx++){
    temp32 = data_prev | (tx_burst1[indx] << 16);
  temp32 = temp32 << burst_shift;
  temp16 = (temp32 & 0xFFFF0000) >> 16;
  data_prev = tx_burst1[indx];
    tx_burst1[indx] = temp16;
  }

  // Set the TX buffer = TX_BUFFER_LENGTH
  WRITE_WORD(drp_regs->TX_PTR_START_END_ADDRH, 0x0020);
  WRITE_WORD(drp_regs->TX_PTR_START_END_ADDRL, (0x0020 + TX_BUFFER_LENGTH - 1));

  for (indx2 = 0;indx2 < TX_BUFFER_LENGTH;indx2++) {
    WRITE_WORD(drp_regs->TX_DATAL,tx_burst1[indx2]);
  }

  dbb_capture_phe_samples_setting();

  if(extended_tx_enable == 1){
    cnt_burst = 0x400;
  }
  else{
    cnt_burst = PHE_AVG_CNT;
  }

  for (indx1 = 0; indx1 < array_size; indx1++) {

    if(flyback_ibias_select == FLYBACK_SELECT)
      dbb_apply_ckv_delay(indx1);

    if(flyback_ibias_select == IBIAS_SELECT)
      dbb_apply_ibias(ibias_index[indx1]);

    rms_array[indx1] = 0;
    mse_array[indx1] = 0;

    //for(indxavg=0;indxavg < PHE_AVG_CNT;indxavg++){
    for(indxavg=0;indxavg < cnt_burst;indxavg++){

      // TX ON
      WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL,0x81);

      //    WAIT_US(250);
      WAIT_US(200);

      // TX_START high
      READ_WORD(drp_regs->DTX_SEL_CONTL,temp16);
      temp16 = (temp16 & ~0x3000) | 0x3000;
      WRITE_WORD(drp_regs->DTX_SEL_CONTL,temp16);

      READ_WORD(drp_regs->DTX_CONFIGL,temp16);
      temp16 = (temp16 & ~0x0080) | 0x0080;
      WRITE_WORD(drp_regs->DTX_CONFIGL,temp16);

      //    Wait for phe to settle
      WAIT_US(250);

      //dbb_capture_phe_samples();

      //start capturing the samples, decimate by 2
      WRITE_WORD(drp_regs->CALCIF_CWL, 0x0005);

      // Start PHE analysis script
      WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL,0x8B);

      WAIT_US((400));

      // Read R4 this has the mean
      scr_mean_32 = *((SINT32*) 0xffff0610);

      // Read R5 this has the sum squared error
      scr_sum_32 = *((uint32*) 0xffff0614);

      #if 0

      //read the captured phe samples
      calc_ptr = (UINT32*) DRP_SRM_CALC_BUFFER_ADDR;

      for(indx2=0;indx2 < NUM_PHE_SAMPLES;indx2++){
        mem_data32 = *calc_ptr;
        phe_samples[indx2] = (SINT16)((mem_data32 >> 8) & 0x0000ffff);
/*
        if(phe_samples[indx2] > PHE_THREHOLD)
          phe_samples[indx2] = PHE_THREHOLD;
        if(phe_samples[indx2] < -PHE_THREHOLD)
         phe_samples[indx2] = -PHE_THREHOLD;
*/
        calc_ptr++;
      }

      mean_phe32 = 0;
      for(indx2=0;indx2 < NUM_PHE_SAMPLES;indx2++){
        mean_phe32 += phe_samples[indx2];
      }

      sum_32 = 0;
      for(indx2=0;indx2 < NUM_PHE_SAMPLES;indx2++){
        sum_32 += ((phe_samples[indx2] * phe_samples[indx2]) >> PHE_SAMPLES_SHIFT);
      }
      #endif

      mean_phe32 = scr_mean_32 >> PHE_SAMPLES_SHIFT;
      sum_32 = scr_sum_32 - (UINT32)(mean_phe32 * mean_phe32);

      //mean_phe32 = mean_phe32 >> PHE_SAMPLES_SHIFT;
      //sum_32 = sum_32 - (UINT32)(mean_phe32 * mean_phe32);

      if(indxavg < PHE_AVG_CNT){
        mse_array[indx1] += sum_32;
        rms_array[indx1] += sqrt(sum_32);
      }
#if FLYBACK_DEBUG
      dbg_buffer[indxavg] = sqrt(sum_32);
#endif

      // TX_START low
      READ_WORD(drp_regs->DTX_CONFIGL,temp16);
      temp16 = (temp16 & ~0x0080);
      WRITE_WORD(drp_regs->DTX_CONFIGL,temp16);

      READ_WORD(drp_regs->DTX_SEL_CONTL,temp16);
      temp16 = (temp16 & ~0x3000) | 0x3000;
      WRITE_WORD(drp_regs->DTX_SEL_CONTL,temp16);

      // Idle Script
      WAIT_US(10);
      WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL,0x8F);
      WAIT_US(10);

    }

    rms_array[indx1] = rms_array[indx1] >> PHE_AVG_SHIFT;
    mse_array[indx1] = mse_array[indx1] >> PHE_AVG_SHIFT;
    variance[indx1] = (mse_array[indx1] - (rms_array[indx1]*rms_array[indx1])) >> PHE_AVG_SHIFT;

  }

  calc_ptr = (UINT32*) DRP_SRM_CALC_BUFFER_ADDR;
  // Now convert to degrees * 1000
  for(indx1=0;indx1 < array_size;indx1++){
    // PHE signal is scaled by 2^24, of this we have already right shift by 8 above
    rms_array[indx1] = (rms_array[indx1] * 1000 * 360) >> (24 - 8);
    // variance[] can be a small number so scale before sqrt, and shift right after multiply
    mem_data32 = variance[indx1] * 256;
    mem_data32 = sqrt(mem_data32);
    std_dev[indx1] = (mem_data32 * 1000 * 360) >> (24 + 4 - 8);
    *calc_ptr++ = rms_array[indx1];
    *rms_array_ptr++ = rms_array[indx1];
  }

  //      drp_sw_data->calib.txrx_common.ckv_dly[band] = indx1;

}

#else

/**************************************************************************************/
/* Compute the RMS phase error routine                                                                           */
/**************************************************************************************/
void rms_phe_compute(UINT16 rf_freq, UINT32 *rms_array_ptr, UINT16 extended_tx_enable, UINT16 flyback_ibias_select) {
  volatile UINT16 indx1, indx2;
  volatile UINT16 indxavg,indx;
  volatile UINT16 cnt_burst;
  volatile UINT16 txrx_mode;
  volatile UINT16 temp16;
  volatile UINT32 mean_phe;
  volatile SINT32 mean_phe32;
  volatile UINT32 sum_32;
  volatile UINT16 array_size;
  volatile UINT16 ibias_index[NUM_DCO_IBIAS] = {4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,100,110,127};
  volatile UINT32 rms_array[NUM_DCO_IBIAS];
  volatile UINT32 mse_array[NUM_DCO_IBIAS];
  volatile UINT32 variance[NUM_DCO_IBIAS], std_dev[NUM_DCO_IBIAS];
  volatile UINT16 mem_data;
  volatile UINT32 mem_data32;
  volatile UINT16 tx_burst1[TX_BUFFER_LENGTH] = {0x4f40,0x5746,0x7390,0x83a6,0x8874,0x9c74,0x4ca0,0x9b5b,0xa9d7,0xfff0};
  volatile UINT16 tx_burst2[TX_BUFFER_LENGTH] = {0x4f40,0x5746,0x7390,0x83a6,0x8874,0x9c74,0x4ca0,0x9b5b,0xa9d7,0xfff0};
  volatile UINT16 tx_burst3[TX_BUFFER_LENGTH] = {0x4f40,0x5746,0x7390,0x83a6,0x8874,0x9c74,0x4ca0,0x9b5b,0xa9d7,0xfff0};
  volatile UINT16 tx_burst4[TX_BUFFER_LENGTH] = {0x4f40,0x5746,0x7390,0x83a6,0x8874,0x9c74,0x4ca0,0x9b5b,0xa9d7,0xfff0};
  volatile SINT16 phe_samples[NUM_PHE_SAMPLES];
  volatile UINT32 *calc_ptr;
  volatile UINT16 data_prev;
  volatile UINT32 temp32;
  volatile UINT16 burst_shift;
  volatile UINT32 *ptr_srm;
  volatile UINT32 srm_data_saved[SRM_DATA_SAVED_LEN];
#if FLYBACK_DEBUG
  volatile UINT32 dbg_buffer[PHE_AVG_CNT];
#endif

  if(flyback_ibias_select == FLYBACK_SELECT)
    array_size = NUM_FLYBACK_DELAY;

  if(flyback_ibias_select == IBIAS_SELECT)
    array_size = NUM_DCO_IBIAS;

  txrx_mode = TX_MODE;
  WRITE_WORD(drp_regs->RF_FREQL, rf_freq);

  burst_shift = 5;
  data_prev = 0;
  for(indx=0;indx < TX_BUFFER_LENGTH;indx++){
    temp32 = data_prev | (tx_burst1[indx] << 16);
  temp32 = temp32 << burst_shift;
  temp16 = (temp32 & 0xFFFF0000) >> 16;
  data_prev = tx_burst1[indx];
    tx_burst1[indx] = temp16;
  }

  data_prev = 0;
  for(indx=0;indx < TX_BUFFER_LENGTH;indx++){
    temp32 = data_prev | (tx_burst2[indx] << 16);
  temp32 = temp32 << burst_shift;
  temp16 = (temp32 & 0xFFFF0000) >> 16;
  data_prev = tx_burst2[indx];
    tx_burst2[indx] = temp16;
  }

  data_prev = 0;
  for(indx=0;indx < TX_BUFFER_LENGTH;indx++){
    temp32 = data_prev | (tx_burst3[indx] << 16);
  temp32 = temp32 << burst_shift;
  temp16 = (temp32 & 0xFFFF0000) >> 16;
  data_prev = tx_burst3[indx];
    tx_burst3[indx] = temp16;
  }

  data_prev = 0;
  for(indx=0;indx < TX_BUFFER_LENGTH;indx++){
    temp32 = data_prev | (tx_burst4[indx] << 16);
  temp32 = temp32 << burst_shift;
  temp16 = (temp32 & 0xFFFF0000) >> 16;
  data_prev = tx_burst4[indx];
    tx_burst4[indx] = temp16;
  }

  // Set the TX buffer = TX_BUFFER_LENGTH
  WRITE_WORD(drp_regs->TX_PTR_START_END_ADDRH, 0x0020);
  WRITE_WORD(drp_regs->TX_PTR_START_END_ADDRL, (0x0020 + TX_BUFFER_LENGTH - 1));

  dbb_capture_phe_samples_setting();

  if(extended_tx_enable == 1){
    cnt_burst = 0x400;
  }
  else{
    cnt_burst = PHE_AVG_CNT;
  }

  for (indx1 = 0; indx1 < array_size; indx1++) {

    if(flyback_ibias_select == FLYBACK_SELECT)
      dbb_apply_ckv_delay(indx1);

    if(flyback_ibias_select == IBIAS_SELECT)
      dbb_apply_ibias(ibias_index[indx1]);

    rms_array[indx1] = 0;
    mse_array[indx1] = 0;

    //for(indxavg=0;indxavg < PHE_AVG_CNT;indxavg++){
    for(indxavg=0;indxavg < cnt_burst;indxavg++){

      temp16 = indxavg/4;
      temp16 = indxavg - (4 * temp16);
      switch(temp16){
        case 0:
           // Load the Tx burst
          for (indx2 = 0;indx2 < TX_BUFFER_LENGTH;indx2++) {
            WRITE_WORD(drp_regs->TX_DATAL,tx_burst1[indx2]);
           }
           break;

        case 1:
           // Load the Tx burst
          for (indx2 = 0;indx2 < TX_BUFFER_LENGTH;indx2++) {
            WRITE_WORD(drp_regs->TX_DATAL,tx_burst2[indx2]);
           }
           break;

        case 2:
           // Load the Tx burst
          for (indx2 = 0;indx2 < TX_BUFFER_LENGTH;indx2++) {
            WRITE_WORD(drp_regs->TX_DATAL,tx_burst3[indx2]);
           }
           break;

        case 3:
           // Load the Tx burst
          for (indx2 = 0;indx2 < TX_BUFFER_LENGTH;indx2++) {
            WRITE_WORD(drp_regs->TX_DATAL,tx_burst4[indx2]);
           }
           break;
      }

      // save the drp_srm tail segment to prevent overwritting by the Calc buffer
      ptr_srm = (UINT32 *)DRP_SRM_DATA_ADD;
      for(indx2=0;indx2 < SRM_DATA_SAVED_LEN;indx2++)
        srm_data_saved[indx2] = *ptr_srm++;

      // TX ON
      WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL,0x81);

      //    WAIT_US(250);
      WAIT_US(200);

      // Enable LFSR
      //      WRITE_WORD(drp_regs->DTX_USER_WORDL,0xffff);
      //      WRITE_BYTE_LOW(drp_regs->DTX_SEL_CONTL,0x0003);

      // TX_START high
      READ_WORD(drp_regs->DTX_SEL_CONTL,temp16);
      temp16 = (temp16 & ~0x3000) | 0x3000;
      WRITE_WORD(drp_regs->DTX_SEL_CONTL,temp16);

      READ_WORD(drp_regs->DTX_CONFIGL,temp16);
      temp16 = (temp16 & ~0x0080) | 0x0080;
      WRITE_WORD(drp_regs->DTX_CONFIGL,temp16);

      //    Wait for phe to settle
      WAIT_US(250);

      dbb_capture_phe_samples();

      //    Complete the burst
      WAIT_US((610 - 250 - (170*2)));

      // TX_START low
      READ_WORD(drp_regs->DTX_CONFIGL,temp16);
      temp16 = (temp16 & ~0x0080);
      WRITE_WORD(drp_regs->DTX_CONFIGL,temp16);

      READ_WORD(drp_regs->DTX_SEL_CONTL,temp16);
      temp16 = (temp16 & ~0x3000) | 0x3000;
      WRITE_WORD(drp_regs->DTX_SEL_CONTL,temp16);

      WAIT_US(10);
      WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL,0x8F);
      WAIT_US(10);

      //Disable LFSR
      WRITE_BYTE_LOW(drp_regs->DTX_SEL_CONTL,0x0000);

      //read the captured phe samples
      calc_ptr = (UINT32*) DRP_SRM_CALC_BUFFER_ADDR;

      for(indx2=0;indx2 < NUM_PHE_SAMPLES;indx2++){
        mem_data32 = *calc_ptr;
        phe_samples[indx2] = (SINT16)((mem_data32 >> 8) & 0x0000ffff);

   if(phe_samples[indx2] > PHE_THREHOLD)
     phe_samples[indx2] = PHE_THREHOLD;
   if(phe_samples[indx2] < -PHE_THREHOLD)
     phe_samples[indx2] = -PHE_THREHOLD;

        calc_ptr++;
      }

      mean_phe32 = 0;
      for(indx2=0;indx2 < NUM_PHE_SAMPLES;indx2++){
        mean_phe32 += phe_samples[indx2];
      }
      mean_phe32 = mean_phe32/NUM_PHE_SAMPLES;

      sum_32 = 0;
      for(indx2=0;indx2 < NUM_PHE_SAMPLES;indx2++){
        phe_samples[indx2] -= (SINT16) mean_phe32;
        sum_32 += ((phe_samples[indx2] * phe_samples[indx2]) >> PHE_SAMPLES_SHIFT);
      }

      if(indxavg < PHE_AVG_CNT){
        mse_array[indx1] += sum_32;
        rms_array[indx1] += sqrt(sum_32);
      }
#if FLYBACK_DEBUG
      dbg_buffer[indxavg] = sqrt(sum_32);
#endif

      // restore the drp_srm tail segment
      ptr_srm = (UINT32 *)DRP_SRM_DATA_ADD;
      for(indx2=0;indx2 < SRM_DATA_SAVED_LEN;indx2++)
        *ptr_srm++ = srm_data_saved[indx2];

      if(extended_tx_enable == 1){
        //Wait for next burst
        WAIT_US(3500);
      }

    }

    rms_array[indx1] = rms_array[indx1] >> PHE_AVG_SHIFT;
    mse_array[indx1] = mse_array[indx1] >> PHE_AVG_SHIFT;
    variance[indx1] = (mse_array[indx1] - (rms_array[indx1]*rms_array[indx1])) >> PHE_AVG_SHIFT;

    if(extended_tx_enable == 1){
      WAIT_US(2000*1000);
    }

  }

  calc_ptr = (UINT32*) DRP_SRM_CALC_BUFFER_ADDR;
  // Now convert to degrees * 1000
  for(indx1=0;indx1 < array_size;indx1++){
    // PHE signal is scaled by 2^24, of this we have already right shift by 8 above
    rms_array[indx1] = (rms_array[indx1] * 1000 * 360) >> (24 - 8);
       // variance[] can be a small number so scale before sqrt, and shift right after multiply
  mem_data32 = variance[indx1] * 256;
  mem_data32 = sqrt(mem_data32);
  std_dev[indx1] = (mem_data32 * 1000 * 360) >> (24 + 4 - 8);
  *calc_ptr++ = rms_array[indx1];
  *rms_array_ptr++ = rms_array[indx1];
  }

  //      drp_sw_data->calib.txrx_common.ckv_dly[band] = indx1;

}

#endif

#endif // #if DRP_FLYBACK_IBIAS_CALIB

#endif //#if ((DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT) || (!DRP_TEST_SW))

#if 0
/**************************************************************************************/
/* Spectral estimation using Goertzel algorithm                                       */
/* x[] is the data vector                                                             */
/* N is the size of x[]                                                               */
/* m is the size of the coefficient vector, coeffptr[], which holds the cosine        */
/* coefficients of the Goertzel filter                                                */
/* spec_est[] is the array that will return the spectral estimate.                    */
/**************************************************************************************/
void spectral_estimate(short *x, short N, short m, short *coeffptr, long *spec_est){

  short n,k;
  short scale_sh;
  long coeff;
  long sn,sn1,sn2;
  long temp,power;

  // scaling shift
  scale_sh = 3;

  for(k=0;k < m;k++){

    sn = 0;
    sn1 = 0;
    sn2 = 0;

    // use 8 bit coefficient to avoid overflow
    coeff = coeffptr[k] >> 6;

    // Goertzel recursive filter for freqency coefficient corresponding to
    // coeffptr[k]

    for(n=0;n <= N;n++){

      temp = coeff*sn1;
      temp = temp >> 8;
      sn = (long)x[n] + (temp) - sn2;
      sn2 = sn1;
      sn1 = sn;

      if((sn1 > 8388608) || (sn1 < -8388608)){
        sn1= sn1+1;
      }

    }

    // scaling to avoid overflow
    sn = sn1 >> 10;
    sn1 = sn2 >> 10;

    // compute the squared magnitude of the FFT
    power = ((sn*sn) >> scale_sh) + ((sn1*sn1) >> scale_sh);
    temp = (sn * coeff) >> 8;
    temp = (temp * sn1) >> scale_sh;
    spec_est[k] = power - temp;

  }

  return;
}

#endif //#if 0

#if (DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT))

/**************************************************************************************/
/* This function does the initial setting for capturing the PHE samples in CALC buffer*/
/**************************************************************************************/
void dbb_capture_phe_samples_setting()
{
  //DLO_TSTOUT is free running on decimated CKR b(22:20)
  WRITE_BYTE_HIGH(drp_regs->DLO_TESTL, 0x0200);
  //Select input for Demux from DLO b(15:8) = 0x07
  //Select phase_err_iir_0mean from DLO b(7:0) = 0x2C
  WRITE_WORD(drp_regs->DTST_MUX_SELL, 0x072C);
  //mem_tstclk0 is CKRD8
  WRITE_BYTE_LOW(drp_regs->CKM_TSTCLKL, 0x0013);
  //decimated enable signal for DLO from CKR domain (CKR/8 = 3.25MHz) b(22:20) = b011
  WRITE_BYTE_LOW(drp_regs->CKM_TSTCLKH, 0x0030);
  WRITE_WORD(drp_regs->CALC_PTR_START_END_ADDRL,0x02ff);
  WRITE_WORD(drp_regs->CALC_PTR_START_END_ADDRH,0x00C0);
}

/**************************************************************************************/
/* This function captures PHE samples in CALC buffer                                  */
/**************************************************************************************/
void dbb_capture_phe_samples()
{
  //start capturing the samples, decimate by 2
  WRITE_WORD(drp_regs->CALCIF_CWL, 0x0005);
  //wait for sometime to capture 512 + delta samples
  WAIT_US((170*2));
  //stop capturing the samples
  WRITE_WORD(drp_regs->CALCIF_CWL, 0x0004);
}

/**************************************************************************************/
/* This function applies the ckv delay (based on index) in ANA_USD(4:2)               */
/**************************************************************************************/
void dbb_apply_ckv_delay(UINT16 index)
{
  UINT16 mem_data;

  READ_WORD(drp_regs->ANA_USDL, mem_data);
  mem_data = (mem_data & 0xFFE3) | (index << 2);
  WRITE_WORD(drp_regs->ANA_USDL, mem_data);
}

#endif //#if (DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT))

//#if (DRP_TEST_SW && !DRP_BURNIN)
/**************************************************************************************/
/*  This function starts the temperature script.                                      */
/*  The temperature script measures the temperature                                   */
/*  using the DCXO ADC and updates DRP_MEAS_TEMP.                                     */
/*  The function dbb_read_temp() can be used to read the                              */
/*  script result after waiting for the script to complete.                           */
/**************************************************************************************/
void dbb_run_temp(void) {

  UINT16 calib_ctl_saved;

  //disable the temp sensor which is enabled in the idle script
  //WRITE_BYTE_LOW(drp_regs->ANA_DCXOL,0x0000);
  //WRITE_BYTE_HIGH(drp_regs->DCXO_IDACH, 0x0002);
  //WRITE_BYTE_LOW(drp_regs->DCXO_CKADCL, 0x0000);
  //WAIT_US(100); //wait for sometime

  // start read_temperature script.
  //WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0083);

  READ_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);
  //dont bypass anything (for temp to be read)
  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0000);

  //start IDLE script
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
  WAIT_US(20);

  //this additional wait is required for the HW temp sensor SAR to stabilise.
  WAIT_US(1000);

  // start RX_ON
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0082);
  WAIT_US(200);

  //start IDLE script
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
  WAIT_US(20);

  WRITE_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);

  return;

}

/**************************************************************************************/
/* This function reads the DRP temperature from the result of                         */
/* the last time the temperature script was invoked.  It should only                  */
/* be called after waiting long enough for the temperature script complete.           */
/* This function retures the temperature in deg C.                                    */
/**************************************************************************************/
SINT16 dbb_read_temp(void) {
  UINT16 mem_data;
  SINT16 temp;

  //mem_data = *(volatile ioport UINT16 *)(DRP_DRP_OFFSET + (DRP_MEAS_TEMP >> 1));
  READ_WORD(drp_srm_api->inout.temperature.output,mem_data);

  // high byte is adc code; low byte is temp degrees C.
  temp = (SINT16)(mem_data << 8);   // Sign Extend data;
  temp = temp >> 8;

//#if DRP_TEST_SW
  return(temp);
//#else
//  return(TREF);          //#@# return 30 deg C for now (testing)
//#endif
}
//#endif //#if (DRP_TEST_SW && !DRP_BURNIN)

#if ((DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT)) || (!DRP_TEST_SW))
/**************************************************************************************/
/* This function runs the RX_ON script (band is 0;afe gain is step 27, abe_index      */
/* is an input parameter).                                                            */
/* It gets the measured ABE VPP from SRM Memory after RX_ON completes,                */
/* and then runs the IDLE script.                                                     */
/* It returns ABE VPP.                                                                */
/**************************************************************************************/
UINT16 dbb_meas_abe_vpp(UINT16 band, UINT16 abe_index) {
  UINT16 vpp;

  // run RX_ON script with desired band/gain settings.
  set_abe_gain(abe_index);
  set_afe_gain(27, band);
  rx_on_calib_timeline(band, g_band_cntr_ch[band]);

  // RX_ON measured vpp in DCEST.
  // vpp = *(volatile ioport UINT16 *)(DRP_DRP_OFFSET + (DRP_MEAS_ABE_VPP_I >> 1));
  READ_WORD(drp_srm_api->inout.rx.agc.meas_abe_vpp_i, vpp)
  // trigger IDLE script
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
  wait_qb(22);   // ~20us

  return(vpp);
}

/**************************************************************************************/
/* This function finds the temperature coefficient                                    */
/**************************************************************************************/
SINT16 dbb_find_temp_coeff(UINT16 index, SINT16 gainlog2) {
  SINT16 gain_weak, alpha_weak, alpha;
  UINT16 beta;

  //extract the ABE gain for the weak process
  gain_weak = (SINT16) (drp_sw_data->simulation.rx.abe.abe_calib_table_weak[index] >> 16);

  //extract the temperature coefficient for the weak process
  alpha_weak = (SINT16) (drp_sw_data->simulation.rx.abe.abe_calib_table_weak[index] & 0xFFFF);

  //extract the beta value correspoonding to the given index
  beta = drp_sw_data->simulation.rx.abe.beta[index];

  // GAIN_SCALE, BETA_SCALE, ALPHA_SCALE are scaling shifts introduced to replace
  // division by multiplication and shifts
  alpha = ((SINT32) beta * (gainlog2 - gain_weak)) >> (GAIN_SCALE + BETA_SCALE - ALPHA_SCALE);

  //since gains are in log2 format, so the equation is
  //alpha_k = alpha_k_weak + beta_k * (gain_k - gain_k_weak)*10log10(2)
  //or
  //alpha_k = alpha_k_weak + beta_k * (gain_k - gain_k_weak)*3

  alpha = alpha * 3;
  alpha += alpha_weak;

  return (alpha);
}

/**************************************************************************************/
/* This function normalizes the gain measured at temperature T to the gain at TREF    */
/**************************************************************************************/
SINT16 dbb_normalize_gain(SINT16 gainlog2, SINT16 alpha, SINT16 temp) {
  SINT16 gain_n;

  // GAIN_SCALE, ALPHA_SCALE are scaling shifts
  gain_n = ((SINT32) alpha * (TREF - temp)) >> (ALPHA_SCALE - GAIN_SCALE);
  gain_n += gainlog2;

  return (gain_n);
}

#if 0
/**************************************************************************************/
/* This function sets the bit 0 of the register reg                                   */
/**************************************************************************************/
void dbb_enable(volatile UINT16 *reg)
{
  UINT16 mem_data;

  READ_WORD(*reg, mem_data);

  mem_data = mem_data | 0x0001;

  WRITE_WORD(*reg, mem_data);
}

/**************************************************************************************/
/* This function applies the DCO ibias value (based on the index) in ANA_DCO(24:18)   */
/**************************************************************************************/
void dbb_apply_ibias(UINT16 index)
{
  UINT16 mem_data, mod_index;

  mod_index = 0x0000;

  if(index & 0x0001)
    mod_index |= 0x0040;

  if(index & 0x0002)
    mod_index |= 0x0020;

  if(index & 0x0004)
    mod_index |= 0x0010;

  if(index & 0x0008)
    mod_index |= 0x0008;

  if(index & 0x0010)
    mod_index |= 0x0004;

  if(index & 0x0020)
    mod_index |= 0x0002;

  if(index & 0x0040)
    mod_index |= 0x0001;

  READ_WORD(drp_regs->ANA_DCOH, mem_data);

  mem_data = (mem_data & 0xFE03) | ((UINT32) mod_index << 2);

  WRITE_WORD(drp_regs->ANA_DCOH, mem_data);
}

//#if DRP_TEST_SW

/**************************************************************************************/
/*  Support functions for compensation code                                           */
/**************************************************************************************/

/**************************************************************************************/
/*  AFE gain compensation after the RX_ON script                                      */
/*  Does AFE gain compensation based on the temperature and the measured              */
/*  abe gain.                                                                         */
/**************************************************************************************/
UINT16 afe_gain_comp2 (UINT16 abe_gain_index, UINT16 afe_gain_index, SINT16 meas_abe_log2) {

  SINT16 tar_abe_gain, tar_afe_gain;
  UINT16 afe_gain_step;

  tar_abe_gain = tar_abe_gain_table[abe_gain_index];

  // calculate the TAR_AFE based on the measured ABE gain, TAR_ABE and the AGC_STEP
  tar_afe_gain = calc_tar_afe(meas_abe_log2, tar_abe_gain, afe_gain_index);

  afe_gain_step = get_closest_afe_gain(tar_afe_gain);

  // save gain for gain reporting function
  g_afe_gain = afe_est_table[afe_gain_index];

  return (afe_gain_step);
}
#endif

#endif //#if ((DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT)) || (!DRP_TEST_SW))

/**************************************************************************************/
/*  Estimating the ABE gains based on the current temperature                         */
/**************************************************************************************/
UINT16 estimate_abe_gain(UINT16 abe_gain_index, SINT16 temp, SINT16 *abe_actual) {
  SINT16 k, g_k_ref, alpha_k, g_k, tar_abe_gain, abe_est_table[NUM_ABE_GAINS];

  for (k = 0; k < NUM_ABE_GAINS; k++) {
    // read the reference ABE table for gains
    g_k_ref = (SINT32) drp_sw_data->calib.rx.abe.gain_alpha_table[k] >> 16;

    // read the reference ABE table for temperature coefficient
    alpha_k = (SINT16) (drp_sw_data->calib.rx.abe.gain_alpha_table[k] & 0xFFFF);

    // estimate the gain at the current temperature
    g_k = ((SINT32) alpha_k * (temp - TREF)) >> (ALPHA_SCALE - GAIN_SCALE);
    g_k += g_k_ref;

    // store the estimated gains in abe_est_table
    abe_est_table[k] = g_k;
  }

  tar_abe_gain = drp_srm_data->tables.tar_abe_gain_table[abe_gain_index];

  // find the ABE gain index which is closest to the desired ABE gain
  for (k = 0; k < NUM_ABE_GAINS; k++) {

    if (abe_est_table[k] < tar_abe_gain) break;
  }

  if (k == NUM_ABE_GAINS) return(NUM_ABE_GAINS - 1);

  // This is how the gain compensation in the scripts will work, so don't change it.
  // pick the gain which is higher slightly than tar_abe_gain
  if (k > 0)
    k = k - 1;

  *abe_actual = abe_est_table[k];
  return (k);

}

/**************************************************************************************/
/*  Estimating the AFE gains based on the current temperature                         */
/**************************************************************************************/
UINT16 estimate_afe_gain(UINT16 afe_gain_index, SINT16 temp, UINT16 band, SINT16 *afe_actual) {
  SINT16 k, g_k_ref, g_k, correction, tar_afe_gain, afe_est_table[NUM_AFE_GAINS];
#if 0
  SINT16 min_diff, closest_index, diff;
#endif

  correction = ((SINT32) AFE_ALPHA * (temp - TREF)) >> (ALPHA_SCALE - GAIN_SCALE);

  for (k = 0; k < NUM_AFE_GAINS; k++) {
    // read the reference AFE table for gains
    g_k_ref = drp_sw_data->calib.rx.afe.gain[band][k];

    // estimate the gain at the current temperature
    g_k = g_k_ref + correction;

    // store the estimated gains in afe_est_table
    afe_est_table[k] = g_k;
  }

  if (band > 1)
    tar_afe_gain = drp_srm_data->tables.tar_afe_gain_table[1][afe_gain_index];
  else
    tar_afe_gain = drp_srm_data->tables.tar_afe_gain_table[0][afe_gain_index];

  // find the AFE gain index which is closest to the desired AFE gain
  for (k = 0; k < NUM_AFE_GAINS; k++) {

    if (afe_est_table[k] < tar_afe_gain)
      break;
  }

  if (k > 0)
    k = k - 1;

  *afe_actual = afe_est_table[k];
  return (k);

#if 0
  min_diff = 0x7FFF;
  closest_index = 0;
  for (k = 0; k < NUM_AFE_GAINS; k++) {
    if (afe_est_table[k] > tar_afe_gain)
      diff = afe_est_table[k] - tar_afe_gain;
    else
      diff = tar_afe_gain - afe_est_table[k];

    if (diff < min_diff)
    {
      min_diff = diff;
      closest_index = k;
    }
  }

  return (closest_index);
#endif

}

#if 0
/**************************************************************************************/
/*  Extracting the closest CTA gain from ABE_EST_TABLE                                */
/**************************************************************************************/
UINT16 get_closest_abe_gain(SINT16 tar_abe_gain_dB) {
  UINT16 k;

  for (k = 0; k < NUM_ABE_GAINS; k++) {
    // search for the CTA gain which is greater than TAR_ABE
    if (abe_est_table[k] < tar_abe_gain_dB)
      break;
  }

  // if k > 0, the current gain is less than TAR_ABE so pick the previous index
  if (k > 0)
    k = k - 1;

  return(k);
}

/**************************************************************************************/
/*  This function Calculate the TAR_AFE based on MEAS_ABE and AGC_STEP                */
/**************************************************************************************/
SINT16 calc_tar_afe(SINT16 meas_abe_gain_dB, SINT16 tar_abe_gain_dB, UINT16 afe_gain_index) {
  SINT16 delta, tar_afe_gain_dB;

  delta = meas_abe_gain_dB - tar_abe_gain_dB;

  tar_afe_gain_dB = tar_afe_gain_table[afe_gain_index] - delta;

  return(tar_afe_gain_dB);
}

/**************************************************************************************/
/*  This function gets the closest AFE gain from the estimated AFE gains              */
/**************************************************************************************/
UINT16 get_closest_afe_gain(SINT16 tar_afe_gain_dB) {
  UINT16 k;

  for (k = 0; k < NUM_AFE_GAINS; k++) {
    // search for the AFE gains in EST gain table for the AFE gain which is closest to TAR_AFE
    if (afe_est_table[k] < tar_afe_gain_dB)
      break;
  }

  // if index > 0, current gain is less than TAR_AFE. So select the previous index.
  if (k > 0)
    k = k - 1;

  return(k);

}
#endif //#if 0

#if (DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT))
/**************************************************************************************/
/* Main phonemode routine - Details of the Rx and Tx timeline implementation          */
/**************************************************************************************/

/**************************************************************************************/
/* New RX timeline implementation                                                     */
/**************************************************************************************/
SINT16 rx_timeline_new(UINT16 band, UINT16 channel, UINT16 abe_gain_indx, UINT16 afe_gain_indx,
                     UINT16 gain_comp_enable, UINT16 pole_num, UINT16 pcb_config) {

  SINT16 total_gain;

  //start temperature script
  //dbb_run_temp();
  //wait_qb(27);

  total_gain = rx_burst_sr1(channel, band, abe_gain_indx, afe_gain_indx, pole_num, gain_comp_enable, pcb_config);

  return(total_gain);
}

/**************************************************************************************/
/*  RX Burst service routine                                                          */
/*                                                                                    */
/*  - Set appropriate RF_FREQ                                                         */
/*  - Set ABE and AFE gains based on gain_indices (in RXON_INPUT) and temperature     */
/*  - Issue RX_ON                                                                     */
/*  - Wait for RX_ON to complete                                                      */
/*  - Assert RX_START                                                                 */
/*  - Wait for about 700us                                                            */
/*  - De-assert RX_START                                                              */
/*  - Issue IDLE script                                                               */
/*  - Wait for about 15us                                                             */
/**************************************************************************************/
SINT16 rx_burst_sr1(UINT16 channel, UINT16 band, UINT16 abe_gain_index, UINT16 afe_gain_index,
                    UINT16 pole_num, UINT16 gain_comp_enable, UINT16 pcb_config) {
  SINT16 total_gain;
  UINT16 mem_data, cont_burst_flag;
  UINT16 rxon_input_saved, temp16;
  UINT16 dbbif_setting;
  UINT16 default_rx_start_end_ptr_settingl, default_rx_start_end_ptr_settingh;
  SINT32 total_gain2;

  #define RXON_INP_SCFPOLE_SHIFT    7
  #define RXON_INP_AFEGAIN_SHIFT    6
  #define RXON_INP_ABEGAIN_SHIFT    2
  #define RXON_INP_GAINCOMP_SHIFT   1

  //DBBIF setting generation and passing to rxon_input
  READ_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);
  temp16 = rxon_input_saved & (~0x0300);
  // dbbif setting
  dbbif_setting = drp_generate_dbbif_setting(pcb_config, band);
  temp16 = (temp16 & (~0x0300)) | ((dbbif_setting & 0x3) << 8);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);


  temp16 = ((pole_num & 1) << RXON_INP_SCFPOLE_SHIFT) | ((afe_gain_index & 1) << RXON_INP_AFEGAIN_SHIFT) | \
           ((abe_gain_index & 0xF) << RXON_INP_ABEGAIN_SHIFT) | ((gain_comp_enable & 1) << RXON_INP_GAINCOMP_SHIFT);

  READ_WORD(drp_srm_api->inout.rx.rxon_input, mem_data);
  mem_data = (mem_data & 0x00301) | temp16;
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, mem_data);

  //Time line starts here
  dbb_write_rffreq(band, channel, RX_MODE);

  //set the appropriate gains
  //dbb_rx_burst2(abe_gain_index, afe_gain_index, pole_num, band, gain_comp_enable);

  //RX_ON script
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0082);

  //this wait is for RX_ON to complete.
  WAIT_US(200);

  //if (gain_comp_enable == 1)
  //  total_gain = dbb_rx_burst3(abe_gain_index, afe_gain_index, band);
  //else
  //  total_gain = 19730; //58dB default

  //WAIT_US(100);

  READ_WORD(drp_srm_api->inout.rx.agc.compensated_gain, mem_data);
  // calculate gain of RX path for gain reporting functions (ADC gain is 91_2 (scaling factor))
  total_gain2 = (SINT32) mem_data + (SINT32) DRP_GAIN_ADCDBM;

  // convert to dB
  total_gain2 *= DRP_GAIN_REPORT_CNVRT;
  total_gain = (SINT16) (total_gain2 >> DRP_GAIN_REPORT_SHIFT);

  READ_WORD(drp_regs->RX_PTR_START_END_ADDRL, default_rx_start_end_ptr_settingl);
  WRITE_WORD(drp_regs->RX_PTR_START_END_ADDRL,0x02FF);
  READ_WORD(drp_regs->RX_PTR_START_END_ADDRH, default_rx_start_end_ptr_settingh);
  WRITE_WORD(drp_regs->RX_PTR_START_END_ADDRH,0x00C0);

  //assert RX_START
  READ_WORD(drp_regs->CKM_CLOCKSL, mem_data);
  mem_data = mem_data | 0x0030;
  WRITE_WORD(drp_regs->CKM_CLOCKSL, mem_data);

  WAIT_US(700);

#if DRP_TEST_SW
  //wait here in CONTINUOUS_MODE until flag is cleared.
  do {
    asm(" nop");

    cont_burst_flag = drp_param5L;

  } while (cont_burst_flag == CONTINUOUS_MODE);
#endif

  //de-assert RX_START
  READ_WORD(drp_regs->CKM_CLOCKSL, mem_data);
  mem_data = mem_data & (~0x0030);
  WRITE_WORD(drp_regs->CKM_CLOCKSL, mem_data);

  //start the idle script
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0000008F);
  WAIT_US(15);

  WRITE_WORD(drp_regs->RX_PTR_START_END_ADDRL, default_rx_start_end_ptr_settingl);
  WRITE_WORD(drp_regs->RX_PTR_START_END_ADDRH, default_rx_start_end_ptr_settingh);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);

  return(total_gain);
}
#endif //#if (DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT))

#if (DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT)) || (!DRP_TEST_SW)
/**************************************************************************************/
/* Triggers the temperature measurement script.                                       */
/**************************************************************************************/
void dbb_rx_burst1()
{
  dbb_read_temp();
}

/**************************************************************************************/
/* This function does the temperature compensation of the ABE and AFE gains based on  */
/* on the target ABE and AFE gains as decided by abe_gain_indx and afe_gain_indx      */
/* respectively.                                                                      */
/**************************************************************************************/
void dbb_rx_burst2(UINT16 abe_gain_indx, UINT16 afe_gain_indx, UINT16 pole_num, UINT16 band, UINT16 gain_comp_enable)
{
  UINT16 abe_step, afe_step, temp16;
  SINT16 temp, lb_hb_flag, abe_actual, afe_actual;

  temp = dbb_read_temp();

  if (gain_comp_enable == 0)
  {
    temp = TREF;
    abe_step = default_ABE_step_table[abe_gain_indx];
    lb_hb_flag = (band >= 2);
    afe_step = default_AFE_step_table[lb_hb_flag][afe_gain_indx];
    set_abe_gain(abe_step);
    set_afe_gain(afe_step, band);
  }
  else
  {
  //get the closest gain settings for ABE and AFE based on the desired gain_step and current temperature
  abe_step = estimate_abe_gain(abe_gain_indx, temp, &abe_actual);
  afe_step = estimate_afe_gain(afe_gain_indx, temp, band, &afe_actual);

  set_abe_gain(abe_step);
    set_afe_gain(afe_step, band);
  }

  // Set the SCF pole index
  READ_WORD(drp_srm_api->inout.rx.rxon_input, temp16);
  temp16 = (temp16 & ~0x0080) | ((pole_num & 0x1) << 7);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

  return;
}

#endif //#if (DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT)) || (!DRP_TEST_SW)

#if 0
/**************************************************************************************/
/* Final AFE gain compensation based on measured ABE gain                             */
/**************************************************************************************/
SINT16 dbb_rx_burst3(UINT16 abe_gain_indx, UINT16 afe_gain_indx, UINT16 band)
{
  SINT16 meas_abe_log2;
  SINT32 total_gain;
  UINT16 abe_vpp, inp_signal_peak, afe_step;

  READ_WORD(drp_srm_api->inout.rx.agc.meas_abe_vpp_i,abe_vpp);

  // gain_dB = 20*log10(Vpp_out/Vpp_in) + SCF_CAL_LOSS
  //         = 20*log10(Vpp_out) - 20*log10(Vpp_in) + SCF_CAL_LOSS
  //
  // Input signal is from -2^abe_meas_input_strength to +2^abe_meas_input_strength
  // i.e., Vpp_in is 2^(abe_meas_input_strength + 1)
  //
  // Since we are taking log2, the second term in the above expression becomes
  //      2 * (2^DRP_LOG_SCALE) * log2(2^(abe_meas_input_strength + 1))
  // or   2 * (2^DRP_LOG_SCALE) * (abe_meas_input_strength + 1)
  // or   (abe_meas_input_strength + 1) << (DRP_LOG_SCALE + 1)
  //
  // Note: calc_log(num) will return (2^DRP_LOG_SCALE)*log2(num)

  READ_WORD(drp_srm_data->abe_gain_comp_index, inp_signal_peak);
  meas_abe_log2 = 2*calc_log(abe_vpp) - ((inp_signal_peak + 1) << (LOG_SCALE + 1)) + SCF_CAL_LOSS;

  afe_step = afe_gain_comp2(abe_gain_indx, afe_gain_indx, meas_abe_log2);

#if DRP_TEST_SW
  //this enables writing into ARX_CW
  WRITE_WORD(drp_regs->SRM_CWL, 0x0342);
#endif

  set_afe_gain(afe_step, band);

#if DRP_TEST_SW
  //this enables writing into ARX_CW
  WRITE_WORD(drp_regs->SRM_CWL, 0x2140);
#endif

  // calculate gain of RX path for gain reporting functions (ADC gain is 91_2 (scaling factor))
  total_gain = (SINT32) g_afe_gain + (SINT32) meas_abe_log2 + (SINT32) DRP_GAIN_ADCDBM;

  // convert to dB
  total_gain*=DRP_GAIN_REPORT_CNVRT;
  total_gain = total_gain >> DRP_GAIN_REPORT_SHIFT;

  return((SINT16) total_gain);
}

#endif //#if 0

#if ((DRP_TEST_SW && !DRP_BURNIN) || (!DRP_TEST_SW))
/**************************************************************************************/
/* Set prefilter setings 0x00000777                                                   */
/**************************************************************************************/
void dbb_pref_set_0777(void){

  WRITE_WORD(drp_regs->PREF_CWH, 0x0100);
  WRITE_WORD(drp_regs->PREF_CWL, 0x0777);
  WAIT_US(1);
  WRITE_WORD(drp_regs->PREF_CWH, 0x0000);

  WAIT_US(25);
}

#endif //#if ((DRP_TEST_SW && DRP_TDL_DFT) || (!DRP_TEST_SW))

#if (DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT))

void ref_roc(CMPLX16 *rxbufptr, UINT16 num_iqsamples, UINT16 accmulation_count, UINT16 start_index) {

  UINT16 mem_data;
  SINT32 tmps32;
  UINT32 tmp32;
  UINT16 lut_incr;
  UINT16 ix;
  UINT16 re_offset,im_offset;
  UINT16 re_offset1,im_offset1;
  UINT16 re_idx,im_idx;
  UINT16 alpha;
  CMPLX32 dc, dc_adjust;
  CMPLX16 iqmc_coeff;
  CMPLX16 zif_tone, zif_tone1;
#if ROC_DEBUG
  float err,err1,err2;
  UINT16 optimize_check;
#endif
  UINT16 attn;

  READ_WORD(drp_srm_api->inout.rx.rxon_input, mem_data);
  if ((mem_data & 1) == 0)
  {
    //for 100kHz IF
    attn = 49371;
    start_index = 443;
    alpha = 30800;
  }
  else
  {
    //for 120kHz IF
    attn = 2400;
    start_index = 216;
    alpha = 45875;
  }

  //make IF carrier generation more generic
  //lut_incr = DRP_LUT_INCR;
  mem_data = drp_regs->ZIF_CWL;
  lut_incr = ((mem_data >> 3) & 0x00FF) * 4;

#if ROC_DEBUG
  optimize_check = 1;
  while(optimize_check == 1){
#endif

  // Get roc accumulates i/q values from drp registers and compute DC
  // and scale by the CSF attenuation factor.
  // Swap the I and Q values as you read me

  //tmp32 = drp_srm_api->inout.rx.roc.output.accum_Q[0];
  READ_WORD(drp_regs->ROC_CALC_QL, mem_data);
  tmp32 = (UINT32) (0x0000FFFF & (UINT32) mem_data);
  READ_WORD(drp_regs->ROC_CALC_QH, mem_data);
  tmp32 |= ((UINT32) mem_data)<<16;

//  sign extend
//  if(tmp32 & 0x02000000) tmp32 |= 0xfc000000;
//  tmps32 = (SINT32)tmp32;
//  tmps32 = (tmps32*64)/accmulation_count;
//  tmps32 = (SINT32)((float)tmps32 * 0.75335);
//  dc.real = tmps32 >> 6;
//  sign extend
  if(tmp32 & 0x02000000) tmp32 |= 0xfc000000;
  //tmps32 = ((SINT32)tmp32 >> 16)*(SINT32)49371;
  //tmps32 += ((tmp32 & 0x0000ffff)*49371) >> 16;
  tmps32 = ((SINT32)tmp32 >> 16)*(SINT32)attn;
  tmps32 += ((tmp32 & 0x0000ffff)*attn) >> 16;
  tmps32 = (tmps32*64)/accmulation_count;
#if IQMC_ROC_DEBUG
  dc.imag = tmps32 >> 6;
#else
  dc.real = tmps32 >> 6;
#endif

  //tmp32 = drp_srm_api->inout.rx.roc.output.accum_I[0];
  READ_WORD(drp_regs->ROC_CALC_IL, mem_data);
  tmp32 = (UINT32) (0x0000FFFF & (UINT32) mem_data);
  READ_WORD(drp_regs->ROC_CALC_IH, mem_data);
  tmp32 |= ((UINT32) mem_data)<<16;

// sign extend
//  if(tmp32 & 0x02000000)  tmp32 |= 0xfc000000;
//  tmps32 = (SINT32)tmp32;
//  tmps32 = (tmps32*64)/accmulation_count;
//  tmps32 = (SINT32)((float)tmps32 * 0.75335);
//  dc.imag = tmps32 >> 6;
  if(tmp32 & 0x02000000) tmp32 |= 0xfc000000;
  //tmps32 = ((SINT32)tmp32 >> 16)*(SINT32)49371;
  //tmps32 += ((tmp32 & 0x0000ffff)*49371) >> 16;
  tmps32 = ((SINT32)tmp32 >> 16)*(SINT32)attn;
  tmps32 += ((tmp32 & 0x0000ffff)*attn) >> 16;
  tmps32 = (tmps32*64)/accmulation_count;
#if IQMC_ROC_DEBUG
  dc.real = tmps32 >> 6;
#else
  dc.imag = tmps32 >> 6;
#endif

#if IQMC_ROC_DEBUG
  READ_WORD(drp_regs->IQMC_CWL, mem_data);
  if ((mem_data >> 4) & 1)
  {
    // Do IQMC adjustment.
    READ_WORD(drp_regs->IQMC_W0_INL, mem_data);
    iqmc_coeff.imag = mem_data;
    READ_WORD(drp_regs->IQMC_W0_INH, mem_data);
    iqmc_coeff.real = mem_data;
    //    DC_I = DC_I - (DC_I*Coefficent_real + DC_Q*Coefficent_imag)
    //    DC_Q = DC_Q - (DC_I*Coefficent_imag - DC_Q*Coefficent_real)
    //    The multiples in the '()' are dones as follows to model as done in IQMC.
    //      A*B = (((A*B >> 15) + 1) >> 2)
    // compute adjust for DC.real = dc.real*iqmc.coeff.real + dc.imag*iqmc.coeff.imag
    tmps32=dc.real*(SINT32)iqmc_coeff.real;
    tmps32=tmps32>>15;
    tmps32++;
    tmps32=tmps32>>1;
    dc_adjust.real=tmps32;
    tmps32=dc.imag*(SINT32)iqmc_coeff.imag;
    tmps32=tmps32>>15;
    tmps32++;
    tmps32=tmps32>>1;
    dc_adjust.real+=tmps32;

    // compute adjust for DC.imag = dc.real*iqmc.coeff.imag - dc.imag*iqmc.coeff.real
    tmps32=dc.real*(SINT32)iqmc_coeff.imag;
    tmps32=tmps32>>15;
    tmps32++;
    tmps32=tmps32>>1;
    dc_adjust.imag=tmps32;
    tmps32=dc.imag*(SINT32)iqmc_coeff.real;
    tmps32=tmps32>>15;
    tmps32++;
    tmps32=tmps32>>1;
    dc_adjust.imag-=tmps32;

    // adjust_dc
    dc.real -= dc_adjust.real;
    dc.imag -= dc_adjust.imag;
  }
  tmps32 = dc.imag;
  dc.imag = dc.real;
  dc.real = tmps32;
#endif //#if IQMC_ROC_DEBUG

  // Do the ROC compensation on the data in the rx buffer.
  // Generate ZIF tone complex value and scale it by the DC.
  // Subtract the scaled value from the data in the RX Buffer.
  // The ZIF tone is geneated using the 128 entry LUT that
  // that contains 1 quadrant of the ZIF LUT.
  // The logic matches that used by the ZIF block.
  // The decimate value is used to index the LUT.


  re_offset=start_index;
  im_offset=(re_offset+128) & 0x1ff;

#if ROC_DEBUG
  err = 0.0;
#endif

  for (ix = 0;ix<num_iqsamples;ix++) {

    if (re_offset<256) {
      if(re_offset>=128) re_idx=255-re_offset;
      else re_idx=re_offset;
      zif_tone.real=ziflut[re_idx];
    }
    else {
      if (re_offset<384) re_idx=(re_offset-256);
      else re_idx=511-re_offset;
      zif_tone.real=-ziflut[re_idx];
    }
    if (im_offset<256) {
      if(im_offset>=128) im_idx=255-im_offset;
      else im_idx=im_offset;
      zif_tone.imag=ziflut[im_idx];
    }
    else {
      if (im_offset<384) im_idx=(im_offset-256);
      else im_idx=511-im_offset;
      zif_tone.imag=-ziflut[im_idx];
    }

    re_offset1 = (re_offset + 1) %512;
    im_offset1 = (im_offset + 1) %512;

    if (re_offset1<256) {
      if(re_offset1>=128) re_idx=255-re_offset1;
      else re_idx=re_offset1;
      zif_tone1.real=ziflut[re_idx];
    }
    else {
      if (re_offset1<384) re_idx=(re_offset1-256);
      else re_idx=511-re_offset1;
      zif_tone1.real=-ziflut[re_idx];
    }
    if (im_offset1<256) {
      if(im_offset1>=128) im_idx=255-im_offset1;
      else im_idx=im_offset1;
      zif_tone1.imag=ziflut[im_idx];
    }
    else {
      if (im_offset1<384) im_idx=(im_offset1-256);
      else im_idx=511-im_offset1;
      zif_tone1.imag=-ziflut[im_idx];
    }

    tmps32 = (((SINT32)zif_tone.real * ((1 <<16) - alpha)) + ((SINT32)zif_tone1.real * alpha)) >> 16;
    zif_tone.real = (SINT16)tmps32;
    tmps32 = (((SINT32)zif_tone.imag * ((1<<16) - alpha)) + ((SINT32)zif_tone1.imag * alpha)) >> 16;
    zif_tone.imag = (SINT16)tmps32;

#if ROC_DEBUG
    zif_tone_gen1[ix].real = zif_tone.real;
    zif_tone_gen1[ix].imag = zif_tone.imag;
#endif

    // increment re_offset and im_offset
    re_offset=(re_offset+lut_incr) & 0x1ff;
    im_offset=(im_offset+lut_incr) & 0x1ff;

    // scale the LUT entry by the DC and subtract from the rx buffer entry.
    // real part
    tmps32 = ((dc.real * (SINT32) zif_tone.real) - (dc.imag * (SINT32) zif_tone.imag)) >> 15;
#if ROC_DEBUG
    err1 = (float)(rxbufptr[ix].real - (SINT16) tmps32);
    subtracted_tone[ix].real = rxbufptr[ix].real - (SINT16) tmps32;
#else
    rxbufptr[ix].real = rxbufptr[ix].real - (SINT16) tmps32;
#endif

#if ROC_DEBUG
    zif_tone_gen2[ix].real = tmps32;
#endif

    // imaginary part
    tmps32 = ((dc.real * (SINT32) zif_tone.imag) + (dc.imag * (SINT32) zif_tone.real)) >> 15;
#if ROC_DEBUG
    err2 = (float)(rxbufptr[ix].imag - (SINT16) tmps32);
    err += (err1*err1) + (err2*err2);
    subtracted_tone[ix].imag = rxbufptr[ix].imag - (SINT16) tmps32;
#else
    rxbufptr[ix].imag = rxbufptr[ix].imag - (SINT16) tmps32;
#endif

#if ROC_DEBUG
  zif_tone_gen2[ix].imag = tmps32;
#endif

  }

#if ROC_DEBUG
  }
#endif

   return;
}
#endif //#if (DRP_TEST_SW && (!DRP_BURNIN && !DRP_TDL_DFT))

#if (DRP_TEST_SW && DRP_BENCH_SW)
 /**
 * Arrange input samples in bit-reverse addressing order
 * the index j is the bit reverse of i
 * @param samples          complex samples array to arrange in-place
 * @param nb_samples_pow2  number of complex samples as a power of 2
 */
static void bit_rev(tFloatingPointComplex* samples, unsigned short nb_samples_pow2)
  {
    unsigned short        i, j, k;
    unsigned short        nb_samples      = 1 << nb_samples_pow2; /* Number of points for FFT */
    unsigned short        half_nb_samples = nb_samples >> 1;
    tFloatingPointComplex temp;                                   /* temporary storage of complex variable */

    for (j = 0, i = 1; i < nb_samples - 1; i++)
      {
        k = half_nb_samples;
        while ( k <= j )
          {
            j -= k;
            k >>= 1;
          }
        j += k;

        if ( i < j )
          {
            temp.Real       = samples[j].Real;
            temp.Imag       = samples[j].Imag;

            samples[j].Real = samples[i].Real;
            samples[j].Imag = samples[i].Imag;

            samples[i].Real = temp.Real;
            samples[i].Imag = temp.Imag;
          }
      }
  }


/**************************************************************************************/
/* Radix 2 decimation in time FFT                                                     */
/* the output overwrite the input array                                               */
/**************************************************************************************/
static void fft(tFloatingPointComplex* samples, unsigned short nb_input_samples)
  {
    tFloatingPointComplex temp;                               /* temporary storage of complex variable */
    tFloatingPointComplex temp2;                              /* temporary storage of complex variable */
    tFloatingPointComplex U;                                  /* Twiddle factor W^k */
    unsigned short        i;
    unsigned short        j;
    unsigned short        id;                                 /* Index for lower point in butterfly */
    unsigned short        L;                                  /* FFT stage */
    unsigned short        LE;                                 /* Number of points in sub DFT at stage L and offset to next DFT in stage */
    unsigned short        LE1;                                /* Number of butterflies in one DFT at stage L.  Also is offset to lower point in butterfly at stage L */
    unsigned short        nb_samples = 1;                     /* Number of points      for FFT */
    unsigned short        nb_samples_pow2 = 0;                /* Number of points pow2 for FFT */
    short                 twiddle_factor_index  = ((MAX_NB_SAMPLES - 1) << nb_samples_pow2) & (MAX_NB_SAMPLES - 1);

    /*
     * Compute the size of the FFT from the size of the input vector
     */
    while ( (nb_samples < nb_input_samples) && (nb_samples_pow2 < MAX_NB_SAMPLES_POW2) )
      {
        nb_samples_pow2++;
        nb_samples *= 2;
      }
    if ( nb_samples != nb_input_samples )
      {
        /* invalid nb_inputs_samples */
        return;
      }

    twiddle_factor_index  = ((MAX_NB_SAMPLES - 1) << nb_samples_pow2) & (MAX_NB_SAMPLES - 1);

    for (L = 0; L < nb_samples_pow2; L++)  /* FFT butterfly */
      {
        LE  = 1 <<  (nb_samples_pow2 - L);  /* LE=2^L=points of sub DFT */
        LE1 = LE >> 1;                      /* Number of butterflies in sub DFT */

        for (j = 0; j < LE1; j++)
          {
            U.Real  = twiddle_factor_LUT[twiddle_factor_index].Real;
            U.Imag  = twiddle_factor_LUT[twiddle_factor_index].Imag;
//            U.Real  = cos(j * M_PI / LE1);
//            U.Imag  = -sin(j * M_PI / LE1);

            twiddle_factor_index++;
            for (i = j; i < nb_samples; i += LE) /* Do the butterflies */
              {
                id                = i + LE1;

                temp.Real         = samples[id].Real;
                temp.Imag         = samples[id].Imag;
                temp2.Real        = samples[i].Real;
                temp2.Imag        = samples[i].Imag;

                samples[i].Real   = temp2.Real + temp.Real;
                samples[i].Imag   = temp2.Imag + temp.Imag;
                temp2.Real       -= temp.Real;
                temp2.Imag       -= temp.Imag;

                samples[id].Real  = temp2.Real * U.Real - temp2.Imag * U.Imag;
                samples[id].Imag  = temp2.Real * U.Imag + temp2.Imag * U.Real;
              }
          }
      }

    /*
     * Arrange sample in bit reversal order
     */
    bit_rev(samples, nb_samples_pow2);
  }





// /**
//  * compute the fft of the I or Q part of input signal applying an hamming window, store the result in a complex array
//  * @param input_IQ_samples  input vector of I/Q samples, vector size is 2^nb_samples_pow2
//  * @param output_FFT        output vector that will receive the FFT of the I or Q part of the input signal, vector size is 2^nb_samples_pow2
//  * @param nb_samples_pow2   size of vectors as a power of 2
//  * @param data_type_to_process  if PROCESS_IDATA, process the I samples of the input vector, else the Q samples
//  * @return 0 if no error, <>0 if an error is detected
//  * /
// short compute_fft(tIQsample* input_IQ_samples, tComplex* output_FFT, unsigned short nb_samples_pow2, short data_type_to_process)
//   {
//     unsigned short  i;
//     unsigned short  j;
//     unsigned short  nb_samples;
//
//     if ( nb_samples_pow2 > MAX_NB_SAMPLES_POW2 )
//       {
//         return -1;
//       }
//
//     /*
//      * Extract I or Q samples and apply to them an hamming window on the input samples
//      * /
//     nb_samples = 1 << nb_samples_pow2;
//
//  switch( data_type_to_process)
//  {
//    case PROCESS_IDATA:
//       for (i = 0, j = 0; i < nb_samples; i++, j += 1 << (MAX_NB_SAMPLES_POW2 - nb_samples_pow2))
//          {
//            output_FFT[i].Real = 0;
// /*           output_FFT[i].Imag = Q31_TO_Q15(_lsmpy(input_IQ_samples[i].I, hamming_window[j]));* /
//            output_FFT[i].Imag = input_IQ_samples[i].I;
//
//          }
//    break;
//    case PROCESS_QDATA:
//
//        for (i = 0, j = 0; i < nb_samples; i++, j += 1 << (MAX_NB_SAMPLES_POW2 - nb_samples_pow2))
//        {
// /*           output_FFT[i].Real = Q31_TO_Q15(_lsmpy(input_IQ_samples[i].Q, hamming_window[j]));* /
//            output_FFT[i].Real = input_IQ_samples[i].Q;
//            output_FFT[i].Imag = 0;
//          }
//    break;
//    case PROCESS_IQDATA:
//
//        for (i = 0, j = 0; i < nb_samples; i++, j += 1 << (MAX_NB_SAMPLES_POW2 - nb_samples_pow2))
//        {
// /*           output_FFT[i].Real = Q31_TO_Q15(_lsmpy(input_IQ_samples[i].Q, hamming_window[j]));
//            output_FFT[i].Imag = Q31_TO_Q15(_lsmpy(input_IQ_samples[i].I, hamming_window[j]));* /
//            output_FFT[i].Real = input_IQ_samples[i].Q;
//            output_FFT[i].Imag = input_IQ_samples[i].I;
//
//         }
//    break;
//    default: return -2;
//    }
//
//      /*
//       * Compute the forward FFT on windowed samples
//       * /
//     fft(output_FFT, nb_samples_pow2);
//
//     return 0;
//   }


/**
 * compute the fft of the I or Q part of input signal applying a window, store the result in a complex array
 * @param input_IQ_samples  input vector of I/Q samples, vector size is nb_input_samples
 * @param output_FFT        output vector that will receive the FFT of the I and/or Q part of the input signal, vector size is nb_samples
 * @param nb_input_samples   size of vectors
 * @param data_type_to_process  if PROCESS_IDATA, process the I samples of the input vector, else the Q samples
 * @return 0 if no error, <>0 if an error is detected
 */
short compute_fft(tIQsample* input_IQ_samples, tFloatingPointComplex* output_FFT, unsigned short nb_input_samples, short data_type_to_process, tFloatingPointReal* window)
  {
    unsigned short  i;
    unsigned short  nb_samples = 1;       /* Number of points needed to compute the FFT */
    unsigned short  nb_samples_pow2 = 0;  /* Number of points pow2 needed to compute the FFT */

    /* compute the size of the FFT from the size of the input vector */
    /* FFT size will be rounded up to the next power of two */
    while ( (nb_samples < nb_input_samples) && (nb_samples_pow2 < MAX_NB_SAMPLES_POW2) )
      {
        nb_samples_pow2++;
        nb_samples *= 2;
      }

     /* Extract I/Q samples and apply to them the specified window */
    switch (data_type_to_process)
      {
        case PROCESS_IDATA:
          /* FFT only on I data */
          for (i = 0; i < nb_input_samples; i++)
            {
              output_FFT[i].Real = input_IQ_samples[i].I * window[i];
              output_FFT[i].Imag = 0.0;
            }
        break;
        case PROCESS_QDATA:
          /* FFT only on Q data */
          for (i = 0; i < nb_input_samples; i++)
            {
              output_FFT[i].Real = input_IQ_samples[i].Q * window[i];
              output_FFT[i].Imag = 0.0;
            }
        break;
        case PROCESS_IQDATA:
          /* FFT on complex data */
          for (i = 0; i < nb_input_samples; i++)
            {
              output_FFT[i].Real = input_IQ_samples[i].I * window[i];
              output_FFT[i].Imag = input_IQ_samples[i].Q * window[i];
            }
        break;
        default:
          return -2;
      }

    /* zero padding on remaining samples */
    for (; i < nb_samples; i++)
      {
        output_FFT[i].Real = 0;
        output_FFT[i].Imag = 0;
      }

     /* compute the forward FFT on windowed samples */
    fft(output_FFT, nb_samples);

    return 0;
  }

//#if 0
/**************************************************************************************/
/* Calculates a number of window functions. The following window                      */
/* functions are currently implemented: Boxcar, Triang, Hanning,                      */
/* Hamming, Blackman, Flattop and Kaiser.                                             */
/**************************************************************************************/

void real_WINDOW(int window_type, unsigned short window_size, int normalisation_type, tFloatingPointReal* extra_parameters, tFloatingPointReal* window)
  {
    unsigned short  i;
  unsigned short normalize_it;
    tFloatingPointReal  normalization_coeff;

    switch ( window_type )
      {
        case RECTANGULAR_WINDOW:
        default:
          /*
           * w(k) = 1.0
           */
          {
            for (i = 0 ; i < window_size ; i++)
              window[i] = 1.0;
          }
          break;

        case TRIANGULAR_WINDOW:
          /*
           *                  |    (N-1)|
           *              2 * |k - -----|
           *                  |      2  |
           * w(k) = 1.0 - ---------------
           *                       N-1
           */
          {
            //tFloatingPointReal  k1  = (tFloatingPointReal)(window_size & 1);
/*          tFloatingPointReal  k2  = 1.0 / ((tFloatingPointReal)window_size + k1); */
            tFloatingPointReal  k2  = 1.0 / ((tFloatingPointReal)window_size - 1);
            unsigned short      end = (window_size + 1) >> 1;

            for (i = 0 ; i < end ; i++)
              {
/*              window[i] = window[window_size - i - 1] = (2.0 * ((tFloatingPointReal)(i + 1)) - (1.0 - k1)) * k2; */
                window[i] = window[window_size - i - 1] = (2.0 * (tFloatingPointReal)i) * k2;
              }

          }
          break;

        case HANNING_WINDOW:
          /*
           *                      2*pi*k
           * w(k) = 0.5 - 0.5*cos(------), where 0 <= k < N
           *                       N-1
           */
          {
            tFloatingPointReal k = 2*M_PI/((tFloatingPointReal)(window_size-1));  /* 2*pi/(N-1) */

            for (i = 0; i < window_size; i++)
              window[i] = 0.5*(1.0 - cos(k * i));
          }
          break;

        case HAMMING_WINDOW:
          /*
           *                        2*pi*k
           * w(k) = 0.54 - 0.46*cos(------), where 0 <= k < N
           *                         N-1
           */
          {
            tFloatingPointReal k = 2*M_PI/((tFloatingPointReal)(window_size-1));  /* 2*pi/(N-1) */

            for (i = 0; i < window_size; i++)
              window[i] = 0.54 - 0.46*cos(k*(tFloatingPointReal)i);
          }
          break;

        case BLACKMAN_WINDOW:
          /*
           *                       2*pi*k             4*pi*k
           * w(k) = 0.42 - 0.5*cos(------) + 0.08*cos(------), where 0 <= k < N
           *                        N-1                 N-1
           */
          {
            tFloatingPointReal k1 = 2*M_PI/((tFloatingPointReal)(window_size-1)); /* 2*pi/(N-1) */
            tFloatingPointReal k2 = 2*k1;                                         /* 4*pi/(N-1) */

            for (i = 0; i < window_size; i++)
              window[i] = 0.42 - 0.50*cos(k1*(tFloatingPointReal)i) + 0.08*cos(k2*(tFloatingPointReal)i);
          }
          break;

        case FLATTOP_WINDOW:
        case KAISER_WINDOW:
           /*
           *                                        2*pi*k                     4*pi*k
           * w(k) = 0.2810638602 - 0.5208971735*cos(------) + 0.1980389663*cos(------), where 0 <= k < N
           *                                          N-1                        N-1
           */
          {
            tFloatingPointReal k1 = 2*M_PI/((tFloatingPointReal)(window_size-1)); /* 2*pi/(N-1) */
            tFloatingPointReal k2 = 2*k1;                                         /* 4*pi/(N-1) */

            for (i = 0; i < window_size; i++)
              window[i] = 0.2810638602 - 0.5208971735*cos(k1*(tFloatingPointReal)i) + 0.1980389663*cos(k2*(tFloatingPointReal)i);
          }
    break;
#if 0
        case KAISER_WINDOW:
          /*
           * extra_parameters[0]: beta parameter of Kaiser window, Beta >= 1
           *
           * Beta trades the rejection of the low pass filter against the
           * transition width from passband to stop band.  Larger Beta means a
           * slower transition and greater stop band rejection.  See Rabiner and
           * Gold (Theory and Application of DSP) under Kaiser windows for more
           * about Beta.  The following table from Rabiner and Gold gives some
           * feel for the effect of Beta:
           *
           * All ripples in dB, width of transition band = D*N where N = window
           * length
           *
           * BETA    D       PB RIP   SB RIP
           * 2.120   1.50  +-0.27      -30
           * 3.384   2.23    0.0864    -40
           * 4.538   2.93    0.0274    -50
           * 5.658   3.62    0.00868   -60
           * 6.764   4.32    0.00275   -70
           * 7.865   5.0     0.000868  -80
           * 8.960   5.7     0.000275  -90
           * 10.056  6.4     0.000087  -100
           */
          {
            tFloatingPointReal  b = extra_parameters[0];
            tFloatingPointReal  tmp;
            tFloatingPointReal  k1  = 1.0 / besselizero(b);
            int                 k2  = 1 - (window_size & 1);
            int                 end = (window_size + 1) >> 1;

            for (i = 0; i < end; i++)
              {
                tmp = (tFloatingPointReal)(2*i + k2) / ((tFloatingPointReal)window_size - 1.0);
                window[end-(1&(!k2))+i] = window[end-1-i] = k1 * besselizero(b*sqrt(1.0 - tmp*tmp));
              }
          }
    break;
#endif
   }

    /*
     * Normalize the window if needed
     */
    normalization_coeff = 1.0;
    normalize_it        = 0;
    switch ( normalisation_type )
      {
        case WINDOW_MAX_NORMALIZATION:
          {
            tFloatingPointReal  window_MAX_value;
            unsigned short      window_MAX_pos;

            real_PEAK(window, &window_MAX_value, &window_MAX_pos, window_size);

            if ( window_MAX_value > 0.0 )
              {
                normalization_coeff = 1.0 / window_MAX_value;
                normalize_it        = 1;
              }
          }
    break;

        case WINDOW_POWER_NORMALIZATION:
          {
            tFloatingPointReal  window_POWER;

            real_POWER(window, &window_POWER, window_size);

            if ( window_POWER > 0.0 )
              {
                normalization_coeff = 1.0 / sqrt(window_POWER);
                normalize_it        = 1;
              }
          }
    break;

    default:
    break;
  }

    if ( normalize_it )
      {
        for (i = 0; i < window_size; i++)
          window[i] *= normalization_coeff;
      }
  }

/**************************************************************************************/
/* Return the max absolute value of the real signal and its position                  */
/**************************************************************************************/
void real_PEAK(tFloatingPointReal* samples, tFloatingPointReal* MAX_value, unsigned short* MAX_pos, unsigned short nb_samples)
  {
    tFloatingPointReal  max;
    tFloatingPointReal  norm;
    unsigned short      i, pos;

    max = real_NORM(samples++);
    pos = 1;
    for (i = 1; i < nb_samples; i++)
      {
        norm     = real_NORM(samples++);
        if ( norm > max )
          {
            max = norm;
            pos = i;
          }
      }

    *MAX_value = max;
    *MAX_pos   = pos;
  }

void real_POWER(tFloatingPointReal* samples, tFloatingPointReal* POWER_value, unsigned short nb_samples)
  {
    real_ENERGY(samples, POWER_value, nb_samples);
    *POWER_value /= (tFloatingPointReal)nb_samples;
  }

tFloatingPointReal real_NORM2(tFloatingPointReal* samples)
  {
    return ((*samples) * (*samples));
  }

tFloatingPointReal real_NORM(tFloatingPointReal* samples)
  {
    return fabs(*samples);
  }


void real_ENERGY(tFloatingPointReal* samples, tFloatingPointReal* ENERGY_value, unsigned short nb_samples)
  {
    tFloatingPointReal  energy;
    unsigned short      i;

    energy  = 0.0;
    for (i = 0; i < nb_samples; i++)
      {
        energy += real_NORM2(samples++);
      }

    *ENERGY_value = energy;
  }
//#endif //#if 0

#endif //#if (DRP_TEST_SW && DRP_BENCH_SW)

#if (DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW))
/**************************************************************************************/
/* compute average over different realization of a complex vector                     */
/**************************************************************************************/
void compute_running_avg_fixed(UINT32* input_samples, float* output_samples,
                                          UINT16 nb_samples, UINT16 avg_type, UINT16 num_avg, UINT16 mag_shift) {
  UINT16 i;

  if (avg_type == AVG_RESET) {
    for(i = 0; i < nb_samples; i++) {
      output_samples[i] = 0;
    }
  } else {
    for(i = 0; i < nb_samples; i++) {
      output_samples[i] = output_samples[i] + (float)input_samples[i] * (1<<mag_shift);
    }
  }
}
#endif //#if (DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW))

#if (DRP_TEST_SW && DRP_BENCH_SW)
/**************************************************************************************/
/* compute average over different realization of a complex vector */
/**************************************************************************************/
void compute_running_avg(tFloatingPointComplex* input_samples, tFloatingPointComplex* output_samples,
                         unsigned short nb_samples, unsigned short avg_type) {
  static uint16 N = 0;
  uint16 i;

  if (avg_type == AVG_RESET) {
    N = 0;
    for(i = 0; i < nb_samples; i++) {
      output_samples[i].Real = output_samples[i].Imag = 0.0;
    }
  } else {
    N++;
    for(i = 0; i < nb_samples; i++) {
      output_samples[i].Real = ((N - 1) * output_samples[i].Real + input_samples[i].Real) / N;
      output_samples[i].Imag = ((N - 1) * output_samples[i].Imag + input_samples[i].Imag) / N;
    }
  }
}
#endif //#if (DRP_TEST_SW && DRP_BENCH_SW)


#if (DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW))

UINT16 compute_fft_iq_pwr_fixed(CMPLX16* input_samples, UINT32* output_samples,
                                          SINT16 data_type_to_process, SINT16 fft_shift, UINT16 phase_bin) {

  UINT16 i;
  SINT32 re_i, re_q, im_i, im_q;
  UINT32 temp;

  switch (data_type_to_process) {
    case PROCESS_IDATA: /* compute the power spectrum for I data only */
      // values for samples[0].Real is already in the correct place
      // DC value and power for I

      for(i = 0; i <= (FFT_BUF_SIZE >> 1); i++) {

        if ((i == 0) || (i == (FFT_BUF_SIZE >> 1)))
        {
          re_i = (SINT32) input_samples[i].real;
          im_i = 0;
        }
        else
        {
          re_i = ((SINT32) input_samples[i].real + (SINT32) input_samples[FFT_BUF_SIZE - i].real) >> 1;
          im_i = ((SINT32) input_samples[i].imag - (SINT32) input_samples[FFT_BUF_SIZE - i].imag) >> 1;
        }

        // put the power spectrum in the real part
        temp = (re_i * re_i) + (im_i * im_i);

        output_samples[i] = temp >> FFT_BUF_SIZE_LOG2;

        // power spectrum has to be symmetrical
        if ((i != (FFT_BUF_SIZE >> 1)) && (i != 0))
          output_samples[FFT_BUF_SIZE - i] = output_samples[i];
      }
    break;

    case PROCESS_QDATA: /* compute the power spectrum for Q data only */
      // values for samples[0].Imag is already in the correct place
      // DC value and power for Q

      for(i = 0; i <= (FFT_BUF_SIZE / 2); i++) {

        if ((i == 0) || (i == (FFT_BUF_SIZE >> 1)))
        {
          re_q = (SINT32) input_samples[i].imag;
          im_q = 0;
        }
        else
        {
          re_q = ((SINT32) input_samples[i].imag + (SINT32) input_samples[FFT_BUF_SIZE - i].imag) >> 1;
          im_q = ((SINT32) input_samples[FFT_BUF_SIZE - i].real - (SINT32) input_samples[i].real) >> 1;
        }

        // put the power spectrum in the real part
        temp = (re_q * re_q) + (im_q * im_q);

        output_samples[i] = temp >> FFT_BUF_SIZE_LOG2;

        // power spectrum has to be symmetrical
        if ((i != (FFT_BUF_SIZE >> 1)) && (i != 0))
          output_samples[FFT_BUF_SIZE - i] = output_samples[i];
      }

      // values for samples[nb_samples/2].Real is already in the correct place
      // DC value and power for I
    break;

    case PROCESS_IQDATA: /* compute the power spectrum for I+jQ data  */
      for(i = 0; i < FFT_BUF_SIZE; i++) {

        re_i = (SINT32)input_samples[i].real;
        im_i = (SINT32)input_samples[i].imag;
        // put the power spectrum in the real part
        temp = (re_i * re_i) + (im_i * im_i);

        output_samples[i] = temp >> FFT_BUF_SIZE_LOG2;
      }
    break;
  }

  return (2*fft_shift);
}

#endif //#if (DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW))

#if (DRP_TEST_SW && DRP_BENCH_SW)
/**************************************************************************************/
/* from the complex FFT output stored in DSP memory compute the power spectrum.       */
/* The user can select to compute the spectra for I only, for Q only anf for I+jQ.    */
/* The output result is stored in a complex array, which contains the power spectrum  */
/* in the real part and the phase spectum in the imaginary part.                      */
/* The input buffer for this service routine is a set of 32 bits word (floating point */
/* numbers) organized sequentially as real part first and imaginary part after for    */
/* each complex data, as obtained from the FFT routine.                               */
/**************************************************************************************/
void compute_fft_iq_pwr(tFloatingPointComplex* input_samples, tFloatingPointComplex* output_samples,
                        unsigned short nb_samples, short data_type_to_process) {
  uint16 i;
  float re_i, im_i, re_q, im_q;

  switch (data_type_to_process) {
    case PROCESS_IDATA: /* compute the power spectrum for I data only */
      // values for samples[0].Real is already in the correct place
      // DC value and power for I
      re_i = input_samples[0].Real;
      output_samples[0].Real = (re_i * re_i) / nb_samples;
      output_samples[0].Imag =0.0;

      for(i = 1; i < (nb_samples / 2); i++) {
        re_i = (input_samples[i].Real + input_samples[nb_samples - i].Real) / 2;
        im_i = (input_samples[i].Imag - input_samples[nb_samples - i].Imag) / 2;
        // put the power spectrum in the real part
        output_samples[i].Real = (re_i * re_i + im_i * im_i) / nb_samples;
      // power spectrum has to be symmetrical
      output_samples[nb_samples - i].Real = output_samples[i].Real;
        // put the phase spectrum in the imaginary part
        output_samples[i].Imag = atan2(im_i, re_i);
      // phase spectrum has to be antysymmetrical
      output_samples[nb_samples - i].Imag = -output_samples[i].Imag;
      }

    // values for samples[nb_samples/2].Real is already in the correct place
      // DC value and power for I
      re_i = input_samples[nb_samples / 2].Real;
      output_samples[nb_samples / 2].Real = (re_i * re_i) / nb_samples;
    output_samples[nb_samples / 2].Imag =0.0;
    break;

    case PROCESS_QDATA: /* compute the power spectrum for Q data only */
      // values for samples[0].Imag is already in the correct place
      // DC value and power for Q
      re_i = input_samples[0].Imag;
      output_samples[0].Real = (re_i * re_i) / nb_samples;
      output_samples[0].Imag =0.0;

      for(i = 1; i < (nb_samples / 2); i++) {
        re_q = (input_samples[i].Imag + input_samples[nb_samples - i].Imag) / 2;
        im_q = (-input_samples[i].Real + input_samples[nb_samples - i].Real) / 2;
        // put the power spectrum in the real part
        output_samples[i].Real = (re_q * re_q + im_q * im_q) / nb_samples;
      // power spectrum has to be symmetrical
      output_samples[nb_samples - i].Real = output_samples[i].Real;
        // put the phase spectrum in the imaginary part
        output_samples[i].Imag = atan2(im_q, re_q);
      // phase spectrum has to be antysymmetrical
      output_samples[nb_samples - i].Imag = -output_samples[i].Imag;
}

    // values for samples[nb_samples/2].Real is already in the correct place
      // DC value and power for I

      re_q = input_samples[nb_samples / 2].Imag;
      output_samples[nb_samples / 2].Real = (re_q * re_q) / nb_samples;
    output_samples[nb_samples / 2].Imag =0.0;
    break;

    case PROCESS_IQDATA: /* compute the power spectrum for I+jQ data  */
      for(i = 0; i < nb_samples; i++) {
        re_i = input_samples[i].Real;
        im_i = input_samples[i].Imag;
        // put the power spectrum in the real part
        output_samples[i].Real = (re_i * re_i + im_i * im_i) / nb_samples;
        // put the phase spectrum in the imaginary part
        output_samples[i].Imag = atan2(im_i, re_i);
      }
      break;
  }
}

#endif //#if (DRP_TEST_SW && DRP_BENCH_SW)

#if (DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW))

/**************************************************************************************/
/*                                    FFT Routine                                            */
/**************************************************************************************/

UINT32 fft_avg_ate(UINT16 band, UINT16 channel, UINT16 ABE_gain_step, UINT16 AFE_gain_step,
               UINT16 pole_num, UINT16 roc_setting, UINT16 gain_comp_enable, UINT16 phase_mm_bin,
                        UINT16 num_avg, UINT16 win_type, UINT16 win_norm_type, UINT16 pcb_config,
                        UINT16 captureburst_computefft_flag) {
  UINT16 index1 = 0; // RX burst index
  UINT16 index2 = 0; // Results data sorting index
  UINT16 index3 = 0; // Decimation index
  float  phase_mm_diff = 0;
  float I_TonePhase = 0;
  float Q_TonePhase = 0;

  SINT16 *rx_data_buffer = (SINT16*)DRP_SRM_RX_BUFFER_ADDR;
  SINT16 *buf_ptr;
  SINT16 mag_shift;
  UINT16 fft_shift = 0;
  SINT16 re_i, im_i, re_q, im_q;

  UINT32* TotalResults      = (UINT32 *)&FFT_Pwr_Avg_I_ate[0];
  float PhaseMismatch = 0.0;

  CplxS16   *p_Samples = (CplxS16 *) &g_rx_data_buffer_decimated_acum[0];
  S16       *p_Cos = &CosTable[0];
  U16       *p_BitReverse = &BitReverseTable[0];

  // Zero out FFT Averages for I, Q, I+jQ Channels, and Phase Mismatch
  compute_running_avg_fixed(FFT_Pwr_I_ate, FFT_Pwr_Avg_I_ate, \
                            3*FFT_BUF_SIZE, AVG_RESET, num_avg, 0);

  // Calculate FFT Average
  for (index1 = 0; index1 < num_avg; index1++) {

    p_Samples = (CplxS16 *) &g_rx_data_buffer_decimated_acum[index1 * FFT_BUF_SIZE * 2];

    if (captureburst_computefft_flag == FFT_AVG_CAPTURE_BURST)
    {
      //always use burst mode for FFT
      drp_param5L = BURST_MODE;

      // Execute an RX burst
      rx_timeline_new(band, channel, ABE_gain_step, AFE_gain_step, gain_comp_enable, pole_num, pcb_config);

      // ROC compensation
      if (roc_setting == ENABLE_ROC)
        ref_roc((CMPLX16 *)DRP_SRM_RX_BUFFER_ADDR, 160, 381, (UINT16) 216);

      // Read the DC OFFSET
      drp_srm_dc_offset[index1].i = drp_srm_data->dc_offset_i;
      drp_srm_dc_offset[index1].q = drp_srm_data->dc_offset_q;

      // Read the DRP measured temperature
      drp_measured_temp[index1] = drp_srm_api->inout.temperature.output;

      // Decimate rx_data_buffer (I and Q)
      //   Note: Q and I are reversed coming into RX Buffer
      buf_ptr = (SINT16 *) &g_rx_data_buffer_decimated_acum[index1 * FFT_BUF_SIZE * 2];
      for (index3 = 0; index3 < FFT_BUF_SIZE; index3++)
      {
        buf_ptr[2*index3 + 1] = rx_data_buffer[2*index3] << 1;     // Q
        buf_ptr[2*index3]     = rx_data_buffer[2*index3 + 1] << 1; // I
      }
    }
    else
    {
      fft_shift = 0;

      Fft_ind( p_Samples, p_Cos, p_BitReverse, FFT_BUF_SIZE_LOG2, &fft_shift );

      // Calculates Power and Phase Spectrum for I Channel
      mag_shift = compute_fft_iq_pwr_fixed((CMPLX16*) p_Samples, FFT_Pwr_I_ate, 1, fft_shift, phase_mm_bin);

      // Calculates Power and Phase Spectrum for Q Channel
      mag_shift = compute_fft_iq_pwr_fixed((CMPLX16*) p_Samples, FFT_Pwr_Q_ate, 2, fft_shift, phase_mm_bin);

      // Calculates Power and Phase Spectrum for I+jQ Channel
      mag_shift = compute_fft_iq_pwr_fixed((CMPLX16*) p_Samples, FFT_Pwr_IQ_ate, 3, fft_shift, phase_mm_bin);

      // Get phase mismatch between I and Q channels
      re_i = (p_Samples[phase_mm_bin].x_I + p_Samples[FFT_BUF_SIZE - phase_mm_bin].x_I);
      im_i = (p_Samples[phase_mm_bin].x_Q - p_Samples[FFT_BUF_SIZE - phase_mm_bin].x_Q);

      re_q = (p_Samples[phase_mm_bin].x_Q + p_Samples[FFT_BUF_SIZE - phase_mm_bin].x_Q);
      im_q = (-p_Samples[phase_mm_bin].x_I + p_Samples[FFT_BUF_SIZE - phase_mm_bin].x_I);

      // Get phase mismatch between I and Q channels
      I_TonePhase = atan2(im_i * 1.0, re_i) * (180 / PI);
      Q_TonePhase = atan2(im_q * 1.0, re_q) * (180 / PI);

      if (I_TonePhase < 0)
        I_TonePhase = I_TonePhase + 360;

      if (Q_TonePhase < 0)
        Q_TonePhase = Q_TonePhase + 360;

      phase_mm_diff = I_TonePhase - Q_TonePhase;

      if ((phase_mm_diff >= -360) && (phase_mm_diff < -180))
        phase_mm_diff = phase_mm_diff + 360;
      else if ((phase_mm_diff > 180) && (phase_mm_diff <= 360))
        phase_mm_diff = phase_mm_diff - 360;
      else
        phase_mm_diff = phase_mm_diff;

      PhaseMismatch += phase_mm_diff;

      // Average Power and Phase for I, Q, I+jQ Channels, and Phase Mismatch
      // For I, Q, IQ and PhaseMismatch[0] - hence length = 6*FFT_BUF_SIZE + 2
      compute_running_avg_fixed(FFT_Pwr_I_ate, FFT_Pwr_Avg_I_ate, \
                          3*FFT_BUF_SIZE, AVG_RUN, num_avg, mag_shift);
    }
  }

  if (captureburst_computefft_flag == FFT_AVG_COMPUTE_FFT)
  {
    // Copy power spectrum (only magnitude) results to results buffers
    for (index2 = 0; index2 < FFT_BUF_SIZE; index2++)
    {
      FFT_Pwr_Avg_I_ate[index2] /= num_avg;
      FFT_Pwr_Avg_Q_ate[index2] /= num_avg;
      FFT_Pwr_Avg_IQ_ate[index2] /= num_avg;
    }

    // Move Phase Mismatch into Results buffer
    PhaseMismatch_avg_ate = PhaseMismatch/num_avg;
  }

  return (UINT32)(TotalResults);
}

#endif //#if (DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW))


#if (DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW))

void goertzel(float f, float fs, CMPLX16 *x, UINT16 N, float *mag, float *angle)
{
  float coeff, v0_i, v1_i = 0.0, v2_i = 0.0, v0_q, v1_q = 0.0, v2_q = 0.0, y_i, y_q;
  float arg;
  UINT16 indx;

  x[N].real = 0;
  x[N].imag = 0;

  arg = 2*PI*f/fs;

  coeff = 2*cos(arg);

  for (indx = 0; indx < N + 1; indx++)
  {
    v0_i = x[indx].real + coeff * v1_i - v2_i;
    v2_i = v1_i;
    v1_i = v0_i;

    v0_q = x[indx].imag + coeff * v1_q - v2_q;
    v2_q = v1_q;
    v1_q = v0_q;
  }

  y_i = v0_i - coeff * v2_i/2 - sin(arg) * v2_q;
  y_q = v0_q - coeff * v2_q/2 + sin(arg) * v2_i;

  *mag = y_i * y_i + y_q * y_q;
  *angle = atan2(y_q, y_i);
}

/**************************************************************************************/
/*                        Fixed Point FFT Routine                                     */
/**************************************************************************************/
UINT32 fft_avg_fixed(UINT16 band, UINT16 channel, UINT16 ABE_gain_step, UINT16 AFE_gain_step,
               UINT16 pole_num, UINT16 roc_setting, UINT16 gain_comp_enable, UINT16 phase_mm_bin,
               UINT16 num_avg, UINT16 win_type, UINT16 win_norm_type, UINT16 pcb_config) {
  UINT16 index1 = 0; // RX burst index
  UINT16 index2 = 0; // Results data sorting index
  UINT16 index3 = 0; // Decimation index
  float  phase_mm_diff = 0;
  float I_TonePhase = 0;
  float Q_TonePhase = 0;

  SINT16 *rx_data_buffer = (SINT16*)DRP_SRM_RX_BUFFER_ADDR;
  SINT16 *buf_ptr;
  SINT16 mag_shift;
  UINT16 fft_shift = 0;
  SINT16 re_i, im_i, re_q, im_q;

  UINT32* TotalResults      = (UINT32 *)&FFT_Pwr_Avg_I_ate[0];
  float PhaseMismatch = 0.0;
  float mag, mag_avg = 0.0, angle, angle_avg = 0.0, freq;

  CplxS16   *p_Samples = (CplxS16 *) &g_rx_data_buffer_decimated_acum[0];
  S16       *p_Cos = &CosTable[0];
  U16       *p_BitReverse = &BitReverseTable[0];

  // Zero out FFT Averages for I, Q, I+jQ Channels, and Phase Mismatch
  compute_running_avg_fixed(FFT_Pwr_I_ate, FFT_Pwr_Avg_I_ate, \
                            3*FFT_BUF_SIZE, AVG_RESET, num_avg, 0);

  // Calculate FFT Average
  for (index1 = 0; index1 < num_avg; index1++) {

    p_Samples = (CplxS16 *) &g_rx_data_buffer_decimated_acum[index1 * FFT_BUF_SIZE * 2];

    //always use burst mode for FFT
    drp_param5L = BURST_MODE;

    // Execute an RX burst
    rx_timeline_new(band, channel, ABE_gain_step, AFE_gain_step, gain_comp_enable, pole_num, pcb_config);

    // ROC compensation
    if (roc_setting == ENABLE_ROC)
      ref_roc((CMPLX16 *)DRP_SRM_RX_BUFFER_ADDR, 160, 381, (UINT16) 216);

    // Read the DC OFFSET
    drp_srm_dc_offset[index1].i = drp_srm_data->dc_offset_i;
    drp_srm_dc_offset[index1].q = drp_srm_data->dc_offset_q;

    // Read the DRP measured temperature
    drp_measured_temp[index1] = drp_srm_api->inout.temperature.output;

    // Decimate rx_data_buffer (I and Q)
    //   Note: Q and I are reversed coming into RX Buffer
    buf_ptr = (SINT16 *) &g_rx_data_buffer_decimated_acum[index1 * FFT_BUF_SIZE * 2];
    for (index3 = 0; index3 < FFT_BUF_SIZE; index3++)
    {
      buf_ptr[2*index3 + 1] = rx_data_buffer[2*index3] << 1;     // Q
      buf_ptr[2*index3]     = rx_data_buffer[2*index3 + 1] << 1; // I
    }

    if (drp_param8L == 0)
    {
    fft_shift = 0;

    Fft_ind( p_Samples, p_Cos, p_BitReverse, FFT_BUF_SIZE_LOG2, &fft_shift );

    // Calculates Power and Phase Spectrum for I Channel
    mag_shift = compute_fft_iq_pwr_fixed((CMPLX16*) p_Samples, FFT_Pwr_I_ate, 1, fft_shift, phase_mm_bin);

    // Calculates Power and Phase Spectrum for Q Channel
    mag_shift = compute_fft_iq_pwr_fixed((CMPLX16*) p_Samples, FFT_Pwr_Q_ate, 2, fft_shift, phase_mm_bin);

    // Calculates Power and Phase Spectrum for I+jQ Channel
    mag_shift = compute_fft_iq_pwr_fixed((CMPLX16*) p_Samples, FFT_Pwr_IQ_ate, 3, fft_shift, phase_mm_bin);

    // Get phase mismatch between I and Q channels
    re_i = (p_Samples[phase_mm_bin].x_I + p_Samples[FFT_BUF_SIZE - phase_mm_bin].x_I);
    im_i = (p_Samples[phase_mm_bin].x_Q - p_Samples[FFT_BUF_SIZE - phase_mm_bin].x_Q);

    re_q = (p_Samples[phase_mm_bin].x_Q + p_Samples[FFT_BUF_SIZE - phase_mm_bin].x_Q);
    im_q = (-p_Samples[phase_mm_bin].x_I + p_Samples[FFT_BUF_SIZE - phase_mm_bin].x_I);

    // Get phase mismatch between I and Q channels
    I_TonePhase = atan2(im_i * 1.0, re_i) * (180 / PI);
    Q_TonePhase = atan2(im_q * 1.0, re_q) * (180 / PI);

    if (I_TonePhase < 0)
      I_TonePhase = I_TonePhase + 360;

    if (Q_TonePhase < 0)
      Q_TonePhase = Q_TonePhase + 360;

    phase_mm_diff = I_TonePhase - Q_TonePhase;

    if ((phase_mm_diff >= -360) && (phase_mm_diff < -180))
      phase_mm_diff = phase_mm_diff + 360;
    else if ((phase_mm_diff > 180) && (phase_mm_diff <= 360))
      phase_mm_diff = phase_mm_diff - 360;
    else
      phase_mm_diff = phase_mm_diff;

    PhaseMismatch += phase_mm_diff;

    // Average Power and Phase for I, Q, I+jQ Channels, and Phase Mismatch
    // For I, Q, IQ and PhaseMismatch[0] - hence length = 6*FFT_BUF_SIZE + 2
    compute_running_avg_fixed(FFT_Pwr_I_ate, FFT_Pwr_Avg_I_ate, \
                              3*FFT_BUF_SIZE, AVG_RUN, num_avg, mag_shift);
  }
    else
    {
      freq = (float)(*((UINT32 *)&drp_param6L));
      goertzel(freq, 270833.333, (CMPLX16 *) &buf_ptr[0], FFT_BUF_SIZE, &mag, &angle);
      mag_avg += mag;
      angle_avg += angle;
    }
  }

  if (drp_param8L == 1)
  {
    mag_avg = mag_avg/(num_avg * FFT_BUF_SIZE);
    angle_avg = 180 * angle_avg/(PI * num_avg * FFT_BUF_SIZE);

    *((float *)&drp_param2L) = mag_avg;
    *((float *)&drp_param3L) = angle_avg;
  }
  else
  {
  // Copy power spectrum (only magnitude) results to results buffers
  for (index2 = 0; index2 < FFT_BUF_SIZE; index2++)
  {
    FFT_Pwr_Avg_I_ate[index2] /= num_avg;
    FFT_Pwr_Avg_Q_ate[index2] /= num_avg;
    FFT_Pwr_Avg_IQ_ate[index2] /= num_avg;
  }

  // Move Phase Mismatch into Results buffer
  PhaseMismatch_avg_ate = PhaseMismatch/num_avg;
  }

  return (UINT32)(TotalResults);

}

#endif //#if (DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW))

#if (DRP_TEST_SW && DRP_BENCH_SW)
/**************************************************************************************/
/*                     Floating point FFT Routine                                     */
/**************************************************************************************/
UINT32 fft_avg_float(UINT16 band, UINT16 channel, UINT16 ABE_gain_step, UINT16 AFE_gain_step,
               UINT16 pole_num, UINT16 roc_setting, UINT16 gain_comp_enable, UINT16 phase_mm_bin,
               UINT16 num_avg, UINT16 win_type, UINT16 win_norm_type, UINT16 pcb_config) {
  UINT16 index1 = 0; // RX burst index
  UINT16 index2 = 0; // Results data sorting index
  UINT16 index3 = 0; // Decimation index
  float  phase_mm_diff = 0;
  float I_TonePhase = 0;
  float Q_TonePhase = 0;

  SINT16 *rx_data_buffer = (SINT16*)DRP_SRM_RX_BUFFER_ADDR;

//#if (DRP_TEST_SW && !FIXEDPOINT_FFT)
  float* WinCoef = g_Buffer + 1*FFT_BUF_SIZE;
  float* FFT_IQ = g_Buffer + 2*FFT_BUF_SIZE;
//#endif
  float* FFT_Pwr_I = g_Buffer + 4*FFT_BUF_SIZE;
  float* FFT_Pwr_Q = g_Buffer + 6*FFT_BUF_SIZE;
  float* FFT_Pwr_IQ = g_Buffer + 8*FFT_BUF_SIZE;
  float* PhaseMismatch = g_Buffer + 10*FFT_BUF_SIZE;
  float* FFT_Pwr_Avg_I = g_Buffer + 10*FFT_BUF_SIZE + 2;
  float* FFT_Pwr_Avg_Q = g_Buffer + 12*FFT_BUF_SIZE + 2;
  float* FFT_Pwr_Avg_IQ = g_Buffer + 14*FFT_BUF_SIZE + 2;
  float* PhaseMismatch_Avg = g_Buffer + 16*FFT_BUF_SIZE + 2;

  float* Results_I = FFT_Pwr_Avg_I;
  float* Results_Q = Results_I + 1*FFT_BUF_SIZE;
  float* Results_IQ = Results_I + 2*FFT_BUF_SIZE;
  float* Results_PM = Results_I + 3*FFT_BUF_SIZE;
  float* TotalResults = Results_I;

  PhaseMismatch[0] = 0;

  // Setup for FFT - Creates window Coefficient
  real_WINDOW(win_type, FFT_BUF_SIZE, win_norm_type, 0x0, WinCoef);

  // Zero out FFT Averages for I, Q, I+jQ Channels, and Phase Mismatch
  compute_running_avg((tFloatingPointComplex*) FFT_Pwr_I, (tFloatingPointComplex*) FFT_Pwr_Avg_I, \
                       3*FFT_BUF_SIZE + 1, AVG_RESET);
  //launch_routine_with_args(RTN_COMPUTE_AVG, (UINT32)FFT_Pwr_I, (UINT32)FFT_Pwr_Avg_I, 6*FFT_BUF_SIZE + 2, AVG_RESET, 0, 0);

  // Calculate FFT Average
  for (index1 = 0; index1 < num_avg; index1++) {

    //always use burst mode for FFT
    drp_param5L = BURST_MODE;

    // Execute an RX burst
    rx_timeline_new(band, channel, ABE_gain_step, AFE_gain_step, gain_comp_enable, pole_num, pcb_config);

    // ROC compensation
    if (roc_setting == ENABLE_ROC)
      ref_roc((CMPLX16 *)DRP_SRM_RX_BUFFER_ADDR, 160, 381, (UINT16) 216);

    // Read the DC OFFSET
    drp_srm_dc_offset[index1].i = drp_srm_data->dc_offset_i;
    drp_srm_dc_offset[index1].q = drp_srm_data->dc_offset_q;

  // Read the DRP measured temperature
    drp_measured_temp[index1] = drp_srm_api->inout.temperature.output;

    // Decimate rx_data_buffer (I and Q)
    //   Note: Q and I are reversed coming into RX Buffer
    for (index3 = 0; index3 < FFT_BUF_SIZE; index3++)
    {
       g_rx_data_buffer_decimated[2*index3 + 1] = rx_data_buffer[2*index3] << 1;     // Q
       g_rx_data_buffer_decimated[2*index3]     = rx_data_buffer[2*index3 + 1] << 1; // I
    }

    // Calculates FFT (Real + Imaginary) for I+jQ Channel
    compute_fft((tIQsample *)(&g_rx_data_buffer_decimated[0]), (tFloatingPointComplex*) FFT_IQ, \
                          FFT_BUF_SIZE, 3, (tFloatingPointReal*) WinCoef);

    // Calculates Power and Phase Spectrum for I Channel
    compute_fft_iq_pwr((tFloatingPointComplex*) FFT_IQ, (tFloatingPointComplex*) FFT_Pwr_I, FFT_BUF_SIZE, 1);

    // Calculates Power and Phase Spectrum for Q Channel
    compute_fft_iq_pwr((tFloatingPointComplex*) FFT_IQ, (tFloatingPointComplex*) FFT_Pwr_Q, FFT_BUF_SIZE, 2);

    // Calculates Power and Phase Spectrum for I+jQ Channel
    compute_fft_iq_pwr((tFloatingPointComplex*) FFT_IQ, (tFloatingPointComplex*) FFT_Pwr_IQ, FFT_BUF_SIZE, 3);

    // Get phase mismatch between I and Q channels
  I_TonePhase = FFT_Pwr_I[2*phase_mm_bin + 1] * (180 / PI);
  Q_TonePhase = FFT_Pwr_Q[2*phase_mm_bin + 1] * (180 / PI);

  if (I_TonePhase < 0)
    I_TonePhase = I_TonePhase + 360;

  if (Q_TonePhase < 0)
    Q_TonePhase = Q_TonePhase + 360;

    phase_mm_diff = I_TonePhase - Q_TonePhase;

  if ((phase_mm_diff >= -360) && (phase_mm_diff < -180))
    phase_mm_diff = phase_mm_diff + 360;
  else if ((phase_mm_diff > 180) && (phase_mm_diff <= 360))
    phase_mm_diff = phase_mm_diff - 360;
  else
    phase_mm_diff = phase_mm_diff;

    PhaseMismatch[0] = phase_mm_diff;

    // Average Power and Phase for I, Q, I+jQ Channels, and Phase Mismatch
    // For I, Q, IQ and PhaseMismatch[0] - hence length = 6*FFT_BUF_SIZE + 2
    compute_running_avg((tFloatingPointComplex*)FFT_Pwr_I, (tFloatingPointComplex*)FFT_Pwr_Avg_I, \
                         3*FFT_BUF_SIZE + 1, AVG_RUN);
  }

  // Copy power spectrum (only magnitude) results to results buffers
  for (index2 = 0; index2 < FFT_BUF_SIZE; index2++) Results_I[index2] = FFT_Pwr_Avg_I[2*index2];
  for (index2 = 0; index2 < FFT_BUF_SIZE; index2++) Results_Q[index2] = FFT_Pwr_Avg_Q[2*index2];
  for (index2 = 0; index2 < FFT_BUF_SIZE; index2++) Results_IQ[index2] = FFT_Pwr_Avg_IQ[2*index2];

  // Move Phase Mismatch into Results buffer
  Results_PM[0] = PhaseMismatch_Avg[0];

  return (UINT32)(TotalResults);
}

#endif //#if (DRP_TEST_SW && DRP_BENCH_SW)

#if DRP_TDL_SW_DEBUG
void fft_avg_debug(float *results)
{
  SINT16 i;
  float *drp_mem_fft_debug = (float *) 0xFFFF2300;

  //copy I magnitude
  for (i = 0; i < FFT_BUF_SIZE; i++) drp_mem_fft_debug[i] = results[i];

  //copy Q magnitude
  for (i = 0; i < FFT_BUF_SIZE; i++) drp_mem_fft_debug[FFT_BUF_SIZE + i] = results[FFT_BUF_SIZE + i];

  //copy IQ magnitude
  for (i = 0; i < FFT_BUF_SIZE; i++) drp_mem_fft_debug[2*FFT_BUF_SIZE + i] = results[2*FFT_BUF_SIZE + i];

  //copy phase mismatch
  drp_mem_fft_debug[3*FFT_BUF_SIZE] = results[3*FFT_BUF_SIZE];

}
#endif //#if DRP_TDL_SW_DEBUG

#if DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW)
void process_dc_offset(UINT16 num_bursts, UINT16 num_shift)
{
  SINT32 sum_mean_i = 0, sum_mean_q = 0;
  SINT32 sum_mean_sq_i = 0, sum_mean_sq_q = 0;
  SINT16 tmp_i, tmp_q, tmp_i1, tmp_i2, median_i, median_q, tmp_q1, tmp_q2;
  SINT16 indx, indx1, max_i, min_i, max_q, min_q;

  SINT32 *drp_ptr = (SINT32 *) 0xFFFF2300;

  max_i = -32768;
  max_q = -32768;
  min_i = 32767;
  min_q = 32767;

  //mean and variance
  for (indx = 0; indx < num_bursts; indx++)
  {
    tmp_i = drp_srm_dc_offset[indx].i;
    tmp_q = drp_srm_dc_offset[indx].q;
    sum_mean_i += tmp_i;
    sum_mean_q += tmp_q;

    sum_mean_sq_i += (((UINT32) (tmp_i * tmp_i)) >> num_shift);
    sum_mean_sq_q += (((UINT32) (tmp_q * tmp_q)) >> num_shift);

 if (tmp_i < min_i) min_i = tmp_i;
    if (tmp_i > max_i) max_i = tmp_i;
 if (tmp_q < min_q) min_q = tmp_q;
    if (tmp_q > max_q) max_q = tmp_q;
  }

  //median
  for (indx = 0; indx < num_bursts - 1; indx++)
  {
    tmp_i1 = drp_srm_dc_offset[indx].i;
    tmp_q1 = drp_srm_dc_offset[indx].q;

    for (indx1 = indx + 1; indx1 < num_bursts; indx1++)
    {
      tmp_i2 = drp_srm_dc_offset[indx1].i;
      tmp_q2 = drp_srm_dc_offset[indx1].q;

      if (tmp_i2 < tmp_i1)
      {
        drp_srm_dc_offset[indx].i = tmp_i2;
        drp_srm_dc_offset[indx1].i = tmp_i1;
  tmp_i1 = tmp_i2;
      }

      if (tmp_q2 < tmp_q1)
      {
        drp_srm_dc_offset[indx].q = tmp_q2;
        drp_srm_dc_offset[indx1].q = tmp_q1;
  tmp_q1 = tmp_q2;
      }
    }
  }

  if ((num_bursts & 1) == 1)
  {
    median_i = drp_srm_dc_offset[num_bursts/2 - 1].i;
    median_q = drp_srm_dc_offset[num_bursts/2 - 1].q;
  }
  else
  {
    median_i = (drp_srm_dc_offset[num_bursts/2 - 1].i + drp_srm_dc_offset[num_bursts/2].i) >> 1;
    median_q = (drp_srm_dc_offset[num_bursts/2 - 1].q + drp_srm_dc_offset[num_bursts/2].q) >> 1;
  }

  drp_ptr[0] = (SINT32) sum_mean_i;
  drp_ptr[1] = (SINT32) sum_mean_sq_i;
  drp_ptr[2] = (SINT32) median_i;
  drp_ptr[3] = (SINT32) max_i;
  drp_ptr[4] = (SINT32) min_i;
  drp_ptr[5] = (SINT32) sum_mean_q;
  drp_ptr[6] = (SINT32) sum_mean_sq_q;
  drp_ptr[7] = (SINT32) median_q;
  drp_ptr[8] = (SINT32) max_q;
  drp_ptr[9] = (SINT32) min_q;

  return;

}
#endif //#if DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW)

#if (DRP_TEST_SW && !DRP_BURNIN)
/**************************************************************************************/
/* This is a temporary wait function to be used for the                               */
/* test SW, should be replaced by the wait_ARM_cycles                                 */
/**************************************************************************************/
void arm_wait_us(UINT32 time_us){
  volatile UINT16 temp16;
  UINT32 indx;

  temp16 = 0;
  for(indx=0;indx < time_us;indx++){
    // 105 noop = 1 us at 105 MHz
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");

    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");

    //increased wait when compiled with -o2 option
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
  }

  return;

}

#endif //#if (DRP_TEST_SW && !DRP_BURNIN)

#if (DRP_TEST_SW && DRP_BURNIN)
/* FIXME : Confirm this for BOARD == 70 and BOARD == 71 */
/*-------------------------------------------------------*/
/* convert_nanosec_to_cycles()                           */
/*-------------------------------------------------------*/
/* parameter: time in 10E-9 seconds                      */
/* return: Number of cycles for the wait_ARM_cycles()    */
/*         function                                      */
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* convert x nanoseconds in y cycles used by the ASM loop*/
/* function . Before calling this function, call the     */
/* initialize_wait_loop() function                       */
/* Called when the HardWare needs time to wait           */
/*-------------------------------------------------------*/
/* NEW COMPILER MANAGEMENT
 * Removal of inline on convert_nanosec_to_cycles.
 * With new compiler, inline means static inline involving the
 * function to not be seen outside this file.
 */
SYS_UWORD32 convert_nanosec_to_cycles(SYS_UWORD32 time)
{
  return( time / ratio_wait_loop);
}


/*-------------------------------------------------------*/
/* initialize_wait_loop()                                */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* Init the ratio used to convert time->Cycles according */
/* to hardware parameters                                */
/* measurement time for this function (ARM 39Mhz, 3 waits*/
/* states) = 75 micoseconds                              */
/*-------------------------------------------------------*/

void initialize_wait_loop(void)
{
  #define NBR_CYCLES_IN_LOOP   5   // this value is got from an oscilloscope measurement

  double src_ratio;
  double final_ratio;

  SYS_UWORD16 flash_access_size;
  SYS_UWORD16 flash_wait_state;
  SYS_UWORD32 nbr;
  SYS_UWORD32 arm_clock;

  //////////////////////////////////
  //  compute the ARM clock used  //
  //////////////////////////////////
  {
    SYS_UWORD16 arm_mclk_xp5;
    SYS_UWORD16 arm_ratio;
    SYS_UWORD16 clk_src;
    SYS_UWORD16 clkm_cntl_arm_clk_reg = * (volatile SYS_UWORD16 *) CLKM_CNTL_ARM_CLK;

  #if 1 //#if ((CHIPSET == 4) || (CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 11) || (CHIPSET == 12) || ( CHIPSET == 15))
      clk_src  = (clkm_cntl_arm_clk_reg & MASK_CLKIN) >> 1;
      switch (clk_src)
      {
        case 0x00: //DPLL selected
          // select the DPLL factor
           #if 1 //#if ((CHIPSET == 12) || (CHIPSET == 15))
              if (((* (volatile SYS_UWORD16 *) C_MAP_DPLL_BASE) & DPLL_LOCK) != 0)
           #else
              if (((* (volatile SYS_UWORD16 *) MEM_DPLL_ADDR) & DPLL_LOCK) != 0)
#endif
          {
             SYS_UWORD16 dpll_div;
             SYS_UWORD16 dpll_mul;

             dpll_div=DPLL_READ_DPLL_DIV;
             dpll_mul=DPLL_READ_DPLL_MUL;
             src_ratio = (double)(dpll_mul)/(double)(dpll_div+1);
          }
          else // DPLL in bypass mode
          {
             SYS_UWORD16 dpll_div = DPLL_BYPASS_DIV;
             src_ratio= (double)(1)/(double)(dpll_div+1);
          }
          break;
        case 0x01: //VTCX0 selected
          src_ratio = 1;
          break;
        case 0x03: //CLKIN selected   (external clock)
          src_ratio = 1;
          break;
  }

  #if 0 //#if(CHIPSET != 15)
      // define the division factor applied to clock source (CLKIN or VTCXO or DPLL)
      arm_ratio = (clkm_cntl_arm_clk_reg & CLKM_MCLK_DIV) >> 4;

      // check if the 1.5 or 2.5 division factor is enabled
      arm_mclk_xp5  = clkm_cntl_arm_clk_reg & CLKM_ARM_MCLK_XP5;

      if (arm_mclk_xp5 == 0) // division factor enable for ARM clock ?
      {
        if (arm_ratio == 0)
          arm_ratio =1;
      }
      else
        arm_ratio = ((arm_ratio>>1) & 0x0001) == 0 ? 1.5 : 2.5;
  #else
  if(( clkm_cntl_arm_clk_reg& CLKM_SWITCH_FREQ))
    arm_ratio = 2;
  else
    arm_ratio = 1;
  #endif
 #else
      src_ratio = 1;

      // define the division factor applied to clock source (CLKIN or VTCXO or DPLL)
      arm_ratio = (clkm_cntl_arm_clk_reg & CLKM_MCLK_DIV) >> 4;

      // check if the 1.5 or 2.5 division factor is enabled
      arm_mclk_xp5  = clkm_cntl_arm_clk_reg & MASK_ARM_MCLK_1P5;

      if (arm_mclk_xp5 == 1) // division factor enable for ARM clock ?
        arm_ratio = 1.5;
      else
      {
        if (arm_ratio == 0)
          arm_ratio = 4;
        else
          if (arm_ratio == 1 )
            arm_ratio = 2;
          else
            arm_ratio = 1;
      }

     #endif

   final_ratio = (src_ratio / (double) arm_ratio);

  }
  //////////////////////////////////////////
  //  compute the Flash wait states used  //
  //////////////////////////////////////////

  #if 1//#if (CHIPSET == 12 || CHIPSET == 15)
     flash_access_size  =  1;
  #else
    flash_access_size  =  *((volatile SYS_UWORD16 *) MEM_REG_nCS0);
  #endif
  flash_access_size  = (flash_access_size >> 5) & 0x0003; // 0=>8bits, 1=>16 bits, 2 =>32 bits

  // the loop file is compiled in 16 bits it means
  //    flash 8  bits => 2 loads for 1 16 bits assembler instruction
  //    flash 16 bits => 1 loads for 1 16 bits assembler instruction
  //    flash/internal RAM 32 bits => 1 loads for 1 16 bits assembler instruction (ARM bus 16 bits !!)

  // !!!!!!!!! be careful: if this file is compile in 32 bits, change these 2 lines here after !!!
  if (flash_access_size == 0) flash_access_size = 2;
  else                        flash_access_size = 1;

  #if 1 //#if (CHIPSET == 12 || CHIPSET == 15)
    /*
     *  loop move to run in internal memory, due to page mode in external memory
     */
    flash_wait_state  =  0;
  #else
    flash_wait_state  =  *((volatile SYS_UWORD16 *) MEM_REG_nCS0);
    flash_wait_state &=  0x001F;
  #endif

  //////////////////////////////////////
  //  compute the length of the loop  //
  //////////////////////////////////////

  // Number of flash cycles for the assembler loop
  nbr = NBR_CYCLES_IN_LOOP;

  // Number of ARM cycles for the assembler loop
  nbr = nbr * (flash_wait_state + 1) * (flash_access_size);

  // time for the assembler loop (unit nanoseconds: 10E-9)
#if DRP_BURNIN_SIMULATE
  arm_clock = final_ratio * 13; // ARM clock in Mhz
#else
  arm_clock = 5;               // ARM clock in Mhz
#endif
  ratio_wait_loop = (SYS_UWORD32)((nbr*1000) / arm_clock);
}

#if 0
/*-------------------------------------------------------*/
/* wait_ARM_cycles()                                     */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* Called when the HardWare needs time to wait.          */
/* this function wait x cycles and is used with the      */
/* convert_nanosec_to_cycles() & initialize_wait_loop()  */
/*                                                       */
/*  Exemple:  wait 10 micro seconds:                     */
/*  initialize_wait_loop();                              */
/*  wait_ARM_cycles(convert_nanosec_to_cycles(10000))    */
/*                                                       */
/*  minimum time value with cpt_loop = 0  (estimated)    */
/*  and C-SAMPLE / flash 6,5Mhz  ~  1,5 micro seconds    */
/*                                                       */
/*                                                       */
/* Be careful : in order to respect the rule about the   */
/* conversion "time => number of cylcles in this loop"   */
/* (Cf the functions: convert_nanosec_to_cycles() and    */
/* initialize_wait_loop() ) respect the following rules: */
/* This function must be placed in Flash Memory and      */
/* compiled in 16 bits instructions length               */
/*-------------------------------------------------------*/
void wait_ARM_cycles(SYS_UWORD32 cpt_loop)
{
  // C code:
  // while (cpt_loop -- != 0);

  asm(" CMP       A1, #0");
  asm(" BEQ       END_FUNCTION");

  asm("LOOP_LINE:        ");
  asm(" SUB       A1, A1, #1");
  asm(" CMP       A1, #0");
  asm(" BNE       LOOP_LINE");

  asm("END_FUNCTION:        ");
}
#endif //#if 0
#endif


#if (DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW))

void fft_post_evaluation(UINT16 bin_interest, UINT16 compute_noise,
                         UINT16 bin_start, UINT16 bin_stop,
                         UINT16 num_bins, UINT16 fixed_float_flag)
{

  UINT16 index0 = 0;
  UINT16 index1 = 0;
  UINT16 index2 = 0;
  UINT16 index3 = 0;
  UINT16 index4 = 0;
  UINT16 index5 = 0;
  volatile UINT16 *bin_src;

  float bin_mag_complex_fft = 0;
  float bin_mag_i_fft = 0;
  float bin_mag_q_fft = 0;
  float fft_complex_bins = 0;
  float fft_i_bins = 0;
  float fft_q_bins = 0;
  //float diff_num_bins = 0;
  float fft_bins_interpol = 0;
  float results_fft_post[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  //float* WinExtraPara = g_buffer + 0*FFT_BUF_SIZE;
  //float* wincoef = g_buffer + 1*FFT_BUF_SIZE;
  //float* fft_iq = g_buffer + 2*FFT_BUF_SIZE;
  //float* fft_pwr_i = g_buffer + 4*FFT_BUF_SIZE;
  //float* fft_pwr_q = g_buffer + 6*FFT_BUF_SIZE;
  //float* fft_pwr_iq = g_buffer + 8*FFT_BUF_SIZE;
  //float* phasemismatch = g_buffer + 10*FFT_BUF_SIZE;

  float *results_i, *results_q, *results_iq, *results_pm, *drp_mem_results;
  if (fixed_float_flag == FFT_TYPE_FLOATINGPOINT)
  {
    results_i = g_Buffer + 10 * FFT_BUF_SIZE + 2;
  }
  else
  {
    results_i = &FFT_Pwr_Avg_I_ate[0];
  }

  results_q = results_i + 1 * FFT_BUF_SIZE;
  results_iq = results_i + 2 * FFT_BUF_SIZE;
  results_pm = results_i + 3 * FFT_BUF_SIZE;

  // Get magnitud of Complex, I and Q channel bin
  bin_mag_complex_fft = results_iq[bin_interest];
  bin_mag_i_fft = results_i[bin_interest];
  bin_mag_q_fft = results_q[bin_interest];

  results_fft_post[0] = sqrt(bin_mag_complex_fft);
  results_fft_post[1] = sqrt(2 * bin_mag_i_fft);
  results_fft_post[2] = sqrt(2 * bin_mag_q_fft);

  // Get phase mismatch between I and Q channels
  results_fft_post[3] = results_pm[0];

  if (compute_noise == 1 && num_bins > 0 && bin_stop >= bin_start) {

    //diff_num_bins = bin_stop - bin_start + 1;
    bin_src = &drp_param1L;

    // Get Integration noise calculated with Complex FFT data on all
    // bins between PARAM3 and PARAM4 except PARAM1 & PARAM6tox bins.
    // Those values have been interpolated lineary.
    for (index0 = bin_start; index0 < (FFT_BUF_SIZE - bin_start); index0++) {

      if (index0 == bin_stop) {
        index0 = FFT_BUF_SIZE / 2 + (FFT_BUF_SIZE / 2 - bin_stop);
      }
      if (index0 == (UINT16) *bin_src && index3 < num_bins) {
        if (index0 == 0) {
          fft_bins_interpol = (results_iq[index0] + results_iq[index0 + 1]) / 2;
          fft_complex_bins = fft_complex_bins + fft_bins_interpol;
        }
        if (index0 == FFT_BUF_SIZE - 1) {
          fft_bins_interpol = (results_iq[index0-1] + results_iq[index0]) / 2;
          fft_complex_bins = fft_complex_bins + fft_bins_interpol;
        }
        else {
          fft_bins_interpol = (results_iq[index0 - 1] + results_iq[index0 + 1]) / 2;
          fft_complex_bins = fft_complex_bins + fft_bins_interpol;
        }
        bin_src = &drp_param6L + 2 * index3;
        index3++;
      } else if (index0 != (UINT16) *bin_src) {
          fft_complex_bins = fft_complex_bins + results_iq[index0];
      }
    }
    results_fft_post[4] = sqrt(fft_complex_bins);

    // Reuse FFT_BIN_INTERPOL and INDEX1 to reduce the .const size on declaring
    // new variables.
    fft_bins_interpol = 0;
    bin_src = &drp_param1L;

    // Get Integration noise calculated with I_FFT data on all
    // bins between PARAM3 and PARAM4 (up to FFT_BUF_SIZE size) except
    //PARAM1 & PARAM6tox bins. Those values have been interpolated lineary.
    for (index1 = bin_start; index1 < (bin_stop + 1); index1++) {
      if ((UINT16) *bin_src > FFT_BUF_SIZE/2)
        index4 = FFT_BUF_SIZE - *bin_src;
      else
        index4 = *bin_src;

      if (index1 == index4 && index3 < num_bins) {
        if (index1 == 0) {
          fft_bins_interpol = (results_i[index1] + results_i[index1 + 1]) / 2;
          fft_i_bins = fft_i_bins + fft_bins_interpol;
        } else {
          fft_bins_interpol = (results_i[index1 - 1] + results_i[index1 + 1]) / 2;
          fft_i_bins = fft_i_bins + fft_bins_interpol;
        }
        bin_src = &drp_param6L + 2 * index3;
        index3++;
      } else if (index1 != index4) {
          fft_i_bins = fft_i_bins + results_i[index1];
      }
    }
    results_fft_post[5] = sqrt(2 * fft_i_bins);

    // Reuse FFT_BIN_INTERPOL and INDEX1 to reduce the .const size on declaring
    // new variables.
    fft_bins_interpol = 0;
    index4 = 0;
    bin_src = &drp_param1L;

    // Get Integration noise calculated with Q_FFT data on all
    // bins between PARAM3 and PARAM4 (up to FFT_BUF_SIZE size) except
    //PARAM1 & PARAM6tox bins. Those values have been interpolated lineary.
    for (index2 = bin_start; index2 < (bin_stop + 1); index2++) {

      if ((UINT16) *bin_src > FFT_BUF_SIZE/2)
        index4 = FFT_BUF_SIZE - *bin_src;
      else
        index4 = *bin_src;

      if (index2 == index4 && index3 < num_bins) {
       if (index2 == 0) {
          fft_bins_interpol = (results_q[index2] + results_q[index2 + 1]) / 2;
          fft_q_bins = fft_q_bins + fft_bins_interpol;
        } else {
          fft_bins_interpol = (results_q[index2 - 1] + results_q[index2 + 1]) / 2;
          fft_q_bins = fft_q_bins + fft_bins_interpol;
        }
        bin_src = &drp_param6L + 2 * index3;
        index3++;
      } else if (index2 != index4) {
          fft_q_bins = fft_q_bins + results_q[index2];
      }
    }
    results_fft_post[6] = sqrt(2 * fft_q_bins);
  }

  drp_mem_results = (float *) &drp_param1L;
  for (index5 = 0; index5 < 7; index5++) {
    drp_mem_results[index5] = results_fft_post[index5];
  }
}

#endif //#if (DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW))

#if DRP_FLYBACK_CALIB

/*Fly Back Delay compensation sw*/
#if (DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW)) || (!DRP_TEST_SW)

double myldexp(double x, int exp)
{
     int *ptr = (int *) &x;
     int texp = exp + ((*ptr >> 20) & 0x7FF);

     /***********************************************************************/
     /* IF RESULT UNDERFLOWS, RETURN 0.0.  IF RESULT OVERFLOWS, RETURN      */
     /* +- INFINITY.                                                        */
     /***********************************************************************/
     if (texp < 1) return 0.0;
     //if (texp > 0x7FF) { errno = ERANGE; return (x < 0) ? -HUGE_VAL:HUGE_VAL;}

     *ptr = (*ptr & 0x800FFFFF) | ((texp << 20) & (0x7FF << 20));
     return x;
}

double mysqrt(double x)
{
    double x0;          /* estimate */
    int exp;

    /************************************************************************/
    /* Check to see if the input is not in the function's domain.           */
    /************************************************************************/
    if (x <= 0.0)
    {
      //if (x < 0.0) errno = EDOM;
      return (0.0);
    }

    /************************************************************************/
    /* initial estimate = .75 * 2 ^ -(exp/2)                                */
    /************************************************************************/
    exp = ( (*((unsigned long *) &x) >> 20) & 0x7FF) - 1023;
    x0  = myldexp(0.75, -exp / 2);

    /************************************************************************/
    /* Refine estimate                                                      */
    /************************************************************************/
    x0 *= (1.5 - x * 0.5 * x0 * x0);
    x0 *= (1.5 - x * 0.5 * x0 * x0);
    x0 *= (1.5 - x * 0.5 * x0 * x0);
    x0 *= (1.5 - x * 0.5 * x0 * x0);
    x0 *= (1.5 - x * 0.5 * x0 * x0);
    x0 *= (1.5 - x * 0.5 * x0 * x0);

    /************************************************************************/
    /* sqrt(x) = x * sqrt(1/x)                                              */
    /************************************************************************/
    return (x0 * x);
}


void flybackdelayCalibration(T_DRP_FLYBACK_DELAY_PARAM *flybackdelay_param )
{
  UINT16 i;
 // UINT16 rmsPhe[7];
  UINT16 ibias0,ibias1,ibias;
  //UINT16 maxPE, maxPeIndex;
  UINT16 rmsPhefstop[2], rmsPhefstart[2];
  float mfcnminus1,mfcnplus1,mfstart,mfstop;
  float s1, s2;
  UINT16 fcutoff1, fcutoff2;
  UINT32 *paramPtr= (UINT32 *) 0xFFFF2300;
  UINT16 dlo_tdcsh_save,temp16;
  float max_phe_index, m2, m1;
  UINT16 max_phe_index_Converted;
  UINT32 perinvCal;
  UINT32 perinvCalRatio;
  UINT8  band;
  UINT32 rmsPhe[8];
  UINT32 maxPE, maxPeIndex, temp32;

  temp32 = drp_copy_ref_sw_to_drpsrm((unsigned char *)&drp_phe_anayser_dft[0]);

  READ_WORD(drp_srm_data->tables.tx_ibias[0], ibias0);
  READ_WORD(drp_srm_data->tables.tx_ibias[1], ibias1);

  if(flybackdelay_param->rf_freqCentre >= 8242 && flybackdelay_param->rf_freqCentre <=8488)
  {
    ibias = ibias0 & 0xff;
    band = 0;
  }
  else if(flybackdelay_param->rf_freqCentre >= 8802 && flybackdelay_param->rf_freqCentre <=9148)
  {
    ibias = ibias0 >> 8 & 0xff;
    band = 1;
  }
  else if(flybackdelay_param->rf_freqCentre >= 17102 && flybackdelay_param->rf_freqCentre <= 17848 )
  {
    ibias = ibias1 & 0xff;
    band = 2;
  }
  else if(flybackdelay_param->rf_freqCentre >= 18502 && flybackdelay_param->rf_freqCentre <=  19098 )
  {
    ibias = ibias1 >> 8 & 0xff;
    band = 3;
  }

  if ((band == GSM_BAND) || (band == EGSM_BAND))
  {
    return;
  }


  /*first calculate the rms phe for 6 flyback delays for centre freq*/
  for(i=0; i<8; i++)
  {
    phe_measurement( flybackdelay_param->rf_freqCentre,
                     flybackdelay_param->loop_count,
                     flybackdelay_param->itter_cnt,
                     flybackdelay_param->alpha,
                     flybackdelay_param->rho,
                     flybackdelay_param->lambda,
                     ibias,
                     flybackdelay_param->ModulationSwtich,
                     i,
                     flybackdelay_param->ckvdsetting,
                     flybackdelay_param->sigmadeltaorder
                   );

    rmsPhe[i]=(*((UINT16*)0xFFFF2068));
  }
  /*find out the flyback value for which the max phe is got*/
  maxPE=rmsPhe[0];
  maxPeIndex=0;
  for(i=0; i<8; i++)
  {
    if(rmsPhe[i] > maxPE)
    {
      maxPE = rmsPhe[i];
      maxPeIndex = i;
    }
  }

  READ_WORD(drp_regs->DLO_PERINVL,perinvCal);

  perinvCalRatio = (perinvCal << 16)/flybackdelay_param->rf_freqCentre; // 32 bit value

  //Now the equation for the find the max phe is implemented here
  if(!(maxPeIndex == 0 || maxPeIndex == 7))
  {
    m2 = maxPE - rmsPhe[maxPeIndex -1];
    m1 = maxPE - rmsPhe[maxPeIndex +1];
    /*this number needs to be stored as 8bit inter and 8 bit fraction*/
    /* so multiply this number by 2^8 to get this number in integer form*/
    max_phe_index =  (maxPeIndex + 0.5 *((m2-m1)/(m2+m1)))*256;
  }
  else
  {
    m2 =0;
    m1= 0;
    max_phe_index =  maxPeIndex << 8;//make this 8bit integer 8bit fractional format
  }

  max_phe_index_Converted = max_phe_index;
  /*now copy the results back to drp memory 0x2300*/

#if (DRP_TEST_SW)
    for(i=0;i<8;i++)
    {
     *paramPtr++ = rmsPhe[i];
    }
    *paramPtr++  = maxPE;
    *paramPtr++  =maxPeIndex ;
    /*conversion from float to integer and this is in the required form*/
    *paramPtr++  = max_phe_index_Converted;
    *paramPtr++  = m1;
    *paramPtr++  = m2;
#endif

    //write the xmax to the srm data structure
    WRITE_WORD(drp_srm_data->flyback_xmax_table[band - DCS_BAND], max_phe_index_Converted);
    WRITE_WORD(drp_sw_data->calib.flyback.xmax_table[band - DCS_BAND], max_phe_index_Converted);
    //write the perinv cal to srm data structure
    WRITE_WORD(drp_srm_data->flyback_perinvcal_table[band - DCS_BAND], perinvCal);
    WRITE_WORD(drp_sw_data->calib.flyback.perinvcal_table[band - DCS_BAND], perinvCal);
    //write the cf to srm data structure
    WRITE_WORD(drp_srm_data->flyback_cf_table[band - DCS_BAND], flybackdelay_param->cf);
    WRITE_WORD(drp_sw_data->calib.flyback.cf_table[band - DCS_BAND], flybackdelay_param->cf);
    //write the ct to srm data structure
    WRITE_WORD(drp_srm_data->flyback_ct_table[band - DCS_BAND], flybackdelay_param->ct);
    WRITE_WORD(drp_sw_data->calib.flyback.ct_table[band - DCS_BAND], flybackdelay_param->ct);
    //write the ct to srm data structure
    WRITE_WORD(drp_srm_data->flyback_fcal_table[band - DCS_BAND], flybackdelay_param->rf_freqCentre);
    WRITE_WORD(drp_sw_data->calib.flyback.fcal_table[band - DCS_BAND], flybackdelay_param->rf_freqCentre);
}

/******************************************************************************
* FUNCTION NAME: phe_measurement
*
* DESCRIPTION:
*       This is the main function which will be call other functions.This function
*       Will setup the calc/RX buffers  and perform the PHE test by calling other functions
*       This is the PHE Measurement function.
*
*
* Return Value:      NONE
*
* Input Parameters:  UINT16 rf_freq, UINT16 loop_count, UINT16 itter_cnt, UINT16 alpha
*                    UINT16 rho, UINT16 lambda,UINT16 Ibias
* Output Parameters: None
* Functions Called:  dbb_set_phe_samples_setting
*                    get_phe_stats
*                    dbb_restore_phe_samples_setting
*                    sqrt
*
*
******************************************************************************/
void phe_measurement(UINT16 rf_freq,
                     UINT16 loop_count,
                     UINT16 itter_cnt,
                     UINT16 alpha,
                     UINT16 rho,
                     UINT16 lambda,
                     UINT16 Ibias,
           UINT16 ModulationSwtich,
           UINT16 flybacksetting,
           UINT16 ckvdsetting,
           UINT16 sigmadeltaorder
                     )
{
  UINT32 loop_dlo_perinv_sum ;
  UINT32 loop_phe_sq_mean_sum ;
  SINT32 loop_phe_mean ;
  SINT32 loop_phe_sum ;
  UINT32 loop_phe_sq;
  UINT32 phe_rms_max;
  UINT32 temp32, calc_ptr_start_end_setting;
  UINT16 dtx_sel_contl_save = 0 ;
  UINT16 temp16 = 0 ;
  UINT16 loop_index ;
  SINT16 loop_phe_max;
  SINT16 loop_phe_min;
  SINT16 phe_max;
  SINT16 phe_min;
  UINT16 dlo_perinv;
  UINT16 phe_rms;
  UINT16 phe_overall_max;
  volatile UINT16 indx2;
  volatile UINT16 indxavg,indx;
  volatile UINT16 data_prev;
  volatile UINT16 burst_shift;

  // Setup for each burst
  WRITE_WORD(drp_regs->RF_FREQL, rf_freq);

  // setup the clocks for the phe capture
  dbb_set_phe_samples_setting();

  // TX_START : Save off original settings and setup for register control
  READ_WORD(drp_regs->DTX_SEL_CONTL, dtx_sel_contl_save);
  // Memory Control of TX_START, User word to transmit 32bits
  temp16 = dtx_sel_contl_save | 0x3006 ;
  WRITE_WORD(drp_regs->DTX_SEL_CONTL,temp16);

  // Set TX LSFR to good initial value 0xFFFFFFFF
  //WRITE_WORD(drp_regs->DTX_USER_WORDL,0xFFFF);
  //WRITE_WORD(drp_regs->DTX_USER_WORDH,0xFFFF);

   /*program user word reg with to transmit aaaa ie.,bit patter 1010101010101...*/
   WRITE_WORD(drp_regs->DTX_USER_WORDL,0xAAAA);
   WRITE_WORD(drp_regs->DTX_USER_WORDH,0xAAAA);

  /* Setup the calc buffer -- [sunil]*/
  /* Setup for 32 words starting from the
   * base of calc buffer */
  READ_WORD(drp_regs->CALC_PTR_START_END_ADDRH, temp16);
  calc_ptr_start_end_setting = temp16 << 16; /*save the previous value*/
  READ_WORD(drp_regs->CALC_PTR_START_END_ADDRL, temp16);
  calc_ptr_start_end_setting |= temp16;      /*save the previous value*/
  WRITE_WORD(drp_regs->CALC_PTR_START_END_ADDRL,CALC_BUFFER_END_ADDRESS); /*end address0x2400+32words*/
  WRITE_WORD(drp_regs->CALC_PTR_START_END_ADDRH,CALC_BUFFER_START_ADDRESS); /*start address 0x2400*/

  // Initialized loop values
  loop_dlo_perinv_sum = 0 ;
  loop_phe_sq_mean_sum = 0 ;
  phe_max = -32767 ;
  phe_min =  32767 ;
  phe_rms_max = 0 ;

  for(loop_index = 0; loop_index < loop_count; loop_index++)
  {
    // setup the clocks for the phe capture
    // IDLE modifies the clock params so we need to it everytime
    dbb_set_phe_samples_setting();


    get_phe_stats(itter_cnt, &temp16, &loop_phe_sum, &loop_phe_sq, &loop_phe_max, &loop_phe_min,
                    alpha, rho, lambda, Ibias, ModulationSwtich,flybacksetting, ckvdsetting, sigmadeltaorder);

    loop_dlo_perinv_sum += temp16 ;
    // Calculate Mean and Magnitude of PHE
    // Note: Inner loop already divides by # of samples sum'd in innerloop ;
    // PHE Mean = sum(phe)/#itterations
    loop_phe_mean = loop_phe_sum / (SINT32)itter_cnt  ;

    // PHE Magnitude = (sum(phe**2)/#itterations) - (PHE_Mean**2)
    temp32 = loop_phe_sq / (UINT32)itter_cnt ;
    temp32 = temp32 - (UINT32)(loop_phe_mean * loop_phe_mean);

    temp32= (UINT32) (mysqrt(temp32)* 125 * 45) >> 10 ;
    loop_phe_sq_mean_sum += temp32 ;

    // Save peak RMS value
    if(temp32 > phe_rms_max)
    {
      phe_rms_max = temp32 ;
    }

    // Remove DC from Peak values
    loop_phe_max = loop_phe_max - loop_phe_mean ;
    loop_phe_min = loop_phe_min - loop_phe_mean ;

    if(loop_phe_max > phe_max)
    {
      phe_max = loop_phe_max ;
    }
    if(loop_phe_min < phe_min)
    {
      phe_min = loop_phe_min ;
    }
  }

  // Restore Settings for TX_Start
  WRITE_WORD(drp_regs->DTX_SEL_CONTL, dtx_sel_contl_save);

  // Restore device settings
  // Settings for phe capture by Calc buffer
  dbb_restore_phe_samples_setting() ;

/*
 * Store the results in the param buffer only
 * drp_param1L = Phe RMS value
 * drp_param2L = Phe over all max
 * drp_param3L = DLO period inv
 * drp_param4L = phe rms max value
 *
 */

  // Convert to RMS and write to DRP Parameter table
  temp32  = loop_phe_sq_mean_sum / loop_count ;
  //temp32  = (UINT32) (sqrt(temp32)* 125 * 45) >> 10 ;
  phe_rms = (UINT16) (temp32 & 0xFFFF) ;

  drp_param1L = phe_rms;

  // Calculate overall absolute maximum peak
  phe_min = 0 - phe_min ;
  if(phe_min > phe_max)
  {
    phe_overall_max = (UINT16) phe_min ;
  }
  else
  {
    phe_overall_max = (UINT16) phe_max ;
  }


  // Calculate DLO Period Inverse
  dlo_perinv = (UINT16) (loop_dlo_perinv_sum / loop_count) ;
  /*Needs to be removed when these variable will be used by making next #if to 1*/
  temp16 =dlo_perinv;//just to make compiler happy;
  temp16= phe_overall_max;//just to make compiler happy;

#if 0 /* for now we are not outputting these params*/
  drp_param2 = phe_overall_max;
  drp_param3 = dlo_perinv;

  //phe_rms_max = (UINT32) (sqrt(phe_rms_max)* 125 * 45) >> 10 ;
  temp16 = (UINT16) (phe_rms_max & 0xFFFF) ;
  drp_param4 = temp16;
#endif
  /* restore the calc to original address*/
  WRITE_WORD(drp_regs->CALC_PTR_START_END_ADDRL, (calc_ptr_start_end_setting & 0xFFFF));
  WRITE_WORD(drp_regs->CALC_PTR_START_END_ADDRH, ((calc_ptr_start_end_setting >> 16) & 0xFFFF));
}

/******************************************************************************
* FUNCTION NAME: get_phe_stats
*
* DESCRIPTION:
*       This is the actual PHE data collection Function this fucntion will
*       Collect the PHE samples and reports the avg value of them.
*
*
* Return Value:      NONE
*
* Input Parameters:  UINT16 itter_cnt, UINT16 *dlo_perinv, SINT32 *phe_sum, UINT32 *phe_square
*                    SINT16 *phe_max, SINT16 *phe_min, UINT16 rho, UINT16 lambda,UINT16 Ibias
* Output Parameters: None
* Functions Called:  TX_ON SCRIPT
*                    TX_START SCRIPT
*                    PHE_ANALYSIS SCRIPT
*                    IDLE
*
*
******************************************************************************/
void get_phe_stats(UINT16 itter_cnt,
                    UINT16 *dlo_perinv,
                    SINT32 *phe_sum,
                    UINT32 *phe_square,
                    SINT16 *phe_max,
                    SINT16 *phe_min,
                    UINT16 alpha,
                    UINT16 rho,
                    UINT16 lambda,
                    UINT16 Ibias,
                    UINT16 ModulationSwtich,
                    UINT16 flybacksetting,
                    UINT16 ckvdsetting,
                    UINT16 sigmadeltaorder
                  )
{
  UINT16 temp16 = 0;
  UINT32 temp32;
  UINT16 def_dlo_alphal, def_dlo_alphah, def_dlo_iirl;
  UINT16 ibias0, ibias1;
  UINT16 ana_usdL, dlo_dcoifh,dlo_dcoifl;
#if DRP_BENCH_SW
  UINT16 dlo_tdcsh_save;
#endif //#if DRP_BENCH_SW


  READ_WORD(drp_srm_data->tables.tx_ibias[0], ibias0);
  READ_WORD(drp_srm_data->tables.tx_ibias[1], ibias1);

  Ibias = Ibias * 0x0101;

  WRITE_WORD(drp_srm_data->tables.tx_ibias[0], Ibias);
  WRITE_WORD(drp_srm_data->tables.tx_ibias[1], Ibias);

  // TX ON
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL,0x81);
  WAIT_US(TX_ON_SCRIPT_RUN_TIME) ;

  READ_WORD(drp_regs->DLO_ALPHAL, def_dlo_alphal);
  temp16 = (def_dlo_alphal & 0xFFF0) | (alpha & 0x000F);
  WRITE_WORD(drp_regs->DLO_ALPHAL, temp16);

  READ_WORD(drp_regs->DLO_ALPHAH, def_dlo_alphah);
  temp16 = (def_dlo_alphah & 0xFFC0) | (rho & 0x003F);
  WRITE_WORD(drp_regs->DLO_ALPHAH, temp16);

  READ_WORD(drp_regs->DLO_IIRL, def_dlo_iirl);
  temp16 = (def_dlo_iirl & ~0xFFFF) | (lambda & 0xFFFF);
  WRITE_WORD(drp_regs->DLO_IIRL, temp16);

  // TX_START high
  READ_WORD(drp_regs->DTX_CONFIGL,temp16);
  temp16 = temp16 | 0x0080;
  WRITE_WORD(drp_regs->DTX_CONFIGL,temp16);

  //    Wait for phe to settle
  WAIT_US(PHE_SETTLING_TIME);

  /* if ModulationSwtich = 0 then Modulation is off else ON*/
  /* Here the script truns on the modulation by default*/
  if (ModulationSwtich == 0)
  {
    /*Turn off the modulation*/
    READ_WORD(drp_regs->DLO_SEQH,temp16);
    temp16 = (temp16 & ~0x0008) ;
    WRITE_WORD(drp_regs->DLO_SEQH,temp16);
  }
  /* Fly back delay setting*/
  READ_WORD(drp_regs->ANA_USDL, ana_usdL);
  temp16 = ana_usdL & ~0x001c;  //mask and clear existing flyback delay bits
  temp16 |= (flybacksetting << 2); // set the fly back delay
  WRITE_WORD(drp_regs->ANA_USDL, temp16);

  /* Ckvd setting*/
  READ_WORD(drp_regs->DLO_DCOIFH, dlo_dcoifh);
  READ_WORD(drp_regs->DLO_DCOIFL, dlo_dcoifl);
  temp16 = (dlo_dcoifh & ~0x03) | ckvdsetting;
  WRITE_WORD(drp_regs->DLO_DCOIFH,temp16);
  temp16 = (dlo_dcoifl & ~0x03) | sigmadeltaorder;
  WRITE_WORD(drp_regs->DLO_DCOIFL,temp16);
#if DRP_BENCH_SW
  //TDC SD setting - turn it on
   READ_WORD(drp_regs->DLO_TDCSDH,dlo_tdcsh_save);
   temp16 = dlo_tdcsh_save | 0x0008;//set bit 19 to 1
   WRITE_WORD(drp_regs->DLO_TDCSDH,temp16);
#endif //#if DRP_BENCH_SW
  // Capture Data (Script 6)
  // R8 = Itteration Count  (DRP Register R8)
  *((uint32*) 0xffff0620)= itter_cnt;

  // Trigger PHE Analysis Script
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL,0x86);
  // 316 uS per itteration  (512 samples/itteration)(1/(26MHz/16))
  //WAIT_US(316*itter_cnt);

  //WAIT_US(PHE_SCRIPT_RUNTIME*itter_cnt);

  // Read the script start register
    READ_WORD(drp_regs->SCRIPT_STARTL,temp16);

    while(temp16 & 0x0100)
    {
    // Read the script start register
      READ_WORD(drp_regs->SCRIPT_STARTL,temp16);
    }

    // Script overhead
  WAIT_US(5) ;

  // DLO Period Inverse
  READ_WORD(drp_regs->DLO_PERINVL,temp16);
  *dlo_perinv = temp16 & 0x1FFF ;

  // Sum(PHE)   (DRP Register R4)
  *phe_sum = *((volatile UINT32*) 0xffff0610);

  // sum(phe**2)   (DRP Register R5)
  *phe_square = *((volatile UINT32*) 0xffff0614);
  // PHE Max value   (DRP Register R6)
  temp32 = *((UINT32*) 0xffff0618);
  *phe_max =(SINT16) ((temp32 >> 8) & 0xFFFF) ;

  // PHE Max value   (DRP Register R7)
  temp32 = *((UINT32*) 0xffff061c);
  *phe_min = (SINT16) ((temp32 >> 8) & 0xFFFF) ;

  // End TX Burst
  // TX_START low
  READ_WORD(drp_regs->DTX_CONFIGL,temp16);
  temp16 = (temp16 & ~0x0080);
  WRITE_WORD(drp_regs->DTX_CONFIGL,temp16);

  WAIT_US(10);

  WRITE_WORD(drp_srm_data->tables.tx_ibias[0], ibias0);
  WRITE_WORD(drp_srm_data->tables.tx_ibias[1], ibias1);

  WRITE_WORD(drp_regs->DLO_ALPHAL, def_dlo_alphal);
  WRITE_WORD(drp_regs->DLO_ALPHAH, def_dlo_alphah);
  WRITE_WORD(drp_regs->DLO_IIRL, def_dlo_iirl);
  /* restore Fly back delay setting*/
  WRITE_WORD(drp_regs->ANA_USDL, ana_usdL);
  /* restore Ckvd setting*/
  WRITE_WORD(drp_regs->DLO_DCOIFH,dlo_dcoifh);
  /* restore Ckvd setting*/
  WRITE_WORD(drp_regs->DLO_DCOIFL,dlo_dcoifl);
#if DRP_BENCH_SW
//restore the value
    WRITE_WORD(drp_regs->DLO_TDCSDH,dlo_tdcsh_save);
#endif //#if DRP_BENCH_SW

  // Idle
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL,0x8F);
  WAIT_US(IDLE_SCRIPT_RUN_TIME); /*Timing for locosto is 20uS and for uppcosto 10uS*/
}

/******************************************************************************
* FUNCTION NAME: dbb_set_phe_samples_setting
*
* DESCRIPTION:
*       This function does the initial setting for capturing the PHE samples.
*
*
* Return Value:      NONE
*
* Input Parameters:  NONE
*
* Output Parameters: None
* Functions Called:  NONE
*
******************************************************************************/
void dbb_set_phe_samples_setting()
{
  // DLO_TSTOUT is free running on decimated CKR b(22:20)
  WRITE_BYTE_HIGH(drp_regs->DLO_TESTL, 0x0200);

 // Select TSTclk1
   WRITE_WORD(drp_regs->DTST_MUX_SELH, 0x0001);

  // Select input for Demux from DLO b(15:8) = 0x07
  // Select phase_err_iir_0mean from DLO b(7:0) = 0x2C
  WRITE_WORD(drp_regs->DTST_MUX_SELL, 0x072C);
  // mem_tstclk0 is CKRD16
 // WRITE_BYTE_LOW(drp_regs->CKM_TSTCLKL, 0x0014);
 //mem tstclk1 is CKRD16
  WRITE_BYTE_HIGH(drp_regs->CKM_TSTCLKL, 0x1400);
  // decimated enable signal for DLO from CKR domain (CKR/16 = 1.6MHz) b(22:20) = b100
  WRITE_BYTE_LOW(drp_regs->CKM_TSTCLKH, 0x0040);
}

/******************************************************************************
* FUNCTION NAME: dbb_restore_phe_samples_setting
*
* DESCRIPTION:
*       This function restores the DRP
*
*
* Return Value:      NONE
*
* Input Parameters:  NONE
*
* Output Parameters: None
* Functions Called:  NONE
*
******************************************************************************/
void dbb_restore_phe_samples_setting()
{
  // DLO_TSTOUT is off
  WRITE_BYTE_HIGH(drp_regs->DLO_TESTL, 0x0000);
  // DTST is off
  WRITE_WORD(drp_regs->DTST_MUX_SELL, 0x0D00);
  WRITE_WORD(drp_regs->DTST_MUX_SELH, 0x0000);
  // TSTCLK is off
  WRITE_BYTE_LOW(drp_regs->CKM_TSTCLKL, 0x0000);
  // TSTCLK is not decimated
  WRITE_BYTE_LOW(drp_regs->CKM_TSTCLKH, 0x0000);
}

#endif //#if (DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW))

#endif

