/*******************************************************************************\
##                                                                           	*
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
##																				*
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
##																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_ftpc_xmlprs.h
*
*   BRIEF:          This file defines the API of the BTL FTP client XML parser.
*					
*   DESCRIPTION:	General
*
*   				This file defines the API of the BTL FTP client XML parser.
*					The functions are designated to serving the application.
*					Used to parse a folder listing in an XML format received. 
*					from the server.					
*
*   AUTHOR:         Yoni Shavit
*
\*******************************************************************************/

#ifndef __BTL_FTPC_XML_PARSER
#define __BTL_FTPC_XML_PARSER


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <btl_ftpc.h>
#include <btl_common.h>
#include "btl_unicode.h"

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlFtpcParsedEntyType type
 *
 *     Defines a parsed entry type. (field in the BtlFtpcParsedFolderEntry structure)
 */
typedef U8 BtlFtpcParsedEntyType;

#define	BTL_FTPC_ENTRY_TYPE_PARENT_FOLDER	    (0x00)	
#define BTL_FTPC_ENTRY_TYPE_FOLDER				(0x01)
#define BTL_FTPC_ENTRY_TYPE_FILE				(0x02)


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * BtlFtpcParsedEntyAttributes type
 *
 *     A bit mask that represents the supported entry fields. Note: the type field is always supported and 
 *     hence doesn't appear is the bit mask.
 *
 */
typedef U16 BtlFtpcParsedEntyAttributes;

#define	BTL_FTPC_ENTRY_SUPPORTS_NAME     			    (0x01)
#define BTL_FTPC_ENTRY_SUPPORTS_SIZE 					(0x02)
#define BTL_FTPC_ENTRY_SUPPORTS_MODIFIED_TIME			(0x04)
#define BTL_FTPC_ENTRY_SUPPORTS_CREATED_TIME			(0x08)
#define BTL_FTPC_ENTRY_SUPPORTS_ACCESSED_TIME			(0x10)
#define BTL_FTPC_ENTRY_SUPPORTS_USER_PERMISSIONS		(0x20)

/*-------------------------------------------------------------------------------
 * BtlFtpcParsedEntyPerm type
 *
 *     Defines a parsed entry access permissions (read, write, delete)
 */
typedef U8 BtlFtpcParsedEntyPerm;

#define	BTL_FTPC_ENTRY_PERMISSIOMS_READ		    (0x01)	
#define BTL_FTPC_ENTRY_PERMISSIOMS_DELETE 		(0x02)
#define BTL_FTPC_ENTRY_PERMISSIOMS_WRITE		(0x04)

/*-------------------------------------------------------------------------------
 * BtlFtpcParsedEntry structure
 *
 *     Represents a parsed entry of a folder listing pulled from the server.
 *	   The supported fields are given in entryAttributes field, while The only mandatory field is the type field.
 * 	   An unsupported field value is 0.
 */
typedef struct _BtlFtpcParsedEntry
{	
	/* Entry supported attributes bit mask*/
	BtlFtpcParsedEntyAttributes entryAttributes;
	
	/* The entry type: file, folder or parent folder. Note: this field is mandatory */
	BtlFtpcParsedEntyType entryType;

	/* Entry name */
	BtlUtf8 entryName[FTPC_MAX_FILE_NAME_LENGTH + 1];

	/* Entry size in Bytes */
	U32	entrySize;

	/* Entry modified time structure */
	BtlDateAndTimeStruct entryModifiedTime;

	/* Entry created time structure */
	BtlDateAndTimeStruct entryCreatedTime;

	/* Entry accessed time structure */
	BtlDateAndTimeStruct entryAccessedTime;
	
	/* Entry read, write, delete permissions */
	BtlFtpcParsedEntyPerm  entryPermissions;

} BtlFtpcParsedEntry;

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_FTPC_GetParsedFolderListingFirst()
 *
 * Brief:  
 *		Begins the procedure of parsing a given folder listing XML data.
 *
 * Description:
 *		Begins the procedure of parsing a given folder listing XML data, which can be 
 *		provided by BTL_FTPC_PullListFolder(). The function will fill the given BtlFtpcParsedEntry 
 * 		object with the contents of the first entry in the given XML data. 
 * 		Call BTL_FTPC_GetParsedFolderListingNext() in order to receive the rest of the folder listing entries.
 *		
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		folderListing [in] -  The folder listing XML object.(see BtlObject for details). 
 *							 
 *     	parsedEntry [out] - A folder listing entry to be filled by the function.
 *
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_NOT_FOUND - The first entry was not found. Folder listing is empty.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_FTPC_GetParsedFolderListingFirst(const BtlObject *folderListing,
												BtlFtpcParsedEntry *parsedEntry);

/*-------------------------------------------------------------------------------
 * BTL_FTPC_GetParsedFolderListingNext()
 *
 * Brief:  
 *		Continues the procedure of parsing a given folder listing XML data.
 *
 * Description:
 *		Continues the procedure of parsing a given folder listing XML data. 
 *		The function will fill the given BtlFtpcParsedEntry object with the contents of 
 *		last entry that hasn't been read yet.
 * 		Call BTL_FTPC_GetParsedFolderListingFirst() in order to start the parsing procedure.
 * 		Call BTL_FTPC_GetParsedFolderListingNext() once again to continue the procedure.
 *		
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		folderListing [in] -  The folder listing XML object.(see BtlObject for details). 
 *							 
 *     	parsedEntry [out] - A folder listing entry to be filled by the function.
 *
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_NOT_FOUND - The entry was not found. There are no more entries in the folder listing.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_FTPC_GetParsedFolderListingNext(const BtlObject *folderListing,
												BtlFtpcParsedEntry *parsedEntry);


#endif /* __BTL_FTPC_XML_PARSER */
