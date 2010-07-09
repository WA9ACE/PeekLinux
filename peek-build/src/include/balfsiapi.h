

#ifndef FSIAPI_H
#define FSIAPI_H
/*****************************************************************************
*****************************************************************************/
#include "sysdefs.h"
#include "fsmapi.h"
#include "fsmdataitem.h"

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

typedef    HFSMFILE       BalFsiHandleT;
typedef    HENUM          BalFsiFindHandle;	/*file search handle*/

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
	uint16        Attrib;
	uint32        CreateTime;  /*Include date and time of day.*/
	uint32        Size;
} BalFsiFileAttribT;

typedef struct 
{
	BalFsiFileAttribT  FileAttrib;
	char               Name[FSI_MAX_FILE_NAME_LENGTH + 1];
} BalFsiFileInfoT;

typedef struct 
{
	uint16    ItemId;
	uint16    ItemType;
	uint32    ItemLength;
} BalFsiDataItemInfoT;

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
} BalFsiFileOpenModeT;

typedef enum 
{ 
	FSI_FILE_SEEK_START   = FSM_FILE_BEGIN,
	FSI_FILE_SEEK_END     = FSM_FILE_END,
	FSI_FILE_SEEK_CURRENT = FSM_FILE_CURRENT
} BalFsiFileSeekTypeT;

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
} BalFsiResultT;

#ifdef  __cplusplus
extern "C" {
#endif

BalFsiResultT BalFsiGetSpaceInfo(const char *VolumeNameP, uint32 *TotalSpaceP,
                           uint32 *FreeSpaceP);

BalFsiResultT BalFsiFileOpen(BalFsiHandleT *FileP, const char *FileNameP,
                       BalFsiFileOpenModeT Mode);

BalFsiResultT BalFsiFileClose(BalFsiHandleT File);

BalFsiResultT BalFsiFileRead(void *BufferP, uint32 ItemSize, uint32 *ItemNumP,
                       BalFsiHandleT File);

BalFsiResultT BalFsiFileWrite(void *BufferP, uint32 ItemSize, uint32 *ItemNumP,
                        BalFsiHandleT File);

BalFsiResultT BalFsiFlush(BalFsiHandleT File);

BalFsiResultT BalFsiGetFileLength(const char *NameP, uint32 *FileLengthP);

BalFsiResultT BalFsiGetFileHandleAttrib(BalFsiHandleT File,
                                  BalFsiFileAttribT *FileAttribP);

BalFsiResultT BalFsiGetFileAttrib(const char *FileNameP,
                            BalFsiFileAttribT *FileAttribP);

BalFsiResultT BalFsiSetFileAttrib(const char *FileNameP, uint16 Attrib);

BalFsiResultT BalFsiSeek(BalFsiHandleT File, BalFsiFileSeekTypeT SeekFrom,
                   int32 MoveDistance);

BalFsiResultT BalFsiTell(BalFsiHandleT File, uint32 *PosP);

BalFsiResultT BalFsiRename(const char *OldNameP, const char *NewNameP);

BalFsiResultT BalFsiFileTruncate(BalFsiHandleT File, uint32 NewSize);

BalFsiResultT BalFsiRemove(const char *NameP);

BalFsiResultT BalFsiFindFirst(BalFsiFindHandle *FindHandleP, const char *NameP,
                        BalFsiFileInfoT *FileInfoP);

BalFsiResultT BalFsiFindNext(BalFsiFindHandle FindHandle, BalFsiFileInfoT *FileInfoP);

BalFsiResultT BalFsiFindClose(BalFsiFindHandle FindHandle);

BalFsiResultT BalFsiMakeDir (const char *DirNameP);

BalFsiResultT BalFsiRemoveDir (const char *DirNameP);


/*Data Item APIs: */

BalFsiResultT BalFsiSidbRead(uint16 ItemID, uint16 ItemType, void*BufferP,
                       uint32 Offset, uint32 *SizeP);

BalFsiResultT BalFsiSidbWrite(uint16 ItemID, uint16 ItemType, void*BufferP,
                        uint32 Offset, uint32 *SizeP);

BalFsiResultT BalFsiSidbDelete(uint16 ItemID, uint16 ItemType);

BalFsiResultT BalFsiSidbFindFirst(BalFsiHandleT *FindHandleP, uint16 ItemID,
                            uint16 ItemType, BalFsiDataItemInfoT *InfoP);

BalFsiResultT BalFsiSidbFindNext (BalFsiHandleT FindHandle, BalFsiDataItemInfoT *InfoP);

BalFsiResultT BalFsiSidbFindClose (BalFsiHandleT FindHandle);

BalFsiResultT BalFsiSidbFlush(uint16 ItemID, uint16 ItemType);

BalFsiResultT BalFsiSidbFlushAll(void);

BalFsiResultT BalFsiSetFileSelfAttr(const char *FileNameP, uint16 FileSelfAttrib);

BalFsiResultT BalFsiGetFileSelfAttr(const char *FileNameP, uint16 *FileSelfAttribP);


#ifdef  __cplusplus
}
#endif





#endif
