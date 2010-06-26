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
*   FILE NAME:      bthal_utils.h
*
*   BRIEF:          This file defines the API of the BTHAL UTILS.
*
*   DESCRIPTION:    General
*
*                   The BTHAL UTILS module is a set of utility functions used 
*                   by the BT. These utility functions are platform dependant and 
*                   may be implemented differently on different platforms. 
*                   
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/

#ifndef __BTHAL_UTILS_H
#define __BTHAL_UTILS_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include <bthal_os.h>


 /********************************************************************************
 *
 * Types
 *
 *******************************************************************************/


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTHAL_UTILS_Rand()
 *
 * Brief:  
 *     Called by the stack to generate a random number between
 *     0x0000 and 0xFFFF.
 *
 * Description:
 *     Called by the stack to generate a random number between
 *     0x0000 and 0xFFFF.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *     A 16-bit random number.
 */
BTHAL_U16 BTHAL_UTILS_Rand(void);


/*-------------------------------------------------------------------------------
 * BTHAL_UTILS_MemCopy()
 *
 * Brief:  
 *		Called by the stack to copy memory from one buffer to another.
 *
 * Description:
 *		Called by the stack to copy memory from one buffer to another.
 *     
 *		This function's implementation could use the ANSI memcpy function.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		dest [out] - Destination buffer for data.
 *
 *		source [in] - Source buffer for data. "dest" and "source" must not overlap.
 *
 *		numBytes [in] - Number of bytes to copy from "source" to "dest".
 *
 * Returns:
 *		void.
 * 
 */
void BTHAL_UTILS_MemCopy(BTHAL_U8 *dest, const BTHAL_U8 *source, BTHAL_U32 numBytes);


/*-------------------------------------------------------------------------------
 * BTHAL_UTILS_MemCmp()
 *
 * Brief:  
 *      Compare characters in two buffers.
 *
 * Description:
 *      Called by the stack to compare the bytes in two different buffers.
 *      If the buffers lengths or contents differ, this function returns FALSE.
 *
 *      This function's implementation could use the ANSI memcmp
 *      routine as shown:
 *
 *      return (len1 != len2) ? FALSE : (0 == memcmp(buffer1, buffer2, len2));
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *      buffer1 [in] - First buffer to compare.
 *
 *      len1 [in] - Length of first buffer to compare.
 *
 *      buffer2 [in] - Second buffer to compare.
 *
 *      len2 [in] - Length of second buffer to compare.
 *
 * Returns:
 *      TRUE - The lengths and contents of both buffers match exactly.
 *
 *      FALSE - Either the lengths or the contents of the buffers do not match.
 */
BTHAL_BOOL BTHAL_UTILS_MemCmp(const BTHAL_U8 *buffer1, BTHAL_U16 len1, const BTHAL_U8 *buffer2, BTHAL_U16 len2);


/*-------------------------------------------------------------------------------
 * BTHAL_UTILS_MemSet()
 *
 * Brief: 
 *      Sets buffers to a specified character.
 *
 * Description:
 *     Fills the destination buffer with the specified byte.
 *
 *     This function's implementation could use the ANSI memset
 *     function.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     dest [out] - Buffer to fill.
 *
 *     byte [in] - Byte to fill with.
 *
 *     len [in] - Length of the destination buffer.
 *
 * Returns:
 *		void.
 */
void BTHAL_UTILS_MemSet(BTHAL_U8 *dest, BTHAL_U8 byte, BTHAL_U32 len);


/*-------------------------------------------------------------------------------
 * BTHAL_UTILS_StrCmp()
 *
 * Brief: 
 *	    Compares two strings for equality.
 * 
 * Description: 
 *	    Compares two strings for equality.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     Str1 [in] - String to compare.
 *
 *     Str2 [in] - String to compare.
 *
 * Returns:
 *     Zero - If strings match.
 *     Non-Zero - If strings do not match.
 */
BTHAL_U8 BTHAL_UTILS_StrCmp(const char *Str1, const char *Str2);


/*-------------------------------------------------------------------------------
 * BTHAL_UTILS_StriCmp()
 *
 * Brief: 
 *     Compares two strings for equality regardless of case for the ASCII
 *     characters (value 0x1-0x7F) in the string.
 *
 * Description: 
 *     Compares two strings for equality regardless of case for the ASCII
 *     characters (value 0x1-0x7F) in the string.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Str1 [in] - String to compare.
 *
 *		Str2 [in]- String to compare.
 *
 * Returns:
 *     Zero - If strings match.
 *     Non-Zero - If strings do not match.
 */
BTHAL_U8 BTHAL_UTILS_StriCmp(const char *Str1, const char *Str2);


/*-------------------------------------------------------------------------------
 * BTHAL_UTILS_StrLen()
 *
 * Brief: 
 *	    Calculate the length (number of bytes) in the 0-terminated string.
 *      String can be in UTF-8 format as well.
 *
 * Description: 
 *	    Calculate the length (number of bytes) in the 0-terminated string.
 *      String can be in UTF-8 format as well.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Str [in]- String to count length (UTF-8 format)
 *
 * Returns:
 *     Returns length of string.(number of bytes)
 */
BTHAL_U16 BTHAL_UTILS_StrLen(const char *Str);


/*-------------------------------------------------------------------------------
 * BTHAL_UTILS_StrCpy()
 *
 * Brief: 
 *	    Copy a string (same as ANSI C strcpy)
 *      Strings can be in UTF-8 format as well.
 *
 * Description: 
 *	    Copy a string (same as ANSI C strcpy)
 *      Strings can be in UTF-8 format as well.
 *  
 * 	    The OS_StrCpy function copies StrSource, including the terminating null character, 
 *	    to the location specified by StrDest. No overflow checking is performed when strings 
 *	    are copied or appended. 
 *
 *	    The behavior of OS_StrCpy is undefined if the source and destination strings overlap 
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		StrDest [out]- Destination string.
 *
 *		StrSource [in]- Source string
 *
 * Returns:
 *      Returns StrDest. No return value is reserved to indicate an error.
 */
char* BTHAL_UTILS_StrCpy(char* StrDest, const char *StrSource);


/*-------------------------------------------------------------------------------
 * BTHAL_UTILS_StrnCpy()
 *
 * Brief: 
 *	    Copy characters of one string to another (same as ANSI C strncpy)
 *      Strings can be in UTF-8 format as well.
 *
 * Description: 
 *	    Copy characters of one string to another (same as ANSI C strncpy)
 *      Strings can be in UTF-8 format as well.
 *
 * 		The OS_StrnCpy function copies the initial Count characters of StrSource to StrDest and 
 *		returns StrDest. If Count is less than or equal to the length of StrSource, a null character 
 *		is not appended automatically to the copied string. If Count is greater than the length of 
 *		StrSource, the destination string is padded with null characters up to length Count. 
 *
 *		The behavior of OS_StrnCpy is undefined if the source and destination strings overlap.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		StrDest [out] - Destination string.
 *
 *		StrSource [in] - Source string
 *
 *		Count - Number of bytes to be copied
 *
 * Returns:
 *     Returns strDest. No return value is reserved to indicate an error.
 */
char* BTHAL_UTILS_StrnCpy(char* StrDest, const char *StrSource, BTHAL_U32 Count);


/*-------------------------------------------------------------------------------
 * BTHAL_UTILS_AtoU32()
 *
 * Brief: 
 *    Convert strings to double  (same as ANSI C atoi)
 *    String can be in UTF-8 format as well, because UTF-8 and ASCII are the same digits.
 *
 * Description: 
 *    Convert strings to double  (same as ANSI C atoi)
 *    String can be in UTF-8 format as well, because UTF-8 and ASCII are the same digits.
 *
 * 	  The BTHAL_UTILS_AtoU32 function converts a character string to an integer value.
 *    The function do not recognize decimal points or exponents.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     string [in] - String to be converted which has the following form:
 *
 *    [whitespace][sign][digits]
 *
 *	  Where whitespace consists of space and/or tab characters, which are ignored; 
 *	  sign is either plus (+) or minus (�); and digits are one or more decimal digits. 
 *
 * Returns:
 *	  A U32 value produced by interpreting the input characters as a number.
 *    The return value is 0 if the input cannot be converted to a value of that type. 
 *	  The return value is undefined in case of overflow.
 */
BTHAL_U32 BTHAL_UTILS_AtoU32(const char *string);


/*-------------------------------------------------------------------------------
 * BTHAL_UTILS_StrCat()
 *
 * Brief: 
 *		Append a string (same as ANSI C strcat)
 *      Strings can be in UTF-8 format as well.
 *
 * Description: 
 *		Append a string (same as ANSI C strcat)
 *      Strings can be in UTF-8 format as well.
 *
 * 		The OS_StrrChr function finds the last occurrence of c (converted to char) in string. 
 *		The search includes the terminating null character.
 *		The OS_StrCat function appends strSource to strDest and terminates the resulting string 
 *		with a null character. The initial character of strSource overwrites the terminating null 
 *		character of strDest. No overflow checking is performed when strings are copied or 
 *		appended. The behavior of OS_StrCat is undefined if the source and destination strings 
 *		overlap
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		strDest [in] - Null-terminated destination string.
 *
 *		strSource [in] - Null-terminated source string
 *
 * Returns:
 *		Returns the destination string (strDest). No return value is reserved to indicate an error.
 */
char *BTHAL_UTILS_StrCat(char *strDest, const char *strSource);


/*-------------------------------------------------------------------------------
 * BTHAL_UTILS_StrrChr()
 *
 * Brief: 
 *      Scan a string for the last occurrence of a character (same as ANSI C strrchr).
 *
 * Description:
 *      Scan a string for the last occurrence of a character (same as ANSI C strrchr).
 *      The string is in UTF-8 format, and so this fucntion will only work for
 *      finding an ASCII character in the range of 0x01 to 0x7F
 *
 * 		The BTHAL_UTILS_StrrChr function finds the last occurrence of c (converted to char) in string. 
 *		The search includes the terminating null character.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Str [in] - tNull-terminated string to search
 *
 *      c [in] - Character to be located.
 *
 * Returns:
 *	
 *		Returns a pointer to the last occurrence of c in Str, or NULL if c is not found.
 */
char *BTHAL_UTILS_StrrChr(const char *Str, BTHAL_S32 c);


/*---------------------------------------------------------------------------
 * BTHAL_UTILS_Assert()
 *
 * Brief: 
 *     Called by the stack to indicate that an assertion failed. 
 *
 * Description: 
 *     Called by the stack to indicate that an assertion failed. BTHAL_UTILS_Assert
 *     should display the failed expression and the file and line number
 *     where the expression occurred.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     expression [in] - A string containing the failed expression.
 *
 *     file [in] - A string containing the file in which the expression occurred.
 *
 *     line [in] - The line number that tested the expression.
 *
 * Returns:
 *		void.
 */
void BTHAL_UTILS_Assert(const char *expression, const char *file, BTHAL_U16 line);



#endif /* __BTHAL_UTILS_H */


