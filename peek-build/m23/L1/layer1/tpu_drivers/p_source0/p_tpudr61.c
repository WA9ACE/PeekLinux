/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 *
 *        Filename p_tpudr12.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#include "l1_macro.h"
#include "l1_confg.h"

#if L1_GPRS

#include "sys_types.h"
#include "iq.h"
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

  #if (L1_AAC == 1)
    #include "l1aac_defty.h"
  #endif
#include "l1_defty.h"
#include "l1_time.h"
#include "tpudrv.h"
#include "tpudrv61.h"
#include "Armio.h"

#if (L1_RF_KBD_FIX == 1)
#include "l1_varex.h"
#endif


// external function prototypes
#if (L1_RF_KBD_FIX == 1)
#if (L1_MADC_ON == 1)
void l1dmacro_rx_up (UWORD8 adc_active, UWORD8 csf_filter_choice, UWORD8 kbd_config 
										#if(NEW_SNR_THRESHOLD==1)
											, UWORD8 saic_flag_rx_up
										#endif
										);
#else
void l1dmacro_rx_up (UWORD8 csf_filter_choice, UWORD8 kbd_config 
										#if(NEW_SNR_THRESHOLD==1)
											, UWORD8 saic_flag_rx_up
										#endif
										);
#endif
#endif/*(L1_RF_KBD_FIX == 1)*/

#if (L1_RF_KBD_FIX == 0)
#if (L1_MADC_ON == 1)
void l1dmacro_rx_up (UWORD8 adc_active, UWORD8 csf_filter_choice 
										#if(NEW_SNR_THRESHOLD==1)
											, UWORD8 saic_flag_rx_up
										#endif
										);
#else
void l1dmacro_rx_up (UWORD8 csf_filter_choice 
										#if(NEW_SNR_THRESHOLD==1)
											, UWORD8 saic_flag_rx_up
										#endif
										);
#endif
#endif/*(L1_RF_KBD_FIX == 0)*/


void l1dmacro_rx_down (WORD32 t);
#if (L1_RF_KBD_FIX == 1)
void l1dmacro_tx_up (UWORD8 kbd_config);
void l1dmacro_tx_down (WORD32 time, BOOL tx_flag, UWORD8  adc_active, UWORD8 kbd_config);
#endif/*#if (L1_RF_KBD_FIX == 1)*/

#if (L1_RF_KBD_FIX == 0)
void l1dmacro_tx_up (void);
void l1dmacro_tx_down (WORD32 time, BOOL tx_flag, UWORD8  adc_active);
#endif/*(L1_RF_KBD_FIX == 0)*/

// external variables and tables
extern SYS_UWORD16      *TP_Ptr;
//extern UWORD16 total_kbd_on_time;


/**************************************************************************/
/**************************************************************************/
/*                    EXTERNAL FUNCTIONS CALLED BY LAYER1                 */
/*                          COMMON TO L1 and TOOLKIT                      */
/**************************************************************************/
/**************************************************************************/

/*------------------------------------------*/
/*            l1dmacro_tx_synth             */
/*------------------------------------------*/
/*      programs RF synth for transmit      */
/*      programs OPLL for transmit          */
/*------------------------------------------*/
void l1pdmacro_tx_synth(SYS_UWORD16 radio_freq)
{
  l1dmacro_tx_synth(radio_freq);
}

/*------------------------------------------*/
/*            l1pdmacro_rx_up               */
/*------------------------------------------*/
/* Open window for normal burst reception   */
/*------------------------------------------*/
#if(L1_RF_KBD_FIX == 1)

#if (L1_MADC_ON == 1)
void l1pdmacro_rx_up (SYS_UWORD16 radio_freq,UWORD8 adc_active, UWORD8 csf_filter_choice
										#if(NEW_SNR_THRESHOLD==1)
											, UWORD8 saic_flag_rx_up
										#endif
										)
{
   l1dmacro_rx_up(adc_active, csf_filter_choice, L1_KBD_DIS_RX_NB 
										#if(NEW_SNR_THRESHOLD==1)
											, saic_flag_rx_up
										#endif
										);
}
#else
void l1pdmacro_rx_up (SYS_UWORD16 radio_freq, UWORD8 csf_filter_choice 
										#if(NEW_SNR_THRESHOLD==1)
											, UWORD8 saic_flag_rx_up
										#endif
										)
{
    l1dmacro_rx_up(csf_filter_choice, L1_KBD_DIS_RX_NB
										#if(NEW_SNR_THRESHOLD==1)
											, saic_flag_rx_up
										#endif
										);

}
#endif

#endif /*#if(L1_RF_KBD_FIX == 1)*/

#if(L1_RF_KBD_FIX == 0)
#if (L1_MADC_ON == 1)
void l1pdmacro_rx_up (SYS_UWORD16 radio_freq,UWORD8 adc_active, UWORD8 csf_filter_choice
										#if(NEW_SNR_THRESHOLD==1)
											, UWORD8 saic_flag_rx_up
										#endif
										)
{
   l1dmacro_rx_up(adc_active, csf_filter_choice 
								#if(NEW_SNR_THRESHOLD==1)
									, saic_flag_rx_up
								#endif
								);
}
#else
void l1pdmacro_rx_up (SYS_UWORD16 radio_freq, UWORD8 csf_filter_choice
										#if(NEW_SNR_THRESHOLD==1)
											, UWORD8 saic_flag_rx_up
										#endif
										)
{
    l1dmacro_rx_up(csf_filter_choice
						#if(NEW_SNR_THRESHOLD==1)
							, saic_flag_rx_up
						#endif
						);

}
#endif


#endif/*#if(L1_RF_KBD_FIX == 0)*/


/*------------------------------------------*/
/*            l1pdmacro_rx_down             */
/*------------------------------------------*/
/* Close window for normal burst reception  */
/*------------------------------------------*/
#if(L1_RF_KBD_FIX == 1)

void l1pdmacro_rx_down (SYS_UWORD16 radio_freq, UWORD8 num_rx, BOOL rx_done_flag)
{
  l1dmacro_rx_down (RX_DOWN_TABLE[num_rx - 1]);
  l1s.total_kbd_on_time = l1s.total_kbd_on_time - L1_KBD_DIS_RX_NB * (-TRF_R3_1 + RX_DOWN_TABLE[num_rx - 1] - TRF_R7);
}
#endif

#if(L1_RF_KBD_FIX == 0)

void l1pdmacro_rx_down (SYS_UWORD16 radio_freq, UWORD8 num_rx, BOOL rx_done_flag)
{
  l1dmacro_rx_down (RX_DOWN_TABLE[num_rx - 1]);

}

#endif
/*------------------------------------------*/
/*            l1pdmacro_tx_up               */
/*------------------------------------------*/
/* Open transmission window for normal burst*/
/*------------------------------------------*/
#if(L1_RF_KBD_FIX == 1)

void l1pdmacro_tx_up (SYS_UWORD16 radio_freq)
{
  l1dmacro_tx_up(L1_KBD_DIS_TX_NB);
}
#endif

#if(L1_RF_KBD_FIX == 0)
void l1pdmacro_tx_up (SYS_UWORD16 radio_freq)
{
  l1dmacro_tx_up();
}

#endif


/*-------------------------------------------*/
/*            l1pdmacro_tx_down              */
/*-------------------------------------------*/
/* Close transmission window for normal burst*/
/*-------------------------------------------*/
#if(L1_RF_KBD_FIX == 1)

void l1pdmacro_tx_down (SYS_UWORD16 radio_freq, WORD16 time, BOOL tx_flag, UWORD8 timing_advance,UWORD8  adc_active)
{
  l1dmacro_tx_down (time, tx_flag, adc_active, L1_KBD_DIS_TX_NB);
  l1s.total_kbd_on_time = l1s.total_kbd_on_time - L1_KBD_DIS_TX_NB * (-TRF_T3_1 + time + TRF_T12);
}
#endif

#if(L1_RF_KBD_FIX == 0)
void l1pdmacro_tx_down (SYS_UWORD16 radio_freq, WORD16 time, BOOL tx_flag, UWORD8 timing_advance,UWORD8  adc_active)
{
  l1dmacro_tx_down (time, tx_flag, adc_active);

}
#endif

/*---------------------------------------------*/
/*       l1pdmacro_it_dsp_gen                  */
/*---------------------------------------------*/
/*       Generate IT to DSP                    */
/*---------------------------------------------*/
void l1pdmacro_it_dsp_gen(UWORD16 time)
{
  // WARNING: 'time' must always be included between 0 and TPU_CLOCK_RANGE !!!

  *TP_Ptr++ = TPU_FAT   (time);
  *TP_Ptr++ = TPU_MOVE  (TPU_IT_DSP_PG,0x0001);
}

// TEMPORARY !!!!!

/*---------------------------------------------*/
/*       l1pdmacro_anchor                      */
/*---------------------------------------------*/
/*  Temporary macro used to program a TPU      */
/* scenario executed on the correct frame      */
/*---------------------------------------------*/
void l1pdmacro_anchor(WORD16 time)
{
  // WARNING: 'time' must always be included between 0 and TPU_CLOCK_RANGE !!!

   *TP_Ptr++ = TPU_FAT    (time);
}

#endif
