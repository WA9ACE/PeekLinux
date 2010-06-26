/*******************************************************************************\
*                                                                               *
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION            *
*                                                                               *
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE          *
*   UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE       *
*   APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO       *
*   BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT        *
*   OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL         *
*   DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.          *
*                                                                               *
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_bip_common.c
*
*   DESCRIPTION:    This file contains functionality that is the same for
*                   the BIP Responder and Initiator.
*
*   AUTHOR:         Gerrit Slot
*
\*******************************************************************************/



#include "btl_config.h"

#if BTL_CONFIG_BIP ==   BTL_CONFIG_ENABLED
/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "osapi.h"
#include "bttypes.h"
#include "debug.h"
#include "bthal_fs.h"

#include "btl_unicode.h"
#include "btl_defs.h"
#include "btl_debug.h"

#include "bip.h"
#include "btl_bip_common.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BIPINT);
/*gesl: TODO: this module is also used by the BIPRSP, so the lgging can be confusing showing BIPINT*/

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/

/********************************************************************************
 *
 * Function implementation
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * btl_bip_FileOpen()
 *
 *    This routine will open a file ('fileName') for reading (create = FALSE) or
 *    writing (create = TRUE).
 *    The 'path' contains the full path name for this 'file'. This 'path' will
 *    also be used as a temp storage for the full name ('path' + 'file').
 *
 *    When 'fileName' is NULL, then the 'patch' does contains the full
 *    path name, including the filename.
 *
 *    Output:
 *    When succeeded, the fileHandle will returned the unique allocated
 *    fileHandle for this file.
 *    The 'bipData' does contain the data administration for the BIP module,
 *    which needs to be initialized for coming read actions.
 *
 *    Return:
 *    - BT_STATUS_SUCCESS: File is openened.
 *    - BT_STATUS_FFS_ERROR: Error detected while opening the file.
 */
BtStatus btl_bip_FileOpen(BOOL create,
								const BtlUtf8 		*path,
                          const BtlUtf8 *fileName,
                          BthalFsFileDesc *fileHandle,
                          BipData *bipData)
{
    BtStatus         status = BT_STATUS_SUCCESS;
    BtFsStatus       fsStatus;
    BthalFsStat      fileStat;
    U16              pathLen;
    BthalFsOpenFlags fileOpenFlags;
	BtlUtf8		tempPath[BTHAL_FS_MAX_PATH_LENGTH + 1];
    

    BTL_FUNC_START("btl_bip_FileOpen");

	BTL_LOG_INFO(("btl_bip_FileOpen: %s %s", ((create == TRUE)? "Creating" : "Opening"), (const char*)fileName));
	
	OS_StrCpyUtf8(tempPath, path);
	
    /* remember original pathLen. */
    pathLen = OS_StrLenUtf8(tempPath);

    /* fileName presnet? --> create complete filename (including path name) */
    if (NULL != fileName)
    {
      OS_StrCatUtf8(tempPath, fileName);
    }
    
    /* Create file? --> define correct flags for creation. */
    if (TRUE == create)
    {    
        fileOpenFlags = BTHAL_FS_O_CREATE | BTHAL_FS_O_WRONLY | BTHAL_FS_O_BINARY;
    }
    /* No file creation! --> define correct flags for reading. */
    else
    {
        fileOpenFlags = BTHAL_FS_O_RDONLY | BTHAL_FS_O_BINARY;
    }
    
    /* Now open this file for reading/writing and store the filehandle. */
    fsStatus = BTHAL_FS_Open(tempPath,fileOpenFlags, fileHandle);

    /* Error detected? --> return error + make a note. */
    if (BT_STATUS_HAL_FS_SUCCESS != fsStatus)
    {
        bipData->ocx = NULL; /* Clear data admin for BIP*/

        /* Return error*/
        BTL_ERR(BT_STATUS_FFS_ERROR, ("Failed Opening %s, Reason: %d", fileName, fsStatus));
    }
    /* Succeeded! --> get file size now. */
    else
    {
        /* Get file size. */
        fsStatus = BTHAL_FS_Stat(tempPath, &fileStat);

        /* Error detected? --> return error + make a note. */
        if (BT_STATUS_HAL_FS_SUCCESS != fsStatus)
        {
            bipData->ocx = NULL; /* Clear data admin for BIP*/
            bipData->totLen = 0; /* clear admin. */

            /* Return error */
            BTL_ERR(BT_STATUS_FFS_ERROR, ("BTHAL_FS_Stat Failed for %s, Reason: %d", fileName, fsStatus));
        }
        /* Everything OK! -->  init data admin for BIP */
        else
        {
            bipData->offset = 0;
            bipData->len    = 0;

            /* Store file size from meta-data */
            bipData->totLen = fileStat.size;

		BTL_LOG_INFO(("btl_bip_FileOpen: Successfully %s File (Size: %d)", ((create == TRUE)? "Creating" : "Opening"), bipData->totLen));
        }
    }

    BTL_FUNC_END();

    return status;
}

/*-------------------------------------------------------------------------------
 * btl_bip_FileClose()
 *
 *    This routine will close a file. The file is identified by a 'fileHandle'.
 *    At functioen exit, this 'fileHandle' will is reset to BTHAL_FS_INVALID_FILE_DESC.
 *
 *    Return:
 *    - BT_STATUS_SUCCESS: File is closed
 *    - BT_STATUS_FFS_ERROR: Error detected while closing the file.
 */
BtStatus btl_bip_FileClose(BthalFsFileDesc *fileHandle)
{
    BtStatus    status = BT_STATUS_SUCCESS;
    BtFsStatus  fsStatus;

    BTL_FUNC_START("btl_bip_FileClose");

    /* fileHandle is valid? --> close it. */
    if (BTHAL_FS_INVALID_FILE_DESC != *fileHandle)
    {
        /* Close this file */
        fsStatus = BTHAL_FS_Close(*fileHandle);

        /* Error detected? --> return it. */
        BTL_VERIFY_ERR(BT_STATUS_HAL_FS_SUCCESS == fsStatus, BT_STATUS_FFS_ERROR,
                        ("Failed closing File, Reason: %d", fsStatus));

        /* Clear filehandle. */
        *fileHandle = BTHAL_FS_INVALID_FILE_DESC; 
    }

    BTL_FUNC_END();

    return status;
}

/*-------------------------------------------------------------------------------
 * btl_bip_FileClose()
 *
 *    This routine will delete a file. The file is identified by a 'fileHandle'.
 *    At functioen exit, this 'fileHandle' will is reset to BTHAL_FS_INVALID_FILE_DESC.
 *
 *    Return:
 *    - BT_STATUS_SUCCESS: File is closed
 *    - BT_STATUS_FFS_ERROR: Error detected while closing the file.
 */
BtStatus btl_bip_FileDelete(BtlUtf8 *fileName)
{
    BtStatus    status = BT_STATUS_SUCCESS;
    BtFsStatus  fsStatus;

    BTL_FUNC_START("btl_bip_FileDelete");

	/* Close this file */
      fsStatus = BTHAL_FS_Remove((U8*)fileName);

        /* Error detected? --> return it. */
        BTL_VERIFY_ERR(BT_STATUS_HAL_FS_SUCCESS == fsStatus, BT_STATUS_FFS_ERROR,
                        ("Failed removing File(%s), Reason: %d", (char*)fileName, fsStatus));

    BTL_FUNC_END();

    return status;
}

/*-------------------------------------------------------------------------------
 * btl_bip_FileRead()
 *
 *    This routine will read a piece of data from a file.
 *    The 'bipData' represents the request from the remote side and does contain
 *    the relevant read parameters:
 *    - 'fileHandle' of a specific file.
 *    - 'length' to be read.
 *
 *    Return:
 *    - BT_STATUS_SUCCESS: Reading is succeeded..
 *    - BT_STATUS_FFS_ERROR: Error detected while reading.
 *    - BT_STATUS_INVALID_PARM: Unknown 'bipOp' detected.in 'bipData'
 */
BtStatus btl_bip_DataRead(BipData *bipData)
{
    BtStatus        status = BT_STATUS_SUCCESS;
    BtlBipObjStore *objStore;
    BtFsStatus      fsStatus;
    U32             numRead;
    U8             *destData = NULL;

    BTL_FUNC_START("btl_bip_FileRead");

    /* Map my own data structure to the received ocx pointer.*/
    objStore = (BtlBipObjStore*) bipData->ocx;

    /* Get correct destination pointer for storing the data read.*/
    switch(bipData->bipOp)
    {
        /* Full image data. */
        case BIPOP_PUT_IMAGE:
        case BIPOP_GET_IMAGE:
        case BIPOP_GET_MONITORING_IMAGE:
            destData = bipData->r.rImg.buff;
/*gesl: QUESTION: BIPOP_GET_MONITORING_IMAGE:is not rThm??*/
            break;
            
        /* Thumbnail format data. */
        case BIPOP_PUT_LINKED_THUMBNAIL:
        case BIPOP_GET_LINKED_THUMBNAIL:
            destData = bipData->r.rThm.buff;
            break;

	case BIPOP_GET_CAPABILITIES:
		destData = bipData->r.rCapa.buff;
		break;
		
	case BIPOP_GET_IMAGE_PROPERTIES:
		destData = bipData->r.rProp.buff;
		break;
		
	case BIPOP_GET_IMAGES_LIST:
		destData = bipData->r.rList.buff;
		break;
		
        /* other? --. return error. */            
        default:
            BTL_ERR(BT_STATUS_INVALID_PARM, ("Unknown bipOp (%d) detected while reading.", bipData->bipOp));

    }

	if (objStore->type == BTL_BIP_OBJ_STORE_TYPEHANDLE)
	{
		BTL_LOG_INFO(("btl_bip_DataRead: Reading %d Bytes", bipData->len));
		
	    /* Now read the requested data. */
	    fsStatus = BTHAL_FS_Read (objStore->location.fileHandle, destData, (int)bipData->len, &numRead);

	    /* Error detected? --> return it + make a note. */
	    if (BT_STATUS_HAL_FS_SUCCESS != fsStatus)
	    {
	        bipData->len = 0; /* clear admin. */

	        /* Return error */
	        BTL_ERR(BT_STATUS_FFS_ERROR, ("Failed reading from file, Reason: %d", fsStatus));
	    }
	    /* Reading was successful! --> store actual number of bytes read */
	    else
	    {
	        bipData->len = numRead;
	    }
	}
	else
	{
		OS_MemCopy(destData, (void*)&((char*)objStore->location.mem)[bipData->offset], bipData->len);
	}
    
    BTL_FUNC_END();

    return status;
}

/*-------------------------------------------------------------------------------
 * btl_bip_FileWrite()
 *
 *    This routine will write a piece of data to a file.
 *    The 'bipData' represents the request from the remote side and does contain
 *    the relevant write parameters:
 *    - 'fileHandle' of a specific file.
 *    - 'length' to be written
 *
 *    Return:
 *    - BT_STATUS_SUCCESS: Write is succeeded
 *    - BT_STATUS_FFS_ERROR: Error detected while writing.
 *    - BT_STATUS_INVALID_PARM: Unknown 'bipOp' detected.in 'bipData'
 */
BtStatus btl_bip_DataWrite(BipData *bipData)
{
    BtStatus        status = BT_STATUS_SUCCESS;
    BtlBipObjStore *objStore;
    U32             numWrite;
    BtFsStatus      fsStatus;
    U8             *srcData = NULL;
	BOOL		isDataAString = TRUE;

    BTL_FUNC_START("btl_bip_DataWrite");

    /* Map my own data structure to the received ocx pointer.*/
    objStore = (BtlBipObjStore*) bipData->ocx;

    /* Get correct source pointer for reading the data to be written.*/
    switch(bipData->bipOp)
    {
        /* Full image data. */
        case BIPOP_PUT_IMAGE:
        case BIPOP_GET_IMAGE:
        case BIPOP_GET_MONITORING_IMAGE:
            srcData = bipData->i.iImg.buff;
			isDataAString = FALSE;
/*gesl: QUESTION: BIPOP_GET_MONITORING_IMAGE:is not rThm??*/
            break;
            
        /* Thumbnail format data. */
        case BIPOP_PUT_LINKED_THUMBNAIL:
        case BIPOP_GET_LINKED_THUMBNAIL:
            srcData = bipData->i.iThm.buff;
			isDataAString = FALSE;
            break;

	case BIPOP_GET_CAPABILITIES:

		srcData = bipData->i.iCapa.buff;
		
	break;

	case BIPOP_GET_IMAGES_LIST:

		srcData = bipData->i.iList.buff;
		
		break;

	case BIPOP_GET_IMAGE_PROPERTIES:

		srcData = bipData->i.iProp.buff;
		
		break;

        /* other? --. return error. */            
        default:
            BTL_ERR(BT_STATUS_INVALID_PARM, ("Unknown bipOp (%d) detected while reading.", bipData->bipOp));

    }

	if (objStore->type == BTL_BIP_OBJ_STORE_TYPEHANDLE)
	{
		BTL_LOG_INFO(("btl_bip_DataWrite: Writing %d bytes to FILE", bipData->len));
		
		/* Now write the received data. */
		fsStatus = BTHAL_FS_Write (objStore->location.fileHandle, srcData, bipData->len, &numWrite);

		/* Error detected? --> return it + make a note. */
		if (BT_STATUS_HAL_FS_SUCCESS != fsStatus)
		{
			bipData->len = 0; /* clear admin. */

			/* Return error */
			BTL_ERR(BT_STATUS_FFS_ERROR, ("Failed writing to file, Reason: %d", fsStatus));
		}
		
		/* Writing was successful! --> store actual number of bytes written*/
		else
		{
			bipData->len = numWrite;
		}
	}
	else
	{
		BTL_LOG_INFO(("btl_bip_DataWrite: Copying %d bytes to location %d of MEMORY", bipData->len, bipData->offset));
		
		OS_MemCopy((void*)&((char*)objStore->location.mem)[bipData->offset], srcData, bipData->len);

		if (isDataAString == TRUE)
		{
			BTL_LOG_INFO(("btl_bip_DataWrite: Writing EOS to location %d", bipData->offset + bipData->len));
			
			/* Make sure that for textual entities, the end of current written section is always a valid string*/
			((char*)objStore->location.mem)[bipData->offset + bipData->len] = 0;
		}
	}

    BTL_FUNC_END();

    return status;
}

BOOL btl_bip_IsHandleValid(const BipImgHandle handle)
{
	U32 	i;
	const char	*handleAsStr = (const char*)handle;

	if (handleAsStr[BTL_BIP_IMAGE_HANDLE_LEN] != '\0')
	{
		return FALSE;
	}
	
	if (OS_StrLen(handleAsStr) != BTL_BIP_IMAGE_HANDLE_LEN)
	{
		return FALSE;
	}

	for (i = 0; i < OS_StrLen(handleAsStr); ++i)
	{
		if ((handleAsStr[i] < '0') || (handleAsStr[i] > '9'))
		{
			return FALSE;
		}
	}

	return TRUE;
}


S8* btl_bip_GetBipEncodingString(BtlBipEncoding bipEncoding, S8* string )
{
/*gesl: TODO: share this function with INT (avoid double code)*/
    switch( bipEncoding )
    {
        case BTL_BIP_ENCODING_JPEG:     OS_StrCpy((char*)string, "JPEG"); break;
        case BTL_BIP_ENCODING_JPEG2000: OS_StrCpy((char*)string, "JPEG2000"); break;
        case BTL_BIP_ENCODING_PNG:      OS_StrCpy((char*)string, "PNG"); break;
        case BTL_BIP_ENCODING_GIF:      OS_StrCpy((char*)string, "GIF"); break;
        case BTL_BIP_ENCODING_BMP:      OS_StrCpy((char*)string, "BMP"); break;
        case BTL_BIP_ENCODING_WBMP:     OS_StrCpy((char*)string, "WBMP"); break;
        default: OS_StrCpy((char*)string, "USR-TIP"); break;
    }
    return string;
}

const char *btl_bip_GetBipOpString(BipOp bipOp)
{
    switch (bipOp) {
	    case BIPOP_GET_CAPABILITIES: 			return "Get Capabilities";
	    case BIPOP_GET_IMAGES_LIST:        		return "Get Images List";
	    case BIPOP_GET_IMAGE_PROPERTIES:  		return "Get Image Properties";
	    case BIPOP_PUT_IMAGE:        			return "Put Image";
	    case BIPOP_PUT_LINKED_THUMBNAIL:        	return "Put Linked Thumbnail";
	    case BIPOP_GET_IMAGE:        			return "Get Image";
	    case BIPOP_GET_LINKED_THUMBNAIL:        	return "Get Linked Thumbnail";
	    case BIPOP_GET_LINKED_ATTACHMENT:      return "Get Linked Attachment";
	    case BIPOP_GET_MONITORING_IMAGE:        return "Get Monitoring Image";
		
	    case BIPOP_PUT_LINKED_ATTACHMENT:
	    case BIPOP_REMOTE_DISPLAY:
	    case BIPOP_DELETE_IMAGE:
	    case BIPOP_START_PRINT:
	    case BIPOP_START_ARCHIVE:
	    case BIPOP_GETSTATUS:
	    default:
	        									return "UNEXPECTED";
    }
}

const char *btl_bip_GetBipEventString(BipEvent bipEvent)
{
    switch( bipEvent )
    {
		case BIPINITIATOR_DATA_REQ: 				return "BIPINITIATOR_DATA_REQ";
		case BIPINITIATOR_DATA_IND: 				return "BIPINITIATOR_DATA_IND";
		case BIPINITIATOR_RESPONSE: 				return "BIPINITIATOR_RESPONSE";
		case BIPINITIATOR_COMPLETE: 				return "BIPINITIATOR_COMPLETE";
		case BIPINITIATOR_FAILED: 				return "BIPINITIATOR_FAILED";
		case BIPINITIATOR_ABORT: 				return "BIPINITIATOR_ABORT";
		case BIPINITIATOR_CONNECTED: 			return "BIPINITIATOR_CONNECTED";
		case BIPINITIATOR_DISCONNECTED: 			return "BIPINITIATOR_DISCONNECTED";
		case BIPINITIATOR_DISABLED: 				return "BIPINITIATOR_DISABLED";
		case BIPINITIATOR_AUTH_CHALLENGE_RCVD: 	return "BIPINITIATOR_AUTH_CHALLENGE_RCVD";
		case BIPINITIATOR_PROGRESS: 				return "BIPINITIATOR_PROGRESS";
		case BIPRESPONDER_DATA_REQ: 			return "BIPRESPONDER_DATA_REQ";
		case BIPRESPONDER_DATA_IND: 				return "BIPRESPONDER_DATA_IND";
		case BIPRESPONDER_REQUEST: 				return "BIPRESPONDER_REQUEST";
		case BIPRESPONDER_COMPLETE: 			return "BIPRESPONDER_COMPLETE";
		case BIPRESPONDER_FAILED: 				return "BIPRESPONDER_FAILED";
		case BIPRESPONDER_ABORT: 				return "BIPRESPONDER_ABORT";
		case BIPRESPONDER_CONTINUE: 			return "BIPRESPONDER_CONTINUE";
		case BIPRESPONDER_CONNECTED: 			return "BIPRESPONDER_CONNECTED";
		case BIPRESPONDER_DISCONNECTED: 		return "BIPRESPONDER_DISCONNECTED";
		case BIPRESPONDER_DISABLED: 				return "BIPRESPONDER_DISABLED";
		case BIPRESPONDER_PROGRESS: 			return "BIPRESPONDER_PROGRESS";
		default: 									return "INVALID";
    }
}

const char *btl_bip_GetBipDataTypeString(BipDataType dataType)
{
	switch (dataType)
	{
		case BIPIND_NO_DATA:				return "_NO_DATA";
		case BIPIND_IMAGE:					return "_IMAGE";
		case BIPIND_THUMBNAIL:				return "_THUMBNAIL";
		case BIPIND_IMG_CAPABILITIES_XML:	return "_IMG_CAPABILITIES_XML";
		case BIPIND_IMG_LISTING_XML:			return "_IMG_LISTING_XML";
		case BIPIND_IMG_PROPERTIES_XML:		return "_IMG_PROPERTIES_XML";
		case BIPIND_HANDLES_DESCRIPTOR:		return "_HANDLES_DESCRIPTOR";
		case BIPIND_IMAGE_DESCRIPTOR:		return "_IMAGE_DESCRIPTOR";
		case BIPIND_IMAGE_HANDLE:			return "_IMAGE_HANDLE";
		default:							return "INVALID";
	}
}

#endif /*BTL_CONFIG_BIP ==   BTL_CONFIG_ENABLED*/

