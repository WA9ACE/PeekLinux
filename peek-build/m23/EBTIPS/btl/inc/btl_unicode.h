/*******************************************************************************\
##                                                                             *
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION          *
##                                                                             *
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE        *
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE      *
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO      *
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT       *
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL        *
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC          *
##                                                                             *
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_unicode.h
*
*   BRIEF:          This file defines all definitions and prototypes related to.
*                   the unicode support in the BTIPS API.
*
*   DESCRIPTION:    General
*
*                   The readable strings that are being exchanged via the BTL
*                   API are all formatted according the UTF-8 format.
*                   To highlight this, a specific type is used for this purpose:
*                   BtlUtf8.
*
*                   A big advantage of the UTF-8 encoding is that it is 100%
*                   compatible with ASCII strings with characters in the range
*                   of 0x00 to 0x7F.
*
*                   When you make use of other character encodings then ASCII or
*                   UTF-8 in your application then a conversion is needed
*                   from and to UTF-8.
*                   A popular encoding scheme for Asian countries is UTF-16.
*
*                   This module does provide conversion routines for UTF-16 to
*                   UTF-8 and the other way around:
*
*                   -	BTL_Utf16ToUtf8: convert from UTF-16 to UTF-8
*                   -	BTL_Utf8ToUtf16: convert from UTF-8 to UTF-16
*
*                   UTF-16 encoding does store characters in 2-byte entities and
*                   is therefore endian sensitive. A processor can order its
*                   bytes in memory in 2 different ways:
*                   -	big endian:
*                     A word is stored as MSB,LSB (increasing memory address)
*                     Also called network order.
*                   -	little endian:
*                     A word is stored as LSB,MSB (increasing memory address)
*
*                   The above mentioned conversion routines are endian independent.
*                   It will convert to the native endianity (big- or little-endian)
*                   of the local processor automatically.
*                   This means that the UTF-16 strings in these conversion routines
*                   are stored in native endian format.
*                   When it should be stored in a specific endian format,
*                   the other 2 conversion routines can be used:
*
*                   -	BTL_Utf16ToUtf8Endian: convert from UTF-16 to UTF-8
*                   -	BTL_Utf8ToUtf16Endian: convert from UTF-8 to UTF-16
*
*                   These 2 routines do convert to a specific endian format
*                   for UTF-16.
*
*                   A detailed description of the usage of these 4 routines is
*                   found at the prototype of each function in this file.
*                   
*   AUTHOR:         Gerrit Slot
*
\*******************************************************************************/

#ifndef __BTL_UNICODE_H
#define __BTL_UNICODE_H

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "xatypes.h" /* for types such as U8, U16, S8, S16,... */

/********************************************************************************
 *
 * Macros
 *
 *******************************************************************************/


/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

typedef U8  BtlUtf8;  /* UTF-8 type in the BTIPS interface  */
typedef U16 BtlUtf16; /* UTF-16 type in the BTIPS interface.*/

/* Three types of endianity domains: big, little or native (big or little)*/
typedef enum _BtlEndianity
{
	btlBigEndian,    /* network order */
	btlLittleEndian,
	btlNativeEndian  /* local endianity, being big or little. */
} BtlEndianity;

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
 * BTL_Utf16ToUtf8Endian()
 *
 * Brief:  
 *     Converts UTF-16 formatted text to UTF-8 format.
 *
 * Description:
 *     Converts UTF-16 text to UTF-8 format. The endianity of the source text
 *     can be defined.
 *
 *     For use in the Application, the endianity of the source is always native
 *     (local endian format). For simplicity (hiding the endianity parameter) in
 *     these cases, another function prototype (macro) is defined to hide this
 *     parameter. See BTL_Utf16ToUtf8
 *
 * Type:
 *		 Synchronous
 *
 * Parameters:
 *     tgtText [out] - pointer to the target buffer that will be filled with the UTF-8 data
 *
 *     tgtSize [in] - size of the target buffer in quantities of bytes
 *
 *     srcText [in] - pointer to the UTF16 source text. The UTF-16 is formatted according
 *         the native endian format (endianity of the local system, either big or little).
 *
 *     srcLen [in] - num of words (U16 entities) that are needed to be converted from the source,
 *         including the 0-termination (U16 word).
 *
 *     endianity [in] - Endianity of the 'srcText'.
 *         The word (2 bytes) can be written in the following formats:
 *         - btlBigEndian: first byte = MSB (network order)
 *         - btlLittleEndian: first byte  = LSB
 *         - btlNativeEndian: the endian format of the local system (big or little)
 *
 * Returns:
 *     Number of bytes filled in the 'tgtText' (including the 0-byte).
 */
U16 BTL_Utf16ToUtf8Endian(BtlUtf8 *tgtText,
                          U16 tgtSize,
                          BtlUtf16 *srcText,
                          U16 srcLen,
                          BtlEndianity endianity);

/*-------------------------------------------------------------------------------
 * BTL_Utf8ToUtf16Endian()
 *
 * Brief:
 *     Converts UTF-8 formatted text to UTF-16 format.
 *
 * Description:
 *     Converts UTF-8 text to UTF-16 format. The endianity of the target
 *     can be defined.
 *
 *     For use in the Application, the endianity of the target is always native
 *     (local endian format). For simplicity (hiding the endianity parameter) in
 *     these cases, another function prototype (macro) is defined to hide this
 *     parameter. See BTL_Utf8ToUtf16
 *
 * Type:
 *		 Synchronous
 *
 * Parameters:
 *     tgtText [out] - pointer to the target buffer that will be filled with
 *         the UTF-16 data
 *
 *     tgtSize [in] - size of the target buffer in quantities of words.
 *         (U16 entities)
 *
 *     srcText [in] - pointer to the UTF-8 source text, assumes the text
 *         is 0-terminated
 *
 *     endianity [in] - Endianity of the 'tgtText'.
 *         The word (2 bytes) can be written in the following formats:
 *         - btlBigEndian: first byte = MSB (network order)
 *         - btlLittleEndian: first byte  = LSB
 *         - btlNativeEndian: the endian format of the local system (big or little)
 *
 * Returns:
 *     How many bytes were used in the target buffer, including the word used
 *     for 0 termination.
 *     This number should always be even. If it is odd, it is an error.
 */
U16 BTL_Utf8ToUtf16Endian(BtlUtf16 *tgtText, 
                          U16 tgtSize, 
                          const BtlUtf8*srcText,
                          BtlEndianity endianity);

/*-------------------------------------------------------------------------------
 * BTL_Utf8ToUtf16()
 *
 * Brief:
 *     Converts UTF-8 formatted text to UTF-16 format (native)
 *
 * Description:
 *     Converts UTF-8 text to UTF-16 format (native format)
 *     It is just a macro around the BTL_Utf8ToUtf16Endian, hiding the
 *     endianity parameter. This function is typically use by the APP developer
 *     that needs conversions from UTF-8 to UTF-16 in its native format.
 *
 * Type:
 *		 Synchronous
 *
 * Parameters:
 *     tgtText [out] - pointer to the target buffer that will be filled with
 *         the UTF-16 data (in native endian format).
 *
 *     tgtSize [in] - size of the target buffer in quantities of words.
 *         (U16 entities)
 *
 *     srcText [in] - pointer to the UTF-8 source text, assumes the text
 *         is 0-terminated
 *
 * Returns:
 *     How many bytes were used in the target buffer, including the word used
 *     for 0 termination.
 *     This number should always be even. If it is odd, it is an error.
 */
#define BTL_Utf8ToUtf16(tgtText,tgtSize,srcText) BTL_Utf8ToUtf16Endian(tgtText,tgtSize,srcText,btlNativeEndian)

/*-------------------------------------------------------------------------------
 * BTL_Utf16ToUtf8()
 *
 * Brief:
 *     Converts UTF-16 formatted text (native) to UTF-8 format
 *
 * Description:
 *     Converts UTF-16 text (native format) to UTF-8
 *     It is just a macro around the BTL_Utf16ToUtf8Endian, hiding the
 *     endianity parameter. This function is typically use by the APP developer
 *     that needs conversions from UTF-16 in its native format to UTF-8.
 *
 * Type:
 *		 Synchronous
 *
 * Parameters:
 *     tgtText [out] - pointer to the target buffer that will be filled with the UTF-8 data
 *
 *     tgtSize [in] - size of the target buffer in quantities of bytes
 *
 *     srcText [in] - pointer to the UTF16 source text. The UTF-16 is formatted according
 *         the native endian format (endianity of the local system, either big or little).
 *
 *     srcLen [in] - num of words (U16 entities) that are needed to be converted from the source,
 *         including the 0-termination (U16 word).
 *
 * Returns:
 *     Number of bytes filled in the 'tgtText' (including the 0-byte).
 */
#define BTL_Utf16ToUtf8(tgtText,tgtSize,srcText,srcLen) BTL_Utf16ToUtf8Endian(tgtText,tgtSize,srcText,srcLen,btlNativeEndian)

#endif /* __BTL_UNICODE_H */
