/*                @(#) nom : sys_conf_dsp_int.h SID: 1.1 date : 04/02/03      */
/* Filename:      sys_conf_dsp_int.h                                          */
/* Version:       1.1                                                         */
/******************************************************************************
 *                   WIRELESS COMMUNICATION SYSTEM DEVELOPMENT
 *
 *             (C) 2002 Texas Instruments France. All rights reserved
 *
 *                          Author : Francois AMAND
 *
 *
 *  Important Note
 *  --------------
 *
 *  This S/W is a preliminary version. It contains information on a product 
 *  under development and is issued for evaluation purposes only. Features 
 *  characteristics, data and other information are subject to change.
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
 *  FILE NAME: sys_conf_dsp_int.h
 *
 *
 *  PURPOSE:  Include file to configure CONF CORE module of CALYPSO PLUS.
 *
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
 *  Date         Name(s)         Version  Description
 *  -----------  --------------  -------  -------------------------------------
 *  2-april-2003  G.Leterrier    0.0.1    First implementation
 *
 *  ALGORITHM: 
 *
 *
 *****************************************************************************/

  #if (CHIPSET != 15)
 
#include "chipset.cfg"
#endif
 
  #if (CHIPSET == 12)
      
    
    /****************************************************************************
     *                            CONSTANT DEFINITION
     ***************************************************************************/
  
    /*
     *  DSP INTERRUPT  DEFINITION
     */
      #define C_DBG_DSP_INT_RIF_RX                 0
      #define C_DBG_DSP_INT_RIF_TX                 1
      #define C_DBG_DSP_INT_UART                   2
      #define C_DBG_DSP_INT_MCSI1_RX_MCSI2_TX      3
      #define C_DBG_DSP_INT_MCSI1_TX_MCSI2_RX      4
      #define C_DBG_DSP_INT_MCSI_FRAME             5
      #define C_DBG_DSP_INT_MCSI_DAI_CPORT_TX_RX   6
      #define C_DBG_DSP_INT_CYPHER                 7
      #define C_DBG_DSP_INT_TPU_FRAME              8
      #define C_DBG_DSP_INT_TPU_PROG_INT           9
      #define C_DBG_DSP_INT_DMA                    10
      #define C_DBG_DSP_INT_EXT_INT                11



    /****************************************************************************
     *                            MACRO DEFINITION
     ***************************************************************************/

    /****************************************************************************
     *                            STRUCTURE DEFINITION
     ***************************************************************************/

    /****************************************************************************
     *                            PROTOTYPE DEFINITION
     ***************************************************************************/
    
 
  #endif /* CHIPSET == 12 */



