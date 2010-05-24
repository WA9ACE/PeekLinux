




#ifndef __FSM_API_H__

#define __FSM_API_H__

#include "fsmdefs.h"
#include "fsmdev.h"
#include "fsmfs.h"
/* Operation mode */

#define FSM_MAX_VFS_FD             16

/* These options can be ORed together. -- access mode. */
#define FSM_OPEN_READ			0x00000001
#define FSM_OPEN_WRITE			0x00000002

#define FSM_ACCESS_MODE_MASK	0x000000FF

/* These options exclude each other. -- open mode.     */
#define FSM_CREATE_NEW			0x00000100
#define FSM_CREATE_ALWAYS		0x00000200  
#define FSM_OPEN_EXISTING		0x00000400
#define FSM_OPEN_ALWAYS			0x00000800

#define FSM_OPEN_MODE_MASK		0x0000FF00

#define FSM_MODE_MASK			0x0000FFFF

/* one and only one mode in these could be set */
#define FSM_OPEN_FILE			0x00010000
#define FSM_OPEN_DIR			0x00020000



/* Seek mode */
#define FSM_FILE_BEGIN			1
#define FSM_FILE_END			2
#define FSM_FILE_CURRENT		3


#define INVALID_FILE_HANDLE		((uint32)(-1))


typedef uint32				HFSMFILE;

typedef void *				HENUM;


#define MAX_FILE_NAME_LEN	256      /*16*/	/*  max filename length is determined by Dfs/nffs/Dos design. */

typedef struct
{
	char			FileName[MAX_FILE_NAME_LEN + 1];
	uint32			FileLength;
	uint16			CreateDate;
	uint16			CreateTime;
	uint16			Attrib;
}FsmFileInfoT;

typedef struct
{
	uint32			FileLength;
	uint16			CreateDate;
	uint16			CreateTime;
	uint16			Attrib;
}FsmFileAttribT;

#define FSM_FORMAT_NAME      1
#define FSM_FORMAT_DEV       2

typedef struct
{
	FsmFsDrvT       *	FsP;
	FsmDevObjHdrT   *	DevP;

} FsmFormatArgumentT;


#define MAX_PATH_LENGTH			128		/*  max PATH length is determined by Dfs/nffs/Dos design. */


#define FSM_FILE_ATTRIB_NORMAL         0x00
#define FSM_FILE_ATTRIB_READONLY       0x01
#define FSM_FILE_ATTRIB_HIDDEN         0x02
#define FSM_FILE_ATTRIB_SYSTEM         0x04
#define FSM_FILE_ATTRIB_ARCHIVE		   0x08
#define FSM_FILE_ATTRIB_MASK           0x0F

#define FSM_FILE_ATTRIB_DIR            0x10
#define FSM_FILE_ATTRIB_FILE           0x20
#define FSM_FILE_TYPE_MASK			   0x30

#define MAX_VOLUME_NAME_LEN            16

typedef struct
{
	char			* 	VolumeName;
	FsmFsDrvT       *	FsP;
	FsmDevObjHdrT   *	DevP;
   uint16         SectorSize;
} FsmDevListT;


/*========================= File System APIs ===============================*/

HFSMFILE	FsmOpen(const char * path, uint32 mode);

uint32		FsmClose(HFSMFILE hFile);
uint32		FsmCloseAll(void);

uint32		FsmRead(HFSMFILE hFile, void * buffer, uint32 size);
uint32		FsmWrite(HFSMFILE hFile, void * buffer, uint32 size);

uint32		FsmSeek(HFSMFILE hFile, int32 offset, uint32 whence);

uint32		FsmTell(HFSMFILE hFile);

uint32		FsmEof(HFSMFILE hFile, uint32 * Eof);

uint32		FsmError(HFSMFILE hFile);

uint32		FsmGetAttrib(HFSMFILE hFile, FsmFileAttribT * attrib);
uint32		FsmGetAttribByName(const char * path, FsmFileAttribT * attrib);

uint32		FsmSetAttrib(const char * path, FsmFileAttribT * attrib);

uint32		FsmTruncate(HFSMFILE hFile, uint32 size);

uint32		FsmRemove(const char* file_dir_name);

uint32		FsmRename(const char *path, const char *newname);

uint32		FsmMakeDir(const char *path);
uint32		FsmRemoveDir(const char *path);

HENUM		FsmFindFirstFile(const char * filename, FsmFileInfoT * Info);
uint32		FsmFindNextFile(HENUM hEnum, FsmFileInfoT * Info);

uint32		FsmFindClose(HENUM hEnum);

uint32		FsmCloseDir(HFSMFILE hFile);

uint32		FsmFlush(HFSMFILE hFile);
uint32		FsmFlushAll(void);

uint32		FsmGetFreeSpace(const char * VolumeName, uint32 * totalspace, uint32 * freespace);

uint32		FsmFileIoCtrl(HFSMFILE hFile, uint32 cmd, void * arg);

HFSMFILE	FsmOpenDir(const char *path);

uint32		FsmReadDir(HFSMFILE hDir, FsmFileInfoT * DirEntry);

/*uint32		FsmFormat(const char * VolumeName);*/
uint32      FsmFormat(FsmFormatArgumentT * FormatArg); 

uint32		FsmMount(FsmFsDrvT * pFs, FsmDevObjHdrT * pDev, const char * VolumeName, uint32 sectorsize);

uint32		FsmUnMount(const char* VolumeName);

uint32		FsmGetLastErrorCode(void);

#endif /* __FSM_API_H__ */






