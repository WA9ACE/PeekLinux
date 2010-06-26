#define __DRP_FILE__

/**********************************************************************************
drp_main.c
-- Copyright (c) 2004, Texas Instruments, Inc.
-- Author: Sumeer Bhatara, Shrinivas Gadkari and Pradeep P

main project function file and global declarations
***********************************************************************************
                               Change History
************************************************************************************

Version     Date           Author(s)        Comments
-------------------------------------------------------------------------------------
Ver 0.0     25 Nov 2004    SG, SB and PP    Locosto version based on DRP ver 0.23
Ver 1.01    29 Dec 2004    SG               Based on DRP Ref SW 1.0, with addresses
                                            of srm variables and tables aligned to
                                            locosto scripts version 129.03
                                            Added LOCOSTO_VALID flag.
                                            Added tables initialization for OP_DRP_STANDALONE = 1
                                            Added drp_srm_init function for OP_DRP_STANDALONE = 1,
                                            to automatically generate the srm variables and tables
                                            address, also automatically generates the ocp_write commands
                                            to be used to create the srm initialization portion of
                                            the drp hex data structure.
Ver 1.02    30 Dec 2004    SB               Default value of KDCO_INV is in SRM now.
                                            Gain tables have been arranged in descending order.
Ver 1.03    04 Jan 2005    SB               Added comments while generating the init.txt file.

Ver 2.00    5 July 2005    SG  Calibation and Test SW for Locosto 1.0
************************************************************************************/

#include <stdio.h>
#include <math.h>
#if (DRP_FW_BUILD==1)
#include "l1_types.h"
#include "l1_drp_if.h"
#define _USER_TYPES_
typedef unsigned char       C_BOOLEAN;
#include "drp_fw_struct.h"
#endif
#include "drp_defines.h"
#include "drp_extern_dependencies.h"
#if (DRP_FW_BUILD == 0)
  #define GENERAL_H
#endif
#include "drp_api.h"
#include "drp_calib_main.h"
#include "drp_main.h"
#if DRP_TDL_DFT
  #include "drp_phe_analysis_dft.h"
#endif

#if DRP_TEST_SW==0
#include "tpudrv.h"
#include "tpudrv61.h"
#include "l1_rf61.h"

#include "l1_macro.h"
#include "l1_confg.h"
#include "l1_const.h"
#include "l1_types.h"

#if TESTMODE
  #include "l1tm_defty.h"
#endif
#if (AUDIO_TASK == 1)
  #include "l1audio_const.h"
  #include "l1audio_cust.h"
  #include "l1audio_defty.h"
#endif
#if (L1_GTT == 1)
  #include "l1gtt_const.h"
  #include "l1gtt_defty.h"
#endif
#if (L1_MP3 == 1)
  #include "l1mp3_defty.h"
#endif
#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif

#include "l1_defty.h"

#endif

#define DRP_DEBUG 0
#if DRP_DEBUG
void test_function(void);
#endif

#define DRP_DELAY_DEBUG 0
#if DRP_DELAY_DEBUG
void drp_delay_check(void);
#endif

#if (DRP_FW_BUILD==0)
extern SINT16 afe_est_table[];
extern SINT16 abe_est_table[];
extern SINT16 tar_afe_gain_table[];
extern SINT16 tar_abe_gain_table[];
#endif

#if DRP_TEST_SW
#include "map.h"
#include "Result.h"

void drp_test_sw_main_loop(void);
void drp_tx_burst(UWORD16 rf_freq, UWORD16 burst_length, T_DRP_SW_DATA *sw_data_ptr);

#if (DRP_TEST_SW && DRP_BENCH_SW)
void drp_tx_burst_tpu_start(UWORD16 rf_freq, UWORD16 burst_length, T_DRP_SW_DATA *sw_data_ptr);
void drp_tx_burst_tpu(T_DRP_SW_DATA *sw_data_ptr);

//void drp_tx_burst_start_pa(UWORD16 rf_freq, UWORD16 burst_length, T_DRP_SW_DATA *sw_data_ptr);
void drp_tx_burst_start_pa(UWORD16 rf_freq, UWORD16 burst_length, UWORD16 power_lev, T_DRP_SW_DATA *sw_data_ptr);
void drp_tx_load_burst_pa(T_DRP_SW_DATA *sw_data_ptr);

void drp_apc_programing(void);
UWORD16 PROG_APCRAM(void);
UWORD16 PROG_APCLEV(UWORD16 apc_level);
UWORD16 PROG_APCDEL1(UWORD16 apcdel1_val);
UWORD16 PROG_APCDEL2(UWORD16 apcdel2_val);
UWORD16 PROG_APCCTRL1(UWORD8 apcptr_val);
UWORD16 PROG_APCCTRL2(UWORD16 apcctrlen_val);
#endif //#if (DRP_TDL_SW == 0)

void drp_rx_burst(UINT16 band, UINT16 channel, UINT16 abe_gain_indx, UINT16 afe_gain_indx, UINT16 gain_comp_enable, UINT16 pole_num, T_DRP_SW_DATA *sw_data_ptr, UINT16 pcb_config);
void drp_fft_check(void);

void drp_copy_calib_str_srm(T_DRP_SW_DATA *sw_data_ptr);
void drp_copy_calib_str_mcu(T_DRP_SW_DATA *sw_data_ptr);

void TP_Enable(UWORD16 on);
void TPU_ClkEnable(UWORD16 on);
void TP_Reset(UWORD16 on);


#if DRP_TDL_GPIO
void drp_read_gpio(void);
#define TRUE  1
#define FALSE 0
void drp_read_gpio(void);
void write_drp_memory(UINT32 address, UINT32 data);
void read_addr_data(void);
UINT32 conv_addr_drp_to_mcu(UINT32 address);
void read_drp_memory(UINT32 address);
void clear_read_register(void);
#endif

#if DRP_TEST_SW
void pulse_gpio39(UINT16 num);
void wait_clocks(UINT16 num);
void gpio_39_toggle(UINT16 state);
void write_drp_memory(UINT32 address, UINT32 data);
UINT32 conv_addr_drp_to_mcu(UINT32 address);
void read_drp_memory(UINT32 address);
#endif

#if DRP_TEST_SW && DRP_BURNIN
void pulse_kbc1(UINT16 num);
void drp2_burnin_txrx_test(UINT16 rf_freq);
#endif

void drp_calibrate_part1(UINT16 band, UINT16 pcb_config);
void drp_calibrate_part2(UINT16 band);

//Handshake parameters between the calibration routines and the test equipment
#define RTN_BUSY         0x01
#define RTN_COMPLETE     0x02

#define RTN_LOAD_SCRIPTS             (UINT32)0x0D
#define RTN_START_SCRIPT             (UINT32)0x10
#define RTN_COPY_CALIB_STR_MCU       (UINT32)0xCD
#define RTN_COPY_CALIB_STR_SRM       (UINT32)0xCE
#define RTN_TX_RX_COMMON_CALIB       (UINT32)0xD1
#define RTN_LNA_CFREQ_CALIB          (UINT32)0xD2
#define RTN_IQMC_CALIB               (UINT32)0xD3
#define RTN_MIXER_POLE_CALIB         (UINT32)0xD4
#define RTN_SCF_POLE_CALIB           (UINT32)0xD5
#define RTN_COPY_CALIBRATION_DATA    (UINT32)0xD6
#define RTN_RXGAIN_AFE_CALIBRATION   (UINT32)0xD9
  #define RTN_WRITE_DRP_MEMORY       (UINT32)0xDA
  #define RTN_READ_DRP_MEMORY        (UINT32)0xE8

#if DRP_TDL_GPIO
  #define RTN_CLEAR_READ_REGISTER    (UINT32)0xE9
#endif

#if DRP_PHASE2_CALIBRATION
  #define RTN_TEMP_SENSOR_CALIB      (UINT32)0xDB
//  #define RTN_DCXO_CALIB           (UINT32)0xDC
//  #define RTN_FLYBACK_CALIB        (UINT32)0xDD
//  #define RTN_DCO_IBIAS_CALIB      (UINT32)0xDE
#endif //DRP_PHASE2_CALIBRATION

#define RTN_DCO_IBIAS_CALIB          (UINT32)0xDE
#define RTN_FLYBACK_CALIB            (UINT32)0xDD
#define RTN_DCXO_CALIB               (UINT32)0xDC

#if DRP_TDL_SW
  #define RTN_FFT_AVG_FLOAT         (UINT32)0xEF
  #define RTN_FFT_AVG_FIXED         (UINT32)0xDF
#else
#define RTN_FFT_AVG_FLOAT           (UINT32)0xDF
#define RTN_FFT_AVG_FIXED           (UINT32)0xEF
#endif
#define RTN_FFT_AVG_ATE             (UINT32)0xFF

#define RTN_RX_TIMELINE              (UINT32)0xE0
#define RTN_CALIBRATE_PART1          (UINT32)0xE1
#define RTN_TX_TIMELINE              (UINT32)0xE2
#define RTN_CALIBRATE_PART2          (UINT32)0xE3
#define RTN_GET_ADDRESS_RX           (UINT32)0xE4
#define RTN_ROC_COMPENSATE           (UINT32)0xE6
//#define RTN_WAIT_CHECK             (UINT32)0xE8
//#define RTN_READ_TEMP              (UINT32)0xE9
#define RTN_DRP_EFUSE_INIT           (UINT32)0xEA

#define RTN_FFT_POST                 (UINT32)0xEE
#define RTN_FEM_RX                   (UINT32)0x41
#define RTN_FEM_TA_BURST_START       (UINT32)0x42
#define RTN_FEM_TA_BURST             (UINT32)0x43

#if DRP_TDL_SW_DEBUG
#define RTN_FFT_AVG_DEBUG           (UINT32)0xF0
#endif

#define RTN_DUMP_DC_OFFSET          (UINT32)0xF1
#define RTN_DUMP_MEASURED_TEMP      (UINT32)0xF2
#define RTN_PROCESS_DC_OFFSET       (UINT32)0xF3

#define RTN_RX_DFT                  (UINT32)0x50
#define RTN_ADPLL_CAP_BANK_DFT      (UINT32)0x51
#define RTN_PHE_ANALYZER_DFT        (UINT32)0xEC /*Reused from -Uppcosto- */


#define RTN_BURNIN_TEST             (UINT32)0x60
#define RTN_FLYBACKDELAY_CALIB      (UINT32)0xA1 /*[sunil] added this for new algo*/

#if DRP_BURNIN
#define PASS                    0
#define FAIL                    1
#include "drp_param.h"
extern UINT16 scripts_data[];
#endif

extern float g_Buffer[17*FFT_BUF_SIZE];
extern SINT16 g_rx_data_buffer_decimated[FFT_BUF_SIZE*2];

extern T_DRP_SRM_DCOFFSET drp_srm_dc_offset[512];
extern SINT8 drp_measured_temp[512];

#if DRP_TDL_GPIO
#define MAX_ADDR_DATA 50
UINT32 addr_array[MAX_ADDR_DATA];
UINT32 data_array[MAX_ADDR_DATA];
UINT16 addr_data_indx;
UINT32 dtst_dmux_const_saved = 0x00000001;
#endif

UINT16 g_burst_count, g_burst_max;
UINT16 g_tx_burst1[TX_BUFFER_LENGTH];
UINT16 g_tx_burst2[TX_BUFFER_LENGTH];
UINT16 g_tx_burst3[TX_BUFFER_LENGTH];
UINT16 g_tx_burst4[TX_BUFFER_LENGTH];

/*************** TPU Control *****************/
/*
 * Macros for defining TPU instructions
 */
  #define TPU_ADDR      0xFFFFF000l            // Strobe 1 address

  #define TPU_RAM       0xFFFF9000l            // TPU RAM

  #define TPU_REG       (TPU_ADDR)             // TPU register

  // TSP should be removed for Locosto. Leaving it as such as the TPU drivers are not yet ported.
  #define TSP_REG       0xFFFE0800l            // TSP register
  #define TPU_TIM       0xFFFE2000l            // ULPD register


#define TPU_CTRL          (TPU_REG + 0x00)
#define TPU_INT_CTRL      (TPU_REG + 0x02)
#define TPU_INT_STAT      (TPU_REG + 0x04)
#define TPU_OFFSET_REG    (TPU_REG + 0x0C)
#define TPU_SYNCHRO_REG   (TPU_REG + 0x0E)
#define TPU_DSP_PG        (TPU_REG + 0x20)

#define TPU_SLEEP             0
#define TPU_MOVE(addr,data)   (0x8000 | ((data)<<5) | (addr))
#define TPU_AT(time)          (0x2000 | (((time + 5000) % 5000)))
#define TPU_FAT(time)         (0x2000 | (time))  // Fast version without modulo
#define TPU_SYNC(time)        (0x6000 | (time))
#define TPU_WAIT(time)        (0xA000 | (time))
#define TPU_OFFSET(time)      (0x4000 | (time))
#define MOD5000(a)            (((a) + 5000) % 5000)


//- Base REGISTER definitions -
#define BIT_0       0x000001
#define BIT_1       0x000002
#define BIT_2       0x000004
#define BIT_3       0x000008
#define BIT_4       0x000010
#define BIT_5       0x000020
#define BIT_6       0x000040
#define BIT_7       0x000080
#define BIT_8       0x000100
#define BIT_9       0x000200
#define BIT_10      0x000400
#define BIT_11      0x000800
#define BIT_12      0x001000
#define BIT_13      0x002000
#define BIT_14      0x004000
#define BIT_15      0x008000

//- RF signals connected to TSPACT -
#define RX_START     BIT_0     // RX_START of DRP2
#define TX_START     BIT_1     // TX_START of DRP2 modulator
#define START_APC    BIT_2     // Start of the APC module
#define LDO_EN       BIT_3     // Activation of the internal LDO inside the APC bloc
#define APC_EN       BIT_5     // Enable of the APC module
#define START_ADC    BIT_0     // Activation of the Triton ADC
#define B3           BIT_3     // Control of the RFMD TX module
#define B1           BIT_5     // Control of the RFMD TX module
#define TX_EN        BIT_6     // Control of the RFMD TX module
#define B2           BIT_7     // Control of the RFMD TX module

#define TXM_SLEEP    (0)  // To avoid leakage during Deep-Seep

// DRP write register
#define OCP_DATA_MSB             0x05
#define OCP_DATA_LSB              0x04
#define OCP_ADDRESS_MSB       0x0B
#define OCP_ADDRESS_LSB        0x0A
#define OCP_ADDRESS_START   0x01

// TSPACT
#define REG_SPI_ACT_U  0x07
#define REG_SPI_ACT_L   0x06

// 3-band config
// RX_UP/DOWN and TX_UP/DOWN
#define RU_900     ( B1 | B3 )
#define RD_900     ( B1 | B3 )
#define TU_900     ( TX_EN | B1 | B3 )
#define TD_900     ( B1 | B3 )

#define RU_850     ( B3 )
#define RD_850     ( B3 )
#define TU_850     ( TX_EN | B3 )
#define TD_850     ( B3 )

#define RU_1800    ( B2 | B3 )
#define RD_1800    ( B2 | B3 )
#define TU_1800    ( TX_EN | B2 | B3 )
#define TD_1800    ( B2 | B3 )

#define RU_1900    ( B1 | B2 | B3 )
#define RD_1900    ( B1 | B2 | B3 )
#define TU_1900    ( TX_EN | B1 | B2 | B3 )
#define TD_1900    ( B1 | B2 | B3 )

// GSM 1.5 : TPU bits changed
//---------------------------------------
#define TPU_CTRL_RESET     0x0001
#define TSP_CTRL_RESET     0x0080
#define TPU_CTRL_T_PAGE    0x0002
#define TPU_CTRL_T_ENBL    0x0004
#define TPU_CTRL_D_ENBL    0x0010       // WARNING THIS BIT DOES NOT EXIST IN HYPERION
#define TPU_CTRL_SPI_RST   0x0080
#define TPU_CTRL_WAIT      0x0200
#define TPU_CTRL_CLK_EN    0x0400

#define MOVE_REG_TSP_TO_RF(data, addr)\
  {\
  *TP_Ptr++ = TPU_MOVE(OCP_DATA_MSB, ((data)>>8) & 0xFF);      \
  *TP_Ptr++ = TPU_MOVE(OCP_DATA_LSB, (data)     & 0xFF);      \
  *TP_Ptr++ = TPU_MOVE(OCP_ADDRESS_MSB, ((addr)>>8) & 0xFF);      \
  *TP_Ptr++ = TPU_MOVE(OCP_ADDRESS_LSB, (addr)     & 0xFF);     \
  *TP_Ptr++ = TPU_MOVE(OCP_ADDRESS_START,     0x01);                \
  }

// - TPU instruction into TSP timings ---
// 1 tpu instruction = 1 qbit
#define DLT_1     1      // 1 tpu instruction = 1 qbit
#define DLT_2     2      // 2 tpu instruction = 2 qbit
#define DLT_3     3      // 3 tpu instruction = 3 qbit
#define DLT_4     4      // 4 tpu instruction = 4 qbit

// - TPU command execution + serialization length ---
#define DLT_4B    5                   // 5*move

#define tdt         0                   // tx delta timing

#define TRF_T1    ( - 255 - DLT_4B - tdt )  //,    //  TRF_T1    Set TX synth
#define TRF_T2    ( - 235 - DLT_4B - tdt )  //,    //  TRF_T2    Power ON TX
#define TRF_T3    ( - 225 - DLT_1        )    //,    //  TRF_T3
#define TRF_T4    ( - 100 - DLT_1        )    //,    //  TRF_T4
#define TRF_T5    ( -  30 - DLT_1        )     //,    //  TRF_T5
#define TRF_T6    (     0 - DLT_1        )      //,    //  TRF_T6
#define TRF_T7    (     8 - DLT_1        )      //,    //  TRF_T7
#define TRF_T8    (    16 - DLT_1        )     //,    //  TRF_T8
  // - TX timings ---
  // - TX down:
  // The times below are offsets to when TXSTART goes down
#define TRF_T9    ( -  40 - DLT_1        )    //,     // TRF_T9   ADC read
#define TRF_T10  (     0 - DLT_1        )     //,     // TRF_T10   Disable APC
#define TRF_T11  (    16 - DLT_1        )    //,    // TRF_T11  Disable PA
#define TRF_T12  (    20 - DLT_1        )    //,    // TRF_T12  Disable TXSTART
#define TRF_T13  (    30 - DLT_4B       )   //,    // TRF_T13  Power off Locosto

UWORD16 *TP_Ptr = (UWORD16 *) TPU_RAM;


// APC Programming related

// Configuration registers
//-------------------------

#define APC_RAM_SIZE        20

  // Last bit is ignored at apc_top . A "0" or "1" is appended as the last bit.
  // This bit is ignored when the adderess goes to apc_top from rhea switch.
#define APCRAM_OFFSET       0x04
#define APCLEV_OFFSET       0x02
#define APCDEL1_OFFSET      0x06
#define APCDEL2_OFFSET      0x08
#define APCCTRL1_OFFSET     0x0A
#define APCCTRL2_OFFSET     0x0C

#define APC_LEV_MASK_VAL    0x03ff
#define APC_DEL_MASK_VAL    0x03ff
#define APC_CTRL1_MASK_VAL  0x0001
#define APC_CTRL2_MASK_VAL  0x01ff
#define APC_EN_MASK         0x0002
#define START_APC_MASK      0x0010


#define RHEA_SWITCH_APC 0xffff8800
#define APC_RHSW_ARM_CNF_MASK 0x03
#define APC_RHSW_ARM_CNF_DSP_MASK 0x02
#define APC_SWITCH_OFFSET         0x02
#define APC_RHSW_ARM_STA          RHEA_SWITCH_APC + APC_SWITCH_OFFSET
#define APC_RHSW_ARM_STA_MASK     0x000f

#define APC_ARM_PROCESS  0x00DD
#define APC_LEAD_PROCESS 0x00AA

UWORD16 comp_array    [APC_RAM_SIZE];

// GPIO Control

#define GPIO0_LATCH_IN         0xfffe4800        // GPIO0-15 input register
#define GPIO0_LATCH_OUT        0xfffe4802        // GPIO0-15 output register
#define GPIO0_CNTL             0xfffe4804        // GPIO0-15 io config, 1=inp, 0= out

#define GPIO1_LATCH_IN         0xfffe5000        // GPIO16-31 input register
#define GPIO1_LATCH_OUT        0xfffe5002        // GPIO16-31 output register
#define GPIO1_CNTL             0xfffe5004        // GPIO16-31 io config, 1=inp, 0= out

#define GPIO2_LATCH_IN         0xfffe5800        // GPIO32-48 input register
#define GPIO2_LATCH_OUT        0xfffe5802        // GPIO32-48 output register
#define GPIO2_CNTL             0xfffe5804        // GPIO32-48 io config, 1=inp, 0= out

#endif

/**************************************************************************************/
/*                 Variable Declarations                                              */
/**************************************************************************************/
#if (DRP_FW_BUILD==0)
T_DRP_SW_DATA *drp_sw_data;

// Base pointer to DRP Registers
T_DRP_REGS_STR *drp_regs = (T_DRP_REGS_STR*) DRP_REGS_BASE_ADD;

// Base pointer to SRM data structure for Calibration data and tables
T_DRP_SRM_DATA *drp_srm_data = (T_DRP_SRM_DATA*) DRP_SRM_DATA_ADD;

// Base pointer to SRM API data structure
T_DRP_SRM_API  *drp_srm_api  = (T_DRP_SRM_API*) DRP_SRM_API_ADD;     // Re-Used in L1/DSP software

UINT16 g_pcb_config = RF_BAND_SYSTEM_INDEX;

unsigned char drp_dcxo_calib_sw[] = {

/* Tag  */ 0x00, 0x00, 0x00, 0x00,
/* Vars */ 0x00, 0x00, 0x00, 0x00,

/* size */ 0x02, 0x00, 0x00, 0x00,
/* addr */ 0x40, 0x05, 0x00, 0x00,
0x04, 0x2a, 0x00, 0x00,

/* size */ 0x36, 0x00, 0x00, 0x00,
/* addr */ 0x04, 0x2a, 0x00, 0x00,
0x0f, 0x00, 0xac, 0x42, 0x07, 0x00, 0xad, 0x42, 0xe0, 0x00, 0x00, 0x82, 0xe0, 0x42, 0x08, 0x82,
0xe0, 0x84, 0x10, 0x82, 0x00, 0x03, 0x21, 0x85, 0x00, 0x88, 0x29, 0x81, 0x01, 0x00, 0x30, 0x9f,
0x00, 0x40, 0x21, 0x91, 0xa0, 0x04, 0x01, 0x82, 0xc0, 0x40, 0x10, 0xa4, 0xe8, 0x46, 0x08, 0x8e,
0xe0, 0x40, 0x08, 0x81, 0x00, 0x83, 0x31, 0x81, 0x00, 0x49, 0x29, 0x81, 0x00, 0x8e, 0x21, 0x8a,
0xf7, 0x3f, 0x01, 0xa9, 0xc0, 0x55, 0x08, 0x86, 0xc0, 0x95, 0x10, 0x86, 0xe8, 0x46, 0x18, 0x8e,
0xe3, 0xc4, 0x18, 0x8e, 0xe3, 0x42, 0x08, 0x8e, 0xe3, 0x84, 0x10, 0x8e, 0xe0, 0x44, 0x00, 0x81,
0xe0, 0x06, 0x00, 0x82, 0x0E, 0x00, 0x80, 0x22, 0x00, 0x00, 0x00, 0x00,

/* Final Record */
/* size */ 0x00, 0x00, 0x00, 0x00,

};
#endif
#if (DRP_TEST_SW && (DRP_TDL_DFT || DRP_TDL_SW || DRP_BENCH_SW))
/*[sunil] added this for phe- analyser code*/
unsigned char drp_phe_anayser_dft[] = {

// this is the for test clock 1
/* Tag  */ 0x00, 0x00, 0x00, 0x00,
/* Vars */ 0x00, 0x00, 0x00, 0x00,


/* size */ 0x02, 0x00, 0x00, 0x00,
/* addr */ 0x20, 0x05, 0x00, 0x00,


0x00, 0x23, 0x00, 0x00,

/* size */ 0x3e, 0x00, 0x00, 0x00,
/* addr */ 0x00, 0x23, 0x00, 0x00,


0x00, 0x00, 0x20, 0x9f, 0x00, 0x00, 0x28, 0x9f, 0x00, 0x80, 0x30, 0xa2, 0xff, 0x7f, 0x38, 0xa2,
0x00, 0x0b, 0x42, 0x85, 0x03, 0x00, 0x3a, 0x2f, 0x10, 0x00, 0x48, 0x9f, 0x00, 0x00, 0x58, 0x9f,
0x00, 0x00, 0x60, 0x9f, 0x00, 0x03, 0x42, 0x82, 0x02, 0x03, 0x00, 0xab, 0x6c, 0x5e, 0x53, 0x93,
0x00, 0x8e, 0x02, 0x8b, 0x01, 0x00, 0x00, 0xa9, 0x00, 0x80, 0x3a, 0xa4, 0x00, 0x8c, 0x02, 0x8a,
0x01, 0x00, 0x00, 0xa9, 0x00, 0x80, 0x32, 0xa4, 0x00, 0x91, 0x52, 0x86, 0x00, 0xd4, 0x5a, 0x81,
0x04, 0x94, 0x52, 0x8e, 0x00, 0x14, 0x63, 0x81, 0x00, 0x43, 0x4a, 0x82, 0xf3, 0x7f, 0x02, 0xaa,
0x00, 0x16, 0x21, 0x81, 0x00, 0x0b, 0x63, 0x86, 0x00, 0x58, 0x29, 0x81, 0xea, 0x3f, 0x02, 0xaa,
0x00, 0x13, 0x21, 0x86, 0x00, 0x00, 0x3a, 0x2f, 0x00, 0x00, 0x00, 0x00,

/* Final Record */
/* size */ 0x00, 0x00, 0x00, 0x00,

};
#endif //#if ( (DRP_TEST_SW && (DRP_TDL_DFT || DRP_TDL_SW || DRP_BENCH_SW)) )

#if 0
unsigned char drp_cfa_update_sw[] = {

/* Tag  */ 0x00, 0x00, 0x00, 0x00,
/* Vars */ 0x00, 0x00, 0x00, 0x00,


/* size */ 0x02, 0x00, 0x00, 0x00,
/* addr */ 0x1c, 0x05, 0x00, 0x00,


0x80, 0x2a, 0x00, 0x00,

/* size */ 0x12, 0x00, 0x00, 0x00,
/* addr */ 0x80, 0x2a, 0x00, 0x00,


0x09, 0x00, 0xa0, 0x20, 0x0b, 0x00, 0xa0, 0x20, 0x22, 0x6f, 0x23, 0x91, 0x4c, 0x48, 0x7b, 0x9a,
0x01, 0x00, 0x00, 0x60, 0x09, 0x00, 0xa0, 0x20, 0x08, 0x00, 0xa0, 0x20, 0x05, 0x00, 0x80, 0x22,
0x00, 0x00, 0x00, 0x00,

/* Final Record */
/* size */ 0x00, 0x00, 0x00, 0x00,
};
#endif

// Various Global Variables used by Calibration Code
#if LOCOSTO_VALID
#else
UINT16 macro_temp16; //for Macro definition purposes
#endif

#if DRP_STANDALONE_BUILD
#if (DRP_FW_BUILD==0)
#pragma DATA_SECTION(drp_sw_data_calib, ".drp_data");
T_DRP_SW_DATA drp_sw_data_calib;
extern UINT8 drp_ref_sw[];
extern T_DRP_SW_DATA drp_sw_data_init;
#endif
#endif


#if DRP_TEST_SW
T_DRP_SW_DATA drp_sw_data_calib;
extern UINT8 drp_ref_sw[];
extern T_DRP_SW_DATA drp_sw_data_init;

//#if FIXEDPOINT_FFT
  #include "fft.h"
  S16 CosTable[FFT_BUF_SIZE];
  U16 BitReverseTable[128];
//#endif

#endif

#if DRP_TEST_SW==0
/* Extern Global variables */
#if (DRP_FW_BUILD==0)
extern UWORD16  *TP_Ptr;
#endif
extern unsigned IQ_FrameCount;
#if (DRP_FW_BUILD==0)
extern SYS_UWORD32 drp_ref_sw_tag;
extern SYS_UWORD32 drp_ref_sw_ver;
extern T_L1_CONFIG l1_config;
#endif

#endif //#if DRP_TEST_SW==0


#if (DRP_FW_BUILD == 1)
extern const unsigned char drp_dcxo_calib_sw[];
extern volatile const T_DRP_CERT_TABLE  drp_fw_certificate;
#endif

#if DRP_STANDALONE_BUILD
  #if DRP_LOCOSTO_DEBUG
    UINT16 *drp_debug_ptr;
  #endif

#if (DRP_TEST_SW && !DRP_BURNIN)
/**************************************************************************************/
/*  drp_calib_main( ) is a function used during the standalone development and        */
/*  testing of the calibration SW.                                                    */
/**************************************************************************************/
void drp_calib_main(void){

  UINT16 indx, strsize;
  UINT8  *ptrsrc, *ptrdst;
  UINT32 version_no_ret;
  volatile UINT16 temp16;
#if 0
  SINT16 ret16;
#endif

  // Base pointer to DRP Registers
  drp_regs = (T_DRP_REGS_STR*) DRP_REGS_BASE_ADD;

  // Base pointer to SRM data structure for Calibration data and tables
  drp_srm_data = (T_DRP_SRM_DATA*) DRP_SRM_DATA_ADD;

  // Base pointer to SRM API data structure
  drp_srm_api  = (T_DRP_SRM_API*) DRP_SRM_API_ADD;     // Re-Used in L1/DSP software

  // load the reference SW in the drp srm
  version_no_ret = drp_copy_ref_sw_to_drpsrm(&drp_ref_sw[0]);

  // Copy drp_sw_data_init into drp_sw_data_calib
  strsize = sizeof(T_DRP_SW_DATA);
  ptrsrc = (UINT8 *)(&drp_sw_data_init);
  ptrdst = (UINT8 *)(&drp_sw_data_calib);

  for(indx=0;indx < strsize;indx++)
    *ptrdst++ = *ptrsrc++;

#if DRP_TEST_SW
  pulse_gpio39(2);
#endif

#if (DRP_TEST_SW && !DRP_BURNIN)
  //Load the LDO trim values from the EFUSE registers
  drp_efuse_init();
#endif

  // Start reg on
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0080);
  WAIT_US(500);

#if DRP_TEST_SW
  pulse_gpio39(3);
#endif

#if (DRP_TEST_SW && DRP_BENCH_SW)
  // Initialize the TPU

  TP_Reset(1); // reset TPU and TSP

  // GSM 1.5 : TPU clock enable is in TPU
  //---------------------------------------
  TPU_ClkEnable(1);         // TPU CLOCK ON

  TP_Reset(0);

  TP_Ptr = (UWORD16 *) TPU_RAM;

  // Allow some time for TPU initialization
  WAIT_US(1000);

#endif //#if (DRP_TEST_SW && DRP_BENCH_SW)

#if DRP_TEST_SW
  pulse_gpio39(4);
  drp_test_sw_main_loop( );
#endif

#if 0
  /*****************************************/
  /*          TX-RX Common Calibration                  */
  /*****************************************/

  drp_tx_rx_common_calib(pcb_config, &drp_sw_data_calib);

  /*****************************************/
  /*          RX Calibration for the EGSM Band          */
  /*****************************************/

  // -70 dbm signal at 942.5 MHz

  // LNA Center Freq Calibration
  ret16 = drp_lna_cfreq_calib(EGSM_BAND,
                              #if DRP_TEST_SW
                              0,
                              #endif
                              &drp_sw_data_calib);

  // IQMC Coefficient Calibration
  ret16 = drp_iqmc_calib(EGSM_BAND,
                         #if DRP_TEST_SW
                         0,
                         #endif
                         &drp_sw_data_calib);

  // Mixer pole calibration
  ret16 = drp_mixer_pole_calib(EGSM_BAND,
                               #if DRP_TEST_SW
                               0,
                               #endif
                               &drp_sw_data_calib);

  // SCF pole calibration
  ret16 = drp_scf_pole_calib(EGSM_BAND,
                             #if DRP_TEST_SW
                             0,
                             #endif
                             &drp_sw_data_calib);

  // AFE Gain calib part 1
  ret16 = drp_afe_gain_calib1(EGSM_BAND,
                              #if DRP_TEST_SW
                              0,
                              #endif
                              &drp_sw_data_calib);

  // -50 dbm signal at 942.5 MHz
  // AFE Gain calib part 2
  ret16 = drp_afe_gain_calib2(EGSM_BAND,
                              #if DRP_TEST_SW
                              0,
                              #endif
                              &drp_sw_data_calib);

  /********************************************/
  /*           Calibration Done                                      */
  /********************************************/

  // Start reg off script
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0087);
  WAIT_US(500);

  // Copy drp_sw_data_calib into drp_sw_data_calib_saved
  strsize = sizeof(T_DRP_SW_DATA);
  ptrsrc = (UINT8 *)(&drp_sw_data_calib);
  ptrdst = (UINT8 *)(&drp_sw_data_calib_saved);

  for(indx=0;indx < strsize;indx++)
    *ptrdst++ = *ptrsrc++;

  ret16 = drp_copy_sw_data_to_drpsrm(&drp_sw_data_calib_saved);

#endif
  return;
}
#endif //#if (DRP_TEST_SW && !DRP_BURNIN)

#endif // DRP_STANDALONE_BUILD

#if (DRP_TEST_SW == 0)
/**************************************************************************************/
/* DRP Gain correction for accurate RSSI                                              */
/**************************************************************************************/
#if (L1_FF_MULTIBAND == 0)
WORD16 drp_gain_correction(UWORD16 arfcn, UWORD8 lna_off, UWORD16 agc)
{
  SINT16 correction, correction_afe, correction_abe, temp, temp_agc;
  UINT16 band_index = 0, afe_gain_index, afe_applied_index, abe_gain_index, abe_applied_index;
  SINT16 afe_actual, abe_actual, afe_target, abe_target;
  //UINT16 temp_num = 0;

  drp_sw_data = &drp_sw_data_calib;

  if( (arfcn >= 1) && (arfcn <= 124) )
    band_index =  EGSM_BAND;

  if( ((arfcn >= 975 ) && (arfcn <= 1023)) || (arfcn == 0))
    band_index = EGSM_BAND;

  if( (arfcn >= 128 ) && (arfcn <= 251) )
    band_index = GSM_BAND;

  if( (arfcn >= 811  ) && (arfcn <= 885) )
    band_index = DCS_BAND;


  if( (arfcn >= 512) && (arfcn <= 810) )
    {
    #if (DRP_FW_BUILD==0)
    if((l1_config.std.id == PCS1900) || (l1_config.std.id == DUAL_US))
    #else
    if((*(drp_env_int_blk->modem_var_tbl->l1_config_std_id)==PCS1900) ||
       (*(drp_env_int_blk->modem_var_tbl->l1_config_std_id)==DUAL_US))
    #endif
       band_index = PCS_BAND;
     else
       band_index = DCS_BAND;
    }

  if (lna_off == 1)
    afe_gain_index = 0;
  else
    afe_gain_index = 1;

  temp = dbb_read_temp();

  afe_applied_index = estimate_afe_gain(afe_gain_index, temp, band_index, &afe_actual);
  //afe_actual = afe_est_table[afe_applied_index];
  //afe_target = tar_afe_gain_table[afe_gain_index];

  if (band_index > 1)
    afe_target = drp_srm_data->tables.tar_afe_gain_table[1][afe_gain_index];
  else
    afe_target = drp_srm_data->tables.tar_afe_gain_table[0][afe_gain_index];

  correction_afe = afe_target - afe_actual;

  temp_agc = (agc >> 1);
  switch (temp_agc)
  {
    case 0:        /* LNA_ON  */
    case 1:        /* LNA_OFF  */
      abe_gain_index = 0;
      break;

    case 2:        /* LNA_ON  */
    case 3:        /* LNA_OFF */
      abe_gain_index = 1;
      break;

    case 5:        /* LNA_OFF */
    case 6:        /* LNA_ON  */
      abe_gain_index = 2;
      break;

    case 8:        /* LNA_ON  */
    case 9:        /* LNA_OFF */
      abe_gain_index = 3;
      break;

    case 11:       /* LNA_OFF */
    case 12:       /* LNA_ON  */
      abe_gain_index = 4;
      break;

    case 14:       /* LNA_ON  */
    case 15:       /* LNA_OFF */
      abe_gain_index = 5;
      break;

    case 17:       /* LNA_OFF */
    case 18:       /* LNA_ON  */
      abe_gain_index = 6;
      break;

    case 20:       /* LNA_ON  */
    case 21:       /* LNA_OFF */
      abe_gain_index = 7;
      break;

    case 23:       /* LNA_OFF */
    case 24:       /* LNA_ON  */
      abe_gain_index = 8;
      break;

    case 26:       /* LNA_ON  */
    case 27:       /* LNA_OFF */
      abe_gain_index = 9;
      break;

    case 29:       /* LNA_OFF */
    case 30:       /* LNA_ON  */
      abe_gain_index = 10;
      break;

    default:
      abe_gain_index = 6;
      break;
  }

  abe_applied_index = estimate_abe_gain(abe_gain_index, temp, &abe_actual);
  //abe_actual = abe_est_table[abe_applied_index];
  //abe_target = tar_abe_gain_table[abe_gain_index];
  abe_target = drp_srm_data->tables.tar_abe_gain_table[abe_gain_index];
  correction_abe = abe_target - abe_actual;

  correction = correction_afe + correction_abe;

  // correction is in 1024*log2() format.
  // correction in dB = 10*log10(2^(correction/1024)
  //                  = (correction * 10*log10(2))/1024
  //                  = (correction * 3)/1024
  //
  // returned value is 2 * 10*log10(correction)
  correction = (SINT16) ((SINT32)(3 * 2 * ((SINT32) correction)) + 512) >> LOG_SCALE;

  return(correction);
}

#else // L1_FF_MULTIBAND = 1 below

WORD16 drp_gain_correction(UWORD16 arfcn, UWORD8 lna_off, UWORD16 agc)
{
  SINT16 correction, correction_afe, correction_abe, temp, temp_agc;
  UINT16 band_index = 0, afe_gain_index, afe_applied_index, abe_gain_index, abe_applied_index;
  SINT16 afe_actual, abe_actual, afe_target, abe_target;
  //UINT16 temp_num = 0;

  drp_sw_data = &drp_sw_data_calib;

  if( ((arfcn >= 0) && (arfcn<=124)) || ((arfcn >= 975) && (arfcn<=1023)) )
  {
    band_index=EGSM_BAND; // PGSM900 or EGSM900
  }
  else if( (arfcn >= 128) && (arfcn<=251) )
  {
    band_index=GSM_BAND; // GSM850
  }
  else if( (arfcn >= 512) && (arfcn<=885) )
  {
    band_index=DCS_BAND; // DCS1800
  }
  else if( (arfcn >= 1024) && (arfcn<=1322) )
  {
    band_index=PCS_BAND; // PCS1900
  }
  else
  {
    // ERROR
  }


  if (lna_off == 1)
    afe_gain_index = 0;
  else
    afe_gain_index = 1;

  temp = dbb_read_temp();

  afe_applied_index = estimate_afe_gain(afe_gain_index, temp, band_index, &afe_actual);
  //afe_actual = afe_est_table[afe_applied_index];
  //afe_target = tar_afe_gain_table[afe_gain_index];

  if (band_index > 1)
    afe_target = drp_srm_data->tables.tar_afe_gain_table[1][afe_gain_index];
  else
    afe_target = drp_srm_data->tables.tar_afe_gain_table[0][afe_gain_index];

  correction_afe = afe_target - afe_actual;

  temp_agc = (agc >> 1);
  switch (temp_agc)
  {
    case 0:        /* LNA_ON  */
    case 1:        /* LNA_OFF  */
      abe_gain_index = 0;
      break;

    case 2:        /* LNA_ON  */
    case 3:        /* LNA_OFF */
      abe_gain_index = 1;
      break;

    case 5:        /* LNA_OFF */
    case 6:        /* LNA_ON  */
      abe_gain_index = 2;
      break;

    case 8:        /* LNA_ON  */
    case 9:        /* LNA_OFF */
      abe_gain_index = 3;
      break;

    case 11:       /* LNA_OFF */
    case 12:       /* LNA_ON  */
      abe_gain_index = 4;
      break;

    case 14:       /* LNA_ON  */
    case 15:       /* LNA_OFF */
      abe_gain_index = 5;
      break;

    case 17:       /* LNA_OFF */
    case 18:       /* LNA_ON  */
      abe_gain_index = 6;
      break;

    case 20:       /* LNA_ON  */
    case 21:       /* LNA_OFF */
      abe_gain_index = 7;
      break;

    case 23:       /* LNA_OFF */
    case 24:       /* LNA_ON  */
      abe_gain_index = 8;
      break;

    case 26:       /* LNA_ON  */
    case 27:       /* LNA_OFF */
      abe_gain_index = 9;
      break;

    case 29:       /* LNA_OFF */
    case 30:       /* LNA_ON  */
      abe_gain_index = 10;
      break;

    default:
      abe_gain_index = 6;
      break;
  }

  abe_applied_index = estimate_abe_gain(abe_gain_index, temp, &abe_actual);
  //abe_actual = abe_est_table[abe_applied_index];
  //abe_target = tar_abe_gain_table[abe_gain_index];
  abe_target = drp_srm_data->tables.tar_abe_gain_table[abe_gain_index];
  correction_abe = abe_target - abe_actual;

  correction = correction_afe + correction_abe;

  // correction is in 1024*log2() format.
  // correction in dB = 10*log10(2^(correction/1024)
  //                  = (correction * 10*log10(2))/1024
  //                  = (correction * 3)/1024
  //
  // returned value is 2 * 10*log10(correction)
  correction = (SINT16) ((SINT32)(3 * 2 * ((SINT32) correction)) + 512) >> LOG_SCALE;

  return(correction);
}

#endif // #if (L1_FF_MULTIBAND == 0) else
#endif

/**************************************************************************************/
/*    Function: drp_addr_init                                                         */
/*    INPUT/OUTPUT Params: Void                                                       */
/*    Description: This function initializes the Variables which are used by L1 MCU to*/
/*                      access the DRP related structs and registers                  */
/**************************************************************************************/
void drp_api_addr_init()
{

   // Base pointer to DRP Registers
   drp_regs = (T_DRP_REGS_STR*) DRP_REGS_BASE_ADD;

   // Base pointer to SRM data structure for Calibration data and tables
   drp_srm_data = (T_DRP_SRM_DATA*) DRP_SRM_DATA_ADD;

   // Base pointer to SRM API data structure
   drp_srm_api  = (T_DRP_SRM_API*) DRP_SRM_API_ADD;     // Re-Used in L1/DSP software
}

#if ((DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN)) || (!DRP_TEST_SW))
SINT16 drp_copy_sw_data_to_drpsrm(T_DRP_SW_DATA *drp_sw_data_loc)
{
  UINT16 index,indx1;
  volatile UINT32 mem_data;

  WRITE_WORD(drp_srm_data->txrx.perinv,drp_sw_data_loc->calib.txrx_common.perinv_value);
  //WRITE_WORD(drp_srm_data->txrx.dlo_tune_at,drp_sw_data_loc->calib.txrx_common.dlo_at_value);
  WRITE_WORD(drp_regs->DLO_TUNE_A_TL,drp_sw_data_loc->calib.txrx_common.dlo_at_value);

  //TX related calibration results
  for (index = 0; index < NUM_BANDS; index++)
  {
    WRITE_WORD(drp_srm_data->tx.pvt[index],drp_sw_data_loc->calib.txrx_common.pvt_value[index]);
    WRITE_WORD(drp_srm_data->tx.kdco_t[index],drp_sw_data_loc->calib.txrx_common.kdco_inv[index]);
  }

  //RX related calibration results
  for (index = 0; index < NUM_BANDS; index++)
  {
    WRITE_WORD(drp_srm_data->rx.pvt[index],drp_sw_data_loc->calib.txrx_common.pvt_value[index + NUM_BANDS]);
    WRITE_WORD(drp_srm_data->rx.kdco_t[index],drp_sw_data_loc->calib.txrx_common.kdco_inv[index + NUM_BANDS]);

    //LNA CFREQ Calibration
    mem_data = drp_sw_data_loc->calib.rx.lna.cfreq[index];
    WRITE_WORD(drp_srm_data->rx.cfreq[index], (UINT16)mem_data);

    //IQMC coefficients
    mem_data = drp_sw_data_loc->calib.rx.iqmc.coeff[index][0]; //Real part
    WRITE_WORD_AT_PTR((((UINT16 *)&(drp_srm_data->rx.iqmc[index])) + 1), (UINT16)mem_data);
    mem_data = drp_sw_data_loc->calib.rx.iqmc.coeff[index][1]; //Imaginary part
    WRITE_WORD_AT_PTR((UINT16*)(&(drp_srm_data->rx.iqmc[index])), (UINT16)mem_data);

    //ANA_MIX values
    mem_data = drp_sw_data_loc->calib.rx.scf_pole.ANA_MIX_LB_IQ_table[index];
    WRITE_WORD_AT_PTR((UINT16 *)(&(drp_srm_data->rx.mix_lb[index])),(mem_data & 0xFFFF));
    WRITE_WORD_AT_PTR((((UINT16 *)&(drp_srm_data->rx.mix_lb[index]))+1),(mem_data >> 16));

    //SCF poles
    mem_data = drp_sw_data_loc->calib.rx.scf_pole.ANA_SCF_IQ_0_table[index];
    WRITE_WORD_AT_PTR((UINT16 *)(&(drp_srm_data->rx.scf[index])),(mem_data & 0xFFFF));
    WRITE_WORD_AT_PTR((((UINT16 *)&(drp_srm_data->rx.scf[index]))+1),(mem_data >> 16));

    mem_data = drp_sw_data_loc->calib.rx.scf_pole.ANA_SCF_IQ_0_table[index+NUM_BANDS];
    WRITE_WORD_AT_PTR(((UINT16 *)&(drp_srm_data->rx.scf[index+NUM_BANDS])),(mem_data & 0xFFFF));
    WRITE_WORD_AT_PTR((((UINT16 *)&(drp_srm_data->rx.scf[index+NUM_BANDS]))+1),(mem_data >> 16));
  }

  // ABE gain calibration
  for(indx1=0;indx1 < NUM_ABE_GAINS - 2;indx1++){
    mem_data = drp_sw_data_loc->calib.rx.abe.gain_alpha_table[indx1];
    WRITE_WORD_AT_PTR(((UINT16 *)&(drp_srm_data->tables.abe_calib_table[indx1])),(mem_data & 0xFFFF));
    WRITE_WORD_AT_PTR((((UINT16 *)&(drp_srm_data->tables.abe_calib_table[indx1]))+1),(mem_data >> 16));
  }

  //force the last two gain values to -12dB to prevent RSSI errors
  for(indx1 = NUM_ABE_GAINS - 2;indx1 < NUM_ABE_GAINS;indx1++){
    mem_data = drp_sw_data->calib.rx.abe.gain_alpha_table[indx1];
    WRITE_WORD_AT_PTR((UINT16 *)&(drp_srm_data->tables.abe_calib_table[indx1]),(mem_data & 0xFFFF));
    WRITE_WORD_AT_PTR(((UINT16 *)&(drp_srm_data->tables.abe_calib_table[indx1]))+1,(0xF000));
  }

  // AFE gain calibration
  for(index=0;index < NUM_BANDS;index++){
    for(indx1=0;indx1 < NUM_AFE_GAINS;indx1++){
      mem_data = ((UINT16)drp_sw_data_loc->calib.rx.afe.gain[index][indx1]);
      WRITE_WORD_AT_PTR(((UINT16 *)&(drp_srm_data->tables.afe_calib_table[index][indx1])),(mem_data & 0xFFFF));
    }
  }

  // copy DXCO calibration data into registers
  READ_WORD(drp_regs->ANA_LDO_DCXOL,mem_data);
  mem_data = mem_data & ~(0x0FFC);
  mem_data = mem_data | ((drp_sw_data_loc->calib.dcxo.cfa_value) & 0x0FFC);

  set_dcxo_cfa(mem_data);

  //make DCXO IDAC denominator 0 (as was done during calibration)
  WRITE_WORD(drp_regs->ANA_RESERVEDL, 0x0000);

  //make DCXO FFA 0
  WRITE_WORD(drp_regs->DCXO_XTALL, 0x0000);

  mem_data = (drp_sw_data_loc->calib.dcxo.idac_value) & 0x01FF;
  WRITE_WORD(drp_regs->DCXO_IDACL,mem_data);

#if DRP_FLYBACK_IBIAS_CALIB
  // Flyback delay calibration results
  for(index=0;index < (NUM_BANDS * NUM_FLYBACK_SUBBANDS);index++){
    drp_srm_data->tx.ckv_dly[index] = drp_sw_data_loc->calib.txrx_common.ckv_dly[index];
  }

  // Period Inverse calibration results
  for(index=0;index < (NUM_BANDS * NUM_FLYBACK_SUBBANDS);index++){
    for(indx1=0;indx1 < 3;indx1++){
      drp_srm_data->tx.perinv_thresh[index][indx1] = drp_sw_data_loc->calib.txrx_common.perinv_thresh[index][indx1];
    }
  }

  // IBias calibration results
  for(index=0;index < (NUM_BANDS * NUM_FLYBACK_SUBBANDS);index++){
    drp_srm_data->tx.ibias[index] = drp_sw_data_loc->calib.txrx_common.ibias[index];
  }
#endif

#if (DRP_TEST_SW == 0)
  if (drp_sw_data_loc->length > 0x01A8)
  {
#endif
    //new calibration data is available. So copy flyback calibration data to DRP SRM
    if ((drp_sw_data_loc->calib.drp_sw_data_ver_efuse.version >> 8) == 0x008A)
    {
      for (index = 0; index < 2; index++)
      {
        WRITE_WORD(drp_srm_data->flyback_xmax_table[index], drp_sw_data_loc->calib.flyback.xmax_table[index]);
        WRITE_WORD(drp_srm_data->flyback_cf_table[index], drp_sw_data_loc->calib.flyback.cf_table[index]);
        WRITE_WORD(drp_srm_data->flyback_ct_table[index], drp_sw_data_loc->calib.flyback.ct_table[index]);
        WRITE_WORD(drp_srm_data->flyback_perinvcal_table[index], drp_sw_data_loc->calib.flyback.perinvcal_table[index]);
        WRITE_WORD(drp_srm_data->flyback_fcal_table[index], drp_sw_data_loc->calib.flyback.fcal_table[index]);
      }
    }
#if (DRP_TEST_SW == 0)
  }
#endif
  return 0;
}

#endif //#if ((DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN)) || (!DRP_TEST_SW))

/**************************************************************************************/
/* load_srm_data: loads the script data contained in drp_ref_sw  */
/* into SRM.                                                     */
/**************************************************************************************/
uint32  drp_copy_ref_sw_to_drpsrm(unsigned char *ref_sw_ptr){

  UINT32 temp;
  UINT32 size;
  UINT32 version_no;
  UINT16 indx;
  UINT16 *ptr16;
  UINT32 tmp_addr;
  UINT16 *drp_addr;
  UINT16 mem_data;
  UINT16 data_read_complete;

#if DRP_TEST_SW==0
  UINT32 *drp_sw_ptr;

  drp_sw_ptr =  (UINT32 *) (ref_sw_ptr) ;

  // read tag
  drp_ref_sw_tag = *drp_sw_ptr++;

  // read version
  drp_ref_sw_ver = *drp_sw_ptr++;
#endif

  drp_sw_data = &drp_sw_data_calib;

  ptr16 = (UINT16*)(ref_sw_ptr);
  ptr16++;
  ptr16++;  //ptr32 now points to size of segment1
  version_no = *ptr16++;
  version_no |= ((*ptr16++) << 16);

  data_read_complete = 0;
  while(data_read_complete == 0){

    size = *ptr16++;
    ptr16++;
    if(size > 0){

      temp = *ptr16++;
      temp = temp + ((*ptr16++) << 16);

      if(temp & DRP_EXTERNAL_MEMORY_OCP_ADD){
        drp_addr = (UINT16*)(temp + DRP_EXTERNAL_MEMORY_ADD - DRP_EXTERNAL_MEMORY_OCP_ADD);
      }
      else{
        drp_addr = (UINT16*)(temp + DRP_REGS_BASE_ADD - DRP_REGS_BASE_OCP_ADD);
      }

      for(indx=0;indx < size;indx++){
        mem_data = *ptr16++;
        tmp_addr = (UINT32)drp_addr;
        WRITE_WORD_AT_PTR(tmp_addr,mem_data);
        drp_addr++;
      }

    }
    else{
      data_read_complete = 1;
    }
  } /* while loop */

#if DRP_TEST_SW
  drp_sw_data->calib.drp_sw_data_ver_efuse.version = drp_regs->MEM_2058L;

  if (drp_srm_api->control.calib_ctl & 0x0200)
  {
    drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[0] = drp_regs->MEM_2300L;
    drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[1] = drp_regs->MEM_2300H;
    drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[2] = drp_regs->MEM_2304L;
    drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[3] = drp_regs->MEM_2304H;
  }
  else
  {
    drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[0] = drp_regs->EFUSE0L;
    drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[1] = drp_regs->EFUSE0H;
    drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[2] = drp_regs->EFUSE1L;
    drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[3] = drp_regs->EFUSE1H;
  }
#endif

  return(version_no);

}

#if (DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN))
//#!# must call with low band first!!
//#!# must call with high band second!!
void drp_calibrate_part1(UINT16 band, UINT16 pcb_config)
{
  UINT16 ret16;

  if(band < 2) drp_tx_rx_common_calib(pcb_config, &drp_sw_data_calib);

#if ((DRP_BURNIN == 0) && (DRP_TDL_DFT == 0))
  // the next routines (except rxgain_abe_calibration) require an input (-60dBm).
  ret16 = drp_lna_cfreq_calib(band, pcb_config, &drp_sw_data_calib);
  ret16 = drp_mixer_pole_calib(band, pcb_config, &drp_sw_data_calib);
  ret16 = drp_scf_pole_calib(band, pcb_config, &drp_sw_data_calib);

  ret16 = drp_afe_gain_calib1(band, pcb_config, &drp_sw_data_calib);
  ret16 = drp_iqmc_calib(band, pcb_config, &drp_sw_data_calib);
#endif

  return;
}

//#!# must do calibrate_part1(band) first!!
//#!# then calibrate_part2(band) first!!
//#!# Requires an input (-60dBm).
//#!#  Note: calibrate_part2(band) no longer required !
//#!#    Empty function here for test backward compatability!!
void drp_calibrate_part2(UINT16 band)
{
  //UINT16 ret16;

  //ret16 = drp_afe_gain_calib2(band, &drp_sw_data_calib);

  return;
}
#endif //#if (DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN))

#if ((DRP_TEST_SW && !DRP_BURNIN) || (!DRP_TEST_SW))

SINT16 drp_tx_rx_common_calib(
                              #if DRP_TEST_SW
                              UINT16 pcb_config,
                              #endif
                              T_DRP_SW_DATA *sw_data_ptr){

  UINT16 rxon_input_saved, temp16, calib_ctl_saved;
  UINT16 index,indx1;
  volatile UINT32 mem_data, efuse0, efuse1;
  T_DRP_FLYBACK_DELAY_PARAM flyback_pcs,flyback_dcs;

  drp_sw_data = sw_data_ptr;

  // No By-pass
  READ_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);
  drp_srm_api->control.calib_ctl &= 0xFE00;

  //flyback delay calibration parameters
  //dcs parameters
  flyback_dcs.rf_freqCentre = 0x4442;
  flyback_dcs.loop_count = 0x14;
  flyback_dcs.itter_cnt = 0x01;
  flyback_dcs.alpha = 0x09;
  flyback_dcs.rho = 0x14;
  flyback_dcs.lambda = 0x4444;
  flyback_dcs.ModulationSwtich = 0x00;
  flyback_dcs.ckvdsetting = 0x02;
  flyback_dcs.sigmadeltaorder = 0x02;
  flyback_dcs.cf = 0xFF62;
  flyback_dcs.ct = 0xE01C;
  //pcs parameters
  flyback_pcs.rf_freqCentre =  0x4970;
  flyback_pcs.loop_count= 0x14;
  flyback_pcs.itter_cnt = 0x1;
  flyback_pcs.alpha = 0x09;
  flyback_pcs.rho = 0x14;
  flyback_pcs.lambda = 0x4444;
  flyback_pcs.ModulationSwtich = 0x00;
  flyback_pcs.ckvdsetting = 0x02;
  flyback_pcs.sigmadeltaorder = 0x02;
  flyback_pcs.cf= 0xFF62;
  flyback_pcs.ct = 0xD422;

  // call the tx-rx common calib functions here
#if DRP_TEST_SW
  tx_rx_common_calib(pcb_config);
#else
  tx_rx_common_calib(g_pcb_config);
#endif

#if DRP_FLYBACK_CALIB
  if (drp_srm_api->control.calib_ctl & 0x0200)
  {
    efuse0 = *((UINT32 *)&drp_regs->MEM_2300L);
    efuse1 = *((UINT32 *)&drp_regs->MEM_2304L);
  }
  else
  {
    efuse0 = *((UINT32 *)&drp_regs->EFUSE0L);
    efuse1 = *((UINT32 *)&drp_regs->EFUSE1L);
  }

  if (((efuse0 & 1) == 1) && ((efuse1 & 0x0000C000) == 0x00004000))
  {
    if (efuse1 & 0x08000000)
    {
      //calibration for the DCS band
      flybackdelayCalibration(&flyback_dcs);
      //calibration for the PCS band
      flybackdelayCalibration(&flyback_pcs);
    }
  }
#endif

  // By-pass ABE and AFE gain compensation
  // disable mixer clocks during ABE gain measurement
  // disable PREF 6dB attenuation for RX calibrations
  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x041C);

  //disable gain compensation in the scripts
  READ_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);
  temp16 = rxon_input_saved & (~0x0002);
  temp16 = temp16 | 0x0001;
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

#if (DRP_TDL_DFT == 0)
  // call the abe gain calibration here, with band index 0 (can be any valid index here)
  rxgain_abe_calibration(0);
#endif //#if ((DRP_BURNIN == 0) && (DRP_TDL_DFT == 0))


  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0000);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);
  WRITE_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);

  // Ensure the calibrated data is also copied into SRM

  WRITE_WORD(drp_srm_data->txrx.perinv,drp_sw_data->calib.txrx_common.perinv_value);
  //WRITE_WORD(drp_srm_data->txrx.dlo_tune_at,drp_sw_data->calib.txrx_common.dlo_at_value);

  //TX related calibration results
  for (index = 0; index < NUM_BANDS; index++)
  {
    WRITE_WORD(drp_srm_data->tx.pvt[index],drp_sw_data->calib.txrx_common.pvt_value[index]);
    WRITE_WORD(drp_srm_data->tx.kdco_t[index],drp_sw_data->calib.txrx_common.kdco_inv[index]);
  }

  //RX related calibration results
  for (index = 0; index < NUM_BANDS; index++)
  {
    WRITE_WORD(drp_srm_data->rx.pvt[index],drp_sw_data->calib.txrx_common.pvt_value[index + NUM_BANDS]);
    WRITE_WORD(drp_srm_data->rx.kdco_t[index],drp_sw_data->calib.txrx_common.kdco_inv[index + NUM_BANDS]);
  }

  // ABE gain calibration
  for(indx1=0;indx1 < NUM_ABE_GAINS;indx1++){
    mem_data = drp_sw_data->calib.rx.abe.gain_alpha_table[indx1];
    WRITE_WORD_AT_PTR(((UINT16 *)&(drp_srm_data->tables.abe_calib_table[indx1])),(mem_data & 0xFFFF));
    WRITE_WORD_AT_PTR((((UINT16 *)&(drp_srm_data->tables.abe_calib_table[indx1]))+1),(mem_data >> 16));
  }

  return 0;

}

#endif

#if ((DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN)) || !DRP_TEST_SW)
SINT16 drp_lna_cfreq_calib(UWORD16 BAND_INDEX,
                           #if DRP_TEST_SW
                           UWORD16 pcb_config,
                           #endif
                           T_DRP_SW_DATA *sw_data_ptr){

  UINT32 FCW_OFFSET;
  UINT16 rxon_input_saved, temp16, calib_ctl_saved;
  UINT16 dbbif_setting;
  volatile UINT32 mem_data;
  UINT32 efuse1_31_16;

  drp_sw_data = sw_data_ptr;

  //DBBIF setting generation and passing to rxon_input
  READ_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);
  READ_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);

  efuse1_31_16 = drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[3];

  temp16 = rxon_input_saved & (~0x0300);
  // dbbif setting
  #if DRP_TEST_SW
    dbbif_setting = drp_generate_dbbif_setting(pcb_config, BAND_INDEX);
  #else
  dbbif_setting = drp_generate_dbbif_setting(g_pcb_config, BAND_INDEX);
  #endif

  temp16 = (temp16 & (~0x0300)) | ((dbbif_setting & 0x3) << 8);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

#if ((DRP_TEST_SW && DRP_BENCH_SW) || (!DRP_TEST_SW))
  // Call twice just in case first call happens at frame boundary
  drp_rx_tspact_enable(dbbif_setting);
  drp_rx_tspact_enable(dbbif_setting);
#endif

  // Offset of 99.447kHz, Offset = 2^24 * 99.447/26e3)
  FCW_OFFSET = 0x0000FAAB;
  drp_rx_fcw(BAND_INDEX, FCW_OFFSET);

  // By-pass ABE, AFE gain compensation in Scripts
  //disable 6dB attenuation in PREF
  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x041F);

  //disable gain compensation in the scripts
  READ_WORD(drp_srm_api->inout.rx.rxon_input, temp16);
  temp16 = temp16 & (~0x0002);
  temp16 = temp16 | 0x0001;
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

  if ((BAND_INDEX == GSM_BAND) || (BAND_INDEX == EGSM_BAND) ||
      ((BAND_INDEX == DCS_BAND) && ((efuse1_31_16 & 0x0002) == 0x0002)) ||
      ((BAND_INDEX == PCS_BAND) && ((efuse1_31_16 & 0x0002) == 0x0002))
     )
  {
  // call the calib functions here
  lna_cfreq_calibration(BAND_INDEX);
  }

  WRITE_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);

  //copy calibration results into srm
  mem_data = drp_sw_data->calib.rx.lna.cfreq[BAND_INDEX];
  WRITE_WORD(drp_srm_data->rx.cfreq[BAND_INDEX], (UINT16)mem_data);

  return 0;

}

SINT16 drp_iqmc_calib(UWORD16 BAND_INDEX,
                      #if DRP_TEST_SW
                      UWORD16 pcb_config,
                      #endif
                      T_DRP_SW_DATA *sw_data_ptr){

  UINT32 FCW_OFFSET;
  UINT16 rxon_input_saved, temp16, calib_ctl_saved;
  UINT16 dbbif_setting;
  volatile UINT32 mem_data;

  drp_sw_data = sw_data_ptr;

  //DBBIF setting generation and passing to rxon_input
  READ_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);
  READ_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);

  temp16 = rxon_input_saved & (~0x0300);
  // dbbif setting
  #if DRP_TEST_SW
    dbbif_setting = drp_generate_dbbif_setting(pcb_config, BAND_INDEX);
  #else
  dbbif_setting = drp_generate_dbbif_setting(g_pcb_config, BAND_INDEX);
  #endif

  temp16 = (temp16 & (~0x0300)) | ((dbbif_setting & 0x3) << 8);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

#if ((DRP_TEST_SW && DRP_BENCH_SW) || (!DRP_TEST_SW))
  // Call twice just in case first call happens at frame boundary
  drp_rx_tspact_enable(dbbif_setting);
  drp_rx_tspact_enable(dbbif_setting);
#endif

  // Offset of 300.455kHz, Offset = 2^24 * 300.455/26e3) i.e LO = RF - 300kHz
  // i.e IQMC block is calibrated with a RF - LO = +300kHz tone
  //FCW_OFFSET = 0x0002F555;


  // Offset of -300.455kHz, Offset = 2^24 * -300.455/26e3) i.e LO = RF + 300kHz
  // i.e IQMC block is calibrated with a RF - LO = -300kHz tone
  FCW_OFFSET = 0xFFFD0AAB;
  drp_rx_fcw(BAND_INDEX, FCW_OFFSET);

  // By-pass FCW computation and ABE, AFE gain compensation
  //disable 6dB attenuation in PREF
  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x041F);

  //disable gain compensation in the scripts
  READ_WORD(drp_srm_api->inout.rx.rxon_input, temp16);
  temp16 = temp16 & (~0x0002);
  temp16 = temp16 | 0x0001;
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

  // call the calib functions here
  iqmc_calibration(BAND_INDEX);

  WRITE_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);

  //copy calibration results into srm
  mem_data = drp_sw_data->calib.rx.iqmc.coeff[BAND_INDEX][0]; //Real part
  WRITE_WORD_AT_PTR(((UINT16 *)&(drp_srm_data->rx.iqmc[BAND_INDEX])) + 1, (UINT16)mem_data);
  mem_data = drp_sw_data->calib.rx.iqmc.coeff[BAND_INDEX][1]; //Imaginary part
  WRITE_WORD_AT_PTR((UINT16*)(&(drp_srm_data->rx.iqmc[BAND_INDEX])), (UINT16)mem_data);

  return 0;

}

SINT16 drp_mixer_pole_calib(UWORD16 BAND_INDEX,
                            #if DRP_TEST_SW
                            UWORD16 pcb_config,
                            #endif
                            T_DRP_SW_DATA *sw_data_ptr){

  UINT32 FCW_OFFSET;
  UINT16 rxon_input_saved, temp16, calib_ctl_saved;
  UINT16 dbbif_setting;
  volatile UINT32 mem_data;

  drp_sw_data = sw_data_ptr;

  //DBBIF setting generation and passing to rxon_input
  READ_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);
  READ_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);

  temp16 = rxon_input_saved & (~0x0300);
  // dbbif setting
  #if DRP_TEST_SW
    dbbif_setting = drp_generate_dbbif_setting(pcb_config, BAND_INDEX);
  #else
  dbbif_setting = drp_generate_dbbif_setting(g_pcb_config, BAND_INDEX);
  #endif
  temp16 = (temp16 & (~0x0300)) | ((dbbif_setting & 0x3) << 8);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

#if ((DRP_TEST_SW && DRP_BENCH_SW) || (!DRP_TEST_SW))
  // Call twice just in case first call happens at frame boundary
  drp_rx_tspact_enable(dbbif_setting);
  drp_rx_tspact_enable(dbbif_setting);
#endif

  // Offset of 400kHz, Offset = 2^24 * 400/26e3)
  FCW_OFFSET = 0x0003F03F;

  drp_rx_fcw(BAND_INDEX, FCW_OFFSET);
  // By-pass FCW computation and ABE, AFE gain compensation
  //disable 6dB attenuation in PREF
  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x041F);

  //disable gain compensation in the scripts
  READ_WORD(drp_srm_api->inout.rx.rxon_input, temp16);
  temp16 = temp16 & (~0x0002);
  temp16 = temp16 | 0x0001;
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

  // mixer pole calibration
  mixer_pole_calibration(BAND_INDEX);

  //WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0000);
  WRITE_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);

  //copy calibration results into srm
  mem_data = drp_sw_data->calib.rx.scf_pole.ANA_MIX_LB_IQ_table[BAND_INDEX];
  WRITE_WORD_AT_PTR(((UINT16 *)(&(drp_srm_data->rx.mix_lb[BAND_INDEX]))),(mem_data & 0xFFFF));
  WRITE_WORD_AT_PTR((((UINT16 *)&(drp_srm_data->rx.mix_lb[BAND_INDEX]))+1),(mem_data >> 16));

  return 0;
}

SINT16 drp_scf_pole_calib(UWORD16 BAND_INDEX,
                          #if DRP_TEST_SW
                          UWORD16 pcb_config,
                          #endif
                          T_DRP_SW_DATA *sw_data_ptr){

  UINT32 FCW_OFFSET;
  UINT32 scf_setting = 0x00000000;
  UINT16 rxon_input_saved, temp16, calib_ctl_saved;
  UINT16 pole_num;
  UINT16 dbbif_setting;
  volatile UINT32 mem_data;

  drp_sw_data = sw_data_ptr;

  //DBBIF setting generation and passing to rxon_input
  READ_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);
  READ_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);

  temp16 = rxon_input_saved & (~0x0300);
  // dbbif setting
  #if DRP_TEST_SW
    dbbif_setting = drp_generate_dbbif_setting(pcb_config, BAND_INDEX);
  #else
  dbbif_setting = drp_generate_dbbif_setting(g_pcb_config, BAND_INDEX);
  #endif
  temp16 = (temp16 & (~0x0300)) | ((dbbif_setting & 0x3) << 8);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

#if ((DRP_TEST_SW && DRP_BENCH_SW) || (!DRP_TEST_SW))
  // Call twice just in case first call happens at frame boundary
  drp_rx_tspact_enable(dbbif_setting);
  drp_rx_tspact_enable(dbbif_setting);
#endif

  //                       Ch      Cb1        Cb2
  // three sets of poles are 400kHz, 150kHz     270kHz
  //                       400kHz, 400kHz and 400kHz
  //                         400kHz, 270kHz and 270kHz (Backup, used only if EFUSE desires)
  for (pole_num = 0; pole_num < 2; pole_num++)
  {
    // By-pass FCW computation and ABE, AFE gain compensation
    //disable 6dB attenuation in PREF
    //drp_srm_api->control.calib_ctl |= 0x041F;
    WRITE_WORD(drp_srm_api->control.calib_ctl, 0x041F);

    //disable gain compensation in the scripts
    READ_WORD(drp_srm_api->inout.rx.rxon_input, temp16);
    temp16 = temp16 & (~0x0002);
    temp16 = temp16 | 0x0001;
    WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

    if (pole_num == 0)
    {
      // Offset of 150kHz, Offset = 2^24 * 150/26e3)
      FCW_OFFSET = 0x00017A18;
    }
    else if (pole_num == 1)
    {
      // Offset of 400kHz, Offset = 2^24 * 400/26e3)
      FCW_OFFSET = 0x0003F03F;
    }
    else
    {
      // Offset of 270kHz, Offset = 2^24 * 270/26e3)
      FCW_OFFSET = 0x0002A891;
    }

    drp_rx_fcw(BAND_INDEX, FCW_OFFSET);

    scf_setting = 0x00000000;
    // scf pole CB1, calibration
    scf_pole_calibrationCB1(BAND_INDEX, &scf_setting);

    if ((pole_num == 0) || (pole_num == 2))
    {
      // Offset of 270kHz, Offset = 2^24 * 270/26e3)
      FCW_OFFSET = 0x0002A891;
    }
    else // (pole_num == 1)
    {
      // Offset of 400kHz, Offset = 2^24 * 400/26e3)
      FCW_OFFSET = 0x0003F03F;
    }
    drp_rx_fcw(BAND_INDEX, FCW_OFFSET);

    // scf pole CB2, calibration
    scf_pole_calibrationCB2(BAND_INDEX, &scf_setting, pole_num);

    if (pole_num < 2)
    {
      drp_sw_data->calib.rx.scf_pole.ANA_SCF_IQ_0_table[(NUM_BANDS*pole_num) + BAND_INDEX] = scf_setting;
    drp_srm_data->rx.scf[BAND_INDEX + pole_num*NUM_BANDS] = scf_setting;
  }
#if 0
    else
    {
      drp_sw_data->calib.scfpole_calib2.ANA_SCF_IQ_0_table[BAND_INDEX] = scf_setting;
      //drp_srm_data->rx.scf[BAND_INDEX + pole_num*NUM_BANDS] = scf_setting;

      check_efuse_and_update_srm(BAND_INDEX, scf_setting);
    }
#endif
  }

  //WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0000);
  WRITE_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);

  //copy calibration results into srm
  //mem_data = drp_sw_data->calib.rx.scf_pole.ANA_SCF_IQ_0_table[BAND_INDEX];
  //WRITE_WORD_AT_PTR((UINT16 *)(&(drp_srm_data->rx.scf[BAND_INDEX])),(mem_data & 0xFFFF));
  //WRITE_WORD_AT_PTR(((UINT16 *)&(drp_srm_data->rx.scf[BAND_INDEX]))+1,(mem_data >> 16));

  //mem_data = drp_sw_data->calib.rx.scf_pole.ANA_SCF_IQ_0_table[BAND_INDEX+NUM_BANDS];
  //WRITE_WORD_AT_PTR((UINT16 *)&(drp_srm_data->rx.scf[BAND_INDEX+NUM_BANDS]),(mem_data & 0xFFFF));
  //WRITE_WORD_AT_PTR(((UINT16 *)&(drp_srm_data->rx.scf[BAND_INDEX+NUM_BANDS]))+1,(mem_data >> 16));

  return 0;
}

#if 0
void check_efuse_and_update_srm(UINT16 BAND_INDEX, UINT32 scf_setting)
{
  UINT32 efuse0, efuse1;

  if (drp_srm_api->control.calib_ctl & 0x0200)
  {
    efuse0 = *((UINT32 *)&drp_regs->MEM_2300L);
    efuse1 = *((UINT32 *)&drp_regs->MEM_2304L);
  }
  else
  {
    efuse0 = *((UINT32 *)&drp_regs->EFUSE0L);
    efuse1 = *((UINT32 *)&drp_regs->EFUSE1L);
  }
  if (efuse0 & 1)
  {
    if ((efuse1 & 0xC000) == 0x4000)
    {
      if (efuse0 & 0x80000000)
      {
        drp_srm_data->rx.scf[BAND_INDEX] = scf_setting;
      }
    }
  }
}
#endif

SINT16 drp_afe_gain_calib1(UWORD16 BAND_INDEX,
                           #if DRP_TEST_SW
                           UWORD16 pcb_config,
                           #endif
                           T_DRP_SW_DATA *sw_data_ptr){

  UINT32 FCW_OFFSET;
  UINT16 rxon_input_saved, temp16, calib_ctl_saved;
  UINT16 indx1;
  UINT16 dbbif_setting;
  volatile UINT32 mem_data;

  drp_sw_data = sw_data_ptr;

  //DBBIF setting generation and passing to rxon_input
  READ_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);
  READ_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);

  temp16 = rxon_input_saved & (~0x0300);
  // dbbif setting
  #if DRP_TEST_SW
    dbbif_setting = drp_generate_dbbif_setting(pcb_config, BAND_INDEX);
  #else
  dbbif_setting = drp_generate_dbbif_setting(g_pcb_config, BAND_INDEX);
  #endif
  temp16 = (temp16 & (~0x0300)) | ((dbbif_setting & 0x3) << 8);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

#if ((DRP_TEST_SW && DRP_BENCH_SW) || (!DRP_TEST_SW))
  // Call twice just in case first call happens at frame boundary
  drp_rx_tspact_enable(dbbif_setting);
  drp_rx_tspact_enable(dbbif_setting);
#endif

  // Offset = 99.447kHz, Offset = 2^24 * 99.447/26e3)
  FCW_OFFSET = 0x0000FAAB;

  drp_rx_fcw(BAND_INDEX, FCW_OFFSET);

  // By-pass FCW computation and ABE, AFE gain compensation
  //disable 6dB attenuation in PREF
  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x041F);

  //disable gain compensation in the scripts
  READ_WORD(drp_srm_api->inout.rx.rxon_input, temp16);
  temp16 = temp16 & (~0x0002);
  temp16 = temp16 | 0x0001;
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

  //update the de-Q bits if needed (bits 40:39 control de-Queing)
  //drp_efuse_update_deq(BAND_INDEX
  //                     #if DRP_TEST_SW
  //                     ,deq_ctl_flag
  //                     #endif
  //                    );

  //afe calibration, step=0, supply -60 dbm signal
  rxgain_afe_calibration(0, BAND_INDEX);
  rxgain_afe_calibration(14, BAND_INDEX);

  //WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0000);
  WRITE_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);

  // Copy AFE gain calibration into SRM
  for(indx1=0;indx1 < NUM_AFE_GAINS;indx1++){
    mem_data = ((UINT16)drp_sw_data->calib.rx.afe.gain[BAND_INDEX][indx1]);
    WRITE_WORD_AT_PTR((UINT16 *)&(drp_srm_data->tables.afe_calib_table[BAND_INDEX][indx1]),(mem_data & 0xFFFF));
  }

  return 0;
}

SINT16 drp_afe_gain_calib2(UWORD16 BAND_INDEX,
                           #if DRP_TEST_SW
                           UWORD16 pcb_config,
                           #endif
                           T_DRP_SW_DATA *sw_data_ptr){
#if 0
  volatile UINT32 FCW_OFFSET;
  volatile UINT16 rxon_input_saved, temp16, calib_ctl_saved;
  volatile UINT16 index,indx1;
  volatile UINT16 dbbif_setting;
  volatile UINT32 mem_data;

  drp_sw_data = sw_data_ptr;

  //DBBIF setting generation and passing to rxon_input
  READ_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);
  READ_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);

  temp16 = rxon_input_saved & (~0x0300);
  // dbbif setting
  #if DRP_TEST_SW
    dbbif_setting = drp_generate_dbbif_setting(pcb_config, BAND_INDEX);
  #else
  dbbif_setting = drp_generate_dbbif_setting(g_pcb_config, BAND_INDEX);
  #endif
  temp16 = (temp16 & (~0x0300))| ((dbbif_setting & 0x3) << 8);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

#if ((DRP_TEST_SW && DRP_BENCH_SW) || (!DRP_TEST_SW))
  // Call twice just in case first call happens at frame boundary
  drp_rx_tspact_enable(dbbif_setting);
  drp_rx_tspact_enable(dbbif_setting);
#endif

  // Offset = 99.447kHz, Offset = 2^24 *99.447/26e3)
  FCW_OFFSET = 0x0000FAAB;

  drp_rx_fcw(BAND_INDEX, FCW_OFFSET);

  // By-pass FCW computation and ABE, AFE gain compensation
  //disable 6dB attenuation in PREF
  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x041F);

  //disable gain compensation in the scripts
  READ_WORD(drp_srm_api->inout.rx.rxon_input, temp16);
  temp16 = temp16 & (~0x0002);
  temp16 = temp16 | 0x0001;
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

  //afe calibration, step= 14, supply -50 dbm signal
  rxgain_afe_calibration(14, BAND_INDEX);

  //WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0000);
  WRITE_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);

  // Copy AFE gain calibration into SRM
  for(index=0;index < NUM_BANDS;index++){
    for(indx1=0;indx1 < NUM_AFE_GAINS;indx1++){
      mem_data = ((UINT16)drp_sw_data->calib.rx.afe.gain[index][indx1]);
      WRITE_WORD_AT_PTR((UINT16 *)&(drp_srm_data->tables.afe_calib_table[index][indx1]),(mem_data & 0xFFFF));
    }
  }
#endif
  return 0;

}


#if DRP_FLYBACK_IBIAS_CALIB

SINT16 drp_dco_retiming_calibration(T_DRP_SW_DATA *sw_data_ptr){

  volatile UINT16 index,indx1;
  volatile UINT32 mem_data;

  drp_sw_data = sw_data_ptr;
  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x00C0);         // flyback being set by TX_ON, use CKVD2

  dco_retiming_calibration( );

  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0000);

  // Flyback delay calibration results
  for(index=0;index < (NUM_BANDS * NUM_FLYBACK_SUBBANDS);index++){
    drp_srm_data->tx.ckv_dly[index] = drp_sw_data->calib.txrx_common.ckv_dly[index];
  }

  // Period Inverse calibration results
  for(index=0;index < (NUM_BANDS * NUM_FLYBACK_SUBBANDS);index++){
    for(indx1=0;indx1 < 3;indx1++){
      drp_srm_data->tx.perinv_thresh[index][indx1] = drp_sw_data->calib.txrx_common.perinv_thresh[index][indx1];
    }
  }

  return 0;

}

SINT16 drp_dco_ibias_calibration(T_DRP_SW_DATA *sw_data_ptr){

  volatile UINT16 index,indx1;
  volatile UINT32 mem_data;

  drp_sw_data = sw_data_ptr;
  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0060);         // bypass flyback and ibias being set by TX_ON

  dco_ibias_calibration( );

  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0000);

  // IBias calibration results
  for(index=0;index < (NUM_BANDS * NUM_FLYBACK_SUBBANDS);index++){
    drp_srm_data->tx.ibias[index] = drp_sw_data->calib.txrx_common.ibias[index];
  }

  return 0;

}

#if DRP_TEST_SW

SINT16 drp_dco_retiming_data_collection(UWORD16 rf_freq, UWORD16 extended_tx_enable, T_DRP_SW_DATA *sw_data_ptr){

  drp_sw_data = sw_data_ptr;
  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0060);         // bypass flyback and ibias being set by TX_ON

  dco_retiming_data_collection(rf_freq, extended_tx_enable);

  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0000);
  return 0;

}

SINT16 drp_dco_ibias_data_collection(UWORD16 rf_freq, UWORD16 extended_tx_enable, T_DRP_SW_DATA *sw_data_ptr){

  drp_sw_data = sw_data_ptr;
  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0060);         // bypass flyback and ibias being set by TX_ON

  dco_ibias_data_collection(rf_freq, extended_tx_enable);

  WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0000);
  return 0;

}

#endif //#if DRP_TEST_SW

#endif // #if DRP_FLYBACK_IBIAS_CALIB

void drp_rx_fcw(UINT16 BAND_INDEX, UINT32 FCW_OFFSET){
  UINT32 FCW = 0; //OMAPS00090550
  UINT16 rf_freq = 0;

  switch(BAND_INDEX){
    case EGSM_BAND:
      // 2^24 * 924.4 * 2/26
      FCW = 0x487E07E0;
    // FCW_OFFSET
    FCW_OFFSET = FCW_OFFSET * 2;
      rf_freq = 0x24D0;
    break;

    case GSM_BAND:
      // 2^24 * 881.4 * 2/26
      FCW = 0x43CCCCCC;
    // FCW_OFFSET
    FCW_OFFSET = FCW_OFFSET * 2;
      rf_freq = 0x226E;
    break;

    case DCS_BAND:
      // 2^24 * 1842.4/26
      FCW = 0x46DC8DC8;
      rf_freq = 0x47F8;
    break;

    case PCS_BAND:
    // 2^24 * 1960/26
    FCW = 0x4B627627;
      rf_freq = 0x4C90;
    break;

  }

  FCW = FCW - FCW_OFFSET;
  WRITE_WORD(drp_regs->DLO_FCWL, (FCW & 0x0000FFFF));
  WRITE_WORD(drp_regs->DLO_FCWH, ((FCW >> 16) & 0x0000FFFF));

  WRITE_WORD(drp_regs->RF_FREQL, rf_freq);
  return;

}

#endif //#if ((DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN)) || (!DRP_TEST_SW))

#if (DRP_TEST_SW && !DRP_BURNIN)
void drp_test_sw_main_loop(void ){

  volatile UINT16 scenario_id;
  volatile UINT32 param[PARAM_NB];
  volatile UINT16 band, pole_num;
  volatile UINT16 channel, abe_gain_indx;
  volatile UINT16 afe_gain_indx,afe_gain_step;
  volatile UINT16 abe_gain_alpha;
  volatile UINT16 gain_comp_enable;
  volatile UINT16 roc_setting,RX_Burst_Mode;
  volatile UINT16 num_avg, phase_mm_bin;
  volatile UINT16 win_type, win_norm_type;
  volatile UINT16 roc_type, samples_persym;
  volatile SINT16 measured_temp;
  volatile UINT16 temp16;
  volatile UINT32 temp32;
  volatile UINT16 indx, strsize;
  volatile UINT8  *ptrsrc, *ptrdst;
#if DRP_TDL_GPIO
  volatile UINT16 * ptr_gpio;
#endif
  volatile SINT16 ret16, rx_total_gain;
  volatile UINT16 dbbif_setting;
  UINT16 pcb_config;
  UINT16 bin_interest, compute_noise, bin_start, bin_stop, num_bins;
  //UINT16 deq_ctl_flag;
#if DRP_TDL_SW_DEBUG
  float *results;
#endif
  UINT16 captureburst_computefft_flag;
  UINT16 fixed_float_flag;

#if DRP_TDL_DFT
  volatile UINT16 tone_indx, power_indx;
  UINT16 num_bursts, dft_type;
#endif
#if (DRP_TEST_SW && ( !DRP_BURNIN))
  UINT16 test_type;
#endif //#if (DRP_TEST_SW && ( !DRP_BURNIN))
  UINT32 address, data;

drp_sw_data = &drp_sw_data_calib;

#if DRP_TDL_GPIO
  // Configure GPIO0-5 as inputs
  ptr_gpio = (UINT16*)GPIO0_CNTL;
  temp16 = *ptr_gpio;
  temp16 |= 0x003F;
  *ptr_gpio = temp16;

  // GPIO-39 is used for handshake.
  // To enable GPIO - 39,37 as an output, Bit 7,5 of GPIO2_CNTL = 0
  ptr_gpio = (UINT16*)GPIO2_CNTL;
  temp16 = *ptr_gpio;
  temp16 &= ~(0x00A0);
  *ptr_gpio = temp16;

  // Now set address to actual GPIO output register
  ptr_gpio = (UINT16*)GPIO2_LATCH_OUT;

  //Default value for GPIO - 39 = 0
  temp16 = *ptr_gpio;
  temp16 &= ~(0x0080);
  *ptr_gpio = temp16;
#endif

  //use LPF mode for ROC since the scripts have set it in HPF mode for LIF (scripts should change this once
  //the baseband has the ROC code implemented)
  WRITE_WORD(drp_regs->ROC_CWL, 0x1080);

  temp32 = drp_copy_ref_sw_to_drpsrm(&drp_ref_sw[0]);

  // Copy drp_sw_data_init into drp_sw_data_calib
  strsize = sizeof(T_DRP_SW_DATA);
  ptrsrc = (UINT8 *)(&drp_sw_data_init);
  ptrdst = (UINT8 *)(&drp_sw_data_calib);

  for(indx=0;indx < strsize;indx++)
    *ptrdst++ = *ptrsrc++;

#if ((DRP_TEST_SW || DRP_TDL_DFT) && !DRP_BURNIN)
  //Load the LDO trim values from the EFUSE registers
  drp_efuse_init();
#endif

  // Start reg on
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0080);
  WAIT_US(500);

  /* Initialize the Scenario_ID and service routine parameters */
  ptrdst = (UINT8 *)&(drp_scenario_id);
  for(temp16=0;temp16 < 40;temp16++)
    *ptrdst++ = 0;

//#if FIXEDPOINT_FFT
  //Initialize FFT tables
  InitFft_ind( &CosTable[0], &BitReverseTable[0], FFT_BUF_SIZE_LOG2 );
//#endif

  pulse_gpio39(5);

#if DRP_DELAY_DEBUG
  drp_delay_check( );
#endif

  scenario_id = 0x0000;
  while(1){

    while(scenario_id == 0){

#if DRP_TDL_GPIO
      drp_read_gpio( );
      READ_WORD(drp_scenario_id,scenario_id);
#else
      READ_WORD(drp_scenario_id,scenario_id);
    WAIT_US(1);
#endif

    }

    param[0] = *((UINT32 *)&(drp_param1));
    param[1] = *((UINT32 *)&(drp_param2));
    param[2] = *((UINT32 *)&(drp_param3));
    param[3] = *((UINT32 *)&(drp_param4));
    param[4] = *((UINT32 *)&(drp_param5));
    param[5] = *((UINT32 *)&(drp_param6));
    param[6] = *((UINT32 *)&(drp_param7));
#if DRP_TDL_DFT
    param[7] = *((UINT32 *)&(drp_param8)); //sunil added this
#endif
    switch (scenario_id) {

#if (DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN))
    case RTN_COPY_CALIB_STR_MCU:
      WRITE_WORD(drp_handshake,(RTN_COPY_CALIB_STR_MCU << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      drp_copy_calib_str_mcu(&drp_sw_data_calib);
      WRITE_WORD(drp_handshake,(RTN_COPY_CALIB_STR_MCU << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;


    case RTN_COPY_CALIB_STR_SRM:
      WRITE_WORD(drp_handshake,(RTN_COPY_CALIB_STR_SRM << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      drp_copy_calib_str_srm(&drp_sw_data_calib);
      WRITE_WORD(drp_handshake,(RTN_COPY_CALIB_STR_SRM << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_LOAD_SCRIPTS:
      WRITE_WORD(drp_handshake,(RTN_LOAD_SCRIPTS << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      // load the reference SW in the drp srm
      temp32 = drp_copy_ref_sw_to_drpsrm(&drp_ref_sw[0]);
      WRITE_WORD(drp_handshake,(RTN_LOAD_SCRIPTS << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_START_SCRIPT:
      WRITE_WORD(drp_handshake,(RTN_START_SCRIPT << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      temp16 = param[0] & (0x0000000f);
      temp16 = temp16 | 0x0080;

      WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL,temp16);
      WRITE_WORD(drp_handshake,(RTN_START_SCRIPT << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_TX_TIMELINE:
      WRITE_WORD(drp_handshake,(RTN_TX_TIMELINE << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      drp_tx_burst((UINT16)param[0], (UINT16)param[1], &drp_sw_data_calib);
      WRITE_WORD(drp_handshake,(RTN_TX_TIMELINE << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_LNA_CFREQ_CALIB:
      WRITE_WORD(drp_handshake,(RTN_LNA_CFREQ_CALIB << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      band = (UINT16) (param[0] & 0xFFFF);
      pcb_config = (UINT16) ((param[1] & 0xFFFF) & 7);

      ret16 = drp_lna_cfreq_calib(band, pcb_config, &drp_sw_data_calib);
      WRITE_WORD(drp_handshake,(RTN_LNA_CFREQ_CALIB << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_IQMC_CALIB:
      WRITE_WORD(drp_handshake,(RTN_IQMC_CALIB << 8 | RTN_BUSY));
      gpio_39_toggle(1);

      band = (UINT16) (param[0] & 0xFFFF);
      pcb_config = (UINT16) ((param[1] & 0xFFFF) & 7);

      ret16 = drp_iqmc_calib(band, pcb_config, &drp_sw_data_calib);
      WRITE_WORD(drp_handshake,(RTN_IQMC_CALIB << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_MIXER_POLE_CALIB:
      WRITE_WORD(drp_handshake,(RTN_MIXER_POLE_CALIB << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      band = (UINT16) (param[0] & 0xFFFF);
      pcb_config = (UINT16) ((param[1] & 0xFFFF) & 7);

      ret16 = drp_mixer_pole_calib(band, pcb_config, &drp_sw_data_calib);
      WRITE_WORD(drp_handshake,(RTN_MIXER_POLE_CALIB << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_SCF_POLE_CALIB:
      WRITE_WORD(drp_handshake,(RTN_SCF_POLE_CALIB << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      band = (UINT16) (param[0] & 0xFFFF);
      pcb_config = (UINT16) ((param[1] & 0xFFFF) & 7);

      ret16 = drp_scf_pole_calib(band, pcb_config, &drp_sw_data_calib);
      WRITE_WORD(drp_handshake,(RTN_SCF_POLE_CALIB << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_COPY_CALIBRATION_DATA:
      WRITE_WORD(drp_handshake,(RTN_COPY_CALIBRATION_DATA << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      ret16 = drp_copy_sw_data_to_drpsrm(&drp_sw_data_calib);
      WRITE_WORD(drp_handshake,(RTN_COPY_CALIBRATION_DATA << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    //case RTN_RXGAIN_ABE_CALIBRATION:
    //  WRITE_WORD(drp_handshake,(RTN_RXGAIN_ABE_CALIBRATION << 8 | RTN_BUSY));
    //  band = (UINT16) (param[0] & 0xFFFF);
    //  rxgain_abe_calibration(band);
    //  WRITE_WORD(drp_handshake,(RTN_RXGAIN_ABE_CALIBRATION << 8 | RTN_COMPLETE));
    //  break;


    case RTN_RXGAIN_AFE_CALIBRATION:
      WRITE_WORD(drp_handshake,(RTN_RXGAIN_AFE_CALIBRATION << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      afe_gain_step = (UINT16) (param[0] & 0xFFFF);
      band = (UINT16) (param[1] & 0xFFFF);
      //deq_ctl_flag  = (UINT16) (param[2] & 0xFFFF);

      // By-pass ABE, AFE gain compensation in Scripts
      WRITE_WORD(drp_srm_api->control.calib_ctl, 0x001C);

      drp_sw_data = &drp_sw_data_calib;

      //rxgain_afe_calibration(afe_gain_step, band);
      ret16 = drp_afe_gain_calib1(band, pcb_config, &drp_sw_data_calib);

      // Restore the default value in calib_ctl
      WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0000);

      WRITE_WORD(drp_handshake,(RTN_RXGAIN_AFE_CALIBRATION << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_RX_TIMELINE:
      WRITE_WORD(drp_handshake,(RTN_RX_TIMELINE << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      band = (UINT16) (param[0] & 0xFFFF);
      channel = (UINT16) (param[1] & 0xFFFF);
      abe_gain_indx = (UINT16) (param[2] & 0xFFFF);
      afe_gain_indx = (UINT16) (param[3] & 0xFFFF);
      gain_comp_enable = (UINT16) (param[5] & 0xFFFF);
      pole_num = (UINT16) ((param[5] >> 16) & 0xFFFF);
      pcb_config       = (UINT16) ((param[6] & 0xFFFF) & 7);

      drp_sw_data = &drp_sw_data_calib;

      // By-pass ABE, AFE gain compensation in Scripts
      //READ_WORD(drp_srm_api->control.calib_ctl, temp16);
      //temp16 = (temp16 & 0x0100) | 0x000C;
      //WRITE_WORD(drp_srm_api->control.calib_ctl, temp16);

      rx_total_gain = rx_timeline_new(band, channel, abe_gain_indx, afe_gain_indx,
                                      gain_comp_enable, pole_num, pcb_config);

      temp16 = (UINT16)(rx_total_gain & (0x0000ffff));
      WRITE_WORD(drp_param5L,temp16);
      temp16 = (UINT16)((rx_total_gain >> 16) & 0x0000ffff);
      WRITE_WORD(drp_param5H,temp16);

      // Restore the default value in calib_ctl
      //READ_WORD(drp_srm_api->control.calib_ctl, temp16);
      //temp16 = (temp16 & 0x0100);;
      //WRITE_WORD(drp_srm_api->control.calib_ctl, temp16);

      WRITE_WORD(drp_handshake,(RTN_RX_TIMELINE << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;
#endif //#if (DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN))

#if (DRP_BENCH_SW || DRP_TDL_SW)
    case RTN_FFT_AVG_FIXED:
      WRITE_WORD(drp_handshake,(RTN_FFT_AVG_FIXED << 8 | RTN_BUSY));

#if DRP_BENCH_SW
      *(volatile UINT16 *)GPIO1_CNTL &= ~0x0002; //GPIO_17 i.e J104-11 on Visu
      *(volatile UINT16 *)0xFFFEF156 = 0x0000;
      *(volatile UINT16 *)GPIO1_LATCH_OUT |= 0x0002;
#endif

      gpio_39_toggle(1);
      band             = (UINT16) (param[0] & 0xFFFF);
      channel          = (UINT16) (param[1] & 0xFFFF);
      abe_gain_indx    = (UINT16) (param[2] & 0xFFFF);
      afe_gain_indx    = (UINT16) ((param[2] >> 16) & 0xFFFF);
      roc_setting      = (UINT16) (param[3] & 0xFF);
      pole_num = (UINT16) ((param[3] & 0xFF00)>>8);
      gain_comp_enable = (UINT16) ((param[3] >> 16) & 0xFFFF);
      num_avg          = (UINT16) (param[4] & 0xFFFF);
      phase_mm_bin     = (UINT16) ((param[4] >> 16) & 0xFFFF);
      win_type         = (UINT16) (param[5] & 0xFFFF);
      win_norm_type    = (UINT16) ((param[5] >> 16) & 0xFFFF);
      pcb_config       = (UINT16) ((param[6] & 0xFFFF) & 7);

      //for default gain settings
      drp_sw_data = &drp_sw_data_calib;

      // By-pass ABE, AFE gain compensation in Scripts
      //READ_WORD(drp_srm_api->control.calib_ctl, temp16);
      //temp16 = (temp16 & 0x0100) | 0x000C;
      //WRITE_WORD(drp_srm_api->control.calib_ctl, temp16);

      // Run FFT Average and return pointer to data buffer into parameter 1
      temp32 = (UINT32) fft_avg_fixed(band, channel, abe_gain_indx, afe_gain_indx, pole_num,
                                roc_setting, gain_comp_enable, phase_mm_bin, num_avg,
                                win_type, win_norm_type, pcb_config);

      temp16 = (UINT16)(temp32 & (0x0000ffff));
      WRITE_WORD(drp_param1L,temp16);
      temp16 = (UINT16)((temp32 >> 16) & 0x0000ffff);
      WRITE_WORD(drp_param1H,temp16);

      // Restore the default value in calib_ctl
      //READ_WORD(drp_srm_api->control.calib_ctl, temp16);
      //temp16 = (temp16 & 0x0100);
      //WRITE_WORD(drp_srm_api->control.calib_ctl, temp16);

      // Handshake to report service routine is done.
      WRITE_WORD(drp_handshake,(RTN_FFT_AVG_FIXED << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
#if DRP_BENCH_SW
      *(volatile UINT16 *)GPIO1_LATCH_OUT &= ~0x0002;
#endif
       break;
#endif

#if DRP_BENCH_SW
    case RTN_FFT_AVG_FLOAT:
      WRITE_WORD(drp_handshake,(RTN_FFT_AVG_FLOAT << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      band             = (UINT16) (param[0] & 0xFFFF);
      channel          = (UINT16) (param[1] & 0xFFFF);
      abe_gain_indx    = (UINT16) (param[2] & 0xFFFF);
      afe_gain_indx    = (UINT16) ((param[2] >> 16) & 0xFFFF);
      roc_setting      = (UINT16) (param[3] & 0xFF);
      pole_num         = (UINT16) ((param[3] & 0xFF00)>>8);
      gain_comp_enable = (UINT16) ((param[3] >> 16) & 0xFFFF);
      num_avg          = (UINT16) (param[4] & 0xFFFF);
      phase_mm_bin     = (UINT16) ((param[4] >> 16) & 0xFFFF);
      win_type         = (UINT16) (param[5] & 0xFFFF);
      win_norm_type    = (UINT16) ((param[5] >> 16) & 0xFFFF);
      pcb_config       = (UINT16) ((param[6] & 0xFFFF) & 7);
      //for default gain settings
      drp_sw_data = &drp_sw_data_calib;

      // By-pass ABE, AFE gain compensation in Scripts
      //READ_WORD(drp_srm_api->control.calib_ctl, temp16);
      //temp16 = (temp16 & 0x0100) | 0x000C;
      //WRITE_WORD(drp_srm_api->control.calib_ctl, temp16);

      // Run FFT Average and return pointer to data buffer into parameter 1
      temp32 = (UINT32) fft_avg_float(band, channel, abe_gain_indx, afe_gain_indx, pole_num,
                                roc_setting, gain_comp_enable, phase_mm_bin, num_avg,
                                win_type, win_norm_type, pcb_config);

      temp16 = (UINT16)(temp32 & (0x0000ffff));
      WRITE_WORD(drp_param1L,temp16);
      temp16 = (UINT16)((temp32 >> 16) & 0x0000ffff);
      WRITE_WORD(drp_param1H,temp16);

      // Restore the default value in calib_ctl
      //READ_WORD(drp_srm_api->control.calib_ctl, temp16);
      //temp16 = (temp16 & 0x0100);
      //WRITE_WORD(drp_srm_api->control.calib_ctl, temp16);

      // Handshake to report service routine is done.
      WRITE_WORD(drp_handshake,(RTN_FFT_AVG_FLOAT << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
       break;
#endif
#if (DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN))
    case RTN_FFT_AVG_ATE:
      WRITE_WORD(drp_handshake,(RTN_FFT_AVG_ATE << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      band             = (UINT16) (param[0] & 0xFFFF);
      channel          = (UINT16) (param[1] & 0xFFFF);
      abe_gain_indx    = (UINT16) (param[2] & 0xFFFF);
      afe_gain_indx    = (UINT16) ((param[2] >> 16) & 0xFFFF);
      roc_setting      = (UINT16) (param[3] & 0xFF);
      pole_num         = (UINT16) ((param[3] & 0xFF00)>>8);
      gain_comp_enable = (UINT16) ((param[3] >> 16) & 0xFFFF);
      num_avg          = (UINT16) (param[4] & 0xFFFF);
      phase_mm_bin     = (UINT16) ((param[4] >> 16) & 0xFFFF);
      win_type         = (UINT16) (param[5] & 0xFFFF);
      win_norm_type    = (UINT16) ((param[5] >> 16) & 0xFFFF);
      pcb_config       = (UINT16) ((param[6] & 0xFFFF) & 7);
      captureburst_computefft_flag = (UINT16) ((param[6] >> 16) & 0xFFFF);

      //for default gain settings
      drp_sw_data = &drp_sw_data_calib;

      // By-pass ABE, AFE gain compensation in Scripts
      //READ_WORD(drp_srm_api->control.calib_ctl, temp16);
      //temp16 = (temp16 & 0x0100) | 0x000C;
      //WRITE_WORD(drp_srm_api->control.calib_ctl, temp16);

      // Run FFT Average and return pointer to data buffer into parameter 1
      temp32 = (UINT32) fft_avg_ate(band, channel, abe_gain_indx, afe_gain_indx, pole_num,
                                    roc_setting, gain_comp_enable, phase_mm_bin, num_avg,
                                    win_type, win_norm_type, pcb_config, captureburst_computefft_flag);

      temp16 = (UINT16)(temp32 & (0x0000ffff));
      WRITE_WORD(drp_param1L,temp16);
      temp16 = (UINT16)((temp32 >> 16) & 0x0000ffff);
      WRITE_WORD(drp_param1H,temp16);

      // Restore the default value in calib_ctl
      //READ_WORD(drp_srm_api->control.calib_ctl, temp16);
      //temp16 = (temp16 & 0x0100);
      //WRITE_WORD(drp_srm_api->control.calib_ctl, temp16);

      // Handshake to report service routine is done.
      WRITE_WORD(drp_handshake,(RTN_FFT_AVG_ATE << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_ROC_COMPENSATE:
      WRITE_WORD(drp_handshake,(RTN_ROC_COMPENSATE << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      samples_persym = (UINT16) (param[1] & 0xFFFF);
      samples_persym = 1;
      //ref_roc((CMPLX16 *)DRP_SRM_RX_BUFFER_ADDR, 160, 381, (UINT16) 443);  //for 100kHz IF
      ref_roc((CMPLX16 *)DRP_SRM_RX_BUFFER_ADDR, 160, 381, (UINT16) 216);    //for 120kHz IF
      WRITE_WORD(drp_handshake,(RTN_ROC_COMPENSATE << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;
/*
    case RTN_WAIT_CHECK:
      WRITE_WORD(drp_handshake,(RTN_WAIT_CHECK << 8 | RTN_BUSY));
      WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0001);
      switch(param[0]) {
        case 1:
          WAIT_US(param[1]);
          break;
        case 2:
          wait_qb((UINT16) param[1]);
          break;
        default:
          // run no compensation if we don't match.
          break;
        }
      WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0000);
      WRITE_WORD(drp_handshake,(RTN_WAIT_CHECK << 8 | RTN_COMPLETE));
      break;

    case RTN_READ_TEMP:
      WRITE_WORD(drp_handshake,(RTN_READ_TEMP << 8 | RTN_BUSY));
      measured_temp = dbb_read_temp();
      WRITE_WORD(drp_param1,(UINT16)measured_temp);
      WRITE_WORD(drp_handshake,(RTN_READ_TEMP << 8 | RTN_COMPLETE));
      break;
*/

    case RTN_CALIBRATE_PART1:
      WRITE_WORD(drp_handshake,(RTN_CALIBRATE_PART1 << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      band = (UINT16)(param[0] & 0xFFFF);
      pcb_config = (UINT16) ((param[1] & 0xFFFF) & 7);
      //deq_ctl_flag = (UINT16) (param[2] & 0xFFFF);

      drp_calibrate_part1(band, pcb_config);

      WRITE_WORD(drp_handshake,(RTN_CALIBRATE_PART1 << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_CALIBRATE_PART2:
      WRITE_WORD(drp_handshake,(RTN_CALIBRATE_PART2 << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      band = (UINT16)(param[0] & 0xFFFF);
      drp_calibrate_part2(band);
      WRITE_WORD(drp_handshake,(RTN_CALIBRATE_PART2 << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_DCXO_CALIB:
      WRITE_WORD(drp_handshake,(RTN_DCXO_CALIB << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      band = (UINT16)(param[0] & 0xFFFF);
   pcb_config = (UINT16) ((param[1] & 0xFFFF) & 7);
      ret16 = drp_dcxo_calib(band, pcb_config, &drp_sw_data_calib);
      WRITE_WORD(drp_handshake,(RTN_DCXO_CALIB << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_FFT_POST:
      gpio_39_toggle(1);
      // Handshake to report service routine starts.
      WRITE_WORD(drp_handshake, (RTN_FFT_POST << 8 | RTN_BUSY));
      bin_interest  = (UINT16)(param[0] & 0xFFFF);
      compute_noise = (UINT16)(param[1] & 0xFFFF);
      bin_start     = (UINT16)(param[2] & 0xFFFF);
      bin_stop      = (UINT16)(param[3] & 0xFFFF);
      num_bins      = (UINT16)(param[4] & 0xFFFF);
      fixed_float_flag = (UINT16)(param[5] & 0xFFFF);

      // Run FFT Post evaluation and return pointer to data buffer into parameter 1
      fft_post_evaluation(bin_interest, compute_noise, bin_start, bin_stop, num_bins, fixed_float_flag);

      // Handshake to report service routine is done.
      WRITE_WORD(drp_handshake,(RTN_FFT_POST << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

#endif //#if (DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN))

#if (DRP_TEST_SW && DRP_BENCH_SW)
    case RTN_FEM_RX:
      WRITE_WORD(drp_handshake,(RTN_FEM_RX << 8 | RTN_BUSY));
      gpio_39_toggle(1);

      band       = (UINT16) (param[0] & 0xFFFF);
      pcb_config = (UINT16) ((param[1] & 0xFFFF) & 7);

      dbbif_setting = drp_generate_dbbif_setting(pcb_config, band);
      drp_rx_tspact_enable(dbbif_setting);
      drp_rx_tspact_enable(dbbif_setting);
      WRITE_WORD(drp_handshake,(RTN_FEM_RX << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_FEM_TA_BURST_START:
      WRITE_WORD(drp_handshake,(RTN_FEM_TA_BURST_START << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      //drp_tx_burst_tpu_start((UINT16)param[0], (UINT16)param[1], &drp_sw_data_calib);
      drp_tx_burst_start_pa((UINT16)param[0], (UINT16)param[1], (UINT16)param[2], &drp_sw_data_calib);
      WRITE_WORD(drp_handshake,(RTN_FEM_TA_BURST_START << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_FEM_TA_BURST:
      WRITE_WORD(drp_handshake,(RTN_FEM_TA_BURST << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      drp_tx_burst_tpu(&drp_sw_data_calib);
      WRITE_WORD(drp_handshake,(RTN_FEM_TA_BURST << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;
#endif //#if (DRP_TEST_SW && DRP_BENCH_SW)

#if ( (DRP_TEST_SW && (DRP_TDL_DFT || DRP_TDL_SW || DRP_BENCH_SW)) )
    case RTN_TX_RX_COMMON_CALIB:
      WRITE_WORD(drp_handshake,(RTN_TX_RX_COMMON_CALIB << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      pcb_config = (UINT16) ((param[0] & 0xFFFF) & 7);
      ret16 = drp_tx_rx_common_calib(pcb_config, &drp_sw_data_calib);
      WRITE_WORD(drp_handshake,(RTN_TX_RX_COMMON_CALIB << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_FLYBACKDELAY_CALIB:
    {
      T_DRP_FLYBACK_DELAY_PARAM flyback_params;
      // temp32 = drp_copy_ref_sw_to_drpsrm(&drp_phe_anayser_dft[0]);
      WRITE_WORD(drp_handshake, (RTN_FLYBACKDELAY_CALIB << 8 | RTN_BUSY));
      flyback_params.rf_freqCentre = (UINT16)(param[0] & 0xFFFF);
      flyback_params.loop_count =  (UINT16)(param[2] & 0xFFFF);
      flyback_params.itter_cnt = (UINT16)(param[3] & 0xFFFF);
      flyback_params.alpha = (UINT16)(param[4] & 0xFFFF);
      flyback_params.rho = (UINT16)(param[5] & 0xFFFF);
      flyback_params.lambda = (UINT16)(param[6] & 0xFFFF);
      flyback_params.ModulationSwtich= (UINT16)((param[6] >> 16) & 0xFFFF);
      flyback_params.ckvdsetting = (UINT16)((param[5]  >> 16) & 0xFFFF);
      flyback_params.sigmadeltaorder = (UINT16)((param[4]  >> 16) & 0xFFFF);
      flyback_params.cf =(UINT16)((param[1]  >> 16) & 0xFFFF);
      flyback_params.ct = (UINT16)(param[1] & 0xFFFF);
      flybackdelayCalibration(&flyback_params);

      WRITE_WORD(drp_handshake, (RTN_FLYBACKDELAY_CALIB << 8 | RTN_BUSY));
      gpio_39_toggle(0);
    }
    break;

#endif //#if ( (DRP_TEST_SW && (DRP_TDL_DFT || DRP_TDL_SW || DRP_BENCH_SW)) )

    case RTN_GET_ADDRESS_RX:
      WRITE_WORD(drp_handshake,(RTN_GET_ADDRESS_RX << 8 | RTN_BUSY));
      gpio_39_toggle(1);
 #if DRP_TDL_DFT
      temp32 = (UINT32)g_Buffer;
 #else
      temp32 = (UINT32)DRP_SRM_RX_BUFFER_ADDR;
#endif
      temp16 = (UINT16)(temp32 & 0x0000FFFF);
      WRITE_WORD(drp_param1L,temp16);
      temp16 = (UINT16)((temp32 >> 16) & 0x0000FFFF);
      WRITE_WORD(drp_param1H,temp16);
      WRITE_WORD(drp_handshake,(RTN_GET_ADDRESS_RX << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

#if DRP_FLYBACK_IBIAS_CALIB

    case RTN_FLYBACK_CALIB:
      WRITE_WORD(drp_handshake,(RTN_FLYBACK_CALIB << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      if(param[1] < 2){
        ret16 = drp_dco_retiming_data_collection((UINT16)param[0], (UINT16)param[1], &drp_sw_data_calib);
      }
      else{
        ret16 = drp_dco_retiming_calibration(&drp_sw_data_calib);
      }
      WRITE_WORD(drp_handshake,(RTN_FLYBACK_CALIB << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_DCO_IBIAS_CALIB:
      WRITE_WORD(drp_handshake,(RTN_DCO_IBIAS_CALIB << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      if(param[1] < 2){
        ret16 = drp_dco_ibias_data_collection((UINT16)param[0], (UINT16)param[1], &drp_sw_data_calib);
      }
      else{
        ret16 = drp_dco_ibias_calibration(&drp_sw_data_calib);
      }
      WRITE_WORD(drp_handshake,(RTN_DCO_IBIAS_CALIB << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

#endif  // #if DRP_FLYBACK_IBIAS_CALIB

    case RTN_WRITE_DRP_MEMORY:
      WRITE_WORD(drp_handshake,(RTN_WRITE_DRP_MEMORY << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      address = param[0];
      data = param[1];
      write_drp_memory(address, data);
      WRITE_WORD(drp_handshake,(RTN_WRITE_DRP_MEMORY << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_READ_DRP_MEMORY:
      WRITE_WORD(drp_handshake,(RTN_READ_DRP_MEMORY << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      address = param[0];
      read_drp_memory(address);
      WRITE_WORD(drp_handshake,(RTN_READ_DRP_MEMORY << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

#if DRP_TDL_GPIO
    case RTN_CLEAR_READ_REGISTER:
      WRITE_WORD(drp_handshake,(RTN_CLEAR_READ_REGISTER << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      clear_read_register();
      WRITE_WORD(drp_handshake,(RTN_CLEAR_READ_REGISTER << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;
#endif

#if (DRP_TEST_SW && !DRP_BURNIN)
    case RTN_DRP_EFUSE_INIT:
      gpio_39_toggle(1);
      WRITE_WORD(drp_handshake,(RTN_DRP_EFUSE_INIT << 8 | RTN_BUSY));
      drp_efuse_init();
      WRITE_WORD(drp_handshake,(RTN_DRP_EFUSE_INIT << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
    break;
#endif //#if (DRP_TEST_SW && !DRP_BURNIN)

#if DRP_TDL_SW_DEBUG
    case RTN_FFT_AVG_DEBUG:
      gpio_39_toggle(1);
      WRITE_WORD(drp_handshake,(RTN_FFT_AVG_DEBUG << 8 | RTN_BUSY));
      results = (float *) param[0];
      fft_avg_debug(results);
      WRITE_WORD(drp_handshake,(RTN_FFT_AVG_DEBUG << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;
#endif

    case RTN_DUMP_DC_OFFSET:
      gpio_39_toggle(1);
      WRITE_WORD(drp_handshake,(RTN_DUMP_DC_OFFSET << 8 | RTN_BUSY));
      fft_avg_debug((float*)&drp_srm_dc_offset);
      WRITE_WORD(drp_handshake,(RTN_DUMP_DC_OFFSET << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_DUMP_MEASURED_TEMP:
      gpio_39_toggle(1);
      WRITE_WORD(drp_handshake,(RTN_DUMP_MEASURED_TEMP << 8 | RTN_BUSY));
      fft_avg_debug((float*)&drp_measured_temp);
      WRITE_WORD(drp_handshake,(RTN_DUMP_MEASURED_TEMP << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

#if (DRP_TEST_SW && !DRP_TDL_DFT)
    case RTN_PROCESS_DC_OFFSET:
      gpio_39_toggle(1);
      WRITE_WORD(drp_handshake,(RTN_PROCESS_DC_OFFSET << 8 | RTN_BUSY));
      process_dc_offset((UINT16)param[0], (UINT16)param[1]);
      WRITE_WORD(drp_handshake,(RTN_PROCESS_DC_OFFSET << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;
#endif //#if (DRP_TEST_SW && !DRP_TDL_DFT)

#if (DRP_TEST_SW && DRP_TDL_DFT)
    case RTN_RX_DFT:
      WRITE_WORD(drp_handshake,(RTN_RX_DFT << 8 | RTN_BUSY));
      gpio_39_toggle(1);
      band          = (UINT16)(param[0] & 0xFFFF);
      channel       = (UINT16)(param[1] & 0xFFFF);
      abe_gain_indx = (UINT16)(param[2] & 0xFFFF);
      afe_gain_indx = (UINT16)((param[2] >> 16) & 0xFFFF);
      tone_indx     = (UINT16)(param[3] & 0xFFFF);
      power_indx    = (UINT16)((param[3] >> 16) & 0xFFFF);
      num_bursts    = (UINT16)(param[4] & 0xFFFF);
      dft_type      = (UINT16)(param[5] & 0xFFFF);
      pcb_config    = (UINT16)((param[6] & 0xFFFF) & 7);

      //for default gain settings
      drp_sw_data = &drp_sw_data_calib;

      // By-pass ABE, AFE gain compensation in Scripts
      WRITE_WORD(drp_srm_api->control.calib_ctl, 0x001C);

      ret16 = drp_rx_dft(band, channel, abe_gain_indx, afe_gain_indx, pcb_config, tone_indx,
                         power_indx, dft_type, num_bursts);

      // Restore the default value in calib_ctl
      WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0000);

      // Handshake to report service routine is done.
      WRITE_WORD(drp_handshake,(RTN_RX_DFT << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
      break;

    case RTN_ADPLL_CAP_BANK_DFT:
      gpio_39_toggle(1);
      WRITE_WORD(drp_handshake, (RTN_ADPLL_CAP_BANK_DFT << 8 | RTN_BUSY));
      test_type = (UINT16)(param[0] & 0xFFFF);
      adpll_dft_test(test_type,(UINT32*) g_Buffer);
      WRITE_WORD(drp_handshake, (RTN_ADPLL_CAP_BANK_DFT << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);
    break;

    case RTN_PHE_ANALYZER_DFT:
      gpio_39_toggle(1);

    /*Load the Phe script code into the memory*/

      temp32 = drp_copy_ref_sw_to_drpsrm(&drp_phe_anayser_dft[0]);

      WRITE_WORD(drp_handshake, (RTN_PHE_ANALYZER_DFT << 8 | RTN_BUSY));
     /*
      * Senario:          0xEC
      * Parameter 0:   RF Frequency (100kHz/bit)
      * Parameter 1:   Loop Count (Number of TX Bursts to average)
      * Parameter 2:   Itteration Count (Number of 512 samples in each burst, typically set to 1)
      * Parameter 3:   0  (not used)
      * Parameter 4:   0  (not used)
      * Parameter 5:   0  (not used)
      */
      phe_measurement(/*UINT16 rf_freq*/((UINT16)(param[0] & 0xFFFF)),/* UINT16 loop_count*/((UINT16)(param[1] & 0xFFFF)),/* UINT16 itter_cnt*/((UINT16)(param[2] & 0xFFFF)));
      WRITE_WORD(drp_handshake, (RTN_PHE_ANALYZER_DFT << 8 | RTN_COMPLETE));
      gpio_39_toggle(0);

#endif //#if (DRP_TEST_SW && DRP_TDL_DFT)

    //code should never come here
    default:
      break;

    }
  scenario_id = 0;
  WRITE_WORD(drp_scenario_id, 0x0000);
  }

  //return;

}

#endif //#if DRP_TEST_SW

#if (DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN))
void drp_tx_burst(UWORD16 rf_freq, UWORD16 burst_length, T_DRP_SW_DATA *sw_data_ptr){

  UINT16 indx,indx1,indx2;
  UINT16 wait_burst;
  UINT16 wait_loop;
  UINT16 tx_burst1[TX_BUFFER_LENGTH] = {0x4f40,0x5746,0x7390,0x83a6,0x8874,0x9c74,0x4ca0,0x9b5b,0xa9d7,0xfff0};
  UINT16 tx_burst2[TX_BUFFER_LENGTH] = {0xee60,0x32ff,0x18aa,0x8278,0x8874,0x0474,0x9e1e,0x446b,0xcbeb,0xfff1};
  UINT16 tx_burst3[TX_BUFFER_LENGTH] = {0x5f28,0x9570,0x518b,0x8806,0x8874,0x3774,0xfeb9,0x8446,0x79af,0xfff1};
  UINT16 tx_burst4[TX_BUFFER_LENGTH] = {0x1e58,0x98cd,0x7093,0x82df,0x8874,0x0374,0x2f09,0xc42b,0x525e,0xfff1};
  UINT16 temp16,data_prev;
  UINT32 temp32;
  UINT16 burst_shift;

#if DRP_TDL_SW
  volatile UINT16 * ptr_gpio;
  #if DRP_TDL_GPIO
  UINT16 loop_condition, state;
  UINT32 *tmp_addr, tmp_data, *mcu_addr;
  #endif

  // GPIO-7 is used for handshake for TX_START
  // To enable GPIO - 7 as an input, Bit 7 of GPIO0_CNTL = 1
  ptr_gpio = (UINT16*)GPIO0_CNTL;
  temp16 = *ptr_gpio;
  temp16 |= (0x0080);
  *ptr_gpio = temp16;

  // Now set address to actual GPIO input register
  ptr_gpio = (UINT16*)GPIO0_LATCH_IN;

  //Default value for GPIO - 7 = 0
  temp16 = *ptr_gpio;
  temp16 &= ~(0x0080);
  *ptr_gpio = temp16;
#endif

  drp_sw_data = sw_data_ptr;

  WRITE_WORD(drp_regs->RF_FREQL, rf_freq);

  // Channel = 880.2 MHz
  // WRITE_WORD(drp_regs->RF_FREQL,0x2262);
  // Channel = 1710.2 MHz
  // WRITE_WORD(drp_regs->RF_FREQL,0x42CE);

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

  //burst duration is now about ~610us (at PPA output)
  wait_burst = 577 + 33;
  wait_loop = 3772 - 53 + 72 - 1;

  for(indx=0;indx < (burst_length/4);indx++){

    for(indx1=0;indx1 < 4;indx1++){

      switch(indx1){
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

      // TX ON
      WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL,0x81);

      //    WAIT_US(250);
      WAIT_US(250 - 66);

#if DRP_TDL_SW
  #if DRP_TDL_GPIO_SIMULATE
      ptr_gpio = (UINT16 *)0xFFFF221C;
  #endif

      //wait for GPIO7 to go low
      while(((*ptr_gpio) & BIT_7));

  #if DRP_TDL_GPIO
      addr_data_indx = 0;

      loop_condition = TRUE;
      while (loop_condition == TRUE)
      {
        state = ((((*ptr_gpio) >> 7) & 1) << 1) | ((((*ptr_gpio) >> 5) & 1));
        switch (state)
        {
          case 0:
          break;

          case 1: //(7:5) = "01"
            read_addr_data();

            //write the registers
            for (indx2 = 0; indx2 < addr_data_indx; indx2++)
            {
              tmp_addr = (UINT32 *) addr_array[indx2];
              mcu_addr = (UINT32 *) conv_addr_drp_to_mcu((UINT32)tmp_addr);
              tmp_data = data_array[indx2];
              *mcu_addr = tmp_data;
            }

            loop_condition = FALSE;
          break;

          case 2: //(7:5) = "10"
            loop_condition = FALSE;
          break;

          case 3: //(7:5) = "11" not possible
            loop_condition = FALSE;
          break;
        }
      }
  #endif //#if DRP_TDL_GPIO

      //wait for GPIO7 to go high
      while(((*ptr_gpio) & BIT_7) == 0);
#endif

      // TX_START high
      READ_WORD(drp_regs->DTX_SEL_CONTL,temp16);
      temp16 = (temp16 & ~0x3000) | 0x3000;
      WRITE_WORD(drp_regs->DTX_SEL_CONTL,temp16);

      READ_WORD(drp_regs->DTX_CONFIGL,temp16);
      temp16 = (temp16 & ~0x0080) | 0x0080;
      WRITE_WORD(drp_regs->DTX_CONFIGL,temp16);

      //*((UINT16 *)0xFFFE9208) = 7;
      //*((UINT16 *)0xFFFEF020) = 1;
      WAIT_US(wait_burst);
      //*((UINT16 *)0xFFFEF020) = 0;

#if DRP_TDL_SW
      //wait for GPIO7 to go low
      while(((*ptr_gpio) & BIT_7));
#endif


      // TX_START low
      READ_WORD(drp_regs->DTX_CONFIGL,temp16);
      temp16 = (temp16 & ~0x0080);
      WRITE_WORD(drp_regs->DTX_CONFIGL,temp16);

      READ_WORD(drp_regs->DTX_SEL_CONTL,temp16);
      temp16 = (temp16 & ~0x3000) | 0x3000;
      WRITE_WORD(drp_regs->DTX_SEL_CONTL,temp16);

      WAIT_US(10 - 6);
      WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL,0x8F);

      WAIT_US(wait_loop);

//#if DRP_TDL_SW
//      indx1 = 4;
//      indx = burst_length/4 + 1;
//#endif

    }
  }
  return;
}

#if (DRP_TEST_SW && DRP_BENCH_SW)
void drp_tx_burst_tpu_start(UWORD16 rf_freq, UWORD16 burst_length, T_DRP_SW_DATA *sw_data_ptr){

  UINT16 indx;
  UINT16 tx_burst1[TX_BUFFER_LENGTH] = {0x4f40,0x5746,0x7390,0x83a6,0x8874,0x9c74,0x4ca0,0x9b5b,0xa9d7,0xfff0};
  UINT16 tx_burst2[TX_BUFFER_LENGTH] = {0xee60,0x32ff,0x18aa,0x8278,0x8874,0x0474,0x9e1e,0x446b,0xcbeb,0xfff1};
  UINT16 tx_burst3[TX_BUFFER_LENGTH] = {0x5f28,0x9570,0x518b,0x8806,0x8874,0x3774,0xfeb9,0x8446,0x79af,0xfff1};
  UINT16 tx_burst4[TX_BUFFER_LENGTH] = {0x1e58,0x98cd,0x7093,0x82df,0x8874,0x0374,0x2f09,0xc42b,0x525e,0xfff1};
  UINT16 temp16,data_prev;
  UINT32 temp32;
  UINT16 burst_shift;

  drp_sw_data = sw_data_ptr;
  g_burst_max = burst_length;
  g_burst_count = 0;

  WRITE_WORD(drp_regs->RF_FREQL, rf_freq);

  burst_shift = 5;
  data_prev = 0;
  for(indx=0;indx < TX_BUFFER_LENGTH;indx++){
    temp32 = data_prev | (tx_burst1[indx] << 16);
  temp32 = temp32 << burst_shift;
  temp16 = (temp32 & 0xFFFF0000) >> 16;
  data_prev = tx_burst1[indx];
    g_tx_burst1[indx] = temp16;
  }

  data_prev = 0;
  for(indx=0;indx < TX_BUFFER_LENGTH;indx++){
    temp32 = data_prev | (tx_burst2[indx] << 16);
  temp32 = temp32 << burst_shift;
  temp16 = (temp32 & 0xFFFF0000) >> 16;
  data_prev = tx_burst2[indx];
    g_tx_burst2[indx] = temp16;
  }

  data_prev = 0;
  for(indx=0;indx < TX_BUFFER_LENGTH;indx++){
    temp32 = data_prev | (tx_burst3[indx] << 16);
  temp32 = temp32 << burst_shift;
  temp16 = (temp32 & 0xFFFF0000) >> 16;
  data_prev = tx_burst3[indx];
    g_tx_burst3[indx] = temp16;
  }

  data_prev = 0;
  for(indx=0;indx < TX_BUFFER_LENGTH;indx++){
    temp32 = data_prev | (tx_burst4[indx] << 16);
  temp32 = temp32 << burst_shift;
  temp16 = (temp32 & 0xFFFF0000) >> 16;
  data_prev = tx_burst4[indx];
    g_tx_burst4[indx] = temp16;
  }

  #if 0
  // Set TPU for next burst
  *TP_Ptr++ = TPU_AT(10);

  MOVE_REG_TSP_TO_RF((UINT16)(RTN_FEM_TA_BURST), (UINT16)SCENARIO_ID_ADD);

  /* start TPU */
  TP_Ptr = (SYS_UWORD16 *) TPU_RAM;
  TP_Enable(1);

  #endif

  // Checking functioning of the TPU

  *TP_Ptr++ = TPU_AT(10);
  MOVE_REG_TSP_TO_RF((UINT16)0x0001, (UINT16)(PARAMETER_3));

  *TP_Ptr++ = TPU_SLEEP;
  /* start TPU */
  TP_Ptr = (SYS_UWORD16 *) TPU_RAM;
  TP_Enable(1);

  drp_param3 = 0x00000000;
  while(g_burst_count < g_burst_max){

    if(drp_param3 != 0){

      drp_param3 = 0;

      // Do TX burst
      drp_tx_burst_tpu(drp_sw_data);

      // Set TPU for next burst
      *TP_Ptr++ = TPU_AT(10);
      MOVE_REG_TSP_TO_RF((UINT16)0x0001, (UINT16)(PARAMETER_3));

      *TP_Ptr++ = TPU_SLEEP;
      /* start TPU */
      TP_Ptr = (SYS_UWORD16 *) TPU_RAM;
      TP_Enable(1);

    }

  }

  return;
}

void drp_tx_burst_start_pa(UWORD16 rf_freq, UWORD16 burst_length, UWORD16 power_lev, T_DRP_SW_DATA *sw_data_ptr){

  UINT16 burst_start, burst_end;
  UINT16 res, band;
  UINT16 TU, TD, RU, RD;

  drp_sw_data = sw_data_ptr;
  g_burst_max = burst_length;
  g_burst_count = 0;

  // Burst start and end in units of qbits
  burst_start = 500;
  burst_end = burst_start + 624;

  WRITE_WORD(drp_regs->RF_FREQL, rf_freq);

  // Find the band of operation
  band = GSM_BAND;
  TU = TU_850;
  TD = TD_850;
  RU = RU_850;
  RD = RD_850;

  if(rf_freq >= EGSM_TX_START){
    band = EGSM_BAND;
    TU = TU_900;
    TD = TD_900;
    RU = RU_900;
    RD = RD_900;
  }


  if(rf_freq >= DCS_TX_START){
    band = DCS_BAND;
    TU = TU_1800;
    TD = TD_1800;
    RU = RU_1800;
    RD = RD_1800;
  }

  if(rf_freq >= PCS_TX_START){
    band = PCS_BAND;
    TU = TU_1900;
    TD = TD_1900;
    RU = RU_1900;
    RD = RD_1900;
  }

  // APC initialization
  drp_apc_programing( );

  // Programming TPU for first frame - Idle

  *TP_Ptr++ = TPU_AT(10);
  MOVE_REG_TSP_TO_RF((UINT16)0x0001, (UINT16)(PARAMETER_3));

  *TP_Ptr++ = TPU_SLEEP;
  /* start TPU */
  TP_Ptr = (SYS_UWORD16 *) TPU_RAM;
  TP_Enable(1);

  drp_param3 = 0x00000000;
  while(g_burst_count < g_burst_max){

    if(drp_param3 != 0){

      drp_param3 = 0;

      // Load next TX burst
      drp_tx_load_burst_pa(drp_sw_data);


      // APC Settings in the Wrapper
      //Programming the RHEA switch
      WR_MEM_8_VOLATILE (MAP_RHEASWITCH_REG,1);

      // Programming the power level
      // res =  PROG_APCLEV(0x0064);
      res =  PROG_APCLEV(power_lev & 0x03ff);

      // Set TPU for next burst

      // t2: Power ON TX
      *TP_Ptr++ = TPU_AT(burst_start + TRF_T2);
      MOVE_REG_TSP_TO_RF((UINT16)0xB081, (UINT16)(SCRIPT_START_ADD));

       // t3: put the TXM in RX mode
      *TP_Ptr++ = TPU_AT(burst_start + TRF_T3);
      *TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_U, RU);

      // t4: enable the APC LDO
      *TP_Ptr++ = TPU_AT(burst_start + TRF_T4);
      *TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_L, LDO_EN);

      // t5: enable the APC module
      *TP_Ptr++ = TPU_AT(burst_start + TRF_T5);
      *TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_L, LDO_EN | APC_EN);

      // t6: enable TX start and enable of Vramp
      *TP_Ptr++ = TPU_AT(burst_start + TRF_T6);
      *TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_L, LDO_EN | APC_EN | TX_START | START_APC);

      // t7: enable TX start and enable of Vramp - Internal mode
      //*TP_Ptr++ = TPU_AT(TRF_T7);
      //*TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_L, LDO_EN | APC_EN | TX_START | START_APC );

      // t7: enable TX start and enable of Vramp
      //*TP_Ptr++ = TPU_AT(TRF_T7+4);
      //*TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_L, LDO_EN | APC_EN | TX_START );

      // t8: enable the TXEN of the TXM
      *TP_Ptr++ = TPU_AT(burst_start + TRF_T8);
      *TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_U, TU);



      // t10: disable APC
      //*TP_Ptr++ = TPU_FAT (t + TRF_T10-6);
      *TP_Ptr++ = TPU_FAT (burst_end + TRF_T10);
      *TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_L, LDO_EN | APC_EN | TX_START );

      // t11: disable PA
      *TP_Ptr++ = TPU_FAT (burst_end + TRF_T11);
      *TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_U, TD);

      // t12: disable TX_START -> end of TX burst LoCosto
      *TP_Ptr++ = TPU_FAT (burst_end + TRF_T12 + 3);
      *TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_L, 0);

      // t13: power off LoCosto
      *TP_Ptr++ = TPU_FAT (burst_end + TRF_T13 + 3);
      MOVE_REG_TSP_TO_RF(0xB08F, (UINT16)(SCRIPT_START_ADD));


      *TP_Ptr++ = TPU_AT(burst_end + 500);
      MOVE_REG_TSP_TO_RF((UINT16)0x0001, (UINT16)(PARAMETER_3));

      *TP_Ptr++ = TPU_SLEEP;
      /* start TPU */
      TP_Ptr = (SYS_UWORD16 *) TPU_RAM;
      TP_Enable(1);

    }

  }

  return;
}


/*****************************************************************************/
/* drp_tx_load_burst_pa(UWORD16 rf_freq, T_DRP_SW_DATA *sw_data_ptr)                              */
/* Function: TPU aided tx burst routine                                                                               */
/*****************************************************************************/
void drp_tx_load_burst_pa(T_DRP_SW_DATA *sw_data_ptr){

  UINT16 indx2;
  UINT16 burst_sel;
  UINT16 tx_burst1[TX_BUFFER_LENGTH] = {0x4A1F,0x19A5,0x5FFA,0xE6F3,0x0E90,0xCE91,0xECEA,0x8677,0xC361,0xFE1A};
  UINT16 tx_burst2[TX_BUFFER_LENGTH] = {0x321F,0x12A4,0x544C,0x78D2,0x0E90,0xCE91,0x98C1,0x9814,0x478B,0xF098};
  UINT16 tx_burst3[TX_BUFFER_LENGTH] = {0x561F,0x98AB,0xF1F2,0x67EF,0x0E90,0xCE91,0x5781,0x097B,0xB981,0xF12E};
  UINT16 tx_burst4[TX_BUFFER_LENGTH] = {0x231F,0xF452,0xEC90,0xA579,0x0E90,0xCE91,0xD953,0x8279,0xD247,0xF760};

  drp_sw_data = sw_data_ptr;

  burst_sel = (g_burst_count + 4) % 4;
  g_burst_count++;

  // Do the current burst
  switch(burst_sel){
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

  return;
}

/********************************************************************/
/* APC Programming                                                                                        */
/********************************************************************/
#define C_APCCTRL2             0x01C8   // BGEN=1, APCOFF=64, APC_LDO_EN=0, MODE=0
#define APCDEL_DOWN     (0)                        // To add this value, the setup delay minimum value: 2   last value : 0
#define APCDEL_UP       (12)                        //To add this value, the setup delay minimum value: 6
#define C_APCDEL1       (((APCDEL_DOWN & 0x1f)<<5)  |(APCDEL_UP & 0x1f) )
#define C_APCDEL2       (((APCDEL_DOWN & 0x3e0)) |((APCDEL_UP>>5) & 0x1f) )

void drp_apc_programing(void){

  UINT16 res;

  // APC Settings in the Wrapper
  //Programming the RHEA switch
  WR_MEM_8_VOLATILE (MAP_RHEASWITCH_REG,1);

  // Progarmming the APC RAM
  res = PROG_APCRAM();

  // Programming the power level
  res =  PROG_APCLEV(0x3ff);

  // Programming the delays
  res =  PROG_APCDEL1(C_APCDEL1);

  // Programming the delays
  res =  PROG_APCDEL2(C_APCDEL2);

  // Programming the delays
  //res =  PROG_APCDEL2(C_APCDEL2);

  // Programming the RAM pointer flush
  res =  PROG_APCCTRL1(0x01);

  // Programming the RAM pointer flush
  res =  PROG_APCCTRL1(0x00);

  // Progarmming  Mode
  res =   PROG_APCCTRL2(C_APCCTRL2);

  WAIT_US(100);

  // May not need this ....
  //res =   PROG_APCCTRL2(0x012A);

  return;
}


// Programming and Checking the RAM VALUES  (RAMP COEFF.'s)

UWORD16 PROG_APCRAM(void){

  int i;
  UWORD16 Result = 0;
  UWORD16 dig_coeff;
  UWORD16 dig_coeff1;
  UINT8 ramp_up[20] = {// Ramp-Up      #0 profile - Power Level 5
        0,0,0,0,0,0,0,0,4,17,37,64,95,127,160,191,218,238,251,255};
  UINT8 ramp_down[20] = {// Ramp-Down    #0 profile
        255,251,238,218,191,160,127,95,64,37,17,4,0,0,0,0,0,0,0,0};

  // Writting into the coeff's into APCRAM register for writting into memory
  for (i =0 ; i < 20; i = i + 1)
  {
    dig_coeff = ((255 - ramp_down[i]) << 8) | ramp_up[i];
    WR_MEM_16_VOLATILE((MAP_DRPWRAP_APC + APCRAM_OFFSET),dig_coeff);
    comp_array[i] = RD_MEM_16_VOLATILE(MAP_DRPWRAP_APC + APCRAM_OFFSET);

    if (comp_array[i] != dig_coeff1)
    {
      Result = 1;
    }
  }
  return (Result);
}

//Programming the Power level
UWORD16 PROG_APCLEV(UWORD16 apc_level){

  UWORD16 Result    = 0;
  UWORD16 comp_reg  = apc_level;
  UWORD16           temp_reg;

  // Reading from the APCLEVEL Register for writting
  temp_reg     = RD_MEM_16_VOLATILE (MAP_DRPWRAP_APC + APCLEV_OFFSET);  // Read contents are stored into temporary register
  temp_reg    &= ~(APC_LEV_MASK_VAL);                                   // Bits that need not be updated are masked
  apc_level    = apc_level | temp_reg;


  // Writting into the register
  WR_MEM_16_VOLATILE((MAP_DRPWRAP_APC + APCLEV_OFFSET),apc_level);

  // Reading from the APCLEVEL register
  apc_level  = RD_MEM_16_VOLATILE(MAP_DRPWRAP_APC + APCLEV_OFFSET);
  apc_level &= APC_LEV_MASK_VAL;
  comp_reg  &= APC_LEV_MASK_VAL;

  // comparing the read contents
  if (apc_level != comp_reg)
  {
    Result = 1;
  }
  return (Result);
}

//Programming the DELAY LSB PART
UWORD16 PROG_APCDEL1(UWORD16 apcdel1_val){

  UWORD16 Result    = 0;
  UWORD16 comp_reg  = apcdel1_val;
  UWORD16             temp_reg;


  // Preserving the REGISTER BITS
  temp_reg     = RD_MEM_16_VOLATILE (MAP_DRPWRAP_APC + APCDEL1_OFFSET); // Read contents are stored into temporary register
  temp_reg    &= ~(APC_DEL_MASK_VAL);                                   // Bits that need not be updated are masked
  apcdel1_val  = apcdel1_val | temp_reg;

  // Writting into the APCDELAY1 Register
  WR_MEM_16_VOLATILE((MAP_DRPWRAP_APC + APCDEL1_OFFSET),apcdel1_val);

  // Reading from the APCDELAY1 register
  apcdel1_val = RD_MEM_16_VOLATILE(MAP_DRPWRAP_APC + APCDEL1_OFFSET);

  // Masking the unwanted bits
  apcdel1_val &= APC_DEL_MASK_VAL;
  comp_reg    &= APC_DEL_MASK_VAL;

  // comparing the read contents
  if (apcdel1_val != comp_reg)
  {
    Result = 1;
  }
  return (Result);
}

//Programming the DELAY MSB PART
UWORD16 PROG_APCDEL2(UWORD16 apcdel2_val)
{
  UWORD16 Result     = 0;
  UWORD16 comp_reg   = apcdel2_val;
  UWORD16             temp_reg;

  // Preserving the REGISTER BITS
  temp_reg     = RD_MEM_16_VOLATILE (MAP_DRPWRAP_APC + APCDEL2_OFFSET); // Read contents are stored into temporary register
  temp_reg    &= ~(APC_DEL_MASK_VAL);                                   // Bits that need not be updated are masked
  apcdel2_val  =  apcdel2_val | temp_reg;


  // Writting into the APCDELAY1 Register
  WR_MEM_16_VOLATILE((MAP_DRPWRAP_APC + APCDEL2_OFFSET),apcdel2_val);

  // Reading from the APCDELAY1 register
  apcdel2_val = RD_MEM_16_VOLATILE(MAP_DRPWRAP_APC + APCDEL2_OFFSET);

  // Masking the unwanted bits
  apcdel2_val &= APC_DEL_MASK_VAL;
  comp_reg    &= APC_DEL_MASK_VAL;

  // comparing the read contents
  if (apcdel2_val != comp_reg)
  {
    Result = 1;
  }
  return (Result);
}

//Setting / Resetting the RAM POINTER
UWORD16 PROG_APCCTRL1(UWORD8 apcptr_val)
{
  UWORD16 Result   = 0;
  UWORD8  comp_reg = apcptr_val;
  UWORD8             temp_reg;

  // Reading from the Ram Pointer value
  temp_reg     =  RD_MEM_8_VOLATILE(MAP_DRPWRAP_APC  + APCCTRL1_OFFSET); // Read contents are stored into temporary register
  temp_reg    &= ~(APC_CTRL1_MASK_VAL);                                      // Bits that need not be updated are masked
  apcptr_val   = apcptr_val | temp_reg;


  // Writting into the APCRAMPTR Register
  WR_MEM_16_VOLATILE((MAP_DRPWRAP_APC + APCCTRL1_OFFSET),apcptr_val);

  // Reading from the APCCTRLL1 register
  apcptr_val = RD_MEM_8_VOLATILE(MAP_DRPWRAP_APC + APCCTRL1_OFFSET);

  // Masking the unwanted bits
  apcptr_val  &= APC_CTRL1_MASK_VAL;
  comp_reg    &= APC_CTRL1_MASK_VAL;

  // comparing the read contents
  if (apcptr_val != comp_reg)
  {
    Result = 1;
  }
  return (Result);
}


// Programming the Bandgap enable, Apcoefficient, APC_LDO_EN, MODE
UWORD16 PROG_APCCTRL2(UWORD16 apcctrlen_val)
{
  UWORD16 Result   = 0;
  UWORD16            temp_reg;
  UWORD16 comp_reg = apcctrlen_val;

  // Reading from register APCCTRL2
  temp_reg       = RD_MEM_16_VOLATILE(MAP_DRPWRAP_APC  + APCCTRL2_OFFSET);
  temp_reg      &= ~(APC_CTRL2_MASK_VAL);                                      // Bits that need not be updated are masked
  apcctrlen_val  = apcctrlen_val | temp_reg;


  // Writting into the APCCONTROL VAULES Register
  WR_MEM_16_VOLATILE((MAP_DRPWRAP_APC + APCCTRL2_OFFSET),apcctrlen_val);

  // Reading from the APCDELAY2 register
  apcctrlen_val = RD_MEM_16_VOLATILE(MAP_DRPWRAP_APC + APCCTRL2_OFFSET);

  // Masking the unwanted bits
  apcctrlen_val  &= APC_CTRL2_MASK_VAL;
  comp_reg       &= APC_CTRL2_MASK_VAL;



  // comparing the read contents
  if (apcctrlen_val != comp_reg)
  {
    Result = 1;
  }
  return (Result);
}


/*****************************************************************************/
/* drp_tx_burst_tpu(UWORD16 rf_freq, T_DRP_SW_DATA *sw_data_ptr)                              */
/* Function: TPU aided tx burst routine                                                                               */
/*****************************************************************************/
void drp_tx_burst_tpu(T_DRP_SW_DATA *sw_data_ptr){

  UINT16 indx2;
  UINT16 temp16;
  UINT16 burst_sel;

  drp_sw_data = sw_data_ptr;

  burst_sel = (g_burst_count + 4) % 4;
  g_burst_count++;

  // Do the current burst
  switch(burst_sel){
    case 0:
           // Load the Tx burst
          for (indx2 = 0;indx2 < TX_BUFFER_LENGTH;indx2++) {
            WRITE_WORD(drp_regs->TX_DATAL,g_tx_burst1[indx2]);
           }
           break;

    case 1:
           // Load the Tx burst
          for (indx2 = 0;indx2 < TX_BUFFER_LENGTH;indx2++) {
            WRITE_WORD(drp_regs->TX_DATAL,g_tx_burst2[indx2]);
           }
           break;

    case 2:
           // Load the Tx burst
          for (indx2 = 0;indx2 < TX_BUFFER_LENGTH;indx2++) {
            WRITE_WORD(drp_regs->TX_DATAL,g_tx_burst3[indx2]);
           }
           break;

    case 3:
           // Load the Tx burst
          for (indx2 = 0;indx2 < TX_BUFFER_LENGTH;indx2++) {
            WRITE_WORD(drp_regs->TX_DATAL,g_tx_burst4[indx2]);
           }
           break;
  }

  // TX ON
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL,0x81);

  //    WAIT_US(250);
  WAIT_US(250 - 66);


  // TX_START high
  READ_WORD(drp_regs->DTX_SEL_CONTL,temp16);
  temp16 = (temp16 & ~0x3000) | 0x3000;
  WRITE_WORD(drp_regs->DTX_SEL_CONTL,temp16);

  READ_WORD(drp_regs->DTX_CONFIGL,temp16);
  temp16 = (temp16 & ~0x0080) | 0x0080;
  WRITE_WORD(drp_regs->DTX_CONFIGL,temp16);

  WAIT_US(577 + 33);


  // TX_START low
  READ_WORD(drp_regs->DTX_CONFIGL,temp16);
  temp16 = (temp16 & ~0x0080);
  WRITE_WORD(drp_regs->DTX_CONFIGL,temp16);

  READ_WORD(drp_regs->DTX_SEL_CONTL,temp16);
  temp16 = (temp16 & ~0x3000);
  WRITE_WORD(drp_regs->DTX_SEL_CONTL,temp16);

  WAIT_US(10 - 6);
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL,0x8F);

  return;
}
#endif //#if (DRP_TEST_SW && DRP_BENCH_SW)

/////////////////////////////////////////////////////////////////////////////////////

void drp_rx_burst(UINT16 band, UINT16 channel, UINT16 abe_gain_indx, UINT16 afe_gain_indx,
                  UINT16 gain_comp_enable, UINT16 pole_num, T_DRP_SW_DATA *sw_data_ptr, UINT16 pcb_config) {
#if 0
  drp_sw_data = sw_data_ptr;

  // bypass the abe and afe gain gain compensation calls in the script
  // diable the mixer clocks during the abe gain measure
  drp_srm_api->control.calib_ctl = 0x001C;

  rx_timeline_new(band, channel, abe_gain_indx, afe_gain_indx, gain_comp_enable, pole_num, pcb_config);

  ref_roc((CMPLX16 *)DRP_SRM_RX_BUFFER_ADDR, 160, 381, (UINT16) 443);
  // ref_roc(CMPLX16 *rxbufptr, UINT16 num_iqsamples, UINT16 accmulation_count, UINT16 start_index) {

  fft_avg(EGSM_BAND,1,7,0,1,0,1,0,10,HAMMING_WINDOW,1, 0);

  //  signed int* fft_avg(UINT16 band, UINT16 channel, UINT16 ABE_gain_step, UINT16 AFE_gain_step,
  //                  UINT16 pole_num, UINT16 roc_setting, UINT16 phase_mm_bin, UINT16 num_avg, UINT16 win_type, UINT16 win_norm_type) {

  // restore the calib_ctl to default value
  drp_srm_api->control.calib_ctl = 0x0000;

  return;
#endif
}

void drp_copy_calib_str_srm(T_DRP_SW_DATA *sw_data_ptr) {

  UINT8 *ptrsrc,*ptrdst;
  UINT16 indx;

  drp_sw_data = sw_data_ptr;

  ptrdst = (UINT8 *) DRP_SRM_SW_DATA_COPY_ADDR;
  ptrsrc = (UINT8 *) drp_sw_data;

  for(indx=0;indx < sizeof(T_DRP_CALIB)+4;indx++)
    *ptrdst++ = *ptrsrc++;

  return;
}

void drp_copy_calib_str_mcu(T_DRP_SW_DATA *sw_data_ptr) {

  UINT8 *ptrsrc,*ptrdst;
  UINT16 indx;

  drp_sw_data = sw_data_ptr;

  ptrsrc = (UINT8 *) DRP_SRM_SW_DATA_COPY_ADDR;
  ptrdst = (UINT8 *) drp_sw_data;

  for(indx=0;indx < sizeof(T_DRP_CALIB)+4;indx++)
    *ptrdst++ = *ptrsrc++;

  //copy the calib structure into DRP SRM
  drp_copy_sw_data_to_drpsrm(drp_sw_data);

  return;
}
#endif //#if (DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN))
/******************************************************************************/
/*  UINT16 drp_generate_dbbif_setting(UINT16 pcb_config, UINT16 band_index)   */
/*  Comments:Computes the dbbif_setting based on the PCB configuration        */
/*  and the band index for the selected channel of operation                  */
/******************************************************************************/

#if ((DRP_TEST_SW && !DRP_BURNIN) || (!DRP_TEST_SW))
UINT16 drp_generate_dbbif_setting(UINT16 pcb_config, UINT16 band_index){

  UINT16 dbbif_setting = 0;

  if(pcb_config == RF_QUADBAND){
    dbbif_setting = band_index;
  }

  if(pcb_config == RF_EU_TRIBAND){
    switch(band_index){

      case EGSM_BAND:
        dbbif_setting = 1;
      break;

      case GSM_BAND:
        dbbif_setting = 1;
      break;

      case DCS_BAND:
        dbbif_setting = 2;
      break;

      case PCS_BAND:
        dbbif_setting = 3;
      break;
    }
  }

  if(pcb_config == RF_EU_DUALBAND){
    switch(band_index){

      case EGSM_BAND:
        dbbif_setting = 1;
      break;

      case GSM_BAND:
        dbbif_setting = 1;
      break;

      case DCS_BAND:
        dbbif_setting = 2;
      break;

      case PCS_BAND:
        dbbif_setting = 2;
      break;
    }
  }

  if(pcb_config == RF_US_TRIBAND){
    dbbif_setting = band_index;
  }

  if(pcb_config == RF_US_DUALBAND){
    switch(band_index){

      case EGSM_BAND:
        dbbif_setting = 1;
      break;

      case GSM_BAND:
        dbbif_setting = 1;
      break;

      case DCS_BAND:
        dbbif_setting = 2;
      break;

      case PCS_BAND:
        dbbif_setting = 2;
      break;
    }
  }

  if(pcb_config == RF_PCS1900_900_DUALBAND){
    switch(band_index){

      case EGSM_BAND:
        dbbif_setting = 1;
      break;

      case GSM_BAND:
        dbbif_setting = 1;
      break;

      case DCS_BAND:
        dbbif_setting = 2;
      break;

      case PCS_BAND:
        dbbif_setting = 2;
      break;
    }
  }

  if(pcb_config == RF_DCS1800_850_DUALBAND){
    dbbif_setting = band_index;
  }

  return dbbif_setting;

}
#endif //#if ((DRP_TEST_SW && !DRP_BURNIN) || (!DRP_TEST_SW))

#if DRP_TEST_SW == 0

#if (L1_FF_MULTIBAND == 0)

UINT16 drp_generate_dbbif_setting_arfcn(UINT16 pcb_config, UINT16 arfcn){

  UINT16 dbbif_setting;
  UINT16 band_index = 0;

  if( (arfcn >= 1) && (arfcn <= 124) )
    band_index =  EGSM_BAND;

  if( ((arfcn >= 975 ) && (arfcn <= 1023)) || (arfcn == 0))
    band_index = EGSM_BAND;

  if( (arfcn >= 128 ) && (arfcn <= 251) )
    band_index = GSM_BAND;

  if( (arfcn >= 811  ) && (arfcn <= 885) )
    band_index = DCS_BAND;


  if( (arfcn >= 512) && (arfcn <= 810) )
    {
    #if (DRP_FW_BUILD==0)
       if((l1_config.std.id == PCS1900) || (l1_config.std.id == DUAL_US))
    #else
    if((*(drp_env_int_blk->modem_var_tbl->l1_config_std_id)==PCS1900) ||
       (*(drp_env_int_blk->modem_var_tbl->l1_config_std_id)==DUAL_US))
    #endif

       band_index = PCS_BAND;
     else
       band_index = DCS_BAND;
    }

  dbbif_setting = drp_generate_dbbif_setting(pcb_config, band_index);

  return dbbif_setting;

}

#else // L1_FF_MULTIBAND = 1 below

UINT16 drp_generate_dbbif_setting_arfcn(UINT16 pcb_config, UINT16 arfcn){

  UINT16 dbbif_setting;
  UINT16 band_index = 0;

  if( ((arfcn >= 0) && (arfcn<=124)) || ((arfcn >= 975) && (arfcn<=1023)) )
  {
    band_index=EGSM_BAND; // PGSM900 or EGSM900
  }
  else if( (arfcn >= 128) && (arfcn<=251) )
  {
    band_index=GSM_BAND; // GSM850
  }
  else if( (arfcn >= 512) && (arfcn<=885) )
  {
    band_index=DCS_BAND; // DCS1800
  }
  else if( (arfcn >= 1024) && (arfcn<=1322) )
  {
    band_index=PCS_BAND; // PCS1900
  }
  else
  {
    // ERROR
  }

  dbbif_setting = drp_generate_dbbif_setting(pcb_config, band_index);

  return dbbif_setting;

}

#endif // #if (L1_FF_MULTIBAND == 0) else

#endif

#if ((DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN)) || (!DRP_TEST_SW))

UINT16 drp_tone_check(UINT16 band, UINT16 channel, UINT16 abe_gain_indx, UINT16 afe_gain_indx, T_DRP_SW_DATA *sw_data_ptr){

  UINT16 abe_step, afe_step;
  UINT16 pole_num, temp;
#if 0
  SINT16 signal[NUM_TONE_SAMPLES+1];
  SINT16 *calc_ptr;
#endif
  UINT16 indx, indx1;
  UINT16 freq_coeff_cnt;
  UINT16 indxpk,temp16;
  SINT32 spectrum[48];
  /* freq_coeff = round(2^15 * cos(2 * pi * k/NUM_PHE_SAMPLES) */
  SINT16 freq_coeff_0_200Khz[] = { 32758, 32729, 32679, 32610, 32522, 32413, 32286, 32138,
                                                          31972, 31786, 31581, 31357, 31114, 30853, 30572, 30274,
                                                          29957, 29622, 29269, 28899, 28511, 28106, 27684, 27246,
                                                          26791, 26320, 25833, 25330, 24812, 24279, 23732, 23170,
                                                          22595, 22006, 21403, 20788, 20160, 19520, 18868, 18205,
                                                          17531, 16846, 16151, 15447, 14733, 14010, 13279, 12540};
  UINT32 *V3reg_ptr, *V0reg_ptr;
  UINT32 dtst_oez_setting, calc_ptr_start_end_setting;
  UINT16 dtst_mul_sel_setting, ckm_tstclk_setting, ckm_clocks_setting;
  SINT16 abe_actual, afe_actual;

  freq_coeff_cnt = 48;
  pole_num = 0;
  temp = 30;

  drp_sw_data = sw_data_ptr;

  // bypass the abe and afe gain gain compensation calls in the script
  // disable FCW computation
  drp_srm_api->control.calib_ctl = 0x000e;

  //get the closest gain settings for ABE and AFE based on the desired gain_step and current temperature
  abe_step = estimate_abe_gain(abe_gain_indx, temp, &abe_actual);
  afe_step = estimate_afe_gain(afe_gain_indx, temp, band, &afe_actual);

  //Time line starts here
  dbb_write_rffreq(band, channel, RX_MODE);

  // Set ABE Gain
  set_abe_gain(abe_step);

  // Set initial AFE Gain
  set_afe_gain(afe_step,band);

  // Set the SCF pole index
  READ_WORD(drp_srm_api->inout.rx.rxon_input, temp16);
  temp16 = (temp16 & ~0x0080) | ((pole_num & 0x1) << 7);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

  //RX_ON script for low band
  //Time line starts here
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0082);

  //this wait is for RX_ON to complete the ABE gain measurement.
  WAIT_US(400);

//  WRITE_WORD(drp_regs->RX_PTR_START_END_ADDRL,0x001F);
//  WRITE_WORD(drp_regs->RX_PTR_START_END_ADDRH,0x0000);

  // Set up the calc buffer for burst capture with DTST MUX set to IQMC output
  // Test Clocks set to 4X Res, enable calc buffer, and set mem_rxstart =1
  READ_WORD(drp_regs->DTST_OEZH, temp16);
  dtst_oez_setting = temp16 << 16;
  READ_WORD(drp_regs->DTST_OEZL, temp16);
  dtst_oez_setting |= temp16;
  WRITE_WORD(drp_regs->DTST_OEZH,0x0000);
  WRITE_WORD(drp_regs->DTST_OEZL,0x0000);

  READ_WORD(drp_regs->DTST_MUX_SELL, dtst_mul_sel_setting);
  WRITE_WORD(drp_regs->DTST_MUX_SELL,0x0914);

  READ_WORD(drp_regs->CKM_TSTCLKL, ckm_tstclk_setting);
  WRITE_WORD(drp_regs->CKM_TSTCLKL,0x0808);

  READ_WORD(drp_regs->CKM_CLOCKSL, ckm_clocks_setting);
  WRITE_BYTE_LOW(drp_regs->CKM_CLOCKSL,0x0136);

  READ_WORD(drp_regs->CALC_PTR_START_END_ADDRH, temp16);
  calc_ptr_start_end_setting = temp16 << 16;
  READ_WORD(drp_regs->CALC_PTR_START_END_ADDRL, temp16);
  calc_ptr_start_end_setting |= temp16;
  WRITE_WORD(drp_regs->CALC_PTR_START_END_ADDRL,0x0280);
  WRITE_WORD(drp_regs->CALC_PTR_START_END_ADDRH,0x00C0);

  // Wait for settling
  WAIT_US(100);

  WRITE_BYTE_LOW(drp_regs->CALCIF_CWL,0x0001);

  // we capture 256 samples at 1.083 MHz = 236.4 us
    WAIT_US(256);
    #if DRP_TEST_SW
    WAIT_US(25);
    #endif

  WRITE_BYTE_LOW(drp_regs->CALCIF_CWL,0x0000);
  WRITE_BYTE_LOW(drp_regs->CKM_CLOCKSL,0x0106);

#if 0

  calc_ptr = (SINT16*) DRP_SRM_CALC_BUFFER_ADDR;

  for(indx=0;indx < NUM_TONE_SAMPLES;indx++){
    signal[indx] = *calc_ptr;
  calc_ptr += 2;
  }

  // zero the N+1 th sample
  *((UINT32*)calc_ptr) = 0x00000000;
  // Read N+1 th sample
  signal[NUM_TONE_SAMPLES] = *calc_ptr;

  spectral_estimate(signal, NUM_TONE_SAMPLES, freq_coeff_cnt, freq_coeff_0_200Khz, spectrum);

#else

  // zero the N+1 th sample
  *((UINT32*)(DRP_SRM_CALC_BUFFER_ADDR + (NUM_TONE_SAMPLES*4))) = 0x00000000;

  for(indx1=0;indx1 < 3;indx1++){

    V3reg_ptr = (UINT32 *)0xffff07C0;
    V0reg_ptr = (UINT32 *)0xffff0700;

    for(indx=0;indx < 16;indx++)
      *V3reg_ptr++ = freq_coeff_0_200Khz[(16*indx1) + indx] >> 6;  // spectal_estimate has coeff >> 7, later 2*coeff is used, so absob this
                                                                                     // mult inot coeff value
    // R4 = 0 for Q path
    * ((UINT32 *) 0xffff0610) = 0x00000000;

    // R6 = 0x2300 start addr of samples
    * ((UINT32 *) 0xffff0618) = 0x00002300;

    // R7 = 256 + 1 Number of samples , +1 is needed since Goertzel loop runs N+1 times
    * ((UINT32 *) 0xffff061C) = 0x00000100 + 1;

    // N + 1 sample needs to be zero, this is already done above

    //Start script 14 for spectral estimation
    WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008E);
    READ_WORD(drp_regs->SCRIPT_STARTL, temp16);
    while((temp16 & 0x0080)){
      READ_WORD(drp_regs->SCRIPT_STARTL, temp16);
    }

    for(indx=0;indx < 16;indx++){
      spectrum[(16*indx1) + indx] = (SINT32)(*V0reg_ptr++);
      //spectrum_scr[(16*indx1) + indx] = (SINT32)(*V0reg_ptr++);
    }

  }
#endif

  // restore the calib_ctl to default value
  drp_srm_api->control.calib_ctl = 0x0000;

  // Idle script
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL,0x008F);

  indxpk=1;
  for(indx=2;indx < freq_coeff_cnt;indx++){
    if(spectrum[indx] > spectrum[indxpk]) indxpk = indx;
  }

//  Reduced the threshold - seen some violations
//  on Callisto board
//  if(spectrum[indxpk] < 500)
  if(spectrum[indxpk] < 500)
    indxpk = 0;

  WRITE_WORD(drp_regs->DTST_OEZH , ((dtst_oez_setting >> 16) & 0xFFFF));
  WRITE_WORD(drp_regs->DTST_OEZL, (dtst_oez_setting & 0xFFFF));
  WRITE_WORD(drp_regs->DTST_MUX_SELL, dtst_mul_sel_setting);
  WRITE_WORD(drp_regs->CKM_TSTCLKL, ckm_tstclk_setting);
  WRITE_BYTE_LOW(drp_regs->CKM_CLOCKSL, ckm_clocks_setting);
  WRITE_WORD(drp_regs->CALC_PTR_START_END_ADDRL, (calc_ptr_start_end_setting & 0xFFFF));
  WRITE_WORD(drp_regs->CALC_PTR_START_END_ADDRH, ((calc_ptr_start_end_setting >> 16) & 0xFFFF));

  return indxpk;
}


void drp_idac_optimize(T_DRP_SW_DATA *sw_data_ptr){

  UINT16 idac0,idac1;
  UINT16 idacstep;
  UINT16 peak0,peakmin;
  UINT16 mem_data/*, silicon_ver*/, idac_min;
  volatile UINT16 delay;

  // Delay of 10 ms for peak to settle
  delay = 10000;
  idac0 = 0x007F;
  idacstep = 0x2;
  peakmin = 0x10;
  //idac_min = 0x0040;

  drp_sw_data = sw_data_ptr;

  // Read Version ID register to determine device type
  //READ_WORD(drp_regs->DRP2_ID_1L, silicon_ver);
  //if (silicon_ver < LOCOSTO_2_0)
  idac_min = 0x0018;

  WRITE_BYTE_HIGH(drp_regs->DCXO_IDACL,0x0100); //bypass efuse
#if 0
  // setup registers for ADC peak detect mode
  WRITE_BYTE_LOW(drp_regs->ANA_DCXOL,0x002F);
  WRITE_BYTE_LOW(drp_regs->DCXO_IDACH,0x0000);
  WRITE_BYTE_HIGH(drp_regs->DCXO_IDACL,0x0100); //bypass efuse
  WRITE_BYTE_LOW(drp_regs->DCXO_CKADCL,0x0001);
#endif

  idac1 = idac0;
  peak0 = 30;

  while((peak0 >= peakmin) && (idac1 > idac_min)){
    WRITE_BYTE_LOW(drp_regs->DCXO_IDACL,idac1);
    WAIT_US(delay);
    //disable IDAC denominator update in AFC script
    drp_srm_api->control.calib_ctl |= 0x0800;
    WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x8D);
    WAIT_US(100);
    drp_srm_api->control.calib_ctl &= ~0x0800;

#if 0
    READ_WORD(drp_regs->RDCXO_ADCL,mem_data);
    peak0 = (mem_data >> 8) & 0x00FF;
#endif

    peak0 = drp_srm_api->inout.afc.output.dcxo_amp;

    idac1 = idac1 - idacstep;
  }

#if 0
  // disable the clocks
  WRITE_BYTE_LOW(drp_regs->DCXO_CKADCL,0x0000);
  WRITE_BYTE_LOW(drp_regs->DCXO_IDACH, 0x02);
  // Clear ADC peak detect
  WRITE_BYTE_LOW(drp_regs->ANA_DCXOL, 0x00);
  // disable the clocks
  WRITE_BYTE_LOW(drp_regs->DCXO_CKADCL,0x0000);
#endif

  return;
}

SINT16 drp_dcxo_calib(UINT16 BAND_INDEX,
                          #if DRP_TEST_SW
                          UINT16 pcb_config,
                          #endif
                          T_DRP_SW_DATA *sw_data_ptr){
  UINT16 mem_data;
  UINT16 indx_max0;
  UINT16 indx_max1;
  UINT16 temp16,rxon_input_saved, calib_ctl_saved;
  UINT32 FCW_OFFSET,FCW;
  UINT16 dbbif_setting;

  drp_sw_data = sw_data_ptr;
  READ_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);

#if ((DRP_TEST_SW && DRP_BENCH_SW) || (!DRP_TEST_SW))
  // Call twice just in case first call happens at frame boundary
  drp_rx_tspact_enable(GSM_BAND);
  drp_rx_tspact_enable(GSM_BAND);
#endif

  // Force a freq offset of 99.405 KHz, at 869.2 MHz channel
  // Offset of 99.405kHz, Offset = 2^24 * 99.405/26e3)
  FCW_OFFSET = 0x0000FA8F;

  if(BAND_INDEX == GSM_BAND){
  // 2^24 * 869.2 * 2/26
  FCW = 0x42DC8DC8;
  }
  else{
    // 2^24 * 925.2 * 2/26
    FCW = 0x472B52B5;
  }

  FCW = FCW - (2*FCW_OFFSET);

  WRITE_WORD(drp_regs->DLO_FCWL, (FCW & 0x0000FFFF));
  WRITE_WORD(drp_regs->DLO_FCWH, ((FCW >> 16) & 0x0000FFFF));

  // load the reference SW in the drp srm
  temp16 = (UINT16) drp_copy_ref_sw_to_drpsrm((unsigned char *)&drp_dcxo_calib_sw[0]);

  //disable gain compensation in the scripts
  READ_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);
  temp16 = rxon_input_saved & (~0x0002);
  temp16 = temp16 | 0x0001;
  // dbbif setting
  #if DRP_TEST_SW
    dbbif_setting = drp_generate_dbbif_setting(pcb_config, BAND_INDEX);
  #else
  dbbif_setting = drp_generate_dbbif_setting(g_pcb_config, BAND_INDEX);
  #endif
  temp16 = (temp16 & (~0x0300)) | ((dbbif_setting & 0x3) << 8);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, temp16);

  // center the mem_xtal DCXO_XTAL(13:0)
  WRITE_WORD(drp_regs->DCXO_XTALL,0x0000);
  WRITE_WORD(drp_srm_api->inout.afc.input.mem_xtal,0x0000);

  set_dcxo_cfa(0x0000);
  //make DCXO IDAC denominator 0
  WRITE_WORD(drp_regs->ANA_RESERVEDL, 0x0000);

  drp_idac_optimize(sw_data_ptr);

  // check the pre-increment frequency
  indx_max0 = drp_tone_check(BAND_INDEX, 1, 8, 1, sw_data_ptr);
  indx_max1 = indx_max0;

  // indx_max = 23 => IF tone is at (22.5 + 1) * 4.23 = 99.405 KHz
  // since if tone moved just right of 22.5 index, peak detect will be at 23
  while(indx_max1 < 23){

    //increment CFA
    READ_WORD(drp_regs->ANA_LDO_DCXOL,mem_data);

    mem_data = 0x0FFC & mem_data;
    if(indx_max1 < 21)
      mem_data += 0x0080;
  else
      mem_data += 0x0010;

    if(mem_data > 0x0FFC)
      break;

    mem_data = mem_data & 0x0FFC;

    set_dcxo_cfa(mem_data);

    WAIT_US(50);
    indx_max1 = drp_tone_check(BAND_INDEX, 1, 8, 1, sw_data_ptr);

  }

  drp_idac_optimize(sw_data_ptr);

  READ_WORD(drp_regs->ANA_LDO_DCXOL,mem_data);
  drp_sw_data->calib.dcxo.cfa_value = mem_data;

  READ_WORD(drp_regs->DCXO_IDACL,mem_data);
  drp_sw_data->calib.dcxo.idac_value = mem_data;

  //WRITE_WORD(drp_srm_api->control.calib_ctl, 0x0000);
  WRITE_WORD(drp_srm_api->control.calib_ctl, calib_ctl_saved);
  WRITE_WORD(drp_srm_api->inout.rx.rxon_input, rxon_input_saved);

  //re-initialize the script pointers
  READ_WORD(drp_regs->SCRIPT_PTR_3L, mem_data);
  WRITE_WORD(mem_data, drp_regs->SCRIPT_PTR_14L);

#if 0
  WRITE_WORD(mem_data, drp_regs->SCRIPT_PTR_5L);
#endif

#if DRP_DELAY_DEBUG
  drp_delay_check( );
#endif

  return 0;

}

// *******************************************************************************
// Set DCXO CFA Value to DRP Register (ana_ldo_dcxo)
// *******************************************************************************
void set_dcxo_cfa(UINT16 cfa) {
  UINT16 temp16 = 0 ;
  UINT16 silicon_ver ;
  //UINT16 ckm_wd_save ;
  //UINT16 rf_freq_save ;
  //UINT16 default_dlo_fcw_settingl, default_dlo_fcw_settingh;

  // Read Version ID register to determine device type
  READ_WORD(drp_regs->DRP2_ID_1L, silicon_ver);
#if 0
  if (silicon_ver >= LOCOSTO_2_0) {
    // Silicon 1.2+
    // 1) Set to low RF Frequency 832.0 MHz (DLO at 1664.0 MHz)
    READ_WORD(drp_regs->RF_FREQL, rf_freq_save);
    WRITE_WORD(drp_regs->RF_FREQL, 0x2080);

    READ_WORD(drp_regs->DLO_FCWL, default_dlo_fcw_settingl);
    READ_WORD(drp_regs->DLO_FCWH, default_dlo_fcw_settingh);

    // 2) Turn on DLO (TX~_ON Script
    WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0081);
    wait_qb(200);

    // 3) DLO Phase Freeze
    READ_WORD(drp_regs->DLO_PHASEL, temp16);
    temp16 |= 0x0040 ;
    WRITE_WORD(drp_regs->DLO_PHASEL, temp16);

    // 4) Force retimed DSP and MCU, non-retimes SYS and TIM Clocks
    READ_WORD(drp_regs->CKM_WDL, ckm_wd_save);
    WRITE_WORD(drp_regs->CKM_WDL, 0x000C);

    // 5) Squeltch SYS, FREF, and Display clocks
    READ_WORD(drp_regs->DLO_CKR_SDL, temp16);
    temp16 |= 0x0001;
    // Make this write to keep CKTIM on, else DBB will not have any DPLL clocks.
    WRITE_WORD(drp_regs->DLO_CKR_SDL, temp16);
    WAIT_US(1);
    temp16 |= 0x0002 ;
    WRITE_WORD(drp_regs->DLO_CKR_SDL, temp16);

    // 6) Update CFA
    //temp16 = ((cfa << 2) | 0x0001) ;
    WRITE_WORD(drp_regs->ANA_LDO_DCXOL, cfa);
    wait_qb(2);

    // 7) Disable Squeltch (step 5)
    READ_WORD(drp_regs->DLO_CKR_SDL, temp16);
    temp16 &= 0xFFFD ;
    WRITE_WORD(drp_regs->DLO_CKR_SDL, temp16);
    temp16 &= 0xFFFE;
    WRITE_WORD(drp_regs->DLO_CKR_SDL, temp16);

    // 8) Restore retime clocks (step 4)
    WRITE_WORD(drp_regs->CKM_WDL, ckm_wd_save);

    // 9) Unfreeze DLO (step 3)
    READ_WORD(drp_regs->DLO_PHASEL, temp16);
    temp16 &= 0xFFBF ;
    WRITE_WORD(drp_regs->DLO_PHASEL, temp16);

    // 10) Idle Script (step 2)
    WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);

    // 1) Restore RF Frequency (step 1)
    WRITE_WORD(drp_regs->RF_FREQL, rf_freq_save);
    WRITE_WORD(drp_regs->DLO_FCWL, default_dlo_fcw_settingl);
    WRITE_WORD(drp_regs->DLO_FCWH, default_dlo_fcw_settingh);

  } else {
#endif
    // Silicon 1.0, 1.1
    READ_WORD(drp_regs->ANA_LDO_DCXOL, temp16);
    temp16 = temp16 & 0xF003;                   // Mask off CFA [11:2]
    temp16 = temp16 | (cfa & 0x0FFC) ;   // Set CFA bits

    WRITE_WORD(drp_regs->ANA_LDO_DCXOL, temp16);
#if 0
  }
#endif
  // Wait for DCXO to stablize
  wait_qb(550);  // ~500us
}
#endif //#if ((DRP_TEST_SW && (!DRP_TDL_DFT && !DRP_BURNIN)) || (!DRP_TEST_SW))

#if 0
void spectrum_check(void){

  SINT16 signal[NUM_TONE_SAMPLES];
  SINT16 *calc_ptr;
  UINT16 indx,freq_coeff_cnt;
  SINT32 spectrum[48];
  /* freq_coeff = round(2^15 * cos(2 * pi * k/NUM_PHE_SAMPLES) */
  SINT16 freq_coeff_0_200Khz[] = { 32758, 32729, 32679, 32610, 32522, 32413, 32286, 32138,
                                                          31972, 31786, 31581, 31357, 31114, 30853, 30572, 30274,
                                                          29957, 29622, 29269, 28899, 28511, 28106, 27684, 27246,
                                                          26791, 26320, 25833, 25330, 24812, 24279, 23732, 23170,
                                                          22595, 22006, 21403, 20788, 20160, 19520, 18868, 18205,
                                                          17531, 16846, 16151, 15447, 14733, 14010, 13279, 12540};
  float fangle,ftemp;
  float fs,f0;

  freq_coeff_cnt = 48;

  fs = 1083.3;
  f0 = 100.0;

  for(indx=0;indx < NUM_TONE_SAMPLES;indx++){
    fangle = (float)indx*f0*2.0*3.14/fs;
    ftemp = 5000.0 * cos(fangle);
  signal[indx] = (SINT16)ftemp;
  }

  spectral_estimate(signal, NUM_TONE_SAMPLES, freq_coeff_cnt, freq_coeff_0_200Khz, spectrum);

  return;
}

void drp_fft_check(void){

  UINT16 indx,temp16;
  SINT16 *rx_buffer;
  float angle, step;
  float ftemp1,ftemp2;

  float* WinCoef = g_Buffer + 1*FFT_BUF_SIZE;
  float* FFT_IQ = g_Buffer + 2*FFT_BUF_SIZE;
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
  PhaseMismatch[1] = 0;

  rx_buffer = (SINT16 *)DRP_SRM_RX_BUFFER_ADDR;
  angle = PI/10.0;
  step = PI/256.0;
  for(indx=0;indx < 128;indx++){
    ftemp1 = (15000.0 * cos(angle));
    ftemp2 = (15000.0 * sin(angle));
    rx_buffer[2*indx] = (SINT16)ftemp1;
    rx_buffer[2*indx+1] = (SINT16)ftemp2;
    angle += step;
  }

  // Setup for FFT - Creates window Coefficient
  real_WINDOW(HAMMING_WINDOW, FFT_BUF_SIZE, 0, 0x0, WinCoef);

  // Zero out FFT Averages for I, Q, I+jQ Channels, and Phase Mismatch
  compute_running_avg((tFloatingPointComplex*) FFT_Pwr_I, (tFloatingPointComplex*) FFT_Pwr_Avg_I, 6*FFT_BUF_SIZE + 2, AVG_RESET);

  for (indx = 0; indx < FFT_BUF_SIZE; indx++)
  {
     g_rx_data_buffer_decimated[2*indx+1] = rx_buffer[2*indx];     // Q
     g_rx_data_buffer_decimated[2*indx]     = rx_buffer[2*indx + 1]; // I
  }

    // Calculates FFT (Real + Imaginary) for I+jQ Channel
   temp16 = compute_fft((tIQsample *)(&g_rx_data_buffer_decimated[0]), (tFloatingPointComplex*) FFT_IQ, FFT_BUF_SIZE, 3, (tFloatingPointReal*) WinCoef);

   return;

}
#endif //#if 0

#if ((DRP_TEST_SW && DRP_BENCH_SW) || (!DRP_TEST_SW))
void drp_rx_tspact_enable(UINT16 band){

  volatile UINT16 FrameCount;

/*
  FrameCount = IQ_FrameCount;
  while(FrameCount == IQ_FrameCount){
    WAIT_US(10);
  }
*/

  // TO ADD
  // wait here for frame counter increment, this way you will be
  // sure that the TPU page is modified when TPU has just started
  // processing the other page. Thus there is not danger of the TPU
  // swapping pages in the middle of this function execution

  *TP_Ptr++ = TPU_AT(10);

  switch(band){
    case EGSM_BAND:
      *TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_U, RU_900);
    break;

    case GSM_BAND:
      *TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_U, RU_850);
    break;

    case DCS_BAND:
      *TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_U, RU_1800);
    break;

    case PCS_BAND:
      *TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_U, RU_1900);
    break;

  }

  *TP_Ptr++ = TPU_SLEEP;
  /* start TPU */
  TP_Ptr = (SYS_UWORD16 *) TPU_RAM;
  TP_Enable(1);

  // TO ADD
  // wait here for frame counter increment, this way you will be
  // sure that the TPU has swapped the page
/*
  FrameCount = IQ_FrameCount;
  while(FrameCount == IQ_FrameCount){
    WAIT_US(10);
  }
*/

  // delay to ensure settings are in effect
  //WAIT_US(500);
  WAIT_US(7500);

  return;

}
#endif //#if ((DRP_TEST_SW && DRP_BENCH_SW) || (!DRP_TEST_SW))

#if DRP_DEBUG
void drp_tspact_check(void){

  volatile UINT16 tspchk_flag;

/*
  TP_Reset(1); // reset TPU and TSP

  // GSM 1.5 : TPU clock enable is in TPU
  //---------------------------------------
  TPU_ClkEnable(1);         // TPU CLOCK ON

  TP_Reset(0);

  TP_Ptr = (UWORD16 *) TPU_RAM;
*/

  tspchk_flag = 0;
  while(1){

  if(tspchk_flag == 0){
         asm(" nop");
    asm(" nop");
  }

  if(tspchk_flag == 1){
    *TP_Ptr++ = TPU_AT(10);
         *TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_U, RU_900);
         *TP_Ptr++ = TPU_SLEEP;

          /* start TPU */
          TP_Ptr = (UWORD16 *) TPU_RAM;
          TP_Enable(1);
       }

  if(tspchk_flag == 2){
    *TP_Ptr++ = TPU_AT(10);
         *TP_Ptr++ = TPU_MOVE(REG_SPI_ACT_U, RU_1800);
         *TP_Ptr++ = TPU_SLEEP;

          /* start TPU */
          TP_Ptr = (UWORD16 *) TPU_RAM;
          TP_Enable(1);
       }

  }

  return;

}

#endif //#if DRP_DEBUG

#if (DRP_TEST_SW && DRP_BENCH_SW)

void TP_Enable(UWORD16 on)
{
   if(on)
   {
     * ((volatile UWORD16 *) TPU_CTRL) |= TPU_CTRL_T_ENBL;

     // Some time shall be wait before leaving the function to ensure that bit has been taken
     // in account by the TPU. A while loop such as in function TP_reset can't be used as the
     // ARM can be interrupted within this loop and in that case the pulse will be missed (CQ20781).
     // The bit is updated in the worst case 24 cycles of 13MHz later it as been written by the MCU.
     // 24 ticks of 13MHz = 1.84us. Lets take 3us to keep some margin.
     WAIT_US(3); // wait 3us
   }
   else
   {
     * ((volatile UWORD16 *) TPU_CTRL) &= ~TPU_CTRL_T_ENBL;
     // Some time shall be wait before leaving the function to ensure that bit has been taken
     // in account by the TPU. A while loop such as in function TP_reset can't be used as the
     // ARM can be interrupted within this loop and in that case the pulse will be missed (CQ20781).
     // The bit is updated in the worst case 24 cycles of 13MHz later it as been written by the MCU.
     // 24 ticks of 13MHz = 1.84us. Lets take 3us to keep some margin.
     WAIT_US(3); // wait 3us
   }
}

/*--------------------------------------------------------------*/
/*  TPU_ClkEnable :                                             */
/*--------------------------------------------------------------*/
/* Parameters : on/off(1/0)                                     */
/* Return : none                                                */
/* Functionality :  Enable the TPU clock                        */
/*--------------------------------------------------------------*/

void TPU_ClkEnable(UWORD16 on)
{
  if (on)
  {
    * ((volatile UWORD16 *) TPU_CTRL) |= TPU_CTRL_CLK_EN;
    // WA for read/modify/write access problem with REG_TPU_CTRL present on Ulysse/Samson/Calypso
    while (!((*(volatile UWORD16 *) TPU_CTRL) &  TPU_CTRL_CLK_EN));
  }
  else
  {
    * ((volatile UWORD16 *) TPU_CTRL) &= ~TPU_CTRL_CLK_EN;
    // WA for read/modify/write access problem with REG_TPU_CTRL present on Ulysse/Samson/Calypso
    while (((*(volatile UWORD16 *) TPU_CTRL) &  TPU_CTRL_CLK_EN));
  }
}

/*--------------------------------------------------------------*/
/*  TP_Reset :                                                                                           */
/*--------------------------------------------------------------*/

void TP_Reset(UWORD16 on)
{
   if (on) {
      * ((volatile UWORD16 *) TPU_CTRL) |= (TPU_CTRL_RESET | TSP_CTRL_RESET);
      while (!((*(volatile UWORD16 *) TPU_CTRL) &  (TPU_CTRL_RESET | TSP_CTRL_RESET)));
   }
   else {
      * ((volatile UWORD16 *) TPU_CTRL) &= ~(TPU_CTRL_RESET | TSP_CTRL_RESET);
      while (((*(volatile UWORD16 *) TPU_CTRL) &  (TPU_CTRL_RESET | TSP_CTRL_RESET)));
   }
}

#if DRP_TDL_GPIO
/**************************************************************************************/
/* This function reads 56 nibbles on GPIO(3:0) for trigerring any scenario (used in TDL)*/
/**************************************************************************************/
void drp_read_gpio(void){

  UINT8 input_data_array[7*8];
  UINT8 temp_char;
  UINT8 *data_ptr;
  UINT16 *ptr_gpio;
  UINT16 indx;

#if DRP_TDL_GPIO_SIMULATE
  ptr_gpio = (UINT16 *)0xFFFF221C;
#else
  ptr_gpio = (UINT16*)GPIO0_LATCH_IN;
#endif

  indx = 0;

  if((*ptr_gpio) & BIT_5){

    while(indx < (7*8)){

      while(((*ptr_gpio) & BIT_4) == 0){
        asm(" nop");
      }

      input_data_array[indx] = (*ptr_gpio) & 0x000F;
      indx++;

      while((*ptr_gpio) & BIT_4){
        asm(" nop");
      }

    }

    data_ptr = (UINT8 *) (&drp_scenario_id);
    for(indx=0;indx < (7*8);indx=indx+2){
      temp_char = input_data_array[indx] & 0x0f;
      temp_char |= ((input_data_array[indx+1] << 4) & 0xf0);
      *data_ptr++ = temp_char;
    }

  while((*ptr_gpio) & BIT_5){
    asm(" nop");
  }

  }

  return;

}
#endif //#if DRP_TDL_GPIO

#endif //#if (DRP_TEST_SW && DRP_BENCH_SW)

#if DRP_DELAY_DEBUG
void drp_delay_check(void){

  volatile UINT16 delay;
  volatile UINT16 temp16;

  temp16 = 0;
  delay = 1000;
  while(1){
    // Start idle script
    WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
    WAIT_US(delay);
    WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
    WAIT_US(delay);

    temp16++;
    temp16++;

  }

  return;
}
#endif //#if DRP_DELAY_DEBUG

#if DRP_TEST_SW
/**************************************************************************************/
/* This function produces pulses on GPIO_39 each 10 ARM clocks wide.                  */
/**************************************************************************************/
void pulse_gpio39(UINT16 num)
{
  UINT16 indx, temp16;
  volatile UINT16 *ptr_gpio;

  // GPIO-39 is used for handshake.
  // To enable GPIO - 39 as an output, Bit 7 of GPIO2_CNTL = 0
  ptr_gpio = (UINT16*)GPIO2_CNTL;
  temp16 = *ptr_gpio;
  temp16 &= ~(0x00A0);
  *ptr_gpio = temp16;

  // Now set address to actual GPIO output register
  ptr_gpio = (UINT16*)GPIO2_LATCH_OUT;

  //Default value for GPIO - 39 = 0
  temp16 = *ptr_gpio;
  temp16 &= ~(0x0080);
  *ptr_gpio = temp16;

  for (indx = 0; indx < num; indx++)
  {
    *ptr_gpio |= 0x0080;
    wait_clocks(10);
    *ptr_gpio &= ~(0x0080);
    wait_clocks(10);
  }
  return;
}

/**************************************************************************************/
/* This is wait function. Waits for num ARM clock cycles.                             */
/**************************************************************************************/
void wait_clocks(UINT16 num)
{
  UINT16 indx;

  for (indx=0; indx < num; indx++)
  {
    asm("  nop");
  }
}

/**************************************************************************************/
/* This function toggles the state of GPIO_39 for TDL debug.                          */
/**************************************************************************************/
void gpio_39_toggle(UINT16 state)
{
  UINT16 temp16;
  volatile UINT16 *ptr_gpio;

  ptr_gpio = (UINT16*)GPIO2_LATCH_OUT;

  temp16 = *ptr_gpio;

  if (state & 1)
    temp16 |= 0x0080;
  else
    temp16 &= (~0x00A0);

  *ptr_gpio = temp16;

  return;
}

/**************************************************************************************/
/* This function writes the data into corresponding MCU address.                      */
/**************************************************************************************/
void write_drp_memory(UINT32 address, UINT32 data)
{
  UINT32 *drp_addr;

  drp_addr = (UINT32 *)conv_addr_drp_to_mcu(address);
  *drp_addr = data;

  return;
}

/**************************************************************************************/
/* This function converts DRP address to MCU address.                                 */
/**************************************************************************************/
UINT32 conv_addr_drp_to_mcu(UINT32 address)
{
  UINT32 drp_addr;

  if(address & DRP_EXTERNAL_MEMORY_OCP_ADD){
    drp_addr = (UINT32)(address + DRP_EXTERNAL_MEMORY_ADD - DRP_EXTERNAL_MEMORY_OCP_ADD);
  }
  else{
    drp_addr = (UINT32)(address + DRP_REGS_BASE_ADD - DRP_REGS_BASE_OCP_ADD);
  }
  return(drp_addr);
}

#endif //#if DRP_TEST_SW

#if DRP_TDL_GPIO
/**************************************************************************************/
/* This function reads address and data during a TX burst for any DRP register        */
/* initialization before transmitting a burst.                                        */
/**************************************************************************************/
void read_addr_data(void)
{
  UINT8  nibble;
  UINT32 temp_data=0;
  UINT16 addr_data_flag;
  UINT16 *ptr_gpio;
  UINT16 indx;
  UINT16 loop_condition, state;

#if DRP_TDL_GPIO_SIMULATE
  ptr_gpio = (UINT16 *)0xFFFF221C;
#else
  ptr_gpio = (UINT16*)GPIO0_LATCH_IN;
#endif

  indx = 0;

  loop_condition = TRUE;
  addr_data_flag = 0;
  while(loop_condition == TRUE)
  {
    state = ((*ptr_gpio) >> 4) & 3;
    switch (state)
    {
      case 0:
        loop_condition = FALSE;
      break;

      case 1:
        loop_condition = FALSE;
      break;

      case 2:
      break;

      case 3:
        nibble = ((*ptr_gpio) & 0x000F);
        //nibble = *(drp_temp_addr) & 0x000F;
        temp_data |= (nibble << (4*(indx & 7)));
        indx++;

        if ((indx & 7) == 0)
        {
          if (addr_data_flag == 0)
          {
            addr_array[addr_data_indx] = temp_data;
          }
          else
          {
            data_array[addr_data_indx] = temp_data;
              addr_data_indx++;
            if (addr_data_indx >= MAX_ADDR_DATA)
              addr_data_indx = 0;
          }

          temp_data = 0;
        } //if

        addr_data_flag = (indx >> 3) & 1;

        while((*ptr_gpio) & BIT_4) {
          asm(" nop");
        }

      break;
    }
  }
}
#endif //#if DRP_TDL_GPIO

#if DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW || DRP_TDL_DFT)
/**************************************************************************************/
/* This function reads the contents of address and puts it on DTST pins 15:0          */
/**************************************************************************************/
void read_drp_memory(UINT32 address)
{
  UINT32 *drp_addr;
  UINT32 mem_data;

  drp_addr = (UINT32 *)conv_addr_drp_to_mcu((UINT32)address);

  mem_data = *drp_addr;

  WRITE_WORD(drp_param1L, (mem_data & 0xFFFF));
  mem_data = (mem_data >> 16) & 0xFFFF;
  WRITE_WORD(drp_param1H, mem_data);

  return;
}
#endif //#if DRP_TEST_SW && (DRP_BENCH_SW || DRP_TDL_SW)

#if DRP_TDL_GPIO
/**************************************************************************************/
/* This function clears the DTST registers (used after scenario 0xE8)                 */
/**************************************************************************************/
void clear_read_register(void)
{
  UINT16 mem_data;

  mem_data = dtst_dmux_const_saved & 0xFFFF;
  WRITE_WORD(drp_regs->DTST_DMUX_CONSTL, mem_data);

  mem_data = (dtst_dmux_const_saved >> 16) & 0xFFFF;
  WRITE_WORD(drp_regs->DTST_DMUX_CONSTH, mem_data);

  WRITE_WORD(drp_regs->DTST_OEZL, 0xFFFF);
  WRITE_WORD(drp_regs->DTST_OEZH, 0xFFFF);
  WRITE_WORD(drp_regs->DTST_MUX_SELL, 0x0000);

  return;
}

#endif //#if DRP_TDL_GPIO

/**************************************************************************************/
/* This function initializes the LDO trim values from the EFUSE registers.            */
/**************************************************************************************/
void drp_efuse_init(void) {
  UINT32  efuse0, efuse1;
  UINT16  temp, temp2, index, temp3, ppa_pcs_4, ppa_dcs_4;

  #define TA_GAIN_I_INDEX  29
  #define TA_GAIN_Q_INDEX  30
  #define MIXER_HB_I_INDEX 17
  #define MIXER_HB_Q_INDEX 18

  READ_WORD(drp_regs->DRP2_ID_1L, temp);
  switch (temp) {

    case 0x0003:

#if (DRP_BURNIN == 0)
                 //Ibias Code (decimal) Table
                 //         RX          TX
                 //-------------------------
                 //EGSM     60          60
                 // GSM     60          60
                 // DCS     60          60
                 // PCS     60          60
                 //-------------------------
                 WRITE_WORD_AT_PTR(&drp_srm_data->tables.rx_ibias[0], 0x0F0F);
                 WRITE_WORD_AT_PTR(&drp_srm_data->tables.rx_ibias[1], 0x0F0F);

                 WRITE_WORD_AT_PTR(&drp_srm_data->tables.tx_ibias[0], 0x0F0F);
                 WRITE_WORD_AT_PTR(&drp_srm_data->tables.tx_ibias[1], 0x0F0F);

                 drp_srm_data->tables.ana_dcu_pcu_table[0] = 0x0327;
                 drp_srm_data->tables.ana_dcu_pcu_table[1] = 0x0327;

                 drp_srm_data->tables.dco_clk_dcu_pcu_table[0] = 0x0000;
                 drp_srm_data->tables.dco_clk_dcu_pcu_table[1] = 0x0000;

                 drp_srm_data->tables.dcxo_ib_table[0] = 0x0100;
                 drp_srm_data->tables.dcxo_ib_table[1] = 0x0100;
                 drp_srm_data->tables.dcxo_ib_table[2] = 0x0000;
                 drp_srm_data->tables.dcxo_ib_table[3] = 0x0100;
#endif
                 break;

    //case 0x0001:  // Silicon 1.0
    //              WRITE_WORD(drp_srm_data->rx.anadcupcu, 0x0303);
    //              WRITE_WORD(drp_regs->ANA_LDO_AL, 0x004E);
    //              WRITE_WORD(drp_srm_data->rx.dco[2], 0xF1E9);
    //              WRITE_WORD(drp_srm_data->rx.dco[3], 0xF1E9);
    //              break;
    default:
                  break;
  }

  //****************************************
  // b0 - FCW bypass for Tx
  // b1 - FCW bypass for RX
  // b2 - Bypass ABE gain comp call
  // b3 - Bypass AFE gain comp call
  // b4 - mixer disable during ABE gain measure
  // b5 - ibias setting bypass
  // b6 - flyback setting bypass
  // b7 - use CVKD2 for TX_ON DCOIF
  // b8 - AFE always ON
  // b9 - Use EFUSE from DRP memory locations
  //****************************************
  temp2 = drp_srm_api->control.calib_ctl;

  if((temp2 & 0x0200) == 0x0000)
  {
  // Read lowest word of EFUSE and detect if valid
  READ_WORD(drp_regs->EFUSE0L, temp);
  }
  else
  {
    // Parameter
    READ_WORD(drp_regs->MEM_2300L, temp);
  }

  efuse0 = (UINT32)temp ;
  if((efuse0 & 0x00000001) == 0x00000001)
  {
    if((temp2 & 0x0200) == 0x0000)
    {
      // EFUSE is valid, read full EFUSE
    READ_WORD(drp_regs->EFUSE0H, temp);
    efuse0 = efuse0 | ((UINT32)temp << 16);

    READ_WORD(drp_regs->EFUSE1L, temp);
    efuse1 = (UINT32)temp ;
    READ_WORD(drp_regs->EFUSE1H, temp);
    efuse1 = efuse1 | ((UINT32)temp << 16);
    }
    else
    {
      // Register 0 is valid, read full Register
      READ_WORD(drp_regs->MEM_2300H, temp);
      efuse0 = efuse0 | ((UINT32)temp << 16);

      READ_WORD(drp_regs->MEM_2304L, temp);
      efuse1 = (UINT32)temp ;
      READ_WORD(drp_regs->MEM_2304H, temp);
      efuse1 = efuse1 | ((UINT32)temp << 16);
    }
#if DRP_FW_BUILD
    drp_sw_data->calib.drp_sw_data_ver_efuse.version = *((UINT16 *)drp_fw_certificate.drp_refsw_ver_mem);
#endif
    drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[0] = efuse0 & 0xFFFF;
    drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[1] = (efuse0 >> 16) & 0xFFFF;
    drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[2] = efuse1 & 0xFFFF;
    drp_sw_data->calib.drp_sw_data_ver_efuse.efuse[3] = (efuse1 >> 16) & 0xFFFF;

    if((efuse1 & 0x0000C000) == 0x00000000)
    {
      //Original Decode

      //Bit[0]    EFUSE programmed bit
      //Bit[6:1]  HW CTL_BGAP
      //Bit[14:7] HW DCXO IDAC (for power-on only)
    // Set ANA_REFSYS
    temp2 = drp_srm_api->control.calib_ctl;
    if ((temp2 & 0x0200) == 0x0200)
    {
      READ_WORD(drp_regs->SRM_CWL, temp2);
      temp2 |= 0x1000;
      WRITE_WORD(drp_regs->SRM_CWL, temp2);

      temp = (efuse0 & 0x007E) << 5;
      READ_WORD(drp_regs->ANA_BGAPL, temp2);
      temp2 = (temp2 & 0xF07F) | temp;
      WRITE_WORD(drp_regs->ANA_BGAPL, temp2);

#if 0
      READ_WORD(drp_regs->DCXO_IDACL, temp2);
      temp2 = ((efuse0 >> 7) & 0x00FF) | 0x0100;
      WRITE_WORD(drp_regs->DCXO_IDACL, temp2);
#endif
    }

    efuse0 = efuse0 >> 14 ;
    READ_WORD(drp_regs->ANA_REFSYSL, temp);
    temp = (temp & 0xFFF9) | (UINT16)(efuse0 & 0x0006) ;
    WRITE_WORD(drp_regs->ANA_REFSYSL, temp);

    // Set ANA_LDO_A
    efuse0 = efuse0 >> 2 ;
    READ_WORD(drp_regs->ANA_LDO_AL, temp);
    temp = (temp & 0xFF81) | (UINT16)(efuse0 & 0x007E) ;
    WRITE_WORD(drp_regs->ANA_LDO_AL, temp);

    // Set ANA_LDO_OSC
    efuse0 = efuse0 >> 6 ;
    READ_WORD(drp_regs->ANA_LDO_OSCL, temp);
    temp = (temp & 0xFF81) | (UINT16)(efuse0 & 0x007E) ;
    WRITE_WORD(drp_regs->ANA_LDO_OSCL, temp);

    // Set ANA_LDO_RFIO
    efuse0 = (efuse0 >> 6) | (efuse1 << 4);
    READ_WORD(drp_regs->ANA_LDO_RFIOL, temp);
    temp = (temp & 0xFF81) | (UINT16)(efuse0 & 0x007E) ;
    WRITE_WORD(drp_regs->ANA_LDO_RFIOL, temp);

    // Set ANA_BGAP
    READ_WORD(drp_regs->ANA_BGAPL, temp);
    temp = (temp & 0x0FFF) | (UINT16)((efuse1 << 9)& 0xF000) ;
    WRITE_WORD(drp_regs->ANA_BGAPL, temp);

      temp = (efuse1 >> 23) & 7;
      //Bit[57:55] EF_LNA_DEQ[2:0] (EFUSE1[25:23])
      drp_efuse_update_deq(temp);
  }

    if((efuse1 & 0x0000C000) == 0x00004000)
    {
      // New EFUSE Decoding

      //Bit[0]    EFUSE programmed bit
      //Bit[6:1]  HW CTL_BGAP
      //Bit[14:7] HW DCXO IDAC (for power-on use only, replaced by calibrated value)
      temp2 = drp_srm_api->control.calib_ctl;
      if ((temp2 & 0x0200) == 0x0200)
      {
        READ_WORD(drp_regs->SRM_CWL, temp2);
        temp2 |= 0x1000;
        WRITE_WORD(drp_regs->SRM_CWL, temp2);

        temp = (efuse0 & 0x007E) << 5;
        READ_WORD(drp_regs->ANA_BGAPL, temp2);
        temp2 = (temp2 & 0xF07F) | temp;
        WRITE_WORD(drp_regs->ANA_BGAPL, temp2);
      }

      efuse0 = efuse0 >> 15;

      //Bit[15] EF_TA BIAS (EFUSE0[15])
      //(ANA_TA_XB_I/Q[15:8] through parameter DRP_TA_GAIN_VALUE)
      if((efuse0 & 0x0000001) == 0x00000000)
      {
        temp2 = 0x0049;
      }
      else
      {
        temp2 = 0x0043;
      }
      READ_WORD(drp_srm_data->tables.arx_default_table[TA_GAIN_I_INDEX], temp);
      temp = (temp & 0x00FF) | temp2 << 8 ;
      WRITE_WORD(drp_srm_data->tables.arx_default_table[TA_GAIN_I_INDEX], temp);
      WRITE_WORD(drp_srm_data->tables.arx_default_table[TA_GAIN_Q_INDEX], temp);
      efuse0 = efuse0 >> 1;

      //Bit[19:16] EF_LDO_A (EFUSE0[19:16])
      READ_WORD(drp_regs->ANA_LDO_AL, temp);
      temp = (temp & 0xFF81) | (((UINT16)(efuse0 & 0x000F) + 15) << 1);
      WRITE_WORD(drp_regs->ANA_LDO_AL, temp);
      efuse0 = efuse0 >> 4;

      //Bit[25:20] EF_LDO_OSC (EFUSE0[25:20])
      READ_WORD(drp_regs->ANA_LDO_OSCL, temp);
      temp = (temp & 0xFF81) | ((UINT16)(efuse0 & 0x003F) << 1);
      WRITE_WORD(drp_regs->ANA_LDO_OSCL, temp);
      efuse0 = efuse0 >> 6;

      //Bit[29:26] EF_LDO_RFIO (EFUSE0[29:26])
      READ_WORD(drp_regs->ANA_LDO_RFIOL, temp);
      temp = (temp & 0xFF81) | (((UINT16)(efuse0 & 0x000F) + 15) << 1);
      WRITE_WORD(drp_regs->ANA_LDO_RFIOL, temp);
      efuse0 = efuse0 >> 4;

      //Bit[31:30] EF_FCU_DAC (EFUSE0[31:30])
      index = (UINT16)(efuse0 & 0x00000003) ;
      switch(index)
      {
        case 0x0000 :
          temp  = 0x0000;    // +0000, +0000
          temp2 = 0x0000;
          temp3 = 0x0000;
          break;

        case 0x0001 :
          temp  = 0x1388;    // -5000, +5000
          temp2 = 0xEC78;
          temp3 = 0x0001;
          break;

        case 0x0002 :
          temp  = 0xEC78;    // +5000, -5000
          temp2 = 0x1388;
          temp3 = 0x0001;
          break;

        case 0x0003 :
          temp  = 0x1770;    // +6000, -6000
          temp2 = 0xE890;
          temp3 = 0x0001;
          break;

        default:
          break;
      }
      drp_srm_data->force_fcuoffset = (temp << 16);
      drp_srm_data->force_fcuoffset |= temp2;

      efuse0 = efuse0 >> 2;

      //Bit[34:32] EF_DCO_IBIAS[2:0] (EFUSE1[2:0])
      temp =  (UINT16)(efuse1 & 0x0007) * 4;
      temp2= (temp << 8) | temp;
      drp_srm_data->tables.rx_ibias[0] = 0x0E0E + temp2;
      drp_srm_data->tables.rx_ibias[1] = 0x1818 + temp2;
      drp_srm_data->tables.tx_ibias[0] = 0x0E0E + temp2;
      drp_srm_data->tables.tx_ibias[1] = 0x100E + temp2;
      efuse1 = efuse1 >> 3;

      // Bit[38:35] EF_CTL_BGAP2[15:12] (EFUSE1[15:12])
      //(ANA_BGAPL[15:12] (EFUSE1[6:3])
      READ_WORD(drp_regs->ANA_BGAPL, temp);
      temp = (temp & 0x0FFF) | (UINT16)((efuse1 << 12) & 0x0000F000) ;
      WRITE_WORD(drp_regs->ANA_BGAPL, temp);
      efuse1 = efuse1 >> 4;

      // Bit[42:39] EF_PPA_LB (DTX_ACW) (EFUSE1[10:7])
      temp = (UINT16) efuse1 & 0x0000000F;
      if (temp > 0)
      {
        temp2 = 325 + temp * 25;
      }
      else
      {
        temp2 = 0x200;
      }
      WRITE_WORD(drp_srm_data->tables.tx_ppa_table[EGSM_BAND], temp2);   // GSM900
      WRITE_WORD(drp_srm_data->tables.tx_ppa_table[GSM_BAND], temp2);    // GSM850
      WRITE_WORD(drp_sw_data->calib.ppa.magnitude[EGSM_BAND], temp2);    // GSM900
      WRITE_WORD(drp_sw_data->calib.ppa.magnitude[GSM_BAND], temp2);     // GSM850
      WRITE_WORD(drp_sw_data->calib.ppa.slope[GSM_BAND], drp_srm_data->tables.tx_ppa_slope[GSM_BAND]);
      WRITE_WORD(drp_sw_data->calib.ppa.slope[EGSM_BAND], drp_srm_data->tables.tx_ppa_slope[EGSM_BAND]);
      efuse1 = efuse1 >> 4;

      //Bit[44:43] EF_GAIN_TGT[1:0] (EFUSE1[12:11])
      //index = (UINT16)(efuse1 & 0x00000003);
      //switch (index)
      //{
      //  case 0x00:
      //    drp_srm_data->tables.tar_afe_gain_table[0][1] = DRP_GAIN_36_5_DB + DRP_GAIN_2_DB;
      //  break;
      //
      //  case 0x01:
      //    //dont do anything
      //  break;
      //
      //  case 0x02:
      //    drp_srm_data->tables.tar_afe_gain_table[0][1] = DRP_GAIN_36_5_DB - DRP_GAIN_1_DB;
      //  break;
      //
      //  case 0x03:
      //    drp_srm_data->tables.tar_afe_gain_table[0][1] = DRP_GAIN_36_5_DB - DRP_GAIN_2_DB;
      //  break;
      //}
      ppa_pcs_4 = efuse1 & 1;
      ppa_dcs_4 = (efuse1 >> 1) & 1;
      efuse1 = efuse1 >> 2;


      //Bit[45] EF_DITHER_EN (EFUSE[13])
      if (!(efuse1 & 0x00000001))
      {
        drp_srm_data->tx_dither_en_flyback_cal_en &= 0x00FF;
        drp_srm_data->tx_dither_en_flyback_cal_en |= 0x0100;
      }
      //else
      //{
      //  drp_srm_data->tx_dither_en_flyback_cal_en &= 0x00FF;
      //}
      efuse1 = efuse1 >> 1;


      //Bit[47:46] EF_VER01_VALID[1:0] (EFUSE1[15:14])
      efuse1 = efuse1 >> 2;

      //Bit[48] EF_SD_ORDER (EFUSE1[16])
      if (efuse1 & 0x00000001)
      {
        //DCO SD order is 2 for both LB and HB
        drp_srm_data->tx_sdorder_cw = 0x0202;
      }
      //else
      //{
      //  drp_srm_data->tx_sdorder_cw = 0x0102;
      //}
      efuse1 = efuse1 >> 1;

      //Bit[49] EF_LNACAL_HB (EFUSE1[17])
      //if((efuse1 & 0x00000001) == 0x00000001)
      //{
      //
      //}
      efuse1 = efuse1 >> 1;

      //Bit[50] EF_KDCO_AVG (EFUSE1[18])
      if (efuse1 & 0x00000001)
      {
        drp_srm_data->tx_kdcoadapten_demcw &= 0x00FF;
        drp_srm_data->tx_kdcoadapten_demcw |= 0x0100;
      }
      //else
      //{
      //  drp_srm_data->tx_kdcoadapten_demcw &= 0x00FF;
      //}
      efuse1 = efuse1 >> 1;

      // Bit[54:51] EF_PPA_PCS (DTX_ACW) (EFUSE1[11] & EFUSE1[22:19])
      temp = ((UINT16) efuse1 & 0x0000000F) + (ppa_pcs_4 << 4);
      if (temp > 0)
      {
        temp2 = 275 + temp * 25;
      }
      else
      {
        temp2 = 0x200;
      }
      WRITE_WORD(drp_srm_data->tables.tx_ppa_table[PCS_BAND], temp2);    // PCS
      WRITE_WORD(drp_sw_data->calib.ppa.magnitude[PCS_BAND], temp2);     // PCS
      WRITE_WORD(drp_sw_data->calib.ppa.slope[PCS_BAND], drp_srm_data->tables.tx_ppa_slope[PCS_BAND]);
      efuse1 = efuse1 >> 4;


      //Bit[57:55] EF_LNA_DEQ[2:0] (EFUSE1[25:23])
      drp_efuse_update_deq(efuse1 & 7);
      efuse1 = efuse1 >> 3;

      //Bit[58] EF_DEM (EFUSE1[26])
      if (!(efuse1 & 0x00000001))
      {
        drp_srm_data->tx_kdcoadapten_demcw &= 0xFF00;
        drp_srm_data->tx_kdcoadapten_demcw |= 0x0007;
      }
      //else
      //{
      //  drp_srm_data->tx_kdcoadapten_demcw &= 0xFF00;
      //}
      efuse1 = efuse1 >> 1;

      //Bit[59] EF_FLYBACK_CAL (EFUSE1[27])
      if (efuse1 & 0x00000001)
      {
        //disable flyback calibration
        drp_srm_data->tx_dither_en_flyback_cal_en &= 0xFF00;
        drp_srm_data->tx_dither_en_flyback_cal_en |= 0x0001;
      }
      //else
      //{
      //  drp_srm_data->tx_dither_en_flyback_cal_en &= 0xFF00;
      //}
      efuse1 = efuse1 >> 1;

      // Bit[63:60] EF_PPA_DCS (DTX_ACW) (EFUSE1[12] & EFUSE1[31:28])
      temp = ((UINT16) efuse1 & 0x0000000F) + (ppa_dcs_4 << 4);
      if (temp > 0)
      {
        temp2 = 275 + temp * 25;
      }
      else
      {
        temp2 = 0x200;
      }
      WRITE_WORD(drp_srm_data->tables.tx_ppa_table[DCS_BAND], temp2);    // DCS
      WRITE_WORD(drp_sw_data->calib.ppa.magnitude[DCS_BAND], temp2);     // DCS
      WRITE_WORD(drp_sw_data->calib.ppa.slope[DCS_BAND], drp_srm_data->tables.tx_ppa_slope[DCS_BAND]);
      efuse1 = efuse1 >> 4;
    }
  }
}

#if (DRP_TEST_SW && DRP_BURNIN)
void drp2_burnin_test()
{
  UINT16 indx, temp16;

#if DRP_BURNIN_SIMULATE
  //set up to see DTS02
  *((UINT8 *)0xFFFE9265) = 0x02;
  *((UINT8 *)0xFFFE9214) = 0x07;
  *((UINT8 *)0xFFFEF13E) = 0x01;
#endif

  //####################################################################################################
  //vobs/drp2/sim/input_data/rf_bist/rfb_rx_loop_back/rfb_ppa_rxlb_burnin.mfi_cnfg.txt@@/main/drp2_dev/1
  //#-------------------------------------------------------------------------------
  //# Enable the mem_srm_arx_clrz_control & enable srm_arx_clk
  //#-------------------------------------------------------------------------------
  WRITE_BYTE_HIGH(drp_regs->SRM_CWL, 0x0300);

  pulse_kbc1(2);
  //load_drp_scripts(&scripts_data[0]);
  // load the reference SW in the drp srm
  drp_copy_ref_sw_to_drpsrm(&drp_ref_sw[0]);
  pulse_kbc1(3);

  //load the LDO trim values
  drp_efuse_init();

#if DRP_BURNIN_HTOL //MAKSED FOR HTOL LOOKAHEAD VERSION PREVENTS PUMPING UP OF VOLTAGE FROM 1.4 TO 1.6
  //# Over-write the standard phone mode initialization with
  //# the burnin mode specific settings

  //Trim = EFUSE_trim + (((V2/V1) - 1) * 55000)/(tcoeff * 625) + EFUSE_trim *
  //                                                             (((V2/V1) - 1) / tcoeff)
  //Where V2          = Final Voltage
  //      V1          = 1.4 V  (Trimmed LDO voltage)
  //      tcoeff      = 1.1992
  //      EFUSE_trim  = EFUSE LDO trim value

  //example1:
  //    V2 = 1.6 V
  //    (V2/V1) - 1  = 0.1428571
  //    (((V2/V1) - 1) * 55000)/(tcoeff * 625) = 10.483   (~6552/625)
  //    (((V2/V1) - 1) / tcoeff) = 0.119127               (  ~75/625)

  //    so:  Trim = EFUSE_trim + (((6552 + (EFUSE_trim * 75))/625)

  //example2:
  //    V2 = 1.5023V
  //    (V2/V1) - 1  = 0.0730714
  //    (((V2/V1) - 1) * 55000)/(tcoeff * 625) = 5.362    (~3351/625)
  //    (((V2/V1) - 1) / tcoeff) = 0.0609335                (~38/625)
  //
  //    so:  Trim = EFUSE_trim + (((3351 + (EFUSE_trim * 38))/625)

  //LDO-osc needs to be 1.5023V, rest of the LDOs at 1.6V
  temp16 = (drp_regs->ANA_LDO_OSCL);
  temp16 = (temp16 >> 1) & 0x3F;
  temp16 = temp16 + ((temp16 * 38) + 3351 + 313)/625;
  drp_regs->ANA_LDO_OSCL = (temp16 << 1);

  temp16 = (drp_regs->ANA_LDO_AL);
  temp16 = (temp16 >> 1) & 0x3F;
  temp16 = temp16 + ((temp16 * 75) + 6552 + 313)/625;
  drp_regs->ANA_LDO_AL = (temp16 << 1);

  temp16 = (drp_regs->ANA_LDO_RFIOL);
  temp16 = (temp16 >> 1) & 0x3F;
  temp16 = temp16 + ((temp16 * 75) + 6552 + 313)/625;
  drp_regs->ANA_LDO_RFIOL = (temp16 << 1);
  #endif

  //Ibias code of 14
  WRITE_BYTE_LOW(drp_regs->ANA_DCOH, 0x001C);

  //#Enable TX HB-LB divider
  //
  //#-----------------------------------------------------
  //# RX_DCO1_TABLE
  //# Write the initial ANA_DCO register setting for RX
  //# ABE gain measurement.
  //#   Note: LB: Disable LB Divider (bit 4)
  //#         HB: Disable HB Buffer  (bit 5)
  //# *** Warning - Disabling HB Divider will break DLO tuning
  //#-----------------------------------------------------
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_RX_DCO1_TABLE + 0x0) = 0xF1CD;     // # RX-EGSM
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_RX_DCO1_TABLE + 0x2) = 0x71CD;     // # RX-GSM
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_RX_DCO1_TABLE + 0x4) = 0xF18B;     // # RX-DCS
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_RX_DCO1_TABLE + 0x6) = 0xF18B;     // # RX-PCS

  //#-----------------------------------------------------
  //# RX_DCO2_TABLE
  //# Write the final ANA_DCO register setting for RX
  //#-----------------------------------------------------
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_RX_DCO2_TABLE + 0x0) = 0xF1DD;     // # RX-EGSM
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_RX_DCO2_TABLE + 0x2) = 0x71DD;     // # RX-GSM
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_RX_DCO2_TABLE + 0x4) = 0xF1AB;     // # RX-DCS
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_RX_DCO2_TABLE + 0x6) = 0xF1AB;     // # RX-PCS

  //Open up the SCF poles
  *(UINT32*)(DRP_REGS_BASE_ADD + DRP_RX_SCF_TABLE + 0x0) = 0x00000000;      // # RX-EGSM
  *(UINT32*)(DRP_REGS_BASE_ADD + DRP_RX_SCF_TABLE + 0x4) = 0x00000000;      // # RX-GSM
  *(UINT32*)(DRP_REGS_BASE_ADD + DRP_RX_SCF_TABLE + 0x8) = 0x00000000;      // # RX-DCS
  *(UINT32*)(DRP_REGS_BASE_ADD + DRP_RX_SCF_TABLE + 0xC) = 0x00000000;      // # RX-PCS

  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_RX_IF_OFFSET) = 0x0000;

  //
  // Lutpre output amplitude = 2^15/2^(LUTPRE_CW[28:24])
  //                         = 2^15/2^8 = 128
  //
  // Lutpre output peak-to-peak = 256 digital counts
  //                 => Vrms_in = [256/(2*sqrt(2))] * 0.9/32768
  //                            = 2.486mV
  //
  //               power_in     = 10*log10[Vrms_in^2]
  //                            = -52.1 dBVrms
  //                            = -39.1 dBm
  //
  // Total Attenuation in feedback path = SCF_LOSSES + FREQ_ATTENUATION
  //                                          |               |
  //                                          |               |__ ~2dB
  //                                          |
  //                                          |__ ~12.0 dB (with RX dividers enabled)
  //                                              ~ 4.3 dB (with RX dividers disabled)
  //
  //
  //                                    = 12 dB (with RX dividers enabled) + ~2dB
  //                                    ~= 14dB
  //
  //      Note: SCF_LOSSES ~= 4.3dB if RX dividers are disabled.
  //            RX dividers are enabled by setting ANA_DCO(4) for LB and ANA_DCO(5) for HB.
  //
  // ABE Gain                   = 23 dB - GAIN_INACCURACY (~3dB)
  //
  //
  // Signal power at CSF output = -39.1 - 14 + 23 - 3
  //                            = -33.1 dBm
  //                            = -63.1 dBW => Vrms_out = 4.949mV
  //
  //                => CSF output peak to peak ~510 digital counts.
  //
  // Max signal magnitude from FFT script for a digital signal with peak to peak
  // amplitude is 510 digital counts is ~3850 and maximum noise magnitude is ~200.
  // Hence choose a signal threshold of 2000 and noise threshold of 500.
  //
  // Lutpre output frequency = (LUTPRE_CW[19:16]/512) * ckadcd64
  //                         = (4/512) * (925.2e6 * 2)/(4*64)
  //                         = 56.47 kHz
  //
  // Note on pass/fail criteria:
  //   Result is pass if all of the foll. conditions are met.
  //     1. Maximum magnitude of FFT is between FFT_SIG_START_INDEX and FFT_SIG_STOP_INDEX.
  //     2. Maximum magniture of FFT is greater than FFT_SIG_THRESH.
  //     3. Maximum magnitude of FFT outside FFT_SIG_START_INDEX and FFT_SIG_STOP_INDEX
  //        should be less than FFT_NOISE_THRESH.
  //
  //     If any of the above conditions are not met, result is fail.
  //
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SIG_THRESH + 0x00) = 0x07D0;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_NOISE_THRESH + 0x00) = 0x01F4;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SIG_START_INDEX + 0x00) = 0x04;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SIG_STOP_INDEX + 0x00) = 0x05;


  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_DSP_DIVIDER_START + 0x00) = 0x0010;
  //update the DSP_DIVIDER_CONST values for Locosto (max DSP clock frequency = 104MHz)
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_DSP_DIVIDER_CONST + 0x00) = 0x40FB;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_DSP_DIVIDER_CONST + 0x02) = 0x450B;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_DSP_DIVIDER_CONST + 0x04) = 0x491B;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_DSP_DIVIDER_CONST + 0x06) = 0x4D2B;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_DSP_DIVIDER_CONST + 0x08) = 0x513B;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_DSP_DIVIDER_CONST + 0x0A) = 0x6000;

  //Disable regenerated clocks for burnin test
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_DRP_CTL_RETIME + 0x00) = 0x0000;

  //initialize all the vector registers to zeros
  for (indx = 0; indx < 64; indx++)
    *(UINT32*)(DRP_REGS_BASE_ADD + 0x0700 + (indx*4)) = 0x00000000;

  //dont decimate RX data
  WRITE_WORD(drp_regs->DRXIF_CWL, 0x0000);

  //####################################################################################################
  //vobs/drp2/sim/input_data/rf_bist/rfb_rx_loop_back/rfb_ppa_rxlb_burnin.mfi_stim.txt@@/main/drp2_dev/1
  //#-----------------------------------------------------------------------
  //# Kick off REG-ON
  //#-----------------------------------------------------------------------
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0080);

  //#-----------------------------------------------------------------------
  //# Longest settling time.
  //#-----------------------------------------------------------------------
#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(2400000));
#else
  wait_ARM_cycles(9600);
#endif
  pulse_kbc1(4);

  //#----------------------------------------------------------------------
  //# Set DCO Ibias value to 14.
  //# For 1.2 it is 24:18 <= "0000_0111"
  //#----------------------------------------------------------------------
  //WRITE_BYTE_LOW(drp_regs->ANA_DCOH,0x001C);

  //#-----------------------------------------------------------------------
  //# the clock used for the data coming into the SRM is ckm_tstmuxclk_0
  //# the data coming into the SRM is selected to be the data coming from
  //# the dtst.
  //#-----------------------------------------------------------------------
  //WRITE_WORD(drp_regs->DRXIF_CWL, 0x0003);      //# 0000 0000 0000 0000 0000 0000 0000 0011

  //#-----------------------------------------------------------------------
  //# Extending the SRM RX-buffer to ensure that the desired number of samples are stored.
  //#-----------------------------------------------------------------------
  WRITE_WORD(drp_regs->RX_PTR_START_END_ADDRL, 0x0150);
  WRITE_WORD(drp_regs->RX_PTR_START_END_ADDRH, 0x0000);

  WRITE_WORD(drp_regs->TX_PTR_START_END_ADDRL, 0x0152);
  WRITE_WORD(drp_regs->TX_PTR_START_END_ADDRH, 0x0151);

  //clear any error flags if any
  WRITE_BYTE_LOW(drp_regs->DLO_SEQ_CQML, 0x0000);
  READ_WORD(drp_regs->DLO_SEQ_SRSTL, temp16);
  temp16 = temp16 | 0x8000;
  WRITE_WORD(drp_regs->DLO_SEQ_SRSTL, temp16);
#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(2000));
#else
  wait_ARM_cycles(8);
#endif
  temp16 = temp16 & 0x7FFF;
  WRITE_WORD(drp_regs->DLO_SEQ_SRSTL, temp16);

  // Generated by round(512 * cos(2*pi*f/fs))
  // where fs = 26e6/24;
  //        f = [30 35 40 45 54 58 70 80 100 150 200 250 300 350 400 450]*1e3;
  //
  //# Write Cos coefficients
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x00) = 0x01F8;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x02) = 0x01F5;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x04) = 0x01F2;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x06) = 0x01EF;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x08) = 0x01E7;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x0A) = 0x01E3;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x0C) = 0x01D6;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x0E) = 0x01CA;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x10) = 0x01AC;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x12) = 0x014A;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x14) = 0x00CC;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x16) = 0x003E;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x18) = 0xFFAA;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x1A) = 0xFF1D;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x1C) = 0xFEA3;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x1E) = 0xFE47;

  // Generated by round(256 * sin(2*pi*f/fs))
  // where fs = 26e6/24;
  //        f = [30 35 40 45 54 58 70 80 100 150 200 250 300 350 400 450]*1e3;
  //
  //# Write Sin coefficients
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x00) = 0x002C;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x02) = 0x0034;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x04) = 0x003B;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x06) = 0x0042;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x08) = 0x004F;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x0A) = 0x0055;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x0C) = 0x0065;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x0E) = 0x0073;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x10) = 0x008C;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x12) = 0x00C4;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x14) = 0x00EB;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x16) = 0x00FE;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x18) = 0x00FC;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x1A) = 0x00E5;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x1C) = 0x00BB;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x1E) = 0x0082;

  //# Write FFT_MAG_INDEX values
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x00) = 0x0000;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x02) = 0x0001;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x04) = 0x0002;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x06) = 0x0003;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x08) = 0x0004;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x0A) = 0x0005;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x0C) = 0x0006;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x0E) = 0x0007;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x10) = 0x0008;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x12) = 0x0009;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x14) = 0x000A;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x16) = 0x000B;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x18) = 0x000C;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x1A) = 0x000D;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x1C) = 0x000E;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x1E) = 0x000F;

  //EGSM channel 1: 925.2MHz
  drp2_burnin_txrx_test(0x2424);

  //EGSM channel 1: 925.2MHz
  drp2_burnin_txrx_test(0x2424);

  //clear any error flags if any
  WRITE_BYTE_LOW(drp_regs->DLO_SEQ_CQML, 0x0000);
  READ_WORD(drp_regs->DLO_SEQ_SRSTL, temp16);
  temp16 = temp16 | 0x8000;
  WRITE_WORD(drp_regs->DLO_SEQ_SRSTL, temp16);
#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(2000));
#else
  wait_ARM_cycles(8);
#endif
  temp16 = temp16 & 0x7FFF;
  WRITE_WORD(drp_regs->DLO_SEQ_SRSTL, temp16);

  // Generated by round(512 * cos(2*pi*f/fs))
  // where fs = 26e6/24;
  //        f = [30 35 40 45 58 62 80 100 120 150 200 250 300 350 400 450]*1e3;
  //
  //# Write Cos coefficients
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x00) = 0x01F8;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x02) = 0x01F5;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x04) = 0x01F2;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x06) = 0x01EF;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x08) = 0x01E3;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x0A) = 0x01DF;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x0C) = 0x01CA;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x0E) = 0x01AC;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x10) = 0x0189;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x12) = 0x014A;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x14) = 0x00CC;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x16) = 0x003E;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x18) = 0xFFAA;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x1A) = 0xFF1D;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x1C) = 0xFEA3;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF + 0x1E) = 0xFE47;

  // Generated by round(256 * sin(2*pi*f/fs))
  // where fs = 26e6/24;
  //        f = [30 35 40 45 58 62 80 100 120 150 200 250 300 350 400 450]*1e3;
  //
  //# Write Sin coefficients
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x00) = 0x002C;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x02) = 0x0034;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x04) = 0x003B;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x06) = 0x0042;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x08) = 0x0055;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x0A) = 0x005A;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x0C) = 0x0073;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x0E) = 0x008C;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x10) = 0x00A4;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x12) = 0x00C4;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x14) = 0x00EB;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x16) = 0x00FE;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x18) = 0x00FC;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x1A) = 0x00E5;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x1C) = 0x00BB;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF + 0x1E) = 0x0082;

  //# Write FFT_MAG_INDEX values
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x00) = 0x0000;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x02) = 0x0001;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x04) = 0x0002;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x06) = 0x0003;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x08) = 0x0004;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x0A) = 0x0005;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x0C) = 0x0006;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x0E) = 0x0007;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x10) = 0x0008;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x12) = 0x0009;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x14) = 0x000A;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x16) = 0x000B;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x18) = 0x000C;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x1A) = 0x000D;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x1C) = 0x000E;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x1E) = 0x000F;

  //else PPA will not work
  WRITE_WORD(drp_regs->ANA_PPAL, 0x0014);

  //PCS channel 1: 1989.8MHz
  drp2_burnin_txrx_test(0x4DBA);

  //PCS channel 1: 1989.8MHz
  drp2_burnin_txrx_test(0x4DBA);

  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0087);

#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(40000));
#else
  wait_ARM_cycles(160);
#endif

  pulse_kbc1(9);

  WRITE_WORD(drp_regs->OCP_SCRATCHL, 0x0000);

  //test_done
  //####################################################################################################
}

/********************************************************************************/
/* Generates pulses on KBC1.                                                    */
/********************************************************************************/
void burnin_status_display(UINT16 err_flag, UINT16 duration)
{

  if (err_flag == PASS)
    *((UINT16 *)CONF_LOCOSTO_DEBUG) = 0x0001;
  else
    *((UINT16 *)CONF_LOCOSTO_DEBUG) = 0x0000;

#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(duration*2000));
#else
  wait_ARM_cycles(duration*8);
#endif
  *((UINT16 *)CONF_LOCOSTO_DEBUG) = 0x0000;

  return;
}

#if 0
/******************************************************************************/
/* Loading DRP scripts into DRP memory                                        */
/******************************************************************************/
void load_drp_scripts(UINT16 *scripts_data)
{
  volatile UINT16 i, j, num_segments, offset, length;
  UINT16 *scripts_ptr;

  num_segments = scripts_data[1];

  scripts_ptr = &scripts_data[2];

  for (i = 0; i < num_segments; i++)
  {
    offset = *(UINT16 *)scripts_ptr++;
    length = *(UINT16 *)scripts_ptr++;
    for (j = 0; j < length; j++)
    {
      *(volatile UINT16*)(DRP_REGS_BASE_ADD + offset) = *(UINT16 *)scripts_ptr++;
      offset = offset + 2;
    }
  }
  return;
}
  #endif

/**************************************************************************************/
/* This function produces pulses on KBC1 each 10 ARM clocks wide.                     */
/**************************************************************************************/
void pulse_kbc1(UINT16 num)
{
  UINT16 indx;

  for (indx = 0; indx < num; indx++)
  {
    *((UINT16 *)CONF_LOCOSTO_DEBUG) = 0x0001;
#if DRP_BURNIN_SIMULATE
 wait_ARM_cycles(2); //each iteration takes 5 ARM clocks
#else
    wait_ARM_cycles(8);
#endif
    *((UINT16 *)CONF_LOCOSTO_DEBUG) = 0x0000;
  #if DRP_BURNIN_SIMULATE
 wait_ARM_cycles(2);
  #else
    wait_ARM_cycles(8);
  #endif
  }

  *((UINT16 *)CONF_LOCOSTO_DEBUG) = 0x0000;

  return;
}

/********************************************************************************/
/* Actual burnin RF test are performed here                                     */
/* This performs 1 test on RX path and 2 tests on TX path                       */
/* TX tests include (i) PPA threshold test                                      */
/*                  (ii) DLO clocks quality monitor testing                     */
/*                                                                              */
/* RX tests include (i) Signal looped back from LUTPRE and captured at CSF      */
/*                      output is checked for correct frequency, signal         */
/*                      amplitude.                                              */
/********************************************************************************/
void drp2_burnin_txrx_test(UINT16 rf_freq)
{
  UINT16 mem_data, err_flag = PASS, fft_errflag_i = PASS, fft_errflag_q = PASS;
  UINT16 dlo_errflag = PASS, ppa_active_errflag = PASS, ppa_inactive_errflag = PASS;
  UINT16 ana_ppa_setting, temp16, i;

#if 0
  //calibrate wait
  temp16 = 10;
  for (i=0; i<50;i++)
  {
    *((UINT16 *)CONF_LOCOSTO_DEBUG) = 0x0001;
    wait_ARM_cycles(temp16); //each iteration takes 5 ARM clocks
    *((UINT16 *)CONF_LOCOSTO_DEBUG) = 0x0000;
    wait_ARM_cycles(temp16); //each iteration takes 5 ARM clocks
  }
#endif

#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(10);
#else
  wait_ARM_cycles(80);
#endif


  //Disable ZIF block
  WRITE_WORD(drp_regs->ZIF_CWL, 0x0000);          //# 0000 0000 0000 0000 0000 0000 0000 0000
  WRITE_BYTE_LOW(drp_regs->ZIF_CWH, 0x0000);      //# 0000 0000 0000 0000 0000 0000 0000 0000

  WRITE_WORD(drp_regs->LUTPRE_CWL, 0x0804);       //# Generate sine-wave
  WRITE_WORD(drp_regs->LUTPRE_CWH, 0x0804);       //# Generate sine-wave

  /*****************************************************************************/
  /* PPA Threshold test                                                        */
  /*****************************************************************************/
  //# Set Amplitute to maximum as per burnin requirements
  //# i.e DTX_ACW(25:10) => Set all the bits to '1'i.e 0x3fffc

  WRITE_BYTE_HIGH(drp_regs->DTX_ACWL, 0xFC00);
  WRITE_WORD(drp_regs->DTX_ACWH, 0x03FF);

  //Select the mem_tx_start over dbbdrptxstart.
  READ_WORD(drp_regs->DTX_SEL_CONTL, temp16);
  temp16 = temp16 | 0x3000;
  WRITE_WORD(drp_regs->DTX_SEL_CONTL, temp16);

  if (rf_freq > 10000)
    ana_ppa_setting = 0x0014; //threshold of -3dBm for HB
  else
    ana_ppa_setting = 0x0005; //threshold of -3dBm for LB

  // PPA THRESHOLD DETECTOR SETTINGS:
  WRITE_BYTE_LOW(drp_regs->ANA_PPAL, ana_ppa_setting);   //# This means that the thershold is set to -3dbm

  //# Load all 1's in the TX DATA buffer
  *(SINT32*)0xffff1e10 = 0xffffffff;
  *(SINT32*)0xffff1e10 = 0xffffffff;
  /*****************************************************************************/

  //#-----------------------------------------------------------------------
  //# Set ABE gain (23 dB)
  //# CTA gains
  //#-----------------------------------------------------------------------
  WRITE_WORD(drp_regs->ANA_CTA_I_0L, 0x87FF);
  WRITE_WORD(drp_regs->ANA_CTA_I_1L, 0x0217);
  WRITE_WORD(drp_regs->ANA_CTA_Q_0L, 0x87FF);
  WRITE_WORD(drp_regs->ANA_CTA_Q_1L, 0x0217);

#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(2000));
#else
  wait_ARM_cycles(8);
#endif

  // Configuring the RF_FREQ register for the desired value using the
  WRITE_WORD(drp_regs->RF_FREQL, rf_freq);

  //####################################################################################################
  //vobs/drp2/sim/input_data/rf_bist/rfb_rx_loop_back/rfb_ppa_rxlb_burnin.jtag_stim.txt@@/main/drp2_dev/1
  //#------------------------------------------------------------------------
  //#Disable LNA & TA for Gain and DC measurements.
  //#-----------------------------------------------------------------------
  WRITE_BYTE_LOW(drp_regs->ANA_LNA_XBL, 0x0000);
  WRITE_BYTE_LOW(drp_regs->ANA_TA_XB_IL, 0x0000);
  WRITE_BYTE_LOW(drp_regs->ANA_TA_XB_QL, 0x0000);

  pulse_kbc1(5);

  //#------------------------------------------------------------------------
  //#Trigger RX_ON script
  //#-----------------------------------------------------------------------
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0082);

#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(1250000));
#else
  wait_ARM_cycles(5000);
#endif

  //dont trigger anything on RX_START edges
  READ_WORD(drp_regs->SCRIPT_STARTL, temp16);
  temp16 = temp16 & 0x3FFF;
  WRITE_WORD(drp_regs->SCRIPT_STARTL, temp16);

  WRITE_BYTE_HIGH(drp_regs->SRM_CWL, 0x0300);
  //#------------------------------------------------------------------------
  //#Enable LNA,TA the total gain set is 23dB + AFE gain
  //#-----------------------------------------------------------------------
  WRITE_BYTE_LOW(drp_regs->ANA_LNA_XBL, 0x00FD);
  // It is seen that when ANA_TA_XB_I/Q[15:8] = 0x49, the DC in the RX signal
  // is quite high. Hence make those bits 0. Need to find out the cause of the
  // large DC.
  WRITE_WORD(drp_regs->ANA_TA_XB_IL, 0x001D);
  WRITE_WORD(drp_regs->ANA_TA_XB_QL, 0x001D);

#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(60000));
#else
  wait_ARM_cycles(240);
#endif
  // Enabling CKM_CLOCKS as they are getting overwritten in the scripts
  READ_WORD(drp_regs->CKM_CLOCKSL, temp16);
  temp16 = temp16 | 0x0080;
  WRITE_WORD(drp_regs->CKM_CLOCKSL, temp16);

  // Enable SAM block as it will not be done inside RX_ON Script
  WRITE_WORD(drp_regs->ANA_SAML, 0x0C01);

  // Enable CQM
  // Frequency error will be monitored for 102 Khz, 51 Khz,25 Khz and 13 Khz
#if DRP_BURNIN_SIMULATE
  WRITE_WORD(drp_regs->DLO_CQML, 0x1413);
  WRITE_WORD(drp_regs->DLO_CQMH, 0x1615);
#else
  //increase thresholds by 2.6 (since all thresholds are powers
  //of 2, we increase them by 4.
  // Frequency error is now monitored for 26MHz, 13MHz, 6.5MHz and 3.25MHz
  WRITE_WORD(drp_regs->DLO_CQML, 0x1615);
  WRITE_WORD(drp_regs->DLO_CQMH, 0x1817);
#endif

#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(2000));
#else
  wait_ARM_cycles(8);
#endif
  WRITE_BYTE_LOW(drp_regs->DLO_SEQ_CQML, 0x000F);

  //assert TX-start
  READ_WORD(drp_regs->DTX_CONFIGL, temp16);
  temp16 = temp16 | 0x0080;
  WRITE_WORD(drp_regs->DTX_CONFIGL, temp16);

  WRITE_WORD(drp_regs->CKM_PPAL, 0x011a);

  //Sumeer: wait for 26us at least to check the status of PPA.
#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(60000));
#else
  wait_ARM_cycles(240);
#endif
  READ_WORD(drp_regs->CKM_WD_STATL, mem_data);
  ppa_active_errflag = (((mem_data >> 8) & 1) != 0);
  err_flag |= ppa_active_errflag;

  burnin_status_display(ppa_active_errflag, 20);

#if 0 //Bug 25 masked out the test
  READ_WORD(drp_regs->RDLO_FLAGS2L, mem_data);
  dlo_errflag = (((mem_data >> 2) & 3) != 0);
  err_flag |= dlo_errflag;

  burnin_status_display(dlo_errflag, 20);
#endif

  //# this control transfer is for the clock-quality monitor
  //# which gets enabled from the mfi-stim.
  //# we are required to do this as currently the CQM
  //# is supported through the OCP MASTER.
  //# We may need to change this later if we can have
  //# the CQM enabled from the JTAG-MASTER itself.

  WRITE_BYTE_LOW(drp_regs->DLO_SEQ_CQML, 0x0000);

  // De-assert TX_Start
  READ_WORD(drp_regs->DTX_CONFIGL, temp16);
  temp16 = temp16 & 0xFF7F;
  WRITE_WORD(drp_regs->DTX_CONFIGL, temp16);

  pulse_kbc1(6);

  /******************************************************************************/
  /* Start RX test                                                              */
  /******************************************************************************/
  // Kick off the sine-wave
  // Before changing the input to the DAC the hold bit is set.
  WRITE_BYTE_HIGH(drp_regs->DAC_CWL, 0x0100);   // Set the hold-bit.
#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(2000));
#else
  wait_ARM_cycles(8);
#endif
  WRITE_BYTE_LOW(drp_regs->DAC_CWL,0x0011);     // Configure DAC to take in LUTPRE output.
#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(2000));
#else
  wait_ARM_cycles(8);
#endif
  WRITE_BYTE_HIGH(drp_regs->DAC_CWL, 0x0000);   // Clear the hold-bit.

#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(4000));
#else
  wait_ARM_cycles(16);
#endif

  //#-----------------------------------------------------------------------
  //# Wait till the sine-wave reaches the DRX output.
  //#-----------------------------------------------------------------------
#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(140000));
#else
  wait_ARM_cycles(560);
#endif

  //#-----------------------------------------------------------------------
  //#
  //#        mem_rx_start=1                             mem_rx_start=0
  //#              ________________________________________
  //#  ___________|                                        |_____________
  //#
  //#-----------------------------------------------------------------------
  //# Set the mem_rx_start bit in the CKM block
  //#-----------------------------------------------------------------------
  READ_WORD(drp_regs->CKM_CLOCKSL, temp16);
  temp16 = temp16 | 0x0010;
  WRITE_WORD(drp_regs->CKM_CLOCKSL, temp16);
#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(2000));
#else
  wait_ARM_cycles(8);
#endif

  // assert RX_START
  temp16 = temp16 | 0x0020;
  WRITE_WORD(drp_regs->CKM_CLOCKSL, temp16);

  //#-----------------------------------------------------------------------
  //# Around 265 us required to collect 128 samples
  //#-----------------------------------------------------------------------

  // Configure GPIO0 pin as output - Reset Bit 0 i.e. set Bit 0 to 0
  //(*( volatile unsigned short* )(0xFFFE4804)) &= (~(1 << 0));    // GPIO-0=1
  // write 1 in GPIO pin 0
  //(*( volatile unsigned short* )(0xFFFE4802)) |= (1 << 0);       // GPIO-0=1

  //wait_ARM_cycles(convert_nanosec_to_cycles(258000)); //wait of 129us
  //WAIT_US(129);
  #if DRP_BURNIN_SIMULATE
    wait_ARM_cycles(6448);   //20.8*129 = 2683.2, 20.8*310 = 6448
  #else
    wait_ARM_cycles(2300);
  #endif
  // write 0 in GPIO pin 0
  //(*( volatile unsigned short* )(0xFFFE4802)) &= (~(1 << 0));    // GPIO-0=0

  //#-----------------------------------------------------------------------
  //# Clear the mem_rx_start bit in the CKM block
  //#-----------------------------------------------------------------------
  temp16 = temp16 & 0xFFDF;
  WRITE_WORD(drp_regs->CKM_CLOCKSL, temp16);

  temp16 = temp16 | 0x0020;
  WRITE_WORD(drp_regs->CKM_CLOCKSL, temp16);
  #if DRP_BURNIN_SIMULATE
    wait_ARM_cycles(6448);   //20.8*129 = 2683.2, 310*20.8 = 6448
  #else
    wait_ARM_cycles(2300);
  #endif
  temp16 = temp16 & 0xFFDF;
  WRITE_WORD(drp_regs->CKM_CLOCKSL, temp16);

  temp16 = temp16 | 0x0020;
  WRITE_WORD(drp_regs->CKM_CLOCKSL, temp16);
  #if DRP_BURNIN_SIMULATE
    wait_ARM_cycles(6448);   //20.8*129 = 2683.2, 310*20.8 = 6448
  #else
    wait_ARM_cycles(2300);
  #endif
  temp16 = temp16 & 0xFFDF;
  WRITE_WORD(drp_regs->CKM_CLOCKSL, temp16);

  //#-----------------------------------------------------------------------
  //# Call the FFT script for I-path
  //#-----------------------------------------------------------------------
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0084);

#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(4400000));
#else
  wait_ARM_cycles(17600);
#endif

  // read the result of FFT
  READ_WORD(drp_regs->OCP_SCRATCHL, mem_data);
  fft_errflag_i = (((mem_data >> 14) & 3) != 1);
  err_flag |= fft_errflag_i;

  processFFT(0);  //fft for I path

#if 0
  //#@# Sumeer testing of wait duration
  // Configure GPIO0 pin as output - Reset Bit 0 i.e. set Bit 0 to 0
  (*( volatile unsigned short* )(0xFFFE4804)) &= (~(1 << 0));    // GPIO-0=1
  // write 1 in GPIO pin 0
  (*( volatile unsigned short* )(0xFFFE4802)) |= (1 << 0);       // GPIO-0=1

  //wait_ARM_cycles(convert_nanosec_to_cycles(20000)); //wait of 10us
  wait_ARM_cycles(208);

   // write 0 in GPIO pin 0
  (*( volatile unsigned short* )(0xFFFE4802)) &= (~(1 << 0));    // GPIO-0=0
#endif

  burnin_status_display(fft_errflag_i, 20);

  pulse_kbc1(7);

  WRITE_WORD(drp_regs->OCP_SCRATCHL, 0x0000);

  //# Write FFT_MAG_INDEX values
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x00) = 0x0000;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x02) = 0x0001;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x04) = 0x0002;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x06) = 0x0003;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x08) = 0x0004;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x0A) = 0x0005;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x0C) = 0x0006;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x0E) = 0x0007;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x10) = 0x0008;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x12) = 0x0009;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x14) = 0x000A;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x16) = 0x000B;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x18) = 0x000C;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x1A) = 0x000D;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x1C) = 0x000E;
  *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_MAG_INDEX + 0x1E) = 0x000F;

  //#-----------------------------------------------------------------------
  //# Call the FFT script for Q-path
  //#-----------------------------------------------------------------------
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x0085);

#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(4400000));
#else
  wait_ARM_cycles(17600);
#endif

  //read the result of FFT
  READ_WORD(drp_regs->OCP_SCRATCHL, mem_data);
  fft_errflag_q = (((mem_data >> 14) & 3) != 1);
  err_flag |= fft_errflag_q;

  processFFT(1);

  burnin_status_display(fft_errflag_q, 20);

  //############################################################
  //### Start Script #15 (IDLE SCRIPT)
  //############################################################
  WRITE_BYTE_LOW(drp_regs->SCRIPT_STARTL, 0x008F);
#if DRP_BURNIN_SIMULATE
  wait_ARM_cycles(convert_nanosec_to_cycles(40000));
#else
  wait_ARM_cycles(160);
#endif

  READ_WORD(drp_regs->CKM_WD_STATL, mem_data);
  ppa_inactive_errflag = (((mem_data >> 8) & 1) != 1);
  err_flag |= ppa_inactive_errflag;

  burnin_status_display(ppa_inactive_errflag, 20);

  pulse_kbc1(8);

  WRITE_WORD(drp_regs->OCP_SCRATCHL, 0x0000);

  //select test port 21 at KBR_1 port
  *((UINT8 *)CONF_KBR_1) = 0x01;
  burnin_status_display(err_flag, 20);
  //unselect test port 21 at KBR_1 port
  *((UINT8 *)CONF_KBR_1) = 0x00;

  return;
}

/**********************************************************************************/
/* Read the captured samples from DRP into MCU memory                             */
/* Also read the Cos and Sin coefficients from DRP memory                         */
/**********************************************************************************/
void capture_sig(SINT16 *sig, SINT16 *cosCoeff, SINT16 *sinCoeff, UINT16 i_q_flag)
{
  #define NUMCOEFF      16

  SINT16 i;
  volatile SINT16 *rx_buf_ptr = (SINT16 *)0xFFFF2000;
  volatile SINT16 *drp_cos_coeff_ptr = (SINT16 *)(DRP_REGS_BASE_ADD + DRP_FFT_COS_COEFF);
  volatile SINT16 *drp_sin_coeff_ptr = (SINT16 *)(DRP_REGS_BASE_ADD + DRP_FFT_SINE_COEFF);

  if (i_q_flag == 0) //I path
    rx_buf_ptr++;

  for (i = 0; i < FFT_BUF_SIZE; i++)
  {
    sig[i] = *rx_buf_ptr++;
   rx_buf_ptr++;
  }
  for (i = 0; i < NUMCOEFF; i++)
  {
    cosCoeff[i] = *drp_cos_coeff_ptr++;
   sinCoeff[i] = *drp_sin_coeff_ptr++;
  }
  return;
}

/**********************************************************************************/
/* Goertzel algorithm to qualify the RX test                                      */
/**********************************************************************************/
void processFFT(UINT16 I_Q_Flag)
{
  #define NUMCOEFF      16
  #define SCALE         8
  #define TRNG_SCALE    (FFT_BUF_SIZE_LOG2 - 1)

  SINT32 v0[NUMCOEFF], v1[NUMCOEFF], v2[NUMCOEFF];
  SINT16 i, j, x[FFT_BUF_SIZE], CoeffCos2[NUMCOEFF], CoeffSin[NUMCOEFF];
  SINT32 real[NUMCOEFF], imag[NUMCOEFF];
  UINT32 mag[NUMCOEFF], temp32;
  UINT16 mag_indx[NUMCOEFF], temp16, sig_thresh, noise_thresh;
  UINT16 sig_start_indx, sig_stop_indx, sig_thresh_test = FAIL, noise_thresh_test = FAIL;
  UINT16 sig_freq_test = FAIL;

  capture_sig(&x[0], &CoeffCos2[0], &CoeffSin[0], I_Q_Flag);

  //initialization
  for (i = 0; i < NUMCOEFF; i++)
  {
    v0[i] = 0;
    v1[i] = 0;
    v2[i] = 0;
   mag_indx[i] = i;
  }

  //Goertzel algorithm with triangular windowing
  for (i = 0; i < (FFT_BUF_SIZE-1); i++)
  {
    for (j = 0; j < NUMCOEFF; j++)
    {
      if (i < (FFT_BUF_SIZE/2 - 1))
        v0[j] = ((x[i]*(i+1)) >> TRNG_SCALE) + ((CoeffCos2[j] * v1[j]) >> SCALE) - v2[j];
  else
        v0[j] = ((x[i]*(FFT_BUF_SIZE-i-1)) >> TRNG_SCALE) + ((CoeffCos2[j] * v1[j]) >> SCALE) - v2[j];

      //if ((v0[j] > 8388608) || (v0[j] < -8388608))
        //printf("OVERFLOW\n");
    }

    for (j = 0; j < NUMCOEFF; j++)
    {
      v2[j] = v1[j];
      v1[j] = v0[j];
    }
  }

  //compute the magnitudes
  for (j = 0; j < NUMCOEFF; j++)
  {
    imag[j] = (CoeffSin[j] * v2[j]) >> (SCALE + FFT_BUF_SIZE_LOG2);
    real[j] = (v0[j] - ((CoeffCos2[j] * v2[j]) >> (SCALE + 1))) >> FFT_BUF_SIZE_LOG2;

    mag[j] = ((real[j] * real[j]) + (imag[j] * imag[j]));
}

  //sort the magnitudes and their indices in descending order
  //Bubble sort
  for (i = 0; i < (NUMCOEFF - 1); i++)
  {
    for (j = i+1; j < NUMCOEFF; j++)
    {
      if (mag[j] >= mag[i])
{
        temp32 = mag[i];
  mag[i] = mag[j];
  mag[j] = temp32;

  temp16 = mag_indx[i];
  mag_indx[i] = mag_indx[j];
  mag_indx[j] = temp16;
      }
    }
  }

  sig_thresh = *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SIG_THRESH + 0x00);
  noise_thresh = *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_NOISE_THRESH + 0x00);
  sig_start_indx = *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SIG_START_INDEX + 0x00);
  sig_stop_indx = *(UINT16*)(DRP_REGS_BASE_ADD + DRP_FFT_SIG_STOP_INDEX + 0x00);

  //maximum magnitude is above signal threshold
  if (mag[0] > sig_thresh)
  {
    sig_thresh_test = PASS;
  }
  burnin_status_display(sig_thresh_test, 5);

  //maximum magnitude is between signal start bin and stop bins
  if ((mag_indx[0] >= sig_start_indx) && (mag_indx[0] <= sig_stop_indx))
  {
    sig_freq_test = PASS;
  }
  burnin_status_display(sig_freq_test, 5);

  //maximum noise magnitude is below noise threshold
  for (i = 0; i < NUMCOEFF; i++)
  {
    if ((mag_indx[i] >= sig_start_indx) && (mag_indx[i] <= sig_stop_indx))
      continue;

    if (mag[i] < noise_thresh)
    {
      noise_thresh_test = PASS;
      break;
    }
 else
   break;
  }
  burnin_status_display(noise_thresh_test, 5);

  return;
}

#endif //#if (DRP_TEST_SW && DRP_BURNIN)


