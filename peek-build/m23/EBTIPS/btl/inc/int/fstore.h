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
*   FILE NAME:      fstore.h
*
*   DESCRIPTION:    This file specifies the file based object store interface.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/


#ifndef __FSTORE_H
#define __FSTORE_H


#include <bthal_fs.h>
#include <obexerr.h>
#include <utils.h>
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
typedef struct _FtpObStoreEntry *FtpObStoreHandle;

/****************************************************************************
 *
 * Function Reference 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode FSTORE_Close(FtpObStoreHandle *ObsPtr)
 *
 * Description:   Close an object store item.
 *
 * Parameters:    obsPtr - handle of object store entry.
 *
 * Returns:       OBRC_SUCCESS - Operation was successful.
 *                OBRC_UNAUTHORIZED - Could not rename object store item.
 */
ObexRespCode FSTORE_Close(FtpObStoreHandle *ObsPtr);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode FSTORE_Create(FtpObStoreHandle  *ObsPtr, 
 *                                           const char     *Name)
 *
 * Description:   Open the specified object store item for writing. ObsPtr
 *                is set to the newly created FtpObStoreHandle on success.
 *
 * Parameters:    ObsPtr - Handle of object store item to create.
 *                Name   - ASCII name of object.
 *
 * Returns:       OBRC_SUCCESS - Operation was successful.
 *                OBRC_NOT_FOUND - Could not create object store item.
 */
ObexRespCode FSTORE_Create(FtpObStoreHandle *ObsPtr, const BtlUtf8 *Name);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode FSTORE_Delete(FtpObStoreHandle *ObsPtr,
 *                                           const char    *Name)
 *
 * Description:   Delete the object and free the storage. The object
 *                can be deleted by providing its FtpObStoreHandle if it
 *                is open, or by providing its Name if it is not open.
 *
 * Parameters:    ObsPtr - handle to object store entry to delete. 
 *                Name   - name of object to delete.
 *
 * Returns:       OBRC_NOT_FOUND - File could not be deleted.
 *                OBRC_SUCCESS   - File was deleted.
 */
ObexRespCode FSTORE_Delete(FtpObStoreHandle *obsPtr, const BtlUtf8 *Name);

/*---------------------------------------------------------------------------
 *
 * Prototype:     U32 FSTORE_GetObjectLen(FtpObStoreHandle obs)
 *
 * Description:   Get the length of an object store item.
 *                This function is REQUIRED by the OBEX Protocol.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       The object length
 */
U32 FSTORE_GetObjectLen(FtpObStoreHandle obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL FSTORE_Init()
 *
 * Description:   Initialize the file system object store.
 *
 * Parameters:    none 
 *
 * Returns:       TRUE - object store was initialized successfully. 
 *                FALSE - Unable to init.
 */
BOOL FSTORE_Init(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode FSTORE_Open(FtpObStoreHandle *ObsPtr, 
 *                                         const char    *Name )
 *
 * Description:   Open an object store item for reading. If Name is NULL
 *                then open the default object. ObsPtr is set to the newly
 *                opened FtpObStoreHandle on success.
 *
 * Parameters:    ObsPtr - Pointer to object store entry
 *                Name   - Name of object to open
 *
 * Returns:       OBRC_SUCCESS indicates that the open is successful. 
 *                OBRC_NOT_FOUND indicates failure. 
 */
ObexRespCode FSTORE_Open(FtpObStoreHandle *ObsPtr, const BtlUtf8 *Name);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode FSTORE_Read(FtpObStoreHandle    Obs, 
 *                                         U8              *Buff,
 *                                         U16              Len )
 *
 * Description:   Read data from the object store entry.
 *                This function is REQUIRED by the OBEX Protocol.
 *
 * Parameters:    Obs - handle of object store entry.
 *                Buff - pointer to location where data is read into.
 *                Len - length in bytes of data to read
 *
 * Returns:       OBRC_SUCCESS      - Read was successful.
 *                OBRC_UNAUTHORIZED - Read failed. 
 */
ObexRespCode FSTORE_Read(FtpObStoreHandle Obs, U8 *Buff, U16 Len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode FSTORE_Write(FtpObStoreHandle   Obs, 
 *                                          U8             *Buff,
 *                                          U16             Len )
 *
 * Description:   Write data to the object store entry.
 *                This function is REQUIRED by the OBEX Protocol.
 *
 * Parameters:    Obs - handle of object store entry.
 *                Buff - pointer to data to write
 *                Len - length in bytes of data to write
 *
 * Returns:       OBRC_SUCCESS      - Write was successful.
 *                OBRC_UNAUTHORIZED - Write failed. 
 */
ObexRespCode FSTORE_Write(FtpObStoreHandle Obs, U8 *Buff, U16 Len);

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
 * Size Constants used by ObStoreEntry
 */
#define FTP_OBS_MAX_NUM_ENTRIES 2

/****************************************************************************
 *
 * Data structures 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * ObStoreEntry Structure
 */
typedef struct _FtpObStoreEntry {
    ListEntry   	node;
    BthalFsFileDesc fp;
    const BtlUtf8 	*name;
    BtlUtf8        	tmpName[BTHAL_FS_MAX_PATH_LENGTH];
    U32         	fileLen;
} FtpObStoreEntry;


#endif /* __FSTORE_H */

