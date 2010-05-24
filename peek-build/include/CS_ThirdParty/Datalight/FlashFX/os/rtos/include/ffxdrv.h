/*---------------------------------------------------------------------------
               ----> DO NOT REMOVE THE FOLLOWING NOTICE <----

      Copyright (c) 1993 - 2006 Datalight, Inc.  All Rights Reserved.

  Datalight, Incorporated is a Washington State corporation located at:

        21520 30th Dr SE, Suite 110,      Tel:  425-951-8086
        Bothell, WA  98021                Fax:  425-951-8094
        USA                               Http://www.datalight.com

  This software, including without limitation, all source code and documen-
  tation, is the confidential, trade secret property of Datalight, Inc., and
  is protected under the copyright laws of the United States and other juris-
  dictions.  Portions of the software may also be subject to one or more the
  following US patents: US#5860082, US#6260156.

  In addition to civil penalties for infringement of copyright under applic-
  able U.S. law, 17 U.S.C. 1204 provides criminal penalties for violation of
  (a) the restrictions on circumvention of copyright protection systems found
  in 17 U.S.C. 1201 and (b) the protections for the integrity of copyright
  management information found in 17 U.S.C. 1202.

  U.S. Government Restricted Rights:  Use, duplication, reproduction, or
  transfer of this commercial product and accompanying documentation is
  restricted in accordance with FAR 12.212 and DFARS 227.7202 and by a
  License Agreement.

  IN ADDITION TO COPYRIGHT AND PATENT LAW, THIS SOFTWARE IS PROTECTED UNDER A
  SOURCE CODE AGREEMENT, NON-DISCLOSURE AGREEMENT (NDA), OR SIMILAR BINDING
  CONTRACT BETWEEN DATALIGHT, INC. AND THE LICENSEE ("BINDING AGREEMENT").
  YOUR RIGHT, IF ANY, TO COPY, PUBLISH, MODIFY OR OTHERWISE USE THE SOFTWARE,
  IS SUBJECT TO THE TERMS AND CONDITIONS OF THE BINDING AGREEMENT.  BY USING
  THE SOFTWARE IN ANY MANNER, IN WHOLE OR IN PART, YOU AGREE TO BE BOUND BY
  THE TERMS OF THE BINDING AGREEMENT.  CONTACT DATALIGHT, INC. AT THE ADDRESS
  SET FORTH ABOVE IF YOU OBTAINED THIS SOFTWARE IN ERROR.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                              Description

    This file contains symbols and structures used by the FlashFX device
    driver for generic RTOS solutions.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                              Revision History
    $Log: ffxdrv.h $
    Revision 1.4  2006/02/13 22:17:49  Pauli
    Updated to fully support devices and disks.
    Revision 1.3  2006/02/11 00:31:39Z  Garyp
    Started the process of factoring things into Devices and Disks.  Work-in-progress,
    not completed yet.
    Revision 1.2  2006/02/09 19:18:42Z  Pauli
    Updated for the new device driver interface that works with the Reliance
    RTOS kit.
    Revision 1.1  2005/10/02 01:45:34Z  Pauli
    Initial revision
---------------------------------------------------------------------------*/
#ifndef FFXDRV_H_INCLUDED
#define FFXDRV_H_INCLUDED


/*  Number of root directory entries if we do the FAT format.
*/
#define ROOT_DIR_SIZE 0xF0

/* NOTE : dont define this macro while building library */
/* This Macro will be enabled by the GBI data light plugin */
#ifndef FFXDRV_INC_LOCOSTO_DATALIGHT_PLUGIN

/*  The disk initialization data is just the disk number.
*/
struct tagFFXDISKINITDATA
{
    D_UINT16        uDiskNum;
};


/*  This structure defines the template to be used for a FlashFX
    device.  This structure embeds all the components needed to
    identify each FlashFX disk.
*/
struct tagFFXDISKHOOK
{
    D_UINT16        uDiskNum;
};


/*  The device initialization data is just the device number.
*/
struct tagFFXDEVINITDATA
{
    D_UINT16        uDeviceNum;
};


/*  This structure defines the template to be used for a FlashFX
    device.  This structure embeds all the components needed to
    identify each FlashFX device.
*/
struct tagFFXDEVHOOK
{
    D_UINT16        uDeviceNum;
};


#endif
/*-------------------------------------------------------------------
  One of the following IOCTL function codes is specified in the
  FlashFXDeviceIoctl function for performing the ioctl calls.

  This values are used in the sample device driver only.

  -------------------------------------------------------------------*/
/*  IOCTL commands for talking to the block device
*/
#define FLASHFX_IOCTL_BASE              	0x4100
#define FLASHFX_IOCTL_DISCARD_SUPPORT   	(FLASHFX_IOCTL_BASE + 1)
#define FLASHFX_IOCTL_DISCARD           	(FLASHFX_IOCTL_BASE + 2)
#define FLASHFX_IOCTL_FLUSH             	(FLASHFX_IOCTL_BASE + 3)
#define FLASHFX_IOCTL_PARAMS            	(FLASHFX_IOCTL_BASE + 4)
#define FLASHFX_IOCTL_MEDIA_PARAMS      	(FLASHFX_IOCTL_BASE + 5)
#define FLASHFX_IOCTL_PARTITION_PARAMS  	(FLASHFX_IOCTL_BASE + 6)
#define FLASHFX_IOCTL_COMPACTION_SUSPEND  	(FLASHFX_IOCTL_BASE + 7)
#define FLASHFX_IOCTL_COMPACTION_RESUME  	(FLASHFX_IOCTL_BASE + 8)


/*-------------------------------------------------------------------
  Prototype for the FlashFX  function
  -------------------------------------------------------------------*/
extern unsigned long FlashFXDeviceOpen(unsigned short uDriveNum);
extern unsigned long FlashFXDeviceClose(unsigned short uDriveNum);
extern unsigned long FlashFXDeviceIO(unsigned short uDriveNum, unsigned long ulSectorNum, void *pClientBuffer, unsigned short uSectorCount, unsigned short fReading);
extern unsigned long FlashFXDeviceIoctl(unsigned short uDriveNum, unsigned short command, void *buffer);



#ifndef RELIANCE_RTOS_H_INCLUDED
	
/*  structure for passing discard parameters through ioctl interface
*/
typedef struct
{
    unsigned long          ulStartSector;
    unsigned long          ulNumSectors;
} DISCARD_PARAMS;



/*  Stucture for retrieving the device parameters through the ioctl interface.
*/
typedef struct disk_params
{
    unsigned long   ulSectorSize;
    unsigned long   ulSectorCount;
} DISK_PARAMS;


#endif


/*  Stucture for retrieving the media parameters through the ioctl interface.
*/
typedef struct disk_media_params
{
    unsigned long   ulSectorSize;
    unsigned long   ulSectorCount;
    unsigned short  fIsMBR;
    unsigned short  fValidPartition;
    unsigned short  uSecPerTrack;
    unsigned short  uHeads;
    unsigned short  uCylinders;
    unsigned short  uNumOfPartitions;
} DISK_MEDIA_PARAMS;

/*  Stucture for retrieving the partition parameters through the ioctl interface.
*/
typedef struct disk_partition_params
{
    unsigned long   ulStartSector;
    unsigned long   ulSectorCount;
    unsigned char   ucBootable;
} DISK_PARTITION_PARAMS;

#endif /* FFXDRV_H_INCLUDED */
