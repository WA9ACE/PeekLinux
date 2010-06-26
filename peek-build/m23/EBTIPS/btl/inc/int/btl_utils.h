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
*   FILE NAME:      btl_utils.h
*
*   BRIEF:          This file defines the API of the BTHAL UTILS.
*
*   AUTHOR:         Ronen Levy
*
\*******************************************************************************/
#ifndef __BTL_UTILS_H
#define __BTL_UTILS_H

#include "bttypes.h"
#include "utils.h"


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/* 
 * Max number of bytes (characters, excluding 0-termination) in the 
 * string in BtlDateAndTime.                                 
 */
#define	BTL_DATE_AND_TIME_STRING_LEN	 ((U8) 16)

/* 
 * Max number of bytes (characters, excluding 0-termination) in the 
 * string in BtllFilePermission.                                 
 */
#define	BTL_PERMISSION_STRING_LEN	    ((U8) 3)


/*-------------------------------------------------------------------------------
 * BtlDateAndTime type
 *
 *    Represents the date and time as a 0-terminated string according the
 *    OBEX ISO time header format.
 *    The format is YYYYMMDDTHHMMSSZ, where the 'T' in the middle
 *		is the sperator between the date and the time sections, and the Z in the end is
 *		not mandatory and its existance indicates that the time is refered to the UTC
 *		time zone (Greenwich)
 */
typedef U8  BtlDateAndTimeString[BTL_DATE_AND_TIME_STRING_LEN+1]; /* Include 0-termination space. */


/*-------------------------------------------------------------------------------
 * BtlPermissionString perm
 *
 *    Represents the Permissions Attributes used in File and Folder Elements
 *    according the OBEX ISO time header format.
 *    The format is RDW, where :
 *
 *    'R' - The READ permission applies to all object types. It indicates that
 *    an attempt to GET the named object should successfully retrieve its
 *
 *    'D' - The DELETE permission applies to file types. It indicates that the
 *    file may be removed by sending a PUT-DELETE command.
 *    contents.
 * 
 *    'W' - The WRITE permission applies to all object types. It indicates
 *    that an attempt to modify the contents of the file by PUT’ing to the
 *    file should succeed. For folder objects it indicates that attempts to
 *    create a folder or other object within that folder should succeed.
 */
typedef U8  BtlPermissionString[BTL_PERMISSION_STRING_LEN+1]; /* Include 0-termination space. */

                                                                  
/*-------------------------------------------------------------------------------
 * BtlDateAndTime structure
 *
 *     Represents the date and time as a structure
 */
typedef struct _BtlDateAndTimeStruct
{
    U16         year;    /* YYYY: e.g  2007 */
    U16         month;   /* MM: [1..12]     */
    U16         day;     /* DD: [1..31]     */
    U16         hour;    /* HH: [0..23]     */
    U16         minute;  /* MM: [0..59]     */
    U16         second;  /* SS: [0..59]     */
    BOOL        utcTime; /* TRUE = UTC time zone (universal time = Greenwich time) */
                          /* FALSE = local time zone                                */
} BtlDateAndTimeStruct;


/*-------------------------------------------------------------------------------
 * BtlUtilsListComparisonFunc type definition
 *
 *	A prototype of a comparison function that compares 2 list entries 
 *
 *	The function is used in the BTL_UTILS_FindMatchingListEntry function 
 *	to find a matching entry from the entries in the list
 *
 *	It allows the caller to specify a matching policy that suits his needs.
 *
 *	The function should return:
 *		TRUE - the entries match
 *		FALSE - the entries do not match
*/
typedef BOOL (*BtlUtilsListComparisonFunc)(const ListEntry *entryToMatch, const ListEntry* checkedEntry);

/*-------------------------------------------------------------------------------
 * BTL_UTILS_ListFindMatchingRequest()
 *
 *		Checks if the specified entry is on this list.
 *
 *		The comparison is according to the matching policy defined by comparisonFunc
 *		(see the documentation of BtlBmgRequestComparisonFunc for details)		
 *
 * Parameters:
 *
 *		listHead [in] - List head
 *
 *		entryToMatch [in] - entry to search for
 *
 *		comparisonFunc [in] - comparison function that defines the matching policy
 *
 *		matchingEntry [out] - The matching entry. 0 if none  was found
 *
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */
BtStatus BTL_UTILS_FindMatchingListEntry(
				ListEntry			 		*listHead, 
				const ListEntry				*entryToMatch, 
				BtlUtilsListComparisonFunc	comparisonFunc,
				ListEntry					**matchingEntry);


/*---------------------------------------------------------------------------
 * BTL_UTILS_LockedBdaddrNtoa()
 *
 *     Converts a BD_ADDR structure into an ASCII-formatted device address.
 *
 *	This a NON-Thread-Safe version of the bdaddr_ntoa function. It allows
 *	the function to be called as part of a statement, such as a debug printing statement
 *
 * Parameters:
 *     addr - Binary device address to convert.
 *
 * Returns:
 *     char * - A static internal string that contains the ASCII version
 */
char    *BTL_UTILS_LockedBdaddrNtoa(const BD_ADDR *addr);


/*-------------------------------------------------------------------------------
 * BTL_UTILS_XmlParseAttrDateAndTime()
 *
 *      Parse the 'dateAndTimeString' attribute and extract the individual
 *      fields from it into 'dateAndTimeStruct'.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *		  dateAndTimeString [in] - Date and time in 0-terminated string format
 *          The format is "YYYYMMDDTHHMMSSZ".
 *          (see BtlDateAndTime decsription for details)
 *
 *      dateAndTimeStruct [out] - Parsed date and time information..
 *
 * Returns:
 *      void.
 *
 */
void BTL_UTILS_XmlParseAttrDateAndTime(BtlDateAndTimeString dateAndTimeString,
                                     BtlDateAndTimeStruct *dateAndTimeStruct);


/*-------------------------------------------------------------------------------
 * BTL_UTILS_XmlBuildAttrDateAndTime()
 *
 *      Build a 'dateAndTimeString' attribute from the individual fields in
 *      'dateAndTimeStruct', according the OBEX definition.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      dateAndTimeStruct [in] - Date and time information to be converted.
 *          NULL = don't care
 *
 *		  dateAndTimeString [out] - Date and time in 0-terminated string format
 *          The format is "YYYYMMDDTHHMMSSZ".
 *          (see BtlDateAndTime description for details)
 *          Don't care (dateAndTimeStruct == NULL) will be represented with "*"
 *
 * Returns:
 *      void
 *
 */
void BTL_UTILS_XmlBuildAttrDateAndTime(BtlDateAndTimeStruct *dateAndTimeStruct,
                                     BtlDateAndTimeString dateAndTimeString);



/*-------------------------------------------------------------------------------
 * BTL_UTILS_XmlBuildPermissions()
 *
 *      Build a 'permString' attribute from the individual fields in
 *      'perm', according the OBEX definition.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      perm [in] - bit mask for permission attributes.
 *      (see BthalFsPermission description for details)
 *
 *		permString [out] - Permissions in 0-terminated string format
 *          The format is "RWD".
 *
 * Returns:
 *      void
 *
 */
void BTL_UTILS_XmlBuildPermissions(BTHAL_U16 perm, BtlPermissionString permString);

/*-------------------------------------------------------------------------------
 * BTL_UTILS_FS_StrCmpUTf8()
 *
 *    Compares two strings for equality.
 *    according to the bthal macro compere case sensitive or regardless of case.  
 *
 * Parameters:
 *     Str1 - String to compare.
 *     Str2 - String to compare.
 *
 * Returns:
 *     Zero - If strings match.
 *     Non-Zero - If strings do not match.
 */
U8 BTL_UTILS_FS_StrCmpUTf8(const BtlUtf8 *Str1, const BtlUtf8 *Str2);

#if (BTHAL_FS_CASE_SENSITIVE == TRUE)
	#define BTL_UTILS_FS_StrCmpUTf8(Str1, Str2) OS_StrCmpUtf8((char*)(Str1), (char*)(Str2))
#else
	#define BTL_UTILS_FS_StrCmpUTf8(Str1, Str2) OS_StriCmpUtf8((char*)(Str1), (char*)(Str2))
#endif

#endif	/* __BTL_UTILS_H */

