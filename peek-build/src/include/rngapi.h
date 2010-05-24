#ifndef _RNGAPI_H_
#define _RNGAPI_H_


#include "sysapi.h"
#include "sysdefs.h"

#include "hwddefs.h"

#define RNG_MAILBOX      EXE_MAILBOX_1_ID    /* Command Mailbox Id          */

/* At voice codec's 8 KHz rate, there are 160 PCM samples in 20 ms */
#define RNG_NUM_SAMPLES_IN_20MS 160

typedef enum
{
   RNG_START_MSG,
   RNG_STOP_MSG
   
}RngCmdIdT;

/*****************************************************************************
 
  FUNCTION NAME: RngSamplesPut 

  DESCRIPTION:  Passes a pointer to the next group of samples to be output.   

  PARAMETERS:  SampleP - Pointer to next group of samples.
               NumSamples - Number of samples (should be 20 ms worth).

  RETURNED VALUES: None

  NOTE: It is intended that this function be called every 20 ms while 
        sample output is in progress.  Samples starting at the input pointer
        are passed to the DSPV for output on the next 20 ms period.
        
*****************************************************************************/
extern void RngSamplesPut( uint16 *SampleP, uint16 NumSamples );




#endif

