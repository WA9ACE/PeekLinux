/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1AAC_FUNC.C
 *
 *        Filename l1aac_func.c
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#include "l1_confg.h"
#include "l1_types.h"
#include "l1_const.h"
#include "l1aac_const.h"

#if (L1_AAC == 1)

/*-------------------------------------------------------*/
/* l1aac_get_frequencyrate()                             */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : Sampling frequency index                 */
/*                                                       */
/* Return     : Frequency rate in Hertz                  */
/*                                                       */
/* Description : Return the frequency rate of AAC file   */
/*                                                       */
/*-------------------------------------------------------*/
UWORD16 l1aac_get_frequencyrate(UWORD16 frequency_index)
{
  UWORD16 sampling_frequency=0;

  switch(frequency_index)
  {
    case 3:
      sampling_frequency=48000;
      break;
    case 4:
      sampling_frequency=44100;
      break;
    case 5:
      sampling_frequency=32000;
      break;
    case 6:
      sampling_frequency=24000;
      break;
    case 7:
      sampling_frequency=22050;
      break;
    case 8:
      sampling_frequency=16000;
      break;
    case 9:
      sampling_frequency=12000;
      break;
    case 10:
      sampling_frequency=11025;
      break;
    case 11:
      sampling_frequency=8000;
      break;
  }

  // Sampling frequency should never be zero
  return sampling_frequency;
}


#endif    // L1_AAC
