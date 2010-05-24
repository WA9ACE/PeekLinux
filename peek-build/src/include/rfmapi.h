#ifndef RFMAPI_H
#define RFMAPI_H
/*****************************************************************************
 
  FILE NAME:  RfmApi.h

  DESCRIPTION:
   
    This file contains all the constants and function prototypes exported 
    by the Remote File Manager (RFM) unit.

Copyright (c) 2002 LSI Logic. All rights reserved. LSI Confidential information.
*****************************************************************************/

#include "sysdefs.h"

/* Each Remote File must be defined using this Remote File Descriptor.
 * The Remote File Descriptor must then be added to the RFM list of descriptors,
 * located in rfm.c.
 * If the contents of this list changes in any way, it is critical that the
 * RFM_VERSION_NUMBER definition (also in rfm.c) is incremented to force an 
 * automatic re-format of the RFM Drive.
 * Content of remote file descriptor:
 *   - It is assumed that the FileName starts with "B:" 
 *   - FilePtr must point to address in CP ROM flash
 *   - FilePtr + FileSize must not go beyond the 2 MByte flash space
 */
typedef struct
{
   char       *FileName;
   const char *FilePtr;
   uint32      FileSize;

} RfmRemoteFileDescrT;

/*---------------------------------------------------------------
 *  External Function Prototypes
 *----------------------------------------------------------------*/
/* The following RFM Initialization function is called by FSI at startup,
 * or when the FSI Drive is re-formatted. It initializes the Remote Files.
 */
extern void RfmInit (void);

/* The following functions are the device drivers for the RFM Drive,
 * called by Nucleus File.
 */
extern INT RfmFlashOpen (INT16 driveno);
extern INT RfmFlashRawOpen (INT16 driveno);
extern INT RfmFlashClose (INT16 driveno);
extern INT RfmFlashIo (UINT16 driveno, UINT32 sector, void *buffer, UINT32 count, INT reading);
extern INT RfmFlashIoctl (UINT16 driveno, UINT16 command, void *buffer);


#endif