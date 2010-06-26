/*                @(#) nom : certificate.c SID: 1.2 date : 02/17/03                           */
/* Filename:      certificate.c                                                         */
/* Version:       1.2                                                         */
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
 *  FILE NAME: firmware.c
 *
 *
 *  PURPOSE:  Simulation of Firmware application located in FLASH.
 *            Used to generate COFF file with certificate management.
 *
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
 *
 *
 *  ALGORITHM:
 *
 *
 *****************************************************************************/

#include "board.cfg"
#include "chipset.cfg"

#include "sys_types.h"
#include "security/secure_types.h"

extern void ResetVector(void);
#pragma DATA_SECTION(d_firmware_certificate, ".cert")

#if (CHIPSET == 12)
volatile const T_MANUFACTURER_CERTIFICATE_PLATFORM_DATA d_firmware_certificate =
    {
      (UWORD16)sizeof(T_MANUFACTURER_CERTIFICATE_PLATFORM_DATA), // Size of Certificate
      (UWORD8)C_CERTTYPE_MAN,       // Type of Certificate
      (UWORD8)C_DEBUGREQUEST,       // Confidentiality Request
      (UWORD32)0x00000000L,         // Address of Code
      (UWORD32)0x00000000L,         // Size of Code
      (UWORD32)ResetVector,         // Entry Point Address
      // Manufacturer Public Key
      {
        // Public Modulus
        {
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L
        },
        // Public Modulus Length in Bytes
        (UWORD32)1024/8,
        // Public Exponent
        0x00000000L,
      },
      // Originator Public Key
      {
        // Public Modulus
        {
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
          0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L
        },
        // Public Modulus Length in Bytes
        (UWORD32)1024/8,
        // Public Exponent
        0x00000000L,
      },
      // Originator Public Key Signature
      {
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L
      },
      // Software Signature
      {
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L
      },
      // Configuration parameters equal to nu_main.c configuration
      // In const T_MEMIF_CS_CONFIG d_memif_cs5_config
      #if (BOARD == 6)
      {
        0x00A5,                     // CONF_CS5 register
        0x7BC2,                     // EXWS_CS5 register
        0x0000,                     // EX_CTRL register
        C_CSIMGREQUEST,             // CS image request
        0x00400000L,                // RAM size in bytes, 4 Mbytes
        1                           // Granularity in word
      },
      #endif
      #if (BOARD == 42)
      {
        0x00A4,                     // CONF_CS5 register
        0x7BC2,                     // EXWS_CS5 register
        0x0000,                     // EX_CTRL register
        C_CSIMGREQUEST,             // CS image request
        0x01000000L,                // RAM size in bytes 16 Mbytes
        1                           // Granularity in word
      },
      #endif
      #if (BOARD == 43)
      {
        0x00A4,                     // CONF_CS5 register
        0x7887,                     // EXWS_CS5 register
        0x0000,                     // EX_CTRL register
        C_CSIMGNOREQUEST,           // CS image request
        0x02000000L,                // Flash size in bytes 32 Mbytes due to bug
                                    // decoding CS5 thanks to A24, A23 and 139
                                    // logic decoder on E-sample Flash.
                                    // This configuration is independent of
                                    // COMBINED SWITCH.
        1                        // Granularity in word
      },
      #endif
      #if (BOARD == 45)
      {
        0x00A4,                     // CONF_CS5 register
        0x7887,                     // EXWS_CS5 register
        0x0000,                     // EX_CTRL register
        C_CSIMGNOREQUEST,           // CS image no request
        0x01000000L,                // Flash size in bytes 16 Mbytes
        1                           // Granularity in word
      },
      #endif
      // Die Id
      {
        0x00000000L, 0x00000000L    // Die Id is set to 0 by default but it
                                    // could be equal to fuse in order to
                                    // attach platform data to the platform.
      },
      // Platform Data
      {
        0x00000000L, 0x00000000L    // The first two words are set to 0 by
                                    // default and are reserved for IMEI-SV.
      },
      // Certificate Signature
      {
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L
      }
    };

#elif (CHIPSET == 15)
volatile const T_MANUFACTURER_CERTIFICATE_PLATFORM_DATA d_firmware_certificate =
{
    (UWORD16)sizeof(T_MANUFACTURER_CERTIFICATE_PLATFORM_DATA), // Size of Certificate
    (UWORD8)C_CERTTYPE_MAN,         // Type of Certificate
    (UWORD8)C_DEBUGREQUEST,         // Confidentiality Request
    (UWORD32)0x00000000L,           // Address of Code
    (UWORD32)0x00000000L,           // Size of Code
    (UWORD32)ResetVector,               // Entry Point Address
    // Manufacturer Public Key
    {
        // Public Modulus
        {
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L
        },
        //Public Modulus Length in bytes
        0x00000000L,
        //Public Exponent
        0x00000000L,
    },
    // Originator Public Key
    {
        //Public Modulus
        {
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
            0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L
        },
        //Public Modulus length in bytes
        0x00000000L,
        //Public Exponent
        0x00000000L,
    },
    // Originator Public Key Signature
    {
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L
    },
    // Software Signature
    {
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L
    },
    // Configuration parameters
    {
        0x0000FFF9L,          // EMIF CS configuration register nCS3
        0x0000,              // EMIF configuration register
        0x0002,              // Advanced EMIF CS configuration register nCS3
        0x000F,              // EMIF dynamic wait states register configuration
        C_CSIMGNOREQUEST,              // Request to check CS image
        0x01000000L,          // Flash size in bytes
        0x00000001L,          // Granularity
        0x0003,              // CS3 size configuration
        0x0000               // Dummy
     },
     // Die Id
    {
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L
    },
    // Platform Data
    {
        0x00000000L, 0x00000000L    // The first two words are set to 0 by
                                    // default and are reserved for IMEI-SV.
    },
    // Certificate Signature
    {
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L,
        0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L
    }
};

#endif /* (CHIPSET == 12) || (CHIPSET == 15) */
