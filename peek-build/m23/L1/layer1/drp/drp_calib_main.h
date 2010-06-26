/**********************************************************************************
drp_calib_main.h
-- Copyright (c) 2004, Texas Instruments, Inc.
-- Author: Sumeer Bhatara, Shrinivas Gadkari and Pradeep P

Calibration routines header file.
***********************************************************************************
                               Change History
************************************************************************************

Version     Date           Author(s)        Comments
-------------------------------------------------------------------------------------
Ver 0.0     25 Nov 2004    SG, SB and PP    Locosto version based on DRP ver 0.23
Ver 1.01    29 Dec 2004    SG               Based on DRP Ref SW 1.0, with addresses
                                            of srm variables and tables aligned to
                                            locosto scripts version 129.03
************************************************************************************/

#ifndef __DRP_CALIBMAIN_H__
#define __DRP_CALIBMAIN_H__


///////////////////////////////////////////////////////////////////////////////////////////////////////
// typedefs and defines
///////////////////////////////////////////////////////////////////////////////////////////////////////
#define TREF                        29                 // reference temperature in deg C
#define GAIN_SCALE                  10                 // Gains in log2() are scaled up by 2^10
#define ALPHA_SCALE                 14                 // temp coeffs are scaled up by 2^14
#define BETA_SCALE                  25
#define AFE_ALPHA                   -163               // AFE temperature coefficient is fixed at -0.03dB/degC for all gains
                                                       // -163 = round((2^GAIN_SCALE)*log2(10^-0.003))<<(ALPHA_SCALE-GAIN_SCALE))
#define GAIN_0_3_DB                 102                // 0.3dB = round(2^10 * log(10^0.03)/log(2))
#define GAIN_20_0_DB                6804               // 20dB  = round((2^DRP_LOG_SCALE) * log(10^2)/log(2))

#define CONST_2_15                  32768
#define LOG_SHIFT                   10
#define LOG_SCALE                   10

#define FLYBACK_SELECT              1
#define IBIAS_SELECT                2

#define NUM_FLYBACK_DELAY           8
#define NUM_DCO_IBIAS               18
#define NUM_PHE_SAMPLES             512
#define NUM_PHE_SAMPLES_LOG2        9
#define PHE_SAMPLES_SHIFT           9
#define PHE_AVG_CNT                 128
#define PHE_AVG_SHIFT               7
#define PHE_THREHOLD                5461             // PHE_sample >> 8 = 5461 = peak PHE = 30 deg

#define SRM_DATA_SAVED_LEN  0x100           // length of the words to save in 32-bit words, save only that segment of
                                                                       // SRM data structure that overlaps with calc buffer
// for converting gain log2 to gain DB with 11 fractional bits (additional).
// gain_log2 * round(10 * log(2) * 2^11)
// gain_log2 * 6165    ; give DRP_LOG_SCALE + 11 fractional bits.
//
// to convert a gain log2 (scaled with DRP_LOG_SCALE) to gain_db (scaled by
// DRP_GAIN_REPORT_SCALE) use the following.
// gain_db = (gain_log2*DRP_GAIN_REPORT_CNVRT) >> DRP_GAIN_REPORT_SHIFT;
#define DRP_GAIN_REPORT_CNVRT  6165
#define DRP_GAIN_REPORT_SCALE  6
#define DRP_GAIN_REPORT_SHIFT  (LOG_SCALE + 11 - DRP_GAIN_REPORT_SCALE)

#define DRP_GAIN_91_2_DB     31023              // round(2^10 * (9.12/log(2)))

// adc gain is 20*log(32768/.9) = 91.224149
#define DRP_GAIN_ADC_DB     31031              // round(2^10 * (91.224149/(10*log(2))))
// DB mv to dBm conversion.  is 10*log(20) = ~13.103
#define DRP_DBMV2DBM        4426               // round(2^10 * (13.103/(10*log(2))))
// adc gain and  DB mv to dBm conversion together (20*log(32768/.9) - 10*log(20) = 78.2138db
#define DRP_GAIN_ADCDBM     26606              //  round(2^10 * (78.2138*(10*log(2))))

#define NUM_TONE_SAMPLES 256
#define NUM_TONE_SAMPLES_LOG2       8

#define NUM_LOOPBACK_FFT_BINS       128
#define TONE_SAMPLES_SHIFT          8

#define SCF_CAL_LOSS                1361               // loss of 4dB assumed
                                                       // 1361 = round((2^DRP_LOG_SCALE) * log2(10^(dB/10)))
#define LOG_512PP_OFFSET            18432
#define LOG_70DBM_OFFSET            5866
#define LOG_50DBM_OFFSET            12669
#define LOG_45DBM_OFFSET            14370              // 14370 = round [2 * 2^DRP_LOG_SCALE * log2 (129.466)
#define LOG_60DBM_OFFSET            9268               // 14370 = round [2 * 2^DRP_LOG_SCALE * log2 (23.0263)

#define SPECTRAL_ESTIMATION_COUNT   16
#define SPECTRAL_ESTIMATION_SHIFT   4

#define CALC_BUF_START_ADDRESS      0x0300
#define CALC_BUF_SIZE               150
#define SRM_BASE_ADDRESS            0x2000

/* The DCXO Center Freq Calibration related SRM variables */
#define DCXO_HANDSHAKE              upp_drp2.MEM_2FFCL
#define DCXO_FREQ_MEASL             upp_drp2.MEM_2FF8L
#define DCXO_FREQ_MEASH             upp_drp2.MEM_2FF8H
#define FREF_NOMINAL                26000000

/* LSB of DCXO_XTAL(13:0) corresponds to 0.26 Hz*/
/* A / 0.26 = (A * 123) / 32 */
#define XTAL_LSB_COEFF              123
#define XTAL_LSB_SCALE              5

/* Maximum value by which DCXO_XTAL(13:0) can be changed */
#define XTAL_MAX_CHANGE             200

/* Maximum value of the DCXO adc output in peak detect mode */
#define MAX_DCXO_AMP                0x0000001F

//#if DRP_TEST_SW

#define DRP_GAIN_38_0_DB            12926              // 12926 = round(2^10 * log(10^3.8)/log(2))
#define DRP_GAIN_36_5_DB            12416
#define DRP_GAIN_29_0_DB            9865
#define DRP_GAIN_26_0_DB            8844
#define DRP_GAIN_23_0_DB            7824
#define DRP_GAIN_20_0_DB            6803
#define DRP_GAIN_17_0_DB            5783

#define DRP_GAIN_14_0_DB            4762
#define DRP_GAIN_11_0_DB            3742
#define DRP_GAIN_8_0_DB             2721
#define DRP_GAIN_5_0_DB             1701
#define DRP_GAIN_2_0_DB             680
#define DRP_GAIN_0_DB               0

#define DRP_GAIN_1_DB               340
#define DRP_GAIN_2_DB               680

#define DRP_NUM_ABE_GAIN_STEPS      11

#define DRP_NUM_AFE_GAIN_STEPS      2


//Handshake parameters between the calibration routines and the test equipment
#define RTN_BUSY                    0x01
#define RTN_COMPLETE                0x02

// #@# have window size be determined by dbb setting.
// #@# i.e. don't hardcode it.  Parameterize it.
// the number of samples accumulated in ROC_CALC registers.
#define DRP_ROC_ACC_WIN_SIZE        381

// channel select filter attenuation factor
// calculated as follows
//    CSF attenuation = -2.46008896655962 dB
//    linear attenuation = 10^(csf_att/20) = 0.753347847413085;
// Use a fractional value so attenuation is DRP_CSF_LIN_ATT_F16/2^(fractional_bits)
// 15 bit fractional use 24686
// 16 bit fractional use 49371
// 17 bit fractional use 98743
// this is a 16-bit fractional value
#define DRP_CSF_LIN_ATT             49371
//
// define number of fractional bits used.
#define DRP_CSF_LIN_ATT_FBITS       16

// The increment required to index the LUT for a ~100KHz LIF.
// Consider using an input parameter to support multiple LIF frequencies.
#define DRP_LUT_INCR                188

#define AVG_RESET                   0
#define AVG_RUN                     1

#define NO_WINDOW_NORMALIZATION     0
#define WINDOW_MAX_NORMALIZATION    1
#define WINDOW_POWER_NORMALIZATION  2

#define RECTANGULAR_WINDOW          0
#define BOXCAR_WINDOW               RECTANGULAR_WINDOW
#define TRIANGULAR_WINDOW           1
#define BARTLETT_WINDOW             TRIANGULAR_WINDOW
#define HANNING_WINDOW              2
#define HANN_WINDOW                 HANNING_WINDOW
#define HAMMING_WINDOW              3
#define BLACKMAN_WINDOW             4
#define FLATTOP_WINDOW              5
#define KAISER_WINDOW               6

#define PROCESS_IDATA               1
#define PROCESS_QDATA               2
#define PROCESS_IQDATA              3

#define FFT_BUF_SIZE                128
#define FFT_BUF_SIZE_LOG2           7
#define PI                          3.1415926535897932384626433832795
#define M_PI                        3.141592653589

#define SCRIPT_START_ADD            0x0500
#define SCENARIO_ID_ADD             0x2064 //0x2134
#define PARAMETER_1                 0x2068 //0x2138
#define PARAMETER_2                 0x206C //0x213C
#define PARAMETER_3                 0x2070 //0x2140
#define PARAMETER_4                 0x2074 //0x2144
#define PARAMETER_5                 0x2078 //0x2148
#define PARAMETER_6                 0x207C //0x214C

#define HANDSHAKE_ADD               0x2088 //0x2158

#define drp_scenario_id             drp_regs->MEM_2064L //drp_regs->MEM_2134L
#define drp_param1                  drp_regs->MEM_2068L //drp_regs->MEM_2138L
#define drp_param1L                 drp_regs->MEM_2068L //drp_regs->MEM_2138L
#define drp_param1H                 drp_regs->MEM_2068H //drp_regs->MEM_2138H
#define drp_param2                  drp_regs->MEM_206CL //drp_regs->MEM_213CL
#define drp_param2L                  drp_regs->MEM_206CL //drp_regs->MEM_213CL
#define drp_param2H                  drp_regs->MEM_206CH //drp_regs->MEM_213CL

#define drp_param3                  drp_regs->MEM_2070L //drp_regs->MEM_2140L
#define drp_param3L                  drp_regs->MEM_2070L //drp_regs->MEM_2140L
#define drp_param3H                  drp_regs->MEM_2070H //drp_regs->MEM_2140L
#define drp_param4                  drp_regs->MEM_2074L //drp_regs->MEM_2144L
#define drp_param4L                  drp_regs->MEM_2074L //drp_regs->MEM_2144L
#define drp_param4H                  drp_regs->MEM_2074H //drp_regs->MEM_2144H
#define drp_param5                  drp_regs->MEM_2078L //drp_regs->MEM_2148L
#define drp_param5L                 drp_regs->MEM_2078L //drp_regs->MEM_2148L
#define drp_param5H                 drp_regs->MEM_2078H //drp_regs->MEM_2148H
#define drp_param6                  drp_regs->MEM_207CL //drp_regs->MEM_214CL
#define drp_param6L                 drp_regs->MEM_207CL //drp_regs->MEM_214CL
#define drp_param6H                 drp_regs->MEM_207CH //drp_regs->MEM_214CH
#define drp_param7                  drp_regs->MEM_2080L //drp_regs->MEM_2150L
#define drp_param7L                 drp_regs->MEM_2080L //drp_regs->MEM_2150L
#define drp_param7H                 drp_regs->MEM_2080H //drp_regs->MEM_2150H
#define drp_param8                  drp_regs->MEM_2084L //drp_regs->MEM_2154L
#define drp_param8L                 drp_regs->MEM_2084L //drp_regs->MEM_2154L
#define drp_param8H                 drp_regs->MEM_2084H //drp_regs->MEM_2154H

#define drp_handshake               drp_regs->MEM_2088L //drp_regs->MEM_2158L

#define CONTINUOUS_MODE             0x0
#define BURST_MODE                  0x1

#if DRP_BURNIN
#define CONF_KBC_1                  0xFFFEF12A
#define CONF_KBR_1                  0xFFFEF13C
#define CONF_LOCOSTO_DEBUG         0xFFFEF020
#endif

#if DRP_TDL_DFT
#define  NOISE_FIGURE_MEASUREMENT   0
#define  AM_LOOPBACK_MEASUREMENT    1
#define  IIP2_MEASUREMENT           2
#endif

#define PHE_SETTLING_TIME           1000
#define PHE_SCRIPT_RUNTIME          340      /*This is PHE run time*/
#define TX_ON_SCRIPT_RUN_TIME       200
#define IDLE_SCRIPT_RUN_TIME        20

#define CALC_BUFFER_START_ADDRESS  0x0100   //from 0x2400 to 0x247F
#define CALC_BUFFER_END_ADDRESS    0x011F


enum {
  FFT_AVG_CAPTURE_BURST = 0,
  FFT_AVG_COMPUTE_FFT   = 1
};

enum {
  FFT_TYPE_FIXEDPOINT = 0,
  FFT_TYPE_FLOATINGPOINT = 1
};

enum {
  DISABLE_ROC = 0x00,
  ENABLE_ROC = 0x01
};

typedef short   tFixedPointReal;
typedef float tFloatingPointReal;

/**
 * Type used to store sample value
 */
typedef struct tIQsample
  {
    short I;  /* I part of the IQ number */
    short Q;  /* Q part of the IQ number */
  } tIQsample;

typedef struct tComplex
  {
    short Real; // Real part of the complex number
    short Imag; // Imag part of the complex number
  } tComplex;

typedef struct tFixedPointComplex
  {
    tFixedPointReal     Real; /* Real part of the complex number */
    tFixedPointReal     Imag; /* Imag part of the complex number */
  } tFixedPointComplex;

typedef struct tFloatingPointComplex
  {
    tFloatingPointReal  Real; /* Real part of the complex number */
    tFloatingPointReal  Imag; /* Imag part of the complex number */
  } tFloatingPointComplex;

typedef struct {
    SINT16 real;
    SINT16 imag;
} CMPLX16;

typedef struct {
    SINT32 real;
    SINT32 imag;
} CMPLX32;

typedef struct
{
  SINT16 i;
  SINT16 q;
} T_DRP_SRM_DCOFFSET;

typedef struct
{
  UINT16 rf_freqCentre ;
  UINT16 loop_count;
  UINT16 itter_cnt ;
  UINT16 alpha ;
  UINT16 rho ;
  UINT16 lambda ;
  UINT16 ModulationSwtich ;
  UINT16 ckvdsetting ;
  UINT16 sigmadeltaorder ;
  UINT16 cf;
  UINT16 ct ;
} T_DRP_FLYBACK_DELAY_PARAM;
//#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Function declarations
///////////////////////////////////////////////////////////////////////////////////////////////////////

#if ((DRP_TEST_SW && !DRP_BURNIN) || (!DRP_TEST_SW))
void tx_rx_common_calib(UINT16 pcb_config);
void perinv_calibration(void);
void pvt_calibration(UINT16 band, UINT16 txrx_mode);
void dlo_a_t_calibration(void);
void KDCO_calibration(UINT16 band, UINT16 txrx_mode);
void rx_on_calib_timeline(UINT16 band, UINT16 channel);
void dbb_write_rffreq(UINT16 band_index, UINT16 channel, UINT16 txrx_mode);
void set_abe_gain(UINT16 cta_gain_index);
void set_afe_gain(UINT16 afe_gain_index, UINT16 band);
void dbb_run_temp(void);
SINT16 dbb_read_temp(void);
void dbb_pref_set_0777();

void drp_efuse_init(void);
void arm_wait_us(UINT32 time_us);
#endif //#if ((DRP_TEST_SW && !DRP_BURNIN) || (!DRP_TEST_SW))

#if (DRP_TEST_SW && DRP_BURNIN)
void initialize_wait_loop(void);
SYS_UWORD32 convert_nanosec_to_cycles(SYS_UWORD32 time);
void wait_ARM_cycles(SYS_UWORD32 cpt_loop);
#endif //#if (DRP_TEST_SW && DRP_BURNIN)

#if DRP_TDL_SW_DEBUG
  void fft_avg_debug(float *results);
#endif

#if ((DRP_TEST_SW && (DRP_TDL_SW || DRP_BENCH_SW || DRP_TDL_DFT)) || (!DRP_TEST_SW))
void lna_cfreq_calibration(UINT16 band);
void iqmc_calibration(UINT16 band);
void mixer_pole_calibration(UINT16 band);
void scf_pole_calibrationCB1(UINT16 band, UINT32* scf_setting);
void scf_pole_calibrationCB2(UINT16 band, UINT32* scf_setting, UINT16 pole_num);
void check_efuse_and_update_srm(UINT16 BAND_INDEX, UINT32 scf_setting);
SINT16 calc_log(UINT16 num);
void dbb_set_LNA_resonator_setting(UINT16 index);
void dbb_enable_LNA_Qreducingresistor(UINT16 index);
UINT32 dbb_measure_total_gain(void);
UINT32 dbb_measure_total_gain_16x_i(void);
UINT32 dbb_measure_total_gain_16x_q(void);
UINT32 dbb_measure_total_gain_16x(void);
void dbb_disable(volatile UINT16 * reg);

void rxgain_abe_calibration(UINT16 band);
void rxgain_afe_calibration(UINT16 step, UINT16 band);

UINT16 dbb_meas_abe_vpp(UINT16 band, UINT16 abe_index);
SINT16 dbb_find_temp_coeff(UINT16 index, SINT16 gainlog2);
SINT16 dbb_normalize_gain(SINT16 gainlog2, SINT16 alpha, SINT16 temp);
void dbb_enable(volatile UINT16 *reg);

  void dbb_apply_ibias(UINT16 index);

  UINT16 afe_gain_comp2 (UINT16 abe_gain_step, UINT16 afe_gain_step, SINT16 meas_abe_log2);
  UINT16 estimate_abe_gain(UINT16 abe_gain_step, SINT16 temp, SINT16 *abe_actual);
  UINT16 estimate_afe_gain(UINT16 afe_gain_step, SINT16 temp, UINT16 band, SINT16 *afe_actual);
  UINT16 get_closest_abe_gain(SINT16 tar_abe_gain_dB);
  SINT16 calc_tar_afe(SINT16 meas_abe_gain_dB, SINT16 tar_abe_gain_dB, UINT16 afe_gain_step);
  UINT16 get_closest_afe_gain(SINT16 tar_afe_gain_dB);

  void dbb_rx_burst1(void);
  void dbb_rx_burst2(UINT16 abe_gain_index, UINT16 afe_gain_index, UINT16 pole_num, UINT16 band, UINT16 gain_comp_enable);

  #if DRP_TEST_SW || DRP_TDL_DFT
    void dco_retiming_calibration(void);
    void dco_retiming_data_collection(UWORD16 rf_freq, UWORD16 extended_tx_enable);

    void rms_phe_compute(UWORD16 rf_freq, UINT32 *rms_array_ptr, UINT16 extended_tx_enable, UINT16 flyback_ibias_select);

    void dbb_capture_phe_samples_setting();
    void dbb_capture_phe_samples();
    void dbb_apply_ckv_delay(UINT16 index);

    void spectral_estimate(short *x, short N, short m, short *coeffptr, long *spec_est);
  SINT16 rx_timeline_new(UINT16 band, UINT16 channel, UINT16 abe_gain_indx, UINT16 afe_gain_indx, UINT16 gain_comp_enable, UINT16 pole_num, UINT16 pcb_config);
  SINT16 rx_burst_sr1(UINT16 channel, UINT16 band, UINT16 abe_gain_indx, UINT16 afe_gain_indx, UINT16 pole_num, UINT16 gain_comp_enable, UINT16 pcb_config);
  void rx_burst_sr2(void);
  SINT16 dbb_rx_burst3(UINT16 abe_gain_index, UINT16 afe_gain_index, UINT16 band);

  void ref_roc(CMPLX16 *rxbufptr, UINT16 num_iqsamples, UINT16 accmulation_count, UINT16 start_index);
  static void bit_rev(tFloatingPointComplex* samples, unsigned short nb_samples_pow2);
  static void fft(tFloatingPointComplex* samples, unsigned short nb_input_samples);
  short compute_fft(tIQsample* input_IQ_samples, tFloatingPointComplex* output_FFT, unsigned short nb_input_samples, short data_type_to_process, tFloatingPointReal* window);
  void real_WINDOW(int window_type, unsigned short window_size, int normalisation_type, tFloatingPointReal* extra_parameters, tFloatingPointReal* window);
  void real_PEAK(tFloatingPointReal* samples, tFloatingPointReal* MAX_value, unsigned short* MAX_pos, unsigned short nb_samples);
  void real_POWER(tFloatingPointReal* samples, tFloatingPointReal* POWER_value, unsigned short nb_samples);
  tFloatingPointReal real_NORM2(tFloatingPointReal* samples);
  tFloatingPointReal real_NORM(tFloatingPointReal* samples);
  void real_ENERGY(tFloatingPointReal* samples, tFloatingPointReal* ENERGY_value, unsigned short nb_samples);
  void compute_running_avg(tFloatingPointComplex* input_samples, tFloatingPointComplex* output_samples, unsigned short nb_samples, unsigned short avg_type);

  UINT32 fft_avg_fixed(UINT16 band, UINT16 channel, UINT16 ABE_gain_step, UINT16 AFE_gain_step,
                 UINT16 pole_num, UINT16 roc_setting, UINT16 gain_comp_enable, UINT16 phase_mm_bin, UINT16 num_avg,
                 UINT16 win_type, UINT16 win_norm_type, UINT16 pcb_config);

  UINT32 fft_avg_float(UINT16 band, UINT16 channel, UINT16 ABE_gain_step, UINT16 AFE_gain_step,
                 UINT16 pole_num, UINT16 roc_setting, UINT16 gain_comp_enable, UINT16 phase_mm_bin, UINT16 num_avg,
                 UINT16 win_type, UINT16 win_norm_type, UINT16 pcb_config);

  UINT32 fft_avg_ate(UINT16 band, UINT16 channel, UINT16 ABE_gain_step, UINT16 AFE_gain_step,
                          UINT16 pole_num, UINT16 roc_setting, UINT16 gain_comp_enable, UINT16 phase_mm_bin,
                          UINT16 num_avg, UINT16 win_type, UINT16 win_norm_type, UINT16 pcb_config,
                          UINT16 captureburst_computefft_flag);

  void fft_post_evaluation(UINT16 bin_interest, UINT16 compute_noise, UINT16 bin_start,
                  UINT16 bin_stop, UINT16 num_bins, UINT16 fixed_float_flag);

void compute_fft_iq_pwr(tFloatingPointComplex* input_samples, tFloatingPointComplex* output_samples,
                        unsigned short nb_samples, short data_type_to_process);


  UINT16 compute_fft_iq_pwr_fixed(CMPLX16* input_samples, UINT32* output_samples,
                                            SINT16 data_type_to_process, SINT16 fft_shift, UINT16 phase_bin);

  #else //only in phone SW
  WORD16 drp_gain_correction(UWORD16 arfcn, UWORD8 lna_off, UWORD16 agc);
#endif

  #if DRP_PHASE2_CALIBRATION
    void temp_sensor_calibration(SINT16 calib_temp);
    void dcxo_center_freq_calibration(void);
    void dco_ibias_calibration(void);
    void dco_ibias_data_collection(UWORD16 rf_freq, UWORD16 extended_tx_enable);
  #endif // DRP_PHASE2_CALIBRATION
#endif

#if (DRP_TEST_SW && DRP_TDL_DFT)
  void initialize_system(void);
  UINT32 set_offset_and_measure(UINT16 offset, UINT16 type_bank, UINT16 wait_time_usec, UINT32* rxbuffer);
  UINT16 test_freq_delta(UINT32 last,UINT32 current, UINT32 min_delta,UINT32 max_delta);
  UINT32 range_test(UINT16 pvt_set, UINT16 ab_set, UINT16 track_bank, UINT32 wait_time_usec, UINT32* rxbuffer);
  void adpll_dft_test(UINT16 test_type, UINT32 *rxbuffer);
  UINT16 drp_rx_dft(UINT16 band, UINT16 channel, UINT16 abe_gain_indx, UINT16 afe_gain_indx,
                    UINT16 pcb_config, UINT16 tone_indx, UINT16 power_indx, UINT16 dft_type, UINT16 num_bursts);
  UINT16 drp_loopback_tone_check(UINT16 band, UINT16 channel, UINT16 abe_gain_indx, UINT16 afe_gain_indx, \
                                 UINT16 power_indx, UINT16 tone_indx, UINT16 pcb_config);
  void compute_sig_noise_power(float *FFT_Pwr_I, float *FFT_Pwr_Q);
  void compute_power(float *tone_power, float *noise_power, float *spectrum, UINT16 num_bins);
  void drp_nf_measurement(UINT16 band, UINT16 channel, UINT16 abe_gain_indx, UINT16 afe_gain_indx,
                          UINT16 afe_on_off, UINT16 pcb_config);
  void drp_iip2_measurement();
#endif //#if ((DRP_TEST_SW && !DRP_BURNIN) || (!DRP_TEST_SW))

#if (DRP_TEST_SW && DRP_BURNIN)
  void drp2_burnin_test();
  void burnin_status_display(UINT16 err_flag, UINT16 duration);
  void load_drp_scripts(UINT16 *scripts_data);
  void capture_sig(SINT16 *sig, SINT16 *cosCoeff, SINT16 *sinCoeff, UINT16 i_q_flag);
  void processFFT(UINT16 I_Q_Flag);
#endif

void flybackdelayCalibration(T_DRP_FLYBACK_DELAY_PARAM *flybackdelay_param );
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
           UINT16 sigmadeltaorder);

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
                  );

void dbb_set_phe_samples_setting();

void dbb_restore_phe_samples_setting();

#endif //#ifndef __CALIBMAIN_H__

