/******************************************************************************
 *                   WIRELESS COMMUNICATION SYSTEM DEVELOPMENT
 *
 *             (C) 2005 Texas Instruments France. All rights reserved
 *
 *                          Author : Mary  TOOHER
 *
 *
 *  Important Note
 *  --------------
 *
 *  The S/W is furnished under Non Disclosure Agreement and may be used or
 *  copied only in accordance with the terms of the agreement. It is an offence
 *  to copy the software in any way except as specifically set out in the 
 *  agreement. No part of this document may be reproduced or transmitted in any
 *  form or by any means, electronic or mechanical, including photocopying and
 *  recording, for any purpose without the express written permission of Texas
 *  Instruments Inc.
 *
 ******************************************************************************
 *
 *  FILE NAME: bspTwl3029_pgx_Audio.h
 *
 *
 *  PURPOSE:  
 *    This module defines tydefs, macros and constants for specific use with
 *    hardware twl3029 PG1.0 or PG1.1.
 *
 *  FILE REFERENCES:
 *
 *  Name                  IO      Description
 *  -------------         --      ---------------------------------------------
 *  
 *
 *
 *  EXTERNAL VARIABLES:
 *
 *  Source:
 *
 *  Name                  Type              IO   Description
 *  -------------         ---------------   --   ------------------------------
 *
 *  
 *
 *
 *  EXTERNAL REFERENCES:
 *
 *  Name                Description
 *  ------------------  -------------------------------------------------------
 *
 *
 *
 *  ABNORMAL TERMINATION CONDITIONS, ERROR AND WARNING MESSAGES:
 *  
 *
 *
 *  ASSUMPTION, CONSTRAINTS, RESTRICTIONS:
 *  
 *
 *
 *  NOTES:
 *  
 *
 *
 *  REQUIREMENTS/FUNCTIONAL SPECIFICATION REFERENCES:
 *
 *
 *
 *
 *  DEVELOPMENT HISTORY:
 *
 *  Date        Name(s)         Version  Description
 *  ----------  --------------  -------  --------------------------------------
 *  04/05/2005  Mary Tooher     V1.0.0   First implementation
 */
#ifndef BSP_TWL3029_PG2X_AUDIO_HEADER
#define BSP_TWL3029_PG2X_AUDIO_HEADER 
 /*******************************************************************************
 *   includes
 */
 
#include "types.h"
 //#include "main_system.h"
//#include "bspTwl3029.h"
//#include "bspTwl3029_I2c.h"
//#include "bspI2c.h"

//#include "main_Platform.h"


//#include "bspUtil_Assert.h"



/*=============================================================================
 *  Defines and Macros
 */

/*============================================================================*/
/*!
 * @constant
 *
 * @discussion 
 * <b> Description </b><br>
 *     These are some default values for all regs for initialization. 
 */ 
#define   BSP_TWL_3029_AUDIO_CTRL5_DEFAULT     0x00   /*  AUDIO:  CTRL5   HOOKEN hook detect disabled */

#define   BSP_TWL_3029_AUDIO_POPMAN_DEFAULT    0x2A   /*  AUDIO:  POPMAN  */  /*enable all post discharge except CARKIT */
#define   BSP_TWL_3029_AUDIO_POPAUTO_DEFAULT   0x07   /*  AUDIO:  POPAUTO : auto ON (except carkit)*/
/*Enable ALL*/
#define   BSP_TWL_3029_MAP_AUDIO_POPMAN_ENABLE 0xFF
/*===========================================================================*/
/*!
 * @typedef BspTwl3029_AudioI2sVoiceUplinkState
 *
 * <b> Description  </b><br>
 *    defines if i2s power on (I2SON) is enabled or disabled. 
 *    and also VMEMO bit ( which can be used for voice memo applications)
 *    Note the stereo power on 
 *    STON bit in PWRONSTATUS must be active for this to take effect.
 *
 *  @constant BSP_TWL3029_AUDIO_STEREO_I2S_POWER_OFF
 *
 *  @constant BSP_TWL3029_AUDIO_STEREO_I2S_POWER_ON_VMEMO_OFF
 *
 *  @constant BSP_TWL3029_AUDIO_STEREO_I2S_POWER_ON_VMEMO_ON
 *
 *  Note: I2S power needs to be ON for VMEMO to function
 *        VMEMO can only be used with  8kHz and 16kHz frequency
 */
enum
{ 
    BSP_TWL3029_AUDIO_STEREO_I2S_POWER_OFF,
    BSP_TWL3029_AUDIO_STEREO_I2S_POWER_ON_VMEMO_OFF_RJ = 5,
    BSP_TWL3029_AUDIO_STEREO_I2S_POWER_ON_VMEMO_OFF_LJ = 13,
    BSP_TWL3029_AUDIO_STEREO_I2S_POWER_ON_VMEMO_ON_RJ = 7,
    BSP_TWL3029_AUDIO_STEREO_I2S_POWER_ON_VMEMO_ON_LJ = 15
};
typedef Uint8 BspTwl3029_AudioI2sVoiceUplinkState ;  

#define BSP_TWL3029_AUDIO_STEREO_I2S_POWER_ON_VMEMO_OFF 5   /* default to RJ : BSP_TWL3029_AUDIO_STEREO_I2S_POWER_ON_VMEMO_OFF_RJ */
#define BSP_TWL3029_AUDIO_STEREO_I2S_POWER_ON_VMEMO_ON  7  /* BSP_TWL3029_AUDIO_STEREO_I2S_POWER_ON_VMEMO_ON_RJ*/
#define BSP_TWL3029_AUDIO_STEREO_I2S_MAX 15


#endif
