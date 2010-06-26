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
*   FILE NAME:      btl_ftpc_xmlprs.c
*
*   DESCRIPTION:    This file contains the BTL FTP client XML parser.
*					Used to parse a folder listing in an XML format revceived 
*					from the server..
*
*   AUTHOR:         Yoni Shavit
*
\*******************************************************************************/



#include "btl_config.h"
#if BTL_CONFIG_FTPC ==   BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include "btl_ftpc_xmlprs.h"
#include "btl_defs.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_FTPC);

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* This is the buffer size for the XML data. Change at porting */
#define BTL_FTPC_XML_PARSER_BUFFER_SIZE 5000

#define BTL_FTPC_XML_ELEMENT_FILE "file"
#define BTL_FTPC_XML_ELEMENT_FOLDER "folder"
#define BTL_FTPC_XML_ELEMENT_PARENTFOLDER "parent-folder"
#define BTL_FTPC_XML_ELEMENT_FOLDER_LISTING "folder-listing"

#define BTL_FTPC_XML_ATTRIBUTE_NAME "name"
#define BTL_FTPC_XML_ATTRIBUTE_SIZE  "size"
#define BTL_FTPC_XML_ATTRIBUTE_MODIFIED  "modified"
#define BTL_FTPC_XML_ATTRIBUTE_CREATED  "created"
#define BTL_FTPC_XML_ATTRIBUTE_ACCESSED  "accessed"
#define BTL_FTPC_XML_ATTRIBUTE_USER_PERM  "user-perm"
#define BTL_FTPC_XML_ATTRIBUTE_VERSION "version"

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

typedef struct _BtlFtpcXmlParser
{
	U8 *element;
	U8 *attribute1;
	U8 *attribute2;
	U8 *attribute3;
	U8 *attribute4;
	U8 *attribute5;
	U8 *attribute6;
	BOOL elementFound;
	
	BtlUtf8 *attributevalue1;
	U32 attributevalue1Len;
	U8 *attributevalue2;
	U32 attributevalue2Len;
	U8 *attributevalue3;
	U32 attributevalue3Len;
	U8 *attributevalue4;
	U32 attributevalue4Len;
	U8 *attributevalue5;
	U32 attributevalue5Len;
	U8 *attributevalue6;
	U32 attributevalue6Len;
	BOOL endOfFile;
	
} BtlFtpcXmlParser; 

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/* points to xml parser structure used by ftc */
BtlFtpcXmlParser 	ftpcParser={0};
U16 	 			ftpcBuffPtr;
U16 				ftpcTempBuffPtr;
BOOL 				ftpcDoneParentFolder;
BtlUtf8 			xmlLine[BTL_CONFIG_FTPC_XML_LINE_SIZE];

/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
 
BOOL 		XMLparser(const char *buf, BtlFtpcXmlParser *xmlparse, U16 *dynptr, U16 len);
static U16	checkElement(const BtlUtf8* elementType, const BtlUtf8 * xmlString);
static U16	getAttributeValue(const BtlUtf8* attribute, const BtlUtf8 * xmlString, U16 *xmlattributeValue);
BOOL 		CheckEndXml(const BtlUtf8 * xmlString);
void 		BtlFtpcXmlExtractPermissions(const BtlUtf8* permBuff, BtlFtpcParsedEntyPerm  *entryPermissions);
void			BtlFtpcXmlZeroAttributesLength();
static BtStatus	BtlFtpcXmlSetAttributes(BtlFtpcParsedEntry *parsedEntry);

/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * BTL_FTPC_GetParsedFolderListingFirst()
 */
BtStatus BTL_FTPC_GetParsedFolderListingFirst(const BtlObject *folderListing,
												BtlFtpcParsedEntry *parsedEntry)
{	
	ObStatus	status = BT_STATUS_SUCCESS;
	U32			ftpcMemLength;
	U8			i;
	BOOL		lineFound;

	BTL_FUNC_START("BTL_FTPC_GetParsedFolderListingFirst");

	/* Initialization */
	OS_MemSet(&ftpcParser, 0, sizeof( BtlFtpcXmlParser) );
	ftpcDoneParentFolder = FALSE;
	ftpcBuffPtr = 0;	
	OS_MemSet(parsedEntry, 0, sizeof(BtlFtpcParsedEntry));
	ftpcMemLength = folderListing->location.memLocation.size;
	
	/* looking for a folder listing header*/
	ftpcParser.element = (U8*)BTL_FTPC_XML_ELEMENT_FOLDER_LISTING;
	ftpcParser.attribute1 = 0;
	ftpcParser.attribute2 = 0;
	ftpcParser.attribute3 = 0;
	ftpcParser.attribute4 = 0;
	ftpcParser.attribute5 = 0;
	ftpcParser.attribute6 = 0;
	for (i = 0; i < 15; i++)
	{	
		lineFound = XMLparser(folderListing->location.memLocation.memAddr, &ftpcParser, &ftpcBuffPtr,(U16) ftpcMemLength);
		if (ftpcParser.endOfFile)
		{
			status = BT_STATUS_NOT_FOUND;
			return status;							
		}
		else if (TRUE == lineFound)
			 break;
	}
	
	/* looking for a parent folder entry */
	ftpcTempBuffPtr= ftpcBuffPtr;
	ftpcParser.element = (U8*)BTL_FTPC_XML_ELEMENT_PARENTFOLDER;
	ftpcParser.attribute1 = 0;
	ftpcParser.attribute2 = 0;
	ftpcParser.attribute3 = 0;
	ftpcParser.attribute4 = 0;
	ftpcParser.attribute5 = 0;
	ftpcParser.attribute6 = 0;
	lineFound = XMLparser(folderListing->location.memLocation.memAddr, &ftpcParser, &ftpcBuffPtr,(U16) ftpcMemLength);
	if (ftpcParser.endOfFile)
	{
		status = BT_STATUS_NOT_FOUND;
		return status;							
	}
	else if (TRUE == lineFound)
	{
		ftpcDoneParentFolder = TRUE;
		status = BT_STATUS_SUCCESS;
		parsedEntry->entryType = BTL_FTPC_ENTRY_TYPE_PARENT_FOLDER;
		return status;
	}
	
	/* No parent folder entry was found, keep looking for other entry kinds */
	/* looking for a folder entry */
	ftpcParser.element	  = (U8*)BTL_FTPC_XML_ELEMENT_FOLDER;
	ftpcParser.attribute1 = (U8*)BTL_FTPC_XML_ATTRIBUTE_NAME;
	ftpcParser.attribute2 = (U8*)BTL_FTPC_XML_ATTRIBUTE_SIZE;
	ftpcParser.attribute3 = (U8*)BTL_FTPC_XML_ATTRIBUTE_MODIFIED;
	ftpcParser.attribute4 = (U8*)BTL_FTPC_XML_ATTRIBUTE_CREATED;
	ftpcParser.attribute5 = (U8*)BTL_FTPC_XML_ATTRIBUTE_ACCESSED;
	ftpcParser.attribute6 = (U8*)BTL_FTPC_XML_ATTRIBUTE_USER_PERM;
	ftpcBuffPtr = ftpcTempBuffPtr;
	lineFound = XMLparser(folderListing->location.memLocation.memAddr, &ftpcParser, &ftpcBuffPtr,(U16) ftpcMemLength);
	if (ftpcParser.endOfFile)
	{
		status = BT_STATUS_NOT_FOUND;
		return status;					
	}
	if (TRUE == lineFound)
	{	
		parsedEntry->entryType = BTL_FTPC_ENTRY_TYPE_FOLDER;
		status = BtlFtpcXmlSetAttributes(parsedEntry);
		return status;		
	}
		
	/* No entries was found yet, keep looking for file entry */
	ftpcBuffPtr = ftpcTempBuffPtr;
	ftpcParser.element    = (U8*)BTL_FTPC_XML_ELEMENT_FILE;
	ftpcParser.attribute1 = (U8*)BTL_FTPC_XML_ATTRIBUTE_NAME;
	ftpcParser.attribute2 = (U8*)BTL_FTPC_XML_ATTRIBUTE_SIZE;
	ftpcParser.attribute3 = (U8*)BTL_FTPC_XML_ATTRIBUTE_MODIFIED;
	ftpcParser.attribute4 = (U8*)BTL_FTPC_XML_ATTRIBUTE_CREATED;
	ftpcParser.attribute5 = (U8*)BTL_FTPC_XML_ATTRIBUTE_ACCESSED;
	ftpcParser.attribute6 = (U8*)BTL_FTPC_XML_ATTRIBUTE_USER_PERM;
	lineFound = XMLparser(folderListing->location.memLocation.memAddr, &ftpcParser, &ftpcBuffPtr,(U16) ftpcMemLength);
	if (ftpcParser.endOfFile)
	{
		status = BT_STATUS_NOT_FOUND;
		return status;					
	}
	if (TRUE == lineFound)
	{
		parsedEntry->entryType = BTL_FTPC_ENTRY_TYPE_FILE;
		status = BtlFtpcXmlSetAttributes(parsedEntry);
		return status;
	}

	BTL_FUNC_END();	
	
	/* We shouldn't get here */	
	status = BT_STATUS_FAILED;
	return status;
}


/*-------------------------------------------------------------------------------
 * BTL_FTPC_GetParsedFolderListingNext()
 */
BtStatus BTL_FTPC_GetParsedFolderListingNext(const BtlObject *folderListing,
												BtlFtpcParsedEntry *parsedEntry)
{
	
	ObStatus		status = BT_STATUS_SUCCESS;
	U32			ftpcMemLength;
	BOOL		lineFound;

	BTL_FUNC_START("BTL_FTPC_GetParsedFolderListingNext");
	
	OS_MemSet(parsedEntry, 0, sizeof(BtlFtpcParsedEntry));
	ftpcMemLength = folderListing->location.memLocation.size;
	
	if (ftpcBuffPtr >= ftpcMemLength)
	{
		status = BT_STATUS_NOT_FOUND;
		return status;
	}

	/* Looking for parent folder entry */
	if (FALSE == ftpcDoneParentFolder)
	{
		ftpcTempBuffPtr = ftpcBuffPtr;
		ftpcParser.element = (U8*)BTL_FTPC_XML_ELEMENT_PARENTFOLDER;
		ftpcParser.attribute1 = 0;
		ftpcParser.attribute2 = 0;
		ftpcParser.attribute3 = 0;
		ftpcParser.attribute4 = 0;
		ftpcParser.attribute5 = 0;
		ftpcParser.attribute6 = 0;
		lineFound = XMLparser(folderListing->location.memLocation.memAddr, &ftpcParser, &ftpcBuffPtr,(U16) ftpcMemLength);
		if (ftpcParser.endOfFile)
		{
			status = BT_STATUS_NOT_FOUND;
			return status;							
		}
		else if (TRUE == lineFound)
		{
			ftpcDoneParentFolder = TRUE;
			status = BT_STATUS_SUCCESS;
			parsedEntry->entryType = BTL_FTPC_ENTRY_TYPE_PARENT_FOLDER;
			return status;
		}

		ftpcBuffPtr = ftpcTempBuffPtr;
	}
	else
		ftpcTempBuffPtr = ftpcBuffPtr;

	/* Looking for folder entry */
	ftpcParser.element    = (U8*)BTL_FTPC_XML_ELEMENT_FOLDER;
	ftpcParser.attribute1 = (U8*)BTL_FTPC_XML_ATTRIBUTE_NAME;
	ftpcParser.attribute2 = (U8*)BTL_FTPC_XML_ATTRIBUTE_SIZE;
	ftpcParser.attribute3 = (U8*)BTL_FTPC_XML_ATTRIBUTE_MODIFIED;
	ftpcParser.attribute4 = (U8*)BTL_FTPC_XML_ATTRIBUTE_CREATED;
	ftpcParser.attribute5 = (U8*)BTL_FTPC_XML_ATTRIBUTE_ACCESSED;
	ftpcParser.attribute6 = (U8*)BTL_FTPC_XML_ATTRIBUTE_USER_PERM;
	lineFound = XMLparser(folderListing->location.memLocation.memAddr, &ftpcParser, &ftpcBuffPtr,(U16) ftpcMemLength);
	if (ftpcParser.endOfFile)
	{
		status = BT_STATUS_NOT_FOUND;
		return status;						
	}
	else if (TRUE == lineFound)
	{	
		parsedEntry->entryType = BTL_FTPC_ENTRY_TYPE_FOLDER;
		status = BtlFtpcXmlSetAttributes(parsedEntry);
		return status;		
	}
	
	/* Looking for file entry */
	ftpcBuffPtr = ftpcTempBuffPtr;
	ftpcParser.element    = (U8*)BTL_FTPC_XML_ELEMENT_FILE;
	ftpcParser.attribute1 = (U8*)BTL_FTPC_XML_ATTRIBUTE_NAME;
	ftpcParser.attribute2 = (U8*)BTL_FTPC_XML_ATTRIBUTE_SIZE;
	ftpcParser.attribute3 = (U8*)BTL_FTPC_XML_ATTRIBUTE_MODIFIED;
	ftpcParser.attribute4 = (U8*)BTL_FTPC_XML_ATTRIBUTE_CREATED;
	ftpcParser.attribute5 = (U8*)BTL_FTPC_XML_ATTRIBUTE_ACCESSED;
	ftpcParser.attribute6 = (U8*)BTL_FTPC_XML_ATTRIBUTE_USER_PERM;
	lineFound = XMLparser(folderListing->location.memLocation.memAddr, &ftpcParser, &ftpcBuffPtr,(U16) ftpcMemLength);
	if (ftpcParser.endOfFile)
	{
		status = BT_STATUS_NOT_FOUND;
		return status;						
	}
	else if (TRUE == lineFound)
	{
		parsedEntry->entryType = BTL_FTPC_ENTRY_TYPE_FILE;
		status = BtlFtpcXmlSetAttributes(parsedEntry);
		return status;		
	}

	/* Never supposed to get here */
	status = BT_STATUS_FAILED;

	BTL_FUNC_END();	
	
	return status;
}

/*---------------------------------------------------------------------------
 *            XMLparser
 *---------------------------------------------------------------------------
 *
 * Synopsis: Parse the XML document for element based on key words.
 *
 * Return:   char number, else -1 if error.
 *
 */
BOOL XMLparser(const char *buf, BtlFtpcXmlParser *xmlparse, U16 *dynptr, U16 len)
{

	BtlUtf8	currchar, prevchar;
	U16		bufsize=0;
	U16		bufcount=0;
	U16		i=0;
	U16		tempChar;

	prevchar = 0;
	bufsize = len;
	currchar = buf[*dynptr];
	OS_MemSet(xmlLine,0, BTL_CONFIG_FTPC_XML_LINE_SIZE);
	while(1)
	{
		if (currchar == '<') /* Checking for the opening tag	*/
		{
			while ((prevchar!='>') && (i < BTL_CONFIG_FTPC_XML_LINE_SIZE)) /*	Copying line into string	*/
			{
				i++;
				(*dynptr)++;
				xmlLine[bufcount]= currchar;
				bufcount++;
				prevchar = currchar;
				currchar = buf[*dynptr];
				if (*dynptr>bufsize)
				{
					xmlparse->endOfFile=1;
					return FALSE;
				}
			} /*	Copy completed	*/
			if ( i == BTL_CONFIG_FTPC_XML_LINE_SIZE)
			{
				BTL_LOG_ERROR(("Fail parse XML line length is out of range"));
				return FALSE;
			}
			
			if (TRUE == CheckEndXml(xmlLine))
			{
				xmlparse->endOfFile=1;
				return FALSE;
			}
			/* Zero all attributes lengths' before extracting values */
			BtlFtpcXmlZeroAttributesLength();
			if (checkElement(xmlparse->element, xmlLine))
			{
				if (xmlparse->attribute1!=0)
				{
					xmlparse->attributevalue1Len=getAttributeValue(xmlparse->attribute1, xmlLine, &tempChar );
					xmlparse->attributevalue1 = xmlLine + tempChar;
				}
				if (xmlparse->attribute2!=0)
				{
					xmlparse->attributevalue2Len=getAttributeValue(xmlparse->attribute2, xmlLine, &tempChar);
					xmlparse->attributevalue2 = xmlLine + tempChar;
				}
				if (xmlparse->attribute3!=0)
				{
					xmlparse->attributevalue3Len=getAttributeValue(xmlparse->attribute3, xmlLine, &tempChar);
					xmlparse->attributevalue3 = xmlLine + tempChar;
				}
				if (xmlparse->attribute4!=0)
				{
					xmlparse->attributevalue4Len=getAttributeValue(xmlparse->attribute4, xmlLine, &tempChar);
					xmlparse->attributevalue4 = xmlLine + tempChar;
				}
				if (xmlparse->attribute5!=0)
				{
					xmlparse->attributevalue5Len=getAttributeValue(xmlparse->attribute5, xmlLine, &tempChar);
					xmlparse->attributevalue5 = xmlLine + tempChar;
				}
				if (xmlparse->attribute6!=0)
				{
					xmlparse->attributevalue6Len=getAttributeValue(xmlparse->attribute6, xmlLine, &tempChar);
					xmlparse->attributevalue6 = xmlLine + tempChar;
				}
				xmlparse->elementFound = 1;
				return TRUE;
			}
			OS_MemSet(xmlLine,0,BTL_CONFIG_FTPC_XML_LINE_SIZE);
			while (prevchar!='<') /*	Looking for the next attribute	*/
			{
				(*dynptr)++;
				bufcount++;
				prevchar = currchar;
				currchar = buf[*dynptr];
				if (*dynptr>bufsize)
				{
					xmlparse->endOfFile=1;
					return FALSE;
				}
			}
			(*dynptr)--;		/* Set pointer to the beginning of next attribute */
			return FALSE;	
		}
		else
		{
			prevchar = currchar;
			(*dynptr)++;
			currchar = buf[*dynptr];
			if (*dynptr>bufsize)
			{
				xmlparse->endOfFile=1;
				return FALSE;
			}
		}

		
	}

}

/*---------------------------------------------------------------------------
 *            checkElement
 *---------------------------------------------------------------------------
 *
 * Synopsis: Checks the line from the xml for the element.
 *
 * Return:  True when found, False if not found.
 *
 */
static U16 checkElement(const BtlUtf8* elementType, const BtlUtf8 * xmlString)
{
	U16		stringLen;
	U16		elementLen;
	U16		curptr = 0;
	BOOL	found=0;
	U16		comparelen=0;
	
	stringLen = (U16)OS_StrLenUtf8(xmlString);
	elementLen = (U16)OS_StrLenUtf8(elementType);
	
	while ((curptr<stringLen) && !found)
	{
		if (*elementType!=*xmlString)
		{
			*xmlString++;
			curptr++;
			if (comparelen>0)
			{
				for (; comparelen>0; comparelen--)
					*elementType--;
				comparelen=0;
			}
		}
		else
		{
			*xmlString++;
			*elementType++;
			curptr++;
			comparelen++;
		}
		if (comparelen==elementLen)
		{
				found = 1;
		}

		if (curptr>(elementLen+3))
			break;
	}

	if (found)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*---------------------------------------------------------------------------
 *            getAttributeValue
 *---------------------------------------------------------------------------
 *
 * Synopsis: Gets the pointer for the first character of the attribute value
 *
 * Return:  length of attribute value if found, 0 if not found.
 *
 */
 
static U16 getAttributeValue(const BtlUtf8* attribute, const BtlUtf8 * xmlString, U16 *xmlattributeValue)
{
	U16		stringLen;
	U16		elementLen;
	U16		curptr = 0;
	BOOL	found=0;
	U16		comparelen=0;
	BOOL	insideQuotes=0;

	stringLen = (U16) OS_StrLenUtf8(xmlString);
	elementLen = (U16) OS_StrLenUtf8(attribute);

	
	while ((curptr<stringLen) && !found)
	{
		if (*xmlString=='"')
		{
			if (insideQuotes)
				insideQuotes = 0;/* Encountered 2nd quote	*/
			else
				insideQuotes = 1;/* Encountered 1st quote	*/
		}
		
		if (*attribute!=*xmlString)
		{
			*xmlString++;
			curptr++;
			if (comparelen>0)
			{
				for (; comparelen>0; comparelen--)
					*attribute--;
				comparelen=0;
			}
		}
		else /*	found a match in characters	*/
		{
			if (!insideQuotes)
			{
				*xmlString++;
				*attribute++;
				curptr++;
				comparelen++;
			}
			else
			{
				*xmlString++;
				curptr++;
				if (comparelen>0)
				{
					for (; comparelen>0; comparelen--)
						*attribute--;
					comparelen=0;
				}
			}
		}

		if (comparelen==elementLen)
			found = 1;
	}

	if (!found)
		return 0;
	
	comparelen = 0;
	while (*xmlString!='"') /*	Checking for the 1st " which will be the start of name	*/
	{
		*xmlString++;
		curptr++;
		comparelen++;
		if (comparelen >5)
			return 0;
	}
	/* Found start of attribute value	*/
	*xmlString++;
	curptr++;
	*xmlattributeValue = curptr;
	comparelen = 0;
	while (*xmlString!='"') /*	Checking for the 1st " which will be the end of name	*/
	{
		*xmlString++;
		comparelen++;
		if (comparelen >BTHAL_FS_MAX_FILE_NAME_LENGTH)
			return 0;
	}
	return comparelen;
	
}


BOOL CheckEndXml(const BtlUtf8 * xmlString)
{
	if (0 == OS_StrCmp((char*)xmlString, "</folder-listing>"))
		return TRUE;
	else
		return FALSE;
}

/*-------------------------------------------------------------------------------
 * BtlFtpcXmlExtractPermissions()
 *
 *		Extracts the permissions associate with a file or a folder out of a given buffer. The buffer 
 *		is an unordered sequence of the alphabetic charachters R, D, W (case independent) which represent:
 *		R - Read permission
 *		D - Delete permission
 *		W - Write permission
 *		
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		permBuff [in] - An unordered sequence of the alphabetic charachters R, D, W (case independent)
 *
 *		entryPermissions [out] -  An BtlFtpcParsedEntyPerm structure that holds the entry permissions.
 *							 
 * Returns: Void
 *	 	
 */
void BtlFtpcXmlExtractPermissions(const BtlUtf8* permBuff, BtlFtpcParsedEntyPerm  *entryPermissions)
{	
	U8 i;
	for (i = 0; i < 3; i++)
	{
		switch (permBuff[i])
		{
			case 'R':
			case 'r':
				*entryPermissions |= BTL_FTPC_ENTRY_PERMISSIOMS_READ;
				break;
			case 'D':
			case 'd':
				*entryPermissions |= BTL_FTPC_ENTRY_PERMISSIOMS_DELETE;
				break;
			case 'W':
			case 'w':
				*entryPermissions |= BTL_FTPC_ENTRY_PERMISSIOMS_WRITE;
				break;
			default:
				break;
				
		}

	}
}

/*-------------------------------------------------------------------------------
 * BtlFtpcXmlZeroAttributesLength()
 *
 *		Zeros all the attirbutes lengths' off the global variable ftpcParser. 
 *
 * Type: Synchronous
 * Parameters: Void 
 * Returns: Void
 *	 	
 */
void BtlFtpcXmlZeroAttributesLength()
{
	ftpcParser.attributevalue1Len = 0;
	ftpcParser.attributevalue2Len = 0;
	ftpcParser.attributevalue3Len = 0;
	ftpcParser.attributevalue4Len = 0;
	ftpcParser.attributevalue5Len = 0;
	ftpcParser.attributevalue6Len = 0;
}

/*-------------------------------------------------------------------------------
 * BtlFtpcXmlSetAttributes()
 *
 *	Set the arttributes to FtpcParsedEntry
 *
 * Type: Synchronous
 * Parameters: BtlFtpcParsedEntry *parsedEntry
 * Returns: BtStatus
 *	 	
 */
BtStatus BtlFtpcXmlSetAttributes(BtlFtpcParsedEntry *parsedEntry)
{
	U8			i;
	U8			tempString[50]={0}; /*Result string for xml parsing*/


	OS_MemSet(tempString,0,sizeof(tempString));
	
	/* Copying name */
	if (ftpcParser.attributevalue1Len > 0)
		parsedEntry->entryAttributes |= BTL_FTPC_ENTRY_SUPPORTS_NAME;
	for( i=0; i<ftpcParser.attributevalue1Len;i++)
	{
		parsedEntry->entryName[i] = *ftpcParser.attributevalue1;
		ftpcParser.attributevalue1++;
	}
	parsedEntry->entryName[i] = '\0';

	/* Extract Size */
	if (ftpcParser.attributevalue2Len > 0)
		parsedEntry->entryAttributes |= BTL_FTPC_ENTRY_SUPPORTS_SIZE;
	for( i=0; i<ftpcParser.attributevalue2Len;i++)
	{
		tempString[i] = *ftpcParser.attributevalue2;
		ftpcParser.attributevalue2++;
	}
	if (OS_AtoU32((char*)tempString)>0)
		parsedEntry->entrySize = OS_AtoU32((char*)tempString);
	else
		parsedEntry->entrySize = 0;
	
	/* Extract modified time */
	OS_MemSet(tempString,0,sizeof(tempString));
	for( i=0; i<ftpcParser.attributevalue3Len;i++)
	{
		/* tempString uses here as a time buffer */
		tempString[i] = *ftpcParser.attributevalue3;
		ftpcParser.attributevalue3++;
	}
	/* Before extarcting the time check the length of temp string. Can be only 15 or 16 */
	/* according to spec */
	if ((15 == i) || (16 == i))
	{
		BTL_UTILS_XmlParseAttrDateAndTime(tempString, &parsedEntry->entryModifiedTime);
		parsedEntry->entryAttributes |= BTL_FTPC_ENTRY_SUPPORTS_MODIFIED_TIME;
	}

	/* Extract created time */
	OS_MemSet(tempString,0,sizeof(tempString));
	for( i=0; i<ftpcParser.attributevalue4Len;i++)
	{
		/* tempString uses here as a time buffer */
		tempString[i] = *ftpcParser.attributevalue4;
		ftpcParser.attributevalue4++;
	}
	
	/* Before extarcting the time check the length of temp string. Can be only 15 or 16 */
	/* according to spec */
	if ((15 == i) || (16 == i))
	{
		BTL_UTILS_XmlParseAttrDateAndTime(tempString, &parsedEntry->entryCreatedTime);
		parsedEntry->entryAttributes |= BTL_FTPC_ENTRY_SUPPORTS_CREATED_TIME;
	}

	/* Extract accessed time */
	OS_MemSet(tempString,0,sizeof(tempString));
	for( i=0; i<ftpcParser.attributevalue5Len;i++)
	{
		/* tempString uses here as a time buffer */
		tempString[i] = *ftpcParser.attributevalue5;
		ftpcParser.attributevalue5++;
	}
	
	/* Before extarcting the time check the length of temp string. Can be only 15 or 16 */
	/* according to spec */
	if ((15 == i) || (16 == i))
	{
		BTL_UTILS_XmlParseAttrDateAndTime(tempString, &parsedEntry->entryAccessedTime);
		parsedEntry->entryAttributes |= BTL_FTPC_ENTRY_SUPPORTS_ACCESSED_TIME;
	}
		
	/* Copying permissions */
	if (ftpcParser.attributevalue6Len > 0)
	{
		BtlFtpcXmlExtractPermissions(ftpcParser.attributevalue6, &parsedEntry->entryPermissions);
		parsedEntry->entryAttributes |= BTL_FTPC_ENTRY_SUPPORTS_USER_PERMISSIONS;
	}
	
	return BT_STATUS_SUCCESS;
}


#endif /*BTL_CONFIG_FTPC ==   BTL_CONFIG_ENABLED*/

