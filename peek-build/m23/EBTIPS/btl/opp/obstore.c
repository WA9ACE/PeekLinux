/****************************************************************************
 *
 * File:        obstore.c
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

#if BTL_CONFIG_OPP == BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "osapi.h"
#include "obstore.h"

#include "btl_defs.h"
#include "bthal_fs.h"
#include "btl_obex_utils.h"


BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_OPP);


/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

#define OBSTORE_CHECK_FS_PATH(object)                                                \
          (OS_StrLenUtf8(object->location.fsLocation.fsPath) <                       \
              (BTHAL_FS_MAX_PATH_LENGTH + BTHAL_FS_MAX_FILE_NAME_LENGTH))


/*******************************************************************************
 *
 * Macro definitions
 *
 ******************************************************************************/
#define OBSTORE_OPPC_TEMP_FILENAME       "oppc_pull.tmp"
#define OBSTORE_OPPS_TEMP_FILENAME       "opps_push.tmp"

#define OBSTORE_BUFF_LEN                 1024

#define DATA_RATE_TEST XA_DISABLED

#if DATA_RATE_TEST == XA_ENABLED
#define DATA_RATE_TEST_TX_SIZE 		5242880		/* 5MB */
#endif


 /********************************************************************************
 *
 * Internal Types
 *
 *******************************************************************************/


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/


/*---------------------------------------------------------------------------
 *
 * Object store entries
 */
static OppObStoreEntry obsEntries[OPP_OBS_MAX_NUM_ENTRIES];
static ListEntry    obsList; 
static U8           ObsInitCounter = 0;


/*---------------------------------------------------------------------------
 *
 * Common static array.
 */
static BtlUtf8 obFsPath[BTHAL_FS_MAX_PATH_LENGTH + BTHAL_FS_MAX_FILE_NAME_LENGTH] = {'\0'};
static BtlUtf8 obDestFsPath[BTHAL_FS_MAX_PATH_LENGTH + BTHAL_FS_MAX_FILE_NAME_LENGTH] = {'\0'};


/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
static void ObstoreMoveObject(const BtlUtf8 *objName, const BtlUtf8 *srcFsPath, const BtlUtf8 *destFsPath);
static void ObstoreSetDefaultName(OppObStoreHandle obs, const BtlUtf8* fsPath);
static BOOL ObstoreCreateDefaultDir(const BtlUtf8 *fsPath);
static void ObstoreFindPath(const BtlUtf8 *source, BtlUtf8 *dest);
static ObexRespCode ObstoreOpenFile(OppObStoreHandle obs,  U8 op);


/*-------------------------------------------------------------------------------
 * OBSTORE_Init()
 *
 *		Initialize the object store.
 */
BOOL OBSTORE_Init(void)
{
	U8 i;

	if (ObsInitCounter == 0)
	{
		if (OBS_DefaultObject.objectLocation == BTL_OBJECT_LOCATION_FS)
		{
		    /* Create default directory */
		    if (TRUE != ObstoreCreateDefaultDir(OBS_DefaultDir))
		    {
		        return FALSE;
		    }
		}
		else if (OBS_DefaultObject.objectLocation == BTL_OBJECT_LOCATION_MEM)
		{
			/* Do-nothing */
		}

		/* Initialize the object store entries */
		InitializeListHead(&obsList);

		for (i = 0; i < OPP_OBS_MAX_NUM_ENTRIES; i++) 
		{
		    InsertTailList(&obsList, &(obsEntries[i].node));
		}
	}

	ObsInitCounter++;

	return TRUE;
} 


/*-------------------------------------------------------------------------------
 * OBSTORE_Deinit()
 *
 *      Deinitialize the object store.
 */
void OBSTORE_Deinit(void)
{
	Assert(ObsInitCounter > 0);    

	ObsInitCounter--;
}


/*-------------------------------------------------------------------------------
 * OBSTORE_New()
 *
 *      Get a handle to a blank object store item.
 */
OppObStoreHandle OBSTORE_New(void)
{
	OppObStoreHandle obs = 0;

	if (!IsListEmpty(&obsList)) 
	{
		/* Get OppObStoreHandle from the pool */
		obs = (OppObStoreHandle)RemoveHeadList(&obsList);

		/* ======= common object fields ========== */
		obs->objectSize = 0;
		obs->objectName[0] = 0;
		obs->object_location = BTL_OBJECT_LOCATION_FS;	

		obs->amount = 0;
		obs->nameLen = 0;
		obs->typeLen = 0;
		obs->progressCallback = NULL;
		obs->operation = OPP_UNDEF_OPERATION;
	    

		/* ======== fs object fields ============= */   
		obs->fp = BTHAL_FS_INVALID_FILE_DESC;
		obs->sysTempFsPath[0] = 0;   
		obs->type[0] = 0;
		obs->fsPath[0] = 0;


		/* ========= memory object fields ======== */
		obs->mem_ocx = NULL;
	} 
	else 
	{
		BTL_LOG_ERROR(("Out of entries\n"));
	}

	return obs;
} 


/*-------------------------------------------------------------------------------
 * OBSTORE_Create()
 *
 *      Open the specified object store item for writing.
 */
ObexRespCode OBSTORE_Create(OppObStoreHandle obs, const U8 op)
{
	BtFsStatus status;
	U16 len;

	Assert(0 != obs);

	if (obs->object_location == BTL_OBJECT_LOCATION_MEM)
	{
		obs->operation = op;
		return OBRC_SUCCESS;
	}

	/* We open a temp file to avoid accidentally clobbering an existing
	 * file that already exists (if the operation doesn't complete 
	 * successfully). We maintain a pointer to the real filename, which
	 * is used when the file is closed.
	 */

	/* Create temp file name according to context */

	if (op == OPPS_PUSH_OPERATION)
	{
	    ObstoreFindPath(obs->fsPath, obs->sysTempFsPath);
	    len = OS_StrLenUtf8(obs->sysTempFsPath);		
	    obs->sysTempFsPath[len] = BTHAL_FS_PATH_DELIMITER;
	    obs->sysTempFsPath[++len] = '\0';
	    OS_StrCatUtf8(obs->sysTempFsPath, OBSTORE_OPPS_TEMP_FILENAME);
	}
	else if (op == OPPC_PULL_OPERATION)
	{
	    ObstoreFindPath(obs->fsPath, obs->sysTempFsPath);
	    len = OS_StrLenUtf8(obs->sysTempFsPath);		
	    obs->sysTempFsPath[len] = BTHAL_FS_PATH_DELIMITER;
	    obs->sysTempFsPath[++len] = '\0';
	    OS_StrCatUtf8(obs->sysTempFsPath, OBSTORE_OPPC_TEMP_FILENAME);
	}
	    
	status = BTHAL_FS_Open((const BTHAL_U8 *)obs->sysTempFsPath, 
	                    BTHAL_FS_O_CREATE | BTHAL_FS_O_RDWR |BTHAL_FS_O_BINARY, /* Create file */
	                        (BthalFsFileDesc *)&(obs->fp));

	if (status != BT_STATUS_HAL_FS_SUCCESS) 
	{
		if (0 != obs->sysTempFsPath[0])
		{
			BTL_LOG_ERROR(("Error creating %s\n", obs->sysTempFsPath));
		}
	    
		return OBRC_NOT_FOUND;
	}

	obs->operation = op;
		
	return OBRC_SUCCESS;
}


/*-------------------------------------------------------------------------------
 * OBSTORE_Open()
 *
 *      This function is used by OPP client and server.
 *       
 *      Open an object store item for reading. If no name has been
 *      set then open the default object.
 */
ObexRespCode OBSTORE_Open(OppObStoreHandle obs, const U8 op)
{
	ObexRespCode rcode;

	Assert(0 != obs);

	if (obs->object_location == BTL_OBJECT_LOCATION_FS)
	{
	    if (OPPS_PULL_OPERATION == op)	
	    {    
			/* Server should open the default vCard */

			/* Fill obs->fsPath field */
			ObstoreSetDefaultName(obs, OBS_DefaultObject.location.fsLocation.fsPath);
		}
	}
	else
	{
		obs->operation = op;
	    
		obs->mem_ocx = (void*)OBS_DefaultObject.location.memLocation.memAddr;
		obs->objectSize = OBS_DefaultObject.location.memLocation.size;

		return OBRC_SUCCESS;
	}

	rcode = ObstoreOpenFile(obs, op);

	return rcode;
} 


/*-------------------------------------------------------------------------------
 * OBSTORE_AppendNameAscii()
 *
 *      Append the ASCII string to the name & update name length.
 */
void OBSTORE_AppendNameAscii(OppObStoreHandle obs, const BtlUtf8* fsPath)
{
	ObstoreSetDefaultName(obs, fsPath);
} 


/*-------------------------------------------------------------------------------
 * OBSTORE_AppendType()
 *
 *      Append the ASCII string to the object type.
 */
void OBSTORE_AppendType(OppObStoreHandle obs, const U8 *type, U16 len)
{
	BTHAL_U32 copyLen;
	U32 i;

	if ((obs == 0) || (len == 0)) 
	{
		return;
	}

	/* Append the ASCII string to the existing ASCII name. */
	copyLen = min((OBS_MAX_TYPE_LEN - 1) - obs->typeLen, len);
	OS_MemCopy((obs->type + obs->typeLen), type, copyLen);

	i = obs->typeLen + copyLen;

	/* Put a 0 at the end of the type if one does not exist */
	if (obs->type[i-1] != 0) 
	{
		obs->type[i] = 0;
		i++;
	}

	/* Update typeLen. It does not include the 0 */
	obs->typeLen = (U16)(i - 1);
}


/*-------------------------------------------------------------------------------
 * OBSTORE_GetObjectLen()
 *
 *      Get the length of an object store item.
 */
U32 OBSTORE_GetObjectLen(void *obs)
{
	return ((OppObStoreHandle)obs)->objectSize;
} 


/*-------------------------------------------------------------------------------
 * OBSTORE_Write()
 *
 *      Write data to the object store entry.
 */
ObexRespCode OBSTORE_Write(void *obs, U8 *buff, U16 len)
{
	OppObStoreHandle obsh = (OppObStoreHandle)obs;
	ObexRespCode ret = OBRC_UNAUTHORIZED;
	BTHAL_U32 pNumWritten;
	BtFsStatus btFsStatus;

	Assert(0 != obsh); 
		
	if (obsh->object_location == BTL_OBJECT_LOCATION_FS)
	{
	    Assert(BTHAL_FS_INVALID_FILE_DESC != obsh->fp);

#if DATA_RATE_TEST == XA_ENABLED
		pNumWritten = len;
		if (obsh->amount == 0) 
		{
			obsh->progressCallback(obsh);
			BTL_LOG_INFO(("DATA_RATE_TEST: RX Start."));
		}	
		obsh->amount += len;

		if (obsh->amount == obsh->objectSize)
			BTL_LOG_INFO(("DATA_RATE_TEST: RX End - %d bytes.", obsh->objectSize));

		ret = OBRC_SUCCESS;
#else
	    btFsStatus = BTHAL_FS_Write(obsh->fp, buff, len, &pNumWritten);
	    if (BT_STATUS_HAL_FS_SUCCESS == btFsStatus)
		{
			if (obsh->amount == 0) 
			{
				obsh->progressCallback(obsh);

				BTL_LOG_INFO(("Writing %10lu ",obsh->amount));
			}
			
			obsh->amount += len;
			BTL_LOG_INFO(("Total written %10lu ", obsh->amount));
			ret = OBRC_SUCCESS;
		}
		else 
		{
			ret = BTL_OBEX_UTILS_ConvertFsErrorToObexError(btFsStatus);
		}
#endif
		
	}
	else 
	{
		Assert(NULL != obsh->mem_ocx);
		Assert(0 != obsh->objectSize);

		if((obsh->objectSize - obsh->amount) < len)
		{
			BTL_LOG_ERROR(("Error writing to Memory Object. Not enough memory.\n"));
			ret = OBRC_UNAUTHORIZED;
		}
		else
		{
			OS_MemCopy(((const char *)obsh->mem_ocx + obsh->amount), buff, len);
			if (obsh->amount == 0) 
			{
				obsh->progressCallback(obsh);
				BTL_LOG_INFO(("Writing %10lu ",obsh->amount));
			}
			
			obsh->amount += len;
			BTL_LOG_INFO(("Total written %10lu ", obsh->amount));
			ret = OBRC_SUCCESS;
		}
	}

	obsh->progressCallback(obsh);
	return ret;
} 


/*-------------------------------------------------------------------------------
 * OBSTORE_Read()
 *
 *      Read data from the object store entry.
 */
ObexRespCode OBSTORE_Read(void *obs, U8* buff, U16 len)
{
	OppObStoreHandle obsh = (OppObStoreHandle)obs;
	ObexRespCode ret = OBRC_UNAUTHORIZED;
	BTHAL_U32 pNumRead;

	Assert(0 != obsh); 

	if (obsh->object_location == BTL_OBJECT_LOCATION_FS)
	{
		Assert(BTHAL_FS_INVALID_FILE_DESC != obsh->fp);

#if DATA_RATE_TEST == XA_ENABLED
		pNumRead = len;
		if (obsh->amount == 0) 
		{
			obsh->progressCallback(obsh);
			BTL_LOG_INFO(("DATA_RATE_TEST: TX Start."));
		}	
		obsh->amount += len;
		
		if (obsh->amount == DATA_RATE_TEST_TX_SIZE)
			BTL_LOG_INFO(("DATA_RATE_TEST: TX End - %d bytes.", DATA_RATE_TEST_TX_SIZE));
		
		ret = OBRC_SUCCESS;
#else
		if (BTHAL_FS_Read(obsh->fp, buff, len, &pNumRead) == BT_STATUS_HAL_FS_SUCCESS)
		{
			if (obsh->amount == 0) 
			{
				obsh->progressCallback(obsh);
				BTL_LOG_INFO(("Reading %10lu ",obsh->amount));
			}			
	        obsh->amount += len;
			BTL_LOG_INFO(("Total read %10lu ", obsh->amount));
			ret = OBRC_SUCCESS;
		} 
		else 
		{
			BTL_LOG_ERROR(("Error reading from file! len %d\n", len));
		}
#endif
		
	}
	else 
	{
		Assert(NULL != obsh->mem_ocx);
		
		OS_MemCopy(buff, (const BTHAL_U8 *)obsh->mem_ocx + obsh->amount, len);
		if (obsh->amount == 0) 
		{
			obsh->progressCallback(obsh);
			BTL_LOG_INFO(("Writing %10lu ",obsh->amount));
		}
		
		obsh->amount += len;
		BTL_LOG_INFO(("Total written %10lu ", obsh->amount));
		ret = OBRC_SUCCESS;
	}

	obsh->progressCallback(obsh);
	return ret;
}


/*-------------------------------------------------------------------------------
 * OBSTORE_Delete()
 *
 *      Delete the object and free the storage.
 */
ObexRespCode OBSTORE_Delete(OppObStoreHandle *obsPtr)
{
	ObexRespCode  ret = OBRC_NOT_FOUND;
	OppObStoreHandle obs = *obsPtr;

	Assert(0 != obs);

	if (obs->object_location == BTL_OBJECT_LOCATION_FS)
	{
	    /* Close the file if is open */
	    if (obs->fp != BTHAL_FS_INVALID_FILE_DESC) 
	    {
		    BTHAL_FS_Close(obs->fp);

		    obs->fp = BTHAL_FS_INVALID_FILE_DESC;
		    
		    BTL_LOG_INFO(("Delete object."));
	    }

	    if (BT_STATUS_HAL_FS_SUCCESS == BTHAL_FS_Remove((const BTHAL_U8 *)obs->sysTempFsPath))
	    {
		    ret = OBRC_SUCCESS;
	    }
	}

	/* Free the object */
	InsertTailList(&obsList, &(obs->node));

	*obsPtr = 0;
	return ret;
}


/*-------------------------------------------------------------------------------
 * OBSTORE_Close()
 *
 *      Close an object store item.
 */
ObexRespCode OBSTORE_Close(OppObStoreHandle *obsPtr)
{
	ObexRespCode  rcode = OBRC_SUCCESS;
	OppObStoreHandle obs = *obsPtr;

	Assert(0 != obs);

	if (obs->object_location == BTL_OBJECT_LOCATION_FS)
	{
		if (obs->fp != BTHAL_FS_INVALID_FILE_DESC) 
		{
			BTHAL_FS_Close(obs->fp);
			obs->fp = BTHAL_FS_INVALID_FILE_DESC;

			BTL_LOG_INFO(("Object Closed"));

			if (obs->operation == OPPS_PUSH_OPERATION)
			{
				/* By default, OPPS saves pushed objects in its default directory */
				/* The user has requested to save pushed object with a given folder,
					and a given name. */
            
				/* The user has provided a new storage name */
				obs->objectName[0] = '\0';
				ObstoreMoveObject(obs->objectName, obs->sysTempFsPath, obs->fsPath);
			}
			else if (obs->operation == OPPC_PULL_OPERATION)
			{
				/* By default, OPPS saves pulled object by its default name. */
				/* The pulled object is renamed to the name received from remote server. */
                
				ObstoreMoveObject(obs->objectName, obs->sysTempFsPath, obs->fsPath);
			}
		}
	}

	/* Return the OppObStoreHandle to the pool */
	InsertTailList(&obsList, &(obs->node));
    
	*obsPtr = 0;
	return rcode;
} 


/*-------------------------------------------------------------------------------
 * OBSTORE_SetNameDefault()
 *
 *      Set the name of the Object store entry to the default name
 *      for a received object that has no name.
 */
void OBSTORE_SetNameDefault(OppObStoreHandle obs, const BtlUtf8* dir)
{
	UNUSED_PARAMETER(obs);
	UNUSED_PARAMETER(dir);
} 

/*-------------------------------------------------------------------------------
 * OBSTORE_SetFileName()
 *
 *      In OPP server, this is the object name of the received object.  
 *      In OPP client, this is the object name of the transmitted object. 
 */
void OBSTORE_SetFileName(OppObStoreHandle obs, const BtlUtf8 *name)
{
	OS_StrCpyUtf8(obs->objectName, name);
}


/*-------------------------------------------------------------------------------
 * OBSTORE_FindPath()
 *
 *      Search for the Path without file name from full path
 */
void OBSTORE_FindPath(const BtlUtf8 *source, BtlUtf8 *dest)
{

	ObstoreFindPath(source, dest);
}


/*-------------------------------------------------------------------------------
 * OBSTORE_SetInboxDir()
 *
 *      Change Inbox Directory by given string
 */
BOOL OBSTORE_SetInboxDir(const BtlUtf8* fsPath)
{
	U16 len;

	OBS_DefaultDir[0] = '\0';

	ObstoreFindPath(fsPath, OBS_DefaultDir);

	len = OS_StrLenUtf8(OBS_DefaultDir);

	if ((len == 0) || (len > BTHAL_FS_MAX_PATH_LENGTH))
	{
		return FALSE;
	}

	return TRUE;
}


/*---------------------------------------------------------------------------
 * OBSTORE_CopyTempFsToMem()
 *
 * We pushed received data to FS object and now user requests Memory object, 
 * therefore we need to move all data from temp FS to Memory object.
 *
 */
ObexRespCode OBSTORE_CopyTempFsToMem(OppObStoreHandle obs)
{
	ObexRespCode ret = OBRC_UNAUTHORIZED;
	BTHAL_U32 pNumRead;
	BtFsStatus	status;

	Assert(0 != obs);

	Assert(BTHAL_FS_INVALID_FILE_DESC != obs->fp);

	/* obs->amount tells how many bytes already written to FS */
	if (obs->amount > obs->objectSize)	
	{
		BTL_LOG_ERROR(("objectSize is too large\n"));
		ret = OBRC_REQ_ENTITY_TOO_LARGE;
	}
	else 
	{	
	    /* close and reopen file before read (windows platform workaround) */
		BTHAL_FS_Close(obs->fp);
		
	    /* Mark obs->fp as invalid */
	    obs->fp = BTHAL_FS_INVALID_FILE_DESC;

	    status = BTHAL_FS_Open((const BTHAL_U8 *)obs->sysTempFsPath, 
		            BTHAL_FS_O_RDONLY | BTHAL_FS_O_BINARY,   /* Read Only */
	                        (BthalFsFileDesc*)&(obs->fp));

		if (BT_STATUS_HAL_FS_SUCCESS == status)
		{
			/* Read the initial bytes from the temp file */
			if (BT_STATUS_HAL_FS_SUCCESS == BTHAL_FS_Read(obs->fp, obs->mem_ocx, obs->amount, &pNumRead))
			{
				BTL_LOG_INFO(("read %10lu ", obs->amount));

				BTHAL_FS_Close(obs->fp);
				obs->fp = BTHAL_FS_INVALID_FILE_DESC;
				
				BTHAL_FS_Remove((const BTHAL_U8 *)obs->sysTempFsPath);
	            
				obs->object_location = BTL_OBJECT_LOCATION_MEM;	
				ret = OBRC_SUCCESS;
			}
			else 
			{
				BTL_LOG_ERROR(("Error reading from file! len %d\n", obs->amount));
				ret = OBRC_REQ_ENTITY_TOO_LARGE;
			}
		}
	}

	return ret;
}

/*---------------------------------------------------------------------------
 * OBSTORE_PushFsSaveUserSettings()
 *
 */
ObexRespCode OBSTORE_PushFsSaveUserSettings(OppObStoreHandle obs, const BtlObject *object)
{
	BthalFsStat fileStat;

	obs->fsPath[0] = '\0';

	if (NULL != object->location.fsLocation.fsPath)
	{
		if (TRUE != OBSTORE_CHECK_FS_PATH(object))
		{
			BTL_LOG_ERROR(("fsPath %s is too long\n", object->location.fsLocation.fsPath));
			return OBRC_FORBIDDEN;
		}
	}
	else
	{
		BTL_LOG_ERROR(("fsPath for push object is not defined\n"));
		return OBRC_NOT_FOUND;
	}

	/* Extract the path */
	ObstoreFindPath(object->location.fsLocation.fsPath, obFsPath);

	/* Check that the user defined folder exists */
	if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Stat((const BTHAL_U8 *)obFsPath, &fileStat))
	{
		return OBRC_NOT_FOUND;
	}

	/* Check that the user defined pushed file is not Read only */
	if (BT_STATUS_HAL_FS_SUCCESS == BTHAL_FS_Stat((const BTHAL_U8 *)object->location.fsLocation.fsPath, &fileStat))
	{
		if (TRUE == fileStat.isReadOnly)
		{
			/* Since file is Read only, it cannot be deleted once push operation is completed. */
			BTL_LOG_ERROR(("Pushed file %s is read only\n", obFsPath));
			return OBRC_FORBIDDEN;
		}
	}

    /* Saved in obs->fsPath used in OBSTORE_Close() */
	OS_StrCpyUtf8(obs->fsPath, object->location.fsLocation.fsPath);

	return OBRC_SUCCESS;
}


/*---------------------------------------------------------------------------
 * OBSTORE_ExtractObjName()
 *
 * Extract object name from fsPath.
 *
 */
void OBSTORE_ExtractObjName(const BtlUtf8 *source, BtlUtf8 *dest)
{
	int i, len;

	len = OS_StrLenUtf8(source);
	len--;
	for ( ;(source[len] != BTHAL_FS_PATH_DELIMITER) && (len >= 0); len--)
	{
	    /* do-nothing */
	}
		
	len++;
	for (i=0; source[len];i++, len++)
	{
	    dest[i] = source[len];
	}

	dest[i] = '\0';	
}

/*-------------------------------------------------------------------------------
 * OBSTORE_CloseAndReopen()
 *
 * Closes obsPtr->fp and reopen object->location.fsLocation.fsPath
 */
ObexRespCode OBSTORE_FileCloseAndReopen(OppObStoreHandle *obsPtr, const BtlObject *object)
{
	ObexRespCode  rcode = OBRC_NOT_FOUND;
	OppObStoreHandle obs = *obsPtr;

	Assert(0 != obs);

	if (obs->operation == OPPS_PULL_OPERATION)
	{
		if (obs->object_location == BTL_OBJECT_LOCATION_FS)
		{
			if (obs->fp != BTHAL_FS_INVALID_FILE_DESC) 
			{
				BTHAL_FS_Close(obs->fp);
				obs->fp = BTHAL_FS_INVALID_FILE_DESC;

				BTL_LOG_INFO(("Object Closed"));

				/* Fill the obs->fsPath field */
				OS_StrCpyUtf8(obs->fsPath, object->location.fsLocation.fsPath);	
				    
				rcode = ObstoreOpenFile(obs, OPPS_PULL_OPERATION);
			}
		}
	}

	return rcode;
} 

static void ObstoreSetDefaultName(OppObStoreHandle obs, const BtlUtf8* fsPath)
{
	/* Fill the nameLen field */
	obs->nameLen = OS_StrLenUtf8(obs->objectName);

	/* Fill the obs->fsPath field */
	OS_StrCpyUtf8(obs->fsPath, fsPath);	
} 

static void ObstoreMoveObject(const BtlUtf8 *objName, const BtlUtf8 *srcFsPath, const BtlUtf8 *destFsPath)
{
	static char buff[OBSTORE_BUFF_LEN];

	BthalFsFileDesc fpSrc, fpDest;
	BTHAL_U32 pNumWritten;
	BTHAL_U32 pNumRead;
	BTHAL_U32 nSize;
	BthalFsStat fileStat;
	U16 len;
	U32 objectSize;
	BtFsStatus status;

	if (!destFsPath)
	{
		return;
	}

	if (0 != OS_StrCmpUtf8(srcFsPath, destFsPath))
	{
		/* Check if the two files are on the same directory */
		ObstoreFindPath(srcFsPath, obFsPath);
		ObstoreFindPath(destFsPath, obDestFsPath);

		if (0 == OS_StrCmpUtf8(obFsPath, obDestFsPath))
		{
			if (0 != objName[0])
			{
				len = OS_StrLenUtf8(obDestFsPath);
				obDestFsPath[len] = BTHAL_FS_PATH_DELIMITER;
				obDestFsPath[++len] = '\0';
				OS_StrCatUtf8(obDestFsPath, objName);

				BTHAL_FS_Remove((const BTHAL_U8 *)obDestFsPath);
				status = BTHAL_FS_Rename((const BTHAL_U8 *)srcFsPath, (const BTHAL_U8 *)obDestFsPath);
			}
			else
			{
				BTHAL_FS_Remove((const BTHAL_U8 *)destFsPath);
				status = BTHAL_FS_Rename((const BTHAL_U8 *)srcFsPath, (const BTHAL_U8 *)destFsPath);
			}
		    
			if (BT_STATUS_HAL_FS_SUCCESS != status) 
			{
				BTL_LOG_ERROR(("Unable to rename file from %s to %s\n", obFsPath, obDestFsPath));
			}
			else
			{
				if (0 != objName[0])
				{
					BTL_LOG_INFO(("File is successfully renamed from  %s to %s\n ", srcFsPath, obDestFsPath));
				}
				else
				{
					BTL_LOG_INFO(("File is successfully renamed from  %s to %s\n ", srcFsPath, destFsPath));
				}
			}	
			return;
		}
		else
		{
		    /* Copy temporary file to its new destination. */
		    
		    if (0 != objName[0])
		    {
		        len = OS_StrLenUtf8(obDestFsPath);
		        obDestFsPath[len] = BTHAL_FS_PATH_DELIMITER;
		        obDestFsPath[++len] = '\0';
		        OS_StrCatUtf8(obDestFsPath, objName);
		    }
		    else
		    {
		        obDestFsPath[0] = '\0';
		        OS_StrCpyUtf8(obDestFsPath, destFsPath);
		    }

		    /* First, remove destination file if exists */
		    BTHAL_FS_Remove((const BTHAL_U8 *)obDestFsPath);

		    status = BTHAL_FS_Open((const BTHAL_U8 *)srcFsPath, 
		            BTHAL_FS_O_RDONLY | BTHAL_FS_O_BINARY,                      /* Read Only */
		                    (BthalFsFileDesc *)&fpSrc);
		    
		    if (BT_STATUS_HAL_FS_SUCCESS != status) 
		    {
				BTL_LOG_ERROR(("Failed to move file from  %s to %s\n ", srcFsPath, destFsPath));
				BTL_LOG_ERROR(("Failed to open file %s\n", srcFsPath));
				return;
		    }

		    status = BTHAL_FS_Open((const BTHAL_U8 *)obDestFsPath, 
		                BTHAL_FS_O_CREATE | BTHAL_FS_O_RDWR |BTHAL_FS_O_BINARY, /* Create file */
		                    (BthalFsFileDesc *)&fpDest);

		    if (BT_STATUS_HAL_FS_SUCCESS != status) 
		    {
				BTL_LOG_ERROR(("Failed to create file %s\n", obDestFsPath));
				goto error_exit;
		    }

		    status = BTHAL_FS_Stat((const BTHAL_U8 *)srcFsPath, &fileStat); 
		    if(status != BT_STATUS_HAL_FS_SUCCESS)
		    {
		        BTL_LOG_ERROR(("File size could not be retrieved."));
		        goto error_exit;
		    }

		    objectSize = fileStat.size;

		    while (objectSize > 0)
		    {
		        if (objectSize > OBSTORE_BUFF_LEN)
		        {
		            nSize = OBSTORE_BUFF_LEN;
		        }
		        else
		        {
		            nSize = objectSize;
		        }
		        
		        if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Read(fpSrc, buff, nSize, &pNumRead))
		        {
		            goto error_exit;
		        }
		            
		        if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Write(fpDest, buff, pNumRead, &pNumWritten))
		        {
		            goto error_exit;
		        }

		        if (pNumRead != pNumWritten)
		        {
		            goto error_exit;
		        }

		        objectSize -= pNumWritten;
		    }
		}    

		BTL_LOG_INFO(("File is successfully moved from  %s to %s\n ", srcFsPath, destFsPath));

		BTHAL_FS_Close(fpSrc);
		BTHAL_FS_Close(fpDest);    
		BTHAL_FS_Remove((const BTHAL_U8 *)srcFsPath);
		return;
	}
	else
	{
		/* Do-nothing */
		return;
	}
    
error_exit:
    BTL_LOG_INFO(("Failed to move file from  %s to %s\n ", srcFsPath, obDestFsPath)); 

    BTHAL_FS_Remove((const BTHAL_U8 *)destFsPath);
    BTHAL_FS_Close(fpSrc);
    BTHAL_FS_Close(fpDest);
}


static BOOL ObstoreCreateDefaultDir(const BtlUtf8 *fsPath)
{
	BtFsStatus status;
	BthalFsStat fileStat;

	if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Stat((const BTHAL_U8 *)fsPath, &fileStat))
	{
	    /* Create default directory */
	    
	    ObstoreFindPath(fsPath, obFsPath);

	    /* First, create the root directory */
	    status = BTHAL_FS_Mkdir((const BTHAL_U8 *)obFsPath);
	    if ((status != BT_STATUS_HAL_FS_ERROR_EXISTS) && (status != BT_STATUS_HAL_FS_SUCCESS)) 
	    {
		    BTL_LOG_ERROR(("Error: Unable to create root dir %s\n", obFsPath)); 
	        return FALSE;
	    }

	    /* Second, create the sub-directory */
	    status = BTHAL_FS_Mkdir((const BTHAL_U8 *)fsPath);
	    if ((status != BT_STATUS_HAL_FS_ERROR_EXISTS) && (status != BT_STATUS_HAL_FS_SUCCESS)) 
	    {
		    BTL_LOG_ERROR(("Error: Unable to create sub dir %s\n", fsPath)); 
	        return FALSE;
	    }
	}

	return TRUE;
}


static void ObstoreFindPath(const BtlUtf8 *source, BtlUtf8 *dest)
{
	int len;

	len = OS_StrLenUtf8(source);
	len--;

	for ( ;(source[len] != BTHAL_FS_PATH_DELIMITER) && (len >= 0);len--)
	{
	    /* do-nothing */
	}
		
	OS_StrnCpyUtf8(dest, source, len);
	*(dest+len) = '\0';
}


static ObexRespCode ObstoreOpenFile(OppObStoreHandle obs, U8 op)
{
	BthalFsStat FileStat;
	BtFsStatus status;

	/* Check if file exists */
	status = BTHAL_FS_Stat((const BTHAL_U8 *)obs->fsPath, &FileStat); 
	if(status != BT_STATUS_HAL_FS_SUCCESS)
	{
		BTL_LOG_ERROR(("File was not found"));
		return OBRC_NOT_FOUND;
	}

	/* Fill obs->objectSize field */
#if DATA_RATE_TEST == XA_ENABLED
	obs->objectSize = DATA_RATE_TEST_TX_SIZE;
#else
	obs->objectSize = FileStat.size;
#endif
		/* Mark obs->fp as invalid */
	obs->fp = BTHAL_FS_INVALID_FILE_DESC;

	status = BTHAL_FS_Open((const BTHAL_U8 *)obs->fsPath, 
		            BTHAL_FS_O_RDONLY | BTHAL_FS_O_BINARY,      /* Read Only */
	                        (BthalFsFileDesc *)&(obs->fp));

	if (status != BT_STATUS_HAL_FS_SUCCESS) 
	{
		BTL_LOG_ERROR(("File was not found"));
		return OBRC_NOT_FOUND;
	} 

	BTL_LOG_INFO(("Object Opened"));

	obs->operation = op;

	return OBRC_SUCCESS;
}

#endif /*#if BTL_CONFIG_OPP == BTL_CONFIG_ENABLED*/
