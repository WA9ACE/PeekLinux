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
*   FILE NAME:      pathmgr.c
*
*   DESCRIPTION:    This file contains the code for managing the current folder.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/


#include "bthal_fs.h"
#include "pathmgr.h"
#include "btl_defs.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_FTP);


/****************************************************************************
 *
 * Local RAM data
 *
 ****************************************************************************/
/* 
 * In the file transfer application, the path manager is responsible for 
 * tracking the correct path/folder for each component of the application.
 * The three components are; the client, the object push server and the file
 * transfer server. The client folder is fixed at the location from which
 * the application was executed. The inbox server folder is fixed in the 
 * inbox. The file transfer server folder varies as set path requests are 
 * received. This folder is the application's current folder.
 */
static BtlUtf8 rootFolder[PATHMGR_MAX_PATH];   		/* Holds root folder */
static BtlUtf8 currentFolder[PATHMGR_MAX_PATH];     /* Holds current folder */
static BtlUtf8 newFolder[PATHMGR_MAX_PATH];       	/* New sub-folder request */
static U8   newFolderFlags;                    		/* Options & state for newFolder */
static U16  newFolderLen;

/* New Folder Flags */
#define NFF_NONE                0x00    /* == OSPF_NONE */
#define NFF_BACKUP              0x01    /* == OSPF_BACKUP */
#define NFF_DONT_CREATE         0x02    /* == OSPF_DONT_CREATE */    
#define NFF_UNICODE_ODD         0x04    /* Used by AppendUni() */

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 *            PATHMGR_Init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the path manager.
 *
 * Return:    TRUE - The path manager is initialized.
 *            FALSE - Could not create the inbox.
 *
 */
BOOL PATHMGR_Init()
{
    newFolderFlags = NFF_NONE;
    newFolder[0] = 0;
    newFolderLen = 0;

    return TRUE;
}

/*---------------------------------------------------------------------------
 *            PATHMGR_Set
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set the current folder to the specified location.
 *
 * Return:    TRUE - success
 *            FALSE - failed
 *
 */
BOOL PATHMGR_Set(const BtlUtf8* newFolder)
{
	if (PATHMGR_IsFolderExists(newFolder) == FALSE)
		return (FALSE);

	/* Set new current folder */
	OS_StrnCpyUtf8(currentFolder, newFolder, (PATHMGR_MAX_PATH - 1));
	currentFolder[PATHMGR_MAX_PATH - 1] = '\0';

	return (TRUE);
}

/*---------------------------------------------------------------------------
 *            PATHMGR_Switch2Root
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set current folder to clients folder.
 *
 * Return:    TRUE - success
 *            FALSE - failed
 *
 */
BOOL PATHMGR_Switch2Root(void)
{	
    return PATHMGR_Set(rootFolder);
}

/*---------------------------------------------------------------------------
 *            PATHMGR_SetRootFolder
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set root folder.
 *
 * Return:    TRUE - success
 *            FALSE - failed
 *
 */
BOOL PATHMGR_SetRootFolder(const BtlUtf8* newRootFolder)
{	
	if (PATHMGR_IsFolderExists(newRootFolder) == FALSE)
		return (FALSE);
	
	BTL_LOG_INFO(("PATHMGR: Root folder is set to \"%s\".", newRootFolder));

	/* Set new root folder */
	OS_StrnCpyUtf8(rootFolder, newRootFolder, (PATHMGR_MAX_PATH - 1));
	rootFolder[PATHMGR_MAX_PATH - 1] = '\0';

	/* Now actually switch to root folder */
	PATHMGR_Switch2Root();
	
	return (TRUE);
}

/*---------------------------------------------------------------------------
 *            PATHMGR_IsFolderExists
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns if a folder with the given name exists in the 
 *				current folder.
 *
 * Return:    TRUE - success
 *            FALSE - failed
 *
 */
BOOL PATHMGR_IsFolderExists(const BtlUtf8* folder)
{	
	U16 		len;
	BthalFsStat stat;
	
	if ((folder == 0) || ((len = OS_StrLenUtf8(folder)) == 0) || (len > (PATHMGR_MAX_PATH - 1)))
	{
		return (FALSE);
	}

	if ((BTHAL_FS_Stat(folder, &stat) == BT_STATUS_HAL_FS_SUCCESS) && (stat.type == BTHAL_FS_DIR))
		return (TRUE);

	return (FALSE);
}

/*---------------------------------------------------------------------------
 *            PATHMGR_Switch2NewFolder
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set file transfer server folder to new folder.
 *
 * Return:    3 - success
 *            2 - Failed since the folder wasn't found (and the client didn't ask to create it),
 *			  1 - Failed since the folder wasn't found, the client asked to create it, but the server is in read only mode.
 *			  0 - Failed because of an error.	
 */
U8 PATHMGR_Switch2NewFolder(BOOL readOnly)
{
    U32	    status = 0;
	BOOL 	tryCreate = FALSE; 
	U16 	len, newLen;
	BtlUtf8 	newPath[PATHMGR_MAX_PATH];

    if (newFolderFlags & NFF_UNICODE_ODD) {
        BTL_LOG_INFO(("PATHMGR: Received Unicode Pathname is malformed.\n"));
        return 0;
    }

    if (newFolderFlags & NFF_BACKUP) {
		if (OS_StrCmpUtf8(rootFolder, currentFolder) == 0)
		{
			BTL_LOG_INFO(("PATHMGR: Error, trying to backup from root folder."));
			status = 0;
			goto exit;
		}

		len = OS_StrLenUtf8(currentFolder);
		newLen = OS_StrLenUtf8(rootFolder);

		/* Backup to parent folder */
		while (len > 0)
		{
			if (currentFolder[len-1] == BTHAL_FS_PATH_DELIMITER)
			{
				if (newLen == len)
				{
					/* This is root folder, leave delimiter */
					currentFolder[len] = '\0';
				}
				else
				{
					currentFolder[len-1] = '\0';
				}
				break;
			}
			
			len--;
		}
		status = 3;
    }

    if (newFolderLen) {
		len = OS_StrLenUtf8(currentFolder);
		newLen = OS_StrLenUtf8(newFolder);

		/* Check if delimiter already present in current folder */
		if (currentFolder[len - 1] == BTHAL_FS_PATH_DELIMITER)
			len--;
		
		if ((len + newLen + 2) > (PATHMGR_MAX_PATH - 1))
		{
			status = 0;
			goto exit;
		}
		else
		{
			OS_StrnCpyUtf8(newPath, currentFolder, len);
			newPath[len] = BTHAL_FS_PATH_DELIMITER;
			OS_StrnCpyUtf8((newPath + len + 1), newFolder, newLen);
			newPath[len + newLen + 1] = '\0';

			if (PATHMGR_Set(newPath) == FALSE)
				tryCreate = TRUE;
			else
				status = 3;
		}
			
		
        if (TRUE == tryCreate) {
            /* Couldn't change to newFolder, if we can create it, try that. */
            if (newFolderFlags & NFF_DONT_CREATE)
			{
				status = 2;
                goto exit;
            }
			if (readOnly == TRUE)
			{
				status = 1;
                goto exit;
            }
            if ((BTHAL_FS_Mkdir(newPath) != BT_STATUS_HAL_FS_SUCCESS) || (PATHMGR_Set(newPath) == FALSE)) {
                /* Couldn't create newFolder either, status is already set. */
                goto exit;
            }
            /* Successfully created new folder. */
            status = 3;
        }
    }

exit:
    newFolder[0] = 0;
    newFolderLen = 0;
    newFolderFlags = NFF_NONE;

    return ((U8)status);
}

BOOL PATHMGR_AppendNewAscii(const U8 *Path, U16 Len)
{
    /* Check if the appended name is going to overflow our buffer. */
    if ((newFolderLen + Len) > (PATHMGR_MAX_PATH - 1)) {
        return FALSE;
    }
    Assert ((newFolderFlags & NFF_UNICODE_ODD) == 0);

    OS_MemCopy(newFolder+newFolderLen, Path, Len);
    newFolderLen = (U16)(newFolderLen + Len);

    return TRUE;
}

/*---------------------------------------------------------------------------
 *            PATHMGR_SetFlags()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets optional backup and don't create flags before applying
 *            new folder.
 *
 */
void PATHMGR_SetFlags(U8 setPathFlags)
{
    newFolderFlags |= (setPathFlags & (NFF_BACKUP|NFF_DONT_CREATE));
}

/*---------------------------------------------------------------------------
 *            PATHMGR_FlushNewPath()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Flush the stored new path information.
 *
 */
void PATHMGR_FlushNewPath()
{
    newFolder[0] = 0;
    newFolderLen = 0;
    newFolderFlags = NFF_NONE;
}

/*---------------------------------------------------------------------------
 *
 * Prototype:     const BtlUtf8 *PATHMGR_GetNewPath()
 *
 * Description:   Returns a pointer to the current newPath if it is valid.
 *
 */
const BtlUtf8 *PATHMGR_GetNewPath(void)
{
    if (newFolderFlags & NFF_UNICODE_ODD) 
        return 0;

    return newFolder;
}

/*---------------------------------------------------------------------------
 *
 * Prototype:     const BtlUtf8 *PATHMGR_GetCurrentPath()
 *
 * Description:   Returns a pointer to the current path of the path manager.
 *
 */
const BtlUtf8 *PATHMGR_GetCurrentPath(void)
{	
	return currentFolder;
}

/*---------------------------------------------------------------------------
 *            PATHMGR_IsFolderExists
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns if a file with the given name exists in the 
 *				current folder.
 *
 * Return:    TRUE - File exists
 *            FALSE - File does not exists.
 *
 */
BOOL PATHMGR_IsFileExists(const BtlUtf8* fileName)
{	
	U16 		len;
	BthalFsStat stat;
	
	if ((fileName == 0) || ((len = OS_StrLenUtf8(fileName)) == 0) || (len > (BTHAL_FS_MAX_FILE_NAME_LENGTH - 1)))
	{
	    return (FALSE);
	}

	if ((BTHAL_FS_Stat(fileName, &stat) == BT_STATUS_HAL_FS_SUCCESS))
	{
        return (TRUE);
    } 

	return (FALSE);
}
