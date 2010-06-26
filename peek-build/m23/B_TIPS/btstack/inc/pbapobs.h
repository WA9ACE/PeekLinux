#ifndef __PBAPOBS_H
#define __PBAPOBS_H

/****************************************************************************
 *
 * File:          pbapobs.h
 *     $Workfile:pbapobs.h$ for iAnywhere Blue SDK, PBAP Version 1.1.2
 *     $Revision:9$
 *
 * Description:   This file specifies the OBEX object store interface.
 * 
 * $Project:XTNDAccess Blue SDK$
 *
 * Copyright 2005-2006 Extended Systems, Inc.
 * Portions copyright 2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "obexerr.h"
#include "utils.h"
#include "obex.h"

/*---------------------------------------------------------------------------
 *
 * Phonebook Access Profile Object Store (PBAPOBS) Layer
 *
 *     These functions are used internally by PBAP to handle the OBEX object
 *     store requirements.  However, PBAP does receive information in
 *     response to the object store function calls.  Specifically, 
 *     PBAPOBS_Read, PBAPOBS_ReadFlex, and PBAPOPBS_Write will cause the 
 *     PBAP_EVENT_DATA_REQ and PBAP_EVENT_DATA_IND events to occur.  These
 *     events allow the PBAP application to deal with data requests and 
 *     data indications as they occur. More information on these events is
 *     available in pbap.h.
 *
 */

/****************************************************************************
 *
 * Section: Default general constants that can be overridden in overide.h
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * OBS_MAX_NUM_ENTRIES Constant
 *
 *  Defines the number of object store entries supported.
 *
 *  The default value is 4.
 */
#if XA_ENABLED == TI_CHANGES 
#define PBAP_OBS_MAX_NUM_ENTRIES 1
#else
#define OBS_MAX_NUM_ENTRIES 1
#endif


/****************************************************************************
 *
 * Section: Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * PbapObStoreEntry structure
 *
 *  Phonebook Access Profile object store entry.
 */
typedef struct _PbapObStoreEntry {
    ListEntry       node;
    void           *app;
    U32             objLen;
    U32             offset;
    U16             nameLen;
    U8              name[GOEP_MAX_UNICODE_LEN];
    U8              typeLen;
    U8              type[GOEP_MAX_TYPE_LEN];
    U32             pbapOp;
    ObexRespCode    code;
} PbapObStoreEntry;


/*---------------------------------------------------------------------------
 * PbapObStoreHandle struct
 *
 *  Phonebook Access Profile object store handle.
 */
typedef struct _PbapObStoreEntry *PbapObStoreHandle;

/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * PBAPOBS_AppendName()
 *
 *     Append the ASCII string to the name.
 *
 * Parameters:    
 *     obs - handle of object store entry.
 *
 *     name - pointer to ASCII string to append
 *
 *     len - length in bytes of string to append
 *
 * Returns:
 *     None
 */
void PBAPOBS_AppendName(PbapObStoreHandle obs, const U8 *name, U16 len);

/*---------------------------------------------------------------------------
 * PBAPOBS_AppendType()
 *
 *     Append the ASCII string to the object type.
 *
 * Parameters:    
 *     obs - handle of object store entry.
 *
 *     name - pointer to ASCII string to append
 *
 *     len - length in bytes of string to append
 *
 * Returns:
 *     None
 */
void PBAPOBS_AppendType(PbapObStoreHandle obs, const U8 *type, U16 len);

/*---------------------------------------------------------------------------
 * PBAPOBS_GetNameAscii()
 *
 *     Return a pointer to the Object Store item's name in ASCII.
 *
 * Parameters:    
 *     obs - handle of object store entry.
 *
 * Returns:
 *     Pointer to NULL terminated ASCII name.
 */
char* PBAPOBS_GetNameAscii(PbapObStoreHandle obs);
#define PBAPOBS_GetNameAscii(obs) ((obs)->name)

/*---------------------------------------------------------------------------
 * PBAPOBS_GetTypeAscii()
 *
 *     Return a pointer to the Object Store item's type in ASCII.
 *
 * Parameters:    
 *     obs - handle of object store entry.
 *
 * Returns:
 *     Pointer to NULL terminated ASCII type.
 */
char* PBAPOBS_GetTypeAscii(PbapObStoreHandle obs);
#define PBAPOBS_GetTypeAscii(obs) ((obs)->type)

/*---------------------------------------------------------------------------
 * PBAPOBS_GetObjectLen()
 *
 *     Get the length of an object store item. A function that performs 
 *     this behavior is REQUIRED by the OBEX Protocol.  This function fills 
 *     in the OBS_GetObjectLen field of the ObStoreFuncTable (obex.h)
 *
 * Parameters:    
 *     obs - pointer to the handle of object store entry. Casting from 
 *           void* to the object store entry must be done inside this 
 *           function.
 *
 * Returns:
 *     The object length.
 */
U32 PBAPOBS_GetObjectLen(void *obs);

/*---------------------------------------------------------------------------
 * PBAPOBS_Init()
 *
 *     Initialize the object store.
 *
 * Parameters:    
 *     None
 *
 * Returns:
 *     TRUE - object store was initialized successfully. 
 *     FALSE - Unable to create inbox.
 */
BOOL PBAPOBS_Init(void);

/*---------------------------------------------------------------------------
 * PBAPOBS_New()
 *
 *     Get a handle to a blank object store item.
 *
 * Parameters:    
 *     app - pointer to the GOEP client or server using the object store handle.
 *
 * Returns:
 *     Handle of an object store entity. 0 means out of objects. 
 */
PbapObStoreHandle PBAPOBS_New(void *app);

/*---------------------------------------------------------------------------
 * PBAPOBS_Close()
 *
 *     Close an object store item.
 *
 * Parameters:    
 *     obshPtr - handle of object store entry.
 *
 * Returns:
 *     OBRC_SUCCESS - Operation was successful.
 *     OBRC_UNAUTHORIZED - Could not create/rename object store item.
 *     OBRC_NOT_FOUND - Could not access object store item. 
 */
ObexRespCode PBAPOBS_Close(PbapObStoreHandle *obshPtr);

/*---------------------------------------------------------------------------
 * PBAPOBS_SetObjectLen()
 *
 *     Set the length of a created object store item.
 *
 * Parameters:    
 *     obs - handle of object store entry.
 *
 *     len - length of the object.
 *
 * Returns:
 *     None
 */
void PBAPOBS_SetObjectLen(PbapObStoreHandle obs, U32 len);
#define PBAPOBS_SetObjectLen(obs, len) ((obs)->objLen = (len))

/*---------------------------------------------------------------------------
 * PBAPOBS_Read()
 *
 *     Read a phonebook object from the Server. A function that performs 
 *     this behavior is REQUIRED by the OBEX Protocol. This function fills 
 *     in the OBS_Read field of the ObStoreFuncTable (obex.h). This 
 *     function will trigger the PBAP_EVENT_DATA_REQ event to indicate that 
 *     the PBAP server must retrieve the requested data from its phonebook 
 *     databases.
 *
 * Parameters:    
 *     obs - pointer to the handle of object store entry. Casting from 
 *           void* to the object store entry must be done inside this 
 *           function.
 *
 *     buff - pointer to location where data is read into.
 *
 *     len - length in bytes of data to read
 *
 * Returns:
 *     OBRC_SUCCESS      - Read was successful.
 *     OBRC_UNAUTHORIZED - Read failed.
 */
ObexRespCode PBAPOBS_Read(void *obs, U8* buff, U16 len);

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * PBAPOBS_ReadFlex()
 *
 *     Read a phonebook object from the Server. The Pull Vcard Listing 
 *     operation will typically use this method, as it does not know the 
 *     size of the dynamic folder listing ahead of time.  A function that 
 *     performs this behavior is REQUIRED by the OBEX Protocol if 
 *     OBEX_DYNAMIC_OBJECT_SUPPORT is enabled.  This function fills in the 
 *     OBS_ReadFlex field of the ObStoreFuncTable (obex.h). This function 
 *     will trigger the PBAP_EVENT_DATA_REQ event to indicate that the PBAP 
 *     server must retrieve the requested data from its phonebook databases.
 *
 * Parameters:    
 *     obs - pointer to the handle of object store entry. Casting from 
 *           void* to the object store entry must be done inside this 
 *           function.
 *
 *     buff - pointer to location where data is read into.
 *
 *     len -  On input, contains the maximum length in bytes of 
 *            data to read. Before returning, the object store 
 *            must write the actual number of bytes read into "buff".
 *
 *     more - Before returning, the object store must write 
 *            "TRUE" or "FALSE" into the boolean referenced by 
 *            this parameter. TRUE indicates that there is more 
 *            data to read, FALSE indicates that all object data 
 *            has been read.
 *
 * Returns:
 *     OBRC_SUCCESS      - Read was successful.
 *     OBRC_UNAUTHORIZED - Read failed.
 */
ObexRespCode PBAPOBS_ReadFlex(void *obs, U8 *buff, U16 *len, BOOL *more);
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * PBAPOBS_Write()
 *
 *     Write the phonebook object data from the Server.  A function that 
 *     performs this behavior is REQUIRED by the OBEX Protocol. This 
 *     function fills in the OBS_Write field of the ObStoreFuncTable 
 *     (obex.h). This function will trigger the PBAP_EVENT_DATA_IND event 
 *     to indicate that the PBAP client must handle the received data from 
 *     the server's phonebook databases.
 *
 * Parameters:    
 *     obs - pointer to the handle of object store entry. Casting from 
 *           void* to the object store entry must be done inside this 
 *           function.
 *
 *     buff - pointer to data to send.
 *
 *     len - length in bytes of data to send.
 *
 * Returns:
 *     OBRC_SUCCESS - Print was successful.
 *     OBRC_UNAUTHORIZED - Print failed.
 */
ObexRespCode PBAPOBS_Write(void *obs, U8 *buff, U16 len);
#endif /* __PBAPOBS_H */
