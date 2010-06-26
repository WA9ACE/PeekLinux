/*******************************************************************************\
*                                                                              *
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION           *
*                                                                              *
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE         *
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE        *
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO        *
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT         *
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL          *
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.           *
*                                                                              *
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:   unicode.c
*
*   DESCRIPTION: This file contains unicode specific routines.
*
*   AUTHOR:      Gerrit Slot
*
\*******************************************************************************/


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <btl_defs.h>
#include <osapi.h>
#include <utils.h>
#include "xatypes.h" /* for types such as U8, U16, U32, TRUE, FALSE, etc.. */
#include "btl_unicode.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_UNICODE);

/******************************************************************************** 
 *
 * Constants 
 *
 *******************************************************************************/
static const int halfShift = 10; /* used for shifting by 10 bits */

static const U32 halfBase = 0x0010000UL;
static const U32 halfMask = 0x3FFUL;

/*
 * Index into the table below with the first byte of a UTF-8 sequence to
 * get the number of trailing bytes that are supposed to follow it.
 * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
 * left as-is for anyone who may want to do such conversion, which was
 * allowed in earlier algorithms.
 */
static const char trailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/*
 * Magic values subtracted from a buffer value during UTF8 conversion.
 * This table contains as many values as there might be trailing bytes
 * in a UTF-8 sequence.
 */
static const U32 offsetsFromUTF8[6] = {0x00000000UL, 0x00003080UL, 0x000E2080UL,
                                       0x03C82080UL, 0xFA082080UL, 0x82082080UL};

/*
 * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
 * into the first byte, depending on how many bytes follow.  There are
 * as many entries in this table as there are UTF-8 sequence types.
 * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
 * for *legal* UTF-8 will be 4 or fewer bytes total.
 */
static const BtlUtf8 firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/
typedef enum _ConversionResult
{
	conversionOK, 		/* conversion successful */
	sourceExhausted,	/* partial character in source, but hit end */
	targetExhausted,	/* insuff. room in target for conversion */
	sourceIllegal		  /* source sequence is illegal/malformed */
} ConversionResult;

typedef enum _ConversionFlags
{
	strictConversion = 0,
	lenientConversion
} ConversionFlags;

/******************************************************************************** 
 *
 * Macros
 *
 *******************************************************************************/

/* Write a word native U16 into the target buffer according to the endianity directive:     */
/*   endianity = native: no translation necessary.                                          */
/*   endianity = big-endian: force to big-endian (network order) storage. First byte = MSB. */
/*   endianity = little-endian: force to little-endian storage. First byte = LSB.           */
/**/
#define WRITE_UTF16(endianity, target, nativeWord)            \
  if(endianity==btlNativeEndian) *target=nativeWord;             \
  else if(endianity==btlBigEndian) StoreBE16((U8*)target,nativeWord); \
  else StoreLE16((U8*)target,nativeWord);

/* Read a word according to the endianity directive into the source in native format:       */
/*   endianity = native: no translation necessary.                                          */
/*   endianity = big-endian: force to big-endian (network order) reading. First byte = MSB. */
/*   endianity = little-endian: force to little-endian reading. First byte = LSB.           */
/**/
#define READ_UTF16(endianity, source, nativeWord)            \
  if(endianity==btlNativeEndian) *nativeWord=*source;             \
  else if(endianity==btlBigEndian) *nativeWord=BEtoHost16((U8*)source); \
  else *nativeWord=LEtoHost16((U8*)source);
  
/* Some fundamental constants */
#define UNI_REPLACEMENT_CHAR ((U32) 0x0000FFFD)
#define UNI_MAX_BMP ((U32) 0x0000FFFF)
#define UNI_MAX_UTF16 ((U32) 0x0010FFFF)
#define UNI_MAX_UTF32 ((U32) 0x7FFFFFFF)
#define UNI_MAX_LEGAL_UTF32 (UTF32) 0x0010FFFF

#define UNI_SUR_HIGH_START  ((U32) 0xD800)
#define UNI_SUR_HIGH_END     ((U32) 0xDBFF)
#define UNI_SUR_LOW_START    ((U32) 0xDC00)
#define UNI_SUR_LOW_END      ((U32) 0xDFFF)

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

static ConversionResult ConvertUTF8toUTF16(const BtlUtf8 **sourceStart,
                                           const BtlUtf8 *sourceEnd, 
                                           BtlUtf16 **targetStart,
                                           BtlUtf16 *targetEnd,
                                           ConversionFlags flags,
                                           BtlEndianity endianity);

static ConversionResult ConvertUTF16toUTF8(const BtlUtf16 **sourceStart,
                                           const BtlUtf16 *sourceEnd, 
                                           BtlUtf8 **targetStart,
                                           BtlUtf8 *targetEnd,
                                           ConversionFlags flags,
                                           BtlEndianity endianity);
		
static BOOL isLegalUTF8Sequence(const BtlUtf8 *source,
                                const BtlUtf8 *sourceEnd);

static BOOL isLegalUTF8(const BtlUtf8 *source,
                        int length);

/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

static ConversionResult ConvertUTF16toUTF8 (const BtlUtf16 **sourceStart,
                                            const BtlUtf16 *sourceEnd, 
                                            BtlUtf8 **targetStart,
                                            BtlUtf8 *targetEnd,
                                            ConversionFlags flags,
                                            BtlEndianity endianity)
{
  ConversionResult result = conversionOK;
  const BtlUtf16 *source = *sourceStart;
  BtlUtf8 *target = *targetStart;

  while (source < sourceEnd)
  {
	  U32 ch;
	  U16 ch16;
    unsigned short bytesToWrite = 0;
    const U32 byteMask = 0xBF;
    const U32 byteMark = 0x80; 
    const BtlUtf16* oldSource = source; /* In case we have to back up because of target overflow. */

    /* Read next UTF-16 word, according the defined enianity. */
    READ_UTF16(endianity, source, &ch16)
    ch = (U32) ch16; /* Use 32 bit value for calculations. */
    source++;

	  /* If we have a surrogate pair, convert to U32 first. */
	  if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END)
    {
	    /* If the 16 bits following the high surrogate are in the source buffer... */
	    if (source < sourceEnd)
      {
        U32 ch2;
        
        /* Read high surrogate. */
        READ_UTF16(endianity, source, &ch16)
        ch2 = (U32) ch16; /* Use 32 bit value for calculations. */

		    /* If it's a low surrogate, convert to UTF32. */
		    if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END)
        {
		      ch = ((ch - UNI_SUR_HIGH_START) << halfShift) + (ch2 - UNI_SUR_LOW_START) + halfBase;
		      ++source;
		    }
        else if (flags == strictConversion)
		    { /* it's an unpaired high surrogate */
		      --source; /* return to the illegal value itself */
		      result = sourceIllegal;
		      break;
		    }
	    }
      else
      { /* We don't have the 16 bits following the high surrogate. */
		    --source; /* return to the high surrogate */
		    result = sourceExhausted;
		    break;
	    }
	  } 
    else if (flags == strictConversion)
    {
	    /* UTF-16 surrogate values are illegal in UTF-32 */
	    if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END)
      {
		    --source; /* return to the illegal value itself */
		    result = sourceIllegal;
		    break;
	    }
	  }

	  /* Figure out how many bytes the result will require */
	  if (ch < (U32)0x80) { bytesToWrite = 1; }
	  else if (ch < (U32)0x800) {   bytesToWrite = 2; }
	  else if (ch < (U32)0x10000) {  bytesToWrite = 3; }
	  else if (ch < (U32)0x110000) { bytesToWrite = 4; }
	  else
    {
      bytesToWrite = 3;
      ch = UNI_REPLACEMENT_CHAR;
	  }

    target += bytesToWrite;
    if (target > targetEnd)
    {
      source = oldSource; /* Back up source pointer! */
      target -= bytesToWrite; result = targetExhausted;
      break;
    }

	  switch (bytesToWrite)
    { /* note: everything falls through. */
      case 4: *--target = (BtlUtf8)((ch | byteMark) & byteMask); ch >>= 6;
      case 3: *--target = (BtlUtf8)((ch | byteMark) & byteMask); ch >>= 6;
      case 2: *--target = (BtlUtf8)((ch | byteMark) & byteMask); ch >>= 6;
      case 1: *--target =  (BtlUtf8)(ch | firstByteMark[bytesToWrite]);
    }

    target += bytesToWrite;
  }

  *sourceStart = source;
  *targetStart = target;

  return result;
}

/* --------------------------------------------------------------------- */

/*
 * Utility routine to tell whether a sequence of bytes is legal UTF-8.
 * This must be called with the length pre-determined by the first byte.
 * If not calling this from ConvertUTF8to*, then the length can be set by:
 *  length = trailingBytesForUTF8[*source]+1;
 * and the sequence is illegal right away if there aren't that many bytes
 * available.
 * If presented with a length > 4, this returns FALSE.  The Unicode
 * definition of UTF-8 goes up to 4-byte sequences.
 */
static BOOL isLegalUTF8(const BtlUtf8 *source,
                        int length)
{
  BtlUtf8 a;
  const BtlUtf8 *srcptr = source+length;

  switch (length)
  {
    default: return FALSE;

    /* Everything else falls through when "TRUE"... */
    case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return FALSE;
    case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return FALSE;
    case 2: if ((a = (*--srcptr)) > 0xBF) return FALSE;

    switch (*source)
    {
	    /* no fall-through in this inner switch */
      case 0xE0: if (a < 0xA0) return FALSE; break;
	    case 0xED: if (a > 0x9F) return FALSE; break;
	    case 0xF0: if (a < 0x90) return FALSE; break;
	    case 0xF4: if (a > 0x8F) return FALSE; break;
	    default:  if (a < 0x80) return FALSE;
    }

    case 1: if (*source >= 0x80 && *source < 0xC2) return FALSE;
  }

  if (*source > 0xF4) return FALSE;
  return TRUE;
}

/* --------------------------------------------------------------------- */

/*
 * Exported function to return whether a UTF-8 sequence is legal or not.
 * This is not used here; it's just exported.
 */
#ifdef SKIP 
static BOOL isLegalUTF8Sequence(const BtlUtf8 *source,
                                const BtlUtf8 *sourceEnd)
{
  int length = trailingBytesForUTF8[*source] + 1;

  if (source + length > sourceEnd)
  {
	  return FALSE;
  }

  return isLegalUTF8(source,length);
}
#endif /* SKIP non used function. */

/* --------------------------------------------------------------------- */

/* ConvertUTF8toUTF16 notes:
 * The interface converts a whole buffer to avoid function-call overhead.
 * Constants have been gathered. Loops & conditionals have been removed as
 * much as possible for efficiency, in favor of drop-through switches.
 * (See "Note A" below for equivalent code.)
 * If your compiler supports it, the "isLegalUTF8" call can be turned
 * into an inline function.
 */
/* ---------------------------------------------------------------------
    Note A.
    The fall-through switches in UTF-8 reading code save a
    temp variable, some decrements & conditionals.  The switches
    are equivalent to the following loop:
    
{
  int tmpBytesToRead = extraBytesToRead+1;
  do
  {
    ch += *source++;
    --tmpBytesToRead;
    if (tmpBytesToRead) ch <<= 6;
  } while (tmpBytesToRead > 0);
}

    In UTF-8 writing code, the switches on "bytesToWrite" are
    similarly unrolled loops.
   --------------------------------------------------------------------- */
static ConversionResult ConvertUTF8toUTF16 (const BtlUtf8 **sourceStart,
                                            const BtlUtf8 *sourceEnd, 
                                            BtlUtf16 **targetStart,
                                            BtlUtf16 *targetEnd, 
                                            ConversionFlags flags,
                                            BtlEndianity endianity)
{
  ConversionResult result = conversionOK;
  const BtlUtf8 *source = *sourceStart;
  BtlUtf16 *target = *targetStart;
    
  while (source < sourceEnd)
  {
    U32 ch = 0;
    unsigned short extraBytesToRead = trailingBytesForUTF8[*source];

    if (source + extraBytesToRead >= sourceEnd)
    {
      result = sourceExhausted; break;
    }

    /* Do this check whether lenient or strict */
    if (FALSE == isLegalUTF8(source, extraBytesToRead+1))
    {
      result = sourceIllegal;
      break;
    }

    /*
     * The cases all fall through. See "Note A" below.
     */
    switch (extraBytesToRead)
    {
      case 5: ch += *source++; ch <<= 6; /* remember, illegal UTF-8 */
      case 4: ch += *source++; ch <<= 6; /* remember, illegal UTF-8 */
      case 3: ch += *source++; ch <<= 6;
      case 2: ch += *source++; ch <<= 6;
      case 1: ch += *source++; ch <<= 6;
      case 0: ch += *source++;
    }
    
    ch -= offsetsFromUTF8[extraBytesToRead];

    if (target >= targetEnd)
    {
      source -= (extraBytesToRead+1); /* Back up source pointer! */
      result = targetExhausted; break;
    }

    if (ch <= UNI_MAX_BMP)
    { /* Target is a character <= 0xFFFF */
	    /* UTF-16 surrogate values are illegal in UTF-32 */
      if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END)
      {
        if (flags == strictConversion)
        {
          source -= (extraBytesToRead+1); /* return to the illegal value itself */
          result = sourceIllegal;
          break;
        }
        else
        {
          WRITE_UTF16(endianity,target,(U16)UNI_REPLACEMENT_CHAR)
          target++;
        }
      }
      else
      {
        WRITE_UTF16(endianity,target,(U16)ch) /* normal case */      
        target++;
      }
    }
    else if (ch > UNI_MAX_UTF16)
    {
      if (flags == strictConversion)
      {
        result = sourceIllegal;
        source -= (extraBytesToRead+1); /* return to the start */
        break; /* Bail out; shouldn't continue */
      }
      else
      {
        WRITE_UTF16(endianity,target,(U16)UNI_REPLACEMENT_CHAR)
        target++;
      }
    }
    else
    {
      /* target is a character in range 0xFFFF - 0x10FFFF. */
      if (target + 1 >= targetEnd)
      {
        source -= (extraBytesToRead+1); /* Back up source pointer! */
        result = targetExhausted; break;
      }

      ch -= halfBase;

      WRITE_UTF16(endianity,target,(U16)((ch >> halfShift) + UNI_SUR_HIGH_START))
      target++;

      WRITE_UTF16(endianity,target,(U16)((ch & halfMask) + UNI_SUR_LOW_START))
      target++;
	  }
  }

  *sourceStart = source;
  *targetStart = target;

  return result;
}


/********************************************************************************
 *
 * Exported function definitions
 *
 *******************************************************************************/


U16 BTL_Utf16ToUtf8Endian(BtlUtf8 *tgtText,
                          U16 tgtSize,
                          BtlUtf16 *srcText,
                          U16 srcLen,
                          BtlEndianity endianity)
{
  BtlUtf16 *sourceStart;
  BtlUtf8 *targetStart;
  
  sourceStart = srcText;
  targetStart = tgtText;
	
  (void) ConvertUTF16toUTF8((const BtlUtf16 **)&sourceStart, (BtlUtf16*) &srcText[srcLen], 
          &targetStart, tgtText + tgtSize + 1, strictConversion, endianity);

  /* conversion failed? --> can use normal error code. */
/*  if(res != conversionOK)
  {
    * Possible error codes:                                           *
    *   sourceExhausted:    partial character in source, but hit end  *
    *   targetExhausted:    insuff. room in target for conversion     *
    *   sourceIllegal:      source sequence is illegal/malformed      *
    *   return -1;          ahh.. can use normal error code           *
  }
*/

  /* Return number of bytes filled in the 'tgtText' (including the 0-byte) */
  return (U16) (targetStart - tgtText);
}

/* --------------------------------------------------------------------- */

U16 BTL_Utf8ToUtf16Endian(BtlUtf16 *tgtText, 
                       U16 tgtSize, 
                       const BtlUtf8 *srcText,
                       BtlEndianity endianity)
{
  BtlUtf8 *sourceStart;
  BtlUtf8 *sourceEnd;
  BtlUtf16 *targetStart;
  BtlUtf16 *targetEnd;
  ConversionResult res;

	/* Point to the first byte which is _out_ of the string */
	/*  because we want the null byte to be converted too   */
  sourceStart = (BtlUtf8*)srcText;
  sourceEnd   =  &sourceStart[OS_StrLenUtf8(srcText)+1];

  targetStart = tgtText;
  targetEnd   = targetStart + tgtSize + 1;
	
  res = ConvertUTF8toUTF16((const BtlUtf8 **)&sourceStart, sourceEnd, 
        &targetStart, targetEnd, strictConversion, endianity);

  /* conversion failed? --> return error. */
  if(res != conversionOK)
  { 
    /* Other possible error codes:                                     */
    /*   sourceExhausted:    partial character in source, but hit end  */
    /*   targetExhausted:    insuff. room in target for conversion     */
    /*   sourceIllegal:      source sequence is illegal/malformed      */
    return 1;
  }

  /* Return the number of bytes that are written in the 'tgtText', */
  /*  including the 0-termination of 2 bytes.                      */
  return (U16) (2*(targetStart - tgtText));
}

