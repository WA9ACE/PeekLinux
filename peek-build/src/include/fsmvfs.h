




#ifndef __FSM_VFS_H__

#define __FSM_VFS_H__

#include "fsmdefs.h"
#include "ostype.h"
#include "FsmDev.h"
#include "FsmFs.h"
#include "FsmApi.h"

/**************************************************************************
 *
 *      Interfaces between VFS and low level filesystem.
 *
 **************************************************************************/


/* File I/O control command. */

enum
{
   FSM_IOCTL_GET_ATTRIB = 2,
   FSM_IOCTL_SET_ATTRIB,
   FSM_IOCTL_TRUNCATE,
   FSM_IOCTL_RENAME,
   FSM_IOCTL_FLUSH,
   FSM_IOCTL_GET_FREE_SPACE,
   FSM_IOCTL_SEEK,
   FSM_IOCTL_TELL,
   FSM_IOCTL_EOF,
   FSM_IOCTL_FORMAT
};



/* File I/O command argument structure. */

typedef struct
{
   int32       offset;
   uint32      whence;
} FsmIoctlSeekT;

typedef struct
{
	FsmDevObjHdrT     *DevObjP; 
	const char        *path;
	const char        *newname;
} FsmIoctlRenameT;

typedef struct
{
   FsmDevObjHdrT     *DevObjP; 
   const char        *path;
   uint16            Attrib;
} FsmIoctlSetAttribT;

typedef struct
{
   FsmDevObjHdrT  *DevObjP;  
   uint32         *total_space;
   uint32         *avail_space;
   uint32         *free_space;
} FsmIoctlGetFreeSpaceT;



/**************************************************************************
 *
 *      The following definitions are used by VFS internally.
 *
 **************************************************************************/

typedef struct
{
	char				VolName[MAX_VOLUME_NAME_LEN + 1];
	FsmDevObjHdrT *DevObjP;
	FsmFsDrvT	  *FsDrvP;
	uint16			RefCnt;
} FsmVolumeT;



/* Flags for Vfs File descriptor. */

#define VFD_FLAG_FREE			0x00
#define VFD_FLAG_IN_USE			0x01
#define VFD_FLAG_PENDING		0x02

typedef struct
{
   FsmVolumeT  *VolumeP;
   uint32      ErrorCode;
   uint32      LowerFd;
   HMSEM       MutexSem;
   uint8       Flag;
} FsmVfsFileDescriptorT;


typedef struct
{
	HFSMFILE		vfd;
	char			pattern[MAX_FILE_NAME_LEN + 1];
} FsmFileEnumT;


/*----------------------------------------------------------*/



#endif /* __FSM_VFS_H__ */







