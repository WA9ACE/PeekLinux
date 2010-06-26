/****************************************************************************
 *
 * File:        ftpcstore.c
 *
 * Description: This file contains the code for the Object store used in 
 *              the GOEP Profiles application. 
 *
 * Created:     May 9, 1997
 *
 * $Project:XTNDAccess IrDA SDK$
 *
 * Copyright 1997-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems,
 * Inc.  This work contains confidential and proprietary information of
 * Extended Systems, Inc. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "btl_config.h"
#if BTL_CONFIG_FTPC ==   BTL_CONFIG_ENABLED

/****************************************************************************/

#include "bthal_fs.h"
#include "osapi.h"
#include "ftpcstore.h"
#include "btl_common.h"

#include "btl_defs.h"
#include "btl_obex_utils.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_FTPC);

/****************************************************************************
 *
 * Function prototypes
 *
 ****************************************************************************/
static void FtpcstoreFindFileName(const BtlUtf8 *source, BtlUtf8 *dest);
static void FtpcstoreFindPathName(const BtlUtf8 *source, BtlUtf8 *dest);
static U8*	FtpcstoreItoa(U32 val);
/****************************************************************************
 *
 * Local ROM data
 *
 ****************************************************************************/

/****************************************************************************
 * 
 * Local RAM data
 *
 ****************************************************************************/
static FtpcStoreEntry	ftpcstoreEntries[FTPCSTORE_MAX_NUM_ENTRIES];
static ListEntry    	ftpcstoreList;

static U8 FtpcstoreInitCounter = 0;

/*---------------------------------------------------------------------------
 *            FTPCSTORE_Init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the FTPCSTORE module.
 *
 * Return:    TRUE - module initialized.
 *            FALSE - failed initialization.
 *
 */
BOOL FTPCSTORE_Init(void)
{
	U8 i;
	if (FtpcstoreInitCounter == 0)
	{
		/* Initialize the object store entries */
		InitializeListHead(&ftpcstoreList);

		for (i = 0; i < FTPCSTORE_MAX_NUM_ENTRIES; i++) 
		{
	       	InsertTailList(&ftpcstoreList, &(ftpcstoreEntries[i].node));
			ftpcstoreEntries[i].index = i;
		}
	}

	FtpcstoreInitCounter++;
	
	return TRUE;
} 

/*---------------------------------------------------------------------------
 *            FTPCSTORE_Deinit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the FTPCSTORE module.
 *
 * Returns:       void
 */
void FTPCSTORE_Deinit(void)
{
	Assert( FtpcstoreInitCounter > 0 );
	
	FtpcstoreInitCounter--;
}


/*---------------------------------------------------------------------------
 *            FTPCSTORE_New()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Obtain an FTPCSTORE item.
 *
 * Return:    Handle of FTPCSTORE object or 0 if no more FTPCSTORE entries.
 *
 */
FtpcStoreHandle FTPCSTORE_New(void)
{
	FtpcStoreHandle ftpcstoreObject;

	ftpcstoreObject = 0;
	
	if (!IsListEmpty(&ftpcstoreList)) 
	{
		ftpcstoreObject = (FtpcStoreHandle) RemoveHeadList(&ftpcstoreList);
		ftpcstoreObject->fp = 0;
		ftpcstoreObject->flags = 0;
		ftpcstoreObject->nameLen = 0;
		ftpcstoreObject->name[0] = 0;
		ftpcstoreObject->fullName[0] = 0;
		ftpcstoreObject->tempFile[0] = 0;
		ftpcstoreObject->fileLen = 0;
		ftpcstoreObject->amount = 0;
		ftpcstoreObject->memAddr = 0;
		ftpcstoreObject->memSize = 0;
		ftpcstoreObject->memIndex = 0;
		ftpcstoreObject->object_location = BTL_OBJECT_LOCATION_FS;		
	} 
	else 
	{
		DebugPrint(("FTPCSTORE_New: Out of entries\n"));
	}
	return ftpcstoreObject;
}


/*---------------------------------------------------------------------------
 *            FTPCSTORE_Create()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open an FTPCSTORE item for writing.
 *
 * Return:    Success or failure of create operation.
 *
 */
ObexRespCode FTPCSTORE_Create(FtpcStoreHandle ftpcstoreObject)
{
	BtFsStatus	status;
	
	Assert( ftpcstoreObject );

	ftpcstoreObject->memSize	= 0;

	/* We open a temp file to avoid accidentally clobbering an existing
	  * file that already exists (if the operation doesn't complete 
	  * successfully). We maintain a pointer to the real filename, which
	  * is used when the file is closed.
	  */

	/* Create temp file name according to context	*/
	FtpcstoreFindPathName(ftpcstoreObject->fullName,ftpcstoreObject->tempFile );
	OS_StrCat((char*)ftpcstoreObject->tempFile, "ftpc.tmp");
	OS_StrCatUtf8(ftpcstoreObject->tempFile, FtpcstoreItoa(ftpcstoreObject->index));
	
	status = BTHAL_FS_Open((const BtlUtf8*)ftpcstoreObject->tempFile, 
		BTHAL_FS_O_WRONLY |BTHAL_FS_O_BINARY|BTHAL_FS_O_CREATE|BTHAL_FS_O_TRUNC,
		&ftpcstoreObject->fp);
     
	if (BT_STATUS_HAL_FS_SUCCESS != status) 
	{
		BTL_LOG_ERROR(("FTPCSTORE:Error creating %s\n", ftpcstoreObject->tempFile));
		
		return OBRC_NOT_FOUND;
	}
    
	return OBRC_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            FTPCSTORE_CreateMem()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  update ftpcstory stack to memory write
 *
 * Return:    Success or failure of create operation.
 *
 */
ObexRespCode FTPCSTORE_CreateMem(FtpcStoreHandle ftpcstoreObject,const char	*memAddr,U32 size)
{
	
	Assert( ftpcstoreObject );

	ftpcstoreObject->memAddr	= memAddr;
	ftpcstoreObject->memSize	= size;	
	ftpcstoreObject->memIndex	= 0;
	
	return OBRC_SUCCESS;
} 

/*---------------------------------------------------------------------------
 *            FTPCSTORE_Open()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open an FTPCSTORE item for reading. If a name is not set
 *            the default object is opened.
 *
 * Return:    OBRC_SUCCESS - Open is successful. 
 *            OBRC_NOT_FOUND - Failed to open file.
 *
 */
ObexRespCode FTPCSTORE_Open(FtpcStoreHandle ftpcstoreObject)
{
	BthalFsStat FileStat;
	BtFsStatus status;

	/* Get data associated with "stat.c": */
	status = BTHAL_FS_Stat( ftpcstoreObject->fullName, &FileStat );

	/* Check if statistics are valid: */
	if( status != BT_STATUS_HAL_FS_SUCCESS )
		goto open_err;
	
	ftpcstoreObject->fileLen = FileStat.size;

	status = BTHAL_FS_Open( ftpcstoreObject->fullName, BTHAL_FS_O_RDONLY | BTHAL_FS_O_BINARY, &ftpcstoreObject->fp);

open_err:
    
	if (BT_STATUS_HAL_FS_SUCCESS != status) 
	{
		return OBRC_NOT_FOUND;
	} 
    
	return OBRC_SUCCESS;
}


/*---------------------------------------------------------------------------
 *            FTPCSTORE_AppendNameAscii()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Append the ASCII string to the name. 
 *
 * Return:    void
 *
 */
void FTPCSTORE_AppendNameAscii(FtpcStoreHandle ftpcstoreObject, const BtlUtf8* fullName)
{

	FtpcstoreFindFileName(fullName, ftpcstoreObject->name); /* find just file name from whole path	*/
	ftpcstoreObject->nameLen = OS_StrLenUtf8(ftpcstoreObject->name);

	OS_StrCpyUtf8(ftpcstoreObject->fullName, fullName);	/* fill field of fullName	*/
} 


/*---------------------------------------------------------------------------
 *            FTPCSTORE_GetObjectLen()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get the length of an FTPCSTORE item.
 *
 * Return:    The length of the object in bytes.
 *
 */
U32 FTPCSTORE_GetObjectLen(void *ftpcstoreObject)
{
	return ((FtpcStoreHandle)ftpcstoreObject)->fileLen;
} 

/*---------------------------------------------------------------------------
 *            FTPCSTORE_Write()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Write data to the given FTPCSTORE item.
 *
 * Return:    OBRC_SUCCESS -      Buffer was written successfully.
 *            OBRC_UNAUTHORIZED - Buffer was not written.
 *
 */
ObexRespCode FTPCSTORE_Write(void *ftpcstoreObject, U8 *buff, U16 len)
{
	FtpcStoreHandle	ftpcstoreObjecth = (FtpcStoreHandle)ftpcstoreObject;
	ObexRespCode	ret = OBRC_UNAUTHORIZED;
	BTHAL_U32		pNumWritten;
	BtFsStatus      btFsStatus;
	

 	Assert( ftpcstoreObject );
	
	if (ftpcstoreObjecth->object_location == BTL_OBJECT_LOCATION_FS)
	{
		/* Write folder listing to FS	*/

		btFsStatus = BTHAL_FS_Write(ftpcstoreObjecth->fp, buff, len, &pNumWritten);

		if (BT_STATUS_HAL_FS_SUCCESS == btFsStatus)
		{
			if (ftpcstoreObjecth->amount == 0) 
			{
				ftpcstoreObjecth->progressCallback(ftpcstoreObjecth);
			}				

			ftpcstoreObjecth->amount += len;
			BTL_LOG_INFO(("Total Writing to FS: \b%10lu ", ftpcstoreObjecth->amount));
			ret = OBRC_SUCCESS;
		} 
		else 
		{
			ret = BTL_OBEX_UTILS_ConvertFsErrorToObexError(btFsStatus);
		}
	}
	else
	{
		Assert(NULL != ftpcstoreObjecth->memAddr);
        
		/* Write folder listing to Memory */
		
		if((ftpcstoreObjecth->memSize - ftpcstoreObjecth->memIndex) < len)
		{
			ret = BTL_OBEX_UTILS_ConvertFsErrorToObexError(BT_STATUS_HAL_FS_ERROR_OUT_OF_SPACE);
		}
		else
		{
			OS_MemCopy((ftpcstoreObjecth->memAddr + ftpcstoreObjecth->memIndex), buff, len);
			if (ftpcstoreObjecth->memIndex == 0) 
			{
				ftpcstoreObjecth->progressCallback(ftpcstoreObjecth);
			}

			ftpcstoreObjecth->memIndex += len;
			BTL_LOG_INFO(("Total Writing to memory: \b%10lu ", ftpcstoreObjecth->memIndex));
			ret = OBRC_SUCCESS;
		}	
	}	
 
	ftpcstoreObjecth->progressCallback(ftpcstoreObjecth);
	return ret;
}


/*---------------------------------------------------------------------------
 *            FTPCSTORE_Read()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Read data from the given FTPCSTORE item.
 *
 * Return:    OBRC_SUCCESS -      Buffer was read successfully.
 *            OBRC_UNAUTHORIZED - Buffer was not read.
 *
 */
ObexRespCode FTPCSTORE_Read(void *ftpcstoreObject, U8* buff, U16 len)
{
	FtpcStoreHandle ftpcstoreObjecth = (FtpcStoreHandle)ftpcstoreObject;
	ObexRespCode ret = OBRC_UNAUTHORIZED;
	BTHAL_U32	pNumRead;
	BtFsStatus      btFsStatus;

	Assert( ftpcstoreObject );

	btFsStatus = BTHAL_FS_Read (ftpcstoreObjecth->fp, buff, len, &pNumRead);
    	
	if (BT_STATUS_HAL_FS_SUCCESS == btFsStatus)
	{
		if (ftpcstoreObjecth->amount == 0) 
		{
			ftpcstoreObjecth->progressCallback(ftpcstoreObjecth);
		}
		
		ftpcstoreObjecth->amount += len;
		BTL_LOG_INFO(("Total Reading from FS \b%10lu ", ftpcstoreObjecth->amount));
		ret = OBRC_SUCCESS;
	} 
	else 
	{
		ret = BTL_OBEX_UTILS_ConvertFsErrorToObexError(btFsStatus);
	}

	ftpcstoreObjecth->progressCallback(ftpcstoreObjecth);
	return ret;
}

/*---------------------------------------------------------------------------
 *            FTPCSTORE_Delete()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Delete the given FTPCSTORE item. Item can be in the 
 *            current OBEX directory or the Inbox. The FTPCSTORE
 *            handle is then returned to the pool.
 *
 * Return:    OBRC_SUCCESS -   File was deleted.
 *            OBRC_NOT_FOUND - File was not deleted.
 *
 */
ObexRespCode FTPCSTORE_Delete(FtpcStoreHandle *obPtr)
{
	ObexRespCode  ret = OBRC_NOT_FOUND;
	FtpcStoreHandle ftpcstoreObject = *obPtr;

	Assert (ftpcstoreObject);

	/* Write folder listing to FS	*/
	if (ftpcstoreObject->object_location == BTL_OBJECT_LOCATION_FS)
	{
		/* Close the file if is open */
		if (BTHAL_FS_INVALID_FILE_DESC  != ftpcstoreObject->fp) 
		{
			BTHAL_FS_Close(ftpcstoreObject->fp);
			ftpcstoreObject->fp = BTHAL_FS_INVALID_FILE_DESC;
			/* Print a newline */
			BTL_LOG_INFO(("\n"));
		}

		/* Delete file */
		if ( BTHAL_FS_Remove( ftpcstoreObject->tempFile) == BT_STATUS_HAL_FS_SUCCESS )
			ret = OBRC_SUCCESS;
	}
	else
	{
		/* Write folder listing to Mem */
		ftpcstoreObject->memIndex = 0;
		ret = OBRC_SUCCESS;
	}
	
	/* Free the object */
	InsertTailList(&ftpcstoreList, &(ftpcstoreObject->node));

	*obPtr = 0;
	return ret;
}

/*---------------------------------------------------------------------------
 *            FTPCSTORE_Close()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close the FTPCSTORE item and return the handle to the pool.
 *
 * Return:    OBRC_SUCCESS -   File was deleted.
 *            OBRC_NOT_FOUND - File was not deleted.
 *
 */
ObexRespCode FTPCSTORE_Close(FtpcStoreHandle *obPtr)
{
	ObexRespCode  rcode = OBRC_SUCCESS;
	FtpcStoreHandle ftpcstoreObject = *obPtr;

	/* Write folder listing to FS	*/
	if (ftpcstoreObject->object_location == BTL_OBJECT_LOCATION_FS)
	{
		if(ftpcstoreObject->fp != BTHAL_FS_INVALID_FILE_DESC)
		{
			if (BTHAL_FS_Close(ftpcstoreObject->fp) != BT_STATUS_HAL_FS_SUCCESS)
			{
				BTL_LOG_INFO(("FTPCSTORE: Unable to close file %s\n",	ftpcstoreObject->tempFile));
				rcode = OBRC_NOT_FOUND;
			}	
	
			ftpcstoreObject->fp = BTHAL_FS_INVALID_FILE_DESC; 
			
			/* The file is a temporary and a new name is set, rename it. */
			if ((ftpcstoreObject->tempFile[0] != 0) && (ftpcstoreObject->fullName[0] != 0)) 
			{
				/* Remove the (possibly) existing file before renaming it */
				BTHAL_FS_Remove(ftpcstoreObject->fullName);

				if (BTHAL_FS_Rename(ftpcstoreObject->tempFile, ftpcstoreObject->fullName) != BT_STATUS_HAL_FS_SUCCESS) 
				{
					BTL_LOG_INFO(("FTPCSTORE: Unable to rename file from %s to %s\n",
									ftpcstoreObject->tempFile, ftpcstoreObject->fullName));

					/* Remove the temp file */
					BTHAL_FS_Remove(ftpcstoreObject->tempFile);   
					rcode = OBRC_UNAUTHORIZED;
				}
				else
				{
					rcode = OBRC_SUCCESS;
				}	
			}
		}
	}
	else
	{
		/* Write folder listing to mem */
		ftpcstoreObject->memIndex = 0;
		rcode = OBRC_SUCCESS;
	}
		
	/* Return the FtpcStoreHandle to the pool */
	InsertTailList(&ftpcstoreList, &(ftpcstoreObject->node));
    
	*obPtr = 0;
	return rcode;
} 



/*---------------------------------------------------------------------------
 *           FtpcstoreFindFileName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  function searchs for file name from entier given path, searching for last '/' apearence
 *
 * Return:    string-file name, pointed by 'dest'
 *
 */
static void FtpcstoreFindFileName(const BtlUtf8 *source, BtlUtf8 *dest)
{
	int i, len;

	len = OS_StrLenUtf8(source);
	len--;
	for ( ;(source[len] != BTHAL_FS_PATH_DELIMITER) && (len >= 0);len--)
		;
	len++;
	for (i=0; source[len];i++, len++)
		dest[i] = source[len];
	dest[i] = '\0';
}

/*---------------------------------------------------------------------------
 *           FtpcstoreFindPathName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  function searchs for path name from entier given path, searching for last '/' apearence
 *
 * Return:    string-file name, pointed by 'dest'
 *
 */
static void FtpcstoreFindPathName(const BtlUtf8 *source, BtlUtf8 *dest)
{
	int i, len;

	len = OS_StrLenUtf8(source);
	len--;
	for ( ;(source[len] != BTHAL_FS_PATH_DELIMITER) && (len >= 0);len--)
		;
	len++;
	for (i=0; i< len;i++)
		dest[i] = source[i];
	dest[i] = '\0';
}

/*---------------------------------------------------------------------------
 *            FtpcstoreItoa
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Converts an unsigned 32-bit value to a base-10 number string.
 *
 * Return:    void.
 */
static U8*  FtpcstoreItoa( U32 val)
{
	static U8 convert[11];
	U8  rem,*out;
	U8  str[11], *dest = str;

	out = convert;

	if (val == 0)
	    *dest++ = '0';

	while (val) {
	    rem = (U8) (val%10);
	    *dest++ = (U8)(rem + '0');
	    val = val/10;
	}

	while (--dest >= str)
	    *out++ = *dest;

	*out = 0;
	return convert;
}



#endif  /*BTL_CONFIG_FTPC == BTL_CONFIG_ENABLED*/



