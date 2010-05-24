/*---------------------------------------------------------------------------
               ----> DO NOT REMOVE THE FOLLOWING NOTICE <----

      Copyright (c) 2003 - 2006 Datalight, Inc.  All Rights Reserved.

  Datalight, Incorporated is a Washington State corporation located at:

        21520 30th Dr SE, Suite 110,      Tel:  425-951-8086
        Bothell, WA  98021                Fax:  425-951-8094
        USA                               Http://www.datalight.com

  This software, including without limitation, all source code and documen-
  tation, is the confidential, trade secret property of Datalight, Inc., and
  is protected under the copyright laws of the United States and other juris-
  dictions.

  ---> Portions of the software are patent pending <---

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

    This header file defines the API function interface to the Reliance
	file system.

    This header file, along with any sub-headers it may include, defines
    the entire set of structures, macros, and function prototypes necessary
    to write a program that interfaces with the Reliance file system.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                              Revision History
    $Log: reliance_rtos.h $
    Revision 1.9  2006/03/08 02:32:23  Pauli
    Added an error code.
    Revision 1.8  2006/03/06 22:34:21Z  Pauli
    Added block device error codes.
    Revision 1.7  2006/03/04 00:04:36Z  Pauli
    Renamed and relocated posix header.
    Revision 1.6  2006/03/02 20:41:10Z  Pauli
    Added a field to the statfs structure for a volume label.
    Revision 1.5  2006/03/02 00:42:05Z  Pauli
    Removed freespace and added statfs.
    Revision 1.4  2006/03/01 18:28:21Z  Pauli
    Moved the posix include to deal with a circular reference issue.
    Revision 1.3  2006/02/25 02:05:14Z  Pauli
    Added Reliance POSIX header.
    Revision 1.2  2006/02/23 00:10:00Z  Pauli
    Removed old directory search APIs (relFs_Get_xxx).
    Added new directory search APIs (relFs_Find_xxx).
    Added new directory enumeration APIs (relFs_Open_Dir, etc.).
    Added const modifier to all pointer input parameters.
    Changed the parameter order of relFs_Get_Attributes to be consistant.
    Updated error codes.
    Renamed open mode and permission flags.
    Replaced the DSTAT structure with the new REL_STAT structure.
    Revision 1.1  2006/02/08 22:10:42Z  Pauli
    Initial revision
---------------------------------------------------------------------------*/

#ifndef RELIANCE_RTOS_H_INCLUDED
#define RELIANCE_RTOS_H_INCLUDED


/*  Reliance defines for transaction mode flags.
*/
#include "rloilcmn.h"


/* ================================================
    Begin Configurable Options
================================================ */


/*  Set this to total number of drives to support.  Setting this value to (4)
    will support drives (A,B,C,D).
*/
/* TI FIX: Changing it to 1 for reducing Memory requirement */
#define REL_NUM_DRIVES (1)


/*  Maximum number of tasks that may use the file system.
*/
#define REL_MAX_USERS   (20)


/*  Maximum file size supported by the signed 32-bit API
*/
#define REL_MAX_FILESIZE (0x7FFFFFFFL)


/*  Maximum Number of open Files/Directories per drive
*/
#define REL_MAX_OPEN_FILES  (30)


/*  Maximum number of sectors that can be transferred to/from any
    block device in a single operation.
*/
#define REL_MAX_SECTORS   (256)


/*  Maximum path length, including drive specifier ("A:")
*/
#define REL_MAX_PATH    (255)


/* ================================================
    End Configurable Options
================================================ */




/*  Seek Flags
*/
#define REL_SEEK_SET    0   /* seek from beginning of file */
#define REL_SEEK_CUR    1   /* seek from current offset */
#define REL_SEEK_END    2   /* seek from end of file */


/*  Error codes.
*/

#define REL_SUCCESS             0
#define REL_FAILURE             -1
#define REL_ERR_ACCES           -1001
#define REL_ERR_BADUSER         -1002
#define REL_ERR_BADDRIVE        -1003
#define REL_ERR_BADFILE         -1004
#define REL_ERR_BADPARM         -1005
#define REL_ERR_BLOCKSIZE       -1006
#define REL_ERR_EXIST           -1007
#define REL_ERR_FORMAT          -1008
#define REL_ERR_INVNAME         -1009
#define REL_ERR_INVPARCMB       -1010
#define REL_ERR_INTERNAL        -1011
#define REL_ERR_IN_USE          -1012
#define REL_ERR_IO_ERROR        -1013
#define REL_ERR_LAST_ENTRY      -1014
#define REL_ERR_LONGPATH        -1015
#define REL_ERR_MAXFILE_SIZE    -1016
#define REL_ERR_NO_MEMORY       -1017
#define REL_ERR_NOEMPTY         -1018
#define REL_ERR_NOFILE          -1019
#define REL_ERR_NOSPC           -1020
#define REL_ERR_NOT_OPENED      -1021
#define REL_ERR_PEMFILE         -1022
#define REL_ERR_SHARE           -1023
#define REL_ERR_ISDIR           -1024


/*  File and directory attributes.
*/
#define REL_ATTR_NORMAL      0x00
#define REL_ATTR_READ_ONLY   0x01
#define REL_ATTR_HIDDEN      0x02
#define REL_ATTR_SYSTEM      0x04
#define REL_ATTR_VOL_LABEL   0x08
#define REL_ATTR_DIRECTORY   0x10
#define REL_ATTR_ARCHIVE     0x20


/*  File creation permissions for open
*/
#define REL_IWRITE   0000400     /* Write permitted */
#define REL_IREAD    0000200     /* Read permitted */


/*  File access flags
*/
#define REL_O_RDONLY       0x0000  /* Open for read only */
#define REL_O_WRONLY       0x0001  /* Open for write only */
#define REL_O_RDWR         0x0002  /* Read/write access allowed. */
#define REL_O_APPEND       0x0004  /* Seek to eof on each write */
#define REL_O_CREAT        0x0008  /* Create the file if it does not exist. */
#define REL_O_TRUNC        0x0010  /* Truncate the file if it already exists */
#define REL_O_EXCL         0x0020  /* Fail if creating and already exists */
#define REL_O_NOSHAREANY   0x0040  /* Fail if already open.  Other opens will fail. */
#define REL_O_NOSHAREWRITE 0x0080  /* Fail if already open for write. Other open for
                                      write calls will fail. */



/*-------------------------------------------------------------------
    Create masks for all the supported and unsupported
    transaction types.
-------------------------------------------------------------------*/
#define REL_TRANSACT_SUPPORTED_MASK         \
(                                           \
    REL_TRANSACT_MANUAL        |            \
    REL_TRANSACT_DIR_CREATE    |            \
    REL_TRANSACT_DIR_DELETE    |            \
    REL_TRANSACT_DIR_RENAME    |            \
    REL_TRANSACT_FILE_CREATE   |            \
    REL_TRANSACT_FILE_WRITE    |            \
    REL_TRANSACT_FILE_FLUSH    |            \
    REL_TRANSACT_FILE_TRUNCATE |            \
    REL_TRANSACT_FILE_CLOSE    |            \
    REL_TRANSACT_FILE_RENAME   |            \
    REL_TRANSACT_FILE_DELETE   |            \
    REL_TRANSACT_ATTRIBUTES    |            \
    REL_TRANSACT_VOL_FULL                   \
)

#define REL_TRANSACT_UNSUPPORTED_MASK (~(REL_TRANSACT_SUPPORTED_MASK))

#define REL_TRANSACT_DEFAULT                \
(                                           \
    REL_TRANSACT_DIR_CREATE    |            \
    REL_TRANSACT_DIR_DELETE    |            \
    REL_TRANSACT_DIR_RENAME    |            \
    REL_TRANSACT_FILE_CLOSE    |            \
    REL_TRANSACT_FILE_FLUSH    |            \
    REL_TRANSACT_FILE_RENAME   |            \
    REL_TRANSACT_FILE_DELETE   |            \
    REL_TRANSACT_ATTRIBUTES    |            \
    REL_TRANSACT_VOL_FULL                   \
)


/*  relFs_Check() bit flag options
*/
#define REL_CHK_OPT_VERBOSE     0x01
#define REL_CHK_OPT_NOPROMPT    0x02
#define REL_CHK_OPT_DUMP_INDEX  0x04
#define REL_CHK_OPT_QUIET       0x08
#define REL_CHK_OPT_SHOW_ERRORS 0x10

#define REL_CHK_OPT_DEFAULT     (  \
      REL_CHK_OPT_NOPROMPT      |  \
      REL_CHK_OPT_SHOW_ERRORS   )


typedef struct rel_find
{
    char            szName[REL_MAX_PATH + 1];

    /*  Private file system data
    */
    void           *pPrivateData;
} REL_FIND;


typedef struct rel_dir
{
    char            szName[REL_MAX_PATH + 1];

    /*  Private file system data
    */
    void           *pPrivateData;
} REL_DIR;


typedef struct rel_stat
{
    unsigned short  uDrive;         /*  drive number */
    unsigned short  uAttributes;    /*  attributes */
    unsigned short  uLinks;         /*  number of links to the file */
    unsigned long   ulCreation;     /*  creation time */
    unsigned long   ulModify;       /*  last modification time */
    unsigned long   ulAccess;       /*  last access time */
    unsigned long   ulSize;         /*  file size */
    unsigned long   ulSerial;       /*  unique serial number */
} REL_STAT;


typedef struct rel_statfs
{
    unsigned short  uDrive;         /*  drive number */
    unsigned long   ulNumBlocks;    /*  total number of blocks */
    unsigned long   ulFreeBlocks;   /*  free blocks on the drive */
    unsigned long   ulBlockSize;    /*  logical block size in bytes */
    unsigned long   ulUsedBlocks;   /*  used blocks on the drive */
    char            szLabel[REL_MAX_PATH + 1]; /*  volume label */
} REL_STATFS;


/******************************************************************************
                    Begin Block Device Interface
******************************************************************************/


/*  IOCTL commands for talking to the block device
*/
#define REL_IOCTL_BASE              0x5100
#define REL_IOCTL_DISCARD_SUPPORT   (REL_IOCTL_BASE + 1)
#define REL_IOCTL_DISCARD           (REL_IOCTL_BASE + 2)
#define REL_IOCTL_FLUSH             (REL_IOCTL_BASE + 3)
#define REL_IOCTL_PARAMS            (REL_IOCTL_BASE + 4)
#define REL_IOCTL_COMPACT           (REL_IOCTL_BASE + 5)


/*  Error codes
*/
#define BLKDEV_SUCCESS              (0)
#define BLKDEV_FAILURE              (-1)


/*  Structure for getting compaction information through the ioctl interface.
*/
typedef struct compaction_info
{
    unsigned long   ulSleepMS;
} COMPACTION_INFO;


/* If already defined in ffsdrv.h */
#ifndef FFXDRV_H_INCLUDED

/*  Structure for passing discard parameters through the ioctl interface.
*/
typedef struct discard_params
{
    unsigned long   ulStartSector;
    unsigned long   ulNumSectors;
} DISCARD_PARAMS;


/*  Stucture for retrieving the device parameters through the ioctl interface.
*/
typedef struct disk_params
{
    unsigned long   ulSectorSize;
    unsigned long   ulSectorCount;
} DISK_PARAMS;

#endif


/*  Block device driver functions.
*/
typedef struct block_dev
{
    /*  Open a device.
    */
    unsigned long(*open_proc)   (unsigned short uDevNum);

    /*  Close a device.
    */
    unsigned long(*close_proc)  (unsigned short uDevNum);

    /*  Read & Write

        This function is called to read and write sectors to/from the device.
    */
    unsigned long(*io_proc)     (unsigned short uDevNum, unsigned long ulSector, void *pBuffer, unsigned short uCount, unsigned short fRead);

    /*  IOCTL

        This interface is used by Reliance to get the physical parameters of
        the device and to perform flush and discard functionality.
    */
    unsigned long(*ioctl_proc)  (unsigned short uDevNum, unsigned short uCommand, void *pBuffer);
}BLOCK_DEV;



/******************************************************************************
                    End Block Device Interface
******************************************************************************/


int             FileSystemInitialize(void);
int             FileSystemUnInitialize(void);


/*  Reliance specific API.

    NOTE: relFs_Initialize must be called before any other Reliance API.
*/
int             relFs_Initialize(void);
int             relFs_Show(const char *szDrive);
int             relFs_Check(const char *szDrive, unsigned long ulOptions);
int             relFs_Test(const char *szCmdLine);
int             relFs_TransactionPoint(const char *szDrive);
int             relFs_SetTransactionMode(const char *szDrive, unsigned long ulMode);
int             relFs_GetTransactionMode(const char *szDrive, unsigned long *pulMode);


/*  General File System Operations
*/
int             relFs_Close(int fd);
int             relFs_Flush(int fd);
int             relFs_Get_Attributes(const char *szName, unsigned short *puAttr);
int             relFs_Stat(int fd, REL_STAT *pStat);
int             relFs_StatFs(int fd, REL_STATFS *pStatFs);
int             relFs_Open(const char *szName, unsigned short uFlags, unsigned short uMode);
signed long     relFs_Read(int fd, char *pBuf, signed long lSize);
signed long     relFs_Seek(int fd, signed long lOffset, unsigned short uOrigin);
int             relFs_Set_Attributes(const char *szName, unsigned short uAttr);
int             relFs_Truncate(int fd, signed long lOffset);
signed long     relFs_Write(int fd, const char *pBuf, signed long lSize);


/*  Disk Operations
*/
int             relFs_Format(unsigned short uDrive, unsigned long ulBlockSize);
int             relFs_Open_Disk(const char *szPath);
void            relFs_Disk_Abort(const char *szDrive);
int             relFs_Close_Disk(const char *szDrive);


/*  Directory Operations
*/
int             relFs_Make_Dir(const char *szName);
int             relFs_Remove_Dir(const char *szName);
int             relFs_Delete(const char *szName);
int             relFs_Rename(const char *szName, const char *szNewName);
int             relFs_Link(const char *szName, const char *szNewName);
int             relFs_Open_Dir(const char *szName, REL_DIR *pDir);
int             relFs_Read_Dir(REL_DIR *pDir);
int             relFs_Close_Dir(REL_DIR *pDir);
int 			relFs_Entries_Dir(REL_DIR *RelDir);
int             relFs_Find_First(const char *szPattern, REL_FIND *pFind);
int             relFs_Find_Next(REL_FIND *pFind);
int             relFs_Find_Close(REL_FIND *pFind);


/*  File System User API
*/
int             relFs_Become_File_User(void);
void            relFs_Release_File_User(void);
int             relFs_Check_File_User(void);
int             relFs_Current_Dir(const char *szDrive, char *szPath);
unsigned short  relFs_Get_Default_Drive(void);
int             relFs_Set_Current_Dir(const char *szPath);
int             relFs_Set_Default_Drive(unsigned short uDrive);


/*  Datalight POSIX API for Reliance
*/
#include "reliance_posix.h"


#endif
