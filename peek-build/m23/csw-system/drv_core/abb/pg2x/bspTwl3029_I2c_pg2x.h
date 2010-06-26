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
 *  FILE NAME: bspTwl3029_I2c_pg2x.h
 *
 *
 *  PURPOSE:  
 *    This module defines tydefs, macros and constants for specific use with
 *    hardware twl3029 PG2.0
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
#ifndef BSP_TWL3029_I2C_PG2X_HEADER
#define BSP_TWL3029_I2C_PG2X_HEADER

//#include "main_system.h"
#include "types.h"

//#include "bspTwl3029_I2c.h"

/*==============================================================================
 * File Contents:
 *   This module contains function definitions and macros 
 *   associated with Triton ABB.
 */

/*===========================================================================
 * Defines and Macros
 */
 
/*
 * typedef bspTwl3029_Reg
 *
 * Description: Used for Triton shadow registers.  Note: only 3 pages are define
 *             even though hardware actually has 4. PG1.x harware only uses 2,
 *             and harware PG2.0 uses 3)
 *
 */

typedef struct 
{
   Uint8 page0[256];
   Uint8 page1[256];
   Uint8 page2[256];
} bspTwl3029_Reg;

//typedef Uint8[3][256] bspTwl3029_Reg;

 
/* use the following defines to identify the pages 
 * ( see struct spTwl3029_I2C_TypeId below )
 */
#define BSP_TWL3029_I2C_CKG  (BSP_TWL3029_I2C_PAGE0)
#define BSP_TWL3029_I2C_INT2 (BSP_TWL3029_I2C_PAGE0)
#define BSP_TWL3029_I2C_AUD  (BSP_TWL3029_I2C_PAGE0)
#define BSP_TWL3029_I2C_AUX  (BSP_TWL3029_I2C_PAGE0)
#define BSP_TWL3029_I2C_USB  (BSP_TWL3029_I2C_PAGE2)
#define BSP_TWL3029_I2C_MADC (BSP_TWL3029_I2C_PAGE0)
#define BSP_TWL3029_I2C_VIB  (BSP_TWL3029_I2C_PAGE0)
#define BSP_TWL3029_I2C_WLED (BSP_TWL3029_I2C_PAGE0)
#define BSP_TWL3029_I2C_BCI  (BSP_TWL3029_I2C_PAGE0)
#define BSP_TWL3029_I2C_SIM  (BSP_TWL3029_I2C_PAGE0)
#define BSP_TWL3029_I2C_RTC  (BSP_TWL3029_I2C_PAGE1)
#define BSP_TWL3029_I2C_PMC_MASTER (BSP_TWL3029_I2C_PAGE1)
#define BSP_TWL3029_I2C_PMC_SLAVE  (BSP_TWL3029_I2C_PAGE1)

#define BSP_TWL3029_I2C_PAGEMAX (BSP_TWL3029_I2C_PAGE2)
                             
#endif
