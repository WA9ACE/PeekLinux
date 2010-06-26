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
*   FILE NAME:      fstore.c
*
*   DESCRIPTION:    This file contains the code for accessing the filesystem.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/


#include "btl_config.h"
#if BTL_CONFIG_FTPS ==   BTL_CONFIG_ENABLED

/********************************************************************************/


#include "osapi.h"
#include "fstore.h"
#include "pathmgr.h"
#include "btl_defs.h"
#include "bthal_fs.h"
#include "btl_obex_utils.h"


BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_FTP);

/****************************************************************************
 *
 * Internal Function prototypes
 *
 ****************************************************************************/
static FtpObStoreHandle ObsNew(void);


/****************************************************************************
 *
 * Local RAM data
 *
 ****************************************************************************/
static FtpObStoreEntry obsEntries[FTP_OBS_MAX_NUM_ENTRIES];
static ListEntry    obsList;
static char* 		tempFile = "fstore.tmp";

 
/*---------------------------------------------------------------------------
 *            FSTORE_Init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the object store.
 *
 * Return:    TRUE - object store initialized.
 *            FALSE - could not init.
 *
 */
BOOL FSTORE_Init(void)
{
    U8 i;

    /* Initialize the object store entries */
    InitializeListHead(&obsList);

    for (i = 0; i < FTP_OBS_MAX_NUM_ENTRIES; i++) {
        InsertTailList(&obsList, &(obsEntries[i].node));
    }

    return TRUE;

}


/*---------------------------------------------------------------------------
 *            FSTORE_Create()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open an object store item for writing.
 *
 * Return:    Success or failure of create operation.
 *
 */
ObexRespCode FSTORE_Create(FtpObStoreHandle *ObsPtr, const BtlUtf8 *Name)
{
	U16 currentPathLen, tempLen;
	const BtlUtf8* currentPath;
	
    Assert( ObsPtr );
    
    if ((*ObsPtr = ObsNew()) == 0) {
        return OBRC_NOT_FOUND;
    }

    (*ObsPtr)->name = Name;

	/* We open a temp file to avoid accidentally clobbering an existing
     * file that already exists (if the operation doesn't complete 
     * successfully). We maintain a pointer to the real filename, which
     * is used when the file is closed.
     */
	currentPath = PATHMGR_GetCurrentPath();
	currentPathLen = OS_StrLenUtf8(currentPath);
	tempLen = OS_StrLenUtf8(tempFile);

	/* Check if delimiter already present in current path */
	if (currentPath[currentPathLen - 1] == BTHAL_FS_PATH_DELIMITER)
		currentPathLen--;

	OS_StrnCpyUtf8((*ObsPtr)->tmpName, currentPath, currentPathLen);
	(*ObsPtr)->tmpName[currentPathLen] = BTHAL_FS_PATH_DELIMITER;
	OS_StrnCpyUtf8(((*ObsPtr)->tmpName + currentPathLen + 1), tempFile, tempLen);
	(*ObsPtr)->tmpName[(currentPathLen + tempLen + 1)] = '\0';
    
    /* Open the file */
	if (BTHAL_FS_Open((*ObsPtr)->tmpName, 
						(BTHAL_FS_O_WRONLY | BTHAL_FS_O_CREATE | BTHAL_FS_O_TRUNC | BTHAL_FS_O_BINARY), 
						&(*ObsPtr)->fp) == BT_STATUS_HAL_FS_SUCCESS)
	{
		return OBRC_SUCCESS;
	}

    BTL_LOG_INFO(("FSTORE: Couldn't Create %s for writing.\n", (*ObsPtr)->name));
    FSTORE_Close( ObsPtr );

    return OBRC_NOT_FOUND;
}

/*---------------------------------------------------------------------------
 *            FSTORE_Open()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open and object store item for reading. If a name is not set
 *            the default object is opened.
 *
 * Return:    OBRC_SUCCESS - Open is successful. 
 *            OBRC_NOT_FOUND - Failed to open file.
 *
 */
ObexRespCode FSTORE_Open(FtpObStoreHandle *obs, const BtlUtf8 *Name)
{
	BthalFsStat stat;
	
    Assert( obs );

    if ((*obs = ObsNew()) == 0) {
        return OBRC_NOT_FOUND;
    }

    if (OS_StrLenUtf8(Name) == 0) {
        goto open_err;
    } else {
        (*obs)->name = Name;
    }

	/* Get lentgh of file */
    if (BTHAL_FS_Stat((*obs)->name, &stat) != BT_STATUS_HAL_FS_SUCCESS) {
        goto open_err;
    }

	(*obs)->fileLen = stat.size;

    /* Open the file for reading */
    if (BTHAL_FS_Open((*obs)->name, (BTHAL_FS_O_RDONLY | BTHAL_FS_O_BINARY), &((*obs)->fp)) != BT_STATUS_HAL_FS_SUCCESS) {
        goto open_err;
    }

    return OBRC_SUCCESS;

open_err:
    BTL_LOG_INFO(("FSTORE: Couldn't Open %s for reading.\n", (*obs)->name));
    FSTORE_Close( obs );

    return OBRC_NOT_FOUND;

}


/*---------------------------------------------------------------------------
 *            FSTORE_GetObjectLen()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get the length of an object store item.
 *
 * Return:    The length of the object in bytes.
 *
 */
U32 FSTORE_GetObjectLen(FtpObStoreHandle obs)
{
    Assert(obs);
    return obs->fileLen;
}

/*---------------------------------------------------------------------------
 *            FSTORE_Write()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Write data to the given object store item.
 *
 * Return:    OBRC_SUCCESS -      Buffer was written successfully.
 *            OBRC_UNAUTHORIZED - Buffer was not written.
 *
 */
ObexRespCode FSTORE_Write(FtpObStoreHandle obs, U8 *buff, U16 len)
{
    ObexRespCode ret = OBRC_UNAUTHORIZED;
	BTHAL_U32 nBytes;
    BtFsStatus btFsStatus;

    if (obs != 0) {
        Assert(obs->fp != BTHAL_FS_INVALID_FILE_DESC);
		
        btFsStatus = BTHAL_FS_Write(obs->fp, buff, len, &nBytes);
        if (BT_STATUS_HAL_FS_SUCCESS == btFsStatus) 
        {
            ret = OBRC_SUCCESS;
        }
        else 
        {
            ret = BTL_OBEX_UTILS_ConvertFsErrorToObexError(btFsStatus);
        }
    }

    return ret;

}

/*---------------------------------------------------------------------------
 *            FSTORE_Read()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Read data from the given object store item.
 *
 * Return:    OBRC_SUCCESS -      Buffer was read successfully.
 *            OBRC_UNAUTHORIZED - Buffer was not read.
 *
 */
ObexRespCode FSTORE_Read(FtpObStoreHandle obs, U8* buff, U16 len)
{
    ObexRespCode ret = OBRC_UNAUTHORIZED;
	BTHAL_U32 nBytes;

    if (obs != 0) {
        Assert(obs->fp != BTHAL_FS_INVALID_FILE_DESC);
		if (BTHAL_FS_Read(obs->fp, buff, len, &nBytes) == BT_STATUS_HAL_FS_SUCCESS) {
            ret = OBRC_SUCCESS;
        } else {
        	BTL_LOG_ERROR(("Error reading from file! len %d\n", len));
        }
    }

    return ret;

}

/*---------------------------------------------------------------------------
 *            FSTORE_Delete()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Delete the given object store item. If the object is deleted
 *            by Name, then it must be in the current OBEX directory. The 
 *            Object store handle (if provided) is returned to the pool.
 *
 * Return:    OBRC_SUCCESS -   File was deleted.
 *            OBRC_NOT_FOUND - File was not deleted.
 *	     	  BRC_PRECONDITION_FAILED - Folder was not deleted since it is not empty
 */				
 
ObexRespCode FSTORE_Delete(FtpObStoreHandle *ObsPtr, const BtlUtf8 *Name)
{
	BtFsStatus status;
    if (ObsPtr && *ObsPtr) {
        /* We're deleting an open file, close it first */
        Assert ((*ObsPtr)->fp != BTHAL_FS_INVALID_FILE_DESC);

		BTHAL_FS_Close( (*ObsPtr)->fp );
        (*ObsPtr)->fp = BTHAL_FS_INVALID_FILE_DESC;

        /* Since it's an open file, we must have created it (as a temp). */
        Assert ((*ObsPtr)->tmpName[0] != 0);

        BTHAL_FS_Remove((*ObsPtr)->tmpName);
    
        /* Free the object handle */
        InsertTailList(&obsList, &((*ObsPtr)->node));
        *ObsPtr = 0;

        return OBRC_SUCCESS;
    }

    /* We're deleting a file which we haven't opened or created */
    Assert(Name);

    if ((BTHAL_FS_Remove(Name) != BT_STATUS_HAL_FS_SUCCESS) && 
		((status = BTHAL_FS_Rmdir(Name)) != BT_STATUS_HAL_FS_SUCCESS))
    	{
    		if (BT_STATUS_HAL_FS_ERROR_DIRECTORY_NOT_EMPTY == status) 
				return OBRC_PRECONDITION_FAILED;
			else		
		        return OBRC_NOT_FOUND;
    	}
    return OBRC_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            FSTORE_Close()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close the object store item and return the handle to the pool.
 *
 * Return:    Success or failure of create operation.
 *
 */
ObexRespCode FSTORE_Close(FtpObStoreHandle *obsPtr)
{
    ObexRespCode  rcode = OBRC_SUCCESS;
    FtpObStoreHandle obs = *obsPtr;

    Assert(obs);

    if (obs->fp != BTHAL_FS_INVALID_FILE_DESC) {
        BTHAL_FS_Close(obs->fp);
        obs->fp = BTHAL_FS_INVALID_FILE_DESC;

        /* If the file is a temporary and a new name is set, rename it. */
        if ((obs->tmpName[0] != 0) && (obs->name[0] != 0)) {
    
            /* Remove the (possibly) existing file before renaming it */
            BTHAL_FS_Remove(obs->name);

            if (BTHAL_FS_Rename(obs->tmpName, obs->name) != BT_STATUS_HAL_FS_SUCCESS) {
                BTL_LOG_INFO(("FSTORE: Unable to rename file from %s to %s\n", 
                        obs->tmpName, obs->name));

                /* Remove the temp file and return an error */
                BTHAL_FS_Remove(obs->tmpName);              
                rcode = OBRC_UNAUTHORIZED;
                goto close_done;
            }
        }
    }

close_done:
    /* Return the FtpObStoreHandle to the pool */
    InsertTailList(&obsList, &(obs->node));
    
    *obsPtr = 0;
    return rcode;
}

/*---------------------------------------------------------------------------
 *            ObsNew()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Obtain an object store item.
 *
 * Return:    Handle of object store or 0 if no more objects store entries.
 *
 */
static FtpObStoreHandle ObsNew(void)
{
    FtpObStoreHandle obs;

    if (!IsListEmpty(&obsList)) {
        obs = (FtpObStoreHandle)RemoveHeadList(&obsList);
        obs->fp = BTHAL_FS_INVALID_FILE_DESC;
        obs->name = 0;
        obs->fileLen = 0;
        obs->tmpName[0] = 0;
    } else {
        obs = 0;
        BTL_LOG_INFO(("ObsNew: Out of entries\n"));
    }

    return obs;
}




#endif  /*BTL_CONFIG_FTPS ==   BTL_CONFIG_ENABLED*/


