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
*   FILE NAME:      obmgr.h
*
*   DESCRIPTION:    This file specifies the OBEX Object Manager interface.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/


#ifndef __OBMGR_H
#define __OBMGR_H


#include "osapi.h"
#include "obexerr.h"
#include "bthal_fs.h"

#include "fstore.h"
#include "pathmgr.h"
#include "browser.h"
#include "btl_unicode.h"

/*---------------------------------------------------------------------------
 *
 * ObStoreHandle Structure
 */

/* Forward declarations */
typedef struct _ObStoreMgr 	ObStoreMgr;


/* Indicates a progress change */
typedef void (*ObmgrProgressCallBack)(ObStoreMgr *obm);


#define OBM_NUM_ENTRIES     2
#define OBM_MAX_TYPE_LEN    32

#define OBMF_INUSE          0x01   /* Entry is inuse */
#define OBMF_UNICODE_ODD    0x02   /* AppendNameUni() position flag */
#define OBMF_DELETE         0x03   /* Delete object on close */

struct _ObStoreMgr {
    U8          					flags;
    U8          					store;
    U16         					nameLen;
    BtlUtf8        					name[BTHAL_FS_MAX_PATH_LENGTH];
    U8          					typeLen;
    char        					type[OBM_MAX_TYPE_LEN];
    U32         					objectLen;
    U32         					amount;
	U8*								hideObjects[BTL_CONFIG_FTPS_MAX_HIDE_OBJECT];
	BOOL							hideMode;
    ObmgrProgressCallBack         	progressCallback;
    union {
        FtpObStoreHandle file;
    } handle;
};

/* Open, Create, Close, Read, Write, Readflex, GetObjectLen */

typedef struct _ObStoreType {
    const char     *name;
    U8              obmt;
} ObStore;

/* Object Store Type */
#define OBMT_UNSPECIFIED    0
#define OBMT_GENERIC        1
#define OBMT_BROWSER        2
#define OBMT_VSTORE         3

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * Constant indicates that the object store item has an unknown length
 */
#define UNKNOWN_OBJECT_LENGTH   0xFFFFFFFF
#endif

 
/****************************************************************************
 *
 * Function Reference 
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBM_Create(ObStoreMgr *Obm)
 *
 * Description:   Open the specified object store item for writing. An
 *                appropriate object store is determined automatically or
 *                it can be overridden by specifying Obm->store before
 *                making this call.
 *
 * Parameters:    Obm - pointer to the object manager entry.
 *
 * Returns:       OBRC_SUCCESS - Operation was successful.
 *                OBRC_NOT_FOUND - Could not create object store item.
 */
ObexRespCode OBM_Create(ObStoreMgr *obm);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBM_Open(ObStoreMgr *Obm)
 *
 * Description:   Open an object manager object for reading. An appropriate
 *                object store is determined automatically or it can be
 *                overridden by specifying Obm->store before making this call.
 *
 * Parameters:    Obm - pointer to the object manager entry.
 *
 * Returns:       OBRC_SUCCESS indicates that the open is successful. 
 *                OBRC_NOT_FOUND indicates failure. 
 */
ObexRespCode OBM_Open(ObStoreMgr *obm);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBM_Close(ObStoreMgr **ObmPtr)
 *
 * Description:   Close an object manager item.
 *
 * Parameters:    ObmPtr - pointer to the object manager entry.
 *
 * Returns:       OBRC_SUCCESS - Operation was successful.
 *                OBRC <other> - Operation failed.
 */
ObexRespCode OBM_Close(ObStoreMgr **ObmPtr);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBM_Delete(ObStoreMgr **ObmPtr)
 *
 * Description:   Delete the object and free the storage.
 *
 * Parameters:    ObmPtr - Handle to object manager entry to delete. 
 *
 * Returns:       OBRC_NOT_FOUND - Object could not be deleted.
 *                OBRC_SUCCESS   - Object was deleted.
 */
ObexRespCode OBM_Delete(ObStoreMgr **ObmPtr);
 
/*---------------------------------------------------------------------------
 *
 * Prototype:     U32 OBM_GetObjectLen(void *obs)
 *
 * Description:   Get the length of an object store item.
 *                This function is REQUIRED by the OBEX Protocol.
 *
 * Parameters:    obs - pointer to the handle of object store entry.
 *                      Casting from void* to the object store entry
 *                      must be done inside this function.
 *
 * Returns:       The object length
 */
U32 OBM_GetObjectLen(void *obs);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObStoreMgr *OBM_New()
 *
 * Description:   Get a handle to a new object manager record.
 *
 * Parameters:    none 
 *
 * Returns:       Handle of an object manager record. 0 means out of objects. 
 */
ObStoreMgr *OBM_New(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBM_Read(void *obs, U8* buff, U16 len)
 *
 * Description:   Read data from the object manager entry.
 *                This function is REQUIRED by the OBEX Protocol.
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
ObexRespCode OBM_Read(void *obs, U8 *buff, U16 len);

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBM_ReadFlex(void *obs, U8 *buff, U16 *len, BOOL *more)
 *
 * Description:   Read data from the object manager entry.
 *                This function is REQUIRED by the OBEX Protocol if
 *                OBEX_DYNAMIC_OBJECT_SUPPORT is enabled.
 *
 * Parameters:    obs - pointer to the handle of object store entry.
 *                      Casting from void* to the object store entry
 *                      must be done inside this function.
 *
 *                buff - pointer to location where data is read into.
 *
 *                len - maximum number of bytes of data to read
 *
 *                more - sets to TRUE if it has more data to read.
 *
 * Returns:       OBRC_SUCCESS      - Read was successful.
 *                OBRC_UNAUTHORIZED - Read failed. 
 */
ObexRespCode OBM_ReadFlex(void *obs, U8 *buff, U16 *len, BOOL *more);
#endif

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBM_Write(void *obs, U8* buff, U16 len)
 *
 * Description:   Write data to the object store entry.
 *                This function is REQUIRED by the OBEX Protocol.
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
ObexRespCode OBM_Write(void *obs, U8 *buff, U16 len);


/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode OBM_AppendNameAscii( ObStoreMgr  Obm, 
 *                                                const U8   *Name, 
 *                                                U16         Len )
 *
 * Description:   Append the unicode string to the objects name. A state
 *                machine is used so the unicode string does not have to
 *                be an even number of bytes.
 *
 * Parameters:    Obm - handle of object manager entry.
 *                Name - pointer to Unicode string to append
 *                Len - length in bytes of string to append
 *
 * Returns:       OBRC_SUCCESS      - Appended Name successfully.
 *                OBRC_UNAUTHORIZED - Unicode Name did not fit in buffer.
 */
ObexRespCode OBM_AppendNameAscii(ObStoreMgr *Obm, const U8 *Name, U16 Len);


#define OBM_GetNameAscii(_OBM)  ((_OBM)->name)

/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL OBM_AppendType(ObStoreMgr    Obm, 
 *                                    const char   *Type,
 *                                    U16           Len )
 *
 * Description:   Append the ASCII string to the object type. 
 *
 * Parameters:    Obm - handle of object manager entry.
 *                Type- pointer to ASCII string to append
 *                Len - length in bytes of string to append
 *
 * Returns:       TRUE  - String was successfully appended
 *                FALSE - Insufficient buffer space to append string.
 */
BOOL OBM_AppendType(ObStoreMgr *Obm, const char *Type, U16 Len);

#define OBM_GetTypeLen(_OBM)    ((_OBM)->typeLen)

#define OBM_GetTypeAscii(_OBM)  ((_OBM)->type)

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBM_SetObjectLen(ObStoreMgr *Obm, U32 Len)
 *
 * Description:   Set the length of a created object manager object.
 *
 * Parameters:    Obm - handle of object manager entry.
 *                Len - length of the object.
 *
 * Returns:       void
 */
#define OBM_SetObjectLen(Obm, Len) ((Obm)->objectLen = (Len))

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBM_SetObmgrCallback(ObStoreMgr *Obm, ObmgrProgressCallBack progressCallback)
 *
 * Description:   Set the OBMGR callback for the object. This callback is called on
 *					every progress change.
 *
 * Parameters:    Obm - handle of object manager entry.
 *                progressCallback - callback.
 *
 * Returns:       void
 */
void OBM_SetObmgrCallback(ObStoreMgr *Obm, ObmgrProgressCallBack progressCallback);
#define OBM_SetObmgrCallback(_OBS, _UIC)  (_OBS)->progressCallback = (_UIC)

/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL OBM_Init()
 *
 * Description:   Initialize the object manager.
 *
 * Parameters:    none 
 *
 * Returns:       TRUE - object manager was initialized successfully. 
 *                FALSE - Unable to init.
 */
BOOL OBM_Init(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void OBM_Deinit()
 *
 * Description:   Deinitialize the object store manager.
 *
 * Parameters:    none 
 *
 * Returns:       void
 */
void OBM_Deinit(void);

#endif	/* __OBMGR_H */
