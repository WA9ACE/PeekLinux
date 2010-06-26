/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      bthal_fs.c
*
*   DESCRIPTION:    This file contain implementation of file system in WIN
*
*   AUTHOR:         Yaniv Rabin
*
\*******************************************************************************/


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "ffs/ffs_api.h"
#include "bthal_fs.h"
#include "osapi.h"
#include "bthal_log.h"
#include "bthal_log_modules.h"

#define BTHAL_FS_LOG_ERROR(msg)	BTHAL_LOG_ERROR(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_FS, msg)	

/* ffs_closedir prototype */
S32 ffs_closedir(T_FFS_DIR *dir_p);

/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/
#define	_BTHAL_FS_MAX_DIRECTORY_HANDLE	2
#define _BTHAL_FS_TEMP_BUF_LEN			64
#define _BTHAL_FS_MIN_FREE_BYTES    	1024

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

 typedef struct _fsDirectoryHandle
 {
	BOOL			busy;
	BTHAL_U16		pathLen;
	BTHAL_U8		local_fname[BTHAL_FS_MAX_PATH_LENGTH + BTHAL_FS_MAX_FILE_NAME_LENGTH];
	T_FFS_DIR 		searchHandle;
 } fsDirectoryHandle;


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

static fsDirectoryHandle	DirectoryStructureArray[_BTHAL_FS_MAX_DIRECTORY_HANDLE+1];

/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/

static BTHAL_INT	FsGetFreeScpaceInDirectoryStructurArray(void);
static BtFsStatus	FsConvertLocostoErrorToFsError(BTHAL_S8 ret);
static BtFsStatus	FsCheckIfDirectoryHandleIsInValidAndRange(const BthalFsDirDesc dirDesc);
static void			FsConvertLocostoTimeToOBEX_ISO(BTHAL_U32 st_time,BTHAL_U8* Time);
static const char	*pFFSRetVal(BTHAL_S8 retVal);


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTHAL_FS_Init()
 *
 *	Synopsis:  int FS set zero to all FileStructureArray and DirectoryStructureArray
 * 
 * 	Returns:
 *		BT_STATUS_HAL_FS_SUCCESS - Operation is successful.
 *
 */
BtFsStatus BTHAL_FS_Init( BthalCallBack	callback)
{
	Report(("BTHAL_FS: Init"));
	
	OS_MemSet((void*)DirectoryStructureArray, 0, (_BTHAL_FS_MAX_DIRECTORY_HANDLE * sizeof(fsDirectoryHandle)));

	return BT_STATUS_HAL_FS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTHAL_FS_DeInit()
 *
 *	Synopsis:  
 * 
 * 	Returns:
 *		BT_STATUS_HAL_FS_SUCCESS - Operation is successful.
 *
 */
BtFsStatus BTHAL_FS_DeInit( void )
{
	return BT_STATUS_HAL_FS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            FsGetFreeScpaceInDirectoryStructurArray
 *---------------------------------------------------------------------------
 *
 * Synopsis:  get the first free space in Directory structure array
 *
 * Return:    int - cell number 
 *
 */ 
static BTHAL_INT FsGetFreeScpaceInDirectoryStructurArray(void)
{
	BTHAL_INT index;

	for (index = 1;index <= _BTHAL_FS_MAX_DIRECTORY_HANDLE;index++)
	{
		if (DirectoryStructureArray[index].busy == 0)
		{
			DirectoryStructureArray[index].busy = 1;
			return index;
		}
	}
	return BTHAL_FS_INVALID_DIRECTORY_DESC;
}

/*---------------------------------------------------------------------------
 *            FsCheckIfDirectoryHandleIsInRange
 *---------------------------------------------------------------------------
 *
 * Synopsis:  get the first free space in Directory structure array
 *
 * 	Returns:	BT_STATUS_HAL_FS_SUCCESS - if successful,
 *				BT_STATUS_HAL_FS_ERROR_DIRECTORY_HANDLE_OUT_OF_RANGE - out of range.   
 *				BT_STATUS_HAL_FS_ERROR_INVALID_DIRECTORY_HANDLE_VALUE - Invalid handle. 
 */ 

static BtFsStatus FsCheckIfDirectoryHandleIsInValidAndRange(const BthalFsDirDesc dirDesc)
{
	if (dirDesc > _BTHAL_FS_MAX_DIRECTORY_HANDLE)
		return BT_STATUS_HAL_FS_ERROR_DIRECTORY_HANDLE_OUT_OF_RANGE;

	if (DirectoryStructureArray[dirDesc].busy == 0)
		return BT_STATUS_HAL_FS_ERROR_INVALID_DIRECTORY_HANDLE_VALUE;

	return BT_STATUS_HAL_FS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            FsConvertWinErrorToFsError
 *---------------------------------------------------------------------------
 *
 * Synopsis:  convert win error to fs error
 *
 * Return:    BtFsStatus
 *
 */ 
static BtFsStatus FsConvertLocostoErrorToFsError(BTHAL_S8 ret)
{
	BtFsStatus btFsStatus;
	
	switch(ret)
	{
	
	case EFFS_ACCESS:
		btFsStatus =  BT_STATUS_HAL_FS_ERROR_ACCESS_DENIED;//Permission denied
		break;
	case EFFS_DIRNOTEMPTY:
		btFsStatus =  BT_STATUS_HAL_FS_ERROR_DIRECTORY_NOT_EMPTY;//Permission denied
		break;
	case EFFS_BADFD:
		btFsStatus = BT_STATUS_HAL_FS_ERROR_FILE_HANDLE;//Bad file number/ handle
		break;		
	case EFFS_EXISTS:
		btFsStatus = BT_STATUS_HAL_FS_ERROR_EXISTS;//File exists
		break;		
	case EFFS_INVALID:
		btFsStatus = BT_STATUS_HAL_FS_ERROR_INVALID;//Invalid argument
		break;
	case EFFS_NOTFOUND:
		btFsStatus = BT_STATUS_HAL_FS_ERROR_NOTFOUND;//No such file or directory
		break;
	default:
		btFsStatus = BT_STATUS_HAL_FS_ERROR_GENERAL;
		break;
	}

	return btFsStatus;
}

/*---------------------------------------------------------------------------
 *            BTHAL_FS_Open
 *---------------------------------------------------------------------------
 *
 * Synopsis:  the file name should include all file-system and drive specification as
 *			   used by convention in the target platform.
 *
 * Return:    BT_STATUS_HAL_FS_SUCCESS if success, BT_STATUS_HAL_FS_ERROR_OPENING_FILE otherwise
 *
 */
BtFsStatus BTHAL_FS_Open(const BTHAL_U8* fullPathFileName, BthalFsOpenFlags flags, BthalFsFileDesc *fd)
{
	BTHAL_INT	filehandle;
	BTHAL_U16	openFlags;

	openFlags = 0;
	*fd = BTHAL_FS_INVALID_FILE_DESC;
	
	if(flags & BTHAL_FS_O_APPEND)
		openFlags = openFlags | FFS_O_APPEND;
	if(flags & BTHAL_FS_O_CREATE)
		openFlags = openFlags | FFS_O_CREATE;
	if(flags & BTHAL_FS_O_EXCL)
		openFlags = openFlags | FFS_O_EXCL;
	if(flags & BTHAL_FS_O_RDONLY)
		openFlags = openFlags | FFS_O_RDONLY;
	if(flags & BTHAL_FS_O_RDWR)
		openFlags = openFlags | FFS_O_RDWR;
	if(flags & BTHAL_FS_O_TRUNC)
		openFlags = openFlags | FFS_O_TRUNC;
	if(flags & BTHAL_FS_O_WRONLY)
		openFlags = openFlags | FFS_O_WRONLY;

	filehandle = ffs_open((const char *)fullPathFileName,openFlags);

	if (filehandle >= 0)
	{
		*fd = filehandle;
		return BT_STATUS_HAL_FS_SUCCESS;
	}

	return FsConvertLocostoErrorToFsError((S8)filehandle);
}


/*---------------------------------------------------------------------------
 *            BTHAL_FS_Close
 *---------------------------------------------------------------------------
 *
 * Synopsis:  close file
 *
 * Return:    BT_STATUS_HAL_FS_SUCCESS if success,
 *			  other - if failed.
 *
 */
BtFsStatus BTHAL_FS_Close(const BthalFsFileDesc fd )
{
	BTHAL_S8 ret = ffs_close(fd);

	if(ret != 0)
	{
		return FsConvertLocostoErrorToFsError(ret);
	}

	return BT_STATUS_HAL_FS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            BTHAL_FS_Read
 *---------------------------------------------------------------------------
 *
 * Synopsis:  read file
 *
 * Return:    BT_STATUS_HAL_FS_SUCCESS if success,
 *			  other - if failed.
 *
 */
BtFsStatus BTHAL_FS_Read ( const BthalFsFileDesc fd, void* buf, BTHAL_U32 nSize, BTHAL_U32 *numRead )
{
	*numRead = ffs_read(fd, buf, nSize);

	if (*numRead > 0)
	{
		return BT_STATUS_HAL_FS_SUCCESS;
	}

	return FsConvertLocostoErrorToFsError((S8)(*numRead));
}

/*---------------------------------------------------------------------------
 *            BTHAL_FS_Write
 *---------------------------------------------------------------------------
 *
 * Synopsis:  write file
 *
 * 	Returns:	BT_STATUS_HAL_FS_SUCCESS - if successful,
 *				other - if failed.
 */
BtFsStatus BTHAL_FS_Write( const BthalFsFileDesc fd, void* buf, BTHAL_U32 nSize, BTHAL_U32 *numWritten )
{
	BTHAL_S32 freeBytes = 0;
	
	/* Check if there is enough space on FFS */
	ffs_query(Q_BYTES_FREE, (void *)&freeBytes);
	if (freeBytes - _BTHAL_FS_MIN_FREE_BYTES < nSize) 
	{
		BTHAL_FS_LOG_ERROR(("BTHAL_FS : Not enought space left on device"));
		return BT_STATUS_HAL_FS_ERROR_FSFULL;
	}
	

	*numWritten = ffs_write(fd, buf, nSize);

	if (*numWritten == nSize)
	{
		return BT_STATUS_HAL_FS_SUCCESS;
	}

	return FsConvertLocostoErrorToFsError((S8)(*numWritten));
}

/*---------------------------------------------------------------------------
 *            BTHAL_FS_Tell
 *---------------------------------------------------------------------------
 *
 * Synopsis:  gets the current position of a file pointer.
 *
 * 	Returns:	BT_STATUS_HAL_FS_SUCCESS - if successful,
 *				other -  Operation failed.
 */
BtFsStatus BTHAL_FS_Tell( const BthalFsFileDesc fd, BTHAL_U32 *curPosition )
{
	*curPosition = ffs_seek(fd, 0, FFS_SEEK_CUR);
	if (*curPosition > 0)
	{
		return BT_STATUS_HAL_FS_SUCCESS;
	}
	return FsConvertLocostoErrorToFsError(-1);
}

/*---------------------------------------------------------------------------
 *            BTHAL_FS_Seek
 *---------------------------------------------------------------------------
 *
 * Synopsis:  moves the file pointer to a specified location.
 *
 * 	Returns:	BT_STATUS_HAL_FS_SUCCESS - if successful,
 *				other -  Operation failed.
 */
BtFsStatus BTHAL_FS_Seek( const BthalFsFileDesc fd, BTHAL_S32 offset, BthalFsSeekOrigin from )
{
	BTHAL_INT origin;
	BTHAL_S32 ret;
	
	switch(from)
	{
	case BTHAL_FS_CUR:
		origin = FFS_SEEK_CUR;
		break;
		
	case BTHAL_FS_END:
		origin = FFS_SEEK_END;
	    break;
		
	case BTHAL_FS_START:
		origin = FFS_SEEK_SET;
	    break;
		
	default:
		origin = FFS_SEEK_CUR;
	    break;
	}
	
	ret = ffs_seek(fd, offset, origin);
	if (ret < 0)
	{
		return FsConvertLocostoErrorToFsError((S8)ret);
	}

	return BT_STATUS_HAL_FS_SUCCESS;
}


/*---------------------------------------------------------------------------
 *            BTHAL_FS_Flush
 *---------------------------------------------------------------------------
 *
 * Synopsis:  flush write buffers from memory to file
 *
 * 	Returns:	BT_STATUS_HAL_FS_SUCCESS - if successful,
 *				BT_STATUS_HAL_FS_ERROR_GENERAL - if failed.
 */
BtFsStatus BTHAL_FS_Flush( const BthalFsFileDesc fd )
{
	BTHAL_S8 ret = ffs_fdatasync(fd);
	if (ret != 0)
	{
		return FsConvertLocostoErrorToFsError(-1);
	}

	return BT_STATUS_HAL_FS_SUCCESS;
}


/*---------------------------------------------------------------------------
 *            BTHAL_FS_Stat
 *---------------------------------------------------------------------------
 *
 * Synopsis:  get information of a file or folder - name, size, type, 
 *            created/modified/accessed time, and Read/Write/Delete         . 
 *            access permissions.
 *
 * Returns:	BT_STATUS_HAL_FS_SUCCESS - if successful,
 *				other -  Operation failed.
 */
BtFsStatus BTHAL_FS_Stat( const BTHAL_U8* fullPathName, BthalFsStat* fileStat )
{
	BTHAL_S8 ret;
	T_FFS_STAT stat;
	
	ret = ffs_stat((const char *)fullPathName, &stat);
	
	if (ret != 0)
	{
		return FsConvertLocostoErrorToFsError(ret);
	}
	
	/* Get file/dir size */ 
    fileStat->size = stat.size;
	
    /* Set either file or directory field */
    if (stat.type != OT_DIR)
	{
		fileStat->type = BTHAL_FS_FILE;
	}
	else
	{
		fileStat->type = BTHAL_FS_DIR;
	}
	
    /* if true read only */
     fileStat->isReadOnly = (stat.flags & FFS_O_WRONLY) ? FALSE: TRUE;

    
    /* ***************************************************************/ 
    /* Extract user/group/other file or directory access permissions */
    /* ***************************************************************/ 

	/* From ffs documentation only one of the next flags can be:
	 * FFS_O_RDWR, FFS_O_WRONLY, FFS_O_RDONLY */
	 
	if (stat.flags & FFS_O_RDWR)
	{
		/* We add Delete permission which is suitable to locosto. */
        fileStat->userPerm  = BTHAL_FS_PERM_READ|BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;
        fileStat->userPerm  = BTHAL_FS_PERM_READ|BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;
        fileStat->userPerm  = BTHAL_FS_PERM_READ|BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;

        /* The group that owns file (UNIX-specific) */
        fileStat->groupPerm = BTHAL_FS_PERM_READ|BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;
        fileStat->groupPerm = BTHAL_FS_PERM_READ|BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;
        fileStat->groupPerm = BTHAL_FS_PERM_READ|BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;

        /* The other that owns file (UNIX-specific) */
        fileStat->otherPerm = BTHAL_FS_PERM_READ|BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;
        fileStat->otherPerm = BTHAL_FS_PERM_READ|BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;
        fileStat->otherPerm = BTHAL_FS_PERM_READ|BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;

	}
	else if (stat.flags & FFS_O_WRONLY)
	{
		/* We add Delete permission which is suitable to locosto. */
        fileStat->userPerm  = BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;
        fileStat->userPerm  = BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;
        fileStat->userPerm  = BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;

        /* The group that owns file (UNIX-specific) */
        fileStat->groupPerm = BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;
        fileStat->groupPerm = BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;
        fileStat->groupPerm = BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;

        /* The other that owns file (UNIX-specific) */
        fileStat->otherPerm = BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;
        fileStat->otherPerm = BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;
        fileStat->otherPerm = BTHAL_FS_PERM_WRITE|BTHAL_FS_PERM_DELETE;
	}
	else if (stat.flags & FFS_O_RDONLY)
	{
		/* We add Delete permission which is suitable to locosto. */
        fileStat->userPerm  = BTHAL_FS_PERM_READ;
        fileStat->userPerm  = BTHAL_FS_PERM_READ;
        fileStat->userPerm  = BTHAL_FS_PERM_READ;

        /* The group that owns file (UNIX-specific) */
        fileStat->groupPerm = BTHAL_FS_PERM_READ;
        fileStat->groupPerm = BTHAL_FS_PERM_READ;
        fileStat->groupPerm = BTHAL_FS_PERM_READ;

        /* The other that owns file (UNIX-specific) */
        fileStat->otherPerm = BTHAL_FS_PERM_READ;
        fileStat->otherPerm = BTHAL_FS_PERM_READ;
        fileStat->otherPerm = BTHAL_FS_PERM_READ;
	}

    /* ***************************************************************/ 
    /* Note: Creation/Modified/Accessed time is set to (in UTC):     */
    /* 'Monday, May 07, 2007, 5:47:05 PM'                            */ 
    /* ***************************************************************/ 

    /* ***************************************************************/ 
    /* Extract file creation fields (in UTC) */
    /* ***************************************************************/ 

    fileStat->cTime.day = 0x0007;
    fileStat->cTime.hour = 0x000e;
    fileStat->cTime.minute = 0x002f;
    fileStat->cTime.month = 0x0005;
    fileStat->cTime.second = 0x0005;
    fileStat->cTime.year = 0x07d7;
    fileStat->cTime.utcTime = TRUE;

    /* ***************************************************************/ 
    /* Extract file modified fields (in UTC) */
    /* ***************************************************************/ 

    fileStat->mTime.day = 0x0007;
    fileStat->mTime.hour = 0x000e;
    fileStat->mTime.minute = 0x002f;
    fileStat->mTime.month = 0x0005;
    fileStat->mTime.second = 0x0005;
    fileStat->mTime.year = 0x07d7;
    fileStat->mTime.utcTime = TRUE;

    /* ***************************************************************/ 
    /* Extract file accessed fields (in UTC) */
    /* ***************************************************************/ 

    fileStat->aTime.day = 0x0007;
    fileStat->aTime.hour = 0x000e;
    fileStat->aTime.minute = 0x002f;
    fileStat->aTime.month = 0x0005;
    fileStat->aTime.second = 0x0005;
    fileStat->aTime.year = 0x07d7;
    fileStat->aTime.utcTime = TRUE;

	return BT_STATUS_HAL_FS_SUCCESS;	
}

/*---------------------------------------------------------------------------
 *            BTHAL_FS_Mkdir
 *---------------------------------------------------------------------------
 *
 * Synopsis:  make dir
 *
 * 	Returns:	BT_STATUS_HAL_FS_SUCCESS - if successful,
 *				other - if failed 
 */
BtFsStatus BTHAL_FS_Mkdir( const BTHAL_U8 *dirFullPathName ) 
{
	BTHAL_S8 ret = ffs_mkdir((const char *)dirFullPathName);
	
	if (ret != 0)
	{
		return FsConvertLocostoErrorToFsError(ret);
	}

	return BT_STATUS_HAL_FS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            BTHAL_FS_Rmdir
 *---------------------------------------------------------------------------
 *
 * Synopsis:  remove dir
 *
 * 	Returns:	BT_STATUS_HAL_FS_SUCCESS - if successful,
 *				other - if failed 
 */
BtFsStatus BTHAL_FS_Rmdir(const BTHAL_U8 *dirFullPathName)
{
	BTHAL_S8 ret = ffs_remove((const char *)dirFullPathName);
	
	if (ret != 0)
	{
		return FsConvertLocostoErrorToFsError(ret);
	}

	return BT_STATUS_HAL_FS_SUCCESS;
}
/*---------------------------------------------------------------------------
 *            BTHAL_FS_OpenDir
 *---------------------------------------------------------------------------
 *
 * Synopsis:  open a directory for reading,
 *
 * 	Returns:	BT_STATUS_HAL_FS_SUCCESS - if successful,
 *				BT_STATUS_HAL_FS_ERROR_INVALID_DIRECTORY_HANDLE_VALUE - if failed 
 */
BtFsStatus BTHAL_FS_OpenDir(const BTHAL_U8 *dirFullPathName, BthalFsDirDesc *dirDesc)
{
	BTHAL_INT	directoryHandle;
	BTHAL_S32 ret;
	
	*dirDesc = (BthalFsDirDesc)BTHAL_FS_INVALID_DIRECTORY_DESC;

	if((directoryHandle = FsGetFreeScpaceInDirectoryStructurArray()) == BTHAL_FS_INVALID_DIRECTORY_DESC)
		return BT_STATUS_HAL_FS_ERROR_MAX_DIRECTORY_HANDLE;

	ret = ffs_opendir((const char *)dirFullPathName, &(DirectoryStructureArray[directoryHandle].searchHandle));

	if (ret < 0)
	{
		return BT_STATUS_HAL_FS_ERROR_INVALID_DIRECTORY_HANDLE_VALUE;
	}

	DirectoryStructureArray[directoryHandle].pathLen = OS_StrLen((const char *)dirFullPathName);
	
	if (DirectoryStructureArray[directoryHandle].pathLen == 1)
	{
		OS_StrCpy((char *)DirectoryStructureArray[directoryHandle].local_fname, "/");
	}
	else
	{
		/* Check if delimiter already present in current path */
		if (dirFullPathName[DirectoryStructureArray[directoryHandle].pathLen - 1] == BTHAL_FS_PATH_DELIMITER)
		{
			DirectoryStructureArray[directoryHandle].pathLen--;
		}

		OS_StrnCpy((char *)(DirectoryStructureArray[directoryHandle].local_fname),
							(const char *)dirFullPathName,
							DirectoryStructureArray[directoryHandle].pathLen);

		DirectoryStructureArray[directoryHandle].local_fname
								[DirectoryStructureArray[directoryHandle].pathLen++] = '\0';

		OS_StrCat((char *)DirectoryStructureArray[directoryHandle].local_fname, "/");
	}
	 	
	*dirDesc = directoryHandle;
	
	return 	BT_STATUS_HAL_FS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTHAL_FS_ReadDir()
 *
 *	Synopsis:  get first/next file name in a directory. return the full path of the file
 *
 * 	Returns:
 *		BT_STATUS_HAL_FS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_HAL_FS_ERROR_FIND_NEXT_FILE -  Operation failed.
 */

BtFsStatus BTHAL_FS_ReadDir( const BthalFsDirDesc dirDesc, BTHAL_U8 **fileName )
{
	BtFsStatus	sts;
	BTHAL_S32	ret;
	BTHAL_U8	tempFlieName[BTHAL_FS_MAX_FILE_NAME_LENGTH];
	
	/* check if file directory valid and in range */
	if((sts = FsCheckIfDirectoryHandleIsInValidAndRange(dirDesc)) != BT_STATUS_HAL_FS_SUCCESS)
		return sts;

	if (DirectoryStructureArray[dirDesc].busy == 0)
	{
		return BT_STATUS_HAL_FS_ERROR_INVALID_DIRECTORY_HANDLE_VALUE;
	}
	
	ret = ffs_readdir(&(DirectoryStructureArray[dirDesc].searchHandle),
						(char *)(tempFlieName),
						BTHAL_FS_MAX_FILE_NAME_LENGTH);

	if (ret < 1)
	{
		return BT_STATUS_HAL_FS_ERROR_INVALID_DIRECTORY_HANDLE_VALUE;
	}

	DirectoryStructureArray[dirDesc].local_fname[DirectoryStructureArray[dirDesc].pathLen] = '\0';
	OS_StrCat((char *)DirectoryStructureArray[dirDesc].local_fname, (const char *)tempFlieName);
	
	*fileName = DirectoryStructureArray[dirDesc].local_fname;

	return BT_STATUS_HAL_FS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTHAL_FS_CloseDir()
 *
 *	Synopsis:  set the busy flag to 0 -> free the cell. 
 *
 * 	Parameters:
 *		dirDesc [in] - points to Directory handle .
 *
 * 	Returns:
 *		BT_STATUS_HAL_FS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_HAL_FS_ERROR_GENERAL - -  Operation failed.
 *
 */
BtFsStatus BTHAL_FS_CloseDir( const BthalFsDirDesc dirDesc )
{
	BtFsStatus sts;
	BTHAL_S32 ret;
	
	/* check if directory handle valid and in range */
	if((sts = FsCheckIfDirectoryHandleIsInValidAndRange(dirDesc)) != BT_STATUS_HAL_FS_SUCCESS)
		return sts;

	/* free the cell */
	DirectoryStructureArray[dirDesc].busy = 0;

 	/* check dir_p pointer */
 	if(&(DirectoryStructureArray[dirDesc].searchHandle) == 0)
 	{
		return BT_STATUS_HAL_FS_ERROR_INVALID_DIRECTORY_HANDLE_VALUE;
 	}

	/* close the directory */
	//ret = ffs_closedir(&(DirectoryStructureArray[dirDesc].searchHandle));
	ret = ffs_close((fd_t)dirDesc);

	if (ret != 0)
	{
		return BT_STATUS_HAL_FS_ERROR_INVALID_DIRECTORY_HANDLE_VALUE;
	}

	return BT_STATUS_HAL_FS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 *
 *  BTHAL_FS_Rename
 *
 *  renames a file or a directory
 */
BtFsStatus BTHAL_FS_Rename(const BTHAL_U8 *fullPathOldName, const BTHAL_U8 *fullPathNewName )
{
	BTHAL_S8 ret = ffs_rename((const char *)fullPathOldName, (const char *)fullPathNewName);
	
	if (ret != 0)
	{
		return FsConvertLocostoErrorToFsError(ret);
	}
	
	return BT_STATUS_HAL_FS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 *
 *  BTHAL_FS_Remove
 *
 *  removes a file
 */ 
BtFsStatus BTHAL_FS_Remove( const BTHAL_U8 *fullPathFileName )
{
	BTHAL_S8 ret = ffs_remove((const char *)fullPathFileName);
	
	if (ret != 0)
	{
		return FsConvertLocostoErrorToFsError(ret);
	}

	return BT_STATUS_HAL_FS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 *
 * BTHAL_FS_IsAbsoluteName()
 *
 * This function checks whether a given file name has an absolute path name
 */
BtFsStatus BTHAL_FS_IsAbsoluteName( const BTHAL_U8 *fileName, BTHAL_BOOL *isAbsolute )
{
	if (fileName[0] == '/')
	{
		*isAbsolute = TRUE;
	}
	else
	{
		*isAbsolute = FALSE;
	}

	
	return BT_STATUS_HAL_FS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 *
 * FsConvertWinTimeToOBEX_ISO()
 *
 * This function convert win Time to OBEX ISO Time
 */
static void	FsConvertLocostoTimeToOBEX_ISO(BTHAL_U32 st_time, BTHAL_U8 *Time)
{
	BTHAL_UTILS_MemSet(Time, '0', 16);
}

/*---------------------------------------------------------------------------
 *            pFFSRetVal
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return a pointer to a description of the FFS return value code.
 *
 * Return:    ASCII String pointer.
 *
 */
const char *pFFSRetVal(BTHAL_S8 retVal)
{
    switch (retVal) 
	{
    case EFFS_OK:
        return "Ok";
    case EFFS_EXISTS:
        return "Object with the same name already exists";
    case EFFS_NAMETOOLONG:
        return "Object's name is too long";
    case EFFS_BADNAME:
        return "Object's name contains illegal characters";
    case EFFS_NOSPACE:
        return "Out of data space";
    case EFFS_FSFULL:
        return "Failed to allocate an inode for object";
	case EFFS_NUMFD:
        return "Max number of used file descriptors reached";
	case EFFS_NOTFOUND:
        return "No such object";
	case EFFS_INVALID:
        return "Bad open flag option or an erase operation is in progress";
	case EFFS_LOCKED:
        return "Object is locked";
	case EFFS_BADFD:
        return "Bad file descriptor";
	case EFFS_BADOP:
        return "The operation isn't allowed by the open flags";
	case EFFS_FILETOOBIG:
        return "The file is too big";
	case EFFS_ACCESS:
        return "File can not be modified";
	case EFFS_MAGIC:
        return "Magic number is incorrect";
	case EFFS_NODEVICE:
        return "The flash device is unknown";
	case EFFS_AGAIN:
        return "Previous ffs_preformat() has not finished yet";
	case EFFS_DIRNOTEMPTY:
        return "Directory is not empty";
	case EFFS_NOTADIR :
        return "Not a directory";
    }
    return "UNKNOWN";
}
