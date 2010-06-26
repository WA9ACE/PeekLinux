#ifndef __FTPCSTOREH
#define __FTPCSTOREH

/****************************************************************************
 *
 * File:          ftpcstore.h
 *
 * Description:   This file specifies the OBEX object store interface.
 * 
 * Created:       May 15, 1997
 *
 * Version:       MTObex 3.4
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

#include <stdio.h>
#include <obexerr.h>
#include <utils.h>
#include <obex.h>
#include <goep.h>
#include <bthal_fs.h>
#include "btl_unicode.h"
#include "btl_common.h"

/****************************************************************************
 *
 * These structure definitions, functions and constants are used by the 
 * OBEX Protocol and must be provided by all Object Store implementations.
 *
 ****************************************************************************/
 
/****************************************************************************
 *
 * Structure and Constants Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * FtpcStoreHandle Structure
 */
typedef struct _FtpcStoreEntry *FtpcStoreHandle;


/* Forward declarations */
typedef struct _FtpcStoreEntry 	FtpcStoreEntry;	

/* Indicates a progress change */
typedef void (*FtpcstoreProgressCallBack)(FtpcStoreEntry *obs);



/****************************************************************************
 *
 * Function Reference 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Prototype:     void FTPCSTORE_AppendNameAscii(FtpObStoreHandle obs, 
 *                                             const BtlUtf8    *name, 
 *                                             U16           len)
 *
 * Description:   Append the ASCII string to the name. 
 *
 * Parameters:    obs - handle of object store entry.
 *
 *                name - pointer to ASCII string to append
 *
 *                len - length in bytes of string to append
 *
 * Returns:       void 
 */
void FTPCSTORE_AppendNameAscii(FtpcStoreHandle obs, const BtlUtf8* fullName);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode FTPCSTORE_Close(FtpObStoreHandle *obs)
 *
 * Description:   Close an object store item.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       OBRC_SUCCESS - Operation was successful.
 *                OBRC_UNAUTHORIZED - Could not create/rename object store item.
 *                OBRC_NOT_FOUND - Could not access object store item.
 */
ObexRespCode FTPCSTORE_Close(FtpcStoreHandle *obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode FTPCSTORE_Create(FtpObStoreHandle *obs, BOOL useInbox)
 *
 * Description:   Open the specified object store item for FS writing. 
 *
 * Parameters:    useInbox: TRUE  - Create the object from within the inbox.
 *                          FALSE - Create object in initial directory.
 *
 *                obs - Handle of object store item to create.
 *
 * Returns:       OBRC_SUCCESS - Operation was successful.
 *                OBRC_NOT_FOUND - Could not create object store item.
 */

ObexRespCode FTPCSTORE_Create(FtpcStoreHandle obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode FTPCSTORE_CreateMem(FtpObStoreHandle *obs, BOOL useInbox)
 *
 * Description:   Open the specified object store item for memory writing. 
 *
 * Parameters:    useInbox: TRUE  - Create the object from within the inbox.
 *                          FALSE - Create object in initial directory.
 *
 *                obs - Handle of object store item to create.
 *
 * Returns:       OBRC_SUCCESS - Operation was successful.
 *                OBRC_NOT_FOUND - Could not create object store item.
 */
ObexRespCode FTPCSTORE_CreateMem(FtpcStoreHandle ftpcstoreObject,const char	*memAddr,U32 size);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_Deinit()
 *
 * Description:   Deinitialize the object store.
 *
 * Parameters:    none 
 *
 * Returns:       void
 */
void FTPCSTORE_Deinit(void);
/*	#define OBSTORE_Deinit() */

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode FTPCSTORE_Delete(FtpObStoreHandle *obs)
 *
 * Description:   Delete the object and free the storage.
 *
 * Parameters:    obs - handle to object store entry to delete. 
 *
 * Returns:       OBRC_NOT_FOUND - File could not be deleted.
 *                OBRC_SUCCESS   - File was deleted.
 */
ObexRespCode FTPCSTORE_Delete(FtpcStoreHandle *obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     BtlUtf8* FTPCSTORE_GetNameAscii(FtpObStoreHandle obs)
 *
 * Description:   Return a pointer to the Object Store item's name in Ascii.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       Pointer to NULL terminated ASCII name.
 */
#define FTPCSTORE_GetNameAscii(obs) ((obs)->name)

/*---------------------------------------------------------------------------
 *
 * Prototype:     BtlUtf8* FTPCSTORE_GetTypeAscii(FtpObStoreHandle obs)
 *
 * Description:   Return a pointer to the Object Store item's type in Ascii.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       Pointer to NULL terminated ASCII type.
 */
/*	#define FTPCSTORE_GetTypeAscii(obs) ((obs)->type)	*/

/*---------------------------------------------------------------------------
 *
 * Prototype:     U32 FTPCSTORE_GetObjectLen(void *obs)
 *
 * Description:   Get the length of an object store item.
 *                A function that performs this behavior is REQUIRED 
 *                by the OBEX Protocol.  This function fills in the 
 *                OBS_Read field of the ObStoreFuncTable (obex.h)
 *
 * Parameters:    obs - pointer to the handle of object store entry.
 *                      Casting from void* to the object store entry
 *                      must be done inside this function.
 *
 * Returns:       The object length
 */
U32 FTPCSTORE_GetObjectLen(void *obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL FTPCSTORE_Init()
 *
 * Description:   Initialize the object store.
 *
 * Parameters:    none 
 *
 * Returns:       TRUE - object store was initialized successfully. 
 *                FALSE - Unable to create inbox.
 */
BOOL FTPCSTORE_Init(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     FtpObStoreHandle FTPCSTORE_New()
 *
 * Description:   Get a handle to a blank object store item.
 *
 * Parameters:    none 
 *
 * Returns:       Handle of an object store entity. 0 means out of objects. 
 */
FtpcStoreHandle FTPCSTORE_New(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode FTPCSTORE_Open(FtpObStoreHandle obs, BOOL useInbox)
 *
 * Description:   Open an object store item for reading. If no name has been
 *                set then open the default object.
 *
 * Parameters:    obs - pointer to object store entry
 *
 *                useInbox: TRUE open the object from within the inbox.
 *                          FALSE open object in current directory.
 *
 * Returns:       OBRC_SUCCESS indicates that the open is successful. 
 *                OBRC_NOT_FOUND indicates failure. 
 */
ObexRespCode FTPCSTORE_Open(FtpcStoreHandle obs);

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode FTPCSTORE_SetOffset(void *obs, 
 *                                               U32  offset)
 *
 * Description:   Sets the offset into the current object store entry.  This
 *                is used to move the value of the file pointer backwards in 
 *                the case that data needs to be retransmitted.
 *
 * Parameters:    obs - pointer to the object store entry
 *                offset - offset into the current object
 *
 * Returns:       OBRC_SUCCESS indicates that the save is successful. 
 *                OBRC_NOT_FOUND indicates failure to save. 
 */
ObexRespCode FTPCSTORE_SetOffset(void *obs, U32 offset);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode FTPCSTORE_Read(void *obs, U8* buff, U16 len)
 *
 * Description:   Read data from the object store entry.
 *                A function that performs this behavior is REQUIRED 
 *                by the OBEX Protocol.  This function fills in the 
 *                OBS_Read field of the ObStoreFuncTable (obex.h)
 *
 * Parameters:    obs - pointer to the handle of object store entry.
 *                      Casting from void* to the object store entry
 *                      must be done inside this function.
 *
 *                buff - pointer to location where data is read into.
 *
 *                len - length in bytes of data to read
 *
 * Returns:       OBRC_SUCCESS      - Read was successful.
 *                OBRC_UNAUTHORIZED - Read failed. 
 */
ObexRespCode FTPCSTORE_Read(void *obs, U8 *buff, U16 len);



/*---------------------------------------------------------------------------
 *
 * Prototype:     void FTPCSTORE_SetObjectLen(FtpObStoreHandle obs, U32 len)
 *
 * Description:   Set the length of a created object store item.
 *
 * Parameters:    obs - handle of object store entry.
 *
 *                len - length of the object.
 *
 * Returns:       void
 */
#define FTPCSTORE_SetObjectLen(obs, len) ((obs)->fileLen = (len))

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode FTPCSTORE_Write(void *obs, U8* buff, U16 len)
 *
 * Description:   Write data to the object store entry.
 *                A function that performs this behavior is REQUIRED 
 *                by the OBEX Protocol.  This function fills in the 
 *                OBS_Read field of the ObStoreFuncTable (obex.h)
 *
 * Parameters:    obs - pointer to the handle of object store entry.
 *                      Casting from void* to the object store entry
 *                      must be done inside this function.
 *
 *                buff - pointer to data to write
 *
 *                len - length in bytes of data to write
 *
 * Returns:       OBRC_SUCCESS      - Write was successful.
 *                OBRC_UNAUTHORIZED - Write failed. 
 */
ObexRespCode FTPCSTORE_Write(void *obs, U8 *buff, U16 len);


/****************************************************************************
 *
 * Object Store Definitions
 *
 * Definitions used internally by the object store.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Structure and Constants Reference
 *
 ****************************************************************************/

#define FTPCSTORE_MAX_NUM_ENTRIES 2

/****************************************************************************
 *
 * Data structures 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * FtpcStoreEntry Structure
 */
 struct _FtpcStoreEntry {
	ListEntry					node;
	

	/* Defines the location of the object, used to determine the relevant data in "location" */
	BtlObjectLocation				object_location;    /* FS or MEMORY location */

	U16						nameLen;
	U8						flags;
	FtpcstoreProgressCallBack	progressCallback;


	/* ======== fs object fields ============= */   
	U32						amount;
	U32						fileLen;
	U8						index;
	BthalFsFileDesc			fp;
	BtlUtf8					tempFile[BTHAL_FS_MAX_FILE_NAME_LENGTH];
	BtlUtf8					name[BTHAL_FS_MAX_FILE_NAME_LENGTH + 1];
	BtlUtf8					fullName[BTHAL_FS_MAX_FILE_NAME_LENGTH + BTHAL_FS_MAX_PATH_LENGTH + 1];


	/* ========= memory object fields ======== */
	const char				*memAddr;
	U32						memIndex;
	U32						memSize;
	
};


/*---------------------------------------------------------------------------
 *
 * Prototype:     void FTPCSTORE_SetFtpcstoreCallback(FptcStoreHandle *obs, ObmgrProgressCallBack progressCallback)
 *
 * Description:   Set the OBMGR callback for the object. This callback is called on
 *					every progress change.
 *
 * Parameters:    Obm - handle of object manager entry.
 *                progressCallback - callback.
 *
 * Returns:       void
 */
#define FTPCSTORE_SetFtpcstoreCallback(_OBS, _UIC)  (_OBS)->progressCallback = (_UIC)


#endif /* __FTPCSTOREH */

