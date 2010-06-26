#ifndef __OBSTOREH
#define __OBSTOREH

/****************************************************************************
 *
 * File:          obstore.h
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

#include "obexerr.h"
#include "utils.h"
#include "obex.h"
#include "goep.h"
#include "bthal_fs.h"
#include "btl_config.h"
#include "btl_common.h"
#include "btl_unicode.h"

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
 * OppObStoreHandle Structure
 */
typedef struct _OppObStoreEntry *OppObStoreHandle;


/* Forward declarations */
typedef struct _OppObStoreEntry OppObStoreEntry;	

/* Indicates a progress change */
typedef void (*ObstoreProgressCallBack)(OppObStoreEntry *obs);


/*---------------------------------------------------------------------------
 *
 * Constants used with OBSTORE_UpdatePathUnicode() function
 */
#define OBS_UPU_NONE            0x00
#define OBS_UPU_BACKUP          0x01
#define OBS_UPU_DONT_CREATE     0x02

/****************************************************************************
 *
 * Function Reference 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_AppendNameAscii(OppObStoreHandle obs, 
 *                                             const BtlUtf8    *fsPath)
 *
 * Description:   Append the ASCII string to the name. 
 *
 * Parameters:    obs - handle of object store entry.
 *
 *                name - pointer to ASCII string to append - FULL file name with path
 *
 * Returns:       void 
 */
void OBSTORE_AppendNameAscii(OppObStoreHandle obs, const BtlUtf8 *fsPath);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_AppendNameUnicode(OppObStoreHandle obs, 
 *                                                       const U8     *name, 
 *                                                       U16           len)
 *
 * Description:   Append the unicode string to the name. A state machine is 
 *                used so the unicode string does not have to be an even 
 *                number of bytes.
 *
 * Parameters:    obs - handle of object store entry.
 *                name - pointer to Unicode string to append
 *                len - length in bytes of string to append
 *
 * Returns:       OBRC_SUCCESS      - Appended Name successfully.
 *                OBRC_UNAUTHORIZED - Unicode Name did not fit in buffer.
 */
ObexRespCode OBSTORE_AppendNameUnicode(OppObStoreHandle obs, const U8 *name, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_AppendPathUnicode(const U8  *path,
 *                                                       U16        len,
 *                                                       BOOL       first )
 *
 * Description:   Append the Unicode path to the current OBEX Server path.
 *
 * Parameters:    path  - Unicode path string.
 *                len   - Length of the path.
 *                first - Start of a new path.
 *
 * Returns:       OBRC_SUCCESS      - The path was successfully appended.
 *                OBRC_UNAUTHORIZED - Unable to append the path.
 */
ObexRespCode OBSTORE_AppendPathUnicode(const U8 *path, U16 len, BOOL first);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_AppendPathAscii(const U8  *path,
 *                                                       U16        len,
 *                                                       BOOL       first )
 *
 * Description:   Append the Ascii path to the current OBEX Server path.
 *
 * Parameters:    path  - Unicode path string.
 *                len   - Length of the path.
 *                first - Start of a new path.
 *
 * Returns:       OBRC_SUCCESS      - The path was successfully appended.
 *                OBRC_UNAUTHORIZED - Unable to append the path.
 */
ObexRespCode OBSTORE_AppendPathAscii(const U8 *path, U16 len, BOOL first);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_AppendType(OppObStoreHandle  obs, 
 *                                        const U8      *type,
 *                                        U16            len )
 *
 * Description:   Append the ASCII string to the object type. 
 *
 * Parameters:    obs - handle of object store entry.
 *                type- pointer to ASCII string to append
 *                len - length in bytes of string to append
 *
 * Returns:       void 
 */
void OBSTORE_AppendType(OppObStoreHandle obs, const U8 *type, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_CancelPathUpdate(void)
 *
 * Description:   Cancel a path update.
 *
 * Parameters:    none 
 *
 * Returns:       void
 */
void OBSTORE_CancelPathUpdate(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_Close(OppObStoreHandle *obs)
 *
 * Description:   Close an object store item.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       OBRC_SUCCESS - Operation was successful.
 *                OBRC_UNAUTHORIZED - Could not create/rename object store item.
 *                OBRC_NOT_FOUND - Could not access object store item.
 */
ObexRespCode OBSTORE_Close(OppObStoreHandle *obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_Create(OppObStoreHandle *obs)
 *
 * Description:   Open the specified object store item for writing. 
 *
 * Parameters:    
 *                obs - Handle of object store item to create.
 *				  op - current operation
 *
 * Returns:       OBRC_SUCCESS - Operation was successful.
 *                OBRC_NOT_FOUND - Could not create object store item.
 */

ObexRespCode OBSTORE_Create(OppObStoreHandle obs, const U8 op);

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
void OBSTORE_Deinit(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_Delete(OppObStoreHandle *obs)
 *
 * Description:   Delete the object and free the storage.
 *
 * Parameters:    obs - handle to object store entry to delete. 
 *
 * Returns:       OBRC_NOT_FOUND - File could not be deleted.
 *                OBRC_SUCCESS   - File was deleted.
 */
ObexRespCode OBSTORE_Delete(OppObStoreHandle *obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     BtlUtf8* OBSTORE_GetNameAscii(OppObStoreHandle obs)
 *
 * Description:   Return a pointer to the Object Store item's name in Ascii.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       Pointer to NULL terminated ASCII name.
 */
#define OBSTORE_GetNameAscii(obs) ((obs)->objectName)

/*---------------------------------------------------------------------------
 *
 * Prototype:     char* OBSTORE_GetTypeAscii(OppObStoreHandle obs)
 *
 * Description:   Return a pointer to the Object Store item's type in Ascii.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       Pointer to NULL terminated ASCII type.
 */
#define OBSTORE_GetTypeAscii(obs) ((obs)->type)

/*---------------------------------------------------------------------------
 *
 * Prototype:     U16 OBSTORE_GetNameLenUnicode(OppObStoreHandle obs)
 *
 * Description:   Get the Unicode length of the object stores entries
 *                name including the terminator.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       Number of bytes in the Unicode name.
 */
#define OBSTORE_GetNameLenUnicode(_OBS) (((_OBS)->nameLen + 1) * 2)

/*---------------------------------------------------------------------------
 *
 * Prototype:     U32 OBSTORE_GetObjectLen(void *obs)
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
U32 OBSTORE_GetObjectLen(void *obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL OBSTORE_Init()
 *
 * Description:   Initialize the object store.
 *
 * Parameters:    none 
 *
 * Returns:       TRUE - object store was initialized successfully. 
 *                FALSE - Unable to create inbox.
 */
BOOL OBSTORE_Init(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     OppObStoreHandle OBSTORE_New()
 *
 * Description:   Get a handle to a blank object store item.
 *
 * Parameters:    none 
 *
 * Returns:       Handle of an object store entity. 0 means out of objects. 
 */
OppObStoreHandle OBSTORE_New(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_Open(OppObStoreHandle obs)
 *
 * Description:   Open an object store item for reading. If no name has been
 *                set, then open the default object.
 *
 * Parameters:    obs - pointer to object store entry
 *                op - current opp operation (i.e. Basic push/pull/exchange or Encapsulated push/pull/exchange)
 *                
 * Returns:       OBRC_SUCCESS indicates that the open is successful. 
 *                OBRC_NOT_FOUND indicates failure. 
 */
ObexRespCode OBSTORE_Open(OppObStoreHandle obs, const U8 op);

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_SetOffset(void *obs, 
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
ObexRespCode OBSTORE_SetOffset(void *obs, U32 offset);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_Read(void *obs, U8* buff, U16 len)
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
ObexRespCode OBSTORE_Read(void *obs, U8 *buff, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_ResetPath(void)
 *
 * Description:   Reset the path to the inbox root.
 *
 * Parameters:    none 
 *
 * Returns:       void
 */
void OBSTORE_ResetPath(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_SetNameDefault(OppObStoreHandle obs)
 *
 * Description:   Set the name of the Object store entry to the default name
 *                for a received object that has no name.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       void 
 */
void OBSTORE_SetNameDefault(OppObStoreHandle obs, const BtlUtf8 *dir);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_SetObjectLen(OppObStoreHandle obs, U32 len)
 *
 * Description:   Set the length of a created object store item.
 *
 * Parameters:    obs - handle of object store entry.
 *
 *                len - length of the object.
 *
 * Returns:       void
 */
#define OBSTORE_SetObjectLen(obs, len) ((obs)->objectSize = (len))


/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBSTORE_Write(void *obs, U8* buff, U16 len)
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
ObexRespCode OBSTORE_Write(void *obs, U8 *buff, U16 len);



/*---------------------------------------------------------------------------
 *
 * Prototype:  	void OBSTORE_SetFileName(OppObStoreHandle obs, const BtlUtf8 *name)
 *
 * Description:	In OPP server, this is the object name of the received object.  
 *				In OPP client, this is the object name of the transmitted object. 
 *
 * Parameters:	obs - handle of object manager entry.
 *				name - pointer to Ascii string to set as object name
 *
 * Returns:     void
 *                
 */
void OBSTORE_SetFileName(OppObStoreHandle obs, const BtlUtf8 *name);



/*---------------------------------------------------------------------------
 *
 * Prototype:  	ObexRespCode OBSTORE_CopyTempFsToMem(OppObStoreHandle obs)
 *
 * Description: Copy bytes from temp FS object to memory object, and
 *              remove temp FS object.
 *				
 *
 * Parameters:  obs - handle of object store entry.
 *
 * Returns:     OBRC_SUCCESS      - Change was successful.
 *              OBRC_UNAUTHORIZED - Change failed.
 */
ObexRespCode OBSTORE_CopyTempFsToMem(OppObStoreHandle obs);


/*---------------------------------------------------------------------------
 *
 * Prototype:  	ObexRespCode OBSTORE_PushFsSaveUserSettings(OppObStoreHandle obs, const BtlObject *object)
 *
 * Description: Save Push user settings
 *				
 *
 * Parameters:  obs - handle of object store entry.
 *
 * Returns:     OBRC_SUCCESS      - Setting parameters were valid.
 *              OBRC_UNAUTHORIZED - Setting parameters failed.
 */
ObexRespCode OBSTORE_PushFsSaveUserSettings(OppObStoreHandle obs, const BtlObject *object);


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

/*---------------------------------------------------------------------------
 *
 * Size Constants used by OppObStoreEntry
 */
#define OBS_MAX_TYPE_LEN        32
#define OPP_OBS_MAX_NUM_ENTRIES     2


/*---------------------------------------------------------------------------
 *
 * Defines operation field in OppObStoreEntry structure
 */
#define OPP_UNDEF_OPERATION     0x00
#define OPPC_PUSH_OPERATION     0x01
#define OPPC_PULL_OPERATION     0x02
#define OPPS_PUSH_OPERATION     0x03
#define OPPS_PULL_OPERATION     0x04


/****************************************************************************
 *
 * Data structures 
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 *
 * OppObStoreEntry Structure
 */
 struct _OppObStoreEntry {
	ListEntry               node;

	/* ======= common object fields ========== */

	/* In TX operation, this is the actual size of the transferred object in memory. */
	/* In RX operation, this is the max available space in memory in which the object */
	U32				objectSize;

	/* A null-terminated string representing the name of the object.
	In TX operation, this is the name of the transferred object as given to the peer.
	In RX operation, if the name is unknown set to NULL (name will be received from 
	peer), or set the name of the remote requested object. */
	BtlUtf8			objectName[BTHAL_FS_MAX_FILE_NAME_LENGTH];

	/* Defines the location of the object, used to determine the relevant data in "location" */
	BtlObjectLocation				object_location;    /* FS or MEMORY location */

	U32				amount;
	U16				nameLen;        
	U16				typeLen;
	ObstoreProgressCallBack progressCallback;
	U8				operation;	 


	/* ======== fs object fields ============= */   
	BtlUtf8			sysTempFsPath[BTHAL_FS_MAX_PATH_LENGTH + BTHAL_FS_MAX_FILE_NAME_LENGTH];
	BtlUtf8			fsPath[BTHAL_FS_MAX_PATH_LENGTH + BTHAL_FS_MAX_FILE_NAME_LENGTH];
	BtlUtf8			type[OBS_MAX_TYPE_LEN];
	BthalFsFileDesc	fp;


	/* ========= memory object fields ======== */
	void				*mem_ocx;			/* memory object pointer */

};


extern BtlObject OBS_DefaultObject;
extern BtlUtf8 OBS_DefaultDir[];


/*---------------------------------------------------------------------------
 * OBSTORE_FindPath()
 *
 * Synopsis:    function searchs for the Path (Exclude the file name)
 *
 * Parameters:  [in] source - holds absolute path name (i.e. directory + fileName)
 *
 *              [out] dest - only the absolute Path name
 *
 * Return:      void
 *
 */
void OBSTORE_FindPath(const BtlUtf8 *source, BtlUtf8 *dest);


/*---------------------------------------------------------------------------
 * OBSTORE_SetInboxDir()
 *
 * Synopsis:    Change Inbox Directory by given string
 *
 * Parameters:  dir - string which represents the new inboxDir
 *
 * Return:  TRUE  - Operation is successful.
 *            
 *          FALSE - Operation failed
 *
 */
BOOL OBSTORE_SetInboxDir(const BtlUtf8* dir);


/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBSTORE_SetObstoreCallback(OppObStoreHandle *obs, ObmgrProgressCallBack progressCallback)
 *
 * Description:   Set the OBMGR callback for the object. This callback is called on
 *					every progress change.
 *
 * Parameters:    obs - handle of object manager entry.
 *                progressCallback - callback.
 *
 * Returns:       void
 */
void OBSTORE_SetObstoreCallback(OppObStoreHandle *obs, ObstoreProgressCallBack progressCallback);
#define OBSTORE_SetObstoreCallback(_OBS, _UIC)  (_OBS)->progressCallback = (_UIC)


/*---------------------------------------------------------------------------
 * OBSTORE_ExtractObjName()
 *
 * Synopsis:    Searchs for the Path (Exclude the file name)
 *
 * Parameters:  [in] source - holds absolute path name (i.e. directory + fileName)
 *
 *              [out] dest - only the file name
 *
 * Return:      void
 *
 */
void OBSTORE_ExtractObjName(const BtlUtf8 *source, BtlUtf8 *dest);


/*---------------------------------------------------------------------------
 * OBSTORE_FileCloseAndReopen()
 *
 * Synopsis:    Closes obsPtr->fp and reopen object->location.fsLocation.fsPath.
 *
 * Parameters:  [in] obs - handle of object store entry.
 *
 *              [in] object - holds absolute path name (i.e. directory + fileName)
 *
 * Return:      OBRC_SUCCESS   - Operation is successful.
 *              OBRC_NOT_FOUND - Operation failed.
 *
 */
ObexRespCode OBSTORE_FileCloseAndReopen(OppObStoreHandle *obsPtr, const BtlObject *object);


#endif /* __OBSTOREH */

