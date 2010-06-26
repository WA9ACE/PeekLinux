/****************************************************************************/
/*                                                                          */
/*  File Name:  dar_watchdog.c                                              */
/*                                                                          */
/*  Purpose:   This function contains the DAR watchdog functions            */
/*                                                                          */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date                 Modification                                       */
/*  ------------------------------------                                    */
/*  8 November 2001    Create                                               */
/*                                                                          */
/*  Author     Stephanie Gerthoux                                           */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"
#ifdef RVM_DAR_SWE
   #ifndef _WINDOWS
      #include "swconfig.cfg"
      #include "sys.cfg"
      #include "chipset.cfg"
   #endif

   #include "rv/rv_general.h"
   #include "rvm/rvm_gen.h"
   #include "rvm/rvm_priorities.h"
   #include "dar/dar_api.h"
   #include "dar/dar_gen.h"
   #include "dar/dar_diagnose_i.h"

   /**** Global variable ****/

   /* Get the dar_current status */
   extern T_DAR_RECOVERY_STATUS dar_current_status;

   /********************************************************************************/
   /*                                                                              */
   /*    Function Name:   dar_watchdog_reset                                       */
   /*                                                                              */
   /*    Purpose: This function is called to reset the system when the general     */
   /*             general purpose timer expires                                    */
   /*                                                                              */
   /*    Input Parameters:                                                         */
   /*        Pointer to the message to store                                       */
   /*        Data Format,                                                          */
   /*        Data level,                                                           */
   /*        Data Use Id,                                                          */
   /*                                                                              */
   /*    Output Parameters:                                                        */
   /*        Validation of the function execution.                                 */
   /*                                                                              */
   /*    Note:                                                                     */
   /*        None                                                                  */
   /*                                                                              */
   /********************************************************************************/

   void dar_watchdog_reset(void)
   {
      /* Update the current status */
      dar_current_status = DAR_WATCHDOG;

      /* reset the system */
      dar_reset(); 
 
   } /* dar_watchdog_reset */


#endif /* #ifdef RVM_DAR_SWE */
