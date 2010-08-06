#ifndef FSIAPI_H
#define FSIAPI_H
/*****************************************************************************
*****************************************************************************/
#include "nucleus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"

#define MAX_FILE_NAME_LEN       	256
#define FSM_FILE_ATTRIB_DIR            0x10
#define FSM_FILE_ATTRIB_FILE           0x20
#define FSM_FILE_TYPE_MASK             0x30
#define MAX_VOLUME_NAME_LEN            16

/* Seek mode */
#define FSM_FILE_BEGIN                  0 
#define FSM_FILE_CURRENT                1
#define FSM_FILE_END                    2

#define FSM_FILE_ATTRIB_NORMAL         0x00
#define FSM_FILE_ATTRIB_READONLY       0x01
#define FSM_FILE_ATTRIB_HIDDEN         0x02
#define FSM_FILE_ATTRIB_SYSTEM         0x04
#define FSM_FILE_ATTRIB_ARCHIVE        0x08
#define FSM_FILE_ATTRIB_MASK           0x0F

/*max full path file name length, doesn't include end char "0" */
#define    FSI_MAX_PATH_NAME_LENGTH    MAX_PATH_LENGTH

/*max file name length, doesn't include end char "0" */
#define    FSI_MAX_FILE_NAME_LENGTH    MAX_FILE_NAME_LEN

/*max volume name length, doesn't include end char "0" */
#define    FSI_MAX_VOLUME_NAME_LENGTH  MAX_VOLUME_NAME_LEN

#define    FSI_INVALID_FIND_HANDLE     NULL
#define    FSI_INVALID_FILE_HANDLE     INVALID_FILE_HANDLE

#define    FSI_SIDB_WILDCARD_TYPE      WILDCARD_TYPE
#define    FSI_SIDB_WILDCARD_ID        WILDCARD_ID

#define FSI_FILE_PATH_SEPARATOR '/'

#define FSI_FILE_NAME_DOT '.'

typedef    UINT32       FsiHandleT;
typedef    void *       FsiFindHandle;	/*file search handle*/

enum
{
	FSI_FILE_ATTRIB_NORMAL      = FSM_FILE_ATTRIB_NORMAL,
	FSI_FILE_ATTRIB_READONLY    = FSM_FILE_ATTRIB_READONLY,
	FSI_FILE_ATTRIB_HIDDEN      = FSM_FILE_ATTRIB_HIDDEN,
	FSI_FILE_ATTRIB_SYSTEM      = FSM_FILE_ATTRIB_SYSTEM,
	FSI_FILE_ATTRIB_ARCHIVE     = FSM_FILE_ATTRIB_ARCHIVE,
	FSI_FILE_ATTRIB_DIR         = FSM_FILE_ATTRIB_DIR,
	FSI_FILE_ATTRIB_FILE        = FSM_FILE_ATTRIB_FILE
};

/*Especially the enum type for setting or getting the file/directory oneself attribute.*/
enum
{
    BAL_FILE_ATTR_NORMAL     = 0x00,     /* NORMAL is identical with FILE */
    BAL_FILE_ATTR_READ_ONLY  = 0x01,
    BAL_FILE_ATTR_HIDDEN     = 0x02,
    BAL_FILE_ATTR_SYSTEM     = 0x04,
    BAL_FILE_ATTR_VOL_LABEL  = 0x08,
    BAL_FILE_ATTR_DIRECTORY  = 0x10,
    BAL_FILE_ATTR_ARCHIVE    = 0x20
};

typedef struct 
{
	UINT16        Attrib;
	UINT32        CreateTime;  /*Include date and time of day.*/
	UINT32        Size;
} FsiFileAttribT;

typedef struct 
{
	FsiFileAttribT  FileAttrib;
	char               Name[FSI_MAX_FILE_NAME_LENGTH + 1];
} FsiFileInfoT;

typedef struct 
{
	UINT16    ItemId;
	UINT16    ItemType;
	UINT32    ItemLength;
} FsiDataItemInfoT;

/*Open modes for file open, All write mode permit read operation:
Mode 0:
Read only mode. If file don't exist, return error. File pointer is at start.
Mode 1:
write an existing file. File pointer is set to file start.
Mode 2:
Create a new file, if file already exist, return error.
Mode 3:
write mode, If file don't exist, create it, else truncate it to zero length.
Mode 4:
write mode, if file don't exist, create it, else don't truncate it, file
pointer is at start.
*/
typedef enum 
{
	FSI_FILE_OPEN_READ_EXIST     = 0,
	FSI_FILE_OPEN_WRITE_EXIST    = 1,
	FSI_FILE_OPEN_CREATE_NEW     = 2,
	FSI_FILE_OPEN_CREATE_ALWAYS  = 3,
	FSI_FILE_OPEN_WRITE_ALWAYS   = 4
} FsiFileOpenModeT;

typedef enum 
{ 
	FSI_FILE_SEEK_START   = FSM_FILE_BEGIN,
	FSI_FILE_SEEK_END     = FSM_FILE_END,
	FSI_FILE_SEEK_CURRENT = FSM_FILE_CURRENT
} FsiFileSeekTypeT;

typedef enum
{
   FSI_SUCCESS         = 0, /*No errors. Function completed successfully.*/
   FSI_ERR_PARAMETER   = 1, /*Incorrect parameter to the function.*/
   FSI_ERR_READ        = 2, /*file read operation is failed.*/
   FSI_ERR_WRITE       = 3, /*file write operation is failed.*/
   FSI_ERR_SYSTEM      = 4, /*Indicates that a system error has occurred.*/
   FSI_ERR_EXIST       = 5, /*The specified object has existed already.*/
   FSI_ERR_NOTEXIST    = 6, /*No matched object in specified media.*/
   FSI_ERR_EOF         = 7, /*file pointer reaches the end-of-file.*/
   FSI_ERR_FULL        = 8, /*Flash device is full*/
   FSI_ERR_NOSUPPORT   = 9, /*FSI does not support this function now .*/
   FSI_ERR_FORMAT      = 10, /*Volume is in the incorrect format.*/
   FSI_ERR_ACCESS_DENY = 11, /*Insufficient permissions to access object.*/
   
   /* reach to a limitation of the maximum number of the files that can be open
   simultaneously.*/
   FSI_ERR_MAX_OPEN    = 12, 
   FSI_ERR_READDIR_END = 13,
   FSI_ERR_BAD_FD      = 14,

   FSI_ERR_UNKNOWN     = 255 /*Other unknowned error occar*/
} FsiResultT;

#ifdef  __cplusplus
extern "C" {
#endif

FsiResultT FsiGetSpaceInfo(const char *VolumeNameP, UINT32 *TotalSpaceP,
                           UINT32 *FreeSpaceP);

FsiResultT FsiFileOpen(FsiHandleT *FileP, const char *FileNameP,
                       FsiFileOpenModeT Mode);

FsiResultT FsiFileClose(FsiHandleT File);

FsiResultT FsiFileRead(void *BufferP, UINT32 ItemSize, UINT32 *ItemNumP,
                       FsiHandleT File);

FsiResultT FsiFileWrite(void *BufferP, UINT32 ItemSize, UINT32 *ItemNumP,
                        FsiHandleT File);

FsiResultT FsiFlush(FsiHandleT File);

FsiResultT FsiGetFileLength(const char *NameP, UINT32 *FileLengthP);

FsiResultT FsiGetFileHandleAttrib(FsiHandleT File,
                                  FsiFileAttribT *FileAttribP);

FsiResultT FsiGetFileAttrib(const char *FileNameP,
                            FsiFileAttribT *FileAttribP);

FsiResultT FsiSetFileAttrib(const char *FileNameP, UINT16 Attrib);

FsiResultT FsiSeek(FsiHandleT File, FsiFileSeekTypeT SeekFrom,
                   int MoveDistance);

FsiResultT FsiTell(FsiHandleT File, UINT32 *PosP);

FsiResultT FsiRename(const char *OldNameP, const char *NewNameP);

FsiResultT FsiFileTruncate(FsiHandleT File, UINT32 NewSize);

FsiResultT FsiRemove(const char *NameP);

FsiResultT FsiFindFirst(FsiFindHandle *FindHandleP, const char *NameP,
                        FsiFileInfoT *FileInfoP);

FsiResultT FsiFindNext(FsiFindHandle FindHandle, FsiFileInfoT *FileInfoP);

FsiResultT FsiFindClose(FsiFindHandle FindHandle);

FsiResultT FsiMakeDir (const char *DirNameP);

FsiResultT FsiRemoveDir (const char *DirNameP);

#ifdef  __cplusplus
}
#endif





#endif
