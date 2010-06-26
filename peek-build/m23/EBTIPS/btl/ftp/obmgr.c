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
*   FILE NAME:      obmgr.c
*
*   DESCRIPTION:    This file contains the code for the Object Store Manager.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/

#include "btl_config.h"
#if BTL_CONFIG_FTPS ==   BTL_CONFIG_ENABLED

/*******************************************************************************/

#include "obmgr.h"
#include "pathmgr.h"
#include "utils.h"
#include "btl_defs.h"


BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_FTP);

/****************************************************************************
 *
 * Local ROM data
 *
 ****************************************************************************/
static const char *defaultVCardName = "mycard.vcf";

static const ObStore ObmTypeTab[] = {
            { "x-obex/folder-listing", OBMT_BROWSER },
            { "text/x-vCard", OBMT_VSTORE },
            { "text/x-vMsg", OBMT_VSTORE },
            { 0, OBMT_GENERIC } };

/****************************************************************************
 *
 * Local RAM data
 *
 ****************************************************************************/
static ObStoreMgr ObmEntries[OBM_NUM_ENTRIES];
static U8 ObmInitCounter = 0;

/*---------------------------------------------------------------------------
 *            OBM_Init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the object store.
 *
 * Return:    TRUE - object store initialized.
 *            FALSE - could not create inbox.
 *
 */
BOOL OBM_Init(void)
{
	if (ObmInitCounter == 0)
	{
		if (FSTORE_Init() == FALSE)
        	return FALSE;

	    if (PATHMGR_Init() == FALSE)
	        return FALSE;
	}

	ObmInitCounter++;

	return TRUE;
}

/*---------------------------------------------------------------------------
 *            OBM_Deinit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the object store.
 *
 * Return:    void.
 *
 */
void OBM_Deinit(void)
{
	Assert( ObmInitCounter > 0 );
	
    ObmInitCounter--;
}

/*---------------------------------------------------------------------------
 *            OBM_New()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Obtain an object manager item.
 *
 * Return:    Handle of object manager item or 0 if no more handles.
 *
 */
ObStoreMgr *OBM_New(void)
{
    I8  i;

    for (i = 0; i < OBM_NUM_ENTRIES; i++) {
        if ((ObmEntries[i].flags & OBMF_INUSE) == 0) {
            /* Found available entry */
            OS_MemSet((ObmEntries+i), 0, sizeof(ObStoreMgr));
            ObmEntries[i].flags = OBMF_INUSE;
            return ObmEntries+i;
        }
    }

    BTL_LOG_INFO(("OBM_New: Out of entries.\n"));
    return 0;
}

#define OBM_Free(_OBM)  (Assert((_OBM)->flags & OBMF_INUSE),(_OBM)->flags = 0)

/*---------------------------------------------------------------------------
 *            OBM_AppendType()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Append the ASCII string to the object type field.
 *
 * Return:    void
 *
 */
BOOL OBM_AppendType(ObStoreMgr *Obm, const char *Type, U16 Len)
{
    Assert( Obm );

    if ((Len + Obm->typeLen) > (OBM_MAX_TYPE_LEN - 1)) {
        return FALSE;
    }

    if (Len > 0) {
        /* Append the ASCII string to the existing ASCII type name. */
        OS_MemCopy((Obm->type + Obm->typeLen), Type, Len);
        Obm->typeLen = (U8)(Obm->typeLen + Len);

        /* Put a null at the end of the type if one does not exists */
        if (Obm->type[Obm->typeLen] != 0) {
            Obm->type[Obm->typeLen] = 0;
            Obm->typeLen++;
        }
    }
    return TRUE;
}

/*---------------------------------------------------------------------------
 *            OBM_Open()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open and object store item for reading.
 *
 * Return:    OBRC_SUCCESS - Open is successful. 
 *            OBRC_NOT_FOUND - Failed to open file.
 */
ObexRespCode OBM_Open(ObStoreMgr *Obm)
{
    const ObStore  	*type;

    if (Obm->store == OBMT_UNSPECIFIED) {
        Obm->store = OBMT_GENERIC;

        if (Obm->typeLen) {
            /* Dispatch to the proper object store */
            type = ObmTypeTab;
            while (type->name) {
                if (OS_StriCmp(Obm->type, type->name) == 0) {
                    Obm->store = type->obmt;
                    break;
                }
                type++;
            }
        }
    }
        
    switch (Obm->store) {
    case OBMT_VSTORE:
        /*
         * On this platform, the VSTORE just maps to the generic filesystem. 
         * However, we need to handle any VSTORE specific behaviors first.
         */
        if (Obm->nameLen == 0) {
            OS_StrCpyUtf8(Obm->name, defaultVCardName);
        }
        /* Drop into generic store */

    case OBMT_GENERIC:
        return FSTORE_Open(&Obm->handle.file, Obm->name);

    case OBMT_BROWSER:
        return BROWSER_Open(Obm->name,Obm->hideObjects,Obm->hideMode);
    }

    return OBRC_NOT_FOUND;
}

/*---------------------------------------------------------------------------
 *            OBM_Create()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open an object store item for writing.
 *
 * Return:    Success or failure of create operation.
 *
 */
ObexRespCode OBM_Create(ObStoreMgr *obm)
{
    const ObStore  *type;

    if (obm->store == OBMT_UNSPECIFIED) {
        obm->store = OBMT_GENERIC;

        if (obm->typeLen) {
            /* Dispatch to the proper object store */
            type = ObmTypeTab;
            while (type->name) {
                if (OS_StriCmp(obm->type, type->name) == 0) {
                    obm->store = type->obmt;
                    break;
                }
                type++;
            }
        }
    }
        
    switch (obm->store) {
    case OBMT_VSTORE:
        /*
         * On this platform, the VSTORE just maps to the generic filesystem. 
         */
        /* Drop into generic store */

    case OBMT_GENERIC:
        return FSTORE_Create(&obm->handle.file, obm->name);

    case OBMT_BROWSER:
        return OBRC_FORBIDDEN;
    }

    return OBRC_NOT_FOUND;
}

/*---------------------------------------------------------------------------
 *            OBM_Close()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close the object store item and return the handle to the pool.
 *
 * Return:    Success or failure of create operation.
 *
 */
ObexRespCode OBM_Close(ObStoreMgr **ObmPtr)
{
    ObexRespCode    rcode = OBRC_SUCCESS;

    switch ((*ObmPtr)->store) {
    case OBMT_VSTORE:
        /*
         * On this platform, the VSTORE just maps to the generic filesystem. 
         */
        /* Drop into generic store */

    case OBMT_GENERIC:
        if ((*ObmPtr)->handle.file)
            rcode = FSTORE_Close(&(*ObmPtr)->handle.file);
        break;
    }

    OBM_Free(*ObmPtr);
    *ObmPtr = 0;

    return rcode;
}

/*---------------------------------------------------------------------------
 *            OBM_Delete()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Delete the given object store item. The Object store
 *            handle is then returned to the pool.
 *
 * Return:    OBRC_SUCCESS -   File was deleted.
 *            OBRC_NOT_FOUND - File was not deleted.
 *	     	  BRC_PRECONDITION_FAILED - Folder was not deleted since it is not empty
 */
ObexRespCode OBM_Delete(ObStoreMgr **obmPtr)
{
    ObexRespCode    rcode = OBRC_NOT_FOUND;
    const ObStore  *type;

    Assert (obmPtr && *obmPtr);

    if ((*obmPtr)->store == OBMT_UNSPECIFIED) {
        (*obmPtr)->store = OBMT_GENERIC;

        if ((*obmPtr)->typeLen) {
            /* Dispatch to the proper object store */
            type = ObmTypeTab;
            while (type->name) {
                if (OS_StriCmp((*obmPtr)->type, type->name) == 0) {
                    (*obmPtr)->store = type->obmt;
                    break;
                }
                type++;
            }
        }
    }

    switch ((*obmPtr)->store) {
    case OBMT_VSTORE:
        /* Drop into generic store */
    case OBMT_GENERIC:
        rcode = FSTORE_Delete(&(*obmPtr)->handle.file, (*obmPtr)->name);
        break;
    }
    
    OBM_Free(*obmPtr);
    *obmPtr = 0;

    return rcode;
}

/*---------------------------------------------------------------------------
 *            OBM_Read()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Read from the given object store item.
 *
 * Return:    OBRC_SUCCESS -  Read was successful.
 *            OBRC <other> -  Read failed.
 *
 */
ObexRespCode OBM_Read(void *obs, U8 *buff, U16 len)
{
    ObStoreMgr *obm = (ObStoreMgr *)obs;
    ObexRespCode rcode = OBRC_FORBIDDEN;

    Assert(obm->store != OBMT_UNSPECIFIED);

    switch (obm->store) {
        case OBMT_VSTORE:
            /* Drop into generic store */

        case OBMT_GENERIC:
            rcode = FSTORE_Read(obm->handle.file, buff, len);
            break;
    }
    
    if (obm->amount == 0)
    {
        obm->progressCallback(obm);
    }

    obm->amount += len;

    obm->progressCallback(obm);
    return rcode;
}

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBM_ReadFlex()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Read from the given object store item.
 *
 * Return:    OBRC_SUCCESS -  Read was successful.
 *            OBRC <other> -  Read failed.
 *
 */
ObexRespCode OBM_ReadFlex(void *obs, U8 *buff, U16 *len, BOOL *more)
{
    ObStoreMgr   *obm = (ObStoreMgr *)obs;
    ObexRespCode rcode = OBRC_FORBIDDEN;

    Assert(obm->store != OBMT_UNSPECIFIED);

    switch (obm->store) {
        
        case OBMT_BROWSER:
            rcode = BROWSER_ReadFlex(buff, len, more);
            break;
    }

    obm->amount += *len;

    return rcode;
}
#endif

/*---------------------------------------------------------------------------
 *            OBM_Write()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Write to the given object manager item.
 *
 * Return:    OBRC_SUCCESS -  Write was successful.
 *            OBRC <other> -  Write failed.
 *
 */
ObexRespCode OBM_Write(void *obs, U8 *buff, U16 len)
{
    ObStoreMgr   *obm = (ObStoreMgr *)obs;
    ObexRespCode rcode = OBRC_FORBIDDEN;

    Assert(obm->store != OBMT_UNSPECIFIED);

    switch (obm->store) {
        case OBMT_VSTORE:
            /* Drop into generic store */

        case OBMT_GENERIC:
            rcode = FSTORE_Write(obm->handle.file, buff, len);
            break;
    }

    if (obm->amount == 0)
    {
        obm->progressCallback(obm);
    }

    obm->amount += len;

    obm->progressCallback(obm);
    return rcode;
}

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
ObexRespCode OBM_AppendNameAscii(ObStoreMgr *Obm, const U8 *Name, U16 Len)
{
	U16 currentPathLen;
	const BtlUtf8* currentPath;

	currentPath = PATHMGR_GetCurrentPath();
	currentPathLen = OS_StrLenUtf8(currentPath);

	/* Check if delimiter already present in current path */
	if (currentPath[currentPathLen - 1] == BTHAL_FS_PATH_DELIMITER)
		currentPathLen--;

	if ((currentPathLen + Len + 2) >= BTHAL_FS_MAX_PATH_LENGTH)
		return (OBRC_UNAUTHORIZED);

	OS_StrnCpyUtf8(Obm->name, currentPath, currentPathLen);
	Obm->name[currentPathLen] = BTHAL_FS_PATH_DELIMITER;
	OS_StrnCpyUtf8((Obm->name + currentPathLen + 1), Name, Len);
	Obm->name[(currentPathLen + Len + 1)] = '\0';

	return (OBRC_SUCCESS);
}

/*---------------------------------------------------------------------------
 *            OBM_GetObjectLen()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get the length of an object manager item.
 *
 * Return:    The length of the object in bytes.
 *
 */
U32 OBM_GetObjectLen(void *obs)
{
    ObStoreMgr   *obm = (ObStoreMgr *)obs;

    Assert(obm->store != OBMT_UNSPECIFIED);

    switch (obm->store) {

        case OBMT_BROWSER:
            obm->objectLen = BROWSER_GetObjectLen();
            break;
        
        case OBMT_VSTORE:
            /* Drop into generic store */
        
        case OBMT_GENERIC:
            obm->objectLen = FSTORE_GetObjectLen(obm->handle.file);
            break;
    }

    return obm->objectLen;
}


#endif /*BTL_CONFIG_FTPS ==   BTL_CONFIG_ENABLED*/

