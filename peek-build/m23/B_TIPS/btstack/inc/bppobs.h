#ifndef __BPPOBS_H
#define __BPPOBS_H

/****************************************************************************
 *
 * File:          bppobs.h
 *     $Workfile:bppobs.h$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision:15$
 *
 * Description:   This file specifies the OBEX object store interface.
 * 
 * Created:       May 15, 1997
 *
 * $Project:XTNDAccess Blue SDK$
 *
 * Copyright 1997-2002 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
#define BPP_OBS_MAX_NUM_ENTRIES 2
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
typedef struct _BppObStoreEntry {
    ListEntry       node;
    U16             bppOp;
    void           *app;
    U8             *buff;
    U32             objLen;
    U32             offset;
    U16             nameLen;
    U8              name[GOEP_MAX_UNICODE_LEN];
    U8              typeLen;
    U8              type[GOEP_MAX_TYPE_LEN];
    U8              appParms[255];
    void           *job;
    ObexRespCode    code;
    void           *ocx;
} BppObStoreEntry;

/*---------------------------------------------------------------------------
 *
 * BppObStoreHandle Structure
 */
typedef struct _BppObStoreEntry *BppObStoreHandle;


/****************************************************************************
 *
 * Function Reference 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Prototype:     void BPPOBS_AppendName(BppObStoreHandle obs,
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
void BPPOBS_AppendName(BppObStoreHandle obs, const U8 *name, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void BPPOBS_AppendType(BppObStoreHandle  obs, 
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
void BPPOBS_AppendType(BppObStoreHandle obs, const U8 *type, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode BPPOBS_Close(BppObStoreHandle *obs)
 *
 * Description:   Close an object store item.
 *
 * Parameters:    obshPtr - handle of object store entry.
 *
 * Returns:       OBRC_SUCCESS - Operation was successful.
 *                OBRC_UNAUTHORIZED - Could not create/rename object store item.
 *                OBRC_NOT_FOUND - Could not access object store item.
 */
ObexRespCode BPPOBS_Close(BppObStoreHandle *obshPtr);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void BPPOBS_Deinit()
 *
 * Description:   Deinitialize the object store.
 *
 * Parameters:    none 
 *
 * Returns:       void
 */
void BPPOBS_Deinit(void);
#define BPPOBS_Deinit()

/*---------------------------------------------------------------------------
 *
 * Prototype:     char* BPPOBS_GetNameAscii(BppObStoreHandle obs)
 *
 * Description:   Return a pointer to the Object Store item's name in Ascii.
 *
 * Parameters:    obs - handle of object store entry.
 *
 * Returns:       Pointer to NULL terminated ASCII name.
 */
#define BPPOBS_GetNameAscii(obs) ((obs)->name)

/*---------------------------------------------------------------------------
 *
 * Prototype:     U32 BPPOBS_GetObjectLen(void *obs)
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
U32 BPPOBS_GetObjectLen(void *obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL BPPOBS_Init()
 *
 * Description:   Initialize the object store.
 *
 * Parameters:    none 
 *
 * Returns:       TRUE - object store was initialized successfully. 
 *                FALSE - Unable to create inbox.
 */
BOOL BPPOBS_Init(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     BppObStoreHandle BPPOBS_New()
 *
 * Description:   Get a handle to a blank object store item.
 *
 * Parameters:    none 
 *
 * Returns:       Handle of an object store entity. 0 means out of objects. 
 */
BppObStoreHandle BPPOBS_New(void *app);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void BPPOBS_SetObjectLen(BppObStoreHandle obs, U32 len)
 *
 * Description:   Set the length of a created object store item.
 *
 * Parameters:    obs - handle of object store entry.
 *
 *                len - length of the object.
 *
 * Returns:       void
 */
#define BPPOBS_SetObjectLen(obs, len) ((obs)->objLen = (len))

#if BPP_NUM_SENDERS > 0
/*---------------------------------------------------------------------------
 *
 * Prototype:   ObexRespCode BPPOBS_ReadDocument(void *obs,
 *                                               U8* buff, U16 len)
 *
 * Description: Read file data from the object store entry.
 *              A function that performs this behavior is REQUIRED 
 *              by the OBEX Protocol.  This function fills in the 
 *              OBS_Read field of the ObStoreFuncTable (obex.h)
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
ObexRespCode BPPOBS_ReadDocument(void *obs, U8* buff, U16 len);
ObexRespCode BPPOBS_ReadFlex(void *obs, U8* buff, U16 *len, BOOL *more);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode BPPOBS_RecvSoapResponse(void *obs,
 *                                                     U8* buff, U16 len)
 *
 * Description:   Send SOAP data to the Sender.
 *                A function that performs this behavior is REQUIRED 
 *                by the OBEX Protocol.  This function fills in the 
 *                OBS_Write field of the ObStoreFuncTable (obex.h)
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
ObexRespCode BPPOBS_RecvSoapResponse(void *obs, U8 *buff, U16 len);
#endif /* BPP_NUM_SENDERS */

#if BPP_NUM_PRINTERS > 0
/*---------------------------------------------------------------------------
 *
 * Prototype:   ObexRespCode BPPOBS_ReadSoapResponse()
 *
 * Description: Read SOAP data from the object store entry.
 *              A function that performs this behavior is REQUIRED 
 *              by the OBEX Protocol.  This function fills in the 
 *              OBS_Read field of the ObStoreFuncTable (obex.h)
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
ObexRespCode BPPOBS_ReadSoapResponse(void *obs, U8* buff, U16 len);

/*---------------------------------------------------------------------------
 *
 * Prototype:   ObexRespCode BPPOBS_ReadGetEvent()
 *
 * Description: Read SOAP GetEvent data from the object store entry.
 *              A function that performs this behavior is REQUIRED 
 *              by the OBEX Protocol.  This function fills in the 
 *              OBS_ReadFlex field of the ObStoreFuncTable (obex.h)
 *
 * Parameters:  obs - pointer to the handle of object store entry.
 *                    Casting from void* to the object store entry
 *                    must be done inside this function.
 *
 *              buff - pointer to location where data is read into.
 *
 *              len - On input, contains the maximum length in bytes of data
 *                    to read. Before returning, the object store must write
 *                    the actual number of bytes read into "buff".
 *
 *              more - Before returning, the object store must write "TRUE"
 *                     or "FALSE" into the boolean referenced by this
 *                     parameter. TRUE indicates that there is more data to
 *                     read, FALSE indicates that all object data has been
 *                     read.
 *
 * Returns:     OBRC_SUCCESS      - Read was successful.
 *              OBRC_UNAUTHORIZED - Read failed. 
 */
ObexRespCode BPPOBS_ReadGetEvent(void *obs, U8* buff, U16 *len, BOOL *more);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode BPPOBS_Print(void *obs, U8* buff, U16 len)
 *
 * Description:   Send print data to the Printer.
 *                A function that performs this behavior is REQUIRED 
 *                by the OBEX Protocol.  This function fills in the 
 *                OBS_Write field of the ObStoreFuncTable (obex.h)
 *
 * Parameters:    obs - pointer to the handle of object store entry.
 *                      Casting from void* to the object store entry
 *                      must be done inside this function.
 *
 *                buff - pointer to data to print.
 *
 *                len - length in bytes of data to print.
 *
 * Returns:       OBRC_SUCCESS      - Print was successful.
 *                OBRC_UNAUTHORIZED - Print failed. 
 */
ObexRespCode BPPOBS_Print(void *obs, U8 *buff, U16 len);
#endif /* BPP_NUM_PRINTERS */

#endif /* __BPPOBS_H */

