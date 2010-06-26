#ifndef __BIPOBS_H
#define __BIPOBS_H

/****************************************************************************
 *
 * File:          bipobs.h
 *     $Workfile:bipobs.h$ for XTNDAccess Blue SDK, Version 1.4
 *     $Revision:16$
 *
 * Description:   This file specifies the OBEX object store interface.
 * 
 * Created:       August 6, 2002
 *
 * $Project:XTNDAccess Blue SDK$
 *
 * Copyright 2002-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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

#include "stdio.h"
#include "obexerr.h"
#include "utils.h"
#include "obex.h"

/****************************************************************************
 *
 * Object Store Definitions
 *
 * Definitions used internally by the object store.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Constants Reference
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 *
 * Size Constants used by ObStoreEntry
 */
#if (TI_CHANGES == XA_ENABLED)
#define BIP_OBS_MAX_NUM_ENTRIES 2
#else
#define OBS_MAX_NUM_ENTRIES 4
#endif


/****************************************************************************
 *
 * Data structures 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * ObStoreEntry Structure
 */
typedef struct _BipObStoreEntry {
    ListEntry       node;
    void           *app;
    void           *ocx;
    U32             objLen;
    U32             offset;
    U16             nameLen;
    U8              name[GOEP_MAX_UNICODE_LEN];
    U8              typeLen;
    U8              type[GOEP_MAX_TYPE_LEN];
    U32             bipOp;
    ObexRespCode    code;
} BipObStoreEntry;

/*---------------------------------------------------------------------------
 *
 * BipObStoreHandle Structure
 */
typedef struct _BipObStoreEntry *BipObStoreHandle;


/****************************************************************************
 *
 * Function Reference 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Prototype:     void BIPOBS_AppendName(BipObStoreHandle obs,
 *                                       const U16    *name, 
 *                                       U16           len)
 *
 * Description:   Append the UTF16 string to the name. 
 *
 * Parameters:    obs - handle of object store entry.
 *
 *                name - pointer to UTF16 string to append
 *
 *                len - length in bytes of string to append
 *
 * Returns:       void 
 */
void BIPOBS_AppendName(BipObStoreHandle obs, const U8 *name, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void BIPOBS_AppendType(BipObStoreHandle  obs, 
 *                                       const U8      *type,
 *                                       U16            len )
 *
 * Description:   Append the ASCII string to the object type. 
 *
 * Parameters:    obs - handle of object store entry.
 *                type- pointer to ASCII string to append
 *                len - length in bytes of string to append
 *
 * Returns:       void 
 */
void BIPOBS_AppendType(BipObStoreHandle obs, const U8 *type, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void BIPOBS_Deinit()
 *
 * Description:   Deinitialize the object store.
 *
 * Parameters:    none 
 *
 * Returns:       void
 */
void BIPOBS_Deinit(void);
#define BIPOBS_Deinit()

/*---------------------------------------------------------------------------
 *
 * Prototype:     char* BIPOBS_GetNameAscii(BipObStoreHandle obs)
 *
 * Description:   Return a pointer to the Object Store item's name in Ascii.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       Pointer to NULL terminated ASCII name.
 */
#define BIPOBS_GetNameAscii(obs) ((obs)->name)

/*---------------------------------------------------------------------------
 *
 * Prototype:     U32 BIPOBS_GetObjectLen(void *obs)
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
U32 BIPOBS_GetObjectLen(void *obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL BIPOBS_Init()
 *
 * Description:   Initialize the object store.
 *
 * Parameters:    none 
 *
 * Returns:       TRUE - object store was initialized successfully. 
 *                FALSE - Unable to create inbox.
 */
#if TI_CHANGES == XA_ENABLED
BOOL BIPOBS_Init(void);
#else
BOOL BIPOBS_Init();
#endif

/*---------------------------------------------------------------------------
 *
 * Prototype:     BipObStoreHandle BIPOBS_New()
 *
 * Description:   Get a handle to a blank object store item.
 *
 * Parameters:    none 
 *
 * Returns:       Handle of an object store entity. 0 means out of objects. 
 */
BipObStoreHandle BIPOBS_New(void *app);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode BIPOBS_Close(BipObStoreHandle *obs)
 *
 * Description:   Close an object store item.
 *
 * Parameters:    obshPtr - handle of object store entry.
 *
 * Returns:       OBRC_SUCCESS - Operation was successful.
 *                OBRC_UNAUTHORIZED - Could not create/rename object store item.
 *                OBRC_NOT_FOUND - Could not access object store item.
 */
ObexRespCode BIPOBS_Close(BipObStoreHandle *obshPtr);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void BIPOBS_SetObjectLen(BipObStoreHandle obs, U32 len)
 *
 * Description:   Set the length of a created object store item.
 *
 * Parameters:    obs - handle of object store entry.
 *
 *                len - length of the object.
 *
 * Returns:       void
 */
#define BIPOBS_SetObjectLen(obs, len) ((obs)->objLen = (len))

/*---------------------------------------------------------------------------
 *
 * Prototype:   ObexRespCode BIPOBS_Read(void *obs, U8* buff, U16 len)
 *
 * Description: Read data from the Initiator or Responder.
 *
 * Parameters:  obs - pointer to the handle of object store entry.
 *                    Casting from void* to the object store entry
 *                    must be done inside this function.
 *
 *              buff - pointer to location where data is read into.
 *
 *              len - length in bytes of data to read
 *
 * Returns:     OBRC_SUCCESS      - Read was successful.
 *              OBRC_UNAUTHORIZED - Read failed. 
 */
ObexRespCode BIPOBS_Read(void *obs, U8* buff, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode BIPOBS_Write(void *obs, U8* buff, U16 len)
 *
 * Description:   Write data to the Initiator or Responder.
 *
 * Parameters:    obs - pointer to the handle of object store entry.
 *                      Casting from void* to the object store entry
 *                      must be done inside this function.
 *
 *                buff - pointer to data to send.
 *
 *                len - length in bytes of data to send.
 *
 * Returns:       OBRC_SUCCESS      - Print was successful.
 *                OBRC_UNAUTHORIZED - Print failed. 
 */
ObexRespCode BIPOBS_Write(void *obs, U8 *buff, U16 len);
#endif /* __BIPOBS_H */

