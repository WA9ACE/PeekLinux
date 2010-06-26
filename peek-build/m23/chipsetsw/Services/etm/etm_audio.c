/********************************************************************************
 * Enhanced TestMode (ETM)
 * @file	etm_audio.c (Support for AUDIO commands)
 *
 * @author	Kim T. Peteren (ktp@ti.com)
 * @version 0.8
 *

 *
 * History:
 *
 * 	Date       	Modification
 *  ------------------------------------
 *  16/06/2003	Creation
 *  30/06/2003  Small cleanup in func. etm_audio_write, etm_audio_saveload and
 *              etm_aud_wait_for_aud_event is updated and renamed
 *  12/08/2003  The func. etm_audio_write has been updated regarding the AEC struct.
 *  14/08/2003  The func. etm_audio_read has been updated regarding the AEC struct.
 *  10/02/2004  Updated supporting of Audio(RD/WR) parameter table regarding RIV101 V2.8 doc.
 *  18/02/2004  Modified the event handling, events revceived from the Audio SWE. 
 *              Integrated event callback function, etm_audio_callback().
 *  20/02/2004  Updated the audio rd/wr cases.
 *  10/03/2004  Integrated support of ECHO SUPPRESSOR (AUDIO_MICROPHONE_ES)
 *  20/04/2004  Updated switch cases _ES, _IIR, and _LIMITER regarding TIF modifications.
 *  09/11/2005  Added support for Locosto acoustic algorithme.
 *  06/12/2005  Fixed pkt->size issue in read function.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 *********************************************************************************/


#include "etm/etm.h"
#include "etm/etm_api.h"
#include "etm/etm_trace.h"
#include "etm/etm_env.h" // Need because use of T_ETM_ENV_CTRL_BLK 
#include "etm/etm_audio_err.h" // Privat Audio error codes for PC and Target 

#include "etm/etm_trace.h"
#if(PSP_STANDALONE==0)
#include "audio/audio_api.h"
#endif

#include "rv/rv_general.h"
#include "spi/spi_drv.h" // used for codec read/write

#include "memif/mem.h"
#include <string.h>

#define AS_RFS_API 1

#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#endif


/******************************************************************************
 * Globals
 *****************************************************************************/

// Version of the ETM AUDIO module
//const uint16 etm_audio_revision = (1<<12) | (0x1);

extern T_ETM_ENV_CTRL_BLK *etm_env_ctrl_blk;

static int etm_audio_event_status = 0;


/******************************************************************************
 * Internal prototypes 
 *****************************************************************************/

int etm_audio(uint8 *indata, int insize);
T_RV_HDR *etm_audio_wait_for_event(UINT16 msg_id_expected);
T_ETM_PKT *etm_audio_setup(uint8 fid, uint8 cfg_data);
void etm_audio_callback(void *event_from_audio);


/******************************************************************************
 * Register the Audio Module to the ETM database 
 *****************************************************************************/

int etm_audio_init(void)
{
    int result; 

    result = etm_register("AUDIO", ETM_AUDIO, 0, 0, etm_audio);
    return result;    
}


/******************************************************************************
 * Audio Functions
 *****************************************************************************/

int etm_audio_read(T_ETM_PKT *pkt, uint8 *buf)
{
    int result = ETM_OK, size = 0, size_tmp, i, u, v;
#if(PSP_STANDALONE==0) /*make the function dummy if its a PSP STANDALONE build*/	
    uint8 param;
    T_AUDIO_FULL_ACCESS_READ audio;
#if (L1_AEC != 2)
    T_AUDIO_AEC_CFG *aec_parameter = NULL;
#else
    T_AUDIO_AQI_AEC_CFG *aec_parameter = NULL;
#endif 
    void *parameter = NULL;
    
    param = *buf;
    if ((result = etm_pkt_put8(pkt, param)) < 0) 
        return result;
    
    tr_etm(TgTrAudio, "ETM AUDIO: _audio_read: param(%d)", param);
    
    audio.variable_indentifier = param;
    audio.data = (T_AUDIO_FULL_ACCESS_READ *) &pkt->data[2]; //data[0] = fid
                                                             //data[1] = parameter/identifier
    
    if ((result = audio_full_access_read(&audio)) != AUDIO_OK){
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_read: ERROR(%d)", result);
        if (result == AUDIO_ERROR) 
            return ETM_INVAL;         // Invalid audio parameter
        else
            return ETM_AUDIO_FATAL;
    }

    switch (param) {
    case AUDIO_PATH_USED:
        size = sizeof(T_AUDIO_VOICE_PATH_SETTING);
        pkt->size += size;
        break;
    case AUDIO_MICROPHONE_MODE:
    case AUDIO_MICROPHONE_GAIN:
    case AUDIO_MICROPHONE_EXTRA_GAIN:
    case AUDIO_MICROPHONE_OUTPUT_BIAS:
    case AUDIO_MICROPHONE_SPEAKER_LOOP_SIDETONE:
        size = sizeof(INT8);
        pkt->size += size;
        break;
#if (L1_ANR == 1)
    case AUDIO_MICROPHONE_ANR:
        size = sizeof(T_AUDIO_ANR_CFG);
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_read: AUDIO_MICROPHONE_ANR (L1_ANR=1) (%d)", size); 

        parameter = &pkt->data[2];
        etm_pkt_put8(pkt, ((T_AUDIO_ANR_CFG *) parameter)->anr_enable);       // 1
        etm_pkt_put16(pkt,((T_AUDIO_ANR_CFG *) parameter)->min_gain);         // 2
        etm_pkt_put8(pkt, ((T_AUDIO_ANR_CFG *) parameter)->div_factor_shift); // 3
        etm_pkt_put8(pkt, ((T_AUDIO_ANR_CFG *) parameter)->ns_level);         // 4
        break;
#endif // (L1_ANR)
#if (L1_ANR == 2)
    case AUDIO_MICROPHONE_ANR:
        size = sizeof(T_AUDIO_AQI_ANR_CFG);
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_read: AUDIO_MICROPHONE_ANR (L1_ANR=2) (%d)", size);

        parameter = &pkt->data[2];
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_ANR_CFG *) parameter)->anr_ul_control);          // 0
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_ANR_CFG *) parameter)->parameters.control);      // 1
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_ANR_CFG *) parameter)->parameters.ns_level);     // 2
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_ANR_CFG *) parameter)->parameters.tone_ene_th);  // 3
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_ANR_CFG *) parameter)->parameters.tone_cnt_th);  // 4
        break;
#endif // (L1_ANR == 2)
#if (L1_AEC != 2)
    case AUDIO_MICROPHONE_SPEAKER_LOOP_AEC:
        size = sizeof(T_AUDIO_AEC_CFG); 
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_read: AUDIO_MICROPHONE_SPEAKER_LOOP_AEC (L1_AEC!=2) (%d)", size);             
        aec_parameter = (T_AUDIO_AEC_CFG *) &pkt->data[2];

        etm_pkt_put16(pkt, aec_parameter->aec_enable);                  // 1
#if (L1_NEW_AEC)
        etm_pkt_put16(pkt, aec_parameter->continuous_filtering);        // 2
        etm_pkt_put16(pkt, aec_parameter->granularity_attenuation);     // 3
        etm_pkt_put16(pkt, aec_parameter->smoothing_coefficient);       // 4
        etm_pkt_put16(pkt, aec_parameter->max_echo_suppression_level);  // 5
        etm_pkt_put16(pkt, aec_parameter->vad_factor);                  // 6
        etm_pkt_put16(pkt, aec_parameter->absolute_threshold);          // 7
        etm_pkt_put16(pkt, aec_parameter->factor_asd_filtering);        // 8
        etm_pkt_put16(pkt, aec_parameter->factor_asd_muting);           // 9
        etm_pkt_put16(pkt, aec_parameter->aec_visibility);              //10
#else
        etm_pkt_put16(pkt, aec_parameter->aec_mode);                    // 2
        etm_pkt_put16(pkt, aec_parameter->echo_suppression_level);      // 3
#endif // end of (L1_NEW_AEC)
#if (L1_ANR == 0)
        etm_pkt_put16(pkt, aec_parameter->noise_suppression_enable);    // 4 or 11
        etm_pkt_put16(pkt, aec_parameter->noise_suppression_level);     // 5 or 12
#endif // end of (L1_ANR)
        break;
#else // for L1_AEC == 2   
    case AUDIO_MICROPHONE_SPEAKER_LOOP_AEC:
        size = sizeof(T_AUDIO_AQI_AEC_CFG);              
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_read: AUDIO_MICROPHONE_SPEAKER_LOOP_AEC (L1_AEC==2) (%d)", size);             
        aec_parameter = (T_AUDIO_AQI_AEC_CFG *) &pkt->data[2];

        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->aec_control);     // 0
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->aec_mode);        // 1
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->mu);              // 2
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->cont_filter);     // 3
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->scale_input_ul);  // 4
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->scale_input_dl);  // 5
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->div_dmax);        // 6
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->div_swap_good);   // 7
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->div_swap_bad);    // 8
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->block_init);      // 9
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->fact_vad);        // 10
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->fact_asd_fil);    // 11
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->fact_asd_mut);    // 12
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->thrs_abs);        // 13
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->es_level_max);    // 14
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->granularity_att); // 15
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->coef_smooth);     // 16
        break;
#endif // (L1_AEC != 2)
#if (L1_ES == 1)  
    case AUDIO_MICROPHONE_ES:
        size = sizeof(T_AUDIO_ES_CFG);
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_read: AUDIO_MICROPHONE_ES (L1_ES=1) (%d)", size);
        parameter = &pkt->data[2];

        etm_pkt_put8 (pkt, ((T_AUDIO_ES_CFG *) parameter)->es_enable);             // 1
        etm_pkt_put8 (pkt, ((T_AUDIO_ES_CFG *) parameter)->es_behavior);           // 2
        etm_pkt_put8 (pkt, ((T_AUDIO_ES_CFG *) parameter)->es_mode);                // 3
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->es_gain_dl);            // 4
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->es_gain_ul_1);          // 5
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->es_gain_ul_2);          // 6
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->tcl_fe_ls_thr);         // 7
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->tcl_dt_ls_thr);         // 8
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->tcl_fe_ns_thr);         // 9
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->tcl_dt_ns_thr);         // 10
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->tcl_ne_thr);            // 11
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->ref_ls_pwr);            // 12
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->switching_time);        // 13
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->switching_time_dt);     // 14
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->hang_time);             // 15
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->gain_lin_dl_vect[0]);   // 16
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->gain_lin_dl_vect[1]);   // 17
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->gain_lin_dl_vect[2]);   // 18
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->gain_lin_dl_vect[3]);   // 19
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->gain_lin_ul_vect[0]);   // 20
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->gain_lin_ul_vect[1]);   // 21
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->gain_lin_ul_vect[2]);   // 22
        etm_pkt_put16(pkt, ((T_AUDIO_ES_CFG *) parameter)->gain_lin_ul_vect[3]);   // 23
        break;
#endif // end of (L1_ES)
#if (L1_AGC_UL== 1)
    case AUDIO_MICROPHONE_AGC:
        size = sizeof(T_AUDIO_AQI_AGC_UL_REQ);
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_read: AUDIO_MICROPHONE_AGC (L1_AGC_UL=1) (%d)", size);
        parameter = &pkt->data[2];

        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->agc_ul_control);                     // 0
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.control);                 // 1
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.frame_size);              // 2
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.targeted_level);          // 3
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.signal_up);               // 4
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.signal_down);             // 5
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.max_scale);               // 6
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.gain_smooth_alpha);       // 7
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.gain_smooth_alpha_fast);  // 8
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.gain_smooth_beta);        // 9
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.gain_smooth_beta_fast);   // 10
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.gain_intp_flag);          // 11
        break;
#endif // (L1_AGC_UL== 1)
    case AUDIO_MICROPHONE_FIR:
    case AUDIO_SPEAKER_FIR:
        size = sizeof(T_AUDIO_FIR_COEF);
        pkt->size += size;
        break;
    case AUDIO_SPEAKER_MODE:
    case AUDIO_SPEAKER_GAIN:
    case AUDIO_SPEAKER_EXTRA_GAIN:
    case AUDIO_STEREO_SPEAKER_EXTRA_GAIN:
    case AUDIO_SPEAKER_FILTER:
    case AUDIO_SPEAKER_HIGHPASS_FILTER:
    case AUDIO_SPEAKER_BUZZER_STATE:
    case AUDIO_STEREO_SPEAKER_MODE:
    case AUDIO_STEREO_SPEAKER_STEREO_MONO:
    case AUDIO_STEREO_SPEAKER_SAMPLING_FREQUENCY:
    case AUDIO_ONOFF:
    case AUDIO_FM_SPEAKER_MODE:
        size = sizeof(INT8);
        pkt->size += size;
        break;
    case AUDIO_VOLUME_SPEED:
        size = sizeof(INT16);
        pkt->size += size;
        break;
#if (L1_IIR == 1)
    case AUDIO_SPEAKER_IIR:
        size = sizeof(T_AUDIO_IIR_CFG);
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_read: AUDIO_SPEAKER_IIR (%d)", size);
        parameter = &pkt->data[2];

        etm_pkt_put8(pkt, ((T_AUDIO_IIR_CFG *) parameter)->iir_enable);          // 1
        etm_pkt_put8(pkt, ((T_AUDIO_IIR_CFG *) parameter)->nb_iir_blocks);       // 2

        // Read the IIR coefs. 
        for (i=0; i < AUDIO_IIR_MAX_IIR_BLOCKS*8 ; i++) {
            etm_pkt_put16(pkt,((T_AUDIO_IIR_CFG *) parameter)->iir_coefs[i]);    // 3
        }
        etm_pkt_put8(pkt,((T_AUDIO_IIR_CFG *) parameter)->nb_fir_coefs);         // 4
        // Read the FIR coefs.
        for (i=0; i < AUDIO_IIR_MAX_FIR_COEFS; i++) {
            etm_pkt_put16(pkt,((T_AUDIO_IIR_CFG *) parameter)->fir_coefs[i]);    // 5
        }
        etm_pkt_put8(pkt, ((T_AUDIO_IIR_CFG *) parameter)->input_scaling);       // 6
        etm_pkt_put8(pkt, ((T_AUDIO_IIR_CFG *) parameter)->fir_scaling);         // 7
        etm_pkt_put8(pkt, ((T_AUDIO_IIR_CFG *) parameter)->input_gain_scaling);  // 8
        etm_pkt_put8(pkt, ((T_AUDIO_IIR_CFG *) parameter)->output_gain_scaling); // 9
        etm_pkt_put16(pkt,((T_AUDIO_IIR_CFG *) parameter)->output_gain);         // 10
        etm_pkt_put16(pkt,((T_AUDIO_IIR_CFG *) parameter)->feedback);            // 11
        break;
#endif // end of (L1_IIR)
#if (L1_IIR == 2)
    case AUDIO_SPEAKER_IIR:
        size = sizeof(T_AUDIO_IIR_DL_CFG); // _UL_ or _DL_ ??? 
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_read: AUDIO_SPEAKER_IIR (L1_IIR=2) (%d)", size); 
        parameter = &pkt->data[2];

        etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->iir_dl_control);                      // 1
        etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.control);                  // 2
        etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.frame_size);               // 3
        etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.fir_swap);                 // 4
        // FIR parameters
        etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.fir_filter.fir_enable);    // 5
        etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.fir_filter.fir_length);    // 6
        etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.fir_filter.fir_shift);     // 7
        // Read the FIR coefs.
        for (i=0; i < AUDIO_IIR_MAX_FIR_TAPS; i++) {
            etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.fir_filter.fir_taps[i]);  // 8 .. 47
        }
        // SOS parameters
        etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.sos_filter.sos_enable);     // 48
        etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.sos_filter.sos_number);     // 49

        for (i=0; i < AUDIO_IIR_MAX_IIR_BLOCKS; i++) { // run 6 times                                // 50 .. 97 
            etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.sos_filter.sos_filter[i].sos_fact);        
            etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.sos_filter.sos_filter[i].sos_fact_form);   
            for (u=0; u < 2; u++) {
                etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.sos_filter.sos_filter[i].sos_den[u]);
            }
            for (v=0; v < 3; v++) {
                etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.sos_filter.sos_filter[i].sos_num[v]);
            }
            etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.sos_filter.sos_filter[i].sos_num_form);  
        }

        etm_pkt_put16(pkt,((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.gain);   // 98
        break;
#endif // (L1_IIR == 2)
#if (L1_LIMITER == 1)
    case AUDIO_SPEAKER_LIMITER:
        size = sizeof(T_AUDIO_LIMITER_CFG);
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_read: AUDIO_SPEAKER_LIMITER (L1_LIMITER=1) (%d)", size); 
        parameter = &pkt->data[2];

        etm_pkt_put8(pkt,  ((T_AUDIO_LIMITER_CFG *) parameter)->limiter_enable);      // 1
        etm_pkt_put16(pkt, ((T_AUDIO_LIMITER_CFG *) parameter)->block_size);          // 2
        etm_pkt_put16(pkt, ((T_AUDIO_LIMITER_CFG *) parameter)->slope_update_period); // 3
        etm_pkt_put16(pkt, ((T_AUDIO_LIMITER_CFG *) parameter)->nb_fir_coefs);        // 4
        // Read filter coefs
        for (i=0; i < ((AUDIO_LIMITER_MAX_FILTER_COEFS-1)/2)+1; i++) {
            etm_pkt_put16(pkt, ((T_AUDIO_LIMITER_CFG *) parameter)->filter_coefs[i]); // 5
        }
        etm_pkt_put16(pkt, ((T_AUDIO_LIMITER_CFG *) parameter)->thr_low_0);           // 6
        etm_pkt_put16(pkt, ((T_AUDIO_LIMITER_CFG *) parameter)->thr_low_slope);       // 7
        etm_pkt_put16(pkt, ((T_AUDIO_LIMITER_CFG *) parameter)->thr_high_0);          // 8
        etm_pkt_put16(pkt, ((T_AUDIO_LIMITER_CFG *) parameter)->thr_high_slope);      // 9
        etm_pkt_put16(pkt, ((T_AUDIO_LIMITER_CFG *) parameter)->gain_fall);           // 10
        etm_pkt_put16(pkt, ((T_AUDIO_LIMITER_CFG *) parameter)->gain_rise);           // 11
        break;
#endif // end of (L1_LIMITER)
#if (L1_AGC_DL == 1)
    case AUDIO_SPEAKER_AGC:
        size = sizeof(T_AUDIO_AQI_AGC_DL_REQ);
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_read: AUDIO_SPEAKER_AGC (L1_AGC_DL=1) (%d)", size);
        parameter = &pkt->data[2];

        etm_pkt_put16(pkt,  ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->agc_dl_control);                    // 1
        // Parameters
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.control);                 // 2
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.frame_size);              // 3
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.targeted_level);          // 4
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.signal_up);               // 5
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.signal_down);             // 6
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.max_scale);               // 7
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.gain_smooth_alpha);       // 8
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.gain_smooth_alpha_fast);  // 9
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.gain_smooth_beta);        // 10
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.gain_smooth_beta_fast);   // 11
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.gain_intp_flag);          // 12
        break;
#endif // (L1_AGC_DL == 1)
    case AUDIO_SPEAKER_VOLUME_LEVEL:
        size = sizeof(T_AUDIO_SPEAKER_LEVEL);
        pkt->size += size;
        break;
    case AUDIO_STEREO_SPEAKER_VOLUME_LEVEL:
        size = sizeof(T_AUDIO_STEREO_SPEAKER_LEVEL);
        pkt->size += size;
        break;
#if (L1_DRC == 1)
    case AUDIO_SPEAKER_DRC:
        size = sizeof(T_AUDIO_DRC_CFG);
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_read: AUDIO_XXXX_DRC (%d)", size);
        parameter = &pkt->data[2];

        etm_pkt_put16(pkt,  ((T_AUDIO_DRC_CFG *) parameter)->drc_dl_control);                              // 1
        // Parameters
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.speech_mode_samp_f);                // 2
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.num_subbands);                      // 3
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.frame_len);                         // 4
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.expansion_knee_fb_bs);              // 5
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.expansion_knee_md_hg);              // 6
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.expansion_ratio_fb_bs);             // 7
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.expansion_ratio_md_hg);             // 8
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.max_amplification_fb_bs);           // 9
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.max_amplification_md_hg);           // 10
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.compression_knee_fb_bs);            // 11
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.compression_knee_md_hg);            // 12
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.compression_ratio_fb_bs);           // 13
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.compression_ratio_md_hg);           // 14
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.energy_limiting_th_fb_bs);          // 15
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.energy_limiting_th_md_hg);          // 16
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.limiter_threshold_fb);              // 17
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.limiter_threshold_bs);              // 18
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.limiter_threshold_md);              // 19
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.limiter_threshold_hg);              // 20
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.limiter_hangover_spect_preserve);   // 21
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.limiter_release_fb_bs);             // 22
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.limiter_release_md_hg);             // 23
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.gain_track_fb_bs);                  // 24
        etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.gain_track_md_hg);                  // 25
        for (i=0; i < 17; i++) { // FixMe size must be a variable
            etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.low_pass_filter[i]);            // 26
        }
        for (i=0; i < 17; i++) { // FixMe size must be a variable
            etm_pkt_put16(pkt, ((T_AUDIO_DRC_CFG *) parameter)->parameters.mid_band_filter[i]);            // 27
        }
        break;
#endif // (L1_DRC == 1)
#if (L1_WCM == 3)
    case XXXX_WCM:
        size = sizeof(T_AUDIO_AQI_WCM_REQ);
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_read: AUDIO_XXXX_WCM (%d)", size);

        parameter = &pkt->data[2];
        etm_pkt_put8(pkt,  ((T_AUDIO_AQI_WCM_REQ *) parameter)->wcm_control);                // 1
        // Parameters
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_WCM_REQ *) parameter)->parameters.mode);            // 2
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_WCM_REQ *) parameter)->parameters.frame_size);      // 3
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_WCM_REQ *) parameter)->parameters.num_sub_frames);  // 4
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_WCM_REQ *) parameter)->parameters.ratio);           // 5
        etm_pkt_put16(pkt, ((T_AUDIO_AQI_WCM_REQ *) parameter)->parameters.threshold);       // 6
        for (i=0; i < 16; i++) {  // FixMe size must be a variable
            etm_pkt_put16(pkt, ((T_AUDIO_AQI_WCM_REQ *) parameter)->parameters.gain[i]);     // 7
        }
        break;
#endif //  (L1_WCM == 1)
    default:
        result = ETM_INVAL;                            
    }

    //pkt->size += size;
#endif
    return result;
}
 

int etm_audio_write(T_ETM_PKT *pkt, uint8 *buf)
{
    T_RV_HDR *msg = NULL;
    T_RV_RETURN return_path;
    int result = ETM_OK, i, v, u;	
#if(PSP_STANDALONE==0)  /*making it a dummy function if its a PSP standalone build */	
    T_AUDIO_FULL_ACCESS_WRITE audio;
    #if (L1_AEC == 2) 
	T_AUDIO_AQI_AEC_CFG *aec_parameter = NULL;
    #else 
    T_AUDIO_AEC_CFG *aec_parameter = NULL;
    #endif  
    void *parameter = NULL;
    uint8 param;

    param = *buf++;
    if ((result = etm_pkt_put8(pkt, param)) < ETM_OK) {
        return result;
    } 

    tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: param(%d)", param);

    return_path.addr_id        = NULL; //etm_env_ctrl_blk->addr_id;
    return_path.callback_func  = etm_audio_callback;

    audio.variable_indentifier = param;
    audio.data = buf;

    switch (param) {


	case AUDIO_ONOFF:
		{
			//Only uplink
			if(*((INT8  *)audio.data)==1)
				{
					((T_AUDIO_MODE_ONOFF *)audio.data)->vdl_onoff=0;
					((T_AUDIO_MODE_ONOFF *)audio.data)->vul_onoff=1;

				}
			//only downlink
			else if(*((INT8  *)audio.data)==2)
				{
					((T_AUDIO_MODE_ONOFF *)audio.data)->vdl_onoff=1;
					((T_AUDIO_MODE_ONOFF *)audio.data)->vul_onoff=0;
			     }
			//both
			else if(*((INT8  *)audio.data)==3)
							{
								((T_AUDIO_MODE_ONOFF *)audio.data)->vdl_onoff=1;
								((T_AUDIO_MODE_ONOFF *)audio.data)->vul_onoff=1;
			     }

		}


#if (L1_ANR == 1)
    case AUDIO_MICROPHONE_ANR:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: AUDIO_MICROPHONE_ANR (L1_ANR=1)");
        parameter = etm_malloc (sizeof(T_AUDIO_AEC_CFG));
    
        ((T_AUDIO_ANR_CFG *) parameter)->anr_enable       = etm_get8(buf);  buf += 1; 
        ((T_AUDIO_ANR_CFG *) parameter)->min_gain         = etm_get16(buf); buf += 2; 
        ((T_AUDIO_ANR_CFG *) parameter)->div_factor_shift = etm_get8(buf);  buf += 1;
        ((T_AUDIO_ANR_CFG *) parameter)->ns_level         = etm_get8(buf);  buf += 1;
        audio.data = parameter;
        break;
#endif // (L1_ANR == 1)
#if (L1_ANR == 2)
    case AUDIO_MICROPHONE_ANR:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: AUDIO_MICROPHONE_ANR (L1_ANR=2) (%d)");
        parameter = etm_malloc (sizeof(T_AUDIO_AQI_ANR_CFG));

        ((T_AUDIO_AQI_ANR_CFG *) parameter)->anr_ul_control          = (T_AUDIO_ANR_CONTROL) etm_get16(buf); buf += 2;     // 0
        ((T_AUDIO_AQI_ANR_CFG *) parameter)->parameters.control      = etm_get16(buf); buf += 2;     // 1
        ((T_AUDIO_AQI_ANR_CFG *) parameter)->parameters.ns_level     = etm_get16(buf); buf += 2;     // 4
        ((T_AUDIO_AQI_ANR_CFG *) parameter)->parameters.tone_ene_th  = etm_get16(buf); buf += 2;     // 2
        ((T_AUDIO_AQI_ANR_CFG *) parameter)->parameters.tone_cnt_th  = etm_get16(buf); buf += 2;     // 3
        audio.data = parameter;
        break;
#endif // (L1_ANR == 2)
#if (L1_AEC != 2)
    case AUDIO_MICROPHONE_SPEAKER_LOOP_AEC:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: AUDIO_MICROPHONE_SPEAKER_LOOP_AEC");
        aec_parameter = etm_malloc (sizeof(T_AUDIO_AEC_CFG));
        
        aec_parameter->aec_enable =                 etm_get16(buf); buf += 2;// 1
#if (L1_NEW_AEC)
        if (etm_get16(buf))  // 2
            aec_parameter->continuous_filtering = TRUE;
        else
            aec_parameter->continuous_filtering = FALSE;
        buf += 2;
        aec_parameter->granularity_attenuation =    etm_get16(buf); buf += 2;// 3
        aec_parameter->smoothing_coefficient =      etm_get16(buf); buf += 2;// 4
        aec_parameter->max_echo_suppression_level = etm_get16(buf); buf += 2;// 5
        aec_parameter->vad_factor =                 etm_get16(buf); buf += 2;// 6
        aec_parameter->absolute_threshold =         etm_get16(buf); buf += 2;// 7
        aec_parameter->factor_asd_filtering =       etm_get16(buf); buf += 2;// 8
        aec_parameter->factor_asd_muting =          etm_get16(buf); buf += 2;// 9
        aec_parameter->aec_visibility =             etm_get16(buf); buf += 2;// 10
#else
        aec_parameter->aec_mode =                   etm_get16(buf); buf += 2;// 2
        aec_parameter->echo_suppression_level =     etm_get16(buf); buf += 2;// 3
#endif // (L1_NEW_AEC)
#if (L1_ANR == 0)
        aec_parameter->noise_suppression_enable =   etm_get16(buf); buf += 2;// 4 or 11
        aec_parameter->noise_suppression_level =    etm_get16(buf); // 5 or 12
#endif // (L1_ANR == 0)
        audio.data = aec_parameter;
        break;
#else // for L1_AEC == 2
    case AUDIO_MICROPHONE_SPEAKER_LOOP_AEC:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: AUDIO_MICROPHONE_SPEAKER_LOOP_AEC (L1_AEC=2)");
        aec_parameter = etm_malloc (sizeof(T_AUDIO_AQI_AEC_CFG));

        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->aec_control     = (T_AUDIO_AEC_CONTROL) etm_get16(buf); buf += 2;  // 0
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->aec_mode        = etm_get16(buf); buf += 2;  // 1
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->mu              = etm_get16(buf); buf += 2;  // 2
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->cont_filter     = etm_get16(buf); buf += 2;  // 3
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->scale_input_ul  = etm_get16(buf); buf += 2;  // 4
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->scale_input_dl  = etm_get16(buf); buf += 2;  // 5
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->div_dmax        = etm_get16(buf); buf += 2;  // 6
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->div_swap_good   = etm_get16(buf); buf += 2;  // 7
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->div_swap_bad    = etm_get16(buf); buf += 2;  // 8
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->block_init      = etm_get16(buf); buf += 2;  // 9
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->fact_vad        = etm_get16(buf); buf += 2;  // 10
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->fact_asd_fil    = etm_get16(buf); buf += 2;  // 11
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->fact_asd_mut    = etm_get16(buf); buf += 2;  // 12
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->thrs_abs        = etm_get16(buf); buf += 2;  // 13
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->es_level_max    = etm_get16(buf); buf += 2;  // 14
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->granularity_att = etm_get16(buf); buf += 2;  // 15
        ((T_AUDIO_AQI_AEC_CFG *) aec_parameter)->coef_smooth     = etm_get16(buf); buf += 2;  // 16
        
        audio.data = aec_parameter;
        break;
#endif // L1_AEC == 2
#if (L1_ES == 1)   
    case AUDIO_MICROPHONE_ES:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: AUDIO_MICROPHONE_ES (L1_ES = 1) ");
        parameter = etm_malloc (sizeof(T_AUDIO_ES_CFG));

        ((T_AUDIO_ES_CFG *) parameter)->es_enable              = etm_get8(buf);   buf += 1;  // 1   
        ((T_AUDIO_ES_CFG *) parameter)->es_behavior            = etm_get8(buf);   buf += 1;  // 2 
                                                                                             
        ((T_AUDIO_ES_CFG *) parameter)->es_mode                = etm_get16(buf);  buf += 2;  // 3 
                                                                                             
        ((T_AUDIO_ES_CFG *) parameter)->es_gain_dl             = etm_get16(buf);  buf += 2;  // 4 
        ((T_AUDIO_ES_CFG *) parameter)->es_gain_ul_1           = etm_get16(buf);  buf += 2;  // 5 
        ((T_AUDIO_ES_CFG *) parameter)->es_gain_ul_2           = etm_get16(buf);  buf += 2;  // 6
        ((T_AUDIO_ES_CFG *) parameter)->tcl_fe_ls_thr          = etm_get16(buf);  buf += 2;  // 7 
        ((T_AUDIO_ES_CFG *) parameter)->tcl_dt_ls_thr          = etm_get16(buf);  buf += 2;  // 8 
        ((T_AUDIO_ES_CFG *) parameter)->tcl_fe_ns_thr          = etm_get16(buf);  buf += 2;  // 9 
        ((T_AUDIO_ES_CFG *) parameter)->tcl_dt_ns_thr          = etm_get16(buf);  buf += 2;  // 10
        ((T_AUDIO_ES_CFG *) parameter)->tcl_ne_thr             = etm_get16(buf);  buf += 2;  // 11
        ((T_AUDIO_ES_CFG *) parameter)->ref_ls_pwr             = etm_get16(buf);  buf += 2;  // 12
        ((T_AUDIO_ES_CFG *) parameter)->switching_time         = etm_get16(buf);  buf += 2;  // 13
        ((T_AUDIO_ES_CFG *) parameter)->switching_time_dt      = etm_get16(buf);  buf += 2;  // 14
        ((T_AUDIO_ES_CFG *) parameter)->hang_time              = etm_get16(buf);  buf += 2;  // 15
        ((T_AUDIO_ES_CFG *) parameter)->gain_lin_dl_vect[0]    = etm_get16(buf);  buf += 2;  // 16
        ((T_AUDIO_ES_CFG *) parameter)->gain_lin_dl_vect[1]    = etm_get16(buf);  buf += 2;  // 17
        ((T_AUDIO_ES_CFG *) parameter)->gain_lin_dl_vect[2]    = etm_get16(buf);  buf += 2;  // 18
        ((T_AUDIO_ES_CFG *) parameter)->gain_lin_dl_vect[3]    = etm_get16(buf);  buf += 2;  // 19
        ((T_AUDIO_ES_CFG *) parameter)->gain_lin_ul_vect[0]    = etm_get16(buf);  buf += 2;  // 20
        ((T_AUDIO_ES_CFG *) parameter)->gain_lin_ul_vect[1]    = etm_get16(buf);  buf += 2;  // 21
        ((T_AUDIO_ES_CFG *) parameter)->gain_lin_ul_vect[2]    = etm_get16(buf);  buf += 2;  // 22
        ((T_AUDIO_ES_CFG *) parameter)->gain_lin_ul_vect[3]    = etm_get16(buf);  buf += 2;  // 23

        audio.data = parameter;
        break;
#endif // end of (L1_ES)
#if (L1_AGC_UL== 1)
    case AUDIO_MICROPHONE_AGC:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: AUDIO_MICROPHONE_AGC (L1_AGC_UL = 1)");
        parameter = etm_malloc (sizeof(T_AUDIO_AQI_AGC_UL_REQ));

        ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->agc_ul_control                    = (T_AUDIO_AGC_CONTROL) etm_get16(buf);  buf += 2;    // 0
        ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.control                = etm_get16(buf);  buf += 2;    // 1
        ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.frame_size             = etm_get16(buf);  buf += 2;    // 2
        ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.targeted_level         = etm_get16(buf);  buf += 2;    // 3
        ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.signal_up              = etm_get16(buf);  buf += 2;    // 4
        ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.signal_down            = etm_get16(buf);  buf += 2;    // 5
        ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.max_scale              = etm_get16(buf);  buf += 2;    // 6
        ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.gain_smooth_alpha      = etm_get16(buf);  buf += 2;    // 7
        ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.gain_smooth_alpha_fast = etm_get16(buf);  buf += 2;    // 8
        ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.gain_smooth_beta       = etm_get16(buf);  buf += 2;    // 9
        ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.gain_smooth_beta_fast  = etm_get16(buf);  buf += 2;    // 10
        ((T_AUDIO_AQI_AGC_UL_REQ *) parameter)->parameters.gain_intp_flag         = etm_get16(buf);  buf += 2;    // 11
        
        audio.data = parameter;
        break;
#endif // (L1_AGC_UL== 1)
    case AUDIO_MICROPHONE_FIR:
    case AUDIO_SPEAKER_FIR:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: AUDIO_MICROPHONE/SPEAKER_FIR"); 
    
        parameter = etm_malloc (sizeof(T_AUDIO_FIR_COEF));
        // Write coeffient values
        for (i=0; i < (sizeof(T_AUDIO_FIR_COEF)/2); i++) {
            ((T_AUDIO_FIR_COEF *) parameter)->coefficient[i]  = etm_get16(buf);  buf += 2;
        }
        
        audio.data = parameter;
        break;
#if (L1_IIR == 1)   
    case AUDIO_SPEAKER_IIR:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: AUDIO_SPEAKER_IIR");
        parameter = etm_malloc (sizeof(T_AUDIO_IIR_CFG));
    
        ((T_AUDIO_IIR_CFG *) parameter)->iir_enable         = etm_get8(buf);   buf += 1; 
        ((T_AUDIO_IIR_CFG *) parameter)->nb_iir_blocks      = etm_get8(buf);   buf += 1;
        // Write IIF coefs
        for (i=0; i < AUDIO_IIR_MAX_IIR_BLOCKS*8; i++) {
            ((T_AUDIO_IIR_CFG *) parameter)->iir_coefs[i]   = etm_get16(buf);  buf += 2;
        }
        ((T_AUDIO_IIR_CFG *) parameter)->nb_fir_coefs       = etm_get8(buf);   buf += 1; 
        // Write FIR coefs
        for (i=0; i < AUDIO_IIR_MAX_FIR_COEFS; i++) {
            ((T_AUDIO_IIR_CFG *) parameter)->fir_coefs[i]   = etm_get16(buf);  buf += 2; 
        }
        ((T_AUDIO_IIR_CFG *) parameter)->input_scaling      = etm_get8(buf);   buf += 1; 
        ((T_AUDIO_IIR_CFG *) parameter)->fir_scaling        = etm_get8(buf);   buf += 1;
        ((T_AUDIO_IIR_CFG *) parameter)->input_gain_scaling = etm_get8(buf);   buf += 1;
        ((T_AUDIO_IIR_CFG *) parameter)->output_gain_scaling= etm_get8(buf);   buf += 1; 
        ((T_AUDIO_IIR_CFG *) parameter)->output_gain        = etm_get16(buf);  buf += 2; 
        ((T_AUDIO_IIR_CFG *) parameter)->feedback           = etm_get16(buf);  buf += 2; 
       
        audio.data = parameter;
        break;
#endif // (L1_IIR)
#if (L1_IIR == 2)
    case AUDIO_SPEAKER_IIR:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: AUDIO_SPEAKER_IIR (L1_IIR 2)"); 
        parameter = etm_malloc (sizeof(T_AUDIO_IIR_DL_CFG));

        ((T_AUDIO_IIR_DL_CFG *) parameter)->iir_dl_control        = (T_AUDIO_IIR_CONTROL) etm_get16(buf);  buf += 2;   // 1
        ((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.control    = etm_get16(buf);  buf += 2;   // 2
        ((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.frame_size = etm_get16(buf);  buf += 2;   // 3
        ((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.fir_swap   = etm_get16(buf);  buf += 2;   // 4
        // FIR parameters
        ((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.fir_filter.fir_enable = etm_get16(buf);  buf += 2;    // 5
        ((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.fir_filter.fir_length = etm_get16(buf);  buf += 2;    // 6
        ((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.fir_filter.fir_shift  = etm_get16(buf);  buf += 2;    // 7
        // Read the FIR coefs.
        for (i=0; i < AUDIO_IIR_MAX_FIR_TAPS; i++) {
            ((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.fir_filter.fir_taps[i] = etm_get16(buf);  buf += 2;  // 8 .. 47
        }
        // SOS parameters
        ((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.sos_filter.sos_enable = etm_get16(buf);  buf += 2;    // 48
        ((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.sos_filter.sos_number = etm_get16(buf);  buf += 2;    // 49

        for (i=0; i < AUDIO_IIR_MAX_IIR_BLOCKS; i++) {                                                       // 50 .. 97 
            ((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.sos_filter.sos_filter[i].sos_fact       = etm_get16(buf);  buf += 2;;    
            ((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.sos_filter.sos_filter[i].sos_fact_form  = etm_get16(buf);  buf += 2;     
            for (u=0; u < 2; u++) {
                ((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.sos_filter.sos_filter[i].sos_den[u] = etm_get16(buf);  buf += 2;   
            }
            for (v=0; v < 3; v++) {
                ((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.sos_filter.sos_filter[i].sos_num[v] = etm_get16(buf);  buf += 2;   
            }
            ((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.sos_filter.sos_filter[i].sos_num_form   = etm_get16(buf);  buf += 2;
        }

        ((T_AUDIO_IIR_DL_CFG *) parameter)->parameters.gain  = etm_get16(buf);  buf += 2;   // 98

        audio.data = parameter;
        break;
#endif // (L1_IIR == 2)
#if (L1_LIMITER == 1)
    case AUDIO_SPEAKER_LIMITER:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: AUDIO_SPEAKER_LIMITER");
        parameter = etm_malloc (sizeof(T_AUDIO_LIMITER_CFG));
    
        ((T_AUDIO_LIMITER_CFG *) parameter)->limiter_enable      = etm_get8(buf);   buf += 1; 
        ((T_AUDIO_LIMITER_CFG *) parameter)->block_size          = etm_get16(buf);  buf += 2;
        ((T_AUDIO_LIMITER_CFG *) parameter)->slope_update_period = etm_get16(buf);  buf += 2;
        ((T_AUDIO_LIMITER_CFG *) parameter)->nb_fir_coefs        = etm_get16(buf);  buf += 2;
        // Write FILTER coefs
        for (i=0; i < ((AUDIO_LIMITER_MAX_FILTER_COEFS-1)/2)+1; i++) {
            ((T_AUDIO_LIMITER_CFG *) parameter)->filter_coefs[i] = etm_get16(buf);  buf += 2;
        }
        ((T_AUDIO_LIMITER_CFG *) parameter)->thr_low_0           = etm_get16(buf);  buf += 2; 
        ((T_AUDIO_LIMITER_CFG *) parameter)->thr_low_slope       = etm_get16(buf);  buf += 2; 
        ((T_AUDIO_LIMITER_CFG *) parameter)->thr_high_0          = etm_get16(buf);  buf += 2;
        ((T_AUDIO_LIMITER_CFG *) parameter)->thr_high_slope      = etm_get16(buf);  buf += 2;
        ((T_AUDIO_LIMITER_CFG *) parameter)->gain_fall           = etm_get16(buf);  buf += 2; 
        ((T_AUDIO_LIMITER_CFG *) parameter)->gain_rise           = etm_get16(buf);  buf += 2; 
       
        audio.data = parameter;
        break;
#endif // end of (L1_LIMITER1)
#if (L1_AGC_DL == 1)
    case AUDIO_SPEAKER_AGC:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: AUDIO_SPEAKER_AGC (L1_AGC_DL=1)");
        parameter = etm_malloc (sizeof(T_AUDIO_AQI_AGC_DL_REQ));

        ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->agc_dl_control =(T_AUDIO_AGC_CONTROL) etm_get16(buf);  buf += 2;   // 1
        // Parameters
        ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.control                  = etm_get16(buf);  buf += 2;   // 2
        ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.frame_size               = etm_get16(buf);  buf += 2;   // 3
        ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.targeted_level           = etm_get16(buf);  buf += 2;   // 4
        ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.signal_up                = etm_get16(buf);  buf += 2;   // 5
        ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.signal_down              = etm_get16(buf);  buf += 2;   // 6
        ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.max_scale                = etm_get16(buf);  buf += 2;   // 7
        ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.gain_smooth_alpha        = etm_get16(buf);  buf += 2;   // 8
        ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.gain_smooth_alpha_fast   = etm_get16(buf);  buf += 2;   // 9
        ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.gain_smooth_beta         = etm_get16(buf);  buf += 2;   // 10
        ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.gain_smooth_beta_fast    = etm_get16(buf);  buf += 2;   // 11
        ((T_AUDIO_AQI_AGC_DL_REQ *) parameter)->parameters.gain_intp_flag           = etm_get16(buf);  buf += 2;   // 12

        audio.data = parameter;
        break;
#endif // (L1_AGC_DL == 1)
#if (L1_DRC == 1)
    case AUDIO_SPEAKER_DRC:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: AUDIO_XXXX_DRC");
        parameter = etm_malloc (sizeof(T_AUDIO_DRC_CFG));

        ((T_AUDIO_DRC_CFG *) parameter)->drc_dl_control       = (T_AUDIO_DRC_CONTROL) etm_get16(buf);  buf += 2;  // 1
        // Parameters
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.speech_mode_samp_f              = etm_get16(buf);  buf += 2;  // 2
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.num_subbands                    = etm_get16(buf);  buf += 2;  // 3
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.frame_len                       = etm_get16(buf);  buf += 2;  // 4
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.expansion_knee_fb_bs            = etm_get16(buf);  buf += 2;  // 5
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.expansion_knee_md_hg            = etm_get16(buf);  buf += 2;  // 6
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.expansion_ratio_fb_bs           = etm_get16(buf);  buf += 2;  // 7
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.expansion_ratio_md_hg           = etm_get16(buf);  buf += 2;  // 8
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.max_amplification_fb_bs         = etm_get16(buf);  buf += 2;  // 9
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.max_amplification_md_hg         = etm_get16(buf);  buf += 2;  // 10
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.compression_knee_fb_bs          = etm_get16(buf);  buf += 2;  // 11
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.compression_knee_md_hg          = etm_get16(buf);  buf += 2;  // 12
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.compression_ratio_fb_bs         = etm_get16(buf);  buf += 2;  // 13
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.compression_ratio_md_hg         = etm_get16(buf);  buf += 2;  // 14
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.energy_limiting_th_fb_bs        = etm_get16(buf);  buf += 2;  // 15
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.energy_limiting_th_md_hg        = etm_get16(buf);  buf += 2;  // 16
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.limiter_threshold_fb            = etm_get16(buf);  buf += 2;  // 17
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.limiter_threshold_bs            = etm_get16(buf);  buf += 2;  // 18
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.limiter_threshold_md            = etm_get16(buf);  buf += 2;  // 19
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.limiter_threshold_hg            = etm_get16(buf);  buf += 2;  // 20
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.limiter_hangover_spect_preserve = etm_get16(buf);  buf += 2;  // 21
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.limiter_release_fb_bs           = etm_get16(buf);  buf += 2;  // 22
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.limiter_release_md_hg           = etm_get16(buf);  buf += 2;  // 23
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.gain_track_fb_bs                = etm_get16(buf);  buf += 2;  // 24
        ((T_AUDIO_DRC_CFG *) parameter)->parameters.gain_track_md_hg                = etm_get16(buf);  buf += 2;  // 25
        for (i=0; i < 17; i++) {
            ((T_AUDIO_DRC_CFG *) parameter)->parameters.low_pass_filter[i]          = etm_get16(buf);  buf += 2;  // 26
        }
        for (i=0; i < 17; i++) {
            ((T_AUDIO_DRC_CFG *) parameter)->parameters.mid_band_filter[i]          = etm_get16(buf);  buf += 2;  // 27
        }
        audio.data = parameter;
        break;
#endif // (L1_DRC == 1)
#if (L1_WCM == 3)
    case XXXX_WCM:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: AUDIO_XXXX_WCM");
        parameter = etm_malloc (sizeof(T_AUDIO_AQI_WCM_REQ));

        ((T_AUDIO_AQI_WCM_REQ *) parameter)->wcm_control                = etm_get16(buf);  buf += 2;  // 1
        // Parameters
        ((T_AUDIO_AQI_WCM_REQ *) parameter)->parameters.mode            = etm_get16(buf);  buf += 2;  // 2
        ((T_AUDIO_AQI_WCM_REQ *) parameter)->parameters.frame_size      = etm_get16(buf);  buf += 2;  // 3
        ((T_AUDIO_AQI_WCM_REQ *) parameter)->parameters.num_sub_frames  = etm_get16(buf);  buf += 2;  // 4
        ((T_AUDIO_AQI_WCM_REQ *) parameter)->parameters.ratio           = etm_get16(buf);  buf += 2;  // 5
        ((T_AUDIO_AQI_WCM_REQ *) parameter)->parameters.threshold       = etm_get16(buf);  buf += 2;  // 6
        for (i=0; i < 16; i++) {  // FixMe size must be a variable
            ((T_AUDIO_AQI_WCM_REQ *) parameter)->parameters.gain[i]     = etm_get16(buf);  buf += 2;  // 7
        }
        audio.data = parameter;
        break;
#endif //  (L1_WCM == 1)
    /* default:
        result = ETM_INVAL; 
        goto etm_audio_write_end; */
    } // end of switch

    if ((result = audio_full_access_write(&audio, return_path)) != AUDIO_OK) {
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: ERROR(%d)", result);
        if (result == AUDIO_ERROR) 
            result = ETM_INVAL;         // Invalid audio parameter
        else
            result = ETM_AUDIO_FATAL;
    }

// Wait for recv. of event: AUDIO_FULL_ACCESS_WRITE_DONE
    rvf_wait(0xffff, 100); // Timeout 100 ticks
    tr_etm(TgTrAudio, "ETM AUDIO: _audio_write: STATUS(%d)", etm_audio_event_status);

    if (parameter != NULL) {
        etm_free(parameter);
        parameter = NULL;
    }

    if (aec_parameter != NULL) {
        etm_free(aec_parameter);    
        aec_parameter = NULL;
    }

    if (etm_audio_event_status != 0) {
        etm_audio_event_status = 0;
        result = ETM_AUDIO_FATAL;
    }

etm_audio_write_end:
#endif	
    return result;
}


int etm_audio_saveload(T_ETM_PKT *pkt, uint8 saveload, void *buf, int size)
{
    int result = ETM_OK; 
#if(PSP_STANDALONE==0)  /*making it a dummy function if its a PSP standalone build */
    T_RV_HDR *msg;
    T_AUDIO_MODE_SAVE audio_s;
    T_AUDIO_MODE_LOAD audio_l;
    T_RV_RETURN return_path;
    int error, event;
    char temp[80];
    
    return_path.addr_id        = etm_env_ctrl_blk->addr_id;
    return_path.callback_func  = NULL;

    switch(saveload) {
    case 'S':
#if(AS_RFS_API == 1)
        memcpy(temp, buf, size);
        convert_u8_to_unicode(temp, audio_s.audio_mode_filename);
        
#else
        memcpy(audio_s.audio_mode_filename, buf, size);
#endif

        result = audio_mode_save(&audio_s, return_path); 
        break;
    case 'L':
#if(AS_RFS_API == 1)
        memcpy(temp, buf, size);
        convert_u8_to_unicode(temp, audio_l.audio_mode_filename);
        
#else
        memcpy(audio_l.audio_mode_filename, buf, size);
#endif


        result = audio_mode_load(&audio_l, return_path); 
        break;
    default:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_saveload: FAILED"); 
        break;
    }
    
    rvf_wait(0xffff, 100); // Timeout 100 ticks
    tr_etm(TgTrAudio, "ETM AUDIO: _audio_saveload: STATUS(%d)", etm_audio_event_status);

    if (etm_audio_event_status != 0) {
        etm_audio_event_status = 0;
        return ETM_AUDIO_FATAL;
    }

    if (result != AUDIO_OK)
        return ETM_AUDIO_FATAL;
#endif
    return result;
}

/******************************************************************************
 * ETM AUDIO Moudle - Main Task
 *****************************************************************************/
#if(PSP_STANDALONE==0)
int etm_audio_tones(UINT8 *buf)
{
#if (TONE == 1)
    #define MAX_NUMBER_OF_TONES 3
    UINT8 i;
    int result = ETM_OK;
    T_AUDIO_TONES_PARAMETER parameter;
    T_ETM_PKT *pkt = NULL;  
//    parameter = etm_malloc(sizeof(T_AUDIO_TONES_PARAMETER));

    //Return path from audio entity
    T_RV_RETURN return_path;
    return_path.addr_id        = NULL; //etm_env_ctrl_blk->addr_id;
    return_path.callback_func  = etm_audio_callback;

    //This typecasting and the & operator is not neccesary, clean it up when you have seen this working
    for(i=0; i<MAX_NUMBER_OF_TONES; ++i)
    {
        ((T_AUDIO_TONES_PARAMETER *) &parameter)->tones[i].start_tone     = etm_get16(buf); buf += 2;
        ((T_AUDIO_TONES_PARAMETER *) &parameter)->tones[i].stop_tone      = etm_get16(buf); buf += 2;
        ((T_AUDIO_TONES_PARAMETER *) &parameter)->tones[i].frequency_tone = etm_get16(buf); buf += 2;
        ((T_AUDIO_TONES_PARAMETER *) &parameter)->tones[i].amplitude_tone = etm_get8(buf);  buf += 1;
    }

    ((T_AUDIO_TONES_PARAMETER *) &parameter)->frame_duration    = etm_get16(buf); buf += 2;
    ((T_AUDIO_TONES_PARAMETER *) &parameter)->sequence_duration = etm_get16(buf); buf += 2;
    ((T_AUDIO_TONES_PARAMETER *) &parameter)->period_duration   = etm_get16(buf); buf += 2;
    ((T_AUDIO_TONES_PARAMETER *) &parameter)->repetition        = etm_get16(buf); buf += 2;

    if((result = audio_tones_start (&parameter, return_path)) < 0)
        result = ETM_NOSYS;

    return result;
#else
    tr_etm(TgTrAudio, "Tones not compiled in.");
    return ETM_NOSYS;
#endif
}
#endif //PSP_STANDALONE=0


/******************************************************************************
 * ETM AUDIO callback functio
 *****************************************************************************/

void etm_audio_callback(void *event_from_audio)
{
#if(PSP_STANDALONE==0)
    tr_etm(TgTrEtmLow,"ETM: AUDIO: _audio_callback: recv. event (0x%x)", 
           ((T_RV_HDR *) event_from_audio)->msg_id);
   
    switch (((T_RV_HDR *) event_from_audio)->msg_id)
    {
    case AUDIO_FULL_ACCESS_WRITE_DONE:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_callback: recv. event AUDIO_FULL_ACCESS_WRITE_DONE");
        etm_audio_event_status = ((T_AUDIO_FULL_ACCESS_WRITE_DONE *) event_from_audio)->status;
        break;
    case AUDIO_MODE_SAVE_DONE:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_callback: recv. event AUDIO_MODE_SAVE_DONE");
        etm_audio_event_status = ((T_AUDIO_SAVE_DONE *) event_from_audio)->status;
        break;
    case AUDIO_MODE_LOAD_DONE:
        tr_etm(TgTrAudio, "ETM AUDIO: _audio_callback: recv. event AUDIO_MODE_LOAD_DONE");
        etm_audio_event_status = ((T_AUDIO_LOAD_DONE *) event_from_audio)->status;
        break;
    }

    if (event_from_audio != NULL) {
//        etm_free(event_from_audio); // Hmmm...
        event_from_audio = NULL;
    }
#endif	
}


/******************************************************************************
 * ETM AUDIO Moudle - Main Task
 *****************************************************************************/

// AUDIO packet structure for audio read/write and codec read/write: 
// |fid(8)|param(8)|--data(W)--| and for audio save/load |fid|--data(W)--|

int etm_audio(uint8 *indata, int insize)
{
    int error = ETM_OK;
    uint8 fid;
    T_ETM_PKT *pkt = NULL;  

    fid = *indata++;   
       
    tr_etm(TgTrAudio, "ETM AUDIO: _audio: fid(%c) param(%d) recv. size(%d)", 
           fid, *indata, insize); 

    /* Create TestMode return Packet */
    if ((pkt = (T_ETM_PKT *) etm_malloc(sizeof(T_ETM_PKT))) == NULL) {
        return ETM_NOMEM;
    }
    
    // Init. of return packet
    pkt->mid     = ETM_AUDIO;
    pkt->status  = ETM_OK;
    pkt->size    = 0;
    pkt->index   = 0;
    etm_pkt_put8(pkt, fid);

    if (error == ETM_OK) {
        switch (fid) {
        case 'R': //Read
            error = etm_audio_read(pkt, indata);                  
            break;
        case 'W': //Write
            error = etm_audio_write(pkt, indata); 
            break;
        case 'S': //Save
        case 'L': //Load
            error = etm_audio_saveload(pkt, fid, indata, insize);  
            break;
        case 'T': //Tone generator
	    #if(PSP_STANDALONE==0)
            error = etm_audio_tones(indata);
            #endif
            break;
            
        default:
            tr_etm(TgTrAudio, "ETM AUDIO: _audio: fid(%c) - ERROR ", fid);
            error = ETM_NOSYS;                                
            break;
        }
    }
    
    if (error < 0) {
        tr_etm(TgTrAudio,"ETM AUDIO: _audio: ERROR(%d)", error); 
        pkt->status = -error;
    }

    etm_pkt_send(pkt);
    etm_free(pkt);

    return ETM_OK;
}

